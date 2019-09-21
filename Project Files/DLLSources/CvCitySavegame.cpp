#include "CvGameCoreDLL.h"

#include "CvSavegame.h"

// set the default values


// 
enum SavegameVariableTypes
{
	CitySave_END,

	NUM_SAVE_ENUM_VALUES,
};

int getNumSavedEnumValuesCity()
{
	return NUM_SAVE_ENUM_VALUES;
}

// assign everything to default values
void CvCity::resetSavedData()
{

}

void CvCity::read(CvSavegameReader reader)
{
	reader.AssignClassType(SAVEGAME_CLASS_CITY);

	// Init data before load
	// This will ensure that all variables not included in the savegame will have default values
	reset();

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
		case CitySave_END:
			bContinue = false;
			break;

		}
	}
	
}

void CvCity::write(CvSavegameWriter writer)
{
	writer.AssignClassType(SAVEGAME_CLASS_CITY);

	// Write the data.
	// Use WriteSwitch since it will automatically include WriteSwitch in the savegame.
	// Also it will not save anything if the variable and the default values are identical.
	// If nothing is saved, the loading code will use the default values.
	// Less data saved/loaded means smaller savegames.


	writer.Write(CitySave_END);
}
