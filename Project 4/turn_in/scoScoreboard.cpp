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
};


class Scoreboard
{
public:
    Scoreboard();
    void print();       //Prints out current score board state
    void deep_copy(Scoreboard *score_in);
    std::vector<instruction_status_line> instruction_status;
    std::vector<fu_status_struct> fu_status;
    std::vector<int> floating_register_result_status;
    std::vector<int> int_register_result_status;
};

/*******
    Class Definition
********/

Scoreboard::Scoreboard()  													//Initialize scoreboard
{
    //empty on purpose
    //Fill fu_status with correct rows
    int i = 0;
    fu_status_struct status = {INTEGER_ALU_ID,0,0,0,0,0,0,0,0,0};
    while (i<INTEGER_ALU_SIZE)
    {
        fu_status.push_back(status);
        i++;
    }
    fu_status_struct status1 = {FLOATING_ADDER_ID,0,0,0,0,0,0,0,0,0};
    while (i<INTEGER_ALU_SIZE+FLOATING_ADDER_SIZE)
    {
        fu_status.push_back(status1);
        i++;
    }
    fu_status_struct status2 = {FLOATING_MULTIPLIER_SIZE,0,0,0,0,0,0,0,0,0};
    while (i<INTEGER_ALU_SIZE+FLOATING_ADDER_SIZE+FLOATING_MULTIPLIER_SIZE)
    {
        fu_status.push_back(status2);
        i++;
    }
    fu_status_struct status3 = {MEMORY_UNIT_SIZE,0,0,0,0,0,0,0,0,0};
    while (i<INTEGER_ALU_SIZE+FLOATING_ADDER_SIZE+FLOATING_MULTIPLIER_SIZE+MEMORY_UNIT_SIZE)
    {
        fu_status.push_back(status3);
        i++;
    }
    i = 0;
    while (i<INT_REGISTER_LENGTH)
    {
        int_register_result_status.push_back(0);
        i++;
    }
    i = 0;
    while (i<FLOATING_REGISTER_LENGTH)
    {
        floating_register_result_status.push_back(0);
        i++;
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
    while (i < int_register_result_status.size())
    {
        cout << int_register_result_status[i] << " | ";
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

void Scoreboard::deep_copy(Scoreboard *score_in)
{
    instruction_status = score_in->instruction_status;
    fu_status = score_in->fu_status;
    floating_register_result_status = score_in->floating_register_result_status;
    int_register_result_status = score_in->int_register_result_status;
}
