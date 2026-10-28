#include "talpmonitor.h"
#include <dlb.h>
#include <dlb_errors.h>
#include <dlb_talp.h>

namespace pc {

TalpMonitor &TalpMonitor::instance() {
  static TalpMonitor monitor;
  return monitor;
}

TalpMonitor::TalpMonitor()
    : cat_(log4cpp::Category::getRoot()), attached_(false) {
  int err = DLB_TALP_Attach();
  attached_ = (err == DLB_SUCCESS);
  if (attached_) {
    cat_.info("TALPMONITOR attached to TALP");
  } else {
    cat_.error("TALPMONITOR could not attach to TALP: %s", DLB_Strerror(err));
  }
}

std::optional<double> TalpMonitor::usefulCpus(pid_t pid) {
  if (!attached_)
    return std::nullopt;

  double mpiTime = 0.0, usefulTime = 0.0;
  int err = DLB_TALP_GetTimes(pid, &mpiTime, &usefulTime);
  if (err != DLB_SUCCESS) {
    cat_.debug("TALPMONITOR no TALP data for pid %d: %s", static_cast<int>(pid),
               DLB_Strerror(err));
    return std::nullopt;
  }

  const Clock::time_point now = Clock::now();
  auto it = last_.find(pid);
  if (it == last_.end()) {
    // first observation: only store the baseline
    last_[pid] = {usefulTime, now};
    return std::nullopt;
  }
  if (usefulTime <= it->second.usefulTime) {
    // no heartbeat since the previous read
    return std::nullopt;
  }

  double elapsed = std::chrono::duration<double>(now - it->second.when).count();
  double cpus = (usefulTime - it->second.usefulTime) / elapsed;
  it->second = {usefulTime, now};
  return cpus;
}

void TalpMonitor::forget(pid_t pid) { last_.erase(pid); }

} // namespace pc