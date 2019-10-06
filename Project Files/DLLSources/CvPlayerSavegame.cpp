#include "CvGameCoreDLL.h"

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
		}
	}
	
	// The player is loaded. Now set up the cache according to the read data.

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
	

	writer.Write(PlayerSave_END);
}
