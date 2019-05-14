#include <iostream>
#include <map>
#include "pp.hpp"
#include "statement.hpp"
#include "symbol.hpp"

std::list<Statement> PreProcessor::renderStatements(std::list<Statement> statements){
	std::map<std::string, Symbol> ppts;
	std::map<std::string, MDT> ppMNT;
	std::list<Statement> pptoRender;
	std::list<Statement> macroList;
	std::string macrolabel;
	ppState state = NORMAL;
	MDT mdt;
	int ok;
	
	for(Statement &it: statements){
		switch(state){
			case NORMAL:
				ok = 1;
				if(it.op == "equ"){
					if(it.label.empty()){
						std::cout << "Syntax Error: EQU requires a label on line " << it.lineNumber << "\n";
						ok = 0;
					}
					if(it.arg[0].op1.empty()){
						std::cout << "Syntax Error: EQU requires one argument on line " << it.lineNumber << "\n";
						ok = 0;
					}
					if(ok){
						Symbol ns;
						ns.str = it.arg[0].op1;
						ppts.insert(std::pair<std::string, Symbol>(it.label, ns));
					}
				}
				else if(it.op == "macro"){
					if(it.label.empty()){
						std::cout << "Syntax Error: MACRO requires a label on line " << it.lineNumber << "\n";
						ok = 0;
					}
					if(it.arg[0].op != NONE || it.arg[1].op != NONE || it.arg[2].op != NONE){
						std::cout << "Syntax Error: Sum expression can't be macro argument on line " << it.lineNumber << "\n";
						ok = 0;
					}
					if(ppMNT.count(it.label) > 0){
						std::cout << "Semantic Error: Macro " << it.label << " already defined. Redefined on line " << it.lineNumber << "\n";
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
						std::cout << "Semantic Error: Macro " << it.op << " requires " << mdt.nargs << " arguments " << it.countArgs() << " given, on line " << it.lineNumber << "\n";
						ok = 0;
					}
					if(ok){
						std::map<std::string,Symbol> macroMap;
						for(int i=0 ; i<it.countArgs() ; i++){
							Symbol toinsert;
							toinsert.str = it.arg[i].op1;
							macroMap.insert(std::pair<std::string,Symbol>(mdt.arg[i], toinsert));
						}
						
						for(Statement &it : mdt.lstmt){
							Statement a = it.subst(macroMap).subst(ppts);
							pptoRender.push_back(a);
							//a.print();
						}
					}
				}
				
			break;
			case MACRO:
				if(it.op == "endmacro"){
					mdt.lstmt = macroList;
					ppMNT.insert(std::pair<std::string, MDT>(macrolabel, mdt));
					state = NORMAL;
					macroList.clear();
				}
				else macroList.push_back(it);
			break;
		
		}
	}
	return pptoRender;
}
