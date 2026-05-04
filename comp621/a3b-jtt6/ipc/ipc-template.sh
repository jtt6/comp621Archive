#!/bin/bash
# Script written by Bryan Boone
# Script updated by Saketh Pannala for compatibility

output_dir="docker_results"
output_prefix="docker_results"
ipc_executable="./ipc-static"
total_ipc_size=$((16 * 1024 * 1024))  # 16 MiB, adjust if needed

# Ensure the output directory exists
mkdir -p "$output_dir"

printf "Starting benchmark runs...\n\n"

# Configuration arrays
thread_modes=("1thread" "2thread" "2proc")
ipc_types=("pipe" "tcp" "local")
pmc_modes=("l1d" "l1i" "l2" "l3" "mem") #add l3 if your on rp5

# Function to compute all divisors of total IPC size
get_divisors() {
    local num=$1
    local divisors=()
    for ((i=1024; i<=num; i*=2)); do
        if (( num % i == 0 )); then
            divisors+=($i)
        fi
    done
    echo "${divisors[@]}"
}

valid_buffers=($(get_divisors $total_ipc_size))

# Main loop
for t in "${thread_modes[@]}"; do
    for i in "${ipc_types[@]}"; do
        for p in "${pmc_modes[@]}"; do
            # Generate header row
            header=$($ipc_executable $t -i "$i" -P "$p" -h)
            
            # Clean output filename (no spaces)
            outfile="${output_dir}/${t}_${i}_${p}_${output_prefix}.csv"
            outfile=$(echo "$outfile" | tr -d ' ')

            # Write header
            echo "$header" > "$outfile"

            # Loop over buffer sizes (2^10 to 2^24)
            for bufsz in "${valid_buffers[@]}"; do
                # bufsz=$((2**buffer))
                for avg in {0..5}; do
                    printf "%s %s %s buffer=%d avg=%d\r" "$t" "$i" "$p" "$bufsz" "$avg"
                    # Run benchmark and append result
                    outvalue=$($ipc_executable $t -i "$i" -P "$p" -c -b "$bufsz")
                    echo "$outvalue" >> "$outfile"
                done
            done
        done
    done
done
echo -e "\nAll benchmarks completed. Results in $output_dir/"
