#ifndef CPUSETCONTROL_H
#define CPUSETCONTROL_H

#include "cgroupcontrol.h"
#include <string>
#include <map>

namespace pc {
/*!
 * \class a class for interacting with the cgroup cpuset controller
 */
class CpusetControl : public CGroupControl {
public:
    enum ControllerFile {
        CPUS,
        CPUS_EFFECTIVE,
        MEMS,
        MEMS_EFFECTIVE
    };
private:
    static const char *controllerName_;
    static const std::map<ControllerFile, const char *> fileNamesMap_;
public:

    template<typename T>
    void setValue(ControllerFile controllerFile, T value, App app) const {
        CGroupControl::setValue(controllerName_, fileNamesMap_.at(controllerFile), value, app);
    }

    void getValue(ControllerFile controllerFile, const std::string &value, App app) const {
        CGroupControl::getValue(fileNamesMap_.at(controllerFile), app);
    }

    void getValueAsInt(ControllerFile controllerFile, const std::string &value, App app) const {
        CGroupControl::getValueAsInt(fileNamesMap_.at(controllerFile), app);
    }
};

}   // namespace pc
#endif // CPUSETCONTROL_H
