#ifndef HEADER_SYMBOL
#define HEADER_SYMBOL

#include <string>
#include <map>

class Symbol {
	public:
	int value;
	std::string str;
};

class SymbolTable {
	public:
	std::map<std::string, Symbol> ts;
	void insert(std::string, Symbol);
	bool symbolExist(std::string);
	Symbol getSymbol(std::string);
};

#endif
