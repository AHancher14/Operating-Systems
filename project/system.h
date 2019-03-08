//Alan Hancher
//system.h
#include <stdio.h>


#ifndef SYSTEM_H
#define SYSTEM_H


//declaring definitions
#define PAGE_TABLE_SIZE 256
#define PAGE_SIZE   256	/* number of bytes in a page or frame */
#define TLB_SIZE    16	/* number of entries in the TLB */
#define FRAME_COUNT 256
#define PHYSICAL_MEM_SIZE 65536
#define MINIMUM_INC 1
#define ADDRESS_MASK 0xFFFF
#define OFFSET_MASK 0xFF
#define vmfile_PATH "inputfile.txt"
#define OUTPUT_FILE_PATH "vm_sim_output.txt"
#define NULL_AGE 0xFFFFFFFF
#define NULL_PAGE -1
#define NULL_FRAME -1

typedef enum{FALSE, TRUE} bool;
typedef unsigned int addr_t;
typedef int page_t;
typedef int frame_t;
typedef int offset_t;
typedef int age_t;
//tlb data struct
struct tlbData{
    page_t  page;
    frame_t frame_num;
    bool    valid;/* indicate if this entry is valid */
    age_t age;
 };

typedef unsigned char pmem_t[PHYSICAL_MEM_SIZE];
typedef struct tlbData tlb_t[TLB_SIZE];
typedef frame_t pagetable_t[PAGE_TABLE_SIZE];
//enumerations for choosing which algorithm
enum TlbAlgorithms {
    ERROR,
    FIFO,
    LRU
};
//struct for holding everything
struct Sys{
    pmem_t mem; //Physical memory
    tlb_t tlb; // tlb
    pagetable_t pageTable; // page table
    enum TlbAlgorithms algo;
    FILE *vmfile;
    FILE *output;
};
//linked list for storing addresses
struct LinkedAddress{
    addr_t addr;
    struct LinkedAddress *next;
};

#endif
