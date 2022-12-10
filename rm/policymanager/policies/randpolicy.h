#ifndef RANDPOLICY_H
#define RANDPOLICY_H

#include "ibasepolicy.h"

namespace rp {

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
    virtual void monitor(std::shared_ptr<const rmcommon::MonitorEvent> event) override;
    virtual void feedback(std::shared_ptr<const rmcommon::FeedbackEvent> event) override;
};

}   // namespace rp

#endif // RANDPOLICY_H
