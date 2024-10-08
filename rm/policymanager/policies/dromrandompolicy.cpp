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
#include <sys/socket.h>
#include <thread>
#include <netinet/in.h>

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
          platformDescription_.getNumProcessingUnits())) { }

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
    sched_setaffinity(app->getPid(), sizeof(cpu_set_t), &mask);
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


//--- new code ---

void DromRandPolicy::start() {
    try {
      std::thread s_thread(&DromRandPolicy::manageServerSocket, this);
      s_thread.detach();
    } catch (const std::exception& e) {
        log4cpp::Category::getRoot().debug("EXCEPTION! :  %s", e.what());
    }
}

void DromRandPolicy::manageServerSocket() {
  log4cpp::Category::getRoot().debug("\nOpening socket server...");
  int server, sock;
  struct sockaddr_in address;
  int opted = 1;
  int port = 28602;
  int backlog = 5;
  int address_length = sizeof(address);
  if( ( server = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    log4cpp::Category::getRoot().error("Failed to open socket server");
    pthread_exit(NULL);
  }

  if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opted, sizeof(opted))) {
        log4cpp::Category::getRoot().error("Socket setsockopt failed");
        pthread_exit(NULL);
  }

    //SERVER address
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    //Socket bind
    if (bind(server, (struct sockaddr*)&address, sizeof(address)) < 0) {
        log4cpp::Category::getRoot().error("Socket bind failed");
        pthread_exit(NULL);
    }

    //Server starts listening
    if (listen(server, backlog) < 0) {
        log4cpp::Category::getRoot().error("Socket listen failed");
        pthread_exit(NULL);
    }

    log4cpp::Category::getRoot().debug("Server listening on port %d", port);

    //Accept incoming connections
    while (true) {
        if ((sock = accept(server, (struct sockaddr*)&address, (socklen_t*)&address_length)) < 0) {
            log4cpp::Category::getRoot().error("Server connection accept failed");
            pthread_exit(NULL);
        }

        log4cpp::Category::getRoot().debug("Connection accepted");

        std::thread client_thread(&DromRandPolicy::manage_client_req, this, sock);
        client_thread.detach();
    }

    close(server);

}

void DromRandPolicy::manage_client_req(int sock){
    char buffer[1024] = {0};
      
    //Receive data
    int valread = read(sock, buffer, sizeof(buffer) - 1 );
    log4cpp::Category::getRoot().debug("Message received: %s", buffer);
    buffer[valread] = '\0';

    //If message received contains "GetFreeCPUs", send response"
    if(strcmp(buffer, "GetFreeCPUs") == 0){
      char response[1024];
      int free_cpus = cpuSetControl.getFreeCpus();
      snprintf(response, sizeof(response), "%d", free_cpus);

      // Send response to the client
      ssize_t sent_bytes = send(sock, response, strlen(response), 0);
      if (sent_bytes < 0) {
          log4cpp::Category::getRoot().error("Failed to send response to SLURM");
      } else {
          log4cpp::Category::getRoot().debug("Sent response FREECPU: %s", response);
      }
  }
  //Otherwise, just ignore message and close socket
  close(sock);
}

} // namespace rp

