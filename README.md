Computer-Architecture
=====================
- COMP 4300 
- Project 1, Group 1
- Ben Gustafson, BRG0005
- John Riddell, JPR0014

Overview: 
-------------

We have two simulators, one is a stack Machine and one is an Accumulator.
They both read in code from their respective "..type..Code.txt" files. (look in the notes section for more) 

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
	
	$ make -f StackMakefile

You should see this:

>g++ -c stackMem.cpp -o stackMem.o <br/>
>g++ stackSim.cpp -o stackSimulator.o

Run: 

	$ ./stackSimulator.o

You should see this: (With original code)

>82 was popped from the top of the sack. <br/>
>Goodbye: Program is ending.

Once you are done run:
	
	$ make -f StackMakefile clean

This will clean up the folder.


Accumulator
-------------

#### Compile Accumulator ####

Open up the terminal navigate to this project and run:
	
	$ make -f AccumMakefile

You should see this:

>g++ -c accumMem.cpp -o accumMem.o <br/>
>g++ accumSim.cpp -o accumSimulator.o

Run:

	$ ./accumSimulator.o

You should see this: (With original code)

>82 is in the internal register. <br/>
>Goodbye: Program is ending.

Once you are done run:

	$ make -f AccumMakefile clean

This will clean up the folder.

Notes
-------------

#### "..type..Code.txt" Files ####

If you take a look in these, they will have two main sections; ".text" and ".data".

>".text"

This is where the binary for the code is. Humans read it as Push x, the computer reads it as "0x01200000". Weird huh?

>".data"

This is where the beginning information is provided for the computation.


#### Thoughts and Issues ####
************************************

###### Memory Address Length ######

> Dealing with the memory address length and then having an instruction length was a particularly hard/confusing subject. We ended up going with the instruction being 32 bits with an op code having 8 bits on the left hand side. This meant that our memory address is only 24 bits. Unfortunately defining a 40 bit instruction didn't seem feasible.


###### Instruction Encoding ######

> With the requirment of supporting 140 operations, we were forced to have an 8 bit op code. <br/>
> 2^7 = 128 <br/>
> 2^8 = 265 <br/>
> At first we were concerned this was going to be the largest hurdle to overcome. It ended up being taken care of with a few well placed bit shifts. In total only about 15 lines of code took care of all the operations we needed to decode out encoding. 

