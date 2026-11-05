#!/usr/bin/env bash
# Runs one experiment: starts Konro with the given configuration, launches one
# or more applications through `runner` and records what happens.
#
# Usage (inside the container, from the repository root):
#   experiments/run_experiment.sh <name> <konro.ini> "<app command>" ["<app command>" ...]
# Example:
#   experiments/run_experiment.sh backprop_talp experiments/konro_talpdrom.ini \
#       "konro_backprop_loc/build/dist/bin/backprop 2000000 80"
#
# Results are written to experiments/results/<name>/:
#   konro.log, konro.out   Konro log and console output
#   sampler.csv            CPUs assigned to / used by every application over time
#   app<i>.log             output of the i-th application
#   times.csv              wall-clock time of every application

set -euo pipefail

if [ $# -lt 3 ]; then
  sed -n '2,/^$/p' "$0"
  exit 1
fi

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
NAME="$1"
CONFIG="$(realpath "$2")"
shift 2
OUT="$ROOT/experiments/results/$NAME"
KONRO="$ROOT/konro_loc/build/rm/konro"
RUNNER="$ROOT/konro_backprop_loc/build/dist/bin/runner"
SAMPLER="$ROOT/tools/build/talp_sampler"

# DLB settings for the applications (see docs/TALP_INTEGRATION.md):
#  - OMP_TOOL_LIBRARIES: Ubuntu's libomp does not load DLB's OMPT tool by itself
#  - --ompt-thread-manager=none: the OpenMP team is resized by drom_omp_binding.h
export OMP_NUM_THREADS=1
export OMP_TOOL_LIBRARIES=/usr/local/lib/libdlb.so
export DLB_ARGS="--drom --ompt --ompt-thread-manager=none --talp=openmp --talp-external-profiler"

rm -rf "$OUT"
mkdir -p "$OUT"
dlb_shm -d > /dev/null 2>&1 || true      # remove stale DLB shared memory

(cd "$OUT" && DLB_ARGS="" OMP_TOOL_LIBRARIES="" exec "$KONRO" -c "$CONFIG" -l INFO > konro.out 2>&1) &
KONRO_PID=$!
sleep 2

"$SAMPLER" 2000 > "$OUT/sampler.csv" &
SAMPLER_PID=$!

echo "app,command,seconds" > "$OUT/times.csv"
APP_PIDS=()
i=0
for cmd in "$@"; do
  i=$((i + 1))
  (
    start=$(date +%s.%N)
    "$RUNNER" $ROOT/$cmd > "$OUT/app$i.log" 2>&1
    end=$(date +%s.%N)
    echo "$i,\"$cmd\",$(awk "BEGIN { printf \"%.2f\", $end - $start }")" >> "$OUT/times.csv"
  ) &
  APP_PIDS+=($!)
  echo "started app$i: $cmd"
  sleep 1
done
wait "${APP_PIDS[@]}"

sleep 2
kill -INT "$SAMPLER_PID" "$KONRO_PID" 2> /dev/null || true
wait "$SAMPLER_PID" "$KONRO_PID" 2> /dev/null || true

echo "Results in $OUT:"
cat "$OUT/times.csv"
