#!/bin/bash
# Assignment 5: IPC Benchmark RTT Sweep
# Mode: 2thread, TCP, 1MiB buffer
# Collects throughput for fixed and auto socket-buffer sizes

ipc="./ipc-static"
bufsz=1048576            # 1 MiB benchmark buffer
outdir="a5"
combined_csv="$outdir/results.csv"

# Create output directory
mkdir -p "$outdir"

# RTTs to test (ms)
rtts=(0 10 20 30 40 50 60 70 80)

# CSV header
echo "RTT_ms,throughput_MBps,mode" > "$combined_csv"

# Increase kernel socket buffer max
sudo sysctl -w net.core.rmem_max=33554432
sudo sysctl -w net.core.rmem_default=33554432

# Set loopback MTU for realistic packet sizes
sudo ifconfig lo mtu 1500

for rtt in "${rtts[@]}"; do
    echo "Running benchmarks for RTT = ${rtt} ms..."

    # One-way delay (half of RTT)
    half_delay=$(( rtt / 2 ))

    # Apply delay using 'replace' so it works even if qdisc exists
    sudo tc qdisc replace dev lo root netem delay ${half_delay}ms

    #
    # FIXED socket-buffer (just use -b; omit -s)
    #
    result_fixed=$($ipc 2thread -i tcp -b $bufsz -c -s)
    thr_fixed=$(echo "$result_fixed" | awk -F, '{print $2}') # throughput is 2nd column
    echo "${rtt},${thr_fixed},Set" >> "$combined_csv"

    #
    # AUTO socket-buffer
    #
    result_auto=$($ipc 2thread -i tcp -b $bufsz -c)
    thr_auto=$(echo "$result_auto" | awk -F, '{print $2}')
    echo "${rtt},${thr_auto},Auto-Resized" >> "$combined_csv"

done

# Clean up tc rules
sudo tc qdisc del dev lo root 2>/dev/null

echo "All benchmarks completed. Combined results written to $combined_csv"
