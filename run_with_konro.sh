#!/bin/bash

KONRO_PATH="/root/es_project-main/konro_loc/build/rm/konro"

PROCESS_NAME="konro"

# Check if the procces is already running
if pgrep -f "$PROCESS_NAME" > /dev/null
then
    echo "'$PROCESS_NAME' is already running."
else
    echo "Starting '$PROCESS_NAME' in a new terminal..."
    gnome-terminal --tab -- bash -c "$KONRO_PATH; exec bash"
    # timer
    sleep 5
fi

# Double check for konro
if ! pgrep -f "$PROCESS_NAME" > /dev/null
then
    echo "Error: '$PROCESS_NAME' did not start correctly."
    exit 1
fi

# env variables 
export DLB_ARGS="--ompt --verbose=ompt --ompt-thread-manager=omp5 --drom"


