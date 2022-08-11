#include "pidscontrol.h"
#include <cstdlib>

namespace pc {

/*static*/
const char *PidsControl::controllerName_ = "pids";

/*static*/
const std::map<PidsControl::ControllerFile, const char *> PidsControl::fileNamesMap_ = {
    { MAX, "pids.max" },
    { CURRENT, "pids.current" }
};

void PidsControl::setPidsMax(int numPids, App app)
{
    if (numPids == MAX_NUM_PIDS) {
        CGroupControl::setValue(controllerName_, fileNamesMap_.at(MAX), "max", app);
    } else {
        CGroupControl::setValue(controllerName_, fileNamesMap_.at(MAX), numPids, app);
    }
}

int PidsControl::getPidsMax(App app)
{
    std::string svalue = CGroupControl::getValue(fileNamesMap_.at(MAX), app);
    if (svalue == "max") {
        return MAX_NUM_PIDS;
    } else {
        return static_cast<int>(strtol(svalue.c_str(), nullptr, 10));
    }
}

int PidsControl::getPidsCurrent(App app)
{
    return CGroupControl::getValueAsInt(fileNamesMap_.at(CURRENT), app);
}

}   // namespace pc
