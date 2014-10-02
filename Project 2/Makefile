# Ben Gustafson
# Computer Architecture COMP 4200
# General Purpose Register Machine
CC=g++
CFLAGS=-c

all: gprSimulator.o

gprSimulator.o:  gprMem.o gprReg.o gprSim.cpp
	$(CC) gprSim.cpp -o gprSimulator.o

gprMem.o: gprMem.cpp
	$(CC) $(CFLAGS) gprMem.cpp -o gprMem.o
	
gprReg.o: gprReg.cpp
	$(CC) $(CFLAGS) gprReg.cpp -o gprReg.o

clean:
	rm gprSimulator.o gprMem.o gprReg.o
	