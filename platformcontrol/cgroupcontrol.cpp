#include "cgroupcontrol.h"
#include "tsplit.h"
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

std::string CGroupControl::getValue(const char *fileName, App app) const
{
    // 1 - Find cgroup path
    string cgroupPath = util::findCgroupPath(app.getPid());

    // 2 - Get value from the file
    return util::getValue(fileName, cgroupPath);
}

int CGroupControl::getValueAsInt(const char *fileName, App app) const
{
    string value = getValue(fileName, app);
    long n = strtol(value.c_str(), nullptr, 10);
    return static_cast<int>(n);
}

}

