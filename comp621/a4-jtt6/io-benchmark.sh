#!/bin/bash

# Configuration
buffers=(1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576)        # Buffer sizes in bytes
filesystems=("/fs1" "/fs2" "/pool")
totalsize=16777216               # 16 MiB total I/O
runs=6                           # Number of runs per buffer size (discard first run)
output_file="results.csv"

# Ensure CSV header
echo "filesystem,buffer_size,run,throughput_MBps" > $output_file

# Loop over filesystems
for fs in "${filesystems[@]}"; do
    datafile="$fs/mydatafile"

    # Create a 16 MiB sample file if it does not exist
    if [ ! -f "$datafile" ]; then
        echo "Creating $datafile..."
        sudo ./io-static -c -t $totalsize "$datafile" -q
    fi

    # Loop over buffer sizes
    for buf in "${buffers[@]}"; do
        echo "Benchmarking $fs with buffer size $buf..."

        # Perform multiple runs, discarding the first
        for run in $(seq 1 $runs); do
            result=$(sudo ./io-static -r -b $buf -t $totalsize -o "$datafile")
            
            # Skip the first run if run=1
            if [ "$run" -eq 1 ]; then
                echo "Discarding warm-up run"
                continue
            fi

            # Extract throughput (last column in CSV)
            throughput=$(echo $result | awk -F',' '{print $NF}')
            
            # Append to CSV
            echo "$fs,$buf,$run,$throughput" >> $output_file
        done
    done
done

echo "Benchmarking complete. Results in $output_file"
