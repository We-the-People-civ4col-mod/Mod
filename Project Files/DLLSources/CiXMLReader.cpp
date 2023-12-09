#include "CvGameCoreDLL.h"
#include "CiXMLReader.h"
#include "CiXMLFileReader.h"
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

bool CiXMLReader::isType(const char* szType) const
{
	return m_FileReader.isType(m_Element, szType);
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

template<>
void CiXMLReader::readElement<int>(const char* szTag, int& var, const tinyxml2::XMLElement* pElement, bool bAllowNone) const
{
	var = pElement->IntText();
}

template<typename T>
void CiXMLReader::readElement(const char* szTag, T& var, const tinyxml2::XMLElement* pElement, bool bAllowNone) const
{
	getIndexOfType(var, pElement->GetText());
}

const char* CiXMLReader::_ReadString(const char* szTag) const
{
	const tinyxml2::XMLElement* element = m_Element->FirstChildElement(szTag);
	return element == NULL ? NULL : element->GetText();
}


template<typename T0>
void CiXMLReader::ReadInfoArray(const char* szTag, InfoArray1Only<T0>& infoArray)
{
	std::vector<short> vec;

	const bool bFlat = isType(getEnumName((T0)0));
	for (const tinyxml2::XMLElement* loopElement = m_Element->FirstChildElement(); loopElement != NULL; loopElement = loopElement->NextSiblingElement(loopElement->Name()))
	{
		const tinyxml2::XMLElement* child = loopElement;
		T0 eType;
		if (!bFlat)
		{
			child = child->FirstChildElement();
		}
		readElement(szTag, eType, child, false);
		vec.push_back(eType);
	}
	infoArray.assignFromVector(vec);
}

template<typename T0, typename T1>
void CiXMLReader::ReadInfoArray(const char* szTag, InfoArray2Only<T0, T1>& infoArray)
{
	std::vector<short> vec;

	for (const tinyxml2::XMLElement* loopElement = m_Element->FirstChildElement(); loopElement != NULL; loopElement = loopElement->NextSiblingElement(loopElement->Name()))
	{
		const tinyxml2::XMLElement* child = loopElement->FirstChildElement();
		T0 eType0;
		readElement(szTag, eType0, child, false);
		child = child->NextSiblingElement();
		T1 eType1;
		readElement(szTag, eType1, child, true);
		vec.push_back(eType0);
		vec.push_back(eType1);
	}
	infoArray.assignFromVector(vec);
}

template<typename T0, typename T1, typename T2>
void CiXMLReader::ReadInfoArray(const char* szTag, InfoArray3Only<T0, T1, T2>& infoArray)
{
	std::vector<short> vec;

	for (const tinyxml2::XMLElement* loopElement = m_Element->FirstChildElement(); loopElement != NULL; loopElement = loopElement->NextSiblingElement(loopElement->Name()))
	{
		const tinyxml2::XMLElement* child = loopElement->FirstChildElement();
		T0 eType0;
		readElement(szTag, eType0, child, false);
		child = child->NextSiblingElement();
		T1 eType1;
		readElement(szTag, eType1, child, false);
		child = child->NextSiblingElement();
		T2 eType2;
		readElement(szTag, eType2, child, true);
		vec.push_back(eType0);
		vec.push_back(eType1);
		vec.push_back(eType2);
	}
	infoArray.assignFromVector(vec);
}

template<typename T0, typename T1, typename T2, typename T3>
void CiXMLReader::ReadInfoArray(const char* szTag, InfoArray4Only<T0, T1, T2, T3>& infoArray)
{
	std::vector<short> vec;

	for (const tinyxml2::XMLElement* loopElement = m_Element->FirstChildElement(); loopElement != NULL; loopElement = loopElement->NextSiblingElement(loopElement->Name()))
	{
		const tinyxml2::XMLElement* child = loopElement->FirstChildElement();
		T0 eType0;
		readElement(szTag, eType0, child, false);
		child = child->NextSiblingElement();
		T1 eType1;
		readElement(szTag, eType1, child, false);
		child = child->NextSiblingElement();
		T2 eType2;
		readElement(szTag, eType2, child, false);
		child = child->NextSiblingElement();
		T3 eType3;
		readElement(szTag, eType3, child, true);
		vec.push_back(eType0);
		vec.push_back(eType1);
		vec.push_back(eType2);
		vec.push_back(eType3);
	}
	infoArray.assignFromVector(vec);
}

// template declarations/specialization
#include "autogenerated\AutoInfoArrayReader.h"
