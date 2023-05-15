//
// Python wrapper class for global vars and fxns
// Author - Mustafa Thamer
//
#include "CvGameCoreDLL.h"
#include "CyGlobalContext.h"
#include "CyGame.h"
#include "CyPlayer.h"
#include "CyMap.h"
#include "CvGlobals.h"
#include "CvPlayerAI.h"
#include "CvGameAI.h"
//#include "CvStructs.h"
#include "CvInfos.h"
#include "CyTeam.h"
#include "CvTeamAI.h"
#include "CyArtFileMgr.h"
#include "UserSettings.h"
#include "CyUserSettings.h"
#include "CvPopupInfo.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "OpenURL.h"

CyGlobalContext::CyGlobalContext()
{
}
CyGlobalContext::~CyGlobalContext()
{
}
CyGlobalContext& CyGlobalContext::getInstance()
{
	static CyGlobalContext globalContext;
	return globalContext;
}
bool CyGlobalContext::isDebugBuild() const
{
#ifdef _DEBUG
	return true;
#else
	return false;
#endif
}
CyGame* CyGlobalContext::getCyGame() const
{
	static CyGame cyGame(&GC.getGameINLINE());
	return &cyGame;
}

CyMap* CyGlobalContext::getCyMap() const
{
	static CyMap cyMap(&GC.getMap());
	return &cyMap;
}

CyPlayer* CyGlobalContext::getCyPlayer(int idx)
{
	static CyPlayer cyPlayers[MAX_PLAYERS];
	static bool bInit=false;
	if (!bInit)
	{
		int i;
		for(i=0;i<MAX_PLAYERS;i++)
			cyPlayers[i]=CyPlayer(&GET_PLAYER((PlayerTypes)i));
		bInit=true;
	}
	FAssert(idx>=0);
	FAssert(idx<MAX_PLAYERS);
	return idx < MAX_PLAYERS && idx != NO_PLAYER ? &cyPlayers[idx] : NULL;
}

CyPlayer* CyGlobalContext::getCyActivePlayer()
{
	PlayerTypes pt = GC.getGameINLINE().getActivePlayer();
	return pt != NO_PLAYER ? getCyPlayer(pt) : NULL;
}

CvRandom& CyGlobalContext::getCyASyncRand() const
{
	return GC.getASyncRand();
}
CyTeam* CyGlobalContext::getCyTeam(int i)
{
	static CyTeam cyTeams[MAX_TEAMS];
	static bool bInit=false;
	if (!bInit)
	{
		int j;
		for(j=0;j<MAX_TEAMS;j++)
		{
			cyTeams[j]=CyTeam(&GET_TEAM((TeamTypes)j));
		}
		bInit = true;
	}
	return i<MAX_TEAMS ? &cyTeams[i] : NULL;
}

CyEffectInfo* CyGlobalContext::getEffectInfo(int /*EffectTypes*/ i) const
{
	return (i>=0 && i<GC.getNumEffectInfos()) ? (CyEffectInfo*)&GC.getEffectInfo((EffectTypes) i) : NULL;
}
CyTerrainInfo* CyGlobalContext::getTerrainInfo(int /*TerrainTypes*/ i) const
{
	return (i>=0 && i<GC.getNumTerrainInfos()) ? (CyTerrainInfo*)&GC.getTerrainInfo((TerrainTypes) i) : NULL;
}
CyBonusInfo* CyGlobalContext::getBonusInfo(int /*(BonusTypes)*/ i) const
{
	return (i>=0 && i<GC.getNumBonusInfos()) ? (CyBonusInfo*)&GC.getBonusInfo((BonusTypes) i) : NULL;
}
CyFeatureInfo* CyGlobalContext::getFeatureInfo(int i) const
{
	return (i>=0 && i<GC.getNumFeatureInfos()) ? (CyFeatureInfo*)&GC.getFeatureInfo((FeatureTypes) i) : NULL;
}
CyCivilizationInfo* CyGlobalContext::getCivilizationInfo(int i) const
{
	return (i>=0 && i<GC.getNumCivilizationInfos()) ? (CyCivilizationInfo*)&GC.getCivilizationInfo((CivilizationTypes) i) : NULL;
}

