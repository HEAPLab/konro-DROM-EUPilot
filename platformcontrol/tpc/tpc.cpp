#include <iostream>
#include <map>
#include <cstdlib>
#include "cgroupcontrol.h"
#include "cpucontrol.h"
#include "iocontrol.h"
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

static void setCpuMax(pc::App app, int n)
{
    //pc::CpuControl().setValue(pc::CpuControl::MAX, n, app);
    pc::CpuControl().setCpuMax(n, app);
    cout << "Cpu max: requested " << n << " read " << pc::CpuControl().getCpuMax(app) << endl;
}

static void setCpuMax(pc::App app)
{
    pc::CpuControl().setCpuMax(100, app);
    cout << "Cpu max: requested " << 100 << " read " << pc::CpuControl().getCpuMax(app) << endl;
}

static void getCpuStat(pc::App app)
{
    map<string, unsigned long> tags = pc::CpuControl().getCpuStat(app);
    cout << "CPU STAT\n";
    for (const auto& kv : tags) {
        cout << kv.first << ":" << kv.second << endl;
    }
}

static void getIoMax(pc::App app)
{
    std::string cgroupPath = pc::util::findCgroupPath(app.getPid());
    pc::util::activateController("io", cgroupPath);

    cout << "Setting max wbps\n";
    pc::IOControl().setIOMax(8, 0, pc::IOControl::WBPS, 1000000, app);
    sleep(2);
    map<string, long> tags = pc::IOControl().getIOMax(8, 0, app);
    cout << "IO MAX\n";
    for (const auto& kv : tags) {
        if (kv.second == pc::IOControl().MAX_IO_CONTROL)
            cout << kv.first << ":max" << endl;
        else
            cout << kv.first << ":" << kv.second << endl;
    }
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
        setCpuMax(app, 33);
        setCpuMax(app);
        getCpuStat(app);
        getIoMax(app);
    } catch (pc::PcException &e) {
        cerr << "PcException: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }

}
