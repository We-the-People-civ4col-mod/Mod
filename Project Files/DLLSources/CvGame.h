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

typedef std::vector<CvReplayMessage*> ReplayMessageList;

class CvGame
{

public:

	CvGame();
	virtual ~CvGame();

	// PatchMod: Victorys START
	int iVictoriesSet;
	// PatchMod: Victorys END
	DllExport void init(HandicapTypes eHandicap);
	DllExport void reset(HandicapTypes eHandicap = NO_HANDICAP, bool bConstructorCall = false);

protected:

	void uninit();

public:

	DllExport void setInitialItems(bool bScenario);
	DllExport void regenerateMap();

	DllExport void initDiplomacy();
	void initFreeState();
	void initFreeUnits();
	void initImmigration();

	void assignStartingPlots();
	void normalizeStartingPlots();

	void assignNativeTerritory();

	DllExport void update();
	void updateScore(bool bForce = false);

	DllExport void updateColoredPlots();

	void updateCitySight(bool bIncrement);

	DllExport void updateSelectionList();
	DllExport void updateTestEndTurn();

	DllExport void testExtendedGame();

	DllExport CvUnit* getPlotUnit(const CvPlot* pPlot, int iIndex);
	DllExport void getPlotUnits(const CvPlot *pPlot, std::vector<CvUnit *> &plotUnits);
	DllExport void cycleCities(bool bForward = true, bool bAdd = false);
	void cycleSelectionGroups(bool bClear, bool bForward = true);
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

	bool canDoControl(ControlTypes eControl);
	DllExport void doControl(ControlTypes eControl);

	DllExport void implementDeal(PlayerTypes eWho, PlayerTypes eOtherWho, CLinkList<TradeData>* pOurList, CLinkList<TradeData>* pTheirList, bool bForce = false);
	void verifyDeals();

	DllExport void getGlobeviewConfigurationParameters(TeamTypes eTeam, bool& bStarsVisible, bool& bWorldIsRound);

	int getProductionPerPopulation(HurryTypes eHurry);

	int getAdjustedPopulationPercent(VictoryTypes eVictory) const;
	int getAdjustedLandPercent(VictoryTypes eVictory) const;

	int countCivPlayersAlive() const;
	int countCivPlayersEverAlive() const;
	int countCivTeamsAlive() const;
	int countCivTeamsEverAlive() const;
	int countHumanPlayersAlive() const;
	int countHumanPlayersEverAlive() const;

	static int countCivPlayerEuropeanAI();

	int countTotalCivPower();

	int getImprovementUpgradeTime(ImprovementTypes eImprovement) const;

	EraTypes getCurrentEra() const;

	DllExport TeamTypes getActiveTeam();
	DllExport CivilizationTypes getActiveCivilizationType();

	unsigned int getLastEndTurnMessageSentTime();
	DllExport bool isNetworkMultiPlayer() const;
	DllExport bool isGameMultiPlayer() const;
	DllExport bool isTeamGame() const;

	bool isModem();
	void setModem(bool bModem);
	DllExport void reviveActivePlayer();

	DllExport int getNumHumanPlayers();

	int getEndTurnMessagesSent();
	DllExport void incrementEndTurnMessagesSent();

	DllExport int getGameTurn();
	void setGameTurn(int iNewValue);
	void incrementGameTurn();
	int getTurnYear(int iGameTurn);
	int getGameTurnYear();

	int getElapsedGameTurns() const;
	void incrementElapsedGameTurns();
	bool isMaxTurnsExtended() const;
	void setMaxTurnsExtended(bool bExtended);
	int getMaxTurns() const;
	void setMaxTurns(int iNewValue);
	void changeMaxTurns(int iChange);

	DllExport void getTurnTimerText(CvWString& szBuffer) const;

	int getMaxCityElimination() const;
	void setMaxCityElimination(int iNewValue);

	int getNumAdvancedStartPoints() const;
	void setNumAdvancedStartPoints(int iNewValue);

	int getStartTurn() const;
	void setStartTurn(int iNewValue);
	int getStartYear() const;
	void setStartYear(int iNewValue);

	int getEstimateEndTurn() const;
	void setEstimateEndTurn(int iNewValue);

	DllExport int getTurnSlice() const;
	int getMinutesPlayed() const;
	void setTurnSlice(int iNewValue);
	void changeTurnSlice(int iChange);

	int getCutoffSlice() const;
	void setCutoffSlice(int iNewValue);
	void changeCutoffSlice(int iChange);

	DllExport int getTurnSlicesRemaining() const;
	void resetTurnTimer();
	void incrementTurnTimer(int iNumTurnSlices);
	TurnTimerTypes getTurnTimerType() const;
	int getMaxTurnLen();
	int getTargetScore() const;

