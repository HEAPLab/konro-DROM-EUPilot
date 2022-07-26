#include "iocontrol.h"

namespace pc {

/*static*/
const char *IOControl::controllerName_ = "io";

/*static*/
const std::map<IOControl::ControllerFile, const char *> IOControl::fileNamesMap_ = {
    { STAT, "io.stat" },
    { MAX, "io.max" }
};

}   // namespace pc
