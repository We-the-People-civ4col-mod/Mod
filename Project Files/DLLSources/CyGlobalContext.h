#pragma once
#ifndef CyGlobalContext_h
#define CyGlobalContext_h
//
// Python wrapper class for global vars and fxns
// Passed to Python
//
#include "CvGlobals.h"
#include "CyInfos.h"
class CyGame;
class CyMap;
class CyPlayer;
class CvRandom;
class CyEngine;
class CyTeam;
class CyArtFileMgr;
class CyUserProfile;
class CyVariableSystem;
class CyUserSettings;

/// xml Editor - start - Nightinggale
class CyXMLEditor;
/// xml Editor - end - Nightinggale

class CyGlobalContext
{
public:
	// PatchMod: Achievements START
	int getNumAchieveInfos() const { return GC.getNumAchieveInfos(); }
	CyAchieveInfo* getAchieveInfo(int i) const;
	// PatchMod: Achievements END
	CyGlobalContext();
	virtual ~CyGlobalContext();
	static CyGlobalContext& getInstance();		// singleton accessor
	bool isDebugBuild() const;
	CyGame* getCyGame() const;
	CyMap* getCyMap() const;
	CyPlayer* getCyPlayer(int idx);
	CyPlayer* getCyActivePlayer();
	CvRandom& getCyASyncRand() const;
	CyTeam* getCyTeam(int i);
	CvArtFileMgr* getCvArtFileMgr() const;
	CyEffectInfo* getEffectInfo(int i) const;
	CyTerrainInfo* getTerrainInfo(int i) const;
	CyBonusInfo* getBonusInfo(int i) const;
	CyFeatureInfo* getFeatureInfo(int i) const;
	CyCivilizationInfo* getCivilizationInfo(int idx) const;
	CyLeaderHeadInfo* getLeaderHeadInfo(int i) const;
	CyTraitInfo* getTraitInfo(int i) const;
	CyUnitInfo* getUnitInfo(int i) const;
	CySpecialUnitInfo* getSpecialUnitInfo(int i) const;
	CyYieldInfo* getYieldInfo(int i) const;
	CyRouteInfo* getRouteInfo(int i) const;
	CyImprovementInfo* getImprovementInfo(int i) const;
	CyGoodyInfo* getGoodyInfo(int i) const;
	CyBuildInfo* getBuildInfo(int i) const;
	CyHandicapInfo* getHandicapInfo(int i) const;
	CyGameSpeedInfo* getGameSpeedInfo(int i) const;
	CyTurnTimerInfo* getTurnTimerInfo(int i) const;
	CyBuildingClassInfo* getBuildingClassInfo(int i) const;
	CyMissionInfo* getMissionInfo(int i) const;
	CyCommandInfo* getCommandInfo(int i) const;
	CyAutomateInfo* getAutomateInfo(int i) const;
	CyActionInfo* getActionInfo(int i) const;
	CyUnitClassInfo* getUnitClassInfo(int i) const;
	CvInfoBase* getUnitCombatInfo(int i) const;
	CvInfoBase* getDomainInfo(int i) const;
	CyBuildingInfo* getBuildingInfo(int i) const;
	CvInfoBase* getCivicOptionInfo(int i) const;
	CyCivicInfo* getCivicInfo(int i) const;
	CyDiplomacyInfo* getDiplomacyInfo(int i) const;
	CyControlInfo* getControlInfo(int i) const;
	CySpecialBuildingInfo* getSpecialBuildingInfo(int i) const;
	CyPromotionInfo* getPromotionInfo(int i) const;
	CyProfessionInfo* getProfessionInfo(int i) const;
	CvAnimationPathInfo * getAnimationPathInfo(int i) const;
	CyEmphasizeInfo * getEmphasizeInfo(int i) const;
	CyCultureLevelInfo * getCultureLevelInfo(int i) const;
	CyEraInfo * getEraInfo(int i) const;
	CyVictoryInfo * getVictoryInfo(int i) const;
	CyWorldInfo * getWorldInfo(int i) const;
	CyClimateInfo * getClimateInfo(int i) const;
	CySeaLevelInfo * getSeaLevelInfo(int i) const;
	CyEuropeInfo * getEuropeInfo(int i) const;
	CvInfoBase * getUnitAIInfo(int i) const;
	CvColorInfo* getColorInfo(int i) const;
	//Androrc UnitArtStyles
    CvUnitArtStyleTypeInfo* getUnitArtStyleTypeInfo(int i) const;
	//Androrc End
	int getInfoTypeForString(const char* szInfoType) const;
	int getNumPlayerColorInfos() const { return GC.getNumPlayerColorInfos(); }
	CvPlayerColorInfo* getPlayerColorInfo(int i) const;
	CvInfoBase* getHints(int i) const;
	CyMainMenuInfo* getMainMenus(int i) const;
	CvInfoBase* getInvisibleInfo(int i) const;
	CyFatherInfo* getFatherInfo(int i) const;
	CyFatherPointInfo* getFatherPointInfo(int i) const;
	CvInfoBase* getAttitudeInfo(int i) const;
	CvInfoBase* getMemoryInfo(int i) const;
	CvInfoBase* getFatherCategoryInfo(int i) const;
	CvInfoBase* getConceptInfo(int i) const;
	CvInfoBase* getCalendarInfo(int i) const;
	CvInfoBase* getGameOptionInfo(int i) const;
	CvInfoBase* getMPOptionInfo(int i) const;
	CvInfoBase* getForceControlInfo(int i) const;
	CvInfoBase* getSeasonInfo(int i) const;
	CvInfoBase* getMonthInfo(int i) const;
	CvInfoBase* getDenialInfo(int i) const;
	CyEventTriggerInfo* getEventTriggerInfo(int i) const;
	CyEventInfo* getEventInfo(int i) const;
	CyHurryInfo* getHurryInfo(int i) const;
	CyPlayerOptionInfo* getPlayerOptionInfo(int i) const;
	CyPlayerOptionInfo* getPlayerOptionsInfoByIndex(int i) const;
	CyGraphicOptionInfo* getGraphicOptionInfo(int i) const;
	CyGraphicOptionInfo* getGraphicOptionsInfoByIndex(int i) const;
	// ArtInfos
	CvArtInfoInterface* getInterfaceArtInfo(int i) const;
	CvArtInfoMovie* getMovieArtInfo(int i) const;
	CvArtInfoMisc* getMiscArtInfo(int i) const;
	CvArtInfoUnit* getUnitArtInfo(int i) const;
	CvArtInfoBuilding* getBuildingArtInfo(int i) const;
	CvArtInfoCivilization* getCivilizationArtInfo(int i) const;
	CvArtInfoLeaderhead* getLeaderheadArtInfo(int i) const;
	CvArtInfoBonus* getBonusArtInfo(int i) const;
	CvArtInfoImprovement* getImprovementArtInfo(int i) const;
	CvArtInfoTerrain* getTerrainArtInfo(int i) const;
	CvArtInfoFeature* getFeatureArtInfo(int i) const;

