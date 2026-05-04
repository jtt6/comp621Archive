#!/bin/bash
# This script was written to test the cachesim assignment
# for COMP 621.
# © Shaquille Que 2021
# Revised John Greiner 2022 for new input specification

CACHESIM=./cachesim

POINTS=0
TOTAL=0

#old:
#cachesim blocksize setsize cachesize policy
BSZ=1 # Block size, in words
SSZ=1 # Set size, in blocks
CSZ=1 # Cache size, in words -- must be a multiple of BSZ * SSZ

#new:
#cachesim sets blocks_per_set words_per_block policy
#SETS = 1    # SETS = old CSZ / (BSZ * SSZ)
#BPS  = 1    # BPS = old SSZ
#WPB  = 1    # WPB = old BSZ


EXPECTED_HITS=""
EXPECTED_NUMHITS=""

expect() {
    EXPECTED_HITS="$1"
    EXPECTED_NUMHITS="$2"
}

# $1 is number of points for this test
# $2 is the cache replacement policy
# The remaining arguments are passed as input to the cache simulator.
test() {
    SETS=$(( CSZ / (BSZ * SSZ) ))
    BPS=$(( SSZ ))
    WPB=$(( BSZ ))

    result=$(echo "${@:3} -1" | $CACHESIM $SETS $BPS $WPB $2)
    hits=$(echo "$result" | head -n 1)
    numHits=$(echo "$result" | tail -n 1)

    if [ "$hits" = "$EXPECTED_HITS" ] && [ "$numHits" = "$EXPECTED_NUMHITS" ]; then
        echo "PASSED... ${@:3}"
        POINTS=$(( $POINTS + $1 ))
    else
        echo "FAILED..."
        echo "     sets=$SETS block_per_sets=$BPS words_per_block=$WPB"
        echo "     Input: ${@:3}"
        echo "     Got : $hits $numHits"
        echo "     Want: $EXPECTED_HITS $EXPECTED_NUMHITS"
    fi
    
    TOTAL=$(( $TOTAL + $1 ))
}

testNoEvictions() {
    # trivial case
    expect 0 0.000000
    test 1 $1 0
    
    # single put and get
    expect 01 0.500000
    test 1 $1 0 0

    # put and then get repeatedly
    expect 011 0.666667
    test 1 $1 0 0 0

    # put and lots of gets
    expect 0111111111111111111111111111111111111111 0.975000
    test 1 $1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0

    # put other value
    expect 01111 0.800000
    test 1 $1 64 64 64 64 64
}

testDirectMapped() {
    BSZ=1
    SSZ=1 # fixed to 1 for direct-mapped
    CSZ=1

    testNoEvictions $1

    BSZ=2
    CSZ=2

    testNoEvictions $1

    BSZ=4
    CSZ=32
    
    testNoEvictions $1

    # 0 cache hits with eviction
    expect 000000000 0.000000
    test 5 $1 0 32 64 96 128 160 192 224 256

    # 0 cache hits with eviction
    expect 0000000000 0.000000
    test 5 $1 0 32 64 96 128 160 192 224 256 0

    # 1 cache hit
    expect 00000001 0.125000
    test 5 $1 0 32 64 96 128 160 192 192

    # only 2 elements
    expect 00111111 0.750000
    test 5 $1 0 32 0 32 0 32 0 32

    # only 3 elements
    expect 000111 0.500000
    test 5 $1 0 32 64 64 32 0

    # cache eviction on same tag
    expect 000000 0.000000
    test 5 $1 0 256 0 256 0 256

    # cache eviction on 1 set but not another
    expect 00001100 0.250000
    test 5 $1 0 256 32 0 32 0 256 0
}

testLRU() {
    echo "Testing LRU"
    POINTS=0
    TOTAL=0
    
    #***** Direct-mapped cache. *****#
    testDirectMapped LRU

    #***** 2-way set associative cache. *****#
    BSZ=4
    SSZ=2
    CSZ=64

    testNoEvictions LRU

    # no cache eviction on same tag for 2 elements
    expect 001111 0.666667
    test 5 LRU 0 256 0 256 0 256

    # cache evictions for scans
    expect 000000 0.000000
    test 5 LRU 0 256 512 0 256 512

    # reorder scans
    expect 000100 0.166667
    test 5 LRU 0 256 512 256 0 512
    
    # use all sets
    expect 0000000011111111 0.500000
    test 5 LRU 0 32 64 96 128 160 192 224 0 32 64 96 128 160 192 224

    # use several sets with evictions
    expect 000011010110000 0.333333
    test 5 LRU 0 32 64 256 0 0 288 64 320 32 256 512 0 576 64

    # ignore frequency -- higher weight
    expect 001111111000100 0.533333
    test 10 LRU 0 32 32 32 32 32 32 32 32 288 544 32 0 288 544

    
    #***** Fully associative cache. *****#
    BSZ=4
    SSZ=4
    CSZ=16

    testNoEvictions LRU

    # no cache eviction on same tag for 4 elements
    expect 00001111 0.500000
    test 5 LRU 0 256 512 768 0 256 512 768

    # cache evictions for scans
    expect 000000000000000000 0.000000
    test 5 LRU 0 32 64 96 128 160 192 224 256 0 32 64 96 128 160 192 224 256

    # reorder scan
    expect 0000011100 0.300000
    test 5 LRU 0 32 64 96 128 32 64 128 0 96

    # ignore frequency -- higher weight
    expect 001111110101000 0.533333
    test 15 LRU 0 32 32 32 32 32 32 32 64 64 96 0 128 32 64

    echo "--- LRU SCORE: $POINTS / $TOTAL"
}