	int getNumGameTurnActive();
	DllExport int countNumHumanGameTurnActive();
	void changeNumGameTurnActive(int iChange);
	int getNumCities() const;
	void changeNumCities(int iChange);
	int getTotalPopulation() const;
	int getMaxPopulation() const;
	int getMaxLand() const;
	int getMaxFather() const;
	int getInitPopulation() const;
	int getInitLand() const;
	int getInitFather() const;
	DllExport void initScoreCalculation();
	int getAIAutoPlay();
	DllExport void setAIAutoPlay(int iNewValue);
	void changeAIAutoPlay(int iChange);
	unsigned int getInitialTime();
	DllExport void setInitialTime(unsigned int uiNewValue);
	bool isScoreDirty() const;
	void setScoreDirty(bool bNewValue);

	DllExport bool isDebugMode() const;
	DllExport void toggleDebugMode();
	DllExport void updateDebugModeCache();
	int getPitbossTurnTime() const;
	void setPitbossTurnTime(int iHours);

	DllExport bool isHotSeat() const;
	DllExport bool isPbem() const;
	DllExport bool isPitboss() const;
	DllExport bool isSimultaneousTeamTurns() const;

	DllExport bool isFinalInitialized() const;
	DllExport void setFinalInitialized(bool bNewValue);

	bool getPbemTurnSent() const;
	DllExport void setPbemTurnSent(bool bNewValue);

	DllExport bool getHotPbemBetweenTurns() const;
	void setHotPbemBetweenTurns(bool bNewValue);

	bool isPlayerOptionsSent() const;
	void sendPlayerOptions(bool bForce = false);
	DllExport PlayerTypes getActivePlayer() const;
	DllExport void setActivePlayer(PlayerTypes eNewValue, bool bForceHotSeat = false);
	DllExport void updateUnitEnemyGlow();

	HandicapTypes getHandicapType() const;
	void setHandicapType(HandicapTypes eHandicap);
	DllExport PlayerTypes getPausePlayer();
	DllExport bool isPaused();
	DllExport void setPausePlayer(PlayerTypes eNewValue);
	DllExport int getBestLandUnitCombat();
	void setBestLandUnitCombat(int iNewValue);
	DllExport TeamTypes getWinner() const;
	DllExport VictoryTypes getVictory() const;
	void setWinner(TeamTypes eNewWinner, VictoryTypes eNewVictory);

	DllExport GameStateTypes getGameState() const;
	DllExport void setGameState(GameStateTypes eNewValue);
	DllExport EraTypes getStartEra() const;

	CalendarTypes getCalendar() const;

	GameSpeedTypes getGameSpeedType() const;
	int getCultureLevelThreshold(CultureLevelTypes eCultureLevel, PlayerTypes ePlayer) const;
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
	bool isOption(GameOptionTypes eIndex) const;
	void setOption(GameOptionTypes eIndex, bool bEnabled);
	DllExport bool isMPOption(MultiplayerOptionTypes eIndex) const;
	void setMPOption(MultiplayerOptionTypes eIndex, bool bEnabled);
	bool isForcedControl(ForceControlTypes eIndex) const;
	void setForceControl(ForceControlTypes eIndex, bool bEnabled);
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

	bool isInAdvancedStart() const;

	DllExport const CvWString & getName();
	void setName(char const* szName);

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
	int getNumDeals();
	DllExport CvDeal* getDeal(int iID);
	CvDeal* addDeal();
	void deleteDeal(int iID);
	// iteration
	CvDeal* firstDeal(int *pIterIdx, bool bRev=false);
	CvDeal* nextDeal(int *pIterIdx, bool bRev=false);

	CvRandom& getMapRand();
	const CvRandom& getMapRand() const;
	int getMapRandNum(int iNum, const char* pszLog);

	CvRandom& getSorenRand();
	const CvRandom& getSorenRand() const;
	int getSorenRandNum(int iNum, const char* pszLog);

	DllExport int calculateSyncChecksum(CvString* pLogString);
	DllExport int calculateOptionsChecksum();

	void addReplayMessage(ReplayMessageTypes eType = NO_REPLAY_MESSAGE, PlayerTypes ePlayer = NO_PLAYER, CvWString pszText = L"",
		int iPlotX = -1, int iPlotY = -1, ColorTypes eColor = NO_COLOR)
		{
			addReplayMessage(eType, ePlayer, pszText, Coordinates(iPlotX, iPlotY), eColor);
		}

	void addReplayMessage(ReplayMessageTypes eType, PlayerTypes ePlayer, CvWString pszText,
		const Coordinates coord, ColorTypes eColor = NO_COLOR);
	void clearReplayMessageMap();
	int getReplayMessageTurn(uint i) const;
	ReplayMessageTypes getReplayMessageType(uint i) const;
	int getReplayMessagePlotX(uint i) const;
	int getReplayMessagePlotY(uint i) const;
	PlayerTypes getReplayMessagePlayer(uint i) const;
	LPCWSTR getReplayMessageText(uint i) const;
	uint getNumReplayMessages() const;
	ColorTypes getReplayMessageColor(uint i) const;

	virtual void read(FDataStreamBase* pStream);
	virtual void write(FDataStreamBase* pStream);
	virtual void writeReplay(FDataStreamBase& stream, PlayerTypes ePlayer);

