#include "CvGameCoreDLL.h"
#include "CiInfos.h"
#include "CiXMLReader.h"

bool CiInfoBase::readType(CiXMLReader& reader)
{
	return false;
}

bool CiInfoBase::read(CiXMLReader& reader)
{
	return false;
}

bool CiInfoBase::postLoadSetup(CiXMLReader& reader)
{
	return false;
}

const char* CiInfoBaseTypeDesc::getType() const
{
	return m_szType;
}

CvWString CiInfoBaseTypeDesc::getTextKeyWide() const
{
	return m_szTextKey;
}

CvWString CiInfoBaseTypeDesc::getDescription(uint uiForm) const
{
	if (m_szTextKey.IsEmpty())
	{
		return L"";
	}

	return gDLL->getObjectText(m_szTextKey, uiForm);
}

bool CiInfoBaseTypeDesc::readType(CiXMLReader& reader)
{
	reader.Read("Type", m_szType);
	return true;
}

bool CiInfoBaseTypeDesc::read(CiXMLReader& reader)
{
	reader.ReadTextKey("Description", m_szTextKey);
	return true;
}

bool CiInfoBaseTypeDesc::postLoadSetup(CiXMLReader& reader)
{
	return false;
}



CiInfoBasePedia::CiInfoBasePedia()
	: m_szType(NULL)
	, m_szTextKey(NULL)
	, m_szCivilopedia(NULL)
	, m_szHelp(NULL)
	, m_szSttrategy(NULL)
{
}

CiInfoBasePedia::~CiInfoBasePedia()
{
	SAFE_DELETE(m_szType);
	SAFE_DELETE(m_szTextKey);
	SAFE_DELETE(m_szCivilopedia);
	SAFE_DELETE(m_szHelp);
	SAFE_DELETE(m_szSttrategy);
}

bool CiInfoBasePedia::readType(CiXMLReader& reader)
{
	return true;
}

bool CiInfoBasePedia::read(CiXMLReader& reader)
{
	return false;
}

bool CiInfoBasePedia::postLoadSetup(CiXMLReader& reader)
{
	return false;
}

const char* CiInfoBasePedia::getType() const
{
	return m_szType;
}

const wchar* CiInfoBasePedia::getTextKeyWide() const
{
	return m_szTextKey;
}

CvWString CiInfoBasePedia::getDescription(uint uiForm) const
{
	if (m_szTextKey == NULL)
	{
		return L"";
	}

	return gDLL->getObjectText(m_szTextKey, uiForm);
}

CvWString CiInfoBasePedia::getCivilopedia() const
{
	if (m_szCivilopedia == NULL)
	{
		return L"";
	}
	return gDLL->getText(m_szCivilopedia);
}

CvWString CiInfoBasePedia::getHelp() const
{
	if (m_szHelp == NULL)
	{
		return L"";
	}
	return gDLL->getText(m_szHelp);
}

CvWString CiInfoBasePedia::getStrategy() const
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

const char* CiCivCategoryInfo::getType() const
{
	return m_szType;
}

CivEffectTypes CiCivCategoryInfo::getCivEffect() const
{
	return m_eCivEffect;
};

bool CiCivCategoryInfo::readType(CiXMLReader& reader)
{
	reader.Read("Type", m_szType);
	return true;
}

bool CiCivCategoryInfo::read(CiXMLReader& reader)
{
	reader.Read("eCivEffect", m_eCivEffect);
	return true;
}

CiCivilizationInfo::CiCivilizationInfo()
	: m_bPlayable(false)
	, m_iAreaMultiplier(0)
{
}

bool CiCivilizationInfo::readType(CiXMLReader& reader)
{
	reader.Read("Type", m_szType);
	return true;
}

bool CiCivilizationInfo::read(CiXMLReader& reader)
{
	reader.Read("bPlayable", m_bPlayable);
	reader.Read("iAreaMultiplier", m_iAreaMultiplier);

	return true;
}
