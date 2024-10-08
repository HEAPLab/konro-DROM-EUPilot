#ifndef DROMRANDPOLICY_H
#define DROMRANDPOLICY_H

#include "ibasepolicy.h"
#include "drom/controllers/cpusetcontrol.h"


namespace rp {

/*!
 * \class random resource management policy
 *
 * Assigns each new process to a random CPU core.
 */
class DromRandPolicy : public IBasePolicy {
    PlatformDescription platformDescription_;
    pc::DromCpusetControl& cpuSetControl;
public:
    explicit DromRandPolicy(PlatformDescription pd);


    // IBasePolicy interface
    virtual const char *name() override {
        return "DromRandPolicy";
    }
    virtual void addApp(AppMappingPtr appMapping) override;
    virtual void removeApp(AppMappingPtr appMapping) override;
    virtual void timer() override;
    virtual void monitor(std::shared_ptr<const rmcommon::MonitorEvent> event) override;
    virtual void feedback(AppMappingPtr appMapping, int feedback) override;
    void start();
    void manageServerSocket();
    void manage_client_req(int sock);
};

}   // namespace rp

#endif // RANDPOLICY_H
