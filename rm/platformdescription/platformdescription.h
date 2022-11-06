#ifndef PLATFORMDESCRIPTION_H
#define PLATFORMDESCRIPTION_H

#include "coremapping.h"
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
    void dumpCoreTopology();

public:
    explicit PlatformDescription();

    /*!
     * \brief Gets the number of CPUs on the machine
     * \return the number of CPUs
     */
    int getNumCpus() const;

    /*!
     * \brief Gets the number of cores on the machine
     * \return the number of cores
     */
    int getNumCores() const;

    /*!
     * \brief Returns the machine topology as a vector of CpuCores
     */
    std::vector<CoreMapping> getCoreTopology() const;

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
};

#endif // PLATFORMDESCRIPTION_H
