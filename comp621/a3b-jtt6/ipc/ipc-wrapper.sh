#!/bin/bash
# Saketh Pannala, sp220@rice.edu
# Wrapper helper Script to help run Benchmark Script more easily

# Defaults
START=1024
END=65536
FACTOR=2
MODE="geo"   # "geo" = geometric, "arith" = arithmetic
PMC="none"
OUTPUT="results.csv"
THREAD_MODE="1thread"
IPC_TYPE="pipe"
BUF=$START

# Usage message
usage() {
    echo "Usage: $0 [--start=NUM] [--end=NUM] [--mode=geo|arith] [--factor=NUM]"
    echo "           [--pmc=TYPE] [--output=FILE] [--thread=MODE] [--ipc=TYPE]"
    echo "Example: $0 --start=1024 --end=65536 --mode=geo --factor=2 --pmc=l1d --output=results.csv"
    exit 1
}

# Parse CLI arguments
for ARG in "$@"; do
    case $ARG in
        --start=*) START="${ARG#*=}" ;;
        --end=*) END="${ARG#*=}" ;;
        --mode=*) MODE="${ARG#*=}" ;;
        --factor=*) FACTOR="${ARG#*=}" ;;
        --pmc=*) PMC="${ARG#*=}" ;;
        --output=*) OUTPUT="${ARG#*=}" ;;
        --thread=*) THREAD_MODE="${ARG#*=}" ;;
        --ipc=*) IPC_TYPE="${ARG#*=}" ;;
        -h|--help) usage ;;
        *) echo "Unknown argument: $ARG"; usage ;;
    esac
done

# Print CSV header
sudo ./ipc-static -P $PMC -h -i $IPC_TYPE $THREAD_MODE >> "$OUTPUT"

# Run loop and append values
while [ "$BUF" -le "$END" ]; do
    echo "Running: buf=$BUF, thread=$THREAD_MODE, ipc=$IPC_TYPE, pmc=$PMC"
    sudo ./ipc-static -P $PMC -c -b $BUF -i $IPC_TYPE $THREAD_MODE >> "$OUTPUT"

    if [ "$MODE" == "geo" ]; then
        BUF=$(( BUF * FACTOR ))
    elif [ "$MODE" == "arith" ]; then
        BUF=$(( BUF + FACTOR ))
    else
        echo "Unknown mode $MODE"
        exit 1
    fi
done

echo "Done. Results written to $OUTPUT"
