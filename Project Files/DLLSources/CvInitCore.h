#pragma once

// game.h

#ifndef CIV4_INITCORE_H
#define CIV4_INITCORE_H

//#include "CvEnums.h"

#define FASSERT_BOUNDS(lower,upper,index,fnString)\
	if (index < lower)\
	{\
		char acOut[256];\
		sprintf(acOut, "Index in %s expected to be >= %d", fnString, lower);\
		FAssertMsg(index >= lower, acOut);\
	}\
	else if (index >= upper)\
	{\
		char acOut[256];\
		sprintf(acOut, "Index in %s expected to be < %d", fnString, upper);\
		FAssertMsg(index < upper, acOut);\
	}

class CvInitCore
{

public:
	CvInitCore();
	virtual ~CvInitCore();

	DllExport void init(GameMode eMode);

protected:

	void uninit();
	void reset(GameMode eMode);

	void setDefaults();

	bool checkBounds( int iValue, int iLower, int iUpper ) const;

public:

	// **************************
	// Applications of data
	// **************************
	DllExport bool getHuman(PlayerTypes eID) const;
	int getNumHumans() const;

	DllExport int getNumDefinedPlayers() const;

	DllExport bool getMultiplayer() const;
	DllExport bool getNewGame() const;
	DllExport bool getSavedGame() const;
	DllExport bool getScenario() const;
	DllExport bool getGameMultiplayer() const { return (getMultiplayer() || getPitboss() || getPbem() || getHotseat()); }

	DllExport bool getPitboss() const;
	DllExport bool getHotseat() const;
	DllExport bool getPbem() const;

	DllExport bool getSlotVacant(PlayerTypes eID) const;
	DllExport PlayerTypes getAvailableSlot();
	DllExport void reassignPlayer(PlayerTypes eOldID, PlayerTypes eNewID);

	DllExport void closeInactiveSlots();
	DllExport void reopenInactiveSlots();

	void resetGame();
	DllExport void resetGame(CvInitCore * pSource, bool bClear = true, bool bSaveGameType = false);
	DllExport void resetPlayers();
	DllExport void resetPlayers(CvInitCore * pSource, bool bClear = true, bool bSaveSlotInfo = false);
	void resetPlayer(PlayerTypes eID);
	DllExport void resetPlayer(PlayerTypes eID, CvInitCore * pSource, bool bClear = true, bool bSaveSlotInfo = false);

	// **************************
	// Member access
	// **************************
	const CvWString & getGameName() const	{ return m_szGameName; }
	DllExport void setGameName(const CvWString & szGameName)	{ m_szGameName = szGameName; }

	const CvWString & getGamePassword() const	{ return m_szGamePassword; }
	DllExport void setGamePassword(const CvWString & szGamePassword)	{ m_szGamePassword = szGamePassword; }

	const CvWString & getAdminPassword() const	{ return m_szAdminPassword; }
	DllExport void setAdminPassword(const CvWString & szAdminPassword, bool bEncrypt = true);

	DllExport CvWString getMapScriptName() const;
	DllExport void setMapScriptName(const CvWString & szMapScriptName);
	DllExport bool getWBMapScript() const;

	bool getWBMapNoPlayers() const { return m_bWBMapNoPlayers; }
	void setWBMapNoPlayers(bool bValue)	{ m_bWBMapNoPlayers = bValue; }

	WorldSizeTypes getWorldSize() const	{ return m_eWorldSize; }
	void setWorldSize(WorldSizeTypes eWorldSize)	{ m_eWorldSize = eWorldSize; }
	DllExport void setWorldSize(const CvWString & szWorldSize);
	DllExport const CvWString & getWorldSizeKey(CvWString & szBuffer) const;

	ClimateTypes getClimate() const	{ return m_eClimate; }
	void setClimate(ClimateTypes eClimate)	{ m_eClimate = eClimate; }
	DllExport void setClimate(const CvWString & szClimate);
	DllExport const CvWString & getClimateKey(CvWString & szBuffer) const;

	SeaLevelTypes getSeaLevel() const	{ return m_eSeaLevel; }
	void setSeaLevel(SeaLevelTypes eSeaLevel)	{ m_eSeaLevel = eSeaLevel; }
	DllExport void setSeaLevel(const CvWString & szSeaLevel);
	DllExport const CvWString & getSeaLevelKey(CvWString & szBuffer) const;

