#pragma once

// player.h

#ifndef CIV4_PLAYER_H
#define CIV4_PLAYER_H

#include "CvCityAI.h"
#include "CvUnitAI.h"
#include "CvSelectionGroupAI.h"
#include "LinkedList.h"
#include "CvIdVector.h"
#include "CvTalkingHeadMessage.h"
#include "CvTradeRouteGroup.h" //R&R mod, vetiarvind, trade groups
#include "PlayerHelperFunctions.h"

#define	UNIT_BIRTHMARK_TEMP_UNIT	20000

class CvDiploParameters;
class CvPlayerAI;
class CvPlayerCivEffect;
class CvPopupInfo;
class CvEventTriggerInfo;
class CvTradeRoute;

typedef std::list<CvTalkingHeadMessage> CvMessageQueue;
typedef std::list<CvPopupInfo*> CvPopupQueue;
typedef std::list<CvDiploParameters*> CvDiploQueue;
typedef stdext::hash_map<int, int> CvTurnScoreMap;
typedef stdext::hash_map<EventTypes, EventTriggeredData> CvEventMap;
typedef std::vector< std::pair<UnitCombatTypes, PromotionTypes> > UnitCombatPromotionArray;
typedef std::vector< std::pair<UnitClassTypes, PromotionTypes> > UnitClassPromotionArray;
typedef std::vector< std::pair<CivilizationTypes, LeaderHeadTypes> > CivLeaderArray;

class CvPlayer
{
public:
	// add support for AI and CivEffect calls
	__forceinline CvPlayerAI& AI() { return (CvPlayerAI&)*this; }
	__forceinline const CvPlayerAI& AI() const { return (CvPlayerAI&)*this; }

	__forceinline CvPlayerCivEffect& CivEffect() { return (CvPlayerCivEffect&)*this; }
	__forceinline const CvPlayerCivEffect& CivEffect() const { return (CvPlayerCivEffect&)*this; }

	CvPlayer();
	virtual ~CvPlayer();

	// Dale - AoD: AI Autoplay START
	void setDisableHuman( bool newVal );
	bool getDisableHuman( );
protected:
	bool m_bDisableHuman;
	CvIdVector<CvTradeRouteGroup> m_aTradeGroups; //R&R mod, vetiarvind, trade groups
public:
	// Dale - AoD: AI Autoplay END
	// PatchMod: Achievements START
protected:
	std::vector<AchieveTypes> m_achievesGained;
	std::vector<int> m_achievesTurn;
	int m_iNumCombatsWon;
	int m_iNumSeaCombatsWon;
public:
	bool isAchieveGained(AchieveTypes eAchieve) const;
	int getAchieveYear(AchieveTypes eAchieve);
	void gainAchievement(AchieveTypes eAchieve, bool bAnnounce, CvPlot* pPlot, PlayerTypes iPlayer);
	void doAchievements(bool afterMove);
	void changeNumCombatsWon(int iChange);
	void changeNumSeaCombatsWon(int iChange); // R&R, ray, Achievments for Sea Combats
	int getNumCombatsWon();
	int getNumSeaCombatsWon(); // R&R, ray, Achievments for Sea Combats
	// PatchMod: Achievements END

	void doAIImmigrant(int iIndex); // PatchMod: AI immigration boost START

	void redistributeWood(); // R&R, ray, redistribute cannons and muskets

	// TAC - LbD - Ray - START
	void doLbD();
	bool LbD_try_become_expert(CvUnit* convUnit, int base, int increase, int pre_rounds, int l_level);
	bool LbD_try_get_free(CvUnit* convUnit, int base, int increase, int pre_rounds, int mod_crim, int mod_serv, int l_level);
	// TAC - LbD - Ray - END
	//TAC Native Mercs
	void checkForNativeMercs();
	void buyNativeMercs(PlayerTypes sellingPlayer, int price, bool mightbeangry);
	//End TAC Native Mercs

	void checkForNativeSlaves(); // R&R, ray, Native Slave, START

	void checkForAfricanSlaves(); // R&R, ray, African Slaves - START

	void checkForPrisonsCrowded(); // R&R, ray, Prisons Crowded - START

	void checkForRevolutionaryNoble(); // R&R, ray, Revolutionary Noble - START

	void checkForBishop(); // R&R, ray, Bishop - START

	void checkForChurchContact(); // R&R, ray, the Church - START

	void checkForChurchWar(); // R&R, ray, Church War

	void checkForColonialInterventionInNativeWar(); //WTP, ray, Colonial Intervention In Native War - START

	void checkForColonialAndNativeAlliesWar(); // WTP, ray, Big Colonies and Native Allies War - START

	void checkForSmugglers(); // R&R, ray, Smuggling - START

	void checkForRangers(); // R&R, ray, Rangers - START

	void checkForConquistadors(); // R&R, ray, Conquistadors - START

	void checkForPirates(); // R&R, ray, Pirates - START

	void checkForRevolutionSupport(); //TAC Revolution Support

	void checkForEuropeanWars(); //TAC European Wars

	void checkForStealingImmigrant(); // R&R, Stealing Immigrant

	void checkForContinentalGuard(); // R&R, ray, Continental Guard - START

	void checkForMortar(); // R&R, ray, Mortar - START

	void checkForMilitiaOrUnrest(); // R&R, ray, Militia or Unrest - START

	void checkForEuropeanPeace(); // R&R, ray, European Peace - START

	void checkForRoyalIntervention(); // WTP, ray, Royal Intervention, START

	void checkForPrivateersAccusation(); // WTP, ray, Privateers DLL Diplo Event - START
	void withDrawAllPrivateersToPortRoyal(); // WTP, ray, Privateers DLL Diplo Event - START

	// R&R, ray, Church Favours - START
	int getNumChurchFavoursReceived();
	void increaseNumChurchFavoursReceived();
	int getChurchFavourPrice();
	// R&R, ray, Church Favours - END

	//WTP, ray Kings Used Ship - START
	int getUsedShipPrice(UnitClassTypes iUsedShipClassType) const;
	UnitClassTypes getRandomUsedShipClassTypeID() const;
	bool isKingWillingToTradeUsedShips();
	void decreaseCounterForUsedShipDeals();
	void doAILogicforUsedShipDeals();
	void resetCounterForUsedShipDeals();
	void acquireUsedShip(UnitClassTypes iUsedShipClassType, int iPrice);
	//WTP, ray Kings Used Ship - END

	// WTP, ray, Foreign Kings, buy Immigrants - START
	int getForeignImmigrantPrice(UnitClassTypes iForeignImmigrantClassType, int iKingID) const;
	UnitClassTypes getRandomForeignImmigrantClassTypeID(int iKingID) const;
	bool isForeignKingWillingToTradeImmigrants(int iKingID);
	void decreaseCounterForForeignKingImmigrantsDeals();
	void doAILogicforForeignImmigrants();
	void resetCounterForForeignImmigrantsDeals();
	void acquireForeignImmigrant(UnitClassTypes iForeignImmigrantClassType, int iPrice);
	// WTP, ray, Foreign Kings, buy Immigrants - START

	// R&R, ray, Bargaining - Start
	bool tryGetNewBargainPriceSell();
	bool tryGetNewBargainPriceBuy();
	// R&R, ray, Bargaining - End

	void toggleMultiRowPlotList(); // TAC - TAC Interface - koma13 - START

	DllExport void init(PlayerTypes eID);
	DllExport void setupGraphical();
	DllExport void reset(PlayerTypes eID = NO_PLAYER, bool bConstructorCall = false);

protected:

	void uninit();
	bool canGainAchievement() const;

public:

/** NBMOD TAX **/
	int NBMOD_GetMaxTaxRate() const;
    int CvPlayer::NBMOD_GetEuropeMilitaryValue() const;
    int CvPlayer::NBMOD_GetColonialMilitaryValue() const;

	//ray10
	void NBMOD_IncreaseMaxTaxRate(); // Steuergrenze anheben
    void NBMOD_DecreaseMaxTaxRate(); // Steuergrenze senken
	//Ende ray10

/** NBMOD TAX **/

/** NBMOD REF **/

    void CvPlayer::NBMOD_AddEuropeRandomUnit(bool bDisplay);
    void CvPlayer::NBMOD_AddEuropeShipUnit(bool bDisplay);

    int NBMOD_GetNumEuropeUnits() const;
    int NBMOD_GetNumEuropeTransporting() const;
    int NBMOD_GetEuropeShipStrength() const;
    int NBMOD_GetColonialShipStrength() const;
    int NBMOD_REF_GetStartValue() const;

