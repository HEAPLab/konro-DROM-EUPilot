#include "resourcepolicies.h"
#include "policies/nopolicy.h"
#include "policies/randpolicy.h"
#include <iostream>
#include <sstream>
#include <thread>

using namespace std;

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

ResourcePolicies::ResourcePolicies(PlatformDescription pd, Policy policy, int timerSeconds) :
    platformDescription_(pd),
    apps_(appMappingComp),
    timerSeconds_(timerSeconds)
{
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

/*!
 * Starts the run() function in a new thread
 * and the timer() function in a new thread
 */
void ResourcePolicies::start()
{
    rpThread_ = thread(&ResourcePolicies::run, this);
    // If a timer was requested, start the thread now
    if (timerSeconds_ > 0)
        timerThread_ = thread(&ResourcePolicies::timer, this);
    else
        cout << "ResourcePolicies: timer not started" << endl;
}

ResourcePolicies::Policy ResourcePolicies::getPolicyByName(const std::string &policyName)
{
    if (policyName == "RandPolicy")
        return ResourcePolicies::Policy::RandPolicy;
    else if (policyName == "NoPolicy")
        return ResourcePolicies::Policy::NoPolicy;
    else
        return ResourcePolicies::Policy::NoPolicy;
}

void ResourcePolicies::run()
{
    cout << "ResourcePolicies thread starting\n";
    while (!stop_) {
        shared_ptr<rmcommon::BaseEvent> event;
        bool rc = queue_.waitAndPop(event, WAIT_POP_TIMEOUT_MILLIS);
        if (!rc) {
            cout << "ResourcePolicies: no message received\n";
            continue;
        }
        processEvent(event);
    }
    cout << "ResourcePolicies thread exiting\n";
}

void ResourcePolicies::timer()
{
    cout << "ResourcePolicies timer thread starting\n";
    while (!stop_) {
        this_thread::sleep_for(chrono::seconds(timerSeconds_));
        addEvent(make_shared<rmcommon::TimerEvent>());
    }
    cout << "ResourcePolicies timer thread exiting\n";
}

void ResourcePolicies::processEvent(std::shared_ptr<rmcommon::BaseEvent> event)
{
#if 1
    ostringstream os;
    os << "ResourcePolicies::run: received message => ";
    event->printOnOstream(os);
    os << endl;
    cout << os.str();
#endif

    if (rmcommon::AddProcEvent *e = dynamic_cast<rmcommon::AddProcEvent *>(event.get())) {
        processAddProcEvent(e);
    } else if (rmcommon::RemoveProcEvent *e = dynamic_cast<rmcommon::RemoveProcEvent *>(event.get())) {
        processRemoveProcEvent(e);
    } else if (rmcommon::TimerEvent *e = dynamic_cast<rmcommon::TimerEvent *>(event.get())) {
        processTimerEvent(e);
    }
}

void ResourcePolicies::processAddProcEvent(rmcommon::AddProcEvent *ev)
{
    shared_ptr<AppMapping> appMapping = make_shared<AppMapping>(ev->getApp());
    apps_.insert(appMapping);
    dumpApps();
    policy_->addApp(appMapping);
}

void ResourcePolicies::processRemoveProcEvent(rmcommon::RemoveProcEvent *ev)
{
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
    cout << "ResourcePolicies: timer event received\n";
}

void ResourcePolicies::dumpApps() const
{
    cout << "ResourcePolicies: handling PIDS {";
    bool first = true;
    for (auto &app: apps_) {
        if (first)
            first = false;
        else
            cout << ",";
        cout << app->getPid();
    }
    cout << "}" << endl;
}
