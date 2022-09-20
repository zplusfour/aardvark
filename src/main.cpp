#include <cctype>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include "../include/colors.hpp"
#include "../include/interpreter.hpp"

Aardvark::AdkScope* globals = new Aardvark::AdkScope();

Aardvark::AdkValue* print(const std::vector<Aardvark::AdkValue*>& args) {
	const std::size_t argsSize = args.size();
	if (argsSize) {
		std::cout << args[0]->toString();
		for (std::size_t i = 1; i < argsSize; ++i) {
			std::cout << ' ' << args[i]->toString();
		}
	}
	std::cout << '\n';
	return new Aardvark::AdkValue();
};

Aardvark::AdkValue* input(const std::vector<Aardvark::AdkValue*>& args) {
	std::string input;
	print(args);
	std::getline(std::cin, input);
	return new Aardvark::AdkValue(input);
}

Aardvark::AdkValue* str(const std::vector<Aardvark::AdkValue*>& args) {
	return new Aardvark::AdkValue(args[0]->toString());
}

Aardvark::AdkValue* num(const std::vector<Aardvark::AdkValue*>& args) {
	return new Aardvark::AdkValue(std::stoi(args[0]->toString()));
}

Aardvark::AdkValue* lower(const std::vector<Aardvark::AdkValue*>& args) {
	std::string result = args[0]->toString();
	for (std::size_t i = 0; i < result.size(); ++i)
  		result[i] = std::tolower(result[i]);
	return new Aardvark::AdkValue(result);
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
		std::cerr << "Aardvark: No file specified!\n";
		return 1;
	};

	std::string fileData = readFile(argv[1]);

	Interpreter* interpreter = new Interpreter();
	globals->Set("output", new AdkFunction(interpreter, print));
	globals->Set("input", new AdkFunction(interpreter, input));
	globals->Set("str", new AdkFunction(interpreter, str));
	globals->Set("num", new AdkFunction(interpreter, num));
	globals->Set("lower", new AdkFunction(interpreter, lower));
	globals->Set("upper", new AdkFunction(interpreter, upper));

	interpreter->SetGlobals(globals);

	try {
		interpreter->Interpret(fileData, argv);
	} catch(exception& e) {
		std::cerr << e.what() << '\n';
	}

	// std::cout << "Function Name: " << ast->block[0]->value.getString() << std::endl;
	// std::cout << "Arg1: " << ast->block[0]->args[0]->value.getString() << std::endl;
}