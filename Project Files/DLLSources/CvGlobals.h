#pragma once

// CvGlobals.h

#ifndef CIV4_GLOBALS_H
#define CIV4_GLOBALS_H

//#include "CvStructs.h"
//
// 'global' vars for Civ IV.  singleton class.
// All globals and global types should be contained in this class
// Author -	Mustafa Thamer
//

class CvDLLUtilityIFaceBase;
class CvRandom;
class CvGameAI;
class CMessageControl;
class CvDropMgr;
class CMessageQueue;
class CvSetupData;
class CvInitCore;
class CvMessageCodeTranslator;
class CvPortal;
class CvStatsReporter;
class CvDLLInterfaceIFaceBase;
class CvPlayerAI;
class CvDiplomacyScreen;
class CvCivicsScreen;
class CvWBUnitEditScreen;
class CvWBCityEditScreen;
class CMPDiplomacyScreen;
class FMPIManager;
class FAStar;
class CvInterface;
class CMainMenu;
class CvEngine;
class CvArtFileMgr;
class FVariableSystem;
class CvMap;
class CvPlayerAI;
class CvTeamAI;
class CvInterfaceModeInfo;
class CvWorldInfo;
class CvClimateInfo;
class CvSeaLevelInfo;
class CvEuropeInfo;
class CvColorInfo;
class CvPlayerColorInfo;
class CvRouteModelInfo;
class CvRiverModelInfo;
class CvWaterPlaneInfo;
class CvTerrainPlaneInfo;
class CvCameraOverlayInfo;
class CvAnimationPathInfo;
class CvAnimationCategoryInfo;
class CvEntityEventInfo;
class CvEffectInfo;
class CvAttachableInfo;
class CvUnitFormationInfo;
class CvGameText;
class CvLandscapeInfo;
class CvTerrainInfo;
class CvBonusInfo;
class CvFeatureInfo;
class CvCivilizationInfo;
class CvLeaderHeadInfo;
class CvTraitInfo;
class CvCursorInfo;
class CvSlideShowInfo;
class CvSlideShowRandomInfo;
class CvWorldPickerInfo;
class CvUnitInfo;
class CvSpecialUnitInfo;
class CvInfoBase;
class CvYieldInfo;
class CvRouteInfo;
class CvImprovementInfo;
class CvGoodyInfo;
class CvBuildInfo;
class CvHandicapInfo;
class CvGameSpeedInfo;
class CvAlarmInfo;
class CvTurnTimerInfo;
class CvBuildingClassInfo;
class CvBuildingInfo;
class CvSpecialBuildingInfo;
class CvUnitClassInfo;
class CvActionInfo;
class CvMissionInfo;
class CvControlInfo;
class CvCommandInfo;
class CvAutomateInfo;
class CvPromotionInfo;
class CvProfessionInfo;
class CvCivicInfo;
class CvDiplomacyInfo;
class CvEraInfo;
class CvHurryInfo;
class CvEmphasizeInfo;
class CvCultureLevelInfo;
class CvVictoryInfo;
class CvGameOptionInfo;
class CvMPOptionInfo;
class CvForceControlInfo;
class CvPlayerOptionInfo;
class CvGraphicOptionInfo;
class CvEventTriggerInfo;
class CvEventInfo;
class CvFatherInfo;
class CvFatherPointInfo;
//Androrc UnitArtStyles
class CvUnitArtStyleTypeInfo;
//Androrc End
class CvMainMenuInfo;
// PatchMod: Achievements START
class CvAchieveInfo;
// PatchMod: Achievements END


class CvGlobals
{
//	friend class CvDLLUtilityIFace;
	friend class CvXMLLoadUtility;
public:

	// singleton accessor
	DllExport inline static CvGlobals& getInstance();

	DllExport CvGlobals();
	DllExport virtual ~CvGlobals();

	// PatchMod: Achievements START
protected:
	std::vector<CvAchieveInfo*> m_paAchieveInfo;
public:
	DllExport int getNumAchieveInfos();
	DllExport std::vector<CvAchieveInfo*>& getAchieveInfo();
	DllExport CvAchieveInfo& getAchieveInfo(AchieveTypes eAchieve);
	// PatchMod: Achievements END

	DllExport void init();
	DllExport void uninit();
	DllExport void clearTypesMap();

	DllExport CvDiplomacyScreen* getDiplomacyScreen();
	DllExport CMPDiplomacyScreen* getMPDiplomacyScreen();

	DllExport FMPIManager*& getFMPMgrPtr();
	DllExport CvPortal& getPortal();
	DllExport CvSetupData& getSetupData();
	DllExport CvInitCore& getInitCore();
	DllExport CvInitCore& getLoadedInitCore();
	DllExport CvInitCore& getIniInitCore();
	DllExport CvMessageCodeTranslator& getMessageCodes();
	DllExport CvStatsReporter& getStatsReporter();
	DllExport CvStatsReporter* getStatsReporterPtr();
	DllExport CvInterface& getInterface();
	DllExport CvInterface* getInterfacePtr();
	DllExport int getMaxCivPlayers() const;
#ifdef _USRDLL
	CvMap& getMapINLINE() { return *m_map; }				// inlined for perf reasons, do not use outside of dll
	CvGameAI& getGameINLINE() { return *m_game; }			// inlined for perf reasons, do not use outside of dll
#endif
	DllExport CvMap& getMap();
	DllExport CvGameAI& getGame();
	DllExport CvGameAI *getGamePointer();
	DllExport CvRandom& getASyncRand();
	DllExport CMessageQueue& getMessageQueue();
	DllExport CMessageQueue& getHotMessageQueue();
	DllExport CMessageControl& getMessageControl();
	DllExport CvDropMgr& getDropMgr();
	DllExport FAStar& getPathFinder();
	DllExport FAStar& getInterfacePathFinder();
	DllExport FAStar& getStepFinder();
	DllExport FAStar& getRouteFinder();
	DllExport FAStar& getBorderFinder();
	DllExport FAStar& getAreaFinder();
	DllExport NiPoint3& getPt3Origin();

	DllExport std::vector<CvInterfaceModeInfo*>& getInterfaceModeInfo();
	DllExport CvInterfaceModeInfo& getInterfaceModeInfo(InterfaceModeTypes e);

	DllExport NiPoint3& getPt3CameraDir();

	DllExport bool& getLogging();
	DllExport bool& getRandLogging();
	DllExport bool& getSynchLogging();
	DllExport bool& overwriteLogs();

	DllExport int* getPlotDirectionX();
	DllExport int* getPlotDirectionY();
	DllExport int* getPlotCardinalDirectionX();
	DllExport int* getPlotCardinalDirectionY();
	DllExport int* getCityPlotX();
	DllExport int* getCityPlotY();
	DllExport int* getCityPlotPriority();
	DllExport int getXYCityPlot(int i, int j);
	DirectionTypes* getTurnLeftDirection();
	DirectionTypes getTurnLeftDirection(int i);
	DirectionTypes* getTurnRightDirection();
	DirectionTypes getTurnRightDirection(int i);
	DllExport DirectionTypes getXYDirection(int i, int j);

	//
	// Global Infos
	// All info type strings are upper case and are kept in this hash map for fast lookup
	//
	DllExport int getInfoTypeForString(const char* szType, bool hideAssert = false) const;			// returns the infos index, use this when searching for an info type string
	DllExport void setInfoTypeFromString(const char* szType, int idx);
	DllExport void infoTypeFromStringReset();
	DllExport void addToInfosVectors(void *infoVector);
	DllExport void infosReset();

	DllExport int getNumWorldInfos();
	DllExport std::vector<CvWorldInfo*>& getWorldInfo();
	DllExport CvWorldInfo& getWorldInfo(WorldSizeTypes e);

	DllExport int getNumClimateInfos();
	DllExport std::vector<CvClimateInfo*>& getClimateInfo();
	DllExport CvClimateInfo& getClimateInfo(ClimateTypes e);

	DllExport int getNumSeaLevelInfos();
	DllExport std::vector<CvSeaLevelInfo*>& getSeaLevelInfo();
	DllExport CvSeaLevelInfo& getSeaLevelInfo(SeaLevelTypes e);

	DllExport int getNumEuropeInfos();
	DllExport std::vector<CvEuropeInfo*>& getEuropeInfo();
	DllExport CvEuropeInfo& getEuropeInfo(EuropeTypes e);

	DllExport int getNumColorInfos();
	DllExport std::vector<CvColorInfo*>& getColorInfo();
	DllExport CvColorInfo& getColorInfo(ColorTypes e);

	DllExport int getNumPlayerColorInfos();
	DllExport std::vector<CvPlayerColorInfo*>& getPlayerColorInfo();
	DllExport CvPlayerColorInfo& getPlayerColorInfo(PlayerColorTypes e);

	DllExport int getNumHints();
	DllExport std::vector<CvInfoBase*>& getHints();
	DllExport CvInfoBase& getHints(int i);

	DllExport int getNumMainMenus();
	DllExport std::vector<CvMainMenuInfo*>& getMainMenus();
	DllExport CvMainMenuInfo& getMainMenus(int i);

	DllExport int getNumRouteModelInfos();
	DllExport std::vector<CvRouteModelInfo*>& getRouteModelInfo();
	DllExport CvRouteModelInfo& getRouteModelInfo(int i);

	DllExport int getNumRiverModelInfos();
	DllExport std::vector<CvRiverModelInfo*>& getRiverModelInfo();
	DllExport CvRiverModelInfo& getRiverModelInfo(int i);

	DllExport int getNumWaterPlaneInfos();
	DllExport std::vector<CvWaterPlaneInfo*>& getWaterPlaneInfo();
	DllExport CvWaterPlaneInfo& getWaterPlaneInfo(int i);

	DllExport int getNumTerrainPlaneInfos();
	DllExport std::vector<CvTerrainPlaneInfo*>& getTerrainPlaneInfo();
	DllExport CvTerrainPlaneInfo& getTerrainPlaneInfo(int i);

	DllExport int getNumCameraOverlayInfos();
	DllExport std::vector<CvCameraOverlayInfo*>& getCameraOverlayInfo();
	DllExport CvCameraOverlayInfo& getCameraOverlayInfo(int i);

	DllExport int getNumAnimationPathInfos();
	DllExport std::vector<CvAnimationPathInfo*>& getAnimationPathInfo();
	DllExport CvAnimationPathInfo& getAnimationPathInfo(AnimationPathTypes e);

	DllExport int getNumAnimationCategoryInfos();
	DllExport std::vector<CvAnimationCategoryInfo*>& getAnimationCategoryInfo();
	DllExport CvAnimationCategoryInfo& getAnimationCategoryInfo(AnimationCategoryTypes e);

	DllExport int getNumEntityEventInfos();
	DllExport std::vector<CvEntityEventInfo*>& getEntityEventInfo();
	DllExport CvEntityEventInfo& getEntityEventInfo(EntityEventTypes e);

	DllExport int getNumEffectInfos();
	DllExport std::vector<CvEffectInfo*>& getEffectInfo();
	DllExport CvEffectInfo& getEffectInfo(int i);

	DllExport int getNumAttachableInfos();
	DllExport std::vector<CvAttachableInfo*>& getAttachableInfo();
	DllExport CvAttachableInfo& getAttachableInfo(int i);

	DllExport int getNumUnitFormationInfos();
	DllExport std::vector<CvUnitFormationInfo*>& getUnitFormationInfo();
	DllExport CvUnitFormationInfo& getUnitFormationInfo(int i);

	DllExport int getNumGameTextXML();
	DllExport std::vector<CvGameText*>& getGameTextXML();

	DllExport int getNumLandscapeInfos();
	DllExport std::vector<CvLandscapeInfo*>& getLandscapeInfo();
	DllExport CvLandscapeInfo& getLandscapeInfo(int iIndex);
	DllExport int getActiveLandscapeID();
	DllExport void setActiveLandscapeID(int iLandscapeID);

	DllExport int getNumTerrainInfos() const;
	DllExport std::vector<CvTerrainInfo*>& getTerrainInfo();
	DllExport CvTerrainInfo& getTerrainInfo(TerrainTypes eTerrainNum);

	DllExport int getNumBonusInfos() const;
	DllExport std::vector<CvBonusInfo*>& getBonusInfo();
	DllExport CvBonusInfo& getBonusInfo(BonusTypes eBonusNum);

	DllExport int getNumFeatureInfos() const;
	DllExport std::vector<CvFeatureInfo*>& getFeatureInfo();
	DllExport CvFeatureInfo& getFeatureInfo(FeatureTypes eFeatureNum);

	DllExport int getNumCivilizationInfos() const;
	DllExport std::vector<CvCivilizationInfo*>& getCivilizationInfo();
	DllExport CvCivilizationInfo& getCivilizationInfo(CivilizationTypes eCivilizationNum);

	DllExport int getNumLeaderHeadInfos() const;
	DllExport std::vector<CvLeaderHeadInfo*>& getLeaderHeadInfo();
	DllExport CvLeaderHeadInfo& getLeaderHeadInfo(LeaderHeadTypes eLeaderHeadNum);

	DllExport int getNumTraitInfos() const;
	DllExport std::vector<CvTraitInfo*>& getTraitInfo();
	DllExport	CvTraitInfo& getTraitInfo(TraitTypes eTraitNum);

