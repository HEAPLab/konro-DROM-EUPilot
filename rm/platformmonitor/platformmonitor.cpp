#include "platformmonitor.h"
#include <chrono>
#include <vector>
#include <iostream>
#include <cstring>
#include <sensors.h>
#include "coretemperature.h"
#include "monitorevent.h"

using namespace std;

struct PlatformMonitor::PlatformMonitorImpl {

    ResourcePolicies &rp;
    bool initialized = false;
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

    PlatformMonitorImpl(ResourcePolicies &rp): rp(rp) {

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

    void handleCoretemp(sensors_chip_name const *cn) {
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

        shared_ptr<rmcommon::MonitorEvent> monitorEvent = make_shared<rmcommon::MonitorEvent>();

        cout << "PlatformMonitor: found Intel CPU temperature sensor" << endl;

        this->cnCoretemp = cn;

        // Scan all features. Features are: package(s) and core(s)
        sensors_feature const *feat;
        int f = 0;
        while ((feat = sensors_get_features(cn, &f)) != 0) {

            if (feat->type != SENSORS_FEATURE_TEMP)
                continue;

            const char *label = sensors_get_label(cn, feat);
            cout << "handleCoretemp: handling label " << label << endl;
            if (strstr(label, "Package") != nullptr) {
                cout << "handleCoretemp: found package\n";
            } else if (strstr(label, "Core") != nullptr) {
                cout << "handleCoretemp: found core\n";
            } else {
                continue;
            }

            rmcommon::CoreTemperature coreTemp;
            coreTemp.label_ = label;
            coreTemp.num_ = feat->number;

            // Scan all subfeatures. Subfeatures are temperatures.
            sensors_subfeature const *subf;
            int s = 0;
            while ((subf = sensors_get_all_subfeatures(cn, feat, &s)) != 0) {
                int rc;
                double val;
                if (strstr(subf->name, "_input") != nullptr) {
                    rc = sensors_get_value(cn, subf->number, &val);
                    if (rc == 0) {
                        coreTemp.temp_ = static_cast<int>(val);
                    }
                } else if (strstr(subf->name, "_max") != nullptr) {
                    rc = sensors_get_value(cn, subf->number, &val);
                    if (rc == 0) {
                        coreTemp.maxTemp_ = static_cast<int>(val);
                    }
                } else if (strstr(subf->name, "_crit") != nullptr) {
                    rc = sensors_get_value(cn, subf->number, &val);
                    if (rc == 0) {
                        coreTemp.critTemp_ = static_cast<int>(val);
                    }
                }
            }
            monitorEvent->addCoreTemperature(coreTemp);
        }
        rp.addEvent(monitorEvent);
    }

    void handleK10temp(sensors_chip_name const *cn) {
        cout << "PlatformMonitor: found AMD K10 CPU temperature sensor" << endl;
        // TODO
    }

    void handleK8temp(sensors_chip_name const *cn) {
        cout << "PlatformMonitor: found AMD K8 CPU temperature sensor" << endl;
        // TODO
    }

    void handleViatemp(sensors_chip_name const *cn) {
        cout << "PlatformMonitor: found VIA CPU temperature sensor" << endl;
        // TODO
    }

    void init() {
        this->clear();
        this->initialized = sensors_init(NULL) == 0;
        if (!this->initialized)
            return;
    }

    void handleSensors() {
        sensors_chip_name const *cn;
        int c = 0;
        while ((cn = sensors_get_detected_chips(0, &c)) != 0) {

            if (strcmp(cn->prefix, "coretemp") == 0) {

                // found Intel CPU temperature sensor
                this->handleCoretemp(cn);

            } else if (strcmp(cn->prefix, "k10temp") == 0) {

                // found AMD K10 CPU temperature sensor
                this->handleK10temp(cn);

            } else if (strcmp(cn->prefix, "k8temp") == 0) {

                // found AMD K8 CPU temperature sensor
                this->handleK8temp(cn);

            } else if (strcmp(cn->prefix, "via-cputemp") == 0) {

                // found via CPU temperature sensor
                this->handleK8temp(cn);
            }
        }
    }

    void fini() {
        sensors_cleanup();
        this->clear();
    }
};

PlatformMonitor::PlatformMonitor(ResourcePolicies &rp) :
    pimpl_(new PlatformMonitorImpl(rp)),
    resourcePolicies_(rp)
{
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
    while (!stop_) {
        this_thread::sleep_for(chrono::milliseconds(1000));
        pimpl_->handleSensors();

        // TODO - generate an event

        // TODO - add event to ResourcePolicies queue

        //resourcePolicies_.addEvent(...);
    }
}
