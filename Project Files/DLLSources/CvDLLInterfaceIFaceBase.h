#pragma once

#ifndef CvDLLInterfaceIFaceBase_h
#define CvDLLInterfaceIFaceBase_h

//
// abstract class containing CvInterface functions that the DLL needs
//

//#include "CvStructs.h"
#include "LinkedList.h"

class CvUnit;
class CvCity;
class CvPlot;
class CvSelectionGroup;
class CvPopupInfo;
class CvPopup;
class CvTalkingHeadMessage;
typedef std::list<CvPopupInfo*> CvPopupQueue;

class CvDLLInterfaceIFaceBase
{
public:
	virtual void lookAtSelectionPlot(bool bRelease = false) = 0;

	virtual bool canHandleAction(int iAction, CvPlot* pPlot = NULL, bool bTestVisible = false) = 0;
	virtual bool canDoInterfaceMode(InterfaceModeTypes eInterfaceMode, CvSelectionGroup* pSelectionGroup) = 0;

	virtual CvPlot* getLookAtPlot() = 0;
	virtual CvPlot* getSelectionPlot() = 0;
	virtual CvUnit* getInterfacePlotUnit(const CvPlot* pPlot, int iIndex) = 0;
	virtual CvUnit* getSelectionUnit(int iIndex) = 0;
	virtual CvUnit* getHeadSelectedUnit() = 0;
	virtual void selectUnit(CvUnit* pUnit, bool bClear, bool bToggle = false, bool bSound = false) = 0;
	virtual void selectGroup(CvUnit* pUnit, bool bShift, bool bCtrl, bool bAlt) = 0;
	virtual void selectAll(CvPlot* pPlot) = 0;

	virtual bool removeFromSelectionList(CvUnit* pUnit) = 0;
	virtual void makeSelectionListDirty() = 0;
	virtual bool mirrorsSelectionGroup() = 0;

	virtual void bringToTop(CvPopup *pPopup) = 0;
	virtual bool isPopupUp() = 0;
	virtual bool isPopupQueued() = 0;
	virtual bool isDiploOrPopupWaiting() = 0;

	virtual CvUnit* getLastSelectedUnit() = 0;
	virtual void setLastSelectedUnit(CvUnit* pUnit) = 0;
	virtual void changePlotListColumn(int iChange) = 0;
	virtual CvPlot* getGotoPlot() = 0;
	virtual CvPlot* getSingleMoveGotoPlot() = 0;
	virtual CvPlot* getOriginalPlot() = 0;

	virtual void playGeneralSound(LPCTSTR pszSound, NiPoint3 vPos = NiPoint3(-1.0f, -1.0f, -1.0f)) = 0;
	virtual void playGeneralSound(int iSoundId, int iSoundType = 1, NiPoint3 vPos = NiPoint3(-1.0f, -1.0f, -1.0f)) = 0;
	virtual void clearQueuedPopups() = 0;

	virtual CvSelectionGroup* getSelectionList() = 0;
	virtual void clearSelectionList() = 0;
	virtual void insertIntoSelectionList(CvUnit* pUnit, bool bClear, bool bToggle, bool bGroup = false, bool bSound = false, bool bMinimalChange = false) = 0;
	virtual void selectionListPostChange() = 0;
	virtual void selectionListPreChange() = 0;
	virtual int getSymbolID(int iSymbol) = 0;
	virtual CLLNode<IDInfo>* deleteSelectionListNode(CLLNode<IDInfo>* pNode) = 0;
	virtual CLLNode<IDInfo>* nextSelectionListNode(CLLNode<IDInfo>* pNode) = 0;
	virtual int getLengthSelectionList() = 0;
	virtual CLLNode<IDInfo>* headSelectionListNode() = 0;

	virtual void selectCity(CvCity* pNewValue, bool bTestProduction = false) = 0;
	virtual void selectLookAtCity(bool bAdd = false) = 0;
	virtual void addSelectedCity(CvCity* pNewValue) = 0;
	virtual void clearSelectedCities() = 0;
	virtual bool isCitySelected(const CvCity *pCity) = 0;
	virtual CvCity* getHeadSelectedCity() = 0;
	virtual bool isCitySelection() = 0;
	virtual CLLNode<IDInfo>* nextSelectedCitiesNode(CLLNode<IDInfo>* pNode) = 0;
	virtual CLLNode<IDInfo>* headSelectedCitiesNode() = 0;

