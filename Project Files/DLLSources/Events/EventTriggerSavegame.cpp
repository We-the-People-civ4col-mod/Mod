#include "EventTrigger.h"
#include "../CvSavegame.h"

// set the default values

const int default_iId = -1;
const EventTriggerTypes default_eTrigger = NO_EVENTTRIGGER;
const int default_iTurn = 0;
const PlayerTypes default_ePlayer = NO_PLAYER;
const int default_iCityId = -1;
const int default_iUnitId = -1;
const PlayerTypes default_eOtherPlayer = NO_PLAYER;
const int default_iOtherPlayerCityId = -1;
const BuildingTypes default_eBuilding = NO_BUILDING;

// 
enum SavegameVariableTypes
{
	EventTrigger_END,
	EventTrigger_iId,
	EventTrigger_eTrigger,
	EventTrigger_iTurn,
	EventTrigger_ePlayer,
	EventTrigger_iCityId,
	EventTrigger_iPlot,
	EventTrigger_iUnitId,
	EventTrigger_eOtherPlayer,
	EventTrigger_iOtherPlayerCityId,
	EventTrigger_eBuilding,
	EventTrigger_szText,
	EventTrigger_szGlobalText,
	EventTrigger_RandomNumberVector,

	NUM_EventTrigger_ENUM_VALUES,
};

const char* getSavedEnumNameEventTrigger(SavegameVariableTypes eType)
{
	switch (eType)
	{
	case EventTrigger_END: return "EventTrigger_END";
	case EventTrigger_iId: return "EventTrigger_iId";
	case EventTrigger_eTrigger: return "EventTrigger_eTrigger";
	case EventTrigger_iTurn: return "EventTrigger_iTurn";
	case EventTrigger_ePlayer: return "EventTrigger_ePlayer";
	case EventTrigger_iCityId: return "EventTrigger_iCityId";
	case EventTrigger_iPlot: return "EventTrigger_iPlot";
	case EventTrigger_iUnitId: return "EventTrigger_iUnitId";
	case EventTrigger_eOtherPlayer: return "EventTrigger_eOtherPlayer";
	case EventTrigger_iOtherPlayerCityId: return "EventTrigger_iOtherPlayerCityId";
	case EventTrigger_eBuilding: return "EventTrigger_eBuilding";
	case EventTrigger_szText: return "EventTrigger_szText";
	case EventTrigger_szGlobalText: return "EventTrigger_szGlobalText";
	case EventTrigger_RandomNumberVector: return "EventTrigger_RandomNumberVector";
	}
	FAssertMsg(0, "Missing case");
	return "";
}

int getNumSavedEnumValuesEventTrigger()
{
	return NUM_EventTrigger_ENUM_VALUES;
}

// assign everything to default values
void EventTriggeredData::resetSavedData()
{
	m_iId = default_iId;
	m_eTrigger = default_eTrigger;
	m_iTurn = default_iTurn;
	m_ePlayer = default_ePlayer;
	m_iCityId = default_iCityId;
	
	Coordinates coord;
	m_iPlotX = coord.x();
	m_iPlotY = coord.y();

	m_iUnitId = default_iUnitId;
	m_eOtherPlayer = default_eOtherPlayer;
	m_iOtherPlayerCityId = default_iOtherPlayerCityId;
	m_eBuilding = default_eBuilding;
	m_szText.clear();
	m_szGlobalText.clear();
	m_RandomNumbers.clear();
}

void EventTriggeredData::read(CvSavegameReader& reader)
{
	// Init data before load
	// This will ensure that all variables not included in the savegame will have default values
	resetSavedData();

	if (reader.getSavegameVersion() < 3)
	{
		// backward compatibility
		readVanilla(reader);
		return;
	}

	reader.AssignClassType(SAVEGAME_CLASS_EVENT_TRIGGER);

	// loop read all the variables
	// As long as each variable has a DealSavegameVariables "header", order doesn't matter.
	// Variables can be read in any order and any number of variables can be skipped.
	bool bContinue = true;
	while (bContinue)
	{
		SavegameVariableTypes eType;
		reader.Read(eType);

		switch (eType)
		{
		case EventTrigger_END:
			bContinue = false;
			break;
		case EventTrigger_iId: reader.Read(m_iId); break;
		case EventTrigger_eTrigger: reader.Read(m_eTrigger); break;
		case EventTrigger_iTurn: reader.Read(m_iTurn); break;
		case EventTrigger_ePlayer: reader.Read(m_ePlayer); break;
		case EventTrigger_iCityId: reader.Read(m_iCityId); break;
		case EventTrigger_iPlot:
		{
			Coordinates coord;
			reader.Read(coord);
			m_iPlotX = coord.x();
			m_iPlotY = coord.y();
		}
		break;
		case EventTrigger_iUnitId: reader.Read(m_iUnitId); break;
		case EventTrigger_eOtherPlayer: reader.Read(m_eOtherPlayer); break;
		case EventTrigger_iOtherPlayerCityId: reader.Read(m_iOtherPlayerCityId); break;
		case EventTrigger_eBuilding: reader.Read(m_eBuilding); break;
		case EventTrigger_szText: reader.Read(m_szText); break;
		case EventTrigger_szGlobalText: reader.Read(m_szGlobalText); break;
		case EventTrigger_RandomNumberVector: reader.Read(m_RandomNumbers); break;
		}

	}
}

void EventTriggeredData::write(CvSavegameWriter& writer) const
{
	writer.AssignClassType(SAVEGAME_CLASS_EVENT_TRIGGER);

	writer.Write(EventTrigger_iId, m_iId, default_iId);
	writer.Write(EventTrigger_eTrigger, m_eTrigger, default_eTrigger);
	writer.Write(EventTrigger_iTurn, m_iTurn, default_iTurn);
	writer.Write(EventTrigger_ePlayer, m_ePlayer, default_ePlayer);
	writer.Write(EventTrigger_iCityId, m_iCityId, default_iCityId);
	writer.Write(EventTrigger_iPlot, Coordinates(m_iPlotX, m_iPlotY));
	writer.Write(EventTrigger_iUnitId, m_iUnitId, default_iUnitId);
	writer.Write(EventTrigger_eOtherPlayer, m_eOtherPlayer, default_eOtherPlayer);
	writer.Write(EventTrigger_iOtherPlayerCityId, m_iOtherPlayerCityId, default_iOtherPlayerCityId);
	writer.Write(EventTrigger_eBuilding, m_eBuilding, default_eBuilding);
	writer.Write(EventTrigger_szText, m_szText);
	writer.Write(EventTrigger_szGlobalText, m_szGlobalText);
	writer.Write(EventTrigger_RandomNumberVector, m_RandomNumbers);
	
	writer.Write(EventTrigger_END);
}

void EventTriggeredData::readVanilla(CvSavegameReader& reader)
{
	reader.Read(m_iId);
	reader.Read(m_eTrigger);
	reader.Read(m_iTurn);
	reader.Read(m_ePlayer);
	reader.Read(m_iCityId);
	reader.Read(m_iPlotX);
	reader.Read(m_iPlotY);
	reader.Read(m_iUnitId);
	reader.Read(m_eOtherPlayer);
	reader.Read(m_iOtherPlayerCityId);
	reader.Read(m_eBuilding);
	reader.Read(m_szText);
	reader.Read(m_szGlobalText);
}

void CvSavegameReader::Read(EventTriggeredData::RandomContainer& container)
{
	Read(container.event);
	Read(container.number);
}

void CvSavegameWriter::Write(EventTriggeredData::RandomContainer container)
{
	Write(container.event);
	Write(container.number);
}
