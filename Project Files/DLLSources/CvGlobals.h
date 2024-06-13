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
class CvTradeScreenInfo; // trade screen type - Nightinggale

class CivEffectInfo;

#include "Profile.h"

class CvGlobals
{
//	friend class CvDLLUtilityIFace;
	friend class CvXMLLoadUtility;
public:

	// singleton accessor
	DllExport inline static CvGlobals& getInstance();

	CvGlobals();
	virtual ~CvGlobals();

	// PatchMod: Achievements START
protected:
	std::vector<CvAchieveInfo*> m_paAchieveInfo;
public:
	int getNumAchieveInfos();
	std::vector<CvAchieveInfo*>& getAchieveInfo();
	CvAchieveInfo& getAchieveInfo(AchieveTypes eAchieve);
	// PatchMod: Achievements END

	DllExport void init();
	DllExport void uninit();
	void clearTypesMap();

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
	CvStatsReporter* getStatsReporterPtr();
	DllExport CvInterface& getInterface();
	DllExport CvInterface* getInterfacePtr();
	DllExport int getMaxCivPlayers() const;
	CvMap& getMap() { return *m_map; }
	CvGameAI& getGameINLINE() { return *m_game; }			// inlined for perf reasons, do not use outside of dll
	const CvGameAI& getGameConst() const { return *m_game; }
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

	NiPoint3& getPt3Origin();

	DllExport std::vector<CvInterfaceModeInfo*>& getInterfaceModeInfo();
	DllExport CvInterfaceModeInfo& getInterfaceModeInfo(InterfaceModeTypes e);

	NiPoint3& getPt3CameraDir();

	DllExport bool& getLogging();
	DllExport bool& getRandLogging();
	DllExport bool& getSynchLogging();
	DllExport bool& overwriteLogs();

	DllExport int* getPlotDirectionX();
	DllExport int* getPlotDirectionY();
	int* getPlotCardinalDirectionX();
	int* getPlotCardinalDirectionY();
	int* getCityPlotX();
	int* getCityPlotY();
	int* getCityPlotPriority();
	int getXYCityPlot(int i, int j);
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
	std::vector<CvWorldInfo*>& getWorldInfo();
	DllExport CvWorldInfo& getWorldInfo(WorldSizeTypes e);

	DllExport int getNumClimateInfos();
	std::vector<CvClimateInfo*>& getClimateInfo();
	DllExport CvClimateInfo& getClimateInfo(ClimateTypes e);

	DllExport int getNumSeaLevelInfos();
	std::vector<CvSeaLevelInfo*>& getSeaLevelInfo();
	DllExport CvSeaLevelInfo& getSeaLevelInfo(SeaLevelTypes e);

	DllExport int getNumEuropeInfos();
	std::vector<CvEuropeInfo*>& getEuropeInfo();
	DllExport CvEuropeInfo& getEuropeInfo(EuropeTypes e);

	DllExport int getNumColorInfos();
	std::vector<CvColorInfo*>& getColorInfo();
	DllExport CvColorInfo& getColorInfo(ColorTypes e);

	DllExport int getNumPlayerColorInfos();
	std::vector<CvPlayerColorInfo*>& getPlayerColorInfo();
	DllExport CvPlayerColorInfo& getPlayerColorInfo(PlayerColorTypes e);

	DllExport int getNumHints();
	std::vector<CvInfoBase*>& getHints();
	DllExport CvInfoBase& getHints(int i);

	int getNumMainMenus();
	std::vector<CvMainMenuInfo*>& getMainMenus();
	DllExport CvMainMenuInfo& getMainMenus(int i);

	DllExport int getNumRouteModelInfos();
	std::vector<CvRouteModelInfo*>& getRouteModelInfo();
	DllExport CvRouteModelInfo& getRouteModelInfo(int i);

	DllExport int getNumRiverModelInfos();
	std::vector<CvRiverModelInfo*>& getRiverModelInfo();
	DllExport CvRiverModelInfo& getRiverModelInfo(int i);

	int getNumWaterPlaneInfos();
	std::vector<CvWaterPlaneInfo*>& getWaterPlaneInfo();
	DllExport CvWaterPlaneInfo& getWaterPlaneInfo(int i);

	DllExport int getNumTerrainPlaneInfos();
	std::vector<CvTerrainPlaneInfo*>& getTerrainPlaneInfo();
	DllExport CvTerrainPlaneInfo& getTerrainPlaneInfo(int i);

	DllExport int getNumCameraOverlayInfos();
	std::vector<CvCameraOverlayInfo*>& getCameraOverlayInfo();
	DllExport CvCameraOverlayInfo& getCameraOverlayInfo(int i);

	int getNumAnimationPathInfos();
	std::vector<CvAnimationPathInfo*>& getAnimationPathInfo();
	DllExport CvAnimationPathInfo& getAnimationPathInfo(AnimationPathTypes e);

	int getNumAnimationCategoryInfos();
	std::vector<CvAnimationCategoryInfo*>& getAnimationCategoryInfo();
	DllExport CvAnimationCategoryInfo& getAnimationCategoryInfo(AnimationCategoryTypes e);

	int getNumEntityEventInfos();
	std::vector<CvEntityEventInfo*>& getEntityEventInfo();
	DllExport CvEntityEventInfo& getEntityEventInfo(EntityEventTypes e);

	int getNumEffectInfos();
	std::vector<CvEffectInfo*>& getEffectInfo();
	DllExport CvEffectInfo& getEffectInfo(int i);

	int getNumAttachableInfos();
	std::vector<CvAttachableInfo*>& getAttachableInfo();
	DllExport CvAttachableInfo& getAttachableInfo(int i);

	DllExport int getNumUnitFormationInfos();
	std::vector<CvUnitFormationInfo*>& getUnitFormationInfo();
	DllExport CvUnitFormationInfo& getUnitFormationInfo(int i);

	int getNumGameTextXML();
	std::vector<CvGameText*>& getGameTextXML();

	int getNumLandscapeInfos();
	std::vector<CvLandscapeInfo*>& getLandscapeInfo();
	DllExport CvLandscapeInfo& getLandscapeInfo(int iIndex);
	DllExport int getActiveLandscapeID();
	DllExport void setActiveLandscapeID(int iLandscapeID);

	DllExport int getNumTerrainInfos();
	std::vector<CvTerrainInfo*>& getTerrainInfo();
	DllExport CvTerrainInfo& getTerrainInfo(TerrainTypes eTerrainNum);
	CvTerrainInfo& getInfo(TerrainTypes eTerrainNum);

	DllExport int getNumBonusInfos();
	DllExport std::vector<CvBonusInfo*>& getBonusInfo();
	DllExport CvBonusInfo& getBonusInfo(BonusTypes eBonusNum);
	CvBonusInfo& getInfo(BonusTypes eBonusNum);

	DllExport int getNumFeatureInfos();
	std::vector<CvFeatureInfo*>& getFeatureInfo();
	DllExport CvFeatureInfo& getFeatureInfo(FeatureTypes eFeatureNum);
	CvFeatureInfo& getInfo(FeatureTypes eFeatureNum);

