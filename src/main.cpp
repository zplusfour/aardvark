#include <iostream>
#include <vector>
#include "../include/interpreter.hpp"
#include "../include/colors.hpp"
#include <sstream>
#include <typeinfo>
using namespace Aardvark;

AdkScope* globals = new AdkScope();

AdkValue* print(std::vector<AdkValue*> args) {
	std::string end = "\n";
	std::string seperator = " ";

	bool isFirst = true;
	
	for (auto arg : args) {
		if (isFirst) isFirst = false;
		else std::cout << seperator;

		std::cout << arg->toString();
	}

	std::cout << end;

	return new AdkValue();
};

AdkValue* input(std::vector<AdkValue*> args) {

	std::string i;
	std::cout << args[0]->toString();
	
	std::getline(std::cin, i);

	return new AdkValue(i);
}

AdkValue* str(std::vector<AdkValue*> args) {
	std::string s = args[0]->toString();
	std::stringstream ss;
	ss << s;
	std::string x;
	ss >> x;

	return new AdkValue(x);
}

AdkValue* num(std::vector<AdkValue*> args) {
	std::string s = args[0]->toString();
	std::stringstream toint(s);

	int n = 0;
	toint >> n;

	return new AdkValue(n);
}

int main(int argc, char** argv)
{
	// Lexer lexer("output(\"Hello, World\")");
  // vector<Token> tokens;
	// try {
	// 	tokens = lexer.tokenize();
	// } catch(Aardvark::Error& e) {
	// 	std::cerr
	// 		<< Colors::Red
	// 		<< e.type << ": " << e.what()
  //     << Colors::Reset
	// 		<< std::endl;
	// };

	// // Uncomment for debugging tokens
  // for (auto& token : tokens) {
  //   token.debugPrint();
  // }

	if (argc < 2) {
		std::cerr << "Aardvark: No file specified!" << std::endl;
		return 1;
	};

	std::string fileData = readFile(argv[1]);

	Interpreter* interpreter = new Interpreter();
	globals->Set("output", new AdkFunction(interpreter, print));
	globals->Set("input", new AdkFunction(interpreter, input));
	globals->Set("str", new AdkFunction(interpreter, str));
	globals->Set("num", new AdkFunction(interpreter, num));

	interpreter->SetGlobals(globals);

	try {
		interpreter->Interpret(fileData);
	} catch(Error& e) {
		std::cerr
			<< Colors::Red
			<< e.type << ": " << e.what()
			<< Colors::Reset
			<< std::endl;
	}

	// std::cout << "Function Name: " << ast->block[0]->value.getString() << std::endl;
	// std::cout << "Arg1: " << ast->block[0]->args[0]->value.getString() << std::endl;
}