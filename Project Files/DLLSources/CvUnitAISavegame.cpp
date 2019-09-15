#include "CvGameCoreDLL.h"

#include "CvSavegame.h"

// set the default values
enum SavegameVariableTypes
{
	Save_END,

	NUM_SAVE_ENUM_VALUES,
};

int getNumSavedEnumValuesUnitAI()
{
	return NUM_SAVE_ENUM_VALUES;
}

// assign everything to default values
void CvUnitAI::resetSavedData()
{
	
}

void CvUnitAI::read(CvSavegameReader reader)
{
	reader.AssignClassType(SAVEGAME_CLASS_UNIT_AI);

	// Init data before load
	// This will ensure that all variables not included in the savegame will have default values
	resetSavedData();

	// read base class. It's always placed first
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
		case Save_END:
			bContinue = false;
			break;

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

	writer.Write(Save_END);
}
