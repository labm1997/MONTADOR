#ifndef HEADER_STATEMENT
#define HEADER_STATEMENT

#include <string>
#include <map>
#include <list>
#include <fstream>
#include "symbol.hpp"

typedef enum {
	NONE,
	SUM
} operation;

class Expression {
	public:
	std::string op1;
	std::string op2;
	operation op;
};

class Statement {
	public:
	std::string line;
	std::string label;
	std::string op;
	Expression arg[3];
	std::string comment;
	int lineNumber;
	int countArgs();
	Statement subst(std::map<std::string, Symbol>);
	void print();
	static std::list<Statement> getStatementList(std::ifstream *file);
};

#endif
