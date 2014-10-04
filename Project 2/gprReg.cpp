/*******
	General Purpose Register Machine Registers Simulation
	10/01/14
	Ben Gustafson
	COMP 4300
********/

#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>


#define REGISTER_LENGTH  32

using namespace std;

/*******
	Data Structures
********/
typedef uint32_t mem_addr;

mem_addr registers[REGISTER_LENGTH];


class Register_Bank
{
public:
	Register_Bank();
    bool write(mem_addr memory_address_in, mem_addr data);			//Writes to a register
    uint32_t read(mem_addr memory_address_in);						//Reads based on given memory address
    void print_memory();											//Prints out current memory state
};


/*******
	Class Definition 
********/

Register_Bank::Register_Bank()  													//Initialize memory
{
	//empty on purpose
}

bool Register_Bank::write(mem_addr memory_address_in, mem_addr data)		//Allows to right to registers
{
	if (memory_address_in > REGISTER_LENGTH)
	{
		cout << "Error: Register write is out of bounds." << endl;
		return false;
	}
	else
	{
		registers[memory_address_in] = data;
		return true;
	}
	cout << "Error: Register write went wrong." << endl;
	return false;
}


mem_addr Register_Bank::read(mem_addr memory_address_in )					//Allows cpu to read registers 
{
	if (memory_address_in > REGISTER_LENGTH)
	{
		cout << "Error: Register read is out of bounds." << endl;
		return false;
	}
	else
	{
		return registers[memory_address_in];
	}
	cout << "Error: Register read went wrong." << endl;
	return 0;
}

void Register_Bank::print_memory()										//To give a visual of the Register Memory space
{
	int memory_index = 0;
	cout <<	"==== REGISTERS ======================" << endl;
	while (memory_index < REGISTER_LENGTH)
	{
		if (registers[memory_index] != 0)
		{
			cout << memory_index << ":  " << std::dec << registers[memory_index] << endl;
		}
		memory_index++;
	}
	cout <<	"==========================" << endl;
}