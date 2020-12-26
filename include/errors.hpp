#include <exception>
#include <string>

namespace Aardvark {
	using namespace std;
	class Error : public exception {
		public:
		string message;
		string type;

		Error(string message, string type): message(message), type(type) {};
		Error(string message): message(message) {
			type = "Error";
		};

		virtual const char* what() const throw() {
			return message.c_str();
		}
	};

	class SyntaxError : public Error {
		public:
		SyntaxError(string msg): Error(msg, "SyntaxError") {};
	};
};