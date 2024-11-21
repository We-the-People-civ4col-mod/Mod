#include "assert.h"
#include "Constants.h"
#include "EnumGen.h"
#include "EnumMap.h"
#include "SourceReader.h"
#include <algorithm>


EnumMapGen::EnumMapTypes::EnumMapTypes()
	: m_EnumDataIndex(NULL)
	, m_EnumDataVar(NULL)
	, m_ArrayType(EnumMapArray_Standard)
	, m_bDefaultZero(true)
{
}

EnumMapGen::EnumMapTypes::EnumMapTypes(std::string input)
	: m_EnumDataIndex(NULL)
	, m_EnumDataVar(NULL)
	, m_ArrayType(EnumMapArray_Standard)
	, m_bDefaultZero(true)
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
			m_bDefaultZero = m_default.compare("0") == 0;
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
		m_EnumDataIndex = EnumGen::getEntry(m_index.substr(0, m_index.size() - 4));
		// fallthrough
	case EnumMap_Class:
		m_indexDec.assign("class ");
		m_indexDec.append(m_index);
		break;
	case EnumMap_Types:
	case EnumMap_Special:
		m_indexDec.assign("enum ");
		m_indexDec.append(m_index);
		m_EnumDataIndex = EnumGen::getEntry(m_index.substr(0, m_index.size() - 5));
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

	if (m_typesVariable == EnumMap_Bool)
	{
		const int iLength = m_EnumDataIndex->length();

		if (iLength <= 32)
		{
			m_ArrayType = m_EnumDataIndex->isStatic() ? EnumMapArray_Bool_static_1 : EnumMapArray_Bool_dynamic_1;
		}
		else if (iLength <= 64)
		{
			m_ArrayType = m_EnumDataIndex->isStatic() ? EnumMapArray_Bool_static_2 : EnumMapArray_Bool_dynamic_2;
		}
		else
		{
			m_ArrayType = EnumMapArray_Bool;
		}
	}

	switch (m_typesIndex)
	{
	case EnumMap_Type:
		m_NUM = m_index;
		m_NUM.append("::NUM");
		break;
	case EnumMap_Types:
	case EnumMap_Special:
	{
		std::string name = m_EnumDataIndex->name();
		std::transform(name.begin(), name.end(), name.begin(), ::toupper);
		m_NUM.assign("NUM_");
		m_NUM.append(name);
		m_NUM.append("_TYPES");
		break;
	}
	}
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

void EnumMapGen::EnumMapTypes::printFile()
{
	m_file_header->printLine("template <>");
	m_file_header->printLine("class ", ENUMMAP_NAME, "<", indexDec(), ", ", varDec(), ", ", m_default, ">");
	
	m_file_header->addStartBracket();
	m_file_header->printLineNoIndent("public:");
	
	func_constructor();
	func_deconstructor();

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
	
	m_file_header->printLineNoIndent("private:");
	addArrayVar();

	m_file_header->addEndBracket(true);
	m_file_header->printLine();
}

void EnumMapGen::EnumMapTypes::func_constructor()
{
	m_file_header->printLine(ENUMMAP_NAME, "();");

	m_file_cpp->printLine("template <>");
	m_file_cpp->printLine(m_fullname, "::", ENUMMAP_NAME, "()");
	switch (m_ArrayType)
	{
	case EnumMapArray_Standard:
		m_file_cpp->printLine("\t: m_pArray(NULL)");
		m_file_cpp->addStartBracket();
		m_file_cpp->addEndBracket();
		break;
	case EnumMapArray_Bool:
		m_file_cpp->printLine("\t: m_pArray(NULL)");
		m_file_cpp->addStartBracket();
		m_file_cpp->addEndBracket();
		break;
	case EnumMapArray_Bool_static_1:
		m_file_cpp->addStartBracket();
		if (!m_bDefaultZero)
		{
			m_file_cpp->printLine("m_Array.setAll(true);");
		}
		m_file_cpp->addEndBracket();
		break;
	case EnumMapArray_Bool_static_2:
		m_file_cpp->addStartBracket();
		if (!m_bDefaultZero)
		{
			m_file_cpp->printLine("m_Array[0].setAll(true);");
			m_file_cpp->printLine("m_Array[1].setAll(true);");
		}
		m_file_cpp->addEndBracket();
		break;
	case EnumMapArray_Bool_dynamic_1:
		m_file_cpp->addStartBracket();
		m_file_cpp->printLineNoIndent("#ifdef HARDCODE_XML_VALUES");
		m_file_cpp->printLine("m_Array.setAll(", m_default, ");");
		m_file_cpp->printLineNoIndent("#else");
		m_file_cpp->printLine("if (", m_NUM, " <= 32)");
		m_file_cpp->addStartBracket();
		m_file_cpp->printLine("m_Array.setAll(", m_default, ");");
		m_file_cpp->addEndBracket();
		m_file_cpp->printLine("else");
		m_file_cpp->addStartBracket();
		m_file_cpp->printLine("m_pArray = NULL;");
		m_file_cpp->addEndBracket();
		m_file_cpp->printLineNoIndent("#endif");
		m_file_cpp->addEndBracket();
		break;
	case EnumMapArray_Bool_dynamic_2:
		m_file_cpp->addStartBracket();
		m_file_cpp->printLineNoIndent("#ifdef HARDCODE_XML_VALUES");
		m_file_cpp->printLine("m_Array.setAll(", m_default, ");");
		m_file_cpp->printLineNoIndent("#else");
		m_file_cpp->printLine("m_pArray = NULL;");
		m_file_cpp->printLineNoIndent("#endif");
		m_file_cpp->addEndBracket();
		break;
	}
	m_file_cpp->printLine();
}

