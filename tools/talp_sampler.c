/*
 * talp_sampler: records, for every DLB process in the node, the number of
 * CPUs assigned to it and the number of CPUs doing useful work (from TALP).
 * It is independent of the Konro policy, so it can be used to compare them.
 *
 * Output (CSV on stdout): time_s,pid,assigned_cpus,useful_cpus
 *   useful_cpus is empty when no new TALP heartbeat arrived in the period.
 *
 * Build: clang -O2 -o talp_sampler talp_sampler.c -ldlb
 * Run:   ./talp_sampler [period_ms]       (stops on SIGINT/SIGTERM)
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

static volatile sig_atomic_t stop = 0;
static void on_signal(int sig) { (void)sig; stop = 1; }

static double now_s(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int main(int argc, char *argv[]) {
    int period_ms = argc > 1 ? atoi(argv[1]) : 1000;
    signal(SIGINT, on_signal);
    signal(SIGTERM, on_signal);

    if (DLB_TALP_Attach() != DLB_SUCCESS) {
        fprintf(stderr, "talp_sampler: cannot attach to TALP\n");
        return EXIT_FAILURE;
    }

    struct { int pid; double useful; double when; } last[MAX_PROCS];
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
            if (sched_getaffinity(pids[i], sizeof(mask), &mask) != 0) continue;
            double mpi = 0, useful = 0;
            int have_times = DLB_TALP_GetTimes(pids[i], &mpi, &useful) == DLB_SUCCESS;

            /* look for the previous sample of this pid */
            int j = 0;
            while (j < nlast && last[j].pid != pids[i]) ++j;

            printf("%.2f,%d,%d,", t - t_start, pids[i], CPU_COUNT(&mask));
            if (have_times && j < nlast && useful > last[j].useful) {
                printf("%.3f", (useful - last[j].useful) / (t - last[j].when));
            }
            printf("\n");

            if (have_times && (j == nlast || useful > last[j].useful)) {
                if (j == nlast && nlast < MAX_PROCS) ++nlast;
                if (j < MAX_PROCS) last[j] = (typeof(last[0])){pids[i], useful, t};
            }
        }
        fflush(stdout);
    }
    DLB_TALP_Detach();
    return EXIT_SUCCESS;
}