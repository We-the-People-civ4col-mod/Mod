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

		EnumMapTypes();
		EnumMapTypes(std::string input);
		bool isValid() const;

		void setup(OutputHandler* header, OutputHandler* cpp);
		void printHeader(OutputHandler&);

		const char* index() const;
		const char* indexDec() const;
		const char* var() const;
		const char* varDec() const;

		bool operator< (const EnumMapTypes& rhs) const;
		bool operator== (const EnumMapTypes& rhs) const;

	private:
		void func_isAllocated();
		void func_allocate();
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


		static EnumMapTypeEnum getType(const std::string&);

		EnumMapTypeEnum m_typesIndex;
		EnumMapTypeEnum m_typesVariable;
		LineString m_index;
		LineString m_indexDec;
		LineString m_varable;
		LineString m_varableDec;
		LineString m_default;
		OutputHandler* m_file_header;
		OutputHandler* m_file_cpp;

		LineString m_fullname;
		LineString m_fullnameDec;

		const class EnumGen* m_EnumDataIndex;
		const class EnumGen* m_EnumDataVar;
	};


public:
	EnumMapGen();

private:
	std::vector<EnumMapTypes> m_types;
	OutputHandler m_file_header;
	OutputHandler m_file_cpp;
};
