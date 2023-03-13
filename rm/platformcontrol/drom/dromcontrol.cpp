#include "dromcontrol.h"
#include <log4cpp/Priority.hh>

bool pc::DromControl::addApplication(std::shared_ptr<rmcommon::App> app)  {
    cat_.log(log4cpp::Priority::DEBUG, "Drom add aplication");
    return true;
}

bool pc::DromControl::removeApplication(std::shared_ptr<rmcommon::App> app) {
        cat_.log(log4cpp::Priority::DEBUG, "Drom Remove aplication");
    return true;
}