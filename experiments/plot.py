#!/usr/bin/env python3
"""Plots and summarizes the experiments recorded by run_experiment.sh.

Usage (from the repository root):
    python3 experiments/plot.py experiments/results/<run1> [experiments/results/<run2> ...]

For every run it writes <run>/cpus.png (CPUs assigned to / used by each
application over time) and it prints a summary table, also saved as
experiments/results/summary.md:
    runtime        wall-clock time of the application
    avg assigned   average number of CPUs assigned by Konro
    CPU*s          CPU-seconds reserved for the application
    avg useful     average number of CPUs doing useful work (TALP)
    efficiency     useful CPU time / assigned CPU time
"""
import csv
import os
import sys
from collections import OrderedDict

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402


def load_samples(run_dir):
    """Returns {pid: {"t": [...], "assigned": [...], "useful": [...]}} in start order."""
    apps = OrderedDict()
    with open(os.path.join(run_dir, "sampler.csv")) as f:
        for row in csv.DictReader(f):
            app = apps.setdefault(row["pid"], {"t": [], "assigned": [], "useful": []})
            app["t"].append(float(row["time_s"]))
            app["assigned"].append(int(row["assigned_cpus"]))
            app["useful"].append(float(row["useful_cpus"]) if row["useful_cpus"] else None)
    return apps


def load_times(run_dir):
    with open(os.path.join(run_dir, "times.csv")) as f:
        rows = sorted(csv.DictReader(f), key=lambda r: int(r["app"]))
    return [(r["command"], float(r["seconds"])) for r in rows]


def summarize(app):
    t, assigned, useful = app["t"], app["assigned"], app["useful"]
    period = (t[-1] - t[0]) / (len(t) - 1) if len(t) > 1 else 1.0
    cpu_seconds = sum(assigned) * period
    pairs = [(a, u) for a, u in zip(assigned, useful) if u is not None]
    avg_useful = sum(u for _, u in pairs) / len(pairs) if pairs else 0.0
    efficiency = sum(u for _, u in pairs) / sum(a for a, _ in pairs) if pairs else 0.0
    return {
        "avg_assigned": sum(assigned) / len(assigned),
        "cpu_seconds": cpu_seconds,
        "avg_useful": avg_useful,
        "efficiency": efficiency,
    }


def plot_run(run_dir, apps, names):
    fig, ax = plt.subplots(figsize=(9, 4))
    colors = ["#1f77b4", "#d62728", "#2ca02c", "#9467bd"]
    for i, (pid, app) in enumerate(apps.items()):
        color = colors[i % len(colors)]
        label = names[i] if i < len(names) else f"pid {pid}"
        ax.step(app["t"], app["assigned"], where="post", color=color,
                label=f"{label}: assigned CPUs")
        pts = [(t, u) for t, u in zip(app["t"], app["useful"]) if u is not None]
        if pts:
            ax.plot(*zip(*pts), "o", ms=3, color=color, alpha=0.6,
                    label=f"{label}: useful CPUs (TALP)")
    ax.set_xlabel("time [s]")
    ax.set_ylabel("CPUs")
    ax.set_ylim(bottom=0)
    ax.set_title(os.path.basename(os.path.normpath(run_dir)))
    ax.grid(alpha=0.3)
    ax.legend(fontsize=8, loc="upper right")
    fig.tight_layout()
    out = os.path.join(run_dir, "cpus.png")
    fig.savefig(out, dpi=120)
    plt.close(fig)
    return out


def main(run_dirs):
    lines = ["| run | app | runtime [s] | avg assigned CPUs | CPU*s | avg useful CPUs | efficiency |",
             "|---|---|---:|---:|---:|---:|---:|"]
    for run_dir in run_dirs:
        apps = load_samples(run_dir)
        times = load_times(run_dir)
        names = [os.path.basename(cmd.split()[0]) for cmd, _ in times]
        print("plot:", plot_run(run_dir, apps, names))
        for i, app in enumerate(apps.values()):
            s = summarize(app)
            name = names[i] if i < len(names) else "?"
            runtime = times[i][1] if i < len(times) else float("nan")
            lines.append(f"| {os.path.basename(os.path.normpath(run_dir))} | {name} | {runtime:.1f} | "
                         f"{s['avg_assigned']:.2f} | {s['cpu_seconds']:.0f} | "
                         f"{s['avg_useful']:.2f} | {s['efficiency']:.2f} |")
    table = "\n".join(lines)
    print(table)
    summary = os.path.join(os.path.dirname(os.path.normpath(run_dirs[0])), "summary.md")
    with open(summary, "w") as f:
        f.write(table + "\n")
    print("summary:", summary)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(__doc__)
        sys.exit(1)
    main(sys.argv[1:])