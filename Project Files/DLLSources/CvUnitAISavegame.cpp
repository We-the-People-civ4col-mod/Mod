#include "CvGameCoreDLL.h"

#include "CvSavegame.h"

// set the default values

const int defaultBirthmark = 0;
const int defaultMovePriority = 0;
const int defaultLastAIChangeTurn = 0;
const UnitAITypes defaultUnitAIType = NO_UNITAI;
const UnitAIStates defaultUnitAIState = UNITAI_STATE_DEFAULT;
const ProfessionTypes defaultOldProfession = NO_PROFESSION;
const ProfessionTypes defaultIdealProfessionCache = INVALID_PROFESSION;
const int defaultAutomatedAbortTurn = -1;

enum SavegameVariableTypes
{
	UnitSaveAI_END,
	UnitSaveAI_Birthmark,
	UnitSaveAI_MovePriority,
	UnitSaveAI_LastAIChangeTurn,
	UnitSaveAI_UnitAIType,
	UnitSaveAI_UnitAIState,
	UnitSaveAI_OldProfession,
	UnitSaveAI_IdealProfessionCache,
	UnitSaveAI_AutomatedAbortTurn,
	NUM_SAVE_ENUM_VALUES,
};

const char* getSavedEnumNameUnitAi(SavegameVariableTypes eType)
{
	switch (eType)
	{
	case UnitSaveAI_END: return "UnitSaveAI_END";
	case UnitSaveAI_Birthmark: return "UnitSaveAI_Birthmark";
	case UnitSaveAI_MovePriority: return "UnitSaveAI_MovePriority";
	case UnitSaveAI_LastAIChangeTurn: return "UnitSaveAI_LastAIChangeTurn";
	case UnitSaveAI_UnitAIType: return "UnitSaveAI_UnitAIType";
	case UnitSaveAI_UnitAIState: return "UnitSaveAI_UnitAIState";
	case UnitSaveAI_OldProfession: return "UnitSaveAI_OldProfession";
	case UnitSaveAI_IdealProfessionCache: return "UnitSaveAI_IdealProfessionCache";
	case UnitSaveAI_AutomatedAbortTurn: return "UnitSaveAI_AutomatedAbortTurn";

}
	return "";
}

int getNumSavedEnumValuesUnitAI()
{
	return NUM_SAVE_ENUM_VALUES;
}

// assign everything to default values
void CvUnitAI::AI_resetSavedData()
{
	m_iBirthmark = defaultBirthmark;
	m_iMovePriority =  defaultMovePriority;
	m_iLastAIChangeTurn =  defaultLastAIChangeTurn;
	m_eUnitAIType =  defaultUnitAIType;
	m_eUnitAIState =  defaultUnitAIState;
	m_eOldProfession =  defaultOldProfession;
	m_eIdealProfessionCache =  defaultIdealProfessionCache;
	m_iAutomatedAbortTurn =  defaultAutomatedAbortTurn;
}

void CvUnitAI::read(CvSavegameReader reader)
{
	reader.AssignClassType(SAVEGAME_CLASS_UNIT_AI);

	// Init data before load
	// This will ensure that all variables not included in the savegame will have default values
	// read base class. It's always placed first because it also resets all the data
	CvUnit::read(reader);

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
		case UnitSaveAI_END: bContinue = false; break;
		case UnitSaveAI_Birthmark: reader.Read(m_iBirthmark); break;
		case UnitSaveAI_MovePriority: reader.Read(m_iMovePriority); break;
		case UnitSaveAI_LastAIChangeTurn: reader.Read(m_iLastAIChangeTurn); break;
		case UnitSaveAI_UnitAIType: reader.Read(m_eUnitAIType); break;
		case UnitSaveAI_UnitAIState: reader.Read(m_eUnitAIState); break;
		case UnitSaveAI_OldProfession: reader.Read(m_eOldProfession); break;
		case UnitSaveAI_IdealProfessionCache: reader.Read(m_eIdealProfessionCache); break;
		case UnitSaveAI_AutomatedAbortTurn: reader.Read(m_iAutomatedAbortTurn); break;
		}
	}
	
	// Loading done. Set up the cache (if any).

}

void CvUnitAI::write(CvSavegameWriter writer)
{
	writer.AssignClassType(SAVEGAME_CLASS_UNIT_AI);

	// Write the data.
	// Use WriteSwitch since it will automatically include WriteSwitch in the savegame.
	// Also it will not save anything if the variable and the default values are identical.
	// If nothing is saved, the loading code will use the default values.
	// Less data saved/loaded means smaller savegames.

	// write base class first
	CvUnit::write(writer);

	writer.Write(UnitSaveAI_Birthmark, m_iBirthmark, defaultBirthmark);
	writer.Write(UnitSaveAI_MovePriority, m_iMovePriority, defaultMovePriority);
	writer.Write(UnitSaveAI_LastAIChangeTurn, m_iLastAIChangeTurn, defaultLastAIChangeTurn);
	writer.Write(UnitSaveAI_UnitAIType, m_eUnitAIType, defaultUnitAIType);
	writer.Write(UnitSaveAI_UnitAIState, m_eUnitAIState, defaultUnitAIState);
	writer.Write(UnitSaveAI_OldProfession, m_eOldProfession, defaultOldProfession);
	writer.Write(UnitSaveAI_IdealProfessionCache, m_eIdealProfessionCache, defaultIdealProfessionCache);
	writer.Write(UnitSaveAI_AutomatedAbortTurn, m_iAutomatedAbortTurn, defaultAutomatedAbortTurn);
	writer.Write(UnitSaveAI_END);
}
