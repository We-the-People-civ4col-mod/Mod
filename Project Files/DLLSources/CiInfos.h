#pragma once

class CiXMLReader;

class CiInfoBase
#ifdef COMPILE_STATIC_TEST
	: private boost::noncopyable
#endif
{
public:
	bool readType(CiXMLReader& reader);
	bool read(CiXMLReader& reader);
	bool postLoadSetup(CiXMLReader& reader);
};

class CiInfoBaseTypeDesc
#ifdef COMPILE_STATIC_TEST
	: private boost::noncopyable
#endif
{
public:
	const char* getType() const;
	CvWString getTextKeyWide() const;
	CvWString getDescription(uint uiForm = 0) const;

	bool readType(CiXMLReader& reader);
	bool read(CiXMLReader& reader);
	bool postLoadSetup(CiXMLReader& reader);

protected:
	CvString m_szType;
	CvString m_szTextKey;
};

class CiInfoBasePedia
#ifdef COMPILE_STATIC_TEST
	: private boost::noncopyable
#endif
{
public:
	CiInfoBasePedia();
	~CiInfoBasePedia();

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

class CiDomainInfo : public CiInfoBaseTypeDesc
{
};

class CiCivCategoryInfo : public CiInfoBase
{
public:
	CiCivCategoryInfo();

	const char* getType() const;
	CivEffectTypes getCivEffect() const;

	bool readType(CiXMLReader& reader);
	bool read(CiXMLReader& reader);

protected:
	CvString m_szType;
	CivEffectTypes m_eCivEffect;
};

class CiCivilizationInfo : public CiInfoBase
{
public:
	CiCivilizationInfo();

	bool readType(CiXMLReader& reader);
	bool read(CiXMLReader& reader);
private:
	CvString m_szType;
	bool m_bPlayable;
	int m_iAreaMultiplier;
};
