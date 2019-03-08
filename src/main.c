// Jacob Laney
// jpl0015@auburn.edu

#include "vm.h"
#include <stdio.h>

int
main(int argc, char ** argv) {
    // make sure proper number of inputs
    // must have input file
    if (argc != 2) {
        printf("usage: <input file path> \n");
        return 1;
    }

    // start the simulation
    vm_sim_start(argv[1]);

    return 0;
}
