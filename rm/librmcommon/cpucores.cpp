#include "cpucores.h"
#include <sys/sysinfo.h>

namespace rmcommon {

int getProcessors() noexcept
{
    return get_nprocs();
}

}   // namespace rmcommon