	/*	K-Mod - block messages from being send to AI players
		(because the game doesn't ever clear AI messages). */
		/*	(Had been named "addHumanMessage" in K-Mod;
			Definition moved into CvDLLInterfaceIFaceBase.cpp.) */ // </advc.127>
	/* WTP - renamed to addPlayerMessage */
	// WTP, jooe: use Coordinates class, add wrapper functions for plot, unit, city
	void addPlayerMessage(PlayerTypes ePlayer, bool bForce, int iLength,
		CvWString szString, const Coordinates coord,
		LPCTSTR pszSound = NULL, InterfaceMessageTypes eType = MESSAGE_TYPE_INFO,
		LPCSTR pszIcon = NULL, ColorTypes eFlashColor = NO_COLOR,
		bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false);

	void addPlayerMessage(PlayerTypes ePlayer, bool bForce, int iLength,
		CvWString szString,
		LPCTSTR pszSound, InterfaceMessageTypes eType,
		LPCSTR pszIcon, ColorTypes eFlashColor, int iFlashX, int iFlashY,
		bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false)
		{
			addPlayerMessage(ePlayer, bForce, iLength, szString,
				Coordinates(iFlashX, iFlashY), pszSound, eType,
				pszIcon, eFlashColor,
				bShowOffScreenArrows, bShowOnScreenArrows);
		}

	void addPlayerMessage(PlayerTypes ePlayer, bool bForce, int iLength,
		CvWString szString,
		LPCTSTR pszSound, InterfaceMessageTypes eType = MESSAGE_TYPE_INFO,
		LPCSTR pszIcon = NULL, ColorTypes eFlashColor = NO_COLOR,
		bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false)
		{
			addPlayerMessage(ePlayer, bForce, iLength, szString,
				Coordinates::invalidCoord(), pszSound, eType,
				pszIcon, eFlashColor,
				bShowOffScreenArrows, bShowOnScreenArrows);
		}

	void addPlayerMessage(PlayerTypes ePlayer, bool bForce, int iLength,
		CvWString szString, CvPlot const *pPlot,
		LPCTSTR pszSound = NULL, InterfaceMessageTypes eType = MESSAGE_TYPE_INFO,
		LPCSTR pszIcon = NULL, ColorTypes eFlashColor = NO_COLOR,
		bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false)
		{
			addPlayerMessage(ePlayer, bForce, iLength, szString,
				pPlot->coord(), pszSound, eType,
				pszIcon, eFlashColor,
				bShowOffScreenArrows, bShowOnScreenArrows);
		}

	void addPlayerMessage(PlayerTypes ePlayer, bool bForce, int iLength,
		CvWString szString, CvCity const *pCity,
		LPCTSTR pszSound = NULL, InterfaceMessageTypes eType = MESSAGE_TYPE_INFO,
		LPCSTR pszIcon = NULL, ColorTypes eFlashColor = NO_COLOR,
		bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false)
		{
			addPlayerMessage(ePlayer, bForce, iLength, szString,
					pCity->coord(), pszSound, eType,
					pszIcon, eFlashColor,
					bShowOffScreenArrows, bShowOnScreenArrows);
		}

	void addPlayerMessage(PlayerTypes ePlayer, bool bForce, int iLength,
		CvWString szString, CvUnit *pUnit,
		LPCTSTR pszSound = NULL, InterfaceMessageTypes eType = MESSAGE_TYPE_INFO,
		LPCSTR pszIcon = NULL, ColorTypes eFlashColor = NO_COLOR,
		bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false)
		{
			addPlayerMessage(ePlayer, bForce, iLength, szString,
					pUnit->coord(), pszSound, eType,
					pszIcon, eFlashColor,
					bShowOffScreenArrows, bShowOnScreenArrows);
		}

	void addAllPlayersMessage(bool bForce, int iLength,
		CvWString szString, const Coordinates coord,
		LPCTSTR pszSound = NULL, InterfaceMessageTypes eType = MESSAGE_TYPE_INFO,
		LPCSTR pszIcon = NULL, ColorTypes eFlashColor = NO_COLOR,
		bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false);

