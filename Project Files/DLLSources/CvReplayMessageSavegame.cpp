#include "CvGameCoreDLL.h"

#include "CvSavegame.h"
#include "CvReplayMessage.h"

// set the default values

const int defaultTurn = 0;
const ReplayMessageTypes defaultType = NO_REPLAY_MESSAGE;
const int defaultPlotX = -1;
const int defaultPlotY = -1;
const PlayerTypes defaultPlayer = NO_PLAYER;
const ColorTypes defaultColor = NO_COLOR;

// 
enum SavegameVariableTypes
{
	ReplayMessageSave_END,
	ReplayMessageSave_Turn,
	ReplayMessageSave_Type,
	ReplayMessageSave_PlotX,
	ReplayMessageSave_PlotY,
	ReplayMessageSave_Player,
	ReplayMessageSave_Text,
	ReplayMessageSave_Color,

	NUM_ReplayMessageSAVE_ENUM_VALUES,
};

const char* getSavedEnumNameReplayMessage(SavegameVariableTypes eType)
{
	switch (eType)
	{
		case ReplayMessageSave_END: return "ReplayMessageSave_END";
		case ReplayMessageSave_Turn: return "ReplayMessageSave_Turn";
		case ReplayMessageSave_Type: return "ReplayMessageSave_Type";
		case ReplayMessageSave_PlotX: return "ReplayMessageSave_PlotX";
		case ReplayMessageSave_PlotY: return "ReplayMessageSave_PlotY";
		case ReplayMessageSave_Player: return "ReplayMessageSave_Player";
		case ReplayMessageSave_Text: return "ReplayMessageSave_Text";
		case ReplayMessageSave_Color: return "ReplayMessageSave_Color";
	}
	FAssertMsg(0, "Missing case");
	return "";
}

int getNumSavedEnumValuesReplayMessage()
{
	return NUM_ReplayMessageSAVE_ENUM_VALUES;
}

// assign everything to default values
void CvReplayMessage::resetSavedData()
{
	m_iTurn = defaultTurn;
	m_eType = defaultType;
	m_iPlotX = defaultPlotX;
	m_iPlotY = defaultPlotY;
	m_ePlayer = defaultPlayer;
	m_szText.clear();
	m_eColor = defaultColor;
}

void CvReplayMessage::read(CvSavegameReader reader)
{
	reader.AssignClassType(SAVEGAME_CLASS_REPLAYMESSAGE);

	// Init data before load
	// This will ensure that all variables not included in the savegame will have default values
	resetSavedData();
	// loop read all the variables
	// As long as each variable has a ReplayMessageSavegameVariables "header", order doesn't matter.
	// Variables can be read in any order and any number of variables can be skipped.
	bool bContinue = true;
	while (bContinue)
	{
		SavegameVariableTypes eType;
		reader.Read(eType);

		switch (eType)
		{
		case ReplayMessageSave_END:
			bContinue = false;
			break;
		case ReplayMessageSave_Turn: reader.Read(m_iTurn); break;
		case ReplayMessageSave_Type: reader.Read(m_eType); break;
		case ReplayMessageSave_PlotX: reader.Read(m_iPlotX); break;
		case ReplayMessageSave_PlotY: reader.Read(m_iPlotY); break;
		case ReplayMessageSave_Player: reader.Read(m_ePlayer); break;
		case ReplayMessageSave_Text: reader.Read(m_szText); break;
		case ReplayMessageSave_Color: reader.Read(m_eColor); break;
		

		}
		
	}
	
}

void CvReplayMessage::write(CvSavegameWriter writer) const
{
	LogIntentHelper helper(writer, "CvReplayMessage");

	writer.AssignClassType(SAVEGAME_CLASS_REPLAYMESSAGE);

	// Write the data.
	// Use WriteSwitch since it will automatically include WriteSwitch in the savegame.
	// Also it will not save anything if the variable and the default values are identical.
	// If nothing is saved, the loading code will use the default values.
	// Less data saved/loaded means smaller savegames.
	writer.Write(ReplayMessageSave_Turn, m_iTurn, defaultTurn);
	writer.Write(ReplayMessageSave_Type, m_eType, defaultType);
	writer.Write(ReplayMessageSave_PlotX, m_iPlotX, defaultPlotX);
	writer.Write(ReplayMessageSave_PlotY, m_iPlotY, defaultPlotY);
	writer.Write(ReplayMessageSave_Player, m_ePlayer, defaultPlayer);
	writer.Write(ReplayMessageSave_Text, m_szText);
	writer.Write(ReplayMessageSave_Color, m_eColor, defaultColor);

	writer.Write(ReplayMessageSave_END);
}
