#pragma once

#include <vector>

class EnumGen
{
public:
	EnumGen(const char* name, int ilength);
	EnumGen(class Element);

	static const std::vector<EnumGen>& getVector();
	static const EnumGen* getEntry(std::string name);

	bool isStatic() const;
	const std::string name() const;
	int length() const;

	void writeFile();

private:
	void writeFile(bool bHardcoded);

	void writeCPP();
	void writeDefinesStart(class OutputHandler&, bool bDynamic, bool bHardcoded);
	void writeDefinesEnd(class OutputHandler&, bool bDynamic, bool bHardcoded);
	void writeEnum(class OutputHandler&, bool bHardcoded);
	

	std::vector<std::string> m_types;
	std::string m_name;
	bool m_bAlwaysStatic;
	bool m_bHasFile;
	int m_iLength;
};
