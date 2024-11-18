#include "assert.h"
#include "Constants.h"
#include "EnumGen.h"
#include "EnumMap.h"
#include "SourceReader.h"
#include <algorithm>


EnumMapGen::EnumMapTypes::EnumMapTypes()
	: m_EnumDataIndex(NULL)
	, m_EnumDataVar(NULL)
{
}

EnumMapGen::EnumMapTypes::EnumMapTypes(std::string input)
	: m_EnumDataIndex(NULL)
	, m_EnumDataVar(NULL)
{
	m_default.assign("0");

	std::size_t offset = input.find(",");
	m_index = input.substr(0, offset);
	m_index.trim();
	m_varable = input.substr(offset + 1);
	m_varable.killLeadingWhitespace();

	if (m_varable.compare(0, 7, "EnumMap") == 0)
	{
		m_varable.trim();
		m_varable.append(">");
	}
	else
	{
		offset = m_varable.find(",");
		if (offset != std::string::npos)
		{
			m_default = m_varable.substr(offset+1);
			m_default.trim();
			m_varable = m_varable.substr(0, offset);
		}
		m_varable.trim();
	}
}

void EnumMapGen::EnumMapTypes::setup(OutputHandler* header, OutputHandler* cpp)
{
	m_file_header = header;
	m_file_cpp = cpp;

	m_typesIndex = getType(m_index);
	m_typesVariable = getType(m_varable);

	switch (m_typesIndex)
	{
	case EnumMap_Bool:
	case EnumMap_Variable:
		m_indexDec = m_index;
		break;
	case EnumMap_Type:
		m_EnumDataVar = EnumGen::getEntry(m_index.substr(0, m_index.size() - 4));
		// fallthrough
	case EnumMap_Class:
		m_indexDec.assign("class ");
		m_indexDec.append(m_index);
		break;
	case EnumMap_Types:
	case EnumMap_Special:
		m_indexDec.assign("enum ");
		m_indexDec.append(m_index);
		m_EnumDataVar = EnumGen::getEntry(m_index.substr(0, m_index.size() - 5));
	};

	switch (m_typesVariable)
	{
	case EnumMap_Bool:
	case EnumMap_Variable:
		m_varableDec = m_varable;
		break;
	case EnumMap_Type:
		m_EnumDataVar = EnumGen::getEntry(m_varable.substr(0, m_varable.size() - 4));
		// fallthrough
	case EnumMap_Class:
		m_varableDec.assign("class ");
		m_varableDec.append(m_varable);
		break;
	case EnumMap_Types:
	case EnumMap_Special:
		m_varableDec.assign("enum ");
		m_varableDec.append(m_varable);
		m_EnumDataVar = EnumGen::getEntry(m_varable.substr(0, m_varable.size() - 5));
	};

	m_fullname.assign("EnumMap<");
	m_fullname.append(m_index);
	m_fullname.append(", ");
	m_fullname.append(m_varable);
	m_fullname.append(", ");
	m_fullname.append(m_default);
	m_fullname.append(">");


	m_fullnameDec.assign("EnumMap<");
	m_fullnameDec.append(m_indexDec);
	m_fullnameDec.append(", ");
	m_fullnameDec.append(m_varableDec);
	m_fullnameDec.append(", ");
	m_fullnameDec.append(m_default);
	m_fullnameDec.append(">");
}

const char* EnumMapGen::EnumMapTypes::index() const
{
	return m_index;
}

const char* EnumMapGen::EnumMapTypes::indexDec() const
{
	return m_indexDec;
}

const char* EnumMapGen::EnumMapTypes::var() const
{
	return m_varable;
}

const char* EnumMapGen::EnumMapTypes::varDec() const
{
	return m_varableDec;
}

