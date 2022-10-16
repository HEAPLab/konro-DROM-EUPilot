#ifndef REMOVEPROCEVENT_H
#define REMOVEPROCEVENT_H

#include "baseevent.h"
#include "app.h"
#include <iostream>

class RemoveProcEvent : public BaseEvent {

    std::shared_ptr<pc::App> app_;

public:

    RemoveProcEvent(std::shared_ptr<pc::App> app) : app_(app) {}

    void printOnOstream(std::ostream &os) const override {
        os << "RemoveProcEvent for PID " << app_->getPid() << std::endl;
    }

    std::shared_ptr<pc::App> getApp() const {
        return app_;
    }

};

#endif // REMOVEPROCEVENT_H
