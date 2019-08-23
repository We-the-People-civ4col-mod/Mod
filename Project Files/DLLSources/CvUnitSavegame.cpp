#include "CvGameCoreDLL.h"

#include "CvSavegame.h"

// set the default values
const UnitTypes defaultUnitType = NO_UNIT;
const DirectionTypes defaultFacingDirection = DIRECTION_SOUTH;

// 
enum SavegameVariableTypes
{
	UnitSave_END,
	UnitSave_Direction,
	UnitSave_UnitType,
};

// assign everything to default values
void CvUnit::resetNew()
{
	m_eFacingDirection = defaultFacingDirection;
	m_eUnitType = defaultUnitType;
}

void CvUnit::read(CvSavegameReader* pStream)
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
		SavegameVariableTypes eType = pStream->ReadSwitch();

		switch (eType)
		{
		case UnitSave_END:
			bContinue = false;
			break;

		case UnitSave_Direction:
			pStream->Read(m_eFacingDirection);
			break;

		case UnitSave_UnitType:
			pStream->Read(m_eUnitType);
			break;
		}
	}
	
	// The unit is loaded. Now set up the cache according to the read data.

	FAssert(NO_UNIT != m_eUnitType);
	m_pUnitInfo = (NO_UNIT != m_eUnitType) ? &GC.getUnitInfo(m_eUnitType) : NULL;
}

void CvUnit::write(CvSavegameWriter* pStream)
{
	// Write the data.
	// Use WriteSwitch since it will automatically include WriteSwitch in the savegame.
	// Also it will not save anything if the variable and the default values are identical.
	// If nothing is saved, the loading code will use the default values.
	// Less data saved/loaded means smaller savegames.

	pStream->Write(UnitSave_Direction, m_eFacingDirection, defaultFacingDirection);
	pStream->Write(UnitSave_UnitType, m_eUnitType, defaultUnitType);

	pStream->Write(UnitSave_END);
}
