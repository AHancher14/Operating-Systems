#
# Jacob Laney
#
# reference: http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
#

cc = gcc
objects = main.o vm.o paging.o
INCLUDE= -I../include

all: $(objects)
		$(cc) $(objects) -o ../vm_sim $(INCLUDE)

main.o: main.c vm.o
	$(cc) -c main.c -o main.o $(INCLUDE)
vm.o: vm.c paging.o
	$(cc) -c vm.c -o vm.o $(INCLUDE)
paging.o:
	$(cc) -c paging.c -o paging.o $(INCLUDE)
clean:
	rm *.o
