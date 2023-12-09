#pragma once

namespace tinyxml2
{
	class XMLElement;
}

class CiXMLTypeContainer;
class InfoArrayBase;

class CiXMLReader
{
public:
	CiXMLReader(const CiXMLTypeContainer& FileReader, const tinyxml2::XMLElement* Element);

	void nextSiblingSameName();
	CiXMLReader openFolder(const char* name) const;

	bool valid() const;
	bool isType(const char* szType) const;


	void Read(const char* szTag, CvString& szText) const;
	void ReadTextKey(const char* szTag, CvString& szText) const;
	
	void Read(const char* szTag, bool& bBool) const;
	void Read(const char* szTag, int& iValue) const;

	template<typename T>
	void Read(const char* szTag, T& type) const;

	template<typename T0, typename T1, typename T2, typename T3>
	void Read(const char* szTag, InfoArray<T0, T1, T2, T3>& infoArray);

private:
	template<typename T0>
	void ReadInfoArray(const char* szTag, InfoArray1Only<T0>& infoArray);
	template<typename T0, typename T1>
	void ReadInfoArray(const char* szTag, InfoArray2Only<T0, T1>& infoArray);
	template<typename T0, typename T1, typename T2>
	void ReadInfoArray(const char* szTag, InfoArray3Only<T0, T1, T2>& infoArray);
	template<typename T0, typename T1, typename T2, typename T3>
	void ReadInfoArray(const char* szTag, InfoArray4Only<T0, T1, T2, T3>& infoArray);

	template<typename T>
	void readElement(const char* szTag, T& var, const tinyxml2::XMLElement* pElement, bool bAllowNone) const;

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


template<typename T0, typename T1, typename T2, typename T3>
void CiXMLReader::Read(const char* szTag, InfoArray<T0, T1, T2, T3>& infoArray)
{
	CiXMLReader child = openFolder(szTag);
	if (!child.valid())
	{
		return;
	}

	child.ReadInfoArray(szTag, infoArray);
}
