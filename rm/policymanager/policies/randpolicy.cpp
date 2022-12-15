#include "randpolicy.h"
#include "cpusetcontrol.h"
#include "cpusetvector.h"
#include <iostream>
#include <sstream>
#include <random>
#include <utility>
#include <log4cpp/Category.hh>

using namespace std;

namespace rp {

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
    platformDescription_(pd)
{
}

void RandPolicy::addApp(shared_ptr<AppMapping> appMapping)
{
    pid_t pid = appMapping->getPid();
    try {
        short puNum = getRandNumber(platformDescription_.getNumProcessingUnits());
        log4cpp::Category::getRoot().debug("RANDPOLICY addApp PID %ld to PU %d", (long)pid, puNum);
        appMapping->setPuVector({{puNum, puNum}});
    } catch (exception &e) {
        // An exception can happen for a short lived process; the
        // process has died and the Workload Manager has already
        // removed the cgroup directory for the process, but the
        // Resource Policicy Manager has not yet received the
        // corresponding RemoveEvent from the WorkloadManager
        log4cpp::Category::getRoot().error("RANDPOLICY addApp PID %ld: EXCEPTION %s",
                                           (long)pid, e.what());
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

void RandPolicy::monitor(std::shared_ptr<const rmcommon::MonitorEvent> event)
{
    // no action required
}

void RandPolicy::feedback(std::shared_ptr<const rmcommon::FeedbackEvent> event)
{
    // no action required
}

}   // namespace rp
