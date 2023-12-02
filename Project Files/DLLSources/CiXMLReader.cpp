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

void CiXMLReader::Read(const char* szTag, CvString& szText) const
{
	const tinyxml2::XMLElement* element = m_Element->FirstChildElement(szTag);
	if (element == NULL)
	{
		return;
	}
	szText = element->GetText();
}

void CiXMLReader::ReadTextKey(const char* szTag, CvString& szText) const
{
	const tinyxml2::XMLElement* element = m_Element->FirstChildElement(szTag);
	if (element == NULL)
	{
		return;
	}
	szText = element->GetText();
	FAssert(CvWString(szText) != gDLL->getText(szText));
}

void CiXMLReader::Read(const char* szTag, bool& bBool) const
{
	const tinyxml2::XMLElement* element = m_Element->FirstChildElement(szTag);
	if (element != NULL)
	{
		bBool = element->BoolText();
	}
}

void CiXMLReader::Read(const char* szTag, int& iValue) const
{
	const tinyxml2::XMLElement* element = m_Element->FirstChildElement(szTag);
	if (element != NULL)
	{
		iValue = element->IntText();
	}
}

const char* CiXMLReader::_ReadString(const char* szTag) const
{
	const tinyxml2::XMLElement* element = m_Element->FirstChildElement(szTag);
	return element == NULL ? NULL : element->GetText();
}
