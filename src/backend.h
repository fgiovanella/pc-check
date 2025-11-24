#ifndef BACKEND_H
#define BACKEND_H

#include <mutex>
#include <thread>
#include <atomic>
#include <string>   
#include <vector>   
#include <map>      

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h> 

struct ProcessInfo {
    unsigned long pid = 0;
    std::string name;
    unsigned long long memoryUsedBytes = 0;
    double cpuUsagePercentage = 0.0;
};

struct ExtraProcessInfo {
    unsigned long threadCount = 0;
    unsigned long long ioReadBytes = 0;
    unsigned long long ioWriteBytes = 0;
    unsigned long handleCount = 0;
    std::string priorityClass;
};

struct ThreadInfo {
    unsigned long tid = 0;
    double cpuUsagePercentage = 0.0;
};

struct SystemInfo {
    double ramUsagePercentage = 0.0;
    unsigned long long ramUsedBytes = 0;
    unsigned long long ramTotalBytes = 0;
    double cpuLoadPercentage = 0.0;
    std::vector<ProcessInfo> processes;
    std::vector<ThreadInfo> focusedProcessThreads;
};

class SystemMonitor {
public:
    SystemMonitor();
    ~SystemMonitor();

    void start();
    void stop();
    void getLatestInfo(SystemInfo& outInfo);
    void killProcess(unsigned long pid);
    ExtraProcessInfo getExtraProcessInfo(unsigned long pid);
    void setFocusedProcess(unsigned long pid);

private:
    void collectionLoop();
    void internal_GetSystemInfo(SystemInfo& info);

    SystemInfo m_info;
    std::mutex m_mutex;
    std::thread m_collectionThread;
    std::atomic<bool> m_running;

    ULARGE_INTEGER m_lastCpuIdle;
    ULARGE_INTEGER m_lastCpuKernel;
    ULARGE_INTEGER m_lastCpuUser;
    ULONGLONG m_lastTotalSystemTime = 0;

    struct ProcessCpuTimes {
        ULARGE_INTEGER kernelTime;
        ULARGE_INTEGER userTime;
    };
    std::map<unsigned long, ProcessCpuTimes> m_processCpuCache;

    struct ThreadCpuTimes {
        ULARGE_INTEGER kernelTime;
        ULARGE_INTEGER userTime;
    };
    std::map<unsigned long, ThreadCpuTimes> m_threadCpuCache;
    std::atomic<unsigned long> m_focusedPid{ 0 };
};

#endif