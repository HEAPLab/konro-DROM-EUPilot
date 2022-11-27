#include "konrofeedback.h"

#include "../lib/httplib/httplib.h"
#include "../lib/json/json.hpp"
#include <string>
#include <sys/types.h>
#include <unistd.h>

std::string KonroFeedback::send()
{
    nlohmann::json j;
    j["pid"] = getpid();
    j["feedback"] = feedback_;
    return sendFeedbackMessage(j.dump());
}

std::string KonroFeedback::sendFeedbackMessage(const std::string &text) {
    std::string body;
    httplib::Client cli("http://localhost:8080");
    httplib::Result result = cli.Post("/feedback", text, "application/json");
    const httplib::Response &response = result.value();
    return response.body;
}
