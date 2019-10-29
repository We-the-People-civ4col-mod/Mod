#include "CvGameCoreDLL.h"
#include "CvInitCore.h"
#include "CvDLLInterfaceIFaceBase.h"

#include "CvSavegame.h"

// set the default values
const int defaultEndTurnMessagesSent = 0;
const int defaultElapsedGameTurns = 0;
const int defaultStartTurn = 0;
const int defaultStartYear = 0;
const int defaultEstimateEndTurn = 0;
const int defaultTurnSlice = 0;
const int defaultCutoffSlice = 0;
const int defaultNumGameTurnActive = 0;
const int defaultNumCities = 0;
const int defaultMaxPopulation = 0;
const int defaultMaxLand = 0;
const int defaultMaxFather = 0;
const int defaultInitPopulation = 0;
const int defaultInitLand = 0;
const int defaultInitFather = 0;
const int defaultAIAutoPlay = 0;
const int defaultBestLandUnitCombat = 1;
const bool defaultScoreDirty = false;
const bool defaultFinalInitialized = false;
const bool defaultHotPbemBetweenTurns = false;
const bool defaultMaxTurnsExtended = false;
const bool defaultWBNorthAmericanNative = true;
const bool defaultWBSouthAmericanNative = true;
const bool defaultWBCentralAmericanNative = true;
const HandicapTypes defaultHandicap = NO_HANDICAP;
const PlayerTypes defaultPausePlayer = NO_PLAYER;
const PlayerTypes defaultBarbarianPlayer = NO_PLAYER;
const PlayerTypes defaultChurchPlayer = NO_PLAYER;
const TeamTypes defaultWinner = NO_TEAM;
const VictoryTypes defaultVictory = NO_VICTORY;
const GameStateTypes defaultGameState = GAMESTATE_ON;

const int defaultNumSessions = 1;
const int defaultNumCultureVictoryCities = 0;
const int defaultCultureVictoryCultureLevel = NO_CULTURELEVEL;

// 
enum SavegameVariableTypes
{
	GameSave_END,

	GameSave_EndTurnMessagesSent,
	GameSave_ElapsedGameTurns,
	GameSave_StartTurn,
	GameSave_StartYear,
	GameSave_EstimateEndTurn,
	GameSave_TurnSlice,
	GameSave_CutoffSlice,
	GameSave_NumGameTurnActive,
	GameSave_NumCities,
	GameSave_MaxPopulation,
	GameSave_MaxLand,
	GameSave_MaxFather,
	GameSave_InitPopulation,
	GameSave_InitLand,
	GameSave_InitFather,
	GameSave_AIAutoPlay,
	GameSave_BestLandUnitCombat,
	GameSave_ScoreDirty,
	GameSave_FinalInitialized,
	GameSave_HotPbemBetweenTurns,
	GameSave_MaxTurnsExtended,
	GameSave_WBNorthAmericanNative,
	GameSave_WBSouthAmericanNative,
	GameSave_WBCentralAmericanNative,
	GameSave_Handicap,
	GameSave_PausePlayer,
	GameSave_BarbarianPlayer,
	GameSave_ChurchPlayer,
	GameSave_Winner,
	GameSave_Victory,
	GameSave_GameState,
	GameSave_ScriptData,

	GameSave_RankPlayer,
	GameSave_PlayerScore,
	GameSave_RankTeam,
	GameSave_TeamRank,
	GameSave_TeamScore,
	GameSave_UnitCreatedCount,
	GameSave_UnitClassCreatedCount,
	GameSave_BuildingClassCreatedCount,
	GameSave_FatherTeam,
	GameSave_FatherGameTurn,
	GameSave_SpecialUnitValid,
	GameSave_SpecialBuildingValid,
	GameSave_UniqueGoodyValid,

	GameSave_DestroyedCities,
	GameSave_GreatGeneralBorn,
	GameSave_GreatAdmiralBorn,
	GameSave_ShipNamed,
	GameSave_deals,
	GameSave_mapRand,
	GameSave_sorenRand,
	GameSave_listReplayMessages,
	GameSave_NumSessions,
	GameSave_PlotExtraYields,
	GameSave_InactiveTriggers,
	GameSave_NumCultureVictoryCities,
	GameSave_CultureVictoryCultureLevel,

