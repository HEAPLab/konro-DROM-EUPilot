#include "cgrouputil.h"
#include "pcexception.h"
#include "tsplit.h"
#include "dir.h"
#include <vector>
#include <sstream>
#include <cstring>
#include <log4cpp/Category.hh>

using namespace std;

#ifdef TIMING
#include "timer.h"
using MicrosecondsTimer = rmcommon::Timer<std::chrono::microseconds>;
#endif

namespace pc {
namespace util {

std::string getCgroupBaseDir()
{
    //return "/sys/fs/cgroup";
    return CGROUPBASEDIR;
}

std::string getCgroupKonroBaseDir()
{
    return getCgroupBaseDir() + "/konro.slice";
}

string getCgroupKonroAppDir(pid_t pid)
{
    string cgroupBasePath = util::getCgroupKonroBaseDir();
    ostringstream os;
    os << "app-" << pid << ".scope";
    return rmcommon::make_path(cgroupBasePath, os.str());
}

void throwCouldNotOpenFile(const char *funcName, const string &fileName)
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
    // for example: "0::/init.scope"
    if (!(in >> cgroupPath))
        throw PcException("findCgroupPath: could not read cgroup path for pid");
    vector<string> parts = rmcommon::tsplit(cgroupPath, ":");
    string result = getCgroupBaseDir() + parts[2];
    return result;
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
    // For example: cgroupPath = "/sys/fs/cgroup/konro.slice"
    // After split with separator "/":
    // [0] : ""
    // [1] : "sys"
    // [2] : "fs"
    // [3] : "cgroup"
    // [4] : "konro.slice"
    log4cpp::Category &cat = log4cpp::Category::getRoot();

#ifdef TIMING
    MicrosecondsTimer timerDetail;
    MicrosecondsTimer timer;
#endif

    vector<string> subPath = rmcommon::tsplit(cgroupPath, "/");
    string currentFolder = rmcommon::make_path("/" + subPath[1], subPath[2]);
    for (int i = 3; i < subPath.size()-1; ++i) {
        currentFolder += "/" + subPath[i];
        string currentFile = rmcommon::make_path(currentFolder, "cgroup.subtree_control");
#ifdef TIMING
        timerDetail.Restart();
#endif
        ofstream fileStream(currentFile.c_str());
        if (!fileStream.is_open()) {
            throwCouldNotOpenFile(__func__, currentFile);
        }
#ifndef TIMING
        cat.debug("activateController: adding %s to %s", controllerName, currentFile.c_str());
#endif
        fileStream << (string("+") + controllerName);
        fileStream.close();
#ifdef TIMING
        chrono::microseconds usd = timerDetail.Elapsed();
        cat.debug("CGROUPUTIL timing: activateController write in cgroup.subtree_control = %d microseconds",
                  (int)usd.count());
#endif
    }
#ifdef TIMING
    chrono::microseconds us = timer.Elapsed();
    cat.debug("CGROUPUTIL timing: activateController = %d microseconds", (int)us.count());
#endif
}

std::string getLine(const char *fileName, const string &cgroupPath)
{
    string filePath = rmcommon::make_path(cgroupPath, fileName);
    ifstream in(filePath.c_str());
    if (!in.is_open()) {
        throwCouldNotOpenFile(__func__, filePath);
    }
    string line;
    getline(in, line);
    return line;
}

std::vector<string> getContent(const char *fileName, const std::string &cgroupPath)
{
    string filePath = rmcommon::make_path(cgroupPath, fileName);
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

string createCgroup(string cgroupPath, const std::string &name)
{
#ifdef TIMING
    MicrosecondsTimer timer;
#endif

    string newPath = rmcommon::make_path(cgroupPath, name);
    rmcommon::Dir::mkdir(newPath.c_str());

#ifdef TIMING
    chrono::microseconds us = timer.Elapsed();
    log4cpp::Category::getRoot().debug("CGROUPUTIL timing: createCgroup = %d microseconds", (int)us.count());
#endif

    return newPath;
}

void moveToCgroup(const string &cgroupPath, pid_t pid)
{
#ifdef TIMING
    MicrosecondsTimer timer;
#endif

    string filePath = rmcommon::make_path(cgroupPath, "cgroup.procs");
    ofstream fileStream(filePath.c_str());
    if (!fileStream.is_open()) {
        throwCouldNotOpenFile(__func__, filePath);
    }
    fileStream << pid;
    fileStream.close();

#ifdef TIMING
    chrono::microseconds us = timer.Elapsed();
    log4cpp::Category::getRoot().debug("CGROUPUTIL timing: moveToCgroup = %d microseconds", (int)us.count());
#endif
}

}   // namespace util
}   // namespace pc
