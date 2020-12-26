#include <iostream>

// A little color namespace so we can easily add Colors
// Don't know if there is a std lib one already though but oh well
namespace Colors {
	using namespace std;

	static string Red = "\x1b[38;2;255;25;25m";
	static string Green = "\x1b[38;2;25;255;25m";
	static string Blue = "\x1b[38;2;25;25;255m";
  static string White = "\x1b[38;2;255;255;255m";
  static string Reset = "\x1b[0m";

	string getColor(int r, int g, int b) {
		string red = to_string(r);
		string green = to_string(g);
		string blue = to_string(b);

		return "\x1b[38;2;" + red + ";" + green + ";" + blue + "m";
	};
};