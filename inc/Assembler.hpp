#ifndef HEADER_ASSEMBLER
#define HEADER_ASSEMBLER

#include <list>
#include <string>
#include "symbol.hpp"
#include "statement.hpp"

typedef enum {
	SECTION_NONE,
	SECTION_TEXT,
	SECTION_DATA
} section;

class Assembler {
	public:
	int text_start, text_end;
	int data_start, data_end;
	std::list<long int> dataAddress;
	std::list<long int> readAddress;
	SymbolTable generateSymbolTable(std::list<Statement>);
	std::string generateObjectCode(SymbolTable, std::list<Statement>);
	bool validDataAddress(long int);
	bool validReadAddress(long int);
};

#endif
