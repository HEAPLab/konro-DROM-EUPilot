#ifndef WORKLOADMANAGER_H
#define WORKLOADMANAGER_H

#include "app.h"
#include "iprocobserver.h"
#include "cgroup/cgroupcontrol.h"
#include <vector>
#include <memory>

namespace wm {
/*!
 * \brief The WorkloadManager class
 */
class WorkloadManager : public IProcObserver {
    /*! pid to monitor */
    int pid_;
    std::vector<std::shared_ptr<pc::App>> apps_;

    /*!
     * Processes a fork event.
     *
     * If a new process was forked by another process already handled by Konro,
     * the forked process is added to Konro.
     */
    void processForkEvent(std::uint8_t *data);

    /*!
     * Processes an exec event.
     *
     * When a process handled by Konro performs an exec (often following a fork),
     * the application name is substituted with the name of the new program.
     */
    void processExecEvent(std::uint8_t *data);

    /*!
     * Processes an exit event.
     *
     * If the exiting process was handled by Konro, the process is removed
     * from Konro's management.
     */
    void processExitEvent(std::uint8_t *data);

    void dumpApps();

    std::shared_ptr<pc::App> getApp(pid_t pid);

public:
    WorkloadManager(int pid);

    /*!
     * Adds the specified application under the management of Konro.
     * \param app the application to add
     */
    void add(std::shared_ptr<pc::App> app);

    /*!
     * Removes the specified application from the management of Konro.
     * \param app the application to remove
     */
    void remove(std::shared_ptr<pc::App> app);

    /*!
     * Removes the application with the specified pid from the management of Konro.
     * \param pid the pid of the application to remove
     */
    void remove(pid_t pid);

    /*!
     * Checks if the application with the specified pid is under Konro's management.
     * \param pid the pid of the application of interest
     * \return true if the app is managed by Konro
     */
    bool isInKonro(pid_t pid);


    virtual void update(std::uint8_t *data) override;
};

}   // namespace wm

#endif // WORKLOADMANAGER_H