	NUM_SAVE_ENUM_VALUES,
};

const char* getSavedEnumNameGame(SavegameVariableTypes eType)
{
	switch (eType)
	{
	case GameSave_END: return "GameSave_END";
	case GameSave_EndTurnMessagesSent: return "GameSave_EndTurnMessagesSent";
	case GameSave_ElapsedGameTurns: return "GameSave_ElapsedGameTurns";
	case GameSave_StartTurn: return "GameSave_StartTurn";
	case GameSave_StartYear: return "GameSave_StartYear";
	case GameSave_EstimateEndTurn: return "GameSave_EstimateEndTurn";
	case GameSave_TurnSlice: return "GameSave_TurnSlice";
	case GameSave_CutoffSlice: return "GameSave_CutoffSlice";
	case GameSave_NumGameTurnActive: return "GameSave_NumGameTurnActive";
	case GameSave_NumCities: return "GameSave_NumCities";
	case GameSave_MaxPopulation: return "GameSave_MaxPopulation";
	case GameSave_MaxLand: return "GameSave_MaxLand";
	case GameSave_MaxFather: return "GameSave_MaxFather";
	case GameSave_InitPopulation: return "GameSave_InitPopulation";
	case GameSave_InitLand: return "GameSave_InitLand";
	case GameSave_InitFather: return "GameSave_InitFather";
	case GameSave_AIAutoPlay: return "GameSave_AIAutoPlay";
	case GameSave_BestLandUnitCombat: return "GameSave_BestLandUnitCombat";
	case GameSave_ScoreDirty: return "GameSave_ScoreDirty";
	case GameSave_FinalInitialized: return "GameSave_FinalInitialized";
	case GameSave_HotPbemBetweenTurns: return "GameSave_HotPbemBetweenTurns";
	case GameSave_MaxTurnsExtended: return "GameSave_MaxTurnsExtended";
	case GameSave_WBNorthAmericanNative: return "GameSave_WBNorthAmericanNative";
	case GameSave_WBSouthAmericanNative: return "GameSave_WBSouthAmericanNative";
	case GameSave_WBCentralAmericanNative: return "GameSave_WBCentralAmericanNative";
	case GameSave_Handicap: return "GameSave_Handicap";
	case GameSave_PausePlayer: return "GameSave_PausePlayer";
	case GameSave_BarbarianPlayer: return "GameSave_BarbarianPlayer";
	case GameSave_ChurchPlayer: return "GameSave_ChurchPlayer";
	case GameSave_Winner: return "GameSave_Winner";
	case GameSave_Victory: return "GameSave_Victory";
	case GameSave_GameState: return "GameSave_GameState";
	case GameSave_ScriptData: return "GameSave_ScriptData";

	case GameSave_RankPlayer: return "GameSave_RankPlayer";
	case GameSave_PlayerScore: return "GameSave_PlayerScore";
	case GameSave_RankTeam: return "GameSave_RankTeam";
	case GameSave_TeamRank: return "GameSave_TeamRank";
	case GameSave_TeamScore: return "GameSave_TeamScore";
	case GameSave_UnitCreatedCount: return "GameSave_UnitCreatedCount";
	case GameSave_UnitClassCreatedCount: return "GameSave_UnitClassCreatedCount";
	case GameSave_BuildingClassCreatedCount: return "GameSave_BuildingClassCreatedCount";
	case GameSave_FatherTeam: return "GameSave_FatherTeam";
	case GameSave_FatherGameTurn: return "GameSave_FatherGameTurn";
	case GameSave_SpecialUnitValid: return "GameSave_SpecialUnitValid";
	case GameSave_SpecialBuildingValid: return "GameSave_SpecialBuildingValid";
	case GameSave_UniqueGoodyValid: return "GameSave_UniqueGoodyValid";

	case GameSave_DestroyedCities: return "GameSave_DestroyedCities";
	case GameSave_GreatGeneralBorn: return "GameSave_GreatGeneralBorn";
	case GameSave_GreatAdmiralBorn: return "GameSave_GreatAdmiralBorn";
	case GameSave_ShipNamed: return "GameSave_ShipNamed";
	case GameSave_deals: return "GameSave_deals";
	case GameSave_mapRand: return "GameSave_mapRand";
	case GameSave_sorenRand: return "GameSave_sorenRand";
	case GameSave_listReplayMessages: return "GameSave_listReplayMessages";
	case GameSave_NumSessions: return "GameSave_NumSessions";
	case GameSave_PlotExtraYields: return "GameSave_PlotExtraYields";
	case GameSave_InactiveTriggers: return "GameSave_InactiveTriggers";
	case GameSave_NumCultureVictoryCities: return "GameSave_NumCultureVictoryCities";
	case GameSave_CultureVictoryCultureLevel: return "GameSave_CultureVictoryCultureLevel";

	}
	return "";
}

