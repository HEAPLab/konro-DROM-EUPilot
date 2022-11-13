#include "monitorevent.h"

using namespace std;

namespace rmcommon {

MonitorEvent::MonitorEvent(PlatformTemperature temp, PlatformPower power) :
    platTemp_(temp),
    platPower_(power)
{
}

void MonitorEvent::printOnOstream(std::ostream &os) const
{
    os << "MonitorEvent {}";
}

}   // namespace rmcommon
