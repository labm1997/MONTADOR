#include <iostream>
#include <fstream>
#include <regex>
#include <list>

using namespace std;

typedef enum {
	NONE,
	SUM
} operation;

typedef struct {
	string op1;
	string op2;
	operation op;
} Expression;

typedef struct {
	string line;
	string label;
	string op;
	Expression arg1;
	Expression arg2;
	string comment;
	int lineNumber;
} Statement;

list<Statement> getStatementList(ifstream *file){
	regex lineRegex("(?:\\s|\\t)*(?:(\\w*):)?(?:(?:\\s|\\t)*(\\w*)(?:(?:\\s|\\t)*(\\w*)(?:(\\+)(\\w*))?)?(?:,(?:\\s|\\t)*(\\w*)(?:(\\+)(\\w*))?)?[^;]*)(?:;(.*))?");
	list<Statement> statements;
	smatch lineMatch;
	string line;

	while(getline(*file, line)){
		if(regex_search(line, lineMatch, lineRegex)){
			Statement statement;
			Expression arg1, arg2;
			statement.line = line;
			statement.label = lineMatch.str(1);
			statement.op = lineMatch.str(2);
			arg1.op1 = lineMatch.str(3);
			if(!lineMatch.str(4).empty() && !lineMatch.str(5).empty()){
				arg1.op = SUM;
				arg1.op2 = lineMatch.str(5);
			}
			else arg1.op = NONE;
			arg2.op1 = lineMatch.str(6);
			if(!lineMatch.str(7).empty() && !lineMatch.str(8).empty()){
				arg1.op = SUM;
				arg1.op2 = lineMatch.str(8);
			}
			else arg2.op = NONE;
			statement.arg1 = arg1;
			statement.arg2 = arg2;
			statement.comment = lineMatch.str(9);
			statements.push_back(statement);
		}
		else {
			cout << "\tINVALID SYNTAX\n";
		}
	}
	return statements;
}

int main(int argc, char** argv) {
	ifstream file;
	list<Statement> statements;
	
	if(argc < 2) {
		cout << "Usage: ./" << argv[0] << " FILE\n";
		return 1;
	}
	file.open(argv[1]);
	if(!file.is_open()) {
		cout << "Failed to open \"" << argv[1] << "\"\n";
		return 1;
	}
	statements = getStatementList(&file);
	for(auto it=statements.begin(); it!=statements.end() ; ++it){
		cout << "Line: \"" << it->line << "\"\n";
		cout << "\tLabel: \"" << it->label << "\"\n";
		cout << "\tOperation: \"" << it->op << "\"\n";
		cout << "\tArg1_op1: \"" << it->arg1.op1 << "\"\n";
		cout << "\tArg1_op: " << (it->arg1.op ? "SUM" : "NONE") << "\n";
		cout << "\tArg1_op2: \"" << it->arg1.op2 << "\"\n";
		cout << "\tArg2_op1: \"" << it->arg2.op1 << "\"\n";
		cout << "\tArg2_op: " << (it->arg2.op ? "SUM" : "NONE") << "\n";
		cout << "\tArg2_op2: \"" << it->arg2.op2 << "\"\n";
		cout << "\tComment: \"" << it->comment << "\"\n";
	}
	file.close();
	return 0;
}