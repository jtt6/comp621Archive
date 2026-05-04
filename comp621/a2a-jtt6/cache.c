#include <stdbool.h>
#include <stdio.h>
#include "cachesim.h"
#include "cache.h"
#include "include/csapp.h"

// This is a stub version just to check for compilation and simple runtime errors.


// Global variables
unsigned int  **tag;          // Tags                         -- use as tag[set#][block#]
bool          **valid;        // Valid bits                   -- use as valid[set#][block#]
int           **access_time;  // Last access timestamp (LRU)  -- use as access_time[set#][block#]
int           **access_count; // Access count          (LFU)  -- use as access_count[set#][block#]
int           **load_time;    // Load timestamp        (FIFO) -- use as load_time[set#][block#]



// Allocates and initializes the cache, which must be statically defined
// (i.e., typically a global variable)
void
cache_init(
    unsigned int sets,
    unsigned int blocks_per_set)
{

    // Allocate space.
    // One tag, one valid bit, ... per block.
    // For code simplicity, we'll keep these in separate arrays,
    // instead of having a single 2D array of structs.
    tag          = (unsigned int **) Malloc(sets * sizeof(unsigned int *));
    valid        = (bool         **) Malloc(sets * sizeof(bool *));
    access_time  = (int          **) Malloc(sets * sizeof(int *));
    access_count = (int          **) Malloc(sets * sizeof(int *));
    load_time    = (int          **) Malloc(sets * sizeof(int *));

    for (unsigned int set=0; set<sets; set++) {
        tag[set]          = (unsigned int *) Malloc(blocks_per_set * sizeof(unsigned int));
        valid[set]        = (bool         *) Malloc(blocks_per_set * sizeof(bool));
        access_time[set]  = (int          *) Malloc(blocks_per_set * sizeof(int));
        access_count[set] = (int          *) Malloc(blocks_per_set * sizeof(int));
        load_time[set]    = (int          *) Malloc(blocks_per_set * sizeof(int));
    }

    // Initialize all valid bits to false.
    for (unsigned int set=0; set<sets; set++)
        for (unsigned int block=0; block<blocks_per_set; block++)
            valid[set][block] = false;

    // Don't need to initialize the other arrays since they will only be accessed
    // when the corresponding valid bit is true.
}


// Simulates an access to the given memory address.
// Returns whether this was a cache hit.
bool
cache_access(
    unsigned int current_timestamp,
    unsigned int address,
    unsigned int sets,
    unsigned int blocks_per_set,
    unsigned int words_per_block,
    enum cachepolicy policy)
{
    bool hit = false;

    // DELETE THIS.  Included just to make the stub compile.
    // fprintf(stderr, "%u %u %u %u %u %u\n",
    //      current_timestamp, address,
    //      sets, blocks_per_set, words_per_block, policy);

    // COMPLETE THIS.

    unsigned int addr = address;
    
    // Remove low 3 bits (word alignment)
    addr >>= 3;

    // Remove offset bits
    unsigned int block_offset_bits = __builtin_ctz(words_per_block);
    addr >>= block_offset_bits;
	
    // Get set index
    unsigned int set_index_bits = __builtin_ctz(sets);
    unsigned int set_index = addr & (sets - 1);

    // Get block tag
    unsigned int block_tag = addr >> set_index_bits;

    // Check cache using valid bit and tag
    for (unsigned int b = 0; b < blocks_per_set; b++) {
	if (valid[set_index][b] && tag[set_index][b] == block_tag) {
	    if (policy == POLICY_LRU) {
		access_time[set_index][b] = current_timestamp;
	    } else if (policy == POLICY_LFU) {
		access_count[set_index][b]++;
		access_time[set_index][b] = current_timestamp;
	    } 
	    return true;
	}
    }

    // Find block for cache miss if there is an empty block
    unsigned int new_block = 0;
    for (unsigned int b = 0; b < blocks_per_set; b++) {
    	if (!valid[set_index][b]) {
	    new_block = b;
	    break;
	}
    }

    // If all blocks are taken, use cache replacement policy
    if (valid[set_index][new_block]) {
	switch(policy) {
            case POLICY_LRU:
		new_block = 0;
		for (unsigned int b = 1; b < blocks_per_set; b++) {
		    if (access_time[set_index][b] < access_time[set_index][new_block]) {
			new_block = b;
		    }
		}
	        break;
	    case POLICY_LFU:
		new_block = 0;
		for (unsigned int b = 1; b < blocks_per_set; b++) {
		    if (access_count[set_index][b] < access_count[set_index][new_block]) {
			new_block = b;
	            } else if (access_count[set_index][b] == access_count[set_index][new_block]) {
		    	if (access_time[set_index][b] < access_time[set_index][new_block]) {
				new_block = b;
			}
		    }
		}
		break;
	    case POLICY_FIFO:
		new_block = 0;
		for (unsigned int b = 1; b < blocks_per_set; b++) {
	            if (load_time[set_index][b] < load_time[set_index][new_block]) {
		       new_block = b;
		    }
		}
		break;
	}
    }

    // Update cache
    tag[set_index][new_block] = block_tag;
    valid[set_index][new_block] = true;
    if (policy == POLICY_LRU) {
        access_time[set_index][new_block] = current_timestamp;
    } else if (policy == POLICY_LFU) {
	access_count[set_index][new_block] = 1;
        access_time[set_index][new_block] = current_timestamp;
    } else if (policy == POLICY_FIFO) {
	load_time[set_index][new_block] = current_timestamp;
    }

    return hit;
}
