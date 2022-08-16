#ifndef IOCONTROL_H
#define IOCONTROL_H

#include "../cgroupcontrol.h"
#include "../numericvalue.h"
#include <string>
#include <map>

namespace pc {
/*!
 * \class a class for interacting with the cgroup IO controller
 */
class IOControl : public CGroupControl {
public:
    const int MAX_IO_CONTROL = -1;

    enum ControllerFile {
        STAT,   // read-only
        MAX     // read-write
    };
    enum IoMax {
        RBPS,   // Max read bytes per second
        WBPS,   // Max write bytes per second
        RIOPS,  // Max read IO operations per second
        WIOPS   // Max write IO operations per second
    };

private:
    static const char *controllerName_;
    static const std::map<ControllerFile, const char *> fileNamesMap_;
    static const std::map<IoMax, const char *> keyNames_;

    std::map<std::string, NumericValue> getIOHelper(ControllerFile cf, int major, int minor, std::shared_ptr<App> app);

public:

    /*!
     * Gets the IO usage statistics for the specified applications.
     *
     * Statistics are available as a set of key-value pairs.
     * Examples of available statistics:
     * rbytes: bytes read
     * wios: number of write IOs
     *
     * \param major the device major number
     * \param minor the device minor number
     * \param app the application of interest
     * \returns the IO usage statistics
     */
    std::map<std::string, NumericValue> getIOStat(int major, int minor, std::shared_ptr<App> app) {
        return getIOHelper(STAT, major, minor, app);
    }

    /*!
     * Sets an IO limit for the specified device and application.
     *
     * The device number is specified through a <major, minor> pair.
     * The IO resource to constraint is specified through a member of the IoMax struct.
     * To remove a limit, the caller must pass the MAX_IO_CONTROL constant as value argument.
     *
     * \example setIOMax(8, 16, WIOPS, 120, app1)
     *          Sets a write limit at 120 IOPS for device 8:16.
     *
     * \param major the device major number
     * \param minor the device minor number
     * \param ioMax the IO resource to limit
     * \param value the maximum value allowed for the IO resource
     * \param app the application to limit
     */
    void setIOMax(int major, int minor, IoMax ioMax, NumericValue value, std::shared_ptr<App> app);

    /*!
     * Gets the specified application's IO limits.
     *
     * Statistics are available as a set of key-value pairs.
     * Examples of available statistics:
     * usage_usec: total cpu time
     * user_usec: user cpu time
     *
     * \param app the application of interest
     * \returns the cpu time statistics
     */
    std::map<std::string, NumericValue> getIOMax(int major, int minor, std::shared_ptr<App> app) {
        return getIOHelper(MAX, major, minor, app);
    }
};

}   // namespace pc
#endif // IOCONTROL_H
