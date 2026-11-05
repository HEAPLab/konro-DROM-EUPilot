/*
 * Records, for every DLB process in the node, the number of CPUs assigned to
 * it and the number of CPUs doing useful work (from TALP). It does not depend
 * on the Konro policy, so it measures all of them alike.
 *
 *   assigned_cpus  CPUs in the affinity mask of the process main thread,
 *                  which drom_omp_binding.h keeps equal to the DROM mask
 *   useful_cpus    TALP useful time accumulated since the previous sample of
 *                  the process, divided by the elapsed time; empty when no
 *                  new TALP heartbeat arrived in the period
 *
 * Output (CSV on stdout): time_s,pid,assigned_cpus,useful_cpus
 *
 * Build: clang -O2 -o talp_sampler talp_sampler.c -ldlb
 * Run:   ./talp_sampler [period_ms]   (default 1000; stops on SIGINT/SIGTERM)
 */
#define _GNU_SOURCE
#include <dlb.h>
#include <dlb_errors.h>
#include <dlb_talp.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_PROCS 64

/* last TALP reading of a process */
struct sample {
    int pid;
    double useful;  /* cumulative useful time [s] */
    double when;    /* time of the reading [s] */
};

static volatile sig_atomic_t stop = 0;
static void on_signal(int sig) { (void)sig; stop = 1; }

static double now_s(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int main(int argc, char *argv[]) {
    int period_ms = argc > 1 ? atoi(argv[1]) : 1000;
    if (period_ms <= 0) {
        fprintf(stderr, "usage: %s [period_ms > 0]\n", argv[0]);
        return EXIT_FAILURE;
    }
    signal(SIGINT, on_signal);
    signal(SIGTERM, on_signal);

    if (DLB_TALP_Attach() != DLB_SUCCESS) {
        fprintf(stderr, "talp_sampler: cannot attach to TALP\n");
        return EXIT_FAILURE;
    }

    struct sample last[MAX_PROCS];
    int nlast = 0;
    double t_start = now_s();

    printf("time_s,pid,assigned_cpus,useful_cpus\n");
    while (!stop) {
        usleep(period_ms * 1000);
        double t = now_s();

        int pids[MAX_PROCS], n = 0;
        DLB_TALP_GetPidList(pids, &n, MAX_PROCS);
        for (int i = 0; i < n; ++i) {
            cpu_set_t mask;
            if (sched_getaffinity(pids[i], sizeof(mask), &mask) != 0)
                continue;   /* the process has just exited */
            double mpi = 0, useful = 0;
            int have_times =
                DLB_TALP_GetTimes(pids[i], &mpi, &useful) == DLB_SUCCESS;

            int j = 0;
            while (j < nlast && last[j].pid != pids[i])
                ++j;

            printf("%.2f,%d,%d,", t - t_start, pids[i], CPU_COUNT(&mask));
            if (have_times && j < nlast && useful > last[j].useful)
                printf("%.3f", (useful - last[j].useful) / (t - last[j].when));
            printf("\n");

            /* keep the reading only when it carries new TALP data */
            if (have_times && (j == nlast || useful > last[j].useful)) {
                if (j == nlast && nlast < MAX_PROCS)
                    ++nlast;
                if (j < MAX_PROCS)
                    last[j] = (struct sample){pids[i], useful, t};
            }
        }
        fflush(stdout);
    }
    DLB_TALP_Detach();
    return EXIT_SUCCESS;
}
