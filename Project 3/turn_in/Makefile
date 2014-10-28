# Ben Gustafson
# Computer Architecture COMP 4200
# General Purpose Register Machine
CC=g++
CFLAGS=-c

all: pipeLineSimulator.o

pipeLineSimulator.o:  pipeMem.o pipeReg.o pipeSim.cpp
	$(CC) -std=c++0x pipeSim.cpp -o pipeSimulator.o

pipeMem.o: pipeLineMem.cpp
	$(CC) $(CFLAGS) -std=c++0x pipeLineMem.cpp -o pipeMem.o

pipeReg.o: pipeLineReg.cpp
	$(CC) $(CFLAGS) -std=c++0x pipeLineReg.cpp -o pipeReg.o

clean:
	rm pipeSimulator.o pipeMem.o pipeReg.o
