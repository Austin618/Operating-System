#include "pagetable_generic.h"
#include <stdlib.h>


int *last_referenced_frame;

int latest_time;


/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int lru_evict(void)
{
	// assert(false);
	// return -1;
	int result = 0;

	for (unsigned int i = 0; i < memsize; i++) {
		if (last_referenced_frame[i] < last_referenced_frame[result]) {
			result = i;
		}
	}

	return result;
}

/* This function is called on each access to a page to update any information
 * needed by the LRU algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(int frame)
{
	latest_time += 1;

	last_referenced_frame[frame] = latest_time;
}

/* Initialize any data structures needed for this replacement algorithm. */
void lru_init(void)
{
	latest_time = 0;

	last_referenced_frame = malloc(memsize * sizeof(int));

	for (unsigned int i = 0; i < memsize; i++) {
		last_referenced_frame[i] = 0;
	}

}

/* Cleanup any data structures created in lru_init(). */
void lru_cleanup(void)
{
	free(last_referenced_frame);
	last_referenced_frame = NULL;
}
