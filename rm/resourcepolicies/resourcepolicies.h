#ifndef RESOURCEPOLICIES_H
#define RESOURCEPOLICIES_H

#include "../include/threadsafequeue.h"
#include "../include/baseevent.h"
#include "../include/addprocevent.h"
#include "../include/removeprocevent.h"
#include <chrono>
#include <memory>

class ResourcePolicies {
    const std::chrono::milliseconds WAIT_POP_TIMEOUT_MILLIS = std::chrono::milliseconds(5000);
    ThreadsafeQueue<std::shared_ptr<BaseEvent>> queue_;

    void run();
    void processEvent(std::shared_ptr<BaseEvent> event);
    void processAddProcEvent(AddProcEvent *ev);
    void processRemoveProcEvent(RemoveProcEvent *ev);

public:
    ResourcePolicies();

    void addEvent(std::shared_ptr<BaseEvent> event) {
        queue_.push(event);
    }

    void operator()() {
        run();
    }
};

#endif // RESOURCEPOLICIES_H
