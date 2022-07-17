#include "cgrouputil.h"
#include "tsplit.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <sys/stat.h>

using namespace std;

namespace pc {
namespace util {

string findCgroupPath(pid_t pid)
{
    ostringstream os;
    os << "/proc/" << pid << "/cgroup";
    ifstream in(os.str());
    if (in.is_open()) {
        string cgroupBaseDir = "/sys/fs/cgroup";
        string cgroupPath;
        in >> cgroupPath;       // for example: "0::/init.scope"
        vector<string> parts = split::tsplit(cgroupPath, ":");
        if (parts.size() == 3)
            return cgroupBaseDir + parts[2];
    }
    return "";
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
    if (subPath.size() < 4)
        return;
    string currentFolder = "/" + subPath[1] + "/" + subPath[2];
    string controllerName = EcGroup::getControllerName(controller);
    for (int i = 3; i < subPath.size()-1; ++i) {
        currentFolder += "/" + subPath[i];
        string currentFile = currentFolder + "/" + "cgroup.subtree_control";
        ofstream fileStream(currentFile.c_str());
        fileStream << controllerName;
        fileStream.close();
    }
}

void writeValue(EcGroup::ECGROUP controller, int value, std::string cgroupPath) {
    string filePath = cgroupPath + "/" + EcGroup::getFileName(controller);
}

std::string getValue(EcGroup::ECGROUP controller, std::string cgroupPath) {
    string filePath = cgroupPath + "/" + EcGroup::getFileName(controller);
    ostringstream os;
    os << filePath;
    ifstream in(os.str());
    if (in.is_open()) {
        string content;
        in >> content;
        return content;
    }
    return "";
}

void createCgroup(std::string cgroupPath, std::string name) {
    int rc = mkdir(cgroupPath.c_str(), S_IRWXU|S_IRWXU|S_IRWXG|S_IRWXG);
    if (rc != 0 && errno != EEXIST) {
        perror(cgroupPath.c_str());
    }
}

void addToCgroup(std::string cgroupPath, pid_t pid) {
    ofstream fileStream(cgroupPath.c_str());
    fileStream << pid;
    fileStream.close();
}

}   // namespace util
}   // namespace pc
