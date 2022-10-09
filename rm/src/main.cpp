#include "config.h"
#include "constants.h"

#include <iostream> // Just for testing purposes

#include "proclistener.h"
#include "cgroup/cgroupcontrol.h"
#include "workloadmanager.h"
#include "resourcepolicies.h"
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <unistd.h>

static void testWorkloadManager(int pid);

int main(int argc, char *argv[]) {
	auto &c = konro::Config::get(CONFIG_PATH);

	std::cout << c.read<std::string>("test", "name") << std::endl;
	std::cout << c.read<int>("test", "number") << std::endl;
	
    if (argc >= 2) {
        testWorkloadManager(atoi(argv[1]));
    }

    std::cout << "main exiting\n";
	return 0;
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

static void testWorkloadManager(int pid)
{
    std::cout << "WorkloadManager test starting" << std::endl;

    trapCtrlC();

    pc::CGroupControl cgc;
    ResourcePolicies rp;
    wm::WorkloadManager workloadManager(cgc, rp, pid);

    rp.start();
    procListener.attach(&workloadManager);
    procListener();
}
