#include "config.h"
#include "constants.h"

#include "dir.h"
#include "proclistener.h"
#include "cgroup/cgroupcontrol.h"
#include "workloadmanager.h"
#include "resourcepolicies.h"
#include "platformdescription.h"
#include "platformmonitor.h"
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
    std::string policy;
    // For the ResourcePolicies internal timer
    int timerSeconds = 30;       // 0 means "no timer"
#if 1
    std::string konroConfigFile = configFilePath();

    if (rmcommon::Dir::file_exists(konroConfigFile.c_str())) {
        std::cout << "Konro configuration file is " << konroConfigFile << std::endl;
        auto &config = konro::Config::get(konroConfigFile);
        try {
            std::cout << config.read<std::string>("test", "name") << std::endl;
            std::cout << config.read<int>("test", "number") << std::endl;
            policy = config.read<std::string>("policy", "policy");
            std::cout << "Policy: " << policy << std::endl;
            timerSeconds = config.read<int>("resourcepolicies", "timerseconds");
            std::cout << "timerseconds: " << timerSeconds << std::endl;
        } catch (std::logic_error &e) {
            std::cout << "Could not parse INI file " << CONFIG_PATH << std::endl;
        }
    } else {
        std::cout << "Konro configuration file " << konroConfigFile << " not found" << std::endl;
    }
#endif
    testPlatformDescription();

    if (argc >= 2) {
        int pidToMonitor = atoi(argv[1]);
        testWorkloadManager(pidToMonitor, policy, timerSeconds);
    }

    std::cout << "main exiting\n";
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
    std::cout << "WorkloadManager test starting" << std::endl;

    trapCtrlC();

    ResourcePolicies::Policy policy = ResourcePolicies::getPolicyByName(policyName);

    pc::CGroupControl cgc;
    PlatformDescription pd;
    ResourcePolicies rp(pd, policy);
    wm::WorkloadManager workloadManager(cgc, rp, pid);
    PlatformMonitor pm(rp);

    rp.start();
    pm.start();
    procListener.attach(&workloadManager);
    procListener();
}

static void testPlatformDescription()
{
    using namespace std;

    PlatformDescription pd;

    cout << "PLATFORM DESCRIPTION\n";
    cout << "CORES     : " << pd.getNumCores() << endl;
    cout << "TOTAL RAM : " << pd.getTotalRam()  << " KB" << endl;
    cout << "TOTAL SWAP: " << pd.getTotalSwap() << " KB" << endl;
}
