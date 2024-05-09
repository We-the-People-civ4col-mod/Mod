#pragma once
#ifndef CyPlayer_h
#define CyPlayer_h
//
// Python wrapper class for CvPlayer
//

//#include "CvEnums.h"
//#include "CvStructs.h"

#include <boost/python/tuple.hpp>
namespace python = boost::python;

class CyUnit;
class CvPlayer;
class CyCity;
class CyArea;
class CyPlot;
class CySelectionGroup;
class CyTradeRoute;
class CyTradeRouteGroup;

class CyPlayer
{
public:

/** NBMOD TAX **/
	int NBMOD_GetMaxTaxRate() const;
/** NBMOD TAX **/

	// TAC - Python Export - Ray - START
	void NBMOD_IncreaseMaxTaxRate();
	void NBMOD_DecreaseMaxTaxRate();
	// TAC - Python Export - Ray - END
	// R&R, ray, Bargaining - Start
	bool tryGetNewBargainPriceSell();
	bool tryGetNewBargainPriceBuy();
	// R&R, ray, Bargaining - End
	CyPlayer();
	CyPlayer(CvPlayer* pPlayer);		// Call from C++
	CvPlayer* getPlayer() { return m_pPlayer;	}	// Call from C++
	// PatchMod: Achievements START
	bool isAchieveGained(int /*AchieveTypes*/ eAchieve);
	int getAchieveYear(int /*AchieveTypes*/ eAchieve);
	// PatchMod: Achievements END
	bool isNone() { return (m_pPlayer==NULL); }
	int startingPlotRange();
	CyPlot* findStartingPlot(bool bRandomize);
	CyCity* initCity(int x, int y);
	void acquireCity(CyCity* pCity, bool bConquest, bool bTrade);
	void killCities();
	std::wstring getNewCityName();
	CyUnit* initUnit(int /*UnitTypes*/ iIndex, int /*ProfessionTypes*/ eProfession, int iX, int iY, UnitAITypes eUnitAI, DirectionTypes eFacingDirection, int iYieldStored);
	CyUnit* initEuropeUnit(int /*UnitTypes*/ eUnit, UnitAITypes eUnitAI, DirectionTypes eFacingDirection);
	void killUnits();
	bool hasTrait(int /*TraitTypes*/ iIndex);
	bool isHuman();
	bool isNative();
	std::wstring getName();
	std::wstring getNameForm(int iForm);
	std::wstring getNameKey();
	std::wstring getCivilizationDescription(int iForm);
	std::wstring getCivilizationDescriptionKey();
	std::wstring getCivilizationShortDescription(int iForm);
	std::wstring getCivilizationShortDescriptionKey();
	std::wstring getCivilizationAdjective(int iForm);
	std::wstring getCivilizationAdjectiveKey();
	std::wstring getWorstEnemyName();
	int /*ArtStyleTypes*/ getArtStyleType();
	std::string getUnitButton(int eUnit);
	int findBestFoundValue();
	int countNumCoastalCities();
	int countNumCoastalCitiesByArea(CyArea* pArea);
	int countTotalCulture();
	int countTotalYieldStored(int /*YieldTypes*/ eYield);
	int countCityFeatures(int /*FeatureTypes*/ eFeature);
	int countNumBuildings(int /*BuildingTypes*/ eBuilding);
	bool canContact(int /*PlayerTypes*/ ePlayer);
	void contact(int /*PlayerTypes*/ ePlayer);
	bool canTradeWith(int /*PlayerTypes*/ eWhoTo);
	bool canTradeItem(int /*PlayerTypes*/ eWhoTo, TradeData item, bool bTestDenial);
	DenialTypes getTradeDenial(int /*PlayerTypes*/ eWhoTo, TradeData item);
	bool canStopTradingWithTeam(int /*TeamTypes*/ eTeam);
	void stopTradingWithTeam(int /*TeamTypes*/ eTeam);
	void killAllDeals();
	bool isTurnActive( void );
	void findNewCapital();
	bool canRaze(CyCity* pCity);
	void raze(CyCity* pCity);
	void disband(CyCity* pCity);
	bool canReceiveGoody(CyPlot* pPlot, int /*GoodyTypes*/ eGoody, CyUnit* pUnit);
	void receiveGoody(CyPlot* pPlot, int /*GoodyTypes*/ eGoody, CyUnit* pUnit);
	void doGoody(CyPlot* pPlot, CyUnit* pUnit);
	bool canFound(int iX, int iY);
	void found(int iX, int iY);
	bool canTrain(int /*UnitTypes*/ eUnit, bool bContinue, bool bTestVisible);
	bool canConstruct(int /*BuildingTypes*/eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost);
	int getUnitYieldProductionNeeded(int /*UnitTypes*/ eUnit, int /*YieldTypes*/ eYield) const;
	int getBuildingYieldProductionNeeded(int /*BuildingTypes*/ eBuilding, int /*YieldTypes*/ eYield) const;
	int getBuildingClassPrereqBuilding(int /*BuildingTypes*/ eBuilding, int /*BuildingClassTypes*/ ePrereqBuildingClass, int iExtra);
	void removeBuildingClass(int /*BuildingClassTypes*/ eBuildingClass);
	bool canBuild(CyPlot* pPlot, int /*BuildTypes*/ eBuild, bool bTestEra, bool bTestVisible);
	int /*RouteTypes*/ getBestRoute(CyPlot* pPlot) const;
	int getImprovementUpgradeRate() const;
	int calculateTotalYield(int /*YieldTypes*/ eYield);
	bool isCivic(int /*CivicTypes*/ eCivic);
	bool canDoCivics(int /*CivicTypes*/ eCivic);
	int greatGeneralThreshold();
	int greatAdmiralThreshold(); // R&R, ray, Great Admirals
	int getImmigrationThresholdModifierFromUnitsWaitingOnDock(); // WTP, ray, increase threshold if more than X units waiting on the docks - START
	int immigrationThreshold();
	int revolutionEuropeUnitThreshold();
	CyPlot* getStartingPlot();
	void setStartingPlot(CyPlot* pPlot, bool bUpdateStartDist);
	int getTotalPopulation();
	int getAveragePopulation();
	long getRealPopulation();
	int getTotalLand();
	int getTotalLandScored();
	int getGold();
	void setGold(int iNewValue);
	void changeGold(int iChange);
	int getAdvancedStartPoints();
	void setAdvancedStartPoints(int iNewValue);
	void changeAdvancedStartPoints(int iChange);
	int getAdvancedStartUnitCost(int /*UnitTypes*/ eUnit, bool bAdd, CyPlot* pPlot);
	int getAdvancedStartCityCost(bool bAdd, CyPlot* pPlot);
	int getAdvancedStartPopCost(bool bAdd, CyCity* pCity);
	int getAdvancedStartCultureCost(bool bAdd, CyCity* pCity);
	int getAdvancedStartBuildingCost(int /*BuildingTypes*/ eBuilding, bool bAdd, CyCity* pCity);
	int getAdvancedStartImprovementCost(int /*ImprovementTypes*/ eImprovement, bool bAdd, CyPlot* pPlot);
	int getAdvancedStartRouteCost(int /*RouteTypes*/ eRoute, bool bAdd, CyPlot* pPlot);
	int getAdvancedStartVisibilityCost(bool bAdd, CyPlot* pPlot);
	void createGreatGeneral(int eGreatGeneralUnit, bool bIncrementExperience, int iX, int iY);
	int getGreatGeneralsCreated();
	int getGreatGeneralsThresholdModifier();
	// R&R, ray, Great Admirals - START
	void createGreatAdmiral(int eGreatAdmiralUnit, bool bIncrementExperience, int iX, int iY);
	int getGreatAdmiralsCreated();
	int getGreatAdmiralsThresholdModifier(); 
	// R&R, ray, Great Admirals - END
	// WTP, ray, Lieutenants and Captains - START
	void createBraveLieutenant(int eBraveLieutenantUnit, int iX, int iY);
	void createCapableCaptain(int eGreatAdmiralUnit, int iX, int iY);
	// WTP, ray, Lieutenants and Captains - END
	int getNumShips(); // WTP, ray, easily counting Ships - START
	int getGreatGeneralRateModifier();
	int getDomesticGreatGeneralRateModifier();
	int getFreeExperience();
	int getWorkerSpeedModifier();
	int getImprovementUpgradeRateModifier();
	int getMilitaryProductionModifier();
	int getCityDefenseModifier();
	int getHighestUnitLevel();
	bool getExpInBorderModifier();
	int getLevelExperienceModifier() const;
	CyCity* getCapitalCity();
	int getCitiesLost();
	int getAssets();
	void changeAssets(int iChange);
	int getPower();
	int getPopScore();
	int getLandScore();
	int getFatherScore();
	int getTotalTimePlayed();
	bool isAlive();
	bool isEverAlive();
	bool isExtendedGame();
	bool isFoundedFirstCity();
	bool isStrike();
	int getID() const;
	WidgetTypes getWikiWidget() const;
	int getChar() const;
	int /* HandicapTypes */ getHandicapType();
	int /* CivilizationTypes */ getCivilizationType();
	int /*LeaderHeadTypes*/ getLeaderType();
	int /*LeaderHeadTypes*/ getPersonalityType();
	void setPersonalityType(int /*LeaderHeadTypes*/ eNewValue);
	int /*ErasTypes*/ getCurrentEra();
	void setCurrentEra(int /*EraTypes*/ iNewValue);
	int /*PlayerTypes*/ getParent();
	int getTeam();
	int /*PlayerColorTypes*/ getPlayerColor();
	int getPlayerTextColorR();
	int getPlayerTextColorG();
	int getPlayerTextColorB();
	int getPlayerTextColorA();
	int getSeaPlotYield(YieldTypes eIndex);
	// R&R, Robert Surcouf, No More Variables Hidden game option START
	int getYieldTradedTotal(YieldTypes eIndex);
	int getYieldTradedTotalINT(int /*YieldTypes*/ eIndex);
    // R&R, Robert Surcouf, No More Variables Hidden game option END
	// WTP, ray, Yields Traded Total for Africa and Port Royal - START
	int getYieldTradedTotalAfrica(YieldTypes eIndex);
	int getYieldTradedTotalINTAfrica(int /*YieldTypes*/ eIndex);
	int getYieldTradedTotalPortRoyal(YieldTypes eIndex);
	int getYieldTradedTotalINTPortRoyal(int /*YieldTypes*/ eIndex);
	// WTP, ray, Yields Traded Total for Africa and Port Royal - END
	int getYieldScoreTotalINT(int /*YieldTypes*/ eIndex); // R&R, vetiarvind, Price dependent tax rate change
	int getYieldRate(YieldTypes eIndex);
	int getHappinessRate(); // WTP, ray, Happiness - START
	int getUnHappinessRate(); // WTP, ray, Happiness - START
	int getLawRate(); // WTP, ray, Crime and Law - START
	int getCrimeRate(); // WTP, ray, Crime and Law - START
	int getYieldRateModifier(YieldTypes eIndex);
	int getCapitalYieldRateModifier(YieldTypes eIndex);
	int getExtraYieldThreshold(YieldTypes eIndex);
	bool isYieldEuropeTradable(int /*YieldTypes*/ eIndex);
	void setYieldEuropeTradable(int /*YieldTypes*/ eIndex, bool bTradeable);
	bool isFeatAccomplished(int /*FeatTypes*/ eIndex);
	void setFeatAccomplished(int /*FeatTypes*/ eIndex, bool bNewValue);
	bool shouldDisplayFeatPopup(int /*FeatTypes*/ eIndex);
	bool isOption(int /*PlayerOptionTypes*/ eIndex);
	void setOption(int /*PlayerOptionTypes*/ eIndex, bool bNewValue);
	bool isPlayable();
	void setPlayable(bool bNewValue);
	int getImprovementCount(int /*ImprovementTypes*/ iIndex);
	bool isBuildingFree(int /*BuildingTypes*/ iIndex);
	int getUnitClassCount(int /*UnitClassTypes*/ eIndex);
	int getUnitClassMaking(int /*UnitClassTypes*/ eIndex);
	int getUnitClassCountPlusMaking(int /*UnitClassTypes*/ eIndex);
	int getBuildingClassCount(int /*BuildingClassTypes*/ iIndex);
	int getBuildingClassMaking(int /*BuildingClassTypes*/ iIndex);
	int getBuildingClassCountPlusMaking(int /*BuildingClassTypes*/ iIndex);
	int getHurryCount(int /*HurryTypes*/ eIndex);
	bool canHurry(int /*HurryTypes*/ eIndex);
	int getSpecialBuildingNotRequiredCount(int /*SpecialBuildingTypes*/ eIndex);
	bool isSpecialBuildingNotRequired(int /*SpecialBuildingTypes*/ eIndex);
	int getBuildingYieldChange(int /*BuildingClassTypes*/ eBuildingClass, int /*YieldTypes*/ eYield);
	int /*CivicTypes*/ getCivic(int /*CivicOptionTypes*/ iIndex);
	void setCivic(int /*CivicOptionTypes*/ eIndex, int /*CivicTypes*/ eNewValue);
	int getCombatExperience() const;
	void changeCombatExperience(int iChange);
	void setCombatExperience(int iExperience);
	// R&R, ray, Great Admirals - START
	int getSeaCombatExperience() const;
	void changeSeaCombatExperience(int iChange);
	void setSeaCombatExperience(int iExperience);
	// R&R, ray, Great Admirals - END
	void addCityName(std::wstring szName);
	int getNumCityNames();
	std::wstring getCityName(int iIndex);
	python::tuple firstCity(bool bRev);	// returns tuple of (CyCity, iterOut)
	python::tuple nextCity(int iterIn, bool bRev);		// returns tuple of (CyCity, iterOut)
	int getNumCities();
	CyCity* getCity(int iID);
	python::tuple firstUnit();	// returns tuple of (CyUnit, iterOut)
	python::tuple nextUnit(int iterIn);		// returns tuple of (CyUnit, iterOut)
	int getNumUnits();
	CyUnit* getUnit(int iID);
	int getNumEuropeUnits();
	CyUnit* getEuropeUnit(int iIndex);
	CyUnit* getEuropeUnitById(int iId);
	void loadUnitFromEurope(CyUnit* pUnit, CyUnit* pTransport);
	void unloadUnitToEurope(CyUnit* pUnit);
	python::tuple firstSelectionGroup(bool bRev);	// returns tuple of (CySelectionGroup, iterOut)
	python::tuple nextSelectionGroup(int iterIn, bool bRev);	// returns tuple of (CySelectionGroup, iterOut)
	int getNumSelectionGroups();
	CySelectionGroup* getSelectionGroup(int iID);
	int countNumTravelUnits(int /*UnitTravelStates*/ eState, int /*DomainTypes*/ eDomain);
	void trigger(/*EventTriggerTypes*/int eEventTrigger);
	const EventTriggeredData* getEventOccured(int /*EventTypes*/ eEvent) const;
	void resetEventOccured(/*EventTypes*/ int eEvent);
	EventTriggeredData* getEventTriggered(int iID) const;
	EventTriggeredData* initTriggeredData(int /*EventTriggerTypes*/ eEventTrigger, bool bFire, int iCityId, int iPlotX, int iPlotY, int /*PlayerTypes*/ eOtherPlayer, int iOtherPlayerCityId, int iUnitId, int /*BuildingTypes*/ eBuilding);
	int getEventTriggerWeight(int /*EventTriggerTypes*/ eTrigger);
	int getHighestTradedYield();
	int getHighestStoredYieldCityId(int /*YieldTypes*/ eYield);
	int getCrossesStored();
	int getBellsStored();
	// R&R, Robert Surcouf, No More Variables Hidden game option START
	int getMissionaryPoints(int /*PlayerTypes*/ ePlayer) const;
	int missionaryThreshold(int /*PlayerTypes*/  ePlayer) const;
	int getMissionaryRateModifier() const;
	// R&R, Robert Surcouf, No More Variables Hidden game option END

