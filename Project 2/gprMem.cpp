/*******
	General Purpose Register Machine Memory Simulation
	10/01/14
	Ben Gustafson
	COMP 4300
********/

#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>


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

//Kernal data starts at mem_addr 0, ommited because we don't use it in this simulation
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
    mem_addr * read(mem_addr memory_address_in);					//Reads based on given memory address
    void print_memory();											//Prints out current memory state
private:
	int decode_address_bin(mem_addr memory_address_in);				//Helps decode address into bin
	int decode_address_index(mem_addr memory_address_in);			//Helps decode address into array index
	int text_next_open_memory_location;								//Internal counter for text_segment
};


/*******
	Class Definition 
********/

Memory::Memory()  													//Initialize memory
{
	text_next_open_memory_location = -1;
	int hexidecimal;
	int hexidecimal2;
	int hexidecimal3;
	string line;
	string line2 = "0000000000";
	string line3 = "0";
	int i = 0;
	ifstream accum_file_code ("accumCode.txt");
	if (accum_file_code.is_open())
	{
		while ( getline (accum_file_code,line))
		{
			if (line == ""){continue;}
			if (line == ".data"){i = 1; continue;}
			if (line == ".text"){continue;}	
			if (i == 0) 												//Text
			{
				hexidecimal = std::stoi(line.c_str(),0, 16);
				load_code(hexidecimal);
			}
			if (i == 1) 												//Data
			{
				for (int c = 0; c < 10; c++)
				{
					line2[c] = line[c];
					line3[0] = line[11];
				}
				hexidecimal2 = std::stoi(line2.c_str(),0, 16);
				hexidecimal3 = atoi(line3.c_str());
				load_data(hexidecimal2,hexidecimal3);
			}	
		}
	}
	else{
		cout << "Error: Unable to open file."; 
	}
	accum_file_code.close();
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
		cout << "Error: Please expand space for Text Memory." << endl;
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
		cout << "Error: Please expand space for Data Memory." << endl;
		return false;														//No More memory open
	}
}


bool Memory::write(mem_addr memory_address_in, mem_addr data)
{
	mem_addr memory_copy_bin = memory_address_in, memory_copy_index = memory_address_in;
	switch(decode_address_bin(memory_copy_bin))
	{
	case 1:
			cout << "Error: You do not have the correct user privileges to write to text segment." << endl;
			return false;
		break;
	case 2:
			cout << "Error: You do not have the correct user privileges to write to data segment." << endl;
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
				cout << "Error: Please expand space for Stack Memory" << endl;
				return false;														//No More stack open
			}
		}
		break;
	default:
			cout << "Error: You cannot write to that memory area." << endl;
			return false;															//Not in current memory
		break;
	}
	cout << "Error: Memory write went wrong." << endl;
	return false;
}


mem_addr * Memory::read(mem_addr memory_address_in )
{	
	mem_addr memory_copy_bin = memory_address_in, memory_copy_index = memory_address_in;
	switch(decode_address_bin(memory_copy_bin))
	{
	case 1:
		{
			int memory_index = (int) decode_address_index(memory_copy_index);									
			if (memory_index < TEXT_LENGTH)											//Checks text memory length
			{
				return &text_segment[memory_index];
			}
		}
		break;
	case 2:
		{
			int memory_index = (int) decode_address_index(memory_copy_index);
			if (text_next_open_memory_location < DATA_LENGTH)						//Checks data memory length
			{
				return &data_segment[memory_index];									
			}
		}
		break;
	case 3:
		{
			int memory_index = (int) decode_address_index(memory_copy_index);
			if (text_next_open_memory_location < STACK_LENGTH)						//Checks stack memory length
			{
				return &stack_segment[memory_index];									
			}
		}
		break;
	default:
			cout << "Error: Memory read is not within current memory." << endl;
			return &stack_top;														//Not in current memory space
		break;
	}
	cout << "Error: Memory read went wrong." << endl;
	return &stack_top;
}

int Memory::decode_address_bin(mem_addr memory_address_in)
{																//Wipes out everything but the bin bits
	memory_address_in = memory_address_in << 7;
	memory_address_in = memory_address_in >> 27;
	return memory_address_in;
	//(-1) -- false
	// 0--kernal
	// 1--text
	// 2--data
	// 3--stack
}

int Memory::decode_address_index(mem_addr memory_address_in)
{																//Removes the (potential) op code and bin
	memory_address_in = memory_address_in << 15;
	memory_address_in = memory_address_in >> 15;
	return memory_address_in;
}

void Memory::print_memory()										//To give a visual of the memory space
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
	cout <<	"==== STACK ========================================================" << endl;
	while (memory_index < STACK_LENGTH)
	{
		cout << "  " << std::dec << stack_segment[memory_index] << endl;
		memory_index++;
	}
	cout <<	"==========================" << endl;
}