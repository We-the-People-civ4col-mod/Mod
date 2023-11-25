#include "CvGameCoreDLL.h"
#include "CiXMLFileReader.h"
#include "CvXMLLoadUtility.h"

class XMLCache;

extern XMLCache* XML_CACHE_HANDLER;

struct XMLCacheContainerEntry
{
	XMLCacheContainerEntry(CvString name) 
		: m_name(name)
		, m_pDoc(new tinyxml2::XMLDocument())
	{}

	tinyxml2::XMLError openFile()
	{
		std::string fullPath = GetDLLPath(true);
		fullPath.append("/XML/");
		fullPath.append(m_name);

		tinyxml2::XMLError error = m_pDoc->LoadFile(fullPath.c_str());
		if (error != tinyxml2::XML_SUCCESS)
		{
			fullPath = "Assets/XML/";
			fullPath.append(m_name);
			error = m_pDoc->LoadFile(fullPath.c_str());
			FAssertMsg(error == tinyxml2::XML_SUCCESS, CvString::format("Failed to open file: %s", m_name.c_str()));
		}
		return error;
	}

	CvString m_name;
	tinyxml2::XMLDocument* m_pDoc; // warning: do not free this pointer. Doing so will mess up when the vector size increase
};

class XMLCacheContainer
{
public:
	~XMLCacheContainer()
	{
		// free m_pDoc as XMLCacheContainerEntry is not able to do it itself
		// this is (partly?) due to missing std::move
		for (unsigned int i = 0; i < m_vector.size(); ++i)
		{
			SAFE_DELETE(m_vector[i].m_pDoc);
		}
	}

	bool getFile(CvString filename, tinyxml2::XMLDocument*& doc)
	{
		for (unsigned i = 0; i < m_vector.size(); ++i)
		{
			if (filename == m_vector[i].m_name)
			{
				doc = m_vector[i].m_pDoc;
				return true;
			}
		}
		m_vector.push_back(XMLCacheContainerEntry(filename));
		doc = m_vector[m_vector.size()-1].m_pDoc;
		m_vector[m_vector.size() - 1].openFile();
		return false;
	}
protected:
	std::vector<XMLCacheContainerEntry> m_vector;
};



class XMLCache
{
public:
	const tinyxml2::XMLDocument* getFile(CvString filename)
	{
		tinyxml2::XMLDocument* pFile;
		m_files.getFile(filename, pFile);
		return pFile;
	}
	const tinyxml2::XMLDocument* getSchema(CvString filename)
	{
		tinyxml2::XMLDocument* pFile;
		bool bHasBeenSetUp = m_schema.getFile(filename, pFile);
		if (!bHasBeenSetUp)
		{
			tinyxml2::XMLElement* m_pSchemaRoot = pFile->FirstChildElement("Schema");
			for (tinyxml2::XMLElement* element = m_pSchemaRoot->FirstChildElement("ElementType"); element != NULL; element = element->NextSiblingElement("ElementType"))
			{
				element->SetName(element->Attribute("name"));
			}
		}
		return pFile;
	}


protected:
	XMLCacheContainer m_files;
	XMLCacheContainer m_schema;
};

//
// ---------------------------------
//            end of cache
// ---------------------------------
//




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
	if (XML_CACHE_HANDLER == NULL)
	{
		// start the cache if it's not already started
		XML_CACHE_HANDLER = new XMLCache;
	}
	
	const char *path = m_FileNameHolder->getFileName();
	m_pFile = XML_CACHE_HANDLER->getFile(path);
	FAssert(m_pFile != NULL);

	for (m_pRoot = m_pFile->FirstChildElement(); m_pRoot != NULL; m_pRoot = m_pRoot->NextSiblingElement())
	{
		m_xmlns = m_pRoot->Attribute("xmlns");
		if (m_xmlns != NULL)
		{
			break;
		}
	}
	FAssert(m_pRoot != NULL);
	FAssert(m_xmlns != NULL);

	std::string schemaStr = getPath(path);
	schemaStr.append(m_xmlns + 9);
	m_pSchema = XML_CACHE_HANDLER->getSchema(schemaStr);
	FAssert(m_pSchema != NULL);

	m_pSchemaRoot = m_pSchema->FirstChildElement("Schema");
	FAssert(m_pSchemaRoot != NULL);
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


void CiXMLFileReader::clearCache()
{
	SAFE_DELETE(XML_CACHE_HANDLER);
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


const char* CiXMLFileNameHolder<CivCategoryTypes>::getFileName() const
{
	return "Civilizations/CivCategoryInfos.xml";
}

const char* CiXMLFileNameHolder<DomainTypes>::getFileName() const
{
	return "BasicInfos/CIV4DomainInfos.xml";
}
