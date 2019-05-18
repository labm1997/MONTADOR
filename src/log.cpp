#include "log.hpp"
#include <iostream>
#include <string>

#define UI_BOLD_RED_BACK_RED "\x1B[1;37;41m"
#define UI_BOLD_RED "\x1B[1;31m"
#define UI_BOLD "\x1B[1m"
#define UI_NORMAL  "\x1B[0m"

void logPrintLines(int ppLine, int defLine, int oLine){
	std::cout << "\tin line " << UI_BOLD << ppLine << UI_NORMAL << " on preprocessed file\n";
	if(defLine != oLine) std::cout << "\tdefined in line " << UI_BOLD << defLine << UI_NORMAL << " on source file\n";
	std::cout << "\tin line " << UI_BOLD << oLine << UI_NORMAL << " on source file\n";
}

void logPrintLines(int oLine){
	std::cout << "\tin line " << UI_BOLD << oLine << UI_NORMAL << " on source file\n";
}

void logSemanticError(std::string message, int ppLine, int defLine, int oLine){
	std::cout << UI_BOLD_RED << "Assembler Semantic Error" << ": " << UI_NORMAL << message << "\n";
	logPrintLines(ppLine, defLine, oLine);
}

void logSemanticError(std::string message){
	std::cout << UI_BOLD_RED << "Assembler Semantic Error" << ": " << UI_NORMAL;
	std::cout << message << "\n";
}

void logSyntaxError(std::string message, int ppLine, int defLine, int oLine){
	std::cout << UI_BOLD_RED << "Assembler Syntax Error" << ": " << UI_NORMAL << message << "\n";
	logPrintLines(ppLine, defLine, oLine);
}

void logInternalError(int ppLine, int defLine, int oLine){
	std::cout << UI_BOLD_RED_BACK_RED << "Assembler Internal Error" << UI_NORMAL;
	logPrintLines(ppLine, defLine, oLine);
}

void logppSyntaxError(std::string message, int lineNumber){
	std::cout << UI_BOLD_RED << "PreProcessor Syntax Error" << ": " << UI_NORMAL << message << "\n";
	logPrintLines(lineNumber);
}

void logppSemanticError(std::string message, int lineNumber){
	std::cout << UI_BOLD_RED << "PreProcessor Semantic Error" << ": " << UI_NORMAL << message << "\n";
	logPrintLines(lineNumber);
}

void logppSemanticError(std::string message){
	std::cout << UI_BOLD_RED << "PreProcessor Semantic Error" << ": " << UI_NORMAL << message << "\n";
}

void logppLexicalError(std::string message, int lineNumber){
	std::cout << UI_BOLD_RED << "PreProcessor Lexical Error" << ": " << UI_NORMAL << message << "\n";
	logPrintLines(lineNumber);
}

void logparserSyntaxError(std::string message, int lineNumber){
	std::cout << UI_BOLD_RED << "Parser Syntax Error" << ": " << UI_NORMAL << message << "\n";
	logPrintLines(lineNumber);
}