	int getTaxRate();
	void changeTaxRate(int iChange);
	bool canTradeWithEurope();
	int getSellToEuropeProfit(int /*YieldTypes*/ eYield, int iAmount);
	int getYieldSellPrice(int /*YieldTypes*/ eYield);
	int getYieldBuyPrice(int /*YieldTypes*/ eYield);
	void setYieldBuyPrice(int /*YieldTypes*/ eYield, int iPrice, bool bMessage);

	// R&R, ray, Africa
	int getYieldAfricaSellPrice(int /*YieldTypes*/ eYield);
	int getYieldAfricaBuyPrice(int /*YieldTypes*/ eYield);
	int getYieldAfricaBuyPriceNoModifier(int /*YieldTypes*/ eYield);
	void setYieldAfricaBuyPrice(int /*YieldTypes*/ eYield, int iPrice, bool bMessage);
	CyUnit* buyYieldUnitFromAfrica(int /*YieldTypes*/ eYield, int iAmount, CyUnit* pTransport);
	// R&R, ray, Africa - END

	// R&R, ray, Port Royal
	int getYieldPortRoyalSellPrice(int /*YieldTypes*/ eYield);
	int getYieldPortRoyalBuyPrice(int /*YieldTypes*/ eYield);
	int getYieldPortRoyalBuyPriceNoModifier(int /*YieldTypes*/ eYield);
	void setYieldPortRoyalBuyPrice(int /*YieldTypes*/ eYield, int iPrice, bool bMessage);
	CyUnit* buyYieldUnitFromPortRoyal(int /*YieldTypes*/ eYield, int iAmount, CyUnit* pTransport);
	// R&R, ray, Port Royal - END

