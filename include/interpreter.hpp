#include <map>
#include "../include/parser.hpp"

namespace Aardvark {
  using namespace std;

  enum class Types {
    None,
    Bool,
    While,
    If,
    Int,
    Double,
    String,
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

  class AdkScope;
  class AdkValue { // Main Dynamic Value class
    public:

    Types type;

    void* value;
    std::map<std::string, AdkValue*> properties;
    AdkScope* Scope;

    bool isReturnValue = false;

    AdkValue* Get(std::string propName) {
      if (!HasProp(propName))
        throw Error("Property " + propName + " is undefined");
      
      return properties[propName];
    } 

    virtual std::string getType() {
      return "value";
    }

    virtual bool HasProp(std::string propName) {
      return properties.find(propName) != properties.end();
    }

    virtual std::string getPropString(std::string propName) {
      if (!HasProp(propName)) return "";

      return properties[propName]->toString();
    }

    virtual int getPropInt(std::string propName) {
      if (!HasProp(propName)) return -1;

      return properties[propName]->toInt();
    }

    int toInt() {
      // No floating point yet

      if (type == Types::String)
        return std::stoi(cast<std::string>());

      if (type == Types::None)
        return 0;

      return cast<int>();
    }

    double toDouble() {
      if (type == Types::String)
        return std::stod(cast<std::string>());

      if (type == Types::None)
        return 0;

      return cast<double>();
    }

    virtual std::string toString() {
      switch (type) {
        case Types::Int:
          return std::to_string(toInt());
        
        case Types::None:
          return "None";

        default:
          return cast<std::string>();
      }
    }

    bool toBool() {
      if (type == Types::Int)
        return toInt() == 1;

      if (type != Types::Bool)
        return type != Types::None;

      return cast<bool>();
    }

    template<typename T>
    T cast() {
      return T(*(T*)value);
    }

    AdkValue() {
      type = Types::None;
      value = nullptr;
    }
    AdkValue(std::string val) {
      type = Types::String;
      value = new std::string(val);
    }
    AdkValue(const char* val) {
      type = Types::String;
      value = new std::string(val);
    }
    AdkValue(int val) {
      type = Types::Int;
      value = new int(val);
    }
    AdkValue(double val) {
      type = Types::Double;
      value = new double(val);
    }
    AdkValue(bool val) {
      type = Types::Int;
      value = new int(val);
    }

    AdkValue* operator==(AdkValue& x) {
      if (type == Types::Int) {
        return new AdkValue(toInt() == x.toInt());
      } else if (type == Types::Double) {
        return new AdkValue(toDouble() == x.toDouble());
      } else if (type == Types::String) {
        return new AdkValue(toString() == x.toString());
      }

      return new AdkValue(toBool() == x.toBool());
    }

    AdkValue* operator+(AdkValue& x) {
      if (type == Types::Int) {
          return new AdkValue(toInt() + x.toInt());
      } else if (type == Types::String) {
        return new AdkValue(toString() + x.toString());
      }

      return new AdkValue(toBool() + x.toBool());
    }

    AdkValue* operator-(AdkValue& x) {
      if (type == Types::Int) {
        return new AdkValue(toInt() - x.toInt());
      } else if (type == Types::Double) {
        return new AdkValue(toDouble() - x.toDouble());
      } else if (type == Types::String) {
        throw TypeError("unsupported operand type(s) for -: 'str'");
      }

      return new AdkValue(toBool() - x.toBool());
    }

    AdkValue* operator*(AdkValue& x) {
      if (type == Types::Int) {
        if (x.type == Types::String) {
          std::string str = "";

          for (int i = 0; i < toInt(); i++)
            str += x.toString();

          return new AdkValue(str);
        }

        return new AdkValue(toInt() * x.toInt());
      } else if (type == Types::Double) {
        return new AdkValue(toDouble() * toDouble());
      } else if (type == Types::String) {
        if (x.type == Types::Int) {
          std::string str = "";

          for (int i = 0; i < x.toInt(); i++)
            str += toString();

          return new AdkValue(str);
        }
        throw TypeError("can't multiply sequence by non-int of type 'str'");
      }

      return new AdkValue(toBool() * x.toBool());
    }

    AdkValue* operator/(AdkValue& x) {
      if (type == Types::Int) {
        if (x.type == Types::String) {
          throw TypeError("unsupported operand type(s) for /: 'int' and 'str'");
        }

        return new AdkValue((double)toInt() / (double)x.toInt());
      } else if (type == Types::Double) {
        return new AdkValue(toDouble() / x.toDouble());
      } else if (type == Types::String) {
        if (x.type == Types::Int) {
          throw TypeError("unsupported operand type(s) for /: 'str' and 'int'");
        }
        throw TypeError("unsupported operand type(s) for /: 'str' and 'str'");
      }

      return new AdkValue(toBool() / x.toBool());
    }
  };
  
  class AdkScope {
    public:

    AdkScope* parent = nullptr;
    std::map<std::string, AdkValue*> properties = std::map<std::string, AdkValue*>();

