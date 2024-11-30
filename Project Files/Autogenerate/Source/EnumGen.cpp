#include <algorithm>
#include <stdio.h>

#include "assert.h"
#include "EnumGen.h"
#include "FileAccess.h"
#include "OutputHandler.h"
#include "SourceReader.h"

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
	else if (strcmp(newType, "TYPE_PLAYER") == 0)
	{
		value = TYPE_PLAYER;
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
	case TYPE_PLAYER:
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
	case TYPE_PLAYER:
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
	, m_bHasOverrideFunctions(false)
	, m_bHasNamedValues(false)
	, m_typeTagName("")
{
	if (num != NULL)
	{
		m_num.assign(num);
	}

	EnumVec.push_back(*this);
}

EnumGen::EnumGen(class Element file)
	: m_bSecondRun(false)
	, m_bHasOverrideFunctions(false)
	, m_bHasNamedValues(true)
	, m_typeTagName("Type")

{
	m_name.assign(file.name());
	m_name.erase(m_name.size()-5, 5);
	m_nameType.assign(m_name);
	m_nameType.append("Type");

	bool bTypeSpecified = false;

	Element TypeTagNameElement = file.FirstChild("Type");
	if (TypeTagNameElement.isValid())
	{
		m_typeTagName = TypeTagNameElement.getText();
	}

	Element RedirectElement = file.FirstChild("Redirect");
	if (RedirectElement.isValid())
	{
		m_RedirectPath.assign(RedirectElement.getText());
	}

	Element TypeElement = file.FirstChild("HardcodingType");
	if (TypeElement.isValid())
	{
		bTypeSpecified = true;
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
		if (bTypeSpecified)
		{
			assert(m_type.isAlwaysHardcoded());
		}
		else
		{
			m_type = HardcodingClass::TYPE_NO_FILE;
		}
	}
	else
	{
		for (current_file = current_file.FirstChild("File"); current_file.isValid(); current_file = current_file.NextSibling("File"))
		{
			LineString filename = current_file.getText();
			FileAccessXML xml_file(filename);
			Element root = xml_file.getRoot();

			int iOffset = filename.find_last_of("/");
			if (iOffset == -1)
			{
				filename.empty();
			}
			else
			{
				filename = filename.substr(0, iOffset + 1);
			}
			std::string schemaPath = root.Attribute("xmlns");
			schemaPath = schemaPath.substr(schemaPath.find(":") + 1);
			filename.append(schemaPath.c_str());
			FileAccessXML xml_schema(filename);

			Element schema_root = xml_schema.getRoot();
			schema_root = schema_root.FirstChild();

			readFile(root, schema_root);
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
		m_bHasNamedValues = false;
		Element lengthTag = file.FirstChild("iLength");
		if (lengthTag.isValid())
		{
			m_iLength = lengthTag.getInt();
		}
		else if (m_type.getVar() == HardcodingClass::TYPE_PLAYER)
		{
			m_iLength = SourceFileList::getInstance().getMaxPlayers();;
		}
		if (m_type.isAlwaysHardcoded())
		{
			// populate the enum with autogenerated names, which makes the debugger say for instance PLAYER_5 instead of the int 5

			LineString prefix(m_name);
			std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::toupper);
			prefix.append("_");

			for (int i = 0; i < m_iLength; ++i)
			{
				LineString buffer(prefix);
				buffer.append(i);
				m_types.push_back(buffer);
			}
		}
	}

	scanSourceFiles();

	EnumVec.push_back(*this);
}

void EnumGen::readFile(const Element xml_file, const Element schema_parent)
{
	const char* name = xml_file.name();

	Element schema = schema_parent;
	for (; schema.isValid() && strcmp(schema.Attribute("name"), name) != 0; schema = schema.NextSibling())
	{
	}

	Element child = xml_file.FirstChild();
	Element schema_child = schema.FirstChild();
	const char* maxOccurs = schema_child.Attribute("maxOccurs");

	if (maxOccurs == NULL || maxOccurs[0] != '*')
	{
		readFile(child, schema_parent);
		return;
	}

	const bool bChildType = m_typeTagName != NULL && strlen(m_typeTagName) > 0;

	const bool bHasTypes = !bChildType || child.FirstChild(m_typeTagName).isValid();

	for (; child.isValid(); child = child.NextSibling())
	{
		++m_iLength;

		if (!bHasTypes) continue;

		const char* tag = bChildType ? child.FirstChild(m_typeTagName).getText() : child.getText();

		m_types.push_back(tag);
	}
}

void EnumGen::scanSourceFiles()
{
	std::string searchPattern = m_nameType;
	searchPattern.append("::");

	// cache if the type is mentioned in the override file. Cache is used to skip scanning the file for each function later
	m_bHasOverrideFunctions = getOverrideFile().find(searchPattern) != std::string::npos;

	if (m_type.getVar() != HardcodingClass::TYPE_STANDARD)
	{
		// we are only interested in which enum values are in use if the enum can be both hardcoded and dynamic
		return;
	}

	const char* const SEARCH_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ_";

	const std::vector<SourceFileContainer>& files = SourceFileList::getInstance().getFiles();

	for (std::vector<SourceFileContainer>::const_iterator it = files.begin(); it != files.end(); it++)
	{
		const std::string& text = it->getText();

		for (size_t pos = text.find(searchPattern); pos != std::string::npos; pos = text.find(searchPattern, pos + 1))
		{
			pos += searchPattern.size();
			size_t pos_end = text.find_first_not_of(SEARCH_CHARS, pos);
			unsigned int iLength = pos_end - pos;
			if (iLength > 1)
			{
				std::string buffer = text.substr(pos, iLength);
				if (iLength != 4 || buffer.compare("NONE") != 0) // filter out NONE as there likely is a lot of those
				{
					m_used_enum_values.push_back(buffer);
				}
			}
		}
	}
	if (m_used_enum_values.size() > 1)
	{
		// sort and remove dublicates
		std::sort(m_used_enum_values.begin(), m_used_enum_values.end());
		std::vector<std::string>::iterator it = std::unique(m_used_enum_values.begin(), m_used_enum_values.end());
		m_used_enum_values.resize(std::distance(m_used_enum_values.begin(), it));
	}
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
	if (!m_RedirectPath.empty())
	{
		file_header.printLine("#pragma once");
		file_header.printLine("#include \"", m_RedirectPath, "\"");
		std::string name = m_name;
		name.append("Type.h");
		file_header.saveFile(name.c_str());
		return;
	}

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
	func_constructor_types();
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
	func_getString_static();
	func_getString();
	func_assignFromString();
	func_next();
	file_header.printLine();


	file_header.printLineNoIndent("private:");
	func_setupAuto();
	func_setupCustom();
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
		if (!m_bHasNamedValues) file_header.printLineNoIndent("#ifdef DEBUG");
		for (unsigned i = 0; i < m_types.size(); ++i)
		{
			file_header.printLine(m_types[i].c_str(), ",");
		}
		if (m_bHasNamedValues)
		{
			file_header.printLine("NUM_ENTRIES,");
		}
		else
		{
			LineString count;
			count.append(m_types.size());

			file_header.printLineNoIndent("#endif");
			file_header.printLine("NUM_ENTRIES = ", count, ",");
		}
	}
	file_header.addEndBracket(true);

	file_header.printLine("");
	if (!bHardcoded)
	{
		for (std::vector<std::string>::iterator it = m_used_enum_values.begin(); it != m_used_enum_values.end(); ++it)
		{
			file_header.printLine("static const types& ", it->c_str(), ";");
		}

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
		file_cpp.printLine("#include \"../Infos.h\"");
		break;
	case InfoSource::ENUMMAP:
		assert(0);
		break;
	default:
		assert(0);
	}

	
	file_cpp.printLine("#include \"AUTO_", m_nameType, ".h\"");
	file_cpp.printLine();

	if (m_used_enum_values.size() > 0)
	{
		file_cpp.printLineNoIndent("#ifndef HARDCODE_XML_VALUES");
		for (unsigned int i = 0; i < m_used_enum_values.size(); ++i)
		{
			LineString temp = m_used_enum_values[i];
			file_cpp.printLine(m_nameType, "::types LOCAL_", temp, " = ", m_nameType, "::NONE;");
			file_cpp.printLine("const ", m_nameType, "::types& ", m_nameType, "::", temp, " = LOCAL_", temp, ";");
		}

		file_cpp.printLineNoIndent("#endif");
	}
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

	if (hasOverride_PrintIfNot(m_nameType, "::", m_nameType, "()"))
	{
		return;
	}
	file_cpp.printLine("\t: m_Value(", m_nameType, "::NONE)");
	file_cpp.addStartBracket();
	file_cpp.addEndBracket();
	file_cpp.printLine();

}

