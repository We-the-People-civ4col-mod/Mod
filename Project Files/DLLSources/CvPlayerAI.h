#pragma once

// playerAI.h

#ifndef CIV4_PLAYER_AI_H
#define CIV4_PLAYER_AI_H

#include "CvPlayer.h"
#include "CvTradeRouteGroup.h"

class CvEventTriggerInfo;

class CvPlayerAI : public CvPlayer
{

public:

	CvPlayerAI();
	virtual ~CvPlayerAI();

  // inlined for performance reasons
#ifdef _USRDLL
  static CvPlayerAI& getPlayer(PlayerTypes ePlayer) 
  {
	  FAssertMsg(ePlayer >= 0, "Player is not assigned a valid value");
	  FAssertMsg(ePlayer < MAX_PLAYERS, "Player is not assigned a valid value");
	  return m_aPlayers[ePlayer]; 
  }
#endif
	DllExport static CvPlayerAI& getPlayerNonInl(PlayerTypes ePlayer);

	static void initStatics();
	static void freeStatics();
	DllExport static bool areStaticsInitialized();

	void AI_init();
	void AI_uninit();
	void AI_reset();

	void AI_doTurnPre();
	void AI_doTurnPost();
	void AI_doTurnUnitsPre();
	void AI_doTurnUnitsPost();

	void AI_doPeace();
	
	void AI_doEurope();

	void AI_updateFoundValues(bool bStartingLoc = false);
	void AI_updateAreaTargets();

	int AI_movementPriority(CvSelectionGroup* pGroup);
	void AI_unitUpdate();
	// TAC - AI BTS Groups - koma13 - START
	void AI_groupUpdate();
	// TAC - AI BTS Groups - koma13 - END

	void AI_makeAssignWorkDirty();
	void AI_assignWorkingPlots();
	void AI_updateAssignWork();

	void AI_makeProductionDirty();

	void AI_conquerCity(CvCity* pCity);

	bool AI_acceptUnit(CvUnit* pUnit);
	bool AI_captureUnit(UnitTypes eUnit, CvPlot* pPlot);

	DomainTypes AI_unitAIDomainType(UnitAITypes eUnitAI) const;
	bool AI_unitAIIsCombat(UnitAITypes eUnitAI) const;

	int AI_yieldWeight(YieldTypes eYield) const;

	int AI_estimatedColonistIncome(CvPlot* pPlot, CvUnit* pColonist);
	int AI_foundValue(int iX, int iY, int iMinRivalRange = -1, bool bStartingLoc = false);

	int AI_foundValueNative(int iX, int iY, bool settleOnBonus = false) const;
	
	bool AI_isAreaAlone(CvArea* pArea);
	bool AI_isCapitalAreaAlone();
	bool AI_isPrimaryArea(CvArea* pArea);

	int AI_militaryWeight(CvArea* pArea);

	int AI_targetCityValue(CvCity* pCity, bool bRandomize, bool bIgnoreAttackers = false);
	CvCity* AI_findTargetCity(CvArea* pArea);

	int AI_getPlotDanger(CvPlot* pPlot, int iRange = -1, bool bTestMoves = true, bool bOffensive = false);
	int AI_getUnitDanger(CvUnit* pUnit, int iRange = -1, bool bTestMoves = true, bool bAnyDanger = true) const;
	
	// TAC - AI Improved Naval AI - koma13 - START
	//int AI_getWaterDanger(CvPlot* pPlot, int iRange, bool bTestMoves = true);
	int AI_getWaterDanger(CvPlot* pPlot, int iRange, bool bTestMoves = true, bool bDangerMap = false, bool bVisibleOnly = false) const;
	// TAC - AI Improved Naval AI - koma13 - END