	DllExport int getNumCivilizationInfos();
	DllExport std::vector<CvCivilizationInfo*>& getCivilizationInfo();
	DllExport CvCivilizationInfo& getCivilizationInfo(CivilizationTypes eCivilizationNum);
	CvCivilizationInfo& getInfo(CivilizationTypes eCivilizationNum);

	DllExport int getNumLeaderHeadInfos();
	DllExport std::vector<CvLeaderHeadInfo*>& getLeaderHeadInfo();
	DllExport CvLeaderHeadInfo& getLeaderHeadInfo(LeaderHeadTypes eLeaderHeadNum);

	int getNumTraitInfos();
	DllExport std::vector<CvTraitInfo*>& getTraitInfo();
	DllExport	CvTraitInfo& getTraitInfo(TraitTypes eTraitNum);

	DllExport int getNumCursorInfos();
	std::vector<CvCursorInfo*>& getCursorInfo();
	DllExport	CvCursorInfo& getCursorInfo(CursorTypes eCursorNum);

	DllExport int getNumSlideShowInfos();
	std::vector<CvSlideShowInfo*>& getSlideShowInfo();
	DllExport	CvSlideShowInfo& getSlideShowInfo(int iIndex);

	DllExport int getNumSlideShowRandomInfos();
	std::vector<CvSlideShowRandomInfo*>& getSlideShowRandomInfo();
	DllExport	CvSlideShowRandomInfo& getSlideShowRandomInfo(int iIndex);

	DllExport int getNumWorldPickerInfos();
	std::vector<CvWorldPickerInfo*>& getWorldPickerInfo();
	DllExport	CvWorldPickerInfo& getWorldPickerInfo(int iIndex);

	int getNumUnitInfos() const;
	std::vector<CvUnitInfo*>& getUnitInfo();
	CvUnitInfo& getUnitInfo(UnitTypes eUnitNum);

	int getNumSpecialUnitInfos();
	std::vector<CvSpecialUnitInfo*>& getSpecialUnitInfo();
	DllExport	CvSpecialUnitInfo& getSpecialUnitInfo(SpecialUnitTypes eSpecialUnitNum);

	int getNumConceptInfos();
	std::vector<CvInfoBase*>& getConceptInfo();
	CvInfoBase& getConceptInfo(ConceptTypes e);
	int getNumCalendarInfos();
	std::vector<CvInfoBase*>& getCalendarInfo();
	CvInfoBase& getCalendarInfo(CalendarTypes e);

	int getNumSeasonInfos();
	std::vector<CvInfoBase*>& getSeasonInfo();
	CvInfoBase& getSeasonInfo(SeasonTypes e);

	int getNumMonthInfos();
	std::vector<CvInfoBase*>& getMonthInfo();
	CvInfoBase& getMonthInfo(MonthTypes e);

	int getNumDenialInfos();
	std::vector<CvInfoBase*>& getDenialInfo();
	CvInfoBase& getDenialInfo(DenialTypes e);

	int getNumInvisibleInfos();
	std::vector<CvInfoBase*>& getInvisibleInfo();
	CvInfoBase& getInvisibleInfo(InvisibleTypes e);

	int getNumFatherInfos();
	DllExport std::vector<CvFatherInfo*>& getFatherInfo();
	CvFatherInfo& getFatherInfo(FatherTypes e);

	DllExport int getNumFatherPointInfos();
	std::vector<CvFatherPointInfo*>& getFatherPointInfo();
	DllExport CvFatherPointInfo& getFatherPointInfo(FatherPointTypes e);
	CvFatherPointInfo& getInfo(FatherPointTypes e);

	int getNumUnitCombatInfos();
	std::vector<CvInfoBase*>& getUnitCombatInfo();
	CvInfoBase& getUnitCombatInfo(UnitCombatTypes e);

	std::vector<CvInfoBase*>& getDomainInfo();
	CvInfoBase& getDomainInfo(DomainTypes e);

	std::vector<CvInfoBase*>& getUnitAIInfo();
	CvInfoBase& getUnitAIInfo(UnitAITypes eUnitAINum);

	std::vector<CvInfoBase*>& getAttitudeInfo();
	DllExport	CvInfoBase& getAttitudeInfo(AttitudeTypes eAttitudeNum);

	std::vector<CvInfoBase*>& getMemoryInfo();
	DllExport	CvInfoBase& getMemoryInfo(MemoryTypes eMemoryNum);

	int getNumFatherCategoryInfos();
	std::vector<CvInfoBase*>& getFatherCategoryInfo();
	DllExport	CvInfoBase& getFatherCategoryInfo(FatherCategoryTypes eFatherCategoryNum);

	int getNumGameOptionInfos();
	std::vector<CvGameOptionInfo*>& getGameOptionInfo();
	DllExport	CvGameOptionInfo& getGameOptionInfo(GameOptionTypes eGameOptionNum);

	int getNumMPOptionInfos();
	std::vector<CvMPOptionInfo*>& getMPOptionInfo();
	DllExport	CvMPOptionInfo& getMPOptionInfo(MultiplayerOptionTypes eMPOptionNum);

	int getNumForceControlInfos();
	std::vector<CvForceControlInfo*>& getForceControlInfo();
	DllExport	CvForceControlInfo& getForceControlInfo(ForceControlTypes eForceControlNum);

	std::vector<CvPlayerOptionInfo*>& getPlayerOptionInfo();
	DllExport	CvPlayerOptionInfo& getPlayerOptionInfo(PlayerOptionTypes ePlayerOptionNum);

	std::vector<CvGraphicOptionInfo*>& getGraphicOptionInfo();
	DllExport	CvGraphicOptionInfo& getGraphicOptionInfo(GraphicOptionTypes eGraphicOptionNum);

	std::vector<CvYieldInfo*>& getYieldInfo();
	DllExport	CvYieldInfo& getYieldInfo(YieldTypes eYieldNum);
	CvYieldInfo& getInfo(YieldTypes eYieldNum);

	DllExport int getNumRouteInfos();
	std::vector<CvRouteInfo*>& getRouteInfo();
	DllExport	CvRouteInfo& getRouteInfo(RouteTypes eRouteNum);
	CvRouteInfo& getInfo(RouteTypes eRouteNum);

	DllExport int getNumImprovementInfos();
	DllExport std::vector<CvImprovementInfo*>& getImprovementInfo();
	DllExport CvImprovementInfo& getImprovementInfo(ImprovementTypes eImprovementNum);
	CvImprovementInfo& getInfo(ImprovementTypes eImprovementNum);

	int getNumGoodyInfos();
	std::vector<CvGoodyInfo*>& getGoodyInfo();
	CvGoodyInfo& getGoodyInfo(GoodyTypes eGoodyNum);

	int getNumBuildInfos();
	std::vector<CvBuildInfo*>& getBuildInfo();
	DllExport CvBuildInfo& getBuildInfo(BuildTypes eBuildNum);

	DllExport int getNumHandicapInfos();
	DllExport std::vector<CvHandicapInfo*>& getHandicapInfo();
	DllExport CvHandicapInfo& getHandicapInfo(HandicapTypes eHandicapNum);

