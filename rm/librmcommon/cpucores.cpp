#include "cpucores.h"
#include <sys/sysinfo.h>

namespace rmcommon {

int getCpuCores() noexcept
{
    return get_nprocs();
}

}   // namespace rmcommon