CyLeaderHeadInfo* CyGlobalContext::getLeaderHeadInfo(int i) const
{
	return (i>=0 && i<GC.getNumLeaderHeadInfos()) ? (CyLeaderHeadInfo*)&GC.getLeaderHeadInfo((LeaderHeadTypes) i) : NULL;
}

CyTraitInfo* CyGlobalContext::getTraitInfo(int i) const
{
	return (i>=0 && i<GC.getNumTraitInfos()) ? (CyTraitInfo*)&GC.getTraitInfo((TraitTypes) i) : NULL;
}

CyUnitInfo* CyGlobalContext::getUnitInfo(int i) const
{
	return (i>=0 && i<GC.getNumUnitInfos()) ? (CyUnitInfo*)&GC.getUnitInfo((UnitTypes) i) : NULL;
}
CySpecialUnitInfo* CyGlobalContext::getSpecialUnitInfo(int i) const
{
	return (i>=0 && i<GC.getNumSpecialUnitInfos()) ? (CySpecialUnitInfo*)&GC.getSpecialUnitInfo((SpecialUnitTypes) i) : NULL;
}
CyYieldInfo* CyGlobalContext::getYieldInfo(int i) const
{
	return (i>=0 && i<NUM_YIELD_TYPES) ? (CyYieldInfo*)&GC.getYieldInfo((YieldTypes) i) : NULL;
}

CyRouteInfo* CyGlobalContext::getRouteInfo(int i) const
{
	return (i>=0 && i<GC.getNumRouteInfos()) ? (CyRouteInfo*)&GC.getRouteInfo((RouteTypes) i) : NULL;
}

CyImprovementInfo* CyGlobalContext::getImprovementInfo(int i) const
{
	return (i>=0 && i<GC.getNumImprovementInfos()) ? (CyImprovementInfo*)&GC.getImprovementInfo((ImprovementTypes) i) : NULL;
}

CyGoodyInfo* CyGlobalContext::getGoodyInfo(int i) const
{
	return (i>=0 && i<GC.getNumGoodyInfos()) ? (CyGoodyInfo*)&GC.getGoodyInfo((GoodyTypes) i) : NULL;
}

CyBuildInfo* CyGlobalContext::getBuildInfo(int i) const
{
	return (i>=0 && i<GC.getNumBuildInfos()) ? (CyBuildInfo*)&GC.getBuildInfo((BuildTypes) i) : NULL;
}

CyHandicapInfo* CyGlobalContext::getHandicapInfo(int i) const
{
	return (i>=0 && i<GC.getNumHandicapInfos()) ? (CyHandicapInfo*)&GC.getHandicapInfo((HandicapTypes) i) : NULL;
}

CyBuildingClassInfo* CyGlobalContext::getBuildingClassInfo(int i) const
{
	return (i>=0 && i<GC.getNumBuildingClassInfos()) ? (CyBuildingClassInfo*)&GC.getBuildingClassInfo((BuildingClassTypes) i) : NULL;
}

CyBuildingInfo* CyGlobalContext::getBuildingInfo(int i) const
{
	return (i>=0 && i<GC.getNumBuildingInfos()) ? (CyBuildingInfo*)&GC.getBuildingInfo((BuildingTypes) i) : NULL;
}
CyUnitClassInfo* CyGlobalContext::getUnitClassInfo(int i) const
{
	return (i>=0 && i<GC.getNumUnitClassInfos()) ? (CyUnitClassInfo*)&GC.getUnitClassInfo((UnitClassTypes) i) : NULL;
}

CvInfoBase* CyGlobalContext::getUnitCombatInfo(int i) const
{
	return (i>=0 && i<GC.getNumUnitCombatInfos()) ? &GC.getUnitCombatInfo((UnitCombatTypes)i) : NULL;
}

CvInfoBase* CyGlobalContext::getDomainInfo(int i) const
{
	return (i>=0 && i<NUM_DOMAIN_TYPES) ? &GC.getDomainInfo((DomainTypes)i) : NULL;
}

