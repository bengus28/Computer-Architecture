Computer-Architecture
=====================
- COMP 4300
- Group 1
- Ben Gustafson, BRG0005
- John Riddell, JPR0014

Overview:
-------------

This is the repository for our class projects. Feel free to poke around and shoot us an email if you wish!

War Eagle.

Project 1
-------------

We have two simulators, one is a stack Machine and one is an Accumulator.
They both read in code from their respective "..type..Code.txt" files.

Each simulator has two parts. Memory simulation and the processor / instruction simulation.

We are trying to evaluate this equation:

>A*X**2 + B*X + C
>> - x = 3
>> - A = 7
>> - B = 5
>> - C = 4
>
> Answer: 82

To run each machine, look at the readme file in project 1 folder!

Happy simulating

**"long time; /* know C? */"**

Project 2
-------------

We have upgraded our single register accumulator to a functioning MIPS machine. Not all instructions of the MIPS ISA are implemented in this machine, but 12 are.

The written code evaluates if an entered word is a palindrome or not. Our machine uses a hex interpreter to load code.

**"Unprecedented performance: Nothing ever ran this slow before."**

Project 3
-------------

We turned our fake MIPS machine into a functional 5-stage pipe line machine. Still all instructions of the MIPS ISA are not implemented but we added a few.

We added a forwarding unit. By default the machine produces some statistics about the program it just ran.

When in doubt: add NOPs.

**"Any sufficiently advanced bug is indistinguishable from a feature."**

Project 4
-------------

We took the five state pile line machine and added a scoreboard, floating point registers, and a floating point functional units.

This machine has:
-A variable fetch buffer
-Two-stage piple-lined integer ALU
-Two-stage piple-lined floating point adder
-Six-stage piple-lined floating point multiplier
-A memory functional unit (Load and Store)

Currently the default program is a dual-tone multifrequency (DTMF) calculator. It is how the frequency (read tone) of each number on the dial pad for a phone number is calculated.

**"Real Programmers confuse Halloween and Christmas, because dec 25 == oct 31."**
**"The best way to accelerate an IBM is at 9.8 m/s/s."**
**"recursion (re - cur' - zhun) n. 1. (see recursion)"**


WAR EAGLE
----------
