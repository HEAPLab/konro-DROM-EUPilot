#ifndef PLATFORMMONITOR_H
#define PLATFORMMONITOR_H

#include "eventbus.h"
#include "basethread.h"
#include <thread>
#include <atomic>
#include <memory>
#include <string>
#include <log4cpp/Category.hh>

/*!
 * \class periodically samples information about the platform status.
 * This information is then encapsulated in a MonitorEvent and pushed to
 * the thread safe queue.
 * PlatformMonitor runs in a dedicated thread.
 */
class PlatformMonitor : public rmcommon::BaseThread {
    struct PlatformMonitorImpl;
    log4cpp::Category &cat_;
    int monitorPeriod_;
    std::unique_ptr<PlatformMonitorImpl> pimpl_;
    rmcommon::EventBus &bus_;

    virtual void run() override;
public:
    PlatformMonitor(rmcommon::EventBus &eventBus, int monitorPeriod);
    ~PlatformMonitor();

    /*!
     * \brief setCpuModuleNames
     * \param names Comma separated list of module names
     */
    void setCpuModuleNames(const std::string &names);

    /*!
     * \brief setBatteryModuleNames
     * \param names Comma separated list of module names
     */
    void setBatteryModuleNames(const std::string &names);
};

#endif  // #ifndef PLATFORMMONITOR_H


