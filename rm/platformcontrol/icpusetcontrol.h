#ifndef ICPUSETCONTROL_H
#define ICPUSETCONTROL_H

#include "numericvalue.h"
#include "app.h"

#include <vector>

namespace pc {

/*!
 * \interface an interface describing the behavior of a cpuset controller
 */
class ICpusetControl {
public:

    /* Vector of cpu numbers pairs */
    typedef std::vector<std::pair<short,short>> CpusetVector;

    /*!
     * Requests the use of a set of cpus by the application.
     * \param cpus the vector of requested cpus
     * \param app the application to limit
     */
    virtual void setCpusetCpus(const CpusetVector &cpus, std::shared_ptr<App> app) = 0;

    /*!
     * Returns the list of cpus that are requested by the specified application
     * as a vector of pairs.
     * \param app the application of interest
     * \returns the cpus requested for use by the application
     */
    virtual CpusetVector getCpusetCpus(std::shared_ptr<App> app) = 0;

    /*!
     * Returns the list of cpus that are granted to the specified application
     * as a vector of pairs.
     * \param app the application of interest
     * \returns the cpus available for use by the application
     */
    virtual CpusetVector getCpusetCpusEffective(std::shared_ptr<App> app) = 0;

    /*!
     * Requests the use of a set of memory nodes by the application.
     * \param memNodes the list of requested memory nodes
     * \param app the application to limit
     */
    virtual void setCpusetMems(const CpusetVector &memNodes, std::shared_ptr<App> app) = 0;

    /*!
     * Returns the list of memory nodes that are requested by the specified application
     * as a vector of pairs.
     * \param app the application of interest
     * \returns the memory nodes requested for use by the application
     */
    virtual CpusetVector getCpusetMems(std::shared_ptr<App> app) = 0;

    /*!
     * Returns the list of memory nodes that are granted to the specified application
     * as a vector of pairs.
     * \param app the application of interest
     * \returns the memory nodes available for use by the application
     */
    virtual CpusetVector getCpusetMemsEffective(std::shared_ptr<App> app) = 0;

};

}
#endif // ICPUSETCONTROL_H
