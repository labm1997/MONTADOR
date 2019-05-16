#include <iostream>
#include <string>
#include "symbol.hpp"
#include "statement.hpp"
#include "Assembler.hpp"
#include "instruction.hpp"

static std::map<std::string, Instruction> instructionMap = {
	{"add", Instruction("add", 1, 2, 1)},
	{"sub", Instruction("sub", 2, 2, 1)},
	{"mult", Instruction("mult", 3, 2, 1)},
	{"div", Instruction("div", 4, 2, 1)},
	{"jmp", Instruction("jmp", 5, 2, 1)},
	{"jmpn", Instruction("jmpn", 6, 2, 1)},
	{"jmpp", Instruction("jmpp", 7, 2, 1)},
	{"jmpz", Instruction("jmpz", 8, 2, 1)},
	{"copy", Instruction("copy", 9, 3, 2)},
	{"load", Instruction("load", 10, 2, 1)},
	{"store", Instruction("store", 11, 2, 1)},
	{"input", Instruction("input", 12, 2, 1)},
	{"output", Instruction("output", 13, 2, 1)},
	{"stop", Instruction("stop", 14, 1, 0)}
};

void Assembler::generateSymbolTable(std::list<Statement> lstmt){
	SymbolTable ts;
	int address = 0;
	int textAddress = 0;
	int dataAddress = 0;
	
	for(Statement &it : lstmt){
		if(!it.label.empty()){
			Symbol s;
			s.value = address;
			ts.insert(it.label, s);
		}
		// Instruction
		if(instructionMap.count(it.op) > 0){
			address += instructionMap[it.op].size;
		}
		// Directive
		else {
			if(it.op == "section");
			else if(it.op == "space"){
				// !TODO
			}
			else if(it.op == "const"){
				address += 1;
			}
			else {
				std::cout << "Semantic Error: \"" << it.op << "\" directive or instruction invalid, in line: " << it.lineNumber << "\n";
			}
		}
	}
}

//{
//				if(it.countArgs() == 1 && it.arg[0].op == NONE){
//					if(it.arg[0].op1 == "text"){
//						textAddress = address;
//					}
//					else if(it.arg[0].op1 == "data"){
//						dataAddress = address;
//					}
//					else {
//						std::cout << "Syntax Error: invalid argument for section: \"" << it.arg[0].op1 << "\", in line " << it.lineNumber << "\n";
//					}
//				}
//				if(it.countArgs() != 1) {
//					std::cout << "Syntax Error: wrong number of arguments, in line " << it.lineNumber << "\n";
//				}
//				if(it.arg[0].op != NONE) {
//					std::cout << "Syntax Error: sum expression not supported for section directive, in line " << it.lineNumber << "\n";
//				}
//			}