int getNumSavedEnumValuesGame()
{
	return NUM_SAVE_ENUM_VALUES;
}

// assign everything to default values
void CvGame::resetSavedData(HandicapTypes eHandicap, bool bConstructorCall)
{
	m_iEndTurnMessagesSent = defaultEndTurnMessagesSent;
	m_iElapsedGameTurns = defaultElapsedGameTurns;
	m_iStartTurn = defaultStartTurn;
	m_iStartYear = defaultStartYear;
	m_iEstimateEndTurn = defaultEstimateEndTurn;
	m_iTurnSlice = defaultTurnSlice;
	m_iCutoffSlice = defaultCutoffSlice;
	m_iNumGameTurnActive = defaultNumGameTurnActive;
	m_iNumCities = defaultNumCities;
	m_iMaxPopulation = defaultMaxPopulation;
	m_iMaxLand = defaultMaxLand;
	m_iMaxFather = defaultMaxFather;
	m_iInitPopulation = defaultInitPopulation;
	m_iInitLand = defaultInitLand;
	m_iInitFather = defaultInitFather;
	m_iAIAutoPlay = defaultAIAutoPlay;
	m_iBestLandUnitCombat = defaultBestLandUnitCombat;
	m_bScoreDirty = defaultScoreDirty;
	m_bFinalInitialized = defaultFinalInitialized;
	m_bHotPbemBetweenTurns = defaultHotPbemBetweenTurns;
	m_bMaxTurnsExtended = defaultMaxTurnsExtended;
	m_bWBNorthAmericanNative = defaultWBNorthAmericanNative;
	m_bWBSouthAmericanNative = defaultWBSouthAmericanNative;
	m_bWBCentralAmericanNative = defaultWBCentralAmericanNative;
	m_eHandicap = eHandicap;
	m_ePausePlayer = defaultPausePlayer;
	m_eBarbarianPlayer = defaultBarbarianPlayer;
	m_eChurchPlayer = defaultChurchPlayer;
	m_eWinner = defaultWinner;
	m_eVictory = defaultVictory;
	m_eGameState = defaultGameState;
	m_szScriptData.clear();

	m_em_iRankPlayer.reset();
	m_em_iPlayerScore.reset();
	m_em_iRankTeam.reset();
	m_em_iTeamRank.reset();
	m_em_iTeamScore.reset();
	m_em_iUnitCreatedCount.reset();
	m_em_iUnitClassCreatedCount.reset();
	m_em_iBuildingClassCreatedCount.reset();
	m_em_eFatherTeam.reset();
	m_em_iFatherGameTurn.reset();
	m_em_bSpecialUnitValid.reset();
	m_em_bSpecialBuildingValid.reset();
	m_em_bUniqueGoodyValid.reset();

	m_aszDestroyedCities.clear();
	m_aszGreatGeneralBorn.clear();
	m_aszGreatAdmiralBorn.clear();
	m_aszShipNamed.clear();
	m_deals.removeAll();
	m_mapRand.reset();
	m_sorenRand.reset();
	clearReplayMessageMap();
	m_iNumSessions = defaultNumSessions;
	m_aPlotExtraYields.clear();
	m_aeInactiveTriggers.clear();
	m_iNumCultureVictoryCities = defaultNumCultureVictoryCities;
	m_eCultureVictoryCultureLevel = defaultCultureVictoryCultureLevel;
}

