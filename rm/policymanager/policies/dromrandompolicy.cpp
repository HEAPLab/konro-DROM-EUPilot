#include "cpuguard.h"
#include "dromrandpolicy.h"

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <dlb_drom.h>
#include <dlb_types.h>
#include <log4cpp/Category.hh>
#include <utility>
#include <vector>

using namespace std;

namespace rp {

vector<short> unpack_cpus(const vector<pair<short, short>> &cpus) {
  vector<short> ret;
  for (const auto &cpu : cpus) {
    for (int i = cpu.first; i <= cpu.second; i++) {
      ret.push_back(i);
    }
  }
  std::sort(ret.begin(), ret.end());
  return ret;
}

vector<pair<short, short>> pack_cpus(const vector<short> &cpus) {
  vector<pair<short, short>> ret;
  for (const auto &cpu : cpus) {
    ret.push_back({cpu, cpu});
  }
  return ret;
}

DromRandPolicy::DromRandPolicy(PlatformDescription pd)
    : platformDescription_(pd),
      cpuSetControl(pc::DromCpusetControl::instance(
          platformDescription_.getNumProcessingUnits())) {}

void DromRandPolicy::addApp(AppMappingPtr appMapping) {
  log4cpp::Category::getRoot().debug("Add PID %i to Konro",
                                     appMapping->getPid());

  auto app = appMapping->getApp();

  auto ticket = cpuSetControl.reserveCpus(app, 1);
  if (ticket.size() == 1) {
    // ticket.apply();
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(ticket.leakOcc(), &mask);
    int i = sched_setaffinity(app->getPid(), sizeof(cpu_set_t), &mask);
    cerr << i << "\n";
  } else {
    // TODO: RUBARE CPU a chi ne ha troppe
  }
  cpuSetControl.print_drom_list();
  return;
}

void DromRandPolicy::removeApp(AppMappingPtr appMapping) {
  // no action required
  cpuSetControl.release(appMapping->getApp());
  DLB_DROM_PostFinalize(appMapping->getPid(), DLB_RETURN_STOLEN);
  log4cpp::Category::getRoot().debug("Remove request");
}

void DromRandPolicy::timer() {
  // no action required
}

void DromRandPolicy::monitor(
    [[maybe_unused]] std::shared_ptr<const rmcommon::MonitorEvent> event) {
  // no action required
}

void DromRandPolicy::feedback(AppMappingPtr appMapping, int feedback) {
  auto app = appMapping->getApp();
  auto cpus = cpuSetControl.getCpus(app);
  auto pu = unpack_cpus(cpus);
  if (feedback < 70) {
    cpuSetControl.reserveCpus(app, pu.size() + 1);
  } else if (feedback > 130) {
    cpuSetControl.reserveCpus(app, pu.size() - 1);
  } else {
    return;
  }
}

} // namespace rp