void EnumGen::func_constructor_type()
{
	file_header.printLine(m_nameType, "(types);");

	if (m_bSecondRun) return;

	if (hasOverride_PrintIfNot(m_nameType, "::", m_nameType, "(", m_nameType, "::types val)"))
	{
		return;
	}
	file_cpp.printLine("\t: m_Value(val)");
	file_cpp.addStartBracket();
	file_cpp.addEndBracket();
	file_cpp.printLine();
}

void EnumGen::func_constructor_types()
{
	file_header.printLine(m_nameType, "(enum ", m_nameType, "s);");

	if (m_bSecondRun) return;

	if (hasOverride_PrintIfNot(m_nameType, "::", m_nameType, "(", m_nameType, "s val)"))
	{
		return;
	}
	file_cpp.printLine("\t: m_Value(static_cast<", m_nameType, "::types>(val))");
	file_cpp.addStartBracket();
	file_cpp.addEndBracket();
	file_cpp.printLine();
}

void EnumGen::func_fromInt()
{
	file_header.printLine("void assignFromInt(int iNewValue);");

	if (m_bSecondRun) return;

	if (hasOverride_PrintIfNot("void ", m_nameType, "::assignFromInt(int iNewValue)"))
	{
		return;
	}
	file_cpp.addStartBracket();
	file_cpp.printLine("m_Value = static_cast<types>(iNewValue);");
	file_cpp.addEndBracket();
	file_cpp.printLine();
}

