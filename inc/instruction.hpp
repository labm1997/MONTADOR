#ifndef HEADER_INSTRUCTION
#define HEADER_INSTRUCTION

#include <string>

class Instruction {
	public:
	std::string mne;
	int size;
	int opcode;
	int nargs;
	Instruction(std::string, int, int, int);
	Instruction() { };
};


#endif