CyActionInfo* CyGlobalContext::getActionInfo(int i) const
{
	return (i>=0 && i<GC.getNumActionInfos()) ? (CyActionInfo*)&GC.getActionInfo(i) : NULL;
}
CyAutomateInfo* CyGlobalContext::getAutomateInfo(int i) const
{
	return (i>=0 && i<GC.getNumAutomateInfos()) ? (CyAutomateInfo*)&GC.getAutomateInfo(i) : NULL;
}
CyCommandInfo* CyGlobalContext::getCommandInfo(int i) const
{
	return (i>=0 && i<NUM_COMMAND_TYPES) ? (CyCommandInfo*)&GC.getCommandInfo((CommandTypes)i) : NULL;
}
CyControlInfo* CyGlobalContext::getControlInfo(int i) const
{
	return (i>=0 && i<NUM_CONTROL_TYPES) ? (CyControlInfo*)&GC.getControlInfo((ControlTypes)i) : NULL;
}
CyMissionInfo* CyGlobalContext::getMissionInfo(int i) const
{
	return (i>=0 && i<NUM_MISSION_TYPES) ? (CyMissionInfo*)&GC.getMissionInfo((MissionTypes) i) : NULL;
}
CyPromotionInfo* CyGlobalContext::getPromotionInfo(int i) const
{
	return (i>=0 && i<GC.getNumPromotionInfos()) ? (CyPromotionInfo*)&GC.getPromotionInfo((PromotionTypes) i) : NULL;
}
CyProfessionInfo* CyGlobalContext::getProfessionInfo(int i) const
{
	return (i>=0 && i<GC.getNumProfessionInfos()) ? (CyProfessionInfo*)&GC.getProfessionInfo((ProfessionTypes) i) : NULL;
}

CySpecialBuildingInfo* CyGlobalContext::getSpecialBuildingInfo(int i) const
{
	return (i>=0 && i<GC.getNumSpecialBuildingInfos()) ? (CySpecialBuildingInfo*)&GC.getSpecialBuildingInfo((SpecialBuildingTypes) i) : NULL;
}

CvInfoBase* CyGlobalContext::getCivicOptionInfo(int i) const
{
	return &GC.getCivicOptionInfo((CivicOptionTypes) i);
}

CyCivicInfo* CyGlobalContext::getCivicInfo(int i) const
{
	return (CyCivicInfo*)&GC.getCivicInfo((CivicTypes) i);
}
CyDiplomacyInfo* CyGlobalContext::getDiplomacyInfo(int i) const
{
	return (CyDiplomacyInfo*)&GC.getDiplomacyInfo(i);
}
CyHurryInfo* CyGlobalContext::getHurryInfo(int i) const
{
	return (i>=0 && i<GC.getNumHurryInfos()) ? (CyHurryInfo*)&GC.getHurryInfo((HurryTypes) i) : NULL;
}

CvAnimationPathInfo* CyGlobalContext::getAnimationPathInfo(int i) const
{
	return (i>=0 && i<GC.getNumAnimationPathInfos()) ? &GC.getAnimationPathInfo((AnimationPathTypes)i) : NULL;
}

CyEmphasizeInfo* CyGlobalContext::getEmphasizeInfo(int i) const
{
	return (i>=0 && i<GC.getNumEmphasizeInfos()) ? (CyEmphasizeInfo*)&GC.getEmphasizeInfo((EmphasizeTypes) i) : NULL;
}

CyCultureLevelInfo* CyGlobalContext::getCultureLevelInfo(int i) const
{
	return (i>=0 && i<GC.getNumCultureLevelInfos()) ? (CyCultureLevelInfo*)&GC.getCultureLevelInfo((CultureLevelTypes) i) : NULL;
}

CyVictoryInfo* CyGlobalContext::getVictoryInfo(int i) const
{
	return (i>=0 && i<GC.getNumVictoryInfos()) ? (CyVictoryInfo*)&GC.getVictoryInfo((VictoryTypes) i) : NULL;
}

