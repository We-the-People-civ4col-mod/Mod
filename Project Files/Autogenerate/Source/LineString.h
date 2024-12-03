#pragma once

#include <string>

class LineString : public std::string
{
public:
	LineString();
	LineString(std::string);
	LineString(int i);

	LineString(const char*);
	LineString(const char*, const char*);
	LineString(const char*, const char*, const char*);
	LineString(const char*, const char*, const char*, const char*);
	LineString(const char*, const char*, const char*, const char*, const char*);
	LineString(const char*, const char*, const char*, const char*, const char*, const char*);
	LineString(const char*, const char*, const char*, const char*, const char*, const char*, const char*);
	LineString(const char*, const char*, const char*, const char*, const char*, const char*, const char*, const char*);

	void killLeadingWhitespace();
	void killTrailingWhitespace();
	void trim();
	void killFirstChar(char character);

	LineString& append(int i);
	LineString& append(const char*);
	LineString& append(std::string);

	operator const char*() const;
};
