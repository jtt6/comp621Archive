#include <stdbool.h>
#include "cachesim.h"

// Allocates and initializes the cache, which must be statically defined
// (i.e., typically a global variable)
void
cache_init(
    unsigned int sets,
    unsigned int blocks_per_set);


// Simulates an access to the given memory address.
// Returns whether this was a cache hit.
bool
cache_access(
    unsigned int current_timestamp,
    unsigned int address,
    unsigned int sets,
    unsigned int blocks_per_set,
    unsigned int words_per_block,
    enum cachepolicy policy);

