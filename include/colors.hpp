#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>


namespace Colors {
	constexpr std::string getColor(const std::uint8_t red, const std::uint8_t green, const std::uint8_t blue) noexcept {
		return "\x1b[38;2;" + std::to_string(red) + ';' + std::to_string(green) + ';' + std::to_string(blue) + 'm';
	};

	static constexpr std::string_view Reset = "\u001b[0m";
	static constexpr std::string_view Red = Colors::getColor(255, 0, 0);
	static constexpr std::string_view Orange = Colors::getColor(255, 127, 0);
	static constexpr std::string_view Yellow = Colors::getColor(255, 255, 0);
	static constexpr std::string_view Lime = Colors::getColor(127, 255, 0);
	static constexpr std::string_view Green = Colors::getColor(0, 255, 0);
	static constexpr std::string_view Teal = Colors::getColor(0, 255, 127);
	static constexpr std::string_view Cyan = Colors::getColor(0, 255, 255);
	static constexpr std::string_view Azure = Colors::getColor(0, 127, 255);
	static constexpr std::string_view Blue = Colors::getColor(0, 0, 255);
	static constexpr std::string_view Purple = Colors::getColor(127, 0, 255);
	static constexpr std::string_view Pink = Colors::getColor(255, 0, 255);
	static constexpr std::string_view Magenta = Colors::getColor(255, 0, 127);
};
