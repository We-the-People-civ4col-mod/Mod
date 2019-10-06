#include "CvGameCoreDLL.h"
#include "CvInitCore.h"

#include "CvSavegame.h"

// set the default values
const int defaultNumCombatsWon = 0;
const int defaultNumSeaCombatsWon = 0;
const int defaultStartingX = INVALID_PLOT_COORD;
const int defaultStartingY = INVALID_PLOT_COORD;
const int defaultTotalPopulation = 0;
const int defaultTotalLand = 0;
const int defaultTotalLandScored = 0;
const int defaultGold = 0;
const int defaultAdvancedStartPoints = -1;
const int defaultGreatGeneralsCreated = 0;
const int defaultGreatGeneralsThresholdModifier = 0;
const int defaultGreatAdmiralsCreated = 0;
const int defaultGreatAdmiralsThresholdModifier = 0;
const int defaultGreatGeneralRateModifier = 0;
const int defaultDomesticGreatGeneralRateModifier = 0;
const int defaultImmigrationThresholdMultiplier = 100;
const int defaultRevolutionEuropeUnitThresholdMultiplier = 100;

const int defaultNativeAngerModifier = 0;
const int defaultFreeExperience = 0;
const int defaultWorkerSpeedModifier = 0;
const int defaultImprovementUpgradeRateModifier = 0;
const int defaultMilitaryProductionModifier = 0;
const int defaultCityDefenseModifier = 0;
const int defaultHighestUnitLevel = 1;
const int defaultFatherOverflowBells = 0;
const int defaultExpInBorderModifier = 0;
const int defaultLevelExperienceModifier = 0;
const int defaultCapitalCityID = FFreeList::INVALID_INDEX;
const int defaultCitiesLost = 0;
const int defaultAssets = 0;
const int defaultPower = 0;
const int defaultPopulationScore = 0;
const int defaultLandScore = 0;
const int defaultFatherScore = 0;
const int defaultCombatExperience = 0;
const int defaultSeaCombatExperience = 0;

const int defaultTimerNativeMerc = 0;
const int defaultTimerEuropeanWars = 0;
const int defaultTimerEuropeanPeace = 0;
const int defaultTimerPrisonsCrowded = 0;
const int defaultTimerRevolutionaryNoble = 0;
const int defaultTimerBishop = 0;
const int defaultTimerChurchDemand = 0;
const int defaultTimerChurchWar = 0;
const int defaultTimerSmugglingShip = 0;
const int defaultTimerRanger = 0;
const int defaultTimerConquistador = 0;
const int defaultTimerPirates = 0;
const int defaultTimerContinentalGuard = 0;
const int defaultTimerMortar = 0;
const int defaultTimerNativeSlave = 0;
const int defaultTimerAfricanSlaves = 0;
const int defaultTimerStealingImmigrant = 0;
const int defaultChurchFavoursReceived = 0;

const int defaultKingNumUnitMultiplier = 100;
const int defaultMissionarySuccessPercent = 100;
const int defaultTimeNoTrade = 0;

const bool defaultAlive = false;
const bool defaultWillingToBargain = false;
const bool defaultEverAlive = false;
const bool defaultTurnActive = false;
const bool defaultAutoMoves = false;
const bool defaultEndTurn = false;
const bool defaultPbemNewTurn = false;
const bool defaultExtendedGame = false;
const bool defaultFoundedFirstCity = false;
const bool defaultStrike = false;

const PlayerTypes defaultID = NO_PLAYER;
const LeaderHeadTypes defaultPersonalityType = NO_LEADER;
const EraTypes defaultCurrentEra = ((EraTypes)0);
const PlayerTypes defaultParent = NO_PLAYER;
const YieldTypes defaultImmigrationConversion = YIELD_CROSSES;


