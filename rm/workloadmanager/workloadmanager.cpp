#include "workloadmanager.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>
#include <linux/cn_proc.h>

using namespace std;

namespace wm {

static string getProcessNameByPid(int pid)
{
    ostringstream os;
    os << "/proc/" << pid << "/cmdline";
    fstream fs(os.str());
    string cmdline;
    if (fs.is_open()) {
        if (getline(fs, cmdline)) {
            // get rid of embedded '\0'
            for (size_t i = 0; i < cmdline.size(); ++i) {
                if (cmdline[i] == '\0')
                    cmdline[i] = ' ';
            }
        }
    }
    return cmdline;
}

/*!
 * Compares two App ("less" function) handled
 * by shared pointers
 *
 * \param lhs the first app to compare
 * \param rhs the second app to compare
 * \return true if pid of lsh is < than pid of rhs
 */
static bool appComp(const shared_ptr<rmcommon::App> &lhs, const shared_ptr<rmcommon::App> &rhs)
{
    return lhs->getPid() < rhs->getPid();
}

WorkloadManager::WorkloadManager(pc::IPlatformControl &pc, ResourcePolicies &rp, int pid) :
    platformControl_(pc),
    resourcePolicies_(rp),
    pid_(0),
    apps_(appComp)
{
    add(rmcommon::App::makeApp(pid, rmcommon::App::STANDALONE));
}

void WorkloadManager::add(shared_ptr<rmcommon::App> app)
{
    apps_.insert(app);
    platformControl_.addApplication(app);
    resourcePolicies_.addEvent(make_shared<rmcommon::AddProcEvent>(app));
}

shared_ptr<rmcommon::App> WorkloadManager::getApp(pid_t pid)
{
    shared_ptr<rmcommon::App> key = rmcommon::App::makeApp(pid, rmcommon::App::UNKNOWN);
    auto it = apps_.find(key);
    return *it;
}


void WorkloadManager::remove(pid_t pid)
{
    shared_ptr<rmcommon::App> key = rmcommon::App::makeApp(pid, rmcommon::App::UNKNOWN);
    auto it = apps_.find(key);
    if (it != end(apps_)) {
        resourcePolicies_.addEvent(make_shared<rmcommon::RemoveProcEvent>(*it));
        platformControl_.removeApplication(*it);
        apps_.erase(it);
    }
}

bool WorkloadManager::isInKonro(pid_t pid)
{
    shared_ptr<rmcommon::App> key = rmcommon::App::makeApp(pid, rmcommon::App::UNKNOWN);
    return apps_.find(key) != end(apps_);

}

void WorkloadManager::update(uint8_t *data)
{
    struct proc_event *ev = reinterpret_cast<struct proc_event *>(data);

    switch (ev->what) {
    case proc_event::PROC_EVENT_FORK:
        cout << "WorkloadManager: PROC_EVENT_FORK received\n";
        processForkEvent(data);
        break;
    case proc_event::PROC_EVENT_EXEC:
        cout << "WorkloadManager: PROC_EVENT_EXEC received\n";
        processExecEvent(data);
        break;
    case proc_event::PROC_EVENT_EXIT:
        cout << "WorkloadManager: PROC_EVENT_EXIT received\n";
        processExitEvent(data);
        break;
    default:
        cout << "WorkloadManager: Event " << ev->what << " received\n";
        break;
    }
}

void WorkloadManager::processForkEvent(uint8_t *data)
{
    struct proc_event *ev = reinterpret_cast<struct proc_event *>(data);

    // Note: parent_pid and parent_tgid refer to the new process’ current parent
    //       which isn’t necessarily the process which called fork and birthed a
    //       child
    // https://natanyellin.com/posts/understanding-netlink-process-connector-output/

    if (isInKonro(ev->event_data.fork.parent_pid)) {
        shared_ptr<rmcommon::App> app = rmcommon::App::makeApp(ev->event_data.fork.child_pid, rmcommon::App::STANDALONE);
        app->setName(getProcessNameByPid(app->getPid()));
        add(app);

        ostringstream os;
        os << "    parent_pid:"
           << ev->event_data.fork.parent_pid
           << " (" << getProcessNameByPid(ev->event_data.fork.parent_pid) << ")"
           << " forked" << endl;
        os << "   parent_tgid:"
           << ev->event_data.fork.parent_tgid << endl;
        os << "     child_pid:"
           << ev->event_data.fork.child_pid
           << " (" << getProcessNameByPid(ev->event_data.fork.child_pid) << ")"
           << " was forked" << endl;
        os << "    child_tgid:"
           << ev->event_data.fork.child_tgid
           << " was forked" << endl;
        cout << os.str();
    }
    dumpApps();
}

void WorkloadManager::processExecEvent(uint8_t *data)
{
    struct proc_event *ev = reinterpret_cast<struct proc_event *>(data);
    pid_t pid = ev->event_data.exec.process_pid;
    if (isInKonro(pid)) {
        shared_ptr<rmcommon::App> app = getApp(pid);
        app->setName(getProcessNameByPid(pid));

        cout << "    process_pid:" << ev->event_data.exec.process_pid
             << " (" << getProcessNameByPid(ev->event_data.exec.process_pid) << ")"
             << " exec" << endl;
        cout << "    process_tgid:" << ev->event_data.exec.process_tgid << endl;
    }
}

void WorkloadManager::processExitEvent(uint8_t *data)
{
    struct proc_event *ev = reinterpret_cast<struct proc_event *>(data);

    pid_t pid = ev->event_data.exit.process_pid;
    if (isInKonro(pid)) {
        remove(pid);

        cout << "    process_pid:" << ev->event_data.exit.process_pid
             << " (" << getProcessNameByPid(ev->event_data.exit.process_pid) << ")"
             << " exited" << endl;
        cout << "    parent_pid:" << ev->event_data.exit.parent_pid
             << " (" << getProcessNameByPid(ev->event_data.exit.parent_pid) << ")"
             << " exited" << endl;
        cout << "    process_tgid:" << ev->event_data.exit.process_tgid << " exited" << endl;
    }
    dumpApps();
}

void WorkloadManager::dumpApps()
{
    cout << "WorkloadManager: monitoring PIDS {";
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

}   // namespace wm
