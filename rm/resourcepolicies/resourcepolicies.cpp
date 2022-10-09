#include "resourcepolicies.h"
#include <iostream>
#include <sstream>
#include <thread>

using namespace std;

ResourcePolicies::ResourcePolicies()
{

}

/*!
 * Starts the run() function in a new thread
 */
void ResourcePolicies::start()
{
    rpThread_ = thread(&ResourcePolicies::run, this);
}

void ResourcePolicies::run()
{
    cout << "ResourcePolicies thread starting\n";
    while (true) {
        shared_ptr<BaseEvent> event;
        bool rc = queue_.waitAndPop(event, WAIT_POP_TIMEOUT_MILLIS);
        if (!rc) {
            cout << "ResourcePolicies: no message received\n";
            continue;
        }
        processEvent(event);
    }
    cout << "ResourcePolicies thread exiting\n";
}

void ResourcePolicies::processEvent(std::shared_ptr<BaseEvent> event)
{
#if 1
    ostringstream os;
    os << "ResourcePolicies::run: received message => ";
    event->printOnOstream(os);
    os << endl;
    cout << os.str();
#endif

    if (AddProcEvent *e = dynamic_cast<AddProcEvent *>(event.get())) {
        processAddProcEvent(e);
    } else if (RemoveProcEvent *e = dynamic_cast<RemoveProcEvent *>(event.get())) {
        processRemoveProcEvent(e);
    }
}

void ResourcePolicies::processAddProcEvent(AddProcEvent *ev)
{
}

void ResourcePolicies::processRemoveProcEvent(RemoveProcEvent *ev)
{
}
