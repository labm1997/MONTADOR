#include <iostream>
#include <fstream>
#include <regex>

using namespace std;

int main(int argc, char** argv) {
	ifstream file;
	string line;
	string comments;
	regex lineRegex("((\\w*):)?(\\s*(\\w*)(\\s*(\\w*))?(,\\s*(\\w*))?[^;]*)(;(.*))?");
	smatch lineMatch;
	
	if(argc < 2) {
		cout << "Usage: ./" << argv[0] << " FILE\n";
		return 1;
	}
	file.open(argv[1]);
	if(!file.is_open()) {
		cout << "Failed to open \"" << argv[1] << "\"\n";
		return 1;
	}
	while(getline(file, line)){
		cout << "Line: \"" << line << "\"\n";
		if(regex_search(line, lineMatch, lineRegex)){
			cout << "\tLabel: \"" << lineMatch.str(2) << "\"\n"; 
			cout << "\tOperation: \"" << lineMatch.str(4) << "\"\n";
			cout << "\tArg1: \"" << lineMatch.str(6) << "\"\n"; 
			cout << "\tArg2: \"" << lineMatch.str(8) << "\"\n"; 
			cout << "\tComment: \"" << lineMatch.str(10) << "\"\n"; 
		}
		else {
			cout << "\tINVALID SYNTAX\n";
		}
	}
	file.close();
	return 0;
}
