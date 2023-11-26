#pragma once

namespace tinyxml2
{
	class XMLElement;
}

class CiXMLTypeContainer;


class CiXMLReader
{
public:
	CiXMLReader(const CiXMLTypeContainer& FileReader, const tinyxml2::XMLElement* Element);

	void nextSiblingSameName();
	CiXMLReader openFolder(const char* name) const;

	bool valid() const;

	void Read(const char* szTag, CvString& szText) const;
	void ReadTextKey(const char* szTag, CvString& szText) const;
	
	template<typename T>
	void Read(const char* szTag, T& type) const;

private:
	const char* _ReadString(const char* szTag) const;
	const CiXMLTypeContainer& m_FileReader;
	const tinyxml2::XMLElement* m_Element;
};


template<typename T>
void CiXMLReader::Read(const char* szTag, T& type) const
{
	const char* text = _ReadString(szTag);
	if (text == NULL)
	{
		return;
	}
	getIndexOfType(type, text);
}
