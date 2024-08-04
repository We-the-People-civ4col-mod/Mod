#pragma once

#include "XMLReader.h"
#include "lib/tinyxml2/tinyxml2.h"

class XMLFileReader;

class XMLTypeContainer
{
public:
	XMLTypeContainer(const XMLFileReader& Reader);

	XMLReader getListElement();
	void next();

	const char* getType() const;
	const char* getCurrentFile() const;

	bool valid() const;

	bool isType(const tinyxml2::XMLElement* pElement, const char* szType) const;

private:
	void setType();

	const XMLFileReader& m_Reader;
	const tinyxml2::XMLElement* m_pElement;
	CvString m_szType;
};


class XMLFileReader
{
	friend class XMLTypeContainer;
public:
	template<typename T>
	XMLFileReader(T var) 
		: m_pFile(NULL)
		, m_pSchema(NULL)
		, m_pRoot(NULL)
		, m_pSchemaRoot(NULL)
		, m_EnumType(getEnumName(var))
		, m_pFileInfo(NULL)
		, m_file(NULL)
	{ openFile(); }

	void validate(CvXMLLoadUtility* pUtility) const;

	int getNumTypes() const;
	const char* getCurrentFile() const;

	// warning: CiXMLTypeContainer and any CiXMLReader it spawns can't be used after clearCache is called
	XMLTypeContainer getFirstListElement() const;

	// remove the cached files to free up memory
	static void clearCache();

private:
	void openFile();
	const tinyxml2::XMLElement* getFirstElement() const;

	const tinyxml2::XMLDocument* m_pFile;
	const tinyxml2::XMLDocument* m_pSchema;

	const tinyxml2::XMLElement* m_pRoot;
	const tinyxml2::XMLElement* m_pSchemaRoot;
	const char* m_xmlns;
	const char* m_EnumType;
	const tinyxml2::XMLElement* m_pFileInfo;
	const char* m_file;
};
