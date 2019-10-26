#include "CvGameCoreDLL.h"

#include "CvSavegame.h"

// set the default values

const int defaultID = 0;
const int defaultInitialGameTurn = 0;
const PlayerTypes defaultFirstPlayer = NO_PLAYER;
const PlayerTypes defaultSecondPlayer = NO_PLAYER;

// 
enum SavegameVariableTypes
{
	DealSave_END,
	DealSave_ID,
	DealSave_InitialGameTurn,
	DealSave_FirstPlayer,
	DealSave_SecondPlayer,
	DealSave_firstTrades,
	DealSave_secondTrades,

	NUM_DealSAVE_ENUM_VALUES,
};

const char* getSavedEnumNameDeal(SavegameVariableTypes eType)
{
	switch (eType)
	{
		case DealSave_END: return "DealSave_END";
		case DealSave_ID: return "DealSave_ID";
		case DealSave_InitialGameTurn: return "DealSave_InitialGameTurn";
		case DealSave_FirstPlayer: return "DealSave_FirstPlayer";
		case DealSave_SecondPlayer: return "DealSave_SecondPlayer";
		case DealSave_firstTrades: return "DealSave_firstTrades";
		case DealSave_secondTrades: return "DealSave_secondTrades";
	}
	return "";
}

int getNumSavedEnumValuesDeal()
{
	return NUM_DealSAVE_ENUM_VALUES;
}

// assign everything to default values
void CvDeal::resetSavedData(int iID, PlayerTypes eFirstPlayer, PlayerTypes eSecondPlayer)
{
	m_iID = iID;
	m_iInitialGameTurn = defaultInitialGameTurn;
	m_eFirstPlayer = eFirstPlayer;
	m_eSecondPlayer = eSecondPlayer;
}

void CvDeal::read(CvSavegameReader reader)
{
	reader.AssignClassType(SAVEGAME_CLASS_DEAL);

	// Init data before load
	// This will ensure that all variables not included in the savegame will have default values
	reset();
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
		case DealSave_END:
			bContinue = false;
			break;
		case DealSave_ID: reader.Read(m_iID); break;
		case DealSave_InitialGameTurn: reader.Read(m_iInitialGameTurn); break;
		case DealSave_FirstPlayer: reader.Read(m_eFirstPlayer); break;
		case DealSave_SecondPlayer: reader.Read(m_eSecondPlayer); break;
		case DealSave_firstTrades: reader.Read(m_firstTrades); break;
		case DealSave_secondTrades: reader.Read(m_secondTrades); break;
		

		}
		
	}
	
}

void CvDeal::write(CvSavegameWriter writer)
{
	writer.AssignClassType(SAVEGAME_CLASS_DEAL);

	// Write the data.
	// Use WriteSwitch since it will automatically include WriteSwitch in the savegame.
	// Also it will not save anything if the variable and the default values are identical.
	// If nothing is saved, the loading code will use the default values.
	// Less data saved/loaded means smaller savegames.
	writer.Write(DealSave_ID, m_iID, defaultID);
	writer.Write(DealSave_InitialGameTurn, m_iInitialGameTurn, defaultInitialGameTurn);
	writer.Write(DealSave_FirstPlayer, m_eFirstPlayer, defaultFirstPlayer);
	writer.Write(DealSave_SecondPlayer, m_eSecondPlayer, defaultSecondPlayer);
	writer.Write(DealSave_firstTrades, m_firstTrades);
	writer.Write(DealSave_secondTrades, m_secondTrades);

	writer.Write(DealSave_END);
}
