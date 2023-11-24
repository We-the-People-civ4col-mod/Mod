#include "CvGameCoreDLL.h"
#include "CiXMLReader.h"
#include "tinyxml2.h"

CiXMLReader::CiXMLReader(const CiXMLTypeContainer& FileReader, const tinyxml2::XMLElement* Element)
	: m_FileReader(FileReader)
	, m_Element(Element)
{
}

void CiXMLReader::nextSiblingSameName()
{
	m_Element = m_Element->NextSiblingElement(m_Element->Name());
}

CiXMLReader CiXMLReader::openFolder(const char* name) const
{
	return CiXMLReader(m_FileReader, m_Element->FirstChildElement(name));
}

bool CiXMLReader::valid() const
{
	return m_Element != NULL;
}

void CiXMLReader::ReadText(const char* szTag, const char*& szText) const
{
	const tinyxml2::XMLElement* element = m_Element->FirstChildElement(szTag);
	if (element == NULL)
	{
		return;
	}
	CvString buffer = element->GetText();

	int iLength = buffer.length();
	if (iLength <= 0)
	{
		return;
	}

	char* newStr = new char[iLength + 1];
	newStr[iLength] = 0;
	memcpy(newStr, buffer.c_str(), iLength);
	szText = newStr;
}

void CiXMLReader::ReadTextKey(const char* szTag, const wchar*& szText) const
{
	const tinyxml2::XMLElement* element = m_Element->FirstChildElement(szTag);
	if (element == NULL)
	{
		return;
	}
	CvWString buffer = element->GetText();

	FAssert(buffer != gDLL->getText(buffer));

	int iLength = buffer.length();
	if (iLength <= 0)
	{
		return;
	}

	wchar* newStr = new wchar[iLength + 1];
	newStr[iLength] = 0;
	memcpy(newStr, buffer.c_str(), iLength * sizeof(wchar));
	szText = newStr;
}

void CiXMLReader::ReadTextKey(const char* szTag, CvWString& szText) const
{
	const tinyxml2::XMLElement* element = m_Element->FirstChildElement(szTag);
	if (element == NULL)
	{
		return;
	}
	szText = element->GetText();
	FAssert(szText != gDLL->getText(szText));
}
