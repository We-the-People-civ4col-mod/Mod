#pragma once

// cityAI.h

#ifndef CIV4_CITY_AI_H
#define CIV4_CITY_AI_H

#include "CvCity.h"

typedef std::vector<std::pair<UnitAITypes, int> > UnitTypeWeightArray;

class CvCityAI : public CvCity
{

public:

	CvCityAI();
	virtual ~CvCityAI();

	void AI_init();
	void AI_uninit();
	void AI_reset();

	void AI_doTurn();
	void AI_doNative();

	void AI_assignWorkingPlots();
	void AI_updateAssignWork();

	bool AI_avoidGrowth() const;
	void AI_setAvoidGrowth(bool bNewValue);
	bool AI_ignoreGrowth() const;

	void AI_chooseProduction();

	UnitTypes AI_bestUnit(bool bAsync = false, UnitAITypes* peBestUnitAI = NULL, bool bPickAny = false) const;
	UnitTypes AI_bestUnitAI(UnitAITypes eUnitAI, bool bAsync = false) const;

	BuildingTypes AI_bestBuilding(int iFocusFlags = 0, int iMaxTurns = 0, bool bAsync = false) const;
	BuildingTypes AI_bestBuildingThreshold(int iFocusFlags = 0, int iMaxTurns = 0, int iMinThreshold = 0, bool bAsync = false) const;
	BuildingTypes AI_bestBuildingIgnoreRequirements(int iFocusFlags = 0, int iMaxTurns = 0) const;

	bool AI_isProductionBuilding(BuildingTypes eBuilding, bool bMajorCity = false) const;	// TAC - AI Buildings - koma13
	
	int AI_buildingValue(BuildingTypes eBuilding, int iFocusFlags = 0) const;

	int AI_neededSeaWorkers() const;

	bool AI_isDefended(int iExtra = 0) const;
	bool AI_isDanger() const;

	int AI_neededDefenders() const;
	int AI_numDefenders(bool bDefenseOnly = true, bool bIncludePotential = true) const;
	int AI_numPotentialDefenders() const;
	int AI_minDefenders() const;
	int AI_neededFloatingDefenders();
	void AI_updateNeededFloatingDefenders();

	int AI_getEmphasizeAvoidGrowthCount() const;
	bool AI_isEmphasizeAvoidGrowth() const;

	bool AI_isAssignWorkDirty() const;
	void AI_setAssignWorkDirty(bool bNewValue);

	bool AI_isChooseProductionDirty() const;
	void AI_setChooseProductionDirty(bool bNewValue);

	CvCity* AI_getRouteToCity() const;
	void AI_updateRouteToCity();

	int AI_getEmphasizeYieldCount(YieldTypes eIndex) const;

	bool AI_isEmphasize(EmphasizeTypes eIndex) const;
	void AI_setEmphasize(EmphasizeTypes eIndex, bool bNewValue);
	void AI_forceEmphasizeCulture(bool bNewValue);

	int AI_getBestBuildValue(int iIndex) const;
	int AI_totalBestBuildValue(CvArea* pArea) const;

	int AI_clearFeatureValue(int iIndex) const;
	BuildTypes AI_getBestBuild(int iIndex) const;
	int AI_countBestBuilds(CvArea* pArea) const;
	void AI_updateBestBuild();

	virtual int AI_cityValue() const;

    int AI_calculateWaterWorldPercent() const;

    int AI_countNumBonuses(BonusTypes eBonus, bool bIncludeOurs, bool bIncludeNeutral, int iOtherCultureThreshold, bool bLand = true, bool bWater = true) const;

	int AI_playerCloseness(PlayerTypes eIndex, int iMaxDistance) const;
	int AI_cityThreat(bool bDangerPercent = false) const;

	int AI_getWorkersHave() const;
	int AI_getWorkersNeeded() const;
	void AI_changeWorkersHave(int iChange);
	BuildingTypes AI_bestAdvancedStartBuilding(int iPass) const;

	ProfessionTypes AI_bestPlotProfession(const CvUnit* pUnit, const CvPlot* pPlot) const;
	int AI_bestProfessionPlot(ProfessionTypes eProfession, const CvUnit* pUnit) const;
	int AI_professionValue(ProfessionTypes eProfession, const CvUnit* pUnit, const CvPlot* pPlot, const CvUnit* pDisplaceUnit) const;
	int AI_professionBasicOutput(ProfessionTypes eProfession, UnitTypes eUnit, const CvPlot* pPlot) const;
	CvUnit* AI_getWorstProfessionUnit(ProfessionTypes eProfession) const;

