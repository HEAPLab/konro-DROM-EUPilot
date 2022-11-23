#include "konrohttp.h"
#include "threadname.h"
#include "../../lib/httplib/httplib.h"
#include <chrono>

using namespace std;

struct KonroHttp::KonroHttpImpl {
    httplib::Server srv;

    void handleGet(const httplib::Request &req, httplib::Response &res){
        log4cpp::Category::getRoot().info("HTTP GET received");
        if (req.has_param("op")) {
            res.set_content("You have requested operation " + req.get_param_value("op") + "\r\n", "text/plain");
        } else {
            res.set_content("Hello World from Konro: please specify an operation", "text/plain");
        }
    }

    void handlePost(const httplib::Request &req, httplib::Response &res, const httplib::ContentReader &content_reader){
        log4cpp::Category::getRoot().info("HTTP POST received");
        std::string body;
        content_reader([&](const char *data, size_t data_length) {
                body.append(data, data_length);
                return true;
            });
        res.set_content("You have requested a POST '" + body + "'\r\n", "text/plain");
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
        this->pimpl_->handlePost(req, res, content_reader);
    });

    pimpl_->srv.listen("localhost", 8080);

    cat_.info("KONROHTTP thread exiting");
}
