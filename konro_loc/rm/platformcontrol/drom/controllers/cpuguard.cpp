
#include "cpuguard.h"
#include "cpusetcontrol.h"
#include "cputracker.h"
#include <algorithm>
#include <cstddef>

namespace pc {
int CPUGuard::leakOcc() {
  isApplied = true;
  int x = values.back();
  values.pop_back();
  return x;
}

void CPUGuard::apply() {
  if (isApplied)
    return;

  auto arg2 = app;
  std::sort(values.begin(), values.end());
  auto it = values.begin();
  auto end = values.end();
  std::vector<std::pair<short, short>> arg1;
  while (it != end) {
    short start = *it;
    short step = 0;
    while (it != end && start + step == *it) {
      it++;
      step++;
    }
    arg1.push_back({start, start + (step - 1)});
  }
  values.clear();

  DromCpusetControl::instance(0).setCpus(arg1, arg2);
  isApplied = true;
}

size_t CPUGuard::size() const { return values.size(); }

CPUGuard::~CPUGuard() {
  if (!isApplied)
    apply();
}
} // namespace pc