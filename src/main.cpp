#include <iostream>
#include <fstream>
#include <regex>
#include <list>
#include <algorithm>
#include <string>
#include <map>

using namespace std;

typedef enum {
	NONE,
	SUM
} operation;

class Expression {
	public:
	string op1;
	string op2;
	operation op;
};

class Statement {
	public:
	string line;
	string label;
	string op;
	Expression arg1;
	Expression arg2;
	string comment;
	int lineNumber;
};

class Symbol {
	public:
	int value;
};

class MDT {
	public:
	int nargs;
	list<Statement> lstmt;
}

string strToLower(string data){
	std::transform(data.begin(), data.end(), data.begin(), ::tolower);
	return data;
}

list<Statement> getStatementList(ifstream *file){
	regex lineRegex("(?:\\s|\\t)*(?:(\\w*):)?(?:\\s|\\t)*(?:(\\w*):)?(?:\\s|\\t)*(?:(?:\\w*):)*(?:(?:\\s|\\t)*([^\\s|\\t|;]*)(?:(?:\\s|\\t)*(?:(?:\\+)?((?:-)?[^\\s|\\t|\\+|,|;]*))(?:\\s|\\t)*(?:(\\+)(?:\\s|\\t)*([^\\s|\\t|,|;]*))?)?(?:,(?:\\s|\\t)*([^\\s|\\t|\\+|;]*)(?:(\\+)([^\\s|\\t|;]*))?)?[^;]*)(?:;(.*))?");
	list<Statement> statements;
	smatch lineMatch;
	string line;

	for(int lineNumber=1 ; getline(*file, line) ; lineNumber++){
		if(regex_search(line, lineMatch, lineRegex)){
			Statement statement;
			Expression arg1, arg2;
			statement.line = line;
			statement.label = strToLower(lineMatch.str(1));
			if(!lineMatch.str(2).empty()) 
				cout << "Syntax Error: Multiple labels in the same statement in line " << lineNumber << "\n";
			statement.op = strToLower(lineMatch.str(3));
			arg1.op1 = strToLower(lineMatch.str(4));
			if(!lineMatch.str(5).empty() && !lineMatch.str(6).empty()){
				arg1.op = SUM;
				arg1.op2 = strToLower(lineMatch.str(6));
			}
			else arg1.op = NONE;
			arg2.op1 = strToLower(lineMatch.str(7));
			if(!lineMatch.str(8).empty() && !lineMatch.str(9).empty()){
				arg2.op = SUM;
				arg2.op2 = strToLower(lineMatch.str(9));
			}
			else arg2.op = NONE;
			statement.arg1 = arg1;
			statement.arg2 = arg2;
			statement.comment = lineMatch.str(10);
			statement.lineNumber = lineNumber;
			
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
	list<Statement> ppStatements;
	map<string, Symbol> ppts;
	map<string, MDT> ppMDT;
	char ok;
	
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
//	for(Statement &it: statements){
//		cout << "Line: \"" << it.line << "\"\n";
//		cout << "\tLine Number: " << it.lineNumber << "\n";
//		cout << "\tLabel: \"" << it.label << "\"\n";
//		cout << "\tOperation: \"" << it.op << "\"\n";
//		cout << "\tArg1_op1: \"" << it.arg1.op1 << "\"\n";
//		cout << "\tArg1_op: " << (it.arg1.op ? "SUM" : "NONE") << "\n";
//		cout << "\tArg1_op2: \"" << it.arg1.op2 << "\"\n";
//		cout << "\tArg2_op1: \"" << it.arg2.op1 << "\"\n";
//		cout << "\tArg2_op: " << (it.arg2.op ? "SUM" : "NONE") << "\n";
//		cout << "\tArg2_op2: \"" << it.arg2.op2 << "\"\n";
//		cout << "\tComment: \"" << it.comment << "\"\n";
//	}
	file.close();
	
	
	for(Statement &it: statements){
		if(it.op == "equ"){
			if(it.label.empty()){
				cout << "Syntax Error: EQU requires a label on line " << it.lineNumber << "\n";
				ok = 0;
			}
			if(it.arg1.op1.empty()){
				cout << "Syntax Error: EQU requires one argument on line " << it.lineNumber << "\n";
				ok = 0;
			}
			if(ok){
				Symbol ns;
				ns.value = atoi(it.arg1.op1.c_str());
				ppts.insert(pair<string, Symbol>(it.label, ns));
			}
		}
		else if(it.op == "macro"){
			if(it.label.empty()){
				cout << "Syntax Error: MACRO requires a label on line " << it.lineNumber << "\n";
				ok = 0;
			}
			
		}
	}
	
	
	return 0;
}
