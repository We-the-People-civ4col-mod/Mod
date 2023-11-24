#pragma once

class CiXMLReader;

class DiInfoBaseTypeDesc
#ifdef COMPILE_STATIC_TEST
	: private boost::noncopyable
#endif
{
public:
	DiInfoBaseTypeDesc();
	~DiInfoBaseTypeDesc();

	bool readType(CiXMLReader& reader);
	bool read(CiXMLReader& reader);
	bool postLoadSetup(CiXMLReader& reader);

	const char* getType() const;

	const wchar* getTextKeyWide() const;
	CvWString getDescription(uint uiForm = 0) const;

protected:
	const char* m_szType;
	const wchar* m_szTextKey;
};

class DiInfoBasePedia
#ifdef COMPILE_STATIC_TEST
	: private boost::noncopyable
#endif
{
public:
	DiInfoBasePedia();
	~DiInfoBasePedia();

	bool readType(CiXMLReader& reader);
	bool read(CiXMLReader& reader);
	bool postLoadSetup(CiXMLReader& reader);

	const char* getType() const;

	const wchar* getTextKeyWide() const;
	CvWString getDescription(uint uiForm = 0) const;
	CvWString getCivilopedia() const;
	CvWString getHelp() const;
	CvWString getStrategy() const;

	bool isMatchForLink(std::wstring szLink, bool bKeysOnly) const;

	void reset();

	// TODO button and isGraphicalOnly

protected:
	const char* m_szType;
	const wchar* m_szTextKey;
	const wchar* m_szCivilopedia;
	const wchar* m_szHelp;
	const wchar* m_szSttrategy;

	mutable std::vector<CvWString> m_aCachedDescriptions;
};

class CiDomainInfo : public DiInfoBaseTypeDesc
{
};

