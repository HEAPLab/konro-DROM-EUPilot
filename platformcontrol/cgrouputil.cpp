#include "cgrouputil.h"
#include "tsplit.h"
#include "pcexception.h"
#include <vector>
#include <sstream>
#include <cstring>
#include <sys/stat.h>

using namespace std;

namespace pc {
namespace util {

std::string getCgroupBaseDir()
{
    return "/sys/fs/cgroup";
}

std::string getCgroupKonroBaseDir()
{
    return "/sys/fs/cgroup/konro.slice/";
}

string getCgroupAppBaseDir(pid_t pid)
{
    string cgroupBasePath = util::getCgroupKonroBaseDir();
    ostringstream os;
    os << cgroupBasePath << "app-" << pid << ".scope";
    return os.str();
}

/*!
 * \brief Throws a PcException
 */
void throwCouldNotOpenFile(string funcName, string fileName)
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
    string cgroupPath;
    in >> cgroupPath;       // for example: "0::/init.scope"
    vector<string> parts = split::tsplit(cgroupPath, ":");
    return getCgroupBaseDir() + parts[2];
}


/*
 * Controllers must always be enabled top down starting from the root of the hierarchy.
 * Enabling a controller in a cgroup indicates that the distribution of the target
 * resource across its immediate children will be controlled. Hence, to activate
 * control and spawn the desired controller-interface files in the target
 * directory, we must enable the controller of interest up to its parent folder.
 */
void activateController(const char *controllerName, const string &cgroupPath)
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
    for (int i = 3; i < subPath.size()-1; ++i) {
        currentFolder += "/" + subPath[i];
        string currentFile = make_path(currentFolder, "cgroup.subtree_control");
        ofstream fileStream(currentFile.c_str());
        if (!fileStream.is_open()) {
            throwCouldNotOpenFile(__func__, currentFile);
        }
        fileStream << (string("+") + controllerName);
        fileStream.close();
    }
}

void writeValue(const char *fileName, const string &value, std::string cgroupPath)
{
    string filePath = make_path(cgroupPath, fileName);
    ofstream fileStream(filePath.c_str());
    if (!fileStream.is_open()) {
        throwCouldNotOpenFile(__func__, filePath);
    }
    fileStream << value;
    fileStream.close();
}

std::string getLine(const char *fileName, std::string cgroupPath)
{
    string filePath = make_path(cgroupPath, fileName);
    ifstream in(filePath.c_str());
    if (!in.is_open()) {
        throwCouldNotOpenFile(__func__, filePath);
    }
    string line;
    getline(in, line);
    return line;
}

std::vector<string> getContent(const char *fileName, std::string cgroupPath)
{
    string filePath = make_path(cgroupPath, fileName);
    ifstream in(filePath.c_str());
    if (!in.is_open()) {
        throwCouldNotOpenFile(__func__, filePath);
    }
    vector<string> content;
    string line;
    while (getline(in, line))
        content.push_back(line);
    return content;
}

string createCgroup(string cgroupPath, string name)
{
    string newPath = make_path(cgroupPath, name);
    int rc = mkdir(newPath.c_str(), S_IRWXU|S_IRWXU|S_IRWXG|S_IRWXG);
    if (rc != 0 && errno != EEXIST) {
        ostringstream os;
        os << __func__ << ": could not create directory " << newPath
           << ": " << strerror(errno);
        throw PcException(os.str());
    }
    return newPath;
}

void moveToCgroup(string cgroupPath, pid_t pid)
{
    string filePath = make_path(cgroupPath, "cgroup.procs");
    ofstream fileStream(filePath.c_str());
    if (!fileStream.is_open()) {
        throwCouldNotOpenFile(__func__, filePath);
    }
    fileStream << pid;
    fileStream.close();
}

}   // namespace util
}   // namespace pc
