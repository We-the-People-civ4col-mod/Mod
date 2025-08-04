#include "InfoArray.h"
#include "SourceReader.h"
#include "assert.h"
#include <algorithm>
#include "LineString.h"
#include "Constants.h"
#include "EnumGen.h"

std::vector<InfoArray::InfoArrayVars> arrayTypes;

bool InfoArray::InfoArrayVars::operator< (const InfoArrayVars& rhs) const
{
	const unsigned int iLeftSize = m_variables.size();
	const unsigned int iRightSize = rhs.m_variables.size();

	const unsigned int iMax = std::max(iLeftSize, iRightSize);

	for (unsigned int i = 0; i < iMax; ++i)
	{
		if (i >= iLeftSize)
		{
			return true;
		}
		if (i >= iRightSize)
		{
			return false;
		}
		if (m_variables[i] != rhs.m_variables[i])
		{
			return m_variables[i] < rhs.m_variables[i];
		}
	}

	return false;
}

bool InfoArray::InfoArrayVars::operator == (const InfoArrayVars& rhs) const
{
	const unsigned int iSize = m_variables.size();
	if (iSize != rhs.m_variables.size())
	{
		return false;
	}
	for (unsigned int i = 0; i < iSize; ++i)
	{
		if (m_variables[i] != rhs.m_variables[i])
		{
			return false;
		}
	}
	return true;
}

InfoArray::InfoArrayVars::InfoArrayVars()
{
}

void InfoArray::InfoArrayVars::setup()
{
	m_types.reserve(m_variables.size());
	std::string type;

	for (unsigned int i = 0; i < m_variables.size(); ++i)
	{
		type = m_variables[i];
		if (type.size() > 5 && type.substr(type.size() - 5).compare("Types") == 0)
		{
			m_types.push_back(TYPE_TYPES);
		}
		else if (type.size() > 4 && type.substr(type.size() - 4).compare("Type") == 0)
		{
			m_types.push_back(TYPE_TYPE);
		}
		else
		{
			m_types.push_back(TYPE_VAR);
		}
	}
}

InfoArray::InfoArrayVars::InfoArrayVars(const std::string& file, const std::size_t offset)
{
	for (std::size_t pos = offset + 9; file.c_str()[pos] != '>';)
	{
		++pos; // skip over commas

		std::size_t pos2 = file.find_first_of(",>", pos);
		if (pos == std::string::npos)
		{
			assert(0);
			break;
		}
		LineString temp = file.substr(pos, pos2 - pos);
		temp.trim();
		m_variables.push_back(temp);

		pos = pos2;
	}
}

bool InfoArray::InfoArrayVars::isValid() const
{
	for (unsigned int i = 0; i < m_variables.size(); ++i)
	{
		if (m_variables[i].size() < 3)
		{
			return false;
		}
		if (m_variables[i].compare("IndexType") == 0)
		{
			return false;
		}

	}
	return true;
}

int InfoArray::InfoArrayVars::size() const
{
	return m_variables.size();
}

LineString InfoArray::InfoArrayVars::getType(int iIndex) const
{
	return m_variables[iIndex];
}

LineString InfoArray::InfoArrayVars::getTypeWithPrefix(int iIndex) const
{
	LineString out;

	switch (m_types[iIndex])
	{
	case TYPE_TYPES:
		out.assign("enum ");
			break;

	case TYPE_TYPE:
		out.assign("class ");
		break;
	}
	
	out.append(m_variables[iIndex]);
	return out;
}

LineString InfoArray::InfoArrayVars::getName(int iIndex) const
{
	switch (m_types[iIndex])
	{
	case TYPE_TYPES:
		return m_variables[iIndex].substr(0, m_variables[iIndex].size() - 5);

	case TYPE_TYPE:
		return m_variables[iIndex].substr(0, m_variables[iIndex].size() - 4);
	}
	
	return "value";
}