CyEraInfo* CyGlobalContext::getEraInfo(int i) const
{
	return (i>=0 && i<GC.getNumEraInfos()) ? (CyEraInfo*)&GC.getEraInfo((EraTypes) i) : NULL;
}

CyWorldInfo* CyGlobalContext::getWorldInfo(int i) const
{
	return (i>=0 && i<GC.getNumWorldInfos()) ? (CyWorldInfo*)&GC.getWorldInfo((WorldSizeTypes) i) : NULL;
}

CyClimateInfo* CyGlobalContext::getClimateInfo(int i) const
{
	return (i>=0 && i<GC.getNumClimateInfos()) ? (CyClimateInfo*)&GC.getClimateInfo((ClimateTypes) i) : NULL;
}

CySeaLevelInfo* CyGlobalContext::getSeaLevelInfo(int i) const
{
	return (i>=0 && i<GC.getNumSeaLevelInfos()) ? (CySeaLevelInfo*)&GC.getSeaLevelInfo((SeaLevelTypes) i) : NULL;
}

CyEuropeInfo* CyGlobalContext::getEuropeInfo(int i) const
{
	return (i>=0 && i<GC.getNumEuropeInfos()) ? (CyEuropeInfo*)&GC.getEuropeInfo((EuropeTypes) i) : NULL;
}

CvInfoBase* CyGlobalContext::getUnitAIInfo(int i) const
{
	return (i>=0 && i<NUM_UNITAI_TYPES) ? &GC.getUnitAIInfo((UnitAITypes)i) : NULL;
}

CvColorInfo* CyGlobalContext::getColorInfo(int i) const
{
	return (i>=0 && i<GC.getNumColorInfos()) ? &GC.getColorInfo((ColorTypes)i) : NULL;
}

int CyGlobalContext::getInfoTypeForString(const char* szInfoType) const
{
	return GC.getInfoTypeForString(szInfoType);
}

CvPlayerColorInfo* CyGlobalContext::getPlayerColorInfo(int i) const
{
	return (i>=0 && i<GC.getNumPlayerColorInfos()) ? &GC.getPlayerColorInfo((PlayerColorTypes)i) : NULL;
}
CvInfoBase* CyGlobalContext::getHints(int i) const
{
	return ((i >= 0 && i < GC.getNumHints()) ? &GC.getHints(i) : NULL);
}
CyMainMenuInfo* CyGlobalContext::getMainMenus(int i) const
{
	return ((i >= 0 && i < GC.getNumMainMenus()) ? (CyMainMenuInfo*)&GC.getMainMenus(i) : NULL);
}
CyFatherInfo* CyGlobalContext::getFatherInfo(int i) const
{
	return ((i >= 0 && i < GC.getNumFatherInfos()) ? (CyFatherInfo*)&GC.getFatherInfo((FatherTypes)i) : NULL);
}
CyFatherPointInfo* CyGlobalContext::getFatherPointInfo(int i) const
{
	return ((i >= 0 && i < GC.getNumFatherPointInfos()) ? (CyFatherPointInfo*)&GC.getFatherPointInfo((FatherPointTypes)i) : NULL);
}
CvInfoBase* CyGlobalContext::getInvisibleInfo(int i) const
{
	return ((i >= 0 && i < GC.getNumInvisibleInfos()) ? &GC.getInvisibleInfo((InvisibleTypes)i) : NULL);
}

CvInfoBase* CyGlobalContext::getAttitudeInfo(int i) const
{
	return (i>=0 && i<NUM_ATTITUDE_TYPES) ? &GC.getAttitudeInfo((AttitudeTypes)i) : NULL;
}
CvInfoBase* CyGlobalContext::getMemoryInfo(int i) const
{
	return (i>=0 && i<NUM_MEMORY_TYPES) ? &GC.getMemoryInfo((MemoryTypes)i) : NULL;
}
CvInfoBase* CyGlobalContext::getFatherCategoryInfo(int i) const
{
	return (i>=0 && i<GC.getNumFatherCategoryInfos()) ? &GC.getFatherCategoryInfo((FatherCategoryTypes)i) : NULL;
}
CyPlayerOptionInfo* CyGlobalContext::getPlayerOptionsInfoByIndex(int i) const
{
	return (CyPlayerOptionInfo*)&GC.getPlayerOptionInfo((PlayerOptionTypes) i);
}

