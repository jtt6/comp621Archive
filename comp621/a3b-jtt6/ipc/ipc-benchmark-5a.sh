#!/bin/bash
# Eric Schumacker, eds6@rice.edu, SUID S01340401
# Utility to perform benchmarking for 

buf_range_idx=24

#Configure the kernel per-socket buffer size
sudo sysctl -w net.core.rmem_max=33554432
sudo sysctl -w net.core.rmem_default=33554432

#2proc mode

#2proc, pipe
out="2proc-pipe.txt"
for (( i=0; i < buf_range_idx; i++ ))
do
    echo $((2 ** i)) > out
    ./ipc-static -i pipe -b $((2 ** i)) 2proc >> $out
done

#2proc, socket
out="2proc-socket-local.txt"
for (( i=0; i < buf_range_idx; i++ ))
do
    echo $((2 ** i)) > out
    ./ipc-static -i local -b $((2 ** i)) 2proc >> $out
done

#2proc, socket -s
out="2proc-socket-local-s.txt"
for (( i=0; i < buf_range_idx; i++ ))
do
    echo $((2 ** i)) > out
    ./ipc-static -i local -b $((2 ** i)) -s 2proc >> $out
done

