/*******
	Accumulator Machine Simulation
	09/07/14
	Ben Gustafson
	COMP 4300
********/

#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include "accumMem.cpp"

using namespace std;

class Sim
{
public:
	Sim();										//Starts state of simulator and initializes memory
	void run();									//This is the simulation, where the majic happens
private:
	int instruction_op();						//Returns the op code of internal current instruction
	mem_addr instruction_memory_address();		//Returns the memory address of internal current instruction
	void load_next_instruction();				//Takes all the steps to load next instruction
	mem_addr internal_register;					//This is the accumulator its self
	mem_addr pc;								//Program counter
	instruction *current_instruction;			//Pointer to the current instruction
	Memory *mem;								//Memory object
};

int main()
{
	Sim *sim = new Sim();
	sim->run();
	return 0;
}

Sim::Sim()
{
	internal_register = 0;
	pc = text_top;
	mem = new Memory();
}

void Sim::run()
{
	bool more_instructions = true;
	while(more_instructions)
	{
		mem->print_memory();
		load_next_instruction();
		switch(instruction_op())
		{
			case 1:	//LOAD
			{
				mem_addr *data = mem->read(instruction_memory_address());
				internal_register = *data;
				break;
			}
			case 2:	//STORE
			{
				mem->write(instruction_memory_address(),internal_register);
				break;
			}
			case 3: //ADD
			{
				mem_addr *data = mem->read(instruction_memory_address());
				internal_register = internal_register + *data;
				break;
			}
			case 4:	//MULT
			{
				mem_addr *data = mem->read(instruction_memory_address());
				internal_register = internal_register * *data;
				break;
			}
			case 5:	//END
			{
				more_instructions = false;
				cout << std::dec << internal_register << " is in the internal_register." <<endl;
				cout << "Goodbye: Program is ending." << endl;
				break;
			}
			default:
				cout << "Error: There was an error with the execution of loaded instructions." << endl;
				break;
		}
	}
}

int Sim::instruction_op()
{															//Removes the memory address from instruction
	instruction op_value;					
	op_value = *current_instruction;
	op_value = op_value >> 24;
	return op_value;
}

mem_addr Sim::instruction_memory_address()
{															//Removes the op code, resets to correct value
	instruction memory_address;
	memory_address = *current_instruction;
	memory_address = memory_address << 8;
	memory_address = memory_address >> 8;
	return memory_address;
}

void Sim::load_next_instruction()
{															//Reads next instruction and increments pc
	cout << "Current Istruction: " << std::dec<< current_instruction << endl;
	current_instruction = mem->read(pc);
	cout << "Current Istruction: " << std::dec<< current_instruction << endl;
	pc++;
}