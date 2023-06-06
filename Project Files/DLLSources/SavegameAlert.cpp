#include "CvGameCoreDll.h"
#include "SavegameAlert.h"
#include "AlertWindow.h"


namespace SavegameAlert
{
	AlertWindow::returnTypes SavegameTooOld()
	{
		AlertWindow window;

		window.header = "TXT_KEY_ALERT_SAVEGAME_READ_ERROR_HEADER";
		window.message = "TXT_KEY_ALERT_SAVEGAME_TOO_OLD";
		window.setIcon(AlertWindow::iconTypes::IconError);
		return window.openWindow();
	}

	AlertWindow::returnTypes SavegameTooNew()
	{
		AlertWindow window;

		window.header = "TXT_KEY_ALERT_SAVEGAME_READ_ERROR_HEADER";
		window.message = "TXT_KEY_ALERT_SAVEGAME_TOO_NEW";
		window.setIcon(AlertWindow::iconTypes::IconError);
		return window.openWindow();
	}

	void HandleSavegameTooOld(bool nIsOld)
	{
		if (nIsOld)
		{
			SavegameTooOld();
			exit(1);
		}
	}

	void HandleSavegameTooNew(bool nIsNew)
	{
		if (nIsNew)
		{
			SavegameTooNew();
			exit(1);
		}
	}
}
