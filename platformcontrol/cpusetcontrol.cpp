#include "cpusetcontrol.h"

namespace pc {

/*static*/
const char *CpusetControl::controllerName_ = "cpuset";

/*static*/
const std::map<CpusetControl::ControllerFile, const char *> CpusetControl::fileNamesMap_ = {
    { CPUS, "cpuset.cpus" },
    { CPUS_EFFECTIVE, "cpuset.cpus.effective" },
    { MEMS, "cpuset.mems" },
    { MEMS_EFFECTIVE, "cpuset.mems.effective" }
};

}   // namespace pc
