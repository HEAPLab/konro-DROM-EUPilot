#ifndef RESOURCEPOLICIES_H
#define RESOURCEPOLICIES_H

#include "threadsafequeue.h"
#include "baseevent.h"
#include "addprocevent.h"
#include "removeprocevent.h"
#include "appinfo.h"
#include "policies/ibasepolicy.h"
#include <set>
#include <chrono>
#include <memory>
#include <thread>

class ResourcePolicies {
    const std::chrono::milliseconds WAIT_POP_TIMEOUT_MILLIS = std::chrono::milliseconds(5000);
    ThreadsafeQueue<std::shared_ptr<BaseEvent>> queue_;
    std::thread rpThread_;
    std::unique_ptr<IBasePolicy> policy_;

    /*! Comparison function for the set */
    using AppComparator = bool (*)(const std::shared_ptr<AppInfo> &lhs,
                                   const std::shared_ptr<AppInfo> &rhs);

    std::set<std::shared_ptr<AppInfo>, AppComparator> apps_;

    void run();
    void processEvent(std::shared_ptr<BaseEvent> event);
    void processAddProcEvent(AddProcEvent *ev);
    void processRemoveProcEvent(RemoveProcEvent *ev);
    void dumpApps() const;  // for debugging
public:
    enum class Policy {
        NoPolicy,
        RandPolicy
    };

    ResourcePolicies(Policy policy = Policy::NoPolicy);

    void addEvent(std::shared_ptr<BaseEvent> event) {
        queue_.push(event);
    }

    void operator()() {
        run();
    }

    void start();
};

#endif // RESOURCEPOLICIES_H
