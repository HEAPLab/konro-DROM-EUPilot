#include "concreteeventreceiver.h"
#include "threadname.h"
#include <log4cpp/Category.hh>

using namespace std;

namespace rmcommon {

ConcreteEventReceiver::ConcreteEventReceiver(const char *threadName) :
    threadName_(threadName),
    stop_(false)
{
}

void ConcreteEventReceiver::addEvent(std::shared_ptr<BaseEvent> event)
{
    queue_.push(event);
}

void ConcreteEventReceiver::start()
{
    stop_= false;
    receiverThread_ = thread(&ConcreteEventReceiver::run, this);
}

void ConcreteEventReceiver::run()
{
    rmcommon::setThreadName(threadName_.c_str());

    using Logger = log4cpp::Category;
    Logger::getRoot().info("%s starts", threadName_.c_str());
    while (!stop_) {
        shared_ptr<rmcommon::BaseEvent> event;
        if (queue_.waitAndPop(event, WAIT_POP_TIMEOUT_MILLIS)) {
            Logger::getRoot().info("CONCRETEEVENTRECEIVER received event %s",
                                   event->getName().c_str());
            stop_ = !processEvent(event);
        } else {
            // Logger::getRoot().info("ConcreteEventReceiver: no event");
        }
    }
    Logger::getRoot().info("%s stops", threadName_.c_str());
}


}   // namespace rmcommon
