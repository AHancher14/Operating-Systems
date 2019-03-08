//Alan Hancher
//tlb.h

#ifndef TLB.h
#define TLB.h

#include "config.h"

int search_tlb(struct Sys *system, page_num *page, frame_num *frame);
int search_page_table(struct Sys *system, page_num *page, frame_num *frame);
int page_fault_handler(struct Sys *system, page_num page, frame_num *frame);
int tlb_fifo_replace(struct Sys *system, page_num page, frame_num frame);
int tlb_replacement_lru(struct Sys *system, page_num page, frame_num frame);
int get_page_num(addr_t laddress);
int get_offset(addr_t laddress, offset_t *off);

#endif

