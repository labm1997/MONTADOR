#include "statement.hpp"
#include "symbol.hpp"
#include <iostream>
#include <string>
#include <regex>
#include <map>


std::string strToLower(std::string data){
	std::transform(data.begin(), data.end(), data.begin(), ::tolower);
	return data;
}

int Statement::countArgs(){
	if(this->arg[0].op1.empty()) return 0;
	else if(this->arg[1].op1.empty()) return 1;
	else if(this->arg[2].op1.empty()) return 2;
	else return 3;
}

Statement Statement::subst(std::map<std::string, Symbol> toSubst){
	Statement ret = *this;
	for(auto it = toSubst.begin() ; it != toSubst.end() ; it++){
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
	std::string matchArgument = "(?:(?:\\+)?((?:-)?[^\\s|\\+|,|;]*))(?:(\\+)([^\\s|,|;]*))?";
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