    AdkScope() {};
    AdkScope(AdkScope* parent): parent(parent) {};

    AdkScope* Lookup(std::string name) {
      if (HasProp(name)) {
        return this;
      }

      if (parent != nullptr)
        return parent->Lookup(name);
      
      return nullptr;
    }

    AdkValue* Set(std::string name, AdkValue* prop) {
      AdkScope* scope = Lookup(name);
      if (scope != nullptr)
        return scope->Define(name, prop);
      else
        return Define(name, prop);
    }

    AdkValue* Define(std::string name, AdkValue* prop) {
      properties[name] = prop;
      return prop;
    }
    void Define(std::string name) {
      properties[name] = nullptr;
    }

    bool HasProp(std::string name) {
      if (properties.size() < 1) return false;
      return properties.find(name) != properties.end();
    }

    AdkValue* Get(std::string name) {
      if (HasProp(name))
        return properties[name];

      AdkScope* scope = Lookup(name);
      if (scope != nullptr)
        return scope->Get(name);

      throw UndeclaredError(name);
    }

    AdkScope* Extend() {
      return new AdkScope(this);
    }
  };

  class Interpreter;

  typedef AdkValue* (*NativeFunction)(std::vector<AdkValue*> args);

  class AdkFunction : public AdkValue {
    public:

    Interpreter* interpreter;

    NativeFunction Func = nullptr;
    std::vector<Expression*> block = std::vector<Expression*>();
    std::vector<std::string> paramNames = std::vector<std::string>();

    AdkFunction(Interpreter* interpreter, NativeFunction Func): AdkValue(), interpreter(interpreter), Func(Func) {
      type = Types::Function;
    }

    AdkFunction(Interpreter* interpreter): AdkValue(), interpreter(interpreter) {
      type = Types::Function;
    }

    AdkValue* Call(std::vector<Expression*> args, AdkScope* scope);

    virtual std::string getType() {
      return "function";
    }

    virtual std::string toString() { // Overwritable - Is that the write word? Writable?
      std::stringstream str;
      str << "<function " << properties["__name__"]->toString() << " at 0x" << static_cast<const void*>(this) << ">";
      return str.str();
    }
  };

  class Interpreter {
    public:

    Expression* ast;
    AdkScope* Scope;

    Interpreter() { Scope = new AdkScope(); }
    Interpreter(Expression* ast): ast(ast) { // What is this
      Scope = new AdkScope();
    }

    void SetGlobals(AdkScope* scope) { // TBH Could just let them pass one in the Interpret Method
      Scope = scope;
    }

    std::string getIdentifier(Expression* exp) {\
      switch (exp->type) {
        case ExprTypes::Identifier:
          return exp->value.getString();
        
        case ExprTypes::FunctionCall:
          return exp->value.getString();

        case ExprTypes::Assign:
          return exp->left->value.getString();

        default:
          throw Error("Invalid token");
      }
    }

    AdkValue* iReturn(Expression* exp, AdkScope* scope) { // Anything prefixed with "i" just means interpret
      AdkValue* returnStmt = Evaluate(exp->scope, scope);
      returnStmt->isReturnValue = true;

      return returnStmt;
    }

    AdkValue* iScope(Expression* exp, AdkScope* scope) {
      bool scopeReturned = false;
      AdkValue* val;

      // Lol sExpression | It is actually scope Expression
      for (Expression* sExp : exp->block) {
        val = Evaluate(sExp, scope);

        if (val != nullptr && val->isReturnValue)
          return val;
      }

      return val;
    }

    AdkValue* iDotOp(Expression* exp, AdkScope* scope, AdkValue* value) {
      std::string name = exp->value.getString();
      std::string propName = getIdentifier(exp->dotOp);
      AdkValue* propValue = value->Get(propName);

      bool isFunction = propValue->getType() == "function";

      switch (exp->dotOp->type) {
        case ExprTypes::FunctionCall: {
          if (isFunction) {
            AdkValue* retVal = ((AdkFunction*)propValue)->Call(exp->dotOp->args, scope->Extend());
            if (exp->dotOp->dotOp != nullptr) {
              return iDotOp(exp->dotOp, scope, retVal);
            }
            
            return retVal;
          } else {
            throw Error("Not a callable function");
          }
        }

        case ExprTypes::Identifier: {
          if (exp->dotOp->dotOp != nullptr) {
            return iDotOp(exp->dotOp, scope, propValue);
          }

          return propValue;
        }

        default:
          throw Error("Unknown token after accessing property.");
      }
    }

    AdkValue* iIdentifier(Expression* exp, AdkScope* scope) {
      AdkValue* value = scope->Get(exp->value.getString());

      if (exp->dotOp != nullptr) {
        return iDotOp(exp, scope, value);
      }

      return value;
    }

    AdkValue* iAssignment(Expression* exp, AdkScope* scope) {
      std::string identifier = exp->left->value.getString();
      AdkValue* value = scope->Define(identifier, Evaluate(exp->right, scope));

      return value;
    }