    int NBMOD_REF_MakeStartValue();

/** NBMOD REF **/
// TAC - AI Revolution - koma13 - START
	int getEuropeMilitary() const;
// TAC - AI Revolution - koma13 - END
	void initFreeState();
	void initFreeUnits();
	void initImmigration();
	void verifyImmigration();
	void addFreeUnitAI(UnitAITypes eUnitAI, int iCount);
	CvUnit* addFreeUnit(UnitTypes eUnit, ProfessionTypes eProfession, UnitAITypes eUnitAI = NO_UNITAI);
	int startingPlotRange() const;
	int startingPlotDistanceFactor(CvPlot* pPlot, PlayerTypes ePlayer, int iRange) const;
	int findStartingArea() const;
	CvPlot* findStartingPlot(bool bRandomize = false);
	CvCity* initCity(Coordinates initCoord, bool bBumpUnits);
	void acquireCity(CvCity* pCity, bool bConquest, bool bTrade);
	void killCities();
	const CvWString getNewCityName() const;
	const CvWString& addNewCityName();
	void getCivilizationCityName(CvWString& szBuffer, CivilizationTypes eCivilization) const;
	bool isCityNameValid(const CvWString& szName, bool bTestDestroyed = true) const;
	DllExport CvUnit* initUnit(UnitTypes eUnit, ProfessionTypes eProfession, int iX, int iY, UnitAITypes eUnitAI = NO_UNITAI, DirectionTypes eFacingDirection = NO_DIRECTION, int iYieldStored = 0);
	CvUnit* initUnit(UnitTypes eUnit, ProfessionTypes eProfession, Coordinates initCoord, UnitAITypes eUnitAI = NO_UNITAI, DirectionTypes eFacingDirection = NO_DIRECTION, int iYieldStored = 0, int iBirthmark = -1);
	CvUnit* initEuropeUnit(UnitTypes eUnit, UnitAITypes eUnitAI = NO_UNITAI, DirectionTypes eFacingDirection = NO_DIRECTION);
	bool initEuropeSettler(bool bPayEquipment);
	bool initEuropeTransport(bool bPay);
	CvUnit* initAfricaUnit(UnitTypes eUnit, UnitAITypes eUnitAI = NO_UNITAI, DirectionTypes eFacingDirection = NO_DIRECTION); /*** TRIANGLETRADE 10/23/08 by DPII ***/
	CvUnit* initPortRoyalUnit(UnitTypes eUnit, UnitAITypes eUnitAI = NO_UNITAI, DirectionTypes eFacingDirection = NO_DIRECTION); // R&R, ray, Port Royal
	void killUnits();
	CvSelectionGroup* cycleSelectionGroups(CvUnit* pUnit, bool bForward, bool* pbWrap);
	bool hasTrait(TraitTypes eTrait) const;
	int getTraitCount(TraitTypes eTrait) const;
	void changeTraitCount(TraitTypes eTrait, int iChange);
	int getMercantileFactor() const;
	void changeTotalPlayerAfricaSellProfitModifierInPercent(int iChange); // WTP, Africa and Port Royal Profit Modifiers - START
	void changeTotalPlayerPortRoyalSellProfitModifierInPercent(int iChange); // WTP, Africa and Port Royal Profit Modifiers - START
	void changeTotalPlayerDomesticMarketProfitModifierInPercent(int iChange); // WTP, ray, Domestic Market Profit Modifier -START
	int getTotalPlayerAfricaSellProfitModifierInPercent() const; // WTP, Africa and Port Royal Profit Modifiers - START
	int getTotalPlayerPortRoyalSellProfitModifierInPercent() const; // WTP, Africa and Port Royal Profit Modifiers - START
	int getTotalPlayerDomesticMarketProfitModifierInPercent() const; // WTP, ray, Domestic Market Profit Modifier -START
	DllExport bool isHuman() const;
	DllExport void updateHuman();
	bool isNative() const;
	bool isColonialNation() const;
	CivCategoryTypes getCivCategoryTypes() const;
	bool isAlwaysOpenBorders() const;
	DllExport const wchar* getName(uint uiForm = 0) const;
	DllExport const wchar* getNameKey() const;
	DllExport const wchar* getCivilizationDescription(uint uiForm = 0) const;
	const wchar* getCivilizationDescriptionKey() const;
	DllExport const wchar* getCivilizationShortDescription(uint uiForm = 0) const;
	const wchar* getCivilizationShortDescriptionKey() const;
	DllExport const wchar* getCivilizationAdjective(uint uiForm = 0) const;
	const wchar* getCivilizationAdjectiveKey() const;
	DllExport const char* getFlagDecal() const;
	DllExport bool isWhiteFlag() const;
	DllExport bool isInvertFlag() const;
	DllExport const CvWString getWorstEnemyName() const;
	DllExport ArtStyleTypes getArtStyleType() const;
	char const* getUnitButton(UnitTypes eUnit) const;
	void doTurn();
	void doTurnUnits();
	void doEra();

	void verifyCivics();

	void updateYield();
	void updateCityPlotYield();
	void updateCitySight(bool bIncrement);

	void updateTimers();

	bool hasReadyUnit(bool bAny = false) const;
	bool hasAutoUnit() const;
	DllExport bool hasBusyUnit() const;
	int calculateScore(bool bFinal = false, bool bVictory = false) const;
	int getScoreTaxFactor() const;
	int findBestFoundValue() const;
	int upgradeAllPrice(UnitTypes eUpgradeUnit, UnitTypes eFromUnit);
	int countNumCoastalCities() const;
	int countNumCoastalCitiesByArea(CvArea* pArea) const;
	int countTotalCulture() const;
	int countTotalYieldStored(YieldTypes eYield) const;
	int countCityFeatures(FeatureTypes eFeature) const;
	int countNumBuildings(BuildingTypes eBuilding) const;
	DllExport bool canContact(PlayerTypes ePlayer) const;
	void contact(PlayerTypes ePlayer);
	DllExport void handleDiploEvent(DiploEventTypes eDiploEvent, PlayerTypes ePlayer, int iData1, int iData2);
	bool canTradeWith(PlayerTypes eWhoTo) const;
	bool canReceiveTradeCity(PlayerTypes eFromPlayer) const;
	DllExport bool canTradeItem(PlayerTypes eWhoTo, TradeData item, bool bTestDenial = false) const;
	DllExport DenialTypes getTradeDenial(PlayerTypes eWhoTo, TradeData item) const;
	bool isTradingWithTeam(TeamTypes eTeam, bool bIncludeCancelable) const;
	bool canStopTradingWithTeam(TeamTypes eTeam, bool bContinueNotTrading = false) const;
	void stopTradingWithTeam(TeamTypes eTeam);
	void killAllDeals();
	void findNewCapital();
	bool canRaze(CvCity* pCity) const;
	void raze(CvCity* pCity);
	void disband(CvCity* pCity, bool bAbandon = false);
	bool canReceiveGoody(CvPlot* pPlot, GoodyTypes eGoody, const CvUnit* pUnit) const;
	int receiveGoody(CvPlot* pPlot, GoodyTypes eGoody, CvUnit* pUnit);
	void receiveRandomGoody(CvPlot* pPlot, GoodyTypes eGoody, CvUnit* pUnit);
	void doGoody(CvPlot* pPlot, CvUnit* pUnit);
	bool canFound(Coordinates foundCoord, bool bTestVisible = false) const;
	CvCity *found(Coordinates foundCoord);
	bool canTrain(UnitTypes eUnit, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false) const;
	bool canConstruct(BuildingTypes eBuilding, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false) const;
	int getYieldProductionNeeded(UnitTypes eUnit, YieldTypes eYield) const;
	int getYieldProductionNeeded(BuildingTypes eBuilding, YieldTypes eYield) const;
	int getProductionModifier(UnitTypes eUnit) const;
	int getProductionModifier(BuildingTypes eBuilding) const;
	int getBuildingClassPrereqBuilding(BuildingTypes eBuilding, BuildingClassTypes ePrereqBuildingClass, int iExtra = 0) const;
	void removeBuildingClass(BuildingClassTypes eBuildingClass);
	void processTrait(TraitTypes eTrait, int iChange);
	void processFather(FatherTypes eFather, int iChange);
	void processFatherOnce(FatherTypes eFather);
	int getFatherPointMultiplier() const;
	void setFatherPointMultiplier(int iValue);
	void changeFatherPoints(FatherPointTypes ePointType, int iChange);
	int getBuildCost(const CvPlot* pPlot, BuildTypes eBuild) const;
	bool canBuild(const CvPlot* pPlot, BuildTypes eBuild, bool bTestEra = false, bool bTestVisible = false) const;
	RouteTypes getBestRoute(CvPlot* pPlot = NULL) const;
	int getImprovementUpgradeRate() const;
	int calculateTotalYield(YieldTypes eYield) const;
	void calculateTotalYields(int aiYields[]) const;
	bool isCivic(CivicTypes eCivic) const;
	bool canDoCivics(CivicTypes eCivic) const;
	int greatGeneralThreshold() const;
	int greatAdmiralThreshold() const; // R&R, ray, Great Admirals
	int immigrationThreshold() const;
	int getImmigrationThresholdModifierFromUnitsWaitingOnDock() const; // WTP, ray, increase threshold if more than X units waiting on the docks - START
	int revolutionEuropeUnitThreshold() const;
	CvPlot* getStartingPlot() const;
	void setStartingPlot(CvPlot* pNewValue, bool bUpdateStartDist);
	int getTotalPopulation() const;
	int getAveragePopulation() const;
	void changeTotalPopulation(int iChange);
	long getRealPopulation() const;
	int getTotalLand() const;
	void changeTotalLand(int iChange);
	int getTotalLandScored() const;
	void changeTotalLandScored(int iChange);
	DllExport int getGold() const;
	DllExport void setGold(int iNewValue);
	DllExport void changeGold(int iChange);
	int getExtraTradeMultiplier(PlayerTypes eOtherPlayer) const;
	DllExport int getAdvancedStartPoints() const;
	void setAdvancedStartPoints(int iNewValue);
	void changeAdvancedStartPoints(int iChange);
	DllExport void doAdvancedStartAction(AdvancedStartActionTypes eAction, int iX, int iY, int iData, bool bAdd);
	void doAdvancedStartAction(AdvancedStartActionTypes eAction, Coordinates coord, int iData, bool bAdd);
	DllExport int getAdvancedStartUnitCost(UnitTypes eUnit, bool bAdd, CvPlot* pPlot = NULL);
	DllExport int getAdvancedStartCityCost(bool bAdd, CvPlot* pPlot = NULL);
	DllExport int getAdvancedStartPopCost(bool bAdd, CvCity* pCity = NULL);
	DllExport int getAdvancedStartCultureCost(bool bAdd, CvCity* pCity = NULL);
	DllExport int getAdvancedStartBuildingCost(BuildingTypes eBuilding, bool bAdd, CvCity* pCity = NULL);
	DllExport int getAdvancedStartImprovementCost(ImprovementTypes eImprovement, bool bAdd, CvPlot* pPlot = NULL);
	DllExport int getAdvancedStartRouteCost(RouteTypes eRoute, bool bAdd, CvPlot* pPlot = NULL);
	DllExport int getAdvancedStartVisibilityCost(bool bAdd, CvPlot* pPlot = NULL);

