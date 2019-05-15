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

typedef enum {
	OK,
	ERROR
} error;

class Expression {
	public:
	std::string op1;
	std::string op2;
	operation op;
//	error eval(std::map<std::string, Symbol>, int *);
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
	Statement subst(SymbolTable);
	void print();
	static std::list<Statement> getStatementList(std::ifstream *file);
	static std::string renderStatementList(std::list<Statement>);
};

error strToInt(std::string, long int *);

#endif
