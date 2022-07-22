#include "cgroupcontrol.h"
#include "cgrouputil.h"
#include "tsplit.h"
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

using namespace std;

namespace pc {

CGroupControl::CGroupControl()
{

}

CGroupControl::~CGroupControl()
{

}


void CGroupControl::setValue(EcGroup::ECGROUP controller, const string &value, App app)
{
    // 1 - FIND CGROUP PATH
    string cgroupPath = util::findCgroupPath(app.getPid());
    if (cgroupPath.empty())
        return;

    // 2 - ACTIVATE CONTROLLER
    util::activateController(controller, cgroupPath);

    // 3 - WRITE VALUE IN CORRECT CGROUP
    util::writeValue(controller, value, cgroupPath);
}

void CGroupControl::setValue(const std::string &controllerName,
                             const std::string &fileName,
                             const std::string &value, App app)
{
    // 1 - FIND CGROUP PATH
    string cgroupPath = util::findCgroupPath(app.getPid());
    if (cgroupPath.empty())
        return;

    // 2 - ACTIVATE CONTROLLER
    util::activateController(controllerName, cgroupPath);

    // 3 - WRITE VALUE IN CORRECT CGROUP
    util::writeValue(fileName, value, cgroupPath);

}

}

