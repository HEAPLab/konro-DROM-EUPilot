#include "config.h"
#include "constants.h"

#include <iostream> // Just for testing purposes

#include "proclistener.h"
#include "workloadmanager.h"
#include <cstdlib>

static void testWorkloadManager(int pid);

int main(int argc, char *argv[]) {
	auto &c = konro::Config::get(CONFIG_PATH);

	std::cout << c.read<std::string>("test", "name") << std::endl;
	std::cout << c.read<int>("test", "number") << std::endl;
	
    if (argc >= 2) {
        testWorkloadManager(atoi(argv[1]));
    }
	return 0;
}

static void testWorkloadManager(int pid)
{
    std::cout << "WorkloadManager test starting" << std::endl;

    wm::WorkloadManager workloadManager(pid);
    wm::ProcListener procListener;

    procListener.attach(&workloadManager);
    procListener();
}
