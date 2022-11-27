#include "konrohttp.h"
#include "threadname.h"
#include "../../lib/httplib/httplib.h"
#include "../../lib/json/json.hpp"
#include <chrono>

using namespace std;

struct KonroHttp::KonroHttpImpl {
    httplib::Server srv;

    void parseJson(const std::string &data) {
        using namespace nlohmann;
        basic_json<> j = json::parse(data);
        for (auto &el : j.items()) {
            ostringstream os;
            os << "Key: " << el.key() << ", Value: " << el.value();
            if (el.key() == "id") {
                int id = j["id"];
                os << "(id is " << id << ")";
            }
            log4cpp::Category::getRoot().info(os.str());
        }
    }

    void handleGet(const httplib::Request &req, httplib::Response &res){
        log4cpp::Category::getRoot().info("HTTP GET received");
        if (req.has_param("op")) {
            res.set_content("You have requested operation " + req.get_param_value("op") + "\r\n", "text/plain");
        } else {
            res.set_content("Hello World from Konro: please specify an operation", "text/plain");
        }
    }

    void handleKonroPost(const httplib::Request &req, httplib::Response &res, const httplib::ContentReader &content_reader){
        log4cpp::Category::getRoot().info("HTTP KONRO POST received");
        std::string body;
        content_reader([&](const char *data, size_t data_length) {
                body.append(data, data_length);
                return true;
            });
        res.set_content("You have send a KONRO POST '" + body + "'\r\n", "text/plain");
        parseJson(body);
    }

    void handleFeedbackPost(const httplib::Request &req, httplib::Response &res, const httplib::ContentReader &content_reader){
        log4cpp::Category::getRoot().info("HTTP FEEDBACK POST received");
        std::string body;
        content_reader([&](const char *data, size_t data_length) {
                body.append(data, data_length);
                return true;
            });
        res.set_content("You have sent a FEEDBACK POST '" + body + "'\r\n", "text/plain");
        parseJson(body);
    }
};

KonroHttp::KonroHttp() :
    pimpl_(new KonroHttpImpl),
    cat_(log4cpp::Category::getRoot())
{
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
}

void KonroHttp::run()
{
    rmcommon::setThreadName("KONROHTTP");
    cat_.info("KONROHTTP thread starting");

    pimpl_->srv.Get("/konro", [this](const httplib::Request &req, httplib::Response &res) {
        this->pimpl_->handleGet(req, res);
    });

    pimpl_->srv.Post("/konro", [this](const httplib::Request &req, httplib::Response &res, const httplib::ContentReader &content_reader) {
        this->pimpl_->handleKonroPost(req, res, content_reader);
    });

    pimpl_->srv.Post("/feedback", [this](const httplib::Request &req, httplib::Response &res, const httplib::ContentReader &content_reader) {
        this->pimpl_->handleFeedbackPost(req, res, content_reader);
    });

    pimpl_->srv.listen("localhost", 8080);

    cat_.info("KONROHTTP thread exiting");
}
