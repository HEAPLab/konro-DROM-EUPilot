#include "cgrouputil.h"
#include "tsplit.h"
#include "makepath.h"
#include "pcexception.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <cstring>
#include <sys/stat.h>

using namespace std;

namespace pc {
namespace util {

/*!
 * \brief Throws a PcException
 */
static void throwCouldNotOpenFile(string funcName,string fileName)
{
    ostringstream os;
    os << funcName << ": could not open file " << fileName << ": " << strerror(errno);
    throw PcException(os.str());
}

string findCgroupPath(pid_t pid)
{
    ostringstream os;
    os << "/proc/" << pid << "/cgroup";
    ifstream in(os.str());
    if (!in.is_open()) {
        throwCouldNotOpenFile(__func__, os.str());
    }
    string cgroupBaseDir = "/sys/fs/cgroup";
    string cgroupPath;
    in >> cgroupPath;       // for example: "0::/init.scope"
    vector<string> parts = split::tsplit(cgroupPath, ":");
    return cgroupBaseDir + parts[2];
}

/*
 * Controllers must always be enabled top down starting from the root of the hierarchy.
 * Enabling a controller in a cgroup indicates that the distribution of the target
 * resource across its immediate children will be controlled. Hence, to activate
 * control and spawn the desired controller-interface files in the target
 * directory, we must enable the controller of interest up to its parent folder.
 */
void activateController(EcGroup::ECGROUP controller, string cgroupPath)
{
    // cgroupPath example: "/sys/fs/cgroup/init.scope"
    // After split:
    // [0] : ""
    // [1] : "sys"
    // [2] : "fs"
    // [3] : "cgroup"
    // [4] : "init.scope"
    vector<string> subPath = split::tsplit(cgroupPath, "/");
    string currentFolder = make_path("/" + subPath[1], subPath[2]);
    string controllerName = EcGroup::getControllerName(controller);
    for (int i = 3; i < subPath.size()-1; ++i) {
        currentFolder += "/" + subPath[i];
        string currentFile = make_path(currentFolder, "cgroup.subtree_control");
        ofstream fileStream(currentFile.c_str());
        if (!fileStream.is_open()) {
            throwCouldNotOpenFile(__func__, currentFile);
        }
        fileStream << controllerName;
        fileStream.close();
    }
}

void writeValue(EcGroup::ECGROUP controller, int value, std::string cgroupPath) {
    string filePath = make_path(cgroupPath, EcGroup::getFileName(controller));
    ofstream fileStream(filePath.c_str());
    if (!fileStream.is_open()) {
        throwCouldNotOpenFile(__func__, filePath);
    }
    fileStream << value;
    fileStream.close();
}

string getValue(EcGroup::ECGROUP controller, std::string cgroupPath) {
    string filePath = make_path(cgroupPath, EcGroup::getFileName(controller));
    ifstream in(filePath.c_str());
    if (!in.is_open()) {
        throwCouldNotOpenFile(__func__, filePath);
    }
    string content;
    in >> content;
    return content;;
}

string createCgroup(string cgroupPath, string name)
{
    string newPath = make_path(cgroupPath, name);
    int rc = mkdir(newPath.c_str(), S_IRWXU|S_IRWXU|S_IRWXG|S_IRWXG);
    if (rc != 0 && errno != EEXIST) {
        ostringstream os;
        os << "createCgroup: could not create directory " << newPath
           << ": " << strerror(errno);
        throw PcException(os.str());
    }
    return newPath;
}

void addToCgroup(string cgroupPath, pid_t pid)
{
    string filePath = make_path(cgroupPath, "cgroup.procs");
    ofstream fileStream(filePath.c_str());
    if (!fileStream.is_open()) {
        throwCouldNotOpenFile("addToCgroup", filePath);
    }
    fileStream << pid;
    fileStream.close();
}

}   // namespace util
}   // namespace pc