	int AI_goldTarget();
	DllExport DiploCommentTypes AI_getGreeting(PlayerTypes ePlayer);
	bool AI_isWillingToTalk(PlayerTypes ePlayer);
	bool AI_demandRebukedSneak(PlayerTypes ePlayer);
	bool AI_demandRebukedWar(PlayerTypes ePlayer);
	DllExport bool AI_hasTradedWithTeam(TeamTypes eTeam);

	AttitudeTypes AI_getAttitude(PlayerTypes ePlayer, bool bForced = true);
	int AI_getAttitudeVal(PlayerTypes ePlayer, bool bForced = true);
	static AttitudeTypes AI_getAttitude(int iAttitudeVal);

	int AI_calculateStolenCityRadiusPlots(PlayerTypes ePlayer);
	int AI_getCloseBordersAttitude(PlayerTypes ePlayer);
	int AI_getStolenPlotsAttitude(PlayerTypes ePlayer);
	int AI_getAlarmAttitude(PlayerTypes ePlayer);
	int AI_getRebelAttitude(PlayerTypes ePlayer);
	void AI_invalidateCloseBordersAttitudeCache();
	int AI_getWarAttitude(PlayerTypes ePlayer);
	int AI_getPeaceAttitude(PlayerTypes ePlayer);
	int AI_getOpenBordersAttitude(PlayerTypes ePlayer);
	int AI_getDefensivePactAttitude(PlayerTypes ePlayer);
	int AI_getRivalDefensivePactAttitude(PlayerTypes ePlayer);
	int AI_getShareWarAttitude(PlayerTypes ePlayer);
	int AI_getTradeAttitude(PlayerTypes ePlayer);
	int AI_getRivalTradeAttitude(PlayerTypes ePlayer);
	int AI_getMemoryAttitude(PlayerTypes ePlayer, MemoryTypes eMemory);

	int AI_dealVal(PlayerTypes ePlayer, const CLinkList<TradeData>* pList, bool bIgnoreAnnual = false, int iExtra = 1);
	bool AI_goldDeal(const CLinkList<TradeData>* pList);
	bool AI_considerOffer(PlayerTypes ePlayer, const CLinkList<TradeData>* pTheirList, const CLinkList<TradeData>* pOurList, int iChange = 1);
	bool AI_counterPropose(PlayerTypes ePlayer, const CLinkList<TradeData>* pTheirList, const CLinkList<TradeData>* pOurList, CLinkList<TradeData>* pTheirInventory, CLinkList<TradeData>* pOurInventory, CLinkList<TradeData>* pTheirCounter, CLinkList<TradeData>* pOurCounter, const IDInfo& kTransport);
	int AI_militaryHelp(PlayerTypes ePlayer, int& iNumUnits, UnitTypes& eUnit, ProfessionTypes& eProfession);

	int AI_maxGoldTrade(PlayerTypes ePlayer) const;

	int AI_cityTradeVal(CvCity* pCity, PlayerTypes eOwner = NO_PLAYER);
	DenialTypes AI_cityTrade(CvCity* pCity, PlayerTypes ePlayer) const;

	int AI_stopTradingTradeVal(TeamTypes eTradeTeam, PlayerTypes ePlayer);
	DenialTypes AI_stopTradingTrade(TeamTypes eTradeTeam, PlayerTypes ePlayer) const;

	int AI_yieldTradeVal(YieldTypes eYield, const IDInfo& kTransport, PlayerTypes ePlayer);
	DenialTypes AI_yieldTrade(YieldTypes eYield, const IDInfo& kTransport, PlayerTypes ePlayer) const;
	
	int AI_calculateDamages(TeamTypes eTeam);

