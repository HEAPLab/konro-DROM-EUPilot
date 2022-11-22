#include "konrohttp.h"
#include <chrono>
#include <functional>

using namespace std;

KonroHttp::KonroHttp() : cat_(log4cpp::Category::getRoot())
{
}

void KonroHttp::start()
{
    httpThread_ = thread(&KonroHttp::run, this);
}

void KonroHttp::run()
{
    cat_.info("HTTP thread starting");

    srv_.Get("/konro", [this](const httplib::Request &req, httplib::Response &res) {
        this->handleRequest(req, res);
    });

    srv_.listen("localhost", 8080);

    cat_.info("HTTP thread exiting");
}

void KonroHttp::handleRequest(const httplib::Request &req, httplib::Response &res)
{
    cat_.info("HTTP handling request");

    if (req.has_param("op")) {
        res.set_content("You have requested operation " + req.get_param_value("op"), "text/plain");
    } else {
        res.set_content("Hello World from Konro: please specify an operation", "text/plain");
    }
}
