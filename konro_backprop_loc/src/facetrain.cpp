#include "backprop.h"
#include "dlb.h"
#include "dlb_talp.h"
#include "drom_omp_binding.h"
#include "feedback.h"
#include "omp.h"
#include <array>
#include <cstddef>
#include <iostream>
#include <math.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char *strcpy();
extern void exit();

int layer_size = 0;

void backprop_face() {
  BPNN *net;
  net = bpnn_create(layer_size, 16, 1); // (16, 1 can not be changed)
  printf("Input layer size : %d\n", layer_size);
  load(net);
  // entering the training kernel, only one iteration
  printf("Starting training kernel\n");
  bpnn_train_kernel(net);
  bpnn_free(net);
  printf("Training done\n");
}

int setup(int argc, char **argv) {
  if (argc != 2 && argc != 3) {
    fprintf(stderr, "usage: backprop <num of input elements> [iterations]\n"
                    "       iterations <= 0 or omitted: run forever\n");
    exit(0);
  }

  layer_size = atoi(argv[1]);
  int iterations = argc == 3 ? atoi(argv[2]) : 0;
  int seed;

#pragma omp for
  for (int i = 0; i < 1; i++) {
    i += 1;
  }

  // apply the CPU masks assigned by Konro (DROM) to the OpenMP team
  konro_drom_omp_init();
  pid_t t = getpid();
  printf("pid: %d\n", t);

  feedback randomFeedback;
  for (int it = 0; iterations <= 0 || it < iterations; ++it) {
    int feed = *randomFeedback;
    feed = feed == -1 ? 50 : (feed == 0 ? 100 : 150);
    int i = DLB_PollDROM_Update();
    printf("update %i\n", i);
    seed = 7;
    bpnn_initialize(seed);
    backprop_face();
    // TALP heartbeat: publish the metrics collected during this iteration to the DLB shared memory, where Konro reads them (--talp-external-profiler)
    DLB_MonitoringRegionsUpdate();
    konro::sendFeedbackMessage(feed);
    printf("feedback %i\n", feed);
  }

  exit(0);
}
