#include "CvGameCoreDLL.h"
#include "CvInitCore.h"

#include "CvSavegame.h"

// set the default values
const int defaultMissionAIX = INVALID_PLOT_COORD;
const int defaultMissionAIY = INVALID_PLOT_COORD;
const bool defaultForceSeparate = false;
const MissionAITypes defaultMissionAIType = NO_MISSIONAI;
const bool defaultGroupAttack = false;
const int defaultGroupAttackX = -1;
const int defaultGroupAttackY = -1;


// 
enum SavegameVariableTypes
{
	SelectionGroupAISave_END,
	SelectionGroupAISave_MissionAIX,
	SelectionGroupAISave_MissionAIY,
	SelectionGroupAISave_ForceSeparate,
	SelectionGroupAISave_MissionAIType,
	SelectionGroupAISave_missionAIUnit,
	SelectionGroupAISave_GroupAttack,
	SelectionGroupAISave_GroupAttackX,
	SelectionGroupAISave_GroupAttackY,
	NUM_SAVE_ENUM_VALUES,
};

const char* getSavedEnumNameSelectionGroupAi(SavegameVariableTypes eType)
{
	switch (eType)
	{
	case SelectionGroupAISave_END: return "SelectionGroupAISave_END";
	case SelectionGroupAISave_MissionAIX: return "SelectionGroupAISave_MissionAIX";
	case SelectionGroupAISave_MissionAIY: return "SelectionGroupAISave_MissionAIY";
	case SelectionGroupAISave_ForceSeparate: return "SelectionGroupAISave_ForceSeparate";
	case SelectionGroupAISave_MissionAIType: return "SelectionGroupAISave_MissionAIType";
	case SelectionGroupAISave_missionAIUnit: return "SelectionGroupAISave_missionAIUnit";
	case SelectionGroupAISave_GroupAttack: return "SelectionGroupAISave_GroupAttack";
	case SelectionGroupAISave_GroupAttackX: return "SelectionGroupAISave_GroupAttackX";
	case SelectionGroupAISave_GroupAttackY: return "SelectionGroupAISave_GroupAttackY";
	}
	FAssertMsg(0, "Missing case");
	return "";
}

int getNumSavedEnumValuesSelectionGroupAi()
{
	return NUM_SAVE_ENUM_VALUES;
}

// assign everything to default values
void CvSelectionGroupAI::AI_resetSavedData()
{
	m_iMissionAIX = defaultMissionAIX;
	m_iMissionAIY = defaultMissionAIY;
	m_bForceSeparate = defaultForceSeparate;
	m_eMissionAIType = defaultMissionAIType;
	m_missionAIUnit.reset();
	m_bGroupAttack = defaultGroupAttack;
	m_iGroupAttackX = defaultGroupAttackX;
	m_iGroupAttackY = defaultGroupAttackY;
}

void CvSelectionGroupAI::read(CvSavegameReader reader)
{
	reader.AssignClassType(SAVEGAME_CLASS_SELECTIONGROUP_AI);

	// Init data before load
	// This will ensure that all variables not included in the savegame will have default values
	reset();
	// read base class. It's always placed first
	CvSelectionGroup::read(reader);
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
		case SelectionGroupAISave_END: bContinue = false; break;
		case SelectionGroupAISave_MissionAIX: reader.Read(m_iMissionAIX); break;
		case SelectionGroupAISave_MissionAIY: reader.Read(m_iMissionAIY); break;
		case SelectionGroupAISave_ForceSeparate: reader.Read(m_bForceSeparate); break;
		case SelectionGroupAISave_MissionAIType: reader.Read(m_eMissionAIType); break;
		case SelectionGroupAISave_missionAIUnit: reader.Read(m_missionAIUnit); break;
		case SelectionGroupAISave_GroupAttack: reader.Read(m_bGroupAttack); break;
		case SelectionGroupAISave_GroupAttackX: reader.Read(m_iGroupAttackX); break;
		case SelectionGroupAISave_GroupAttackY: reader.Read(m_iGroupAttackY); break;
		}
	}
	
}

void CvSelectionGroupAI::write(CvSavegameWriter writer)
{
	writer.AssignClassType(SAVEGAME_CLASS_SELECTIONGROUP_AI);

	// Write the data.
	// Use WriteSwitch since it will automatically include WriteSwitch in the savegame.
	// Also it will not save anything if the variable and the default values are identical.
	// If nothing is saved, the loading code will use the default values.
	// Less data saved/loaded means smaller savegames.
	// write base class first
	CvSelectionGroup::write(writer);
	writer.Write(SelectionGroupAISave_MissionAIX, m_iMissionAIX, defaultMissionAIX);
	writer.Write(SelectionGroupAISave_MissionAIY, m_iMissionAIY, defaultMissionAIY);
	writer.Write(SelectionGroupAISave_ForceSeparate, m_bForceSeparate, defaultForceSeparate);
	writer.Write(SelectionGroupAISave_MissionAIType, m_eMissionAIType, defaultMissionAIType);
	writer.Write(SelectionGroupAISave_missionAIUnit, m_missionAIUnit);
	writer.Write(SelectionGroupAISave_GroupAttack, m_bGroupAttack, defaultGroupAttack);
	writer.Write(SelectionGroupAISave_GroupAttackX, m_iGroupAttackX, defaultGroupAttackX);
	writer.Write(SelectionGroupAISave_GroupAttackY, m_iGroupAttackY, defaultGroupAttackY);
	writer.Write(SelectionGroupAISave_END);
}
