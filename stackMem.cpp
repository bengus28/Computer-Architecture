/*******
	Stack Machine Memory Simulation
	09/07/14
	Ben Gustafson
	COMP 4300
********/

#include <cstdlib>
#include <stdio.h>
#include <iostream>

#define TEXT_LENGTH  100
#define	DATA_LENGTH  50
#define STACK_LENGTH  50

using namespace std;




/*******
	Data Structures
********/
typedef uint32_t mem_addr;
typedef uint32_t instruction;

mem_addr text_top = 0x00100000;
mem_addr data_top = 0x00200000;
mem_addr stack_top = 0x00300000;

instruction text_segment[TEXT_LENGTH];
mem_addr data_segment[DATA_LENGTH];
mem_addr stack_segment[STACK_LENGTH];


class Memory
{
public:
	Memory();
	bool load_code(mem_addr memory_address_in);						//Loads from .text section
	bool load_data(mem_addr memory_address_in, mem_addr data);		//Loads from .data section
    bool write(mem_addr memory_address_in, mem_addr data);			//Writes to stack section
    mem_addr read(mem_addr memory_address_in);						//Reads based on memory address
    void print_memory();											//Prints out current memory state
private:
	int decode_address_bin(mem_addr memory_address_in);
	int decode_address_index(mem_addr memory_address_in);
	int text_next_open_memory_location;
};

int main()
{
	Memory *mem = new Memory();
	
	mem->load_code(0x01200000);
	mem->load_code(0x01200000);
	mem->load_code(0x04000000);
	mem->load_code(0x01200001);
	mem->load_code(0x03000000);
	mem->load_code(0x01200003);
	mem->load_data(0x00200000,3);
	mem->load_data(0x00200001,7);
	mem->load_data(0x00200002,5);
	mem->load_data(0x00200003,4);
	mem->print_memory();
	return 0;
}
/*******
	Class Definition 
********/

Memory::Memory()  //Initialize memory
{
	//Kernal data starts at mem_addr 0, ommited because we don't use it in this simulation
	text_next_open_memory_location = -1;
}
bool Memory::load_code(mem_addr memory_address_in)
{
	text_next_open_memory_location++;										
	if (text_next_open_memory_location < TEXT_LENGTH)						//Checks memory length
	{
		text_segment[text_next_open_memory_location] = memory_address_in;	//Stores instruction
		return true;
	}
	else
	{		
		return false;														//No More memory open
	}
}
bool Memory::load_data(mem_addr memory_address_in, mem_addr data)
{
	mem_addr memory_copy_index = memory_address_in;
	
	int memory_index = (int) decode_address_index(memory_copy_index);
	if (text_next_open_memory_location < DATA_LENGTH)						//Checks memory length
	{
		data_segment[memory_index] = data;									//Stores data
		return true;
	}
	else
	{		
		return false;														//No More memory open
	}
}
bool Memory::write(mem_addr memory_address_in, mem_addr data)
{
	mem_addr memory_copy_bin = memory_address_in, memory_copy_index = memory_address_in;
	switch(decode_address_bin(memory_copy_bin))
	{
	case 1:
			return false;
		break;
	case 2:
			return false;
		break;
	case 3:
		{
			int memory_index = (int) decode_address_index(memory_copy_index);
			if (text_next_open_memory_location < STACK_LENGTH)						//Checks memory length
			{
				stack_segment[memory_index] = data;									//Store data in stack
				return true;
			}
			else
			{		
				return false;														//No More stack open
			}
		}
		break;
	default:
			return false;															//Not in current memory
		break;
	}
	return false;
}
mem_addr Memory::read(mem_addr memory_address_in )
{	
	mem_addr memory_copy_bin = memory_address_in, memory_copy_index = memory_address_in;
	switch(decode_address_bin(memory_copy_bin))
	{
	case 1:
		{
			int memory_index = (int) decode_address_index(memory_copy_index);									
			if (memory_index < TEXT_LENGTH)											//Checks memory length
			{
				return text_segment[memory_index];
			}
		}
		break;
	case 2:
		{
			int memory_index = (int) decode_address_index(memory_copy_index);
			if (text_next_open_memory_location < DATA_LENGTH)						//Checks memory length
			{
				return data_segment[memory_index];									
			}
		}
		break;
	case 3:
		{
			int memory_index = (int) decode_address_index(memory_copy_index);
			if (text_next_open_memory_location < STACK_LENGTH)						//Checks memory length
			{
				return stack_segment[memory_index];									
			}
		}
		break;
	default:
			return stack_top;														//Not in current memory
		break;
	}
	return stack_top;
}
//General funciton to decode the memory addresses coming in and give the correct "bin" where it is stored
//(-1) -- false
// 0--kernal
// 1--text
// 2--data
// 3--stack
int Memory::decode_address_bin(mem_addr memory_address_in)
{
	memory_address_in = memory_address_in << 9;
	memory_address_in = memory_address_in >> 31;
	return memory_address_in;
}
int Memory::decode_address_index(mem_addr memory_address_in)
{
	memory_address_in = memory_address_in << 15;
	memory_address_in = memory_address_in >> 15;
	return memory_address_in;
}
void Memory::print_memory()
{
//text
	int memory_index = 0;
	cout <<	"==== TEXT ======================" << endl;
	while (memory_index < TEXT_LENGTH)
	{
		cout << "  " << std::hex << text_segment[memory_index] << endl;
		memory_index++;
	}
	cout <<	"==========================" << endl;
//data 
	memory_index = 0;
	cout <<	"==== DATA ======================" << endl;
	while (memory_index < DATA_LENGTH)
	{
		cout << "  " << data_segment[memory_index] << endl;
		memory_index++;
	}
	cout <<	"==========================" << endl;
//stack
	memory_index = 0;
	cout <<	"==== STACK ======================" << endl;
	while (memory_index < STACK_LENGTH)
	{
		cout << "  " << stack_segment[memory_index] << endl;
		memory_index++;
	}
	cout <<	"==========================" << endl;
}
