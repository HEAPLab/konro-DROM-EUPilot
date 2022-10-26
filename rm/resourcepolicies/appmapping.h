#ifndef APPMAPPING_H
#define APPMAPPING_H

#include <app.h>
#include <memory>
#include "numericvalue.h"

/*!
 * \class encapsulates an application and adds information about
 * the resources it can use
 */
class AppMapping {
    std::shared_ptr<rmcommon::App> app_;
    /*! cpus that can execute the app */
    short coreNum_;
    /*! maximum cpu bandwidth limit */
    rmcommon::NumericValue cpuMax_;
    /*! memory nodes that can be used by the app */
    short memNodes_;
    /*! total amount of memory currently used by the app */
    int currentMemoryAmount_;
    /*! minimum amount of memory the app must always retain */
    int minMemory_;
    /*! memory usage hard limit for the app */
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
        coreNum_ = n;
    }

    short getCpu() {
        return coreNum_;
    }
};

#endif // APPMAPPING_H
