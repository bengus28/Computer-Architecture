/*******
	Stack Machine Simulation
	09/07/14
	Ben Gustafson
	COMP 4300
********/

#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include "stackMem.cpp"

using namespace std;

class Sim
{
public:
	Sim();											//Sets internal values, and initilizes Memory
	void run();										//This is where the fun is
private:
	int instruction_op();							//gives the op code of the internal current_instruction
	mem_addr instruction_memory_address();			//gives the memory address of the internal current_instruction
	void load_next_instruction();					//Loads next instruction based on pc
	mem_addr top_of_stack;							//Points to the current OPEN spot on top of stack
	mem_addr pc;									//Program counter, keeps up with where program is in memory
	instruction *current_instruction;				//Pointer to current instruction
	Memory *mem;
};

int main()
{
	Sim *sim = new Sim();
	sim->run();
	return 0;
}

Sim::Sim()
{
	top_of_stack = stack_top;
	pc = text_top;
	mem = new Memory();
}

void Sim::run()
{													//Simulates Stack Included functions are:
	bool more_instructions = true;					//Push, Pop, Add, Mult, end
	while(more_instructions)
	{
		load_next_instruction();
		
		switch(instruction_op())
		{
			case 1:	//PUSH
			{
				mem_addr *data = mem->read(instruction_memory_address());
				mem->write(top_of_stack, *data);
				top_of_stack++;
				break;
			}
			case 2:	//POP, top_of_stack points to NEXT empty space, so have to decrement it to find current value
			{
				top_of_stack--;
				mem_addr *data = mem->read(top_of_stack);
				cout << std::dec << *data << " was popped from the top of the sack." <<endl;
				break;
			}
			case 3: //ADD top_of_stack points to NEXT empty space, so have to decrement it to find current value
			{
				top_of_stack--;
				mem_addr *first_data = mem->read(top_of_stack);
				top_of_stack--;
				mem_addr *second_data = mem->read(top_of_stack);
				mem_addr result = *first_data + *second_data;
				mem->write(top_of_stack, result);
				top_of_stack++;
				break;
			}
			case 4:	//MULT top_of_stack points to NEXT empty space, so have to decrement it to find current value
			{
				top_of_stack--;
				mem_addr *first_data = mem->read(top_of_stack);
				top_of_stack--;
				mem_addr *second_data = mem->read(top_of_stack);
				mem_addr result = *first_data * *second_data;
				mem->write(top_of_stack, result);
				top_of_stack++;
				break;
			}
			case 5:	//END
			{
				more_instructions = false;
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
{															// removes the memory address from the back
	instruction op_value;
	op_value = *current_instruction;
	op_value = op_value >> 24;
	return op_value;
}

mem_addr Sim::instruction_memory_address()
{															//removes the bin and op code from the front
	instruction memory_address;
	memory_address = *current_instruction;
	memory_address = memory_address << 8;
	memory_address = memory_address >> 8;
	return memory_address;
}

void Sim::load_next_instruction()
{															//Loads next instruction and increments pc				
	current_instruction = mem->read(pc);
	pc++;
}


