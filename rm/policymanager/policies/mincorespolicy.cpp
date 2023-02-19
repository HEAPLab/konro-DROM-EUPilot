#include "mincorespolicy.h"
#include <vector>
#include <algorithm>
#include <sstream>
#include <limits>
#include <log4cpp/Category.hh>

namespace rp {

MinCoresPolicy::MinCoresPolicy(const AppMappingSet &apps, PlatformDescription pd) :
    apps_(apps),
    platformDescription_(pd),
    hasLastPlatformLoad(false)
{
}

/*! Counts the number of apps in the same cgroup of the specified one */
int countAppsWithSameCgroup(const AppMappingSet &apps, AppMappingPtr appMapping) {
    int n = 0;
    for (const auto &am: apps) {
        if (am->getCgroupDir() == appMapping->getCgroupDir()) {
            ++n;
        }
    }
    return n;
}

/*! Returns the PU that currently has lower usage. */
int MinCoresPolicy::getLowerUsagePU()
{
    if (hasLastPlatformLoad) {
        const std::vector<int> &pus = lastPlatformLoad.getPUs();
        if (!pus.empty()) {
            std::vector<int>::const_iterator minElem = std::min_element(pus.begin(), pus.end());
            return std::distance(pus.begin(), minElem);
        }
    }
    return 0;
}

static void dumpSet(const char *name, const std::set<short> &s)
{
    std::ostringstream os;

    for (short n: s) {
        os << n << ' ';
    }
    log4cpp::Category::getRoot().info("%s %s", name, os.str().c_str());
}

static void dumpCpuSetVector(const char *name, const rmcommon::CpusetVector &vec)
{
    std::string s = rmcommon::toString(vec);
    log4cpp::Category::getRoot().info("%s %s", name, s.c_str());
}

/*! Picks which PU is the most appropriate to execute a new process.
    Initially, every process is always assigned to a single PU.
    The range of PUs available for a process can later be expanded. */
int MinCoresPolicy::pickInitialCpu()
{
    return getLowerUsagePU();
}

std::set<short> MinCoresPolicy::getAvailablePUs(const rmcommon::CpusetVector &vec)
{
    std::set<short> allPUs = platformDescription_.getPUSet();
    std::set<short> usedPUs = rmcommon::toSet(vec);
    std::set<short> res;
    std::set_difference(allPUs.begin(), allPUs.end(), usedPUs.begin(), usedPUs.end(), std::inserter(res, end(res)));

    dumpSet("allPUs : ", allPUs);
    dumpSet("usedPUs: ", usedPUs);
    dumpSet("res    : ", res);
    return res;
}

short MinCoresPolicy::getNextPU(const rmcommon::CpusetVector &vec)
{
    std::set<short> availPUs = getAvailablePUs(vec);
    if (availPUs.empty()) {
        return -1;          // no free PUs
    }
    std::set<short> usedPUs = rmcommon::toSet(vec);
    short bestPU = -1;
    int bestPUdistance = numeric_limits<int>::max();
    for (short usedPu: usedPUs) {
        for (short availPu: availPUs) {
            int distance = platformDescription_.getPUDistance(usedPu, availPu);
            if (distance >= 0 && distance < bestPUdistance) {
                bestPU = availPu;
                bestPUdistance = distance;
            }
        }
    }
    return bestPU;
}

short MinCoresPolicy::pickWorstPU(const rmcommon::CpusetVector &vec)
{
    if (rmcommon::countPUs(vec) == 1) {
        return -1;          // can't reduce the number of PUs
    }
    // tranform to linear vector, i.e. "1-3,5" becomes "1,2,3,5"
    std::vector<short> vec1 = rmcommon::toVector(vec);
//    log4cpp::Category::getRoot().info("MINCORESPOLICY testing removal of PU. Number of PUs = %d", (int)vec1.size());
    int bestTotalDistance = std::numeric_limits<int>::max();
    int bestTotalDistanceIdx = -1;
    for (int removePUIdx = 0; removePUIdx < vec1.size(); ++removePUIdx) {
//        log4cpp::Category::getRoot().info("MINCORESPOLICY testing removal of PU %d", vec1[removePUIdx]);
        // calculate the sum of all distances between PUs
        // when removePUIdx is removed
        int totalDistance = 0;
        for (int j = 0; j < vec1.size(); ++j) {
            for (int k = j+1; k < vec1.size(); ++k) {
                if (j != removePUIdx && k != removePUIdx && j != k) {
                    int distance = platformDescription_.getPUDistance(vec1[j], vec1[k]);
                    //log4cpp::Category::getRoot().info("MINCORESPOLICY distance between PU %d and %d = %d",
                    //          vec1[j], vec1[k], distance);
                    totalDistance += distance;
                }
            }
        }
//        log4cpp::Category::getRoot().info("MINCORESPOLICY total distance without PU %d = %d",
//                                           vec1[removePUIdx], totalDistance);
        if (totalDistance < bestTotalDistance) {
            bestTotalDistance = totalDistance;
            bestTotalDistanceIdx = removePUIdx;
//            log4cpp::Category::getRoot().info("MINCORESPOLICY best total distance = %d without PU %d",
//                                               bestTotalDistance, vec1[removePUIdx]);
        }
    }
    return bestTotalDistanceIdx != -1 ? vec1[bestTotalDistanceIdx] : -1;
}

void MinCoresPolicy::addApp(AppMappingPtr appMapping)
{
    // If there are already other Apps in the same cgroup folder,
    // handle them as a group and do nothing here
    if (countAppsWithSameCgroup(apps_, appMapping) > 1) {
        log4cpp::Category::getRoot().debug("MINCORESPOLICY addApp to an already initialized cgroup");
        return;
    }

    pid_t pid = appMapping->getPid();
    try {
        short initialPU = pickInitialCpu();
        appMapping->setPuVector({{initialPU, initialPU}});
        log4cpp::Category::getRoot().info("MINCORESPOLICY addApp PID %ld to PU %d", (long)pid, initialPU);
    } catch (exception &e) {
        // An exception can happen for a short lived process; the
        // process has died and the Workload Manager has already
        // removed the cgroup directory for the process, but the
        // Resource Policicy Manager has not yet received the
        // corresponding RemoveEvent from the WorkloadManager
        log4cpp::Category::getRoot().error("MINCORESPOLICY addApp PID %ld: EXCEPTION %s",
                                           (long)pid, e.what());
    }
}

void MinCoresPolicy::removeApp(AppMappingPtr appMapping)
{

}

void MinCoresPolicy::timer()
{
    // no action required
}

void MinCoresPolicy::monitor(std::shared_ptr<const rmcommon::MonitorEvent> event)
{
    lastPlatformLoad = event->getPlatformLoad();
    hasLastPlatformLoad = true;
}

void MinCoresPolicy::feedback(AppMappingPtr appMapping, int feedback)
{
    float lowerLimit = 100.0f * (1-slack);
    float upperLimit = 100.0f * (1+slack);
    if (feedback < lowerLimit) {
        if (appMapping->getLastFeedback() < lowerLimit) {
            rmcommon::CpusetVector vec = appMapping->getPuVector();
            dumpCpuSetVector("usedPUs: ", vec);
#if 0
            // TODO - remove - this is a test of PU removal
            if (rmcommon::countPUs(vec) == 3) {
                short pu = pickWorstPU(vec);
                log4cpp::Category::getRoot().info("MINCORESPOLICY removing PU %d", pu);
                rmcommon::removePU(vec, pu);
                appMapping->setPuVector(vec);
                dumpCpuSetVector("newPUs: ", vec);
                return;
            }
#endif
            short newPU = getNextPU(vec);
            if (newPU != -1) {
                log4cpp::Category::getRoot().info("MINCORESPOLICY adding PU %d", newPU);
                rmcommon::addPU(vec, newPU);
                appMapping->setPuVector(vec);
                dumpCpuSetVector("newPUs: ", vec);
            } else {
                log4cpp::Category::getRoot().info("MINCORESPOLICY no new PU available for proc %d", (long)appMapping->getPid());
            }
        }
    } else if (feedback > upperLimit) {
        // app is going well
        rmcommon::CpusetVector vec = appMapping->getPuVector();
        if (rmcommon::countPUs(vec) < 2) {
            log4cpp::Category::getRoot().info("MINCORESPOLICY no PU to remove for proc %d",
                                              (long)appMapping->getPid());
        } else {
            short pu = pickWorstPU(vec);
            bool rc = rmcommon::removePU(vec, pu);
            if (rc) {
                log4cpp::Category::getRoot().info("MINCORESPOLICY removing PU %d for proc %d",
                                                  pu, (long)appMapping->getPid());
                appMapping->setPuVector(vec);
            } else {
                log4cpp::Category::getRoot().error("MINCORESPOLICY could not remove PU %d for proc %d",
                                                  pu, (long)appMapping->getPid());
            }
        }
    }
    appMapping->setLastFeedback(feedback);
}

}   // namespace rp
