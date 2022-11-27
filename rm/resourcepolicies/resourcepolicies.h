#ifndef RESOURCEPOLICIES_H
#define RESOURCEPOLICIES_H

#include "ieventreceiver.h"
#include "threadsafequeue.h"
#include "baseevent.h"
#include "addprocevent.h"
#include "removeprocevent.h"
#include "timerevent.h"
#include "monitorevent.h"
#include "procfeedbackevent.h"
#include "appmapping.h"
#include "policies/ibasepolicy.h"
#include "platformdescription.h"
#include <log4cpp/Category.hh>
#include <set>
#include <chrono>
#include <memory>
#include <thread>
#include <atomic>

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
class ResourcePolicies : public rmcommon::IEventReceiver {
public:
    enum class Policy {
        NoPolicy,
        RandPolicy
    };

private:
    log4cpp::Category &cat_;
    const std::chrono::milliseconds WAIT_POP_TIMEOUT_MILLIS = std::chrono::milliseconds(3000);
    rmcommon::ThreadsafeQueue<std::shared_ptr<rmcommon::BaseEvent>> queue_;
    std::thread rpThread_;
    std::thread timerThread_;
    std::unique_ptr<IBasePolicy> policy_;
    PlatformDescription platformDescription_;
    int timerSeconds_;
    std::atomic_bool stop_;

    /*! Comparison function for the set */
    using AppComparator = bool (*)(const std::shared_ptr<AppMapping> &lhs,
                                   const std::shared_ptr<AppMapping> &rhs);

    std::set<std::shared_ptr<AppMapping>, AppComparator> apps_;

    void run();

    /*! Encapsulates the timer thread logic */
    void timer();

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

    /*!
     * Processes a TimerEvent
     * \param ev the event to process
     */
    void processTimerEvent(rmcommon::TimerEvent *ev);

    /*!
     * Processes a MonitorEvent
     * \param ev the event to process
     */
    void processMonitorEvent(rmcommon::MonitorEvent *ev);

    /*!
     * Processes a ProcFeedbackEvent
     * \param ev the event to process
     */
    void processProcFeedbackEvent(rmcommon::ProcFeedbackEvent *ev);

    /* for debugging */
    void dumpApps() const;

    /*! Factory method */
    std::unique_ptr<IBasePolicy> makePolicy(Policy policy);
public:

    /*!
     * \param timerSeconds if 0, then the internal timer thread is not started
     */
    ResourcePolicies(PlatformDescription pd, Policy policy = Policy::NoPolicy, int timerSeconds = 30);

    /*!
     * \brief Adds an event to the tread safe queue
     * \param event the event to add
     */
    virtual void addEvent(std::shared_ptr<rmcommon::BaseEvent> event) override {
        queue_.push(event);
    }

    void operator()() {
        run();
    }

    void start();

    void stop() {
        stop_ = true;
        if (rpThread_.joinable())
            rpThread_.join();
        if (timerThread_.joinable())
            timerThread_.join();
    }

    /*!
     * Return the policy with the specified name.
     * If no policy exists with that name, NoPolicy is returned.
     */
    static Policy getPolicyByName(const std::string &policyName);
};

#endif // RESOURCEPOLICIES_H
