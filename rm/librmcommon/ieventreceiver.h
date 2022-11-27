#ifndef IEVENTRECEIVER_H
#define IEVENTRECEIVER_H

#include "events/baseevent.h"
#include <memory>

namespace rmcommon {

class IEventReceiver {
public:
    virtual void addEvent(std::shared_ptr<BaseEvent> event) = 0;
};

}   // namespace rmcommon

#endif // IEVENTRECEIVER_H