void EnumMapGen::EnumMapTypes::func_deconstructor()
{
	m_file_header->printLine("~", ENUMMAP_NAME, "();");

	m_file_cpp->printLine("template <>");
	m_file_cpp->printLine(m_fullname, "::~", ENUMMAP_NAME, "()");

	m_file_cpp->addStartBracket();
	switch (m_ArrayType)
	{
	case EnumMapArray_Standard:
	case EnumMapArray_Bool:
		m_file_cpp->printLine("SAFE_DELETE_ARRAY(m_pArray);");
		break;
	case EnumMapArray_Bool_static_1:
	case EnumMapArray_Bool_static_2:
		break;
	case EnumMapArray_Bool_dynamic_1:
		m_file_cpp->printLineNoIndent("#ifndef HARDCODE_XML_VALUES");
		m_file_cpp->printLine("if (", m_NUM, " > 32)");
		m_file_cpp->addStartBracket();
		m_file_cpp->printLine("SAFE_DELETE_ARRAY(m_pArray);");
		m_file_cpp->addEndBracket();
		m_file_cpp->printLineNoIndent("#endif");
		break;
	case EnumMapArray_Bool_dynamic_2:
		m_file_cpp->printLineNoIndent("#ifndef HARDCODE_XML_VALUES");
		m_file_cpp->printLine("SAFE_DELETE_ARRAY(m_pArray);");
		m_file_cpp->printLineNoIndent("#endif");
		break;
	}
	m_file_cpp->addEndBracket();
	m_file_cpp->printLine();
}

void EnumMapGen::EnumMapTypes::func_isAllocated()
{
	m_file_header->printLine("bool isAllocated() const;");

	m_file_cpp->printLine("template <>");
	m_file_cpp->printLine("bool ", m_fullname, "::isAllocated() const");
	m_file_cpp->addStartBracket();

	switch (m_ArrayType)
	{
	case EnumMapArray_Standard:
	case EnumMapArray_Bool:
		m_file_cpp->printLine("return m_pArray != NULL;");
		break;
	case EnumMapArray_Bool_static_1:
	case EnumMapArray_Bool_static_2:
		m_file_cpp->printLine("return true;");
		break;
	case EnumMapArray_Bool_dynamic_1:
		m_file_cpp->printLineNoIndent("#ifdef HARDCODE_XML_VALUES");
		m_file_cpp->printLine("return true;");
		m_file_cpp->printLineNoIndent("#else");
		m_file_cpp->printLine("return ", m_NUM, " <= 32 || m_pArray != NULL;");
		m_file_cpp->printLineNoIndent("#endif");
		break;
	case EnumMapArray_Bool_dynamic_2:
		m_file_cpp->printLineNoIndent("#ifdef HARDCODE_XML_VALUES");
		m_file_cpp->printLine("return true;");
		m_file_cpp->printLineNoIndent("#else");
		m_file_cpp->printLine("return m_pArray != NULL;");
		m_file_cpp->printLineNoIndent("#endif");
		break;
	}
	m_file_cpp->addEndBracket();
	m_file_cpp->printLine();
}

