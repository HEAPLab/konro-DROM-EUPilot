#ifndef CORETEMPERATURE_H
#define CORETEMPERATURE_H

#include <string>
#include <iostream>

namespace rmcommon {

struct CoreTemperature {
    int num_;
    std::string label_;
    int temp_;
    int maxTemp_;
    int critTemp_;

    explicit CoreTemperature() {
        num_ = temp_ = maxTemp_ = critTemp_ = -1;
    }

    friend std::ostream &operator << (std::ostream &os, const CoreTemperature &ct) {
        os << "CoreTemperature: "
           << ct.label_
           << ", temp=" << ct.temp_
           << ", max temp=" << ct.maxTemp_
           << ", crit temp=" << ct.critTemp_;
        return os;
    }
};

}   // namespace rmcommon

#endif // CORETEMPERATURE_H
