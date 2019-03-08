//
// Alan Hancher.
// display.h
//

#ifndef DISPLAY_H
#define DISPLAY_H

#include "system.h"
#include <stdio.h>

void display_begin(char *inputFilePath);
int display_start(struct Sys *system, char *vmfilePath, char *outputPath, enum TlbAlgorithms alg );
int run_display(struct Sys *system, struct LinkedAddress *logicAddressList, char displayAddress);
int shutdown(struct Sys *system);

int get_value_from_mem(struct Sys *system, addr_t pAddr);
struct LinkedAddress *
create_LinkedAddress(addr_t  addr, struct LinkedAddress *next);
void delete_LinkedAddress(struct LinkedAddress * list);
struct LinkedAddress *
logic_address_reader(char* file);



#endif //DISPLAY_H
