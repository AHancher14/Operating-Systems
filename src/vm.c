// Group 8
// written by Jacob Laney
// jpl0015@auburn.edu

#include "config.h"
#include "vm.h"
#include "paging.h"

#include <stdio.h>
#include <stdlib.h>

void
vm_sim_start(char *inputFilePath) {
    int err; // manage errors
    struct System vmSystem; // encapsulates the computer system
    struct LinkedAddress * logicAddressList; // list of logical addresses
    enum TlbAlgorithms algorithmChoice; // stores tlb replacement alg choice
    char shouldDisplayPhysicalAddresses; // stores display choice
    
    
    // get the tlb replacement algorithm option from the user
    do {
        menu(&algorithmChoice, &shouldDisplayPhysicalAddresses);
    }
    while (algorithmChoice == ERROR || shouldDisplayPhysicalAddresses == -1);
    
    // populate a linked list of logical addresses
    logicAddressList = logical_addresses_from_file(inputFilePath);
    if (logicAddressList == NULL) {
        printf("There was an error reading integers from file: %s\n", inputFilePath);
        return;
    }
    
    // initialize the system
    err = init_vm_system(&vmSystem, vmfile_PATH, OUTPUT_FILE_PATH, algorithmChoice);
    if (err) {
        printf("There was an error loading the system\n");
        return;
    }
    else {
        // system initialized properly
        fprintf(vmSystem.output, "Group 8's VM Simulator Version 1.00 \n\n");
        fprintf(vmSystem.output, "#### INPUT FILE: %s #### \n\n", inputFilePath);
    }
    
    // print the algorithm choice to file
    switch (vmSystem.algorithmChoice) {
        case FIFO:
            fprintf(vmSystem.output, "#### FIFO TLB REPLACEMENT ALGORITHM ####\n\n");
            break;
        case LRU:
            fprintf(vmSystem.output, "#### LRU TLB REPLACEMENT ALGORITHM ####\n\n");
            break;
        default:
            printf("The system does not have an tlb replacement algorithm\n");
            shutdown_vm_system(&vmSystem);
            return;
            break;
    }
    
    // run the simulation
    simulate_vm_system(&vmSystem, logicAddressList, shouldDisplayPhysicalAddresses);
    
    printf("Check the results in output file: %s \n", OUTPUT_FILE_PATH);
    
    shutdown_vm_system(&vmSystem);
    
    printf("\n"); // one more newline
    return;
}

int
init_vm_system(struct System *sys, char *vmfilePath, char *outputPath, enum TlbAlgorithms alg) {
    int i;
    
    // open files
    sys->vmfile = fopen(vmfilePath, "r");
    sys->output = fopen(outputPath, "w");
    if (sys->vmfile == NULL || sys->output == NULL) {
        return 1;
    }
    
    // init mem
    for (i = 0; i < PHYSICAL_MEM_SIZE; i++) {
        sys->mem[i] = 0;
    }
    
    // init tlb
    for (i = 0; i < TLB_SIZE; i++) {
        sys->tlb[i].age = NULL_TLB_AGE;
        sys->tlb[i].page = NULL_PAGE;
        sys->tlb[i].frame = NULL_FRAME;
    }
    
    // init page table
    for (i = 0; i < PAGE_TABLE_SIZE; i++) {
        sys->ptable[i] = -1;
    }
    
    sys->algorithmChoice = alg; // set LRU algorithm
    
    return 0;
}

int
simulate_vm_system(struct System *sys, struct LinkedAddress *logicAddressList, char shouldDisplayPhysicalAddresses) {
    int err, total, value;
    int totalTlbHits, totalFaults, faults;
    float tlbHitRate, pageFaultRate;
    pagenum_t page;
    framenum_t frame;
    offset_t offset;
    addr_t virtaddr, physaddr;
    
    if (sys->output == NULL || sys->vmfile == NULL) {
        return 1; // need the files to be open
    }
    
    printf("\n");
    total = 0;
    totalTlbHits = 0;
    totalFaults = 0;
    while (logicAddressList != NULL) {
        // try to get convert the logical address to a valid page and offset
        if (logical_to_page_and_offset(logicAddressList->addr, &page, &offset) == 0) {
            // cascading error check
            // 1) check tlb
            // 2) check the page table
            // 3) try to swap a page into mem
            err = search_tlb(sys, page, &frame);
            if (err) {
                //printf("TLB MISS\n");
                err = search_page_table(sys, page, &frame);
                if (err) {
                    //printf("PAGE FAULT\n");
                    ++totalFaults;
                    err = page_fault_handle(sys, page, &frame);
                }
            }
            else {
                totalTlbHits++;
            }
            // if frame found then get the values
            if (err == 0) {
                virtaddr = get_virtual_address(page, offset);
                physaddr = get_physical_address(frame, offset);
                value = get_value_from_mem(sys, physaddr);
                if (shouldDisplayPhysicalAddresses)
                    printf("Virtual Address: %u; Physical Address: %u; Value: %d \n", virtaddr, physaddr, value);
                else
                    printf("Virtual Address: %u; Value: %d \n", virtaddr, value);
                fprintf(sys->output, "Virtual Address: %u; Physical Address: %u; Value: %d \n", virtaddr, physaddr, value);
            }
            // if no value could be found
            else {
                printf("!!! Unresolved logical address: %u\n", logicAddressList->addr);
            }
            
        }
        // if the logical address was an invalid page and offest
        else {
            // there was an error
            printf("!!! Unresolved logical address: %u\n", logicAddressList->addr);
        }
        ++total;
        logicAddressList = logicAddressList->next; // move forward in linked list
    }
    
    // display hit/fault rates
    tlbHitRate = (float)totalTlbHits / (float)total * 100.0;
    pageFaultRate = (float)totalFaults / (float)total * 100.0;
    printf("\n");
    printf("* Page fault rate: %.2f%%\n", pageFaultRate);
    fprintf(sys->output, "\n* Page fault rate: %.2f%%\n", pageFaultRate);
    printf("* TLB hit rate: %.2f%%\n", tlbHitRate);
    fprintf(sys->output, "* TLB hit rate: %.2f%%\n", tlbHitRate);
    printf("\n");
    
    return 0;
}