void EnumMapGen::EnumMapTypes::func_allocate()
{
	m_file_header->printLine("void allocate();");

	m_file_cpp->printLine("template <>");
	m_file_cpp->printLine("void ", m_fullname, "::allocate()");
	m_file_cpp->addStartBracket();

	switch (m_ArrayType)
	{
	case EnumMapArray_Standard:
		m_file_cpp->printLine("if (m_pArray != NULL)");
		m_file_cpp->addStartBracket();
		m_file_cpp->printLine("m_pArray = new ", var(), "[", m_NUM, "];");
		if (m_typesVariable != EnumMap_Class) // class constructor already called
		{
			if (m_bDefaultZero)
			{
				m_file_cpp->printLine("memset(m_pArray, 0, ", m_NUM, " * sizeof(", var(), "));");
			}
			else if (m_default.compare("-1") == 0)
			{
				m_file_cpp->printLine("memset(m_pArray, 0xFF, ", m_NUM, " * sizeof(", var(), "));");
			}
			else
			{
				m_file_cpp->printLine("for (int i = 0; i < ", m_NUM, "; ++i)");
				m_file_cpp->addStartBracket();
				m_file_cpp->printLine("m_pArray[i] = static_cast<", var(), ">(i);");
				m_file_cpp->addEndBracket();
			}
		}
		m_file_cpp->addEndBracket();
		break;
	case EnumMapArray_Bool:
		m_file_cpp->printLine("if (m_pArray != NULL)");
		m_file_cpp->addStartBracket();
		m_file_cpp->printLine("const int size = (", m_NUM, " + 31)/32;");
		m_file_cpp->printLine("m_pArray = new EnumArrayToken[size];");
		if (!m_bDefaultZero)
		{
			m_file_cpp->printLine("for (int i = 0; i < size; ++i)");
			m_file_cpp->addStartBracket();
			m_file_cpp->printLine("m_pArray[i].setAll(true);");
			m_file_cpp->addEndBracket();
		}
		m_file_cpp->addEndBracket();
		break;
	case EnumMapArray_Bool_static_1:
	case EnumMapArray_Bool_static_2:
		break;
	case EnumMapArray_Bool_dynamic_1:
		m_file_cpp->printLineNoIndent("#ifndef HARDCODE_XML_VALUES");
		m_file_cpp->printLine("if (", m_NUM, " > 32 && m_pArray == NULL)");
		m_file_cpp->addStartBracket();
		m_file_cpp->printLine("const int size = (", m_NUM, " + 31)/32;");
		m_file_cpp->printLine("m_pArray = new EnumArrayToken[size];");
		if (!m_bDefaultZero)
		{
			m_file_cpp->printLine("for (int i = 0; i < size; ++i)");
			m_file_cpp->addStartBracket();
			m_file_cpp->printLine("m_pArray[i].setAll(true);");
			m_file_cpp->addEndBracket();
		}
		m_file_cpp->addEndBracket();

		m_file_cpp->printLine("return ", m_NUM, " <= 32 || m_pArray != NULL;");
		m_file_cpp->printLineNoIndent("#endif");
		break;
	case EnumMapArray_Bool_dynamic_2:
		m_file_cpp->printLineNoIndent("#ifndef HARDCODE_XML_VALUES");
		m_file_cpp->printLine("const int size = (", m_NUM, " + 31)/32;");
		m_file_cpp->printLine("m_pArray = new EnumArrayToken[size];");
		if (!m_bDefaultZero)
		{
			m_file_cpp->printLine("for (int i = 0; i < size; ++i)");
			m_file_cpp->addStartBracket();
			m_file_cpp->printLine("m_pArray[i].setAll(true);");
			m_file_cpp->addEndBracket();
		}
		m_file_cpp->printLineNoIndent("#endif");
		break;
	}
	m_file_cpp->addEndBracket();
	m_file_cpp->printLine();
}