	void addAllPlayersMessage(bool bForce, int iLength,
		CvWString szString,
		LPCTSTR pszSound, InterfaceMessageTypes eType,
		LPCSTR pszIcon, ColorTypes eFlashColor, int iFlashX, int iFlashY,
		bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false)
		{
			addAllPlayersMessage(bForce, iLength, szString,
				Coordinates(iFlashX, iFlashY), pszSound, eType,
				pszIcon, eFlashColor,
				bShowOffScreenArrows, bShowOnScreenArrows);
		}

	void addAllPlayersMessage(bool bForce, int iLength,
		CvWString szString,
		LPCTSTR pszSound, InterfaceMessageTypes eType = MESSAGE_TYPE_INFO,
		LPCSTR pszIcon = NULL, ColorTypes eFlashColor = NO_COLOR,
		bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false)
		{
			addAllPlayersMessage(bForce, iLength, szString,
				Coordinates::invalidCoord(), pszSound, eType,
				pszIcon, eFlashColor,
				bShowOffScreenArrows, bShowOnScreenArrows);
		}

	void addAllPlayersMessage(bool bForce, int iLength,
		CvWString szString, CvPlot const *pPlot,
		LPCTSTR pszSound = NULL, InterfaceMessageTypes eType = MESSAGE_TYPE_INFO,
		LPCSTR pszIcon = NULL, ColorTypes eFlashColor = NO_COLOR,
		bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false)
		{
			addAllPlayersMessage(bForce, iLength, szString,
				pPlot->coord(), pszSound, eType,
				pszIcon, eFlashColor,
				bShowOffScreenArrows, bShowOnScreenArrows);
		}

	void addAllPlayersMessage(bool bForce, int iLength,
		CvWString szString, CvCity const *pCity,
		LPCTSTR pszSound = NULL, InterfaceMessageTypes eType = MESSAGE_TYPE_INFO,
		LPCSTR pszIcon = NULL, ColorTypes eFlashColor = NO_COLOR,
		bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false)
		{
			addAllPlayersMessage(bForce, iLength, szString,
					pCity->coord(), pszSound, eType,
					pszIcon, eFlashColor,
					bShowOffScreenArrows, bShowOnScreenArrows);
		}

	void addAllPlayersMessage(bool bForce, int iLength,
		CvWString szString, CvUnit *pUnit,
		LPCTSTR pszSound = NULL, InterfaceMessageTypes eType = MESSAGE_TYPE_INFO,
		LPCSTR pszIcon = NULL, ColorTypes eFlashColor = NO_COLOR,
		bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false)
		{
			addAllPlayersMessage(bForce, iLength, szString,
					pUnit->coord(), pszSound, eType,
					pszIcon, eFlashColor,
					bShowOffScreenArrows, bShowOnScreenArrows);
		}

	void addOtherPlayersMessage(PlayerTypes eExcludePlayer, bool bForce, int iLength,
		CvWString szString, const Coordinates coord,
		LPCTSTR pszSound = NULL, InterfaceMessageTypes eType = MESSAGE_TYPE_INFO,
		LPCSTR pszIcon = NULL, ColorTypes eFlashColor = NO_COLOR,
		bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false);

	void addOtherPlayersMessage(PlayerTypes eExcludePlayer, bool bForce, int iLength,
		CvWString szString,
		LPCTSTR pszSound, InterfaceMessageTypes eType,
		LPCSTR pszIcon, ColorTypes eFlashColor, int iFlashX, int iFlashY,
		bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false)
		{
			addOtherPlayersMessage(eExcludePlayer, bForce, iLength, szString,
				Coordinates(iFlashX, iFlashY), pszSound, eType,
				pszIcon, eFlashColor,
				bShowOffScreenArrows, bShowOnScreenArrows);
		}

