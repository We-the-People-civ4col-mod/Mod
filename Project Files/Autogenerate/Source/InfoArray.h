#pragma once

#include <vector>
#include "LineString.h"
#include "OutputHandler.h"

class InfoArray
{
public:
	class InfoArrayVars
	{
		enum types
		{
			TYPE_TYPES,
			TYPE_TYPE,
			TYPE_VAR,
		};

	public:
		InfoArrayVars();
		InfoArrayVars(const std::string& file, const std::size_t offset);

		void setup();

		bool isValid() const;
		int size() const;
		LineString getType(int iIndex) const;
		LineString getTypeWithPrefix(int iIndex) const;
		LineString getName(int iIndex) const;
		LineString getBitSize(int iIndex, bool bHardcoded) const;

		bool operator< (const InfoArrayVars& rhs) const;
		bool operator== (const InfoArrayVars& rhs) const;
	private:
		std::vector<LineString> m_variables;
		std::vector<types> m_types;
	};

	InfoArray();

private:
	void addClass(const InfoArrayVars& variables);

	void func_constructor();
	void func_deconstructor();
	void func_length();
	void func_get(unsigned int iVarIndex);

	unsigned int m_MaxLength;
	OutputHandler m_header;
	OutputHandler m_cpp;
};
