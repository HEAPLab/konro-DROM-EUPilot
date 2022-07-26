#ifndef CPUCONTROL_H
#define CPUCONTROL_H

#include "cgroupcontrol.h"
#include <string>
#include <map>


namespace pc {
/*!
 * \class a class for interacting with the cgroup cpu controller
 */
class CpuControl : public CGroupControl {
public:
    enum ControllerFile {
        WEIGHT,         // read-write
        MAX,            // read-write
        MAX_BURST,      // read-write
        STAT            // read-only
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
#endif // CPUCONTROL_H
