#include "memorycontrol.h"

namespace pc {

/*static*/
const char *MemoryControl::controllerName_ = "memory";

/*static*/
const std::map<MemoryControl::ControllerFile, const char *> MemoryControl::fileNamesMap_ = {
    { CURRENT, "memory.current" },
    { MIN, "memory.min" },
    { MAX, "memory.max" },
    { EVENTS, "memory.events" },
    { STAT, "memory.stat" }
};

}   // namespace pc
