#include "platformmonitor.h"
#include <chrono>
#include <vector>
#include <iostream>
#include <cstring>
#include <sensors.h>
#include "componenttemperature.h"
#include "monitorevent.h"
#include "platformtemperature.h"
#include "platformpower.h"
#include "threadname.h"


using namespace std;

struct PlatformMonitor::PlatformMonitorImpl {

    ResourcePolicies &rp;
    bool initialized = false;
    rmcommon::PlatformTemperature platTemp;
    rmcommon::PlatformPower platPower;
    // Detected chip temperature sensors

    // Chip names
    sensors_chip_name const *cnCoretemp;
    sensors_chip_name const *cnK10temp;
    sensors_chip_name const *cnK8temp;
    sensors_chip_name const *cnViacputemp;

    // Chip subfeatures
    sensors_subfeature const *cnCoretempSubf;
    sensors_subfeature const *cnK10tempSubf;
    sensors_subfeature const *cnK8tempSubf;
    sensors_subfeature const *cnViatempSubf;

    PlatformMonitorImpl(ResourcePolicies &rp): rp(rp)
    {

    }

    void clear() {
        // Clear chip names
        this->cnCoretemp = nullptr;
        this->cnK10temp = nullptr;
        this->cnK8temp = nullptr;
        this->cnViacputemp = nullptr;
        // Clear chip subfeatures
        this->cnCoretempSubf = nullptr;
        this->cnK10tempSubf = nullptr;
        this->cnK8tempSubf = nullptr;
        this->cnViatempSubf = nullptr;
        this->initialized = false;
    }

    void handleBattery(sensors_chip_name const *cn) {
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
                    platPower.setBatteryCurrent(static_cast<int>(val));
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
    void handleCpuTemp(sensors_chip_name const *cn) {
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

    void init() {
        this->clear();
        this->initialized = sensors_init(NULL) == 0;
        if (!this->initialized)
            return;
    }

    /*!
     * Detects each sensor available on the machine and calls the
     * appropriate handler function to store information about its
     * current status.
     */
    void handleSensors() {
        sensors_chip_name const *cn;
        int c = 0;
        while ((cn = sensors_get_detected_chips(0, &c)) != 0) {
            char *prefix = cn->prefix;

            /* Cpu temperature sensor found */
            if ((strcmp(prefix, "coretemp")            // found Intel CPU temperature sensor
                    && strcmp(prefix, "k10temp")       // found AMD K10 CPU temperature sensor
                    && strcmp(prefix, "k8temp")        // found AMD K8 CPU temperature sensor
                    && strcmp(prefix, "via-cputemp"))  // found via CPU temperature sensor
                    == 0) {
                handleCpuTemp(cn);

            }
            /* Battery sensor found  */
            else if (strstr(prefix, "BAT") != nullptr) {
                handleBattery(cn);
            }
        }
    }

    void fini() {
        sensors_cleanup();
        this->clear();
    }
};

PlatformMonitor::PlatformMonitor(ResourcePolicies &rp, int monitorPeriod) :
    pimpl_(new PlatformMonitorImpl(rp)),
    cat_(log4cpp::Category::getRoot()),
    resourcePolicies_(rp),
    monitorPeriod_(monitorPeriod)
{
    rmcommon::setThreadName("PLATFORMMONITOR");
    pimpl_->init();
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

void PlatformMonitor::run()
{
    cat_.info("PLATFORMMONITOR: running");
    while (!stop_) {
        this_thread::sleep_for(chrono::seconds(monitorPeriod_));
        pimpl_->handleSensors();
        resourcePolicies_.addEvent(make_shared<rmcommon::MonitorEvent>
                                   (pimpl_->platTemp, pimpl_->platPower));
    }
    cat_.info("PLATFORMMONITOR: exiting");
}
