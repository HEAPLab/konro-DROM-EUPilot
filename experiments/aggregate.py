#!/usr/bin/env python3
"""Aggregates the repeated runs produced by run_all.sh.

Usage (from the repository root):
    python3 experiments/aggregate.py [results_dir]     (default: experiments/results)

Reads every <scenario>_<policy>_r<N> directory and writes, in results_dir:
    aggregate.md    mean +- standard deviation over the repetitions
    comparison.png  makespan and reserved CPU-seconds, random vs TALP policy
"""
import os
import re
import statistics
import sys
from collections import defaultdict

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt  #noqa: E402

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from plot import load_samples, load_times, summarize  #noqa: E402

RUN_RE = re.compile(r"^(?P<scenario>\w+?)_(?P<policy>rand|talp)_r(?P<rep>\d+)$")
POLICY_NAME = {"rand": "random feedback", "talp": "TALP"}


def fmt(values, digits=1):
    if len(values) < 2:
        return f"{values[0]:.{digits}f}"
    return f"{statistics.mean(values):.{digits}f} ± {statistics.stdev(values):.{digits}f}"


def main(results):
    data = defaultdict(lambda: defaultdict(lambda: defaultdict(list)))
    totals = defaultdict(lambda: defaultdict(list))   # makespan / total CPU*s per run
    for name in sorted(os.listdir(results)):
        m = RUN_RE.match(name)
        run_dir = os.path.join(results, name)
        if not m or not os.path.isdir(run_dir):
            continue
        key = (m["scenario"], m["policy"])
        apps = list(load_samples(run_dir).values())
        times = load_times(run_dir)
        cpu_total = 0.0
        for (cmd, runtime), app in zip(times, apps):
            s = summarize(app)
            appname = os.path.basename(cmd.split()[0])
            data[key][appname]["runtime"].append(runtime)
            data[key][appname]["cpu_s"].append(s["cpu_seconds"])
            data[key][appname]["eff"].append(s["efficiency"])
            cpu_total += s["cpu_seconds"]
        totals[key]["makespan"].append(max(t for _, t in times))
        totals[key]["cpu_s"].append(cpu_total)

    lines = ["| scenario | policy | app | runs | runtime [s] | CPU*s | efficiency |",
             "|---|---|---|---:|---:|---:|---:|"]
    for (scenario, policy) in sorted(data):
        for app, v in data[(scenario, policy)].items():
            lines.append(f"| {scenario} | {POLICY_NAME[policy]} | {app} | {len(v['runtime'])} | "
                         f"{fmt(v['runtime'])} | {fmt(v['cpu_s'], 0)} | {fmt(v['eff'], 2)} |")
    lines += ["", "| scenario | policy | makespan [s] | total CPU*s |", "|---|---|---:|---:|"]
    for (scenario, policy) in sorted(totals):
        t = totals[(scenario, policy)]
        lines.append(f"| {scenario} | {POLICY_NAME[policy]} | {fmt(t['makespan'])} | {fmt(t['cpu_s'], 0)} |")
    table = "\n".join(lines)
    print(table)
    with open(os.path.join(results, "aggregate.md"), "w") as f:
        f.write(table + "\n")

    # comparison chart: makespan and total CPU*s per scenario
    scenarios = sorted({s for s, _ in totals})
    fig, axes = plt.subplots(1, 2, figsize=(10, 4))
    for ax, metric, label in [(axes[0], "makespan", "makespan [s]"),
                              (axes[1], "cpu_s", "reserved CPU-seconds")]:
        width = 0.38
        for i, (policy, color) in enumerate([("rand", "#9e9e9e"), ("talp", "#1f77b4")]):
            means, errs = [], []
            for s in scenarios:
                vals = totals.get((s, policy), {}).get(metric, [0])
                means.append(statistics.mean(vals))
                errs.append(statistics.stdev(vals) if len(vals) > 1 else 0)
            xs = [x + (i - 0.5) * width for x in range(len(scenarios))]
            ax.bar(xs, means, width, yerr=errs, capsize=3, color=color, label=POLICY_NAME[policy])
        ax.set_xticks(range(len(scenarios)))
        ax.set_xticklabels(scenarios)
        ax.set_ylabel(label)
        ax.grid(axis="y", alpha=0.3)
    axes[0].legend()
    fig.suptitle("Konro: random-feedback vs TALP-driven DROM policy "
                 "(mean ± 1 std over repeated runs)")
    fig.tight_layout()
    out = os.path.join(results, "comparison.png")
    fig.savefig(out, dpi=120)
    print("chart:", out)


if __name__ == "__main__":
    main(sys.argv[1] if len(sys.argv) > 1 else "experiments/results")
