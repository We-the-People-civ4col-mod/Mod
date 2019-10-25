#include "CvGameCoreDLL.h"

#include "CvSavegame.h"



// set the default values
const int defaultDistanceMapDistance = -1;
const int defaultAttackOddsChange = 0;
const int defaultExtraGoldTarget = 0;
const int defaultAveragesCacheTurn = -1;
const UnitTypes defaultNextBuyUnit = NO_UNIT;
const UnitAITypes defaultNextBuyUnitAI = NO_UNITAI;
const int defaultNextBuyUnitValue = 0;
const ProfessionTypes defaultNextBuyProfession = NO_PROFESSION;
const UnitTypes defaultNextBuyProfessionUnit = NO_UNIT;
const UnitAITypes defaultNextBuyProfessionAI = NO_UNITAI;
const int defaultNextBuyProfessionValue = 0;
const int defaultTotalIncome = 0;
const int defaultHurrySpending = 0;
const int defaultImmigrantSpending = 0;
const int defaultEuropeYieldSpending = 0;


enum SavegameVariableTypes
{
	PlayerSaveAI_END,
	PlayerSaveAI_distanceMap,
	PlayerSaveAI_DistanceMapDistance,
	PlayerSaveAI_AttackOddsChange,
	PlayerSaveAI_ExtraGoldTarget,
	PlayerSaveAI_AveragesCacheTurn,
	PlayerSaveAI_NextBuyUnit,
	PlayerSaveAI_NextBuyUnitAI,
	PlayerSaveAI_NextBuyUnitValue,
	PlayerSaveAI_NextBuyProfession,
	PlayerSaveAI_NextBuyProfessionUnit,
	PlayerSaveAI_NextBuyProfessionAI,
	PlayerSaveAI_NextBuyProfessionValue,
	PlayerSaveAI_TotalIncome,
	PlayerSaveAI_HurrySpending,
	PlayerSaveAI_ImmigrantSpending,
	PlayerSaveAI_EuropeYieldSpending,

	NUM_SAVE_ENUM_VALUES,
};

const char* getSavedEnumNamePlayerAi(SavegameVariableTypes eType)
{
	switch (eType)
	{
	case PlayerSaveAI_END: return "PlayerSaveAI_END";

	case PlayerSaveAI_distanceMap: return "PlayerSaveAI_distanceMap";
	case PlayerSaveAI_DistanceMapDistance: return "PlayerSaveAI_DistanceMapDistance";
	case PlayerSaveAI_AttackOddsChange: return "PlayerSaveAI_AttackOddsChange";
	case PlayerSaveAI_ExtraGoldTarget: return "PlayerSaveAI_ExtraGoldTarget";
	case PlayerSaveAI_AveragesCacheTurn: return "PlayerSaveAI_AveragesCacheTurn";
	case PlayerSaveAI_NextBuyUnit: return "PlayerSaveAI_NextBuyUnit";
	case PlayerSaveAI_NextBuyUnitAI: return "PlayerSaveAI_NextBuyUnitAI";
	case PlayerSaveAI_NextBuyUnitValue: return "PlayerSaveAI_NextBuyUnitValue";
	case PlayerSaveAI_NextBuyProfession: return "PlayerSaveAI_NextBuyProfession";
	case PlayerSaveAI_NextBuyProfessionUnit: return "PlayerSaveAI_NextBuyProfessionUnit";
	case PlayerSaveAI_NextBuyProfessionAI: return "PlayerSaveAI_NextBuyProfessionAI";
	case PlayerSaveAI_NextBuyProfessionValue: return "PlayerSaveAI_NextBuyProfessionValue";
	case PlayerSaveAI_TotalIncome: return "PlayerSaveAI_TotalIncome";
	case PlayerSaveAI_HurrySpending: return "PlayerSaveAI_HurrySpending";
	case PlayerSaveAI_ImmigrantSpending: return "PlayerSaveAI_ImmigrantSpending";
	case PlayerSaveAI_EuropeYieldSpending: return "PlayerSaveAI_EuropeYieldSpending";

}	return "";
}

int getNumSavedEnumValuesPlayerAI()
{
	return NUM_SAVE_ENUM_VALUES;
}

// assign everything to default values
void CvPlayerAI::AI_resetSavedData()
{
	m_distanceMap.clear();
	m_iDistanceMapDistance = defaultDistanceMapDistance;
	m_iAttackOddsChange = defaultAttackOddsChange;
	m_iExtraGoldTarget,defaultExtraGoldTarget;
	m_iAveragesCacheTurn = defaultAveragesCacheTurn;
	m_eNextBuyUnit = defaultNextBuyUnit;
	m_eNextBuyUnitAI = defaultNextBuyUnitAI;
	m_iNextBuyUnitValue = defaultNextBuyUnitValue;
	m_eNextBuyProfession = defaultNextBuyProfession;
	m_eNextBuyProfessionUnit = defaultNextBuyProfessionUnit;
	m_eNextBuyProfessionAI = defaultNextBuyProfessionAI;
	m_iNextBuyProfessionValue = defaultNextBuyProfessionValue;
	m_iTotalIncome = defaultTotalIncome;
	m_iHurrySpending = defaultHurrySpending;
	m_iImmigrantSpending = defaultImmigrantSpending;
	m_iEuropeYieldSpending = defaultEuropeYieldSpending;
}

