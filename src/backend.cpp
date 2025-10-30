#include "backend.h"
#include <chrono>   
#include <string>
#include <vector>
#include <algorithm> 
#include <map>

#include <Pdh.h>    
#include <psapi.h>  
#pragma comment(lib, "pdh.lib") 

void SystemMonitor::internal_GetSystemInfo(SystemInfo& info) {

    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memInfo)) {
        info.ramTotalBytes = memInfo.ullTotalPhys;
        info.ramUsedBytes = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
        info.ramUsagePercentage = memInfo.dwMemoryLoad;
    }
    else {
        info.ramTotalBytes = 0;
        info.ramUsedBytes = 0;
        info.ramUsagePercentage = 0.0;
    }

    FILETIME idleTime, kernelTime, userTime;
    ULARGE_INTEGER currentIdle, currentKernel, currentUser;
    ULONGLONG totalSystem = 0;

    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        currentIdle.QuadPart = idleTime.dwLowDateTime | ((ULONGLONG)idleTime.dwHighDateTime << 32);
        currentKernel.QuadPart = kernelTime.dwLowDateTime | ((ULONGLONG)kernelTime.dwHighDateTime << 32);
        currentUser.QuadPart = userTime.dwLowDateTime | ((ULONGLONG)userTime.dwHighDateTime << 32);

        if (m_lastCpuKernel.QuadPart != 0) {
            ULONGLONG totalKernel = currentKernel.QuadPart - m_lastCpuKernel.QuadPart;
            ULONGLONG totalUser = currentUser.QuadPart - m_lastCpuUser.QuadPart;
            ULONGLONG totalIdle = currentIdle.QuadPart - m_lastCpuIdle.QuadPart;

            totalSystem = totalKernel + totalUser;
            ULONGLONG totalUsed = totalSystem - totalIdle;

            if (totalSystem > 0) {
                info.cpuLoadPercentage = (double)(totalUsed * 100.0) / totalSystem;
            }
            else {
                info.cpuLoadPercentage = 0.0;
            }
        }
        else {
            info.cpuLoadPercentage = 0.0;
        }

        m_lastCpuIdle = currentIdle;
        m_lastCpuKernel = currentKernel;
        m_lastCpuUser = currentUser;
    }
    else {
        info.cpuLoadPercentage = 0.0;
    }

    std::map<unsigned long, ProcessCpuTimes> newProcessCpuCache;

    info.processes.clear();
    DWORD aProcesses[1024], cbNeeded;
    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
        return;
    }
    DWORD cProcesses = cbNeeded / sizeof(DWORD);

    for (DWORD i = 0; i < cProcesses; i++) {
        if (aProcesses[i] != 0) {
            ProcessInfo procInfo;
            procInfo.pid = aProcesses[i];

            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ |
                PROCESS_QUERY_LIMITED_INFORMATION,
                FALSE, aProcesses[i]);

            if (hProcess != NULL) {
               
                PROCESS_MEMORY_COUNTERS pmc;
                if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
                    procInfo.memoryUsedBytes = pmc.WorkingSetSize;
                }

                TCHAR szProcessName[MAX_PATH] = TEXT("<desconhecido>");
                if (GetModuleBaseName(hProcess, NULL, szProcessName, sizeof(szProcessName) / sizeof(TCHAR))) {
#ifdef UNICODE
                    std::wstring wstr(szProcessName);
                    std::string str(wstr.begin(), wstr.end());
                    procInfo.name = str;
#else
                    procInfo.name = szProcessName;
#endif
                }

                FILETIME creationTime, exitTime, kernelTimeFile, userTimeFile;
                if (GetProcessTimes(hProcess, &creationTime, &exitTime, &kernelTimeFile, &userTimeFile)) {
                    ULARGE_INTEGER procKernel, procUser;
                    procKernel.QuadPart = kernelTimeFile.dwLowDateTime | ((ULONGLONG)kernelTimeFile.dwHighDateTime << 32);
                    procUser.QuadPart = userTimeFile.dwLowDateTime | ((ULONGLONG)userTimeFile.dwHighDateTime << 32);

                    auto it = m_processCpuCache.find(procInfo.pid);
                    if (it != m_processCpuCache.end() && totalSystem > 0) {
                        ULONGLONG procKernelDelta = procKernel.QuadPart - it->second.kernelTime.QuadPart;
                        ULONGLONG procUserDelta = procUser.QuadPart - it->second.userTime.QuadPart;
                        ULONGLONG totalProcDelta = procKernelDelta + procUserDelta;

                        procInfo.cpuUsagePercentage = (double)(totalProcDelta * 100.0) / totalSystem;
                    }
                    else {
                        procInfo.cpuUsagePercentage = 0.0;
                    }

                    newProcessCpuCache[procInfo.pid] = { procKernel, procUser };
                }

                CloseHandle(hProcess);
            }
            else {
                procInfo.name = "<acesso negado>";
            }
            info.processes.push_back(procInfo);
        }
    }

    m_processCpuCache.swap(newProcessCpuCache);
    std::sort(info.processes.begin(), info.processes.end(), [](const ProcessInfo& a, const ProcessInfo& b) {
        return a.memoryUsedBytes > b.memoryUsedBytes;
        });
}

SystemMonitor::SystemMonitor() :
    m_running(false),
    m_info{},
    m_lastCpuIdle{ 0 },
    m_lastCpuKernel{ 0 },
    m_lastCpuUser{ 0 }
{
    FILETIME idleTime, kernelTime, userTime;
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        m_lastCpuIdle.QuadPart = idleTime.dwLowDateTime | ((ULONGLONG)idleTime.dwHighDateTime << 32);
        m_lastCpuKernel.QuadPart = kernelTime.dwLowDateTime | ((ULONGLONG)kernelTime.dwHighDateTime << 32);
        m_lastCpuUser.QuadPart = userTime.dwLowDateTime | ((ULONGLONG)userTime.dwHighDateTime << 32);
    }
}

SystemMonitor::~SystemMonitor() {
    stop();
}

void SystemMonitor::start() {
    m_running = true;
    m_collectionThread = std::thread(&SystemMonitor::collectionLoop, this);
}

void SystemMonitor::stop() {
    m_running = false;
    if (m_collectionThread.joinable()) {
        m_collectionThread.join();
    }
}

void SystemMonitor::collectionLoop() {
    while (m_running) {
        SystemInfo localInfo;
        internal_GetSystemInfo(localInfo);

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_info = localInfo;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

SystemInfo SystemMonitor::getLatestInfo() {
    SystemInfo infoCopy;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        infoCopy = m_info;
    }
    return infoCopy;
}

void SystemMonitor::killProcess(unsigned long pid) {
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess == NULL) {
        return;
    }
    TerminateProcess(hProcess, 1);
    CloseHandle(hProcess);
}