#ifndef MINCORESPOLICY_H
#define MINCORESPOLICY_H

#include "ibasepolicy.h"
#include <set>

namespace rp {

class MinCoresPolicy : public IBasePolicy {
    const AppMappingSet &apps_;
    PlatformDescription platformDescription_;
    rmcommon::PlatformLoad lastPlatformLoad;
    bool hasLastPlatformLoad;
    // Acceptable performace slack for applications (in percentage)
    float slack = 0.1;

    int getLowerUsagePU();
    int pickInitialCpu();
    std::set<short> getAvailablePUs(const rmcommon::CpusetVector &vec);
    short getNextPU(const rmcommon::CpusetVector &vec);
    short pickWorstPU(const rmcommon::CpusetVector &vec);

public:
    MinCoresPolicy(const AppMappingSet &apps, PlatformDescription pd);

    // IBasePolicy interface
    virtual const char *name() override {
        return "MinCoresPolicy";
    }
    virtual void addApp(AppMappingPtr appMapping) override;
    virtual void removeApp(AppMappingPtr appMapping) override;
    virtual void timer() override;
    virtual void monitor(std::shared_ptr<const rmcommon::MonitorEvent> event) override;
    virtual void feedback(AppMappingPtr appMapping, int feedback) override;
};

}   // namespace rp

#endif // MINCORESPOLICY_H

