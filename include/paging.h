// Jacob Laney
// jpl0015@auburn.edu

#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#include "config.h"

// get the page from a logical address
pagenum_t
get_page_number(addr_t logicalAddress);

// get an offset from a logical address
offset_t
get_page_offset(addr_t logicalAddress);

// get a virtual address from a page and offset
addr_t
get_virtual_address(pagenum_t page, offset_t offset);

// get an address from a frame and offset
addr_t
get_physical_address(framenum_t frame, offset_t offset);

// get the page and offset from a logical address
int
logical_to_page_and_offset(addr_t addr, pagenum_t *page, offset_t *offset);

// search the tlb, returns frame associated with page in the *frame param
// will return an error if page is not found
int
search_tlb(struct System *sys, pagenum_t page, framenum_t *frame);

// search the page table, returns frame associated with page in the *frame param
// will return an error if page is not found
int
search_page_table(struct System *sys, pagenum_t page, framenum_t *frame);

// load a page into memory
// returns frame associated with page in the *frame param
// updates the memory and page table
// returns 0 on success
int
page_fault_handle(struct System *sys, pagenum_t page, framenum_t *frame);

// load a new entry (page and frame)
// into the tlb using FIFO algorithm
int
tlb_fifo_replace(struct System *sys, pagenum_t page, framenum_t frame);

// load a new entry (page and frame)
// into the tlb using LRU algorithm
int
tlb_lru_replace(struct System *sys, pagenum_t page, framenum_t frame);

#endif