void EnumMapGen::EnumMapTypes::func_reset()
{
	m_file_header->printLine("void reset();");

	m_file_cpp->printLine("template <>");
	m_file_cpp->printLine("void ", m_fullname, "::reset()");
	m_file_cpp->addStartBracket();

	switch (m_ArrayType)
	{
	case EnumMapArray_Standard:
	case EnumMapArray_Bool:
		m_file_cpp->printLine("SAFE_DELETE_ARRAY(m_pArray);");
		break;
	case EnumMapArray_Bool_static_1:
		m_file_cpp->printLine("m_Array.setAll(", m_default, ");");
		break;
	case EnumMapArray_Bool_static_2:
		m_file_cpp->printLine("m_Array[0].setAll(", m_default, ");");
		m_file_cpp->printLine("m_Array[1].setAll(", m_default, ");");
		break;
	case EnumMapArray_Bool_dynamic_1:
		m_file_cpp->printLineNoIndent("#ifdef HARDCODE_XML_VALUES");
		m_file_cpp->printLine("m_Array.setAll(", m_default, ");");
		m_file_cpp->printLineNoIndent("#else");
		m_file_cpp->printLine("if (", m_NUM, " > 32)");
		m_file_cpp->addStartBracket();
		m_file_cpp->printLine("SAFE_DELETE_ARRAY(m_pArray);");
		m_file_cpp->addEndBracket();
		m_file_cpp->printLine("else");
		m_file_cpp->addStartBracket();
		m_file_cpp->printLine("m_Array.setAll(", m_default, ");");
		m_file_cpp->addEndBracket();
		m_file_cpp->printLineNoIndent("#endif");
		break;
	case EnumMapArray_Bool_dynamic_2:
		m_file_cpp->printLineNoIndent("#ifdef HARDCODE_XML_VALUES");
		m_file_cpp->printLine("m_Array[0].setAll(", m_default, ");");
		m_file_cpp->printLine("m_Array[1].setAll(", m_default, ");");
		m_file_cpp->printLineNoIndent("#else");
		m_file_cpp->printLine("SAFE_DELETE_ARRAY(m_pArray);");
		m_file_cpp->printLineNoIndent("#endif");
		break;
	}
	m_file_cpp->addEndBracket();
	m_file_cpp->printLine();
}

void EnumMapGen::EnumMapTypes::func_hasContent()
{
	m_file_header->printLine("bool hasContent() const;");

	m_file_cpp->printLine("template <>");
	m_file_cpp->printLine("bool ", m_fullname, "::hasContent() const");
	m_file_cpp->addStartBracket();

	const LineString szDefault(m_bDefaultZero ? "0" : "0xFFFFFFFF");

	switch (m_ArrayType)
	{
	case EnumMapArray_Standard:
		m_file_cpp->printLine("if (m_pArray == NULL) return false;");
		m_file_cpp->printLine("for (int i = 0; i < ", m_NUM, "; ++i)");
		m_file_cpp->addStartBracket();
		if (m_typesVariable == EnumMap_Class)
		{
			m_file_cpp->printLine("if (!m_pArray[i].hasContent()) return false;");
		}
		else
		{
			m_file_cpp->printLine("if (m_pArray[i] != static_cast<", var(), ">(", m_default, ") return false;");
		}
		m_file_cpp->addEndBracket();
		break;
	case EnumMapArray_Bool:
		m_file_cpp->printLine("if (m_pArray == NULL) return false;");
		m_file_cpp->printLine("const int size = (", m_NUM, " + 31)/32;");
		m_file_cpp->printLine("for (int i = 0; i < size; ++i)");
		m_file_cpp->addStartBracket();
		m_file_cpp->printLine("if (m_pArray[i].getInt() != ", szDefault, ") return false;");
		m_file_cpp->addEndBracket();
		break;
	case EnumMapArray_Bool_static_1:
		m_file_cpp->printLine("return m_Array.getInt() != ", szDefault, ";");
		break;
	case EnumMapArray_Bool_static_2:
		m_file_cpp->printLine("return m_pArray[0].getInt() != ", szDefault, " || m_pArray[1].getInt() != ", szDefault, ";");
		break;
	case EnumMapArray_Bool_dynamic_1:
		m_file_cpp->printLineNoIndent("#ifdef HARDCODE_XML_VALUES");
		m_file_cpp->printLine("return m_Array.getInt() != ", szDefault, ";");
		m_file_cpp->printLineNoIndent("#else");
		m_file_cpp->printLine("if (", m_NUM, " > 32)");
		m_file_cpp->addStartBracket();
		m_file_cpp->printLine("if (m_pArray == NULL) return false;");
		m_file_cpp->printLine("const int size = (", m_NUM, " + 31)/32;");
		m_file_cpp->printLine("for (int i = 0; i < size; ++i)");
		m_file_cpp->addStartBracket();
		m_file_cpp->printLine("if (m_pArray[i].getInt() != ", szDefault, ") return false;");
		m_file_cpp->addEndBracket();
		m_file_cpp->addEndBracket();
		m_file_cpp->printLine("else");
		m_file_cpp->addStartBracket();
		m_file_cpp->printLine("return m_Array.getInt() != ", szDefault, ";");
		m_file_cpp->addEndBracket();
		m_file_cpp->printLineNoIndent("#endif");
		break;
	case EnumMapArray_Bool_dynamic_2:
		m_file_cpp->printLineNoIndent("#ifdef HARDCODE_XML_VALUES");
		m_file_cpp->printLine("return m_pArray[0].getInt() != ", szDefault, " || m_pArray[1].getInt() != ", szDefault, ";");
		m_file_cpp->printLineNoIndent("#else");
		m_file_cpp->printLine("if (m_pArray == NULL) return false;");
		m_file_cpp->printLine("const int size = (", m_NUM, " + 31)/32;");
		m_file_cpp->printLine("for (int i = 0; i < size; ++i)");
		m_file_cpp->addStartBracket();
		m_file_cpp->printLine("if (m_pArray[i].getInt() != ", szDefault, ") return false;");
		m_file_cpp->addEndBracket();
		m_file_cpp->printLineNoIndent("#endif");
		break;
	}
	m_file_cpp->printLine("return true;");
	m_file_cpp->addEndBracket();
	m_file_cpp->printLine();
}