void CvGame::read(CvSavegameReader reader)
{
	reader.AssignClassType(SAVEGAME_CLASS_GAME);

	// Init data before load
	// This will ensure that all variables not included in the savegame will have default values
	reset();

	// loop read all the variables
	// As long as each variable has a UnitSavegameVariables "header", order doesn't matter.
	// Variables can be read in any order and any number of variables can be skipped.
	bool bContinue = true;
	while (bContinue)
	{
		SavegameVariableTypes eType;
		reader.Read(eType);

		switch (eType)
		{
		case GameSave_END: bContinue = false; break;
		case GameSave_EndTurnMessagesSent: reader.Read(m_iEndTurnMessagesSent); break;
		case GameSave_ElapsedGameTurns: reader.Read(m_iElapsedGameTurns); break;
		case GameSave_StartTurn: reader.Read(m_iStartTurn); break;
		case GameSave_StartYear: reader.Read(m_iStartYear); break;
		case GameSave_EstimateEndTurn: reader.Read(m_iEstimateEndTurn); break;
		case GameSave_TurnSlice: reader.Read(m_iTurnSlice); break;
		case GameSave_CutoffSlice: reader.Read(m_iCutoffSlice); break;
		case GameSave_NumGameTurnActive: reader.Read(m_iNumGameTurnActive); break;
		case GameSave_NumCities: reader.Read(m_iNumCities); break;
		case GameSave_MaxPopulation: reader.Read(m_iMaxPopulation); break;
		case GameSave_MaxLand: reader.Read(m_iMaxLand); break;
		case GameSave_MaxFather: reader.Read(m_iMaxFather); break;
		case GameSave_InitPopulation: reader.Read(m_iInitPopulation); break;
		case GameSave_InitLand: reader.Read(m_iInitLand); break;
		case GameSave_InitFather: reader.Read(m_iInitFather); break;
		case GameSave_AIAutoPlay: reader.Read(m_iAIAutoPlay); break;
		case GameSave_BestLandUnitCombat: reader.Read(m_iBestLandUnitCombat); break;
		case GameSave_ScoreDirty: reader.Read(m_bScoreDirty); break;
		case GameSave_FinalInitialized: reader.Read(m_bFinalInitialized); break;
		case GameSave_HotPbemBetweenTurns: reader.Read(m_bHotPbemBetweenTurns); break;
		case GameSave_MaxTurnsExtended: reader.Read(m_bMaxTurnsExtended); break;
		case GameSave_WBNorthAmericanNative: reader.Read(m_bWBNorthAmericanNative); break;
		case GameSave_WBSouthAmericanNative: reader.Read(m_bWBSouthAmericanNative); break;
		case GameSave_WBCentralAmericanNative: reader.Read(m_bWBCentralAmericanNative); break;
		case GameSave_Handicap: reader.Read(m_eHandicap); break;
		case GameSave_PausePlayer: reader.Read(m_ePausePlayer); break;
		case GameSave_BarbarianPlayer: reader.Read(m_eBarbarianPlayer); break;
		case GameSave_ChurchPlayer: reader.Read(m_eChurchPlayer); break;
		case GameSave_Winner: reader.Read(m_eWinner); break;
		case GameSave_Victory: reader.Read(m_eVictory); break;
		case GameSave_GameState: reader.Read(m_eGameState); break;
		case GameSave_ScriptData: reader.Read(m_szScriptData); break;

		case GameSave_RankPlayer: reader.Read(m_em_iRankPlayer); break;
		case GameSave_PlayerScore: reader.Read(m_em_iPlayerScore); break;
		case GameSave_RankTeam: reader.Read(m_em_iRankTeam); break;
		case GameSave_TeamRank: reader.Read(m_em_iTeamRank); break;
		case GameSave_TeamScore: reader.Read(m_em_iTeamScore); break;
		case GameSave_UnitCreatedCount: reader.Read(m_em_iUnitCreatedCount); break;
		case GameSave_UnitClassCreatedCount: reader.Read(m_em_iUnitClassCreatedCount); break;
		case GameSave_BuildingClassCreatedCount: reader.Read(m_em_iBuildingClassCreatedCount); break;
		case GameSave_FatherTeam: reader.Read(m_em_eFatherTeam); break;
		case GameSave_FatherGameTurn: reader.Read(m_em_iFatherGameTurn); break;
		case GameSave_SpecialUnitValid: reader.Read(m_em_bSpecialUnitValid); break;
		case GameSave_SpecialBuildingValid: reader.Read(m_em_bSpecialBuildingValid); break;
		case GameSave_UniqueGoodyValid: reader.Read(m_em_bUniqueGoodyValid); break;

		case GameSave_DestroyedCities: reader.Read(m_aszDestroyedCities); break;
		case GameSave_GreatGeneralBorn: reader.Read(m_aszGreatGeneralBorn); break;
		case GameSave_GreatAdmiralBorn: reader.Read(m_aszGreatAdmiralBorn); break;
		case GameSave_ShipNamed: reader.Read(m_aszShipNamed); break;
		case GameSave_deals: reader.Read(m_deals); break;
		case GameSave_mapRand: reader.Read(m_mapRand); break;
		case GameSave_sorenRand: reader.Read(m_sorenRand); break;
		case GameSave_listReplayMessages: reader.Read(m_listReplayMessages); break;
		case GameSave_NumSessions: reader.Read(m_iNumSessions); break;
		case GameSave_PlotExtraYields: reader.Read(m_aPlotExtraYields); break;
		case GameSave_InactiveTriggers: reader.Read(m_aeInactiveTriggers); break;
		case GameSave_NumCultureVictoryCities: reader.Read(m_iNumCultureVictoryCities); break;
		case GameSave_CultureVictoryCultureLevel: reader.Read(m_eCultureVictoryCultureLevel); break;
		}
		
	}

	if (!isNetworkMultiPlayer())
	{
		++m_iNumSessions;
	} 
	// Get the active player information from the initialization structure
	if (!isGameMultiPlayer())
	{
		int iI;
		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isHuman())
			{
				setActivePlayer((PlayerTypes)iI);
				break;
			}
		}
		addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getActivePlayer(), gDLL->getText("TXT_KEY_MISC_RELOAD", m_iNumSessions));
	}
		if (isOption(GAMEOPTION_NEW_RANDOM_SEED))
	{
		if (!isNetworkMultiPlayer())
		{
			m_sorenRand.reseed(timeGetTime());
		}
	}
}

