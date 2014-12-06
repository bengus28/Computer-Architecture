# Ben Gustafson
# Computer Architecture COMP 4200
# Pipeline & Score Board Machine
CC=g++
CFLAGS=-c

all: scoreboardSimulator.o

scoreboardSimulator.o:  scoMem.o scoReg.o scoRegFloat.o scoScoreboard.o scoSim.cpp
	$(CC) -std=c++0x scoSim.cpp -o scoreboardSimulator.o

scoMem.o: scoMem.cpp
	$(CC) $(CFLAGS) -std=c++0x scoMem.cpp -o scoMem.o

scoReg.o: scoReg.cpp
	$(CC) $(CFLAGS) -std=c++0x scoReg.cpp -o scoReg.o

scoRegFloat.o: scoRegFloat.cpp
	$(CC) $(CFLAGS) -std=c++0x scoRegFloat.cpp -o scoRegFloat.o

scoScoreboard.o: scoScoreboard.cpp
	$(CC) $(CFLAGS) -std=c++0x scoScoreboard.cpp -o scoScoreboard.o

clean:
	rm scoreboardSimulator.o scoMem.o scoReg.o scoRegFloat.o scoScoreboard.o
