#pragma once

#include <string>

class OutputHandler
{
public:
	OutputHandler();
	
	void addStartBracket();
	void addEndBracket(bool bAddSemicolon = false);

	void printLine();
	void printLine(const char* A);
	void printLine(const char* A, const char* B);
	void printLine(const char* A, const char* B, const char* C);
	void printLine(const char* A, const char* B, const char* C, const char* D);
	void printLine(const char* A, const char* B, const char* C, const char* D, const char* E);
	void printLine(const char* A, const char* B, const char* C, const char* D, const char* E, const char* F);
	void printLine(const char* A, const char* B, const char* C, const char* D, const char* E, const char* F, const char* G);
	void printLine(const char* A, const char* B, const char* C, const char* D, const char* E, const char* F, const char* G, const char* H);
	void printLine(const char* A, const char* B, const char* C, const char* D, const char* E, const char* F, const char* G, const char* H, const char* I);

	void printLineRemoveLastChar(const char* A, const char* B);

	void printLineNoIndent(const char* A);

	void saveFile(const char* filenameA, const char* filenameB = "", const char* filenameC = "");

private:
	void changeIndent(int iChange);
	void addIndent();

	std::string m_text;
	int m_indent;
};
