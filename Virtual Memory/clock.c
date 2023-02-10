#include "pagetable_generic.h"
#include <stdlib.h>


int *reference;

int current_position;

/* Page to evict is chosen using the CLOCK algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int clock_evict(void)
{
	// assert(false);
	// return -1;
	while (reference[current_position] != 0) {
		reference[current_position] = 0;
		current_position = (current_position + 1) % memsize;
	}

	reference[current_position] = 1;
	int result = current_position;
	current_position = (current_position + 1) % memsize;
	return result;
}

/* This function is called on each access to a page to update any information
 * needed by the CLOCK algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void clock_ref(int frame)
{
	// (void)frame;
	
	reference[frame] = 1;
}

/* Initialize any data structures needed for this replacement algorithm. */
void clock_init(void)
{
	current_position = 0;

	reference = malloc(memsize *sizeof(int));

	for (unsigned int i = 0; i < memsize; i ++) {
		reference[i] = 0;
	}
}

/* Cleanup any data structures created in clock_init(). */
void clock_cleanup(void)
{
	free(reference);
	reference = NULL;
}
