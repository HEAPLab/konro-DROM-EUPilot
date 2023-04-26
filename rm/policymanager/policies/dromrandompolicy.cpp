#include "cpusetvector.h"
#include "dromrandpolicy.h"

#include <iostream>
#include <log4cpp/Category.hh>
#include <random>
#include <sstream>
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
      cpuSetControl(pc::DromCpusetControl::instance()) {}

void DromRandPolicy::addApp(AppMappingPtr appMapping) {
  // If there are already other Apps in the same cgroup folder,
  // handle them as a group and do nothing here
  log4cpp::Category::getRoot().debug("Add request");
  cpuSetControl.print_drom_list();
  vector<pair<short, short>> cpus{std::make_pair(1, 1)};
  
  auto app = appMapping->getApp();

  cpuSetControl.setCpus(cpus, app);
  
  return;
}

void DromRandPolicy::removeApp(AppMappingPtr appMapping) {
  // no action required
  log4cpp::Category::getRoot().debug("Remove request");
}

void DromRandPolicy::timer() {
  // no action required
}

void DromRandPolicy::monitor(
    std::shared_ptr<const rmcommon::MonitorEvent> event) {
  // no action required
}

void DromRandPolicy::feedback(AppMappingPtr appMapping, int feedback) {
  auto cpunum = platformDescription_.getNumProcessingUnits();
  auto app = appMapping->getApp();
  auto cpus = cpuSetControl.getCpus(app);
  auto pu = unpack_cpus(cpus);
  if (feedback < 70 ) {
    short last = pu.back();
    if (last + 1 < cpunum) {
      pu.push_back(last + 1);
    }
  }else
  if (feedback > 130 && pu.size() > 1) {
    pu.pop_back();   
  }else{
    return;
  }

  cpuSetControl.setCpus(pack_cpus(pu), appMapping->getApp());
}

} // namespace rp
