#ifndef CGROUPUTIL_H
#define CGROUPUTIL_H

#include "makepath.h"
#include <string>
#include <vector>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


namespace pc {
namespace util {

void throwCouldNotOpenFile(const std::string &funcName, const std::string &fileName);

/*!
 * Gets the path of the root folder of the cgroup hierarchy
 * \returns the root folder's path
 */
std::string getCgroupBaseDir();

/*!
 * Gets the path of the base cgroup directory for all apps managed by Konro
 * \returns the Konro base folder's path
 */
std::string getCgroupKonroBaseDir();

/*!
 * Gets the path of the cgroup directory containing the specified process
 * \param pid the pid of the process
 * \returns the path
 */
std::string getCgroupAppBaseDir(pid_t pid);

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
void writeValue(const char *fileName, T value, const std::string &cgroupPath) {
    std::string filePath = make_path(cgroupPath, fileName);
    std::ofstream fileStream(filePath.c_str());
    if (!fileStream.is_open()) {
        throwCouldNotOpenFile(__func__, filePath);
    }
    fileStream << value;
    fileStream.close();
}

/*!
 * \brief Gets the value contained in a specified cgroup interface file
 * \note This function will only read the first line from the target file.
 *       Therefore, it's not suitable for retrieving the content of multi-line
 *       files such as cpu.stat.
 *       To read such files, use the getContent function instead.
 * \param fileName the file to read
 * \param cgroupPath the directory where the file is located
 * \returns the content of the file
 */
std::string getLine(const char *fileName, const std::string &cgroupPath);

/*!
 * \brief Gets the content of a specified cgroup interface file
 * \note This function reads the content of a multi-line file
 *       and returns a vector of lines.
 * \param fileName the file to read
 * \param cgroupPath the directory where the file is located
 * \returns the content of the file as a vector of strings
 */
std::vector<std::string> getContent(const char *fileName, const std::string &cgroupPath);

/*!
 * \brief Creates a new cgroup
 * \param cgroupPath the parent directory to the new cgroup
 * \param name the name of the cgroup to create
 * \returns the path of the new cgroup
 */
std::string createCgroup(std::string cgroupPath, const std::string &name);

/*!
 * \brief Moves a process to the specified cgroup directory
 * \param cgroupPath the cgroup directory where to move the process
 * \param pid the pid of the process to move
 */
void moveToCgroup(const std::string &cgroupPath, pid_t pid);

/*!
 * \brief Checks if the specified file exists
 * \param path the filename with path
 * \return
 */
bool fileExists(const char *path);

}   // namespace util
}   // namespace pc

#endif // CGROUPUTIL_H