void EnumMapGen::EnumMapTypes::func_get()
{
	m_file_header->printLine(varDec(), " get(", indexDec(), "eIndex) const;");

	m_file_cpp->printLine("template <>");
	m_file_cpp->printLine(var(), " ", m_fullname, "::get(", index(), " eIndex) const");
	m_file_cpp->addStartBracket();
	printRangeCheck("eIndex");

	switch (m_ArrayType)
	{
	case EnumMapArray_Standard:
		m_file_cpp->printLine("if (m_pArray == NULL) return static_cast<", var(), ">(", m_default, ");");
		m_file_cpp->printLine("return m_pArray[eIndex]);");
		break;
	case EnumMapArray_Bool:
		m_file_cpp->printLine("if (m_pArray == NULL) return ", m_default, ";");
		m_file_cpp->printLine("return m_pArray[eIndex >> 5].get(eIndex);");
		break;
	case EnumMapArray_Bool_static_1:
		m_file_cpp->printLine("return m_Array.get(eIndex);");
		break;
	case EnumMapArray_Bool_static_2:
		m_file_cpp->printLine("return m_Array[eIndex >> 5].get(eIndex);");
		break;
	case EnumMapArray_Bool_dynamic_1:
		m_file_cpp->printLineNoIndent("#ifdef HARDCODE_XML_VALUES");
		m_file_cpp->printLine("return m_Array.get(eIndex);");
		m_file_cpp->printLineNoIndent("#else");
		m_file_cpp->printLine("if (", m_NUM, " > 32)");
		m_file_cpp->addStartBracket();
		m_file_cpp->printLine("if (m_pArray == NULL) return ", m_default, ";");
		m_file_cpp->printLine("return m_pArray[eIndex]);");
		m_file_cpp->addEndBracket();
		m_file_cpp->printLine("else");
		m_file_cpp->addStartBracket();
		m_file_cpp->printLine("return m_Array.get(eIndex);");
		m_file_cpp->addEndBracket();
		m_file_cpp->printLineNoIndent("#endif");
		break;
	case EnumMapArray_Bool_dynamic_2:
		m_file_cpp->printLineNoIndent("#ifdef HARDCODE_XML_VALUES");
		m_file_cpp->printLine("return m_Array[eIndex >> 5].get(eIndex);");
		m_file_cpp->printLineNoIndent("#else");
		m_file_cpp->printLine("if (m_pArray == NULL) return ", m_default, ";");
		m_file_cpp->printLine("return m_Array[eIndex >> 5].get(eIndex);");
		m_file_cpp->printLineNoIndent("#endif");
		break;
	}
	m_file_cpp->addEndBracket();
	m_file_cpp->printLine();
}

