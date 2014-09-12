/*******
	Stack Machine Memory Simulation
	09/07/14
	Ben Gustafson
	COMP 4300
********/

#include <cstdlib>
#include <stdio.h>
using namespace std;

#define TEXT_LENGTH  100;
#define	DATA_LENGTH  50;
#define STACK_LENGTH  50;


/*******
	Data Structures
********/
typedef uint32_t mem_addr;
typedef uint32_t instruction;


class Memory
{
public:
	Memory();
	bool load();
    bool write();
    mem_addr read();
private:
	int decode_address();
};


/*******
	Class Definition 
********/

Memory::Memory()  //Initialize memory
{
	//Kernal data starts at mem_addr 0, ommited because we don't use it in this simulation
	mem_addr text_top = 0x10000000;
	mem_addr data_top = 0x20000000;
	mem_addr stack_top = 0x30000000;
	
	instruction text_segment[TEXT_LENGTH];
	mem_addr data_segment[DATA_LENGTH];
	mem_addr stack_segment[STACK_LENGTH];
}
Memory::load()
{
	return false;
}
Memory::write()
{
	return false;
}
Memory::read()
{	
	return stack_top;
}




