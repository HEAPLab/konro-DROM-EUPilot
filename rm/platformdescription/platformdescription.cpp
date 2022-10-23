#include "platformdescription.h"
#include "cpucores.h"
#include "memoryinfo.h"
#include <thread>
#include <sys/sysinfo.h>

using namespace std;

PlatformDescription::PlatformDescription()
{
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