	DllExport int getNumGameSpeedInfos();
	std::vector<CvGameSpeedInfo*>& getGameSpeedInfo();
	DllExport CvGameSpeedInfo& getGameSpeedInfo(GameSpeedTypes eGameSpeedNum);

	int getNumAlarmInfos();
	std::vector<CvAlarmInfo*>& getAlarmInfo();
	CvAlarmInfo& getAlarmInfo(AlarmTypes eAlarm);

	DllExport int getNumTurnTimerInfos();
	std::vector<CvTurnTimerInfo*>& getTurnTimerInfo();
	DllExport CvTurnTimerInfo& getTurnTimerInfo(TurnTimerTypes eTurnTimerNum);

	DllExport int getNumBuildingClassInfos();
	std::vector<CvBuildingClassInfo*>& getBuildingClassInfo();
	CvBuildingClassInfo& getBuildingClassInfo(BuildingClassTypes eBuildingClassNum);

	int getNumBuildingInfos() const;
	int getNumBuildingInfosFakeExe() const;
	std::vector<CvBuildingInfo*>& getBuildingInfo();
	CvBuildingInfo& getBuildingInfo(BuildingTypes eBuildingNum);

	DllExport int getNumSpecialBuildingInfos();
	std::vector<CvSpecialBuildingInfo*>& getSpecialBuildingInfo();
	DllExport CvSpecialBuildingInfo& getSpecialBuildingInfo(SpecialBuildingTypes eSpecialBuildingNum);
	CvSpecialBuildingInfo& getInfo(SpecialBuildingTypes eSpecialBuilding);

	DllExport int getNumUnitClassInfos();
	std::vector<CvUnitClassInfo*>& getUnitClassInfo();
	CvUnitClassInfo& getUnitClassInfo(UnitClassTypes eUnitClassNum);

	DllExport int getNumActionInfos();
	std::vector<CvActionInfo*>& getActionInfo();
	DllExport CvActionInfo& getActionInfo(int i);

	std::vector<CvMissionInfo*>& getMissionInfo();
	DllExport CvMissionInfo& getMissionInfo(MissionTypes eMissionNum);

	std::vector<CvControlInfo*>& getControlInfo();
	CvControlInfo& getControlInfo(ControlTypes eControlNum);

	std::vector<CvCommandInfo*>& getCommandInfo();
	DllExport CvCommandInfo& getCommandInfo(CommandTypes eCommandNum);

	int getNumAutomateInfos();
	std::vector<CvAutomateInfo*>& getAutomateInfo();
	CvAutomateInfo& getAutomateInfo(int iAutomateNum);

	DllExport int getNumPromotionInfos();
	DllExport std::vector<CvPromotionInfo*>& getPromotionInfo();
	DllExport CvPromotionInfo& getPromotionInfo(PromotionTypes ePromotionNum);

	int getNumProfessionInfos();
	DllExport std::vector<CvProfessionInfo*>& getProfessionInfo();
	CvProfessionInfo& getProfessionInfo(ProfessionTypes eProfessionNum);

	DllExport int getNumCivicOptionInfos();
	std::vector<CvInfoBase*>& getCivicOptionInfo();
	CvInfoBase& getCivicOptionInfo(CivicOptionTypes eCivicOptionNum);

	int getNumCivicInfos();
	DllExport std::vector<CvCivicInfo*>& getCivicInfo();
	CvCivicInfo& getCivicInfo(CivicTypes eCivicNum);

	int getNumDiplomacyInfos();
	DllExport std::vector<CvDiplomacyInfo*>& getDiplomacyInfo();
	DllExport	CvDiplomacyInfo& getDiplomacyInfo(int iDiplomacyNum);

	DllExport int getNumEraInfos();
	std::vector<CvEraInfo*>& getEraInfo();
	DllExport	CvEraInfo& getEraInfo(EraTypes eEraNum);

	DllExport int getNumHurryInfos();
	std::vector<CvHurryInfo*>& getHurryInfo();
	DllExport	CvHurryInfo& getHurryInfo(HurryTypes eHurryNum);

	int getNumEmphasizeInfos();
	std::vector<CvEmphasizeInfo*>& getEmphasizeInfo();
	DllExport	CvEmphasizeInfo& getEmphasizeInfo(EmphasizeTypes eEmphasizeNum);

	int getNumCultureLevelInfos();
	std::vector<CvCultureLevelInfo*>& getCultureLevelInfo();
	DllExport	CvCultureLevelInfo& getCultureLevelInfo(CultureLevelTypes eCultureLevelNum);

	DllExport int getNumVictoryInfos();
	std::vector<CvVictoryInfo*>& getVictoryInfo();
	DllExport	CvVictoryInfo& getVictoryInfo(VictoryTypes eVictoryNum);

	int getNumEventTriggerInfos();
	DllExport std::vector<CvEventTriggerInfo*>& getEventTriggerInfo();
	CvEventTriggerInfo& getEventTriggerInfo(EventTriggerTypes eEventTrigger);

	int getNumEventInfos();
	DllExport std::vector<CvEventInfo*>& getEventInfo();
	CvEventInfo& getEventInfo(EventTypes eEvent);

	// trade screen type - start - Nightinggale
	int getNumTradeScreenInfos() const;
	std::vector<CvTradeScreenInfo*>& getTradeScreenInfo();
	const CvTradeScreenInfo& getTradeScreenInfo(TradeScreenTypes eTradeScreen) const;
	// trade screen type - end - Nightinggale

	// CivEffect
	//inline int getNumCivEffectInfos() const { return m_paCivEffectInfo.size(); }
	// use NUM_CIV_EFFECT_TYPES

	const CivEffectInfo* getAutogeneratedCivEffect() const { return m_pAutogeneratedCivEffect; }

	std::vector<CivEffectInfo*>& getCivEffectInfo();
	const CivEffectInfo& getCivEffectInfo(CivEffectTypes eCivEffect) const;
	// CivEffect end

	//Androrc UnitArtStyles
	int getNumUnitArtStyleTypeInfos();
	std::vector<CvUnitArtStyleTypeInfo*>& getUnitArtStyleTypeInfo();
	CvUnitArtStyleTypeInfo& getUnitArtStyleTypeInfo(UnitArtStyleTypes eUnitArtStyleTypeNum);
	//Androrc End
	DllExport int getNUM_ENGINE_DIRTY_BITS() const;
	DllExport int getNUM_INTERFACE_DIRTY_BITS() const;
	DllExport int getNUM_FORCECONTROL_TYPES() const;
	DllExport int getNUM_INFOBAR_TYPES() const;
	DllExport int getNUM_HEALTHBAR_TYPES() const;
	int getNUM_CONTROL_TYPES() const;
	DllExport int getNUM_LEADERANIM_TYPES() const;

	int& getNumEntityEventTypes();
	CvString*& getEntityEventTypes();
	DllExport CvString& getEntityEventTypes(EntityEventTypes e);

