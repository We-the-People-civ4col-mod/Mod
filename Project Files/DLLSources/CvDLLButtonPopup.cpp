// buttonPopup.cpp

#include "CvGameCoreDLL.h"
#include "CvDLLButtonPopup.h"
#include "CvGlobals.h"
#include "CvPlayerAI.h"
#include "CvGameAI.h"
#include "CvTeamAI.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvArtFileMgr.h"
#include "CyCity.h"
#include "CvTradeRoute.h"
#include "CyArgsList.h"
#include "CvPopupReturn.h"
#include "CvInfos.h"
#include "CvInitCore.h"
#include "CvGameTextMgr.h"
#include "CvDLLPythonIFaceBase.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLEntityIFaceBase.h"
#include "CvGameCoreUtils.h"
#include "CvDLLEngineIFaceBase.h"
#include "CvDLLEventReporterIFaceBase.h"

// Public Functions...

#define PASSWORD_DEFAULT (L"*****")

// static class to allow launch and clicked code for a popup to be put together
// being a static class rather than just two functions allows adding enums, which can only be used inside the class
// this in turn allows assigning enums to button IDs without risking name clashes between popup windows
template <int T>
class PopupButtonContainer
{
};

template <>
class PopupButtonContainer<BUTTONPOPUP_START_GAME_CHECK_FAILED>
{
	enum ErrorTypes
	{
		TEST_WETLAND,
		TEST_AI_PLAYER,
	};
public:
	static bool launch(CvPopup* pPopup, CvPopupInfo &info)
	{
		CvWString error;
		error
			.append(L"<color=255,0,0,0>")
			.append(gDLL->getText("TXT_KEY_STARTUP_TEST_ERROR"))
			.append(L"</color>\n")
			;
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, error);
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, getErrorText((ErrorTypes)info.getData1()));

		gDLL->getInterfaceIFace()->popupLaunch(pPopup, true, POPUPSTATE_IMMEDIATE);

		return true;
	}

	static void clicked(PopupReturn *pPopupReturn)
	{
	}

	// technically this doesn't belong here, but by placing it inside the class it gains access to the enum
	static void runTest()
	{
		if (GC.getMap().getNumPlots(TERRAIN_WETLAND) == 0)
		{
			displayError(TEST_WETLAND);
		}
		if (CvGame::countCivPlayerEuropeanAI() == 0)
		{
			displayError(TEST_AI_PLAYER);
		}
	}
private:
	static void displayError(ErrorTypes eError)
	{
		CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_START_GAME_CHECK_FAILED, eError);
		gDLL->getInterfaceIFace()->addPopup(pInfo, GC.getGameINLINE().getActivePlayer(), false, true);
	}

	static CvWString getErrorText(ErrorTypes eError)
	{
		switch (eError)
		{
		case TEST_WETLAND: return gDLL->getText("TXT_KEY_STARTUP_TEST_WETLANDS");
		case TEST_AI_PLAYER: return gDLL->getText("TXT_KEY_STARTUP_TEST_AI_PLAYERS");
		default:
			FAssert(false);
			return L"error message missing";
		}
	}
};
void doNewGameErrorTesting()
{
	PopupButtonContainer<BUTTONPOPUP_START_GAME_CHECK_FAILED>::runTest();
}

template <>
class PopupButtonContainer<BUTTONPOPUP_NETWORK_OOS_MENU>
{
	enum
	{
		BUTTOM_TEST_canDoEvent,
		BUTTON_TEST_canDoGoody,
	};
public:
	static bool launch(CvPopup* pPopup, CvPopupInfo &info)
	{
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_DEBUG_OOS_MENU"));
		
		// TODO: figure out how to simulate EventTriggeredData
		//gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, "CvPlayer::canDoEvent", 0, BUTTOM_TEST_canDoEvent);

		if (gDLL->getInterfaceIFace()->getHeadSelectedUnit() != NULL)
		{
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, "CvUnit::canDoGoody", 0, BUTTON_TEST_canDoGoody);
		}

		gDLL->getInterfaceIFace()->popupLaunch(pPopup, true, POPUPSTATE_IMMEDIATE);

		return true;
	}

	static void clicked(PopupReturn *pPopupReturn)
	{
		const PlayerTypes ePlayer = GC.getGameINLINE().getActivePlayer();
		CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);

		switch (pPopupReturn->getButtonClicked())
		{
		case BUTTOM_TEST_canDoEvent:
			for (EventTypes eEvent = FIRST_EVENT; eEvent < NUM_EVENT_TYPES; ++eEvent)
			{
				EventTriggeredData kTriggeredData;
				bool bSuccess = kPlayer.canDoEvent(eEvent, kTriggeredData);
				gDLL->sendPlayerAction(ePlayer, PLAYER_ACTION_NETWORK_canDoEvent, eEvent, bSuccess, -1);
			}
			break;
		case BUTTON_TEST_canDoGoody:
			{
				CvUnit* pUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();
				if (pUnit != NULL)
				{
					FAssert(pUnit->getOwnerINLINE() == ePlayer);
					for (GoodyTypes eGoody = FIRST_GOODY; eGoody < NUM_GOODY_TYPES; ++eGoody)
					{
						bool bSuccess = kPlayer.canReceiveGoody(pUnit->plot(), eGoody, pUnit);
						gDLL->sendPlayerAction(ePlayer, PLAYER_ACTION_NETWORK_canDoGoody, eGoody, pUnit->getID(), bSuccess);
					}
				}
			}
		}
	}
};

// WTP enums
// Nake it easier to read windows with a lot of buttons
// This also allows easy access to changing the order of buttons

enum PopupButtonsMainMenu
{
	PopupButtonsMainMenu_ExitDesktop,
	PopupButtonsMainMenu_ExitMenu,
	PopupButtonsMainMenu_Retire,
	PopupButtonsMainMenu_RegenerateMap,
	PopupButtonsMainMenu_Load,
	PopupButtonsMainMenu_Save,
	PopupButtonsMainMenu_Options,
	PopupButtonsMainMenu_WorldBuilder,
	PopupButtonsMainMenu_GameDetails,
	PopupButtonsMainMenu_DetailsTitle,
	PopupButtonsMainMenu_Cancel,
	PopupButtonsMainMenu_OOS,

};

// WTP end

CvDLLButtonPopup* CvDLLButtonPopup::m_pInst = NULL;

CvDLLButtonPopup& CvDLLButtonPopup::getInstance()
{
	if (m_pInst == NULL)
	{
		m_pInst = new CvDLLButtonPopup;
	}
	return *m_pInst;
}

void CvDLLButtonPopup::freeInstance()
{
	delete m_pInst;
	m_pInst = NULL;
}

CvDLLButtonPopup::CvDLLButtonPopup()
{
}


CvDLLButtonPopup::~CvDLLButtonPopup()
{
}

void CvDLLButtonPopup::OnAltExecute(CvPopup& popup, const PopupReturn& popupReturn, CvPopupInfo &info)
{
	switch (info.getButtonPopupType())
	{
	case BUTTONPOPUP_CHOOSE_PROFESSION:
		{
			CvPopupInfo* pInfo = new CvPopupInfo;
			if (pInfo)
			{
				*pInfo = info;
				gDLL->getInterfaceIFace()->addPopup(pInfo);
				gDLL->getInterfaceIFace()->popupSetAsCancelled(&popup);
			}
		}
	default:
		break;
	}
}


void CvDLLButtonPopup::OnEscape(CvPopup& popup, CvPopupInfo &info)
{
	switch (info.getButtonPopupType())
	{
	case BUTTONPOPUP_MAIN_MENU:
	case BUTTONPOPUP_CHOOSE_PROFESSION:
	case BUTTONPOPUP_PURCHASE_EUROPE_UNIT:
	case BUTTONPOPUP_PURCHASE_AFRICA_UNIT:
	case BUTTONPOPUP_PURCHASE_PORT_ROYAL_UNIT:
	case BUTTONPOPUP_LOADUNIT:
	case BUTTONPOPUP_LOAD_CARGO:
	case BUTTONPOPUP_LEADUNIT:
	case BUTTONPOPUP_DIPLOMACY:
	case BUTTONPOPUP_FREE_COLONY:
	case BUTTONPOPUP_PROMOTE:
	case BUTTONPOPUP_SELECT_YIELD_AMOUNT:
	case BUTTONPOPUP_DETAILS:
	case BUTTONPOPUP_ADMIN_PASSWORD:
	case BUTTONPOPUP_ADMIN:
	case BUTTONPOPUP_TALK_NATIVES:
	case BUTTONPOPUP_FEAT:
	case BUTTONPOPUP_CONFIRMTASK:
	case BUTTONPOPUP_CONFIRMCOMMAND:
	case BUTTONPOPUP_ACHIEVEMENTS: //closing achievement window with escape
	case BUTTONPOPUP_GOTO_MENU:		// TAC - Goto Menu - koma13
	case BUTTONPOPUP_CHOOSE_CITY_PLOT_YIELD:
		gDLL->getInterfaceIFace()->popupSetAsCancelled(&popup);
		break;
	default:
		break;
	}
}


void CvDLLButtonPopup::OnOkClicked(CvPopup* pPopup, PopupReturn *pPopupReturn, CvPopupInfo &info)
{
	int iExamineCityID;

	switch (info.getButtonPopupType())
	{
	case BUTTONPOPUP_TEXT:
		break;

	case BUTTONPOPUP_CONFIRM_MENU:
		if ( pPopupReturn->getButtonClicked() == 0 )
		{
			switch (info.getData1())
			{
			case 0:
				gDLL->SetDone(true);
				break;
			case 1:
				gDLL->getInterfaceIFace()->exitingToMainMenu();
				break;
			case 2:
				GC.getGameINLINE().doControl(CONTROL_RETIRE);
				break;
			case 3:
				GC.getGameINLINE().regenerateMap();
				break;
			case 4:
				GC.getGameINLINE().doControl(CONTROL_WORLD_BUILDER);
				break;
			}
		}
		break;

	case BUTTONPOPUP_MAIN_MENU:
		if (pPopupReturn->getButtonClicked() == PopupButtonsMainMenu_ExitDesktop)
		{	// exit to desktop
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CONFIRM_MENU);
			if (NULL != pInfo)
			{
				pInfo->setData1(0);
				gDLL->getInterfaceIFace()->addPopup(pInfo, GC.getGameINLINE().getActivePlayer(), true);
			}
		}
		else if (pPopupReturn->getButtonClicked() == PopupButtonsMainMenu_ExitMenu)
		{	// exit to main menu
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CONFIRM_MENU);
			if (NULL != pInfo)
			{
				pInfo->setData1(1);
				gDLL->getInterfaceIFace()->addPopup(pInfo, GC.getGameINLINE().getActivePlayer(), true);
			}
		}
		else if (pPopupReturn->getButtonClicked() == PopupButtonsMainMenu_Retire)
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CONFIRM_MENU);
			if (NULL != pInfo)
			{
				pInfo->setData1(2);
				gDLL->getInterfaceIFace()->addPopup(pInfo, GC.getGameINLINE().getActivePlayer(), true);
			}
		}
		else if (pPopupReturn->getButtonClicked() == PopupButtonsMainMenu_RegenerateMap)
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CONFIRM_MENU);
			if (NULL != pInfo)
			{
				pInfo->setData1(3);
				gDLL->getInterfaceIFace()->addPopup(pInfo, GC.getGameINLINE().getActivePlayer(), true);
			}
		}
		else if (pPopupReturn->getButtonClicked() == PopupButtonsMainMenu_Load)
		{	// load game
			GC.getGameINLINE().doControl(CONTROL_LOAD_GAME);
		}
		else if (pPopupReturn->getButtonClicked() == PopupButtonsMainMenu_Save)
		{	// save game
			GC.getGameINLINE().doControl(CONTROL_SAVE_NORMAL);
		}
		else if (pPopupReturn->getButtonClicked() == PopupButtonsMainMenu_Options)
		{	// options
			gDLL->getPythonIFace()->callFunction("CvScreensInterface", "showOptionsScreen");
		}
		else if (pPopupReturn->getButtonClicked() == PopupButtonsMainMenu_WorldBuilder)
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CONFIRM_MENU);
			if (NULL != pInfo)
			{
				pInfo->setData1(4);
				gDLL->getInterfaceIFace()->addPopup(pInfo, GC.getGameINLINE().getActivePlayer(), true);
			}
		}
		else if (pPopupReturn->getButtonClicked() == PopupButtonsMainMenu_GameDetails)
		{	// Game details
			GC.getGameINLINE().doControl(CONTROL_ADMIN_DETAILS);
		}
		else if (pPopupReturn->getButtonClicked() == PopupButtonsMainMenu_DetailsTitle)
		{	// player details
			GC.getGameINLINE().doControl(CONTROL_DETAILS);
		}

		else if (pPopupReturn->getButtonClicked() == PopupButtonsMainMenu_OOS)
		{	// write network desync log. Has to be done in sync, hence send network traffic
			gDLL->sendPlayerAction(static_cast<PlayerTypes>(0), PLAYER_ACTION_NETWORK_DESYNC_LOG_WRITE, -1, -1, -1);
		}

		// 10 - cancel
		break;

	case BUTTONPOPUP_DECLAREWARMOVE:
		if (pPopupReturn->getButtonClicked() == 0)
		{
			gDLL->sendChangeWar((TeamTypes)info.getData1(), true);
		}
		if (((pPopupReturn->getButtonClicked() == 0) || info.getOption2()) && info.getFlags() == 0)
		{
			GC.getGameINLINE().selectionListGameNetMessage(GAMEMESSAGE_PUSH_MISSION, MISSION_MOVE_TO, info.getData2(), info.getData3(), info.getFlags(), false, info.getOption1());
		}
		break;

	case BUTTONPOPUP_CONFIRMCOMMAND:
		if (pPopupReturn->getButtonClicked() == 0)
		{
			int iAction = info.getData1();
			GC.getGameINLINE().selectionListGameNetMessage(GAMEMESSAGE_DO_COMMAND, GC.getActionInfo(iAction).getCommandType(), GC.getActionInfo(iAction).getCommandData(), -1, 0, info.getOption1());
		}
		break;

	case BUTTONPOPUP_CONFIRMTASK:
		if (pPopupReturn->getButtonClicked() == 0)
		{
			gDLL->sendDoTask(info.getData1(), (TaskTypes) info.getData3(), info.getData2(), -1, false, false, false, false);
		}
		break;

	case BUTTONPOPUP_LOADUNIT:
		if (pPopupReturn->getButtonClicked() != 0)
		{
			CLLNode<IDInfo>* pUnitNode;
			CvSelectionGroup* pSelectionGroup;
			CvUnit* pLoopUnit;
			CvPlot* pPlot;
			int iCount;

			pSelectionGroup = gDLL->getInterfaceIFace()->getSelectionList();

			if (NULL != pSelectionGroup)
			{
				pPlot = pSelectionGroup->plot();

				iCount = pPopupReturn->getButtonClicked();

				pUnitNode = pPlot->headUnitNode();

				while (pUnitNode != NULL)
				{
					pLoopUnit = ::getUnit(pUnitNode->m_data);
					pUnitNode = pPlot->nextUnitNode(pUnitNode);

					if (pLoopUnit != NULL && pSelectionGroup->canDoCommand(COMMAND_LOAD_UNIT, pLoopUnit->getOwnerINLINE(), pLoopUnit->getID()))
					{
						iCount--;
						if (iCount == 0)
						{
							GC.getGameINLINE().selectionListGameNetMessage(GAMEMESSAGE_DO_COMMAND, COMMAND_LOAD_UNIT, pLoopUnit->getOwnerINLINE(), pLoopUnit->getID());
							break;
						}
					}
				}
			}
		}
		break;

	case BUTTONPOPUP_LOAD_CARGO:
		if (pPopupReturn->getButtonClicked() < NUM_YIELD_TYPES)
		{
			CvSelectionGroup* pSelectionGroup = gDLL->getInterfaceIFace()->getSelectionList();

			if (NULL != pSelectionGroup)
			{
				YieldTypes eYield = (YieldTypes) pPopupReturn->getButtonClicked();

				//find transport that can carry yield
				int iNumUnits = pSelectionGroup->getNumUnits();
				for (int i = 0; i < iNumUnits; ++i)
				{
					CvUnit* pTransport = pSelectionGroup->getUnitAt(i);
					if (NULL != pTransport)
					{
						if (pTransport->canLoadYield(pTransport->plot(), eYield, false))
						{
							gDLL->sendDoCommand(pTransport->getID(), COMMAND_LOAD_YIELD, eYield, -1, false);
							gDLL->sendDoCommand(pTransport->getID(), COMMAND_LOAD_CARGO, -1, -1, false);
							break;
						}
					}
				}
			}
		}
		break;

	case BUTTONPOPUP_LEADUNIT:
		if (pPopupReturn->getButtonClicked() != 0)
		{
			CvUnit* pLeaderUnit = GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getUnit(info.getData2());
			if (pLeaderUnit != NULL)
			{
				CvSelectionGroup* pSelectionGroup = pLeaderUnit->getGroup();
				if (NULL != pSelectionGroup)
				{
					CvPlot* pPlot = pSelectionGroup->plot();
					int iCount = pPopupReturn->getButtonClicked();

					CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
					while (pUnitNode != NULL)
					{
						CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
						pUnitNode = pPlot->nextUnitNode(pUnitNode);

						// WTP, fixing Generals and Admirals to lead civilists or small tiny fishing boats - START
						if (pLoopUnit != NULL && ((pLoopUnit->getDomainType() == DOMAIN_LAND && pLoopUnit->canAttack()) || (pLoopUnit->getDomainType() == DOMAIN_SEA && pLoopUnit->baseCombatStr() >= 20)))
						{
							if (pLoopUnit->canPromote((PromotionTypes) info.getData1(), info.getData2()))
							{
								iCount--;
								if (iCount == 0)
								{
									gDLL->sendPushMission(info.getData2(), MISSION_LEAD, pLoopUnit->getID(), -1, 0, false);
									break;
								}
							}
						}
					}
				}
			}
		}
		break;
	case BUTTONPOPUP_RAZECITY:
		if (pPopupReturn->getButtonClicked() == 1)
		{
			gDLL->sendDoTask(info.getData1(), TASK_RAZE, -1, -1, false, false, false, false);
		}
		else if (pPopupReturn->getButtonClicked() == 2)
		{
			CvCity* pCity = GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getCity(info.getData1());
			if (NULL != pCity)
			{
				gDLL->getEventReporterIFace()->cityAcquiredAndKept(GC.getGameINLINE().getActivePlayer(), pCity);
			}

			gDLL->sendDoTask(info.getData1(), TASK_GIFT, info.getData2(), -1, false, false, false, false);
		}
		else if (pPopupReturn->getButtonClicked() == 0)
		{
			CvCity* pCity = GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getCity(info.getData1());
			if (NULL != pCity)
			{
				pCity->chooseProduction();
				gDLL->getEventReporterIFace()->cityAcquiredAndKept(GC.getGameINLINE().getActivePlayer(), pCity);
			}
		}
		break;

	case BUTTONPOPUP_CHOOSEPRODUCTION:
		iExamineCityID = 0;
		iExamineCityID = std::max(iExamineCityID, GC.getNumUnitInfos());
		iExamineCityID = std::max(iExamineCityID, GC.getNumBuildingInfos());

		if (pPopupReturn->getButtonClicked() == iExamineCityID)
		{
			CvCity* pCity = GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getCity(info.getData1());
			if (pCity != NULL)
			{
				gDLL->getInterfaceIFace()->selectCity(pCity, true);
			}
		}
		break;

	case BUTTONPOPUP_CHOOSE_YIELD_BUILD:
// Ramstormp, WtP, Add Examine Settlement option to the no longer lacking yields for building production popup - START
		iExamineCityID = 0;
		iExamineCityID = std::max(iExamineCityID, GC.getNumUnitInfos());
		iExamineCityID = std::max(iExamineCityID, GC.getNumBuildingInfos());

		if (pPopupReturn->getButtonClicked() == iExamineCityID)
		{
			CvCity* pCity = GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getCity(info.getData1());
			if (pCity != NULL)
			{
				gDLL->getInterfaceIFace()->selectCity(pCity, true);
			}
		}

		else if (pPopupReturn->getButtonClicked() >= GC.getNumUnitInfos())
