// Jacob Laney
// jpl0015@auburn.edu

#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>

// system constants
#define PAGE_TABLE_SIZE 256
#define TLB_SIZE 16
#define PAGE_SIZE 256
#define FRAME_SIZE 256
#define FRAME_COUNT 256
#define PHYSICAL_MEM_SIZE 65536

// file path constants
#define vmfile_PATH "BACKING_STORE"
#define OUTPUT_FILE_PATH "vm_sim_output.txt"

// numbers to designate null entries
// do not modify
#define MINIMUM_TLB_AGE 1
#define NULL_TLB_AGE 0xFFFFFFFF
#define NULL_PAGE -1
#define NULL_FRAME -1

// basic types
typedef unsigned int addr_t;
typedef int offset_t;
typedef int pagenum_t;
typedef int framenum_t;

// data structure for an entry in the tlb
struct TlbData {
    pagenum_t page;
    framenum_t frame;
    unsigned int age;
};

// the memory, tlb, and page table types
typedef unsigned char memory_t[PHYSICAL_MEM_SIZE];
typedef struct TlbData tlb_t[TLB_SIZE];
typedef framenum_t pagetable_t[PAGE_TABLE_SIZE];

// enumerate the algorithms offered for TLB replacement
enum TlbAlgorithms {
    ERROR,
    FIFO,
    LRU
};

// encapsulates all members of the system
struct System {
    memory_t mem; // physical memory
    tlb_t tlb; // tlb
    pagetable_t ptable; // page table
    FILE *vmfile; // virtual memory
    FILE *output; // the output file
    enum TlbAlgorithms algorithmChoice; // the tlb algorithm
};

// linked list of addresses
// this structure is used for storing logical addresses
struct LinkedAddress {
    addr_t addr;
    struct LinkedAddress *next;
};

#endif
