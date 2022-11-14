#ifndef RANDPOLICY_H
#define RANDPOLICY_H

#include "ibasepolicy.h"
#include <log4cpp/Category.hh>

/*!
 * \class random resource management policy
 *
 * Assigns each new process to a random CPU core.
 */
class RandPolicy : public IBasePolicy {
    PlatformDescription platformDescription_;
    log4cpp::Category &cat_;
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
};

#endif // RANDPOLICY_H
