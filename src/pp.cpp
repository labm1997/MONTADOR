#include <iostream>
#include <map>
#include "pp.hpp"
#include "statement.hpp"
#include "symbol.hpp"

void PreProcessor::dEQU(Statement stmt){
	int ok = 1;
	if(stmt.label.empty()){
		std::cout << "Syntax Error: EQU requires a label on line " << stmt.lineNumber << "\n";
		ok = 0;
	}
	if(stmt.arg[0].op1.empty()){
		std::cout << "Syntax Error: EQU requires one argument on line " << stmt.lineNumber << "\n";
		ok = 0;
	}
	if(ok){
		Symbol ns;
		ns.str = stmt.arg[0].op1;
		this->ppts.insert(std::pair<std::string, Symbol>(stmt.label, ns));
	}
}

void PreProcessor::dMACRO(Statement stmt){
	int ok = 1;
	if(stmt.label.empty()){
		std::cout << "Syntax Error: MACRO requires a label on line " << stmt.lineNumber << "\n";
		ok = 0;
	}
	if(stmt.arg[0].op != NONE || stmt.arg[1].op != NONE || stmt.arg[2].op != NONE){
		std::cout << "Syntax Error: Sum expression can't be macro argument on line " << stmt.lineNumber << "\n";
		ok = 0;
	}
	if(this->ppMNT.count(stmt.label) > 0){
		std::cout << "Semantic Error: Macro " << stmt.label << " already defined. Redefined on line " << stmt.lineNumber << "\n";
		ok = 0;
	}
	// !TODO: Detect number of arguments
	if(ok){
		this->mdt.nargs = stmt.countArgs();
		this->mdt.arg[0] = stmt.arg[0].op1;
		this->mdt.arg[1] = stmt.arg[1].op1;
		this->mdt.arg[2] = stmt.arg[2].op1;
		this->macrolabel = stmt.label;
		this->state = MACRO;
	}
	
}

void PreProcessor::dMACROEXPAND(Statement stmt){
	int ok = 1;
	this->mdt = this->ppMNT[stmt.op];
	if(stmt.countArgs() != this->mdt.nargs){
		std::cout << "Semantic Error: Macro " << stmt.op << " requires " << this->mdt.nargs << " arguments " << stmt.countArgs() << " given, on line " << stmt.lineNumber << "\n";
		ok = 0;
	}
	if(ok){
		std::map<std::string,Symbol> macroMap;
		for(int i=0 ; i<stmt.countArgs() ; i++){
			Symbol toinsert;
			toinsert.str = stmt.arg[i].op1;
			macroMap.insert(std::pair<std::string,Symbol>(this->mdt.arg[i], toinsert));
		}
		
		for(Statement &it : mdt.lstmt){
			Statement a = it.subst(macroMap).subst(ppts);
			this->pptoRender.push_back(a);
			//a.print();
		}
	}
}

void PreProcessor::dENDMACRO(Statement stmt){
	this->mdt.lstmt = this->macroList;
	this->ppMNT.insert(std::pair<std::string, MDT>(this->macrolabel, this->mdt));
	this->state = NORMAL;
	this->macroList.clear();
}

void PreProcessor::dAPPENDMACRO(Statement stmt){
	this->macroList.push_back(stmt);
}

void PreProcessor::renderStatements(std::list<Statement> statements){
	int ok;
	
	for(Statement &it: statements){
		switch(state){
			case NORMAL:
				if(it.op == "equ") this->dEQU(it);
				else if(it.op == "macro") this->dMACRO(it);
				else if(ppMNT.count(it.op) > 0) this->dMACROEXPAND(it);
			break;
			
			case MACRO:
				if(it.op == "endmacro") this->dENDMACRO(it);
				else this->dAPPENDMACRO(it);
			break;
		
		}
	}
}

std::list<Statement> PreProcessor::getResult(){
	return this->pptoRender;
}
