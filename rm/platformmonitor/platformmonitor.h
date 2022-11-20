#ifndef PLATFORMMONITOR_H
#define PLATFORMMONITOR_H

#include "resourcepolicies.h"

#include <log4cpp/Category.hh>
#include <thread>
#include <atomic>
#include <memory>

/*!
 * \class periodically samples information about the platform status.
 * This information is then encapsulated in a MonitorEvent and pushed to
 * the thread safe queue.
 * PlatformMonitor runs in a dedicated thread.
 */
class PlatformMonitor {
    struct PlatformMonitorImpl;
    log4cpp::Category &cat_;
    int monitorPeriod_;
    std::unique_ptr<PlatformMonitorImpl> pimpl_;
    ResourcePolicies &resourcePolicies_;
    std::thread pmThread_;
    std::atomic_bool stop_;
    void run();
public:
    PlatformMonitor(ResourcePolicies &rp, int monitorPeriod);
    ~PlatformMonitor();
    void start();
    void stop();
};

#endif  // #ifndef PLATFORMMONITOR_H


