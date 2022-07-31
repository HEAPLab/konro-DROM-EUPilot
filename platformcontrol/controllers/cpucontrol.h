#ifndef CPUCONTROL_H
#define CPUCONTROL_H

#include "../cgroupcontrol.h"
#include <string>
#include <map>
#include <iostream>
#include <sstream>


namespace pc {
/*!
 * \class a class for interacting with the cgroup cpu controller
 */
class CpuControl : public CGroupControl {
    const int period_ = 100000;
public:
    enum ControllerFile {
        WEIGHT,         // read-write
        MAX,            // read-write
        STAT            // read-only
    };

private:
    static const char *controllerName_;
    static const std::map<ControllerFile, const char *> fileNamesMap_;
public:

    void setCpuMax(int percentage, App app);
    int getCpuMax(App app);

    std::map<std::string, unsigned long> getCpuStat(App app);

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

template<>
inline void CpuControl::setValue<int>(ControllerFile controllerFile, int value, App app) const {
    if (controllerFile == MAX) {
        std::ostringstream os;
        // for cpu.max normalize value between 0 and period_
        os << ((value * period_) / 100) << ' ' << period_;
        CGroupControl::setValue(controllerName_, fileNamesMap_.at(controllerFile), os.str(), app);
    } else {
        CGroupControl::setValue(controllerName_, fileNamesMap_.at(controllerFile), value, app);
    }
}

}   // namespace pc
#endif // CPUCONTROL_H
