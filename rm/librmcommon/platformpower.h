#ifndef PLATFORMPOWER_H
#define PLATFORMPOWER_H

#include <iostream>

namespace rmcommon {
/*!
 * \class encapsulates power information about the machine
 */
class PlatformPower {
    int batteryCurrent_;
    int batteryVoltage_;
public:

    void setBatteryCurrent(int val) {
        batteryCurrent_ = val;
    }

    void setBatteryVoltage(int val) {
        batteryVoltage_ = val;
    }

    friend std::ostream &operator << (std::ostream &os, const PlatformPower &pp) {
        os << "{";
        os << "\"batteryCurrent\":" << pp.batteryCurrent_;
        os << ",\"batteryVoltage\":" << pp.batteryVoltage_;
        os << "}";
        return os;
    }
};

}   // namespace rmcommon

#endif // PLATFORMPOWER_H
