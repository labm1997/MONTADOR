#ifndef HEADER_ASSEMBLER
#define HEADER_ASSEMBLER

#include <string>
#include "symbol.hpp"
#include "statement.hpp"

class Assembler {
	public:
	void generateSymbolTable(std::list<Statement>);
};

#endif
