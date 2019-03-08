/*
 * Project 6 tlb.c
 * This file utilizes the functions that Dr. Qin wanted us to implement
 */
#include "system.h"
#include "tlb.h"
int tlb_init(tlb_t *tlb) {
    u_int_t i;
 
    for (i = 0; i < TLB_SIZE; i++)
        tlb->tlb_entry[i].valid = FALSE;

    return 0; /* success */
}

int tlb_display(tlb_t tlb) {
    u_int_t i;

    for (i = 0; i < TLB_SIZE; i++) { 
        printf("TLB entry %d, page num: %d, frame num: %d, ", i, tlb.tlb_entry[i].page_num, tlb.tlb_entry[i].frame_num);
        if (tlb.tlb_entry[i].valid == FALSE) 
            printf("Invalid\n");
        else printf("Valide\n");
    }

    return 0;
}



//function get_a_logic_address() takes in the logical address
//And converts it to page and offset by calling the get_page_number
//and get_page_offset functions
int get_a_logic_address(addr_t logical_address, page_t *page, offset_t *offset) {
	*page = get_page_number(logical_address, *page);
	*offset = get_page_offset(logical_address);
	return 0;
}

/*
 * get_page_number() takes in the logical address and a page number
 * and returns the page from a logical address
 */
int get_page_number(addr_t logic_address, page_t *page_num){
	int pageNumber = ((logic_address & ADDRESS_MASK)>>8);
	return pageNumber;
}

/*
 * get_page_offset() returns the offset of a logical address
 */
int get_page_offset(addr_t logic_address, offset_t *off) {
	int offset = (logic_address & OFFSET_MASK);
	return offset;
}

/*
 * search_tlb() checks to see if their is a tlb hit or not
 * The boolean value would get flipped to true if there is a hit
 */
int search_tlb(page_num page, struct Sys *system, bool *is_tlb_hit, frame_num *frame) {
	int hit = 0;

	int i = 0;
	for(i = 0; i < TLB_SIZE; i++) {
		if(system->[page] == NULL_PAGE) {
		//This means there was a tlb hit
			is_tlb_hit = TRUE;
			*frame = system->tlb[i].frame
			system->tlb[i].age = MINIMUM_TLB_AGE;
			hit = 1;
		}
		system->tlb[i].age++;
	}
	return 1;
}
			
/*
 * This function creates the physical address
 */
addr_t create_physical_addr(frame_num frame, offset_t offset, physical_addr *p_addr) {} {
	p_addr = frame * FRAME_SIZE + offset;
	retrun p_addr;
}


/*
 * search_page_table() searchers through the page table to find any faults
 *
 */
int search_page_table(page_num page, struct Sys system, bool *is_page_fault, frame_num *frame) {
	int hit = 0;
	int i = 0;
	if (system->pagetable[page] != NULL_PAGE) {
		*frame = system->pagetable[page];
		hit = 1;
		is_page_fault = FALSE;
	}
	else {
		return 1;
	}
	if (hit == 1) {
		return 0;
	}

}

int tlb_fifo_replace(page_t page, frame_t frame, struct Sys *system) {
	static int first = 0;
	if (page == NULL_PAGE || frame == NULL_FRAME) {
		return 1;
	}
	system->tlb[first].age = MINIMUM_TLB_AGE;
	system->tlb[first].page = page;
	system->tlb[first].frame = frame;

	first = (first + 1) % TLB_SIZE;
	return 0;
}

//LRU replacement algorithm that gets rid of the one whose age had been the oldest
int tlb_replacement_LRU(page_num page, frame_num frame, struct Sys *system) {
	int i;
	int maxAge;
	int maxIndex;
	
	if (page == NULL_PAGE || frame == NULL_FRAME) {
		return 1;
	}
	maxAge = system->tlb[0].age;
	maxIndex = 0;
	for (i = 0; i < TLB_SIZE; i++) {
		if(system->tlb[i].age >= maxAge) {
			maxAge = system->tlb[i].age;
			maxIndex = i;
		}
	}
	system->tlb[maxIndex].age - MINIMUM_TLB_AGE;
	system->tlb[maxIndex].page = page;
	system->tlb[maxIndex].frame - frame;
	return 0;
}

// page_fault_handler gets called when there is a page fault and it calls one of the two replacement algorithms	
int page_fault_handler(page_t page, frame_t frame, addr_t *p_addr, pagetable_t *pageTable, struct Sys *system) {
	int error;
	static int currentFrame = 0;
	
	int memOffset = currentFrame * FRAME_SIZE;
	int vmfileOffset = page * PAGE_SIZE;
	
	fseek(system->vmfile, vmfileOffset, 0);
	error = fread(system->mem + memOffset, FRAME_SIZE, 1, system->vmfile);
	if (error != 1) {
		retrun 1;
	}
	system->pagetable[page] = currentFrame;
	frame = currentFrame;
	
	error = 0;
	switch(system->algorithmChoice) {
		case FIFO:
			error = tlb_fifo_replace(page, frame, *system);
			break;
		case LRU:
			error = tlb_repleacement_LRU(page, frame, *system);
			break;
		default:
			error = 1;
			break;
	}
	currentFrame = (currentFrame + 1) % FRAME_COUNT;
	retrun error;
}

addr_t get_virtual_address(page_t page, offset_t offset) {
	return page * PAGE_SIZE + offset;

}



