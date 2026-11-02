#!/usr/bin/env bash
# Runs the whole evaluation: 3 scenarios x 2 policies, repeated N times.
# Usage (inside the container, from the repository root):
#   experiments/run_all.sh [repetitions]        (default: 3)
# Then: python3 experiments/aggregate.py
set -euo pipefail
cd "$(dirname "$0")/.."

REPS="${1:-3}"
BACKPROP="konro_backprop_loc/build/dist/bin/backprop 2000000 80"
SCAL="tools/build/talp_testapp 60 800 0.1"

for r in $(seq 1 "$REPS"); do
  for policy in rand talp; do
    if [ "$policy" = rand ]; then cfg=experiments/konro_dromrand.ini; else cfg=experiments/konro_talpdrom.ini; fi
    experiments/run_experiment.sh "backprop_${policy}_r$r" "$cfg" "$BACKPROP"
    experiments/run_experiment.sh "scal_${policy}_r$r"     "$cfg" "$SCAL"
    experiments/run_experiment.sh "mix_${policy}_r$r"      "$cfg" "$BACKPROP" "$SCAL"
  done
done
echo "All runs done: python3 experiments/aggregate.py"