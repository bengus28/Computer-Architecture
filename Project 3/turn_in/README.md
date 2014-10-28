Computer-Architecture
=====================
- COMP 4300
- Project 3, Group 1
- Ben Gustafson, BRG0005
- John Riddell, JPR0014

Overview:
-------------

In this lab we migrated a single cycle MIPS machine to a pipe line MIPS machine.

With that comes a few complications.

Program
-------------

To run the program:

    make
    ./pipeSimulator.o

clean up:

    make clean  

Currently it is defaulted to lab3b to run

Results
-------------

###Lab3a:

Number of Instructions Executed (IC): 68
Number of Cycles Spent in Execution (C): 263
Number of NOPs: 196
Goodbye.

###Lab3b:

Please enter a word: hahahah
"The string is a palindrome."

Number of Instructions Executed (IC): 65
Number of Cycles Spent in Execution (C): 124
Number of NOPs: 60
Goodbye.


Please enter a word: notslsf
"The string is not a palindrome."

Number of Instructions Executed (IC): 47
Number of Cycles Spent in Execution (C): 95
Number of NOPs: 49
Goodbye.


###Lab3c:

Number of Instructions Executed (IC): 12
Number of Cycles Spent in Execution (C): 16
Number of NOPs: 5
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

A couple of things:

One, this migration wasn't all that hard, it was just extremely tedious. All of instruction logic had already been written, so it was
just the simple task of splitting it into the five different pipe line steps. This quickly turned out to be quite confusing.

One of the main issues we had was forwarding logic. Our Machine works fine without any data hazards and a LOT  of NOPs. Adding in the forwarding logic
took much more time than expected and is still not 100%. The programs we had to run for this project work, but that is about it.

The more we thought about being able to run just part of the pipe line, the more we realized we need all of it. We can't just run part of the pipe Line
and hope that the rest will be taken care of. There is no easy way to simply remove a segment of the pipe line.


###### This was hard ######
