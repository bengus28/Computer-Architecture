Computer-Architecture
=====================
- COMP 4300
- Project 4, Group 1
- Ben Gustafson, BRG0005
- John Riddell, JPR0014

Overview:
-------------

In this lab we migrated a pipe line MIPS machine to a pipe line with a Scoreboard and floating point memory and computational units.

With that comes a few complications.

Program
-------------

To run the program:

    make
    ./scoreboardSimulator.o

clean up:

    make clean  


Results
-------------



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

#A couple of things:


###### This was hard ######