void EnumGen::func_toInt()
{
	file_header.printLine("int toInt() const;");

	if (m_bSecondRun) return;

	if (hasOverride_PrintIfNot("int ", m_nameType, "::toInt() const"))
	{
		return;
	}
	file_cpp.addStartBracket();
	file_cpp.printLine("return m_Value;");
	file_cpp.addEndBracket();
	file_cpp.printLine();
}

void EnumGen::func_conversion_operator_value()
{
	file_header.printLine("const types value() const;");

	if (m_bSecondRun) return;

	if (hasOverride_PrintIfNot("const ", m_nameType, "::types ", m_nameType, "::value() const"))
	{
		return;
	}
	file_cpp.addStartBracket();
	file_cpp.printLine("return m_Value;");
	file_cpp.addEndBracket();
	file_cpp.printLine();
}

void EnumGen::func_conversion_operator_enum()
{
	file_header.printLine("operator const types() const;");

	if (m_bSecondRun) return;

	if (hasOverride_PrintIfNot(m_nameType, "::operator const ", m_nameType, "::types() const"))
	{
		return;
	}
	file_cpp.addStartBracket();
	file_cpp.printLine("return m_Value;");
	file_cpp.addEndBracket();
	file_cpp.printLine();
}


void EnumGen::func_conversion_operator_types()
{
	file_header.printLine("operator const ", m_nameType, "s() const;");

	if (m_bSecondRun) return;

	if (hasOverride_PrintIfNot(m_nameType, "::operator const ", m_nameType, "s() const"))
	{
		return;
	}
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

	if (hasOverride_PrintIfNot("const ", m_InfoClass, "& ", m_nameType, "::info(", m_nameType, " eIndex)"))
	{
		return;
	}
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

	if (hasOverride_PrintIfNot("const ", m_InfoClass, "& ", m_nameType, "::info() const"))
	{
		return;
	}
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

	if (hasOverride_PrintIfNot("bool ", m_nameType, "::isInRange(types eValue)"))
	{
		return;
	}
	file_cpp.addStartBracket();
	file_cpp.printLine("return eValue > NONE && eValue < NUM_ENTRIES;");
	file_cpp.addEndBracket();
	file_cpp.printLine();
}

void EnumGen::func_range()
{
	file_header.printLine("bool isInRange() const;");

	if (m_bSecondRun) return;

	if (hasOverride_PrintIfNot("bool ", m_nameType, "::isInRange() const"))
	{
		return;
	}
	file_cpp.addStartBracket();
	file_cpp.printLine("return m_Value > NONE && m_Value < NUM_ENTRIES;");
	file_cpp.addEndBracket();
	file_cpp.printLine();
}