	int AI_unitImpassableCount(UnitTypes eUnit);
	int AI_unitEconomicValue(UnitTypes eUnit, UnitAITypes* peUnitAI, CvCity* pCity);
	int AI_unitValue(UnitTypes eUnit, UnitAITypes eUnitAI, CvArea* pArea);
	int AI_unitGoldValue(UnitTypes eUnit, UnitAITypes eUnitAI, CvArea* pArea) const;
	int AI_unitValuePercent(UnitTypes eUnit, UnitAITypes* peUnitAI, CvArea* pArea);
	int AI_totalUnitAIs(UnitAITypes eUnitAI);
	int AI_totalAreaUnitAIs(CvArea* pArea, UnitAITypes eUnitAI);
	int AI_totalWaterAreaUnitAIs(CvArea* pArea, UnitAITypes eUnitAI);
	bool AI_hasSeaTransport(const CvUnit* pCargo) const;

	int AI_neededExplorers(CvArea* pArea);
	int AI_neededWorkers(CvArea* pArea);
	//int AI_neededMissionary(CvArea* pArea);
	
	int AI_adjacentPotentialAttackers(CvPlot* pPlot, bool bTestCanMove = false);
	// TAC - AI City Defense - koma13 - START
	//int AI_totalMissionAIs(MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup = NULL);
	int AI_totalMissionAIs(MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup = NULL, UnitAITypes eUnitAI = NO_UNITAI);
	// TAC - AI City Defense - koma13 - END

	int AI_areaMissionAIs(CvArea* pArea, MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup = NULL);
	int AI_adjacantToAreaMissionAIs(CvArea* pArea, MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup = NULL);
	int AI_plotTargetMissionAIs(CvPlot* pPlot, MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup = NULL, int iRange = 0);
	int AI_plotTargetMissionAIs(CvPlot* pPlot, MissionAITypes eMissionAI, int& iClosestTargetRange, CvSelectionGroup* pSkipSelectionGroup = NULL, int iRange = 0);
	int AI_plotTargetMissionAIs(CvPlot* pPlot, MissionAITypes* aeMissionAI, int iMissionAICount, int& iClosestTargetRange, CvSelectionGroup* pSkipSelectionGroup = NULL, int iRange = 0);
	int AI_unitTargetMissionAIs(CvUnit* pUnit, MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup = NULL);
	
	int AI_unitTargetMissionAIs(CvUnit* pUnit, MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup, UnitAITypes eUnitAI);	// TAC - AI Attack City - koma13

	int AI_unitTargetMissionAIs(CvUnit* pUnit, MissionAITypes* aeMissionAI, int iMissionAICount, CvSelectionGroup* pSkipSelectionGroup = NULL);
	int AI_enemyTargetMissionAIs(MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup = NULL);
	int AI_enemyTargetMissionAIs(MissionAITypes* aeMissionAI, int iMissionAICount, CvSelectionGroup* pSkipSelectionGroup = NULL);
	int AI_wakePlotTargetMissionAIs(CvPlot* pPlot, MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup = NULL);

	int AI_cargoSpaceToEurope(CvSelectionGroup* pSkipSelectionGroup = NULL);	// TAC - AI Improved Navel AI - koma13

	int AI_cityTargetUnitsByPath(CvCity* pCity, CvSelectionGroup* pSkipSelectionGroup, int iMaxPathTurns) const;	// TAC - AI Attack City - koma13, jdog5000(BBAI)
	int AI_unitTargetMissionAIs(CvUnit* pUnit, MissionAITypes* aeMissionAI, int iMissionAICount, CvSelectionGroup* pSkipSelectionGroup, int iMaxPathTurns) const;	// TAC - AI Assault Sea - koma13, jdog5000(BBAI)

	int AI_unitTargetMissionAIs(CvUnit* pUnit, MissionAITypes* aeMissionAI, int iMissionAICount, CvSelectionGroup* pSkipSelectionGroup, int iMaxPathTurns, UnitAITypes eUnitAI) const;	// TAC - AI Attack City - koma13

	CivicTypes AI_bestCivic(CivicOptionTypes eCivicOption);
	int AI_civicValue(CivicTypes eCivic);

	int AI_getAttackOddsChange();
	void AI_setAttackOddsChange(int iNewValue);

