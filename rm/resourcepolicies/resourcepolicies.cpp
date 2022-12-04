#include "resourcepolicies.h"
#include "policies/nopolicy.h"
#include "policies/randpolicy.h"
#include "threadname.h"
#include "simpleeventbus.h"
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

ResourcePolicies::ResourcePolicies(rmcommon::EventBus &bus, PlatformDescription pd, Policy policy, int timerSeconds) :
    rmcommon::ConcreteEventReceiver("RESOURCEPOLICIES"),
    cat_(log4cpp::Category::getRoot()),
    bus_(bus),
    platformDescription_(pd),
    apps_(appMappingComp),
    timerSeconds_(timerSeconds)
{
    subscribeToEvents();
    policy_ = makePolicy(policy);
}

std::unique_ptr<IBasePolicy> ResourcePolicies::makePolicy(Policy policy)
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

void ResourcePolicies::start()
{
    ConcreteEventReceiver::start();
    // If a timer was requested, start the thread now
    if (timerSeconds_ > 0) {
        timerThread_ = thread(&ResourcePolicies::timer, this);
    }
    else {
        cat_.info("RESOURCEPOLICIES timer not started");
    }
}

ResourcePolicies::Policy ResourcePolicies::getPolicyByName(const std::string &policyName)
{
    if (policyName == "RandPolicy")
        return ResourcePolicies::Policy::RandPolicy;
    else
        return ResourcePolicies::Policy::NoPolicy;
}

void ResourcePolicies::subscribeToEvents()
{
    bus_.subscribe<ResourcePolicies, rmcommon::AddProcEvent, rmcommon::BaseEvent>(this, &ResourcePolicies::addEvent);
    bus_.subscribe<ResourcePolicies, rmcommon::ProcFeedbackEvent, rmcommon::BaseEvent>(this, &ResourcePolicies::addEvent);
}

void ResourcePolicies::timer()
{
    cat_.info("RESOURCEPOLICIES timer thread starting");
    while (!stop_) {
        this_thread::sleep_for(chrono::seconds(timerSeconds_));
        addEvent(make_shared<rmcommon::TimerEvent>());
    }
    cat_.info("RESOURCEPOLICIES timer thread exiting");
}

bool ResourcePolicies::processEvent(std::shared_ptr<rmcommon::BaseEvent> event)
{
#if 1
    ostringstream os;
    os << "RESOURCEPOLICIES received message => " << *event;
    cat_.debug(os.str());
#endif

    if (rmcommon::AddProcEvent *e = dynamic_cast<rmcommon::AddProcEvent *>(event.get())) {
        processAddProcEvent(e);
    } else if (rmcommon::RemoveProcEvent *e = dynamic_cast<rmcommon::RemoveProcEvent *>(event.get())) {
        processRemoveProcEvent(e);
    } else if (rmcommon::TimerEvent *e = dynamic_cast<rmcommon::TimerEvent *>(event.get())) {
        processTimerEvent(e);
    } else if (rmcommon::MonitorEvent *e = dynamic_cast<rmcommon::MonitorEvent *>(event.get())) {
        processMonitorEvent(e);
    } else if (rmcommon::ProcFeedbackEvent *e = dynamic_cast<rmcommon::ProcFeedbackEvent *>(event.get())) {
        processProcFeedbackEvent(e);
    }
    return true;        // continue processing
}

void ResourcePolicies::processAddProcEvent(rmcommon::AddProcEvent *ev)
{
    cat_.debug("RESOURCEPOLICIES AddProc event received");
    shared_ptr<AppMapping> appMapping = make_shared<AppMapping>(ev->getApp());
    apps_.insert(appMapping);
    dumpApps();
    policy_->addApp(appMapping);
}

void ResourcePolicies::processRemoveProcEvent(rmcommon::RemoveProcEvent *ev)
{
    cat_.debug("RESOURCEPOLICIES RemoveProc event received");
    // search target
    shared_ptr<AppMapping> appMapping = make_shared<AppMapping>(ev->getApp());
    auto it = apps_.find(appMapping);
    if (it != end(apps_)) {
        policy_->removeApp(*it);
        apps_.erase(it);
    }
    dumpApps();
}

void ResourcePolicies::processTimerEvent(rmcommon::TimerEvent *ev)
{
    cat_.debug("RESOURCEPOLICIES timer event received");
    policy_->timer();
}

void ResourcePolicies::processMonitorEvent(rmcommon::MonitorEvent *ev)
{
    cat_.debug("RESOURCEPOLICIES monitor event received");
    policy_->monitor(ev);
}

void ResourcePolicies::processProcFeedbackEvent(rmcommon::ProcFeedbackEvent *ev)
{
    cat_.debug("RESOURCEPOLICIES feedback event received");
    policy_->feedback(ev);
}

void ResourcePolicies::dumpApps() const
{
    std::ostringstream os;
    os << "RESOURCEPOLICIES handling PIDS [";
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
