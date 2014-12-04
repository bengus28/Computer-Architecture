/*******
	General Purpose Register Machine Simulation - Pipe Line with a Scoreboard
	12/6/14
	Ben Gustafson
	COMP 4300
********/

#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include "scoMem.cpp"
#include "scoReg.cpp"
#include "scoRegFloat.cpp"

using namespace std;

// Press any key to continue
void PressAnyKey(void);

struct If_Id
{
	instruction *instruction_;
};

struct Id_Ex
{
	instruction op;
	instruction instruct;
	int8_t immediate;
	mem_addr first_reg;
	mem_addr second_reg;
	mem_addr third_reg;
	instruction op_A;
	instruction op_B;
};

struct Ex_Mm
{
	instruction op;
	instruction instruct;
	int8_t immediate;
	mem_addr first_reg;
	mem_addr second_reg;
	mem_addr third_reg;
	instruction op_A;
	instruction op_B;
	instruction alu_results;
};

struct Mm_Wb
{
	instruction op;
	instruction instruct;
	int8_t immediate;
	mem_addr first_reg;
	mem_addr second_reg;
	mem_addr third_reg;
	instruction op_A;
	instruction op_B;
	instruction alu_results;
	instruction mem_read_results;
};

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
	Floating_Point_Register_Bank *floating_registers;
	int8_t signed_immediate(mem_addr m_addr);   //return a sgined value;
	void print_buffers();
	void IF_();
	void ID();
	void EX();
	void MM();
	void WB();
	If_Id new_If_Id;
	If_Id old_If_Id;
	Id_Ex new_Id_Ex;
	Id_Ex old_Id_Ex;
	Ex_Mm new_Ex_Mm;
	Ex_Mm old_Ex_Mm;
	Mm_Wb new_Mm_Wb;
	Mm_Wb old_Mm_Wb;
	bool more_instructions;
	int total_instructions_executed;
	int total_cycles_spent;
	int total_nops;
};

void PressAnyKey(void)
{
	cout << "Press any key to continue" << endl;
	cin.ignore().get(); //Pause Command for Linux Terminal
}


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
	floating_registers = new Floating_Point_Register_Bank();
	more_instructions = true;
	total_instructions_executed = 0;
	total_cycles_spent = 0;
	total_nops = 0;
	new_If_Id.instruction_ =0;
	old_If_Id.instruction_ =0;
	Id_Ex new_Id_Ex = {0,0,0,0,0,0,0,0};
	Id_Ex old_Id_Ex = {0,0,0,0,0,0,0,0};
	Ex_Mm old_Ex_Mm = {0,0,0,0,0,0,0,0,0};
	Ex_Mm new_Ex_Mm = {0,0,0,0,0,0,0,0,0};
	Mm_Wb new_Mm_Wb = {0,0,0,0,0,0,0,0,0,0};
	Mm_Wb old_Mm_Wb = {0,0,0,0,0,0,0,0,0,0};
}

void Sim::run()
{
	while(more_instructions)
	{
		//PressAnyKey();
		//mem->print_memory();
		//print_buffers();
		old_If_Id = new_If_Id;
		IF_();
		old_Id_Ex = new_Id_Ex;
		ID();
		old_Ex_Mm = new_Ex_Mm;
		EX();
		old_Mm_Wb = new_Mm_Wb;
		MM();
		WB();
		total_cycles_spent++;
	}
}

