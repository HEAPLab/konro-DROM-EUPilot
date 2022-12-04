#include "platformmonitor.h"
#include <chrono>
#include <vector>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <sensors.h>
#include "componenttemperature.h"
#include "monitorevent.h"
#include "platformtemperature.h"
#include "platformpower.h"
#include "threadname.h"
#include "tsplit.h"

using namespace std;

struct PlatformMonitor::PlatformMonitorImpl {

    bool initialized = false;
    vector<string> cpuChips;
    vector<string> batteryChips;

    PlatformMonitorImpl() {
        init();
    }

    ~PlatformMonitorImpl() {
        fini();
    }

    void init() {
        this->initialized = sensors_init(NULL) == 0;
    }

    void fini() {
        if (this->initialized)
            sensors_cleanup();
    }
    void setCpuModuleNames(const std::string &names) {
        cpuChips = rmcommon::tsplit(names, ",");
    }

    void setBatteryModuleNames(const std::string &names) {
        batteryChips = rmcommon::tsplit(names, ",");
        log4cpp::Category::getRoot().info("setBatteryModuleNames: %s", names.c_str());
        log4cpp::Category::getRoot().info("setBatteryModuleNames: %s", batteryChips[0].c_str());
    }

    bool isCpuChip(string chip) {
        return find(begin(cpuChips), end(cpuChips), chip) != end(cpuChips);
    }

    bool isBatteryChip(string chip) {
        // Example:
        // chip is "BAT0"
        // batteryChips is [ "BAT" ]

        // remove trailing digits from string "chip"
        // This works because std::string::npos is (unsigned ..)-1
        // and npos+1 becomes 0 in case there are no trailing digits
        chip.erase(chip.find_last_not_of("0123456789")+1);
        return find(begin(batteryChips), end(batteryChips), chip) != end(batteryChips);
    }

    void handleBattery(sensors_chip_name const *cn, rmcommon::PlatformPower &platPower) {
        sensors_feature const *feat;
        int f = 0;
        while ((feat = sensors_get_features(cn, &f)) != 0) {
            if (feat->type == SENSORS_FEATURE_IN) {
                sensors_subfeature const *subf =
                        sensors_get_subfeature(cn, feat, SENSORS_SUBFEATURE_IN_INPUT);
                double val;
                int rc = sensors_get_value(cn, subf->number, &val);
                if (rc == 0)
                    platPower.setBatteryVoltage(static_cast<int>(val));
            } else if (feat->type == SENSORS_FEATURE_CURR) {
                sensors_subfeature const *subf =
                        sensors_get_subfeature(cn, feat, SENSORS_SUBFEATURE_CURR_INPUT);
                double val;
                int rc = sensors_get_value(cn, subf->number, &val);
                if (rc == 0)
                    platPower.setBatteryCurrent(static_cast<int>(val * 1000));
            }
        }
    }

    rmcommon::ComponentTemperature getTemperatureInfo(sensors_chip_name const *cn, sensors_feature const *feat) {
        rmcommon::ComponentTemperature componentTemp;
        componentTemp.label_ = sensors_get_label(cn, feat);
        componentTemp.num_ = feat->number;

        // Scan all subfeatures. Subfeatures are temperatures.
        sensors_subfeature const *subf;
        int s = 0;
        while ((subf = sensors_get_all_subfeatures(cn, feat, &s)) != 0) {
            int rc;
            double val;
            rc = sensors_get_value(cn, subf->number, &val);
            if (rc == 0) {
                switch(subf->type) {
                case SENSORS_SUBFEATURE_TEMP_INPUT:
                    componentTemp.temp_ = static_cast<int>(val);
                    break;
                case SENSORS_SUBFEATURE_TEMP_MAX:
                    componentTemp.maxTemp_ = static_cast<int>(val);
                    break;
                case SENSORS_SUBFEATURE_TEMP_MIN:
                    componentTemp.minTemp_ = static_cast<int>(val);
                    break;
                case SENSORS_SUBFEATURE_TEMP_HIGHEST:
                    componentTemp.highestTemp_ = static_cast<int>(val);
                    break;
                case SENSORS_SUBFEATURE_TEMP_LOWEST:
                    componentTemp.lowestTemp_ = static_cast<int>(val);
                    break;
                case SENSORS_SUBFEATURE_TEMP_CRIT:
                    componentTemp.critTemp_ = static_cast<int>(val);
                    break;
                case SENSORS_SUBFEATURE_TEMP_EMERGENCY:
                    componentTemp.emergencyTemp_ = static_cast<int>(val);
                    break;
                default:
                    break;
                }
            }
        }
        return componentTemp;
    }

