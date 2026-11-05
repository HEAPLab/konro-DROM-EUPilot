#include "talpdrompolicy.h"
#include "config.h"
#include "drom/talpmonitor.h"
#include <log4cpp/Category.hh>
#include <optional>

namespace rp {

namespace {

/*! Reads a value from the [talp] section of the configuration file */
double readTalpConfig(const char *key, double defaultValue) {
  try {
    return konro::Config::get().read<double>("talp", key);
  } catch (...) {
    return defaultValue;
  }
}

} // namespace

TalpDromPolicy::TalpDromPolicy(const AppMappingSet &apps, PlatformDescription pd)
    : DromRandPolicy(pd), apps_(apps),
      lowEfficiency_(readTalpConfig("lowefficiency", 0.6)),
      highEfficiency_(readTalpConfig("highefficiency", 0.8)),
      backoffPeriods_(static_cast<int>(readTalpConfig("backoffperiods", 5))) {
  log4cpp::Category &cat = log4cpp::Category::getRoot();
  if (!(0.0 <= lowEfficiency_ && lowEfficiency_ < highEfficiency_) ||
      backoffPeriods_ < 0) {
    cat.warn("TALPDROMPOLICY invalid [talp] configuration, using the defaults");
    lowEfficiency_ = 0.6;
    highEfficiency_ = 0.8;
    backoffPeriods_ = 5;
  }
  // attach to TALP as soon as Konro starts
  pc::TalpMonitor::instance();
  cat.info(
      "TALPDROMPOLICY lowEfficiency=%.2f highEfficiency=%.2f backoffPeriods=%d",
      lowEfficiency_, highEfficiency_, backoffPeriods_);
}

int TalpDromPolicy::assignedCpus(std::shared_ptr<rmcommon::App> app) {
  int n = 0;
  for (const auto &range : cpuSetControl.getCpus(app)) {
    n += range.second - range.first + 1;
  }
  return n;
}

void TalpDromPolicy::removeApp(AppMappingPtr appMapping) {
  pid_t pid = appMapping->getPid();
  pc::TalpMonitor::instance().forget(pid);
  state_.erase(pid);
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

    AppState &st = state_[pid];
    const char *decision = "keep";
    if (efficiency < lowEfficiency_ && ncpus > 1) {
      cpuSetControl.reserveCpus(app, ncpus - 1);
      st.backoff = backoffPeriods_;
      decision = "remove 1 CPU";
    } else if (efficiency >= highEfficiency_ && st.backoff == 0 &&
               cpuSetControl.getFreeCpus() > 0) {
      cpuSetControl.reserveCpus(app, ncpus + 1);
      decision = "add 1 CPU";
    } else if (st.backoff > 0) {
      --st.backoff;
    }

    cat.info("TALPDROMPOLICY pid=%d cpus=%d usefulCpus=%.2f efficiency=%.2f -> %s",
             static_cast<int>(pid), ncpus, *useful, efficiency, decision);
  }
}

void TalpDromPolicy::feedback(AppMappingPtr appMapping, int feedback) {
  // decisions are based on TALP: the application feedback is only recorded
  appMapping->setLastFeedback(feedback);
}

} // namespace rp
