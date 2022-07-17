#ifndef APP_H
#define APP_H
#include <unistd.h>

namespace pc {
/*!
 * \class a class containing basic information about a generic application.
 */
class App {

    enum AppType {
        STANDALONE,
        INTEGRATED,
        KUBERNETES
    };

    pid_t pid_;
    AppType appType_;
public:
    App(pid_t pid, AppType appType) : pid_(pid), appType_(appType) {}
    ~App() = default;
    pid_t getPid() const {
        return pid_;
    }
    AppType getAppType() const {
        return appType_;
    }
};

}
#endif // APP_H
