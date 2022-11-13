#include "config.h"
#include "constants.h"

#include "dir.h"
#include "proclistener.h"
#include "cgroup/cgroupcontrol.h"
#include "workloadmanager.h"
#include "resourcepolicies.h"
#include "platformdescription.h"
#include "platformmonitor.h"
#include "threadname.h"
#include <log4cpp/Category.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/SimpleLayout.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Priority.hh>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <unistd.h>

static void testWorkloadManager(int pid, std::string policy, int timerSeconds);
static void testPlatformDescription();
static std::string configFilePath();

int main(int argc, char *argv[])
{
    rmcommon::setThreadName("MAIN");

    // Log4CPP
    log4cpp::Appender *appender1 = new log4cpp::OstreamAppender("console", &std::cout);
    log4cpp::Appender *appender2 = new log4cpp::FileAppender("logfile", "konro.log");
    log4cpp::PatternLayout *layout = new log4cpp::PatternLayout();
    layout->setConversionPattern("%d [%p] %m%n");
    appender1->setLayout(layout);
    appender2->setLayout(layout);
    log4cpp::Category &root = log4cpp::Category::getRoot();
    root.setPriority(log4cpp::Priority::DEBUG);
    root.addAppender(appender1);
    root.addAppender(appender2);

    root.info("MAIN starting");

    std::string policy;
    // For the ResourcePolicies internal timer
    int timerSeconds = 30;       // 0 means "no timer"

    std::string konroConfigFile = configFilePath();

    if (rmcommon::Dir::file_exists(konroConfigFile.c_str())) {
        root.info("MAIN Konro configuration file is %s", konroConfigFile.c_str());
        auto &config = konro::Config::get(konroConfigFile);
        try {
            policy = config.read<std::string>("policy", "policy");
            root.info("MAIN policy = %s", policy.c_str());
            timerSeconds = config.read<int>("resourcepolicies", "timerseconds");
            root.info("MAIN timer seconds = %d", timerSeconds);
        } catch (std::logic_error &e) {
            root.error("MAIN could not parse Konro configuration %s", konroConfigFile.c_str());
        }
    } else {
        root.error("MAIN Konro configuration %s not found", konroConfigFile.c_str());
    }

#if 1
    testPlatformDescription();
#endif

    if (argc >= 2) {
        int pidToMonitor = atoi(argv[1]);
        testWorkloadManager(pidToMonitor, policy, timerSeconds);
    }

    root.info("MAIN exiting");
    return 0;
}

std::string configFilePath()
{
    std::string home = rmcommon::Dir::home();
    if (home.empty())
        return CONFIG_PATH;
    else
        return rmcommon::make_path(home, CONFIG_PATH);
}

wm::ProcListener procListener;

static void ctrlCHandler(int s)
{
    puts("Ctrl-C");
    log4cpp::Category::getRoot().info("MAIN Ctrl-C: stopping konro");
    procListener.stop();
}


static void trapCtrlC()
{
#if 0
    struct sigaction sig_action;

    memset(&sig_action,0, sizeof(sig_action));
    sig_action.sa_handler = ctrlCHandler;
    sigemptyset(&sig_action.sa_mask);
    sig_action.sa_flags = 0;
    sig_action.sa_restorer = nullptr;

    sigaction(SIGINT, &sig_action, nullptr);
#else
    signal(SIGINT, ctrlCHandler);
#endif
}

static void testWorkloadManager(int pid, std::string policyName, int timerSeconds)
{    
    log4cpp::Category::getRoot().info("MAIN WorkloadManager test starting");

    trapCtrlC();

    ResourcePolicies::Policy policy = ResourcePolicies::getPolicyByName(policyName);

    pc::CGroupControl cgc;
    PlatformDescription pd;
    ResourcePolicies rp(pd, policy, timerSeconds);
    wm::WorkloadManager workloadManager(cgc, rp, pid);
    PlatformMonitor pm(rp);

    log4cpp::Category::getRoot().info("MAIN starting ResourcePolicies thread");
    rp.start();

    log4cpp::Category::getRoot().info("MAIN starting PlatformMonitor thread");
    pm.start();

    log4cpp::Category::getRoot().info("MAIN starting ProcListener thread");
    procListener.attach(&workloadManager);
    procListener();
}

static void testPlatformDescription()
{
    using namespace std;

    PlatformDescription pd;

//    cout << "PLATFORM DESCRIPTION\n";
//    cout << "PROC.UNITS: " << pd.getNumProcessingUnits() << endl;
//    cout << "TOTAL RAM : " << pd.getTotalRam()  << " KB" << endl;
//    cout << "TOTAL SWAP: " << pd.getTotalSwap() << " KB" << endl;
    pd.logTopology();
}