	void addOtherPlayersMessage(PlayerTypes eExcludePlayer, bool bForce, int iLength,
		CvWString szString,
		LPCTSTR pszSound, InterfaceMessageTypes eType = MESSAGE_TYPE_INFO,
		LPCSTR pszIcon = NULL, ColorTypes eFlashColor = NO_COLOR,
		bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false)
		{
			addOtherPlayersMessage(eExcludePlayer, bForce, iLength, szString,
				Coordinates::invalidCoord(), pszSound, eType,
				pszIcon, eFlashColor,
				bShowOffScreenArrows, bShowOnScreenArrows);
		}

	void addOtherPlayersMessage(PlayerTypes eExcludePlayer, bool bForce, int iLength,
		CvWString szString, CvPlot const *pPlot,
		LPCTSTR pszSound = NULL, InterfaceMessageTypes eType = MESSAGE_TYPE_INFO,
		LPCSTR pszIcon = NULL, ColorTypes eFlashColor = NO_COLOR,
		bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false)
		{
			addOtherPlayersMessage(eExcludePlayer, bForce, iLength, szString,
				pPlot->coord(), pszSound, eType,
				pszIcon, eFlashColor,
				bShowOffScreenArrows, bShowOnScreenArrows);
		}

	void addOtherPlayersMessage(PlayerTypes eExcludePlayer, bool bForce, int iLength,
		CvWString szString, CvCity const *pCity,
		LPCTSTR pszSound = NULL, InterfaceMessageTypes eType = MESSAGE_TYPE_INFO,
		LPCSTR pszIcon = NULL, ColorTypes eFlashColor = NO_COLOR,
		bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false)
		{
			addOtherPlayersMessage(eExcludePlayer, bForce, iLength, szString,
					pCity->coord(), pszSound, eType,
					pszIcon, eFlashColor,
					bShowOffScreenArrows, bShowOnScreenArrows);
		}

	void addOtherPlayersMessage(PlayerTypes eExcludePlayer, bool bForce, int iLength,
		CvWString szString, CvUnit *pUnit,
		LPCTSTR pszSound = NULL, InterfaceMessageTypes eType = MESSAGE_TYPE_INFO,
		LPCSTR pszIcon = NULL, ColorTypes eFlashColor = NO_COLOR,
		bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false)
		{
			addOtherPlayersMessage(eExcludePlayer, bForce, iLength, szString,
					pUnit->coord(), pszSound, eType,
					pszIcon, eFlashColor,
					bShowOffScreenArrows, bShowOnScreenArrows);
		}


protected:
	virtual void addMessage(PlayerTypes ePlayer, bool bForce, int iLength, CvWString szString, LPCTSTR pszSound = NULL,
		InterfaceMessageTypes eType = MESSAGE_TYPE_INFO, LPCSTR pszIcon = NULL, ColorTypes eFlashColor = NO_COLOR,
		int iFlashX = -1, int iFlashY = -1, bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false) = 0;

	void addMessage(PlayerTypes ePlayer, bool bForce, int iLength, CvWString szString, const Coordinates coord, LPCTSTR pszSound,
			InterfaceMessageTypes eType = MESSAGE_TYPE_INFO, LPCSTR pszIcon = NULL, ColorTypes eFlashColor = NO_COLOR,
			bool bShowOffScreenArrows = false, bool bShowOnScreenArrows = false)
			{
				addMessage(ePlayer, bForce, iLength, szString, pszSound,
						eType, pszIcon, eFlashColor,
						coord.x(), coord.y(), bShowOffScreenArrows, bShowOnScreenArrows);
			}

public:
	virtual void addCombatMessage(PlayerTypes ePlayer, CvWString szString) = 0;
	virtual void addQuestMessage(PlayerTypes ePlayer, CvWString szString, int iQuestId) = 0;
	virtual void addTutorialMessage(PlayerTypes ePlayer, CvWString szString) = 0;
	virtual void showMessage(CvTalkingHeadMessage& msg) = 0;
	virtual void flushTalkingHeadMessages() = 0;
	virtual void clearEventMessages() = 0;
	virtual void addPopup(CvPopupInfo* pInfo, PlayerTypes ePlayer = NO_PLAYER, bool bImmediate = false, bool bFront = false) = 0;
	virtual void getDisplayedButtonPopups(CvPopupQueue& infos) = 0;

