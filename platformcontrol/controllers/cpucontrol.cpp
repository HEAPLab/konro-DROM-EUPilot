#include "cpucontrol.h"

namespace pc {

/*static*/
const char *CpuControl::controllerName_ = "cpu";

/*static*/
const std::map<CpuControl::ControllerFile, const char *> CpuControl::fileNamesMap_ = {
    { WEIGHT, "cpu.weight" },
    { MAX, "cpu.max" },
    { MAX_BURST, "cpu.max.burst" },
    { STAT, "cpu.stat" }
};


}   // namespace pc
