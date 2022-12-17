#ifndef NOPOLICY_H
#define NOPOLICY_H

#include "ibasepolicy.h"

namespace rp {

/*!
 * \class empty resource management policy
 *
 * Useful for testing.
 */
class NoPolicy : public IBasePolicy {
public:
    explicit NoPolicy();

    virtual const char *name() override {
        return "NoPolicy";
    }
    virtual void addApp(std::shared_ptr<AppMapping> appMapping) override {
        // do nothing
    }
    virtual void removeApp(std::shared_ptr<AppMapping> appMapping) override {
        // do nothing
    }
    virtual void timer() override {
        // do nothing
    }

    virtual void monitor(std::shared_ptr<const rmcommon::MonitorEvent> event) override {
        // do nothing
    }

    virtual void feedback(std::shared_ptr<AppMapping> appMapping, int feedback) override {
        // do nothing
    }
};

}   // namespace rp

#endif // NOPOLICY_H