	EraTypes getEra() const	{ return m_eEra; }
	void setEra(EraTypes eEra)	{ m_eEra = eEra; }
	DllExport void setEra(const CvWString & szEra);
	DllExport const CvWString & getEraKey(CvWString & szBuffer) const;

	GameSpeedTypes getGameSpeed() const	{ return m_eGameSpeed; }
	void setGameSpeed(GameSpeedTypes eGameSpeed)	{ m_eGameSpeed = eGameSpeed; }
	DllExport void setGameSpeed(const CvWString & szGameSpeed);
	DllExport const CvWString & getGameSpeedKey(CvWString & szBuffer) const;

	TurnTimerTypes getTurnTimer() const	{ return m_eTurnTimer; }
	void setTurnTimer(TurnTimerTypes eTurnTimer)	{ m_eTurnTimer = eTurnTimer; }
	void setTurnTimer(const CvWString & szTurnTimer);
	const CvWString & getTurnTimerKey(CvWString & szBuffer) const;

	CalendarTypes getCalendar() const	{ return m_eCalendar; }
	void setCalendar(CalendarTypes eCalendar)	{ m_eCalendar = eCalendar; }
	void setCalendar(const CvWString & szCalendar);
	const CvWString & getCalendarKey(CvWString & szBuffer) const;


	int getNumCustomMapOptions() const	{ return m_iNumCustomMapOptions; }
	int getNumHiddenCustomMapOptions() const	{ return m_iNumHiddenCustomMapOptions; }

	DllExport const CustomMapOptionTypes * getCustomMapOptions() const	{ return m_aeCustomMapOptions; }
	DllExport void setCustomMapOptions(int iNumCustomMapOptions, const CustomMapOptionTypes * aeCustomMapOptions);

	DllExport CustomMapOptionTypes getCustomMapOption(int iOptionID) const;
	DllExport void setCustomMapOption(int iOptionID, CustomMapOptionTypes eCustomMapOption);


	DllExport int getNumVictories() const	{ return m_iNumVictories; }

	DllExport const bool * getVictories() const	{ return m_abVictories; }
	DllExport void setVictories(int iNumVictories, const bool * abVictories);

	DllExport bool getVictory(VictoryTypes eVictoryID) const;
	DllExport void setVictory(VictoryTypes eVictoryID, bool bVictory);


	const bool * getOptions() const	{ return m_abOptions; }
	DllExport bool getOption(GameOptionTypes eIndex) const;
	DllExport void setOption(GameOptionTypes eIndex, bool bOption);

	const bool * getMPOptions() const	{ return m_abMPOptions; }
	DllExport bool getMPOption(MultiplayerOptionTypes eIndex) const;
	DllExport void setMPOption(MultiplayerOptionTypes eIndex, bool bMPOption);

	bool getStatReporting() const	{ return m_bStatReporting; }
	void setStatReporting(bool bStatReporting)	{ m_bStatReporting = bStatReporting; }

	const bool * getForceControls() const	{ return m_abForceControls; }
	DllExport bool getForceControl(ForceControlTypes eIndex) const;
	DllExport void setForceControl(ForceControlTypes eIndex, bool bForceControl);


	int getGameTurn() const	{ return m_iGameTurn; }
	void setGameTurn(int iGameTurn)	{ m_iGameTurn = iGameTurn; }

	int getMaxTurns() const	{ return m_iMaxTurns; }
	void setMaxTurns(int iMaxTurns)	{ m_iMaxTurns = iMaxTurns; }

	DllExport int getPitbossTurnTime() const	{ return m_iPitbossTurnTime; }
	DllExport void setPitbossTurnTime(int iPitbossTurnTime)	{ m_iPitbossTurnTime = iPitbossTurnTime; }

	int getTargetScore() const	{ return m_iTargetScore; }
	void setTargetScore(int iTargetScore)	{ m_iTargetScore = iTargetScore; }


	int getMaxCityElimination() const	{ return m_iMaxCityElimination; }
	void setMaxCityElimination(int iMaxCityElimination)	{ m_iMaxCityElimination = iMaxCityElimination; }

