#ifndef NOPOLICY_H
#define NOPOLICY_H

#include "ibasepolicy.h"

/*!
 * \class an empty scheduling policy.
 * Useful for testing purposes.
 */
class NoPolicy : public IBasePolicy {
    virtual const char *name() override {
        return "NoPolicy";
    }
    virtual void addApp(std::shared_ptr<AppInfo> appInfo) override {
        // do nothing
    }
    virtual void removeApp(std::shared_ptr<AppInfo> appInfo) override {
        // do nothing
    }
};

#endif // NOPOLICY_H
