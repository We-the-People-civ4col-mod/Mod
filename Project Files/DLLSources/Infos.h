#pragma once

class XMLReader;

// string, which can only be written by XMLReader, hence write once
// low memory usage and automatic memory allocation/clearing
class XML_VAR_String
{
	friend class XMLReader;
public:
	XML_VAR_String();
	~XML_VAR_String();

	CvString get() const;
	CvWString getWide() const;
	const char* getPointer() const;

	operator const char*() const;
	operator CvString() const;
	operator CvWString() const;

private:
	void clear();
	const char* m_string;
};

class InfoBase
#ifdef COMPILE_STATIC_TEST
	: private boost::noncopyable
#endif
{
public:
	const char* getType() const;

	bool readType(XMLReader& reader);
	bool read(XMLReader& reader);
	bool postLoadSetup(XMLReader& reader);
};

class InfoBaseTypeOnly : public InfoBase
#ifdef COMPILE_STATIC_TEST
	: private boost::noncopyable
#endif
{
public:
	const char* getType() const;

	bool readType(XMLReader& reader);

protected:
	XML_VAR_String m_szType;
};

class InfoBaseTypeDesc
#ifdef COMPILE_STATIC_TEST
	: private boost::noncopyable
#endif
{
public:
	const char* getType() const;
	CvWString getTextKeyWide() const;
	CvWString getDescription(uint uiForm = 0) const;

	bool readType(XMLReader& reader);
	bool read(XMLReader& reader);
	bool postLoadSetup(XMLReader& reader);

protected:
	XML_VAR_String m_szType;
	XML_VAR_String m_szTextKey;
};

class InfoBasePedia
#ifdef COMPILE_STATIC_TEST
	: private boost::noncopyable
#endif
{
public:
	InfoBasePedia();
	~InfoBasePedia();

	bool readType(XMLReader& reader);
	bool read(XMLReader& reader);
	bool postLoadSetup(XMLReader& reader);

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

class DomainInfo : public InfoBaseTypeDesc {};
class UnitAIInfo : public InfoBaseTypeOnly {};


class CivCategoryInfo : public InfoBase
{
public:
	CivCategoryInfo();

	const char* getType() const;
	CivEffectTypes getCivEffect() const;

	bool readType(XMLReader& reader);
	bool read(XMLReader& reader);

protected:
	XML_VAR_String m_szType;
	CivEffectTypes m_eCivEffect;
};

class CivilizationInfo : public InfoBase
{
public:
	CivilizationInfo();

	bool readType(XMLReader& reader);
	bool read(XMLReader& reader);
private:
	XML_VAR_String m_szType;
	bool m_bPlayable;
	int m_iAreaMultiplier;
	InfoArray<UnitClassTypes, UnitTypes> m_Units;
	InfoArray<LeaderHeadTypes> m_Leaders;


	
	
};
