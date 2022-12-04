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
    log4cpp::Category::getRoot().info("%s starts", threadName_.c_str());
    stop_= false;
    receiverThread_ = thread(&ConcreteEventReceiver::run, this);
    while (!stop_) {
        shared_ptr<rmcommon::BaseEvent> event;
        if (queue_.waitAndPop(event, WAIT_POP_TIMEOUT_MILLIS)) {
            stop_ = !processEvent(event);
        }
    }
    log4cpp::Category::getRoot().info("%s stops", threadName_.c_str());
}

void ConcreteEventReceiver::run()
{
    rmcommon::setThreadName(threadName_.c_str());
}


}   // namespace rmcommon
