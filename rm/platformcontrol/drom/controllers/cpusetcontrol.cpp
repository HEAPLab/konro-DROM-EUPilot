#include "cpusetcontrol.h"
#include "app.h"
#include "cpuguard.h"
#include <dlb.h>
#include <dlb_drom.h>
#include <dlb_errors.h>
#include <dlb_types.h>
#include <iostream>
#include <log4cpp/Priority.hh>
#include <memory>
#include <sched.h>
#include <sys/types.h>
#include <utility>
#include <vector>

#define MAX_CPU_SET 255

namespace pc {

void DromCpusetControl::print_drom_list() const {
  int arr[100];
  int out;
  auto res = DLB_DROM_GetPidList(arr, &out, 100);
  if (res == DLB_SUCCESS) {
    cat_.log(log4cpp::Priority::DEBUG, "Start Dump Pid List {");
  } else {
    cat_.log(log4cpp::Priority::DEBUG, "Fail shared memory");
    return;
  }

  for (int i = 0; i < out; i++) {
    cat_.log(log4cpp::Priority::DEBUG, "%d", arr[i]);
  }
  cat_.log(log4cpp::Priority::DEBUG, "}");
}

DromCpusetControl::DromCpusetControl(int ncpus)
    : DromControl(), cpuTracker(ncpus) {

  cpu_set_t proc;
  CPU_ZERO(&proc);
  CPU_SET(0, &proc);

  auto res = DLB_Init(0, &proc, 0);
  if (res == DLB_SUCCESS) {
    cat_.log(log4cpp::Priority::INFO, "Registered as drom administrator");
  } else {
    cat_.log(log4cpp::Priority::INFO, "Fail Registered as drom administrator");
  }
  res = DLB_DROM_Attach();
  if (res == DLB_SUCCESS) {
    cat_.log(log4cpp::Priority::INFO, "Registered as drom administrator");
  } else {
    cat_.log(log4cpp::Priority::INFO, "Fail Registered as drom administrator");
  }
  print_drom_list();
};

DromCpusetControl &DromCpusetControl::DromCpusetControl::instance(int ncpu) {
  static DromCpusetControl cc(ncpu);

  return cc;
}

CPUGuard DromCpusetControl::reserveCpus(std::shared_ptr<rmcommon::App> app,
                                        int ncpu) {
  return cpuTracker.setCPU(app, ncpu);
}

void DromCpusetControl::DromCpusetControl::setCpus(
    const std::vector<std::pair<short, short>> &cpus,
    std::shared_ptr<rmcommon::App> app) {
  cat_.debug("Calling %s with cpus size %i", __FUNCTION__, cpus.size());
  auto pid = app->getPid();
  cpu_set_t cpusetp;
  CPU_ZERO(&cpusetp);
  if (cpus.size() == 0) {
    return;
  }
  for (const auto &cpu_range : cpus) {

    if (cpu_range.first > cpu_range.second) {
      cat_.error("setCPU with invalid parameters  %i > %i", cpu_range.first,
                 cpu_range.second);
      continue;
    }

    {
      // Set Cpu
      auto x = cpu_range.first;
      for (; x <= cpu_range.second; x++) {
        if (!cpuTracker.isOccPid(pid, x)) {
          cat_.error("Try to bind a non acquired CPUS");
          return;
        }
        CPU_SET(x, &cpusetp);
      }
    }
  }

  cat_.log(log4cpp::Priority::DEBUG, "%d", pid);
  auto res = DLB_DROM_SetProcessMask(pid, &cpusetp, DLB_SYNC_QUERY);

  if (res == DLB_SUCCESS) {
    cat_.log(log4cpp::Priority::DEBUG, "Set mask");
  } else if (res == DLB_ERR_NOPROC) {
    cat_.log(log4cpp::Priority::DEBUG, "DLB_ERR_NOPROC Fail Set mask");
  } else if (res == DLB_ERR_PDIRTY) {
    cat_.log(log4cpp::Priority::DEBUG, "DLB_ERR_PDIRTY Fail Set mask");
  } else if (res == DLB_ERR_TIMEOUT) {
    cat_.log(log4cpp::Priority::DEBUG, "DLB_ERR_TIMEOUT Fail Set mask");
  } else if (res == DLB_ERR_PERM) {
    cat_.log(log4cpp::Priority::DEBUG, "DLB_ERR_PERM Fail Set mask");
  }

  cat_.debugStream() << "Dump cpu set {";
  for (const auto &cpu_range : cpus) {
    cat_.debugStream() << "(" << cpu_range.first << " - " << cpu_range.second
                       << ") ";
  }
  cat_.debugStream() << "}";
}

std::vector<std::pair<short, short>>
DromCpusetControl::getCpus(std::shared_ptr<rmcommon::App> app) {
  const auto pid = app->getPid();
  cpu_set_t cpusetp;
  CPU_ZERO(&cpusetp);
  auto res = DLB_DROM_GetProcessMask(pid, &cpusetp, DLB_SYNC_QUERY);

  if (res == DLB_SUCCESS) {
    cat_.log(log4cpp::Priority::DEBUG, "Get mask");
  } else if (res == DLB_NOTED) {
    cat_.log(log4cpp::Priority::DEBUG, "DLB_NOTED Fail Set mask");
  } else if (res == DLB_ERR_NOPROC) {
    cat_.log(log4cpp::Priority::DEBUG, "DLB_ERR_NOPROC Fail Set mask");
  } else if (res == DLB_ERR_TIMEOUT) {
    cat_.log(log4cpp::Priority::DEBUG, "DLB_ERR_TIMEOUT Fail Set mask");
  }

  std::vector<std::pair<short, short>> ret;
  int first = -1;
  int last = -1;
  for (int i = 0; i < MAX_CPU_SET; i++) {
    if (CPU_ISSET(i, &cpusetp)) {
      first = i;
      for (int j = i + 1; j < MAX_CPU_SET; j++) {
        if (!CPU_ISSET(j, &cpusetp)) {
          last = j - 1;
          break;
        }
      }
      if (last == -1)
        last = first;
      ret.emplace_back(first, last);
      i = last;
      first = -1;
      last = -1;
    }
  }

  for (auto elem : ret) {
    for (short i = elem.first; i <= elem.second; i++) {
      if (!cpuTracker.isOccPid(pid, i)) {
        cat_.alert("CPU Retrived mask for pid %i not match what is tracked",
                   pid);
      }
    }
  }
  return ret;
}

void DromCpusetControl::release(std::shared_ptr<rmcommon::App> app) {
  setCpus({}, app);
  cpuTracker.release(app->getPid());
}

std::vector<std::pair<short, short>>
DromCpusetControl::getCpusEffective(std::shared_ptr<rmcommon::App> app) {
  return getCpus(app);
}

void DromCpusetControl::setMems(
    [[maybe_unused]] const rmcommon::CpusetVector &memNodes,
    [[maybe_unused]] std::shared_ptr<rmcommon::App> app) {}

std::vector<std::pair<short, short>> DromCpusetControl::getMems(
    [[maybe_unused]] std::shared_ptr<rmcommon::App> app) {

  return std::vector<std::pair<short, short>>();
}

std::vector<std::pair<short, short>> DromCpusetControl::getMemsEffective(
    [[maybe_unused]] std::shared_ptr<rmcommon::App> app) {
  return std::vector<std::pair<short, short>>();
}

int DromCpusetControl::getFreeCpus(){
  return cpuTracker.getFreeCpus();
}

} // namespace pc
