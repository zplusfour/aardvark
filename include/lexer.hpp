#include <iostream>
#include <vector>
#include <unordered_map>
#include "errors.hpp"

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
		{ "while", "Keyword" }
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

		int getInt() {
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
        c == ';'
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

		bool isOperator(char c) { return false; };

		vector<Token> tokenize() {
			if (input.size() < 1) return {};

			curChar = input[0];

			while (curChar != '\0') {
				const int lastIndex = index;

        if (isWhitespace(curChar))
          advance();

				// I like this if statement -ZDev1
        if (curChar == '\n') {
					++line;
					column = 0;

          advance();
        }

        if (isDelimiter(curChar)) {
          Token tok = Token("Delimiter", curChar);
					tok.setPosition(column, line);
					tokens.push_back(tok);

					advance();
        }

				if (isQuote(curChar)) {
					char quote = curChar;
					int col = column;
					int ln = line;

					string val = "";

					advance();

					while (curChar != '\0' && curChar != quote) {
						val += curChar;
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