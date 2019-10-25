#include "CvGameCoreDLL.h"
#include "CvInitCore.h"

#include "CvSavegame.h"

// set the default values

const int defaultId = 0;
const YieldTypes defaultYield = NO_YIELD;

// 
enum SavegameVariableTypes
{
	TradeRouteSave_END,
	TradeRouteSave_Id,
	TradeRouteSave_SourceCity,
	TradeRouteSave_DestinationCity,
	TradeRouteSave_Yield,
	NUM_SAVE_ENUM_VALUES,
};

const char* getSavedEnumNameTradeRoute(SavegameVariableTypes eType)
{
	switch (eType)
	{
	case TradeRouteSave_END: return "TradeRouteSave_END";
	case TradeRouteSave_Id: return "TradeRouteSave_Id";
	case TradeRouteSave_SourceCity: return "TradeRouteSave_SourceCity";
	case TradeRouteSave_DestinationCity: return "TradeRouteSave_DestinationCity";
	case TradeRouteSave_Yield: return "TradeRouteSave_Yield";
	}
	return "";
}

int getNumSavedEnumValuesTradeRoute()
{
	return NUM_SAVE_ENUM_VALUES;
}

// assign everything to default values
void CvTradeRoute::resetSavedData()
{
	m_iId = defaultId;
	m_kSourceCity.reset();
	m_kDestinationCity.reset();
	m_eYield = defaultYield;
}

void CvTradeRoute::read(CvSavegameReader reader)
{
	reader.AssignClassType(SAVEGAME_CLASS_TRADEROUTE);

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
		case TradeRouteSave_END: bContinue = false; break;
		case TradeRouteSave_Id: reader.Read(m_iId); break;
		case TradeRouteSave_SourceCity: reader.Read(m_kSourceCity); break;
		case TradeRouteSave_DestinationCity: reader.Read(m_kDestinationCity); break;
		case TradeRouteSave_Yield : reader.Read(m_eYield); break;

		}
	}
	
}

void CvTradeRoute::write(CvSavegameWriter writer)
{
	writer.AssignClassType(SAVEGAME_CLASS_TRADEROUTE);

	// Write the data.
	// Use WriteSwitch since it will automatically include WriteSwitch in the savegame.
	// Also it will not save anything if the variable and the default values are identical.
	// If nothing is saved, the loading code will use the default values.
	// Less data saved/loaded means smaller savegames.
	writer.Write(TradeRouteSave_Id, m_iId, defaultId);
	writer.Write(TradeRouteSave_SourceCity, m_kSourceCity);
	writer.Write(TradeRouteSave_DestinationCity, m_kDestinationCity);
	writer.Write(TradeRouteSave_Yield, m_eYield, defaultYield);
	
	writer.Write(TradeRouteSave_END);
}
