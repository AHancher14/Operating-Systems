// Jacob Laney
// jpl0015@auburn.edu

#ifndef VM_H
#define VM_H

#include "config.h"
#include <stdio.h>

// starts the simulation
// parameter is path to file containing logical address list
void
vm_sim_start(char *inputFilePath);

// initialize a new System
int
init_vm_system(struct System *sys, char *vmfilePath, char *outputPath, enum TlbAlgorithms alg);

// simulate the virtual memory System
int
simulate_vm_system(struct System *sys, struct LinkedAddress *logicAddressList, char shouldDisplayPhysicalAddresses);

// shutdown the System, closing files references, etc.
int
shutdown_vm_system(struct System *sys);

// returns value at physical memory address 
int
get_value_from_mem(struct System *sys, addr_t physaddr);

// displays a menu to prompt user for TLB algorithm choice and whether
// or not physical addresses will display
void
menu(enum TlbAlgorithms *algorithmChoice, char *shouldDisplayPhysicalAddresses);

// functions for handling address linked list
struct LinkedAddress *
create_linked_address(addr_t addr, struct LinkedAddress *next);

void
delete_linked_address(struct LinkedAddress *list);

struct LinkedAddress *
logical_addresses_from_file(char *path);



#endif
