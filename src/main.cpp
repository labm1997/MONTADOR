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

typedef enum {
	NORMAL, 
	MACRO
} ppState;

class Expression {
	public:
	string op1;
	string op2;
	operation op;
};

class Symbol {
	public:
	int value;
	string str;
};


class Statement {
	public:
	string line;
	string label;
	string op;
	Expression arg[3];
	string comment;
	int lineNumber;
	int countArgs(){
		if(this->arg[0].op1.empty()) return 0;
		else if(this->arg[1].op1.empty()) return 1;
		else if(this->arg[2].op1.empty()) return 2;
		else return 3;
	}
	Statement subst(map<string, Symbol> toSubst){
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
	void print(){
		cout << "Line: \"" << (*this).line << "\"\n";
		cout << "\tLine Number: " << (*this).lineNumber << "\n";
		cout << "\tLabel: \"" << (*this).label << "\"\n";
		cout << "\tOperation: \"" << (*this).op << "\"\n";
		cout << "\targ[0]_op1: \"" << (*this).arg[0].op1 << "\"\n";
		cout << "\targ[0]_op: " << ((*this).arg[0].op ? "SUM" : "NONE") << "\n";
		cout << "\targ[0]_op2: \"" << (*this).arg[0].op2 << "\"\n";
		cout << "\targ[1]_op1: \"" << (*this).arg[1].op1 << "\"\n";
		cout << "\targ[1]_op: " << ((*this).arg[1].op ? "SUM" : "NONE") << "\n";
		cout << "\targ[1]_op2: \"" << (*this).arg[1].op2 << "\"\n";
		cout << "\targ[2]_op1: \"" << (*this).arg[2].op1 << "\"\n";
		cout << "\targ[2]_op: " << ((*this).arg[2].op ? "SUM" : "NONE") << "\n";
		cout << "\targ[2]_op2: \"" << (*this).arg[2].op2 << "\"\n";
		cout << "\tComment: \"" << (*this).comment << "\"\n";
	}
};

class MDT {
	public:
	int nargs;
	string arg[3];
	list<Statement> lstmt;
};

string strToLower(string data){
	std::transform(data.begin(), data.end(), data.begin(), ::tolower);
	return data;
}

list<Statement> getStatementList(ifstream *file){
	regex lineRegex("(?:\\s|\\t)*(?:(\\w*):)?(?:\\s|\\t)*(?:(\\w*):)?(?:\\s|\\t)*(?:(?:\\w*):)*(?:(?:\\s|\\t)*([^\\s|\\t|;]*)(?:(?:\\s|\\t)*(?:(?:\\+)?((?:-)?[^\\s|\\t|\\+|,|;]*))(?:\\s|\\t)*(?:(\\+)(?:\\s|\\t)*([^\\s|\\t|,|;]*))?)?(?:,(?:\\s|\\t)*([^\\s|\\t|\\+|,|;]*)(?:(\\+)([^\\s|\\t|,|;]*))?)?(?:,(?:\\s|\\t)*([^\\s|\\t|\\+|,|;]*)(?:(\\+)([^\\s|\\t|;]*))?)?[^;]*)(?:;(.*))?"); //!TODO: Macro's args should begin with &
	
	regex replaceTabs("\\t");
	regex removeSpaces("(\\s){2,}");
	regex removeInitialSpace("^(\\s)");
	regex removeFinalSpace("(\\s)$");
	regex removeSpaceNotBetweenWordsRight("([^\\w])(\\s)");
	regex removeSpaceNotBetweenWordsLeft("(\\s)([^\\w])");
	
	string matchArgument = "(?:(?:\\+)?((?:-)?[^\\s|\\+|,|;]*))(?:(\\+)([^\\s|,|;]*))?";
	regex matchStatement("(?:(\\w*):)?([^\\s||;])\\s" + matchArgument + "(?:," + matchArgument + ")?(?:," + matchArgument + ")?[^;]*(?:;(.*))?");
	
	list<Statement> statements;
	smatch lineMatch;
	string line;
	string semespacos;

	for(int lineNumber=1 ; getline(*file, line) ; lineNumber++){
	
		semespacos = regex_replace(regex_replace(regex_replace(regex_replace(regex_replace(regex_replace(line, replaceTabs, " "), removeSpaces, " "), removeInitialSpace, ""), removeFinalSpace, ""), removeSpaceNotBetweenWordsRight, "$01"), removeSpaceNotBetweenWordsLeft, "$02");
		
		//cout << "Sem espaços: \"" <<  semespacos << "\"\n";
	
		if(regex_search(line, lineMatch, lineRegex)){
			Statement statement;
			Expression arg[3];
			statement.line = line;
			statement.label = strToLower(lineMatch.str(1));
			if(!lineMatch.str(2).empty()) 
				cout << "Syntax Error: Multiple labels in the same statement in line " << lineNumber << "\n";
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
	map<string, MDT> ppMNT;
	ppState state = NORMAL;
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
	for(Statement &it: statements){
		//it.print();
	}
	file.close();
	
	
	MDT mdt;
	list<Statement> macroList;
	string macrolabel;
	list<Statement> pptoRender;
	
	for(Statement &it: statements){
		switch(state){
			case NORMAL:
				ok = 1;
				if(it.op == "equ"){
					if(it.label.empty()){
						cout << "Syntax Error: EQU requires a label on line " << it.lineNumber << "\n";
						ok = 0;
					}
					if(it.arg[0].op1.empty()){
						cout << "Syntax Error: EQU requires one argument on line " << it.lineNumber << "\n";
						ok = 0;
					}
					if(ok){
						Symbol ns;
						ns.str = it.arg[0].op1;
						ppts.insert(pair<string, Symbol>(it.label, ns));
					}
				}
				else if(it.op == "macro"){
					if(it.label.empty()){
						cout << "Syntax Error: MACRO requires a label on line " << it.lineNumber << "\n";
						ok = 0;
					}
					if(it.arg[0].op != NONE || it.arg[1].op != NONE || it.arg[2].op != NONE){
						cout << "Syntax Error: Sum expression can't be macro argument on line " << it.lineNumber << "\n";
						ok = 0;
					}
					if(ppMNT.count(it.label) > 0){
						cout << "Semantic Error: Macro " << it.label << " already defined. Redefined on line " << it.lineNumber << "\n";
						ok = 0;
					}
					// !TODO: Detect number of arguments
					if(ok){
						mdt.nargs = it.countArgs();
						mdt.arg[0] = it.arg[0].op1;
						mdt.arg[1] = it.arg[1].op1;
						mdt.arg[2] = it.arg[2].op1;
						macrolabel = it.label;
						state = MACRO;
					}
					
				}
				
				else if(ppMNT.count(it.op) > 0){
					mdt = ppMNT[it.op];
					if(it.countArgs() != mdt.nargs){
						cout << "Semantic Error: Macro " << it.op << " requires " << mdt.nargs << " arguments " << it.countArgs() << " given, on line " << it.lineNumber << "\n";
						ok = 0;
					}
					if(ok){
						map<string,Symbol> macroMap;
						for(int i=0 ; i<it.countArgs() ; i++){
							Symbol toinsert;
							toinsert.str = it.arg[i].op1;
							macroMap.insert(pair<string,Symbol>(mdt.arg[i], toinsert));
						}
						
						for(Statement &it : mdt.lstmt){
							Statement a = it.subst(macroMap).subst(ppts);
							pptoRender.push_back(a);
							a.print();
						}
					}
				}
				
			break;
			case MACRO:
				if(it.op == "endmacro"){
					mdt.lstmt = macroList;
					ppMNT.insert(pair<string, MDT>(macrolabel, mdt));
					state = NORMAL;
					macroList.clear();
				}
				else macroList.push_back(it);
			break;
		
		}

	}

	
	
	return 0;
}
