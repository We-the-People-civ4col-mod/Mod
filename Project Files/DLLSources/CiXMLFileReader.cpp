#include "CvGameCoreDLL.h"
#include "CiXMLFileReader.h"
#include "CvXMLLoadUtility.h"

CiXMLTypeContainer::CiXMLTypeContainer(const CiXMLFileReader& Reader)
	: m_Reader(Reader)
	, m_pElement(Reader.getFirstElement())
{
	setType();
}


CiXMLReader CiXMLTypeContainer::getListElement()
{
	setType();
	return CiXMLReader(*this, m_pElement);
}

void CiXMLTypeContainer::next()
{
	FAssert(m_pElement != NULL);
	if (m_pElement != NULL)
	{
		m_pElement = m_pElement->NextSiblingElement(m_pElement->Name());
	}
}

const char* CiXMLTypeContainer::getType() const
{
	return m_szType.c_str();
}

bool CiXMLTypeContainer::valid() const
{
	return m_pElement != NULL;
}

void CiXMLTypeContainer::setType()
{
	if (m_pElement == NULL)
	{
		FAssert(m_pElement != NULL);
		m_szType.clear();
		return;
	}

	const tinyxml2::XMLElement* type = m_pElement->FirstChildElement(m_Reader.m_FileNameHolder->getType());
	if (type != NULL)
	{
		m_szType = type->GetText();
	}
}








static std::string getPath(std::string path)
{
	int iIndex = path.find_last_of("/");
	if (iIndex != -1)
	{
		path = path.substr(0, iIndex + 1);
	}
	else
	{
		path.clear();
	}
	return path;
}

CiXMLFileReader::~CiXMLFileReader()
{
	SAFE_DELETE(m_FileNameHolder);
}


void CiXMLFileReader::openFile()
{
	const char *path = m_FileNameHolder->getFileName();
	tinyxml2::XMLError error = openFile(m_File, path);
	FAssertMsg(error == tinyxml2::XML_SUCCESS, CvString::format("XML error: couldn't read %s", path).c_str());

	for (m_pRoot = m_File.FirstChildElement(); m_pRoot != NULL; m_pRoot = m_pRoot->NextSiblingElement())
	{
		m_xmlns = m_pRoot->Attribute("xmlns");
		if (m_xmlns != NULL)
		{
			break;
		}
	}

	std::string schemaStr = getPath(path);
	schemaStr.append(m_xmlns + 9);
	error = openFile(m_Schema, schemaStr.c_str());
	FAssertMsg(error == tinyxml2::XML_SUCCESS, CvString::format("XML error: couldn't read schema %s", schemaStr.c_str()).c_str());
	m_pSchemaRoot = m_Schema.FirstChildElement("Schema");

	// now we play a little dirty. Rename all ElementType to whatever is in the name attribute
	// this way m_SchemapRoot->FirstChildElement(name) can be used to get the schema for a specific name and avoid looping in our code to locate an attribute
	for (tinyxml2::XMLElement* element = m_pSchemaRoot->FirstChildElement("ElementType"); element != NULL; element = element->NextSiblingElement("ElementType"))
	{
		element->SetName(element->Attribute("name"));
	}
	
}

void CiXMLFileReader::validate(CvXMLLoadUtility* pUtility) const
{
	std::string path = "XML/";
	path.append(m_FileNameHolder->getFileName());
	bool bLoaded = pUtility->LoadCivXml(pUtility->GetXML(), path.c_str());

	if (!bLoaded)
	{
		char szMessage[1024];
		sprintf(szMessage, "LoadXML call failed for %s.", m_FileNameHolder->getFileName());
		gDLL->MessageBox(szMessage, "XML Load Error");
		return;
	}
}

tinyxml2::XMLError CiXMLFileReader::openFile(tinyxml2::XMLDocument& doc, const char *path)
{
	std::string fullPath = GetDLLPath(true);
	fullPath.append("/XML/");
	fullPath.append(path);

	tinyxml2::XMLError error = doc.LoadFile(fullPath.c_str());
	if (error != tinyxml2::XML_SUCCESS)
	{
		fullPath = "Assets/XML/";
		fullPath.append(path);
		error = doc.LoadFile(fullPath.c_str());
	}
	return error;
}

int CiXMLFileReader::getNumTypes() const
{
	int iCount = 0;
	for (const tinyxml2::XMLElement* element = getFirstElement(); element != NULL; element = element->NextSiblingElement(element->Name()))
	{
		++iCount;
	}
	return iCount;
}

CiXMLTypeContainer CiXMLFileReader::getFirstListElement() const
{
	return CiXMLTypeContainer(*this);
}


const tinyxml2::XMLElement* CiXMLFileReader::getFirstElement() const
{
	const tinyxml2::XMLElement* element = m_pRoot;

	while (element != NULL)
	{
		const tinyxml2::XMLElement* schema = m_pSchemaRoot->FirstChildElement(element->Name());
		schema = schema->FirstChildElement("element");
		element = element->FirstChildElement(schema->Attribute("type"));
		if (schema->Attribute("maxOccurs"))
		{
			return element;
		}
	}
	return NULL;
}

const char* CiXMLFileNameHolder<DomainTypes>::getFileName() const
{
	return "BasicInfos/CIV4DomainInfos.xml";
}
