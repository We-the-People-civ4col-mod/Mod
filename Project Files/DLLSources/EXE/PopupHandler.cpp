#include "../CvGameCoreDLL.h"
#include "Vanilla_CvDLLInterfaceIFaceBase.h"
#include "PopupHandler.h"

PopupHandler::GenericButton::GenericButton(CvPopup* popup)
	: m_popup(popup)
	, m_szIcon(NULL)
	, m_iButtonId(0)
	, m_eWidgetType(WIDGET_GENERAL)
	, m_iData1(MAX_INT)
	, m_iData2(MAX_INT)
	, m_bOption(true)
	, m_ctrlLayout(POPUP_LAYOUT_CENTER)
	, m_textJustifcation(DLL_FONT_LEFT_JUSTIFY)
{
}

void PopupHandler::GenericButton::create()
{
	gDLL->getInterfaceIFace()->popupAddGenericButton(m_popup, m_szText, m_szIcon, m_iButtonId, m_eWidgetType, m_iData1, m_iData2, m_bOption, m_ctrlLayout, m_textJustifcation);
}

PopupHandler::GenericButton& PopupHandler::GenericButton::text(CvWString text)
{
	m_szText = text;
	return *this;
}

PopupHandler::GenericButton& PopupHandler::GenericButton::icon(const char* icon)
{
	m_szIcon = icon;
	return *this;
}

PopupHandler::GenericButton& PopupHandler::GenericButton::id(int ID)
{
	m_iButtonId = ID;
	return *this;
}

PopupHandler::GenericButton& PopupHandler::GenericButton::widget(WidgetTypes widget)
{
	m_eWidgetType = widget;
	return *this;
}

PopupHandler::GenericButton& PopupHandler::GenericButton::data1(int data)
{
	m_iData1 = data;
	return *this;
}

PopupHandler::GenericButton& PopupHandler::GenericButton::data2(int data)
{
	m_iData2 = data;
	return *this;
}

PopupHandler::GenericButton& PopupHandler::GenericButton::option(bool option)
{
	m_bOption = option;
	return *this;
}

PopupHandler::GenericButton& PopupHandler::GenericButton::controlLayout(PopupControlLayout layout)
{
	m_ctrlLayout = layout;
	return *this;
}

PopupHandler::GenericButton& PopupHandler::GenericButton::textJustifcation(JustificationTypes justification)
{
	m_textJustifcation = justification;
	return *this;
}

PopupHandler::EditBox::EditBox(CvPopup* popup)
	: m_popup(popup)
	, m_eWidgetType(WIDGET_GENERAL)
	, m_iGroup(0)
	, m_ctrlLayout(POPUP_LAYOUT_STRETCH)
	, m_preferredCharWidth(0)
	, m_maxCharCount(256)
	, m_bEnable(true)
{
}

void PopupHandler::EditBox::create()
{
	gDLL->getInterfaceIFace()->popupCreateEditBox(m_popup, m_szDefaultString, m_eWidgetType, m_szHelpText, m_iGroup, m_ctrlLayout, m_preferredCharWidth, m_maxCharCount);
	if (!m_bEnable)
	{
		gDLL->getInterfaceIFace()->popupEnableEditBox(m_popup, m_iGroup, false);
	}
}

PopupHandler::EditBox& PopupHandler::EditBox::defaultString(CvWString string)
{
	m_szDefaultString = string;
	return *this;
}

PopupHandler::EditBox& PopupHandler::EditBox::widget(WidgetTypes input)
{
	m_eWidgetType = input;
	return *this;
}

PopupHandler::EditBox& PopupHandler::EditBox::helpString(CvWString string)
{
	m_szHelpText = string;
	return *this;
}

PopupHandler::EditBox& PopupHandler::EditBox::group(int input)
{
	m_iGroup = input;
	return *this;
}

PopupHandler::EditBox& PopupHandler::EditBox::controlLayout(PopupControlLayout input)
{
	m_ctrlLayout = input;
	return *this;
}

PopupHandler::EditBox& PopupHandler::EditBox::charWidth(unsigned int input)
{
	m_preferredCharWidth = input;
	return *this;
}

PopupHandler::EditBox& PopupHandler::EditBox::charCount(unsigned int input)
{
	m_maxCharCount = input;
	return *this;
}

PopupHandler::EditBox& PopupHandler::EditBox::enable(bool input)
{
	m_bEnable = input;
	return *this;
}

