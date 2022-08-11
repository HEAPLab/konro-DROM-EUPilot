#include "cpucontrol.h"
#include <cstdlib>

namespace pc {

/*static*/
const char *CpuControl::controllerName_ = "cpu";

/*static*/
const std::map<CpuControl::ControllerFile, const char *> CpuControl::fileNamesMap_ = {
    { WEIGHT, "cpu.weight" },
    { MAX, "cpu.max" },
    { STAT, "cpu.stat" }
};


void CpuControl::setCpuMax(int percentage, App app)
{
    std::ostringstream os;
    // for cpu.max normalize value between 0 and period_
    if (percentage == 100) {
        os << "max " << period_;
    } else {
        os << ((percentage * period_) / 100) << ' ' << period_;
    }
    CGroupControl::setValue(controllerName_, fileNamesMap_.at(MAX), os.str(), app);
}

int CpuControl::getCpuMax(App app)
{
    std::string svalue = CGroupControl::getValue(fileNamesMap_.at(MAX), app);
    std::istringstream is(svalue);
    std::string value;
    int period;
    is >> value >> period;
    if (is.fail())
        return -1;      // TODO - fixme
    if (value == "max")
        return 100;
    int intValue = static_cast<int>(strtol(value.c_str(), nullptr, 10));
    return (intValue * 100) / period;
}

std::map<std::string, unsigned long> CpuControl::getCpuStat(App app)
{
    return getContentAsMap(fileNamesMap_.at(STAT), app);
}

void CpuControl::setCpuWeight(int weight, App app)
{
    CGroupControl::setValue(controllerName_, fileNamesMap_.at(WEIGHT), weight, app);
}


}   // namespace pc
