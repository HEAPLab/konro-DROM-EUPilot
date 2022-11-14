#include "randpolicy.h"
#include "cpusetcontrol.h"
#include "cpusetvector.h"
#include <iostream>
#include <sstream>
#include <random>
#include <utility>

using namespace std;
/*!
 * Extracts a random CPU number
 * \param cpusNum the number of CPUs avaiable on the machine
 * \return a random CPU number
 */
int getRandNumber(int cpusNum)
{
    std::random_device rd;
    std::mt19937 rng(rd()); // Mersenne-Twister generator
    std::uniform_int_distribution<int> uni(0, cpusNum-1);
    return uni(rng);
}

RandPolicy::RandPolicy(PlatformDescription pd) :
    platformDescription_(pd),
    cat_(log4cpp::Category::getRoot())
{
}

void RandPolicy::addApp(shared_ptr<AppMapping> appMapping)
{
    try {
        short puNum = getRandNumber(platformDescription_.getNumProcessingUnits());
        rmcommon::CpusetVector vec{{puNum, puNum}};
        pc::CpusetControl::instance().setCpus(vec, appMapping->getApp());
        appMapping->setPuVector(vec);
    } catch (exception &e) {
        // An exception can happen for a short lived process; the
        // process has died and the Workload Manager has already
        // removed the cgroup directory for the process, but the
        // Resource Policicy Manager has not yet received the
        // corresponding RemoveProcEvent from the WorkloadManager
        ostringstream os;
        os << "RANDPOLICY addApp: EXCEPTION " << e.what();
        cat_.error(os.str());
    }
}

void RandPolicy::removeApp(shared_ptr<AppMapping> appMapping)
{
    // no action required
}

void RandPolicy::timer()
{
    // no action required
}

void RandPolicy::monitor(rmcommon::MonitorEvent *ev)
{
    // no action required
}
