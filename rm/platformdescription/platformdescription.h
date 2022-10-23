#ifndef PLATFORMDESCRIPTION_H
#define PLATFORMDESCRIPTION_H

/*!
 * \brief stores information about the machine on which Konro is running
 */
class PlatformDescription {
    int numCores_;
    unsigned long totalRamKB_;
    unsigned long freeRamKB_;
    unsigned long totalSwapKB_;
    unsigned long freeSwapKB_;

    void findNumCores();
    void findMemory();

public:
    explicit PlatformDescription();

    int getNumCores() const {
        return numCores_;
    }

    unsigned long getTotalRam() const {
        return totalRamKB_;
    }

    unsigned long getTotalSwap() const {
        return totalSwapKB_;
    }
};

#endif // PLATFORMDESCRIPTION_H
