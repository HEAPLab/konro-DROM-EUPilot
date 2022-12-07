#ifndef APP_H
#define APP_H

#include <memory>
#include <string>
#include <unistd.h>

namespace rmcommon {
/*!
 * \class a class containing basic information about a generic application.
 */
class App {
public:
    enum class AppType {
        UNKNOWN,
        STANDALONE,
        INTEGRATED
    };

private:
    pid_t pid_;
    AppType appType_;
    std::string name_;

    App(pid_t pid, AppType appType, std::string appName = "") :
        pid_(pid), appType_(appType), name_(appName) {}

public:
    typedef std::shared_ptr<App> AppPtr;

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
     * Return the app type with the specified name.
     * If no app type exists with that name, UNKNOWN is returned.
     */
    static AppType getTypeByName(const std::string &appType) {
        if (appType == "STANDALONE")
            return AppType::STANDALONE;
        else if (appType == "INTEGRATED")
            return AppType::INTEGRATED;
        else
            return AppType::UNKNOWN;
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

    /*!
     * \brief Gets the name of the application
     * \return the application's name
     */
    const std::string &getName() const {
        return name_;
    }

    /*!
     * \brief Sets the name of the application
     * \param name the application's name
     */
    void setName(const std::string &appName) {
        name_ = appName;
    }
};


}
#endif // APP_H
