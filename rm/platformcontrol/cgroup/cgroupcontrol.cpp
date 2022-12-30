#include "cgroupcontrol.h"
#include "tsplit.h"
#include "pcexception.h"
#include "dir.h"
#include "timer.h"
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <chrono>

using namespace std;

namespace pc {

CGroupControl::CGroupControl() :
    cat_(log4cpp::Category::getRoot())
{
}

CGroupControl::~CGroupControl()
{
}

void CGroupControl::cleanup()
{
    using namespace rmcommon;

    try {
        string konroBaseDir = util::getCgroupKonroBaseDir();
        /* If the Konro base folder doesn't exist, there's nothing to cleanup */
        if (!Dir::dir_exists(konroBaseDir.c_str())) {
            cat_.info("CGROUPCONTROL cleanup: %s does not exist", konroBaseDir.c_str());
            return;
        }
        string cgroupBaseDir = util::getCgroupBaseDir();
        {
            Dir dir = Dir::localdir(konroBaseDir.c_str());
            for (Dir::DirIterator it = dir.begin(); it != dir.end(); ++it) {
                if (!it->is_dir())
                    continue;
                string pathName = make_path(konroBaseDir, it->name());
                string pidStr = util::getLine("cgroup.procs", pathName);
                /* If the folder contains a live process, migrate it to the root */
                if (!pidStr.empty()) {
                    int pid = atoi(pidStr.c_str());
                    cat_.info("CGROUPCONTROL moving pid %d to %s", pid, cgroupBaseDir.c_str());
                    util::moveToCgroup(cgroupBaseDir, pid);
                }
                cat_.info("CGROUPCONTROL removing directory %s", pathName.c_str());
                Dir::rmdir(pathName.c_str());
            }
        }
        cat_.info("CGROUPCONTROL removing directory %s", konroBaseDir.c_str());
        Dir::rmdir(konroBaseDir.c_str());
    } catch (runtime_error &e) {
        cat_.error(e.what());
    }
}

void CGroupControl::checkActivateController(const char *controllerName, const char *fileName, const std::string &cgroupPath) const
{
    std::string filePath = rmcommon::make_path(cgroupPath, fileName);
    if (!rmcommon::Dir::file_exists(filePath.c_str())) {
        util::activateController(controllerName, cgroupPath);
    }
}

std::string CGroupControl::getLine(const char *controllerName, const char *fileName, std::shared_ptr<rmcommon::App> app) const
{
    // 1 - Find cgroup path
    string cgroupPath = util::getCgroupKonroAppDir(app->getPid());

    // 2 - If the controller interface file doesn't exist, activate the controller
    checkActivateController(controllerName, fileName, cgroupPath);

    // 3 - Get line from the file
    return util::getLine(fileName, cgroupPath);
}

std::vector<string> CGroupControl::getContent(const char *controllerName, const char *fileName, std::shared_ptr<rmcommon::App> app) const
{
    // 1 - Find cgroup path
    string cgroupPath = util::getCgroupKonroAppDir(app->getPid());

    // 2 - If the controller interface file doesn't exist, activate the controller
    checkActivateController(controllerName, fileName, cgroupPath);

    // 3 - Get content from the file
    return util::getContent(fileName, cgroupPath);
}

int CGroupControl::getValueAsInt(const char *controllerName, const char *fileName, std::shared_ptr<rmcommon::App> app) const
{
    string value = getLine(controllerName, fileName, app);
    uint64_t n = strtoull(value.c_str(), nullptr, 10);
    return static_cast<int>(n);
}

std::map<string, uint64_t> CGroupControl::getContentAsMap(const char *controllerName, const char *fileName, std::shared_ptr<rmcommon::App> app)
{
    string cgroupPath = util::getCgroupKonroAppDir(app->getPid());
    checkActivateController(controllerName, fileName, cgroupPath);

    std::map<std::string, uint64_t> tags;
    std::vector<std::string> fileContent = getContent(controllerName, fileName, app);
    for (auto &line: fileContent) {
        std::string tag;
        uint64_t value;
        std::istringstream is(line);
        is >> tag >> value;
        if (is.fail())
            break;
        tags.emplace(tag, value);
    }
    return tags;
}

bool CGroupControl::addApplication(std::shared_ptr<rmcommon::App> app)
{
    string cgroupAppBaseDir = util::getCgroupKonroAppDir(app->getPid());

#ifdef TIMING
    rmcommon::Timer<chrono::microseconds> timer(true);
#endif
    try {
        rmcommon::Dir::mkdir_r(cgroupAppBaseDir.c_str());
    } catch (runtime_error &e) {
        cat_.error("CGROUPCONTROL addApplication: could not create directory %s: %s",
                   cgroupAppBaseDir.c_str(),
                   e.what());
        return false;
    }

#ifdef TIMING
    chrono::microseconds u1 = timer.Elapsed(true);
#endif

#ifndef TIMING
    cat_.info("CGROUPCONTROL addApplication: move PID %ld to cgroup directory %s",
              (long)app->getPid(), cgroupAppBaseDir.c_str());
#endif

    try {
        util::moveToCgroup(cgroupAppBaseDir, app->getPid());
    } catch (runtime_error &e) {
        cat_.error("CGROUPCONTROL addApplication: could not move to cgroup %s: %s",
                   cgroupAppBaseDir.c_str(),
                   e.what());
        return false;
    }

#ifdef TIMING
    chrono::microseconds u2 = timer.Elapsed();
    cat_.debug("CGROUPCONTROL timing mkdir_r = %ld microseconds", (long)u1.count());
    cat_.debug("CGROUPCONTROL timing moveToCgroup = %ld microseconds", (long)u2.count());
#endif
    return true;
}

bool CGroupControl::removeApplication(std::shared_ptr<rmcommon::App> app)
{
    string cgroupAppBaseDir = util::getCgroupKonroAppDir(app->getPid());
    cat_.info("CGROUPCONTROL removeApplication PID %ld: remove cgroup directory %s",
              (long)app->getPid(), cgroupAppBaseDir.c_str());

#ifdef TIMING
    rmcommon::Timer<chrono::microseconds> timer(true);
#endif

    try {
        rmcommon::Dir::rmdir(cgroupAppBaseDir.c_str());
    } catch (runtime_error &e) {
        cat_.error("CGROUPCONTROL removeApplication PID %ld: could not remove cgroup directory %s",
                  (long)app->getPid(), cgroupAppBaseDir.c_str());
        return false;
    }

#ifdef TIMING
    chrono::microseconds u1 = timer.Elapsed(true);
    cat_.debug("CGROUPCONTROL timing rmdir = %ld microseconds", (long)u1.count());
#endif
    return true;
}

}

