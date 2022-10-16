#ifndef APPINFO_H
#define APPINFO_H

#include <app.h>
#include <memory>

class AppInfo {
    std::shared_ptr<pc::App> app_;
    short cpu_;

public:
    explicit AppInfo(std::shared_ptr<pc::App> app);
    ~AppInfo() = default;


    /*!
     * \brief Gets the pid of the application
     * \returns the pid of the application
     */
    pid_t getPid() const {
        return app_->getPid();
    }

    std::shared_ptr<pc::App> getApp() const {
        return app_;
    }

    void setCpu(short n) {
        cpu_ = n;
    }

    short getCpu() {
        return cpu_;
    }
};

#endif // APPINFO_H