void EnumMapGen::EnumMapTypes::printHeader(OutputHandler& file)
{
	file.printLine("template <>");
	file.printLine("class ", ENUMMAP_NAME, "<", indexDec(), ", ", varDec(), ", ", m_default, ">");
	
	file.addStartBracket();
	file.printLineNoIndent("public:");

	func_isAllocated();
	func_allocate();
	func_reset();
	func_hasContent();


	if (m_typesVariable != EnumMap_Class)
	{
		func_get();
		func_set();


		if (m_typesVariable == EnumMap_Variable)
		{
			func_add();
			func_setAll();
			func_addAll();
			func_keepMin();
			func_keepMax();
			func_getMin();
			func_getMax();
			func_getTotal();
		}
		else if (m_typesVariable == EnumMap_Bool)
		{
			func_getTotalBool();
		}
	}

	if (m_typesVariable == EnumMap_Class || m_typesVariable == EnumMap_Variable)
	{
		func_arrayAccess();
		func_arrayAccessConst();
	}

	if (m_typesVariable != EnumMap_Class)
	{
		func_toVector();
		func_fromVector();
	}
	
	file.printLineNoIndent("private:");

	file.addEndBracket(true);
	file.printLine();
}

void EnumMapGen::EnumMapTypes::func_isAllocated()
{
	m_file_header->printLine("bool isAllocated() const;");
}

void EnumMapGen::EnumMapTypes::func_allocate()
{
	m_file_header->printLine("void allocate();");
}

void EnumMapGen::EnumMapTypes::func_reset()
{
	m_file_header->printLine("void reset();");
}

void EnumMapGen::EnumMapTypes::func_hasContent()
{
	m_file_header->printLine("bool hasContent() const;");
}

void EnumMapGen::EnumMapTypes::func_get()
{
	m_file_header->printLine(varDec(), " get(", indexDec(), "eIndex) const;");
}

void EnumMapGen::EnumMapTypes::func_set()
{
	m_file_header->printLine("void set(", indexDec(), " eIndex, ", varDec(), " bValue);");
}

void EnumMapGen::EnumMapTypes::func_add()
{
	m_file_header->printLine("void add(", indexDec(), " eIndex, ", varDec(), " bValue);");
}

void EnumMapGen::EnumMapTypes::func_setAll()
{
	m_file_header->printLine("void setAll(", varDec(), " bValue);");
}

void EnumMapGen::EnumMapTypes::func_addAll()
{
	m_file_header->printLine("void addAll(", varDec(), " bValue);");
}

void EnumMapGen::EnumMapTypes::func_keepMin()
{
	m_file_header->printLine("void keepMin(", indexDec(), " eIndex, ", varDec(), " bValue);");
}

void EnumMapGen::EnumMapTypes::func_keepMax()
{
	m_file_header->printLine("void keepMax(", indexDec(), " eIndex, ", varDec(), " bValue);");
}

void EnumMapGen::EnumMapTypes::func_getMin()
{
	m_file_header->printLine(varDec(), " getMin(", indexDec(), " bValue) const;");
}

void EnumMapGen::EnumMapTypes::func_getMax()
{
	m_file_header->printLine(varDec(), " getMax(", indexDec(), " bValue) const;");
}

void EnumMapGen::EnumMapTypes::func_getTotal()
{
	m_file_header->printLine(varDec(), " getTotal() const;");
}

void EnumMapGen::EnumMapTypes::func_getTotalBool()
{
	m_file_header->printLine("int getTotal() const;");
}

void EnumMapGen::EnumMapTypes::func_arrayAccess()
{
	m_file_header->printLine(varDec(), "& operator[](", indexDec(), " eIndex);");
}

void EnumMapGen::EnumMapTypes::func_arrayAccessConst()
{
	m_file_header->printLine("const ", varDec(), "& operator[](", indexDec(), " eIndex) const;");
}

void EnumMapGen::EnumMapTypes::func_toVector()
{
	m_file_header->printLine("void copyToVector(std::vector<", varDec(), ">& thisVector) const;");
}

void EnumMapGen::EnumMapTypes::func_fromVector()
{
	m_file_header->printLine("void copyFromVector(const std::vector<", varDec(), ">& thisVector);");
}



