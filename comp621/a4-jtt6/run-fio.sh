#!/bin/bash

# Check if argument is passed
if [ -z "$1" ]; then
  echo "Usage: $0 jobfile.fio"
  exit 1
fi

# Strip .fio to create output name
jobfile="$1"
jobname=$(basename "$jobfile" .fio)

# Create results directory
mkdir -p fio-results

# Run fio
echo "Running $jobfile..."
fio "$jobfile" --output=fio-results/"$jobname".json --output-format=json

echo "Done. Output saved to fio-results/$jobname.json"
