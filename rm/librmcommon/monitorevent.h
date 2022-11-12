#ifndef MONITOREVENT_H
#define MONITOREVENT_H

#include "baseevent.h"
#include "platformpower.h"
#include "platformtemperature.h"
#include <vector>

namespace rmcommon {

/*!
 * \class event generated by the periodical sampling of machine status information.
 */
class MonitorEvent : public BaseEvent {
    PlatformTemperature platTemp_;
    PlatformPower platPower_;
public:
    MonitorEvent(PlatformTemperature temp, PlatformPower power);
    virtual ~MonitorEvent() = default;

    PlatformTemperature getPlatformTemperature() {
        return platTemp_;
    }

    PlatformPower getPlatformPower() {
        return platPower_;
    }

    void printOnOstream(std::ostream &os) const override;
};

}   // namespace rmcommon

#endif // MONITOREVENT_H