	// Structs
	const char* getEntityEventTypes(int i) const { return GC.getEntityEventTypes((EntityEventTypes) i); }
	const char* getAnimationOperatorTypes(int i) const { return GC.getAnimationOperatorTypes((AnimationOperatorTypes) i); }
	const char* getFunctionTypes(int i) const { return GC.getFunctionTypes((FunctionTypes) i); }
	const char* getArtStyleTypes(int i) const { return GC.getArtStyleTypes((ArtStyleTypes) i); }
	const char* getCitySizeTypes(int i) const { return GC.getCitySizeTypes(i); }
	const char* getContactTypes(int i) const { return GC.getContactTypes((ContactTypes) i); }
	const char* getDiplomacyPowerTypes(int i) const { return GC.getDiplomacyPowerTypes((DiplomacyPowerTypes) i); }
	const char *getFootstepAudioTypes(int i) { return GC.getFootstepAudioTypes(i); }
	const char *getFootstepAudioTags(int i) { return GC.getFootstepAudioTags(i); }
	int getNumEffectInfos() const { return GC.getNumEffectInfos(); }
	int getNumTerrainInfos() const { return GC.getNumTerrainInfos(); }
	int getNumYieldInfos() const { return NUM_YIELD_TYPES; }
	int getNumSpecialBuildingInfos() const { return GC.getNumSpecialBuildingInfos(); }
	int getNumBonusInfos() const { return GC.getNumBonusInfos(); };
	int getNumCivilizatonInfos() const { return GC.getNumCivilizationInfos(); }
	int getNumLeaderHeadInfos() const { return GC.getNumLeaderHeadInfos(); }
	int getNumTraitInfos() const { return GC.getNumTraitInfos(); }
	int getNumUnitInfos() const { return GC.getNumUnitInfos(); }
	int getNumSpecialUnitInfos() const { return GC.getNumSpecialUnitInfos(); }
	int getNumRouteInfos() const { return GC.getNumRouteInfos(); }
	int getNumFeatureInfos() const { return GC.getNumFeatureInfos(); }
	int getNumImprovementInfos() const { return GC.getNumImprovementInfos(); }
	int getNumGoodyInfos() const { return GC.getNumGoodyInfos(); }
	int getNumBuildInfos() const { return GC.getNumBuildInfos(); }
	int getNumHandicapInfos() const { return GC.getNumHandicapInfos(); }
	int getNumGameSpeedInfos() const { return GC.getNumGameSpeedInfos(); }
	int getNumTurnTimerInfos() const { return GC.getNumTurnTimerInfos(); }
	int getNumBuildingClassInfos() const { return GC.getNumBuildingClassInfos(); }
	int getNumBuildingInfos() const { return GC.getNumBuildingInfos(); }
	int getNumBuildingInfosFakeExe() const { return GC.getNumBuildingInfosFakeExe(); }
	int getNumUnitClassInfos() const { return GC.getNumUnitClassInfos(); }
	int getNumUnitCombatInfos() const { return GC.getNumUnitCombatInfos(); }
	int getNumAutomateInfos() const { return GC.getNumAutomateInfos(); }
	int getNumCommandInfos() const { return GC.getNumCommandInfos(); }
	int getNumControlInfos() const { return GC.getNumControlInfos(); }
	int getNumMissionInfos() const { return GC.getNumMissionInfos(); }
	int getNumActionInfos() const { return GC.getNumActionInfos(); }
	int getNumPromotionInfos() const { return GC.getNumPromotionInfos(); }
	int getNumProfessionInfos() const { return GC.getNumProfessionInfos(); }
	int getNumCivicInfos() const { return GC.getNumCivicInfos(); }
	int getNumDiplomacyInfos() const { return GC.getNumDiplomacyInfos(); }
	int getNumCivicOptionInfos() const { return GC.getNumCivicOptionInfos(); }
	int getNumEmphasizeInfos() const { return GC.getNumEmphasizeInfos(); }
	int getNumHurryInfos() const { return GC.getNumHurryInfos(); }
	int getNumCultureLevelInfos() const { return GC.getNumCultureLevelInfos(); }
	int getNumEraInfos() const { return GC.getNumEraInfos(); }
	int getNumVictoryInfos() const { return GC.getNumVictoryInfos(); }
	int getNumWorldInfos() const { return GC.getNumWorldInfos(); }
	int getNumSeaLevelInfos() const { return GC.getNumSeaLevelInfos(); }
	int getNumEuropeInfos() const { return GC.getNumEuropeInfos(); }
	int getNumClimateInfos() const { return GC.getNumClimateInfos(); }
	int getNumConceptInfos() const { return GC.getNumConceptInfos(); }
	int getNumCalendarInfos() const { return GC.getNumCalendarInfos(); }
	int getNumPlayerOptionInfos() const { return GC.getNumPlayerOptionInfos(); }
	int getNumGameOptionInfos() const { return GC.getNumGameOptionInfos(); }
	int getNumMPOptionInfos() const { return GC.getNumMPOptionInfos(); }
	int getNumForceControlInfos() const { return GC.getNumForceControlInfos(); }
	int getNumSeasonInfos() const { return GC.getNumSeasonInfos(); }
	int getNumMonthInfos() const { return GC.getNumMonthInfos(); }
	int getNumDenialInfos() const { return GC.getNumDenialInfos(); }
	int getNumEventTriggerInfos() const { return GC.getNumEventTriggerInfos(); }
	int getNumEventInfos() const { return GC.getNumEventInfos(); }
	int getNumHints() const { return GC.getNumHints(); }
	int getNumMainMenus() const { return GC.getNumMainMenus(); }
	int getNumInvisibleInfos() const { return GC.getNumInvisibleInfos(); }
	int getNumFatherInfos() const { return GC.getNumFatherInfos(); }
	int getNumFatherPointInfos() const { return GC.getNumFatherPointInfos(); }
	int getNumFatherCategoryInfos() const { return GC.getNumFatherCategoryInfos(); }
	// ArtInfos
	int getNumInterfaceArtInfos() const { return ARTFILEMGR.getNumInterfaceArtInfos(); }
	int getNumMovieArtInfos() const { return ARTFILEMGR.getNumMovieArtInfos(); }
	int getNumMiscArtInfos() const { return ARTFILEMGR.getNumMiscArtInfos(); }
	int getNumUnitArtInfos() const { return ARTFILEMGR.getNumUnitArtInfos(); }
	int getNumBuildingArtInfos() const { return ARTFILEMGR.getNumBuildingArtInfos(); }
	int getNumCivilizationArtInfos() const { return ARTFILEMGR.getNumCivilizationArtInfos(); }
	int getNumLeaderheadArtInfos() const { return ARTFILEMGR.getNumLeaderheadArtInfos(); }
	int getNumImprovementArtInfos() const { return ARTFILEMGR.getNumImprovementArtInfos(); }
	int getNumBonusArtInfos() const { return ARTFILEMGR.getNumBonusArtInfos(); }
	int getNumTerrainArtInfos() const { return ARTFILEMGR.getNumTerrainArtInfos(); }
	int getNumFeatureArtInfos() const { return ARTFILEMGR.getNumFeatureArtInfos(); }
	int getNumAnimationPathInfos() const { return GC.getNumAnimationPathInfos(); }
	int getNumAnimationCategoryInfos() const { return GC.getNumAnimationCategoryInfos(); }
	//Androrc UnitArtStyles
    int getNumUnitArtStyleTypeInfos() const { return GC.getNumUnitArtStyleTypeInfos(); }
	//Androrc End
	int getNumEntityEventTypes() const { return GC.getNumEntityEventTypes(); }
	int getNumAnimationOperatorTypes() const { return GC.getNumAnimationOperatorTypes(); }
	int getNumArtStyleTypes() const { return GC.getNumArtStyleTypes(); }
	int getNumCitySizeTypes() const { return GC.getNumCitySizeTypes(); }
	int getNumFootstepAudioTypes() const { return GC.getNumFootstepAudioTypes(); }
	//////////////////////
	// Globals Defines
	//////////////////////
	CyVariableSystem* getCyDefinesVarSystem();
	int getDefineINT( const char * szName ) const { return GC.getDefineINT( szName ); }
	float getDefineFLOAT( const char * szName ) const { return GC.getDefineFLOAT( szName ); }
	const char * getDefineSTRING( const char * szName ) const { return GC.getDefineSTRING( szName ); }
	void setDefineINT( const char * szName, int iValue ) { return GC.setDefineINT( szName, iValue ); }
	void setDefineFLOAT( const char * szName, float fValue ) { return GC.setDefineFLOAT( szName, fValue ); }
	void setDefineSTRING( const char * szName, const char * szValue ) { return GC.setDefineSTRING( szName, szValue ); }
	int getMOVE_DENOMINATOR() const { return GLOBAL_DEFINE_MOVE_DENOMINATOR; }
	int getFOOD_CONSUMPTION_PER_POPULATION() const { return GLOBAL_DEFINE_FOOD_CONSUMPTION_PER_POPULATION; }
	int getMAX_HIT_POINTS() const { return GC.getMAX_HIT_POINTS(); }
	int getHILLS_EXTRA_DEFENSE() const { return GC.getHILLS_EXTRA_DEFENSE(); }
	int getRIVER_ATTACK_MODIFIER() const { return GC.getRIVER_ATTACK_MODIFIER(); }
	int getAMPHIB_ATTACK_MODIFIER() const { return GC.getAMPHIB_ATTACK_MODIFIER(); }
	int getHILLS_EXTRA_MOVEMENT() const { return GLOBAL_DEFINE_HILLS_EXTRA_MOVEMENT; }
	int getPEAK_EXTRA_MOVEMENT() const { return GLOBAL_DEFINE_PEAK_EXTRA_MOVEMENT; }
	int getMAX_PLOT_LIST_ROWS() const { return GC.getMAX_PLOT_LIST_ROWS(); }
	int getUNIT_MULTISELECT_MAX() const { return GC.getUNIT_MULTISELECT_MAX(); }
	int getEVENT_MESSAGE_TIME() const { return GC.getEVENT_MESSAGE_TIME(); }
	int getROUTE_FEATURE_GROWTH_MODIFIER() const { return GC.getROUTE_FEATURE_GROWTH_MODIFIER(); }
	int getFEATURE_GROWTH_MODIFIER() const { return GC.getFEATURE_GROWTH_MODIFIER(); }
	int getMIN_CITY_RANGE() const { return GC.getMIN_CITY_RANGE(); }
	int getCITY_MAX_NUM_BUILDINGS() const { return GC.getCITY_MAX_NUM_BUILDINGS(); }
	int getLAKE_MAX_AREA_SIZE() const { return GC.getLAKE_MAX_AREA_SIZE(); }
	int getMIN_WATER_SIZE_FOR_OCEAN() const { return GC.getMIN_WATER_SIZE_FOR_OCEAN(); }
	int getFORTIFY_MODIFIER_PER_TURN() const { return GC.getFORTIFY_MODIFIER_PER_TURN(); }
	int getMAX_CITY_DEFENSE_DAMAGE() const { return GC.getMAX_CITY_DEFENSE_DAMAGE(); }
	int getPEAK_SEE_THROUGH_CHANGE() const { return GC.getPEAK_SEE_THROUGH_CHANGE(); }
	int getHILLS_SEE_THROUGH_CHANGE() const { return GC.getHILLS_SEE_THROUGH_CHANGE(); }
	int getSEAWATER_SEE_FROM_CHANGE() const { return GC.getSEAWATER_SEE_FROM_CHANGE(); }
	int getPEAK_SEE_FROM_CHANGE() const { return GC.getPEAK_SEE_FROM_CHANGE(); }
	int getHILLS_SEE_FROM_CHANGE() const { return GC.getHILLS_SEE_FROM_CHANGE(); }
	int getMAX_REBEL_YIELD_MODIFIER() const { return GC.getMAX_REBEL_YIELD_MODIFIER(); }

