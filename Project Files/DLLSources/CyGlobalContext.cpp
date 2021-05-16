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
	static CyMap cyMap(&GC.getMapINLINE());
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

CvEffectInfo* CyGlobalContext::getEffectInfo(int /*EffectTypes*/ i) const
{
	return (i>=0 && i<GC.getNumEffectInfos()) ? &GC.getEffectInfo((EffectTypes) i) : NULL;
}
CvTerrainInfo* CyGlobalContext::getTerrainInfo(int /*TerrainTypes*/ i) const
{
	return (i>=0 && i<GC.getNumTerrainInfos()) ? &GC.getTerrainInfo((TerrainTypes) i) : NULL;
}
CvBonusInfo* CyGlobalContext::getBonusInfo(int /*(BonusTypes)*/ i) const
{
	return (i>=0 && i<GC.getNumBonusInfos()) ? &GC.getBonusInfo((BonusTypes) i) : NULL;
}
CvFeatureInfo* CyGlobalContext::getFeatureInfo(int i) const
{
	return (i>=0 && i<GC.getNumFeatureInfos()) ? &GC.getFeatureInfo((FeatureTypes) i) : NULL;
}
CvCivilizationInfo* CyGlobalContext::getCivilizationInfo(int i) const
{
	return (i>=0 && i<GC.getNumCivilizationInfos()) ? &GC.getCivilizationInfo((CivilizationTypes) i) : NULL;
}

CvLeaderHeadInfo* CyGlobalContext::getLeaderHeadInfo(int i) const
{
	return (i>=0 && i<GC.getNumLeaderHeadInfos()) ? &GC.getLeaderHeadInfo((LeaderHeadTypes) i) : NULL;
}

CvTraitInfo* CyGlobalContext::getTraitInfo(int i) const
{
	return (i>=0 && i<GC.getNumTraitInfos()) ? &GC.getTraitInfo((TraitTypes) i) : NULL;
}

CvUnitInfo* CyGlobalContext::getUnitInfo(int i) const
{
	return (i>=0 && i<GC.getNumUnitInfos()) ? &GC.getUnitInfo((UnitTypes) i) : NULL;
}
CvSpecialUnitInfo* CyGlobalContext::getSpecialUnitInfo(int i) const
{
	return (i>=0 && i<GC.getNumSpecialUnitInfos()) ? &GC.getSpecialUnitInfo((SpecialUnitTypes) i) : NULL;
}
CvYieldInfo* CyGlobalContext::getYieldInfo(int i) const
{
	return (i>=0 && i<NUM_YIELD_TYPES) ? &GC.getYieldInfo((YieldTypes) i) : NULL;
}

CvRouteInfo* CyGlobalContext::getRouteInfo(int i) const
{
	return (i>=0 && i<GC.getNumRouteInfos()) ? &GC.getRouteInfo((RouteTypes) i) : NULL;
}

CvImprovementInfo* CyGlobalContext::getImprovementInfo(int i) const
{
	return (i>=0 && i<GC.getNumImprovementInfos()) ? &GC.getImprovementInfo((ImprovementTypes) i) : NULL;
}

CvGoodyInfo* CyGlobalContext::getGoodyInfo(int i) const
{
	return (i>=0 && i<GC.getNumGoodyInfos()) ? &GC.getGoodyInfo((GoodyTypes) i) : NULL;
}

CvBuildInfo* CyGlobalContext::getBuildInfo(int i) const
{
	return (i>=0 && i<GC.getNumBuildInfos()) ? &GC.getBuildInfo((BuildTypes) i) : NULL;
}

CvHandicapInfo* CyGlobalContext::getHandicapInfo(int i) const
{
	return (i>=0 && i<GC.getNumHandicapInfos()) ? &GC.getHandicapInfo((HandicapTypes) i) : NULL;
}

CvBuildingClassInfo* CyGlobalContext::getBuildingClassInfo(int i) const
{
	return (i>=0 && i<GC.getNumBuildingClassInfos()) ? &GC.getBuildingClassInfo((BuildingClassTypes) i) : NULL;
}

