#ifndef RANDPOLICY_H
#define RANDPOLICY_H

#include "ibasepolicy.h"
/*!
 * \class random resource management policy
 *
 * Assigns each new process to a random CPU core.
 */
class RandPolicy : public IBasePolicy {
public:
    explicit RandPolicy();
    virtual const char *name() override {
        return "RandPolicy";
    }
    virtual void addApp(std::shared_ptr<AppInfo> appInfo) override;
    virtual void removeApp(std::shared_ptr<AppInfo> appInfo) override;
};

#endif // RANDPOLICY_H
