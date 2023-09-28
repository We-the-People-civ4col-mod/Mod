#include "CvGameCoreDLL.h"
#include "CvInitCore.h"

#include "CvSavegame.h"

// set the default values
const int defaultID = 0;
const int defaultMissionTimer = 0;

const bool defaultForceUpdate = false;

const PlayerTypes defaultOwner = NO_PLAYER;
const ActivityTypes defaultActivityType = ACTIVITY_AWAKE;
const AutomateTypes defaultAutomateType = NO_AUTOMATE;


// 
enum SavegameVariableTypes
{
	SelectionGroupSave_END,
	SelectionGroupSave_ID,
	SelectionGroupSave_MissionTimer,
	SelectionGroupSave_ForceUpdate,
	SelectionGroupSave_Owner,
	SelectionGroupSave_ActivityType,
	SelectionGroupSave_AutomateType,
	SelectionGroupSave_units,
	SelectionGroupSave_missionQueue,
	SelectionGroupSave_TradeRoutes,
	NUM_SAVE_ENUM_VALUES,
};

const char* getSavedEnumNameSelectionGroup(SavegameVariableTypes eType)
{
	switch (eType)
	{
	case SelectionGroupSave_END: return "SelectionGroupSave_END";
	case SelectionGroupSave_ID: return "SelectionGroupSave_ID";
	case SelectionGroupSave_MissionTimer: return "SelectionGroupSave_MissionTimer";
	case SelectionGroupSave_ForceUpdate: return "SelectionGroupSave_ForceUpdate";
	case SelectionGroupSave_Owner: return "SelectionGroupSave_Owner";
	case SelectionGroupSave_ActivityType: return "SelectionGroupSave_ActivityType";
	case SelectionGroupSave_AutomateType: return "SelectionGroupSave_AutomateType";
	case SelectionGroupSave_units: return "SelectionGroupSave_units";
	case SelectionGroupSave_missionQueue: return "SelectionGroupSave_missionQueue";
	case SelectionGroupSave_TradeRoutes: return "SelectionGroupSave_TradeRoutes";
	}
	FAssertMsg(0, "Missing case");
	return "";
}

int getNumSavedEnumValuesSelectionGroup()
{
	return NUM_SAVE_ENUM_VALUES;
}

// assign everything to default values
void CvSelectionGroup::resetSavedData(int iID, PlayerTypes eOwner)
{
	m_iID=iID;
	m_iMissionTimer = defaultMissionTimer;
	m_bForceUpdate = defaultForceUpdate;
	m_eOwner = eOwner;
	m_eActivityType = defaultActivityType;
	m_eAutomateType = defaultAutomateType;
}

void CvSelectionGroup::read(CvSavegameReader reader)
{
	reader.AssignClassType(SAVEGAME_CLASS_SELECTIONGROUP);

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
		case SelectionGroupSave_END: bContinue = false; break;
		case SelectionGroupSave_ID: reader.Read(m_iID); break;
		case SelectionGroupSave_MissionTimer: reader.Read(m_iMissionTimer); break;
		case SelectionGroupSave_ForceUpdate: reader.Read(m_bForceUpdate); break;
		case SelectionGroupSave_Owner: reader.Read(m_eOwner); break;
		case SelectionGroupSave_ActivityType: reader.Read(m_eActivityType); break;
		case SelectionGroupSave_AutomateType: reader.Read(m_eAutomateType); break;
		case SelectionGroupSave_units: reader.Read(m_units); break;
		case SelectionGroupSave_missionQueue: reader.Read(m_missionQueue); break;
		case SelectionGroupSave_TradeRoutes:
			int iSize;
			reader.Read(iSize);
			for(int iI=0;iI<iSize;iI++)
			{
				int iRouteID;
				reader.Read(iRouteID);
				m_aTradeRoutes.insert(iRouteID);
			}
			break;
		}
	}
	
}

void CvSelectionGroup::write(CvSavegameWriter writer)
{
	LogIntentHelper helper(writer, "CvSelectionGroup");

	writer.AssignClassType(SAVEGAME_CLASS_SELECTIONGROUP);

	// Write the data.
	// Use WriteSwitch since it will automatically include WriteSwitch in the savegame.
	// Also it will not save anything if the variable and the default values are identical.
	// If nothing is saved, the loading code will use the default values.
	// Less data saved/loaded means smaller savegames.
	writer.Write(SelectionGroupSave_ID, m_iID, defaultID);
	writer.Write(SelectionGroupSave_MissionTimer, m_iMissionTimer, defaultMissionTimer);
	writer.Write(SelectionGroupSave_ForceUpdate, m_bForceUpdate, defaultForceUpdate);
	writer.Write(SelectionGroupSave_Owner, m_eOwner, defaultOwner);
	writer.Write(SelectionGroupSave_ActivityType, m_eActivityType, defaultActivityType);
	writer.Write(SelectionGroupSave_AutomateType, m_eAutomateType, defaultAutomateType);
	writer.Write(SelectionGroupSave_units, m_units);
	writer.Write(SelectionGroupSave_missionQueue, m_missionQueue);
	int iSize = m_aTradeRoutes.size();
	if(iSize>0)
	{
		writer.Write(SelectionGroupSave_TradeRoutes);
		writer.Write(iSize);
		for(std::set<int>::iterator it = m_aTradeRoutes.begin(); it != m_aTradeRoutes.end(); ++it)
		{
			writer.Write(*it);
		}
	}
	writer.Write(SelectionGroupSave_END);
}
