#include "EnumMap.h"
#include "SourceReader.h"
#include <algorithm>


EnumMapGen::EnumMapTypes::EnumMapTypes()
{
}

EnumMapGen::EnumMapTypes::EnumMapTypes(std::string input)
{
	std::size_t offset = input.find(",");
	m_index = input.substr(0, offset);
	m_index.trim();
	m_varable = input.substr(offset+1);
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
			m_varable = m_varable.substr(0, offset);
		}
		m_varable.trim();
	}
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
	return m_varable < rhs.m_varable;
}

bool EnumMapGen::EnumMapTypes::operator == (const EnumMapTypes& rhs) const
{
	return m_index == rhs.m_index && m_varable == rhs.m_varable;
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
	std::sort(m_types.begin(), m_types.end());
	std::vector<EnumMapTypes>::iterator it_cutoff = std::unique(m_types.begin(), m_types.end());
	m_types.resize(std::distance(m_types.begin(), it_cutoff));

	// m_types now contains all EnumMap types mentioned in the source code without dublicates
}
