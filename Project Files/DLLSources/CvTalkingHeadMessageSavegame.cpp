#include "CvGameCoreDLL.h"
#include "CvInitCore.h"

#include "CvSavegame.h"

// set the default values

const int defaultLength = 0;
const ColorTypes defaultFlashColor = NO_COLOR;
const int defaultFlashX = -1;
const int defaultFlashY = -1;
const bool defaultOffScreenArrows = false;
const bool defaultOnScreenArrows = false;
const int defaultTurn = 0;
const InterfaceMessageTypes defaultMessageType = MESSAGE_TYPE_INFO;
const PlayerTypes defaultFromPlayer = NO_PLAYER;
const ChatTargetTypes defaultTarget = NO_CHATTARGET;
const bool defaultShown = false;

// 
enum SavegameVariableTypes
{
	TalkingHeadMessageSave_END,
	TalkingHeadMessageSave_Description,
	TalkingHeadMessageSave_Sound,
	TalkingHeadMessageSave_Icon,
	TalkingHeadMessageSave_Length,
	TalkingHeadMessageSave_FlashColor,
	TalkingHeadMessageSave_FlashX,
	TalkingHeadMessageSave_FlashY,
	TalkingHeadMessageSave_OffScreenArrows,
	TalkingHeadMessageSave_OnScreenArrows,
	TalkingHeadMessageSave_Turn,
	TalkingHeadMessageSave_MessageType,
	TalkingHeadMessageSave_FromPlayer,
	TalkingHeadMessageSave_Target,
	TalkingHeadMessageSave_Shown,
	NUM_SAVE_ENUM_VALUES,
};

const char* getSavedEnumNameTalkingHeadMessage(SavegameVariableTypes eType)
{
	switch (eType)
	{
	case TalkingHeadMessageSave_END: return "TalkingHeadMessageSave_END";
	case TalkingHeadMessageSave_Description: return "TalkingHeadMessageSave_Description";
	case TalkingHeadMessageSave_Sound: return "TalkingHeadMessageSave_Sound";
	case TalkingHeadMessageSave_Icon: return "TalkingHeadMessageSave_Icon";
	case TalkingHeadMessageSave_Length: return "TalkingHeadMessageSave_Length";
	case TalkingHeadMessageSave_FlashColor: return "TalkingHeadMessageSave_FlashColor";
	case TalkingHeadMessageSave_FlashX: return "TalkingHeadMessageSave_FlashX";
	case TalkingHeadMessageSave_FlashY: return "TalkingHeadMessageSave_FlashY";
	case TalkingHeadMessageSave_OffScreenArrows: return "TalkingHeadMessageSave_OffScreenArrows";
	case TalkingHeadMessageSave_OnScreenArrows: return "TalkingHeadMessageSave_OnScreenArrows";
	case TalkingHeadMessageSave_Turn: return "TalkingHeadMessageSave_Turn";
	case TalkingHeadMessageSave_MessageType: return "TalkingHeadMessageSave_MessageType";
	case TalkingHeadMessageSave_FromPlayer: return "TalkingHeadMessageSave_FromPlayer";
	case TalkingHeadMessageSave_Target: return "TalkingHeadMessageSave_Target";
	case TalkingHeadMessageSave_Shown: return "TalkingHeadMessageSave_Shown";
	}
	return "";
}

int getNumSavedEnumValuesTalkingHeadMessage()
{
	return NUM_SAVE_ENUM_VALUES;
}

// assign everything to default values
void CvTalkingHeadMessage::resetSavedData()
{
	m_szDescription.clear();
	m_szSound.clear();
	m_szIcon.clear();
	m_iLength = defaultLength;
	m_eFlashColor = defaultFlashColor;
	m_iFlashX = defaultFlashX;
	m_iFlashY = defaultFlashY;
	m_bOffScreenArrows = defaultOffScreenArrows;
	m_bOnScreenArrows = defaultOnScreenArrows;
	m_iTurn = defaultTurn;
	m_eMessageType = defaultMessageType;
	m_eFromPlayer = defaultFromPlayer;
	m_eTarget = defaultTarget;
	m_bShown = defaultShown;
}

void CvTalkingHeadMessage::read(CvSavegameReader reader)
{
	reader.AssignClassType(SAVEGAME_CLASS_TALKINGHEADMESSAGE);

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
		case TalkingHeadMessageSave_END: bContinue = false; break;
		case TalkingHeadMessageSave_Description: reader.Read(m_szDescription); break;
		case TalkingHeadMessageSave_Sound: reader.Read(m_szSound); break;
		case TalkingHeadMessageSave_Icon: reader.Read(m_szIcon); break;
		case TalkingHeadMessageSave_Length: reader.Read(m_iLength); break;
		case TalkingHeadMessageSave_FlashColor: reader.Read(m_eFlashColor); break;
		case TalkingHeadMessageSave_FlashX: reader.Read(m_iFlashX); break;
		case TalkingHeadMessageSave_FlashY: reader.Read(m_iFlashY); break;
		case TalkingHeadMessageSave_OffScreenArrows: reader.Read(m_bOffScreenArrows); break;
		case TalkingHeadMessageSave_OnScreenArrows: reader.Read(m_bOnScreenArrows); break;
		case TalkingHeadMessageSave_Turn: reader.Read(m_iTurn); break;
		case TalkingHeadMessageSave_MessageType: reader.Read(m_eMessageType); break;
		case TalkingHeadMessageSave_FromPlayer: reader.Read(m_eFromPlayer); break;
		case TalkingHeadMessageSave_Target: reader.Read(m_eTarget); break;
		case TalkingHeadMessageSave_Shown: reader.Read(m_bShown); break;

		}
	}
	
}

void CvTalkingHeadMessage::write(CvSavegameWriter writer)
{
	writer.AssignClassType(SAVEGAME_CLASS_TALKINGHEADMESSAGE);

	// Write the data.
	// Use WriteSwitch since it will automatically include WriteSwitch in the savegame.
	// Also it will not save anything if the variable and the default values are identical.
	// If nothing is saved, the loading code will use the default values.
	// Less data saved/loaded means smaller savegames.
	writer.Write(TalkingHeadMessageSave_Description, m_szDescription);
	writer.Write(TalkingHeadMessageSave_Sound, m_szSound);
	writer.Write(TalkingHeadMessageSave_Icon, m_szIcon);
	writer.Write(TalkingHeadMessageSave_Length, m_iLength, defaultLength);
	writer.Write(TalkingHeadMessageSave_FlashColor, m_eFlashColor, defaultFlashColor);
	writer.Write(TalkingHeadMessageSave_FlashX, m_iFlashX, defaultFlashX);
	writer.Write(TalkingHeadMessageSave_FlashY, m_iFlashY, defaultFlashY);
	writer.Write(TalkingHeadMessageSave_OffScreenArrows, m_bOffScreenArrows, defaultOffScreenArrows);
	writer.Write(TalkingHeadMessageSave_OnScreenArrows, m_bOnScreenArrows, defaultOnScreenArrows);
	writer.Write(TalkingHeadMessageSave_Turn, m_iTurn, defaultTurn);
	writer.Write(TalkingHeadMessageSave_MessageType, m_eMessageType, defaultMessageType);
	writer.Write(TalkingHeadMessageSave_FromPlayer, m_eFromPlayer, defaultFromPlayer);
	writer.Write(TalkingHeadMessageSave_Target, m_eTarget, defaultTarget);
	writer.Write(TalkingHeadMessageSave_Shown, m_bShown, defaultShown);
	
	writer.Write(TalkingHeadMessageSave_END);
}