	void read(CvSavegameReader reader);
	void write(CvSavegameWriter writer);
	void resetSavedData(HandicapTypes eHandicap, bool bConstructorCall);

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
	void setFatherTeam(AssertCallerData assertData, FatherTypes eFather, TeamTypes eTeam);
	bool getRemainingFathers(FatherPointTypes ePointType, std::vector<FatherTypes>& aFathers);
	int getFatherCategoryPosition(FatherTypes eFather) const;

	void changeYieldBoughtTotal(PlayerTypes eMainEurope, YieldTypes eYield, int iChange) const;
	void changeYieldBoughtTotalAfrica(PlayerTypes eMainEurope, YieldTypes eYield, int iChange) const; // WTP, ray, Yields Traded Total for Africa and Port Royal - START
	void changeYieldBoughtTotalPortRoyal(PlayerTypes eMainEurope, YieldTypes eYield, int iChange) const; // WTP, ray, Yields Traded Total for Africa and Port Royal - START

	// < JAnimals Mod Start >
	PlayerTypes getBarbarianPlayer();
    bool hasBarbarianPlayer();
    void setBarbarianPlayer(PlayerTypes eNewValue);
    bool isBarbarianPlayer(PlayerTypes ePlayer);
	PlayerTypes getNextPlayerType() const;
	// < JAnimals Mod End >

	// R&R, ray, the Church - START
	PlayerTypes getChurchPlayer();
    bool hasChurchPlayer();
    void setChurchPlayer(PlayerTypes eNewValue);
    bool isChurchPlayer(PlayerTypes ePlayer);
	// R&R, ray, the Church - END

	// R&R, ray, Correct Geographical Placement of Natives - START
	bool isWBNorthAmericanNative() const;
	void setWBNorthAmericanNative(bool bValue);
	bool isWBSouthAmericanNative() const;
	void setWBSouthAmericanNative(bool bValue);
	bool isWBCentralAmericanNative() const;
	void setWBCentralAmericanNative(bool bValue);
	// R&R, ray, Correct Geographical Placement of Natives - END

	void writeDesyncLog();

	int getRemainingForcedPeaceTurns() const;

	static const int PLOT_OCEAN_DISTANCE_IMPASSABLE_THRESHOLD = 1000;

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

	EnumMap<PlayerTypes, int> m_em_iRankPlayer;        // Ordered by rank...
	EnumMap<PlayerTypes, int> m_em_iPlayerScore;       // Ordered by player ID...
	EnumMap<TeamTypes, int> m_em_iRankTeam;						// Ordered by rank...
	EnumMap<TeamTypes, int> m_em_iTeamRank;						// Ordered by team ID...
	EnumMap<TeamTypes, int> m_em_iTeamScore;						// Ordered by team ID...

	EnumMap<UnitTypes, int> m_em_iUnitCreatedCount;
	EnumMap<UnitClassTypes, int> m_em_iUnitClassCreatedCount;
	EnumMap<BuildingClassTypes, int> m_em_iBuildingClassCreatedCount;

	EnumMap<FatherTypes, TeamTypes> m_em_eFatherTeam;
	EnumMap<FatherTypes, int, -1> m_em_iFatherGameTurn;

	EnumMap<SpecialUnitTypes, bool> m_em_bSpecialUnitValid;
	EnumMap<SpecialBuildingTypes, bool> m_em_bSpecialBuildingValid;

	EnumMap<GoodyTypes, bool> m_em_bUniqueGoodyValid; // R&R, ray, Goody Enhancement

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

	int m_iUnitUpdateAttempts; // advc.001y

	void doTurn();
	void doFoundingFathers();
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
	CvPlot* getAnyEuropePlot() const;
};

// <advc.003s> For generating variable names. (The layer of indirection is necessary.)
#define CONCATVARNAME_IMPL(prefix, suffix) prefix##suffix
#define CONCATVARNAME(prefix, suffix) CONCATVARNAME_IMPL(prefix, suffix) // </advc.003s>
// <advc.007c> For debug output
#define STRINGIFY_HELPER2(x) #x
#define STRINGIFY_HELPER1(x) STRINGIFY_HELPER2(x)
// (__FILE__ prints some path info; that gets too verbose.)
#define CALL_LOC_STR __FUNCTION__ /*"(" __FILE__ ")"*/ "@L" STRINGIFY_HELPER1(__LINE__)
// </advc.007c>

// Implementation files can re-define this to use a different CvGame instance
#define CVGAME_INSTANCE_FOR_RNG GC.getGame()
/*
inline CvRandom& syncRand()
{
	return CVGAME_INSTANCE_FOR_RNG.getSorenRand();
}
*/
// These have to be macros to let CALL_LOC_STR expand to the proper code location
#define SyncRandNum(iNumOutcomes) \
	CVGAME_INSTANCE_FOR_RNG.getSorenRandNum((iNumOutcomes), CALL_LOC_STR)

#endif
