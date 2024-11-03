#include "LineString.h"

LineString::LineString()
{}

LineString::LineString(std::string str)
	: std::string(str)
{}

void LineString::killLeadingWhitespace()
{
	while (c_str() != NULL && (*c_str() == ' ' || *c_str() == '\t'))
	{
		erase(0, 1);
	}
}

void LineString::killFirstChar(char character)
{
	if (c_str() != NULL && *c_str() == character)
	{
		erase(0, 1);
	}
}
