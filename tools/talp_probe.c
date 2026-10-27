/*
 * talp_probe: minimal TALP third-party reader.
 *
 * Attaches to the DLB shared memory as TALP administrator and prints, once per
 * period, the cumulative useful/MPI time of every DLB process in the node,
 * plus the number of CPUs doing useful work in the last period.
 * Used to validate the Konro <-> TALP data path.
 *
 * Build: clang -O2 -o talp_probe talp_probe.c -ldlb
 * Run:   ./talp_probe [period_ms] [iterations]
 */
#include <dlb.h>
#include <dlb_errors.h>
#include <dlb_talp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_PROCS 64

static double now_s(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int main(int argc, char *argv[]) {
    int period_ms = argc > 1 ? atoi(argv[1]) : 1000;
    int iterations = argc > 2 ? atoi(argv[2]) : 20;

    int err = DLB_TALP_Attach();
    printf("DLB_TALP_Attach: %s\n", DLB_Strerror(err));
    if (err != DLB_SUCCESS) return EXIT_FAILURE;

    int ncpus = 0;
    DLB_TALP_GetNumCPUs(&ncpus);
    printf("node cpus: %d\n", ncpus);

    int last_pid[MAX_PROCS] = {0};
    double last_useful[MAX_PROCS] = {0};
    double t_prev = now_s();

    for (int it = 0; it < iterations; ++it) {
        usleep(period_ms * 1000);
        double t = now_s(), dt = t - t_prev;
        t_prev = t;

        int pids[MAX_PROCS], n = 0;
        DLB_TALP_GetPidList(pids, &n, MAX_PROCS);
        for (int i = 0; i < n; ++i) {
            double mpi = 0, useful = 0;
            if (DLB_TALP_GetTimes(pids[i], &mpi, &useful) != DLB_SUCCESS) continue;
            double d = (last_pid[i] == pids[i]) ? useful - last_useful[i] : 0.0;
            printf("pid %d: useful=%.3fs  useful CPUs in last period=%.2f\n",
                   pids[i], useful, d / dt);
            last_pid[i] = pids[i];
            last_useful[i] = useful;
        }
        fflush(stdout);
    }
    DLB_TALP_Detach();
    return EXIT_SUCCESS;
}