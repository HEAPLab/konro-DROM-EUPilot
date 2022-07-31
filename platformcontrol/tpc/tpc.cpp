#include <iostream>
#include <cstdlib>
#include "cgroupcontrol.h"
#include "cgrouputil.h"
#include "pcexception.h"

using namespace std;

static void checkAppDir(int pid)
{
    string realDir = pc::util::findCgroupPath(static_cast<pid_t>(pid));
    string wantedDir = pc::util::getCgroupAppBaseDir(static_cast<pid_t>(pid));
    if (realDir != wantedDir) {
        cerr << "checkAppDir: invalid app base dir\n";
        exit(EXIT_FAILURE);
    }
    cout << pid << " base directory is " << realDir << endl;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        cerr << "Missing parameter: PID\n";
        exit(EXIT_FAILURE);
    }
    int pid = atoi(argv[1]);

    pc::CGroupControl cgc;
    pc::App app(pid, pc::App::STANDALONE);

    try {
        cgc.addApplication(app);
        checkAppDir(pid);
    } catch (pc::PcException &e) {
        cerr << "PcException: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }

}
