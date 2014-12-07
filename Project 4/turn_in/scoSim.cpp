/*******
	General Purpose Register Machine Simulation - Pipe Line with a Scoreboard
	12/6/14
	Ben Gustafson
	COMP 4300
********/

#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <vector>
#include "scoMem.cpp"
#include "scoReg.cpp"
#include "scoRegFloat.cpp"
#include "scoScoreboard.cpp"

using namespace std;

// Press any key to continue
void PressAnyKey(void);

struct fetch_buffer
{
	instruction instruct;
	bool used;
	mem_addr pc;
	bool ready;
}

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
	struct instruction_struct populate_new_instruction();
	instruction *current_instruction;			//Pointer to the current instruction
	void load_next_instruction();				//Takes all the steps to load next instruction
	void fetch_and_issue_instruction();
	void read_instruction();
	void run_functional_units();
	void write_out();
	void floating_multiply();
	void floating_add();
	void integer_alu();
	void memory_write();
	mem_addr pc;								//Program counter
	Memory *mem;								//Memory object
	Register_Bank *registers;					//CPU internal registers
	Floating_Point_Register_Bank *floating_registers;
	Scoreboard *scoreboard_current;
	Scoreboard *scoreboard_next;
	int8_t signed_immediate(mem_addr m_addr);   //return a sgined value;
	void print_buffers();
	bool more_instructions;
	int total_instructions_executed;
	int total_cycles_spent;
	int total_nops;
	std::vector<instruction_struct> read_operands_buffer;
	std::vector<instruction_struct> instructions_in_functional_units;
	std::vector<instruction_struct> write_out_buffer;
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
	scoreboard_current = new Scoreboard();
	scoreboard_next = new Scoreboard();
	fetch_buffer next_instruction = {0,0,pc};
	more_instructions = true;
	total_instructions_executed = 0;
	total_cycles_spent = 0;
	total_nops = 0;
	new_If_Id.instruction_ =0;
	old_If_Id.instruction_ =0;
}

void Sim::run()
{
	while(more_instructions)
	{
		//PressAnyKey();
		//mem->print_memory();
		//print_buffers();
		//scoreboard_current->print();

		fetch_and_issue_instruction();
		read_instruction();
		run_functional_units();
		write_out();

		total_cycles_spent++;
		scoreboard_current->deep_copy(scoreboard_next);
		if(scoreboard_next->all_instructions_complete() && mem->read(pc) == 0 ) //All instructions complete and current instruciton is nop
		{
			more_instructions = false;
			cout << "Number of Instructions Executed (IC): " << std::dec<< total_instructions_executed << endl;
			cout << "Number of Cycles Spent in Execution (C): " <<std::dec<<  total_cycles_spent << endl;
			cout << "Number of NOPs: " << std::dec << total_nops << endl;
			cout << "Goodbye." << endl;
		}
	}
}

