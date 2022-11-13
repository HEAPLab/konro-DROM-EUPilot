#ifndef TIMEREVENT_H
#define TIMEREVENT_H

#include "baseevent.h"

namespace rmcommon {

/*!
 * \class empty event periodically generated by a timer thread.
 * This type of event can be used to wake up the resource management policy
 * at a fixed temporal interval.
 */
class TimerEvent : public BaseEvent {
    void printOnOstream(std::ostream &os) const override {
        os << "TimerEvent {}";
    }
};

}

#endif // TIMEREVENT_H
