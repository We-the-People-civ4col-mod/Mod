#include <stdio.h>

#include "assert.h"
#include "EnumGen.h"
#include "FileAccess.h"
#include "OutputHandler.h"

EnumGen::HardcodingClass::HardcodingClass()
	: value(TYPE_STANDARD)
{
}

EnumGen::HardcodingClass::HardcodingClass(var initValue)
	: value(initValue)
{
}

void EnumGen::HardcodingClass::assign(const char* newType)
{
	if (strcmp(newType, "TYPE_STANDARD") == 0)
	{
		value = TYPE_STANDARD;
	}
	else if (strcmp(newType, "TYPE_HARDCODED") == 0)
	{
		value = TYPE_HARDCODED;
	}
	else if (strcmp(newType, "TYPE_DYNAMIC") == 0)
	{
		value = TYPE_DYNAMIC;
	}
	else if (strcmp(newType, "TYPE_NO_INFO") == 0)
	{
		value = TYPE_NO_INFO;
	}
	else if (strcmp(newType, "TYPE_NO_FILE") == 0)
	{
		value = TYPE_NO_FILE;
	}
	else
	{
		assert(false);
	}
}

bool EnumGen::HardcodingClass::isAlwaysHardcoded() const
{
	switch (value)
	{
	case TYPE_STANDARD:
		return false;
	case TYPE_HARDCODED:
		return true;
	case TYPE_DYNAMIC:
		return false;
	case TYPE_NO_INFO:
	case TYPE_NO_FILE:
		return true;
	default:
		assert(0);
		return false;
	}
}

EnumGen::HardcodingClass::var EnumGen::HardcodingClass::getVar() const
{
	return value;
}

EnumGen::HardcodingClass& EnumGen::HardcodingClass::operator = (const HardcodingClass::var rhs)
{
	value = rhs;
	return *this;
}

EnumGen::HardcodingClass& EnumGen::HardcodingClass::operator = (const HardcodingClass rhs)
{
	value = rhs.value;
	return *this;
}

bool EnumGen::HardcodingClass::operator == (const var rhs) const
{
	return value == rhs;
}

bool EnumGen::HardcodingClass::operator == (const HardcodingClass rhs) const
{
	return value == rhs.value;
}

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

const EnumGen::HardcodingClass EnumGen::type() const
{
	return m_type;
}

EnumGen::EnumGen(const char* name, int ilength, const char* num)
	: m_iLength(ilength)
	, m_name(name)
	, m_type(HardcodingClass::TYPE_NO_FILE)
{
	if (num != NULL)
	{
		m_num.assign(num);
	}

	EnumVec.push_back(*this);
}

EnumGen::EnumGen(class Element file)
{
	m_name = file.name();
	m_name.erase(m_name.size()-5, 5);

	Element TypeElement = file.FirstChild("Type");
	if (TypeElement.isValid())
	{
		m_type.assign(TypeElement.getText());
	}

	Element NumName = file.FirstChild("NumName");
	if (NumName.isValid())
	{
		m_num = NumName.getText();
	}

	Element current_file = file.FirstChild("Files");
	if (!current_file.isValid())
	{
		m_type = HardcodingClass::TYPE_NO_FILE;
	}
	else
	{
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

	if (m_types.size() > 0)
	{
		m_iLength = m_types.size();
	}
	else
	{
		Element lengthTag = file.FirstChild("iLength");
		if (lengthTag.isValid())
		{
			m_iLength = lengthTag.getInt();
		}
	}

	EnumVec.push_back(*this);
}

const char* EnumGen::name() const
{
	return m_name.c_str();
}

const char* EnumGen::num() const
{
	return m_num.c_str();
}

int EnumGen::length() const
{
	return m_iLength;
}

void EnumGen::writeFile()
{
	writeFile(false);
	if (type() == HardcodingClass::TYPE_STANDARD)
	{
		writeFile(true);
	}
	writeCPP();
}

void EnumGen::writeFile(bool bHardcoded)
{
	std::string nameType = m_name;
	nameType.append("Type");

	OutputHandler text;

	text.printLineNoIndent("#pragma once\n");

	writeDefinesStart(text, bHardcoded);

	text.printLine("class ", nameType.c_str());
	text.addStartBracket();
	text.printLine("friend class CvXMLLoadUtility;");
	text.printLineNoIndent("public:");

	writeEnum(text, bHardcoded);

	text.printLine(nameType.c_str(), "();");
	text.printLine(nameType.c_str(), "(enum ", nameType.c_str(), "s);");
	text.printLine();
	text.printLine("void assignFromInt(int iNewValue);");
	text.printLine("int getInt() const;");
	text.printLine("const types value() const;");
	text.printLine();
	text.printLine("operator const types() const;");
	text.printLine("operator const ", nameType.c_str(), "s() const;");
	text.printLine();

	text.printLineNoIndent("private:");
	text.printLine("void setup();");
	text.printLine("void setupLength();");
	text.printLine();
	text.printLine("types m_Value;");


	text.addEndBracket(true);

	writeDefinesEnd(text, bHardcoded);

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

void EnumGen::writeDefinesStart(class OutputHandler& text, bool bHardcoded)
{
	if (m_type == HardcodingClass::TYPE_STANDARD && !bHardcoded)
	{
		if (!bHardcoded)
		{
			std::string name = m_name;
			name.append("Type");

			text.printLineNoIndent("#ifdef HARDCODE_XML_VALUES");
			text.printLine("#include \"AUTO_", name.c_str(), "_HARDCODED.h\"");
			text.printLineNoIndent("#else // HARDCODE_XML_VALUES");
		}
		else
		{
			text.printLineNoIndent("#ifdef HARDCODE_XML_VALUES");
		}
		text.printLine();
	}
}

void EnumGen::writeDefinesEnd(class OutputHandler& text, bool bHardcoded)
{
	if (m_type == HardcodingClass::TYPE_STANDARD && !bHardcoded)
	{
		text.printLine();
		text.printLineNoIndent("#endif // HARDCODE_XML_VALUES");
	}
}

void EnumGen::writeEnum(class OutputHandler& text, bool bHardcoded)
{
	bHardcoded |= m_type.isAlwaysHardcoded();

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
		text.printLine("static const types& NUM;");
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

	/*
	if (m_type == HardcodingClass::TYPE_STANDARD)
	{
		text.printLineNoIndent("#ifndef HARDCODE_XML_VALUES");
		text.printLine(type, "::types LOCAL_NUM;");
		text.printLine("const ", type, "::types& ", type, "::NUM = LOCAL_NUM;");
		text.printLineNoIndent("#endif // HARDCODE_XML_VALUES");
		text.printLine();
	}
	*/

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

	text.printLine("const ", type, "::types ", type, "::value() const");
	text.addStartBracket();
	text.printLine("return m_Value;");
	text.addEndBracket();
	text.printLine();

	text.printLine(type, "::operator const ", type, "::types() const");
	text.addStartBracket();
	text.printLine("return m_Value;");
	text.addEndBracket();
	text.printLine();

	text.printLine(type, "::operator const ", type, "s() const");
	text.addStartBracket();
	text.printLine("return static_cast<", type, "s>(m_Value);");
	text.addEndBracket();
	text.printLine();

	// fill in file name
	std::string name = m_name;
	name.append("Type.cpp");

	// write file if out of date
	text.saveFile(name.c_str());
}
