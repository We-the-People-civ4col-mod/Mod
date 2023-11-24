#pragma once

class CiXMLTypeContainer;

namespace tinyxml2
{
	class XMLElement;
}

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

private:
	const CiXMLTypeContainer& m_FileReader;
	const tinyxml2::XMLElement* m_Element;
};
