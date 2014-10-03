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
#include <math.h>


#define TEXT_LENGTH  100
#define	DATA_LENGTH  50
#define STACK_LENGTH  50


using namespace std;

/*******
	Data Structures
********/
typedef uint32_t mem_addr;
typedef uint32_t instruction;

mem_addr text_top = 0x00001000;
mem_addr data_top = 0x00002000;
mem_addr stack_top = 0x00003000;

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
    bool load_string(mem_addr m_add, char string_to_be_stored[]);	//Write given string to memory
    string read_string(mem_addr memory_address);					//Read a string from memory
    mem_addr read_byte(mem_addr memory_address_in, int byte);		//Reads byte at given address
    void print_memory();											//Prints out current memory state
private:
	int decode_address_bin(mem_addr memory_address_in);				//Helps decode address into bin
	int decode_address_index(mem_addr memory_address_in);			//Helps decode address into array index
	int text_next_open_memory_location;								//Internal counter for text_segment
	int length_of_string(mem_addr memory_address_in, int max_length);//Helper to find the end of string in memory.
	mem_addr mem_byte(instruction data_in,int byte_number);		//returns a byte inside the instruction.
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
	string textLine;
	string dataAddress = "0000000000";
	string data = "0000000000000000000000000000000000000";
	char dataArray[37];
	int i = 0;
	ifstream gpr_file_code ("gprCode.txt");
	if (gpr_file_code.is_open())
	{
		while ( getline (gpr_file_code,textLine))
		{
			if (textLine == ""){continue;}
			if (textLine == ".data"){i = 1; continue;}
			if (textLine == ".text"){continue;}	
			if (i == 0) 												//Text
			{
				hexidecimal = std::stoi(textLine.c_str(),0, 16);
				cout << textLine + "\n";
				//read_string(hexidecimal);
			}
			if (i == 1) 												//Data
			{
				for (int c = 0; c < 10; c++){dataAddress[c] = textLine[c];}
				for (int c = 0;c < 37; c++){
					if (data[c] == NULL){data[c] = 0;}
					data[c] = textLine[c+11];
				}
				hexidecimal2 = std::stoi(dataAddress.c_str(),0, 16);
				hexidecimal3 = atoi(data.c_str());
				cout << dataAddress + "\n";
				cout << data + "\n";
				//load_string(hexidecimal2, hexidecimal3)
				
			}	
		}
	}
	else{
		cout << "Error: Unable to open file."; 
	}
	gpr_file_code.close();
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
			if (memory_index < STACK_LENGTH)										//Checks memory length
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


mem_addr * Memory::read(mem_addr memory_address_in)
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
			if (memory_index < DATA_LENGTH)											//Checks data memory length
			{
				return &data_segment[memory_index];									
			}
		}
		break;
	case 3:
		{
			int memory_index = (int) decode_address_index(memory_copy_index);
			if (memory_index < STACK_LENGTH)										//Checks stack memory length
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
	memory_address_in = memory_address_in << 16;
	memory_address_in = memory_address_in >> 28;
	return memory_address_in;
	//(-1) -- false
	// 0--kernal
	// 1--text
	// 2--data
	// 3--stack
}

int Memory::decode_address_index(mem_addr memory_address_in)
{																//Removes the (potential) op code and bin
	memory_address_in = memory_address_in << 20;
	memory_address_in = memory_address_in >> 20;
	return memory_address_in;
}

bool Memory::load_string(mem_addr m_add, char string_to_be_stored[])	//Write given string to memory
{
	switch(decode_address_bin(m_add))
	{
		case 1: //TEXT 
		{ 
			cout << "Error: There was an error loading your string into TEXT segment of memory." << endl;
			return false;
		}
		case 2: //DATA
		{ 
			int data_index = decode_address_index(m_add);
			if (data_index < DATA_LENGTH)											
			{
				memcpy(&data_segment[data_index], string_to_be_stored,  strlen(string_to_be_stored)+1);
				return true;									
			}
			return false;
		}
		case 3: //STACK
		{ 
			int data_index = decode_address_index(m_add);
			if (data_index < STACK_LENGTH)											
			{
				memcpy(&stack_segment[data_index], string_to_be_stored,  strlen(string_to_be_stored)+1);
				return true;
			}
			return false;
		}
		default :
		{
			cout << "Error: There was an error loading your string into memory, Bin not selected" << endl;
			return false;
		}
	}
	cout << "Error: There was an error loading your string into memory" << endl;
	return false;
}

