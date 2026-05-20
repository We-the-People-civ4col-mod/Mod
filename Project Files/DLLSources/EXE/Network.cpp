#include "../CvGameCoreDLL.h"
#include "Network.h"
#include "../DesyncMonitor.h"

namespace EXE
{
	namespace network
	{
		// city
		void CityPushOrder(int iCityID, OrderTypes eOrder, int iData, bool bAlt, bool bShift, bool bCtrl)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendPushOrder(iCityID, eOrder, iData, bAlt, bShift, bCtrl);
		}

		void CityPopOrder(int iCity, int iNum)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendPopOrder(iCity, iNum);
		}

		void CityTask(int iCity, TaskTypes eTask, int iData1, int iData2, bool bOption, bool bAlt, bool bShift, bool bCtrl)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendDoTask(iCity, eTask, iData1, iData2, bOption, bAlt, bShift, bCtrl);
		}

		// global
		void sendExtendedGame()
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendExtendedGame();
		}

		void sendPause(int iPauseID)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendPause(iPauseID);
		}

		void sendGameInfo(const CvWString& szGameName, const CvWString& szAdminPassword)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendGameInfo(szGameName, szAdminPassword);
		}

		// player 
		void sendPlayerInfo(PlayerTypes eActivePlayer)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendPlayerInfo(eActivePlayer);
		}

		void sendMPRetire()
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendMPRetire();
		}

		void PlayerAdvancedStartAction(AdvancedStartActionTypes eAction, PlayerTypes ePlayer, int iX, int iY, int iData, bool bAdd)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendAdvancedStartAction(eAction, ePlayer, iX, iY, iData, bAdd);
		}

		void PlayerAdvancedStartAction(AdvancedStartActionTypes eAction, PlayerTypes ePlayer, Coordinates coord, int iData, bool bAdd)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendAdvancedStartAction(eAction, ePlayer, coord.x(), coord.y(), iData, bAdd);
		}

		void PlayerAction(PlayerTypes ePlayer, PlayerActionTypes eAction, int iData1, int iData2, int iData3)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendPlayerAction(ePlayer, eAction, iData1, iData2, iData3);
		}

		void SetDone(bool bDone)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->SetDone(bDone);
		}

		// plot
		void PlotPing(Coordinates coord)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendPing(coord.x(), coord.y());
		}

		// unit
		void UnitJoinGroup(int iUnitID, int iHeadID)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendJoinGroup(iUnitID, iHeadID);
		}

		void SelectionGroupPushMission(int iUnitID, MissionTypes eMission, int iData1, int iData2, int iFlags, bool bShift) // calls the selection group the unit is in
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendPushMission(iUnitID, eMission, iData1, iData2, iFlags, bShift);
		}

		void UnitAutoMission(int iUnitID)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendAutoMission(iUnitID);
		}

		void UnitCommand(int iUnitID, CommandTypes eCommand, int iData1, int iData2, bool bAlt) // calls the unit itself
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendDoCommand(iUnitID, eCommand, iData1, iData2, bAlt);
		}

		// misc
		void sendPopup(PlayerTypes ePlayer, CvPopupInfo* pInfo)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendPopup(ePlayer, pInfo);
		}

		void sendPlayerOption(PlayerOptionTypes eOption, bool bValue)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendPlayerOption(eOption, bValue);
		}

		void sendChat(const CvWString& szChatString, ChatTargetTypes eTarget)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendChat(szChatString, eTarget);
		}


		// send to EXE_CvPlayer::changeCivic() where the player is the active player
		void sendArray(int* pArray)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendUpdateCivics((CivicTypes*)pArray);
		}

		// diplomacy
		void beginDiplomacy(CvDiploParameters* pDiploParams, PlayerTypes ePlayer)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->beginDiplomacy(pDiploParams, ePlayer);
		}

		void endDiplomacy()
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->endDiplomacy();
		}

		void updateDiplomacyAttitude(bool bForce)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->updateDiplomacyAttitude(bForce);
		}

		void beginMPDiplomacy(PlayerTypes eWhoTalkingTo, bool bRenegotiate, bool bSimultaneous, IDInfo kTransport)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->beginMPDiplomacy(eWhoTalkingTo, bRenegotiate, bSimultaneous, kTransport);
		}

		void endMPDiplomacy()
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->endMPDiplomacy();
		}

		void sendToggleTradeMessage(PlayerTypes eWho, const TradeData& kTradeData, int iOtherWho, bool bAIOffer, bool bSendToAll)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendToggleTradeMessage(eWho, kTradeData, iOtherWho, bAIOffer, bSendToAll);
		}

		void sendClearTableMessage(PlayerTypes eWhoTradingWith)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendClearTableMessage(eWhoTradingWith);
		}

		void sendImplementDealMessage(PlayerTypes eOtherWho, CLinkList<TradeData>* pOurList, CLinkList<TradeData>* pTheirList)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendImplementDealMessage(eOtherWho, pOurList, pTheirList);
		}

		void sendChangeWar(TeamTypes iRivalTeam, bool bWar)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendChangeWar(iRivalTeam, bWar);
		}

		void sendContactCiv(NetContactTypes eContactType, PlayerTypes eWho, int iTransportId)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendContactCiv(eContactType, eWho, iTransportId);
		}

		void sendOffer()
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendOffer();
		}

		void sendDiploEvent(PlayerTypes eWhoTradingWith, DiploEventTypes eDiploEvent, int iData1, int iData2)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendDiploEvent(eWhoTradingWith, eDiploEvent, iData1, iData2);
		}

		void sendRenegotiate(PlayerTypes eWhoTradingWith)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendRenegotiate(eWhoTradingWith);
		}

		void sendRenegotiateThisItem(PlayerTypes ePlayer2, TradeableItems eItemType, int iData)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendRenegotiateThisItem(ePlayer2, eItemType, iData);
		}

		void sendExitTrade()
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendExitTrade();
		}

		void sendKillDeal(int iDealID, bool bFromDiplomacy, TeamTypes eEndingTeam)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendKillDeal(iDealID, bFromDiplomacy, eEndingTeam);
		}

		void sendDiplomacy(PlayerTypes ePlayer, CvDiploParameters* pParams)
		{
			FAssert(!CxDesyncMonitor::isCurrentlySync());
			gDLL->sendDiplomacy(ePlayer, pParams);
		}
	}
}
