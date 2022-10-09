#ifndef BASEEVENT_H
#define BASEEVENT_H

#include <iostream>

/*!
 * \class a generic event related to a process managed by Konro
 */
class BaseEvent {
    virtual void printOnOstream(std::ostream &os) const {
        std::cout << "BaseEvent\n";
    }

    friend std::ostream &operator <<(std::ostream &os, const BaseEvent &event) {
        event.printOnOstream(os);
        return os;
    }
};

#endif // BASEEVENT_H
