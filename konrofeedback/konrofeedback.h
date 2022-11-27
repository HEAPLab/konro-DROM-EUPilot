#ifndef KONROFEEDBACK_H
#define KONROFEEDBACK_H

#include <memory>
#include <string>

class KonroFeedback {
    // hide implementation
    struct KonroFeedbackImpl;
    std::unique_ptr<KonroFeedbackImpl> pimpl_;
    bool feedback_;
public:
    KonroFeedback(bool feedback);
    ~KonroFeedback();

    void setFeedback(bool feedback) noexcept {
        feedback_ = feedback;
    }

    std::string send();
};

#endif // KONROFEEDBACK_H
