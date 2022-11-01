#include "cpucore.h"

using namespace std;

CpuCore::CpuCore(int hwlIdx, int osIdx) :
    hwlCoreIdx_(hwlIdx),
    osCoreIdx_(osIdx),
    hwlCpuIdx_(-1),
    osCpuIdx_(-1)
{
    for (int i = 0; i < NUM_CACHES; ++i) {
        hwlCacheIdx[i] = osCacheIdx[i] = -1;
    }
}

void CpuCore::printOnOstream(std::ostream &os) const
{
    os << "CpuCore " << hwlCoreIdx_ << " (os index: " << osCoreIdx_ << ")" << endl
       << "  on CPU " << hwlCpuIdx_ << " (os index: " << osCpuIdx_ << ")" << endl
       << "  L1 cache " << hwlCacheIdx[0] << " (os index: " << osCacheIdx[0] << ")" << endl
       << "  L2 cache " << hwlCacheIdx[1] << " (os index: " << osCacheIdx[1] << ")" << endl
       << "  L3 cache " << hwlCacheIdx[2] << " (os index: " << osCacheIdx[2] << ")" << endl
       << "  L4 cache " << hwlCacheIdx[3] << " (os index: " << osCacheIdx[3] << ")" << endl
       << "  L5 cache " << hwlCacheIdx[4] << " (os index: " << osCacheIdx[4] << ")" << endl;
}
