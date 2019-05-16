#include "instruction.hpp"

Instruction::Instruction(std::string _mne, int _opcode, int _size, int _nargs){
	mne = _mne;
	opcode = _opcode;
	size = _size;
	nargs = _nargs;
}