PopupHandler::Checkbox::Checkbox(CvPopup* popup, int ID, int iGroup)
	: m_popup(popup)
	, m_iID(ID)
	, m_iGroup(iGroup)
{
}

void PopupHandler::Checkbox::setText(CvWString szText, CvWString szHelpText)
{
	gDLL->getInterfaceIFace()->popupSetCheckBoxText(m_popup, m_iID, szText, m_iGroup, szHelpText);
}

void PopupHandler::Checkbox::setState(bool bChecked)
{
	gDLL->getInterfaceIFace()->popupSetCheckBoxState(m_popup, m_iID, bChecked, m_iGroup);
}

PopupHandler::CheckBoxes::CheckBoxes(CvPopup* popup, int iNumBoxes)
	: m_popup(popup)
	, m_bCreated(false)
	, m_iNumButtons(iNumBoxes)
	, m_iGroup(0)
	, m_eWidgetType(WIDGET_GENERAL)
	, m_ctrlLayout(POPUP_LAYOUT_CENTER)
{
}

void PopupHandler::CheckBoxes::create()
{
	FAssert(!m_bCreated);
	m_bCreated = true;
	gDLL->getInterfaceIFace()->popupCreateCheckBoxes(m_popup, m_iNumButtons, m_iGroup, m_eWidgetType, m_ctrlLayout);
}

PopupHandler::CheckBoxes& PopupHandler::CheckBoxes::group(int input)
{
	FAssert(!m_bCreated);
	m_iGroup = input;
	return *this;
}

PopupHandler::CheckBoxes& PopupHandler::CheckBoxes::widget(WidgetTypes input)
{
	FAssert(!m_bCreated);
	m_eWidgetType = input;
	return *this;
}

PopupHandler::CheckBoxes& PopupHandler::CheckBoxes::controlLayout(PopupControlLayout input)
{
	FAssert(!m_bCreated);
	m_ctrlLayout = input;
	return *this;
}

PopupHandler::Checkbox PopupHandler::CheckBoxes::getButton(int ID)
{
	FAssert(m_bCreated);
	return Checkbox(m_popup, ID, m_iGroup);
}

PopupHandler::RadioButton::RadioButton(CvPopup* popup, int ID, int iGroup)
	: m_popup(popup)
	, m_iID(ID)
	, m_iGroup(iGroup)
{
}

void PopupHandler::RadioButton::setText(CvWString szText, CvWString szHelpText)
{
	gDLL->getInterfaceIFace()->popupSetRadioButtonText(m_popup, m_iID, szText, m_iGroup, szHelpText);
}

PopupHandler::RadioButtons::RadioButtons(CvPopup* popup, int iNumButtons)
	: m_popup(popup)
	, m_bCreated(false)
	, m_iNumButtons(iNumButtons)
	, m_iGroup(0)
	, m_eWidgetType(WIDGET_GENERAL)
	, m_ctrlLayout(POPUP_LAYOUT_CENTER)
{
}

void PopupHandler::RadioButtons::create()
{
	FAssert(!m_bCreated);
	m_bCreated = true;
	gDLL->getInterfaceIFace()->popupCreateRadioButtons(m_popup, m_iNumButtons, m_iGroup, m_eWidgetType, m_ctrlLayout);
}

PopupHandler::RadioButtons& PopupHandler::RadioButtons::group(int input)
{
	FAssert(!m_bCreated);
	m_iGroup = input;
	return *this;
}

PopupHandler::RadioButtons& PopupHandler::RadioButtons::widget(WidgetTypes input)
{
	FAssert(!m_bCreated);
	m_eWidgetType = input;
	return *this;
}

PopupHandler::RadioButtons& PopupHandler::RadioButtons::controlLayout(PopupControlLayout input)
{
	FAssert(!m_bCreated);
	m_ctrlLayout = input;
	return *this;
}

PopupHandler::RadioButton PopupHandler::RadioButtons::getButton(int ID)
{
	FAssert(m_bCreated);
	return RadioButton(m_popup, ID, m_iGroup);
}

PopupHandler::SpinBox::SpinBox(CvPopup* popup)
	: m_popup(popup)
	, m_iIndex(MIN_INT)
	, m_iDefault(MIN_INT)
	, m_iIncrement(MIN_INT)
	, m_iMax(MIN_INT)
	, m_iMin(MIN_INT)
{
}

