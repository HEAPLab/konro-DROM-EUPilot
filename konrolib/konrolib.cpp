#include "konrolib.h"

#include "../lib/httplib/httplib.h"
#include "../lib/json/json.hpp"
#include <string>
#include <sys/types.h>
#include <unistd.h>

namespace konro {

namespace {

    std::string sendPost(const std::string &msgType, const std::string &text) {
        httplib::Client cli("http://localhost:8080");
        std::string addr = "/" + msgType;
        httplib::Result result = cli.Post(addr.c_str(), text, "application/json");
        const httplib::Response &response = result.value();
        return response.body;
    }

}

int computeFeedback(int curValue, int target) {
    int feedback = (curValue * 100) / target;
    if (feedback > 200)
        return 200;
    if (feedback < 0)
        return 0;
    return feedback;
}


std::string sendFeedbackMessage(int feedback)
{
    nlohmann::json j;
    j["pid"] = getpid();
    j["feedback"] = feedback;
    return sendPost("feedback", j.dump());
}

std::string sendAddMessage()
{
    nlohmann::json j;
    j["pid"] = getpid();
    j["type"] = "INTEGRATED";
    return sendPost("add", j.dump());
}

}   // namespace feedback
