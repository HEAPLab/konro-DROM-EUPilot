#ifndef BASEEVENT_H
#define BASEEVENT_H

#include <iostream>

namespace rmcommon {

/*!
 * \class a generic event related to a process managed by Konro
 */
class BaseEvent {
public:
    virtual void printOnOstream(std::ostream &os) const {
        std::cout << "BaseEvent\n";
    }

    friend std::ostream &operator <<(std::ostream &os, const BaseEvent &event) {
        event.printOnOstream(os);
        return os;
    }
};

}   // namespace rmcommon


#endif // BASEEVENT_H