	DllExport int getNumCursorInfos() const;
	DllExport std::vector<CvCursorInfo*>& getCursorInfo();
	DllExport	CvCursorInfo& getCursorInfo(CursorTypes eCursorNum);

	DllExport int getNumSlideShowInfos() const;
	DllExport std::vector<CvSlideShowInfo*>& getSlideShowInfo();
	DllExport	CvSlideShowInfo& getSlideShowInfo(int iIndex);

	DllExport int getNumSlideShowRandomInfos() const;
	DllExport std::vector<CvSlideShowRandomInfo*>& getSlideShowRandomInfo();
	DllExport	CvSlideShowRandomInfo& getSlideShowRandomInfo(int iIndex);

	DllExport int getNumWorldPickerInfos() const;
	DllExport std::vector<CvWorldPickerInfo*>& getWorldPickerInfo();
	DllExport	CvWorldPickerInfo& getWorldPickerInfo(int iIndex);

	DllExport int getNumUnitInfos() const;
	DllExport std::vector<CvUnitInfo*>& getUnitInfo();
	DllExport	CvUnitInfo& getUnitInfo(UnitTypes eUnitNum);

	DllExport int getNumSpecialUnitInfos() const;
	DllExport std::vector<CvSpecialUnitInfo*>& getSpecialUnitInfo();
	DllExport	CvSpecialUnitInfo& getSpecialUnitInfo(SpecialUnitTypes eSpecialUnitNum);

	DllExport int getNumConceptInfos() const;
	DllExport std::vector<CvInfoBase*>& getConceptInfo();
	DllExport CvInfoBase& getConceptInfo(ConceptTypes e);
	DllExport int getNumCalendarInfos() const;
	DllExport std::vector<CvInfoBase*>& getCalendarInfo();
	DllExport CvInfoBase& getCalendarInfo(CalendarTypes e);

	DllExport int getNumSeasonInfos();
	DllExport std::vector<CvInfoBase*>& getSeasonInfo();
	DllExport CvInfoBase& getSeasonInfo(SeasonTypes e);

	DllExport int getNumMonthInfos();
	DllExport std::vector<CvInfoBase*>& getMonthInfo();
	DllExport CvInfoBase& getMonthInfo(MonthTypes e);

	DllExport int getNumDenialInfos();
	DllExport std::vector<CvInfoBase*>& getDenialInfo();
	DllExport CvInfoBase& getDenialInfo(DenialTypes e);

	DllExport int getNumInvisibleInfos();
	DllExport std::vector<CvInfoBase*>& getInvisibleInfo();
	DllExport CvInfoBase& getInvisibleInfo(InvisibleTypes e);

	DllExport int getNumFatherInfos();
	DllExport std::vector<CvFatherInfo*>& getFatherInfo();
	DllExport CvFatherInfo& getFatherInfo(FatherTypes e);

	DllExport int getNumFatherPointInfos();
	DllExport std::vector<CvFatherPointInfo*>& getFatherPointInfo();
	DllExport CvFatherPointInfo& getFatherPointInfo(FatherPointTypes e);

	DllExport int getNumUnitCombatInfos();
	DllExport std::vector<CvInfoBase*>& getUnitCombatInfo();
	DllExport CvInfoBase& getUnitCombatInfo(UnitCombatTypes e);

	DllExport std::vector<CvInfoBase*>& getDomainInfo();
	DllExport CvInfoBase& getDomainInfo(DomainTypes e);

	DllExport std::vector<CvInfoBase*>& getUnitAIInfo();
	DllExport CvInfoBase& getUnitAIInfo(UnitAITypes eUnitAINum);

	DllExport std::vector<CvInfoBase*>& getAttitudeInfo();
	DllExport	CvInfoBase& getAttitudeInfo(AttitudeTypes eAttitudeNum);

	DllExport std::vector<CvInfoBase*>& getMemoryInfo();
	DllExport	CvInfoBase& getMemoryInfo(MemoryTypes eMemoryNum);

	DllExport int getNumFatherCategoryInfos();
	DllExport std::vector<CvInfoBase*>& getFatherCategoryInfo();
	DllExport	CvInfoBase& getFatherCategoryInfo(FatherCategoryTypes eFatherCategoryNum);

	DllExport int getNumGameOptionInfos();
	DllExport std::vector<CvGameOptionInfo*>& getGameOptionInfo();
	DllExport	CvGameOptionInfo& getGameOptionInfo(GameOptionTypes eGameOptionNum);

	DllExport int getNumMPOptionInfos();
	DllExport std::vector<CvMPOptionInfo*>& getMPOptionInfo();
	DllExport	CvMPOptionInfo& getMPOptionInfo(MultiplayerOptionTypes eMPOptionNum);

	DllExport int getNumForceControlInfos();
	DllExport std::vector<CvForceControlInfo*>& getForceControlInfo();
	DllExport	CvForceControlInfo& getForceControlInfo(ForceControlTypes eForceControlNum);

	DllExport std::vector<CvPlayerOptionInfo*>& getPlayerOptionInfo();
	DllExport	CvPlayerOptionInfo& getPlayerOptionInfo(PlayerOptionTypes ePlayerOptionNum);

	DllExport std::vector<CvGraphicOptionInfo*>& getGraphicOptionInfo();
	DllExport	CvGraphicOptionInfo& getGraphicOptionInfo(GraphicOptionTypes eGraphicOptionNum);

	DllExport std::vector<CvYieldInfo*>& getYieldInfo();
	DllExport	CvYieldInfo& getYieldInfo(YieldTypes eYieldNum);

	DllExport int getNumRouteInfos();
	DllExport std::vector<CvRouteInfo*>& getRouteInfo();
	DllExport	CvRouteInfo& getRouteInfo(RouteTypes eRouteNum);

	DllExport int getNumImprovementInfos();
	DllExport std::vector<CvImprovementInfo*>& getImprovementInfo();
	DllExport CvImprovementInfo& getImprovementInfo(ImprovementTypes eImprovementNum);

	DllExport int getNumGoodyInfos();
	DllExport std::vector<CvGoodyInfo*>& getGoodyInfo();
	DllExport CvGoodyInfo& getGoodyInfo(GoodyTypes eGoodyNum);

	DllExport int getNumBuildInfos();
	DllExport std::vector<CvBuildInfo*>& getBuildInfo();
	DllExport CvBuildInfo& getBuildInfo(BuildTypes eBuildNum);

	DllExport int getNumHandicapInfos();
	DllExport std::vector<CvHandicapInfo*>& getHandicapInfo();
	DllExport CvHandicapInfo& getHandicapInfo(HandicapTypes eHandicapNum);

	DllExport int getNumGameSpeedInfos();
	DllExport std::vector<CvGameSpeedInfo*>& getGameSpeedInfo();
	DllExport CvGameSpeedInfo& getGameSpeedInfo(GameSpeedTypes eGameSpeedNum);

	DllExport int getNumAlarmInfos();
	DllExport std::vector<CvAlarmInfo*>& getAlarmInfo();
	DllExport CvAlarmInfo& getAlarmInfo(AlarmTypes eAlarm);

	DllExport int getNumTurnTimerInfos();
	DllExport std::vector<CvTurnTimerInfo*>& getTurnTimerInfo();
	DllExport CvTurnTimerInfo& getTurnTimerInfo(TurnTimerTypes eTurnTimerNum);

	DllExport int getNumBuildingClassInfos();
	DllExport std::vector<CvBuildingClassInfo*>& getBuildingClassInfo();
	DllExport CvBuildingClassInfo& getBuildingClassInfo(BuildingClassTypes eBuildingClassNum);

	DllExport int getNumBuildingInfos();
	DllExport std::vector<CvBuildingInfo*>& getBuildingInfo();
	DllExport CvBuildingInfo& getBuildingInfo(BuildingTypes eBuildingNum);

	DllExport int getNumSpecialBuildingInfos();
	DllExport std::vector<CvSpecialBuildingInfo*>& getSpecialBuildingInfo();
	DllExport CvSpecialBuildingInfo& getSpecialBuildingInfo(SpecialBuildingTypes eSpecialBuildingNum);

	DllExport int getNumUnitClassInfos();
	DllExport std::vector<CvUnitClassInfo*>& getUnitClassInfo();
	DllExport CvUnitClassInfo& getUnitClassInfo(UnitClassTypes eUnitClassNum);

	DllExport int getNumActionInfos();
	DllExport std::vector<CvActionInfo*>& getActionInfo();
	DllExport CvActionInfo& getActionInfo(int i);

	DllExport std::vector<CvMissionInfo*>& getMissionInfo();
	DllExport CvMissionInfo& getMissionInfo(MissionTypes eMissionNum);

	DllExport std::vector<CvControlInfo*>& getControlInfo();
	DllExport CvControlInfo& getControlInfo(ControlTypes eControlNum);

	DllExport std::vector<CvCommandInfo*>& getCommandInfo();
	DllExport CvCommandInfo& getCommandInfo(CommandTypes eCommandNum);

	DllExport int getNumAutomateInfos();
	DllExport std::vector<CvAutomateInfo*>& getAutomateInfo();
	DllExport CvAutomateInfo& getAutomateInfo(int iAutomateNum);

	DllExport int getNumPromotionInfos();
	DllExport std::vector<CvPromotionInfo*>& getPromotionInfo();
	DllExport CvPromotionInfo& getPromotionInfo(PromotionTypes ePromotionNum);

	DllExport int getNumProfessionInfos();
	DllExport std::vector<CvProfessionInfo*>& getProfessionInfo();
	DllExport CvProfessionInfo& getProfessionInfo(ProfessionTypes eProfessionNum);

	DllExport int getNumCivicOptionInfos();
	DllExport std::vector<CvInfoBase*>& getCivicOptionInfo();
	DllExport CvInfoBase& getCivicOptionInfo(CivicOptionTypes eCivicOptionNum);

	DllExport int getNumCivicInfos();
	DllExport std::vector<CvCivicInfo*>& getCivicInfo();
	DllExport CvCivicInfo& getCivicInfo(CivicTypes eCivicNum);

	DllExport int getNumDiplomacyInfos();
	DllExport std::vector<CvDiplomacyInfo*>& getDiplomacyInfo();
	DllExport	CvDiplomacyInfo& getDiplomacyInfo(int iDiplomacyNum);

	DllExport int getNumEraInfos();
	DllExport std::vector<CvEraInfo*>& getEraInfo();
	DllExport	CvEraInfo& getEraInfo(EraTypes eEraNum);

	DllExport int getNumHurryInfos();
	DllExport std::vector<CvHurryInfo*>& getHurryInfo();
	DllExport	CvHurryInfo& getHurryInfo(HurryTypes eHurryNum);

	DllExport int getNumEmphasizeInfos();
	DllExport std::vector<CvEmphasizeInfo*>& getEmphasizeInfo();
	DllExport	CvEmphasizeInfo& getEmphasizeInfo(EmphasizeTypes eEmphasizeNum);

	DllExport int getNumCultureLevelInfos();
	DllExport std::vector<CvCultureLevelInfo*>& getCultureLevelInfo();
	DllExport	CvCultureLevelInfo& getCultureLevelInfo(CultureLevelTypes eCultureLevelNum);

	DllExport int getNumVictoryInfos();
	DllExport std::vector<CvVictoryInfo*>& getVictoryInfo();
	DllExport	CvVictoryInfo& getVictoryInfo(VictoryTypes eVictoryNum);

	DllExport int getNumEventTriggerInfos();
	DllExport std::vector<CvEventTriggerInfo*>& getEventTriggerInfo();
	DllExport CvEventTriggerInfo& getEventTriggerInfo(EventTriggerTypes eEventTrigger);

	DllExport int getNumEventInfos();
	DllExport std::vector<CvEventInfo*>& getEventInfo();
	DllExport CvEventInfo& getEventInfo(EventTypes eEvent);

	//Androrc UnitArtStyles
	DllExport int getNumUnitArtStyleTypeInfos();
	std::vector<CvUnitArtStyleTypeInfo*>& getUnitArtStyleTypeInfo();
	DllExport CvUnitArtStyleTypeInfo& getUnitArtStyleTypeInfo(UnitArtStyleTypes eUnitArtStyleTypeNum);
	//Androrc End
	DllExport int getNUM_ENGINE_DIRTY_BITS() const;
	DllExport int getNUM_INTERFACE_DIRTY_BITS() const;
	DllExport int getNUM_YIELD_TYPES() const;
	DllExport int getNUM_FORCECONTROL_TYPES() const;
	DllExport int getNUM_INFOBAR_TYPES() const;
	DllExport int getNUM_HEALTHBAR_TYPES() const;
	DllExport int getNUM_CONTROL_TYPES() const;
	DllExport int getNUM_LEADERANIM_TYPES() const;

	DllExport int& getNumEntityEventTypes();
	CvString*& getEntityEventTypes();
	DllExport CvString& getEntityEventTypes(EntityEventTypes e);

	DllExport int& getNumAnimationOperatorTypes();
	CvString*& getAnimationOperatorTypes();
	DllExport CvString& getAnimationOperatorTypes(AnimationOperatorTypes e);

	CvString*& getFunctionTypes();
	DllExport CvString& getFunctionTypes(FunctionTypes e);

	DllExport int& getNumArtStyleTypes();
	CvString*& getArtStyleTypes();
	DllExport CvString& getArtStyleTypes(ArtStyleTypes e);

