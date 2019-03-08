// Alan Hancher
// file.h

#ifndef VM_H
#define VM_H

#include <stdio.h>

//System constants
#define PAGE_TABLE_SIZE 256
#define TLB_SIZE 16
#define PAGE_SIZE 256
#define FRAME_SIZE 256
#define FRAME_COUNT 256
#define PHYSICAL_MEM_SIZE 65536
#define OFFSET_BITS 8
#define OFFSET_MASK 0xFF

//file path constants
#define vmfile_PATH "InputFile.txt"
#define OUTPUT_FILE_PATH "vm_sim_output.txt"

#define MINIMUM_TLB_AGE 1
#define NULL_TLB_AGE 0xFFFFFFFF
#define NULL_PAGE -1
#define NULL_FRAME -1

typedef unsigned int addr_t;
typedef int offset_t;
typedef int page_num;
typedef int frame_num;


struct TlbData{
	page_num page;
	frame_num frame;
	unsigned int age
	
};

typedef unsigned char memory_t[PHYSICAL_MEM_SIZE];
typedef struct TlbData tlb_t[TLB_SIZE];
typedef frame_num pagetable_t[PAGE_TABLE_SIZE];

enum TlbAlgorithms {
	ERROR,
	FIFO,
	LRU
};

struct Sys {
	memory_t mem;
	tlb_t tlb;
	pagetable_t ptable;
	FILE *vmfile;
	FILE *output;
	enum TlbAlgorithms algorithmChoice;


};

struct LinkedAddress {
	addr_t addr;
	struct LinkedAddress *next;

};

#endif

