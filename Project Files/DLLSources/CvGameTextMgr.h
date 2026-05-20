#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvGameTextMgr.h
//
//  AUTHOR:  Jesse Smith  --  10/2004
//
//  PURPOSE: Group of functions to manage CIV Game Text
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2004 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef CIV4_GAME_TEXT_MGR_H
#define CIV4_GAME_TEXT_MGR_H

#include "CvInfos.h"
//#include "CvEnums.h"

#pragma warning( disable: 4251 )	// needs to have dll-interface to be used by clients of class

class CvCity;
class CvDeal;
class CvPopupInfo;
class CvPlayer;

//
// Class:		CvGameTextMgr
// Purpose:		Manages Game Text...
class CvGameTextMgr
{
	friend class CvGlobals;
public:
	// singleton accessor
	DllExport static CvGameTextMgr& GetInstance();

	DllExport CvGameTextMgr();
	DllExport virtual ~CvGameTextMgr();

	DllExport void Initialize();
	DllExport void DeInitialize();
	DllExport void Reset();

	DllExport int getCurrentLanguage();

	DllExport void setTimeStr(CvWString& szString, int iGameTurn, bool bSave);
	DllExport void setYearStr(CvWString& szString, int iGameTurn, bool bSave, CalendarTypes eCalendar, int iStartYear, GameSpeedTypes eSpeed);
	DllExport void setDateStr(CvWString& szString, int iGameTurn, bool bSave, CalendarTypes eCalendar, int iStartYear, GameSpeedTypes eSpeed);
	DllExport void setInterfaceTime(CvWString& szString, PlayerTypes ePlayer);
	DllExport void setGoldStr(CvWString& szString, PlayerTypes ePlayer);
	DllExport void setOOSSeeds(CvWString& szString, PlayerTypes ePlayer);
	DllExport void setNetStats(CvWString& szString, PlayerTypes ePlayer);
	DllExport void setMinimizePopupHelp(CvWString& szString, const CvPopupInfo & info);

