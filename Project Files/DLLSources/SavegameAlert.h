#pragma once

namespace SavegameAlert
{
	AlertWindow::returnTypes SavegameTooOld();
	AlertWindow::returnTypes SavegameTooNew();

	void HandleSavegameTooOld(bool nIsOld);
	void HandleSavegameTooNew(bool nIsNew);
}
