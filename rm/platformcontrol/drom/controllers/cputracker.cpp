
#include "cputracker.h"
#include "app.h"
#include "cpuguard.h"
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <memory>
#include <ranges>

namespace pc {

bool CPUTracker::isOccPid(pid_t pid, cpu_t elem) {
  return std::ranges::contains(occCPU[pid], elem);
}

constexpr bool CPUTracker::isOcc(cpu_t elem) const {
  return std::ranges::contains(initCPU, elem) &&
         std::ranges::contains(
             occCPU |
                 std::ranges::views::transform(
                     [](std::pair<pid_t, std::vector<cpu_t>> x) {
                       return x.second;
                     }) |
                 std::views::join,
             elem);
}

constexpr bool CPUTracker::isFree(cpu_t elem) const {
  return std::ranges::contains(initCPU, elem) &&
         std::ranges::contains(freeCPU, elem);
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

constexpr void CPUTracker::pushOcc(pid_t pid, cpu_t x) {
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
};

} // namespace pc