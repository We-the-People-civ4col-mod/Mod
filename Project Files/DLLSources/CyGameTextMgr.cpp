//
// Python wrapper class for CyGameTextMgr
//
#include "CvGameCoreDLL.h"
#include "CvGameTextMgr.h"
#include "CyGameTextMgr.h"
#include "CyCity.h"
#include "CyDeal.h"
#include "CyUnit.h"
CyGameTextMgr::CyGameTextMgr() :
m_pGameTextMgr(NULL)
{
	m_pGameTextMgr = &CvGameTextMgr::GetInstance();
}
CyGameTextMgr::CyGameTextMgr(CvGameTextMgr* pGameTextMgr) :
m_pGameTextMgr(pGameTextMgr)
{}
void CyGameTextMgr::Reset()
{
	GAMETEXT.Reset();
}
std::wstring CyGameTextMgr::getTimeStr(int iGameTurn, bool bSave)
{
	CvWString str;
	GAMETEXT.setTimeStr(str, iGameTurn, bSave);
	return str;
}
std::wstring CyGameTextMgr::getDateStr(int iGameTurn, bool bSave, int /*CalendarTypes*/ eCalendar, int iStartYear, int /*GameSpeedTypes*/ eSpeed)
{
	CvWString str;
	GAMETEXT.setDateStr(str, iGameTurn, bSave, (CalendarTypes)eCalendar, iStartYear, (GameSpeedTypes)eSpeed);
	return str;
}
std::wstring CyGameTextMgr::getInterfaceTimeStr(int /*PlayerTypes*/ iPlayer)
{
	CvWString szBuffer;
	GAMETEXT.setInterfaceTime(szBuffer, ((PlayerTypes)iPlayer));
	return szBuffer;
}
std::wstring CyGameTextMgr::getGoldStr(int /*PlayerTypes*/ iPlayer)
{
	CvWString szBuffer;
	GAMETEXT.setGoldStr(szBuffer, ((PlayerTypes)iPlayer));
	return szBuffer;
}
std::wstring CyGameTextMgr::getOOSSeeds(int /*PlayerTypes*/ iPlayer)
{
	CvWString szBuffer;
	GAMETEXT.setOOSSeeds(szBuffer, ((PlayerTypes)iPlayer));
	return szBuffer;
}
std::wstring CyGameTextMgr::getNetStats(int /*PlayerTypes*/ iPlayer)
{
	CvWString szBuffer;
	GAMETEXT.setNetStats(szBuffer, ((PlayerTypes)iPlayer));
	return szBuffer;
}
std::wstring CyGameTextMgr::getUnitHelp(int iUnit, bool bCivilopediaText, bool bStrategyText, CyCity* pCity)
{
	CvWStringBuffer szBuffer;
	GAMETEXT.setUnitHelp(szBuffer, (UnitTypes)iUnit, bCivilopediaText, bStrategyText, ((pCity != NULL) ? pCity->getCity() : NULL));
	return szBuffer.getCString();
}
std::wstring CyGameTextMgr::getSpecificUnitHelp(CyUnit* pUnit, bool bOneLine, bool bShort)
{
	CvWStringBuffer szBuffer;
	if (pUnit && pUnit->getUnit())
	{
		GAMETEXT.setUnitHelp(szBuffer, pUnit->getUnit(), bOneLine, bShort);
	}
	return szBuffer.getCString();
}
std::wstring CyGameTextMgr::getBuildingHelp(int iBuilding, bool bCivilopediaText, bool bStrategyText, CyCity* pCity)
{
	CvWStringBuffer szBuffer;
	GAMETEXT.setBuildingHelp(szBuffer, (BuildingTypes)iBuilding, bCivilopediaText, bStrategyText, ((pCity != NULL) ? pCity->getCity() : NULL));
	return szBuffer.getCString();
}
std::wstring CyGameTextMgr::getPromotionHelp(int iPromotion, bool bCivilopediaText)
{
	CvWStringBuffer szBuffer;
	GAMETEXT.setPromotionHelp(szBuffer, (PromotionTypes)iPromotion, bCivilopediaText);
	return szBuffer.getCString();
}
std::wstring CyGameTextMgr::getProfessionHelp(int iProfession, bool bCivilopediaText)
{
	CvWStringBuffer szBuffer;
	GAMETEXT.setProfessionHelp(szBuffer, (ProfessionTypes)iProfession, bCivilopediaText);
	return szBuffer.getCString();
}
std::wstring CyGameTextMgr::getBonusHelp(int iBonus, bool bCivilopediaText)
{
	CvWStringBuffer szBuffer;
	GAMETEXT.setBonusHelp(szBuffer, (BonusTypes)iBonus, bCivilopediaText);
	return szBuffer.getCString();
}
std::wstring CyGameTextMgr::getImprovementHelp(int iImprovement, bool bCivilopediaText)
{
	CvWStringBuffer szBuffer;
	GAMETEXT.setImprovementHelp(szBuffer, (ImprovementTypes)iImprovement, bCivilopediaText);
	return szBuffer.getCString();
}
std::wstring CyGameTextMgr::getFeatureHelp(int iFeature, bool bCivilopediaText)
{
	CvWStringBuffer szBuffer;
	GAMETEXT.setFeatureHelp(szBuffer, (FeatureTypes)iFeature, bCivilopediaText);
	return szBuffer.getCString();
}
std::wstring CyGameTextMgr::getTerrainHelp(int iTerrain, bool bCivilopediaText)
{
	CvWStringBuffer szBuffer;
	GAMETEXT.setTerrainHelp(szBuffer, (TerrainTypes)iTerrain, bCivilopediaText);
	return szBuffer.getCString();
}
std::wstring CyGameTextMgr::getYieldsHelp(int iYield, bool bCivilopediaText)
{
	CvWStringBuffer szBuffer;
	GAMETEXT.setYieldsHelp(szBuffer, (YieldTypes)iYield, bCivilopediaText);
	return szBuffer.getCString();
}
std::wstring CyGameTextMgr::parseCivicInfo(int /*CivicTypes*/ iCivicType, bool bCivilopediaText, bool bPlayerContext, bool bSkipName)
{
	CvWStringBuffer szBuffer;
	GAMETEXT.parseCivicInfo(szBuffer, (CivicTypes) iCivicType, bCivilopediaText, bPlayerContext, bSkipName);
	return szBuffer.getCString();
}
std::wstring CyGameTextMgr::parseCivInfos(int /*CivilizationTypes*/ iCivilization, bool bDawnOfMan)
{
	CvWStringBuffer szBuffer;
	GAMETEXT.parseCivInfos(szBuffer, (CivilizationTypes) iCivilization, bDawnOfMan);
	return szBuffer.getCString();
}
std::wstring CyGameTextMgr::getFatherHelp(int iFather, bool bCivilopediaText)
{
	CvWStringBuffer szBuffer;
	GAMETEXT.setFatherHelp(szBuffer, (FatherTypes)iFather, bCivilopediaText);
	return szBuffer.getCString();
}
std::wstring CyGameTextMgr::parseLeaderTraits(int /*LeaderHeadTypes*/ iLeader, int /*CivilizationTypes*/ iCivilization, bool bDawnOfMan, bool bCivilopediaText)
{
	CvWStringBuffer szBuffer;
	GAMETEXT.parseLeaderTraits(szBuffer, (LeaderHeadTypes)iLeader, (CivilizationTypes) iCivilization, bDawnOfMan, bCivilopediaText);
	return szBuffer.getCString();
}
std::wstring CyGameTextMgr::getTradeString(TradeData* pTradeData, int iPlayer1, int iPlayer2)
{
	CvWStringBuffer szBuffer;
	if (NULL != pTradeData)
	{
		GAMETEXT.getTradeString(szBuffer, *pTradeData, (PlayerTypes)iPlayer1, (PlayerTypes) iPlayer2);
	}
	return szBuffer.getCString();
}
std::wstring CyGameTextMgr::buildHintsList()
{
	CvWStringBuffer szBuffer;
	GAMETEXT.buildHintsList(szBuffer);
	return szBuffer.getCString();
}
std::wstring CyGameTextMgr::getAttitudeString(int iPlayer, int iTargetPlayer)
{
	CvWStringBuffer szBuffer;
	GAMETEXT.getAttitudeString(szBuffer, (PlayerTypes)iPlayer, (PlayerTypes) iTargetPlayer);
	return szBuffer.getCString();
}
std::wstring CyGameTextMgr::getActiveDealsString(int iThisPlayer, int iOtherPlayer)
{
	CvWStringBuffer szBuffer;
	GAMETEXT.getActiveDealsString(szBuffer, (PlayerTypes)iThisPlayer, (PlayerTypes)iOtherPlayer);
	return szBuffer.getCString();
}
std::wstring CyGameTextMgr::getDealString(CyDeal* pDeal, int iPlayerPerspective)
{
	CvWStringBuffer szBuffer;
	if (pDeal && pDeal->getDeal())
	{
		GAMETEXT.getDealString(szBuffer, *(pDeal->getDeal()), (PlayerTypes)iPlayerPerspective);
	}
	return szBuffer.getCString();
}
