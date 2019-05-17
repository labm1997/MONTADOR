#include <iostream>
#include <string>
#include <algorithm>
#include "symbol.hpp"
#include "statement.hpp"
#include "Assembler.hpp"
#include "instruction.hpp"

static std::map<std::string, Instruction> instructionMap = {
	{"add", Instruction("add", 1, 2, 1, {READ})},
	{"sub", Instruction("sub", 2, 2, 1, {READ})},
	{"mult", Instruction("mult", 3, 2, 1, {READ})},
	{"div", Instruction("div", 4, 2, 1, {READ})},
	{"jmp", Instruction("jmp", 5, 2, 1, {NON})},
	{"jmpn", Instruction("jmpn", 6, 2, 1, {NON})},
	{"jmpp", Instruction("jmpp", 7, 2, 1, {NON})},
	{"jmpz", Instruction("jmpz", 8, 2, 1, {NON})},
	{"copy", Instruction("copy", 9, 3, 2, {READ, WRITE})},
	{"load", Instruction("load", 10, 2, 1, {READ})},
	{"store", Instruction("store", 11, 2, 1, {WRITE})},
	{"input", Instruction("input", 12, 2, 1, {WRITE})},
	{"output", Instruction("output", 13, 2, 1, {READ})},
	{"stop", Instruction("stop", 14, 1, 0, {NON})}
};

SymbolTable Assembler::generateSymbolTable(std::list<Statement> lstmt){
	SymbolTable ts;
	bool textHappened = false;
	bool dataHappened = false;
	int address = 0;

	text_start = 0;
	text_end = -1;
	data_start = 0;
	data_end = -1;

	for(Statement &it : lstmt){
		if(!it.label.empty()){
			Symbol s;
			s.value = address;
			if(ts.symbolExist(it.label)){
				std::cout << "Semantic Error: Redefined label \"" << it.label << "\", in line " << it.lineNumber << "\n";
			}
			ts.insert(it.label, s);
		}
		// Instruction
		if(instructionMap.count(it.op) > 0){
			address += instructionMap[it.op].size;
		}
		// Directive
		else {
			if(it.op == "section"){
				if(it.countArgs() == 1 && it.arg[0].op == NONE){
					if(it.arg[0].op1 == "text"){
						if(textHappened){
							std::cout << "Semantic Error: Multiple TEXT sections defined, in line " << it.lineNumber << "\n";
						}
						if(dataHappened){
							data_end = address-1;
						}
						text_start = address;
						textHappened = true;
					}
					else if(it.arg[0].op1 == "data"){
						if(dataHappened){
							std::cout << "Semantic Error: Multiple DATA sections defined, in line " << it.lineNumber << "\n";
						}
						if(textHappened){
							text_end = address-1;
						}
						data_start = address;
						dataHappened = true;
					}
					else {
						std::cout << "Syntax Error: invalid argument for section: \"" << it.arg[0].op1 << "\", in line " << it.lineNumber << "\n";
					}
				}
				if(it.countArgs() != 1) {
					std::cout << "Syntax Error: wrong number of arguments, in line " << it.lineNumber << "\n";
				}
				if(it.arg[0].op != NONE) {
					std::cout << "Syntax Error: sum expression not supported for section directive, in line " << it.lineNumber << "\n";
				}
			}
			else if(it.op == "space"){
				long int nspaces;
				switch(it.arg[0].evalUnary(&nspaces)){
					case EVAL_UNARY_OK:
						for(int i=0 ; i < nspaces ; i++){
							dataAddress.push_back(address+i);
						}
						address += nspaces;
					break;
					case EVAL_UNARY_ERROR_EMPTY:
						dataAddress.push_back(address);
						address += 1;
					break;
					case EVAL_UNARY_ERROR_CONVERTION:
						std::cout << "Syntax Error: invalid argument expected integer, in line " << it.lineNumber << "\n";
					break;
					case EVAL_UNARY_ERROR_NOT_UNARY:
						std::cout << "Syntax Error: argument can't be evaluated on space directive, in line " << it.lineNumber << "\n";
					break;
				}
			}
			else if(it.op == "const"){
				readAddress.push_back(address);
				address += 1;
			}
			else {
				std::cout << "Semantic Error: \"" << it.op << "\" directive or instruction invalid, in line: " << it.lineNumber << "\n";
			}
		}
	}

	if(text_end == -1){
		text_end = address-1;
	}

	if(data_end == -1){
		data_end = address-1;
	}

	if(!textHappened){
		std::cout << "Semantic Error: no TEXT section\n";
	}
	return ts;
}

