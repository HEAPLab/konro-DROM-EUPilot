#ifndef ADDPROCEVENT_H
#define ADDPROCEVENT_H

#include "baseevent.h"
#include "app.h"
#include <memory>
#include <iostream>

class AddProcEvent : public BaseEvent {

    std::shared_ptr<pc::App> app_;

public:

    AddProcEvent(std::shared_ptr<pc::App> app) : app_(app) {}

    void printOnOstream(std::ostream &os) const override {
        os << "AddProcEvent for PID " << app_->getPid() << std::endl;
    }

    std::shared_ptr<pc::App> getApp() const {
        return app_;
    }

};

#endif // ADDPROCEVENT_H