	int& getNumAnimationOperatorTypes();
	CvString*& getAnimationOperatorTypes();
	CvString& getAnimationOperatorTypes(AnimationOperatorTypes e);

	CvString*& getFunctionTypes();
	CvString& getFunctionTypes(FunctionTypes e);

	DllExport int& getNumArtStyleTypes();
	CvString*& getArtStyleTypes();
	DllExport CvString& getArtStyleTypes(ArtStyleTypes e);

	int& getNumCitySizeTypes();
	CvString*& getCitySizeTypes();
	CvString& getCitySizeTypes(int i);

	CvString*& getContactTypes();
	CvString& getContactTypes(ContactTypes e);

	CvString*& getDiplomacyPowerTypes();
	CvString& getDiplomacyPowerTypes(DiplomacyPowerTypes e);

	CvString*& getAutomateTypes();
	CvString& getAutomateTypes(AutomateTypes e);

	CvString*& getDirectionTypes();
	CvString& getDirectionTypes(DirectionTypes e);

	DllExport int& getNumFootstepAudioTypes();
	CvString*& getFootstepAudioTypes();
	CvString& getFootstepAudioTypes(int i);
	int getFootstepAudioTypeByTag(CvString strTag);

	CvString*& getFootstepAudioTags();
	DllExport CvString& getFootstepAudioTags(int i);

	CvString& getCurrentXMLFile();
	void setCurrentXMLFile(char const* szFileName);

	//
	///////////////// BEGIN global defines
	// THESE ARE READ-ONLY
	//

	DllExport FVariableSystem* getDefinesVarSystem();
	void cacheGlobals();

	// ***** EXPOSED TO PYTHON *****
	DllExport int getDefineINT( const char * szName ) const;
	DllExport float getDefineFLOAT( const char * szName ) const;
	float getDefineFLOATUncached(const char * szName) const;
	DllExport const char * getDefineSTRING( const char * szName ) const;
	DllExport void setDefineINT( const char * szName, int iValue );
	DllExport void setDefineFLOAT( const char * szName, float fValue );
	DllExport void setDefineSTRING( const char * szName, const char * szValue );

	int getMAX_HIT_POINTS();
	int getHILLS_EXTRA_DEFENSE();
	int getRIVER_ATTACK_MODIFIER();
	int getAMPHIB_ATTACK_MODIFIER();
	DllExport int getMAX_PLOT_LIST_ROWS();
	DllExport int getUNIT_MULTISELECT_MAX();
	DllExport int getEVENT_MESSAGE_TIME();
	int getROUTE_FEATURE_GROWTH_MODIFIER();
	int getFEATURE_GROWTH_MODIFIER();
	int getMIN_CITY_RANGE();
	int getCITY_MAX_NUM_BUILDINGS();
	int getLAKE_MAX_AREA_SIZE();
	int getMIN_WATER_SIZE_FOR_OCEAN();
	int getFORTIFY_MODIFIER_PER_TURN();
	int getMAX_CITY_DEFENSE_DAMAGE();
	int getPEAK_SEE_THROUGH_CHANGE();
	int getHILLS_SEE_THROUGH_CHANGE();
	int getSEAWATER_SEE_FROM_CHANGE();
	int getPEAK_SEE_FROM_CHANGE();
	int getHILLS_SEE_FROM_CHANGE();
	int getMAX_REBEL_YIELD_MODIFIER();
	// TAC - AI Improved Navel AI - koma13 - START
	int getAI_TRANSPORT_DANGER_RANGE();
	int getAI_LOST_TRANSPORT_MEMORY_COUNT();
	// TAC - AI Improved Navel AI - koma13 - END
	// R&R, ray, caching globals from Global Defines Alt - START
	int getPLOT_VISIBILITY_RANGE();
	int getUNIT_VISIBILITY_RANGE();
	int getMIN_CITY_YIELD_DECAY();
	int getCITY_YIELD_DECAY_PERCENT();
	int getIMMIGRATION_THRESHOLD();
	int getIMMIGRATION_THRESHOLD_INCREASE();
	int getIMMIGRATION_THRESHOLD_MODIFIER_UNITS_ON_DOCK();
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
	// WTP, ray Domestic Market Events - START
	int getENABLE_DOMESTIC_DEMAND_EVENTS();
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
	int getMAX_TAX_RATE();
	int getMIN_TAX_RATE();
	int getINCREASE_MAX_TAX_RATE();
	int getDECREASE_MAX_TAX_RATE();
	// TAC AI hurries Immigrants
	int getAI_HURRY_IMMIGRANTS();
	// TAC LbD
	int getLBD_BASE_CHANCE_EXPERT();
	int getLBD_CHANCE_INCREASE_EXPERT();
	int getLBD_PRE_ROUNDS_EXPERT();
	int getLBD_CHANCE_INCREASE_EXPERT_FROM_TEACHER(); // WTP, ray, teacher addon for LbD
	int getLBD_PRE_ROUNDS_EXPERT_DECREASE_FROM_TEACHER(); // WTP, ray, teacher addon for LbD
	int getLBD_KI_MOD_EXPERT();
	int getLBD_BASE_CHANCE_FREE();
	int getLBD_CHANCE_INCREASE_FREE();
	int getLBD_PRE_ROUNDS_FREE();
	int getLBD_CHANCE_MOD_FREE_CRIMINAL();
	int getLBD_CHANCE_MOD_FREE_SERVANT();
	int getLBD_CHANCE_ESCAPE();
	int getLBD_CHANCE_MOD_ESCAPE_CRIMINAL();
	int getLBD_CHANCE_MOD_ESCAPE_SERVANT();
	// WTP, ray, LbD Slaves Revolt and Free - START
	int getLBD_CHANCE_REVOLT();
	int getLBD_CHANCE_MOD_REVOLT_SLAVE();
	int getLBD_CHANCE_MOD_REVOLT_CRIMINAL();
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
	int getBASE_CHANCE_ROYAL_INTERVENTIONS();// WTP, ray, Royal Intervention, START
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
	int getMIN_ROUND_COLONIAL_INTERVENTION_NATIVE_WAR(); //WTP, ray, Colonial Intervention In Native War - START
	int getCOLONIAL_INTERVENTION_NATIVE_WAR_CHANCE(); //WTP, ray, Colonial Intervention In Native War - START
	int getCOLONIAL_INTERVENTION_NATIVE_WAR_GOLD_TO_PAY_PER_UNIT(); //WTP, ray, Colonial Intervention In Native War - START
	int getMIN_ROUND_COLONIES_AND_NATIVE_ALLIES_WAR(); // WTP, ray, Big Colonies and Native Allies War - START
	int getBASE_CHANCE_COLONIES_AND_NATIVE_ALLIES_WAR(); // WTP, ray, Big Colonies and Native Allies War - START
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
	int getTIMER_ROYAL_INTERVENTIONS(); // WTP, ray, Royal Intervention, START
	int getTIMER_PRIVATEERS_DIPLO_EVENT(); // WTP, ray, Privateers DLL Diplo Event - START
	int getTIMER_PRISONS_CROWDED();
	int getTIMER_REVOLUTIONARY_NOBLE();
	int getTIMER_BISHOP();
	int getTIMER_COLONIAL_INTERVENTION_NATIVE_WAR(); //WTP, ray, Colonial Intervention In Native War - START
	int getTIMER_COLONIES_AND_NATIVE_ALLIES_WAR(); // WTP, ray, Big Colonies and Native Allies War - START
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
	int getNATIVE_PRODUCTION_RAID_MIN();
	int getNATIVE_PRODUCTION_RAID_RANDOM();
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

