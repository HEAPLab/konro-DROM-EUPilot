#include <iostream>
#include <map>
#include <cstdlib>
#include "cgroupcontrol.h"
#include "cpucontrol.h"
#include "iocontrol.h"
#include "cgrouputil.h"
#include "numericvalue.h"
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
    pc::CpuControl().setCpuMax(pc::NumericValue::max(), app);
    cout << "Cpu max: requested " << pc::NumericValue::max() << " read " << pc::CpuControl().getCpuMax(app) << endl;
}

static void getCpuStat(pc::App app)
{
    map<string, unsigned long> tags = pc::CpuControl().getCpuStat(app);
    cout << "CPU STAT\n";
    for (const auto& kv : tags) {
        cout << kv.first << ":" << kv.second << endl;
    }
}

static void getIoMax(pc::App app, int major, int minor)
{
    std::string cgroupPath = pc::util::findCgroupPath(app.getPid());
    pc::util::activateController("io", cgroupPath);

    cout << "Setting max wbps\n";
    pc::IOControl().setIOMax(8, 0, pc::IOControl::WBPS, 1000000, app);
    sleep(2);
    map<string, pc::NumericValue> tags = pc::IOControl().getIOMax(major, minor, app);
    cout << "IO MAX\n";
    for (const auto& kv : tags) {
        cout << kv.first << ":" << kv.second << endl;
    }
}

static void getIoStat(pc::App app, int major, int minor)
{
    std::string cgroupPath = pc::util::findCgroupPath(app.getPid());
    pc::util::activateController("io", cgroupPath);
    map<string, pc::NumericValue> tags = pc::IOControl().getIOStat(major, minor, app);
    cout << "IO STAT\n";
    for (const auto& kv : tags) {
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
        getIoStat(app, 253, 0);
    } catch (pc::PcException &e) {
        cerr << "PcException: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }

}
