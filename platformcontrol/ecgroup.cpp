#include "ecgroup.h"

namespace pc {

/*static*/
const char *EcGroup::fileNames[] = {
    "cpu.weight",
    "cpu.max",
    "cpu.stat",
    "cpuset.cpus",
    "cpuset.mems"
    "cpuset.cpus.effective",
    "cpuset.mems.effective",
    "pids.max"
};

/*static*/
std::string EcGroup::getControllerName(ECGROUP ecGroup)
{
    switch (ecGroup) {
    case CPU_WEIGHT:
    case CPU_MAX:
    case CPU_STAT:
        return "cpu";
    case CPUSET_CPUS:
    case CPUSET_MEMS:
    case CPUSET_CPUS_EFFECTIVE:
    case CPUSET_MEMS_EFFECTIVE:
        return "cpuset";
    case PIDS_MAX:
        return "pids";
    default:
        return "";
    }
}

std::string EcGroup::getFileName(EcGroup::ECGROUP ecGroup)
{
    return fileNames[ecGroup];
}

}   // namespace pc
