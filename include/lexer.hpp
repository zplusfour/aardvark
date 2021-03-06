#include <iostream>
#include <vector>
#include <unordered_map>
#include "errors.hpp"
#include "filesystem.hpp"

/*
This is a very very simple lexer don't judge. (I didn't judge)
using the vector class.
*/

namespace Aardvark {
	using namespace std;

	// Why is this wrrrrooongng
	unordered_map<string, string> keywords = {
		{ "True", "Keyword" },
		{ "False", "Keyword" },
		{ "if", "Keyword" },
		{ "while", "Keyword" },
		{ "funct", "Keyword" },
		{ "return", "Keyword" }
	};
	class Token {
		public:
		string type; // To keep track of what the token is
		void* value; // We would have to cast the value to the correct type

		int column = 1;
		int line = 1;

		Token() { // Just a null token that you can create
			type = "None";
			value = nullptr;
		};

		Token(string type, string val): type(type) {
			value = new string(val); // Making a copy because we could get errors
		};

		Token(string type, const char* val): type(type) {
			value = new string(val); // Making a copy because we could get errors
		};

		Token(string type, char val): type(type) {
			value = new string(1, val);
		};
    Token(std::string type, int val): type(type) {
      value = new int(val);
    }
    Token(std::string type, double val): type(type) {
      value = new double(val);
    }

		Token(string val, bool isIdentifier = true) {
			type = isIdentifier ? "Identifier" : "String";
			value = new string(val);
		};

		Token(string type, void* value): type(type), value(value) {};

		void setPosition(int col, int ln) {
			column = col;
			line = ln;
		}

		string getString() {
			return *(string*)value;
		}

		int getInt(bool cast = false) {
			if (cast)
				return std::stoi(getString());
			return *(int*)value;
		}

		double getDouble() {
			return *(double*)value;
		}

		bool getBool() {
			return *(bool*)value;
		}

		bool isNull() { return type == "None"; };

		void debugPrint() {
			cout <<
				"Token<'" << type << "', '" << getString() << "'>" <<
				" At " << column << ":" << line
				<< endl;
		}
	};

	class Lexer {
		public:
		string input;
		vector<Token> tokens;

		char curChar;
		int index = 0;

		int column = 1;
		int line = 1;

		Lexer(string input): input(input) {};

		char advance(int amt = 1) {
			index += amt;
			column += amt;

			if (index >= input.length())
				return curChar = '\0'; // return nullbyte if index > the length of the input

			return curChar = input[index];
		}

		char peek(int amt = 1) {
			if (index + amt >= input.length())
				return '\0'; // return nullbyte if index > the length of the input

			if (input[index + amt] == '\r') return peek(amt + 1);

			return input[index + amt];
		}

		bool isAlpha(char c) {
			return (
				('a' <= c && 'z' >= c) ||
				('A' <= c && 'Z' >= c)
			);
		}

		bool isSpecial(char c) {
			return (
				c == '$' ||
				c == '_'
			);
		}

    bool isDelimiter(char c) {
      return (
        c == '(' ||
        c == ')' ||
        c == '{' ||
        c == '}' ||
        c == ';' ||
				c == ','
      );
    }

    bool isWhitespace(char c) {
      return (
        c == ' '  ||
        c == '\r' ||
        c == '\t'
      );
    }

    bool isQuote(char c) {
      return c == '\"' || c == '\'';
    }

		bool isIdentifier(char c) {
			return (
				!isDelimiter(c) &&
				!isQuote(c) &&
				!isWhitespace(c) &&
				(isSpecial(c) || isAlpha(c))
			);
		}

		// Kind ugly but whatever
    int isOperator(char c) {
      if (
        (c == '=' && peek() == '=') // '==' operator
        || (c == '!' && peek() == '=') // '!=' operator
        || (c == '&' && peek() == '&') // '&&' operator
        || (c == '|' && peek() == '|') // '||' operator
      ) return 2;
      else if (
        (c == '+')
        || (c == '-')
        || (c == '*')
        || (c == '/')
        || (c == '%')
        || (c == '=')
      ) return 1;
      else return 0;
		}

