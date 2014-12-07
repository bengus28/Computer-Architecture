/*******
    Scoreboard
    12/6/14
    Ben Gustafson
    COMP 4300
********/

#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


#define INTEGER_ALU_SIZE  2
#define FLOATING_ADDER_SIZE 2
#define FLOATING_MULTIPLIER_SIZE 6
#define MEMORY_UNIT_SIZE 1


#define INTEGER_ALU_ID 1
#define FLOATING_ADDER_ID 2
#define FLOATING_MULTIPLIER_ID 3
#define MEMORY_UNIT_ID 4

#define INT_REGISTER_LENGTH  32
#define FLOATING_REGISTER_LENGTH 16

using namespace std;

/*******
    Data Structures
********/
typedef uint32_t mem_addr;
typedef float float_mem;
typedef uint32_t instruction;

struct instruction_status_line
{
    mem_addr pc;
    mem_addr issue;
    mem_addr read;
    mem_addr execute_finished;
    mem_addr write_resutls;
};

struct instruction_struct
{
    mem_addr pc;
    instruction op;
    instruction instruct;
    int8_t immediate;
    mem_addr first_reg_name;
    mem_addr second_reg_name;
    mem_addr third_reg_name;
    instruction op_A;
    instruction op_B;
    mem_addr alu_results;
    mem_addr mem_read_results;
    float_mem float_op_A;
    float_mem float_flop_B;
    float_mem float_alu_results;
    float_mem float_mem_read_results;
    bool used;
    bool ready;
    int clocks_left;
};

struct fu_status_struct
{
    int unit_id;
    bool busy;		//busy?
    mem_addr op;	//op_code
    mem_addr fi;	//dest
    mem_addr fj;	//src 1
    mem_addr fk;	//src 2
    mem_addr qj;	//FU src 1
    mem_addr qk;	//FU src 2
    bool rj;		//Fj full?
    bool rk;		//Fk full?
    mem_addr pc;
};


class Scoreboard
{
public:
    Scoreboard();
    void deep_copy(Scoreboard *score_in);
    int functional_unit_id(mem_addr op_code);
    bool open_functional_unit(int functional_unit_id);
    bool write_buffer_open(mem_addr op_code,mem_addr dest);
    bool all_instructions_complete();
    bool issue_instruction(int clock_time, struct instruction_struct new_instruction);

    bool is_int_register_bank(mem_addr op_code);
    void print();       //Prints out current score board state
    std::vector<instruction_status_line> instruction_status;
    std::vector<fu_status_struct> fu_status;
    std::vector<mem_addr> floating_register_result_status;
    std::vector<mem_addr> integer_register_result_status;
};

/*******
    Class Definition
********/

Scoreboard::Scoreboard()  													//Initialize scoreboard
{
    //empty on purpose
    //Fill fu_status with correct rows
    int i = 0;
    fu_status_struct status = {INTEGER_ALU_ID,0,0,0,0,0,0,0,0,0,0};
    while (i<INTEGER_ALU_SIZE)
    {
        fu_status.push_back(status);
        i++;
    }
    fu_status_struct status1 = {FLOATING_ADDER_ID,0,0,0,0,0,0,0,0,0,0};
    while (i<INTEGER_ALU_SIZE+FLOATING_ADDER_SIZE)
    {
        fu_status.push_back(status1);
        i++;
    }
    fu_status_struct status2 = {FLOATING_MULTIPLIER_ID,0,0,0,0,0,0,0,0,0,0};
    while (i<INTEGER_ALU_SIZE+FLOATING_ADDER_SIZE+FLOATING_MULTIPLIER_SIZE)
    {
        fu_status.push_back(status2);
        i++;
    }
    fu_status_struct status3 = {MEMORY_UNIT_ID,0,0,0,0,0,0,0,0,0,0};
    while (i<INTEGER_ALU_SIZE+FLOATING_ADDER_SIZE+FLOATING_MULTIPLIER_SIZE+MEMORY_UNIT_SIZE)
    {
        fu_status.push_back(status3);
        i++;
    }
    i = 0;
    while (i<INT_REGISTER_LENGTH)
    {
        integer_register_result_status.push_back(0);
        i++;
    }
    i = 0;
    while (i<FLOATING_REGISTER_LENGTH)
    {
        floating_register_result_status.push_back(0);
        i++;
    }
}

