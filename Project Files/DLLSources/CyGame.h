#pragma once
#ifndef CyGame_h
#define CyGame_h
//
// Python wrapper class for CvGame
// SINGLETON
// updated 6-5
//#include "CvEnums.h"
class CvGame;
class CvGameAI;
class CyCity;
class CvRandom;
class CyDeal;
class CyReplayInfo;
class CyPlot;
class CyGame
{
public:
	CyGame();
	CyGame(CvGame* pGame);			// Call from C++
	CyGame(CvGameAI* pGame);			// Call from C++;
	CvGame* getGame() { return m_pGame;	}	// Call from C++

	// R&R, ray, Correct Geographical Placement of Natives - START
	bool isWBNorthAmericanNative();
	void setWBNorthAmericanNative(bool bValue);
	bool isWBSouthAmericanNative();
	void setWBSouthAmericanNative(bool bValue);
	bool isWBCentralAmericanNative();
	void setWBCentralAmericanNative(bool bValue);
	// R&R, ray, Correct Geographical Placement of Natives - END

	bool isNone() { return (m_pGame==NULL); }
	void updateScore(bool bForce);
	void cycleCities(bool bForward, bool bAdd);
	void cycleSelectionGroups(bool bClear, bool bForward);
	bool cyclePlotUnits(CyPlot* pPlot, bool bForward, bool bAuto, int iCount);
	void selectionListMove(CyPlot* pPlot, bool bAlt, bool bShift, bool bCtrl);
	void selectionListGameNetMessage(int eMessage, int iData2, int iData3, int iData4, int iFlags, bool bAlt, bool bShift);
	void selectedCitiesGameNetMessage(int eMessage, int iData2, int iData3, int iData4, bool bOption, bool bAlt, bool bShift, bool bCtrl);
	void cityPushOrder(CyCity* pCity, OrderTypes eOrder, int iData, bool bAlt, bool bShift, bool bCtrl);
	int getSymbolID(int iSymbol);
	int getProductionPerPopulation(int /*HurryTypes*/ eHurry);
	int getAdjustedPopulationPercent(int /*VictoryTypes*/ eVictory);
	int getAdjustedLandPercent(int /* VictoryTypes*/ eVictory);
	int countCivPlayersAlive();
	int countCivPlayersEverAlive();
	int countCivTeamsAlive();
	int countCivTeamsEverAlive();
	int countHumanPlayersAlive();
	int countHumanPlayersEverAlive();
	int countTotalCivPower();
	int getImprovementUpgradeTime(int /* ImprovementTypes*/ eImprovement);
	int /* EraTypes */ getCurrentEra();
	int getActiveTeam();
	int /* CivilizationTypes */ getActiveCivilizationType();
	bool isNetworkMultiPlayer();
	bool isGameMultiPlayer();
	bool isTeamGame();
	bool isModem();
	void setModem(bool bModem);
	void reviveActivePlayer();
	int getNumHumanPlayers();
	int getGameTurn();
	void setGameTurn(int iNewValue);
	int getTurnYear(int iGameTurn);
	int getGameTurnYear();
	int getElapsedGameTurns();
	int getMaxTurns() const;
	void setMaxTurns(int iNewValue);
	void changeMaxTurns(int iChange);
	int getMaxCityElimination() const;
	void setMaxCityElimination(int iNewValue);
	int getNumAdvancedStartPoints() const;
	void setNumAdvancedStartPoints(int iNewValue);
	int getStartTurn() const;
	int getStartYear() const;
	void setStartYear(int iNewValue);
	int getEstimateEndTurn() const;
	void setEstimateEndTurn(int iNewValue);
	int getTurnSlice() const;
	int getMinutesPlayed() const;
	int getTargetScore() const;
	int /*TurnTimerTypes*/ getTurnTimerType() const;
	int getNumGameTurnActive();
	int countNumHumanGameTurnActive();
	int getNumCities();
	int getTotalPopulation();
	int getMaxPopulation() const;
	int getMaxLand() const;
	int getMaxFather() const;
	int getInitPopulation() const;
	int getInitLand() const;
	int getInitFather() const;
	int getAIAutoPlay() const;
	void setAIAutoPlay(int iNewValue);
	bool isScoreDirty() const;
	void setScoreDirty(bool bNewValue);
	bool isDebugMode() const;
	void toggleDebugMode();
	int getPitbossTurnTime();
	void setPitbossTurnTime(int iHours);
	bool isHotSeat();
	bool isPbem();
	bool isPitboss();
	bool isSimultaneousTeamTurns();
	bool isFinalInitialized();
	int /*PlayerTypes*/ getActivePlayer();
	void setActivePlayer(int /*PlayerTypes*/ eNewValue, bool bForceHotSeat);
	int getPausePlayer();
	bool isPaused();
	int getBestLandUnitCombat();
	int /*TeamTypes*/ getWinner();
	int /*VictoryTypes*/ getVictory();
	void setWinner(int /*TeamTypes*/ eNewWinner, int /*VictoryTypes*/ eNewVictory);
	int /*GameStateTypes*/ getGameState();
	int /*HandicapTypes*/ getHandicapType();
	CalendarTypes getCalendar() const;
	int /*EraTypes*/ getStartEra();
	int /*GameSpeedTypes*/ getGameSpeedType();
	int getCargoYieldCapacity();
	/*PlayerTypes*/ int getRankPlayer(int iRank);
	int getPlayerScore(int /*PlayerTypes*/ iIndex);
	int /*TeamTypes*/ getRankTeam(int iRank);
	int getTeamRank(int /*TeamTypes*/ iIndex);
	int getTeamScore(int /*TeamTypes*/ iIndex);
	bool isOption(int /*GameOptionTypes*/ eIndex);
	void setOption(int /*GameOptionTypes*/ eIndex, bool bEnabled);
	bool isMPOption(int /*MultiplayerOptionTypes*/ eIndex);
	bool isForcedControl(int /*ForceControlTypes*/ eIndex);
	int getUnitCreatedCount(int /*UnitTypes*/ eIndex);
	int getUnitClassCreatedCount(int /*UnitClassTypes*/ eIndex);
	int getBuildingClassCreatedCount(int /*BuildingClassTypes*/ eIndex);
	bool isVictoryValid(int /*VictoryTypes*/ eIndex);
	bool isSpecialUnitValid(int /*SpecialUnitTypes*/ eSpecialUnitType);
	void makeSpecialUnitValid(int /*SpecialUnitTypes*/ eSpecialUnitType);
	bool isSpecialBuildingValid(int /*SpecialBuildingTypes*/ eIndex);
	void makeSpecialBuildingValid(int /*SpecialBuildingTypes*/ eIndex);
	bool isUniqueGoodyValid(int /*GoodyTypes*/ eIndex); // R&R, ray, Goody Enhancement
	void setUniqueGoodyValid(int /*GoodyTypes*/ eIndex, bool bValid); // R&R, ray, Goody Enhancement
	bool isInAdvancedStart();
	std::string getScriptData() const;
	void setScriptData(std::string szNewValue);
	void setName(char const* szName);
	std::wstring getName();
	int getIndexAfterLastDeal();
	int getNumDeals();
	CyDeal* getDeal(int iID);
	CyDeal* addDeal();
	void deleteDeal(int iID);
	CvRandom& getMapRand();
	int getMapRandNum(int iNum, char const* pszLog);
	CvRandom& getSorenRand();
	int getSorenRandNum(int iNum, char const* pszLog);
	int calculateSyncChecksum();
	int calculateOptionsChecksum();
	bool GetWorldBuilderMode() const;				// remove once CvApp is exposed
	bool isPitbossHost() const;				// remove once CvApp is exposed
	int getCurrentLanguage() const;				// remove once CvApp is exposed
	void setCurrentLanguage(int iNewLanguage);				// remove once CvApp is exposed
	int getReplayMessageTurn(int i) const;
	ReplayMessageTypes getReplayMessageType(int i) const;
	int getReplayMessagePlotX(int i) const;
	int getReplayMessagePlotY(int i) const;
	int getReplayMessagePlayer(int i) const;
	ColorTypes getReplayMessageColor(int i) const;
	std::wstring getReplayMessageText(int i) const;
	uint getNumReplayMessages() const;
	CyReplayInfo* getReplayInfo() const;
	void saveReplay(int iPlayer);
	void addPlayer(int /*PlayerTypes*/ eNewPlayer, int /*LeaderHeadTypes*/ eLeader, int /*CivilizationTypes*/ eCiv);
	void setPlotExtraYield(int iX, int iY, int /*YieldTypes*/ eYield, int iExtraYield);
	bool isCivEverActive(int /*CivilizationTypes*/ eCivilization);
	bool isLeaderEverActive(int /*LeaderHeadTypes*/ eLeader);
	bool isUnitEverActive(int /*UnitTypes*/ eUnit);
	bool isBuildingEverActive(int /*BuildingTypes*/ eBuilding);
	bool isEventActive(int /*EventTriggerTypes*/ eTrigger);
	int /*TeamTypes*/ getFatherTeam(int /*FatherTypes*/ eFather);
	int getFatherGameTurn(int /*FatherTypes*/ eFather);
	void setFatherTeam(int /*FatherTypes*/ eFather, int /*TeamTypes*/ eTeam);
	int getFatherCategoryPosition(int /*FatherTypes*/ eFather);
	// < JAnimals Mod Start >
	int /*PlayerTypes*/ getBarbarianPlayer();
	bool hasBarbarianPlayer();
	void setBarbarianPlayer(int /*PlayerTypes*/ eNewValue);
	bool isBarbarianPlayer(int /*PlayerTypes*/ ePlayer);
	int /*PlayerTypes*/ getNextPlayerType();
	// < JAnimals Mod End >
	// R&R, ray, the Church - START
	int /*PlayerTypes*/ getChurchPlayer();
	bool hasChurchPlayer();
	void setChurchPlayer(int /*PlayerTypes*/ eNewValue);
	bool isChurchPlayer(int /*PlayerTypes*/ ePlayer);
	// R&R, ray, the Church - END

protected:
	CvGame* m_pGame;
};
#endif	// #ifndef CyGame
