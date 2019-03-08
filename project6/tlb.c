/*
 * Project 5: tlb.c
 * Xiao Qin.
 */
#include <stdio.h>
#include <limits.h>
#include "config.h"
#include "tlb.h"
//#define DEBUG

//
//This function searches the tlb table and sees page is found 
//and then retruns if their was a hit or not
//

int search_tlb(struct Sys *system, page_num page, frame_num *frame) {
	int hit = 0;
	int i = 0;
	
	for(i = 0; i < TLB_SIZE; i++) {
		if(system->tlb[i].page == page) {
			*frame = system->tlb[i].frame;
			system->tlb[i].age = MINIMUM_TLB_AGE;
			hit = 1;
		}
	system->tlb[i].age++;
	}
	//if hit is one, then the tlb hit
	if(hit == 1) {
		return 0;
	}
	//if not, then it missed
	else {
	return 1;
	}
}




int search_page_table(struct Sys *system, page_num page, frame_num *frame) {
	int hit = 0;
	int algorithmCall = 0;
	
	if (system->ptable[page] != NULL_PAGE) {
		*frame = system->ptable[page];
		hit = 1;
		if (system->algorithmChoice == FIFO) {
			algorithmCall == tlb_fifo_replace(system, page, *frame);
		}
		else if (system-> algorithmChoice == LRU) {
			algorithmCall = tlb_lru_replace(system, page, *frame);
		}
		else {
			return 1;
		}
	}
	return 1;

}




int page_fault_handler(struct Sys *system, page_num page, frame_num *frame) {
	int error;
	static int currentFrame;
	int memOffset = currentFrame * FRAME_SIZE;
	int vmmfileOffset = page * FRAME_SIZE;
	fseek(system->vmfile, vmmfileOffset, 0);
	
	error = fread(system->mem + memOffset, FRAME_SIZE, 1, system->vmfile);
	if (error != 1) {
		return 1;
	}
	
	system->ptable[page] = currentFrame;
	*frame = currentFrame;
	
	error = 0;
	switch(system->algorithmChoice) {
		case FIFO:
			error = tlb_fifo_replace(system, page, *frame);
			break;
		case LRU:
			error = tlb_lru_replace(system, page, *frame);
			break;
		default:
			error = 1;
			break;
	currentFrame = (currentFrame + 1) % FRAME_COUNT;
	return error;

	}

}
int tlb_fifo_replace(struct Sys *system, page_num page, frame_num frame){
	int i;
	static int head = 0;
	
	if(page == NULL_PAGE || frame == NULL_FRAME) {
		return 1;
	}
	system->tlb[head].age = MINIMUM_TLB_AGE;
	system->tlb[head].page = page;
	system->tlb[head].frame = frame;
	
	head = (head + 1) % TLB_SIZE;
	return 0;
}

int tlb_replacement_lru(struct Sys *system, page_num page, frame_num frame) {
	int i = 0;
	int maxAge;
	int maxIndex;
	

	if (page == NULL_PAGE || frame == NULL_FRAME) {
		return 1;
	}
	//set the max age to the first element in the list
	maxAge = system->tlb[0].age;
	maxIndex = 0;

	//search through the list comparing the variables
	//to find the one that has been their the longes
	for (i = 0; i < TLB_SIZE; i++) {
		if(system->tlb[i].age >= maxAge) {
			maxAge = system->tlb[i].age;
			maxIndex = i;
		}
	}
	//set the oldest entry with the new entry
	system->tlb[maxIndex].age = MINIMUM_TLB_AGE;
	system->tlb[maxIndex].page = page;
	system->tlb[maxIndex].frame = frame;
	return 0;



}

int get_page_num(addr_t laddress) {
	retrun (laddress >> 8) & 0xFF;
}

int get_offset(addr_t laddress, offset_t *off) {
	return laddress & 0xFF;

}
