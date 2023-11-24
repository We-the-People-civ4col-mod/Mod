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

private:
	void setType();

	const CiXMLFileReader& m_Reader;
	const tinyxml2::XMLElement* m_pElement;
	CvString m_szType;
};


class CiXMLFileNameHolderBase
{
public:
	virtual const char* getFileName() const = 0;
	virtual const char* getType() const = 0;
};

template<typename T>
class CiXMLFileNameHolder : public CiXMLFileNameHolderBase
{
public:
	const char* getFileName() const;
	const char* getType() const;
};

class CiXMLFileReader
{
	friend class CiXMLTypeContainer;
public:
	template<typename T>
	CiXMLFileReader(T var) { m_FileNameHolder = new CiXMLFileNameHolder<T>(); }
	~CiXMLFileReader();

	void openFile();

	void validate(CvXMLLoadUtility* pUtility) const;

	int getNumTypes() const;

	// warning: CiXMLReader can't be used after the parent CiXMLFileReader goes out of scope
	//CiXMLReader getFirstListElement() const;
	CiXMLTypeContainer getFirstListElement() const;

private:
	const tinyxml2::XMLElement* getFirstElement() const;

	tinyxml2::XMLError openFile(tinyxml2::XMLDocument& doc, const char *path);

	tinyxml2::XMLDocument m_File;
	tinyxml2::XMLDocument m_Schema;

	tinyxml2::XMLElement* m_pRoot;
	tinyxml2::XMLElement* m_pSchemaRoot;
	const char* m_xmlns;

	CiXMLFileNameHolderBase* m_FileNameHolder;
};

template<typename T>
const char* CiXMLFileNameHolder<T>::getType() const
{
	return "Type";
}
