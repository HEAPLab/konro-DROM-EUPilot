#ifndef APPMAPPING_H
#define APPMAPPING_H

#include <app.h>
#include <memory>
#include "../platformcontrol/utilities/numericvalue.h"


class AppMapping {
    std::shared_ptr<rmcommon::App> app_;
    short coresNum_;
    pc::NumericValue cpuMax_;
    short memNodes_;
    int currentMemoryAmount_;
    int minMemory_;
    int maxMemory_;

public:
    typedef std::shared_ptr<AppMapping> AppMappingPtr;

    explicit AppMapping(std::shared_ptr<rmcommon::App> app);
    ~AppMapping() = default;

    /*!
     * \brief Gets the pid of the application
     * \returns the pid of the application
     */
    pid_t getPid() const {
        return app_->getPid();
    }

    std::shared_ptr<rmcommon::App> getApp() const {
        return app_;
    }

    void setCpu(short n) {
        coresNum_ = n;
    }

    short getCpu() {
        return coresNum_;
    }
};

#endif // APPMAPPING_H
