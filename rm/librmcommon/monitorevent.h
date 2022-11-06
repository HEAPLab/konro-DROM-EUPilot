#ifndef MONITOREVENT_H
#define MONITOREVENT_H

#include "baseevent.h"
#include "coretemperature.h"
#include <vector>

namespace rmcommon {

class MonitorEvent : public BaseEvent {
    // Temperature information
    std::vector<CoreTemperature> cpuTemp_;
public:
    MonitorEvent();

    void addCoreTemperature(const CoreTemperature &ct) {
        cpuTemp_.push_back(ct);
    }

    void printOnOstream(std::ostream &os) const override;
};

}   // namespace rmcommon

#endif // MONITOREVENT_H