// Ramstormp - END
		{
			BuildingTypes eBuilding = (BuildingTypes) (pPopupReturn->getButtonClicked() - GC.getNumUnitInfos());
			gDLL->sendDoTask(info.getData1(), TASK_PUSH_CONSTRUCT_BUILDING, eBuilding, -1, false, false, false, false);
		}
		else if (pPopupReturn->getButtonClicked() >= 0)
		{
			UnitTypes eUnit = (UnitTypes) pPopupReturn->getButtonClicked();
			gDLL->sendDoTask(info.getData1(), TASK_PUSH_TRAIN_UNIT, eUnit, NO_UNITAI, false, false, false, false);
		}
		break;

	case BUTTONPOPUP_CHOOSE_EDUCATION:
		if (pPopupReturn->getButtonClicked() == GC.getNumUnitInfos())
		{
			CvCity* pCity = GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getCity(info.getData1());
			if (pCity != NULL)
			{
				gDLL->getInterfaceIFace()->selectCity(pCity, true);

				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CHOOSE_EDUCATION, info.getData1(), info.getData2());
				gDLL->getInterfaceIFace()->addPopup(pInfo, GC.getGameINLINE().getActivePlayer(), false, true);
			}
		}
		else if (pPopupReturn->getButtonClicked() >= 0)
		{
			gDLL->sendDoTask(info.getData1(), TASK_EDUCATE, info.getData2(), pPopupReturn->getButtonClicked(), false, false, false, false);
		}
		break;

	case BUTTONPOPUP_ALARM:
		break;

	case BUTTONPOPUP_DEAL_CANCELED:
		if (pPopupReturn->getButtonClicked() == 0)
		{
			gDLL->sendKillDeal(info.getData1(), info.getOption1(), GC.getGameINLINE().getActiveTeam());
		}
		break;

	case BUTTONPOPUP_PYTHON:
	case BUTTONPOPUP_FEAT:
		if (!info.getOnClickedPythonCallback().IsEmpty())
		{
			FAssertMsg(!GC.getGameINLINE().isNetworkMultiPlayer(), "Danger: Out of Sync");
			CyArgsList argsList;
			argsList.add(pPopupReturn->getButtonClicked());
			argsList.add(info.getData1());
			argsList.add(info.getData2());
			argsList.add(info.getData3());
			argsList.add(info.getFlags());
			argsList.add(info.getText());
			argsList.add(info.getOption1());
			argsList.add(info.getOption2());
			gDLL->getPythonIFace()->callFunction((info.getPythonModule().IsEmpty() ? PYScreensModule : info.getPythonModule()), info.getOnClickedPythonCallback(), argsList.makeFunctionArgs());
			break;
		}
		break;

	case BUTTONPOPUP_DETAILS:
		{
			// Civ details
			PlayerTypes eID = GC.getInitCore().getActivePlayer();

			CvWString szLeaderName = GC.getInitCore().getLeaderName(eID);
			CvWString szCivDescription = GC.getInitCore().getCivDescription(eID);
			CvWString szCivShortDesc = GC.getInitCore().getCivShortDesc(eID);
			CvWString szCivAdjective = GC.getInitCore().getCivAdjective(eID);
			CvWString szCivPassword = PASSWORD_DEFAULT;
			CvString szEmail = GC.getInitCore().getEmail(eID);
			CvString szSmtpHost = GC.getInitCore().getSmtpHost(eID);

			if (pPopupReturn->getEditBoxString(0) && *(pPopupReturn->getEditBoxString(0)))
			{
				szLeaderName = pPopupReturn->getEditBoxString(0);
			}
			if (pPopupReturn->getEditBoxString(1) && *(pPopupReturn->getEditBoxString(1)))
			{
				szCivDescription = pPopupReturn->getEditBoxString(1);
			}
			if (pPopupReturn->getEditBoxString(2) && *(pPopupReturn->getEditBoxString(2)))
			{
				szCivShortDesc = pPopupReturn->getEditBoxString(2);
			}
			if (pPopupReturn->getEditBoxString(3) && *(pPopupReturn->getEditBoxString(3)))
			{
				szCivAdjective = pPopupReturn->getEditBoxString(3);
			}
			if (GC.getGameINLINE().isHotSeat() || GC.getGameINLINE().isPbem())
			{
				if (pPopupReturn->getEditBoxString(4) && *(pPopupReturn->getEditBoxString(4)))
				{
					szCivPassword = pPopupReturn->getEditBoxString(4);
				}
			}
			if (GC.getGameINLINE().isPitboss() || GC.getGameINLINE().isPbem())
			{
				if (pPopupReturn->getEditBoxString(5) && *(pPopupReturn->getEditBoxString(5)))
				{
					szEmail = CvString(pPopupReturn->getEditBoxString(5));
				}
			}
			if (GC.getGameINLINE().isPbem())
			{
				if (pPopupReturn->getEditBoxString(6) && *(pPopupReturn->getEditBoxString(6)))
				{
					szSmtpHost = CvString(pPopupReturn->getEditBoxString(6));
				}
			}

			GC.getInitCore().setLeaderName(eID, szLeaderName);
			GC.getInitCore().setCivDescription(eID, szCivDescription);
			GC.getInitCore().setCivShortDesc(eID, szCivShortDesc);
			GC.getInitCore().setCivAdjective(eID, szCivAdjective);
			if (szCivPassword != PASSWORD_DEFAULT)
			{
				GC.getInitCore().setCivPassword(eID, szCivPassword);
			}
			GC.getInitCore().setEmail(eID, szEmail);
			GC.getInitCore().setSmtpHost(eID, szSmtpHost);
			gDLL->sendPlayerInfo(eID);

			if (GC.getGameINLINE().isPbem() && pPopupReturn->getButtonClicked() == 0)
			{
				gDLL->sendPbemTurn(NO_PLAYER);
			}

		}
		break;

	case BUTTONPOPUP_ADMIN:
		{
			// Game details
			CvWString szGameName;
			CvWString szAdminPassword = GC.getInitCore().getAdminPassword();
			if (pPopupReturn->getEditBoxString(0) && *(pPopupReturn->getEditBoxString(0)))
			{
				szGameName = pPopupReturn->getEditBoxString(0);
			}
			if (pPopupReturn->getEditBoxString(1) && CvWString(pPopupReturn->getEditBoxString(1)) != PASSWORD_DEFAULT)
			{
				if (*(pPopupReturn->getEditBoxString(1)))
				{
					szAdminPassword = CvWString(gDLL->md5String((char*)CvString(pPopupReturn->getEditBoxString(1)).GetCString()));
				}
				else
				{
					szAdminPassword = L"";
				}
			}
			if (!GC.getGameINLINE().isGameMultiPlayer())
			{
				if (pPopupReturn->getCheckboxBitfield(2) > 0)
				{
					gDLL->setChtLvl(1);
				}
				else
				{
					gDLL->setChtLvl(0);
				}
			}

			gDLL->sendGameInfo(szGameName, szAdminPassword);

		}
		break;

	case BUTTONPOPUP_ADMIN_PASSWORD:
		{
			CvWString szAdminPassword;
			if (pPopupReturn->getEditBoxString(0) && CvWString(pPopupReturn->getEditBoxString(0)) != PASSWORD_DEFAULT)
			{
				szAdminPassword = pPopupReturn->getEditBoxString(0);
			}
			if (CvWString(gDLL->md5String((char*)CvString(szAdminPassword).GetCString())) == GC.getInitCore().getAdminPassword())
			{
				switch ((ControlTypes)info.getData1())
				{
				case CONTROL_WORLD_BUILDER:
					gDLL->getInterfaceIFace()->setWorldBuilder(!(gDLL->GetWorldBuilderMode()));
					break;
				case CONTROL_ADMIN_DETAILS:
					gDLL->getInterfaceIFace()->showAdminDetails();
					break;
				default:
					break;
				}
			}
			else
			{
				CvPopupInfo* pInfo = new CvPopupInfo();
				if (NULL != pInfo)
				{
					pInfo->setText(gDLL->getText("TXT_KEY_BAD_PASSWORD_DESC"));
					gDLL->getInterfaceIFace()->addPopup(pInfo, NO_PLAYER, true);
				}
			}
		}
		break;

	case BUTTONPOPUP_EXTENDED_GAME:
		if (pPopupReturn->getButtonClicked() == 0)
		{
			if (GC.getGameINLINE().isNetworkMultiPlayer())
			{
				gDLL->sendExtendedGame();
			}
			else
			{
				GC.getGameINLINE().setGameState(GAMESTATE_EXTENDED);
			}
		}
		else if (pPopupReturn->getButtonClicked() == 1)
		{
			// exit to main menu
			if (GC.getGameINLINE().isNetworkMultiPlayer() && GC.getGameINLINE().canDoControl(CONTROL_RETIRE) && GC.getGameINLINE().countHumanPlayersAlive() > 1)
			{
				GC.getGameINLINE().doControl(CONTROL_RETIRE);
			}
			else
			{
				gDLL->getInterfaceIFace()->exitingToMainMenu();
			}
		}
		break;

	case BUTTONPOPUP_DIPLOMACY:
		if (pPopupReturn->getButtonClicked() != MAX_PLAYERS)
		{
			GET_PLAYER(GC.getGameINLINE().getActivePlayer()).contact((PlayerTypes)(pPopupReturn->getButtonClicked()));
		}
		break;

	case BUTTONPOPUP_ADDBUDDY:
		if (pPopupReturn->getButtonClicked() == 0)
		{
			gDLL->AcceptBuddy(CvString(info.getText()).GetCString(), info.getData1());
		}
		else
		{
			gDLL->RejectBuddy(CvString(info.getText()).GetCString(), info.getData1());
		}
		break;

	case BUTTONPOPUP_FORCED_DISCONNECT:
	case BUTTONPOPUP_PITBOSS_DISCONNECT:
	case BUTTONPOPUP_KICKED:
		gDLL->getInterfaceIFace()->exitingToMainMenu();
		break;

	case BUTTONPOPUP_EVENT:
		{
			CvPlayer& kActivePlayer = GET_PLAYER(GC.getGameINLINE().getActivePlayer());
			EventTriggeredData* pTriggeredData = kActivePlayer.getEventTriggered(info.getData1());

			if (pPopupReturn->getButtonClicked() == GC.getNumEventInfos())
			{
				if (NULL != pTriggeredData)
				{
					CvCity* pCity = kActivePlayer.getCity(pTriggeredData->m_iCityId);
					if (NULL != pCity)
					{
						gDLL->getInterfaceIFace()->selectCity(pCity, true);
					}
				}

				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_EVENT, info.getData1());
				gDLL->getInterfaceIFace()->addPopup(pInfo, GC.getGameINLINE().getActivePlayer(), false, true);
			}
			else if (-1 != pPopupReturn->getButtonClicked())
			{
				if (NULL != pTriggeredData && GC.getEventTriggerInfo(pTriggeredData->m_eTrigger).isSinglePlayer())
				{
					GET_PLAYER(GC.getGameINLINE().getActivePlayer()).doAction(PLAYER_ACTION_APPLY_EVENT, pPopupReturn->getButtonClicked(), info.getData1(), -1);
				}
				else
				{
					gDLL->sendPlayerAction(GC.getGameINLINE().getActivePlayer(), PLAYER_ACTION_APPLY_EVENT, pPopupReturn->getButtonClicked(), info.getData1(), -1);
				}
			}
		}
		break;

	case BUTTONPOPUP_FREE_COLONY:
		if (pPopupReturn->getButtonClicked() >= 0)
		{
			gDLL->sendDoTask(pPopupReturn->getButtonClicked(), TASK_LIBERATE, 0, -1, false, false, false, false);
		}
		break;

	case BUTTONPOPUP_CHOOSE_PROFESSION:
		if (pPopupReturn->getButtonClicked() == -1)
		{
			gDLL->sendDoTask(info.getData1(), TASK_SET_CITIZEN_AUTOMATED, info.getData2(), pPopupReturn->getButtonClicked(), false, false, false, false);
		}
		else if (pPopupReturn->getButtonClicked() == -2)
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CONFIRMTASK, info.getData1(), info.getData2(), TASK_CLEAR_SPECIALTY);
			pInfo->setText(GC.getCommandInfo(COMMAND_CLEAR_SPECIALTY).getTextKeyWide());
			gDLL->getInterfaceIFace()->addPopup(pInfo, NO_PLAYER, true);
		}
		else if (pPopupReturn->getButtonClicked() < GC.getNumProfessionInfos())
		{
			if (GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getUnit(info.getData2()) != NULL)
			{
				gDLL->sendDoCommand(info.getData2(), COMMAND_PROFESSION, pPopupReturn->getButtonClicked(), -1, false);
			}
			else if (GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getEuropeUnitById(info.getData2()) != NULL)
			{
				gDLL->sendPlayerAction(GC.getGameINLINE().getActivePlayer(), PLAYER_ACTION_EUROPE_CHANGE_PROFESSION, info.getData2(), pPopupReturn->getButtonClicked(), -1);
			}
			else
			{
				gDLL->sendDoTask(info.getData1(), TASK_CHANGE_PROFESSION, info.getData2(), pPopupReturn->getButtonClicked(), false, false, false, false);
			}
		}
		break;

	case BUTTONPOPUP_PURCHASE_EUROPE_UNIT:
		if (pPopupReturn->getButtonClicked() < GC.getNumUnitInfos())
		{
			gDLL->sendPlayerAction(GC.getGameINLINE().getActivePlayer(), PLAYER_ACTION_BUY_EUROPE_UNIT, pPopupReturn->getButtonClicked(), -1, -1);
		}
		break;

	/*** TRIANGLETRADE 10/24/08 by DPII ***/
	case BUTTONPOPUP_PURCHASE_AFRICA_UNIT:
		if (pPopupReturn->getButtonClicked() < GC.getNumUnitInfos())
		{
			gDLL->sendPlayerAction(GC.getGameINLINE().getActivePlayer(), PLAYER_ACTION_BUY_AFRICA_UNIT, pPopupReturn->getButtonClicked(), -1, -1);
		}
		break;
	/*************************************/

	// R&R, ray, Port Royal
	case BUTTONPOPUP_PURCHASE_PORT_ROYAL_UNIT:
		if (pPopupReturn->getButtonClicked() < GC.getNumUnitInfos())
		{
			gDLL->sendPlayerAction(GC.getGameINLINE().getActivePlayer(), PLAYER_ACTION_BUY_PORT_ROYAL_UNIT, pPopupReturn->getButtonClicked(), -1, -1);
		}
		break;
	// R&R, ray, Port Royal - END

	//R&R mod, vetiarvind, trade groups - start
	case BUTTONPOPUP_SAVE_TRADEGROUP:
		if (pPopupReturn->getButtonClicked() != -2) //not nvm button
		{
			const wchar* szGroupName = pPopupReturn->getEditBoxString(0);
			CvPlayer& player = GET_PLAYER(GC.getGameINLINE().getActivePlayer());
			int lastAddedId = player.addTradeRouteGroup(szGroupName);
			CvTradeRouteGroup* addedTradeGroup = player.getTradeRouteGroupById(lastAddedId)						;
			CvString str(info.getText().c_str());

			std::vector<std::string> tradeRoutes = split(str, ',');
			for(uint i = 0 ; i < tradeRoutes.size(); ++i)
			{
				CvString tradeRouteStr = CvString(tradeRoutes[i]);
				std::vector<std::string> routeDetails = split(tradeRouteStr, ' ');
				int srcId = atoi(routeDetails[0].c_str());
				int destId = atoi(routeDetails[1].c_str());
				IDInfo europeCity((PlayerTypes)GC.getGameINLINE().getActivePlayer(),CvTradeRoute::EUROPE_CITY_ID);

				addedTradeGroup->addRoute(
					srcId != CvTradeRoute::EUROPE_CITY_ID ?	player.getCity(srcId)->getIDInfo() : europeCity,
					destId != CvTradeRoute::EUROPE_CITY_ID ?  player.getCity(destId)->getIDInfo() : europeCity,
					(YieldTypes) atoi(routeDetails[2].c_str()));
			}

		}
		break;

	//R&R mod, vetiarvind, trade groups - end

	case BUTTONPOPUP_FOUNDING_FATHER:
		if (pPopupReturn->getButtonClicked() == -1)
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_FOUNDING_FATHER, info.getData1(), info.getData2(), info.getData3(), info.getFlags(), info.getOption1(), info.getOption2());
			gDLL->getInterfaceIFace()->addPopup(pInfo, GC.getGameINLINE().getActivePlayer(), false, true);

			pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN);
			pInfo->setText(L"showFoundingFatherScreen");
			pInfo->setData1(GC.getFatherInfo((FatherTypes)info.getData1()).getFatherCategory());
			gDLL->getInterfaceIFace()->addPopup(pInfo, GC.getGameINLINE().getActivePlayer(), false, true);
		}
		else
		{
			gDLL->sendPlayerAction(GC.getGameINLINE().getActivePlayer(), PLAYER_ACTION_CONVINCE_FATHER, info.getData1(), pPopupReturn->getButtonClicked(), -1);
		}
		break;

	case BUTTONPOPUP_CIVIC_OPTION:
		if (pPopupReturn->getButtonClicked() == -1)
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CIVIC_OPTION, info.getData1(), info.getData2(), info.getData3(), info.getFlags(), info.getOption1(), info.getOption2());
			gDLL->getInterfaceIFace()->addPopup(pInfo, GC.getGameINLINE().getActivePlayer(), false, true);

			CivicTypes eCivic = NO_CIVIC;
			for (int iCivic = 0; iCivic < GC.getNumCivicInfos(); ++iCivic)
			{
				if (GC.getCivicInfo((CivicTypes) iCivic).getCivicOptionType() == info.getData1())
				{
					eCivic = (CivicTypes) iCivic;
					break;
				}
			}

			pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN);
			pInfo->setText(L"pediaJumpToCivic");
			pInfo->setData1(eCivic);
			gDLL->getInterfaceIFace()->addPopup(pInfo, GC.getGameINLINE().getActivePlayer(), false, true);
		}
		else
		{
			gDLL->sendPlayerAction(GC.getGameINLINE().getActivePlayer(), PLAYER_ACTION_SET_CIVIC, info.getData1(), pPopupReturn->getButtonClicked(), -1);
		}
		break;

	case BUTTONPOPUP_TRADE_ROUTES:
		{
			CvPlayer& kPlayer = GET_PLAYER(GC.getGameINLINE().getActivePlayer());
			std::vector<CvTradeRoute*> aiRoutes;
			kPlayer.getTradeRoutes(aiRoutes);
			for (uint i = 0; i < aiRoutes.size(); ++i)
			{
				int iRouteID = aiRoutes[i]->getID();
				bool bAssignRoute = (pPopupReturn->getCheckboxBitfield(iRouteID) != 0);
				GC.getGameINLINE().selectionListGameNetMessage(GAMEMESSAGE_DO_COMMAND, COMMAND_ASSIGN_TRADE_ROUTE, iRouteID, bAssignRoute);
			}
		}
		break;

	case BUTTONPOPUP_YIELD_IMPORT_EXPORT:
		{
			// Note: has to match code in CvCity::handleAutoTraderouteSetup

			CvPlayer& kPlayer = GET_PLAYER(GC.getGameINLINE().getActivePlayer());
			CvCity* pCity = kPlayer.getCity(info.getData1());
			if (pCity != NULL)
			{
				for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
				{
					if (GC.getYieldInfo(eYield).isCargo())
					{
						YieldTypes eExportYield = eYield + NUM_YIELD_TYPES;
						// transport feeder - start - Nightinggale
						// R&R mod, vetiarvind, max yield import limit - start

						// if yield is to be skipped, skip it before looking up data for performance reasons
						if (info.getOption1()) //if we are in "condensed" mode, only consider produced and traded yields
						{
							if(!isYieldSupportedInExportPanelForCity(pCity, eYield))
								continue;
						}

						// load yield data and store in human readable variable names
						const bool bImport          = (pPopupReturn->getCheckboxBitfield(eYield)       & 0x01);
						const bool bExport          = (pPopupReturn->getCheckboxBitfield(eExportYield) & 0x01);
						const bool bMaintainImport  = (pPopupReturn->getCheckboxBitfield(eYield)       & 0x02);
						const bool bAutoExport      = (pPopupReturn->getCheckboxBitfield(eExportYield) & 0x02);
						const int iImportLimitLevel =  pPopupReturn->getSpinnerWidgetValue(eYield);
						const int iMaintainLevel    =  pPopupReturn->getSpinnerWidgetValue(eExportYield);

						// check if any data is different from the same data in the city
						if (bImport != pCity->isImport(eYield)
							|| bExport != pCity->isExport(eYield)
							|| bMaintainImport != pCity->getImportsMaintain(eYield)
							|| bAutoExport != pCity->isAutoExport(eYield) // auto traderoute - Nightinggale
							|| iMaintainLevel != pCity->getMaintainLevel(eYield)
							|| iImportLimitLevel != pCity->getImportsLimit(eYield) // R&R mod, vetiarvind, max yield import limit
							)
						{
							// a difference is detected. Send one network package containing everything.
							// It's not important what changed because the update is all or nothing.

							// first merge the ints values into a single int because the network package only has a single int32 available
							NetworkDataTradeRouteInts buffer;
							buffer.iImportLimitLevel = iImportLimitLevel;
							buffer.iMaintainLevel    = iMaintainLevel;

							gDLL->sendDoTask(info.getData1(), TASK_YIELD_TRADEROUTE, eYield, buffer.iNetwork, bImport, bExport, bMaintainImport, bAutoExport);
						}
						// R&R mod, vetiarvind, max yield import limit - end
						// transport feeder - end - Nightinggale
					}
				}
			}

			// auto traderoute - start - Nightinggale
		if (pPopupReturn->getButtonClicked() >= 0 && pPopupReturn->getButtonClicked() <= 2)
			{
				bool bReset          = pPopupReturn->getButtonClicked() == 0;
				bool bImportAll      = pPopupReturn->getButtonClicked() == 1;
				bool bAutoExportAll  = pPopupReturn->getButtonClicked() == 2;
				gDLL->sendDoTask(info.getData1(), TASK_AUTO_TRADEROUTE, 0, 0, bReset, bImportAll, bAutoExportAll, false);
			}
			// auto traderoute - end - Nightinggale
			
			//bobisback import changes
			if (pPopupReturn->getButtonClicked() == 3 || pPopupReturn->getButtonClicked() == 4)
			{
				bool importDemandedGoods = pPopupReturn->getButtonClicked() == 3;
				bool importConstructionGoods = pPopupReturn->getButtonClicked() == 4;

				gDLL->sendDoTask(info.getData1(), TASK_IMPORT_CHANGES, 0, 0, importDemandedGoods, importConstructionGoods, false, false);
			}
			
			if (pPopupReturn->getButtonClicked() == 5 || pPopupReturn->getButtonClicked() == 6)
			{
				bool importMilitaryGoods = pPopupReturn->getButtonClicked() == 5;
				bool importLivestockGoods = pPopupReturn->getButtonClicked() == 6;
				gDLL->sendDoTask(info.getData1(), TASK_IMPORT_CHANGES_GRP2, 0, 0, importMilitaryGoods, importLivestockGoods, false, false);
			}						
		}
		break;
		
		
	// Teacher List - start - Nightinggale
	case BUTTONPOPUP_TEACHER_LIST:
		{
			CvPlayer& kPlayer = GET_PLAYER(GC.getGameINLINE().getActivePlayer());
			CvCity* pCity = kPlayer.getCity(info.getData1());
			if (pCity != NULL)
			{
				if (pPopupReturn->getCheckboxBitfield(1) & 0x01)
				{
					// Reset checkbox is set
					for (int iUnit = 0; iUnit < GC.getNumUnitInfos(); iUnit++)
					{
						UnitTypes eUnit = (UnitTypes)iUnit;
						if (pCity->getOrderedStudents(eUnit) > 0 || pCity->getOrderedStudentsRepeat(eUnit))
						{
							gDLL->sendDoTask(info.getData1(), TASK_CHANGE_ORDERED_STUDENTS, 0, 0, false, false, true, false);
							break;
						}
					}
					break;
				} else {
					// Send doTasks though a pipeline like cache.
					// This way the last DoTask is in the cache when the loop is done.
					// This is because the last DoTask needs to be found as it needs bAlt to be set
					//  in order to call CvCity::checkOrderedStudentsForRepeats()
					UnitTypes eUnitCached = NO_UNIT;
					bool bRepeatCached = false;
					int iLevelCached = 0;
					for (int iUnit = 0; iUnit < GC.getNumUnitInfos(); iUnit++)
					{
						UnitTypes eUnit = (UnitTypes)iUnit;
						int iLevel = pPopupReturn->getSpinnerWidgetValue(iUnit);
						bool bRepeat = (pPopupReturn->getCheckboxBitfield(iUnit) & 0x01);

						if (iLevel >= 0 && ( iLevel != pCity->getOrderedStudents(eUnit) || bRepeat != pCity->getOrderedStudentsRepeat(eUnit)))
						{
							if (eUnitCached != NO_UNIT)
							{
								gDLL->sendDoTask(info.getData1(), TASK_CHANGE_ORDERED_STUDENTS, eUnitCached, iLevelCached, bRepeatCached, false, false, false);
							}
							eUnitCached = eUnit;
							iLevelCached = iLevel;
							bRepeatCached = bRepeat;
						}
					}
					if (eUnitCached != NO_UNIT)
					{
						gDLL->sendDoTask(info.getData1(), TASK_CHANGE_ORDERED_STUDENTS, eUnitCached, iLevelCached, bRepeatCached, true, false, false);
					}
				}
			}
		}
		break;
	// Teacher List - end - Nightinggale

	// R&R, Robert Surcouf, Custom House Popup-Screen START
	case BUTTONPOPUP_CUSTOM_HOUSE:
		{
			CvPlayer& kPlayer = GET_PLAYER(GC.getGameINLINE().getActivePlayer());
			CvCity* pCity = kPlayer.getCity(info.getData1());
			if (pCity != NULL)
			{
				for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
				{
					YieldTypes eYield = (YieldTypes) iYield;
					// ray, making special storage capacity rules for Yields XML configurable
					if (GC.getYieldInfo((YieldTypes)iYield).isCargo() && !GC.getYieldInfo((YieldTypes)iYield).isIgnoredForStorageCapacity())
					// if (GC.getYieldInfo((YieldTypes) iYield).isCargo() && (eYield != YIELD_FOOD) && (eYield != YIELD_LUMBER) && (eYield != YIELD_STONE))
					{
						bool bNeverSell = (pPopupReturn->getCheckboxBitfield(iYield) & 0x01);
						int iLevel = pPopupReturn->getSpinnerWidgetValue(iYield);

						if (bNeverSell != pCity->isCustomHouseNeverSell(eYield) || iLevel != pCity->getCustomHouseSellThreshold(eYield))
						{
							gDLL->sendDoTask(info.getData1(), TASK_CHANGE_CUSTOM_HOUSE_SETTINGS, iYield, iLevel, bNeverSell, false, false, false);
						}
					}
				}
			}
		}
		break;

	case BUTTONPOPUP_DOMESTIC_MARKET:
		{
		}
		break;
	// R&R, Robert Surcouf, Custom House Popup-Screen END

	case BUTTONPOPUP_PROMOTE:
		{
			CvPlayer& kPlayer = GET_PLAYER(GC.getGameINLINE().getActivePlayer());
			CvSelectionGroup* pSelectionGroup = gDLL->getInterfaceIFace()->getSelectionList();
			int iPromotion = pPopupReturn->getButtonClicked();
			if (iPromotion < GC.getNumPromotionInfos())
			{
				if (pSelectionGroup != NULL)
				{
					CLLNode<IDInfo>* pUnitNode = pSelectionGroup->headUnitNode();
					while (pUnitNode != NULL)
					{
						CvUnit* ploopUnit = ::getUnit(pUnitNode->m_data);
						pUnitNode = pSelectionGroup->nextUnitNode(pUnitNode);
						if (NULL != ploopUnit)
						{
							gDLL->sendDoCommand(ploopUnit->getID(), COMMAND_PROMOTION, iPromotion, -1, false);
							gDLL->sendDoCommand(ploopUnit->getID(), COMMAND_PROMOTE, -1, -1, false);
						}
					}
				}
			}
		}
		break;

	case BUTTONPOPUP_CHOOSE_GOODY:
		if (pPopupReturn->getButtonClicked() == 1)
		{
			gDLL->sendPlayerAction(GC.getGameINLINE().getActivePlayer(), PLAYER_ACTION_RECEIVE_GOODY, info.getData1(), info.getData2(), info.getData3());
		}
		break;

	case BUTTONPOPUP_SELECT_YIELD_AMOUNT:
		{
			CvUnit* pUnit = GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getUnit(info.getData2());
			if (pUnit != NULL)
			{
				// R&R, ray, fix for SHIFT issue in Africa and Port Royal
				if (pUnit->getUnitTravelState() == UNIT_TRAVEL_STATE_IN_EUROPE || pUnit->getUnitTravelState() == UNIT_TRAVEL_STATE_IN_AFRICA || pUnit->getUnitTravelState() == UNIT_TRAVEL_STATE_IN_PORT_ROYAL)
				//if (pUnit->getUnitTravelState() == UNIT_TRAVEL_STATE_IN_EUROPE)
				{
					gDLL->sendPlayerAction(GC.getGameINLINE().getActivePlayer(), (PlayerActionTypes)info.getData3(), info.getData1(), pPopupReturn->getCurrentSpinBoxValue(0), info.getData2());
				}
				else
				{
					gDLL->sendDoCommand(info.getData2(), (CommandTypes)info.getData3(), info.getData1(), pPopupReturn->getCurrentSpinBoxValue(0), false);
				}
			}
		}
		break;

	case BUTTONPOPUP_TALK_NATIVES:
		if (pPopupReturn->getButtonClicked() >= 0 && pPopupReturn->getButtonClicked() < NUM_COMMAND_TYPES)
		{
			gDLL->sendDoCommand(info.getData1(), (CommandTypes) pPopupReturn->getButtonClicked(), -1, -1, false);
		}
		else if (pPopupReturn->getButtonClicked() == NUM_COMMAND_TYPES)
		{
			gDLL->sendPlayerAction(GC.getGameINLINE().getActivePlayer(), PLAYER_ACTION_FEAT, FEAT_TALK_NATIVES, 1, -1);
		}
		break;

	case BUTTONPOPUP_ACHIEVEMENTS:
		{
			// just to avoid assert
		}
		break;

	// TAC - Goto Menu - koma13 - START
	case BUTTONPOPUP_GOTO_MENU:
		if (pPopupReturn->getButtonClicked() >= 0)
		{
			CvCity* pCity = GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getCity(pPopupReturn->getButtonClicked());
			if (pCity != NULL)
			{
				gDLL->sendPushMission(info.getData1(), MISSION_MOVE_TO, pCity->getX(), pCity->getY(), 0, false);
			}
		}
		else if (pPopupReturn->getButtonClicked() == -2)
		{
			CvUnit* pUnit = GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getUnit(info.getData1());
			if (pUnit != NULL)
			{
				if (pUnit->canCrossOcean(pUnit->plot(), UNIT_TRAVEL_STATE_TO_EUROPE))
				{
					gDLL->sendDoCommand(info.getData1(), COMMAND_SAIL_TO_EUROPE, UNIT_TRAVEL_STATE_TO_EUROPE, -1, false);
				}
				else
				{
					gDLL->sendDoCommand(info.getData1(), COMMAND_AUTOMATE, AUTOMATE_SAIL_TO_EUROPE, -1, false);
				}
			}
		}
		// R&R, vetiarvind, Goto other screens - START
		else if (pPopupReturn->getButtonClicked() == -3) //africa
		{
			CvUnit* pUnit = GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getUnit(info.getData1());
			if (pUnit != NULL)
			{
				if (pUnit->canCrossOcean(pUnit->plot(), UNIT_TRAVEL_STATE_TO_AFRICA))
				{
					gDLL->sendDoCommand(info.getData1(), COMMAND_SAIL_TO_AFRICA, UNIT_TRAVEL_STATE_TO_AFRICA, -1, false);
				}
				else
				{
					gDLL->sendDoCommand(info.getData1(), COMMAND_AUTOMATE, AUTOMATE_SAIL_TO_AFRICA, -1, false);
				}
			}
		}
		else if (pPopupReturn->getButtonClicked() == -4) //port royal
		{
			CvUnit* pUnit = GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getUnit(info.getData1());
			if (pUnit != NULL)
			{
				if (pUnit->canCrossOcean(pUnit->plot(), UNIT_TRAVEL_STATE_TO_PORT_ROYAL))
				{
					gDLL->sendDoCommand(info.getData1(), COMMAND_SAIL_TO_PORT_ROYAL, UNIT_TRAVEL_STATE_TO_PORT_ROYAL, -1, false);
				}
				else
				{
					gDLL->sendDoCommand(info.getData1(), COMMAND_AUTOMATE, AUTOMATE_SAIL_TO_PORT_ROYAL, -1, false);
				}
			}
		}
		// R&R, vetiarvind, Goto other screens - END

		break;
	// TAC - Goto Menu - koma13 - END

	case BUTTONPOPUP_CHOOSE_CITY_PLOT_YIELD:
		{
			const int iYield = pPopupReturn->getButtonClicked();
			if (iYield >= NO_YIELD) // cancel is -2
			{
				const CvCity* pCity = GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getCity(info.getData1());
				if (pCity != NULL && pCity->getPreferredYieldAtCityPlot() != iYield)
				{
					gDLL->sendDoTask(info.getData1(), TASK_CHOOSE_CITY_PLOT_YIELD, iYield, -1, false, false, false, false);
				}
			}
		}
		break;

	case BUTTONPOPUP_DESYNC_LOG_COMPLETE:
		break;

	case BUTTONPOPUP_START_GAME_CHECK_FAILED:
		PopupButtonContainer<BUTTONPOPUP_START_GAME_CHECK_FAILED>::clicked(pPopupReturn);
		break;
	case BUTTONPOPUP_NETWORK_OOS_MENU:
		PopupButtonContainer<BUTTONPOPUP_NETWORK_OOS_MENU>::clicked(pPopupReturn);
		break;

	case BUTTONPOPUP_NO_EVENT_ON_OK_CLICKED:
		// generic "do nothing"
		break;

	default:
		FAssert(false);
		break;
	}
}

