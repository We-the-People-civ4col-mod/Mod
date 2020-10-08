#include "CvGameCoreDLL.h"
#include "CvInitCore.h"

#include "CvSavegame.h"

// set the default values
const int defaultNumMembers = 0;
const int defaultAliveCount = 0;
const int defaultEverAliveCount = 0;
const int defaultNumCities = 0;
const int defaultTotalLand = 0;
const int defaultMapTradingCount = 0;
const int defaultGoldTradingCount = 0;
const int defaultOpenBordersTradingCount = 0;
const int defaultDefensivePactTradingCount = 0;
const int defaultPermanentAllianceTradingCount = 0;

const bool defaultMapCentering = false;

const TeamTypes defaultID = NO_TEAM;


// 
enum SavegameVariableTypes
{
	TeamSave_END,
	TeamSave_NumMembers,
	TeamSave_AliveCount,
	TeamSave_EverAliveCount,
	TeamSave_NumCities,
	TeamSave_TotalLand,
	TeamSave_MapTradingCount,
	TeamSave_GoldTradingCount,
	TeamSave_OpenBordersTradingCount,
	TeamSave_DefensivePactTradingCount,
	TeamSave_PermanentAllianceTradingCount,

	TeamSave_MapCentering,

	TeamSave_ID,

	TeamSave_bHasMet,
	TeamSave_bAtWar,
	TeamSave_bPermanentWarPeace,
	TeamSave_bOpenBorders,
	TeamSave_bDefensivePact,
	TeamSave_bForcePeace,

	TeamSave_FatherIgnore,
	TeamSave_FatherPoints,
	TeamSave_UnitClassCount,
	TeamSave_BuildingClassCount,
	TeamSave_EuropeUnitsPurchased,

	TeamSave_RevealedBonuses,
	NUM_SAVE_ENUM_VALUES,
};

const char* getSavedEnumNameTeam(SavegameVariableTypes eType)
{
	switch (eType)
	{
	case TeamSave_END: return "TeamSave_END";
	case TeamSave_NumMembers: return "TeamSave_NumMembers";
	case TeamSave_AliveCount: return "TeamSave_AliveCount";
	case TeamSave_EverAliveCount: return "TeamSave_EverAliveCount";
	case TeamSave_NumCities: return "TeamSave_NumCities";
	case TeamSave_TotalLand: return "TeamSave_TotalLand";
	case TeamSave_MapTradingCount: return "TeamSave_MapTradingCount";
	case TeamSave_GoldTradingCount: return "TeamSave_GoldTradingCount";
	case TeamSave_OpenBordersTradingCount: return "TeamSave_OpenBordersTradingCount";
	case TeamSave_DefensivePactTradingCount: return "TeamSave_DefensivePactTradingCount";
	case TeamSave_PermanentAllianceTradingCount: return "TeamSave_PermanentAllianceTradingCount";

	case TeamSave_MapCentering: return "TeamSave_MapCentering";

	case TeamSave_ID: return "TeamSave_ID";

	case TeamSave_bHasMet: return "TeamSave_HasMet";
	case TeamSave_bAtWar: return "TeamSave_AtWar";
	case TeamSave_bPermanentWarPeace: return "TeamSave_PermanentWarPeace";
	case TeamSave_bOpenBorders: return "TeamSave_OpenBorders";
	case TeamSave_bDefensivePact: return "TeamSave_DefensivePact";
	case TeamSave_bForcePeace: return "TeamSave_ForcePeace";

	case TeamSave_FatherIgnore: return "TeamSave_FatherIgnore";
	case TeamSave_FatherPoints: return "TeamSave_FatherPoints";
	case TeamSave_UnitClassCount: return "TeamSave_UnitClassCount";
	case TeamSave_BuildingClassCount: return "TeamSave_BuildingClassCount";
	case TeamSave_EuropeUnitsPurchased: return "TeamSave_EuropeUnitsPurchased";

	case TeamSave_RevealedBonuses: return "TeamSave_RevealedBonuses";
	}
	return "";
}

int getNumSavedEnumValuesTeam()
{
	return NUM_SAVE_ENUM_VALUES;
}

// assign everything to default values
void CvTeam::resetSavedData(TeamTypes eID)
{
		m_iNumMembers = defaultNumMembers;
		m_iAliveCount = defaultAliveCount;
		m_iEverAliveCount = defaultEverAliveCount;
		m_iNumCities = defaultNumCities;
		m_iTotalLand = defaultTotalLand;
		m_iMapTradingCount =  GC.getDefineINT("ENABLE_MAP_TRADING");
		m_iGoldTradingCount = GC.getDefineINT("ENABLE_GOLD_TRADING");
		m_iOpenBordersTradingCount = GC.getDefineINT("ENABLE_OPEN_BORDERS");
		m_iDefensivePactTradingCount = GC.getDefineINT("ENABLE_DEFENSIVE_PACT_TRADING");
		m_iPermanentAllianceTradingCount = GC.getDefineINT("ENABLE_ALLIANCE_TRADING");

		m_bMapCentering = defaultMapCentering;

		m_eID = eID;

		m_em_bHasMet.reset();
		m_em_bAtWar.reset();
		m_em_bPermanentWarPeace.reset();
		m_em_bOpenBorders.reset();
		m_em_bDefensivePact.reset();
		m_em_bForcePeace.reset();

		m_em_bFatherIgnore.reset();
		m_em_iFatherPoints.reset();
		m_em_iUnitClassCount.reset();
		m_em_iBuildingClassCount.reset();
		m_em_iEuropeUnitsPurchased.reset();

		m_aeRevealedBonuses.clear();
}

