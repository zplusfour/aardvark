#include <algorithm>
#include "lexer.hpp"

namespace Aardvark {
  using namespace std;

  unordered_map<string, int> PRECEDENCE = {
    { "=", 1 },

    { "||", 4 },
    { "&&", 5 },

    { "<", 7 }, { ">", 7 }, { "<=", 7 }, { ">=", 7 }, { "==", 7 },

    { "+", 10 }, { "-", 10 },
    { "*", 20 }, { "/", 20 }, { "%", 20 }
  };

  enum class ExprTypes { // access it by ExprTypes::Whatever
    None,
    While,
    If,
    Int,
    Float,
    String,
    Bool,
    Variable,
    Identifier,
    Assign,
    Binary,
    Scope,
    Function,
    FunctionCall,
    Class,
    Return
  };

  vector<string> ExprStrings = {
    "None",
    "While",
    "If",
    "Int",
    "Float",
    "String",
    "Variable",
    "Identifier",
    "Assign",
    "Binary",
    "Scope",
    "Function",
    "FunctionCall",
    "Class",
    "Return"
  };

  // For debugging later
  string getExprString(ExprTypes type) {
    return ExprStrings[((int)type)];
  }

  vector<string> splitStr(string str, char split) {
    vector<string> arr;

    string main = "";

    for (auto c : str) {
      if (c == split) {
        arr.push_back(main);
        main = "";
      } else {
        main += c;
      }
    }

    if (main.length() > 0)
      arr.push_back(main);

    return arr;
  }

  string joinVector(vector<string> arr, string join) {
    string main = "";

    bool isFirst = true;
    for (auto str : arr) {
      if (isFirst) isFirst = false;
      else main += join;

      main += str;
    }

    return main;
  }

  template<typename T>
  bool vincludes(vector<T> arr, T value) {
    return find(arr.begin(), arr.end(), value) != arr.end();
  };

  class Expression {
    public:
    ExprTypes type;
    Token value;

    Expression* left;
    Token op;
    Expression* right;

    vector<Expression*> block;
    vector<Expression*> args;

    Token identifier;
    Expression* dotOp; // dot operation for like properties and stuff

    bool isArray;
    Expression* then;
    Expression* els;
    Expression* condition;
    Expression* scope;

    Expression(Token value): type(ExprTypes::None), value(value) {};
    Expression(ExprTypes type, Token value): type(type), value(value) {};
  };

  class Parser {
    public:
    vector<Token> tokens;

    Token curTok;
    int pos = 0;

    Expression* ast;

    char** argv;

    Parser(vector<Token> tokens): tokens(tokens) {};
    Parser(vector<Token> tokens, char** argv): tokens(tokens), argv(argv) {};

    Token advance(int amt = 1) {
      if (pos + amt >= tokens.size()) return curTok = Token();
      pos += amt;

      return curTok = tokens[pos];
    }

    Token peek(int amt = 1) {
      if (pos + amt >= tokens.size()) return Token();

      return tokens[pos + amt];
    }

    bool isEOF() {
      return curTok.type == "EOF";
    }

    bool isType(string type, string value, Token peeked) {
      return peeked.type == type && peeked.getString() == value;
    }
    bool isType(string type, Token peeked) {
      return peeked.type == type;
    }
    bool isType(string type, string value) {
      return curTok.type == type && curTok.getString() == value;
    }
    bool isType(string type) {
      return curTok.type == type;
    }

    bool isIgnore(Token tok) {
      return (
        (tok.type == "Delimiter" && tok.getString() == ";") ||
        (tok.type == "Linebreak" && tok.getString() == "\n")
      );
    }

    void skipIgnore() {
      // if (!isIgnore(curTok)) throw SyntaxError("Unexpected token '" + curTok.type + "'");
      while (isIgnore(curTok) && !curTok.isNull() && !isEOF()) advance();
    }

		void skipOverVal(string val, Token tok) {
      if (tok.getString() != val) throw SyntaxError(tok.getString());

      advance();
    }

    void skipOver(string type, string val) {
      if (curTok.type != type || curTok.getString() != val) throw SyntaxError(curTok.getString());

      advance();
    }
    void skipOver(string type) {
      if (curTok.type != type) throw SyntaxError(curTok.getString());

      advance();
    }

    void skipOver(string type, Token tok) {
      if (tok.type != type) throw SyntaxError(tok.getString());

      advance();
    }
    void skipOver(string type, string val, Token tok) {
      if (tok.type != type || tok.getString() != val) throw SyntaxError(tok.getString());

      advance();
    }

		bool nonCallables(Expression* expression) {
      return (
        expression->type != ExprTypes::Function
        && expression->type != ExprTypes::If
      );
    }

    // Parse delimtiers from start to end with expressions in a vector
    vector<Expression*> pDelimiters(string start, string end, string separator) {
      vector<Expression*> values = {};
      bool isFirst = true;

      skipOverVal(start, curTok);
      if (isIgnore(curTok)) skipIgnore();

      while (!isEOF()) {
        if (isType("Delimiter", end)) {
          break;
        } else if (isFirst) {
          isFirst = false;
        } else if (separator == ";\n") {
          skipIgnore();
        } else {
          skipOverVal(separator, curTok);
        }

        Expression* val = pExpression();
        values.push_back(val);\
      }
      skipOverVal(end, curTok);

      return values;
    }

		// Parse any prop / dot operations
		// probably should change how this works
		bool pDotOp(Expression* exp) {
			// TODO
			if (isType("Delimiter", ".")) {
        advance();

				if (!isType("Identifier"))
					throw SyntaxError(curTok.getString()); // Meaningful errors soon

				exp->identifier = curTok;
        exp->dotOp = pExpression();
        return true;
      }

      return false;
		}