// 
enum SavegameVariableTypes
{
	PlayerSave_END,
	PlayerSave_NumCombatsWon,
	PlayerSave_NumSeaCombatsWon,
	PlayerSave_StartingX,
	PlayerSave_StartingY,
	PlayerSave_TotalPopulation,
	PlayerSave_TotalLand,
	PlayerSave_TotalLandScored,
	PlayerSave_Gold,
	PlayerSave_AdvancedStartPoints,
	PlayerSave_GreatGeneralsCreated,
	PlayerSave_GreatGeneralsThresholdModifier,
	PlayerSave_GreatAdmiralsCreated,
	PlayerSave_GreatAdmiralsThresholdModifier,
	PlayerSave_GreatGeneralRateModifier,
	PlayerSave_DomesticGreatGeneralRateModifier,
	PlayerSave_ImmigrationThresholdMultiplier,
	PlayerSave_RevolutionEuropeUnitThresholdMultiplier,
	PlayerSave_NativeAngerModifier,
	PlayerSave_FreeExperience,
	PlayerSave_WorkerSpeedModifier,
	PlayerSave_ImprovementUpgradeRateModifier,
	PlayerSave_MilitaryProductionModifier,
	PlayerSave_CityDefenseModifier,
	PlayerSave_HighestUnitLevel,
	PlayerSave_FatherOverflowBells,
	PlayerSave_ExpInBorderModifier,
	PlayerSave_LevelExperienceModifier,
	PlayerSave_CapitalCityID,
	PlayerSave_CitiesLost,
	PlayerSave_Assets,
	PlayerSave_Power,
	PlayerSave_PopulationScore,
	PlayerSave_LandScore,
	PlayerSave_FatherScore,
	PlayerSave_CombatExperience,
	PlayerSave_SeaCombatExperience,
	PlayerSave_TimerNativeMerc,
	PlayerSave_TimerEuropeanWars,
	PlayerSave_TimerEuropeanPeace,
	PlayerSave_TimerPrisonsCrowded,
	PlayerSave_TimerRevolutionaryNoble,
	PlayerSave_TimerBishop,
	PlayerSave_TimerChurchDemand,
	PlayerSave_TimerChurchWar,
	PlayerSave_TimerSmugglingShip,
	PlayerSave_TimerRanger,
	PlayerSave_TimerConquistador,
	PlayerSave_TimerPirates,
	PlayerSave_TimerContinentalGuard,
	PlayerSave_TimerMortar,
	PlayerSave_TimerNativeSlave,
	PlayerSave_TimerAfricanSlaves,
	PlayerSave_TimerStealingImmigrant,
	PlayerSave_ChurchFavoursReceived,

	PlayerSave_KingNumUnitMultiplier,
	PlayerSave_MissionarySuccessPercent,
	PlayerSave_TimeNoTrade,

	PlayerSave_Alive,
	PlayerSave_WillingToBargain,
	PlayerSave_EverAlive,
	PlayerSave_TurnActive,
	PlayerSave_AutoMoves,
	PlayerSave_EndTurn,
	PlayerSave_PbemNewTurn,
	PlayerSave_ExtendedGame,
	PlayerSave_FoundedFirstCity,
	PlayerSave_Strike,

	PlayerSave_ID,
	PlayerSave_PersonalityType,
	PlayerSave_CurrentEra,
	PlayerSave_Parent,
	PlayerSave_ImmigrationConversion,

	NUM_SAVE_ENUM_VALUES,
};

