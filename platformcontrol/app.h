#ifndef APP_H
#define APP_H
#include <unistd.h>

namespace pc {
/*!
 * \class a class containing basic information about a generic application.
 */
class App {
public:
    enum AppType {
        STANDALONE,
        INTEGRATED,
        KUBERNETES
    };
private:
    pid_t pid_;
    AppType appType_;
public:
    App(pid_t pid, AppType appType) : pid_(pid), appType_(appType) {}
    ~App() = default;

    /*!
     * \brief Gets the pid of the application
     * \returns the pid of the application
     */
    pid_t getPid() const {
        return pid_;
    }

    /*!
     * \brief Gets the type of the application
     * \returns the type of the application
     */
    AppType getAppType() const {
        return appType_;
    }
};

}
#endif // APP_H