void EnumGen::func_getString_static()
{
	if (!m_bHasNamedValues) return;

	file_header.printLine("static const char* getString(", m_nameType, " eIndex);");

	if (m_bSecondRun) return;

	if (hasOverride_PrintIfNot("const char* ", m_nameType, "::getString(", m_nameType, " eIndex)"))
	{
		return;
	}
	file_cpp.addStartBracket();
	
	if (m_type.hasInfo())
	{
		file_cpp.printLine("if (eIndex.isInRange())");
		file_cpp.addStartBracket();
		file_cpp.printLine("return eIndex.info().getType();");
		file_cpp.addEndBracket();
		file_cpp.printLine("else");
		file_cpp.addStartBracket();
		file_cpp.printLine("return \"\";");
		file_cpp.addEndBracket();
	}
	else
	{
		file_cpp.printLine("switch (eIndex.value())");
		file_cpp.addStartBracket();
		for (unsigned i = 0; i < m_types.size(); ++i)
		{
			const char* tempType = m_types[i].c_str();
			file_cpp.printLine("case ", tempType, ": return \"", tempType, "\";");
		}
		file_cpp.printLine("default: return \"\";");
		file_cpp.addEndBracket();
	}

	file_cpp.addEndBracket();
	file_cpp.printLine();
}

void EnumGen::func_getString()
{
	if (!m_bHasNamedValues) return;

	file_header.printLine("const char* getString() const;");

	if (m_bSecondRun) return;

	if (hasOverride_PrintIfNot("const char* ", m_nameType, "::getString() const"))
	{
		return;
	}
	file_cpp.addStartBracket();
	file_cpp.printLine("return getString(m_Value);");
	file_cpp.addEndBracket();
	file_cpp.printLine();
}

void EnumGen::func_assignFromString()
{
	if (!m_bHasNamedValues) return;

	file_header.printLine("void assignFromString(const char* szType);");

	if (m_bSecondRun) return;

	if (hasOverride_PrintIfNot("void ", m_nameType, "::assignFromString(const char* szType)"))
	{
		return;
	}
	file_cpp.addStartBracket();
	file_cpp.printLine("m_Value = NONE;");
	file_cpp.printLine("for (; next();)");
	file_cpp.addStartBracket();
	file_cpp.printLine("if (strcmp(szType, getString()) == 0) return;");
	file_cpp.addEndBracket();
	file_cpp.printLine("m_Value = NONE;");
	file_cpp.addEndBracket();
	file_cpp.printLine();
}

void EnumGen::func_next()
{
	file_header.printLine("bool next();");

	if (m_bSecondRun) return;

	if (hasOverride_PrintIfNot("bool ", m_nameType, "::next()"))
	{
		return;
	}
	file_cpp.addStartBracket();
	file_cpp.printLine("m_Value = static_cast<types>(m_Value+1);");
	file_cpp.printLine("return m_Value < NUM_ENTRIES;");
	file_cpp.addEndBracket();
	file_cpp.printLine();
}


void EnumGen::func_setupAuto()
{
	file_header.printLine("static void setupAuto();");

	if (m_bSecondRun) return;

	if (hasOverride_PrintIfNot("void ", m_nameType, "::setupAuto()"))
	{
		return;
	}
	file_cpp.addStartBracket();
	if (m_used_enum_values.size() > 0)
	{
		file_cpp.printLineNoIndent("#ifndef HARDCODE_XML_VALUES");
		file_cpp.printLine(m_nameType, " buffer;");
		for (unsigned int i = 0; i < m_used_enum_values.size(); ++i)
		{
			const char* enumTypeName = m_used_enum_values[i].c_str();
			file_cpp.printLine("buffer.assignFromString(\"", enumTypeName, "\");");
			file_cpp.printLine("LOCAL_", enumTypeName, " = buffer;");
		}
		file_cpp.printLineNoIndent("#endif");
	}
	file_cpp.addEndBracket();
	file_cpp.printLine();
}

void EnumGen::func_setupCustom()
{
	file_header.printLine("static void setupCustom();");

	if (m_bSecondRun) return;

	if (hasOverride_PrintIfNot("void ", m_nameType, "::setupCustom()"))
	{
		return;
	}
	file_cpp.addStartBracket();
	file_cpp.addEndBracket();
	file_cpp.printLine();
}