	DllExport void setUnitHelp(CvWStringBuffer &szString, const CvUnit* pUnit, bool bOneLine = false, bool bShort = false);
	DllExport void setUnitPromotionHelp(CvWStringBuffer &szString, const CvUnit* pUnit);
	DllExport void setPlotListHelp(CvWStringBuffer &szString, const CvPlot* pPlot, bool bOneLine, bool bShort);
	DllExport bool setCombatPlotHelp(CvWStringBuffer &szString, CvPlot* pPlot);
	DllExport void setPlotHelp(CvWStringBuffer &szString, CvPlot* pPlot);
	void setCityPlotHelp(CvWStringBuffer &szString, CvPlot* pPlot); // city plot mouse over help - inaiwae
	DllExport void setCityBarHelp(CvWStringBuffer &szString, CvCity* pCity);
	DllExport void setScoreHelp(CvWStringBuffer &szString, PlayerTypes ePlayer);
	DllExport void setCitizenHelp(CvWStringBuffer &szString, const CvCity& kCity, const CvUnit& kUnit);
	DllExport void setEuropeYieldSoldHelp(CvWStringBuffer &szString, const CvPlayer& kPlayer, YieldTypes eYield, int iAmount, int iCommission);
	DllExport void setEuropeYieldBoughtHelp(CvWStringBuffer &szString, const CvPlayer& kPlayer, YieldTypes eYield, int iAmount);
	DllExport void parseTraits(CvWStringBuffer &szHelpString, TraitTypes eTrait, CivilizationTypes eCivilization = NO_CIVILIZATION, bool bDawnOfMan = false, bool bIndent = true);
	DllExport void parseLeaderTraits(CvWStringBuffer &szInfoText, LeaderHeadTypes eLeader = NO_LEADER, CivilizationTypes eCivilization = NO_CIVILIZATION, bool bDawnOfMan = false, bool bCivilopediaText = false);
	DllExport void parseLeaderShortTraits(CvWStringBuffer &szInfoText, LeaderHeadTypes eLeader);
	DllExport void parseCivShortTraits(CvWStringBuffer &szInfoText, CivilizationTypes eCiv);
	DllExport void parseCivInfos(CvWStringBuffer &szHelpString, CivilizationTypes eCivilization, bool bDawnOfMan = false, bool bLinks = true);
	DllExport void parsePromotionHelp(CvWStringBuffer &szBuffer, PromotionTypes ePromotion, const wchar* pcNewline = NEWLINE);
	DllExport void parseCivicInfo(CvWStringBuffer &szBuffer, CivicTypes eCivic, bool bCivilopediaText = false, bool bPlayerContext = false, bool bSkipName = false);
	DllExport void parsePlayerTraits(CvWStringBuffer &szBuffer, PlayerTypes ePlayer);
	DllExport void parseLeaderHeadHelp(CvWStringBuffer &szBuffer, PlayerTypes eThisPlayer, PlayerTypes eOtherPlayer);
	DllExport void parseLeaderLineHelp(CvWStringBuffer &szBuffer, PlayerTypes eThisPlayer, PlayerTypes eOtherPlayer);
	DllExport void parseGreatGeneralHelp(CvWStringBuffer &szBuffer, CvPlayer& kPlayer);
	// CivEffects - Nightinggale - start
	void parseCivEffects(CvWStringBuffer &szHelpString, CivEffectTypes eCivEffect, CivilizationTypes eCivilization = NO_CIVILIZATION, bool bDawnOfMan = false, bool bIndent = true);
	// CivEffects - Nightinggale - end
	DllExport void setBasicUnitHelp(CvWStringBuffer &szBuffer, UnitTypes eUnit, bool bCivilopediaText = false);
	DllExport void setUnitHelp(CvWStringBuffer &szBuffer, UnitTypes eUnit, bool bCivilopediaText = false, bool bStrategyText = false, CvCity* pCity = NULL);
	DllExport void setProfessionHelp(CvWStringBuffer &szBuffer, ProfessionTypes eProfession, bool bCivilopediaText = false, bool bStrategyText = false);
	DllExport void setBuildingHelp(CvWStringBuffer &szBuffer, BuildingTypes eBuilding, bool bCivilopediaText = false, bool bStrategyText = false, CvCity* pCity = NULL);
	DllExport void setFatherPointHelp(CvWStringBuffer &szBuffer, FatherPointTypes eFatherPointType);
	DllExport void setYieldChangeHelp(CvWStringBuffer &szBuffer, const CvWString& szStart, const CvWString& szSpace, const CvWString& szEnd, const int* piYieldChange, bool bPercent = false, bool bNewLine = true);
	DllExport void setBonusHelp(CvWStringBuffer &szBuffer, BonusTypes eBonus, bool bCivilopediaText = false);
	DllExport void setPromotionHelp(CvWStringBuffer &szBuffer, PromotionTypes ePromotion, bool bCivilopediaText = false);
	DllExport void setImprovementHelp(CvWStringBuffer &szBuffer, ImprovementTypes eImprovement, bool bCivilopediaText = false);
	DllExport void setTerrainHelp(CvWStringBuffer &szBuffer, TerrainTypes eTerrain, bool bCivilopediaText = false);
	DllExport void setYieldsHelp(CvWStringBuffer &szBuffer, YieldTypes eYield, bool bCivilopediaText = false);
	DllExport void setFeatureHelp(CvWStringBuffer &szBuffer, FeatureTypes eFeature, bool bCivilopediaText = false);
	DllExport void setProductionHelp(CvWStringBuffer &szBuffer, CvCity& city);
	DllExport void setYieldPriceHelp(CvWStringBuffer &szBuffer, PlayerTypes ePlayer, YieldTypes eYield);
	DllExport void setYieldHelp(CvWStringBuffer &szBuffer, CvCity& city, YieldTypes eYieldType);
	DllExport void setEventHelp(CvWStringBuffer& szBuffer, EventTypes eEvent, int iEventTriggeredId, PlayerTypes ePlayer);
	DllExport void setFatherHelp(CvWStringBuffer &szBuffer, FatherTypes eFather, bool bCivilopediaText);
	DllExport void buildHintsList(CvWStringBuffer& szBuffer);
	DllExport void buildBuildingRequiresString(CvWStringBuffer& szBuffer, BuildingTypes eBuilding, bool bCivilopediaText, const CvCity* pCity);
	DllExport void buildCityBillboardIconString( CvWStringBuffer& szBuffer, CvCity* pCity);
	DllExport void buildCityBillboardCityNameString( CvWStringBuffer& szBuffer, CvCity* pCity);
	DllExport void buildCityBillboardProductionString( CvWStringBuffer& szBuffer, CvCity* pCity);
	DllExport void buildCityBillboardCitySizeString( CvWStringBuffer& szBuffer, CvCity* pCity, const NiColorA& kColor);
	void getWarplanString(CvWStringBuffer& szString, WarPlanTypes eWarPlan);
	DllExport void getAttitudeString(CvWStringBuffer& szBuffer, PlayerTypes ePlayer, PlayerTypes eTargetPlayer);
	DllExport void getTradeString(CvWStringBuffer& szBuffer, const TradeData& tradeData, PlayerTypes ePlayer1, PlayerTypes ePlayer2);
	DllExport void getDealString(CvWStringBuffer& szString, CvDeal& deal, PlayerTypes ePlayerPerspective = NO_PLAYER);
	void getDealString(CvWStringBuffer& szBuffer, PlayerTypes ePlayer1, PlayerTypes ePlayer2, const CLinkList<TradeData>* pListPlayer1, const CLinkList<TradeData>* pListPlayer2, PlayerTypes ePlayerPerspective = NO_PLAYER);
	DllExport void getActiveDealsString(CvWStringBuffer& szString, PlayerTypes eThisPlayer, PlayerTypes eOtherPlayer);
	DllExport void getTradeScreenTitleIcon(CvString& szButton, CvWidgetDataStruct& widgetData, PlayerTypes ePlayer);
	DllExport void getTradeScreenIcons(std::vector< std::pair<CvString, CvWidgetDataStruct> >& aIconInfos, PlayerTypes ePlayer);
	DllExport void getTradeScreenHeader(CvWString& szHeader, PlayerTypes ePlayer, PlayerTypes eOtherPlayer, bool bAttitude, CvCity* pCity);

