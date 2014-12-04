# Ben Gustafson
# Computer Architecture COMP 4200
# Pipeline & Score Board Machine
CC=g++
CFLAGS=-c

all: scoreboardSimulator.o

scoreboardSimulator.o:  scoMem.o scoReg.o scoSim.cpp
	$(CC) -std=c++0x scoSim.cpp -o scoSimulator.o

scoMem.o: scoLineMem.cpp
	$(CC) $(CFLAGS) -std=c++0x scoLineMem.cpp -o scoMem.o

scoReg.o: scoLineReg.cpp
	$(CC) $(CFLAGS) -std=c++0x scoLineReg.cpp -o scoReg.o

clean:
	rm scoreboardSimulator.o scoMem.o scoReg.o
