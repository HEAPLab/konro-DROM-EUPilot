#include "../cgroupcontrol.h"
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


void CpuControl::setCpuMax(NumericValue percentage, std::shared_ptr<App> app)
{
    std::ostringstream os;
    // for cpu.max normalize value between 0 and period_
    if (percentage.isMax()) {
        os << percentage << ' ' << period_;
    } else {
        os << ((percentage * period_) / 100) << ' ' << period_;
    }
    setValue(controllerName_, fileNamesMap_.at(MAX), os.str(), app);
}

NumericValue CpuControl::getCpuMax(std::shared_ptr<App> app)
{
    std::string svalue = getLine(controllerName_, fileNamesMap_.at(MAX), app);
    std::istringstream is(svalue);
    NumericValue value;
    int period;
    is >> value >> period;
    if (is.fail()) {
        return NumericValue();      // return invalid numeric value
    } else if (value.isInvalid() || value.isMax()) {
        return value;
    } else {
        // return normalized value
        return (value * 100) / period;
    }
}

std::map<std::string, uint64_t> CpuControl::getCpuStat(std::shared_ptr<App> app)
{
    return getContentAsMap(controllerName_, fileNamesMap_.at(STAT), app);
}

void CpuControl::setCpuWeight(int weight, std::shared_ptr<App> app)
{
    setValue(controllerName_, fileNamesMap_.at(WEIGHT), weight, app);
}


}   // namespace pc
