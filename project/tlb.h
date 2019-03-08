/*
 * Alan Hancher
 * tlb.h
 */
#ifndef TLB_H
#define TLB_H


#include "system.h"




int init_tlb(tlb_t* tlb);
int TLB_display(tlb_t tlb);
int search_tlb(page_t page, struct Sys *system, bool *is_tlb_hit, frame_t *frame);
int get_a_logic_address(addr_t addr, page_t *page, offset_t *offset);
int get_page_offset(addr_t logic_address, offset_t *off);
int search_page_table(page_t page, struct Sys *system, bool *is_page_fault, frame_t *frame);
int page_fault_handler(page_t page, frame_t frame, addr_t *p_addr, pagetable_t *pageTable, struct Sys *system);
addr_t get_virtual_address(page_t page, offset_t offset);
addr_t create_physical_addr(frame_t frame, offset_t offset, addr_t *p_addr);
int get_page_number(addr_t logic_address, page_t *page_num);
int tlb_fifo_replace(page_t page, frame_t frame, struct Sys *system);
int tlb_replacement_LRU(page_t page, frame_t frame, struct Sys *system);
int read_physical_memory(physadr p_addr, struct Sys *system);
#endif