	void sellYieldUnitToEurope(CyUnit* pUnit, int iAmount, int iCommission);
	CyUnit* buyYieldUnitFromEurope(int /*YieldTypes*/ eYield, int iAmount, CyUnit* pTransport);
	int getEuropeUnitBuyPrice(int /*UnitTypes*/ eUnit);
	CyUnit* buyEuropeUnit(int /*UnitTypes*/ eUnit);
	int getYieldBoughtTotal(int /*YieldTypes*/ eYield) const;
	int getYieldBoughtTotalAfrica(int /*YieldTypes*/ eYield) const; // WTP, ray, Yields Traded Total for Africa and Port Royal - START
	int getYieldBoughtTotalPortRoyal(int /*YieldTypes*/ eYield) const; // WTP, ray, Yields Traded Total for Africa and Port Royal - START
	int getNumRevolutionEuropeUnits() const;
	int getRevolutionEuropeUnit(int iIndex) const;
	int getRevolutionEuropeProfession(int iIndex) const;
	int getDocksNextUnit(int iIndex) const;
	bool isEurope() const;
	bool isInRevolution() const;

	void AI_updateFoundValues(bool bStartingLoc);
	int AI_foundValue(int iX, int iY, int iMinUnitRange/* = -1*/, bool bStartingLoc/* = false*/);
	bool AI_demandRebukedWar(int /*PlayerTypes*/ ePlayer);
	AttitudeTypes AI_getAttitude(int /*PlayerTypes*/ ePlayer);
	// R&R, Robert Surcouf, No More Variables Hidden game option START
    int AI_getAttitudeVal(int /*PlayerTypes*/ ePlayer);
    // R&R, Robert Surcouf, No More Variables Hidden game option END
	int AI_unitValue(int /*UnitTypes*/ eUnit, int /*UnitAITypes*/ eUnitAI, CyArea* pArea);
	int AI_civicValue(int /*CivicTypes*/ eCivic);
	int AI_totalUnitAIs(int /*UnitAITypes*/ eUnitAI);
	int AI_totalAreaUnitAIs(CyArea* pArea, int /*UnitAITypes*/ eUnitAI);
	int AI_totalWaterAreaUnitAIs(CyArea* pArea, int /*UnitAITypes*/ eUnitAI);
	int AI_getNumAIUnits(int /*UnitAITypes*/ eIndex);
	int AI_getAttitudeExtra(int /*PlayerTypes*/ eIndex);
	void AI_setAttitudeExtra(int /*PlayerTypes*/ eIndex, int iNewValue);
	void AI_changeAttitudeExtra(int /*PlayerTypes*/ eIndex, int iChange);
	int AI_getMemoryCount(int /*PlayerTypes*/ eIndex1, int /*MemoryTypes*/ eIndex2);
	void AI_changeMemoryCount(int /*PlayerTypes*/ eIndex1, int /*MemoryTypes*/ eIndex2, int iChange);
	int AI_getExtraGoldTarget() const;
	void AI_setExtraGoldTarget(int iNewValue);

