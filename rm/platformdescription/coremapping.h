#ifndef COREMAPPING_H
#define COREMAPPING_H

#include <vector>
#include <iostream>
#include <utility>

/*!
 * \brief The CpuCore class encapsulates information on a Core
 *
 * A Core, called "Processing Unit" by hwloc, is contained in a CPU.
 * A CPU can have one or more Cores.
 */
class CoreMapping {
    /*! number of caches supported by hwloc */
    static constexpr int NUM_CACHES = 5;

    // There are two types of indexes for each object:
    // - the "logical" index assigned by hwloc
    // - the index used by the Operating System

    /*! operating system index of the Core */
    int osCoreIdx_;

    // The CPU is the object which contains the Core
    // One CPU -> one or more cores

    /*! operating system CPU index */
    int osCpuIdx_;

    /*! operating system Cache index */
    int osCacheIdx[NUM_CACHES];     // element [0] is the index of the L1 cache

    void printOnOstream(std::ostream &os) const;

public:
    CoreMapping(int osIdx);

    void setCpu(int osIdx) {
        osCpuIdx_ = osIdx;
    }

    void setCache(int cacheLevel, int osIdx) {
        if (cacheLevel >= 1 && cacheLevel <= NUM_CACHES) {
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

    /*!
     * Returns the OS index of the specified cache level
     * connected to the core.
     */
    int getCacheOsIdx(int cacheLevel) const {
        if (cacheLevel >= 1 && cacheLevel <= NUM_CACHES) {
            return osCacheIdx[cacheLevel-1];
        } else {
            return -1;
        }
    }

    /*!
     * Returns the OS index of the CPU connected to the core.
     */
    int getCpuOsIdx() const {
        return osCpuIdx_;
    }

    friend std::ostream &operator <<(std::ostream &os, const CoreMapping &core) {
        core.printOnOstream(os);
        return os;
    }
};

#endif // COREMAPPING_H
