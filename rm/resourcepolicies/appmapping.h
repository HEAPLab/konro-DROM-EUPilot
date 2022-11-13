#ifndef APPMAPPING_H
#define APPMAPPING_H

#include <app.h>
#include <memory>
#include "cpusetcontrol.h"
#include "numericvalue.h"

/*!
 * \class encapsulates an application and adds information about
 * the resources it can use
 */
class AppMapping {
    std::shared_ptr<rmcommon::App> app_;
    /*! Processing Units that can execute the app */
    rmcommon::CpusetVector puNum_;
    /*! maximum cpu bandwidth limit */
    rmcommon::NumericValue cpuMax_;
    /*! memory nodes that can be used by the app */
    rmcommon::CpusetVector memNodes_;
    /*! total amount of memory currently used by the app */
    int currentMemory_;
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

    void setPuVector(rmcommon::CpusetVector puVec) {
        puNum_ = puVec;
    }

    rmcommon::CpusetVector getPuVector() {
        return puNum_;
    }

    void setCpuMax(rmcommon::NumericValue cpuMax) {
        cpuMax_ = cpuMax;
    }

    rmcommon::NumericValue getCpuMax() {
        return cpuMax_;
    }

    void setMemNodes(rmcommon::CpusetVector memNodes) {
        memNodes_ = memNodes;
    }

    rmcommon::CpusetVector getMemNodes() {
        return memNodes_;
    }

    void setCurrentMem(int currentMemory) {
        currentMemory_ = currentMemory;
    }

    int getCurrentMem() {
        return currentMemory_;
    }

    void getMinMemory(int minMemory) {
        minMemory_ = minMemory;
    }

    int setMinMemory() {
        return minMemory_;
    }

    void getMaxMemory(int maxMemory) {
        maxMemory_ = maxMemory;
    }

    int setMaxMemory() {
        return maxMemory_;
    }

};

#endif // APPMAPPING_H
