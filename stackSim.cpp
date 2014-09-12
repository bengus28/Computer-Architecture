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
	Sim();
	void run();
private:
	int instruction_op();
	mem_addr instruction_memory_address();
	void load_next_instruction();
	mem_addr top_of_stack;
	mem_addr pc;
	instruction *current_instruction;
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
{
	bool more_instructions = true;
	while(more_instructions)
	{
		load_next_instruction();
		//cout << instruction_op() << " mem " << instruction_memory_address() << endl;
		
		switch(instruction_op())
		{
			case 1:	//PUSH
			{
				mem_addr *data = mem->read(instruction_memory_address());
				mem->write(top_of_stack, *data);
				top_of_stack++;
				break;
			}
			case 2:	//POP
			{
				top_of_stack--;
				mem_addr *data = mem->read(top_of_stack);
				cout << std::dec << *data << " was poped from the top of the sack." <<endl;
				break;
			}
			case 3: //ADD
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
			case 4:	//MULT
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
		//mem->print_memory();
		//more_instructions = false;
	}
}

int Sim::instruction_op()
{	
	instruction op_value;
	op_value = *current_instruction;
	//cout << "--OP--" << endl;
	//cout << std::hex << op_value << endl;
	op_value = op_value >> 24;
	//cout << std::hex << op_value << endl;
	return op_value;
}

mem_addr Sim::instruction_memory_address()
{
	instruction memory_address;
	memory_address = *current_instruction;
	//cout << "--Memory Address--" << endl;
	//cout << std::hex << memory_address << endl;
	memory_address = memory_address << 8;
	//cout << std::hex << memory_address << endl;
	memory_address = memory_address >> 8;
	return memory_address;
}

void Sim::load_next_instruction()
{
	current_instruction = mem->read(pc);
	//cout << "Loaded instruction" << endl;
	//cout << std::hex << (int) *current_instruction << endl;
	pc++;
}