void CvPlayerAI::read(CvSavegameReader reader)
{
	reader.AssignClassType(SAVEGAME_CLASS_PLAYER_AI);

	// Init data before load
	// This will ensure that all variables not included in the savegame will have default values
	AI_resetSavedData();

	// read base class. It's always placed first
	CvPlayer::read(reader);

	// loop read all the variables
	// As long as each variable has a PlayerSavegameVariables "header", order doesn't matter.
	// Variables can be read in any order and any number of variables can be skipped.
	bool bContinue = true;
	while (bContinue)
	{
		SavegameVariableTypes eType;
		reader.Read(eType);

		switch (eType)
		{
		case PlayerSaveAI_END: bContinue = false; break;
		case PlayerSaveAI_distanceMap: reader.Read(m_distanceMap); break;
		case PlayerSaveAI_DistanceMapDistance: reader.Read(m_iDistanceMapDistance); break;
		case PlayerSaveAI_AttackOddsChange: reader.Read(m_iAttackOddsChange); break;
		case PlayerSaveAI_ExtraGoldTarget: reader.Read(m_iExtraGoldTarget); break;
		case PlayerSaveAI_AveragesCacheTurn: reader.Read(m_iAveragesCacheTurn); break;
		case PlayerSaveAI_NextBuyUnit: reader.Read(m_eNextBuyUnit); break;
		case PlayerSaveAI_NextBuyUnitAI: reader.Read(m_eNextBuyUnitAI); break;
		case PlayerSaveAI_NextBuyUnitValue: reader.Read(m_iNextBuyUnitValue); break;
		case PlayerSaveAI_NextBuyProfession: reader.Read(m_eNextBuyProfession); break;
		case PlayerSaveAI_NextBuyProfessionUnit: reader.Read(m_eNextBuyProfessionUnit); break;
		case PlayerSaveAI_NextBuyProfessionAI: reader.Read(m_eNextBuyProfessionAI); break;
		case PlayerSaveAI_NextBuyProfessionValue: reader.Read(m_iNextBuyProfessionValue); break;
		case PlayerSaveAI_TotalIncome: reader.Read(m_iTotalIncome); break;
		case PlayerSaveAI_HurrySpending: reader.Read(m_iHurrySpending); break;
		case PlayerSaveAI_ImmigrantSpending: reader.Read(m_iImmigrantSpending); break;
		case PlayerSaveAI_EuropeYieldSpending: reader.Read(m_iEuropeYieldSpending); break;

		}
	}
	
	// Loading done. Set up the cache (if any).

}

void CvPlayerAI::write(CvSavegameWriter writer)
{
	writer.AssignClassType(SAVEGAME_CLASS_PLAYER_AI);

	// Write the data.
	// Use WriteSwitch since it will automatically include WriteSwitch in the savegame.
	// Also it will not save anything if the variable and the default values are identical.
	// If nothing is saved, the loading code will use the default values.
	// Less data saved/loaded means smaller savegames.

	// write base class first
	CvPlayer::write(writer);

	writer.Write(PlayerSaveAI_distanceMap, m_distanceMap);
	writer.Write(PlayerSaveAI_DistanceMapDistance, m_iDistanceMapDistance, defaultDistanceMapDistance);
	writer.Write(PlayerSaveAI_AttackOddsChange, m_iAttackOddsChange, defaultAttackOddsChange);
	writer.Write(PlayerSaveAI_ExtraGoldTarget, m_iExtraGoldTarget,defaultExtraGoldTarget);
	writer.Write(PlayerSaveAI_AveragesCacheTurn, m_iAveragesCacheTurn, defaultAveragesCacheTurn);
	writer.Write(PlayerSaveAI_NextBuyUnit, m_eNextBuyUnit, defaultNextBuyUnit);
	writer.Write(PlayerSaveAI_NextBuyUnitAI, m_eNextBuyUnitAI, defaultNextBuyUnitAI);
	writer.Write(PlayerSaveAI_NextBuyUnitValue, m_iNextBuyUnitValue, defaultNextBuyUnitValue);
	writer.Write(PlayerSaveAI_NextBuyProfession, m_eNextBuyProfession, defaultNextBuyProfession);
	writer.Write(PlayerSaveAI_NextBuyProfessionUnit, m_eNextBuyProfessionUnit, defaultNextBuyProfessionUnit);
	writer.Write(PlayerSaveAI_NextBuyProfessionAI, m_eNextBuyProfessionAI, defaultNextBuyProfessionAI);
	writer.Write(PlayerSaveAI_NextBuyProfessionValue, m_iNextBuyProfessionValue, defaultNextBuyProfessionValue);
	writer.Write(PlayerSaveAI_TotalIncome, m_iTotalIncome, defaultTotalIncome);
	writer.Write(PlayerSaveAI_HurrySpending, m_iHurrySpending, defaultHurrySpending);
	writer.Write(PlayerSaveAI_ImmigrantSpending, m_iImmigrantSpending, defaultImmigrantSpending);
	writer.Write(PlayerSaveAI_EuropeYieldSpending, m_iEuropeYieldSpending, defaultEuropeYieldSpending);

	writer.Write(PlayerSaveAI_END);
}
