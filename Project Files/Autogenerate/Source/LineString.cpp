#include "LineString.h"

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <sstream>

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

LineString::LineString(int i)
{
	append(i);
}


LineString::LineString(const char* A)
{
	assign(A);
}

LineString::LineString(const char* A, const char* B)
{
	assign(A);
	append(B);
}

LineString::LineString(const char* A, const char* B, const char* C)
{
	assign(A);
	append(B);
	append(C);
}

LineString::LineString(const char* A, const char* B, const char* C, const char* D)
{
	assign(A);
	append(B);
	append(C);
	append(D);
}

LineString::LineString(const char* A, const char* B, const char* C, const char* D, const char* E)
{
	assign(A);
	append(B);
	append(C);
	append(D);
	append(E);
}

LineString::LineString(const char* A, const char* B, const char* C, const char* D, const char* E, const char* F)
{
	assign(A);
	append(B);
	append(C);
	append(D);
	append(E);
	append(F);
}

LineString::LineString(const char* A, const char* B, const char* C, const char* D, const char* E, const char* F, const char* G)
{
	assign(A);
	append(B);
	append(C);
	append(D);
	append(E);
	append(F);
	append(G);
}

LineString::LineString(const char* A, const char* B, const char* C, const char* D, const char* E, const char* F, const char* G, const char* H)
{
	assign(A);
	append(B);
	append(C);
	append(D);
	append(E);
	append(F);
	append(G);
	append(H);
}

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

LineString& LineString::append(int i)
{
	std::ostringstream buffer;
	buffer << i;

	return append(buffer.str());
}

LineString& LineString::append(const char* str)
{
	std::string::append(str);
	return *this;
}

LineString& LineString::append(std::string str)
{
	std::string::append(str);
	return *this;
}

LineString::operator const char*() const
{
	return c_str();
}