	int getScoreHistory(int iTurn) const;
	int getEconomyHistory(int iTurn) const;
	int getIndustryHistory(int iTurn) const;
	int getAgricultureHistory(int iTurn) const;
	int getPowerHistory(int iTurn) const;
	int getCultureHistory(int iTurn) const;

	int addTradeRoute(int iSourceCityOwner, int iSourceCityId, int iDestinationCityOwner, int iDestinationCityId, int /*YieldTypes*/ eYield);
	bool removeTradeRoute(int iId);
	CyTradeRoute* getTradeRoute(int iId) const;
	int getNumTradeRoutes() const;
	CyTradeRoute* getTradeRouteByIndex(int iIndex) const;
	bool editTradeRoute(int iId, int iSourceCityOwner, int iSourceCityId, int iDestinationCityOwner, int iDestinationCityId, int /*YieldTypes*/ eYield);
	bool canLoadYield(int /*PlayerTypes*/ eCityPlayer) const;
	bool canUnloadYield(int /*PlayerTypes*/ eCityPlayer) const;

	int getYieldEquipmentAmount(int /*ProfessionTypes*/ eProfession, int /*YieldTypes*/ eYield);

	void addRevolutionEuropeUnit(int /*UnitTypes*/ eUnit, int /*ProfessionTypes*/ eProfession);

