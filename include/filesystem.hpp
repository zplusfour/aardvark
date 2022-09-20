#pragma once

#include <fstream>
#include <ios>
#include <string>
#include "./util.hpp"


namespace Aardvark {
	bool isOnlyWhitespace(std::string string);

	template<AdkUtil::Streamable T>
	void writeFile(const std::string& path, const T& data) { // constructor of `std::ofstream` doesn't accept `std::string_view` so `const std::string&` used instead
		std::ofstream writer(path);
		if (writer)
			writer << data;
	}
	
	std::string readFile(const std::string& path);

	template<AdkUtil::Streamable T>
	void appendFile(const std::string& path, const T& data) {
		std::ofstream writer(path, std::ios::app);
		if (writer)
			writer << data;
	}

	bool fileExists(const std::string& path);
}; // namespace Aardvark