	void createGreatGeneral(UnitTypes eGreatGeneralUnit, bool bIncrementExperience, const Coordinates coord);
	int getGreatGeneralsCreated() const;
	void incrementGreatGeneralsCreated();
	int getGreatGeneralsThresholdModifier() const;
	void changeGreatGeneralsThresholdModifier(int iChange);
	// R&R, ray, Great Admirals - START
	void createGreatAdmiral(UnitTypes eGreatAdmiralUnit, bool bIncrementExperience, const Coordinates coord);
	int getGreatAdmiralsCreated() const;
	void incrementGreatAdmiralsCreated();
	int getGreatAdmiralsThresholdModifier() const;
	void changeGreatAdmiralsThresholdModifier(int iChange);
	// R&R, ray, Great Admirals - END
	// WTP, ray, Lieutenants and Captains - START
	void createBraveLieutenant(UnitTypes eBraveLieutenantUnit, const Coordinates coord);
	void createCapableCaptain(UnitTypes eCapableCaptainUnit, const Coordinates coord);
	// WTP, ray, Lieutenants and Captains - END
	int getGreatGeneralRateModifier() const;
	void changeGreatGeneralRateModifier(int iChange);
	int getDomesticGreatGeneralRateModifier() const;
	void changeDomesticGreatGeneralRateModifier(int iChange);
	int getImmigrationThresholdMultiplier() const;
	void setImmigrationThresholdMultiplier(int iChange);
	int getRevolutionEuropeUnitThresholdMultiplier() const;
	void setRevolutionEuropeUnitThresholdMultiplier(int iChange);
	int getKingNumUnitMultiplier() const;
	void setKingNumUnitMultiplier(int iChange);
	int getNativeAngerModifier() const;
	void changeNativeAngerModifier(int iChange);
	int getFreeExperience() const;
	void changeFreeExperience(int iChange);
	int getWorkerSpeedModifier() const;
	void changeWorkerSpeedModifier(int iChange);
	int getImprovementUpgradeRateModifier() const;
	void changeImprovementUpgradeRateModifier(int iChange);
	int getImprovementUpgradeDurationModifier() const; // WTP, ray, Improvement Growth Modifier - START
	void changeImprovementUpgradeDurationModifier(int iChange); // WTP, ray, Improvement Growth Modifier - START
	int getMilitaryProductionModifier() const;
	void changeMilitaryProductionModifier(int iChange);
	int getCityDefenseModifier() const;
	void changeCityDefenseModifier(int iChange);
	int getHighestUnitLevel() const;
	void setHighestUnitLevel(int iNewValue);
	int getFatherOverflowBells() const;
	void setFatherOverflowBells(int iNewValue);
	void changeFatherOverflowBells(int iChange);
	int getExpInBorderModifier() const;
	void changeExpInBorderModifier(int iChange);
	int getLevelExperienceModifier() const;
	void changeLevelExperienceModifier(int iChange);
	CvCity* getCapitalCity() const;
	void setCapitalCity(CvCity* pNewCapitalCity);
	DllExport CvCity* getPrimaryCity() const;
	int getCitiesLost() const;
	void changeCitiesLost(int iChange);
	int getAssets() const;
	void changeAssets(int iChange);
	int getPower() const;
	void changePower(int iChange);
	int getPopScore() const;
	void changePopScore(int iChange);
	int getLandScore() const;
	void changeLandScore(int iChange);
	int getFatherScore() const;
	void changeFatherScore(int iChange);
	int getCombatExperience() const;
	void setCombatExperience(int iExperience);
	void changeCombatExperience(int iChange);
	// R&R, ray, Great Admirals - START
	int getSeaCombatExperience() const;
	void setSeaCombatExperience(int iExperience);
	void changeSeaCombatExperience(int iChange);
	// R&R, ray, Great Admirals - END
	bool isConnected() const;
	DllExport int getNetID() const;
	DllExport void setNetID(int iNetID);
	void sendReminder();

	uint getStartTime() const;
	DllExport void setStartTime(uint uiStartTime);
	DllExport uint getTotalTimePlayed() const;
	DllExport bool isAlive() const;

	DllExport bool isEverAlive() const;
	void setAlive(bool bNewValue);

	// R&R, ray, Bargaining - Start
	bool isWillingToBargain() const;
	void setWillingToBargain(bool bNewValue);
	int getTimeNoTrade() const;
	void setTimeNoTrade(int bNewValue);
	void decreaseTimeNoTrade();
	// R&R, ray, Bargaining - End

	void verifyAlive();
	DllExport bool isTurnActive() const;
	DllExport void setTurnActive(bool bNewValue, bool bDoTurn = true);

	bool isAutoMoves() const;
	void setAutoMoves(bool bNewValue);
	DllExport void setTurnActiveForPbem(bool bActive);

	DllExport bool isPbemNewTurn() const;
	DllExport void setPbemNewTurn(bool bNew);

	bool isEndTurn() const;
	DllExport void setEndTurn(bool bNewValue);

	DllExport bool isTurnDone() const;
	bool isExtendedGame() const;
	DllExport void makeExtendedGame();
	bool isFoundedFirstCity() const;
	void setFoundedFirstCity(bool bNewValue);
	DllExport PlayerTypes getID() const;
	DllExport HandicapTypes getHandicapType() const;
	DllExport CivilizationTypes getCivilizationType() const;
	DllExport LeaderHeadTypes getLeaderType() const;
	LeaderHeadTypes getPersonalityType() const;
	void setPersonalityType(LeaderHeadTypes eNewValue);
	DllExport EraTypes getCurrentEra() const;
	void setCurrentEra(EraTypes eNewValue);

	PlayerTypes getParent() const;
	void setParent(PlayerTypes eParent);
	PlayerTypes getColony() const;
	void setColony(PlayerTypes eColony);

	CvPlayerAI* getColonyPlayer() const;
	CvPlayerAI* getParentPlayer() const;

