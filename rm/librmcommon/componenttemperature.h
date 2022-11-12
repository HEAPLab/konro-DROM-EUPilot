#ifndef COMPONENTTEMPERATURE_H
#define COMPONENTTEMPERATURE_H

#include <string>
#include <iostream>

namespace rmcommon {

/*!
 * \brief encapsulates temperature information about a machine component.
 * A component can be either a Package or a CPU.
 */
struct ComponentTemperature {
    int num_;
    char *label_;
    int temp_;
    int maxTemp_;
    int minTemp_;
    int highestTemp_;
    int lowestTemp_;
    int critTemp_;
    int emergencyTemp_;

    explicit ComponentTemperature() {
        num_ = temp_ = maxTemp_ = critTemp_ = -1;
    }

    friend std::ostream &operator << (std::ostream &os, const ComponentTemperature &ct) {
        os << "ComponentTemperature: "
           << ct.label_
           << ", temp=" << ct.temp_
           << ", max temp=" << ct.maxTemp_
           << ", crit temp=" << ct.critTemp_;
        return os;
    }
};

}   // namespace rmcommon

#endif // COMPONENTTEMPERATURE_H
