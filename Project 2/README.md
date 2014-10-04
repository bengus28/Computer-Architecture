Computer-Architecture
=====================
- COMP 4300 
- Project 1, Group 1
- Ben Gustafson, BRG0005
- John Riddell, JPR0014

Overview: 
-------------

We have upgraded out single register accumulator to a functioning MIPS machine. Not all instructions of the MIPS ISA are implemented in this machine, but 12 are. The code this machine runs can be found in gprCode.txt.

The written code evaluates if an entered word is a palindrome. For this program spaces are seen as an ending character. For example:

	Please enter a word: palindrome
	The string is not a palindrome.

OR

	Please enter a word: theeht
	The string is a palindrome.


To run the machine, look below!

Happy simulating

War Eagle.



Simple MIPS Machine
-------------

#### Compile MIPS Machine ####

Open up the terminal navigate to this project and run:
	
	$ make

You should see this:

>g++ -c gprMem.cpp -o gprMem.o
>g++ -c gprReg.cpp -o gprReg.o
>g++ gprSim.cpp -o gprSimulator.o

Run: 

	$ ./gprSimulator.o

You should see this: (With original code)

>Please enter a word:

Enter any word and hit enter. If it is a palindrome the program will tell you!

Once you are done run:
	
	$ make clean

This will clean up the folder.



Notes
-------------

#### "..type..Code.txt" Files ####

If you take a look in these, they will have two main sections; ".text" and ".data".

>".text"

This is where the binary for the code is. Humans read it as addi $t1, $t1, 1 the computer reads it as "0x01323281". Weird huh?

>".data"

This is where the beginning information is provided for the computation.


#### Thoughts and Issues ####
************************************

###### Memory Address Length ######

> We had to change the default structure of our memory to fit in the MIPS architecture. We ran into several issues including finding bytes in memory segments, storing strings across several words of memory, and dealing with signed integers. We will need to upgrade the memory to reference individual bytes of memory for the next project instead of words of memory.

###### MIPS in general ######

> We ran into a bug where the 32bit integer notation actually flipped the way the characters were stored. Some special functions had to be built to retrieve strings. 

###### MIPS in general ######

> We were very surprised to find MIPS does not have a SUBI, they just ask the programers to add a negative number. It seems very useful to have a limited set of instructions available, although it does mean you go about certain things in a very weird way.

