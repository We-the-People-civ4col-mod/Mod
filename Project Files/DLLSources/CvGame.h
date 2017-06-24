#pragma once

// game.h

#ifndef CIV4_GAME_H
#define CIV4_GAME_H

//#include "CvStructs.h"
#include "CvDeal.h"
#include "CvRandom.h"

class CvPlot;
class CvCity;
class CvReplayMessage;
class CvReplayInfo;

typedef std::vector<const CvReplayMessage*> ReplayMessageList;

class CvGame
{

public:

	DllExport CvGame();
	DllExport virtual ~CvGame();

	// PatchMod: Victorys START
	int iVictoriesSet;
	// PatchMod: Victorys END
	DllExport void init(HandicapTypes eHandicap);
	DllExport void reset(HandicapTypes eHandicap, bool bConstructorCall = false);

protected:

	void uninit();

public:

	DllExport void setInitialItems(bool bScenario);
	DllExport void regenerateMap();

	DllExport void initDiplomacy();
	DllExport void initFreeState();
	DllExport void initFreeUnits();
	DllExport void initImmigration();

	DllExport void assignStartingPlots();
	DllExport void normalizeStartingPlots();

	void assignNativeTerritory();

	DllExport void update();
	DllExport void updateScore(bool bForce = false);

	DllExport void updateColoredPlots();

	DllExport void updateCitySight(bool bIncrement);

	DllExport void updateSelectionList();
	DllExport void updateTestEndTurn();

	DllExport void testExtendedGame();

	DllExport CvUnit* getPlotUnit(const CvPlot* pPlot, int iIndex);
	DllExport void getPlotUnits(const CvPlot *pPlot, std::vector<CvUnit *> &plotUnits);
	DllExport void cycleCities(bool bForward = true, bool bAdd = false);
	DllExport void cycleSelectionGroups(bool bClear, bool bForward = true);
	DllExport bool cyclePlotUnits(CvPlot* pPlot, bool bForward = true, bool bAuto = false, int iCount = -1);

	DllExport void selectionListMove(CvPlot* pPlot, bool bAlt, bool bShift, bool bCtrl);
	DllExport void selectionListGameNetMessage(int eMessage, int iData2 = -1, int iData3 = -1, int iData4 = -1, int iFlags = 0, bool bAlt = false, bool bShift = false);
	DllExport void selectedCitiesGameNetMessage(int eMessage, int iData2 = -1, int iData3 = -1, int iData4 = -1, bool bOption = false, bool bAlt = false, bool bShift = false, bool bCtrl = false);
	DllExport void cityPushOrder(CvCity* pCity, OrderTypes eOrder, int iData, bool bAlt = false, bool bShift = false, bool bCtrl = false);

	DllExport void selectUnit(CvUnit* pUnit, bool bClear, bool bToggle = false, bool bSound = false);
	DllExport void selectGroup(CvUnit* pUnit, bool bShift, bool bCtrl, bool bAlt);
	DllExport void selectAll(CvPlot* pPlot);

	DllExport bool canHandleAction(int iAction, CvPlot* pPlot = NULL, bool bTestVisible = false, bool bUseCache = false);
	DllExport void setupActionCache();
	DllExport void handleAction(int iAction);

	DllExport bool canDoControl(ControlTypes eControl);
	DllExport void doControl(ControlTypes eControl);

	DllExport void implementDeal(PlayerTypes eWho, PlayerTypes eOtherWho, CLinkList<TradeData>* pOurList, CLinkList<TradeData>* pTheirList, bool bForce = false);
	DllExport void verifyDeals();

	DllExport void getGlobeviewConfigurationParameters(TeamTypes eTeam, bool& bStarsVisible, bool& bWorldIsRound);
	DllExport int getSymbolID(int iSymbol);

	int getProductionPerPopulation(HurryTypes eHurry);

	int getAdjustedPopulationPercent(VictoryTypes eVictory) const;
	int getAdjustedLandPercent(VictoryTypes eVictory) const;

	DllExport int countCivPlayersAlive() const;
	DllExport int countCivPlayersEverAlive() const;
	DllExport int countCivTeamsAlive() const;
	DllExport int countCivTeamsEverAlive() const;
	DllExport int countHumanPlayersAlive() const;
	DllExport int countHumanPlayersEverAlive() const;

	int countTotalCivPower();

	DllExport int getImprovementUpgradeTime(ImprovementTypes eImprovement) const;