void CvDLLButtonPopup::OnFocus(CvPopup* pPopup, CvPopupInfo &info)
{
	if (gDLL->getInterfaceIFace()->popupIsDying(pPopup))
	{
		return;
	}

	switch (info.getButtonPopupType())
	{
	case BUTTONPOPUP_CHOOSEPRODUCTION:
		{
			PlayerTypes ePlayer = GC.getGameINLINE().getActivePlayer();
			CvCity* pCity = GET_PLAYER(ePlayer).getCity(info.getData1());

			if (NULL == pCity || pCity->getOwnerINLINE() != ePlayer || pCity->isProduction())
			{
				gDLL->getInterfaceIFace()->popupSetAsCancelled(pPopup);
				break;
			}

			gDLL->getInterfaceIFace()->lookAtCityOffset(pCity->getID());
		}
		break;

	case BUTTONPOPUP_CHOOSE_EDUCATION:
		{
			PlayerTypes ePlayer = GC.getGameINLINE().getActivePlayer();
			CvCity* pCity = GET_PLAYER(ePlayer).getCity(info.getData1());
			if (NULL == pCity || pCity->getOwnerINLINE() != ePlayer)
			{
				gDLL->getInterfaceIFace()->popupSetAsCancelled(pPopup);
			}
			else
			{
				gDLL->getInterfaceIFace()->lookAtCityOffset(pCity->getID());
			}

		}
		break;

	case BUTTONPOPUP_RAZECITY:
		{
			PlayerTypes ePlayer = GC.getGameINLINE().getActivePlayer();
			CvCity* pCity = GET_PLAYER(ePlayer).getCity(info.getData1());

			if (NULL == pCity || pCity->getOwnerINLINE() != ePlayer)
			{
				gDLL->getInterfaceIFace()->popupSetAsCancelled(pPopup);
				break;
			}

			gDLL->getInterfaceIFace()->lookAtCityOffset(pCity->getID());
		}
		break;

	case BUTTONPOPUP_PYTHON:
	case BUTTONPOPUP_FEAT:
	case BUTTONPOPUP_PYTHON_SCREEN:
	case BUTTONPOPUP_MOVIE:
		if (!info.getOnFocusPythonCallback().IsEmpty())
		{
			long iResult;
			CyArgsList argsList;
			argsList.add(info.getData1());
			argsList.add(info.getData2());
			argsList.add(info.getData3());
			argsList.add(info.getFlags());
			argsList.add(info.getText());
			argsList.add(info.getOption1());
			argsList.add(info.getOption2());
			gDLL->getPythonIFace()->callFunction(PYScreensModule, info.getOnFocusPythonCallback(), argsList.makeFunctionArgs(), &iResult);
			if (0 != iResult)
			{
				gDLL->getInterfaceIFace()->popupSetAsCancelled(pPopup);
			}
		}
		break;

	case BUTTONPOPUP_FOUNDING_FATHER:
		if (!GET_TEAM(GC.getGameINLINE().getActiveTeam()).canConvinceFather((FatherTypes) info.getData1()))
		{
			gDLL->getInterfaceIFace()->popupSetAsCancelled(pPopup);
		}
		break;
	}
}

