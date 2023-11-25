#pragma once

#include "tinyxml2.h"

class CiXMLTypeContainer;


class CiXMLReader
{
public:
	CiXMLReader(const CiXMLTypeContainer& FileReader, const tinyxml2::XMLElement* Element);

	void nextSiblingSameName();
	CiXMLReader openFolder(const char* name) const;

	bool valid() const;

	void ReadText(const char* szTag, const char*& szText) const;
	void ReadTextKey(const char* szTag, const wchar*& szText) const;
	void ReadTextKey(const char* szTag, CvWString& szText) const;
	
	template<typename T>
	void Read(const char* szTag, T& type) const;

private:
	const CiXMLTypeContainer& m_FileReader;
	const tinyxml2::XMLElement* m_Element;
};


template<typename T>
void CiXMLReader::Read(const char* szTag, T& type) const
{
	const tinyxml2::XMLElement* element = m_Element->FirstChildElement(szTag);
	if (element == NULL)
	{
		return;
	}
	getIndexOfType(type, element->GetText());
}
