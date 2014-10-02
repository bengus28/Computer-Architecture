# Ben Gustafson
# Computer Architecture COMP 4200
# Accumulator Machine
CC=g++
CFLAGS=-c

all: gprSimulator.o

gprSimulator.o:  gprMem.o gprSim.cpp
	$(CC) gprSim.cpp -o gprSimulator.o

gprMem.o: gprMem.cpp
	$(CC) $(CFLAGS) gprMem.cpp -o gprMem.o

clean:
	rm gprSimulator.o gprMem.o 
	