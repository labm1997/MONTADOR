#include <iostream>
#include "symbol.hpp"
#include "statement.hpp"

void SymbolTable::insert(std::string label, Symbol s){
	this->ts.insert(std::pair<std::string,Symbol>(label, s));
}

bool SymbolTable::symbolExist(std::string label){
	return this->ts.count(label) > 0;
}

Symbol SymbolTable::getSymbol(std::string label){
	return this->ts[label];
}
