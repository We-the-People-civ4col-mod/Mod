#include "CvGameCoreDLL.h"

#include "CvSavegame.h"



// set the default values
const int defaultPad = 0;


enum SavegameVariableTypes
{
	GameSaveAI_END,
	GameSaveAI_Pad,

	NUM_SAVE_ENUM_VALUES,
};

const char* getSavedEnumNameGameAi(SavegameVariableTypes eType)
{
	switch (eType)
	{
	case GameSaveAI_END: return "GameSaveAI_END";
	case GameSaveAI_Pad: return "GameSaveAI_Pad";

	}
	FAssertMsg(0, "Missing case");
	return "";
}

int getNumSavedEnumValuesGameAI()
{
	return NUM_SAVE_ENUM_VALUES;
}

// assign everything to default values
void CvGameAI::AI_resetSavedData()
{
	m_iPad = defaultPad;
}

void CvGameAI::read(CvSavegameReader reader)
{
	reader.AssignClassType(SAVEGAME_CLASS_GAME_AI);
	// Init data before load
	// read base class. It's always placed first
	CvGame::read(reader);

	// loop read all the variables
	// As long as each variable has a GameSavegameVariables "header", order doesn't matter.
	// Variables can be read in any order and any number of variables can be skipped.
	bool bContinue = true;
	while (bContinue)
	{
		SavegameVariableTypes eType;
		reader.Read(eType);

		switch (eType)
		{
		case GameSaveAI_END: bContinue = false; break;
		case GameSaveAI_Pad: reader.Read(m_iPad); break;
		}
	}
	
	// Loading done. Set up the cache (if any).

}

void CvGameAI::write(CvSavegameWriter writer)
{
	writer.AssignClassType(SAVEGAME_CLASS_GAME_AI);

	// Write the data.
	// Use WriteSwitch since it will automatically include WriteSwitch in the savegame.
	// Also it will not save anything if the variable and the default values are identical.
	// If nothing is saved, the loading code will use the default values.
	// Less data saved/loaded means smaller savegames.

	// write base class first
	CvGame::write(writer);
	
	writer.Write(GameSaveAI_Pad, m_iPad, defaultPad);

	writer.Write(GameSaveAI_END);
}