	int AI_getExtraGoldTarget() const;
	void AI_setExtraGoldTarget(int iNewValue);

	void AI_chooseCivic(CivicOptionTypes eCivicOption);
	bool AI_chooseGoody(GoodyTypes eGoody);

	CvCity* AI_findBestCity() const;
	// TAC - AI Economy - koma13 - START
	//CvCity* AI_findBestPort() const;
	CvCity* AI_findBestPort(CvArea* pArea = NULL, CvCity* pNoAccessCity = NULL) const;
	void AI_updateBestPortCities();
	// TAC - AI Economy - koma13 - END

	int AI_getNumTrainAIUnits(UnitAITypes eIndex);
	void AI_changeNumTrainAIUnits(UnitAITypes eIndex, int iChange);

	int AI_getNumAIUnits(UnitAITypes eIndex);
	void AI_changeNumAIUnits(UnitAITypes eIndex, int iChange);
	
	int AI_getNumRetiredAIUnits(UnitAITypes eIndex);
	void AI_changeNumRetiredAIUnits(UnitAITypes eIndex, int iChange);

	int AI_getPeacetimeTradeValue(PlayerTypes eIndex);
	void AI_changePeacetimeTradeValue(PlayerTypes eIndex, int iChange);

	int AI_getPeacetimeGrantValue(PlayerTypes eIndex);
	void AI_changePeacetimeGrantValue(PlayerTypes eIndex, int iChange);

	int AI_getGoldTradedTo(PlayerTypes eIndex) const;
	void AI_changeGoldTradedTo(PlayerTypes eIndex, int iChange);

	int AI_getAttitudeExtra(PlayerTypes eIndex);
	void AI_setAttitudeExtra(PlayerTypes eIndex, int iNewValue);
	void AI_changeAttitudeExtra(PlayerTypes eIndex, int iChange);

	bool AI_isFirstContact(PlayerTypes eIndex);
	void AI_setFirstContact(PlayerTypes eIndex, bool bNewValue);

	int AI_getContactTimer(PlayerTypes eIndex1, ContactTypes eIndex2);
	void AI_changeContactTimer(PlayerTypes eIndex1, ContactTypes eIndex2, int iChange);

	int AI_getMemoryCount(PlayerTypes eIndex1, MemoryTypes eIndex2);
	void AI_changeMemoryCount(PlayerTypes eIndex1, MemoryTypes eIndex2, int iChange);

	EventTypes AI_chooseEvent(int iTriggeredId);
	
    int AI_countDeadlockedBonuses(CvPlot* pPlot);
    
    int AI_getOurPlotStrength(CvPlot* pPlot, int iRange, bool bDefensiveBonuses, bool bTestMoves) const;
    int AI_getEnemyPlotStrength(CvPlot* pPlot, int iRange, bool bDefensiveBonuses, bool bTestMoves);

	int AI_goldToUpgradeAllUnits(int iExpThreshold = 0);

	int AI_goldTradeValuePercent();
	
	int AI_averageYieldMultiplier(YieldTypes eYield);
	
	int AI_playerCloseness(PlayerTypes eIndex, int iMaxDistance);
	int AI_targetValidity(PlayerTypes ePlayer);
	
	// TAC - AI City Defense - koma13 - START
	//int AI_totalDefendersNeeded(int* piUndefendedCityCount);
	int AI_totalDefendersNeeded(int* piUndefendedCityCount, CvArea* pArea = NULL, bool bIncludeStacks = false);
	// TAC - AI City Defense - koma13 - END

	int AI_getTotalCityThreat();
	int AI_getTotalFloatingDefenseNeeded();
	
	
	int AI_getTotalAreaCityThreat(CvArea* pArea);
	int AI_countNumAreaHostileUnits(CvArea* pArea, bool bPlayer, bool bTeam, bool bNeutral, bool bHostile);
	int AI_getTotalFloatingDefendersNeeded(CvArea* pArea);
	int AI_getTotalFloatingDefenders(CvArea* pArea);

