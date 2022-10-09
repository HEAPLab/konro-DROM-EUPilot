#include "resourcepolicies.h"
#include <iostream>

using namespace std;

ResourcePolicies::ResourcePolicies()
{

}

void ResourcePolicies::run()
{
    while (true) {
        shared_ptr<BaseEvent> event;
        bool rc = queue_.waitAndPop(event, WAIT_POP_TIMEOUT_MILLIS);
        if (!rc) {
            cout << "ResourcePolicies::run: no message received\n";
            continue;
        }
        processEvent(event);
    }
}

void ResourcePolicies::processEvent(std::shared_ptr<BaseEvent> event)
{
    cout << "ResourcePolicies::run: received message => " << event.get() << endl;

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
