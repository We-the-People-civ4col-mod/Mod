#pragma once

#include <string>

class LineString : public std::string
{
public:
	LineString();
	LineString(std::string);

	void killLeadingWhitespace();
	void killTrailingWhitespace();
	void trim();
	void killFirstChar(char character);
};
