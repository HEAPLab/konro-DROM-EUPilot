#ifndef PLATFORMPOWER_H
#define PLATFORMPOWER_H
namespace rmcommon {

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

};

}   // namespace rmcommon

#endif // PLATFORMPOWER_H
