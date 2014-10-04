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
	int8_t signed_immediate(mem_addr m_addr);   //return a sgined value;
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

	bool more_instructions = true;
	int total_instructions_executed = 0;
	int total_cycles_spent = 0;
	while(more_instructions)
	{
		load_next_instruction();
		switch(instruction_op())
		{
			case 1: //ADDI ADD IMMEDIATE 
			{
				int8_t immediate = signed_immediate(third_register());
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
			case 2: //B BRANCH
			{
				int8_t label_offset =0;
				label_offset = signed_immediate(third_register());
				pc += label_offset;
				
				total_instructions_executed += 1;
				total_cycles_spent += 4;
				break;
			}
			case 3: //BEQZ BRACH IF EQUAL TO ZERO
			{
				int8_t label_offset =0;
				if (registers->read(first_register()) == 0)
				{
					label_offset = signed_immediate(third_register());
					pc += label_offset;
				}
				
				total_instructions_executed += 1;
				total_cycles_spent += 5;
				break;
			}
			case 4: //BGE BRANCH IF GREATER OR EQUAL $t0,$t1,target,  $t0 >= $t1
			{
				int8_t label_offset =0;
				if ( registers->read(first_register())  >=  registers->read(second_register()))
				{
					label_offset = signed_immediate(third_register());
					pc += label_offset;
				}
				
				total_instructions_executed += 1;
				total_cycles_spent += 5;
				break;
			}
			case 5: //BNE BRANCH IF NOT EQUAL  $t0,$t1,target, $t0 <> $t1
			{
				int8_t label_offset =0;
				if ( registers->read(first_register())  !=  registers->read(second_register()))
				{
					label_offset = signed_immediate(third_register());
					pc += label_offset;
				}
				
				total_instructions_executed += 1;
				total_cycles_spent += 5;
				break;
			}
			case 6: //LA LOAD ADDRESS
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
			case 7: //LB LOAD BYTE
			{
				mem_addr address_value = registers->read(second_register()); 		//number of bytes
				int8_t immediate = signed_immediate(third_register());
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
			case 8: //LI LOAD IMMEDIATE 
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
			case 9: //SUBI SUBTRACT IMMEDIATE
			{
				int8_t immediate = signed_immediate(third_register());
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
			case 10: //SYSCALL
			{
				total_instructions_executed += 1;
				total_cycles_spent += 8;
				
				switch(registers->read(3))
				{
					case 4:	//Print String 
					{
						cout << mem->read_string(registers->read(1)) << endl;
						break;
					}
					case 8:	//Read String In
					{
						char palin[1024];
						string incoming_palin;
						
						int length=1024;
					    for (int i=0;i<1024;i++)		// clear mem
					    {
					            palin[i]=0;
					    }
						cout << "Please enter a word: ";
						getline(cin, incoming_palin);
						incoming_palin.copy(palin,1024,0);
						int len=strlen(palin);
						palin[len] = '\0';
						//cin >> *palin >> "\0";
						mem->load_string(registers->read(1),palin);
						break;
					}
					case 10:// End Program
					{
						more_instructions = false;
						cout << endl;
						cout << "Number of Instructions Executed (IC): " << std::dec<< total_instructions_executed << endl;
						cout << "Number of Cycles Spent in Execution (C): " <<std::dec<<  total_cycles_spent << endl;
						printf("Speed-up: %3.2F \n",(8.0*total_instructions_executed) / total_cycles_spent );
						cout << "Goodbye." << endl;
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
			case 11:	//LOAD
			{
				cout << "Error: LOAD Instruction not implemented." << endl;
				break;
			}
			case 12:	//STORE
			{
				cout << "Error: STORE Instruction not implemented." << endl;
				break;
			}
			default:
				more_instructions = false;
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

int8_t Sim::signed_immediate(mem_addr m_addr)				//Helper method for handling immediates and signing them correctly
{
	mem_addr sign_bit = m_addr, value = m_addr;
	sign_bit = sign_bit >> 7;
	value = value <<26;
	value = value >>26;
	int return_value = 0;
	if (sign_bit == 1)
	{
		return_value = 0 -value;
		return return_value;
	}
	else
	{
		return value;
	}
	return 0;
}
		
void Sim::load_next_instruction()
{															//Reads next instruction and increments pc
	current_instruction = mem->read(pc);
	pc++;
}