	DllExport EraTypes getCurrentEra() const;

	DllExport TeamTypes getActiveTeam();
	DllExport CivilizationTypes getActiveCivilizationType();

	DllExport unsigned int getLastEndTurnMessageSentTime();
	DllExport bool isNetworkMultiPlayer() const;
	DllExport bool isGameMultiPlayer() const;
	DllExport bool isTeamGame() const;

	DllExport bool isModem();
	DllExport void setModem(bool bModem);
	DllExport void reviveActivePlayer();

	DllExport int getNumHumanPlayers();

	DllExport int getEndTurnMessagesSent();
	DllExport void incrementEndTurnMessagesSent();

	DllExport int getGameTurn();
	DllExport void setGameTurn(int iNewValue);
	DllExport void incrementGameTurn();
	DllExport int getTurnYear(int iGameTurn);
	DllExport int getGameTurnYear();

	DllExport int getElapsedGameTurns() const;
	void incrementElapsedGameTurns();
	bool isMaxTurnsExtended() const;
	void setMaxTurnsExtended(bool bExtended);
	DllExport int getMaxTurns() const;
	DllExport void setMaxTurns(int iNewValue);
	void changeMaxTurns(int iChange);

	DllExport void getTurnTimerText(CvWString& szBuffer) const;

	DllExport int getMaxCityElimination() const;
	DllExport void setMaxCityElimination(int iNewValue);

	DllExport int getNumAdvancedStartPoints() const;
	DllExport void setNumAdvancedStartPoints(int iNewValue);

	DllExport int getStartTurn() const;
	DllExport void setStartTurn(int iNewValue);
	DllExport int getStartYear() const;
	DllExport void setStartYear(int iNewValue);

	DllExport int getEstimateEndTurn() const;
	DllExport void setEstimateEndTurn(int iNewValue);

	DllExport int getTurnSlice() const;
	DllExport int getMinutesPlayed() const;
	DllExport void setTurnSlice(int iNewValue);
	DllExport void changeTurnSlice(int iChange);

	DllExport int getCutoffSlice() const;
	DllExport void setCutoffSlice(int iNewValue);
	DllExport void changeCutoffSlice(int iChange);

	DllExport int getTurnSlicesRemaining() const;
	DllExport void resetTurnTimer();
	DllExport void incrementTurnTimer(int iNumTurnSlices);
	TurnTimerTypes getTurnTimerType() const;
	DllExport int getMaxTurnLen();
	int getTargetScore() const;

	DllExport int getNumGameTurnActive();
	DllExport int countNumHumanGameTurnActive();
	void changeNumGameTurnActive(int iChange);
	DllExport int getNumCities() const;
	void changeNumCities(int iChange);
	DllExport int getTotalPopulation() const;
	int getMaxPopulation() const;
	int getMaxLand() const;
	int getMaxFather() const;
	DllExport int getInitPopulation() const;
	DllExport int getInitLand() const;
	DllExport int getInitFather() const;
	DllExport void initScoreCalculation();
	DllExport int getAIAutoPlay();
	DllExport void setAIAutoPlay(int iNewValue);
	DllExport void changeAIAutoPlay(int iChange);
	DllExport unsigned int getInitialTime();
	DllExport void setInitialTime(unsigned int uiNewValue);
	bool isScoreDirty() const;
	void setScoreDirty(bool bNewValue);

	DllExport bool isDebugMode() const;
	DllExport void toggleDebugMode();
	DllExport void updateDebugModeCache();
	DllExport int getPitbossTurnTime() const;
	DllExport void setPitbossTurnTime(int iHours);

	DllExport bool isHotSeat() const;
	DllExport bool isPbem() const;
	DllExport bool isPitboss() const;
	DllExport bool isSimultaneousTeamTurns() const;

	DllExport bool isFinalInitialized() const;
	DllExport void setFinalInitialized(bool bNewValue);

	DllExport bool getPbemTurnSent() const;
	DllExport void setPbemTurnSent(bool bNewValue);

	DllExport bool getHotPbemBetweenTurns() const;
	DllExport void setHotPbemBetweenTurns(bool bNewValue);

	DllExport bool isPlayerOptionsSent() const;
	DllExport void sendPlayerOptions(bool bForce = false);
	DllExport PlayerTypes getActivePlayer() const;
	DllExport void setActivePlayer(PlayerTypes eNewValue, bool bForceHotSeat = false);
	DllExport void updateUnitEnemyGlow();

