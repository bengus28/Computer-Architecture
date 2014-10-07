# Ben Gustafson
# Computer Architecture COMP 4200
# General Purpose Register Machine
CC=g++
CFLAGS=-c -std=gnu++0x

all: gprSimulator.o

gprSimulator.o:  gprMem.o gprReg.o gprSim.cpp
	$(CC) gprSim.cpp -o gprSimulator.o

gprMem.o: gprMem.cpp
	$(CC) $(CFLAGS) -std=gnu++0x gprMem.cpp -o gprMem.o
	
gprReg.o: gprReg.cpp
	$(CC) $(CFLAGS) -std=gnu++0x gprReg.cpp -o gprReg.o

clean:
	rm gprSimulator.o gprMem.o gprReg.o
	