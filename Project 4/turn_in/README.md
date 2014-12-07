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

This machine has:

    * Two-stage piple-lined integer ALU
    * Two-stage piple-lined floating point adder
    * Six-stage piple-lined floating point multiplier
    * A memory functional unit (Load and Store)

Program
-------------

To run the program:

    make
    ./scoreboardSimulator.o

clean up:

    make clean  


Results
-------------
To run any of these programs change line 78 of scoMem.cpp, current default is lab4c.s.


**The simple program: (lab4a.s)**


Number of Instructions Executed (IC): 43
Number of Cycles Spent in Execution (C): 50
Number of NOPs: 31
Goodbye.


**The floating point program: (lab4b.s)**


Number of Instructions Executed (IC): 58
Number of Cycles Spent in Execution (C): 118
Number of NOPs: 15
Goodbye.

**DTMF loop: (lab4c.s)**



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

**"Real Programmers confuse Halloween and Christmas, because dec 25 == oct 31."**
