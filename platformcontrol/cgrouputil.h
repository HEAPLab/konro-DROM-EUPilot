#ifndef CGROUPUTIL_H
#define CGROUPUTIL_H

#include "makepath.h"
#include <string>
#include <fstream>
#include <unistd.h>

namespace pc {
namespace util {

void throwCouldNotOpenFile(std::string funcName, std::string fileName);

/*!
 * \brief Finds the location in the cgroup hierarchy of the specified process
 * \param pid the pid of the process
 * \returns the absolute path of the cgroup to which the process belongs
 */
std::string findCgroupPath(pid_t pid);

/*!
 * \brief Enables control of a specified resource type for the processes
 *        inside the target directory
 * \param controllerName the type of controller to activate
 * \param cgroupPath the target directory
 */
void activateController(const char *controllerName, const std::string &cgroupPath);

/*!
 * \brief Writes a value to the specified cgroup interface file
 * \param fileName the file to write
 * \param value the value to write
 * \param cgroupPath the target directory
 */
template<typename T>
void writeValue(const char *fileName, T value, std::string cgroupPath) {
    std::string filePath = make_path(cgroupPath, fileName);
    std::ofstream fileStream(filePath.c_str());
    if (!fileStream.is_open()) {
        throwCouldNotOpenFile(__func__, filePath);
    }
    fileStream << value;
    fileStream.close();
}

/*!
 * \brief Gets the content of a specified cgroup interface file
 * \param fileName the file to read
 * \param cgroupPath the directory where the file is located
 * \returns the content of the file
 */
std::string getValue(const char *fileName, std::string cgroupPath);

/*!
 * \brief Creates a new cgroup
 * \param cgroupPath the parent directory to the new cgroup
 * \param name the name of the cgroup to create
 * \returns the path of the new cgroup
 */
std::string createCgroup(std::string cgroupPath, std::string name);

/*!
 * \brief Moves a process to the specified cgroup directory
 * \param cgroupPath the cgroup directory where to move the process
 * \param pid the pid of the process to move
 */
void moveToCgroup(std::string cgroupPath, pid_t pid);

}   // namespace util
}   // namespace pc

#endif // CGROUPUTIL_H
