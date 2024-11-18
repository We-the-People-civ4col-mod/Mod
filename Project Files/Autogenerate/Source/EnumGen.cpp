#include <stdio.h>

#include "assert.h"
#include "EnumGen.h"
#include "FileAccess.h"
#include "OutputHandler.h"

std::vector<EnumGen> EnumVec;

const std::vector<EnumGen>& EnumGen::getVector()
{
	return EnumVec;
}

const EnumGen* EnumGen::getEntry(std::string name)
{
	for (std::vector<EnumGen>::iterator it = EnumVec.begin(); it != EnumVec.end(); it++)
	{
		if (it->name() == name)
		{
			return &(*it);
		}
	}
	assert(false);
	return NULL;
}

EnumGen::EnumGen(const char* name, int ilength)
	: m_bAlwaysStatic(true)
	, m_bHasFile(false)
	, m_iLength(ilength)
	, m_name(name)
{
	EnumVec.push_back(*this);
}

EnumGen::EnumGen(class Element file)
	: m_bAlwaysStatic(false)
	, m_bHasFile(true)
{
	m_name = file.name();
	m_name.erase(m_name.size()-5, 5);

	Element AlwaysStatic = file.FirstChild("AlwaysStatic");
	if (AlwaysStatic.isValid())
	{
		m_bAlwaysStatic = AlwaysStatic.getBool();
	}

	Element current_file = file.FirstChild("Files");
	for (current_file = current_file.FirstChild("File"); current_file.isValid(); current_file = current_file.NextSibling("File"))
	{
		const char* filename = current_file.getText();
		FileAccessXML xml_file(filename);

		Element tag = xml_file.getRoot();

		while (tag.isValid() && !tag.FirstChild("Type").isValid())
		{
			tag = tag.FirstChild();
		}

		for (; tag.isValid(); tag = tag.NextSibling())
		{
			const char* type = tag.FirstChild("Type").getText();
			m_types.push_back(type);
		}
	}

	// if empty, check if the file has a list of vanilla tags
	if (m_types.size() == 0)
	{
		Element types = file.FirstChild("Types");
		if (types.isValid())
		{
			for (types = types.FirstChild("Type"); types.isValid(); types = types.NextSibling("Type"))
			{
				const char* type = types.getText();
				m_types.push_back(type);
			}
		}
	}

	m_iLength = m_types.size();

	EnumVec.push_back(*this);
}


bool EnumGen::isStatic() const
{
	return m_bAlwaysStatic;
}

const std::string EnumGen::name() const
{
	return m_name;
}

int EnumGen::length() const
{
	return m_iLength;
}

void EnumGen::writeFile()
{
	writeFile(false);
	if (!m_bAlwaysStatic)
	{
		writeFile(true);
	}
	writeCPP();
}

void EnumGen::writeFile(bool bHardcoded)
{
	std::string nameType = m_name;
	nameType.append("Type");

	bool bDynamic = !bHardcoded && !m_bAlwaysStatic;

	OutputHandler text;

	text.printLineNoIndent("#pragma once\n");

	writeDefinesStart(text, bDynamic, bHardcoded);

	text.printLine("class ", nameType.c_str());
	text.addStartBracket();
	text.printLine("friend class CvXMLLoadUtility;");
	text.printLineNoIndent("public:");
	text.printLine(nameType.c_str(), "();");
	text.printLine(nameType.c_str(), "(enum ", nameType.c_str(), "s);");
	text.printLine();
	
	writeEnum(text, bHardcoded);

	text.printLineNoIndent("private:");
	text.printLine("void setup();");
	text.printLine("void setupLength();");
	text.printLine();
	text.printLine("types m_Value;");


	text.addEndBracket(true);

	writeDefinesEnd(text, bDynamic, bHardcoded);

	// fill in file name
	std::string name = m_name;
	name.append("Type");
	if (bHardcoded)
	{
		name.append("_HARDCODED");
	}
	name.append(".h");

	// write file if out of date
	text.saveFile(name.c_str());
}

void EnumGen::writeDefinesStart(class OutputHandler& text, bool bDynamic, bool bHardcoded)
{
	if (bDynamic)
	{
		std::string name = m_name;
		name.append("Type");

		text.printLineNoIndent("#ifdef HARDCODE_XML_VALUES");
		text.printLine("#include \"AUTO_", name.c_str(), "_HARDCODED.h\"");
		text.printLineNoIndent("#else // HARDCODE_XML_VALUES");
		text.printLine();

	}
	else if (bHardcoded)
	{
		text.printLineNoIndent("#ifdef HARDCODE_XML_VALUES");
		text.printLine();
	}
}

void EnumGen::writeDefinesEnd(class OutputHandler& text, bool bDynamic, bool bHardcoded)
{
	if (bDynamic || bHardcoded)
	{
		text.printLine();
		text.printLineNoIndent("#endif // HARDCODE_XML_VALUES");
	}
}

void EnumGen::writeEnum(class OutputHandler& text, bool bHardcoded)
{
	bHardcoded |= m_bAlwaysStatic;

	text.printLine("enum types");
	text.addStartBracket();
	text.printLine("NONE = -1,");

	if (bHardcoded)
	{
		for (unsigned i = 0; i < m_types.size(); ++i)
		{
			text.printLine(m_types[i].c_str(), ",");
		}
		text.printLine("NUM,");
	}
	text.addEndBracket(true);

	text.printLine("");
	if (!bHardcoded)
	{
		text.printLine("extern const types& NUM;");
	}
}


void EnumGen::writeCPP()
{
	OutputHandler text;

	std::string typeName = m_name;
	typeName.append("Type");
	const char* type = typeName.c_str();

	text.printLine("#include \"AUTO_", type, ".h\"");
	text.printLine();

	if (!m_bAlwaysStatic)
	{
		text.printLineNoIndent("#ifndef HARDCODE_XML_VALUES");
		text.printLine(type, "::types LOCAL_NUM;");
		text.printLine("const ", type, "::types& ", type, "::NUM = LOCAL_NUM;");
		text.printLineNoIndent("#endif // HARDCODE_XML_VALUES");
		text.printLine();
	}

	text.printLine(type, "::", type, "()");
	text.printLine("\t: m_Value(", type, "::NONE)");
	text.addStartBracket();
	text.addEndBracket();
	text.printLine();


	text.printLine(type, "::", type, "(", type, "s val)");
	text.printLine("\t: m_Value(static_cast<", type, "::types>(val))");
	text.addStartBracket();
	text.addEndBracket();
	text.printLine();

	text.printLine("void ", type, "::setup()");
	text.addStartBracket();
	text.addEndBracket();
	text.printLine();

	text.printLine("void ", type, "::setupLength()");
	text.addStartBracket();
	text.addEndBracket();
	text.printLine();

	// fill in file name
	std::string name = m_name;
	name.append("Type.cpp");

	// write file if out of date
	text.saveFile(name.c_str());
}
