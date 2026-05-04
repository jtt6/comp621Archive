#!/bin/bash
# Eric Schumacker, eds6@rice.edu, SUID S01340401
# Utility to perform benchmarking for 

buf_range_idx=24

#Configure the kernel per-socket buffer size
sudo sysctl -w net.core.rmem_max=33554432
sudo sysctl -w net.core.rmem_default=33554432

#2proc mode

#2proc, pipe
#out="2proc-pipe-mem.txt"
#rm $out
#for (( i=0; i < buf_range_idx; i++ ))
#do
#    echo $((2 ** i)) >> $out
#    #sudo ./ipc-static -vP mem -i pipe -b $((2 ** i)) 2proc >> $out
#    sudo ./ipc-static -vP l1d -i pipe -b $((2 ** i)) 2proc >> $out
#    sudo ./ipc-static -vP l1i -i pipe -b $((2 ** i)) 2proc >> $out
#    sudo ./ipc-static -vP l2 -i pipe -b $((2 ** i)) 2proc >> $out
#    sudo ./ipc-static -vP bus -i pipe -b $((2 ** i)) 2proc >> $out
#    sudo ./ipc-static -vP tlb -i pipe -b $((2 ** i)) 2proc >> $out
#done

#2proc, socket
#out="2proc-socket-local-mem.txt"
#rm $out
#for (( i=0; i < buf_range_idx; i++ ))
#do
#    echo $((2 ** i)) >> $out
    #sudo ./ipc-static -vP mem -i local -b $((2 ** i)) 2proc >> $out
#    sudo ./ipc-static -vP l1d -i local -b $((2 ** i)) 2proc >> $out
#    sudo ./ipc-static -vP l1i -i local -b $((2 ** i)) 2proc >> $out
#    sudo ./ipc-static -vP l2 -i local -b $((2 ** i)) 2proc >> $out
#    sudo ./ipc-static -vP bus -i local -b $((2 ** i)) 2proc >> $out
#    sudo ./ipc-static -vP tlb -i local -b $((2 ** i)) 2proc >> $out
#done

#2proc, socket -s
out="2proc-socket-local-s-mem.txt"
rm $out
for (( i=0; i < buf_range_idx; i++ ))
do
    echo $((2 ** i)) >> $out
    #sudo ./ipc-static -vP mem -i local -b $((2 ** i)) -s 2proc >> $out
    sudo ./ipc-static -vP l1d -i local -b $((2 ** i)) -s 2proc >> $out
    sudo ./ipc-static -vP l1i -i local -b $((2 ** i)) -s 2proc >> $out
    sudo ./ipc-static -vP l2 -i local -b $((2 ** i)) -s 2proc >> $out
    sudo ./ipc-static -vP bus -i local -b $((2 ** i)) -s 2proc >> $out
    sudo ./ipc-static -vP tlb -i local -b $((2 ** i)) -s 2proc >> $out
done