//=========================================================================================
//----------------------------- Fetch & Issue Stage ---------------------------------------
//=========================================================================================
/*
Used Paramaters:
Effected buffers:
*/
void Sim::fetch_and_issue_instruction()
{
	// Can this instruction be issued? Next three checks will decide.
	current_instruction = mem->read(pc);
	bool issue_instruction = true;
	mem_addr op_code = instruction_op();
	//Is a functional unit free?
	//( Does it have any structural hazards? )
	int unit_id = scoreboard_current->functional_unit_id(op_code);
	if (!scoreboard_current->open_functional_unit(unit_id))
	{
		issue_instruction = false;
	}
	//Does any other instruction currently in functional units have the same dest?
	// (Does it have any WAW hazards?)
	if(!scoreboard_current->write_buffer_open(op_code,first_register()))
	{
		issue_instruction = false;
	}
	//Issue instruction (or not)
	if(issue_instruction)
	{
		//Issue instruciton
		if (next_instruction == 0)
		{
			issue_instruciton = false;
			pc++;
			total_instructions_executed++;
			total_nops++;
		}
		else
		{
			pc++;
			total_instructions_executed++;
			//Add it to the correct read-operands buffer
			instruction_struct new_instruction = populate_new_instruction();
			read_operands_buffer.push_back(new_instruction);
			//Update scoreboard
			scoreboard_current->issue_instruction(total_cycles_spent,new_instruction);
		}
	}
	else
	{
		//Do not issue instruction
		//This is a stall
		//Do nothing.
	}
}
//=========================================================================================
//----------------------------- Read Operands Stage ---------------------------------------
//=========================================================================================
/*
Used Paramaters:
Effected buffers:
*/
void Sim::read_instruction()
{
	//Are all operands ready to be read?
	int i = 0, k = 0;
	while (k < read_operands_buffer.size())
	{
		i = 0;
		while (i < scoreboard_current.fu_status.size())
		{
			if (fu_status[i].pc == read_operands_buffer[k].pc && read_operands_buffer[k].used == false && read_operands_buffer[k].ready = false)
			{
				if(fu_status[i].rj == true && fu_status[i].rk == true);
				{
					read_operands_buffer[k].ready = true;
				}
			}
			i++;
		}
		k++;
	}
	k = 0;
	while (k < read_operands_buffer.size())
	{
		if (read_operands_buffer[k].ready == true)
		{
			read_operands_buffer[k].used == true;
			switch(read_operands_buffer[k].op)
			{
				case 0:
				{
					//Nop
					cout << "Error: There should not be a NOP in the read operands stage. Make it stop!" << endl;
					cout << "PC: " << std::hex << pc << endl;
					cout << "Current Istruction: " <<std::hex << *current_instruction << endl;
					//more_instructions = false;
					break;
					break;
				}
				case 1: //ADDI ADD IMMEDIATE
				{
					read_operands_buffer[k].op_A = registers->read(read_operands_buffer[k].second_reg_name);
					break;
				}
				case 2: //B BRANCH
				{
					//Do nothing
					break;
				}
				case 3: //BEQZ BRACH IF EQUAL TO ZERO
				{
					read_operands_buffer[k].op_A = registers->read(read_operands_buffer[k].first_reg_name);
					break;
				}
				case 4: //BGE BRANCH IF GREATER OR EQUAL $t0,$t1,target,  $t0 >= $t1
				{
					read_operands_buffer[k].op_A = registers->read(read_operands_buffer[k].first_reg_name);
					read_operands_buffer[k].op_B = registers->read(read_operands_buffer[k].second_reg_name);
					break;
				}
				case 5: //BNE BRANCH IF NOT EQUAL  $t0,$t1,target, $t0 <> $t1
				{
					read_operands_buffer[k].op_A = registers->read(read_operands_buffer[k].first_reg_name);
					read_operands_buffer[k].op_B = registers->read(read_operands_buffer[k].second_reg_name);
					break;
				}
				case 6: //LA LOAD ADDRESS
				{
					//Do nothing
					break;
				}
				case 7: //LB LOAD BYTE
				{
					read_operands_buffer[k].op_A = registers->read(read_operands_buffer[k].second_reg_name);//number of bytes
					break;
				}
				case 8: //LI LOAD IMMEDIATE
				{
					//Do nothing
					break;
				}
				case 9: //SUBI SUBTRACT IMMEDIATE
				{
					read_operands_buffer[k].op_A = registers->read(read_operands_buffer[k].second_reg_name);
					break;
				}
				case 10: //SYSCALL
				{
					read_operands_buffer[k].op_B = registers->read(read_operands_buffer[k].second_reg_name);
					switch(read_operands_buffer[k].op_B)
					{
						case 1:
						{
							read_operands_buffer[k].op_A = registers->read(read_operands_buffer[k].first_reg_name);
							break;
						}
						case 4:	//Print String
						{
							read_operands_buffer[k].op_A = registers->read(read_operands_buffer[k].first_reg_name);
							break;
						}
						case 8:	//Read String In
						{
							read_operands_buffer[k].op_A = registers->read(read_operands_buffer[k].first_reg_name);
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
				case 11:	//FADD - add two floats
				{
					read_operands_buffer[k].float_op_A = floating_registers->read(read_operands_buffer[k].second_reg_name);
					read_operands_buffer[k].float_op_B = floating_registers->read(read_operands_buffer[k].third_reg_name);
					break;
				}
				case 12:	//FMUL - multiply two floats
				{
					read_operands_buffer[k].float_op_A = floating_registers->read(read_operands_buffer[k].second_reg_name);
					read_operands_buffer[k].float_op_B = floating_registers->read(read_operands_buffer[k].third_reg_name);
					break;
				}
				case 13:  //FSUB
				{
					read_operands_buffer[k].float_op_A = floating_registers->read(read_operands_buffer[k].second_reg_name);
					read_operands_buffer[k].float_op_B = floating_registers->read(read_operands_buffer[k].third_reg_name);
					break;
				}
				case 14: // L.D load
				{
					read_operands_buffer[k].op_A = registers->read(read_operands_buffer[k].second_reg_name);
					break;
				}
				case 15: // S.D -- store
				{
					read_operands_buffer[k].op_A = registers->read(read_operands_buffer[k].second_reg_name);
					read_operands_buffer[k].float_op_A = floating_registers->read(read_operands_buffer[k].first_reg_name);
					break;
				}
				case 16: // add
				{
					read_operands_buffer[k].op_A = registers->read(read_operands_buffer[k].second_reg_name);
					read_operands_buffer[k].op_B = registers->read(read_operands_buffer[k].third_reg_name);
					return INTEGER_ALU_ID;
					break;
				}
				default:
					cout << "Error: There was an error with reading operands" << endl;
					cout << "PC: " << std::hex << read_operands_buffer[k].pc << endl;
					cout << "Current Istruction: " << std::hex << read_operands_buffer[k].instruction << endl;
					//more_instructions = false;
					break;
			}
			struct instruction_struct updated_instruction = read_operands_buffer[k];
			read_operands_buffer.erase(k);
			k--;

			//put instruction into functional units buffer, with clocks left
			switch(scoreboard_current->functional_unit_id(updated_instruction.op))
			{
				case 1:
				{
					updated_instruction.clocks_left = 1;
					break;
				}
				case 2:
				{
					updated_instruction.clocks_left = 1;
					break;
				}
				case 3:
				{
					updated_instruction.clocks_left = 5;
					break;
				}
				case 4:
				{
					updated_instruction.clocks_left = 1;
					break;
				}
				default:
				{
					cout << "Error: There was an error putting an instruciton into the functional unit buffer" << endl;
					cout << "PC: " << std::hex << updated_instruction.pc << endl;
					cout << "Current Istruction: " << std::hex << updated_instruction.instruction << endl;
					//more_instructions = false;
					break;
				}
			}
			instructions_in_functional_units.push_back(updated_instruction);

			//update scoreboard
			int j = 0;
			while (j < scoreboard_current.instruction_status.size())
			{
				if(scoreboard_current.instruction_status[j].pc == updated_instruction.pc)
				{
					scoreboard_current.instruction_status[j].read = total_cycles_spent;
				}
			}

		}
	k++;
	}
}

//=========================================================================================
//-----------------------------Functional stage--------------------------------------------
//=========================================================================================
/*
Used Paramaters:
Effected buffers:
*/
void Sim::run_functional_units()
{
	//TODO:

	floating_multiply();
	floating_add();
	integer_alu();
	memory_write();





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
//=========================================================================================
//----------------------------- Write Stage -----------------------------------------------
//=========================================================================================
/*
Used Paramaters:
Effected registers:
*/
void Sim::write_out()
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

//=========================================================================================
//-----------------------------Functional untis--------------------------------------------
//=========================================================================================
void Sim::floating_multiply()
{
	int i = 0;
	while(i < instructions_in_functional_units.size())
	{
		if(3 == scoreboard_current->fuctional_units_id(instructions_in_functional_units[i].op))
		{
			if(instructions_in_functional_units[i].clocks_left == 0)
			{
				//do the instruciton
				switch(instructions_in_functional_units[i].op)
				{
					case 12: //FMUL
					{
						instructions_in_functional_units[i].floating_alu_results = instructions_in_functional_units[i].float_op_A * instructions_in_functional_units[i].float_op_B;
						break;
					}
					default:
					{
						cout << "Error: There was an error with the floating multiply functional unit." << endl;
						cout << "PC: " << std::hex << instructions_in_functional_units[i].pc << endl;
						cout << "Current Istruction: " <<std::hex << instructions_in_functional_units[i].instruction << endl;
						more_instructions = false;
						break;
					}
				}
				//put it in write buffer
				instructions_in_functional_units[i].used = false;
				instructions_in_functional_units[i].ready = false;
				write_out_buffer.push_back(instructions_in_functional_units[i]);
				//erase it from the instructions_in_functional_units
				instructions_in_functional_units.erase(i);
				i--;
			}
			else
			{
				instructions_in_functional_units[i].clocks_left--;
			}
		}
		i++;
	}
}
void Sim::floating_add()
{
	int i = 0;
	while(i < instructions_in_functional_units.size())
	{
		if(2 == scoreboard_current->fuctional_units_id(instructions_in_functional_units[i].op))
		{
			if(instructions_in_functional_units[i].clocks_left == 0)
			{
				//do the instruciton
				switch(instructions_in_functional_units[i].op)
				{
					case 11: //FADD
					{
						instructions_in_functional_units[i].floating_alu_results = instructions_in_functional_units[i].float_op_A + instructions_in_functional_units[i].float_op_B;
						break;
					}
					case 13: //FSUB
					{
						instructions_in_functional_units[i].floating_alu_results = instructions_in_functional_units[i].float_op_A - instructions_in_functional_units[i].float_op_B;
						break;
					}
					default:
					{
						cout << "Error: There was an error with the floating multiply functional unit." << endl;
						cout << "PC: " << std::hex << instructions_in_functional_units[i].pc << endl;
						cout << "Current Istruction: " <<std::hex << instructions_in_functional_units[i].instruction << endl;
						more_instructions = false;
						break;
					}
				}
				//put it in write buffer
				instructions_in_functional_units[i].used = false;
				instructions_in_functional_units[i].ready = false;
				write_out_buffer.push_back(instructions_in_functional_units[i]);
				//erase it from the instructions_in_functional_units
				instructions_in_functional_units.erase(i);
				i--;
			}
			else
			{
				instructions_in_functional_units[i].clocks_left--;
			}
		}
		i++;
	}
}
void Sim::integer_alu()
{
	int i = 0;
	while(i < instructions_in_functional_units.size())
	{
		if(1 == scoreboard_current->fuctional_units_id(instructions_in_functional_units[i].op))
		{
			if(instructions_in_functional_units[i].clocks_left == 0)
			{
				//do the instruciton
				switch(instructions_in_functional_units[i].op)
				{
					case 1: // ADDI
					{
						instructions_in_functional_units[i].alu_results = instructions_in_functional_units[i].op_A + instructions_in_functional_units[i].immediate;
						break;
					}
					case 2: //B BRANCH
					{
						pc = instructions_in_functional_units[i].pc + instructions_in_functional_units[i].immediate;
						break;
					}
					case 3: //BEQZ BRACH IF EQUAL TO ZERO
					{
						if (instructions_in_functional_units[i].op_A == 0)
						{
							pc = instructions_in_functional_units[i].pc + instructions_in_functional_units[i].immediate;
						}
						break;
					}
					case 4: //BGE BRANCH IF GREATER OR EQUAL $t0,$t1,target,  $t0 >= $t1
					{
						if(instructions_in_functional_units[i].op_A >= instructions_in_functional_units[i].op_B)
						{
							pc = instructions_in_functional_units[i].pc + instructions_in_functional_units[i].immediate;
						}
						break;
					}
					case 5: //BNE BRANCH IF NOT EQUAL  $t0,$t1,target, $t0 <> $t1
					{
						if(instructions_in_functional_units[i].op_A != instructions_in_functional_units[i].op_B)
						{
							pc = instructions_in_functional_units[i].pc + instructions_in_functional_units[i].immediate;
						}
						break;
					}
					case 9: // SUBI
					{
						instructions_in_functional_units[i].alu_results = instructions_in_functional_units[i].op_A - instructions_in_functional_units[i].immediate;
						break;
					}
					case 16: // ADD
					{
						instructions_in_functional_units[i].alu_results = instructions_in_functional_units[i].op_A + instructions_in_functional_units[i].op_B;
						break;
					}
					default:
					{
						cout << "Error: There was an error with the ALU functional unit." << endl;
						cout << "PC: " << std::hex << instructions_in_functional_units[i].pc << endl;
						cout << "Current Istruction: " <<std::hex << instructions_in_functional_units[i].instruction << endl;
						more_instructions = false;
						break;
					}

				}
				//put it in write buffer, we do not add branches to write out
				if ( 2 != instructions_in_functional_units[i].op && 3 != instructions_in_functional_units[i].op && 4 != instructions_in_functional_units[i].op && 5 != instructions_in_functional_units[i].op && )
				{
					instructions_in_functional_units[i].used = false;
					instructions_in_functional_units[i].ready = false;
					write_out_buffer.push_back(instructions_in_functional_units[i]);
				}
				//erase it from the instructions_in_functional_units
				instructions_in_functional_units.erase(i);
				i--;
			}
			else
			{
				instructions_in_functional_units[i].clocks_left--;
			}
		}
		i++;
	}
}
void Sim::memory_write()
{
	int i = 0;
	while(i < instructions_in_functional_units.size())
	{
		if(4 == scoreboard_current->fuctional_units_id(instructions_in_functional_units[i].op))
		{
			if(instructions_in_functional_units[i].clocks_left == 0)
			{
				//do the instruciton
				switch(instructions_in_functional_units[i].op)
				{
					case 6: // LA
					{
						instructions_in_functional_units[i].mem_read_results = mem->read(instructions_in_functional_units[i].immedtiate);
						break;
					}
					case 7: // Load Byte
					{
						instructions_in_functional_units[i].alu_results = instructions_in_functional_units[i].op_A + instructions_in_functional_units[i].immediate;
						instructions_in_functional_units[i].mem_read_results = mem->read_byte(instructions_in_functional_units[i].alu_results, instructions_in_functional_units[i].alu_results%4);
						break;
					}
					case 8: //B LI
					{
						instructions_in_functional_units[i].mem_read_results = mem->read(instructions_in_functional_units[i].immedtiate);
						break;
					}
					case 10: //SYSCall
					{
						//do Nothing
						break;
					}
					case 14: // L.D
					{
						instructions_in_functional_units[i].alu_results = instructions_in_functional_units[i].op_A + instructions_in_functional_units[i].immediate;
						instructions_in_functional_units[i].float_mem_read_results = mem->read(instructions_in_functional_units[i].alu_results);
						break;
					}
					case 15: // S.D
					{
						instructions_in_functional_units[i].alu_results = instructions_in_functional_units[i].op_A + instructions_in_functional_units[i].immediate;
						mem->write(instructions_in_functional_units[i].alu_results, instructions_in_functional_units[i].float_op_A);
						break;
					}
					default:
					{
						cout << "Error: There was an error with the ALU functional unit." << endl;
						cout << "PC: " << std::hex << instructions_in_functional_units[i].pc << endl;
						cout << "Current Istruction: " <<std::hex << instructions_in_functional_units[i].instruction << endl;
						more_instructions = false;
						break;
					}

				}
				//put it in write buffer, we do not add branches to write out
				instructions_in_functional_units[i].used = false;
				instructions_in_functional_units[i].ready = false;
				write_out_buffer.push_back(instructions_in_functional_units[i]);
				//erase it from the instructions_in_functional_units
				instructions_in_functional_units.erase(i);
				i--;
			}
			else
			{
				instructions_in_functional_units[i].clocks_left--;
			}
		}
		i++;
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
		return_value = 0 - value;
		return return_value;
	}
	else
	{
		return value;
	}
	return 0;
}

void Sim::populate_new_instruction()
{
	instruction_struct new_instruction;
	swtich(instruction_op())
	{
		//all three registers
		case 16:
		case 11:
		case 12:
		case 13:
		{
			new_instruction = {pc,instruction_op(),0,first_register(),second_register(), third_register(),0,0,0,0,0,0,0,0,0,0,0};
			break;
		}
		//two registers, one immediate
		case 1:
		case 4:
		case 5:
		case 7:
		case 9:
		case 14:
		case 15:
		{
			new_instruction = {pc,instruction_op(),signed_immediate(third_register()),first_register(),second_register(),0,0,0,0,0,0,0,0,0,0,0,0};
			break;
		}
		//one register, one immediate
		case 3:
		case 6:
		case 8:
		{
			new_instruction = {pc,instruction_op(),immediate_value(),first_register(),0,0,0,0,0,0,0,0,0,0,0,0,0};
			break;
		}
		// just immediate
		case 2:
		{
			new_instruction = {pc,instruction_op(),signed_immediate(third_register()),0,0,0,0,0,0,0,0,0,0,0,0,0,0};
			break;
		}
		// syscall
		case 10:
		{
			new_instruction = {pc,instruction_op(),0,1,3,0,0,0,0,0,0,0,0,0,0,0,0};
			break;
		}
		default:
		{
			cout << "Error: There was an error in populating the new instruciton" << endl;
			cout << "PC: " << std::hex << pc << endl;
			cout << "Current Istruction: " <<std::hex << current_instruction << endl;
			more_instructions = false;
			break;
		}
	}
	return new_instruction;
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
