#include "../CvGameCoreDLL.h"
#include "GUI_input.h"

GUI_input::GUI_input(GUI_NotificationTypes eNotification, int iData, unsigned int iFlags, int iID, bool bShift, bool bControl, bool bAlt, int iMouseX, int iMouseY, WidgetTypes eWidget, int iData1, int iData2, bool bOption)
	: m_eNotification(eNotification)
	, m_iData(iData)
	, m_iFlags(iFlags)
	, m_iID(iID)
	, m_bShift(bShift)
	, m_bControl(bControl)
	, m_bAlt(bAlt)
	, m_iMouseX(iMouseX)
	, m_iMouseY(iMouseY)
	, m_eWidget(eWidget)
	, m_iData1(iData1)
	, m_iData2(iData2)
	, m_bOption(bOption)
{
}

GUI_input::GUI_NotificationTypes GUI_input::getNotification() const
{
	return m_eNotification;
}

int GUI_input::getData() const
{
	return m_iData;
}

unsigned int GUI_input::getFlags() const
{
	return m_iFlags;
}

int GUI_input::getID() const
{
	return m_iID;
}

bool GUI_input::isShiftKeyDown() const
{
	return m_bShift;
}

bool GUI_input::isCtrlKeyDown() const
{
	return m_bControl;
}

bool GUI_input::isAltKeyDown() const
{
	return m_bAlt;
}

int GUI_input::getMouseX() const
{
	return m_iMouseX;
}

int GUI_input::getMouseY() const
{
	return m_iMouseY;
}

WidgetTypes GUI_input::getWidget() const
{
	return m_eWidget;
}

int GUI_input::getData1() const
{
	return m_iData1;
}

int GUI_input::getData2() const
{
	return m_iData2;
}

bool GUI_input::getOption() const
{
	return m_bOption;
}