		bool isDigit(char c) {
			return '0' <= c && '9' >= c;
		}
    
		bool checkLinebreak() {
			// I like this if statement -ZDev1
			if (curChar == '\n') {
				Token tok = Token("Linebreak", '\n');
				tok.setPosition(column, line);
				tokens.push_back(tok);

				++line;
				column = 0;

				advance();
				return true;
			}

			return false;
		}

		char fixEscapes(char c) {
			if (c == '\\') {
				switch(advance()) {
					case 'n': return '\n';
					case 't': return '\t';
					case 'r': return '\r';

					default: return curChar;
				}
			}
			return c;
		}

		bool isNumber(char c) {
      return (
        (c == '-' && isDigit(peek()))
        || isDigit(c)
      );
    }

		bool isDirective(char c) {
			return c == '#';
		}

		bool isComment(char c) {
			return c == '/' && peek() == '/';
		}

		bool isCommentEnd(char c) {
			return c == '\\' && peek() == '\\';
		}

		vector<Token> tokenize() {
			if (input.size() < 1) return {};

			curChar = input[0];

			while (curChar != '\0') {
				const int lastIndex = index;

        if (isWhitespace(curChar)) {
					while (isWhitespace(curChar)) advance();
				}

				if (isDirective(curChar)) {
					int col = column;
					int ln = line;

					advance();
					string directive = "";
					
					while (curChar != '\n' && curChar != '\0') {
						directive += curChar;
						advance();
					}

					Token tok = Token("Directive", directive);
					tok.setPosition(col, ln);

					tokens.push_back(tok);
				}

        checkLinebreak();

				if (isComment(curChar)) {
					while (!isCommentEnd(curChar)) {
						if (!checkLinebreak()) advance();
					}

					advance(2);
				}

        if (isDelimiter(curChar)) {
          Token tok = Token("Delimiter", curChar);
					tok.setPosition(column, line);
					tokens.push_back(tok);

					advance();
        }

				if (isNumber(curChar)) {
          std::string type = "Int";
          int col = column;
          int ln = line;

          std::string val = "";

          if (curChar == '-') {
            val += curChar;
            advance();
          }

          while (isNumber(curChar)) {
            val += curChar;
            advance();

            if (curChar == '.') {
              type = "Float";
              val += ".";

              advance();
            }
          }

          Token tok = Token(type, val.c_str());
          tok.column = col;
          tok.line = ln;

          tokens.push_back(tok);
        }

				if (isOperator(curChar) > 0) {
          int amt = isOperator(curChar);
          std::string op = std::string(1, curChar);
          int col = column;
          int ln = line;

          for (int i = 0; i < amt - 1; i++) {
            op += advance();
          }

          advance();

          Token tok = Token("Operator", op.c_str());
          tok.column = col;
          tok.line = ln;

          tokens.push_back(tok);
        }

				if (isQuote(curChar)) {
					char quote = curChar;
					int col = column;
					int ln = line;

					string val = "";

					advance();

					while (curChar != '\0' && curChar != quote) {
						val += fixEscapes(curChar);
						advance();
					}

					advance();

					Token tok = Token("String", val.c_str());
					tok.setPosition(col, ln);

					tokens.push_back(tok);
				}

				if (isIdentifier(curChar)) {
					string val = "";

          int col = column;
          int ln = line;

          while (curChar != '\0' && isIdentifier(curChar)) {
            val += curChar;
            advance();
          }

          string type = keywords.find(val) != keywords.end()
            ? keywords[val]
            : "Identifier";

          Token tok = Token(type, val.c_str());
          tok.setPosition(col, ln);

          tokens.push_back(tok);
				}

				// If the index has not changed then we got an error on our hands
				// Because it doesn't know what the character / token is
				if (lastIndex == index) {
					// Probably should add more errors so we don't have to type the string out every time
					throw SyntaxError("Invalid Character '" + string(1, curChar) + "'");
				}
			}

			Token EOFTok = Token("EOF", "EOF");
			EOFTok.setPosition(column, line);
			tokens.push_back(EOFTok);

			return tokens;
		}
	};
}