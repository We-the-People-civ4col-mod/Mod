#include "CvGameCoreDLL.h"
#include "Infos.h"
#include "XMLReader.h"


XML_VAR_String::XML_VAR_String()
	: m_string(NULL)
{}

XML_VAR_String::~XML_VAR_String()
{
	SAFE_DELETE(m_string);
}

CvString XML_VAR_String::get() const
{
	return m_string;
}

CvWString XML_VAR_String::getWide() const
{
	return CvWString(m_string);
}

const char* XML_VAR_String::getPointer() const
{
	return m_string;
}

XML_VAR_String::operator const char*() const
{
	return m_string;
}

XML_VAR_String::operator CvString() const
{
	return m_string;
}

XML_VAR_String::operator CvWString() const
{
	return CvWString(m_string);
}


bool InfoBase::readType(XMLReader& reader)
{
	return false;
}

bool InfoBase::read(XMLReader& reader)
{
	return false;
}

bool InfoBase::postLoadSetup(XMLReader& reader)
{
	return false;
}

const char* InfoBaseTypeOnly::getType() const
{
	return m_szType.getPointer();
}

bool InfoBaseTypeOnly::readType(XMLReader& reader)
{
	reader.Read("Type", m_szType);
	return true;
}

const char* InfoBaseTypeDesc::getType() const
{
	return m_szType;
}

CvWString InfoBaseTypeDesc::getTextKeyWide() const
{
	return m_szTextKey;
}

CvWString InfoBaseTypeDesc::getDescription(uint uiForm) const
{
	if (m_szTextKey.getPointer() == NULL)
	{
		return L"";
	}

	return gDLL->getObjectText(m_szTextKey.getWide(), uiForm);
}

bool InfoBaseTypeDesc::readType(XMLReader& reader)
{
	reader.Read("Type", m_szType);
	return true;
}

bool InfoBaseTypeDesc::read(XMLReader& reader)
{
	reader.ReadTextKey("Description", m_szTextKey);
	return true;
}

bool InfoBaseTypeDesc::postLoadSetup(XMLReader& reader)
{
	return false;
}



InfoBasePedia::InfoBasePedia()
	: m_szType(NULL)
	, m_szTextKey(NULL)
	, m_szCivilopedia(NULL)
	, m_szHelp(NULL)
	, m_szSttrategy(NULL)
{
}

InfoBasePedia::~InfoBasePedia()
{
	SAFE_DELETE(m_szType);
	SAFE_DELETE(m_szTextKey);
	SAFE_DELETE(m_szCivilopedia);
	SAFE_DELETE(m_szHelp);
	SAFE_DELETE(m_szSttrategy);
}

bool InfoBasePedia::readType(XMLReader& reader)
{
	return true;
}

bool InfoBasePedia::read(XMLReader& reader)
{
	return false;
}

bool InfoBasePedia::postLoadSetup(XMLReader& reader)
{
	return false;
}

const char* InfoBasePedia::getType() const
{
	return m_szType;
}

const wchar* InfoBasePedia::getTextKeyWide() const
{
	return m_szTextKey;
}

CvWString InfoBasePedia::getDescription(uint uiForm) const
{
	if (m_szTextKey == NULL)
	{
		return L"";
	}

	return gDLL->getObjectText(m_szTextKey, uiForm);
}

CvWString InfoBasePedia::getCivilopedia() const
{
	if (m_szCivilopedia == NULL)
	{
		return L"";
	}
	return gDLL->getText(m_szCivilopedia);
}

CvWString InfoBasePedia::getHelp() const
{
	if (m_szHelp == NULL)
	{
		return L"";
	}
	return gDLL->getText(m_szHelp);
}

CvWString InfoBasePedia::getStrategy() const
{
	if (m_szSttrategy == NULL)
	{
		return L"";
	}
	return gDLL->getText(m_szSttrategy);
}


CivCategoryInfo::CivCategoryInfo()
	: m_eCivEffect(NO_CIV_EFFECT)
{}

const char* CivCategoryInfo::getType() const
{
	return m_szType;
}

CivEffectTypes CivCategoryInfo::getCivEffect() const
{
	return m_eCivEffect;
};

bool CivCategoryInfo::readType(XMLReader& reader)
{
	reader.Read("Type", m_szType);
	return true;
}

bool CivCategoryInfo::read(XMLReader& reader)
{
	reader.Read("eCivEffect", m_eCivEffect);
	return true;
}

CivilizationInfo::CivilizationInfo()
	: m_bPlayable(false)
	, m_iAreaMultiplier(0)
{
}

bool CivilizationInfo::readType(XMLReader& reader)
{
	reader.Read("Type", m_szType);
	return true;
}

bool CivilizationInfo::read(XMLReader& reader)
{
	reader.Read("bPlayable", m_bPlayable);
	reader.Read("iAreaMultiplier", m_iAreaMultiplier);
	reader.Read("Units", m_Units);
	reader.Read("Leaders", m_Leaders);

	return true;
}
