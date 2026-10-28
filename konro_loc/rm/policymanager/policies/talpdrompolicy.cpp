#include "talpdrompolicy.h"
#include "drom/talpmonitor.h"
#include <log4cpp/Category.hh>
#include <optional>

namespace rp {

TalpDromPolicy::TalpDromPolicy(const AppMappingSet &apps, PlatformDescription pd)
    : DromRandPolicy(pd), apps_(apps) {
  // attach to TALP as soon as Konro starts
  pc::TalpMonitor::instance();
}

int TalpDromPolicy::assignedCpus(std::shared_ptr<rmcommon::App> app) {
  int n = 0;
  for (const auto &range : cpuSetControl.getCpus(app)) {
    n += range.second - range.first + 1;
  }
  return n;
}

void TalpDromPolicy::removeApp(AppMappingPtr appMapping) {
  pc::TalpMonitor::instance().forget(appMapping->getPid());
  DromRandPolicy::removeApp(appMapping);
}

void TalpDromPolicy::timer() {
  log4cpp::Category &cat = log4cpp::Category::getRoot();

  for (const AppMappingPtr &appMapping : apps_) {
    auto app = appMapping->getApp();
    pid_t pid = app->getPid();

    std::optional<double> useful = pc::TalpMonitor::instance().usefulCpus(pid);
    if (!useful) {
      continue;   // no heartbeat from this app in the last period
    }
    int ncpus = assignedCpus(app);
    if (ncpus == 0) {
      continue;
    }
    double efficiency = *useful / ncpus;

    cat.info("TALPDROMPOLICY pid=%d cpus=%d usefulCpus=%.2f efficiency=%.2f",
             static_cast<int>(pid), ncpus, *useful, efficiency);
  }
}

} // namespace rp