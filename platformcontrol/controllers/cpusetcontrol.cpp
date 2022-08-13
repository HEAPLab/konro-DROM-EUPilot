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

void CpusetControl::setCpusetCpus(std::string cpus, App app)
{
    CGroupControl::setValue(controllerName_, fileNamesMap_.at(CPUS), cpus, app);
}

std::string CpusetControl::getCpusetCpusEffective(App app)
{
    return CGroupControl::getLine(fileNamesMap_.at(CPUS_EFFECTIVE), app);
}

void CpusetControl::setCpusetMems(std::string memNodes, App app)
{
    CGroupControl::setValue(controllerName_, fileNamesMap_.at(MEMS), memNodes, app);
}

std::string CpusetControl::getCpusetMemsEffective(App app)
{
    return CGroupControl::getLine(fileNamesMap_.at(MEMS_EFFECTIVE), app);
}

}   // namespace pc
