//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvInfos.cpp
//
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2003 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include "CvInfos.h"
#include "CvGlobals.h"
#include "CvArtFileMgr.h"
#include "CvXMLLoadUtility.h"
#include "CvDLLXMLIFaceBase.h"
#include "CvGameTextMgr.h"
#include "CvGameCoreUtils.h"
#include "lib/iconv/converters.h"
#include "XMLReader.h"
#include "Infos.h"
#include "GlobalsInfoContainer.h"

// static pointer used only by CvInfoBase
// main purpose is to add read functions to CvInfoBase
// and have pXML and getType() available without passing them as arguments
CvXMLLoadUtility* CvInfoBase::m_pXML = NULL;

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CInfoBase()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvInfoBase::CvInfoBase() :
m_bGraphicalOnly(false)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CInfoBase()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvInfoBase::~CvInfoBase()
{
}

void CvInfoBase::read(FDataStreamBase* pStream)
{
	m_szCachedText.clear();
	m_szCachedCivilopedia.clear();
	m_szCachedHelp.clear();
	m_szCachedStrategy.clear();
	m_aszExtraXMLforPass3.clear();

	pStream->Read(&m_bGraphicalOnly);
	pStream->ReadString(m_szType);
	pStream->ReadString(m_szCivilopediaKey);
	pStream->ReadString(m_szHelpKey);
	pStream->ReadString(m_szStrategyKey);
	pStream->ReadString(m_szButton);
	pStream->ReadString(m_szTextKey);

	int iSize;
	pStream->Read(&iSize);
	m_aszExtraXMLforPass3.resize(iSize);
	for(int i=0;i<iSize;i++)
	{
		pStream->ReadString(m_aszExtraXMLforPass3[i]);
	}
}

void CvInfoBase::write(FDataStreamBase* pStream)
{
	pStream->Write(m_bGraphicalOnly);
	pStream->WriteString(m_szType);
	pStream->WriteString(m_szCivilopediaKey);
	pStream->WriteString(m_szHelpKey);
	pStream->WriteString(m_szStrategyKey);
	pStream->WriteString(m_szButton);
	pStream->WriteString(m_szTextKey);

	pStream->Write((int)m_aszExtraXMLforPass3.size());
	for(int i=0;i<(int)m_aszExtraXMLforPass3.size();i++)
	{
		pStream->WriteString(m_aszExtraXMLforPass3[i]);
	}
}

void CvInfoBase::reset()
{
	//clear cache
	m_aCachedDescriptions.clear();
	m_szCachedText.clear();
	m_szCachedCivilopedia.clear();
	m_szCachedHelp.clear();
	m_szCachedStrategy.clear();
}

bool CvInfoBase::isGraphicalOnly() const
{
	return m_bGraphicalOnly;
}

bool CvInfoBase::hideFromPedia() const
{
	// split this question away from isGraphicalOnly
	// default to vanilla behavior where they reply the same
	// however individual classes can override this
	//    Nightinggale
	return m_bGraphicalOnly;
}

const char* CvInfoBase::getType() const
{
	if(m_szType.empty())
	{
		return NULL;
	}

	return m_szType;
}

const char* CvInfoBase::getButton() const
{
	return m_szButton;
}

const wchar* CvInfoBase::getTextKeyWide() const
{
	return m_szTextKey;
}

const wchar* CvInfoBase::getDescription(uint uiForm) const
{
	while(m_aCachedDescriptions.size() <= uiForm)
	{
		m_aCachedDescriptions.push_back(gDLL->getObjectText(m_szTextKey, m_aCachedDescriptions.size()));
	}

	return m_aCachedDescriptions[uiForm];
}

const wchar* CvInfoBase::getText() const
{
	// used instead of getDescription for Info entries that are not objects
	// so they do not have gender/plurality/forms defined in the Translator system
	if (m_szCachedText.empty())
	{
		m_szCachedText = gDLL->getText(m_szTextKey);
	}

	return m_szCachedText;
}

const wchar* CvInfoBase::getCivilopedia() const
{
	if (m_szCachedCivilopedia.empty())
	{
		m_szCachedCivilopedia = gDLL->getText(m_szCivilopediaKey);
	}

	return m_szCachedCivilopedia;
}

const wchar*  CvInfoBase::getHelp() const
{
	if (m_szCachedHelp.empty())
	{
		m_szCachedHelp = gDLL->getText(m_szHelpKey);
	}

	if (m_szCachedHelp == L"????")
	{
		return L"";
	}

	return m_szCachedHelp;
}

const wchar* CvInfoBase::getStrategy() const
{
	if (m_szCachedStrategy.empty())
	{
		m_szCachedStrategy = gDLL->getText(m_szStrategyKey);
	}

	if (m_szCachedStrategy == L"????")
	{
		return L"";
	}

	return m_szCachedStrategy;
}

bool CvInfoBase::isMatchForLink(std::wstring szLink, bool bKeysOnly) const
{
	if (szLink == CvWString(getType()).GetCString())
	{
		return true;
	}

	if (!bKeysOnly)
	{
		uint iNumForms = gDLL->getNumForms(getTextKeyWide());
		for (uint i = 0; i < iNumForms; i++)
		{
			if (szLink == getDescription(i))
			{
				return true;
			}
		}
	}

	return false;
}

bool CvInfoBase::readType(XMLReader& reader)
{
	XML_VAR_String szBuffer;

	reader.Read("Type", szBuffer);
	m_szType = szBuffer.get();
	
	return true;
}

bool CvInfoBase::read(XMLReader& reader)
{
	XML_VAR_String szBuffer;

	reader.Read("bGraphicalOnly", m_bGraphicalOnly);

	reader.Read("Button", szBuffer);
	m_szButton = szBuffer.get();

	return true;
}

bool CvInfoBase::postLoadSetup(XMLReader& reader)
{
	return false;
}

bool CvInfoBase::loadText(XMLReader& reader)
{
	XML_VAR_String szBuffer;

	reader.ReadTextKey("Description", szBuffer, true);
	m_szTextKey = szBuffer.getWide();

	reader.ReadTextKey("Civilopedia", szBuffer, true);
	m_szCivilopediaKey = szBuffer.getWide();

	reader.ReadTextKey("Help", szBuffer, true);
	m_szHelpKey = szBuffer.getWide();

	reader.ReadTextKey("Strategy", szBuffer, true);
	m_szStrategyKey = szBuffer.getWide();

	return true;
}

//
// read from XML
// TYPE, DESC, BUTTON
//
bool CvInfoBase::read(CvXMLLoadUtility* pXML)
{
	// update static pointer
	m_pXML = pXML;

	// Skip any comments and stop at the next value we might want
	if (!pXML->SkipToNextVal())
	{
		return false;
	}

	pXML->MapChildren();	// try to hash children for fast lookup by name

	// GRAPHICAL ONLY
	pXML->GetChildXmlValByName(&m_bGraphicalOnly, "bGraphicalOnly");

	// TYPE
	pXML->GetChildXmlValByName(m_szType, "Type");

	// DESCRIPTION
	pXML->GetChildXmlValByName(m_szTextKey, "Description");

	// CIVILOPEDIA
	pXML->GetChildXmlValByName(m_szCivilopediaKey, "Civilopedia");

	// HELP
	pXML->GetChildXmlValByName(m_szHelpKey, "Help");

	// STRATEGY
	pXML->GetChildXmlValByName(m_szStrategyKey, "Strategy");

	// BUTTON
	pXML->GetChildXmlValByName(m_szButton, "Button");

	return true;
}

void CvInfoBase::cleanStrings()
{
	checkStringContents(m_szTextKey       , L"_DESCRIPTION");
	checkStringContents(m_szCivilopediaKey, L"_PEDIA"      );
	checkStringContents(m_szHelpKey       , L"_HELP"       );
	checkStringContents(m_szStrategyKey   , L"_STRATEGY"   );
}

void CvInfoBase::checkStringContents(CvWString& szStr, const wchar* szExtension)
{
	if (szStr.length() == 0)
	{
		return;
	}

	// plenty of arguments to get around stuff like %2_city crashing the exe during this test
	CvWString szText = gDLL->getText(szStr, L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"");

	if (szText == L"????" || (szText.length() > 7 && wcsncmp(szText, L"TXT_KEY", 7) == 0))
	{
		szStr.clear();
		return;
	}
}

bool CvInfoBase::postLoadSetup()
{
	// returns if all instances will need to be run postLoadSetup
	// obvoiusly if this is overwritten somewhere, then it must return true to make it work
	return false;
}

//
// XML reading code
//

template<typename T0, typename T1, typename T2, typename T3>
void CvInfoBase::readXML(InfoArray<T0, T1, T2, T3>& kInfo, const char* szTag)
{
	kInfo.read(m_pXML, getType(), szTag);
}

//======================================================================================================
//					CvScalableInfo
//======================================================================================================
bool CvScalableInfo::read(CvXMLLoadUtility* pXML)
{
	float fScale;
	pXML->GetChildXmlValByName(&fScale, "fScale");
	setScale(fScale);
	pXML->GetChildXmlValByName(&fScale, "fInterfaceScale", 1.0f);
	setInterfaceScale(fScale);
	return true;
}

float CvScalableInfo::getScale() const
{
	return m_fScale;
}

void CvScalableInfo::setScale(float fScale)
{
	m_fScale = fScale;
}

float CvScalableInfo::getInterfaceScale() const
{
	return m_fInterfaceScale;
}

void CvScalableInfo::setInterfaceScale(float fInterfaceScale)
{
	m_fInterfaceScale = fInterfaceScale;
}


//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvHotkeyInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvHotkeyInfo::CvHotkeyInfo() :
m_iHotKeyPriority(-1),
m_iHotKeyPriorityAlt(-1),
m_iOrderPriority(0),
m_bAltDown(0),
m_bShiftDown(0),
m_bCtrlDown(0),
m_bAltDownAlt(0),
m_bShiftDownAlt(0),
m_bCtrlDownAlt(0)
{
}

bool CvHotkeyInfo::read(XMLReader& reader)
{
	CvInfoBase::read(reader);

	XML_VAR_String szBuffer;
	KeyboardKeyTypes eKey;
	int iVal;
	bool bVal;

	reader.Read("HotKey", szBuffer);
	eKey.assignFromString(szBuffer);
	m_eHotKeyVal = eKey.value();
	FAssert(m_eHotKeyVal == eKey.value());

	reader.Read("iHotKeyPriority", iVal, -1);
	m_iHotKeyPriority = iVal;
	FAssert(m_iHotKeyPriority == iVal);

	reader.Read("HotKeyAlt", szBuffer);
	eKey.assignFromString(szBuffer);
	m_eHotKeyValAlt = eKey.value();
	FAssert(m_eHotKeyValAlt == eKey.value());

	reader.Read("iHotKeyPriorityAlt", iVal, -1);
	m_iHotKeyPriorityAlt = iVal;
	FAssert(m_iHotKeyPriorityAlt == iVal);

	reader.Read("bAltDown", bVal);
	m_bAltDown = bVal ? 1 : 0;

	reader.Read("bShiftDown", bVal);
	m_bShiftDown = bVal ? 1 : 0;

	reader.Read("bCtrlDown", bVal);
	m_bCtrlDown = bVal ? 1 : 0;

	reader.Read("bAltDownAlt", bVal);
	m_bAltDownAlt = bVal ? 1 : 0;

	reader.Read("bShiftDownAlt", bVal);
	m_bShiftDownAlt = bVal ? 1 : 0;

	reader.Read("bCtrlDownAlt", bVal);
	m_bCtrlDownAlt = bVal ? 1 : 0;

	reader.Read("iOrderPriority", iVal, 5); // note: vanilla has 5 as default for unknown reasons
	m_iOrderPriority = iVal;
	FAssert(m_iOrderPriority == iVal);

	return true;
}

bool CvHotkeyInfo::read(CvXMLLoadUtility* pXML)
{
	int iVal;
	bool bVal;
	KeyboardKeyTypes eKey;
	CvString szTextVal;

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(szTextVal, "HotKey");
	eKey.assignFromString(szTextVal);
	m_eHotKeyVal = eKey.value();
	FAssert(m_eHotKeyVal == eKey.value());

	pXML->GetChildXmlValByName(&iVal, "iHotKeyPriority", -1);
	m_iHotKeyPriority = iVal;
	FAssert(m_iHotKeyPriority == iVal);

	pXML->GetChildXmlValByName(szTextVal, "HotKeyAlt");
	eKey.assignFromString(szTextVal);
	m_eHotKeyValAlt = eKey.value();
	FAssert(m_eHotKeyValAlt == eKey.value());

	pXML->GetChildXmlValByName(&iVal, "iHotKeyPriorityAlt", -1);
	m_iHotKeyPriorityAlt = iVal;
	FAssert(m_iHotKeyPriorityAlt == iVal);

	pXML->GetChildXmlValByName(&bVal, "bAltDown");
	m_bAltDown = bVal ? 1 : 0;

	pXML->GetChildXmlValByName(&bVal, "bShiftDown");
	m_bShiftDown = bVal ? 1 : 0;

	pXML->GetChildXmlValByName(&bVal, "bCtrlDown");
	m_bCtrlDown = bVal ? 1 : 0;

	pXML->GetChildXmlValByName(&bVal, "bAltDownAlt");
	m_bAltDownAlt = bVal ? 1 : 0;

	pXML->GetChildXmlValByName(&bVal, "bShiftDownAlt");
	m_bShiftDownAlt = bVal ? 1 : 0;

	pXML->GetChildXmlValByName(&bVal, "bCtrlDownAlt");
	m_bCtrlDownAlt = bVal ? 1 : 0;

	pXML->GetChildXmlValByName(&iVal, "iOrderPriority", 5); // note: vanilla has 5 as default for unknown reasons
	m_iOrderPriority = iVal;
	FAssert(m_iOrderPriority == iVal);

	return true;
}

void CvHotkeyInfo::read(FDataStreamBase* pStream)
{
	CvInfoBase::read(pStream);

	uint uiFlag=0;
	pStream->Read(&uiFlag);	// flags for expansion

//	pStream->Read(&m_iHotKeyVal);
//	pStream->Read(&m_iHotKeyPriority);
//	pStream->Read(&m_iHotKeyValAlt);
//	pStream->Read(&m_iHotKeyPriorityAlt);
//	pStream->Read(&m_iOrderPriority);
//	pStream->Read(&m_bAltDown);
//	pStream->Read(&m_bShiftDown);
//	pStream->Read(&m_bCtrlDown);
//	pStream->Read(&m_bAltDownAlt);
//	pStream->Read(&m_bShiftDownAlt);
//	pStream->Read(&m_bCtrlDownAlt);
//	pStream->ReadString(m_szHotKey);
//	pStream->ReadString(m_szHotKeyDescriptionKey);
//	pStream->ReadString(m_szHotKeyAltDescriptionKey);
//	pStream->ReadString(m_szHotKeyString);
}

void CvHotkeyInfo::write(FDataStreamBase* pStream)
{
	CvInfoBase::write(pStream);

	uint uiFlag = 0;
	pStream->Write(uiFlag);		// flag for expansion

	//pStream->Write(m_iHotKeyVal);
	pStream->Write(m_iHotKeyPriority);
//	pStream->Write(m_iHotKeyValAlt);
	pStream->Write(m_iHotKeyPriorityAlt);
	pStream->Write(m_iOrderPriority);
	pStream->Write(m_bAltDown);
	pStream->Write(m_bShiftDown);
	pStream->Write(m_bCtrlDown);
	pStream->Write(m_bAltDownAlt);
	pStream->Write(m_bShiftDownAlt);
	pStream->Write(m_bCtrlDownAlt);
//	pStream->WriteString(m_szHotKey);
//	pStream->WriteString(m_szHotKeyDescriptionKey);
//	pStream->WriteString(m_szHotKeyAltDescriptionKey);
//	pStream->WriteString(m_szHotKeyString);
}

ActionTypes CvHotkeyInfo::getActionInfoIndex() const
{
	return m_eActionInfoIndex;
}

void CvHotkeyInfo::setActionInfoIndex(ActionTypes eIndex)
{
	m_eActionInfoIndex = eIndex.value();
	FAssert(m_eActionInfoIndex == eIndex.value());
}

int CvHotkeyInfo::getHotKeyPriority() const
{
	return m_iHotKeyPriority;
}

KeyboardKeyTypes CvHotkeyInfo::getHotKeyValAlt() const
{
	return m_eHotKeyValAlt;
}

int CvHotkeyInfo::getHotKeyPriorityAlt() const
{
	return m_iHotKeyPriorityAlt;
}

int CvHotkeyInfo::getOrderPriority() const
{
	return m_iOrderPriority;
}

bool CvHotkeyInfo::isAltDown() const
{
	return m_bAltDown != 0;
}

bool CvHotkeyInfo::isShiftDown() const
{
	return m_bShiftDown != 0;
}

bool CvHotkeyInfo::isCtrlDown() const
{
	return m_bCtrlDown != 0;
}

bool CvHotkeyInfo::isAltDownAlt() const
{
	return m_bAltDownAlt != 0;
}

bool CvHotkeyInfo::isShiftDownAlt() const
{
	return m_bShiftDownAlt != 0;
}

bool CvHotkeyInfo::isCtrlDownAlt() const
{
	return m_bCtrlDownAlt != 0;
}

KeyboardKeyTypes CvHotkeyInfo::getHotKey() const
{
	return m_eHotKeyVal;
}

//======================================================================================================
//					CvDiplomacyResponse
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvDiplomacyResponse()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvDiplomacyResponse::CvDiplomacyResponse() :
m_iNumDiplomacyText(0),
m_abCivilizationTypes(NULL),
m_abLeaderHeadTypes(NULL),
m_abAttitudeTypes(NULL),
m_abDiplomacyPowerTypes(NULL),
m_paszDiplomacyText(NULL)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvDiplomacyResponse()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvDiplomacyResponse::~CvDiplomacyResponse()
{
	SAFE_DELETE_ARRAY(m_abCivilizationTypes);
	SAFE_DELETE_ARRAY(m_abLeaderHeadTypes);
	SAFE_DELETE_ARRAY(m_abAttitudeTypes);
	SAFE_DELETE_ARRAY(m_abDiplomacyPowerTypes);
	SAFE_DELETE_ARRAY(m_paszDiplomacyText);
}

int CvDiplomacyResponse::getNumDiplomacyText()
{
	return m_iNumDiplomacyText;
}

void CvDiplomacyResponse::setNumDiplomacyText(int i)
{
	m_iNumDiplomacyText = i;
}

bool CvDiplomacyResponse::getCivilizationTypes(int i)
{
	FAssertMsg(i < GC.getNumCivilizationInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abCivilizationTypes[i];
}

bool* CvDiplomacyResponse::getCivilizationTypes() const
{
	return m_abCivilizationTypes;
}

void CvDiplomacyResponse::setCivilizationTypes(int i, bool bVal)
{
	FAssertMsg(i < GC.getNumCivilizationInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	m_abCivilizationTypes[i] = bVal;
}

bool CvDiplomacyResponse::getLeaderHeadTypes(int i)
{
	FAssertMsg(i < GC.getNumLeaderHeadInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abLeaderHeadTypes[i];
}

bool* CvDiplomacyResponse::getLeaderHeadTypes() const
{
	return m_abLeaderHeadTypes;
}

void CvDiplomacyResponse::setLeaderHeadTypes(int i, bool bVal)
{
	FAssertMsg(i < GC.getNumLeaderHeadInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	m_abLeaderHeadTypes[i] = bVal;
}

bool CvDiplomacyResponse::getAttitudeTypes(int i) const
{
	FAssertMsg(i < NUM_ATTITUDE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abAttitudeTypes[i];
}

bool* CvDiplomacyResponse::getAttitudeTypes() const
{
	return m_abAttitudeTypes;
}

void CvDiplomacyResponse::setAttitudeTypes(int i, bool bVal)
{
	FAssertMsg(i < NUM_ATTITUDE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	m_abAttitudeTypes[i] = bVal;
}

bool CvDiplomacyResponse::getDiplomacyPowerTypes(int i)
{
	FAssertMsg(i < NUM_DIPLOMACYPOWER_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abDiplomacyPowerTypes[i];
}

bool* CvDiplomacyResponse::getDiplomacyPowerTypes() const
{
	return m_abDiplomacyPowerTypes;
}

void CvDiplomacyResponse::setDiplomacyPowerTypes(int i, bool bVal)
{
	FAssertMsg(i < NUM_DIPLOMACYPOWER_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	m_abDiplomacyPowerTypes[i] = bVal;
}

const char* CvDiplomacyResponse::getDiplomacyText(int i) const
{
	return m_paszDiplomacyText[i];
}

const CvString* CvDiplomacyResponse::getDiplomacyText() const
{
	return m_paszDiplomacyText;
}

void CvDiplomacyResponse::setDiplomacyText(int i, CvString szText)
{
	FAssertMsg(i < getNumDiplomacyText(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	m_paszDiplomacyText[i] = szText;
}

void CvDiplomacyResponse::read(FDataStreamBase* stream)
{
	uint uiFlag=0;
	stream->Read(&uiFlag);		// flag for expansion

	stream->Read(&m_iNumDiplomacyText);

	SAFE_DELETE_ARRAY(m_abCivilizationTypes);
	m_abCivilizationTypes = new bool[GC.getNumCivilizationInfos()];
	stream->Read(GC.getNumCivilizationInfos(), m_abCivilizationTypes);

	SAFE_DELETE_ARRAY(m_abLeaderHeadTypes);
	m_abLeaderHeadTypes = new bool[GC.getNumLeaderHeadInfos()];
	stream->Read(GC.getNumLeaderHeadInfos(), m_abLeaderHeadTypes);

	SAFE_DELETE_ARRAY(m_abAttitudeTypes);
	m_abAttitudeTypes = new bool[NUM_ATTITUDE_TYPES];
	stream->Read(NUM_ATTITUDE_TYPES, m_abAttitudeTypes);

	SAFE_DELETE_ARRAY(m_abDiplomacyPowerTypes);
	m_abDiplomacyPowerTypes = new bool[NUM_DIPLOMACYPOWER_TYPES];
	stream->Read(NUM_DIPLOMACYPOWER_TYPES, m_abDiplomacyPowerTypes);

	SAFE_DELETE_ARRAY(m_paszDiplomacyText);
	m_paszDiplomacyText = new CvString[m_iNumDiplomacyText];
	stream->ReadString(m_iNumDiplomacyText, m_paszDiplomacyText);
}

void CvDiplomacyResponse::write(FDataStreamBase* stream)
{
	uint uiFlag=0;
	stream->Write(uiFlag);		// flag for expansion

	stream->Write(m_iNumDiplomacyText);

	stream->Write(GC.getNumCivilizationInfos(), m_abCivilizationTypes);
	stream->Write(GC.getNumLeaderHeadInfos(), m_abLeaderHeadTypes);
	stream->Write(NUM_ATTITUDE_TYPES, m_abAttitudeTypes);
	stream->Write(NUM_DIPLOMACYPOWER_TYPES, m_abDiplomacyPowerTypes);
	stream->WriteString(m_iNumDiplomacyText, m_paszDiplomacyText);
}

bool CvDiplomacyResponse::read(CvXMLLoadUtility* pXML)
{
	pXML->SetVariableListTagPair(&m_abCivilizationTypes, "Civilizations", GC.getNumCivilizationInfos(), false);
	// Leaders
	pXML->SetVariableListTagPair(&m_abLeaderHeadTypes, "Leaders", GC.getNumLeaderHeadInfos(), false);
	// AttitudeTypes
	pXML->SetVariableListTagPair(&m_abAttitudeTypes, "Attitudes", NUM_ATTITUDE_TYPES, false);
	// PowerTypes
	pXML->SetVariableListTagPair(&m_abDiplomacyPowerTypes, "DiplomacyPowers", NUM_DIPLOMACYPOWER_TYPES, false);
	// DiplomacyText
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"DiplomacyText"))
	{
		pXML->SetStringList(&m_paszDiplomacyText, &m_iNumDiplomacyText);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}

	return true;
}

//======================================================================================================
//					CvPromotionInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvPromotionInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvPromotionInfo::CvPromotionInfo() :
m_eIndex(NO_PROMOTION),
m_iPrereqPromotion(NO_PROMOTION),
m_iPrereqOrPromotion1(NO_PROMOTION),
m_iPrereqOrPromotion2(NO_PROMOTION),
m_iVisibilityChange(0),
m_iMovesChange(0),
m_iMoveDiscountChange(0),
m_iWithdrawalChange(0),
m_iCargoChange(0),
m_iBombardRateChange(0),
m_iEnemyHealChange(0),
m_iNeutralHealChange(0),
m_iFriendlyHealChange(0),
m_iSameTileHealChange(0),
m_iAdjacentTileHealChange(0),
m_iCombatPercent(0),
m_iCityAttackPercent(0),
m_iCityDefensePercent(0),
m_iHillsAttackPercent(0),
m_iHillsDefensePercent(0),
m_iDomesticBonusPercent(0),
m_iCommandType(NO_COMMAND),
m_iPillageChange(0),
m_iAnimalGoldChange(0), //WTP, ray, Animal Promotions increase gold from Animals
m_iSlaveRevoltReductionBonus(0), //WTP, ray, Slave Hunter and Slave Master
m_iSlaveWorkerProductionBonus(0), //WTP, ray, Slave Hunter and Slave Master
m_iAdditionalLawToCity(0), // WTP, ray, Lawkeeper Promotion - START
m_iUpgradeDiscount(0),
m_iExperiencePercent(0),
m_bHideFromPedia(false),
m_bNotEarnedByXP(false), //WTP, ray, Promotions not earned by XP
m_bLeader(false),
m_bNegativePromotion(false), //WTP, ray Negative Promotions - START
m_bBlitz(false),
m_bAmphib(false),
m_bRiver(false),
m_bEnemyRoute(false),
m_bAlwaysHeal(false),
m_bHillsDoubleMove(false),
m_bAvailableForDefensiveUnit(true),       // WTP, johanb - cache for defensive unit availabilty - by default everything is allowed
m_aiTerrainAttackPercent(NULL),
m_aiTerrainDefensePercent(NULL),
m_aiFeatureAttackPercent(NULL),
m_aiFeatureDefensePercent(NULL),
m_aiUnitClassAttackModifier(NULL),
m_aiUnitClassDefenseModifier(NULL),
m_aiUnitCombatModifierPercent(NULL),
m_aiDomainModifierPercent(NULL),
m_abTerrainDoubleMove(NULL),
m_abFeatureDoubleMove(NULL),
m_abUnitCombat(NULL)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvPromotionInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvPromotionInfo::~CvPromotionInfo()
{
	SAFE_DELETE_ARRAY(m_aiTerrainAttackPercent);
	SAFE_DELETE_ARRAY(m_aiTerrainDefensePercent);
	SAFE_DELETE_ARRAY(m_aiFeatureAttackPercent);
	SAFE_DELETE_ARRAY(m_aiFeatureDefensePercent);
	SAFE_DELETE_ARRAY(m_aiUnitClassAttackModifier);
	SAFE_DELETE_ARRAY(m_aiUnitClassDefenseModifier);
	SAFE_DELETE_ARRAY(m_aiUnitCombatModifierPercent);
	SAFE_DELETE_ARRAY(m_aiDomainModifierPercent);
	SAFE_DELETE_ARRAY(m_abTerrainDoubleMove);
	SAFE_DELETE_ARRAY(m_abFeatureDoubleMove);
	SAFE_DELETE_ARRAY(m_abUnitCombat);
}
bool CvPromotionInfo::hideFromPedia() const
{
	return m_bHideFromPedia || isGraphicalOnly();
}

//WTP, ray, Promotions not earned by XP - START
bool CvPromotionInfo::isNotEarnedByXP() const
{
	return m_bNotEarnedByXP;
}
//WTP, ray, Promotions not earned by XP - END

int CvPromotionInfo::getPrereqPromotion() const
{
	return m_iPrereqPromotion;
}
void CvPromotionInfo::setPrereqPromotion(int i)
{
	m_iPrereqPromotion = i;
}
int CvPromotionInfo::getPrereqOrPromotion1() const
{
	return m_iPrereqOrPromotion1;
}
void CvPromotionInfo::setPrereqOrPromotion1(int i)
{
	m_iPrereqOrPromotion1 = i;
}
int CvPromotionInfo::getPrereqOrPromotion2() const
{
	return m_iPrereqOrPromotion2;
}
void CvPromotionInfo::setPrereqOrPromotion2(int i)
{
	m_iPrereqOrPromotion2 = i;
}
int CvPromotionInfo::getVisibilityChange() const
{
	return m_iVisibilityChange;
}
int CvPromotionInfo::getMovesChange() const
{
	return m_iMovesChange;
}
int CvPromotionInfo::getMoveDiscountChange() const
{
	return m_iMoveDiscountChange;
}
int CvPromotionInfo::getWithdrawalChange() const
{
	return m_iWithdrawalChange;
}
int CvPromotionInfo::getCargoChange() const
{
	return m_iCargoChange;
}
int CvPromotionInfo::getBombardRateChange() const
{
	return m_iBombardRateChange;
}
int CvPromotionInfo::getEnemyHealChange() const
{
	return m_iEnemyHealChange;
}
int CvPromotionInfo::getNeutralHealChange() const
{
	return m_iNeutralHealChange;
}
int CvPromotionInfo::getFriendlyHealChange() const
{
	return m_iFriendlyHealChange;
}
int CvPromotionInfo::getSameTileHealChange() const
{
	return m_iSameTileHealChange;
}
int CvPromotionInfo::getAdjacentTileHealChange() const
{
	return m_iAdjacentTileHealChange;
}
int CvPromotionInfo::getCombatPercent() const
{
	return m_iCombatPercent;
}
int CvPromotionInfo::getCityAttackPercent() const
{
	return m_iCityAttackPercent;
}
int CvPromotionInfo::getCityDefensePercent() const
{
	return m_iCityDefensePercent;
}
int CvPromotionInfo::getHillsAttackPercent() const
{
	return m_iHillsAttackPercent;
}
int CvPromotionInfo::getHillsDefensePercent() const
{
	return m_iHillsDefensePercent;
}
int CvPromotionInfo::getDomesticBonusPercent() const
{
	return m_iDomesticBonusPercent;
}
int CvPromotionInfo::getCommandType() const
{
	return m_iCommandType;
}
void CvPromotionInfo::setCommandType(int iNewType)
{
	m_iCommandType = iNewType;
}
int CvPromotionInfo::getPillageChange() const
{
	return m_iPillageChange;
}
//WTP, ray, Animal Promotions increase gold from Animals - START
int CvPromotionInfo::getAnimalGoldChange() const
{
	return m_iAnimalGoldChange;
}
//WTP, ray, Animal Promotions increase gold from Animals - END

//WTP, ray, Slave Hunter and Slave Master - START
int CvPromotionInfo::getSlaveRevoltReductionBonus() const
{
	return m_iSlaveRevoltReductionBonus;
}

int CvPromotionInfo::getSlaveWorkerProductionBonus() const
{
	return m_iSlaveWorkerProductionBonus;
}
//WTP, ray, Slave Hunter and Slave Master - END

// WTP, ray, Lawkeeper Promotion - START
int CvPromotionInfo::getAdditionalLawToCity() const
{
	return m_iAdditionalLawToCity;
}
// WTP, ray, Lawkeeper Promotion - END


int CvPromotionInfo::getUpgradeDiscount() const
{
	return m_iUpgradeDiscount;
}
int CvPromotionInfo::getExperiencePercent() const
{
	return m_iExperiencePercent;
}
bool CvPromotionInfo::isLeader() const
{
	return m_bLeader;
}
//WTP, ray Negative Promotions - START
bool CvPromotionInfo::isNegativePromotion() const
{
	return m_bNegativePromotion;
}
//WTP, ray Negative Promotions - END
bool CvPromotionInfo::isBlitz() const
{
	return m_bBlitz;
}
bool CvPromotionInfo::isAmphib() const
{
	return m_bAmphib;
}
bool CvPromotionInfo::isRiver() const
{
	return m_bRiver;
}
bool CvPromotionInfo::isEnemyRoute() const
{
	return m_bEnemyRoute;
}
bool CvPromotionInfo::isAlwaysHeal() const
{
	return m_bAlwaysHeal;
}
bool CvPromotionInfo::isHillsDoubleMove() const
{
	return m_bHillsDoubleMove;
}

bool CvPromotionInfo::isAvailableForDefensiveUnit() const
{
	return m_bAvailableForDefensiveUnit;
}

bool CvPromotionInfo::isNotAvailableForDefensiveUnit() const
{
	return !m_bAvailableForDefensiveUnit;
}

void CvPromotionInfo::calculateAvailableForDefensiveUnit()
{
	if (getCityAttackPercent() != 0)
	{
		m_bAvailableForDefensiveUnit = false;   return;
	}
	if (getWithdrawalChange() != 0)
	{
		m_bAvailableForDefensiveUnit = false;   return;
	}
	if (isBlitz())
	{
		m_bAvailableForDefensiveUnit = false;   return;
	}
	if (isAmphib())
	{
		m_bAvailableForDefensiveUnit = false;   return;
	}
	if (isRiver())
	{
		m_bAvailableForDefensiveUnit = false;   return;
	}
	if (getHillsAttackPercent() != 0)
	{
		m_bAvailableForDefensiveUnit = false;   return;
	}

	for (TerrainTypes eTerrain = FIRST_TERRAIN; eTerrain < NUM_TERRAIN_TYPES; ++eTerrain)
	{
		if (getTerrainAttackPercent(eTerrain) != 0)
		{
			m_bAvailableForDefensiveUnit = false;   return;
		}
	}
	for (FeatureTypes eFeature = FIRST_FEATURE; eFeature < NUM_FEATURE_TYPES; ++eFeature)
	{
		if (getFeatureAttackPercent(eFeature) != 0)
		{
			m_bAvailableForDefensiveUnit = false;   return;
		}
	}
	for (UnitClassTypes eUnitClass = FIRST_UNITCLASS; eUnitClass < NUM_UNITCLASS_TYPES; ++eUnitClass)
	{
		if (getUnitClassAttackModifier(eUnitClass) != 0)
		{
			m_bAvailableForDefensiveUnit = false;   return;
		}
	}
	m_bAvailableForDefensiveUnit = true;
}

const char* CvPromotionInfo::getSound() const
{
	return m_szSound;
}
void CvPromotionInfo::setSound(const char* szVal)
{
	m_szSound = szVal;
}
// Arrays
int CvPromotionInfo::getTerrainAttackPercent(int i) const
{
	FAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiTerrainAttackPercent ? m_aiTerrainAttackPercent[i] : -1;
}
int CvPromotionInfo::getTerrainDefensePercent(int i) const
{
	FAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiTerrainDefensePercent ? m_aiTerrainDefensePercent[i] : -1;
}
int CvPromotionInfo::getFeatureAttackPercent(int i) const
{
	FAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiFeatureAttackPercent ? m_aiFeatureAttackPercent[i] : -1;
}
int CvPromotionInfo::getFeatureDefensePercent(int i) const
{
	FAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiFeatureDefensePercent ? m_aiFeatureDefensePercent[i] : -1;
}
int CvPromotionInfo::getUnitClassAttackModifier(int i) const
{
	FAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiUnitClassAttackModifier ? m_aiUnitClassAttackModifier[i] : -1;
}
int CvPromotionInfo::getUnitClassDefenseModifier(int i) const
{
	FAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiUnitClassDefenseModifier ? m_aiUnitClassDefenseModifier[i] : -1;
}
int CvPromotionInfo::getUnitCombatModifierPercent(int i) const
{
	FAssertMsg(i < GC.getNumUnitCombatInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiUnitCombatModifierPercent ? m_aiUnitCombatModifierPercent[i] : -1;
}
int CvPromotionInfo::getDomainModifierPercent(int i) const
{
	FAssertMsg(i < NUM_DOMAIN_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiDomainModifierPercent ? m_aiDomainModifierPercent[i] : -1;
}
bool CvPromotionInfo::getTerrainDoubleMove(int i) const
{
	FAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abTerrainDoubleMove ? m_abTerrainDoubleMove[i] : false;
}
bool CvPromotionInfo::getFeatureDoubleMove(int i) const
{
	FAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abFeatureDoubleMove ? m_abFeatureDoubleMove[i] : false;
}
bool CvPromotionInfo::getUnitCombat(int i) const
{
	FAssertMsg(i < GC.getNumUnitCombatInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abUnitCombat ? m_abUnitCombat[i] : false;
}

void CvPromotionInfo::read(FDataStreamBase* stream)
{
	CvHotkeyInfo::read(stream);

	uint uiFlag=0;
	stream->Read(&uiFlag);		// flag for expansion
	stream->Read(&m_iPrereqPromotion);
	stream->Read(&m_iPrereqOrPromotion1);
	stream->Read(&m_iPrereqOrPromotion2);

	stream->Read(&m_iVisibilityChange);
	stream->Read(&m_iMovesChange);
	stream->Read(&m_iMoveDiscountChange);
	stream->Read(&m_iWithdrawalChange);
	stream->Read(&m_iCargoChange);
	stream->Read(&m_iBombardRateChange);
	stream->Read(&m_iEnemyHealChange);
	stream->Read(&m_iNeutralHealChange);
	stream->Read(&m_iFriendlyHealChange);
	stream->Read(&m_iSameTileHealChange);
	stream->Read(&m_iAdjacentTileHealChange);
	stream->Read(&m_iCombatPercent);
	stream->Read(&m_iCityAttackPercent);
	stream->Read(&m_iCityDefensePercent);
	stream->Read(&m_iHillsAttackPercent);
	stream->Read(&m_iHillsDefensePercent);
	stream->Read(&m_iDomesticBonusPercent);
	stream->Read(&m_iCommandType);
	stream->Read(&m_iPillageChange);
	stream->Read(&m_iAnimalGoldChange); //WTP, ray, Animal Promotions increase gold from Animals
	stream->Read(&m_iSlaveRevoltReductionBonus); //WTP, ray, Slave Hunter and Slave Master
	stream->Read(&m_iSlaveWorkerProductionBonus); //WTP, ray, Slave Hunter and Slave Master
	stream->Read(&m_iAdditionalLawToCity); // WTP, ray, Lawkeeper Promotion - START
	stream->Read(&m_iUpgradeDiscount);
	stream->Read(&m_iExperiencePercent);

	stream->Read(&m_bLeader);
	stream->Read(&m_bNegativePromotion); //WTP, ray Negative Promotions - START
	stream->Read(&m_bBlitz);
	stream->Read(&m_bAmphib);
	stream->Read(&m_bRiver);
	stream->Read(&m_bEnemyRoute);
	stream->Read(&m_bAlwaysHeal);
	stream->Read(&m_bHillsDoubleMove);

	stream->ReadString(m_szSound);

	// Arrays

	SAFE_DELETE_ARRAY(m_aiTerrainAttackPercent);
	m_aiTerrainAttackPercent = new int[GC.getNumTerrainInfos()];
	stream->Read(GC.getNumTerrainInfos(), m_aiTerrainAttackPercent);
	SAFE_DELETE_ARRAY(m_aiTerrainDefensePercent);
	m_aiTerrainDefensePercent = new int[GC.getNumTerrainInfos()];
	stream->Read(GC.getNumTerrainInfos(), m_aiTerrainDefensePercent);
	SAFE_DELETE_ARRAY(m_aiFeatureAttackPercent);
	m_aiFeatureAttackPercent = new int[GC.getNumFeatureInfos()];
	stream->Read(GC.getNumFeatureInfos(), m_aiFeatureAttackPercent);
	SAFE_DELETE_ARRAY(m_aiFeatureDefensePercent);
	m_aiFeatureDefensePercent = new int[GC.getNumFeatureInfos()];
	stream->Read(GC.getNumFeatureInfos(), m_aiFeatureDefensePercent);
	SAFE_DELETE_ARRAY(m_aiUnitClassAttackModifier);
	m_aiUnitClassAttackModifier = new int[GC.getNumUnitClassInfos()];
	stream->Read(GC.getNumUnitClassInfos(), m_aiUnitClassAttackModifier);
	SAFE_DELETE_ARRAY(m_aiUnitClassDefenseModifier);
	m_aiUnitClassDefenseModifier = new int[GC.getNumUnitClassInfos()];
	stream->Read(GC.getNumUnitClassInfos(), m_aiUnitClassDefenseModifier);
	SAFE_DELETE_ARRAY(m_aiUnitCombatModifierPercent);
	m_aiUnitCombatModifierPercent = new int[GC.getNumUnitCombatInfos()];
	stream->Read(GC.getNumUnitCombatInfos(), m_aiUnitCombatModifierPercent);
	SAFE_DELETE_ARRAY(m_aiDomainModifierPercent);
	m_aiDomainModifierPercent = new int[NUM_DOMAIN_TYPES];
	stream->Read(NUM_DOMAIN_TYPES, m_aiDomainModifierPercent);
	SAFE_DELETE_ARRAY(m_abTerrainDoubleMove);
	m_abTerrainDoubleMove = new bool[GC.getNumTerrainInfos()];
	stream->Read(GC.getNumTerrainInfos(), m_abTerrainDoubleMove);
	SAFE_DELETE_ARRAY(m_abFeatureDoubleMove);
	m_abFeatureDoubleMove = new bool[GC.getNumFeatureInfos()];
	stream->Read(GC.getNumFeatureInfos(), m_abFeatureDoubleMove);
	SAFE_DELETE_ARRAY(m_abUnitCombat);
	m_abUnitCombat = new bool[GC.getNumUnitCombatInfos()];
	stream->Read(GC.getNumUnitCombatInfos(), m_abUnitCombat);
}
void CvPromotionInfo::write(FDataStreamBase* stream)
{
	CvHotkeyInfo::write(stream);
	uint uiFlag = 0;
	stream->Write(uiFlag);		// flag for expansion
	stream->Write(m_iPrereqPromotion);
	stream->Write(m_iPrereqOrPromotion1);
	stream->Write(m_iPrereqOrPromotion2);

	stream->Write(m_iVisibilityChange);
	stream->Write(m_iMovesChange);
	stream->Write(m_iMoveDiscountChange);
	stream->Write(m_iWithdrawalChange);
	stream->Write(m_iCargoChange);
	stream->Write(m_iBombardRateChange);
	stream->Write(m_iEnemyHealChange);
	stream->Write(m_iNeutralHealChange);
	stream->Write(m_iFriendlyHealChange);
	stream->Write(m_iSameTileHealChange);
	stream->Write(m_iAdjacentTileHealChange);
	stream->Write(m_iCombatPercent);
	stream->Write(m_iCityAttackPercent);
	stream->Write(m_iCityDefensePercent);
	stream->Write(m_iHillsAttackPercent);
	stream->Write(m_iHillsDefensePercent);
	stream->Write(m_iDomesticBonusPercent);
	stream->Write(m_iCommandType);
	stream->Write(m_iPillageChange);
	stream->Write(m_iAnimalGoldChange); //WTP, ray, Animal Promotions increase gold from Animals
	stream->Write(m_iSlaveRevoltReductionBonus); //WTP, ray, Slave Hunter and Slave Master
	stream->Write(m_iSlaveWorkerProductionBonus); //WTP, ray, Slave Hunter and Slave Master
	stream->Write(m_iAdditionalLawToCity); // WTP, ray, Lawkeeper Promotion - START
	stream->Write(m_iUpgradeDiscount);
	stream->Write(m_iExperiencePercent);
	stream->Write(m_bLeader);
	stream->Write(m_bNegativePromotion); //WTP, ray Negative Promotions - START
	stream->Write(m_bBlitz);
	stream->Write(m_bAmphib);
	stream->Write(m_bRiver);
	stream->Write(m_bEnemyRoute);
	stream->Write(m_bAlwaysHeal);
	stream->Write(m_bHillsDoubleMove);
	stream->WriteString(m_szSound);
	// Arrays
	stream->Write(GC.getNumTerrainInfos(), m_aiTerrainAttackPercent);
	stream->Write(GC.getNumTerrainInfos(), m_aiTerrainDefensePercent);
	stream->Write(GC.getNumFeatureInfos(), m_aiFeatureAttackPercent);
	stream->Write(GC.getNumFeatureInfos(), m_aiFeatureDefensePercent);
	stream->Write(GC.getNumUnitClassInfos(), m_aiUnitClassAttackModifier);
	stream->Write(GC.getNumUnitClassInfos(), m_aiUnitClassDefenseModifier);
	stream->Write(GC.getNumUnitCombatInfos(), m_aiUnitCombatModifierPercent);
	stream->Write(NUM_DOMAIN_TYPES, m_aiDomainModifierPercent);
	stream->Write(GC.getNumTerrainInfos(), m_abTerrainDoubleMove);
	stream->Write(GC.getNumFeatureInfos(), m_abFeatureDoubleMove);
	stream->Write(GC.getNumUnitCombatInfos(), m_abUnitCombat);
}
bool CvPromotionInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	m_eIndex = static_cast<PromotionTypes>(getIndexForType(JIT_ARRAY_PROMOTION, getType()));
	FAssert(m_eIndex != NO_PROMOTION);

	pXML->GetChildXmlValByName(szTextVal, "Sound");
	setSound(szTextVal);
	pXML->GetChildXmlValByName(&m_bHideFromPedia, "bHideFromPedia");
	pXML->GetChildXmlValByName(&m_bNotEarnedByXP, "bNotEarnedByXP"); //WTP, ray, Promotions not earned by XP
	pXML->GetChildXmlValByName(&m_bLeader, "bLeader");
	pXML->GetChildXmlValByName(&m_bNegativePromotion, "bNegativePromotion"); //WTP, ray Negative Promotions - START
	pXML->GetChildXmlValByName(&m_bBlitz, "bBlitz");
	pXML->GetChildXmlValByName(&m_bAmphib, "bAmphib");
	pXML->GetChildXmlValByName(&m_bRiver, "bRiver");
	pXML->GetChildXmlValByName(&m_bEnemyRoute, "bEnemyRoute");
	pXML->GetChildXmlValByName(&m_bAlwaysHeal, "bAlwaysHeal");
	pXML->GetChildXmlValByName(&m_bHillsDoubleMove, "bHillsDoubleMove");
	pXML->GetChildXmlValByName(&m_iVisibilityChange, "iVisibilityChange");
	pXML->GetChildXmlValByName(&m_iMovesChange, "iMovesChange");
	pXML->GetChildXmlValByName(&m_iMoveDiscountChange, "iMoveDiscountChange");
	pXML->GetChildXmlValByName(&m_iWithdrawalChange, "iWithdrawalChange");
	pXML->GetChildXmlValByName(&m_iCargoChange, "iCargoChange");
	pXML->GetChildXmlValByName(&m_iBombardRateChange, "iBombardRateChange");
	pXML->GetChildXmlValByName(&m_iEnemyHealChange, "iEnemyHealChange");
	pXML->GetChildXmlValByName(&m_iNeutralHealChange, "iNeutralHealChange");
	pXML->GetChildXmlValByName(&m_iFriendlyHealChange, "iFriendlyHealChange");
	pXML->GetChildXmlValByName(&m_iSameTileHealChange, "iSameTileHealChange");
	pXML->GetChildXmlValByName(&m_iAdjacentTileHealChange, "iAdjacentTileHealChange");
	pXML->GetChildXmlValByName(&m_iCombatPercent, "iCombatPercent");
	pXML->GetChildXmlValByName(&m_iCityAttackPercent, "iCityAttack");
	pXML->GetChildXmlValByName(&m_iCityDefensePercent, "iCityDefense");
	pXML->GetChildXmlValByName(&m_iHillsAttackPercent, "iHillsAttack");
	pXML->GetChildXmlValByName(&m_iHillsDefensePercent, "iHillsDefense");
	pXML->GetChildXmlValByName(&m_iDomesticBonusPercent, "iDomesticBonus");
	pXML->GetChildXmlValByName(&m_iPillageChange, "iPillageChange");
	pXML->GetChildXmlValByName(&m_iAnimalGoldChange, "iAnimalGoldChange"); //WTP, ray, Animal Promotions increase gold from Animals
	pXML->GetChildXmlValByName(&m_iSlaveRevoltReductionBonus, "iSlaveRevoltReductionBonus"); //WTP, ray, Slave Hunter and Slave Master
	pXML->GetChildXmlValByName(&m_iSlaveWorkerProductionBonus, "iSlaveWorkerProductionBonus"); //WTP, ray, Slave Hunter and Slave Master
	pXML->GetChildXmlValByName(&m_iAdditionalLawToCity, "iAdditionalLawToCity"); // WTP, ray, Lawkeeper Promotion - START
	pXML->GetChildXmlValByName(&m_iUpgradeDiscount, "iUpgradeDiscount");
	pXML->GetChildXmlValByName(&m_iExperiencePercent, "iExperiencePercent");
	pXML->SetVariableListTagPair(&m_aiTerrainAttackPercent, "TerrainAttacks", GC.getNumTerrainInfos(), 0);
	pXML->SetVariableListTagPair(&m_aiTerrainDefensePercent, "TerrainDefenses", GC.getNumTerrainInfos(), 0);
	pXML->SetVariableListTagPair(&m_aiFeatureAttackPercent, "FeatureAttacks", GC.getNumFeatureInfos(), 0);
	pXML->SetVariableListTagPair(&m_aiFeatureDefensePercent, "FeatureDefenses", GC.getNumFeatureInfos(), 0);
	pXML->SetVariableListTagPair(&m_aiUnitClassAttackModifier, "UnitClassAttackMods", GC.getNumUnitClassInfos(), 0);
	pXML->SetVariableListTagPair(&m_aiUnitClassDefenseModifier, "UnitClassDefenseMods", GC.getNumUnitClassInfos(), 0);
	pXML->SetVariableListTagPair(&m_aiUnitCombatModifierPercent, "UnitCombatMods", GC.getNumUnitCombatInfos(), 0);
	pXML->SetVariableListTagPair(&m_aiDomainModifierPercent, "DomainMods", NUM_DOMAIN_TYPES, 0);
	pXML->SetVariableListTagPair(&m_abTerrainDoubleMove, "TerrainDoubleMoves", GC.getNumTerrainInfos(), false);
	pXML->SetVariableListTagPair(&m_abFeatureDoubleMove, "FeatureDoubleMoves", GC.getNumFeatureInfos(), false);
	pXML->SetVariableListTagPair(&m_abUnitCombat, "UnitCombats", GC.getNumUnitCombatInfos(), false);
	return true;
}

bool CvPromotionInfo::postLoadSetup()
{
	calculateAvailableForDefensiveUnit();
	return true;
}

bool CvPromotionInfo::readPass2(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	pXML->GetChildXmlValByName(szTextVal, "PromotionPrereq");
	m_iPrereqPromotion = GC.getInfoTypeForString(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "PromotionPrereqOr1");
	m_iPrereqOrPromotion1 = GC.getInfoTypeForString(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "PromotionPrereqOr2");
	m_iPrereqOrPromotion2 = GC.getInfoTypeForString(szTextVal);
	return true;
}



//======================================================================================================
//					CvProfessionInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvProfessionInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvProfessionInfo::CvProfessionInfo() :
	m_eIndex(NO_PROFESSION),
	m_iUnitCombatType(NO_UNITCOMBAT),
	// R&R, ray , MYCP partially based on code of Aymerick - START
	// m_iYieldProduced(NO_YIELD),
	// m_iYieldConsumed(NO_YIELD),
	// R&R, ray , MYCP partially based on code of Aymerick - END
	m_iSpecialBuilding(NO_SPECIALBUILDING),
	m_iCombatChange(0),
	m_iBombardRateChangeProfession(0), // WTP, ray, Cannons to Professions - START
	m_iBarracksSpaceNeededChange(0), // WTP, ray, new Barracks System - START
	m_iMovesChange(0),
	m_iWorkRate(0),
	m_iMissionaryRate(0),
	m_iNativeTradeRate(0), // WTP, ray, Native Trade Posts - START
	m_iPowerValue(0),
	m_iAssetValue(0),
	m_bWorkPlot(false),
	m_bCitizen(false),
	m_bWater(false),
	m_bScout(false),
	m_bCanCrossLargeRivers(false), //WTP, ray, Large Rivers
	m_bCityDefender(false),
	m_bCanFound(false),
	// TAC - LbD - Ray - START
	m_LbDused(false),
	// zu pruefen
	m_expert(0),
	m_learnLevel(0),
	// TAC - LbD - Ray - END
	/// Move Into Peak - start - Nightinggale
	m_bMoveIntoPeak(false),
	/// Move Into Peak - end - Nightinggale
	m_bUnarmed(false),
	m_bNoDefensiveBonus(false),
	m_abFreePromotions(NULL),
	m_iDefaultUnitAIType(NO_UNITAI)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvProfessionInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvProfessionInfo::~CvProfessionInfo()
{
	SAFE_DELETE_ARRAY(m_abFreePromotions);
}
int CvProfessionInfo::getUnitCombatType() const
{
	return m_iUnitCombatType;
}

// TAC - LbD - Ray - START
bool CvProfessionInfo::LbD_isUsed() const
{
	return m_LbDused;
}

int CvProfessionInfo::LbD_getExpert() const
{
	return m_expert;
}

int CvProfessionInfo::LbD_getLearnLevel() const
{
	return m_learnLevel;
}
// TAC - LbD - Ray - END

// R&R, ray , MYCP partially based on code of Aymerick - START
/*int CvProfessionInfo::getYieldProduced() const
{
	return m_iYieldProduced;
}
int CvProfessionInfo::getYieldConsumed() const
{
	return m_iYieldConsumed;
}
*/
// R&R, ray , MYCP partially based on code of Aymerick - END
int CvProfessionInfo::getSpecialBuilding() const
{
	return m_iSpecialBuilding;
}
int CvProfessionInfo::getCombatChange() const
{
	return m_iCombatChange;
}

// WTP, ray, Cannons to Professions - START
int CvProfessionInfo::getBombardRateChangeProfession() const
{
	return m_iBombardRateChangeProfession;
}
// WTP, ray, Cannons to Professions - END

// WTP, ray, new Barracks System - START
int CvProfessionInfo::getBarracksSpaceNeededChange() const
{
	return m_iBarracksSpaceNeededChange;
}
// WTP, ray, new Barracks System - END

int CvProfessionInfo::getMovesChange() const
{
	return m_iMovesChange;
}
int CvProfessionInfo::getWorkRate() const
{
	return m_iWorkRate;
}
int CvProfessionInfo::getMissionaryRate() const
{
	return m_iMissionaryRate;
}
// WTP, ray, Native Trade Posts - START
int CvProfessionInfo::getNativeTradeRate() const
{
	return m_iNativeTradeRate;
}
// WTP, ray, Native Trade Posts - END
int CvProfessionInfo::getPowerValue() const
{
	return m_iPowerValue;
}
int CvProfessionInfo::getAssetValue() const
{
	return m_iAssetValue;
}
bool CvProfessionInfo::isWorkPlot() const
{
	return m_bWorkPlot;
}
bool CvProfessionInfo::isCitizen() const
{
	return m_bCitizen;
}
bool CvProfessionInfo::isWater() const
{
	return m_bWater;
}
bool CvProfessionInfo::isScout() const
{
	return m_bScout;
}

//WTP, ray, Large Rivers - START
bool CvProfessionInfo::isCanCrossLargeRivers() const
{
	return m_bCanCrossLargeRivers;
}
//WTP, ray, Large Rivers - END

bool CvProfessionInfo::isCityDefender() const
{
	return m_bCityDefender;
}
bool CvProfessionInfo::canFound() const
{
	return m_bCanFound;
}
bool CvProfessionInfo::isUnarmed() const
{
	return m_bUnarmed;
}
bool CvProfessionInfo::isNoDefensiveBonus() const
{
	return m_bNoDefensiveBonus;
}
int CvProfessionInfo::getYieldEquipmentAmount(int iYield) const
{
	for (uint i = 0; i < m_aYieldEquipments.size(); ++i)
	{
		const YieldEquipment& kYieldEquipment = m_aYieldEquipments[i];
		if (kYieldEquipment.iYieldType == iYield)
		{
			return kYieldEquipment.iYieldAmount;
		}
	}
	return 0;
}

bool CvProfessionInfo::isFreePromotion(int i) const
{
	FAssertMsg(i < GC.getNumPromotionInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abFreePromotions ? m_abFreePromotions[i] : false;
}
// R&R, ray , MYCP partially based on code of Aymerick - START
int CvProfessionInfo::getYieldsProduced(int i) const
{
	return m_aiYieldsProduced[i];
}

int CvProfessionInfo::getNumYieldsProduced() const
{
	return (int)m_aiYieldsProduced.size();
}
int CvProfessionInfo::getYieldsConsumed(int i) const
{
	return m_aiYieldsConsumed[i];
}

int CvProfessionInfo::getNumYieldsConsumed() const
{
	return (int)m_aiYieldsConsumed.size();
}
// R&R, ray , MYCP partially based on code of Aymerick - END
int CvProfessionInfo::getDefaultUnitAIType() const
{
    return m_iDefaultUnitAIType;
}

bool CvProfessionInfo::isWorkSlot() const
{
	return (m_bCitizen && !m_bWorkPlot);
}

void CvProfessionInfo::read(FDataStreamBase* stream)
{
	// R&R, ray , MYCP partially based on code of Aymerick - START
	int iNumElements;
	int iElement;
	// R&R, ray , MYCP partially based on code of Aymerick - END
	CvInfoBase::read(stream);
	uint uiFlag=0;
	stream->Read(&uiFlag);		// flag for expansion
	stream->Read(&m_iUnitCombatType);
	stream->Read(&m_iDefaultUnitAIType);
	// R&R, ray , MYCP partially based on code of Aymerick - START
	// stream->Read(&m_iYieldProduced);
	// stream->Read(&m_iYieldConsumed);
	// R&R, ray , MYCP partially based on code of Aymerick - END
	stream->Read(&m_iSpecialBuilding);
	stream->Read(&m_iCombatChange);
	stream->Read(&m_iBombardRateChangeProfession); // WTP, ray, Cannons to Professions - START
	stream->Read(&m_iBarracksSpaceNeededChange); // WTP, ray, new Barracks System - START
	stream->Read(&m_iMovesChange);
	stream->Read(&m_iWorkRate);
	stream->Read(&m_iMissionaryRate);
	stream->Read(&m_iNativeTradeRate); // WTP, ray, Native Trade Posts - START
	stream->Read(&m_iPowerValue);
	stream->Read(&m_iAssetValue);
	stream->Read(&m_bWorkPlot);
	stream->Read(&m_bCitizen);
	stream->Read(&m_bWater);
	stream->Read(&m_bScout);
	stream->Read(&m_bCanCrossLargeRivers);//WTP, ray, Large Rivers
	stream->Read(&m_bCityDefender);
	stream->Read(&m_bCanFound);

	// TAC - LbD - Ray - START
	stream->Read(&m_LbDused);
	stream->Read(&m_expert);
	stream->Read(&m_learnLevel);
	// TAC - LbD - Ray - END

	stream->Read(&m_bUnarmed);
	stream->Read(&m_bNoDefensiveBonus);
	m_aYieldEquipments.clear();
	int iYieldEquipmentSize = 0;
	stream->Read(&iYieldEquipmentSize);
	for(int i=0;i<iYieldEquipmentSize;i++)
	{
		YieldEquipment kEquipment;
		stream->Read(&kEquipment.iYieldType);
		stream->Read(&kEquipment.iYieldAmount);
		m_aYieldEquipments.push_back(kEquipment);
	}

	SAFE_DELETE_ARRAY(m_abFreePromotions);
	m_abFreePromotions = new bool[GC.getNumPromotionInfos()];
	stream->Read(GC.getNumPromotionInfos(), m_abFreePromotions);
	// R&R, ray , MYCP partially based on code of Aymerick - START
	stream->Read(&iNumElements);
	m_aiYieldsProduced.clear();
	for (int i = 0; i < iNumElements; ++i)
	{
		stream->Read(&iElement);
		m_aiYieldsProduced.push_back(iElement);
	}
	stream->Read(&iNumElements);
	m_aiYieldsConsumed.clear();
	for (int i = 0; i < iNumElements; ++i)
	{
		stream->Read(&iElement);
		m_aiYieldsConsumed.push_back(iElement);
	}
	// R&R, ray , MYCP partially based on code of Aymerick - END
}
void CvProfessionInfo::write(FDataStreamBase* stream)
{
	CvInfoBase::write(stream);
	uint uiFlag = 0;
	stream->Write(uiFlag);		// flag for expansion
	stream->Write(m_iUnitCombatType);
	stream->Write(m_iDefaultUnitAIType);
	// R&R, ray , MYCP partially based on code of Aymerick - START
	// stream->Write(m_iYieldProduced);
	// stream->Write(m_iYieldConsumed);
	// R&R, ray , MYCP partially based on code of Aymerick - END
	stream->Write(m_iSpecialBuilding);
	stream->Write(m_iCombatChange);
	stream->Write(m_iBombardRateChangeProfession); // WTP, ray, Cannons to Professions - START
	stream->Write(m_iBarracksSpaceNeededChange); // WTP, ray, new Barracks System - START
	stream->Write(m_iMovesChange);
	stream->Write(m_iWorkRate);
	stream->Write(m_iMissionaryRate);
	stream->Write(m_iNativeTradeRate); // WTP, ray, Native Trade Posts - START
	stream->Write(m_iPowerValue);
	stream->Write(m_iAssetValue);
	stream->Write(m_bWorkPlot);
	stream->Write(m_bCitizen);
	stream->Write(m_bWater);
	stream->Write(m_bScout);
	stream->Write(m_bCanCrossLargeRivers);//WTP, ray, Large Rivers
	stream->Write(m_bCityDefender);
	stream->Write(m_bCanFound);

	// TAC - LbD - Ray - START
	stream->Write(m_LbDused);
	stream->Write(m_expert);
	stream->Write(m_learnLevel);
	// TAC - LbD - Ray - END

	stream->Write(m_bUnarmed);
	stream->Write(m_bNoDefensiveBonus);
	stream->Write((int)m_aYieldEquipments.size());
	for(int i=0;i<(int)m_aYieldEquipments.size();i++)
	{
		stream->Write(m_aYieldEquipments[i].iYieldType);
		stream->Write(m_aYieldEquipments[i].iYieldAmount);
	}
	stream->Write(GC.getNumPromotionInfos(), m_abFreePromotions);

	// R&R, ray , MYCP partially based on code of Aymerick - START
	stream->Write(m_aiYieldsProduced.size());
	for (std::vector<int>::iterator it = m_aiYieldsProduced.begin(); it != m_aiYieldsProduced.end(); ++it)
	{
		stream->Write(*it);
	}

	stream->Write(m_aiYieldsConsumed.size());
	for (std::vector<int>::iterator it = m_aiYieldsConsumed.begin(); it != m_aiYieldsConsumed.end(); ++it)
	{
		stream->Write(*it);
	}
	// R&R, ray , MYCP partially based on code of Aymerick - END
}
bool CvProfessionInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	m_eIndex = static_cast<ProfessionTypes>(getIndexForType(JIT_ARRAY_PROFESSION, getType()));
	FAssert(m_eIndex != NO_PROFESSION);

	pXML->GetChildXmlValByName(szTextVal, "Combat");
	m_iUnitCombatType = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "DefaultUnitAI");
	m_iDefaultUnitAIType = pXML->FindInInfoClass(szTextVal);
	// R&R, ray , MYCP partially based on code of Aymerick - START
	// pXML->GetChildXmlValByName(szTextVal, "YieldProduced");
	// m_iYieldProduced = pXML->FindInInfoClass(szTextVal);
	// pXML->GetChildXmlValByName(szTextVal, "YieldConsumed");
	// m_iYieldConsumed = pXML->FindInInfoClass(szTextVal);
	// R&R, ray , MYCP partially based on code of Aymerick - END
	pXML->GetChildXmlValByName(szTextVal, "SpecialBuilding");
	m_aszExtraXMLforPass3.push_back(szTextVal);
	pXML->GetChildXmlValByName(&m_iCombatChange, "iCombatChange");
	pXML->GetChildXmlValByName(&m_iBombardRateChangeProfession, "iBombardRateChangeProfession"); // WTP, ray, Cannons to Professions - START
	pXML->GetChildXmlValByName(&m_iBarracksSpaceNeededChange, "iBarracksSpaceNeededChange"); // WTP, ray, new Barracks System - START
	pXML->GetChildXmlValByName(&m_iMovesChange, "iMovesChange");
	pXML->GetChildXmlValByName(&m_iWorkRate, "iWorkRate");
	pXML->GetChildXmlValByName(&m_iMissionaryRate, "iMissionaryRate");
	pXML->GetChildXmlValByName(&m_iNativeTradeRate, "iNativeTradeRate"); // WTP, ray, Native Trade Posts - START
	pXML->GetChildXmlValByName(&m_iPowerValue, "iPower");
	pXML->GetChildXmlValByName(&m_iAssetValue, "iAsset");
	pXML->GetChildXmlValByName(&m_bWorkPlot, "bWorkPlot");
	pXML->GetChildXmlValByName(&m_bCitizen, "bCitizen");
	pXML->GetChildXmlValByName(&m_bWater, "bWater");
	pXML->GetChildXmlValByName(&m_bScout, "bScout");
	pXML->GetChildXmlValByName(&m_bCanCrossLargeRivers, "bCanCrossLargeRivers"); //WTP, ray, Large Rivers
	pXML->GetChildXmlValByName(&m_bCityDefender, "bCityDefender");
	pXML->GetChildXmlValByName(&m_bCanFound, "bCanFound");

	// TAC - LbD - Ray - START
	pXML->GetChildXmlValByName(&m_LbDused, "bUseLbD");
	pXML->GetChildXmlValByName(szTextVal, "ExpertUnit");
	m_expert = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(&m_learnLevel, "iLbDLearnLevel");
	// TAC - LbD - Ray - END

	/// Move Into Peak - start - Nightinggale
	pXML->GetChildXmlValByName(&m_bMoveIntoPeak, "bMoveIntoPeak");
	/// Move Into Peak - end - Nightinggale

	pXML->GetChildXmlValByName(&m_bUnarmed, "bUnarmed");
	pXML->GetChildXmlValByName(&m_bNoDefensiveBonus, "bNoDefensiveBonus");

	m_aYieldEquipments.clear();
	int *aiYieldAmounts;
	pXML->SetVariableListTagPair(&aiYieldAmounts, "YieldEquipedNums", NUM_YIELD_TYPES, 0);
	for (int i = 0; i < NUM_YIELD_TYPES; ++i)
	{
		if (aiYieldAmounts[i] != 0)
		{
			YieldEquipment kYieldEquipment;
			kYieldEquipment.iYieldType = i;
			kYieldEquipment.iYieldAmount = aiYieldAmounts[i];
			m_aYieldEquipments.push_back(kYieldEquipment);
		}
	}
	SAFE_DELETE_ARRAY(aiYieldAmounts);
	pXML->SetVariableListTagPair(&m_abFreePromotions, "FreePromotions", GC.getNumPromotionInfos(), false);
	// R&R, ray , MYCP partially based on code of Aymerick - START
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"YieldsProduced"))
	{
		if (pXML->SkipToNextVal())
		{
			int iNumSibs = gDLL->getXMLIFace()->GetNumChildren(pXML->GetXML());
			m_aiYieldsProduced.clear();
			if (0 < iNumSibs)
			{
				if (pXML->GetChildXmlVal(szTextVal))
				{
					for (int j = 0; j < iNumSibs; j++)
					{
						m_aiYieldsProduced.push_back(pXML->FindInInfoClass(szTextVal));
						if (!pXML->GetNextXmlVal(&szTextVal))
						{
							break;
						}
					}
					gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
				}
			}
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"YieldsConsumed"))
	{
		if (pXML->SkipToNextVal())
		{
			int iNumSibs = gDLL->getXMLIFace()->GetNumChildren(pXML->GetXML());
			m_aiYieldsConsumed.clear();
			if (0 < iNumSibs)
			{
				if (pXML->GetChildXmlVal(szTextVal))
				{
					for (int j = 0; j < iNumSibs; j++)
					{
						m_aiYieldsConsumed.push_back(pXML->FindInInfoClass(szTextVal));
						if (!pXML->GetNextXmlVal(&szTextVal))
						{
							break;
						}
					}
					gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
				}
			}
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	// R&R, ray , MYCP partially based on code of Aymerick - END
	return true;
}

bool CvProfessionInfo::readPass3()
{
	if (m_aszExtraXMLforPass3.size() < 1)
	{
		FAssert(false);
		return false;
	}
	m_iSpecialBuilding = GC.getInfoTypeForString(m_aszExtraXMLforPass3[0]);
	m_aszExtraXMLforPass3.clear();
	return true;
}



//======================================================================================================
//					CvMissionInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvMissionInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvMissionInfo::CvMissionInfo() :
m_iTime(0),
m_bSound(false),
m_bTarget(false),
m_bBuild(false),
m_bVisible(false),
m_eEntityEvent(ENTITY_EVENT_NONE)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvMissionInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvMissionInfo::~CvMissionInfo()
{
}
int CvMissionInfo::getTime() const
{
	return m_iTime;
}
bool CvMissionInfo::isSound() const
{
	return m_bSound;
}
bool CvMissionInfo::isTarget() const
{
	return m_bTarget;
}
bool CvMissionInfo::isBuild() const
{
	return m_bBuild;
}
bool CvMissionInfo::getVisible() const
{
	return m_bVisible;
}
const char* CvMissionInfo::getWaypoint() const
{
	return m_szWaypoint;
}
EntityEventTypes CvMissionInfo::getEntityEvent() const
{
	return m_eEntityEvent;
}
bool CvMissionInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTmp;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(m_szWaypoint, "Waypoint");
	pXML->GetChildXmlValByName(&m_iTime, "iTime");
	pXML->GetChildXmlValByName(&m_bSound, "bSound");
	pXML->GetChildXmlValByName(&m_bTarget, "bTarget");
	pXML->GetChildXmlValByName(&m_bBuild, "bBuild");
	pXML->GetChildXmlValByName(&m_bVisible, "bVisible");
	if ( pXML->GetChildXmlValByName(szTmp, "EntityEventType") )
	{
		m_eEntityEvent = (EntityEventTypes)pXML->FindInInfoClass(szTmp);
	}
	else
	{
		m_eEntityEvent = ENTITY_EVENT_NONE;
	}
	return true;
}

//======================================================================================================
//					CvControlInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvControlInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvControlInfo::CvControlInfo()
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvControlInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvControlInfo::~CvControlInfo()
{
}
bool CvControlInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}
	return true;
}

//======================================================================================================
//					CvCommandInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvCommandInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvCommandInfo::CvCommandInfo() :
m_bConfirmCommand(false),
m_bVisible(false),
m_bAll(false)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvCommandInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvCommandInfo::~CvCommandInfo()
{
}
bool CvCommandInfo::getConfirmCommand() const
{
	return m_bConfirmCommand;
}
bool CvCommandInfo::getVisible() const
{
	return m_bVisible;
}
bool CvCommandInfo::getAll() const
{
	return m_bAll;
}
bool CvCommandInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(&m_bConfirmCommand, "bConfirmCommand");
	pXML->GetChildXmlValByName(&m_bVisible, "bVisible");
	pXML->GetChildXmlValByName(&m_bAll, "bAll");
	return true;
}
//======================================================================================================
//					CvAutomateInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvAutomateInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvAutomateInfo::CvAutomateInfo() :
m_iCommand(NO_COMMAND),
m_iAutomate(NO_AUTOMATE),
m_bConfirmCommand(false),
m_bVisible(false)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvAutomateInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvAutomateInfo::~CvAutomateInfo()
{
}
int CvAutomateInfo::getCommand() const
{
	return m_iCommand;
}
void CvAutomateInfo::setCommand(int i)
{
	m_iCommand = i;
}
int CvAutomateInfo::getAutomate() const
{
	return m_iAutomate;
}
void CvAutomateInfo::setAutomate(int i)
{
	m_iAutomate = i;
}
bool CvAutomateInfo::getConfirmCommand() const
{
	return m_bConfirmCommand;
}
bool CvAutomateInfo::getVisible() const
{
	return m_bVisible;
}
bool CvAutomateInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(szTextVal, "Command");
	setCommand(pXML->FindInInfoClass(szTextVal));
	pXML->GetChildXmlValByName(szTextVal, "Automate");
	setAutomate(GC.getInfoTypeForString(szTextVal));
	pXML->GetChildXmlValByName(&m_bConfirmCommand, "bConfirmCommand");
	pXML->GetChildXmlValByName(&m_bVisible, "bVisible");
	return true;
}
//======================================================================================================
//					CvActionInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvActionInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvActionInfo::CvActionInfo() :
m_eSubType(NO_ACTIONSUBTYPE)
{
}

CvActionInfo::CvActionInfo(InterfaceModeTypes eInterfaceMode)
	: m_eInterfaceMode(eInterfaceMode)
	, m_eSubType(ACTIONSUBTYPE_INTERFACEMODE)
{
}

CvActionInfo::CvActionInfo(CommandTypes eCommand)
	: m_eCommand(eCommand)
	, m_eSubType(ACTIONSUBTYPE_COMMAND)
{
}

CvActionInfo::CvActionInfo(BuildTypes eBuild)
	: m_eBuild(eBuild)
	, m_eSubType(ACTIONSUBTYPE_BUILD)
{
}

CvActionInfo::CvActionInfo(PromotionTypes ePromotion)
	: m_ePromotion(ePromotion)
	, m_eSubType(ACTIONSUBTYPE_PROMOTION)
{
}

CvActionInfo::CvActionInfo(UnitTypes eUnit)
	: m_eUnit(eUnit)
	, m_eSubType(ACTIONSUBTYPE_UNIT)
{
}

CvActionInfo::CvActionInfo(ControlTypes eControl)
	: m_eControl(eControl)
	, m_eSubType(ACTIONSUBTYPE_CONTROL)
{
}

CvActionInfo::CvActionInfo(AutomateTypes eAutomate)
	: m_eAutomate(eAutomate)
	, m_eSubType(ACTIONSUBTYPE_AUTOMATE)
{
}

CvActionInfo::CvActionInfo(MissionTypes eMission)
	: m_eMission(eMission)
	, m_eSubType(ACTIONSUBTYPE_MISSION)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvActionInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvActionInfo::~CvActionInfo()
{
}
BuildTypes CvActionInfo::getBuildType() const
{
	if	(ACTIONSUBTYPE_BUILD == m_eSubType)
	{
		return m_eBuild;
	}
	return NO_BUILD;
}
int CvActionInfo::getCommandData() const
{
	int iData = -1;
	switch (m_eSubType)
	{
	case ACTIONSUBTYPE_PROMOTION:
		iData = m_ePromotion;
		break;
	case ACTIONSUBTYPE_UNIT:
		iData = m_eUnit;
		break;
	case ACTIONSUBTYPE_COMMAND:
		if (m_eCommand == COMMAND_SAIL_TO_EUROPE)
		{
			iData = UNIT_TRAVEL_STATE_TO_EUROPE;
		}
		break;
	case ACTIONSUBTYPE_AUTOMATE:
		iData = GC.getAutomateInfo(m_eAutomate).getAutomate();
		break;
	default:
		break;
	}
	return iData;
}
AutomateTypes CvActionInfo::getAutomateType() const
{
	if (ACTIONSUBTYPE_AUTOMATE == m_eSubType)
	{
		return (AutomateTypes)GC.getAutomateInfo(m_eAutomate).getAutomate();
	}
	return NO_AUTOMATE;
}
InterfaceModeTypes CvActionInfo::getInterfaceModeType() const
{
	if (ACTIONSUBTYPE_INTERFACEMODE == m_eSubType)
	{
		return m_eInterfaceMode;
	}
	return NO_INTERFACEMODE;
}
MissionTypes CvActionInfo::getMissionType() const
{
	if (ACTIONSUBTYPE_BUILD == m_eSubType)
	{
		return LoopBuildTypes::info(m_eBuild).getMissionType();
	}
	else if (ACTIONSUBTYPE_MISSION == m_eSubType)
	{
		return m_eMission;
	}
	return NO_MISSION;
}
CommandTypes CvActionInfo::getCommandType() const
{
	if (ACTIONSUBTYPE_COMMAND == m_eSubType)
	{
		return m_eCommand;
	}
	else if (ACTIONSUBTYPE_PROMOTION == m_eSubType)
	{
		return (CommandTypes)GC.getPromotionInfo(m_ePromotion).getCommandType();
	}
	else if (ACTIONSUBTYPE_UNIT == m_eSubType)
	{
		return (CommandTypes)GC.getUnitInfo(m_eUnit).getCommandType();
	}
	else if (ACTIONSUBTYPE_AUTOMATE == m_eSubType)
	{
		return (CommandTypes)GC.getAutomateInfo(m_eAutomate).getCommand();
	}
	return NO_COMMAND;
}
ControlTypes CvActionInfo::getControlType() const
{
	if (ACTIONSUBTYPE_CONTROL == m_eSubType)
	{
		return m_eControl;
	}
	return NO_CONTROL;
}
int CvActionInfo::getOriginalIndex() const
{
	switch (m_eSubType)
	{
	case ACTIONSUBTYPE_INTERFACEMODE: return m_eInterfaceMode;
	case ACTIONSUBTYPE_COMMAND: return m_eCommand;
	case ACTIONSUBTYPE_BUILD: return m_eBuild;
	case ACTIONSUBTYPE_PROMOTION: return m_ePromotion;
	case ACTIONSUBTYPE_UNIT: return m_eUnit;
	case ACTIONSUBTYPE_CONTROL: return m_eControl;
	case ACTIONSUBTYPE_AUTOMATE: return m_eAutomate;
	case ACTIONSUBTYPE_MISSION: return m_eMission;
	}
	FAssert(false);
	return -1;
}
bool CvActionInfo::isConfirmCommand() const
{
	if	(ACTIONSUBTYPE_COMMAND == m_eSubType)
	{
		return GC.getCommandInfo(m_eCommand).getConfirmCommand();
	}
	else if (ACTIONSUBTYPE_AUTOMATE == m_eSubType)
	{
		return GC.getAutomateInfo(m_eAutomate).getConfirmCommand();
	}
	return false;
}
bool CvActionInfo::isVisible() const
{
	if (ACTIONSUBTYPE_CONTROL == m_eSubType)
	{
		return false;
	}
	else if	(ACTIONSUBTYPE_COMMAND == m_eSubType)
	{
		return GC.getCommandInfo(m_eCommand).getVisible();
	}
	else if (ACTIONSUBTYPE_AUTOMATE == m_eSubType)
	{
		return GC.getAutomateInfo(m_eAutomate).getVisible();
	}
	else if (ACTIONSUBTYPE_MISSION == m_eSubType)
	{
		return GC.getMissionInfo(m_eMission).getVisible();
	}
	else if (ACTIONSUBTYPE_INTERFACEMODE== m_eSubType)
	{
		return GC.getInterfaceModeInfo(m_eInterfaceMode).getVisible();
	}
	else if (ACTIONSUBTYPE_PROMOTION == m_eSubType)
	{
		return false;
	}
	return true;
}
ActionSubTypes CvActionInfo::getSubType() const
{
	return m_eSubType;
}
CvHotkeyInfo* CvActionInfo::getHotkeyInfo() const
{
	switch (getSubType())
	{
	case ACTIONSUBTYPE_INTERFACEMODE:
		return &GC.getInterfaceModeInfo(m_eInterfaceMode);
		break;
	case ACTIONSUBTYPE_COMMAND:
		return &GC.getCommandInfo(m_eCommand);
		break;
	case ACTIONSUBTYPE_BUILD:
		return &INFO.m_info.m_BuildTypes[m_eBuild];
			break;
		case ACTIONSUBTYPE_PROMOTION:
			return &GC.getPromotionInfo(m_ePromotion);
			break;
		case ACTIONSUBTYPE_UNIT:
			return &GC.getUnitInfo(m_eUnit);
			break;
		case ACTIONSUBTYPE_CONTROL:
			return &GC.getControlInfo(m_eControl);
			break;
		case ACTIONSUBTYPE_AUTOMATE:
			return &GC.getAutomateInfo(m_eAutomate);
			break;
		case ACTIONSUBTYPE_MISSION:
			return &GC.getMissionInfo(m_eMission);
			break;
	}
	FAssertMsg((0) ,"Unknown Action Subtype in CvActionInfo::getHotkeyInfo");
	return NULL;
}

void CvActionInfo::setIndex(const ActionTypes eAction)
{
	CvHotkeyInfo* info = getHotkeyInfo();
	info->setActionInfoIndex(eAction);

	switch (getSubType())
	{
	case ACTIONSUBTYPE_BUILD:
		INFO.m_info.m_BuildTypes[m_eBuild].setMissionType(MISSION_BUILD);
		break;
	case ACTIONSUBTYPE_PROMOTION:
		GC.getPromotionInfo(m_ePromotion).setCommandType(COMMAND_PROMOTION);
		break;
	case ACTIONSUBTYPE_UNIT:
		GC.getUnitInfo(m_eUnit).setCommandType(COMMAND_UPGRADE);
		break;
	}
}

const char* CvActionInfo::getType() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getType();
	}
	return NULL;
}
const wchar* CvActionInfo::getDescription() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getDescription();
	}
	return L"";
}
const wchar* CvActionInfo::getCivilopedia() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getCivilopedia();
	}
	return L"";
}
const wchar* CvActionInfo::getHelp() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getHelp();
	}
	return L"";
}
const wchar* CvActionInfo::getStrategy() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getStrategy();
	}
	return L"";
}
const char* CvActionInfo::getButton() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getButton();
	}
	return NULL;
}
const wchar* CvActionInfo::getTextKeyWide() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getTextKeyWide();
	}
	return NULL;
}
ActionTypes CvActionInfo::getActionInfoIndex() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getActionInfoIndex();
	}
	return ActionTypes::NONE;
}
int CvActionInfo::getHotKeyVal() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getHotKey().value();
	}
	return -1;
}
int CvActionInfo::getHotKeyPriority() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getHotKeyPriority();
	}
	return -1;
}
int CvActionInfo::getHotKeyValAlt() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getHotKeyValAlt().value();
	}
	return -1;
}
int CvActionInfo::getHotKeyPriorityAlt() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getHotKeyPriorityAlt();
	}
	return -1;
}
int CvActionInfo::getOrderPriority() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getOrderPriority();
	}
	return -1;
}
bool CvActionInfo::isAltDown() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->isAltDown();
	}
	return false;
}
bool CvActionInfo::isShiftDown() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->isShiftDown();
	}
	return false;
}
bool CvActionInfo::isCtrlDown() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->isCtrlDown();
	}
	return false;
}
bool CvActionInfo::isAltDownAlt() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->isAltDownAlt();
	}
	return false;
}
bool CvActionInfo::isShiftDownAlt() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->isShiftDownAlt();
	}
	return false;
}
bool CvActionInfo::isCtrlDownAlt() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->isCtrlDownAlt();
	}
	return false;
}
const char* CvActionInfo::getHotKey() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getHotKey().getString();
	}
	return NULL;
}
std::wstring CvActionInfo::getHotKeyDescription() const
{
	const CvHotkeyInfo* info = getHotkeyInfo();

	if (info == NULL)
	{
		FAssertMsg(info != NULL, "CvActionInfo has no hotkey info");
		return L"";
	}

	CvWString szTemp;
	CvWString szHotKeyDescriptionKey = info->getTextKeyWide();
	CvWString szHotKeyString = CreateHotKeyFromDescription(info->getHotKey(), info->isShiftDown(), info->isAltDown(), info->isCtrlDown());

	CommandTypes eCommand = NO_COMMAND;

	switch (getSubType())
	{
	case ACTIONSUBTYPE_PROMOTION:
		eCommand = (CommandTypes)GC.getPromotionInfo(m_ePromotion).getCommandType();
		break;
	case ACTIONSUBTYPE_UNIT:
		eCommand = (CommandTypes)GC.getUnitInfo(m_eUnit).getCommandType();
		break;
	}

	if (eCommand != NO_COMMAND)
	{
		szTemp.Format(L"%s (%s)", gDLL->getObjectText(GC.getCommandInfo(eCommand).getTextKeyWide(), 0).GetCString(), gDLL->getObjectText(szHotKeyDescriptionKey, 0).GetCString());
	}
	else
	{
		szTemp = gDLL->getObjectText(szHotKeyDescriptionKey, 0);
	}

	if (!szHotKeyString.empty())
	{
		szTemp += szHotKeyString;
	}

	return szTemp;
}

CvWString CvActionInfo::CreateHotKeyFromDescription(KeyboardKeyTypes eHotKey, bool bShift, bool bAlt, bool bCtrl) const
{
	if (eHotKey == eHotKey.NONE)
	{
		return "";
	}

	CvWString szHotKey;

	szHotKey += L" <color=140,255,40,255>";
	szHotKey += L"&lt;";

	if (bShift)
	{
		szHotKey += gDLL->getText("TXT_KEY_SHIFT");
	}

	if (bAlt)
	{
		szHotKey += gDLL->getText("TXT_KEY_ALT");
	}

	if (bCtrl)
	{
		szHotKey += gDLL->getText("TXT_KEY_CTRL");
	}

	szHotKey += eHotKey.getReadableText();
	szHotKey += L">";
	szHotKey += L"</color>";

	return szHotKey;
}

bool CvActionInfo::operator<(const CvActionInfo rhs) const
{
	const int priority = getOrderPriority();
	const int rhs_priority = rhs.getOrderPriority();

	// highest priority first
	if (priority != rhs_priority)
	{
		return priority > rhs_priority;
	}

	const ActionSubTypes subtype = getSubType();
	const ActionSubTypes rhs_subtype = rhs.getSubType();

	// sort by which file the hotkey comes from
	if (subtype != rhs_subtype)
	{
		return subtype < rhs_subtype;
	}

	// when everything else fails, sort by index in the file
	return getOriginalIndex() < rhs.getOriginalIndex();
}
//======================================================================================================
//					CvUnitInfo
//======================================================================================================
CvUnitMeshGroup::CvUnitMeshGroup() :
	m_iNumRequired(0)
{
}
CvUnitMeshGroup::CvUnitMeshGroup(int iNumRequired, const CvString &szArtDefineTag) :
	m_iNumRequired(iNumRequired),
	m_szArtDefineTag(szArtDefineTag)
{
}
CvUnitMeshGroup::~CvUnitMeshGroup()
{
}
int CvUnitMeshGroup::getNumRequired() const
{
	return m_iNumRequired;
}
const CvString& CvUnitMeshGroup::getArtDefineTag() const
{
	return m_szArtDefineTag;
}
void CvUnitMeshGroup::read(FDataStreamBase* stream)
{
	stream->Read(&m_iNumRequired);
	stream->ReadString(m_szArtDefineTag);
}
void CvUnitMeshGroup::write(FDataStreamBase* stream)
{
	stream->Write(m_iNumRequired);
	stream->WriteString(m_szArtDefineTag);
}

CvUnitMeshGroups::CvUnitMeshGroups() :
	m_eProfession(NO_PROFESSION),
	m_iMeleeWaveSize(0),
	m_iRangedWaveSize(0),
	m_fMaxSpeed(0),
	m_fPadTime(0)
{
}
CvUnitMeshGroups::CvUnitMeshGroups(ProfessionTypes eProfession, int iMeleeWaveSize, int iRangedWaveSize, float fMaxSpeed, float fPadTime) :
	m_eProfession(eProfession),
	m_iMeleeWaveSize(iMeleeWaveSize),
	m_iRangedWaveSize(iRangedWaveSize),
	m_fMaxSpeed(fMaxSpeed),
	m_fPadTime(fPadTime)
{
}
CvUnitMeshGroups::~CvUnitMeshGroups()
{
}
void CvUnitMeshGroups::addMeshGroup(const CvUnitMeshGroup& kMeshGroup)
{
	m_aMeshGroups.push_back(kMeshGroup);
}
ProfessionTypes CvUnitMeshGroups::getProfession() const
{
	return m_eProfession;
}
int CvUnitMeshGroups::getMeleeWaveSize() const
{
	return m_iMeleeWaveSize;
}
int CvUnitMeshGroups::getRangedWaveSize() const
{
	return m_iRangedWaveSize;
}
float CvUnitMeshGroups::getMaxSpeed() const
{
	return m_fMaxSpeed;
}
float CvUnitMeshGroups::getPadTime() const
{
	return m_fPadTime;
}
int CvUnitMeshGroups::getNumMeshGroups() const
{
	return m_aMeshGroups.size();
}
const CvUnitMeshGroup& CvUnitMeshGroups::getMeshGroup(int index) const
{
	FAssert(index >= 0);
	FAssert(index < (int)m_aMeshGroups.size());
	return m_aMeshGroups[index];
}
void CvUnitMeshGroups::read(FDataStreamBase* stream)
{
	stream->Read((int*)&m_eProfession);
	stream->Read(&m_iMeleeWaveSize);
	stream->Read(&m_iRangedWaveSize);
	stream->Read(&m_fMaxSpeed);
	stream->Read(&m_fPadTime);
	int iNumGroups;
	stream->Read(&iNumGroups);
	m_aMeshGroups.clear();
	for (int i = 0; i < iNumGroups; ++i)
	{
		CvUnitMeshGroup kGroup;
		kGroup.read(stream);
		m_aMeshGroups.push_back(kGroup);
	}
}
void CvUnitMeshGroups::write(FDataStreamBase* stream)
{
	stream->Write(m_eProfession);
	stream->Write(m_iMeleeWaveSize);
	stream->Write(m_iRangedWaveSize);
	stream->Write(m_fMaxSpeed);
	stream->Write(m_fPadTime);
	stream->Write((int)m_aMeshGroups.size());
	for (int i = 0; i < (int)m_aMeshGroups.size(); ++i)
	{
		m_aMeshGroups[i].write(stream);
	}
}

/** NBMOD REF **/

/*********************************************************************/
/**                                                                 **/
/** float CvUnitInfo::NBMOD_GetStrengthWeight() const               **/
/** Gibt das St�rke-Gewicht dieser Einheit f�r die EMW/KMW Berech-  **/
/** nun zur�ck.                                                     **/
/**                                                                 **/
/*********************************************************************/
float CvUnitInfo::NBMOD_GetStrengthWeight() const
{
    return m_fNBMOD_REF_StrengthWeight;
}

/** NBMOD REF **/

/** NBMOD EDU **/

/*********************************************************************/
/**                                                                 **/
/** int CvUnitInfo::NBMOD_GetTeachLevel() const                     **/
/** Ermittelt die Bildungsstufe, welche diese Einheit ben�tigt.     **/
/**                                                                 **/
/*********************************************************************/

int CvUnitInfo::NBMOD_GetTeachLevel() const
{

    return m_iTeachLevel;
}

/** NBMOD EDU **/
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvUnitInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvUnitInfo::CvUnitInfo() :

m_iBerthSize(1), // PatchMod: Berth size START
m_iHarbourSpaceNeeded(0), // WTP, ray, new Harbour System - START
m_iBarracksSpaceNeeded(0), // WTP, ray, new Barracks System - START

/** NBMOD EDU **/
m_iTeachLevel(3),
/** NBMOD EDU **/

/** NBMOD REF **/
m_fNBMOD_REF_StrengthWeight(1.0),
/** NBMOD REF **/
m_iAIWeight(0),
m_iHurryCostModifier(0),
m_iProductionWhenUsed(0), // WTP, ray, Construction Supplies - START
m_iEuropeCost(0),
m_iEuropeCostIncrease(0),
/*** TRIANGLETRADE 10/24/08 by DPII ***/
m_iAfricaCost(0),
m_iAfricaCostIncrease(0),
/**************************************/
// R&R, ray, Port Royal
m_iPortRoyalCost(0),
m_iPortRoyalCostIncrease(0),
// R&R, ray, Port Royal - END
m_iImmigrationWeight(0),
m_iImmigrationWeightDecay(0),
m_iAdvancedStartCost(0),
m_iAdvancedStartCostIncrease(0),
m_iMinAreaSize(0),
m_iMoves(0),
m_iWorkRate(0),
m_iWorkRateModifier(0),
m_iGoldFromGoodiesAndChiefsModifier(0), // WTP, ray, Scout Gold Modifier for Goodies and Chiefs at Unit - START
m_iMissionaryRateModifier(0),
m_iNativeTradeRateModifier(0), // WTP, ray, Native Trade Posts - START
m_iCombat(0),
m_iXPValueAttack(0),
m_iXPValueDefense(0),
m_iWithdrawalProbability(0),
m_iCityAttackModifier(0),
m_iCityDefenseModifier(0),
m_iHillsAttackModifier(0),
m_iHillsDefenseModifier(0),
m_iBombardRate(0),
m_iSpecialCargo(0),
m_iDomainCargo(0),
m_iCargoSpace(0),
m_iRequiredTransportSize(0),
m_iAssetValue(0),
m_iPowerValue(0),
m_eUnitClassType(NO_UNITCLASS),
m_iSpecialUnitType(NO_SPECIALUNIT),
m_iUnitCaptureClassType(NO_UNITCLASS),
m_iUnitCombatType(NO_UNITCOMBAT),
m_iDomainType(NO_DOMAIN),
m_eDefaultProfession(NO_PROFESSION),
m_iDefaultUnitAIType(NO_UNITAI),
m_iInvisibleType(NO_INVISIBLE),
m_iPrereqBuilding(NO_BUILDING),
m_iNumUnitNames(0),
m_iCommandType(NO_COMMAND),
m_iLeaderExperience(0),
m_iLearnTime(-1),
m_iStudentWeight(0),
m_iTeacherWeight(0),
// < JAnimals Mod Start >
m_iAnimalPatrolWeight(0),
m_iAnimalAttackWeight(0),
// < JAnimals Mod End >
m_bNoBadGoodies(false),
m_bOnlyDefensive(false),
m_bNoCapture(false),
m_bQuickCombat(false),
m_bRivalTerritory(false),
m_bMilitaryProduction(false),
m_bFound(false),
m_bInvisible(false),
m_bNoDefensiveBonus(false),
m_bCanMoveImpassable(false),
m_bCanMoveAllTerrain(false),
m_bFlatMovementCost(false),
m_bIgnoreTerrainCost(false),
m_bMechanized(false),
m_bLineOfSight(false),
m_bHiddenNationality(false),
m_bSlaveShip(false), // WTP, ray Slave Ship
m_bTreasureShip(false), // WTP, ray Treasure Ship
m_bTroopShip(false), // WTP, ray Troop Ship
m_bAlwaysHostile(false),
m_bTreasure(false),
m_eEthnicity(NO_ETHNICITY), // WTP, ray, Ethnically correct Population Growth - START
m_eCitizenStatus(NO_CITIZEN_STATUS), // WTP, ray, Ethnically correct Population Growth - START
// R&R, ray, Changes for Treasures, START
m_bNoRevealMap(false),
// R&R, ray, Changes for Treasures, END
// TAC - LbD - Ray - START
m_canBecomeExpert(false),
m_canGetFree(false),
m_canEscape(false),
// TAC - LbD - Ray - END
// WTP, ray, LbD Slaves Revolt and Free - START
m_canRevolt(false),
// WTP, ray, LbD Slaves Revolt and Free - END
m_bCapturesCargo(false),
// TAC Capturing Ships - ray
m_bCapturesShips(false),
m_iCaptureShipsChanceIncrease(0), // WTP, ray, Capture Ship chance increase - START
// TAC Capturing Ships - ray - END
m_bLandYieldChanges(false),
m_bWaterYieldChanges(false),
//TAC Whaling, ray
m_bGatherBoat(false),
//End TAC Whaling, ray
// < JAnimals Mod Start >
m_bAnimal(false),
// < JAnimals Mod End >
m_eLeaderPromotion(NO_PROMOTION),
/// Move Into Peak - start - Nightinggale
m_bMoveIntoPeak(false),
/// Move Into Peak - end - Nightinggale
m_abUpgradeUnitClass(NULL),
m_abUnitAIType(NULL),
m_abNotUnitAIType(NULL),
m_abBuilds(NULL),
m_abTerrainImpassable(NULL),
m_abFeatureImpassable(NULL),
// < JAnimals Mod Start >
m_abTerrainNative(NULL),
// < JAnimals Mod End >
m_abEvasionBuilding(NULL),
//TAC Whaling, ray
m_abYieldGatherable(NULL),
//End TAC Whaling, ray
m_aiProductionTraits(NULL),
m_aiTerrainAttackModifier(NULL),
m_aiTerrainDefenseModifier(NULL),
m_aiFeatureAttackModifier(NULL),
m_aiFeatureDefenseModifier(NULL),
m_aiUnitClassAttackModifier(NULL),
m_aiUnitClassDefenseModifier(NULL),
m_aiUnitCombatModifier(NULL),
m_aiDomainModifier(NULL),
m_aiYieldModifier(NULL),
m_aiBonusYieldChange(NULL),
m_aiYieldChange(NULL),
m_aiYieldCost(NULL),
m_abFreePromotions(NULL),
///TK Viscos Mod
m_abProfessionsNotAllowed(NULL),
///TK end
m_abPrereqOrBuilding(NULL),
m_paszUnitNames(NULL)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvUnitInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvUnitInfo::~CvUnitInfo()
{
	SAFE_DELETE_ARRAY(m_abUpgradeUnitClass);
	SAFE_DELETE_ARRAY(m_abUnitAIType);
	SAFE_DELETE_ARRAY(m_abNotUnitAIType);
	SAFE_DELETE_ARRAY(m_abBuilds);
	SAFE_DELETE_ARRAY(m_abTerrainImpassable);
	SAFE_DELETE_ARRAY(m_abFeatureImpassable);
	// < JAnimals Mod Start >
	SAFE_DELETE_ARRAY(m_abTerrainNative);
	// < JAnimals Mod End >
	SAFE_DELETE_ARRAY(m_abEvasionBuilding);
	//TAC Whaling, ray
	SAFE_DELETE_ARRAY(m_abYieldGatherable);
	//End TAC Whaling, ray
	SAFE_DELETE_ARRAY(m_aiProductionTraits);
	SAFE_DELETE_ARRAY(m_aiTerrainAttackModifier);
	SAFE_DELETE_ARRAY(m_aiTerrainDefenseModifier);
	SAFE_DELETE_ARRAY(m_aiFeatureAttackModifier);
	SAFE_DELETE_ARRAY(m_aiFeatureDefenseModifier);
	SAFE_DELETE_ARRAY(m_aiUnitClassAttackModifier);
	SAFE_DELETE_ARRAY(m_aiUnitClassDefenseModifier);
	SAFE_DELETE_ARRAY(m_aiUnitCombatModifier);
	SAFE_DELETE_ARRAY(m_aiDomainModifier);
	SAFE_DELETE_ARRAY(m_aiYieldModifier);
	SAFE_DELETE_ARRAY(m_aiBonusYieldChange);
	SAFE_DELETE_ARRAY(m_aiYieldChange);
	SAFE_DELETE_ARRAY(m_aiYieldCost);
	SAFE_DELETE_ARRAY(m_abFreePromotions);
	///TK Viscos Mod
	SAFE_DELETE_ARRAY(m_abProfessionsNotAllowed);
	///TK end
	SAFE_DELETE_ARRAY(m_abPrereqOrBuilding);
	SAFE_DELETE_ARRAY(m_paszUnitNames);
}
int CvUnitInfo::getAIWeight() const
{
	return m_iAIWeight;
}
int CvUnitInfo::getHurryCostModifier() const
{
	return m_iHurryCostModifier;
}

// WTP, ray, Construction Supplies - START
int CvUnitInfo::getProductionWhenUsed() const
{
	return m_iProductionWhenUsed;
}
// WTP, ray, Construction Supplies - END

int CvUnitInfo::getEuropeCost() const
{
	return m_iEuropeCost;
}
int CvUnitInfo::getEuropeCostIncrease() const
{
	return m_iEuropeCostIncrease;
}

/*** TRIANGLETRADE 10/24/08 by DPII ***/
int CvUnitInfo::getAfricaCost() const
{
	return m_iAfricaCost;
}
int CvUnitInfo::getAfricaCostIncrease() const
{
	return m_iAfricaCostIncrease;
}
/**************************************/

// R&R, ray, Port Royal
int CvUnitInfo::getPortRoyalCost() const
{
	return m_iPortRoyalCost;
}
int CvUnitInfo::getPortRoyalCostIncrease() const
{
	return m_iPortRoyalCostIncrease;
}
// R&R, ray, Port Royal - END

int CvUnitInfo::getImmigrationWeight() const
{
	return m_iImmigrationWeight;
}
int CvUnitInfo::getImmigrationWeightDecay() const
{
	return m_iImmigrationWeightDecay;
}
int CvUnitInfo::getAdvancedStartCost() const
{
	return m_iAdvancedStartCost;
}
int CvUnitInfo::getAdvancedStartCostIncrease() const
{
	return m_iAdvancedStartCostIncrease;
}
int CvUnitInfo::getMinAreaSize() const
{
	return m_iMinAreaSize;
}
int CvUnitInfo::getMoves() const
{
	return m_iMoves;
}
int CvUnitInfo::getWorkRate() const
{
	return m_iWorkRate;
}
int CvUnitInfo::getWorkRateModifier() const
{
	return m_iWorkRateModifier;
}
// WTP, ray, Scout Gold Modifier for Goodies and Chiefs at Unit - START
int CvUnitInfo::getGoldFromGoodiesAndChiefsModifier() const
{
	return m_iGoldFromGoodiesAndChiefsModifier;
}
// WTP, ray, Scout Gold Modifier for Goodies and Chiefs at Unit - END
int CvUnitInfo::getMissionaryRateModifier() const
{
	return m_iMissionaryRateModifier;
}
// WTP, ray, Native Trade Posts - START
int CvUnitInfo::getNativeTradeRateModifier() const
{
	return m_iNativeTradeRateModifier;
}
// WTP, ray, Native Trade Posts - END
int CvUnitInfo::getCombat() const
{
	return m_iCombat;
}
void CvUnitInfo::setCombat(int iNum)
{
	m_iCombat = iNum;
}
int CvUnitInfo::getXPValueAttack() const
{
	return m_iXPValueAttack;
}
int CvUnitInfo::getXPValueDefense() const
{
	return m_iXPValueDefense;
}
int CvUnitInfo::getWithdrawalProbability() const
{
	return m_iWithdrawalProbability;
}
int CvUnitInfo::getCityAttackModifier() const
{
	return m_iCityAttackModifier;
}
int CvUnitInfo::getCityDefenseModifier() const
{
	return m_iCityDefenseModifier;
}
int CvUnitInfo::getHillsAttackModifier() const
{
	return m_iHillsAttackModifier;
}
int CvUnitInfo::getHillsDefenseModifier() const
{
	return m_iHillsDefenseModifier;
}
int CvUnitInfo::getDomesticBonusModifier() const
{
	return m_iDomesticBonusModifier;
}
int CvUnitInfo::getBombardRate() const
{
	return m_iBombardRate;
}
int CvUnitInfo::getSpecialCargo() const
{
	return m_iSpecialCargo;
}
int CvUnitInfo::getDomainCargo() const
{
	return m_iDomainCargo;
}
int CvUnitInfo::getCargoSpace() const
{
	return m_iCargoSpace;
}
int CvUnitInfo::getRequiredTransportSize() const
{
	return m_iRequiredTransportSize;
}
int CvUnitInfo::getAssetValue() const
{
	return m_iAssetValue;
}
int CvUnitInfo::getPowerValue() const
{
	return m_iPowerValue;
}
UnitClassTypes CvUnitInfo::getUnitClassType() const
{
	return m_eUnitClassType;
}
int CvUnitInfo::getSpecialUnitType() const
{
	return m_iSpecialUnitType;
}
int CvUnitInfo::getUnitCaptureClassType() const
{
	return m_iUnitCaptureClassType;
}
int CvUnitInfo::getUnitCombatType() const
{
	return m_iUnitCombatType;
}
int CvUnitInfo::getDomainType() const
{
	return m_iDomainType;
}
ProfessionTypes CvUnitInfo::getDefaultProfession() const
{
	return m_eDefaultProfession;
}
int CvUnitInfo::getDefaultUnitAIType() const
{
	return m_iDefaultUnitAIType;
}
int CvUnitInfo::getInvisibleType() const
{
	return m_iInvisibleType;
}
int CvUnitInfo::getSeeInvisibleType(int i) const
{
	FAssert(i < (int)m_aiSeeInvisibleTypes.size());
	return m_aiSeeInvisibleTypes[i];
}
int CvUnitInfo::getNumSeeInvisibleTypes() const
{
	return (int)m_aiSeeInvisibleTypes.size();
}
int CvUnitInfo::getPrereqBuilding() const
{
	return m_iPrereqBuilding;
}
int CvUnitInfo::getGroupSize(int iProfession) const// the initial number of individuals in the unit group
{
	int iSize = 0;
	for (int i = 0; i < getGroupDefinitions(iProfession); i++)
	{
		iSize += getUnitGroupRequired(i, iProfession);
	}
	return iSize;
}
int CvUnitInfo::getGroupDefinitions(int iProfession) const// the number of UnitMeshGroups for this unit
{
	return getProfessionMeshGroup(iProfession).getNumMeshGroups();
}
int CvUnitInfo::getMeleeWaveSize(int iProfession) const
{
	return getProfessionMeshGroup(iProfession).getMeleeWaveSize();
}
int CvUnitInfo::getRangedWaveSize(int iProfession) const
{
	return getProfessionMeshGroup(iProfession).getRangedWaveSize();
}
int CvUnitInfo::getNumUnitNames() const
{
	return m_iNumUnitNames;
}
int CvUnitInfo::getLearnTime() const
{
	return m_iLearnTime;
}
int CvUnitInfo::getStudentWeight() const
{
	return m_iStudentWeight;
}
int CvUnitInfo::getTeacherWeight() const
{
	return m_iTeacherWeight;
}
// < JAnimals Mod Start >
int CvUnitInfo::getAnimalPatrolWeight() const
{
	return m_iAnimalPatrolWeight;
}
int CvUnitInfo::getAnimalAttackWeight() const
{
	return m_iAnimalAttackWeight;
}
// < JAnimals Mod End >
bool CvUnitInfo::isNoBadGoodies() const
{
	return m_bNoBadGoodies;
}
bool CvUnitInfo::isOnlyDefensive() const
{
	return m_bOnlyDefensive;
}
bool CvUnitInfo::isNoCapture() const
{
	return m_bNoCapture;
}
bool CvUnitInfo::isQuickCombat() const
{
	return m_bQuickCombat;
}
bool CvUnitInfo::isRivalTerritory() const
{
	return m_bRivalTerritory;
}
bool CvUnitInfo::isMilitaryProduction() const
{
	return m_bMilitaryProduction;
}
bool CvUnitInfo::isFound() const
{
	return m_bFound;
}
bool CvUnitInfo::isInvisible() const
{
	return m_bInvisible;
}
void CvUnitInfo::setInvisible(bool bEnable)
{
	m_bInvisible = bEnable;
}
bool CvUnitInfo::isNoDefensiveBonus() const
{
	return m_bNoDefensiveBonus;
}
bool CvUnitInfo::isCanMoveImpassable() const
{
	return m_bCanMoveImpassable;
}
bool CvUnitInfo::isCanMoveAllTerrain() const
{
	return m_bCanMoveAllTerrain;
}
bool CvUnitInfo::isFlatMovementCost() const
{
	return m_bFlatMovementCost;
}
bool CvUnitInfo::isIgnoreTerrainCost() const
{
	return m_bIgnoreTerrainCost;
}
bool CvUnitInfo::isMechUnit() const
{
	return m_bMechanized;
}
bool CvUnitInfo::isLineOfSight() const
{
	return m_bLineOfSight;
}
bool CvUnitInfo::isHiddenNationality() const
{
	return m_bHiddenNationality;
}
// WTP, ray Slave Ship - START
bool CvUnitInfo::isSlaveShip() const
{
	return m_bSlaveShip;
}
// WTP, ray Slave Ship - END

// WTP, ray Treasure Ship - START
bool CvUnitInfo::isTreasureShip() const
{
	return m_bTreasureShip;
}
// WTP, ray Treasure Ship - END

// WTP, ray Troop Ship - START
bool CvUnitInfo::isTroopShip() const
{
	return m_bTroopShip;
}
// WTP, ray Troop Ship - END

bool CvUnitInfo::isAlwaysHostile() const
{
	return m_bAlwaysHostile;
}
bool CvUnitInfo::isTreasure() const
{
	return m_bTreasure;
}

// WTP, ray, Ethnically correct Population Growth - START
EthnicityTypes CvUnitInfo::getEthnicity() const
{
	return m_eEthnicity;
}

CitizenStatusTypes CvUnitInfo::getCitizenStatus() const
{
	return m_eCitizenStatus;
}
// WTP, ray, Ethnically correct Population Growth - END

// R&R, ray, Changes for Treasures, START
bool CvUnitInfo::isNoRevealMap() const
{
	return m_bNoRevealMap;
}
// R&R, ray, Changes for Treasures, END

// TAC - LbD - Ray - START
bool CvUnitInfo::LbD_canBecomeExpert() const
{
	return m_canBecomeExpert;
}

bool CvUnitInfo::LbD_canGetFree() const
{
	return m_canGetFree;
}

bool CvUnitInfo::LbD_canEscape() const
{
	return m_canEscape;
}
// TAC - LbD - Ray - END

// WTP, ray, LbD Slaves Revolt and Free - START
bool CvUnitInfo::LbD_canRevolt() const
{
	return m_canRevolt;
}
// WTP, ray, LbD Slaves Revolt and Free - END

bool CvUnitInfo::isCapturesCargo() const
{
	return m_bCapturesCargo;
}

// TAC Capturing Ships - ray
bool CvUnitInfo::isCapturesShips() const
{
	return m_bCapturesShips;
}
// TAC Capturing Ships - ray - END

// TAC Capturing Ships - ray
int CvUnitInfo::getCaptureShipsChanceIncrease() const
{
	return m_iCaptureShipsChanceIncrease;
}
// TAC Capturing Ships - ray - END

bool CvUnitInfo::isLandYieldChanges() const
{
	return m_bLandYieldChanges;
}
bool CvUnitInfo::isWaterYieldChanges() const
{
	return m_bWaterYieldChanges;
}
//TAC Whaling, ray

// < JAnimals Mod Start >
bool CvUnitInfo::isAnimal() const
{
	return m_bAnimal;
}
// < JAnimals Mod End >

bool CvUnitInfo::isGatherBoat() const
{
	return m_bGatherBoat;
}
//End TAC Whaling, ray
float CvUnitInfo::getUnitMaxSpeed(int iProfession) const
{
	return getProfessionMeshGroup(iProfession).getMaxSpeed();
}
float CvUnitInfo::getUnitPadTime(int iProfession) const
{
	return getProfessionMeshGroup(iProfession).getPadTime();
}
int CvUnitInfo::getCommandType() const
{
	return m_iCommandType;
}
void CvUnitInfo::setCommandType(int iNewType)
{
	m_iCommandType = iNewType;
}

// Arrays
int CvUnitInfo::getProductionTraits(int i) const
{
	FAssertMsg(i < GC.getNumTraitInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiProductionTraits ? m_aiProductionTraits[i] : -1;
}
int CvUnitInfo::getTerrainAttackModifier(int i) const
{
	FAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiTerrainAttackModifier ? m_aiTerrainAttackModifier[i] : -1;
}
int CvUnitInfo::getTerrainDefenseModifier(int i) const
{
	FAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiTerrainDefenseModifier ? m_aiTerrainDefenseModifier[i] : -1;
}
int CvUnitInfo::getFeatureAttackModifier(int i) const
{
	FAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiFeatureAttackModifier ? m_aiFeatureAttackModifier[i] : -1;
}
int CvUnitInfo::getFeatureDefenseModifier(int i) const
{
	FAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiFeatureDefenseModifier ? m_aiFeatureDefenseModifier[i] : -1;
}
int CvUnitInfo::getUnitClassAttackModifier(int i) const
{
	FAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiUnitClassAttackModifier ? m_aiUnitClassAttackModifier[i] : -1;
}
int CvUnitInfo::getUnitClassDefenseModifier(int i) const
{
	FAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiUnitClassDefenseModifier ? m_aiUnitClassDefenseModifier[i] : -1;
}
int CvUnitInfo::getUnitCombatModifier(int i) const
{
	FAssertMsg(i < GC.getNumUnitCombatInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiUnitCombatModifier ? m_aiUnitCombatModifier[i] : -1;
}
int CvUnitInfo::getDomainModifier(int i) const
{
	FAssertMsg(i < NUM_DOMAIN_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiDomainModifier ? m_aiDomainModifier[i] : -1;
}
int CvUnitInfo::getYieldModifier(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiYieldModifier ? m_aiYieldModifier[i] : -1;
}
// R&R, Androrc, Domestic Market -- modified by Nightinggale - start
const InfoArray<YieldTypes, int>& CvUnitInfo::getYieldDemands() const
{
	return m_info_YieldDemands;
}
// R&R, Androrc, Domestic Market -- modified by Nightinggale - end
int CvUnitInfo::getBonusYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiBonusYieldChange ? m_aiBonusYieldChange[i] : -1;
}
int CvUnitInfo::getYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiYieldChange ? m_aiYieldChange[i] : -1;
}
int CvUnitInfo::getYieldCost(YieldTypes eYield) const
{
	FAssert(validEnumRange(eYield));
	return m_aiYieldCost ? m_aiYieldCost[eYield] : -1;
}
int CvUnitInfo::getUnitGroupRequired(int i, int iProfession) const
{
	return getProfessionMeshGroup(iProfession).getMeshGroup(i).getNumRequired();
}
bool CvUnitInfo::getUpgradeUnitClass(int i) const
{
	FAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abUpgradeUnitClass ? m_abUpgradeUnitClass[i] : false;
}
bool CvUnitInfo::getUnitAIType(int i) const
{
	FAssertMsg(i < NUM_UNITAI_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abUnitAIType ? m_abUnitAIType[i] : false;
}
bool CvUnitInfo::getNotUnitAIType(int i) const
{
	FAssertMsg(i < NUM_UNITAI_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abNotUnitAIType ? m_abNotUnitAIType[i] : false;
}
bool CvUnitInfo::getBuilds(int i) const
{
	LoopBuildTypes eBuild = LoopBuildTypes::createFromInt(i);
	FAssertMsg(eBuild.isValid(), "Index out of bounds");
	return m_abBuilds ? m_abBuilds[eBuild] : false;
}
bool CvUnitInfo::getTerrainImpassable(int i) const
{
	FAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abTerrainImpassable ? m_abTerrainImpassable[i] : false;
}
bool CvUnitInfo::getFeatureImpassable(int i) const
{
	FAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abFeatureImpassable ? m_abFeatureImpassable[i] : false;
}

// < JAnimals Mod Start >
bool CvUnitInfo::getTerrainNative(int iIndex) const
{
	FAssertMsg(iIndex < GC.getNumTerrainInfos(), "Index out of bounds");
	FAssertMsg(iIndex > -1, "Index out of bounds");
	return m_abTerrainNative ? m_abTerrainNative[iIndex] : false;
}
// < JAnimals Mod End >

bool CvUnitInfo::isEvasionBuilding(int i) const
{
	FAssertMsg(i > -1 && i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	return m_abEvasionBuilding ? m_abEvasionBuilding[i] : false;
}
//TAC Whaling, ray
int CvUnitInfo::getNumYieldsGatherable() const
{
	int iNumYields = 0;
	for (int i = 0; i < NUM_YIELD_TYPES; i++)
	{
		if (m_abYieldGatherable[i])
		{
			iNumYields++;
		}
	}

	return iNumYields;
}
bool CvUnitInfo::canGatherYield(int i) const
{
	FAssert(i < NUM_YIELD_TYPES);
	FAssert(i > -1);
	return m_abYieldGatherable ? m_abYieldGatherable[i] : false;
}
//End TAC Whaling, ray
bool CvUnitInfo::getFreePromotions(int i) const
{
	FAssertMsg(i < GC.getNumPromotionInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abFreePromotions ? m_abFreePromotions[i] : false;
}
///TK Viscos Mod
bool CvUnitInfo::getProfessionsNotAllowed(int i) const
{
	return m_abProfessionsNotAllowed ? m_abProfessionsNotAllowed[i] : false;
}
///TK end
bool CvUnitInfo::isPrereqOrBuilding(int i) const
{
	FAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abPrereqOrBuilding ? m_abPrereqOrBuilding[i] : false;
}
PromotionTypes CvUnitInfo::getLeaderPromotion() const
{
	return m_eLeaderPromotion;
}
int CvUnitInfo::getLeaderExperience() const
{
	return m_iLeaderExperience;
}

//Androrc UnitArtStyles
//const char* CvUnitInfo::getArtDefineTag(int index, int iProfession) const
const char* CvUnitInfo::getArtDefineTag(int index, int iProfession, int iStyle) const
//Androrc End
{
	//Androrc UnitArtStyles
	UnitArtStyleTypes eStyle = (UnitArtStyleTypes) iStyle;

	if (NO_UNIT_ARTSTYLE != eStyle)
	{
		int iUnit = GC.getInfoTypeForString(getType());
		if (-1 != iUnit)
		{
			const char* pcTag = GC.getUnitArtStyleTypeInfo(eStyle).getArtDefineTag(index, iUnit, iProfession);
			if (NULL != pcTag)
			{
				return pcTag;
			}
			else if (-1 == iProfession)//retrieve the unit art style tag for no profession if the one for the desired profession is missing
			{
				pcTag = GC.getUnitArtStyleTypeInfo(eStyle).getArtDefineTag(index, iUnit, -1);
				if (NULL != pcTag)
{
					return pcTag;
				}
			}
		}
	}
	//Androrc End

	return getProfessionMeshGroup(iProfession).getMeshGroup(index).getArtDefineTag();
}

const char* CvUnitInfo::getUnitNames(int i) const
{
	FAssertMsg(i < getNumUnitNames(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return (m_paszUnitNames) ? m_paszUnitNames[i] : NULL;
}
const char* CvUnitInfo::getFormationType() const
{
	return m_szFormationType;
}
const char* CvUnitInfo::getButton() const
{
	return m_szArtDefineButton;
}
void CvUnitInfo::updateArtDefineButton()
{
	m_szArtDefineButton = getArtInfo(0, NO_PROFESSION)->getButton();
}
const CvArtInfoUnit* CvUnitInfo::getArtInfo(int index, int iProfession) const
{
	//Androrc UnitArtStyles
//	return ARTFILEMGR.getUnitArtInfo(getArtDefineTag(index, iProfession));
	UnitArtStyleTypes eStyle = NO_UNIT_ARTSTYLE;
	if (GC.getGameINLINE().isFinalInitialized())
	{
		eStyle = (UnitArtStyleTypes) GC.getCivilizationInfo(GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getCivilizationType()).getUnitArtStyleType();
	}
	return ARTFILEMGR.getUnitArtInfo(getArtDefineTag(index, iProfession, eStyle));
	//Androrc End
}
//Androrc UnitArtStyles
const CvArtInfoUnit* CvUnitInfo::getUnitArtStylesArtInfo(int index, int iProfession, int iStyle) const
{
	UnitArtStyleTypes eStyle = (UnitArtStyleTypes) iStyle;
	return ARTFILEMGR.getUnitArtInfo(getArtDefineTag(index, iProfession, eStyle));
}
//Androrc End
const CvUnitMeshGroups& CvUnitInfo::getProfessionMeshGroup(int iProfession) const
{
	FAssert(m_aProfessionGroups.size() > 0);
	for(int i=0;i<(int)m_aProfessionGroups.size();i++)
	{
		if(m_aProfessionGroups[i].getProfession() == iProfession)
		{
			return m_aProfessionGroups[i];
		}
	}
	for(int i=0;i<(int)m_aProfessionGroups.size();i++)
	{
		if(m_aProfessionGroups[i].getProfession() == NO_PROFESSION)
		{
			return m_aProfessionGroups[i];
		}
	}
	return m_aProfessionGroups[0];
}
void CvUnitInfo::read(FDataStreamBase* stream)
{
	CvHotkeyInfo::read(stream);
	uint uiFlag=0;
	stream->Read(&uiFlag);	// flags for expansion
	stream->Read(&m_iAIWeight);
	/** NBMOD EDU **/
	stream->Read(&m_iTeachLevel);
	/** NBMOD EDU **/

    /** NBMOD REF **/
	stream->Read(&m_fNBMOD_REF_StrengthWeight);
    /** NBMOD REF **/

	stream->Read(&m_iHurryCostModifier);
	stream->Read(&m_iProductionWhenUsed); // WTP, ray, Construction Supplies - START
	stream->Read(&m_iEuropeCost);
	stream->Read(&m_iEuropeCostIncrease);
	/*** TRIANGLETRADE 10/24/08 by DPII ***/
	stream->Read(&m_iAfricaCost);
	stream->Read(&m_iAfricaCostIncrease);
	/**************************************/
	// R&R, ray, Port Royal
	stream->Read(&m_iPortRoyalCost);
	stream->Read(&m_iPortRoyalCostIncrease);
	// R&R, ray, Port Royal - END
	stream->Read(&m_iImmigrationWeight);
	stream->Read(&m_iImmigrationWeightDecay);
	stream->Read(&m_iAdvancedStartCost);
	stream->Read(&m_iAdvancedStartCostIncrease);
	stream->Read(&m_iMinAreaSize);
	stream->Read(&m_iMoves);
	stream->Read(&m_iWorkRate);
	stream->Read(&m_iWorkRateModifier);
	stream->Read(&m_iGoldFromGoodiesAndChiefsModifier); // WTP, ray, Scout Gold Modifier for Goodies and Chiefs at Unit - START
	stream->Read(&m_iMissionaryRateModifier);
	stream->Read(&m_iNativeTradeRateModifier); // WTP, ray, Native Trade Posts - START
	stream->Read(&m_iCombat);
	stream->Read(&m_iXPValueAttack);
	stream->Read(&m_iXPValueDefense);
	stream->Read(&m_iWithdrawalProbability);
	stream->Read(&m_iCityAttackModifier);
	stream->Read(&m_iCityDefenseModifier);
	stream->Read(&m_iHillsAttackModifier);
	stream->Read(&m_iHillsDefenseModifier);
	stream->Read(&m_iDomesticBonusModifier);
	stream->Read(&m_iBombardRate);
	stream->Read(&m_iSpecialCargo);
	stream->Read(&m_iDomainCargo);
	stream->Read(&m_iCargoSpace);
	stream->Read(&m_iRequiredTransportSize);
	stream->Read(&m_iBerthSize); // PatchMod: Berth size START
	stream->Read(&m_iHarbourSpaceNeeded); // WTP, ray, new Harbour System - START
	stream->Read(&m_iBarracksSpaceNeeded); // WTP, ray, new Barracks System - START
	stream->Read(&m_iAssetValue);
	stream->Read(&m_iPowerValue);
	stream->Read(&m_eUnitClassType);
	stream->Read(&m_iSpecialUnitType);
	stream->Read(&m_iUnitCaptureClassType);
	stream->Read(&m_iUnitCombatType);
	stream->Read(&m_iDomainType);
	stream->Read(&m_eDefaultProfession);
	stream->Read(&m_iDefaultUnitAIType);
	stream->Read(&m_iInvisibleType);
	int iNumInvisibleTypes;
	stream->Read(&iNumInvisibleTypes);
	for(int i=0;i<iNumInvisibleTypes;i++)
	{
		int iSeeInvisibleType;
		stream->Read(&iSeeInvisibleType);
		m_aiSeeInvisibleTypes.push_back(iSeeInvisibleType);
	}
	stream->Read(&m_iPrereqBuilding);
	stream->Read(&m_iNumUnitNames);
	stream->Read(&m_iCommandType);
	stream->Read(&m_iLearnTime);
	stream->Read(&m_iStudentWeight);
	stream->Read(&m_iTeacherWeight);
	// < JAnimals Mod Start >
	stream->Read(&m_iAnimalPatrolWeight);
	stream->Read(&m_iAnimalAttackWeight);
	// < JAnimals Mod End >
	stream->Read(&m_bNoBadGoodies);
	stream->Read(&m_bOnlyDefensive);
	stream->Read(&m_bNoCapture);
	stream->Read(&m_bQuickCombat);
	stream->Read(&m_bRivalTerritory);
	stream->Read(&m_bMilitaryProduction);
	stream->Read(&m_bFound);
	stream->Read(&m_bInvisible);
	stream->Read(&m_bNoDefensiveBonus);
	stream->Read(&m_bCanMoveImpassable);
	stream->Read(&m_bCanMoveAllTerrain);
	stream->Read(&m_bFlatMovementCost);
	stream->Read(&m_bIgnoreTerrainCost);
	stream->Read(&m_bMechanized);
	stream->Read(&m_bLineOfSight);
	stream->Read(&m_bHiddenNationality);
	stream->Read(&m_bSlaveShip); // WTP, ray Slave Ship
	stream->Read(&m_bTreasureShip); // WTP, ray Treasure Ship
	stream->Read(&m_bTroopShip); // WTP, ray Troop Ship
	stream->Read(&m_bAlwaysHostile);
	stream->Read(&m_bTreasure);
	// R&R, ray, Changes for Treasures, START
	stream->Read(&m_bNoRevealMap);
	// R&R, ray, Changes for Treasures, END

	// TAC - LbD - Ray - START
	stream->Read(&m_canBecomeExpert);
	stream->Read(&m_canGetFree);
	stream->Read(&m_canEscape);
	// TAC - LbD - Ray - END

	// WTP, ray, LbD Slaves Revolt and Free - START
	stream->Read(&m_canRevolt);
	// WTP, ray, LbD Slaves Revolt and Free - END

	stream->Read(&m_bCapturesCargo);
	// TAC Capturing Ships - ray
	stream->Read(&m_bCapturesShips);
	stream->Read(&m_iCaptureShipsChanceIncrease); // WTP, ray, Capture Ship chance increase - START
	// TAC Capturing Ships - ray - End
	stream->Read(&m_bLandYieldChanges);
	stream->Read(&m_bWaterYieldChanges);
	//TAC Whaling, ray
	stream->Read(&m_bGatherBoat);
	//End TAC Whaling, ray

	// < JAnimals Mod Start >
	stream->Read(&m_bAnimal);
	// < JAnimals Mod End >

	int iSize;
	stream->Read(&iSize);
	m_aProfessionGroups.clear();
	for (int i = 0; i < iSize; ++i)
	{
		CvUnitMeshGroups kGroup;
		kGroup.read(stream);
		m_aProfessionGroups.push_back(kGroup);
	}
	SAFE_DELETE_ARRAY(m_aiProductionTraits);
	m_aiProductionTraits = new int[GC.getNumTraitInfos()];
	stream->Read(GC.getNumTraitInfos(), m_aiProductionTraits);
	SAFE_DELETE_ARRAY(m_aiTerrainAttackModifier);
	m_aiTerrainAttackModifier = new int[GC.getNumTerrainInfos()];
	stream->Read(GC.getNumTerrainInfos(), m_aiTerrainAttackModifier);
	SAFE_DELETE_ARRAY(m_aiTerrainDefenseModifier);
	m_aiTerrainDefenseModifier = new int[GC.getNumTerrainInfos()];
	stream->Read(GC.getNumTerrainInfos(), m_aiTerrainDefenseModifier);
	SAFE_DELETE_ARRAY(m_aiFeatureAttackModifier);
	m_aiFeatureAttackModifier = new int[GC.getNumFeatureInfos()];
	stream->Read(GC.getNumFeatureInfos(), m_aiFeatureAttackModifier);
	SAFE_DELETE_ARRAY(m_aiFeatureDefenseModifier);
	m_aiFeatureDefenseModifier = new int[GC.getNumFeatureInfos()];
	stream->Read(GC.getNumFeatureInfos(), m_aiFeatureDefenseModifier);
	SAFE_DELETE_ARRAY(m_aiUnitClassAttackModifier);
	m_aiUnitClassAttackModifier = new int[GC.getNumUnitClassInfos()];
	stream->Read(GC.getNumUnitClassInfos(), m_aiUnitClassAttackModifier);
	SAFE_DELETE_ARRAY(m_aiUnitClassDefenseModifier);
	m_aiUnitClassDefenseModifier = new int[GC.getNumUnitClassInfos()];
	stream->Read(GC.getNumUnitClassInfos(), m_aiUnitClassDefenseModifier);
	SAFE_DELETE_ARRAY(m_aiUnitCombatModifier);
	m_aiUnitCombatModifier = new int[GC.getNumUnitCombatInfos()];
	stream->Read(GC.getNumUnitCombatInfos(), m_aiUnitCombatModifier);
	SAFE_DELETE_ARRAY(m_aiDomainModifier);
	m_aiDomainModifier = new int[NUM_DOMAIN_TYPES];
	stream->Read(NUM_DOMAIN_TYPES, m_aiDomainModifier);
	SAFE_DELETE_ARRAY(m_aiYieldModifier);
	m_aiYieldModifier = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_aiYieldModifier);
	SAFE_DELETE_ARRAY(m_aiBonusYieldChange);
	m_aiBonusYieldChange = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_aiBonusYieldChange);
	SAFE_DELETE_ARRAY(m_aiYieldChange);
	m_aiYieldChange = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_aiYieldChange);
	SAFE_DELETE_ARRAY(m_aiYieldCost);
	m_aiYieldCost = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_aiYieldCost);
	SAFE_DELETE_ARRAY(m_abUpgradeUnitClass);
	m_abUpgradeUnitClass = new bool[GC.getNumUnitClassInfos()];
	stream->Read(GC.getNumUnitClassInfos(), m_abUpgradeUnitClass);
	SAFE_DELETE_ARRAY(m_abUnitAIType);
	m_abUnitAIType = new bool[NUM_UNITAI_TYPES];
	stream->Read(NUM_UNITAI_TYPES, m_abUnitAIType);
	SAFE_DELETE_ARRAY(m_abNotUnitAIType);
	m_abNotUnitAIType = new bool[NUM_UNITAI_TYPES];
	stream->Read(NUM_UNITAI_TYPES, m_abNotUnitAIType);
	SAFE_DELETE_ARRAY(m_abBuilds);
	//m_abBuilds = new bool[GC.getNumBuildInfos()];
	//stream->Read(GC.getNumBuildInfos(), m_abBuilds);
	SAFE_DELETE_ARRAY(m_abTerrainImpassable);
	m_abTerrainImpassable = new bool[GC.getNumTerrainInfos()];
	stream->Read(GC.getNumTerrainInfos(), m_abTerrainImpassable);
	SAFE_DELETE_ARRAY(m_abFeatureImpassable);
	m_abFeatureImpassable = new bool[GC.getNumFeatureInfos()];
	stream->Read(GC.getNumFeatureInfos(), m_abFeatureImpassable);
	// < JAnimals Mod Start >
	SAFE_DELETE_ARRAY(m_abTerrainNative);
	m_abTerrainNative = new bool[GC.getNumTerrainInfos()];
	stream->Read(GC.getNumTerrainInfos(), m_abTerrainNative);
	// < JAnimals Mod End >
	SAFE_DELETE_ARRAY(m_abEvasionBuilding);
	m_abEvasionBuilding = new bool[GC.getNumBuildingClassInfos()];
	stream->Read(GC.getNumBuildingClassInfos(), m_abEvasionBuilding);
	//TAC Whaling, ray
	SAFE_DELETE_ARRAY(m_abYieldGatherable);
	m_abYieldGatherable = new bool[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_abYieldGatherable);
	//End TAC Whaling, ray
	SAFE_DELETE_ARRAY(m_abFreePromotions);
	m_abFreePromotions = new bool[GC.getNumPromotionInfos()];
	stream->Read(GC.getNumPromotionInfos(), m_abFreePromotions);
	///TK Viscos Mod
    SAFE_DELETE_ARRAY(m_abProfessionsNotAllowed);
	m_abProfessionsNotAllowed = new bool[GC.getNumProfessionInfos()];
	stream->Read(GC.getNumProfessionInfos(), m_abProfessionsNotAllowed);
	///TK end
	SAFE_DELETE_ARRAY(m_abPrereqOrBuilding);
	m_abPrereqOrBuilding = new bool[GC.getNumBuildingClassInfos()];
	stream->Read(GC.getNumBuildingClassInfos(), m_abPrereqOrBuilding);
	//stream->Read(&m_iLeaderPromotion);
	stream->Read(&m_iLeaderExperience);
	SAFE_DELETE_ARRAY(m_paszUnitNames);
	m_paszUnitNames = new CvString[m_iNumUnitNames];
	stream->ReadString(m_iNumUnitNames, m_paszUnitNames);
	stream->ReadString(m_szFormationType);
	updateArtDefineButton();
}
void CvUnitInfo::write(FDataStreamBase* stream)
{
	CvHotkeyInfo::write(stream);
	uint uiFlag=0;
	stream->Write(uiFlag);		// flag for expansion
	stream->Write(m_iAIWeight);
	/** NBMOD EDU **/
	stream->Write(m_iTeachLevel);
	/** NBMOD EDU **/

	/** NBMOD REF **/
	stream->Write(m_fNBMOD_REF_StrengthWeight);
	/** NBMOD REF **/

	stream->Write(m_iHurryCostModifier);
	stream->Write(m_iProductionWhenUsed); // WTP, ray, Construction Supplies - START
	stream->Write(m_iEuropeCost);
	stream->Write(m_iEuropeCostIncrease);
	/*** TRIANGLETRADE 10/24/08 by DPII ***/
	stream->Write(m_iAfricaCost);
	stream->Write(m_iAfricaCostIncrease);
	/**************************************/
	// R&R, ray, Port Royal
	stream->Write(m_iPortRoyalCost);
	stream->Write(m_iPortRoyalCostIncrease);
	// R&R, ray, Port Royal - END
	stream->Write(m_iImmigrationWeight);
	stream->Write(m_iImmigrationWeightDecay);
	stream->Write(m_iAdvancedStartCost);
	stream->Write(m_iAdvancedStartCostIncrease);
	stream->Write(m_iMinAreaSize);
	stream->Write(m_iMoves);
	stream->Write(m_iWorkRate);
	stream->Write(m_iWorkRateModifier);
	stream->Write(m_iGoldFromGoodiesAndChiefsModifier); // WTP, ray, Scout Gold Modifier for Goodies and Chiefs at Unit - START
	stream->Write(m_iMissionaryRateModifier);
	stream->Write(m_iNativeTradeRateModifier); // WTP, ray, Native Trade Posts - START
	stream->Write(m_iCombat);
	stream->Write(m_iXPValueAttack);
	stream->Write(m_iXPValueDefense);
	stream->Write(m_iWithdrawalProbability);
	stream->Write(m_iCityAttackModifier);
	stream->Write(m_iCityDefenseModifier);
	stream->Write(m_iHillsAttackModifier);
	stream->Write(m_iHillsDefenseModifier);
	stream->Write(m_iDomesticBonusModifier);
	stream->Write(m_iBombardRate);
	stream->Write(m_iSpecialCargo);
	stream->Write(m_iDomainCargo);
	stream->Write(m_iCargoSpace);
	stream->Write(m_iRequiredTransportSize);
	stream->Write(m_iBerthSize); // PatchMod: Berth size START
	stream->Write(m_iHarbourSpaceNeeded); // WTP, ray, new Harbour System - START
	stream->Write(m_iBarracksSpaceNeeded); // WTP, ray, new Barracks System - START
	stream->Write(m_iAssetValue);
	stream->Write(m_iPowerValue);
	stream->Write(m_eUnitClassType);
	stream->Write(m_iSpecialUnitType);
	stream->Write(m_iUnitCaptureClassType);
	stream->Write(m_iUnitCombatType);
	stream->Write(m_iDomainType);
	stream->Write(m_eDefaultProfession);
	stream->Write(m_iDefaultUnitAIType);
	stream->Write(m_iInvisibleType);
	stream->Write((int)m_aiSeeInvisibleTypes.size());
	for(int i=0;i<(int)m_aiSeeInvisibleTypes.size();i++)
	{
		stream->Write(m_aiSeeInvisibleTypes[i]);
	}
	stream->Write(m_iPrereqBuilding);
	stream->Write(m_iNumUnitNames);
	stream->Write(m_iCommandType);
	stream->Write(m_iLearnTime);
	stream->Write(m_iStudentWeight);
	stream->Write(m_iTeacherWeight);
	// < JAnimals Mod Start >
	stream->Write(m_iAnimalPatrolWeight);
	stream->Write(m_iAnimalAttackWeight);
	// < JAnimals Mod End >
	stream->Write(m_bNoBadGoodies);
	stream->Write(m_bOnlyDefensive);
	stream->Write(m_bNoCapture);
	stream->Write(m_bQuickCombat);
	stream->Write(m_bRivalTerritory);
	stream->Write(m_bMilitaryProduction);
	stream->Write(m_bFound);
	stream->Write(m_bInvisible);
	stream->Write(m_bNoDefensiveBonus);
	stream->Write(m_bCanMoveImpassable);
	stream->Write(m_bCanMoveAllTerrain);
	stream->Write(m_bFlatMovementCost);
	stream->Write(m_bIgnoreTerrainCost);
	stream->Write(m_bMechanized);
	stream->Write(m_bLineOfSight);
	stream->Write(m_bHiddenNationality);
	stream->Write(m_bSlaveShip); // WTP, ray Slave Ship
	stream->Write(m_bTreasureShip); // WTP, ray Treasure Ship
	stream->Write(m_bTroopShip); // WTP, ray Troop Ship
	stream->Write(m_bAlwaysHostile);
	stream->Write(m_bTreasure);
	stream->Write(m_bNoRevealMap); // R&R, ray, Changes for Treasures, START

	// TAC - LbD - Ray - START
	stream->Write(m_canBecomeExpert);
	stream->Write(m_canGetFree);
	stream->Write(m_canEscape);
	// TAC - LbD - Ray - END

	// WTP, ray, LbD Slaves Revolt and Free - START
	stream->Write(m_canRevolt);
	// WTP, ray, LbD Slaves Revolt and Free - END

	stream->Write(m_bCapturesCargo);
	// TAC Capturing Ships - ray
	stream->Write(m_bCapturesShips);
	stream->Write(m_iCaptureShipsChanceIncrease); // WTP, ray, Capture Ship chance increase - START
	// TAC Capturing Ships - ray - END
	stream->Write(m_bLandYieldChanges);
	stream->Write(m_bWaterYieldChanges);
	//TAC Whaling, ray
	stream->Write(m_bGatherBoat);
	//End TAC Whaling, ray

	// < JAnimals Mod Start >
	stream->Write(m_bAnimal);
	// < JAnimals Mod End >

	stream->Write((int) m_aProfessionGroups.size());
	for (int i = 0; i < (int) m_aProfessionGroups.size(); ++i)
	{
		m_aProfessionGroups[i].write(stream);
	}
	stream->Write(GC.getNumTraitInfos(), m_aiProductionTraits);
	stream->Write(GC.getNumTerrainInfos(), m_aiTerrainAttackModifier);
	stream->Write(GC.getNumTerrainInfos(), m_aiTerrainDefenseModifier);
	stream->Write(GC.getNumFeatureInfos(), m_aiFeatureAttackModifier);
	stream->Write(GC.getNumFeatureInfos(), m_aiFeatureDefenseModifier);
	stream->Write(GC.getNumUnitClassInfos(), m_aiUnitClassAttackModifier);
	stream->Write(GC.getNumUnitClassInfos(), m_aiUnitClassDefenseModifier);
	stream->Write(GC.getNumUnitCombatInfos(), m_aiUnitCombatModifier);
	stream->Write(NUM_DOMAIN_TYPES, m_aiDomainModifier);
	stream->Write(NUM_YIELD_TYPES, m_aiYieldModifier);
	stream->Write(NUM_YIELD_TYPES, m_aiBonusYieldChange);
	stream->Write(NUM_YIELD_TYPES, m_aiYieldChange);
	stream->Write(NUM_YIELD_TYPES, m_aiYieldCost);
	stream->Write(GC.getNumUnitClassInfos(), m_abUpgradeUnitClass);
	stream->Write(NUM_UNITAI_TYPES, m_abUnitAIType);
	stream->Write(NUM_UNITAI_TYPES, m_abNotUnitAIType);
//	stream->Write(GC.getNumBuildInfos(), m_abBuilds);
	stream->Write(GC.getNumTerrainInfos(), m_abTerrainImpassable);
	stream->Write(GC.getNumFeatureInfos(), m_abFeatureImpassable);
	// < JAnimals Mod Start >
	stream->Write(GC.getNumTerrainInfos(), m_abTerrainNative);
	// < JAnimals Mod End >
	stream->Write(GC.getNumBuildingClassInfos(), m_abEvasionBuilding);
	//TAC Whaling, ray
	stream->Write(NUM_YIELD_TYPES, m_abYieldGatherable);
	//End TAC Whaling, ray
	stream->Write(GC.getNumPromotionInfos(), m_abFreePromotions);
	///TK Viscos Mod
	stream->Write(GC.getNumProfessionInfos(), m_abProfessionsNotAllowed);
	///TK end
	stream->Write(GC.getNumBuildingClassInfos(), m_abPrereqOrBuilding);
	//stream->Write(m_iLeaderPromotion);
	stream->Write(m_iLeaderExperience);
	stream->WriteString(m_iNumUnitNames, m_paszUnitNames);
	stream->WriteString(m_szFormationType);
}
//
// read from xml
//
bool CvUnitInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}
	int j=0;				//loop counter
	int k=0;				//loop counter
	int iNumSibs=0;				// the number of siblings the current xml node has
	pXML->GetEnum(getType(), m_eUnitClassType, "Class");
	pXML->GetChildXmlValByName(szTextVal, "Special");
	m_iSpecialUnitType = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "Capture");
	m_iUnitCaptureClassType = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "Combat");
	m_iUnitCombatType = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "Domain");
	m_iDomainType = pXML->FindInInfoClass(szTextVal);
	pXML->GetEnum(getType(), m_eDefaultProfession, "DefaultProfession", false);
	pXML->GetChildXmlValByName(szTextVal, "DefaultUnitAI");
	m_iDefaultUnitAIType = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "Invisible");
	m_iInvisibleType = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "SeeInvisible");
	std::vector<CvString> tokens;
	szTextVal.getTokens(",", tokens);
	for(int i=0;i<(int)tokens.size();i++)
	{
		int iInvisibleType = pXML->FindInInfoClass(tokens[i]);
		if(iInvisibleType != NO_INVISIBLE)
		{
			m_aiSeeInvisibleTypes.push_back(iInvisibleType);
		}
	}
	pXML->GetChildXmlValByName(&m_iLearnTime, "iNativeLearnTime");
	/** NBMOD EDU **/
	pXML->GetChildXmlValByName(&m_iTeachLevel, "iTeachLevel");
	/** NBMOD EDU **/

	/** NBMOD REF **/
	pXML->GetChildXmlValByName(&m_fNBMOD_REF_StrengthWeight, "fNBMOD_REF_StrengthWeight");
	/** NBMOD REF **/

	pXML->GetChildXmlValByName(&m_iStudentWeight, "iStudentWeight");
	pXML->GetChildXmlValByName(&m_iTeacherWeight, "iTeacherWeight");
	// < JAnimals Mod Start >
	pXML->GetChildXmlValByName(&m_iAnimalPatrolWeight, "iAnimalPatrolWeight", -1);
	pXML->GetChildXmlValByName(&m_iAnimalAttackWeight, "iAnimalAttackWeight", -1);
	// < JAnimals Mod End >
	pXML->GetChildXmlValByName(&m_bNoBadGoodies, "bNoBadGoodies");
	pXML->GetChildXmlValByName(&m_bOnlyDefensive, "bOnlyDefensive");
	pXML->GetChildXmlValByName(&m_bNoCapture, "bNoCapture");
	pXML->GetChildXmlValByName(&m_bQuickCombat, "bQuickCombat");
	pXML->GetChildXmlValByName(&m_bRivalTerritory, "bRivalTerritory");
	pXML->GetChildXmlValByName(&m_bMilitaryProduction, "bMilitaryProduction");
	pXML->GetChildXmlValByName(&m_bFound, "bFound");
	pXML->GetChildXmlValByName(&m_bInvisible, "bInvisible");
	pXML->GetChildXmlValByName(&m_bNoDefensiveBonus, "bNoDefensiveBonus");
	pXML->GetChildXmlValByName(&m_bCanMoveImpassable, "bCanMoveImpassable");
	pXML->GetChildXmlValByName(&m_bCanMoveAllTerrain, "bCanMoveAllTerrain");
	pXML->GetChildXmlValByName(&m_bFlatMovementCost, "bFlatMovementCost");
	pXML->GetChildXmlValByName(&m_bIgnoreTerrainCost, "bIgnoreTerrainCost");
	pXML->GetChildXmlValByName(&m_bMechanized,"bMechanized",false);
	pXML->GetChildXmlValByName(&m_bLineOfSight,"bLineOfSight",false);
	pXML->GetChildXmlValByName(&m_bHiddenNationality,"bHiddenNationality",false);
	pXML->GetChildXmlValByName(&m_bSlaveShip,"bSlaveShip",false); // WTP, ray Slave Ship
	pXML->GetChildXmlValByName(&m_bTreasureShip,"bTreasureShip",false); // WTP, ray Treasure Ship
	pXML->GetChildXmlValByName(&m_bTroopShip,"bTroopShip",false); // WTP, ray Troop Ship
	pXML->GetChildXmlValByName(&m_bAlwaysHostile,"bAlwaysHostile",false);
	pXML->GetChildXmlValByName(&m_bTreasure,"bTreasure",false);
	pXML->GetEnum(getType(), m_eEthnicity, "eEthnicity", false); // WTP, ray, Ethnically correct Population Growth - START
	pXML->GetEnum(getType(), m_eCitizenStatus, "eCitizenStatus", false); // WTP, ray, Ethnically correct Population Growth - START
	// R&R, ray, Changes for Treasures, START
	pXML->GetChildXmlValByName(&m_bNoRevealMap, "bNoRevealMap", false);
	// R&R, ray, Changes for Treasures, END

	// TAC - LbD - Ray - START
	pXML->GetChildXmlValByName(&m_canBecomeExpert,"bLbDCanBecomeExpert",false);
	pXML->GetChildXmlValByName(&m_canGetFree,"bLbDCanGetFree",false);
	pXML->GetChildXmlValByName(&m_canEscape,"bLbDCanEscape",false);
	// TAC - LbD - Ray - END

	// WTP, ray, LbD Slaves Revolt and Free - START
	pXML->GetChildXmlValByName(&m_canRevolt,"bLbDCanRevolt",false);
	// WTP, ray, LbD Slaves Revolt and Free - END

	pXML->GetChildXmlValByName(&m_bCapturesCargo,"bCapturesCargo",false);
	// TAC Capturing Ships - ray
	pXML->GetChildXmlValByName(&m_bCapturesShips,"bCapturesShips",false);
	pXML->GetChildXmlValByName(&m_iCaptureShipsChanceIncrease,"iCaptureShipsChanceIncrease",false); // WTP, ray, Capture Ship chance increase - START
	// TAC Capturing Ships - ray - END
	pXML->GetChildXmlValByName(&m_bLandYieldChanges,"bLandYieldChanges",true);
	pXML->GetChildXmlValByName(&m_bWaterYieldChanges,"bWaterYieldChanges",true);
	//TAC Whaling, ray
	pXML->GetChildXmlValByName(&m_bGatherBoat,"bGatherBoat",false);
	//End TAC Whaling, ray
	// < JAnimals Mod Start >
	pXML->GetChildXmlValByName(&m_bAnimal, "bAnimal", false);
	// < JAnimals Mod End >
	/// Move Into Peak - start - Nightinggale
	pXML->GetChildXmlValByName(&m_bMoveIntoPeak, "bMoveIntoPeak");
	/// Move Into Peak - end - Nightinggale
	pXML->SetVariableListTagPair(&m_abUpgradeUnitClass, "UnitClassUpgrades", GC.getNumUnitClassInfos(), false);
	pXML->SetVariableListTagPair(&m_abUnitAIType, "UnitAIs", NUM_UNITAI_TYPES, false);
	pXML->SetVariableListTagPair(&m_abNotUnitAIType, "NotUnitAIs", NUM_UNITAI_TYPES, false);
	pXML->SetVariableListTagPair(&m_abBuilds, "Builds", NUM_BUILD_TYPES, false);
	pXML->GetChildXmlValByName(szTextVal, "PrereqBuilding");
	m_iPrereqBuilding = pXML->FindInInfoClass(szTextVal);
	pXML->SetVariableListTagPair(&m_abPrereqOrBuilding, "PrereqOrBuildings", GC.getNumBuildingClassInfos(), false);
	pXML->SetVariableListTagPair(&m_aiProductionTraits, "ProductionTraits", GC.getNumTraitInfos(), 0);
	pXML->GetChildXmlValByName(&m_iAIWeight, "iAIWeight");
	pXML->GetChildXmlValByName(&m_iHurryCostModifier, "iHurryCostModifier");
	pXML->GetChildXmlValByName(&m_iProductionWhenUsed, "iProductionWhenUsed"); // WTP, ray, Construction Supplies - START
	pXML->GetChildXmlValByName(&m_iAdvancedStartCost, "iAdvancedStartCost");
	pXML->GetChildXmlValByName(&m_iAdvancedStartCostIncrease, "iAdvancedStartCostIncrease");
	pXML->GetChildXmlValByName(&m_iEuropeCost, "iEuropeCost");
	pXML->GetChildXmlValByName(&m_iEuropeCostIncrease, "iEuropeCostIncrease");
	/*** TRIANGLETRADE 10/24/08 by DPII ***/
	pXML->GetChildXmlValByName(&m_iAfricaCost, "iAfricaCost", -1);
	pXML->GetChildXmlValByName(&m_iAfricaCostIncrease, "iAfricaCostIncrease", -1);
	/**************************************/
	// R&R, ray, Port Royal
	pXML->GetChildXmlValByName(&m_iPortRoyalCost, "iPortRoyalCost", -1);
	pXML->GetChildXmlValByName(&m_iPortRoyalCostIncrease, "iPortRoyalCostIncrease", -1);
	// R&R, ray, Port Royal - END
	pXML->GetChildXmlValByName(&m_iImmigrationWeight, "iImmigrationWeight");
	pXML->GetChildXmlValByName(&m_iImmigrationWeightDecay, "iImmigrationWeightDecay");
	pXML->GetChildXmlValByName(&m_iMinAreaSize, "iMinAreaSize");
	pXML->GetChildXmlValByName(&m_iMoves, "iMoves");
	pXML->GetChildXmlValByName(&m_iWorkRate, "iWorkRate");
	pXML->GetChildXmlValByName(&m_iWorkRateModifier, "iWorkRateModifier");
	pXML->GetChildXmlValByName(&m_iGoldFromGoodiesAndChiefsModifier, "iGoldFromGoodiesAndChiefsModifier"); // WTP, ray, Scout Gold Modifier for Goodies and Chiefs at Unit - START
	pXML->GetChildXmlValByName(&m_iMissionaryRateModifier, "iMissionaryRateModifier");
	pXML->GetChildXmlValByName(&m_iNativeTradeRateModifier, "iNativeTradeRateModifier"); // WTP, ray, Native Trade Posts - START
	pXML->SetVariableListTagPair(&m_abTerrainImpassable, "TerrainImpassables", GC.getNumTerrainInfos(), false);
	pXML->SetVariableListTagPair(&m_abFeatureImpassable, "FeatureImpassables", GC.getNumFeatureInfos(), false);
	// < JAnimals Mod Start >
	pXML->SetVariableListTagPair(&m_abTerrainNative, "TerrainNatives", GC.getNumTerrainInfos(), false);
	// < JAnimals Mod End >
	pXML->SetVariableListTagPair(&m_abEvasionBuilding, "EvasionBuildings", GC.getNumBuildingClassInfos(), false);
	//TAC Whaling, ray
	pXML->SetVariableListTagPair(&m_abYieldGatherable, "YieldsGatherable", NUM_YIELD_TYPES, false);
	//End TAC Whaling, ray
	pXML->GetChildXmlValByName(&m_iCombat, "iCombat");
	pXML->GetChildXmlValByName(&m_iXPValueAttack, "iXPValueAttack");
	pXML->GetChildXmlValByName(&m_iXPValueDefense, "iXPValueDefense");
	pXML->GetChildXmlValByName(&m_iWithdrawalProbability, "iWithdrawalProb");
	pXML->GetChildXmlValByName(&m_iCityAttackModifier, "iCityAttack");
	pXML->GetChildXmlValByName(&m_iCityDefenseModifier, "iCityDefense");
	pXML->GetChildXmlValByName(&m_iHillsAttackModifier, "iHillsAttack");
	pXML->GetChildXmlValByName(&m_iHillsDefenseModifier, "iHillsDefense");
	pXML->GetChildXmlValByName(&m_iDomesticBonusModifier, "iDomesticBonus");
	pXML->SetVariableListTagPair(&m_aiTerrainAttackModifier, "TerrainAttacks", GC.getNumTerrainInfos(), 0);
	pXML->SetVariableListTagPair(&m_aiTerrainDefenseModifier, "TerrainDefenses", GC.getNumTerrainInfos(), 0);
	pXML->SetVariableListTagPair(&m_aiFeatureAttackModifier, "FeatureAttacks", GC.getNumFeatureInfos(), 0);
	pXML->SetVariableListTagPair(&m_aiFeatureDefenseModifier, "FeatureDefenses", GC.getNumFeatureInfos(), 0);
	pXML->SetVariableListTagPair(&m_aiUnitClassAttackModifier, "UnitClassAttackMods", GC.getNumUnitClassInfos(), 0);
	pXML->SetVariableListTagPair(&m_aiUnitClassDefenseModifier, "UnitClassDefenseMods", GC.getNumUnitClassInfos(), 0);
	pXML->SetVariableListTagPair(&m_aiUnitCombatModifier, "UnitCombatMods", GC.getNumUnitCombatInfos(), 0);
	pXML->SetVariableListTagPair(&m_aiDomainModifier, "DomainMods", NUM_DOMAIN_TYPES, 0);
	pXML->SetVariableListTagPair(&m_aiYieldModifier, "YieldModifiers", NUM_YIELD_TYPES, 0);
	// R&R, Androrc, Domestic Market
	readXML(m_info_YieldDemands, "YieldDemands");
	//Androrc End
	pXML->SetVariableListTagPair(&m_aiBonusYieldChange, "BonusYieldChanges", NUM_YIELD_TYPES, 0);
	pXML->SetVariableListTagPair(&m_aiYieldChange, "YieldChanges", NUM_YIELD_TYPES, 0);
	pXML->SetVariableListTagPair(&m_aiYieldCost, "YieldCosts", NUM_YIELD_TYPES, 0);
	pXML->GetChildXmlValByName(&m_iBombardRate, "iBombardRate");
	pXML->GetChildXmlValByName(szTextVal, "SpecialCargo");
	m_iSpecialCargo = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "DomainCargo");
	m_iDomainCargo = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(&m_iCargoSpace, "iCargo");
	pXML->GetChildXmlValByName(&m_iRequiredTransportSize, "iRequiredTransportSize");
	pXML->GetChildXmlValByName(&m_iBerthSize, "iBerthSize"); // PatchMod: Berth size START
	pXML->GetChildXmlValByName(&m_iHarbourSpaceNeeded, "iHarbourSpaceNeeded"); // WTP, ray, new Harbour System - START
	pXML->GetChildXmlValByName(&m_iBarracksSpaceNeeded, "iBarracksSpaceNeeded"); // WTP, ray, new Barracks System - START
	pXML->GetChildXmlValByName(&m_iAssetValue, "iAsset");
	pXML->GetChildXmlValByName(&m_iPowerValue, "iPower");
	// Read the mesh groups elements
	if ( gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"ProfessionMeshGroups") )
	{
		if ( gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"UnitMeshGroups") )
		{
			do
			{
				pXML->GetChildXmlValByName(szTextVal, "ProfessionType");
				ProfessionTypes eProfession = (ProfessionTypes) pXML->FindInInfoClass(szTextVal);
				int iMeleeWaveSize;
                pXML->GetChildXmlValByName( &iMeleeWaveSize, "iMeleeWaveSize" );
				int iRangedWaveSize;
				pXML->GetChildXmlValByName( &iRangedWaveSize, "iRangedWaveSize" );
				float fMaxSpeed;
				pXML->GetChildXmlValByName( &fMaxSpeed, "fMaxSpeed");
				float fPadTime;
				pXML->GetChildXmlValByName( &fPadTime, "fPadTime");
				CvUnitMeshGroups kMeshGroups(eProfession, iMeleeWaveSize, iRangedWaveSize, fMaxSpeed, fPadTime);
				if(gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "UnitMeshGroup"))
				{
					do
					{
						int iNumRequired;
						pXML->GetChildXmlValByName( &iNumRequired, "iRequired");
						pXML->GetChildXmlValByName( szTextVal, "ArtDefineTag");
						CvUnitMeshGroup kMeshGroup(iNumRequired, szTextVal);
						kMeshGroups.addMeshGroup(kMeshGroup);
					} while(gDLL->getXMLIFace()->NextSibling(pXML->GetXML()));
					gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
				}
				m_aProfessionGroups.push_back(kMeshGroups);
			} while(gDLL->getXMLIFace()->NextSibling(pXML->GetXML()));
			gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	pXML->GetChildXmlValByName(m_szFormationType, "FormationType");
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"UniqueNames"))
	{
		pXML->SetStringList(&m_paszUnitNames, &m_iNumUnitNames);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	pXML->SetVariableListTagPair(&m_abFreePromotions, "FreePromotions", GC.getNumPromotionInfos(), false);
	///TK Viscos Mod
	pXML->SetVariableListTagPair(&m_abProfessionsNotAllowed, "ProfessionsNotAllowed", GC.getNumProfessionInfos(), false);
	///TK end
	pXML->GetEnum(getType(), m_eLeaderPromotion, "LeaderPromotion", false);
	pXML->GetChildXmlValByName(&m_iLeaderExperience, "iLeaderExperience");
	updateArtDefineButton();


#ifdef FASSERT_ENABLE
	// test if xml data is valid
	for (int i = 0; i < m_info_YieldDemands.getLength(); ++i)
	{
		FAssertMsg(m_info_YieldDemands.get1(i) > 0, CvString::format("%s: %s demanded at 0 amount (index %d)", getType(), getTypeStr(m_info_YieldDemands.get0(i)), i).c_str());
	}

#endif // FASSERT_ENABLE



	return true;
}

int CvUnitInfo::PYgetYieldCost(int i) const
{
	return getYieldCost(static_cast<YieldTypes>(i));
}


//======================================================================================================
//					CvUnitFormationInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvUnitFormationInfo()
//
//  \brief		Default Constructor
//
//------------------------------------------------------------------------------------------------------
CvUnitFormationInfo::CvUnitFormationInfo()
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvUnitFormationInfo()
//
//  \brief		Destructor
//
//------------------------------------------------------------------------------------------------------
CvUnitFormationInfo::~CvUnitFormationInfo()
{
}
const char* CvUnitFormationInfo::getFormationType() const
{
	return m_szFormationType;
}
const std::vector<EntityEventTypes> & CvUnitFormationInfo::getEventTypes() const
{
	return m_vctEventTypes;
}
int CvUnitFormationInfo::getNumUnitEntries() const
{
	return m_vctUnitEntries.size();
}
const CvUnitEntry &CvUnitFormationInfo::getUnitEntry(int index) const
{
	return m_vctUnitEntries[index];
}
void CvUnitFormationInfo::addUnitEntry(const CvUnitEntry &unitEntry)
{
	m_vctUnitEntries.push_back(unitEntry);
}
int CvUnitFormationInfo::getNumGreatUnitEntries() const
{
	return m_vctGreatUnitEntries.size();
}
const CvUnitEntry &CvUnitFormationInfo::getGreatUnitEntry(int index) const
{
	return m_vctGreatUnitEntries[index];
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvUnitFormationInfo()
//
//  \brief		Reads from XML
//
//------------------------------------------------------------------------------------------------------
bool CvUnitFormationInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	int iIndex;
	bool bNextSibling;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(m_szFormationType, "FormationType");
	if ( gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "EventMaskList" ))
	{
		if ( gDLL->getXMLIFace()->SetToChild( pXML->GetXML() ) )
		{
			pXML->GetXmlVal( szTextVal );
			do
			{
				iIndex = pXML->FindInInfoClass(szTextVal);
				if ( iIndex != -1 )
					m_vctEventTypes.push_back( (EntityEventTypes)iIndex );
				bNextSibling = pXML->GetNextXmlVal( &szTextVal );
			}
			while( bNextSibling );
			gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	// Read the entries
	if ( gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "UnitEntry" ) )
	{
		do
		{
			CvUnitEntry unitEntry;
			pXML->GetChildXmlValByName(szTextVal, "UnitEntryType");
			if ( gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "Position" ) )
			{
				pXML->GetChildXmlValByName( &unitEntry.m_position.x, "x");
				pXML->GetChildXmlValByName( &unitEntry.m_position.y, "y");
				gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
			}
			pXML->GetChildXmlValByName( &unitEntry.m_fRadius, "PositionRadius");
			pXML->GetChildXmlValByName( &unitEntry.m_fFacingDirection, "Direction");
			pXML->GetChildXmlValByName( &unitEntry.m_fFacingVariance, "DirVariation");
			if(szTextVal.CompareNoCase("Unit") == 0)
				m_vctUnitEntries.push_back(unitEntry);
			else if(szTextVal.CompareNoCase("General") == 0)
				m_vctGreatUnitEntries.push_back(unitEntry);
			else
			{
				FAssertMsg(false, "[Jason] Unknown unit formation entry type.");
			}
		}
		while ( gDLL->getXMLIFace()->LocateNextSiblingNodeByTagName(pXML->GetXML(), "UnitEntry"));
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	FAssertMsg(m_vctGreatUnitEntries.size() > 0, "[Jason] Formation missing great general entry.");
	return true;
}

//======================================================================================================
//					CvSpecialUnitInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvSpecialUnitInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvSpecialUnitInfo::CvSpecialUnitInfo() :
m_bValid(false),
m_bCityLoad(false),
m_abCarrierUnitAITypes(NULL),
m_aiProductionTraits(NULL)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvSpecialUnitInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvSpecialUnitInfo::~CvSpecialUnitInfo()
{
	SAFE_DELETE_ARRAY(m_abCarrierUnitAITypes);
	SAFE_DELETE_ARRAY(m_aiProductionTraits);
}
bool CvSpecialUnitInfo::isValid() const
{
	return m_bValid;
}
bool CvSpecialUnitInfo::isCityLoad() const
{
	return m_bCityLoad;
}
// Arrays
bool CvSpecialUnitInfo::isCarrierUnitAIType(int i) const
{
	FAssertMsg(i < NUM_UNITAI_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abCarrierUnitAITypes ? m_abCarrierUnitAITypes[i] : -1;
}
int CvSpecialUnitInfo::getProductionTraits(int i) const
{
	FAssertMsg(i < GC.getNumTraitInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiProductionTraits ? m_aiProductionTraits[i] : -1;
}
bool CvSpecialUnitInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(&m_bValid, "bValid");
	pXML->GetChildXmlValByName(&m_bCityLoad, "bCityLoad");
	pXML->SetVariableListTagPair(&m_abCarrierUnitAITypes, "CarrierUnitAITypes", NUM_UNITAI_TYPES, false);
	pXML->SetVariableListTagPair(&m_aiProductionTraits, "ProductionTraits", GC.getNumTraitInfos(), 0);
	return true;
}
//======================================================================================================
//					CvCivicInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvCivicInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvCivicInfo::CvCivicInfo() :
m_iCivicOptionType(NO_CIVICOPTION),
m_eCivEffect(NO_CIV_EFFECT),
m_iAIWeight(0),
m_iGreatGeneralRateModifier(0),
m_iDomesticGreatGeneralRateModifier(0),
m_iFreeExperience(0),
m_iWorkerSpeedModifier(0),
m_iImprovementUpgradeRateModifier(0),
m_iMilitaryProductionModifier(0),
m_iExpInBorderModifier(0),
m_iImmigrationConversion(YIELD_CROSSES),
m_iNativeAttitudeChange(0),
m_iNativeCombatModifier(0),
m_iFatherPointModifier(0),
m_bDominateNativeBorders(false),
m_bRevolutionEuropeTrade(false),
m_aiYieldModifier(NULL),
m_aiCapitalYieldModifier(NULL),
m_aiProfessionCombatChange(NULL),
m_pabHurry(NULL),
m_pabSpecialBuildingNotRequired(NULL)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvCivicInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvCivicInfo::~CvCivicInfo()
{
	SAFE_DELETE_ARRAY(m_aiYieldModifier);
	SAFE_DELETE_ARRAY(m_aiCapitalYieldModifier);
	SAFE_DELETE_ARRAY(m_aiProfessionCombatChange);
	SAFE_DELETE_ARRAY(m_pabHurry);
	SAFE_DELETE_ARRAY(m_pabSpecialBuildingNotRequired);
	for (uint iI=0;iI<m_aaiImprovementYieldChanges.size();iI++)
	{
		SAFE_DELETE_ARRAY(m_aaiImprovementYieldChanges[iI]);
	}
}
int CvCivicInfo::getCivicOptionType() const
{
	return m_iCivicOptionType;
}
int CvCivicInfo::getAIWeight() const
{
	return m_iAIWeight;
}
int CvCivicInfo::getGreatGeneralRateModifier() const
{
	return m_iGreatGeneralRateModifier;
}
int CvCivicInfo::getDomesticGreatGeneralRateModifier() const
{
	return m_iDomesticGreatGeneralRateModifier;
}
int CvCivicInfo::getFreeExperience() const
{
	return m_iFreeExperience;
}
int CvCivicInfo::getWorkerSpeedModifier() const
{
	return m_iWorkerSpeedModifier;
}
int CvCivicInfo::getImprovementUpgradeRateModifier() const
{
	return m_iImprovementUpgradeRateModifier;
}
int CvCivicInfo::getMilitaryProductionModifier() const
{
	return m_iMilitaryProductionModifier;
}
int CvCivicInfo::getExpInBorderModifier() const
{
	return m_iExpInBorderModifier;
}
int CvCivicInfo::getImmigrationConversion() const
{
	return m_iImmigrationConversion;
}
int CvCivicInfo::getNativeAttitudeChange() const
{
	return m_iNativeAttitudeChange;
}

int CvCivicInfo::getNativeCombatModifier() const
{
	return m_iNativeCombatModifier;
}
int CvCivicInfo::getFatherPointModifier() const
{
	return m_iFatherPointModifier;
}
bool CvCivicInfo::isDominateNativeBorders() const
{
	return m_bDominateNativeBorders;
}
bool CvCivicInfo::isRevolutionEuropeTrade() const
{
	return m_bRevolutionEuropeTrade;
}
// Arrays
int CvCivicInfo::getYieldModifier(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiYieldModifier ? m_aiYieldModifier[i] : -1;
}
int* CvCivicInfo::getYieldModifierArray() const
{
	return m_aiYieldModifier;
}
int CvCivicInfo::getCapitalYieldModifier(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiCapitalYieldModifier ? m_aiCapitalYieldModifier[i] : -1;
}
int CvCivicInfo::getProfessionCombatChange(int i) const
{
	FAssertMsg(i < GC.getNumProfessionInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiProfessionCombatChange ? m_aiProfessionCombatChange[i] : -1;
}
int* CvCivicInfo::getCapitalYieldModifierArray() const
{
	return m_aiCapitalYieldModifier;
}
bool CvCivicInfo::isHurry(int i) const
{
	FAssertMsg(i < GC.getNumHurryInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pabHurry ? m_pabHurry[i] : false;
}
bool CvCivicInfo::isSpecialBuildingNotRequired(int i) const
{
	FAssertMsg(i < GC.getNumSpecialBuildingInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pabSpecialBuildingNotRequired ? m_pabSpecialBuildingNotRequired[i] : false;
}
int CvCivicInfo::getImprovementYieldChanges(int i, int j) const
{
	FAssertMsg(i < GC.getNumImprovementInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return m_aaiImprovementYieldChanges[i][j];
}
int CvCivicInfo::getNumFreeUnitClasses() const
{
	return m_aFreeUnitClasses.size();
}
int CvCivicInfo::getFreeUnitClass(int i) const
{
	FAssert(i >= 0 && i < getNumFreeUnitClasses());
	return m_aFreeUnitClasses[i];
}
void CvCivicInfo::read(FDataStreamBase* stream)
{
	CvInfoBase::read(stream);
	uint uiFlag=0;
	stream->Read(&uiFlag);		// flag for expansion
	stream->Read(&m_iCivicOptionType);
	stream->Read(&m_iAIWeight);
	stream->Read(&m_iGreatGeneralRateModifier);
	stream->Read(&m_iDomesticGreatGeneralRateModifier);
	stream->Read(&m_iFreeExperience);
	stream->Read(&m_iWorkerSpeedModifier);
	stream->Read(&m_iImprovementUpgradeRateModifier);
	stream->Read(&m_iMilitaryProductionModifier);
	stream->Read(&m_iExpInBorderModifier);
	stream->Read(&m_iImmigrationConversion);
	stream->Read(&m_iNativeAttitudeChange);
	stream->Read(&m_iNativeCombatModifier);
	stream->Read(&m_iFatherPointModifier);
	stream->Read(&m_bDominateNativeBorders);
	stream->Read(&m_bRevolutionEuropeTrade);
	// Arrays
	SAFE_DELETE_ARRAY(m_aiYieldModifier);
	m_aiYieldModifier = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_aiYieldModifier);
	SAFE_DELETE_ARRAY(m_aiCapitalYieldModifier);
	m_aiCapitalYieldModifier = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_aiCapitalYieldModifier);
	SAFE_DELETE_ARRAY(m_aiProfessionCombatChange);
	m_aiProfessionCombatChange = new int[GC.getNumProfessionInfos()];
	stream->Read(GC.getNumProfessionInfos(), m_aiProfessionCombatChange);
	SAFE_DELETE_ARRAY(m_pabHurry);
	m_pabHurry = new bool[GC.getNumHurryInfos()];
	stream->Read(GC.getNumHurryInfos(), m_pabHurry);
	SAFE_DELETE_ARRAY(m_pabSpecialBuildingNotRequired);
	m_pabSpecialBuildingNotRequired = new bool[GC.getNumSpecialBuildingInfos()];
	stream->Read(GC.getNumSpecialBuildingInfos(), m_pabSpecialBuildingNotRequired);
	for (uint iI=0;iI<m_aaiImprovementYieldChanges.size();iI++)
		{
		SAFE_DELETE_ARRAY(m_aaiImprovementYieldChanges[iI]);
		}
	m_aaiImprovementYieldChanges.clear();
	for(int i=0;i<GC.getNumImprovementInfos();i++)
	{
		m_aaiImprovementYieldChanges.push_back(new int[NUM_YIELD_TYPES]);
		stream->Read(NUM_YIELD_TYPES, m_aaiImprovementYieldChanges[i]);
	}

	int iNumFreeUnitClasses;
	stream->Read(&iNumFreeUnitClasses);
	if (iNumFreeUnitClasses > 0)
	{
		m_aFreeUnitClasses.resize(iNumFreeUnitClasses);
		stream->Read(iNumFreeUnitClasses, &m_aFreeUnitClasses[0]);
	}
}
void CvCivicInfo::write(FDataStreamBase* stream)
{
	CvInfoBase::write(stream);
	uint uiFlag=0;
	stream->Write(uiFlag);		// flag for expansion
	stream->Write(m_iCivicOptionType);
	stream->Write(m_iAIWeight);
	stream->Write(m_iGreatGeneralRateModifier);
	stream->Write(m_iDomesticGreatGeneralRateModifier);
	stream->Write(m_iFreeExperience);
	stream->Write(m_iWorkerSpeedModifier);
	stream->Write(m_iImprovementUpgradeRateModifier);
	stream->Write(m_iMilitaryProductionModifier);
	stream->Write(m_iExpInBorderModifier);
	stream->Write(m_iImmigrationConversion);
	stream->Write(m_iNativeAttitudeChange);
	stream->Write(m_iNativeCombatModifier);
	stream->Write(m_iFatherPointModifier);
	stream->Write(m_bDominateNativeBorders);
	stream->Write(m_bRevolutionEuropeTrade);
	// Arrays
	stream->Write(NUM_YIELD_TYPES, m_aiYieldModifier);
	stream->Write(NUM_YIELD_TYPES, m_aiCapitalYieldModifier);
	stream->Write(GC.getNumProfessionInfos(), m_aiProfessionCombatChange);
	stream->Write(GC.getNumHurryInfos(), m_pabHurry);
	stream->Write(GC.getNumSpecialBuildingInfos(), m_pabSpecialBuildingNotRequired);
	for(int i=0;i<GC.getNumImprovementInfos();i++)
	{
		stream->Write(NUM_YIELD_TYPES, m_aaiImprovementYieldChanges[i]);
	}

	stream->Write(getNumFreeUnitClasses());
	if (getNumFreeUnitClasses() > 0)
	{
		stream->Write(getNumFreeUnitClasses(), &m_aFreeUnitClasses[0]);
	}
}
bool CvCivicInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(szTextVal, "CivicOptionType");
	m_iCivicOptionType = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(&m_iAIWeight, "iAIWeight");
	pXML->GetEnum(getType(), m_eCivEffect, "eCivEffect", false);
	pXML->GetChildXmlValByName(&m_iGreatGeneralRateModifier, "iGreatGeneralRateModifier");
	pXML->GetChildXmlValByName(&m_iDomesticGreatGeneralRateModifier, "iDomesticGreatGeneralRateModifier");
	pXML->GetChildXmlValByName(&m_iFreeExperience, "iFreeExperience");
	pXML->GetChildXmlValByName(&m_iWorkerSpeedModifier, "iWorkerSpeedModifier");
	pXML->GetChildXmlValByName(&m_iImprovementUpgradeRateModifier, "iImprovementUpgradeRateModifier");
	pXML->GetChildXmlValByName(&m_iMilitaryProductionModifier, "iMilitaryProductionModifier");
	pXML->GetChildXmlValByName(&m_iExpInBorderModifier, "iExpInBorderModifier");
	pXML->GetChildXmlValByName(szTextVal, "ImmigrationConversion");
	m_iImmigrationConversion = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(&m_iNativeAttitudeChange, "iNativeAttitudeChange");
	pXML->GetChildXmlValByName(&m_iNativeCombatModifier, "iNativeCombatModifier");
	pXML->GetChildXmlValByName(&m_iFatherPointModifier, "iFatherPointModifier");
	pXML->GetChildXmlValByName(&m_bDominateNativeBorders, "bDominateNativeBorders");
	pXML->GetChildXmlValByName(&m_bRevolutionEuropeTrade, "bRevolutionEuropeTrade");
	pXML->SetVariableListTagPair(&m_aiYieldModifier, "YieldModifiers", NUM_YIELD_TYPES, 0);
	pXML->SetVariableListTagPair(&m_aiCapitalYieldModifier, "CapitalYieldModifiers", NUM_YIELD_TYPES, 0);
	pXML->SetVariableListTagPair(&m_aiProfessionCombatChange, "ProfessionCombatChanges", GC.getNumProfessionInfos(), 0);
	pXML->SetVariableListTagPair(&m_pabHurry, "Hurrys", GC.getNumHurryInfos(), false);
	pXML->SetVariableListTagPair(&m_pabSpecialBuildingNotRequired, "SpecialBuildingNotRequireds", GC.getNumSpecialBuildingInfos(), false);
	// initialize the boolean list to the correct size and all the booleans to false
	FAssertMsg((GC.getNumImprovementInfos() > 0) && (NUM_YIELD_TYPES) > 0,"either the number of improvement infos is zero or less or the number of yield types is zero or less");
	pXML->Init2DIntList(m_aaiImprovementYieldChanges, GC.getNumImprovementInfos(), NUM_YIELD_TYPES);
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"ImprovementYieldChanges"))
	{
		if (pXML->SkipToNextVal())
		{
			int iNumSibs = gDLL->getXMLIFace()->GetNumChildren(pXML->GetXML());
			if (gDLL->getXMLIFace()->SetToChild(pXML->GetXML()))
			{
				if (0 < iNumSibs)
				{
					for (int j=0;j<iNumSibs;j++)
					{
						pXML->GetChildXmlValByName(szTextVal, "ImprovementType");
						int iIndex = pXML->FindInInfoClass(szTextVal);
						if (iIndex > -1)
						{
							// delete the array since it will be reallocated
							SAFE_DELETE_ARRAY(m_aaiImprovementYieldChanges[iIndex]);
							pXML->SetVariableListTagPair(&m_aaiImprovementYieldChanges[iIndex], "ImprovementYields", NUM_YIELD_TYPES, 0);
						}
						if (!gDLL->getXMLIFace()->NextSibling(pXML->GetXML()))
						{
							break;
						}
					}
				}
				gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
			}
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}

	int* aFreeUnitClasses = NULL;
	pXML->SetVariableListTagPair(&aFreeUnitClasses, "FreeUnitClasses", GC.getNumUnitClassInfos(), 0);
	for (int iUnitClass = 0; iUnitClass < GC.getNumUnitClassInfos(); ++iUnitClass)
	{
		for (int iFree = 0; iFree < aFreeUnitClasses[iUnitClass]; ++iFree)
		{
			m_aFreeUnitClasses.push_back((UnitClassTypes) iUnitClass);
		}
	}
	SAFE_DELETE_ARRAY(aFreeUnitClasses);

	return true;
}
//======================================================================================================
//					CvDiplomacyInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvDiplomacyInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvDiplomacyInfo::CvDiplomacyInfo()
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvDiplomacyInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvDiplomacyInfo::~CvDiplomacyInfo()
{
	uninit();
}
// note - Response member vars allocated by CvXmlLoadUtility
void CvDiplomacyInfo::uninit()
{
	for (std::vector<CvDiplomacyResponse*>::iterator it = m_pResponses.begin(); it != m_pResponses.end(); ++it)
	{
		SAFE_DELETE(*it);
	}
	m_pResponses.clear();
}
const CvDiplomacyResponse& CvDiplomacyInfo::getResponse(int iNum) const
{
	return *(m_pResponses[iNum]);
}
int CvDiplomacyInfo::getNumResponses() const
{
	return m_pResponses.size();
}
bool CvDiplomacyInfo::getCivilizationTypes(int i, int j) const
{
	FAssertMsg(i < getNumResponses(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < GC.getNumCivilizationInfos(), "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return m_pResponses[i]->getCivilizationTypes(j);
}
bool CvDiplomacyInfo::getLeaderHeadTypes(int i, int j) const
{
	FAssertMsg(i < getNumResponses(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < GC.getNumLeaderHeadInfos(), "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return m_pResponses[i]->getLeaderHeadTypes(j);
}
bool CvDiplomacyInfo::getAttitudeTypes(int i, int j) const
{
	FAssertMsg(i < getNumResponses(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < NUM_ATTITUDE_TYPES, "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return m_pResponses[i]->getAttitudeTypes(j);
}
bool CvDiplomacyInfo::getDiplomacyPowerTypes(int i, int j) const
{
	FAssertMsg(i < getNumResponses(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < NUM_DIPLOMACYPOWER_TYPES, "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return m_pResponses[i]->getDiplomacyPowerTypes(j);
}
int CvDiplomacyInfo::getNumDiplomacyText(int i) const
{
	FAssertMsg(i < getNumResponses(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pResponses[i]->getNumDiplomacyText();
}
const char* CvDiplomacyInfo::getDiplomacyText(int i, int j) const
{
	FAssertMsg(i < getNumResponses(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < getNumDiplomacyText(i), "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return m_pResponses[i]->getDiplomacyText(j);
}
void CvDiplomacyInfo::read(FDataStreamBase* stream)
{
	CvInfoBase::read(stream);
	uint uiFlag=0;
	stream->Read(&uiFlag);		// flag for expansion
	int iNumResponses;
	stream->Read(&iNumResponses);
	// Arrays
	uninit();
	for (int uiIndex = 0; uiIndex < iNumResponses; ++uiIndex)
	{
		CvDiplomacyResponse* pResponse = new CvDiplomacyResponse;
		pResponse->read(stream);
		m_pResponses.push_back(pResponse);
	}
}
void CvDiplomacyInfo::write(FDataStreamBase* stream)
{
	CvInfoBase::write(stream);
	uint uiFlag=0;
	stream->Write(uiFlag);		// flag for expansion
	int iNumResponses = m_pResponses.size();
	stream->Write(iNumResponses);
	// Arrays
	for (int uiIndex = 0; uiIndex < iNumResponses; ++uiIndex)
	{
		m_pResponses[uiIndex]->write(stream);
	}
}
bool CvDiplomacyInfo::read(CvXMLLoadUtility* pXML)
{
	int i;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	uninit();
	if ( gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"Responses") )
	{
		int iNewResponses = gDLL->getXMLIFace()->NumOfChildrenByTagName(pXML->GetXML(), "Response");
		gDLL->getXMLIFace()->SetToChild(pXML->GetXML());
		for (i = 0; i < iNewResponses; i++)
		{
			CvDiplomacyResponse* pNewResponse = new CvDiplomacyResponse;
			pNewResponse->read(pXML);
			m_pResponses.push_back(pNewResponse);
			if (!gDLL->getXMLIFace()->NextSibling(pXML->GetXML()))
			{
				break;
			}
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	return true;
}
//======================================================================================================
//					CvUnitClassInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvUnitClassInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvUnitClassInfo::CvUnitClassInfo() :
m_iDefaultUnitIndex(NO_UNIT)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvUnitClassInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvUnitClassInfo::~CvUnitClassInfo()
{
}
int CvUnitClassInfo::getDefaultUnitIndex() const
{
	return m_iDefaultUnitIndex;
}
void CvUnitClassInfo::setDefaultUnitIndex(int i)
{
	m_iDefaultUnitIndex = i;
}
bool CvUnitClassInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	CvString szTextVal;
	pXML->GetChildXmlValByName(szTextVal, "DefaultUnit");
	m_aszExtraXMLforPass3.push_back(szTextVal);
	return true;
}
bool CvUnitClassInfo::readPass3()
{
	if (m_aszExtraXMLforPass3.size() < 1)
	{
		FAssert(false);
		return false;
	}
	m_iDefaultUnitIndex = GC.getInfoTypeForString(m_aszExtraXMLforPass3[0]);
	m_aszExtraXMLforPass3.clear();
	return true;
}

//======================================================================================================
//					CvBuildingInfo
//======================================================================================================

/** NBMOD EDU **/

/*********************************************************************/
/**                                                                 **/
/** int CvBuildingInfo::NBMOD_GetTeachLevel() const                 **/
/** Ermittelt die Bildungsstufe, die das Geb�ude bereit stellt.     **/
/**                                                                 **/
/*********************************************************************/

int CvBuildingInfo::NBMOD_GetTeachLevel() const
{
    return m_iTeachLevel;
}

/** NBMOD EDU **/
// TAC - AI Buildings - koma13 - START
int CvBuildingInfo::getAICitySize() const
{
    return m_iAICitySize;
}
// TAC - AI Buildings - koma13 - END
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvBuildingInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvBuildingInfo::CvBuildingInfo() :
/** NBMOD EDU **/
m_iTeachLevel(0),
/** NBMOD EDU **/
// TAC - AI Buildings - koma13 - START
m_iAICitySize(0),
// TAC - AI Buildings - koma13 - END
m_iBuildingClassType(NO_BUILDINGCLASS),
m_iVictoryPrereq(NO_VICTORY),
m_iFreeStartEra(NO_ERA),
m_iMaxStartEra(NO_ERA),
m_iFreePromotion(NO_PROMOTION),
m_iRouteTypeCreated(NO_ROUTE), //ray, removing hardcoded Roads for Buildings
m_iAIWeight(0),
m_iHurryCostModifier(0),
m_iAdvancedStartCost(0),
m_iAdvancedStartCostIncrease(0),
m_iProfessionOutput(0),
m_iMaxWorkers(0),
m_iMinAreaSize(0),
m_iNumCitiesPrereq(0),
m_iNumTeamsPrereq(0),
m_iUnitLevelPrereq(0),
m_iMinLatitude(0),
m_iMaxLatitude(90),
m_iFreeExperience(0),
m_iFoodKept(0),
m_iMilitaryProductionModifier(0),
m_iAssetValue(0),
m_iPowerValue(0),
m_iYieldStorage(0),
m_iMaxHarbourSpaceProvided(0), // WTP, ray, new Harbour System - START
m_iMaxBarracksSpaceProvided(0), // WTP, ray, new Barracks System - START
m_iSpecialBuildingType(NO_SPECIALBUILDING),
m_eIndexOf_NextBuildingType_In_SpecialBuilding(NO_BUILDING),
m_iConquestProbability(0),
m_iHealRateChange(0),
m_iDefenseModifier(0),
m_iBombardDefenseModifier(0),
m_iMissionType(NO_MISSION),
m_iStorageLossSellPercentage(0),
m_bIgnoresBoycott(false),
m_bUnlocksStorageLossTradeSettings(false),
m_iSpecialBuildingPriority(0),
m_fVisibilityPriority(0.0f),
m_bWorksWater(false),
m_bWater(false),
m_bRiver(false),
m_bCapital(false),
m_bNationalWonder(false), // R&R, ray, National Wonders
m_bNeverCapture(false),
m_bCenterInCity(false),
m_iDomesticMarketModifier(0),
m_iEntertainmentGoldModifier(0), // ray, Balancing of Entertainment Buildings in XML
m_aiProductionTraits(NULL),
m_aiLandPlotYieldChange(NULL), // R&R, ray, Landplot Yields
m_aiSeaPlotYieldChange(NULL),
m_aiRiverPlotYieldChange(NULL),
m_aiYieldChange(NULL),
m_aiYieldModifier(NULL),
m_aiUnitCombatFreeExperience(NULL),
m_aiDomainFreeExperience(NULL),
m_aiDomainProductionModifier(NULL),
m_aiPrereqNumOfBuildingClass(NULL),
m_aiYieldCost(NULL),
m_abBuildingClassNeededInCity(NULL)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvBuildingInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvBuildingInfo::~CvBuildingInfo()
{
	SAFE_DELETE_ARRAY(m_aiProductionTraits);
	SAFE_DELETE_ARRAY(m_aiLandPlotYieldChange); // R&R, ray, Landplot Yields
	SAFE_DELETE_ARRAY(m_aiSeaPlotYieldChange);
	SAFE_DELETE_ARRAY(m_aiRiverPlotYieldChange);
	SAFE_DELETE_ARRAY(m_aiYieldChange);
	SAFE_DELETE_ARRAY(m_aiYieldModifier);
	SAFE_DELETE_ARRAY(m_aiUnitCombatFreeExperience);
	SAFE_DELETE_ARRAY(m_aiDomainFreeExperience);
	SAFE_DELETE_ARRAY(m_aiDomainProductionModifier);
	SAFE_DELETE_ARRAY(m_aiPrereqNumOfBuildingClass);
	SAFE_DELETE_ARRAY(m_aiYieldCost);
	SAFE_DELETE_ARRAY(m_abBuildingClassNeededInCity);
}
int CvBuildingInfo::getBuildingClassType() const
{
	return m_iBuildingClassType;
}
int CvBuildingInfo::getVictoryPrereq() const
{
	return m_iVictoryPrereq;
}
int CvBuildingInfo::getFreeStartEra() const
{
	return m_iFreeStartEra;
}
int CvBuildingInfo::getMaxStartEra() const
{
	return m_iMaxStartEra;
}
int CvBuildingInfo::getFreePromotion() const
{
	return m_iFreePromotion;
}
//ray, removing hardcoded Roads for Buildings - START
int CvBuildingInfo::getRouteTypeCreated() const
{
	return m_iRouteTypeCreated;
}
//ray, removing hardcoded Roads for Buildings - END
int CvBuildingInfo::getAIWeight() const
{
	return m_iAIWeight;
}
int CvBuildingInfo::getHurryCostModifier() const
{
	return m_iHurryCostModifier;
}
int CvBuildingInfo::getAdvancedStartCost() const
{
	return m_iAdvancedStartCost;
}
int CvBuildingInfo::getAdvancedStartCostIncrease() const
{
	return m_iAdvancedStartCostIncrease;
}
int CvBuildingInfo::getProfessionOutput() const
{
	return m_iProfessionOutput;
}
int CvBuildingInfo::getMaxWorkers() const
{
	return m_iMaxWorkers;
}
int CvBuildingInfo::getMinAreaSize() const
{
	return m_iMinAreaSize;
}
int CvBuildingInfo::getNumCitiesPrereq() const
{
	return m_iNumCitiesPrereq;
}
int CvBuildingInfo::getNumTeamsPrereq() const
{
	return m_iNumTeamsPrereq;
}
int CvBuildingInfo::getUnitLevelPrereq() const
{
	return m_iUnitLevelPrereq;
}
int CvBuildingInfo::getMinLatitude() const
{
	return m_iMinLatitude;
}
int CvBuildingInfo::getMaxLatitude() const
{
	return m_iMaxLatitude;
}
int CvBuildingInfo::getFreeExperience() const
{
	return m_iFreeExperience;
}
int CvBuildingInfo::getFoodKept() const
{
	return m_iFoodKept;
}
int CvBuildingInfo::getMilitaryProductionModifier() const
{
	return m_iMilitaryProductionModifier;
}
int CvBuildingInfo::getAssetValue() const
{
	return m_iAssetValue;
}
int CvBuildingInfo::getPowerValue() const
{
	return m_iPowerValue;
}
int CvBuildingInfo::getYieldStorage() const
{
	return m_iYieldStorage;
}
// WTP, ray, new Harbour System - START
int CvBuildingInfo::getMaxHarbourSpaceProvided() const
{
	return m_iMaxHarbourSpaceProvided;
}
// WTP, ray, new Harbour System - END

// WTP, ray, new Barracks System - START
int CvBuildingInfo::getMaxBarracksSpaceProvided() const
{
	return m_iMaxBarracksSpaceProvided;
}
// WTP, ray, new Barracks System - END

int CvBuildingInfo::getSpecialBuildingType() const
{
	return m_iSpecialBuildingType;
}
int CvBuildingInfo::getConquestProbability() const
{
	return m_iConquestProbability;
}
int CvBuildingInfo::getHealRateChange() const
{
	return m_iHealRateChange;
}
int CvBuildingInfo::getDefenseModifier() const
{
	return m_iDefenseModifier;
}
int CvBuildingInfo::getBombardDefenseModifier() const
{
	return m_iBombardDefenseModifier;
}
int CvBuildingInfo::getMissionType() const
{
	return m_iMissionType;
}
void CvBuildingInfo::setMissionType(int iNewType)
{
	m_iMissionType = iNewType;
}
int CvBuildingInfo::getSpecialBuildingPriority() const
{
	return m_iSpecialBuildingPriority;
}
float CvBuildingInfo::getVisibilityPriority() const
{
	return m_fVisibilityPriority;
}
bool CvBuildingInfo::isWater() const
{
	return m_bWater;
}
bool CvBuildingInfo::isWorksWater() const
{
	return m_bWorksWater;
}
bool CvBuildingInfo::isRiver() const
{
	return m_bRiver;
}
//WTP, Nightinggale - Terrain locator - start
const InfoArray<TerrainTypes>& CvBuildingInfo::getRequiredAdjacentTerrains() const
{
	return m_info_RequiredAdjacentTerrains;
}
const InfoArray<TerrainTypes>& CvBuildingInfo::getRequiredCatchmentAreaTerrains() const
{
	return m_info_RequiredCatchmentAreaTerrains;
}
const InfoArray<FeatureTypes>& CvBuildingInfo::getRequiredCatchmentAreaFeatures() const
{
	return m_info_RequiredCatchmentAreaFeatures;
}
const InfoArray<TerrainTypes>& CvBuildingInfo::getAIRequiredCatchmentAreaTerrains() const
{
	return m_info_AIRequiredCatchmentAreaTerrains;
}
const InfoArray<FeatureTypes>& CvBuildingInfo::getAIRequiredCatchmentAreaFeatures() const
{
	return m_info_AIRequiredCatchmentAreaFeatures;
}
const InfoArray<PlotTypes>& CvBuildingInfo::AI_getRequiredCatchmentAreaPlotTypes() const
{
	return m_info_AIRequiredCatchmentAreaPlotTypes;
}
const InfoArray<UnitClassTypes, int>& CvBuildingInfo::AI_getUnitClassWeight() const
{
	return m_info_AIUnitClassWeight;
}
//WTP, Nightinggale - Terrain locator - end
bool CvBuildingInfo::isCapital() const
{
	return m_bCapital;
}
bool CvBuildingInfo::isNationalWonder() const // R&R, ray, National Wonders
{
	return m_bNationalWonder;
}
bool CvBuildingInfo::isNeverCapture() const
{
	return m_bNeverCapture;
}
bool CvBuildingInfo::isCenterInCity() const
{
	return m_bCenterInCity;
}
// R&R, Androrc, Domestic Market -- modified by Nightinggale - start
const InfoArray<YieldTypes, int>& CvBuildingInfo::getYieldDemands() const
{
	return m_info_YieldDemands;
}
// R&R, Androrc, Domestic Market -- modified by Nightinggale - end
const char* CvBuildingInfo::getConstructSound() const
{
	return m_szConstructSound;
}
void CvBuildingInfo::setConstructSound(const char* szVal)
{
	m_szConstructSound = szVal;
}
const char* CvBuildingInfo::getArtDefineTag() const
{
	return m_szArtDefineTag;
}
void CvBuildingInfo::setArtDefineTag(const char* szVal)
{
	m_szArtDefineTag = szVal;
}
const char* CvBuildingInfo::getMovieDefineTag() const
{
	return m_szMovieDefineTag;
}
void CvBuildingInfo::setMovieDefineTag(const char* szVal)
{
	m_szMovieDefineTag = szVal;
}
// Arrays
int CvBuildingInfo::getYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiYieldChange ? m_aiYieldChange[i] : -1;
}
int* CvBuildingInfo::getYieldChangeArray() const
{
	return m_aiYieldChange;
}
int CvBuildingInfo::getYieldModifier(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiYieldModifier ? m_aiYieldModifier[i] : -1;
}
int* CvBuildingInfo::getYieldModifierArray() const
{
	return m_aiYieldModifier;
}
// R&R, ray, Landplot Yields - START
int CvBuildingInfo::getLandPlotYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiLandPlotYieldChange ? m_aiLandPlotYieldChange[i] : -1;
}
int* CvBuildingInfo::getLandPlotYieldChangeArray() const
{
	return m_aiLandPlotYieldChange;
}
// R&R, ray, Landplot Yields - END
int CvBuildingInfo::getSeaPlotYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiSeaPlotYieldChange ? m_aiSeaPlotYieldChange[i] : -1;
}
int* CvBuildingInfo::getSeaPlotYieldChangeArray() const
{
	return m_aiSeaPlotYieldChange;
}
int CvBuildingInfo::getRiverPlotYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiRiverPlotYieldChange ? m_aiRiverPlotYieldChange[i] : -1;
}
int* CvBuildingInfo::getRiverPlotYieldChangeArray() const
{
	return m_aiRiverPlotYieldChange;
}
int CvBuildingInfo::getUnitCombatFreeExperience(int i) const
{
	FAssertMsg(i < GC.getNumUnitCombatInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiUnitCombatFreeExperience ? m_aiUnitCombatFreeExperience[i] : -1;
}
int CvBuildingInfo::getDomainFreeExperience(int i) const
{
	FAssertMsg(i < NUM_DOMAIN_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiDomainFreeExperience ? m_aiDomainFreeExperience[i] : -1;
}
int CvBuildingInfo::getDomainProductionModifier(int i) const
{
	FAssertMsg(i < NUM_DOMAIN_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiDomainProductionModifier ? m_aiDomainProductionModifier[i] : -1;
}
int CvBuildingInfo::getProductionTraits(int i) const
{
	FAssertMsg(i < GC.getNumTraitInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiProductionTraits ? m_aiProductionTraits[i] : -1;
}
int CvBuildingInfo::getPrereqNumOfBuildingClass(int i) const
{
	FAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiPrereqNumOfBuildingClass ? m_aiPrereqNumOfBuildingClass[i] : -1;
}
int CvBuildingInfo::getYieldCost(int i) const
{
	FAssert(i < NUM_YIELD_TYPES && i >= 0);
	return m_aiYieldCost ? m_aiYieldCost[i] : -1;
}
bool CvBuildingInfo::isBuildingClassNeededInCity(int i) const
{
	FAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abBuildingClassNeededInCity ? m_abBuildingClassNeededInCity[i] : false;
}
const char* CvBuildingInfo::getButton() const
{
	const CvArtInfoBuilding * pBuildingArtInfo;
	pBuildingArtInfo = getArtInfo();
	if (pBuildingArtInfo != NULL)
	{
		return pBuildingArtInfo->getButton();
	}
	else
	{
		return NULL;
	}
}
const CvArtInfoBuilding* CvBuildingInfo::getArtInfo() const
{
	return ARTFILEMGR.getBuildingArtInfo(getArtDefineTag());
}
const CvArtInfoMovie* CvBuildingInfo::getMovieInfo() const
{
	const char* pcTag = getMovieDefineTag();
	if (NULL != pcTag && 0 != strcmp(pcTag, "NONE"))
	{
		return ARTFILEMGR.getMovieArtInfo(pcTag);
	}
	else
	{
		return NULL;
	}
}
const char* CvBuildingInfo::getMovie() const
{
	const CvArtInfoMovie* pArt;
	pArt = getMovieInfo();
	if (pArt != NULL)
	{
		return pArt->getPath();
	}
	else
	{
		return NULL;
	}
}

//
// serialization
//
void CvBuildingInfo::read(FDataStreamBase* stream)
{
	CvHotkeyInfo::read(stream);
	uint uiFlag=0;
	stream->Read(&uiFlag);	// flags for expansion
	stream->Read(&m_iBuildingClassType);
	stream->Read(&m_iVictoryPrereq);
	stream->Read(&m_iFreeStartEra);
	stream->Read(&m_iMaxStartEra);
	stream->Read(&m_iFreePromotion);
	stream->Read(&m_iRouteTypeCreated); //ray, removing hardcoded Roads for Buildings
	stream->Read(&m_iAIWeight);
	stream->Read(&m_iHurryCostModifier);
	stream->Read(&m_iAdvancedStartCost);
	stream->Read(&m_iAdvancedStartCostIncrease);
	stream->Read(&m_iProfessionOutput);
	stream->Read(&m_iMaxWorkers);
	stream->Read(&m_iMinAreaSize);
	stream->Read(&m_iNumCitiesPrereq);
	stream->Read(&m_iNumTeamsPrereq);
	stream->Read(&m_iUnitLevelPrereq);
	stream->Read(&m_iMinLatitude);
	stream->Read(&m_iMaxLatitude);
	stream->Read(&m_iFreeExperience);
	stream->Read(&m_iFoodKept);
	stream->Read(&m_iMilitaryProductionModifier);
	stream->Read(&m_iAssetValue);
	stream->Read(&m_iPowerValue);
	stream->Read(&m_iYieldStorage);
	stream->Read(&m_iMaxHarbourSpaceProvided); // WTP, ray, new Harbour System - START
	stream->Read(&m_iMaxBarracksSpaceProvided); // WTP, ray, new Barracks System - START
	stream->Read(&m_iSpecialBuildingType);
	stream->Read(&m_eIndexOf_NextBuildingType_In_SpecialBuilding);
	stream->Read(&m_iConquestProbability);
	stream->Read(&m_iHealRateChange);
	stream->Read(&m_iDefenseModifier);
	stream->Read(&m_iBombardDefenseModifier);
	stream->Read(&m_iMissionType);
	/** NBMOD EDU **/
	stream->Read(&m_iTeachLevel);
	/** NBMOD EDU **/
	// TAC - AI Buildings - koma13 - START
	stream->Read(&m_iAICitySize);
	// TAC - AI Buildings - koma13 - END
	stream->Read(&m_iStorageLossSellPercentage);
	stream->Read(&m_iSpecialBuildingPriority);
	stream->Read(&m_fVisibilityPriority);
	stream->Read(&m_bWorksWater);
	stream->Read(&m_bWater);
	stream->Read(&m_bRiver);
	stream->Read(&m_bCapital);
	stream->Read(&m_bNationalWonder); // R&R, ray, National Wonders
	stream->Read(&m_bNeverCapture);
	stream->Read(&m_bCenterInCity);
	stream->ReadString(m_szConstructSound);
	stream->ReadString(m_szArtDefineTag);
	stream->ReadString(m_szMovieDefineTag);
	SAFE_DELETE_ARRAY(m_aiProductionTraits);
	m_aiProductionTraits = new int[GC.getNumTraitInfos()];
	stream->Read(GC.getNumTraitInfos(), m_aiProductionTraits);
	SAFE_DELETE_ARRAY(m_aiLandPlotYieldChange); // R&R, ray, Landplot Yields
	m_aiLandPlotYieldChange = new int[NUM_YIELD_TYPES]; // R&R, ray, Landplot Yields
	stream->Read(NUM_YIELD_TYPES, m_aiLandPlotYieldChange); // R&R, ray, Landplot Yields
	SAFE_DELETE_ARRAY(m_aiSeaPlotYieldChange);
	m_aiSeaPlotYieldChange = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_aiSeaPlotYieldChange);
	SAFE_DELETE_ARRAY(m_aiRiverPlotYieldChange);
	m_aiRiverPlotYieldChange = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_aiRiverPlotYieldChange);
	SAFE_DELETE_ARRAY(m_aiYieldChange);
	m_aiYieldChange = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_aiYieldChange);
	SAFE_DELETE_ARRAY(m_aiYieldModifier);
	m_aiYieldModifier = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_aiYieldModifier);
	SAFE_DELETE_ARRAY(m_aiUnitCombatFreeExperience);
	m_aiUnitCombatFreeExperience = new int[GC.getNumUnitCombatInfos()];
	stream->Read(GC.getNumUnitCombatInfos(), m_aiUnitCombatFreeExperience);
	SAFE_DELETE_ARRAY(m_aiDomainFreeExperience);
	m_aiDomainFreeExperience = new int[NUM_DOMAIN_TYPES];
	stream->Read(NUM_DOMAIN_TYPES, m_aiDomainFreeExperience);
	SAFE_DELETE_ARRAY(m_aiDomainProductionModifier);
	m_aiDomainProductionModifier = new int[NUM_DOMAIN_TYPES];
	stream->Read(NUM_DOMAIN_TYPES, m_aiDomainProductionModifier);
	SAFE_DELETE_ARRAY(m_aiPrereqNumOfBuildingClass);
	m_aiPrereqNumOfBuildingClass = new int[GC.getNumBuildingClassInfos()];
	stream->Read(GC.getNumBuildingClassInfos(), m_aiPrereqNumOfBuildingClass);
	SAFE_DELETE_ARRAY(m_aiYieldCost);
	m_aiYieldCost = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_aiYieldCost);
	SAFE_DELETE_ARRAY(m_abBuildingClassNeededInCity);
	m_abBuildingClassNeededInCity = new bool[GC.getNumBuildingClassInfos()];
	stream->Read(GC.getNumBuildingClassInfos(), m_abBuildingClassNeededInCity);
}
//
// serialization
//
void CvBuildingInfo::write(FDataStreamBase* stream)
{
	CvHotkeyInfo::write(stream);
	uint uiFlag=0;
	stream->Write(uiFlag);		// flag for expansion
	stream->Write(m_iBuildingClassType);
	stream->Write(m_iVictoryPrereq);
	stream->Write(m_iFreeStartEra);
	stream->Write(m_iMaxStartEra);
	stream->Write(m_iFreePromotion);
	stream->Write(m_iRouteTypeCreated); //ray, removing hardcoded Roads for Buildings
	stream->Write(m_iAIWeight);
	stream->Write(m_iHurryCostModifier);
	stream->Write(m_iAdvancedStartCost);
	stream->Write(m_iAdvancedStartCostIncrease);
	stream->Write(m_iProfessionOutput);
	stream->Write(m_iMaxWorkers);
	stream->Write(m_iMinAreaSize);
	stream->Write(m_iNumCitiesPrereq);
	stream->Write(m_iNumTeamsPrereq);
	stream->Write(m_iUnitLevelPrereq);
	stream->Write(m_iMinLatitude);
	stream->Write(m_iMaxLatitude);
	stream->Write(m_iFreeExperience);
	stream->Write(m_iFoodKept);
	stream->Write(m_iMilitaryProductionModifier);
	stream->Write(m_iAssetValue);
	stream->Write(m_iPowerValue);
	stream->Write(m_iYieldStorage);
	stream->Write(m_iMaxHarbourSpaceProvided); // WTP, ray, new Harbour System - START
	stream->Write(m_iMaxBarracksSpaceProvided);	// WTP, ray, new Barracks System - START
	stream->Write(m_iSpecialBuildingType);
	stream->Write(m_eIndexOf_NextBuildingType_In_SpecialBuilding);
	stream->Write(m_iConquestProbability);
	stream->Write(m_iHealRateChange);
	stream->Write(m_iDefenseModifier);
	stream->Write(m_iBombardDefenseModifier);
	stream->Write(m_iMissionType);
	/** NBMOD EDU **/
	stream->Write(m_iTeachLevel);
	/** NBMOD EDU **/
	// TAC - AI Buildings - koma13 - START
	stream->Write(m_iAICitySize);
	// TAC - AI Buildings - koma13 - END
	stream->Write(m_iStorageLossSellPercentage);
	stream->Write(m_iSpecialBuildingPriority);
	stream->Write(m_fVisibilityPriority);
	stream->Write(m_bWorksWater);
	stream->Write(m_bWater);
	stream->Write(m_bRiver);
	stream->Write(m_bCapital);
	stream->Write(m_bNationalWonder); // R&R, ray, National Wonders
	stream->Write(m_bNeverCapture);
	stream->Write(m_bCenterInCity);
	stream->WriteString(m_szConstructSound);
	stream->WriteString(m_szArtDefineTag);
	stream->WriteString(m_szMovieDefineTag);
	stream->Write(GC.getNumTraitInfos(), m_aiProductionTraits);
	stream->Write(NUM_YIELD_TYPES, m_aiLandPlotYieldChange); // R&R, ray, Landplot Yields
	stream->Write(NUM_YIELD_TYPES, m_aiSeaPlotYieldChange);
	stream->Write(NUM_YIELD_TYPES, m_aiRiverPlotYieldChange);
	stream->Write(NUM_YIELD_TYPES, m_aiYieldChange);
	stream->Write(NUM_YIELD_TYPES, m_aiYieldModifier);
	stream->Write(GC.getNumUnitCombatInfos(), m_aiUnitCombatFreeExperience);
	stream->Write(NUM_DOMAIN_TYPES, m_aiDomainFreeExperience);
	stream->Write(NUM_DOMAIN_TYPES, m_aiDomainProductionModifier);
	stream->Write(GC.getNumBuildingClassInfos(), m_aiPrereqNumOfBuildingClass);
	stream->Write(NUM_YIELD_TYPES, m_aiYieldCost);
	stream->Write(GC.getNumBuildingClassInfos(), m_abBuildingClassNeededInCity);
}
//
// read from XML
//
bool CvBuildingInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}
	int j=0;						//loop counter
	int k=0;						//loop counter
	int iNumSibs=0;				// the number of siblings the current xml node has

    /** NBMOD EDU **/
	pXML->GetChildXmlValByName(&m_iTeachLevel, "iTeachLevel");
	/** NBMOD EDU **/
	// TAC - AI Buildings - koma13 - START
	pXML->GetChildXmlValByName(&m_iAICitySize, "iAICitySize");
	// TAC - AI Buildings - koma13 - END

	pXML->GetChildXmlValByName(szTextVal, "BuildingClass");
	m_iBuildingClassType = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "SpecialBuildingType");
	m_iSpecialBuildingType = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "ArtDefineTag");
	setArtDefineTag(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "MovieDefineTag");
	setMovieDefineTag(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "VictoryPrereq");
	m_iVictoryPrereq = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "FreeStartEra");
	m_iFreeStartEra = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "MaxStartEra");
	m_iMaxStartEra = pXML->FindInInfoClass(szTextVal);
	pXML->SetVariableListTagPair(&m_aiProductionTraits, "ProductionTraits", GC.getNumTraitInfos(), 0);
	pXML->GetChildXmlValByName(szTextVal, "FreePromotion");
	m_iFreePromotion = pXML->FindInInfoClass(szTextVal);
	//ray, removing hardcoded Roads for Buildings - START
	pXML->GetChildXmlValByName(szTextVal, "RouteTypeCreated");
	m_iRouteTypeCreated = pXML->FindInInfoClass(szTextVal);
	//ray, removing hardcoded Roads for Buildings - END
	pXML->GetChildXmlValByName(&m_iDomesticMarketModifier, "iDomesticMarketModifier");
	pXML->GetChildXmlValByName(&m_iEntertainmentGoldModifier, "iEntertainmentGoldModifier"); // ray, Balancing of Entertainment Buildings in XML
	readXML(m_info_YieldDemands, "YieldDemands");
	pXML->GetChildXmlValByName(&m_bWorksWater, "bWorksWater");
	pXML->GetChildXmlValByName(&m_bWater, "bWater");
	pXML->GetChildXmlValByName(&m_bRiver, "bRiver");
	pXML->GetChildXmlValByName(&m_bCapital, "bCapital");
	pXML->GetChildXmlValByName(&m_bNationalWonder, "bNationalWonder"); // R&R, ray, National Wonders
	pXML->GetChildXmlValByName(&m_bNeverCapture, "bNeverCapture");
	pXML->GetChildXmlValByName(&m_bCenterInCity, "bCenterInCity");
	pXML->GetChildXmlValByName(&m_iAIWeight, "iAIWeight");
	pXML->GetChildXmlValByName(&m_iHurryCostModifier, "iHurryCostModifier");
	pXML->GetChildXmlValByName(&m_iAdvancedStartCost, "iAdvancedStartCost");
	pXML->GetChildXmlValByName(&m_iAdvancedStartCostIncrease, "iAdvancedStartCostIncrease");
	pXML->GetChildXmlValByName(&m_iProfessionOutput, "iProfessionOutput");
	pXML->GetChildXmlValByName(&m_iMaxWorkers, "iMaxWorkers");
	pXML->GetChildXmlValByName(&m_iMinAreaSize, "iMinAreaSize");
	pXML->GetChildXmlValByName(&m_iConquestProbability, "iConquestProb");
	pXML->GetChildXmlValByName(&m_iNumCitiesPrereq, "iCitiesPrereq");
	pXML->GetChildXmlValByName(&m_iNumTeamsPrereq, "iTeamsPrereq");
	pXML->GetChildXmlValByName(&m_iUnitLevelPrereq, "iLevelPrereq");
	pXML->GetChildXmlValByName(&m_iMinLatitude, "iMinLatitude");
	pXML->GetChildXmlValByName(&m_iMaxLatitude, "iMaxLatitude");
	pXML->GetChildXmlValByName(&m_iFreeExperience, "iExperience");
	pXML->GetChildXmlValByName(&m_iFoodKept, "iFoodKept");
	pXML->GetChildXmlValByName(&m_iHealRateChange, "iHealRateChange");
	pXML->GetChildXmlValByName(&m_iMilitaryProductionModifier, "iMilitaryProductionModifier");
	pXML->GetChildXmlValByName(&m_iDefenseModifier, "iDefense");
	pXML->GetChildXmlValByName(&m_iBombardDefenseModifier, "iBombardDefense");
	pXML->GetChildXmlValByName(&m_iStorageLossSellPercentage, "iStorageLossSellPercentage");
	pXML->GetChildXmlValByName(&m_bIgnoresBoycott, "bIgnoresBoycott");
	pXML->GetChildXmlValByName(&m_bUnlocksStorageLossTradeSettings, "bUnlocksStorageLossTradeSettings");
	pXML->GetChildXmlValByName(&m_iSpecialBuildingPriority, "iSpecialBuildingPriority");
	pXML->GetChildXmlValByName(&m_iAssetValue, "iAsset");
	pXML->GetChildXmlValByName(&m_iPowerValue, "iPower");
	pXML->GetChildXmlValByName(&m_iYieldStorage, "iYieldStorage");
	pXML->GetChildXmlValByName(&m_iMaxHarbourSpaceProvided, "iMaxHarbourSpaceProvided"); // WTP, ray, new Harbour System - START
	pXML->GetChildXmlValByName(&m_iMaxBarracksSpaceProvided, "iMaxBarracksSpaceProvided"); // WTP, ray, new Barracks System - START
	pXML->GetChildXmlValByName(&m_fVisibilityPriority, "fVisibilityPriority");
	pXML->SetVariableListTagPair(&m_aiLandPlotYieldChange, "LandPlotYieldChanges", NUM_YIELD_TYPES, 0); // R&R, ray, Landplot Yields
	pXML->SetVariableListTagPair(&m_aiSeaPlotYieldChange, "SeaPlotYieldChanges", NUM_YIELD_TYPES, 0);
	pXML->SetVariableListTagPair(&m_aiRiverPlotYieldChange, "RiverPlotYieldChanges", NUM_YIELD_TYPES, 0);
	pXML->SetVariableListTagPair(&m_aiYieldChange, "YieldChanges", NUM_YIELD_TYPES, 0);
	pXML->SetVariableListTagPair(&m_aiYieldModifier, "YieldModifiers", NUM_YIELD_TYPES, 0);
	pXML->GetChildXmlValByName(szTextVal, "ConstructSound");
	setConstructSound(szTextVal);
	pXML->SetVariableListTagPair(&m_aiUnitCombatFreeExperience, "UnitCombatFreeExperiences", GC.getNumUnitCombatInfos(), 0);
	pXML->SetVariableListTagPair(&m_aiDomainFreeExperience, "DomainFreeExperiences", NUM_DOMAIN_TYPES, 0);
	pXML->SetVariableListTagPair(&m_aiDomainProductionModifier, "DomainProductionModifiers", NUM_DOMAIN_TYPES, 0);
	pXML->SetVariableListTagPair(&m_aiPrereqNumOfBuildingClass, "PrereqBuildingClasses", GC.getNumBuildingClassInfos(), 0);
	pXML->SetVariableListTagPair(&m_abBuildingClassNeededInCity, "BuildingClassNeededs", GC.getNumBuildingClassInfos(), false);
	//WTP, Nightinggale - Terrain locator - start
	readXML(m_info_RequiredAdjacentTerrains        , "RequiredAdjacentTerrains"        );
	readXML(m_info_RequiredCatchmentAreaTerrains   , "RequiredCatchmentAreaTerrains"   );
	readXML(m_info_RequiredCatchmentAreaFeatures   , "RequiredCatchmentAreaFeatures"   );
	readXML(m_info_AIRequiredCatchmentAreaTerrains , "AIRequiredCatchmentAreaTerrains" );
	readXML(m_info_AIRequiredCatchmentAreaFeatures , "AIRequiredCatchmentAreaFeatures" );
	readXML(m_info_AIRequiredCatchmentAreaPlotTypes, "AIRequiredCatchmentAreaPlotTypes");
	readXML(m_info_AIUnitClassWeight               , "AIUnitClassWeights"              );
	//WTP, Nightinggale - Terrain locator - end
	pXML->SetVariableListTagPair(&m_aiYieldCost, "YieldCosts", NUM_YIELD_TYPES, 0);
	return true;
}

bool CvBuildingInfo::readPass2(CvXMLLoadUtility* pXML)
{
	m_eIndexOf_NextBuildingType_In_SpecialBuilding = getIndexForType<BuildingTypes>(getType());
	if(getSpecialBuildingType() != NO_SPECIALBUILDING)
	{
		for(int i=0;i<GC.getNumBuildingInfos();i++)
		{
			BuildingTypes eLoopBuilding = (BuildingTypes) ((m_eIndexOf_NextBuildingType_In_SpecialBuilding + i + 1) % GC.getNumBuildingInfos());
			if(GC.getBuildingInfo(eLoopBuilding).getSpecialBuildingType() == getSpecialBuildingType())
			{
				m_eIndexOf_NextBuildingType_In_SpecialBuilding = eLoopBuilding;
				break;
			}
		}
	}

	return true;
}
//======================================================================================================
//					CvSpecialBuildingInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvSpecialBuildingInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvSpecialBuildingInfo::CvSpecialBuildingInfo() :
m_bValid(false),
m_iChar(0),
m_iFontButtonIndex(0),
m_aiProductionTraits(NULL)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvSpecialBuildingInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvSpecialBuildingInfo::~CvSpecialBuildingInfo()
{
	SAFE_DELETE_ARRAY(m_aiProductionTraits);
}
bool CvSpecialBuildingInfo::isValid() const
{
	return m_bValid;
}
int CvSpecialBuildingInfo::getChar() const
{
	return m_iChar;
}
void CvSpecialBuildingInfo::setChar(int i)
{
	m_iChar = i;
}
int CvSpecialBuildingInfo::getFontButtonIndex() const
{
	return m_iFontButtonIndex;
}
// Arrays
const InfoArray<BuildingTypes, int>& CvSpecialBuildingInfo::getBuildings() const
{
	return m_buildings;
}
int CvSpecialBuildingInfo::getProductionTraits(int i) const
{
	FAssertMsg(i < GC.getNumTraitInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiProductionTraits ? m_aiProductionTraits[i] : -1;
}
// TAC - Nature Objects - koma13 - START
const char* CvSpecialBuildingInfo::getNatureObject() const
{
	return m_szNatureObject;
}
// TAC - Nature Objects - koma13 - END
bool CvSpecialBuildingInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(&m_bValid, "bValid");
	pXML->GetChildXmlValByName(&m_iFontButtonIndex, "FontButtonIndex");
	pXML->SetVariableListTagPair(&m_aiProductionTraits, "ProductionTraits", GC.getNumTraitInfos(), 0);
	pXML->GetChildXmlValByName(m_szNatureObject, "NatureObject");	// TAC - Nature Objects - koma13
	return true;
}
void CvSpecialBuildingInfo::postXmlReadSetup()
{
	EnumMap<SpecialBuildingTypes, EnumMap<BuildingTypes, int, -1> > em;

	for (BuildingTypes eBuilding = FIRST_BUILDING; eBuilding < NUM_BUILDING_TYPES; ++eBuilding)
	{
		CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
		SpecialBuildingTypes eSpecial = static_cast<SpecialBuildingTypes>(kBuilding.getSpecialBuildingType());
		if (eSpecial != NO_SPECIALBUILDING)
		{
			em[eSpecial].set(eBuilding, kBuilding.getSpecialBuildingPriority());
		}
	}
	for (SpecialBuildingTypes eSpecialBuilding = em.FIRST; eSpecialBuilding <= em.LAST; ++eSpecialBuilding)
	{
		GC.getSpecialBuildingInfo(eSpecialBuilding).m_buildings.assignFrom(em[eSpecialBuilding]);
	}
}
//======================================================================================================
//					CvBuildingClassInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvBuildingClassInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvBuildingClassInfo::CvBuildingClassInfo() :
m_iDefaultBuildingIndex(NO_BUILDING),
m_aiVictoryThreshold(NULL)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvBuildingClassInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvBuildingClassInfo::~CvBuildingClassInfo()
{
	SAFE_DELETE_ARRAY(m_aiVictoryThreshold);
}
int CvBuildingClassInfo::getDefaultBuildingIndex() const
{
	return m_iDefaultBuildingIndex;
}
void CvBuildingClassInfo::setDefaultBuildingIndex(int i)
{
	m_iDefaultBuildingIndex = i;
}
// Arrays
int CvBuildingClassInfo::getVictoryThreshold(int i) const
{
	FAssertMsg(i < GC.getNumVictoryInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiVictoryThreshold ? m_aiVictoryThreshold[i] : -1;
}
bool CvBuildingClassInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->SetVariableListTagPair(&m_aiVictoryThreshold, "VictoryThresholds", GC.getNumVictoryInfos(), 0);
	CvString szTextVal;
	pXML->GetChildXmlValByName(szTextVal, "DefaultBuilding");
	m_aszExtraXMLforPass3.push_back(szTextVal);
	return true;
}
bool CvBuildingClassInfo::readPass3()
{
	if (m_aszExtraXMLforPass3.size() < 1)
	{
		FAssert(false);
		return false;
	}
	m_iDefaultBuildingIndex = GC.getInfoTypeForString(m_aszExtraXMLforPass3[0]);
	m_aszExtraXMLforPass3.clear();
	return true;
}

//======================================================================================================
//					CvRiverModelInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvRiverModelInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvRiverModelInfo::CvRiverModelInfo() :
m_iTextureIndex(0)
{
	m_szDeltaString[0] = '\0';
	m_szConnectString[0] = '\0';
	m_szRotateString[0] = '\0';
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvRiverModelInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvRiverModelInfo::~CvRiverModelInfo()
{
}
const char* CvRiverModelInfo::getModelFile() const
{
	return m_szModelFile;
}
void CvRiverModelInfo::setModelFile(const char* szVal)					// The model filename
{
	m_szModelFile=szVal;
}
const char* CvRiverModelInfo::getBorderFile() const
{
	return m_szBorderFile;
}
void CvRiverModelInfo::setBorderFile(const char* szVal)					// The model filename
{
	m_szBorderFile=szVal;
}
int CvRiverModelInfo::getTextureIndex() const
{
	return m_iTextureIndex;
}
const char* CvRiverModelInfo::getDeltaString() const
{
	return m_szDeltaString;
}
const char* CvRiverModelInfo::getConnectString() const
{
	return m_szConnectString;
}
const char* CvRiverModelInfo::getRotateString() const
{
	return m_szRotateString;
}
bool CvRiverModelInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	if(pXML->GetChildXmlValByName(szTextVal, "ModelFile"))
	{
		setModelFile(szTextVal);
	}
	if(pXML->GetChildXmlValByName(szTextVal, "BorderFile"))
	{
		setBorderFile(szTextVal);
	}
	pXML->GetChildXmlValByName(&m_iTextureIndex, "TextureIndex");
	pXML->GetChildXmlValByName(m_szDeltaString, "DeltaType");
	pXML->GetChildXmlValByName(m_szConnectString, "Connections");
	pXML->GetChildXmlValByName(m_szRotateString, "Rotations");
	return true;
}
//======================================================================================================
//					CvRouteModelInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvRouteModelInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvRouteModelInfo::CvRouteModelInfo() :
m_eRouteType(NO_ROUTE)
{
	m_szConnectString[0] = '\0';
	m_szModelConnectString[0] = '\0';
	m_szRotateString[0] = '\0';
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvRouteModelInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvRouteModelInfo::~CvRouteModelInfo()
{
}
RouteTypes CvRouteModelInfo::getRouteType() const		// The route type
{
	return m_eRouteType;
}
const char* CvRouteModelInfo::getModelFile() const
{
	return m_szModelFile;
}
void CvRouteModelInfo::setModelFile(const char* szVal)				// The model filename
{
	m_szModelFile=szVal;
}
const char* CvRouteModelInfo::getConnectString() const
{
	return m_szConnectString;
}
const char* CvRouteModelInfo::getModelConnectString() const
{
	return m_szModelConnectString;
}
const char* CvRouteModelInfo::getRotateString() const
{
	return m_szRotateString;
}
bool CvRouteModelInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(szTextVal, "ModelFile");
	setModelFile(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "RouteType");
	m_eRouteType = (RouteTypes)(pXML->FindInInfoClass(szTextVal));
	pXML->GetChildXmlValByName(m_szConnectString, "Connections");
	pXML->GetChildXmlValByName(m_szModelConnectString, "ModelConnections");
	pXML->GetChildXmlValByName(m_szRotateString, "Rotations");
	return true;
}
//======================================================================================================
//					CvCivilizationInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvCivilizationInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvCivilizationInfo::CvCivilizationInfo():
m_iDefaultPlayerColor(NO_PLAYERCOLOR),
m_iArtStyleType(NO_ARTSTYLE),
//Androrc UnitArtStyles
m_iUnitArtStyleType(NO_UNIT_ARTSTYLE),
//Androrc End
m_iNumCityNames(0),

m_iNumGeneralNames(0), // TAC - Great General Names - Ray - START
m_iNumAdmiralNames(0), // R&R, ray, Great Admirals - START
m_iNumShipNames(0), // TAC - Ship Names - Ray - START

m_iNumLeaders(0),
m_iSelectionSoundScriptId(0),
m_iActionSoundScriptId(0),
m_iDerivativeCiv(NO_CIVILIZATION),
m_iAdvancedStartPoints(0),
m_iAreaMultiplier(0),
m_iDensityMultiplier(0),
m_iTreasure(0),
m_iFavoredTerrain(NO_TERRAIN),
m_iCapturedCityUnitClass(NO_UNITCLASS),
m_eDefaultProfession(NO_PROFESSION),
m_iMissionaryChar(0),
m_bPlayable(false),
m_bAIPlayable(false),
m_bWaterStart(false),
m_bOpenBorders(false),
m_bWaterWorks(false),
m_bEurope(false),
m_bNative(false),
// R&R, ray, Correct Geographical Placement of Natives - START
m_bNorthAmericanNative(false),
m_bSouthAmericanNative(false),
m_bCentralAmericanNative(false),
// R&R, ray, Correct Geographical Placement of Natives - END
m_eCivCategory(NO_CIV_CATEGORY),
m_eCivEffect(NO_CIV_EFFECT),
m_aiCivilizationBuildings(NULL),
m_aiCivilizationUnits(NULL),
m_aiCivilizationInitialCivics(NULL),
m_aiFreeYields(NULL),
m_aiTeachUnitClassWeights(NULL),
m_abLeaders(NULL),
m_abCivilizationFreeBuildingClass(NULL),
m_abValidProfessions(NULL),
m_abTraits(NULL)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvCivilizationInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvCivilizationInfo::~CvCivilizationInfo()
{
	SAFE_DELETE_ARRAY(m_aiCivilizationBuildings);
	SAFE_DELETE_ARRAY(m_aiCivilizationUnits);
	SAFE_DELETE_ARRAY(m_aiCivilizationInitialCivics);
	SAFE_DELETE_ARRAY(m_aiFreeYields);
	SAFE_DELETE_ARRAY(m_aiTeachUnitClassWeights);
	SAFE_DELETE_ARRAY(m_abLeaders);
	SAFE_DELETE_ARRAY(m_abCivilizationFreeBuildingClass);
	SAFE_DELETE_ARRAY(m_abValidProfessions);
	SAFE_DELETE_ARRAY(m_abTraits);
}
void CvCivilizationInfo::reset()
{
	CvInfoBase::reset();
	m_szCachedShortDescription.clear();
	m_szCachedAdjective.clear();
}
int CvCivilizationInfo::getDefaultPlayerColor() const
{
	return m_iDefaultPlayerColor;
}
int CvCivilizationInfo::getArtStyleType() const
{
	return m_iArtStyleType;
}
//Androrc UnitArtStyles
int CvCivilizationInfo::getUnitArtStyleType() const
{
	return m_iUnitArtStyleType;
}
//Androrc End
int CvCivilizationInfo::getNumCityNames() const
{
	return m_iNumCityNames;
}

// TAC - Great General Names - Ray - START
int CvCivilizationInfo::getNumGeneralNames() const
{
	return m_iNumGeneralNames;
}
// TAC - Great General Names - Ray - END

// R&R, ray, Great Admirals - START
int CvCivilizationInfo::getNumAdmiralNames() const
{
	return m_iNumAdmiralNames;
}
// R&R, ray, Great Admirals - END


// TAC - Ship Names - Ray - START
int CvCivilizationInfo::getNumShipNames() const
{
	return m_iNumShipNames;
}
// TAC - Ship Names - Ray - END

int CvCivilizationInfo::getNumLeaders() const// the number of leaders the Civ has, this is needed so that random leaders can be generated easily
{
	return m_iNumLeaders;
}
int CvCivilizationInfo::getSelectionSoundScriptId() const
{
	return m_iSelectionSoundScriptId;
}
int CvCivilizationInfo::getActionSoundScriptId() const
{
	return m_iActionSoundScriptId;
}
int CvCivilizationInfo::getAdvancedStartPoints() const
{
	return m_iAdvancedStartPoints;
}
int CvCivilizationInfo::getAreaMultiplier() const
{
	return m_iAreaMultiplier;
}
int CvCivilizationInfo::getDensityMultiplier() const
{
	return m_iDensityMultiplier;
}
int CvCivilizationInfo::getTreasure() const
{
	return m_iTreasure;
}
int CvCivilizationInfo::getFavoredTerrain() const
{
	return m_iFavoredTerrain;
}
int CvCivilizationInfo::getCapturedCityUnitClass() const
{
	return m_iCapturedCityUnitClass;
}
ProfessionTypes CvCivilizationInfo::getDefaultProfession() const
{
	return m_eDefaultProfession;
}
int CvCivilizationInfo::getMissionaryChar() const
{
	return m_iMissionaryChar;
}
void CvCivilizationInfo::setMissionaryChar(int iChar)
{
	m_iMissionaryChar = iChar;
}
int CvCivilizationInfo::getChar() const
{
	return m_iMissionaryChar;
}
int CvCivilizationInfo::getTradingPostChar() const
{
	return m_iMissionaryChar + 1;
}
void CvCivilizationInfo::setChar(int iChar)
{
	m_iMissionaryChar = iChar;
}
bool CvCivilizationInfo::isAIPlayable() const
{
	return m_bAIPlayable;
}
bool CvCivilizationInfo::isPlayable() const
{
	return m_bPlayable;
}
bool CvCivilizationInfo::isWaterStart() const
{
	return m_bWaterStart;
}
bool CvCivilizationInfo::isOpenBorders() const
{
	return m_bOpenBorders;
}
bool CvCivilizationInfo::isWaterWorks() const
{
	return m_bWaterWorks;
}
bool CvCivilizationInfo::isEurope() const
{
	return m_bEurope;
}
bool CvCivilizationInfo::isNative() const
{
	return m_bNative;
}

// R&R, ray, Correct Geographical Placement of Natives - START
bool CvCivilizationInfo::isNorthAmericanNative() const
{
	return m_bNorthAmericanNative;
}
bool CvCivilizationInfo::isSouthAmericanNative() const
{
	return m_bSouthAmericanNative;
}
bool CvCivilizationInfo::isCentralAmericanNative() const
{
	return m_bCentralAmericanNative;
}
// R&R, ray, Correct Geographical Placement of Natives - END

CivCategoryTypes CvCivilizationInfo::getCivCategoryTypes() const
{
	return m_eCivCategory;
}

const wchar* CvCivilizationInfo::getShortDescription(uint uiForm)
{
	while (m_szCachedShortDescription.size() <= uiForm)
	{
		m_szCachedShortDescription.push_back(gDLL->getObjectText(m_szShortDescriptionKey, m_szCachedShortDescription.size()));
	}

	return m_szCachedShortDescription[uiForm];
}
const wchar* CvCivilizationInfo::getShortDescriptionKey() const
{
	return m_szShortDescriptionKey;
}
const wchar* CvCivilizationInfo::getAdjective(uint uiForm)
{
	while (m_szCachedAdjective.size() <= uiForm)
	{
		m_szCachedAdjective.push_back(gDLL->getObjectText(m_szAdjectiveKey, m_szCachedAdjective.size()));
	}

	return m_szCachedAdjective[uiForm];
}

const wchar* CvCivilizationInfo::getAdjectiveKey() const
{
	return m_szAdjectiveKey;
}
const char* CvCivilizationInfo::getFlagTexture() const
{
	return ARTFILEMGR.getCivilizationArtInfo( getArtDefineTag() )->getPath();
}
const char* CvCivilizationInfo::getArtDefineTag() const
{
	return m_szArtDefineTag;
}
void CvCivilizationInfo::setArtDefineTag(const char* szVal)
{
	m_szArtDefineTag = szVal;
}
// Arrays

/*
	int CvCivilizationInfo::getCivilizationBuildings(int iBuilding) const
	Complexity: O( 1 )
	Purpose:
		Check if a given building, is available in this civilization.
	Parameters:
		int iIndexBuildingClass ... index, that corresponds to the list of BuildingClasses
*/
int CvCivilizationInfo::getCivilizationBuildings(int i) const
{
	FAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiCivilizationBuildings ? m_aiCivilizationBuildings[i] : -1;
}

int CvCivilizationInfo::getCivilizationUnits(int i) const
{
	FAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiCivilizationUnits ? m_aiCivilizationUnits[i] : -1;
}

int CvCivilizationInfo::getNumCivilizationFreeUnits() const
{
	return m_aCivilizationFreeUnits.size();
}
int CvCivilizationInfo::getCivilizationFreeUnitsClass(int index) const
{
	FAssert(index < (int) m_aCivilizationFreeUnits.size());
	FAssert(index > -1);
	return m_aCivilizationFreeUnits[index].first;
}
int CvCivilizationInfo::getCivilizationFreeUnitsProfession(int index) const
{
	FAssert(index < (int) m_aCivilizationFreeUnits.size());
	FAssert(index > -1);
	return m_aCivilizationFreeUnits[index].second;
}
int CvCivilizationInfo::getCivilizationInitialCivics(int i) const
{
	FAssertMsg(i < GC.getNumCivicOptionInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiCivilizationInitialCivics ? m_aiCivilizationInitialCivics[i] : -1;
}
int CvCivilizationInfo::getFreeYields(int i) const
{
	FAssert(i < NUM_YIELD_TYPES && i >= 0);
	return m_aiFreeYields ? m_aiFreeYields[i] : -1;
}
bool CvCivilizationInfo::isLeaders(int i) const
{
	FAssertMsg(i < GC.getNumLeaderHeadInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abLeaders ? m_abLeaders[i] : false;
}
bool CvCivilizationInfo::isCivilizationFreeBuildingClass(int i) const
{
	FAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abCivilizationFreeBuildingClass ? m_abCivilizationFreeBuildingClass[i] : false;
}
bool CvCivilizationInfo::isValidProfession(int i) const
{
	FAssertMsg(i < GC.getNumProfessionInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abValidProfessions ? m_abValidProfessions[i] : false;
}
bool CvCivilizationInfo::hasTrait(int i) const
{
	FAssertMsg(i < GC.getNumTraitInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abTraits ? m_abTraits[i] : false;
}
int CvCivilizationInfo::getTeachUnitClassWeight(int i) const
{
	FAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiTeachUnitClassWeights ? m_aiTeachUnitClassWeights[i] : false;
}
const CvArtInfoCivilization* CvCivilizationInfo::getArtInfo() const
{
	return ARTFILEMGR.getCivilizationArtInfo( getArtDefineTag() );
}
const char* CvCivilizationInfo::getButton() const
{
	return getArtInfo()->getButton();
}
CvWString CvCivilizationInfo::getCityNames(int i) const
{
	FAssertMsg(i < getNumCityNames(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	CvWString tag;
	tag.Format(L"%s_CITY_%d", m_szTextKey.GetCString(), i);
	return gDLL->getText(tag);
}

// TAC - Great General Names - Ray - START
CvWString CvCivilizationInfo::getGeneralNames(int i) const
{
	FAssertMsg(i < getNumGeneralNames(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	CvWString tag;
	tag.Format(L"%s_GENERAL_%d", m_szTextKey.GetCString(), i);
	return gDLL->getText(tag);
}
// TAC - Great General Names - Ray - END

// R&R, ray, Great Admirals - START
CvWString CvCivilizationInfo::getAdmiralNames(int i) const
{
	FAssertMsg(i < getNumAdmiralNames(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	CvWString tag;
	tag.Format(L"%s_ADMIRAL_%d", m_szTextKey.GetCString(), i);
	return gDLL->getText(tag);
}
// R&R, ray, Great Admirals - END

// TAC - Ship Names - Ray - START
CvWString CvCivilizationInfo::getShipNames(int i) const
{
	FAssertMsg(i < getNumShipNames(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	CvWString tag;
	tag.Format(L"%s_SHIP_%d", m_szTextKey.GetCString(), i);
	return gDLL->getText(tag);
}
// TAC - Ship Names - Ray - END

int CvCivilizationInfo::getDerivativeCiv() const
{
	return m_iDerivativeCiv;
}
void CvCivilizationInfo::setDerivativeCiv(int iCiv)
{
	m_iDerivativeCiv = iCiv;
}
void CvCivilizationInfo::read(FDataStreamBase* stream)
{
	CvInfoBase::read(stream);

	uint uiFlag=0;
	stream->Read(&uiFlag);		// flag for expansion
	stream->Read(&m_iDefaultPlayerColor);
	stream->Read(&m_iArtStyleType);
	//Androrc UnitArtStyles
	stream->Read(&m_iUnitArtStyleType);   // FlavorUnits by Impaler[WrG]
	//Androrc End
	stream->Read(&m_iNumCityNames);
	stream->Read(&m_iNumGeneralNames); // TAC - Great General Names - Ray - START
	stream->Read(&m_iNumAdmiralNames); // R&R, ray, Great Admirals - START
	stream->Read(&m_iNumShipNames); // TAC - Ship Names - Ray - START
	stream->Read(&m_iNumLeaders);
	stream->Read(&m_iSelectionSoundScriptId);
	stream->Read(&m_iActionSoundScriptId);
	stream->Read(&m_iDerivativeCiv);
	stream->Read(&m_iAdvancedStartPoints);
	stream->Read(&m_iAreaMultiplier);
	stream->Read(&m_iDensityMultiplier);
	stream->Read(&m_iTreasure);
	stream->Read(&m_iFavoredTerrain);
	stream->Read(&m_iCapturedCityUnitClass);
	stream->Read(&m_eDefaultProfession);
	stream->Read(&m_iMissionaryChar);
	stream->Read(&m_bAIPlayable);
	stream->Read(&m_bPlayable);
	stream->Read(&m_bWaterStart);
	stream->Read(&m_bOpenBorders);
	stream->Read(&m_bWaterWorks);
	stream->Read(&m_bEurope);
	stream->Read(&m_bNative);
	// R&R, ray, Correct Geographical Placement of Natives - START
	stream->Read(&m_bNorthAmericanNative);
	stream->Read(&m_bSouthAmericanNative);
	stream->Read(&m_bCentralAmericanNative);
	// R&R, ray, Correct Geographical Placement of Natives - END
	stream->ReadString(m_szArtDefineTag);
	stream->ReadString(m_szShortDescriptionKey);
	stream->ReadString(m_szAdjectiveKey);
	// Arrays
	SAFE_DELETE_ARRAY(m_aiCivilizationBuildings);
	m_aiCivilizationBuildings = new int[GC.getNumBuildingClassInfos()];
	stream->Read(GC.getNumBuildingClassInfos(), m_aiCivilizationBuildings);
	SAFE_DELETE_ARRAY(m_aiCivilizationUnits);
	m_aiCivilizationUnits = new int[GC.getNumUnitClassInfos()];
	stream->Read(GC.getNumUnitClassInfos(), m_aiCivilizationUnits);
	m_aCivilizationFreeUnits.clear();
	int iSize;
	stream->Read(&iSize);
	for(int i=0;i<iSize;i++)
	{
		int iUnitClass;
		stream->Read(&iUnitClass);
		int iProfession;
		stream->Read(&iProfession);
		m_aCivilizationFreeUnits.push_back(std::make_pair((UnitClassTypes) iUnitClass, (ProfessionTypes) iProfession));
	}

	SAFE_DELETE_ARRAY(m_aiCivilizationInitialCivics);
	m_aiCivilizationInitialCivics = new int[GC.getNumCivicOptionInfos()];
	stream->Read(GC.getNumCivicOptionInfos(), m_aiCivilizationInitialCivics);

	SAFE_DELETE_ARRAY(m_aiFreeYields);
	m_aiFreeYields = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_aiFreeYields);

	SAFE_DELETE_ARRAY(m_aiTeachUnitClassWeights);
	m_aiTeachUnitClassWeights = new int[GC.getNumUnitClassInfos()];
	stream->Read(GC.getNumUnitClassInfos(), m_aiTeachUnitClassWeights);

	SAFE_DELETE_ARRAY(m_abLeaders);
	m_abLeaders = new bool[GC.getNumLeaderHeadInfos()];
	stream->Read(GC.getNumLeaderHeadInfos(), m_abLeaders);

	SAFE_DELETE_ARRAY(m_abCivilizationFreeBuildingClass);
	m_abCivilizationFreeBuildingClass = new bool[GC.getNumBuildingClassInfos()];
	stream->Read(GC.getNumBuildingClassInfos(), m_abCivilizationFreeBuildingClass);

	SAFE_DELETE_ARRAY(m_abValidProfessions);
	m_abValidProfessions = new bool[GC.getNumProfessionInfos()];
	stream->Read(GC.getNumProfessionInfos(), m_abValidProfessions);

	SAFE_DELETE_ARRAY(m_abTraits);
	m_abTraits = new bool[GC.getNumTraitInfos()];
	stream->Read(GC.getNumTraitInfos(), m_abTraits);
}
void CvCivilizationInfo::write(FDataStreamBase* stream)
{
	CvInfoBase::write(stream);
	uint uiFlag=0;
	stream->Write(uiFlag);		// flag for expansion
	stream->Write(m_iDefaultPlayerColor);
	stream->Write(m_iArtStyleType);
	stream->Write(m_iUnitArtStyleType); //Androrc UnitArtStyles
	stream->Write(m_iNumCityNames);
	stream->Write(m_iNumGeneralNames);  // TAC - Great General Names - Ray - START
	stream->Write(m_iNumAdmiralNames);  // R&R, ray, Great Admirals - START
	stream->Write(m_iNumShipNames); // TAC - Ship Names - Ray - START
	stream->Write(m_iNumLeaders);
	stream->Write(m_iSelectionSoundScriptId);
	stream->Write(m_iActionSoundScriptId);
	stream->Write(m_iDerivativeCiv);
	stream->Write(m_iAdvancedStartPoints);
	stream->Write(m_iAreaMultiplier);
	stream->Write(m_iDensityMultiplier);
	stream->Write(m_iTreasure);
	stream->Write(m_iFavoredTerrain);
	stream->Write(m_iCapturedCityUnitClass);
	stream->Write(m_eDefaultProfession);
	stream->Write(m_iMissionaryChar);
	stream->Write(m_bAIPlayable);
	stream->Write(m_bPlayable);
	stream->Write(m_bWaterStart);
	stream->Write(m_bOpenBorders);
	stream->Write(m_bWaterWorks);
	stream->Write(m_bEurope);
	stream->Write(m_bNative);
	// R&R, ray, Correct Geographical Placement of Natives - START
	stream->Write(m_bNorthAmericanNative);
	stream->Write(m_bSouthAmericanNative);
	stream->Write(m_bCentralAmericanNative);
	// R&R, ray, Correct Geographical Placement of Natives - END
	stream->WriteString(m_szArtDefineTag);
	stream->WriteString(m_szShortDescriptionKey);
	stream->WriteString(m_szAdjectiveKey);
	// Arrays
	stream->Write(GC.getNumBuildingClassInfos(), m_aiCivilizationBuildings);
	stream->Write(GC.getNumUnitClassInfos(), m_aiCivilizationUnits);
	stream->Write((int)m_aCivilizationFreeUnits.size());
	for(int i=0;i<(int)m_aCivilizationFreeUnits.size();i++)
	{
		stream->Write(m_aCivilizationFreeUnits[i].first);
		stream->Write(m_aCivilizationFreeUnits[i].second);
	}
	stream->Write(GC.getNumCivicOptionInfos(), m_aiCivilizationInitialCivics);
	stream->Write(NUM_YIELD_TYPES, m_aiFreeYields);
	stream->Write(GC.getNumUnitClassInfos(), m_aiTeachUnitClassWeights);
	stream->Write(GC.getNumLeaderHeadInfos(), m_abLeaders);
	stream->Write(GC.getNumBuildingClassInfos(), m_abCivilizationFreeBuildingClass);
	stream->Write(GC.getNumProfessionInfos(), m_abValidProfessions);
	stream->Write(GC.getNumTraitInfos(), m_abTraits);
}

bool CvCivilizationInfo::read(CvXMLLoadUtility* pXML)
{
	char szClassVal[256];					// holds the text value of the relevant classinfo
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	int j, iNumSibs;
	pXML->GetChildXmlValByName(m_szShortDescriptionKey, "ShortDescription");
	pXML->GetChildXmlValByName(m_szAdjectiveKey, "Adjective");
	pXML->GetChildXmlValByName(szTextVal, "DefaultPlayerColor");
	m_iDefaultPlayerColor = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "ArtDefineTag");
	setArtDefineTag(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "ArtStyleType");
	m_iArtStyleType = GC.getInfoTypeForString(szTextVal);
	//Androrc UnitArtStyles
	pXML->GetChildXmlValByName(szTextVal, "UnitArtStyleType");
	m_iUnitArtStyleType = pXML->FindInInfoClass(szTextVal);
	//Androrc End
	pXML->GetChildXmlValByName(szTextVal, "CivilizationSelectionSound");
	m_iSelectionSoundScriptId = (szTextVal.GetLength() > 0) ? gDLL->getAudioTagIndex( szTextVal.GetCString(), AUDIOTAG_3DSCRIPT ) : -1;
	pXML->GetChildXmlValByName(szTextVal, "CivilizationActionSound");
	m_iActionSoundScriptId = (szTextVal.GetLength() > 0) ? gDLL->getAudioTagIndex( szTextVal.GetCString(), AUDIOTAG_3DSCRIPT ) : -1;
	pXML->GetChildXmlValByName(&m_iAdvancedStartPoints, "iAdvancedStartPoints");
	pXML->GetChildXmlValByName(&m_iAreaMultiplier, "iAreaMultiplier");
	pXML->GetChildXmlValByName(&m_iDensityMultiplier, "iDensityMultiplier");
	pXML->GetChildXmlValByName(&m_iTreasure, "iTreasure");
	pXML->GetChildXmlValByName(szTextVal, "FavoredTerrain");
	m_iFavoredTerrain = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "CapturedCityUnitClass");
	m_iCapturedCityUnitClass = pXML->FindInInfoClass(szTextVal);
	pXML->GetEnum(getType(), m_eDefaultProfession, "DefaultProfession", false);
	// set the current xml node to it's next sibling and then
	pXML->GetChildXmlValByName(&m_bPlayable, "bPlayable");
	pXML->GetChildXmlValByName(&m_bAIPlayable, "bAIPlayable");
	pXML->GetChildXmlValByName(&m_bWaterStart, "bWaterStart");
	pXML->GetChildXmlValByName(&m_bOpenBorders, "bOpenBorders");
	pXML->GetChildXmlValByName(&m_bWaterWorks, "bWaterWorks");
	pXML->GetChildXmlValByName(&m_bEurope, "bEurope");
	pXML->GetChildXmlValByName(&m_bNative, "bNative");
	// R&R, ray, Correct Geographical Placement of Natives - START
	pXML->GetChildXmlValByName(&m_bNorthAmericanNative, "bNorthAmericanNative");
	pXML->GetChildXmlValByName(&m_bSouthAmericanNative, "bSouthAmericanNative");
	pXML->GetChildXmlValByName(&m_bCentralAmericanNative, "bCentralAmericanNative");
	// R&R, ray, Correct Geographical Placement of Natives - END

	pXML->GetEnum(getType(), m_eCivCategory, "eCivCategory");

	pXML->GetEnum(getType(), m_eCivEffect, "eCivEffect", false);

	// if we can set the current xml node to it's next sibling
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"Buildings"))
	{
		// pXML->Skip any comments and stop at the next value we might want
		if (pXML->SkipToNextVal())
		{
			// call the function that sets the default civilization buildings
			pXML->InitBuildingDefaults(&m_aiCivilizationBuildings);
			// get the total number of children the current xml node has
			iNumSibs = gDLL->getXMLIFace()->GetNumChildren(pXML->GetXML());
			// if the call to the function that sets the current xml node to it's first non-comment
			// child and sets the parameter with the new node's value succeeds
			if ( (0 < iNumSibs) && (gDLL->getXMLIFace()->SetToChild(pXML->GetXML())) )
			{
				int iBuildingClassIndex;
				FAssertMsg((iNumSibs <= GC.getNumBuildingClassInfos()) ,"In SetGlobalCivilizationInfo iNumSibs is greater than GC.getNumBuildingClassInfos()");
				// loop through all the siblings
				for (j=0;j<iNumSibs;j++)
				{
					if (pXML->GetChildXmlVal(szClassVal))
					{
						// get the index into the array based on the building class type
						iBuildingClassIndex = pXML->FindInInfoClass(szClassVal);
						if (-1 < iBuildingClassIndex)
						{
							// get the next value which should be the building type to set this civilization's version of this building class too
							pXML->GetNextXmlVal( &szTextVal);
							// call the find in list function to return either -1 if no value is found
							// or the index in the list the match is found at
							m_aiCivilizationBuildings[iBuildingClassIndex] = pXML->FindInInfoClass(szTextVal);
						}
						else
						{
							FAssertMsg(0,"BuildingClass index is -1 in SetGlobalCivilizationInfo function");
						}
						// set the current xml node to it's parent node
						gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
					}
					// if the call to the function that sets the current xml node to it's first non-comment
					// sibling and sets the parameter with the new node's value does not succeed
					// we will break out of this for loop
					if (!gDLL->getXMLIFace()->NextSibling(pXML->GetXML()))
					{
						break;
					}
				}
				// set the current xml node to it's parent node
				gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
			}
		}
		// set the current xml node to it's parent node
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	// if we can set the current xml node to it's next sibling
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"Units"))
	{
		// pXML->Skip any comments and stop at the next value we might want
		if (pXML->SkipToNextVal())
		{
			// call the function that sets the default civilization buildings
			pXML->InitUnitDefaults(&m_aiCivilizationUnits);
			// get the total number of children the current xml node has
			iNumSibs = gDLL->getXMLIFace()->GetNumChildren(pXML->GetXML());
			// if the call to the function that sets the current xml node to it's first non-comment
			// child and sets the parameter with the new node's value succeeds
			if ( (0 < iNumSibs) && (gDLL->getXMLIFace()->SetToChild(pXML->GetXML())) )
			{
				int iUnitClassIndex;
				FAssertMsg((iNumSibs <= GC.getNumUnitClassInfos()),"In SetGlobalCivilizationInfo iNumSibs is greater than GC.getNumUnitClassInfos()");
				// loop through all the siblings
				for (j=0;j<iNumSibs;j++)
				{
					if (pXML->GetChildXmlVal(szClassVal))
					{
						// set the unit class index
						iUnitClassIndex = pXML->FindInInfoClass(szClassVal);
						if (-1 < iUnitClassIndex)
						{
							// get the next value which should be the building type to set this civilization's version of this building class too
							pXML->GetNextXmlVal( &szTextVal);
							// call the find in list function to return either -1 if no value is found
							// or the index in the list the match is found at
							m_aiCivilizationUnits[iUnitClassIndex] = pXML->FindInInfoClass(szTextVal);
						}
						else
						{
							FAssertMsg(0, "UnitClass index is -1 in SetGlobalCivilizationInfo function");
						}
						// set the current xml node to it's parent node
						gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
					}
					// if the call to the function that sets the current xml node to it's first non-comment
					// sibling and sets the parameter with the new node's value does not succeed
					// we will break out of this for loop
					if (!gDLL->getXMLIFace()->NextSibling(pXML->GetXML()))
					{
						break;
					}
				}
				// set the current xml node to it's parent node
				gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
			}
		}
		// set the current xml node to it's parent node
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	// if we can set the current xml node to it's next sibling
	m_aCivilizationFreeUnits.clear();
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"FreeUnitClasses"))
	{
		if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"FreeUnitClass"))
		{
			do
			{
				pXML->GetChildXmlValByName(szTextVal, "UnitClassType");
				int iUnitClass = pXML->FindInInfoClass(szTextVal);
				pXML->GetChildXmlValByName(szTextVal, "FreeUnitProfession");
				int iProfession = pXML->FindInInfoClass(szTextVal);
				m_aCivilizationFreeUnits.push_back(std::make_pair((UnitClassTypes) iUnitClass, (ProfessionTypes) iProfession));
			} while(gDLL->getXMLIFace()->NextSibling(pXML->GetXML()));
			// set the current xml node to it's parent node
			gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		}
		// set the current xml node to it's parent node
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	pXML->SetVariableListTagPair(&m_abCivilizationFreeBuildingClass, "FreeBuildingClasses", GC.getNumBuildingClassInfos(), false);
	pXML->SetVariableListTagPair(&m_abValidProfessions, "Professions", GC.getNumProfessionInfos(), true);
	pXML->SetVariableListTagPair(&m_abTraits, "Traits", GC.getNumTraitInfos(), false);
	pXML->SetVariableListTagPair(&m_aiTeachUnitClassWeights, "TeachUnitClasses", GC.getNumUnitClassInfos(), 0);
	pXML->SetVariableListTagPair(&m_aiFreeYields, "FreeYields", NUM_YIELD_TYPES, 0);
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"InitialCivics"))
	{
		if (pXML->SkipToNextVal())
		{
			iNumSibs = gDLL->getXMLIFace()->GetNumChildren(pXML->GetXML());
			pXML->InitList(&m_aiCivilizationInitialCivics, GC.getNumCivicOptionInfos(), (int)NO_CIVIC);
			if (0 < iNumSibs)
			{
				if (pXML->GetChildXmlVal(szTextVal))
				{
					FAssertMsg((iNumSibs <= GC.getNumCivicOptionInfos()),"For loop iterator is greater than array size");
					for (j=0;j<iNumSibs;j++)
					{
						m_aiCivilizationInitialCivics[j] = pXML->FindInInfoClass(szTextVal);
						if (!pXML->GetNextXmlVal(&szTextVal))
						{
							break;
						}
					}
					gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
				}
			}
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	pXML->SetVariableListTagPair(&m_abLeaders, "Leaders", GC.getNumLeaderHeadInfos(), false);
	pXML->GetChildXmlValByName(szTextVal, "CivilizationSelectionSound");

	return true;
}

bool CvCivilizationInfo::postLoadSetup()
{
	// setlength of lists of names based on available TXT_KEYs
	CvWString tag;
	CvWString text;

	for (;; ++m_iNumAdmiralNames)
	{
		tag.Format(L"%s_ADMIRAL_%d", m_szTextKey.GetCString(), m_iNumAdmiralNames);
		text = gDLL->getText(tag);
		if (tag == text) break;
	}
	for (;; ++m_iNumCityNames)
	{
		tag.Format(L"%s_CITY_%d", m_szTextKey.GetCString(), m_iNumCityNames);
		text = gDLL->getText(tag);
		if (tag == text) break;
	}
	for (;; ++m_iNumGeneralNames)
	{
		tag.Format(L"%s_GENERAL_%d", m_szTextKey.GetCString(), m_iNumGeneralNames);
		text = gDLL->getText(tag);
		if (tag == text) break;
	}
	for (;; ++m_iNumShipNames)
	{
		tag.Format(L"%s_SHIP_%d", m_szTextKey.GetCString(), m_iNumShipNames);
		text = gDLL->getText(tag);
		if (tag == text) break;
	}
	return true;
}

bool CvCivilizationInfo::readPass2(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	pXML->GetChildXmlValByName(szTextVal, "DerivativeCiv");
	m_iDerivativeCiv = GC.getInfoTypeForString(szTextVal);
	return true;
}

int CvCivilizationInfo::PY_getDefaultProfession() const
{
	return getDefaultProfession();
}

//======================================================================================================
//					CvVictoryInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvVictoryInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvVictoryInfo::CvVictoryInfo() :
// PatchMod: Victorys START
m_iTotalProductionRate(0),
// PatchMod: Victorys END
m_iPopulationPercentLead(0),
m_iLandPercent(0),
m_iMinLandPercent(0),
m_iCityCulture(0),
m_iNumCultureCities(0),
m_iTotalCultureRatio(0),
m_bDefault(false),
m_bTargetScore(false),
m_bEndEurope(false),
m_bEndScore(false),
m_bConquest(false),
m_bPermanent(false),
m_bRevolution(false)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvVictoryInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvVictoryInfo::~CvVictoryInfo()
{
}
int CvVictoryInfo::getPopulationPercentLead() const
{
	return m_iPopulationPercentLead;
}
int CvVictoryInfo::getLandPercent() const
{
	return m_iLandPercent;
}
int CvVictoryInfo::getMinLandPercent() const
{
	return m_iMinLandPercent;
}
int CvVictoryInfo::getCityCulture() const
{
	return m_iCityCulture;
}
int CvVictoryInfo::getNumCultureCities() const
{
	return m_iNumCultureCities;
}
int CvVictoryInfo::getTotalCultureRatio() const
{
	return m_iTotalCultureRatio;
}
bool CvVictoryInfo::getDefault() const
{
	return m_bDefault;
}
bool CvVictoryInfo::isTargetScore() const
{
	return m_bTargetScore;
}
bool CvVictoryInfo::isEndEurope() const
{
	return m_bEndEurope;
}
bool CvVictoryInfo::isEndScore() const
{
	return m_bEndScore;
}
bool CvVictoryInfo::isConquest() const
{
	return m_bConquest;
}
bool CvVictoryInfo::isPermanent() const
{
	return m_bPermanent;
}
bool CvVictoryInfo::isRevolution() const
{
	return m_bRevolution;
}
const char* CvVictoryInfo::getMovie() const
{
	return m_szMovie;
}

//
// read from xml
//
bool CvVictoryInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(&m_bDefault, "bDefault");
	pXML->GetChildXmlValByName(&m_bTargetScore, "bTargetScore");
	pXML->GetChildXmlValByName(&m_bEndEurope, "bEndEurope");
	pXML->GetChildXmlValByName(&m_bEndScore, "bEndScore");
	pXML->GetChildXmlValByName(&m_bConquest, "bConquest");
	pXML->GetChildXmlValByName(&m_bPermanent, "bPermanent");
	pXML->GetChildXmlValByName(&m_bRevolution, "bRevolution");
	pXML->GetChildXmlValByName(&m_iPopulationPercentLead, "iPopulationPercentLead");
	pXML->GetChildXmlValByName(&m_iLandPercent, "iLandPercent");
	pXML->GetChildXmlValByName(&m_iMinLandPercent, "iMinLandPercent");
	pXML->GetChildXmlValByName(szTextVal, "CityCulture");
	m_iCityCulture = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(&m_iNumCultureCities, "iNumCultureCities");
	pXML->GetChildXmlValByName(&m_iTotalCultureRatio, "iTotalCultureRatio");

	// PatchMod: Victorys START
	pXML->GetChildXmlValByName(&m_iTotalProductionRate, "iTotalProductionRate");
	// PatchMod: Victorys END

	pXML->GetChildXmlValByName(m_szMovie, "VictoryMovie");
	return true;
}
//======================================================================================================
//					CvHurryInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvHurryInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvHurryInfo::CvHurryInfo() :
	m_iGoldPerProduction(0),
	m_iProductionPerPopulation(0),
	m_iGoldPerCross(0),
	m_iYieldCostEuropePercent(0),
	m_eProductionYieldConsumed(NO_YIELD),
	m_iProductionYieldPercent(0),
	m_iFlatGold(0),
	m_bStarting(false),
	m_bCity(false)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvHurryInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvHurryInfo::~CvHurryInfo()
{
}
int CvHurryInfo::getGoldPerProduction() const
{
	return m_iGoldPerProduction;
}
int CvHurryInfo::getProductionPerPopulation() const
{
	return m_iProductionPerPopulation;
}
int CvHurryInfo::getGoldPerCross() const
{
	return m_iGoldPerCross;
}
int CvHurryInfo::getYieldCostEuropePercent() const
{
	return m_iYieldCostEuropePercent;
}
YieldTypes CvHurryInfo::getProductionYieldConsumed() const
{
	return m_eProductionYieldConsumed;
}
int CvHurryInfo::getProductionYieldPercent() const
{
	return m_iProductionYieldPercent;
}
int CvHurryInfo::getFlatGold() const
{
	return m_iFlatGold;
}
bool CvHurryInfo::isStarting() const
{
	return m_bStarting;
}
bool CvHurryInfo::isCity() const
{
	return m_bCity;
}
bool CvHurryInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(&m_iGoldPerProduction, "iGoldPerProduction");
	pXML->GetChildXmlValByName(&m_iProductionPerPopulation, "iProductionPerPopulation");
	pXML->GetChildXmlValByName(&m_iGoldPerCross, "iGoldPerCross");
	pXML->GetChildXmlValByName(&m_iYieldCostEuropePercent, "iYieldCostEuropePercent");
	pXML->GetEnum(getType(), m_eProductionYieldConsumed, "ProductionYieldConsumed", false);
	pXML->GetChildXmlValByName(&m_iProductionYieldPercent, "iProductionYieldPercent");
	pXML->GetChildXmlValByName(&m_iFlatGold, "iFlatGold");
	pXML->GetChildXmlValByName(&m_bStarting, "bStarting");
	pXML->GetChildXmlValByName(&m_bCity, "bCity");
	return true;
}

int CvHurryInfo::PYgetProductionYieldConsumed() const
{
	return getProductionYieldConsumed();
}
//======================================================================================================
//					CvHandicapInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvHandicapInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvHandicapInfo::CvHandicapInfo() :

// TAC: ray Starting Location Start
m_iStartingLocationPercent(0),
// TAC: ray Starting Location End

/** NBMOD TAX **/
m_iInitMaxTaxRate(50),
m_iMaxGoldAskPercent(50),
m_iMinGoldAskPercent(25),
/** NBMOD TAX**/

/** NBMOD REF **/
m_iNBMOD_REF_Weight(100),
m_iOppressometerGrowthHandicap(100),
/** NBMOD REF **/
m_iAdvancedStartPointsMod(0),
m_iStartingGold(0),
m_iFatherPercent(0),
m_iAttitudeChange(0),
m_iStartingDefenseUnits(0),
m_iStartingWorkerUnits(0),
m_iStartingExploreUnits(0),
m_iAIStartingUnitMultiplier(0),
m_iAIStartingDefenseUnits(0),
m_iAIStartingWorkerUnits(0),
m_iAIStartingExploreUnits(0),
m_iAIDeclareWarProb(0),
m_iAIWorkRateModifier(0),
m_iAINativeCombatModifier(0),
m_iAIKingCombatModifier(0),
m_iAIRebelModifier(0),
m_iAIGrowthPercent(0),
m_iAITrainPercent(0),
m_iAIConstructPercent(0),
m_iAIUnitUpgradePercent(0),
m_iAIHurryPercent(0),
// TAC - AI More Immigrants - koma13 - START
m_iAIImmigration(0),
m_iAIMinimumStorageLossSellPercentage(0),
m_iAIMaxTaxrate(0),
// TAC - AI More Immigrants - koma13 - END
m_iAIExtraTradePercent(0),
m_iAIPerEraModifier(0),
m_iAIAdvancedStartPercent(0),
m_iAIKingUnitThresholdPercent(0),
// < JAnimals Mod Start >
m_iAIAnimalLandMaxPercent(0),
m_iAIAnimalSeaMaxPercent(0),
m_iAIAnimalLandNumTurnsNoSpawn(0),
m_iAIAnimalSeaNumTurnsNoSpawn(0),
// < JAnimals Mod End >
m_iKingGoldThresholdPercent(0),
m_iNumGoodies(0),
m_iEuropePriceThresholdMultiplier(0),
m_iNativePacifismPercent(0),
m_iMissionFailureThresholdPercent(0),
m_iKingNumUnitMultiplier(0),
m_iWaveTurns(0),	// TAC - AI Revolution - koma13
m_aiGoodies(NULL)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvHandicapInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvHandicapInfo::~CvHandicapInfo()
{
	SAFE_DELETE_ARRAY(m_aiGoodies);
}

// TAC: ray Starting Location Start
int CvHandicapInfo::getStartingLocationPercent() const
{
	return m_iStartingLocationPercent;
}
// TAC: ray Starting Location End

/** NBMOD REF **/
int CvHandicapInfo::NBMOD_GetREFWeight() const
{
    return m_iNBMOD_REF_Weight;
}
/** NBMOD REF **/

/** NBMOD TAX **/

int CvHandicapInfo::NBMOD_GetInitMaxTaxRate() const
{
    return m_iInitMaxTaxRate;
}

int CvHandicapInfo::NBMOD_GetMaxGoldAskPercent() const
{
    return m_iMaxGoldAskPercent;
}

int CvHandicapInfo::NBMOD_GetMinGoldAskPercent() const
{
    return m_iMinGoldAskPercent;
}

/** NBMOD TAX **/

int CvHandicapInfo::getAdvancedStartPointsMod() const
{
	return m_iAdvancedStartPointsMod;
}
int CvHandicapInfo::getStartingGold() const
{
	return m_iStartingGold;
}
int CvHandicapInfo::getFatherPercent() const
{
	return m_iFatherPercent;
}
int CvHandicapInfo::getAttitudeChange() const
{
	return m_iAttitudeChange;
}
int CvHandicapInfo::getStartingDefenseUnits() const
{
	return m_iStartingDefenseUnits;
}
int CvHandicapInfo::getStartingWorkerUnits() const
{
	return m_iStartingWorkerUnits;
}
int CvHandicapInfo::getStartingExploreUnits() const
{
	return m_iStartingExploreUnits;
}
int CvHandicapInfo::getAIStartingUnitMultiplier() const
{
	return m_iAIStartingUnitMultiplier;
}
int CvHandicapInfo::getAIStartingDefenseUnits() const
{
	return m_iAIStartingDefenseUnits;
}
int CvHandicapInfo::getAIStartingWorkerUnits() const
{
	return m_iAIStartingWorkerUnits;
}
int CvHandicapInfo::getAIStartingExploreUnits() const
{
	return m_iAIStartingExploreUnits;
}
int CvHandicapInfo::getAIDeclareWarProb() const
{
	return m_iAIDeclareWarProb;
}
int CvHandicapInfo::getAIWorkRateModifier() const
{
	return m_iAIWorkRateModifier;
}
int CvHandicapInfo::getAINativeCombatModifier() const
{
	return m_iAINativeCombatModifier;
}
int CvHandicapInfo::getAIKingCombatModifier() const
{
	return m_iAIKingCombatModifier;
}
int CvHandicapInfo::getAIRebelModifier() const
{
	return m_iAIRebelModifier;
}
int CvHandicapInfo::getAIGrowthPercent() const
{
	return m_iAIGrowthPercent;
}
int CvHandicapInfo::getAITrainPercent() const
{
	return m_iAITrainPercent;
}
int CvHandicapInfo::getAIConstructPercent() const
{
	return m_iAIConstructPercent;
}
int CvHandicapInfo::getAIUnitUpgradePercent() const
{
	return m_iAIUnitUpgradePercent;
}
int CvHandicapInfo::getAIHurryPercent() const
{
	return m_iAIHurryPercent;
}
int CvHandicapInfo::getAIExtraTradePercent() const
{
	return m_iAIExtraTradePercent;
}
int CvHandicapInfo::getAIPerEraModifier() const
{
	return m_iAIPerEraModifier;
}
int CvHandicapInfo::getAIAdvancedStartPercent() const
{
	return m_iAIAdvancedStartPercent;
}
int CvHandicapInfo::getAIKingUnitThresholdPercent() const
{
	return m_iAIKingUnitThresholdPercent;
}

// < JAnimals Mod Start >
int CvHandicapInfo::getAIAnimalLandMaxPercent() const
{
	return m_iAIAnimalLandMaxPercent;
}
int CvHandicapInfo::getAIAnimalSeaMaxPercent() const
{
	return m_iAIAnimalSeaMaxPercent;
}
int CvHandicapInfo::getAIAnimalLandNumTurnsNoSpawn() const
{
	return m_iAIAnimalLandNumTurnsNoSpawn;
}
int CvHandicapInfo::getAIAnimalSeaNumTurnsNoSpawn() const
{
	return m_iAIAnimalSeaNumTurnsNoSpawn;
}
// < JAnimals Mod End >

int CvHandicapInfo::getNumGoodies() const
{
	return m_iNumGoodies;
}
int CvHandicapInfo::getEuropePriceThresholdMultiplier() const
{
	return m_iEuropePriceThresholdMultiplier;
}
int CvHandicapInfo::getNativePacifismPercent() const
{
	return m_iNativePacifismPercent;
}
int CvHandicapInfo::getMissionFailureThresholdPercent() const
{
	return m_iMissionFailureThresholdPercent;
}
int CvHandicapInfo::getKingNumUnitMultiplier() const
{
	return m_iKingNumUnitMultiplier;
}
int CvHandicapInfo::getKingGoldThresholdPercent() const
{
	return m_iKingGoldThresholdPercent;
}

// TAC - AI Revolution - koma13 - START
int CvHandicapInfo::getWaveTurns() const
{
	return m_iWaveTurns;
}
int CvHandicapInfo::getNumWaves()
{
	return m_aWaves.size();
}
int CvHandicapInfo::getWaves(int index)
{
	FAssertMsg(index < getNumWaves(), "Index out of bounds");
	FAssertMsg(index > -1, "Index out of bounds");
	return m_aWaves[index];
}
// TAC - AI Revolution - koma13 - END

// Arrays
int CvHandicapInfo::getGoodies(int i) const
{
	FAssertMsg(i < getNumGoodies(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiGoodies[i];
}
void CvHandicapInfo::read(FDataStreamBase* stream)
{
	CvInfoBase::read(stream);
	uint uiFlag=0;
	// TAC: ray Starting Location Start
	stream->Read(&m_iStartingLocationPercent);
	// TAC: ray Starting Location End

	stream->Read(&uiFlag);		// Flag for Expansion
	stream->Read(&m_iAdvancedStartPointsMod);
	stream->Read(&m_iStartingGold);
	stream->Read(&m_iFatherPercent);
	stream->Read(&m_iAttitudeChange);
	stream->Read(&m_iStartingDefenseUnits);
	stream->Read(&m_iStartingWorkerUnits);
	stream->Read(&m_iStartingExploreUnits);
	stream->Read(&m_iAIStartingUnitMultiplier);
	stream->Read(&m_iAIStartingDefenseUnits);
	stream->Read(&m_iAIStartingWorkerUnits);
	stream->Read(&m_iAIStartingExploreUnits);
	stream->Read(&m_iAIDeclareWarProb);
	stream->Read(&m_iAIWorkRateModifier);
	stream->Read(&m_iAINativeCombatModifier);
	stream->Read(&m_iAIKingCombatModifier);
	stream->Read(&m_iAIRebelModifier);
	stream->Read(&m_iAIGrowthPercent);
	stream->Read(&m_iAITrainPercent);
	stream->Read(&m_iAIConstructPercent);
	stream->Read(&m_iAIUnitUpgradePercent);
	stream->Read(&m_iAIHurryPercent);
	// TAC - AI More Immigrants - koma13 - START
	stream->Read(&m_iAIImmigration);
	stream->Read(&m_iAIMinimumStorageLossSellPercentage);
	stream->Read(&m_iAIMaxTaxrate);
	// TAC - AI More Immigrants - koma13 - END
	stream->Read(&m_iAIExtraTradePercent);
	stream->Read(&m_iAIPerEraModifier);
	stream->Read(&m_iAIAdvancedStartPercent);
	stream->Read(&m_iAIKingUnitThresholdPercent);
	// < JAnimals Mod Start >
	//stream->Read(&m_iAIAnimalLandStartDistance);
	//stream->Read(&m_iAIAnimalSeaStartDistance);
	stream->Read(&m_iAIAnimalLandMaxPercent);
	stream->Read(&m_iAIAnimalSeaMaxPercent);
	stream->Read(&m_iAIAnimalLandNumTurnsNoSpawn);
	stream->Read(&m_iAIAnimalSeaNumTurnsNoSpawn);
	// < JAnimals Mod End >
	stream->Read(&m_iKingGoldThresholdPercent);
	stream->Read(&m_iNumGoodies);
	stream->Read(&m_iEuropePriceThresholdMultiplier);
	stream->Read(&m_iNativePacifismPercent);
	stream->Read(&m_iMissionFailureThresholdPercent);
	stream->Read(&m_iKingNumUnitMultiplier);
	/** NBMOD TAX **/
	stream->Read(&m_iInitMaxTaxRate);
	stream->Read(&m_iMaxGoldAskPercent);
	stream->Read(&m_iMinGoldAskPercent);
	/** NBMOD TAX **/

	/** NBMOD REF **/
	stream->Read(&m_iNBMOD_REF_Weight);
	/** NBMOD REF **/

	stream->Read(&m_iOppressometerGrowthHandicap);

	stream->ReadString(m_szHandicapName);
	stream->Read(&m_iWaveTurns);	// TAC - AI Revolution - koma13

	// Arrays
	SAFE_DELETE_ARRAY(m_aiGoodies);
	m_aiGoodies = new int[getNumGoodies()];
	stream->Read(getNumGoodies(), m_aiGoodies);
}
void CvHandicapInfo::write(FDataStreamBase* stream)
{
	CvInfoBase::write(stream);
	uint uiFlag=0;
	stream->Write(uiFlag);		// Flag for Expansion
	// TAC: ray Starting Location Start
	stream->Write(m_iStartingLocationPercent);
	// TAC: ray Starting Location End
	stream->Write(m_iAdvancedStartPointsMod);
	stream->Write(m_iStartingGold);
	stream->Write(m_iFatherPercent);
	stream->Write(m_iAttitudeChange);
	stream->Write(m_iStartingDefenseUnits);
	stream->Write(m_iStartingWorkerUnits);
	stream->Write(m_iStartingExploreUnits);
	stream->Write(m_iAIStartingUnitMultiplier);
	stream->Write(m_iAIStartingDefenseUnits);
	stream->Write(m_iAIStartingWorkerUnits);
	stream->Write(m_iAIStartingExploreUnits);
	stream->Write(m_iAIDeclareWarProb);
	stream->Write(m_iAIWorkRateModifier);
	stream->Write(m_iAINativeCombatModifier);
	stream->Write(m_iAIKingCombatModifier);
	stream->Write(m_iAIRebelModifier);
	stream->Write(m_iAIGrowthPercent);
	stream->Write(m_iAITrainPercent);
	stream->Write(m_iAIConstructPercent);
	stream->Write(m_iAIUnitUpgradePercent);
	stream->Write(m_iAIHurryPercent);
	// TAC - AI More Immigrants - koma13 - START
	stream->Write(m_iAIImmigration);
	stream->Write(m_iAIMinimumStorageLossSellPercentage);
	stream->Write(m_iAIMaxTaxrate);
	// TAC - AI More Immigrants - koma13 - END
	stream->Write(m_iAIExtraTradePercent);
	stream->Write(m_iAIPerEraModifier);
	stream->Write(m_iAIAdvancedStartPercent);
	stream->Write(m_iAIKingUnitThresholdPercent);
	stream->Write(m_iKingGoldThresholdPercent);
	// < JAnimals Mod Start >
	//stream->Write(m_iAIAnimalLandStartDistance);
	//stream->Write(m_iAIAnimalSeaStartDistance);
	stream->Write(m_iAIAnimalLandMaxPercent);
	stream->Write(m_iAIAnimalSeaMaxPercent);
	stream->Write(m_iAIAnimalLandNumTurnsNoSpawn);
	stream->Write(m_iAIAnimalSeaNumTurnsNoSpawn);
	// < JAnimals Mod End >
	stream->Write(m_iNumGoodies);
	stream->Write(m_iEuropePriceThresholdMultiplier);
	stream->Write(m_iNativePacifismPercent);
	stream->Write(m_iMissionFailureThresholdPercent);
	stream->Write(m_iKingNumUnitMultiplier);
	/** NBMOD TAX **/
	stream->Write(m_iInitMaxTaxRate);
	stream->Write(m_iMaxGoldAskPercent);
	stream->Write(m_iMinGoldAskPercent);
	/** NBMOD TAX **/

	/** NBMOD REF **/
	stream->Write(m_iNBMOD_REF_Weight);
	/** NBMOD REF **/

	stream->Write(m_iOppressometerGrowthHandicap);

	stream->WriteString(m_szHandicapName);

	stream->Write(m_iWaveTurns);	// TAC - AI Revolution - koma13

	// Arrays
	stream->Write(getNumGoodies(), m_aiGoodies);
}
bool CvHandicapInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	int iVal;	// TAC - AI Revolution - koma13
	int j;
	// TAC: ray Starting Location Start
	pXML->GetChildXmlValByName(&m_iStartingLocationPercent, "iStartingLocPercent");
	// TAC: ray Starting Location End

	/** NBMOD TAX **/
	pXML->GetChildXmlValByName(&m_iInitMaxTaxRate, "iInitMaxTaxRate");
	pXML->GetChildXmlValByName(&m_iMaxGoldAskPercent, "iMaxGoldAskPercent");
	pXML->GetChildXmlValByName(&m_iMinGoldAskPercent, "iMinGoldAskPercent");
	/** NBMOD TAX **/

	/** NBMOD REF **/
	pXML->GetChildXmlValByName(&m_iNBMOD_REF_Weight, "iNBMOD_REF_Weight");
	/** NBMOD REF **/
	pXML->GetChildXmlValByName(&m_iOppressometerGrowthHandicap, "iOppressometerGrowthHandicap");
	pXML->GetChildXmlValByName(&m_iAdvancedStartPointsMod, "iAdvancedStartPointsMod");
	pXML->GetChildXmlValByName(&m_iStartingGold, "iGold");
	pXML->GetChildXmlValByName(&m_iFatherPercent, "iFatherPercent");
	pXML->GetChildXmlValByName(&m_iAttitudeChange, "iAttitudeChange");
	pXML->GetChildXmlValByName(&m_iStartingDefenseUnits, "iStartingDefenseUnits");
	pXML->GetChildXmlValByName(&m_iStartingWorkerUnits, "iStartingWorkerUnits");
	pXML->GetChildXmlValByName(&m_iStartingExploreUnits, "iStartingExploreUnits");
	pXML->GetChildXmlValByName(&m_iAIStartingUnitMultiplier, "iAIStartingUnitMultiplier");
	pXML->GetChildXmlValByName(&m_iAIStartingDefenseUnits, "iAIStartingDefenseUnits");
	pXML->GetChildXmlValByName(&m_iAIStartingWorkerUnits, "iAIStartingWorkerUnits");
	pXML->GetChildXmlValByName(&m_iAIStartingExploreUnits, "iAIStartingExploreUnits");
	pXML->GetChildXmlValByName(&m_iAIDeclareWarProb, "iAIDeclareWarProb");
	pXML->GetChildXmlValByName(&m_iAIWorkRateModifier, "iAIWorkRateModifier");
	pXML->GetChildXmlValByName(&m_iAINativeCombatModifier, "iAINativeCombatModifier");
	pXML->GetChildXmlValByName(&m_iAIKingCombatModifier, "iAIKingCombatModifier");
	pXML->GetChildXmlValByName(&m_iAIRebelModifier, "iAIRebelModifier");
	pXML->GetChildXmlValByName(&m_iAIGrowthPercent, "iAIGrowthPercent");
	pXML->GetChildXmlValByName(&m_iAITrainPercent, "iAITrainPercent");
	pXML->GetChildXmlValByName(&m_iAIConstructPercent, "iAIConstructPercent");
	pXML->GetChildXmlValByName(&m_iAIUnitUpgradePercent, "iAIUnitUpgradePercent");
	pXML->GetChildXmlValByName(&m_iAIHurryPercent, "iAIHurryPercent");
	// TAC - AI More Immigrants - koma13 - START
	pXML->GetChildXmlValByName(&m_iAIImmigration, "iAIImmigration");
	pXML->GetChildXmlValByName(&m_iAIMinimumStorageLossSellPercentage, "iAIMinimumStorageLossSellPercentage");
	pXML->GetChildXmlValByName(&m_iAIMaxTaxrate, "iAIMaxTaxrate");
	// TAC - AI More Immigrants - koma13 - END
	pXML->GetChildXmlValByName(&m_iAIExtraTradePercent, "iAIExtraTradePercent");
	pXML->GetChildXmlValByName(&m_iAIPerEraModifier, "iAIPerEraModifier");
	pXML->GetChildXmlValByName(&m_iAIAdvancedStartPercent, "iAIAdvancedStartPercent");
	pXML->GetChildXmlValByName(&m_iAIKingUnitThresholdPercent, "iAIKingUnitThresholdPercent");
	// < JAnimals Mod Start >
	pXML->GetChildXmlValByName(&m_iAIAnimalLandMaxPercent, "iAIAnimalLandMaxPercent", 0);
	pXML->GetChildXmlValByName(&m_iAIAnimalSeaMaxPercent, "iAIAnimalSeaMaxPercent", 0);
	pXML->GetChildXmlValByName(&m_iAIAnimalLandNumTurnsNoSpawn, "iAIAnimalLandNumTurnsNoSpawn", 0);
	pXML->GetChildXmlValByName(&m_iAIAnimalSeaNumTurnsNoSpawn, "iAIAnimalSeaNumTurnsNoSpawn", 0);
	// < JAnimals Mod End >
	pXML->GetChildXmlValByName(&m_iKingGoldThresholdPercent, "iKingGoldThresholdPercent");
	pXML->GetChildXmlValByName(&m_iEuropePriceThresholdMultiplier, "iEuropePriceThresholdMultiplier");
	pXML->GetChildXmlValByName(&m_iNativePacifismPercent, "iNativePacifismPercent");
	pXML->GetChildXmlValByName(&m_iMissionFailureThresholdPercent, "iMissionFailureThresholdPercent");
	pXML->GetChildXmlValByName(&m_iKingNumUnitMultiplier, "iKingNumUnitMultiplier");
	// TAC - AI Revolution - koma13 - START
	pXML->GetChildXmlValByName(&m_iWaveTurns, "iWaveTurns");
	if(gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "Waves"))
	{
		if(gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "Wave"))
		{
			do
			{
				pXML->GetXmlVal(&iVal);
				m_aWaves.push_back(iVal);
			} while(gDLL->getXMLIFace()->LocateNextSiblingNodeByTagName(pXML->GetXML(), "Wave"));
			gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	// TAC - AI Revolution - koma13 - END

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "Goodies"))
	{
		CvString* pszGoodyNames = NULL;
		pXML->SetStringList(&pszGoodyNames, &m_iNumGoodies);
		if (m_iNumGoodies > 0)
		{
			m_aiGoodies = new int[m_iNumGoodies];
			for (j=0;j<m_iNumGoodies;j++)
			{
				m_aiGoodies[j] = pXML->FindInInfoClass(pszGoodyNames[j]);
			}
		}
		else
		{
			m_aiGoodies = NULL;
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		SAFE_DELETE_ARRAY(pszGoodyNames);
	}
	return true;
}
//======================================================================================================
//					CvGameSpeedInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvGameSpeedInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvGameSpeedInfo::CvGameSpeedInfo() :
m_iGrowthPercent(0),
m_iStoragePercent(0),
m_iTrainPercent(0),
m_iConstructPercent(0),
m_iFatherPercent(0),
m_iGreatGeneralPercent(0),
m_iRevolutionTurns(0),
m_iNumTurnIncrements(0),
m_pGameTurnInfo(NULL)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvGameSpeedInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvGameSpeedInfo::~CvGameSpeedInfo()
{
	SAFE_DELETE_ARRAY(m_pGameTurnInfo);
}
int CvGameSpeedInfo::getGrowthPercent() const
{
	return m_iGrowthPercent;
}
int CvGameSpeedInfo::getStoragePercent() const
{
	return m_iStoragePercent;
}
int CvGameSpeedInfo::getTrainPercent() const
{
	return m_iTrainPercent;
}
int CvGameSpeedInfo::getConstructPercent() const
{
	return m_iConstructPercent;
}
int CvGameSpeedInfo::getFatherPercent() const
{
	return m_iFatherPercent;
}
int CvGameSpeedInfo::getGreatGeneralPercent() const
{
	return m_iGreatGeneralPercent;
}
int CvGameSpeedInfo::getRevolutionTurns() const
{
    return m_iRevolutionTurns;
}
int CvGameSpeedInfo::getNumTurnIncrements() const
{
	return m_iNumTurnIncrements;
}
GameTurnInfo& CvGameSpeedInfo::getGameTurnInfo(int iIndex) const
{
	return m_pGameTurnInfo[iIndex];
}
void CvGameSpeedInfo::allocateGameTurnInfos(const int iSize)
{
	m_pGameTurnInfo = new GameTurnInfo[iSize];
}
bool CvGameSpeedInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	int j, iTempVal;
	pXML->GetChildXmlValByName(&m_iGrowthPercent, "iGrowthPercent");
	pXML->GetChildXmlValByName(&m_iStoragePercent, "iStoragePercent");
	pXML->GetChildXmlValByName(&m_iTrainPercent, "iTrainPercent");
	pXML->GetChildXmlValByName(&m_iConstructPercent, "iConstructPercent");
	pXML->GetChildXmlValByName(&m_iFatherPercent, "iFatherPercent");
	pXML->GetChildXmlValByName(&m_iGreatGeneralPercent, "iGreatGeneralPercent");
	pXML->GetChildXmlValByName(&m_iRevolutionTurns, "iRevolutionTurns");
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"GameTurnInfos"))
	{
		m_iNumTurnIncrements = gDLL->getXMLIFace()->GetNumChildren(pXML->GetXML());
		if (gDLL->getXMLIFace()->SetToChild(pXML->GetXML()))
		{
			allocateGameTurnInfos(getNumTurnIncrements());
			// loop through each tag
			for (j=0;j<getNumTurnIncrements();j++)
			{
				pXML->GetChildXmlValByName(&iTempVal, "iMonthIncrement");
				getGameTurnInfo(j).iMonthIncrement = iTempVal;
				pXML->GetChildXmlValByName(&iTempVal, "iTurnsPerIncrement");
				getGameTurnInfo(j).iNumGameTurnsPerIncrement = iTempVal;
				// if we cannot set the current xml node to it's next sibling then we will break out of the for loop
				// otherwise we will continue looping
				if (!gDLL->getXMLIFace()->NextSibling(pXML->GetXML()))
				{
					break;
				}
			}
			gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	return true;
}

//======================================================================================================
//					CvTurnTimerInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvTurnTimerInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvTurnTimerInfo::CvTurnTimerInfo() :
m_iBaseTime(0),
m_iCityBonus(0),
m_iUnitBonus(0),
m_iFirstTurnMultiplier(0)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvTurnTimerInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvTurnTimerInfo::~CvTurnTimerInfo()
{
}
int CvTurnTimerInfo::getBaseTime() const
{
	return m_iBaseTime;
}
int CvTurnTimerInfo::getCityBonus() const
{
	return m_iCityBonus;
}
int CvTurnTimerInfo::getUnitBonus() const
{
	return m_iUnitBonus;
}
int CvTurnTimerInfo::getFirstTurnMultiplier() const
{
	return m_iFirstTurnMultiplier;
}
bool CvTurnTimerInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(&m_iBaseTime, "iBaseTime");
	pXML->GetChildXmlValByName(&m_iCityBonus, "iCityBonus");
	pXML->GetChildXmlValByName(&m_iUnitBonus, "iUnitBonus");
	pXML->GetChildXmlValByName(&m_iFirstTurnMultiplier, "iFirstTurnMultiplier");
	return true;
}
//======================================================================================================
//					CvBuildInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvBuildInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvBuildInfo::CvBuildInfo() :
m_iTime(0),
m_iCost(0),
m_eImprovement(NO_IMPROVEMENT),
m_ePrereqTerrain(NO_TERRAIN), // R&R, ray, Terraforming Features
m_eResultTerrain(NO_TERRAIN), // R&R, ray, Terraforming Features
m_eResultFeature(NO_FEATURE), // R&R, ray, Terraforming Features
m_eRoute(NO_ROUTE),
m_eEntityEvent(ENTITY_EVENT_NONE),
m_eMissionType(NO_MISSION),
m_bKill(false),
m_iFeatureRemoveLength(0),
m_aFeatureRemoves(NULL)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvBuildInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvBuildInfo::~CvBuildInfo()
{
	SAFE_DELETE_ARRAY(m_aFeatureRemoves);
}

CvBuildInfo::FeatureStruct::FeatureStruct()
	: FeatureType(NO_FEATURE)
	, iTime(0)
{
}

int CvBuildInfo::getTime() const
{
	return m_iTime;
}
int CvBuildInfo::getCost() const
{
	return m_iCost;
}
ImprovementTypes CvBuildInfo::getImprovement() const
{
	return m_eImprovement;
}
// R&R, ray, Terraforming Features - START
TerrainTypes CvBuildInfo::getPrereqTerrain() const
{
	return m_ePrereqTerrain;
}
TerrainTypes CvBuildInfo::getResultTerrain() const
{
	return m_eResultTerrain;
}
FeatureTypes CvBuildInfo::getResultFeature() const
{
	return m_eResultFeature;
}
// R&R, ray, Terraforming Features - END
RouteTypes CvBuildInfo::getRoute() const
{
	return m_eRoute;
}
EntityEventTypes CvBuildInfo::getEntityEvent() const
{
	return m_eEntityEvent;
}
MissionTypes CvBuildInfo::getMissionType() const
{
	return m_eMissionType;
}
void CvBuildInfo::setMissionType(MissionTypes eNewType)
{
	m_eMissionType = eNewType;
}
bool CvBuildInfo::isKill() const
{
	return m_bKill;
}
bool CvBuildInfo::isRoute() const
{
	return (getRoute() != NO_ROUTE);
}

const CvBuildInfo::FeatureStruct* CvBuildInfo::getFeatureRemove(FeatureTypes eFeature) const
{
	if (!isFeatureRemove(eFeature))
	{
		return NULL;
	}

	for (int i = 0; i < m_iFeatureRemoveLength; ++i)
	{
		if (m_aFeatureRemoves[i].FeatureType)
		{
			return &m_aFeatureRemoves[i];
		}
	}
	return NULL;
}

bool CvBuildInfo::isFeatureRemove(FeatureTypes eFeature) const
{
	if (isInRange(eFeature))
	{
		return m_baFeatureRemove.get(eFeature);
	}
	return false;
}

bool CvBuildInfo::read(XMLReader& reader)
{
	CvHotkeyInfo::read(reader);

	reader.Read("iTime", m_iTime);
	reader.Read("iCost", m_iCost);
	reader.Read("bKill", m_bKill);
	reader.Read("ImprovementType", m_eImprovement);
	reader.Read("PrereqTerrainType", m_ePrereqTerrain);
	reader.Read("ResultTerrainType", m_eResultTerrain);
	reader.Read("ResultFeatureType", m_eResultFeature);
	reader.Read("RouteType", m_eRoute);
	reader.Read("EntityEvent", m_eEntityEvent);

	if (XMLReader childReader = reader.getFirstChild("FeatureStructs"))
	{
		m_iFeatureRemoveLength = childReader.getNumChildren("FeatureStruct");
		if (m_iFeatureRemoveLength > 0)
		{
			SAFE_DELETE_ARRAY(m_aFeatureRemoves);
			m_aFeatureRemoves = new FeatureStruct[m_iFeatureRemoveLength];
			XMLReader loopReader = childReader.getFirstChild("FeatureStruct");
			for (int i = 0; loopReader; loopReader.nextSiblingSameName(), ++i)
			{
				FeatureStruct& loopStruct = m_aFeatureRemoves[i];
				
				FeatureTypes eFeature;
				loopReader.Read("FeatureType", eFeature);
				loopStruct.FeatureType = eFeature;
				m_baFeatureRemove.set(eFeature, true);
				
				int iTime;
				loopReader.Read("iTime", iTime);
				loopStruct.iTime = iTime;

				loopReader.Read("Yields", loopStruct.Yields);
				
			}
			FAssert(i == m_iFeatureRemoveLength);
		}
	}

	return true;
}
//======================================================================================================
//					CvGoodyInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvGoodyInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvGoodyInfo::CvGoodyInfo() :
m_iGold(0),
m_iGoldRand1(0),
m_iGoldRand2(0),
m_iImmigration(0), // R&R, ray, Goody Enhancement
m_iMapOffset(0),
m_iMapRange(0),
m_iMapProb(0),
m_iExperience(0),
m_iHealing(0),
m_iDamagePrereq(0),
m_iCityGoodyWeight(0),
m_iUnitClassType(NO_UNITCLASS),
m_iTeachUnitClassType(NO_UNITCLASS),
m_bBad(false),
m_bWar(false),
m_bWaterGoody(false), // R&R, ray, Goodies on Water
m_bUnique(false), // R&R, ray, Goody Enhancement
// WTP, ray, Unit spawning Goodies and Goody Huts - START
m_iMinTurnValid(0),
m_iRandNumHostilesSpawned(0),
m_bSpawnHostileUnitsAsXML(false),
m_bSpawnHostileAnimals(false),
m_bSpawnHostileNatives(false),
m_bSpawnHostileCriminals(false),
// WTP, ray, Unit spawning Goodies and Goody Huts - END
m_aGoodyWeights(NULL)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvGoodyInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvGoodyInfo::~CvGoodyInfo()
{
	SAFE_DELETE_ARRAY(m_aGoodyWeights);
}
int CvGoodyInfo::getGold() const
{
	return m_iGold;
}
int CvGoodyInfo::getGoldRand1() const
{
	return m_iGoldRand1;
}
int CvGoodyInfo::getGoldRand2() const
{
	return m_iGoldRand2;
}
int CvGoodyInfo::getImmigration() const // R&R, ray, Goody Enhancement
{
	return m_iImmigration;
}
int CvGoodyInfo::getMapOffset() const
{
	return m_iMapOffset;
}
int CvGoodyInfo::getMapRange() const
{
	return m_iMapRange;
}
int CvGoodyInfo::getMapProb() const
{
	return m_iMapProb;
}
int CvGoodyInfo::getExperience() const
{
	return m_iExperience;
}
int CvGoodyInfo::getHealing() const
{
	return m_iHealing;
}
int CvGoodyInfo::getDamagePrereq() const
{
	return m_iDamagePrereq;
}
int CvGoodyInfo::getCityGoodyWeight() const
{
	return m_iCityGoodyWeight;
}
int CvGoodyInfo::getUnitClassType() const
{
	return m_iUnitClassType;
}
int CvGoodyInfo::getTeachUnitClassType() const
{
	return m_iTeachUnitClassType;
}
bool CvGoodyInfo::isBad() const
{
	return m_bBad;
}
bool CvGoodyInfo::isWar() const
{
	return m_bWar;
}
// R&R, ray, Goodies on Water - START
bool CvGoodyInfo::isWaterGoody() const
{
	return m_bWaterGoody;
}
// R&R, ray, Goodies on Water - END
bool CvGoodyInfo::isUnique() const // R&R, ray, Goody Enhancement
{
	return m_bUnique;
}
// WTP, ray, Unit spawning Goodies and Goody Huts -START
int CvGoodyInfo::getMinTurnValid() const
{
	return m_iMinTurnValid;
}
int CvGoodyInfo::getRandNumHostilesSpawned() const
{
	return m_iRandNumHostilesSpawned;
}

bool CvGoodyInfo::isSpawnHostileUnitsAsXML()const
{
	return m_bSpawnHostileUnitsAsXML;
}
bool CvGoodyInfo::isSpawnHostileAnimals()const
{
	return m_bSpawnHostileAnimals;
}
bool CvGoodyInfo::isSpawnHostileNatives()const
{
	return m_bSpawnHostileNatives;
}
bool CvGoodyInfo::isSpawnHostileCriminals()const
{
	return m_bSpawnHostileCriminals;
}
// WTP, ray, Unit spawning Goodies and Goody Huts -END
int CvGoodyInfo::getGoodyWeight(int iGoody) const
{
	FAssert(iGoody >= 0 && iGoody < GC.getNumGoodyInfos());
	return m_aGoodyWeights[iGoody];
}
const char* CvGoodyInfo::getSound() const
{
	return m_szSound;
}
void CvGoodyInfo::setSound(const char* szVal)
{
	m_szSound=szVal;
}
const char* CvGoodyInfo::getAnnounceTextKey() const
{
	return m_szAnnounceTextKey;
}
const char* CvGoodyInfo::getChiefTextKey() const
{
	return m_szChiefTextKey;
}
bool CvGoodyInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(m_szSound, "Sound");
	pXML->GetChildXmlValByName(m_szAnnounceTextKey, "AnnounceText");
	pXML->GetChildXmlValByName(m_szChiefTextKey, "ChiefText");
	pXML->GetChildXmlValByName(&m_iGold, "iGold");
	pXML->GetChildXmlValByName(&m_iGoldRand1, "iGoldRand1");
	pXML->GetChildXmlValByName(&m_iGoldRand2, "iGoldRand2");
	pXML->GetChildXmlValByName(&m_iImmigration, "iImmigration"); // R&R, ray, Goody Enhancement
	pXML->GetChildXmlValByName(&m_iMapOffset, "iMapOffset");
	pXML->GetChildXmlValByName(&m_iMapRange, "iMapRange");
	pXML->GetChildXmlValByName(&m_iMapProb, "iMapProb");
	pXML->GetChildXmlValByName(&m_iExperience, "iExperience");
	pXML->GetChildXmlValByName(&m_iHealing, "iHealing");
	pXML->GetChildXmlValByName(&m_iDamagePrereq, "iDamagePrereq");
	pXML->GetChildXmlValByName(&m_bBad, "bBad");
	pXML->GetChildXmlValByName(&m_bWar, "bWar");
	pXML->GetChildXmlValByName(szTextVal, "UnitClass");
	m_iUnitClassType = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "TeachUnitClass");
	m_iTeachUnitClassType = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(&m_iCityGoodyWeight, "iCityGoodyWeight");
	pXML->GetChildXmlValByName(&m_bWaterGoody, "bWaterGoody"); // R&R, ray, Goodies on Water
	pXML->GetChildXmlValByName(&m_bUnique, "bUnique"); // // R&R, ray, Goody Enhancement
	// WTP, ray, Unit spawning Goodies and Goody Huts - START
	pXML->GetChildXmlValByName(&m_iMinTurnValid, "iMinTurnValid");
	pXML->GetChildXmlValByName(&m_iRandNumHostilesSpawned, "iRandNumHostilesSpawned");
	pXML->GetChildXmlValByName(&m_bSpawnHostileUnitsAsXML, "bSpawnHostileUnitsAsXML");
	pXML->GetChildXmlValByName(&m_bSpawnHostileAnimals, "bSpawnHostileAnimals");
	pXML->GetChildXmlValByName(&m_bSpawnHostileNatives, "bSpawnHostileNatives");
	pXML->GetChildXmlValByName(&m_bSpawnHostileCriminals, "bSpawnHostileCriminals");
	// WTP, ray, Unit spawning Goodies and Goody Huts - END
	return true;
}
bool CvGoodyInfo::readPass2(CvXMLLoadUtility* pXML)
{
	pXML->SetVariableListTagPair(&m_aGoodyWeights, "GoodyWeights", GC.getNumGoodyInfos(), 0);
	return true;
}
//======================================================================================================
//					CvRouteInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvRouteInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvRouteInfo::CvRouteInfo() :
m_iAdvancedStartCost(0),
m_iAdvancedStartCostIncrease(0),
m_iValue(0),
m_iMovementCost(0),
m_iFlatMovementCost(0),
m_aiYieldChange(NULL)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvRouteInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvRouteInfo::~CvRouteInfo()
{
	SAFE_DELETE_ARRAY(m_aiYieldChange);
}
int CvRouteInfo::getAdvancedStartCost() const
{
	return m_iAdvancedStartCost;
}
int CvRouteInfo::getAdvancedStartCostIncrease() const
{
	return m_iAdvancedStartCostIncrease;
}
int CvRouteInfo::getValue() const
{
	return m_iValue;
}
int CvRouteInfo::getMovementCost() const
{
	return m_iMovementCost;
}
int CvRouteInfo::getFlatMovementCost() const
{
	return m_iFlatMovementCost;
}
// Arrays
int CvRouteInfo::getYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiYieldChange ? m_aiYieldChange[i] : -1;
}
bool CvRouteInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(&m_iAdvancedStartCost, "iAdvancedStartCost");
	pXML->GetChildXmlValByName(&m_iAdvancedStartCostIncrease, "iAdvancedStartCostIncrease");
	pXML->GetChildXmlValByName(&m_iValue, "iValue");
	pXML->GetChildXmlValByName(&m_iMovementCost, "iMovement");
	pXML->GetChildXmlValByName(&m_iFlatMovementCost, "iFlatMovement");
	pXML->GetChildXmlValByName(szTextVal, "BonusType");

	pXML->SetVariableListTagPair(&m_aiYieldChange, "Yields", NUM_YIELD_TYPES, 0);
	return true;
}
//======================================================================================================
//					CvImprovementBonusInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvImprovementBonusInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvImprovementBonusInfo::CvImprovementBonusInfo() :
m_iDiscoverRand(0),
m_bBonusMakesValid(false),
m_aiYieldChange(NULL)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvImprovementBonusInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvImprovementBonusInfo::~CvImprovementBonusInfo()
{
	SAFE_DELETE_ARRAY(m_aiYieldChange);
}
int CvImprovementBonusInfo::getDiscoverRand() const
{
	return m_iDiscoverRand;
}
bool CvImprovementBonusInfo::isBonusMakesValid() const
{
	return m_bBonusMakesValid;
}
int CvImprovementBonusInfo::getYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiYieldChange ? m_aiYieldChange[i] : -1;
}
void CvImprovementBonusInfo::read(FDataStreamBase* stream)
{
	CvInfoBase::read(stream);
	uint uiFlag=0;
	stream->Read(&uiFlag);		// flag for expansion
	stream->Read(&m_iDiscoverRand);
	stream->Read(&m_bBonusMakesValid);
	// Arrays
	SAFE_DELETE_ARRAY(m_aiYieldChange);
	m_aiYieldChange = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_aiYieldChange);
}
void CvImprovementBonusInfo::write(FDataStreamBase* stream)
{
	CvInfoBase::write(stream);
	uint uiFlag=0;
	stream->Write(uiFlag);		// flag for expansion
	stream->Write(m_iDiscoverRand);
	stream->Write(m_bBonusMakesValid);
	// Arrays
	stream->Write(NUM_YIELD_TYPES, m_aiYieldChange);
}
//======================================================================================================
//					CvImprovementInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvImprovementInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvImprovementInfo::CvImprovementInfo() :
m_iAdvancedStartCost(0),
m_iAdvancedStartCostIncrease(0),
m_iTilesPerGoody(0),
m_iGoodyUniqueRange(0),
m_iFeatureGrowthProbability(0),
m_iUpgradeTime(0),
m_iDefenseModifier(0),
m_iFoodModifierForCity(0), // WTP, ray, Improvements give Bonus to their City - PART 2 - START
m_iHammersModifierForCity(0), // WTP, ray, Improvements give Bonus to their City - PART 2 - START
m_iToolsModifierForCity(0), // WTP, ray, Improvements give Bonus to their City - PART 2 - START
m_iStorageModifierForCity(0), // WTP, ray, Improvements give Bonus to their City - PART 3 - START
m_iPillageGold(0),
m_iHealModifier(0),// WTP, ray, Plot Heal Modifier for Improvements - START
m_iImprovementPillage(NO_IMPROVEMENT),
m_iImprovementUpgrade(NO_IMPROVEMENT),
// Super Forts begin *XML*
m_iCulture(0),
m_iCultureRange(0),
m_iPlotVisibilityChange(0),
m_iUnitVisibilityChange(0),
m_iSeeFrom(0),
m_iUniqueRange(0),
m_bBombardable(false),
m_bUpgradeRequiresFortify(false),
// Super Forts end
m_bActsAsCity(true),
m_bFort(true), // R&R, ray, Monasteries and Forts
m_bMonastery(true), // R&R, ray, Monasteries and Forts
m_bCanal(true), // WTP, ray, Canal - START
m_bNotAllowedNextToSameAsItself(false), // WTP, ray, Not allowed next to itself - START
m_bHillsMakesValid(false),
m_bRiverSideMakesValid(false),
m_bRequiresFlatlands(false),
m_bRequiresRiverSide(false),
m_bRequiresFeature(false),
m_bWater(false),
m_bGoody(false),
m_bGoodyForSpawningUnits(false), //WTP, Unit only Goodies
m_bGoodyForSpawningHostileAnimals(false), //WTP, Protected Hostile Goodies
m_bGoodyForSpawningHostileNatives(false), //WTP, Protected Hostile Goodies
m_bGoodyForSpawningHostileCriminals(false), //WTP, Protected Hostile Goodies
m_bPermanent(false),
m_bUseLSystem(false),
m_bOutsideBorders(false),
m_iWorldSoundscapeScriptId(0),
m_aiPrereqNatureYield(NULL),
m_aiYieldIncrease(NULL),
m_aiRiverSideYieldChange(NULL),
m_aiHillsYieldChange(NULL),
m_abTerrainMakesValid(NULL),
m_abFeatureMakesValid(NULL),
m_paImprovementBonus(NULL)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvImprovementInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvImprovementInfo::~CvImprovementInfo()
{
	SAFE_DELETE_ARRAY(m_aiPrereqNatureYield);
	SAFE_DELETE_ARRAY(m_aiYieldIncrease);
	SAFE_DELETE_ARRAY(m_aiRiverSideYieldChange);
	SAFE_DELETE_ARRAY(m_aiHillsYieldChange);
	SAFE_DELETE_ARRAY(m_abTerrainMakesValid);
	SAFE_DELETE_ARRAY(m_abFeatureMakesValid);
	SAFE_DELETE_ARRAY(m_paImprovementBonus);
	for (uint iI=0;iI<m_aaiRouteYieldChanges.size();iI++)
		{
		SAFE_DELETE_ARRAY(m_aaiRouteYieldChanges[iI]);
	}
}
int CvImprovementInfo::getAdvancedStartCost() const
{
	return m_iAdvancedStartCost;
}
int CvImprovementInfo::getAdvancedStartCostIncrease() const
{
	return m_iAdvancedStartCostIncrease;
}
int CvImprovementInfo::getTilesPerGoody() const
{
	return m_iTilesPerGoody;
}
int CvImprovementInfo::getGoodyUniqueRange() const
{
	return m_iGoodyUniqueRange;
}
int CvImprovementInfo::getFeatureGrowthProbability() const
{
	return m_iFeatureGrowthProbability;
}
int CvImprovementInfo::getUpgradeTime() const
{
	return m_iUpgradeTime;
}
int CvImprovementInfo::getDefenseModifier() const
{
	return m_iDefenseModifier;
}

// WTP, ray, Improvements give Bonus to their City - PART 2 - START
int CvImprovementInfo::getFoodModifierForCity() const
{
	return m_iFoodModifierForCity;
}
int CvImprovementInfo::getHammersModifierForCity() const
{
	return m_iHammersModifierForCity;
}
int CvImprovementInfo::getToolsModifierForCity() const
{
	return m_iToolsModifierForCity;
}
// WTP, ray, Improvements give Bonus to their City - PART 2 - END

// WTP, ray, Improvements give Bonus to their City - PART 3 - START
int CvImprovementInfo::getStorageModifierForCity() const
{
	return m_iStorageModifierForCity;
}
// WTP, ray, Improvements give Bonus to their City - PART 3 - END

int CvImprovementInfo::getPillageGold() const
{
	return m_iPillageGold;
}

// WTP, ray, Plot Heal Modifier for Improvements - START
int CvImprovementInfo::getHealModifier() const
{
	return m_iHealModifier;
}
// WTP, ray, Plot Heal Bonus for Improvements - END

bool CvImprovementInfo::isOutsideBorders() const
{
	return m_bOutsideBorders;
}
int CvImprovementInfo::getImprovementPillage() const
{
	return m_iImprovementPillage;
}
void CvImprovementInfo::setImprovementPillage(int i)
{
	m_iImprovementPillage = i;
}
int CvImprovementInfo::getImprovementUpgrade() const
{
	return m_iImprovementUpgrade;
}
void CvImprovementInfo::setImprovementUpgrade(int i)
{
	m_iImprovementUpgrade = i;
}
// Super Forts begin *XML*
int CvImprovementInfo::getCulture() const
{
	return m_iCulture;
}

int CvImprovementInfo::getCultureRange() const
{
	return m_iCultureRange;
}

int CvImprovementInfo::getPlotVisibilityChange() const
{
	return m_iPlotVisibilityChange;
}

int CvImprovementInfo::getUnitVisibilityChange() const
{
	return m_iUnitVisibilityChange;
}

int CvImprovementInfo::getSeeFrom() const
{
	return m_iSeeFrom;
}

int CvImprovementInfo::getUniqueRange() const
{
	return m_iUniqueRange;
}

bool CvImprovementInfo::isBombardable() const
{
	return m_bBombardable;
}

bool CvImprovementInfo::isUpgradeRequiresFortify() const
{
	return m_bUpgradeRequiresFortify;
}
// Super Forts end
bool CvImprovementInfo::isActsAsCity() const
{
	return m_bActsAsCity;
}
// R&R, ray, Monasteries and Forts - START
bool CvImprovementInfo::isFort() const
{
	return m_bFort;
}
bool CvImprovementInfo::isMonastery() const
{
	return m_bMonastery;
}
// R&R, ray, Monasteries and Forts- END

// WTP, ray, Canal - START
bool CvImprovementInfo::isCanal() const
{
	return m_bCanal;
}
// WTP, ray, Canal - END

// WTP, ray, Not allowed next to itself - START
bool CvImprovementInfo::isNotAllowedNextToSameAsItself() const
{
	return m_bNotAllowedNextToSameAsItself;
}
// WTP, ray, Not allowed next to itself - END

bool CvImprovementInfo::isHillsMakesValid() const
{
	return m_bHillsMakesValid;
}
bool CvImprovementInfo::isRiverSideMakesValid() const
{
	return m_bRiverSideMakesValid;
}
bool CvImprovementInfo::isRequiresFlatlands() const
{
	return m_bRequiresFlatlands;
}
bool CvImprovementInfo::isRequiresRiverSide() const
{
	return m_bRequiresRiverSide;
}
bool CvImprovementInfo::isRequiresFeature() const
{
	return m_bRequiresFeature;
}
bool CvImprovementInfo::isWater() const
{
	return m_bWater;
}
bool CvImprovementInfo::isGoody() const
{
	return m_bGoody;
}
//WTP, Unit only Goodies - START
bool CvImprovementInfo::isGoodyForSpawningUnits() const
{
	return m_bGoodyForSpawningUnits;
}
//WTP, Unit only Goodies - END

//WTP, Protected Hostile Goodies - START
bool CvImprovementInfo::isGoodyForSpawningHostileAnimals() const
{
	return m_bGoodyForSpawningHostileAnimals;
}

bool CvImprovementInfo::isGoodyForSpawningHostileNatives() const
{
	return m_bGoodyForSpawningHostileNatives;
}

bool CvImprovementInfo::isGoodyForSpawningHostileCriminals() const
{
	return m_bGoodyForSpawningHostileCriminals;
}
bool CvImprovementInfo::isAINoRemove() const
{
	// todo: figure out if forts or other improvements should be added here, possibly by xml bool setting
	return isGoody();
}
//WTP, Protected Hostile Goodies - END
bool CvImprovementInfo::isPermanent() const
{
	return m_bPermanent;
}
bool CvImprovementInfo::useLSystem() const
{
	return m_bUseLSystem;
}
const char* CvImprovementInfo::getArtDefineTag() const
{
	return m_szArtDefineTag;
}
void CvImprovementInfo::setArtDefineTag(const char* szVal)
{
	m_szArtDefineTag = szVal;
}
int CvImprovementInfo::getWorldSoundscapeScriptId() const
{
	return m_iWorldSoundscapeScriptId;
}
// Arrays
int CvImprovementInfo::getPrereqNatureYield(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiPrereqNatureYield ? m_aiPrereqNatureYield[i] : -1;
}
int* CvImprovementInfo::getPrereqNatureYieldArray()
{
	return m_aiPrereqNatureYield;
}
int CvImprovementInfo::getYieldIncrease(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiYieldIncrease ? m_aiYieldIncrease[i] : -1;
}
int* CvImprovementInfo::getYieldIncreaseArray()
{
	return m_aiYieldIncrease;
}
int CvImprovementInfo::getRiverSideYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiRiverSideYieldChange ? m_aiRiverSideYieldChange[i] : -1;
}
int* CvImprovementInfo::getRiverSideYieldChangeArray()
{
	return m_aiRiverSideYieldChange;
}
int CvImprovementInfo::getHillsYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiHillsYieldChange ? m_aiHillsYieldChange[i] : -1;
}
int* CvImprovementInfo::getHillsYieldChangeArray()
{
	return m_aiHillsYieldChange;
}
bool CvImprovementInfo::getTerrainMakesValid(int i) const
{
	FAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abTerrainMakesValid ? m_abTerrainMakesValid[i] : false;
}
bool CvImprovementInfo::getFeatureMakesValid(int i) const
{
	FAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abFeatureMakesValid ? m_abFeatureMakesValid[i] : false;
}
int CvImprovementInfo::getRouteYieldChanges(int i, int j) const
{
	FAssertMsg(i < GC.getNumRouteInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return m_aaiRouteYieldChanges[i][j];
}
int* CvImprovementInfo::getRouteYieldChangesArray(int i)
{
	return &(m_aaiRouteYieldChanges[i][0]);
}
int CvImprovementInfo::getImprovementBonusYield(int i, int j) const
{
	FAssertMsg(i < GC.getNumBonusInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return m_paImprovementBonus[i].m_aiYieldChange ? m_paImprovementBonus[i].getYieldChange(j) : -1;
}
bool CvImprovementInfo::isImprovementBonusMakesValid(int i) const
{
	FAssertMsg(i < GC.getNumBonusInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paImprovementBonus[i].m_bBonusMakesValid;
}
int CvImprovementInfo::getImprovementBonusDiscoverRand(int i) const
{
	FAssertMsg(i < GC.getNumBonusInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paImprovementBonus[i].m_iDiscoverRand;
}
const char* CvImprovementInfo::getButton() const
{
	const CvArtInfoImprovement * pImprovementArtInfo;
	pImprovementArtInfo = getArtInfo();
	if (pImprovementArtInfo != NULL)
	{
		return pImprovementArtInfo->getButton();
	}
	return NULL;
}
const CvArtInfoImprovement* CvImprovementInfo::getArtInfo() const
{
	return ARTFILEMGR.getImprovementArtInfo(getArtDefineTag());
}
void CvImprovementInfo::read(FDataStreamBase* stream)
{
	CvInfoBase::read(stream);
	uint uiFlag=0;
	stream->Read(&uiFlag);		// flag for expansion
	stream->Read(&m_iAdvancedStartCost);
	stream->Read(&m_iAdvancedStartCostIncrease);
	stream->Read(&m_iTilesPerGoody);
	stream->Read(&m_iGoodyUniqueRange);
	stream->Read(&m_iFeatureGrowthProbability);
	stream->Read(&m_iUpgradeTime);
	stream->Read(&m_iDefenseModifier);
	stream->Read(&m_iFoodModifierForCity); // WTP, ray, Improvements give Bonus to their City - PART 2 - START
	stream->Read(&m_iHammersModifierForCity); // WTP, ray, Improvements give Bonus to their City - PART 2 - START
	stream->Read(&m_iToolsModifierForCity); // WTP, ray, Improvements give Bonus to their City - PART 2 - START
	stream->Read(&m_iStorageModifierForCity); // WTP, ray, Improvements give Bonus to their City - PART 3 - START
	stream->Read(&m_iPillageGold);
	stream->Read(&m_iHealModifier); // WTP, ray, Plot Heal Modifier for Improvements - START
	stream->Read(&m_iImprovementPillage);
	stream->Read(&m_iImprovementUpgrade);
	// Super Forts begin *XML*
	stream->Read(&m_iCulture);
	stream->Read(&m_iCultureRange);
//	stream->Read(&m_iVisibilityChange);
	stream->Read(&m_iSeeFrom);
	stream->Read(&m_iUniqueRange);
	stream->Read(&m_bBombardable);
	stream->Read(&m_bUpgradeRequiresFortify);
	// Super Forts end
	stream->Read(&m_bActsAsCity);
	stream->Read(&m_bFort); // R&R, ray, Monasteries and Forts
	stream->Read(&m_bMonastery); // R&R, ray, Monasteries and Forts
	stream->Read(&m_bCanal); // WTP, ray, Canal - START
	stream->Read(&m_bNotAllowedNextToSameAsItself);// WTP, ray, Not allowed next to itself - START
	stream->Read(&m_bHillsMakesValid);
	stream->Read(&m_bRiverSideMakesValid);
	stream->Read(&m_bRequiresFlatlands);
	stream->Read(&m_bRequiresRiverSide);
	stream->Read(&m_bRequiresFeature);
	stream->Read(&m_bWater);
	stream->Read(&m_bGoody);
	stream->Read(&m_bGoodyForSpawningUnits); //WTP, Unit only Goodies
	stream->Read(&m_bGoodyForSpawningHostileAnimals); //WTP, Protected Hostile Goodies
	stream->Read(&m_bGoodyForSpawningHostileNatives); //WTP, Protected Hostile Goodies
	stream->Read(&m_bGoodyForSpawningHostileCriminals); //WTP, Protected Hostile Goodies
	stream->Read(&m_bPermanent);
	stream->Read(&m_bUseLSystem);
	stream->Read(&m_bOutsideBorders);
	stream->ReadString(m_szArtDefineTag);
	stream->Read(&m_iWorldSoundscapeScriptId);
	// Arrays
	SAFE_DELETE_ARRAY(m_aiPrereqNatureYield);
	m_aiPrereqNatureYield = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_aiPrereqNatureYield);
	SAFE_DELETE_ARRAY(m_aiYieldIncrease);
	m_aiYieldIncrease = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_aiYieldIncrease);
	SAFE_DELETE_ARRAY(m_aiRiverSideYieldChange);
	m_aiRiverSideYieldChange = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_aiRiverSideYieldChange);
	SAFE_DELETE_ARRAY(m_aiHillsYieldChange);
	m_aiHillsYieldChange = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_aiHillsYieldChange);
	SAFE_DELETE_ARRAY(m_abTerrainMakesValid);
	m_abTerrainMakesValid = new bool[GC.getNumTerrainInfos()];
	stream->Read(GC.getNumTerrainInfos(), m_abTerrainMakesValid);
	SAFE_DELETE_ARRAY(m_abFeatureMakesValid);
	m_abFeatureMakesValid = new bool[GC.getNumFeatureInfos()];
	stream->Read(GC.getNumFeatureInfos(), m_abFeatureMakesValid);
	SAFE_DELETE_ARRAY(m_paImprovementBonus);
	m_paImprovementBonus = new CvImprovementBonusInfo[GC.getNumBonusInfos()];
	for (int i = 0; i < GC.getNumBonusInfos(); i++)
	{
		m_paImprovementBonus[i].read(stream);
	}
	for (uint iI=0;iI<m_aaiRouteYieldChanges.size();iI++)
	{
		SAFE_DELETE_ARRAY(m_aaiRouteYieldChanges[iI]);
		}
	m_aaiRouteYieldChanges.clear();

	for(int i=0;i<GC.getNumRouteInfos();i++)
	{
		m_aaiRouteYieldChanges.push_back(new int[NUM_YIELD_TYPES]);
		stream->Read(NUM_YIELD_TYPES, m_aaiRouteYieldChanges[i]);
	}
}
void CvImprovementInfo::write(FDataStreamBase* stream)
{
	CvInfoBase::write(stream);
	uint uiFlag=0;
	stream->Write(uiFlag);		// flag for expansion
	stream->Write(m_iAdvancedStartCost);
	stream->Write(m_iAdvancedStartCostIncrease);
	stream->Write(m_iTilesPerGoody);
	stream->Write(m_iGoodyUniqueRange);
	stream->Write(m_iFeatureGrowthProbability);
	stream->Write(m_iUpgradeTime);
	stream->Write(m_iDefenseModifier);
	stream->Write(m_iFoodModifierForCity); // WTP, ray, Improvements give Bonus to their City - PART 2 - START
	stream->Write(m_iHammersModifierForCity); // WTP, ray, Improvements give Bonus to their City - PART 2 - START
	stream->Write(m_iToolsModifierForCity); // WTP, ray, Improvements give Bonus to their City - PART 2 - START
	stream->Write(m_iStorageModifierForCity); // WTP, ray, Improvements give Bonus to their City - PART 3 - START
	stream->Write(m_iPillageGold);
	stream->Write(m_iHealModifier); // WTP, ray, Plot Heal Modifier for Improvements - START
	stream->Write(m_iImprovementPillage);
	stream->Write(m_iImprovementUpgrade);
	// Super Forts begin *XML*
	stream->Write(m_iCulture);
	stream->Write(m_iCultureRange);
//	stream->Write(m_iVisibilityChange);
	stream->Write(m_iSeeFrom);
	stream->Write(m_iUniqueRange);
	stream->Write(m_bBombardable);
	stream->Write(m_bUpgradeRequiresFortify);
	// Super Forts end
	stream->Write(m_bActsAsCity);
	stream->Write(m_bFort); // R&R, ray, Monasteries and Forts
	stream->Write(m_bMonastery); // R&R, ray, Monasteries and Forts
	stream->Write(m_bCanal); // WTP, ray, Canal - START
	stream->Write(m_bNotAllowedNextToSameAsItself);// WTP, ray, Not allowed next to itself - START
	stream->Write(m_bHillsMakesValid);
	stream->Write(m_bRiverSideMakesValid);
	stream->Write(m_bRequiresFlatlands);
	stream->Write(m_bRequiresRiverSide);
	stream->Write(m_bRequiresFeature);
	stream->Write(m_bWater);
	stream->Write(m_bGoody);
	stream->Write(m_bGoodyForSpawningUnits); //WTP, Unit only Goodies
	stream->Write(m_bGoodyForSpawningHostileAnimals); //WTP, Protected Hostile Goodies
	stream->Write(m_bGoodyForSpawningHostileNatives); //WTP, Protected Hostile Goodies
	stream->Write(m_bGoodyForSpawningHostileCriminals); //WTP, Protected Hostile Goodies
	stream->Write(m_bPermanent);
	stream->Write(m_bUseLSystem);
	stream->Write(m_bOutsideBorders);
	stream->WriteString(m_szArtDefineTag);
	stream->Write(m_iWorldSoundscapeScriptId);
	// Arrays
	stream->Write(NUM_YIELD_TYPES, m_aiPrereqNatureYield);
	stream->Write(NUM_YIELD_TYPES, m_aiYieldIncrease);
	stream->Write(NUM_YIELD_TYPES, m_aiRiverSideYieldChange);
	stream->Write(NUM_YIELD_TYPES, m_aiHillsYieldChange);
	stream->Write(GC.getNumTerrainInfos(), m_abTerrainMakesValid);
	stream->Write(GC.getNumFeatureInfos(), m_abFeatureMakesValid);
	int i;
	for (i = 0; i < GC.getNumBonusInfos(); i++)
	{
		m_paImprovementBonus[i].write(stream);
	}
	for(i=0;i<GC.getNumRouteInfos();i++)
	{
		stream->Write(NUM_YIELD_TYPES, m_aaiRouteYieldChanges[i]);
	}
}
bool CvImprovementInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	int iIndex, j, iNumSibs;
	pXML->GetChildXmlValByName(szTextVal, "ArtDefineTag");
	setArtDefineTag(szTextVal);
	pXML->SetVariableListTagPair(&m_aiPrereqNatureYield, "PrereqNatureYields", NUM_YIELD_TYPES, 0);
	pXML->SetVariableListTagPair(&m_aiYieldIncrease, "YieldIncreases", NUM_YIELD_TYPES, 0);
	pXML->SetVariableListTagPair(&m_aiRiverSideYieldChange, "RiverSideYieldChanges", NUM_YIELD_TYPES, 0);
	pXML->SetVariableListTagPair(&m_aiHillsYieldChange, "HillsYieldChanges", NUM_YIELD_TYPES, 0);
	pXML->GetChildXmlValByName(&m_iAdvancedStartCost, "iAdvancedStartCost");
	pXML->GetChildXmlValByName(&m_iAdvancedStartCostIncrease, "iAdvancedStartCostIncrease");
	pXML->GetChildXmlValByName(&m_bActsAsCity, "bActsAsCity");
	pXML->GetChildXmlValByName(&m_bFort, "bFort"); // R&R, ray, Monasteries and Forts
	pXML->GetChildXmlValByName(&m_bMonastery, "bMonastery"); // R&R, ray, Monasteries and Forts
	pXML->GetChildXmlValByName(&m_bCanal, "bCanal"); // WTP, ray, Canal - START
	pXML->GetChildXmlValByName(&m_bNotAllowedNextToSameAsItself, "bNotAllowedNextToSameAsItself"); // WTP, ray, Not allowed next to itself - START
	pXML->GetChildXmlValByName(&m_bHillsMakesValid, "bHillsMakesValid");
	pXML->GetChildXmlValByName(&m_bRiverSideMakesValid, "bRiverSideMakesValid");
	pXML->GetChildXmlValByName(&m_bRequiresFlatlands, "bRequiresFlatlands");
	pXML->GetChildXmlValByName(&m_bRequiresRiverSide, "bRequiresRiverSide");
	pXML->GetChildXmlValByName(&m_bRequiresFeature, "bRequiresFeature");
	pXML->GetChildXmlValByName(&m_bWater, "bWater");
	pXML->GetChildXmlValByName(&m_bGoody, "bGoody");
	pXML->GetChildXmlValByName(&m_bGoodyForSpawningUnits, "bGoodyForSpawningUnits"); //WTP, Unit only Goodies
	pXML->GetChildXmlValByName(&m_bGoodyForSpawningHostileAnimals, "bGoodyForSpawningHostileAnimals"); //WTP, Unit only Goodies
	pXML->GetChildXmlValByName(&m_bGoodyForSpawningHostileNatives, "bGoodyForSpawningHostileNatives"); //WTP, Unit only Goodies
	pXML->GetChildXmlValByName(&m_bGoodyForSpawningHostileCriminals, "bGoodyForSpawningHostileCriminals"); //WTP, Unit only Goodies
	pXML->GetChildXmlValByName(&m_bPermanent, "bPermanent");
	pXML->GetChildXmlValByName(&m_bUseLSystem, "bUseLSystem");
	pXML->GetChildXmlValByName(&m_iTilesPerGoody, "iTilesPerGoody");
	pXML->GetChildXmlValByName(&m_iGoodyUniqueRange, "iGoodyRange");
	pXML->GetChildXmlValByName(&m_iFeatureGrowthProbability, "iFeatureGrowth");
	pXML->GetChildXmlValByName(&m_iUpgradeTime, "iUpgradeTime");
	pXML->GetChildXmlValByName(&m_iDefenseModifier, "iDefenseModifier");
	pXML->GetChildXmlValByName(&m_iFoodModifierForCity, "iFoodModifierForCity"); // WTP, ray, Improvements give Bonus to their City - PART 2 - START
	pXML->GetChildXmlValByName(&m_iHammersModifierForCity, "iHammersModifierForCity"); // WTP, ray, Improvements give Bonus to their City - PART 2 - START
	pXML->GetChildXmlValByName(&m_iToolsModifierForCity, "iToolsModifierForCity"); // WTP, ray, Improvements give Bonus to their City - PART 2 - START
	pXML->GetChildXmlValByName(&m_iStorageModifierForCity, "iStorageModifierForCity"); // WTP, ray, Improvements give Bonus to their City - PART 3 - START
	pXML->GetChildXmlValByName(&m_iPillageGold, "iPillageGold");
	pXML->GetChildXmlValByName(&m_iHealModifier, "iHealModifier"); // WTP, ray, Plot Heal Modifier for Improvements - START
	pXML->GetChildXmlValByName(&m_bOutsideBorders, "bOutsideBorders");
	// Super Forts begin *XML*
	pXML->GetChildXmlValByName(&m_iCulture, "iCulture");
	pXML->GetChildXmlValByName(&m_iCultureRange, "iCultureRange");
	pXML->GetChildXmlValByName(&m_iPlotVisibilityChange, "iPlotVisibilityChange");
	pXML->GetChildXmlValByName(&m_iUnitVisibilityChange, "iUnitVisibilityChange");
	pXML->GetChildXmlValByName(&m_iSeeFrom, "iSeeFrom");
	pXML->GetChildXmlValByName(&m_iUniqueRange, "iUnique");
	pXML->GetChildXmlValByName(&m_bBombardable, "bBombardable");
	pXML->GetChildXmlValByName(&m_bUpgradeRequiresFortify, "bUpgradeRequiresFortify");
	// Super Forts end

	pXML->SetVariableListTagPair(&m_abTerrainMakesValid, "TerrainMakesValids", GC.getNumTerrainInfos(), false);
	pXML->SetVariableListTagPair(&m_abFeatureMakesValid, "FeatureMakesValids", GC.getNumFeatureInfos(), false);
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"BonusTypeStructs"))
	{
		// call the function that sets the bonus booleans
		pXML->SetImprovementBonuses(&m_paImprovementBonus);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	else
	{
		// initialize the boolean list to the correct size and all the booleans to false
		pXML->InitImprovementBonusList(&m_paImprovementBonus, GC.getNumBonusInfos());
	}
	// initialize the boolean list to the correct size and all the booleans to false
	FAssertMsg((GC.getNumRouteInfos() > 0) && (NUM_YIELD_TYPES) > 0,"either the number of route infos is zero or less or the number of yield types is zero or less");
	pXML->Init2DIntList(m_aaiRouteYieldChanges, GC.getNumRouteInfos(), NUM_YIELD_TYPES);
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"RouteYieldChanges"))
	{
		if (pXML->SkipToNextVal())
		{
			iNumSibs = gDLL->getXMLIFace()->GetNumChildren(pXML->GetXML());
			if (gDLL->getXMLIFace()->SetToChild(pXML->GetXML()))
			{
				if (0 < iNumSibs)
				{
					for (j=0;j<iNumSibs;j++)
					{
						pXML->GetChildXmlValByName(szTextVal, "RouteType");
						iIndex = pXML->FindInInfoClass(szTextVal);
						if (iIndex > -1)
						{
							// delete the array since it will be reallocated
							SAFE_DELETE_ARRAY(m_aaiRouteYieldChanges[iIndex]);
							pXML->SetVariableListTagPair(&m_aaiRouteYieldChanges[iIndex], "RouteYields", NUM_YIELD_TYPES, 0);
						}
						if (!gDLL->getXMLIFace()->NextSibling(pXML->GetXML()))
						{
							break;
						}
					}
				}
				gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
			}
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	pXML->GetChildXmlValByName(szTextVal, "WorldSoundscapeAudioScript");
	if ( szTextVal.GetLength() > 0 )
		m_iWorldSoundscapeScriptId = gDLL->getAudioTagIndex( szTextVal.GetCString(), AUDIOTAG_SOUNDSCAPE );
	else
		m_iWorldSoundscapeScriptId = -1;
	return true;
}
bool CvImprovementInfo::readPass2(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	pXML->GetChildXmlValByName(szTextVal, "ImprovementPillage");
	m_iImprovementPillage = GC.getInfoTypeForString(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "ImprovementUpgrade");
	m_iImprovementUpgrade = GC.getInfoTypeForString(szTextVal);
	return true;
}
//======================================================================================================
//					CvBonusInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvBonusInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvBonusInfo::CvBonusInfo() :
m_iChar(0),
m_iHealthEffectFromRessource(0), // WTP, ray, Health Overhaul
m_iAIObjective(0),
m_iMinAreaSize(0),
m_iMinLatitude(0),
m_iMaxLatitude(0),
m_iPlacementOrder(0),
m_iConstAppearance(0),
m_iRandAppearance1(0),
m_iRandAppearance2(0),
m_iRandAppearance3(0),
m_iRandAppearance4(0),
m_iPercentPerPlayer(0),
m_iTilesPer(0),
m_iMinLandPercent(0),
m_iUniqueRange(0),
m_iGroupRange(0),
m_iGroupRand(0),
m_iBuilding(0),
m_bOneArea(false),
m_bPeaks(false), // R&R, ray, Bonus on Peaks - START
m_bHills(false),
m_bOcean(false), //TAC Whaling, ray
m_bFlatlands(false),
m_bNoRiverSide(false),
m_bRiverSideOnly(false), // Ray, adding 2 more XML tags to control bonus placement
m_bCoastalLandOnly(false), // Ray, adding 2 more XML tags to control bonus placement
m_bOnlySouthernHemisphere(false), //ray, Norther and Southern Hemisphere, using hint of f1rpo
m_bOnlyNorthernHemisphere(false), //ray, Norther and Southern Hemisphere, using hint of f1rpo
m_bUseLSystem(false),
m_bWhalingboatWorkable(false), //TAC Whaling, ray
m_bFishingboatWorkable(false), // R&R, ray, High Sea Fishing
m_aiYieldChange(NULL),
m_aiImprovementChange(NULL),
m_abTerrain(NULL),
m_abFeature(NULL),
m_abFeatureTerrain(NULL)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvBonusInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvBonusInfo::~CvBonusInfo()
{
	SAFE_DELETE_ARRAY(m_aiYieldChange);
	SAFE_DELETE_ARRAY(m_aiImprovementChange);
	SAFE_DELETE_ARRAY(m_abTerrain);
	SAFE_DELETE_ARRAY(m_abFeature);
	SAFE_DELETE_ARRAY(m_abFeatureTerrain);	// free memory - MT
}
int CvBonusInfo::getChar() const
{
	return m_iChar;
}
void CvBonusInfo::setChar(int i)
{
	m_iChar = i;
}
// WTP, ray, Health Overhaul - START
int CvBonusInfo::getHealthEffectFromRessource() const
{
	return m_iHealthEffectFromRessource;
}
// WTP, ray, Health Overhaul - END
int CvBonusInfo::getAIObjective() const
{
	return m_iAIObjective;
}
int CvBonusInfo::getMinAreaSize() const
{
	return m_iMinAreaSize;
}
int CvBonusInfo::getMinLatitude() const
{
	return m_iMinLatitude;
}
int CvBonusInfo::getMaxLatitude() const
{
	return m_iMaxLatitude;
}
int CvBonusInfo::getPlacementOrder() const
{
	return m_iPlacementOrder;
}
int CvBonusInfo::getConstAppearance() const
{
	return m_iConstAppearance;
}
int CvBonusInfo::getRandAppearance1() const
{
	return m_iRandAppearance1;
}
int CvBonusInfo::getRandAppearance2() const
{
	return m_iRandAppearance2;
}
int CvBonusInfo::getRandAppearance3() const
{
	return m_iRandAppearance3;
}
int CvBonusInfo::getRandAppearance4() const
{
	return m_iRandAppearance4;
}
int CvBonusInfo::getPercentPerPlayer() const
{
	return m_iPercentPerPlayer;
}
int CvBonusInfo::getTilesPer() const
{
	return m_iTilesPer;
}
int CvBonusInfo::getMinLandPercent() const
{
	return m_iMinLandPercent;
}
int CvBonusInfo::getUniqueRange() const
{
	return m_iUniqueRange;
}
int CvBonusInfo::getGroupRange() const
{
	return m_iGroupRange;
}
int CvBonusInfo::getGroupRand() const
{
	return m_iGroupRand;
}
bool CvBonusInfo::isOneArea() const
{
	return m_bOneArea;
}
// R&R, ray, Bonus on Peaks - START
bool CvBonusInfo::isPeaks() const
{
	return m_bPeaks;
}
// R&R, ray, Bonus on Peaks - END
bool CvBonusInfo::isHills() const
{
	return m_bHills;
}
//TAC Whaling, ray
bool CvBonusInfo::isOcean() const
{
	return m_bOcean;
}
//End TAC Whaling, ray
bool CvBonusInfo::isFlatlands() const
{
	return m_bFlatlands;
}
bool CvBonusInfo::isNoRiverSide() const
{
	return m_bNoRiverSide;
}

// Ray, adding 2 more XML tags to control bonus placement - START
bool CvBonusInfo::isRiverSideOnly() const
{
	return m_bRiverSideOnly;
}
bool CvBonusInfo::isCoastalLandOnly() const
{
	return m_bCoastalLandOnly;
}
// Ray, adding 2 more XML tags to control bonus placement - END


//ray, Norther and Southern Hemisphere, using hint of f1rpo - START
bool CvBonusInfo::isOnlySouthernHemisphere() const
{
	return m_bOnlySouthernHemisphere;
}
bool CvBonusInfo::isOnlyNorthernHemisphere() const
{
	return m_bOnlyNorthernHemisphere;
}
//ray, Norther and Southern Hemisphere, using hint of f1rpo - START

//TAC Whaling, ray
bool CvBonusInfo::isWhalingboatWorkable() const
{
	return m_bWhalingboatWorkable;
}
//End TAC Whaling, ray

// R&R, ray, High Sea Fishing - START
bool CvBonusInfo::isFishingboatWorkable() const
{
	return m_bFishingboatWorkable;
}
// R&R, ray, High Sea Fishing - END

bool CvBonusInfo::useLSystem() const
{
	return m_bUseLSystem;
}
const char* CvBonusInfo::getArtDefineTag() const
{
	return m_szArtDefineTag;
}
void CvBonusInfo::setArtDefineTag(const char* szVal)
{
	m_szArtDefineTag = szVal;
}

int CvBonusInfo::getNumYieldChanges() const
{
	int iNumYieldChanges = 0;
	for (int i = 0; i < NUM_YIELD_TYPES; i++)
	{
		if (getYieldChange(i) > 0)
		{
			iNumYieldChanges++;
		}
	}

	return iNumYieldChanges;
}

std::vector<int> CvBonusInfo::getYieldChangesArray()
{
	std::vector<int> aiYields(NUM_YIELD_TYPES, 0);

	for (int i = 0; i < NUM_YIELD_TYPES; i++)
	{
		if (getYieldChange(i) > 0)
		{
			aiYields[i] = getYieldChange(i);
		}
	}

	return aiYields;
}

// Arrays
int CvBonusInfo::getYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiYieldChange ? m_aiYieldChange[i] : -1;
}
int* CvBonusInfo::getYieldChangeArray()
{
	return m_aiYieldChange;
}
int CvBonusInfo::getImprovementChange(int i) const
{
	FAssertMsg(i < GC.getNumImprovementInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiImprovementChange ? m_aiImprovementChange[i] : -1;
}
bool CvBonusInfo::isTerrain(int i) const
{
	FAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abTerrain ?	m_abTerrain[i] : false;
}
bool CvBonusInfo::isFeature(int i) const
{
	FAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abFeature ? m_abFeature[i] : false;
}
bool CvBonusInfo::isFeatureTerrain(int i) const
{
	FAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abFeatureTerrain ?	m_abFeatureTerrain[i] : false;
}
const char* CvBonusInfo::getButton() const
{
	const CvArtInfoBonus * pBonusArtInfo;
	pBonusArtInfo = getArtInfo();
	if (pBonusArtInfo != NULL)
	{
		return pBonusArtInfo->getButton();
	}
	else
	{
		return NULL;
	}
}
int CvBonusInfo::getBuilding() const
{
	return m_iBuilding;
}
void CvBonusInfo::read(FDataStreamBase* stream)
{
	CvInfoBase::read(stream);
	uint uiFlag=0;
	stream->Read(&uiFlag);		// flag for expansion
	stream->Read(&m_iChar);
	stream->Read(&m_iHealthEffectFromRessource);	// WTP, ray, Health Overhaul
	stream->Read(&m_iAIObjective);
	stream->Read(&m_iMinAreaSize);
	stream->Read(&m_iMinLatitude);
	stream->Read(&m_iMaxLatitude);
	stream->Read(&m_iPlacementOrder);
	stream->Read(&m_iConstAppearance);
	stream->Read(&m_iRandAppearance1);
	stream->Read(&m_iRandAppearance2);
	stream->Read(&m_iRandAppearance3);
	stream->Read(&m_iRandAppearance4);
	stream->Read(&m_iPercentPerPlayer);
	stream->Read(&m_iTilesPer);
	stream->Read(&m_iMinLandPercent);
	stream->Read(&m_iUniqueRange);
	stream->Read(&m_iGroupRange);
	stream->Read(&m_iGroupRand);
	stream->Read(&m_iBuilding);
	stream->Read(&m_bOneArea);
	stream->Read(&m_bPeaks); // R&R, ray, Bonus on Peaks - START
	stream->Read(&m_bHills);
	stream->Read(&m_bOcean); //TAC Whaling, ray
	stream->Read(&m_bFlatlands);
	stream->Read(&m_bNoRiverSide);
	stream->Read(&m_bRiverSideOnly); // Ray, adding 2 more XML tags to control bonus placement
	stream->Read(&m_bCoastalLandOnly); // Ray, adding 2 more XML tags to control bonus placement
	stream->Read(&m_bOnlySouthernHemisphere); //ray, Norther and Southern Hemisphere, using hint of f1rpo
	stream->Read(&m_bOnlyNorthernHemisphere); //ray, Norther and Southern Hemisphere, using hint of f1rpo
	stream->Read(&m_bWhalingboatWorkable); //TAC Whaling, ray
	stream->Read(&m_bFishingboatWorkable); //TAC Whaling, ray
	stream->Read(&m_bUseLSystem);
	stream->ReadString(m_szArtDefineTag);
	// Arrays
	SAFE_DELETE_ARRAY(m_aiYieldChange);
	m_aiYieldChange = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_aiYieldChange);
	SAFE_DELETE_ARRAY(m_aiImprovementChange);
	m_aiImprovementChange = new int[GC.getNumImprovementInfos()];
	stream->Read(GC.getNumImprovementInfos(), m_aiImprovementChange);
	SAFE_DELETE_ARRAY(m_abTerrain);
	m_abTerrain = new bool[GC.getNumTerrainInfos()];
	stream->Read(GC.getNumTerrainInfos(), m_abTerrain);
	SAFE_DELETE_ARRAY(m_abFeature);
	m_abFeature = new bool[GC.getNumFeatureInfos()];
	stream->Read(GC.getNumFeatureInfos(), m_abFeature);
	SAFE_DELETE_ARRAY(m_abFeatureTerrain);
	m_abFeatureTerrain = new bool[GC.getNumTerrainInfos()];
	stream->Read(GC.getNumTerrainInfos(), m_abFeatureTerrain);
}
void CvBonusInfo::write(FDataStreamBase* stream)
{
	CvInfoBase::write(stream);
	uint uiFlag=0;
	stream->Write(uiFlag);		// flag for expansion
	stream->Write(m_iChar);
	stream->Write(m_iHealthEffectFromRessource);	// WTP, ray, Health Overhaul
	stream->Write(m_iAIObjective);
	stream->Write(m_iMinAreaSize);
	stream->Write(m_iMinLatitude);
	stream->Write(m_iMaxLatitude);
	stream->Write(m_iPlacementOrder);
	stream->Write(m_iConstAppearance);
	stream->Write(m_iRandAppearance1);
	stream->Write(m_iRandAppearance2);
	stream->Write(m_iRandAppearance3);
	stream->Write(m_iRandAppearance4);
	stream->Write(m_iPercentPerPlayer);
	stream->Write(m_iTilesPer);
	stream->Write(m_iMinLandPercent);
	stream->Write(m_iUniqueRange);
	stream->Write(m_iGroupRange);
	stream->Write(m_iGroupRand);
	stream->Write(m_iBuilding);
	stream->Write(m_bOneArea);
	stream->Write(m_bPeaks); // R&R, ray, Bonus on Peaks - START
	stream->Write(m_bHills);
	stream->Write(m_bOcean); //TAC Whaling, ray
	stream->Write(m_bFlatlands);
	stream->Write(m_bNoRiverSide);
	stream->Write(m_bRiverSideOnly); // Ray, adding 2 more XML tags to control bonus placement
	stream->Write(m_bCoastalLandOnly); // Ray, adding 2 more XML tags to control bonus placement
	stream->Write(m_bOnlySouthernHemisphere); //ray, Norther and Southern Hemisphere, using hint of f1rpo
	stream->Write(m_bOnlyNorthernHemisphere); //ray, Norther and Southern Hemisphere, using hint of f1rpo
	stream->Write(m_bWhalingboatWorkable); //TAC Whaling, ray
	stream->Write(m_bFishingboatWorkable); // R&R, ray, High Sea Fishing
	stream->Write(m_bUseLSystem);
	stream->WriteString(m_szArtDefineTag);
	// Arrays
	stream->Write(NUM_YIELD_TYPES, m_aiYieldChange);
	stream->Write(GC.getNumImprovementInfos(), m_aiImprovementChange);
	stream->Write(GC.getNumTerrainInfos(), m_abTerrain);
	stream->Write(GC.getNumFeatureInfos(), m_abFeature);
	stream->Write(GC.getNumTerrainInfos(), m_abFeatureTerrain);
}
bool CvBonusInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName( szTextVal, "ArtDefineTag");
	setArtDefineTag(szTextVal);
	pXML->GetChildXmlValByName( szTextVal, "BuildingType");
	m_iBuilding = GC.getInfoTypeForString(szTextVal);
	pXML->SetVariableListTagPair(&m_aiYieldChange, "YieldChanges", NUM_YIELD_TYPES, 0);
	pXML->GetChildXmlValByName(&m_iHealthEffectFromRessource, "iHealthEffectFromRessource");	// WTP, ray, Health Overhaul
	pXML->GetChildXmlValByName(&m_iAIObjective, "iAIObjective");
	pXML->GetChildXmlValByName(&m_iMinAreaSize, "iMinAreaSize");
	pXML->GetChildXmlValByName(&m_iMinLatitude, "iMinLatitude");
	pXML->GetChildXmlValByName(&m_iMaxLatitude, "iMaxLatitude");
	pXML->GetChildXmlValByName(&m_iPlacementOrder, "iPlacementOrder");
	pXML->GetChildXmlValByName(&m_iConstAppearance, "iConstAppearance");
	// if we can set the current xml node to it's next sibling
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"Rands"))
	{
		pXML->GetChildXmlValByName(&m_iRandAppearance1, "iRandApp1");
		pXML->GetChildXmlValByName(&m_iRandAppearance2, "iRandApp2");
		pXML->GetChildXmlValByName(&m_iRandAppearance3, "iRandApp3");
		pXML->GetChildXmlValByName(&m_iRandAppearance4, "iRandApp4");
		// set the current xml node to it's parent node
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	pXML->GetChildXmlValByName(&m_iPercentPerPlayer, "iPlayer");
	pXML->GetChildXmlValByName(&m_iTilesPer, "iTilesPer");
	pXML->GetChildXmlValByName(&m_iMinLandPercent, "iMinLandPercent");
	pXML->GetChildXmlValByName(&m_iUniqueRange, "iUnique");
	pXML->GetChildXmlValByName(&m_iGroupRange, "iGroupRange");
	pXML->GetChildXmlValByName(&m_iGroupRand, "iGroupRand");
	pXML->GetChildXmlValByName(&m_bOneArea, "bArea");
	pXML->GetChildXmlValByName(&m_bPeaks, "bPeaks"); // R&R, ray, Bonus on Peaks - START
	pXML->GetChildXmlValByName(&m_bHills, "bHills");
	pXML->GetChildXmlValByName(&m_bOcean, "bOcean"); //TAC Whaling, ray
	pXML->GetChildXmlValByName(&m_bFlatlands, "bFlatlands");
	pXML->GetChildXmlValByName(&m_bNoRiverSide, "bNoRiverSide");
	pXML->GetChildXmlValByName(&m_bRiverSideOnly, "bRiverSideOnly"); // Ray, adding 2 more XML tags to control bonus placement
	pXML->GetChildXmlValByName(&m_bCoastalLandOnly, "bCoastalLandOnly"); // Ray, adding 2 more XML tags to control bonus placement
	pXML->GetChildXmlValByName(&m_bOnlySouthernHemisphere, "bOnlySouthernHemisphere"); //ray, Norther and Southern Hemisphere, using hint of f1rpo
	pXML->GetChildXmlValByName(&m_bOnlyNorthernHemisphere, "bOnlyNorthernHemisphere"); //ray, Norther and Southern Hemisphere, using hint of f1rpo
	pXML->GetChildXmlValByName(&m_bWhalingboatWorkable, "bWhalingboatWorkable"); //TAC Whaling, ray
	pXML->GetChildXmlValByName(&m_bFishingboatWorkable, "bFishingboatWorkable"); // R&R, ray, High Sea Fishing
	pXML->GetChildXmlValByName(&m_bUseLSystem, "bUseLSystem");
	pXML->SetVariableListTagPair(&m_abTerrain, "TerrainBooleans", GC.getNumTerrainInfos(), false);
	pXML->SetVariableListTagPair(&m_abFeature, "FeatureBooleans", GC.getNumFeatureInfos(), false);
	pXML->SetVariableListTagPair(&m_abFeatureTerrain, "FeatureTerrainBooleans", GC.getNumTerrainInfos(), false);
	return true;
}
//======================================================================================================
//					CvFeatureInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvFeatureInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvFeatureInfo::CvFeatureInfo() :
m_iMovementCost(0),
m_iSeeThroughChange(0),
m_iAppearanceProbability(0),
m_iDisappearanceProbability(0),
m_iGrowthProbability(0),
m_iDefenseModifier(0),
m_iAdvancedStartRemoveCost(0),
// R&R, Robert Surcouf, Damage on Storm plots, Start
m_iTurnDamage(0),
m_bGeneratedEveryRound(false),
// R&R, Robert Surcouf, Damage on Storm plots, End
m_bNoCoast(false),
m_bOnlyCoastalLand(false), //WTP, Feature settings enhancements
m_bNoRiver(false),
m_bNoAdjacent(false),
m_bRequiresFlatlands(false),
m_bRequiresHills(false), //WTP, Feature settings enhancements
m_bRequiresPeaks(false), //WTP, Feature settings enhancements
m_bRequiresRiver(false),
m_bImpassable(false),
// ray, Streams Feature - START
m_bNorthMovementBonus(false),
m_bSouthMovementBonus(false),
m_bEastMovementBonus(false),
m_bWestMovementBonus(false),
m_bNorthEastMovementBonus(false),
m_bNorthWestMovementBonus(false),
m_bSouthEastMovementBonus(false),
m_bSouthWestMovementBonus(false),
// ray, Streams Feature - END

m_bNoCity(false),
m_bNoImprovement(false),
m_bVisibleAlways(false),
m_iWorldSoundscapeScriptId(0),
m_eEffectType(NO_EFFECT),
m_iEffectProbability(0),
m_aiYieldChange(NULL),
m_aiRiverYieldIncrease(NULL),
m_ai3DAudioScriptFootstepIndex(NULL),
m_abTerrain(NULL)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvFeatureInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvFeatureInfo::~CvFeatureInfo()
{
	SAFE_DELETE_ARRAY(m_aiYieldChange);
	SAFE_DELETE_ARRAY(m_aiRiverYieldIncrease);
	SAFE_DELETE_ARRAY(m_ai3DAudioScriptFootstepIndex);
	SAFE_DELETE_ARRAY(m_abTerrain);
}
int CvFeatureInfo::getMovementCost() const
{
	return m_iMovementCost;
}
int CvFeatureInfo::getSeeThroughChange() const
{
	return m_iSeeThroughChange;
}
int CvFeatureInfo::getAppearanceProbability() const
{
	return m_iAppearanceProbability;
}
int CvFeatureInfo::getDisappearanceProbability() const
{
	return m_iDisappearanceProbability;
}
int CvFeatureInfo::getGrowthProbability() const
{
	return m_iGrowthProbability;
}
int CvFeatureInfo::getDefenseModifier() const
{
	return m_iDefenseModifier;
}
int CvFeatureInfo::getAdvancedStartRemoveCost() const
{
	return m_iAdvancedStartRemoveCost;
}
// R&R, Robert Surcouf, Damage on Storm plots, Start
int CvFeatureInfo::getTurnDamage() const
{
	return m_iTurnDamage;
}
bool CvFeatureInfo::isGeneratedEveryRound() const
{
	return m_bGeneratedEveryRound;
}
// R&R, Robert Surcouf, Damage on Storm plots, End
bool CvFeatureInfo::isNoCoast() const
{
	return m_bNoCoast;
}

//WTP, Feature settings enhancements
bool CvFeatureInfo::isOnlyCoastalLand() const
{
	return m_bOnlyCoastalLand;
}

bool CvFeatureInfo::isNoRiver() const
{
	return m_bNoRiver;
}
bool CvFeatureInfo::isNoAdjacent() const
{
	return m_bNoAdjacent;
}
bool CvFeatureInfo::isRequiresFlatlands() const
{
	return m_bRequiresFlatlands;
}

// ray, Streams Feature
bool CvFeatureInfo::isNorthMovementBonus() const
{
	return m_bNorthMovementBonus;
}
bool CvFeatureInfo::isSouthMovementBonus() const
{
	return m_bSouthMovementBonus;
}
bool CvFeatureInfo::isEastMovementBonus() const
{
	return m_bEastMovementBonus;
}
bool CvFeatureInfo::isWestMovementBonus() const
{
	return m_bWestMovementBonus;
}
bool CvFeatureInfo::isNorthEastMovementBonus() const
{
	return m_bNorthEastMovementBonus;
}
bool CvFeatureInfo::isNorthWestMovementBonus() const
{
	return m_bNorthWestMovementBonus;
}
bool CvFeatureInfo::isSouthEastMovementBonus() const
{
	return m_bSouthEastMovementBonus;
}
bool CvFeatureInfo::isSouthWestMovementBonus() const
{
	return m_bSouthWestMovementBonus;
}

//WTP, Feature settings enhancements
bool CvFeatureInfo::isRequiresHills() const
{
	return m_bRequiresHills;
}
bool CvFeatureInfo::isRequiresPeaks() const
{
	return m_bRequiresPeaks;
}

bool CvFeatureInfo::isRequiresRiver() const
{
	return m_bRequiresRiver;
}
bool CvFeatureInfo::isImpassable() const
{
	return m_bImpassable;
}
bool CvFeatureInfo::isNoCity() const
{
	return m_bNoCity;
}
bool CvFeatureInfo::isNoImprovement() const
{
	return m_bNoImprovement;
}
bool CvFeatureInfo::isVisibleAlways() const
{
	return m_bVisibleAlways;
}
const char* CvFeatureInfo::getOnUnitChangeTo() const
{
	return m_szOnUnitChangeTo;
}
const char* CvFeatureInfo::getArtDefineTag() const
{
	return m_szArtDefineTag;
}
void CvFeatureInfo::setArtDefineTag(const char* szTag)
{
	m_szArtDefineTag = szTag;
}
int CvFeatureInfo::getWorldSoundscapeScriptId() const
{
	return m_iWorldSoundscapeScriptId;
}
EffectTypes CvFeatureInfo::getEffectType() const
{
	return m_eEffectType;
}
int CvFeatureInfo::getEffectProbability() const
{
	return m_iEffectProbability;
}
// Arrays
int CvFeatureInfo::getYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiYieldChange ? m_aiYieldChange[i] : -1;
}
int CvFeatureInfo::getRiverYieldIncrease(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiRiverYieldIncrease ? m_aiRiverYieldIncrease[i] : -1;
}
int CvFeatureInfo::get3DAudioScriptFootstepIndex(int i) const
{
	//	FAssertMsg(i < ?, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_ai3DAudioScriptFootstepIndex ? m_ai3DAudioScriptFootstepIndex[i] : -1;
}
bool CvFeatureInfo::isTerrain(int i) const
{
	FAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abTerrain ? m_abTerrain[i] : false;
}
int CvFeatureInfo::getNumVarieties() const
{
	return getArtInfo()->getNumVarieties();
}
const char* CvFeatureInfo::getButton() const
{
	const CvArtInfoFeature * pFeatureArtInfo;
	pFeatureArtInfo = getArtInfo();
	if (pFeatureArtInfo != NULL)
	{
		return pFeatureArtInfo->getButton();
	}
	else
	{
		return NULL;
	}
}
const CvArtInfoFeature* CvFeatureInfo::getArtInfo() const
{
	return ARTFILEMGR.getFeatureArtInfo( getArtDefineTag());
}
bool CvFeatureInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName( szTextVal, "ArtDefineTag");
	setArtDefineTag(szTextVal);
	pXML->SetVariableListTagPair(&m_aiYieldChange, "YieldChanges", NUM_YIELD_TYPES, 0);
	pXML->SetVariableListTagPair(&m_aiRiverYieldIncrease, "RiverYieldIncreases", NUM_YIELD_TYPES, 0);
	pXML->GetChildXmlValByName(&m_iMovementCost, "iMovement");
	pXML->GetChildXmlValByName(&m_iSeeThroughChange, "iSeeThrough");
	pXML->GetChildXmlValByName(&m_iDefenseModifier, "iDefense");
	pXML->GetChildXmlValByName(&m_iAdvancedStartRemoveCost, "iAdvancedStartRemoveCost");
	// R&R, Robert Surcouf, Damage on Storm plots, Start
	pXML->GetChildXmlValByName(&m_iTurnDamage, "iTurnDamage");
	pXML->GetChildXmlValByName(&m_bGeneratedEveryRound, "bGeneratedEveryRound");
	// R&R, Robert Surcouf, Damage on Storm plots, End
	pXML->GetChildXmlValByName(&m_iAppearanceProbability, "iAppearance");
	pXML->GetChildXmlValByName(&m_iDisappearanceProbability, "iDisappearance");
	pXML->GetChildXmlValByName(&m_iGrowthProbability, "iGrowth");
	pXML->GetChildXmlValByName(&m_bNoCoast, "bNoCoast");
	pXML->GetChildXmlValByName(&m_bOnlyCoastalLand, "bOnlyCoastalLand"); //WTP, Feature settings enhancements
	pXML->GetChildXmlValByName(&m_bNoRiver, "bNoRiver");
	pXML->GetChildXmlValByName(&m_bNoAdjacent, "bNoAdjacent");
	pXML->GetChildXmlValByName(&m_bRequiresFlatlands, "bRequiresFlatlands");
	pXML->GetChildXmlValByName(&m_bRequiresHills, "bRequiresHills"); //WTP, Feature settings enhancements
	pXML->GetChildXmlValByName(&m_bRequiresPeaks, "bRequiresPeaks"); //WTP, Feature settings enhancements
	pXML->GetChildXmlValByName(&m_bRequiresRiver, "bRequiresRiver");
	pXML->GetChildXmlValByName(&m_bImpassable, "bImpassable");
	// ray, Streams Feature - START
	pXML->GetChildXmlValByName(&m_bNorthMovementBonus, "bNorthMovementBonus");
	pXML->GetChildXmlValByName(&m_bSouthMovementBonus, "bSouthMovementBonus");
	pXML->GetChildXmlValByName(&m_bEastMovementBonus, "bEastMovementBonus");
	pXML->GetChildXmlValByName(&m_bWestMovementBonus, "bWestMovementBonus");
	pXML->GetChildXmlValByName(&m_bNorthEastMovementBonus, "bNorthEastMovementBonus");
	pXML->GetChildXmlValByName(&m_bNorthWestMovementBonus, "bNorthWestMovementBonus");
	pXML->GetChildXmlValByName(&m_bSouthEastMovementBonus, "bSouthEastMovementBonus");
	pXML->GetChildXmlValByName(&m_bSouthWestMovementBonus, "bSouthWestMovementBonus");
	// ray, Streams Feature - END
	pXML->GetChildXmlValByName(&m_bNoCity, "bNoCity");
	pXML->GetChildXmlValByName(&m_bNoImprovement, "bNoImprovement");
	pXML->GetChildXmlValByName(&m_bVisibleAlways, "bVisibleAlways");
	pXML->GetChildXmlValByName(m_szOnUnitChangeTo, "OnUnitChangeTo");
	pXML->SetVariableListTagPairForAudioScripts(&m_ai3DAudioScriptFootstepIndex, "FootstepSounds", GC.getNumFootstepAudioTypes());
	pXML->GetChildXmlValByName(szTextVal, "WorldSoundscapeAudioScript");
	if ( szTextVal.GetLength() > 0 )
	{
		m_iWorldSoundscapeScriptId = gDLL->getAudioTagIndex( szTextVal.GetCString(), AUDIOTAG_SOUNDSCAPE );
	}
	else
	{
		m_iWorldSoundscapeScriptId = -1;
	}
	pXML->GetEnum(getType(), m_eEffectType, "EffectType", false);
	pXML->GetChildXmlValByName(&m_iEffectProbability, "iEffectProbability");
	pXML->SetVariableListTagPair(&m_abTerrain, "TerrainBooleans", GC.getNumTerrainInfos(), false);
	return true;
}
//======================================================================================================
//					CvYieldInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvYieldInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvYieldInfo::CvYieldInfo() :
m_eIndex(NO_YIELD),
m_iChar(0),
m_iBuyPriceLow(0),
m_iBuyPriceHigh(0),
// TAC - Price Limits - Ray - START
m_iBuyPriceMin(0),
// TAC - Price Limits - Ray - END
m_iSellPriceDifference(0),
m_iPriceChangeThreshold(0),
m_iPriceCorrectionPercent(0),
// R&R, Androrc Price Recovery
m_iEuropeVolumeAttrition(0),
//Androrc End
// R&R, ray, Africa
m_iAfricaBuyPriceLow(0),
m_iAfricaBuyPriceHigh(0),
m_iAfricaSellPriceDifference(0),
// R&R, ray, Africa - END
// R&R, ray, Port Royal
m_iPortRoyalBuyPriceLow(0),
m_iPortRoyalBuyPriceHigh(0),
m_iPortRoyalSellPriceDifference(0),
// R&R, ray, Port Royal - END
m_iNativeBuyPrice(0),
m_iNativeSellPrice(0),
m_iNativeConsumptionPercent(0),
m_iNativeHappy(0),
m_iHillsChange(0),
m_iPeakChange(0),
m_iLakeChange(0),
m_iCityChange(0),
m_iMinCity(0),
m_iAIWeightPercent(0),
m_iAIBaseValue(0),
m_iNativeBaseValue(0),
m_iColorType(NO_COLOR),
m_iUnitClass(NO_UNITCLASS),
m_iTextureIndex(-1),
m_iWaterTextureIndex(-1),
m_iPowerValue(0),
m_iAssetValue(0),
m_bCargo(false),
m_bIgnoredForStorageCapacity(false), // ray, making special storage capacity rules for Yields XML configurable
m_bIsExportYield(false), // auto traderoute - Nightinggale
// R&R, Androrc,  Livestock Breeding
m_bLivestock(false)
// R&R, Androrc,  Livestock Breeding, END
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvYieldInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvYieldInfo::~CvYieldInfo()
{
}
int CvYieldInfo::getChar() const
{
	return m_iChar;
}
void CvYieldInfo::setChar(int i)
{
	m_iChar = i;
}
const char* CvYieldInfo::getIcon() const
{
	return m_szIcon;
}

WidgetTypes CvYieldInfo::getWikiWidget() const
{
	return WIDGET_PEDIA_JUMP_TO_YIELDS;
}

YieldTypes CvYieldInfo::getID() const
{
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		YieldTypes eYield = (YieldTypes)iYield;
		if (this == &GC.getYieldInfo(eYield))
		{
			return eYield;
		}
	}
	FAssertMsg(false, "Yield info failed to find itself")
	return NO_YIELD;
}

// KJ Jansson addon for Multiple Professions per Building modcomp by Androrc the Orc START
const char* CvYieldInfo::getCombiIcon() const
{
	return m_szCombiIcon;
}
// KJ Jansson addon for Multiple Professions per Building modcomp by Androrc the Orc END
const char* CvYieldInfo::getHighlightIcon() const
{
	return m_szHightlightIcon;
}
int CvYieldInfo::getBuyPriceLow() const
{
	return m_iBuyPriceLow;
}
int CvYieldInfo::getBuyPriceHigh() const
{
	return m_iBuyPriceHigh;
}

// R&R, ray, Africa
int CvYieldInfo::getAfricaBuyPriceLow() const
{
	return m_iAfricaBuyPriceLow;
}
int CvYieldInfo::getAfricaBuyPriceHigh() const
{
	return m_iAfricaBuyPriceHigh;
}
int CvYieldInfo::getAfricaSellPriceDifference() const
{
	return m_iAfricaSellPriceDifference;
}
// R&R, ray, Africa- END
// R&R, ray, Port Royal
int CvYieldInfo::getPortRoyalBuyPriceLow() const
{
	return m_iPortRoyalBuyPriceLow;
}
int CvYieldInfo::getPortRoyalBuyPriceHigh() const
{
	return m_iPortRoyalBuyPriceHigh;
}
int CvYieldInfo::getPortRoyalSellPriceDifference() const
{
	return m_iPortRoyalSellPriceDifference;
}
// R&R, ray, Port Royal- END

// TAC - Price Limits - Ray - START
int CvYieldInfo::getMinimumBuyPrice() const
{
	return m_iBuyPriceMin;
}
// TAC - Price Limits - Ray - END

int CvYieldInfo::getSellPriceDifference() const
{
	return m_iSellPriceDifference;
}
int CvYieldInfo::getPriceChangeThreshold() const
{
	return m_iPriceChangeThreshold;
}
int CvYieldInfo::getPriceCorrectionPercent() const
{
	return m_iPriceCorrectionPercent;
}
// R&R, Androrc Price Recovery
int CvYieldInfo::getEuropeVolumeAttrition() const
{
	return m_iEuropeVolumeAttrition;
}
//Androrc End
int CvYieldInfo::getNativeBuyPrice() const
{
	return m_iNativeBuyPrice;
}
int CvYieldInfo::getNativeSellPrice() const
{
	return m_iNativeSellPrice;
}
int CvYieldInfo::getNativeConsumptionPercent() const
{
	return m_iNativeConsumptionPercent;
}
int CvYieldInfo::getNativeHappy() const
{
	return m_iNativeHappy;
}
int CvYieldInfo::getHillsChange() const
{
	return m_iHillsChange;
}
int CvYieldInfo::getPeakChange() const
{
	return m_iPeakChange;
}
int CvYieldInfo::getLakeChange() const
{
	return m_iLakeChange;
}
int CvYieldInfo::getCityChange() const
{
	return m_iCityChange;
}
int CvYieldInfo::getMinCity() const
{
	return m_iMinCity;
}
int CvYieldInfo::getAIWeightPercent() const
{
	return m_iAIWeightPercent;
}
int CvYieldInfo::getAIBaseValue() const
{
	return m_iAIBaseValue;
}
int CvYieldInfo::getNativeBaseValue() const
{
	return m_iNativeBaseValue;
}
int CvYieldInfo::getColorType() const
{
	return m_iColorType;
}
int CvYieldInfo::getUnitClass() const
{
	return m_iUnitClass;
}
int CvYieldInfo::getTextureIndex() const
{
	return m_iTextureIndex;
}
int CvYieldInfo::getWaterTextureIndex() const
{
	return m_iWaterTextureIndex;
}
int CvYieldInfo::getPowerValue() const
{
	return m_iPowerValue;
}
int CvYieldInfo::getAssetValue() const
{
	return m_iAssetValue;
}
bool CvYieldInfo::isCargo() const
{
	return m_bCargo;
}
// ray, making special storage capacity rules for Yields XML configurable - START
bool CvYieldInfo::isIgnoredForStorageCapacity() const
{
	return m_bIgnoredForStorageCapacity;
}
// ray, making special storage capacity rules for Yields XML configurable - END
// R&R, Androrc, Livestock Breeding
bool CvYieldInfo::isLivestock() const
{
	return m_bLivestock;
}
// R&R, Androrc, Livestock Breeding, END

bool CvYieldInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	m_eIndex = static_cast<YieldTypes>(getIndexForType(JIT_ARRAY_YIELD, getType()));
	FAssert(m_eIndex != NO_YIELD);

	pXML->GetChildXmlValByName(&m_iBuyPriceLow, "iBuyPriceLow");
	pXML->GetChildXmlValByName(&m_iBuyPriceHigh, "iBuyPriceHigh");
	// TAC - Price Limits - Ray - START
	pXML->GetChildXmlValByName(&m_iBuyPriceMin, "iBuyPriceMin");
	// TAC - Price Limits - Ray - END
	pXML->GetChildXmlValByName(&m_iSellPriceDifference, "iSellPriceDifference");
	pXML->GetChildXmlValByName(&m_iPriceChangeThreshold, "iPriceChangeThreshold");
	pXML->GetChildXmlValByName(&m_iPriceCorrectionPercent, "iPriceCorrectionPercent");
	// R&R, Androrc Price Recovery
	pXML->GetChildXmlValByName(&m_iEuropeVolumeAttrition, "iEuropeVolumeAttrition");
	//Androrc End
	// R&R, ray, Africa
	pXML->GetChildXmlValByName(&m_iAfricaBuyPriceLow, "iAfricaBuyPriceLow");
	pXML->GetChildXmlValByName(&m_iAfricaBuyPriceHigh, "iAfricaBuyPriceHigh");
	pXML->GetChildXmlValByName(&m_iAfricaSellPriceDifference, "iAfricaSellPriceDifference");
	// R&R, ray, Africa - END
	// R&R, ray, Port Royal
	pXML->GetChildXmlValByName(&m_iPortRoyalBuyPriceLow, "iPortRoyalBuyPriceLow");
	pXML->GetChildXmlValByName(&m_iPortRoyalBuyPriceHigh, "iPortRoyalBuyPriceHigh");
	pXML->GetChildXmlValByName(&m_iPortRoyalSellPriceDifference, "iPortRoyalSellPriceDifference");
	// R&R, ray, Port Royal - END
	pXML->GetChildXmlValByName(&m_iNativeBuyPrice, "iNativeBuyPrice");
	pXML->GetChildXmlValByName(&m_iNativeSellPrice, "iNativeSellPrice");
	pXML->GetChildXmlValByName(&m_iNativeConsumptionPercent, "iNativeConsumptionPercent");
	pXML->GetChildXmlValByName(&m_iNativeHappy, "iNativeHappy");
	pXML->GetChildXmlValByName(&m_iHillsChange, "iHillsChange");
	pXML->GetChildXmlValByName(&m_iPeakChange, "iPeakChange");
	pXML->GetChildXmlValByName(&m_iLakeChange, "iLakeChange");
	pXML->GetChildXmlValByName(&m_iCityChange, "iCityChange");
	pXML->GetChildXmlValByName(&m_iMinCity, "iMinCity");
	pXML->GetChildXmlValByName(&m_iAIWeightPercent, "iAIWeightPercent");
	pXML->GetChildXmlValByName(&m_iAIBaseValue, "iAIBaseValue");
	pXML->GetChildXmlValByName(&m_iNativeBaseValue, "iNativeBaseValue");
	pXML->GetChildXmlValByName(szTextVal, "ColorType");
	m_iColorType = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "UnitClass");
	m_iUnitClass = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(&m_iTextureIndex, "iTextureIndex");
	pXML->GetChildXmlValByName(&m_iWaterTextureIndex, "iWaterTextureIndex");
	pXML->GetChildXmlValByName(&m_iPowerValue, "iPower");
	pXML->GetChildXmlValByName(&m_iAssetValue, "iAsset");
	pXML->GetChildXmlValByName(&m_bCargo, "bCargo");
	pXML->GetChildXmlValByName(&m_bIgnoredForStorageCapacity, "bIgnoredForStorageCapacity"); // ray, making special storage capacity rules for Yields XML configurable

	pXML->GetChildXmlValByName(&m_bIsExportYield, "bIsExportYield"); // auto traderoute - Nightinggale
	// R&R, Androrc, Livestock Breeding
	pXML->GetChildXmlValByName(&m_bLivestock, "bLivestock", false);
	// R&R, Androrc, Livestock Breeding, END
	pXML->GetChildXmlValByName(m_szIcon, "Icon");
	// KJ Jansson addon for Multiple Professions per Building modcomp by Androrc the Orc START
	pXML->GetChildXmlValByName(m_szCombiIcon, "CombiIcon");
	// KJ Jansson addon for Multiple Professions per Building modcomp by Androrc the Orc END
	pXML->GetChildXmlValByName(m_szHightlightIcon, "HightlightIcon");

	return true;
}
//======================================================================================================
//					CvTerrainInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvTerrainInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvTerrainInfo::CvTerrainInfo() :
m_iMovementCost(0),
m_iSeeFromLevel(0),
m_iSeeThroughLevel(0),
m_iBuildModifier(0),
m_iDefenseModifier(0),
m_bWater(false),
m_bImpassable(false),
m_bFound(false),
m_bFoundCoast(false),
m_bBadCityLocation(false),// WTP, ray, Health Overhaul
m_iWorldSoundscapeScriptId(0),
m_aiYields(NULL),
m_aiRiverYieldIncrease(NULL),
m_ai3DAudioScriptFootstepIndex(NULL)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvTerrainInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvTerrainInfo::~CvTerrainInfo()
{
	SAFE_DELETE_ARRAY(m_aiYields);
	SAFE_DELETE_ARRAY(m_aiRiverYieldIncrease);
	SAFE_DELETE_ARRAY(m_ai3DAudioScriptFootstepIndex);
}
int CvTerrainInfo::getMovementCost() const
{
	return m_iMovementCost;
}
int CvTerrainInfo::getSeeFromLevel() const
{
	return m_iSeeFromLevel;
}
int CvTerrainInfo::getSeeThroughLevel() const
{
	return m_iSeeThroughLevel;
}
int CvTerrainInfo::getBuildModifier() const
{
	return m_iBuildModifier;
}
int CvTerrainInfo::getDefenseModifier() const
{
	return m_iDefenseModifier;
}
bool CvTerrainInfo::isWater() const
{
	return m_bWater;
}
bool CvTerrainInfo::isImpassable() const
{
	return m_bImpassable;
}
bool CvTerrainInfo::isFound() const
{
	return m_bFound;
}
bool CvTerrainInfo::isFoundCoast() const
{
	return m_bFoundCoast;
}

// WTP, ray, Health Overhaul - START
bool CvTerrainInfo::isBadCityLocation() const
{
	return m_bBadCityLocation;
}
// WTP, ray, Health Overhaul - END

const char* CvTerrainInfo::getArtDefineTag() const
{
	return m_szArtDefineTag;
}
void CvTerrainInfo::setArtDefineTag(const char* szTag)
{
	m_szArtDefineTag = szTag;
}
int CvTerrainInfo::getWorldSoundscapeScriptId() const
{
	return m_iWorldSoundscapeScriptId;
}
// Arrays
int CvTerrainInfo::getYield(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiYields ? m_aiYields[i] : -1;
}
int CvTerrainInfo::getRiverYieldIncrease(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiRiverYieldIncrease ? m_aiRiverYieldIncrease[i] : -1;
}
int CvTerrainInfo::get3DAudioScriptFootstepIndex(int i) const
{
//	FAssertMsg(i < ?, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_ai3DAudioScriptFootstepIndex ? m_ai3DAudioScriptFootstepIndex[i] : -1;
}
bool CvTerrainInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName( szTextVal, "ArtDefineTag");
	setArtDefineTag(szTextVal);
	pXML->SetVariableListTagPair(&m_aiYields, "Yields", NUM_YIELD_TYPES, 0);
	pXML->SetVariableListTagPair(&m_aiRiverYieldIncrease, "RiverYieldIncreases", NUM_YIELD_TYPES, 0);

	pXML->GetChildXmlValByName(&m_bWater, "bWater");
	pXML->GetChildXmlValByName(&m_bImpassable, "bImpassable");
	pXML->GetChildXmlValByName(&m_bFound, "bFound");
	pXML->GetChildXmlValByName(&m_bFoundCoast, "bFoundCoast");
	pXML->GetChildXmlValByName(&m_bBadCityLocation, "bBadCityLocation"); // WTP, ray, Health Overhaul
	pXML->GetChildXmlValByName(&m_iMovementCost, "iMovement");
	pXML->GetChildXmlValByName(&m_iSeeFromLevel, "iSeeFrom");
	pXML->GetChildXmlValByName(&m_iSeeThroughLevel, "iSeeThrough");
	pXML->GetChildXmlValByName(&m_iBuildModifier, "iBuildModifier");
	pXML->GetChildXmlValByName(&m_iDefenseModifier, "iDefense");
	pXML->SetVariableListTagPairForAudioScripts(&m_ai3DAudioScriptFootstepIndex, "FootstepSounds", GC.getNumFootstepAudioTypes());
	pXML->GetChildXmlValByName(szTextVal, "WorldSoundscapeAudioScript");
	if ( szTextVal.GetLength() > 0 )
		m_iWorldSoundscapeScriptId = gDLL->getAudioTagIndex( szTextVal.GetCString(), AUDIOTAG_SOUNDSCAPE );
	else
		m_iWorldSoundscapeScriptId = -1;
	return true;
}
const char* CvTerrainInfo::getButton() const
{
	const CvArtInfoTerrain * pTerrainArtInfo;
	pTerrainArtInfo = getArtInfo();
	if (pTerrainArtInfo != NULL)
	{
		return pTerrainArtInfo->getButton();
	}
	else
	{
		return NULL;
	}
}
const CvArtInfoTerrain* CvTerrainInfo::getArtInfo() const
{
	return ARTFILEMGR.getTerrainArtInfo(getArtDefineTag());
}

bool CvTerrainInfo::canHavePlotType(PlotTypes ePlotType) const
{
	switch (ePlotType)
	{
	case NO_PLOT:
		return true;
	case PLOT_PEAK:
	case PLOT_HILLS:
	case PLOT_LAND:
		return !isWater();
	case PLOT_OCEAN:
		return isWater();
	}
	BOOST_STATIC_ASSERT(NUM_PLOT_TYPES == static_cast<PlotTypes>(4));
	return false;
}

//======================================================================================================
//					CvInterfaceModeInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvInterfaceModeInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvInterfaceModeInfo::CvInterfaceModeInfo() :
m_iCursorIndex(NO_CURSOR),
m_iMissionType(NO_MISSION),
m_bVisible(false),
m_bGotoPlot(false),
m_bHighlightPlot(false),
m_bSelectType(false),
m_bSelectAll(false)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvInterfaceModeInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvInterfaceModeInfo::~CvInterfaceModeInfo()
{
}
int CvInterfaceModeInfo::getCursorIndex() const
{
	return m_iCursorIndex;
}
int CvInterfaceModeInfo::getMissionType() const
{
	return m_iMissionType;
}
bool CvInterfaceModeInfo::getVisible() const
{
	return m_bVisible;
}
bool CvInterfaceModeInfo::getGotoPlot() const
{
	return m_bGotoPlot;
}
bool CvInterfaceModeInfo::getHighlightPlot() const
{
	return m_bHighlightPlot;
}
bool CvInterfaceModeInfo::getSelectType() const
{
	return m_bSelectType;
}
bool CvInterfaceModeInfo::getSelectAll() const
{
	return m_bSelectAll;
}
bool CvInterfaceModeInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(szTextVal, "CursorType");
	m_iCursorIndex = pXML->FindInInfoClass( szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "Mission");
	m_iMissionType = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(&m_bVisible, "bVisible");
	pXML->GetChildXmlValByName(&m_bGotoPlot, "bGotoPlot");
	pXML->GetChildXmlValByName(&m_bHighlightPlot, "bHighlightPlot");
	pXML->GetChildXmlValByName(&m_bSelectType, "bSelectType");
	pXML->GetChildXmlValByName(&m_bSelectAll, "bSelectAll");
	return true;
}
//======================================================================================================
//					CvLeaderHeadInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvLeaderHeadInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvLeaderHeadInfo::CvLeaderHeadInfo() :
m_iAlarmType(NO_ALARM),
m_iBaseAttitude(0),
m_iNativeAttitude(0),
//TAC Native Mercs
m_iMinAttitudeGiveMercs(0),
m_iBaseChanceGiveMercs(0),
//End TAC Native Mercs
// R&R, ray, Native Slave, START
m_iMinAttitudeGiveNativeSlaves(0),
m_iBaseChanceGiveNativeSlaves(0),
// R&R, ray, Native Slave, END
m_iRefuseToTalkWarThreshold(0),
m_iMaxGoldTradePercent(0),
m_iMaxWarRand(0),
m_iMaxWarNearbyPowerRatio(0),
m_iMaxWarDistantPowerRatio(0),
m_iMaxWarMinAdjacentLandPercent(0),
m_iLimitedWarRand(0),
m_iLimitedWarPowerRatio(0),
m_iDogpileWarRand(0),
m_iMakePeaceRand(0),
m_iDeclareWarTradeRand(0),
m_iDemandRebukedSneakProb(0),
m_iDemandRebukedWarProb(0),
m_iRazeCityProb(0),
m_iBaseAttackOddsChange(0),
m_iAttackOddsChangeRand(0),
m_iCloseBordersAttitudeChange(0),
m_iAlarmAttitudeChange(0),
m_iLostWarAttitudeChange(0),
m_iRebelAttitudeDivisor(0),
m_iAtWarAttitudeDivisor(0),
m_iAtWarAttitudeChangeLimit(0),
m_iAtPeaceAttitudeDivisor(0),
m_iAtPeaceAttitudeChangeLimit(0),
m_iOpenBordersAttitudeDivisor(0),
m_iOpenBordersAttitudeChangeLimit(0),
m_iDefensivePactAttitudeDivisor(0),
m_iDefensivePactAttitudeChangeLimit(0),
m_iShareWarAttitudeChange(0),
m_iShareWarAttitudeDivisor(0),
m_iShareWarAttitudeChangeLimit(0),
m_iDemandTributeAttitudeThreshold(NO_ATTITUDE),
m_iNoGiveHelpAttitudeThreshold(NO_ATTITUDE),
m_iMapRefuseAttitudeThreshold(NO_ATTITUDE),
m_iDeclareWarRefuseAttitudeThreshold(NO_ATTITUDE),
m_iDeclareWarThemRefuseAttitudeThreshold(NO_ATTITUDE),
m_iStopTradingRefuseAttitudeThreshold(NO_ATTITUDE),
m_iStopTradingThemRefuseAttitudeThreshold(NO_ATTITUDE),
m_iOpenBordersRefuseAttitudeThreshold(NO_ATTITUDE),
m_iDefensivePactRefuseAttitudeThreshold(NO_ATTITUDE),
m_iPermanentAllianceRefuseAttitudeThreshold(NO_ATTITUDE),
// R&R, ray, Natives raiding party - START
m_iNativeRaidAttitudeThreshold(NO_ATTITUDE),
// R&R, ray, Natives raiding party - END
m_abTraits(NULL),
m_aiContactRand(NULL),
m_aiContactDelay(NULL),
m_aiMemoryDecayRand(NULL),
m_aiMemoryAttitudePercent(NULL),
m_aiNoWarAttitudeProb(NULL),
m_aiUnitAIWeightModifier(NULL),
m_aiImprovementWeightModifier(NULL),
m_aiDiploPeaceMusicScriptIds(NULL),
m_aiDiploWarMusicScriptIds(NULL)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvLeaderHeadInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvLeaderHeadInfo::~CvLeaderHeadInfo()
{
	SAFE_DELETE_ARRAY(m_abTraits);
	SAFE_DELETE_ARRAY(m_aiContactRand);
	SAFE_DELETE_ARRAY(m_aiContactDelay);
	SAFE_DELETE_ARRAY(m_aiMemoryDecayRand);
	SAFE_DELETE_ARRAY(m_aiMemoryAttitudePercent);
	SAFE_DELETE_ARRAY(m_aiNoWarAttitudeProb);
	SAFE_DELETE_ARRAY(m_aiUnitAIWeightModifier);
	SAFE_DELETE_ARRAY(m_aiImprovementWeightModifier);
	SAFE_DELETE_ARRAY(m_aiDiploPeaceMusicScriptIds);
	SAFE_DELETE_ARRAY(m_aiDiploWarMusicScriptIds);
}
const char* CvLeaderHeadInfo::getButton() const
{
	const CvArtInfoLeaderhead * pLeaderheadArtInfo;
	pLeaderheadArtInfo = getArtInfo();
	if (pLeaderheadArtInfo != NULL)
	{
		return pLeaderheadArtInfo->getButton();
	}
	else
	{
		return NULL;
	}
}
int CvLeaderHeadInfo::getAlarmType() const
{
	return m_iAlarmType;
}
int CvLeaderHeadInfo::getBaseAttitude() const
{
	return m_iBaseAttitude;
}
int CvLeaderHeadInfo::getNativeAttitude() const
{
	return m_iNativeAttitude;
}

//TAC Native Mercs
int CvLeaderHeadInfo::getMinAttitudeGiveMercs() const
{
	return m_iMinAttitudeGiveMercs;
}

int CvLeaderHeadInfo::getBaseChanceGiveMercs() const
{
	return m_iBaseChanceGiveMercs;
}
//End TAC Native Mercs

// R&R, ray, Native Slave, START
int CvLeaderHeadInfo::getMinAttitudeGiveNativeSlaves() const
{
	return m_iMinAttitudeGiveNativeSlaves;
}

int CvLeaderHeadInfo::getBaseChanceGiveNativeSlaves() const
{
	return m_iBaseChanceGiveNativeSlaves;
}
// R&R, ray, Native Slave, END

int CvLeaderHeadInfo::getRefuseToTalkWarThreshold() const
{
	return m_iRefuseToTalkWarThreshold;
}
int CvLeaderHeadInfo::getMaxGoldTradePercent() const
{
	return m_iMaxGoldTradePercent;
}
int CvLeaderHeadInfo::getMaxWarRand() const
{
	return m_iMaxWarRand;
}
int CvLeaderHeadInfo::getMaxWarNearbyPowerRatio() const
{
	return m_iMaxWarNearbyPowerRatio;
}
int CvLeaderHeadInfo::getMaxWarDistantPowerRatio() const
{
	return m_iMaxWarDistantPowerRatio;
}
int CvLeaderHeadInfo::getMaxWarMinAdjacentLandPercent() const
{
	return m_iMaxWarMinAdjacentLandPercent;
}
int CvLeaderHeadInfo::getLimitedWarRand() const
{
	return m_iLimitedWarRand;
}
int CvLeaderHeadInfo::getLimitedWarPowerRatio() const
{
	return m_iLimitedWarPowerRatio;
}
int CvLeaderHeadInfo::getDogpileWarRand() const
{
	return m_iDogpileWarRand;
}
int CvLeaderHeadInfo::getMakePeaceRand() const
{
	return m_iMakePeaceRand;
}
int CvLeaderHeadInfo::getDeclareWarTradeRand() const
{
	return m_iDeclareWarTradeRand;
}
int CvLeaderHeadInfo::getDemandRebukedSneakProb() const
{
	return m_iDemandRebukedSneakProb;
}
int CvLeaderHeadInfo::getDemandRebukedWarProb() const
{
	return m_iDemandRebukedWarProb;
}
int CvLeaderHeadInfo::getRazeCityProb() const
{
	return m_iRazeCityProb;
}
int CvLeaderHeadInfo::getBaseAttackOddsChange() const
{
	return m_iBaseAttackOddsChange;
}
int CvLeaderHeadInfo::getAttackOddsChangeRand() const
{
	return m_iAttackOddsChangeRand;
}
int CvLeaderHeadInfo::getCloseBordersAttitudeChange() const
{
	return m_iCloseBordersAttitudeChange;
}
int CvLeaderHeadInfo::getAlarmAttitudeChange() const
{
	return m_iAlarmAttitudeChange;
}
int CvLeaderHeadInfo::getLostWarAttitudeChange() const
{
	return m_iLostWarAttitudeChange;
}
int CvLeaderHeadInfo::getRebelAttitudeDivisor() const
{
	return m_iRebelAttitudeDivisor;
}
int CvLeaderHeadInfo::getAtWarAttitudeDivisor() const
{
	return m_iAtWarAttitudeDivisor;
}
int CvLeaderHeadInfo::getAtWarAttitudeChangeLimit() const
{
	return m_iAtWarAttitudeChangeLimit;
}
int CvLeaderHeadInfo::getAtPeaceAttitudeDivisor() const
{
	return m_iAtPeaceAttitudeDivisor;
}
int CvLeaderHeadInfo::getAtPeaceAttitudeChangeLimit() const
{
	return m_iAtPeaceAttitudeChangeLimit;
}
int CvLeaderHeadInfo::getOpenBordersAttitudeDivisor() const
{
	return m_iOpenBordersAttitudeDivisor;
}
int CvLeaderHeadInfo::getOpenBordersAttitudeChangeLimit() const
{
	return m_iOpenBordersAttitudeChangeLimit;
}
int CvLeaderHeadInfo::getDefensivePactAttitudeDivisor() const
{
	return m_iDefensivePactAttitudeDivisor;
}
int CvLeaderHeadInfo::getDefensivePactAttitudeChangeLimit() const
{
	return m_iDefensivePactAttitudeChangeLimit;
}
int CvLeaderHeadInfo::getShareWarAttitudeChange() const
{
	return m_iShareWarAttitudeChange;
}
int CvLeaderHeadInfo::getShareWarAttitudeDivisor() const
{
	return m_iShareWarAttitudeDivisor;
}
int CvLeaderHeadInfo::getShareWarAttitudeChangeLimit() const
{
	return m_iShareWarAttitudeChangeLimit;
}
int CvLeaderHeadInfo::getDemandTributeAttitudeThreshold() const
{
	return m_iDemandTributeAttitudeThreshold;
}
int CvLeaderHeadInfo::getNoGiveHelpAttitudeThreshold() const
{
	return m_iNoGiveHelpAttitudeThreshold;
}
int CvLeaderHeadInfo::getMapRefuseAttitudeThreshold() const
{
	return m_iMapRefuseAttitudeThreshold;
}
int CvLeaderHeadInfo::getDeclareWarRefuseAttitudeThreshold() const
{
	return m_iDeclareWarRefuseAttitudeThreshold;
}
int CvLeaderHeadInfo::getDeclareWarThemRefuseAttitudeThreshold() const
{
	return m_iDeclareWarThemRefuseAttitudeThreshold;
}
int CvLeaderHeadInfo::getStopTradingRefuseAttitudeThreshold() const
{
	return m_iStopTradingRefuseAttitudeThreshold;
}
int CvLeaderHeadInfo::getStopTradingThemRefuseAttitudeThreshold() const
{
	return m_iStopTradingThemRefuseAttitudeThreshold;
}
int CvLeaderHeadInfo::getOpenBordersRefuseAttitudeThreshold() const
{
	return m_iOpenBordersRefuseAttitudeThreshold;
}
int CvLeaderHeadInfo::getDefensivePactRefuseAttitudeThreshold() const
{
	return m_iDefensivePactRefuseAttitudeThreshold;
}
int CvLeaderHeadInfo::getPermanentAllianceRefuseAttitudeThreshold() const
{
	return m_iPermanentAllianceRefuseAttitudeThreshold;
}
// R&R, ray, Natives raiding party - START
int CvLeaderHeadInfo::getNativeRaidAttitudeThreshold() const
{
	return m_iNativeRaidAttitudeThreshold;
}
// R&R, ray, Natives raiding party - END
const char* CvLeaderHeadInfo::getArtDefineTag() const
{
	return m_szArtDefineTag;
}
void CvLeaderHeadInfo::setArtDefineTag(const char* szVal)
{
	m_szArtDefineTag = szVal;
}
// Arrays
bool CvLeaderHeadInfo::hasTrait(int i) const
{
	FAssertMsg(i < GC.getNumTraitInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_abTraits ? m_abTraits[i] : false;
}
int CvLeaderHeadInfo::getContactRand(int i) const
{
	FAssertMsg(i < NUM_CONTACT_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiContactRand ? m_aiContactRand[i] : -1;
}
int CvLeaderHeadInfo::getContactDelay(int i) const
{
	FAssertMsg(i < NUM_CONTACT_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiContactDelay ? m_aiContactDelay[i] : -1;
}
int CvLeaderHeadInfo::getMemoryDecayRand(int i) const
{
	FAssertMsg(i < NUM_MEMORY_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiMemoryDecayRand ? m_aiMemoryDecayRand[i] : -1;
}
int CvLeaderHeadInfo::getMemoryAttitudePercent(int i) const
{
	FAssertMsg(i < NUM_MEMORY_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiMemoryAttitudePercent ? m_aiMemoryAttitudePercent[i] : -1;
}
int CvLeaderHeadInfo::getNoWarAttitudeProb(int i) const
{
	FAssertMsg(i < NUM_ATTITUDE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiNoWarAttitudeProb ? m_aiNoWarAttitudeProb[i] : -1;
}
int CvLeaderHeadInfo::getUnitAIWeightModifier(int i) const
{
	FAssertMsg(i < NUM_UNITAI_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiUnitAIWeightModifier ? m_aiUnitAIWeightModifier[i] : -1;
}
int CvLeaderHeadInfo::getImprovementWeightModifier(int i) const
{
	FAssertMsg(i < GC.getNumImprovementInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiImprovementWeightModifier ? m_aiImprovementWeightModifier[i] : -1;
}
int CvLeaderHeadInfo::getDiploPeaceMusicScriptIds(int i) const
{
	FAssertMsg(i < GC.getNumEraInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiDiploPeaceMusicScriptIds ? m_aiDiploPeaceMusicScriptIds[i] : -1;
}
int CvLeaderHeadInfo::getDiploWarMusicScriptIds(int i) const
{
	FAssertMsg(i < GC.getNumEraInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiDiploWarMusicScriptIds ? m_aiDiploWarMusicScriptIds[i] : -1;
}
const char* CvLeaderHeadInfo::getLeaderHead() const
{
	const CvArtInfoLeaderhead * pLeaderheadArtInfo;
	pLeaderheadArtInfo = getArtInfo();
	if (pLeaderheadArtInfo != NULL)
	{
		return pLeaderheadArtInfo->getNIF();
	}
	else
	{
		return NULL;
	}
}
void CvLeaderHeadInfo::read(FDataStreamBase* stream)
{
	CvInfoBase::read(stream);
	uint uiFlag=0;
	stream->Read(&uiFlag);		// flag for expansion
	stream->Read(&m_iAlarmType);
	stream->Read(&m_iBaseAttitude);
	stream->Read(&m_iNativeAttitude);
	//TAC Native Mercs
	stream->Read(&m_iMinAttitudeGiveMercs);
	stream->Read(&m_iBaseChanceGiveMercs);
	//End TAC Native Mercs
	// R&R, ray, Native Slave, START
	stream->Read(&m_iMinAttitudeGiveNativeSlaves);
	stream->Read(&m_iBaseChanceGiveNativeSlaves);
	// R&R, ray, Native Slave, END
	stream->Read(&m_iRefuseToTalkWarThreshold);
	stream->Read(&m_iMaxGoldTradePercent);
	stream->Read(&m_iMaxWarRand);
	stream->Read(&m_iMaxWarNearbyPowerRatio);
	stream->Read(&m_iMaxWarDistantPowerRatio);
	stream->Read(&m_iMaxWarMinAdjacentLandPercent);
	stream->Read(&m_iLimitedWarRand);
	stream->Read(&m_iLimitedWarPowerRatio);
	stream->Read(&m_iDogpileWarRand);
	stream->Read(&m_iMakePeaceRand);
	stream->Read(&m_iDeclareWarTradeRand);
	stream->Read(&m_iDemandRebukedSneakProb);
	stream->Read(&m_iDemandRebukedWarProb);
	stream->Read(&m_iRazeCityProb);
	stream->Read(&m_iBaseAttackOddsChange);
	stream->Read(&m_iAttackOddsChangeRand);
	stream->Read(&m_iCloseBordersAttitudeChange);
	stream->Read(&m_iAlarmAttitudeChange);
	stream->Read(&m_iLostWarAttitudeChange);
	stream->Read(&m_iRebelAttitudeDivisor);
	stream->Read(&m_iAtWarAttitudeDivisor);
	stream->Read(&m_iAtWarAttitudeChangeLimit);
	stream->Read(&m_iAtPeaceAttitudeDivisor);
	stream->Read(&m_iAtPeaceAttitudeChangeLimit);
	stream->Read(&m_iOpenBordersAttitudeDivisor);
	stream->Read(&m_iOpenBordersAttitudeChangeLimit);
	stream->Read(&m_iDefensivePactAttitudeDivisor);
	stream->Read(&m_iDefensivePactAttitudeChangeLimit);
	stream->Read(&m_iShareWarAttitudeChange);
	stream->Read(&m_iShareWarAttitudeDivisor);
	stream->Read(&m_iShareWarAttitudeChangeLimit);
	stream->Read(&m_iDemandTributeAttitudeThreshold);
	stream->Read(&m_iNoGiveHelpAttitudeThreshold);
	stream->Read(&m_iMapRefuseAttitudeThreshold);
	stream->Read(&m_iDeclareWarRefuseAttitudeThreshold);
	stream->Read(&m_iDeclareWarThemRefuseAttitudeThreshold);
	stream->Read(&m_iStopTradingRefuseAttitudeThreshold);
	stream->Read(&m_iStopTradingThemRefuseAttitudeThreshold);
	stream->Read(&m_iOpenBordersRefuseAttitudeThreshold);
	stream->Read(&m_iDefensivePactRefuseAttitudeThreshold);
	stream->Read(&m_iPermanentAllianceRefuseAttitudeThreshold);
	// R&R, ray, Natives raiding party - START
	stream->Read(&m_iNativeRaidAttitudeThreshold);
	// R&R, ray, Natives raiding party - END
	stream->ReadString(m_szArtDefineTag);
	// Arrays
	SAFE_DELETE_ARRAY(m_abTraits);
	m_abTraits = new bool[GC.getNumTraitInfos()];
	stream->Read(GC.getNumTraitInfos(), m_abTraits);
	SAFE_DELETE_ARRAY(m_aiContactRand);
	m_aiContactRand = new int[NUM_CONTACT_TYPES];
	stream->Read(NUM_CONTACT_TYPES, m_aiContactRand);
	SAFE_DELETE_ARRAY(m_aiContactDelay);
	m_aiContactDelay = new int[NUM_CONTACT_TYPES];
	stream->Read(NUM_CONTACT_TYPES, m_aiContactDelay);
	SAFE_DELETE_ARRAY(m_aiMemoryDecayRand);
	m_aiMemoryDecayRand = new int[NUM_MEMORY_TYPES];
	stream->Read(NUM_MEMORY_TYPES, m_aiMemoryDecayRand);
	SAFE_DELETE_ARRAY(m_aiMemoryAttitudePercent);
	m_aiMemoryAttitudePercent = new int[NUM_MEMORY_TYPES];
	stream->Read(NUM_MEMORY_TYPES, m_aiMemoryAttitudePercent);
	SAFE_DELETE_ARRAY(m_aiNoWarAttitudeProb);
	m_aiNoWarAttitudeProb = new int[NUM_ATTITUDE_TYPES];
	stream->Read(NUM_ATTITUDE_TYPES, m_aiNoWarAttitudeProb);
	SAFE_DELETE_ARRAY(m_aiUnitAIWeightModifier);
	m_aiUnitAIWeightModifier = new int[NUM_UNITAI_TYPES];
	stream->Read(NUM_UNITAI_TYPES, m_aiUnitAIWeightModifier);
	SAFE_DELETE_ARRAY(m_aiImprovementWeightModifier);
	m_aiImprovementWeightModifier = new int[GC.getNumImprovementInfos()];
	stream->Read(GC.getNumImprovementInfos(), m_aiImprovementWeightModifier);
	SAFE_DELETE_ARRAY(m_aiDiploPeaceMusicScriptIds);
	m_aiDiploPeaceMusicScriptIds = new int[GC.getNumEraInfos()];
	stream->Read(GC.getNumEraInfos(), m_aiDiploPeaceMusicScriptIds);
	SAFE_DELETE_ARRAY(m_aiDiploWarMusicScriptIds);
	m_aiDiploWarMusicScriptIds = new int[GC.getNumEraInfos()];
	stream->Read(GC.getNumEraInfos(), m_aiDiploWarMusicScriptIds);
}
void CvLeaderHeadInfo::write(FDataStreamBase* stream)
{
	CvInfoBase::write(stream);
	uint uiFlag=0;
	stream->Write(uiFlag);		// flag for expansion
	stream->Write(m_iAlarmType);
	stream->Write(m_iBaseAttitude);
	stream->Write(m_iNativeAttitude);
	//TAC Native Mercs
	stream->Write(m_iMinAttitudeGiveMercs);
	stream->Write(m_iBaseChanceGiveMercs);
	//End TAC Native Mercs
	// R&R, ray, Native Slave, START
	stream->Write(m_iMinAttitudeGiveNativeSlaves);
	stream->Write(m_iBaseChanceGiveNativeSlaves);
	// R&R, ray, Native Slave, END
	stream->Write(m_iRefuseToTalkWarThreshold);
	stream->Write(m_iMaxGoldTradePercent);
	stream->Write(m_iMaxWarRand);
	stream->Write(m_iMaxWarNearbyPowerRatio);
	stream->Write(m_iMaxWarDistantPowerRatio);
	stream->Write(m_iMaxWarMinAdjacentLandPercent);
	stream->Write(m_iLimitedWarRand);
	stream->Write(m_iLimitedWarPowerRatio);
	stream->Write(m_iDogpileWarRand);
	stream->Write(m_iMakePeaceRand);
	stream->Write(m_iDeclareWarTradeRand);
	stream->Write(m_iDemandRebukedSneakProb);
	stream->Write(m_iDemandRebukedWarProb);
	stream->Write(m_iRazeCityProb);
	stream->Write(m_iBaseAttackOddsChange);
	stream->Write(m_iAttackOddsChangeRand);
	stream->Write(m_iCloseBordersAttitudeChange);
	stream->Write(m_iAlarmAttitudeChange);
	stream->Write(m_iLostWarAttitudeChange);
	stream->Write(m_iRebelAttitudeDivisor);
	stream->Write(m_iAtWarAttitudeDivisor);
	stream->Write(m_iAtWarAttitudeChangeLimit);
	stream->Write(m_iAtPeaceAttitudeDivisor);
	stream->Write(m_iAtPeaceAttitudeChangeLimit);
	stream->Write(m_iOpenBordersAttitudeDivisor);
	stream->Write(m_iOpenBordersAttitudeChangeLimit);
	stream->Write(m_iDefensivePactAttitudeDivisor);
	stream->Write(m_iDefensivePactAttitudeChangeLimit);
	stream->Write(m_iShareWarAttitudeChange);
	stream->Write(m_iShareWarAttitudeDivisor);
	stream->Write(m_iShareWarAttitudeChangeLimit);
	stream->Write(m_iDemandTributeAttitudeThreshold);
	stream->Write(m_iNoGiveHelpAttitudeThreshold);
	stream->Write(m_iMapRefuseAttitudeThreshold);
	stream->Write(m_iDeclareWarRefuseAttitudeThreshold);
	stream->Write(m_iDeclareWarThemRefuseAttitudeThreshold);
	stream->Write(m_iStopTradingRefuseAttitudeThreshold);
	stream->Write(m_iStopTradingThemRefuseAttitudeThreshold);
	stream->Write(m_iOpenBordersRefuseAttitudeThreshold);
	stream->Write(m_iDefensivePactRefuseAttitudeThreshold);
	stream->Write(m_iPermanentAllianceRefuseAttitudeThreshold);
	// R&R, ray, Natives raiding party - START
	stream->Write(&m_iNativeRaidAttitudeThreshold);
	// R&R, ray, Natives raiding party - END
	stream->WriteString(m_szArtDefineTag);
	// Arrays
	stream->Write(GC.getNumTraitInfos(), m_abTraits);
	stream->Write(NUM_CONTACT_TYPES, m_aiContactRand);
	stream->Write(NUM_CONTACT_TYPES, m_aiContactDelay);
	stream->Write(NUM_MEMORY_TYPES, m_aiMemoryDecayRand);
	stream->Write(NUM_MEMORY_TYPES, m_aiMemoryAttitudePercent);
	stream->Write(NUM_ATTITUDE_TYPES, m_aiNoWarAttitudeProb);
	stream->Write(NUM_UNITAI_TYPES, m_aiUnitAIWeightModifier);
	stream->Write(GC.getNumImprovementInfos(), m_aiImprovementWeightModifier);
	stream->Write(GC.getNumEraInfos(), m_aiDiploPeaceMusicScriptIds);
	stream->Write(GC.getNumEraInfos(), m_aiDiploWarMusicScriptIds);
}
const CvArtInfoLeaderhead* CvLeaderHeadInfo::getArtInfo() const
{
	return ARTFILEMGR.getLeaderheadArtInfo( getArtDefineTag());
}
bool CvLeaderHeadInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(szTextVal, "ArtDefineTag");
	setArtDefineTag(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "AlarmType");
	m_iAlarmType = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(&m_iBaseAttitude, "iBaseAttitude");
	pXML->GetChildXmlValByName(&m_iNativeAttitude, "iNativeAttitude");
	//TAC Native Mercs
	pXML->GetChildXmlValByName(&m_iMinAttitudeGiveMercs, "iMinAttitudeGiveMercs");
	pXML->GetChildXmlValByName(&m_iBaseChanceGiveMercs, "iBaseChanceGiveMercs");
	//End TAC Native Mercs
	// R&R, ray, Native Slave, START
	pXML->GetChildXmlValByName(&m_iMinAttitudeGiveNativeSlaves, "iMinAttitudeGiveNativeSlaves");
	pXML->GetChildXmlValByName(&m_iBaseChanceGiveNativeSlaves, "iBaseChanceGiveNativeSlaves");
	// R&R, ray, Native Slave, END
	pXML->GetChildXmlValByName(&m_iRefuseToTalkWarThreshold, "iRefuseToTalkWarThreshold");
	pXML->GetChildXmlValByName(&m_iMaxGoldTradePercent, "iMaxGoldTradePercent");
	pXML->GetChildXmlValByName(&m_iMaxWarRand, "iMaxWarRand");
	pXML->GetChildXmlValByName(&m_iMaxWarNearbyPowerRatio, "iMaxWarNearbyPowerRatio");
	pXML->GetChildXmlValByName(&m_iMaxWarDistantPowerRatio, "iMaxWarDistantPowerRatio");
	pXML->GetChildXmlValByName(&m_iMaxWarMinAdjacentLandPercent, "iMaxWarMinAdjacentLandPercent");
	pXML->GetChildXmlValByName(&m_iLimitedWarRand, "iLimitedWarRand");
	pXML->GetChildXmlValByName(&m_iLimitedWarPowerRatio, "iLimitedWarPowerRatio");
	pXML->GetChildXmlValByName(&m_iDogpileWarRand, "iDogpileWarRand");
	pXML->GetChildXmlValByName(&m_iMakePeaceRand, "iMakePeaceRand");
	pXML->GetChildXmlValByName(&m_iDeclareWarTradeRand, "iDeclareWarTradeRand");
	pXML->GetChildXmlValByName(&m_iDemandRebukedSneakProb, "iDemandRebukedSneakProb");
	pXML->GetChildXmlValByName(&m_iDemandRebukedWarProb, "iDemandRebukedWarProb");
	pXML->GetChildXmlValByName(&m_iRazeCityProb, "iRazeCityProb");
	pXML->GetChildXmlValByName(&m_iBaseAttackOddsChange, "iBaseAttackOddsChange");
	pXML->GetChildXmlValByName(&m_iAttackOddsChangeRand, "iAttackOddsChangeRand");
	pXML->GetChildXmlValByName(&m_iCloseBordersAttitudeChange, "iCloseBordersAttitudeChange");
	pXML->GetChildXmlValByName(&m_iAlarmAttitudeChange, "iAlarmAttitudeChange");
	pXML->GetChildXmlValByName(&m_iLostWarAttitudeChange, "iLostWarAttitudeChange");
	pXML->GetChildXmlValByName(&m_iRebelAttitudeDivisor, "iRebelAttitudeDivisor");
	pXML->GetChildXmlValByName(&m_iAtWarAttitudeDivisor, "iAtWarAttitudeDivisor");
	pXML->GetChildXmlValByName(&m_iAtWarAttitudeChangeLimit, "iAtWarAttitudeChangeLimit");
	pXML->GetChildXmlValByName(&m_iAtPeaceAttitudeDivisor, "iAtPeaceAttitudeDivisor");
	pXML->GetChildXmlValByName(&m_iAtPeaceAttitudeChangeLimit, "iAtPeaceAttitudeChangeLimit");
	pXML->GetChildXmlValByName(&m_iOpenBordersAttitudeDivisor, "iOpenBordersAttitudeDivisor");
	pXML->GetChildXmlValByName(&m_iOpenBordersAttitudeChangeLimit, "iOpenBordersAttitudeChangeLimit");
	pXML->GetChildXmlValByName(&m_iDefensivePactAttitudeDivisor, "iDefensivePactAttitudeDivisor");
	pXML->GetChildXmlValByName(&m_iDefensivePactAttitudeChangeLimit, "iDefensivePactAttitudeChangeLimit");
	pXML->GetChildXmlValByName(&m_iShareWarAttitudeChange, "iShareWarAttitudeChange");
	pXML->GetChildXmlValByName(&m_iShareWarAttitudeDivisor, "iShareWarAttitudeDivisor");
	pXML->GetChildXmlValByName(&m_iShareWarAttitudeChangeLimit, "iShareWarAttitudeChangeLimit");
	pXML->GetChildXmlValByName(szTextVal, "DemandTributeAttitudeThreshold");
	m_iDemandTributeAttitudeThreshold = pXML->FindInInfoClass( szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "NoGiveHelpAttitudeThreshold");
	m_iNoGiveHelpAttitudeThreshold = pXML->FindInInfoClass( szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "MapRefuseAttitudeThreshold");
	m_iMapRefuseAttitudeThreshold = pXML->FindInInfoClass( szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "DeclareWarRefuseAttitudeThreshold");
	m_iDeclareWarRefuseAttitudeThreshold = pXML->FindInInfoClass( szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "DeclareWarThemRefuseAttitudeThreshold");
	m_iDeclareWarThemRefuseAttitudeThreshold = pXML->FindInInfoClass( szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "StopTradingRefuseAttitudeThreshold");
	m_iStopTradingRefuseAttitudeThreshold = pXML->FindInInfoClass( szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "StopTradingThemRefuseAttitudeThreshold");
	m_iStopTradingThemRefuseAttitudeThreshold = pXML->FindInInfoClass( szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "OpenBordersRefuseAttitudeThreshold");
	m_iOpenBordersRefuseAttitudeThreshold = pXML->FindInInfoClass( szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "DefensivePactRefuseAttitudeThreshold");
	m_iDefensivePactRefuseAttitudeThreshold = pXML->FindInInfoClass( szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "PermanentAllianceRefuseAttitudeThreshold");
	m_iPermanentAllianceRefuseAttitudeThreshold = pXML->FindInInfoClass( szTextVal);
	// R&R, ray, Natives raiding party - START
	pXML->GetChildXmlValByName(szTextVal, "NativeRaidAttitudeThreshold");
	m_iNativeRaidAttitudeThreshold = pXML->FindInInfoClass( szTextVal);
	// R&R, ray, Natives raiding party - END
	pXML->SetVariableListTagPair(&m_abTraits, "Traits", GC.getNumTraitInfos(), false);
	pXML->SetVariableListTagPair(&m_aiContactRand, "ContactRands", NUM_CONTACT_TYPES, 0);
	pXML->SetVariableListTagPair(&m_aiContactDelay, "ContactDelays", NUM_CONTACT_TYPES, 0);
	pXML->SetVariableListTagPair(&m_aiMemoryDecayRand, "MemoryDecays", NUM_MEMORY_TYPES, 0);
	pXML->SetVariableListTagPair(&m_aiMemoryAttitudePercent, "MemoryAttitudePercents", NUM_MEMORY_TYPES, 0);
	pXML->SetVariableListTagPair(&m_aiNoWarAttitudeProb, "NoWarAttitudeProbs", NUM_ATTITUDE_TYPES, 0);
	pXML->SetVariableListTagPair(&m_aiUnitAIWeightModifier, "UnitAIWeightModifiers", NUM_UNITAI_TYPES, 0);
	pXML->SetVariableListTagPair(&m_aiImprovementWeightModifier, "ImprovementWeightModifiers", GC.getNumImprovementInfos(), 0);
	pXML->SetVariableListTagPairForAudioScripts(&m_aiDiploPeaceMusicScriptIds, "DiplomacyMusicPeace", GC.getNumEraInfos());
	pXML->SetVariableListTagPairForAudioScripts(&m_aiDiploWarMusicScriptIds, "DiplomacyMusicWar", GC.getNumEraInfos());
	return true;
}
//======================================================================================================
//					CvWorldInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvWorldInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvWorldInfo::CvWorldInfo() :
m_iDefaultPlayers(0),
m_iDefaultNativePlayers(0),
m_iUnitNameModifier(0),
m_iTargetNumCities(0),
m_iBuildingClassPrereqModifier(0),
m_iGridWidth(0),
m_iGridHeight(0),
m_iTerrainGrainChange(0),
m_iFeatureGrainChange(0),
m_iFatherPercent(0),
m_iAdvancedStartPointsMod(0),
m_iAIImmigrationModifier(0)	// TAC - AI More Immigrants - koma13
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvWorldInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvWorldInfo::~CvWorldInfo()
{
}
int CvWorldInfo::getDefaultPlayers() const
{
	return m_iDefaultPlayers;
}
int CvWorldInfo::getDefaultNativePlayers() const
{
	return m_iDefaultNativePlayers;
}
int CvWorldInfo::getUnitNameModifier() const
{
	return m_iUnitNameModifier;
}
int CvWorldInfo::getTargetNumCities() const
{
	return m_iTargetNumCities;
}
int CvWorldInfo::getBuildingClassPrereqModifier() const
{
	return m_iBuildingClassPrereqModifier;
}
int CvWorldInfo::getGridWidth() const
{
	return m_iGridWidth;
}
int CvWorldInfo::getGridHeight() const
{
	return m_iGridHeight;
}
int CvWorldInfo::getTerrainGrainChange() const
{
	return m_iTerrainGrainChange;
}
int CvWorldInfo::getFeatureGrainChange() const
{
	return m_iFeatureGrainChange;
}
int CvWorldInfo::getFatherPercent() const
{
	return m_iFatherPercent;
}
int CvWorldInfo::getAdvancedStartPointsMod() const
{
	return m_iAdvancedStartPointsMod;
}
// TAC - AI More Immigrants - koma13 - START
int CvWorldInfo::getAIImmigrationModifier() const
{
	return m_iAIImmigrationModifier;
}
// TAC - AI More Immigrants - koma13 - END

bool CvWorldInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(&m_iDefaultPlayers, "iDefaultPlayers");
	pXML->GetChildXmlValByName(&m_iDefaultNativePlayers, "iDefaultNativePlayers");
	pXML->GetChildXmlValByName(&m_iUnitNameModifier, "iUnitNameModifier");
	pXML->GetChildXmlValByName(&m_iTargetNumCities, "iTargetNumCities");
	pXML->GetChildXmlValByName(&m_iBuildingClassPrereqModifier, "iBuildingClassPrereqModifier");
	pXML->GetChildXmlValByName(&m_iGridWidth, "iGridWidth");
	pXML->GetChildXmlValByName(&m_iGridHeight, "iGridHeight");
	pXML->GetChildXmlValByName(&m_iTerrainGrainChange, "iTerrainGrainChange");
	pXML->GetChildXmlValByName(&m_iFeatureGrainChange, "iFeatureGrainChange");
	pXML->GetChildXmlValByName(&m_iFatherPercent, "iFatherPercent");
	pXML->GetChildXmlValByName(&m_iAdvancedStartPointsMod, "iAdvancedStartPointsMod");
	pXML->GetChildXmlValByName(&m_iAIImmigrationModifier, "iAIImmigrationModifier");	// TAC - AI More Immigrants - koma13

	return true;
}
//======================================================================================================
//					CvClimateInfo
//======================================================================================================
CvClimateInfo::CvClimateInfo() :
m_iDesertPercentChange(0),
m_iJungleLatitude(0),
m_iHillRange(0),
m_iPeakPercent(0),
m_fSnowLatitudeChange(0.0f),
m_fTundraLatitudeChange(0.0f),
m_fGrassLatitudeChange(0.0f),
m_fDesertBottomLatitudeChange(0.0f),
m_fDesertTopLatitudeChange(0.0f),
m_fRandIceLatitude(0.0f)
{
}
CvClimateInfo::~CvClimateInfo()
{
}
int CvClimateInfo::getDesertPercentChange() const
{
	return m_iDesertPercentChange;
}
int CvClimateInfo::getJungleLatitude() const
{
	return m_iJungleLatitude;
}
int CvClimateInfo::getHillRange() const
{
	return m_iHillRange;
}
int CvClimateInfo::getPeakPercent() const
{
	return m_iPeakPercent;
}
float CvClimateInfo::getSnowLatitudeChange() const
{
	return m_fSnowLatitudeChange;
}
float CvClimateInfo::getTundraLatitudeChange() const
{
	return m_fTundraLatitudeChange;
}
float CvClimateInfo::getGrassLatitudeChange() const
{
	return m_fGrassLatitudeChange;
}
float CvClimateInfo::getDesertBottomLatitudeChange() const
{
	return m_fDesertBottomLatitudeChange;
}
float CvClimateInfo::getDesertTopLatitudeChange() const
{
	return m_fDesertTopLatitudeChange;
}
float CvClimateInfo::getRandIceLatitude() const
{
	return m_fRandIceLatitude;
}
bool CvClimateInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(&m_iDesertPercentChange, "iDesertPercentChange");
	pXML->GetChildXmlValByName(&m_iJungleLatitude, "iJungleLatitude");
	pXML->GetChildXmlValByName(&m_iHillRange, "iHillRange");
	pXML->GetChildXmlValByName(&m_iPeakPercent, "iPeakPercent");
	pXML->GetChildXmlValByName(&m_fSnowLatitudeChange, "fSnowLatitudeChange");
	pXML->GetChildXmlValByName(&m_fTundraLatitudeChange, "fTundraLatitudeChange");
	pXML->GetChildXmlValByName(&m_fGrassLatitudeChange, "fGrassLatitudeChange");
	pXML->GetChildXmlValByName(&m_fDesertBottomLatitudeChange, "fDesertBottomLatitudeChange");
	pXML->GetChildXmlValByName(&m_fDesertTopLatitudeChange, "fDesertTopLatitudeChange");
	pXML->GetChildXmlValByName(&m_fRandIceLatitude, "fRandIceLatitude");
	return true;
}
//======================================================================================================
//					CvSeaLevelInfo
//======================================================================================================
CvSeaLevelInfo::CvSeaLevelInfo() :
m_iSeaLevelChange(0)
{
}
CvSeaLevelInfo::~CvSeaLevelInfo()
{
}
int CvSeaLevelInfo::getSeaLevelChange() const
{
	return m_iSeaLevelChange;
}
bool CvSeaLevelInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(&m_iSeaLevelChange, "iSeaLevelChange");
	return true;
}

//======================================================================================================
//					CvEuropeInfo
//======================================================================================================
CvEuropeInfo::CvEuropeInfo() :
	m_bStart(false),
	m_iCardinalDirection(NO_CARDINALDIRECTION),
	m_iTripLength(0),
	m_iMinLandDistance(0),
	m_iWidthPercent(0)
{
}
CvEuropeInfo::~CvEuropeInfo()
{
}
bool CvEuropeInfo::isStart() const
{
	return m_bStart;
}
int CvEuropeInfo::getTripLength() const
{
	return m_iTripLength;
}
int CvEuropeInfo::getCardinalDirection() const
{
	return m_iCardinalDirection;
}
int CvEuropeInfo::getMinLandDistance() const
{
	return m_iMinLandDistance;
}
int CvEuropeInfo::getWidthPercent() const
{
	return m_iWidthPercent;
}
bool CvEuropeInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	CvString szVal;
	pXML->GetChildXmlValByName(szVal, "Direction");
	bool bFound = false;
	for(int iDirection=0; iDirection < NUM_CARDINALDIRECTION_TYPES; ++iDirection)
	{
		CvWString szDirectionString;
		getCardinalDirectionTypeString(szDirectionString, (CardinalDirectionTypes) iDirection);
		if (szDirectionString == CvWString(szVal))
		{
			m_iCardinalDirection = iDirection;
			bFound = true;
			break;
		}
	}
	if (!bFound)
	{
		FAssertMsg(false, "Could not match direction string.");
		m_iCardinalDirection = CARDINALDIRECTION_EAST;
	}

	pXML->GetChildXmlValByName(&m_bStart, "bStart");
	pXML->GetChildXmlValByName(&m_iTripLength, "iTripLength");
	pXML->GetChildXmlValByName(&m_iMinLandDistance, "iMinLandDistance");
	pXML->GetChildXmlValByName(&m_iWidthPercent, "iWidthPercent");

	return true;
}

//======================================================================================================
//					CvTraitInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvTraitInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvTraitInfo::CvTraitInfo() :
	m_eCivEffect(NO_CIV_EFFECT),
	m_iLevelExperienceModifier(0),
	m_iGreatGeneralRateModifier(0),
	m_iDomesticGreatGeneralRateModifier(0),
	m_iNativeAngerModifier(0),
	m_iLearnTimeModifier(0),
	m_iNativeCombatModifier(0),
	m_iMissionaryModifier(0),
	m_iNativeTradeModifier(0), // R&R, ray, new Attribute in Traits
	m_iRebelCombatModifier(0),
	m_iTaxRateThresholdModifier(0),
	m_iMaxTaxRateThresholdDecrease(0), // R&R, ray, new Attribute in Traits
	m_iMercantileFactor(0),
	m_iAfricaSellProfitModifierInPercent(0), // WTP, Africa and Port Royal Profit Modifiers - START
	m_iPortRoyalSellProfitModifierInPercent(0), // WTP, Africa and Port Royal Profit Modifiers - START
	m_iDomesticMarketProfitModifierInPercent(0), // WTP, ray, Domestic Market Profit Modifier
	m_iTreasureModifier(0),
	m_iGoodUniqueGoodyChanceModifierLand(0), // WTP, ray, Unique Goody Chance Modifiers - START
	m_iGoodUniqueGoodyChanceModifierWater(0), // WTP, ray, Unique Goody Chance Modifiers - START
	m_iUnhappinessFromSlavesModifier(0), // WTP, ray, Happiness - START
	m_iChiefGoldModifier(0),
	m_iNativeAttitudeChange(0),
	m_iEuropeanAttitudeChange(0), // R&R, ray, new Attribute in Traits
	m_iKingAttitudeChange(0), // R&R, ray, new Attribute in Traits
	m_iCityDefense(0),
	m_iLandPriceDiscount(0),
	m_iRecruitPriceDiscount(0),
	m_iRecruitPriceDiscountAfrica(0), // WTP, ray, Recruit Price Discounts Africa and Port Royal
	m_iRecruitPriceDiscountPortRoyal(0), // WTP, ray, Recruit Price Discounts Africa and Port Royal
	m_iEuropeTravelTimeModifier(0),
	m_iImmigrationThresholdModifier(0),
	m_iPopGrowthThresholdModifier(0),		// Schmiddie, 7 new variables for traits for Europeans, START
	m_iCultureLevelModifier(0),
	m_iPioneerSpeedModifier(0),
	m_iImprovementPriceModifier(0),
	m_iImprovementGrowthTimeModifier(0), // WTP, ray, Improvement Growth Modifier
	m_iLearningByDoingFreeModifier(0), // WTP, ray, adding modifiers for other LBD features - START
	m_iLearningByDoingRunawayModifier(0), // WTP, ray, adding modifiers for other LBD features - START
	m_iLearningByDoingRevoltModifier(0), // WTP, ray, adding modifiers for other LBD features - START
	m_iSpecialistPriceModifier(0),
	m_iStorageCapacityModifier(0),		// Schmiddie, 7 new variables for traits for Europeans, END
	m_aiYieldModifier(NULL),
	m_aiGoodyFactor(NULL),
	m_aiBuildingProductionModifier(NULL),
	m_aiBuildingRequiredYieldModifier(NULL),
	m_aiUnitMoveChange(NULL),
	m_aiUnitStrengthModifier(NULL),
	m_aiProfessionMoveChange(NULL),
	m_aiCityExtraYields(NULL),
	m_aiExtraYieldThreshold(NULL),
	m_aiProfessionEquipmentModifier(NULL),
	m_abTaxYieldModifier(NULL),
	m_abFreePromotionUnitCombat(NULL),
	m_abFreePromotion(NULL),
	m_abFreeBuildingClass(NULL)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvTraitInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvTraitInfo::~CvTraitInfo()
{
	SAFE_DELETE_ARRAY(m_aiCityExtraYields);
	SAFE_DELETE_ARRAY(m_aiExtraYieldThreshold);
	SAFE_DELETE_ARRAY(m_aiProfessionEquipmentModifier);
	SAFE_DELETE_ARRAY(m_aiYieldModifier);
	SAFE_DELETE_ARRAY(m_aiGoodyFactor);
	SAFE_DELETE_ARRAY(m_aiBuildingProductionModifier);
	SAFE_DELETE_ARRAY(m_aiBuildingRequiredYieldModifier);
	SAFE_DELETE_ARRAY(m_aiUnitMoveChange);
	SAFE_DELETE_ARRAY(m_aiUnitStrengthModifier);
	SAFE_DELETE_ARRAY(m_aiProfessionMoveChange);
	SAFE_DELETE_ARRAY(m_abFreePromotionUnitCombat);
	SAFE_DELETE_ARRAY(m_abFreePromotion);
	SAFE_DELETE_ARRAY(m_abFreeBuildingClass);
	SAFE_DELETE_ARRAY(m_abTaxYieldModifier);

	for (uint iBuildingClass = 0; iBuildingClass < m_aaiBuildingYieldChanges.size(); ++iBuildingClass)
	{
		SAFE_DELETE_ARRAY(m_aaiBuildingYieldChanges[iBuildingClass]);
	}
}
int CvTraitInfo::getLevelExperienceModifier() const
{
	return m_iLevelExperienceModifier;
}
int CvTraitInfo::getGreatGeneralRateModifier() const
{
	return m_iGreatGeneralRateModifier;
}
int CvTraitInfo::getDomesticGreatGeneralRateModifier() const
{
	return m_iDomesticGreatGeneralRateModifier;
}
int CvTraitInfo::getNativeAngerModifier() const
{
	return m_iNativeAngerModifier;
}
int CvTraitInfo::getLearnTimeModifier() const
{
	return m_iLearnTimeModifier;
}
int CvTraitInfo::getMercantileFactor() const
{
	return m_iMercantileFactor;
}

int CvTraitInfo::getTreasureModifier() const
{
	return m_iTreasureModifier;
}

// WTP, Africa and Port Royal Profit Modifiers - START
int CvTraitInfo::getAfricaSellProfitModifierInPercent() const
{
	return m_iAfricaSellProfitModifierInPercent;
}
int CvTraitInfo::getPortRoyalSellProfitModifierInPercent() const
{
	return m_iPortRoyalSellProfitModifierInPercent;
}
// WTP, Africa and Port Royal Profit Modifiers - END

// WTP, ray, Domestic Market Profit Modifier - START
int CvTraitInfo::getDomesticMarketProfitModifierInPercent() const
{
	return m_iDomesticMarketProfitModifierInPercent;
}
// WTP, ray, Domestic Market Profit Modifier - END

// WTP, ray, Unique Goody Chance Modifiers - START
int CvTraitInfo::getGoodUniqueGoodyChanceModifierLand() const
{
	return m_iGoodUniqueGoodyChanceModifierLand;
}
int CvTraitInfo::getGoodUniqueGoodyChanceModifierWater() const
{
	return m_iGoodUniqueGoodyChanceModifierWater;
}
// WTP, ray, Unique Goody Chance Modifiers - END
// WTP, ray, Happiness - START
int CvTraitInfo::getUnhappinessFromSlavesModifier() const
{
	return m_iUnhappinessFromSlavesModifier;
}
// WTP, ray, Happiness - END
int CvTraitInfo::getChiefGoldModifier() const
{
	return m_iChiefGoldModifier;
}
int CvTraitInfo::getNativeCombatModifier() const
{
	return m_iNativeCombatModifier;
}
int CvTraitInfo::getMissionaryModifier() const
{
	return m_iMissionaryModifier;
}
int CvTraitInfo::getNativeTradeModifier() const // R&R, ray, new Attribute in Traits
{
	return m_iNativeTradeModifier;
}
int CvTraitInfo::getRebelCombatModifier() const
{
	return m_iRebelCombatModifier;
}
int CvTraitInfo::getTaxRateThresholdModifier() const
{
	return m_iTaxRateThresholdModifier;
}
int CvTraitInfo::getMaxTaxRateThresholdDecrease() const // R&R, ray, new Attribute in Traits
{
	return m_iMaxTaxRateThresholdDecrease;
}
int CvTraitInfo::getNativeAttitudeChange() const
{
	return m_iNativeAttitudeChange;
}
// R&R, ray, new Attribute in Traits - START
int CvTraitInfo::getEuropeanAttitudeChange() const
{
	return m_iEuropeanAttitudeChange;
}
int CvTraitInfo::getKingAttitudeChange() const
{
	return m_iKingAttitudeChange;
}
// R&R, ray, new Attribute in Traits - END
int CvTraitInfo::getCityDefense() const
{
	return m_iCityDefense;
}

int CvTraitInfo::getLandPriceDiscount() const
{
	return m_iLandPriceDiscount;
}

int CvTraitInfo::getRecruitPriceDiscount() const
{
	return m_iRecruitPriceDiscount;
}

// WTP, ray, Recruit Price Discounts Africa and Port Royal - START
int CvTraitInfo::getRecruitPriceDiscountAfrica() const
{
	return m_iRecruitPriceDiscountAfrica;
}

int CvTraitInfo::getRecruitPriceDiscountPortRoyal() const
{
	return m_iRecruitPriceDiscountPortRoyal;
}
// WTP, ray, Recruit Price Discounts Africa and Port Royal - END

int CvTraitInfo::getEuropeTravelTimeModifier() const
{
	return m_iEuropeTravelTimeModifier;
}

int CvTraitInfo::getImmigrationThresholdModifier() const
{
	return m_iImmigrationThresholdModifier;
}
		// Schmiddie, 7 new variables for traits for Europeans, START
int CvTraitInfo::getPopGrowthThresholdModifier() const
{
	return m_iPopGrowthThresholdModifier;
}

int CvTraitInfo::getCultureLevelModifier() const
{
	return m_iCultureLevelModifier;
}

int CvTraitInfo::getPioneerSpeedModifier() const
{
	return m_iPioneerSpeedModifier;
}

int CvTraitInfo::getImprovementPriceModifier() const
{
	return m_iImprovementPriceModifier;
}

// WTP, ray, Improvement Growth Modifier - START
int CvTraitInfo::getImprovementGrowthTimeModifier() const
{
	return m_iImprovementGrowthTimeModifier ;
}
// WTP, ray, Improvement Growth Modifier - END

// WTP, ray, adding modifiers for other LBD features - START
int CvTraitInfo::getLearningByDoingFreeModifier() const
{
	return m_iLearningByDoingFreeModifier;
}

int CvTraitInfo::getLearningByDoingRunawayModifier() const
{
	return m_iLearningByDoingRunawayModifier;
}

int CvTraitInfo::getLearningByDoingRevoltModifier() const
{
	return m_iLearningByDoingRevoltModifier;
}
// WTP, ray, adding modifiers for other LBD features - END

int CvTraitInfo::getSpecialistPriceModifier() const
{
	return m_iSpecialistPriceModifier;
}

int CvTraitInfo::getStorageCapacityModifier() const
{
	return m_iStorageCapacityModifier;
}
		// Schmiddie, 7 new variables for traits for Europeans, END
int CvTraitInfo::getYieldModifier(int i) const
{
	FAssert(i < NUM_YIELD_TYPES);
	FAssert(i >= 0);
	return m_aiYieldModifier ? m_aiYieldModifier[i] : -1;
}

int CvTraitInfo::getGoodyFactor(int i) const
{
	FAssert((i >= 0) && (i < GC.getNumGoodyInfos()));
	return m_aiGoodyFactor ? m_aiGoodyFactor[i] : -1;
}

int CvTraitInfo::getBuildingProductionModifier(int i) const
{
	FAssert((i >= 0) && (i < GC.getNumBuildingClassInfos()));
	return m_aiBuildingProductionModifier ? m_aiBuildingProductionModifier[i] : -1;
}

int CvTraitInfo::getBuildingRequiredYieldModifier(int i) const
{
	FAssert((i >= 0) && (i < NUM_YIELD_TYPES));
	return m_aiBuildingRequiredYieldModifier ? m_aiBuildingRequiredYieldModifier[i] : -1;
}

const int* CvTraitInfo::getBuildingRequiredYieldModifierArray() const
{
	return m_aiBuildingRequiredYieldModifier;
}

int CvTraitInfo::getUnitMoveChange(int i) const
{
	FAssert(i < GC.getNumUnitClassInfos());
	FAssert(i >= 0);
	return m_aiUnitMoveChange ? m_aiUnitMoveChange[i] : -1;
}

int CvTraitInfo::getUnitStrengthModifier(int i) const
{
	FAssert(i < GC.getNumUnitClassInfos());
	FAssert(i >= 0);
	return m_aiUnitStrengthModifier ? m_aiUnitStrengthModifier[i] : -1;
}

int CvTraitInfo::getProfessionMoveChange(int i) const
{
	FAssert(i < GC.getNumProfessionInfos());
	FAssert(i >= 0);
	return m_aiProfessionMoveChange ? m_aiProfessionMoveChange[i] : -1;
}

bool CvTraitInfo::isTaxYieldModifier(int i) const
{
	FAssert(i < NUM_YIELD_TYPES);
	FAssert(i > -1);
	return m_abTaxYieldModifier ? m_abTaxYieldModifier[i] : false;
}

int CvTraitInfo::getBuildingYieldChange(int iBuildingClass, int iYieldType) const
{
	FAssert(iBuildingClass >= 0);
	FAssert(iBuildingClass < GC.getNumBuildingClassInfos());
	FAssert(iYieldType >= 0);
	FAssert(iYieldType < NUM_YIELD_TYPES);

	return m_aaiBuildingYieldChanges[iBuildingClass][iYieldType];
}

const char* CvTraitInfo::getShortDescription() const
{
	return m_szShortDescription;
}
void CvTraitInfo::setShortDescription(const char* szVal)
{
	m_szShortDescription = szVal;
}
// Arrays
int CvTraitInfo::getCityExtraYield(int i) const
{
	return m_aiCityExtraYields ? m_aiCityExtraYields[i] : -1;
}
int CvTraitInfo::getExtraYieldThreshold(int i) const
{
	return m_aiExtraYieldThreshold ? m_aiExtraYieldThreshold[i] : -1;
}
int CvTraitInfo::getProfessionEquipmentModifier(int i) const
{
	return m_aiProfessionEquipmentModifier ? m_aiProfessionEquipmentModifier[i] : -1;
}
int CvTraitInfo::isFreePromotion(int i) const
{
	return m_abFreePromotion ? m_abFreePromotion[i] : -1;
}
int CvTraitInfo::isFreePromotionUnitCombat(int i) const
{
	return m_abFreePromotionUnitCombat ? m_abFreePromotionUnitCombat[i] : -1;
}
bool CvTraitInfo::isFreeBuildingClass(int i) const
{
	return m_abFreeBuildingClass ? m_abFreeBuildingClass[i] : -1;
}
void CvTraitInfo::read(FDataStreamBase* stream)
{
	CvInfoBase::read(stream);

	uint uiFlag=0;
	stream->Read(&uiFlag);	// flags for expansion

	stream->Read(&m_iLevelExperienceModifier);
	stream->Read(&m_iGreatGeneralRateModifier);
	stream->Read(&m_iDomesticGreatGeneralRateModifier);
	stream->Read(&m_iNativeAngerModifier);
	stream->Read(&m_iLearnTimeModifier);
	stream->Read(&m_iNativeCombatModifier);
	stream->Read(&m_iMissionaryModifier);
	stream->Read(&m_iNativeTradeModifier); // R&R, ray, new Attribute in Traits
	stream->Read(&m_iRebelCombatModifier);
	stream->Read(&m_iTaxRateThresholdModifier);
	stream->Read(&m_iMaxTaxRateThresholdDecrease); // R&R, ray, new Attribute in Traits
	stream->Read(&m_iMercantileFactor);
	stream->Read(&m_iAfricaSellProfitModifierInPercent); // WTP, Africa and Port Royal Profit Modifiers - START
	stream->Read(&m_iPortRoyalSellProfitModifierInPercent); // WTP, Africa and Port Royal Profit Modifiers - START
	stream->Read(&m_iDomesticMarketProfitModifierInPercent); // WTP, ray, Domestic Market Profit Modifier
	stream->Read(&m_iTreasureModifier);
	stream->Read(&m_iGoodUniqueGoodyChanceModifierLand); // WTP, ray, Unique Goody Chance Modifiers - START
	stream->Read(&m_iGoodUniqueGoodyChanceModifierWater); // WTP, ray, Unique Goody Chance Modifiers - START
	stream->Read(&m_iUnhappinessFromSlavesModifier); // WTP, ray, Happiness - START
	stream->Read(&m_iChiefGoldModifier);
	stream->Read(&m_iNativeAttitudeChange);
	stream->Read(&m_iEuropeanAttitudeChange); // R&R, ray, new Attribute in Traits
	stream->Read(&m_iKingAttitudeChange); // R&R, ray, new Attribute in Traits
	stream->Read(&m_iCityDefense);
	stream->Read(&m_iLandPriceDiscount);
	stream->Read(&m_iRecruitPriceDiscount);
	stream->Read(&m_iRecruitPriceDiscountAfrica); // WTP, ray, Recruit Price Discounts Africa and Port Royal
	stream->Read(&m_iRecruitPriceDiscountPortRoyal); // WTP, ray, Recruit Price Discounts Africa and Port Royal
	stream->Read(&m_iEuropeTravelTimeModifier);
	stream->Read(&m_iImmigrationThresholdModifier);
	stream->Read(&m_iPopGrowthThresholdModifier);		// Schmiddie, 7 new variables for traits for Europeans, START
	stream->Read(&m_iCultureLevelModifier);
	stream->Read(&m_iPioneerSpeedModifier);
	stream->Read(&m_iImprovementPriceModifier);
	stream->Read(&m_iImprovementGrowthTimeModifier);  // WTP, ray, Improvement Growth Modifier
	stream->Read(&m_iLearningByDoingFreeModifier); // WTP, ray, adding modifiers for other LBD features - START
	stream->Read(&m_iLearningByDoingRunawayModifier); // WTP, ray, adding modifiers for other LBD features - START
	stream->Read(&m_iLearningByDoingRevoltModifier); // WTP, ray, adding modifiers for other LBD features - START
	stream->Read(&m_iSpecialistPriceModifier);
	stream->Read(&m_iStorageCapacityModifier);		// Schmiddie, 7 new variables for traits for Europeans, END
	stream->ReadString(m_szShortDescription);

	SAFE_DELETE_ARRAY(m_aiCityExtraYields);
	m_aiCityExtraYields = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_aiCityExtraYields);

	SAFE_DELETE_ARRAY(m_aiExtraYieldThreshold);
	m_aiExtraYieldThreshold = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_aiExtraYieldThreshold);

	SAFE_DELETE_ARRAY(m_aiProfessionEquipmentModifier);
	m_aiProfessionEquipmentModifier = new int[GC.getNumProfessionInfos()];
	stream->Read(GC.getNumProfessionInfos(), m_aiProfessionEquipmentModifier);

	SAFE_DELETE_ARRAY(m_aiYieldModifier);
	m_aiYieldModifier = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_aiYieldModifier);

	SAFE_DELETE_ARRAY(m_aiGoodyFactor);
	m_aiGoodyFactor = new int[GC.getNumGoodyInfos()];
	stream->Read(GC.getNumGoodyInfos(), m_aiGoodyFactor);

	SAFE_DELETE_ARRAY(m_aiBuildingProductionModifier);
	m_aiBuildingProductionModifier = new int[GC.getNumBuildingClassInfos()];
	stream->Read(GC.getNumBuildingClassInfos(), m_aiBuildingProductionModifier);

	SAFE_DELETE_ARRAY(m_aiBuildingRequiredYieldModifier);
	m_aiBuildingRequiredYieldModifier = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_aiBuildingRequiredYieldModifier);

	SAFE_DELETE_ARRAY(m_aiUnitMoveChange);
	m_aiUnitMoveChange = new int[GC.getNumUnitClassInfos()];
	stream->Read(GC.getNumUnitClassInfos(), m_aiUnitMoveChange);

	SAFE_DELETE_ARRAY(m_aiUnitStrengthModifier);
	m_aiUnitStrengthModifier = new int[GC.getNumUnitClassInfos()];
	stream->Read(GC.getNumUnitClassInfos(), m_aiUnitStrengthModifier);

	SAFE_DELETE_ARRAY(m_aiProfessionMoveChange);
	m_aiProfessionMoveChange = new int[GC.getNumProfessionInfos()];
	stream->Read(GC.getNumProfessionInfos(), m_aiProfessionMoveChange);

	SAFE_DELETE_ARRAY(m_abTaxYieldModifier);
	m_abTaxYieldModifier = new bool[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_abTaxYieldModifier);

	SAFE_DELETE_ARRAY(m_abFreePromotion);
	m_abFreePromotion = new bool[GC.getNumPromotionInfos()];
	stream->Read(GC.getNumPromotionInfos(), m_abFreePromotion);

	SAFE_DELETE_ARRAY(m_abFreePromotionUnitCombat);
	m_abFreePromotionUnitCombat = new bool[GC.getNumUnitCombatInfos()];
	stream->Read(GC.getNumUnitCombatInfos(), m_abFreePromotionUnitCombat);

	SAFE_DELETE_ARRAY(m_abFreeBuildingClass);
	m_abFreeBuildingClass = new bool[GC.getNumBuildingClassInfos()];
	stream->Read(GC.getNumBuildingClassInfos(), m_abFreeBuildingClass);

	for (uint iBuildingClass = 0; iBuildingClass < m_aaiBuildingYieldChanges.size(); ++iBuildingClass)
	{
		SAFE_DELETE_ARRAY(m_aaiBuildingYieldChanges[iBuildingClass]);
	}
	m_aaiBuildingYieldChanges.clear();

	for (int i=0; i< GC.getNumBuildingClassInfos(); i++)
	{
		m_aaiBuildingYieldChanges.push_back(new int[NUM_YIELD_TYPES]);
		stream->Read(NUM_YIELD_TYPES, m_aaiBuildingYieldChanges[i]);
	}
}

void CvTraitInfo::write(FDataStreamBase* stream)
{
	CvInfoBase::write(stream);

	uint uiFlag=0;
	stream->Write(uiFlag);		// flag for expansion

	stream->Write(m_iLevelExperienceModifier);
	stream->Write(m_iGreatGeneralRateModifier);
	stream->Write(m_iDomesticGreatGeneralRateModifier);
	stream->Write(m_iNativeAngerModifier);
	stream->Write(m_iLearnTimeModifier);
	stream->Write(m_iNativeCombatModifier);
	stream->Write(m_iMissionaryModifier);
	stream->Write(m_iNativeTradeModifier); // R&R, ray, new Attribute in Traits
	stream->Write(m_iRebelCombatModifier);
	stream->Write(m_iTaxRateThresholdModifier);
	stream->Write(m_iMaxTaxRateThresholdDecrease); // R&R, ray, new Attribute in Traits
	stream->Write(m_iMercantileFactor);
	stream->Write(m_iAfricaSellProfitModifierInPercent); // WTP, Africa and Port Royal Profit Modifiers - START
	stream->Write(m_iPortRoyalSellProfitModifierInPercent); // WTP, Africa and Port Royal Profit Modifiers - START
	stream->Write(m_iDomesticMarketProfitModifierInPercent); // WTP, ray, Domestic Market Profit Modifier
	stream->Write(m_iTreasureModifier);
	stream->Write(m_iGoodUniqueGoodyChanceModifierLand); // WTP, ray, Unique Goody Chance Modifiers - START
	stream->Write(m_iGoodUniqueGoodyChanceModifierWater); // WTP, ray, Unique Goody Chance Modifiers - START
	stream->Write(m_iUnhappinessFromSlavesModifier); // WTP, ray, Happiness - START
	stream->Write(m_iChiefGoldModifier);
	stream->Write(m_iNativeAttitudeChange);
	stream->Write(m_iEuropeanAttitudeChange); // R&R, ray, new Attribute in Traits
	stream->Write(m_iKingAttitudeChange); // R&R, ray, new Attribute in Traits
	stream->Write(m_iCityDefense);
	stream->Write(m_iLandPriceDiscount);
	stream->Write(m_iRecruitPriceDiscount);
	stream->Write(m_iRecruitPriceDiscountAfrica); // WTP, ray, Recruit Price Discounts Africa and Port Royal
	stream->Write(m_iRecruitPriceDiscountPortRoyal); // WTP, ray, Recruit Price Discounts Africa and Port Royal
	stream->Write(m_iEuropeTravelTimeModifier);
	stream->Write(m_iImmigrationThresholdModifier);
	stream->Write(m_iPopGrowthThresholdModifier);		// Schmiddie, 7 new variables for traits for Europeans, START
	stream->Write(m_iCultureLevelModifier);
	stream->Write(m_iPioneerSpeedModifier);
	stream->Write(m_iImprovementPriceModifier);
	stream->Write(m_iImprovementGrowthTimeModifier); // WTP, ray, Improvement Growth Modifier
	stream->Write(m_iLearningByDoingFreeModifier); // WTP, ray, adding modifiers for other LBD features - START
	stream->Write(m_iLearningByDoingRunawayModifier); // WTP, ray, adding modifiers for other LBD features - START
	stream->Write(m_iLearningByDoingRevoltModifier); // WTP, ray, adding modifiers for other LBD features - STAR
	stream->Write(m_iSpecialistPriceModifier);
	stream->Write(m_iStorageCapacityModifier);		// Schmiddie, 7 new variables for traits for Europeans, END
	stream->WriteString(m_szShortDescription);
	stream->Write(NUM_YIELD_TYPES, m_aiCityExtraYields);
	stream->Write(NUM_YIELD_TYPES, m_aiExtraYieldThreshold);
	stream->Write(GC.getNumProfessionInfos(), m_aiProfessionEquipmentModifier);
	stream->Write(NUM_YIELD_TYPES, m_aiYieldModifier);
	stream->Write(GC.getNumGoodyInfos(), m_aiGoodyFactor);
	stream->Write(GC.getNumBuildingClassInfos(), m_aiBuildingProductionModifier);
	stream->Write(NUM_YIELD_TYPES, m_aiBuildingRequiredYieldModifier);
	stream->Write(GC.getNumUnitClassInfos(), m_aiUnitMoveChange);
	stream->Write(GC.getNumUnitClassInfos(), m_aiUnitStrengthModifier);
	stream->Write(GC.getNumProfessionInfos(), m_aiProfessionMoveChange);
	stream->Write(NUM_YIELD_TYPES, m_abTaxYieldModifier);
	stream->Write(GC.getNumPromotionInfos(), m_abFreePromotion);
	stream->Write(GC.getNumUnitCombatInfos(), m_abFreePromotionUnitCombat);
	stream->Write(GC.getNumBuildingClassInfos(), m_abFreeBuildingClass);

	for (int i=0; i < GC.getNumBuildingClassInfos(); i++)
	{
		stream->Write(NUM_YIELD_TYPES, m_aaiBuildingYieldChanges[i]);
	}
}

bool CvTraitInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(szTextVal, "ShortDescription");
	setShortDescription(szTextVal);

	pXML->GetEnum(getType(), m_eCivEffect, "eCivEffect", false);

	pXML->GetChildXmlValByName(&m_iLevelExperienceModifier, "iLevelExperienceModifier");
	pXML->GetChildXmlValByName(&m_iGreatGeneralRateModifier, "iGreatGeneralRateModifier");
	pXML->GetChildXmlValByName(&m_iDomesticGreatGeneralRateModifier, "iDomesticGreatGeneralRateModifier");
	pXML->GetChildXmlValByName(&m_iNativeAngerModifier, "iNativeAngerModifier");
	pXML->GetChildXmlValByName(&m_iLearnTimeModifier, "iLearnTimeModifier");
	pXML->GetChildXmlValByName(&m_iNativeCombatModifier, "iNativeCombatModifier");
	pXML->GetChildXmlValByName(&m_iMissionaryModifier, "iMissionaryModifier");
	pXML->GetChildXmlValByName(&m_iNativeTradeModifier, "iNativeTradeModifier"); // R&R, ray, new Attribute in Traits
	pXML->GetChildXmlValByName(&m_iRebelCombatModifier, "iRebelCombatModifier");
	pXML->GetChildXmlValByName(&m_iTaxRateThresholdModifier, "iTaxRateThresholdModifier");
	pXML->GetChildXmlValByName(&m_iMaxTaxRateThresholdDecrease, "iMaxTaxRateThresholdDecrease"); // R&R, ray, new Attribute in Traits
	pXML->GetChildXmlValByName(&m_iMercantileFactor, "iMercantileFactor");
	pXML->GetChildXmlValByName(&m_iAfricaSellProfitModifierInPercent, "iAfricaSellProfitModifierInPercent"); // WTP, Africa and Port Royal Profit Modifiers - START
	pXML->GetChildXmlValByName(&m_iPortRoyalSellProfitModifierInPercent, "iPortRoyalSellProfitModifierInPercent"); // WTP, Africa and Port Royal Profit Modifiers - START
	pXML->GetChildXmlValByName(&m_iDomesticMarketProfitModifierInPercent, "iDomesticMarketProfitModifierInPercent"); // WTP, ray, Domestic Market Profit Modifier
	pXML->GetChildXmlValByName(&m_iTreasureModifier, "iTreasureModifier");
	pXML->GetChildXmlValByName(&m_iGoodUniqueGoodyChanceModifierLand, "iGoodUniqueGoodyChanceModifierLand"); // WTP, ray, Unique Goody Chance Modifiers - START
	pXML->GetChildXmlValByName(&m_iGoodUniqueGoodyChanceModifierWater, "iGoodUniqueGoodyChanceModifierWater"); // WTP, ray, Unique Goody Chance Modifiers - START
	pXML->GetChildXmlValByName(&m_iUnhappinessFromSlavesModifier, "iUnhappinessFromSlavesModifier"); // WTP, ray, Happiness - START
	pXML->GetChildXmlValByName(&m_iChiefGoldModifier, "iChiefGoldModifier");
	pXML->GetChildXmlValByName(&m_iNativeAttitudeChange, "iNativeAttitudeChange");
	pXML->GetChildXmlValByName(&m_iEuropeanAttitudeChange, "iEuropeanAttitudeChange"); // R&R, ray, new Attribute in Traits
	pXML->GetChildXmlValByName(&m_iKingAttitudeChange, "iKingAttitudeChange"); // R&R, ray, new Attribute in Traits
	pXML->GetChildXmlValByName(&m_iCityDefense, "iCityDefense");
	pXML->GetChildXmlValByName(&m_iLandPriceDiscount, "iLandPriceDiscount");
	pXML->GetChildXmlValByName(&m_iRecruitPriceDiscount, "iRecruitPriceDiscount");
	pXML->GetChildXmlValByName(&m_iRecruitPriceDiscountAfrica, "iRecruitPriceDiscountAfrica"); // WTP, ray, Recruit Price Discounts Africa and Port Royal
	pXML->GetChildXmlValByName(&m_iRecruitPriceDiscountPortRoyal, "iRecruitPriceDiscountPortRoyal"); // WTP, ray, Recruit Price Discounts Africa and Port Royal
	pXML->GetChildXmlValByName(&m_iEuropeTravelTimeModifier, "iEuropeTravelTimeModifier");
	pXML->GetChildXmlValByName(&m_iImmigrationThresholdModifier, "iImmigrationThresholdModifier");
	pXML->GetChildXmlValByName(&m_iPopGrowthThresholdModifier, "iPopGrowthThresholdModifier");		// Schmiddie, 7 new variables for traits for Europeans, START
	pXML->GetChildXmlValByName(&m_iCultureLevelModifier, "iCultureLevelModifier");
	pXML->GetChildXmlValByName(&m_iPioneerSpeedModifier, "iPioneerSpeedModifier");
	pXML->GetChildXmlValByName(&m_iImprovementPriceModifier, "iImprovementPriceModifier");
	pXML->GetChildXmlValByName(&m_iImprovementGrowthTimeModifier, "iImprovementGrowthTimeModifier"); // WTP, ray, Improvement Growth Modifier
	pXML->GetChildXmlValByName(&m_iLearningByDoingFreeModifier, "iLearningByDoingFreeModifier"); // WTP, ray, adding modifiers for other LBD features - START
	pXML->GetChildXmlValByName(&m_iLearningByDoingRunawayModifier, "iLearningByDoingRunawayModifier"); // WTP, ray, adding modifiers for other LBD features - START
	pXML->GetChildXmlValByName(&m_iLearningByDoingRevoltModifier, "iLearningByDoingRevoltModifier"); // WTP, ray, adding modifiers for other LBD features - START
	pXML->GetChildXmlValByName(&m_iSpecialistPriceModifier, "iSpecialistPriceModifier");
	pXML->GetChildXmlValByName(&m_iStorageCapacityModifier, "iStorageCapacityModifier");		// Schmiddie, 7 new variables for traits for Europeans, END
	pXML->SetVariableListTagPair(&m_aiGoodyFactor, "GoodyFactors", GC.getNumGoodyInfos(), 1);
	pXML->SetVariableListTagPair(&m_aiBuildingProductionModifier, "BuildingProductionModifiers", GC.getNumBuildingClassInfos(), 0);
	pXML->SetVariableListTagPair(&m_aiBuildingRequiredYieldModifier, "BuildingRequiredYieldModifiers", NUM_YIELD_TYPES, 0);
	pXML->SetVariableListTagPair(&m_aiYieldModifier, "YieldModifiers", NUM_YIELD_TYPES, 0);
	pXML->SetVariableListTagPair(&m_aiUnitMoveChange, "UnitMoveChanges", GC.getNumUnitClassInfos(), 0);
	pXML->SetVariableListTagPair(&m_aiUnitStrengthModifier, "UnitStrengthModifiers", GC.getNumUnitClassInfos(), 0);
	pXML->SetVariableListTagPair(&m_aiProfessionMoveChange, "ProfessionMoveChanges", GC.getNumProfessionInfos(), 0);
	pXML->SetVariableListTagPair(&m_abTaxYieldModifier, "TaxYieldModifiers", NUM_YIELD_TYPES, false);

	pXML->Init2DIntList(m_aaiBuildingYieldChanges, GC.getNumBuildingClassInfos(), NUM_YIELD_TYPES);
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"BuildingYieldChanges"))
	{
		if (pXML->SkipToNextVal())
		{
			int iNumSibs = gDLL->getXMLIFace()->GetNumChildren(pXML->GetXML());
			if (gDLL->getXMLIFace()->SetToChild(pXML->GetXML()))
			{
				if (0 < iNumSibs)
				{
					for (int j = 0; j < iNumSibs; j++)
					{
						pXML->GetChildXmlValByName(szTextVal, "BuildingClassType");
						int iIndex = pXML->FindInInfoClass(szTextVal);
						if (iIndex > -1)
						{
							// delete the array since it will be reallocated
							SAFE_DELETE_ARRAY(m_aaiBuildingYieldChanges[iIndex]);
							pXML->SetVariableListTagPair(&m_aaiBuildingYieldChanges[iIndex], "BuildingYields", NUM_YIELD_TYPES, 0);
						}
						if (!gDLL->getXMLIFace()->NextSibling(pXML->GetXML()))
						{
							break;
						}
					}
				}
				gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
			}
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}


	pXML->SetVariableListTagPair(&m_aiCityExtraYields, "CityExtraYields", NUM_YIELD_TYPES, 0);
	pXML->SetVariableListTagPair(&m_aiExtraYieldThreshold, "ExtraYieldThresholds", NUM_YIELD_TYPES, 0);
	pXML->SetVariableListTagPair(&m_aiProfessionEquipmentModifier, "ProfessionEquipmentModifiers", GC.getNumProfessionInfos(), 0);
	pXML->SetVariableListTagPair(&m_abFreePromotion, "FreePromotions", GC.getNumPromotionInfos(), false);
	pXML->SetVariableListTagPair(&m_abFreePromotionUnitCombat, "FreePromotionUnitCombats", GC.getNumUnitCombatInfos(), false);
	pXML->SetVariableListTagPair(&m_abFreeBuildingClass, "FreeBuildingClasses", GC.getNumBuildingClassInfos(), false);
	return true;
}

//======================================================================================================
//					CvCursorInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvCursorInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvCursorInfo::CvCursorInfo()
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvCursorInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvCursorInfo::~CvCursorInfo()
{
}
const char* CvCursorInfo::getPath()
{
	return m_szPath;
}
void CvCursorInfo::setPath(const char* szVal)
{
	m_szPath = szVal;
}
bool CvCursorInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(szTextVal, "CursorPath");
	setPath(szTextVal);
	return true;
}
//======================================================================================================
//					CvSlideShowInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvSlideShowInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvSlideShowInfo::CvSlideShowInfo() :
m_fStartTime(0.0f)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvSlideShowInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvSlideShowInfo::~CvSlideShowInfo()
{
}
const char* CvSlideShowInfo::getPath()
{
	return m_szPath;
}
void CvSlideShowInfo::setPath(const char* szVal)
{
	m_szPath = szVal;
}
const char* CvSlideShowInfo::getTransitionType()
{
	return m_szTransitionType;
}
void CvSlideShowInfo::setTransitionType(const char* szVal)
{
	m_szTransitionType = szVal;
}
float CvSlideShowInfo::getStartTime()
{
	return m_fStartTime;
}
void CvSlideShowInfo::setStartTime(float fVal)
{
	m_fStartTime = fVal;
}
bool CvSlideShowInfo::read(CvXMLLoadUtility* pXML)
{
	float fVal;
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(szTextVal, "Path");
	setPath(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "TransitionType");
	setTransitionType(szTextVal);
	pXML->GetChildXmlValByName(&fVal, "fStartTime");
	setStartTime(fVal);
	return true;
}
//======================================================================================================
//					CvSlideShowRandomInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvSlideShowRandomInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvSlideShowRandomInfo::CvSlideShowRandomInfo()
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvSlideShowRandomInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvSlideShowRandomInfo::~CvSlideShowRandomInfo()
{
}
const char* CvSlideShowRandomInfo::getPath()
{
	return m_szPath;
}
void CvSlideShowRandomInfo::setPath(const char* szVal)
{
	m_szPath = szVal;
}
bool CvSlideShowRandomInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(szTextVal, "Path");
	setPath(szTextVal);
	return true;
}
//======================================================================================================
//					CvWorldPickerInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvWorldPickerInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvWorldPickerInfo::CvWorldPickerInfo()
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvWorldPickerInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvWorldPickerInfo::~CvWorldPickerInfo()
{
}
const char* CvWorldPickerInfo::getMapName()
{
	return m_szMapName;
}
void CvWorldPickerInfo::setMapName(const char* szVal)
{
	m_szMapName = szVal;
}
const char* CvWorldPickerInfo::getModelFile()
{
	return m_szModelFile;
}
void CvWorldPickerInfo::setModelFile(const char* szVal)
{
	m_szModelFile = szVal;
}
int CvWorldPickerInfo::getNumSizes()
{
	return m_aSizes.size();
}
float CvWorldPickerInfo::getSize(int index)
{
	return m_aSizes[index];
}
int CvWorldPickerInfo::getNumClimates()
{
	return m_aClimates.size();
}
const char* CvWorldPickerInfo::getClimatePath(int index)
{
	return m_aClimates[index];
}
int CvWorldPickerInfo::getNumWaterLevelDecals()
{
	return m_aWaterLevelDecals.size();
}
const char* CvWorldPickerInfo::getWaterLevelDecalPath(int index)
{
	return m_aWaterLevelDecals[index];
}
int CvWorldPickerInfo::getNumWaterLevelGloss()
{
	return m_aWaterLevelGloss.size();
}
const char* CvWorldPickerInfo::getWaterLevelGlossPath(int index)
{
	return m_aWaterLevelGloss[index];
}
bool CvWorldPickerInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	float fVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(szTextVal, "MapName");
	setMapName(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "ModelFile");
	setModelFile(szTextVal);
	//sizes
	if(gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "Sizes"))
	{
		if(gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "Size"))
		{
			do
			{
				pXML->GetXmlVal(&fVal);
				m_aSizes.push_back(fVal);
			} while(gDLL->getXMLIFace()->LocateNextSiblingNodeByTagName(pXML->GetXML(), "Size"));
			gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	//climates
	if(gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "Climates"))
	{
		if(gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "ClimatePath"))
		{
			do
			{
				pXML->GetXmlVal(szTextVal);
				m_aClimates.push_back(szTextVal);
			} while(gDLL->getXMLIFace()->LocateNextSiblingNodeByTagName(pXML->GetXML(), "ClimatePath"));
			gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	//water level decals
	if(gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "WaterLevelDecals"))
	{
		if(gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "WaterLevelDecalPath"))
		{
			do
			{
				pXML->GetXmlVal(szTextVal);
				m_aWaterLevelDecals.push_back(szTextVal);
			} while(gDLL->getXMLIFace()->LocateNextSiblingNodeByTagName(pXML->GetXML(), "WaterLevelDecalPath"));
			gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	//water level gloss
	if(gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "WaterLevelGloss"))
	{
		if(gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "WaterLevelGlossPath"))
		{
			do
			{
				pXML->GetXmlVal(szTextVal);
				m_aWaterLevelGloss.push_back(szTextVal);
			} while(gDLL->getXMLIFace()->LocateNextSiblingNodeByTagName(pXML->GetXML(), "WaterLevelGlossPath"));
			gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	return true;
}
//======================================================================================================
//					CvAnimationPathInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvAnimationPathInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvAnimationPathInfo::CvAnimationPathInfo() :
	m_bMissionPath(false)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvAnimationPathInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvAnimationPathInfo::~CvAnimationPathInfo()
{
}
int CvAnimationPathInfo::getPathCategory( int i )
{
	return (int)m_vctPathDefinition.size() > i ? m_vctPathDefinition[i].first : -1;
}
float CvAnimationPathInfo::getPathParameter( int i )
{
	return (int)m_vctPathDefinition.size() > i ? m_vctPathDefinition[i].second : -1;
}
int CvAnimationPathInfo::getNumPathDefinitions()
{
	return m_vctPathDefinition.size();
}
CvAnimationPathDefinition * CvAnimationPathInfo::getPath( )
{
	return &m_vctPathDefinition;
}
bool CvAnimationPathInfo::isMissionPath() const
{
	return m_bMissionPath;
}
//------------------------------------------------------------------------------------------------
// FUNCTION:    CvAnimationPathInfo::read
//! \brief      Reads in a CvAnimationPathInfo definition from XML
//! \param      pXML Pointer to the XML loading object
//! \retval     true if the definition was read successfully, false otherwise
//------------------------------------------------------------------------------------------------
bool CvAnimationPathInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	char	szTempString[1024];				// Extracting text
	int		iCurrentCategory;				// The current category information we are building
	float	fParameter;						// Temporary
	pXML->GetChildXmlValByName( &m_bMissionPath, "bMissionPath" );
	gDLL->getXMLIFace()->SetToChild(pXML->GetXML() );
	gDLL->getXMLIFace()->GetLastNodeText(pXML->GetXML(), szTempString);
	gDLL->getXMLIFace()->NextSibling(pXML->GetXML());
	gDLL->getXMLIFace()->NextSibling(pXML->GetXML());
	do
	{
		if ( pXML->GetChildXmlValByName( szTempString, "Category" ))
		{
			iCurrentCategory = pXML->FindInInfoClass( szTempString);
			fParameter = 0.0f;
		}
		else
		{
			pXML->GetChildXmlValByName( szTempString, "Operator" );
			iCurrentCategory = GC.getInfoTypeForString(szTempString);
			iCurrentCategory = ((int)ANIMOP_FIRST) + iCurrentCategory;
			if ( !pXML->GetChildXmlValByName( &fParameter, "Parameter" ) )
			{
				fParameter = 0.0f;
			}
		}
			m_vctPathDefinition.push_back( std::make_pair(iCurrentCategory, fParameter ));
	}
	while ( gDLL->getXMLIFace()->NextSibling(pXML->GetXML()));
	gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	return true;
}
//======================================================================================================
//					CvAnimationCategoryInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvAnimationCategoryInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvAnimationCategoryInfo::CvAnimationCategoryInfo()
{
	m_kCategory.second = -7540; // invalid.
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvAnimationCategoryInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvAnimationCategoryInfo::~CvAnimationCategoryInfo()
{
}
int CvAnimationCategoryInfo::getCategoryBaseID( )
{
	return m_kCategory.first;
}
int CvAnimationCategoryInfo::getCategoryDefaultTo( )
{
	if ( m_kCategory.second < -1 )
	{
		// CvXMLLoadUtility *pXML = new CvXMLLoadUtility();
		m_kCategory.second = CvXMLLoadUtility::FindInInfoClass( m_szDefaultTo);
	}
	return (int)m_kCategory.second;
}
bool CvAnimationCategoryInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	int		iBaseID;						// Temporary
	pXML->GetChildXmlValByName( m_szDefaultTo, "DefaultTo");
	pXML->GetChildXmlValByName( &iBaseID, "BaseID");
	m_kCategory.first = iBaseID;
	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////
// CvEntityEventInfo
/////////////////////////////////////////////////////////////////////////////////////////////
CvEntityEventInfo::CvEntityEventInfo() :
m_bUpdateFormation(true)
{
}
CvEntityEventInfo::~CvEntityEventInfo()
{
}
bool CvEntityEventInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTmp, szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	int iNumSibs, i;
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"AnimationPathTypes"))
	{
		// Skip any comments and stop at the next value we might want
		if (pXML->SkipToNextVal())
		{
			// get the total number of children the current xml node has
			iNumSibs = gDLL->getXMLIFace()->GetNumChildren(pXML->GetXML());
			if (iNumSibs > 0)
			{
				// if the call to the function that sets the current xml node to it's first non-comment
				// child and sets the parameter with the new node's value succeeds
				if (pXML->GetChildXmlVal(szTmp))
				{
					AnimationPathTypes eAnimationPath = (AnimationPathTypes)CvXMLLoadUtility::FindInInfoClass( szTmp);
					if ( eAnimationPath > ANIMATIONPATH_NONE )
						m_vctAnimationPathType.push_back( eAnimationPath );
					// loop through all the siblings, we start at 1 since we already have the first value
					for (i=1;i<iNumSibs;i++)
					{
						if (!pXML->GetNextXmlVal(&szTmp))
						{
							break;
						}
						AnimationPathTypes eAnimationPath = (AnimationPathTypes)CvXMLLoadUtility::FindInInfoClass( szTmp);
						if ( eAnimationPath > ANIMATIONPATH_NONE )
							m_vctAnimationPathType.push_back( eAnimationPath );
					}
					gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
				}
			}
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"EffectTypes"))
	{
		// Skip any comments and stop at the next value we might want
		if (pXML->SkipToNextVal())
		{
			// get the total number of children the current xml node has
			iNumSibs = gDLL->getXMLIFace()->GetNumChildren(pXML->GetXML());
			if (iNumSibs > 0)
			{
				// if the call to the function that sets the current xml node to it's first non-comment
				// child and sets the parameter with the new node's value succeeds
				if (pXML->GetChildXmlVal(szTmp))
				{
					EffectTypes eEffectType = (EffectTypes)CvXMLLoadUtility::FindInInfoClass( szTmp);
					if ( eEffectType > NO_EFFECT )
						m_vctEffectTypes.push_back( eEffectType );
					// loop through all the siblings, we start at 1 since we already have the first value
					for (i=1;i<iNumSibs;i++)
					{
						if (!pXML->GetNextXmlVal(&szTmp))
						{
							break;
						}
						EffectTypes eEffectType = (EffectTypes)CvXMLLoadUtility::FindInInfoClass( szTmp);
						if ( eEffectType > NO_EFFECT )
							m_vctEffectTypes.push_back( eEffectType );
					}
					gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
				}
			}
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	pXML->GetChildXmlValByName( &m_bUpdateFormation, "bUpdateFormation" );
	return true;
}
AnimationPathTypes CvEntityEventInfo::getAnimationPathType(int iIndex) const
{
	return iIndex >= (int)m_vctAnimationPathType.size() ? ANIMATIONPATH_NONE : m_vctAnimationPathType[iIndex];
}
EffectTypes CvEntityEventInfo::getEffectType(int iIndex) const
{
	return iIndex >= (int)m_vctEffectTypes.size() ? NO_EFFECT : m_vctEffectTypes[iIndex];
}
int CvEntityEventInfo::getAnimationPathCount() const
{
	return m_vctAnimationPathType.size();
}
int CvEntityEventInfo::getEffectTypeCount() const
{
	return m_vctEffectTypes.size();
}
bool CvEntityEventInfo::getUpdateFormation() const
{
	return m_bUpdateFormation;
}
/////////////////////////////////////////////////////////////////////////////////////////////
// CvAssetInfoBase
/////////////////////////////////////////////////////////////////////////////////////////////
const char* CvAssetInfoBase::getTag() const
{
	return getType();
}
void CvAssetInfoBase::setTag(const char* szDesc)
{
	m_szType = szDesc;
}
const char* CvAssetInfoBase::getPath() const
{
	return m_szPath;
}
void CvAssetInfoBase::setPath(const char* szDesc)
{
	m_szPath = szDesc;
}
bool CvAssetInfoBase::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))		// 'tag' is the same as 'type'
	{
		return false;
	}
	pXML->GetChildXmlValByName(szTextVal, "Path");
	setPath(szTextVal);
	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////
// CvArtInfoAsset
/////////////////////////////////////////////////////////////////////////////////////////////
const char* CvArtInfoAsset::getNIF() const
{
	return m_szNIF;
}
const char* CvArtInfoAsset::getKFM() const
{
	return m_szKFM;
}
void CvArtInfoAsset::setNIF(const char* szDesc)
{
	m_szNIF = szDesc;
}
void CvArtInfoAsset::setKFM(const char* szDesc)
{
	m_szKFM = szDesc;
}
bool CvArtInfoAsset::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvAssetInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(szTextVal, "NIF");
	setNIF(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "KFM");
	setKFM(szTextVal);

	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////
// CvArtInfoBonus
/////////////////////////////////////////////////////////////////////////////////////////////
bool CvArtInfoBonus::read(CvXMLLoadUtility* pXML)
{
	if (!CvArtInfoScalableAsset::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(&m_iFontButtonIndex, "FontButtonIndex");
	pXML->GetChildXmlValByName(&m_bShadowCastor, "bShadowCastor");
	pXML->GetChildXmlValByName(&m_bRefractionCastor, "bRefractionCastor");
	return true;
}
CvArtInfoBonus::CvArtInfoBonus()
{
	m_iFontButtonIndex = 0;
	m_bShadowCastor = false;
	m_bRefractionCastor = false;
}
int CvArtInfoBonus::getFontButtonIndex() const
{
	return m_iFontButtonIndex;
}
bool CvArtInfoBonus::isShadowCastor() const
{
	return m_bShadowCastor;
}
bool CvArtInfoBonus::isRefractionCastor() const
{
	return m_bRefractionCastor;
}
const CvArtInfoBonus* CvBonusInfo::getArtInfo() const
{
	return ARTFILEMGR.getBonusArtInfo( getArtDefineTag());
}

/////////////////////////////////////////////////////////////////////////////////////////////
// CvArtInfoUnit
/////////////////////////////////////////////////////////////////////////////////////////////
CvArtInfoUnit::CvArtInfoUnit() :
m_iDamageStates(0),
m_bActAsRanged(false),
m_bCombatExempt(false),
m_fTrailWidth(0.0f),
m_fTrailLength(0.0f),
m_fTrailTaper(0.0f),
m_fTrailFadeStartTime(0.0f),
m_fTrailFadeFalloff(0.0f),
m_fRangedDeathTime(0.0f),
m_fExchangeAngle(0.0f),
m_bSmoothMove(false),
m_fAngleInterRate(FLT_MAX),
m_fBankRate(0),
m_iRunLoopSoundTag(0),
m_iRunEndSoundTag(0),
m_iSelectionSoundScriptId(0),
m_iActionSoundScriptId(0)
{
}
CvArtInfoUnit::~CvArtInfoUnit()
{
}
bool CvArtInfoUnit::getActAsRanged() const
{
	return m_bActAsRanged;
}
const char* CvArtInfoUnit::getFullLengthIcon() const
{
	return m_szFullLengthIcon;
}
int CvArtInfoUnit::getDamageStates() const
{
	return m_iDamageStates;
}
const char* CvArtInfoUnit::getTrailTexture() const
{
	return m_szTrailTexture;
}
float CvArtInfoUnit::getTrailWidth() const
{
	return m_fTrailWidth;
}
float CvArtInfoUnit::getTrailLength() const
{
	return m_fTrailLength;
}
float CvArtInfoUnit::getTrailTaper() const
{
	return m_fTrailTaper;
}
float CvArtInfoUnit::getTrailFadeStarTime() const
{
	return m_fTrailFadeStartTime;
}
float CvArtInfoUnit::getTrailFadeFalloff() const
{
	return m_fTrailFadeFalloff;
}
float CvArtInfoUnit::getBattleDistance() const
{
	return m_fBattleDistance;
}
float CvArtInfoUnit::getRangedDeathTime() const
{
	return m_fRangedDeathTime;
}
float CvArtInfoUnit::getExchangeAngle() const
{
	return m_fExchangeAngle;
}
bool CvArtInfoUnit::getCombatExempt() const
{
	return m_bCombatExempt;
}
bool CvArtInfoUnit::getSmoothMove() const
{
	return m_bSmoothMove;
}
float CvArtInfoUnit::getAngleInterpRate() const
{
	return m_fAngleInterRate;
}
float CvArtInfoUnit::getBankRate() const
{
	return m_fBankRate;
}
bool CvArtInfoUnit::read(CvXMLLoadUtility* pXML)
{
	if (!CvArtInfoScalableAsset::read(pXML))
	{
		return false;
	}
	CvString szTextVal;
	pXML->GetChildXmlValByName(szTextVal, "ActionSound");
	m_iActionSoundScriptId = (szTextVal.GetLength() > 0) ? gDLL->getAudioTagIndex( szTextVal.GetCString(), AUDIOTAG_3DSCRIPT ) : -1;
	pXML->GetChildXmlValByName(szTextVal, "SelectionSound");
	m_iSelectionSoundScriptId = (szTextVal.GetLength() > 0) ? gDLL->getAudioTagIndex( szTextVal.GetCString(), AUDIOTAG_3DSCRIPT ) : -1;
	pXML->GetChildXmlValByName(szTextVal, "TrainSound");
	setTrainSound(szTextVal);
	pXML->GetChildXmlValByName(&m_bActAsRanged, "bActAsRanged" );
	pXML->GetChildXmlValByName(&m_bCombatExempt, "bCombatExempt", false );
	pXML->GetChildXmlValByName(&m_fExchangeAngle, "fExchangeAngle", 0.0f );
	pXML->GetChildXmlValByName(&m_bSmoothMove, "bSmoothMove", false );
	pXML->GetChildXmlValByName(&m_fAngleInterRate, "fAngleInterpRate", FLT_MAX );
	pXML->GetChildXmlValByName(&m_fBankRate, "fBankRate", 0 );
	pXML->GetChildXmlValByName(m_szFullLengthIcon, "FullLengthIcon");
	pXML->GetChildXmlValByName(&m_iDamageStates, "iDamageStates", 0);
	pXML->GetChildXmlValByName(&m_fBattleDistance, "fBattleDistance", 0.0f);
	pXML->GetChildXmlValByName(&m_fRangedDeathTime, "fRangedDeathTime", 0.0f );
	m_fTrailWidth = -1.0f; // invalid.
	if ( gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "TrailDefinition"))
	{
		pXML->GetChildXmlValByName(m_szTrailTexture, "Texture" );
		pXML->GetChildXmlValByName(&m_fTrailWidth, "fWidth" );
		pXML->GetChildXmlValByName(&m_fTrailLength, "fLength" );
		pXML->GetChildXmlValByName(&m_fTrailTaper, "fTaper" );
		pXML->GetChildXmlValByName(&m_fTrailFadeStartTime, "fFadeStartTime" );
		pXML->GetChildXmlValByName(&m_fTrailFadeFalloff, "fFadeFalloff" );
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML() );
	}
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"AudioRunSounds"))
	{
		pXML->GetChildXmlValByName(szTextVal, "AudioRunTypeLoop");
		m_iRunLoopSoundTag = GC.getFootstepAudioTypeByTag(szTextVal);
		pXML->GetChildXmlValByName(szTextVal, "AudioRunTypeEnd");
		m_iRunEndSoundTag = GC.getFootstepAudioTypeByTag(szTextVal);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	return true;
}
const char* CvArtInfoUnit::getTrainSound() const
{
	return m_szTrainSound;
}
void CvArtInfoUnit::setTrainSound(const char* szVal)
{
	m_szTrainSound = szVal;
}
int CvArtInfoUnit::getRunLoopSoundTag() const
{
	return m_iRunLoopSoundTag;
}
int CvArtInfoUnit::getRunEndSoundTag() const
{
	return m_iRunEndSoundTag;
}
int CvArtInfoUnit::getSelectionSoundScriptId() const
{
	return m_iSelectionSoundScriptId;
}
int CvArtInfoUnit::getActionSoundScriptId() const
{
	return m_iActionSoundScriptId;
}
/////////////////////////////////////////////////////////////////////////////////////////////
// CvArtInfoBuilding
/////////////////////////////////////////////////////////////////////////////////////////////
CvArtInfoBuilding::CvArtInfoBuilding() :
m_bAnimated(false)
{
}
CvArtInfoBuilding::~CvArtInfoBuilding()
{
}
bool CvArtInfoBuilding::isAnimated() const
{
	return m_bAnimated;
}
const char* CvArtInfoBuilding::getLSystemName() const
{
	return m_szLSystemName;
}
const char* CvArtInfoBuilding::getCityTexture() const
{
	return m_cityTexture;
}
const char* CvArtInfoBuilding::getCitySelectedTexture() const
{
	return m_citySelectedTexture;
}

bool CvArtInfoBuilding::read(CvXMLLoadUtility* pXML)
{
	if (!CvArtInfoScalableAsset::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(m_cityTexture, "CityTexture");
	pXML->GetChildXmlValByName(m_citySelectedTexture, "CitySelectedTexture");
	pXML->GetChildXmlValByName(m_szLSystemName, "LSystem");
	pXML->GetChildXmlValByName(&m_bAnimated, "bAnimated");
	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////
// CvArtInfoCivilization
/////////////////////////////////////////////////////////////////////////////////////////////
CvArtInfoCivilization::CvArtInfoCivilization() :
m_bWhiteFlag(false),
m_bInvertFlag(false),
m_iFontButtonIndex(0)
{
}
CvArtInfoCivilization::~CvArtInfoCivilization()
{
}
bool CvArtInfoCivilization::isWhiteFlag() const
{
	return m_bWhiteFlag;
}
int CvArtInfoCivilization::getFontButtonIndex() const
{
	return m_iFontButtonIndex;
}
bool CvArtInfoCivilization::isInvertFlag() const
{
	return m_bInvertFlag;
}
bool CvArtInfoCivilization::read(CvXMLLoadUtility* pXML)
{
	if (!CvArtInfoAsset::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(&m_bWhiteFlag, "bWhiteFlag");
	pXML->GetChildXmlValByName(&m_bInvertFlag, "bInvertFlag");
	pXML->GetChildXmlValByName(&m_iFontButtonIndex, "iFontButtonIndex");
	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////
// CvArtInfoLeaderhead
/////////////////////////////////////////////////////////////////////////////////////////////
const char* CvArtInfoLeaderhead::getBackgroundKFM() const
{
	return m_szBackgroundKFM;
}
void CvArtInfoLeaderhead::setBackgroundKFM( const char* szKFM)
{
	m_szBackgroundKFM = szKFM;
}
bool CvArtInfoLeaderhead::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvArtInfoAsset::read(pXML))
	{
		return false;
	}
	if (pXML->GetChildXmlValByName(szTextVal, "BackgroundKFM"))
		setBackgroundKFM(szTextVal);
	else
		setBackgroundKFM("");
	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////
// CvArtInfoScalableAsset
/////////////////////////////////////////////////////////////////////////////////////////////
bool CvArtInfoScalableAsset::read(CvXMLLoadUtility* pXML)
{
	if (!CvArtInfoAsset::read(pXML))
	{
		return false;
	}
	return CvScalableInfo::read(pXML);
}
//////////////////////////////////////////////////////////////////////////
// CvArtInfoImprovement
//////////////////////////////////////////////////////////////////////////
CvArtInfoImprovement::CvArtInfoImprovement() :
m_bExtraAnimations(false)
{
}
CvArtInfoImprovement::~CvArtInfoImprovement()
{
}
bool CvArtInfoImprovement::isExtraAnimations() const
{
	return m_bExtraAnimations;
}
bool CvArtInfoImprovement::read(CvXMLLoadUtility* pXML)
{
	if (!CvArtInfoScalableAsset::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(&m_bExtraAnimations, "bExtraAnimations");
	return true;
}
//////////////////////////////////////////////////////////////////////////
// CvArtInfoTerrain
//////////////////////////////////////////////////////////////////////////
CvArtInfoTerrain::CvArtInfoTerrain() :
m_iLayerOrder(0),
m_eTerrainGroup(TERRAIN_GROUP_NONE),
m_numTextureBlends(16),
m_pTextureSlots(NULL)
{
	m_pTextureSlots = new CvTextureBlendSlotList * [m_numTextureBlends];
	for ( int i = 0; i < m_numTextureBlends; i++ )
	{
		m_pTextureSlots[i] = new CvTextureBlendSlotList;
	}
}
CvArtInfoTerrain::~CvArtInfoTerrain()
{
	for ( int i = 0; i < m_numTextureBlends; i++ )
	{
		SAFE_DELETE(m_pTextureSlots[i]);
	}
	SAFE_DELETE_ARRAY( m_pTextureSlots);
}
const char* CvArtInfoTerrain::getBaseTexture()
{
	return getPath();
}
void CvArtInfoTerrain::setBaseTexture(const char* szTmp )
{
	setPath(szTmp);
}
const char* CvArtInfoTerrain::getGridTexture()
{
	return m_szGridTexture;
}
void CvArtInfoTerrain::setGridTexture(const char* szTmp )
{
	m_szGridTexture = szTmp;
}
const char* CvArtInfoTerrain::getDetailTexture()
{
	return m_szDetailTexture;
}
void CvArtInfoTerrain::setDetailTexture(const char* szTmp)
{
	m_szDetailTexture = szTmp;
}
int CvArtInfoTerrain::getLayerOrder() const
{
	return m_iLayerOrder;
}
TerrainGroupTypes CvArtInfoTerrain::getTerrainGroup() const
{
	return m_eTerrainGroup;
}
CvTextureBlendSlotList &CvArtInfoTerrain::getBlendList(int blendMask)
{
	FAssert(blendMask>0 && blendMask<16);
	return *m_pTextureSlots[blendMask];
}
void BuildSlotList( CvTextureBlendSlotList &list, CvString &numlist)
{
	//convert string to
	char seps[]   = " ,\t\n";
	char *token;
	const char *numstring = numlist;
	token = strtok( const_cast<char *>(numstring), seps);
	while( token != NULL )
	{
		int slot = atoi(token);
		token = strtok( NULL, seps);
		int rotation = atoi(token);
		list.push_back(std::make_pair( slot, rotation));
		token = strtok( NULL, seps);
	}
}
bool CvArtInfoTerrain::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvArtInfoAsset::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(szTextVal, "Grid");
	setGridTexture(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "Detail");
	setDetailTexture(szTextVal);
	pXML->GetChildXmlValByName(&m_iLayerOrder, "LayerOrder");

	pXML->GetChildXmlValByName(szTextVal, "TerrainGroup");
	if(szTextVal.CompareNoCase("TERRAIN_GROUP_OCEAN") == 0)
	{
		m_eTerrainGroup = TERRAIN_GROUP_OCEAN;
	}
	else if(szTextVal.CompareNoCase("TERRAIN_GROUP_COAST") == 0)
	{
		m_eTerrainGroup = TERRAIN_GROUP_COAST;
	}
	else if(szTextVal.CompareNoCase("TERRAIN_GROUP_LAND") == 0)
	{
		m_eTerrainGroup = TERRAIN_GROUP_LAND;
	}
	else
	{
		FAssertMsg(false, "[Jason] Unknown TerrainGroupType.");
	}

	// Parse texture slots for blend tile lists
	char xmlName[] = "TextureBlend00";
	for(int i =1; i<m_numTextureBlends;i++ )
	{
		sprintf(xmlName+(strlen(xmlName)-2),"%02d",i);
		pXML->GetChildXmlValByName(szTextVal, xmlName);
		BuildSlotList(*m_pTextureSlots[i], szTextVal);
	}
	return CvArtInfoAsset::read(pXML);
}
//////////////////////////////////////////////////////////////////////////
// CvArtInfoFeature
//////////////////////////////////////////////////////////////////////////
CvArtInfoFeature::CvArtInfoFeature() :
m_bAnimated(false),
m_eTileArtType(TILE_ART_TYPE_NONE)
{
}
CvArtInfoFeature::~CvArtInfoFeature()
{
}
bool CvArtInfoFeature::isAnimated() const
{
	return m_bAnimated;
}
TileArtTypes CvArtInfoFeature::getTileArtType() const
{
	return m_eTileArtType;
}
bool CvArtInfoFeature::read(CvXMLLoadUtility* pXML)
{
	if (!CvArtInfoScalableAsset::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(&m_bAnimated, "bAnimated");
	CvString szTemp;
	pXML->GetChildXmlValByName(szTemp, "TileArtType");
	if(szTemp.CompareNoCase("TILE_ART_TYPE_NONE") == 0)
		m_eTileArtType = TILE_ART_TYPE_NONE;
	else if(szTemp.CompareNoCase("TILE_ART_TYPE_TREES") == 0)
		m_eTileArtType = TILE_ART_TYPE_TREES;
	else if(szTemp.CompareNoCase("TILE_ART_TYPE_HALF_TILING") == 0)
		m_eTileArtType = TILE_ART_TYPE_HALF_TILING;
	else
	{
		FAssertMsg(false, "[Jason] Unknown TileArtType.");
	}

	//feature varieties
	if(gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"FeatureVariety"))
	{
		do
		{
			m_aFeatureVarieties.push_back(FeatureVariety());
			FeatureVariety &featureVariety = m_aFeatureVarieties.back();
			//feature art pieces
			if(gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"FeatureArtPieces"))
			{
				if(gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"FeatureArtPiece"))
				{
					do
					{
						//connection mask
						pXML->GetChildXmlValByName(szTemp, "Connections");
						int connectionMask = getConnectionMaskFromString(szTemp);
						//model files
						if(gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"ModelFile"))
						{
							do
							{
								pXML->GetXmlVal(szTemp);
								FeatureArtPiece &featureArtPiece = featureVariety.createFeatureArtPieceFromConnectionMask(connectionMask);
								featureArtPiece.m_aArtModels.push_back(FeatureArtModel(szTemp));
							} while(gDLL->getXMLIFace()->LocateNextSiblingNodeByTagName(pXML->GetXML(), "ModelFile"));
							gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
						}
					} while(gDLL->getXMLIFace()->LocateNextSiblingNodeByTagName(pXML->GetXML(), "FeatureArtPiece"));
					gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
				}
				gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
			}
			//variety button
			pXML->GetChildXmlValByName(&featureVariety.m_iModelCopies, "iModelCopies", 1);
			pXML->GetChildXmlValByName(featureVariety.m_szVarietyButton, "VarietyButton");
		} while(gDLL->getXMLIFace()->LocateNextSiblingNodeByTagName(pXML->GetXML(), "FeatureVariety"));
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	return true;
}
const CvArtInfoFeature::FeatureVariety &CvArtInfoFeature::getVariety(int index) const
{
	FAssertMsg((index >= 0) && (index < (int) m_aFeatureVarieties.size()), "[Jason] Feature Variety index out of range.");
	return m_aFeatureVarieties[index];
}
int CvArtInfoFeature::getNumVarieties() const
{
	return m_aFeatureVarieties.size();
}
int CvArtInfoFeature::getConnectionMaskFromString(const CvString &connectionString)
{
	if(connectionString.IsEmpty())
		return 0;
	else
	{
		std::vector<CvString> tokens;
		connectionString.getTokens(" \t\n", tokens);
		int connectionMask = 0;
		for(int i=0;i<(int)tokens.size();i++)
		{
			// found a token, parse it.
			CvString &token = tokens[i];
			if(token.CompareNoCase("NW") == 0)
				connectionMask |= DIRECTION_NORTHWEST_MASK;
			else if(token.CompareNoCase("N") == 0)
				connectionMask |= DIRECTION_NORTH_MASK;
			else if(token.CompareNoCase("NE") == 0)
				connectionMask |= DIRECTION_NORTHEAST_MASK;
			else if(token.CompareNoCase("E") == 0)
				connectionMask |= DIRECTION_EAST_MASK;
			else if(token.CompareNoCase("SE") == 0)
				connectionMask |= DIRECTION_SOUTHEAST_MASK;
			else if(token.CompareNoCase("S") == 0)
				connectionMask |= DIRECTION_SOUTH_MASK;
			else if(token.CompareNoCase("SW") == 0)
				connectionMask |= DIRECTION_SOUTHWEST_MASK;
			else if(token.CompareNoCase("W") == 0)
				connectionMask |= DIRECTION_WEST_MASK;
			else
			{
				FAssertMsg(false, "[Jason] Invalid connection direction.");
			}
		}
		FAssertMsg(connectionMask > 0, "[Jason] Did not find feature connection mask.");
		return connectionMask;
	}
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvEmphasizeInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvEmphasizeInfo::CvEmphasizeInfo() :
m_bAvoidGrowth(false),
m_aiYieldModifiers(NULL)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvEmphasizeInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvEmphasizeInfo::~CvEmphasizeInfo()
{
	SAFE_DELETE_ARRAY(m_aiYieldModifiers);
}
bool CvEmphasizeInfo::isAvoidGrowth() const
{
	return m_bAvoidGrowth;
}
// Arrays
int CvEmphasizeInfo::getYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiYieldModifiers ? m_aiYieldModifiers[i] : -1;
}
//
// read from XML
//
bool CvEmphasizeInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(&m_bAvoidGrowth, "bAvoidGrowth");
	pXML->SetVariableListTagPair(&m_aiYieldModifiers, "YieldModifiers", NUM_YIELD_TYPES, 0);
	return true;
}
//------------------------------------------------------------------------------------------------------
//
//  CvCultureLevelInfo
//
CvCultureLevelInfo::CvCultureLevelInfo() :
m_iThreshold(0)
{
}
CvCultureLevelInfo::~CvCultureLevelInfo()
{
}
int CvCultureLevelInfo::getThreshold() const
{
	return m_iThreshold;
}
bool CvCultureLevelInfo::read(CvXMLLoadUtility* pXml)
{
	if (!CvInfoBase::read(pXml))
	{
		return false;
	}
	pXml->GetChildXmlValByName(&m_iThreshold, "iThreshold");
	return true;
}
//------------------------------------------------------------------------------------------------------
//
//  CvEraInfo
//
CvEraInfo::CvEraInfo() :
m_iGameTurn(0),
m_iStartingUnitMultiplier(0),
m_iStartingDefenseUnits(0),
m_iStartingWorkerUnits(0),
m_iStartingExploreUnits(0),
m_iAdvancedStartPoints(0),
m_iStartingGold(0),
m_iFreePopulation(0),
m_iStartPercent(0),
m_iGrowthPercent(0),
m_iTrainPercent(0),
m_iConstructPercent(0),
m_iFatherPercent(0),
m_iGreatGeneralPercent(0),
m_iEventChancePerTurn(0),
m_iSoundtrackSpace(0),
m_iNumSoundtracks(0),
m_eCivEffect(NO_CIV_EFFECT),
m_bRevolution(false),
m_bNoGoodies(false),
m_bFirstSoundtrackFirst(false),
m_paiCitySoundscapeSciptIds(NULL),
m_paiSoundtracks(NULL)
{
}
CvEraInfo::~CvEraInfo()
{
	SAFE_DELETE_ARRAY(m_paiCitySoundscapeSciptIds);
	SAFE_DELETE_ARRAY(m_paiSoundtracks);
}
int CvEraInfo::getGameTurn() const
{
	return m_iGameTurn;
}
int CvEraInfo::getStartingUnitMultiplier() const
{
	return m_iStartingUnitMultiplier;
}
int CvEraInfo::getStartingDefenseUnits() const
{
	return m_iStartingDefenseUnits;
}
int CvEraInfo::getStartingWorkerUnits() const
{
	return m_iStartingWorkerUnits;
}
int CvEraInfo::getStartingExploreUnits() const
{
	return m_iStartingExploreUnits;
}
int CvEraInfo::getAdvancedStartPoints() const
{
	return m_iAdvancedStartPoints;
}
int CvEraInfo::getStartingGold() const
{
	return m_iStartingGold;
}
int CvEraInfo::getFreePopulation() const
{
	return m_iFreePopulation;
}
int CvEraInfo::getStartPercent() const
{
	return m_iStartPercent;
}
int CvEraInfo::getGrowthPercent() const
{
	return m_iGrowthPercent;
}
int CvEraInfo::getTrainPercent() const
{
	return m_iTrainPercent;
}
int CvEraInfo::getConstructPercent() const
{
	return m_iConstructPercent;
}
int CvEraInfo::getFatherPercent() const
{
	return m_iFatherPercent;
}
int CvEraInfo::getGreatGeneralPercent() const
{
	return m_iGreatGeneralPercent;
}
int CvEraInfo::getEventChancePerTurn() const
{
	return m_iEventChancePerTurn;
}
int CvEraInfo::getSoundtrackSpace() const
{
	return m_iSoundtrackSpace;
}
bool CvEraInfo::isFirstSoundtrackFirst() const
{
	return m_bFirstSoundtrackFirst;
}
int CvEraInfo::getNumSoundtracks() const
{
	return m_iNumSoundtracks;
}
const char* CvEraInfo::getAudioUnitVictoryScript() const
{
	return m_szAudioUnitVictoryScript;
}
const char* CvEraInfo::getAudioUnitDefeatScript() const
{
	return m_szAudioUnitDefeatScript;
}
bool CvEraInfo::isRevolution() const
{
	return m_bRevolution;
}
bool CvEraInfo::isNoGoodies() const
{
	return m_bNoGoodies;
}
// Arrays
int CvEraInfo::getSoundtracks(int i) const
{
	FAssertMsg(i < getNumSoundtracks(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiSoundtracks ? m_paiSoundtracks[i] : -1;
}
int CvEraInfo::getCitySoundscapeSciptId(int i) const
{
//	FAssertMsg(i < ?, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiCitySoundscapeSciptIds ? m_paiCitySoundscapeSciptIds[i] : -1;
}
bool CvEraInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetEnum(getType(), m_eCivEffect, "eCivEffect", false);
	pXML->GetChildXmlValByName(&m_bRevolution, "bRevolution");
	pXML->GetChildXmlValByName(&m_bNoGoodies, "bNoGoodies");
	pXML->GetChildXmlValByName(&m_iGameTurn, "iGameTurn");
	pXML->GetChildXmlValByName(&m_iStartingUnitMultiplier, "iStartingUnitMultiplier");
	pXML->GetChildXmlValByName(&m_iStartingDefenseUnits, "iStartingDefenseUnits");
	pXML->GetChildXmlValByName(&m_iStartingWorkerUnits, "iStartingWorkerUnits");
	pXML->GetChildXmlValByName(&m_iStartingExploreUnits, "iStartingExploreUnits");
	pXML->GetChildXmlValByName(&m_iAdvancedStartPoints, "iAdvancedStartPoints");
	pXML->GetChildXmlValByName(&m_iStartingGold, "iStartingGold");
	pXML->GetChildXmlValByName(&m_iFreePopulation, "iFreePopulation");
	pXML->GetChildXmlValByName(&m_iStartPercent, "iStartPercent");
	pXML->GetChildXmlValByName(&m_iGrowthPercent, "iGrowthPercent");
	pXML->GetChildXmlValByName(&m_iTrainPercent, "iTrainPercent");
	pXML->GetChildXmlValByName(&m_iConstructPercent, "iConstructPercent");
	pXML->GetChildXmlValByName(&m_iFatherPercent, "iFatherPercent");
	pXML->GetChildXmlValByName(&m_iGreatGeneralPercent, "iGreatGeneralPercent");
	pXML->GetChildXmlValByName(&m_iEventChancePerTurn, "iEventChancePerTurn");
	pXML->GetChildXmlValByName(&m_iSoundtrackSpace, "iSoundtrackSpace");
	pXML->GetChildXmlValByName(&m_bFirstSoundtrackFirst, "bFirstSoundtrackFirst");
	pXML->GetChildXmlValByName(m_szAudioUnitVictoryScript, "AudioUnitVictoryScript");
	pXML->GetChildXmlValByName(m_szAudioUnitDefeatScript, "AudioUnitDefeatScript");
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "EraInfoSoundtracks"))
	{
		CvString* pszSoundTrackNames = NULL;
		pXML->SetStringList(&pszSoundTrackNames, &m_iNumSoundtracks);
		if (m_iNumSoundtracks > 0)
		{
			m_paiSoundtracks = new int[m_iNumSoundtracks];
			for (int j = 0; j < m_iNumSoundtracks; ++j)
			{
				m_paiSoundtracks[j] = ((!gDLL->getAudioDisabled()) ? gDLL->getAudioTagIndex(pszSoundTrackNames[j], AUDIOTAG_2DSCRIPT) : -1);
			}
		}
		else
		{
			m_paiSoundtracks = NULL;
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		SAFE_DELETE_ARRAY(pszSoundTrackNames);
	}
	pXML->SetVariableListTagPairForAudioScripts(&m_paiCitySoundscapeSciptIds, "CitySoundscapes", GC.getNumCitySizeTypes());
	return true;
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvColorInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvColorInfo::CvColorInfo()
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvColorInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvColorInfo::~CvColorInfo()
{
}
const NiColorA& CvColorInfo::getColor() const
{
	return m_Color;
}
bool CvColorInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	float afColorVals[4];	// array to hold the 4 color values, red, green, blue, and alpha

	pXML->GetChildXmlValByName(&afColorVals[0], "fRed");
	pXML->GetChildXmlValByName(&afColorVals[1], "fGreen");
	pXML->GetChildXmlValByName(&afColorVals[2], "fBlue");
	pXML->GetChildXmlValByName(&afColorVals[3], "fAlpha");
	m_Color = NiColorA(afColorVals[0], afColorVals[1], afColorVals[2], afColorVals[3]);
	return true;
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvPlayerColorInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvPlayerColorInfo::CvPlayerColorInfo() :
m_iColorTypePrimary(NO_COLOR),
m_iColorTypeSecondary(NO_COLOR),
m_iTextColorType(NO_COLOR)
{
}
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvPlayerColorInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvPlayerColorInfo::~CvPlayerColorInfo()
{
}
int CvPlayerColorInfo::getColorTypePrimary() const
{
	return m_iColorTypePrimary;
}
int CvPlayerColorInfo::getColorTypeSecondary() const
{
	return m_iColorTypeSecondary;
}
int CvPlayerColorInfo::getTextColorType() const
{
	return m_iTextColorType;
}
bool CvPlayerColorInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(szTextVal, "ColorTypePrimary");
	m_iColorTypePrimary = pXML->FindInInfoClass( szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "ColorTypeSecondary");
	m_iColorTypeSecondary = pXML->FindInInfoClass( szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "TextColorType");
	m_iTextColorType = pXML->FindInInfoClass( szTextVal);
	return true;
}
//------------------------------------------------------------------------------------------------------
//
//	CvLandscapeInfo
//
//
CvLandscapeInfo::CvLandscapeInfo() :
m_iPlotVertsWide(0),
m_iPlotVertsHigh(0),
m_iPlotsPerCellX(0),
m_iPlotsPerCellY(0),
m_iCellVertsWide(0),
m_iCellVertsHigh(0),
m_iWaterHeight(0),
m_fTextureScaleX(0.0f),
m_fTextureScaleY(0.0f),
m_fZScale(0.0f),
m_fPeakScale(0.0f),
m_fHillScale(0.0f)
{
}
int CvLandscapeInfo::getPlotVertsWide() const
{
	return m_iPlotVertsWide;
}
int CvLandscapeInfo::getPlotVertsHigh() const
{
	return m_iPlotVertsHigh;
}
int CvLandscapeInfo::getPlotsPerCellX() const
{
	return m_iPlotsPerCellX;
}
int CvLandscapeInfo::getPlotsPerCellY() const
{
	return m_iPlotsPerCellY;
}
int CvLandscapeInfo::getCellVertsWide() const
{
	return m_iCellVertsWide;
}
int CvLandscapeInfo::getCellVertsHigh() const
{
	return m_iCellVertsHigh;
}
int CvLandscapeInfo::getWaterHeight() const
{
	return m_iWaterHeight;
}
float CvLandscapeInfo::getTextureScaleX() const
{
	return m_fTextureScaleX;
}
float CvLandscapeInfo::getTextureScaleY() const
{
	return m_fTextureScaleY;
}
float CvLandscapeInfo::getZScale() const
{
	return m_fZScale;
}
float CvLandscapeInfo::getPeakScale() const
{
	return 	m_fPeakScale;
}
float CvLandscapeInfo::getHillScale() const
{
	return 	m_fHillScale;
}
const char* CvLandscapeInfo::getEnvironmentTexture()
{
	return m_szEnvironmentTexture;
}
//
// read from xml
//
bool CvLandscapeInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(&m_iWaterHeight, "iWaterHeight");
	pXML->GetChildXmlValByName(m_szEnvironmentTexture, "EnvironmentTexture");
	pXML->GetChildXmlValByName(&m_fTextureScaleX, "fTextureScaleX");
	pXML->GetChildXmlValByName(&m_fTextureScaleY, "fTextureScaleY");
	pXML->GetChildXmlValByName(&m_iPlotVertsWide, "iPlotVertsWide");
	pXML->GetChildXmlValByName(&m_iPlotVertsHigh, "iPlotVertsHigh");
	pXML->GetChildXmlValByName(&m_iPlotsPerCellX, "iPlotsPerCellX");
	pXML->GetChildXmlValByName(&m_iPlotsPerCellY, "iPlotsPerCellY");
	m_iCellVertsWide = m_iPlotsPerCellX * (m_iPlotVertsWide - 1) + 1;
	m_iCellVertsHigh = m_iPlotsPerCellY * (m_iPlotVertsHigh - 1) + 1;
	pXML->GetChildXmlValByName(&m_fZScale, "fZScale");
	pXML->GetChildXmlValByName(&m_fPeakScale, "fPeakScale");
	pXML->GetChildXmlValByName(&m_fHillScale, "fHillScale");

	return true;
}
//////////////////////////////////////////////////////////////////////////
// CvGameText
//////////////////////////////////////////////////////////////////////////
// static
int CvGameText::STATIC_iNumLanguages = 0;
int* CvGameText::STATIC_pLanguageArray = NULL;
bool CvGameText::STATIC_bChangeLanguage = false;

/*
 * Vanilla has an issue with selecting language because it saves language as an int in CivilizationIV.ini.
 * The core of the issue is that this ini file is shared by all mods, meaning you don't select English, you select language 0.
 * Vanilla provides IDs for 5 languages and all mods use those, hiding the issue. As long as all mods only use languages
 *   from vanilla, this won't be a problem. Selecting language 0 is English in all mods.
 * However this is only true for languages 0-4. If two mods adds language 5, but disagree on which is 5, a huge mess will occur.
 *
 * A really big problem is selecting language 5 and then switch to a mod with only 5 languagages as this will remove all text.
 * Not just write errors, more like simply removing all text entirely like writing text delivers nothing.
 * This is clear right away as the main menu is empty. No text means nothing to click on, not even change language.
 *
 * Another issue is that the xml file stores a number of languages of each entry. The vanilla read code reads the Xth entry.
 * This means language 0 will read the first, even if that isn't English. It also means it requires a line for each language.
 * Adding a text string is an issue if you have to add it for each language, particularly if the number of languages is increasing.
 *
 * This heavily modded part of CvGameText aims to allow adding languages much easier while avoiding issues when switching mods.
 *
 * The core idea is splitting the language int into multiple ints:
 *  - Language ID: the value written to CivilizationIV.ini
 *  - Language menu ID: the index in the language selection menu in the game options
 *  - Make string reading depend on tag name rather than index
 *
 * Language ID:
 *  Since this setting is shared between all mods, the goal is to make sure all mods agree on which language each ID represents.
 *  CvGameText::getLanguageName() converts an index into a language.
 *
 * Language menu ID:
 *  New concept. It's set by the file xml\Interface\Languages.xml.
 *  It sets which languages should appear in the menu and in which order.
 *  This means we aren't forced to include all the langauges we have added a Language ID for and the menu can be mod specific.
 *
 * TXT_KEY loading:
 *  This no longer depends on index. Instead the new requirements are:
 *  Tag needs to go first. The language tag used is CvGameText::getLanguageName(current Language ID)
 *  This means it's perfectly valid to only add English, English and German or whatever translations you actually have.
 *  Vanilla requires a tag for each language, even if no translator is available.
 *
 *
 * English is the default language
 *  This means if a TXT_KEY doesn't have the requested language, the game will use English instead for the TXT_KEY in question.
 *  This means partial translations will be a mix of English and that language rather than a crash when selecting the language.
 *  It also mean if the player switch mod and the other mod doesn't have the language in question, English will be used.
 *
 * Adding more languages to the list
 *  If a mod needs to add a language not present, just do so, but do make other mods aware of this.
 *  The problem is if two mods adds two different languages at the same ID, which is why new IDs should be synced.
 *  If one mod adds a language and another isn't updated for that language, it will use the defaults to English fallback.
 *  Since out of bounds use English, this isn't an issue if one mod adds a language, only if two mods adds in parallel.
 *  In other words if a mod use this system and it dies, it will still work with other mods using this system without being updated.
 *  While we hope no mods die, it's an important aspect to code into the system from the start.
 *
 * If a player use language X in mod A, switches to mod B, which doesn't have X, then English is used.
 *  However if the player doesn't change the language and switch back to mod A, language X is still remembered and will be used.
 *  This is because the English fallback will not actually update the ini file.
 *  As a result, people can select their language of choice, use it if available and use English if not. No mod specific setup.
 */

void CvGameText::setChangeLanguage()
{
	STATIC_bChangeLanguage = true;
}

int CvGameText::getNumLanguages() const
{
	// The exe will use this function for two purposes:
	// 1: number of lines in the language menu
	// 2: setting a new current Language ID will be ignored if the value is >= the return value here

	// This is a problem for our modded system if one or more languages are skipped in Languages.xml.
	// Say we have English and Tag. That's 0 and 25, but the number of languages is 2, meaning requests to change to Tag are ignored.

	// The solution to this is when changing the language, the bool CHANGE_LANGUAGE is set.
	// This means we know if the exe calls for the menu length or for verifying Language ID.
	if (STATIC_bChangeLanguage)
	{
		// New language being tested.
		// All languages will be lower than MAX_INT, meaning returning this value disables the unwanted test.
		STATIC_bChangeLanguage = false;
		return MAX_INT;
	}

	// The exe asks for the number of languages, which should be put in the menu (from Languages.xml)
	return STATIC_iNumLanguages;
}

// Same as the non-static function, except this one doesn't need an instance to work.
// Also being modded, we don't have to consider calls from the exe.
int CvGameText::getNumLanguagesStatic()
{
	return STATIC_iNumLanguages;
}

void CvGameText::setNumLanguages(int iNum)
{
	// Note: vanilla exe exposed function. It doesn't look like it's used at all.
	STATIC_iNumLanguages = iNum;
}

// Convert Language Menu ID to Language ID
int CvGameText::getLanguageAtIndex(int iIndex)
{
	if (iIndex >= 0 && iIndex < STATIC_iNumLanguages)
	{
		return STATIC_pLanguageArray[iIndex];
	}
	return 0;
}

char const* CvGameText::getLanguageName(int iLanguageID)
{
	// The game will store the chosen language as an int, not a string.
	// This has a history of messing up switching between mods.
	// Hardcoding indexes here should solve that problem if all mods use this function/DLL.
	// 0-4 is hardcoded by vanilla.
	// 0-10 are copied from Caveman2Cosmos
	// 11-24 attempts to complete the list of languages using codepage 1252 (the translations not burdened by encoding issues)
	// 25 Tag is displaying the Tag rather than text. Useful for figuring out which TXT_KEY to look up if you want to edit a string

	// Not a performance critical function. Adding to the list will not really affect either memory or CPU usage.

	// IDs serve no purpose other than identification. This means there is no functional difference between say language 1 and 20.
	// All it requires is that English is 0 and the IDs goes from 0 to the end without skipping any numbers.

	// Changing existing IDs is not a good idea because big problems will occur if two mods disagree on which language an ID refers to.
	// Even if all mods update at the same time, there is still the risk that a player updates one mod and not the other.
	// There is also the fact that even if the player updates everything, CivilizationIV.ini will still use the old value.
	// Because of this, expand the list at the end and make sure all mods expand the same way.
	// If one mod adds a language and another isn't updated, that mod will use English instead because default is English.
	// This way if one mod updates the list, other mods will not have to update until they add to the list anyway.

	switch (iLanguageID)
	{
	case 0: return "English";
	case 1: return "French";
	case 2: return "German";
	case 3: return "Italian";
	case 4: return "Spanish";
	case 5:	return "Finnish";
	case 6:	return "Hungarian";
	case 7:	return "Polish";
	case 8: return "Russian";
	case 9: return "Chinese";
	case 10: return "Japanese";

	case 11: return "Afrikaans";
	case 12: return "Albanian";
	case 13: return "Basque";
	case 14: return "Catalan";
	case 15: return "Danish";
	case 16: return "Dutch";
	case 17: return "Faroese";
	case 18: return "Galician";
	case 19: return "Icelandic";
	case 20: return "PortugueseBR";
	case 21: return "PortuguesePT";
	case 22: return "Norwegian";
	case 23: return "Scottish";
	case 24: return "Swedish";

	case 25: return "Tag";

	case 26: return "Greek";
	case 27: return "Turkish";
	case 28: return "Hebrew";
	case 29: return "Arabic";
	case 30: return "Estonian";
	case 31: return "Latvian";
	case 32: return "Lithuanian";

	default:
		FAssertMsg(false, CvString::format("Language out of bound: %i", iLanguageID));
	}
	return "English";
}

// Returns the codepage for the current language
int CvGameText::getCodePage()
{
	switch (GAMETEXT.getCurrentLanguage())
	{
	case 6:
	case 7:
		return 1250;
	case 8:
		return 1251;
	case 26:
		return 1253;
	case 27:
		return 1254;
	case 28:
		return 1255;
	case 29:
		return 1256;
	case 30:
	case 31:
	case 32:
		return 1257;
	default:
		return 1252;
	}
}

// Get the Language ID from a string
// Used to read strings from xml.
int CvGameText::getLanguageID(const char* szLanguageName)
{
	for (int i = 0;; ++i)
	{
		const char* szIndexLanguage = getLanguageName(i);
		if (strcmp(szIndexLanguage, szLanguageName) == 0)
		{
			return i;
		}
		if (i > 0 && strcmp(szIndexLanguage, "English") == 0)
		{
			// Language index is higher than the table allows, meaning szLanguage isn't in the list
			FAssertMsg(false, CvString::format("Unable to locate language: %s", szLanguageName))
			return -1;
		}
	}

	return -1;
}

// Read xml\Interface\Languages.xml
bool CvGameText::readLanguages(CvXMLLoadUtility* pXML)
{
	CvString szBuffer;
	if (!gDLL->getXMLIFace()->LocateNode(pXML->GetXML(), "Languages"))
	{
		return false;
	}
	STATIC_iNumLanguages = gDLL->getXMLIFace()->NumOfChildrenByTagName(pXML->GetXML(), "Language");
	SAFE_DELETE_ARRAY(STATIC_pLanguageArray);
	STATIC_pLanguageArray = new int[STATIC_iNumLanguages];
	if (!gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "Language"))
	{
		return false;
	}
	for (int i = 0; i < STATIC_iNumLanguages && pXML->GetXmlVal(szBuffer); ++i, gDLL->getXMLIFace()->LocateNextSiblingNodeByTagName(pXML->GetXML(), "Language"))
	{
		STATIC_pLanguageArray[i] = getLanguageID(szBuffer.c_str());
	}
	return STATIC_iNumLanguages > 0;
}

CvGameText::CvGameText() :
	m_szGender("N"),
	m_szPlural("false")
{
}
const wchar* CvGameText::getText() const
{
	return m_szText;
}
void CvGameText::setText(const wchar* szText)
{
	m_szText = szText;
}
bool CvGameText::read(CvXMLLoadUtility* pXML, bool bUTF8, const char *szFileName, char const* szLanguage)
{
	CvString szTextVal;
	CvWString wszTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	gDLL->getXMLIFace()->SetToChild(pXML->GetXML()); // Move down to Child level
	pXML->GetXmlVal(m_szType);		// TAG

	setGender(L"N");
	setPlural(L"false");

	// move to the tag, which contains the language, which we will load.

	bool bLanguageFound = true;

	// First try the user selected language
	/*
	if (!gDLL->getXMLIFace()->LocateFirstSiblingNodeByTagName(pXML->GetXML(), getLanguageName(GAMETEXT.getCurrentLanguage())))
	{
		bLanguageFound = false;
		// language not found. Use Default
		if (getCodePage() == 1252 || !gDLL->getXMLIFace()->LocateFirstSiblingNodeByTagName(pXML->GetXML(), "Default"))
		{
			// No Default found. Use English as fallback
			if (!gDLL->getXMLIFace()->LocateFirstSiblingNodeByTagName(pXML->GetXML(), "English"))
			{
				// English missing!!!
				// This should never happen!
				// Use Tag because we have to find a child to work on.
				FAssertMsg(false, CvString::format("Missing languages in string %s", m_szType).c_str());
				gDLL->getXMLIFace()->LocateFirstSiblingNodeByTagName(pXML->GetXML(), "Tag");
			}
		}
	}
	*/

	if (!gDLL->getXMLIFace()->LocateFirstSiblingNodeByTagName(pXML->GetXML(), szLanguage))
	{
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML()); // Move back up to Parent
		return false;
	}

	if (strcmp(szLanguage, "English") == 0)
	{
		// bLanguageFound is used for non-western languages
		// do not trigger while reading the English fallback string
		bLanguageFound = false;
	}

	if (readString(pXML, wszTextVal, "Text", bUTF8, szFileName, bLanguageFound, getType()))
	{
		// There are child tags. Read all 3 of them.

		// TEXT
		setText(wszTextVal);
		// GENDER
		if (readString(pXML, wszTextVal, "Gender", bUTF8, szFileName, bLanguageFound, getType()))
		{
			setGender(wszTextVal);
		}
		// PLURAL
		if (readString(pXML, wszTextVal, "Plural", bUTF8, szFileName, bLanguageFound, getType()))
		{
			setPlural(wszTextVal);
		}
	}
	else
	{
		// No Text child meaning no gender or plural. Just read the text.
		readString(pXML, wszTextVal, NULL, bUTF8, szFileName, bLanguageFound, getType());
		setText(wszTextVal);
	}

	gDLL->getXMLIFace()->SetToParent(pXML->GetXML()); // Move back up to Parent
	return true;
}

// read a string from an xml tag. Key feature is to convert from UTF-8 to whatever codepage the current language is using.
bool CvGameText::readString(CvXMLLoadUtility* pXML, CvWString &wszTextVal, const char* szTagName, bool bUTF8, const char *szFileName, bool bLanguageFound, const char* szType)
{
	if (!bUTF8)
	{
		// vanilla reading code. Assuming the correct encoding from xml without verification.
		if (szTagName == NULL)
		{
			return pXML->GetXmlVal(wszTextVal);
		}
		else
		{
			return pXML->GetChildXmlValByName(wszTextVal, szTagName);
		}
	}

	// Convert UTF8 to whatever encoding the language in question requires.

	bool bLoaded = false;
	wszTextVal.clear();
	CvString szBuffer;

	if (szTagName == NULL)
	{
		bLoaded = pXML->GetXmlVal(szBuffer);
	}
	else
	{
		bLoaded = pXML->GetChildXmlValByName(szBuffer, szTagName);
	}

	if (!bLoaded)
	{
		return false;
	}

	wszTextVal = convertFromUTF8(szBuffer, !bLanguageFound, szFileName, szType);
	return true;
}

CvWString CvGameText::convertFromUTF8(const CvString szBuffer, bool bFallback, const char *szFileName, const char* szType)
{
	CvWString wszTextVal;

	for (unsigned int i = 0; i < szBuffer.size(); ++i)
	{
		unsigned int iBuffer = szBuffer.c_str()[i] & 0xFF; // GetBits doesn't work if it wants to read all the bits, like 8 bits from a byte

		int iExtraBytes = 0;

		if (HasBit(iBuffer, 7))
		{
			ClrBit(iBuffer, 7);

			for (int j = 6; HasBit(iBuffer, j); --j)
			{
				++iExtraBytes;
				ClrBit(iBuffer, j);
			}
		}

		for (int iByte = 0; iByte < iExtraBytes; ++iByte)
		{
			++i;
			FAssert(i < szBuffer.size());
			const wchar_t cWChar = szBuffer.c_str()[i];
			FAssert(HasBit(cWChar, 7) && !HasBit(cWChar, 6));
			iBuffer <<= 6;
			iBuffer |= GetBits(cWChar, 0, 6);
		}

		unsigned char iChar = '?';

		int iReturnVal = 0;

		switch (getCodePage())
		{
		case 1250: iReturnVal = cp1250_wctomb(0, &iChar, iBuffer, 0); break;
		case 1251: iReturnVal = cp1251_wctomb(0, &iChar, iBuffer, 0); break;
		case 1252: iReturnVal = cp1252_wctomb(0, &iChar, iBuffer, 0); break;
		case 1253: iReturnVal = cp1253_wctomb(0, &iChar, iBuffer, 0); break;
		case 1254: iReturnVal = cp1254_wctomb(0, &iChar, iBuffer, 0); break;
		case 1255: iReturnVal = cp1255_wctomb(0, &iChar, iBuffer, 0); break;
		case 1256: iReturnVal = cp1256_wctomb(0, &iChar, iBuffer, 0); break;
		case 1257: iReturnVal = cp1257_wctomb(0, &iChar, iBuffer, 0); break;
		default: FAssert(false);
		}

		if (iReturnVal != 1)
		{
			if (bFallback && getCodePage() != 1252)
			{
				// Convert to ASCII if possible as 0x7F and below is the same for all code pages
				switch (iBuffer)
				{
				case 0xB4:
					wszTextVal.append(L"'");
					continue;
				case 0xC0:
				case 0xC1:
				case 0xC2:
				case 0xC3:
				case 0xC4:
				case 0xC5:
				case 0xC6:
					wszTextVal.append(L"A");
					continue;
				case 0xC7:
					wszTextVal.append(L"C");
					continue;
				case 0xC8:
				case 0xC9:
				case 0xCA:
				case 0xCB:
					wszTextVal.append(L"E");
					continue;
				case 0xCC:
				case 0xCD:
				case 0xCE:
				case 0xCF:
					wszTextVal.append(L"I");
					continue;

				case 0xD0:
					wszTextVal.append(L"D");
					continue;
				case 0xD1:
					wszTextVal.append(L"N");
					continue;
				case 0xD2:
				case 0xD3:
				case 0xD4:
				case 0xD5:
				case 0xD6:
					wszTextVal.append(L"O");
					continue;
				case 0xD7:
					wszTextVal.append(L"x");
					continue;
				case 0xD8:
					wszTextVal.append(L"O");
					continue;
				case 0xD9:
				case 0xDA:
				case 0xDB:
				case 0xDC:
					wszTextVal.append(L"U");
					continue;
				case 0xDD:
					wszTextVal.append(L"Y");
					continue;
				case 0xDE:
					wszTextVal.append(L"Th");
					continue;
				case 0xDF:
					wszTextVal.append(L"ss");
					continue;

				case 0xE0:
				case 0xE1:
				case 0xE2:
				case 0xE3:
				case 0xE4:
				case 0xE5:
				case 0xE6:
					wszTextVal.append(L"a");
					continue;
				case 0xE7:
					wszTextVal.append(L"c");
					continue;
				case 0xE8:
				case 0xE9:
				case 0xEA:
				case 0xEB:
					wszTextVal.append(L"e");
					continue;
				case 0xEC:
				case 0xED:
				case 0xEE:
				case 0xEF:
					wszTextVal.append(L"i");
					continue;

				case 0xF0:
					wszTextVal.append(L"d");
					continue;
				case 0xF1:
					wszTextVal.append(L"n");
					continue;
				case 0xF2:
				case 0xF3:
				case 0xF4:
				case 0xF5:
				case 0xF6:
					wszTextVal.append(L"o");
					continue;
				case 0xF7:
					wszTextVal.append(L"-");
					continue;
				case 0xF8:
					wszTextVal.append(L"o");
					continue;
				case 0xF9:
				case 0xFA:
				case 0xFB:
				case 0xFC:
					wszTextVal.append(L"u");
					continue;
				case 0xFD:
					wszTextVal.append(L"y");
					continue;
				case 0xFE:
					wszTextVal.append(L"th");
					continue;
				case 0xFF:
					wszTextVal.append(L"y");
					continue;

				}
			}

			{
				CvString szBuffer;
				szBuffer.Convert(wszTextVal);

				char	szMessage[1024];
				sprintf(szMessage, "Error reading file %s\n%s\nCurrent string: %s\nNext character isn't in codepage %d\n", szFileName, szType, szBuffer.c_str(), getCodePage());
				gDLL->MessageBox(szMessage, "Text encoding error");
			}
		}

		wchar_t buffer = iChar;
		wszTextVal.append(&buffer, 1);
	}
	return wszTextVal;
}

//////////////////////////////////////////////////////////////////////////
//
//	CvDiplomacyTextInfo
//
//
CvDiplomacyTextInfo::CvDiplomacyTextInfo() :
m_iNumResponses(0),
m_pResponses(NULL)
{
}
// note - Response member vars allocated by CvXmlLoadUtility
void CvDiplomacyTextInfo::init(int iNum)
{
	uninit();
	m_pResponses = new Response[iNum];
	m_iNumResponses=iNum;
}
void CvDiplomacyTextInfo::uninit()
{
	SAFE_DELETE_ARRAY(m_pResponses);
}
int CvDiplomacyTextInfo::getNumResponses() const
{
	return m_iNumResponses;
}
bool CvDiplomacyTextInfo::getCivilizationTypes(int i, int j) const
{
	FAssertMsg(i < getNumResponses(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < GC.getNumCivilizationInfos(), "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return m_pResponses[i].m_abCivilizationTypes[j];
}
bool CvDiplomacyTextInfo::getLeaderHeadTypes(int i, int j) const
{
	FAssertMsg(i < getNumResponses(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < GC.getNumLeaderHeadInfos(), "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return m_pResponses[i].m_abLeaderHeadTypes[j];
}
bool CvDiplomacyTextInfo::getAttitudeTypes(int i, int j) const
{
	FAssertMsg(i < getNumResponses(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < NUM_ATTITUDE_TYPES, "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return m_pResponses[i].m_abAttitudeTypes[j];
}
bool CvDiplomacyTextInfo::getDiplomacyPowerTypes(int i, int j) const
{
	FAssertMsg(i < getNumResponses(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < NUM_DIPLOMACYPOWER_TYPES, "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return m_pResponses[i].m_abDiplomacyPowerTypes[j];
}
int CvDiplomacyTextInfo::getNumDiplomacyText(int i) const
{
	FAssertMsg(i < getNumResponses(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pResponses[i].m_iNumDiplomacyText;
}
const char* CvDiplomacyTextInfo::getDiplomacyText(int i, int j) const
{
	FAssertMsg(i < getNumResponses(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < getNumDiplomacyText(i), "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return m_pResponses[i].m_paszDiplomacyText[j];
}
void CvDiplomacyTextInfo::Response::read(FDataStreamBase* stream)
{
	stream->Read(&m_iNumDiplomacyText);
	SAFE_DELETE_ARRAY(m_abCivilizationTypes);
	m_abCivilizationTypes = new bool[GC.getNumCivilizationInfos()];
	stream->Read(GC.getNumCivilizationInfos(), m_abCivilizationTypes);
	SAFE_DELETE_ARRAY(m_abLeaderHeadTypes);
	m_abLeaderHeadTypes = new bool[GC.getNumLeaderHeadInfos()];
	stream->Read(GC.getNumLeaderHeadInfos(), m_abLeaderHeadTypes);
	SAFE_DELETE_ARRAY(m_abAttitudeTypes);
	m_abAttitudeTypes = new bool[NUM_ATTITUDE_TYPES];
	stream->Read(NUM_ATTITUDE_TYPES, m_abAttitudeTypes);
	SAFE_DELETE_ARRAY(m_abDiplomacyPowerTypes);
	m_abDiplomacyPowerTypes = new bool[NUM_DIPLOMACYPOWER_TYPES];
	stream->Read(NUM_DIPLOMACYPOWER_TYPES, m_abDiplomacyPowerTypes);
	SAFE_DELETE_ARRAY(m_paszDiplomacyText);
	m_paszDiplomacyText = new CvString[m_iNumDiplomacyText];
	stream->ReadString(m_iNumDiplomacyText, m_paszDiplomacyText);
}
void CvDiplomacyTextInfo::Response::write(FDataStreamBase* stream)
{
	stream->Write(m_iNumDiplomacyText);
	stream->Write(GC.getNumCivilizationInfos(), m_abCivilizationTypes);
	stream->Write(GC.getNumLeaderHeadInfos(), m_abLeaderHeadTypes);
	stream->Write(NUM_ATTITUDE_TYPES, m_abAttitudeTypes);
	stream->Write(NUM_DIPLOMACYPOWER_TYPES, m_abDiplomacyPowerTypes);
	stream->WriteString(m_iNumDiplomacyText, m_paszDiplomacyText);
}
void CvDiplomacyTextInfo::read(FDataStreamBase* stream)
{
	CvInfoBase::read(stream);
	uint uiFlag=0;
	stream->Read(&uiFlag);		// flag for expansion
	stream->Read(&m_iNumResponses);
	// Arrays
	init(m_iNumResponses);
	for (uint uiIndex = 0; (int) uiIndex < m_iNumResponses; uiIndex++)
	{
		m_pResponses[uiIndex].read(stream);
	}
}
void CvDiplomacyTextInfo::write(FDataStreamBase* stream)
{
	CvInfoBase::write(stream);
	uint uiFlag=0;
	stream->Write(uiFlag);		// flag for expansion
	stream->Write(m_iNumResponses);
	// Arrays
	for (uint uiIndex = 0; (int) uiIndex < m_iNumResponses; uiIndex++)
	{
		m_pResponses[uiIndex].write(stream);
	}
}
bool CvDiplomacyTextInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	int j;
	pXML->GetChildXmlValByName(szTextVal, "Type");
	if ( gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"Responses") )
	{
		int iIndexVal = gDLL->getXMLIFace()->NumOfChildrenByTagName(pXML->GetXML(), "Response");
		init(iIndexVal);
		for (j = 0; j < iIndexVal; j++)
		{
			if (j == 0)
			{
				gDLL->getXMLIFace()->SetToChild(pXML->GetXML());
			}
			// Civilizations
			pXML->SetVariableListTagPair(&m_pResponses[j].m_abCivilizationTypes, "Civilizations", GC.getNumCivilizationInfos(), false);
			// Leaders
			pXML->SetVariableListTagPair(&m_pResponses[j].m_abLeaderHeadTypes, "Leaders", GC.getNumLeaderHeadInfos(), false);
			// AttitudeTypes
			pXML->SetVariableListTagPair(&m_pResponses[j].m_abAttitudeTypes, "Attitudes", NUM_ATTITUDE_TYPES, false);
			// PowerTypes
			pXML->SetVariableListTagPair(&m_pResponses[j].m_abDiplomacyPowerTypes, "DiplomacyPowers", NUM_DIPLOMACYPOWER_TYPES, false);
			// DiplomacyText
			if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"DiplomacyText"))
			{
				pXML->SetStringList(&m_pResponses[j].m_paszDiplomacyText, &m_pResponses[j].m_iNumDiplomacyText);
				gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
			}
			if (!gDLL->getXMLIFace()->NextSibling(pXML->GetXML()))
			{
				break;
			}
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	return true;
}

//////////////////////////////////////////////////////////////////////////
//
//	CvEffectInfo			Misc\CIV4EffectInfos.xml
//
//
CvEffectInfo::CvEffectInfo() :
m_fUpdateRate(0.0f),
m_bProjectile(false),
m_bSticky(false),
m_fProjectileSpeed(0.0f),
m_fProjectileArc(0.0f)
{
}
CvEffectInfo::~CvEffectInfo()
{
}
bool CvEffectInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	CvScalableInfo::read(pXML);
	pXML->GetChildXmlValByName(szTextVal, "Path");
	setPath(szTextVal);
	pXML->GetChildXmlValByName(&m_fUpdateRate, "fUpdateRate" );
	int iTemporary;
	pXML->GetChildXmlValByName(&iTemporary, "bIsProjectile" );
	m_bProjectile = iTemporary != 0;
	pXML->GetChildXmlValByName(&m_fProjectileSpeed, "fSpeed" );
	pXML->GetChildXmlValByName(&m_fProjectileArc, "fArcValue" );
	pXML->GetChildXmlValByName(&m_bSticky, "bSticky", false );
	return true;
}

//////////////////////////////////////////////////////////////////////////
//
//	CvAttachableInfo			Misc\CIV4AttachableInfos.xml
//
//
CvAttachableInfo::CvAttachableInfo() :
m_fUpdateRate(0.0f)
{
}
CvAttachableInfo::~CvAttachableInfo()
{
}
bool CvAttachableInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	CvScalableInfo::read(pXML);
	pXML->GetChildXmlValByName(szTextVal, "Path");
	setPath(szTextVal);
	return true;
}

//////////////////////////////////////////////////////////////////////////
//
//	CvGameOptionInfo
//	Game options and their default values
//
//
CvGameOptionInfo::CvGameOptionInfo() :
m_bDefault(false),
m_bVisible(true)
{
}
CvGameOptionInfo::~CvGameOptionInfo()
{
}
bool CvGameOptionInfo::getDefault() const
{
	return m_bDefault;
}
bool CvGameOptionInfo::getVisible() const
{
	return m_bVisible;
}
bool CvGameOptionInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(&m_bDefault, "bDefault");
	pXML->GetChildXmlValByName(&m_bVisible, "bVisible");
	return true;
}
//////////////////////////////////////////////////////////////////////////
//
//	CvMPOptionInfo
//	Multiplayer options and their default values
//
//
CvMPOptionInfo::CvMPOptionInfo() :
m_bDefault(false)
{
}
CvMPOptionInfo::~CvMPOptionInfo()
{
}
bool CvMPOptionInfo::getDefault() const
{
	return m_bDefault;
}
bool CvMPOptionInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(&m_bDefault, "bDefault");
	return true;
}
//////////////////////////////////////////////////////////////////////////
//
//	CvForceControlInfo
//	Forced Controls and their default values
//
//
CvForceControlInfo::CvForceControlInfo() :
m_bDefault(false)
{
}
CvForceControlInfo::~CvForceControlInfo()
{
}
bool CvForceControlInfo::getDefault() const
{
	return m_bDefault;
}
bool CvForceControlInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(&m_bDefault, "bDefault");
	return true;
}
//////////////////////////////////////////////////////////////////////////
//
//	CvPlayerOptionInfo
//	Player options and their default values
//
//
CvPlayerOptionInfo::CvPlayerOptionInfo() :
m_bDefault(false)
{
}
CvPlayerOptionInfo::~CvPlayerOptionInfo()
{
}
bool CvPlayerOptionInfo::getDefault() const
{
	return m_bDefault;
}
bool CvPlayerOptionInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(&m_bDefault, "bDefault");
	return true;
}
//////////////////////////////////////////////////////////////////////////
//
//	CvGraphicOptionInfo
//	Graphic options and their default values
//
//
CvGraphicOptionInfo::CvGraphicOptionInfo() :
m_bDefault(false)
{
}
CvGraphicOptionInfo::~CvGraphicOptionInfo()
{
}
bool CvGraphicOptionInfo::getDefault() const
{
	return m_bDefault;
}
bool CvGraphicOptionInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(&m_bDefault, "bDefault");
	return true;
}
//////////////////////////////////////////////////////////////////////////
//
//	CvEventTriggerInfo
//	Event triggers
//
//
CvEventTriggerInfo::CvEventTriggerInfo() :
	m_iPercentGamesActive(0),
	m_iProbability(0),
	m_iNumUnits(0),
	m_iNumBuildings(0),
	m_iNumUnitsGlobal(0),
	m_iNumBuildingsGlobal(0),
	m_iNumPlotsRequired(0),
	m_ePlotType(PLOT_PEAK),
	m_iOtherPlayerShareBorders(0),
	m_eCivic(NO_CIVIC),
	m_iMinPopulation(0),
	m_iMaxPopulation(0),
	m_iMinMapLandmass(0),
	m_iMinOurLandmass(0),
	m_iMaxOurLandmass(0),
	m_eMinDifficulty(NO_HANDICAP),
	m_iUnitDamagedWeight(0),
	m_iUnitDistanceWeight(0),
	m_iUnitExperienceWeight(0),
	m_iMinTreasury(0),
	m_bTutorial(false),
	m_bSinglePlayer(false),
	m_bTeam(false),
	m_bRecurring(false),
	m_bGlobal(false),
	m_bPickPlayer(false),
	m_bOtherPlayerWar(false),
	m_bOtherPlayerAI(false),
	m_bOtherPlayerNative(false),
	m_bOtherPlayerPlayable(false),
	m_bPickCity(false),
	m_bPickOtherPlayerCity(false),
	m_bShowPlot(true),
	m_iCityFoodWeight(0),
	m_bUnitsOnPlot(false),
	m_bOwnPlot(false),
	m_bProbabilityUnitMultiply(false),
	m_bProbabilityBuildingMultiply(false),
	m_bPrereqEventCity(false),
	m_bFrontPopup(false)
{
}
CvEventTriggerInfo::~CvEventTriggerInfo()
{
}
int CvEventTriggerInfo::getPercentGamesActive() const
{
	return m_iPercentGamesActive;
}
int CvEventTriggerInfo::getProbability() const
{
	return m_iProbability;
}
int CvEventTriggerInfo::PY_getUnitRequired(int i) const
{
	return m_info_UnitsRequired.getUnitClass(i);
}
int CvEventTriggerInfo::PY_getNumUnitsRequired() const
{
	return m_info_UnitsRequired.getLength();
}
int CvEventTriggerInfo::PY_getBuildingRequired(int i) const
{
	return m_info_BuildingsRequired.getBuildingClass(i);
}
int CvEventTriggerInfo::PY_getNumBuildingsRequired() const
{
	return m_info_BuildingsRequired.getLength();
}
int CvEventTriggerInfo::getNumUnits() const
{
	return m_iNumUnits;
}
int CvEventTriggerInfo::getNumBuildings() const
{
	return m_iNumBuildings;
}
int CvEventTriggerInfo::getNumUnitsGlobal() const
{
	return m_iNumUnitsGlobal;
}
int CvEventTriggerInfo::getNumBuildingsGlobal() const
{
	return m_iNumBuildingsGlobal;
}
int CvEventTriggerInfo::getNumPlotsRequired() const
{
	return m_iNumPlotsRequired;
}
PlotTypes CvEventTriggerInfo::getPlotType() const
{
	return m_ePlotType;
}
int CvEventTriggerInfo::PY_getPlotType() const
{
	return m_ePlotType;
}
int CvEventTriggerInfo::getOtherPlayerShareBorders() const
{
	return m_iOtherPlayerShareBorders;
}
CivicTypes CvEventTriggerInfo::getCivic() const
{
	return m_eCivic;
}
int CvEventTriggerInfo::PY_getCivic() const
{
	return m_eCivic;
}
int CvEventTriggerInfo::getMinPopulation() const
{
	return m_iMinPopulation;
}
int CvEventTriggerInfo::getMaxPopulation() const
{
	return m_iMaxPopulation;
}
int CvEventTriggerInfo::getMinMapLandmass() const
{
	return m_iMinMapLandmass;
}
int CvEventTriggerInfo::getMinOurLandmass() const
{
	return m_iMinOurLandmass;
}
int CvEventTriggerInfo::getMaxOurLandmass() const
{
	return m_iMaxOurLandmass;
}
HandicapTypes CvEventTriggerInfo::getMinDifficulty() const
{
	return m_eMinDifficulty;
}
int CvEventTriggerInfo::PY_getMinDifficulty() const
{
	return m_eMinDifficulty;
}
int CvEventTriggerInfo::getUnitDamagedWeight() const
{
	return m_iUnitDamagedWeight;
}
int CvEventTriggerInfo::getUnitDistanceWeight() const
{
	return m_iUnitDistanceWeight;
}
int CvEventTriggerInfo::getUnitExperienceWeight() const
{
	return m_iUnitExperienceWeight;
}
int CvEventTriggerInfo::getMinTreasury() const
{
	return m_iMinTreasury;
}
int CvEventTriggerInfo::PY_getEvent(int i) const
{
	return m_info_Events.getEvent(i);
}
int CvEventTriggerInfo::PY_getNumEvents() const
{
	return m_info_Events.getLength();
}
int CvEventTriggerInfo::PY_getPrereqEvent(int i) const
{
	return m_info_PrereqEvents.getEvent(i);
}
int CvEventTriggerInfo::PY_getNumPrereqEvents() const
{
	return m_info_PrereqEvents.getLength();
}
int CvEventTriggerInfo::PY_getFeatureRequired(int i) const
{
	return m_info_FeaturesRequired.getFeature(i);
}
int CvEventTriggerInfo::PY_getNumFeaturesRequired() const
{
	return m_info_FeaturesRequired.getLength();
}
int CvEventTriggerInfo::PY_getTerrainRequired(int i) const
{
	return m_info_TerrainsRequired.getTerrain(i);
}
int CvEventTriggerInfo::PY_getNumTerrainsRequired() const
{
	return m_info_TerrainsRequired.getLength();
}
int CvEventTriggerInfo::PY_getImprovementRequired(int i) const
{
	return m_info_ImprovementsRequired.getImprovement(i);
}
int CvEventTriggerInfo::PY_getNumImprovementsRequired() const
{
	return m_info_ImprovementsRequired.getLength();
}
int CvEventTriggerInfo::PY_getRouteRequired(int i) const
{
	return m_info_RoutesRequired.getRoute(i);
}
int CvEventTriggerInfo::PY_getNumRoutesRequired() const
{
	return m_info_RoutesRequired.getLength();
}
bool CvEventTriggerInfo::canTriggerOnCivCategory(CivCategoryTypes eCategory) const
{
	return m_emAllowedCivCategories.get(eCategory);
}
const InfoArray<UnitClassTypes>& CvEventTriggerInfo::getUnitsRequired() const
{
	return m_info_UnitsRequired;
}
const InfoArray<BuildingClassTypes>& CvEventTriggerInfo::getBuildingsRequired() const
{
	return m_info_BuildingsRequired;
}
const InfoArray<EventTypes>& CvEventTriggerInfo::getEvents() const
{
	return m_info_Events;
}
const InfoArray<EventTypes>& CvEventTriggerInfo::getPrereqEvents() const
{
	return m_info_PrereqEvents;
}
const InfoArray<FeatureTypes>& CvEventTriggerInfo::getFeaturesRequired() const
{
	return m_info_FeaturesRequired;
}
const InfoArray<TerrainTypes>& CvEventTriggerInfo::getTerrainsRequired() const
{
	return m_info_TerrainsRequired;
}
const InfoArray<ImprovementTypes>& CvEventTriggerInfo::getImprovementsRequired() const
{
	return m_info_ImprovementsRequired;
}
const InfoArray<RouteTypes>& CvEventTriggerInfo::getRoutesRequired() const
{
	return m_info_RoutesRequired;
}
bool CvEventTriggerInfo::isTutorial() const
{
	return m_bTutorial;
}
bool CvEventTriggerInfo::isSinglePlayer() const
{
	return m_bSinglePlayer;
}
bool CvEventTriggerInfo::isTeam() const
{
	return m_bTeam;
}

// Begin EmperorFool: Events with Images
char const* CvEventTriggerInfo::getEventArt() const
{
	if (m_szEventArt.empty())
	{
		return NULL;
	}

	return m_szEventArt;
}
// End EmperorFool: Events with Images

const CvWString& CvEventTriggerInfo::getText(int i) const
{
	FAssert(i >= 0 && i < (int)m_aszText.size());
	return m_aszText[i];
}
int CvEventTriggerInfo::getTextEra(int i) const
{
	FAssert(i >= 0 && i < (int)m_aiTextEra.size());
	return m_aiTextEra[i];
}
int CvEventTriggerInfo::getNumTexts() const
{
	FAssert(m_aiTextEra.size() == m_aszText.size());
	return m_aszText.size();
}
const CvWString& CvEventTriggerInfo::getWorldNews(int i) const
{
	FAssert(i >= 0 && i < (int)m_aszWorldNews.size());
	return m_aszWorldNews[i];
}
int CvEventTriggerInfo::getNumWorldNews() const
{
	return m_aszWorldNews.size();
}
bool CvEventTriggerInfo::isRecurring() const
{
	return m_bRecurring;
}
bool CvEventTriggerInfo::isGlobal() const
{
	return m_bGlobal;
}
bool CvEventTriggerInfo::isPickPlayer() const
{
	return m_bPickPlayer;
}
bool CvEventTriggerInfo::isOtherPlayerWar() const
{
	return m_bOtherPlayerWar;
}
bool CvEventTriggerInfo::isOtherPlayerAI() const
{
	return m_bOtherPlayerAI;
}
bool CvEventTriggerInfo::isOtherPlayerNative() const
{
	return m_bOtherPlayerNative;
}
bool CvEventTriggerInfo::isOtherPlayerPlayable() const
{
	return m_bOtherPlayerPlayable;
}
bool CvEventTriggerInfo::isPickCity() const
{
	return m_bPickCity;
}
bool CvEventTriggerInfo::isPickOtherPlayerCity() const
{
	return m_bPickOtherPlayerCity;
}
bool CvEventTriggerInfo::isShowPlot() const
{
	return m_bShowPlot;
}
int CvEventTriggerInfo::getCityFoodWeight() const
{
	return m_iCityFoodWeight;
}
bool CvEventTriggerInfo::isUnitsOnPlot() const
{
	return m_bUnitsOnPlot;
}
bool CvEventTriggerInfo::isOwnPlot() const
{
	return m_bOwnPlot;
}
bool CvEventTriggerInfo::isProbabilityUnitMultiply() const
{
	return m_bProbabilityUnitMultiply;
}
bool CvEventTriggerInfo::isProbabilityBuildingMultiply() const
{
	return m_bProbabilityBuildingMultiply;
}
bool CvEventTriggerInfo::isPrereqEventCity() const
{
	return m_bPrereqEventCity;
}
bool CvEventTriggerInfo::isFrontPopup() const
{
	return m_bFrontPopup;
}
const char* CvEventTriggerInfo::getPythonCallback() const
{
	return m_szPythonCallback;
}
const char* CvEventTriggerInfo::getPythonCanDo() const
{
	return m_szPythonCanDo;
}
const char* CvEventTriggerInfo::getPythonCanDoCity() const
{
	return m_szPythonCanDoCity;
}
const char* CvEventTriggerInfo::getPythonCanDoUnit() const
{
	return m_szPythonCanDoUnit;
}
void CvEventTriggerInfo::read(FDataStreamBase* stream)
{
	int iNumElements;
	int iElement;
	CvWString szElement;
	CvInfoBase::read(stream);
	uint uiFlag=0;
	stream->Read(&uiFlag);	// flags for expansion
	stream->Read(&m_iPercentGamesActive);
	stream->Read(&m_iProbability);
	stream->Read(&m_iNumUnits);
	stream->Read(&m_iNumBuildings);
	stream->Read(&m_iNumUnitsGlobal);
	stream->Read(&m_iNumBuildingsGlobal);
	stream->Read(&m_iNumPlotsRequired);
	stream->Read(&m_ePlotType);
	stream->Read(&m_iOtherPlayerShareBorders);
	stream->Read(&m_eCivic);
	stream->Read(&m_iMinPopulation);
	stream->Read(&m_iMaxPopulation);
	stream->Read(&m_iMinMapLandmass);
	stream->Read(&m_iMinOurLandmass);
	stream->Read(&m_iMaxOurLandmass);
	stream->Read(&m_eMinDifficulty);
	stream->Read(&m_iUnitDamagedWeight);
	stream->Read(&m_iUnitDistanceWeight);
	stream->Read(&m_iUnitExperienceWeight);
	stream->Read(&m_iMinTreasury);
	stream->Read(&iNumElements);
	// Begin EmperorFool: Events with Images
	stream->ReadString(m_szEventArt);
	// End EmperorFool: Events with Images
	stream->Read(&m_bTutorial);
	stream->Read(&m_bSinglePlayer);
	stream->Read(&m_bTeam);
	stream->Read(&m_bRecurring);
	stream->Read(&m_bGlobal);
	stream->Read(&m_bPickPlayer);
	stream->Read(&m_bOtherPlayerWar);
	stream->Read(&m_bOtherPlayerAI);
	stream->Read(&m_bOtherPlayerNative);
	stream->Read(&m_bOtherPlayerPlayable);
	stream->Read(&m_bPickCity);
	stream->Read(&m_bPickOtherPlayerCity);
	stream->Read(&m_bShowPlot);
	stream->Read(&m_iCityFoodWeight);
	stream->Read(&m_bUnitsOnPlot);
	stream->Read(&m_bOwnPlot);
	stream->Read(&m_bProbabilityUnitMultiply);
	stream->Read(&m_bProbabilityBuildingMultiply);
	stream->Read(&m_bPrereqEventCity);
	stream->Read(&m_bFrontPopup);
	stream->Read(&iNumElements);
	m_aszText.clear();
	for (int i = 0; i < iNumElements; ++i)
	{
		stream->ReadString(szElement);
		m_aszText.push_back(szElement);
	}
	m_aiTextEra.clear();
	for (int i = 0; i < iNumElements; ++i)
	{
		stream->Read(&iElement);
		m_aiTextEra.push_back(iElement);
	}
	stream->Read(&iNumElements);
	m_aszWorldNews.clear();
	for (int i = 0; i < iNumElements; ++i)
	{
		stream->ReadString(szElement);
		m_aszWorldNews.push_back(szElement);
	}
	stream->ReadString(m_szPythonCallback);
	stream->ReadString(m_szPythonCanDo);
	stream->ReadString(m_szPythonCanDoCity);
	stream->ReadString(m_szPythonCanDoUnit);
}
void CvEventTriggerInfo::write(FDataStreamBase* stream)
{
	CvInfoBase::write(stream);
	uint uiFlag=0;
	stream->Write(uiFlag);		// flag for expansion
	stream->Write(m_iPercentGamesActive);
	stream->Write(m_iProbability);
	stream->Write(m_iNumUnits);
	stream->Write(m_iNumBuildings);
	stream->Write(m_iNumUnitsGlobal);
	stream->Write(m_iNumBuildingsGlobal);
	stream->Write(m_iNumPlotsRequired);
	stream->Write(m_ePlotType);
	stream->Write(m_iOtherPlayerShareBorders);
	stream->Write(m_eCivic);
	stream->Write(m_iMinPopulation);
	stream->Write(m_iMaxPopulation);
	stream->Write(m_iMinMapLandmass);
	stream->Write(m_iMinOurLandmass);
	stream->Write(m_iMaxOurLandmass);
	stream->Write(m_eMinDifficulty);
	stream->Write(m_iUnitDamagedWeight);
	stream->Write(m_iUnitDistanceWeight);
	stream->Write(m_iUnitExperienceWeight);
	stream->Write(m_iMinTreasury);
	// Begin EmperorFool: Events with Images
	stream->WriteString(m_szEventArt);
	// End EmperorFool: Events with Images
	stream->Write(m_bTutorial);
	stream->Write(m_bSinglePlayer);
	stream->Write(m_bTeam);
	stream->Write(m_bRecurring);
	stream->Write(m_bGlobal);
	stream->Write(m_bPickPlayer);
	stream->Write(m_bOtherPlayerWar);
	stream->Write(m_bOtherPlayerAI);
	stream->Write(m_bOtherPlayerNative);
	stream->Write(m_bOtherPlayerPlayable);
	stream->Write(m_bPickCity);
	stream->Write(m_bPickOtherPlayerCity);
	stream->Write(m_bShowPlot);
	stream->Write(m_iCityFoodWeight);
	stream->Write(m_bUnitsOnPlot);
	stream->Write(m_bOwnPlot);
	stream->Write(m_bProbabilityUnitMultiply);
	stream->Write(m_bProbabilityBuildingMultiply);
	stream->Write(m_bPrereqEventCity);
	stream->Write(m_bFrontPopup);
	stream->Write(m_aszText.size());
	for (std::vector<CvWString>::iterator it = m_aszText.begin(); it != m_aszText.end(); ++it)
	{
		stream->WriteString(*it);
	}
	for (std::vector<int>::iterator it = m_aiTextEra.begin(); it != m_aiTextEra.end(); ++it)
	{
		stream->Write(*it);
	}
	stream->Write(m_aszWorldNews.size());
	for (std::vector<CvWString>::iterator it = m_aszWorldNews.begin(); it != m_aszWorldNews.end(); ++it)
	{
		stream->WriteString(*it);
	}
	stream->WriteString(m_szPythonCallback);
	stream->WriteString(m_szPythonCanDo);
	stream->WriteString(m_szPythonCanDoCity);
	stream->WriteString(m_szPythonCanDoUnit);
}
bool CvEventTriggerInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(&m_iPercentGamesActive, "iPercentGamesActive");
	pXML->GetChildXmlValByName(&m_iProbability, "iWeight");
	pXML->GetChildXmlValByName(&m_iNumUnits, "iNumUnits");
	pXML->GetChildXmlValByName(&m_iNumBuildings, "iNumBuildings");
	pXML->GetChildXmlValByName(&m_iNumUnitsGlobal, "iNumUnitsGlobal");
	pXML->GetChildXmlValByName(&m_iNumBuildingsGlobal, "iNumBuildingsGlobal");
	pXML->GetChildXmlValByName(&m_iNumPlotsRequired, "iNumPlotsRequired");
	pXML->GetEnum(getType(), m_ePlotType, "ePlotType", false);
	pXML->GetEnum(getType(), m_eCivic, "eCivic", false);
	pXML->GetChildXmlValByName(&m_iOtherPlayerShareBorders, "iOtherPlayerShareBorders");
	pXML->GetChildXmlValByName(&m_iMinPopulation, "iMinPopulation");
	pXML->GetChildXmlValByName(&m_iMaxPopulation, "iMaxPopulation");
	pXML->GetChildXmlValByName(&m_iMinMapLandmass, "iMinMapLandmass");
	pXML->GetChildXmlValByName(&m_iMinOurLandmass, "iMinOurLandmass");
	pXML->GetChildXmlValByName(&m_iMaxOurLandmass, "iMaxOurLandmass");
	pXML->GetEnum(getType(), m_eMinDifficulty, "eMinDifficulty", false);
	pXML->GetChildXmlValByName(&m_iUnitDamagedWeight, "iUnitDamagedWeight");
	pXML->GetChildXmlValByName(&m_iUnitDistanceWeight, "iUnitDistanceWeight");
	pXML->GetChildXmlValByName(&m_iUnitExperienceWeight, "iUnitExperienceWeight");
	pXML->GetChildXmlValByName(&m_iMinTreasury, "iMinTreasury");

	{
		// reads the required Civ Categories
		// usage is random access and not looping meaning EnumMap will be faster than InfoArray during the game
		// since EnumMap has no xml reading code, rely on InfoArray for reading and error messages
		// copy contents here as there is currently no generic function to copy InfoArrays into bool EnumMaps

		InfoArray<CivCategoryTypes> info_Categories;
		readXML(info_Categories, "CivCategoriesRequired");
		if (info_Categories.getLength() > 0)
		{
			for (int i = 0; i < info_Categories.getLength(); ++i)
			{
				m_emAllowedCivCategories.set(info_Categories.getCivCategory(i), true);
			}
		}
		else
		{
			// no civ categories set in xml. Assume European only
			m_emAllowedCivCategories.set(CIV_CATEGORY_EUROPEAN, true);
		}
	}

	readXML(m_info_UnitsRequired, "UnitsRequired");
	readXML(m_info_BuildingsRequired, "BuildingsRequired");
	readXML(m_info_Events, "Events");
	readXML(m_info_PrereqEvents, "PrereqEvents");
	readXML(m_info_FeaturesRequired, "FeaturesRequired");
	readXML(m_info_TerrainsRequired, "TerrainsRequired");
	readXML(m_info_ImprovementsRequired, "ImprovementsRequired");
	readXML(m_info_RoutesRequired, "RoutesRequired");

	m_aszText.clear();
	m_aiTextEra.clear();
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"TriggerTexts"))
	{
		if (pXML->SkipToNextVal())
		{
			int iNumSibs = gDLL->getXMLIFace()->GetNumChildren(pXML->GetXML());
			if (0 < iNumSibs)
			{
				if (gDLL->getXMLIFace()->SetToChild(pXML->GetXML()))
				{
					for (int j = 0; j < iNumSibs; ++j)
					{
						if (pXML->GetChildXmlVal(szTextVal))
						{
							m_aszText.push_back(szTextVal);
							pXML->GetNextXmlVal(&szTextVal);
							m_aiTextEra.push_back(pXML->FindInInfoClass(szTextVal));
							gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
						}
						if (!gDLL->getXMLIFace()->NextSibling(pXML->GetXML()))
						{
							break;
						}
					}
					gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
				}
			}
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	m_aszWorldNews.clear();
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"WorldNewsTexts"))
	{
		if (pXML->SkipToNextVal())
		{
			int iNumSibs = gDLL->getXMLIFace()->GetNumChildren(pXML->GetXML());
			if (0 < iNumSibs)
			{
				if (pXML->GetChildXmlVal(szTextVal))
				{
					for (int j=0; j<iNumSibs; ++j)
					{
						m_aszWorldNews.push_back(szTextVal);
						if (!pXML->GetNextXmlVal(&szTextVal))
						{
							break;
						}
					}
					gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
				}
			}
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	// Begin EmperorFool: Events with Images
	pXML->GetChildXmlValByName(m_szEventArt, "EventArt");
	// End EmperorFool: Events with Images
	pXML->GetChildXmlValByName(&m_bTutorial, "bTutorial");
	pXML->GetChildXmlValByName(&m_bSinglePlayer, "bSinglePlayer");
	pXML->GetChildXmlValByName(&m_bTeam, "bTeam");
	pXML->GetChildXmlValByName(&m_bRecurring, "bRecurring");
	pXML->GetChildXmlValByName(&m_bGlobal, "bGlobal");
	pXML->GetChildXmlValByName(&m_bPickPlayer, "bPickPlayer");
	pXML->GetChildXmlValByName(&m_bOtherPlayerWar, "bOtherPlayerWar");
	pXML->GetChildXmlValByName(&m_bOtherPlayerAI, "bOtherPlayerAI");
	pXML->GetChildXmlValByName(&m_bOtherPlayerNative, "bOtherPlayerNative");
	pXML->GetChildXmlValByName(&m_bOtherPlayerPlayable, "bOtherPlayerPlayable");
	pXML->GetChildXmlValByName(&m_bPickCity, "bPickCity");
	pXML->GetChildXmlValByName(&m_bPickOtherPlayerCity, "bPickOtherPlayerCity");
	pXML->GetChildXmlValByName(&m_bShowPlot, "bShowPlot");
	pXML->GetChildXmlValByName(&m_iCityFoodWeight, "iCityFoodWeight");
	pXML->GetChildXmlValByName(&m_bUnitsOnPlot, "bUnitsOnPlot");
	pXML->GetChildXmlValByName(&m_bOwnPlot, "bOwnPlot");
	pXML->GetChildXmlValByName(&m_bProbabilityUnitMultiply, "bProbabilityUnitMultiply");
	pXML->GetChildXmlValByName(&m_bProbabilityBuildingMultiply, "bProbabilityBuildingMultiply");
	pXML->GetChildXmlValByName(&m_bPrereqEventCity, "bPrereqEventPlot");
	pXML->GetChildXmlValByName(&m_bFrontPopup, "bFrontPopup");
	pXML->GetChildXmlValByName(m_szPythonCallback, "PythonCallback");
	pXML->GetChildXmlValByName(m_szPythonCanDo, "PythonCanDo");
	pXML->GetChildXmlValByName(m_szPythonCanDoCity, "PythonCanDoCity");
	pXML->GetChildXmlValByName(m_szPythonCanDoUnit, "PythonCanDoUnit");
	return true;
}

void CvEventTriggerInfo::verifyTriggerSettings() const
{
	// test that for each requirement, at least one combo allows it
	// It doesn't test if all combos of requirements is allows
	// example:
	// terrain A allows feature A
	// terrain B allows feature B
	// terrain A doesn't allow feature B
	// both terrains and both features will pass
	// removing terrain B will make feature B fail as none of the required terrains allow feature B
	verifyTriggerSettings(getFeaturesRequired());
	verifyTriggerSettings(getTerrainsRequired());
	verifyTriggerSettings(getImprovementsRequired());
	verifyTriggerSettings(getRoutesRequired());
}

template<typename T>
void CvEventTriggerInfo::verifyTriggerSettings(const InfoArray<T>& kArray) const
{
	for (int i = 0; i < kArray.getLength(); ++i)
	{
		const T eVar = kArray.get0(i);

		const char* szError = NULL;

		if (GC.getInfo(eVar).isGraphicalOnly())
		{
			szError = "TXT_KEY_EVENT_TRIGGER_ERROR_GRAPHICAL_ONLY";
		}
		else
		{
			szError = verifyTriggerSettings(eVar);
		}

		if (szError != NULL)
		{
			CvWString arg1(getType());
			CvWString arg2(GC.getInfo(eVar).getType());
			CvWString arg3(szError);

			CvString szDesc(gDLL->getText("TXT_KEY_EVENT_TRIGGER_ERROR", arg1.c_str(), arg2.c_str(), arg3.c_str()));
			CvString szTitle(gDLL->getText("TXT_KEY_EVENT_TRIGGER_ERROR_TITLE"));

			gDLL->MessageBox(szDesc.c_str(), szTitle.c_str());
		}

	}
}

const char* CvEventTriggerInfo::verifyTriggerSettings(FeatureTypes eFeature) const
{
	if (getPlotType() == PLOT_PEAK)
	{
		return "TXT_KEY_EVENT_TRIGGER_ERROR_PLOT_TYPE";
	}

	const CvFeatureInfo& kInfo = GC.getFeatureInfo(eFeature);

	const InfoArray<TerrainTypes>& Terrains = getTerrainsRequired();
	if (Terrains.getLength() > 0)
	{
		bool bValid = false;
		for (int i = 0; i < Terrains.getLength(); ++i)
		{
			if (kInfo.isTerrain(Terrains.getTerrain(i)))
			{
				bValid = true;
				break;
			}
		}
		if (!bValid)
		{
			return "TXT_KEY_EVENT_TRIGGER_ERROR_NO_TERRAIN";
		}
	}
	else if (getPlotType() != NO_PLOT)
	{
		bool bValid = false;
		for (TerrainTypes eTerrain = FIRST_TERRAIN; eTerrain < NUM_TERRAIN_TYPES; ++eTerrain)
		{
			if (kInfo.isTerrain(eTerrain))
			{
				if (GC.getTerrainInfo(eTerrain).canHavePlotType(getPlotType()))
				{
					bValid = true;
					break;
				}
			}
		}
		if (!bValid)
		{
			return "TXT_KEY_EVENT_TRIGGER_ERROR_NO_TERRAIN_PLOT_TYPE";
		}
	}

	if (kInfo.isNoImprovement() && getImprovementsRequired().getLength() > 0)
	{
		return false;
	}

	const InfoArray<ImprovementTypes>& Improvements = getImprovementsRequired();
	if (Improvements.getLength() > 0)
	{
		bool bValid = false;

		for (int i = 0; i < Improvements.getLength(); ++i)
		{
			const CvImprovementInfo& kImprovement = GC.getInfo(Improvements.getImprovement(i));
			if (kImprovement.getFeatureMakesValid(eFeature))
			{
				bValid = true;
				break;
			}
		}

		if (!bValid)
		{
			return "TXT_KEY_EVENT_TRIGGER_ERROR_NO_IMPROVEMENT";
		}
	}

	return NULL;
}
const char* CvEventTriggerInfo::verifyTriggerSettings(TerrainTypes eTerrain) const
{
	const CvTerrainInfo& kInfo = GC.getInfo(eTerrain);

	if (!kInfo.canHavePlotType(getPlotType()))
	{
		return "TXT_KEY_EVENT_TRIGGER_ERROR_PLOT_TYPE";
	}

	if (kInfo.isWater() && getRoutesRequired().getLength() > 0)
	{
		return "TXT_KEY_EVENT_TRIGGER_ERROR_NO_ROUTE";
	}

	const InfoArray<FeatureTypes>& Features = getFeaturesRequired();
	if (Features.getLength() > 0)
	{
		bool bValid = false;
		for (int i = 0; i < Features.getLength(); ++i)
		{
			if (GC.getFeatureInfo(Features.getFeature(i)).isTerrain((eTerrain)))
			{
				bValid = true;
				break;
			}
		}
		if (!bValid)
		{
			return "TXT_KEY_EVENT_TRIGGER_ERROR_NO_FEATURE";
		}
	}

	const InfoArray<ImprovementTypes>& Improvements = getImprovementsRequired();
	if (Improvements.getLength() > 0)
	{
		bool bValid = false;

		for (int i = 0; i < Improvements.getLength(); ++i)
		{
			const CvImprovementInfo& kImprovement = GC.getInfo(Improvements.getImprovement(i));
			if (kImprovement.getTerrainMakesValid(eTerrain))
			{
				bValid = true;
				break;
			}
		}

		if (!bValid)
		{
			return "TXT_KEY_EVENT_TRIGGER_ERROR_NO_IMPROVEMENT";
		}
	}

	return NULL;
}
const char* CvEventTriggerInfo::verifyTriggerSettings(ImprovementTypes eImprovement) const
{
	const CvImprovementInfo& kInfo = GC.getInfo(eImprovement);

	PlotTypes ePlot = getPlotType();

	if (ePlot != NO_PLOT)
	{
		if (kInfo.isRequiresFlatlands() && ePlot != PLOT_LAND)
		{
			return "TXT_KEY_EVENT_TRIGGER_ERROR_PLOT_TYPE";
		}
		if (kInfo.isWater() && ePlot != PLOT_OCEAN)
		{
			return "TXT_KEY_EVENT_TRIGGER_ERROR_PLOT_TYPE";
		}
		if (kInfo.isHillsMakesValid() && ePlot != PLOT_HILLS && ePlot != PLOT_PEAK)
		{
			return "TXT_KEY_EVENT_TRIGGER_ERROR_PLOT_TYPE";
		}
	}

	if (kInfo.isWater() && getRoutesRequired().getLength() > 0)
	{
		return "TXT_KEY_EVENT_TRIGGER_ERROR_NO_ROUTE";
	}

	const InfoArray<FeatureTypes>& Features = getFeaturesRequired();
	if (Features.getLength() > 0)
	{
		bool bValid = false;
		for (int i = 0; i < Features.getLength(); ++i)
		{
			if (kInfo.getFeatureMakesValid(Features.getFeature(i)))
			{
				bValid = true;
				break;
			}
		}
		if (!bValid)
		{
			return "TXT_KEY_EVENT_TRIGGER_ERROR_NO_FEATURE";
		}
	}

	const InfoArray<TerrainTypes>& Terrains = getTerrainsRequired();
	if (Terrains.getLength() > 0)
	{
		bool bValid = false;
		for (int i = 0; i < Terrains.getLength(); ++i)
		{
			if (kInfo.getTerrainMakesValid(Terrains.getTerrain(i)))
			{
				bValid = true;
				break;
			}
		}
		if (!bValid)
		{
			if (kInfo.isHillsMakesValid())
			{
				// improvement wasn't allowed on any of the terrains, but it is allowed on any terrain with hills
				// test all terrains to see if any can contain hills
				for (int i = 0; i < Terrains.getLength(); ++i)
				{
					const CvTerrainInfo& kTerrain = GC.getInfo(Terrains.getTerrain(i));
					if (kTerrain.canHavePlotType(PLOT_HILLS))
					{
						bValid = true;
						break;
					}
				}
			}
			if (!bValid)
			{
				return "TXT_KEY_EVENT_TRIGGER_ERROR_NO_TERRAIN";
			}
		}
	}
	if (kInfo.isRequiresFeature() && Features.getLength() == 0)
	{
		if (Terrains.getLength() > 0)
		{
			bool bValid = false;
			for (int i = 0; i < Terrains.getLength() && !bValid; ++i)
			{
				const TerrainTypes eTerrain = Terrains.getTerrain(i);
				if (kInfo.getTerrainMakesValid(eTerrain))
				{
					for (FeatureTypes eFeature = FIRST_FEATURE; eFeature < NUM_FEATURE_TYPES; ++eFeature)
					{
						if (kInfo.getFeatureMakesValid(eFeature))
						{
							const CvFeatureInfo& kFeature = GC.getFeatureInfo(eFeature);
							if (!kFeature.isNoImprovement() && kFeature.isTerrain((eTerrain)))
							{
								bValid = true;
								break;
							}
						}
					}
				}

			}
			if (!bValid)
			{
				return "TXT_KEY_EVENT_TRIGGER_ERROR_NO_FEATURE_TERRAIN";
			}
		}
	}

	return NULL;
}

const char* CvEventTriggerInfo::verifyTriggerSettings(RouteTypes eRoute) const
{
	if (getPlotType() == PLOT_OCEAN)
	{
		return "TXT_KEY_EVENT_TRIGGER_ERROR_PLOT_TYPE";
	}

	return NULL;
}




//////////////////////////////////////////////////////////////////////////
//
//	CvEventInfo
//	Events
//
//
CvEventInfo::CvEventInfo() :
	m_bQuest(false),
	m_bGlobal(false),
	m_bTeam(false),
	m_bCityEffect(false),
	m_bOtherPlayerCityEffect(false),
	m_bGoldToPlayer(false),
	m_bDeclareWar(false),
	m_bDisbandUnit(false),
	m_iGold(0),
	m_iRandomGold(0),
	m_iCulture(0),
	m_iHealth(0), // R&R, ray, change for Health in Events
	m_iUnitClass(NO_UNITCLASS),
	m_iNumUnits(0),
	m_iUnitExperience(0),
	m_iUnitImmobileTurns(0),
	m_iBuildingClass(NO_BUILDINGCLASS),
	m_iBuildingChange(0),
	m_iFood(0),
	m_iFoodPercent(0),
	m_iFeature(NO_FEATURE),
	m_iFeatureChange(0),
	m_iImprovement(NO_IMPROVEMENT),
	m_iImprovementChange(0),
	m_iRoute(NO_ROUTE),
	m_iRouteChange(0),
	m_iBonusRevealed(NO_BONUS),
	m_iOurAttitudeModifier(0),
	m_iAttitudeModifier(0),
	m_iTheirEnemyAttitudeModifier(0),
	m_iPopulationChange(0),
	m_iRevoltTurns(0),
	m_iMinPillage(0),
	m_iMaxPillage(0),
	m_iUnitPromotion(NO_PROMOTION),
	m_iAIValue(0),
	// TAC - Generic Parameters for Events - Ray - START
	m_iGenericParameter1(0),
	m_iGenericParameter2(0),
	m_iGenericParameter3(0),
	m_iGenericParameter4(0),
	// TAC - Generic Parameters for Events - Ray - END
	m_aiPlotExtraYields(NULL),
	m_aiAdditionalEventChance(NULL),
	m_aiAdditionalEventTime(NULL),
	m_aiClearEventChance(NULL),
	m_aiUnitCombatPromotions(NULL),
	m_aiUnitClassPromotions(NULL)
{
}
CvEventInfo::~CvEventInfo()
{
	SAFE_DELETE_ARRAY(m_aiPlotExtraYields);
	SAFE_DELETE_ARRAY(m_aiAdditionalEventChance);
	SAFE_DELETE_ARRAY(m_aiAdditionalEventTime);
	SAFE_DELETE_ARRAY(m_aiClearEventChance);
	SAFE_DELETE_ARRAY(m_aiUnitCombatPromotions);
	SAFE_DELETE_ARRAY(m_aiUnitClassPromotions);
}
bool CvEventInfo::isGlobal() const
{
	return m_bGlobal;
}
bool CvEventInfo::isQuest() const
{
	return m_bQuest;
}
bool CvEventInfo::isTeam() const
{
	return m_bTeam;
}
bool CvEventInfo::isCityEffect() const
{
	return m_bCityEffect;
}
bool CvEventInfo::isOtherPlayerCityEffect() const
{
	return m_bOtherPlayerCityEffect;
}
bool CvEventInfo::isGoldToPlayer() const
{
	return m_bGoldToPlayer;
}
bool CvEventInfo::isDeclareWar() const
{
	return m_bDeclareWar;
}
bool CvEventInfo::isDisbandUnit() const
{
	return m_bDisbandUnit;
}
int CvEventInfo::getGold() const
{
	return m_iGold;
}
int CvEventInfo::getRandomGold() const
{
	return m_iRandomGold;
}
int CvEventInfo::getCulture() const
{
	return m_iCulture;
}
int CvEventInfo::getHealth() const // R&R, ray, change for Health in Events
{
	return m_iHealth;
}
int CvEventInfo::getUnitClass() const
{
	return m_iUnitClass;
}
int CvEventInfo::getNumUnits() const
{
	return m_iNumUnits;
}
int CvEventInfo::getUnitExperience() const
{
	return m_iUnitExperience;
}
int CvEventInfo::getUnitImmobileTurns() const
{
	return m_iUnitImmobileTurns;
}
int CvEventInfo::getBuildingClass() const
{
	return m_iBuildingClass;
}
int CvEventInfo::getBuildingChange() const
{
	return m_iBuildingChange;
}
int CvEventInfo::getFood() const
{
	return m_iFood;
}
int CvEventInfo::getFoodPercent() const
{
	return m_iFoodPercent;
}
int CvEventInfo::getFeature() const
{
	return m_iFeature;
}
int CvEventInfo::getFeatureChange() const
{
	return m_iFeatureChange;
}
int CvEventInfo::getImprovement() const
{
	return m_iImprovement;
}
int CvEventInfo::getImprovementChange() const
{
	return m_iImprovementChange;
}
int CvEventInfo::getRoute() const
{
	return m_iRoute;
}
int CvEventInfo::getRouteChange() const
{
	return m_iRouteChange;
}
int CvEventInfo::getBonusRevealed() const
{
	return m_iBonusRevealed;
}
int CvEventInfo::getOurAttitudeModifier() const
{
	return m_iOurAttitudeModifier;
}
int CvEventInfo::getAttitudeModifier() const
{
	return m_iAttitudeModifier;
}
int CvEventInfo::getTheirEnemyAttitudeModifier() const
{
	return m_iTheirEnemyAttitudeModifier;
}
int CvEventInfo::getPopulationChange() const
{
	return m_iPopulationChange;
}
int CvEventInfo::getRevoltTurns() const
{
	return m_iRevoltTurns;
}
int CvEventInfo::getMinPillage() const
{
	return m_iMinPillage;
}
int CvEventInfo::getMaxPillage() const
{
	return m_iMaxPillage;
}
int CvEventInfo::getUnitPromotion() const
{
	return m_iUnitPromotion;
}
int CvEventInfo::getAIValue() const
{
	return m_iAIValue;
}

// TAC - Generic Parameters for Events - Ray - START
int CvEventInfo::getGenericParameter(int x) const
{
	if (x < 1) return 0;
	if (x == 1) return m_iGenericParameter1;
	if (x == 2) return m_iGenericParameter2;
	if (x == 3) return m_iGenericParameter3;
	if (x == 4) return m_iGenericParameter4;
	if (x > 4) return 0;

	//koma13
	return 0;
}
// TAC - Generic Parameters for Events - Ray - END

int CvEventInfo::getAdditionalEventChance(int i) const
{
	FAssert (i >= 0 && i < GC.getNumEventInfos());
	return m_aiAdditionalEventChance ? m_aiAdditionalEventChance[i] : 0;
}
int CvEventInfo::getAdditionalEventTime(int i) const
{
	FAssert (i >= 0 && i < GC.getNumEventInfos());
	return m_aiAdditionalEventTime ? m_aiAdditionalEventTime[i] : 0;
}
int CvEventInfo::getClearEventChance(int i) const
{
	FAssert (i >= 0 && i < GC.getNumEventInfos());
	return m_aiClearEventChance ? m_aiClearEventChance[i] : 0;
}
int CvEventInfo::getPlotExtraYield(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiPlotExtraYields ? m_aiPlotExtraYields[i] : -1;
}
int CvEventInfo::getUnitCombatPromotion(int i) const
{
	FAssertMsg(i < GC.getNumUnitCombatInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiUnitCombatPromotions ? m_aiUnitCombatPromotions[i] : -1;
}
int CvEventInfo::getUnitClassPromotion(int i) const
{
	FAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aiUnitClassPromotions ? m_aiUnitClassPromotions[i] : -1;
}
const CvWString& CvEventInfo::getWorldNews(int i) const
{
	FAssert(i >= 0 && i < (int)m_aszWorldNews.size());
	return m_aszWorldNews[i];
}
int CvEventInfo::getNumWorldNews() const
{
	return m_aszWorldNews.size();
}
int CvEventInfo::getBuildingYieldChange(int iBuildingClass, int iYield) const
{
	for (std::vector<BuildingYieldChange>::const_iterator it = m_aBuildingYieldChanges.begin(); it != m_aBuildingYieldChanges.end(); ++it)
	{
		if ((*it).eBuildingClass == (BuildingClassTypes)iBuildingClass && (*it).eYield == (YieldTypes)iYield)
		{
			return (*it).iChange;
		}
	}
	return 0;
}
int CvEventInfo::getNumBuildingYieldChanges() const
{
	return m_aBuildingYieldChanges.size();
}
const char* CvEventInfo::getPythonCallback() const
{
	return m_szPythonCallback;
}
const char* CvEventInfo::getPythonExpireCheck() const
{
	return m_szPythonExpireCheck;
}
const char* CvEventInfo::getPythonCanDo() const
{
	return m_szPythonCanDo;
}
const char* CvEventInfo::getPythonHelp() const
{
	return m_szPythonHelp;
}
const char* CvEventInfo::getHelpText() const
{
	return m_szHelpText;
}
const wchar* CvEventInfo::getUnitNameKey() const
{
	return m_szUnitName;
}
const wchar* CvEventInfo::getQuestFailTextKey() const
{
	return m_szQuestFailText;
}
const wchar* CvEventInfo::getLocalInfoTextKey() const
{
	return m_szLocalInfoText;
}
const wchar* CvEventInfo::getOtherPlayerPopup() const
{
	return m_szOtherPlayerPopup;
}

// changed to make apply and can apply calculate using the same code (read: same result) - Nightinggale
int CvEventInfo::getFoodChange(const CvCity* pCity) const
{
	int iFood = getFood();

	if (iFood != 0)
	{
		int iGrowthPercent = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
		iFood *= iGrowthPercent;
		iFood /= 100;
	}

	if (getFoodPercent())
	{
		iFood = (getFoodPercent() * pCity->getFood()) / 100;
	}
	return iFood;
}

void CvEventInfo::read(FDataStreamBase* stream)
{
	CvInfoBase::read(stream);
	uint uiFlag=0;
	stream->Read(&uiFlag);	// flags for expansion
	stream->Read(&m_bQuest);
	stream->Read(&m_bGlobal);
	stream->Read(&m_bTeam);
	stream->Read(&m_bCityEffect);
	stream->Read(&m_bOtherPlayerCityEffect);
	stream->Read(&m_bGoldToPlayer);
	stream->Read(&m_bDeclareWar);
	stream->Read(&m_bDisbandUnit);
	stream->Read(&m_iGold);
	stream->Read(&m_iRandomGold);
	stream->Read(&m_iCulture);
	stream->Read(&m_iHealth); // R&R, ray, change for Health in Events
	stream->Read(&m_iUnitClass);
	stream->Read(&m_iNumUnits);
	stream->Read(&m_iUnitExperience);
	stream->Read(&m_iUnitImmobileTurns);
	stream->Read(&m_iBuildingClass);
	stream->Read(&m_iBuildingChange);
	stream->Read(&m_iFood);
	stream->Read(&m_iFoodPercent);
	stream->Read(&m_iFeature);
	stream->Read(&m_iFeatureChange);
	stream->Read(&m_iImprovement);
	stream->Read(&m_iImprovementChange);
	stream->Read(&m_iRoute);
	stream->Read(&m_iRouteChange);
	stream->Read(&m_iBonusRevealed);
	stream->Read(&m_iOurAttitudeModifier);
	stream->Read(&m_iAttitudeModifier);
	stream->Read(&m_iTheirEnemyAttitudeModifier);
	stream->Read(&m_iPopulationChange);
	stream->Read(&m_iRevoltTurns);
	stream->Read(&m_iMinPillage);
	stream->Read(&m_iMaxPillage);
	stream->Read(&m_iUnitPromotion);
	stream->Read(&m_iAIValue);
	// TAC - Generic Parameters for Events - Ray - START
	stream->Read(&m_iGenericParameter1);
	stream->Read(&m_iGenericParameter2);
	stream->Read(&m_iGenericParameter3);
	stream->Read(&m_iGenericParameter4);
	// TAC - Generic Parameters for Events - Ray - END
	SAFE_DELETE_ARRAY(m_aiPlotExtraYields);
	m_aiPlotExtraYields = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_aiPlotExtraYields);
	SAFE_DELETE_ARRAY(m_aiAdditionalEventChance);
	m_aiAdditionalEventChance = new int[GC.getNumEventInfos()];
	stream->Read(GC.getNumEventInfos(), m_aiAdditionalEventChance);
	SAFE_DELETE_ARRAY(m_aiAdditionalEventTime);
	m_aiAdditionalEventTime = new int[GC.getNumEventInfos()];
	stream->Read(GC.getNumEventInfos(), m_aiAdditionalEventTime);
	SAFE_DELETE_ARRAY(m_aiClearEventChance);
	m_aiClearEventChance = new int[GC.getNumEventInfos()];
	stream->Read(GC.getNumEventInfos(), m_aiClearEventChance);
	SAFE_DELETE_ARRAY(m_aiUnitCombatPromotions);
	m_aiUnitCombatPromotions = new int[GC.getNumUnitCombatInfos()];
	stream->Read(GC.getNumUnitCombatInfos(), m_aiUnitCombatPromotions);
	SAFE_DELETE_ARRAY(m_aiUnitClassPromotions);
	m_aiUnitClassPromotions = new int[GC.getNumUnitClassInfos()];
	stream->Read(GC.getNumUnitClassInfos(), m_aiUnitClassPromotions);
	int iNumElements;
	CvWString szText;
	stream->Read(&iNumElements);
	m_aszWorldNews.clear();
	for (int i = 0; i < iNumElements; ++i)
	{
		stream->ReadString(szText);
		m_aszWorldNews.push_back(szText);
	}
	stream->Read(&iNumElements);
	m_aBuildingYieldChanges.clear();
	for (int i = 0; i < iNumElements; ++i)
	{
		BuildingYieldChange kChange;
		kChange.read(stream);
		m_aBuildingYieldChanges.push_back(kChange);
	}
	stream->ReadString(m_szUnitName);
	stream->ReadString(m_szOtherPlayerPopup);
	stream->ReadString(m_szQuestFailText);
	stream->ReadString(m_szLocalInfoText);
	stream->ReadString(m_szPythonCallback);
	stream->ReadString(m_szPythonExpireCheck);
	stream->ReadString(m_szPythonCanDo);
	stream->ReadString(m_szPythonHelp);
}
void CvEventInfo::write(FDataStreamBase* stream)
{
	CvInfoBase::write(stream);
	uint uiFlag=0;
	stream->Write(uiFlag);		// flag for expansion
	stream->Write(m_bQuest);
	stream->Write(m_bGlobal);
	stream->Write(m_bTeam);
	stream->Write(m_bCityEffect);
	stream->Write(m_bOtherPlayerCityEffect);
	stream->Write(m_bGoldToPlayer);
	stream->Write(m_bDeclareWar);
	stream->Write(m_bDisbandUnit);
	stream->Write(m_iGold);
	stream->Write(m_iRandomGold);
	stream->Write(m_iCulture);
	stream->Write(m_iHealth); // R&R, ray, change for Health in Events
	stream->Write(m_iUnitClass);
	stream->Write(m_iNumUnits);
	stream->Write(m_iUnitExperience);
	stream->Write(m_iUnitImmobileTurns);
	stream->Write(m_iBuildingClass);
	stream->Write(m_iBuildingChange);
	stream->Write(m_iFood);
	stream->Write(m_iFoodPercent);
	stream->Write(m_iFeature);
	stream->Write(m_iFeatureChange);
	stream->Write(m_iImprovement);
	stream->Write(m_iImprovementChange);
	stream->Write(m_iRoute);
	stream->Write(m_iRouteChange);
	stream->Write(m_iBonusRevealed);
	stream->Write(m_iOurAttitudeModifier);
	stream->Write(m_iAttitudeModifier);
	stream->Write(m_iTheirEnemyAttitudeModifier);
	stream->Write(m_iPopulationChange);
	stream->Write(m_iRevoltTurns);
	stream->Write(m_iMinPillage);
	stream->Write(m_iMaxPillage);
	stream->Write(m_iUnitPromotion);
	stream->Write(m_iAIValue);
	// TAC - Generic Parameters for Events - Ray - START
	stream->Write(m_iGenericParameter1);
	stream->Write(m_iGenericParameter2);
	stream->Write(m_iGenericParameter3);
	stream->Write(m_iGenericParameter4);
	// TAC - Generic Parameters for Events - Ray - END
	stream->Write(NUM_YIELD_TYPES, m_aiPlotExtraYields);
	stream->Write(GC.getNumEventInfos(), m_aiAdditionalEventChance);
	stream->Write(GC.getNumEventInfos(), m_aiAdditionalEventTime);
	stream->Write(GC.getNumEventInfos(), m_aiClearEventChance);
	stream->Write(GC.getNumUnitCombatInfos(), m_aiUnitCombatPromotions);
	stream->Write(GC.getNumUnitClassInfos(), m_aiUnitClassPromotions);
	stream->Write(m_aszWorldNews.size());
	for (std::vector<CvWString>::iterator it = m_aszWorldNews.begin(); it != m_aszWorldNews.end(); ++it)
	{
		stream->WriteString(*it);
	}
	stream->Write(m_aBuildingYieldChanges.size());
	for (std::vector<BuildingYieldChange>::iterator it = m_aBuildingYieldChanges.begin(); it != m_aBuildingYieldChanges.end(); ++it)
	{
		(*it).write(stream);
	}
	stream->WriteString(m_szUnitName);
	stream->WriteString(m_szOtherPlayerPopup);
	stream->WriteString(m_szQuestFailText);
	stream->WriteString(m_szLocalInfoText);
	stream->WriteString(m_szPythonCallback);
	stream->WriteString(m_szPythonExpireCheck);
	stream->WriteString(m_szPythonCanDo);
	stream->WriteString(m_szPythonHelp);
}
bool CvEventInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(&m_bQuest, "bQuest");
	pXML->GetChildXmlValByName(&m_bGlobal, "bGlobal");
	pXML->GetChildXmlValByName(&m_bTeam, "bTeam");
	pXML->GetChildXmlValByName(&m_bCityEffect, "bPickCity");
	pXML->GetChildXmlValByName(&m_bOtherPlayerCityEffect, "bPickOtherPlayerCity");
	pXML->GetChildXmlValByName(&m_bGoldToPlayer, "bGoldToPlayer");
	pXML->GetChildXmlValByName(&m_bDeclareWar, "bDeclareWar");
	pXML->GetChildXmlValByName(&m_iGold, "iGold");
	pXML->GetChildXmlValByName(&m_iRandomGold, "iRandomGold");
	pXML->GetChildXmlValByName(&m_iCulture, "iCulture");
	pXML->GetChildXmlValByName(&m_iHealth, "iHealth"); // R&R, ray, change for Health in Events
	pXML->GetChildXmlValByName(szTextVal, "UnitClass");
	m_iUnitClass = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(&m_iNumUnits, "iNumFreeUnits");
	pXML->GetChildXmlValByName(&m_bDisbandUnit, "bDisbandUnit");
	pXML->GetChildXmlValByName(&m_iUnitExperience, "iUnitExperience");
	pXML->GetChildXmlValByName(&m_iUnitImmobileTurns, "iUnitImmobileTurns");
	pXML->GetChildXmlValByName(szTextVal, "BuildingClass");
	m_iBuildingClass = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(&m_iBuildingChange, "iBuildingChange");
	pXML->GetChildXmlValByName(&m_iFood, "iFood");
	pXML->GetChildXmlValByName(&m_iFoodPercent, "iFoodPercent");
	pXML->GetChildXmlValByName(szTextVal, "FeatureType");
	m_iFeature = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(&m_iFeatureChange, "iFeatureChange");
	pXML->GetChildXmlValByName(szTextVal, "ImprovementType");
	m_iImprovement = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(&m_iImprovementChange, "iImprovementChange");
	pXML->GetChildXmlValByName(szTextVal, "RouteType");
	m_iRoute = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(&m_iRouteChange, "iRouteChange");
	pXML->GetChildXmlValByName(szTextVal, "BonusRevealed");
	m_iBonusRevealed = pXML->FindInInfoClass(szTextVal);
	pXML->SetVariableListTagPair(&m_aiPlotExtraYields, "PlotExtraYields", NUM_YIELD_TYPES, 0);
	pXML->GetChildXmlValByName(&m_iOurAttitudeModifier, "iOurAttitudeModifier");
	pXML->GetChildXmlValByName(&m_iAttitudeModifier, "iAttitudeModifier");
	pXML->GetChildXmlValByName(&m_iTheirEnemyAttitudeModifier, "iTheirEnemyAttitudeModifier");
	pXML->GetChildXmlValByName(&m_iPopulationChange, "iPopulationChange");
	pXML->GetChildXmlValByName(&m_iRevoltTurns, "iRevoltTurns");
	pXML->GetChildXmlValByName(&m_iMinPillage, "iMinPillage");
	pXML->GetChildXmlValByName(&m_iMaxPillage, "iMaxPillage");
	pXML->GetChildXmlValByName(szTextVal, "UnitPromotion");
	m_iUnitPromotion = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(&m_iAIValue, "iAIValue");
	// TAC - Generic Parameters for Events - Ray - START
	pXML->GetIntOrType(getType(), m_iGenericParameter1, "iGenericParameter1", false);
	pXML->GetIntOrType(getType(), m_iGenericParameter2, "iGenericParameter2", false);
	pXML->GetIntOrType(getType(), m_iGenericParameter3, "iGenericParameter3", false);
	pXML->GetIntOrType(getType(), m_iGenericParameter4, "iGenericParameter4", false);
	// TAC - Generic Parameters for Events - Ray - END
	CvString* pszPromotions = NULL;
	FAssertMsg(NULL == m_aiUnitCombatPromotions, "Memory leak");
	m_aiUnitCombatPromotions = new int[GC.getNumUnitCombatInfos()];
	pXML->SetVariableListTagPair<CvString>(&pszPromotions, "UnitCombatPromotions", GC.getNumUnitCombatInfos(), "NONE");
	for (int i = 0; i < GC.getNumUnitCombatInfos(); ++i)
	{
		m_aiUnitCombatPromotions[i] = pXML->FindInInfoClass(pszPromotions[i]);
	}
	SAFE_DELETE_ARRAY(pszPromotions);
	FAssertMsg(NULL == m_aiUnitClassPromotions, "Memory leak");
	m_aiUnitClassPromotions = new int[GC.getNumUnitClassInfos()];
	pXML->SetVariableListTagPair<CvString>(&pszPromotions, "UnitClassPromotions", GC.getNumUnitClassInfos(), "NONE");
	for (int i = 0; i < GC.getNumUnitClassInfos(); ++i)
	{
		m_aiUnitClassPromotions[i] = pXML->FindInInfoClass(pszPromotions[i]);
	}
	SAFE_DELETE_ARRAY(pszPromotions);
	pXML->GetChildXmlValByName(m_szUnitName, "UnitName");
	pXML->GetChildXmlValByName(m_szOtherPlayerPopup, "OtherPlayerPopup");
	pXML->GetChildXmlValByName(m_szQuestFailText, "QuestFailText");
	pXML->GetChildXmlValByName(m_szLocalInfoText, "LocalInfoText");
	pXML->GetChildXmlValByName(m_szPythonCallback, "PythonCallback");
	pXML->GetChildXmlValByName(m_szPythonExpireCheck, "PythonExpireCheck");
	pXML->GetChildXmlValByName(m_szPythonCanDo, "PythonCanDo");
	pXML->GetChildXmlValByName(m_szPythonHelp, "PythonHelp");
	pXML->GetChildXmlValByName(m_szHelpText, "HelpText");
	m_aszWorldNews.clear();
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"WorldNewsTexts"))
	{
		if (pXML->SkipToNextVal())
		{
			int iNumSibs = gDLL->getXMLIFace()->GetNumChildren(pXML->GetXML());
			if (0 < iNumSibs)
			{
				if (pXML->GetChildXmlVal(szTextVal))
				{
					for (int j=0; j<iNumSibs; ++j)
					{
						m_aszWorldNews.push_back(szTextVal);
						if (!pXML->GetNextXmlVal(&szTextVal))
						{
							break;
						}
					}
					gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
				}
			}
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	m_aBuildingYieldChanges.clear();
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"BuildingExtraYields"))
	{
		if (pXML->SkipToNextVal())
		{
			int iNumSibs = gDLL->getXMLIFace()->GetNumChildren(pXML->GetXML());
			if (0 < iNumSibs)
			{
				if (gDLL->getXMLIFace()->SetToChild(pXML->GetXML()))
				{
					for (int j = 0; j < iNumSibs; ++j)
					{
						if (pXML->GetChildXmlVal(szTextVal))
						{
							BuildingYieldChange kChange;
							kChange.eBuildingClass = (BuildingClassTypes)pXML->FindInInfoClass(szTextVal);
							pXML->GetNextXmlVal(&szTextVal);
							kChange.eYield = (YieldTypes)pXML->FindInInfoClass(szTextVal);
							pXML->GetNextXmlVal(&kChange.iChange);
							m_aBuildingYieldChanges.push_back(kChange);
							gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
						}
						if (!gDLL->getXMLIFace()->NextSibling(pXML->GetXML()))
						{
							break;
						}
					}
					gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
				}
			}
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	return true;
}
bool CvEventInfo::readPass2(CvXMLLoadUtility* pXML)
{
	pXML->SetVariableListTagPair(&m_aiAdditionalEventChance, "AdditionalEvents", GC.getNumEventInfos(), 0);
	pXML->SetVariableListTagPair(&m_aiAdditionalEventTime, "EventTimes", GC.getNumEventInfos(), 0);
	pXML->SetVariableListTagPair(&m_aiClearEventChance, "ClearEvents", GC.getNumEventInfos(), 0);
	return true;
}
CvMainMenuInfo::CvMainMenuInfo()
{
}
CvMainMenuInfo::~CvMainMenuInfo()
{
}
std::string CvMainMenuInfo::getScene() const
{
	GC.cleanInfoStrings();
	return m_szScene;
}
std::string CvMainMenuInfo::getSoundtrack() const
{
	/// GameFont XML control - start - Nightinggale
	static bool bGameFontSet = false;
	if (!bGameFontSet)
	{
		// setup GameFont, but only the first time the main menu is reached
		bGameFontSet = true;
		GC.setupGameFontChars();
	}
	/// GameFont XML control - end - Nightinggale

	return m_szSoundtrack;
}
std::string CvMainMenuInfo::getLoading() const
{
	// here we need a random number to pick an image.
	// Instead of messing with random seed, just use the number of milliseconds since windows started
	int iRandom = GetTickCount();
	iRandom = iRandom % m_a_szLoading.size();
	return m_a_szLoading[iRandom];
}
std::string CvMainMenuInfo::getLoadingSlideshow() const
{
	return m_szLoadingSlideshow;
}

bool CvMainMenuInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(m_szScene, "Scene");
	pXML->GetChildXmlValByName(m_szSoundtrack, "Soundtrack");
	pXML->GetChildXmlValByName(m_szLoadingSlideshow, "LoadingSlideshow");

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "LoadingScreens"))
	{
		CvString szBuffer;
		do
		{
			pXML->GetChildXmlVal(szBuffer);
			if (!szBuffer.empty())
			{
				m_a_szLoading.push_back(szBuffer);
			}
		} while (gDLL->getXMLIFace()->NextSibling(pXML->GetXML()));
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	return true;
}

CvFatherInfo::CvFatherInfo() :
	m_iFatherCategory(NO_FATHERCATEGORY),
	m_eTrait(NO_TRAIT),
	m_eCivEffect(NO_CIV_EFFECT),
	m_aiFreeUnits(NULL),
	m_aiPointCost(NULL),
	m_abRevealImprovement(NULL)
{
}

CvFatherInfo::~CvFatherInfo()
{
	SAFE_DELETE_ARRAY(m_aiFreeUnits);
	SAFE_DELETE_ARRAY(m_aiPointCost);
	SAFE_DELETE_ARRAY(m_abRevealImprovement);
}

int CvFatherInfo::getFatherCategory() const
{
	return m_iFatherCategory;
}

TraitTypes CvFatherInfo::getTrait() const
{
	return m_eTrait;
}

int CvFatherInfo::getFreeUnits(int iUnitClass) const
{
	FAssert(iUnitClass >= 0 && iUnitClass < GC.getNumUnitClassInfos());
	return m_aiFreeUnits ? m_aiFreeUnits[iUnitClass] : -1;
}

int CvFatherInfo::getPointCost(int i) const
{
	FAssert((i >= 0) && (i < GC.getNumFatherPointInfos()));
	return m_aiPointCost ? m_aiPointCost[i] : -1;
}

bool CvFatherInfo::isRevealImprovement(int iImprovement) const
{
	FAssert(iImprovement >= 0 && iImprovement < GC.getNumImprovementInfos());
	return m_abRevealImprovement[iImprovement];
}

const char* CvFatherInfo::getPortrait() const
{
	return m_szPortrait;
}

const wchar* CvFatherInfo::getQuoteKey()
{
	return m_szQuoteKey;
}

const char* CvFatherInfo::getSound() const
{
	return m_szSound;
}

const char* CvFatherInfo::getSoundMP() const
{
	return m_szSoundMP;
}

void CvFatherInfo::read(FDataStreamBase* stream)
{
	CvInfoBase::read(stream);

	uint uiFlag=0;
	stream->Read(&uiFlag);	// flags for expansion
	stream->Read(&m_iFatherCategory);
	stream->Read(&m_eTrait);

	SAFE_DELETE_ARRAY(m_aiFreeUnits);
	m_aiFreeUnits = new int [GC.getNumUnitClassInfos()];
	stream->Read(GC.getNumUnitClassInfos(), m_aiFreeUnits);

	SAFE_DELETE_ARRAY(m_aiPointCost);
	m_aiPointCost = new int [GC.getNumFatherPointInfos()];
	stream->Read(GC.getNumFatherPointInfos(), m_aiPointCost);

	SAFE_DELETE_ARRAY(m_abRevealImprovement);
	m_abRevealImprovement = new bool [GC.getNumImprovementInfos()];
	stream->Read(GC.getNumImprovementInfos(), m_abRevealImprovement);

	stream->ReadString(m_szQuoteKey);
	stream->ReadString(m_szSound);
	stream->ReadString(m_szSoundMP);
	stream->ReadString(m_szPortrait);
}

void CvFatherInfo::write(FDataStreamBase* stream)
{
	CvInfoBase::write(stream);

	uint uiFlag=0;
	stream->Write(uiFlag);		// flag for expansion
	stream->Write(m_iFatherCategory);
	stream->Write(m_eTrait);
	stream->Write(GC.getNumUnitClassInfos(), m_aiFreeUnits);
	stream->Write(GC.getNumFatherPointInfos(), m_aiPointCost);
	stream->Write(GC.getNumImprovementInfos(), m_abRevealImprovement);
	stream->WriteString(m_szQuoteKey);
	stream->WriteString(m_szSound);
	stream->WriteString(m_szSoundMP);
	stream->WriteString(m_szPortrait);
}

bool CvFatherInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	CvString szTextVal;
	pXML->GetChildXmlValByName(szTextVal, "FatherCategory");
	m_iFatherCategory = GC.getInfoTypeForString(szTextVal);

	pXML->GetChildXmlValByName(m_szPortrait, "Portrait");
	pXML->GetEnum(getType(), m_eTrait, "Trait", false);

	pXML->GetEnum(getType(), m_eCivEffect, "eCivEffect", false);

	pXML->SetVariableListTagPair(&m_aiFreeUnits, "FreeUnits", GC.getNumUnitClassInfos(), 0);
	pXML->SetVariableListTagPair(&m_aiPointCost, "FatherPointCosts", GC.getNumFatherPointInfos(), 0);
	pXML->SetVariableListTagPair(&m_abRevealImprovement, "RevealImprovements", GC.getNumImprovementInfos(), false);

	pXML->GetChildXmlValByName(m_szQuoteKey, "Quote");
	pXML->GetChildXmlValByName(m_szSound, "Sound");
	pXML->GetChildXmlValByName(m_szSoundMP, "SoundMP");
	return true;
}

CvFatherPointInfo::CvFatherPointInfo() :
	m_iChar(0),
	m_iFontButtonIndex(0),
	m_iLandTilePoints(0),
	m_iWaterTilePoints(0),
	m_iMeetNativePoints(0),
	m_iScoutVillagePoints(0),
	m_iGoodyPoints(0),
	m_iExperiencePoints(0),
	m_iConquerCityPoints(0),
	m_iRazeCityPoints(0),
	m_iMissionaryPoints(0),
	m_iProductionConversionPoints(0),
	m_iEuropeTradeGoldPointPercent(0),
	m_iNativeTradeGoldPointPercent(0),
	m_aiBuildingPoints(NULL),
	m_aiYieldPoints(NULL)
{
}

CvFatherPointInfo::~CvFatherPointInfo()
{
	SAFE_DELETE_ARRAY(m_aiBuildingPoints);
	SAFE_DELETE_ARRAY(m_aiYieldPoints);
}

int CvFatherPointInfo::getChar() const
{
	return m_iChar;
}

void CvFatherPointInfo::setChar(int i)
{
	m_iChar = i;
}

int CvFatherPointInfo::getFontButtonIndex() const
{
	return m_iFontButtonIndex;
}

int CvFatherPointInfo::getLandTilePoints() const
{
	return m_iLandTilePoints;
}

int CvFatherPointInfo::getWaterTilePoints() const
{
	return m_iWaterTilePoints;
}

int CvFatherPointInfo::getMeetNativePoints() const
{
	return m_iMeetNativePoints;
}

int CvFatherPointInfo::getScoutVillagePoints() const
{
	return m_iScoutVillagePoints;
}

int CvFatherPointInfo::getGoodyPoints() const
{
	return m_iGoodyPoints;
}

int CvFatherPointInfo::getExperiencePoints() const
{
	return m_iExperiencePoints;
}

int CvFatherPointInfo::getConquerCityPoints() const
{
	return m_iConquerCityPoints;
}

int CvFatherPointInfo::getRazeCityPoints() const
{
	return m_iRazeCityPoints;
}

int CvFatherPointInfo::getMissionaryPoints() const
{
	return m_iMissionaryPoints;
}

int CvFatherPointInfo::getProductionConversionPoints() const
{
	return m_iProductionConversionPoints;
}

int CvFatherPointInfo::getEuropeTradeGoldPointPercent() const
{
	return m_iEuropeTradeGoldPointPercent;
}

int CvFatherPointInfo::getNativeTradeGoldPointPercent() const
{
	return m_iNativeTradeGoldPointPercent;
}

int CvFatherPointInfo::getBuildingPoints(int iBuildingClass) const
{
	FAssert(iBuildingClass >= 0 && iBuildingClass < GC.getNumBuildingClassInfos());
	return m_aiBuildingPoints[iBuildingClass];
}

int CvFatherPointInfo::getYieldPoints(int iYield) const
{
	FAssert(iYield >= 0 && iYield < NUM_YIELD_TYPES);
	return m_aiYieldPoints[iYield];
}

bool CvFatherPointInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(&m_iFontButtonIndex, "iFontButtonIndex");
	pXML->GetChildXmlValByName(&m_iLandTilePoints, "iLandTilePoints");
	pXML->GetChildXmlValByName(&m_iWaterTilePoints, "iWaterTilePoints");
	pXML->GetChildXmlValByName(&m_iMeetNativePoints, "iMeetNativePoints");
	pXML->GetChildXmlValByName(&m_iScoutVillagePoints, "iScoutVillagePoints");
	pXML->GetChildXmlValByName(&m_iGoodyPoints, "iGoodyPoints");
	pXML->GetChildXmlValByName(&m_iExperiencePoints, "iExperiencePoints");
	pXML->GetChildXmlValByName(&m_iConquerCityPoints, "iConquerCityPoints");
	pXML->GetChildXmlValByName(&m_iRazeCityPoints, "iRazeCityPoints");
	pXML->GetChildXmlValByName(&m_iMissionaryPoints, "iMissionaryPoints");
	pXML->GetChildXmlValByName(&m_iProductionConversionPoints, "iProductionConversionPoints");
	pXML->GetChildXmlValByName(&m_iEuropeTradeGoldPointPercent, "iEuropeTradeGoldPointPercent");
	pXML->GetChildXmlValByName(&m_iNativeTradeGoldPointPercent, "iNativeTradeGoldPointPercent");

	pXML->SetVariableListTagPair(&m_aiBuildingPoints, "BuildingPoints", GC.getNumBuildingClassInfos(), 0);
	pXML->SetVariableListTagPair(&m_aiYieldPoints, "YieldPoints", NUM_YIELD_TYPES, 0);

	return true;
}

CvAlarmInfo::CvAlarmInfo() :
	m_iRange(0),
	m_iColony(0),
	m_iNumColonies(0),
	m_iPopulation(0),
	m_iUnit(0),
	m_iMissionary(0),
	m_iAttitudeDivisor(0)
{

}

CvAlarmInfo::~CvAlarmInfo()
{

}

int CvAlarmInfo::getRange() const
{
	return m_iRange;
}

int CvAlarmInfo::getColony() const
{
	return m_iColony;
}

int CvAlarmInfo::getNumColonies() const
{
	return m_iNumColonies;
}

int CvAlarmInfo::getPopulation() const
{
	return m_iPopulation;
}

int CvAlarmInfo::getUnit() const
{
	return m_iUnit;
}

int CvAlarmInfo::getMissionary() const
{
	return m_iMissionary;
}

int CvAlarmInfo::getAttitudeDivisor() const
{
	return m_iAttitudeDivisor;
}

bool CvAlarmInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(&m_iRange, "iRange");
	pXML->GetChildXmlValByName(&m_iColony, "iColony");
	pXML->GetChildXmlValByName(&m_iNumColonies, "iNumColonies");
	pXML->GetChildXmlValByName(&m_iPopulation, "iPopulation");
	pXML->GetChildXmlValByName(&m_iUnit, "iUnit");
	pXML->GetChildXmlValByName(&m_iMissionary, "iMissionary");
	pXML->GetChildXmlValByName(&m_iAttitudeDivisor, "iAttitudeDivisor");

	return true;
}

//Androrc UnitArtStyles
//////////////////////////////////////////////////////////////////////////
//
//	CvUnitArtStyleTypeInfo
//  Graphic Flavor Units packages by Impaler[WrG]
//

CvUnitArtStyleTypeInfo::CvUnitArtStyleTypeInfo()
{
}

CvUnitArtStyleTypeInfo::~CvUnitArtStyleTypeInfo()
{
}

const char* CvUnitArtStyleTypeInfo::getArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, int /*ProfessionType*/ m) const
{
	FAssertMsg(i < GC.getUnitInfo((UnitTypes) j).getGroupDefinitions((ProfessionTypes) m), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < GC.getNumUnitInfos(), "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");

	for (ArtDefineArray::const_iterator it = m_azArtDefineTags.begin(); it != m_azArtDefineTags.end(); ++it)
	{
		if ((*it).iMeshIndex == i && (*it).iUnitType == j && (*it).iProfessionType == m)
		{
			return (*it).szTag;
		}
	}

	return NULL;
}

void CvUnitArtStyleTypeInfo::setArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, int /*ProfessionType*/ m, const char* szVal)
{
	FAssertMsg(i < GC.getUnitInfo((UnitTypes) j).getGroupDefinitions((ProfessionTypes) m), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < GC.getNumUnitInfos(), "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");

	for (ArtDefineArray::iterator it = m_azArtDefineTags.begin(); it != m_azArtDefineTags.end(); ++it)
	{
		if ((*it).iMeshIndex == i && (*it).iUnitType == j && (*it).iProfessionType == m)
		{
			(*it).szTag = szVal;
			return;
		}
	}

	ArtDefneTag kTag;
	kTag.iMeshIndex = i;
	kTag.iUnitType = j;
	kTag.iProfessionType = m;
	kTag.szTag = szVal;
	m_azArtDefineTags.push_back(kTag);
}

bool CvUnitArtStyleTypeInfo::read(CvXMLLoadUtility* pXML)
{
	int j, i;
	int iNumSibs;
	int iIndex; // UnitIndex
	int iMesh;  // Mesh Index
	int iProfession;  // Profession Index

	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"StyleUnits"))
	{
		if (pXML->SkipToNextVal())
		{
			iNumSibs = gDLL->getXMLIFace()->GetNumChildren(pXML->GetXML());
			if (gDLL->getXMLIFace()->SetToChild(pXML->GetXML()))
			{
				if (0 < iNumSibs)
				{
					for (i = 0; i < iNumSibs; i++)
					{
						pXML->GetChildXmlValByName(szTextVal, "UnitType");
						iIndex = pXML->FindInInfoClass(szTextVal);

						pXML->GetChildXmlValByName(szTextVal, "ProfessionType");
						iProfession = pXML->FindInInfoClass(szTextVal);

						if (iIndex > -1 || iProfession > -1)
						{
							if (iIndex > -1)
							{
								iMesh = GC.getUnitInfo((UnitTypes) iIndex).getGroupDefinitions((ProfessionTypes) iProfession);

								if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"UnitMeshGroup"))
								{
									for (j = 0; j < iMesh; j++)
									{
										// Overwrite with the Style Art
										pXML->GetChildXmlValByName(szTextVal, "ArtDefineTag");
										setArtDefineTag(j, iIndex, iProfession, szTextVal);

										if (!gDLL->getXMLIFace()->NextSibling(pXML->GetXML()))
										{
											break;
										}
									}
									gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
								}
							}
							else
							{
								for (int iUnit = 0; iUnit < GC.getNumUnitInfos(); ++iUnit)
								{
									iIndex = iUnit;

									iMesh = GC.getUnitInfo((UnitTypes) iIndex).getGroupDefinitions((ProfessionTypes) iProfession);

									if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"UnitMeshGroup"))
									{
										for (j = 0; j < iMesh; j++)
										{
											//don't overwrite if already has art set for this profession for this unit type
											const char* pcTag = getArtDefineTag(j, iIndex, iProfession);
											if (NULL == pcTag)
											{
												// Overwrite with the Style Art
												pXML->GetChildXmlValByName(szTextVal, "ArtDefineTag");
												setArtDefineTag(j, iIndex, iProfession, szTextVal);
											}

											if (!gDLL->getXMLIFace()->NextSibling(pXML->GetXML()))
											{
												break;
											}
										}
										gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
									}
								}
							}
						}

						if (!gDLL->getXMLIFace()->NextSibling(pXML->GetXML()))
						{
							break;
						}
					}
				}
				gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
			}
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}

	return true;
}
//Androrc End

// PatchMod: Berth size START
int CvUnitInfo::getBerthSize() const
{
	return m_iBerthSize;
}
// PatchMod: Berth size END

// WTP, ray, new Harbour System - START
int CvUnitInfo::getHarbourSpaceNeeded() const
{
	return m_iHarbourSpaceNeeded;
}
// WTP, ray, new Harbour System - END

// WTP, ray, new Barracks System - START
int CvUnitInfo::getBarracksSpaceNeeded() const
{
	return m_iBarracksSpaceNeeded;
}
// WTP, ray, new Harbour System - END

// PatchMod: Achievements START
CvAchieveInfo::CvAchieveInfo() :
	m_bActive(false),
	m_bLandDiscovered(false),
	m_bAfterMove(false),
	m_iLowTurnNumber(0),
	m_iHighTurnNumber(0),
	m_iPictureX(0),
	m_iPictureY(0),
	m_iNumColonies(0),
	m_iNumEuroContacts(0),
	m_iNumNativeContacts(0),
	m_iNumGoodsTraded(0),
	m_iNumCombatsWonNeeded(0),
	m_iNumSeaCombatsWonNeeded(0), // R&R, ray, Achievments for Sea Combats
	m_bDiscoverEast(false),
	m_bDiscoverWest(false),
	m_aiNumBuildings(NULL),
	m_aiNumUnits(NULL),
	m_aiNumProfessions(NULL),
	m_aiYieldProduced(NULL)
{
}

CvAchieveInfo::~CvAchieveInfo()
{
//	SAFE_DELETE_ARRAY(m_aiNumBuildings);
//	SAFE_DELETE_ARRAY(m_aiNumUnits);
//	SAFE_DELETE_ARRAY(m_aiNumProfessions);
//	SAFE_DELETE_ARRAY(m_aiYieldProduced);
}

bool CvAchieveInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(m_szDescriptionKey, "Description");
	pXML->GetChildXmlValByName(&m_bActive, "bActive");
	pXML->GetChildXmlValByName(&m_bAfterMove, "bAfterMove");
	pXML->GetChildXmlValByName(m_szHeaderTextKey, "HeaderTextKey");
	pXML->GetChildXmlValByName(m_szBodyTextKey, "BodyTextKey");
	pXML->GetChildXmlValByName(m_szPictureFileName, "PictureFileName");
	pXML->GetChildXmlValByName(&m_iPictureX, "iPictureX");
	pXML->GetChildXmlValByName(&m_iPictureY, "iPictureY");
	pXML->GetChildXmlValByName(&m_iLowTurnNumber, "iLowTurnNumber");
	pXML->GetChildXmlValByName(&m_iHighTurnNumber, "iHighTurnNumber");
	pXML->GetChildXmlValByName(&m_bLandDiscovered, "bLandDiscovered");
	pXML->GetChildXmlValByName(&m_bDiscoverEast, "bDiscoverEast");
	pXML->GetChildXmlValByName(&m_bDiscoverWest, "bDiscoverWest");
	pXML->GetChildXmlValByName(&m_iNumColonies, "iNumColonies");
	pXML->GetChildXmlValByName(&m_iNumEuroContacts, "iNumEuroContacts");
	pXML->GetChildXmlValByName(&m_iNumNativeContacts, "iNumNativeContacts");
	pXML->GetChildXmlValByName(&m_iNumGoodsTraded, "iNumGoodsTraded");
	pXML->GetChildXmlValByName(&m_iNumCombatsWonNeeded, "iNumCombatsWonNeeded");
	pXML->GetChildXmlValByName(&m_iNumSeaCombatsWonNeeded, "iNumSeaCombatsWonNeeded"); // R&R, ray, Achievments for Sea Combats
	pXML->SetVariableListTagPair(&m_aiNumBuildings, "NumBuildings", GC.getNumBuildingInfos(), 0);
	pXML->SetVariableListTagPair(&m_aiNumUnits, "NumUnits", GC.getNumUnitInfos(), 0);
	pXML->SetVariableListTagPair(&m_aiNumProfessions, "NumProfessions", GC.getNumProfessionInfos(), 0);
	pXML->SetVariableListTagPair(&m_aiYieldProduced, "YieldProduced", NUM_YIELD_TYPES, 0);
	return true;
}

bool CvAchieveInfo::isActive()
{
	return m_bActive;
}

int CvAchieveInfo::getLowTurnNumber()
{
	return m_iLowTurnNumber;
}

int CvAchieveInfo::getHighTurnNumber()
{
	return m_iHighTurnNumber;
}

bool CvAchieveInfo::isLandDiscovered()
{
	return m_bLandDiscovered;
}

const wchar* CvAchieveInfo::getDescription() const
{
	return m_szDescriptionKey;
}

const wchar* CvAchieveInfo::getHeaderTextKey() const
{
	return m_szHeaderTextKey;
}

const wchar* CvAchieveInfo::getBodyTextKey() const
{
	return m_szBodyTextKey;
}

const char* CvAchieveInfo::getPictureFileName() const
{
	return m_szPictureFileName;
}

bool CvAchieveInfo::isAfterMove()
{
	return m_bAfterMove;
}

int CvAchieveInfo::getPictureX()
{
	return m_iPictureX;
}

int CvAchieveInfo::getPictureY()
{
	return m_iPictureY;
}

bool CvAchieveInfo::isDiscoverEast()
{
	return m_bDiscoverEast;
}

bool CvAchieveInfo::isDiscoverWest()
{
	return m_bDiscoverWest;
}

int CvAchieveInfo::getNumColonies()
{
	return m_iNumColonies;
}

int CvAchieveInfo::getNumEuroContacts()
{
	return m_iNumEuroContacts;
}

int CvAchieveInfo::getNumNativeContacts()
{
	return m_iNumNativeContacts;
}

int CvAchieveInfo::getNumBuildings(int iBuilding) const
{
	return m_aiNumBuildings ? m_aiNumBuildings[iBuilding] : -1;
}

int CvAchieveInfo::getNumUnits(int iUnit) const
{
	return m_aiNumUnits ? m_aiNumUnits[iUnit] : -1;
}

int CvAchieveInfo::getNumProfessions(int iProfession) const
{
	return m_aiNumProfessions ? m_aiNumProfessions[iProfession] : -1;
}

int CvAchieveInfo::getYieldProduced(int iYield) const
{
	return m_aiYieldProduced ? m_aiYieldProduced[iYield] : -1;
}

int CvAchieveInfo::getNumGoodsTraded()
{
	return m_iNumGoodsTraded;
}

int CvAchieveInfo::getNumCombatsWonNeeded()
{
	return m_iNumCombatsWonNeeded;
}

int CvAchieveInfo::getNumSeaCombatsWonNeeded() // R&R, ray, Achievments for Sea Combats
{
	return m_iNumSeaCombatsWonNeeded;
}
// PatchMod: Achievements END

// PatchMod: Victorys START
int CvVictoryInfo::getTotalProductionRate() const
{
	return m_iTotalProductionRate;
}

void CvVictoryInfo::setTotalProductionRate(int iValue)
{
	m_iTotalProductionRate = iValue;
}
// PatchMod: Victorys END

// TAC - AI More Immigrants - koma13 - START
int CvHandicapInfo::getAIImmigration() const
{
	return m_iAIImmigration;
}

int CvHandicapInfo::getAIMaxTaxrate() const
{
	return m_iAIMaxTaxrate;
}
// TAC - AI More Immigrants - koma13 - END

int CvHandicapInfo::getOppressometerGrowthHandicap() const
{
    return m_iOppressometerGrowthHandicap;
}

// trade screen type - start - Nightinggale
CvTradeScreenInfo::CvTradeScreenInfo()
{
}
CvTradeScreenInfo::~CvTradeScreenInfo()
{
}

bool CvTradeScreenInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	return true;
}
// trade screen type - end - Nightinggale


///
/// CivEffect
///
int CivEffectInfo::getLearningByDoingModifier() const
{
	return m_iLearningByDoingModifier;
}

CivEffectInfo::CivEffectInfo(bool bAutogenerateAllow)
	// allow
	: m_iAllowFoundCity(0)
	// city
	, m_iCanUseDomesticMarket(0)

	// growth
	, m_iNumUnitsOnDockChange(0)

	// unit
	, m_iLearningByDoingModifier(0)
{
	if (bAutogenerateAllow)
	{
		// Make a CivEffect, which is applied to all players at the start/load

		// the goal is to set the allow array start values in CvPlayer as follows:
		// If a CivEffect provides a positive number: 0
		// If no CivEffect can unlock the item: 1

		// This will make anything, which isn't unlockable appear at game start
		// Since the array all have 1 as default, the task for this CivEffect is to provide -1
		//   whenever there is a positive value in a CivEffect

		EnumMap<BonusTypes          , int> Bonuses;
		EnumMap<BuildTypes          , int> Builds;
		EnumMap<BuildingClassTypes  , int> Buildings;
		EnumMap<CivicTypes          , int> Civics;
		EnumMap<UnitClassTypes      , int> Immigrants;
		EnumMap<ImprovementTypes    , int> Improvements;
		EnumMap<ProfessionTypes     , int> Professions;
		EnumMap<PromotionTypes      , int> Promotions;
		EnumMap<RouteTypes          , int> Routes;
		EnumMap<UnitClassTypes      , int> Units;
		EnumMap<YieldTypes          , int> Yields;

		Bonuses             .setAll(1);
		Builds              .setAll(1);
		Buildings           .setAll(1);
		Civics              .setAll(1);
		Immigrants          .setAll(1);
		Improvements        .setAll(1);
		Professions         .setAll(1);
		Promotions          .setAll(1);
		Routes              .setAll(1);
		Units               .setAll(1);
		Yields              .setAll(1);

		for (CivEffectTypes eCivEffect = FIRST_CIV_EFFECT; eCivEffect < NUM_CIV_EFFECT_TYPES; ++eCivEffect)
		{
			const CivEffectInfo& kInfo = GC.getCivEffectInfo(eCivEffect);
			Bonuses         .generateInitCivEffect(kInfo.getAllowedBonuses());
			Builds          .generateInitCivEffect(kInfo.getAllowedBuilds());
			Buildings       .generateInitCivEffect(kInfo.getAllowedBuildingClasses());
			Civics          .generateInitCivEffect(kInfo.getAllowedCivics());
			Immigrants      .generateInitCivEffect(kInfo.getAllowedImmigrants());
			Improvements    .generateInitCivEffect(kInfo.getAllowedImprovements());
			Professions     .generateInitCivEffect(kInfo.getAllowedProfessions());
			Promotions      .generateInitCivEffect(kInfo.getAllowedPromotions());
			Routes          .generateInitCivEffect(kInfo.getAllowedRoutes());
			Units           .generateInitCivEffect(kInfo.getAllowedUnitClasses());
			Yields          .generateInitCivEffect(kInfo.getAllowedYields());
		}

		m_info_AllowBonuses         .assignFrom(Bonuses);
		m_info_AllowBuilds          .assignFrom(Builds);
		m_info_AllowBuildings       .assignFrom(Buildings);
		m_info_AllowCivics          .assignFrom(Civics);
		m_info_AllowImmigrants      .assignFrom(Immigrants);
		m_info_AllowImprovements    .assignFrom(Improvements);
		m_info_AllowProfessions     .assignFrom(Professions);
		m_info_AllowPromotions      .assignFrom(Promotions);
		m_info_AllowRoutes          .assignFrom(Routes);
		m_info_AllowUnits           .assignFrom(Units);
		m_info_AllowYields          .assignFrom(Yields);
	}
}

CivEffectInfo::~CivEffectInfo()
{
}

bool CivEffectInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "TagGroupAllow"))
	{
		m_info_AllowBonuses                .read(pXML, getType(), "AllowBonuses"                );
		m_info_AllowBuilds                 .read(pXML, getType(), "AllowBuilds"                 );
		m_info_AllowBuildings              .read(pXML, getType(), "AllowBuildingClasses"        );
		m_info_AllowCivics                 .read(pXML, getType(), "AllowCivics"                 );
		m_info_AllowConqueringCity         .read(pXML, getType(), "AllowConqueringCities"       );
		m_info_AllowImmigrants             .read(pXML, getType(), "AllowImmigrants"             );
		m_info_AllowImprovements           .read(pXML, getType(), "AllowImprovements"           );
		m_info_AllowProfessions            .read(pXML, getType(), "AllowProfessions"            );
		m_info_AllowPromotions             .read(pXML, getType(), "AllowPromotions"             );
		m_info_AllowRoutes                 .read(pXML, getType(), "AllowRoutes"                 );
		m_info_AllowUnits                  .read(pXML, getType(), "AllowUnitClasses"            );
		m_info_AllowYields                 .read(pXML, getType(), "AllowYields"                 );

		pXML->GetChildXmlValByName(&m_iAllowFoundCity, "iAllowFoundCity");

		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "TagGroupCity"))
	{
		pXML->GetChildXmlValByName(&m_iCanUseDomesticMarket, "iCanUseDomesticMarket");

		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "TagGroupGrowth"))
	{
		pXML->GetChildXmlValByName(&m_iLearningByDoingModifier , "iLearningByDoingModifier"     );
		pXML->GetChildXmlValByName(&m_iNumUnitsOnDockChange    , "iNumUnitsOnDockChange"        );

		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "TagGroupUnit"))
	{
		m_info_FreePromotions              .read(pXML, getType(), "FreePromotions"              );
		m_info_FreePromotionsForProfessions.read(pXML, getType(), "FreePromotionsForProfessions");
		m_info_FreePromotionsForUnitCombats.read(pXML, getType(), "FreePromotionsForUnitCombats");

		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}

	return true;
}