	int getChurchFavourPrice() const; // R&R, ray, Church Favours

	// R&R, ray, Church Favours - START
	int getRandomUsedShipClassTypeID() const;
	int getUsedShipPrice(int iUsedShipClassType) const;
	bool isKingWillingToTradeUsedShips() const;
	void resetCounterForUsedShipDeals();
	// R&R, ray, Church Favours - START

	// WTP, ray, Foreign Kings, buy Immigrants - START
	int getRandomForeignImmigrantClassTypeID(int iKingID) const;
	int getForeignImmigrantPrice(int iForeignImmigrantClassType, int iKingID) const;
	bool isForeignKingWillingToTradeImmigrants(int iKingID) const;
	void resetCounterForForeignImmigrantsDeals();
	// WTP, ray, Foreign Kings, buy Immigrants - END

	int getNumTradeMessages() const;
	std::wstring getTradeMessage(int i) const;
	// TAC - Trade Messages - koma13 - START
	int /*TradeMessageTypes*/ getTradeMessageType(int i) const;
	int /*YieldTypes*/ getTradeMessageYield(int i) const;
	int getTradeMessageAmount(int i) const;
	int getTradeMessageCommission(int i) const;
	// TAC - Trade Messages - koma13 - END
	// TAC - TAC Interface - koma13 - START
	void toggleMultiRowPlotList();
	// TAC - TAC Interface - koma13 - END

