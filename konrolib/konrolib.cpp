#include "konrolib.h"

#include "../lib/httplib/httplib.h"
#include "../lib/json/json.hpp"
#include <string>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>

#ifdef TIMING
#include <chrono>
#include <iostream>
#endif

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
#ifdef TIMING
    using namespace std::chrono;
    high_resolution_clock::time_point _start_ = high_resolution_clock::now();
    microseconds us;
#endif

    nlohmann::json j;
    j["pid"] = getpid();
    j["type"] = "INTEGRATED";
    std::string out = sendPost("add", j.dump());

#ifdef TIMING
    high_resolution_clock::time_point _end_ = high_resolution_clock::now();
    microseconds elapsed = std::chrono::duration_cast<microseconds>(_end_ - _start_);
    std::cout << "KONROLIB timing: sendAddMessage = "
              << elapsed.count()
              << " microseconds"
              << std::endl;
#endif
    return out;
}

}   // namespace feedback