	// R&R, ray, Health - START
	int getMIN_POP_NEG_HEALTH();
	int getPOP_DIVISOR_NEG_HEALTH();
	int getMAX_CITY_HEALTH();
	int getLOWEST_CITY_HEALTH();
	// R&R, ray, Health - END

	// WTP, ray, Health Overhaul - START
	int getSWEET_WATER_CITY_LOCATION_HEALTH_BONUS();
	int getCOASTAL_CITY_LOCATION_HEALTH_BONUS();
	int getHILL_CITY_LOCATION_HEALTH_BONUS();
	int getBAD_CITY_LOCATION_HEALTH_MALUS();
	// WTP, ray, Health Overhaul - END

	// WTP, ray, Happiness - START
	int getMIN_POP_NEG_HAPPINESS();
	int getPOP_DIVISOR_HAPPINESS();
	int getPER_EUROPEAN_AT_WAR_UNHAPPINESS();
	int getPOP_DIVISOR_DEFENSE_UNHAPPINESS();
	int getTAX_DIVISOR_UNHAPPINESS();

	int getBASE_CHANCE_UNREST_UNHAPPINESS();
	int getBASE_CHANCE_FESTIVITIES_HAPPINESS();
	int getMIN_BALANCE_UNREST_UNHAPPINESS();
	int getMIN_BALANCE_FESTIVITIES_HAPPINESS();
	int getTURNS_UNREST_UNHAPPINESS();
	int getFOUNDING_FATHER_POINTS_FESTIVITIES_HAPPINESS();
	int getTIMER_FESTIVITIES_OR_UNRESTS();
	// WTP, ray, Happiness - END

	// WTP, ray, Crime and Law - START
	int getMIN_POP_CRIME();
	int getPOP_DIVISOR_CRIME();
	int getPER_EUROPEAN_AT_WAR_CRIME();
	int getCRIME_PERCENT_BONUS_FACTOR_OVERFLOW();
	// WTP, ray, Crime and Law - END

	//WTP, ray, Slave Hunter and Slave Master - START
	int getMAX_SLAVE_REVOLT_REDUCTION_BONUS_PER_CITY();
	int getMAX_SLAVE_WORKER_PRODUCTION_BONUS_PER_CITY();
	//WTP, ray, Slave Hunter and Slave Master - END

    int getMAX_TREASURE_AMOUNT(); // WTP, merge Treasures, of Raubwuerger - START
	int getTRADE_POST_GOLD_PER_NATIVE(); // WTP, ray, Native Trade Posts - START

	// softcoding enum values

	int getOPPRESSOMETER_DISCRIMINATION_MODIFIER_BASE_COLONIZERS();
	int getOPPRESSOMETER_DISCRIMINATION_MODIFIER_BASE_NATIVES();
	int getOPPRESSOMETER_FORCED_LABOR_MODIFIER_BASE();
	int getOPPRESSOMETER_DECAY_RATE_BASE();


	void postXMLLoad(bool bFirst);

	/// GameFont XML control - start - Nightinggale
	int getSymbolID(FontSymbols eSymbol) const;

	void setupGameFontChars();
	int getFontSymbolBonusOffset() const;
	int getFontSymbolCustomOffset() const;
	/// GameFont XML control - end - Nightinggale

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

	int getUSE_CAN_FOUND_CITIES_ON_WATER_CALLBACK();
	int getUSE_CANNOT_DO_CIVIC_CALLBACK();
	int getUSE_CAN_DO_CIVIC_CALLBACK();
	int getUSE_CANNOT_CONSTRUCT_CALLBACK();
	int getUSE_CAN_CONSTRUCT_CALLBACK();
	int getUSE_CAN_DECLARE_WAR_CALLBACK();
	int getUSE_GET_UNIT_COST_MOD_CALLBACK();
	int getUSE_GET_BUILDING_COST_MOD_CALLBACK();
	int getUSE_GET_CITY_FOUND_VALUE_CALLBACK();
	int getUSE_CANNOT_HANDLE_ACTION_CALLBACK();
	int getUSE_CAN_BUILD_CALLBACK();
	int getUSE_CANNOT_TRAIN_CALLBACK();
	int getUSE_CAN_TRAIN_CALLBACK();
	int getUSE_UNIT_CANNOT_MOVE_INTO_CALLBACK();
	DllExport int getUSE_FINISH_TEXT_CALLBACK();
	DllExport int getUSE_ON_UNIT_SET_XY_CALLBACK();
	DllExport int getUSE_ON_UNIT_SELECTED_CALLBACK();
	DllExport int getUSE_ON_MISSIONARY_CONVERTED_UNIT_CALLBACK();
	DllExport int getUSE_ON_UPDATE_CALLBACK();
	DllExport int getUSE_ON_UNIT_CREATED_CALLBACK();
	DllExport int getUSE_ON_UNIT_LOST_CALLBACK();

	DllExport int getMAX_CIV_PLAYERS();
	int getMAX_PLAYERS();
	int getMAX_CIV_TEAMS();
	int getMAX_TEAMS();
	int getINVALID_PLOT_COORD();
	int getNUM_CITY_PLOTS();
	int getCITY_HOME_PLOT();

	// ***** END EXPOSED TO PYTHON *****

	////////////// END DEFINES //////////////////

	DllExport void setDLLIFace(CvDLLUtilityIFaceBase* pDll);
	CvDLLUtilityIFaceBase* getDLLIFace() { return m_pDLL; }		// inlined for perf reasons, do not use outside of dll
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

	void cleanInfoStrings();

	const InfoArray<YieldTypes>& getDomesticDemandYieldTypes() const { return m_iaDomesticDemandYieldTypes; }

	void setCityCatchmentRadius(int iSetting);

	bool isMainThread() const;

	ProfilerManager& getProfiler() { return m_ProfileManager; }

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
	int* m_aaiXYCityPlot;// [CITY_PLOTS_DIAMETER][CITY_PLOTS_DIAMETER];
	int* m_aaiXYCityPlot_1_plot;
	int* m_aaiXYCityPlot_2_plot;

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
	std::vector< CvTradeScreenInfo*> m_paTradeScreenInfo; // trade screen type - Nightinggale
	std::vector<CivEffectInfo*> m_paCivEffectInfo;
	CivEffectInfo* m_pAutogeneratedCivEffect;

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