	DllExport void setResourceLayerInfo(ResourceLayerOptions eOption, CvWString& szName, CvString& szButton);
	DllExport void setUnitLayerInfo(UnitLayerOptionTypes eOption, CvWString& szName, CvString& szButton);

	// WTP, ray, fixing wrong Trade Log in Port Royal
	void setAfricaYieldSoldHelp(CvWStringBuffer &szString, const CvPlayer& kPlayer, YieldTypes eYield, int iAmount, int iCommission);
	void setPortRoyalYieldSoldHelp(CvWStringBuffer &szString, const CvPlayer& kPlayer, YieldTypes eYield, int iAmount, int iCommission);

private:
	void eventGoldHelp(CvWStringBuffer& szBuffer, EventTypes eEvent, PlayerTypes ePlayer, PlayerTypes eOtherPlayer);

	std::vector<int*> m_apbPromotion;

	void setCityPlotYieldValueString(CvWStringBuffer& szString, CvCity* pCity, int iIndex, bool bAvoidGrowth, bool bIgnoreGrowth, bool bIgnoreFood = false);
	void setYieldValueString(CvWStringBuffer& szString, int iValue, bool bActive = false, bool bMakeWhitespace = false);
	int setCityYieldModifierString(CvWStringBuffer& szBuffer, YieldTypes eYieldType, const CvCity& kCity);
};

// Singleton Accessor
#define GAMETEXT CvGameTextMgr::GetInstance()

#endif
