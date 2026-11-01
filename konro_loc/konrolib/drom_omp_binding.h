/*
 * drom_omp_binding.h: applies the CPU masks assigned by Konro (via DROM) to an
 * OpenMP application. Header-only, usable from C and C++.
 *
 * Call konro_drom_omp_init() once, after the first OpenMP construct; afterwards every
 * DLB_PollDROM_Update() that finds a new mask:
 *   1. sets the number of OpenMP threads to the number of CPUs in the mask
 *   2. binds each OpenMP worker thread to one CPU of the mask
 *   3. lets the primary thread run on the whole mask (so that the process
 *      affinity seen from outside, e.g. by `taskset -p`, matches the mask)
 *
 * This replaces DLB's OMPT thread manager (--ompt-thread-manager=omp5), which
 * changes the team size inside the OMPT parallel-begin callback: TALP has
 * already sized its per-thread arrays at that point, so a growing team
 * corrupts the heap (DLB 3.8). Here the team is resized outside any parallel
 * region, before the next one starts.
 */
#ifndef KONRO_DROM_OMP_BINDING_H
#define KONRO_DROM_OMP_BINDING_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <dlb.h>
#include <dlb_errors.h>
#include <omp.h>
#include <pthread.h>
#include <sched.h>
#include <stdlib.h>

static void konro_drom_set_process_mask(const_dlb_cpu_set_t mask, void *arg) {
    (void)arg;
    const cpu_set_t *cpus = (const cpu_set_t *)mask;
    int ncpus = CPU_COUNT(cpus);
    if (ncpus <= 0)
        return;

    /* list of the CPUs in the new mask */
    int cpu_list[CPU_SETSIZE];
    int n = 0;
    for (int cpu = 0; cpu < CPU_SETSIZE && n < ncpus; ++cpu)
        if (CPU_ISSET(cpu, cpus))
            cpu_list[n++] = cpu;

    omp_set_num_threads(ncpus);
    /* bind thread i of the (resized) team to the i-th CPU of the mask */
    #pragma omp parallel
    {
        cpu_set_t one;
        CPU_ZERO(&one);
        CPU_SET(cpu_list[omp_get_thread_num() % n], &one);
        pthread_setaffinity_np(pthread_self(), sizeof(one), &one);
    }
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), cpus);
}

static void konro_drom_finalize(void) {
    DLB_Finalize();   /* no-op if DLB was already finalized */
}

static inline int konro_drom_omp_init(void) {
    /* Make sure DLB is initialized in this thread: the OMPT-driven
     * initialization is not always visible from the main thread.
     * DLB_ERR_INIT just means that DLB was already initialized. */
    int err = DLB_Init(0, NULL, NULL);
    if (err == DLB_SUCCESS)
        atexit(konro_drom_finalize);   /* we initialized it: we finalize it */
    else if (err != DLB_ERR_INIT)
        return err;
    return DLB_CallbackSet(dlb_callback_set_process_mask,
                           (dlb_callback_t)konro_drom_set_process_mask, NULL);
}

#endif /* KONRO_DROM_OMP_BINDING_H */