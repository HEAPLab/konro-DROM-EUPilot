#ifndef CPUCORE_H
#define CPUCORE_H

#include <vector>
#include <iostream>
#include <utility>

/*!
 * \brief The CpuCore class encapsulates information on a Core
 *
 * A Core, called "Processing Unit" by hwloc, is contained in a CPU.
 * A CPU can have one or more Cores.
 */
class CpuCore {
    /*! number of caches supported by hwloc */
    static constexpr int NUM_CACHES = 5;

    // There are two types of indexes for each object:
    // - the "logical" index assigned by hwloc
    // - the index used by the Operating System

    /*! hwloc index of the Core (Processing Unit) */
    int hwlCoreIdx_;

    /*! operating system index of the Core */
    int osCoreIdx_;

    // The CPU is the object which contains the Core
    // One CPU -> one or more cores

    /*! hwloc CPU index */
    int hwlCpuIdx_;

    /*! operating system CPU index */
    int osCpuIdx_;

    /*! hwloc Cache index */
    int hwlCacheIdx[NUM_CACHES];    // element [0] is the index of the L1 cache

    /*! operating system Cache index */
    int osCacheIdx[NUM_CACHES];     // element [0] is the index of the L1 cache

    void printOnOstream(std::ostream &os) const;

public:
    CpuCore(int hwlIdx, int osIdx);

    void setCpu(int hwlIdx, int osIdx) {
        hwlCpuIdx_ = hwlIdx;
        osCpuIdx_ = osIdx;
    }

    void setCache(int cacheLevel, int hwlIdx, int osIdx) {
        if (cacheLevel >= 1 && cacheLevel <= NUM_CACHES) {
            hwlCacheIdx[cacheLevel-1] = hwlIdx;
            osCacheIdx[cacheLevel-1] = osIdx;
        }
    }

    /*!
     * Returns the Operating System index of this core
     * or -1 if not available
     */
    int getOsIdx() const {
        return osCoreIdx_;
    }

    int getCacheOsIdx(int cacheLevel) const {
        if (cacheLevel >= 1 && cacheLevel <= NUM_CACHES) {
            return osCacheIdx[cacheLevel-1];
        } else {
            return -1;
        }
    }

    int getCpuOsIdx() const {
        return osCpuIdx_;
    }

    friend std::ostream &operator <<(std::ostream &os, const CpuCore &core) {
        core.printOnOstream(os);
        return os;
    }
};

#endif // CPUCORE_H