void CvTeam::read(CvSavegameReader reader)
{
	reader.AssignClassType(SAVEGAME_CLASS_TEAM);

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
		case TeamSave_END: bContinue = false; break;
		case TeamSave_NumMembers: reader.Read(m_iNumMembers); break;
		case TeamSave_AliveCount: reader.Read(m_iAliveCount); break;
		case TeamSave_EverAliveCount: reader.Read(m_iEverAliveCount); break;
		case TeamSave_NumCities: reader.Read(m_iNumCities); break;
		case TeamSave_TotalLand: reader.Read(m_iTotalLand); break;
		case TeamSave_MapTradingCount: reader.Read(m_iMapTradingCount); break;
		case TeamSave_GoldTradingCount: reader.Read(m_iGoldTradingCount); break;
		case TeamSave_OpenBordersTradingCount: reader.Read(m_iOpenBordersTradingCount); break;
		case TeamSave_DefensivePactTradingCount: reader.Read(m_iDefensivePactTradingCount); break;
		case TeamSave_PermanentAllianceTradingCount: reader.Read(m_iPermanentAllianceTradingCount); break;

		case TeamSave_MapCentering: reader.Read(m_bMapCentering); break;

		case TeamSave_ID: reader.Read(m_eID); break;

		case TeamSave_bHasMet: reader.Read(m_em_bHasMet); break;
		case TeamSave_bAtWar: reader.Read(m_em_bAtWar); break;
		case TeamSave_bPermanentWarPeace: reader.Read(m_em_bPermanentWarPeace); break;
		case TeamSave_bOpenBorders: reader.Read(m_em_bOpenBorders); break;
		case TeamSave_bDefensivePact: reader.Read(m_em_bDefensivePact); break;
		case TeamSave_bForcePeace: reader.Read(m_em_bForcePeace); break;

		case TeamSave_FatherIgnore: reader.Read(m_em_bFatherIgnore); break;
		case TeamSave_FatherPoints: reader.Read(m_em_iFatherPoints); break;
		case TeamSave_UnitClassCount: reader.Read(m_em_iUnitClassCount); break;
		case TeamSave_BuildingClassCount: reader.Read(m_em_iBuildingClassCount); break;
		case TeamSave_EuropeUnitsPurchased: reader.Read(m_em_iEuropeUnitsPurchased); break;

		case TeamSave_RevealedBonuses: reader.Read(m_aeRevealedBonuses); break;
		}
	}
	
}

void CvTeam::write(CvSavegameWriter writer)
{
	writer.AssignClassType(SAVEGAME_CLASS_TEAM);

	// Write the data.
	// Use WriteSwitch since it will automatically include WriteSwitch in the savegame.
	// Also it will not save anything if the variable and the default values are identical.
	// If nothing is saved, the loading code will use the default values.
	// Less data saved/loaded means smaller savegames.
	writer.Write(TeamSave_NumMembers, m_iNumMembers, defaultNumMembers);
	writer.Write(TeamSave_AliveCount, m_iAliveCount, defaultAliveCount);
	writer.Write(TeamSave_EverAliveCount, m_iEverAliveCount, defaultEverAliveCount);
	writer.Write(TeamSave_NumCities, m_iNumCities, defaultNumCities);
	writer.Write(TeamSave_TotalLand, m_iTotalLand, defaultTotalLand);
	writer.Write(TeamSave_MapTradingCount, m_iMapTradingCount, defaultMapTradingCount);
	writer.Write(TeamSave_GoldTradingCount, m_iGoldTradingCount, defaultGoldTradingCount);
	writer.Write(TeamSave_OpenBordersTradingCount, m_iOpenBordersTradingCount, defaultOpenBordersTradingCount);
	writer.Write(TeamSave_DefensivePactTradingCount, m_iDefensivePactTradingCount, defaultDefensivePactTradingCount);
	writer.Write(TeamSave_PermanentAllianceTradingCount, m_iPermanentAllianceTradingCount, defaultPermanentAllianceTradingCount);

	writer.Write(TeamSave_MapCentering, m_bMapCentering, defaultMapCentering);

	writer.Write(TeamSave_ID, m_eID, defaultID);

	writer.Write(TeamSave_bHasMet, m_em_bHasMet);
	writer.Write(TeamSave_bAtWar, m_em_bAtWar);
	writer.Write(TeamSave_bPermanentWarPeace, m_em_bPermanentWarPeace);
	writer.Write(TeamSave_bOpenBorders, m_em_bOpenBorders);
	writer.Write(TeamSave_bDefensivePact, m_em_bDefensivePact);
	writer.Write(TeamSave_bForcePeace, m_em_bForcePeace);

	writer.Write(TeamSave_FatherIgnore, m_em_bFatherIgnore);
	writer.Write(TeamSave_FatherPoints, m_em_iFatherPoints);
	writer.Write(TeamSave_UnitClassCount, m_em_iUnitClassCount);
	writer.Write(TeamSave_BuildingClassCount, m_em_iBuildingClassCount);
	writer.Write(TeamSave_EuropeUnitsPurchased, m_em_iEuropeUnitsPurchased);

	writer.Write(TeamSave_RevealedBonuses, m_aeRevealedBonuses);
	writer.Write(TeamSave_END);
}
