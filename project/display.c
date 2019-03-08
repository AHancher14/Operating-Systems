//Alan Hancher
#include "system.h"
#include "display.h"
#include "tlb.h"
#include <stdio.h>
#include <stdlib.h>

/*
*initialize allow for the system to run
*/

void display_begin(char *inputFilePath){
    int error;
    int check;
    addr_t values;
    struct Sys vmSystem;
    struct LinkedAddress * logicAddressList;
    char displayAddresses;
    enum TlbAlgorithms algorithmChoice;
	
    logicAddressList = logic_address_reader(inputFilePath);
    check = display_start(&vmSystem, vmfile_PATH, OUTPUT_FILE_PATH, algorithmChoice);
    if(check == 1) {
        printf("There was an error reading the file: %s\n", inputFilePath);
        return;
    }
    error = display_start(&vmSystem, vmfile_PATH, OUTPUT_FILE_PATH, algorithmChoice);
    if (error){
        printf("There was an error loading the system\n");
        return;
    }
    else {
        fprintf(vmSystem.output, "Alan Hancher's VM Simulator Version 1.0 \n\n");
    }
    switch(vmSystem.algorithmChoice){
        case FIFO:
            fprintf(vmSystem.output, "### TLB FIFO REPLACEMENT ALGORITHM###\n\n");
            break;
        case LRU:
            fprintf(vmSystem.output, "### TLB LRU REPLACEMENT ALGORITHM###\n\n");
            break;
        default:
            printf("The VM System does not have a tlb replacement algorithm\n");
            shutdown(&vmSystem);
            return;

    }
    run_display(&vmSystem, logicAddressList, displayAddresses);
    printf("to view results, check the output file: %s \n", OUTPUT_FILE_PATH);
    shutdown(&vmSystem);
    return;


}

//initialiing values from the files
int display_start(struct Sys *system, char *vmfilePath, char *outputPath){
    int i;

    system->vmfile = fopen(vmfilePath, "r");
    system->output = fopen(outputPath, "w");
    if (system->vmfile == NULL || system->output == NULL) {
        return 1;
    }
    for (i = 0; i < PHYSICAL_MEM_SIZE; i++) {
        system->mem[i] = 0;
    }
    for (i = 0; i < TLB_SIZE; i++) {
        system->tlb[i].page = NULL_PAGE;
        system->tlb[i].frame_num = NULL_FRAME;
        system->tlb[i].age = NULL_AGE;

    }
    for (i = 0; i < PAGE_TABLE_SIZE; i++) {
        system->pageTable[i] = -1;
    }
    
    return 0;
}

/*
*this is where I got to before I had to turn it in.
*I was using the logic from worksheet "project 6 part 4"
*/
int run_display(struct Sys *system, struct LinkedAddress *logicAddressList, char displayAddress) {
    int error, total, value;
    int totalTlbHit, totalFaults, faults;
    float tlbRate, faultRate;
    bool is_tlb_hit, is_page_fault;
    page_t page;
    frame_t frame;
    offset_t offset;
    addr_t vAddr, pAddr;
    pagetable_t pageTable;

    if (system->output == NULL || system->vmfile == NULL) {
        return 1;
    }
    total = 0;
    totalTlbHit = 0;
    totalFaults = 0;
   
	//first started out but loading in addresses using the 
	//logic address reader function
    while (logicAddressList != NULL) {
	//while it is still loading in addresses
	//get the address, page, and offset
        if(get_a_logic_address(logicAddressList->addr, &page, &offset) == 0) {
	    //then search the tlb for a hit
            error = search_tlb(page, system, is_tlb_hit, &frame);
		//if there isnt a hit then search the page table for a fault
                if (error) {
                    error = search_page_table(page, system, is_page_fault, &frame);
		    //if there is a fault, go to the page_fault_handler
                    if (error) {
                        ++totalFaults;
                        error = page_fault_handler(page, frame, &pAddr, pageTable, &system);

                    }
                }
		//if there is a hit, increment the counter
                else {
                    totalTlbHit++;
                }
		//then get the virtual address and create a phyiscal address
                if(error == 0) {
                    vAddr = get_virtual_address(page, offset);
                    pAddr = create_physical_addr(page, offset, &pAddr);
                    value = get_value_from_mem(system, pAddr);
			//tried to set up the function to print our values here
                    if(displayAddress){
                        printf("Virtual Address: %u; Physical Address: %u; Value: %d \n", vAddr, pAddr, value);

                    }
                    else {
                        printf("Virtual Address: %u; Value: %d \n", vAddr, value);
                        fprintf(system->output, "Virtual Address: %u; Physical Address: %u; Value: %d \n", vAddr, pAddr, value);

                    }
                }
                else {
                    printf("!!! Unresolved logical address: %u\n", logicAddressList->addr);
                }

            }
        ++total;
        logicAddressList = logicAddressList->next;
        }
    //calculate the rates and the misses etc.
    tlbRate = (float)totalTlbHit / (float)total * 100.0;
    faultRate = (float)totalFaults / (float)total * 100.0;
    printf("\n");
    printf("* Page fault rate: %.2f%%\n", faultRate);
    fprintf(system->output, "\n* Page fault rate: %.2f%%\n", faultRate);
    printf("* TLB hit rate: %.2f%%\n", totalTlbHit);
    fprintf(system->output, "* TLB hit rate: %.2f%%\n", totalTlbHit);
    printf("\n");

    return 0;

}

//tell the system to stop
int shutdown(struct Sys *system){
    fclose(system->output);
    fclose(system->vmfile);
    return 0;
}






//get a value from memory
int get_value_from_mem(struct Sys *system, addr_t physaddr) {
    return system->mem[physaddr];
}
//initialize a linked list
struct LinkedAddress *
create_LinkedAddress(addr_t addr, struct LinkedAddress *next) {
    struct LinkedAddress *ptr = malloc(sizeof(struct LinkedAddress));
    if(ptr == NULL) {
        return NULL;
    }
    ptr->addr = addr;
    ptr->next = NULL;
    return ptr;

}
//delete the pointer in the list
void delete_LinkedAddress(struct LinkedAddress * list) {
    struct LinkedAddress *ptr;
    while (list != NULL) {
        ptr = list;
        list = list->next;
        free(ptr);
    }
    return;
}
//this was one of the more important functions because it laid the groundwork for the other functions
//source: www.stackoverflow.com
struct LinkedAddress *
        logic_address_reader(char *file) {

    FILE *fp;
    fp = fopen(file, "r");
    int i;
    int value;
    struct LinkedAddress *start;
    struct LinkedAddress *ptr;

    if (fp == NULL){
        return NULL;
    }
    value = fscanf(fp, "%d", &i);
    if(value != 0 && value != EOF) {
        start = create_LinkedAddress(i, NULL);
        ptr = start;
    }
    while(ptr != NULL) {
        value = fscanf(fp, "%d", &i);
        if (value == 0) {
            delete_LinkedAddress(start);
            fclose(fp);
            return NULL;
        }
        if (value != EOF) {
            ptr->next = create_LinkedAddress(i, NULL);
        }
        ptr = ptr->next;
    }
    fclose(fp);

    return NULL;
}