void Scoreboard::deep_copy(Scoreboard *score_in)
{
    instruction_status = score_in->instruction_status;
    fu_status = score_in->fu_status;
    floating_register_result_status = score_in->floating_register_result_status;
    integer_register_result_status = score_in->integer_register_result_status;
}

int Scoreboard::functional_unit_id(mem_addr op_code)
{
    switch(op_code)
    {
        case 0: // nop
        {
            return 0;
            break;
        }
        case 1: //ADDI ADD IMMEDIATE
        {
            return INTEGER_ALU_ID;
            break;
        }
        case 2: //B BRANCH
        {
            return INTEGER_ALU_ID;
            break;
        }
        case 3: //BEQZ BRACH IF EQUAL TO ZERO
        {
            return INTEGER_ALU_ID;
            break;
        }
        case 4: //BGE BRANCH IF GREATER OR EQUAL $t0,$t1,target,  $t0 >= $t1
        {
            return INTEGER_ALU_ID;
            break;
        }
        case 5: //BNE BRANCH IF NOT EQUAL  $t0,$t1,target, $t0 <> $t1
        {
            return INTEGER_ALU_ID;
            break;
        }
        case 6: //LA LOAD ADDRESS
        {
            return MEMORY_UNIT_ID;
            break;
        }
        case 7: //LB LOAD BYTE
        {
            return MEMORY_UNIT_ID;
            break;
        }
        case 8: //LI LOAD IMMEDIATE
        {
            return MEMORY_UNIT_ID;
            break;
        }
        case 9: //SUBI SUBTRACT IMMEDIATE
        {
            return INTEGER_ALU_ID;
            break;
        }
        case 10: //SYSCALL
        {
            return MEMORY_UNIT_ID;
            break;
        }
        case 11:	//FADD - add two floats
        {
            return FLOATING_ADDER_ID;
            break;
        }
        case 12:	//FMUL - multiply two floats
        {
            return FLOATING_MULTIPLIER_ID;
            break;
        }
        case 13:  //FSUB
        {
            return FLOATING_ADDER_ID;
            break;
        }
        case 14: // L.D load
        {
            return MEMORY_UNIT_ID;
            break;
        }
        case 15: // S.D -- store
        {
            return MEMORY_UNIT_ID;
            break;
        }
        case 16: // add
        {
            return INTEGER_ALU_ID;
            break;
        }
        default:
            cout << "Error: There was an error with the finding a Functional Unit id." << endl;
            cout << 'Given OP code: ' << std::dec << op_code << endl;
            return -1;
            break;
    }
}

bool open_functional_unit(int functional_unit_id)
{
    //Functional units memory
    int i = 0;
    while (i < fu_status.size())
    {
        if (fu_status[i].unit_id == functional_unit_id && fu_status[i].busy == false )
        {
            return true;
        }
    }
    return false;
}

bool write_buffer_open(mem_addr op_code, mem_addr dest)
{
    //selects the correct buffer,
    //looks for an empty buffer in the dest place, if any, return false

    //NOPs
    if (functional_unit_id(op_code)==0)
    {
        return true;
    }
    if(is_int_register_bank(op_code))
    {
        if(integer_register_result_status[dest] != 0)
        {
            return false;
        }
        else
        {
            return true;  //no current instruction in funcitonal units writing to this dest
        }
    }
    else
    {
        if(floating_register_result_status[dest] != 0)
        {
            return false;
        }
        else
        {
            return true;  //no current instruction in funcitonal units writing to this dest
        }
    }
}

bool Scoreboard::all_instructions_complete()
{
    int i = 0;
    while (i < instruction_status.size())
    {
        //Has not completed yet
        if(instruction_status[i].write_resutls == 0)
        {
            return false;
        }
        i++;
    }

}

