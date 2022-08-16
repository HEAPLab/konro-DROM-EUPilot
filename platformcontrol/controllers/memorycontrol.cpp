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

int MemoryControl::getMemoryCurrent(std::shared_ptr<App> app)
{
    return getValueAsInt(fileNamesMap_.at(CURRENT), app);
}

void MemoryControl::setMemoryMin(int minMem, std::shared_ptr<App> app)
{
    setValue(controllerName_, fileNamesMap_.at(MIN), minMem, app);
}

int MemoryControl::getMemoryMin(std::shared_ptr<App> app)
{
    return getValueAsInt(fileNamesMap_.at(MIN), app);
}

void MemoryControl::setMemoryMax(NumericValue maxMem, std::shared_ptr<App> app)
{
    setValue(controllerName_, fileNamesMap_.at(MAX), maxMem, app);
}

NumericValue MemoryControl::getMemoryMax(std::shared_ptr<App> app)
{
    return getLine(fileNamesMap_.at(MAX), app);
}

std::map<std::string, unsigned long> MemoryControl::getMemoryEvents(std::shared_ptr<App> app)
{
    return getContentAsMap(fileNamesMap_.at(EVENTS), app);
}

std::map<std::string, unsigned long> MemoryControl::getMemoryStat(std::shared_ptr<App> app)
{
    return getContentAsMap(fileNamesMap_.at(STAT), app);
}

}   // namespace pc
