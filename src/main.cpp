#include <iostream>
#include <vector>
#include "../include/interpreter.hpp"
#include "../include/colors.hpp"
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

	interpreter->SetGlobals(globals);
	interpreter->Interpret(fileData);

	// std::cout << "Function Name: " << ast->block[0]->value.getString() << std::endl;
	// std::cout << "Arg1: " << ast->block[0]->args[0]->value.getString() << std::endl;
}