// returns false if popup is not launched
bool CvDLLButtonPopup::launchButtonPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	bool bLaunched = false;

	switch (info.getButtonPopupType())
	{
	case BUTTONPOPUP_TEXT:
		bLaunched = launchTextPopup(pPopup, info);
		break;
	case BUTTONPOPUP_CHOOSEPRODUCTION:
		bLaunched = launchProductionPopup(pPopup, info);
		break;
	case BUTTONPOPUP_CHOOSE_YIELD_BUILD:
		bLaunched = launchChooseYieldBuildPopup(pPopup, info);
		break;
	case BUTTONPOPUP_CHOOSE_EDUCATION:
		bLaunched = launchEducationPopup(pPopup, info);
		break;
	case BUTTONPOPUP_RAZECITY:
		bLaunched = launchRazeCityPopup(pPopup, info);
		break;
	case BUTTONPOPUP_ALARM:
		bLaunched = launchAlarmPopup(pPopup, info);
		break;
	case BUTTONPOPUP_DECLAREWARMOVE:
		bLaunched = launchDeclareWarMovePopup(pPopup, info);
		break;
	case BUTTONPOPUP_CONFIRMCOMMAND:
		bLaunched = launchConfirmCommandPopup(pPopup, info);
		break;
	case BUTTONPOPUP_CONFIRMTASK:
		bLaunched = launchConfirmTaskPopup(pPopup, info);
		break;
	case BUTTONPOPUP_LOADUNIT:
		bLaunched = launchLoadUnitPopup(pPopup, info);
		break;
	case BUTTONPOPUP_LOAD_CARGO:
		bLaunched = launchLoadCargoPopup(pPopup, info);
		break;
	case BUTTONPOPUP_LEADUNIT:
		bLaunched = launchLeadUnitPopup(pPopup, info);
		break;
	case BUTTONPOPUP_MAIN_MENU:
		bLaunched = launchMainMenuPopup(pPopup, info);
		break;
	case BUTTONPOPUP_CONFIRM_MENU:
		bLaunched = launchConfirmMenu(pPopup, info);
		break;
	case BUTTONPOPUP_PYTHON_SCREEN:
		bLaunched = launchPythonScreen(pPopup, info);
		break;
	case BUTTONPOPUP_MOVIE:
		bLaunched = launchMovie(pPopup, info);
		break;
	case BUTTONPOPUP_DEAL_CANCELED:
		bLaunched = launchCancelDeal(pPopup, info);
		break;
	case BUTTONPOPUP_PYTHON:
	case BUTTONPOPUP_FEAT:
		bLaunched = launchPythonPopup(pPopup, info);
		break;
	case BUTTONPOPUP_DETAILS:
		bLaunched = launchDetailsPopup(pPopup, info);
		break;
	case BUTTONPOPUP_ADMIN:
		bLaunched = launchAdminPopup(pPopup, info);
		break;
	case BUTTONPOPUP_ADMIN_PASSWORD:
		bLaunched = launchAdminPasswordPopup(pPopup, info);
		break;
	case BUTTONPOPUP_EXTENDED_GAME:
		bLaunched = launchExtendedGamePopup(pPopup, info);
		break;
	case BUTTONPOPUP_DIPLOMACY:
		bLaunched = launchDiplomacyPopup(pPopup, info);
		break;
	case BUTTONPOPUP_ADDBUDDY:
		bLaunched = launchAddBuddyPopup(pPopup, info);
		break;
	case BUTTONPOPUP_FORCED_DISCONNECT:
		bLaunched = launchForcedDisconnectPopup(pPopup, info);
		break;
	case BUTTONPOPUP_PITBOSS_DISCONNECT:
		bLaunched = launchPitbossDisconnectPopup(pPopup, info);
		break;
	case BUTTONPOPUP_KICKED:
		bLaunched = launchKickedPopup(pPopup, info);
		break;
	case BUTTONPOPUP_EVENT:
		bLaunched = launchEventPopup(pPopup, info);
		break;
	case BUTTONPOPUP_FREE_COLONY:
		bLaunched = launchFreeColonyPopup(pPopup, info);
		break;
	case BUTTONPOPUP_CHOOSE_PROFESSION:
		bLaunched = launchChooseProfessionPopup(pPopup, info);
		break;
	case BUTTONPOPUP_PURCHASE_EUROPE_UNIT:
		bLaunched = launchPurchaseEuropeUnitPopup(pPopup, info);
		break;
	/*** TRIANGLETRADE 10/24/08 by DPII ***/
	case BUTTONPOPUP_PURCHASE_AFRICA_UNIT:
		bLaunched = launchPurchaseAfricaUnitPopup(pPopup, info);
		break;
	/*************************************/
	// R&R, ray, Port Royal
	case BUTTONPOPUP_PURCHASE_PORT_ROYAL_UNIT:
		bLaunched = launchPurchasePortRoyalUnitPopup(pPopup, info);
		break;
	// R&R, ray, Port Royal - END
	//R&R mod, vetiarvind, trade groups - start
	case BUTTONPOPUP_SAVE_TRADEGROUP:
		bLaunched = launchSaveTradeGroupPopup(pPopup, info);
		break;
	//R&R mod, vetiarvind, trade groups - end

	case BUTTONPOPUP_FOUNDING_FATHER:
		bLaunched = launchFoundingFatherPopup(pPopup, info);
		break;
	case BUTTONPOPUP_CIVIC_OPTION:
		bLaunched = launchCivicOptionPopup(pPopup, info);
		break;
	case BUTTONPOPUP_TRADE_ROUTES:
		bLaunched = launchTradeRoutesPopup(pPopup, info);
		break;
	case BUTTONPOPUP_YIELD_IMPORT_EXPORT:
		bLaunched = launchYieldImportExportPopup(pPopup, info);
		break;
	// Teacher List - start - Nightinggale
	case BUTTONPOPUP_TEACHER_LIST:
		bLaunched = launchTeacherListPopup(pPopup, info);
		break;
	// Teacher List - end - Nightinggale
	// R&R, Robert Surcouf, Custom House Popup-Screen START
	case BUTTONPOPUP_CUSTOM_HOUSE:
		bLaunched = launchCustomHousePopup(pPopup, info);
		break;
	case BUTTONPOPUP_DOMESTIC_MARKET:
		bLaunched = launchDomesticMarketPopup(pPopup, info);
		break;
	// R&R, Robert Surcouf, Custom House Popup-Screen END
	case BUTTONPOPUP_PROMOTE:
		bLaunched = launchPromotionPopup(pPopup, info);
		break;
	case BUTTONPOPUP_CHOOSE_GOODY:
		bLaunched = launchChooseGoodyPopup(pPopup, info);
		break;
	case BUTTONPOPUP_SELECT_YIELD_AMOUNT:
		bLaunched = launchSelectYieldAmountPopup(pPopup, info);
		break;
	case BUTTONPOPUP_TALK_NATIVES:
		bLaunched = launchTalkNativesPopup(pPopup, info);
		break;
	// PatchMod: Achievements START
	case BUTTONPOPUP_ACHIEVEMENTS:
		bLaunched = launchAchievementPopup(pPopup, info);
		break;
	// PatchMod: Achievements END
	// TAC - Goto Menu - koma13 - START
	case BUTTONPOPUP_GOTO_MENU:
		bLaunched = launchGotoMenuPopup(pPopup, info);
		break;
	// TAC - Goto Menu - koma13 - END
	case BUTTONPOPUP_CHOOSE_CITY_PLOT_YIELD:
		bLaunched = launchChooseCityPlotYieldPopup(pPopup, info);
		break;
	case BUTTONPOPUP_DESYNC_LOG_COMPLETE:
		bLaunched = launchDesyncLogCompletePopup(pPopup, info);
		break;
	case BUTTONPOPUP_START_GAME_CHECK_FAILED:
		bLaunched = PopupButtonContainer<BUTTONPOPUP_START_GAME_CHECK_FAILED>::launch(pPopup, info);
		break;
	case BUTTONPOPUP_NETWORK_OOS_MENU:
		bLaunched = PopupButtonContainer<BUTTONPOPUP_NETWORK_OOS_MENU>::launch(pPopup, info);
		break;

	default:
		FAssert(false);
		break;
	}
	return (bLaunched);
}


bool CvDLLButtonPopup::launchTextPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, info.getText());
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, true, POPUPSTATE_IMMEDIATE);
	return (true);
}


bool CvDLLButtonPopup::launchProductionPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvWString szBuffer;
	CvString szArtFilename;

	CvCity* pCity = GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getCity(info.getData1());
	if (NULL == pCity || pCity->isProductionAutomated())
	{
		return (false);
	}

	CyCity* pyCity = new CyCity(pCity);
	CyArgsList argsList;
	argsList.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in plot class
	long lResult=0;
	gDLL->getPythonIFace()->callFunction(PYGameModule, "skipProductionPopup", argsList.makeFunctionArgs(), &lResult);
	delete pyCity;	// python fxn must not hold on to this pointer
	if (lResult == 1)
	{
		return (false);
	}

	FAssertMsg(pCity->getOwnerINLINE() == GC.getGameINLINE().getActivePlayer(), "City must belong to Active Player");

	UnitTypes eTrainUnit = NO_UNIT;
	BuildingTypes eConstructBuilding = NO_BUILDING;
	switch (info.getData2())
	{
	case (ORDER_TRAIN):
		eTrainUnit = (UnitTypes)info.getData3();
		break;
	case (ORDER_CONSTRUCT):
		eConstructBuilding = (BuildingTypes)info.getData3();
		break;
	default:
		break;
	}
	bool bFinish = info.getOption1();

	if (eTrainUnit != NO_UNIT)
	{
		if (bFinish)
		{
			szBuffer = gDLL->getText("TXT_KEY_POPUP_CONSTRUCTED_WORK_ON_NEXT", GC.getUnitInfo(eTrainUnit).getTextKeyWide(), pCity->getNameKey());
		}
		else
		{
			CvWString szRequires;
			for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
			{
				if (GC.getYieldInfo((YieldTypes) iYield).isCargo())
				{
					int iYieldNeeded = GET_PLAYER(pCity->getOwnerINLINE()).getYieldProductionNeeded(eTrainUnit, (YieldTypes) iYield);
					if (iYieldNeeded > pCity->getYieldStored((YieldTypes) iYield))
					{
 						if (!szRequires.empty())
						{
							szRequires += L", ";
						}
						szRequires += CvWString::format(L"%d %s", iYieldNeeded, GC.getYieldInfo((YieldTypes) iYield).getDescription());
					}
				}
			}
			if (!szRequires.empty())
			{
				szRequires = gDLL->getText("TXT_KEY_POPUP_REQUIRES", szRequires.GetCString());;
			}

			szBuffer = gDLL->getText("TXT_KEY_POPUP_CANNOT_TRAIN_WORK_NEXT", GC.getUnitInfo(eTrainUnit).getTextKeyWide(), pCity->getNameKey(), szRequires.GetCString());
		}
		szArtFilename = GET_PLAYER(pCity->getOwnerINLINE()).getUnitButton(eTrainUnit);
	}
	else if (eConstructBuilding != NO_BUILDING)
	{
		if (bFinish)
		{
			szBuffer = gDLL->getText("TXT_KEY_POPUP_CONSTRUCTED_WORK_ON_NEXT", GC.getBuildingInfo(eConstructBuilding).getTextKeyWide(), pCity->getNameKey());
		}
		else
		{
			CvWString szRequires;
			for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
			{
				if (GC.getYieldInfo((YieldTypes) iYield).isCargo())
				{
					int iYieldNeeded = GET_PLAYER(pCity->getOwnerINLINE()).getYieldProductionNeeded(eConstructBuilding, (YieldTypes) iYield);
					if (iYieldNeeded > pCity->getYieldStored((YieldTypes) iYield))
					{
						if (!szRequires.empty())
						{
							szRequires += L", ";
						}
						szRequires += CvWString::format(L"%d %s", iYieldNeeded, GC.getYieldInfo((YieldTypes) iYield).getDescription());
					}
				}
			}
			if (!szRequires.empty())
			{
				szRequires = gDLL->getText("TXT_KEY_POPUP_REQUIRES", szRequires.GetCString());;
			}

			szBuffer = gDLL->getText("TXT_KEY_POPUP_CANNOT_CONSTRUCT_WORK_NEXT", GC.getBuildingInfo(eConstructBuilding).getTextKeyWide(), pCity->getNameKey(), szRequires.GetCString());
		}
		szArtFilename = GC.getBuildingInfo(eConstructBuilding).getButton();
	}
	else
	{
		szBuffer = gDLL->getText("TXT_KEY_POPUP_WHAT_TO_BUILD", pCity->getNameKey());
		szArtFilename = ARTFILEMGR.getInterfaceArtInfo("INTERFACE_POPUPBUTTON_PRODUCTION")->getPath();
	}

	gDLL->getInterfaceIFace()->popupSetHeaderString(pPopup, szBuffer, DLL_FONT_LEFT_JUSTIFY);

	pyCity = new CyCity(pCity);
	CyArgsList argsList2;
	argsList2.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in plot class
	lResult=1;
	gDLL->getPythonIFace()->callFunction(PYGameModule, "showExamineCityButton", argsList2.makeFunctionArgs(), &lResult);
	delete pyCity;	// python fxn must not hold on to this pointer
	if (lResult == 1)
	{
		int iExamineCityID = 0;
		iExamineCityID = std::max(iExamineCityID, GC.getNumUnitInfos());
		iExamineCityID = std::max(iExamineCityID, GC.getNumBuildingInfos());

		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_EXAMINE_CITY").c_str(), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CITYSELECTION")->getPath(), iExamineCityID, WIDGET_GENERAL, -1, -1, true, POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY);
	}

	UnitTypes eProductionUnit = pCity->getProductionUnit();
	BuildingTypes eProductionBuilding = pCity->getProductionBuilding();

	int iNumBuilds = 0;

	pyCity = new CyCity(pCity);
	CyArgsList argsList3;
	argsList3.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
	lResult=-1;
	gDLL->getPythonIFace()->callFunction(PYGameModule, "getRecommendedUnit", argsList3.makeFunctionArgs(), &lResult);
	eProductionUnit = ((UnitTypes)lResult);
	CyArgsList argsList4; // XXX
	argsList4.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
	lResult=-1;
	gDLL->getPythonIFace()->callFunction(PYGameModule, "getRecommendedBuilding", argsList4.makeFunctionArgs(), &lResult);
	eProductionBuilding = ((BuildingTypes)lResult);
	delete pyCity;	// python fxn must not hold on to this pointer

	if (eProductionUnit == NO_UNIT)
	{
		eProductionUnit = pCity->AI_bestUnit(true);
	}

	if (eProductionBuilding == NO_BUILDING)
	{
		eProductionBuilding = pCity->AI_bestBuilding(0, 50, true);
	}

	if (eProductionUnit != NO_UNIT)
	{
		if ((eProductionUnit != eTrainUnit) || bFinish)
		{
			int iTurns = pCity->getProductionTurnsLeft(eProductionUnit, 0);
			szBuffer = gDLL->getText("TXT_KEY_POPUP_RECOMMENDED", GC.getUnitInfo(eProductionUnit).getTextKeyWide(), iTurns);
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szBuffer, GET_PLAYER(pCity->getOwnerINLINE()).getUnitButton(eProductionUnit), GC.getUnitInfo(eProductionUnit).getUnitClassType(), WIDGET_TRAIN, GC.getUnitInfo(eProductionUnit).getUnitClassType(), pCity->getID(), true, POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY );
			iNumBuilds++;
		}
	}

	if (eProductionBuilding != NO_BUILDING)
	{
		if (eProductionBuilding != eConstructBuilding)
		{
			int iTurns = pCity->getProductionTurnsLeft(eProductionBuilding, 0);
			szBuffer = gDLL->getText("TXT_KEY_POPUP_RECOMMENDED", GC.getBuildingInfo(eProductionBuilding).getTextKeyWide(), iTurns);
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szBuffer, GC.getBuildingInfo(eProductionBuilding).getButton(), GC.getBuildingInfo(eProductionBuilding).getBuildingClassType(), WIDGET_CONSTRUCT, GC.getBuildingInfo(eProductionBuilding).getBuildingClassType(), pCity->getID(), true, POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY );
			iNumBuilds++;
		}
	}

	for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		UnitTypes eLoopUnit = (UnitTypes)GC.getCivilizationInfo(pCity->getCivilizationType()).getCivilizationUnits(iI);

		if (eLoopUnit != NO_UNIT)
		{
			if (eLoopUnit != eProductionUnit)
			{
				if (pCity->canTrain(eLoopUnit))
				{
					if ((eLoopUnit != eTrainUnit) || bFinish)
					{
						int iTurns = pCity->getProductionTurnsLeft(eLoopUnit, 0);
						szBuffer.Format(L"%s (%d)", GC.getUnitInfo(eLoopUnit).getDescription(), iTurns);
						gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szBuffer, GET_PLAYER(pCity->getOwnerINLINE()).getUnitButton(eLoopUnit), iI, WIDGET_TRAIN, iI, pCity->getID(), true, POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY );
						iNumBuilds++;
					}
				}
			}
		}
	}
	for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		BuildingTypes eLoopBuilding = ((BuildingTypes)(GC.getCivilizationInfo(pCity->getCivilizationType()).getCivilizationBuildings(iI)));

		if (eLoopBuilding != NO_BUILDING)
		{
			if (eLoopBuilding != eProductionBuilding)
			{
				if (pCity->canConstruct(eLoopBuilding))
				{
					if (eLoopBuilding != eConstructBuilding)
					{
						int iTurns = pCity->getProductionTurnsLeft(eLoopBuilding, 0);
						szBuffer.Format(L"%s (%d)", GC.getBuildingInfo(eLoopBuilding).getDescription(), iTurns);
						gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szBuffer, GC.getBuildingInfo(eLoopBuilding).getButton(), iI, WIDGET_CONSTRUCT, iI, pCity->getID(), true, POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY);
						iNumBuilds++;
					}
				}
			}
		}
	}
	for (int iI = 0; iI < GC.getNumFatherPointInfos(); iI++)
	{
		FatherPointTypes ePointType = (FatherPointTypes) iI;
		if (pCity->canConvince(ePointType))
		{
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, GC.getFatherPointInfo(ePointType).getDescription(), GC.getFatherPointInfo(ePointType).getButton(), iI, WIDGET_CONVINCE, iI, pCity->getID(), true, POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY);
			iNumBuilds++;
		}
	}

	if (0 == iNumBuilds)
	{
		// city cannot build anything, so don't show popup after all
		return (false);
	}

	CvWStringBuffer szExtraBuilds;
	bool bFirst = true;
	for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		UnitTypes eLoopUnit = (UnitTypes)GC.getCivilizationInfo(pCity->getCivilizationType()).getCivilizationUnits(iI);
		if (eLoopUnit != NO_UNIT)
		{
			if (pCity->canTrain(eLoopUnit, false, true) && !pCity->canTrain(eLoopUnit))
			{
				if (bFirst)
				{
					bFirst = false;
				}
				else
				{
					szExtraBuilds.append(L", ");
				}
				szExtraBuilds.append(GC.getUnitInfo(eLoopUnit).getDescription());

				for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
				{
					YieldTypes eYield = (YieldTypes) iYield;
					if (GC.getYieldInfo(eYield).isCargo())
					{
						int iNeeded = pCity->getYieldProductionNeeded(eLoopUnit, eYield) - pCity->getYieldStored(eYield);
						if (iNeeded > 0)
						{
							szExtraBuilds.append(CvWString::format(L" (%d%c)", iNeeded, GC.getYieldInfo(eYield).getChar()));
						}
					}
				}
			}
		}
	}
	for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		BuildingTypes eLoopBuilding = ((BuildingTypes)(GC.getCivilizationInfo(pCity->getCivilizationType()).getCivilizationBuildings(iI)));
		if (eLoopBuilding != NO_BUILDING)
		{
			if (pCity->canConstruct(eLoopBuilding, false, true) && !pCity->canConstruct(eLoopBuilding))
			{
				if (bFirst)
				{
					bFirst = false;
				}
				else
				{
					szExtraBuilds.append(L", ");
				}
				szExtraBuilds.append(GC.getBuildingInfo(eLoopBuilding).getDescription());
				for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
				{
					YieldTypes eYield = (YieldTypes) iYield;
					if (GC.getYieldInfo(eYield).isCargo())
					{
						int iNeeded = pCity->getYieldProductionNeeded(eLoopBuilding, eYield) - pCity->getYieldStored(eYield);
						if (iNeeded > 0)
						{
							szExtraBuilds.append(CvWString::format(L" (%d%c)", iNeeded, GC.getYieldInfo(eYield).getChar()));
						}
					}
				}
			}
		}
	}

	if (!szExtraBuilds.isEmpty())
	{
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_POPUP_CANNOT_BUILD") + szExtraBuilds.getCString(), DLL_FONT_LEFT_JUSTIFY);
	}

	gDLL->getInterfaceIFace()->popupSetPopupType(pPopup, POPUPEVENT_PRODUCTION, szArtFilename);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_MINIMIZED, 252);

	switch (info.getData2())
	{
	case ORDER_TRAIN:
		//Androrc UnitArtStyles
//		gDLL->getInterfaceIFace()->playGeneralSound(GC.getUnitInfo((UnitTypes)info.getData3()).getArtInfo(0, NO_PROFESSION)->getTrainSound());
		gDLL->getInterfaceIFace()->playGeneralSound(GC.getUnitInfo((UnitTypes)info.getData3()).getUnitArtStylesArtInfo(0, NO_PROFESSION, (UnitArtStyleTypes) GC.getCivilizationInfo(GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getCivilizationType()).getUnitArtStyleType())->getTrainSound());
		//Androrc End
		break;

	case ORDER_CONSTRUCT:
		gDLL->getInterfaceIFace()->playGeneralSound(GC.getBuildingInfo((BuildingTypes)info.getData3()).getConstructSound());
		break;

	default:
		break;
	}

	return (true);
}

bool CvDLLButtonPopup::launchChooseYieldBuildPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvCity* pCity = GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getCity(info.getData1());
	if (NULL == pCity || pCity->isProductionAutomated())
	{
		return false;
	}
	// Ramstormp, Wtp, Add Examine Settlement option to the 'no longer lacking yields for building production' popup - START
	CyCity* pyCity = new CyCity(pCity);
	CyArgsList argsList;
	argsList.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in plot class
	long lResult = 0;
	gDLL->getPythonIFace()->callFunction(PYGameModule, "skipProductionPopup", argsList.makeFunctionArgs(), &lResult);
	delete pyCity;	// python fxn must not hold on to this pointer
	if (lResult == 1)
	{
		return (false);
	}
	// Ramstormp - END
	FAssertMsg(pCity->getOwnerINLINE() == GC.getGameINLINE().getActivePlayer(), "City must belong to Active Player");

	YieldTypes eYield = (YieldTypes) info.getData2();
	if (eYield == NO_YIELD)
	{
		return false;
	}

	CvWString szBuffer = gDLL->getText("TXT_KEY_POPUP_CHOOSE_COMLPETED_BUILD", GC.getYieldInfo(eYield).getTextKeyWide(), pCity->getNameKey());
	gDLL->getInterfaceIFace()->popupSetHeaderString(pPopup, szBuffer, DLL_FONT_LEFT_JUSTIFY);

	// Ramstormp, Wtp, Add Examine Settlement option to the no longer lacking yields for building production popup - START
	pyCity = new CyCity(pCity);
	CyArgsList argsList2;
	argsList2.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in plot class
	lResult = 1;
	gDLL->getPythonIFace()->callFunction(PYGameModule, "showExamineCityButton", argsList2.makeFunctionArgs(), &lResult);
	delete pyCity;	// python fxn must not hold on to this pointer
	if (lResult == 1)
	{
		int iExamineCityID = 0;
		iExamineCityID = std::max(iExamineCityID, GC.getNumUnitInfos());
		iExamineCityID = std::max(iExamineCityID, GC.getNumBuildingInfos());

		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_EXAMINE_CITY").c_str(), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CITYSELECTION")->getPath(), iExamineCityID, WIDGET_GENERAL, -1, -1, true, POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY);
	}
	// Ramstormp - END

	std::vector< std::pair<OrderTypes, int> > aOrders;
	pCity->getOrdersWaitingForYield(aOrders, eYield, true, pCity->getYieldStored(eYield) + pCity->getYieldRushed(eYield));

	if (aOrders.empty())
	{
		return false;
	}

	for (uint i = 0; i < aOrders.size(); ++i)
	{
		if (aOrders[i].first == ORDER_TRAIN)
		{
			UnitTypes eUnit = (UnitTypes) aOrders[i].second;
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, GC.getUnitInfo(eUnit).getDescription(), GET_PLAYER(pCity->getOwnerINLINE()).getUnitButton(eUnit), eUnit, WIDGET_GENERAL, -1, -1, true, POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY );
		}
		else if (aOrders[i].first == ORDER_CONSTRUCT)
		{
			BuildingTypes eBuilding = (BuildingTypes) aOrders[i].second;
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, GC.getBuildingInfo(eBuilding).getDescription(), GC.getBuildingInfo(eBuilding).getButton(), GC.getNumUnitInfos() + eBuilding, WIDGET_GENERAL, -1, -1, true, POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY );
		}
	}

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_WB_CITY_NOTHING"), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL")->getPath(), -1, WIDGET_GENERAL);

	gDLL->getInterfaceIFace()->popupSetPopupType(pPopup, POPUPEVENT_PRODUCTION, ARTFILEMGR.getInterfaceArtInfo("INTERFACE_POPUPBUTTON_PRODUCTION")->getPath());
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_MINIMIZED, 252);

	return (true);
}

