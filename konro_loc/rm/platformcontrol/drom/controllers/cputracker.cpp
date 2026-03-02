
#include "cputracker.h"
#include "app.h"
#include "cpuguard.h"
#include <algorithm>
#include <cstddef>
#include <cstdlib>

namespace pc {



  template < typename Iterable, typename Elem>

   bool contains (Iterable && iterable, Elem e)  {
    return std::find(iterable.cbegin(), iterable.cend(), e) != iterable.end();
  }


bool CPUTracker::isOccPid(pid_t pid, cpu_t elem) {
  const auto& cpuVec = occCPU[pid];
  return contains(cpuVec, elem);
}


 bool CPUTracker::isOcc(cpu_t elem) const {
  bool f = contains(initCPU, elem);
  bool s = false;
  auto begin = occCPU.cbegin();
  auto end = occCPU.cend();
  for (; begin != end; ++begin){
    auto& cpuVec = begin->second;
    if ( contains(cpuVec, elem) ){
      s = true;
      break;
    }
  }
  return f && s;

}

 bool CPUTracker::isFree(cpu_t elem) const {
  return contains(initCPU, elem) &&
         contains(freeCPU, elem);
}

void CPUTracker::pushFree(pid_t pid, cpu_t x) {
  if (isOccPid(pid, x)) {
    freeCPU.push_back(x);
    std::erase(occCPU[pid], x);
  } else {

    cat_.error("Set free %i an untracked CPU", x);
    exit(-1);
  }
}

 void CPUTracker::pushOcc(pid_t pid, cpu_t x) {
  if (isFree(x)) {
    occCPU[pid].push_back(x);
    std::erase(freeCPU, x);
  } else {
    cat_.error("Set Occ %i an untracked CPU", x);
  }
}

CPUGuard CPUTracker::setCPU(std::shared_ptr<rmcommon::App> app, cpu_t ancpu) {
  cpu_t pid = app->getPid();
  size_t ncpu = ancpu <= 0 ? 1 : ancpu;
  std::cerr << "FREECPU: " << freeCPU.size() << "\n";
  std::cerr << "Required: " << ancpu << "\n";
  // Release Ris
  if (ncpu < occCPU[pid].size()) {
    auto to_remove = occCPU[pid].size() - ncpu;
    std::vector<cpu_t> values;
    for (size_t i = 0; i < to_remove; i++) {
      auto val = occCPU[pid].back();
      std::cerr << "FREEE: " << val << "\n";
      pushFree(pid, val);
    }
    values.insert(values.end(), occCPU[pid].begin(), occCPU[pid].end());
    return CPUGuard{*this, values, app};
  }
  // Acquire ris
  else if (ncpu > occCPU[pid].size()) {
    std::vector<cpu_t> values;
    values.insert(values.end(), occCPU[pid].begin(), occCPU[pid].end());
    ncpu = ncpu - values.size();
    auto size = freeCPU.size();
    for (size_t i = 0; i < std::min((unsigned long)ncpu, size); i++) {
      cpu_t value = freeCPU.front();
      values.push_back(value);
      pushOcc(pid, value);
    }
    return CPUGuard{*this, values, app};
  } else {
    std::vector<cpu_t> values;
    values.insert(values.end(), occCPU[pid].begin(), occCPU[pid].end());
    return CPUGuard{*this, values, app};
  }
}

void CPUTracker::release(pid_t pid) {
  auto value = occCPU.extract(pid);
  auto map = value.mapped();
  freeCPU.insert(freeCPU.end(), map.begin(), map.end());
}

int CPUTracker::getFreeCpus(){
  return freeCPU.size();
};

} // namespace pc