	// TAC - AI Improved Naval AI - koma13 - START
	int getAI_TRANSPORT_DANGER_RANGE() const { return GC.getAI_TRANSPORT_DANGER_RANGE(); }
	int getAI_LOST_TRANSPORT_MEMORY_COUNT() const { return GC.getAI_LOST_TRANSPORT_MEMORY_COUNT(); }
	// TAC - AI Improved Naval AI - koma13 - END

	float getCAMERA_MIN_YAW() const { return GC.getCAMERA_MIN_YAW(); }
	float getCAMERA_MAX_YAW() const { return GC.getCAMERA_MAX_YAW(); }
	float getCAMERA_FAR_CLIP_Z_HEIGHT() const { return GC.getCAMERA_FAR_CLIP_Z_HEIGHT(); }
	float getCAMERA_MAX_TRAVEL_DISTANCE() const { return GC.getCAMERA_MAX_TRAVEL_DISTANCE(); }
	float getCAMERA_START_DISTANCE() const { return GC.getCAMERA_START_DISTANCE(); }
	float getPLOT_SIZE() const { return GC.getPLOT_SIZE(); }
	float getCAMERA_SPECIAL_PITCH() const { return GC.getCAMERA_SPECIAL_PITCH(); }
	float getCAMERA_MAX_TURN_OFFSET() const { return GC.getCAMERA_MAX_TURN_OFFSET(); }
	float getCAMERA_MIN_DISTANCE() const { return GC.getCAMERA_MIN_DISTANCE(); }
	float getCAMERA_UPPER_PITCH() const { return GC.getCAMERA_UPPER_PITCH(); }
	float getCAMERA_LOWER_PITCH() const { return GC.getCAMERA_LOWER_PITCH(); }
	float getFIELD_OF_VIEW() const { return GC.getFIELD_OF_VIEW(); }
	float getUNIT_MULTISELECT_DISTANCE() const { return GC.getUNIT_MULTISELECT_DISTANCE(); }
	int getMAX_CIV_PLAYERS() const { return GC.getMAX_CIV_PLAYERS(); }
	int getMAX_PLAYERS() const { return GC.getMAX_PLAYERS(); }
	int getMAX_CIV_TEAMS() const { return GC.getMAX_CIV_TEAMS(); }
	int getMAX_TEAMS() const { return GC.getMAX_TEAMS(); }
	int getINVALID_PLOT_COORD() const { return GC.getINVALID_PLOT_COORD(); }
	int getNUM_CITY_PLOTS() const { return GC.getNUM_CITY_PLOTS(); }
	int getCITY_HOME_PLOT() const { return GC.getCITY_HOME_PLOT(); }

	/// one/two city plot radius
	int getCityDiameter() const { return CITY_PLOTS_DIAMETER;}
	int getDefaultCityCatchmentRadius() const;
	// city radius end

	int getSymbolID(int iSymbol);
	std::string getSymbolName(int iSymbol);
	void setGameFontDebug(int iChar);
	int getDomesticAdvisorState() const;
	void setDomesticAdvisorState(int iPage);
	void setExeXmlLengthOverride(bool bEnabled);

	CyUserSettings* getUserSettings() const;

	void openNetworkOOSMenu();
	void openReadme(std::string section);

	/// xml Editor - start - Nightinggale
	CyXMLEditor* getxmlEditor() const;
	/// xml Editor - end - Nightinggale
};
#endif	// CyGlobalContext_h
