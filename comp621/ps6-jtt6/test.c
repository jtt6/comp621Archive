#include "test.h"
#include "gc.h"
#include <stdio.h>

#include "csapp.h"

/**********
 * Globals
 **********/
Block heap[2][HEAPSIZE];	   /* The heap.                             */
unsigned int from_index = 0;	   /* FROM space is heap[from_index][]      */
unsigned int to_index = 1;	   /* TO   space is heap[to_index][]        */
unsigned int first_free_index = 0; /* First free block is
				    * heap[from_index][first_free_index]    */

/* Root pointers -- separated into convenient data structures to
 * simplify problem.
 */
Block *root_set[2 * HEAPSIZE];
unsigned int first_unused_root = 0;

/* An invalid pointer to serve as a flag.  Shouldn't be dereferenced. */
static Block *HAS_BEEN_COPIED = (Block *)1;

/* Garbage data to set as default values for the heap.
 * In real systems, the heap values are undefined and varies from system to
 * system. Some systems default it to 0. Others use guard values such as 0xEE.
 * We use sentinel values to test that heap data and pointers are not copied
 * incorrectly during garbage collection.
 */
static Block GARBAGE_BLOCK;
static Block STACK_BLOCK;
static int GARBAGE_DATA = 314159;
static int STACK_DATA = 271815;

static bool
is_garbage_block(Block *block_ptr) {
	return block_ptr != NULL && block_ptr->ptr1 == &GARBAGE_BLOCK &&
	       block_ptr->ptr2 == &GARBAGE_BLOCK;
}

bool
is_ptr_on_stack(Block *block_ptr) {
	return block_ptr == &STACK_BLOCK;
}

void
copy_block(Block *from_block_ptr, Block *to_block_ptr) {
	to_block_ptr->data = from_block_ptr->data;
	to_block_ptr->ptr1 = from_block_ptr->ptr1;
	to_block_ptr->ptr2 = from_block_ptr->ptr2;

	return;
}

void
mark_block_as_copied(Block *from_block_ptr, Block *to_block_ptr) {
	from_block_ptr->ptr1 = HAS_BEEN_COPIED;
	from_block_ptr->ptr2 = to_block_ptr;

	return;
}

bool
has_block_been_copied(Block *from_block_ptr) {
	return from_block_ptr->ptr1 == HAS_BEEN_COPIED;
}

Block
*location_of_copy(Block *from_block_ptr) {
	if (has_block_been_copied(from_block_ptr))
		return from_block_ptr->ptr2;
	else
		return NULL;
}

static bool
is_heap_full() {
	return first_free_index == HEAPSIZE;
}

void
print_block_for_debugging(Block *block_ptr) {
	if (block_ptr == NULL)
                printf("NULL");
        else if (block_ptr == &STACK_BLOCK)
		printf(" STACK POINTER");
	else if (is_garbage_block(block_ptr))
		printf(" GARBAGE BLOCK");
	else {
		printf(" data=%2d, ", block_ptr->data);
		if (block_ptr->ptr1 == NULL)
			printf("NULL");
		else if (block_ptr->ptr1 == &STACK_BLOCK)
			printf("STACK POINTER");
		else if (has_block_been_copied(block_ptr))
			printf("copy flag");
		else
			printf("ptr to data=%2d", block_ptr->ptr1->data);
		printf(", ");
		if (block_ptr->ptr2 == NULL)
			printf("NULL");
		else if(block_ptr->ptr2 == &STACK_BLOCK)
                        printf("STACK POINTER");
		else if (has_block_been_copied(block_ptr))
			printf("copy flag");
		else
			printf("ptr to data=%2d", block_ptr->ptr2->data);
	}
	printf("\n");

	return;
}