	DllExport int& getNumCitySizeTypes();
	CvString*& getCitySizeTypes();
	DllExport CvString& getCitySizeTypes(int i);

	CvString*& getContactTypes();
	DllExport CvString& getContactTypes(ContactTypes e);

	CvString*& getDiplomacyPowerTypes();
	DllExport CvString& getDiplomacyPowerTypes(DiplomacyPowerTypes e);

	CvString*& getAutomateTypes();
	DllExport CvString& getAutomateTypes(AutomateTypes e);

	CvString*& getDirectionTypes();
	DllExport CvString& getDirectionTypes(AutomateTypes e);

	DllExport int& getNumFootstepAudioTypes();
	CvString*& getFootstepAudioTypes();
	DllExport CvString& getFootstepAudioTypes(int i);
	DllExport int getFootstepAudioTypeByTag(CvString strTag);

	CvString*& getFootstepAudioTags();
	DllExport CvString& getFootstepAudioTags(int i);

	CvString& getCurrentXMLFile();
	void setCurrentXMLFile(const TCHAR* szFileName);

	//
	///////////////// BEGIN global defines
	// THESE ARE READ-ONLY
	//

	DllExport FVariableSystem* getDefinesVarSystem();
	DllExport void cacheGlobals();

	// ***** EXPOSED TO PYTHON *****
	DllExport int getDefineINT( const char * szName ) const;
	DllExport float getDefineFLOAT( const char * szName ) const;
	DllExport const char * getDefineSTRING( const char * szName ) const;
	DllExport void setDefineINT( const char * szName, int iValue );
	DllExport void setDefineFLOAT( const char * szName, float fValue );
	DllExport void setDefineSTRING( const char * szName, const char * szValue );

	DllExport int getMOVE_DENOMINATOR();
	DllExport int getFOOD_CONSUMPTION_PER_POPULATION();
	DllExport int getMAX_HIT_POINTS();
	DllExport int getHILLS_EXTRA_DEFENSE();
	DllExport int getRIVER_ATTACK_MODIFIER();
	DllExport int getAMPHIB_ATTACK_MODIFIER();
	DllExport int getHILLS_EXTRA_MOVEMENT();
	DllExport int getPEAK_EXTRA_MOVEMENT();
	DllExport int getMAX_PLOT_LIST_ROWS();
	DllExport int getUNIT_MULTISELECT_MAX();
	DllExport int getEVENT_MESSAGE_TIME();
	DllExport int getROUTE_FEATURE_GROWTH_MODIFIER();
	DllExport int getFEATURE_GROWTH_MODIFIER();
	DllExport int getMIN_CITY_RANGE();
	DllExport int getCITY_MAX_NUM_BUILDINGS();
	DllExport int getLAKE_MAX_AREA_SIZE();
	DllExport int getMIN_WATER_SIZE_FOR_OCEAN();
	DllExport int getFORTIFY_MODIFIER_PER_TURN();
	DllExport int getMAX_CITY_DEFENSE_DAMAGE();
	DllExport int getPEAK_SEE_THROUGH_CHANGE();
	DllExport int getHILLS_SEE_THROUGH_CHANGE();
	DllExport int getSEAWATER_SEE_FROM_CHANGE();
	DllExport int getPEAK_SEE_FROM_CHANGE();
	DllExport int getHILLS_SEE_FROM_CHANGE();
	DllExport int getMAX_REBEL_YIELD_MODIFIER();
	// TAC - AI Improved Navel AI - koma13 - START
	DllExport int getAI_TRANSPORT_DANGER_RANGE();
	DllExport int getAI_LOST_TRANSPORT_MEMORY_COUNT();
	// TAC - AI Improved Navel AI - koma13 - END
	DllExport int getNEW_CAPACITY(); //VET NewCapacity - 1/2

	// R&R, ray, caching globals from Global Defines Alt - START
	int getPLOT_VISIBILITY_RANGE();
	int getUNIT_VISIBILITY_RANGE();
	int getMIN_CITY_YIELD_DECAY();
	int getCITY_YIELD_DECAY_PERCENT();
	int getIMMIGRATION_THRESHOLD();
	int getIMMIGRATION_THRESHOLD_INCREASE();
	int getTAX_TRADE_THRESHOLD();
	int getTAX_TRADE_THRESHOLD_TAX_RATE_PERCENT();
	int getTAX_INCREASE_CHANCE();
	// TAC reduced REF
	int getENABLE_REDUCED_REF_FOR_AI();
	int getREDUCED_REF_PERCENT();
	// Domestic Market
	int getPRICE_DIFF_EUROPE_DOMESTIC_LUXURY_GOODS();
	int getDOMESTIC_SALES_MESSAGES();
	// Wild Animals
	int getWILD_ANIMAL_LAND_TERRAIN_NATIVE_WEIGHT();
	int getWILD_ANIMAL_LAND_UNIT_VARIATION_WEIGHT();
	int getWILD_ANIMAL_SEA_TERRAIN_NATIVE_WEIGHT();
	int getWILD_ANIMAL_SEA_UNIT_VARIATION_WEIGHT();
	int getWILD_ANIMAL_REWARD_RANDOM_BASE();
	// Max Cross Limit
	int getIMMIGRATION_MAX_CROSS_LIMIT();
	// NBMOD REF
	int getNBMOD_REF_ENABLE();
	int getNBMOD_REF_RANDOM_SHIPS();
	int getNBMOD_REF_REAL_WEAPONS();
	int getNBMOD_REF_WEAPONS_BONUS();
	int getNBMOD_REF_REVOLUTION_PERCENT_ENABLE();
	int getNBMOD_REF_MAX_TAX_ENABLE();
	int getNBMOD_REF_WEAPONS_BONUS_PERCENT();
	int getNBMOD_REF_HORSES_BONUS_PERCENT();
	float getNBMOD_REF_WEAPONS_WEIGHT();
	float getNBMOD_REF_HORSES_WEIGHT();
	int getNBMOD_REF_NUM_UNITS_PERCENT();
	float getNBMOD_REF_POPULATION_WEIGHT();
	float getNBMOD_REF_TURN_WEIGHT();
	float getNBMOD_REF_KING_PANIC_WEIGHT();
	int getNBMOD_REF_START_VALUE_PERCENT();
	int getNBMOD_REF_MAP_COEFFICIENT();
	int getNBMOD_REF_DEBUG();
	// NBMOD Education
	int getEDUCATION_THRESHOLD_INCREASE();
	int getEDUCATION_THRESHOLD();
	// TAC Education
	int getUSE_TAC_EDUCATION_COST_SYSTEM();
	float getTAC_EDUCATION_RANK1();
	float getTAC_EDUCATION_RANK2();
	float getTAC_EDUCATION_RANK3();
	// NBMod Tax
	DllExport int getMAX_TAX_RATE();
	DllExport int getMIN_TAX_RATE();
	DllExport int getINCREASE_MAX_TAX_RATE();
	DllExport int getDECREASE_MAX_TAX_RATE();
	// TAC AI hurries Immigrants
	int getAI_HURRY_IMMIGRANTS();
	// TAC LbD
	int getLBD_BASE_CHANCE_EXPERT();
	int getLBD_CHANCE_INCREASE_EXPERT();
	int getLBD_PRE_ROUNDS_EXPERT();
	int getLBD_KI_MOD_EXPERT();
	int getLBD_BASE_CHANCE_FREE();
	int getLBD_CHANCE_INCREASE_FREE();
	int getLBD_PRE_ROUNDS_FREE();
	int getLBD_CHANCE_MOD_FREE_CRIMINAL();
	int getLBD_CHANCE_MOD_FREE_SERVANT();
	int getLBD_CHANCE_ESCAPE();
	int getLBD_CHANCE_MOD_ESCAPE_CRIMINAL();
	int getLBD_CHANCE_MOD_ESCAPE_SERVANT();
	// R&R, ray, getting Veterans or Free through Combat Experience
	int getLBD_EXPERIENCE_VETERAN();
	int getLBD_EXPERIENCE_FREE();
	// TAC price diff raw to produced
	int getPRICE_DIFF_MAN_TO_RAW();
	// TAC KI patch
	int getAI_REBELS_FOR_REVOLUTION();
	int getUNIT_REVOLUTION_RATIO();
	int getAI_FREE_CITY_DEFENDERS();
	// TAC capturing Ships
	int getBASE_CHANCE_CAPTURING_SHIPS();
	// Min Rounds and Base Chances for DLL-Diplo-Events
	int getMIN_ROUND_NATIVE_MERCS();
	int getMIN_ROUND_EUROPE_WARS();
	int getBASE_CHANCE_EUROPE_WARS();
	int getBASE_CHANCE_EUROPE_PEACE();
	int getMIN_ROUND_PRISONS_CROWDED();
	int getPRISONS_CROWDED_CHANCE();
	int getMIN_ROUND_REVOLUTIONARY_NOBLE();
	int getREVOLUTIONARY_NOBLE_CHANCE();
	int getMIN_ROUND_SMUGGLERS_AVAILABLE();
	int getBASE_CHANCE_SMUGGLERS_AVAILABLE();
	int getBASE_CHANCE_RANGERS_AVAILABLE();
	int getBASE_CHANCE_CONQUISTADORS_AVAILABLE();
	int getMIN_ROUND_NATIVE_SLAVE();
	int getMIN_ROUND_AFRICAN_SLAVES();
	int getAFRICAN_SLAVES_CHANCE();
	int getMIN_ROUND_BISHOP();
	int getBISHOP_CHANCE();
	int getMIN_ROUND_PIRATES();
	int getBASE_CHANCE_PIRATES();
	int getBASE_CHANCE_CONTINENTAL_GUARD();
	int getBASE_CHANCE_MORTAR();
	int getCHURCH_CONTACT_CHANCE();
	int getMIN_ROUND_CHURCH_CONTACT();	
	int getBASE_CHANCE_CHURCH_WAR();
	int getMIN_ROUND_CHURCH_WAR();
	int getPRICE_MILITIA();
	int getMIN_CITY_SIZE_MILITIA_OR_UNREST();
	int getCHANCE_MILITIA();
	int getCHANCE_UNREST();
	int getMIN_ROUND_STEALING_IMMIGRANT();
	int getBASE_CHANCE_STEALING_IMMIGRANT();
	int getTIMER_STEALING_IMMIGRANT();
	// Timers for DLL-Diplo-Events
	int getTIMER_NATIVE_MERC();
	int getTIMER_EUROPEAN_WARS();
	int getTIMER_EUROPEAN_PEACE();
	int getTIMER_PRISONS_CROWDED();
	int getTIMER_REVOLUTIONARY_NOBLE();
	int getTIMER_BISHOP();
	int getTIMER_CHURCH_DEMAND();
	int getTIMER_CHURCH_WAR();
	int getTIMER_SMUGGLING_SHIP();
	int getTIMER_RANGER();
	int getTIMER_CONQUISTADOR();
	int getTIMER_PIRATES();
	int getTIMER_CONTINENTAL_GUARD();
	int getTIMER_MORTAR();
	int getTIMER_NATIVE_SLAVE();
	int getTIMER_AFRICAN_SLAVE();
	// Native Raids
	int getNATIVE_POTENTIAL_RAID_TARGET_THRESHOLD();
	int getNATIVE_GOODS_RAID_PERCENT();
	int getRANDOM_NATIVE_RAID_BASECHANCE();
	int getNATIVE_SPARE_AI_TREASURE_CHANCE();
	// Roundwise Native Income
	int getPER_ROUND_PER_VILLAGE_INCOME_MAX();
	int getPER_VILLAGE_FEATURE_GENERATION_LIMIT();
	// New Storage
	int getCUSTOMHOUSE_STRATEGIC_RAW_SELL_THRESHOLD();
	int getCUSTOMHOUSE_STRATEGIC_PRODUCED_SELL_THRESHOLD();
	int getCUSTOMHOUSE_RAW_SELL_THRESHOLD();
	int getCUSTOMHOUSE_PRODUCED_SELL_THRESHOLD();
	// Improvement TAX mechanism
	int getTAX_TRADE_INCREASE_CHANCE_KING_ATTITUDE_BASE();
	// R&R, ray, caching globals from Global Defines Alt - END

	// cache ship profession - start - Nightinggale
	int getPROFESSION_WHALING_BOAT_WORKING();
	int getPROFESSION_FISHING_BOAT_WORKING();
	// cache ship profession - end - Nightinggale

	// R&R, ray, enhanced caching Whaling and Fishing - START
	int getUNITCLASS_WHALING_BOAT();
	int getUNITCLASS_FISHING_BOAT();
	// R&R, ray, enhanced caching Whaling and Fishing - END

	// R&R, ray, Health - START
	int getMIN_POP_NEG_HEALTH();
	int getPOP_DIVISOR_NEG_HEALTH();
	int getMAX_CITY_HEALTH();
	int getLOWEST_CITY_HEALTH();
	// R&R, ray, Health - END

