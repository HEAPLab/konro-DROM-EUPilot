#ifndef CGROUPCONTROL_H
#define CGROUPCONTROL_H

#include "app.h"
#include "cgrouputil.h"
#include "dir.h"

#include <string>
#include <vector>
#include <map>
#include <unistd.h>

namespace pc {

/*!
 * \class a class for interacting with the cgroup hieararchy
 */
class CGroupControl {

    /*!
     * Checks if the specified controller interface file exists in a folder.
     * If it doesn't, the function activates the proper cgroup controller in order
     * to spawn the file.
     * \param controllerName the type of resource of interest
     * \param fileName the file we want to check the existence of
     * \param cgroupPath the location of the file
     */
    void checkActivateController(const char *controllerName, const char *fileName, const std::string &cgroupPath) const;

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
    void setValue(const char *controllerName, const char *fileName, T value, std::shared_ptr<App> app) const {
        // 1 - Find cgroup path
        std::string cgroupPath = util::findCgroupPath(app->getPid());

        // 2 - If the controller interface file doesn't exist, activate the controller
        checkActivateController(controllerName, fileName, cgroupPath);

        // 3 - Write value in the correct cgroup
        util::writeValue(fileName, value, cgroupPath);
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
    std::string getLine(const char *controllerName, const char *fileName, std::shared_ptr<App> app) const;

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
    std::vector<std::string> getContent(const char *controllerName, const char *fileName, std::shared_ptr<App> app) const;

    /*!
     * \brief Returns the current limit applied to an application for a specific resource
     *        as an integer.
     * \param fileName the file to read
     * \param app the application of interest
     * \returns the content of the controller interface file as integer
     * \throws PcException in case of error
     */
    int getValueAsInt(const char *controllerName, const char *fileName, std::shared_ptr<App> app) const;

    /*!
     * \brief Returns the content of a specified controller interface file as a map.
     *
     * This function should be used to retrieve the content of a multi-line flat-keyed
     * file, such as memory.events.
     *
     * \param fileName the file to read
     * \param app the application of interest
     * \returns the content of the controller interface file as map
     * \throws PcException in case of error
     */
    std::map<std::string, uint64_t> getContentAsMap(const char *controllerName, const char *fileName, std::shared_ptr<App> app);

    /*!
     * \brief Adds an application under the management of Konro.
     *
     * The PID of the application is moved to a new direcotry of the cgroup hieararchy.
     * The name name of the new directory is app-<PID>.scope and is located at
     * /sys/fs/cgroup/konro.slice.
     *
     * \param app the application to manage
     */
    void addApplication(std::shared_ptr<App> app);

    /*!
     * \brief Removes an application from the management of Konro.
     *
     * This function only removes the app's directory from the cgroup hierarchy.
     * Hence, the caller should always ensure the app has terminated before calling
     * this function.
     *
     * \param app the application to remove from Konro's management
     */
    void removeApplication(std::shared_ptr<App> app);
};

}
#endif // CGROUPCONTROL_H
