#include "dromcontrol.h"
#include <log4cpp/Priority.hh>

bool pc::DromControl::addApplication(
    [[maybe_unused]] std::shared_ptr<rmcommon::App> app) {
  cat_.log(log4cpp::Priority::DEBUG, "Drom add aplication");
  return true;
}

bool pc::DromControl::removeApplication(
    [[maybe_unused]] std::shared_ptr<rmcommon::App> app) {
  cat_.log(log4cpp::Priority::DEBUG, "Drom Remove aplication");
  return true;
}