int
shutdown_vm_system(struct System *sys) {
    // close the systems output and vmfile files
    fclose(sys->output);
    fclose(sys->vmfile);
    return 0;
}

void
menu(enum TlbAlgorithms *algorithmChoice, char *shouldDisplayPhysicalAddresses) {
    char x, y;
    
    if (algorithmChoice == NULL || shouldDisplayPhysicalAddresses == NULL) {
        return;
    }
    
    puts("");
    puts("____Welcome to Group 8's VM Simulator Version 1.00___\n");
    printf("Number of logical pages: %d\n", PAGE_TABLE_SIZE);
    printf("Page size: %d bytes\n", PAGE_SIZE);
    printf("Page table size: %d\n", PAGE_TABLE_SIZE);
    printf("TLB size: %d \n", TLB_SIZE);
    printf("Number of physical frames: %d \n", FRAME_COUNT);
    printf("Physical memory size: %d bytes \n", PHYSICAL_MEM_SIZE);
    printf("\n");
    
    printf("Display Physical Address? [1: yes, 2: no] >> ");
    scanf("%c", &x);
    getchar();
    if (x == '1') {
        *shouldDisplayPhysicalAddresses = 1;
    }
    else if (x == '2') {
        *shouldDisplayPhysicalAddresses = 0;
    }
    else {
        *shouldDisplayPhysicalAddresses = -1;
    }
    //c = getchar();
    printf("Choose TLB Replacement Strategy [1: FIFO, 2: LRU] >> ");
    scanf("%c", &y);
    getchar();
    puts("");
    if (y == '1') {
        printf ("* You have chosen FIFO\n");
        *algorithmChoice = FIFO;
    }
    else if (y == '2') {
        printf("* You have chosen LRU\n");
        *algorithmChoice = LRU;
    }
    else {
        *algorithmChoice = ERROR;
    }
    
    fflush(stdin);
    
    return;
}

int
get_value_from_mem(struct System *sys, addr_t physaddr) {
    return sys->mem[physaddr];
}

struct LinkedAddress *
create_LinkedAddress(addr_t addr, struct LinkedAddress *next) {
    struct LinkedAddress *ptr = malloc(sizeof(struct LinkedAddress));
    if (ptr == NULL)
        return NULL;
    ptr->addr = addr;
    ptr->next = NULL;
    return ptr;
}

void
delete_LinkedAddress(struct LinkedAddress * list) {
    struct LinkedAddress *ptr;
    while (list != NULL) {
        ptr = list;
        list = list->next;
        free(ptr);
    }
    
    return;
}


struct LinkedAddress *
logical_addresses_from_file(char *path) {
    FILE * input;
    int value;
    int err;
    struct LinkedAddress *start;
    struct LinkedAddress *ptr;
    
    input = fopen(path, "r");
    if (input == NULL) {
        return NULL;
    }
    
    // first value in list
    err = fscanf(input, "%d", &value);
    if (err != 0 && err != EOF) {
        start = create_LinkedAddress(value, NULL);
        if (start == NULL) {
            fclose(input);
            return NULL;
        }
        
        ptr = start;
    }
    // get the rest of the list
    while (ptr != NULL) {
        err = fscanf(input, "%d", &value);
        if (err == 0) {
            delete_LinkedAddress(start);
            fclose(input);
            return NULL;
        }
        if (err != EOF) {
            ptr->next = create_LinkedAddress(value, NULL);
        }
        ptr = ptr->next;
    }
    
    fclose(input);
    
    return start;
}
