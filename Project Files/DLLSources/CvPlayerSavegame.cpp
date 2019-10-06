#include "CvGameCoreDLL.h"

#include "CvSavegame.h"

// set the default values


// 
enum SavegameVariableTypes
{
	PlayerSave_END,


	NUM_SAVE_ENUM_VALUES,
};

const char* getSavedEnumNamePlayer(SavegameVariableTypes eType)
{
	switch (eType)
	{
	case PlayerSave_END: return "PlayerSave_END";
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

	

	writer.Write(PlayerSave_END);
}
