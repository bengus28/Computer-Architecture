/*******
    General Purpose Register Machine Floating Point Registers Simulation
    12/6/14
    Ben Gustafson
    COMP 4300
********/

#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>


#define REGISTER_LENGTH_FLOAT  16

using namespace std;

/*******
    Data Structures
********/
typedef float float_mem;
typedef uint32_t mem_addr;

float_mem floating_registers[REGISTER_LENGTH_FLOAT];


class Floating_Point_Register_Bank
{
public:
    Floating_Point_Register_Bank();
    bool write(mem_addr memory_address_in, float_mem data);			//Writes to a register
    float_mem read(mem_addr memory_address_in);						//Reads based on given memory address
    void print_memory();											//Prints out current memory state
};


/*******
    Class Definition
********/

Floating_Point_Register_Bank::Floating_Point_Register_Bank()  													//Initialize memory
{
    //empty on purpose
}

bool Floating_Point_Register_Bank::write(mem_addr memory_address_in, float_mem data)		//Allows to right to registers
{
    //cout << "Register Write Location: " << memory_address_in << " data: " << data << endl;
    if (memory_address_in > REGISTER_LENGTH_FLOAT)
    {
        cout << "Error: Register write is out of bounds." << endl;
        return false;
    }
    else
    {
        floating_registers[memory_address_in] = data;
        return true;
    }
    cout << "Error: Register write went wrong." << endl;
    return false;
}


float_mem Floating_Point_Register_Bank::read(mem_addr memory_address_in )					//Allows cpu to read registers
{
    //cout << "Register Read Location: " << memory_address_in << " data: " << floating_registers[memory_address_in] << endl;
    if (memory_address_in > REGISTER_LENGTH_FLOAT)
    {
        cout << "Error: Register read is out of bounds." << endl;
        return false;
    }
    else
    {
        return floating_registers[memory_address_in];
    }
    cout << "Error: Register read went wrong." << endl;
    return 0;
}

void Floating_Point_Register_Bank::print_memory()										//To give a visual of the Register Memory space
{
    int memory_index = 0;
    cout <<	"==== Floating Point Registers ===" << endl;
    while (memory_index < REGISTER_LENGTH_FLOAT)
    {
        if (floating_registers[memory_index] != 0)
        {
            cout << memory_index << ":  " << std::dec << floating_registers[memory_index] << endl;
        }
        memory_index++;
    }
    cout <<	"=================================" << endl;
}