	DllExport TeamTypes getTeam() const;
	void setTeam(TeamTypes eTeam);
	void updateTeamType();
	YieldTypes getImmigrationConversion() const;
	void setImmigrationConversion(YieldTypes eConversion);
	DllExport PlayerColorTypes getPlayerColor() const;
	DllExport int getPlayerTextColorR() const;
	DllExport int getPlayerTextColorG() const;
	DllExport int getPlayerTextColorB() const;
	int getPlayerTextColorA() const;
	int getSeaPlotYield(YieldTypes eIndex) const;
	void changeSeaPlotYield(YieldTypes eIndex, int iChange);
	int getYieldRateModifier(YieldTypes eIndex) const;
	int getTaxYieldRateModifier(YieldTypes eIndex) const;
	void changeYieldRateModifier(YieldTypes eIndex, int iChange);
	int getCapitalYieldRateModifier(YieldTypes eIndex) const;
	void changeCapitalYieldRateModifier(YieldTypes eIndex, int iChange);
	int getBuildingRequiredYieldModifier(YieldTypes eIndex) const;
	void changeBuildingRequiredYieldModifier(YieldTypes eIndex, int iChange);
	int getCityExtraYield(YieldTypes eIndex) const;
	void updateCityExtraYield(YieldTypes eIndex);
	int getExtraYieldThreshold(YieldTypes eIndex) const;
	void updateExtraYieldThreshold(YieldTypes eIndex);
	int getYieldRate(YieldTypes eIndex) const;
	int getHappinessRate() const; // WTP, ray, Happiness - START
	int getUnHappinessRate() const; // WTP, ray, Happiness - START
	int getLawRate() const; // WTP, ray, Crime and Law - START
	int getCrimeRate() const; // WTP, ray, Crime and Law - START
	bool isYieldEuropeTradable(YieldTypes eIndex) const;
	void setYieldEuropeTradable(YieldTypes eIndex, bool bTradeable);
	void setYieldEuropeTradableAll();
	bool isFeatAccomplished(FeatTypes eIndex) const;
	void setFeatAccomplished(FeatTypes eIndex, bool bNewValue);
	bool shouldDisplayFeatPopup(FeatTypes eIndex) const;
	DllExport bool isOption(PlayerOptionTypes eIndex) const;
	void setOption(PlayerOptionTypes eIndex, bool bNewValue);
	bool isPlayable() const;
	void setPlayable(bool bNewValue);
	int getImprovementCount(ImprovementTypes eIndex) const;
	void changeImprovementCount(ImprovementTypes eIndex, int iChange);
	int getFreeBuildingCount(BuildingTypes eIndex) const;
	bool isBuildingFree(BuildingTypes eIndex) const;
	void changeFreeBuildingCount(BuildingTypes eIndex, int iChange);
	int getUnitClassCount(UnitClassTypes eIndex) const;
	void changeUnitClassCount(UnitClassTypes eIndex, int iChange);
	int getUnitClassMaking(UnitClassTypes eIndex) const;
	void changeUnitClassMaking(UnitClassTypes eIndex, int iChange);
	int getUnitClassImmigrated(UnitClassTypes eIndex) const;
	void changeUnitClassImmigrated(UnitClassTypes eIndex, int iChange);
	int getUnitClassCountPlusMaking(UnitClassTypes eIndex) const;
	int getUnitMoveChange(UnitClassTypes eIndex) const;
	DllExport void changeUnitMoveChange(UnitClassTypes eIndex, int iChange);
	int getUnitStrengthModifier(UnitClassTypes eIndex) const;
	void changeUnitStrengthModifier(UnitClassTypes eIndex, int iChange);
	int getProfessionCombatChange(ProfessionTypes eIndex) const;
	void changeProfessionCombatChange(ProfessionTypes eIndex, int iChange);
	int getProfessionMoveChange(ProfessionTypes eIndex) const;
	void changeProfessionMoveChange(ProfessionTypes eIndex, int iChange);
	int getBuildingClassCount(BuildingClassTypes eIndex) const;
	void changeBuildingClassCount(BuildingClassTypes eIndex, int iChange);
	int getBuildingClassMaking(BuildingClassTypes eIndex) const;
	void changeBuildingClassMaking(BuildingClassTypes eIndex, int iChange);
	int getBuildingClassCountPlusMaking(BuildingClassTypes eIndex) const;
	int getSpecialBuildingCount(int iIndex, bool bPlusMaking = false) const;	// TAC - AI Buildings - koma13
	int getHurryCount(HurryTypes eIndex) const;
	bool canPopRush();
	void changeHurryCount(HurryTypes eIndex, int iChange);
	int getSpecialBuildingNotRequiredCount(SpecialBuildingTypes eIndex) const;
	bool isSpecialBuildingNotRequired(SpecialBuildingTypes eIndex) const;
	void changeSpecialBuildingNotRequiredCount(SpecialBuildingTypes eIndex, int iChange);
	CivicTypes getCivic(CivicOptionTypes eIndex) const;
	DllExport void setCivic(CivicOptionTypes eIndex, CivicTypes eNewValue);
	int getImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const;
	void changeImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2, int iChange);
	int getBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield) const;
	void changeBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield, int iChange);
	int getTaxYieldModifierCount(YieldTypes eYield) const;
	void changeTaxYieldModifierCount(YieldTypes eYield, int iChange);

	void updateGroupCycle(CvUnit* pUnit);
	void removeGroupCycle(int iID);

	CLLNode<int>* deleteGroupCycleNode(CLLNode<int>* pNode);
	CLLNode<int>* nextGroupCycleNode(CLLNode<int>* pNode) const;
	CLLNode<int>* previousGroupCycleNode(CLLNode<int>* pNode) const;
	CLLNode<int>* headGroupCycleNode() const;
	CLLNode<int>* tailGroupCycleNode() const;
	void addCityName(const CvWString& szName);
	int getNumCityNames() const;
	const CvWString& getCityName(int iIndex) const;

	// city iteration
	DllExport CvCity* firstCity(int *pIterIdx, bool bRev=false) const;
	CvCity* firstCity() const;
	DllExport CvCity* nextCity(int *pIterIdx, bool bRev=false) const;
	DllExport int getNumCities() const;
	DllExport CvCity* getCity(int iID) const;
	CvCity* addCity();
	void deleteCity(int iID);

	// unit iteration
	DllExport CvUnit* firstUnit(int *pIterIdx) const;
	CvUnit* firstUnitInternal(int* pIterIdx) const;
	DllExport CvUnit* nextUnit(int *pIterIdx) const;
	CvUnit* nextUnitInternal(int* pIterIdx) const;
	DllExport int getNumUnits() const;
	int getNumShips() const;// WTP, ray, easily counting Ships - START
	DllExport CvUnit* getUnit(int iID) const;
	CvUnit* addUnit();
	void addExistingUnit(CvUnit *pUnit);
	void deleteUnit(int iID);
	CvUnit* getAndRemoveUnit(int iId);

	int getNumEuropeUnits() const;
	CvUnit* getEuropeUnit(int iIndex) const;
	DllExport CvUnit* getEuropeUnitById(int iId) const;
	void loadUnitFromEurope(CvUnit* pUnit, CvUnit* pTransport);
	void unloadUnitToEurope(CvUnit* pUnit);
	void transferUnitInEurope(CvUnit* pUnit, CvUnit* pTransport);
	/*** TRIANGLETRADE 10/23/08 by DPII ***/
	int getNumAfricaUnits() const;
	CvUnit* getAfricaUnit(int iIndex) const;
	CvUnit* getAfricaUnitById(int iId) const;
	void loadUnitFromAfrica(CvUnit* pUnit, CvUnit* pTransport);
	void unloadUnitToAfrica(CvUnit* pUnit);
	void transferUnitInAfrica(CvUnit* pUnit, CvUnit* pTransport);
	// R&R, ray, Port Royal
	int getNumPortRoyalUnits() const;
	CvUnit* getPortRoyalUnit(int iIndex) const;
	CvUnit* getPortRoyalUnitById(int iId) const;
	void loadUnitFromPortRoyal(CvUnit* pUnit, CvUnit* pTransport);
	void unloadUnitToPortRoyal(CvUnit* pUnit);
	void transferUnitInPortRoyal(CvUnit* pUnit, CvUnit* pTransport);

	void doREFReduction(int iGold);

	int countNumTravelUnits(UnitTravelStates eState, DomainTypes eDomain) const;
	int countNumDomainUnits(DomainTypes eDomain) const;

	// selection groups iteration
	CvSelectionGroup* firstSelectionGroup(int *pIterIdx, bool bRev=false) const;
	CvSelectionGroup* nextSelectionGroup(int *pIterIdx, bool bRev=false) const;
	int getNumSelectionGroups() const;
	CvSelectionGroup* getSelectionGroup(int iID) const;
	CvSelectionGroup* addSelectionGroup();
	void deleteSelectionGroup(int iID);
	// pending triggers iteration
	EventTriggeredData* firstEventTriggered(int *pIterIdx, bool bRev=false) const;
	EventTriggeredData* nextEventTriggered(int *pIterIdx, bool bRev=false) const;
	int getNumEventsTriggered() const;
	EventTriggeredData* getEventTriggered(int iID) const;
	EventTriggeredData* addEventTriggered();
	void deleteEventTriggered(int iID);
	EventTriggeredData* initTriggeredData(EventTriggerTypes eEventTrigger, bool bFire = false, int iCityId = -1, int iPlotX = INVALID_PLOT_COORD, int iPlotY = INVALID_PLOT_COORD, PlayerTypes eOtherPlayer = NO_PLAYER, int iOtherPlayerCityId = -1, int iUnitId = -1, BuildingTypes eBuilding = NO_BUILDING);
	int getEventTriggerWeight(EventTriggerTypes eTrigger) const;
	DllExport void addMessage(const CvTalkingHeadMessage& message);
	void showMissedMessages();
	void clearMessages();
	DllExport const CvMessageQueue& getGameMessages() const;
	void expireMessages();
	DllExport void addPopup(CvPopupInfo* pInfo, bool bFront = false);
	void clearPopups();
	DllExport CvPopupInfo* popFrontPopup();
	DllExport const CvPopupQueue& getPopups() const;
	DllExport void addDiplomacy(CvDiploParameters* pDiplo);
	void clearDiplomacy();
	DllExport const CvDiploQueue& getDiplomacy() const;
	DllExport CvDiploParameters* popFrontDiplomacy();

	int getScoreHistory(int iTurn) const;
	void updateScoreHistory(int iTurn, int iBestScore);

	int getEconomyHistory(int iTurn) const;
	void updateEconomyHistory(int iTurn, int iBestEconomy);
	int getIndustryHistory(int iTurn) const;
	void updateIndustryHistory(int iTurn, int iBestIndustry);
	int getAgricultureHistory(int iTurn) const;
	void updateAgricultureHistory(int iTurn, int iBestAgriculture);
	int getPowerHistory(int iTurn) const;
	void updatePowerHistory(int iTurn, int iBestPower);
	int getCultureHistory(int iTurn) const;
	void updateCultureHistory(int iTurn, int iBestCulture);

	int addTradeRoute(const IDInfo& kSource, const IDInfo& kDestination, YieldTypes eYield);
	bool editTradeRoute(int iId, const IDInfo& kSource, const IDInfo& kDestination, YieldTypes eYield);
	bool removeTradeRoute(int iId, bool bRemoveImport = false); // auto traderoute - Nightinggale
	CvTradeRoute* getTradeRoute(int iId) const;
	int getNumTradeRoutes() const;
	CvTradeRoute* getTradeRouteByIndex(int iIndex) const;
	void getTradeRoutes(std::vector<CvTradeRoute*>& aTradeRoutes) const;
	void validateTradeRoutes();
	bool canLoadYield(PlayerTypes eCityPlayer) const;
	bool canUnloadYield(PlayerTypes eCityPlayer) const;

	// Script data needs to be a narrow string for pickling in Python
	std::string getScriptData() const;
	void setScriptData(std::string szNewValue);
	DllExport const CvString& getPbemEmailAddress() const;
	DllExport void setPbemEmailAddress(const char* szAddress);
	DllExport const CvString& getSmtpHost() const;
	void setSmtpHost(const char* szHost);
	const EventTriggeredData* getEventOccured(EventTypes eEvent) const;
	bool isTriggerFired(EventTriggerTypes eEventTrigger) const;
	void setEventOccured(EventTypes eEvent, const EventTriggeredData& kEventTriggered, bool bOthers = true);
	void resetEventOccured(EventTypes eEvent, bool bAnnounce = true);
	void setTriggerFired(const EventTriggeredData& kTriggeredData, bool bOthers = true, bool bAnnounce = true);
	void resetTriggerFired(EventTriggerTypes eEventTrigger);
	void trigger(EventTriggerTypes eEventTrigger);
	void trigger(const EventTriggeredData& kData);
	void applyEvent(EventTypes eEvent, int iTriggeredId, bool bUpdateTrigger = true);
	bool canDoEvent(EventTypes eEvent, const EventTriggeredData& kTriggeredData) const;
	int getEventCost(EventTypes eEvent, PlayerTypes eOtherPlayer, bool bRandom) const;
	bool canTrigger(EventTriggerTypes eTrigger, PlayerTypes ePlayer) const;
	const EventTriggeredData* getEventCountdown(EventTypes eEvent) const;
	void setEventCountdown(EventTypes eEvent, const EventTriggeredData& kEventTriggered);
	void resetEventCountdown(EventTypes eEvent);

	bool isFreePromotion(UnitCombatTypes eUnitCombat, PromotionTypes ePromotion) const;
	void setFreePromotion(UnitCombatTypes eUnitCombat, PromotionTypes ePromotion, bool bFree);
	bool isFreePromotion(UnitClassTypes eUnitCombat, PromotionTypes ePromotion) const;
	void setFreePromotion(UnitClassTypes eUnitCombat, PromotionTypes ePromotion, bool bFree);

	void invalidatePopulationRankCache();
	void invalidateYieldRankCache(YieldTypes eYield = NO_YIELD);

	PlayerTypes pickConqueredCityOwner(const CvCity& kCity) const;
	void forcePeace(PlayerTypes ePlayer);
	bool canSpiesEnterBorders(PlayerTypes ePlayer) const;
	int getNewCityProductionValue() const;

	int getGrowthThreshold(int iPopulation) const;

	void verifyUnitStacksValid();

	DllExport void buildTradeTable(PlayerTypes eOtherPlayer, CLinkList<TradeData>& ourList, const IDInfo& kTransport) const;
	DllExport bool getHeadingTradeString(PlayerTypes eOtherPlayer, TradeableItems eItem, CvWString& szString, CvString& szIcon) const;
	DllExport bool getItemTradeString(PlayerTypes eOtherPlayer, bool bOffer, bool bShowingCurrent, const TradeData& zTradeData, const IDInfo& kTransport, CvWString& szString, CvString& szIcon) const;
	DllExport void updateTradeList(PlayerTypes eOtherPlayer, CLinkList<TradeData>& ourInventory, const CLinkList<TradeData>& ourOffer, const CLinkList<TradeData>& theirOffer, const IDInfo& kTransport) const;
	DllExport int getMaxGoldTrade(PlayerTypes eOtherPlayer, const IDInfo& kTransport) const;

	CvCity* getPopulationUnitCity(int iUnitId) const;
	int getCrossesStored() const;
	void changeCrossesStored(int iChange);
	int getBellsStored() const;
	void changeBellsStored(int iChange);
	int getTaxRate() const;
	void setTaxRate(int iValue);
	void changeTaxRate(int iChange);
	int getNativeCombatModifier() const;
	void setNativeCombatModifier(int iValue);
	void changeNativeCombatModifier(int iChange);
	int getDominateNativeBordersCount() const;
	void changeDominateNativeBordersCount(int iChange);
	int getRevolutionEuropeTradeCount() const;
	void changeRevolutionEuropeTradeCount(int iChange);
	bool canTradeWithEurope() const;
	void interceptEuropeUnits();

    int AI_getAttitudeValue(PlayerTypes ePlayer); // R&R, Robert Surcouf, No More Variables Hidden game option START
	void createEnemyPirates(); // R&R, ray, Pirates - START
	int getSellToEuropeProfit(YieldTypes eYield, int iAmount) const;
	int getYieldSellPrice(YieldTypes eYield) const;
	int getYieldBuyPrice(YieldTypes eYield) const;
	void setYieldBuyPrice(YieldTypes eYield, int iPrice, bool bMessage);
	/*** TRIANGLETRADE 10/24/08 by DPII ***/
	void sellYieldUnitToAfrica(CvUnit* pUnit, int iAmount, int iCommission);
	int getAfricaUnitBuyPrice(UnitTypes eUnit) const;
	CvUnit* buyAfricaUnit(UnitTypes eUnit, int iPriceModifier);
	/**************************************/
	// R&R, ray, Africa
	int getYieldAfricaSellPrice(YieldTypes eYield) const;
	int getYieldAfricaBuyPrice(YieldTypes eYield) const;
	int getYieldAfricaBuyPriceNoModifier(YieldTypes eYield) const;
	void setYieldAfricaBuyPrice(YieldTypes eYield, int iPrice, bool bMessage);
	CvUnit* buyYieldUnitFromAfrica(YieldTypes eYield, int iAmount, CvUnit* pTransport);
	// R&R, ray, Africa - END
	// R&R, ray, Port Royal
	void sellYieldUnitToPortRoyal(CvUnit* pUnit, int iAmount, int iCommission);
	int getPortRoyalUnitBuyPrice(UnitTypes eUnit) const;
	CvUnit* buyPortRoyalUnit(UnitTypes eUnit, int iPriceModifier);
	int getYieldPortRoyalSellPrice(YieldTypes eYield) const;
	int getYieldPortRoyalBuyPrice(YieldTypes eYield) const;
	int getYieldPortRoyalBuyPriceNoModifier(YieldTypes eYield) const;
	void setYieldPortRoyalBuyPrice(YieldTypes eYield, int iPrice, bool bMessage);
	CvUnit* buyYieldUnitFromPortRoyal(YieldTypes eYield, int iAmount, CvUnit* pTransport);
	// R&R, ray, Port Royal - END
	void sellYieldUnitToEurope(CvUnit* pUnit, int iAmount, int iCommission);
	CvUnit* buyYieldUnitFromEurope(YieldTypes eYield, int iAmount, CvUnit* pTransport);
	// TAC - AI purchases military units - koma13 - START
	//int getEuropeUnitBuyPrice(UnitTypes eUnit) const;
	int getEuropeUnitBuyPrice(UnitTypes eUnit, bool bIncrease = true) const;
	// TAC - AI purchases military units - koma13 - END
	CvUnit* buyEuropeUnit(UnitTypes eUnit, int iPriceModifier);
	void buyUnitsFromKing();
	int getYieldTradedTotal(YieldTypes eYield) const;
	void setYieldTradedTotal(YieldTypes eYield, int iValue);
	// WTP, ray, Yields Traded Total for Africa and Port Royal - START
	int getYieldTradedTotalAfrica(YieldTypes eYield) const;
	void setYieldTradedTotalAfrica(YieldTypes eYield, int iValue);
	int getYieldTradedTotalPortRoyal(YieldTypes eYield) const;
	void setYieldTradedTotalPortRoyal(YieldTypes eYield, int iValue);
	// WTP, ray, Yields Traded Total for Africa and Port Royal - END
	// R&R, vetiarvind, Price dependent tax rate change - Start
	int CvPlayer::getYieldScoreTotal(YieldTypes eYield) const;
	void CvPlayer::setYieldScoreTotal(YieldTypes eYield, int iValue);
	const int getFullYieldScore();
	const int getTaxRaiseChance();
	const int getTaxThresold();

	void CvPlayer::changeYieldTradedTotal(YieldTypes eYield, int iChange, int iUnitPrice = -1);
	void CvPlayer::changeYieldTradedTotalAfrica(YieldTypes eYield, int iChange, int iUnitPrice = -1); // WTP, ray, Yields Traded Total for Africa and Port Royal - START
	void CvPlayer::changeYieldTradedTotalPortRoyal(YieldTypes eYield, int iChange, int iUnitPrice = -1); // WTP, ray, Yields Traded Total for Africa and Port Royal - START
	//void changeYieldTradedTotal(YieldTypes eYield, int iChange);
	// R&R, vetiarvind, Price dependent tax rate change - End

	int getYieldBoughtTotal(YieldTypes eYield) const;
	void setYieldBoughtTotal(YieldTypes eYield, int iValue);
	void changeYieldBoughtTotal(YieldTypes eYield, int iChange);

	// WTP, ray, Yields Traded Total for Africa and Port Royal - START
	int getYieldBoughtTotalAfrica(YieldTypes eYield) const;
	void setYieldBoughtTotalAfrica(YieldTypes eYield, int iValue);
	void changeYieldBoughtTotalAfrica(YieldTypes eYield, int iChange);
	int getYieldBoughtTotalPortRoyal(YieldTypes eYield) const;
	void setYieldBoughtTotalPortRoyal(YieldTypes eYield, int iValue);
	void changeYieldBoughtTotalPortRoyal(YieldTypes eYield, int iChange);
	// WTP, ray, Yields Traded Total for Africa and Port Royal - END

	YieldTypes getHighestTradedYield() const;
	int getHighestStoredYieldCityId(YieldTypes eYield) const;

	DllExport void doAction(PlayerActionTypes eAction, int iData1, int iData2, int iData3);
	int getTradeYieldAmount(YieldTypes eYield, CvUnit* pTransport) const;
	void setCityBillboardDirty(bool bNewValue);
	bool isEurope() const;
	bool isInRevolution() const;
	bool checkIndependence() const;

	void applyMissionaryPoints(CvCity* pCity);
	int getMissionaryPoints(PlayerTypes ePlayer) const;
	void changeMissionaryPoints(PlayerTypes ePlayer, int iChange);
	int getMissionaryThresholdMultiplier(PlayerTypes ePlayer) const;
	void setMissionaryThresholdMultiplier(PlayerTypes ePlayer, int iValue);
	int missionaryThreshold(PlayerTypes ePlayer) const;
	void burnMissions(PlayerTypes ePlayer);
	bool canHaveMission(PlayerTypes ePlayer) const;
	void validateMissions();
	int getMissionaryRateModifier() const;
	void changeMissionaryRateModifier(int iChange);
	int getNativeTradeModifier() const; // R&R, ray, new Attribute in Traits
	void changeNativeTradeModifier(int iChange); // R&R, ray, new Attribute in Traits
	int getMissionarySuccessPercent() const;
	void setMissionarySuccessPercent(int iValue);

	// WTP, ray, Native Trade Posts - START
	void burnTradePosts(PlayerTypes ePlayer);
	bool canHaveTradePost(PlayerTypes ePlayer) const;
	void validateTradePosts();
	int getNativeTradePostSuccessPercent() const;
	void setNativeTradePostSuccessPercent(int iValue);
	// WTP, ray, Native Trade Posts - END

	int getRebelCombatPercent() const;

	int getProfessionEquipmentModifier(ProfessionTypes eProfession) const;
	void setProfessionEquipmentModifier(ProfessionTypes eProfession, int iValue);
	int getYieldEquipmentAmount(ProfessionTypes eProfession, YieldTypes eYield) const;
	// cache CvPlayer::getYieldEquipmentAmount - start - Nightinggale
	bool hasContentsYieldEquipmentAmount(ProfessionTypes eProfession) const;
	int getYieldEquipmentAmountSecure(ProfessionTypes eProfession, YieldTypes eYield) const;
	bool hasContentsYieldEquipmentAmountSecure(ProfessionTypes eProfession) const;
	// cache CvPlayer::getYieldEquipmentAmount - end - Nightinggale
	bool isProfessionValid(ProfessionTypes eProfession, UnitTypes eUnit) const;
	void taxIncreaseDiploCall(int iOldTaxRate, int iNewTaxRate, int iChange);
	void changeProfessionEurope(int iUnitId, ProfessionTypes eProfession);

	int getNumRevolutionEuropeUnits() const;
	UnitTypes getRevolutionEuropeUnit(int i) const;
	ProfessionTypes getRevolutionEuropeProfession(int i) const;
	void addRevolutionEuropeUnit(UnitTypes eUnit, ProfessionTypes eProfession);
	void clearRevolutionEuropeUnits();

	UnitTypes getDocksNextUnit(int i) const;
	UnitTypes pickBestImmigrant();
	bool canHurry(HurryTypes eHurry, int iIndex) const;
	void hurry(HurryTypes eHurry, int iIndex);
	int getHurryGold(HurryTypes eHurry, int iIndex) const;
	const wchar* getHurryItemTextKey(HurryTypes eHurry, int iData) const;
	// TAC - short messages for immigration after fist - RAY
	void doImmigrant(int iIndex, bool shortmessage);

	void buyLand(CvPlot* pPlot, bool bFree);

	int getNumTradeMessages() const;
	const wchar* getTradeMessage(int i) const;

	// TAC - Trade Messages - koma13 - START
	TradeMessageTypes getTradeMessageType(int i) const;
	YieldTypes getTradeMessageYield(int i) const;
	int getTradeMessageAmount(int i) const;
	int getTradeMessageCommission(int i) const;
	// TAC - Trade Messages - koma13 - END

	virtual void AI_init() = 0;
	virtual void AI_reset() = 0;
	virtual void AI_doTurnPre() = 0;
	virtual void AI_doTurnPost() = 0;
	virtual void AI_doTurnUnitsPre() = 0;
	virtual void AI_doTurnUnitsPost() = 0;
	virtual void AI_updateFoundValues(bool bStartingLoc = false) = 0;
	virtual void AI_unitUpdate() = 0;
	virtual void AI_makeAssignWorkDirty() = 0;
	virtual void AI_assignWorkingPlots() = 0;
	virtual void AI_updateAssignWork() = 0;
	virtual void AI_makeProductionDirty() = 0;
	virtual void AI_conquerCity(CvCity* pCity) = 0;
	virtual int AI_foundValue(int iX, int iY, int iMinUnitRange = -1, bool bStartingLoc = false) = 0;
	virtual int AI_getPlotDanger(CvPlot* pPlot, int iRange = -1, bool bTestMoves = true, bool bOffensive = false) const = 0;
	virtual bool AI_isWillingToTalk(PlayerTypes ePlayer) = 0;
	virtual bool AI_demandRebukedSneak(PlayerTypes ePlayer) = 0;
	virtual bool AI_demandRebukedWar(PlayerTypes ePlayer) = 0;
	virtual AttitudeTypes AI_getAttitude(PlayerTypes ePlayer, bool bForced = true) = 0;
	virtual int AI_dealVal(PlayerTypes ePlayer, const CLinkList<TradeData>* pList, bool bIgnoreAnnual = false, int iExtra = 0) = 0;
	virtual bool AI_considerOffer(PlayerTypes ePlayer, const CLinkList<TradeData>* pTheirList, const CLinkList<TradeData>* pOurList, int iChange = 1) = 0;
	virtual int AI_militaryHelp(PlayerTypes ePlayer, int& iNumUnits, UnitTypes& eUnit, ProfessionTypes& eProfession) = 0;
	virtual int AI_maxGoldTrade(PlayerTypes ePlayer) const = 0;
	virtual bool AI_counterPropose(PlayerTypes ePlayer, const CLinkList<TradeData>* pTheirList, const CLinkList<TradeData>* pOurList, CLinkList<TradeData>* pTheirInventory, CLinkList<TradeData>* pOurInventory, CLinkList<TradeData>* pTheirCounter, CLinkList<TradeData>* pOurCounter, const IDInfo& kTransport) = 0;
	virtual int AI_cityTradeVal(CvCity* pCity, PlayerTypes eOwner = NO_PLAYER) = 0;
	virtual DenialTypes AI_cityTrade(CvCity* pCity, PlayerTypes ePlayer) const = 0;
	virtual DenialTypes AI_stopTradingTrade(TeamTypes eTradeTeam, PlayerTypes ePlayer) const = 0;
	virtual DenialTypes AI_yieldTrade(YieldTypes eYield, const IDInfo& kTransport, PlayerTypes ePlayer) const = 0;
	virtual int AI_unitValue(UnitTypes eUnit, UnitAITypes eUnitAI, CvArea* pArea) = 0;
	virtual int AI_totalUnitAIs(UnitAITypes eUnitAI) = 0;
	virtual int AI_totalAreaUnitAIs(CvArea* pArea, UnitAITypes eUnitAI) = 0;
	virtual int AI_totalWaterAreaUnitAIs(CvArea* pArea, UnitAITypes eUnitAI) = 0;
	virtual int AI_plotTargetMissionAIs(CvPlot* pPlot, MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup = NULL, int iRange = 0) = 0;
	virtual int AI_unitTargetMissionAIs(CvUnit* pUnit, MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup = NULL) = 0;
	virtual int AI_civicValue(CivicTypes eCivic) = 0;
	virtual int AI_getNumAIUnits(UnitAITypes eIndex) = 0;
	virtual void AI_changePeacetimeTradeValue(PlayerTypes eIndex, int iChange) = 0;
	virtual void AI_changePeacetimeGrantValue(PlayerTypes eIndex, int iChange) = 0;
	virtual int AI_getAttitudeExtra(PlayerTypes eIndex) = 0;
	virtual void AI_setAttitudeExtra(PlayerTypes eIndex, int iNewValue) = 0;
	virtual void AI_changeAttitudeExtra(PlayerTypes eIndex, int iChange) = 0;
	virtual void AI_setFirstContact(PlayerTypes eIndex, bool bNewValue) = 0;
	virtual int AI_getMemoryCount(PlayerTypes eIndex1, MemoryTypes eIndex2) = 0;
	virtual void AI_changeMemoryCount(PlayerTypes eIndex1, MemoryTypes eIndex2, int iChange) = 0;
	virtual EventTypes AI_chooseEvent(int iTriggeredId) = 0;
	virtual void AI_doAdvancedStart(bool bNoExit = false) = 0;
	virtual int AI_getExtraGoldTarget() const = 0;
	virtual void AI_setExtraGoldTarget(int iNewValue) = 0;
	virtual void AI_chooseCivic(CivicOptionTypes eCivicOption) = 0;
	virtual bool AI_chooseGoody(GoodyTypes eGoody) = 0;
	virtual CvCity* AI_findBestCity() const = 0;
	// TAC - AI purchases military units - koma13 - START
	//virtual CvCity* AI_findBestPort() const = 0;
	virtual CvCity* AI_findBestPort(CvArea* pArea = NULL, CvCity* pNoAccessCity = NULL) const = 0;
	// TAC - AI purchases military units - koma13 - END

	bool checkPopulation() const;
	bool checkPower(bool bReset);

	/*** TRIANGLETRADE 10/15/08 by DPII ***/
	bool canTradeWithAfrica() const;
	int getSellToAfricaProfit(YieldTypes eYield, int iAmount) const;
	bool isYieldAfricaTradable(YieldTypes eYield) const;
	// R&R, ray, Port Royal
	bool canTradeWithPortRoyal() const;
	int getSellToPortRoyalProfit(YieldTypes eYield, int iAmount) const;
	bool isYieldPortRoyalTradable(YieldTypes eYield) const;

	// R&R mod, vetiarvind, trade groups - start
	int addTradeRouteGroup(const std::wstring& groupName);
	bool editTradeRouteGroup(int iId, const std::wstring& groupName);
	bool removeTradeRouteGroup(int iId);
	CvTradeRouteGroup* getTradeRouteGroupById(int tradeGroupId) const;
	CvTradeRouteGroup* getTradeRouteGroup(int iIndex) const;
	int getNumTradeGroups() const;
	// R&R mod, vetiarvind, trade groups - end

	// get constant (including xml) info
	CvCivilizationInfo& getCivilizationInfo() const;
	BuildingTypes getBuildingType(BuildingClassTypes eBuildingClass) const;
	UnitTypes getUnitType(UnitClassTypes eUnitClass) const;

	void writeDesyncLog(FILE *f) const;

	DirectionTypes getPreferredStartingDirection() const;
	std::vector<CvUnit*> getPortUnitsByProfession(ProfessionTypes eProfession) const;
	CvUnit* buyYieldUnit(YieldTypes eYield, int iAmount, CvUnit* pTransport, Port port);