	virtual int getCycleSelectionCounter() = 0;
	virtual void setCycleSelectionCounter(int iNewValue) = 0;
	virtual void changeCycleSelectionCounter(int iChange) = 0;

	virtual int getEndTurnCounter() = 0;
	virtual void setEndTurnCounter(int iNewValue) = 0;
	virtual void changeEndTurnCounter(int iChange) = 0;

	virtual bool isCombatFocus() = 0;
	virtual void setCombatFocus(bool bNewValue) = 0;

	virtual bool isDirty(InterfaceDirtyBits eDirtyItem) = 0;
	virtual void setDirty(InterfaceDirtyBits eDirtyItem, bool bNewValue) = 0;
	virtual void makeInterfaceDirty() = 0;
	virtual bool updateCursorType() = 0;

	virtual void lookAt(NiPoint3 pt3Target, CameraLookAtTypes type, NiPoint3 attackDirection = NiPoint3(0, 1, 0)) = 0;
	virtual void centerCamera(CvUnit*) = 0;
	virtual void releaseLockedCamera() = 0;
	virtual bool isFocusedWidget() = 0;
	virtual bool isFocused() = 0;
	virtual bool isBareMapMode() = 0;
	virtual void toggleBareMapMode() = 0;
	virtual bool isShowYields() = 0;
	virtual void toggleYieldVisibleMode() = 0;
	virtual bool isScoresVisible() = 0;
	virtual void toggleScoresVisible() = 0;
	virtual bool isNetStatsVisible() = 0;

	virtual int getOriginalPlotCount() = 0;
	virtual bool isCityScreenUp() = 0;
	virtual bool isEndTurnMessage() = 0;
	virtual void setInterfaceMode(InterfaceModeTypes eNewValue) = 0;
	virtual void setFlashing(PlayerTypes eWho, bool bFlashing = true) = 0;
	virtual bool isFlashing(PlayerTypes eWho) = 0;
	virtual void setDiplomacyLocked(bool bLocked) = 0;
	virtual bool isDiplomacyLocked() = 0;
	virtual void setDiplomacyTransportId(PlayerTypes eWho, int iTransportId) = 0;
	virtual int getDiplomacyTransportId(PlayerTypes eWho) = 0;

	virtual void setMinimapColor(MinimapModeTypes eMinimapMode, int iX, int iY, ColorTypes eColor, float fAlpha) = 0;
	void setMinimapColor(MinimapModeTypes eMinimapMode, Coordinates coord, ColorTypes eColor, float fAlpha)
	{
		setMinimapColor(eMinimapMode, coord.x(), coord.y(), eColor, fAlpha);
	}
	virtual unsigned char* getMinimapBaseTexture() const = 0;
	virtual void setEndTurnMessage(bool bNewValue) = 0;

	virtual bool isHasMovedUnit() = 0;
	virtual void setHasMovedUnit(bool bNewValue) = 0;

	virtual bool isForcePopup() = 0;
	virtual void setForcePopup(bool bNewValue) = 0;

	virtual void lookAtCityOffset(int iCity) = 0;
	virtual void lookAtUnit(int iUnit) = 0;

	virtual void toggleTurnLog() = 0;
	virtual void showTurnLog(ChatTargetTypes eTarget = NO_CHATTARGET) = 0;
	virtual void dirtyTurnLog(PlayerTypes ePlayer) = 0;

	virtual int getPlotListColumn() = 0;
	virtual void verifyPlotListColumn() = 0;
	virtual int getPlotListOffset() = 0;
	virtual void toggleMultiRowPlotList() = 0;

	virtual void unlockPopupHelp() = 0;

	virtual void showDetails(bool bPasswordOnly = false) = 0;
	virtual void showAdminDetails() = 0;

	virtual void toggleClockAlarm(bool bValue, int iHour = 0, int iMin = 0) = 0;
	virtual bool isClockAlarmOn() = 0;

	virtual void setScreenDying(int iPythonFileID, bool bDying) = 0;
	virtual void exitingToMainMenu(const char* szLoadFile=NULL) = 0;
	virtual void setWorldBuilder(bool bTurnOn) = 0;