LineString InfoArray::InfoArrayVars::getBitSize(int iIndex, bool bHardcoded) const
{
	switch (m_types[iIndex])
	{
	case TYPE_TYPES:
	{
		const EnumGen* entry = EnumGen::getEntry(getName(iIndex));
		const int iLength = entry->length();

		if (bHardcoded || entry->type().isAlwaysHardcoded())
		{
			
			int i = 1;
			while ((1 << i) < iLength)
			{
				++i;
			}
			return i;

			//return i < 127 ? "8" : "16";
		}
	}
		return "16";

	case TYPE_TYPE:
		return "16";
	}
	
	return "16";
}

InfoArray::InfoArray()
	: m_MaxLength(0)
{
	const std::vector<SourceFileContainer>& files = SourceFileList::getInstance().getFiles();

	for (std::vector<SourceFileContainer>::const_iterator it = files.begin(); it != files.end(); it++)
	{
		const std::string& text = it->getText();

		for (std::size_t offset = text.find("InfoArray<"); offset != std::string::npos; offset = text.find("InfoArray<", offset + 1))
		{
			InfoArrayVars temp(text, offset);

			if (temp.isValid())
			{
				arrayTypes.push_back(temp);
			}
		}
	}

	if (arrayTypes.size() > 1)
	{
		std::sort(arrayTypes.begin(), arrayTypes.end());
		std::vector<InfoArray::InfoArrayVars>::iterator it = std::unique(arrayTypes.begin(), arrayTypes.end());
		arrayTypes.resize(std::distance(arrayTypes.begin(), it));
	}

	std::vector<int> lengthTypes;
	lengthTypes.reserve(arrayTypes.size());

	for (unsigned int i = 0; i < arrayTypes.size(); ++i)
	{
		arrayTypes[i].setup();
		lengthTypes.push_back(arrayTypes[i].size());
	}

	if (lengthTypes.size() > 1)
	{
		std::sort(lengthTypes.begin(), lengthTypes.end());
		std::vector<int>::iterator it = std::unique(lengthTypes.begin(), lengthTypes.end());
		lengthTypes.resize(std::distance(lengthTypes.begin(), it));
	}

	//m_header.printLine("template<typename...> ", INFOARRAY_NAME, ";");
	
	for (unsigned int i = 0; i < lengthTypes.size(); ++i)
	{
		const unsigned int iLength = lengthTypes[i];
		if (iLength > m_MaxLength)
		{
			m_MaxLength = iLength;
		}
	}

	m_header.printLine();
	m_header.printLine("namespace InfoArrayTypes");
	m_header.addStartBracket();
	m_header.printLine("class ArrayTypeNone{};");
	m_header.printLine("enum ArrayType");
	m_header.addStartBracket();
	m_header.printLine("TYPE_CHAR,");
	m_header.printLine("TYPE_UCHAR,");
	m_header.printLine("TYPE_SHORT,");
	//m_header.printLine("TYPE_USHORT,");
	m_header.printLine("TYPE_POINTER_CHAR,");
	m_header.printLine("TYPE_POINTER_UCHAR,");
	m_header.printLine("TYPE_POINTER_SHORT,");
	//m_header.printLine("TYPE_POINTER_USHORT,");
	//m_header.printLine("TYPE_EXTRA_CHAR,");
	//m_header.printLine("TYPE_EXTRA_UCHAR,");
	//m_header.printLine("TYPE_EXTRA_SHORT,");
	//m_header.printLine("TYPE_EXTRA_USHORT,");
	m_header.addEndBracket(true);
	m_header.addEndBracket();
	m_header.printLine();

	{
		LineString output;
		output.append("template<class var_0");
		for (unsigned int j = 1; j < m_MaxLength; ++j)
		{
			output.append(", class var_");
			output.append(j);
			output.append(" = InfoArrayTypes::ArrayTypeNone");
		}
		output.append(">");

		m_header.printLine(output);
		m_header.printLine("class ", INFOARRAY_NAME);
		m_header.addStartBracket();
		m_header.addEndBracket(true);
		m_header.printLine();
	}

#if 0
	for (unsigned int i = 0; i < lengthTypes.size(); ++i)
	{
		const unsigned int iLength = lengthTypes[i];

		LineString output;
		output.append("template<class var_0");
		for (unsigned int j = 1; j < iLength; ++j)
		{
			output.append(", class var_");
			output.append(j);
		}
		output.append(">");
		
		m_header.printLine(output);
		m_header.printLine("class ", INFOARRAY_NAME, LineString(iLength));
		m_header.addStartBracket();
		m_header.addEndBracket(true);
		m_header.printLine();
	}
#endif

	for (unsigned int i = 0; i < arrayTypes.size(); ++i)
	{
		addClass(arrayTypes[i]);
	}



	m_header.saveFile("InfoArray.h");
}

