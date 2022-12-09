#include "workloadmanager.h"
#include "addevent.h"
#include "removeevent.h"
#include "forkevent.h"
#include "execevent.h"
#include "exitevent.h"
#include "feedbackevent.h"
#include "eventbus.h"
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

WorkloadManager::WorkloadManager(rmcommon::EventBus &bus, pc::IPlatformControl &pc, int pid) :
    rmcommon::BaseEventReceiver("WORKLOADMANAGER"),
    bus_(bus),
    platformControl_(pc),
    cat_(log4cpp::Category::getRoot()),
    pid_(0),
    apps_(appComp)
{
    subscribeToEvents();

    add(rmcommon::App::makeApp(pid, rmcommon::App::AppType::STANDALONE));
}

void WorkloadManager::subscribeToEvents()
{
    using namespace rmcommon;
    auto handlerFunc = &WorkloadManager::addEvent;

    bus_.subscribe<WorkloadManager, ForkEvent, BaseEvent>(this, handlerFunc);
    bus_.subscribe<WorkloadManager, ExecEvent, BaseEvent>(this, handlerFunc);
    bus_.subscribe<WorkloadManager, ExitEvent, BaseEvent>(this, handlerFunc);
    bus_.subscribe<WorkloadManager, AddRequestEvent, BaseEvent>(this, handlerFunc);
    bus_.subscribe<WorkloadManager, FeedbackRequestEvent, BaseEvent>(this, handlerFunc);
}

void WorkloadManager::add(shared_ptr<rmcommon::App> app)
{
    apps_.insert(app);
    platformControl_.addApplication(app);
    bus_.publish(new rmcommon::AddEvent(app));
}

shared_ptr<rmcommon::App> WorkloadManager::getApp(pid_t pid)
{
    shared_ptr<rmcommon::App> key = rmcommon::App::makeApp(pid, rmcommon::App::AppType::UNKNOWN);
    auto it = apps_.find(key);
    return *it;
}

void WorkloadManager::remove(pid_t pid)
{
    shared_ptr<rmcommon::App> key = rmcommon::App::makeApp(pid, rmcommon::App::AppType::UNKNOWN);
    auto it = apps_.find(key);
    if (it != end(apps_)) {
        bus_.publish(new rmcommon::RemoveEvent(*it));
        platformControl_.removeApplication(*it);
        apps_.erase(it);
    }
}

bool WorkloadManager::isInKonro(pid_t pid)
{
    shared_ptr<rmcommon::App> key = rmcommon::App::makeApp(pid, rmcommon::App::AppType::UNKNOWN);
    return apps_.find(key) != end(apps_);
}

bool WorkloadManager::processEvent(std::shared_ptr<rmcommon::BaseEvent> event)
{
    using namespace rmcommon;

    if (ForkEvent *ev = dynamic_cast<ForkEvent *>(event.get())) {
        processForkEvent(&ev->data_[0]);
    } else if (ExecEvent *ev = dynamic_cast<ExecEvent *>(event.get())) {
        processExecEvent(&ev->data_[0]);
    } else if (ExitEvent *ev = dynamic_cast<ExitEvent *>(event.get())) {
        processExitEvent(&ev->data_[0]);
    } else if (AddRequestEvent *ev = dynamic_cast<AddRequestEvent *>(event.get())) {
        processAddRequestEvent(ev);
    } else if (FeedbackRequestEvent *ev = dynamic_cast<FeedbackRequestEvent *>(event.get())) {
        processFeedbackRequestEvent(ev);
    } else {
        cat_.error("WORKLOADMANAGER received wrong event: %s", event->getName().c_str());
    }
    return true;
}

