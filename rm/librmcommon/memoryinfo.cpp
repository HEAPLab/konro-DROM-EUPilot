#include "memoryinfo.h"
#include <sys/sysinfo.h>

namespace rmcommon {

void getMemoryInfo(unsigned long &totalRamKb, unsigned long &freeRamKb) noexcept
{
    struct sysinfo info;
    if (sysinfo(&info) < 0) {
        totalRamKb = freeRamKb = 0;
    } else{
        totalRamKb = info.totalram * info.mem_unit / 1024;
        freeRamKb = info.freeram * info.mem_unit / 1024;
    }
}

void getSwapInfo(unsigned long &totalSwapKb, unsigned long &freeSwapKb) noexcept
{
    struct sysinfo info;
    if (sysinfo(&info) < 0) {
        totalSwapKb = freeSwapKb = 0;
    } else {
        totalSwapKb = info.totalswap * info.mem_unit / 1024;
        freeSwapKb = info.freeswap * info.mem_unit / 1024;
    }
}

}   // namespace rmcommon