	DllExport float getCAMERA_MIN_YAW();
	DllExport float getCAMERA_MAX_YAW();
	DllExport float getCAMERA_FAR_CLIP_Z_HEIGHT();
	DllExport float getCAMERA_MAX_TRAVEL_DISTANCE();
	DllExport float getCAMERA_START_DISTANCE();
	DllExport float getPLOT_SIZE();
	DllExport float getCAMERA_SPECIAL_PITCH();
	DllExport float getCAMERA_MAX_TURN_OFFSET();
	DllExport float getCAMERA_MIN_DISTANCE();
	DllExport float getCAMERA_UPPER_PITCH();
	DllExport float getCAMERA_LOWER_PITCH();
	DllExport float getFIELD_OF_VIEW();
	DllExport float getUNIT_MULTISELECT_DISTANCE();

	DllExport int getUSE_CAN_FOUND_CITIES_ON_WATER_CALLBACK();
	DllExport int getUSE_CANNOT_DO_CIVIC_CALLBACK();
	DllExport int getUSE_CAN_DO_CIVIC_CALLBACK();
	DllExport int getUSE_CANNOT_CONSTRUCT_CALLBACK();
	DllExport int getUSE_CAN_CONSTRUCT_CALLBACK();
	DllExport int getUSE_CAN_DECLARE_WAR_CALLBACK();
	DllExport int getUSE_GET_UNIT_COST_MOD_CALLBACK();
	DllExport int getUSE_GET_BUILDING_COST_MOD_CALLBACK();
	DllExport int getUSE_GET_CITY_FOUND_VALUE_CALLBACK();
	DllExport int getUSE_CANNOT_HANDLE_ACTION_CALLBACK();
	DllExport int getUSE_CAN_BUILD_CALLBACK();
	DllExport int getUSE_CANNOT_TRAIN_CALLBACK();
	DllExport int getUSE_CAN_TRAIN_CALLBACK();
	DllExport int getUSE_UNIT_CANNOT_MOVE_INTO_CALLBACK();
	DllExport int getUSE_FINISH_TEXT_CALLBACK();
	DllExport int getUSE_ON_UNIT_SET_XY_CALLBACK();
	DllExport int getUSE_ON_UNIT_SELECTED_CALLBACK();
	DllExport int getUSE_ON_MISSIONARY_CONVERTED_UNIT_CALLBACK();
	DllExport int getUSE_ON_UPDATE_CALLBACK();
	DllExport int getUSE_ON_UNIT_CREATED_CALLBACK();
	DllExport int getUSE_ON_UNIT_LOST_CALLBACK();

	DllExport int getMAX_CIV_PLAYERS();
	DllExport int getMAX_PLAYERS();
	DllExport int getMAX_CIV_TEAMS();
	DllExport int getMAX_TEAMS();
	DllExport int getINVALID_PLOT_COORD();
	DllExport int getNUM_CITY_PLOTS();
	DllExport int getCITY_HOME_PLOT();

	// ***** END EXPOSED TO PYTHON *****

	////////////// END DEFINES //////////////////

	DllExport void setDLLIFace(CvDLLUtilityIFaceBase* pDll);
#ifdef _USRDLL
	CvDLLUtilityIFaceBase* getDLLIFace() { return m_pDLL; }		// inlined for perf reasons, do not use outside of dll
#endif
	DllExport CvDLLUtilityIFaceBase* getDLLIFaceNonInl();

	DllExport bool IsGraphicsInitialized() const;
	DllExport void SetGraphicsInitialized(bool bVal);

	//
	// additional accessors for initting globals
	//

	DllExport void setInterface(CvInterface* pVal);
	DllExport void setDiplomacyScreen(CvDiplomacyScreen* pVal);
	DllExport void setMPDiplomacyScreen(CMPDiplomacyScreen* pVal);
	DllExport void setMessageQueue(CMessageQueue* pVal);
	DllExport void setHotJoinMessageQueue(CMessageQueue* pVal);
	DllExport void setMessageControl(CMessageControl* pVal);
	DllExport void setSetupData(CvSetupData* pVal);
	DllExport void setMessageCodeTranslator(CvMessageCodeTranslator* pVal);
	DllExport void setDropMgr(CvDropMgr* pVal);
	DllExport void setPortal(CvPortal* pVal);
	DllExport void setStatsReport(CvStatsReporter* pVal);
	DllExport void setPathFinder(FAStar* pVal);
	DllExport void setInterfacePathFinder(FAStar* pVal);
	DllExport void setStepFinder(FAStar* pVal);
	DllExport void setRouteFinder(FAStar* pVal);
	DllExport void setBorderFinder(FAStar* pVal);
	DllExport void setAreaFinder(FAStar* pVal);

	// So that CvEnums are moddable in the DLL
	DllExport int getNumDirections() const;
	DllExport int getNumResourceLayers() const;
	DllExport int getNumUnitLayerOptionTypes() const;
	DllExport int getNumGameOptions() const;
	DllExport int getNumMPOptions() const;
	DllExport int getNumSpecialOptions() const;
	DllExport int getNumGraphicOptions() const;
	DllExport int getNumTradeableItems() const;
	DllExport int getNumBasicItems() const;
	DllExport int getNumTradeableHeadings() const;
	DllExport int getNumCommandInfos() const;
	DllExport int getNumControlInfos() const;
	DllExport int getNumMissionInfos() const;
	DllExport int getNumPlayerOptionInfos() const;
	DllExport int getMaxNumSymbols() const;
	DllExport int getNumGraphicLevels() const;

	void deleteInfoArrays();

protected:

	bool m_bGraphicsInitialized;
	bool m_bLogging;
	bool m_bRandLogging;
	bool m_bSynchLogging;
	bool m_bOverwriteLogs;
	NiPoint3  m_pt3CameraDir;
	int m_iNewPlayers;

	CMainMenu* m_pkMainMenu;

	bool m_bZoomOut;
	bool m_bZoomIn;
	bool m_bLoadGameFromFile;

	FMPIManager * m_pFMPMgr;

	CvRandom* m_asyncRand;

	CvGameAI* m_game;

	CMessageQueue* m_messageQueue;
	CMessageQueue* m_hotJoinMsgQueue;
	CMessageControl* m_messageControl;
	CvSetupData* m_setupData;
	CvInitCore* m_iniInitCore;
	CvInitCore* m_loadedInitCore;
	CvInitCore* m_initCore;
	CvMessageCodeTranslator * m_messageCodes;
	CvDropMgr* m_dropMgr;
	CvPortal* m_portal;
	CvStatsReporter * m_statsReporter;
	CvInterface* m_interface;

	CvArtFileMgr* m_pArtFileMgr;

	CvMap* m_map;

	CvDiplomacyScreen* m_diplomacyScreen;
	CMPDiplomacyScreen* m_mpDiplomacyScreen;

	FAStar* m_pathFinder;
	FAStar* m_interfacePathFinder;
	FAStar* m_stepFinder;
	FAStar* m_routeFinder;
	FAStar* m_borderFinder;
	FAStar* m_areaFinder;

	NiPoint3 m_pt3Origin;

	int* m_aiPlotDirectionX;	// [NUM_DIRECTION_TYPES];
	int* m_aiPlotDirectionY;	// [NUM_DIRECTION_TYPES];
	int* m_aiPlotCardinalDirectionX;	// [NUM_CARDINALDIRECTION_TYPES];
	int* m_aiPlotCardinalDirectionY;	// [NUM_CARDINALDIRECTION_TYPES];
	int* m_aiCityPlotX;	// [NUM_CITY_PLOTS];
	int* m_aiCityPlotY;	// [NUM_CITY_PLOTS];
	int* m_aiCityPlotPriority;	// [NUM_CITY_PLOTS];
	int m_aaiXYCityPlot[CITY_PLOTS_DIAMETER][CITY_PLOTS_DIAMETER];

	DirectionTypes* m_aeTurnLeftDirection;	// [NUM_DIRECTION_TYPES];
	DirectionTypes* m_aeTurnRightDirection;	// [NUM_DIRECTION_TYPES];
	DirectionTypes m_aaeXYDirection[DIRECTION_DIAMETER][DIRECTION_DIAMETER];

	//InterfaceModeInfo m_aInterfaceModeInfo[NUM_INTERFACEMODE_TYPES] =
	std::vector<CvInterfaceModeInfo*> m_paInterfaceModeInfo;

	/***********************************************************************************************************************
	Globals loaded from XML
	************************************************************************************************************************/

	// all type strings are upper case and are kept in this hash map for fast lookup, Moose
	typedef stdext::hash_map<std::string /* type string */, int /* info index */> InfosMap;
	InfosMap m_infosMap;
	std::vector<std::vector<CvInfoBase *> *> m_aInfoVectors;

	std::vector<CvColorInfo*> m_paColorInfo;
	std::vector<CvPlayerColorInfo*> m_paPlayerColorInfo;
	std::vector<CvInfoBase*> m_paHints;
	std::vector<CvMainMenuInfo*> m_paMainMenus;
	std::vector<CvTerrainInfo*> m_paTerrainInfo;
	std::vector<CvLandscapeInfo*> m_paLandscapeInfo;
	int m_iActiveLandscapeID;
	std::vector<CvWorldInfo*> m_paWorldInfo;
	std::vector<CvClimateInfo*> m_paClimateInfo;
	std::vector<CvSeaLevelInfo*> m_paSeaLevelInfo;
	std::vector<CvEuropeInfo*> m_paEuropeInfo;
	std::vector<CvYieldInfo*> m_paYieldInfo;
	std::vector<CvRouteInfo*> m_paRouteInfo;
	std::vector<CvFeatureInfo*> m_paFeatureInfo;
	std::vector<CvBonusInfo*> m_paBonusInfo;
	std::vector<CvImprovementInfo*> m_paImprovementInfo;
	std::vector<CvGoodyInfo*> m_paGoodyInfo;
	std::vector<CvBuildInfo*> m_paBuildInfo;
	std::vector<CvHandicapInfo*> m_paHandicapInfo;
	std::vector<CvGameSpeedInfo*> m_paGameSpeedInfo;
	std::vector<CvAlarmInfo*> m_paAlarmInfo;
	std::vector<CvTurnTimerInfo*> m_paTurnTimerInfo;
	std::vector<CvCivilizationInfo*> m_paCivilizationInfo;
	std::vector<CvLeaderHeadInfo*> m_paLeaderHeadInfo;
	std::vector<CvTraitInfo*> m_paTraitInfo;
	std::vector<CvCursorInfo*> m_paCursorInfo;
	std::vector<CvSlideShowInfo*> m_paSlideShowInfo;
	std::vector<CvSlideShowRandomInfo*> m_paSlideShowRandomInfo;
	std::vector<CvWorldPickerInfo*> m_paWorldPickerInfo;
	std::vector<CvBuildingClassInfo*> m_paBuildingClassInfo;
	std::vector<CvBuildingInfo*> m_paBuildingInfo;
	std::vector<CvSpecialBuildingInfo*> m_paSpecialBuildingInfo;
	std::vector<CvUnitClassInfo*> m_paUnitClassInfo;
	std::vector<CvUnitInfo*> m_paUnitInfo;
	std::vector<CvSpecialUnitInfo*> m_paSpecialUnitInfo;
	std::vector<CvInfoBase*> m_paConceptInfo;
	std::vector<CvInfoBase*> m_paCalendarInfo;
	std::vector<CvInfoBase*> m_paSeasonInfo;
	std::vector<CvInfoBase*> m_paMonthInfo;
	std::vector<CvInfoBase*> m_paDenialInfo;
	std::vector<CvInfoBase*> m_paInvisibleInfo;
	std::vector<CvFatherInfo*> m_paFatherInfo;
	std::vector<CvFatherPointInfo*> m_paFatherPointInfo;
	std::vector<CvInfoBase*> m_paUnitCombatInfo;
	std::vector<CvInfoBase*> m_paDomainInfo;
	std::vector<CvInfoBase*> m_paUnitAIInfos;
	std::vector<CvInfoBase*> m_paAttitudeInfos;
	std::vector<CvInfoBase*> m_paMemoryInfos;
	std::vector<CvInfoBase*> m_paFatherCategoryInfos;
	std::vector<CvInfoBase*> m_paFeatInfos;
	std::vector<CvGameOptionInfo*> m_paGameOptionInfos;
	std::vector<CvMPOptionInfo*> m_paMPOptionInfos;
	std::vector<CvForceControlInfo*> m_paForceControlInfos;
	std::vector<CvPlayerOptionInfo*> m_paPlayerOptionInfos;
	std::vector<CvGraphicOptionInfo*> m_paGraphicOptionInfos;
	std::vector<CvEmphasizeInfo*> m_paEmphasizeInfo;
	std::vector<CvCultureLevelInfo*> m_paCultureLevelInfo;
	std::vector<CvActionInfo*> m_paActionInfo;
	std::vector<CvMissionInfo*> m_paMissionInfo;
	std::vector<CvControlInfo*> m_paControlInfo;
	std::vector<CvCommandInfo*> m_paCommandInfo;
	std::vector<CvAutomateInfo*> m_paAutomateInfo;
	std::vector<CvPromotionInfo*> m_paPromotionInfo;
	std::vector<CvProfessionInfo*> m_paProfessionInfo;
	std::vector<CvInfoBase*> m_paCivicOptionInfo;
	std::vector<CvCivicInfo*> m_paCivicInfo;
	std::vector<CvDiplomacyInfo*> m_paDiplomacyInfo;
	std::vector<CvEraInfo*> m_aEraInfo;	// [NUM_ERA_TYPES];
	std::vector<CvHurryInfo*> m_paHurryInfo;
	std::vector<CvVictoryInfo*> m_paVictoryInfo;
	std::vector<CvRouteModelInfo*> m_paRouteModelInfo;
	std::vector<CvRiverModelInfo*> m_paRiverModelInfo;
	std::vector<CvWaterPlaneInfo*> m_paWaterPlaneInfo;
	std::vector<CvTerrainPlaneInfo*> m_paTerrainPlaneInfo;
	std::vector<CvCameraOverlayInfo*> m_paCameraOverlayInfo;
	std::vector<CvAnimationPathInfo*> m_paAnimationPathInfo;
	std::vector<CvAnimationCategoryInfo*> m_paAnimationCategoryInfo;
	std::vector<CvEntityEventInfo*> m_paEntityEventInfo;
	std::vector<CvUnitFormationInfo*> m_paUnitFormationInfo;
	std::vector<CvEffectInfo*> m_paEffectInfo;
	std::vector<CvAttachableInfo*> m_paAttachableInfo;
	std::vector<CvEventTriggerInfo*> m_paEventTriggerInfo;
	std::vector<CvEventInfo*> m_paEventInfo;
	//Androrc UnitArtStyles
    std::vector<CvUnitArtStyleTypeInfo*> m_paUnitArtStyleTypeInfo;
	//Androrc End

