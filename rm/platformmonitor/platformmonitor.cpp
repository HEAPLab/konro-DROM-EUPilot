#include "platformmonitor.h"
#include <chrono>

using namespace std;

PlatformMonitor::PlatformMonitor(ResourcePolicies &rp) : resourcePolicies_(rp)
{

}

PlatformMonitor::~PlatformMonitor()
{
    stop();
}

/*!
 * \brief Starts the thred function "run()"
 */
void PlatformMonitor::start()
{
    stop_ = false;
    pmThread_ = thread(&PlatformMonitor::run, this);
}

/*!
 * \brief Stops and joins the thread
 */
void PlatformMonitor::stop()
{
    stop_ = true;
    if (pmThread_.joinable()) {
        pmThread_.join();
    }
}

void PlatformMonitor::run()
{
    while (!stop_) {
        this_thread::sleep_for(chrono::milliseconds(1000));
        // TODO - do something

        // TODO - generate an event

        // TODO - add event to ResourcePolicies queue

        //resourcePolicies_.addEvent(...);
    }
}