testLFU() {
    echo "Testing LFU"
    POINTS=0
    TOTAL=0
    
    #***** Direct-mapped cache. *****#
    testDirectMapped LFU

    #***** 2-way set associative cache. *****#
    BSZ=4
    SSZ=2
    CSZ=64

    testNoEvictions LFU

    # no cache eviction on same tag for 2 elements
    expect 001111 0.666667
    test 5 LFU 0 256 0 256 0 256

    # no cache eviction on different tags
    expect 011001101111111 0.733333
    test 5 LFU 0 0 0 32 64 32 64 96 96 32 64 0 0 64 32

    # cache thrashing
    expect 0110000000 0.200000
    test 5 LFU 0 0 0 256 512 256 512 256 512 256

    # maximize cache
    expect 01111011111111111111 0.900000
    test 5 LFU 0 0 0 0 0 256 256 256 256 256 0 0 0 0 0 256 256 256 256 256

    # scanning
    expect 011011011100010 0.533333
    test 5 LFU 0 0 0 256 256 256 512 512 512 512 1024 0 256 512 1024
    
    # use all sets
    expect 00000000 0.000000
    test 5 LFU 0 32 64 96 128 160 192 224

    # new MFU retained -- higher weight
    expect 01101011101011110010 0.600000
    test 10 LFU 0 0 0 256 256 512 512 512 512 256 256 0 0 0 0 0 256 512 512 256

    
    #***** Fully associative cache. *****#
    BSZ=4
    SSZ=4
    CSZ=16

    testNoEvictions LFU

    # no cache eviction on same tag for 4 elements
    expect 00001111 0.500000
    test 5 LFU 0 256 512 768 0 256 512 768

    # cache evictions for scans
    expect 000000000000000000000000 0.000000
    test 5 LFU 0 32 64 96 128 160 192 224 256 288 320 352 384 416 448 480 0 32 64 96 128 160 192 224

    # respect frequency -- higher weight
    expect 001111110101001110 0.611111
    test 15 LFU 0 32 32 32 32 32 32 32 64 64 96 0 128 96 32 64 0 128

    

    echo "--- LFU SCORE: $POINTS / $TOTAL"
}

testFIFO() {
    echo "Testing FIFO"
    POINTS=0
    TOTAL=0
    
    #***** Direct-mapped cache. *****#
    testDirectMapped FIFO

    #***** 2-way set associative cache. *****#
    BSZ=4
    SSZ=2
    CSZ=64

    testNoEvictions FIFO

    # no cache eviction on same tag for 2 elements
    expect 001111 0.666667
    test 5 FIFO 0 256 0 256 0 256

    # cache evictions for scans
    expect 000000 0.000000
    test 5 FIFO 0 256 512 0 256 512

    # reorder scans
    expect 000101 0.333333
    test 5 FIFO 0 256 512 256 0 512
    
    # use all sets
    expect 0000000011111111 0.500000
    test 5 FIFO 0 32 64 96 128 160 192 224 0 32 64 96 128 160 192 224

    # use several sets with evictions
    expect 00001101011000000 0.294118
    test 5 FIFO 0 32 64 256 0 0 288 64 320 32 256 512 0 576 64 544 32

    # ignore frequency
    expect 0011111110001001 0.562500
    test 5 FIFO 0 32 32 32 32 32 32 32 32 288 544 32 0 288 544 288

    
    #***** Fully associative cache. *****#
    BSZ=4
    SSZ=4
    CSZ=16

    testNoEvictions FIFO

    # no cache eviction on same tag for 4 elements
    expect 00001111 0.500000
    test 5 FIFO 0 256 512 768 0 256 512 768

    # cache evictions for scans
    expect 000000000000000000 0.000000
    test 5 FIFO 0 32 64 96 128 160 192 224 256 0 32 64 96 128 160 192 224 256

    # reorder scan
    expect 0000011101 0.400000
    test 5 FIFO 0 32 64 96 128 32 64 128 0 96

    # ignore LRU -- higher weight
    expect 00001000000000 0.071429
    test 10 FIFO 0 32 64 96 0 128 0 160 32 192 64 224 96 128

    # ignore frequency -- higher weight
    expect 001111110101011 0.666667
    test 10 FIFO 0 32 32 32 32 32 32 32 64 64 96 0 128 32 64

    echo "--- FIFO SCORE: $POINTS / $TOTAL"
}

# $1 = v means print out failed cases.
runtests() {
    if [ "$1" = "v" ]; then
        testLRU
        testLFU
        testFIFO
    else
        testLRU > /dev/null
        echo "LRU SCORE: $POINTS / $TOTAL"
        testLFU > /dev/null
        echo "LFU SCORE: $POINTS / $TOTAL"
        testFIFO > /dev/null
        echo "FIFO SCORE: $POINTS / $TOTAL"
    fi
}

runtests $1
