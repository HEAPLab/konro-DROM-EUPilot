#include "konrolib.h"

#include "../lib/httplib/httplib.h"
#include "../lib/json/json.hpp"
#include <string>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>

namespace konro {

namespace {

    std::string getServerAddress() {
        const char *envAddr = getenv("KONRO");
        if (envAddr == nullptr)
            return "http://localhost:8080";
        else
            return envAddr;
    }

    std::string sendPost(const std::string &msgType, const std::string &text) {
        httplib::Client cli(getServerAddress());
        std::string addr = "/" + msgType;
        httplib::Result result = cli.Post(addr.c_str(), text, "application/json");
        if (result) {
            const httplib::Response &response = result.value();
            return response.body;
        } else {
            return "";
        }
    }

}

int computeFeedback(int curValue, int target) {
    int feedback = (curValue * 100) / target;
    feedback = std::min(feedback, 200);
    feedback = std::max(feedback, 0);
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
