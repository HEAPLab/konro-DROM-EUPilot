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
 * Compares two Apps ("less" function)
 *
 * \param lhs the first app to compare
 * \param rhs the second app to compare
 * \return true if pid of lsh is < than pid of rhs
 */
static bool appComp(const shared_ptr<pc::App> &lhs, const shared_ptr<pc::App> &rhs)
{
    return lhs->getPid() < rhs->getPid();
}

WorkloadManager::WorkloadManager(pc::IPlatformControl &pc, int pid) :
    pc_(pc),
    pid_(0),
    apps_(appComp)
{
    add(pc::App::makeApp(pid, pc::App::STANDALONE));
}

void WorkloadManager::add(shared_ptr<pc::App> app)
{
    apps_.insert(app);
    pc_.addApplication(app);
}

shared_ptr<pc::App> WorkloadManager::getApp(pid_t pid)
{
    shared_ptr<pc::App> key = pc::App::makeApp(pid, pc::App::UNKNOWN);
    auto it = apps_.find(key);
    return *it;
}


void WorkloadManager::remove(pid_t pid)
{
    shared_ptr<pc::App> key = pc::App::makeApp(pid, pc::App::UNKNOWN);
    auto it = apps_.find(key);
    if (it != end(apps_)) {
        pc_.removeApplication(*it);
        apps_.erase(it);
    }
}

bool WorkloadManager::isInKonro(pid_t pid)
{
    shared_ptr<pc::App> key = pc::App::makeApp(pid, pc::App::UNKNOWN);
    return apps_.find(key) != end(apps_);

}

void WorkloadManager::update(uint8_t *data)
{
    struct proc_event *ev = reinterpret_cast<struct proc_event *>(data);

    switch (ev->what) {
    case proc_event::PROC_EVENT_NONE:
        cout << "WorkloadManager: PROC_EVENT_NONE received\n";
        break;
    case proc_event::PROC_EVENT_FORK:
        cout << "WorkloadManager: PROC_EVENT_FORK received\n";
        processForkEvent(data);
        break;
    case proc_event::PROC_EVENT_EXEC:
        cout << "WorkloadManager: PROC_EVENT_EXEC received\n";
        processExecEvent(data);
        break;
    case proc_event::PROC_EVENT_UID:
        cout << "WorkloadManager: PROC_EVENT_UID received\n";
        break;
    case proc_event::PROC_EVENT_GID:
        cout << "WorkloadManager: PROC_EVENT_GID received\n";
        break;
    case proc_event::PROC_EVENT_SID:
        cout << "WorkloadManager: PROC_EVENT_SID received\n";
        break;
    case proc_event::PROC_EVENT_PTRACE:
        cout << "WorkloadManager: PROC_EVENT_PTRACE received\n";
        break;
    case proc_event::PROC_EVENT_COMM:
        cout << "WorkloadManager: PROC_EVENT_COMM received\n";
        break;
    case proc_event::PROC_EVENT_COREDUMP:
        cout << "WorkloadManager: PROC_EVENT_COREDUMP received\n";
        cout.flush();
        processCoreDumpEvent(data);
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
        shared_ptr<pc::App> app = pc::App::makeApp(ev->event_data.fork.child_pid, pc::App::STANDALONE);
        app->setName(getProcessNameByPid(app->getPid()));
        add(app);

        cout << "    parent_pid:"
             << ev->event_data.fork.parent_pid
             << " (" << getProcessNameByPid(ev->event_data.fork.parent_pid) << ")"
             << " forked" << endl;
        cout << "   parent_tgid:" << ev->event_data.fork.parent_tgid << endl;
        cout << "     child_pid:" << ev->event_data.fork.child_pid
             << " (" << getProcessNameByPid(ev->event_data.fork.parent_pid) << ")"
             << " was forked" << endl;
        cout << "    child_tgid:" << ev->event_data.fork.child_tgid << " was forked" << endl;
    }
    dumpApps();
}

void WorkloadManager::processExecEvent(uint8_t *data)
{
    struct proc_event *ev = reinterpret_cast<struct proc_event *>(data);
    pid_t pid = ev->event_data.exec.process_pid;
    if (isInKonro(pid)) {
        shared_ptr<pc::App> app = getApp(pid);
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

void WorkloadManager::processCoreDumpEvent(uint8_t *data)
{
    struct proc_event *ev = reinterpret_cast<struct proc_event *>(data);

    pid_t pid = ev->event_data.coredump.process_pid;
    if (isInKonro(pid)) {
        // remove will be done by the EXIT event which follows
        //remove(pid);

        cout << "    process_pid:" << ev->event_data.coredump.process_pid
             << " (" << getProcessNameByPid(ev->event_data.exit.process_pid) << ")"
             << " exited" << endl;
        cout << "    parent_pid:" << ev->event_data.coredump.parent_pid
             << " (" << getProcessNameByPid(ev->event_data.coredump.parent_pid) << ")"
             << " exited" << endl;
        cout << "    process_tgid:" << ev->event_data.coredump.process_tgid << " exited" << endl;
    }
    dumpApps();
}

void WorkloadManager::dumpApps()
{
    cout << "WorkloadManager: monitoring ";
    bool first = true;
    for (auto &app: apps_) {
        if (first)
            first = false;
        else
            cout << ",";
        cout << app->getPid();
    }
    cout << endl;
}

}   // namespace wm
