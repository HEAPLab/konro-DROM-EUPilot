#include "coremapping.h"

using namespace std;

CoreMapping::CoreMapping(int osIdx) :
    osCoreIdx_(osIdx),
    osCpuIdx_(-1)
{
    for (int i = 0; i < NUM_CACHES; ++i) {
        osCacheIdx[i] = -1;
    }
}

void CoreMapping::printOnOstream(std::ostream &os) const
{
    os << "CpuCore "    << "OS index: " << osCoreIdx_ << endl
       << "  on CPU "   << "OS index: " << osCpuIdx_ << endl
       << "  L1 cache " << "OS index: " << osCacheIdx[0] << endl
       << "  L2 cache " << "OS index: " << osCacheIdx[1] << endl
       << "  L3 cache " << "OS index: " << osCacheIdx[2] << endl
       << "  L4 cache " << "OS index: " << osCacheIdx[3] << endl
       << "  L5 cache " << "OS index: " << osCacheIdx[4] << endl;
}
