#include "konrohttp.h"
#include "threadname.h"
#include "../../lib/httplib/httplib.h"
#include "../../lib/json/json.hpp"
#include "procfeedbackevent.h"
#include <chrono>

using namespace std;

struct KonroHttp::KonroHttpImpl {
    log4cpp::Category &cat_;
    httplib::Server srv;
    rmcommon::IEventReceiver *resourcePolicies_;

    KonroHttpImpl() : cat_(log4cpp::Category::getRoot()) {
    }

    void setEventReceiver(rmcommon::IEventReceiver *er) {
        resourcePolicies_ = er;
    }

    /*!
     * Extracts the data from the JSON and sends a ProcFeedbackEvent
     * to the handler, i.e. the EventReceiver
     *
     * \param data the JSON in text format
     */
    void sendFeedbackEvent(const std::string &data) {
        if (!resourcePolicies_) {
            cat_.error("KONROHTTP parseJson: ResourcePolicies not set");
            return;
        }
        using namespace nlohmann;
        basic_json<> j = json::parse(data);
        if (!j.contains("pid")) {
            cat_.error("KONROHTTP missing \"pid\" in feedback message");
            return;
        }
        if (!j.contains("feedback")) {
            cat_.error("KONROHTTP missing \"feedback\" in feedback message");
            return;
        }
        long pid = j["pid"];
        bool feedback = j["feedback"];
        cat_.info("KONROHTTP sending feedback event to ResourcePolicies");
        resourcePolicies_->addEvent(make_shared<rmcommon::ProcFeedbackEvent>(pid, feedback));
    }

    void handleGet(const httplib::Request &req, httplib::Response &res){
        cat_.info("HTTP GET received");
        if (req.has_param("op")) {
            res.set_content("You have requested operation " + req.get_param_value("op") + "\r\n", "text/plain");
        } else {
            res.set_content("Hello World from Konro: please specify an operation", "text/plain");
        }
    }

    void handleKonroPost(const httplib::Request &req, httplib::Response &res, const httplib::ContentReader &content_reader){
        cat_.info("HTTP KONRO POST received");
        std::string body;
        content_reader([&](const char *data, size_t data_length) {
                body.append(data, data_length);
                return true;
            });
        res.set_content("You have sent a POST '" + body + "'\r\n", "text/plain");
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

KonroHttp::KonroHttp(rmcommon::IEventReceiver *rp) :
    pimpl_(new KonroHttpImpl),
    cat_(log4cpp::Category::getRoot())
{
  rmcommon::setThreadName("KONROHTTP");
}


KonroHttp::~KonroHttp()
{
}

void KonroHttp::start()
{
    httpThread_ = thread(&KonroHttp::run, this);
}

void KonroHttp::stop() {
    pimpl_->srv.stop();
    if (httpThread_.joinable()) {
        httpThread_.join();
    }
    cat_.info("KONROHTTP stopped");
}

void KonroHttp::setEventReceiver(rmcommon::IEventReceiver *er)
{
    pimpl_->setEventReceiver(er);
}

void KonroHttp::run()
{
    rmcommon::setThreadName("KONROHTTP");
    cat_.info("KONROHTTP thread starting");

    pimpl_->srv.Get("/konro", [this](const httplib::Request &req, httplib::Response &res) {
        this->pimpl_->handleGet(req, res);
    });

    /* Add new process under Konro's management */
    pimpl_->srv.Post("/konro", [this](const httplib::Request &req, httplib::Response &res, const httplib::ContentReader &content_reader) {
        this->pimpl_->handleKonroPost(req, res, content_reader);
    });

    /* Communication with integrated applications */
    pimpl_->srv.Post("/feedback", [this](const httplib::Request &req, httplib::Response &res, const httplib::ContentReader &content_reader) {
        this->pimpl_->handleFeedbackPost(req, res, content_reader);
    });

    pimpl_->srv.listen("localhost", 8080);

    cat_.info("KONROHTTP thread exiting");
}