	int getNumAdvancedStartPoints() const	{ return m_iNumAdvancedStartPoints; }
	void setNumAdvancedStartPoints(int iNumPoints)	{ m_iNumAdvancedStartPoints = iNumPoints; }

	DllExport unsigned int getSyncRandSeed() const	{ return m_uiSyncRandSeed; }
	DllExport void setSyncRandSeed(unsigned int uiSyncRandSeed)	{ m_uiSyncRandSeed = uiSyncRandSeed; }

	DllExport unsigned int getMapRandSeed() const	{ return m_uiMapRandSeed; }
	DllExport void setMapRandSeed(unsigned int uiMapRandSeed)	{ m_uiMapRandSeed = uiMapRandSeed; }

	PlayerTypes getActivePlayer() const	{ return m_eActivePlayer; }
	DllExport void setActivePlayer(PlayerTypes eActivePlayer);

	DllExport GameType getType() const	{ return m_eType; }
	DllExport void setType(GameType eType);
	DllExport void setType(const CvWString & szType);

	GameMode getMode() const	{ return m_eMode; }
	DllExport void setMode(GameMode eMode);


	DllExport const CvWString & getLeaderName(PlayerTypes eID, uint uiForm = 0) const;
	DllExport void setLeaderName(PlayerTypes eID, const CvWString & szLeaderName);
	DllExport const CvWString & getLeaderNameKey(PlayerTypes eID) const;

	DllExport const CvWString & getCivDescription(PlayerTypes eID, uint uiForm = 0) const;
	DllExport void setCivDescription(PlayerTypes eID, const CvWString & szCivDescription);
	DllExport const CvWString & getCivDescriptionKey(PlayerTypes eID) const;

	DllExport const CvWString & getCivShortDesc(PlayerTypes eID, uint uiForm = 0) const;
	DllExport void setCivShortDesc(PlayerTypes eID, const CvWString & szCivShortDesc);
	DllExport const CvWString & getCivShortDescKey(PlayerTypes eID) const;

	DllExport const CvWString & getCivAdjective(PlayerTypes eID, uint uiForm = 0) const;
	DllExport void setCivAdjective(PlayerTypes eID, const CvWString & szCivAdjective);
	DllExport const CvWString & getCivAdjectiveKey(PlayerTypes eID) const;

	DllExport const CvWString & getCivPassword(PlayerTypes eID) const;
	DllExport void setCivPassword(PlayerTypes eID, const CvWString & szCivPassword, bool bEncrypt = true);

	DllExport const CvString & getEmail(PlayerTypes eID) const;
	DllExport void setEmail(PlayerTypes eID, const CvString & szEmail);

	DllExport const CvString & getSmtpHost(PlayerTypes eID) const;
	DllExport void setSmtpHost(PlayerTypes eID, const CvString & szHost);

	DllExport CivilizationTypes getCiv(PlayerTypes eID) const;
	DllExport void setCiv(PlayerTypes eID, CivilizationTypes eCiv);

	DllExport LeaderHeadTypes getLeader(PlayerTypes eID) const;
	DllExport void setLeader(PlayerTypes eID, LeaderHeadTypes eLeader);

	DllExport TeamTypes getTeam(PlayerTypes eID) const;
	DllExport void setTeam(PlayerTypes eID, TeamTypes eTeam);

	DllExport HandicapTypes getHandicap(PlayerTypes eID) const;
	DllExport void setHandicap(PlayerTypes eID, HandicapTypes eHandicap);

	DllExport PlayerColorTypes getColor(PlayerTypes eID) const;
	DllExport void setColor(PlayerTypes eID, PlayerColorTypes eColor);

	DllExport ArtStyleTypes getArtStyle(PlayerTypes eID) const;
	DllExport void setArtStyle(PlayerTypes eID, ArtStyleTypes eArtStyle);


	DllExport SlotStatus getSlotStatus(PlayerTypes eID) const;
	DllExport void setSlotStatus(PlayerTypes eID, SlotStatus eSlotStatus);

	DllExport SlotClaim getSlotClaim(PlayerTypes eID) const;
	DllExport void setSlotClaim(PlayerTypes eID, SlotClaim eSlotClaim);


	DllExport bool getPlayableCiv(PlayerTypes eID) const;
	DllExport void setPlayableCiv(PlayerTypes eID, bool bPlayableCiv);

