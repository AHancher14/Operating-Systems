//vm_sim.c
//Alan Hancher

#include "config.h"
#include "vm.h"
#include "tlb.h"

#include <stdio.h>
#include <stdlib.h>


void vm_sim_start(char *inputFilePath) {
	int error;
	struct Sys vmSystem;
	struct LinkedAddress *logicAddressList;
	enum TlbAlgorithms algorithmChoice;
	char displayAddress;


	do {
		menu(&algorithmChoice, & displayAddress);
	}
	while(algorithmChoice == ERROR || displayAddress == -1);

	logicAddressList = logical_address_loader(inputFilePath);
	if(logicAddressList == NULL) {
		printf("There was an error reading from the file: %s\n", inputFilePath);
		return;
	}
	error = init_vm_system(&vmSystem, vmfile_PATH, OUTPUT_FILE_PATH, algorithmChoice);
	if (error) {
		printf("There was an error loading the system\n");
		return;
	}
	else {
		fprintf(vmSystem.output, "Welcome to Alan Hancher's VM Simulator Version 1.0 \n\n");
	}
	
	
	simulate_vm_system(&vmSystem, logicAddressList, displayAddress);
	printf("Check the results in the output file: %s \n", OUTPUT_FILE_PATH);
	
	shutdown_vm_system(&vmSystem);
	return; 


}

int create_system(struct Sys *system, char *vmFilePath, char *outputPath, enum TlbAlgorithms elg) {
	int i;
	
	system->vmfile = fopen(vmFilePath, "r");
	system->output = fopen(outputPath, "w");
	if(system->vmfile == NULL || system->output == NULL) {
		return 1;
	}
	
	for (i = 0; i < PAGE_TABLE_SIZE; i++) {
		system->tlb[i].age = NULL_TLB_AGE;
		system->tlb[i].page = NULL_PAGE;
		system->tlb[i].frame = NULL_FRAME;
	}
	
	for(i = 0; i < PAGE_TABLE_SIZE; i++) {
		system->ptable[i] = -1;
	}
	system->algorithmChoice = elg;
	return -0;
}


int simulate_system(struct Sys *system, struct LinkedAddress *logicAddressList, char displayAddress) {
	int error, total, val;
	int totalTlb, totalFaults, faults;
	float tlbRate, pageRate;	
	page_num page;
	frame_num frame;
	offset_t offset;
	addr_t laddr, paddr;
	
	if(system->output == NULL || system->vmfile == NULL) {
		return 1;
	}
	total = 0;
	totalTlb = 0;
	totalFaults = 0;
	
	while (logicAddressList != NULL) {
		if(logical_to_page_and_offset(logicAddressList->addr, &page, &offset) == 0) {
			error = search_tlb(system, page, &frame);
			if(error) {
				error = search_page_table(system, page, &frame);
				if(error) {
					++totalFaults;
					error = page_fault_handle(system, page, &frame);
				}
			}
			else {
				totalTlb++;
			}
		
			if (error == 0) {
				laddr = get_virtual_address(page, offset);
				paddr = get_physical_address(frame, offset);
				val = get_value_from_mem(system, paddr);
			
				if(displayAddress) {
					printf("Virtual Address: %u; Physical Address: %u; Value %d \n", laddr, paddr, val);
				}
				else {
					printf("No value found");
				}
			}
		else {
			printf("Error");
		}
		
		++total;
		logicAddressList = logicAddressList->next;
	}
	tlbRate = (float)totalTlb / (float)total * 100.0;
	pageRate = (float)totalFaults / (float)total * 100.0;
	printf("\n");
	printf("Page fault rate: %.2f%%\n", pageRate);
	printf("TLB hit rate: %.2f%%\n", tlbRate);
	printf("\n");
	return 0;
	
}

int shutdown_system(struct Sys *system) {
	fclose(system->output);
	fclose(system->vmfile);
	return 0;

}

void menu(enum TlbAlgorithms *algorithmChoice, char *displayAddress) {
	char a, b;
	if(algorithmChoice == NULL || displayAddress == NULL) {
		return;
	}

	printf("Welcome to Alan Hanchers VM Simulator version 1.0\n");
	printf("Number of logican pages: %d\n", PAGE_TABLE_SIZE);
	printf("Page size: %d bytes\n", PAGE_SIZE);
	printf("Page table size: %d\n", PAGE_TABLE_SIZE);
	printf("TLB size: %d\n", TLB_SIZE);
	printf("Number of physical frames: %d\n", FRAME_COUNT);
	printf("Physical memory size: %d bytes\n\n", PHYSICAL_MEM_SIZE);
	
	printf("Display physical Address? [1: yes, 2: no] >> ");
	scanf("%c", &a);
	getchar();
	if(a == '1') {
		*displayAddress = 1;
	}
	else if (a == '2') {
		*displayAddress = 0;
	}
	else {
		*displayAddress = -1;
	}
	printf("Choose TLB replacement Strategy [1: FIFO, 2:LRU] >> ");
	scanf("%c", &b);
	getchar();
	if(b == '1') {
		*algorithmChoice = FIFO;
	}
	else if (b == '2') {
		*algorithmChoice = LRU;
	}
	else {
		*algorithmChoice = ERROR;
	}
	fflush(stdin);
	return;

}
}

struct LinkedAddress *
create_LinkedAddress(addr_t addr, struct LinkedAddress *next) {
	struct LinkedAddress *ptr = malloc(sizeof(struct LinkedAddress));
	if (ptr == NULL) {
		return NULL;
	}
	ptr->addr = addr;
	ptr->next = NULL;
	return ptr;
}

void
delete_LinkedAddress(struct LinkedAddress * list) {
	struct LinkedAddress *ptr;
	while(list != NULL) {
		ptr = list;
		list = list->next;
		free(ptr);
	}
	return;
}
//
//This function loads the addresses from a file
//into a singly linked list
//

struct LinkedAddress *
logic_address_loader(char *path) {
	FILE * input;
	int v;
	int error;
	struct LinkedAddress *start;
	struct LinkedAddress *ptr;
	
	input = fopen(path, "r");
	error = fscanf(input, "%d", &v);
	if (error != 0 && error != EOF) {
		start = create_LinkedAddress(v, NULL);
		if (start == NULL) {
			fclose(input);
			return NULL;
		}
		ptr = start;
	}
	while(ptr != NULL) {
		error = fscanf(input, "%d", &v);
		if(error == 0) {
			delete_LinkedAddress(start);
			fclose(input);
			return NULL;
		}
		if(error != EOF) {
			ptr->next = createLinkedAddress(v, NULL);
		}
		ptr = ptr->next;
	}
	fclose(input);
	return start;
}






