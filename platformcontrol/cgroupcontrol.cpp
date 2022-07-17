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


void CGroupControl::setValue(EcGroup::ECGROUP controller, int value, App app)
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

}