/*
Used Paramaters:
Effected buffers:
*/
void Sim::IF_()
{
	new_If_Id.instruction_ = mem->read(pc);
	pc++;
}
/*
Used Paramaters:
Effected buffers:
*/
void Sim::ID()
{

	if(old_If_Id.instruction_ != 0 ) //nop
	{
		//cout << std::hex << *(old_If_Id.instruction_) << endl;
		current_instruction = old_If_Id.instruction_;
		new_Id_Ex.op = instruction_op();
		new_Id_Ex.instruct = *current_instruction;
	}
	else
	{
		new_Id_Ex.op = 0;
		new_Id_Ex.instruct = 0;
	}
	switch(new_Id_Ex.op)
	{
		case 0:
		{
			//Nop
			new_Id_Ex.immediate = 0 ;
			new_Id_Ex.first_reg = 0 ;
			new_Id_Ex.second_reg = 0 ;
			new_Id_Ex.third_reg = 0 ;
			new_Id_Ex.op_A = 0 ;
			new_Id_Ex.op_B = 0 ;
			break;
		}
		case 1: //ADDI ADD IMMEDIATE
		{
			new_Id_Ex.immediate = signed_immediate(third_register());
			new_Id_Ex.second_reg = second_register();
			new_Id_Ex.first_reg = first_register();
			new_Id_Ex.op_A = registers->read(second_register());
			break;
		}
		case 2: //B BRANCH
		{
			int8_t label_offset =0;
			label_offset = signed_immediate(third_register());
			pc--;
			pc += label_offset;
			break;
		}
		case 3: //BEQZ BRACH IF EQUAL TO ZERO
		{
			int8_t label_offset = 0;
			instruction first_regist_value = registers->read(first_register());
			if (first_regist_value  == 0)
			{
				label_offset = signed_immediate(third_register());
				pc--;
				pc += label_offset;
			}
			break;
		}
		case 4: //BGE BRANCH IF GREATER OR EQUAL $t0,$t1,target,  $t0 >= $t1
		{
			instruction first_regist_value = registers->read(first_register());
			instruction second_regist_value = registers->read(second_register());
			if(first_register() == old_Ex_Mm.first_reg)
			{
				first_regist_value = old_Ex_Mm.alu_results;
			}
			if(first_register() == old_Mm_Wb.first_reg)
			{
				first_regist_value = old_Mm_Wb.alu_results;
			}
			if(second_register() == old_Ex_Mm.first_reg)
			{
				second_regist_value = old_Ex_Mm.alu_results;
			}
			if(second_register() == new_Ex_Mm.first_reg)
			{
				second_regist_value = new_Ex_Mm.alu_results;
			}
			if(second_register() == old_Mm_Wb.first_reg)
			{
				second_regist_value = old_Mm_Wb.alu_results;
			}
			int8_t label_offset = 0 ;
			if ( first_regist_value >= second_regist_value )
			{
				label_offset = signed_immediate(third_register());
				pc--;
				pc += label_offset;
			}
			break;
		}
		case 5: //BNE BRANCH IF NOT EQUAL  $t0,$t1,target, $t0 <> $t1
		{
			instruction first_regist_value = registers->read(first_register());
			instruction second_regist_value = registers->read(second_register());
			if(second_register() == new_Ex_Mm.first_reg)
			{
				second_regist_value = mem->read_byte(new_Ex_Mm.alu_results, new_Ex_Mm.alu_results%4);
			}
			int8_t label_offset =0;
			if ( first_regist_value  !=  second_regist_value)
			{
				label_offset = signed_immediate(third_register());
				pc--;
				pc += label_offset;
			}
			break;
		}
		case 6: //LA LOAD ADDRESS
		{
			new_Id_Ex.first_reg = first_register();
			new_Id_Ex.op_A = immediate_value();
			break;
		}
		case 7: //LB LOAD BYTE
		{
			new_Id_Ex.first_reg = first_register();
			new_Id_Ex.second_reg = second_register();
		 	new_Id_Ex.immediate = signed_immediate(third_register());
			new_Id_Ex.op_A = registers->read(second_register());//number of bytes
			break;
		}
		case 8: //LI LOAD IMMEDIATE
		{
			new_Id_Ex.first_reg = first_register();
			new_Id_Ex.second_reg = second_register();
			new_Id_Ex.op_A = second_register();
			break;
		}
		case 9: //SUBI SUBTRACT IMMEDIATE
		{
			new_Id_Ex.immediate = signed_immediate(third_register());
			new_Id_Ex.op_A = registers->read(second_register());
			new_Id_Ex.second_reg = second_register();
			old_Id_Ex.first_reg = first_register();
			break;
		}
		case 10: //SYSCALL
		{
			new_Id_Ex.second_reg = 3;
			new_Id_Ex.op_B = registers->read(3);
			switch(new_Id_Ex.op_B)
			{
				case 1:
				{
					new_Id_Ex.first_reg = 1;
					new_Id_Ex.op_A = registers->read(1);
					break;
				}
				case 4:	//Print String
				{
					new_Id_Ex.first_reg = 1;
					new_Id_Ex.op_A = registers->read(1);
					break;
				}
				case 8:	//Read String In
				{
					new_Id_Ex.first_reg = 1;
					new_Id_Ex.op_A = registers->read(1);
					break;
				}
				case 10:// End Program
				{
					break;
				}
				default:
				{
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
			cout << "Error: There was an error with the Decoding Stage." << endl;
			cout << "PC: " << std::hex << pc << endl;
			cout << "Current Istruction: " <<std::hex << *current_instruction << endl;
			//more_instructions = false;
			break;
	}
}
/*
Used Paramaters:
Effected buffers:
*/
void Sim::EX()
{
	//run the instruciton
	new_Ex_Mm.op = old_Id_Ex.op;
	new_Ex_Mm.instruct = old_Id_Ex.instruct;
	switch(old_Id_Ex.op)
	{
		case 0:
		{
			//Nop
			new_Ex_Mm.immediate = 0 ;
			new_Ex_Mm.first_reg = 0 ;
			new_Ex_Mm.second_reg = 0 ;
			new_Ex_Mm.third_reg = 0 ;
			new_Ex_Mm.op_A = 0 ;
			new_Ex_Mm.op_B = 0 ;
			new_Ex_Mm.alu_results = 0;
			break;
		}
		case 1: //ADDI ADD IMMEDIATE
		{
			new_Ex_Mm.immediate = old_Id_Ex.immediate;
			new_Ex_Mm.second_reg = old_Id_Ex.second_reg;
			new_Ex_Mm.first_reg = old_Id_Ex.first_reg;
			new_Ex_Mm.op_A = old_Id_Ex.op_A;
			if(old_Id_Ex.second_reg == old_Ex_Mm.first_reg)
			{
				old_Id_Ex.op_A = old_Ex_Mm.alu_results;
			}
			if(old_Id_Ex.second_reg == old_Mm_Wb.first_reg)
			{
				old_Id_Ex.op_A = old_Mm_Wb.alu_results;
			}
			new_Ex_Mm.alu_results = old_Id_Ex.immediate + old_Id_Ex.op_A;
			break;
		}
		case 2: //B BRANCH
		{
			break;
		}
		case 3: //BEQZ BRACH IF EQUAL TO ZERO
		{
			break;
		}
		case 4: //BGE BRANCH IF GREATER OR EQUAL $t0,$t1,target,  $t0 >= $t1
		{
			break;
		}
		case 5: //BNE BRANCH IF NOT EQUAL  $t0,$t1,target, $t0 <> $t1
		{
			break;
		}
		case 6: //LA LOAD ADDRESS
		{
			new_Ex_Mm.first_reg = old_Id_Ex.first_reg;
			new_Ex_Mm.op_A = old_Id_Ex.op_A;
			if(old_Id_Ex.second_reg == old_Ex_Mm.first_reg)
			{
				old_Id_Ex.op_A = old_Ex_Mm.alu_results;
			}
			if(old_Id_Ex.second_reg == old_Mm_Wb.first_reg)
			{
				old_Id_Ex.op_A = old_Mm_Wb.alu_results;
			}
			new_Ex_Mm.alu_results = old_Id_Ex.op_A;
			break;
		}
		case 7: //LB LOAD BYTE
		{
			new_Ex_Mm.first_reg = old_Id_Ex.first_reg;
			new_Ex_Mm.second_reg = old_Id_Ex.second_reg;
			new_Ex_Mm.immediate = old_Id_Ex.immediate;
			new_Ex_Mm.op_A = old_Id_Ex.op_A;			//number of bytes
			if(old_Id_Ex.second_reg == old_Ex_Mm.first_reg)
			{
				old_Id_Ex.op_A = old_Ex_Mm.alu_results;
			}
			if(old_Id_Ex.second_reg == old_Mm_Wb.first_reg)
			{
				old_Id_Ex.op_A = old_Mm_Wb.alu_results;
			}
			new_Ex_Mm.alu_results = old_Id_Ex.op_A + old_Id_Ex.immediate;
			break;
		}
		case 8: //LI LOAD IMMEDIATE
		{
			new_Ex_Mm.first_reg = old_Id_Ex.first_reg;
			new_Ex_Mm.second_reg = old_Id_Ex.second_reg;
			new_Ex_Mm.op_A = old_Id_Ex.op_A;
			if(old_Id_Ex.second_reg == old_Ex_Mm.first_reg)
			{
				old_Id_Ex.op_A = old_Ex_Mm.alu_results;
			}
			if(old_Id_Ex.second_reg == old_Mm_Wb.first_reg)
			{
				old_Id_Ex.op_A = old_Mm_Wb.alu_results;
			}
			new_Ex_Mm.alu_results = old_Id_Ex.op_A;
			break;
		}
		case 9: //SUBI SUBTRACT IMMEDIATE
		{
			new_Ex_Mm.immediate = old_Id_Ex.immediate;
			new_Ex_Mm.op_A = old_Id_Ex.op_A;
			new_Ex_Mm.second_reg = old_Id_Ex.second_reg;
			new_Ex_Mm.first_reg = old_Id_Ex.first_reg;
			if(old_Id_Ex.second_reg == old_Ex_Mm.first_reg)
			{
				old_Id_Ex.op_A = old_Ex_Mm.alu_results;
			}
			if(old_Id_Ex.second_reg == old_Mm_Wb.first_reg)
			{
				old_Id_Ex.op_A = old_Mm_Wb.alu_results;
			}
			new_Ex_Mm.alu_results =  old_Id_Ex.op_A + (- old_Id_Ex.immediate);
			break;
		}
		case 10: //SYSCALL
		{
			instruction regist_value = old_Id_Ex.op_B;
			if(old_Id_Ex.second_reg == old_Ex_Mm.first_reg)
			{
				regist_value = old_Ex_Mm.alu_results;
			}
			if(old_Id_Ex.second_reg == old_Mm_Wb.first_reg)
			{
				regist_value = old_Mm_Wb.alu_results;
			}
			new_Ex_Mm.second_reg = old_Id_Ex.second_reg;
			new_Ex_Mm.op_B = regist_value;
			switch(regist_value)
			{
				case 1:
				{
					new_Ex_Mm.first_reg = old_Id_Ex.first_reg;
					new_Ex_Mm.op_A = old_Id_Ex.op_A;
					break;
				}
				case 4:	//Print String
				{
					old_Id_Ex.first_reg =1;
					instruction regist_value = old_Id_Ex.op_A;
					if(old_Id_Ex.first_reg == old_Ex_Mm.first_reg)
					{
						regist_value = old_Ex_Mm.alu_results;
					}
					if(old_Id_Ex.first_reg == old_Mm_Wb.first_reg)
					{
						regist_value = old_Mm_Wb.alu_results;
					}
					if(old_Id_Ex.first_reg == new_Mm_Wb.first_reg)
					{
						regist_value = new_Mm_Wb.alu_results;
					}
					new_Ex_Mm.first_reg = old_Id_Ex.first_reg;
					new_Ex_Mm.op_A = regist_value;
					break;
				}
				case 8:	//Read String In
				{
					old_Id_Ex.first_reg = 1;
					instruction regist_value = old_Id_Ex.op_A;
					if(old_Id_Ex.first_reg == old_Ex_Mm.first_reg)
					{
						regist_value = old_Ex_Mm.alu_results;
					}
					if(old_Id_Ex.first_reg == old_Mm_Wb.first_reg)
					{
						regist_value = old_Mm_Wb.alu_results;
					}
					if(old_Id_Ex.first_reg == new_Mm_Wb.first_reg)
					{
						regist_value = new_Mm_Wb.alu_results;
					}
					new_Ex_Mm.first_reg = old_Id_Ex.first_reg;
					new_Ex_Mm.op_A = regist_value;
					break;
				}
				case 10:// End Program
				{
					break;
				}
				default:
				{
					cout << "Error: There was an error with the Execute of SYSCALL." << endl;
					cout << "PC: " << std::hex << pc << endl;
					cout << "Current Istruction: " <<std::hex << *current_instruction << endl;
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
			cout << "Error: There was an error with the Execute Stage." << endl;
			cout << "PC: " << std::hex << pc << endl;
			cout << "Current Istruction: " <<std::hex << *current_instruction << endl;
			more_instructions = false;
			break;
	}
}
/*
Used Paramaters:
Effected registers:
*/
void Sim::MM()
{
	//store or load from Memory
	new_Mm_Wb.op = old_Ex_Mm.op;
	new_Mm_Wb.instruct = old_Ex_Mm.instruct;
	switch(old_Ex_Mm.op)
	{
		case 0:
		{
			//Nop
			new_Mm_Wb.immediate = 0 ;
			new_Mm_Wb.first_reg = 0 ;
			new_Mm_Wb.second_reg = 0 ;
			new_Mm_Wb.third_reg = 0 ;
			new_Mm_Wb.op_A = 0 ;
			new_Mm_Wb.op_B = 0 ;
			new_Mm_Wb.alu_results = 0;
			new_Mm_Wb.mem_read_results =0;
			break;
		}
		case 1: //ADDI ADD IMMEDIATE
		{
			new_Mm_Wb.immediate = old_Ex_Mm.immediate;
			new_Mm_Wb.second_reg = old_Ex_Mm.second_reg;
			new_Mm_Wb.first_reg = old_Ex_Mm.first_reg;
			new_Mm_Wb.op_A = old_Ex_Mm.op_A;
			new_Mm_Wb.alu_results = old_Ex_Mm.alu_results;
			break;
		}
		case 2: //B BRANCH
		{
			break;
		}
		case 3: //BEQZ BRACH IF EQUAL TO ZERO
		{
			break;
		}
		case 4: //BGE BRANCH IF GREATER OR EQUAL $t0,$t1,target,  $t0 >= $t1
		{
			break;
		}
		case 5: //BNE BRANCH IF NOT EQUAL  $t0,$t1,target, $t0 <> $t1
		{
			break;
		}
		case 6: //LA LOAD ADDRESS
		{
			new_Mm_Wb.first_reg = old_Ex_Mm.first_reg;
			new_Mm_Wb.op_A = old_Ex_Mm.op_A;
			new_Mm_Wb.alu_results = old_Ex_Mm.op_A;
			break;
		}
		case 7: //LB LOAD BYTE
		{
			new_Mm_Wb.first_reg = old_Ex_Mm.first_reg;
			new_Mm_Wb.second_reg = old_Ex_Mm.second_reg;
			new_Mm_Wb.immediate = old_Ex_Mm.immediate;
			new_Mm_Wb.op_A = old_Ex_Mm.op_A;//number of bytes
			new_Mm_Wb.alu_results = old_Ex_Mm.alu_results;
			new_Mm_Wb.mem_read_results = mem->read_byte(old_Ex_Mm.alu_results, old_Ex_Mm.alu_results%4);
			break;
		}
		case 8: //LI LOAD IMMEDIATE
		{
			new_Mm_Wb.first_reg = old_Ex_Mm.first_reg;
			new_Mm_Wb.second_reg = old_Ex_Mm.second_reg;
			new_Mm_Wb.op_A = old_Ex_Mm.op_A;
			new_Mm_Wb.alu_results = old_Ex_Mm.alu_results;
			break;
		}
		case 9: //SUBI SUBTRACT IMMEDIATE
		{
			new_Mm_Wb.immediate = old_Ex_Mm.immediate;
			new_Mm_Wb.op_A = old_Ex_Mm.op_A;
			new_Mm_Wb.second_reg = old_Ex_Mm.second_reg;
			new_Mm_Wb.first_reg = old_Ex_Mm.first_reg;
			new_Mm_Wb.alu_results = old_Ex_Mm.alu_results;
			break;
		}
		case 10: //SYSCALL
		{
			new_Mm_Wb.second_reg = old_Ex_Mm.second_reg;
			new_Mm_Wb.op_B = old_Ex_Mm.op_B;
			switch(old_Ex_Mm.op_B)
			{
				case 1:
				{
					if(old_Ex_Mm.first_reg == 1)
						{
							cout << "Printed Integer: 1001" << endl;
						}
					break;
				}
				case 4:	//Print String
				{
					new_Mm_Wb.first_reg = old_Ex_Mm.first_reg;
					new_Mm_Wb.op_A = old_Ex_Mm.op_A;
					cout << mem->read_string(old_Ex_Mm.op_A) << endl;
					break;
				}
				case 8:	//Read String In
				{
					new_Mm_Wb.first_reg = old_Ex_Mm.first_reg;
					new_Mm_Wb.op_A = old_Ex_Mm.op_A;

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
					mem->load_string(old_Ex_Mm.op_A,palin);
					break;
				}
				case 10:// End Program
				{
					break;
				}
				default:
				{
					cout << "Error: There was an error with the Memory of SYSCALL." << endl;
					cout << "PC: " << std::hex << pc << endl;
					cout << "Current Istruction: " <<std::hex << *current_instruction << endl;
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
			cout << "Error: There was an error with the Memory Stage." << endl;
			cout << "PC: " << std::hex << pc << endl;
			cout << "Current Istruction: " <<std::hex << *current_instruction << endl;
			more_instructions = false;
			break;
	}
}
/*
Used Paramaters:
Effected buffers:
*/
void Sim::WB()
{
	//store in registers
	switch(old_Mm_Wb.op)
	{
		case 0:
		{
			total_nops++;
			//Nop
			break;
		}
		case 1: //ADDI ADD IMMEDIATE
		{
			bool success = registers->write(old_Mm_Wb.first_reg, old_Mm_Wb.alu_results);
			if (false == success)
			{
				cout << "Error: Adding value (1) to register: "<< std::dec << old_Mm_Wb.first_reg << endl;
			}

			total_instructions_executed++;
			break;
		}
		case 2: //B BRANCH
		{
			total_instructions_executed++;
			break;
		}
		case 3: //BEQZ BRACH IF EQUAL TO ZERO
		{
			total_instructions_executed++;
			break;
		}
		case 4: //BGE BRANCH IF GREATER OR EQUAL $t0,$t1,target,  $t0 >= $t1
		{
			total_instructions_executed++;
			break;
		}
		case 5: //BNE BRANCH IF NOT EQUAL  $t0,$t1,target, $t0 <> $t1
		{
			total_instructions_executed++;
			break;
		}
		case 6: //LA LOAD ADDRESS
		{
			bool success = registers->write(old_Mm_Wb.first_reg,old_Mm_Wb.op_A);
			if (false == success)
			{
				cout << "Error: Loading Address (6) to register: "<< std::dec << old_Mm_Wb.first_reg << endl;
			}
			total_instructions_executed++;
			break;
		}
		case 7: //LB LOAD BYTE
		{
			bool success = registers->write(old_Mm_Wb.first_reg, old_Mm_Wb.mem_read_results );
			if (false == success)
			{
				cout << "Error: Loading Byte (7) into register: "<< std::dec << old_Mm_Wb.first_reg << endl;
			}
			total_instructions_executed++;
			break;
		}
		case 8: //LI LOAD IMMEDIATE
		{
			bool success = registers->write(old_Mm_Wb.first_reg, old_Mm_Wb.second_reg);
			if (false == success)
			{
				cout << "Error: Loading Immediate value (8) to register: "<< std::dec << old_Mm_Wb.first_reg << endl;
			}
			total_instructions_executed++;
			break;
		}
		case 9: //SUBI SUBTRACT IMMEDIATE
		{
			bool success = registers->write(old_Mm_Wb.first_reg, old_Mm_Wb.alu_results);
			if (false == success)
			{
				cout << "Error: Adding value (9) to register: "<< std::dec << old_Mm_Wb.first_reg << endl;
			}
			total_instructions_executed++;
			break;
		}
		case 10: //SYSCALL
		{
			switch(old_Mm_Wb.op_B)
			{
				case 1:
				{
					total_instructions_executed++;
					break;
				}
				case 4:	//Print String
				{
					total_instructions_executed++;
					break;
				}
				case 8:	//Read String In
				{
					total_instructions_executed++;
					break;
				}
				case 10:// End Program
				{
					total_instructions_executed++;
					cout << endl;
					cout << "Number of Instructions Executed (IC): " << std::dec<< total_instructions_executed << endl;
					cout << "Number of Cycles Spent in Execution (C): " <<std::dec<<  total_cycles_spent << endl;
					cout << "Number of NOPs: " << std::dec << total_nops << endl;
					cout << "Goodbye." << endl;
					more_instructions = false;
					break;
				}
				default:
				{
					cout << "Error: There was an error with the Write Buffer of SYSCALL." << endl;
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
			cout << "Error: There was an error with the Write Buffer Stage." << endl;
			cout << "PC: " << std::hex << pc << endl;
			cout << "Current Istruction: " <<std::hex << current_instruction << endl;
			more_instructions = false;
			break;
	}
}

int Sim::instruction_op()
{															//Removes the memory address from instruction, bits 32-24
	instruction op_value;
	op_value = *current_instruction;
	if(op_value == 0 )
		op_value = 0;
	else
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

void Sim::print_buffers()
{
	cout << "NOW: " <<endl;
	cout << "=========================================================================" << endl;
	//cout << ""<< std::hex << *(old_If_Id.instruction_) << endl;
	cout << "            op "<< std::hex << old_Id_Ex.instruct << endl;
	cout << "            op "<< std::dec << old_Id_Ex.op << endl;
	cout << "            im "<< std::dec << static_cast<int16_t>(old_Id_Ex.immediate) << endl;
	cout << "            f "<< std::dec << old_Id_Ex.first_reg << endl;
	cout << "            s "<< std::dec << old_Id_Ex.second_reg << endl;
	cout << "            t "<< std::dec << old_Id_Ex.third_reg << endl;
	cout << "            A "<< std::dec << old_Id_Ex.op_A << endl;
	cout << "            B "<< std::dec << old_Id_Ex.op_B << endl;
	cout << "                        op "<< std::hex << old_Ex_Mm.instruct << endl;
	cout << "                        op "<< std::dec << old_Ex_Mm.op << endl;
	cout << "                        im "<< std::dec << static_cast<int16_t>(old_Ex_Mm.immediate) << endl;
	cout << "                         f "<< std::dec << old_Ex_Mm.first_reg << endl;
	cout << "                         s "<< std::dec << old_Ex_Mm.second_reg << endl;
	cout << "                         t "<< std::dec << old_Ex_Mm.third_reg << endl;
	cout << "                         A "<< std::dec << old_Ex_Mm.op_A << endl;
	cout << "                         B "<< std::dec << old_Ex_Mm.op_B << endl;
	cout << "                       alu "<< std::dec << old_Ex_Mm.alu_results << endl;
	cout << "                                            op "<< std::hex << old_Mm_Wb.instruct << endl;
	cout << "                                            op "<< std::dec << old_Mm_Wb.op << endl;
	cout << "                                            im "<< std::dec << static_cast<int16_t>(old_Mm_Wb.immediate) << endl;
	cout << "                                             f "<< std::dec << old_Mm_Wb.first_reg << endl;
	cout << "                                             s "<< std::dec << old_Mm_Wb.second_reg << endl;
	cout << "                                             t "<< std::dec << old_Mm_Wb.third_reg << endl;
	cout << "                                             A "<< std::dec << old_Mm_Wb.op_A << endl;
	cout << "                                             B "<< std::dec << old_Mm_Wb.op_B << endl;
	cout << "                                           alu "<< std::dec << old_Mm_Wb.alu_results << endl;
	cout << "                                           mem "<< std::dec << old_Mm_Wb.mem_read_results << endl;
	cout << "=========================================================================" << endl;
	registers->print_memory();
	// cout << "NEXT: " << endl;
	// cout << "=========================================================================" << endl;
	// //cout << ""<< std::hex << *(new_If_Id.instruction_) << endl;
	// cout << "            op "<< std::hex << new_Id_Ex.instruct << endl;
	// cout << "            op "<< std::dec << new_Id_Ex.op << endl;
	// cout << "            im "<< std::dec << static_cast<int16_t>(new_Id_Ex.immediate) << endl;
	// cout << "            f "<< std::dec << new_Id_Ex.first_reg << endl;
	// cout << "            s "<< std::dec << new_Id_Ex.second_reg << endl;
	// cout << "            t "<< std::dec << new_Id_Ex.third_reg << endl;
	// cout << "            A "<< std::dec << new_Id_Ex.op_A << endl;
	// cout << "            B "<< std::dec << new_Id_Ex.op_B << endl;
	// cout << "                        op "<< std::hex << new_Ex_Mm.instruct << endl;
	// cout << "                        op "<< std::dec << new_Ex_Mm.op << endl;
	// cout << "                        im "<< std::dec << static_cast<int16_t>(new_Ex_Mm.immediate) << endl;
	// cout << "                         f "<< std::dec << new_Ex_Mm.first_reg << endl;
	// cout << "                         s "<< std::dec << new_Ex_Mm.second_reg << endl;
	// cout << "                         t "<< std::dec << new_Ex_Mm.third_reg << endl;
	// cout << "                         A "<< std::dec << new_Ex_Mm.op_A << endl;
	// cout << "                         B "<< std::dec << new_Ex_Mm.op_B << endl;
	// cout << "                       alu "<< std::dec << new_Ex_Mm.alu_results << endl;
	// cout << "                                            op "<< std::hex << new_Mm_Wb.instruct << endl;
	// cout << "                                            op "<< std::dec << new_Mm_Wb.op << endl;
	// cout << "                                            im "<< std::dec << static_cast<int16_t>(new_Mm_Wb.immediate )<< endl;
	// cout << "                                             f "<< std::dec << new_Mm_Wb.first_reg << endl;
	// cout << "                                             s "<< std::dec << new_Mm_Wb.second_reg << endl;
	// cout << "                                             t "<< std::dec << new_Mm_Wb.third_reg << endl;
	// cout << "                                             A "<< std::dec << new_Mm_Wb.op_A << endl;
	// cout << "                                             B "<< std::dec << new_Mm_Wb.op_B << endl;
	// cout << "                                           alu "<< std::dec << new_Mm_Wb.alu_results << endl;
	// cout << "                                           mem "<< std::dec << new_Mm_Wb.mem_read_results << endl;
	// cout << "=========================================================================" << endl;
}