bool EnumGen::hasOverride_PrintIfNot(const char* arg)
{
	if (m_bHasOverrideFunctions)
	{
		if (getOverrideFile().find(arg) != std::string::npos)
		{
			return true;
		}
	}
	file_cpp.printLine(arg);
	return false;
}

bool EnumGen::hasOverride_PrintIfNot(const char* arg_1, const char* arg_2)
{
	LineString buffer = arg_1;
	buffer.append(arg_2);
	return hasOverride_PrintIfNot(buffer);
}

bool EnumGen::hasOverride_PrintIfNot(const char* arg_1, const char* arg_2, const char* arg_3)
{
	LineString buffer = arg_1;
	buffer.append(arg_2);
	buffer.append(arg_3);
	return hasOverride_PrintIfNot(buffer);
}

bool EnumGen::hasOverride_PrintIfNot(const char* arg_1, const char* arg_2, const char* arg_3, const char* arg_4)
{
	LineString buffer = arg_1;
	buffer.append(arg_2);
	buffer.append(arg_3);
	buffer.append(arg_4);
	return hasOverride_PrintIfNot(buffer);
}

bool EnumGen::hasOverride_PrintIfNot(const char* arg_1, const char* arg_2, const char* arg_3, const char* arg_4, const char* arg_5)
{
	LineString buffer = arg_1;
	buffer.append(arg_2);
	buffer.append(arg_3);
	buffer.append(arg_4);
	buffer.append(arg_5);
	return hasOverride_PrintIfNot(buffer);
}

bool EnumGen::hasOverride_PrintIfNot(const char* arg_1, const char* arg_2, const char* arg_3, const char* arg_4, const char* arg_5, const char* arg_6)
{
	LineString buffer = arg_1;
	buffer.append(arg_2);
	buffer.append(arg_3);
	buffer.append(arg_4);
	buffer.append(arg_5);
	buffer.append(arg_6);
	return hasOverride_PrintIfNot(buffer);
}

bool EnumGen::hasOverride_PrintIfNot(const char* arg_1, const char* arg_2, const char* arg_3, const char* arg_4, const char* arg_5, const char* arg_6, const char* arg_7)
{
	LineString buffer = arg_1;
	buffer.append(arg_2);
	buffer.append(arg_3);
	buffer.append(arg_4);
	buffer.append(arg_5);
	buffer.append(arg_6);
	buffer.append(arg_7);
	return hasOverride_PrintIfNot(buffer);
}

bool EnumGen::hasOverride_PrintIfNot(const char* arg_1, const char* arg_2, const char* arg_3, const char* arg_4, const char* arg_5, const char* arg_6, const char* arg_7, const char* arg_8)
{
	LineString buffer = arg_1;
	buffer.append(arg_2);
	buffer.append(arg_3);
	buffer.append(arg_4);
	buffer.append(arg_5);
	buffer.append(arg_6);
	buffer.append(arg_7);
	buffer.append(arg_8);
	return hasOverride_PrintIfNot(buffer);
}



void EnumGen::createInitFunction()
{
	const std::vector<EnumGen>& vec = getVector();

	OutputHandler includes;
	OutputHandler setupAuto;
	OutputHandler setupCustom;

	includes.printLine("#include \"../CvGameCoreDLL.h\"");
	includes.printLine("#include \"../CvXMLLoadUtility.h\"");
	includes.printLine();

	setupAuto.printLine("void CvXMLLoadUtility::EnumSetupAuto()");
	setupAuto.addStartBracket();

	setupCustom.printLine("void CvXMLLoadUtility::EnumSetupCustom()");
	setupCustom.addStartBracket();

	for (std::vector<EnumGen>::const_iterator it = vec.begin(); it != vec.end(); ++it)
	{
		includes.printLine("#include \"AUTO_", it->m_nameType, ".h\"");
		setupAuto.printLine(it->m_nameType, "::setupAuto();");
		setupCustom.printLine(it->m_nameType, "::setupCustom();");
	}
	includes.printLine();
	setupAuto.addEndBracket();
	setupAuto.printLine();
	setupCustom.addEndBracket();

	includes.printLine(setupAuto.getText());
	includes.printLine(setupCustom.getText());

	includes.saveFile("CvXMLLoadUtility.cpp");
}

const std::string& EnumGen::getOverrideFile()
{
	static const std::string& file = SourceFileList::getInstance().getFile("Types/EnumTypeOverride.cpp").getText();
	return file;
}