	// Game Text
	std::vector<CvGameText*> m_paGameTextXML;

	//////////////////////////////////////////////////////////////////////////
	// GLOBAL TYPES
	//////////////////////////////////////////////////////////////////////////

	// XXX These are duplicates and are kept for enumeration convenience - most could be removed, Moose
	CvString *m_paszEntityEventTypes2;
	CvString *m_paszEntityEventTypes;
	int m_iNumEntityEventTypes;

	CvString *m_paszAnimationOperatorTypes;
	int m_iNumAnimationOperatorTypes;

	CvString* m_paszFunctionTypes;

	CvString *m_paszArtStyleTypes;
	int m_iNumArtStyleTypes;

	CvString *m_paszCitySizeTypes;
	int m_iNumCitySizeTypes;

	CvString *m_paszContactTypes;

	CvString *m_paszDiplomacyPowerTypes;

	CvString *m_paszAutomateTypes;

	CvString *m_paszDirectionTypes;

	CvString *m_paszFootstepAudioTypes;
	int m_iNumFootstepAudioTypes;

	CvString *m_paszFootstepAudioTags;
	int m_iNumFootstepAudioTags;

	CvString m_szCurrentXMLFile;
	//////////////////////////////////////////////////////////////////////////
	// Formerly Global Defines
	//////////////////////////////////////////////////////////////////////////

	FVariableSystem* m_VarSystem;

	int m_iMOVE_DENOMINATOR;
	int m_iFOOD_CONSUMPTION_PER_POPULATION;
	int m_iMAX_HIT_POINTS;
	int m_iHILLS_EXTRA_DEFENSE;
	int m_iRIVER_ATTACK_MODIFIER;
	int m_iAMPHIB_ATTACK_MODIFIER;
	int m_iHILLS_EXTRA_MOVEMENT;
	int m_iPEAK_EXTRA_MOVEMENT;
	int m_iMAX_PLOT_LIST_ROWS;
	int m_iUNIT_MULTISELECT_MAX;
	int m_iEVENT_MESSAGE_TIME;
	int m_iROUTE_FEATURE_GROWTH_MODIFIER;
	int m_iFEATURE_GROWTH_MODIFIER;
	int m_iMIN_CITY_RANGE;
	int m_iCITY_MAX_NUM_BUILDINGS;
	int m_iLAKE_MAX_AREA_SIZE;
	int m_iMIN_WATER_SIZE_FOR_OCEAN;
	int m_iFORTIFY_MODIFIER_PER_TURN;
	int m_iMAX_CITY_DEFENSE_DAMAGE;
	int m_iPEAK_SEE_THROUGH_CHANGE;
	int m_iHILLS_SEE_THROUGH_CHANGE;
	int m_iSEAWATER_SEE_FROM_CHANGE;
	int m_iPEAK_SEE_FROM_CHANGE;
	int m_iHILLS_SEE_FROM_CHANGE;
	int m_iMAX_REBEL_YIELD_MODIFIER;
	int m_iNEW_CAPACITY; //VET NewCapacity - 2/2
	// TAC - AI Improved Navel AI - koma13 - START
	int m_iAI_TRANSPORT_DANGER_RANGE;
	int m_iAI_LOST_TRANSPORT_MEMORY_COUNT;
	// TAC - AI Improved Navel AI - koma13 - END

	// R&R, ray, caching globals from Global Defines Alt - START
	// Caching Vanilla variables 
	int m_PLOT_VISIBILITY_RANGE;
	int m_UNIT_VISIBILITY_RANGE;
	int m_MIN_CITY_YIELD_DECAY;
	int m_CITY_YIELD_DECAY_PERCENT;
	int m_IMMIGRATION_THRESHOLD;
	int m_IMMIGRATION_THRESHOLD_INCREASE;
	int m_TAX_TRADE_THRESHOLD;
	int m_TAX_TRADE_THRESHOLD_TAX_RATE_PERCENT;
	int m_TAX_INCREASE_CHANCE;
	// TAC reduced REF
	int m_ENABLE_REDUCED_REF_FOR_AI;
	int m_REDUCED_REF_PERCENT;
	// Domestic Market
	int m_PRICE_DIFF_EUROPE_DOMESTIC_LUXURY_GOODS;
	int m_DOMESTIC_SALES_MESSAGES;
	// Wild Animals	
	int m_WILD_ANIMAL_LAND_TERRAIN_NATIVE_WEIGHT;
	int m_WILD_ANIMAL_LAND_UNIT_VARIATION_WEIGHT;
	int m_WILD_ANIMAL_SEA_TERRAIN_NATIVE_WEIGHT;
	int m_WILD_ANIMAL_SEA_UNIT_VARIATION_WEIGHT;
	int m_WILD_ANIMAL_REWARD_RANDOM_BASE;
	// Max Cross Limit
	int m_IMMIGRATION_MAX_CROSS_LIMIT;
	// NBMOD REF
	int m_NBMOD_REF_ENABLE;
	int m_NBMOD_REF_RANDOM_SHIPS;
	int m_NBMOD_REF_REAL_WEAPONS;
	int m_NBMOD_REF_WEAPONS_BONUS;
	int m_NBMOD_REF_REVOLUTION_PERCENT_ENABLE;
	int m_NBMOD_REF_MAX_TAX_ENABLE;
	int m_NBMOD_REF_WEAPONS_BONUS_PERCENT;
	int m_NBMOD_REF_HORSES_BONUS_PERCENT;
	float m_NBMOD_REF_WEAPONS_WEIGHT;
	float m_NBMOD_REF_HORSES_WEIGHT;
	int m_NBMOD_REF_NUM_UNITS_PERCENT;
	float m_NBMOD_REF_POPULATION_WEIGHT;
	float m_NBMOD_REF_TURN_WEIGHT;
	float m_NBMOD_REF_KING_PANIC_WEIGHT;
	int m_NBMOD_REF_START_VALUE_PERCENT;
	int m_NBMOD_REF_MAP_COEFFICIENT;
	int m_NBMOD_REF_DEBUG;
	// NBMOD Education
	int m_EDUCATION_THRESHOLD_INCREASE;
	int m_EDUCATION_THRESHOLD;
	// TAC Education
	int m_USE_TAC_EDUCATION_COST_SYSTEM;
	float m_TAC_EDUCATION_RANK1;
	float m_TAC_EDUCATION_RANK2;
	float m_TAC_EDUCATION_RANK3;
	// NBMod Tax
	int m_MAX_TAX_RATE;
	int m_MIN_TAX_RATE;
	int m_INCREASE_MAX_TAX_RATE;
	int m_DECREASE_MAX_TAX_RATE;
	// TAC AI hurries Immigrants
	int m_AI_HURRY_IMMIGRANTS;
	// TAC LbD
	int m_LBD_BASE_CHANCE_EXPERT;
	int m_LBD_CHANCE_INCREASE_EXPERT;
	int m_LBD_PRE_ROUNDS_EXPERT;
	int m_LBD_KI_MOD_EXPERT;
	int m_LBD_BASE_CHANCE_FREE;
	int m_LBD_CHANCE_INCREASE_FREE;
	int m_LBD_PRE_ROUNDS_FREE;
	int m_LBD_CHANCE_MOD_FREE_CRIMINAL;
	int m_LBD_CHANCE_MOD_FREE_SERVANT;
	int m_LBD_CHANCE_ESCAPE;
	int m_LBD_CHANCE_MOD_ESCAPE_CRIMINAL;
	int m_LBD_CHANCE_MOD_ESCAPE_SERVANT;
	// R&R, ray, getting Veterans or Free through Combat Experience
	int m_LBD_MIN_EXPERIENCE_VETERAN_BY_COMBAT;
	int m_LBD_MIN_EXPERIENCE_FREE_BY_COMBAT;
	// TAC price diff raw to produced
	int m_PRICE_DIFF_MAN_TO_RAW;
	// TAC KI patch
	int m_AI_REBELS_FOR_REVOLUTION;
	int m_UNIT_REVOLUTION_RATIO;
	int m_AI_FREE_CITY_DEFENDERS;
	// TAC capturing Ships
	int m_BASE_CHANCE_CAPTURING_SHIPS;
	// Min Rounds and Base Chances for DLL-Diplo-Events
	int m_MIN_ROUND_NATIVE_MERCS;
	int m_MIN_ROUND_EUROPE_WARS;
	int m_BASE_CHANCE_EUROPE_WARS;
	int m_BASE_CHANCE_EUROPE_PEACE;
	int m_MIN_ROUND_PRISONS_CROWDED;
	int m_PRISONS_CROWDED_CHANCE;
	int m_MIN_ROUND_REVOLUTIONARY_NOBLE;
	int m_REVOLUTIONARY_NOBLE_CHANCE;
	int m_MIN_ROUND_SMUGGLERS_AVAILABLE;
	int m_MIN_ROUND_PIRATES;
	int m_BASE_CHANCE_SMUGGLERS_AVAILABLE;
	int m_BASE_CHANCE_PIRATES;
	int m_BASE_CHANCE_CONTINENTAL_GUARD;
	int m_BASE_CHANCE_MORTAR;
	int m_BASE_CHANCE_RANGERS_AVAILABLE;
	int m_BASE_CHANCE_CONQUISTADORS_AVAILABLE;
	int m_MIN_ROUND_NATIVE_SLAVE;
	int m_MIN_ROUND_AFRICAN_SLAVES;
	int m_AFRICAN_SLAVES_CHANCE;
	int m_MIN_ROUND_BISHOP;
	int m_BISHOP_CHANCE;
	int m_BASE_CHANCE_CHURCH_WAR;
	int m_MIN_ROUND_CHURCH_WAR;
	int m_CHURCH_CONTACT_CHANCE;
	int m_MIN_ROUND_CHURCH_CONTACT;
	int m_PRICE_MILITIA;
	int m_MIN_CITY_SIZE_MILITIA_OR_UNREST;
	int m_CHANCE_MILITIA;
	int m_CHANCE_UNREST;
	int m_MIN_ROUND_STEALING_IMMIGRANT;
	int m_BASE_CHANCE_STEALING_IMMIGRANT;
	// Timers for DLL-Diplo-Events
	int m_TIMER_NATIVE_MERC;
	int m_TIMER_EUROPEAN_WARS;
	int m_TIMER_EUROPEAN_PEACE;
	int m_TIMER_PRISONS_CROWDED;
	int m_TIMER_REVOLUTIONARY_NOBLE;
	int m_TIMER_BISHOP;
	int m_TIMER_CHURCH_DEMAND;
	int m_TIMER_CHURCH_WAR;
	int m_TIMER_SMUGGLING_SHIP;
	int m_TIMER_PIRATES;
	int m_TIMER_CONTINENTAL_GUARD;
	int m_TIMER_MORTAR;
	int m_TIMER_RANGER;
	int m_TIMER_CONQUISTADOR;
	int m_TIMER_NATIVE_SLAVE;
	int m_TIMER_AFRICAN_SLAVE;
	int m_TIMER_STEALING_IMMIGRANT;
	// Native Raids
	int m_NATIVE_POTENTIAL_RAID_TARGET_THRESHOLD;
	int m_NATIVE_GOODS_RAID_PERCENT;
	int m_RANDOM_NATIVE_RAID_BASECHANCE;
	int m_NATIVE_SPARE_AI_TREASURE_CHANCE;
	// Roundwise Native Income
	int m_PER_ROUND_PER_VILLAGE_INCOME_MAX;
	int m_PER_VILLAGE_FEATURE_GENERATION_LIMIT;
	// New Storage
	int m_CUSTOMHOUSE_STRATEGIC_RAW_SELL_THRESHOLD;
	int m_CUSTOMHOUSE_STRATEGIC_PRODUCED_SELL_THRESHOLD;
	int m_CUSTOMHOUSE_RAW_SELL_THRESHOLD;
	int m_CUSTOMHOUSE_PRODUCED_SELL_THRESHOLD;
	// Improvement TAX mechanism
	int m_TAX_TRADE_INCREASE_CHANCE_KING_ATTITUDE_BASE;
	// R&R, ray, caching globals from Global Defines Alt - END