	DllExport HandicapTypes getHandicapType() const;
	DllExport void setHandicapType(HandicapTypes eHandicap);
	DllExport PlayerTypes getPausePlayer();
	DllExport bool isPaused();
	DllExport void setPausePlayer(PlayerTypes eNewValue);
	DllExport int getBestLandUnitCombat();
	DllExport void setBestLandUnitCombat(int iNewValue);
	DllExport TeamTypes getWinner() const;
	DllExport VictoryTypes getVictory() const;
	DllExport void setWinner(TeamTypes eNewWinner, VictoryTypes eNewVictory);

	DllExport GameStateTypes getGameState() const;
	DllExport void setGameState(GameStateTypes eNewValue);
	DllExport EraTypes getStartEra() const;

	CalendarTypes getCalendar() const;

	GameSpeedTypes getGameSpeedType() const;
	int getCultureLevelThreshold(CultureLevelTypes eCultureLevel) const;
	int getCargoYieldCapacity() const;

	PlayerTypes getRankPlayer(int iRank);
	void setRankPlayer(int iRank, PlayerTypes ePlayer);
	DllExport int getPlayerScore(PlayerTypes ePlayer);
	void setPlayerScore(PlayerTypes ePlayer, int iScore);
	TeamTypes getRankTeam(int iRank);
	void setRankTeam(int iRank, TeamTypes eTeam);
	int getTeamRank(TeamTypes eTeam);
	void setTeamRank(TeamTypes eTeam, int iRank);
	int getTeamScore(TeamTypes eTeam) const;
	void setTeamScore(TeamTypes eTeam, int iScore);
	DllExport bool isOption(GameOptionTypes eIndex) const;
	DllExport void setOption(GameOptionTypes eIndex, bool bEnabled);
	DllExport bool isMPOption(MultiplayerOptionTypes eIndex) const;
	DllExport void setMPOption(MultiplayerOptionTypes eIndex, bool bEnabled);
	DllExport bool isForcedControl(ForceControlTypes eIndex) const;
	DllExport void setForceControl(ForceControlTypes eIndex, bool bEnabled);
	int getUnitCreatedCount(UnitTypes eIndex);
	void incrementUnitCreatedCount(UnitTypes eIndex);
	int getUnitClassCreatedCount(UnitClassTypes eIndex);
	void incrementUnitClassCreatedCount(UnitClassTypes eIndex);
	int getBuildingClassCreatedCount(BuildingClassTypes eIndex);
	void incrementBuildingClassCreatedCount(BuildingClassTypes eIndex);
	bool isVictoryValid(VictoryTypes eIndex) const;
	bool isSpecialUnitValid(SpecialUnitTypes eIndex);
	void makeSpecialUnitValid(SpecialUnitTypes eIndex);

	bool isSpecialBuildingValid(SpecialBuildingTypes eIndex);
	void makeSpecialBuildingValid(SpecialBuildingTypes eIndex, bool bAnnounce = false);

	bool isUniqueGoodyValid(GoodyTypes eIndex); // R&R, ray, Goody Enhancement
	void setUniqueGoodyValid(GoodyTypes eIndex, bool bValid); // R&R, ray, Goody Enhancement

	DllExport bool isInAdvancedStart() const;

	DllExport const CvWString & getName();
	DllExport void setName(const TCHAR* szName);

	// Script data needs to be a narrow string for pickling in Python
	std::string getScriptData() const;
	void setScriptData(std::string szNewValue);

	bool isDestroyedCityName(const CvWString& szName) const;
	void addDestroyedCityName(const CvWString& szName);
	
	bool isGreatGeneralBorn(CvWString& szName) const;
	void addGreatGeneralBornName(const CvWString& szName);

	// R&R, ray, Great Admirals - START
	bool isGreatAdmiralBorn(CvWString& szName) const;
	void addGreatAdmiralBornName(const CvWString& szName);
	// R&R, ray, Great Admirals - END

	// TAC - Ship Names - Ray - Start
	bool isShipNamed(CvWString& szName) const;
	void addShipName(const CvWString& szName);
	// TAC - Ship Names - Ray - END

	DllExport int getIndexAfterLastDeal();
	DllExport int getNumDeals();
	DllExport CvDeal* getDeal(int iID);
	DllExport CvDeal* addDeal();
	DllExport void deleteDeal(int iID);
	// iteration
	DllExport CvDeal* firstDeal(int *pIterIdx, bool bRev=false);
	DllExport CvDeal* nextDeal(int *pIterIdx, bool bRev=false);

