#include "CvGameCoreDLL.h"

#include "CvSavegame.h"

// set the default values
enum SavegameVariableTypes
{
	Save_END,

	NUM_SAVE_ENUM_VALUES,
};

int getNumSavedEnumValuesCityAI()
{
	return NUM_SAVE_ENUM_VALUES;
}

// assign everything to default values
void CvCityAI::resetSavedData()
{
	
}

void CvCityAI::read(CvSavegameReader reader)
{
	reader.AssignClassType(SAVEGAME_CLASS_CITY_AI);

	// Init data before load
	// This will ensure that all variables not included in the savegame will have default values
	resetSavedData();

	// read base class. It's always placed first
	CvCity::read(reader);

	// loop read all the variables
	// As long as each variable has a CitySavegameVariables "header", order doesn't matter.
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

void CvCityAI::write(CvSavegameWriter writer)
{
	writer.AssignClassType(SAVEGAME_CLASS_CITY_AI);

	// Write the data.
	// Use WriteSwitch since it will automatically include WriteSwitch in the savegame.
	// Also it will not save anything if the variable and the default values are identical.
	// If nothing is saved, the loading code will use the default values.
	// Less data saved/loaded means smaller savegames.

	// write base class first
	CvCity::write(writer);

	writer.Write(Save_END);
}
