/*******
	Stack Machine Memory Simulation
	09/07/14
	Ben Gustafson
	COMP 4300
********/

#include <cstdlib>
#include <stdio.h>

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
	bool load();
    bool write();
    mem_addr read();
private:
	int decode_address_bin(mem_addr memory_address_in);
	int decode_address_index(mem_addr memory_address_in);
};

int main()
{
	Memory *mem = new Memory();
	return 0;
}
/*******
	Class Definition 
********/

Memory::Memory()  //Initialize memory
{
	//Kernal data starts at mem_addr 0, ommited because we don't use it in this simulation
	int text_next_open_memory_location =0;
}
bool Memory::load(mem_addr memory_address_in)
{
	mem_addr memory_copy_bin = memory_address_in, memory_copy_index = memory_address_in;
	swtich(decode_address_bin(memory_address_bin))
	{
	case:1
		text_next_open_memory_location++;										
		if text_next_open_memory_location < TEXT_LENGTH							//Checks memory length
		{
			text_segment[text_next_open_memory_location] = memory_address_in;	//Stores instruction
			return true;
		}
		else
		{		
			return false;														//No More memory open
		}
		break;
	case:2
		int index = decode_address_bin(memory_address_in);
		if text_next_open_memory_location < DATA_LENGTH							//Checks memory length
		{
			data_segment[index] = memory_address_in;							//Stores data
			return true;
		}
		else
		{		
			return false;														//No More memory open
		}
		break;
	case:3
			return false;
		break;														//No More stack open
	default:
		return false;															//Not in current memory
		break;
	}
	return false;
}
bool Memory::write()
{
	mem_addr memory_copy_bin = memory_address_in, memory_copy_index = memory_address_in;
	swtich(decode_address_bin(memory_address_bin))
	{
	case:1
		text_next_open_memory_location++;										
		if text_next_open_memory_location < TEXT_LENGTH							//Checks memory length
		{
			text_segment[text_next_open_memory_location] = memory_address_in;	//Stores instruction
			return true;
		}
		else
		{		
			return false;														//No More memory open
		}
		break;
	case:2
		int index = decode_address_bin(memory_address_in);
		if text_next_open_memory_location < DATA_LENGTH							//Checks memory length
		{
			data_segment[index] = memory_address_in;							//Stores data
			return true;
		}
		else
		{		
			return false;														//No More memory open
		}
		break;
	case:3
		int index = decode_address_bin(memory_address_in);
		if text_next_open_memory_location < STACK_LENGTH						//Checks memory length
		{
			stack_segment[index] = memory_address_in;							//Store data in stack
			return true;
		}
		else
		{		
			return false;														//No More stack open
		}
		break;
	default:
		return false;															//Not in current memory
		break;
	}
	return false;
}
mem_addr Memory::read()
{	
	return stack_top;
}
//General funciton to decode the memory addresses coming in and give the correct "bin" where it is stored
//(-1) -- false
// 0--kernal
// 1--text
// 2--data
// 3--stack
int Memory::decode_address(mem_addr memory_address_in)
{
	memory_address_in << 9;
	memory_address_in >> 31;
	return memory_addres_in;
}
int Memory::decode_address_index(mem_addr memory_address_in)
{
	memory_address_in << 13;
	memory_address_in >> 13;
	return memory_addres_in;
}