	CvRandom& getMapRand();
	int getMapRandNum(int iNum, const char* pszLog);

	CvRandom& getSorenRand();
	int getSorenRandNum(int iNum, const char* pszLog);

	DllExport int calculateSyncChecksum(CvString* pLogString);
	DllExport int calculateOptionsChecksum();

	DllExport void addReplayMessage(ReplayMessageTypes eType = NO_REPLAY_MESSAGE, PlayerTypes ePlayer = NO_PLAYER, CvWString pszText = L"",
		int iPlotX = -1, int iPlotY = -1, ColorTypes eColor = NO_COLOR);
	DllExport void clearReplayMessageMap();
	DllExport int getReplayMessageTurn(uint i) const;
	DllExport ReplayMessageTypes getReplayMessageType(uint i) const;
	DllExport int getReplayMessagePlotX(uint i) const;
	DllExport int getReplayMessagePlotY(uint i) const;
	DllExport PlayerTypes getReplayMessagePlayer(uint i) const;
	DllExport LPCWSTR getReplayMessageText(uint i) const;
	DllExport uint getNumReplayMessages() const;
	DllExport ColorTypes getReplayMessageColor(uint i) const;

	DllExport virtual void read(FDataStreamBase* pStream);
	DllExport virtual void write(FDataStreamBase* pStream);
	DllExport virtual void writeReplay(FDataStreamBase& stream, PlayerTypes ePlayer);

	virtual void AI_init() = 0;
	virtual void AI_reset() = 0;
	virtual void AI_makeAssignWorkDirty() = 0;
	virtual void AI_updateAssignWork() = 0;
	virtual int AI_combatValue(UnitTypes eUnit) = 0;

	CvReplayInfo* getReplayInfo() const;
	DllExport void setReplayInfo(CvReplayInfo* pReplay);
	void saveReplay(PlayerTypes ePlayer);

	void addPlayer(PlayerTypes eNewPlayer, LeaderHeadTypes eLeader, CivilizationTypes eCiv);

	bool testVictory(VictoryTypes eVictory, TeamTypes eTeam, bool* pbEndScore = NULL) const;

	bool culturalVictoryValid();
	int culturalVictoryNumCultureCities();
	CultureLevelTypes culturalVictoryCultureLevel();

	int getPlotExtraYield(int iX, int iY, YieldTypes eYield) const;
	void setPlotExtraYield(int iX, int iY, YieldTypes eYield, int iCost);
	void removePlotExtraYield(int iX, int iY);
	bool isEventActive(EventTriggerTypes eTrigger) const;
	DllExport void initEvents();
	bool isCivEverActive(CivilizationTypes eCivilization) const;
	bool isLeaderEverActive(LeaderHeadTypes eLeader) const;
	bool isUnitEverActive(UnitTypes eUnit) const;
	bool isBuildingEverActive(BuildingTypes eBuilding) const;

	TeamTypes getFatherTeam(FatherTypes eFather) const;
	int getFatherGameTurn(FatherTypes eFather) const;
	void setFatherTeam(FatherTypes eFather, TeamTypes eTeam);
	bool getRemainingFathers(FatherPointTypes ePointType, std::vector<FatherTypes>& aFathers);
	int getFatherCategoryPosition(FatherTypes eFather) const;

	void changeYieldBoughtTotal(PlayerTypes eMainEurope, YieldTypes eYield, int iChange) const;

	// < JAnimals Mod Start >
	DllExport PlayerTypes getBarbarianPlayer();
    DllExport bool hasBarbarianPlayer();
    void setBarbarianPlayer(PlayerTypes eNewValue);
    DllExport bool isBarbarianPlayer(PlayerTypes ePlayer);
	PlayerTypes getNextPlayerType() const;
	// < JAnimals Mod End >

	// R&R, ray, the Church - START
	DllExport PlayerTypes getChurchPlayer();
    DllExport bool hasChurchPlayer();
    void setChurchPlayer(PlayerTypes eNewValue);
    DllExport bool isChurchPlayer(PlayerTypes ePlayer);
	// R&R, ray, the Church - END

