#include "platformdescription.h"
#include "cpucores.h"
#include "memoryinfo.h"
#include <iostream>
#include <algorithm>
#include <sys/sysinfo.h>
#include <hwloc.h>

using namespace std;

struct PlatformDescription::PlatformDescriptionImpl {
    static constexpr int NUM_CACHES = 5;
    // hwloc data
    hwloc_topology_t topology;
    // Depth in the hwloc topology tree
    int depthPU;

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

        this->depthPU = hwloc_get_type_depth(this->topology, HWLOC_OBJ_PU);
    }

    /*!
     * \brief findObjByOsIndex
     * \param depth where to search
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
    vector<CpuCore> getCpuCores() {
        vector<CpuCore> vec;
        // scan all the cores (Processing Units for hwloc)
        hwloc_obj_t objCore = nullptr;
        while ((objCore = hwloc_get_next_obj_by_type(this->topology, HWLOC_OBJ_PU, objCore)) != nullptr) {
            CpuCore cpuCore(objCore->logical_index, objCore->os_index);
            hwloc_obj_t parent = objCore->parent;
            while (parent != nullptr) {
                switch (parent->type) {
                case HWLOC_OBJ_CORE:
                    cpuCore.setCpu(parent->logical_index, parent->os_index);
                    break;
                case HWLOC_OBJ_L1CACHE:
                    cpuCore.setCache(1, parent->logical_index, parent->os_index);
                    break;
                case HWLOC_OBJ_L2CACHE:
                    cpuCore.setCache(2, parent->logical_index, parent->os_index);
                    break;
                case HWLOC_OBJ_L3CACHE:
                    cpuCore.setCache(3, parent->logical_index, parent->os_index);
                    break;
                case HWLOC_OBJ_L4CACHE:
                    cpuCore.setCache(4, parent->logical_index, parent->os_index);
                    break;
                case HWLOC_OBJ_L5CACHE:
                    cpuCore.setCache(5, parent->logical_index, parent->os_index);
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
    testHwloc1();
    testHwloc2();
    testHwloc3();
    testHwloc4();
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
std::vector<CpuCore> PlatformDescription::getCoreTopology() const
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
 * Dumps the hwloc topolgy by depth
 */
void PlatformDescription::testHwloc1()
{
    int topodepth = hwloc_topology_get_depth(pimpl_->topology);
    cout << "Topology depth = " << topodepth << endl;
    for (int depth = 0; depth < topodepth; ++depth) {
        int nobjs = hwloc_get_nbobjs_by_depth(pimpl_->topology, depth);
        cout << "*** Objects at level " << depth << ": " << nobjs << endl;
        for (int i = 0; i < nobjs; ++i) {
            hwloc_obj_t obj = hwloc_get_obj_by_depth(pimpl_->topology, depth, i);
            char objType[64], attr[1024];
            hwloc_obj_type_snprintf(objType, sizeof objType, obj, 0);
            cout << "   *** " << i << ": " << objType;
            hwloc_obj_attr_snprintf(attr, sizeof attr, obj, "/", 0);
            if (*attr) {
                cout << "  (" << attr << ")";
            }
            if (obj->os_index != (unsigned)-1) {
                cout << ", os index: " << obj->os_index;
            }
            cout << endl;
        }
    }
    int num_cores = hwloc_get_nbobjs_by_type(pimpl_->topology, HWLOC_OBJ_CORE);
    int num_pu = hwloc_get_nbobjs_by_type(pimpl_->topology, HWLOC_OBJ_PU);
    cout << endl << "Cores: " << num_cores << ", processing units: " << num_pu << endl;
}

/*!
 * Dumps the hwloc tree
 */
void PlatformDescription::testHwloc2()
{
    hwloc_obj_t obj = hwloc_get_root_obj(pimpl_->topology);
    printHwlocObj(0, obj);
}

void PlatformDescription::printHwlocObj(int level, void *obj)
{
    if (!obj) {
        return;
    }
    for (int i = 0; i < level; ++i) {
        cout << "    ";       // indent
    }
    hwloc_obj_t o = reinterpret_cast<hwloc_obj_t>(obj);
    char objType[64], attr[1024];
    hwloc_obj_type_snprintf(objType, sizeof objType, o, 0);
    cout << "level " << level << ": object " << objType;
    if (o->os_index != (unsigned)-1) {
        cout << ", os index: " << o->os_index;
    }
    hwloc_obj_attr_snprintf(attr, sizeof attr, o, "/", 0);
    if (*attr) {
        cout << "  (" << attr << ")";
    }
    cout << endl;
    // recursively print the children
    for (int i = 0; i < o->arity; ++i) {
        printHwlocObj(level+1, o->children[i]);
    }
}

/*!
 * \brief Tests hwloc distance matrix
 */
void PlatformDescription::testHwloc3()
{
    struct hwloc_distances_s *distances;
    unsigned int nr = 1;

    int rc = hwloc_distances_get(pimpl_->topology, &nr, &distances, 0, 0);
    cout << "hwloc_distances_get returned " << rc << endl;
    if (rc == 0) {
        // no error
        cout << "Created " << nr << " matrices" << endl;
    } else {
        cout << "ERROR calling hwloc_distances_get" << endl;
    }
    if (distances) {
        hwloc_distances_release(pimpl_->topology, distances);
    } else {
        cout << "No distance to release" << endl;
    }
}

/*!
 * \brief Test Core topology
 */
void PlatformDescription::testHwloc4()
{
    vector<CpuCore> coreTopo = getCoreTopology();
    cout << "----- CORE TOPOLOGY START -----" << endl;
    for (const auto &core: coreTopo) {
        cout << core << endl;
    }
    cout << "----- CORE TOPOLOGY END -----" << endl;

}