CyGraphicOptionInfo* CyGlobalContext::getGraphicOptionsInfoByIndex(int i) const
{
	return (CyGraphicOptionInfo*)&GC.getGraphicOptionInfo((GraphicOptionTypes) i);
}

CvInfoBase* CyGlobalContext::getConceptInfo(int i) const
{
	return (i>=0 && i<GC.getNumConceptInfos()) ? &GC.getConceptInfo((ConceptTypes)i) : NULL;
}

CvInfoBase* CyGlobalContext::getCalendarInfo(int i) const
{
	return (i>=0 && i<GC.getNumCalendarInfos()) ? &GC.getCalendarInfo((CalendarTypes)i) : NULL;
}

CvInfoBase* CyGlobalContext::getGameOptionInfo(int i) const
{
	return (i>=0 && i<GC.getNumGameOptionInfos()) ? &GC.getGameOptionInfo((GameOptionTypes)i) : NULL;
}

CvInfoBase* CyGlobalContext::getMPOptionInfo(int i) const
{
	return (i>=0 && i<GC.getNumMPOptionInfos()) ? &GC.getMPOptionInfo((MultiplayerOptionTypes)i) : NULL;
}

CvInfoBase* CyGlobalContext::getForceControlInfo(int i) const
{
	return (i>=0 && i<GC.getNumForceControlInfos()) ? &GC.getForceControlInfo((ForceControlTypes)i) : NULL;
}

CvInfoBase* CyGlobalContext::getSeasonInfo(int i) const
{
	return (i>=0 && i<GC.getNumSeasonInfos()) ? &GC.getSeasonInfo((SeasonTypes)i) : NULL;
}

CvInfoBase* CyGlobalContext::getMonthInfo(int i) const
{
	return (i>=0 && i<GC.getNumMonthInfos()) ? &GC.getMonthInfo((MonthTypes)i) : NULL;
}

CvInfoBase* CyGlobalContext::getDenialInfo(int i) const
{
	return (i>=0 && i<GC.getNumDenialInfos()) ? &GC.getDenialInfo((DenialTypes)i) : NULL;
}

CyEventTriggerInfo* CyGlobalContext::getEventTriggerInfo(int i) const
{
	return (i>=0 && i<GC.getNumEventTriggerInfos()) ? (CyEventTriggerInfo*)&GC.getEventTriggerInfo((EventTriggerTypes)i) : NULL;
}

CyEventInfo* CyGlobalContext::getEventInfo(int i) const
{
	return (i>=0 && i<GC.getNumEventInfos()) ? (CyEventInfo*)&GC.getEventInfo((EventTypes)i) : NULL;
}

//Androrc UnitArtStyles
CvUnitArtStyleTypeInfo* CyGlobalContext::getUnitArtStyleTypeInfo(int i) const
{
	return (i>=0 && i<GC.getNumUnitArtStyleTypeInfos()) ? &GC.getUnitArtStyleTypeInfo((UnitArtStyleTypes)i) : NULL;
}
//Androrc End

CvArtInfoInterface* CyGlobalContext::getInterfaceArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumInterfaceArtInfos()) ? &ARTFILEMGR.getInterfaceArtInfo(i) : NULL;
}

CvArtInfoMovie* CyGlobalContext::getMovieArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumMovieArtInfos()) ? &ARTFILEMGR.getMovieArtInfo(i) : NULL;
}

CvArtInfoMisc* CyGlobalContext::getMiscArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumMiscArtInfos()) ? &ARTFILEMGR.getMiscArtInfo(i) : NULL;
}

CvArtInfoUnit* CyGlobalContext::getUnitArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumUnitArtInfos()) ? &ARTFILEMGR.getUnitArtInfo(i) : NULL;
}

CvArtInfoBuilding* CyGlobalContext::getBuildingArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumBuildingArtInfos()) ? &ARTFILEMGR.getBuildingArtInfo(i) : NULL;
}

