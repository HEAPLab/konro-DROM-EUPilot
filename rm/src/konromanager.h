#ifndef KONROMANAGER_H
#define KONROMANAGER_H

#include <string>
#include <memory>
#include <log4cpp/Category.hh>

class KonroManager {
    struct KonroManagerImpl;
    std::unique_ptr<KonroManagerImpl> pimpl_;
    log4cpp::Category &cat_;

    std::string cfgPolicyName_;
    int cfgTimerSeconds_;       // 0 means "no timer"
    int cfgMonitorPeriod_;
    std::string cfgCpuModuleNames_;
    std::string cfgBatteryModuleNames_;
    std::string httpListenHost_;
    int httpListenPort_;

    std::string configFilePath();
    void setupLogging();
    void loadConfiguration();
public:
    KonroManager();
    ~KonroManager();

    /*!
     * Configures and starts the ProcListener in the main thread
     * and all the other Konro application threads
     *
     * \param pidToMonitor
     */
    void run(long pidToMonitor);

    /*!
     * Stops the ProcListener and all the threads
     */
    void stop();

    void testPlatformDescription();
};

#endif // KONROMANAGER_H
