#ifndef DROMCPUSETCONTROL_H
#define DROMCPUSETCONTROL_H

#include "../cgroupcontrol.h"
#include "../icpusetcontrol.h"
#include "drom/dromcontrol.h"
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace pc {
/*!
 * \class a class for interacting with the cgroup cpuset controller
 */
class DromCpusetControl : public ICpusetControl, DromControl {

  DromCpusetControl();

public:
  void print_drom_list() const;

  static DromCpusetControl &instance();
  /*!
   * Requests the use of a set of processing units by the application.
   * \param cpus the vector of requested processing units
   * \param app the application to limit
   */
  void setCpus(const rmcommon::CpusetVector &cpus,
               std::shared_ptr<rmcommon::App> app) override;

  /*!
   * Returns the list of processing units that are requested by the specified
   * application as a vector of pairs.
   * \param app the application of interest
   * \returns the processing units requested for use by the application
   */
  rmcommon::CpusetVector getCpus(std::shared_ptr<rmcommon::App> app) override;

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
};

} // namespace pc
#endif // DROMCPUSETCONTROL_H