string Memory::read_string(mem_addr memory_address)
{
	switch(decode_address_bin(memory_address))
	{
		case 1: //TEXT 
		{ 
			cout << "Error: There was an error loading your string into TEXT segment of memory." << endl;
			return "Error";
		}
		case 2: //DATA
		{ 
			int data_index = decode_address_index(memory_address);
			char *data_out;
			data_out = (char*) malloc( length_of_string(memory_address, 2000));
			if (data_index < DATA_LENGTH)											
			{
				memcpy(data_out, &data_segment[data_index],  length_of_string(memory_address, 2000));
				return string(data_out);									
			}
			return "Error";
		}
		case 3: //STACK
		{ 
			int data_index = decode_address_index(memory_address);
			char *data_out;
			data_out = (char*) malloc( length_of_string(memory_address, 2000));
			if (data_index < STACK_LENGTH)											
			{
				memcpy(data_out, &stack_segment[data_index],  length_of_string(memory_address, 2000));
				return string(data_out);									
			}
			return "Error";
		}
		default :
		{
			cout << "Error: There was an error loading your string into memory, Bin not selected" << endl;
			return "Error";
		}
	}
	cout << "Error: There was an error loading your string into memory" << endl;
	return "Error";
}

int Memory::length_of_string(mem_addr memory_address_in, int max_length)		//Helper to find the end of string in memory.
{
	switch(decode_address_bin(memory_address_in))
	{
		case 1: //TEXT 
		{ 
			cout << "Error: There was an error finding the length of a string in TEXT segment of memory." << endl;
			return 0;
		}
		case 2: //DATA
		{ 
			int data_index = decode_address_index(memory_address_in);
			if (data_index < DATA_LENGTH)											//Checks data memory length
			{
				bool end_not_found = true;
				int length =0;
				mem_addr current_byte=0;
				while(end_not_found && length < max_length)
				{
					current_byte = mem_byte( data_segment[data_index], 1+(length %4));
					if(0 == current_byte)
					{
						end_not_found = false;
					}
					length++;
				}
				return length--;
			}
			return 0;
		}
		case 3: //STACK
		{ 
			int data_index = decode_address_index(memory_address_in);
			if (data_index < STACK_LENGTH)											//Checks data memory length
			{
				bool end_not_found = true;
				int length =0;
				mem_addr current_byte =0;
				while(end_not_found && length < max_length)
				{
					current_byte = mem_byte(stack_segment[data_index], 1+(length %4));
					if(0 == current_byte)
					{
						end_not_found = false;
					}
					length++;
				}
				return length--;
				
			}
			return 0;
		}
		default :
		{
			cout << "Error: There was an error finding the length of your string, Bin not selected" << endl;
			return 0;
		}
	}
	cout << "Error: There was an error finding the length of your string in memory" << endl;
	return 0;
}

mem_addr Memory::mem_byte(instruction data_in, int byte_number)
{																//Returns a byte inside the instruction.					
	if (byte_number < 5 && byte_number > 0)
	{
		byte_number --;
		instruction data;
		data = data_in;
		data = data << 8*byte_number;
		data = data >> 24;
		return data;
	}
	cout << "Error: Memory byte read" << endl;
	return 0;
} 

mem_addr Memory::read_byte(mem_addr memory_address_in, int byte)
{
	mem_addr memory_copy_bin = memory_address_in, memory_copy_index = memory_address_in;
	int memory_index = (int) decode_address_index(memory_copy_index);
	memory_index = (int) floor(memory_index/4.0);	
	mem_addr memory_value;
	switch(decode_address_bin(memory_copy_bin))
	{
	case 1:
		{						
			if (memory_index < TEXT_LENGTH)											//Checks text memory length
			{
				memory_value= text_segment[memory_index];
			}
		}
		break;
	case 2:
		{
			if (memory_index < DATA_LENGTH)											//Checks data memory length
			{
				memory_value= data_segment[memory_index];									
			}
		}
		break;
	case 3:
		{
			if (memory_index < STACK_LENGTH)										//Checks stack memory length
			{
				memory_value= stack_segment[memory_index];									
			}
		}
		break;
	default:
			cout << "Error: Memory read is not within current memory." << endl;
			memory_value= stack_top;														//Not in current memory space
		break;
	}
	return mem_byte(memory_value, byte);
}
		
void Memory::print_memory()									//To give a visual of the memory space
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