bool CvDLLButtonPopup::launchEducationPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvPlayer& kPlayer = GET_PLAYER(GC.getGameINLINE().getActivePlayer());
	CvCity* pCity = kPlayer.getCity(info.getData1());
	if (NULL == pCity)
	{
		return false;
	}
	FAssertMsg(pCity->getOwnerINLINE() == GC.getGameINLINE().getActivePlayer(), "City must belong to Active Player");

	CvUnit* pUnit = pCity->getPopulationUnitById(info.getData2());
	if (pUnit == NULL)
	{
		return false;
	}

	CvWString szText = gDLL->getText("TXT_KEY_CHOOSE_SPECIALTY", pUnit->getNameOrProfessionKey(), pCity->getNameKey());
	gDLL->getInterfaceIFace()->popupSetHeaderString(pPopup, szText, DLL_FONT_LEFT_JUSTIFY);

	int iNumUnits = 0;
	UnitTypes eLastUnit = NO_UNIT;
	for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		CvUnitInfo& kUnit = GC.getUnitInfo((UnitTypes) iI);
		int iPrice = pCity->getSpecialistTuition((UnitTypes) iI);
		if (iPrice >= 0 && iPrice <= kPlayer.getGold())
		{
			szText.Format(L"%s", kUnit.getDescription());
			if (iPrice > 0)
			{
				szText += CvWString::format(L" (%d%c)", iPrice, GC.getSymbolID(GOLD_CHAR));
			}
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szText, kUnit.getButton(), iI, WIDGET_GENERAL, -1, -1, true, POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY);
			++iNumUnits;
			eLastUnit = (UnitTypes) iI;
		}
	}

	if (iNumUnits <= 1)
	{
		if (iNumUnits == 1)
		{
			gDLL->sendDoTask(info.getData1(), TASK_EDUCATE, info.getData2(), eLastUnit, false, false, false, false);
		}
		return false;
	}

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_EXAMINE_CITY"), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CITYSELECTION")->getPath(), GC.getNumUnitInfos(), WIDGET_GENERAL, -1, -1);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_MINIMIZED, 252);

	return true;
}

bool CvDLLButtonPopup::launchRazeCityPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvPlayer& player = GET_PLAYER(GC.getGameINLINE().getActivePlayer());

	CvCity* pNewCity = player.getCity(info.getData1());
	if (NULL == pNewCity)
	{
		FAssert(false);
		return (false);
	}

	if (0 != GC.getDefineINT("PLAYER_ALWAYS_RAZES_CITIES"))
	{
		player.raze(pNewCity);
		return false;
	}

	PlayerTypes eHighestCulturePlayer = (PlayerTypes)info.getData2();

	int iCaptureGold = info.getData3();
	bool bRaze = player.canRaze(pNewCity);
	bool bGift = ((eHighestCulturePlayer != NO_PLAYER)
		&& (eHighestCulturePlayer != player.getID())
		&& ((player.getTeam() == GET_PLAYER(eHighestCulturePlayer).getTeam()) || GET_TEAM(player.getTeam()).isOpenBorders(GET_PLAYER(eHighestCulturePlayer).getTeam())));

	CvWString szBuffer;
	if (iCaptureGold > 0)
	{
		szBuffer = gDLL->getText("TXT_KEY_POPUP_GOLD_CITY_CAPTURE", iCaptureGold, pNewCity->getNameKey());
	}
	else
	{
		szBuffer = gDLL->getText("TXT_KEY_POPUP_CITY_CAPTURE_KEEP", pNewCity->getNameKey());
	}
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, szBuffer);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_KEEP_CAPTURED_CITY").c_str(), NULL, 0, WIDGET_GENERAL);

	if (bRaze)
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_RAZE_CAPTURED_CITY").c_str(), NULL, 1, WIDGET_GENERAL);
	}
	if (bGift)
	{
		szBuffer = gDLL->getText("TXT_KEY_POPUP_RETURN_ALLIED_CITY", GET_PLAYER(eHighestCulturePlayer).getCivilizationDescriptionKey());
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szBuffer, NULL, 2, WIDGET_GENERAL, 2, eHighestCulturePlayer);
	}
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	gDLL->getInterfaceIFace()->playGeneralSound("AS2D_CITYCAPTURE");

	return (true);
}

bool CvDLLButtonPopup::launchAlarmPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	gDLL->getInterfaceIFace()->playGeneralSound("AS2D_ALARM");

	gDLL->getInterfaceIFace()->popupSetHeaderString(pPopup, gDLL->getText("TXT_KEY_POPUP_ALARM_TITLE").c_str());
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, info.getText());
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, true, POPUPSTATE_IMMEDIATE);

	return (true);
}


bool CvDLLButtonPopup::launchDeclareWarMovePopup(CvPopup* pPopup, CvPopupInfo &info)
{
	TeamTypes eRivalTeam = (TeamTypes)info.getData1();
	Coordinates coord(info.getData2(), info.getData3());

	FAssert(eRivalTeam != NO_TEAM);

	CvPlot* pPlot = coord.plot();
	bool bOpenBorders = info.getOption2();

	CvWString szBuffer;
	if ((pPlot != NULL) && pPlot->isOwned() && !bOpenBorders)
	{
		szBuffer = gDLL->getText("TXT_KEY_POPUP_ENTER_LANDS_WAR", GET_PLAYER(pPlot->getOwnerINLINE()).getCivilizationAdjective());

		if (GET_TEAM(GC.getGameINLINE().getActiveTeam()).isOpenBordersTrading())
		{
			szBuffer += gDLL->getText("TXT_KEY_POPUP_ENTER_WITH_OPEN_BORDERS");
		}
	}
	else
	{
		szBuffer = gDLL->getText("TXT_KEY_POPUP_DOES_THIS_MEAN_WAR", GET_TEAM(eRivalTeam).getName().GetCString());
	}
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, szBuffer);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_DECLARE_WAR_YES").c_str(), NULL, 0);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_DECLARE_WAR_NO").c_str());
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return (true);
}


bool CvDLLButtonPopup::launchConfirmCommandPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	int iAction = info.getData1();
	CvWString szBuffer;
	szBuffer = gDLL->getText("TXT_KEY_POPUP_ARE_YOU_SURE_ACTION", GC.getActionInfo(iAction).getTextKeyWide());
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, szBuffer);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_YES").c_str(), NULL, 0);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_NO").c_str());
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return (true);
}

bool CvDLLButtonPopup::launchConfirmTaskPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	int iAction = info.getData1();
	CvWString szBuffer;
	szBuffer = gDLL->getText("TXT_KEY_POPUP_ARE_YOU_SURE_ACTION", info.getText().GetCString());
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, szBuffer);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_YES").c_str(), NULL, 0);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_NO").c_str());
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return (true);
}


bool CvDLLButtonPopup::launchLoadUnitPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CLLNode<IDInfo>* pUnitNode;
	CvSelectionGroup* pSelectionGroup;
	CvUnit* pLoopUnit;
	CvPlot* pPlot;
	CvWStringBuffer szBuffer;
	int iCount;
	CvUnit* pFirstUnit = NULL;

	pSelectionGroup = gDLL->getInterfaceIFace()->getSelectionList();

	if (NULL == pSelectionGroup)
	{
		return (false);
	}

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_CHOOSE_TRANSPORT"));

	pPlot = pSelectionGroup->plot();
	if (NULL == pPlot)
	{
		return (false);
	}

	iCount = 1;

	pUnitNode = pPlot->headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if (pLoopUnit != NULL && pSelectionGroup->canDoCommand(COMMAND_LOAD_UNIT, pLoopUnit->getOwnerINLINE(), pLoopUnit->getID()))
		{
			if (!pFirstUnit)
			{
				pFirstUnit = pLoopUnit;
			}
			szBuffer.clear();
			GAMETEXT.setUnitHelp(szBuffer, pLoopUnit, true);
			szBuffer.append(L", ");
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_HELP_CARGO_SPACE", pLoopUnit->getCargo(), pLoopUnit->cargoSpace()));
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, CvWString(szBuffer.getCString()), NULL, iCount, WIDGET_GENERAL);
			iCount++;
		}
	}

	if (iCount <= 2)
	{
		if (pFirstUnit)
		{
			GC.getGameINLINE().selectionListGameNetMessage(GAMEMESSAGE_DO_COMMAND, COMMAND_LOAD_UNIT, pFirstUnit->getOwnerINLINE(), pFirstUnit->getID());
		}
		return (false);
	}

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"), NULL, 0, WIDGET_GENERAL);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return (true);
}


bool CvDLLButtonPopup::launchLoadCargoPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvWStringBuffer szBuffer;
	CvUnit* pFirstUnit = NULL;

	CvSelectionGroup* pSelectionGroup = gDLL->getInterfaceIFace()->getSelectionList();
	if (NULL == pSelectionGroup)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_CHOOSE_GOODS"));

	CvPlot* pPlot = pSelectionGroup->plot();
	if (NULL == pPlot)
	{
		return false;
	}

	CvCity* pCity = pPlot->getPlotCity();
	if (NULL == pCity)
	{
		return false;
	}

	int iNumYields = 0;
	for (int iYield = 0; iYield <  NUM_YIELD_TYPES; ++iYield)
	{
		YieldTypes eYield = (YieldTypes) iYield;
		CvYieldInfo& kYield = GC.getYieldInfo(eYield);
		if (kYield.isCargo())
		{
			int iYieldStored = pCity->getYieldStored(eYield);
			if (iYieldStored > 0)
			{
				iYieldStored = std::min(iYieldStored, GC.getGameINLINE().getCargoYieldCapacity());
				UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getCivilizationType()).getCivilizationUnits(kYield.getUnitClass());
				if (NO_UNIT != eUnit)
				{
					int iYieldAmountAvailable = 0;
					for (int i = 0; i < pSelectionGroup->getNumUnits(); ++i)
					{
						CvUnit* pLoopUnit = pSelectionGroup->getUnitAt(i);
						if (pLoopUnit != NULL)
						{
							iYieldAmountAvailable = pLoopUnit->getLoadYieldAmount(eYield);
							if (iYieldAmountAvailable > 0)
							{
								break;
							}
						}
					}

					if (iYieldAmountAvailable > 0)
					{
						++iNumYields;
						CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);
						CvWString szBuffer = gDLL->getText("TXT_KEY_POPUP_LOAD_YIELD", kUnit.getTextKeyWide(), std::min(iYieldStored, iYieldAmountAvailable), pCity->getYieldStored(eYield), kYield.getChar());
						gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szBuffer.GetCString(), kUnit.getButton(), iYield, WIDGET_HELP_YIELD, iYield, pCity->getID());
					}
				}
			}
		}
	}

	if (iNumYields == 0)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL")->getPath(), NUM_YIELD_TYPES, WIDGET_GENERAL);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}


bool CvDLLButtonPopup::launchLeadUnitPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvUnit* pLeaderUnit = GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getUnit(info.getData2());
	if (pLeaderUnit == NULL)
	{
		return false;
	}

	CvSelectionGroup* pSelectionGroup = pLeaderUnit->getGroup();
	if (NULL == pSelectionGroup)
	{
		return false;
	}

	CvPlot* pPlot = pSelectionGroup->plot();
	if (pPlot == NULL)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_CHOOSE_UNIT_TO_LEAD"));

	int iCount = 1;
	CvUnit* pFirstUnit = NULL;
	CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		// WTP, fixing Generals and Admirals to lead civilists or small tiny fishing boats - START
		if (pLoopUnit != NULL && ((pLoopUnit->getDomainType() == DOMAIN_LAND && pLoopUnit->canAttack()) || (pLoopUnit->getDomainType() == DOMAIN_SEA && pLoopUnit->baseCombatStr() >= 20)))
		{
			if (pLoopUnit->canPromote((PromotionTypes) info.getData1(), info.getData2()))
			{
				if (!pFirstUnit)
				{
					pFirstUnit = pLoopUnit;
				}
				CvWStringBuffer szBuffer;
				GAMETEXT.setUnitHelp(szBuffer, pLoopUnit, true);
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szBuffer.getCString(), NULL, iCount, WIDGET_GENERAL);
				iCount++;
			}
		}
	}

	if (iCount <= 2)
	{
		if (pFirstUnit)
		{
			gDLL->sendPushMission(info.getData2(), MISSION_LEAD, pFirstUnit->getID(), -1, 0, false);
		}
		return false;
	}

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"), NULL, 0, WIDGET_GENERAL);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return (true);
}

bool CvDLLButtonPopup::launchMainMenuPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	gDLL->getInterfaceIFace()->popupSetStyle( pPopup, "Window_NoTitleBar_Style" );

	// 288,72
	gDLL->getInterfaceIFace()->popupAddDDS(pPopup, "Resource/Temp/civ4_title_small.dds", 192, 48);

	gDLL->getInterfaceIFace()->popupAddSeparator(pPopup);

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_EXIT_TO_DESKTOP").c_str(), NULL, PopupButtonsMainMenu_ExitDesktop, WIDGET_GENERAL, PopupButtonsMainMenu_ExitDesktop, 0, true, POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);

	// commenting out since you can't exit to main menu and then restart a game
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_EXIT_TO_MAIN_MENU").c_str(), NULL, PopupButtonsMainMenu_ExitMenu, WIDGET_GENERAL, PopupButtonsMainMenu_ExitMenu, 0, true, POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);

	if (GC.getGameINLINE().canDoControl(CONTROL_RETIRE))
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_RETIRE").c_str(), NULL, PopupButtonsMainMenu_Retire, WIDGET_GENERAL, PopupButtonsMainMenu_Retire, 0, true, POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
	}

	if ((GC.getGameINLINE().getElapsedGameTurns() == 0) && !(GC.getGameINLINE().isGameMultiPlayer()) && !(GC.getInitCore().getWBMapScript()))
	{
		// Don't allow if there has already been diplomacy
		bool bShow = true;
		for (int i = 0; bShow && i < MAX_TEAMS; i++)
		{
			for (int j = i+1; bShow && j < MAX_TEAMS; j++)
			{
				if (GET_TEAM((TeamTypes)i).isHasMet((TeamTypes)j))
				{
					bShow = false;
				}
			}
		}

		if (bShow)
		{
			if (!GC.getGameINLINE().getScriptData().empty())
			{
				bShow = false;
			}
		}

		if (bShow)
		{
			for (int i = 0; i < GC.getMap().numPlots(); ++i)
			{
				CvPlot* pPlot = GC.getMap().plotByIndexINLINE(i);
				if (!pPlot->getScriptData().empty())
				{
					bShow = false;
					break;
				}
			}
		}

		if (bShow)
		{
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_REGENERATE_MAP").c_str(), NULL, PopupButtonsMainMenu_RegenerateMap, WIDGET_GENERAL, PopupButtonsMainMenu_RegenerateMap, 0, true, POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
		}
	}

	if (GC.getGameINLINE().canDoControl(CONTROL_LOAD_GAME))
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_LOAD_GAME").c_str(), NULL, PopupButtonsMainMenu_Load, WIDGET_GENERAL, PopupButtonsMainMenu_Load, 0, true, POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
	}
	if (GC.getGameINLINE().canDoControl(CONTROL_SAVE_NORMAL))
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_SAVE_GAME").c_str(), NULL, PopupButtonsMainMenu_Save, WIDGET_GENERAL, PopupButtonsMainMenu_Save, 0, true, POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
	}
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_OPTIONS").c_str(), NULL, PopupButtonsMainMenu_Options, WIDGET_GENERAL, PopupButtonsMainMenu_Options, 0, true, POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);

	if (GC.getGameINLINE().canDoControl(CONTROL_WORLD_BUILDER))
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_ENTER_WB").c_str(), NULL, PopupButtonsMainMenu_WorldBuilder, WIDGET_GENERAL, PopupButtonsMainMenu_WorldBuilder, 0, true, POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
	}

	if (GC.getGameINLINE().canDoControl(CONTROL_ADMIN_DETAILS))
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_GAME_DETAILS").c_str(), NULL, PopupButtonsMainMenu_GameDetails, WIDGET_GENERAL, PopupButtonsMainMenu_GameDetails, 0, true, POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
	}

	if (GC.getGameINLINE().canDoControl(CONTROL_DETAILS))
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_DETAILS_TITLE").c_str(), NULL, PopupButtonsMainMenu_DetailsTitle, WIDGET_GENERAL, PopupButtonsMainMenu_DetailsTitle, 0, true, POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
	}

	if (gDLL->isOOSVisible())
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_DEBUG_DESYNC_LOG_BUTTON").c_str(), NULL, PopupButtonsMainMenu_OOS, WIDGET_GENERAL, PopupButtonsMainMenu_OOS, 0, true, POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
	}

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_CANCEL").c_str(), NULL, PopupButtonsMainMenu_Cancel, WIDGET_GENERAL, PopupButtonsMainMenu_Cancel, 0, true, POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);



	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return (true);
}

bool CvDLLButtonPopup::launchConfirmMenu(CvPopup *pPopup, CvPopupInfo &info)
{
	if(info.getData1() == 2) //retire
	{
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_POPUP_ARE_YOU_SURE_ACTION", GC.getControlInfo(CONTROL_RETIRE).getTextKeyWide()).c_str());
	}
	else
	{
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_POPUP_ARE_YOU_SURE").c_str());
	}

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_YES").c_str(), NULL, 0, WIDGET_GENERAL);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_NO").c_str(), NULL, 1, WIDGET_GENERAL);
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchPythonScreen(CvPopup* pPopup, CvPopupInfo &info)
{
	// this is not really a popup, but a Python screen
	// we trick the app into thinking that it's a popup so that we can take advantage of the popup queuing system

	CyArgsList argsList;
	argsList.add(info.getData1());
	argsList.add(info.getData2());
	argsList.add(info.getData3());
	argsList.add(info.getOption1());
	argsList.add(info.getOption2());
	gDLL->getPythonIFace()->callFunction(PYScreensModule, CvString(info.getText()).GetCString(), argsList.makeFunctionArgs());

	return (false); // return false, so the Popup object is deleted, since it's just a dummy
}

bool CvDLLButtonPopup::launchMovie(CvPopup* pPopup, CvPopupInfo &info)
{
	// this is not really a popup, but a Python screen
	// we trick the app into thinking that it's a popup so that we can take advantage of the popup queuing system

	CyArgsList argsList;
	argsList.add(CvString(info.getText()));
	gDLL->getPythonIFace()->callFunction(PYScreensModule, "showVictoryMovie", argsList.makeFunctionArgs());

	return false; // return false, so the Popup object is deleted, since it's just a dummy
}

bool CvDLLButtonPopup::launchCancelDeal(CvPopup* pPopup, CvPopupInfo &info)
{
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup,  gDLL->getText("TXT_KEY_POPUP_CANCEL_DEAL") );

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_CANCEL_DEAL_YES"), NULL, 0, WIDGET_GENERAL);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_CANCEL_DEAL_NO"), NULL, 1, WIDGET_GENERAL);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchPythonPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, info.getText());
	for (int i = 0; i < info.getNumPythonButtons(); i++)
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, info.getPythonButtonText(i), info.getPythonButtonArt(i).IsEmpty() ? NULL : info.getPythonButtonArt(i).GetCString(), i);
	}

	gDLL->getInterfaceIFace()->popupSetPopupType(pPopup, POPUPEVENT_WARNING, ARTFILEMGR.getInterfaceArtInfo("INTERFACE_POPUPBUTTON_WARNING")->getPath());
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return (true);
}