	/*** TRIANGLETRADE 10/16/08 by DPII ***/
	int getNumAfricaUnits();
	CyUnit* getAfricaUnit(int iIndex);
	CyUnit* getAfricaUnitById(int iId);
	void loadUnitFromAfrica(CyUnit* pUnit, CyUnit* pTransport);
	void unloadUnitToAfrica(CyUnit* pUnit);

	int getAfricaUnitBuyPrice(int /*UnitTypes*/ eUnit);
	CyUnit* buyAfricaUnit(int /*UnitTypes*/ eUnit);

	bool canTradeWithAfrica();
	int getSellToAfricaProfit(int /*YieldTypes*/ eYield, int iAmount);
	bool isYieldAfricaTradable(int /*YieldTypes*/ eIndex);
	/*************************************/

	//WTP, ray, Colonial Intervention In Native War - START
	int getIDSecondPlayerFrenchNativeWar();
	//WTP, ray, Colonial Intervention In Native War - END

	// R&R, ray, Port Royal
	int getNumPortRoyalUnits();
	CyUnit* getPortRoyalUnit(int iIndex);
	CyUnit* getPortRoyalUnitById(int iId);
	void loadUnitFromPortRoyal(CyUnit* pUnit, CyUnit* pTransport);
	void unloadUnitToPortRoyal(CyUnit* pUnit);

	int getPortRoyalUnitBuyPrice(int /*UnitTypes*/ eUnit);
	CyUnit* buyPortRoyalUnit(int /*UnitTypes*/ eUnit);

	bool canTradeWithPortRoyal();
	int getSellToPortRoyalProfit(int /*YieldTypes*/ eYield, int iAmount);
	bool isYieldPortRoyalTradable(int /*YieldTypes*/ eIndex);
	// R&R, ray, Port Royal - END

	// R&R mod, vetiarvind, trade groups - start
	int getLoadedTradeGroup();
	void setLoadedTradeGroup(int iId);
	int getLastUpdatedTradeGroup();
	int addTradeRouteGroup(const std::wstring groupName);
	bool editTradeRouteGroup(int iId, const std::wstring groupName);
	bool removeTradeRouteGroup(int iId);
	int getNumTradeGroups() const;
	CyTradeRouteGroup* getTradeGroup(int iIndex);	
	// R&R mod, vetiarvind, trade groups - end

	CyInfoArray* getSpecialBuildingTypes() const;
	CyInfoArray* getStoredYieldTypes() const;
	CyInfoArray* getDomesticDemandYieldTypes() const;

	CyInfoArray* getTeachUnitTypes(int iTeachLevel) const;
	int getMaxTeachLevel() const;

	int getCivEffectCount(CivEffectTypes eCivEffect) const;

	unsigned int getNumUnitsOnDock() const;

	std::string getScriptData() const;
	void setScriptData(std::string szNewValue);
	int AI_maxGoldTrade(int iPlayer);
	void forcePeace(int iPlayer);
private:
	CvPlayer* m_pPlayer;
	// R&R mod, vetiarvind, trade groups - start
	int m_lastUpdatedTradegroup; 
	int m_loadedTradeGroup;
	// R&R mod, vetiarvind, trade groups - end
};
#endif	// CyPlayer_h