void PopupHandler::SpinBox::create()
{
	FAssert(m_iIndex != MIN_INT);
	FAssert(m_iDefault != MIN_INT);
	FAssert(m_iIncrement != MIN_INT);
	FAssert(m_iMax != MIN_INT);
	FAssert(m_iMin != MIN_INT);
	gDLL->getInterfaceIFace()->popupCreateSpinBox(m_popup, m_iIndex, m_szHelpText, m_iDefault, m_iIncrement, m_iMax, m_iMin);
}

PopupHandler::SpinBox& PopupHandler::SpinBox::index(int input)
{
	m_iIndex = input;
	return *this;
}

PopupHandler::SpinBox& PopupHandler::SpinBox::help(CvWString input)
{
	m_szHelpText = input;
	return *this;
}

PopupHandler::SpinBox& PopupHandler::SpinBox::setDefault(int input)
{
	m_iDefault = input;
	return *this;
}

PopupHandler::SpinBox& PopupHandler::SpinBox::increment(int input)
{
	m_iIncrement = input;
	return *this;
}

PopupHandler::SpinBox& PopupHandler::SpinBox::max(int input)
{
	m_iMax = input;
	return *this;
}

PopupHandler::SpinBox& PopupHandler::SpinBox::min(int input)
{
	m_iMin = input;
	return *this;
}

PopupHandler::PopupHandler(CvPopup* popup)
	: m_popup(popup)
	, m_bOkButton(true)
	, m_eState(POPUPSTATE_QUEUED)
	, m_iNumPixelScroll(0)
{
}

void PopupHandler::startVLayout(uint iFlags)
{
	gDLL->getInterfaceIFace()->popupStartVLayout(m_popup, iFlags);
}

void PopupHandler::startHLayout(uint iFlags)
{
	gDLL->getInterfaceIFace()->popupStartHLayout(m_popup, iFlags);
}

void PopupHandler::endLayout()
{
	gDLL->getInterfaceIFace()->popupEndLayout(m_popup);
}

void PopupHandler::setHeaderString(CvWString szText, JustificationTypes uiFlags)
{
	gDLL->getInterfaceIFace()->popupSetHeaderString(m_popup, szText, uiFlags);
}

void PopupHandler::setBodyString(CvWString szText, JustificationTypes uiFlags, char* szName, CvWString szHelpText)
{
	gDLL->getInterfaceIFace()->popupSetBodyString(m_popup, szText, uiFlags, szName, szHelpText);
}

void PopupHandler::addSeparator(int iSpace)
{
	gDLL->getInterfaceIFace()->popupAddSeparator(m_popup, iSpace);
}

void PopupHandler::setPopupType(PopupEventTypes ePopupType, LPCTSTR szArtFileName)
{
	gDLL->getInterfaceIFace()->popupSetPopupType(m_popup, ePopupType, szArtFileName);
}

void PopupHandler::addDDS(const char* szIconFilename, int iWidth, int iHeight, CvWString szHelpText)
{
	gDLL->getInterfaceIFace()->popupAddDDS(m_popup, szIconFilename, iWidth, iHeight, szHelpText);
}

PopupHandler::GenericButton PopupHandler::createGenericButton()
{
	return GenericButton(m_popup);
}

PopupHandler::EditBox PopupHandler::createEditBox()
{
	return EditBox(m_popup);
}

PopupHandler::CheckBoxes PopupHandler::createCheckBoxes(int iNumBoxes)
{
	return CheckBoxes(m_popup, iNumBoxes);
}

PopupHandler::RadioButtons PopupHandler::createRadioButtons(int iNumButtons)
{
	return RadioButtons(m_popup, iNumButtons);
}

PopupHandler::SpinBox PopupHandler::createSpinBox()
{
	return SpinBox(m_popup);
}

void PopupHandler::setAsCancelled()
{
	gDLL->getInterfaceIFace()->popupSetAsCancelled(m_popup);
}

bool PopupHandler::isDying()
{
	return gDLL->getInterfaceIFace()->popupIsDying(m_popup);
}

PopupHandler& PopupHandler::enableOKButton(bool bEnable)
{
	m_bOkButton = bEnable;
	return *this;
}

PopupHandler& PopupHandler::setState(PopupStates eState)
{
	m_eState = eState;
	return *this;
}

PopupHandler& PopupHandler::setNumPixelScroll(int iCount)
{
	m_iNumPixelScroll = iCount;
	return *this;
}

void PopupHandler::launch()
{
	gDLL->getInterfaceIFace()->popupLaunch(m_popup, m_bOkButton, m_eState, m_iNumPixelScroll);
}