bool CvDLLButtonPopup::launchDetailsPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	if (!info.getOption1())
	{
		gDLL->getInterfaceIFace()->popupSetHeaderString(pPopup, gDLL->getText("TXT_KEY_POPUP_DETAILS_TITLE"));

		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_MENU_LEADER_NAME"));
		gDLL->getInterfaceIFace()->popupCreateEditBox(pPopup, GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getName(), WIDGET_GENERAL, gDLL->getText("TXT_KEY_MENU_LEADER_NAME"), 0, POPUP_LAYOUT_STRETCH, 0, MAX_PLAYERINFO_CHAR_COUNT);
		gDLL->getInterfaceIFace()->popupAddSeparator(pPopup);
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_MENU_CIV_DESC"));
		gDLL->getInterfaceIFace()->popupCreateEditBox(pPopup, GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getCivilizationDescription(), WIDGET_GENERAL, gDLL->getText("TXT_KEY_MENU_CIV_DESC"), 1, POPUP_LAYOUT_STRETCH, 0, MAX_PLAYERINFO_CHAR_COUNT);
		gDLL->getInterfaceIFace()->popupAddSeparator(pPopup);
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_MENU_CIV_SHORT_DESC"));
		gDLL->getInterfaceIFace()->popupCreateEditBox(pPopup, GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getCivilizationShortDescription(), WIDGET_GENERAL, gDLL->getText("TXT_KEY_MENU_CIV_SHORT_DESC"), 2, POPUP_LAYOUT_STRETCH, 0, MAX_PLAYERINFO_CHAR_COUNT);
		gDLL->getInterfaceIFace()->popupAddSeparator(pPopup);
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_MENU_CIV_ADJ"));
		gDLL->getInterfaceIFace()->popupCreateEditBox(pPopup, GET_PLAYER(GC.getGameINLINE().getActivePlayer()).getCivilizationAdjective(), WIDGET_GENERAL, gDLL->getText("TXT_KEY_MENU_CIV_ADJ"), 3, POPUP_LAYOUT_STRETCH, 0, MAX_PLAYERINFO_CHAR_COUNT);
		gDLL->getInterfaceIFace()->popupAddSeparator(pPopup);
	}
	else if (!GC.getInitCore().getCivPassword(GC.getInitCore().getActivePlayer()).empty())
	{
		// the purpose of the popup with the option set to true is to ask for the civ password if it's not set
		return false;
	}
	if (GC.getGameINLINE().isPbem() || GC.getGameINLINE().isHotSeat() || GC.getGameINLINE().isPitboss())
	{
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_MAIN_MENU_PASSWORD"));
		gDLL->getInterfaceIFace()->popupCreateEditBox(pPopup, PASSWORD_DEFAULT, WIDGET_GENERAL, gDLL->getText("TXT_KEY_MAIN_MENU_PASSWORD"), 4, POPUP_LAYOUT_STRETCH, 0, MAX_PASSWORD_CHAR_COUNT);
		gDLL->getInterfaceIFace()->popupAddSeparator(pPopup);
	}
	if ( (GC.getGameINLINE().isPitboss() || GC.getGameINLINE().isPbem()) && !info.getOption1() )
	{
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_POPUP_DETAILS_EMAIL"));
		gDLL->getInterfaceIFace()->popupCreateEditBox(pPopup, CvWString(GC.getInitCore().getEmail(GC.getInitCore().getActivePlayer())), WIDGET_GENERAL, gDLL->getText("TXT_KEY_POPUP_DETAILS_EMAIL"), 5, POPUP_LAYOUT_STRETCH, 0, MAX_PLAYEREMAIL_CHAR_COUNT);
		gDLL->getInterfaceIFace()->popupAddSeparator(pPopup);
	}
	if (GC.getGameINLINE().isPbem() && !info.getOption1())
	{
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_POPUP_DETAILS_SMTP"));
		gDLL->getInterfaceIFace()->popupCreateEditBox(pPopup, CvWString(GC.getInitCore().getSmtpHost(GC.getInitCore().getActivePlayer())), WIDGET_GENERAL, gDLL->getText("TXT_KEY_POPUP_DETAILS_SMTP"), 6, POPUP_LAYOUT_STRETCH, 0, MAX_PLAYEREMAIL_CHAR_COUNT);
		gDLL->getInterfaceIFace()->popupAddSeparator(pPopup);

		if (GC.getGameINLINE().getPbemTurnSent())
		{
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_MISC_SEND"), NULL, 0, WIDGET_GENERAL);
		}
	}

	// Disable leader name edit box for internet games
	if (GC.getInitCore().getMultiplayer() && gDLL->isFMPMgrPublic())
	{
		gDLL->getInterfaceIFace()->popupEnableEditBox(pPopup, 0, false);
	}

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, true, POPUPSTATE_IMMEDIATE);
	return (true);
}

bool CvDLLButtonPopup::launchAdminPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	gDLL->getInterfaceIFace()->popupSetHeaderString(pPopup, gDLL->getText("TXT_KEY_POPUP_GAME_DETAILS"));

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_MAIN_MENU_GAME_NAME"));
	gDLL->getInterfaceIFace()->popupCreateEditBox(pPopup, GC.getInitCore().getGameName(), WIDGET_GENERAL, gDLL->getText("TXT_KEY_MAIN_MENU_GAME_NAME"), 0, POPUP_LAYOUT_STRETCH, 0, MAX_GAMENAME_CHAR_COUNT);
	gDLL->getInterfaceIFace()->popupAddSeparator(pPopup);
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_POPUP_ADMIN_PASSWORD"));
	gDLL->getInterfaceIFace()->popupCreateEditBox(pPopup, PASSWORD_DEFAULT, WIDGET_GENERAL, gDLL->getText("TXT_KEY_POPUP_ADMIN_PASSWORD"), 1, POPUP_LAYOUT_STRETCH, 0, MAX_PASSWORD_CHAR_COUNT);
	gDLL->getInterfaceIFace()->popupAddSeparator(pPopup);
	if (!GC.getGameINLINE().isGameMultiPlayer())
	{
		gDLL->getInterfaceIFace()->popupCreateCheckBoxes(pPopup, 1, 2);
		gDLL->getInterfaceIFace()->popupSetCheckBoxText(pPopup, 0, gDLL->getText("TXT_KEY_POPUP_ADMIN_ALLOW_CHEATS"), 2);
		gDLL->getInterfaceIFace()->popupSetCheckBoxState(pPopup, 0, gDLL->getChtLvl() > 0, 2);
	}

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, true, POPUPSTATE_IMMEDIATE);
	return (true);
}


bool CvDLLButtonPopup::launchAdminPasswordPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_POPUP_ADMIN_PASSWORD"));
	gDLL->getInterfaceIFace()->popupCreateEditBox(pPopup, L"", WIDGET_GENERAL, gDLL->getText("TXT_KEY_POPUP_ADMIN_PASSWORD"), 0, POPUP_LAYOUT_STRETCH, 0, MAX_PASSWORD_CHAR_COUNT);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, true, POPUPSTATE_IMMEDIATE);
	return (true);
}


bool CvDLLButtonPopup::launchExtendedGamePopup(CvPopup* pPopup, CvPopupInfo &info)
{
	gDLL->getInterfaceIFace()->popupSetHeaderString(pPopup, gDLL->getText("TXT_KEY_EXTENDED_GAME_TITLE"));

	if (GC.getGameINLINE().countHumanPlayersAlive() > 0)
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_EXTENDED_GAME_YES"), NULL, 0, WIDGET_GENERAL);
	}
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_EXTENDED_GAME_NO_MENU"), NULL, 1, WIDGET_GENERAL);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false);
	return (true);
}

bool CvDLLButtonPopup::launchDiplomacyPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	gDLL->getInterfaceIFace()->popupSetHeaderString(pPopup, gDLL->getText("TXT_KEY_DIPLOMACY_TITLE"));

	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		PlayerTypes ePlayer = (PlayerTypes) iI;
		CvPlayer& kLoopPlayer = GET_PLAYER(ePlayer);

		if (kLoopPlayer.isAlive())
		{
			if (GET_PLAYER(GC.getGameINLINE().getActivePlayer()).canContact(ePlayer))
			{
				CvWString szTempBuffer = CvWString::format(SETCOLR L"%s" ENDCOLR, kLoopPlayer.getPlayerTextColorR(), kLoopPlayer.getPlayerTextColorG(), kLoopPlayer.getPlayerTextColorB(), kLoopPlayer.getPlayerTextColorA(), kLoopPlayer.getName());
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szTempBuffer, GC.getLeaderHeadInfo(kLoopPlayer.getLeaderType()).getButton(), iI, WIDGET_GENERAL);
				iCount++;
			}
		}
	}

	if (iCount == 0)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL")->getPath(), MAX_PLAYERS, WIDGET_GENERAL);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);
	return true;
}


bool CvDLLButtonPopup::launchAddBuddyPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	gDLL->getInterfaceIFace()->popupSetHeaderString( pPopup, gDLL->getText("TXT_KEY_SYSTEM_ADD_BUDDY", info.getText().GetCString()) );
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_YES"), NULL, 0, WIDGET_GENERAL);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_NO"), NULL, 1, WIDGET_GENERAL);
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false);
	return (true);
}

bool CvDLLButtonPopup::launchForcedDisconnectPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	gDLL->getInterfaceIFace()->popupSetHeaderString( pPopup, gDLL->getText("TXT_KEY_MAIN_MENU_FORCED_DISCONNECT_INGAME") );
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, true);
	return (true);
}

bool CvDLLButtonPopup::launchPitbossDisconnectPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	gDLL->getInterfaceIFace()->popupSetHeaderString( pPopup, gDLL->getText("TXT_KEY_PITBOSS_DISCONNECT") );
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, true);
	return (true);
}

bool CvDLLButtonPopup::launchKickedPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	gDLL->getInterfaceIFace()->popupSetHeaderString( pPopup, gDLL->getText("TXT_KEY_POPUP_KICKED") );
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, true);
	return (true);
}

bool CvDLLButtonPopup::launchEventPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvPlayer& kActivePlayer = GET_PLAYER(GC.getGameINLINE().getActivePlayer());
	EventTriggeredData* pTriggeredData = kActivePlayer.getEventTriggered(info.getData1());
	if (NULL == pTriggeredData)
	{
		return false;
	}

	if (pTriggeredData->m_eTrigger == NO_EVENTTRIGGER)
	{
		return false;
	}

	CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(pTriggeredData->m_eTrigger);

	// Begin EmperorFool: Events with Images
	if (kTrigger.getEventArt())
	{
		gDLL->getInterfaceIFace()->popupAddDDS(pPopup, kTrigger.getEventArt());
	}
	// End EmperorFool: Events with Images
	gDLL->getInterfaceIFace()->popupAddSeparator(pPopup, 10);	// TAC - Events with Images - koma13

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, pTriggeredData->m_szText);

	bool bEventAvailable = false;
	const InfoArray<EventTypes>& Events = kTrigger.getEvents();
	for (int i = 0; i < Events.getLength(); i++)
	{
		if (GET_PLAYER(GC.getGameINLINE().getActivePlayer()).canDoEvent(Events.getEvent(i), *pTriggeredData))
		{
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, GC.getEventInfo(Events.getEvent(i)).getDescription(), GC.getEventInfo(Events.getEvent(i)).getButton(), Events.getEvent(i), WIDGET_CHOOSE_EVENT, Events.getEvent(i), info.getData1());
			bEventAvailable = true;
		}
		else
		{
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, GC.getEventInfo(Events.getEvent(i)).getDescription(), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_EVENT_UNAVAILABLE_BULLET")->getPath(), -1, WIDGET_CHOOSE_EVENT, Events.getEvent(i), info.getData1(), false);
		}
	}

	if (!bEventAvailable)
	{
		return false;
	}

	if (kTrigger.isPickCity())
	{
		CvCity* pCity = kActivePlayer.getCity(pTriggeredData->m_iCityId);
		FAssert(NULL != pCity);
		if (NULL != pCity)
		{
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_EXAMINE_CITY").c_str(), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CITYSELECTION")->getPath(), GC.getNumEventInfos(), WIDGET_GENERAL, -1, -1);
		}
	}

	if (kTrigger.isShowPlot())
	{
		CvPlot* pPlot = GC.getMap().plot(pTriggeredData->m_iPlotX, pTriggeredData->m_iPlotY);
		if (NULL != pPlot)
		{
			gDLL->getEngineIFace()->addColoredPlot(pPlot->getX_INLINE(), pPlot->getY_INLINE(), GC.getColorInfo((ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT")).getColor(), PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_RECOMMENDED_PLOTS);
			gDLL->getInterfaceIFace()->lookAt(pPlot->getPoint(), CAMERALOOKAT_NORMAL);
		}
	}

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, !bEventAvailable, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchFreeColonyPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	int iLoop;
	PlayerTypes ePlayer = GC.getGameINLINE().getActivePlayer();
	if (ePlayer == NO_PLAYER)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_FREE_COLONY"));

	for (CvCity* pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
	{
		PlayerTypes ePlayer = pLoopCity->getLiberationPlayer(false);
		if (NO_PLAYER != ePlayer)
		{
			CvWString szCity = gDLL->getText("TXT_KEY_CITY_LIBERATE", pLoopCity->getNameKey(), GET_PLAYER(ePlayer).getNameKey());
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szCity, ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CITYSELECTION")->getPath(), pLoopCity->getID(), WIDGET_GENERAL);
		}
	}

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL")->getPath(), -1, WIDGET_GENERAL);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchChooseProfessionPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	PlayerTypes ePlayer = GC.getGameINLINE().getActivePlayer();
	if (ePlayer == NO_PLAYER)
	{
		return false;
	}

	CvCity* pCity = GET_PLAYER(ePlayer).getCity(info.getData1());

	bool bEuropeUnit = false;
	CvUnit* pUnit = NULL;
	if (pCity != NULL)
	{
		pUnit = pCity->getPopulationUnitById(info.getData2());
	}
	else
	{
		pUnit = GET_PLAYER(ePlayer).getEuropeUnitById(info.getData2());
		bEuropeUnit = (pUnit != NULL);
	}
	if (NULL == pUnit)
	{
		pUnit = GET_PLAYER(ePlayer).getUnit(info.getData2());
	}
	if (NULL == pUnit)
	{
		return false;
	}

	// Erik: If we have no valid profession to assign, we bail out
	if (pUnit->getProfession() == NO_PROFESSION)
	{
		return false;
	}

	bool bShowOnlyNonCitizens = (info.getData3() == 0);
	bool bShowOnlyPlotCitizens = (info.getData3() == 1);
	//Androrc Multiple Professions per Building
	bool bShowOnlyBuildingCitizens = (info.getData3() == 2);
	//Androrc End


	if (bShowOnlyNonCitizens && !pUnit->canLeaveCity())
	{
		// unrest in city. Show a menu telling nothing can be done.
		// change button type to avoid having code when ok button is clicked
		info.setButtonPopupType(BUTTONPOPUP_NO_EVENT_ON_OK_CLICKED);
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_POPUP_CHOOSE_PROFESSION_UNREST"));
		gDLL->getInterfaceIFace()->popupLaunch(pPopup, true, POPUPSTATE_IMMEDIATE);
		return true;
	}


	CvPlot* pWorkingPlot = NULL;
	if (pCity != NULL)
	{
		pWorkingPlot = pCity->getPlotWorkedByUnit(pUnit);
	}

	//Androrc Multiple Professions per Building
	BuildingTypes eWorkingBuilding = NO_BUILDING;
	if (pCity != NULL)
	{
		// R&R, ray , MYCP partially based on code of Aymerick - START
		eWorkingBuilding = pCity->getYieldBuilding((YieldTypes) GC.getProfessionInfo(pUnit->getProfession()).getYieldsProduced(0));
		// R&R, ray , MYCP partially based on code of Aymerick - END
	}
	//Androrc End

	ProfessionTypes eProfession = pUnit->getProfession();
	FAssert(NULL == pWorkingPlot || (NO_PROFESSION != eProfession && GC.getProfessionInfo(eProfession).isWorkPlot()));

	CvWString szProfession;
	if (NO_PROFESSION == eProfession)
	{
		szProfession = L"TXT_KEY_PROFESSION_IDLE_CITIZEN";
	}
	else
	{
		szProfession = GC.getProfessionInfo(eProfession).getTextKeyWide();
	}

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_CHOOSE_PROFESSION", pUnit->getNameKey(), szProfession.GetCString()));

//Androrc Multiple Professions per Building
//	if (!bShowOnlyPlotCitizens)
	if (!bShowOnlyPlotCitizens && !bShowOnlyBuildingCitizens)
	//Androrc End
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL")->getPath(), GC.getNumProfessionInfos(), WIDGET_GENERAL);

		if(pUnit->isColonistLocked() && !bShowOnlyNonCitizens)
		{
			FAssert(pCity != NULL);
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_AUTOMATE_CITIZEN"), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_CITY_AUTOMATE_CITIZENS")->getPath(), -1, WIDGET_GENERAL);
		}
	}

	int iNumButtons = 0;
	for (int iProfession = 0; iProfession < GC.getNumProfessionInfos(); ++iProfession)
	{
		ProfessionTypes eLoopProfession = (ProfessionTypes) iProfession;
		CvProfessionInfo& kProfession = GC.getProfessionInfo(eLoopProfession);

		//Androrc Multiple Professions per Building
//		if ((iProfession != pUnit->getProfession() || bShowOnlyPlotCitizens) && pUnit->canHaveProfession(eLoopProfession, false))
		if ((iProfession != pUnit->getProfession() || bShowOnlyPlotCitizens || bShowOnlyBuildingCitizens) && pUnit->canHaveProfession(eLoopProfession, false))
		//Androrc End
		{
			//inside or outside city
			// R&R, ray , MYCP partially based on code of Aymerick - START
			if (kProfession.isCitizen() && pCity != NULL)
			{
				if (!bShowOnlyNonCitizens)
				{
					std::vector<YieldTypes> eProfessionYields;
					std::vector<int> aiProfessionYieldChar;
					std::vector<int> aiBestYieldAmount;
					std::vector<int> aiYieldAmount(kProfession.getNumYieldsProduced(), 0);
					for (int i = 0; i < kProfession.getNumYieldsProduced(); i++)
					{
						YieldTypes eYieldProduced = (YieldTypes) kProfession.getYieldsProduced(i);
						if (eYieldProduced != NO_YIELD)
						{
							eProfessionYields.push_back((YieldTypes) eYieldProduced);
							aiProfessionYieldChar.push_back(GC.getYieldInfo(eYieldProduced).getChar());
							aiBestYieldAmount.push_back(pCity->getBestYieldsAmountAvailable(eYieldProduced, eLoopProfession, pUnit));

							if (kProfession.isWorkPlot() && NULL != pWorkingPlot)
							{
								aiYieldAmount[i] = pWorkingPlot->calculatePotentialProfessionYieldsAmount(eYieldProduced, eLoopProfession, pUnit, false);
							}
						}
					}

					if (!eProfessionYields.empty())
					{
						CvWString szTempBuffer;
						CvWString szYieldsProducedList = kProfession.getDescription();
						szYieldsProducedList += L" ";
						int iBestYieldAmount = pCity->getBestYieldAmountAvailable(eLoopProfession, pUnit);
						//Androrc Multiple Professions per Building
//						if (kProfession.isWorkPlot() && NULL != pWorkingPlot)
						if (kProfession.isWorkPlot() && NULL != pWorkingPlot && !bShowOnlyBuildingCitizens)
						//Androrc End
						{
							int iYieldAmount = pWorkingPlot->calculatePotentialProfessionYieldAmount(eLoopProfession, pUnit, false);
							if(iYieldAmount > 0)
							{
								++iNumButtons;
								if (bShowOnlyPlotCitizens)
								{
									for (uint iI = 0; iI < eProfessionYields.size(); iI++)
									{
										// R&R, ray modification
										if (iI > 0)
										{
											szYieldsProducedList += L", ";
											szYieldsProducedList += CvWString::format(L"(%d %c)", aiYieldAmount[0] / 2, aiProfessionYieldChar[iI]);
										}
										else
										{
											szYieldsProducedList += CvWString::format(L"(%d %c)", aiYieldAmount[iI], aiProfessionYieldChar[iI]);
										}
										// R&R, ray modification, end
									}
									szTempBuffer.Format(gDLL->getText("TXT_KEY_CHOOSE_PROFESSION_ITEMS", szYieldsProducedList.GetCString()));
								}
								else
								{

									for (uint iI = 0; iI < eProfessionYields.size(); iI++)
									{
										// R&R, ray modification
										if (iI > 0)
										{
											szYieldsProducedList += L", ";
											szYieldsProducedList += CvWString::format(L"(%d/%d %c)", aiYieldAmount[iI], aiBestYieldAmount[iI], aiProfessionYieldChar[iI]);
										}
										else
										{
											szYieldsProducedList += CvWString::format(L"(%d/%d %c)", aiYieldAmount[iI], aiBestYieldAmount[iI], aiProfessionYieldChar[iI]);
										}
										// R&R, ray modification, end
									}
									szTempBuffer.Format(gDLL->getText("TXT_KEY_CHOOSE_PROFESSION_ITEMS", szYieldsProducedList.GetCString()));
								}

								gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szTempBuffer, kProfession.getButton(), iProfession, WIDGET_GENERAL);
							}
						}
						else if (!bShowOnlyPlotCitizens)
						{
							if(iBestYieldAmount > 0)
							{
								//Androrc Multiple Professions per Building
//								++iNumButtons;
//								szText = gDLL->getText("TXT_KEY_CHOOSE_PROFESSION_ITEM", kProfession.getTextKeyWide(), iBestYieldAmount, iProfessionYieldChar);
//								gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szText, kProfession.getButton(), iProfession, WIDGET_GENERAL);
								if(!bShowOnlyBuildingCitizens || (eWorkingBuilding != NO_BUILDING && kProfession.getSpecialBuilding() == GC.getBuildingInfo(eWorkingBuilding).getSpecialBuildingType()))
								{
									++iNumButtons;
									for (uint iI = 0; iI < eProfessionYields.size(); iI++)
									{
										// R&R, ray modification
										if (iI > 0)
										{
											szYieldsProducedList += L", ";
										}
										if (iI > 0 && kProfession.isWorkPlot())
										{
											szYieldsProducedList += CvWString::format(L"(%d %c)", aiBestYieldAmount[0] / 2, aiProfessionYieldChar[iI]);
										}
										else
										{
											szYieldsProducedList += CvWString::format(L"(%d %c)", aiBestYieldAmount[iI], aiProfessionYieldChar[iI]);
										}
										// R&R, ray modification, end
									}
									szTempBuffer.Format(gDLL->getText("TXT_KEY_CHOOSE_PROFESSION_ITEMS", szYieldsProducedList.GetCString()));
									gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szTempBuffer, kProfession.getButton(), iProfession, WIDGET_GENERAL);
								}
								//Androrc End
							}
						}
					}
				}
			}
			// R&R, ray , MYCP partially based on code of Aymerick - END
			else
			{
				//Androrc Multiple Professions per Building
//				if (!bShowOnlyPlotCitizens)
				if (!bShowOnlyPlotCitizens && !bShowOnlyBuildingCitizens)
				//Androrc End
				{
					CvWString szText = kProfession.getDescription();
					if (bEuropeUnit)
					{
						int iCost = pUnit->getEuropeProfessionChangeCost(eLoopProfession);
						if (iCost > 0)
						{
							szText += gDLL->getText("TXT_KEY_EUROPE_CHANGE_PROFESSION_COST", iCost);
						}
						else if (iCost < 0)
						{
							szText += gDLL->getText("TXT_KEY_EUROPE_CHANGE_PROFESSION_REFUND", -iCost);
						}
					}
					else
					{
						szText += gDLL->getText("TXT_KEY_PROFESSION_NON_CITIZEN");
					}

					++iNumButtons;
					gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szText, kProfession.getButton(), iProfession, WIDGET_GENERAL);
				}
			}
		}
	}

	//Androrc Multiple Professions per Building
