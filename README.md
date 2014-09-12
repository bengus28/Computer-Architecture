Computer-Architecture
=====================
- COMP 4300 
- Project 1, Group 1
- Ben Gustafson, BRG0005
- John Riddell, JPR0014

Overview: 
-------------

We have two simulators, one is a stack Machine and one is an Accumulator.
They both read in code from their respective "..type..Code.txt" files. (look in the notes seciton for more) 

Each simulator has two parts. Memory simulation and the processor / instruction simulation.

We are trying to evaluate this equation: 

>A*X**2 + B*X + C
>> - x = 3
>> - A = 7
>> - B = 5
>> - C = 4
>
> Answer: 82

To run each machine, look below!

Happy simulating

War Eagle.



Stack Machine
-------------

#### Compile Stack Machine ####

Open up the terminal navigate to this project and run:
	
	"$ make -f StackMakefile".

You should see this:

>g++ -c stackMem.cpp -o stackMem.o <br/>
>g++ stackSim.cpp -o stackSimulator.o

Run 
	"$ ./stackSimulator.o"

You should see this: (With original code)

>82 was poped from the top of the sack. <br/>
>Goodbye: Program is ending.

Once you are done run:
	
	"$ make -f StackMakefile clean"

This will clean up the folder.


Accumulator
-------------

#### Compile Accumulator ####

Open up the terminal navigate to this project and run:
	
	"$ make -f AccumMakefile".

You should see this:

>g++ -c accumMem.cpp -o accumMem.o <br/>
>g++ accumSim.cpp -o accumSimulator.o

Run 
	"$ ./accumSimulator.o"

You should see this: (With original code)

>82 is in the internal register. <br/>
>Goodbye: Program is ending.

Once you are done run:
	"$ make -f AccumMakefile clean"

This will clean up the folder.

Noets
-------------

###### "..type..Code.txt" Files #####

If you take a look in these, they will have two main sections; ".text" and ".data".

>".text"

This is where the biniary for the code is. Humans read it as Push x, the computer reads it as "0x01200000". Weird huh?

>".data"

This is where the begining information is provided for the computation.
