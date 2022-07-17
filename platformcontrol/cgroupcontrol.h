#ifndef CGROUPCONTROL_H
#define CGROUPCONTROL_H

#include "iplatformcontrol.h"
#include "ecgroup.h"
#include "app.h"

#include <string>
#include <unistd.h>

namespace pc {

class CGroupControl : public IPlatformControl {

public:
    CGroupControl();
    void setValue(EcGroup::ECGROUP controller, int value, App app);
};

}
#endif // CGROUPCONTROL_H
