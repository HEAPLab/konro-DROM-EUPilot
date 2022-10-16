#ifndef IBASEPOLICY_H
#define IBASEPOLICY_H

#include "../appinfo.h"
#include <memory>

/*!
 * \interface an interface for event-based scheduling.
 * Each event type in the system is associated to a handler function.
 */
class IBasePolicy {
public:
    IBasePolicy() = default;
    virtual ~IBasePolicy() = default;

    virtual const char *name() = 0;

    /*!
     * Handles the addition of a new app to the system.
     */
    virtual void addApp(std::shared_ptr<AppInfo> appInfo) = 0;

    /*!
     * Handles the removal of an app from the system.
     */
    virtual void removeApp(std::shared_ptr<AppInfo> appInfo) = 0;
};

#endif // IBASEPOLICY_H
