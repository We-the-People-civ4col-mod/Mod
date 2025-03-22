

class GUI_input
{
public:
	enum GUI_NotificationTypes
	{
		// the list has to match what the exe provides even if it looks weird and uses 12 twice
		NOTIFY_CLICKED,
		NOTIFY_DBL_CLICKED,
		NOTIFY_FOCUS,
		NOTIFY_UNFOCUS,
		NOTIFY_CURSOR_MOVE_ON,
		NOTIFY_CURSOR_MOVE_OFF,
		NOTIFY_CHARACTER,
		NOTIFY_SCROLL_UP,
		NOTIFY_SCROLL_DOWN,
		NOTIFY_NEW_HORIZONTAL_STOP,
		NOTIFY_NEW_VERTICAL_STOP,
		NOTIFY_LISTBOX_ITEM_SELECTED,
		NOTIFY_MOUSEMOVE = 12,
		NOTIFY_FLYOUT_ITEM_SELECTED = 12,
		// 13
		NOTIFY_MOUSEWHEELUP = 14,
		NOTIFY_MOUSEWHEELDOWN,
		// 16
		// 17
		NOTIFY_LINKEXECUTE = 18,
		NOTIFY_MOVIE_DONE,
		NOTIFY_SLIDER_NEWSTOP,
		NOTIFY_TABLE_HEADER_SELECTED
	};
	
	GUI_input(GUI_NotificationTypes eNotification, int iData, unsigned int iFlags, int iID, bool bShift, bool bControl, bool bAlt, int iMouseX, int iMouseY, enum WidgetTypes eWidget, int iData1, int iData2, bool bOption);

	GUI_NotificationTypes getNotification() const;
	int getData() const;
	unsigned int getFlags() const;
	int getID() const;
	//int getFunctionName() const; // needed?
	bool isShiftKeyDown() const;
	bool isCtrlKeyDown() const;
	bool isAltKeyDown() const;
	int getMouseX() const;
	int getMouseY() const;
	enum WidgetTypes getWidget() const;
	int getData1() const;
	int getData2() const;
	bool getOption() const;

private:
	GUI_NotificationTypes m_eNotification;
	int m_iData;
	unsigned int m_iFlags;
	int m_iID;
	bool m_bShift;
	bool m_bControl;
	bool m_bAlt;
	int m_iMouseX;
	int m_iMouseY;
	enum WidgetTypes m_eWidget;
	int m_iData1;
	int m_iData2;
	bool m_bOption;
};