void EnumMapGen::EnumMapTypes::func_set()
{
	m_file_header->printLine("void set(", indexDec(), " eIndex, ", varDec(), " bValue);");


	m_file_cpp->printLine("template <>");
	m_file_cpp->printLine(var(), " ", m_fullname, "::set(", index(), " eIndex, ", var(), "bvalue) const");
	m_file_cpp->addStartBracket();
	printRangeCheck("eIndex");

	switch (m_ArrayType)
	{
	case EnumMapArray_Standard:
		m_file_cpp->printLine("if (!isAllocated() && bValue == ", m_default, ") return;");
		m_file_cpp->printLine("allocate();");
		break;
	case EnumMapArray_Bool:
		m_file_cpp->printLine("if (!isAllocated() && ", m_bDefaultZero ? "!" : "", "bValue) return;");
		m_file_cpp->printLine("allocate();");
		break;
	case EnumMapArray_Bool_dynamic_1:
		m_file_cpp->printLineNoIndent("#ifndef HARDCODE_XML_VALUES");
		m_file_cpp->printLine("if (", m_NUM, " > 32)");
		m_file_cpp->addStartBracket();
		m_file_cpp->printLine("if (!isAllocated() && ", m_bDefaultZero ? "!" : "", "bValue) return;");
		m_file_cpp->printLine("allocate();");
		m_file_cpp->addEndBracket();
		m_file_cpp->printLineNoIndent("#endif");
		break;
	case EnumMapArray_Bool_dynamic_2:
		m_file_cpp->printLineNoIndent("#ifndef HARDCODE_XML_VALUES");
		m_file_cpp->printLine("if (!isAllocated() && ", m_bDefaultZero ? "!" : "", "bValue) return;");
		m_file_cpp->printLine("allocate();");
		m_file_cpp->printLineNoIndent("#endif");
		break;
	}
	

	switch (m_ArrayType)
	{
	case EnumMapArray_Standard:
		
		m_file_cpp->printLine("m_pArray[eIndex] = bValue;");
		break;
	case EnumMapArray_Bool:
		m_file_cpp->printLine("m_pArray[eIndex >> 5].set(eIndex, bValue);");
		break;
	case EnumMapArray_Bool_static_1:
		m_file_cpp->printLine("m_Array.set(eIndex, bValue);");
		break;
	case EnumMapArray_Bool_static_2:
		m_file_cpp->printLine("m_Array[eIndex >> 5].set(eIndex, bValue);");
		break;
	case EnumMapArray_Bool_dynamic_1:
		m_file_cpp->printLineNoIndent("#ifdef HARDCODE_XML_VALUES");
		m_file_cpp->printLine("m_Array[eIndex >> 5].set(eIndex, bValue);");
		m_file_cpp->printLineNoIndent("#else");
		m_file_cpp->printLine("if (", m_NUM, " > 32)");
		m_file_cpp->addStartBracket();
		m_file_cpp->printLine("m_pArray[eIndex >> 5].set(eIndex, bValue);");
		m_file_cpp->addEndBracket();
		m_file_cpp->printLine("else");
		m_file_cpp->addStartBracket();
		m_file_cpp->printLine("m_Array[eIndex >> 5].set(eIndex, bValue);");
		m_file_cpp->addEndBracket();
		m_file_cpp->printLineNoIndent("#endif");
		break;
	case EnumMapArray_Bool_dynamic_2:
		m_file_cpp->printLineNoIndent("#ifdef HARDCODE_XML_VALUES");
		m_file_cpp->printLine("m_Array[eIndex >> 5].set(eIndex, bValue);");
		m_file_cpp->printLineNoIndent("#else");
		m_file_cpp->printLine("m_Array.set(eIndex, bValue);");
		m_file_cpp->printLineNoIndent("#endif");
		break;
	}
	m_file_cpp->addEndBracket();
	m_file_cpp->printLine();
}

void EnumMapGen::EnumMapTypes::func_add()
{
	m_file_header->printLine("void add(", indexDec(), " eIndex, ", varDec(), " bValue);");

	m_file_cpp->printLine("template <>");
	m_file_cpp->printLine(var(), " ", m_fullname, "::set(", index(), " eIndex, ", var(), "bvalue)");
	m_file_cpp->addStartBracket();
	printRangeCheck("eIndex");
	assert(m_ArrayType == EnumMapArray_Standard);

	
	m_file_cpp->printLine("if ( bValue == 0) return;");
	m_file_cpp->printLine("allocate();");
	m_file_cpp->printLine("m_pArray[eIndex] += bValue;");

	m_file_cpp->addEndBracket();
	m_file_cpp->printLine();
}

void EnumMapGen::EnumMapTypes::func_setAll()
{
	m_file_header->printLine("void setAll(", varDec(), " bValue);");
}

void EnumMapGen::EnumMapTypes::func_addAll()
{
	m_file_header->printLine("void addAll(", varDec(), " Value);");

	m_file_cpp->printLine("template <>");
	m_file_cpp->printLine(var(), " ", m_fullname, "::addAll(", var(), "value)");
	m_file_cpp->addStartBracket();
	printRangeCheck("eIndex");
	assert(m_ArrayType == EnumMapArray_Standard);


	m_file_cpp->printLine("if ( bValue == 0) return;");
	m_file_cpp->printLine("allocate();");
	m_file_cpp->printLine("for (int i = 0; i < ", m_NUM, "; ++i)");
	m_file_cpp->addStartBracket();
	m_file_cpp->printLine("m_pArray[i] += Value;");
	m_file_cpp->addEndBracket();

	m_file_cpp->addEndBracket();
	m_file_cpp->printLine();
}

