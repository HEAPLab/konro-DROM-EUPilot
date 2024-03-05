#ifndef DROMCPUSETCONTROL_H
#define DROMCPUSETCONTROL_H

#include "../icpusetcontrol.h"
#include "app.h"
#include "cpuguard.h"
#include "cputracker.h"
#include "drom/dromcontrol.h"
#include <hwloc.h>
#include <memory>
#include <sys/types.h>

namespace pc {

struct CPUTracker;

/*!
 * \class a class for interacting with the cgroup cpuset controller
 */
class DromCpusetControl : public ICpusetControl, DromControl {

  friend CPUGuard;
  friend CPUTracker;
  CPUTracker cpuTracker;

  DromCpusetControl(int ncpu);

  /*!
   * Requests the use of a set of processing units by the application.
   * \param cpus the vector of requested processing units
   * \param app the application to limit
   */
  void setCpus(const rmcommon::CpusetVector &cpus,
               std::shared_ptr<rmcommon::App> app) override;

  /*!
   * Returns the list of processing units that are granted to the specified
   * application as a vector of pairs.
   * \param app the application of interest
   * \returns the processing units available for use by the application
   */
  rmcommon::CpusetVector
  getCpusEffective(std::shared_ptr<rmcommon::App> app) override;

  /*!
   * Requests the use of a set of memory nodes by the application.
   * \param memNodes the list of requested memory nodes
   * \param app the application to limit
   */
  void setMems(const rmcommon::CpusetVector &memNodes,
               std::shared_ptr<rmcommon::App> app) override;

  /*!
   * Returns the list of memory nodes that are requested by the specified
   * application as a vector of pairs. \param app the application of interest
   * \returns the memory nodes requested for use by the application
   */
  rmcommon::CpusetVector getMems(std::shared_ptr<rmcommon::App> app) override;

  /*!
   * Returns the list of memory nodes that are granted to the specified
   * application as a vector of pairs. \param app the application of interest
   * \returns the memory nodes available for use by the application
   */
  rmcommon::CpusetVector
  getMemsEffective(std::shared_ptr<rmcommon::App> app) override;

public:
  void print_drom_list() const;

  static DromCpusetControl &instance(int ncpu);

  /*!
   * Returns the list of processing units that are requested by the specified
   * application as a vector of pairs.
   * \param app the application of interest
   * \returns the processing units requested for use by the application
   */
  rmcommon::CpusetVector getCpus(std::shared_ptr<rmcommon::App> app) override;

  /*! Release all resource assigned to \p app */
  void release(std::shared_ptr<rmcommon::App> app);

  /*! Reserve ncpu to be finer refined by the user and after required by
   * invoking apply resources are guarded in a RAI style*/
  pc::CPUGuard reserveCpus(std::shared_ptr<rmcommon::App> app, int ncpu);
};

} // namespace pc
#endif // DROMCPUSETCONTROL_H
