#ifndef RANDPOLICY_H
#define RANDPOLICY_H

#include "ibasepolicy.h"

/*!
 * \class random resource management policy
 *
 * Assigns each new process to a random CPU core.
 */
class RandPolicy : public IBasePolicy {
    PlatformDescription platformDescription_;
public:
    explicit RandPolicy(PlatformDescription pd);
    virtual const char *name() override {
        return "RandPolicy";
    }
    // IBasePolicy interface
    virtual void addApp(std::shared_ptr<AppMapping> appMapping) override;
    virtual void removeApp(std::shared_ptr<AppMapping> appMapping) override;
    virtual void timer() override;
    virtual void monitor(rmcommon::MonitorEvent *ev) override;
    virtual void feedback(rmcommon::ProcFeedbackEvent *ev) override;
};

#endif // RANDPOLICY_H
