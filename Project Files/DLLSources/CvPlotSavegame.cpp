#include "CvGameCoreDLL.h"

#include "CvSavegame.h"

// set the default values
enum SavegameVariableTypes
{
	Save_END,
};

// assign everything to default values
void CvPlot::resetSavedData()
{
	
}

void CvPlot::read(CvSavegameReader& reader)
{
	// Init data before load
	// This will ensure that all variables not included in the savegame will have default values
	reset();

	// loop read all the variables
	// As long as each variable has a UnitSavegameVariables "header", order doesn't matter.
	// Variables can be read in any order and any number of variables can be skipped.
	bool bContinue = true;
	while (bContinue)
	{
		SavegameVariableTypes eType = reader.ReadSwitch();

		switch (eType)
		{
		case Save_END:
			bContinue = false;
			break;

		}
	}
	
	// Loading done. Set up the cache (if any).

}

void CvPlot::write(CvSavegameWriter& writer)
{
	// Write the data.
	// Use WriteSwitch since it will automatically include WriteSwitch in the savegame.
	// Also it will not save anything if the variable and the default values are identical.
	// If nothing is saved, the loading code will use the default values.
	// Less data saved/loaded means smaller savegames.

	writer.Write(Save_END);
}