	// cache ship profession - start - Nightinggale
	int m_PROFESSION_WHALING_BOAT_WORKING;
	int m_PROFESSION_FISHING_BOAT_WORKING;
	int m_UNITCLASS_WHALING_BOAT; // R&R, ray
	int m_UNITCLASS_FISHING_BOAT; // R&R, ray
	// cache ship profession - end - Nightinggale

	// R&R, ray, Health - START
	int m_MIN_POP_NEG_HEALTH;
	int m_POP_DIVISOR_NEG_HEALTH;
	int m_MAX_CITY_HEALTH;
	int m_LOWEST_CITY_HEALTH;
	// R&R, ray, Health - END

	float m_fCAMERA_MIN_YAW;
	float m_fCAMERA_MAX_YAW;
	float m_fCAMERA_FAR_CLIP_Z_HEIGHT;
	float m_fCAMERA_MAX_TRAVEL_DISTANCE;
	float m_fCAMERA_START_DISTANCE;
	float m_fPLOT_SIZE;
	float m_fCAMERA_SPECIAL_PITCH;
	float m_fCAMERA_MAX_TURN_OFFSET;
	float m_fCAMERA_MIN_DISTANCE;
	float m_fCAMERA_UPPER_PITCH;
	float m_fCAMERA_LOWER_PITCH;
	float m_fFIELD_OF_VIEW;
	float m_fUNIT_MULTISELECT_DISTANCE;

	int m_iUSE_CAN_FOUND_CITIES_ON_WATER_CALLBACK;
	int m_iUSE_CANNOT_DO_CIVIC_CALLBACK;
	int m_iUSE_CAN_DO_CIVIC_CALLBACK;
	int m_iUSE_CANNOT_CONSTRUCT_CALLBACK;
	int m_iUSE_CAN_CONSTRUCT_CALLBACK;
	int m_iUSE_CAN_DECLARE_WAR_CALLBACK;
	int m_iUSE_GET_UNIT_COST_MOD_CALLBACK;
	int m_iUSE_GET_BUILDING_COST_MOD_CALLBACK;
	int m_iUSE_GET_CITY_FOUND_VALUE_CALLBACK;
	int m_iUSE_CANNOT_HANDLE_ACTION_CALLBACK;
	int m_iUSE_CAN_BUILD_CALLBACK;
	int m_iUSE_CANNOT_TRAIN_CALLBACK;
	int m_iUSE_CAN_TRAIN_CALLBACK;
	int m_iUSE_UNIT_CANNOT_MOVE_INTO_CALLBACK;
	int m_iUSE_FINISH_TEXT_CALLBACK;
	int m_iUSE_ON_UNIT_SET_XY_CALLBACK;
	int m_iUSE_ON_UNIT_SELECTED_CALLBACK;
	int m_iUSE_ON_MISSIONARY_CONVERTED_UNIT_CALLBACK;
	int m_iUSE_ON_UPDATE_CALLBACK;
	int m_iUSE_ON_UNIT_CREATED_CALLBACK;
	int m_iUSE_ON_UNIT_LOST_CALLBACK;

	// DLL interface
	CvDLLUtilityIFaceBase* m_pDLL;
};

extern CvGlobals gGlobals;	// for debugging

//
// inlines
//
inline CvGlobals& CvGlobals::getInstance()
{
	return gGlobals;
}


//
// helpers
//
#define GC CvGlobals::getInstance()
#ifndef _USRDLL
#define gDLL GC.getDLLIFaceNonInl()
#else
#define gDLL GC.getDLLIFace()
#endif

#ifndef _USRDLL
#define NUM_DIRECTION_TYPES (GC.getNumDirections())
#define NUM_RESOURCE_LAYERS (GC.getNumResourceLayers())
#define NUM_UNIT_LAYER_OPTION_TYPES (GC.getNumUnitLayerOptionTypes())
#define NUM_GAMEOPTION_TYPES (GC.getNumGameOptions())
#define NUM_MPOPTION_TYPES (GC.getNumMPOptions())
#define NUM_SPECIALOPTION_TYPES (GC.getNumSpecialOptions())
#define NUM_GRAPHICOPTION_TYPES (GC.getNumGraphicOptions())
#define NUM_TRADEABLE_ITEMS (GC.getNumTradeableItems())
#define NUM_BASIC_ITEMS (GC.getNumBasicItems())
#define NUM_TRADEABLE_HEADINGS (GC.getNumTradeableHeadings())
#define NUM_COMMAND_TYPES (GC.getNumCommandInfos())
#define NUM_CONTROL_TYPES (GC.getNumControlInfos())
#define NUM_MISSION_TYPES (GC.getNumMissionInfos())
#define NUM_PLAYEROPTION_TYPES (GC.getNumPlayerOptionInfos())
#define MAX_NUM_SYMBOLS (GC.getMaxNumSymbols())
#define NUM_GRAPHICLEVELS (GC.getNumGraphicLevels())
#endif

