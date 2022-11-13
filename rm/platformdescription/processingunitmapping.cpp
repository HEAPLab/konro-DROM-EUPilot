#include "processingunitmapping.h"

using namespace std;

ProcessingUnitMapping::ProcessingUnitMapping(int osPuIdx) :
    osPuIdx_(osPuIdx),
    osCoreIdx_(-1),
    osCpuIdx_(-1)
{
    for (int i = 0; i < NUM_CACHES; ++i) {
        osCacheIdx[i] = -1;
    }
}

void ProcessingUnitMapping::printOnOstream(std::ostream &os) const
{
    os << "ProcessingUnit {"
       << "\"idx\":" << osPuIdx_
       << ",\"core\":" << osCoreIdx_
       << ",\"cpu\":" << osCpuIdx_
       << ",\"L1cache\":" << osCacheIdx[0]
       << ",\"L2cache\":" << osCacheIdx[1]
       << ",\"L3cache\":" << osCacheIdx[2]
       << ",\"L4cache\":" << osCacheIdx[3]
       << ",\"L5cache\":" << osCacheIdx[4]
       << "}";
}
