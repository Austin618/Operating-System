/*
 * This code is provided solely for the personal and private use of students
 * taking the CSC369H course at the University of Toronto. Copying for purposes
 * other than this use is expressly prohibited. All forms of distribution of
 * this code, including but not limited to public repositories on GitHub,
 * GitLab, Bitbucket, or any other online platform, whether as given or with
 * any changes, are expressly prohibited.
 *
 * Authors: Andrew Peterson, Karen Reid, Alexey Khrabrov
 *
 * All of the files in this directory and all subdirectories are:
 * Copyright (c) 2019, 2021 Karen Reid
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pagetable_generic.h"
#include "pagetable.h"
#include "swap.h"


// The 1st level page table; page directory
pt_directory_entry_t pt_dir[PGDIR_SIZE]; 

// Counters for various events.
// Your code must increment these when the related events occur.
size_t hit_count = 0;
size_t miss_count = 0;
size_t ref_count = 0;
size_t evict_clean_count = 0;
size_t evict_dirty_count = 0;

/*
 * Allocates a frame to be used for the virtual page represented by p.
 * If all frames are in use, calls the replacement algorithm's evict_func to
 * select a victim frame. Writes victim to swap if needed, and updates
 * page table entry for victim to indicate that virtual page is no longer in
 * (simulated) physical memory.
 *
 * Counters for evictions should be updated appropriately in this function.
 */
static int allocate_frame(pt_entry_t *pte)
{
	int frame = -1;
	for (size_t i = 0; i < memsize; ++i) {
		if (!coremap[i].in_use) {
			frame = i;
			break;
		}
	}

	if (frame == -1) { // Didn't find a free page.
		// Call replacement algorithm's evict function to select victim
		frame = evict_func();
		assert(frame != -1);

		// All frames were in use, so victim frame must hold some page
		// Write victim page to swap, if needed, and update page table

		// IMPLEMENTATION NEEDED
		pt_entry_t *victim_pte = coremap[frame].pte;

		if (victim_pte -> dirty == 1) {
			// If there is no free frame, use a page-replacement algorithm to 
			// select any existing frame to be replaced, such frame is victim frame.

			// swap out: swap page table from disk and put it into main memory
			// swap in: swap page table from main memory and put it into disk
			int swap_offset = swap_pageout(frame, victim_pte -> offset);

			// Invalid swap out -> exit immediately
			if (swap_offset == INVALID_SWAP) {
				printf("INVALID SWAP OUT!");
				exit(1);
			}

			victim_pte -> offset = swap_offset;

			victim_pte -> on_swap = 1; // on_swap

			evict_dirty_count += 1;

		} else {
			evict_clean_count += 1;
		}

		victim_pte -> dirty = 0; // not dirty
		victim_pte -> valid = 0; // invalid
	} 

	// Record information for virtual page that will now be stored in frame
	coremap[frame].in_use = true;
	coremap[frame].pte = pte;

	return frame;
}

/*
 * Initializes your page table.
 * This function is called once at the start of the simulation.
 * For the simulation, there is a single "process" whose reference trace is
 * being simulated, so there is just one overall page table.
 *
 * In a real OS, each process would have its own page table, which would
 * need to be allocated and initialized as part of process creation.
 * 
 * The format of the page table, and thus what you need to do to get ready
 * to start translating virtual addresses, is up to you. 
 */
void init_pagetable()
{
	for (int i = 0; i < PAGE_SIZE; i++) {
		pt_dir[i].pd = NULL;
	}
}

pt_directory_entry_t init_mid_level() {
	// only one high page directory
	pt_directory_entry_t new_high_pd;
	// multiple mid page directories
	pt_directory_entry_t *mid_pd;

	mid_pd = (pt_directory_entry_t *)calloc(PGDIR_SIZE, PGDIR_SIZE * sizeof(pt_directory_entry_t *));

	for (int i = 0; i < PGDIR_SIZE; i++) {
		mid_pd[i].pd = NULL;
	}

	new_high_pd.pd = mid_pd;

	return new_high_pd;
}

pt_directory_entry_t init_low_level() {
	pt_directory_entry_t new_mid_pd;
	pt_entry_t *low_pt;

	// new_mid_pd = (pt_directory_entry_t *)malloc(sizeof(pt_directory_entry_t *));

	low_pt = (pt_entry_t *)calloc(PGDIR_SIZE, PAGE_SIZE * sizeof(pt_entry_t *));

	for (int i = 0; i < PAGE_SIZE; i++) {
		low_pt[i].frame = 0;
		low_pt[i].offset = INVALID_SWAP;
		low_pt[i].dirty = 0;
		low_pt[i].on_swap = 0;
		low_pt[i].reference = 0;
		low_pt[i].valid = 0;
	}

	new_mid_pd.pd = low_pt;

	return new_mid_pd;
}