CvArtInfoCivilization* CyGlobalContext::getCivilizationArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumCivilizationArtInfos()) ? &ARTFILEMGR.getCivilizationArtInfo(i) : NULL;
}

CvArtInfoLeaderhead* CyGlobalContext::getLeaderheadArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumLeaderheadArtInfos()) ? &ARTFILEMGR.getLeaderheadArtInfo(i) : NULL;
}

CvArtInfoBonus* CyGlobalContext::getBonusArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumBonusArtInfos()) ? &ARTFILEMGR.getBonusArtInfo(i) : NULL;
}

CvArtInfoImprovement* CyGlobalContext::getImprovementArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumImprovementArtInfos()) ? &ARTFILEMGR.getImprovementArtInfo(i) : NULL;
}

CvArtInfoTerrain* CyGlobalContext::getTerrainArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumTerrainArtInfos()) ? &ARTFILEMGR.getTerrainArtInfo(i) : NULL;
}

CvArtInfoFeature* CyGlobalContext::getFeatureArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumFeatureArtInfos()) ? &ARTFILEMGR.getFeatureArtInfo(i) : NULL;
}

CyGameSpeedInfo* CyGlobalContext::getGameSpeedInfo(int i) const
{
	return (CyGameSpeedInfo*)&(GC.getGameSpeedInfo((GameSpeedTypes) i));
}
CyTurnTimerInfo* CyGlobalContext::getTurnTimerInfo(int i) const
{
	return (CyTurnTimerInfo*)&(GC.getTurnTimerInfo((TurnTimerTypes) i));
}

// PatchMod: Achievements START
CyAchieveInfo* CyGlobalContext::getAchieveInfo(int i) const
{
	return ((i >= 0 && i < GC.getNumAchieveInfos()) ? (CyAchieveInfo*)&GC.getAchieveInfo((AchieveTypes)i) : NULL);
}
// PatchMod: Achievements END



int CyGlobalContext::getSymbolID(int iSymbol)
{
	return GC.getSymbolID(static_cast<FontSymbols>(iSymbol));
}

std::string CyGlobalContext::getSymbolName(int iSymbol)
{
	return getTypeStr(static_cast<FontSymbols>(iSymbol));
}

// variable exclusively for CvGameTextMgr::buildCityBillboardIconString
// used to display a bunch of GameFont on billboards for debug purposes.
extern int iGameFontDebugChar;

void CyGlobalContext::setGameFontDebug(int iChar)
{
	if (iGameFontDebugChar != iChar)
	{
		iGameFontDebugChar = iChar;
		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			CvPlayer &kPlayer = GET_PLAYER(static_cast<PlayerTypes>(iPlayer));
			CvCity* pLoopCity;
			int iLoop;
			for (pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				pLoopCity->setBillboardDirty(true);
			}
		}
	}
}

/// one/two city plot radius
int CyGlobalContext::getDefaultCityCatchmentRadius() const
{
	UserSettings settings;
	return settings.getColonyRadius();
}
// city radius end

// not the nicest solution, but who cares?
// All it does is allowing the DA to open on the last open state. It's not saved or anything.
int iDomesticAdvisorState = 0;

int CyGlobalContext::getDomesticAdvisorState() const
{
	return iDomesticAdvisorState;
}

void CyGlobalContext::setDomesticAdvisorState(int iPage)
{
	iDomesticAdvisorState = iPage;
}

void CyGlobalContext::setExeXmlLengthOverride(bool bEnabled)
{
	GC.setExeXmlLengthOverride(bEnabled);
}

CyUserSettings* CyGlobalContext::getUserSettings() const
{
	return new CyUserSettings();
}

void CyGlobalContext::openNetworkOOSMenu()
{
	CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_NETWORK_OOS_MENU);
	gDLL->getInterfaceIFace()->addPopup(pInfo, GC.getGameINLINE().getActivePlayer(), true, true);
}

void CyGlobalContext::openReadme(std::string section)
{
	if (section.length() > 0)
	{
		OpenURL::openReadme(section);
	}
	else
	{
		OpenURL::openReadme();
	}
}