    Expression* isCall(Expression* expression) {
      return (isType("Delimiter", "(", peek()) && nonCallables(expression)) ? pCall(expression) : expression;
    }

    Expression* pCall(Expression* expr) {
      // TODO: Parse function calls here
      advance();
      Token identifier = expr->value;

      Expression* call = new Expression(ExprTypes::FunctionCall, identifier);
      call->args = pDelimiters("(", ")", ",");
      call->dotOp = nullptr;

      return call;
    }

    Expression* pBinary(Expression* left, int prec) {
      // TODO: Parse Binary Operations or Assignment
      Token op = curTok;

      if (isType("Operator")) {
        string opvalue = op.getString();
        int newPrec = PRECEDENCE[opvalue];

        if (newPrec > prec) {
          advance();

          vector<string> assignments = {
            "=",
          };

          ExprTypes type = vincludes<string>(assignments, opvalue)
            ? ExprTypes::Assign
            : ExprTypes::Binary;

          Expression* expr = new Expression(type, op);
          expr->left = left;
          expr->op = op;
          expr->right = pBinary(isCall(pAll()), newPrec); // Now parse the right node and right right node
          // Something like that

          return pBinary(expr, prec); // Parse right node again if its smaller binding 
        }
      }

      return left;
    }

    Expression* pIdentifier(Expression* expr) {
      expr->type = ExprTypes::Identifier;
			expr->dotOp = nullptr;

			if (!isType("Delimiter", "(", peek()))
				advance();

			if (!pDotOp(expr)) {
        // TODO
			}

			return expr;
    }

    Expression* pFunction() {
      skipOverVal("funct", curTok); // Skip over keyword
      Token identifier = curTok;
      advance(); // Skip over Identifier

      Expression* function = new Expression(ExprTypes::Function, identifier);
      function->args = pDelimiters("(", ")", ",");
      function->scope = new Expression(ExprTypes::Scope, identifier);
      function->scope->block = pDelimiters("{", "}", ";\n");

      return function;
    };

    Expression* pReturn() {
      skipOverVal("return", curTok); // Skip over keyword

      Expression* returnVal = pExpression();
      Expression* returnExpr = new Expression(ExprTypes::Return, curTok);
      returnExpr->scope = returnVal;

      return returnExpr;
    };

    Expression* pIf() {
      skipOverVal("if", curTok); // Skip over keyword

      Expression* ifStmt = new Expression(ExprTypes::If, Token("If", true));
      Expression* expr = pExpression();

      ifStmt->condition = expr;
      ifStmt->els = nullptr;


      Expression* then = new Expression(ExprTypes::Scope, Token("Then", true));
      then->block = pDelimiters("{", "}", ";\n");

      ifStmt->then = then;

      return ifStmt;
    }

    Expression* pBoolean(Expression* token) {
      advance();
      return new Expression(ExprTypes::Bool, token->value);
    }

    Expression* pInclude(vector<string> args) {
      string filename = args[1] + ".adk";
      
      string mainfile = argv[1];
      vector<string> folders = splitStr(mainfile, '/');

      string folder = "";

      if (folders.size() > 1) {
        folders.pop_back();
        folder = joinVector(folders, "/") + "/";
      }

      string path = fileExists("./packages/" + filename) ? "./packages/" + filename : folder + filename;
      string fileText;

      if (!fileExists(path)) throw FileError("Not Found", filename);

      fileText = readFile(path);

      Lexer lexer = Lexer(fileText);
      vector<Token> tokens = lexer.tokenize();

      Parser parser = Parser(tokens);
      Expression* fileast = parser.parse();

      ast->block.insert(ast->block.end(), fileast->block.begin(), fileast->block.end());
      advance();

      return new Expression(ExprTypes::None, Token());
    }

    Expression* pDirective(Expression* token) {
      string fulldirective = token->value.getString();
      vector<string> args = splitStr(fulldirective, ' ');

      if (args[0] == "include") {
        return pInclude(args);
      }

      advance();
      return token;
    }

    Expression* pAll() {
      // TODO: Complete this function
      if (isType("Delimiter", "(")) { // Parses basic (2 + 2) or Anything surrounded by parenthesis
        advance();
        Expression* expr = pExpression();
        skipOver("Delimiter", ")");
        return expr;
      }

      Expression* token = new Expression(curTok);

      if (isType("Keyword", "funct"))
        return pFunction();

      if (isType("Keyword", "return"))
        return pReturn();

      if (isType("Keyword", "if"))
        return pIf();

      if (isType("Directive"))
        return pDirective(token);

      if (isType("Keyword", "True") || isType("Keyword", "False"))
        return pBoolean(token);

      if (isType("Int")) {
        token->type = ExprTypes::Int;
				advance();

				return token;
			} else if (isType("Float")) {
        token->type = ExprTypes::Float;
				advance();

				return token;
			} else if (isType("String")) {
        token->type = ExprTypes::String;
				advance();

				return token;
			} else if (isType("Identifier")) {
				return pIdentifier(token);
			}

      if (isIgnore(curTok)) {
        skipIgnore();
        return token;
      }
    }
    
    Expression* pExpression() {
      // this automagically helps us with formatting the ast correctly
      return isCall(pBinary(isCall(pAll()), 0));
    };
  
    Expression* parse() {
      ast = new Expression(ExprTypes::Scope, Token("_TOP_", true));
      curTok = tokens[0];

      if (isIgnore(curTok)) skipIgnore();

      while (!curTok.isNull() && !isEOF()) {
        Expression* expr = pExpression();
        if (expr->type != ExprTypes::None)
          ast->block.push_back(expr);

        if (!curTok.isNull() && !isEOF()) skipIgnore();
      }

      return ast;
    }
  };
} // namespace Aardvark