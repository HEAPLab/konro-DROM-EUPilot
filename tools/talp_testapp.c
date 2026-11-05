/*
 * Synthetic OpenMP application with a tunable serial fraction, used together with backprop to evaluate Konro's TALP-driven policy.
 *
 * Every iteration performs a fixed amount of work (work_ms of CPU time):
 *   - serial_fraction * work_ms    on one thread only
 *   - the rest split into chunks and shared among the OpenMP threads so the iteration time follows Amdahl's law:
 *   T(n) = work_ms * (serial_fraction + (1 - serial_fraction) / n)
 *
 * Like backprop, it applies the CPU mask assigned by Konro (DROM) at the start of every iteration and publishes its TALP metrics at the end (heartbeat).
 * Unlike backprop, it sends no feedback to Konro.
 *
 * Build: clang -O2 -fopenmp -o talp_testapp talp_testapp.c -ldlb
 * Usage: talp_testapp <iterations> <work_ms_per_iteration> <serial_fraction>
 */
#include "../konro_loc/konrolib/drom_omp_binding.h"
#include <dlb.h>
#include <dlb_talp.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CHUNKS 64

static double now_s(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

/* busy-waits for the given number of seconds */
static void spin(double seconds) {
    volatile double x = 0;
    double end = now_s() + seconds;
    while (now_s() < end)
        x += 1.0;
}

int main(int argc, char *argv[]) {
    int iterations = argc == 4 ? atoi(argv[1]) : 0;
    double work = argc == 4 ? atof(argv[2]) / 1000.0 : 0.0;
    double serial = argc == 4 ? atof(argv[3]) : -1.0;
    if (iterations <= 0 || work <= 0.0 || serial < 0.0 || serial > 1.0) {
        fprintf(stderr, "usage: %s <iterations> <work_ms_per_iteration> "
                        "<serial_fraction in [0,1]>\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* start the OpenMP runtime (and DLB, through OMPT), then let the CPU masks assigned by Konro resize and bind the OpenMP team */
    #pragma omp parallel
    { }
    int cb = konro_drom_omp_init();
    printf("konro_drom_omp_init: %s\n", DLB_Strerror(cb));

    double start = now_s();
    for (int it = 0; it < iterations; ++it) {
        /* apply the CPU mask assigned by Konro (DROM), if it changed */
        int update = DLB_PollDROM_Update();
        double t0 = now_s();

        spin(serial * work);                            /* serial part */
        #pragma omp parallel for schedule(dynamic, 1)
        for (int c = 0; c < CHUNKS; ++c)
            spin((1.0 - serial) * work / CHUNKS);       /* parallel part */

        /* TALP heartbeat */
        DLB_MonitoringRegionsUpdate();
        printf("iter %d: %.3fs threads=%d update=%d\n", it, now_s() - t0,
               omp_get_max_threads(), update);
        fflush(stdout);
    }
    printf("total time: %.3fs\n", now_s() - start);
    return EXIT_SUCCESS;
}
