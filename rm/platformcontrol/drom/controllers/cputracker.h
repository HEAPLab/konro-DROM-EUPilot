#ifndef DROMCPUTRACKER_H
#define DROMCPUTRACKER_H

#include "app.h"
#include <deque>
#include <log4cpp/Category.hh>
#include <map>
#include <sys/types.h>
#include <vector>

namespace pc {

struct CPUGuard;
using cpu_t = int;

struct CPUTracker {
private:
  std::deque<cpu_t> freeCPU;
  std::map<pid_t, std::vector<cpu_t>> occCPU;
  std::vector<cpu_t> initCPU;
  log4cpp::Category &cat_;

public:
  /*! \return if a CPU is assigned
   * \param elem cpu to check
   */
  constexpr bool isOcc(cpu_t elem) const;

  /*! \return if a CPU is assigned to a specific pid
   * \param pid of the program to check
   * \param elem cpu to check
   */
  bool isOccPid(pid_t pid, cpu_t elem);

  /*! \return if a CPU is free
   * \param elem cpu to check
   */
  constexpr bool isFree(cpu_t elem) const;

  /*! Release the tracking of a cpu for pid
   * \param elem cpu to release
   */
  void pushFree(pid_t pid, cpu_t x);

  /*! Start the tracking of a cpu for pid
   * \param elem cpu to release
   */
  constexpr void pushOcc(pid_t pid, cpu_t x);

  /*! Release all cpu tracked for \p pid */
  void release(pid_t pid);

  /*! Start the tracking of some cpu for pid
   * \param app askink for cpu
   * \param ncpu number of required cpu
   * \return a RAI for cpus that keep track if they are really used in a set
   * operation
   */
  CPUGuard setCPU(std::shared_ptr<rmcommon::App> app, cpu_t ncpu);

  template <typename E>
    requires(std::convertible_to<E, cpu_t>)
  constexpr CPUTracker(E arg)
      : freeCPU(), occCPU(), initCPU(),
        cat_(log4cpp::Category::getRoot()) { // Only Positive  Number of CPUS
    assert(arg >= 0 && "Not Positive CPU?");
    for (E i = 1; i < arg; i++) {
      freeCPU.insert(freeCPU.begin(), i);
      initCPU.insert(initCPU.begin(), i);
    }
  }
};
} // namespace pc
#endif