EnumMapGen::EnumMapTypes::EnumMapTypeEnum EnumMapGen::EnumMapTypes::getType(const std::string& input)
{
	if (input == "CityPlotTypes") return EnumMap_Special;


	if (input == "bool") return EnumMap_Bool;
	if (input.size() > 5)
	{
		if (input.substr(input.size() - 5).compare("Types") == 0) return EnumMap_Types;
		if (input.substr(input.size() - 4).compare("Type") == 0) return EnumMap_Type;

		if (input.substr(input.size() - 4).compare("Info") == 0) return EnumMap_Class;
	}

	if (input.size() > 9 && input.substr(0, 9).compare("unsigned ") == 0) return EnumMap_Variable;
	if (input.compare("int") == 0
		|| input.compare("short") == 0
		|| input.compare("char") == 0
		) return EnumMap_Variable;

	if (input.size() > 7 && input.substr(0, 7).compare("EnumMap") == 0) return EnumMap_Class;

	assert(0);
	
	return EnumMap_Bool;
}

bool EnumMapGen::EnumMapTypes::isValid() const
{
	if (m_index.size() < 3)
	{
		return false;
	}

	if (m_index.substr(0, 4).compare("TYPE") == 0)
	{
		return false;
	}

	if (m_index.substr(0, 8).compare("VARIABLE") == 0)
	{
		return false;
	}
	if (m_varable.size() < 3)
	{
		return false;
	}

	if (m_varable.substr(0, 4).compare("TYPE") == 0)
	{
		return false;
	}

	if (m_varable.substr(0, 8).compare("VARIABLE") == 0)
	{
		return false;
	}

	return true;
}

bool EnumMapGen::EnumMapTypes::operator < (const EnumMapTypes& rhs) const
{
	if (m_index != rhs.m_index)
	{
		return m_index < rhs.m_index;
	}
	if (m_varable != rhs.m_varable)
	{
		return m_varable < rhs.m_varable;
	}
	return m_default < rhs.m_default;
}

bool EnumMapGen::EnumMapTypes::operator == (const EnumMapTypes& rhs) const
{
	return m_index == rhs.m_index && m_varable == rhs.m_varable && m_default == rhs.m_default;
}


EnumMapGen::EnumMapGen()
{
	const std::vector<SourceFileContainer>& files = SourceFileList::getInstance().getFiles();


	
	for (std::vector<SourceFileContainer>::const_iterator it = files.begin(); it != files.end(); it++)
	{
		const std::string& str = it->getText();
		
		for (std::size_t offset = str.find("EnumMap<"); offset != std::string::npos; offset = str.find("EnumMap<", offset + 1))
		{
			std::size_t offset_end = str.find(">", offset);
			EnumMapTypes buffer = str.substr(offset + 8, offset_end - offset - 8);
			if (buffer.isValid())
			{
				m_types.push_back(buffer);
			}
		}
	}

	// remove dublicated EnumMap types
	std::sort(m_types.begin(), m_types.end());
	std::vector<EnumMapTypes>::iterator it_cutoff = std::unique(m_types.begin(), m_types.end());
	m_types.resize(std::distance(m_types.begin(), it_cutoff));

	// m_types now contains all EnumMap types mentioned in the source code without dublicates

	m_file_header.printLine("#pragma once");
	m_file_header.printLine();
	m_file_header.printLine("template <class IndexType, class T, int DEFAULT = 0>");
	m_file_header.printLine("class EnumMap");
	m_file_header.addStartBracket();
	m_file_header.printLine("BOOST_STATIC_ASSERT(false);");
	m_file_header.addEndBracket();
	m_file_header.printLine();


	for (it_cutoff = m_types.begin(); it_cutoff != m_types.end(); it_cutoff++)
	{
		it_cutoff->setup(&m_file_header, &m_file_cpp);
		it_cutoff->printHeader(m_file_header);
	}
	m_file_header.saveFile("EnumMap.h");
}