bool Scoreboard::issue_instruction(int clock_time, struct instruction_struct new_instruction)
{
    // Update Instruction Status
    struct instruction_status_line this_instruction = {new_instruction.pc,clock_time,0,0,0}
    instruction_status.push_back(this_instruction);

    // Update Functional Unit Satus
    i = 0;
    while (i < fu_status.size())
    {
        //corrent functional unit, not busy
        if(fu_status[i].unit_id == functional_unit_id(new_instruciton.op) && fu_status[i].busy == false)
        {
            swtich(new_instruciton.op)
            {
                //all three registers
                case 16:
                case 11:
                case 12:
                case 13:
                {
                    fu_status[i].busy = true;
                    fu_status[i].op = new_instruction.op;
                    fu_status[i].fi = new_instruction.first_reg_name;
                    fu_status[i].fj = new_instruction.second_reg_name;
                    fu_status[i].fk = new_instruction.third_reg_name;
                    fu_status[i].qj = get_read_buffer_value(new_instruction.second_reg_name);
                    fu_status[i].qk = get_read_buffer_value(new_instruction.third_reg_name);
                    fu_status[i].rj = (fu_status[i].qj == 0) ? true : false;
                    fu_status[i].rk = (fu_status[i].qk == 0) ? true : false;
                    fu_status[i].pc = new_instruction.pc;
                    break;
                }
                //two registers, one immediate
                case 1:
                case 7:
                case 9:
                case 14:
                case 15:
                {
                    fu_status[i].busy = true;
                    fu_status[i].op = new_instruction.op;
                    fu_status[i].fi = new_instruction.first_reg_name;
                    fu_status[i].fj = new_instruction.second_reg_name;
                    fu_status[i].fk = 0;
                    fu_status[i].qj = get_read_buffer_value(new_instruction.second_reg_name);
                    fu_status[i].qk = 0;
                    fu_status[i].rj = (fu_status[i].qj == 0) ? true : false;
                    fu_status[i].rk = true;
                    fu_status[i].pc = new_instruction.pc;
                    break;
                }
                //one register, one immediate
                case 6:
                case 8:
                {
                    fu_status[i].busy = true;
                    fu_status[i].op = new_instruction.op;
                    fu_status[i].fi = new_instruction.first_reg_name;
                    fu_status[i].fj = 0;
                    fu_status[i].fk = 0;
                    fu_status[i].qj = 0;
                    fu_status[i].qk = 0;
                    fu_status[i].rj = true;
                    fu_status[i].rk = true;
                    fu_status[i].pc = new_instruction.pc;
                    break;
                }
                // Branches
                case 2:
                {
                    fu_status[i].busy = true;
                    fu_status[i].op = new_instruction.op;
                    fu_status[i].fi = 0;
                    fu_status[i].fj = 0;
                    fu_status[i].fk = 0;
                    fu_status[i].qj = 0;
                    fu_status[i].qk = 0;
                    fu_status[i].rj = true;
                    fu_status[i].rk = true;
                    fu_status[i].pc = new_instruction.pc;
                    break;
                }
                case 3:
                {
                    fu_status[i].busy = true;
                    fu_status[i].op = new_instruction.op;
                    fu_status[i].fi = 0;
                    fu_status[i].fj = new_instruction.first_reg_name;
                    fu_status[i].fk = 0;
                    fu_status[i].qj = get_read_buffer_value(new_instruction.first_reg_name);
                    fu_status[i].qk = 0;
                    fu_status[i].rj = (fu_status[i].qj == 0) ? false : true;
                    fu_status[i].rk = true;
                    fu_status[i].pc = new_instruction.pc;
                    break;
                }
                case 4:
                case 5:
                {
                    fu_status[i].busy = true;
                    fu_status[i].op = new_instruction.op;
                    fu_status[i].fi = 0;
                    fu_status[i].fj = new_instruction.first_reg_name;
                    fu_status[i].fk = new_instruction.second_reg_name;
                    fu_status[i].qj = get_read_buffer_value(new_instruction.first_reg_name);
                    fu_status[i].qk = get_read_buffer_value(new_instruction.second_reg_name);
                    fu_status[i].rj = (fu_status[i].qj == 0) ? true : false;
                    fu_status[i].rk = (fu_status[i].qk == 0) ? true : false;
                    fu_status[i].pc = new_instruction.pc;
                    break;
                }
                // syscall
                case 10:
                {
                    fu_status[i].busy = true;
                    fu_status[i].op = new_instruction.op;
                    fu_status[i].fi = 0;
                    fu_status[i].fj = new_instruction.first_reg_name;
                    fu_status[i].fk = new_instruction.second_reg_name;
                    fu_status[i].qj = get_read_buffer_value(new_instruction.first_reg_name);
                    fu_status[i].qk = get_read_buffer_value(new_instruction.second_reg_name);
                    fu_status[i].rj = (fu_status[i].qj == 0) ? true : false;
                    fu_status[i].rk = (fu_status[i].qk == 0) ? true : false;
                    fu_status[i].pc = new_instruction.pc;
                    break;
                }
                default:
                {
                    cout << "Error: There was an error in populating the scoreboard's Functional unit status for the new instruciton" << endl;
                    cout << "PC: " << std::hex << new_instruction.pc << endl;
                    return false;
                    break;
                }
            }
        break;
        }
        i++;
    }

    // Update Result Status
    swtich(new_instruciton.op)
    {
        //Have destinations
        case 1:
        case 6:
        case 7:
        case 8:
        case 9:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        {
            //NOPs
            if (functional_unit_id(new_instruciton.op) == 0)
            {
                cout << "Error: Tried to get value for a NOP in the read buffer. [1]" << endl;
                return false;
            }
            if(is_int_register_bank(new_instruciton.op))
            {
                integer_register_result_status[new_instruction.first_reg_name] = functional_unit_id(new_instruciton.op);
            }
            else
            {
                floating_register_result_status[new_instruction.first_reg_name] = functional_unit_id(new_instruciton.op);
            }
            break;
        }
        // Branches & syscall
        case 2:
        case 3:
        case 4:
        case 5:
        case 10:
        {
            //no dest
            //do nothing
            break;
        }
        default:
        {
            cout << "Error: There was an error in populating the scoreboard's read buffer for the new instruciton" << endl;
            cout << "PC: " << std::hex << new_instruction.pc << endl;
            return false;
            break;
        }
    }
    //Success
    return true;
}
mem_addr get_read_buffer_value(mem_addr op_code, mem_addr dest)
{
    //selects the correct buffer,
    //looks for an empty buffer in the dest place, if any, return false

    //NOPs
    if (functional_unit_id(op_code) == 0)
    {
        cout << "Error: Tried to get value for a NOP in the read buffer.[2]" << endl;
        return 0;
    }
    if(is_int_register_bank(op_code))
    {
        return integer_register_result_status[dest];
    }
    else
    {
        return floating_register_result_status[dest];
    }
}

