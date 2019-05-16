#include <iostream>
#include <map>
#include "pp.hpp"
#include "statement.hpp"
#include "symbol.hpp"

void PreProcessor::dEQU(Statement stmt){
	int ok = 1;
	long int value;
	if(stmt.label.empty()){
		std::cout << "Syntax Error: EQU requires a label in line " << stmt.lineNumber << "\n";
		ok = 0;
	}
	if(stmt.arg[0].op1.empty()){
		std::cout << "Syntax Error: EQU requires one argument in line " << stmt.lineNumber << "\n";
		ok = 0;
	}
	if(strToInt(stmt.arg[0].op1, &value) == ERROR){
		std::cout << "Syntax Error: EQU requires integer argument in line " << stmt.lineNumber << "\n";
		ok = 0;
	}
	if(ok){
		Symbol ns;
		ns.str = stmt.arg[0].op1;
		ns.value = value;
		this->ppts.insert(stmt.label, ns);
	}
}

void PreProcessor::dMACRO(Statement stmt){
	int ok = 1;
	int nargs = stmt.countArgs();
	if(stmt.label.empty()){
		std::cout << "Syntax Error: MACRO requires a label in line " << stmt.lineNumber << "\n";
		ok = 0;
	}
	if(stmt.arg[0].op != NONE || stmt.arg[1].op != NONE || stmt.arg[2].op != NONE){
		std::cout << "Syntax Error: Sum expression can't be macro argument in line " << stmt.lineNumber << "\n";
		ok = 0;
	}
	if(this->ppMNT.count(stmt.label) > 0){
		std::cout << "Semantic Error: Macro " << stmt.label << " already defined. Redefined in line " << stmt.lineNumber << "\n";
		ok = 0;
	}
	for(int i=0 ; i<nargs ; i++){
		if(stmt.arg[i].op1[0] != '&'){
			std::cout << "Lexical Error: expected & on token \"" << stmt.arg[i].op1[0] << "\", in line " << stmt.lineNumber << "\n";
			ok = 0;
		}
	}
	// !TODO: Detect number of arguments
	if(ok){
		this->mdt.nargs = nargs;
		this->mdt.arg[0] = stmt.arg[0].op1;
		this->mdt.arg[1] = stmt.arg[1].op1;
		this->mdt.arg[2] = stmt.arg[2].op1;
		this->macrolabel = stmt.label;
	}
	
	this->state = MACRO;
	
}

void PreProcessor::dMACROEXPAND(Statement stmt){
	int ok = 1;
	this->mdt = this->ppMNT[stmt.op];
	if(stmt.countArgs() != this->mdt.nargs){
		std::cout << "Semantic Error: Macro " << stmt.op << " requires " << this->mdt.nargs << " arguments " << stmt.countArgs() << " given, in line " << stmt.lineNumber << "\n";
		ok = 0;
	}
	if(ok){
		SymbolTable macroMap;
		for(int i=0 ; i<stmt.countArgs() ; i++){
			Symbol toinsert;
			toinsert.str = stmt.arg[i].op1;
			macroMap.insert(this->mdt.arg[i], toinsert);
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

void PreProcessor::dIF(Statement stmt){
	int ok = 1;
	if(stmt.countArgs() != 1){
		std::cout << "Syntax Error: wrong number of arguments given for IF, " << stmt.countArgs() << " given but 1 required, in line " << stmt.lineNumber << "\n";
		ok = 0;
	}
	if(!this->ppts.symbolExist(stmt.arg[0].op1)){
		std::cout << "Semantic Error: Symbol \"" << stmt.arg[0].op1 << " not defined, you should define it with EQU before, in line " << stmt.lineNumber << "\n";
		ok = 0;
	}
	if(ok){
		if(this->ppts.getSymbol(stmt.arg[0].op1).value == 0){
			this->state = IFFALSE;
		}
	}
}

void PreProcessor::dAPPEND(Statement stmt){
	Statement a = stmt.subst(ppts);
	this->pptoRender.push_back(a);
}

void PreProcessor::renderStatements(std::list<Statement> statements){
	
	for(Statement &it: statements){
		switch(state){
			case NORMAL:
				if(it.op == "equ") this->dEQU(it);
				else if(it.op == "macro") this->dMACRO(it);
				else if(it.op == "if") this->dIF(it);
				else if(ppMNT.count(it.op) > 0) this->dMACROEXPAND(it);
				else this->dAPPEND(it);
			break;
			
			case MACRO:
				if(it.op == "endmacro") this->dENDMACRO(it);
				else this->dAPPENDMACRO(it);
			break;
			
			case IFFALSE:
				state = NORMAL;
			break;
		
		}
	}
	
	if(state == MACRO){
		std::cout << "Sintax Error: Missing ENDMACRO directive\n";
	}
}

std::list<Statement> PreProcessor::getResult(){
	bool unusedLastLabel = false;
	std::list<Statement> lstmt;
	std::string lastLabel;
	for(Statement &it : this->pptoRender){
		if(!it.label.empty()){
			if(it.op.empty()) unusedLastLabel = true;
			lastLabel = it.label;
		}
		if(!it.op.empty()){
			if(unusedLastLabel){
				if(it.label.empty()){
					it.label = lastLabel;
				}
				else {
					std::cout << "Semantic Error: Multiple labels for the same operation in line " << it.lineNumber << ", using \"" << it.label << "\"\n";
				}
				unusedLastLabel = false;
			}
			lstmt.push_back(it);
		}
	}
	return lstmt;
}