protected:

/** NBMOD REF **/
	int m_iNBMOD_REF_StartValue;
	bool m_bNBMOD_REF_Display;
	int m_iNBMOD_REF_DisplayTurn;
/** NBMOD REF **/

/** NBMOD TAX **/
	int m_iMaxTaxRate;

	int NBMOD_GetNewTaxRate(int iWantedTax) const;

	//ray10
	//in public verlagert
	//void NBMOD_IncreaseMaxTaxRate(); // Steuergrenze anheben
	//void NBMOD_DecreaseMaxTaxRate(); // Steuergrenze senken
	//Ende ray10

/** NBMOD TAX **/
	int m_iStartingX;
	int m_iStartingY;
	int m_iTotalPopulation;
	int m_iTotalLand;
	int m_iTotalLandScored;
	int m_iGold;
	int m_iAdvancedStartPoints;
	int m_iGreatGeneralsCreated;
	int m_iGreatGeneralsThresholdModifier;
	int m_iGreatAdmiralsCreated; // R&R, ray, Great Admirals
	int m_iGreatAdmiralsThresholdModifier; // R&R, ray, Great Admirals
	int m_iGreatGeneralRateModifier;
	int m_iDomesticGreatGeneralRateModifier;
	int m_iImmigrationThresholdMultiplier;
	int m_iRevolutionEuropeUnitThresholdMultiplier;
	int m_iKingNumUnitMultiplier;
	int m_iNativeAngerModifier;
	int m_iFreeExperience;
	int m_iWorkerSpeedModifier;
	int m_iImprovementUpgradeRateModifier;
	int m_iImprovementUpgradeDurationModifier; // WTP, ray, Improvement Growth Modifier - START
	int m_iMilitaryProductionModifier;
	int m_iCityDefenseModifier;
	int m_iHighestUnitLevel;
	int m_iFatherOverflowBells;
	int m_iExpInBorderModifier;
	int m_iLevelExperienceModifier;
	int m_iCapitalCityID;
	int m_iCitiesLost;
	int m_iAssets;
	int m_iPower;
	int m_iPopulationScore;
	int m_iLandScore;
	int m_iFatherScore;
	int m_iCombatExperience;
	int m_iSeaCombatExperience; // R&R, ray, Great Admirals
	int m_iPopRushHurryCount;
	int m_iCrossesStored;
	int m_iBellsStored;
	int m_iTaxRate;
	int m_iNativeCombatModifier;
	int m_iDominateNativeBordersCount;
	int m_iRevolutionEuropeTradeCount;
	int m_iFatherPointMultiplier;
	int m_iMissionaryRateModifier;
	int m_iNativeTradeModifier; // R&R, ray, new Attribute in Traits
	int m_iTotalPlayerAfricaSellProfitModifierInPercent; // WTP, Africa and Port Royal Profit Modifiers - START
	int m_iTotalPlayerPortRoyalSellProfitModifierInPercent; // WTP, Africa and Port Royal Profit Modifiers - START
	int m_iTotalPlayerDomesticMarketProfitModifierInPercent; // WTP, ray, Domestic Market Profit Modifier - START
	int m_iMissionarySuccessPercent;
	int m_iNativeTradePostSuccessPercent; // WTP, ray, Native Trade Posts - START

	uint m_uiStartTime;  // XXX save these?

	bool m_bAlive;
	bool m_bEverAlive;
	bool m_bTurnActive;
	bool m_bAutoMoves;
	bool m_bEndTurn;
	bool m_bPbemNewTurn;
	bool m_bExtendedGame;
	bool m_bFoundedFirstCity;
	bool m_bStrike;
	bool m_bHuman;
	// R&R, ray, Bargaining - START
	bool m_bWillingToBargain;
	int m_iTimeNoTrade;
	// R&R, ray, Bargaining - END

	// R&R, ray, Timers Diplo Events - START
	int m_iTimerNativeMerc;
	int m_iTimerEuropeanWars;
	int m_iTimerEuropeanPeace;
	int m_iTimerRoyalInterventions; // WTP, ray, Royal Intervention, START
	int m_iTimerPrivateersDiploEvent; // WTP, ray, Privateers DLL Diplo Event - START
	int m_iTimerPrisonsCrowded;
	int m_iTimerRevolutionaryNoble;
	int m_iTimerBishop;
	int m_iTimerChurchDemand;
	int m_iTimerChurchWar;
	int m_iTimerColonialInterventionInNativeWar; //WTP, ray, Colonial Intervention In Native War - START
	int m_iTimerColoniesAndNativeAlliesWar; // WTP, ray, Big Colonies and Native Allies War - START
	int m_iTimerSmugglingShip;
	int m_iTimerRanger;
	int m_iTimerConquistador;
	int m_iTimerPirates;
	int m_iTimerContinentalGuard;
	int	m_iTimerMortar;
	int m_iTimerNativeSlave;
	int m_iTimerAfricanSlaves;
	int m_iTimerStealingImmigrant;
	// R&R, ray, Timers Diplo Events - END

	int m_iDSecondPlayerFrenchNativeWar; //WTP, ray, Colonial Intervention In Native War - START

	//WTP, ray Kings Used Ship - START
	int m_iTimerUsedShips;
	//WTP, ray Kings Used Ship - END

	// WTP, ray, Foreign Kings, buy Immigrants - START
	int m_iTimerForeignImmigrants;
	// WTP, ray, Foreign Kings, buy Immigrants - END

	int m_iChurchFavoursReceived; // R&R, ray, Church Favours

	// a random seed, which is only set at start of doTurn, hence constant for a turn
	// useful for avoiding OOS issues and if a random number should return the same each time for an entire turn
	unsigned long m_ulRandomSeed;

	PlayerTypes m_eID;
	LeaderHeadTypes m_ePersonalityType;
	EraTypes m_eCurrentEra;
	PlayerTypes m_eParent;
	PlayerTypes m_eColony;
	TeamTypes m_eTeamType;
	YieldTypes m_eImmigrationConversion;

	CvPlayerAI* m_pParent;
	CvPlayerAI* m_pColony;

	EnumMap<YieldTypes, int> m_em_iLandPlotYield; // R&R, ray, Landplot Yields - START
	EnumMap<YieldTypes, int> m_em_iSeaPlotYield;
	EnumMap<YieldTypes, int> m_em_iYieldRateModifier;
	EnumMap<YieldTypes, int> m_em_iCapitalYieldRateModifier;
	EnumMap<YieldTypes, int> m_em_iBuildingRequiredYieldModifier;
	EnumMap<YieldTypes, int> m_em_iCityExtraYield;
	EnumMap<YieldTypes, int> m_em_iExtraYieldThreshold;
	EnumMap<YieldTypes, int> m_em_iYieldBuyPrice;
	EnumMap<YieldTypes, int> m_em_iYieldAfricaBuyPrice; // R&R, ray, Africa
	EnumMap<YieldTypes, int> m_em_iYieldPortRoyalBuyPrice; // R&R, ray, Port Royal
	EnumMap<YieldTypes, int> m_em_iYieldTradedTotal;
	EnumMap<YieldTypes, int> m_em_iYieldTradedTotalAfrica; // WTP, ray, Yields Traded Total for Africa and Port Royal - START
	EnumMap<YieldTypes, int> m_em_iYieldTradedTotalPortRoyal; // WTP, ray, Yields Traded Total for Africa and Port Royal - START
	EnumMap<YieldTypes, int> m_em_iYieldBoughtTotal;
	EnumMap<YieldTypes, int> m_em_iYieldBoughtTotalAfrica; // WTP, ray, Yields Traded Total for Africa and Port Royal - START
	EnumMap<YieldTypes, int> m_em_iYieldBoughtTotalPortRoyal; // WTP, ray, Yields Traded Total for Africa and Port Royal - START
	EnumMap<YieldTypes, int> m_em_iTaxYieldModifierCount;
	EnumMap<YieldTypes, int> m_em_iYieldScoreTotal; // R&R, vetiarvind, Price dependent tax rate change

	EnumMap<YieldTypes, bool, true> m_em_bYieldEuropeTradable;
	EnumMap<FeatTypes, bool> m_em_bFeatAccomplished;
	EnumMap<PlayerOptionTypes, bool> m_em_bOptions;

	CvString m_szScriptData;

	EnumMap<ImprovementTypes, int> m_em_iImprovementCount;
	EnumMap<BuildingTypes, int> m_em_iFreeBuildingCount;
	EnumMap<UnitClassTypes, int> m_em_iUnitClassCount;
	EnumMap<UnitClassTypes, int> m_em_iUnitClassMaking;
	EnumMap<UnitClassTypes, int> m_em_iUnitClassImmigrated;
	EnumMap<UnitClassTypes, int> m_em_iUnitMoveChange;
	EnumMap<UnitClassTypes, int> m_em_iUnitStrengthModifier;
	EnumMap<ProfessionTypes, int> m_em_iProfessionCombatChange;
	EnumMap<ProfessionTypes, int> m_em_iProfessionMoveChange;
	EnumMap<BuildingClassTypes, int> m_em_iBuildingClassCount;
	EnumMap<BuildingClassTypes, int> m_em_iBuildingClassMaking;
	EnumMap<HurryTypes, int> m_em_iHurryCount;
	EnumMap<SpecialBuildingTypes, int> m_em_iSpecialBuildingNotRequiredCount;
	EnumMap<PlayerTypes, int> m_em_iMissionaryPoints;
	EnumMap<PlayerTypes, int, 100> m_em_iMissionaryThresholdMultiplier;
	EnumMap<ProfessionTypes, int> m_em_iProfessionEquipmentModifier;
	EnumMap<TraitTypes, int> m_em_iTraitCount;
	// cache CvPlayer::getYieldEquipmentAmount - start - Nightinggale
	YieldArray<unsigned short> *m_cache_YieldEquipmentAmount;
	void Update_cache_YieldEquipmentAmount();
	bool Update_cache_YieldEquipmentAmount(ProfessionTypes eProfession);
	int getYieldEquipmentAmountUncached(ProfessionTypes eProfession, YieldTypes eYield) const;
	// cache CvPlayer::getYieldEquipmentAmount - start - Nightinggale
	std::vector<EventTriggerTypes> m_triggersFired;
	EnumMap<CivicOptionTypes, CivicTypes> m_em_eCivics;
	EnumMap<ImprovementTypes  , EnumMap<YieldTypes, int> > m_em_iImprovementYieldChange;
	EnumMap<BuildingClassTypes, EnumMap<YieldTypes, int> > m_em_iBuildingYieldChange;
	CLinkList<int> m_groupCycle;
	std::vector<CvWString> m_aszCityNames;
	FFreeListTrashArray<CvCityAI> m_cities;
	CvIdVector<CvTradeRoute> m_tradeRoutes;
	CvIdVector<CvUnitAI> m_units;
	std::vector<CvUnit*> m_aEuropeUnits;
	std::vector<CvUnit*> m_aAfricaUnits; /*** TRIANGLETRADE 10/23/08 by DPII ***/
	std::vector<CvUnit*> m_aPortRoyalUnits; // R&R, ray, Port Royal
	FFreeListTrashArray<CvSelectionGroupAI> m_selectionGroups;
	FFreeListTrashArray<EventTriggeredData> m_eventsTriggered;
	CvEventMap m_mapEventsOccured;
	CvEventMap m_mapEventCountdown;
	UnitCombatPromotionArray m_aFreeUnitCombatPromotions;
	UnitClassPromotionArray m_aFreeUnitClassPromotions;

	std::vector< std::pair<UnitTypes, ProfessionTypes> > m_aEuropeRevolutionUnits;
	std::vector<UnitTypes> m_aDocksNextUnits;
	CvMessageQueue m_listGameMessages;
	CvPopupQueue m_listPopups;
	CvDiploQueue m_listDiplomacy;
	CvTurnScoreMap m_mapScoreHistory;
	CvTurnScoreMap m_mapEconomyHistory;
	CvTurnScoreMap m_mapIndustryHistory;
	CvTurnScoreMap m_mapAgricultureHistory;
	CvTurnScoreMap m_mapPowerHistory;
	CvTurnScoreMap m_mapCultureHistory;
	std::vector<CvWString> m_aszTradeMessages;
	// TAC - Trade Messages - koma13 - START
	std::vector<TradeMessageTypes> m_aeTradeMessageTypes;
	std::vector<YieldTypes> m_aeTradeMessageYields;
	std::vector<int> m_aiTradeMessageAmounts;
	std::vector<int> m_aiTradeMessageCommissions;
	// TAC - Trade Messages - koma13 - END

	void doGold();
	void doBells();
	void doCrosses();
	void doWarnings();
	void doEvents();
	void doPrices();
	void doTaxRaises(); //WTP, jburet, Tax Reform
	void doAfricaPrices(); // R&R, ray, Africa
	void doPortRoyalPrices(); // R&R, ray, Port Royal
	void wipeRoyalYieldScore(); //WTP, jburet, Tax Reform

	const int getFullYieldScore(bool fullCalculation);//WTP, jburet, Tax Reform
	const int getTaxThresold(bool fullCalculation);//WTP, jburet, Tax Reform

	bool checkExpireEvent(EventTypes eEvent, const EventTriggeredData& kTriggeredData) const;
	void expireEvent(EventTypes eEvent, const EventTriggeredData& kTriggeredData, bool bFail);
	CvCity* pickTriggerCity(EventTriggerTypes eTrigger) const;
	CvUnit* pickTriggerUnit(EventTriggerTypes eTrigger, CvPlot* pPlot, bool bPickPlot) const;
	void freeEuropeUnits();
	void freeAfricaUnits(); /*** TRIANGLETRADE 10/23/08 by DPII ***/
	void freePortRoyalUnits(); // R&R, ray, Port Royal
	void processCivics(CivicTypes eCivic, int iChange);

	// for serialization
	virtual void read(FDataStreamBase* pStream);
	virtual void write(FDataStreamBase* pStream);

	void read(CvSavegameReader reader);
	void write(CvSavegameWriter writer);

	void resetSavedData(PlayerTypes eID = NO_PLAYER, bool bConstructorCall = false);

	void doUpdateCacheOnTurn();
	void onTurnLogging() const; // K-Mod

	bool hasAnyChanceOfWinning();
	bool canRespawn();
	void buyEuropeSettlerIfLandlockedAI();
	void respawnTaxIncrease();
	bool isUnitInActiveCombat();
	void killMissionsAndTradeposts();
	// WTP, jooe: move "make peace with all on player kill" logic to team kill and remove it here
	// void makePeaceWithAll();
	void kill();

	// WTP, jooe, functionalise the buy Unit logic. Return value is the city where the units will appear (or NULL)
	CvCity* buyUnitFromParentPlayer(PlayerTypes eSellingPlayer, const char *szUnitClass, int iNumUnits, CvWString szIDTag = CvWString(), int iPriceToPay = 0, LocationFlags eLocationFlags = LocationFlags::LocationFlagNone, bool bReceivePrice = true, bool bMessageMentionLocation = true);
	CvCity* buyUnitFromPlayer(PlayerTypes eSellingPlayer, UnitClassTypes eUnitClass, int iNumUnits, CvWString szIDTag = CvWString(), int iPriceToPay = 0, LocationFlags eLocationFlags = LocationFlags::LocationFlagNone, bool bReceivePrice = true, bool bMessageMentionLocation = true);
	CvCity* buyUnitFromPlayer(PlayerTypes eSellingPlayer, UnitTypes eUnitType, int iNumUnits, CvWString szIDTag = CvWString(), int iPriceToPay = 0, LocationFlags eLocationFlags = LocationFlags::LocationFlagNone, bool bReceivePrice = true, bool bMessageMentionLocation = true);

	void testOOSanDoEvent(EventTypes eEvent, bool bSuccess) const;
	void testOOSanDoGoody(GoodyTypes eGoody, int iUnitID, bool bSuccess) const;

	// Temp unit which is used to generate paths for hypothetical units.
	// Kept around rather than created each usage to avoid chewing through the ID space.
	CvUnit* m_pTempUnit;

	// transport feeder - start - Nightinggale