void CvGame::write(CvSavegameWriter writer)
{
	writer.AssignClassType(SAVEGAME_CLASS_GAME);

	// Write the data.
	// Use WriteSwitch since it will automatically include WriteSwitch in the savegame.
	// Also it will not save anything if the variable and the default values are identical.
	// If nothing is saved, the loading code will use the default values.
	// Less data saved/loaded means smaller savegames.

	writer.Write(GameSave_EndTurnMessagesSent, m_iEndTurnMessagesSent, defaultEndTurnMessagesSent);
	writer.Write(GameSave_ElapsedGameTurns, m_iElapsedGameTurns, defaultElapsedGameTurns);
	writer.Write(GameSave_StartTurn, m_iStartTurn, defaultStartTurn);
	writer.Write(GameSave_StartYear, m_iStartYear, defaultStartYear);
	writer.Write(GameSave_EstimateEndTurn, m_iEstimateEndTurn, defaultEstimateEndTurn);
	writer.Write(GameSave_TurnSlice, m_iTurnSlice, defaultTurnSlice);
	writer.Write(GameSave_CutoffSlice, m_iCutoffSlice, defaultCutoffSlice);
	writer.Write(GameSave_NumGameTurnActive, m_iNumGameTurnActive, defaultNumGameTurnActive);
	writer.Write(GameSave_NumCities, m_iNumCities, defaultNumCities);
	writer.Write(GameSave_MaxPopulation, m_iMaxPopulation, defaultMaxPopulation);
	writer.Write(GameSave_MaxLand, m_iMaxLand, defaultMaxLand);
	writer.Write(GameSave_MaxFather, m_iMaxFather, defaultMaxFather);
	writer.Write(GameSave_InitPopulation, m_iInitPopulation, defaultInitPopulation);
	writer.Write(GameSave_InitLand, m_iInitLand, defaultInitLand);
	writer.Write(GameSave_InitFather, m_iInitFather, defaultInitFather);
	writer.Write(GameSave_AIAutoPlay, m_iAIAutoPlay, defaultAIAutoPlay);
	writer.Write(GameSave_BestLandUnitCombat, m_iBestLandUnitCombat, defaultBestLandUnitCombat);
	writer.Write(GameSave_ScoreDirty, m_bScoreDirty, defaultScoreDirty);
	writer.Write(GameSave_FinalInitialized, m_bFinalInitialized, defaultFinalInitialized);
	writer.Write(GameSave_HotPbemBetweenTurns, m_bHotPbemBetweenTurns, defaultHotPbemBetweenTurns);
	writer.Write(GameSave_MaxTurnsExtended, m_bMaxTurnsExtended, defaultMaxTurnsExtended);
	writer.Write(GameSave_WBNorthAmericanNative, m_bWBNorthAmericanNative, defaultWBNorthAmericanNative);
	writer.Write(GameSave_WBSouthAmericanNative, m_bWBSouthAmericanNative, defaultWBSouthAmericanNative);
	writer.Write(GameSave_WBCentralAmericanNative, m_bWBCentralAmericanNative, defaultWBCentralAmericanNative);
	writer.Write(GameSave_Handicap, m_eHandicap, defaultHandicap);
	writer.Write(GameSave_PausePlayer, m_ePausePlayer, defaultPausePlayer);
	writer.Write(GameSave_BarbarianPlayer, m_eBarbarianPlayer, defaultBarbarianPlayer);
	writer.Write(GameSave_ChurchPlayer, m_eChurchPlayer, defaultChurchPlayer);
	writer.Write(GameSave_Winner, m_eWinner, defaultWinner);
	writer.Write(GameSave_Victory, m_eVictory, defaultVictory);
	writer.Write(GameSave_GameState, m_eGameState, defaultGameState);
	writer.Write(GameSave_ScriptData, m_szScriptData);

	writer.Write(GameSave_RankPlayer, m_em_iRankPlayer);
	writer.Write(GameSave_PlayerScore, m_em_iPlayerScore);
	writer.Write(GameSave_RankTeam, m_em_iRankTeam);
	writer.Write(GameSave_TeamRank, m_em_iTeamRank);
	writer.Write(GameSave_TeamScore, m_em_iTeamScore);
	writer.Write(GameSave_UnitCreatedCount, m_em_iUnitCreatedCount);
	writer.Write(GameSave_UnitClassCreatedCount, m_em_iUnitClassCreatedCount);
	writer.Write(GameSave_BuildingClassCreatedCount, m_em_iBuildingClassCreatedCount);
	writer.Write(GameSave_FatherTeam, m_em_eFatherTeam);
	writer.Write(GameSave_FatherGameTurn, m_em_iFatherGameTurn);
	writer.Write(GameSave_SpecialUnitValid, m_em_bSpecialUnitValid);
	writer.Write(GameSave_SpecialBuildingValid, m_em_bSpecialBuildingValid);
	writer.Write(GameSave_UniqueGoodyValid, m_em_bUniqueGoodyValid);

	writer.Write(GameSave_DestroyedCities, m_aszDestroyedCities);
	writer.Write(GameSave_GreatGeneralBorn, m_aszGreatGeneralBorn);
	writer.Write(GameSave_GreatAdmiralBorn, m_aszGreatAdmiralBorn);
	writer.Write(GameSave_ShipNamed, m_aszShipNamed);
	writer.Write(GameSave_deals, m_deals);
	writer.Write(GameSave_mapRand, m_mapRand);
	writer.Write(GameSave_sorenRand, m_sorenRand);
	writer.Write(GameSave_listReplayMessages, m_listReplayMessages);
	writer.Write(GameSave_NumSessions, m_iNumSessions, defaultNumSessions);
	writer.Write(GameSave_PlotExtraYields, m_aPlotExtraYields);
	writer.Write(GameSave_InactiveTriggers, m_aeInactiveTriggers);
	writer.Write(GameSave_NumCultureVictoryCities, m_iNumCultureVictoryCities, defaultNumCultureVictoryCities);
	writer.Write(GameSave_CultureVictoryCultureLevel, m_eCultureVictoryCultureLevel, defaultCultureVictoryCultureLevel);

	writer.Write(GameSave_END);
}
