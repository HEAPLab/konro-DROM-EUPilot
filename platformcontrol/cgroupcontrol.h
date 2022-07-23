#ifndef CGROUPCONTROL_H
#define CGROUPCONTROL_H

#include "iplatformcontrol.h"
#include "app.h"
#include "cgrouputil.h"

#include <string>
#include <unistd.h>

namespace pc {

/*!
 * \class a class for interacting with the cgroup hieararchy
 */
class CGroupControl : public IPlatformControl {

public:
    virtual ~CGroupControl();

    /*!
     * \brief Enforces a resource constraint on the specified application.
     *
     * This is done by writing a value to the specified
     * controller interface file.
     *
     * \example setValue("cpuset", "cpuset.cpus", "2", app1)
     *          Limits app1 to use CPU number 2 only
     *
     * \param controllerName the type of resource to limit
     * \param fileName the file to write to
     * \param value the value to write
     * \param app the application to limit
     * \throws PcException in case of error
     */
    template<typename T>
    void setValue(const char *controllerName, const char *fileName, T value, App app) const {
        // 1 - Find cgroup path
        std::string cgroupPath = util::findCgroupPath(app.getPid());

        // 2 - Activate controller
        util::activateController(controllerName, cgroupPath);

        // 3 - Write value in the correct cgroup
        util::writeValue(fileName, value, cgroupPath);
    }

    /*!
     * \brief Returns the current limit applied to an application for a specific resource.
     *
     * This is done by reading the content of a controller interface file associated
     * to the specified application.
     *
     * \example getValue("cpuset", "cpuset.cpus.effective", app1)
     *          Returns the CPUs granted for use to app1
     *
     * \param fileName the file to read
     * \param app the application of interest
     * \returns the content of the controller interface file
     * \throws PcException in case of error
     */
    std::string getValue(const char *fileName, App app) const;

    int getValueAsInt(const char *fileName, App app) const;
};

}
#endif // CGROUPCONTROL_H
