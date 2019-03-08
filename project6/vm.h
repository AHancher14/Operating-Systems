/*
 * COMP3500: Project 5
 * Sample header file for the virtual memory manager
 * Xiao Qin
 */
#ifndef VM_H
#define VM_H

#include "config.h"
#include <stdio.h>

void vm_sim_start(char *inputFilePath);
int create_system(struct Sys *system, char *vmFilePath, char *outputPath, enum tlbAlgorithms elg);
int simulate_system(struct Sys *system, struct LinkedAddress *logicAddressList, char displayyAddress);
int shutdown_system(struct Sys *system);
void menu(enum TlbAlgorithms *algorithmChoice, char *displayAddress);
struct LinkedAddress *
create_LinkedAddress(addr_t addr, struct LinkedAddress *next);
void delete_LinkedAddress(struct LinkedAddress *list);
struct LinkedAddress *logic_address_loader(char *path);


#endif

