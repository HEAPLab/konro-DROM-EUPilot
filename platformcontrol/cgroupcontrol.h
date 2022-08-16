#ifndef CGROUPCONTROL_H
#define CGROUPCONTROL_H

#include "iplatformcontrol.h"
#include "app.h"
#include "cgrouputil.h"

#include <string>
#include <vector>
#include <map>
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

        // 2 - If the controller interface file doesn't exist, activate the controller
        std::string filePath = make_path(cgroupPath, fileName);
        if (!util::fileExists(filePath.c_str())) {
            util::activateController(controllerName, cgroupPath);
        }
        // 3 - Write value in the correct cgroup
        util::writeValue(fileName, value, cgroupPath);
    }

    /*!
     * \brief Enforces a resource constraint on all konro applications
     * \param controllerName the type of resource to limit
     * \param fileName the file to write to
     * \param value the value to write
     * \throws PcException in case of error
     */
    template<typename T>
    void setValue(const char *controllerName, const char *fileName, T value) const {
        // 1 - Find cgroup konro path
        std::string cgroupKonroBaseDir = util::getCgroupKonroBaseDir();

        // 2 - Write value in the correct cgroup
        util::writeValue(fileName, value, cgroupKonroBaseDir);
    }

    /*!
     * \brief Returns the current limit applied to an application for a specific resource.
     *
     * This is done by reading the content of a controller interface file associated
     * to the specified application.
     *
     * \example getLine("cpuset", "cpuset.cpus.effective", app1)
     *          Returns the CPUs granted for use to app1
     *
     * \param fileName the file to read
     * \param app the application of interest
     * \returns the content of the controller interface file
     * \throws PcException in case of error
     */
    std::string getLine(const char *fileName, App app) const;

    /*!
     * \brief Returns the content of a specified controller interface file.
     *
     * This function should be used to retrieve the content of a multi-line
     * file, such as cpu.stat.
     *
     * \param fileName the file to read
     * \param app the application of interest
     * \returns the content of the controller interface file
     * \throws PcException in case of error
     */
    std::vector<std::string> getContent(const char *fileName, App app) const;

    /*!
     * \brief Returns the current limit applied to an application for a specific resource
     *        as an integer.
     * \param fileName the file to read
     * \param app the application of interest
     * \returns the content of the controller interface file
     * \throws PcException in case of error
     */
    int getValueAsInt(const char *fileName, App app) const;

    std::map<std::string, unsigned long> getContentAsMap(const char *fileName, App app);

    /*!
     * \brief Adds an application under the management of Konro.
     *
     * The PID of the application is moved to a new direcotry of the cgroup hieararchy.
     * The name name of the new directory is app-<PID>.scope and is located at
     * /sys/fs/cgroup/konro.slice.
     *
     * \param app the application to manage
     */
    void addApplication(App app);

    /*!
     * \brief Removes an application from the management of Konro.
     *
     * TBD
     *
     * \param app the application to manage
     */
    void removeApplication(App app);
};

}
#endif // CGROUPCONTROL_H
