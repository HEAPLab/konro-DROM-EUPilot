# Konro + TALP integration

This document describes the integration of **TALP** (Tracking Application Live
Performance, part of [DLB](https://pm.bsc.es/dlb) by BSC) into the Konro-DROM
resource manager. TALP acts as a *heartbeat* library: applications publish their
performance metrics and Konro uses them to decide, through DROM, how many CPUs
each application gets.

## 1. Motivation

Before this work, the DROM policy (`DromRandPolicy`) changed the CPUs of an
application according to a feedback value sent by the application itself over
HTTP (`< 70` more CPUs, `> 130` fewer CPUs). In the demonstrator (`backprop`) this
value was **random**, so Konro had no information about how well an application
was actually using its CPUs.

With TALP, Konro measures it directly: TALP tracks the time every thread of the
application spends doing useful work, and Konro reads it from the DLB shared
memory without any extra work from the application.

## 2. Architecture

```
application (OpenMP)                         Konro
─────────────────────                        ─────────────────────────────────────
DLB_MonitoringRegionsUpdate()  ──► DLB shmem ──► TalpMonitor::usefulCpus(pid)
  (heartbeat, every iteration)      (TALP)          │  DLB_TALP_GetTimes()
                                                    ▼
DLB_PollDROM_Update()          ◄── DLB shmem ◄── TalpDromPolicy::timer()
  + drom_omp_binding.h              (DROM)          efficiency = useful CPUs / assigned CPUs
  (resize + bind OpenMP team)                       DLB_DROM_SetProcessMask()
```

| Component | File | Role |
|---|---|---|
| `TalpMonitor` | `konro_loc/rm/platformcontrol/drom/talpmonitor.{h,cpp}` | Attaches to TALP as third-party reader (`DLB_TALP_Attach`) and returns, for each process, the average number of CPUs doing useful work since the previous read. Returns *no data* when no new heartbeat arrived. |
| `TalpDromPolicy` | `konro_loc/rm/policymanager/policies/talpdrompolicy.{h,cpp}` | DROM policy driven by the TALP parallel efficiency. Extends `DromRandPolicy` (reuses app add/remove, initial CPU, SLURM socket). |
| `drom_omp_binding.h` | `konro_loc/konrolib/drom_omp_binding.h` | Header-only helper for applications: applies the DROM masks to the OpenMP team (number of threads + thread binding). See §6. |
| Heartbeat in the demonstrator | `konro_backprop_loc/src/facetrain.cpp` | `DLB_MonitoringRegionsUpdate()` at the end of every training iteration. |

### Decision rule (`TalpDromPolicy::timer`, every `timerseconds`)

`efficiency = useful CPUs (TALP) / CPUs assigned by Konro`

| Condition | Action |
|---|---|
| `efficiency < lowefficiency` and CPUs > 1 | remove 1 CPU, then do not grow for `backoffperiods` periods |
| `efficiency >= highefficiency`, no backoff, free CPUs available | add 1 CPU |
| otherwise | keep |

The backoff avoids oscillations for applications that cannot use a second CPU
(1 → 2 → 1 → 2 ...). With the default thresholds an application with serial
fraction *s* stops growing where its Amdahl efficiency drops below 0.8
(e.g. 3 CPUs for *s* = 10%).

Configuration (`[talp]` section of the Konro ini file, all optional):

```ini
[policy]
policy = TalpDromPolicy

[talp]
lowefficiency = 0.6
highefficiency = 0.8
backoffperiods = 5
```

## 3. Integrating an application

1. Run it with TALP and DROM enabled:
   ```bash
   export OMP_TOOL_LIBRARIES=/usr/local/lib/libdlb.so   # needed with Ubuntu's libomp, see §6
   export DLB_ARGS="--drom --ompt --ompt-thread-manager=none --talp=openmp --talp-external-profiler"
   ```
2. After the first OpenMP construct, call `konro_drom_omp_init()` (from `drom_omp_binding.h`).
3. In the main loop: `DLB_PollDROM_Update()` at the beginning of each iteration
   and `DLB_MonitoringRegionsUpdate()` at the end (heartbeat).
4. Launch it through `runner` (as before) so that Konro manages it.

No feedback message is needed with `TalpDromPolicy`.

## 4. Build and run

A Docker environment (Ubuntu 24.04, clang, LLVM libomp, DLB 3.8.0) is provided;
it works on x86_64 and arm64 (tested on Apple Silicon).

```bash
docker build -t konro-talp -f docker/Dockerfile .
docker run --rm -it --name konro-talp --shm-size=256m --privileged -v "$PWD":/workspace konro-talp
# inside the container
cmake -S konro_loc -B konro_loc/build -DCMAKE_BUILD_TYPE=Release && cmake --build konro_loc/build -j
cmake -S konro_backprop_loc -B konro_backprop_loc/build -DCMAKE_BUILD_TYPE=Release \
      -DPATH_TO_KONROLIB=$PWD/konro_loc/build/konrolib/libkonrolib.a \
      -DCMAKE_INSTALL_PREFIX=$PWD/konro_backprop_loc/build/dist
cmake --build konro_backprop_loc/build --target install -j
mkdir -p tools/build
clang -O2 -fopenmp -o tools/build/talp_testapp tools/talp_testapp.c -ldlb
clang -O2 -o tools/build/talp_sampler tools/talp_sampler.c -ldlb
clang -O2 -o tools/build/talp_probe tools/talp_probe.c -ldlb
```

`--privileged` is needed because Konro moves each application into its own
cgroup under `/sys/fs/cgroup`.

Single experiment / full evaluation:

```bash
experiments/run_experiment.sh <name> experiments/konro_talpdrom.ini "konro_backprop_loc/build/dist/bin/backprop 2000000 80"
experiments/run_all.sh 5 && python3 experiments/aggregate.py
```

## 5. Evaluation

Setup: Apple M4 Pro, Docker Desktop VM with 8 CPUs, Konro timer 2 s,
5 repetitions per configuration (mean ± standard deviation).
CPU-seconds and efficiency are measured by `tools/talp_sampler`, independently of
the policy (assigned CPUs from the kernel affinity, useful CPUs from TALP).

Workloads:
- **backprop**: the existing demonstrator, 80 iterations (limited parallelism);
- **scal**: `talp_testapp`, synthetic OpenMP app with 10% serial work (scales well);
- **mix**: both at the same time.

Baseline: `DromRandPolicy`, driven by backprop's random feedback.
`talp_testapp` sends no feedback, so under the baseline it keeps 1 CPU (static allocation).

![comparison](img/comparison.png)

| Scenario | Policy | Makespan [s] | Reserved CPU·s | Efficiency |
|---|---|---:|---:|---:|
| backprop | random feedback | 25.6 ± 0.7 | 88 ± 30 | 0.41 ± 0.08 |
| backprop | TALP | 27.0 ± 0.5 | **35 ± 2** | **0.80 ± 0.03** |
| scal | static (1 CPU) | 48.2 ± 0.0 | 48 ± 0 | 0.99 |
| scal | TALP | **25.0 ± 0.2** | 55 ± 1 | 0.83 |
| mix | random feedback | 48.2 ± 0.0 | 149 ± 13 | backprop 0.39, testapp 1.01 |
| mix | TALP | **29.4 ± 0.5** | **97 ± 2** | backprop 0.82, testapp 0.78 |

- **backprop**: TALP reserves 60% fewer CPU-seconds for a 5% longer runtime; efficiency doubles.
- **scal**: TALP grows the application to 3 CPUs, where the Amdahl efficiency reaches 0.8: 1.9x faster.
- **mix**: CPUs move from backprop (which wastes them) to the scalable app:
  makespan −39% and 35% fewer CPU-seconds.

## 6. Issues found and workarounds

1. **Heap corruption with DLB 3.8 (TALP + DROM + OMPT thread manager).**
   In the OMPT *parallel-begin* callback TALP sizes its per-thread array with the
   announced team size; right after, the `omp5` thread manager polls DROM and calls
   `omp_set_num_threads()` with a larger value. libomp resizes the team after the
   callback, so the extra threads write past the TALP array (`realloc(): invalid next size`).
   Workaround: `--ompt-thread-manager=none` plus `drom_omp_binding.h`, which resizes
   and binds the team in `DLB_PollDROM_Update()`, outside any parallel region.
2. **Without the thread manager DROM masks are not applied**: nobody registers the
   `dlb_callback_set_process_mask` callback, so `DLB_PollDROM_Update()` returns
   success but nothing changes. Solved by `drom_omp_binding.h`.
3. **OMPT-driven `DLB_Init` not visible from the main thread** in some runs
   (`DLB_PollDROM_Update()` returns `DLB_ERR_NOINIT`): `konro_drom_omp_init()`
   calls `DLB_Init()` explicitly (`DLB_ERR_INIT` is ignored) and finalizes DLB at exit.
4. **Ubuntu's `libomp.so.5` does not find DLB's OMPT tool** in the address space
   (`ompt_start_tool` is resolved inside libomp itself): set
   `OMP_TOOL_LIBRARIES=/usr/local/lib/libdlb.so`.
5. **Stale DLB shared memory** after an application is killed: run `dlb_shm -d`
   before restarting (done automatically by `run_experiment.sh`).

Other fixes to the existing code included in this work:
- build fixes for recent toolchains (missing `<cassert>`; Linux ≥ 6.6 proc connector enum);
- `ProcListener` falls back to polling `/proc` when the kernel has no Proc Connector
  (e.g. Docker Desktop), instead of stopping Konro;
- note: in `PolicyManager::makePolicy` the `NoPolicy` case falls through to
  `DromRandPolicy`.

## 7. Limitations and future work

- Konro computes useful CPUs between its own reads: with iterations comparable to
  the timer period, the number of heartbeats per window varies (quantization noise,
  e.g. 0.8/1.2). A longer `timerseconds` or a TALP timestamp of the last update
  would reduce it.
- One CPU is added/removed per period; a proportional step (towards
  `useful CPUs / target efficiency`) would converge faster.
- Only OpenMP is evaluated; MPI applications can be handled by the same
  `TalpMonitor` (`mpi_time` is already read).
- Port to RISC-V platforms (e.g. BananaPI), as planned for a two-student project.