static void
print_heap_for_debugging() {
	unsigned int index;

	printf("Roots:\n");
	for (index = 0; index < first_unused_root; index++)
		print_block_for_debugging(root_set[index]);

	printf("FROM space data (section %d):\n", from_index);
	for (index = 0; index < HEAPSIZE; index++) {
		printf("  Block %2d:", index);
		print_block_for_debugging(&(heap[from_index][index]));
	}
	/*
	 * printf("TO space data (section %d):\n",to_index);
	 * for (index=0; index<HEAPSIZE; index+=1) {
	 * printf("  Block %2d:",index);
	 * print_block_for_debugging(&(heap[to_index][index]));
	 * }
	 */
	printf("free space pointer: %d\n", first_free_index);

	return;
}

static void
reset_space(unsigned int space) {
	unsigned int index;

	for (index = 0; index < HEAPSIZE; index++) {
		heap[space][index] = GARBAGE_BLOCK;
	}
}

static void
reset_heap() {
	reset_space(from_index);
	reset_space(to_index);
}

static void
gc() {
	unsigned int temp;
	unsigned int root;

	first_free_index = 0;

	/* Copy blocks from from_space to to_space.
	 * At end, first_free_index should be first free index into to_space.
	 */
	for (root = 0; root < first_unused_root; root++)
		gc_copy(&(root_set[root]));

	reset_space(from_index);

	/* Swap spaces. */
	temp = from_index;
	from_index = to_index;
	to_index = temp;

	return;
}

/* Functions for stack-based allocation.
 * Stack-based approach, while unusual, allows maintaining root_set,
 * to simplify gc().
 */
static void
push_root(Block *ptr) {
	root_set[first_unused_root] = ptr;
	first_unused_root++;

	return;
}

static
Block *pop_root() {
	first_unused_root--;

	return root_set[first_unused_root];
}

static void
make_empty() {
	push_root(NULL);

	return;
}

static void
make_stack_ptr() {
	push_root(&STACK_BLOCK);

	return;
}

static void
make_block(int data) {
	Block *ptr;
	Block *ptr1;
	Block *ptr2;

	if (is_heap_full()) {
		printf("\nGarbage collection started.\n");
		gc();
		printf("Garbage collection finished.\n\n");
	}

	if (is_heap_full()) {
		fprintf(stdout,
			"Heap full.  Can't allocate block.  Aborting.\n");
		exit(1);
	}

	/* Unsave roots that are on stack from special data structure. */
	ptr2 = pop_root();
	ptr1 = pop_root();

	/* Allocate first free block in FROM space to this block. */
	heap[from_index][first_free_index].data = data;
	heap[from_index][first_free_index].ptr1 = ptr1;
	heap[from_index][first_free_index].ptr2 = ptr2;
	ptr = &(heap[from_index][first_free_index]);

	first_free_index++;

	/* Save roots that are on stack into special data structure. */
	push_root(ptr);

	return;
}

