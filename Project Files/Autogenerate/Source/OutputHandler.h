#pragma once

#include <string>

class OutputHandler
{
public:
	OutputHandler();
	
	void addStartBracket();
	void addEndBracket();

	void printLine(const char* A);
	void printLine(const char* A, const char* B);
	void printLine(const char* A, const char* B, const char* C);

	void printLineRemoveLastChar(const char* A, const char* B);

	void printLineNoIndent(const char* A);

	void saveFile(const char* filenameA, const char* filenameB = "", const char* filenameC = "");

private:
	void changeIndent(int iChange);
	void addIndent();

	std::string m_text;
	int m_indent;
};
