#ifndef APP_H
#define APP_H

#include <memory>
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

    App(pid_t pid, AppType appType) : pid_(pid), appType_(appType) {}

public:
    App(const App &rhs) = delete;
    App &operator = (const App &rhs) = delete;
    App(App &&rhs) noexcept = delete;
    App& operator=(App&& other) noexcept = delete;
    ~App() = default;

    /*!
     * \brief Factory function to create a shared_ptr to an App
     * \param pid the pid of the App
     * \param appType the type of the app
     * \returns the shared_ptr to the App
     */
    static std::shared_ptr<App> makeApp(pid_t pid, AppType appType) {
        // Note: to use std::make_shared, the constructor must be public;
        //       in this context it is better to use new App(...)
        return std::shared_ptr<App>(new App(pid, appType));
    }

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
