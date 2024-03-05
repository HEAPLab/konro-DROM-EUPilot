#ifndef DROMCPUGUARD_H
#define DROMCPUGUARD_H

#include "app.h"
#include <cstddef>
#include <memory>
#include <vector>

namespace pc {

struct CPUTracker;
using cpu_t = int;

struct CPUGuard {
private:
  bool isApplied;
  [[maybe_unused]] CPUTracker &cpuTracker;
  std::vector<cpu_t> values;
  std::shared_ptr<rmcommon::App> app;
  pid_t pid;

public:
  CPUGuard(CPUTracker &cpuTracker, std::vector<cpu_t> values,
           std::shared_ptr<rmcommon::App> app)
      : isApplied(false), cpuTracker(cpuTracker), values(values), app(app),
        pid(app->getPid()) {}

  CPUGuard() = delete;
  CPUGuard(const CPUGuard &) = delete;
  CPUGuard &operator=(const CPUGuard &) = delete;

  /*!
   * Remove first reserved element releasing it
   */
  int leakOcc();

  /*!
   * To be use in conjunction with setCPU
   */
  void apply();

  /*! Return the total ammount of tickets
   */
  size_t size() const;

  ~CPUGuard();
};
} // namespace pc
#endif