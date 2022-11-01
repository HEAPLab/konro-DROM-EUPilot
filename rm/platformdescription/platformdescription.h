#ifndef PLATFORMDESCRIPTION_H
#define PLATFORMDESCRIPTION_H

#include "cpucore.h"
#include <memory>
/*!
 * \brief stores information about the machine on which Konro is running
 */
class PlatformDescription {
    struct PlatformDescriptionImpl;
    std::shared_ptr<PlatformDescriptionImpl> pimpl_;
    int numCores_;
    unsigned long totalRamKB_;
    unsigned long freeRamKB_;
    unsigned long totalSwapKB_;
    unsigned long freeSwapKB_;

    void findNumCores();
    void findMemory();

    void printHwlocObj(int level, void *obj);
    void testHwloc1();
    void testHwloc2();
    void testHwloc3();
    void testHwloc4();

public:
    explicit PlatformDescription();

    int getNumCpus() const;
    int getNumCores() const;
    std::vector<CpuCore> getCoreTopology() const;

    unsigned long getTotalRam() const {
        return totalRamKB_;
    }

    unsigned long getTotalSwap() const {
        return totalSwapKB_;
    }
};

#endif // PLATFORMDESCRIPTION_H
