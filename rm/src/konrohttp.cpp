#include "konrohttp.h"
#include "threadname.h"
#include "../../lib/httplib/httplib.h"
#include "../../lib/json/json.hpp"
#include "feedbackrequestevent.h"
#include "addrequestevent.h"
#include "app.h"
#include <chrono>

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
        else
            return true;
    }

    /*!
     * Extracts the data from the JSON and publishes a FeedbackRequestEvent
     *
     * \param data the JSON in text format
     */
    void sendFeedbackEvent(const std::string &data) {
        using namespace nlohmann;
        basic_json<> j = json::parse(data);
        if (isInJson(j, "pid") && isInJson(j, "feedback")) {
            long pid = j["pid"];
            bool feedback = j["feedback"];
            cat_.info("KONROHTTP publishing FeedbackRequestEvent from pid %ld", pid);
            bus_.publish(new rmcommon::FeedbackRequestEvent(pid, feedback));
        }
   }

    /*!
     * Extracts the data from the JSON and publishes an AddRequestEvent
     *
     * \param data the JSON in text format
     */
    void sendAddEvent(const std::string &data) {
        using namespace nlohmann;
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
            bus_.publish(new rmcommon::AddRequestEvent(rmcommon::App::makeApp(pid, appType, name)));
        }
   }

    void handleGet(const httplib::Request &req, httplib::Response &res) {
        cat_.info("HTTP GET received");
        if (req.has_param("op")) {
            res.set_content("You have requested operation " + req.get_param_value("op") + "\r\n", "text/plain");
        } else {
            res.set_content("Hello World from Konro: please specify an operation", "text/plain");
        }
    }

    /*!
     * \brief handles the addition of a new process to Konro
     */
    void handleAddPost(const httplib::Request &req, httplib::Response &res, const httplib::ContentReader &content_reader){
        cat_.info("HTTP ADD POST received");
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
        cat_.info("HTTP FEEDBACK POST received");
        std::string body;
        content_reader([&](const char *data, size_t data_length) {
                body.append(data, data_length);
                return true;
            });
        res.set_content("You have sent a FEEDBACK POST '" + body + "'\r\n", "text/plain");
        sendFeedbackEvent(body);
    }
};

KonroHttp::KonroHttp(rmcommon::EventBus &eventBus) :
    pimpl_(new KonroHttpImpl(eventBus)),
    cat_(log4cpp::Category::getRoot())
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

    pimpl_->srv.Get("/konro", [this](const httplib::Request &req, httplib::Response &res) {
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

    pimpl_->srv.listen("localhost", 8080);

    cat_.info("KONROHTTP thread exiting");
}

}   // namespace http
