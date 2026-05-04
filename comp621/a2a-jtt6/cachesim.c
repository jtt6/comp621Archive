#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include "cachesim.h"
#include "cache.h"

bool
is_power2(
    unsigned int num)
{
    if (num == 0)
        return false;

    for (unsigned int power = 1; power > 0; power = power << 1) {
        if (power == num)
            return true;
        if (power > num)
            return false;
    }

    return false;
}


void
cachesim(
    unsigned int sets,
    unsigned int blocks_per_set,
    unsigned int words_per_block,
    enum cachepolicy policy)
{
    unsigned int num_accesses = 0;  // Also used as timestamp.
    int address;     // Signed so that a negative value can terminate input.
    bool hit;
    int num_hits = 0;
    float hitrate;


    cache_init(sets, blocks_per_set);

    do {
        scanf("%d", &address);
        if (address >= 0) {
            if (address % 8) {
                fprintf(stderr, "Bad memory address: %d.  Value ignored.\n", address);
            }
            else {
                hit = cache_access(num_accesses, (unsigned int) address,
                                   sets, blocks_per_set, words_per_block,
                                   policy);
                num_accesses++;
                num_hits += hit;

                printf("%d", hit);
            }
        }
    } while (address >= 0);

    hitrate = num_hits / (float) num_accesses;
    printf("\n%f\n", hitrate);
}


int
main(
    int argc,
    char * argv[])
{
    bool called_ok = true;
    unsigned int sets;
    unsigned int blocks_per_set;
    unsigned int words_per_block;
    enum cachepolicy policy;


    if (argc == 5) {
        sets = (unsigned int) atoi(argv[1]);
        blocks_per_set = (unsigned int) atoi(argv[2]);
        words_per_block = (unsigned int) atoi(argv[3]);
        called_ok = called_ok &&
                    is_power2(sets) &&
                    is_power2(blocks_per_set) &&
                    is_power2(words_per_block);

        if (strcasecmp(argv[4], "LRU") == 0)
            policy = POLICY_LRU;
        else if (strcasecmp(argv[4], "LFU") == 0)
            policy = POLICY_LFU;
        else if (strcasecmp(argv[4], "FIFO") == 0)
            policy = POLICY_FIFO;
        else
            called_ok = false;
    }
    else
        called_ok = false;

    if (!called_ok) {
        fprintf(stderr, "Usage:  %s sets blocks_per_set words_per_block policy\n", argv[0]);
        fprintf(stderr, "   sets is a power of 2.\n");
        fprintf(stderr, "   blocks_per_set is power of 2.\n");
        fprintf(stderr, "   words_per_blocks is power of 2\n");
        fprintf(stderr, "   policy is one of LRU, LFU, FIFO.\n");

        exit(1);
    }

    cachesim(sets, blocks_per_set, words_per_block, policy);
}