	DllExport bool getMinorNationCiv(PlayerTypes eID) const;
	DllExport void setMinorNationCiv(PlayerTypes eID, bool bMinorNationCiv);


	DllExport int getNetID(PlayerTypes eID) const;
	DllExport void setNetID(PlayerTypes eID, int iNetID);

	DllExport bool getReady(PlayerTypes eID) const;
	DllExport void setReady(PlayerTypes eID, bool bReady);

	DllExport const CvString & getPythonCheck(PlayerTypes eID) const;
	DllExport void setPythonCheck(PlayerTypes eID, const CvString & iPythonCheck);

	DllExport const CvString & getXMLCheck(PlayerTypes eID) const;
	DllExport void setXMLCheck(PlayerTypes eID, const CvString & iXMLCheck);

	DllExport void resetAdvancedStartPoints();

	DllExport int getMaxEuropePlayers() const;

	virtual void read(FDataStreamBase* pStream);
	virtual void write(FDataStreamBase* pStream);

protected:

	int read(CvSavegameReader& reader, JITarrayTypes eType, bool*& pArray, bool bAllocate);
	void write(CvSavegameWriter& writer, JITarrayTypes eType, bool* pArray, int iLength);

	void clearCustomMapOptions();
	void refreshCustomMapOptions();

	void clearVictories();
	void refreshVictories();

	// ***
	// CORE GAME INIT DATA
	// ***

	// Game type
	GameType m_eType;

	// Descriptive strings about game and map
	CvWString m_szGameName;
	CvWString m_szGamePassword;
	CvWString m_szAdminPassword;
	CvWString m_szMapScriptName;

	bool m_bWBMapNoPlayers;

	// Standard game parameters
	WorldSizeTypes m_eWorldSize;
	ClimateTypes m_eClimate;
	SeaLevelTypes m_eSeaLevel;
	EraTypes m_eEra;
	GameSpeedTypes m_eGameSpeed;
	TurnTimerTypes m_eTurnTimer;
	CalendarTypes m_eCalendar;

	// Map-specific custom parameters
	int m_iNumCustomMapOptions;
	int m_iNumHiddenCustomMapOptions;
	CustomMapOptionTypes * m_aeCustomMapOptions;

	// Standard game options
	bool* m_abOptions;
	bool* m_abMPOptions;
	bool m_bStatReporting;

	bool* m_abForceControls;

	// Dynamic victory condition setting
	int m_iNumVictories;
	bool * m_abVictories;

	// Game turn mgmt
	int m_iGameTurn;
	int m_iMaxTurns;
	int m_iPitbossTurnTime;
	int m_iTargetScore;

	// Number of city eliminations permitted
	int m_iMaxCityElimination;

	int m_iNumAdvancedStartPoints;

	// Unsaved data
	unsigned int m_uiSyncRandSeed;
	unsigned int m_uiMapRandSeed;
	PlayerTypes m_eActivePlayer;
	GameMode m_eMode;

	// Temp var so we don't return locally scoped var
	mutable CvWString m_szTemp;
	mutable CvString m_szTempA;


	// ***
	// CORE PLAYER INIT DATA
	// ***

	// Civ details
	CvWString* m_aszLeaderName;
	CvWString* m_aszCivDescription;
	CvWString* m_aszCivShortDesc;
	CvWString* m_aszCivAdjective;
	CvWString* m_aszCivPassword;
	CvString* m_aszEmail;
	CvString* m_aszSmtpHost;

	CivilizationTypes* m_aeCiv;
	LeaderHeadTypes* m_aeLeader;
	TeamTypes* m_aeTeam;
	HandicapTypes* m_aeHandicap;
	PlayerColorTypes* m_aeColor;
	ArtStyleTypes* m_aeArtStyle;

	// Slot data
	SlotStatus* m_aeSlotStatus;
	SlotClaim* m_aeSlotClaim;

	// Civ flags
	bool* m_abPlayableCiv;
	bool* m_abMinorNationCiv;

	// Unsaved player data
	int* m_aiNetID;
	bool* m_abReady;

	CvString* m_aszPythonCheck;
	CvString* m_aszXMLCheck;
	mutable CvString m_szTempCheck;
};

#endif
