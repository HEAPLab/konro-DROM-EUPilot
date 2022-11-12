#ifndef PLATFORMDESCRIPTION_H
#define PLATFORMDESCRIPTION_H

#include "processingunitmapping.h"
#include <memory>
/*!
 * \brief stores information about the machine on which Konro is running
 */
class PlatformDescription {
    struct PlatformDescriptionImpl;
    std::shared_ptr<PlatformDescriptionImpl> pimpl_;
    int numProcessors_;
    unsigned long totalRamKB_;
    unsigned long freeRamKB_;
    unsigned long totalSwapKB_;
    unsigned long freeSwapKB_;

    void findNumProcessors();
    void findMemory();

    void printHwlocObj(int level, void *obj);

public:
    explicit PlatformDescription();

    /*!
     * \brief Gets the number of cores on the machine
     * \return the number of cores
     */
    int getNumCores() const;

    /*!
     * \brief Gets the number of processing units on the machine
     * \return the number of processing units
     */
    int getNumProcessingUnits() const;

    /*!
     * \brief Returns the machine topology as a vector of CpuCores
     */
    std::vector<ProcessingUnitMapping> getCoreTopology() const;

    /*!
     * \brief Gets the total amount of RAM on the machine in Kb
     * \return the amount of RAM
     */
    unsigned long getTotalRam() const {
        return totalRamKB_;
    }

    /*!
     * \brief Gets the total amount of swap on the machine in Kb
     * \return the amount of swap
     */
    unsigned long getTotalSwap() const {
        return totalSwapKB_;
    }

    void dumpCoreTopology();
};

#endif // PLATFORMDESCRIPTION_H