void WorkloadManager::processForkEvent(uint8_t *data)
{
    struct proc_event *ev = reinterpret_cast<struct proc_event *>(data);

    // Note: parent_pid and parent_tgid refer to the new process’ current parent
    //       which isn’t necessarily the process which called fork and birthed a
    //       child
    // https://natanyellin.com/posts/understanding-netlink-process-connector-output/

    if (isInKonro(ev->event_data.fork.parent_pid)) {
        shared_ptr<rmcommon::App> app = rmcommon::App::makeApp(ev->event_data.fork.child_pid, rmcommon::App::AppType::STANDALONE);
        app->setName(getProcessNameByPid(app->getPid()));
        add(app);

        ostringstream os;
        os << "WORKLOADMANAGER fork {"
           << "\"parent_pid\":"
           << ev->event_data.fork.parent_pid
           << ",\"parent_name\":" << '\'' << getProcessNameByPid(ev->event_data.fork.parent_pid) << '\''
           << ",\"child_pid\":"
           << ev->event_data.fork.child_pid
           << ",\"child_tgid\":"
           << ev->event_data.fork.child_tgid
           << ",\"child_name\":" << '\'' << getProcessNameByPid(ev->event_data.fork.child_pid) << '\''
           << "}";
        cat_.info(os.str());
        dumpMonitoredApps();
    }
}

void WorkloadManager::processExecEvent(uint8_t *data)
{
    struct proc_event *ev = reinterpret_cast<struct proc_event *>(data);
    pid_t pid = ev->event_data.exec.process_pid;
    if (isInKonro(pid)) {
        shared_ptr<rmcommon::App> app = getApp(pid);
        app->setName(getProcessNameByPid(pid));

        ostringstream os;
        os << "WORKLOADMANAGER exec {"
           << "\"process_pid\":"
           << ev->event_data.exec.process_pid
           << ",\"process_name\":" << '\'' << getProcessNameByPid(ev->event_data.exec.process_pid) << '\''
           << ",\"process_tgid\":"
           << ev->event_data.exec.process_pid
           << "}";
        cat_.info(os.str());
        dumpMonitoredApps();
    }
}

void WorkloadManager::processExitEvent(uint8_t *data)
{
    struct proc_event *ev = reinterpret_cast<struct proc_event *>(data);

    pid_t pid = ev->event_data.exit.process_pid;
    if (isInKonro(pid)) {
        remove(pid);

        ostringstream os;
        os << "WORKLOADMANAGER exit {"
           << "\"process_pid\":"
           << ev->event_data.exit.process_pid
           << ",\"process_name\":" << '\'' << getProcessNameByPid(ev->event_data.exit.process_pid) << '\''
           << ",\"process_tgid\":"
           << ev->event_data.exit.process_tgid
           << "}";
        cat_.info(os.str());
        dumpMonitoredApps();
    }
}

void WorkloadManager::processAddRequestEvent(rmcommon::AddRequestEvent *ev)
{
    add(ev->getApp());

    ostringstream os;
    os << "WORKLOADMANAGER AddRequest {"
       << "\"process_pid\":"
       << ev->getApp()->getPid()
       << ",\"process_name\":" << '\'' << ev->getApp()->getPid() << '\''
       << "}";
    cat_.info(os.str());
    dumpMonitoredApps();
}

void WorkloadManager::processFeedbackRequestEvent(rmcommon::FeedbackRequestEvent *ev)
{
    if(isInKonro(ev->getPid())) {
        bus_.publish(new rmcommon::FeedbackEvent(ev->getPid(), ev->getFeedback()));

        ostringstream os;
        os << "WORKLOADMANAGER FeedbackRequest {"
           << "\"process_pid\":"
           << ev->getPid()
           << ",\"feedback_value\":" << '\'' << ev->getFeedback() << '\''
           << "}";
        cat_.info(os.str());
    } else {
        ostringstream os;
        os << "WORKLOADMANAGER FeedbackRequest received from process not in Konro {"
           << "\"process_pid\":"
           << ev->getPid()
           << ",\"feedback_value\":" << '\'' << ev->getFeedback() << '\''
           << "}";
        cat_.error(os.str());
    }
}

void WorkloadManager::dumpMonitoredApps()
{
    ostringstream os;
    os << "WORKLOADMANAGER monitoring PIDS [";
    bool first = true;
    for (auto &app: apps_) {
        if (first)
            first = false;
        else
            os << ",";
        os << app->getPid();
    }
    os << "]";
    cat_.info(os.str());
}

}   // namespace wm
