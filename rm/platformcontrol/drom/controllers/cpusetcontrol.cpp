#include "cpusetcontrol.h"
#include "../pcexception.h"
#include <cctype>
#include <cstdlib>
#include <dlb.h>
#include <dlb_drom.h>

#include <dlb_errors.h>
#include <dlb_types.h>

#include <log4cpp/Priority.hh>
#include <sched.h>
#include <sstream>
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

DromCpusetControl::DromCpusetControl() : DromControl() {

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

DromCpusetControl &DromCpusetControl::DromCpusetControl::instance() {
  static DromCpusetControl cc;
  return cc;
}

void DromCpusetControl::DromCpusetControl::setCpus(
    const std::vector<std::pair<short, short>> &cpus,
    std::shared_ptr<rmcommon::App> app) {
  auto pid = app->getPid();
  cpu_set_t cpusetp;
  CPU_ZERO(&cpusetp);

  if (cpus.size() == 0) {
    return;
  }

  for (const auto &cpu_range : cpus) {
    for (auto x = cpu_range.first; x <= cpu_range.second; x++) {
      CPU_SET(x, &cpusetp);
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
      i=last;
      first = -1;
      last = -1;
    }
  }

  if (res == DLB_SUCCESS) {
    cat_.log(log4cpp::Priority::INFO, "Get mask");
  } else {
    cat_.log(log4cpp::Priority::INFO, "Fail Get mask");
  }

  return ret;
}

std::vector<std::pair<short, short>>
DromCpusetControl::getCpusEffective(std::shared_ptr<rmcommon::App> app) {
  const auto pid = app->getPid();
  cpu_set_t cpusetp;
  CPU_ZERO(&cpusetp);
  DLB_DROM_GetProcessMask(pid, &cpusetp, DLB_SYNC_QUERY);
  std::vector<std::pair<short, short>> ret;
  int first = -1;
  int last = -1;
  for (int i = 0; i < MAX_CPU_SET; i++) {
    if (CPU_ISSET(i, &cpusetp)) {
      first = i;
      for (int j = i + 1; j < MAX_CPU_SET; j++) {
        if (!CPU_ISSET(j, &cpusetp)) {
          last = j;
          break;
        }
      }
      if (last != -1) {
        ret.emplace_back(first, last);
      } else {
        ret.emplace_back(first, first);
      }
      first = -1;
      last = -1;
    }
  }

  return ret;
}

void DromCpusetControl::setMems(const rmcommon::CpusetVector &memNodes,
                                std::shared_ptr<rmcommon::App> app) {}

std::vector<std::pair<short, short>>
DromCpusetControl::getMems(std::shared_ptr<rmcommon::App> app) {

  return std::vector<std::pair<short, short>>();
}

std::vector<std::pair<short, short>>
DromCpusetControl::getMemsEffective(std::shared_ptr<rmcommon::App> app) {
  return std::vector<std::pair<short, short>>();
}

} // namespace pc
