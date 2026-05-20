#pragma once

#ifndef CVPOPUPRETURN_h
#define CVPOPUPRETURN_h

//	Class:		PopupReturn
//	Purpose:	Return structure for popups
//	Author:		Patrick Dawson
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2002 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------

class FDataStream;

class PopupReturn
{

public:
	DllExport PopupReturn();

	DllExport void setSelectedRadioButton(int iValue, int iGroup = 0);
	DllExport int getSelectedRadioButton(int iGroup = 0) const;
	DllExport void setCheckboxBitfield(int iValue, int iGroup = 0);
	DllExport int getCheckboxBitfield(int iGroup = 0) const;
	DllExport void setEditBoxString(const wchar* szValue, int iGroup = 0);
	DllExport const wchar* getEditBoxString(int iGroup = 0) const;
	DllExport void setSpinnerWidgetValue(int iValue, int iGroup = 0);
	DllExport int getSpinnerWidgetValue(int iGroup = 0) const;
	DllExport void setSelectedPulldownValue(int iValue, int iGroup = 0);
	DllExport int getSelectedPullDownValue(int iGroup = 0) const;
	DllExport void setSelectedListBoxValue(int iValue, int iGroup = 0);
	DllExport int getSelectedListBoxValue(int iGroup = 0) const;
	DllExport void setCurrentSpinBoxValue(int iValue, int iIndex = 0);
	DllExport int getCurrentSpinBoxValue( int iIndex = 0 ) const;
	DllExport void setButtonClicked(int iValue, int iGroup = 0);
	DllExport int getButtonClicked( int iGroup = 0 ) const;

	// read and write object to a stream
	DllExport void read(FDataStreamBase* pStream);
	DllExport void write(FDataStreamBase* pStream) const;

private:
	std::map<int, int> m_aiSelectedRadioButton;		//	Selected Radio Button
	std::map<int, int> m_aiBitField;					//	BitField
	std::map<int, CvWString> m_aszEditBoxString;			//	EditBox String
	std::map<int, int> m_aiSpinnerWidgetValues;		//	Spinner Widget Values
	std::map<int, int> m_aiPulldownID;					//	Pulldown ID
	std::map<int, int> m_aiListBoxID;					//	ListBox ID
	std::map<int, int> m_aiButtonID;					//	The button clicked on
	std::map<int, int> m_aiSpinBoxID;					//	SpinBox ID
};

#endif	// CVPOPUPRETURN_h
