#!/bin/bash
# Eric Schumacker, eds6@rice.edu, SUID S01340401
# Utility to perform benchmarking for number 5 and 6

buf_range_idx=24

#Configure the kernel per-socket buffer size
sudo sysctl -w net.core.rmem_max=33554432
sudo sysctl -w net.core.rmem_default=33554432

#2thread, pipe
out="2thread-pipe-readtimes.txt"
for (( i=0; i < buf_range_idx; i++ ))
do
    echo $((2 ** i)) >> $out
    sudo bpftrace -c "./ipc-static -i pipe -b $((2 ** i)) 2thread" -e "kprobe:vfs_read { @start[tid] = nsecs; } kretprobe:vfs_read /@start[tid]/ { @ns[comm] = hist(nsecs - @start[tid]); delete(@start[tid]); }" >> $out
done

#2thread, socket
out="2thread-socket-local-readtimes.txt"
for (( i=0; i < buf_range_idx; i++ ))
do
    echo $((2 ** i)) >> $out
    sudo bpftrace -c "./ipc-static -i local -b $((2 ** i)) 2thread" -e "kprobe:vfs_read { @start[tid] = nsecs; } kretprobe:vfs_read /@start[tid]/ { @ns[comm] = hist(nsecs - @start[tid]); delete(@start[tid]); }" >> $out
done

#2thread, socket -s
out="2thread-socket-local-s-readtimes.txt"
for (( i=0; i < buf_range_idx; i++ ))
do
    echo $((2 ** i)) >> $out
    sudo bpftrace -c "./ipc-static -i local -b $((2 ** i)) -s 2thread" -e 'kprobe:vfs_read { @start[tid] = nsecs; } kretprobe:vfs_read /@start[tid]/ { @ns[comm] = hist(nsecs - @start[tid]); delete(@start[tid]); }' >> $out
done

