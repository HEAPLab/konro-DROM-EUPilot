#ifndef CPUBASEDPOLICY_H
#define CPUBASEDPOLICY_H

#include "ibasepolicy.h"

namespace rp {

class CpuBasedPolicy : public IBasePolicy {
    const AppMappingSet &apps_;
    PlatformDescription platformDescription_;
    rmcommon::PlatformLoad lastPlatformLoad;
    bool hasLastPlatformLoad;

    int getLowerUsagePU();
    int pickInitialCpu();

public:
    CpuBasedPolicy(const AppMappingSet &apps, PlatformDescription pd);

    // IBasePolicy interface
    virtual const char *name() override {
        return "CpuBasedPolicy";
    }
    virtual void addApp(AppMappingPtr appMapping) override;
    virtual void removeApp(AppMappingPtr appMapping) override;
    virtual void timer() override;
    virtual void monitor(std::shared_ptr<const rmcommon::MonitorEvent> event) override;
    virtual void feedback(AppMappingPtr appMapping, int feedback) override;
};

}   // namespace rp

#endif // CPUBASEDPOLICY_H