    /*!
     * Extracts information from the specified CPU temperature sensor.
     */
    void handleCpuTemp(sensors_chip_name const *cn, rmcommon::PlatformTemperature &platTemp) {
        // Chip: coretemp - /sys/class/hwmon/hwmon3
        // 1: feature name is temp1
        // 1: feature label is Package id 0
        // 1:1: subfeature name is temp1_input/0 = 69
        // 1:2: subfeature name is temp1_max/1 = 105
        // 1:3: subfeature name is temp1_crit/2 = 105
        // 1:4: subfeature name is temp1_crit_alarm/3 = 0
        // 2: feature name is temp2
        // 2: feature label is Core 0
        // 2:5: subfeature name is temp2_input/4 = 65
        // 2:6: subfeature name is temp2_max/5 = 105
        // 2:7: subfeature name is temp2_crit/6 = 105
        // 2:8: subfeature name is temp2_crit_alarm/7 = 0
        // 3: feature name is temp3
        // 3: feature label is Core 1
        // 3:9: subfeature name is temp3_input/8 = 69
        // 3:10: subfeature name is temp3_max/9 = 105
        // 3:11: subfeature name is temp3_crit/10 = 105
        // 3:12: subfeature name is temp3_crit_alarm/11 = 0

        // Scan all features. Known features are: package(s) and core(s)
        sensors_feature const *feat;
        int f = 0;
        while ((feat = sensors_get_features(cn, &f)) != 0) {
            if (feat->type == SENSORS_FEATURE_TEMP) {
                rmcommon::ComponentTemperature componentTemp = getTemperatureInfo(cn, feat);
                char *label = componentTemp.label_;
                if (strstr(label, "Package") != nullptr) {
                    platTemp.addCpuTemperature(componentTemp);
                }
                else if (strstr(label, "Core") != nullptr) {
                    platTemp.addCoreTemperature(componentTemp);
               }
            }
        }
    }

    /*!
     * Detects each sensor available on the machine and calls the
     * appropriate handler function to store information about its
     * current status.
     */
    void handleSensors(rmcommon::PlatformTemperature &platTemp, rmcommon::PlatformPower &platPower) {
        sensors_chip_name const *cn;
        int c = 0;
        while ((cn = sensors_get_detected_chips(0, &c)) != 0) {
            if (isCpuChip(cn->prefix)) {
                // CPU temperature sensor found
                handleCpuTemp(cn, platTemp);
            }
            else if (isBatteryChip(cn->prefix)) {
                // Battery sensor found
                handleBattery(cn, platPower);
            }
        }
    }
};

PlatformMonitor::PlatformMonitor(rmcommon::EventBus &eventBus, int monitorPeriod) :
    pimpl_(new PlatformMonitorImpl()),
    bus_(eventBus),
    cat_(log4cpp::Category::getRoot()),
    monitorPeriod_(monitorPeriod)
{
    rmcommon::setThreadName("PLATFORMMONITOR");
}

PlatformMonitor::~PlatformMonitor()
{
    stop();
    pimpl_->fini();
}

/*!
 * \brief Starts the thred function "run()"
 */
void PlatformMonitor::start()
{
    stop_ = false;
    pmThread_ = thread(&PlatformMonitor::run, this);
}

/*!
 * \brief Stops and joins the thread
 */
void PlatformMonitor::stop()
{
    stop_ = true;
    if (pmThread_.joinable()) {
        pmThread_.join();
    }
}

void PlatformMonitor::setCpuModuleNames(const std::string &names)
{
    pimpl_->setCpuModuleNames(names);
}

void PlatformMonitor::setBatteryModuleNames(const std::string &names)
{
    pimpl_->setBatteryModuleNames(names);
}

void PlatformMonitor::run()
{
    cat_.info("PLATFORMMONITOR running");
    while (!stop_) {
        this_thread::sleep_for(chrono::seconds(monitorPeriod_));
        rmcommon::PlatformTemperature platTemp;
        rmcommon::PlatformPower platPower;
        pimpl_->handleSensors(platTemp, platPower);
        bus_.publish(new rmcommon::MonitorEvent(platTemp, platPower));
        //resourcePolicies_.addEvent(make_shared<rmcommon::MonitorEvent>(platTemp, platPower));
    }
    cat_.info("PLATFORMMONITOR exiting");
}
