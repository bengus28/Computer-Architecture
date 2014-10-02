/*******
	General Purpose Register Machine Simulation
	10/10/14
	Ben Gustafson
	COMP 4300
********/

#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include "gprMem.cpp"
#include "gprReg.cpp"

using namespace std;

class Sim
{
public:
	Sim();										//Starts state of simulator and initializes memory
	void run();									//This is the simulation, where the magic happens
private:
	int instruction_op();						//Returns the op code of internal current instruction
	mem_addr instruction_memory_address();		//Returns the memory address of internal current instruction (last 24 bits)
	mem_addr first_register();					//Returns bits 24 - 16
	mem_addr second_register();					//Returns bits 16-8
	mem_addr third_register();					//Returns bits 8-0
	mem_addr immediate_value();					//Returns bits 16-0
	void load_next_instruction();				//Takes all the steps to load next instruction
	mem_addr internal_register;					//This is the accumulator its self
	mem_addr pc;								//Program counter
	instruction *current_instruction;			//Pointer to the current instruction
	Memory *mem;								//Memory object
	Register_Bank *registers;					//CPU internal registers
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
	registers = new Register_Bank();
}

void Sim::run()
{
	load_next_instruction();
	
	cout << "a " << std::hex << *current_instruction << endl;
	cout << "1 " << std::hex << instruction_op() << endl;
	cout << "2 " << std::hex << instruction_memory_address() << endl;
	cout << "3 " << std::hex << first_register() << endl;
	cout << "4 " << std::hex << second_register() << endl;
	cout << "5 " << std::hex << third_register() << endl;
	cout << "6 " << std::hex << immediate_value() << endl;
	mem->print_memory();
		
	bool more_instructions = false;
	int total_instructions_executed = 0;
	int total_cycles_spent = 0;
	while(more_instructions)
	{
		load_next_instruction();
		switch(instruction_op())
		{
			case 1:	//LOAD
			{
				cout << "Error: LOAD Instruction not implemented." << endl;
				break;
			}
			case 2:	//STORE
			{
				cout << "Error: STORE Instruction not implemented." << endl;
				break;
			}
			case 3: //ADDI ADD IMMEDIATE 
			{
				total_instructions_executed += 1;
				total_cycles_spent += 6;
				
				break;
			}
			case 4: //B BRANCH
			{
				total_instructions_executed += 1;
				total_cycles_spent += 4;
				
				break;
			}
			case 5: //BEQZ BRACH IF EQUAL TO ZERO
			{
				total_instructions_executed += 1;
				total_cycles_spent += 5;
				
				break;
			}
			case 6: //BGE BRANCH IF GREATER OR EQUAL $t0,$t1,target,  $t0 >= $t1
			{
				total_instructions_executed += 1;
				total_cycles_spent += 5;
				
				break;
			}
			case 7: //BNE BRANCH IF NOT EQUAL  $t0,$t1,target, $t0 <> $t1
			{
				total_instructions_executed += 1;
				total_cycles_spent += 5;
				
				break;
			}
			case 8: //LA LOAD ADDRESS
			{
				total_instructions_executed += 1;
				total_cycles_spent += 5;
				
				break;
			}
			case 9: //LB LOAD BYTE
			{
				total_instructions_executed += 1;
				total_cycles_spent += 6;
				
				break;
			}
			case 10: //LI LOAD IMMEDIATE 
			{
				total_instructions_executed += 1;
				total_cycles_spent += 3;
				
				break;
			}
			case 11: //SUBI SUBTRACT IMMEDIATE
			{
				total_instructions_executed += 1;
				total_cycles_spent += 6;
				
				break;
			}
			case 12: //SYSCALL
			{
				total_instructions_executed += 1;
				total_cycles_spent += 8;
				
				break;
			}
			case 13://END
			{
				more_instructions = false;
				cout << "Ending:" << endl;
				cout << "Number of Instructions Executed (IC): " << total_instructions_executed << endl;
				cout << "Number of Cycles Spent in Execution (C):" << total_cycles_spent << endl;
				cout << "Speed-up:" << (8*total_instructions_executed) / total_cycles_spent << endl;
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
{															//Removes the memory address from instruction, bits 32-24
	instruction op_value;					
	op_value = *current_instruction;
	op_value = op_value >> 24;
	return op_value;
}

mem_addr Sim::instruction_memory_address()
{															//Removes the op code, resets to correct value, bits 24-0
	instruction memory_address;
	memory_address = *current_instruction;
	memory_address = memory_address << 8;
	memory_address = memory_address >> 8;
	return memory_address;
}

mem_addr Sim::first_register()									//Left most register slot in instruciton, Returns bits 24 - 16
{
	instruction memory_address;
	memory_address = *current_instruction;
	memory_address = memory_address << 8;
	memory_address = memory_address >> 24;
	return memory_address;
}

mem_addr Sim::second_register()									//Center register slot in instruction, Returns bits 16-8
{
	instruction memory_address;
	memory_address = *current_instruction;
	memory_address = memory_address << 16;
	memory_address = memory_address >> 24;
	return memory_address;
}

mem_addr Sim::third_register()									//Right most register slot in instruction, Returns bits 8-0
{
	instruction memory_address;
	memory_address = *current_instruction;
	memory_address = memory_address << 24;
	memory_address = memory_address >> 24;
	return memory_address;
}

mem_addr Sim::immediate_value()									//Gives value of immediate slot, Returns bits 16-0
{
	instruction memory_address;
	memory_address = *current_instruction;
	memory_address = memory_address << 16;
	memory_address = memory_address >> 16;
	return memory_address;
}
		
void Sim::load_next_instruction()
{															//Reads next instruction and increments pc
	current_instruction = mem->read(pc);
	pc++;
}