CvBuildingInfo* CyGlobalContext::getBuildingInfo(int i) const
{
	return (i>=0 && i<GC.getNumBuildingInfos()) ? &GC.getBuildingInfo((BuildingTypes) i) : NULL;
}
CvUnitClassInfo* CyGlobalContext::getUnitClassInfo(int i) const
{
	return (i>=0 && i<GC.getNumUnitClassInfos()) ? &GC.getUnitClassInfo((UnitClassTypes) i) : NULL;
}

CvInfoBase* CyGlobalContext::getUnitCombatInfo(int i) const
{
	return (i>=0 && i<GC.getNumUnitCombatInfos()) ? &GC.getUnitCombatInfo((UnitCombatTypes)i) : NULL;
}

CvInfoBase* CyGlobalContext::getDomainInfo(int i) const
{
	return (i>=0 && i<NUM_DOMAIN_TYPES) ? &GC.getDomainInfo((DomainTypes)i) : NULL;
}

CvActionInfo* CyGlobalContext::getActionInfo(int i) const
{
	return (i>=0 && i<GC.getNumActionInfos()) ? &GC.getActionInfo(i) : NULL;
}
CvAutomateInfo* CyGlobalContext::getAutomateInfo(int i) const
{
	return (i>=0 && i<GC.getNumAutomateInfos()) ? &GC.getAutomateInfo(i) : NULL;
}
CvCommandInfo* CyGlobalContext::getCommandInfo(int i) const
{
	return (i>=0 && i<NUM_COMMAND_TYPES) ? &GC.getCommandInfo((CommandTypes)i) : NULL;
}
CvControlInfo* CyGlobalContext::getControlInfo(int i) const
{
	return (i>=0 && i<NUM_CONTROL_TYPES) ? &GC.getControlInfo((ControlTypes)i) : NULL;
}
CvMissionInfo* CyGlobalContext::getMissionInfo(int i) const
{
	return (i>=0 && i<NUM_MISSION_TYPES) ? &GC.getMissionInfo((MissionTypes) i) : NULL;
}
CvPromotionInfo* CyGlobalContext::getPromotionInfo(int i) const
{
	return (i>=0 && i<GC.getNumPromotionInfos()) ? &GC.getPromotionInfo((PromotionTypes) i) : NULL;
}
CvProfessionInfo* CyGlobalContext::getProfessionInfo(int i) const
{
	return (i>=0 && i<GC.getNumProfessionInfos()) ? &GC.getProfessionInfo((ProfessionTypes) i) : NULL;
}

CvSpecialBuildingInfo* CyGlobalContext::getSpecialBuildingInfo(int i) const
{
	return (i>=0 && i<GC.getNumSpecialBuildingInfos()) ? &GC.getSpecialBuildingInfo((SpecialBuildingTypes) i) : NULL;
}

CvInfoBase* CyGlobalContext::getCivicOptionInfo(int i) const
{
	return &GC.getCivicOptionInfo((CivicOptionTypes) i);
}

CvCivicInfo* CyGlobalContext::getCivicInfo(int i) const
{
	return &GC.getCivicInfo((CivicTypes) i);
}
CvDiplomacyInfo* CyGlobalContext::getDiplomacyInfo(int i) const
{
	return &GC.getDiplomacyInfo(i);
}
CvHurryInfo* CyGlobalContext::getHurryInfo(int i) const
{
	return (i>=0 && i<GC.getNumHurryInfos()) ? &GC.getHurryInfo((HurryTypes) i) : NULL;
}

CvAnimationPathInfo* CyGlobalContext::getAnimationPathInfo(int i) const
{
	return (i>=0 && i<GC.getNumAnimationPathInfos()) ? &GC.getAnimationPathInfo((AnimationPathTypes)i) : NULL;
}

CvEmphasizeInfo* CyGlobalContext::getEmphasizeInfo(int i) const
{
	return (i>=0 && i<GC.getNumEmphasizeInfos()) ? &GC.getEmphasizeInfo((EmphasizeTypes) i) : NULL;
}