	RouteTypes AI_bestAdvancedStartRoute(CvPlot* pPlot, int* piYieldValue = NULL);
	UnitTypes AI_bestAdvancedStartUnitAI(CvPlot* pPlot, UnitAITypes eUnitAI);
	CvPlot* AI_advancedStartFindCapitalPlot();
	
	bool AI_advancedStartPlaceExploreUnits(bool bLand);
	void AI_advancedStartRevealRadius(CvPlot* pPlot, int iRadius);
	bool AI_advancedStartPlaceCity(CvPlot* pPlot);
	bool AI_advancedStartDoRoute(CvPlot* pFromPlot, CvPlot* pToPlot);
	void AI_advancedStartRouteTerritory();
	void AI_doAdvancedStart(bool bNoExit = false);
	
	int AI_getMinFoundValue();
	
	int AI_bestAreaUnitAIValue(UnitAITypes eUnitAI, CvArea* pArea, UnitTypes* peBestUnitType = NULL);
	int AI_bestCityUnitAIValue(UnitAITypes eUnitAI, CvCity* pCity, UnitTypes* peBestUnitType = NULL);
	
	int AI_calculateTotalBombard(DomainTypes eDomain);
	
	int AI_getUnitClassWeight(UnitClassTypes eUnitClass);
	int AI_getUnitCombatWeight(UnitCombatTypes eUnitCombat);
	int AI_calculateUnitAIViability(UnitAITypes eUnitAI, DomainTypes eDomain);
	
	int AI_getAttitudeWeight(PlayerTypes ePlayer);
	
	int AI_getPlotCanalValue(CvPlot* pPlot) const; // vetiarvind, editied function for Super Forts, signature marked as const
		
	int AI_getPlotChokeValue(CvPlot* pPlot) const; // Super Forts begin *choke*
	
	void AI_nativeYieldGift(CvUnit* pUnit);

	void AI_nativeTrade(CvUnit* pUnit); // R&R, ray, Natives Trading - START
	
	bool AI_isYieldForSale(YieldTypes eYield) const;
	
	bool AI_isYieldNeeded(YieldTypes eYield, int iCapacityPercent = -1, CvCity* pCity = NULL) const;	// TAC - AI Economy - koma13

	bool AI_isYieldFinalProduct(YieldTypes eYield) const;
	bool AI_shouldBuyFromEurope(YieldTypes eYield) const;
	
	// TAC - AI More food - koma13 - START
	//int AI_yieldValue(YieldTypes eYield, bool bProduce = true, int iAmount = 1);
	int AI_yieldValue(YieldTypes eYield, bool bProduce = true, int iAmount = 1, bool bFood = false);
	// TAC - AI More food - koma13 - END

	void AI_updateYieldValues();
	int AI_transferYieldValue(const IDInfo target, YieldTypes eYield, int iAmount);
	
	int AI_countYieldWaiting();
	int AI_highestYieldAdvantage(YieldTypes eYield);
	
	void AI_manageEconomy();

	CvPlot* AI_getTerritoryCenter() const;
	int AI_getTerritoryRadius() const;
	
	void AI_createNatives();
	void AI_createNativeCities();
	
	bool AI_isKing();
	
	CvPlot* AI_getImperialShipSpawnPlot();

	void AI_addUnitToMoveQueue(CvUnit* pUnit);
	void AI_removeUnitFromMoveQueue(CvUnit* pUnit);
	void AI_verifyMoveQueue();
	CvUnit* AI_getNextMoveUnit();
	
	int AI_highestProfessionOutput(ProfessionTypes eProfession, const CvCity* pIgnoreCity = NULL);

	CvCity* AI_bestCityForBuilding(BuildingTypes eBuilding);
	
	UnitTypes AI_bestUnit(UnitAITypes eUnitAI = NO_UNITAI, CvArea* pArea = NULL);

