#include "LineString.h"

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

// trim from start (in place)
inline void ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
		std::not1(std::ptr_fun<int, int>(std::isspace))));
}

// trim from end (in place)
inline void rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(),
		std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}


LineString::LineString()
{}

LineString::LineString(std::string str)
	: std::string(str)
{}

void LineString::killLeadingWhitespace()
{
	ltrim(*this);
}
void LineString::killTrailingWhitespace()
{
	rtrim(*this);
}

void LineString::trim()
{
	ltrim(*this);
	rtrim(*this);
}

void LineString::killFirstChar(char character)
{
	if (c_str() != NULL && *c_str() == character)
	{
		erase(0, 1);
	}
}

LineString::operator const char*() const
{
	return c_str();
}
