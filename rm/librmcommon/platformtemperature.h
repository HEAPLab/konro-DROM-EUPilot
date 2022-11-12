#ifndef PLATFORMTEMPERATURE_H
#define PLATFORMTEMPERATURE_H

#include "componenttemperature.h"
#include <vector>

namespace rmcommon {

class PlatformTemperature {
    rmcommon::ComponentTemperature package_;
    std::vector<rmcommon::ComponentTemperature> cpus_;
public:
    void addPackageTemperature(const rmcommon::ComponentTemperature &ct) {
        package_ = ct;
    }

    rmcommon::ComponentTemperature getPackageTemp() {
        return package_;
    }

    void addCpuTemperature(const rmcommon::ComponentTemperature &ct) {
        cpus_.push_back(ct);
    }

};

}   // namespace rmcommon

#endif // PLATFORMTEMPERATURE_H
