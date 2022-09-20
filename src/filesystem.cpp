
#include <filesystem>
#include <fstream>
#include <istream>
#include <iterator>
#include <string>
#include <sstream>
#include "../include/filesystem.hpp"


bool Aardvark::isOnlyWhitespace(std::string string) {
	std::istringstream iss(x);
	std::getline(iss >> std::ws, string);
	return !string.size();
}

std::string Aardvark::readFile(const std::string& path) { 
	std::ifstream reader(path);
	return std::string(std::istreambuf_iterator<char>(reader), {});
}

bool Aardvark::fileExists(const std::string& path) {
	return std::filesystem::exists(path)
}