	virtual int getFontLeftJustify() = 0;
	virtual int getFontRightJustify() = 0;
	virtual int getFontCenterJustify() = 0;
	virtual int getFontCenterVertically() = 0;
	virtual int getFontAdditive() = 0;

	virtual void popupStartVLayout(CvPopup* pPopup, uint iFlags) = 0;
	virtual void popupStartHLayout(CvPopup* pPopup, uint iFlags) = 0;
	virtual void popupEndLayout(CvPopup* pPopup) = 0;
	virtual void popupSetHeaderString( CvPopup* pPopup, CvWString szText, uint uiFlags = DLL_FONT_CENTER_JUSTIFY ) = 0;
	virtual void popupSetBodyString( CvPopup* pPopup, CvWString szText, uint uiFlags = DLL_FONT_LEFT_JUSTIFY, char *szName = NULL, CvWString szHelpText = "" ) = 0;
	virtual void popupLaunch( CvPopup* pPopup, bool bCreateOkButton = true, PopupStates bState = POPUPSTATE_QUEUED, int iNumPixelScroll = 0 ) = 0;
	virtual void popupSetPopupType( CvPopup* pPopup, PopupEventTypes ePopupType, LPCTSTR szArtFileName = NULL ) = 0;
	virtual void popupSetStyle( CvPopup* pPopup, const char* styleId ) = 0;

	virtual void popupAddDDS( CvPopup* pPopup, const char* szIconFilename, int iWidth = 0, int iHeight = 0, CvWString szHelpText = "") = 0;

	virtual void popupAddSeparator( CvPopup* pPopup, int iSpace = 0) = 0;
	virtual void popupAddGenericButton( CvPopup* pPopup, CvWString szText, const char* szIcon = 0, int iButtonId = -1, WidgetTypes eWidgetType = WIDGET_GENERAL, int iData1 = MAX_INT, int iData2 = MAX_INT,
		bool bOption = true, PopupControlLayout ctrlLayout = POPUP_LAYOUT_CENTER, unsigned int textJustifcation = DLL_FONT_LEFT_JUSTIFY ) = 0;
	virtual void popupCreateEditBox( CvPopup* pPopup, CvWString szDefaultString = "", WidgetTypes eWidgetType = WIDGET_GENERAL, CvWString szHelpText = "", int iGroup = 0,
		PopupControlLayout ctrlLayout = POPUP_LAYOUT_STRETCH, unsigned int preferredCharWidth = 0, unsigned int maxCharCount = 256 ) = 0;
	virtual void popupEnableEditBox( CvPopup* pPopup, int iGroup = 0, bool bEnable = false ) = 0;
	virtual void popupCreateSpinBox(CvPopup* pPopup, int iIndex, const CvWString& szHelpText, int iDefault, int iIncrement, int iMax, int iMin) = 0;

	virtual void popupCreateRadioButtons( CvPopup * pPopup, int iNumButtons, int iGroup = 0, WidgetTypes eWidgetType = WIDGET_GENERAL, PopupControlLayout ctrlLayout = POPUP_LAYOUT_CENTER ) = 0;
	virtual void popupSetRadioButtonText( CvPopup * pPopup, int iRadioButtonID, CvWString szText, int iGroup = 0, CvWString szHelpText = "" ) = 0;

	virtual void popupCreateCheckBoxes( CvPopup* pPopup, int iNumBoxes, int iGroup = 0, WidgetTypes eWidgetType = WIDGET_GENERAL, PopupControlLayout ctrlLayout = POPUP_LAYOUT_CENTER ) = 0;
	virtual void popupSetCheckBoxText( CvPopup* pPopup, int iCheckBoxID, CvWString szText, int iGroup = 0, CvWString szHelpText = "") = 0;
	virtual void popupSetCheckBoxState( CvPopup* pPopup, int iCheckBoxID, bool bChecked, int iGroup = 0 ) = 0;

	virtual void popupSetAsCancelled(CvPopup* pPopup) = 0;
	virtual bool popupIsDying(CvPopup* pPopup) = 0;
	virtual bool isInAdvancedStart() const = 0;
	virtual void setInAdvancedStart(bool bAdvancedStart) = 0;

	virtual void setBusy(bool bBusy) = 0;
};


#endif // CvDLLInterfaceIFaceBase_h
