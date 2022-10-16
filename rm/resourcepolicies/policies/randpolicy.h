#ifndef RANDPOLICY_H
#define RANDPOLICY_H

#include "ibasepolicy.h"
/*!
 * \class random scheduling policy.
 * This policy assigns each new process to a random CPU.
 */
class RandPolicy : public IBasePolicy {
public:
    RandPolicy() = default;
    virtual const char *name() override {
        return "RandPolicy";
    }
    virtual void addApp(std::shared_ptr<AppInfo> appInfo) override;
    virtual void removeApp(std::shared_ptr<AppInfo> appInfo) override;
};

#endif // RANDPOLICY_H
