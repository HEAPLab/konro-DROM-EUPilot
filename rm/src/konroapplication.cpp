#include "config.h"
#include "konroapplication.h"
#include "dir.h"
#include "makepath.h"
#include "constants.h"
#include "proclistener.h"
#include "resourcepolicies.h"
#include "workloadmanager.h"
#include "platformmonitor.h"
#include "proclistener.h"
#include "konrohttp.h"
#include "simpleeventbus.h"
#include <unistd.h>


KonroApplication::KonroApplication() :
    cat_(log4cpp::Category::getRoot()),
    procListener_(nullptr),
    workloadManager_(nullptr)
{
    // set some default values

    cfgCpuModuleNames_ = "coretemp,k10temp,k8temp,cputemp";
    cfgBatteryModuleNames_ = "BAT";
    cfgTimerSeconds_ = 30;
    cfgMonitorPeriod_ = 20;

    setupLogging();
    loadConfiguration();
}

std::string KonroApplication::configFilePath()
{
    std::string home = rmcommon::Dir::home();
    if (home.empty())
        return CONFIG_PATH;
    else
        return rmcommon::make_path(home, CONFIG_PATH);
}

void KonroApplication::setupLogging()
{
    // Log4CPP configuration

    log4cpp::Appender *appender1 = new log4cpp::OstreamAppender("console", &std::cout);
    log4cpp::Appender *appender2 = new log4cpp::FileAppender("logfile", "konro.log");
    log4cpp::PatternLayout *layout = new log4cpp::PatternLayout();
    layout->setConversionPattern("%d [%p] %m%n");
    appender1->setLayout(layout);
    appender2->setLayout(layout);

    cat_.setPriority(log4cpp::Priority::DEBUG);
    cat_.addAppender(appender1);
    cat_.addAppender(appender2);

    cat_.info("KONRO starting");
}


void KonroApplication::loadConfiguration()
{
    std::string konroConfigFile = configFilePath();
    cat_.info("MAIN Konro configuration file is %s", konroConfigFile.c_str());

    if (!rmcommon::Dir::file_exists(konroConfigFile.c_str())) {
        cat_.error("MAIN Konro configuration %s not found", konroConfigFile.c_str());
        return;
    }

    const konro::Config &config = konro::Config::get(konroConfigFile);

    try {
        cfgPolicyName_ = config.read<std::string>("policy", "policy");
        cat_.info("MAIN policy = %s", cfgPolicyName_.c_str());
    } catch (std::logic_error &e) {
        cat_.info("MAIN could not read policy from Konro configuration %s", konroConfigFile.c_str());
    }

    try {
        cfgTimerSeconds_ = config.read<int>("resourcepolicies", "timerseconds");
        cat_.info("MAIN timer seconds = %d", cfgTimerSeconds_);
    } catch (std::logic_error &e) {
        cat_.info("MAIN could not read timerseconds from Konro configuration %s", konroConfigFile.c_str());
    }

    try {
        cfgMonitorPeriod_ = config.read<int>("platformmonitor", "monitorperiod");
        cat_.info("MAIN monitor period = %d", cfgMonitorPeriod_);
    } catch (std::logic_error &e) {
        cat_.info("MAIN could not read monitorperiod from Konro configuration %s", konroConfigFile.c_str());
    }

    try {
        cfgCpuModuleNames_ = config.read<std::string>("platformmonitor", "kernelcpumodulenames");
        cat_.info("MAIN cpuModuleNames = %s", cfgCpuModuleNames_.c_str());
    } catch (std::logic_error &e) {
        cat_.info("MAIN could not read kernelcpumodulenames from Konro configuration %s", konroConfigFile.c_str());
    }

    try {
        cfgBatteryModuleNames_ = config.read<std::string>("platformmonitor", "kernelbatterymodulenames");
        cat_.info("MAIN batteryModuleNames = %s", cfgBatteryModuleNames_.c_str());
    } catch (std::logic_error &e) {
        cat_.info("MAIN could not read kernelbatterymodulenames from Konro configuration %s", konroConfigFile.c_str());
    }
}

void KonroApplication::run(long pidToMonitor)
{
    pid_t pid = static_cast<pid_t>(pidToMonitor);

    //trapCtrlC();

    rp::ResourcePolicies::Policy policy = rp::ResourcePolicies::getPolicyByName(cfgPolicyName_);

    rmcommon::EventBus eventBus;
    pc::CGroupControl cgc;
    PlatformDescription pd;
    http::KonroHttp http(eventBus);
    rp::ResourcePolicies resourcePolicies(eventBus, pd, policy, cfgTimerSeconds_);
    wm::WorkloadManager workloadManager(eventBus, cgc, pid);
    wm::ProcListener procListener(eventBus);
    PlatformMonitor pm(eventBus, cfgMonitorPeriod_);

    procListener_ = &procListener;
    http_ = &http;
    workloadManager_ = &workloadManager;
    resourcePolicies_ = &resourcePolicies;

    pd.logTopology();

    pm.setCpuModuleNames(cfgCpuModuleNames_);
    pm.setBatteryModuleNames(cfgBatteryModuleNames_);

    // Note on threads:
    //
    // 1. ProcListener runs in the current (main) thread
    // 2. WorkloadManager runs in a separate thread
    // 3. ResourcePolicies runs in a separate thread
    // 4. PlatformMonitor runs in a separate thread
    // 5. KonroHttp runs in a separate thread

    cat_.info("MAIN starting WorkloadManager thread");
    workloadManager.start();

    cat_.info("MAIN starting ResourcePolicies thread");
    resourcePolicies.start();

    cat_.info("MAIN starting PlatformMonitor thread");
    pm.start();

    cat_.info("MAIN starting ProcListener in the main thread");

    cat_.info("MAIN starting HTTP thread");
    http.start();

    procListener();
}

void KonroApplication::stop()
{
    http_->stop();
    procListener_->stop();
    workloadManager_->stop();

}

void KonroApplication::testPlatformDescription()
{
    PlatformDescription pd;
    pd.logTopology();
}
