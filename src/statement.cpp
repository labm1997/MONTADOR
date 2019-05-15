#include "statement.hpp"
#include "symbol.hpp"
#include <iostream>
#include <string>
#include <regex>
#include <map>

// !TODO: Don't put numbers on TS!!!!

//eval_error eval(std::map<std::string, Symbol> ts, int *ret){
//	int op1;
//	error e = strToInt(this.op1, &op1);
//}

std::string strToLower(std::string data){
	std::transform(data.begin(), data.end(), data.begin(), ::tolower);
	return data;
}

error strToInt(std::string str, long int *ret){
	char *c = (char *)str.c_str();
	char *a = c;
	bool hexa = false;
	
	if(*c == '-') c++;
	// Hexa
	if(*c == '0' && *(c+1) == 'x') {
		hexa = true;
		c = c+2;
	}
	
	while(*c != '\0'){
		if(!((*c >= '0' && *c <= '9') || (hexa && (*c >= 'a' && *c <= 'f')) ))
			return ERROR;
		c++;
	}
	
	if(ret) {
		*ret = hexa ? strtol(a, NULL, 16) : strtol(a, NULL, 10);
	}
	
	return OK;
}

int Statement::countArgs(){
	if(this->arg[0].op1.empty()) return 0;
	else if(this->arg[1].op1.empty()) return 1;
	else if(this->arg[2].op1.empty()) return 2;
	else return 3;
}

Statement Statement::subst(SymbolTable toSubst){
	Statement ret = *this;
	for(auto it = toSubst.ts.begin() ; it != toSubst.ts.end() ; it++){
		for(int i=0 ; i<3 ; i++){
			if(ret.arg[i].op1 == it->first)
				ret.arg[i].op1 = it->second.str;
			if(ret.arg[i].op2 == it->first)
				ret.arg[i].op2 = it->second.str;
		}
	}
	return ret;
}

void Statement::print(){
	std::cout << "Line: \"" << (*this).line << "\"\n";
	std::cout << "\tLine Number: " << (*this).lineNumber << "\n";
	std::cout << "\tLabel: \"" << (*this).label << "\"\n";
	std::cout << "\tOperation: \"" << (*this).op << "\"\n";
	std::cout << "\targ[0]_op1: \"" << (*this).arg[0].op1 << "\"\n";
	std::cout << "\targ[0]_op: " << ((*this).arg[0].op ? "SUM" : "NONE") << "\n";
	std::cout << "\targ[0]_op2: \"" << (*this).arg[0].op2 << "\"\n";
	std::cout << "\targ[1]_op1: \"" << (*this).arg[1].op1 << "\"\n";
	std::cout << "\targ[1]_op: " << ((*this).arg[1].op ? "SUM" : "NONE") << "\n";
	std::cout << "\targ[1]_op2: \"" << (*this).arg[1].op2 << "\"\n";
	std::cout << "\targ[2]_op1: \"" << (*this).arg[2].op1 << "\"\n";
	std::cout << "\targ[2]_op: " << ((*this).arg[2].op ? "SUM" : "NONE") << "\n";
	std::cout << "\targ[2]_op2: \"" << (*this).arg[2].op2 << "\"\n";
	std::cout << "\tComment: \"" << (*this).comment << "\"\n";
}

std::list<Statement> Statement::getStatementList(std::ifstream *file){
	std::string rejectSpaces = "(?:\\s|\\t)*";
	std::string matchLabel = rejectSpaces + "(?:(\\w*):)?" + rejectSpaces + "(?:(\\w*):)?" + rejectSpaces + "(?:(?:\\w*):)*" + rejectSpaces;
	std::string matchOperation = "([^\\s|\\t|;]*)";
	std::string matchFirstOp = rejectSpaces + "(?:(?:\\+)?((?:-)?[^\\s|\\t|\\+|,|;]*))" + rejectSpaces;
	std::string matchArg1 = "(?:"  + matchFirstOp + "(?:(\\+)" + rejectSpaces + "([^\\s|\\t|,|;]*))?)?";
	std::string matchArg2 = "(?:," + matchFirstOp + "(?:(\\+)" + rejectSpaces + "([^\\s|\\t|,|;]*))?)?";
	std::string matchArg3 = "(?:," + matchFirstOp + "(?:(\\+)" + rejectSpaces + "([^\\s|\\t|;]*))?)?";
	std::string matchRest = "[^;]*(?:;(.*))?";
	std::regex matchStatement(matchLabel + matchOperation + matchArg1 + matchArg2 + matchArg3 + matchRest);
	
	std::list<Statement> statements;
	std::smatch lineMatch;
	std::string line;

	for(int lineNumber=1 ; std::getline(*file, line) ; lineNumber++){
	
		if(std::regex_search(line, lineMatch, matchStatement)){
			Statement statement;
			Expression arg[3];
			statement.line = line;
			statement.label = strToLower(lineMatch.str(1));
			if(!statement.label.empty() && strToInt(statement.label, NULL) == OK)
				std::cout << "Syntax Error: Label is an integer in line " << lineNumber << "\n";
			if(!lineMatch.str(2).empty()) 
				std::cout << "Syntax Error: Multiple labels in the same statement in line " << lineNumber << "\n";
			statement.op = strToLower(lineMatch.str(3));
			arg[0].op1 = strToLower(lineMatch.str(4));
			if(!lineMatch.str(5).empty() && !lineMatch.str(6).empty()){
				arg[0].op = SUM;
				arg[0].op2 = strToLower(lineMatch.str(6));
			}
			else arg[0].op = NONE;
			arg[1].op1 = strToLower(lineMatch.str(7));
			if(!lineMatch.str(8).empty() && !lineMatch.str(9).empty()){
				arg[1].op = SUM;
				arg[1].op2 = strToLower(lineMatch.str(9));
			}
			else arg[1].op = NONE;
			arg[2].op1 = strToLower(lineMatch.str(10));
			if(!lineMatch.str(11).empty() && !lineMatch.str(12).empty()){
				arg[2].op = SUM;
				arg[2].op2 = strToLower(lineMatch.str(12));
			}
			else arg[2].op = NONE;
			statement.arg[0] = arg[0];
			statement.arg[1] = arg[1];
			statement.arg[2] = arg[2];
			statement.comment = lineMatch.str(13);
			statement.lineNumber = lineNumber;
			//statement.print();
			statements.push_back(statement);
		}
		else {
			std::cout << "\tINVALID SYNTAX\n";
		}
	}
	return statements;
}


std::string Statement::renderStatementList(std::list<Statement> lstmt){
	std::string out;
	std::string lastLabel;
	bool unusedLastLabel = false;
	for(Statement &it : lstmt){
		if(!it.label.empty()){
			lastLabel = it.label;
			if(it.op.empty()) unusedLastLabel = true;
			else out +=  it.label + ": ";
		}
		if(!it.op.empty()){
			if(unusedLastLabel) {
				unusedLastLabel = false;
				out += lastLabel + ": ";
			}
			out += it.op;
		}
		for(int i=0 ; i<3 ; i++){
			if(!it.arg[i].op1.empty()){
				if(i != 0) out += ",";
				out += " " + it.arg[i].op1;
			}
			if(it.arg[i].op == SUM && !it.arg[i].op2.empty()){
				out += "+" + it.arg[i].op2;
			}
		}
		if(!it.op.empty()) out += "\n";
	}
	return out;
}
