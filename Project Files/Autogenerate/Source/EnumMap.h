#pragma once

#include <vector>
#include <string>

#include "LineString.h"
#include "OutputHandler.h"

class EnumMapGen
{
	class EnumMapTypes
	{
	public:
		enum EnumMapTypeEnum
		{
			EnumMap_Bool,
			EnumMap_Variable,
			EnumMap_Class,
			EnumMap_Type,
			EnumMap_Types,
			EnumMap_Special,
		};

		enum EnumMapArrayType
		{
			EnumMapArray_Standard,
			EnumMapArray_Bool,
			EnumMapArray_Bool_static_1,
			EnumMapArray_Bool_static_2,
			EnumMapArray_Bool_dynamic_1,
			EnumMapArray_Bool_dynamic_2,
		};

		EnumMapTypes();
		EnumMapTypes(std::string input);
		bool isValid() const;

		void setup(OutputHandler* header, OutputHandler* cpp, OutputHandler* inlineFile);
		void printFile();

		const char* index() const;
		const char* indexDec() const;
		const char* var() const;
		const char* varDec() const;

		bool operator< (const EnumMapTypes& rhs) const;
		bool operator== (const EnumMapTypes& rhs) const;

	private:
		void func_constructor();
		void func_deconstructor();
		void func_isAllocated();
		void func_allocate();
		void func_allocateInternal();
		void func_reset();
		void func_hasContent();
		void func_get();
		void func_set();
		void func_add();
		void func_setAll();
		void func_addAll();
		void func_keepMin();
		void func_keepMax();
		void func_getMin();
		void func_getMax();
		void func_getTotal();
		void func_getTotalBool();
		void func_arrayAccess();
		void func_arrayAccessConst();
		void func_toVector();
		void func_fromVector();

		void addArrayVar();

		void printRangeCheck(const char* varName);


		static EnumMapTypeEnum getType(const std::string&);
		LineString classAddDec(const std::string&);

		EnumMapTypeEnum m_typesIndex;
		EnumMapTypeEnum m_typesVariable;
		LineString m_index;
		LineString m_indexDec;
		LineString m_varable;
		LineString m_varableDec;
		LineString m_default;
		LineString m_BoolDefault;
		bool m_bDefaultZero;
		OutputHandler* m_file_header;
		OutputHandler* m_file_cpp;
		OutputHandler* m_file_inline;

		LineString m_fullname;
		LineString m_fullnameDec;

		const class EnumGen* m_EnumDataIndex;
		const class EnumGen* m_EnumDataVar;

		EnumMapArrayType m_ArrayType;

		LineString m_NUM;
		LineString m_varName;
		bool m_bIsEnumMap;
	};


public:
	EnumMapGen();

private:
	std::vector<EnumMapTypes> m_types;
	OutputHandler m_file_header;
	OutputHandler m_file_cpp;
	OutputHandler m_file_inline;
};
