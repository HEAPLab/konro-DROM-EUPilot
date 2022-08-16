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

void PidsControl::setPidsMax(NumericValue numPids, std::shared_ptr<App> app)
{
    setValue(controllerName_, fileNamesMap_.at(MAX), numPids, app);
}

NumericValue PidsControl::getPidsMax(std::shared_ptr<App> app)
{
    return getLine(fileNamesMap_.at(MAX), app);
}

NumericValue PidsControl::getPidsCurrent(std::shared_ptr<App> app)
{
    return getValueAsInt(fileNamesMap_.at(CURRENT), app);
}

}   // namespace pc