void PopupHandler::setStyle(WindowStyle style)
{
	const char* szStyle = getStyle(style);
	if (szStyle != NULL)
	{
		gDLL->getInterfaceIFace()->popupSetStyle(m_popup, szStyle);
	}
}

const char* PopupHandler::getStyle(WindowStyle style) const
{
	switch (style)
	{
	case Window_Standard_Style: return "Window_Standard_Style";
	case Window_StandardResize_Style: return "Window_StandardResize_Style";
	case Window_StandardNoResize_Style: return "Window_StandardNoResize_Style";
	case Window_Standard_TitleBar_Normal_Style: return "Window_Standard_TitleBar_Normal_Style";
	case Window_Standard_TitleBar_Maximized_Style: return "Window_Standard_TitleBar_Maximized_Style";
	case Window_Standard_TitleBar_Minimized_Style: return "Window_Standard_TitleBar_Minimized_Style";
	case Window_Standard_TitleBar_Collapsed_Style: return "Window_Standard_TitleBar_Collapsed_Style";
	case Window_Standard_TitleBar_SysBorderCollapsed_Style: return "Window_Standard_TitleBar_SysBorderCollapsed_Style";
	case Window_StandardNoTitleBar_Style: return "Window_StandardNoTitleBar_Style";
	case Window_StandardNoTitleBarNoResize_Style: return "Window_StandardNoTitleBarNoResize_Style";
	case Window_NoTitleBar_Style: return "Window_NoTitleBar_Style";
	case Window_NoTitleBarResize_Style: return "Window_NoTitleBarResize_Style";
	case Window_Dialog_TitleBar_Maximized_Style: return "Window_Dialog_TitleBar_Maximized_Style";
	case Window_Dialog_TitleBar_Collapsed_Style: return "Window_Dialog_TitleBar_Collapsed_Style";
	case Window_Tool_TitleBar_Normal_Style: return "Window_Tool_TitleBar_Normal_Style";
	case Window_Tool_TitleBar_Maximized_Style: return "Window_Tool_TitleBar_Maximized_Style";
	case Window_Tool_TitleBar_Minimized_Style: return "Window_Tool_TitleBar_Minimized_Style";
	case Window_Tool_TitleBar_Collapsed_Style: return "Window_Tool_TitleBar_Collapsed_Style";
	case Window_ToolResize_Style: return "Window_ToolResize_Style";
	case Window_Alt_Style: return "Window_Alt_Style";
	case Window_AltNoResize_Style: return "Window_AltNoResize_Style";
	case Window_Standard_OuterBorder_Normal_Style: return "Window_Standard_OuterBorder_Normal_Style";
	case Window_NoTitleBar_OuterBorder_Normal_Style: return "Window_NoTitleBar_OuterBorder_Normal_Style";
	case Window_Standard_OuterBorder_Min_Style: return "Window_Standard_OuterBorder_Min_Style";
	case Window_Standard_OuterBorder_Max_Style: return "Window_Standard_OuterBorder_Max_Style";
	case Window_Standard_OuterBorder_Collapse_Style: return "Window_Standard_OuterBorder_Collapse_Style";
	case Window_Standard_InFrameBorder_Normal_Style: return "Window_Standard_InFrameBorder_Normal_Style";
	case Window_StandardResize_InFrameBorder_Normal_Style: return "Window_StandardResize_InFrameBorder_Normal_Style";
	case Window_NoTitleBar_InFrameBorder_Normal_Style: return "Window_NoTitleBar_InFrameBorder_Normal_Style";
	case Window_NoTitleBarResize_InFrameBorder_Normal_Style: return "Window_NoTitleBarResize_InFrameBorder_Normal_Style";
	case Window_Tool_OuterBorder_Normal_Style: return "Window_Tool_OuterBorder_Normal_Style";
	case Window_Tool_OuterBorder_Min_Style: return "Window_Tool_OuterBorder_Min_Style";
	case Window_Tool_OuterBorder_Max_Style: return "Window_Tool_OuterBorder_Max_Style";
	case Window_Tool_OuterBorder_Collapse_Style: return "Window_Tool_OuterBorder_Collapse_Style";
	case Window_Tool_InFrameBorder_Normal_Style: return "Window_Tool_InFrameBorder_Normal_Style";
	case Window_ToolResize_InFrameBorder_Normal_Style: return "Window_ToolResize_InFrameBorder_Normal_Style";
	default:
		FAssertMsg(false, "Input not supported by the EXE");
		return NULL;
	}
}