void InfoArray::addClass(const InfoArray::InfoArrayVars& variables)
{
	const unsigned int NUM_VARIABLES = variables.size();

	m_header.printLine("template <>");

	LineString className;
	className.assign(INFOARRAY_NAME).append("<").append(variables.getTypeWithPrefix(0));
	//for (int i = 1; i < m_MaxLength; ++i)
	for (unsigned int i = 1; i < NUM_VARIABLES; ++i)
	{
		className.append(", ").append(variables.getTypeWithPrefix(i));
	}

	className.append(">");

	// #pragma pack(push,1) and #pragma pack(pop)

	m_header.printLine("class ", className);
	m_header.addStartBracket();
#if 0
	m_header.printLineNoIndent("public:");
	//m_header.printLineNoIndent("#pragma pack(push,1)");
	m_header.printLine("struct token");
	m_header.addStartBracket();
	switch (variables.size())
	{
	case 1:
		m_header.printLine(variables.getTypeWithPrefix(0), " ", variables.getName(0), ";");
		break;
	case 2:
		m_header.printLine(variables.getTypeWithPrefix(0), " ", variables.getName(0), " : 16;");
		m_header.printLine(variables.getTypeWithPrefix(1), " ", variables.getName(1), " : 16;");
		break;

	default:
		for (int i = 0; i < variables.size(); ++i)
		{
			m_header.printLine(variables.getTypeWithPrefix(i), " ", variables.getName(i), " :", variables.getBitSize(i, true), ";");
		}
		break;
	}

	//for (int i = 0; i < variables.size(); ++i)
	{
	//	m_header.printLine(variables.getTypeWithPrefix(i), " ", variables.getName(i), " :", variables.getBitSize(i), ";");
	}

	m_header.addEndBracket(true);
	//m_header.printLineNoIndent("#pragma pack(pop)");
	m_header.printLine("static const int iSize = sizeof(token);");
#endif
	m_header.printLineNoIndent("public:");

	func_constructor();
	func_deconstructor();
	func_length();

	for (unsigned int i = 0; i < NUM_VARIABLES; ++i)
	{
		func_get(i);
	}

	m_header.printLine();
	m_header.printLineNoIndent("private:");
	m_header.printLine("union");
	m_header.addStartBracket();
	if (NUM_VARIABLES == 3)
	{
		m_header.printLine("char m_Array_char[4];");
		m_header.printLine("unsigned char m_Array_uchar[4];");
	}
	else if (NUM_VARIABLES <= 4)
	{
		m_header.printLine("char m_Array_char[4];");
		m_header.printLine("unsigned char m_Array_uchar[4];");
		m_header.printLine("short m_Array_short[2];");
		//m_header.printLine("unsigned short m_Array_ushort[2];");
	}
	m_header.printLine("char* m_pArray_char;");
	m_header.printLine("unsigned char* m_pArray_uchar;");
	m_header.printLine("short* m_pArray_short;");
	//m_header.printLine("unsigned short* m_pArray_ushort;");
	

	m_header.addEndBracket(true);
	m_header.printLine("unsigned int m_iLength : 13;");
	m_header.printLine("InfoArrayTypes::ArrayType m_ArrayType : 3;");
	m_header.printLine("unsigned int m_iExtra : 16;");
	
	m_header.addEndBracket(true);
	m_header.printLine();
}

void InfoArray::func_constructor()
{
	m_header.printLine(INFOARRAY_NAME, "();");
}

void InfoArray::func_deconstructor()
{
	m_header.printLine("~", INFOARRAY_NAME, "();");
}

void InfoArray::func_length()
{
	m_header.printLine("unsigned int length() const;");
}

void InfoArray::func_get(unsigned int iVarIndex)
{
}