    AdkValue* iFunction(Expression* exp, AdkScope* scope) {
      std::string funcName = exp->value.getString();
      AdkFunction* function = new AdkFunction(this);
      function->Scope = scope;
      function->block = exp->scope->block;

      function->properties["__name__"] = new AdkValue(funcName);
      
      for (Expression* arg : exp->args) {
        function->paramNames.push_back(arg->value.getString());
      }

      if (scope->Lookup(funcName) != nullptr) {
        throw Error("Function already defined");
      }

      scope->Define(funcName, function);

      return function;
    }

    AdkValue* iFunctionCall(Expression* exp, AdkScope* scope) {
      std::string funcName = exp->value.getString();
      AdkValue* function = scope->Get(funcName);

      if (function->getType() == "function") {
        AdkValue* value = ((AdkFunction*)function)->Call(exp->args, scope->Extend());
        if (exp->dotOp != nullptr) {
          return iDotOp(exp, scope, value);
        }
        
        return value;
      } else {
        throw Error("Not a callable function");
      }
    }

    AdkValue* iOperation(std::string op, AdkValue* a, AdkValue* b) {
      if (op == "==") {
        return *a == *b;
      } else if (op == "+") {
        return *a + *b;
      } else if (op == "-") {
        return *a - *b;
      } else if (op == "*") {
        return *a * (*b);
      } else if (op == "/") {
        return *a / *b;
      }

      return new AdkValue(false);
    }

    AdkValue* iBinary(Expression* exp, AdkScope* scope) {
      AdkValue* left = Evaluate(exp->left, scope);
      AdkValue* right = Evaluate(exp->right, scope);
      std::string op = exp->op.getString();

      return iOperation(op, left, right);
    }

    AdkValue* iIf(Expression* exp, AdkScope* scope) {
      AdkValue* condition = Evaluate(exp->condition, scope);

      if (condition->toBool() || condition->toInt()) {
        return Evaluate(exp->then, scope->Extend());
      }

      if (exp->els != nullptr)
        return Evaluate(exp->els, scope->Extend());
      
      return nullptr;
    }

    AdkValue* Interpret() {
      return Evaluate(ast, Scope);
    }

    AdkValue* Interpret(std::string input, char** argv) {
      Lexer lexer = Lexer(input);
      Parser parser = Parser(lexer.tokenize(), argv);
      ast = parser.parse();
      // for (auto token : parser.tokens) token.debugPrint();
      return Evaluate(ast, Scope);
    }

    AdkValue* Evaluate(Expression* exp, AdkScope* scope) { // Main evaluation using capital letter becasuse it looks nicer when doing interpreter->
      switch (exp->type) {
        case ExprTypes::Scope:
          return iScope(exp, scope);
        case ExprTypes::String:
          return new AdkValue(exp->value.getString());
        case ExprTypes::Int:
          return new AdkValue(exp->value.getInt(true));
        case ExprTypes::Bool:
          return new AdkValue(exp->value.getString() == "True" ? true : false);

        case ExprTypes::If:
          return iIf(exp, scope);

        case ExprTypes::Identifier:
          return iIdentifier(exp, scope);
        case ExprTypes::Assign:
          return iAssignment(exp, scope);
        case ExprTypes::Binary:
          return iBinary(exp, scope);

        case ExprTypes::Function:
          return iFunction(exp, scope);

        case ExprTypes::FunctionCall:
          return iFunctionCall(exp, scope);

        case ExprTypes::Return:
          return iReturn(exp, scope);

        default:
          return new AdkValue();
      }
    }
  };

  AdkValue* AdkFunction::Call(std::vector<Expression*> args, AdkScope* scope) { // Going to Clean this Up eventually and make something a lot nicer and easier to use
    std::vector<AdkValue*> AdkValues = {};

    if (this->Func != nullptr) { // WHy?
      for (Expression* arg : args) {
        if (arg->type == ExprTypes::Assign) {
          AdkValue* argValue = interpreter->Evaluate(arg->right, scope);
          argValue->properties["__ARGNAME__"] = new AdkValue(arg->left->value.getString());
          AdkValues.insert(AdkValues.begin(), argValue);
          continue;          
        } else {
          AdkValue* argValue = interpreter->Evaluate(arg, scope);
          argValue->properties["__ARGNAME__"] = new AdkValue(0);
          AdkValues.push_back(argValue);
        }
      }
      
      AdkValue* returnVal = this->Func(AdkValues);

      return returnVal;
    }
    AdkValue* returnVal = new AdkValue();

    for (int i = 0; i < paramNames.size(); i++) {
      if (args[i]->type == ExprTypes::Assign) { // Yea these if statements need to go or I need to change something this is quite long
        interpreter->Evaluate(args[i], scope);
      } else {
        scope->Define(paramNames[i], interpreter->Evaluate(args[i], scope));
      }
    }

    for (Expression* expr : block) {
      returnVal = interpreter->Evaluate(expr, scope);

      if (returnVal != nullptr && returnVal->isReturnValue) return returnVal;
    }

    return returnVal;
  };
}; // namespace Aardvark