CvCultureLevelInfo* CyGlobalContext::getCultureLevelInfo(int i) const
{
	return (i>=0 && i<GC.getNumCultureLevelInfos()) ? &GC.getCultureLevelInfo((CultureLevelTypes) i) : NULL;
}

CvVictoryInfo* CyGlobalContext::getVictoryInfo(int i) const
{
	return (i>=0 && i<GC.getNumVictoryInfos()) ? &GC.getVictoryInfo((VictoryTypes) i) : NULL;
}

CvEraInfo* CyGlobalContext::getEraInfo(int i) const
{
	return (i>=0 && i<GC.getNumEraInfos()) ? &GC.getEraInfo((EraTypes) i) : NULL;
}

CvWorldInfo* CyGlobalContext::getWorldInfo(int i) const
{
	return (i>=0 && i<GC.getNumWorldInfos()) ? &GC.getWorldInfo((WorldSizeTypes) i) : NULL;
}

CvClimateInfo* CyGlobalContext::getClimateInfo(int i) const
{
	return (i>=0 && i<GC.getNumClimateInfos()) ? &GC.getClimateInfo((ClimateTypes) i) : NULL;
}

CvSeaLevelInfo* CyGlobalContext::getSeaLevelInfo(int i) const
{
	return (i>=0 && i<GC.getNumSeaLevelInfos()) ? &GC.getSeaLevelInfo((SeaLevelTypes) i) : NULL;
}

CvEuropeInfo* CyGlobalContext::getEuropeInfo(int i) const
{
	return (i>=0 && i<GC.getNumEuropeInfos()) ? &GC.getEuropeInfo((EuropeTypes) i) : NULL;
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
CvMainMenuInfo* CyGlobalContext::getMainMenus(int i) const
{
	return ((i >= 0 && i < GC.getNumMainMenus()) ? &GC.getMainMenus(i) : NULL);
}
CvFatherInfo* CyGlobalContext::getFatherInfo(int i) const
{
	return ((i >= 0 && i < GC.getNumFatherInfos()) ? &GC.getFatherInfo((FatherTypes)i) : NULL);
}
CvFatherPointInfo* CyGlobalContext::getFatherPointInfo(int i) const
{
	return ((i >= 0 && i < GC.getNumFatherPointInfos()) ? &GC.getFatherPointInfo((FatherPointTypes)i) : NULL);
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
CvPlayerOptionInfo* CyGlobalContext::getPlayerOptionsInfoByIndex(int i) const
{
	return &GC.getPlayerOptionInfo((PlayerOptionTypes) i);
}

CvGraphicOptionInfo* CyGlobalContext::getGraphicOptionsInfoByIndex(int i) const
{
	return &GC.getGraphicOptionInfo((GraphicOptionTypes) i);
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

CvEventTriggerInfo* CyGlobalContext::getEventTriggerInfo(int i) const
{
	return (i>=0 && i<GC.getNumEventTriggerInfos()) ? &GC.getEventTriggerInfo((EventTriggerTypes)i) : NULL;
}

CvEventInfo* CyGlobalContext::getEventInfo(int i) const
{
	return (i>=0 && i<GC.getNumEventInfos()) ? &GC.getEventInfo((EventTypes)i) : NULL;
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

CvGameSpeedInfo* CyGlobalContext::getGameSpeedInfo(int i) const
{
	return &(GC.getGameSpeedInfo((GameSpeedTypes) i));
}
CvTurnTimerInfo* CyGlobalContext::getTurnTimerInfo(int i) const
{
	return &(GC.getTurnTimerInfo((TurnTimerTypes) i));
}

// PatchMod: Achievements START
CvAchieveInfo* CyGlobalContext::getAchieveInfo(int i) const
{
	return ((i >= 0 && i < GC.getNumAchieveInfos()) ? &GC.getAchieveInfo((AchieveTypes)i) : NULL);
}
// PatchMod: Achievements END

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

CyUserSettings* CyGlobalContext::getUserSettings() const
{
	return new CyUserSettings();
}
