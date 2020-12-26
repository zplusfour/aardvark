#include <iostream>
#include <vector>
#include "../include/parser.hpp"
#include "../include/colors.hpp"
using namespace Aardvark;

int main()
{
	Lexer lexer("output(\"Hello, World\")");
  vector<Token> tokens;
	try {
		tokens = lexer.tokenize();
	} catch(Aardvark::Error& e) {
		cerr
			<< Colors::Red
			<< e.type << ": " << e.what()
      << Colors::Reset
			<< endl;
	}

  // Uncomment for debugging tokens
  // for (auto& token : tokens) {
  //   token.debugPrint();
  // }
}