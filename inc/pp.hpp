#ifndef HEADER_PP
#define HEADER_PP

#include <string>
#include <list>
#include <map>
#include "statement.hpp"


typedef enum {
	NORMAL, 
	MACRO
} ppState;

class MDT {
	public:
	int nargs;
	std::string arg[3];
	std::list<Statement> lstmt;
};

class PreProcessor {
	public:
	static std::list<Statement> renderStatements(std::list<Statement>);
};

#endif
