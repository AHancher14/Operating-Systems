CC=gcc
CFLAGS=-I.
DEPS = vm.h config.h tlb.h
OBJ = main.o tlb.o vm.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $, $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) 