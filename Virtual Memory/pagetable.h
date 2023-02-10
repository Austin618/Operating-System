#ifndef __PAGETABLE_H__
#define __PAGETABLE_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>


// User-level virtual addresses on a 64-bit Linux system are 48 bits in our
// traces, and the page size is 4096 (12 bits). The remaining 36 bits are
// the virtual page number, which is used as the lookup key (or index) into
// your page table. 

#define PG_VALID        (0x1) // Valid bit in pgd or pte, set if in memory
#define PG_DIRTY        (0x2) // Dirty bit in pgd or pte, set if modified
#define PG_REF          (0x4) // Reference bit, set if page has been referenced
#define PG_ONSWAP       (0x8) // Set if page has been evicted to swap

#define PGDIR_SHIFT     24
#define PGDIR_SIZE      4096

// Page table entry 
// This structure will need to record the physical page frame number
// for a virtual page, as well as the swap offset if it is evicted. 
// You will also need to keep track of the Valid, Dirty and Referenced
// status bits (or flags). 

// You do not need to keep track of Read/Write/Execute permissions.
typedef struct pt_entry_s {
	// Add any fields you need ...
	// unsigned long int; page directry entry
	// struct pt_directory_entry_t *pd;
	long int frame;
	off_t offset;
	int dirty;
	int valid;
	int reference;
	int on_swap;
} pt_entry_t;

typedef struct pt_directory_entry_s {
	void *pd;
	// long int frame;
	// int valid;

	// unsigned int frame;
	// off_t swap_off;
} pt_directory_entry_t;


// Page

#endif /* __PAGETABLE_H__ */
