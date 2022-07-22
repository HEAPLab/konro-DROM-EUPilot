#ifndef CPUCONTROL_H
#define CPUCONTROL_H

#include "cgroupcontrol.h"
#include <string>

namespace pc {

class CpuControl : public CGroupControl {
    static const char *fileNames_[];
public:
    enum ControllerFile {
        // read-write
        WEIGHT,
        MAX,
        MAX_BURST,
        // read-only
        STAT
    };

    void setValue(ControllerFile controllerFile, const std::string &value, App app) {
        CGroupControl::setValue("cpu", fileNames_[controllerFile], value, app);
    }
};

}   // namespace pc
#endif // CPUCONTROL_H
