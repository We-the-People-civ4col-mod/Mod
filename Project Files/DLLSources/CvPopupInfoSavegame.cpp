#include "CvGameCoreDLL.h"
#include "CvInitCore.h"
#include "CvPopupInfo.h"

#include "CvSavegame.h"

// set the default values
const int defaultData1 = -1;
const int defaultData2 = -1;
const int defaultData3 = -1;
const int defaultFlags = 0;
const bool defaultOption1 = false;
const bool defaultOption2 = false;

const ButtonPopupTypes defaultButtonPopupType = BUTTONPOPUP_TEXT;

// 
enum SavegameVariableTypes
{
	PopupInfoSave_END,
	PopupInfoSave_Data1,
	PopupInfoSave_Data2,
	PopupInfoSave_Data3,
	PopupInfoSave_Flags,
	PopupInfoSave_Option1,
	PopupInfoSave_Option2,

	PopupInfoSave_ButtonPopupType,
	PopupInfoSave_Text,

	PopupInfoSave_OnFocusPythonCallback,
	PopupInfoSave_OnClickedPythonCallback,
	PopupInfoSave_PythonModule,
	PopupInfoSave_PythonButtons,
	NUM_SAVE_ENUM_VALUES,
};

const char* getSavedEnumNamePopupInfo(SavegameVariableTypes eType)
{
	switch (eType)
	{
	case PopupInfoSave_END: return "PopupInfoSave_END";
	case PopupInfoSave_Data1: return "PopupInfoSave_Data1";
	case PopupInfoSave_Data2: return "PopupInfoSave_Data2";
	case PopupInfoSave_Data3: return "PopupInfoSave_Data3";
	case PopupInfoSave_Flags: return "PopupInfoSave_Flags";
	case PopupInfoSave_Option1: return "PopupInfoSave_Option1";
	case PopupInfoSave_Option2: return "PopupInfoSave_Option2";

	case PopupInfoSave_ButtonPopupType: return "PopupInfoSave_ButtonPopupType";
	case PopupInfoSave_Text: return "PopupInfoSave_Text";

	case PopupInfoSave_OnFocusPythonCallback: return "PopupInfoSave_OnFocusPythonCallback";
	case PopupInfoSave_OnClickedPythonCallback: return "PopupInfoSave_OnClickedPythonCallback";
	case PopupInfoSave_PythonModule: return "PopupInfoSave_PythonModule";
	case PopupInfoSave_PythonButtons: return "PopupInfoSave_PythonButtons";
	}
	return "";
}

int getNumSavedEnumValuesPopupInfo()
{
	return NUM_SAVE_ENUM_VALUES;
}

// assign everything to default values
void CvPopupInfo::resetSavedData()
{
	m_iData1 = defaultData1;
	m_iData2 = defaultData2;
	m_iData3 = defaultData3;
	m_iFlags = defaultFlags;
	m_bOption1 = defaultOption1;
	m_bOption2 = defaultOption2;

	m_eButtonPopupType = defaultButtonPopupType;
	m_szText.clear();

	m_szOnFocusPythonCallback.clear();
	m_szOnClickedPythonCallback.clear();
	m_szPythonModule.clear();
	m_aPythonButtons.clear();
}

void CvPopupInfo::read(CvSavegameReader reader)
{
	reader.AssignClassType(SAVEGAME_CLASS_POPUPINFO);

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
		case PopupInfoSave_END: bContinue = false; break;
		case PopupInfoSave_Data1: reader.Read(m_iData1); break;
		case PopupInfoSave_Data2: reader.Read(m_iData2); break;
		case PopupInfoSave_Data3: reader.Read(m_iData3); break;
		case PopupInfoSave_Flags: reader.Read(m_iFlags); break;
		case PopupInfoSave_Option1: reader.Read(m_bOption1); break;
		case PopupInfoSave_Option2: reader.Read(m_bOption2); break;

		case PopupInfoSave_ButtonPopupType: reader.Read(m_eButtonPopupType); break;
		case PopupInfoSave_Text: reader.Read(m_szText); break;

		case PopupInfoSave_OnFocusPythonCallback: reader.Read(m_szOnFocusPythonCallback); break;
		case PopupInfoSave_OnClickedPythonCallback: reader.Read(m_szOnClickedPythonCallback); break;
		case PopupInfoSave_PythonModule: reader.Read(m_szPythonModule); break;
		case PopupInfoSave_PythonButtons: reader.Read(m_aPythonButtons); break;
		
		}
	}
	
}

void CvPopupInfo::write(CvSavegameWriter writer) const
{
	writer.AssignClassType(SAVEGAME_CLASS_POPUPINFO);

	// Write the data.
	// Use WriteSwitch since it will automatically include WriteSwitch in the savegame.
	// Also it will not save anything if the variable and the default values are identical.
	// If nothing is saved, the loading code will use the default values.
	// Less data saved/loaded means smaller savegames.
	writer.Write(PopupInfoSave_Data1, m_iData1, defaultData1);
	writer.Write(PopupInfoSave_Data2, m_iData2, defaultData2);
	writer.Write(PopupInfoSave_Data3, m_iData3, defaultData3);
	writer.Write(PopupInfoSave_Flags, m_iFlags, defaultFlags);
	writer.Write(PopupInfoSave_Option1, m_bOption1, defaultOption1);
	writer.Write(PopupInfoSave_Option2, m_bOption2, defaultOption2);

	writer.Write(PopupInfoSave_ButtonPopupType, m_eButtonPopupType, defaultButtonPopupType);
	writer.Write(PopupInfoSave_Text, m_szText);

	writer.Write(PopupInfoSave_OnFocusPythonCallback, m_szOnFocusPythonCallback);
	writer.Write(PopupInfoSave_OnClickedPythonCallback, m_szOnClickedPythonCallback);

	writer.Write(PopupInfoSave_PythonModule, m_szPythonModule);
	writer.Write(PopupInfoSave_PythonButtons, m_aPythonButtons);
	
	writer.Write(PopupInfoSave_END);
}

	void CvPopupButtonPython::read(CvSavegameReader reader)
	{
		reader.Read(szText);
		reader.Read(szArt);
	}
	void CvPopupButtonPython::write(CvSavegameWriter writer)
	{
		writer.Write(szText);
		writer.Write(szArt);
	}
