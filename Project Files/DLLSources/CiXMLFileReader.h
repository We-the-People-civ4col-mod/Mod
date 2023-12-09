#pragma once

#include "CiXMLReader.h"
#include "tinyxml2.h"

class CiXMLFileReader;

class CiXMLTypeContainer
{
public:
	CiXMLTypeContainer(const CiXMLFileReader& Reader);

	CiXMLReader getListElement();
	void next();

	const char* getType() const;

	bool valid() const;

	bool isType(const tinyxml2::XMLElement* pElement, const char* szType) const;

private:
	void setType();

	const CiXMLFileReader& m_Reader;
	const tinyxml2::XMLElement* m_pElement;
	CvString m_szType;
};


class CiXMLFileReader
{
	friend class CiXMLTypeContainer;
public:
	template<typename T>
	CiXMLFileReader(T var) 
		: m_pFile(NULL)
		, m_pSchema(NULL)
		, m_pRoot(NULL)
		, m_pSchemaRoot(NULL)
		, m_EnumType(getEnumName(var))
		, m_pFileInfo(NULL)
	{ openFile(); }

	void validate(CvXMLLoadUtility* pUtility) const;

	int getNumTypes() const;

	// warning: CiXMLTypeContainer and any CiXMLReader it spawns can't be used after clearCache is called
	CiXMLTypeContainer getFirstListElement() const;

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
};