bool Scoreboard::is_int_register_bank(mem_addr op_code)
{
    switch(functional_unit_id(op_code))
    {
        case INTEGER_ALU_ID:
        case MEMORY_UNIT_ID: //B BRANCH
        {
            if(op_code == 14 || op_code == 15 ) // L.D & S.D
            {
                return false;
            }
            else
            {
                return true;
            }
            break;
        }
        case FLOATING_MULTIPLIER_ID: //BEQZ BRACH IF EQUAL TO ZERO
        case FLOATING_ADDER_ID: //BGE BRANCH IF GREATER OR EQUAL $t0,$t1,target,  $t0 >= $t1
        {
            return false;
        }
        default:
        {
            cout << "Error: There was an error when deciding which write buffer to pick in scoreboard" << endl;
            cout << 'Given OP code: ' << std::dec << op_code << endl;
            return false;
            break;
        }
    }
}

void Scoreboard::print()										//To give a visual of the Register Memory space
{
    cout <<	"**********************************************" << endl;
    cout <<	"**********************************************" << endl;
    int i = 0;
    cout <<	"==== Instructoin Status ======================" << endl;
    while (i < instruction_status.size())
    {
        cout << instruction_status[i].pc << " | " << instruction_status[i].issue << " | "
            << instruction_status[i].read << " | " <<instruction_status[i].execute_finished << " | "
            << instruction_status[i].write_resutls << " | " << endl;
        i++;
    }
    cout <<	"==========================" << endl;

    //Functional units memory
    i = 0;
    cout <<	"==== Functional Unit Status ======================" << endl;
    while (i < fu_status.size())
    {
        cout << fu_status[i].unit_id << " | " << fu_status[i].busy << " | "
            << fu_status[i].op << " | " <<fu_status[i].fi << " | "
            << fu_status[i].fj << " | " <<fu_status[i].fk << " | "
            << fu_status[i].qj << " | " <<fu_status[i].qk << " | "
            << fu_status[i].rj << " | " << fu_status[i].rk << " | " << endl;
        i++;
    }
    cout <<	"==========================" << endl;

    // Write out buffer memory
    i = 0;
    cout <<	"==== Write out Status ======================" << endl;
    cout << " == Iteger registers == " << endl;
    while (i < integer_register_result_status.size())
    {
        cout << integer_register_result_status[i] << " | ";
        i++;
    }
    i = 0;
    cout << endl << " == Floating point == " << endl;
    while (i < floating_register_result_status.size())
    {
        cout << floating_register_result_status[i] << " | ";
        i++;
    }
    cout << endl <<	"==========================" << endl;


    cout <<	"**********************************************" << endl;
    cout <<	"**********************************************" << endl;
}
