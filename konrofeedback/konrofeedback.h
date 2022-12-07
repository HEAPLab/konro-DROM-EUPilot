#ifndef KONROFEEDBACK_H
#define KONROFEEDBACK_H

#include <string>

/*!
 * \brief library for implementing integrated applications.
 * An integrated application must periodically send a feedback
 * message to Konro using the elements of this class.
 */
class KonroFeedback {

    int feedback_;

    std::string sendFeedbackHelper(const std::string &text);

public:
    KonroFeedback(int feedback) : feedback_(feedback)
      { }
    ~KonroFeedback() = default;

    void setFeedback(int feedback) noexcept {
        feedback_ = feedback;
    }

    /*!
     * Sends a feedback message to Konro in JSON format.
     */
    std::string sendFeedbackMessage();

};

#endif // KONROFEEDBACK_H
