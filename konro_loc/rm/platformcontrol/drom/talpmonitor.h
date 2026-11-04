#ifndef TALPMONITOR_H
#define TALPMONITOR_H

#include <chrono>
#include <log4cpp/Category.hh>
#include <map>
#include <optional>
#include <sys/types.h>

namespace pc {

/*!
 * \class reads the performance metrics that TALP (DLB) publishes for the
 * applications managed by Konro.
 *
 * Applications must run with DLB_ARGS="--talp=openmp --talp-external-profiler"
 * and publish their metrics periodically with DLB_MonitoringRegionsUpdate():
 * each publication is a heartbeat that Konro can observe.
 * Used only by the policy thread, so it needs no locking.
 */
class TalpMonitor {
public:
  /*! Returns the single instance, attached to TALP on first use */
  static TalpMonitor &instance();

  /*!
   * Returns the average number of CPUs that the process kept busy with
   * useful work since the previous call that returned a value, or
   * std::nullopt if no new TALP data was published in the meantime.
   */
  std::optional<double> usefulCpus(pid_t pid);

  /*! Discards the history of a process (to be called when it exits) */
  void forget(pid_t pid);

private:
  using Clock = std::chrono::steady_clock;

  /*! cumulative useful time of a process and when Konro read it */
  struct Sample {
    double usefulTime;
    Clock::time_point when;
  };

  TalpMonitor();
  ~TalpMonitor();
  TalpMonitor(const TalpMonitor &) = delete;
  TalpMonitor &operator=(const TalpMonitor &) = delete;

  log4cpp::Category &cat_;
  bool attached_;
  std::map<pid_t, Sample> last_;
};

} // namespace pc

#endif // TALPMONITOR_H