const char* getSavedEnumNamePlayer(SavegameVariableTypes eType)
{
	switch (eType)
	{
	case PlayerSave_END: return "PlayerSave_END";
	case PlayerSave_NumCombatsWon: return "PlayerSave_NumCombatsWon";
	case PlayerSave_NumSeaCombatsWon: return "PlayerSave_NumSeaCombatsWon";
	case PlayerSave_StartingX: return "PlayerSave_StartingX";
	case PlayerSave_StartingY: return "PlayerSave_StartingY";
	case PlayerSave_TotalPopulation: return "PlayerSave_TotalPopulation";
	case PlayerSave_TotalLand: return "PlayerSave_TotalLand";
	case PlayerSave_TotalLandScored: return "PlayerSave_TotalLandScored";
	case PlayerSave_Gold: return "PlayerSave_Gold";
	case PlayerSave_AdvancedStartPoints: return "PlayerSave_AdvancedStartPoints";
	case PlayerSave_GreatGeneralsCreated: return "PlayerSave_GreatGeneralsCreated";
	case PlayerSave_GreatGeneralsThresholdModifier: return "PlayerSave_GreatGeneralsThresholdModifier";
	case PlayerSave_GreatAdmiralsCreated: return "PlayerSave_GreatAdmiralsCreated";
	case PlayerSave_GreatAdmiralsThresholdModifier: return "PlayerSave_GreatAdmiralsThresholdModifier";
	case PlayerSave_GreatGeneralRateModifier: return "PlayerSave_GreatGeneralRateModifier";
	case PlayerSave_DomesticGreatGeneralRateModifier: return "PlayerSave_DomesticGreatGeneralRateModifier";
	case PlayerSave_ImmigrationThresholdMultiplier: return "PlayerSave_ImmigrationThresholdMultiplier";
	case PlayerSave_RevolutionEuropeUnitThresholdMultiplier: return "PlayerSave_RevolutionEuropeUnitThresholdMultiplier";
	case PlayerSave_NativeAngerModifier: return "PlayerSave_NativeAngerModifier";
	case PlayerSave_FreeExperience: return "PlayerSave_FreeExperience";
	case PlayerSave_WorkerSpeedModifier: return "PlayerSave_WorkerSpeedModifier";
	case PlayerSave_ImprovementUpgradeRateModifier: return "PlayerSave_ImprovementUpgradeRateModifier";
	case PlayerSave_MilitaryProductionModifier: return "PlayerSave_MilitaryProductionModifier";
	case PlayerSave_CityDefenseModifier: return "PlayerSave_CityDefenseModifier";
	case PlayerSave_HighestUnitLevel: return "PlayerSave_HighestUnitLevel";
	case PlayerSave_FatherOverflowBells: return "PlayerSave_FatherOverflowBells";
	case PlayerSave_ExpInBorderModifier: return "PlayerSave_ExpInBorderModifier";
	case PlayerSave_LevelExperienceModifier: return "PlayerSave_LevelExperienceModifier";
	case PlayerSave_CapitalCityID: return "PlayerSave_CapitalCityID";
	case PlayerSave_CitiesLost: return "PlayerSave_CitiesLost";
	case PlayerSave_Assets: return "PlayerSave_Assets";
	case PlayerSave_Power: return "PlayerSave_Power";
	case PlayerSave_PopulationScore: return "PlayerSave_PopulationScore";
	case PlayerSave_LandScore: return "PlayerSave_LandScore";
	case PlayerSave_FatherScore: return "PlayerSave_FatherScore";
	case PlayerSave_CombatExperience: return "PlayerSave_CombatExperience";
	case PlayerSave_SeaCombatExperience: return "PlayerSave_SeaCombatExperience";
	case PlayerSave_TimerNativeMerc: return "PlayerSave_TimerNativeMerc";
	case PlayerSave_TimerEuropeanWars: return "PlayerSave_TimerEuropeanWars";
	case PlayerSave_TimerEuropeanPeace: return "PlayerSave_TimerEuropeanPeace";
	case PlayerSave_TimerPrisonsCrowded: return "PlayerSave_TimerPrisonsCrowded";
	case PlayerSave_TimerRevolutionaryNoble: return "PlayerSave_TimerRevolutionaryNoble";
	case PlayerSave_TimerBishop: return "PlayerSave_TimerBishop";
	case PlayerSave_TimerChurchDemand: return "PlayerSave_TimerChurchDemand";
	case PlayerSave_TimerChurchWar: return "PlayerSave_TimerChurchWar";
	case PlayerSave_TimerSmugglingShip: return "PlayerSave_TimerSmugglingShip";
	case PlayerSave_TimerRanger: return "PlayerSave_TimerRanger";
	case PlayerSave_TimerConquistador: return "PlayerSave_TimerConquistador";
	case PlayerSave_TimerPirates: return "PlayerSave_TimerPirates";
	case PlayerSave_TimerContinentalGuard: return "PlayerSave_TimerContinentalGuard";
	case PlayerSave_TimerMortar: return "PlayerSave_TimerMortar";
	case PlayerSave_TimerNativeSlave: return "PlayerSave_TimerNativeSlave";
	case PlayerSave_TimerAfricanSlaves: return "PlayerSave_TimerAfricanSlaves";
	case PlayerSave_TimerStealingImmigrant: return "PlayerSave_TimerStealingImmigrant";
	case PlayerSave_ChurchFavoursReceived: return "PlayerSave_ChurchFavoursReceived";

	case PlayerSave_KingNumUnitMultiplier: return "PlayerSave_KingNumUnitMultiplier";
	case PlayerSave_MissionarySuccessPercent: return "PlayerSave_MissionarySuccessPercent";
	case PlayerSave_TimeNoTrade: return "PlayerSave_TimeNoTrade";

	case PlayerSave_Alive: return "PlayerSave_Alive";
	case PlayerSave_WillingToBargain: return "PlayerSave_WillingToBargain";
	case PlayerSave_EverAlive: return "PlayerSave_EverAlive";
	case PlayerSave_TurnActive: return "PlayerSave_TurnActive";
	case PlayerSave_AutoMoves: return "PlayerSave_AutoMoves";
	case PlayerSave_EndTurn: return "PlayerSave_EndTurn";
	case PlayerSave_PbemNewTurn: return "PlayerSave_PbemNewTurn";
	case PlayerSave_ExtendedGame: return "PlayerSave_ExtendedGame";
	case PlayerSave_FoundedFirstCity: return "PlayerSave_FoundedFirstCity";
	case PlayerSave_Strike: return "PlayerSave_Strike";

	case PlayerSave_ID: return "PlayerSave_ID";
	case PlayerSave_PersonalityType: return "PlayerSave_PersonalityType";
	case PlayerSave_CurrentEra: return "PlayerSave_CurrentEra";
	case PlayerSave_Parent: return "PlayerSave_Parent";
	case PlayerSave_ImmigrationConversion: return "PlayerSave_ImmigrationConversion";
	}
	return "";
}

