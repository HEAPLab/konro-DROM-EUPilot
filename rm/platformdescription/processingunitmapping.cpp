#include "processingunitmapping.h"

using namespace std;

ProcessingUnitMapping::ProcessingUnitMapping(int osPuIdx) :
    osPuIdx_(osPuIdx),
    osCoreIdx_(-1)
{
    for (int i = 0; i < NUM_CACHES; ++i) {
        osCacheIdx[i] = -1;
    }
}

void ProcessingUnitMapping::printOnOstream(std::ostream &os) const
{
    os << "Proc. Unit " << "OS index: " << osPuIdx_ << endl
       << "  on Core "  << "OS index: " << osCoreIdx_ << endl
       << "  L1 cache " << "OS index: " << osCacheIdx[0] << endl
       << "  L2 cache " << "OS index: " << osCacheIdx[1] << endl
       << "  L3 cache " << "OS index: " << osCacheIdx[2] << endl
       << "  L4 cache " << "OS index: " << osCacheIdx[3] << endl
       << "  L5 cache " << "OS index: " << osCacheIdx[4] << endl;
}
