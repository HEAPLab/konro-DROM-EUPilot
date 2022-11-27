#include "konrofeedback.h"

#include "../lib/httplib/httplib.h"
#include "../lib/json/json.hpp"
#include <string>

using json = nlohmann::json;

struct KonroFeedback::KonroFeedbackImpl {
    std::string sendMessage(const std::string &text) {
        std::string body;
        httplib::Client cli("http://localhost:8080");
        httplib::Result result = cli.Post("/feedback", text, "application/json");
        const httplib::Response &response = result.value();
        return response.body;
    }
};

KonroFeedback::KonroFeedback(bool feedback) :
    pimpl_(new KonroFeedbackImpl()),
    feedback_(feedback)
{
}

KonroFeedback::~KonroFeedback()
{
}

std::string KonroFeedback::send()
{
    json j;
    j["operation"] = "feedback";
    j["feedback"] = feedback_;
    return pimpl_->sendMessage(j.dump());
}
