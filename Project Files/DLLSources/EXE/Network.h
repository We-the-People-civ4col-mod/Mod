
struct IDInfo;
class CvWString;
struct TradeData;
class Coordinates;

template<class T>
class CLinkList;

namespace EXE
{
	namespace network
	{
		// city
		void CityPushOrder(int iCityID, enum OrderTypes eOrder, int iData, bool bAlt, bool bShift, bool bCtrl);
		void CityPopOrder(int iCity, int iNum);
		void CityTask(int iCity, enum TaskTypes eTask, int iData1, int iData2, bool bOption, bool bAlt, bool bShift, bool bCtrl);

		// global
		void sendExtendedGame();
		void sendPause(int iPauseID = -1);
		void sendGameInfo(const CvWString& szGameName, const CvWString& szAdminPassword);
		void SetDone(bool bDone);

		// player 
		void sendPlayerInfo(PlayerTypes eActivePlayer);
		void sendMPRetire();
		void PlayerAdvancedStartAction(enum AdvancedStartActionTypes eAction, PlayerTypes ePlayer, int iX, int iY, int iData, bool bAdd);
		void PlayerAdvancedStartAction(enum AdvancedStartActionTypes eAction, PlayerTypes ePlayer, Coordinates coord, int iData, bool bAdd);
		void PlayerAction(PlayerTypes ePlayer, enum PlayerActionTypes eAction, int iData1, int iData2, int iData3);

		// plot
		void PlotPing(Coordinates coord);

		// unit
		void UnitJoinGroup(int iUnitID, int iHeadID);
		void SelectionGroupPushMission(int iUnitID, enum MissionTypes eMission, int iData1, int iData2, int iFlags, bool bShift); // calls the selection group the unit is in
		void UnitAutoMission(int iUnitID);
		void UnitCommand(int iUnitID, enum CommandTypes eCommand, int iData1, int iData2, bool bAlt); // calls the unit itself

		// misc
		void sendPopup(PlayerTypes ePlayer, class CvPopupInfo* pInfo);
		void sendPlayerOption(enum PlayerOptionTypes eOption, bool bValue);
		void sendChat(const CvWString& szChatString, enum ChatTargetTypes eTarget);

		// send to EXE_CvPlayer::changeCivic() where the player is the active player
		void sendArray(int* pArray);

		// diplomacy
		void beginDiplomacy(class CvDiploParameters* pDiploParams, enum PlayerTypes ePlayer);
		void endDiplomacy();
		void updateDiplomacyAttitude(bool bForce = false);
		void beginMPDiplomacy(PlayerTypes eWhoTalkingTo, bool bRenegotiate, bool bSimultaneous, IDInfo kTransport);
		void endMPDiplomacy();

		void sendToggleTradeMessage(PlayerTypes eWho, const TradeData& kTradeData, int iOtherWho, bool bAIOffer, bool bSendToAll = false);
		void sendClearTableMessage(PlayerTypes eWhoTradingWith);
		void sendImplementDealMessage(PlayerTypes eOtherWho, CLinkList<TradeData>* pOurList, CLinkList<TradeData>* pTheirList);
		void sendChangeWar(TeamTypes iRivalTeam, bool bWar);
		void sendContactCiv(enum NetContactTypes eContactType, PlayerTypes eWho, int iTransportId);
		void sendOffer();
		void sendDiploEvent(PlayerTypes eWhoTradingWith, enum DiploEventTypes eDiploEvent, int iData1, int iData2);
		void sendRenegotiate(PlayerTypes eWhoTradingWith);
		void sendRenegotiateThisItem(PlayerTypes ePlayer2, enum TradeableItems eItemType, int iData);
		void sendExitTrade();
		void sendKillDeal(int iDealID, bool bFromDiplomacy, enum TeamTypes eEndingTeam);
		void sendDiplomacy(PlayerTypes ePlayer, CvDiploParameters* pParams);
	}
}
