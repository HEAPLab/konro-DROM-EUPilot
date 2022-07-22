#ifndef CGROUPCONTROL_H
#define CGROUPCONTROL_H

#include "iplatformcontrol.h"
#include "ecgroup.h"
#include "app.h"

#include <string>
#include <unistd.h>

namespace pc {

/*!
 * \class a class for interacting with the cgroup hieararchy
 */
class CGroupControl : public IPlatformControl {

public:
    CGroupControl();
    virtual ~CGroupControl();

    /*!
     * \brief Enforces a resource constraint on the specified application.
     *
     * This is done by writing a value to the specified
     * controller interface file.
     *
     * \example setValue(CPUSET_CPUS, 2, 145)
     *
     * \param controller the file to write to
     * \param value the value to write
     * \param app the application to limit
     */
    void setValue(EcGroup::ECGROUP controller, const std::string &value, App app);

    void setValue(const std::string &controllerName, const std::string &fileName, const std::string &value, App app);

    /*!
     * \brief Returns the current limit applied to an application for a specific resource.
     *
     * This is done by reading the content of a controller interface file associated
     * to the specified application.
     *
     * \example
     *
     * \param controller the file to read
     * \param app the application of interest
     * \returns the content of the controller interface file
     */
    std::string getValue(EcGroup::ECGROUP controller, App app);
};

}
#endif // CGROUPCONTROL_H
