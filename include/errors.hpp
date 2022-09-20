#pragma once

#include <exception>
#include <string>
#include "./colors.hpp"


namespace Aardvark {
	class Error : public exception {
	public:
		std::string message;
		std::string type;

		Error(std::string_view message, std::string_view type): message(message), type(type) {
			format();
		}

		Error(std::string_view message): message(message) {
			type = "Error";
			format();
		}

		virtual void format() {
			message = Colors::Red + type + ": " + message + Colors::Reset;
		}

		virtual const char* what() const throw() {
			return message.c_str();
		}
	};

	class SyntaxError : public Aardvark::Error {
	public:
		SyntaxError(std::string_view msg): Aardvark::Error(msg, "SyntaxError") {}
	};

	class TypeError : public Aardvark::Error {
	public:
		TypeError(std::string_view msg): Aardvark::Error(msg, "TypeError") {}
	};

	class UndeclaredError : public Aardvark::Error {
    public:
		UndeclaredError(std::string_view msg): Aardvark::Error(msg, "UndeclaredError") {}
  };

	class FileError : public Aardvark::Error {
	public:
		std::string message;
		std::string type;

		FileError(string type, string message): message(message), type(type) {
			format();
		}

		FileError(string message): message(message) {
			type = "FileError";
			format();
		}

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