//	if (bShowOnlyPlotCitizens)
	if (bShowOnlyPlotCitizens || bShowOnlyBuildingCitizens)
	//Androrc End
	{
		if (iNumButtons <= 1)
		{
			return false;
		}
	}

	//Androrc Multiple Professions per Building
//	if (pUnit->canClearSpecialty() && !bShowOnlyPlotCitizens && !bShowOnlyNonCitizens)
	if (pUnit->canClearSpecialty() && !bShowOnlyPlotCitizens && !bShowOnlyBuildingCitizens && !bShowOnlyNonCitizens)
	//Androrc End
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, GC.getCommandInfo(COMMAND_CLEAR_SPECIALTY).getDescription(), GC.getCommandInfo(COMMAND_CLEAR_SPECIALTY).getButton(), -2, WIDGET_GENERAL);
	}

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchPurchaseEuropeUnitPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	PlayerTypes ePlayer = GC.getGameINLINE().getActivePlayer();
	if (ePlayer == NO_PLAYER)
	{
		return false;
	}
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_PURCHASE_EUROPE_UNIT"));
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL")->getPath(), GC.getNumUnitInfos(), WIDGET_GENERAL);

	bool bFound = false;
	for (int iUnitClass = 0; iUnitClass < GC.getNumUnitClassInfos(); ++iUnitClass)
	{
		UnitTypes eUnit = (UnitTypes) GC.getCivilizationInfo(kPlayer.getCivilizationType()).getCivilizationUnits(iUnitClass);

		if (NO_UNIT != eUnit)
		{
			int iCost = kPlayer.getEuropeUnitBuyPrice(eUnit);
			if (iCost >= 0)
			{
				CvWString szText = gDLL->getText("TXT_KEY_EUROPE_UNIT_BUY_PRICE", GC.getUnitInfo(eUnit).getTextKeyWide(), iCost);
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szText, kPlayer.getUnitButton(eUnit), eUnit, WIDGET_PEDIA_JUMP_TO_UNIT, eUnit, 1);
				bFound = true;
			}
		}
	}

	if (!bFound)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchFoundingFatherPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	FatherTypes eFather = (FatherTypes) info.getData1();

	if (eFather == NO_FATHER)
	{
		return false;
	}

	if (!GET_TEAM(GC.getGameINLINE().getActiveTeam()).canConvinceFather(eFather))
	{
		return false;
	}

	// TAC - FF Picture Popups - koma13 - START
	gDLL->getInterfaceIFace()->popupAddDDS(pPopup, GC.getFatherInfo(eFather).getPortrait(), 208, 208, "");
	gDLL->getInterfaceIFace()->popupAddSeparator(pPopup, -48);
	// TAC - FF Picture Popups - koma13 - END

	CvWStringBuffer szBody;
	szBody.assign(gDLL->getText("TXT_KEY_FOUNDING_FATHER_OFFERS_POPUP", GC.getFatherInfo(eFather).getTextKeyWide()));
	GAMETEXT.setFatherHelp(szBody, eFather, true);
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, szBody.getCString());

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_YES"), NULL, 1, WIDGET_GENERAL);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_NO"), NULL, 0, WIDGET_GENERAL);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_FEAT_ACCOMPLISHED_MORE"), NULL, -1, WIDGET_GENERAL);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchCivicOptionPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CivicOptionTypes eCivicOption = (CivicOptionTypes) info.getData1();

	if (eCivicOption == NO_CIVICOPTION)
	{
		return false;
	}

	PlayerTypes ePlayer = GC.getGameINLINE().getActivePlayer();
	if (ePlayer == NO_PLAYER)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_CONSTITUTION_POPUP", GC.getCivicOptionInfo(eCivicOption).getTextKeyWide()));

	bool bFoundValid = false;
	for (int iCivic = 0; iCivic < GC.getNumCivicInfos(); ++iCivic)
	{
		CvCivicInfo& kCivicInfo = GC.getCivicInfo((CivicTypes) iCivic);
		if (kCivicInfo.getCivicOptionType() == eCivicOption)
		{
			if (GET_PLAYER(ePlayer).canDoCivics((CivicTypes) iCivic))
			{
				bFoundValid = true;
				CvWStringBuffer szBuffer;
				GAMETEXT.parseCivicInfo(szBuffer, (CivicTypes) iCivic, false, true, false);
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szBuffer.getCString(), kCivicInfo.getButton(), iCivic, WIDGET_GENERAL);
			}
		}
	}

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_FEAT_ACCOMPLISHED_MORE"), "", -1, WIDGET_GENERAL);

	if (!bFoundValid)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchTradeRoutesPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	PlayerTypes ePlayer = GC.getGameINLINE().getActivePlayer();
	if (ePlayer == NO_PLAYER)
	{
		return false;
	}

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	CvUnit* pUnit = kPlayer.getUnit(info.getData1());
	if (pUnit == NULL)
	{
		return false;
	}

	CvSelectionGroup* pGroup = pUnit->getGroup();
	if (pGroup == NULL)
	{
		return false;
	}

	std::vector< std::pair<int, CvWString> > aTradeRoutes;
	std::vector<CvTradeRoute*> aiRoutes;
	kPlayer.getTradeRoutes(aiRoutes);
	for (uint i = 0; i < aiRoutes.size(); ++i)
	{
		CvTradeRoute* pTradeRoute = aiRoutes[i];
		if (pGroup->canAssignTradeRoute(pTradeRoute->getID(), i != 0))
		{
			CvWString szText = gDLL->getText("TXT_KEY_TRADE_ROUTE_DESCRIPTION", GC.getYieldInfo(pTradeRoute->getYield()).getTextKeyWide(), pTradeRoute->getSourceCityNameKey(), pTradeRoute->getDestinationCityNameKey());
			aTradeRoutes.push_back(std::make_pair(pTradeRoute->getID(), szText));
		}
	}

	if (aTradeRoutes.empty())
	{
		return false;
	}

	for (uint i = 0; i < aTradeRoutes.size(); ++i)
	{
		int iRouteId = aTradeRoutes[i].first;
		gDLL->getInterfaceIFace()->popupCreateCheckBoxes(pPopup, 1, iRouteId, WIDGET_GENERAL, POPUP_LAYOUT_LEFT);
		gDLL->getInterfaceIFace()->popupSetCheckBoxText(pPopup, 0, aTradeRoutes[i].second, iRouteId);
		CvUnit* pUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();
		if (pUnit != NULL)
		{
			gDLL->getInterfaceIFace()->popupSetCheckBoxState(pPopup, 0, pGroup->isAssignedTradeRoute(iRouteId), iRouteId);
		}
	}

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, true, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchYieldImportExportPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	PlayerTypes ePlayer = GC.getGameINLINE().getActivePlayer();
	if (ePlayer == NO_PLAYER)
	{
		return false;
	}

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	CvCity* pCity = kPlayer.getCity(info.getData1());
	if (pCity == NULL)
	{
		return false;
	}
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_POPUP_IMPORT_TITLE", pCity->getNameKey()));

	// auto traderoute - start - Nightinggale
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_IMPORT_CLEAR_ALL").c_str(), NULL, 0);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_IMPORT_ALL").c_str(), NULL, 1);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_IMPORT_EXPORT_ALL").c_str(), NULL, 2);
	// auto traderoute - end - Nightinggale
	
//bobisback demanded goods
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_IMPORT_DEMANDED_GOODS").c_str(), NULL, 3);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_IMPORT_CONSTRUCTION_GOODS").c_str(), NULL, 4);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_IMPORT_MILITARY_GOODS").c_str(), NULL, 5);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_IMPORT_LIVESTOCK_GOODS").c_str(), NULL, 6);
	
	for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
	{
		const CvYieldInfo& kYield = GC.getYieldInfo(eYield);
		if (kYield.isCargo())
		{
			// R&R mod, vetiarvind, max yield import limit - start
			if(info.getOption1()) //if we are in "condensed" mode, show only producing and importing yields
			{
				if(!isYieldSupportedInExportPanelForCity(pCity, eYield))
					continue;
			}
			// R&R mod, vetiarvind, max yield import limit - end

			// transport feeder - start - Nightinggale


			// import
			gDLL->getInterfaceIFace()->popupStartHLayout(pPopup, 0);

			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, L"", kYield.getButton(), -1, WIDGET_HELP_YIELD, eYield);
			gDLL->getInterfaceIFace()->popupCreateSpinBox(pPopup, eYield, L"", pCity->getImportsLimit(eYield), 10, 0xFFFF, 0);
			gDLL->getInterfaceIFace()->popupCreateCheckBoxes(pPopup, 2, eYield, WIDGET_GENERAL, POPUP_LAYOUT_TOP);
			gDLL->getInterfaceIFace()->popupSetCheckBoxText(pPopup, 0, L"<font=1>" + gDLL->getText("TXT_KEY_POPUP_IMPORT_IMPORT") + L"</font>", eYield, gDLL->getText("TXT_KEY_POPUP_IMPORT_IMPORT_HELP").getWithoutFormatting());
			gDLL->getInterfaceIFace()->popupSetCheckBoxState(pPopup, 0, pCity->isImport(eYield), eYield);
			gDLL->getInterfaceIFace()->popupSetCheckBoxState(pPopup, 1, pCity->getImportsMaintain(eYield), eYield);
			gDLL->getInterfaceIFace()->popupSetCheckBoxText(pPopup, 1, L"<font=1>" + gDLL->getText("TXT_KEY_POPUP_IMPORT_FEEDER") + L"</font>", eYield, gDLL->getText("TXT_KEY_POPUP_IMPORT_FEEDER_HELP").getWithoutFormatting());
			gDLL->getInterfaceIFace()->popupEndLayout(pPopup);

			// export
			YieldTypes eExportYield = eYield + NUM_YIELD_TYPES;

			gDLL->getInterfaceIFace()->popupStartHLayout(pPopup, 0);

			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, L"", kYield.getButton(), -1, WIDGET_HELP_YIELD, eYield);
			gDLL->getInterfaceIFace()->popupCreateSpinBox(pPopup, eExportYield, L"", pCity->getMaintainLevel(eYield), 10, 0xFFFF, 0);
			gDLL->getInterfaceIFace()->popupCreateCheckBoxes(pPopup, 2, eExportYield, WIDGET_GENERAL, POPUP_LAYOUT_TOP);
			gDLL->getInterfaceIFace()->popupSetCheckBoxText(pPopup, 0, L"<font=1>" + gDLL->getText("TXT_KEY_POPUP_IMPORT_EXPORT") + L"</font>", eExportYield, gDLL->getText("TXT_KEY_POPUP_IMPORT_EXPORT_HELP").getWithoutFormatting());
			gDLL->getInterfaceIFace()->popupSetCheckBoxState(pPopup, 0, pCity->isExport(eYield), eExportYield);
			gDLL->getInterfaceIFace()->popupSetCheckBoxState(pPopup, 1, pCity->isAutoExport(eYield), eExportYield);
			gDLL->getInterfaceIFace()->popupSetCheckBoxText(pPopup, 1, L"<font=1>" + gDLL->getText("TXT_KEY_POPUP_IMPORT_AUTO_EXPORT") + L"</font>", eExportYield, gDLL->getText("TXT_KEY_POPUP_IMPORT_AUTO_EXPORT_HELP").getWithoutFormatting());
			gDLL->getInterfaceIFace()->popupEndLayout(pPopup);
			// R&R mod, vetiarvind, max yield import limit - end
		}
	}

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, true, POPUPSTATE_IMMEDIATE);

	return true;
}

// Teacher List - start - Nightinggale
bool CvDLLButtonPopup::launchTeacherListPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	PlayerTypes ePlayer = GC.getGameINLINE().getActivePlayer();
	if (ePlayer == NO_PLAYER)
	{
		return false;
	}

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	CvCity* pCity = kPlayer.getCity(info.getData1());
	if (pCity == NULL)
	{
		return false;
	}

	if (pCity->NBMOD_GetCityTeachLevel())
	{
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_EDIT_TEACHER_LIST_POPUP_TEXT", pCity->getNameKey()));
		gDLL->getInterfaceIFace()->popupStartHLayout(pPopup, 0);
		gDLL->getInterfaceIFace()->popupCreateCheckBoxes(pPopup, 1, 1, WIDGET_GENERAL, POPUP_LAYOUT_TOP);
		gDLL->getInterfaceIFace()->popupSetCheckBoxText(pPopup, 0, gDLL->getText("TXT_KEY_EDIT_TEACHER_LIST_POPUP_RESET_ALL_TEXT"), 1, gDLL->getText("TXT_KEY_EDIT_TEACHER_LIST_POPUP_RESET_ALL_HELP").getWithoutFormatting());
		gDLL->getInterfaceIFace()->popupEndLayout(pPopup);
		for (int iUnitClass = 0; iUnitClass < GC.getNumUnitClassInfos(); iUnitClass++)
		{
			UnitTypes eUnit = (UnitTypes) GC.getCivilizationInfo(kPlayer.getCivilizationType()).getCivilizationUnits(iUnitClass);
			if (eUnit == NO_UNIT) continue; // Unitclass not used by civ

			CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);

			if (kUnit.NBMOD_GetTeachLevel() < 1 || kUnit.NBMOD_GetTeachLevel() > pCity->NBMOD_GetCityTeachLevel()) continue;

			int iPrice = pCity->getSpecialistTuition(eUnit);

			gDLL->getInterfaceIFace()->popupStartHLayout(pPopup, 0);
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, L"", kUnit.getButton(), -1, WIDGET_HELP_TEACHER_UNIT, eUnit);
			if (iPrice >= 0)
			{
				gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, CvWString::format(L" %d%c", iPrice, GC.getSymbolID(GOLD_CHAR)));
				gDLL->getInterfaceIFace()->popupCreateCheckBoxes(pPopup, 1, eUnit, WIDGET_GENERAL, POPUP_LAYOUT_TOP);
				gDLL->getInterfaceIFace()->popupSetCheckBoxText(pPopup, 0, gDLL->getText("TXT_KEY_EDIT_TEACHER_LIST_POPUP_REPEAT_TEXT"), eUnit, gDLL->getText("TXT_KEY_EDIT_TEACHER_LIST_POPUP_REPEAT_HELP").getWithoutFormatting());
				gDLL->getInterfaceIFace()->popupSetCheckBoxState(pPopup, 0, pCity->getOrderedStudentsRepeat(eUnit), eUnit);
				gDLL->getInterfaceIFace()->popupCreateSpinBox(pPopup, eUnit, L"", pCity->getOrderedStudents(eUnit), 1, 50, 0);
			} else {
				gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_EDIT_TEACHER_LIST_NO_TEACHER", kUnit.getDescription()));
			}
			gDLL->getInterfaceIFace()->popupEndLayout(pPopup);
		}
	} else {
		// Colony has no school
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_EDIT_TEACHER_LIST_POPUP_NO_SCHOOL_TEXT", pCity->getNameKey()));
	}
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, true, POPUPSTATE_IMMEDIATE);

	return true;
}
// Teacher List - end - Nightinggale