void EnumMapGen::EnumMapTypes::func_keepMin()
{
	m_file_header->printLine("void keepMin(", indexDec(), " eIndex, ", varDec(), " Value);");

	m_file_cpp->printLine("template <>");
	m_file_cpp->printLine("void ", m_fullname, "::keepMin(", index(), " eIndex, ", var(), "value)");
	m_file_cpp->addStartBracket();
	printRangeCheck("eIndex");
	assert(m_ArrayType == EnumMapArray_Standard);

	m_file_cpp->printLine("allocate();");
	m_file_cpp->printLine("const ", var(), " temp = m_pArray[eIndex];");
	m_file_cpp->printLine("m_pArray[eIndex] = Value < temp ? Value : temp;");

	m_file_cpp->addEndBracket();
	m_file_cpp->printLine();
}

void EnumMapGen::EnumMapTypes::func_keepMax()
{
	m_file_header->printLine("void keepMax(", indexDec(), " eIndex, ", varDec(), " Value);");

	m_file_cpp->printLine("template <>");
	m_file_cpp->printLine("void ", m_fullname, "::keepMax(", index(), " eIndex, ", var(), "value)");
	m_file_cpp->addStartBracket();
	printRangeCheck("eIndex");
	assert(m_ArrayType == EnumMapArray_Standard);

	m_file_cpp->printLine("allocate();");
	m_file_cpp->printLine("const ", var(), " temp = m_pArray[eIndex];");
	m_file_cpp->printLine("m_pArray[eIndex] = Value > temp ? Value : temp;");

	m_file_cpp->addEndBracket();
	m_file_cpp->printLine();
}

void EnumMapGen::EnumMapTypes::func_getMin()
{
	m_file_header->printLine(varDec(), " getMin() const;");

	m_file_cpp->printLine("template <>");
	m_file_cpp->printLine(var(), " ", m_fullname, "::getMin()");
	m_file_cpp->addStartBracket();
	assert(m_ArrayType == EnumMapArray_Standard);

	m_file_cpp->printLine("if (!isAllocated()) return ", m_default, ";");
	m_file_cpp->printLine(var(), " iMin = m_pArray[0];");
	m_file_cpp->printLine("for (int i = 1; i < ", m_NUM, "; ++i)");
	m_file_cpp->addStartBracket();
	m_file_cpp->printLine("const ", var(), " iTemp = m_pArray[i];");
	m_file_cpp->printLine("if (iMin > iTemp) iMin = iTemp;");
	m_file_cpp->addEndBracket();
	m_file_cpp->printLine("return iMin;");

	m_file_cpp->addEndBracket();
	m_file_cpp->printLine();
}

void EnumMapGen::EnumMapTypes::func_getMax()
{
	m_file_header->printLine(varDec(), " getMax() const;");

	m_file_cpp->printLine("template <>");
	m_file_cpp->printLine(var(), " ", m_fullname, "::getMax()");
	m_file_cpp->addStartBracket();
	assert(m_ArrayType == EnumMapArray_Standard);

	m_file_cpp->printLine("if (!isAllocated()) return ", m_default, ";");
	m_file_cpp->printLine(var(), " iMax = m_pArray[0];");
	m_file_cpp->printLine("for (int i = 1; i < ", m_NUM, "; ++i)");
	m_file_cpp->addStartBracket();
	m_file_cpp->printLine("const ", var(), " iTemp = m_pArray[i];");
	m_file_cpp->printLine("if (iMax < iTemp) iMax = iTemp;");
	m_file_cpp->addEndBracket();
	m_file_cpp->printLine("return iMax;");

	m_file_cpp->addEndBracket();
	m_file_cpp->printLine();
}

void EnumMapGen::EnumMapTypes::func_getTotal()
{
	m_file_header->printLine("int getTotal() const;");

	m_file_cpp->printLine("template <>");
	m_file_cpp->printLine("int ", m_fullname, "::getTotal()");
	m_file_cpp->addStartBracket();
	assert(m_ArrayType == EnumMapArray_Standard);

	m_file_cpp->printLine("if (!isAllocated()) return ", m_default, " * ", m_NUM, ";");
	m_file_cpp->printLine("int iSum = 0;");
	m_file_cpp->printLine("for (int i = 1; i < ", m_NUM, "; ++i)");
	m_file_cpp->addStartBracket();
	m_file_cpp->printLine("iSum += m_pArray[i];");
	m_file_cpp->addEndBracket();
	m_file_cpp->printLine("return iSum;");

	m_file_cpp->addEndBracket();
	m_file_cpp->printLine();
}