	int AI_unitJoinCityValue(CvUnit* pUnit, ProfessionTypes* peNewProfession) const;
	int AI_unitJoinReplaceValue(CvUnit* pUnit, CvUnit** pReplaceUnit = NULL) const;

	bool AI_canMakeGift() const;
	int AI_getGiftTimer() const;
	void AI_setGiftTimer(int iNewValue);
	void AI_changeGiftTimer(int iChange);

	// R&R, ray, Natives Trading - START
	bool AI_canMakeTrade() const;
	int AI_getTradeTimer() const;
	void AI_setTradeTimer(int iNewValue);
	void AI_changeTradeTimer(int iChange);
	// R&R, ray, Natives Trading - END

	int AI_maxGoldTrade(PlayerTypes ePlayer) const;

    int AI_calculateAlarm(PlayerTypes eIndex) const;

	int AI_estimateYieldValue(YieldTypes eYield, int iAmount) const;

	int AI_getTargetSize() const;
	void AI_setTargetSize(int iNewValue);

	int AI_getYieldOutputWeight(YieldTypes eYield) const;
	void AI_setYieldOutputWeight(YieldTypes eYield, int iNewValue);

	int AI_getNeededYield(YieldTypes eYield) const;
	void AI_setNeededYield(YieldTypes eYield, int iNewValue);

	int AI_getAverageYieldImported(YieldTypes eYield) const;
	int AI_getAverageYieldExported(YieldTypes eYield) const;

	int AI_getTradeBalance(YieldTypes eYield) const;
	void AI_changeTradeBalance(YieldTypes eYield, int iAmount);
	
	int AI_getYieldAdvantage(YieldTypes eYield) const;
	void AI_setYieldAdvantage(YieldTypes eYield, int iNewValue);

	void AI_addImportedYield(YieldTypes eYield, int iAmount);
	void AI_addExportedYield(YieldTypes eYield, int iAmount);

	void AI_assignDesiredYield();
	YieldTypes AI_getDesiredYield() const;

	void AI_updateNeededYields();

	bool AI_shouldImportYield(YieldTypes eYield) const;
	bool AI_shouldExportYield(YieldTypes eYield) const;

	int AI_getTransitYield(YieldTypes eYield) const;

	int AI_getFoodGatherable(int iPop, int iPlotFoodThreshold) const;

	bool AI_isPort() const;
	void AI_setPort(bool iNewValue);

	int AI_getFoundValue();

	int AI_getRequiredYieldLevel(YieldTypes eYield);
	void AI_updateRequiredYieldLevels();

	void AI_educateStudent(int iUnitId);

	void AI_setWorkforceHack(bool bNewValue);
	bool AI_isWorkforceHack() const;
	
	bool AI_isMajorCity() const;

	void read(FDataStreamBase* pStream); 
	void write(FDataStreamBase* pStream); 

	void read(CvSavegameReader reader);
	void write(CvSavegameWriter writer);

	void AI_doSettlerProfessionCheat();

	void AI_doHurry(bool bForce = false);
	bool canHaveCitizenProfession(const CvUnit& kUnit, ProfessionTypes eProfession, bool bBumpOther) const;

protected:

	void AI_resetSavedData();

	int m_iGiftTimer;
	int m_iTradeTimer; // R&R, ray, Natives Trading - START

	YieldTypes m_eDesiredYield;

	int m_iFoundValue;
	int m_iTargetSize;

	EnumMap<YieldTypes,int> m_em_iYieldOutputWeight;
	EnumMap<YieldTypes,int> m_em_iNeededYield;
	EnumMap<YieldTypes,int> m_em_iTradeBalance;
	EnumMap<YieldTypes,int> m_em_iYieldAdvantage;

	int m_iEmphasizeAvoidGrowthCount;

	bool m_bPort;
	bool m_bAssignWorkDirty;
	bool m_bChooseProductionDirty;

	int m_iWorkforceHack; //Does not need to be serialized.

	IDInfo m_routeToCity;

