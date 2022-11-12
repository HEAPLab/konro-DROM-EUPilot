#include "memoryinfo.h"
#include <sys/sysinfo.h>

namespace rmcommon {

void getMemoryInfo(unsigned long &totalRamKb) noexcept
{
    struct sysinfo info;
    if (sysinfo(&info) < 0) {
        totalRamKb = 0;
    } else {
        totalRamKb = info.totalram * info.mem_unit / 1024;
    }
}

void getSwapInfo(unsigned long &totalSwapKb) noexcept
{
    struct sysinfo info;
    if (sysinfo(&info) < 0) {
        totalSwapKb = 0;
    } else {
        totalSwapKb = info.totalswap * info.mem_unit / 1024;
    }
}

}   // namespace rmcommon
