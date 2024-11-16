#pragma once

#include <vector>
#include <string>

#include "LineString.h"

class EnumMapGen
{
	class EnumMapTypes
	{
	public:
		EnumMapTypes();
		EnumMapTypes(std::string input);
		bool isValid() const;

		bool operator< (const EnumMapTypes& rhs) const;
		bool operator== (const EnumMapTypes& rhs) const;

	private:
		LineString m_index;
		LineString m_varable;
	};


public:
	EnumMapGen();

private:
	std::vector<EnumMapTypes> m_types;
};