	int m_iMAX_HIT_POINTS;
	int m_iHILLS_EXTRA_DEFENSE;
	int m_iRIVER_ATTACK_MODIFIER;
	int m_iAMPHIB_ATTACK_MODIFIER;
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
	int m_IMMIGRATION_THRESHOLD_MODIFIER_UNITS_ON_DOCK;
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
	// WTP, ray Domestic Market Events - START
	int m_ENABLE_DOMESTIC_DEMAND_EVENTS;
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
	int m_LBD_CHANCE_INCREASE_EXPERT_FROM_TEACHER; // WTP, ray, teacher addon for LbD
	int m_LBD_PRE_ROUNDS_EXPERT_DECREASE_FROM_TEACHER; // WTP, ray, teacher addon for LbD
	int m_LBD_KI_MOD_EXPERT;
	int m_LBD_BASE_CHANCE_FREE;
	int m_LBD_CHANCE_INCREASE_FREE;
	int m_LBD_PRE_ROUNDS_FREE;
	int m_LBD_CHANCE_MOD_FREE_CRIMINAL;
	int m_LBD_CHANCE_MOD_FREE_SERVANT;
	int m_LBD_CHANCE_ESCAPE;
	int m_LBD_CHANCE_MOD_ESCAPE_CRIMINAL;
	int m_LBD_CHANCE_MOD_ESCAPE_SERVANT;
	// WTP, ray, LbD Slaves Revolt and Free - START
	int m_LBD_CHANCE_REVOLT;
	int m_LBD_CHANCE_MOD_REVOLT_SLAVE;
	int m_LBD_CHANCE_MOD_REVOLT_CRIMINAL;
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
	int m_BASE_CHANCE_ROYAL_INTERVENTIONS; // WTP, ray, Royal Intervention, START
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
	int m_MIN_ROUND_COLONIAL_INTERVENTION_NATIVE_WAR; //WTP, ray, Colonial Intervention In Native War - START
	int m_COLONIAL_INTERVENTION_NATIVE_WAR_CHANCE; //WTP, ray, Colonial Intervention In Native War - START
	int m_COLONIAL_INTERVENTION_NATIVE_WAR_GOLD_TO_PAY_PER_UNIT; //WTP, ray, Colonial Intervention In Native War - START
	int m_MIN_ROUND_COLONIES_AND_NATIVE_ALLIES_WAR; // WTP, ray, Big Colonies and Native Allies War - START
	int m_BASE_CHANCE_COLONIES_AND_NATIVE_ALLIES_WAR; // WTP, ray, Big Colonies and Native Allies War - START
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
	int m_TIMER_ROYAL_INTERVENTIONS; // WTP, ray, Royal Intervention, START
	int m_TIMER_PRIVATEERS_DIPLO_EVENT; // WTP, ray, Privateers DLL Diplo Event - START
	int m_TIMER_PRISONS_CROWDED;
	int m_TIMER_REVOLUTIONARY_NOBLE;
	int m_TIMER_BISHOP;
	int m_TIMER_COLONIAL_INTERVENTION_NATIVE_WAR; //WTP, ray, Colonial Intervention In Native War - START
	int m_TIMER_COLONIES_AND_NATIVE_ALLIES_WAR; // WTP, ray, Big Colonies and Native Allies War - START
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
	int m_NATIVE_PRODUCTION_RAID_MIN;
	int m_NATIVE_PRODUCTION_RAID_RANDOM;
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

	// R&R, ray, Health - START
	int m_MIN_POP_NEG_HEALTH;
	int m_POP_DIVISOR_NEG_HEALTH;
	int m_MAX_CITY_HEALTH;
	int m_LOWEST_CITY_HEALTH;
	// R&R, ray, Health - END

	// WTP, ray, Health Overhaul - START
	int m_SWEET_WATER_CITY_LOCATION_HEALTH_BONUS;
	int m_COASTAL_CITY_LOCATION_HEALTH_BONUS;
	int m_HILL_CITY_LOCATION_HEALTH_BONUS;
	int m_BAD_CITY_LOCATION_HEALTH_MALUS;
	// WTP, ray, Health Overhaul - END

	InfoArray<YieldTypes> m_iaDomesticDemandYieldTypes;

	// WTP, ray, Happiness - START
	int m_MIN_POP_NEG_HAPPINESS;
	int m_POP_DIVISOR_HAPPINESS;
	int m_PER_EUROPEAN_AT_WAR_UNHAPPINESS;
	int m_POP_DIVISOR_DEFENSE_UNHAPPINESS;
	int m_TAX_DIVISOR_UNHAPPINESS;

	int m_BASE_CHANCE_UNREST_UNHAPPINESS;
	int m_BASE_CHANCE_FESTIVITIES_HAPPINESS;
	int m_MIN_BALANCE_UNREST_UNHAPPINESS;
	int m_MIN_BALANCE_FESTIVITIES_HAPPINESS;
	int m_TURNS_UNREST_UNHAPPINESS;
	int m_FOUNDING_FATHER_POINTS_FESTIVITIES_HAPPINESS;
	int m_TIMER_FESTIVITIES_OR_UNRESTS;
	// WTP, ray, Happiness - END

	// WTP, ray, Crime and Law - START
	int m_MIN_POP_CRIME;
	int m_POP_DIVISOR_CRIME;
	int m_PER_EUROPEAN_AT_WAR_CRIME;
	int m_CRIME_PERCENT_BONUS_FACTOR_OVERFLOW;
	// WTP, ray, Crime and Law - END

	//WTP, ray, Slave Hunter and Slave Master - START
	int m_MAX_SLAVE_REVOLT_REDUCTION_BONUS_PER_CITY;
	int m_MAX_SLAVE_WORKER_PRODUCTION_BONUS_PER_CITY;
	//WTP, ray, Slave Hunter and Slave Master - END

	int m_iOPPRESSOMETER_DISCRIMINATION_MODIFIER_BASE_COLONIZERS;
	int m_iOPPRESSOMETER_DISCRIMINATION_MODIFIER_BASE_NATIVES;
	int m_iOPPRESSOMETER_FORCED_LABOR_MODIFIER_BASE;
	int m_iOPPRESSOMETER_DECAY_RATE_BASE;

	/// GameFont XML control - start - Nightinggale
	FontSymbols  m_aiGameFontCustomSymbolID[MAX_NUM_SYMBOLS];
	/// GameFont XML control - end - Nightinggale

	int m_MAX_TREASURE_AMOUNT; // WTP, merge Treasures, of Raubwuerger
	int m_TRADE_POST_GOLD_PER_NATIVE; // WTP, ray, Native Trade Posts - START

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

	ProfilerManager m_ProfileManager;

	// K-Mod \ RaR
	bool m_bUSE_AI_UNIT_UPDATE_CALLBACK;
	bool m_bUSE_AI_DO_DIPLO_CALLBACK;
	bool m_bUSE_AI_DO_WAR_CALLBACK;
	bool m_bUSE_DO_GROWTH_CALLBACK;
	bool m_bUSE_DO_CULTURE_CALLBACK;
	bool m_bUSE_DO_PLOT_CULTURE_CALLBACK;
	bool m_bUSE_DO_PRODUCTION_CALLBACK;
	bool m_bUSE_AI_CHOOSE_PRODUCTION_CALLBACK;
	bool m_bUSE_DO_PILLAGE_GOLD_CALLBACK;
	bool m_bUSE_GET_EXPERIENCE_NEEDED_CALLBACK;
	bool m_bUSE_DO_COMBAT_CALLBACK;
	bool m_bUSE_DO_GOLD_CALLBACK;
	bool m_bUSE_UPDATE_COLORED_PLOTS_CALLBACK;
	bool m_bUSE_IS_VICTORY_TEST_CALLBACK;
	// K-Mod \ RaR end

