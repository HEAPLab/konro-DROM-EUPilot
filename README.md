# Konro Resource Manager

This repository contains the **Konro** resource manager, its integration with the **SLURM** workload manager, and a functional test application.

---

## Project Structure

The project is divided into four main components. Each directory contains its own specific build instructions and dependencies:

### 1. [Konro Core](konro_loc/README.md)
The central resource manager. It handles the core logic for resource allocation.
* **Location:** `konro_loc/`

### 2. [SLURM Integration](slurm/INSTALL)
Modified version of slurm capable of comunicating with Konro
* **Location:** `slurm/`

### 3. [Konro Backprop (Test Application)](konro_backprop_loc/README.md)
A sample application used to validate the integration.
* **Location:** `konro_backprop_loc/`

### 4. [TALP integration](docs/TALP_INTEGRATION.md)
Konro reads the applications' performance metrics from TALP (DLB) and uses them to assign CPUs through DROM.
Includes a Docker environment (`docker/`), tools (`tools/`) and the evaluation (`experiments/`).
* **Location:** `docs/TALP_INTEGRATION.md`

---

## Build Instructions

To ensure all dependencies are correctly linked, please build the projects in the following order:

### Step 1: Build Konro Core
Navigate to the core directory and follow the build steps.
> [Go to Konro Core README](konro_loc/README.md)

### Step 2: Build SLURM Integration
Navigate to the core directory and follow the build steps.
> [Go to SLURM README](slurm/INSTALL)

### Step 3: Build Backprop Test Case
Navigate to the core directory and follow the build steps.
> [Go to Konro Backprop README](konro_backprop_loc/README.md)

### Step 4: TALP Integration (optional)
Builds Konro, the demonstrator and the evaluation tools with TALP support, using the provided Docker environment or a native Ubuntu 24.04 machine.
> [Go to TALP integration](docs/TALP_INTEGRATION.md#build-and-run)
