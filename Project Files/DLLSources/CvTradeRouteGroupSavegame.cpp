#include "CvGameCoreDLL.h"
#include "CvInitCore.h"

#include "CvSavegame.h"

// set the default values

const int defaultId = 0;
const YieldTypes defaultYield = NO_YIELD;

// 
enum SavegameVariableTypes
{
	TradeRouteGroupSave_END,
	TradeRouteGroupSave_Id,
	TradeRouteGroupSave_Name,
	TradeRouteGroupSave_Routes,
	NUM_SAVE_ENUM_VALUES,
};

const char* getSavedEnumNameTradeRouteGroup(SavegameVariableTypes eType)
{
	switch (eType)
	{
	case TradeRouteGroupSave_END: return "TradeRouteGroupSave_END";
	case TradeRouteGroupSave_Id: return "TradeRouteGroupSave_Id";
	case TradeRouteGroupSave_Name: return "TradeRouteGroupSave_Name";
	case TradeRouteGroupSave_Routes: return "TradeRouteGroupSave_Routes";
	}
	return "";
}

int getNumSavedEnumValuesTradeRouteGroup()
{
	return NUM_SAVE_ENUM_VALUES;
}

// assign everything to default values
void CvTradeRouteGroup::resetSavedData()
{
	m_iId = defaultId;
	m_sName.clear();
	m_Routes.clear();
}

void CvTradeRouteGroup::read(CvSavegameReader reader)
{
	reader.AssignClassType(SAVEGAME_CLASS_TRADEROUTEGROUP);

	// Init data before load
	// This will ensure that all variables not included in the savegame will have default values
	resetSavedData();

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
		case TradeRouteGroupSave_END: bContinue = false; break;
		case TradeRouteGroupSave_Id: reader.Read(m_iId); break;
		case TradeRouteGroupSave_Name: reader.Read(m_sName); break;
		case TradeRouteGroupSave_Routes: reader.Read(m_Routes); break;

		}
	}
	
}

void CvTradeRouteGroup::write(CvSavegameWriter writer)
{
	writer.AssignClassType(SAVEGAME_CLASS_TRADEROUTEGROUP);

	// Write the data.
	// Use WriteSwitch since it will automatically include WriteSwitch in the savegame.
	// Also it will not save anything if the variable and the default values are identical.
	// If nothing is saved, the loading code will use the default values.
	// Less data saved/loaded means smaller savegames.
	writer.Write(TradeRouteGroupSave_Id, m_iId, defaultId);
	writer.Write(TradeRouteGroupSave_Name, m_sName);
	writer.Write(TradeRouteGroupSave_Routes, m_Routes);
	
	writer.Write(TradeRouteGroupSave_END);
}