/*
 * Initializes the content of a (simulated) physical memory frame when it
 * is first allocated for some virtual address. Just like in a real OS, we
 * fill the frame with zeros to prevent leaking information across pages.
 */
static void init_frame(int frame)
{
	// Calculate pointer to start of frame in (simulated) physical memory
	unsigned char *mem_ptr = &physmem[frame * SIMPAGESIZE];
	memset(mem_ptr, 0, SIMPAGESIZE); // zero-fill the frame
}

/*
 * Locate the physical frame number for the given vaddr using the page table.
 *
 * If the page table entry is invalid and not on swap, then this is the first 
 * reference to the page and a (simulated) physical frame should be allocated 
 * and initialized to all zeros (using init_frame).
 *
 * If the page table entry is invalid and on swap, then a (simulated) physical 
 * frame should be allocated and filled by reading the page data from swap.
 *
 * When you have a valid page table entry, return the start of the page frame
 * that holds the requested virtual page.
 *
 * Counters for hit, miss and reference events should be incremented in
 * this function.
 */
unsigned char *find_physpage(vaddr_t vaddr, char type)
{
	int frame = -1; // Frame used to hold vaddr

	// // To keep compiler happy - remove when you have a real use
	// (void)vaddr;
	// (void)type;
	// (void)allocate_frame;
	// (void)init_frame;

	// IMPLEMENTATION NEEDED
	unsigned high = vaddr >> 36; // first-directory
	unsigned mid = (vaddr >> 24) & (PGDIR_SIZE - 1); // second-directory
	unsigned low = (vaddr >> 12) & (PGDIR_SIZE - 1); // third-directory

	// Use your page table to find the page table entry (pte) for the 
	// requested vaddr. 

	pt_directory_entry_t *pd_mid = (pt_directory_entry_t *)pt_dir[high].pd;

	if (pd_mid == NULL) {
		// second level page directory is invalid
		pt_dir[high] = init_mid_level();
	}

	// pt_directory_entry_t *pd_mid = (pt_directory_entry_t *)pt_dir[high].pd;
	pt_entry_t *pt_low = (pt_entry_t *)pd_mid[mid].pd;

	if (pt_low == NULL) {
		pd_mid[mid] = init_low_level();
	}
	// At this time, pt_dir[high] != NULL
	// pt_entry_t *pt_low = (pt_entry_t *)pd_mid[mid].pd;

	pt_entry_t *target_pt = &pt_low[low];

	if (target_pt -> valid == 1) {
		hit_count += 1;
	} else {
		// page table invalid
		miss_count += 1;
		frame = allocate_frame(target_pt);

		if (target_pt -> on_swap == 1) {
			int swap_offset = swap_pagein(frame, target_pt -> offset);
			if (swap_offset == INVALID_SWAP) {
				printf("INVALID SWAP IN!");
				exit(1);
			}
			// target_pt -> frame = frame << PAGE_SHIFT;
			target_pt -> frame = frame;
			// target_pt -> frame = frame;
			target_pt -> dirty = 0;
			target_pt -> on_swap = 1;
		} else {
			init_frame(frame);
			// target_pt -> frame = frame << PAGE_SHIFT;
			target_pt -> frame = frame;
			// target_pt -> offset = INVALID_SWAP;
			target_pt -> dirty = 1;
		}
	}

	ref_count += 1;

	target_pt -> valid = 1;
	target_pt -> reference = 1;

	if (type == 'S' || type == 'M') {
		target_pt -> dirty = 1;
	}
	
	// Make sure that pte is marked valid and referenced. Also mark it
	// dirty if the access type indicates that the page will be written to.
	// (Note that a page should be marked DIRTY when it is first accessed, 
	// even if the type of first access is a read (Load or Instruction type).

	// Call replacement algorithm's ref_func for this page.
	assert(frame != -1);
	ref_func(frame); // ref_func(target_pt)

	// Return pointer into (simulated) physical memory at start of frame
	return &physmem[frame * SIMPAGESIZE];
}

void print_pagetable(void)
{
}


void free_pagetable(void)
{
}