int getNumSavedEnumValuesPlayer()
{
	return NUM_SAVE_ENUM_VALUES;
}

// assign everything to default values
void CvPlayer::resetSavedData(PlayerTypes eID, bool bConstructorCall)
{
	m_iNumCombatsWon = defaultNumCombatsWon;
	m_iNumSeaCombatsWon = m_iNumSeaCombatsWon; 
	m_iStartingX = defaultStartingX;
	m_iStartingY = defaultStartingY;
	m_iTotalPopulation = defaultTotalPopulation;
	m_iTotalLand = defaultTotalLand;
	m_iTotalLandScored = defaultTotalLandScored;
	m_iGold = defaultGold;
	m_iAdvancedStartPoints = defaultAdvancedStartPoints;
	m_iGreatGeneralsCreated = defaultGreatGeneralsCreated;
	m_iGreatGeneralsThresholdModifier = defaultGreatGeneralsThresholdModifier;
	m_iGreatAdmiralsCreated = defaultGreatAdmiralsCreated;
	m_iGreatAdmiralsThresholdModifier = defaultGreatAdmiralsThresholdModifier;
	m_iGreatGeneralRateModifier = defaultGreatGeneralRateModifier;
	m_iDomesticGreatGeneralRateModifier = defaultDomesticGreatGeneralRateModifier;
	m_iImmigrationThresholdMultiplier = defaultImmigrationThresholdMultiplier;
	m_iRevolutionEuropeUnitThresholdMultiplier = defaultRevolutionEuropeUnitThresholdMultiplier;

	m_iNativeAngerModifier = defaultNativeAngerModifier;
	m_iFreeExperience = defaultFreeExperience;
	m_iWorkerSpeedModifier = defaultWorkerSpeedModifier;
	m_iImprovementUpgradeRateModifier = defaultImprovementUpgradeRateModifier;
	m_iMilitaryProductionModifier = defaultMilitaryProductionModifier;
	m_iCityDefenseModifier = defaultCityDefenseModifier;
	m_iHighestUnitLevel = defaultHighestUnitLevel;
	m_iFatherOverflowBells = defaultFatherOverflowBells;
	m_iExpInBorderModifier = defaultExpInBorderModifier;
	m_iLevelExperienceModifier = defaultLevelExperienceModifier;
	m_iCapitalCityID = defaultCapitalCityID;
	m_iCitiesLost = defaultCitiesLost;
	m_iAssets = defaultAssets;
	m_iPower = defaultPower;
	m_iPopulationScore = defaultPopulationScore;
	m_iLandScore = defaultLandScore;
	m_iFatherScore = defaultFatherScore;
	m_iCombatExperience = defaultCombatExperience;
	m_iSeaCombatExperience = defaultSeaCombatExperience;
	m_iTimerNativeMerc = defaultTimerNativeMerc;
	m_iTimerEuropeanWars = defaultTimerEuropeanWars;
	m_iTimerEuropeanPeace = defaultTimerEuropeanPeace;
	m_iTimerPrisonsCrowded = defaultTimerPrisonsCrowded;
	m_iTimerRevolutionaryNoble = defaultTimerRevolutionaryNoble;
	m_iTimerBishop = defaultTimerBishop;
	m_iTimerChurchDemand = defaultTimerChurchDemand;
	m_iTimerChurchWar = defaultTimerChurchWar;
	m_iTimerSmugglingShip = defaultTimerSmugglingShip;
	m_iTimerRanger = defaultTimerRanger;
	m_iTimerConquistador = defaultTimerConquistador;
	m_iTimerPirates = defaultTimerPirates;
	m_iTimerContinentalGuard = defaultTimerContinentalGuard;
	m_iTimerMortar = defaultTimerMortar;
	m_iTimerNativeSlave = defaultTimerNativeSlave;
	m_iTimerAfricanSlaves = defaultTimerAfricanSlaves;
	m_iTimerStealingImmigrant = defaultTimerStealingImmigrant;
	m_iChurchFavoursReceived = defaultChurchFavoursReceived;

	m_iKingNumUnitMultiplier = defaultKingNumUnitMultiplier;
	m_iMissionarySuccessPercent = defaultMissionarySuccessPercent;
	m_iTimeNoTrade = defaultTimeNoTrade;

	m_bAlive = defaultAlive;
	m_bWillingToBargain = defaultWillingToBargain;
	m_bEverAlive = defaultEverAlive;
	m_bTurnActive = defaultTurnActive;
	m_bAutoMoves = defaultAutoMoves;
	m_bEndTurn = defaultEndTurn;
	m_bPbemNewTurn = defaultPbemNewTurn;
	m_bExtendedGame = defaultExtendedGame;
	m_bFoundedFirstCity = defaultFoundedFirstCity;
	m_bStrike = defaultStrike;

	m_eID = eID;
	updateTeamType();
	updateHuman();
	if (m_eID != NO_PLAYER)
	{
		m_ePersonalityType = GC.getInitCore().getLeader(m_eID); //??? Is this repeated data???
	}
	else
	{
		m_ePersonalityType = NO_LEADER;
	}
	m_eCurrentEra= defaultCurrentEra;
	m_eParent= defaultParent;
	m_eImmigrationConversion= defaultImmigrationConversion;
}