int
main() {
	/***************************
	 * A sequence of test data.
	 ***************************/
	GARBAGE_BLOCK = (Block){GARBAGE_DATA, &GARBAGE_BLOCK, &GARBAGE_BLOCK};
	STACK_BLOCK = (Block){STACK_DATA, &STACK_BLOCK, &STACK_BLOCK};

	reset_heap();

	printf("\nTest: Create an initial tree.");
	/* Create a large tree filling most of the heap. */
	make_empty();

	make_empty();
	make_stack_ptr();
	make_block(4);

	make_empty();
	make_empty();
	make_block(5);

	make_block(3);

	make_block(2);

	make_stack_ptr();
	make_empty();
	make_block(7);

	make_empty();
	make_empty();
	make_block(9);

	make_stack_ptr();
	make_block(8);

	make_block(6);

	make_block(1);

	printf("Expected reachable graph, size=9:\n");
	printf("          1\n");
	printf("   2             6\n");
	printf("X     3      7      8\n");
	printf("    4   5   X X   9   X\n");
	printf("   X X X X       X X\n");
	printf("No unreachable garbage:\n");

	printf("\nStudent's heap:\n");
	print_heap_for_debugging();

	printf("\nTest: Cut off one subtree to create garbage.\n");

	/* Cut off part of the tree. */
	make_empty();

	/* Change right child of left child of root to be the new empty
	 * tree just created. */
	root_set[0]->ptr1->ptr2 = pop_root();

	printf("Expected reachable graph, size=6:\n");
	printf("       1\n");
	printf(" 2           6\n");
	printf("X X      7      8\n");
	printf("        X X   9  X\n");
	printf("             X X\n");
	printf("Unreachable garbage:\n");
	printf("   3\n");
	printf(" 4   5\n");
        printf("X X X X\n");

	printf("\nStudent's heap:\n");
	print_heap_for_debugging();

	printf("\nTest: Add to tree to force GC.\n");

	/* Add to the tree enough to force GC of data just deleted. */
	make_empty();
	make_empty();
	make_block(11);

	make_stack_ptr();
	make_empty();
	make_block(12);

	make_block(10);

	/* Change left child of left child of root to bethe new 10-11-12
	 * tree just created. */
	root_set[0]->ptr1->ptr1 = pop_root();

	printf("Expected reachable graph, size=9:\n");
	printf("              1\n");
	printf("        2             6\n");
	printf("    10     X      7      8\n");
	printf(" 11    12        X X   9   X\n");
	printf("X X   X X             X X\n");

	printf("\nStudent's heap:\n");
	print_heap_for_debugging();

	printf("\nTest: Cut off one subtree to create garbage.\n");

	/* Cut off part of the tree. */
	make_empty();

	/* Change right child of right child of root to be the new empty
	 * tree just created. */
	root_set[0]->ptr2->ptr2 = pop_root();

	printf("Expected reachble graph, size=7:\n");
	printf("              1\n");
	printf("        2           6\n");
	printf("    10     X      7   X\n");
	printf(" 11    12        X X\n");
	printf("X X   X X\n");
	printf("Unreachable garbage:\n");
	printf("   8\n");
	printf(" 9   X\n");
        printf("X X\n");

	printf("Student's heap:\n");
	print_heap_for_debugging();

	printf("\nTest: Add to tree to force GC, plus add cycle.\n");

	/* Add to the tree enough to force GC of data just deleted.
	 * Add a back pointer and cross pointer, too. */
	push_root(root_set[0]->ptr1);
	push_root(root_set[0]->ptr2);
	make_block(14);

	push_root(root_set[0]);
	make_block(13);

	/* Change right child of left child of root to be the new 13-14
	 * tree just created. */
	root_set[0]->ptr1->ptr2 = pop_root();

	printf("Expected reachable graph, size=9, with cycle:\n");
	printf("                   a=1\n");
	printf("           b=2             c=6\n");
	printf("    10           13       7   X\n");
	printf(" 11    12      14  a     X X\n");
	printf("X X   X X     b c\n");

	printf("\nStudent's heap:\n");
	print_heap_for_debugging();

	printf("\nTest: Cut off one subtree, creating garbage.\n");

	/* Cut off part of the tree. (Not removing back or cross pointers.) */
	make_empty();

	/* Change left child of right child of root to be the new empty
	 * tree just created. */
	root_set[0]->ptr2->ptr1 = pop_root();

	printf("Expected reachable graph, size=8, with cycle:\n");
	printf("                   a=1\n");
	printf("           b=2             c=6\n");
	printf("    10           13       X   X\n");
	printf(" 11    12      14  a\n");
	printf("X X   X X     b c\n");
	printf("Unreachable garbage:\n");
	printf("  7\n");
	printf(" X X\n");

	printf("\nStudent's heap:\n");
	print_heap_for_debugging();

	printf("\nTest: Add to tree to force GC.\n");

	/* Add to the tree enough to force GC of data just deleted. */
	make_empty();

	make_empty();
	make_stack_ptr();
	make_block(16);

	make_block(15);

	/* Change right child of right child of root to be the new 15-16
	 * tree just created. */
	root_set[0]->ptr2->ptr2 = pop_root();

	printf("Expected graph, size=10:\n");
	printf("                   a=1\n");
	printf("           b=2             c=6\n");
	printf("    10           13       X   15\n");
	printf(" 11    12      14  a         X  16\n");
	printf("X X   X X     b c               X X\n");
	printf("No unreachable garbage\n");

	printf("\nStudent's heap:\n");
	print_heap_for_debugging();

	return 0;
}
