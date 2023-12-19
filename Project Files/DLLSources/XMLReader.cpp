#include "CvGameCoreDLL.h"
#include <boost/type_traits/is_enum.hpp>
#include "XMLReader.h"
#include "XMLFileReader.h"
#include "tinyxml2.h"
#include "XMLReaderAlerts.h"

XMLReader::XMLReader(const XMLTypeContainer& FileReader, const tinyxml2::XMLElement* Element)
	: m_FileReader(FileReader)
	, m_Element(Element)
{
}

void XMLReader::nextSiblingSameName()
{
	m_Element = m_Element->NextSiblingElement(m_Element->Name());
}

XMLReader XMLReader::openFolder(const char* name) const
{
	return XMLReader(m_FileReader, m_Element->FirstChildElement(name));
}

bool XMLReader::valid() const
{
	return m_Element != NULL;
}

bool XMLReader::isType(const char* szType) const
{
	return m_FileReader.isType(m_Element, szType);
}

void XMLReader::Read(const char* szTag, CvString& szText) const
{
	const tinyxml2::XMLElement* element = m_Element->FirstChildElement(szTag);
	if (element == NULL)
	{
		return;
	}
	szText = element->GetText();
}

void XMLReader::ReadTextKey(const char* szTag, CvString& szText) const
{
	const tinyxml2::XMLElement* element = m_Element->FirstChildElement(szTag);
	if (element == NULL)
	{
		return;
	}
	szText = element->GetText();
	FAssert(CvWString(szText) != gDLL->getText(szText));
}

void XMLReader::Read(const char* szTag, bool& bBool) const
{
	const tinyxml2::XMLElement* element = m_Element->FirstChildElement(szTag);
	if (element != NULL)
	{
		bBool = element->BoolText();
	}
}

void XMLReader::Read(const char* szTag, int& iValue) const
{
	const tinyxml2::XMLElement* element = m_Element->FirstChildElement(szTag);
	if (element != NULL)
	{
		iValue = element->IntText();
	}
}

template<>
void XMLReader::readElement<int>(const char* szTag, int& var, const tinyxml2::XMLElement* pElement, const tinyxml2::XMLElement* pParent, bool bAllowNone) const
{
	var = pElement->IntText();
}

template<typename T>
void XMLReader::readElement(const char* szTag, T& var, const tinyxml2::XMLElement* pElement, const tinyxml2::XMLElement* pParent, bool bAllowNone) const
{
	BOOST_STATIC_ASSERT(boost::is_enum<T>::value);

	if (pElement == NULL)
	{
		var = (T)-1;
		if (!bAllowNone)
		{
			// error: element missing when NONE isn't allowed
			XMLAlert::MissingEnumType(m_FileReader.getCurrentFile(), m_FileReader.getType(), szTag);
		}
	}
	else
	{
		const char* text = pElement->GetText();
		getIndexOfType(var, text);
		if (var == (T)-1)
		{
			if (strcmp(text, "NONE") != 0)
			{
				// error: string not of requested type
				XMLAlert::WrongEnumType(m_FileReader.getCurrentFile(), m_FileReader.getType(), szTag, pElement->Name(), text, getEnumName((T)0));
			}
			else if (!bAllowNone)
			{
				// error: NONE type not allowed
				XMLAlert::NoneEnumType(m_FileReader.getCurrentFile(), m_FileReader.getType(), szTag, pElement->Name());
			}
		}
	}
}

const char* XMLReader::_ReadString(const char* szTag) const
{
	const tinyxml2::XMLElement* element = m_Element->FirstChildElement(szTag);
	return element == NULL ? NULL : element->GetText();
}

const tinyxml2::XMLElement* XMLReader::childElement(const char* szTag) const
{
	return m_Element->FirstChildElement(szTag);
}


template<typename T0>
void XMLReader::ReadInfoArray(const char* szTag, InfoArray1Only<T0>& infoArray)
{
	std::vector<short> vec;

	const bool bFlat = isType(getEnumName((T0)0));
	for (const tinyxml2::XMLElement* loopElement = m_Element->FirstChildElement(); loopElement != NULL; loopElement = loopElement->NextSiblingElement(loopElement->Name()))
	{
		const tinyxml2::XMLElement* child = loopElement;
		T0 eType;
		if (bFlat)
		{
			readElement(szTag, eType, child, loopElement, false);
		}
		else
		{
			readElement(szTag, eType, child->FirstChildElement(), child, false);
		}
		vec.push_back(eType);
	}
	infoArray.assignFromVector(vec);
}

template<typename T0, typename T1>
void XMLReader::ReadInfoArray(const char* szTag, InfoArray2Only<T0, T1>& infoArray)
{
	std::vector<short> vec;

	for (const tinyxml2::XMLElement* loopElement = m_Element->FirstChildElement(); loopElement != NULL; loopElement = loopElement->NextSiblingElement(loopElement->Name()))
	{
		const tinyxml2::XMLElement* child = loopElement->FirstChildElement();
		T0 eType0;
		readElement(szTag, eType0, child, loopElement, false);
		child = child->NextSiblingElement();
		T1 eType1;
		readElement(szTag, eType1, child, loopElement, true);
		vec.push_back(eType0);
		vec.push_back(eType1);
	}
	infoArray.assignFromVector(vec);
}

template<typename T0, typename T1, typename T2>
void XMLReader::ReadInfoArray(const char* szTag, InfoArray3Only<T0, T1, T2>& infoArray)
{
	std::vector<short> vec;

	for (const tinyxml2::XMLElement* loopElement = m_Element->FirstChildElement(); loopElement != NULL; loopElement = loopElement->NextSiblingElement(loopElement->Name()))
	{
		const tinyxml2::XMLElement* child = loopElement->FirstChildElement();
		T0 eType0;
		readElement(szTag, eType0, child, loopElement, false);
		child = child->NextSiblingElement();
		T1 eType1;
		readElement(szTag, eType1, child, loopElement, false);
		child = child->NextSiblingElement();
		T2 eType2;
		readElement(szTag, eType2, child, loopElement, true);
		vec.push_back(eType0);
		vec.push_back(eType1);
		vec.push_back(eType2);
	}
	infoArray.assignFromVector(vec);
}

template<typename T0, typename T1, typename T2, typename T3>
void XMLReader::ReadInfoArray(const char* szTag, InfoArray4Only<T0, T1, T2, T3>& infoArray)
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

template <typename T>
typename boost::enable_if<boost::is_enum<T>, void>::type
declareSpecialization(T var)
{
	FAssert(0);
	XMLReader* reader = NULL;
	reader->Read("", var);
}

// template declarations/specialization
#include "autogenerated\AutoInfoArrayReader.h"

void declarations2()
{
	// never call this function. It will fail/crash
	FAssert(0);
	XMLReader* reader = NULL;
	declareSpecialization((CivEffectTypes)0);
}
