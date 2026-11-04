#ifndef TALPDROMPOLICY_H
#define TALPDROMPOLICY_H

#include "dromrandpolicy.h"
#include <map>
#include <sys/types.h>

namespace rp {

/*!
 * \class DROM policy driven by the performance metrics measured by TALP.
 *
 * On every timer event it reads from TALP how many CPUs each application
 * kept busy with useful work and computes its parallel efficiency
 * (useful CPUs / assigned CPUs):
 *  - efficiency >= highEfficiency: the app uses all its CPUs -> one CPU more
 *  - efficiency <  lowEfficiency:  CPUs are wasted -> one CPU less, and no
 *    growth for the next backoffPeriods timer events (avoids oscillations)
 *  - otherwise the allocation is kept.
 * Feedback messages sent by the applications are not needed and are ignored.
 *
 * Thresholds are read from the [talp] section of the configuration file.
 */
class TalpDromPolicy : public DromRandPolicy {
  /*! per-application state of the policy */
  struct AppState {
    int backoff = 0;    // timer periods left before the app may grow again
  };

  const AppMappingSet &apps_;
  std::map<pid_t, AppState> state_;
  double lowEfficiency_;
  double highEfficiency_;
  int backoffPeriods_;

  /*! returns the number of CPUs currently assigned to the application */
  int assignedCpus(std::shared_ptr<rmcommon::App> app);

public:
  TalpDromPolicy(const AppMappingSet &apps, PlatformDescription pd);

  // IBasePolicy interface
  virtual const char *name() override { return "TalpDromPolicy"; }
  virtual void removeApp(AppMappingPtr appMapping) override;
  virtual void timer() override;
  virtual void feedback(AppMappingPtr appMapping, int feedback) override;
};

} // namespace rp

#endif // TALPDROMPOLICY_H
