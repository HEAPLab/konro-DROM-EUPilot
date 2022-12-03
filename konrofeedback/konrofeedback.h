#ifndef KONROFEEDBACK_H
#define KONROFEEDBACK_H

#include <string>

/*!
 * \brief library for implementing integrated applications.
 * An integrated application must periodically send a feedback
 * message to Konro using the elements of this class.
 */
class KonroFeedback {
    /*
     * 0 = the current QoS level is sufficient
     * 1 = the current QoS level is insufficient
     */
    bool feedback_;

    std::string sendFeedbackHelper(const std::string &text);

public:
    KonroFeedback(bool feedback) : feedback_(feedback)
      { }
    ~KonroFeedback() = default;

    void setFeedback(bool feedback) noexcept {
        feedback_ = feedback;
    }

    /*!
     * Sends a feedback message to Konro in JSON format.
     */
    std::string sendFeedbackMessage();

};

#endif // KONROFEEDBACK_H