	int AI_desiredCityCount();
	
	int AI_professionValue(ProfessionTypes eProfession, UnitAITypes eUnitAI);
	int AI_professionGoldValue(ProfessionTypes eProfession);
	ProfessionTypes AI_idealProfessionForUnit(UnitTypes eUnitType);
	ProfessionTypes AI_idealProfessionForUnitAIType(UnitAITypes eUnitAI, CvCity* pCity = NULL);
	
	int AI_professionBasicValue(ProfessionTypes eProfession, UnitTypes eUnit, CvCity* pCity);
	int AI_professionUpgradeValue(ProfessionTypes eProfession, UnitTypes eUnit);
	
	int AI_unitAIValueMultipler(UnitAITypes eUnitAI);
	int AI_professionSuitability(UnitTypes eUnit, ProfessionTypes eProfession) const;
	int AI_professionSuitability(const CvUnit* pUnit, ProfessionTypes eProfession, const CvPlot* pPlot, UnitAITypes eUnitAI = NO_UNITAI) const;
	
	void AI_swapUnitJobs(CvUnit* pUnitA, CvUnit* pUnitB);
	
	bool AI_isCityAcceptingYield(CvCity* pCity, YieldTypes eYield);
	
	bool AI_isTradeRouteConnection(CvCity* pSourceCity, CvCity* pDestinationCity); // TAC - AI Economy - koma13
	
	int AI_sumAttackerStrength(CvPlot* pPlot, CvPlot* pAttackedPlot, int iRange = 1, DomainTypes eDomainType = NO_DOMAIN, bool bCheckCanAttack = false, bool bCheckCanMove = false);
	int AI_sumEnemyStrength(CvPlot* pPlot, int iRange = 0, bool bAttack = false, DomainTypes eDomainType = NO_DOMAIN);
	
	int AI_setUnitAIStatesRange(CvPlot* pPlot, int iRange, UnitAIStates eNewUnitAIState, UnitAIStates eValidUnitAIState, const std::vector<UnitAITypes>& validUnitAITypes);
	
	void AI_diplomaticHissyFit(PlayerTypes ePlayer, int iAttitudeChange);
	
	UnitTypes AI_nextBuyUnit(UnitAITypes* peUnitAI = NULL, int* piValue = NULL);
	UnitTypes AI_nextBuyProfessionUnit(ProfessionTypes* peProfession = NULL, UnitAITypes* peUnitAI = NULL, int* piValue = NULL);
	
	// TAC - AI purchases military units - koma13 - START
	//void AI_updateNextBuyUnit();
	void AI_updateNextBuyUnit(bool bPriceLimit = true);
	// TAC - AI purchases military units - koma13 - END
	void AI_updateNextBuyProfession();
	int AI_highestNextBuyValue();
	
	EmotionTypes AI_strongestEmotion();
	int AI_emotionWeight(EmotionTypes eEmotion);
	int AI_getEmotion(EmotionTypes eEmotion);
	void AI_setEmotion(EmotionTypes eEmotion, int iNewValue);
	void AI_changeEmotion(EmotionTypes eEmotion, int iChange);
	
	bool AI_isAnyStrategy() const;
	bool AI_isStrategy(StrategyTypes eStrategy) const;
	int AI_getStrategyDuration(StrategyTypes eStrategy) const;
	int AI_getStrategyData(StrategyTypes eStrategy);
	void AI_setStrategy(StrategyTypes eStrategy, int iData = -1);
	void AI_clearStrategy(StrategyTypes eStrategy);
	
	UnitAITypes AI_bestBuildupUnitAI();	// TAC - AI Military Buildup - koma13
	
	bool AI_isPathDanger(const CvSelectionGroup* pGroup, const CvPlot* pFromPlot, const CvPlot* pToPlot, int iRange = -1) const;	// TAC - AI Improved Naval AI - koma13
	
