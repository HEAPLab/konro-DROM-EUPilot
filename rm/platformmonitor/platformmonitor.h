#ifndef PLATFORMMONITOR_H
#define PLATFORMMONITOR_H

#include "resourcepolicies.h"
#include <thread>
#include <atomic>
#include <memory>

class PlatformMonitor {
    struct PlatformMonitorImpl;
    std::unique_ptr<PlatformMonitorImpl> pimpl_;
    ResourcePolicies &resourcePolicies_;
    std::thread pmThread_;
    std::atomic_bool stop_;
    void run();
public:
    PlatformMonitor(ResourcePolicies &rp);
    ~PlatformMonitor();
    void start();
    void stop();
};

#endif  // #ifndef PLATFORMMONITOR_H


