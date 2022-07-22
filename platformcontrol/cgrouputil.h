#ifndef CGROUPUTIL_H
#define CGROUPUTIL_H

#include "ecgroup.h"

#include <string>
#include <unistd.h>

namespace pc {
namespace util {

/*!
 * \brief Finds the location in the cgroup hierarchy of the specified process
 * \param pid the pid of the process
 * \returns the absolute path of the cgroup to which the process belongs
 */
std::string findCgroupPath(pid_t pid);


void activateController(const std::string &controllerName, const std::string &cgroupPath);

/*!
 * \brief Enables control of a specified resource type for the processes
 *        inside the target directory
 * \param controller the type of controller to activate
 * \param cgroupPath the target directory
 */
void activateController(EcGroup::ECGROUP controller, std::string cgroupPath);

void writeValue(const std::string &fileName, const std::string &value, std::string cgroupPath);

/*!
 * \brief Writes a value in the specified cgroup interface file
 * \param controller the file to write
 * \param value the value to write
 * \param cgroupPath the target directory
 */
void writeValue(EcGroup::ECGROUP controller, const std::string &value, std::string cgroupPath);

/*!
 * \brief Gets the content of a specified cgroup interface file
 * \param controller the file to read
 * \param cgroupPath the directory where the file is located
 * \returns the content of the file
 */
std::string getValue(EcGroup::ECGROUP controller, std::string cgroupPath);

/*!
 * \brief Creates a new cgroup
 * \param cgroupPath the parent directory to the new cgroup
 * \param name the name of the cgroup to create
 * \returns the path of the new cgroup
 */
std::string createCgroup(std::string cgroupPath, std::string name);

/*!
 * \brief Adds a process to the specified cgroup directory
 * \param cgroupPath the cgroup directory where to move the process
 * \param pid the pid of the process to move
 */
void addToCgroup(std::string cgroupPath, pid_t pid);

}   // namespace util
}   // namespace pc

#endif // CGROUPUTIL_H
