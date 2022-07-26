#include "pidscontrol.h"

namespace pc {

/*static*/
const char *PidsControl::controllerName_ = "pids";

/*static*/
const std::map<PidsControl::ControllerFile, const char *> PidsControl::fileNamesMap_ = {
    { MAX, "pids.max" },
    { CURRENT, "pids.current" }
};

}   // namespace pc
