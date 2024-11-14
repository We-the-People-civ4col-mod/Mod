#pragma once

#include <vector>

class EnumGen
{
public:
	EnumGen(class Element);

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
};
