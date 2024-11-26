#include <stdio.h>

#include "assert.h"
#include "EnumGen.h"
#include "FileAccess.h"
#include "OutputHandler.h"

EnumGen::InfoSource::InfoSource()
	: value(GC)
{}

void EnumGen::InfoSource::assign(const char* newType)
{
	if (strcmp(newType, "GC") == 0)
	{
		value = GC;
	}
	else if (strcmp(newType, "INFO") == 0)
	{
		value = INFO;
	}
	else if (strcmp(newType, "ENUMMAP") == 0)
	{
		value = ENUMMAP;
	}
	else
	{
		assert(false);
	}
}

EnumGen::InfoSource::var EnumGen::InfoSource::getVar() const
{
	return value;
}

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

bool EnumGen::HardcodingClass::hasInfo() const
{
	switch (value)
	{
	case TYPE_STANDARD:
	case TYPE_HARDCODED:
	case TYPE_DYNAMIC:
		return true;
	case TYPE_NO_INFO:
	case TYPE_NO_FILE:
		return false;
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
	, m_bSecondRun(false)
{
	if (num != NULL)
	{
		m_num.assign(num);
	}

	EnumVec.push_back(*this);
}

EnumGen::EnumGen(class Element file)
	: m_bSecondRun(false)
{
	m_name.assign(file.name());
	m_name.erase(m_name.size()-5, 5);
	m_nameType.assign(m_name);
	m_nameType.append("Type");

	Element TypeElement = file.FirstChild("HardcodingType");
	if (TypeElement.isValid())
	{
		m_type.assign(TypeElement.getText());
	}

	Element InfoSource = file.FirstChild("InfoSource");
	if (InfoSource.isValid())
	{
		m_InfoSource.assign(InfoSource.getText());
	}
	else
	{
		// TODO auto set EnumMap as source
	}

	Element NumName = file.FirstChild("NumName");
	if (NumName.isValid())
	{
		m_num.assign(NumName.getText());
	}
	
	Element GetInfo = file.FirstChild("GetInfo");
	if (GetInfo.isValid())
	{
		m_GetInfo.assign(GetInfo.getText());
	}
	else
	{
		m_GetInfo.assign("get");
		m_GetInfo.append(m_name);
		m_GetInfo.append("Info");
	}

	Element InfoClass = file.FirstChild("InfoClass");
	if (InfoClass.isValid())
	{
		m_InfoClass.assign(InfoClass.getText());
	}
	else
	{
		m_InfoClass.assign("Cv");
		m_InfoClass.append(m_name);
		m_InfoClass.append("Info");
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

void EnumGen::generateFiles()
{
	writeCPPStart();
	writeFile();
	writeCPP();
	if (type() == HardcodingClass::TYPE_STANDARD)
	{
		file_header.reset();
		m_bSecondRun = true;
		writeFile();
	}
}

void EnumGen::writeFile()
{
	file_header.printLineNoIndent("#pragma once\n");

	writeDefinesStart();

	file_header.printLine("class ", m_nameType.c_str());
	file_header.addStartBracket();
	file_header.printLine("friend class CvXMLLoadUtility;");
	file_header.printLineNoIndent("public:");

	writeEnum();

	func_constructor();
	func_constructor_type();
	file_header.printLine();

	func_fromInt();
	func_toInt();
	func_conversion_operator_value();
	func_conversion_operator_enum();
	func_conversion_operator_types();

	file_header.printLine();
	func_info_static();
	func_info();
	file_header.printLine();
	func_range_static();
	func_range();
	file_header.printLine();


	file_header.printLineNoIndent("private:");
	//text.printLine("void setup();");
	//text.printLine("void setupLength();");
	file_header.printLine();
	file_header.printLine("types m_Value;");


	file_header.addEndBracket(true);

	writeDefinesEnd();

	// fill in file name
	std::string name = m_name;
	name.append("Type");
	if (m_bSecondRun)
	{
		name.append("_HARDCODED");
	}
	name.append(".h");

	// write file if out of date
	file_header.saveFile(name.c_str());
}

void EnumGen::writeDefinesStart()
{
	if (m_type == HardcodingClass::TYPE_STANDARD && !m_bSecondRun)
	{
		if (!m_bSecondRun)
		{
			std::string name = m_name;
			name.append("Type");

			file_header.printLineNoIndent("#ifdef HARDCODE_XML_VALUES");
			file_header.printLine("#include \"AUTO_", name.c_str(), "_HARDCODED.h\"");
			file_header.printLineNoIndent("#else // HARDCODE_XML_VALUES");
			file_header.printLine();
		}
		else
		{
			file_header.printLineNoIndent("#ifdef HARDCODE_XML_VALUES");
		}
	}
}

void EnumGen::writeDefinesEnd()
{
	if (m_type == HardcodingClass::TYPE_STANDARD && !m_bSecondRun)
	{
		file_header.printLine();
		file_header.printLineNoIndent("#endif // HARDCODE_XML_VALUES");
	}
}

void EnumGen::writeEnum()
{
	bool bHardcoded = m_bSecondRun || m_type.isAlwaysHardcoded();

	file_header.printLine("enum types");
	file_header.addStartBracket();
	file_header.printLine("NONE = -1,");

	if (bHardcoded)
	{
		for (unsigned i = 0; i < m_types.size(); ++i)
		{
			file_header.printLine(m_types[i].c_str(), ",");
		}
		file_header.printLine("NUM_ENTRIES,");
	}
	file_header.addEndBracket(true);

	file_header.printLine("");
	if (!bHardcoded)
	{
		file_header.printLine("static const types& NUM_ENTRIES;");
	}
}


void EnumGen::writeCPPStart()
{
	switch (m_InfoSource.getVar())
	{
	case InfoSource::GC:
		file_cpp.printLine("#include \"../CvGameCoreDLL.h\"");
		break;
	case InfoSource::INFO:
		file_cpp.printLine("#include \"../CvGameCoreDLL.h\"");
		file_cpp.printLine("#include \"../GlobalInfos.h\"");
		break;
	case InfoSource::ENUMMAP:
		assert(0);
		break;
	default:
		assert(0);
	}

	
	file_cpp.printLine("#include \"AUTO_", m_nameType, ".h\"");
	file_cpp.printLine();
}

void EnumGen::writeCPP()
{
	// fill in file name
	std::string name = m_name;
	name.append("Type.cpp");

	// write file if out of date
	file_cpp.saveFile(name.c_str());
}

void EnumGen::func_constructor()
{
	file_header.printLine(m_nameType, "();");

	if (m_bSecondRun) return;

	file_cpp.printLine(m_nameType, "::", m_nameType, "()");
	file_cpp.printLine("\t: m_Value(", m_nameType, "::NONE)");
	file_cpp.addStartBracket();
	file_cpp.addEndBracket();
	file_cpp.printLine();

}
void EnumGen::func_constructor_type()
{
	file_header.printLine(m_nameType, "(enum ", m_nameType, "s);");

	if (m_bSecondRun) return;

	file_cpp.printLine(m_nameType, "::", m_nameType, "(", m_nameType, "s val)");
	file_cpp.printLine("\t: m_Value(static_cast<", m_nameType, "::types>(val))");
	file_cpp.addStartBracket();
	file_cpp.addEndBracket();
	file_cpp.printLine();
}

void EnumGen::func_fromInt()
{
	file_header.printLine("void assignFromInt(int iNewValue);");

	if (m_bSecondRun) return;

	file_cpp.printLine("void ", m_nameType, "::assignFromInt(int iNewValue)");
	file_cpp.addStartBracket();
	file_cpp.printLine("m_Value = static_cast<types>(iNewValue);");
	file_cpp.addEndBracket();
	file_cpp.printLine();
}

void EnumGen::func_toInt()
{
	file_header.printLine("int toInt() const;");

	if (m_bSecondRun) return;

	file_cpp.printLine("int ", m_nameType, "::toInt() const");
	file_cpp.addStartBracket();
	file_cpp.printLine("return m_Value;");
	file_cpp.addEndBracket();
	file_cpp.printLine();
}

void EnumGen::func_conversion_operator_value()
{
	file_header.printLine("const types value() const;");

	if (m_bSecondRun) return;

	file_cpp.printLine("const ", m_nameType, "::types ", m_nameType, "::value() const");
	file_cpp.addStartBracket();
	file_cpp.printLine("return m_Value;");
	file_cpp.addEndBracket();
	file_cpp.printLine();
}

void EnumGen::func_conversion_operator_enum()
{
	file_header.printLine("operator const types() const;");

	if (m_bSecondRun) return;

	file_cpp.printLine(m_nameType, "::operator const ", m_nameType, "::types() const");
	file_cpp.addStartBracket();
	file_cpp.printLine("return m_Value;");
	file_cpp.addEndBracket();
	file_cpp.printLine();
}


void EnumGen::func_conversion_operator_types()
{
	file_header.printLine("operator const ", m_nameType, "s() const;");

	if (m_bSecondRun) return;

	file_cpp.printLine(m_nameType, "::operator const ", m_nameType, "s() const");
	file_cpp.addStartBracket();
	file_cpp.printLine("return static_cast<", m_nameType, "s>(m_Value);");
	file_cpp.addEndBracket();
	file_cpp.printLine();
}

void EnumGen::func_info_static()
{
	if (!m_type.hasInfo()) return;

	file_header.printLine("static const class ", m_InfoClass, "& info(", m_nameType, " eIndex);");

	if (m_bSecondRun) return;

	file_cpp.printLine("const ", m_InfoClass, "& ", m_nameType, "::info(", m_nameType, " eIndex)");
	file_cpp.addStartBracket();
	file_cpp.printLine("FAssert(eIndex.isInRange());");

	switch (m_InfoSource.getVar())
	{
	case InfoSource::GC:
		file_cpp.printLine("return GC.", m_GetInfo, "(static_cast<", m_nameType, "s>(eIndex.toInt()));");
		break;
	case InfoSource::INFO:
		file_cpp.printLine("return INFO.getInfo(static_cast<", m_nameType, "s>(eIndex.toInt()));");
		break;
	case InfoSource::ENUMMAP:
		assert(0);
		break;
	default:
		assert(0);
	}
	file_cpp.addEndBracket();
	file_cpp.printLine();
}

void EnumGen::func_info()
{
	if (!m_type.hasInfo()) return;

	file_header.printLine("const class ", m_InfoClass, "& info() const;");

	if (m_bSecondRun) return;

	file_cpp.printLine("const ", m_InfoClass, "& ", m_nameType, "::info() const");
	file_cpp.addStartBracket();
	file_cpp.printLine("FAssert(isInRange());");
	switch (m_InfoSource.getVar())
	{
	case InfoSource::GC:
		file_cpp.printLine("return GC.", m_GetInfo, "(static_cast<", m_nameType, "s>(m_Value));");
		break;
	case InfoSource::INFO:
		file_cpp.printLine("return INFO.getInfo(static_cast<", m_nameType, "s>(m_Value));");
		break;
	case InfoSource::ENUMMAP:
		assert(0);
		break;
	default:
		assert(0);
	}
	file_cpp.addEndBracket();
	file_cpp.printLine();
}

void EnumGen::func_range_static()
{
	file_header.printLine("static bool isInRange(types eValue);");

	if (m_bSecondRun) return;

	file_cpp.printLine("bool ", m_nameType, "::isInRange(types eValue)");
	file_cpp.addStartBracket();
	file_cpp.printLine("return eValue > NONE && eValue < NUM_ENTRIES;");
	file_cpp.addEndBracket();
	file_cpp.printLine();
}

void EnumGen::func_range()
{
	file_header.printLine("bool isInRange() const;");

	if (m_bSecondRun) return;

	file_cpp.printLine("bool ", m_nameType, "::isInRange() const");
	file_cpp.addStartBracket();
	file_cpp.printLine("return m_Value > NONE && m_Value < NUM_ENTRIES;");
	file_cpp.addEndBracket();
	file_cpp.printLine();
}

