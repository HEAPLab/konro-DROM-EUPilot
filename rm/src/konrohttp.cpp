#include "konrohttp.h"
#include "threadname.h"
#include "../../lib/httplib/httplib.h"
#include "../../lib/json/json.hpp"
#include "feedbackrequestevent.h"
#include "addrequestevent.h"
#include "app.h"

#ifdef TIMING
#include "timer.h"
#endif

using namespace std;

namespace http {

struct KonroHttp::KonroHttpImpl {
    rmcommon::EventBus &bus_;
    log4cpp::Category &cat_;
    httplib::Server srv;

    KonroHttpImpl(rmcommon::EventBus &eventBus) :
        bus_(eventBus),
        cat_(log4cpp::Category::getRoot()) {
    }

    bool isInJson(nlohmann::basic_json<> j, const char *param) {
        if (!j.contains(param)) {
            cat_.error("KONROHTTP missing \"%s\" in feedback message", param);
            return false;
        }
        return true;
    }

    /*!
     * Extracts the data from the JSON and publishes a FeedbackRequestEvent
     *
     * \param data the JSON in text format
     */
    void sendFeedbackEvent(const std::string &data) {
        rmcommon::KonroTimer::TimePoint tp = rmcommon::KonroTimer::now();

        using namespace nlohmann;

        basic_json<> j = json::parse(data);
        if (isInJson(j, "pid") && isInJson(j, "feedback")) {
            long pid = j["pid"];
            int feedback = j["feedback"];
            cat_.info("KONROHTTP publishing FeedbackRequestEvent from pid %ld", pid);
            rmcommon::FeedbackRequestEvent *event = new rmcommon::FeedbackRequestEvent(pid, feedback);
            event->setTimePoint(tp);
            bus_.publish(event);
        }
   }

    /*!
     * Extracts the data from the JSON and publishes an AddRequestEvent
     *
     * \param data the JSON in text format
     */
    void sendAddEvent(const std::string &data) {
        using namespace nlohmann;
        rmcommon::KonroTimer::TimePoint tp = rmcommon::KonroTimer::now();
        basic_json<> j = json::parse(data);
        if (isInJson(j, "pid") && isInJson(j, "type")) {
            long pid = j["pid"];
            string type = j["type"];
            // optional "name"
            string name;
            if (j.contains("name"))
                name = j["name"];
            rmcommon::App::AppType appType = rmcommon::App::getTypeByName(type);
            if (appType == rmcommon::App::AppType::UNKNOWN) {
                cat_.error("KONROHTTP invalid application type %s", type.c_str());
                return;
            }
            cat_.info("KONROHTTP publishing AddRequestEvent for pid %ld", pid);
            rmcommon::AddRequestEvent *event = new rmcommon::AddRequestEvent(rmcommon::App::makeApp(pid, appType, name));
            event->setTimePoint(tp);
            bus_.publish(event);
        }
    }

    void handleGet(const httplib::Request &req, httplib::Response &res) {
        cat_.info("HTTP GET received");
        res.status = 200;
        res.set_content("200 - OK\r\n", "text/html");
    }

    /*!
     * \brief handles the addition of a new process to Konro
     */
    void handleAddPost(const httplib::Request &req, httplib::Response &res, const httplib::ContentReader &content_reader){
        cat_.info("KONROHTTP ADD POST received");
#ifdef TIMING
        cat_.debug("KONROHTTP timing: ADD POST received at %ld system microseconds",
                   rmcommon::KonroTimer::getSystemMicroseconds());
#endif
        std::string body;
        content_reader([&](const char *data, size_t data_length) {
                body.append(data, data_length);
                return true;
            });
        res.set_content("You have sent an ADD POST '" + body + "'\r\n", "text/plain");
        sendAddEvent(body);
    }

    /*!
     * \brief handles a feedback message sent by an integrated application
     */
    void handleFeedbackPost(const httplib::Request &req, httplib::Response &res, const httplib::ContentReader &content_reader){
        cat_.info("KONROHTTP FEEDBACK POST received");
        std::string body;
        content_reader([&](const char *data, size_t data_length) {
                body.append(data, data_length);
                return true;
            });
        res.set_content("You have sent a FEEDBACK POST '" + body + "'\r\n", "text/plain");
        sendFeedbackEvent(body);
    }
};

KonroHttp::KonroHttp(rmcommon::EventBus &eventBus, const char *listen_host, int listen_port) :
    pimpl_(new KonroHttpImpl(eventBus)),
    cat_(log4cpp::Category::getRoot()),
    listen_host_(listen_host),
    listen_port_(listen_port)
{
}

KonroHttp::~KonroHttp()
{
}

void KonroHttp::stop()
{
    pimpl_->srv.stop();
    BaseThread::stop();     // not really needed here because there is no loop
    cat_.info("KONROHTTP server stopped");
}

void KonroHttp::run()
{
    setThreadName("KONROHTTP");
    cat_.info("KONROHTTP thread starting");

    /* Test if server is running */
    pimpl_->srv.Get("/status", [this](const httplib::Request &req, httplib::Response &res) {
        this->pimpl_->handleGet(req, res);
    });

    /* Add new process under Konro's management */
    pimpl_->srv.Post("/add", [this](const httplib::Request &req, httplib::Response &res, const httplib::ContentReader &content_reader) {
        this->pimpl_->handleAddPost(req, res, content_reader);
    });

    /* Communication with integrated applications */
    pimpl_->srv.Post("/feedback", [this](const httplib::Request &req, httplib::Response &res, const httplib::ContentReader &content_reader) {
        this->pimpl_->handleFeedbackPost(req, res, content_reader);
    });

    cat_.info("KONROHTTP server starting");

    pimpl_->srv.listen(listen_host_.c_str(), listen_port_);

    cat_.info("KONROHTTP thread exiting");
}

}   // namespace http