	int AI_cityDistance(CvPlot* pPlot);
	std::vector<short> *AI_getDistanceMap();
	void AI_invalidateDistanceMap();
	
	void AI_updateBestYieldPlots();
	
	CvPlot* AI_getBestWorkedYieldPlot(YieldTypes eYield);
	CvPlot* AI_getBestUnworkedYieldPlot(YieldTypes eYield);
	int AI_getBestPlotYield(YieldTypes eYield);
	
	void AI_changeTotalIncome(int iChange);
	int AI_getTotalIncome();
	
	void AI_changeHurrySpending(int iChange);
	int AI_getHurrySpending();
	
	// TAC - AI More Immigrants - koma13 - START
	void AI_changeImmigrantSpending(int iChange);
	int AI_getImmigrantSpending();
	// TAC - AI More Immigrants - koma13 - END
	// TAC - AI Economy - koma13 - START
	void AI_changeEuropeYieldSpending(int iChange);
	int AI_getEuropeYieldSpending();
	// TAC - AI Economy - koma13 - END

	bool AI_needsProtection(UnitAITypes eUnitAI) const; // TAC - AI Improved Naval AI - koma13

	int AI_getPopulation();
	bool AI_shouldAttackAdjacentCity(CvPlot* pPlot);
	
	int AI_getNumProfessionUnits(ProfessionTypes eProfession);
	int AI_countNumCityUnits(UnitTypes eUnit);
	
	int AI_getNumCityUnitsNeeded(UnitTypes eUnit);
	
	int AI_countPromotions(PromotionTypes ePromotion, CvPlot* pPlot, int iRange, int* piUnitCount = NULL);
	
	// TAC - AI More Immigrants - koma13 - START
	void AI_hurryDocks();
	// TAC - AI More Immigrants - koma13 - END
	
	bool AI_prepareAssaultSea();	// TAC - AI Assault Sea - koma13
	
	void AI_doNativeArmy(TeamTypes eTeam);
	
	CvCity* AI_getPrimaryCity();
	int AI_getOverpopulationPercent();
	int AI_countNumHomedUnits(CvCity* pCity, UnitAITypes eUnitAI, UnitAIStates eUnitAIState);

	// TAC - AI Revolution - koma13 - START
	int AI_getLastWave() const;
	void AI_setLastWave(int iTurn);
	int AI_getWaveIndex() const;
	void AI_setWaveIndex(int iNum);
	// TAC - AI Revolution - koma13 - END
	bool AI_isLandLocked(bool bIgnoreCultureLock = false) const;	// TAC - AI City Sites - koma13
	
	// R&R, ray, Natives raiding party - START
	bool AI_isPotentialRaidTarget(PlayerTypes ePlayer);
	bool AI_hasPotentialRaidTarget();
	int AI_getNumPotentialRaids();
	// R&R, ray, Natives raiding party - END
	
	// for serialization
  virtual void read(FDataStreamBase* pStream);
  virtual void write(FDataStreamBase* pStream);

  int AI_getYieldBestExportPrice(YieldTypes eYield) const;

protected:

    /** NBMOD TAX **/
    int CvPlayerAI::NBMOD_GetGoldAsk(PlayerTypes ePlayer) const; // Beachtet die Gold-Höchstgrenze
    /** NBMOD TAX **/

	static CvPlayerAI* m_aPlayers;

	std::vector<short> m_distanceMap;
	int m_iDistanceMapDistance;

	int m_iAttackOddsChange;
	int m_iExtraGoldTarget;
	
	UnitTypes m_eNextBuyUnit;
	UnitAITypes m_eNextBuyUnitAI;
	int m_iNextBuyUnitValue;
	
	ProfessionTypes m_eNextBuyProfession;
	UnitTypes m_eNextBuyProfessionUnit;
	UnitAITypes m_eNextBuyProfessionAI;
	int m_iNextBuyProfessionValue;
	
