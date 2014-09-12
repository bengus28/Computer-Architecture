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
	load_next_instruction();
	cout << instruction_op() << " mem " << instruction_memory_address() << endl;
	load_next_instruction();
	cout << instruction_op() << " mem " << instruction_memory_address() << endl;
	load_next_instruction();
	cout << instruction_op() << " mem " << instruction_memory_address() << endl;
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
	cout << "Loaded instruction" << endl;
	cout << std::hex << (int) *current_instruction << endl;
	pc++;
}