void CvPlayer::read(CvSavegameReader reader)
{
	reader.AssignClassType(SAVEGAME_CLASS_PLAYER);

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
		case PlayerSave_END: bContinue = false; break;
		case PlayerSave_NumCombatsWon: reader.Read(m_iNumCombatsWon); break;
		case PlayerSave_NumSeaCombatsWon: reader.Read(m_iNumSeaCombatsWon); break;
		case PlayerSave_StartingX: reader.Read(m_iStartingX); break;
		case PlayerSave_StartingY: reader.Read(m_iStartingY); break;
		case PlayerSave_TotalPopulation: reader.Read(m_iTotalPopulation); break;
		case PlayerSave_TotalLand: reader.Read(m_iTotalLand); break;
		case PlayerSave_TotalLandScored: reader.Read(m_iTotalLandScored); break;
		case PlayerSave_Gold: reader.Read(m_iGold); break;
		case PlayerSave_AdvancedStartPoints: reader.Read(m_iAdvancedStartPoints); break;
		case PlayerSave_GreatGeneralsCreated: reader.Read(m_iGreatGeneralsCreated); break;
		case PlayerSave_GreatGeneralsThresholdModifier: reader.Read(m_iGreatGeneralsThresholdModifier); break;
		case PlayerSave_GreatAdmiralsCreated: reader.Read(m_iGreatAdmiralsCreated); break;
		case PlayerSave_GreatAdmiralsThresholdModifier: reader.Read(m_iGreatAdmiralsThresholdModifier); break;
		case PlayerSave_GreatGeneralRateModifier: reader.Read(m_iGreatGeneralRateModifier); break;
		case PlayerSave_DomesticGreatGeneralRateModifier: reader.Read(m_iDomesticGreatGeneralRateModifier); break;
		case PlayerSave_ImmigrationThresholdMultiplier: reader.Read(m_iImmigrationThresholdMultiplier); break;
		case PlayerSave_RevolutionEuropeUnitThresholdMultiplier: reader.Read(m_iRevolutionEuropeUnitThresholdMultiplier); break;
		
		case PlayerSave_NativeAngerModifier: reader.Read(m_iNativeAngerModifier); break;
		case PlayerSave_FreeExperience: reader.Read(m_iFreeExperience); break;
		case PlayerSave_WorkerSpeedModifier: reader.Read(m_iWorkerSpeedModifier); break;
		case PlayerSave_ImprovementUpgradeRateModifier: reader.Read(m_iImprovementUpgradeRateModifier); break;
		case PlayerSave_MilitaryProductionModifier: reader.Read(m_iMilitaryProductionModifier); break;
		case PlayerSave_CityDefenseModifier: reader.Read(m_iCityDefenseModifier); break;
		case PlayerSave_HighestUnitLevel: reader.Read(m_iHighestUnitLevel); break;
		case PlayerSave_FatherOverflowBells: reader.Read(m_iFatherOverflowBells); break;
		case PlayerSave_ExpInBorderModifier: reader.Read(m_iExpInBorderModifier); break;
		case PlayerSave_LevelExperienceModifier: reader.Read(m_iLevelExperienceModifier); break;
		case PlayerSave_CapitalCityID: reader.Read(m_iCapitalCityID); break;
		case PlayerSave_CitiesLost: reader.Read(m_iCitiesLost); break;
		case PlayerSave_Assets: reader.Read(m_iAssets); break;
		case PlayerSave_Power: reader.Read(m_iPower); break;
		case PlayerSave_PopulationScore: reader.Read(m_iPopulationScore); break;
		case PlayerSave_LandScore: reader.Read(m_iLandScore); break;
		case PlayerSave_FatherScore: reader.Read(m_iFatherScore); break;
		case PlayerSave_CombatExperience: reader.Read(m_iCombatExperience); break;
		case PlayerSave_SeaCombatExperience: reader.Read(m_iSeaCombatExperience); break;
		case PlayerSave_TimerNativeMerc: reader.Read(m_iTimerNativeMerc); break;
		case PlayerSave_TimerEuropeanWars: reader.Read(m_iTimerEuropeanWars); break;
		case PlayerSave_TimerEuropeanPeace: reader.Read(m_iTimerEuropeanPeace); break;
		case PlayerSave_TimerPrisonsCrowded: reader.Read(m_iTimerPrisonsCrowded); break;
		case PlayerSave_TimerRevolutionaryNoble: reader.Read(m_iTimerRevolutionaryNoble); break;
		case PlayerSave_TimerBishop: reader.Read(m_iTimerBishop); break;
		case PlayerSave_TimerChurchDemand: reader.Read(m_iTimerChurchDemand); break;
		case PlayerSave_TimerChurchWar: reader.Read(m_iTimerChurchWar); break;
		case PlayerSave_TimerSmugglingShip: reader.Read(m_iTimerSmugglingShip); break;
		case PlayerSave_TimerRanger: reader.Read(m_iTimerRanger); break;
		case PlayerSave_TimerConquistador: reader.Read(m_iTimerConquistador); break;
		case PlayerSave_TimerPirates: reader.Read(m_iTimerPirates); break;
		case PlayerSave_TimerContinentalGuard: reader.Read(m_iTimerContinentalGuard); break;
		case PlayerSave_TimerMortar: reader.Read(m_iTimerMortar); break;
		case PlayerSave_TimerNativeSlave: reader.Read(m_iTimerNativeSlave); break;
		case PlayerSave_TimerAfricanSlaves: reader.Read(m_iTimerAfricanSlaves); break;
		case PlayerSave_TimerStealingImmigrant: reader.Read(m_iTimerStealingImmigrant); break;
		case PlayerSave_ChurchFavoursReceived: reader.Read(m_iChurchFavoursReceived); break;

		case PlayerSave_KingNumUnitMultiplier: reader.Read(m_iKingNumUnitMultiplier); break;
		case PlayerSave_MissionarySuccessPercent: reader.Read(m_iMissionarySuccessPercent); break;
		case PlayerSave_TimeNoTrade: reader.Read(m_iTimeNoTrade); break;

		case PlayerSave_Alive: reader.Read(m_bAlive); break;
		case PlayerSave_WillingToBargain: reader.Read(m_bWillingToBargain); break;
		case PlayerSave_EverAlive: reader.Read(m_bEverAlive); break;
		case PlayerSave_TurnActive: reader.Read(m_bTurnActive); break;
		case PlayerSave_AutoMoves: reader.Read(m_bAutoMoves); break;
		case PlayerSave_EndTurn: reader.Read(m_bEndTurn); break;
		case PlayerSave_PbemNewTurn: reader.Read(m_bPbemNewTurn); break;
		case PlayerSave_ExtendedGame: reader.Read(m_bExtendedGame); break;
		case PlayerSave_FoundedFirstCity: reader.Read(m_bFoundedFirstCity); break;
		case PlayerSave_Strike: reader.Read(m_bStrike); break;

		case PlayerSave_ID: reader.Read(m_eID); break;
		case PlayerSave_PersonalityType: reader.Read(m_ePersonalityType); break;
		case PlayerSave_CurrentEra: reader.Read(m_eCurrentEra); break;
		case PlayerSave_Parent: reader.Read(m_eParent); break;
		case PlayerSave_ImmigrationConversion: reader.Read(m_eImmigrationConversion); break;
		}
	}
	
	// The player is loaded. Now set up the cache according to the read data.
	updateTeamType(); //m_eTeamType not saved
	updateHuman();
}

