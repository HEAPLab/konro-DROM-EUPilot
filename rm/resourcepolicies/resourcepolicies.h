#ifndef RESOURCEPOLICIES_H
#define RESOURCEPOLICIES_H

#include "threadsafequeue.h"
#include "baseevent.h"
#include "addprocevent.h"
#include "removeprocevent.h"
#include "appmapping.h"
#include "policies/ibasepolicy.h"
#include "platformdescription.h"
#include <set>
#include <chrono>
#include <memory>
#include <thread>

/*!
 * \brief The ResourcePolicies class
 *
 * Using a queue, ResourcePolicies receives events from multiple
 * threads, adds/removes managed applications to
 * a set and forwards the events to the chosen resource
 * policy.
 *
 * ResourcePolicies runs in a dedicated thread.
 */
class ResourcePolicies {
public:
    enum class Policy {
        NoPolicy,
        RandPolicy
    };

private:
    const std::chrono::milliseconds WAIT_POP_TIMEOUT_MILLIS = std::chrono::milliseconds(5000);
    rmcommon::ThreadsafeQueue<std::shared_ptr<rmcommon::BaseEvent>> queue_;
    std::thread rpThread_;
    std::unique_ptr<IBasePolicy> policy_;
    PlatformDescription platformDescription_;

    /*! Comparison function for the set */
    using AppComparator = bool (*)(const std::shared_ptr<AppMapping> &lhs,
                                   const std::shared_ptr<AppMapping> &rhs);

    std::set<std::shared_ptr<AppMapping>, AppComparator> apps_;

    void run();

    /*!
     * Processes a generic event by calling the appropriate handler function.
     * \param event the event to process
     */
    void processEvent(std::shared_ptr<rmcommon::BaseEvent> event);

    /*!
     * Processes an AddProcEvent.
     * \param event the event to process
     */
    void processAddProcEvent(rmcommon::AddProcEvent *ev);

    /*!
     * Processes a RemoveProcEvent.
     * \param event the event to process
     */
    void processRemoveProcEvent(rmcommon::RemoveProcEvent *ev);

    /* for debugging */
    void dumpApps() const;

    /*! Factory method */
    std::unique_ptr<IBasePolicy> makePolicy(Policy policy);
public:

    ResourcePolicies(PlatformDescription pd, Policy policy = Policy::NoPolicy);

    void addEvent(std::shared_ptr<rmcommon::BaseEvent> event) {
        queue_.push(event);
    }

    void operator()() {
        run();
    }

    void start();
};

#endif // RESOURCEPOLICIES_H
