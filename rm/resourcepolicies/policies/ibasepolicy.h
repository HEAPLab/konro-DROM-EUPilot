#ifndef IBASEPOLICY_H
#define IBASEPOLICY_H

#include "../appmapping.h"
#include "platformdescription.h"
#include "monitorevent.h"
#include "procfeedbackevent.h"
#include <memory>

namespace rp {

/*!
 * \interface an interface for event-based scheduling.
 * Each event type in the system is associated to a handler function.
 */
class IBasePolicy {
protected:
    PlatformDescription platformDescription_;
public:
    explicit IBasePolicy() = default;
    virtual ~IBasePolicy() = default;

    virtual const char *name() = 0;

    /*!
     * Handles the addition of a new app to the system.
     */
    virtual void addApp(std::shared_ptr<AppMapping> appMapping) = 0;

    /*!
     * Handles the removal of an app from the system.
     */
    virtual void removeApp(std::shared_ptr<AppMapping> appMapping) = 0;

    /*!
     * Handles a timer event.
     */
    virtual void timer() = 0;

    /*!
     * Handles a platform monitor event.
     */
    virtual void monitor(rmcommon::MonitorEvent *ev) = 0;

    /*!
     * Handles a platform monitor event.
     */
    virtual void feedback(rmcommon::ProcFeedbackEvent *ev) = 0;
};

}   // namespace rp

#endif // IBASEPOLICY_H
