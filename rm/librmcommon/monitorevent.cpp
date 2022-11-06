#include "monitorevent.h"

using namespace std;

namespace rmcommon {

MonitorEvent::MonitorEvent()
{
}

void MonitorEvent::printOnOstream(std::ostream &os) const
{
    os << "MonitorEvent" << std::endl;
    int i = 0;
    for (const rmcommon::CoreTemperature &t: cpuTemp_) {
        os << "[" << i++ << "] " << t << endl;
    }
}

}   // namespace rmcommon
