#ifndef PLATFORMLOAD_H
#define PLATFORMLOAD_H

#include <vector>

namespace rmcommon {
/*!
 * \class encapsulates cpu utilization information
 */
class PlatformLoad {
    /*! Usage of each CPU */
    std::vector<int> cpus_;
    /*! Usage of each core */
    std::vector<int> cores_;
    /*! Usage of each processing unit */
    std::vector<int> pus_;
public:
    void addCpuLoad(int load) {
        cpus_.push_back(load);
    }

    void addCoreLoad(int load) {
        cores_.push_back(load);
    }

    void addPULoad(int load) {
        pus_.push_back(load);
    }

    int getCpuLoad(int idx) {
        return cpus_[idx];
    }

    int getCoreLoad(int idx) {
        return cores_[idx];
    }

    int getPULoad(int idx) {
        return pus_[idx];
    }

};

}   // namespace rmcommon

#endif // PLATFORMLOAD_H