// R&R, Robert Surcouf, Custom House Popup-Screen START
bool CvDLLButtonPopup::launchCustomHousePopup(CvPopup* pPopup, CvPopupInfo &info)
{
	PlayerTypes ePlayer = GC.getGameINLINE().getActivePlayer();
	if (ePlayer == NO_PLAYER)
	{
		return false;
	}

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	CvCity* pCity = kPlayer.getCity(info.getData1());
	if (pCity == NULL)
	{
		return false;
	}

	if (!pCity->getHasUnlockedStorageLossTradeSettings())
	{
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_CUSTOM_HOUSE_NOT_BUILT_YET_POPUP_TEXT", pCity->getNameKey()));
	}
	else
	{
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_CUSTOM_HOUSE_POPUP_TEXT", pCity->getNameKey()));
	}

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		YieldTypes eYield = (YieldTypes) iYield;
		CvYieldInfo& kYield = GC.getYieldInfo(eYield);
		// R&R, ray, finishing Custom House Screen
		if (kYield.isCargo())
		{
			// ray, making special storage capacity rules for Yields XML configurable
			// if (eYield == YIELD_FOOD || eYield == YIELD_LUMBER || eYield == YIELD_STONE)
			if (GC.getYieldInfo(eYield).isIgnoredForStorageCapacity())
			{
				// Write never sell
				gDLL->getInterfaceIFace()->popupStartHLayout(pPopup, 0);
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, L"", kYield.getButton(), -1, WIDGET_HELP_YIELD, iYield);
				gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_POPUP_NEVER_SELL"));
				gDLL->getInterfaceIFace()->popupEndLayout(pPopup);

				// disabled the full line. TODO figure out how we want to display this for a permanent solution.
				/*
				gDLL->getInterfaceIFace()->popupStartHLayout(pPopup, 0);
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, L"", kYield.getButton(), -1, WIDGET_HELP_YIELD, iYield);
				gDLL->getInterfaceIFace()->popupCreateCheckBoxes(pPopup, 1, iYield, WIDGET_GENERAL, POPUP_LAYOUT_TOP);
				gDLL->getInterfaceIFace()->popupSetCheckBoxText(pPopup, 0, gDLL->getText("TXT_KEY_POPUP_NEVER_SELL"), iYield);
				// R&R, ray, finishing Custom House Screen
				gDLL->getInterfaceIFace()->popupSetCheckBoxState(pPopup, 0, true, iYield);
				gDLL->getInterfaceIFace()->popupCreateSpinBox(pPopup, iYield, L"", pCity->getCustomHouseSellThreshold(eYield), 10, 999, 0);
				gDLL->getInterfaceIFace()->popupEndLayout(pPopup);
				*/
			}
			else
			{
				gDLL->getInterfaceIFace()->popupStartHLayout(pPopup, 0);
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, L"", kYield.getButton(), -1, WIDGET_HELP_YIELD, iYield);
				gDLL->getInterfaceIFace()->popupCreateCheckBoxes(pPopup, 1, iYield, WIDGET_GENERAL, POPUP_LAYOUT_TOP);
				gDLL->getInterfaceIFace()->popupSetCheckBoxText(pPopup, 0, gDLL->getText("TXT_KEY_POPUP_NEVER_SELL"), iYield);
				// R&R, ray, finishing Custom House Screen
				gDLL->getInterfaceIFace()->popupSetCheckBoxState(pPopup, 0, pCity->isCustomHouseNeverSell(eYield), iYield);
				gDLL->getInterfaceIFace()->popupCreateSpinBox(pPopup, iYield, L"", pCity->getCustomHouseSellThreshold(eYield), 10, 999, 0);
				gDLL->getInterfaceIFace()->popupEndLayout(pPopup);
			}
		}
	}

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, true, POPUPSTATE_IMMEDIATE);

	return true;
}
bool CvDLLButtonPopup::launchDomesticMarketPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	PlayerTypes ePlayer = GC.getGameINLINE().getActivePlayer();
	if (ePlayer == NO_PLAYER)
	{
		return false;
	}

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	CvCity* pCity = kPlayer.getCity(info.getData1());
	if (pCity == NULL)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_DOMESTIC_MARKET_POPUP", pCity->getNameKey()));

	gDLL->getInterfaceIFace()->popupStartHLayout(pPopup, 0);
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_EU_TRADE_LOG_1"));
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_STORED"));
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_DOMESTIC_PRICE"));
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_DOMESTIC_DEMAND"));
	gDLL->getInterfaceIFace()->popupEndLayout(pPopup);

	// R&R, ray, adjustment to displayed yield list of Domestic Market Screen
	YieldCargoArray<int> aYields;
	pCity->getYieldDemands(aYields);

	const InfoArray<YieldTypes>& kYieldArray = GC.getDomesticDemandYieldTypes();
	for (int i = 0; i < kYieldArray.getLength(); ++i)
	{
		YieldTypes eYield = kYieldArray.get(i);
		const CvYieldInfo& kYield = GC.getYieldInfo(eYield);

		gDLL->getInterfaceIFace()->popupStartHLayout(pPopup, 0);
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, L"", kYield.getButton(), -1, WIDGET_HELP_YIELD, eYield);
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_NUMBER", pCity->getYieldStored(eYield)));
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_NUMBER", pCity->getYieldBuyPrice(eYield)));
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_NUMBER", aYields.get(eYield)));
		gDLL->getInterfaceIFace()->popupEndLayout(pPopup);
	}

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, true, POPUPSTATE_IMMEDIATE);

	return true;
}
// R&R, Robert Surcouf, Custom House Popup-Screen END


bool CvDLLButtonPopup::launchPromotionPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	PlayerTypes ePlayer = GC.getGameINLINE().getActivePlayer();
	if (ePlayer == NO_PLAYER)
	{
		return false;
	}

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	CvSelectionGroup* pSelectionGroup = kPlayer.getSelectionGroup(info.getData1());
	if (pSelectionGroup == NULL)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupSetHeaderString(pPopup, gDLL->getText("TXT_KEY_CHOOSE_PROMOTION"));
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL")->getPath(), GC.getNumPromotionInfos(), WIDGET_GENERAL);

	int iNumPromotions = 0;
	for (int iPromotion = 0; iPromotion < GC.getNumPromotionInfos(); iPromotion++)
	{
		if (iPromotion != info.getData2())
		{
			CLLNode<IDInfo>* pSelectedUnitNode = gDLL->getInterfaceIFace()->headSelectionListNode();

			while (pSelectedUnitNode != NULL)
			{
				CvUnit* pSelectedUnit = ::getUnit(pSelectedUnitNode->m_data);
				pSelectedUnitNode = gDLL->getInterfaceIFace()->nextSelectionListNode(pSelectedUnitNode);
				if (pSelectedUnit != NULL && pSelectedUnit->canPromote((PromotionTypes) iPromotion, info.getData2()))
				{
					gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText(GC.getPromotionInfo((PromotionTypes) iPromotion).getTextKeyWide()), GC.getPromotionInfo((PromotionTypes)iPromotion).getButton(), iPromotion, WIDGET_HELP_PROMOTION, iPromotion, -1);
					++iNumPromotions;
					break;
				}
			}
		}
	}

	if (iNumPromotions == 0)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchChooseGoodyPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	GoodyTypes eGoody = (GoodyTypes) info.getData1();
	if (eGoody == NO_GOODY)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText(GC.getGoodyInfo(eGoody).getAnnounceTextKey()));
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_YES"), NULL, 1);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_NO"), NULL, 0);
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchSelectYieldAmountPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	YieldTypes eYield = (YieldTypes) info.getData1();
	if (eYield == NO_YIELD)
	{
		return false;
	}

	PlayerTypes ePlayer = GC.getGameINLINE().getActivePlayer();
	if (ePlayer == NO_PLAYER)
	{
		return false;
	}
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	CvUnit* pUnit = kPlayer.getUnit(info.getData2());
	if (pUnit == NULL)
	{
		return false;
	}

	//R&R, vetiarvind, bug fix for shift-key trading in africa and PR - start
	UnitTravelStates uts = pUnit->getUnitTravelState();
	// WTP, ray, refixing SHIFT-KEY
	// if (uts != UNIT_TRAVEL_STATE_IN_EUROPE && uts != UNIT_TRAVEL_STATE_IN_AFRICA && uts != UNIT_TRAVEL_STATE_FROM_PORT_ROYAL)
	if (uts != UNIT_TRAVEL_STATE_IN_EUROPE && uts != UNIT_TRAVEL_STATE_IN_AFRICA && uts != UNIT_TRAVEL_STATE_IN_PORT_ROYAL)
	//if (pUnit->getUnitTravelState() != UNIT_TRAVEL_STATE_IN_EUROPE)
	//R&R, vetiarvind, bug fix for shift-key trading in africa and PR - end
	{
		FAssert(pUnit->getUnitTravelState() == NO_UNIT_TRAVEL_STATE);
		if (!pUnit->canDoCommand((CommandTypes) info.getData3(), info.getData1(), -1))
		{
			return false;
		}
	}

	int iMaxAmount = info.getOption1() ? pUnit->getMaxLoadYieldAmount(eYield) : pUnit->getYieldStored();
	if (iMaxAmount <= 0)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_SELECT_YIELD_POPUP", GC.getYieldInfo(eYield).getTextKeyWide()));
	gDLL->getInterfaceIFace()->popupCreateSpinBox(pPopup, 0, L"", iMaxAmount, 1, iMaxAmount, 0);
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, true, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchTalkNativesPopup(CvPopup* pPopup, CvPopupInfo& info)
{
	PlayerTypes ePlayer = GC.getGameINLINE().getActivePlayer();
	if (ePlayer == NO_PLAYER)
	{
		return false;
	}

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	CvUnit* pUnit = kPlayer.getUnit(info.getData1());
	if (pUnit == NULL)
	{
		return false;
	}
	CvPlot* pPlot = pUnit->plot();
	if (pPlot == NULL)
	{
		return false;
	}
	CvCity* pCity = pPlot->getPlotCity();
	if (pCity == NULL)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_TALK_NATIVES_POPUP", pCity->getNameKey()));

	int iNumActions = 0;
	if (pUnit->canSpeakWithChief(pUnit->plot()))
	{
		++iNumActions;
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_TALK_NATIVES_POPUP_CHIEF"), NULL, COMMAND_SPEAK_WITH_CHIEF);
	}

	if (pUnit->canLearn())
	{
		++iNumActions;
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_TALK_NATIVES_POPUP_LEARN"), NULL, COMMAND_LEARN);
	}

	if (pUnit->canEstablishMission())
	{
		++iNumActions;
		CvWString szText = gDLL->getText("TXT_KEY_TALK_NATIVES_POPUP_MISSION");
		szText += L" (" + gDLL->getText("TXT_KEY_TALK_NATIVES_POPUP_MISSION2", std::min(100, pUnit->getMissionarySuccessPercent())) + L")";
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szText, NULL, COMMAND_ESTABLISH_MISSION);
	}

	// WTP, ray, Native Trade Posts - START
	if (pUnit->canEstablishTradePost())
	{
		++iNumActions;
		CvWString szText = gDLL->getText("TXT_KEY_TALK_NATIVES_POPUP_TRADE_POST");
		szText += L" (" + gDLL->getText("TXT_KEY_TALK_NATIVES_POPUP_TRADE_POST2", std::min(100, pUnit->getNativeTradePostSuccessPercent())) + L")";
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szText, NULL, COMMAND_ESTABLISH_TRADE_POST);
	}
	// WTP, ray, Native Trade Posts - END

	if (pUnit->canTradeYield(pUnit->plot()))
	{
		++iNumActions;
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_TALK_NATIVES_POPUP_TRADE"), NULL, COMMAND_YIELD_TRADE);
	}

	if (iNumActions == 0)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_TALK_NATIVES_POPUP_NOTHING"), NULL, -1);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_TALK_NATIVES_POPUP_NEVER"), NULL, NUM_COMMAND_TYPES);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}


// PatchMod: Achievements START
bool CvDLLButtonPopup::launchAchievementPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvPlayer& kActivePlayer = GET_PLAYER((PlayerTypes)info.getData2());
	CvAchieveInfo& pAchievement = GC.getAchieveInfo((AchieveTypes)info.getData1());

	if (!pAchievement.isActive())
	{
		return false;
	}
	if (pAchievement.getPictureFileName() != NULL && pAchievement.getPictureX() > 0 && pAchievement.getPictureY() > 0)
	{
		gDLL->getInterfaceIFace()->popupAddDDS(pPopup, pAchievement.getPictureFileName(), pAchievement.getPictureX(), pAchievement.getPictureY());
	}
	gDLL->getInterfaceIFace()->popupSetHeaderString(pPopup, gDLL->getText(pAchievement.getHeaderTextKey()));
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText(pAchievement.getBodyTextKey()));
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, true, POPUPSTATE_IMMEDIATE);
	return true;
}
// PatchMod: Achievements END

// TAC - Goto Menu - koma13 - START
bool CvDLLButtonPopup::launchGotoMenuPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	PlayerTypes ePlayer = GC.getGameINLINE().getActivePlayer();
	if (ePlayer == NO_PLAYER)
	{
		return false;
	}
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	CvUnit* pUnit = kPlayer.getUnit(info.getData1());
	if (pUnit == NULL)
	{
		return false;
	}

	bool bValid = false;
	int iLoop;

	gDLL->getInterfaceIFace()->popupSetHeaderString(pPopup, gDLL->getText("TXT_KEY_COMMAND_GOTO_MENU_TITLE"));

	// WTP, ray, prevent Coastal Ships to Display EUROPE, AFRICA and Port Royal in GO-TO -START
	if (pUnit->canCrossCoastOnly() == false)
	{
		if (pUnit->canCrossOcean(pUnit->plot(), UNIT_TRAVEL_STATE_TO_EUROPE) || pUnit->canAutoCrossOcean(pUnit->plot()))
		{
			CvString szArtFilename = (kPlayer.getParent() != NO_PLAYER) ? GC.getCivilizationInfo(GET_PLAYER(kPlayer.getParent()).getCivilizationType()).getButton() : ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CITYSELECTION")->getPath();
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, L"  " + gDLL->getText("TXT_KEY_COMMAND_SAIL_TO_EUROPE"), szArtFilename, -2, WIDGET_GENERAL);
			bValid = true;
		}
		// R&R, vetiarvind, Goto other screens - START
		if (pUnit->canCrossOcean(pUnit->plot(), UNIT_TRAVEL_STATE_TO_AFRICA) || pUnit->canAutoCrossOcean(pUnit->plot()))
		{
			CvString szArtFilename = (kPlayer.getParent() != NO_PLAYER) ? GC.getCivilizationInfo(GET_PLAYER(kPlayer.getParent()).getCivilizationType()).getButton() : ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CITYSELECTION")->getPath();
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, L"  " + gDLL->getText("TXT_KEY_COMMAND_SAIL_TO_AFRICA"), szArtFilename, -3, WIDGET_GENERAL, pUnit->getID(), -1);
			bValid = true;
		}
		// WTP, ray, added a bracket around or before caSailToPortRoyal check to fix changed order a bit - fix for issue 252
		if (pUnit->canSailToPortRoyal(NULL) && (pUnit->canCrossOcean(pUnit->plot(), UNIT_TRAVEL_STATE_TO_PORT_ROYAL) || (pUnit->canAutoCrossOcean(pUnit->plot())))) //R&R, vetiarvind fix for hidden-nationality units to sail to PR
		{
			const char* portRoyalImage = ARTFILEMGR.getInterfaceArtInfo("INTERFACE_PORT_ROYAL")->getPath();
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, L"  " + gDLL->getText("TXT_KEY_COMMAND_SAIL_TO_PORT_ROYAL"), portRoyalImage, -4, WIDGET_GENERAL, pUnit->getID(), -1);
			bValid = true;
		}
	}
	// WTP, ray, prevent Coastal Ships to Display EUROPE, AFRICA and Port Royal in GO-TO -END
	// R&R, vetiarvind, Goto other screens - END
	for (CvCity* pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
	{
		if (pUnit->plot() != pLoopCity->plot())
		{
			int iPathTurns = 0;
			// if (pUnit->generatePath(pLoopCity->plot(), 0, true, &iPathTurns))
			if (pUnit->generatePath(pLoopCity->plot(), 0, false, &iPathTurns))	// R&R, ray, improvment from vetiarvind
			{
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, L"  " + gDLL->getText("TXT_KEY_COMMAND_GOTO_MENU_SELECTION", pLoopCity->getNameKey(), iPathTurns), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CITYSELECTION")->getPath(), pLoopCity->getID(), WIDGET_GENERAL, pUnit->getID(), -1);
				bValid = true;
			}
		}
	}

	if (!bValid)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, L"  " + gDLL->getText("TXT_KEY_NEVER_MIND"), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL")->getPath(), -1, WIDGET_GENERAL);
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}
// TAC - Goto Menu - koma13 - END

/*** TRIANGLETRADE 10/24/08 by DPII ***/
bool CvDLLButtonPopup::launchPurchaseAfricaUnitPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	PlayerTypes ePlayer = GC.getGameINLINE().getActivePlayer();
	if (ePlayer == NO_PLAYER)
	{
		return false;
	}

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_PURCHASE_EUROPE_UNIT"));
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL")->getPath(), GC.getNumUnitInfos(), WIDGET_GENERAL);

	bool bFound = false;
	for (int iUnitClass = 0; iUnitClass < GC.getNumUnitClassInfos(); ++iUnitClass)
	{
		UnitTypes eUnit = (UnitTypes) GC.getCivilizationInfo(kPlayer.getCivilizationType()).getCivilizationUnits(iUnitClass);
		if (NO_UNIT != eUnit)
		{
			int iCost = kPlayer.getAfricaUnitBuyPrice(eUnit);
			if (iCost >= 0)
			{
				CvWString szText = gDLL->getText("TXT_KEY_EUROPE_UNIT_BUY_PRICE", GC.getUnitInfo(eUnit).getTextKeyWide(), iCost);
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szText, kPlayer.getUnitButton(eUnit), eUnit, WIDGET_PEDIA_JUMP_TO_UNIT, eUnit, 1);
				bFound = true;
			}
		}
	}

	if (!bFound)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}
/*************************************/

// R&R, ray, Port Royal
bool CvDLLButtonPopup::launchPurchasePortRoyalUnitPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	PlayerTypes ePlayer = GC.getGameINLINE().getActivePlayer();
	if (ePlayer == NO_PLAYER)
	{
		return false;
	}

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_PURCHASE_EUROPE_UNIT"));
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL")->getPath(), GC.getNumUnitInfos(), WIDGET_GENERAL);

	bool bFound = false;
	for (int iUnitClass = 0; iUnitClass < GC.getNumUnitClassInfos(); ++iUnitClass)
	{
		UnitTypes eUnit = (UnitTypes) GC.getCivilizationInfo(kPlayer.getCivilizationType()).getCivilizationUnits(iUnitClass);
		if (NO_UNIT != eUnit)
		{
			int iCost = kPlayer.getPortRoyalUnitBuyPrice(eUnit);
			if (iCost >= 0)
			{
				CvWString szText = gDLL->getText("TXT_KEY_EUROPE_UNIT_BUY_PRICE", GC.getUnitInfo(eUnit).getTextKeyWide(), iCost);
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szText, kPlayer.getUnitButton(eUnit), eUnit, WIDGET_PEDIA_JUMP_TO_UNIT, eUnit, 1);
				bFound = true;
			}
		}
	}

	if (!bFound)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}
// R&R, ray, Port Royal - END

//R&R mod, vetiarvind, trade groups - start
bool CvDLLButtonPopup::launchSaveTradeGroupPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	PlayerTypes ePlayer = GC.getGameINLINE().getActivePlayer();
	if (ePlayer == NO_PLAYER)
	{
		return false;
	}


	gDLL->getInterfaceIFace()->popupSetHeaderString(pPopup, gDLL->getText("TXT_KEY_PITBOSS_SAVE"));
	gDLL->getInterfaceIFace()->popupCreateEditBox(pPopup);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL")->getPath(), -2, WIDGET_GENERAL);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, true, POPUPSTATE_IMMEDIATE);

	return true;
}

std::vector<std::string> CvDLLButtonPopup::split(CvString &s, char delim)
{
	std::stringstream ss(s);
	std::string item;
	std::vector<std::string> elems;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

//R&R mod, vetiarvind, trade groups - end


// R&R mod, vetiarvind, max yield import limit - start
bool CvDLLButtonPopup::isYieldSupportedInExportPanelForCity(CvCity* pCity, YieldTypes eYield)
{
	if(pCity->getYieldRate(eYield) == 0 && !pCity->isImport(eYield) && !pCity->getImportsMaintain(eYield) && !pCity->isExport(eYield))
		return false;
	return true;
}

// R&R mod, vetiarvind, max yield import limit - end

bool CvDLLButtonPopup::launchChooseCityPlotYieldPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvCity *pCity = gDLL->getInterfaceIFace()->getHeadSelectedCity();

	if (pCity == NULL)
	{
		return false;
	}

	CvPlot *pPlot = pCity->plot();

	if (pPlot == NULL)
	{
		return false;
	}

	PlayerTypes ePlayer = GC.getGameINLINE().getActivePlayer();

	if (ePlayer == NO_PLAYER)
	{
		return false;
	}

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	info.setData1(pCity->getID());


	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_BUTTON_POPUP_CHOOSE_CITY_PLOT_YIELD_TITLE"));
	gDLL->getInterfaceIFace()->popupEndLayout(pPopup);

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_BUTTON_POPUP_CHOOSE_CITY_PLOT_YIELD_AUTO"), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_AUTOMATE")->getPath(), -1, WIDGET_HELP_TEXT, -1, HELP_TEXT_BUTTON_POPUP_CITY_YIELD_PLOT);

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		YieldTypes eYield = (YieldTypes)iYield;
		int iAmount = pPlot->calculatePotentialCityYield(eYield, pCity);
		if (iAmount > 0)
		{
			CvYieldInfo& kYieldInfo = GC.getYieldInfo(eYield);

			CvWString text = CvWString::format(L"%d%c ", iAmount, kYieldInfo.getChar());
			text += gDLL->getText(kYieldInfo.getTextKeyWide());

			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, text, kYieldInfo.getButton(), eYield, WIDGET_HELP_YIELD, eYield, -1);
		}
	}

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL")->getPath(), -2, WIDGET_GENERAL);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchDesyncLogCompletePopup(CvPopup* pPopup, CvPopupInfo &info)
{
	int iPlayer = GC.getGameINLINE().getActivePlayer();
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_DEBUG_DESYNC_LOG_COMPLETE", iPlayer, iPlayer, iPlayer));

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, true, POPUPSTATE_IMMEDIATE);

	return true;
}
