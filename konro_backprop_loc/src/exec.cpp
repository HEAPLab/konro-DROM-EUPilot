#include "dlb.h"
#include "konrolib.h"
#include <dlb_drom.h>
#include <getopt.h>
#include <limits.h>
#include <sched.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/* This signal handler is called after X seconds while trying to correctly
 * finalize after a signal. In case of timeout, forcefully finalize */
static void hard_sighandler(int signum) { exit(EXIT_FAILURE); }

static void wait_child(void) {
  int status;
  wait(&status);
}

static void soft_sighandler(int signum) {
  enum { ALARM_SECONDS = 5 };

  /* Set up alarm signal to forcefully finalize if child or DLB_Finalize
   * cannot finalize in their own */
  struct sigaction sa = {.sa_handler = &hard_sighandler};
  sigaction(SIGALRM, &sa, NULL);
  alarm(ALARM_SECONDS);

  /* (try to) wait for child process */
  wait_child();

  exit(EXIT_FAILURE);
}

static void __attribute__((__noreturn__)) execute(char **argv) {
  /* PreInit from current process */
  cpu_set_t mask, def;
  CPU_ZERO(&mask);
  CPU_ZERO(&def);

  /* Set up signal handler to properly finalize DLB */
  struct sigaction sa = {.sa_handler = &soft_sighandler};
  sigfillset(&sa.sa_mask);
  sigdelset(&sa.sa_mask, SIGALRM);
  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGQUIT, &sa, NULL);
  sigaction(SIGABRT, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);

  /* Fork-exec */
  pid_t pid = fork();
  if (pid < 0) {
    exit(EXIT_FAILURE);
  } else if (pid == 0) {

    sched_getaffinity(pid, sizeof(cpu_set_t), &mask);
    sched_getaffinity(pid, sizeof(cpu_set_t), &def);

    konro::sendAddMessage();

    while (CPU_EQUAL(&mask, &def)) {
      sched_getaffinity(pid, sizeof(cpu_set_t), &mask);
      usleep(100000);
    }
    int error = DLB_PreInit(&mask, NULL);
    printf("err: %i\n", error);
    execvp(argv[0], argv);
    exit(EXIT_FAILURE);
  }
  wait_child();
  exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {

  // Actions
  if (argc > 1) {
    argv += 1;
    execute(argv);
  }

  return EXIT_SUCCESS;
}