#include "cgroupcontrol.h"
#include "tsplit.h"
#include "pcexception.h"
#include "dir.h"
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

using namespace std;

namespace pc {

CGroupControl::~CGroupControl()
{
}

std::string CGroupControl::getLine(const char *fileName, App app) const
{
    // 1 - Find cgroup path
    string cgroupPath = util::findCgroupPath(app.getPid());

    // 2 - Get line from the file
    return util::getLine(fileName, cgroupPath);
}

std::vector<string> CGroupControl::getContent(const char *fileName, App app) const
{
    // 1 - Find cgroup path
    string cgroupPath = util::findCgroupPath(app.getPid());

    // 2 - Get content from the file
    return util::getContent(fileName, cgroupPath);
}

int CGroupControl::getValueAsInt(const char *fileName, App app) const
{
    string value = getLine(fileName, app);
    long n = strtol(value.c_str(), nullptr, 10);
    return static_cast<int>(n);
}

std::map<string, unsigned long> CGroupControl::getContentAsMap(const char *fileName, App app)
{
    std::map<std::string, unsigned long> tags;
    std::vector<std::string> fileContent = getContent(fileName, app);
    for (auto &line: fileContent) {
        std::string tag;
        unsigned long value;
        std::istringstream is(line);
        is >> tag >> value;
        if (is.fail())
            break;
        tags.emplace(tag, value);
    }
    return tags;
}

void CGroupControl::addApplication(App app)
{
    string cgroupAppBaseDir = util::getCgroupAppBaseDir(app.getPid());
    Dir::mkdir_r(cgroupAppBaseDir.c_str());
    util::moveToCgroup(cgroupAppBaseDir, app.getPid());
}

void CGroupControl::removeApplication(App app)
{
    string cgroupAppBaseDir = util::getCgroupAppBaseDir(app.getPid());
    Dir::rmdir(cgroupAppBaseDir.c_str());
}

}

