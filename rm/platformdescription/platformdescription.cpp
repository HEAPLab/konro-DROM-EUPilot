#include "platformdescription.h"
#include "cpucores.h"
#include "memoryinfo.h"
#include <iostream>
#include <sys/sysinfo.h>
#include <hwloc.h>

using namespace std;

PlatformDescription::PlatformDescription()
{
    doHwloc();
    findNumCores();
    findMemory();
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

void PlatformDescription::doHwloc()
{
    // See: /usr/share/doc/libhwloc-dev/hwloc-hello.c.gz
    hwloc_topology_t topology;

    // Allocate and initialize topology object
    hwloc_topology_init(&topology);

    // TODO - set filters

    // Perform the topology detection
    hwloc_topology_load(topology);

    int topodepth = hwloc_topology_get_depth(topology);
    cout << "Topology depth = " << topodepth << endl;
    for (int depth = 0; depth < topodepth; ++depth) {
        int nobjs = hwloc_get_nbobjs_by_depth(topology, depth);
        cout << "*** Objects at level " << depth << ": " << nobjs << endl;
        for (int i = 0; i < nobjs; ++i) {
            hwloc_obj_t obj = hwloc_get_obj_by_depth(topology, depth, i);
            if (obj->type == HWLOC_OBJ_MACHINE) {
                cout << "   *** " << i << ": Machine" << endl;
            } else if (obj->type == HWLOC_OBJ_PACKAGE) {
                cout << "   *** " << i << ": Package" << endl;
            } else if (obj->type == HWLOC_OBJ_DIE) {
                cout << "   *** " << i << ": Die" << endl;
            } else if (obj->type == HWLOC_OBJ_GROUP) {
                cout << "   *** " << i << ": Group" << endl;
            } else if (obj->type == HWLOC_OBJ_PU) {
                cout << "   *** " << i << ": Processing Unit, or (Logical) Processor" << endl;
            } else if (obj->type == HWLOC_OBJ_CORE) {
                cout << "   *** " << i << ": Core" << endl;
            } else if (obj->type == HWLOC_OBJ_L1CACHE) {
                cout << "   *** " << i << ": L1 cache" << endl;
            } else if (obj->type == HWLOC_OBJ_L2CACHE) {
                cout << "   *** " << i << ": L2 cache" << endl;
            } else if (obj->type == HWLOC_OBJ_L3CACHE) {
                cout << "   *** " << i << ": L3 cache" << endl;
            } else if (obj->type == HWLOC_OBJ_L4CACHE) {
                cout << "   *** " << i << ": L4 cache" << endl;
            } else if (obj->type == HWLOC_OBJ_L5CACHE) {
                cout << "   *** " << i << ": L5 cache" << endl;
            } else {
                cout << "   *** " << i << ": type = " << obj->type << endl;
            }
        }
    }
    hwloc_topology_destroy(topology);
}