void CvPlayer::write(CvSavegameWriter writer)
{
	writer.AssignClassType(SAVEGAME_CLASS_PLAYER);

	// Write the data.
	// Use WriteSwitch since it will automatically include WriteSwitch in the savegame.
	// Also it will not save anything if the variable and the default values are identical.
	// If nothing is saved, the loading code will use the default values.
	// Less data saved/loaded means smaller savegames.
	writer.Write(PlayerSave_NumCombatsWon, m_iNumCombatsWon, defaultNumCombatsWon);
	writer.Write(PlayerSave_NumSeaCombatsWon, m_iNumSeaCombatsWon, defaultNumSeaCombatsWon); 
	writer.Write(PlayerSave_StartingX, m_iStartingX, defaultStartingX);
	writer.Write(PlayerSave_StartingY, m_iStartingY, defaultStartingY);
	writer.Write(PlayerSave_TotalPopulation, m_iTotalPopulation, defaultTotalPopulation);
	writer.Write(PlayerSave_TotalLand, m_iTotalLand, defaultTotalLand);
	writer.Write(PlayerSave_TotalLandScored, m_iTotalLandScored, defaultTotalLandScored);
	writer.Write(PlayerSave_Gold, m_iGold, defaultGold);
	writer.Write(PlayerSave_AdvancedStartPoints, m_iAdvancedStartPoints, defaultAdvancedStartPoints);
	writer.Write(PlayerSave_GreatGeneralsCreated, m_iGreatGeneralsCreated, defaultGreatGeneralsCreated);
	writer.Write(PlayerSave_GreatGeneralsThresholdModifier, m_iGreatGeneralsThresholdModifier, defaultGreatGeneralsThresholdModifier);
	writer.Write(PlayerSave_GreatAdmiralsCreated, m_iGreatAdmiralsCreated, defaultGreatAdmiralsCreated);
	writer.Write(PlayerSave_GreatAdmiralsThresholdModifier, m_iGreatAdmiralsThresholdModifier, defaultGreatAdmiralsThresholdModifier);
	writer.Write(PlayerSave_GreatGeneralRateModifier, m_iGreatGeneralRateModifier, defaultGreatGeneralRateModifier);
	writer.Write(PlayerSave_DomesticGreatGeneralRateModifier, m_iDomesticGreatGeneralRateModifier, defaultDomesticGreatGeneralRateModifier);
	writer.Write(PlayerSave_ImmigrationThresholdMultiplier, m_iImmigrationThresholdMultiplier, defaultImmigrationThresholdMultiplier);
	writer.Write(PlayerSave_RevolutionEuropeUnitThresholdMultiplier, m_iRevolutionEuropeUnitThresholdMultiplier, defaultRevolutionEuropeUnitThresholdMultiplier);
	
	writer.Write(PlayerSave_NativeAngerModifier, m_iNativeAngerModifier, defaultNativeAngerModifier);
	writer.Write(PlayerSave_FreeExperience, m_iFreeExperience, defaultFreeExperience);
	writer.Write(PlayerSave_WorkerSpeedModifier, m_iWorkerSpeedModifier, defaultWorkerSpeedModifier);
	writer.Write(PlayerSave_ImprovementUpgradeRateModifier, m_iImprovementUpgradeRateModifier, defaultImprovementUpgradeRateModifier);
	writer.Write(PlayerSave_MilitaryProductionModifier, m_iMilitaryProductionModifier, defaultMilitaryProductionModifier);
	writer.Write(PlayerSave_CityDefenseModifier, m_iCityDefenseModifier, defaultCityDefenseModifier);
	writer.Write(PlayerSave_HighestUnitLevel, m_iHighestUnitLevel, defaultHighestUnitLevel);
	writer.Write(PlayerSave_FatherOverflowBells, m_iFatherOverflowBells, defaultFatherOverflowBells);
	writer.Write(PlayerSave_ExpInBorderModifier, m_iExpInBorderModifier, defaultExpInBorderModifier);
	writer.Write(PlayerSave_LevelExperienceModifier, m_iLevelExperienceModifier, defaultLevelExperienceModifier);
	writer.Write(PlayerSave_CapitalCityID, m_iCapitalCityID, defaultCapitalCityID);
	writer.Write(PlayerSave_CitiesLost, m_iCitiesLost, defaultCitiesLost);
	writer.Write(PlayerSave_Assets, m_iAssets, defaultAssets);
	writer.Write(PlayerSave_Power, m_iPower, defaultPower);
	writer.Write(PlayerSave_PopulationScore, m_iPopulationScore, defaultPopulationScore);
	writer.Write(PlayerSave_LandScore, m_iLandScore, defaultLandScore);
	writer.Write(PlayerSave_FatherScore, m_iFatherScore, defaultFatherScore);
	writer.Write(PlayerSave_CombatExperience, m_iCombatExperience, defaultCombatExperience);
	writer.Write(PlayerSave_SeaCombatExperience, m_iSeaCombatExperience, defaultSeaCombatExperience);
	writer.Write(PlayerSave_TimerNativeMerc, m_iTimerNativeMerc, defaultTimerNativeMerc);
	writer.Write(PlayerSave_TimerEuropeanWars, m_iTimerEuropeanWars, defaultTimerEuropeanWars);
	writer.Write(PlayerSave_TimerEuropeanPeace, m_iTimerEuropeanPeace, defaultTimerEuropeanPeace);
	writer.Write(PlayerSave_TimerPrisonsCrowded, m_iTimerPrisonsCrowded, defaultTimerPrisonsCrowded);
	writer.Write(PlayerSave_TimerRevolutionaryNoble, m_iTimerRevolutionaryNoble, defaultTimerRevolutionaryNoble);
	writer.Write(PlayerSave_TimerBishop, m_iTimerBishop, defaultTimerBishop);
	writer.Write(PlayerSave_TimerChurchDemand, m_iTimerChurchDemand, defaultTimerChurchDemand);
	writer.Write(PlayerSave_TimerChurchWar, m_iTimerChurchWar, defaultTimerChurchWar);
	writer.Write(PlayerSave_TimerSmugglingShip, m_iTimerSmugglingShip, defaultTimerSmugglingShip);
	writer.Write(PlayerSave_TimerRanger, m_iTimerRanger, defaultTimerRanger);
	writer.Write(PlayerSave_TimerConquistador, m_iTimerConquistador, defaultTimerConquistador);
	writer.Write(PlayerSave_TimerPirates, m_iTimerPirates, defaultTimerPirates);
	writer.Write(PlayerSave_TimerContinentalGuard, m_iTimerContinentalGuard, defaultTimerContinentalGuard);
	writer.Write(PlayerSave_TimerMortar, m_iTimerMortar, defaultTimerMortar);
	writer.Write(PlayerSave_TimerNativeSlave, m_iTimerNativeSlave, defaultTimerNativeSlave);
	writer.Write(PlayerSave_TimerAfricanSlaves, m_iTimerAfricanSlaves, defaultTimerAfricanSlaves);
	writer.Write(PlayerSave_TimerStealingImmigrant, m_iTimerStealingImmigrant, defaultTimerStealingImmigrant);
	writer.Write(PlayerSave_ChurchFavoursReceived, m_iChurchFavoursReceived, defaultChurchFavoursReceived);

	writer.Write(PlayerSave_KingNumUnitMultiplier, m_iKingNumUnitMultiplier, defaultKingNumUnitMultiplier);
	writer.Write(PlayerSave_MissionarySuccessPercent, m_iMissionarySuccessPercent, defaultMissionarySuccessPercent);
	writer.Write(PlayerSave_TimeNoTrade, m_iTimeNoTrade, defaultTimeNoTrade);

	writer.Write(PlayerSave_Alive, m_bAlive, defaultAlive);
	writer.Write(PlayerSave_WillingToBargain, m_bWillingToBargain, defaultWillingToBargain);
	writer.Write(PlayerSave_EverAlive, m_bEverAlive, defaultEverAlive);
	writer.Write(PlayerSave_TurnActive, m_bTurnActive, defaultTurnActive);
	writer.Write(PlayerSave_AutoMoves, m_bAutoMoves, defaultAutoMoves);
	writer.Write(PlayerSave_EndTurn, m_bEndTurn, defaultEndTurn);
	writer.Write(PlayerSave_PbemNewTurn, m_bPbemNewTurn && GC.getGameINLINE().isPbem(), defaultPbemNewTurn); 
	writer.Write(PlayerSave_ExtendedGame, m_bExtendedGame, defaultExtendedGame);
	writer.Write(PlayerSave_FoundedFirstCity, m_bFoundedFirstCity, defaultFoundedFirstCity);
	writer.Write(PlayerSave_Strike, m_bStrike, defaultStrike);

	writer.Write(PlayerSave_ID, m_eID, defaultID);
	writer.Write(PlayerSave_PersonalityType, m_ePersonalityType, defaultPersonalityType);
	writer.Write(PlayerSave_CurrentEra, m_eCurrentEra, defaultCurrentEra);
	writer.Write(PlayerSave_Parent, m_eParent, defaultParent);
	writer.Write(PlayerSave_ImmigrationConversion, m_eImmigrationConversion, defaultImmigrationConversion);

	writer.Write(PlayerSave_END);
}
