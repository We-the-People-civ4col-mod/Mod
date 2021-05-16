#include "CvGameCoreDLL.h"
#include "CvInitCore.h"

#include "CvSavegame.h"

// set the default values
const TeamTypes defaultWorstEnemy = NO_TEAM;

// 
enum SavegameVariableTypes
{
	TeamAISave_END,
	TeamAISave_EnemyCityDistance,
	TeamAISave_EnemyUnitDistance,

	TeamAISave_WarPlanStateCounter,
	TeamAISave_AtWarCounter,
	TeamAISave_AtPeaceCounter,
	TeamAISave_HasMetCounter,
	TeamAISave_OpenBordersCounter,
	TeamAISave_DefensivePactCounter,
	TeamAISave_ShareWarCounter,
	TeamAISave_WarSuccess,
	TeamAISave_EnemyPeacetimeTradeValue,
	TeamAISave_EnemyPeacetimeGrantValue,
	TeamAISave_Damages,

	TeamAISave_WarPlan,
	TeamAISave_WorstEnemy,
	NUM_SAVE_ENUM_VALUES,
};

const char* getSavedEnumNameTeamAI(SavegameVariableTypes eType)
{
	switch (eType)
	{
	case TeamAISave_END: return "TeamAISave_END";
	case TeamAISave_EnemyCityDistance: return "TeamAISave_EnemyCityDistance";
	case TeamAISave_EnemyUnitDistance: return "TeamAISave_EnemyUnitDistance";

	case TeamAISave_WarPlanStateCounter: return "TeamAISave_WarPlanStateCounter";
	case TeamAISave_AtWarCounter: return "TeamAISave_AtWarCounter";
	case TeamAISave_AtPeaceCounter: return "TeamAISave_AtPeaceCounter";
	case TeamAISave_HasMetCounter: return "TeamAISave_HasMetCounter";
	case TeamAISave_OpenBordersCounter: return "TeamAISave_OpenBordersCounter";
	case TeamAISave_DefensivePactCounter: return "TeamAISave_DefensivePactCounter";
	case TeamAISave_ShareWarCounter: return "TeamAISave_ShareWarCounter";
	case TeamAISave_WarSuccess: return "TeamAISave_WarSuccess";
	case TeamAISave_EnemyPeacetimeTradeValue: return "TeamAISave_EnemyPeacetimeTradeValue";
	case TeamAISave_EnemyPeacetimeGrantValue: return "TeamAISave_EnemyPeacetimeGrantValue";
	case TeamAISave_Damages: return "TeamAISave_Damages";

	case TeamAISave_WarPlan: return "TeamAISave_WarPlan";
	case TeamAISave_WorstEnemy: return "TeamAISave_WorstEnemy";
	}
	return "";
}

int getNumSavedEnumValuesTeamAI()
{
	return NUM_SAVE_ENUM_VALUES;
}

// assign everything to default values
void CvTeamAI::AI_resetSavedData()
{
	m_aiEnemyCityDistance.clear();
	m_aiEnemyUnitDistance.clear();

	m_em_iWarPlanStateCounter.reset();
	m_em_iAtWarCounter.reset();
	m_em_iAtPeaceCounter.reset();
	m_em_iHasMetCounter.reset();
	m_em_iOpenBordersCounter.reset();
	m_em_iDefensivePactCounter.reset();
	m_em_iShareWarCounter.reset();
	m_em_iWarSuccess.reset();
	m_em_iEnemyPeacetimeTradeValue.reset();
	m_em_iEnemyPeacetimeGrantValue.reset();
	m_em_iDamages.reset();

	m_em_eWarPlan.reset();
	m_eWorstEnemy=defaultWorstEnemy;
}

void CvTeamAI::read(CvSavegameReader reader)
{
	reader.AssignClassType(SAVEGAME_CLASS_TEAM_AI);

	// Init data before load
	// This will ensure that all variables not included in the savegame will have default values
	AI_resetSavedData();

	// read base class. It's always placed first
	CvTeam::read(reader);

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
		case TeamAISave_END: bContinue = false; break;
		case TeamAISave_EnemyCityDistance: reader.Read(m_aiEnemyCityDistance); break;
		case TeamAISave_EnemyUnitDistance: reader.Read(m_aiEnemyUnitDistance); break;

		case TeamAISave_WarPlanStateCounter: reader.Read(m_em_iWarPlanStateCounter); break;
		case TeamAISave_AtWarCounter: reader.Read(m_em_iAtWarCounter); break;
		case TeamAISave_AtPeaceCounter: reader.Read(m_em_iAtPeaceCounter); break;
		case TeamAISave_HasMetCounter: reader.Read(m_em_iHasMetCounter); break;
		case TeamAISave_OpenBordersCounter: reader.Read(m_em_iOpenBordersCounter); break;
		case TeamAISave_DefensivePactCounter: reader.Read(m_em_iDefensivePactCounter); break;
		case TeamAISave_ShareWarCounter: reader.Read(m_em_iShareWarCounter); break;
		case TeamAISave_WarSuccess: reader.Read(m_em_iWarSuccess); break;
		case TeamAISave_EnemyPeacetimeTradeValue: reader.Read(m_em_iEnemyPeacetimeTradeValue); break;
		case TeamAISave_EnemyPeacetimeGrantValue: reader.Read(m_em_iEnemyPeacetimeGrantValue); break;
		case TeamAISave_Damages: reader.Read(m_em_iDamages); break;

		case TeamAISave_WarPlan: reader.Read(m_em_eWarPlan); break;
		case TeamAISave_WorstEnemy: reader.Read(m_eWorstEnemy); break;
		}
	}
	
}

void CvTeamAI::write(CvSavegameWriter writer)
{
	writer.AssignClassType(SAVEGAME_CLASS_TEAM_AI);

	// Write the data.
	// Use WriteSwitch since it will automatically include WriteSwitch in the savegame.
	// Also it will not save anything if the variable and the default values are identical.
	// If nothing is saved, the loading code will use the default values.
	// Less data saved/loaded means smaller savegames.
	// write base class first
	CvTeam::write(writer);
	writer.Write(TeamAISave_EnemyCityDistance, m_aiEnemyCityDistance);
	writer.Write(TeamAISave_EnemyUnitDistance, m_aiEnemyUnitDistance);

	writer.Write(TeamAISave_WarPlanStateCounter, m_em_iWarPlanStateCounter);
	writer.Write(TeamAISave_AtWarCounter, m_em_iAtWarCounter);
	writer.Write(TeamAISave_AtPeaceCounter, m_em_iAtPeaceCounter);
	writer.Write(TeamAISave_HasMetCounter, m_em_iHasMetCounter);
	writer.Write(TeamAISave_OpenBordersCounter, m_em_iOpenBordersCounter);
	writer.Write(TeamAISave_DefensivePactCounter, m_em_iDefensivePactCounter);
	writer.Write(TeamAISave_ShareWarCounter, m_em_iShareWarCounter);
	writer.Write(TeamAISave_WarSuccess, m_em_iWarSuccess);
	writer.Write(TeamAISave_EnemyPeacetimeTradeValue, m_em_iEnemyPeacetimeTradeValue);
	writer.Write(TeamAISave_EnemyPeacetimeGrantValue, m_em_iEnemyPeacetimeGrantValue);
	writer.Write(TeamAISave_Damages, m_em_iDamages);

	writer.Write(TeamAISave_WarPlan, m_em_eWarPlan);
	writer.Write(TeamAISave_WorstEnemy, m_eWorstEnemy, defaultWorstEnemy);
	writer.Write(TeamAISave_END);
}
