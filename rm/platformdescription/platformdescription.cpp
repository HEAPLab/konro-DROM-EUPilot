#include "platformdescription.h"
#include "cpucores.h"
#include "memoryinfo.h"
#include <iostream>
#include <algorithm>
#include <sys/sysinfo.h>
#include <hwloc.h>

using namespace std;

struct PlatformDescription::PlatformDescriptionImpl {
    // max number of cache levels supported by hwloc
    static constexpr int NUM_CACHES = 5;
    // hwloc data
    hwloc_topology_t topology;

    PlatformDescriptionImpl() {
        initTopology();
    }

    ~PlatformDescriptionImpl() {
        if (this->topology) {
            hwloc_topology_destroy(this->topology);
        }
    }

    void initTopology() {
        // See: /usr/share/doc/libhwloc-dev/hwloc-hello.c.gz

        // Allocate and initialize topology object
        hwloc_topology_init(&this->topology);

        // For NUMA nodes see:
        // https://www.open-mpi.org/projects/hwloc/doc/v2.3.0/a00360.php
        hwloc_topology_set_all_types_filter(this->topology, HWLOC_TYPE_FILTER_KEEP_ALL);

        // Perform the topology detection
        hwloc_topology_load(this->topology);
    }

    /*!
     * \brief Finds an object in the hwloc tree using its OS index
     * \param objType the hwloc type of the object
     * \param osIdx OS index of the object
     * \return object or nullptr
     */
    hwloc_obj_t findObjByOsIndex(hwloc_obj_type_t objType, int osIdx) {
        hwloc_obj_t obj = nullptr;
        while ((obj = hwloc_get_next_obj_by_type(this->topology, objType, obj)) != nullptr) {
            if (obj->os_index == osIdx) {
                return obj;
            }
        }
        return nullptr;
    }

    /*!
     * \brief Returns a list of CPU Cores
     */
    vector<CoreMapping> getCpuCores() {
        vector<CoreMapping> vec;
        // scan all the cores (Processing Units for hwloc)
        hwloc_obj_t objCore = nullptr;
        while ((objCore = hwloc_get_next_obj_by_type(this->topology, HWLOC_OBJ_PU, objCore)) != nullptr) {
            CoreMapping cpuCore(objCore->os_index);
            hwloc_obj_t parent = objCore->parent;
            while (parent != nullptr) {
                switch (parent->type) {
                case HWLOC_OBJ_CORE:
                    cpuCore.setCpu(parent->os_index);
                    break;
                case HWLOC_OBJ_L1CACHE:
                    cpuCore.setCache(1, parent->os_index);
                    break;
                case HWLOC_OBJ_L2CACHE:
                    cpuCore.setCache(2, parent->os_index);
                    break;
                case HWLOC_OBJ_L3CACHE:
                    cpuCore.setCache(3, parent->os_index);
                    break;
                case HWLOC_OBJ_L4CACHE:
                    cpuCore.setCache(4, parent->os_index);
                    break;
                case HWLOC_OBJ_L5CACHE:
                    cpuCore.setCache(5, parent->os_index);
                    break;
                default:
                    break;
                }
                parent = parent->parent;
            }
            vec.push_back(cpuCore);
        }
        return vec;
    }
};

PlatformDescription::PlatformDescription() :
    pimpl_(new PlatformDescriptionImpl)
{
    //dumpCoreTopology();
    findNumCores();
    findMemory();
}

int PlatformDescription::getNumCpus() const
{
    // CPU is called Core by hwloc
    return hwloc_get_nbobjs_by_type(pimpl_->topology, HWLOC_OBJ_CORE);
}

int PlatformDescription::getNumCores() const
{
    // Cores are called Processing Units by hwloc
    return hwloc_get_nbobjs_by_type(pimpl_->topology, HWLOC_OBJ_PU);
}

/*!
 * Returns a list of CPUs (colled "cores" by hwloc) that share the
 * same L1 cache. In order to share the same cache, the core must
 * belong to the same CPU.
 *
 * \param core
 * \return
 */
std::vector<CoreMapping> PlatformDescription::getCoreTopology() const
{
    // Example hwloc tree:
    //    level 0: object Machine, os index: 0
    //        level 1: object Package, os index: 0
    //            level 2: object L3, os index: 0  (4096KB)
    //                level 3: object L2, os index: 0  (256KB)
    //                    level 4: object L1d, os index: 0  (32KB)
    //                        level 5: object Core, os index: 0
    //                            level 6: object PU, os index: 0
    //                            level 6: object PU, os index: 2
    //                level 3: object L2, os index: 1  (256KB)
    //                    level 4: object L1d, os index: 1  (32KB)
    //                        level 5: object Core, os index: 1
    //                            level 6: object PU, os index: 1
    //                            level 6: object PU, os index: 3

    return pimpl_->getCpuCores();
}

void PlatformDescription::findNumCores()
{
    numCores_ = rmcommon::getCpuCores();
}

void PlatformDescription::findMemory()
{
    rmcommon::getMemoryInfo(totalRamKB_, freeRamKB_);
    rmcommon::getSwapInfo(totalSwapKB_, freeSwapKB_);
}

/*!
 * \brief Test Core topology
 */
void PlatformDescription::dumpCoreTopology()
{
    vector<CoreMapping> coreTopo = getCoreTopology();
    cout << "----- CORE TOPOLOGY START -----" << endl;
    for (const auto &core: coreTopo) {
        cout << core << endl;
    }
    cout << "----- CORE TOPOLOGY END -----" << endl;

}