	int m_iTotalIncome;
	int m_iHurrySpending;
	// TAC - AI More Immigrants - koma13 - START
	int m_iImmigrantSpending;
	// TAC - AI More Immigrants - koma13 - END
	int m_iEuropeYieldSpending;	// TAC - AI Economy - koma13
	
	int m_iAveragesCacheTurn;
	
	int *m_aiAverageYieldMultiplier;
	int* m_aiYieldValuesTimes100;
	int* m_aiBestWorkedYieldPlots;
	int* m_aiBestUnworkedYieldPlots;
	
	int m_iUpgradeUnitsCacheTurn;
	int m_iUpgradeUnitsCachedExpThreshold;
	int m_iUpgradeUnitsCachedGold;
	
	
	int* m_aiNumTrainAIUnits;
	int* m_aiNumAIUnits;
	int* m_aiNumRetiredAIUnits;
	int* m_aiUnitAIStrategyWeights;
	int* m_aiPeacetimeTradeValue;
	int* m_aiPeacetimeGrantValue;
	int* m_aiGoldTradedTo;
	int* m_aiAttitudeExtra;
	int* m_aiUnitClassWeights;
	int* m_aiUnitCombatWeights;
	int* m_aiEmotions;
	int* m_aiStrategyStartedTurn;
	int* m_aiStrategyData;

	mutable int* m_aiCloseBordersAttitudeCache;
	mutable int* m_aiStolenPlotsAttitudeCache;


	bool* m_abFirstContact;

	int** m_aaiContactTimer;
	int** m_aaiMemoryCount;
	
	std::vector<int> m_aiAICitySites;
	
	std::vector<int> m_unitPriorityHeap;
	
	int m_iTurnLastProductionDirty;
	int m_iTurnLastManagedPop;
	int m_iMoveQueuePasses;

	void AI_doTradeRoutes();
	void AI_doCounter();
	void AI_doMilitary();
	void AI_doDiplo();
	bool AI_doDiploCancelDeals(PlayerTypes ePlayer);
	bool AI_doDiploOfferCity(PlayerTypes ePlayer);
	bool AI_doDiploOfferAlliance(PlayerTypes ePlayer);
	bool AI_doDiploAskJoinWar(PlayerTypes ePlayer);
	bool AI_doDiploAskStopTrading(PlayerTypes ePlayer);
	bool AI_doDiploGiveHelp(PlayerTypes ePlayer);
	bool AI_doDiploAskForHelp(PlayerTypes ePlayer);
	bool AI_doDiploDemandTribute(PlayerTypes ePlayer);
	bool AI_doDiploKissPinky(PlayerTypes ePlayer);
	bool AI_doDiploOpenBorders(PlayerTypes ePlayer);
	bool AI_doDiploDefensivePact(PlayerTypes ePlayer);
	bool AI_doDiploTradeMap(PlayerTypes ePlayer);
	bool AI_doDiploDeclareWar(PlayerTypes ePlayer);
	
	void AI_doProfessions();
	
	void AI_doMilitaryStrategy();
	void AI_doSuppressRevolution();
	void AI_doUnitAIWeights();
	void AI_doEmotions();
	void AI_doStrategy();
	
	void AI_calculateAverages();
	
	void AI_convertUnitAITypesForCrush();
	int AI_eventValue(EventTypes eEvent, const EventTriggeredData& kTriggeredData);
		
	void AI_doEnemyUnitData();
	
	// TAC - AI Revolution - koma13 - START
	int m_iLastWave;
	int m_iWaveIndex;
	// TAC - AI Revolution - koma13 - END
	
	
	friend class CvGameTextMgr;
};

// helper for accessing static functions
#ifdef _USRDLL
#define GET_PLAYER CvPlayerAI::getPlayer
#else
#define GET_PLAYER CvPlayerAI::getPlayerNonInl
#endif

#endif
