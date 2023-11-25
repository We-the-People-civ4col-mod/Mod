#include "CvGameCoreDLL.h"
#include "CiInfos.h"
#include "CiXMLReader.h"


DiInfoBaseTypeDesc::DiInfoBaseTypeDesc()
	: m_szType(NULL)
	, m_szTextKey(NULL)
{
}

DiInfoBaseTypeDesc::~DiInfoBaseTypeDesc()
{
	SAFE_DELETE(m_szType);
	SAFE_DELETE(m_szTextKey);
}

bool DiInfoBaseTypeDesc::readType(CiXMLReader& reader)
{
	reader.ReadText("Type", m_szType);
	return true;
}

bool DiInfoBaseTypeDesc::read(CiXMLReader& reader)
{
	reader.ReadTextKey("Description", m_szTextKey);
	return true;
}

bool DiInfoBaseTypeDesc::postLoadSetup(CiXMLReader& reader)
{
	return false;
}

const char* DiInfoBaseTypeDesc::getType() const
{
	return m_szType;
}

const wchar* DiInfoBaseTypeDesc::getTextKeyWide() const
{
	return m_szTextKey;
}

CvWString DiInfoBaseTypeDesc::getDescription(uint uiForm) const
{
	if (m_szTextKey == NULL)
	{
		return L"";
	}

	return gDLL->getObjectText(m_szTextKey, uiForm);
}



DiInfoBasePedia::DiInfoBasePedia()
	: m_szType(NULL)
	, m_szTextKey(NULL)
	, m_szCivilopedia(NULL)
	, m_szHelp(NULL)
	, m_szSttrategy(NULL)
{
}

DiInfoBasePedia::~DiInfoBasePedia()
{
	SAFE_DELETE(m_szType);
	SAFE_DELETE(m_szTextKey);
	SAFE_DELETE(m_szCivilopedia);
	SAFE_DELETE(m_szHelp);
	SAFE_DELETE(m_szSttrategy);
}

bool DiInfoBasePedia::readType(CiXMLReader& reader)
{
	return true;
}

bool DiInfoBasePedia::read(CiXMLReader& reader)
{
	return false;
}

bool DiInfoBasePedia::postLoadSetup(CiXMLReader& reader)
{
	return false;
}

const char* DiInfoBasePedia::getType() const
{
	return m_szType;
}

const wchar* DiInfoBasePedia::getTextKeyWide() const
{
	return m_szTextKey;
}

CvWString DiInfoBasePedia::getDescription(uint uiForm) const
{
	if (m_szTextKey == NULL)
	{
		return L"";
	}

	return gDLL->getObjectText(m_szTextKey, uiForm);
}

CvWString DiInfoBasePedia::getCivilopedia() const
{
	if (m_szCivilopedia == NULL)
	{
		return L"";
	}
	return gDLL->getText(m_szCivilopedia);
}

CvWString DiInfoBasePedia::getHelp() const
{
	if (m_szHelp == NULL)
	{
		return L"";
	}
	return gDLL->getText(m_szHelp);
}

CvWString DiInfoBasePedia::getStrategy() const
{
	if (m_szSttrategy == NULL)
	{
		return L"";
	}
	return gDLL->getText(m_szSttrategy);
}


CiCivCategoryInfo::CiCivCategoryInfo()
	: m_eCivEffect(NO_CIV_EFFECT)
{}

bool CiCivCategoryInfo::read(CiXMLReader& reader)
{
	reader.Read("eCivEffect", m_eCivEffect);
	return true;
}

CivEffectTypes CiCivCategoryInfo::getCivEffect() const
{
	return m_eCivEffect;
};
