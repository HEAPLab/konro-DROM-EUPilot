#ifndef KONROFEEDBACK_H
#define KONROFEEDBACK_H

#include <string>

class KonroFeedback {
    bool feedback_;

    std::string sendFeedbackMessage(const std::string &text);

public:
    KonroFeedback(bool feedback) : feedback_(feedback)
      { }
    ~KonroFeedback() = default;

    void setFeedback(bool feedback) noexcept {
        feedback_ = feedback;
    }

    std::string send();
};

#endif // KONROFEEDBACK_H
