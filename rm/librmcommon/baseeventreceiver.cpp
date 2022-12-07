#include "baseeventreceiver.h"
#include "threadname.h"
#include <log4cpp/Category.hh>

using namespace std;

namespace rmcommon {

BaseEventReceiver::BaseEventReceiver(const char *threadName) :
    threadName_(threadName),
    stop_(false)
{
}

void BaseEventReceiver::addEvent(std::shared_ptr<BaseEvent> event)
{
    queue_.push(event);
}

void BaseEventReceiver::start()
{
    stop_= false;
    receiverThread_ = thread(&BaseEventReceiver::run, this);
}

void BaseEventReceiver::stop()
{
    stop_= true;
}

void BaseEventReceiver::run()
{
    rmcommon::setThreadName(threadName_.c_str());

    using Logger = log4cpp::Category;
    Logger::getRoot().info("%s starts", threadName_.c_str());
    while (!stop_) {
        shared_ptr<rmcommon::BaseEvent> event;
        if (queue_.waitAndPop(event, WAIT_POP_TIMEOUT_MILLIS)) {
//            Logger::getRoot().info("BASEEEVENTRECEIVER received event %s",
//                                   event->getName().c_str());
            stop_ = !processEvent(event);
        } else {
            // Logger::getRoot().info("BaseEventReceiver: no event");
        }
    }
    Logger::getRoot().info("%s stops", threadName_.c_str());
}


}   // namespace rmcommon