	const DWORD m_iThreadID;

	// DLL interface
	CvDLLUtilityIFaceBase* m_pDLL;

	public:
		// Erik: Support for coastal route path finding
		void setCoastalRouteFinder(FAStar* pVal);
		FAStar* m_coastalRouteFinder;
		FAStar& getCoastalRouteFinder();

		// Erik: These are from K-Mod
		// more reliable versions of the 'gDLL->xxxKey' functions:
		inline bool altKey()   { return (GetKeyState(VK_MENU   ) & 0x8000) != 0; }
		inline bool ctrlKey()  { return (GetKeyState(VK_CONTROL) & 0x8000) != 0; }
		inline bool shiftKey() { return (GetKeyState(VK_SHIFT  ) & 0x8000) != 0; }
		// NOTE: I've replaced all calls to the gDLL key functions with calls to these functions.

		inline bool getUSE_AI_UNIT_UPDATE_CALLBACK() { return m_bUSE_AI_UNIT_UPDATE_CALLBACK; }
		inline bool getUSE_AI_DO_DIPLO_CALLBACK() { return m_bUSE_AI_DO_DIPLO_CALLBACK; }
		inline bool getUSE_AI_DO_WAR_CALLBACK() { return m_bUSE_AI_DO_WAR_CALLBACK; }
		inline bool getUSE_DO_GROWTH_CALLBACK() { return m_bUSE_DO_GROWTH_CALLBACK; }
		inline bool getUSE_DO_CULTURE_CALLBACK() { return m_bUSE_DO_CULTURE_CALLBACK; }
		inline bool getUSE_DO_PLOT_CULTURE_CALLBACK() { return m_bUSE_DO_PLOT_CULTURE_CALLBACK; }
		inline bool getUSE_DO_PRODUCTION_CALLBACK() { return m_bUSE_DO_PRODUCTION_CALLBACK; }
		inline bool getUSE_AI_CHOOSE_PRODUCTION_CALLBACK() { return m_bUSE_AI_CHOOSE_PRODUCTION_CALLBACK; }
		inline bool getUSE_DO_PILLAGE_GOLD_CALLBACK() { return m_bUSE_DO_PILLAGE_GOLD_CALLBACK; }
		inline bool getUSE_GET_EXPERIENCE_NEEDED_CALLBACK() { return m_bUSE_GET_EXPERIENCE_NEEDED_CALLBACK; }
		inline bool getUSE_DO_COMBAT_CALLBACK() { return m_bUSE_DO_COMBAT_CALLBACK; }
		inline bool getUSE_DO_GOLD_CALLBACK() { return m_bUSE_DO_GOLD_CALLBACK; }
		inline bool getUSE_UPDATE_COLORED_PLOTS_CALLBACK() { return m_bUSE_UPDATE_COLORED_PLOTS_CALLBACK; }
		inline bool getUSE_IS_VICTORY_TEST_CALLBACK() { return m_bUSE_IS_VICTORY_TEST_CALLBACK; }

public:
	void setExeXmlLengthOverride(bool bEnabled)
	{
		m_bExeXmlLengthOverride = bEnabled;
	}

protected:
	bool m_bExeXmlLengthOverride;
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
#define gDLL GC.getDLLIFace()

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
inline int CvGlobals::getIMMIGRATION_THRESHOLD_MODIFIER_UNITS_ON_DOCK()
{
	return m_IMMIGRATION_THRESHOLD_MODIFIER_UNITS_ON_DOCK;
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

// WTP, ray Domestic Market Events - START
inline int CvGlobals::getENABLE_DOMESTIC_DEMAND_EVENTS()
{
	return m_ENABLE_DOMESTIC_DEMAND_EVENTS;
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
// WTP, ray, teacher addon for LbD - START
inline int CvGlobals::getLBD_CHANCE_INCREASE_EXPERT_FROM_TEACHER()
{
	return m_LBD_CHANCE_INCREASE_EXPERT_FROM_TEACHER;
}
inline int CvGlobals::getLBD_PRE_ROUNDS_EXPERT_DECREASE_FROM_TEACHER()
{
	return m_LBD_PRE_ROUNDS_EXPERT_DECREASE_FROM_TEACHER;
}
// WTP, ray, teacher addon for LbD -END

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
// WTP, ray, LbD Slaves Revolt and Free - START
inline int CvGlobals::getLBD_CHANCE_REVOLT()
{
	return m_LBD_CHANCE_REVOLT;
}
inline int CvGlobals::getLBD_CHANCE_MOD_REVOLT_SLAVE()
{
	return m_LBD_CHANCE_MOD_REVOLT_SLAVE;
}
inline int CvGlobals::getLBD_CHANCE_MOD_REVOLT_CRIMINAL()
{
	return m_LBD_CHANCE_MOD_REVOLT_CRIMINAL;
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
// WTP, ray, Royal Intervention, START
inline int CvGlobals::getBASE_CHANCE_ROYAL_INTERVENTIONS()
{
	return m_BASE_CHANCE_ROYAL_INTERVENTIONS;
}
// WTP, ray, Royal Intervention, END
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
//WTP, ray, Colonial Intervention In Native War - START
inline int CvGlobals::getMIN_ROUND_COLONIAL_INTERVENTION_NATIVE_WAR()
{
	return m_MIN_ROUND_COLONIAL_INTERVENTION_NATIVE_WAR;
}
inline int CvGlobals::getCOLONIAL_INTERVENTION_NATIVE_WAR_CHANCE()
{
	return m_COLONIAL_INTERVENTION_NATIVE_WAR_CHANCE;
}
inline int CvGlobals::getCOLONIAL_INTERVENTION_NATIVE_WAR_GOLD_TO_PAY_PER_UNIT()
{
	return m_COLONIAL_INTERVENTION_NATIVE_WAR_GOLD_TO_PAY_PER_UNIT;
}
//WTP, ray, Colonial Intervention In Native War - END

// WTP, ray, Big Colonies and Native Allies War - START
inline int CvGlobals::getMIN_ROUND_COLONIES_AND_NATIVE_ALLIES_WAR()
{
	return m_MIN_ROUND_COLONIES_AND_NATIVE_ALLIES_WAR;
}
inline int CvGlobals::getBASE_CHANCE_COLONIES_AND_NATIVE_ALLIES_WAR()
{
	return m_BASE_CHANCE_COLONIES_AND_NATIVE_ALLIES_WAR;
}
// WTP, ray, Big Colonies and Native Allies War - END

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
// WTP, ray, Royal Intervention, START
inline int CvGlobals::getTIMER_ROYAL_INTERVENTIONS()
{
	return m_TIMER_ROYAL_INTERVENTIONS;
}
// WTP, ray, Royal Intervention, END

// WTP, ray, Privateers DLL Diplo Event - START
inline int CvGlobals::getTIMER_PRIVATEERS_DIPLO_EVENT()
{
	return m_TIMER_PRIVATEERS_DIPLO_EVENT;
}
// WTP, ray, Privateers DLL Diplo Event - END
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
//WTP, ray, Colonial Intervention In Native War - START
inline int CvGlobals::getTIMER_COLONIAL_INTERVENTION_NATIVE_WAR()
{
	return m_TIMER_COLONIAL_INTERVENTION_NATIVE_WAR;
}
//WTP, ray, Colonial Intervention In Native War - END

// WTP, ray, Big Colonies and Native Allies War - START
inline int CvGlobals::getTIMER_COLONIES_AND_NATIVE_ALLIES_WAR()
{
	return m_TIMER_COLONIES_AND_NATIVE_ALLIES_WAR;
}
// WTP, ray, Big Colonies and Native Allies War - END

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
inline int CvGlobals::getNATIVE_PRODUCTION_RAID_MIN()
{
	return m_NATIVE_PRODUCTION_RAID_MIN;
}
inline int CvGlobals::getNATIVE_PRODUCTION_RAID_RANDOM()
{
	return m_NATIVE_PRODUCTION_RAID_RANDOM;
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


// WTP, ray, Health Overhaul - START
inline int CvGlobals::getSWEET_WATER_CITY_LOCATION_HEALTH_BONUS()
{
	return m_SWEET_WATER_CITY_LOCATION_HEALTH_BONUS;
}

inline int CvGlobals::getCOASTAL_CITY_LOCATION_HEALTH_BONUS()
{
	return m_COASTAL_CITY_LOCATION_HEALTH_BONUS;
}

inline int CvGlobals::getHILL_CITY_LOCATION_HEALTH_BONUS()
{
	return m_HILL_CITY_LOCATION_HEALTH_BONUS;
}

inline int CvGlobals::getBAD_CITY_LOCATION_HEALTH_MALUS()
{
	return m_BAD_CITY_LOCATION_HEALTH_MALUS;
}
// WTP, ray, Health Overhaul - END


// WTP, ray, Happiness - START
inline int CvGlobals::getMIN_POP_NEG_HAPPINESS()
{
	return m_MIN_POP_NEG_HAPPINESS;
}

inline int CvGlobals::getPOP_DIVISOR_HAPPINESS()
{
	return m_POP_DIVISOR_HAPPINESS;
}

inline int CvGlobals::getPER_EUROPEAN_AT_WAR_UNHAPPINESS()
{
	return m_PER_EUROPEAN_AT_WAR_UNHAPPINESS;
}

inline int CvGlobals::getPOP_DIVISOR_DEFENSE_UNHAPPINESS()
{
	return m_POP_DIVISOR_DEFENSE_UNHAPPINESS;
}

inline int CvGlobals::getTAX_DIVISOR_UNHAPPINESS()
{
	return m_TAX_DIVISOR_UNHAPPINESS;
}

inline int CvGlobals::getBASE_CHANCE_UNREST_UNHAPPINESS()
{
	return m_BASE_CHANCE_UNREST_UNHAPPINESS;
}

inline int CvGlobals::getBASE_CHANCE_FESTIVITIES_HAPPINESS()
{
	return m_BASE_CHANCE_FESTIVITIES_HAPPINESS;
}

inline int CvGlobals::getMIN_BALANCE_UNREST_UNHAPPINESS()
{
	return m_MIN_BALANCE_UNREST_UNHAPPINESS;
}

inline int CvGlobals::getMIN_BALANCE_FESTIVITIES_HAPPINESS()
{
	return m_MIN_BALANCE_FESTIVITIES_HAPPINESS;
}

inline int CvGlobals::getTURNS_UNREST_UNHAPPINESS()
{
	return m_TURNS_UNREST_UNHAPPINESS;
}

inline int CvGlobals::getFOUNDING_FATHER_POINTS_FESTIVITIES_HAPPINESS()
{
	return m_FOUNDING_FATHER_POINTS_FESTIVITIES_HAPPINESS;
}

inline int CvGlobals::getTIMER_FESTIVITIES_OR_UNRESTS()
{
	return m_TIMER_FESTIVITIES_OR_UNRESTS;
}
// WTP, ray, Happiness - END

// WTP, ray, Crime and Law - START
inline int CvGlobals::getMIN_POP_CRIME()
{
	return m_MIN_POP_CRIME;
}

inline int CvGlobals::getPOP_DIVISOR_CRIME()
{
	return m_POP_DIVISOR_CRIME;
}

inline int CvGlobals::getPER_EUROPEAN_AT_WAR_CRIME()
{
	return m_PER_EUROPEAN_AT_WAR_CRIME;
}

inline int CvGlobals::getCRIME_PERCENT_BONUS_FACTOR_OVERFLOW()
{
	return m_CRIME_PERCENT_BONUS_FACTOR_OVERFLOW;
}
// WTP, ray, Crime and Law - END

//WTP, ray, Slave Hunter and Slave Master - START
inline int CvGlobals::getMAX_SLAVE_REVOLT_REDUCTION_BONUS_PER_CITY()
{
	return m_MAX_SLAVE_REVOLT_REDUCTION_BONUS_PER_CITY;
}

inline int CvGlobals::getMAX_SLAVE_WORKER_PRODUCTION_BONUS_PER_CITY()
{
	return m_MAX_SLAVE_WORKER_PRODUCTION_BONUS_PER_CITY;
}
//WTP, ray, Slave Hunter and Slave Master - END

// WTP, merge Treasures, of Raubwuerger - START
inline int CvGlobals::getMAX_TREASURE_AMOUNT()
{
	return m_MAX_TREASURE_AMOUNT;
}
// WTP, merge Treasures, of Raubwuerger - END

// WTP, ray, Native Trade Posts - START
inline int CvGlobals::getTRADE_POST_GOLD_PER_NATIVE()
{
	return m_TRADE_POST_GOLD_PER_NATIVE;
}
// WTP, ray, Native Trade Posts - END

inline int CvGlobals::getOPPRESSOMETER_DISCRIMINATION_MODIFIER_BASE_COLONIZERS()
{
	return m_iOPPRESSOMETER_DISCRIMINATION_MODIFIER_BASE_COLONIZERS;
}

inline int CvGlobals::getOPPRESSOMETER_DISCRIMINATION_MODIFIER_BASE_NATIVES()
{
	return m_iOPPRESSOMETER_DISCRIMINATION_MODIFIER_BASE_NATIVES;
}

inline int CvGlobals::getOPPRESSOMETER_FORCED_LABOR_MODIFIER_BASE()
{
	return m_iOPPRESSOMETER_FORCED_LABOR_MODIFIER_BASE;
}

inline int CvGlobals::getOPPRESSOMETER_DECAY_RATE_BASE()
{
	return m_iOPPRESSOMETER_DECAY_RATE_BASE;
}



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
