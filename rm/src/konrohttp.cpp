#include "konrohttp.h"
#include "threadname.h"
#include "../../lib/httplib/httplib.h"
#include <chrono>

using namespace std;

struct KonroHttp::KonroHttpImpl {
    httplib::Server srv;
    void handleRequest(const httplib::Request &req, httplib::Response &res){
        log4cpp::Category::getRoot().info("HTTP request received");
        if (req.has_param("op")) {
            res.set_content("You have requested operation " + req.get_param_value("op"), "text/plain");
        } else {
            res.set_content("Hello World from Konro: please specify an operation", "text/plain");
        }
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
        this->pimpl_->handleRequest(req, res);
    });

    pimpl_->srv.listen("localhost", 8080);

    cat_.info("KONROHTTP thread exiting");
}