	EnumMap<YieldTypes,int> m_em_iEmphasizeYieldCount;
	bool m_bForceEmphasizeCulture;

	EnumMap<CityPlotTypes, int> m_em_iBestBuildValue;
	EnumMap<CityPlotTypes,BuildTypes> m_em_eBestBuild;

	EnumMap<EmphasizeTypes,bool> m_em_bEmphasize;

	mutable int m_iCachePlayerClosenessTurn;
	mutable int m_iCachePlayerClosenessDistance;
	EnumMap<PlayerTypes,int> m_em_iPlayerCloseness;

	int m_iNeededFloatingDefenders;
	int m_iNeededFloatingDefendersCacheTurn;

	int m_iWorkersNeeded;
	int m_iWorkersHave;

	void AI_doEmphasize();
	void AI_doNativeTrade();
	int AI_calculateCulturePressure() const;

	void AI_resetTradedYields();
	void AI_doTradedYields();

	bool AI_chooseBuild();

	bool AI_chooseUnit(UnitAITypes eUnitAI = NO_UNITAI, bool bPickAny = false);
	bool AI_chooseUnit(UnitTypes eUnit, UnitAITypes eUnitAI);

	bool AI_chooseDefender();
	bool AI_chooseLeastRepresentedUnit(UnitTypeWeightArray &allowedTypes);
	bool AI_chooseBuilding(int iFocusFlags = 0, int iMaxTurns = MAX_INT, int iMinThreshold = 0);

	// TAC - AI produces FF Points - koma13 - START
	bool AI_chooseConvince();
	FatherPointTypes AI_bestFatherPoint() const;
	// TAC - AI produces FF Points - koma13 - END

	bool AI_bestSpreadUnit(bool bMissionary, int iBaseChance, UnitTypes* eBestSpreadUnit, int* iBestSpreadUnitValue) const;
	bool AI_addBestCitizen();
	bool AI_removeWorstCitizen();
	bool AI_removeWorstPopulationUnit(bool bDelete);
	CvUnit* AI_bestPopulationUnit(UnitAITypes eUnitAI, ProfessionTypes eProfession = NO_PROFESSION);
	void AI_juggleCitizens();

	CvUnit* AI_assignToBestJob(CvUnit* pUnit, bool bIndoorOnly = false);
	CvUnit* AI_parallelAssignToBestJob(CvUnit& kUnit, bool bIndoorOnly = false);

	CvUnit* AI_juggleColonist(CvUnit* pUnit);

	void AI_swapUnits(CvUnit* pUnitA, CvUnit* pUnitB);

	bool AI_potentialPlot(const EnumMap<YieldTypes, short>& em_iYields) const;
	bool AI_foodAvailable(int iExtra = 0) const;
	int AI_yieldValue(const EnumMap<YieldTypes, short>& em_iYields, bool bAvoidGrowth, bool bRemove, bool bIgnoreFood = false, bool bIgnoreGrowth = false, bool bIgnoreStarvation = false, bool bWorkerOptimization = false) const;
	int AI_plotValue(const CvPlot* pPlot, bool bAvoidGrowth, bool bRemove, bool bIgnoreFood = false, bool bIgnoreGrowth = false, bool bIgnoreStarvation = false) const;

	int AI_experienceWeight() const;

	int AI_plotYieldValue(const CvPlot* pPlot, int* piYields) const;
	void AI_bestPlotBuild(const CvPlot* pPlot, int* piBestValue, BuildTypes* peBestBuild) const;

	int AI_getYieldMagicValue(const int* piYieldsTimes100) const;
	int AI_getPlotMagicValue(const CvPlot* pPlot, bool bWorkerOptimization = false) const;
	int AI_countGoodTiles(bool bUnworkedOnly, int iThreshold = 50, bool bWorkerOptimization = false) const;
	int AI_calculateTargetCulturePerTurn();

	int AI_buildingSpecialYieldChangeValue(BuildingTypes kBuilding, YieldTypes eYield) const;

	void AI_cachePlayerCloseness(int iMaxDistance) const;
	void AI_updateWorkersNeededHere();

	bool AI_hasCoastalRoute() const;

	void AI_assignCityPlot();

	// added so under cheat mode we can call protected functions for testing
	friend class CvGameTextMgr;
};

#endif