public:
	int getIDSecondPlayerFrenchNativeWar() const;//WTP, ray, Colonial Intervention In Native War - START
	// transport feeder - start - Nightinggale
	void updateTransportThreshold();
	void updateTransportThreshold(YieldTypes eYield);
	// transport feeder - end - Nightinggale
	void sortEuropeUnits();
	void postLoadFixes();

	// Clean this up
	std::vector<ProfessionTypes> m_validCityJobProfessions;

protected:
	int m_iOppressometerDiscriminationModifier;
	int m_iOppressometerForcedLaborModifier;
	long long m_lPlayerOppressometer;

	void changeOppressometerDiscriminationModifier(int iChange);
	void changeOppressometerForcedLaborModifier(int iChange);
	void recalculatePlayerOppressometer();

public:
	int getOppressometerDiscriminationModifier() const
	{
		return m_iOppressometerDiscriminationModifier;
	}
	int getOppressometerForcedLaborModifier() const
	{
		return m_iOppressometerForcedLaborModifier;
	}
	long long getPlayerOppressometer() const
	{
		return m_lPlayerOppressometer;
	}
	CvUnit* getOrCreateTempUnit(UnitTypes eUnit, int iX, int iY);
	void releaseTempUnit();

	inline bool isTempUnit(const CvUnit* pUnit) const
	{
		return (pUnit == m_pTempUnit || pUnit->AI_getBirthmark() == UNIT_BIRTHMARK_TEMP_UNIT);
	}
};

