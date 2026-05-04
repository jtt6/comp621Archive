#include "stdio.h"
#include "test.h"

#include "csapp.h"

/* **from_block_ptr_ptr is a pointer to a pointer.
 * *from_block_ptr_ptr points into FROM space, or is NULL.
 * Your code will change *from_block_ptr_ptr to instead point into TO space.
 * Passing a pointer to a to-be-changed value is the C way to
 * pass that value by reference.
 * All the data is in the global variable heap[][].
 *
 * In additon to copying the pointed-to-block, this function
 * should traverse all data reachable from it, copying it into TO space.
 *
 * first_free_index gives first free index into TO space,
 * i.e., where to start copying into TO space.
 */
void gc_copy(Block **from_block_ptr_ptr) {
	/* Fill code here. */
	Block *from_block_ptr = *from_block_ptr_ptr;
	
	if (from_block_ptr == NULL) return;
	if (is_ptr_on_stack(from_block_ptr)) return;

	if (has_block_been_copied(from_block_ptr)) {
		*from_block_ptr_ptr = location_of_copy(from_block_ptr);
		return;
	}

	Block *new_block = &heap[to_index][first_free_index++];
	copy_block(from_block_ptr, new_block);
	mark_block_as_copied(from_block_ptr, new_block);

	gc_copy(&(new_block->ptr1));
	gc_copy(&(new_block->ptr2));

	*from_block_ptr_ptr = new_block;
}