//helper functions
template <class T>
void deleteInfoArray(std::vector<T*>& array)
{
	for (std::vector<T*>::iterator it = array.begin(); it != array.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	array.clear();
}

template <class T>
bool readInfoArray(FDataStreamBase* pStream, std::vector<T*>& array, const char* szClassName)
{
	GC.addToInfosVectors(&array);

	int iSize;
	pStream->Read(&iSize);
	FAssertMsg(iSize==sizeof(T), CvString::format("class size doesn't match cache size - check info read/write functions:%s", szClassName).c_str());
	if (iSize!=sizeof(T))
		return false;
	pStream->Read(&iSize);

	deleteInfoArray(array);

	for (int i = 0; i < iSize; ++i)
	{
		array.push_back(new T);
	}

	int iIndex = 0;
	for (std::vector<T*>::iterator it = array.begin(); it != array.end(); ++it)
	{
		(*it)->read(pStream);
		GC.setInfoTypeFromString((*it)->getType(), iIndex);
		++iIndex;
	}

	return true;
}

template <class T>
bool writeInfoArray(FDataStreamBase* pStream,  std::vector<T*>& array)
{
	int iSize = sizeof(T);
	pStream->Write(iSize);
	pStream->Write(array.size());
	for (std::vector<T*>::iterator it = array.begin(); it != array.end(); ++it)
	{
		(*it)->write(pStream);
	}
	return true;
}

#endif


// inlined functions
// The following functions are copied from CvGlobals.cpp as they are ideal candidates for inlining
// Nightinggale

inline int CvGlobals::getMOVE_DENOMINATOR()
{
	return m_iMOVE_DENOMINATOR;
}

inline int CvGlobals::getFOOD_CONSUMPTION_PER_POPULATION()
{
	return m_iFOOD_CONSUMPTION_PER_POPULATION;
}

inline int CvGlobals::getMAX_HIT_POINTS()
{
	return m_iMAX_HIT_POINTS;
}

inline int CvGlobals::getHILLS_EXTRA_DEFENSE()
{
	return m_iHILLS_EXTRA_DEFENSE;
}

inline int CvGlobals::getRIVER_ATTACK_MODIFIER()
{
	return m_iRIVER_ATTACK_MODIFIER;
}

inline int CvGlobals::getAMPHIB_ATTACK_MODIFIER()
{
	return m_iAMPHIB_ATTACK_MODIFIER;
}

inline int CvGlobals::getHILLS_EXTRA_MOVEMENT()
{
	return m_iHILLS_EXTRA_MOVEMENT;
}

inline int CvGlobals::getPEAK_EXTRA_MOVEMENT()
{
	return m_iPEAK_EXTRA_MOVEMENT;
}

inline int CvGlobals::getMAX_PLOT_LIST_ROWS()
{
	return m_iMAX_PLOT_LIST_ROWS;
}

inline int CvGlobals::getUNIT_MULTISELECT_MAX()
{
	return m_iUNIT_MULTISELECT_MAX;
}

inline int CvGlobals::getEVENT_MESSAGE_TIME()
{
	return m_iEVENT_MESSAGE_TIME;
}

inline int CvGlobals::getROUTE_FEATURE_GROWTH_MODIFIER()
{
	return m_iROUTE_FEATURE_GROWTH_MODIFIER;
}

inline int CvGlobals::getFEATURE_GROWTH_MODIFIER()
{
	return m_iFEATURE_GROWTH_MODIFIER;
}

inline int CvGlobals::getMIN_CITY_RANGE()
{
	return m_iMIN_CITY_RANGE;
}

inline int CvGlobals::getCITY_MAX_NUM_BUILDINGS()
{
	return m_iCITY_MAX_NUM_BUILDINGS;
}
inline int CvGlobals::getLAKE_MAX_AREA_SIZE()
{
	return m_iLAKE_MAX_AREA_SIZE;
}
inline int CvGlobals::getMIN_WATER_SIZE_FOR_OCEAN()
{
	return m_iMIN_WATER_SIZE_FOR_OCEAN;
}

inline int CvGlobals::getFORTIFY_MODIFIER_PER_TURN()
{
	return m_iFORTIFY_MODIFIER_PER_TURN;
}

inline int CvGlobals::getMAX_CITY_DEFENSE_DAMAGE()
{
	return m_iMAX_CITY_DEFENSE_DAMAGE;
}

inline int CvGlobals::getPEAK_SEE_THROUGH_CHANGE()
{
	return m_iPEAK_SEE_THROUGH_CHANGE;
}

inline int CvGlobals::getHILLS_SEE_THROUGH_CHANGE()
{
	return m_iHILLS_SEE_THROUGH_CHANGE;
}

inline int CvGlobals::getSEAWATER_SEE_FROM_CHANGE()
{
	return m_iSEAWATER_SEE_FROM_CHANGE;
}

inline int CvGlobals::getPEAK_SEE_FROM_CHANGE()
{
	return m_iPEAK_SEE_FROM_CHANGE;
}

inline int CvGlobals::getHILLS_SEE_FROM_CHANGE()
{
	return m_iHILLS_SEE_FROM_CHANGE;
}

inline int CvGlobals::getMAX_REBEL_YIELD_MODIFIER()
{
	return m_iMAX_REBEL_YIELD_MODIFIER;
}
inline int CvGlobals::getNEW_CAPACITY() //VET NewCapacity - 3/3
{
	return m_iNEW_CAPACITY;
}

// TAC - AI Improved Navel AI - koma13 - START
inline int CvGlobals::getAI_TRANSPORT_DANGER_RANGE()
{
	return m_iAI_TRANSPORT_DANGER_RANGE;
}

inline int CvGlobals::getAI_LOST_TRANSPORT_MEMORY_COUNT()
{
	return m_iAI_LOST_TRANSPORT_MEMORY_COUNT;
}
// TAC - AI Improved Navel AI - koma13 - END


// R&R, ray, caching globals from Global Defines Alt - START
// Caching Vanilla variables
inline int CvGlobals::getPLOT_VISIBILITY_RANGE()
{
	return m_PLOT_VISIBILITY_RANGE;
}
inline int CvGlobals::getUNIT_VISIBILITY_RANGE()
{
	return m_UNIT_VISIBILITY_RANGE;
}
inline int CvGlobals::getMIN_CITY_YIELD_DECAY()
{
	return m_MIN_CITY_YIELD_DECAY;
}
inline int CvGlobals::getCITY_YIELD_DECAY_PERCENT()
{
	return m_CITY_YIELD_DECAY_PERCENT;
}
inline int CvGlobals::getIMMIGRATION_THRESHOLD()
{
	return m_IMMIGRATION_THRESHOLD;
}
inline int CvGlobals::getIMMIGRATION_THRESHOLD_INCREASE()
{
	return m_IMMIGRATION_THRESHOLD_INCREASE;
}
inline int CvGlobals::getTAX_TRADE_THRESHOLD()
{
	return m_TAX_TRADE_THRESHOLD;
}
inline int CvGlobals::getTAX_TRADE_THRESHOLD_TAX_RATE_PERCENT()
{
	return m_TAX_TRADE_THRESHOLD_TAX_RATE_PERCENT;
}
inline int CvGlobals::getTAX_INCREASE_CHANCE()
{
	return m_TAX_INCREASE_CHANCE;
}
// TAC reduced REF
inline int CvGlobals::getENABLE_REDUCED_REF_FOR_AI()
{
	return m_ENABLE_REDUCED_REF_FOR_AI;
}
inline int CvGlobals::getREDUCED_REF_PERCENT()
{
	return m_REDUCED_REF_PERCENT;
}
// Domestic Market
inline int CvGlobals::getPRICE_DIFF_EUROPE_DOMESTIC_LUXURY_GOODS()
{
	return m_PRICE_DIFF_EUROPE_DOMESTIC_LUXURY_GOODS;
}
inline int CvGlobals::getDOMESTIC_SALES_MESSAGES()
{
	return m_DOMESTIC_SALES_MESSAGES;
}
// Wild Animals

inline int CvGlobals::getWILD_ANIMAL_LAND_TERRAIN_NATIVE_WEIGHT()
{
	return m_WILD_ANIMAL_LAND_TERRAIN_NATIVE_WEIGHT;
}
inline int CvGlobals::getWILD_ANIMAL_LAND_UNIT_VARIATION_WEIGHT()
{
	return m_WILD_ANIMAL_LAND_UNIT_VARIATION_WEIGHT;
}
inline int CvGlobals::getWILD_ANIMAL_SEA_TERRAIN_NATIVE_WEIGHT()
{
	return m_WILD_ANIMAL_SEA_TERRAIN_NATIVE_WEIGHT;
}
inline int CvGlobals::getWILD_ANIMAL_SEA_UNIT_VARIATION_WEIGHT()
{
	return m_WILD_ANIMAL_SEA_UNIT_VARIATION_WEIGHT;
}
inline int CvGlobals::getWILD_ANIMAL_REWARD_RANDOM_BASE()
{
	return m_WILD_ANIMAL_REWARD_RANDOM_BASE;
}
// Max Cross Limit
inline int CvGlobals::getIMMIGRATION_MAX_CROSS_LIMIT()
{
	return m_IMMIGRATION_MAX_CROSS_LIMIT;
}
// NBMOD REF
inline int CvGlobals::getNBMOD_REF_ENABLE()
{
	return m_NBMOD_REF_ENABLE;
}
inline int CvGlobals::getNBMOD_REF_RANDOM_SHIPS()
{
	return m_NBMOD_REF_RANDOM_SHIPS;
}
inline int CvGlobals::getNBMOD_REF_REAL_WEAPONS()
{
	return m_NBMOD_REF_REAL_WEAPONS;
}
inline int CvGlobals::getNBMOD_REF_WEAPONS_BONUS()
{
	return m_NBMOD_REF_WEAPONS_BONUS;
}
inline int CvGlobals::getNBMOD_REF_REVOLUTION_PERCENT_ENABLE()
{
	return m_NBMOD_REF_REVOLUTION_PERCENT_ENABLE;
}
inline int CvGlobals::getNBMOD_REF_MAX_TAX_ENABLE()
{
	return m_NBMOD_REF_MAX_TAX_ENABLE;
}
inline int CvGlobals::getNBMOD_REF_WEAPONS_BONUS_PERCENT()
{
	return m_NBMOD_REF_WEAPONS_BONUS_PERCENT;
}
inline int CvGlobals::getNBMOD_REF_HORSES_BONUS_PERCENT()
{
	return m_NBMOD_REF_HORSES_BONUS_PERCENT;
}
inline float CvGlobals::getNBMOD_REF_WEAPONS_WEIGHT()
{
	return m_NBMOD_REF_WEAPONS_WEIGHT;
}
inline float CvGlobals::getNBMOD_REF_HORSES_WEIGHT()
{
	return m_NBMOD_REF_HORSES_WEIGHT;
}
inline int CvGlobals::getNBMOD_REF_NUM_UNITS_PERCENT()
{
	return m_NBMOD_REF_NUM_UNITS_PERCENT;
}
inline float CvGlobals::getNBMOD_REF_POPULATION_WEIGHT()
{
	return m_NBMOD_REF_POPULATION_WEIGHT;
}
inline float CvGlobals::getNBMOD_REF_TURN_WEIGHT()
{
	return m_NBMOD_REF_TURN_WEIGHT;
}
inline float CvGlobals::getNBMOD_REF_KING_PANIC_WEIGHT()
{
	return m_NBMOD_REF_KING_PANIC_WEIGHT;
}
inline int CvGlobals::getNBMOD_REF_START_VALUE_PERCENT()
{
	return m_NBMOD_REF_START_VALUE_PERCENT;
}
inline int CvGlobals::getNBMOD_REF_MAP_COEFFICIENT()
{
	return m_NBMOD_REF_MAP_COEFFICIENT;
}
inline int CvGlobals::getNBMOD_REF_DEBUG()
{
	return m_NBMOD_REF_DEBUG;
}
// NBMOD Education
inline int CvGlobals::getEDUCATION_THRESHOLD_INCREASE()
{
	return m_EDUCATION_THRESHOLD_INCREASE;
}
inline int CvGlobals::getEDUCATION_THRESHOLD()
{
	return m_EDUCATION_THRESHOLD;
}
// TAC Education
inline int CvGlobals::getUSE_TAC_EDUCATION_COST_SYSTEM()
{
	return m_USE_TAC_EDUCATION_COST_SYSTEM;
}
inline float CvGlobals::getTAC_EDUCATION_RANK1()
{
	return m_TAC_EDUCATION_RANK1;
}
inline float CvGlobals::getTAC_EDUCATION_RANK2()
{
	return m_TAC_EDUCATION_RANK2;
}
inline float CvGlobals::getTAC_EDUCATION_RANK3()
{
	return m_TAC_EDUCATION_RANK3;
}
// NBMod Tax
inline int CvGlobals::getMAX_TAX_RATE()
{
	return m_MAX_TAX_RATE;
}
inline int CvGlobals::getMIN_TAX_RATE()
{
	return m_MIN_TAX_RATE;
}
inline int CvGlobals::getINCREASE_MAX_TAX_RATE()
{
	return m_INCREASE_MAX_TAX_RATE;
}
inline int CvGlobals::getDECREASE_MAX_TAX_RATE()
{
	return m_DECREASE_MAX_TAX_RATE;
}
// TAC AI hurries Immigrants
inline int CvGlobals::getAI_HURRY_IMMIGRANTS()
{
	return m_AI_HURRY_IMMIGRANTS;
}
// TAC LbD
inline int CvGlobals::getLBD_BASE_CHANCE_EXPERT()
{
	return m_LBD_BASE_CHANCE_EXPERT;
}
inline int CvGlobals::getLBD_CHANCE_INCREASE_EXPERT()
{
	return m_LBD_CHANCE_INCREASE_EXPERT;
}
inline int CvGlobals::getLBD_PRE_ROUNDS_EXPERT()
{
	return m_LBD_PRE_ROUNDS_EXPERT;
}
inline int CvGlobals::getLBD_KI_MOD_EXPERT()
{
	return m_LBD_KI_MOD_EXPERT;
}
inline int CvGlobals::getLBD_BASE_CHANCE_FREE()
{
	return m_LBD_BASE_CHANCE_FREE;
}
inline int CvGlobals::getLBD_CHANCE_INCREASE_FREE()
{
	return m_LBD_CHANCE_INCREASE_FREE;
}
inline int CvGlobals::getLBD_PRE_ROUNDS_FREE()
{
	return m_LBD_PRE_ROUNDS_FREE;
}
inline int CvGlobals::getLBD_CHANCE_MOD_FREE_CRIMINAL()
{
	return m_LBD_CHANCE_MOD_FREE_CRIMINAL;
}
inline int CvGlobals::getLBD_CHANCE_MOD_FREE_SERVANT()
{
	return m_LBD_CHANCE_MOD_FREE_SERVANT;
}
inline int CvGlobals::getLBD_CHANCE_ESCAPE()
{
	return m_LBD_CHANCE_ESCAPE;
}
inline int CvGlobals::getLBD_CHANCE_MOD_ESCAPE_CRIMINAL()
{
	return m_LBD_CHANCE_MOD_ESCAPE_CRIMINAL;
}
inline int CvGlobals::getLBD_CHANCE_MOD_ESCAPE_SERVANT()
{
	return m_LBD_CHANCE_MOD_ESCAPE_SERVANT;
}
// R&R, ray, getting Veterans or Free through Combat Experience
inline int CvGlobals::getLBD_EXPERIENCE_VETERAN()
{
	return m_LBD_MIN_EXPERIENCE_VETERAN_BY_COMBAT;
}
inline int CvGlobals::getLBD_EXPERIENCE_FREE()
{
	return m_LBD_MIN_EXPERIENCE_FREE_BY_COMBAT;
}
// TAC price diff raw to produced
inline int CvGlobals::getPRICE_DIFF_MAN_TO_RAW()
{
	return m_PRICE_DIFF_MAN_TO_RAW;
}
// TAC KI patch
inline int CvGlobals::getAI_REBELS_FOR_REVOLUTION()
{
	return m_AI_REBELS_FOR_REVOLUTION;
}
inline int CvGlobals::getUNIT_REVOLUTION_RATIO()
{
	return m_UNIT_REVOLUTION_RATIO;
}
inline int CvGlobals::getAI_FREE_CITY_DEFENDERS()
{
	return m_AI_FREE_CITY_DEFENDERS;
}
// TAC capturing Ships
inline int CvGlobals::getBASE_CHANCE_CAPTURING_SHIPS()
{
	return m_BASE_CHANCE_CAPTURING_SHIPS;
}
// Min Rounds and Base Chances for DLL-Diplo-Events
inline int CvGlobals::getMIN_ROUND_NATIVE_MERCS()
{
	return m_MIN_ROUND_NATIVE_MERCS;
}
inline int CvGlobals::getMIN_ROUND_EUROPE_WARS()
{
	return m_MIN_ROUND_EUROPE_WARS;
}
inline int CvGlobals::getBASE_CHANCE_EUROPE_WARS()
{
	return m_BASE_CHANCE_EUROPE_WARS;
}
inline int CvGlobals::getBASE_CHANCE_CHURCH_WAR()
{
	return m_BASE_CHANCE_CHURCH_WAR;
}
inline int CvGlobals::getMIN_ROUND_CHURCH_WAR()
{
	return m_MIN_ROUND_CHURCH_WAR;
}
inline int CvGlobals::getCHURCH_CONTACT_CHANCE()
{
	return m_CHURCH_CONTACT_CHANCE;
}
inline int CvGlobals::getMIN_ROUND_CHURCH_CONTACT()
{
	return m_MIN_ROUND_CHURCH_CONTACT;
}
inline int CvGlobals::getBASE_CHANCE_EUROPE_PEACE()
{
	return m_BASE_CHANCE_EUROPE_PEACE;
}
inline int CvGlobals::getMIN_ROUND_PRISONS_CROWDED()
{
	return m_MIN_ROUND_PRISONS_CROWDED;
}
inline int CvGlobals::getPRISONS_CROWDED_CHANCE()
{
	return m_PRISONS_CROWDED_CHANCE;
}
inline int CvGlobals::getMIN_ROUND_REVOLUTIONARY_NOBLE()
{
	return m_MIN_ROUND_REVOLUTIONARY_NOBLE;
}
inline int CvGlobals::getREVOLUTIONARY_NOBLE_CHANCE()
{
	return m_REVOLUTIONARY_NOBLE_CHANCE;
}
inline int CvGlobals::getMIN_ROUND_SMUGGLERS_AVAILABLE()
{
	return m_MIN_ROUND_SMUGGLERS_AVAILABLE;
}
inline int CvGlobals::getBASE_CHANCE_SMUGGLERS_AVAILABLE()
{
	return m_BASE_CHANCE_SMUGGLERS_AVAILABLE;
}
inline int CvGlobals::getMIN_ROUND_PIRATES()
{
	return m_MIN_ROUND_PIRATES;
}
inline int CvGlobals::getBASE_CHANCE_PIRATES()
{
	return m_BASE_CHANCE_PIRATES;
}
inline int CvGlobals::getBASE_CHANCE_CONTINENTAL_GUARD()
{
	return m_BASE_CHANCE_CONTINENTAL_GUARD;
}
inline int CvGlobals::getBASE_CHANCE_MORTAR()
{
	return m_BASE_CHANCE_MORTAR;
}
inline int CvGlobals::getBASE_CHANCE_RANGERS_AVAILABLE()
{
	return m_BASE_CHANCE_RANGERS_AVAILABLE;
}
inline int CvGlobals::getBASE_CHANCE_CONQUISTADORS_AVAILABLE()
{
	return m_BASE_CHANCE_CONQUISTADORS_AVAILABLE;
}
inline int CvGlobals::getMIN_ROUND_NATIVE_SLAVE()
{
	return m_MIN_ROUND_NATIVE_SLAVE;
}
inline int CvGlobals::getMIN_ROUND_AFRICAN_SLAVES()
{
	return m_MIN_ROUND_AFRICAN_SLAVES;
}
inline int CvGlobals::getAFRICAN_SLAVES_CHANCE()
{
	return m_AFRICAN_SLAVES_CHANCE;
}
inline int CvGlobals::getMIN_ROUND_BISHOP()
{
	return m_MIN_ROUND_BISHOP;
}
inline int CvGlobals::getBISHOP_CHANCE()
{
	return m_BISHOP_CHANCE;
}
inline int CvGlobals::getPRICE_MILITIA()
{
	return m_PRICE_MILITIA;
}
inline int CvGlobals::getMIN_CITY_SIZE_MILITIA_OR_UNREST()
{
	return m_MIN_CITY_SIZE_MILITIA_OR_UNREST;
}
inline int CvGlobals::getCHANCE_MILITIA()
{
	return m_CHANCE_MILITIA;
}
inline int CvGlobals::getCHANCE_UNREST()
{
	return m_CHANCE_UNREST;
}
inline int CvGlobals::getMIN_ROUND_STEALING_IMMIGRANT()
{
	return m_MIN_ROUND_STEALING_IMMIGRANT;
}
inline int CvGlobals::getBASE_CHANCE_STEALING_IMMIGRANT()
{
	return m_BASE_CHANCE_STEALING_IMMIGRANT;
}

// Timers for DLL-Diplo-Events
inline int CvGlobals::getTIMER_NATIVE_MERC()
{
	return m_TIMER_NATIVE_MERC;
}
inline int CvGlobals::getTIMER_EUROPEAN_WARS()
{
	return m_TIMER_EUROPEAN_WARS;
}
inline int CvGlobals::getTIMER_EUROPEAN_PEACE()
{
	return m_TIMER_EUROPEAN_PEACE;
}
inline int CvGlobals::getTIMER_PRISONS_CROWDED()
{
	return m_TIMER_PRISONS_CROWDED;
}
inline int CvGlobals::getTIMER_REVOLUTIONARY_NOBLE()
{
	return m_TIMER_REVOLUTIONARY_NOBLE;
}
inline int CvGlobals::getTIMER_BISHOP()
{
	return m_TIMER_BISHOP;
}
inline int CvGlobals::getTIMER_CHURCH_DEMAND()
{
	return m_TIMER_CHURCH_DEMAND;
}
inline int CvGlobals::getTIMER_CHURCH_WAR()
{
	return m_TIMER_CHURCH_WAR;
}
inline int CvGlobals::getTIMER_SMUGGLING_SHIP()
{
	return m_TIMER_SMUGGLING_SHIP;
}
inline int CvGlobals::getTIMER_PIRATES()
{
	return m_TIMER_PIRATES;
}
inline int CvGlobals::getTIMER_CONTINENTAL_GUARD()
{
	return m_TIMER_CONTINENTAL_GUARD;
}
inline int CvGlobals::getTIMER_MORTAR()
{
	return m_TIMER_MORTAR;
}
inline int CvGlobals::getTIMER_RANGER()
{
	return m_TIMER_RANGER;
}
inline int CvGlobals::getTIMER_CONQUISTADOR()
{
	return m_TIMER_CONQUISTADOR;
}
inline int CvGlobals::getTIMER_NATIVE_SLAVE()
{
	return m_TIMER_NATIVE_SLAVE;
}
inline int CvGlobals::getTIMER_AFRICAN_SLAVE()
{
	return m_TIMER_AFRICAN_SLAVE;
}
inline int CvGlobals::getTIMER_STEALING_IMMIGRANT()
{
	return m_TIMER_STEALING_IMMIGRANT;
}

// Native Raids
inline int CvGlobals::getNATIVE_POTENTIAL_RAID_TARGET_THRESHOLD()
{
	return m_NATIVE_POTENTIAL_RAID_TARGET_THRESHOLD;
}
inline int CvGlobals::getNATIVE_GOODS_RAID_PERCENT()
{
	return m_NATIVE_GOODS_RAID_PERCENT;
}
inline int CvGlobals::getRANDOM_NATIVE_RAID_BASECHANCE()
{
	return m_RANDOM_NATIVE_RAID_BASECHANCE;
}
inline int CvGlobals::getNATIVE_SPARE_AI_TREASURE_CHANCE()
{
	return m_NATIVE_SPARE_AI_TREASURE_CHANCE;
}
// Roundwise Native Income
inline int CvGlobals::getPER_ROUND_PER_VILLAGE_INCOME_MAX()
{
	return m_PER_ROUND_PER_VILLAGE_INCOME_MAX;
}
inline int CvGlobals::getPER_VILLAGE_FEATURE_GENERATION_LIMIT()
{
	return m_PER_VILLAGE_FEATURE_GENERATION_LIMIT;
}
// New Storage
inline int CvGlobals::getCUSTOMHOUSE_STRATEGIC_RAW_SELL_THRESHOLD()
{
	return m_CUSTOMHOUSE_STRATEGIC_RAW_SELL_THRESHOLD;
}
inline int CvGlobals::getCUSTOMHOUSE_STRATEGIC_PRODUCED_SELL_THRESHOLD()
{
	return m_CUSTOMHOUSE_STRATEGIC_PRODUCED_SELL_THRESHOLD;
}
inline int CvGlobals::getCUSTOMHOUSE_RAW_SELL_THRESHOLD()
{
	return m_CUSTOMHOUSE_RAW_SELL_THRESHOLD;
}
inline int CvGlobals::getCUSTOMHOUSE_PRODUCED_SELL_THRESHOLD()
{
	return m_CUSTOMHOUSE_PRODUCED_SELL_THRESHOLD;
}
// Improvement TAX mechanism
inline int CvGlobals::getTAX_TRADE_INCREASE_CHANCE_KING_ATTITUDE_BASE()
{
	return m_TAX_TRADE_INCREASE_CHANCE_KING_ATTITUDE_BASE;
}
// R&R, ray, caching globals from Global Defines Alt - END

// cache ship profession - start - Nightinggale
inline int CvGlobals::getPROFESSION_WHALING_BOAT_WORKING()
{
	return m_PROFESSION_WHALING_BOAT_WORKING;
}

inline int CvGlobals::getPROFESSION_FISHING_BOAT_WORKING()
{
	return m_PROFESSION_FISHING_BOAT_WORKING;
}
// cache ship profession - end - Nightinggale

// R&R, ray, enhanced caching Whaling and Fishing - START
inline int CvGlobals::getUNITCLASS_WHALING_BOAT()
{
	return m_UNITCLASS_WHALING_BOAT;
}

inline int CvGlobals::getUNITCLASS_FISHING_BOAT()
{
	return m_UNITCLASS_FISHING_BOAT;
}
// R&R, ray, enhanced caching Whaling and Fishing - END

// R&R, ray, Health - START
inline int CvGlobals::getMIN_POP_NEG_HEALTH()
{
	return m_MIN_POP_NEG_HEALTH;
}

inline int CvGlobals::getPOP_DIVISOR_NEG_HEALTH()
{
	return m_POP_DIVISOR_NEG_HEALTH;
}

inline int CvGlobals::getMAX_CITY_HEALTH()
{
	return m_MAX_CITY_HEALTH;
}

inline int CvGlobals::getLOWEST_CITY_HEALTH()
{
	return m_LOWEST_CITY_HEALTH;
}
// R&R, ray, Health - END

inline float CvGlobals::getCAMERA_MIN_YAW()
{
	return m_fCAMERA_MIN_YAW;
}

inline float CvGlobals::getCAMERA_MAX_YAW()
{
	return m_fCAMERA_MAX_YAW;
}

inline float CvGlobals::getCAMERA_FAR_CLIP_Z_HEIGHT()
{
	return m_fCAMERA_FAR_CLIP_Z_HEIGHT;
}

inline float CvGlobals::getCAMERA_MAX_TRAVEL_DISTANCE()
{
	return m_fCAMERA_MAX_TRAVEL_DISTANCE;
}

inline float CvGlobals::getCAMERA_START_DISTANCE()
{
	return m_fCAMERA_START_DISTANCE;
}

inline float CvGlobals::getPLOT_SIZE()
{
	return m_fPLOT_SIZE;
}

inline float CvGlobals::getCAMERA_SPECIAL_PITCH()
{
	return m_fCAMERA_SPECIAL_PITCH;
}

inline float CvGlobals::getCAMERA_MAX_TURN_OFFSET()
{
	return m_fCAMERA_MAX_TURN_OFFSET;
}

inline float CvGlobals::getCAMERA_MIN_DISTANCE()
{
	return m_fCAMERA_MIN_DISTANCE;
}

inline float CvGlobals::getCAMERA_UPPER_PITCH()
{
	return m_fCAMERA_UPPER_PITCH;
}

inline float CvGlobals::getCAMERA_LOWER_PITCH()
{
	return m_fCAMERA_LOWER_PITCH;
}

inline float CvGlobals::getFIELD_OF_VIEW()
{
	return m_fFIELD_OF_VIEW;
}

inline float CvGlobals::getUNIT_MULTISELECT_DISTANCE()
{
	return m_fUNIT_MULTISELECT_DISTANCE;
}

inline int CvGlobals::getUSE_CAN_FOUND_CITIES_ON_WATER_CALLBACK()
{
	return m_iUSE_CAN_FOUND_CITIES_ON_WATER_CALLBACK;
}
inline int CvGlobals::getUSE_CANNOT_DO_CIVIC_CALLBACK()
{
	return m_iUSE_CANNOT_DO_CIVIC_CALLBACK;
}

inline int CvGlobals::getUSE_CAN_DO_CIVIC_CALLBACK()
{
	return m_iUSE_CAN_DO_CIVIC_CALLBACK;
}

inline int CvGlobals::getUSE_CANNOT_CONSTRUCT_CALLBACK()
{
	return m_iUSE_CANNOT_CONSTRUCT_CALLBACK;
}

inline int CvGlobals::getUSE_CAN_CONSTRUCT_CALLBACK()
{
	return m_iUSE_CAN_CONSTRUCT_CALLBACK;
}

inline int CvGlobals::getUSE_CAN_DECLARE_WAR_CALLBACK()
{
	return m_iUSE_CAN_DECLARE_WAR_CALLBACK;
}
inline int CvGlobals::getUSE_GET_UNIT_COST_MOD_CALLBACK()
{
	return m_iUSE_GET_UNIT_COST_MOD_CALLBACK;
}

inline int CvGlobals::getUSE_GET_BUILDING_COST_MOD_CALLBACK()
{
	return m_iUSE_GET_BUILDING_COST_MOD_CALLBACK;
}

inline int CvGlobals::getUSE_GET_CITY_FOUND_VALUE_CALLBACK()
{
	return m_iUSE_GET_CITY_FOUND_VALUE_CALLBACK;
}

inline int CvGlobals::getUSE_CANNOT_HANDLE_ACTION_CALLBACK()
{
	return m_iUSE_CANNOT_HANDLE_ACTION_CALLBACK;
}

inline int CvGlobals::getUSE_CAN_BUILD_CALLBACK()
{
	return m_iUSE_CAN_BUILD_CALLBACK;
}

inline int CvGlobals::getUSE_CANNOT_TRAIN_CALLBACK()
{
	return m_iUSE_CANNOT_TRAIN_CALLBACK;
}

inline int CvGlobals::getUSE_CAN_TRAIN_CALLBACK()
{
	return m_iUSE_CAN_TRAIN_CALLBACK;
}

inline int CvGlobals::getUSE_UNIT_CANNOT_MOVE_INTO_CALLBACK()
{
	return m_iUSE_UNIT_CANNOT_MOVE_INTO_CALLBACK;
}

inline int CvGlobals::getUSE_FINISH_TEXT_CALLBACK()
{
	return m_iUSE_FINISH_TEXT_CALLBACK;
}

inline int CvGlobals::getUSE_ON_UNIT_SET_XY_CALLBACK()
{
	return m_iUSE_ON_UNIT_SET_XY_CALLBACK;
}

inline int CvGlobals::getUSE_ON_UNIT_SELECTED_CALLBACK()
{
	return m_iUSE_ON_UNIT_SELECTED_CALLBACK;
}

inline int CvGlobals::getUSE_ON_MISSIONARY_CONVERTED_UNIT_CALLBACK()
{
	return m_iUSE_ON_MISSIONARY_CONVERTED_UNIT_CALLBACK;
}

inline int CvGlobals::getUSE_ON_UPDATE_CALLBACK()
{
	return m_iUSE_ON_UPDATE_CALLBACK;
}

inline int CvGlobals::getUSE_ON_UNIT_CREATED_CALLBACK()
{
	return m_iUSE_ON_UNIT_CREATED_CALLBACK;
}

inline int CvGlobals::getUSE_ON_UNIT_LOST_CALLBACK()
{
	return m_iUSE_ON_UNIT_LOST_CALLBACK;
}

inline int CvGlobals::getMAX_CIV_PLAYERS()
{
	return MAX_PLAYERS;
}

inline int CvGlobals::getMAX_PLAYERS()
{
	return MAX_PLAYERS;
}

inline int CvGlobals::getMAX_CIV_TEAMS()
{
	return MAX_TEAMS;
}

inline int CvGlobals::getMAX_TEAMS()
{
	return MAX_TEAMS;
}

inline int CvGlobals::getINVALID_PLOT_COORD()
{
	return INVALID_PLOT_COORD;
}

inline int CvGlobals::getNUM_CITY_PLOTS()
{
	return NUM_CITY_PLOTS;
}

inline int CvGlobals::getCITY_HOME_PLOT()
{
	return CITY_HOME_PLOT;
}

inline void CvGlobals::setDLLIFace(CvDLLUtilityIFaceBase* pDll)
{
	m_pDLL = pDll;
}

inline int CvGlobals::getNUM_ENGINE_DIRTY_BITS() const
{
	return NUM_ENGINE_DIRTY_BITS;
}

inline int CvGlobals::getNUM_INTERFACE_DIRTY_BITS() const
{
	return NUM_INTERFACE_DIRTY_BITS;
}

inline int CvGlobals::getNUM_YIELD_TYPES() const
{
	return NUM_YIELD_TYPES;
}

inline int CvGlobals::getNUM_FORCECONTROL_TYPES() const
{
	return NUM_FORCECONTROL_TYPES;
}

inline int CvGlobals::getNUM_INFOBAR_TYPES() const
{
	return NUM_INFOBAR_TYPES;
}

inline int CvGlobals::getNUM_HEALTHBAR_TYPES() const
{
	return NUM_HEALTHBAR_TYPES;
}

inline int CvGlobals::getNUM_CONTROL_TYPES() const
{
	return NUM_CONTROL_TYPES;
}

inline int CvGlobals::getNUM_LEADERANIM_TYPES() const
{
	return NUM_LEADERANIM_TYPES;
}