// cache CvPlayer::getYieldEquipmentAmount - start - Nightinggale
inline int CvPlayer::getYieldEquipmentAmount(ProfessionTypes eProfession, YieldTypes eYield) const
{
	FAssert(m_cache_YieldEquipmentAmount != NULL);
	FAssert(eProfession >= 0 && eProfession < GC.getNumProfessionInfos());
	return m_cache_YieldEquipmentAmount[eProfession].get(eYield);
}

inline bool CvPlayer::hasContentsYieldEquipmentAmount(ProfessionTypes eProfession) const
{
	// strictly speaking it returns true if the array is allocated without considering the content of the array.
	// The reason why it works is because Update_cache_YieldEquipmentAmount() will only allocate arrays if they contain anything
	//   and deallocate them if it is changed to contain only 0.
	FAssert(m_cache_YieldEquipmentAmount != NULL);
	FAssert(eProfession >= 0 && eProfession < GC.getNumProfessionInfos());
	return m_cache_YieldEquipmentAmount[eProfession].isAllocated();
}

// same functions, but with the added return 0 if professions is NO_PROFESSION or INVALID_PROFESSION
inline int CvPlayer::getYieldEquipmentAmountSecure(ProfessionTypes eProfession, YieldTypes eYield) const
{
	return eProfession > NO_PROFESSION ? getYieldEquipmentAmount(eProfession, eYield) : 0;
}

inline bool CvPlayer::hasContentsYieldEquipmentAmountSecure(ProfessionTypes eProfession) const
{
	return eProfession > NO_PROFESSION ? hasContentsYieldEquipmentAmount(eProfession) : false;
}
// cache CvPlayer::getYieldEquipmentAmount - end - Nightinggale
#endif
