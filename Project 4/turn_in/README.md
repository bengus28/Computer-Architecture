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

==== Floating Point Registers ===
3:  1.64                (kx)
4:  0.612678            (X2)
5:  0.0530322           (X1)
6:  0.8                 (ky)
7:  0.263799            (Y2)
8:  -0.782143           (Y1)
9:  0.0530322           (X0)
10:  -0.782143          (Y0)
11:  0.0530322          (Store X0)
12:  -0.782143          (Store Y0)
=================================
Number of Instructions Executed (IC): 553
Number of Cycles Spent in Execution (C): 1693
Number of NOPs: 76
Goodbye.

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

I was thrown off by having to add some instructions to make the DTMF code run. I figured that if we implemented everything in the document we would get through it.

To be honest this wasn't that hard. I felt it was much easier than trying to implement forwarding in the 5 stage pipe line last time. The nice thing was everything
you needed was in the scoreboard so all you had to do was look at it to figure out what to do. Where with forwarding you had to cover ever case over and over again
and the way you wrote one forwarding section forced you to write others in round about ways.

It was much easier to add in the extra instructions with this machine than the previous ones. There was about 8 places you had to change and it took care of the rest
which was fantastic.

I loved this project / class. I learn how to implement a simple machine to a highly sophisticated one. I now understand how it all works, which is so so cool.

#Issues

Reading in the floating point numbers was an absolute pain. This is mainly due to the fact we compile the instructions down to the bite level and store it in hex
before the machine ever sees it. So we ended up reading in the numbers * 100 and then dividing them once inside the machine. This would not suffice for a general application,
but it was a quick hack we needed to do.

One side effect of the way we search through the score board (mainly the functional unit status) we did not differentiate between floating point registers and integer registers.
This means if they are named the same thing - that is to say they have the same index, then they are seen as the same. Which makes some code block when it doesn't have to.
For now we just use different register names to make it all run correctly. However, this is VERY limiting. We noticed this very late in the game and didn't spend the time to fix it.

Please read the collaboration document.


**"Real Programmers confuse Halloween and Christmas, because dec 25 == oct 31."**