bool Assembler::validDataAddress(long int arg){
	return std::find(dataAddress.begin(), dataAddress.end(), arg) != dataAddress.end();
}

bool Assembler::validReadAddress(long int arg){
	return (std::find(readAddress.begin(), readAddress.end(), arg) != readAddress.end()) || validDataAddress(arg);
}

std::string Assembler::generateObjectCode(SymbolTable ts, std::list<Statement> lstmt){
	std::string out;
	int address = 0;

	for(Statement &it : lstmt){

		// Instruction
		if(instructionMap.count(it.op) > 0){

			if(!(address >= text_start && address <= text_end)){
				std::cout << "Semantic Error: Invalid section, instruction out of TEXT section, in line " << it.lineNumber << "\n";
			}

			Instruction in = instructionMap[it.op];
			int nargs_stmt = it.countArgs();
			out += std::to_string(in.opcode) + " ";
			address += instructionMap[it.op].size;

			if(in.nargs == nargs_stmt){
				for(int i = 0 ; i < nargs_stmt ; i++){
					int arg;
					switch(it.arg[i].eval(ts, &arg)){
						case EVAL_OK:
							if(it.op == "div" && arg == 0){
								std::cout << "Syntax Error: Division by zero, in line " << it.lineNumber << "\n";
							}
							if(in.access[i] == WRITE && !validDataAddress(arg)){
								std::cout << "Semantic Error: Modify readonly (const) address is not allowed, in line " << it.lineNumber << "\n";
							}
							if(in.access[i] == READ && !validReadAddress(arg)){
								std::cout << "Semantic Error: Access to forbidden address, in line " << it.lineNumber << "\n";
							}
							if(it.op == "jmp" || it.op == "jmpn" || it.op == "jmpp" || it.op == "jmpz"){
								if(!(arg >= text_start && arg <= text_end)){
									std::cout << "Semantic Error: Jump out of TEXT section, in line " << it.lineNumber << "\n";
								}
							}
							out += std::to_string(arg) + " ";
						break;
						case EVAL_ERROR_CONVERTION:
							std::cout << "Syntax Error: Expected integer, label given as second operand in line " << it.lineNumber << "\n";
						break;
						case EVAL_ERROR_EMPTY:
							std::cout << "Internal Error: it should not happend\n";
						break;
						case EVAL_ERROR_SYMBOL_NOT_FOUND:
							std::cout << "Semantic Error: Symbol \"" << it.arg[i].op1 << "\" not defined, in line " << it.lineNumber << "\n";
						break;
					}
				}

			}
			else {
				std::cout << "Syntax Error: \"" << it.op << "\" requires " << in.nargs << " arguments, " << nargs_stmt << " given, in line " << it.lineNumber << "\n";
			}
		}
		// Directive
		else {
			if(it.op == "section");

			else if(it.op == "space"){
				if(!(address >= data_start && address <= data_end)){
					std::cout << "Semantic Error: Invalid section, instruction out of DATA section, in line " << it.lineNumber << "\n";
				}
				long int nspaces;
				switch(it.arg[0].evalUnary(&nspaces)){
					case EVAL_UNARY_OK:
						for(int i=0 ; i < nspaces ; i++) out += "00 ";
						address += nspaces;
					break;
					case EVAL_UNARY_ERROR_EMPTY:
						out += "00 ";
						address += 1;
					break;
					default:

					break;
				}
			}
			else if(it.op == "const"){
				if(!(address >= data_start && address <= data_end)){
					std::cout << "Semantic Error: Invalid section, instruction out of DATA section, in line " << it.lineNumber << "\n";
				}
				address += 1;

				if(it.countArgs() == 1 && it.arg[0].op == NONE){
					long int value;
					switch(it.arg[0].evalUnary(&value)){
						case EVAL_UNARY_OK:
							out += std::to_string(value) + " ";
						break;
						case EVAL_UNARY_ERROR_EMPTY:
							std::cout << "Internal Error: it should not happend\n";
						break;
						case EVAL_UNARY_ERROR_CONVERTION:
							std::cout << "Syntax Error: invalid argument expected integer, in line " << it.lineNumber << "\n";
						break;
						case EVAL_UNARY_ERROR_NOT_UNARY:
							std::cout << "Syntax Error: argument can't be evaluated on const directive, in line " << it.lineNumber << "\n";
						break;
					}
				}
				if(it.countArgs() != 1) {
					std::cout << "Syntax Error: wrong number of arguments, in line " << it.lineNumber << "\n";
				}
				if(it.arg[0].op != NONE) {
					std::cout << "Syntax Error: sum expression not supported for const directive, in line " << it.lineNumber << "\n";
				}

			}
		}
	}

	return out;
}

