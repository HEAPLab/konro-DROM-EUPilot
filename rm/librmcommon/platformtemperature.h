#ifndef PLATFORMTEMPERATURE_H
#define PLATFORMTEMPERATURE_H

#include "componenttemperature.h"
#include <vector>

namespace rmcommon {

class PlatformTemperature {
    std::vector<rmcommon::ComponentTemperature> cpus_;
    std::vector<rmcommon::ComponentTemperature> cores_;
public:
    void addCpuTemperature(const rmcommon::ComponentTemperature &ct) {
        cpus_.push_back(ct);
    }

//    rmcommon::ComponentTemperature getCpuTemp() {
//        return cpu_;
//    }

    void addCoreTemperature(const rmcommon::ComponentTemperature &ct) {
        cores_.push_back(ct);
    }
};

}   // namespace rmcommon

#endif // PLATFORMTEMPERATURE_H