void EnumMapGen::EnumMapTypes::func_getTotalBool()
{
	m_file_header->printLine("int getTotal() const;");

	m_file_cpp->printLine("template <>");
	m_file_cpp->printLine("int ", m_fullname, "::getTotal()");
	m_file_cpp->addStartBracket();
	m_file_cpp->printLine("if (!isAllocated())");
	m_file_cpp->addStartBracket();
	m_file_cpp->printLine("return ", m_bDefaultZero ? "0" : m_NUM, ";");
	m_file_cpp->addEndBracket();
	m_file_cpp->printLine("int iSum = 0;");
	m_file_cpp->printLine("for (int i = 0; i < ", m_NUM, "; ++i)");
	m_file_cpp->addStartBracket();
	m_file_cpp->printLine("if (get(static_cast<", index(), ">(i)) ++iSum;");
	m_file_cpp->addEndBracket();
	m_file_cpp->printLine("return iSum;");
	m_file_cpp->addEndBracket();
	m_file_cpp->printLine();
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

void EnumMapGen::EnumMapTypes::addArrayVar()
{
	switch (m_ArrayType)
	{
	case EnumMapArray_Standard:
		m_file_header->printLine(varDec(), "* m_pArray;");
		break;
	case EnumMapArray_Bool:
		m_file_header->printLine("EnumArrayToken* m_pArray;");
		break;
	case EnumMapArray_Bool_static_1:
		m_file_header->printLine("EnumArrayToken m_Array;");
		break;
	case EnumMapArray_Bool_static_2:
		m_file_header->printLine("EnumArrayToken m_Array[2];");
		break;
	case EnumMapArray_Bool_dynamic_1:
		m_file_header->printLineNoIndent("#ifdef HARDCODE_XML_VALUES");
		m_file_header->printLine("EnumArrayToken m_Array;");
		m_file_header->printLineNoIndent("#else");
		m_file_header->printLine("union");
		m_file_header->addStartBracket();
		m_file_header->printLine("EnumArrayToken m_Array;");
		m_file_header->printLine("EnumArrayToken* m_pArray;");
		m_file_header->addEndBracket(true);
		m_file_header->printLineNoIndent("#endif");
		break;
	case EnumMapArray_Bool_dynamic_2:
		m_file_header->printLineNoIndent("#ifdef HARDCODE_XML_VALUES");
		m_file_header->printLine("EnumArrayToken m_Array[2];");
		m_file_header->printLineNoIndent("#else");
		m_file_header->printLine("EnumArrayToken* m_pArray;");
		m_file_header->printLineNoIndent("#endif");
		break;
	}
}

void EnumMapGen::EnumMapTypes::printRangeCheck(const char* varName)
{
	switch (m_typesIndex)
	{
	case EnumMap_Type:
		m_file_cpp->printLine("FAssert(", varName, ".isInRange());");
		break;
	case EnumMap_Types:
		m_file_cpp->printLine("FAssert(VARINFO<", index(), ">::isInRange(", varName, "));");
		break;
	case EnumMap_Special:
		break;
	default:
		assert(0);
	}
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
	m_file_header.printLine("class EnumArrayToken");
	m_file_header.addStartBracket();
	m_file_header.printLineNoIndent("public:");
	m_file_header.printLine("void set(int iIndex, bool bValue);");
	m_file_header.printLine("bool get(int iIndex) const;");
	m_file_header.printLine("void setAll(bool bValue);");
	m_file_header.printLine("bool hasNonDefault(bool bDefault) const;");
	m_file_header.printLineNoIndent("private:");
	m_file_header.printLine("union");
	m_file_header.addStartBracket();
	m_file_header.printLine("unsigned int m_iVar;");
	m_file_header.printLine("struct");
	m_file_header.addStartBracket();

	char* int_buffer = new char[10];
	for (int i = 0; i < 32; i++)
	{
		sprintf(int_buffer, "%i", i);
		m_file_header.printLine("bool var_", int_buffer, ";");
	}
	m_file_header.addEndBracket(true);
	m_file_header.addEndBracket(true);
	m_file_header.addEndBracket(true);
	m_file_header.printLine("BOOST_STATIC_ASSERT(sizeof(EnumArrayToken) == 4);");



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
		it_cutoff->printFile();
	}
	m_file_header.saveFile("EnumMap.h");
	m_file_cpp.saveFile("EnumMap.cpp");
}