	// R&R, ray, Correct Geographical Placement of Natives - START
	DllExport bool isWBNorthAmericanNative() const;
	DllExport void setWBNorthAmericanNative(bool bValue);
	DllExport bool isWBSouthAmericanNative() const;
	DllExport void setWBSouthAmericanNative(bool bValue);
	DllExport bool isWBCentralAmericanNative() const;
	DllExport void setWBCentralAmericanNative(bool bValue);
	// R&R, ray, Correct Geographical Placement of Natives - END

protected:

	int m_iEndTurnMessagesSent;
	int m_iElapsedGameTurns;
	int m_iStartTurn;
	int m_iStartYear;
	int m_iEstimateEndTurn;
	int m_iTurnSlice;
	int m_iCutoffSlice;
	int m_iNumGameTurnActive;
	int m_iNumCities;
	int m_iMaxPopulation;
	int m_iMaxLand;
	int m_iMaxFather;
	int m_iInitPopulation;
	int m_iInitLand;
	int m_iInitFather;
	int m_iAIAutoPlay;
	int m_iBestLandUnitCombat;

	unsigned int m_uiInitialTime;

	bool m_bScoreDirty;
	bool m_bDebugMode;
	bool m_bDebugModeCache;
	bool m_bFinalInitialized;
	bool m_bPbemTurnSent;
	bool m_bHotPbemBetweenTurns;
	bool m_bPlayerOptionsSent;
	bool m_bMaxTurnsExtended;

	// R&R, ray, Correct Geographical Placement of Natives - START
	bool m_bWBNorthAmericanNative;
	bool m_bWBSouthAmericanNative;
	bool m_bWBCentralAmericanNative;
	// R&R, ray, Correct Geographical Placement of Natives - END

	HandicapTypes m_eHandicap;
	PlayerTypes m_ePausePlayer;
	PlayerTypes m_eBarbarianPlayer; // < JAnimals Mod Start >
	PlayerTypes m_eChurchPlayer; // R&R, ray, the Church - START
	TeamTypes m_eWinner;
	VictoryTypes m_eVictory;
	GameStateTypes m_eGameState;
	PlayerTypes m_eEventPlayer;

	CvString m_szScriptData;

	int* m_aiRankPlayer;        // Ordered by rank...
	int* m_aiPlayerScore;       // Ordered by player ID...
	int* m_aiRankTeam;						// Ordered by rank...
	int* m_aiTeamRank;						// Ordered by team ID...
	int* m_aiTeamScore;						// Ordered by team ID...

	int* m_paiUnitCreatedCount;
	int* m_paiUnitClassCreatedCount;
	int* m_paiBuildingClassCreatedCount;

	TeamTypes* m_aeFatherTeam;
	int* m_aiFatherGameTurn;

	bool* m_pabSpecialUnitValid;
	bool* m_pabSpecialBuildingValid;

	bool* m_pabUniqueGoodyValid; // R&R, ray, Goody Enhancement

	std::vector<CvWString> m_aszDestroyedCities;
	std::vector<CvWString> m_aszGreatGeneralBorn;
	std::vector<CvWString> m_aszGreatAdmiralBorn; // R&R, ray, Great Admirals - START
	std::vector<CvWString> m_aszShipNamed; // TAC - Ship Names - Ray - Start
	

	FFreeListTrashArray<CvDeal> m_deals;

	CvRandom m_mapRand;
	CvRandom m_sorenRand;
	ReplayMessageList m_listReplayMessages;
	CvReplayInfo* m_pReplayInfo;

	int m_iNumSessions;

	std::vector<PlotExtraYield> m_aPlotExtraYields;
	std::vector<EventTriggerTypes> m_aeInactiveTriggers;

	// CACHE: cache frequently used values
	int		m_iNumCultureVictoryCities;
	int		m_eCultureVictoryCultureLevel;

	void doTurn();
	// < JAnimals Mod Start >
	void createBarbarianPlayer();
	void createAnimalsLand();
	void createAnimalsSea();
	// < JAnimals Mod End >

	void createChurchPlayer(); // R&R, ray, the Church - START

	void doDeals();

	void verifyCivics();

	void updateWar();
	void updateMoves();
	void updateTimers();
	void updateTurnTimer();

	void testAlive();
	void testVictory();

	int getTeamClosenessScore(int** aaiDistances, int* aiStartingLocs);
	void normalizeStartingPlotLocations();

	void showEndGameSequence();

	void updateOceanDistances();

	void doUpdateCacheOnTurn();
};

#endif
