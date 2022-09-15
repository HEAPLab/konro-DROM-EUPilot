#include "cgroupcontrol.h"
#include "utilities/tsplit.h"
#include "pcexception.h"
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

using namespace std;

namespace pc {

CGroupControl::~CGroupControl()
{
}

void CGroupControl::checkActivateController(const char *controllerName, const char *fileName, const std::string &cgroupPath) const
{
    std::string filePath = make_path(cgroupPath, fileName);
    if (!Dir::file_exists(filePath.c_str())) {
        util::activateController(controllerName, cgroupPath);
    }
}

std::string CGroupControl::getLine(const char *controllerName, const char *fileName, std::shared_ptr<App> app) const
{
    // 1 - Find cgroup path
    string cgroupPath = util::findCgroupPath(app->getPid());

    // 2 - If the controller interface file doesn't exist, activate the controller
    checkActivateController(controllerName, fileName, cgroupPath);

    // 3 - Get line from the file
    return util::getLine(fileName, cgroupPath);
}

std::vector<string> CGroupControl::getContent(const char *controllerName, const char *fileName, std::shared_ptr<App> app) const
{
    // 1 - Find cgroup path
    string cgroupPath = util::findCgroupPath(app->getPid());

    // 2 - If the controller interface file doesn't exist, activate the controller
    checkActivateController(controllerName, fileName, cgroupPath);

    // 3 - Get content from the file
    return util::getContent(fileName, cgroupPath);
}

int CGroupControl::getValueAsInt(const char *controllerName, const char *fileName, std::shared_ptr<App> app) const
{
    string value = getLine(controllerName, fileName, app);
    uint64_t n = strtoull(value.c_str(), nullptr, 10);
    return static_cast<int>(n);
}

std::map<string, uint64_t> CGroupControl::getContentAsMap(const char *controllerName, const char *fileName, std::shared_ptr<App> app)
{
    string cgroupPath = util::findCgroupPath(app->getPid());
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

void CGroupControl::addApplication(std::shared_ptr<App> app)
{
    string cgroupAppBaseDir = util::getCgroupAppBaseDir(app->getPid());
    Dir::mkdir_r(cgroupAppBaseDir.c_str());
    util::moveToCgroup(cgroupAppBaseDir, app->getPid());
}

void CGroupControl::removeApplication(std::shared_ptr<App> app)
{
    string cgroupAppBaseDir = util::getCgroupAppBaseDir(app->getPid());
    Dir::rmdir(cgroupAppBaseDir.c_str());
}

}

