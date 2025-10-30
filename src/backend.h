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

struct SystemInfo {
   
    double ramUsagePercentage = 0.0;
    unsigned long long ramUsedBytes = 0;
    unsigned long long ramTotalBytes = 0;
    
    double cpuLoadPercentage = 0.0;

    std::vector<ProcessInfo> processes;
};

class SystemMonitor {
public:
    SystemMonitor();
    ~SystemMonitor();

    void start();
    void stop();
    SystemInfo getLatestInfo();
    void killProcess(unsigned long pid);

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


    struct ProcessCpuTimes {
        ULARGE_INTEGER kernelTime;
        ULARGE_INTEGER userTime;
    };

    std::map<unsigned long, ProcessCpuTimes> m_processCpuCache;
};

#endif