#ifndef TALPDROMPOLICY_H
#define TALPDROMPOLICY_H

#include "dromrandpolicy.h"
#include <sys/types.h>

namespace rp {

/*!
 * \class DROM policy driven by the performance metrics measured by TALP.
 *
 * Step 1 (observation only): on every timer event it reads from TALP how
 * many CPUs each application kept busy with useful work and logs its
 * parallel efficiency (useful CPUs / assigned CPUs). CPU allocation is
 * still driven by the application feedback, as in DromRandPolicy.
 */
class TalpDromPolicy : public DromRandPolicy {
  const AppMappingSet &apps_;

  /*! returns the number of CPUs currently assigned to the application */
  int assignedCpus(std::shared_ptr<rmcommon::App> app);

public:
  TalpDromPolicy(const AppMappingSet &apps, PlatformDescription pd);

  // IBasePolicy interface
  virtual const char *name() override { return "TalpDromPolicy"; }
  virtual void removeApp(AppMappingPtr appMapping) override;
  virtual void timer() override;
};

} // namespace rp

#endif // TALPDROMPOLICY_H