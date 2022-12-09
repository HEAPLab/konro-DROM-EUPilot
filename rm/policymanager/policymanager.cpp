#include "policymanager.h"
#include "policies/nopolicy.h"
#include "policies/randpolicy.h"
#include "threadname.h"
#include "eventbus.h"
#include <iostream>
#include <sstream>
#include <thread>

using namespace std;


namespace rp {

/*!
 * Compares two AppMapping ("less" function) handled
 * by shared pointers
 *
 * \param lhs the first app to compare
 * \param rhs the second app to compare
 * \return true if pid of lsh is < than pid of rhs
 */
static bool appMappingComp(const shared_ptr<AppMapping> &lhs, const shared_ptr<AppMapping> &rhs)
{
    return lhs->getPid() < rhs->getPid();
}

PolicyManager::PolicyManager(rmcommon::EventBus &bus, PlatformDescription pd, Policy policy, int timerSeconds) :
    rmcommon::BaseEventReceiver("POLICYMANAGER"),
    cat_(log4cpp::Category::getRoot()),
    bus_(bus),
    platformDescription_(pd),
    apps_(appMappingComp),
    timerSeconds_(timerSeconds)
{
    subscribeToEvents();
    policy_ = makePolicy(policy);
}

std::unique_ptr<IBasePolicy> PolicyManager::makePolicy(Policy policy)
{
    switch (policy) {
    case Policy::RandPolicy:
        return make_unique<RandPolicy>(platformDescription_);
    case Policy::NoPolicy:
    default:
        return make_unique<NoPolicy>();
        break;
    }
}

void PolicyManager::start()
{
    BaseEventReceiver::start();
    // If a timer was requested, start the thread now
    if (timerSeconds_ > 0) {
        timerThread_ = thread(&PolicyManager::timer, this);
    }
    else {
        cat_.info("POLICYMANAGER timer not started");
    }
}

void PolicyManager::stop()
{
    // stop the internal timer thread
    if (timerSeconds_ > 0) {
        stopTimer_ = true;
        if (timerThread_.joinable()) {
            timerThread_.join();
        }
    }
    // stop our own thread
    BaseEventReceiver::stop();
}

PolicyManager::Policy PolicyManager::getPolicyByName(const std::string &policyName)
{
    if (policyName == "RandPolicy")
        return PolicyManager::Policy::RandPolicy;
    else
        return PolicyManager::Policy::NoPolicy;
}

void PolicyManager::subscribeToEvents()
{
    bus_.subscribe<PolicyManager, rmcommon::AddEvent, rmcommon::BaseEvent>(this, &PolicyManager::addEvent);
    bus_.subscribe<PolicyManager, rmcommon::RemoveEvent, rmcommon::BaseEvent>(this, &PolicyManager::addEvent);
    bus_.subscribe<PolicyManager, rmcommon::TimerEvent, rmcommon::BaseEvent>(this, &PolicyManager::addEvent);
    bus_.subscribe<PolicyManager, rmcommon::FeedbackEvent, rmcommon::BaseEvent>(this, &PolicyManager::addEvent);
    bus_.subscribe<PolicyManager, rmcommon::MonitorEvent, rmcommon::BaseEvent>(this, &PolicyManager::addEvent);
}

void PolicyManager::timer()
{
    cat_.info("POLICYMANAGER timer thread starting");
    while (!stopTimer_) {
        for (int i = 0; i < timerSeconds_ && !stopTimer_; ++i) {
            this_thread::sleep_for(chrono::seconds(1));
        }
        if (stopTimer_) {
            break;
        }
        // put the event directly on our queue, without
        // going through the event bus
        addEvent(make_shared<rmcommon::TimerEvent>());
    }
    cat_.info("POLICYMANAGER timer thread exiting");
}

bool PolicyManager::processEvent(std::shared_ptr<rmcommon::BaseEvent> event)
{
#if 1
    ostringstream os;
    os << "POLICYMANAGER received message => " << *event;
    cat_.debug(os.str());
#endif

    if (rmcommon::AddEvent *e = dynamic_cast<rmcommon::AddEvent *>(event.get())) {
        processAddEvent(e);
    } else if (rmcommon::RemoveEvent *e = dynamic_cast<rmcommon::RemoveEvent *>(event.get())) {
        processRemoveEvent(e);
    } else if (rmcommon::TimerEvent *e = dynamic_cast<rmcommon::TimerEvent *>(event.get())) {
        processTimerEvent(e);
    } else if (rmcommon::MonitorEvent *e = dynamic_cast<rmcommon::MonitorEvent *>(event.get())) {
        processMonitorEvent(e);
    } else if (rmcommon::FeedbackEvent *e = dynamic_cast<rmcommon::FeedbackEvent *>(event.get())) {
        processFeedbackEvent(e);
    }
    return true;        // continue processing
}

void PolicyManager::processAddEvent(rmcommon::AddEvent *ev)
{
    cat_.debug("POLICYMANAGER AddEvent received");
    shared_ptr<AppMapping> appMapping = make_shared<AppMapping>(ev->getApp());
    apps_.insert(appMapping);
    dumpApps();
    policy_->addApp(appMapping);
}

void PolicyManager::processRemoveEvent(rmcommon::RemoveEvent *ev)
{
    cat_.debug("POLICYMANAGER RemoveProc event received");
    // search target
    shared_ptr<AppMapping> appMapping = make_shared<AppMapping>(ev->getApp());
    auto it = apps_.find(appMapping);
    if (it != end(apps_)) {
        policy_->removeApp(*it);
        apps_.erase(it);
    }
    dumpApps();
}

void PolicyManager::processTimerEvent(rmcommon::TimerEvent *ev)
{
    cat_.debug("POLICYMANAGER timer event received");
    policy_->timer();
}

void PolicyManager::processMonitorEvent(rmcommon::MonitorEvent *ev)
{
    cat_.debug("POLICYMANAGER monitor event received");
    policy_->monitor(ev);
}

void PolicyManager::processFeedbackEvent(rmcommon::FeedbackEvent *ev)
{
    cat_.debug("POLICYMANAGER feedback event received");
    policy_->feedback(ev);
}

void PolicyManager::dumpApps() const
{
    std::ostringstream os;
    os << "POLICYMANAGER handling PIDS [";
    bool first = true;
    for (auto &app: apps_) {
        if (first)
            first = false;
        else
            os << ",";
        os << app->getPid();
    }
    os << "]";
    cat_.info(os.str());
}

}   // namespace rp
