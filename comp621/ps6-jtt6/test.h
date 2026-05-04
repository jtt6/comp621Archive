#ifndef TEST_H

#define TEST_H

#include <stdbool.h>

/* All data in this simplified heap is in binary trees. */
struct ABlock {
	int data;
	struct ABlock *ptr1;
	struct ABlock *ptr2;
};
typedef struct ABlock Block;

#define HEAPSIZE 10 /* Use a very small test heap.       */

extern Block heap[2][HEAPSIZE];	      /* The (simulated) heap.             */
extern unsigned int from_index;	      /* FROM space is heap[from_index][]  */
extern unsigned int to_index;	      /* TO   space is heap[to_index][]    */
extern unsigned int first_free_index; /* First free block is
				       * during allocation:
				       *  heap[from_index][first_free_index]
				       * during GC:
				       *  heap[to_index][first_free_index] */

/* Given a pointer, return whether the pointer belongs to the stack,
 * as opposed to the heap. */
bool is_ptr_on_stack(Block *);

/* Given pointers to source (FROM space) and destination (TO space) blocks,
 * respectively, copy the source to the destination. */
void copy_block(Block *, Block *);

/* Given pointers to source (FROM space) and destination (TO space) blocks,
 * respectively, mark the source as having been copied, and
 * leave a forwarding pointer to the destination. */
void mark_block_as_copied(Block *, Block *);

/* Given pointer to a block in FROM space, return whether it has been
 * copied. */
bool has_block_been_copied(Block *);

/* Given pointer to a block in FROM space that has been copied, return the
 * forwarding pointer to its copy in TO space. */
Block *location_of_copy(Block *);

#endif
