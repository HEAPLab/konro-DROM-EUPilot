#include "backprop.h"
#include "dlb.h"
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
  if (argc != 2) {
    fprintf(stderr, "usage: backprop <num of input elements>\n");
    exit(0);
  }

  layer_size = atoi(argv[1]);
  int seed;

#pragma omp for
  for (int i = 0; i < 1; i++) {
    i += 1;
  }

  pid_t t = getpid();
  printf("pid: %d\n", t);

  for (auto feed : feedback{}) {
    feed = feed == -1 ? 50 : (feed == 0 ? 100 : 150);
    int i = DLB_PollDROM_Update();
    printf("update %i\n", i);
    seed = 7;
    bpnn_initialize(seed);
    backprop_face();
    konro::sendFeedbackMessage(feed);
    printf("feedback %i\n", feed);
  }

  exit(0);
}
