#include <exception>
#include <string>
#include "colors.hpp"

namespace Aardvark {
	using namespace std;
	class Error : public exception {
		public:
		string message;
		string type;

		Error(string message, string type): message(message), type(type) {
			format();
		};
		Error(string message): message(message) {
			type = "Error";
			format();
		};

		void format() {
			message = Colors::Red + type + ": " + message + Colors::Reset;
		}

		virtual const char* what() const throw() {
			return message.c_str();
		}
	};

	class SyntaxError : public Error {
		public:
		SyntaxError(string msg): Error(msg, "SyntaxError") {};
	};

	class TypeError : public Error {
		public:
		TypeError(string msg): Error(msg, "TypeError") {};
	};

	class UndeclaredError : public Error {
    public:
		UndeclaredError(string msg): Error(msg, "UndeclaredError") {};
  };

	class FileError : public exception {
		public:
		string message;
		string type;

		FileError(string type, string message): message(message), type(type) {
			format();
		};
		FileError(string message): message(message) {
			type = "FileError";
			format();
		};

		void format() {
			if (type == "Not Found") {
				message = Colors::Red + "FileNotFound: File '" + message + "' does not exist!" + Colors::Reset;
			} else {
				message = type + ": " + message;
			}
		}

		virtual const char* what() const throw() {
			return message.c_str();
		}
	};
} // namespace Aardvark