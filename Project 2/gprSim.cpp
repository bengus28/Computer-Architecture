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
	pc = text_top;
	mem = new Memory();
	registers = new Register_Bank();
}

void Sim::run()
{
//	char palin[50] = "The string is a palindrome.\n";
//	char palin2[70] = "The string is NOT a palindrome.\n";
//	mem->load_string(0x00200000,palin);
//	cout << "Str read:" << mem->read_string(0x00200000) << endl;
//	mem->load_string(0x00300000,palin2);
//	cout << "Str read:" << mem->read_string(0x00300000) << endl;

	bool more_instructions = true;
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
				int8_t immediate = third_register();
				uint32_t register_value = registers->read(second_register());
				bool success = registers->write(first_register(), immediate + register_value);
				if (false == success)
				{
					cout << "Error: Adding value to register: "<< std::dec << second_register() << endl;
				}
				
				total_instructions_executed += 1;
				total_cycles_spent += 6;
				break;
			}
			case 4: //B BRANCH
			{
				int8_t label_offset =0;
				label_offset = third_register();
				pc += label_offset;
				
				total_instructions_executed += 1;
				total_cycles_spent += 4;
				break;
			}
			case 5: //BEQZ BRACH IF EQUAL TO ZERO
			{
				int8_t label_offset =0;
				if (second_register() == 0)
				{
					label_offset = third_register();
					pc += label_offset;
				}
				
				total_instructions_executed += 1;
				total_cycles_spent += 5;
				break;
			}
			case 6: //BGE BRANCH IF GREATER OR EQUAL $t0,$t1,target,  $t0 >= $t1
			{
				int8_t label_offset =0;
				if ( registers->read(first_register())  >=  registers->read(second_register()))
				{
					label_offset = third_register();
					pc += label_offset;
				}
				
				total_instructions_executed += 1;
				total_cycles_spent += 5;
				break;
			}
			case 7: //BNE BRANCH IF NOT EQUAL  $t0,$t1,target, $t0 <> $t1
			{
				int8_t label_offset =0;
				if ( registers->read(first_register())  !=  registers->read(second_register()))
				{
					label_offset = third_register();
					pc += label_offset;
				}
				
				total_instructions_executed += 1;
				total_cycles_spent += 5;
				break;
			}
			case 8: //LA LOAD ADDRESS
			{
				bool success = registers->write(first_register(),immediate_value());
				if (false == success)
				{
					cout << "Error: Loading Address to register: "<< std::dec << first_register() << endl;
				}
				
				total_instructions_executed += 1;
				total_cycles_spent += 5;
				break;
			}
			case 9: //LB LOAD BYTE
			{
				mem_addr address_value = registers->read(second_register()); 		//number of bytes
				int8_t immediate = third_register();
				address_value += immediate;
				bool success = registers->write(first_register(),mem->read_byte(address_value, address_value%4) );
				if (false == success)
				{
					cout << "Error: Loading Byte into register: "<< std::dec << first_register() << endl;
				}
				
				total_instructions_executed += 1;
				total_cycles_spent += 6;
				break;
			}
			case 10: //LI LOAD IMMEDIATE 
			{
				bool success = registers->write(first_register(), second_register());
				if (false == success)
				{
					cout << "Error: Loading Immediate value to register: "<< std::dec << first_register() << endl;
				}
				
				total_instructions_executed += 1;
				total_cycles_spent += 3;
				break;
			}
			case 11: //SUBI SUBTRACT IMMEDIATE
			{
				int8_t immediate = third_register();
				uint32_t register_value = registers->read(second_register());
				bool success = registers->write(first_register(), register_value - immediate);
				if (false == success)
				{
					cout << "Error: Adding value to register: "<< std::dec << second_register() << endl;
				}
				total_instructions_executed += 1;
				total_cycles_spent += 6;
				break;
			}
			case 12: //SYSCALL
			{
				total_instructions_executed += 1;
				total_cycles_spent += 8;
				
				switch(registers->read(20))
				{
					case 4:	//Print String 
					{
						cout << "Str read:" << mem->read_string(registers->read(0)) << endl;
						break;
					}
					case 8:	//Read String In
					{
						char palin[1024];
						string incoming_palin;
						cout << "Please enter a word: ";
						getline(cin, incoming_palin);
						incoming_palin.copy(palin,1024,0);
						//char palin[1024];
						//cin >> *palin >> "\0";
						mem->load_string(registers->read(0),palin);
						break;
					}
					case 10:// End Program
					{
						more_instructions = false;
						cout << " " <<endl;
						cout << "Ending:" << endl;
						cout << "Number of Instructions Executed (IC): " << total_instructions_executed << endl;
						cout << "Number of Cycles Spent in Execution (C):" << total_cycles_spent << endl;
						cout << "Speed-up:" << (8*total_instructions_executed) / total_cycles_spent << endl;
						cout << "Goodbye: Program is ending." << endl;
						break;
					}
					default:
					{
						cout << "Error: There was an error with the execution of SYSCALL." << endl;
						cout << "PC: " << std::hex << pc << endl;
						cout << "Current Istruction: " <<std::hex << current_instruction << endl;
						more_instructions = false;
						break;
					}
				}
				break;
			}
			default:
				cout << "Error: There was an error with the execution of loaded instruction." << endl;
				cout << "PC: " << std::hex <<pc << endl;
				cout << "Current Istruction: " << std::hex<< current_instruction << endl;
				more_instructions = false;
				break;
		}
		registers->print_memory();
		mem->print_memory();
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
	cout << "Current Istruction: " << std::hex<< current_instruction << endl;
	current_instruction = mem->read(pc);
	cout << "Current Istruction: " << std::hex<< current_instruction << endl;
	pc++;
}