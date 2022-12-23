#include "workloadmanager.h"
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

WorkloadManager::WorkloadManager(rmcommon::EventBus &bus, pc::IPlatformControl &pc) :
    rmcommon::BaseEventReceiver("WORKLOADMANAGER"),
    bus_(bus),
    platformControl_(pc),
    cat_(log4cpp::Category::getRoot()),
    apps_(appComp)
{
    subscribeToEvents();
}

WorkloadManager::AppSet::iterator WorkloadManager::findAppByPid(int pid)
{
    // create a temporary App as key for the search
    shared_ptr<rmcommon::App> key = rmcommon::App::makeApp(pid, rmcommon::App::AppType::UNKNOWN);
    return apps_.find(key);
}

void WorkloadManager::subscribeToEvents()
{
    using namespace rmcommon;

    bus_.subscribe<WorkloadManager, ForkEvent, BaseEvent>(this, &WorkloadManager::addEvent);
    bus_.subscribe<WorkloadManager, ExecEvent, BaseEvent>(this, &WorkloadManager::addEvent);
    bus_.subscribe<WorkloadManager, ExitEvent, BaseEvent>(this, &WorkloadManager::addEvent);
    bus_.subscribe<WorkloadManager, AddRequestEvent, BaseEvent>(this, &WorkloadManager::addEvent);
    bus_.subscribe<WorkloadManager, FeedbackRequestEvent, BaseEvent>(this, &WorkloadManager::addEvent);
}

void WorkloadManager::add(shared_ptr<rmcommon::App> app)
{
    apps_.insert(app);
    platformControl_.addApplication(app);
    bus_.publish(new rmcommon::AddEvent(app));
}

void WorkloadManager::remove(pid_t pid)
{
    WorkloadManager::AppSet::iterator it = findAppByPid(pid);
    if (it != end(apps_)) {
        bus_.publish(new rmcommon::RemoveEvent(*it));
        platformControl_.removeApplication(*it);
        apps_.erase(it);
    }
}

bool WorkloadManager::isInKonro(pid_t pid)
{
    return findAppByPid(pid) != end(apps_);
}

bool WorkloadManager::processEvent(std::shared_ptr<const rmcommon::BaseEvent> event)
{
    using namespace rmcommon;

    if (const ForkEvent *ev = dynamic_cast<const ForkEvent *>(event.get())) {
        processForkEvent(static_pointer_cast<const ForkEvent>(event));
    } else if (const ExecEvent *ev = dynamic_cast<const ExecEvent *>(event.get())) {
        processExecEvent(static_pointer_cast<const ExecEvent>(event));
    } else if (const ExitEvent *ev = dynamic_cast<const ExitEvent *>(event.get())) {
        processExitEvent(static_pointer_cast<const ExitEvent>(event));
    } else if (const AddRequestEvent *ev = dynamic_cast<const AddRequestEvent *>(event.get())) {
        processAddRequestEvent(static_pointer_cast<const AddRequestEvent>(event));
    } else if (const FeedbackRequestEvent *ev = dynamic_cast<const FeedbackRequestEvent *>(event.get())) {
        processFeedbackRequestEvent(static_pointer_cast<const FeedbackRequestEvent>(event));
    } else {
        cat_.error("WORKLOADMANAGER received wrong event: %s", event->getName().c_str());
    }
    return true;
}

void WorkloadManager::processForkEvent(std::shared_ptr<const rmcommon::ForkEvent> event)
{
    const struct proc_event *ev = reinterpret_cast<const struct proc_event *>(&event->data_[0]);

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

void WorkloadManager::processExecEvent(std::shared_ptr<const rmcommon::ExecEvent> event)
{
    const struct proc_event *ev = reinterpret_cast<const struct proc_event *>(&event->data_[0]);
    pid_t pid = ev->event_data.exec.process_pid;
    AppSet::iterator it = findAppByPid(pid);
    if (it != apps_.end()) {
        shared_ptr<rmcommon::App> app = *it;
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

void WorkloadManager::processExitEvent(std::shared_ptr<const rmcommon::ExitEvent> event)
{
    const struct proc_event *ev = reinterpret_cast<const struct proc_event *>(&event->data_[0]);

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

void WorkloadManager::processAddRequestEvent(std::shared_ptr<const rmcommon::AddRequestEvent> event)
{
    if(!isInKonro(event->getApp()->getPid())) {
        add(event->getApp());

        ostringstream os;
        os << "WORKLOADMANAGER AddRequest {"
           << "\"process_pid\":"
           << event->getApp()->getPid()
           << ",\"process_name\":" << '\'' << event->getApp()->getName() << '\''
           << "}";
        cat_.info(os.str());
        dumpMonitoredApps();
    } else {
        ostringstream os;
        os << "WORKLOADMANAGER AddRequest from process already in Konro {"
           << "\"process_pid\":"
           << event->getApp()->getPid()
           << ",\"process_name\":" << '\'' << event->getApp()->getName() << '\''
           << "}";
        cat_.info(os.str());
    }
}

void WorkloadManager::processFeedbackRequestEvent(std::shared_ptr<const rmcommon::FeedbackRequestEvent> event)
{
    AppSet::iterator it = findAppByPid(event->getPid());
    if (it != end(apps_)) {
        bus_.publish(new rmcommon::FeedbackEvent(*it, event->getFeedback()));

        ostringstream os;
        os << "WORKLOADMANAGER FeedbackRequest {"
           << "\"process_pid\":"
           << event->getPid()
           << ",\"feedback_value\":" << '\'' << event->getFeedback() << '\''
           << "}";
        cat_.info(os.str());
    } else {
        ostringstream os;
        os << "WORKLOADMANAGER FeedbackRequest received from process not in Konro {"
           << "\"process_pid\":"
           << event->getPid()
           << ",\"feedback_value\":" << '\'' << event->getFeedback() << '\''
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
