// game.cpp

#include "CvGameCoreDLL.h"
#include "CvGameCoreUtils.h"
#include "CvGameCoreUtils.h"
#include "CvGame.h"
#include "CvGameAI.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvPlayerAI.h"
#include "CvRandom.h"
#include "CvTeamAI.h"
#include "CvGlobals.h"
#include "CvInitCore.h"
#include "CvMapGenerator.h"
#include "CvArtFileMgr.h"
#include "CvDiploParameters.h"
#include "CvReplayMessage.h"
#include "CyArgsList.h"
#include "CvInfos.h"
#include "CvPopupInfo.h"
#include "FProfiler.h"
#include "CvReplayInfo.h"
#include "CyPlot.h"
#include "CvGameTextMgr.h"

// interface uses
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLEngineIFaceBase.h"
#include "CvDLLEventReporterIFaceBase.h"
#include "CvDLLPythonIFaceBase.h"

#include "CvSavegame.h"

// Public Functions...

CvGame::CvGame()
{
	m_sorenRand.setSorenRand();
	m_sorenRand.setSyncedStatus(true);
	m_mapRand.setSyncedStatus(true);

	m_pReplayInfo = NULL;

	// PatchMod: Victorys START
	iVictoriesSet = 0;
	// PatchMod: Victorys END
	reset(NO_HANDICAP, true);
}


CvGame::~CvGame()
{
	uninit();
}

void CvGame::init(HandicapTypes eHandicap)
{
	bool bValid;
	int iStartTurn;
	int iEstimateEndTurn;
	int iI;

	//--------------------------------
	// Init saved data
	reset(eHandicap);

	//--------------------------------
	// Init containers
	m_deals.init();

	m_mapRand.init(GC.getInitCore().getMapRandSeed() % 73637381);
	m_sorenRand.init(GC.getInitCore().getSyncRandSeed() % 52319761);

	//--------------------------------
	// Init non-saved data

	//--------------------------------
	// Init other game data

	// Turn off all MP options if it's a single player game
	if (GC.getInitCore().getType() == GAME_SP_NEW ||
		GC.getInitCore().getType() == GAME_SP_SCENARIO)
	{
		for (iI = 0; iI < NUM_MPOPTION_TYPES; ++iI)
		{
			setMPOption((MultiplayerOptionTypes)iI, false);
		}
	}

	// If this is a hot seat game, simultaneous turns is always off
	if (isHotSeat() || isPbem())
	{
		setMPOption(MPOPTION_SIMULTANEOUS_TURNS, false);
	}
	// If we didn't set a time in the Pitboss, turn timer off
	if (isPitboss() && getPitbossTurnTime() == 0)
	{
		setMPOption(MPOPTION_TURN_TIMER, false);
	}

	if (isMPOption(MPOPTION_SHUFFLE_TEAMS))
	{
		int aiTeams[MAX_PLAYERS];

		int iNumPlayers = 0;
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			if (GC.getInitCore().getSlotStatus((PlayerTypes)i) == SS_TAKEN)
			{
				aiTeams[iNumPlayers] = GC.getInitCore().getTeam((PlayerTypes)i);
				++iNumPlayers;
			}
		}

		for (int i = 0; i < iNumPlayers; i++)
		{
			int j = (getSorenRand().get(iNumPlayers - i, NULL) + i);

			if (i != j)
			{
				int iTemp = aiTeams[i];
				aiTeams[i] = aiTeams[j];
				aiTeams[j] = iTemp;
			}
		}

		iNumPlayers = 0;
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			if (GC.getInitCore().getSlotStatus((PlayerTypes)i) == SS_TAKEN)
			{
				GC.getInitCore().setTeam((PlayerTypes)i, (TeamTypes)aiTeams[iNumPlayers]);
				++iNumPlayers;
			}
		}
	}

	if (isOption(GAMEOPTION_LOCK_MODS))
	{
		if (isGameMultiPlayer())
		{
			setOption(GAMEOPTION_LOCK_MODS, false);
		}
		else
		{
			static const int iPasswordSize = 8;
			char szRandomPassword[iPasswordSize];
			for (int i = 0; i < iPasswordSize-1; i++)
			{
				szRandomPassword[i] = getSorenRandNum(128, NULL);
			}
			szRandomPassword[iPasswordSize-1] = 0;

			GC.getInitCore().setAdminPassword(szRandomPassword);
		}
	}

	if (getGameTurn() == 0)
	{
		iStartTurn = 0;

		for (iI = 0; iI < GC.getGameSpeedInfo(getGameSpeedType()).getNumTurnIncrements(); iI++)
		{
			iStartTurn += GC.getGameSpeedInfo(getGameSpeedType()).getGameTurnInfo(iI).iNumGameTurnsPerIncrement;
		}

		iStartTurn *= GC.getEraInfo(getStartEra()).getStartPercent();
		iStartTurn /= 100;

		setGameTurn(iStartTurn);
	}

	setStartTurn(getGameTurn());

	if (getMaxTurns() == 0)
	{
		iEstimateEndTurn = 0;

		for (iI = 0; iI < GC.getGameSpeedInfo(getGameSpeedType()).getNumTurnIncrements(); iI++)
		{
			iEstimateEndTurn += GC.getGameSpeedInfo(getGameSpeedType()).getGameTurnInfo(iI).iNumGameTurnsPerIncrement;
		}

		setEstimateEndTurn(iEstimateEndTurn);

		if (getEstimateEndTurn() > getGameTurn())
		{
			bValid = false;

			for (iI = 0; iI < GC.getNumVictoryInfos(); iI++)
			{
				if (isVictoryValid((VictoryTypes)iI))
				{
					if (GC.getVictoryInfo((VictoryTypes)iI).isEndScore() || GC.getVictoryInfo((VictoryTypes)iI).isEndEurope())
					{
						bValid = true;
						break;
					}
				}
			}

			if (bValid)
			{
				setMaxTurns(getEstimateEndTurn() - getGameTurn());
			}
		}
	}
	else
	{
		setEstimateEndTurn(getGameTurn() + getMaxTurns());
	}

	setStartYear(GC.getDefineINT("START_YEAR"));

	for (iI = 0; iI < GC.getNumSpecialUnitInfos(); iI++)
	{
		if (GC.getSpecialUnitInfo((SpecialUnitTypes)iI).isValid())
		{
			makeSpecialUnitValid((SpecialUnitTypes)iI);
		}
	}

	for (iI = 0; iI < GC.getNumSpecialBuildingInfos(); iI++)
	{
		if (GC.getSpecialBuildingInfo((SpecialBuildingTypes)iI).isValid())
		{
			makeSpecialBuildingValid((SpecialBuildingTypes)iI);
		}
	}

	for (iI = 0; iI < GC.getNumGoodyInfos(); iI++)  // R&R, ray, Goody Enhancement
	{
		if (GC.getGoodyInfo((GoodyTypes)iI).isUnique())
		{
			setUniqueGoodyValid((GoodyTypes)iI, true);
		}
	}

	AI_init();

	doUpdateCacheOnTurn();
}

// placed in CvDLLButtonPopup.cpp as it needs local enum access there to function correctly
void doNewGameErrorTesting();

//
// Set initial items (units, techs, etc...)
//
void CvGame::setInitialItems(bool bScenario)
{
	PROFILE_FUNC();

	for (PlayerTypes ePlayer = FIRST_PLAYER; ePlayer < NUM_PLAYER_TYPES; ++ePlayer)
	{
		CvPlayerAI& kPlayer = CvPlayerAI::getPlayer(ePlayer);
		if (!kPlayer.isAlive())
			continue;
		kPlayer.AI_updateYieldValues();
	}

	createBarbarianPlayer(); // < JAnimals Mod Start >
	createChurchPlayer(); // R&R, ray, the Church - START

	// R&R, ray, Correct Geographical Placement of Natives - START
	if (!GC.getGameINLINE().isOption(GAMEOPTION_RANDOM_SETTLEMENT_AREAS))
	{
		for (PlayerTypes ePlayer = FIRST_PLAYER; ePlayer < NUM_PLAYER_TYPES; ++ePlayer)
		{
			CvPlayer& kPlayer = CvPlayerAI::getPlayer(ePlayer);
			if (!kPlayer.isAlive())
				continue;
			if (!kPlayer.isNative())
				continue;
			if ((!kPlayer.getCivilizationInfo().isNorthAmericanNative() || GC.getGame().isWBNorthAmericanNative()) &&
				(!kPlayer.getCivilizationInfo().isCentralAmericanNative() || GC.getGame().isWBCentralAmericanNative()) &&
				(!kPlayer.getCivilizationInfo().isSouthAmericanNative() || GC.getGame().isWBSouthAmericanNative()))
				continue;
			//Player does not fit into Scenario
			bool playerShouldBeDeleted = true;

			//Let us see, if we find an alternative Civ that does fit and has not been placed yet
			for (CivilizationTypes eCiv = FIRST_CIVILIZATION; eCiv < NUM_CIVILIZATION_TYPES; ++eCiv)
			{
				CvCivilizationInfo& kCiv = GC.getCivilizationInfo(eCiv);
				//Check if it would fit to Scenario
				if (kCiv.isNorthAmericanNative() && !GC.getGame().isWBNorthAmericanNative())
					continue;
				if (kCiv.isCentralAmericanNative() && !GC.getGame().isWBCentralAmericanNative())
					continue;
				if (kCiv.isSouthAmericanNative() && !GC.getGame().isWBSouthAmericanNative())
					continue;

				//Check if it was already placed
				bool alreadyPlaced = false;

				for (PlayerTypes eOtherPlayer = FIRST_PLAYER; eOtherPlayer < NUM_PLAYER_TYPES; ++eOtherPlayer)
				{
					CvPlayer& kOtherPlayer = CvPlayerAI::getPlayer(eOtherPlayer);
					if (!kOtherPlayer.isAlive())
						continue;
					if (!kOtherPlayer.isNative())
						continue;
					if (kOtherPlayer.getCivilizationType() != eCiv)
						continue;
					alreadyPlaced = true;
				}

				if (alreadyPlaced)
					continue; // So ok, we have found a Civ that fits

				// We now choose the leader and replace the non matching Civ
				for (LeaderHeadTypes eLeader = FIRST_LEADER; eLeader < NUM_LEADER_TYPES; ++eLeader)
				{
					if (kCiv.isLeaders(eLeader)) {
						GC.getInitCore().setColor(ePlayer, static_cast<PlayerColorTypes>(kCiv.getDefaultPlayerColor()));
						GC.getInitCore().setLeader(ePlayer, eLeader);
						GC.getInitCore().setCiv(ePlayer, eCiv);
						GC.getInitCore().setSlotStatus(ePlayer, SS_COMPUTER);
						kPlayer.init(ePlayer);
						//we will not delete the player, because we can do replacement instead
						playerShouldBeDeleted = false;
						break;
					}
				}
				if (!playerShouldBeDeleted)
					break; //do not continue looping through the civs
			}

			//So ok, player does not fit and could not be replaced
			if (playerShouldBeDeleted)
			{
				FAssertMsg(false, "TODO: Remove unfitting player from scenario");
			}
		}
	}
	// R&R, ray, Correct Geographical Placement of Natives - END

	if (!bScenario)
	{
		initFreeState();
	}
	updateOceanDistances();

	if (!bScenario)
	{
		assignStartingPlots();
		//normalizeStartingPlots();
		assignNativeTerritory();
	}
	initFreeUnits();
	initImmigration();

	for (PlayerTypes ePlayer = FIRST_PLAYER; ePlayer < NUM_PLAYER_TYPES; ++ePlayer)
	{
		CvPlayer& kPlayer = CvPlayerAI::getPlayer(ePlayer);
		if (!kPlayer.isAlive())
			continue;
		kPlayer.AI_updateFoundValues();
	}

	doNewGameErrorTesting();
}


void CvGame::regenerateMap()
{
	if (GC.getInitCore().getWBMapScript())
	{
		return;
	}

	setFinalInitialized(false);

	for (PlayerTypes ePlayer = FIRST_PLAYER; ePlayer < NUM_PLAYER_TYPES; ++ePlayer)
	{
		CvPlayerAI::getPlayer(ePlayer).killUnits();
	}

	for (PlayerTypes ePlayer = FIRST_PLAYER; ePlayer < NUM_PLAYER_TYPES; ++ePlayer)
	{
		CvPlayerAI::getPlayer(ePlayer).clearRevolutionEuropeUnits();
	}

	for (PlayerTypes ePlayer = FIRST_PLAYER; ePlayer < NUM_PLAYER_TYPES; ++ePlayer)
	{
		CvPlayerAI::getPlayer(ePlayer).killCities();
	}

	for (PlayerTypes ePlayer = FIRST_PLAYER; ePlayer < NUM_PLAYER_TYPES; ++ePlayer)
	{
		CvPlayerAI::getPlayer(ePlayer).killAllDeals();
	}

	for (PlayerTypes ePlayer = FIRST_PLAYER; ePlayer < NUM_PLAYER_TYPES; ++ePlayer)
	{
		CvPlayerAI::getPlayer(ePlayer).setFoundedFirstCity(false);
		CvPlayerAI::getPlayer(ePlayer).setStartingPlot(NULL, false);
	}

	for (TeamTypes eTeam = FIRST_TEAM; eTeam < NUM_TEAM_TYPES; ++eTeam)
	{
		GC.getMap().setRevealedPlots(eTeam, false);
	}

	gDLL->getEngineIFace()->clearSigns();

	GC.getMap().erasePlots();

	CvMapGenerator::GetInstance().generateRandomMap();
	CvMapGenerator::GetInstance().addGameElements();

	gDLL->getEngineIFace()->RebuildAllPlots();

	gDLL->resetStatistics();

	setInitialItems(false);

	// Super Forts begin *choke* *canal*
	GC.getMap().calculateCanalAndChokePoints();
	// Super Forts end

	initScoreCalculation();
	setFinalInitialized(true);

	GC.getMap().setupGraphical();
	gDLL->getEngineIFace()->SetDirty(GlobeTexture_DIRTY_BIT, true);
	gDLL->getEngineIFace()->SetDirty(MinimapTexture_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(ColoredPlots_DIRTY_BIT, true);

	gDLL->getInterfaceIFace()->setCycleSelectionCounter(1);

	gDLL->getEngineIFace()->AutoSave(true);

	if (NO_PLAYER != getActivePlayer())
	{
		CvPlot* pPlot = GET_PLAYER(getActivePlayer()).getStartingPlot();

		if (NULL != pPlot)
		{
			gDLL->getInterfaceIFace()->lookAt(pPlot->getPoint(), CAMERALOOKAT_NORMAL);
		}
	}
}


void CvGame::uninit()
{
	m_aszDestroyedCities.clear();
	m_aszGreatGeneralBorn.clear();
	m_aszGreatAdmiralBorn.clear(); // R&R, ray, Great Admirals - START
	m_aszShipNamed.clear(); // TAC - Ship Names - Ray - Start

	m_deals.uninit();

	m_mapRand.uninit();
	m_sorenRand.uninit();

	clearReplayMessageMap();
	SAFE_DELETE(m_pReplayInfo);

	m_aPlotExtraYields.clear();
	m_aeInactiveTriggers.clear();
}


// FUNCTION: reset()
// Initializes data members that are serialized.
void CvGame::reset(HandicapTypes eHandicap, bool bConstructorCall)
{
#ifndef CHECK_GLOBAL_CONSTANTS
	GAME_IS_STARTING_UP = true;
#endif

	//--------------------------------
	// Uninit class
	uninit();

	resetSavedData(eHandicap,bConstructorCall);

	m_uiInitialTime = 0;

	m_bDebugMode = false;
	m_bDebugModeCache = false;
	m_bPbemTurnSent = false;
	m_bPlayerOptionsSent = false;

	if (!bConstructorCall)
	{
		AI_reset();
	}
}


void CvGame::initDiplomacy()
{
	PROFILE_FUNC();

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		CvTeam& kTeam = GET_TEAM((TeamTypes)iI);
		kTeam.meet(((TeamTypes)iI), false);

		for (int iJ = 0; iJ < MAX_PLAYERS; ++iJ)
		{
			CvPlayer& kTeamPlayer = GET_PLAYER((PlayerTypes) iJ);
			if (kTeamPlayer.getTeam() == iI)
			{
				PlayerTypes eParent = kTeamPlayer.getParent();
				if(eParent != NO_PLAYER)
				{
					//ray, all Europeans know all Kings and the Church- START
					//kTeam.meet(GET_PLAYER(eParent).getTeam(), false);
					for (int x = 0; x < MAX_PLAYERS; ++x)
					{
						CvPlayer& possibleKing = GET_PLAYER((PlayerTypes) x);
						if (possibleKing.isEurope())
						{
							kTeam.meet(possibleKing.getTeam(), false);
						}
					}
					//ray, all Europeans know all Kings - END
				}
			}
		}
	}

	// Forced peace at the beginning of Advanced starts
	if (isOption(GAMEOPTION_ADVANCED_START))
	{
		CLinkList<TradeData> player1List;
		CLinkList<TradeData> player2List;
		TradeData kTradeData;
		setTradeItem(&kTradeData, TRADE_PEACE_TREATY, 0, NULL);
		player1List.insertAtEnd(kTradeData);
		player2List.insertAtEnd(kTradeData);

		for (int iPlayer1 = 0; iPlayer1 < MAX_PLAYERS; ++iPlayer1)
		{
			CvPlayer& kLoopPlayer1 = GET_PLAYER((PlayerTypes)iPlayer1);

			if (kLoopPlayer1.isAlive())
			{
				for (int iPlayer2 = iPlayer1 + 1; iPlayer2 < MAX_PLAYERS; ++iPlayer2)
				{
					CvPlayer& kLoopPlayer2 = GET_PLAYER((PlayerTypes)iPlayer2);

					if (kLoopPlayer2.isAlive())
					{
						if (GET_TEAM(kLoopPlayer1.getTeam()).canChangeWarPeace(kLoopPlayer2.getTeam()))
						{
							implementDeal((PlayerTypes)iPlayer1, (PlayerTypes)iPlayer2, &player1List, &player2List);
						}
					}
				}
			}
		}
	}



	//ray, fixing bug of player initialization braking traits impacting relations - START
	//code for initialization of Traits has been moved here
	for (int iPlayerX = 0; iPlayerX < MAX_PLAYERS; ++iPlayerX)
	{
		CvPlayer& kLoopPlayer2 = GET_PLAYER((PlayerTypes)iPlayerX);
		if (kLoopPlayer2.isAlive())
		{
			for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
			{
				if (GC.getCivilizationInfo(kLoopPlayer2.getCivilizationType()).hasTrait(iI))
				{
					kLoopPlayer2.processTrait((TraitTypes) iI, 1);
				}

				if (GC.getLeaderHeadInfo(kLoopPlayer2.getLeaderType()).hasTrait(iI))
				{
					kLoopPlayer2.processTrait((TraitTypes) iI, 1);
				}
			}
			for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				kLoopPlayer2.updateExtraYieldThreshold((YieldTypes)iI);
				kLoopPlayer2.updateCityExtraYield((YieldTypes) iI);
			}
		}
	}
	//ray, fixing bug of player initialization braking traits impacting relations - END
}


void CvGame::initFreeState()
{
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			GET_PLAYER((PlayerTypes)iI).initFreeState();
		}
	}
}


void CvGame::initFreeUnits()
{
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{

			if ((GET_PLAYER((PlayerTypes)iI).getNumUnits() == 0) && (GET_PLAYER((PlayerTypes)iI).getNumCities() == 0))
			{
					GET_PLAYER((PlayerTypes)iI).initFreeUnits();
			}
		}
	}
}

void CvGame::initImmigration()
{
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iI);
		if (kPlayer.isAlive() &&  kPlayer.getParent() != NO_PLAYER)
		{
			kPlayer.initImmigration();
		}
	}
}

void CvGame::assignStartingPlots()
{
	FAssertMsg(CvPlayerAI::areStaticsInitialized(), "Players must be initialized before assigning them starting plots");

	PROFILE_FUNC();

	// Try placing players on any predefined starting plots
	for (PlayerTypes ePlayer = FIRST_PLAYER; ePlayer < NUM_PLAYER_TYPES; ++ePlayer)
	{
		CvPlayerAI& kPlayer = CvPlayerAI::getPlayer(ePlayer);
		if (!kPlayer.isAlive())
			continue;
		if (!kPlayer.canFoundCity())
			continue;
		if (kPlayer.getStartingPlot() != NULL)
			continue;

		int iBestValue = 0;
		CvPlot* pBestPlot = NULL;

		for (int iPlot = 0; iPlot < GC.getMap().numPlotsINLINE(); ++iPlot)
		{
			gDLL->callUpdater(); // allow window updates during launch

			CvPlot* pPlot = GC.getMap().plotByIndexINLINE(iPlot);
			if (!pPlot->isStartingPlot())
				continue;

			bool bPlotTaken = false;
			for (PlayerTypes eOtherPlayer = FIRST_PLAYER; eOtherPlayer < NUM_PLAYER_TYPES; ++eOtherPlayer)
			{
				CvPlayer const& kOtherPlayer = CvPlayerAI::getPlayer(eOtherPlayer);
				if (!kOtherPlayer.isAlive())
					continue;
				if (kOtherPlayer.getStartingPlot() == pPlot)
				{
					bPlotTaken = true;
					break;
				}
			}
			if (bPlotTaken)
				continue;

			int iValue = (1 + getSorenRandNum(1000, "Starting Plot"));
			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestPlot = pPlot;
			}
		}

		if (pBestPlot != NULL)
		{
			kPlayer.setStartingPlot(pBestPlot, true);
		}
	}

	if (gDLL->getPythonIFace()->pythonAssignStartingPlots() && !gDLL->getPythonIFace()->pythonUsingDefaultImpl())
	{
		return; // Python override
	}

	std::vector<PlayerTypes> playerOrder;
	if (isTeamGame())
	{
		for (int iPass = 0; iPass < 2 * MAX_PLAYERS; ++iPass)
		{
			bool bMakingProgress = false;
			int iRandOffset = getSorenRandNum(countCivTeamsAlive(), "Team Starting Plot");
			for (int iTeamIndex = 0; iTeamIndex < NUM_TEAM_TYPES - FIRST_TEAM; ++iTeamIndex)
			{
				TeamTypes eTeam = static_cast<TeamTypes>((FIRST_TEAM + iTeamIndex + iRandOffset) % NUM_TEAM_TYPES);
				if (!CvTeamAI::getTeam(eTeam).isAlive())
					continue;

				for (PlayerTypes ePlayer = FIRST_PLAYER; ePlayer < NUM_PLAYER_TYPES; ++ePlayer)
				{
					CvPlayerAI& kPlayer = CvPlayerAI::getPlayer(ePlayer);
					if (!kPlayer.isAlive())
						continue;
					if (!kPlayer.canFoundCity())
						continue;
					if (kPlayer.getTeam() != eTeam)
						continue;
					if (kPlayer.getStartingPlot() != NULL)
						continue;

					CvPlot* pStartingPlot = kPlayer.findStartingPlot();
					if (pStartingPlot != NULL)
					{
						kPlayer.setStartingPlot(pStartingPlot, true);
						playerOrder.push_back(ePlayer);
					}
					bMakingProgress = true;
					break;
				}
			}
			if (!bMakingProgress)
				break;
		}

		//check all players have starting plots
		for (PlayerTypes ePlayer = FIRST_PLAYER; ePlayer < NUM_PLAYER_TYPES; ++ePlayer)
		{
			FAssertMsg(!CvPlayerAI::getPlayer(ePlayer).isAlive() || CvPlayerAI::getPlayer(ePlayer).getStartingPlot() != NULL, "Player has no starting plot");
		}
	}
	else if (isGameMultiPlayer())
	{
		// First place all humans starting with a random player
		int iRandOffset = getSorenRandNum(countCivPlayersAlive(), "Player Starting Plot");
		for (int iPlayerIndex = 0; iPlayerIndex < NUM_PLAYER_TYPES - FIRST_PLAYER; ++iPlayerIndex)
		{
			PlayerTypes ePlayer = static_cast<PlayerTypes>((FIRST_PLAYER + iPlayerIndex + iRandOffset) % NUM_PLAYER_TYPES);
			CvPlayerAI& kPlayer = CvPlayerAI::getPlayer(ePlayer);
			if (!kPlayer.isAlive())
				continue;
			if (!kPlayer.canFoundCity())
				continue;
			if (!kPlayer.isHuman())
				continue;
			if (kPlayer.getStartingPlot() != NULL)
				continue;
			kPlayer.setStartingPlot(kPlayer.findStartingPlot(), true);
			playerOrder.push_back(ePlayer);
		}

		// Then place all the non-humans
		for (PlayerTypes ePlayer = FIRST_PLAYER; ePlayer < NUM_PLAYER_TYPES; ++ePlayer)
		{
			CvPlayerAI& kPlayer = CvPlayerAI::getPlayer(ePlayer);
			if (!kPlayer.isAlive())
				continue;
			if (!kPlayer.canFoundCity())
				continue;
			if (kPlayer.isHuman())
				continue;
			if (kPlayer.getStartingPlot() != NULL)
				continue;
			kPlayer.setStartingPlot(kPlayer.findStartingPlot(), true);
			playerOrder.push_back(ePlayer);
		}
	}
	else
	{
		for (PlayerTypes ePlayer = FIRST_PLAYER; ePlayer < NUM_PLAYER_TYPES; ++ePlayer)
		{
			CvPlayerAI& kPlayer = CvPlayerAI::getPlayer(ePlayer);
			if (!kPlayer.isAlive())
				continue;
			if (!kPlayer.canFoundCity())
				continue;
			if (kPlayer.getStartingPlot() != NULL)
				continue;
			CvPlot* pStartingPlot = kPlayer.findStartingPlot();
			FAssertMsg(pStartingPlot != NULL, "Player has no starting plot");
			kPlayer.setStartingPlot(pStartingPlot, true);
			playerOrder.push_back(ePlayer);
		}

		// R&R, ray, Correct Geographical Placement of Natives - START
		// resorting the starting locations depending on geographical setting
		if (!GC.getGameINLINE().isOption(GAMEOPTION_RANDOM_SETTLEMENT_AREAS))
		{
			for (PlayerTypes eSelfPlayer = FIRST_PLAYER; eSelfPlayer < NUM_PLAYER_TYPES; ++eSelfPlayer)
			{
				CvPlayer& selfPlayer = CvPlayerAI::getPlayer(eSelfPlayer);
				if (!selfPlayer.isAlive())
					continue;
				if (!selfPlayer.isNative())
					continue;
				FAssertMsg(selfPlayer.getStartingPlot() != NULL, "Expected all native players to be placed by now");
				FAssertMsg(&selfPlayer.getStartingPlot()->coord() != NULL, "Expected all native players to be placed on nonnull coords");
				FAssertMsg(!selfPlayer.getStartingPlot()->coord().isInvalidPlotCoord(), "Expected all native players to be placed on valid plot coords");

				bool selfIsNorthAmericanNative = selfPlayer.getCivilizationInfo().isNorthAmericanNative();
				bool selfIsCentralAmericanNative = selfPlayer.getCivilizationInfo().isCentralAmericanNative();
				bool selfIsSouthAmericanNative = selfPlayer.getCivilizationInfo().isSouthAmericanNative();

				for (PlayerTypes eOtherPlayer = FIRST_PLAYER; eOtherPlayer < NUM_PLAYER_TYPES; ++eOtherPlayer)
				{
					if (eSelfPlayer == eOtherPlayer)
						continue;
					CvPlayer& otherPlayer = CvPlayerAI::getPlayer(eOtherPlayer);
					if (!otherPlayer.isAlive())
						continue;
					if (!otherPlayer.isNative())
						continue;
					FAssertMsg(otherPlayer.getStartingPlot() != NULL, "Expected all native players to be placed by now");
					FAssertMsg(!otherPlayer.getStartingPlot()->coord().isInvalidPlotCoord(), "Expected all native players to be placed on valid plot coords");

					bool otherIsNorthAmericanNative = otherPlayer.getCivilizationInfo().isNorthAmericanNative();
					bool otherIsCentralAmericanNative = otherPlayer.getCivilizationInfo().isCentralAmericanNative();
					bool otherIsSouthAmericanNative = otherPlayer.getCivilizationInfo().isSouthAmericanNative();
					//getting Y of Starting Plots and eventually switching
					int selfY = selfPlayer.getStartingPlot()->getY_INLINE();
					int otherY = otherPlayer.getStartingPlot()->getY_INLINE();

					bool shouldSwap = false;
					// self is North, other is Central or South
					if (selfIsNorthAmericanNative && (otherIsCentralAmericanNative || otherIsSouthAmericanNative))
					{
						if (selfY < otherY)
							shouldSwap = true;
					}
					// self is Central, other is North
					else if (selfIsCentralAmericanNative && otherIsNorthAmericanNative)
					{
						if (selfY > otherY)
							shouldSwap = true;
					}
					// self is Central, other is South
					else if (selfIsCentralAmericanNative && otherIsSouthAmericanNative)
					{
						if (selfY < otherY)
							shouldSwap = true;
					}
					//self is South, other is Central or North
					else if (selfIsSouthAmericanNative && (otherIsCentralAmericanNative || otherIsNorthAmericanNative))
					{
						if (selfY > otherY)
							shouldSwap = true;
					}

					if (shouldSwap) {
						CvPlot* nativeTempPlot = selfPlayer.getStartingPlot();
						selfPlayer.setStartingPlot(otherPlayer.getStartingPlot(), true);
						otherPlayer.setStartingPlot(nativeTempPlot, true);
					}
				}
			}
		}
		// R&R, ray, Correct Geographical Placement of Natives - END

		// PatchMod: Random Start Locs START
		// Randomise start locations amongst water starters (Human Europeans)
		int iWaterStartCount = 0;
		for (PlayerTypes ePlayer = FIRST_PLAYER; ePlayer < NUM_PLAYER_TYPES; ++ePlayer)
		{
			CvPlayer& kPlayer = CvPlayerAI::getPlayer(ePlayer);
			if (!kPlayer.isAlive())
				continue;
			if (!kPlayer.getCivilizationInfo().isWaterStart())
				continue;
			iWaterStartCount++;
		}
		for (PlayerTypes ePlayer = FIRST_PLAYER; ePlayer < NUM_PLAYER_TYPES; ++ePlayer)
		{
			CvPlayer& kPlayer = CvPlayerAI::getPlayer(ePlayer);
			if (!kPlayer.isAlive())
				continue;
			if (!kPlayer.isHuman())
				continue;
			if (!kPlayer.getCivilizationInfo().isWaterStart())
				continue;

			int iSwapIndex = getSorenRandNum(iWaterStartCount, "");
			int iIndex = 0;
			for (PlayerTypes eOtherPlayer = FIRST_PLAYER; eOtherPlayer < NUM_PLAYER_TYPES; ++eOtherPlayer)
			{
				CvPlayer& kOtherPlayer = CvPlayerAI::getPlayer(eOtherPlayer);
				if (!kOtherPlayer.isAlive())
					continue;
				if (!kOtherPlayer.getCivilizationInfo().isWaterStart())
					continue;
				if (iIndex++ == iSwapIndex)
				{
					CvPlot* pTempPlot = kPlayer.getStartingPlot();
					kPlayer.setStartingPlot(kOtherPlayer.getStartingPlot(), true);
					kOtherPlayer.setStartingPlot(pTempPlot, true);
					break;
				}
			}
		}
	}
	// PatchMod: Random Start Locs END

	// PatchMod: Random Start Locs START
	// This section of code made Euros appear from top-bottom in order (after removing the human player)
	/*
	//Now iterate over the player starts in the original order and re-place them.
	for (PlayerTypes playerOrderIter = playerOrder.begin(); playerOrderIter != playerOrder.end(); ++playerOrderIter)
	{
		CvPlayerAI::getPlayer((PlayerTypes)(*playerOrderIter)).setStartingPlot(CvPlayerAI::getPlayer((PlayerTypes)(*playerOrderIter)).findStartingPlot(), true);
	}

	//Do it again to even out the water player starts.
	for (PlayerTypes playerOrderIter = playerOrder.begin(); playerOrderIter != playerOrder.end(); ++playerOrderIter)
	{
		if (GC.getCivilizationInfo(CvPlayerAI::getPlayer((PlayerTypes)(*playerOrderIter)).getCivilizationType()).isWaterStart())
		{
			CvPlayerAI::getPlayer((PlayerTypes)(*playerOrderIter)).setStartingPlot(CvPlayerAI::getPlayer((PlayerTypes)(*playerOrderIter)).findStartingPlot(), true);
		}
	}*/
	// PatchMod: Random Start Locs END

	for (PlayerTypes ePlayer = FIRST_PLAYER; ePlayer < NUM_PLAYER_TYPES; ++ePlayer)
	{
		CvPlayerAI const& kPlayer = CvPlayerAI::getPlayer(ePlayer);
		if (!kPlayer.isAlive())
			continue;
		if (!kPlayer.canFoundCity())
			continue;
		FAssertMsg(kPlayer.getStartingPlot() != NULL, "Every player should have been placed");
	}
}

// Swaps starting locations until we have reached the optimal closeness between teams
// (caveat: this isn't quite "optimal" because we could get stuck in local minima, but it's pretty good)

void CvGame::normalizeStartingPlotLocations()
{
	CvPlot* apNewStartPlots[MAX_PLAYERS];
	int* aaiDistances[MAX_PLAYERS];
	int aiStartingLocs[MAX_PLAYERS];

	// Precalculate distances between all starting positions:
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			gDLL->callUpdater();	// allow window to update during launch
			aaiDistances[iI] = new int[iI];
			for (int iJ = 0; iJ < iI; iJ++)
			{
				aaiDistances[iI][iJ] = 0;
			}
			CvPlot *pPlotI = GET_PLAYER((PlayerTypes)iI).getStartingPlot();
			if (pPlotI != NULL)
			{
				for (int iJ = 0; iJ < iI; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iJ).isAlive())
					{
						CvPlot *pPlotJ = GET_PLAYER((PlayerTypes)iJ).getStartingPlot();
						if (pPlotJ != NULL)
						{
							int iDist = GC.getMap().calculatePathDistance(pPlotI, pPlotJ);
							if (iDist == -1)
							{
								// 5x penalty for not being on the same area, or having no passable route
								iDist = 5*plotDistance(pPlotI->getX_INLINE(), pPlotI->getY_INLINE(), pPlotJ->getX_INLINE(), pPlotJ->getY_INLINE());
							}
							aaiDistances[iI][iJ] = iDist;
						}
					}
				}
			}
		}
		else
		{
			aaiDistances[iI] = NULL;
		}
	}

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		aiStartingLocs[iI] = iI; // each player starting in own location
	}

	int iBestScore = getTeamClosenessScore(aaiDistances, aiStartingLocs);
	bool bFoundSwap = true;
	while (bFoundSwap)
	{
		bFoundSwap = false;
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				for (int iJ = 0; iJ < iI; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iJ).isAlive())
					{
						int iTemp = aiStartingLocs[iI];
						aiStartingLocs[iI] = aiStartingLocs[iJ];
						aiStartingLocs[iJ] = iTemp;
						int iScore = getTeamClosenessScore(aaiDistances, aiStartingLocs);
						if (iScore < iBestScore)
						{
							iBestScore = iScore;
							bFoundSwap = true;
						}
						else
						{
							// Swap them back:
							iTemp = aiStartingLocs[iI];
							aiStartingLocs[iI] = aiStartingLocs[iJ];
							aiStartingLocs[iJ] = iTemp;
						}
					}
				}
			}
		}
	}

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		apNewStartPlots[iI] = NULL;
	}

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (aiStartingLocs[iI] != iI)
			{
				apNewStartPlots[iI] = GET_PLAYER((PlayerTypes)aiStartingLocs[iI]).getStartingPlot();
			}
		}
	}

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (apNewStartPlots[iI] != NULL)
			{
				GET_PLAYER((PlayerTypes)iI).setStartingPlot(apNewStartPlots[iI], false);
			}
		}
	}

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		SAFE_DELETE_ARRAY(aaiDistances[iI]);
	}
}

void CvGame::normalizeStartingPlots()
{
	PROFILE_FUNC();

	if (!(GC.getInitCore().getWBMapScript()) || GC.getInitCore().getWBMapNoPlayers())
	{
		if (!gDLL->getPythonIFace()->pythonNormalizeStartingPlotLocations()  || gDLL->getPythonIFace()->pythonUsingDefaultImpl())
		{
			normalizeStartingPlotLocations();
		}
	}

	if (GC.getInitCore().getWBMapScript())
	{
		return;
	}
}

//For each plot, find the nearest starting point.
void CvGame::assignNativeTerritory()
{
	//The owners.
	std::vector<int> territoryId(GC.getMap().numPlotsINLINE(), -1);

	//The native players.
	std::vector<PlayerTypes> natives;

	//The territory centers.
	std::vector<CvPlot*> centers;

	for (int iJ = 0; iJ < MAX_PLAYERS; iJ++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes)iJ;
		CvPlayer& kPlayer = GET_PLAYER(eLoopPlayer);

		if (kPlayer.isNative())
		{
			CvPlot* pStartingPlot = kPlayer.getStartingPlot();
			FAssert(pStartingPlot != NULL);
			natives.push_back(eLoopPlayer);
			centers.push_back(pStartingPlot);
		}
	}


	for(int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);
		if (pLoopPlot->isWater())
		{
			territoryId[iI] = -1;
		}
		else
		{
			int iBestDistance = MAX_INT;
			int iClosest = -1;

			for (int iJ = 0; iJ < (int)centers.size(); iJ++)
			{
				int iDistance = 100 * plotDistance(centers[iJ]->getX_INLINE(), centers[iJ]->getY_INLINE(), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE());
				iDistance += getSorenRandNum(150, "Assign Territories");

				if (iDistance < iBestDistance)
				{
					iBestDistance = iDistance;
					iClosest = iJ;
				}
			}

			if (iClosest != -1)
			{
				territoryId[iI] = iClosest;
			}
		}
	}

	//We now iterate over the players, we find the player with the largest
	//demand for land. We assign that player the largest territory.
	//And then repeat.

	for(int i=0;i<(int)natives.size();i++)
	{
		PlayerTypes eBestPlayer = natives[i];
		for(int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
		{
			if (territoryId[iI] == i)
			{
				CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);
				pLoopPlot->setOwner(eBestPlayer, false);
			}
		}
	}
}

// For each of n teams, let the closeness score for that team be the average distance of an edge between two players on that team.
// This function calculates the closeness score for each team and returns the sum of those n scores.
// The lower the result, the better "clumped" the players' starting locations are.
//
// Note: for the purposes of this function, player i will be assumed to start in the location of player aiStartingLocs[i]

int CvGame::getTeamClosenessScore(int** aaiDistances, int* aiStartingLocs)
{
	int iScore = 0;

	for (int iTeam = 0; iTeam < MAX_TEAMS; iTeam++)
	{
		if (GET_TEAM((TeamTypes)iTeam).isAlive())
		{
			int iTeamTotalDist = 0;
			int iNumEdges = 0;
			for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
			{
				if (GET_PLAYER((PlayerTypes)iPlayer).isAlive())
				{
					if (GET_PLAYER((PlayerTypes)iPlayer).getTeam() == (TeamTypes)iTeam)
					{
						for (int iOtherPlayer = 0; iOtherPlayer < iPlayer; iOtherPlayer++)
						{
							if (GET_PLAYER((PlayerTypes)iOtherPlayer).getTeam() == (TeamTypes)iTeam)
							{
								// Add the edge between these two players that are on the same team
								iNumEdges++;
								int iPlayerStart = aiStartingLocs[iPlayer];
								int iOtherPlayerStart = aiStartingLocs[iOtherPlayer];

								if (iPlayerStart < iOtherPlayerStart) // Make sure that iPlayerStart > iOtherPlayerStart
								{
									int iTemp = iPlayerStart;
									iPlayerStart = iOtherPlayerStart;
									iOtherPlayerStart = iTemp;
								}
								else if (iPlayerStart == iOtherPlayerStart)
								{
									FAssertMsg(false, "Two players are (hypothetically) assigned to the same starting location!");
								}
								iTeamTotalDist += aaiDistances[iPlayerStart][iOtherPlayerStart];
							}
						}
					}
				}
			}

			int iTeamScore;
			if (iNumEdges == 0)
			{
				iTeamScore = 0;
			}
			else
			{
				iTeamScore = iTeamTotalDist/iNumEdges; // the avg distance between team edges is the team score
			}
			iScore += iTeamScore;
		}
	}
	return iScore;
}


void CvGame::update()
{
	MOD_PROFILE("CvGame::update");

	if (!gDLL->GetWorldBuilderMode() || isInAdvancedStart())
	{
		sendPlayerOptions();

		// sample generic event
		CyArgsList pyArgs;
		pyArgs.add(getTurnSlice());
		gDLL->getEventReporterIFace()->genericEvent("gameUpdate", pyArgs.makeFunctionArgs());

		if (getTurnSlice() == 0)
		{
			gDLL->getEngineIFace()->AutoSave(true);
		}

		if (getNumGameTurnActive() == 0)
		{
			if (!isPbem() || !getPbemTurnSent())
			{
				doTurn();
			}
		}

		updateScore();

		updateWar();

		updateMoves();

		updateTimers();

		updateTurnTimer();

		AI_updateAssignWork();

		testAlive();

		if ((getAIAutoPlay() == 0) && !(gDLL->GetAutorun()) && GAMESTATE_EXTENDED != getGameState())
		{
			if (countHumanPlayersAlive() == 0)
			{
				setGameState(GAMESTATE_OVER);
			}
		}

		changeTurnSlice(1);

		if (NO_PLAYER != getActivePlayer() && GET_PLAYER(getActivePlayer()).getAdvancedStartPoints() >= 0 && !gDLL->getInterfaceIFace()->isInAdvancedStart())
		{
			gDLL->getInterfaceIFace()->setInAdvancedStart(true);
			gDLL->getInterfaceIFace()->setWorldBuilder(true);
		}
	}
}


void CvGame::updateScore(bool bForce)
{
	bool abPlayerScored[MAX_PLAYERS];
	bool abTeamScored[MAX_TEAMS];
	int iScore;
	int iBestScore;
	PlayerTypes eBestPlayer;
	TeamTypes eBestTeam;
	int iI, iJ, iK;

	if (!isScoreDirty() && !bForce)
	{
		return;
	}

	setScoreDirty(false);

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		abPlayerScored[iI] = false;
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		iBestScore = MIN_INT;
		eBestPlayer = NO_PLAYER;

		for (iJ = 0; iJ < MAX_PLAYERS; iJ++)
		{
			if (!abPlayerScored[iJ])
			{
				iScore = GET_PLAYER((PlayerTypes)iJ).calculateScore(false);

				if (iScore >= iBestScore)
				{
					iBestScore = iScore;
					eBestPlayer = (PlayerTypes)iJ;
				}
			}
		}

		abPlayerScored[eBestPlayer] = true;

		setRankPlayer(iI, eBestPlayer);
		setPlayerScore(eBestPlayer, iBestScore);
		if (GET_PLAYER(eBestPlayer).isAlive())
		{
			GET_PLAYER(eBestPlayer).updateScoreHistory(getGameTurn(), iBestScore);
		}
	}

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		abTeamScored[iI] = false;
	}

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		iBestScore = MIN_INT;
		eBestTeam = NO_TEAM;

		for (iJ = 0; iJ < MAX_TEAMS; iJ++)
		{
			if (!abTeamScored[iJ])
			{
				iScore = 0;

				for (iK = 0; iK < MAX_PLAYERS; iK++)
				{
					if (GET_PLAYER((PlayerTypes)iK).getTeam() == iJ)
					{
						iScore += getPlayerScore((PlayerTypes)iK);
					}
				}

				if (iScore >= iBestScore)
				{
					iBestScore = iScore;
					eBestTeam = (TeamTypes)iJ;
				}
			}
		}

		abTeamScored[eBestTeam] = true;

		setRankTeam(iI, eBestTeam);
		setTeamRank(eBestTeam, iI);
		setTeamScore(eBestTeam, iBestScore);
	}
}


void CvGame::updateColoredPlots()
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pSelectedCityNode;
	CvCity* pHeadSelectedCity;
	CvCity* pSelectedCity;
	CvCity* pCity;
	CvUnit* pHeadSelectedUnit;
	CvPlot* pRallyPlot;
	CvPlot* pLoopPlot;
	CvPlot* pBestPlot;
	CvPlot* pNextBestPlot;
	long lResult;
	int iRange;
	//int iPathTurns;
	int iDX, iDY;

	gDLL->getEngineIFace()->clearColoredPlots(PLOT_LANDSCAPE_LAYER_BASE);
	gDLL->getEngineIFace()->clearAreaBorderPlots(AREA_BORDER_LAYER_CITY_RADIUS);
	gDLL->getEngineIFace()->clearAreaBorderPlots(AREA_BORDER_LAYER_RANGED);
	gDLL->getEngineIFace()->clearAreaBorderPlots(AREA_BORDER_LAYER_EUROPE);

	if (!gDLL->GetWorldBuilderMode() || gDLL->getInterfaceIFace()->isInAdvancedStart())
	{
		gDLL->getEngineIFace()->clearColoredPlots(PLOT_LANDSCAPE_LAYER_RECOMMENDED_PLOTS);
	}

	if (GC.getUSE_UPDATE_COLORED_PLOTS_CALLBACK())
	{ 
		lResult = 0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "updateColoredPlots", NULL, &lResult);
		if (lResult == 1)
		{
			return;
		}
	}

	// City circles when in Advanced Start
	if (gDLL->getInterfaceIFace()->isInAdvancedStart())
	{
		for (int iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
		{
			CvPlot* pLoopPlot = GC.getMap().plotByIndex(iPlotLoop);

			if (pLoopPlot != NULL)
			{
				if (GET_PLAYER(getActivePlayer()).getAdvancedStartCityCost(true, pLoopPlot) > 0)
				{
					bool bStartingPlot = false;
					for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
					{
						CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayer);
						if (kPlayer.isAlive() && getActiveTeam() == kPlayer.getTeam())
						{
							if (pLoopPlot == kPlayer.getStartingPlot())
							{
								bStartingPlot = true;
								break;
							}
						}
					}
					if (bStartingPlot)
					{
						gDLL->getEngineIFace()->addColoredPlot(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), GC.getColorInfo((ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT")).getColor(), PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_RECOMMENDED_PLOTS);
					}

					if (pLoopPlot->isRevealed(getActiveTeam(), false))
					{
						NiColorA color(GC.getColorInfo((ColorTypes)GC.getInfoTypeForString("COLOR_WHITE")).getColor());
						color.a = 0.4f;
						gDLL->getEngineIFace()->fillAreaBorderPlot(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), color, AREA_BORDER_LAYER_CITY_RADIUS);
					}
				}
			}
		}
	}

	// WTP, ray, Game Option Goodies always display coloured circle - START
	if (isOption(GAMEOPTION_GOODIES_ALWAYS_DISPLAY_COLOURED_CIRCLE))
	{
		for (int iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
		{
			CvPlot* pLoopPlot = GC.getMap().plotByIndex(iPlotLoop);
			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->isGoody() && pLoopPlot->isRevealed(getActiveTeam(), false))
				{
					for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
					{
						CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayer);
						if (kPlayer.isAlive() && getActiveTeam() == kPlayer.getTeam())
						{
							gDLL->getEngineIFace()->addColoredPlot(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), GC.getColorInfo((ColorTypes)GC.getInfoTypeForString("COLOR_GREEN")).getColor(), PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_RECOMMENDED_PLOTS);
						}
					}
				}
			}
		}
	}
	// WTP, ray, Game Option Goodies always display coloured circle - END

	pHeadSelectedCity = gDLL->getInterfaceIFace()->getHeadSelectedCity();
	pHeadSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();

	//fill europe plots
	if (GET_PLAYER(getActivePlayer()).canTradeWithEurope() || GET_PLAYER(getActivePlayer()).canTradeWithPortRoyal()) //RaR, ray, small fix for Port Royal
	{
		PlayerTypes eEuropePlayer = GET_PLAYER(getActivePlayer()).getParent();
		if(eEuropePlayer != NO_PLAYER)
		{
			NiColorA color(GC.getColorInfo((ColorTypes)GC.getPlayerColorInfo(GET_PLAYER(eEuropePlayer).getPlayerColor()).getColorTypePrimary()).getColor());
			color.a = 0.5f;
			for(int i=0;i<GC.getMap().numPlotsINLINE();i++)
			{
				CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(i);
				if(pLoopPlot->isEurope() && pLoopPlot->isRevealed(getActiveTeam(), true))
				{
					gDLL->getEngineIFace()->fillAreaBorderPlot(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), color, AREA_BORDER_LAYER_EUROPE);
				}
			}
		}
	}

	if (pHeadSelectedCity != NULL)
	{
		if (gDLL->getInterfaceIFace()->isCityScreenUp())
		{
			FOREACH(CityPlot)
			{
				pLoopPlot = pHeadSelectedCity->getCityIndexPlot(eLoopCityPlot);
				if (pLoopPlot != NULL)
				{
					CvUnit* pWorkingUnit = pHeadSelectedCity->getUnitWorkingPlot(eLoopCityPlot);
					if (pWorkingUnit != NULL)
					{
						NiColorA color(GC.getColorInfo((ColorTypes)GC.getInfoTypeForString("COLOR_WHITE")).getColor());
						color.a = 0.7f;
						gDLL->getEngineIFace()->addColoredPlot(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), color, PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_BASE);
					}

					if (pLoopPlot->getWorkingCity() != pHeadSelectedCity)
					{
						NiColorA color(GC.getColorInfo((ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT")).getColor());
						color.a = 0.7f;
						gDLL->getEngineIFace()->addColoredPlot(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), color, PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_BASE);
					}
				}
			}
		}
		else
		{
			pSelectedCityNode = gDLL->getInterfaceIFace()->headSelectedCitiesNode();

			while (pSelectedCityNode != NULL)
			{
				pSelectedCity = ::getCity(pSelectedCityNode->m_data);
				pSelectedCityNode = gDLL->getInterfaceIFace()->nextSelectedCitiesNode(pSelectedCityNode);

				if (pSelectedCity != NULL)
				{
					pRallyPlot = pSelectedCity->getRallyPlot();

					if (pRallyPlot != NULL)
					{
						gDLL->getEngineIFace()->addColoredPlot(pRallyPlot->getX_INLINE(), pRallyPlot->getY_INLINE(), GC.getColorInfo((ColorTypes)GC.getInfoTypeForString("COLOR_YELLOW")).getColor(), PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_BASE);
					}
				}
			}
		}
	}
	else if (pHeadSelectedUnit != NULL)
	{
		FAssert(getActivePlayer() != NO_PLAYER);

		if (!(GET_PLAYER(getActivePlayer()).isOption(PLAYEROPTION_NO_UNIT_RECOMMENDATIONS)))
		{
			if (pHeadSelectedUnit->workRate(true) > 0)
			{
				if (pHeadSelectedUnit->plot()->getOwnerINLINE() == pHeadSelectedUnit->getOwnerINLINE())
				{
					pCity = pHeadSelectedUnit->plot()->getWorkingCity();

					if (pCity != NULL)
					{
						if (pHeadSelectedUnit->AI_bestCityBuild(pCity, &pBestPlot))
						{
							FAssert(pBestPlot != NULL);
							gDLL->getEngineIFace()->addColoredPlot(pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), GC.getColorInfo((ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT")).getColor(), PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_RECOMMENDED_PLOTS);

							if (pHeadSelectedUnit->AI_bestCityBuild(pCity, &pNextBestPlot, NULL, pBestPlot))
							{
								FAssert(pNextBestPlot != NULL);
								gDLL->getEngineIFace()->addColoredPlot(pNextBestPlot->getX_INLINE(), pNextBestPlot->getY_INLINE(), GC.getColorInfo((ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT")).getColor(), PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_RECOMMENDED_PLOTS);
							}
						}
					}
				}
			}

			iRange = 4;

			for (iDX = -(iRange); iDX <= iRange; iDX++)
			{
				for (iDY = -(iRange); iDY <= iRange; iDY++)
				{
					pLoopPlot = plotXY(pHeadSelectedUnit->getX_INLINE(), pHeadSelectedUnit->getY_INLINE(), iDX, iDY);

					if (pLoopPlot != NULL)
					{
						if (pLoopPlot->isVisible(pHeadSelectedUnit->getTeam(), false))
						{
							if ((pLoopPlot->area() == pHeadSelectedUnit->area()) || pLoopPlot->isAdjacentToArea(pHeadSelectedUnit->area()))
							{
								if (pHeadSelectedUnit->canFound(pLoopPlot))
								{
									if (pLoopPlot->isBestAdjacentFound(pHeadSelectedUnit->getOwnerINLINE()))
									{
										gDLL->getEngineIFace()->addColoredPlot(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), GC.getColorInfo((ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT")).getColor(), PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_RECOMMENDED_PLOTS);
									}
								}
								if (plotDistance(pHeadSelectedUnit->getX_INLINE(), pHeadSelectedUnit->getY_INLINE(), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE()) <= iRange)
								{
									if (pLoopPlot->isVisible(pHeadSelectedUnit->getTeam(), false))
									{
										// Erik: Ships implicitly have "nobadgoodies". TODO: Evaluate that this holds true for alls ships
										if (pHeadSelectedUnit->isNoBadGoodies() || pHeadSelectedUnit->getDomainType() == DOMAIN_SEA)
										{
											if (pLoopPlot->isRevealedGoody(pHeadSelectedUnit->getTeam()) && (pHeadSelectedUnit->canMoveInto(*pLoopPlot)))
											{
												gDLL->getEngineIFace()->addColoredPlot(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), GC.getColorInfo((ColorTypes)GC.getInfoTypeForString("COLOR_GREEN")).getColor(), PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_RECOMMENDED_PLOTS);
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void CvGame::updateCitySight(bool bIncrement)
{
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			GET_PLAYER((PlayerTypes)iI).updateCitySight(bIncrement);
		}
	}
}


void CvGame::updateSelectionList()
{
	if (GET_PLAYER(getActivePlayer()).isOption(PLAYEROPTION_NO_UNIT_CYCLING))
	{
		return;
	}

	CvUnit* pHeadSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();

	if ((pHeadSelectedUnit == NULL) || !(pHeadSelectedUnit->getGroup()->readyToSelect(true)))
	{
		if ((gDLL->getInterfaceIFace()->getOriginalPlot() == NULL) || !(cyclePlotUnits(gDLL->getInterfaceIFace()->getOriginalPlot(), true, true, gDLL->getInterfaceIFace()->getOriginalPlotCount())))
		{
			if ((gDLL->getInterfaceIFace()->getSelectionPlot() == NULL) || !(cyclePlotUnits(gDLL->getInterfaceIFace()->getSelectionPlot(), true, true)))
			{
				cycleSelectionGroups(true);
			}
		}

		pHeadSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();

		if (pHeadSelectedUnit != NULL)
		{
			if (!(pHeadSelectedUnit->getGroup()->readyToSelect()))
			{
				gDLL->getInterfaceIFace()->clearSelectionList();
			}
		}
	}
}


void CvGame::updateTestEndTurn()
{
	bool bAny;

	bAny = ((gDLL->getInterfaceIFace()->getHeadSelectedUnit() != NULL) && !(GET_PLAYER(getActivePlayer()).isOption(PLAYEROPTION_NO_UNIT_CYCLING)));

	if (GET_PLAYER(getActivePlayer()).isTurnActive())
	{
		if (gDLL->getInterfaceIFace()->isEndTurnMessage())
		{
			if (GET_PLAYER(getActivePlayer()).hasReadyUnit(bAny))
			{
				gDLL->getInterfaceIFace()->setEndTurnMessage(false);
			}
		}
		else
		{
			if (!(GET_PLAYER(getActivePlayer()).hasBusyUnit()) && !(GET_PLAYER(getActivePlayer()).hasReadyUnit(bAny)))
			{
				if (!(gDLL->getInterfaceIFace()->isForcePopup()))
				{
					gDLL->getInterfaceIFace()->setForcePopup(true);
				}
				else
				{
					if (GET_PLAYER(getActivePlayer()).hasAutoUnit())
					{
						if (!(gDLL->shiftKey()))
						{
							gDLL->sendPlayerAction(getActivePlayer(), PLAYER_ACTION_AUTO_MOVES, -1, -1, -1);
						}
					}
					else
					{
						if (GET_PLAYER(getActivePlayer()).isOption(PLAYEROPTION_WAIT_END_TURN) || !(gDLL->getInterfaceIFace()->isHasMovedUnit()) || isHotSeat() || isPbem())
						{
							gDLL->getInterfaceIFace()->setEndTurnMessage(true);
						}
						else
						{
							if (gDLL->getInterfaceIFace()->getEndTurnCounter() > 0)
							{
								gDLL->getInterfaceIFace()->changeEndTurnCounter(-1);
							}
							else
							{
								gDLL->sendPlayerAction(getActivePlayer(), PLAYER_ACTION_TURN_COMPLETE, -1, -1, -1);
								gDLL->getInterfaceIFace()->setEndTurnCounter(3); // XXX
							}
						}
					}
				}
			}
		}
	}
}


void CvGame::testExtendedGame()
{
	int iI;

	if (getGameState() != GAMESTATE_OVER)
	{
		return;
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).isHuman())
			{
				if (GET_PLAYER((PlayerTypes)iI).isExtendedGame())
				{
					setGameState(GAMESTATE_EXTENDED);
					break;
				}
			}
		}
	}
}


CvUnit* CvGame::getPlotUnit(const CvPlot* pPlot, int iIndex)
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode1;
	CLLNode<IDInfo>* pUnitNode2;
	CvUnit* pLoopUnit1;
	CvUnit* pLoopUnit2;
	int iCount;
	int iPass;
	PlayerTypes activePlayer = getActivePlayer();
	TeamTypes activeTeam = getActiveTeam();

	if (pPlot != NULL)
	{
		iCount = 0;

		for (iPass = 0; iPass < 2; iPass++)
		{
			pUnitNode1 = pPlot->headUnitNode();

			while (pUnitNode1 != NULL)
			{
				pLoopUnit1 = pPlot->getUnitNodeLoop(pUnitNode1);

				if (pLoopUnit1 != NULL && !(pLoopUnit1->isInvisible(activeTeam, true)))
				{
					if (!(pLoopUnit1->isCargo()))
					{
						if ((pLoopUnit1->getOwnerINLINE() == activePlayer) == (iPass == 0))
						{
							if (iCount == iIndex)
							{
								return pLoopUnit1;
							}

							iCount++;

							//if ((pLoopUnit1->getTeam() == activeTeam) || isDebugMode())
							{
								if (pLoopUnit1->hasCargo())
								{
									pUnitNode2 = pPlot->headUnitNode();

									while (pUnitNode2 != NULL)
									{
										pLoopUnit2 = pPlot->getUnitNodeLoop(pUnitNode2);

										if (pLoopUnit2 != NULL && !(pLoopUnit2->isInvisible(activeTeam, true)))
										{
											if (pLoopUnit2->getTransportUnit() == pLoopUnit1)
											{
												if (iCount == iIndex)
												{
													return pLoopUnit2;
												}

												iCount++;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return NULL;
}

void CvGame::getPlotUnits(const CvPlot *pPlot, std::vector<CvUnit *> &plotUnits)
{
	PROFILE_FUNC();
	plotUnits.erase(plotUnits.begin(), plotUnits.end());

	CLLNode<IDInfo>* pUnitNode1;
	CLLNode<IDInfo>* pUnitNode2;
	CvUnit* pLoopUnit1;
	CvUnit* pLoopUnit2;
	int iPass;
	PlayerTypes activePlayer = getActivePlayer();
	TeamTypes activeTeam = getActiveTeam();

	if (pPlot != NULL)
	{
		for (iPass = 0; iPass < 2; iPass++)
		{
			pUnitNode1 = pPlot->headUnitNode();

			while (pUnitNode1 != NULL)
			{
				pLoopUnit1 = pPlot->getUnitNodeLoop(pUnitNode1);

				if (pLoopUnit1 != NULL && !(pLoopUnit1->isInvisible(activeTeam, true)))
				{
					if (!(pLoopUnit1->isCargo()))
					{
						if ((pLoopUnit1->getOwnerINLINE() == activePlayer) == (iPass == 0))
						{
							plotUnits.push_back(pLoopUnit1);

							//if ((pLoopUnit1->getTeam() == activeTeam) || isDebugMode())
							{
								if (pLoopUnit1->hasCargo())
								{
									pUnitNode2 = pPlot->headUnitNode();

									while (pUnitNode2 != NULL)
									{
										pLoopUnit2 = pPlot->getUnitNodeLoop(pUnitNode2);

										if (pLoopUnit2 != NULL && !(pLoopUnit2->isInvisible(activeTeam, true)))
										{
											if (pLoopUnit2->getTransportUnit() == pLoopUnit1)
											{
												plotUnits.push_back(pLoopUnit2);
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void CvGame::cycleCities(bool bForward, bool bAdd)
{
	CvCity* pHeadSelectedCity;
	CvCity* pSelectCity;
	CvCity* pLoopCity;
	int iLoop;

	pSelectCity = NULL;

	pHeadSelectedCity = gDLL->getInterfaceIFace()->getHeadSelectedCity();

	if ((pHeadSelectedCity != NULL) && ((pHeadSelectedCity->getTeam() == getActiveTeam()) || isDebugMode()))
	{
		iLoop = pHeadSelectedCity->getIndex();
		iLoop += (bForward ? 1 : -1);

		pLoopCity = GET_PLAYER(pHeadSelectedCity->getOwnerINLINE()).nextCity(&iLoop, !bForward);

		if (pLoopCity == NULL)
		{
			pLoopCity = GET_PLAYER(pHeadSelectedCity->getOwnerINLINE()).firstCity(&iLoop, !bForward);
		}

		if ((pLoopCity != NULL) && (pLoopCity != pHeadSelectedCity))
		{
			pSelectCity = pLoopCity;
		}
	}
	else
	{
		pSelectCity = GET_PLAYER(getActivePlayer()).firstCity(&iLoop, !bForward);
	}

	if (pSelectCity != NULL)
	{
		if (bAdd)
		{
			gDLL->getInterfaceIFace()->clearSelectedCities();
			gDLL->getInterfaceIFace()->addSelectedCity(pSelectCity);
		}
		else
		{
			gDLL->getInterfaceIFace()->selectCity(pSelectCity);
		}
	}
}


void CvGame::cycleSelectionGroups(bool bClear, bool bForward)
{
	CvSelectionGroup* pNextSelectionGroup;
	CvPlot* pPlot;
	CvUnit* pCycleUnit;
	bool bWrap;

	pCycleUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();

	//ray Einheitendurchlauf
	//if (pCycleUnit != NULL)
	if (pCycleUnit != NULL && pCycleUnit->getUnitTravelState() == -1)
	{
		if (pCycleUnit->getOwnerINLINE() != getActivePlayer())
		{
			pCycleUnit = NULL;
		}

		pNextSelectionGroup = GET_PLAYER(getActivePlayer()).cycleSelectionGroups(pCycleUnit, bForward, &bWrap);

		if (bWrap)
		{
			if (GET_PLAYER(getActivePlayer()).hasAutoUnit())
			{
				gDLL->sendPlayerAction(getActivePlayer(), PLAYER_ACTION_AUTO_MOVES, -1, -1, -1);
			}
		}
	}
	else
	{
		pPlot = gDLL->getInterfaceIFace()->getLookAtPlot();
		pNextSelectionGroup = GC.getMap().findSelectionGroup(((pPlot != NULL) ? pPlot->getX() : 0), ((pPlot != NULL) ? pPlot->getY() : 0), getActivePlayer(), true);
	}

	if (pNextSelectionGroup != NULL)
	{
		FAssert(pNextSelectionGroup->getOwnerINLINE() == getActivePlayer());
		gDLL->getInterfaceIFace()->selectUnit(pNextSelectionGroup->getHeadUnit(), bClear);
	}

	if ((pCycleUnit != gDLL->getInterfaceIFace()->getHeadSelectedUnit()) || ((pCycleUnit != NULL) && pCycleUnit->getGroup()->readyToSelect()))
	{
		gDLL->getInterfaceIFace()->lookAtSelectionPlot();
	}
}


// Returns true if unit was cycled...
bool CvGame::cyclePlotUnits(CvPlot* pPlot, bool bForward, bool bAuto, int iCount)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pSelectedUnit;
	CvUnit* pLoopUnit = NULL;

	FAssertMsg(iCount >= -1, "iCount expected to be >= -1");

	if (iCount == -1)
	{
		pUnitNode = pPlot->headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);

			if (pLoopUnit != NULL && pLoopUnit->IsSelected())
			{
				break;
			}

			pUnitNode = pPlot->nextUnitNode(pUnitNode);
		}
	}
	else
	{
		pUnitNode = pPlot->headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);

			if ((iCount - 1) == 0)
			{
				break;
			}

			if (iCount > 0)
			{
				iCount--;
			}

			pUnitNode = pPlot->nextUnitNode(pUnitNode);
		}

		if (pUnitNode == NULL)
		{
			pUnitNode = pPlot->tailUnitNode();

			if (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
			}
		}
	}

	if (pUnitNode != NULL)
	{
		pSelectedUnit = pLoopUnit;

		while (true)
		{
			if (bForward)
			{
				pUnitNode = pPlot->nextUnitNode(pUnitNode);
				if (pUnitNode == NULL)
				{
					pUnitNode = pPlot->headUnitNode();
				}
			}
			else
			{
				pUnitNode = pPlot->prevUnitNode(pUnitNode);
				if (pUnitNode == NULL)
				{
					pUnitNode = pPlot->tailUnitNode();
				}
			}

			pLoopUnit = ::getUnit(pUnitNode->m_data);

			if (iCount == -1)
			{
				if (pLoopUnit == pSelectedUnit)
				{
					break;
				}
			}

			if (pLoopUnit != NULL && pLoopUnit->getOwnerINLINE() == getActivePlayer())
			{
				if (bAuto)
				{
					if (pLoopUnit->getGroup()->readyToSelect())
					{
						gDLL->getInterfaceIFace()->selectUnit(pLoopUnit, true);
						return true;
					}
				}
				else
				{
					gDLL->getInterfaceIFace()->insertIntoSelectionList(pLoopUnit, true, false);
					return true;
				}
			}

			if (pLoopUnit == pSelectedUnit)
			{
				break;
			}
		}
	}

	return false;
}


void CvGame::selectionListMove(CvPlot* pPlot, bool bAlt, bool bShift, bool bCtrl)
{
	CLLNode<IDInfo>* pSelectedUnitNode;
	CvUnit* pHeadSelectedUnit;
	CvUnit* pSelectedUnit;
	TeamTypes eRivalTeam;

	if (pPlot == NULL)
	{
		return;
	}

	CyPlot* pyPlot = new CyPlot(pPlot);
	CyArgsList argsList;
	argsList.add(gDLL->getPythonIFace()->makePythonObject(pyPlot));	// pass in plot class
	argsList.add(bAlt);
	argsList.add(bShift);
	argsList.add(bCtrl);
	long lResult=0;
	gDLL->getPythonIFace()->callFunction(PYGameModule, "cannotSelectionListMove", argsList.makeFunctionArgs(), &lResult);
	delete pyPlot;	// python fxn must not hold on to this pointer
	if (lResult == 1)
	{
		return;
	}

	pHeadSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();

	if ((pHeadSelectedUnit == NULL) || (pHeadSelectedUnit->getOwnerINLINE() != getActivePlayer()))
	{
		return;
	}

	if (bAlt)
	{
		gDLL->getInterfaceIFace()->selectGroup(pHeadSelectedUnit, false, false, true);
	}
	else if (bCtrl)
	{
		gDLL->getInterfaceIFace()->selectGroup(pHeadSelectedUnit, false, true, false);
	}

	pSelectedUnitNode = gDLL->getInterfaceIFace()->headSelectionListNode();

	while (pSelectedUnitNode != NULL)
	{
		pSelectedUnit = ::getUnit(pSelectedUnitNode->m_data);

		eRivalTeam = pSelectedUnit->getDeclareWarUnitMove(pPlot);

		// Erik: No annoying popup for transport units
		// WTP, ray, unless it is a "Troop only" ship
		//if (pSelectedUnit->cargoSpace() == 0 && eRivalTeam != NO_TEAM)
		if (pSelectedUnit != NULL && eRivalTeam != NO_TEAM && (pSelectedUnit->cargoSpace() == 0 || pSelectedUnit->getUnitInfo().isTroopShip()))
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_DECLAREWARMOVE);
			if (NULL != pInfo)
			{
				pInfo->setData1(eRivalTeam);
				pInfo->setData2(pPlot->getX());
				pInfo->setData3(pPlot->getY());
				pInfo->setOption1(bShift);
				if (pPlot->isOwned())
				{
					pInfo->setOption2(pSelectedUnit->canEnterTerritory(pPlot->getOwnerINLINE()));
				}
				else
				{
					pInfo->setOption2(true);
				}
				gDLL->getInterfaceIFace()->addPopup(pInfo);
			}
			return;
		}

		pSelectedUnitNode = gDLL->getInterfaceIFace()->nextSelectionListNode(pSelectedUnitNode);
	}

	selectionListGameNetMessage(GAMEMESSAGE_PUSH_MISSION, MISSION_MOVE_TO, pPlot->getX(), pPlot->getY(), 0, false, bShift);
}


void CvGame::selectionListGameNetMessage(int eMessage, int iData2, int iData3, int iData4, int iFlags, bool bAlt, bool bShift)
{
	CLLNode<IDInfo>* pSelectedUnitNode;
	CvUnit* pHeadSelectedUnit;
	CvUnit* pSelectedUnit;

	CyArgsList argsList;
	argsList.add(eMessage);	// pass in plot class
	argsList.add(iData2);
	argsList.add(iData3);
	argsList.add(iData4);
	argsList.add(iFlags);
	argsList.add(bAlt);
	argsList.add(bShift);
	long lResult=0;
	gDLL->getPythonIFace()->callFunction(PYGameModule, "cannotSelectionListGameNetMessage", argsList.makeFunctionArgs(), &lResult);
	if (lResult == 1)
	{
		return;
	}

	pHeadSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();

	if (pHeadSelectedUnit != NULL)
	{
		if (pHeadSelectedUnit->getOwnerINLINE() == getActivePlayer())
		{
			if (eMessage == GAMEMESSAGE_JOIN_GROUP)
			{
				pSelectedUnitNode = gDLL->getInterfaceIFace()->headSelectionListNode();

				while (pSelectedUnitNode != NULL)
				{
					pSelectedUnit = ::getUnit(pSelectedUnitNode->m_data);
					pSelectedUnitNode = gDLL->getInterfaceIFace()->nextSelectionListNode(pSelectedUnitNode);

					if (pSelectedUnit != NULL)
					{
						if (bShift)
						{
							gDLL->sendJoinGroup(pSelectedUnit->getID(), FFreeList::INVALID_INDEX);
						}
						else
						{
							if (pSelectedUnit == pHeadSelectedUnit)
							{
								gDLL->sendJoinGroup(pSelectedUnit->getID(), FFreeList::INVALID_INDEX);
							}

							gDLL->sendJoinGroup(pSelectedUnit->getID(), pHeadSelectedUnit->getID());
						}
					}
				}

				if (bShift)
				{
					gDLL->getInterfaceIFace()->selectUnit(pHeadSelectedUnit, true);
				}
			}
			else if (eMessage == GAMEMESSAGE_DO_COMMAND)
			{
				pSelectedUnitNode = gDLL->getInterfaceIFace()->headSelectionListNode();

				while (pSelectedUnitNode != NULL)
				{
					pSelectedUnit = ::getUnit(pSelectedUnitNode->m_data);
					pSelectedUnitNode = gDLL->getInterfaceIFace()->nextSelectionListNode(pSelectedUnitNode);

					if (pSelectedUnit != NULL)
					{
						gDLL->sendDoCommand(pSelectedUnit->getID(), ((CommandTypes)iData2), iData3, iData4, bAlt);
					}
				}
			}
			else if ((eMessage == GAMEMESSAGE_PUSH_MISSION) || (eMessage == GAMEMESSAGE_AUTO_MISSION))
			{
				if (!(gDLL->getInterfaceIFace()->mirrorsSelectionGroup()))
				{
					selectionListGameNetMessage(GAMEMESSAGE_JOIN_GROUP);
				}

				if (eMessage == GAMEMESSAGE_PUSH_MISSION)
				{
					gDLL->sendPushMission(pHeadSelectedUnit->getID(), ((MissionTypes)iData2), iData3, iData4, iFlags, bShift);
				}
				else
				{
					gDLL->sendAutoMission(pHeadSelectedUnit->getID());
				}
			}
			else
			{
				FAssert(false);
			}
		}
	}
}


void CvGame::selectedCitiesGameNetMessage(int eMessage, int iData2, int iData3, int iData4, bool bOption, bool bAlt, bool bShift, bool bCtrl)
{
	CLLNode<IDInfo>* pSelectedCityNode;
	CvCity* pSelectedCity;

	pSelectedCityNode = gDLL->getInterfaceIFace()->headSelectedCitiesNode();

	while (pSelectedCityNode != NULL)
	{
		pSelectedCity = ::getCity(pSelectedCityNode->m_data);
		pSelectedCityNode = gDLL->getInterfaceIFace()->nextSelectedCitiesNode(pSelectedCityNode);

		if (pSelectedCity != NULL)
		{
			if (pSelectedCity->getOwnerINLINE() == getActivePlayer())
			{
				switch (eMessage)
				{
				case GAMEMESSAGE_PUSH_ORDER:
					cityPushOrder(pSelectedCity, ((OrderTypes)iData2), iData3, bAlt, bShift, bCtrl);
					break;

				case GAMEMESSAGE_POP_ORDER:
					if (pSelectedCity->getOrderQueueLength() > 1)
					{
						gDLL->sendPopOrder(pSelectedCity->getID(), iData2);
					}
					break;

				case GAMEMESSAGE_DO_TASK:
					gDLL->sendDoTask(pSelectedCity->getID(), ((TaskTypes)iData2), iData3, iData4, bOption, bAlt, bShift, bCtrl);
					break;

				default:
					FAssert(false);
					break;
				}
			}
		}
	}
}


void CvGame::cityPushOrder(CvCity* pCity, OrderTypes eOrder, int iData, bool bAlt, bool bShift, bool bCtrl)
{
	if (pCity->getProduction() > 0)
	{
		gDLL->sendPushOrder(pCity->getID(), eOrder, iData, bAlt, bShift, !bShift);
	}
	else if ((eOrder == ORDER_TRAIN) && (pCity->getProductionUnit() == iData))
	{
		gDLL->sendPushOrder(pCity->getID(), eOrder, iData, bAlt, !bCtrl, bCtrl);
	}
	else
	{
		gDLL->sendPushOrder(pCity->getID(), eOrder, iData, bAlt, bShift, bCtrl);
	}
}


void CvGame::selectUnit(CvUnit* pUnit, bool bClear, bool bToggle, bool bSound)
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pEntityNode;
	CvSelectionGroup* pSelectionGroup;
	bool bSelectGroup;
	bool bGroup;

	if (gDLL->getInterfaceIFace()->getHeadSelectedUnit() == NULL)
	{
		bSelectGroup = true;
	}
	else if (gDLL->getInterfaceIFace()->getHeadSelectedUnit()->getGroup() != pUnit->getGroup())
	{
		bSelectGroup = true;
	}
	else if (pUnit->IsSelected() && !(gDLL->getInterfaceIFace()->mirrorsSelectionGroup()))
	{
		bSelectGroup = !bToggle;
	}
	else
	{
		bSelectGroup = false;
	}

	gDLL->getInterfaceIFace()->clearSelectedCities();

	if (bClear)
	{
		gDLL->getInterfaceIFace()->clearSelectionList();
		bGroup = false;
	}
	else
	{
		bGroup = gDLL->getInterfaceIFace()->mirrorsSelectionGroup();
	}

	if (bSelectGroup)
	{
		pSelectionGroup = pUnit->getGroup();

		gDLL->getInterfaceIFace()->selectionListPreChange();

		pEntityNode = pSelectionGroup->headUnitNode();

		while (pEntityNode != NULL)
		{
			FAssertMsg(::getUnit(pEntityNode->m_data), "null entity in selection group");
			CvUnit *pUnit = pSelectionGroup->getUnitNodeLoop(pEntityNode);
			if (pUnit != NULL)
			{
				gDLL->getInterfaceIFace()->insertIntoSelectionList(pUnit, false, bToggle, bGroup, bSound, true);
			}
		}

		gDLL->getInterfaceIFace()->selectionListPostChange();
	}
	else
	{
		gDLL->getInterfaceIFace()->insertIntoSelectionList(pUnit, false, bToggle, bGroup, bSound);
	}

	gDLL->getInterfaceIFace()->makeSelectionListDirty();
}


void CvGame::selectGroup(CvUnit* pUnit, bool bShift, bool bCtrl, bool bAlt)
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pUnitPlot;
	bool bGroup;

	FAssertMsg(pUnit != NULL, "pUnit == NULL unexpectedly");

	if (bAlt || bCtrl)
	{
		gDLL->getInterfaceIFace()->clearSelectedCities();

		if (!bShift)
		{
			gDLL->getInterfaceIFace()->clearSelectionList();
			bGroup = true;
		}
		else
		{
			bGroup = gDLL->getInterfaceIFace()->mirrorsSelectionGroup();
		}

		pUnitPlot = pUnit->plot();

		pUnitNode = pUnitPlot->headUnitNode();

		gDLL->getInterfaceIFace()->selectionListPreChange();

		while (pUnitNode != NULL)
		{
			pLoopUnit = pUnitPlot->getUnitNodeLoop(pUnitNode);

			if (pLoopUnit != NULL && pLoopUnit->getOwnerINLINE() == getActivePlayer())
			{
				if (pLoopUnit->canMove())
				{
					if (!isMPOption(MPOPTION_SIMULTANEOUS_TURNS) || getTurnSlice() - pLoopUnit->getLastMoveTurn() > GC.getDefineINT("MIN_TIMER_UNIT_DOUBLE_MOVES"))
					{
						if (bAlt || (pLoopUnit->getUnitType() == pUnit->getUnitType()))
						{
							gDLL->getInterfaceIFace()->insertIntoSelectionList(pLoopUnit, false, false, bGroup, false, true);
						}
					}
				}
			}
		}

		gDLL->getInterfaceIFace()->selectionListPostChange();
	}
	else
	{
		gDLL->getInterfaceIFace()->selectUnit(pUnit, !bShift, bShift, true);
	}
}


void CvGame::selectAll(CvPlot* pPlot)
{
	CvUnit* pSelectUnit;
	CvUnit* pCenterUnit;

	pSelectUnit = NULL;

	if (pPlot != NULL)
	{
		pCenterUnit = pPlot->getDebugCenterUnit();

		if ((pCenterUnit != NULL) && (pCenterUnit->getOwnerINLINE() == getActivePlayer()))
		{
			pSelectUnit = pCenterUnit;
		}
	}

	if (pSelectUnit != NULL)
	{
		gDLL->getInterfaceIFace()->selectGroup(pSelectUnit, false, false, true);
	}
}


bool CvGame::canHandleAction(int iAction, CvPlot* pPlot, bool bTestVisible, bool bUseCache)
{
	PROFILE_FUNC();

	CvSelectionGroup* pSelectedGroup;
	CvUnit* pHeadSelectedUnit;
	CvPlot* pMissionPlot;
	bool bShift = gDLL->shiftKey();

	if(GC.getUSE_CANNOT_HANDLE_ACTION_CALLBACK())
	{
		CyPlot* pyPlot = new CyPlot(pPlot);
		CyArgsList argsList;
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pyPlot));	// pass in plot class
		argsList.add(iAction);
		argsList.add(bTestVisible);
		long lResult=0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "cannotHandleAction", argsList.makeFunctionArgs(), &lResult);
		delete pyPlot;	// python fxn must not hold on to this pointer
		if (lResult == 1)
		{
			return false;
		}
	}

	if (GC.getActionInfo(iAction).getControlType() != NO_CONTROL)
	{
		if (canDoControl((ControlTypes)(GC.getActionInfo(iAction).getControlType())))
		{
			return true;
		}
	}

	if (gDLL->getInterfaceIFace()->isCitySelection())
	{
		return false; // XXX hack!
	}

	pHeadSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();

	if (pHeadSelectedUnit != NULL)
	{
		if (pHeadSelectedUnit->getOwnerINLINE() == getActivePlayer())
		{
			if (isMPOption(MPOPTION_SIMULTANEOUS_TURNS) || GET_PLAYER(pHeadSelectedUnit->getOwnerINLINE()).isTurnActive())
			{
				CvSelectionGroup* pSelectedInterfaceList = gDLL->getInterfaceIFace()->getSelectionList();

				if (GC.getActionInfo(iAction).getMissionType() != NO_MISSION)
				{
					if (gDLL->getInterfaceIFace()->mirrorsSelectionGroup())
					{
						pSelectedGroup = pHeadSelectedUnit->getGroup();

						if (pPlot != NULL)
						{
							pMissionPlot = pPlot;
						}
						else if (bShift)
						{
							pMissionPlot = pSelectedGroup->lastMissionPlot();
						}
						else
						{
							pMissionPlot = NULL;
						}

						if ((pMissionPlot == NULL) || !(pMissionPlot->isVisible(pHeadSelectedUnit->getTeam(), false)))
						{
							pMissionPlot = pSelectedGroup->plot();
						}

					}
					else
					{
						pMissionPlot = pSelectedInterfaceList->plot();
					}

					if (pSelectedInterfaceList->canStartMission(GC.getActionInfo(iAction).getMissionType(), GC.getActionInfo(iAction).getMissionData(), -1, pMissionPlot, bTestVisible, bUseCache))
					{
						return true;
					}
				}

				if (GC.getActionInfo(iAction).getCommandType() != NO_COMMAND)
				{
					if (pSelectedInterfaceList->canDoCommand(((CommandTypes)(GC.getActionInfo(iAction).getCommandType())), GC.getActionInfo(iAction).getCommandData(), -1, bTestVisible, bUseCache))
					{
						return true;
					}
				}

				if (gDLL->getInterfaceIFace()->canDoInterfaceMode(((InterfaceModeTypes)GC.getActionInfo(iAction).getInterfaceModeType()), pSelectedInterfaceList))
				{
					return true;
				}
			}
		}
	}

	return false;
}

void CvGame::setupActionCache()
{
	gDLL->getInterfaceIFace()->getSelectionList()->setupActionCache();
}

// WARNING: function is not networked synced
void CvGame::handleAction(int iAction)
{
	CvUnit* pHeadSelectedUnit;
	bool bAlt;
	bool bShift;
	bool bSkip;

	bAlt = gDLL->altKey();
	bShift = gDLL->shiftKey();

	if (!(gDLL->getInterfaceIFace()->canHandleAction(iAction)))
	{
		return;
	}

	if (GC.getActionInfo(iAction).getControlType() != NO_CONTROL)
	{
		if (GC.getActionInfo(iAction).getControlType() == CONTROL_RETIRE)
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CONFIRM_MENU);
			pInfo->setData1(2);
			gDLL->getInterfaceIFace()->addPopup(pInfo, GC.getGameINLINE().getActivePlayer(), true);
		}
		else
		{
			doControl((ControlTypes)(GC.getActionInfo(iAction).getControlType()));
		}
	}

	if (gDLL->getInterfaceIFace()->canDoInterfaceMode((InterfaceModeTypes)GC.getActionInfo(iAction).getInterfaceModeType(), gDLL->getInterfaceIFace()->getSelectionList()))
	{
		pHeadSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();

		if (pHeadSelectedUnit != NULL)
		{
			if (GC.getInterfaceModeInfo((InterfaceModeTypes)GC.getActionInfo(iAction).getInterfaceModeType()).getSelectAll())
			{
				gDLL->getInterfaceIFace()->selectGroup(pHeadSelectedUnit, false, false, true);
			}
			else if (GC.getInterfaceModeInfo((InterfaceModeTypes)GC.getActionInfo(iAction).getInterfaceModeType()).getSelectType())
			{
				gDLL->getInterfaceIFace()->selectGroup(pHeadSelectedUnit, false, true, false);
			}
		}

		gDLL->getInterfaceIFace()->setInterfaceMode((InterfaceModeTypes)GC.getActionInfo(iAction).getInterfaceModeType());
	}

	if (GC.getActionInfo(iAction).getMissionType() != NO_MISSION)
	{
		selectionListGameNetMessage(GAMEMESSAGE_PUSH_MISSION, GC.getActionInfo(iAction).getMissionType(), GC.getActionInfo(iAction).getMissionData(), -1, 0, false, bShift);
	}

	if (GC.getActionInfo(iAction).getCommandType() != NO_COMMAND)
	{
		bSkip = false;

		int iData1 = -1;
		if (GC.getActionInfo(iAction).getCommandType() == COMMAND_LOAD)
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_LOADUNIT);
			gDLL->getInterfaceIFace()->addPopup(pInfo);
			bSkip = true;
		}
		else if (GC.getActionInfo(iAction).getCommandType() == COMMAND_LOAD_CARGO)
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_LOAD_CARGO);
			gDLL->getInterfaceIFace()->addPopup(pInfo);
			bSkip = true;
		}

		if (!bSkip)
		{
			if (GC.getActionInfo(iAction).isConfirmCommand())
			{
				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CONFIRMCOMMAND);
				pInfo->setData1(iAction);
				pInfo->setOption1(bAlt);
				gDLL->getInterfaceIFace()->addPopup(pInfo);
			}
			else
			{
				selectionListGameNetMessage(GAMEMESSAGE_DO_COMMAND, GC.getActionInfo(iAction).getCommandType(), GC.getActionInfo(iAction).getCommandData(), -1, 0, bAlt);
			}
		}
	}
}


bool CvGame::canDoControl(ControlTypes eControl)
{
	CyArgsList argsList;
	argsList.add(eControl);
	long lResult=0;
	gDLL->getPythonIFace()->callFunction(PYGameModule, "cannotDoControl", argsList.makeFunctionArgs(), &lResult);
	if (lResult == 1)
	{
		return false;
	}

	switch (eControl)
	{
	case CONTROL_SELECTYUNITTYPE:
	case CONTROL_SELECTYUNITALL:
	case CONTROL_SELECT_HEALTHY:
	case CONTROL_SELECTCITY:
	case CONTROL_SELECTCAPITAL:
	case CONTROL_NEXTUNIT:
	case CONTROL_PREVUNIT:
	case CONTROL_CYCLEUNIT:
	case CONTROL_CYCLEUNIT_ALT:
	case CONTROL_LASTUNIT:
	case CONTROL_AUTOMOVES:
	case CONTROL_SAVE_GROUP:
	case CONTROL_QUICK_SAVE:
	case CONTROL_QUICK_LOAD:
	case CONTROL_SAVE_NORMAL:
	case CONTROL_ORTHO_CAMERA:
	case CONTROL_CYCLE_CAMERA_FLYING_MODES:
	case CONTROL_ISOMETRIC_CAMERA_LEFT:
	case CONTROL_ISOMETRIC_CAMERA_RIGHT:
	case CONTROL_FLYING_CAMERA:
	case CONTROL_MOUSE_FLYING_CAMERA:
	case CONTROL_TOP_DOWN_CAMERA:
	case CONTROL_TURN_LOG:
	case CONTROL_CHAT_ALL:
	case CONTROL_CHAT_TEAM:
	case CONTROL_GLOBE_VIEW:
		if (!gDLL->getInterfaceIFace()->isFocused())
		{
			return true;
		}
		break;

	case CONTROL_FORCEENDTURN:
		if (!gDLL->getInterfaceIFace()->isFocused() && !gDLL->getInterfaceIFace()->isInAdvancedStart())
		{
			return true;
		}
		break;


	case CONTROL_PING:
	case CONTROL_SIGN:
	case CONTROL_GRID:
	case CONTROL_BARE_MAP:
	case CONTROL_YIELDS:
	case CONTROL_RESOURCE_ALL:
	case CONTROL_UNIT_ICONS:
	case CONTROL_GLOBELAYER:
	case CONTROL_SCORES:
	case CONTROL_FREE_COLONY:
		if (!gDLL->getInterfaceIFace()->isFocusedWidget())
		{
			return true;
		}
		break;

	case CONTROL_OPTIONS_SCREEN:
	case CONTROL_DOMESTIC_SCREEN:
	case CONTROL_VICTORY_SCREEN:
	case CONTROL_CIVILOPEDIA:
	case CONTROL_FOREIGN_SCREEN:
	case CONTROL_CONGRESS_SCREEN:
	case CONTROL_REVOLUTION_SCREEN:
	case CONTROL_EUROPE_SCREEN:
	case CONTROL_MILITARY_SCREEN:
	case CONTROL_FATHER_SCREEN:
	case CONTROL_DIPLOMACY:
	case CONTROL_AFRICA_SCREEN: /*** TRIANGLETRADE 10/15/08 by DPII ***/
	case CONTROL_PORT_ROYAL_SCREEN: // R&R, ray, Port Royal
	case CONTROL_HALL_OF_FAME:
	case CONTROL_INFO:
	case CONTROL_DETAILS:
	case CONTROL_NEXTCITY:
	case CONTROL_PREVCITY:
		return true;
		break;

	case CONTROL_ADMIN_DETAILS:
		return true;
		break;

	case CONTROL_CENTERONSELECTION:
		if (gDLL->getInterfaceIFace()->getLookAtPlot() != gDLL->getInterfaceIFace()->getSelectionPlot())
		{
			return true;
		}
		break;

	case CONTROL_LOAD_GAME:
		if (!(isNetworkMultiPlayer()))
		{
			return true;
		}
		break;

	case CONTROL_RETIRE:
		if ((getGameState() == GAMESTATE_ON) || isGameMultiPlayer())
		{
			if (GET_PLAYER(getActivePlayer()).isAlive())
			{
				if (isPbem() || isHotSeat())
				{
					if (!GET_PLAYER(getActivePlayer()).isEndTurn())
					{
						return true;
					}
				}
				else
				{
					return true;
				}
			}
		}
		break;

	case CONTROL_WORLD_BUILDER:
		if (!(isGameMultiPlayer()) && GC.getInitCore().getAdminPassword().empty() && !gDLL->getInterfaceIFace()->isInAdvancedStart() && !gDLL->getInterfaceIFace()->isCombatFocus())
		{
#ifdef FASSERT_ENABLE
			// added game option to remove the WB as requested on the forum - Nightinggale
			// assert builds can cheat to get the option as asserts tend to be used to explore problems.
			if (!isOption(GAMEOPTION_REMOVE_WORLD_BUILDER) || gDLL->getChtLvl() > 0)
#else
			// without asserts the game should ignore cheat level and will only look at the game option
			if (!isOption(GAMEOPTION_REMOVE_WORLD_BUILDER))
#endif
			{
				return true;
			}
		}
		break;

	case CONTROL_ENDTURN:
	case CONTROL_ENDTURN_ALT:
		if (gDLL->getInterfaceIFace()->isEndTurnMessage() && !gDLL->getInterfaceIFace()->isFocused() && !gDLL->getInterfaceIFace()->isInAdvancedStart())
		{
			return true;
		}
		break;

	case CONTROL_ACHIEVE_SCREEN:
		return false; // handled by CvEventManager.py

	default:
		FAssertMsg(false, "eControl did not match any valid options");
		break;
	}

	return false;
}


// WARNING: function is not networked synced
void CvGame::doControl(ControlTypes eControl)
{
	CvPopupInfo* pInfo;
	CvCity* pCapitalCity;
	CvUnit* pHeadSelectedUnit;
	CvUnit* pUnit;
	CvPlot* pPlot;

	if (!canDoControl(eControl))
	{
		return;
	}

	switch (eControl)
	{
	case CONTROL_CENTERONSELECTION:
		gDLL->getInterfaceIFace()->lookAtSelectionPlot();
		break;

	case CONTROL_SELECTYUNITTYPE:
		pHeadSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();
		if (pHeadSelectedUnit != NULL)
		{
			gDLL->getInterfaceIFace()->selectGroup(pHeadSelectedUnit, false, true, false);
		}
		break;

	case CONTROL_SELECTYUNITALL:
		pHeadSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();
		if (pHeadSelectedUnit != NULL)
		{
			gDLL->getInterfaceIFace()->selectGroup(pHeadSelectedUnit, false, false, true);
		}
		break;

	case CONTROL_SELECT_HEALTHY:
		{
			CvUnit* pGroupHead = NULL;
			pHeadSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();
			gDLL->getInterfaceIFace()->clearSelectionList();
			if (pHeadSelectedUnit != NULL)
			{
				CvPlot* pPlot = pHeadSelectedUnit->plot();
				std::vector<CvUnit *> plotUnits;
				getPlotUnits(pPlot, plotUnits);
				gDLL->getInterfaceIFace()->selectionListPreChange();
				for (int iI = 0; iI < (int) plotUnits.size(); iI++)
				{
					pUnit = plotUnits[iI];

					if (pUnit->getOwnerINLINE() == getActivePlayer())
					{
						if (!isMPOption(MPOPTION_SIMULTANEOUS_TURNS) || getTurnSlice() - pUnit->getLastMoveTurn() > GC.getDefineINT("MIN_TIMER_UNIT_DOUBLE_MOVES"))
						{
							if (pUnit->isHurt())
							{
								if (pGroupHead != NULL)
								{
									gDLL->sendJoinGroup(pUnit->getID(), pGroupHead->getID());
								}
								else
								{
									pGroupHead = pUnit;
								}

								gDLL->getInterfaceIFace()->insertIntoSelectionList(pUnit, false, false, true, true, true);
							}
						}
					}
				}

				gDLL->getInterfaceIFace()->selectionListPostChange();
			}
		}
		break;

	case CONTROL_SELECTCITY:
		if (gDLL->getInterfaceIFace()->isCityScreenUp())
		{
			cycleCities();
		}
		else
		{
			gDLL->getInterfaceIFace()->selectLookAtCity();
		}
		break;

	case CONTROL_SELECTCAPITAL:
		pCapitalCity = GET_PLAYER(getActivePlayer()).getPrimaryCity();
		if (pCapitalCity != NULL)
		{
			gDLL->getInterfaceIFace()->selectCity(pCapitalCity);
		}
		break;

	case CONTROL_NEXTCITY:
		if (gDLL->getInterfaceIFace()->isCitySelection())
		{
			cycleCities(true, !(gDLL->getInterfaceIFace()->isCityScreenUp()));
		}
		else
		{
			gDLL->getInterfaceIFace()->selectLookAtCity(true);
		}
		gDLL->getInterfaceIFace()->lookAtSelectionPlot();
		break;

	case CONTROL_PREVCITY:
		if (gDLL->getInterfaceIFace()->isCitySelection())
		{
			cycleCities(false, !(gDLL->getInterfaceIFace()->isCityScreenUp()));
		}
		else
		{
			gDLL->getInterfaceIFace()->selectLookAtCity(true);
		}
		gDLL->getInterfaceIFace()->lookAtSelectionPlot();
		break;

	case CONTROL_NEXTUNIT:
		pPlot = gDLL->getInterfaceIFace()->getSelectionPlot();
		if (pPlot != NULL)
		{
			cyclePlotUnits(pPlot);
		}
		break;

	case CONTROL_PREVUNIT:
		pPlot = gDLL->getInterfaceIFace()->getSelectionPlot();
		if (pPlot != NULL)
		{
			cyclePlotUnits(pPlot, false);
		}
		break;

	case CONTROL_CYCLEUNIT:
	case CONTROL_CYCLEUNIT_ALT:
		cycleSelectionGroups(true);
		break;

	case CONTROL_LASTUNIT:
		pUnit = gDLL->getInterfaceIFace()->getLastSelectedUnit();

		if (pUnit != NULL)
		{
			gDLL->getInterfaceIFace()->selectUnit(pUnit, true);
			gDLL->getInterfaceIFace()->lookAtSelectionPlot();
		}
		else
		{
			cycleSelectionGroups(true, false);
		}

		gDLL->getInterfaceIFace()->setLastSelectedUnit(NULL);
		break;

	case CONTROL_ENDTURN:
	case CONTROL_ENDTURN_ALT:
		if (gDLL->getInterfaceIFace()->isEndTurnMessage())
		{
			gDLL->sendPlayerAction(getActivePlayer(), PLAYER_ACTION_TURN_COMPLETE, -1, -1, -1);
		}
		break;

	case CONTROL_FORCEENDTURN:
		gDLL->sendPlayerAction(getActivePlayer(), PLAYER_ACTION_TURN_COMPLETE, -1, -1, -1);
		break;

	case CONTROL_AUTOMOVES:
		gDLL->sendPlayerAction(getActivePlayer(), PLAYER_ACTION_AUTO_MOVES, -1, -1, -1);
		break;

	case CONTROL_PING:
		gDLL->getInterfaceIFace()->setInterfaceMode(INTERFACEMODE_PING);
		break;

	case CONTROL_SIGN:
		gDLL->getInterfaceIFace()->setInterfaceMode(INTERFACEMODE_SIGN);
		break;

	case CONTROL_GRID:
		gDLL->getEngineIFace()->SetGridMode(!(gDLL->getEngineIFace()->GetGridMode()));
		break;

	case CONTROL_BARE_MAP:
		gDLL->getInterfaceIFace()->toggleBareMapMode();
		break;

	case CONTROL_YIELDS:
		gDLL->getInterfaceIFace()->toggleYieldVisibleMode();
		break;

	case CONTROL_RESOURCE_ALL:
		gDLL->getEngineIFace()->toggleResourceLayer();
		break;

	case CONTROL_UNIT_ICONS:
		gDLL->getEngineIFace()->toggleUnitLayer();
		break;

	case CONTROL_GLOBELAYER:
		gDLL->getEngineIFace()->toggleGlobeview();
		break;

	case CONTROL_SCORES:
		gDLL->getInterfaceIFace()->toggleScoresVisible();
		break;

	case CONTROL_LOAD_GAME:
		gDLL->LoadGame();
		break;

	case CONTROL_OPTIONS_SCREEN:
		gDLL->getPythonIFace()->callFunction("CvScreensInterface", "showOptionsScreen");
		break;

	case CONTROL_RETIRE:
		if (!isGameMultiPlayer() || countHumanPlayersAlive() == 1)
		{
			setGameState(GAMESTATE_OVER);
			gDLL->getInterfaceIFace()->setDirty(Soundtrack_DIRTY_BIT, true);
		}
		else
		{
			if (isNetworkMultiPlayer())
			{
				gDLL->sendMPRetire();
				gDLL->getInterfaceIFace()->exitingToMainMenu();
			}
			else
			{
				gDLL->handleRetirement(getActivePlayer());
			}
		}
		break;

	case CONTROL_SAVE_GROUP:
		gDLL->SaveGame(SAVEGAME_GROUP);
		break;

	case CONTROL_SAVE_NORMAL:
		gDLL->SaveGame(SAVEGAME_NORMAL);
		break;

	case CONTROL_QUICK_SAVE:
		if (!(isNetworkMultiPlayer()))	// SP only!
		{
			gDLL->QuickSave();
		}
		break;

	case CONTROL_QUICK_LOAD:
		if (!(isNetworkMultiPlayer()))	// SP only!
		{
			gDLL->QuickLoad();
		}
		break;

	case CONTROL_ORTHO_CAMERA:
		gDLL->getEngineIFace()->SetOrthoCamera(!(gDLL->getEngineIFace()->GetOrthoCamera()));
		break;

	case CONTROL_CYCLE_CAMERA_FLYING_MODES:
		gDLL->getEngineIFace()->CycleFlyingMode(1);
		break;

	case CONTROL_ISOMETRIC_CAMERA_LEFT:
		gDLL->getEngineIFace()->MoveBaseTurnLeft();
		break;

	case CONTROL_ISOMETRIC_CAMERA_RIGHT:
		gDLL->getEngineIFace()->MoveBaseTurnRight();
		break;

	case CONTROL_FLYING_CAMERA:
		gDLL->getEngineIFace()->SetFlying(!(gDLL->getEngineIFace()->GetFlying()));
		break;

	case CONTROL_MOUSE_FLYING_CAMERA:
		gDLL->getEngineIFace()->SetMouseFlying(!(gDLL->getEngineIFace()->GetMouseFlying()));
		break;

	case CONTROL_TOP_DOWN_CAMERA:
		gDLL->getEngineIFace()->SetSatelliteMode(!(gDLL->getEngineIFace()->GetSatelliteMode()));
		break;

	case CONTROL_CIVILOPEDIA:
		gDLL->getPythonIFace()->callFunction(PYScreensModule, "pediaShow");
		break;

	case CONTROL_FOREIGN_SCREEN:
		{
			CyArgsList argsList;
			argsList.add(-1);
			gDLL->getPythonIFace()->callFunction(PYScreensModule, "showForeignAdvisorScreen", argsList.makeFunctionArgs());
		}
		break;

	case CONTROL_CONGRESS_SCREEN:
		{
			CyArgsList argsList;
			argsList.add(-1);
			gDLL->getPythonIFace()->callFunction(PYScreensModule, "showCongressAdvisorScreen", argsList.makeFunctionArgs());
		}
		break;
	case CONTROL_REVOLUTION_SCREEN:
		{
			CyArgsList argsList;
			argsList.add(-1);
			gDLL->getPythonIFace()->callFunction(PYScreensModule, "showRevolutionAdvisorScreen", argsList.makeFunctionArgs());
		}
		break;
	case CONTROL_EUROPE_SCREEN:
		{
			// TAC - No Europe screen access in diplomacy mode - koma13 - START
			if (!gDLL->isDiplomacy())
			{
				CyArgsList argsList;
				argsList.add(-1);
				gDLL->getPythonIFace()->callFunction(PYScreensModule, "showEuropeScreen", argsList.makeFunctionArgs());
			}
			// TAC - No Europe screen access in diplomacy mode - koma13 - END
		}
		break;

	/*** TRIANGLETRADE 10/24/08 by DPII ***/
	case CONTROL_AFRICA_SCREEN:
		{
			// TAC - No Africa screen access in diplomacy mode - koma13 - START
			if (!gDLL->isDiplomacy())
			{
				CyArgsList argsList;
				argsList.add(-1);
				gDLL->getPythonIFace()->callFunction(PYScreensModule, "showAfricaScreen", argsList.makeFunctionArgs());
			}
			// TAC - No Africa screen access in diplomacy mode - koma13 - END
		}
		break;
	// R&R, ray, Port Royal
	case CONTROL_PORT_ROYAL_SCREEN:
		{
			// TAC - No Port Royal screen access in diplomacy mode - koma13 - START
			if (!gDLL->isDiplomacy())
			{
				CyArgsList argsList;
				argsList.add(-1);
				gDLL->getPythonIFace()->callFunction(PYScreensModule, "showPortRoyalScreen", argsList.makeFunctionArgs());
			}
			// TAC - No Port Royal screen access in diplomacy mode - koma13 - END
		}
		break;

	case CONTROL_MILITARY_SCREEN:
		{
			CyArgsList argsList;
			argsList.add(-1);
			gDLL->getPythonIFace()->callFunction(PYScreensModule, "showMilitaryAdvisor", argsList.makeFunctionArgs());
		}
		break;
	case CONTROL_FATHER_SCREEN:
		{
			CyArgsList argsList;
			argsList.add(-1);
			gDLL->getPythonIFace()->callFunction(PYScreensModule, "showFoundingFatherScreen", argsList.makeFunctionArgs());
		}
		break;

	case CONTROL_TURN_LOG:
		if (!gDLL->GetWorldBuilderMode() || gDLL->getInterfaceIFace()->isInAdvancedStart())
		{
			gDLL->getInterfaceIFace()->toggleTurnLog();
		}
		break;

	case CONTROL_CHAT_ALL:
		if (!gDLL->GetWorldBuilderMode() || gDLL->getInterfaceIFace()->isInAdvancedStart())
		{
			gDLL->getInterfaceIFace()->showTurnLog(CHATTARGET_ALL);
		}
		break;

	case CONTROL_CHAT_TEAM:
		if (!gDLL->GetWorldBuilderMode() || gDLL->getInterfaceIFace()->isInAdvancedStart())
		{
			gDLL->getInterfaceIFace()->showTurnLog(CHATTARGET_TEAM);
		}
		break;

	case CONTROL_DOMESTIC_SCREEN:
		if (!gDLL->isDiplomacy())
		{
			bool bFound = false;
			const CvPopupQueue& aPopups = GET_PLAYER(getActivePlayer()).getPopups();
			for (CvPopupQueue::const_iterator it = aPopups.begin(); it != aPopups.end() && !bFound; ++it)
			{
				CvPopupInfo* pPopup = *it;
				if (pPopup->getButtonPopupType() == BUTTONPOPUP_PYTHON_SCREEN && pPopup->getText() == L"showDomesticAdvisor")
				{
					bFound = true;
				}
			}

			if (!bFound)
			{
				CyArgsList argsList;
				argsList.add(-1);
				gDLL->getPythonIFace()->callFunction(PYScreensModule, "showDomesticAdvisor", argsList.makeFunctionArgs());
			}
		}
		break;

	case CONTROL_VICTORY_SCREEN:
		{
			CyArgsList argsList;
			argsList.add(-1);
			gDLL->getPythonIFace()->callFunction(PYScreensModule, "showVictoryScreen", argsList.makeFunctionArgs());
		}
		break;

	case CONTROL_INFO:
		{
			CyArgsList args;
			args.add(0);
			args.add(getGameState() == GAMESTATE_ON ? 0 : 1);
			gDLL->getPythonIFace()->callFunction(PYScreensModule, "showInfoScreen", args.makeFunctionArgs());
		}
		break;

	case CONTROL_GLOBE_VIEW:
		gDLL->getEngineIFace()->toggleGlobeview();
		break;

	case CONTROL_DETAILS:
		gDLL->getInterfaceIFace()->showDetails();
		break;

	case CONTROL_ADMIN_DETAILS:
		if (GC.getInitCore().getAdminPassword().empty())
		{
			gDLL->getInterfaceIFace()->showAdminDetails();
		}
		else
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_ADMIN_PASSWORD);
			if (NULL != pInfo)
			{
				pInfo->setData1((int)CONTROL_ADMIN_DETAILS);
				gDLL->getInterfaceIFace()->addPopup(pInfo, NO_PLAYER, true);
			}
		}
		break;

	case CONTROL_HALL_OF_FAME:
		{
			CyArgsList args;
			args.add(true);
			gDLL->getPythonIFace()->callFunction(PYScreensModule, "showHallOfFame", args.makeFunctionArgs());
		}
		break;

	case CONTROL_WORLD_BUILDER:
		if (GC.getInitCore().getAdminPassword().empty())
		{
			gDLL->getInterfaceIFace()->setWorldBuilder(!(gDLL->GetWorldBuilderMode()));
		}
		else
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_ADMIN_PASSWORD);
			if (NULL != pInfo)
			{
				pInfo->setData1((int)CONTROL_WORLD_BUILDER);
				gDLL->getInterfaceIFace()->addPopup(pInfo, NO_PLAYER, true);
			}
		}
		break;

	case CONTROL_FREE_COLONY:
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_FREE_COLONY);
			if (pInfo)
			{
				gDLL->getInterfaceIFace()->addPopup(pInfo);
			}
		}
		break;

	case CONTROL_DIPLOMACY:
		pInfo = new CvPopupInfo(BUTTONPOPUP_DIPLOMACY);
		if (NULL != pInfo)
		{
			gDLL->getInterfaceIFace()->addPopup(pInfo);
		}
		break;

	default:
		FAssertMsg(false, "eControl did not match any valid options");
		break;
	}
}


void CvGame::implementDeal(PlayerTypes eWho, PlayerTypes eOtherWho, CLinkList<TradeData>* pOurList, CLinkList<TradeData>* pTheirList, bool bForce)
{
	CvDeal* pDeal;

	FAssertMsg(eWho != NO_PLAYER, "Who is not assigned a valid value");
	FAssertMsg(eOtherWho != NO_PLAYER, "OtherWho is not assigned a valid value");
	FAssertMsg(eWho != eOtherWho, "eWho is not expected to be equal with eOtherWho");

	pDeal = addDeal();
	pDeal->init(pDeal->getID(), eWho, eOtherWho);
	pDeal->addTrades(pOurList, pTheirList, !bForce);
	if ((pDeal->getLengthFirstTrades() == 0) && (pDeal->getLengthSecondTrades() == 0))
	{
		pDeal->kill(true, NO_TEAM);
	}
}


void CvGame::verifyDeals()
{
	CvDeal* pLoopDeal;
	int iLoop;

	for(pLoopDeal = firstDeal(&iLoop); pLoopDeal != NULL; pLoopDeal = nextDeal(&iLoop))
	{
		pLoopDeal->verify();
	}
}


/* Globeview configuration control:
If bStarsVisible, then there will be stars visible behind the globe when it is on
If bWorldIsRound, then the world will bend into a globe; otherwise, it will show up as a plane  */
void CvGame::getGlobeviewConfigurationParameters(TeamTypes eTeam, bool& bStarsVisible, bool& bWorldIsRound)
{
	if(GET_TEAM(eTeam).isMapCentering())
	{
		bStarsVisible = true;
		bWorldIsRound = true;
	}
	else
	{
		bStarsVisible = false;
		bWorldIsRound = false;
	}
}

int CvGame::getAdjustedPopulationPercent(VictoryTypes eVictory) const
{
	int iPopulation;
	int iBestPopulation;
	int iNextBestPopulation;
	int iI;

	if (GC.getVictoryInfo(eVictory).getPopulationPercentLead() == 0)
	{
		return 0;
	}

	if (getTotalPopulation() == 0)
	{
		return 100;
	}

	iBestPopulation = 0;
	iNextBestPopulation = 0;

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			iPopulation = GET_TEAM((TeamTypes)iI).getTotalPopulation();

			if (iPopulation > iBestPopulation)
			{
				iNextBestPopulation = iBestPopulation;
				iBestPopulation = iPopulation;
			}
			else if (iPopulation > iNextBestPopulation)
			{
				iNextBestPopulation = iPopulation;
			}
		}
	}

	return std::min(100, (((iNextBestPopulation * 100) / getTotalPopulation()) + GC.getVictoryInfo(eVictory).getPopulationPercentLead()));
}


int CvGame::getProductionPerPopulation(HurryTypes eHurry)
{
	if (NO_HURRY == eHurry)
	{
		return 0;
	}
	return (GC.getHurryInfo(eHurry).getProductionPerPopulation() * GC.getGameSpeedInfo(getGameSpeedType()).getGrowthPercent() / 100);
}


int CvGame::getAdjustedLandPercent(VictoryTypes eVictory) const
{
	int iPercent;

	if (GC.getVictoryInfo(eVictory).getLandPercent() == 0)
	{
		return 0;
	}

	iPercent = GC.getVictoryInfo(eVictory).getLandPercent();

	iPercent -= (countCivTeamsEverAlive() * 2);

	return std::max(iPercent, GC.getVictoryInfo(eVictory).getMinLandPercent());
}


int CvGame::countCivPlayersAlive() const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			iCount++;
		}
	}

	return iCount;
}


int CvGame::countCivPlayersEverAlive() const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isEverAlive())
		{
			iCount++;
		}
	}

	return iCount;
}


int CvGame::countCivTeamsAlive() const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			iCount++;
		}
	}

	return iCount;
}


int CvGame::countCivTeamsEverAlive() const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isEverAlive())
		{
			iCount++;
		}
	}

	return iCount;
}


int CvGame::countHumanPlayersAlive() const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).isHuman())
			{
				iCount++;
			}
		}
	}

	return iCount;
}


int CvGame::countHumanPlayersEverAlive() const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isEverAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).isHuman())
			{
				iCount++;
			}
		}
	}

	return iCount;
}

int CvGame::countCivPlayerEuropeanAI()
{
	int iCount = 0;

	for (PlayerTypes ePlayer = FIRST_PLAYER; ePlayer < NUM_PLAYER_TYPES; ePlayer++)
	{
		const CvPlayer& kPlayer = GET_PLAYER(ePlayer);
		if (kPlayer.isEverAlive())
		{
			if (!kPlayer.isHuman() && kPlayer.getCivCategoryTypes() == CIV_CATEGORY_EUROPEAN)
			{
				iCount++;
			}
		}
	}

	return iCount;
}


int CvGame::countTotalCivPower()
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			iCount += GET_PLAYER((PlayerTypes)iI).getPower();
		}
	}

	return iCount;
}

int CvGame::getImprovementUpgradeTime(ImprovementTypes eImprovement) const
{
	int iTime;

	iTime = GC.getImprovementInfo(eImprovement).getUpgradeTime();

	iTime *= GC.getGameSpeedInfo(getGameSpeedType()).getGrowthPercent();
	iTime /= 100;

	iTime *= GC.getEraInfo(getStartEra()).getGrowthPercent();
	iTime /= 100;

	return iTime;
}

EraTypes CvGame::getCurrentEra() const
{
	int iEra;
	int iCount;
	int iI;

	iEra = 0;
	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			iEra += GET_PLAYER((PlayerTypes)iI).getCurrentEra();
			iCount++;
		}
	}

	if (iCount > 0)
	{
		return ((EraTypes)(iEra / iCount));
	}

	return NO_ERA;
}


TeamTypes CvGame::getActiveTeam()
{
	if (getActivePlayer() == NO_PLAYER)
	{
		return NO_TEAM;
	}
	else
	{
		return (TeamTypes)GET_PLAYER(getActivePlayer()).getTeam();
	}
}


CivilizationTypes CvGame::getActiveCivilizationType()
{
	if (getActivePlayer() == NO_PLAYER)
	{
		return NO_CIVILIZATION;
	}
	else
	{
		return (CivilizationTypes)GET_PLAYER(getActivePlayer()).getCivilizationType();
	}
}


unsigned int CvGame::getLastEndTurnMessageSentTime()
{
	return (getInitialTime() + (gDLL->getMillisecsPerTurn() * (getEndTurnMessagesSent() - 2)));
}


bool CvGame::isNetworkMultiPlayer() const
{
	return GC.getInitCore().getMultiplayer();
}


bool CvGame::isGameMultiPlayer() const
{
	return (isNetworkMultiPlayer() || isPbem() || isHotSeat());
}


bool CvGame::isTeamGame() const
{
	FAssert(countCivPlayersAlive() >= countCivTeamsAlive());
	return (countCivPlayersAlive() > countCivTeamsAlive());
}


bool CvGame::isModem()
{
	return gDLL->IsModem();
}
void CvGame::setModem(bool bModem)
{
	if (bModem)
	{
		gDLL->ChangeINIKeyValue("CONFIG", "Bandwidth", "modem");
	}
	else
	{
		gDLL->ChangeINIKeyValue("CONFIG", "Bandwidth", "broadband");
	}

	gDLL->SetModem(bModem);
}


void CvGame::reviveActivePlayer()
{
	if (!(GET_PLAYER(getActivePlayer()).isAlive()))
	{
		setAIAutoPlay(0);

		GC.getInitCore().setSlotStatus(getActivePlayer(), SS_TAKEN);

		// Let Python handle it
		long lResult=0;
		CyArgsList argsList;
		argsList.add(getActivePlayer());

		gDLL->getPythonIFace()->callFunction(PYGameModule, "doReviveActivePlayer", argsList.makeFunctionArgs(), &lResult);
		if (lResult == 1)
		{
			return;
		}

		UnitTypes eUnit = (UnitTypes) 0;
		GET_PLAYER(getActivePlayer()).initUnit(eUnit, GC.getUnitInfo(eUnit).getDefaultProfession(), 0, 0);
	}
}


int CvGame::getNumHumanPlayers()
{
	return GC.getInitCore().getNumHumans();
}


int CvGame::getEndTurnMessagesSent()
{
	return m_iEndTurnMessagesSent;
}


void CvGame::incrementEndTurnMessagesSent()
{
	m_iEndTurnMessagesSent++;
}


int CvGame::getGameTurn()
{
	return GC.getInitCore().getGameTurn();
}


void CvGame::setGameTurn(int iNewValue)
{
	if (getGameTurn() != iNewValue)
	{
		GC.getInitCore().setGameTurn(iNewValue);
		FAssert(getGameTurn() >= 0);

		setScoreDirty(true);

		gDLL->getInterfaceIFace()->setDirty(TurnTimer_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
	}
}


void CvGame::incrementGameTurn()
{
	setGameTurn(getGameTurn() + 1);
}


int CvGame::getTurnYear(int iGameTurn)
{
	// moved the body of this method to Game Core Utils so we have access for other games than the current one (replay screen in HOF)
	return getTurnYearForGame(iGameTurn, getStartYear(), getCalendar(), getGameSpeedType());
}


int CvGame::getGameTurnYear()
{
	return getTurnYear(getGameTurn());
}


int CvGame::getElapsedGameTurns() const
{
	return m_iElapsedGameTurns;
}


void CvGame::incrementElapsedGameTurns()
{
	m_iElapsedGameTurns++;
}

bool CvGame::isMaxTurnsExtended() const
{
	return m_bMaxTurnsExtended;
}

void CvGame::setMaxTurnsExtended(bool bExtended)
{
	m_bMaxTurnsExtended = bExtended;
}

int CvGame::getMaxTurns() const
{
	return GC.getInitCore().getMaxTurns();
}


void CvGame::setMaxTurns(int iNewValue)
{
	GC.getInitCore().setMaxTurns(iNewValue);
	FAssert(getMaxTurns() >= 0);
}


void CvGame::changeMaxTurns(int iChange)
{
	setMaxTurns(getMaxTurns() + iChange);
}

void CvGame::getTurnTimerText(CvWString& szBuffer) const
{
	if (isMPOption(MPOPTION_TURN_TIMER))
	{
		// Get number of turn slices remaining until end-of-turn
		int iTurnSlicesRemaining = getTurnSlicesRemaining();

		if (iTurnSlicesRemaining > 0)
		{
			// Get number of seconds remaining
			int iTurnSecondsRemaining = ((int)floorf((float)(iTurnSlicesRemaining-1) * ((float)gDLL->getMillisecsPerTurn()/1000.0f)) + 1);
			int iTurnMinutesRemaining = (int)(iTurnSecondsRemaining/60);
			iTurnSecondsRemaining = (iTurnSecondsRemaining%60);
			int iTurnHoursRemaining = (int)(iTurnMinutesRemaining/60);
			iTurnMinutesRemaining = (iTurnMinutesRemaining%60);

			// Display time remaining
			CvWString szTempBuffer;
			szTempBuffer.Format(L"%d:%02d:%02d", iTurnHoursRemaining, iTurnMinutesRemaining, iTurnSecondsRemaining);
			szBuffer += szTempBuffer;
		}
		else
		{
			// Flash zeroes
			if (iTurnSlicesRemaining % 2 == 0)
			{
				// Display 0
				szBuffer+=L"0:00";
			}
		}
	}

	if (getGameState() == GAMESTATE_ON)
	{
		if (isOption(GAMEOPTION_ADVANCED_START) && !isOption(GAMEOPTION_ALWAYS_WAR) && getElapsedGameTurns() <= GC.getDefineINT("PEACE_TREATY_LENGTH"))
		{
			if (!szBuffer.empty())
			{
				szBuffer += L" -- ";
			}

			szBuffer += gDLL->getText("TXT_KEY_MISC_ADVANCED_START_PEACE_REMAINING", GC.getDefineINT("PEACE_TREATY_LENGTH") - getElapsedGameTurns());
		}
		else if (getMaxTurns() > 0)
		{
			if ((getElapsedGameTurns() >= (getMaxTurns() - GC.getDefineINT("END_GAME_DISPLAY_WARNING"))) && (getElapsedGameTurns() < getMaxTurns()))
			{
				if (!isEmpty(szBuffer))
				{
					szBuffer += L" -- ";
				}

				if (isMaxTurnsExtended() || GET_PLAYER(getActivePlayer()).isInRevolution())
				{
					szBuffer += gDLL->getText("TXT_KEY_MISC_TURNS_LEFT", (getMaxTurns() - getElapsedGameTurns()));
				}
				else
				{
					szBuffer += gDLL->getText("TXT_KEY_MISC_TURNS_LEFT_TO_DOI", (getMaxTurns() - getElapsedGameTurns()));
				}
			}
		}
	}
}


int CvGame::getMaxCityElimination() const
{
	return GC.getInitCore().getMaxCityElimination();
}


void CvGame::setMaxCityElimination(int iNewValue)
{
	GC.getInitCore().setMaxCityElimination(iNewValue);
	FAssert(getMaxCityElimination() >= 0);
}

int CvGame::getNumAdvancedStartPoints() const
{
	return GC.getInitCore().getNumAdvancedStartPoints();
}


void CvGame::setNumAdvancedStartPoints(int iNewValue)
{
	GC.getInitCore().setNumAdvancedStartPoints(iNewValue);
	FAssert(getNumAdvancedStartPoints() >= 0);
}

int CvGame::getStartTurn() const
{
	return m_iStartTurn;
}


void CvGame::setStartTurn(int iNewValue)
{
	m_iStartTurn = iNewValue;
}


int CvGame::getStartYear() const
{
	return m_iStartYear;
}


void CvGame::setStartYear(int iNewValue)
{
	m_iStartYear = iNewValue;
}


int CvGame::getEstimateEndTurn() const
{
	return m_iEstimateEndTurn;
}


void CvGame::setEstimateEndTurn(int iNewValue)
{
	m_iEstimateEndTurn = iNewValue;
}


int CvGame::getTurnSlice() const
{
	return m_iTurnSlice;
}


int CvGame::getMinutesPlayed() const
{
	return (getTurnSlice() / gDLL->getTurnsPerMinute());
}


void CvGame::setTurnSlice(int iNewValue)
{
	m_iTurnSlice = iNewValue;
}


void CvGame::changeTurnSlice(int iChange)
{
	setTurnSlice(getTurnSlice() + iChange);
}


int CvGame::getCutoffSlice() const
{
	return m_iCutoffSlice;
}


void CvGame::setCutoffSlice(int iNewValue)
{
	m_iCutoffSlice = iNewValue;
}


void CvGame::changeCutoffSlice(int iChange)
{
	setCutoffSlice(getCutoffSlice() + iChange);
}


int CvGame::getTurnSlicesRemaining() const
{
	return (getCutoffSlice() - getTurnSlice());
}


void CvGame::resetTurnTimer()
{
	// We should only use the turn timer if we are in multiplayer
	if (isMPOption(MPOPTION_TURN_TIMER))
	{
		if (getElapsedGameTurns() > 0 || !isOption(GAMEOPTION_ADVANCED_START))
		{
			// Determine how much time we should allow
			int iTurnLen = getMaxTurnLen();
			if (getElapsedGameTurns() == 0 && !isPitboss())
			{
				// Let's allow more time for the initial turn
				TurnTimerTypes eTurnTimer = GC.getInitCore().getTurnTimer();
				FAssertMsg(VARINFO<TurnTimerTypes>::isInRange(eTurnTimer), "Invalid TurnTimer selection in InitCore");
				iTurnLen = (iTurnLen * GC.getTurnTimerInfo(eTurnTimer).getFirstTurnMultiplier());
			}
			// Set the current turn slice to start the 'timer'
			setCutoffSlice(getTurnSlice() + iTurnLen);
		}
	}
}

void CvGame::incrementTurnTimer(int iNumTurnSlices)
{
	if (isMPOption(MPOPTION_TURN_TIMER))
	{
		// If the turn timer has expired, we shouldn't increment it as we've sent our turn complete message
		if (getTurnSlice() <= getCutoffSlice())
		{
			changeCutoffSlice(iNumTurnSlices);
		}
	}
}

TurnTimerTypes CvGame::getTurnTimerType() const
{
	return GC.getInitCore().getTurnTimer();
}


int CvGame::getMaxTurnLen()
{
	if (isPitboss())
	{
		// Use the user provided input
		// Turn time is in hours
		return ( getPitbossTurnTime() * 3600 * 4);
	}
	else
	{
		int iMaxUnits = 0;
		int iMaxCities = 0;

		// Find out who has the most units and who has the most cities
		// Calculate the max turn time based on the max number of units and cities
		for (int i = 0; i < MAX_PLAYERS; ++i)
		{
			if (GET_PLAYER((PlayerTypes)i).isAlive())
			{
				if (GET_PLAYER((PlayerTypes)i).getNumUnits() > iMaxUnits)
				{
					iMaxUnits = GET_PLAYER((PlayerTypes)i).getNumUnits();
				}
				if (GET_PLAYER((PlayerTypes)i).getNumCities() > iMaxCities)
				{
					iMaxCities = GET_PLAYER((PlayerTypes)i).getNumCities();
				}
			}
		}

		// Now return turn len based on base len and unit and city bonuses
		TurnTimerTypes eTurnTimer = GC.getInitCore().getTurnTimer();
		FAssertMsg(VARINFO<TurnTimerTypes>::isInRange(eTurnTimer), "Invalid TurnTimer Selection in InitCore");
		return ( GC.getTurnTimerInfo(eTurnTimer).getBaseTime() +
			    (GC.getTurnTimerInfo(eTurnTimer).getCityBonus()*iMaxCities) +
				(GC.getTurnTimerInfo(eTurnTimer).getUnitBonus()*iMaxUnits) );
	}
}


int CvGame::getTargetScore() const
{
	return GC.getInitCore().getTargetScore();
}

int CvGame::getNumGameTurnActive()
{
	return m_iNumGameTurnActive;
}

int CvGame::countNumHumanGameTurnActive()
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isHuman())
		{
			if (GET_PLAYER((PlayerTypes)iI).isTurnActive())
			{
				iCount++;
			}
		}
	}

	return iCount;
}


void CvGame::changeNumGameTurnActive(int iChange)
{
	m_iNumGameTurnActive = (m_iNumGameTurnActive + iChange);
	FAssert(getNumGameTurnActive() >= 0);
}


int CvGame::getNumCities() const
{
	return m_iNumCities;
}

void CvGame::changeNumCities(int iChange)
{
	m_iNumCities = (m_iNumCities + iChange);
	FAssert(getNumCities() >= 0);
}

int CvGame::getTotalPopulation() const
{
	int iPopulation = 0;
	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) i);
		if (kPlayer.isAlive())
		{
			iPopulation += kPlayer.getTotalPopulation();
		}
	}

	return iPopulation;
}

int CvGame::getMaxPopulation() const
{
	return m_iMaxPopulation;
}


int CvGame::getMaxLand() const
{
	return m_iMaxLand;
}

int CvGame::getMaxFather() const
{
	return m_iMaxFather;
}


int CvGame::getInitPopulation() const
{
	return m_iInitPopulation;
}


int CvGame::getInitLand() const
{
	return m_iInitLand;
}

int CvGame::getInitFather() const
{
	return m_iInitFather;
}

void CvGame::initScoreCalculation()
{
	// initialize score calculation
	int iMaxFood = 0;
	for (int i = 0; i < GC.getMap().numPlotsINLINE(); i++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexINLINE(i);
		if (!pPlot->isWater() || pPlot->isAdjacentToLand())
		{
			iMaxFood += pPlot->calculateBestNatureYield(YIELD_FOOD, NO_TEAM);
		}
	}

	m_iMaxPopulation = iMaxFood / std::max(1, GLOBAL_DEFINE_FOOD_CONSUMPTION_PER_POPULATION);
	m_iMaxLand = GC.getMap().getLandPlots();
	m_iMaxFather = GC.getNumFatherInfos();

	if (NO_ERA != getStartEra())
	{
		int iNumSettlers = GC.getEraInfo(getStartEra()).getStartingUnitMultiplier();
		m_iInitPopulation = iNumSettlers * (GC.getEraInfo(getStartEra()).getFreePopulation() + 1);
		m_iInitLand = iNumSettlers *  NUM_CITY_PLOTS;
	}
	else
	{
		m_iInitPopulation = 0;
		m_iInitLand = 0;
	}
	m_iInitFather = 0;

	for (int iTeam = 0; iTeam < MAX_TEAMS; ++iTeam)
	{
		CvTeam& kTeam = GET_TEAM((TeamTypes) iTeam);
		for (int iPointType = 0; iPointType < GC.getNumFatherPointInfos(); ++iPointType)
		{
			FatherPointTypes ePointType = (FatherPointTypes) iPointType;
			kTeam.changeFatherPoints(ePointType, -kTeam.getFatherPoints(ePointType));
		}
	}
}


int CvGame::getAIAutoPlay()
{
	return m_iAIAutoPlay;
}


void CvGame::setAIAutoPlay(int iNewValue)
{
	int iOldValue;

	iOldValue = getAIAutoPlay();

	if (iOldValue != iNewValue)
	{
		m_iAIAutoPlay = std::max(0, iNewValue);
		// Dale - AoD: AI Autoplay START

		GET_PLAYER(getActivePlayer()).setDisableHuman((getAIAutoPlay() != 0));
		GET_PLAYER(getActivePlayer()).updateHuman();

		/*
		if (getAIAutoPlay() > 0)
		{
			GC.getInitCore().setSlotStatus(getActivePlayer(), SS_COMPUTER);
		}
		else
		{
			GC.getInitCore().setSlotStatus(getActivePlayer(), SS_TAKEN);
		}

		GET_PLAYER(getActivePlayer()).checkPower(true);
		*/
		// Dale - AoD: AI Autoplay END
	}
}


void CvGame::changeAIAutoPlay(int iChange)
{
	setAIAutoPlay(getAIAutoPlay() + iChange);
	// Dale - AoD: AI Autoplay START
	GET_PLAYER(getActivePlayer()).setDisableHuman((getAIAutoPlay() != 0));
	// Dale - AoD: AI Autoplay END
}


unsigned int CvGame::getInitialTime()
{
	return m_uiInitialTime;
}


void CvGame::setInitialTime(unsigned int uiNewValue)
{
	m_uiInitialTime = uiNewValue;
}


bool CvGame::isScoreDirty() const
{
	return m_bScoreDirty;
}


void CvGame::setScoreDirty(bool bNewValue)
{
	m_bScoreDirty = bNewValue;
}

bool CvGame::isDebugMode() const
{
	return m_bDebugModeCache;
}

void CvGame::toggleDebugMode()
{
	m_bDebugMode = ((m_bDebugMode) ? false : true);
	updateDebugModeCache();

	GC.getMap().updateVisibility();
	GC.getMap().updateSymbols();
	GC.getMap().updateMinimapColor();

	gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(MinimapSection_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(UnitInfo_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(GlobeLayer_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(ColoredPlots_DIRTY_BIT, true);

	//gDLL->getEngineIFace()->SetDirty(GlobeTexture_DIRTY_BIT, true);
	gDLL->getEngineIFace()->SetDirty(MinimapTexture_DIRTY_BIT, true);
	gDLL->getEngineIFace()->SetDirty(CultureBorders_DIRTY_BIT, true);


	if (m_bDebugMode)
	{
		gDLL->getEngineIFace()->PushFogOfWar(FOGOFWARMODE_OFF);
	}
	else
	{
		gDLL->getEngineIFace()->PopFogOfWar();
	}
	gDLL->getEngineIFace()->setFogOfWarFromStack();
}

void CvGame::updateDebugModeCache()
{
	if ((gDLL->getChtLvl() > 0) || (gDLL->GetWorldBuilderMode()))
	{
		m_bDebugModeCache = m_bDebugMode;
	}
	else
	{
		m_bDebugModeCache = false;
	}
}

int CvGame::getPitbossTurnTime() const
{
	return GC.getInitCore().getPitbossTurnTime();
}

void CvGame::setPitbossTurnTime(int iHours)
{
	GC.getInitCore().setPitbossTurnTime(iHours);
}


bool CvGame::isHotSeat() const
{
	return (GC.getInitCore().getHotseat());
}

bool CvGame::isPbem() const
{
	return (GC.getInitCore().getPbem());
}



bool CvGame::isPitboss() const
{
	return (GC.getInitCore().getPitboss());
}

bool CvGame::isSimultaneousTeamTurns() const
{
	if (!isNetworkMultiPlayer())
	{
		return false;
	}

	if (isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
	{
		return false;
	}

	return true;
}

bool CvGame::isFinalInitialized() const
{
	return m_bFinalInitialized;
}


void CvGame::setFinalInitialized(bool bNewValue)
{
	PROFILE_FUNC();

	int iI;

	if (isFinalInitialized() != bNewValue)
	{
		m_bFinalInitialized = bNewValue;

		if (isFinalInitialized())
		{
			for (iI = 0; iI < MAX_TEAMS; iI++)
			{
				if (GET_TEAM((TeamTypes)iI).isAlive())
				{
					GET_TEAM((TeamTypes)iI).AI_updateAreaStragies();
				}
			}
			/// PlotGroup - start - Nightinggale
			postLoadGameFixes();
			/// PlotGroup - end - Nightinggale
		}
	}
}


bool CvGame::getPbemTurnSent() const
{
	return m_bPbemTurnSent;
}


void CvGame::setPbemTurnSent(bool bNewValue)
{
	m_bPbemTurnSent = bNewValue;
}


bool CvGame::getHotPbemBetweenTurns() const
{
	return m_bHotPbemBetweenTurns;
}


void CvGame::setHotPbemBetweenTurns(bool bNewValue)
{
	m_bHotPbemBetweenTurns = bNewValue;
}


bool CvGame::isPlayerOptionsSent() const
{
	return m_bPlayerOptionsSent;
}


void CvGame::sendPlayerOptions(bool bForce)
{
	int iI;

	if (getActivePlayer() == NO_PLAYER)
	{
		return;
	}

	if (!isPlayerOptionsSent() || bForce)
	{
		m_bPlayerOptionsSent = true;

		for (iI = 0; iI < NUM_PLAYEROPTION_TYPES; iI++)
		{
			gDLL->sendPlayerOption(((PlayerOptionTypes)iI), gDLL->getPlayerOption((PlayerOptionTypes)iI));
		}
	}
}


PlayerTypes CvGame::getActivePlayer() const
{
	return GC.getInitCore().getActivePlayer();
}


void CvGame::setActivePlayer(PlayerTypes eNewValue, bool bForceHotSeat)
{
	PlayerTypes eOldActivePlayer = getActivePlayer();
	if (eOldActivePlayer != eNewValue)
	{
		int iActiveNetId = ((NO_PLAYER != eOldActivePlayer) ? GET_PLAYER(eOldActivePlayer).getNetID() : -1);
		GC.getInitCore().setActivePlayer(eNewValue);

		if (GET_PLAYER(eNewValue).isHuman() && (isHotSeat() || isPbem() || bForceHotSeat))
		{
			gDLL->getPassword(eNewValue);
			setHotPbemBetweenTurns(false);
			gDLL->getInterfaceIFace()->dirtyTurnLog(eNewValue);

			if (NO_PLAYER != eOldActivePlayer)
			{
				int iInactiveNetId = GET_PLAYER(eNewValue).getNetID();
				GET_PLAYER(eNewValue).setNetID(iActiveNetId);
				GET_PLAYER(eOldActivePlayer).setNetID(iInactiveNetId);
			}

			GET_PLAYER(eNewValue).showMissedMessages();

			if (countHumanPlayersAlive() == 1 && isPbem())
			{
				// Nobody else left alive
				GC.getInitCore().setType(GAME_HOTSEAT_NEW);
			}

			if (isHotSeat() || bForceHotSeat)
			{
				sendPlayerOptions(true);
			}
		}

		if (GC.IsGraphicsInitialized())
		{
			GC.getMap().updateFog();
			GC.getMap().updateVisibility();
			GC.getMap().updateSymbols();
			GC.getMap().updateMinimapColor();

			updateUnitEnemyGlow();

			gDLL->getInterfaceIFace()->setEndTurnMessage(false);

			gDLL->getInterfaceIFace()->clearSelectedCities();
			gDLL->getInterfaceIFace()->clearSelectionList();
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(MinimapSection_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(UnitInfo_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(Flag_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(GlobeLayer_DIRTY_BIT, true);

			gDLL->getEngineIFace()->SetDirty(CultureBorders_DIRTY_BIT, true);
		}
	}
}

void CvGame::updateUnitEnemyGlow()
{
	//update unit enemy glow
	for(int i=0;i<MAX_PLAYERS;i++)
	{
		PlayerTypes playerType = (PlayerTypes) i;
		int iLoop;
		for(CvUnit *pLoopUnit = GET_PLAYER(playerType).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(playerType).nextUnit(&iLoop))
		{
			//update glow
			gDLL->getEntityIFace()->updateEnemyGlow(pLoopUnit->getUnitEntity());
		}
	}
}

HandicapTypes CvGame::getHandicapType() const
{
	return m_eHandicap;
}

void CvGame::setHandicapType(HandicapTypes eHandicap)
{
	m_eHandicap = eHandicap;
}

PlayerTypes CvGame::getPausePlayer()
{
	return m_ePausePlayer;
}


bool CvGame::isPaused()
{
	return (getPausePlayer() != NO_PLAYER);
}


void CvGame::setPausePlayer(PlayerTypes eNewValue)
{
	m_ePausePlayer = eNewValue;
}


int CvGame::getBestLandUnitCombat()
{
	return m_iBestLandUnitCombat;
}


void CvGame::setBestLandUnitCombat(int iNewValue)
{
	if (getBestLandUnitCombat() != iNewValue)
	{
		m_iBestLandUnitCombat = iNewValue;

		gDLL->getInterfaceIFace()->setDirty(UnitInfo_DIRTY_BIT, true);
	}
}


TeamTypes CvGame::getWinner() const
{
	return m_eWinner;
}


VictoryTypes CvGame::getVictory() const
{
	return m_eVictory;
}


void CvGame::setWinner(TeamTypes eNewWinner, VictoryTypes eNewVictory)
{
	CvWString szBuffer;

	if ((getWinner() != eNewWinner) || (getVictory() != eNewVictory))
	{
		m_eWinner = eNewWinner;
		m_eVictory = eNewVictory;

		if (getVictory() != NO_VICTORY)
		{
			if (GC.getVictoryInfo(getVictory()).isRevolution())
			{
				for (int iTeam = 0; iTeam < MAX_TEAMS; ++iTeam)
				{
					if (GET_TEAM((TeamTypes) iTeam).isParentOf(getWinner()))
					{
						GET_TEAM(getWinner()).makePeace((TeamTypes) iTeam);
					}
				}

				for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
				{
					if (GET_PLAYER((PlayerTypes) iPlayer).getTeam() == getWinner())
					{
						GET_PLAYER((PlayerTypes) iPlayer).setTaxRate(0);
					}
				}
			}

			if (getWinner() != NO_TEAM)
			{
				szBuffer = gDLL->getText("TXT_KEY_GAME_WON", GET_TEAM(getWinner()).getName().GetCString(), GC.getVictoryInfo(getVictory()).getTextKeyWide());
				addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, GET_TEAM(getWinner()).getLeaderID(), szBuffer, -1, -1, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
			}

			if ((getAIAutoPlay() > 0) || gDLL->GetAutorun())
			{
				setGameState(GAMESTATE_EXTENDED);
			}
			else
			{
				setGameState(GAMESTATE_OVER);
			}
		}

		gDLL->getInterfaceIFace()->setDirty(Center_DIRTY_BIT, true);

		gDLL->getEventReporterIFace()->victory(eNewWinner, eNewVictory);

		gDLL->getInterfaceIFace()->setDirty(Soundtrack_DIRTY_BIT, true);
	}
}


GameStateTypes CvGame::getGameState() const
{
	return m_eGameState;
}


void CvGame::setGameState(GameStateTypes eNewValue)
{
	CvPopupInfo* pInfo;
	int iI;

	if (getGameState() != eNewValue)
	{
		m_eGameState = eNewValue;

		if (eNewValue == GAMESTATE_OVER)
		{
			gDLL->getEventReporterIFace()->gameEnd();

			showEndGameSequence();

			for (iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isHuman())
				{
					// One more turn?
					pInfo = new CvPopupInfo(BUTTONPOPUP_EXTENDED_GAME);
					if (NULL != pInfo)
					{
						GET_PLAYER((PlayerTypes)iI).addPopup(pInfo);
					}
				}
			}
		}

		gDLL->getInterfaceIFace()->setDirty(Cursor_DIRTY_BIT, true);
	}
}


GameSpeedTypes CvGame::getGameSpeedType() const
{
	return GC.getInitCore().getGameSpeed();
}

int CvGame::getCultureLevelThreshold(CultureLevelTypes eCultureLevel, PlayerTypes ePlayer) const
{
	int iThreshold;

	iThreshold = GC.getCultureLevelInfo(eCultureLevel).getThreshold() * GC.getGameSpeedInfo(getGameSpeedType()).getGrowthPercent() / 100;

	if (ePlayer != NO_PLAYER)
	{
		CvPlayer &kPlayer = GET_PLAYER(ePlayer);
		for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); ++iTrait)
		{
			TraitTypes eTrait = (TraitTypes)iTrait;

			if (kPlayer.hasTrait(eTrait))
			{
				iThreshold *= std::max(0, (100 + GC.getTraitInfo(eTrait).getCultureLevelModifier()));
				iThreshold /= 100;
			}
		}
	}

	return iThreshold;
}

int CvGame::getCargoYieldCapacity() const
{
	return (GC.getDefineINT("CITY_YIELD_CAPACITY") * GC.getGameSpeedInfo(getGameSpeedType()).getStoragePercent() / 100);
}

EraTypes CvGame::getStartEra() const
{
	return GC.getInitCore().getEra();
}


CalendarTypes CvGame::getCalendar() const
{
	return GC.getInitCore().getCalendar();
}


PlayerTypes CvGame::getRankPlayer(int iRank)
{
	FAssertMsg(iRank >= 0, "iRank is expected to be non-negative (invalid Rank)");
	FAssertMsg(iRank < MAX_PLAYERS, "iRank is expected to be within maximum bounds (invalid Rank)");
	return (PlayerTypes)m_em_iRankPlayer.get((PlayerTypes)iRank);
}


void CvGame::setRankPlayer(int iRank, PlayerTypes ePlayer)
{
	FAssertMsg(iRank >= 0, "iRank is expected to be non-negative (invalid Rank)");
	FAssertMsg(iRank < MAX_PLAYERS, "iRank is expected to be within maximum bounds (invalid Rank)");

	if (getRankPlayer(iRank) != ePlayer)
	{
		m_em_iRankPlayer.set((PlayerTypes)iRank, ePlayer);

		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	}
}

int CvGame::getPlayerScore(PlayerTypes ePlayer)
{
	FAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	return m_em_iPlayerScore.get(ePlayer);
}


void CvGame::setPlayerScore(PlayerTypes ePlayer, int iScore)
{
	FAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	if (getPlayerScore(ePlayer) != iScore)
	{
		m_em_iPlayerScore.set(ePlayer, iScore);
		FAssert(getPlayerScore(ePlayer) >= 0);

		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	}
}


TeamTypes CvGame::getRankTeam(int iRank)
{
	FAssertMsg(iRank >= 0, "iRank is expected to be non-negative (invalid Rank)");
	FAssertMsg(iRank < MAX_TEAMS, "iRank is expected to be within maximum bounds (invalid Index)");
	return (TeamTypes)m_em_iRankTeam.get((TeamTypes)iRank);
}


void CvGame::setRankTeam(int iRank, TeamTypes eTeam)
{
	FAssertMsg(iRank >= 0, "iRank is expected to be non-negative (invalid Rank)");
	FAssertMsg(iRank < MAX_TEAMS, "iRank is expected to be within maximum bounds (invalid Index)");

	if (getRankTeam(iRank) != eTeam)
	{
		m_em_iRankTeam.set((TeamTypes)iRank, eTeam);

		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	}
}


int CvGame::getTeamRank(TeamTypes eTeam)
{
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	return m_em_iTeamRank.get(eTeam);
}


void CvGame::setTeamRank(TeamTypes eTeam, int iRank)
{
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	m_em_iTeamRank.set(eTeam, iRank);
	FAssert(getTeamRank(eTeam) >= 0);
}


int CvGame::getTeamScore(TeamTypes eTeam) const
{
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	return m_em_iTeamScore.get(eTeam);
}


void CvGame::setTeamScore(TeamTypes eTeam, int iScore)
{
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	m_em_iTeamScore.set(eTeam, iScore);
	FAssert(getTeamScore(eTeam) >= 0);
}


bool CvGame::isOption(GameOptionTypes eIndex) const
{
	return GC.getInitCore().getOption(eIndex);
}


void CvGame::setOption(GameOptionTypes eIndex, bool bEnabled)
{
	GC.getInitCore().setOption(eIndex, bEnabled);
}


bool CvGame::isMPOption(MultiplayerOptionTypes eIndex) const
{
	return GC.getInitCore().getMPOption(eIndex);
}


void CvGame::setMPOption(MultiplayerOptionTypes eIndex, bool bEnabled)
{
	GC.getInitCore().setMPOption(eIndex, bEnabled);
}


bool CvGame::isForcedControl(ForceControlTypes eIndex) const
{
	return GC.getInitCore().getForceControl(eIndex);
}


void CvGame::setForceControl(ForceControlTypes eIndex, bool bEnabled)
{
	GC.getInitCore().setForceControl(eIndex, bEnabled);
}


int CvGame::getUnitCreatedCount(UnitTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_iUnitCreatedCount.get(eIndex);
}


void CvGame::incrementUnitCreatedCount(UnitTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_em_iUnitCreatedCount.add(eIndex, 1);
}


int CvGame::getUnitClassCreatedCount(UnitClassTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_iUnitClassCreatedCount.get(eIndex);
}

void CvGame::incrementUnitClassCreatedCount(UnitClassTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_em_iUnitClassCreatedCount.add(eIndex, 1);
}

int CvGame::getBuildingClassCreatedCount(BuildingClassTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_iBuildingClassCreatedCount.get(eIndex);
}

void CvGame::incrementBuildingClassCreatedCount(BuildingClassTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_em_iBuildingClassCreatedCount.add(eIndex, 1);
}

bool CvGame::isVictoryValid(VictoryTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumVictoryInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return GC.getInitCore().getVictory(eIndex);
}

bool CvGame::isSpecialUnitValid(SpecialUnitTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumSpecialUnitInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_bSpecialUnitValid.get(eIndex);
}


void CvGame::makeSpecialUnitValid(SpecialUnitTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumSpecialUnitInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_em_bSpecialUnitValid.set(eIndex, true);
}


bool CvGame::isSpecialBuildingValid(SpecialBuildingTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumSpecialBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_bSpecialBuildingValid.get(eIndex);
}


void CvGame::makeSpecialBuildingValid(SpecialBuildingTypes eIndex, bool bAnnounce)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumSpecialBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (!m_em_bSpecialBuildingValid.get(eIndex))
	{
		m_em_bSpecialBuildingValid.set(eIndex, true);


		if (bAnnounce)
		{
			CvWString szBuffer = gDLL->getText("TXT_KEY_SPECIAL_BUILDING_VALID", GC.getSpecialBuildingInfo(eIndex).getTextKeyWide());

			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					gDLL->UI().addPlayerMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BUILDING_COMPLETED", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
				}
			}
		}
	}
}


// R&R, ray, Goody Enhancement - START
bool CvGame::isUniqueGoodyValid(GoodyTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumGoodyInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_bUniqueGoodyValid.get(eIndex);
}


void CvGame::setUniqueGoodyValid(GoodyTypes eIndex, bool bValid)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumGoodyInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_em_bUniqueGoodyValid.set(eIndex, bValid);
}
// R&R, ray, Goody Enhancement - END

bool CvGame::isInAdvancedStart() const
{
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		if ((GET_PLAYER((PlayerTypes)iPlayer).getAdvancedStartPoints() >= 0) && GET_PLAYER((PlayerTypes)iPlayer).isHuman())
		{
			return true;
		}
	}

	return false;
}

std::string CvGame::getScriptData() const
{
	return m_szScriptData;
}


void CvGame::setScriptData(std::string szNewValue)
{
	m_szScriptData = szNewValue;
}

const CvWString & CvGame::getName()
{
	return GC.getInitCore().getGameName();
}


void CvGame::setName(char const* szName)
{
	GC.getInitCore().setGameName(szName);
}


bool CvGame::isDestroyedCityName(const CvWString& szName) const
{
	std::vector<CvWString>::const_iterator it;

	for (it = m_aszDestroyedCities.begin(); it != m_aszDestroyedCities.end(); ++it)
	{
		if (gDLL->getText(*it) == gDLL->getText(szName))
		{
			return true;
		}
	}

	return false;
}

void CvGame::addDestroyedCityName(const CvWString& szName)
{
	m_aszDestroyedCities.push_back(szName);
}

bool CvGame::isGreatGeneralBorn(CvWString& szName) const
{
	std::vector<CvWString>::const_iterator it;

	for (it = m_aszGreatGeneralBorn.begin(); it != m_aszGreatGeneralBorn.end(); ++it)
	{
		if (*it == szName)
		{
			return true;
		}
	}

	return false;
}

void CvGame::addGreatGeneralBornName(const CvWString& szName)
{
	m_aszGreatGeneralBorn.push_back(szName);
}

// R&R, ray, Great Admirals - START
bool CvGame::isGreatAdmiralBorn(CvWString& szName) const
{
	std::vector<CvWString>::const_iterator it;

	for (it = m_aszGreatAdmiralBorn.begin(); it != m_aszGreatAdmiralBorn.end(); ++it)
	{
		if (*it == szName)
		{
			return true;
		}
	}

	return false;
}

void CvGame::addGreatAdmiralBornName(const CvWString& szName)
{
	m_aszGreatAdmiralBorn.push_back(szName);
}
// R&R, ray, Great Admirals - END


// < JAnimals Mod Start >
PlayerTypes CvGame::getBarbarianPlayer()
{
	return m_eBarbarianPlayer;
}

bool CvGame::hasBarbarianPlayer()
{
	return (getBarbarianPlayer() != NO_PLAYER);
}

void CvGame::setBarbarianPlayer(PlayerTypes eNewValue)
{
	m_eBarbarianPlayer = eNewValue;
}

bool CvGame::isBarbarianPlayer(PlayerTypes ePlayer)
{
	return (getBarbarianPlayer() == ePlayer);
}

PlayerTypes CvGame::getNextPlayerType() const
{
	PlayerTypes eNewPlayer = NO_PLAYER;
	int iPlayer;
	// Try to find a player who's never been in the game before
	//for (iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	// Start with the last possible player and work back to 0
	for (iPlayer = MAX_PLAYERS - 1; iPlayer >= 0; --iPlayer)
	{
		if (!GET_PLAYER((PlayerTypes) iPlayer).isEverAlive())

		{
			eNewPlayer = (PlayerTypes) iPlayer;
			break;

		}
	}

	if (eNewPlayer == NO_PLAYER)
	{
		// Try to recycle a dead player
		for (iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			if (!GET_PLAYER((PlayerTypes) iPlayer).isAlive())
			{
				eNewPlayer = (PlayerTypes) iPlayer;
				break;
			}
		}
	}
	return eNewPlayer;
	//return ((PlayerTypes) MAX_PLAYERS);
}
// < JAnimals Mod End >


// R&R, ray, the Church - START
PlayerTypes CvGame::getChurchPlayer()
{
	return m_eChurchPlayer;
}

bool CvGame::hasChurchPlayer()
{
	return (getChurchPlayer() != NO_PLAYER);
}

void CvGame::setChurchPlayer(PlayerTypes eNewValue)
{
	m_eChurchPlayer = eNewValue;
}

bool CvGame::isChurchPlayer(PlayerTypes ePlayer)
{
	return (getChurchPlayer() == ePlayer);
}
// R&R, ray, the Church - END



// TAC - Ship Names - Ray - Start
bool CvGame::isShipNamed(CvWString& szName) const
{
	std::vector<CvWString>::const_iterator it;

	for (it = m_aszShipNamed.begin(); it != m_aszShipNamed.end(); ++it)
	{
		if (*it == szName)
		{
			return true;
		}
	}

	return false;
}

void CvGame::addShipName(const CvWString& szName)
{
	m_aszShipNamed.push_back(szName);
}
// TAC - Ship Names - Ray - END


// Protected Functions...

void CvGame::doTurn()
{
	PROFILE_FUNC();

	int iLoopPlayer;
	int iI;

	// END OF TURN
	gDLL->getEventReporterIFace()->beginGameTurn( getGameTurn() );

	doUpdateCacheOnTurn();

	updateScore();

	doDeals();

	// TAC - AI More Immigrants - koma13 - START
	if (GC.getGameINLINE().getGameTurn() > 1)
	{
		int iChance = GC.getHandicapInfo(getHandicapType()).getAIImmigration() * 100;
		iChance /= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();

		if (NO_WORLDSIZE != GC.getMap().getWorldSize())
		{
			iChance *= GC.getWorldInfo(GC.getMap().getWorldSize()).getAIImmigrationModifier();
			iChance /= 100;
		}

		if (GC.getGameINLINE().getSorenRandNum(100, "AI") < iChance)
		{
			for (PlayerTypes ePlayer = FIRST_PLAYER; ePlayer < NUM_PLAYER_TYPES; ++ePlayer)
			{
				CvPlayer &kPlayer = GET_PLAYER(ePlayer);
				if (kPlayer.isAlive())
				{
					if (!kPlayer.isHuman() && !kPlayer.isEurope() && !kPlayer.isNative())
					{
						kPlayer.doAIImmigrant(1);
					}
				}
			}
		}
	}
	// TAC - AI More Immigrants - koma13 - END
	for (TeamTypes eTeam = FIRST_TEAM; eTeam < NUM_TEAM_TYPES; ++eTeam)
	{
		CvTeam &kTeam = GET_TEAM(eTeam);
		if (kTeam.isAlive())
		{
			kTeam.doTurn();
		}
	}

	GC.getMap().doTurn();

	// < JAnimals Mod Start >
	createAnimalsLand();
	createAnimalsSea();
	// < JAnimals Mod End >
	gDLL->getInterfaceIFace()->setEndTurnMessage(false);
	gDLL->getInterfaceIFace()->setHasMovedUnit(false);

	if (getAIAutoPlay() > 0)
	{
		changeAIAutoPlay(-1);

		if (getAIAutoPlay() == 0)
		{
			reviveActivePlayer();
		}
	}

	gDLL->getEventReporterIFace()->endGameTurn(getGameTurn());

	incrementGameTurn();
	incrementElapsedGameTurns();

	if (isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
	{
		std::vector<int> aiShuffle(MAX_PLAYERS);
		getSorenRand().shuffleSequence(aiShuffle, NULL);

		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			iLoopPlayer = aiShuffle[iI];

			if (GET_PLAYER((PlayerTypes)iLoopPlayer).isAlive())
			{
				GET_PLAYER((PlayerTypes)iLoopPlayer).setTurnActive(true);
			}
		}
	}
	else if (isSimultaneousTeamTurns())
	{
		for (iI = 0; iI < MAX_TEAMS; iI++)
		{
			CvTeam& kTeam = GET_TEAM((TeamTypes)iI);
			if (kTeam.isAlive())
			{
				kTeam.setTurnActive(true);
				FAssert(getNumGameTurnActive() == kTeam.getAliveCount());
			}

			break;
		}
	}
	else
	{
		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (isPbem() && GET_PLAYER((PlayerTypes)iI).isHuman())
				{
					if (iI == getActivePlayer())
					{
						// Nobody else left alive
						GC.getInitCore().setType(GAME_HOTSEAT_NEW);
						GET_PLAYER((PlayerTypes)iI).setTurnActive(true);
					}
					else if (!getPbemTurnSent())
					{
						gDLL->sendPbemTurn((PlayerTypes)iI);
					}
				}
				else
				{
					GET_PLAYER((PlayerTypes)iI).setTurnActive(true);
					FAssert(getNumGameTurnActive() == 1);
				}

				break;
			}
		}
	}

	testVictory();

	gDLL->getEngineIFace()->SetDirty(GlobePartialTexture_DIRTY_BIT, true);
	gDLL->getEngineIFace()->DoTurn();

	gDLL->getEngineIFace()->AutoSave();
}

void CvGame::doFoundingFathers()
{
	std::vector<CvTeam*> teams;

	// find alive colonial players for fast looping
	for (TeamTypes eTeam = FIRST_TEAM; eTeam < NUM_TEAM_TYPES; ++eTeam)
	{
		CvTeam &kTeam = GET_TEAM(eTeam);
		if (kTeam.isAlive() && kTeam.hasColonialPlayer())
		{
			teams.push_back(&kTeam);
		}
	}

	std::vector<CvTeam*> teamsThisFather;

	for (FatherTypes eFather = FIRST_FATHER; eFather < NUM_FATHER_TYPES; ++eFather)
	{
		if (getFatherTeam(eFather) != NO_TEAM)
		{
			continue;
		}
		const CvFatherInfo& kFather = GC.getFatherInfo(eFather);

		int iMaxPoints = -1;
		teamsThisFather.clear();
		for (std::vector<CvTeam*>::iterator it = teams.begin(); it != teams.end(); ++it)
		{
			CvTeam *pTeam = *it;
			if (pTeam->canConvinceFather(eFather))
			{
				const int iPoints = pTeam->getFatherPoints(kFather.getFatherPointType());
				if (iPoints > iMaxPoints)
				{
					teamsThisFather.clear();
					teamsThisFather.push_back(pTeam);
					iMaxPoints = iPoints;
				}
				else if (iPoints == iMaxPoints)
				{
					teamsThisFather.push_back(pTeam);
				}
			}
		}

		const unsigned int iNumWinningTeams = teamsThisFather.size();
		if (iNumWinningTeams == 1)
		{
			teamsThisFather[0]->offerFoundingFather(eFather);
		}
		else if (iNumWinningTeams > 1)
		{
			// since there is more than one team, offer the founding father to a random one. This way if a bunch of FFs are released in one turn,
			// we will likely not have the case where one team takes all of them prior to them even being offered to other teams
			teamsThisFather[getSorenRand().get(iNumWinningTeams, "pick team for founding father")]->offerFoundingFather(eFather);
		}
	}
}

// < JAnimals Mod Start >
void CvGame::createBarbarianPlayer()
{
	if (getBarbarianPlayer() != NO_PLAYER)
		return; // we know him, it's done

	for (PlayerTypes eCandidate = FIRST_PLAYER; eCandidate < NUM_PLAYER_TYPES; ++eCandidate )
	{
		CvPlayer const& kPlayer = GET_PLAYER(eCandidate);
		if (!kPlayer.isEverAlive())  continue; //Civs dead all along don't count
		if (kPlayer.getCivilizationInfo().getCivCategoryTypes() == CIV_CATEGORY_BARBARIAN )
		{
			setBarbarianPlayer(eCandidate);
			return;
		}
	}

	PlayerTypes eNewPlayer = getNextPlayerType();

	if (eNewPlayer == NO_PLAYER)
	{
		FAssertMsg(false, "No more room to add the Barbarian player");
		return; //no more room;
	}

	//LEADER_BARBARIAN  and CIVILIZATION_BARBARIAN are always assumed to exist,see HardcodedEnumSetup.h
	addPlayer(eNewPlayer, GLOBAL_DEFINE_BARBARIAN_LEADER, GLOBAL_DEFINE_BARBARIAN_CIVILIZATION);
	setBarbarianPlayer(eNewPlayer);

	TeamTypes eTeam = GET_PLAYER(getBarbarianPlayer()).getTeam();
	CvTeamAI& kBarbarianTeam = GET_TEAM(eTeam);

	for (TeamTypes eOtherTeam = FIRST_TEAM; eOtherTeam < NUM_TEAM_TYPES; ++eOtherTeam)
	{
		if (eOtherTeam == eTeam) continue;

		CvTeamAI& kOtherTeam = GET_TEAM(eOtherTeam);

		if (!kOtherTeam.isAlive()) continue;

		// R&R, ray, changes to Wild Animals
		// Natives and Kings will not Fight Animals
		if (!kOtherTeam.hasNativePlayer() && !kOtherTeam.hasEuropePlayer() && !kBarbarianTeam.isAtWar(eOtherTeam))
		{
			kBarbarianTeam.declareWar(eOtherTeam, false, kBarbarianTeam.AI_getWarPlan(eOtherTeam));
		}

		if (!kBarbarianTeam.isPermanentWarPeace(eOtherTeam))
		{
			kBarbarianTeam.setPermanentWarPeace(eOtherTeam, true);
		}
	}
}

void CvGame::createAnimalsLand()
{
    if (getBarbarianPlayer() == NO_PLAYER)
    {
        return;
    }

    if (isOption(GAMEOPTION_NO_WILD_LAND_ANIMALS))
	{
		return;
	}

	if (getElapsedGameTurns() < GC.getHandicapInfo(getHandicapType()).getAIAnimalLandNumTurnsNoSpawn())
	{
		return;
	}

	CvArea* pLoopArea;
	CvPlot* pPlot;
	UnitTypes eLastUnit, eBestUnit, eLoopUnit;
	int iNeededAnimals;
	int iValue, iBestValue, iRand;
	int iLoop, iI, iJ;
	int iStartDist = 0;

	for(pLoopArea = GC.getMap().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop))
	{
		iNeededAnimals = (pLoopArea->getNumTiles() * GC.getHandicapInfo(getHandicapType()).getAIAnimalLandMaxPercent()) / 100;
		//iNeededAnimals = std::min(100, iNeededAnimals);
		//iNeededAnimals -= pLoopArea->getUnitsPerPlayer(getBarbarianPlayer());
		if (iNeededAnimals == 0 && GC.getGameINLINE().getSorenRandNum(10, "Small Area Animal Spwan") == 1)
		{
			iNeededAnimals = 1;
		}

		if (iNeededAnimals > pLoopArea->getUnitsPerPlayer(getBarbarianPlayer()))
		{
			eLastUnit = NO_UNIT;

			if (!pLoopArea->isWater())
			{
				for (iI = 0; iI < iNeededAnimals; iI++)
				{
				    pPlot = GC.getMap().syncRandPlot((RANDPLOT_NOT_CITY), pLoopArea->getID(), iStartDist);

					// WTP, ray, Do not spawn in impassable Terrain - owner is checked below
					if (pPlot != NULL && !pPlot->isImpassable())
					{
					    /*
					    // May Cause OOS Errors as Active Player is different on each Computer
					    if (pPlot->isActiveVisible(false))
					    {
					        continue
					    }
					    */
						eBestUnit = NO_UNIT;
						iBestValue = 0;

						CivilizationTypes eBarbCiv = GET_PLAYER(getBarbarianPlayer()).getCivilizationType();
						for (iJ = 0; iJ < GC.getNumUnitInfos(); iJ++)
						{
						    //eLoopUnit = (UnitTypes) GC.getCivilizationInfo(eBarbCiv).getCivilizationUnits(iJ);
							eLoopUnit = (UnitTypes) iJ;

							if (eLoopUnit == NO_UNIT)
							{
							    continue;
							}
							if (!GC.getUnitInfo(eLoopUnit).isAnimal())
							{
								continue;
							}
							iValue = 0;
							// WTP, ray, for spawing we now als check that the Owern of the Plot is either NO_Player or Native, thus it will not spawn in cultural radius of Europeans and Kings
							if (pPlot->getTerrainType() != NO_TERRAIN && (pPlot->getOwnerINLINE() == NO_PLAYER || GET_PLAYER(pPlot->getOwnerINLINE()).isNative()))
							{
								if (GC.getUnitInfo(eLoopUnit).getTerrainNative(pPlot->getTerrainType()))
								{
									iRand = GC.getWILD_ANIMAL_LAND_TERRAIN_NATIVE_WEIGHT();
									iValue += (1 + getSorenRandNum(iRand, "Wild Land Animal Selection - Terrain Weight"));
								}
							}
							if (eLastUnit != NO_UNIT && eLastUnit == eBestUnit)
							{
								iRand = GC.getWILD_ANIMAL_LAND_UNIT_VARIATION_WEIGHT();
								iValue -= (1 + getSorenRandNum(iRand, "Wild Land Animal Selection - Unit Variation Weight"));
							}
							if (iValue > 0 && iValue > iBestValue)
							{
								eBestUnit = eLoopUnit;
								iBestValue = iValue;
							}
						}

						if (eBestUnit != NO_UNIT)
						{
						    CvUnit* pNewUnit;
							pNewUnit = GET_PLAYER(getBarbarianPlayer()).initUnit(eBestUnit, NO_PROFESSION, pPlot->getX_INLINE(), pPlot->getY_INLINE(), UNITAI_ANIMAL);
							pNewUnit->setBarbarian(true);
							eLastUnit = eBestUnit;
						}
					}
				}
			}
		}
	}
}

void CvGame::createAnimalsSea()
{
	if (getBarbarianPlayer() == NO_PLAYER)
	{
		return;
	}

	if (isOption(GAMEOPTION_NO_WILD_SEA_ANIMALS))
	{
		return;
	}

	if (getElapsedGameTurns() < GC.getHandicapInfo(getHandicapType()).getAIAnimalSeaNumTurnsNoSpawn())
	{
		return;
	}

	UnitTypes eLastUnit, eBestUnit, eLoopUnit;
	int iNeededAnimals;
	int iValue, iBestValue, iRand;
	int iStartDist = 0;

	CvPlot* const pEuropePlot = getAnyEuropePlot();

	int iLoop;
	for(CvArea* pLoopArea = GC.getMap().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop))
	{
		iNeededAnimals = (pLoopArea->getNumTiles() * GC.getHandicapInfo(getHandicapType()).getAIAnimalSeaMaxPercent()) / 100;
		//iNeededAnimals = std::min(100, iNeededAnimals);
		//iNeededAnimals -= pLoopArea->getUnitsPerPlayer(getBarbarianPlayer());
		if (iNeededAnimals == 0 && GC.getGameINLINE().getSorenRandNum(10, "Small Area Animal Spwan") == 1)
		{
			iNeededAnimals = 1;
		}

		// Another random to create less wild Animals in Water
		const int lessrandom = getSorenRandNum(5, "Less Animal Sea Unit Random");

		if (lessrandom <= 1 && iNeededAnimals > pLoopArea->getUnitsPerPlayer(getBarbarianPlayer()))
		{
			eLastUnit = NO_UNIT;

			if (pLoopArea->isWater())
			{
				for (int iI = 0; iI < iNeededAnimals; iI++)
				{
					// WTP, ray, Do not spawn in impassable Terrain - owner is checked below
					CvPlot* const pPlot = GC.getMap().syncRandPlot((RANDPLOT_NOT_VISIBLE_TO_CIV | RANDPLOT_PASSIBLE), pLoopArea->getID(), iStartDist);

					if (pPlot != NULL)
					{
					    eBestUnit = NO_UNIT;
						iBestValue = 0;

						CivilizationTypes eBarbCiv = GET_PLAYER(getBarbarianPlayer()).getCivilizationType();
						for (int iJ = 0; iJ < GC.getNumUnitInfos(); iJ++)
						{
							eLoopUnit = (UnitTypes) iJ;

							// Check if the plot is connected to any Europe plot
							if (pPlot->getDistanceToOcean() >= PLOT_OCEAN_DISTANCE_IMPASSABLE_THRESHOLD)
								continue;

							if (eLoopUnit == NO_UNIT)
							{
								continue;
							}
							if (!GC.getUnitInfo(eLoopUnit).isAnimal())
							{
								continue;
							}
							iValue = 0;
							if (pPlot->getTerrainType() != NO_TERRAIN)
							{
								// WTP, ray, for spawing we now also check that the Owner of the Plot is either NO_Player or Native, thus it will not spawn in cultural radius of Europeans and Kings
								if (GC.getUnitInfo(eLoopUnit).getTerrainNative(pPlot->getTerrainType()) && (pPlot->getOwnerINLINE() == NO_PLAYER || GET_PLAYER(pPlot->getOwnerINLINE()).isNative()))
								{
									iRand = GC.getWILD_ANIMAL_SEA_TERRAIN_NATIVE_WEIGHT();
									iValue += (1 + getSorenRandNum(iRand, "Wild Sea Animal Selection - Terrain Weight"));
								}
							}
							if (eLastUnit != NO_UNIT && eLastUnit == eBestUnit)
							{
								iRand = GC.getWILD_ANIMAL_SEA_UNIT_VARIATION_WEIGHT();
								iValue -= (1 + getSorenRandNum(iRand, "Animal Sea Unit Selection"));
							}
							if (iValue > 0 && iValue > iBestValue)
							{
								eBestUnit = eLoopUnit;
								iBestValue = iValue;
							}
						}

						if (eBestUnit != NO_UNIT)
						{
							CvUnit* const pNewUnit = GET_PLAYER(getBarbarianPlayer()).initUnit(eBestUnit, NO_PROFESSION, pPlot->getX_INLINE(), pPlot->getY_INLINE(), UNITAI_ANIMAL_SEA);
							pNewUnit->setBarbarian(true);
							eLastUnit = eBestUnit;
						}
					}
				}
			}
		}
	}
}
// < JAnimals Mod End >


// R&R, ray, the Church - START
void CvGame::createChurchPlayer()
{
	if (getChurchPlayer() != NO_PLAYER)
		return; // we have the Church

	for (PlayerTypes eCandidate = FIRST_PLAYER; eCandidate < NUM_PLAYER_TYPES; ++eCandidate)
	{
		CvPlayer const& kPlayer = GET_PLAYER(eCandidate);
		if (!kPlayer.isEverAlive())  continue; //Civs dead all along don't count
		if (kPlayer.getCivilizationInfo().getCivCategoryTypes() == CIV_CATEGORY_CHURCH)
		{
			setChurchPlayer(eCandidate);
			return;
		}
	}


	PlayerTypes eNewPlayer = getNextPlayerType();
	if (eNewPlayer == NO_PLAYER)
	{
		FAssertMsg(false, "No more room to add the Church player");
		return;
	}


	// LEADER_CHURCH and CIVILIZATION_CHURCH are assumed to exist, see HardcodedEnumSetup.h
	addPlayer(eNewPlayer, GLOBAL_DEFINE_CHURCH_LEADER, GLOBAL_DEFINE_CHURCH_CIVILIZATION);
	setChurchPlayer(eNewPlayer);

	TeamTypes eTeam = GET_PLAYER(getChurchPlayer()).getTeam();
	CvTeamAI& kChurchTeam = GET_TEAM(eTeam);

	for (TeamTypes eOtherTeam = FIRST_TEAM; eOtherTeam < NUM_TEAM_TYPES; ++eOtherTeam)
	{
		if (eOtherTeam == eTeam) continue;

		CvTeamAI& kOtherTeam = GET_TEAM(eOtherTeam);

		// set permanent peace with all Europeans and make contact
		if (kOtherTeam.isAlive() && !kOtherTeam.hasNativePlayer())
		{
			kChurchTeam.setPermanentWarPeace(eOtherTeam, true);
			kChurchTeam.meet(eOtherTeam, false);
		}

	}

}
// R&R, ray, the Church - END


void CvGame::doDeals()
{
	CvDeal* pLoopDeal;
	int iLoop;

	verifyDeals();

	for(pLoopDeal = firstDeal(&iLoop); pLoopDeal != NULL; pLoopDeal = nextDeal(&iLoop))
	{
		pLoopDeal->doTurn();
	}
}


void CvGame::updateWar()
{
	if (!isOption(GAMEOPTION_ALWAYS_WAR))
	{
		return;
	}

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		CvTeam& kTeam1 = GET_TEAM((TeamTypes)iI);
		if (kTeam1.isAlive() && kTeam1.isHuman())
		{
			for (int iJ = 0; iJ < MAX_TEAMS; iJ++)
			{
				TeamTypes eTeam2 = (TeamTypes) iJ;
				CvTeam& kTeam2 = GET_TEAM(eTeam2);
				if (kTeam2.isAlive() && !kTeam2.isHuman())
				{
					FAssert(iI != iJ);

					if (kTeam1.isHasMet(eTeam2))
					{
						if (!kTeam1.isAtWar(eTeam2) && kTeam1.canDeclareWar(eTeam2))
						{
							kTeam1.declareWar(eTeam2, false, NO_WARPLAN);
						}
					}
				}
			}
		}
	}
}


void CvGame::updateMoves()
{
	MOD_PROFILE("CvGame::updateMoves");

	CvSelectionGroup* pLoopSelectionGroup;
	std::vector<int> aiShuffle(MAX_PLAYERS);
	int iLoop;
	int iI;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		aiShuffle[iI] = iI;
	}

	if (isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
	{
		getSorenRand().shuffleArray(aiShuffle, NULL);
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& player = GET_PLAYER((PlayerTypes)(aiShuffle[iI]));

		if (player.isAlive())
		{
			if (player.isTurnActive())
			{
				if (!player.isAutoMoves())
				{
					player.AI_unitUpdate();

					if (!player.isHuman())
					{
						if (!player.hasBusyUnit() && !player.hasReadyUnit(true))
						{
							player.setAutoMoves(true);
						}
					}
				}

				if (player.isAutoMoves())
				{
					for(pLoopSelectionGroup = player.firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = player.nextSelectionGroup(&iLoop))
					{
						pLoopSelectionGroup->autoMission();
					}

					if (!(player.hasBusyUnit()))
					{
						player.setAutoMoves(false);
					}
				}
			}
		}
	}
}


void CvGame::verifyCivics()
{
	int iI;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			GET_PLAYER((PlayerTypes)iI).verifyCivics();
		}
	}
}


void CvGame::updateTimers()
{
	int iI;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			GET_PLAYER((PlayerTypes)iI).updateTimers();
		}
	}
}


void CvGame::updateTurnTimer()
{
	int iI;

	// Are we using a turn timer?
	if (isMPOption(MPOPTION_TURN_TIMER))
	{
		if (getElapsedGameTurns() > 0 || !isOption(GAMEOPTION_ADVANCED_START))
		{
			// Has the turn expired?
			if (getTurnSlice() > getCutoffSlice())
			{
				for (iI = 0; iI < MAX_PLAYERS; iI++)
				{
					if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).isTurnActive())
					{
						GET_PLAYER((PlayerTypes)iI).setEndTurn(true);

						if (!isMPOption(MPOPTION_SIMULTANEOUS_TURNS) && !isSimultaneousTeamTurns())
						{
							break;
						}
					}
				}
			}
		}
	}
}


void CvGame::testAlive()
{
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
	    // < JAnimals Mod Start > AND // R&R, ray, the Church - START
	    if (!isBarbarianPlayer((PlayerTypes) iI) && !isChurchPlayer((PlayerTypes) iI))
	    {
            GET_PLAYER((PlayerTypes)iI).verifyAlive();
	    }
		// < JAnimals Mod End >
	}
}

bool CvGame::testVictory(VictoryTypes eVictory, TeamTypes eTeam, bool* pbEndScore) const
{
	FAssert(eVictory >= 0 && eVictory < GC.getNumVictoryInfos());
	FAssert(eTeam >=0 && eTeam < MAX_TEAMS);
	FAssert(GET_TEAM(eTeam).isAlive());

	bool bValid = isVictoryValid(eVictory);
	if (pbEndScore)
	{
		*pbEndScore = false;
	}

	if (bValid)
	{
		if (GC.getVictoryInfo(eVictory).isEndScore())
		{
			if (pbEndScore)
			{
				*pbEndScore = true;
			}

			if (getMaxTurns() == 0)
			{
				bValid = false;
			}
			else if (getElapsedGameTurns() < getMaxTurns())
			{
				bValid = false;
			}
			else
			{
				bool bFound = false;

				for (int iK = 0; iK < MAX_TEAMS; iK++)
				{
					if (GET_TEAM((TeamTypes)iK).isAlive())
					{
						if (iK != eTeam)
						{
							if (getTeamScore((TeamTypes)iK) >= getTeamScore(eTeam))
							{
								bFound = true;
								break;
							}
						}
					}
				}

				if (bFound)
				{
					bValid = false;
				}
			}
		}
	}

	if (bValid)
	{
		if (GC.getVictoryInfo(eVictory).isEndEurope())
		{
			if (getMaxTurns() == 0)
			{
				bValid = false;
			}
			else if (getElapsedGameTurns() < getMaxTurns())
			{
				bValid = false;
			}
			else
			{
				if (isNetworkMultiPlayer())
				{
					if (!GET_TEAM(eTeam).hasEuropePlayer())
					{
						bValid = false;
					}
				}
				else //Single Player always picks active parent as winner
				{
					PlayerTypes eParent = GET_PLAYER(getActivePlayer()).getParent();
					if(eParent == NO_PLAYER)
					{
						bValid = false;
					}
					else
					{
						if (!GET_PLAYER(eParent).isAlive() || !GET_PLAYER(eParent).isEurope() || (GET_PLAYER(eParent).getTeam() != eTeam))
						{
							bValid = false;
						}
					}
				}
			}
		}
	}

	if (bValid)
	{
		if (GC.getVictoryInfo(eVictory).isTargetScore())
		{
			if (getTargetScore() == 0)
			{
				bValid = false;
			}
			else if (getTeamScore(eTeam) < getTargetScore())
			{
				bValid = false;
			}
			else
			{
				bool bFound = false;

				for (int iK = 0; iK < MAX_TEAMS; iK++)
				{
					if (GET_TEAM((TeamTypes)iK).isAlive())
					{
						if (iK != eTeam)
						{
							if (getTeamScore((TeamTypes)iK) >= getTeamScore(eTeam))
							{
								bFound = true;
								break;
							}
						}
					}
				}

				if (bFound)
				{
					bValid = false;
				}
			}
		}
	}

	if (bValid)
	{
		if (GC.getVictoryInfo(eVictory).isConquest())
		{
			if (GET_TEAM(eTeam).getNumCities() == 0)
			{
				bValid = false;
			}
			else
			{
				bool bFound = false;

				for (int iK = 0; iK < MAX_TEAMS; iK++)
				{
					if (GET_TEAM((TeamTypes)iK).isAlive())
					{
						if (iK != eTeam)
						{
							if (GET_TEAM((TeamTypes)iK).getNumCities() > 0)
							{
								bFound = true;
								break;
							}
						}
					}
				}

				if (bFound)
				{
					bValid = false;
				}
			}
		}
	}

	if (bValid)
	{
		if (GC.getVictoryInfo(eVictory).isRevolution())
		{
			if (!GET_TEAM(eTeam).checkIndependence())
			{
				bValid = false;
			}
		}
	}

	if (bValid)
	{
		if (getAdjustedPopulationPercent(eVictory) > 0)
		{
			if (100 * GET_TEAM(eTeam).getTotalPopulation() < getTotalPopulation() * getAdjustedPopulationPercent(eVictory))
			{
				bValid = false;
			}
		}
	}

	if (bValid)
	{
		if (getAdjustedLandPercent(eVictory) > 0)
		{
			if (100 * GET_TEAM(eTeam).getTotalLand() < GC.getMap().getLandPlots() * getAdjustedLandPercent(eVictory))
			{
				bValid = false;
			}
		}
	}

	if (bValid)
	{
		if ((GC.getVictoryInfo(eVictory).getCityCulture() != NO_CULTURELEVEL) && (GC.getVictoryInfo(eVictory).getNumCultureCities() > 0))
		{
			int iCount = 0;

			for (int iK = 0; iK < MAX_PLAYERS; iK++)
			{
				if (GET_PLAYER((PlayerTypes)iK).isAlive())
				{
					if (GET_PLAYER((PlayerTypes)iK).getTeam() == eTeam)
					{
						int iLoop;
						for (CvCity* pLoopCity = GET_PLAYER((PlayerTypes)iK).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iK).nextCity(&iLoop))
						{
							if (pLoopCity->getCultureLevel() >= GC.getVictoryInfo(eVictory).getCityCulture())
							{
								iCount++;
							}
						}
					}
				}
			}

			if (iCount < GC.getVictoryInfo(eVictory).getNumCultureCities())
			{
				bValid = false;
			}
		}
	}

	if (bValid)
	{
		if (GC.getVictoryInfo(eVictory).getTotalCultureRatio() > 0)
		{
			int iThreshold = ((GET_TEAM(eTeam).countTotalCulture() * 100) / GC.getVictoryInfo(eVictory).getTotalCultureRatio());

			bool bFound = false;

			for (int iK = 0; iK < MAX_TEAMS; iK++)
			{
				if (GET_TEAM((TeamTypes)iK).isAlive())
				{
					if (iK != eTeam)
					{
						if (GET_TEAM((TeamTypes)iK).countTotalCulture() > iThreshold)
						{
							bFound = true;
							break;
						}
					}
				}
			}

			if (bFound)
			{
				bValid = false;
			}
		}
	}

	if (bValid)
	{
		for (int iK = 0; iK < GC.getNumBuildingClassInfos(); iK++)
		{
			if (GC.getBuildingClassInfo((BuildingClassTypes) iK).getVictoryThreshold(eVictory) > GET_TEAM(eTeam).getBuildingClassCount((BuildingClassTypes)iK))
			{
				bValid = false;
				break;
			}
		}
	}

	// PatchMod: Victorys START
	if (bValid)
	{
		if (GC.getVictoryInfo(eVictory).getTotalProductionRate() > 0)
		{
			if (GC.getVictoryInfo(eVictory).getTotalProductionRate() > GET_TEAM(eTeam).getTotalProductionRate())
			{
				bValid = false;
			}
		}
	}
	// PatchMod: Victorys END

	if (bValid)
	{
		long lResult = 1;
		CyArgsList argsList;
		argsList.add(eVictory);
		gDLL->getPythonIFace()->callFunction(PYGameModule, "isVictory", argsList.makeFunctionArgs(), &lResult);
		if (0 == lResult)
		{
			bValid = false;
		}
	}

	return bValid;
}


struct CvWinner
{
	TeamTypes eTeam;
	VictoryTypes eVictory;
	int iScore;

	bool operator<(const CvWinner& rhs)
	{
		return iScore < rhs.iScore;
	}
};

void CvGame::testVictory()
{
	// PatchMod: Victorys START
	for (int iJ = 0; iJ < GC.getNumVictoryInfos(); iJ++)
	{
		VictoryTypes eVictory = (VictoryTypes) iJ;
		if (GC.getVictoryInfo(eVictory).getTotalProductionRate() > 0)
		{
			if (iVictoriesSet == 0)
			{
				GC.getVictoryInfo(eVictory).setTotalProductionRate(GC.getVictoryInfo(eVictory).getTotalProductionRate() * GC.getGameSpeedInfo(GC.getInitCore().getGameSpeed()).getGrowthPercent() / 100);
			}
		}
	}
	iVictoriesSet = 1;
	// PatchMod: Victorys END

	bool bEndScore = false;

	if (getVictory() != NO_VICTORY)
	{
		return;
	}

	if (getGameState() == GAMESTATE_EXTENDED)
	{
		return;
	}

	updateScore();

	if (GC.getUSE_IS_VICTORY_TEST_CALLBACK())
	{ 
		long lResult = 1;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "isVictoryTest", NULL, &lResult);
		if (lResult == 0)
		{
			return;
		}
	}

	std::vector<CvWinner> aaiWinners;
	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		TeamTypes eTeam = (TeamTypes) iI;
		CvTeam& kLoopTeam = GET_TEAM(eTeam);
		if (kLoopTeam.isAlive())
		{
			for (int iJ = 0; iJ < GC.getNumVictoryInfos(); iJ++)
			{
				VictoryTypes eVictory = (VictoryTypes) iJ;
				if (testVictory((VictoryTypes)iJ, eTeam, &bEndScore))
				{
					CvWinner kWinner;
					kWinner.eTeam = eTeam;
					kWinner.eVictory = eVictory;
					kWinner.iScore = 0;
					for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
					{
						CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayer);
						if (kPlayer.isAlive() && kPlayer.getTeam() == eTeam)
						{
							kWinner.iScore += kPlayer.calculateScore();
						}
					}
					aaiWinners.push_back(kWinner);
				}
			}
		}
	}

	if (aaiWinners.size() > 0)
	{
		std::vector<CvWinner>::iterator itMin = std::max_element(aaiWinners.begin(), aaiWinners.end());  // finds the max score
		//int iWinner = getSorenRandNum(aaiWinners.size(), "Victory tie breaker");
		setWinner(itMin->eTeam, itMin->eVictory);
	}

	if (getVictory() == NO_VICTORY)
	{
		if (getMaxTurns() > 0)
		{
			if (getElapsedGameTurns() >= getMaxTurns())
			{
				if (!bEndScore)
				{
					if ((getAIAutoPlay() > 0) || gDLL->GetAutorun())
					{
						setGameState(GAMESTATE_EXTENDED);
					}
					else
					{
						setGameState(GAMESTATE_OVER);
					}
				}
			}
		}
	}
}


int CvGame::getIndexAfterLastDeal()
{
	return m_deals.getIndexAfterLast();
}


int CvGame::getNumDeals()
{
	return m_deals.getCount();
}


 CvDeal* CvGame::getDeal(int iID)
{
	return ((CvDeal *)(m_deals.getAt(iID)));
}


CvDeal* CvGame::addDeal()
{
	return ((CvDeal *)(m_deals.add()));
}


 void CvGame::deleteDeal(int iID)
{
	m_deals.removeAt(iID);
	gDLL->getInterfaceIFace()->setDirty(Foreign_Screen_DIRTY_BIT, true);
}

CvDeal* CvGame::firstDeal(int *pIterIdx, bool bRev)
{
	return !bRev ? m_deals.beginIter(pIterIdx) : m_deals.endIter(pIterIdx);
}


CvDeal* CvGame::nextDeal(int *pIterIdx, bool bRev)
{
	return !bRev ? m_deals.nextIter(pIterIdx) : m_deals.prevIter(pIterIdx);
}


CvRandom& CvGame::getMapRand()
{
	FAssertMsg(GC.isMainThread(), "Using non-const random while multi threaded (causes OOS)");
	return m_mapRand;
}

const CvRandom& CvGame::getMapRand() const
{
	return m_mapRand;
}


int CvGame::getMapRandNum(int iNum, const char* pszLog)
{
	FAssertMsg(GC.isMainThread(), "Using non-const random while multi threaded (causes OOS)");
	return m_mapRand.get(iNum, pszLog);
}


CvRandom& CvGame::getSorenRand()
{
	FAssertMsg(GC.isMainThread(), "Using non-const random while multi threaded (causes OOS)");
	return m_sorenRand;
}

const CvRandom& CvGame::getSorenRand() const
{
	return m_sorenRand;
}

int CvGame::getSorenRandNum(int iNum, const char* pszLog)
{
	return m_sorenRand.get(iNum, pszLog);
}

int CvGame::calculateSyncChecksum(CvString* pLogString)
{
	// Don't bother with any checksum in a SP game!
	if (!GC.getGameINLINE().isGameMultiPlayer())
		return 0;

	PROFILE_FUNC();

	CvUnit* pLoopUnit;
	int iMultiplier;
	int iValue;
	int iLoop;
	int iI, iJ;

	iValue = 0;

	iValue += getMapRand().getSeed();
	iValue += getSorenRand().getSeed();

	iValue += getNumCities();
	iValue += getNumDeals();

	iValue += GC.getMap().getOwnedPlots();
	iValue += GC.getMap().getNumAreas();

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kPlayer.isEverAlive())
		{
			iMultiplier = getPlayerScore((PlayerTypes)iI);

			switch (getTurnSlice() % 4)
			{
			case 0:
				iMultiplier += (kPlayer.getTotalPopulation() * 543271);
				iMultiplier += (kPlayer.getTotalLand() * 327382);
				iMultiplier += (kPlayer.getGold() * 107564);
				iMultiplier += (kPlayer.getAssets() * 327455);
				iMultiplier += (kPlayer.getPower() * 135647);
				iMultiplier += (kPlayer.getNumCities() * 436432);
				iMultiplier += (kPlayer.getNumUnits() * 324111);
				iMultiplier += (kPlayer.getNumSelectionGroups() * 215356);
				break;

			case 1:
				{
					int aiYields[NUM_YIELD_TYPES];
					kPlayer.calculateTotalYields(aiYields);
					for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
					{
						iMultiplier += aiYields[iJ] * 432754;
					}
				}
				break;

			case 2:
				for (iJ = 0; iJ < GC.getNumImprovementInfos(); iJ++)
				{
					iMultiplier += (kPlayer.getImprovementCount((ImprovementTypes)iJ) * 883422);
				}

				for (iJ = 0; iJ < GC.getNumBuildingClassInfos(); iJ++)
				{
					iMultiplier += (kPlayer.getBuildingClassCountPlusMaking((BuildingClassTypes)iJ) * 954531);
				}

				for (iJ = 0; iJ < GC.getNumUnitClassInfos(); iJ++)
				{
					iMultiplier += (kPlayer.getUnitClassCountPlusMaking((UnitClassTypes)iJ) * 754843);
				}

				for (iJ = 0; iJ < NUM_UNITAI_TYPES; iJ++)
				{
					iMultiplier += (kPlayer.AI_totalUnitAIs((UnitAITypes)iJ) * 643383);
				}
				break;

			case 3:
				for (pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoop))
				{
					iMultiplier += (pLoopUnit->getX_INLINE() * 876543);
					iMultiplier += (pLoopUnit->getY_INLINE() * 985310);
					iMultiplier += (pLoopUnit->getDamage() * 736373);
					iMultiplier += (pLoopUnit->getExperience() * 820622);
					iMultiplier += (pLoopUnit->getLevel() * 367291);
				}
				break;
			}

			if (iMultiplier != 0)
			{
				iValue *= iMultiplier;
			}
		}
	}

	if (pLogString != NULL)
	{
		// doesn't look right that iValue should change if logging is enabled - Nightinggale
		/*
		iValue += getMapRand().getSeed();
		iValue += getSorenRand().getSeed();

		iValue += getNumCities();
		iValue += getNumDeals();

		iValue += GC.getMapINLINE().getOwnedPlots();
		iValue += GC.getMapINLINE().getNumAreas();
		*/

		*pLogString += CvString::format("Map seed = %d\n", getMapRand().getSeed());
		*pLogString += CvString::format("Gameplay seed = %d\n", getSorenRand().getSeed());
		*pLogString += CvString::format("NumCities = %d\n", getNumCities());
		*pLogString += CvString::format("NumDeals = %d\n", getNumDeals());
		*pLogString += CvString::format("Owned plots = %d\n", GC.getMap().getOwnedPlots());
		*pLogString += CvString::format("NumAreas = %d\n", GC.getMap().getNumAreas());
		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
			if (kPlayer.isEverAlive())
			{
				*pLogString += CvString::format("----PLAYER %d (%s)\n", iI, CvString(kPlayer.getName()).GetCString());
				*pLogString += CvString::format("Score = %d\n", getPlayerScore((PlayerTypes)iI));
				*pLogString += CvString::format("Population = %d\n", kPlayer.getTotalPopulation());
				*pLogString += CvString::format("Land = %d\n", kPlayer.getTotalLand());
				*pLogString += CvString::format("Gold = %d\n", kPlayer.getGold());
				*pLogString += CvString::format("Assets = %d\n", kPlayer.getAssets());
				*pLogString += CvString::format("Power = %d\n", kPlayer.getPower());
				*pLogString += CvString::format("NumCities = %d\n", kPlayer.getNumCities());
				*pLogString += CvString::format("NumUnits = %d\n", kPlayer.getNumUnits());
				*pLogString += CvString::format("NumSelectionGroups = %d\n", kPlayer.getNumSelectionGroups());
				int aiYields[NUM_YIELD_TYPES];
				kPlayer.calculateTotalYields(aiYields);
				for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				{
					*pLogString += CvString::format("%s = %d\n", CvString(GC.getYieldInfo((YieldTypes) iJ).getDescription()).GetCString(), aiYields[iJ]);
				}
				for (iJ = 0; iJ < GC.getNumImprovementInfos(); iJ++)
				{
					*pLogString += CvString::format("%s = %d\n", CvString(GC.getImprovementInfo((ImprovementTypes) iJ).getDescription()).GetCString(), kPlayer.getImprovementCount((ImprovementTypes)iJ));
				}
				for (iJ = 0; iJ < GC.getNumBuildingClassInfos(); iJ++)
				{
					*pLogString += CvString::format("%s = %d\n", CvString(GC.getBuildingClassInfo((BuildingClassTypes) iJ).getDescription()).GetCString(), kPlayer.getBuildingClassCountPlusMaking((BuildingClassTypes)iJ));
				}
				for (iJ = 0; iJ < GC.getNumUnitClassInfos(); iJ++)
				{
					*pLogString += CvString::format("%s = %d\n", CvString(GC.getUnitClassInfo((UnitClassTypes) iJ).getDescription()).GetCString(), kPlayer.getUnitClassCountPlusMaking((UnitClassTypes)iJ));
				}
				for (iJ = 0; iJ < NUM_UNITAI_TYPES; iJ++)
				{
					*pLogString += CvString::format("%s = %d\n", CvString(GC.getUnitAIInfo((UnitAITypes) iJ).getDescription()).GetCString(), kPlayer.AI_totalUnitAIs((UnitAITypes)iJ));
				}
				for (pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoop))
				{
					*pLogString += CvString::format("UnitID=%d (%s) x=%d, y=%d, damage=%d, experience=%d, level=%d\n", pLoopUnit->getID(), CvString(pLoopUnit->getName()).GetCString(), pLoopUnit->getX_INLINE(), pLoopUnit->getY_INLINE(), pLoopUnit->getDamage(), pLoopUnit->getExperience(), pLoopUnit->getLevel());
				}
			}
		}
	}

	return iValue;
}


int CvGame::calculateOptionsChecksum()
{
	PROFILE_FUNC();

	int iValue;
	int iI, iJ;

	iValue = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		for (iJ = 0; iJ < NUM_PLAYEROPTION_TYPES; iJ++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isOption((PlayerOptionTypes)iJ))
			{
				iValue += (iI * 943097);
				iValue += (iJ * 281541);
			}
		}
	}

	return iValue;
}


void CvGame::addReplayMessage(ReplayMessageTypes eType, PlayerTypes ePlayer, CvWString pszText, Coordinates coord, ColorTypes eColor)
{
	int iGameTurn = getGameTurn();
	CvReplayMessage* pMessage = new CvReplayMessage(iGameTurn, eType, ePlayer);
	if (NULL != pMessage)
	{
		pMessage->setPlot(coord.x(), coord.y());
		pMessage->setText(pszText);
		if (NO_COLOR == eColor)
		{
			eColor = (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE");
		}
		pMessage->setColor(eColor);
		m_listReplayMessages.push_back(pMessage);
	}
}

void CvGame::clearReplayMessageMap()
{
	for (ReplayMessageList::const_iterator itList = m_listReplayMessages.begin(); itList != m_listReplayMessages.end(); ++itList)
	{
		const CvReplayMessage* pMessage = *itList;
		if (NULL != pMessage)
		{
			delete pMessage;
		}
	}
	m_listReplayMessages.clear();
}

int CvGame::getReplayMessageTurn(uint i) const
{
	if (i >= m_listReplayMessages.size())
	{
		return (-1);
	}
	const CvReplayMessage* pMessage =  m_listReplayMessages[i];
	if (NULL == pMessage)
	{
		return (-1);
	}
	return pMessage->getTurn();
}

ReplayMessageTypes CvGame::getReplayMessageType(uint i) const
{
	if (i >= m_listReplayMessages.size())
	{
		return (NO_REPLAY_MESSAGE);
	}
	const CvReplayMessage* pMessage =  m_listReplayMessages[i];
	if (NULL == pMessage)
	{
		return (NO_REPLAY_MESSAGE);
	}
	return pMessage->getType();
}

int CvGame::getReplayMessagePlotX(uint i) const
{
	if (i >= m_listReplayMessages.size())
	{
		return (-1);
	}
	const CvReplayMessage* pMessage =  m_listReplayMessages[i];
	if (NULL == pMessage)
	{
		return (-1);
	}
	return pMessage->getPlotX();
}

int CvGame::getReplayMessagePlotY(uint i) const
{
	if (i >= m_listReplayMessages.size())
	{
		return (-1);
	}
	const CvReplayMessage* pMessage =  m_listReplayMessages[i];
	if (NULL == pMessage)
	{
		return (-1);
	}
	return pMessage->getPlotY();
}

PlayerTypes CvGame::getReplayMessagePlayer(uint i) const
{
	if (i >= m_listReplayMessages.size())
	{
		return (NO_PLAYER);
	}
	const CvReplayMessage* pMessage =  m_listReplayMessages[i];
	if (NULL == pMessage)
	{
		return (NO_PLAYER);
	}
	return pMessage->getPlayer();
}

LPCWSTR CvGame::getReplayMessageText(uint i) const
{
	if (i >= m_listReplayMessages.size())
	{
		return (NULL);
	}
	const CvReplayMessage* pMessage =  m_listReplayMessages[i];
	if (NULL == pMessage)
	{
		return (NULL);
	}
	return pMessage->getText().GetCString();
}

ColorTypes CvGame::getReplayMessageColor(uint i) const
{
	if (i >= m_listReplayMessages.size())
	{
		return (NO_COLOR);
	}
	const CvReplayMessage* pMessage =  m_listReplayMessages[i];
	if (NULL == pMessage)
	{
		return (NO_COLOR);
	}
	return pMessage->getColor();
}


uint CvGame::getNumReplayMessages() const
{
	return m_listReplayMessages.size();
}

// Private Functions...

void CvGame::read(FDataStreamBase* pStream)
{
	CvSavegameReaderBase readerbase(pStream);
	CvSavegameReader reader(readerbase);

	read(reader);
}

void CvGame::write(FDataStreamBase* pStream)
{
	CvSavegameWriterBase writerbase(pStream);
	CvSavegameWriter writer(writerbase);
	write(writer);
	writerbase.WriteFile();
}

void CvGame::writeReplay(FDataStreamBase& stream, PlayerTypes ePlayer)
{
	SAFE_DELETE(m_pReplayInfo);
	m_pReplayInfo = new CvReplayInfo();
	if (m_pReplayInfo)
	{
		m_pReplayInfo->createInfo(ePlayer);

		m_pReplayInfo->write(stream);
	}
}

void CvGame::saveReplay(PlayerTypes ePlayer)
{
	gDLL->getEngineIFace()->SaveReplay(ePlayer);
}


void CvGame::showEndGameSequence()
{
	long iHours = getMinutesPlayed() / 60;
	long iMinutes = getMinutesPlayed() % 60;

	addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, NO_PLAYER, gDLL->getText("TXT_KEY_MISC_TIME_SPENT", iHours, iMinutes));

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& player = GET_PLAYER((PlayerTypes)iI);
		if (player.isHuman())
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_TEXT);
			if ((getWinner() != NO_TEAM) && (getVictory() != NO_VICTORY))
			{
				pInfo->setText(gDLL->getText("TXT_KEY_GAME_WON", GET_TEAM(getWinner()).getName().GetCString(), GC.getVictoryInfo(getVictory()).getTextKeyWide()));
			}
			else
			{
				pInfo->setText(gDLL->getText("TXT_KEY_MISC_DEFEAT"));
			}
			player.addPopup(pInfo);

			if (getWinner() == player.getTeam())
			{
				if (!CvString(GC.getVictoryInfo(getVictory()).getMovie()).empty())
				{
					// show movie
					pInfo = new CvPopupInfo(BUTTONPOPUP_MOVIE);
					pInfo->setText(CvWString(GC.getVictoryInfo(getVictory()).getMovie()));
					player.addPopup(pInfo);
				}
			}

			// show replay
			pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN);
			pInfo->setText(L"showReplay");
			pInfo->setData1(iI);
			pInfo->setOption1(false); // don't go to HOF on exit
			player.addPopup(pInfo);

			// show top cities / stats
			pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN);
			pInfo->setText(L"showInfoScreen");
			pInfo->setData1(0);
			pInfo->setData2(1);
			player.addPopup(pInfo);

			// show Hall of Fame
			pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN);
			pInfo->setText(L"showHallOfFame");
			player.addPopup(pInfo);
		}
	}
}

CvReplayInfo* CvGame::getReplayInfo() const
{
	return m_pReplayInfo;
}

void CvGame::setReplayInfo(CvReplayInfo* pReplay)
{
	SAFE_DELETE(m_pReplayInfo);
	m_pReplayInfo = pReplay;
}

void CvGame::addPlayer(PlayerTypes eNewPlayer, LeaderHeadTypes eLeader, CivilizationTypes eCiv)
{
	PlayerColorTypes eColor = (PlayerColorTypes)GC.getCivilizationInfo(eCiv).getDefaultPlayerColor();

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (eColor == NO_PLAYERCOLOR || GET_PLAYER((PlayerTypes)iI).getPlayerColor() == eColor)
		{
			for (int iK = 0; iK < GC.getNumPlayerColorInfos(); iK++)
			{
				if (iK != GC.getCivilizationInfo((CivilizationTypes)GC.getDefineINT("BARBARIAN_CIVILIZATION")).getDefaultPlayerColor())
				{
					bool bValid = true;

					for (int iL = 0; iL < MAX_PLAYERS; iL++)
					{
						if (GET_PLAYER((PlayerTypes)iL).getPlayerColor() == iK)
						{
							bValid = false;
							break;
						}
					}

					if (bValid)
					{
						eColor = (PlayerColorTypes)iK;
						iI = MAX_PLAYERS;
						break;
					}
				}
			}
		}
	}

	GC.getInitCore().setLeader(eNewPlayer, eLeader);
	GC.getInitCore().setCiv(eNewPlayer, eCiv);
	GC.getInitCore().setSlotStatus(eNewPlayer, SS_COMPUTER);
	GC.getInitCore().setColor(eNewPlayer, eColor);
	GET_PLAYER(eNewPlayer).init(eNewPlayer);
}

int CvGame::getPlotExtraYield(int iX, int iY, YieldTypes eYield) const
{
	for (std::vector<PlotExtraYield>::const_iterator it = m_aPlotExtraYields.begin(); it != m_aPlotExtraYields.end(); ++it)
	{
		if ((*it).m_iX == iX && (*it).m_iY == iY)
		{
			return (*it).m_aeExtraYield[eYield];
		}
	}

	return 0;
}

void CvGame::setPlotExtraYield(int iX, int iY, YieldTypes eYield, int iExtraYield)
{
	bool bFound = false;

	for (std::vector<PlotExtraYield>::iterator it = m_aPlotExtraYields.begin(); it != m_aPlotExtraYields.end(); ++it)
	{
		if ((*it).m_iX == iX && (*it).m_iY == iY)
		{
			(*it).m_aeExtraYield[eYield] += iExtraYield;
			bFound = true;
			break;
		}
	}

	if (!bFound)
	{
		PlotExtraYield kExtraYield;
		kExtraYield.m_iX = iX;
		kExtraYield.m_iY = iY;
		for (int i = 0; i < NUM_YIELD_TYPES; ++i)
		{
			if (eYield == i)
			{
				kExtraYield.m_aeExtraYield.push_back(iExtraYield);
			}
			else
			{
				kExtraYield.m_aeExtraYield.push_back(0);
			}
		}
		m_aPlotExtraYields.push_back(kExtraYield);
	}

	CvPlot* pPlot = GC.getMap().plot(iX, iY);
	if (NULL != pPlot)
	{
		pPlot->updateYield(true);
	}
}

void CvGame::removePlotExtraYield(int iX, int iY)
{
	for (std::vector<PlotExtraYield>::iterator it = m_aPlotExtraYields.begin(); it != m_aPlotExtraYields.end(); ++it)
	{
		if ((*it).m_iX == iX && (*it).m_iY == iY)
		{
			m_aPlotExtraYields.erase(it);
			break;
		}
	}

	CvPlot* pPlot = GC.getMap().plot(iX, iY);
	if (NULL != pPlot)
	{
		pPlot->updateYield(true);
	}
}

// CACHE: cache frequently used values
///////////////////////////////////////


bool CvGame::culturalVictoryValid()
{
	if (m_iNumCultureVictoryCities > 0)
	{
		return true;
	}

	return false;
}

int CvGame::culturalVictoryNumCultureCities()
{
	return m_iNumCultureVictoryCities;
}

CultureLevelTypes CvGame::culturalVictoryCultureLevel()
{
	if (m_iNumCultureVictoryCities > 0)
	{
		return (CultureLevelTypes) m_eCultureVictoryCultureLevel;
	}

	return NO_CULTURELEVEL;
}


void CvGame::doUpdateCacheOnTurn()
{
	// reset cultural victories
	m_iNumCultureVictoryCities = 0;
	for (int iI = 0; iI < GC.getNumVictoryInfos(); iI++)
	{
		if (isVictoryValid((VictoryTypes) iI))
		{
			CvVictoryInfo& kVictoryInfo = GC.getVictoryInfo((VictoryTypes) iI);
			if (kVictoryInfo.getCityCulture() > 0)
			{
				int iNumCultureCities = kVictoryInfo.getNumCultureCities();
				if (iNumCultureCities > m_iNumCultureVictoryCities)
				{
					m_iNumCultureVictoryCities = iNumCultureCities;
					m_eCultureVictoryCultureLevel = kVictoryInfo.getCityCulture();
				}
			}
		}
	}
}

bool CvGame::isEventActive(EventTriggerTypes eTrigger) const
{
	for (std::vector<EventTriggerTypes>::const_iterator it = m_aeInactiveTriggers.begin(); it != m_aeInactiveTriggers.end(); ++it)
	{
		if (*it == eTrigger)
		{
			return false;
		}
	}

	return true;
}

void CvGame::initEvents()
{
	for (int iTrigger = 0; iTrigger < GC.getNumEventTriggerInfos(); ++iTrigger)
	{
		if (GC.getGameINLINE().isOption(GAMEOPTION_NO_EVENTS) || getSorenRandNum(100, "Event Active?") >= GC.getEventTriggerInfo((EventTriggerTypes)iTrigger).getPercentGamesActive())
		{
			m_aeInactiveTriggers.push_back((EventTriggerTypes)iTrigger);
		}
	}
}

bool CvGame::isCivEverActive(CivilizationTypes eCivilization) const
{
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (kLoopPlayer.isEverAlive())
		{
			if (kLoopPlayer.getCivilizationType() == eCivilization)
			{
				return true;
			}
		}
	}

	return false;
}

bool CvGame::isLeaderEverActive(LeaderHeadTypes eLeader) const
{
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (kLoopPlayer.isEverAlive())
		{
			if (kLoopPlayer.getLeaderType() == eLeader)
			{
				return true;
			}
		}
	}

	return false;
}

bool CvGame::isUnitEverActive(UnitTypes eUnit) const
{
	for (int iCiv = 0; iCiv < GC.getNumCivilizationInfos(); ++iCiv)
	{
		if (isCivEverActive((CivilizationTypes)iCiv))
		{
			if (eUnit == GC.getCivilizationInfo((CivilizationTypes)iCiv).getCivilizationUnits(GC.getUnitInfo(eUnit).getUnitClassType()))
			{
				return true;
			}
		}
	}

	return false;
}

bool CvGame::isBuildingEverActive(BuildingTypes eBuilding) const
{
	for (int iCiv = 0; iCiv < GC.getNumCivilizationInfos(); ++iCiv)
	{
		if (isCivEverActive((CivilizationTypes)iCiv))
		{
			if (eBuilding == GC.getCivilizationInfo((CivilizationTypes)iCiv).getCivilizationBuildings(GC.getBuildingInfo(eBuilding).getBuildingClassType()))
			{
				return true;
			}
		}
	}

	return false;
}

TeamTypes CvGame::getFatherTeam(FatherTypes eFather) const
{
	FAssert(eFather >= 0 && eFather < GC.getNumFatherInfos());
	return m_em_eFatherTeam.get(eFather);
}

int CvGame::getFatherGameTurn(FatherTypes eFather) const
{
	FAssert(eFather >= 0 && eFather < GC.getNumFatherInfos());
	return m_em_iFatherGameTurn.get(eFather);
}

void CvGame::setFatherTeam(AssertCallerData assertData, FatherTypes eFather, TeamTypes eTeam)
{
	FAssertWithCaller(assertData, eFather >= 0);
	FAssertWithCaller(assertData, eFather < NUM_FATHER_TYPES);
	FAssertWithCaller(assertData, eTeam >= 0);
	FAssertWithCaller(assertData, eTeam < NUM_TEAM_TYPES);

	if (getFatherTeam(eFather) != eTeam)
	{
		bool bFirstTime = true;
		if (getFatherTeam(eFather) != NO_TEAM)
		{
			GET_TEAM(getFatherTeam(eFather)).processFather(eFather, -1);
			bFirstTime = false;
		}

		m_em_eFatherTeam.set(eFather, eTeam);

		if (getFatherTeam(eFather) != NO_TEAM)
		{
			GET_TEAM(getFatherTeam(eFather)).processFather(eFather, 1);

			if (bFirstTime)
			{
				m_em_iFatherGameTurn.set(eFather, getGameTurn());

				for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
				{
					CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayer);
					// R&R, ray, fix for Natives in Permanent Alliance getting Units from FFs of Players
					if (kPlayer.isAlive() && !kPlayer.isNative() && kPlayer.getTeam() == eTeam)
					// if (kPlayer.isAlive() && kPlayer.getTeam() == eTeam)
					{
						kPlayer.processFatherOnce(eFather);
					}
				}

				CvWString szBuffer;

				for (int iI = 0; iI < MAX_PLAYERS; iI++)
				{
					CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iI);
					if (kPlayer.isAlive())
					{
						if (GET_TEAM(kPlayer.getTeam()).isHasMet(eTeam))
						{
							szBuffer = gDLL->getText("TXT_KEY_FATHER_JOINED_TEAM", GC.getFatherInfo(eFather).getTextKeyWide(), GET_TEAM(getFatherTeam(eFather)).getName().GetCString());
						}
						else
						{
							szBuffer = gDLL->getText("TXT_KEY_FATHER_JOINED_UNKNOWN", GC.getFatherInfo(eFather).getTextKeyWide());
						}

						gDLL->UI().addPlayerMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_GLOBECIRCUMNAVIGATED", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
					}
				}
				szBuffer = gDLL->getText("TXT_KEY_FATHER_JOINED_TEAM", GC.getFatherInfo(eFather).getTextKeyWide(), GET_TEAM(getFatherTeam(eFather)).getName().GetCString());
				addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, NO_PLAYER, szBuffer, -1, -1, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
			}
		}
	}
}

bool CvGame::getRemainingFathers(FatherPointTypes ePointType, std::vector<FatherTypes>& aFathers)
{
	bool bAll = true;
	aFathers.clear();
	for (int iFather = 0; iFather < GC.getNumFatherInfos(); ++iFather)
	{
		CvFatherInfo& kFather = GC.getFatherInfo((FatherTypes) iFather);
		if (kFather.getPointCost(ePointType) > 0 && getFatherTeam((FatherTypes) iFather) == NO_TEAM)
		{
			aFathers.push_back((FatherTypes) iFather);
		}
		else
		{
			bAll = false;
		}
	}

	return bAll;
}


int CvGame::getFatherCategoryPosition(FatherTypes eFather) const
{
	FAssert(eFather != NO_FATHER);
	int iCost = eFather;
	FatherCategoryTypes eCategory = GC.getFatherInfo(eFather).getFatherCategory();
	int iPosition = 0;
	for (int iLoopFather = 0; iLoopFather < GC.getNumFatherInfos(); ++iLoopFather)
	{
		FatherTypes eLoopFather = (FatherTypes) iLoopFather;
		CvFatherInfo& kLoopFather = GC.getFatherInfo(eLoopFather);
		if (kLoopFather.getFatherCategory() == eCategory)
		{
			int iLoopCost = eLoopFather;
			if (iLoopCost < iCost)
			{
				iPosition++;
			}

			if (iLoopCost == iCost && iLoopFather < eFather)
			{
				iPosition++;
			}
		}
	}

	return iPosition;
}

void CvGame::changeYieldBoughtTotal(PlayerTypes eMainEurope, YieldTypes eYield, int iChange) const
{
	//change non-mercantile Europes by partial amount
	for(int iEurope=0;iEurope<MAX_PLAYERS;iEurope++)
	{
		CvPlayer& kEuropePlayer = GET_PLAYER((PlayerTypes) iEurope);
		if(kEuropePlayer.isAlive() && kEuropePlayer.isEurope())
		{
			//check if any children are mercantile
			int iMercantilePercent = (iEurope == eMainEurope) ? 100 : GC.getDefineINT("EUROPE_MARKET_CORRELATION_PERCENT");
			for(int iPlayer=0;iPlayer<MAX_PLAYERS;iPlayer++)
			{
				CvPlayer& kChildPlayer = GET_PLAYER((PlayerTypes) iPlayer);
				if(kChildPlayer.isAlive() && (kChildPlayer.getParent() == iEurope))
				{
					iMercantilePercent *= 100 + kChildPlayer.getMercantileFactor();
					iMercantilePercent /= 100;
				}
			}

			//affect non-mercantile amounts
			kEuropePlayer.changeYieldBoughtTotal(eYield, iChange * iMercantilePercent / 100);
		}
	}
}

// WTP, ray, Yields Traded Total for Africa and Port Royal - START
void CvGame::changeYieldBoughtTotalAfrica(PlayerTypes eMainEurope, YieldTypes eYield, int iChange) const
{
	//change non-mercantile Europes by partial amount
	for(int iEurope=0;iEurope<MAX_PLAYERS;iEurope++)
	{
		CvPlayer& kEuropePlayer = GET_PLAYER((PlayerTypes) iEurope);
		if(kEuropePlayer.isAlive() && kEuropePlayer.isEurope())
		{
			//check if any children are mercantile
			int iMercantilePercent = (iEurope == eMainEurope) ? 100 : GC.getDefineINT("EUROPE_MARKET_CORRELATION_PERCENT");
			for(int iPlayer=0;iPlayer<MAX_PLAYERS;iPlayer++)
			{
				CvPlayer& kChildPlayer = GET_PLAYER((PlayerTypes) iPlayer);
				if(kChildPlayer.isAlive() && (kChildPlayer.getParent() == iEurope))
				{
					iMercantilePercent *= 100 + kChildPlayer.getMercantileFactor();
					iMercantilePercent /= 100;
				}
			}

			//affect non-mercantile amounts
			kEuropePlayer.changeYieldBoughtTotalAfrica(eYield, iChange * iMercantilePercent / 100);
		}
	}
}

void CvGame::changeYieldBoughtTotalPortRoyal(PlayerTypes eMainEurope, YieldTypes eYield, int iChange) const
{
	//change non-mercantile Europes by partial amount
	for(int iEurope=0;iEurope<MAX_PLAYERS;iEurope++)
	{
		CvPlayer& kEuropePlayer = GET_PLAYER((PlayerTypes) iEurope);
		if(kEuropePlayer.isAlive() && kEuropePlayer.isEurope())
		{
			//check if any children are mercantile
			int iMercantilePercent = (iEurope == eMainEurope) ? 100 : GC.getDefineINT("EUROPE_MARKET_CORRELATION_PERCENT");
			for(int iPlayer=0;iPlayer<MAX_PLAYERS;iPlayer++)
			{
				CvPlayer& kChildPlayer = GET_PLAYER((PlayerTypes) iPlayer);
				if(kChildPlayer.isAlive() && (kChildPlayer.getParent() == iEurope))
				{
					iMercantilePercent *= 100 + kChildPlayer.getMercantileFactor();
					iMercantilePercent /= 100;
				}
			}

			//affect non-mercantile amounts
			kEuropePlayer.changeYieldBoughtTotalPortRoyal(eYield, iChange * iMercantilePercent / 100);
		}
	}
}
// WTP, ray, Yields Traded Total for Africa and Port Royal - END

void CvGame::updateOceanDistances()
{
	PROFILE_FUNC();

	std::deque<CvPlot*> plotQueue;
	for(int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);
		if (pLoopPlot->isEurope())
		{
			pLoopPlot->setDistanceToOcean(0);
			plotQueue.push_back(pLoopPlot);
		}
		else
		{
			pLoopPlot->setDistanceToOcean(MAX_SHORT);
		}
	}

	int iVisits = 0;
	while (!plotQueue.empty())
	{
		iVisits++;
		CvPlot* pPlot = plotQueue.front();
		plotQueue.pop_front();

		int iDistance = pPlot->getDistanceToOcean();
		iDistance += 1;

		if (!pPlot->isWater())
		{
			iDistance += 2;
		}

		if (pPlot->isImpassable())
		{	
			// Non-impassable plots with a number higher than this threshold are considered ice-locked.
			iDistance += PLOT_OCEAN_DISTANCE_IMPASSABLE_THRESHOLD;
		}

		for (int iDirection = 0; iDirection < NUM_DIRECTION_TYPES; iDirection++)
		{
			CvPlot* pDirectionPlot = plotDirection(pPlot->getX_INLINE(), pPlot->getY_INLINE(), (DirectionTypes)iDirection);
			if (pDirectionPlot != NULL)
			{
				if ((pPlot->isWater() && pDirectionPlot->isWater() && pPlot->isAdjacentWaterPassable(pDirectionPlot))
					|| (pPlot->isWater() && !pDirectionPlot->isWater())
					|| (!pPlot->isWater() && !pDirectionPlot->isWater()))
				{
					if (iDistance < pDirectionPlot->getDistanceToOcean())
					{
						pDirectionPlot->setDistanceToOcean(iDistance);
						plotQueue.push_back(pDirectionPlot);
					}
				}
			}
		}
	}

	//DannyDaemonic fix
	//OutputDebugStr(CvString::format("[CvGame::updateOceanDistances] Plots: %i, Visits: %i\n", GC.getMapINLINE().numPlotsINLINE(), iVisits).GetCString());
	OutputDebugString(CvString::format("[CvGame::updateOceanDistances] Plots: %i, Visits: %i\n", GC.getMap().numPlotsINLINE(), iVisits).GetCString());
	//end
}

// R&R, ray, Correct Geographical Placement of Natives - START
bool CvGame::isWBNorthAmericanNative() const
{
	return m_bWBNorthAmericanNative;
}

void CvGame::setWBNorthAmericanNative(bool bValue)
{
	m_bWBNorthAmericanNative = bValue;
}

bool CvGame::isWBSouthAmericanNative() const
{
	return m_bWBSouthAmericanNative;
}

void CvGame::setWBSouthAmericanNative(bool bValue)
{
	m_bWBSouthAmericanNative = bValue;
}

bool CvGame::isWBCentralAmericanNative() const
{
	return m_bWBCentralAmericanNative;
}

void CvGame::setWBCentralAmericanNative(bool bValue)
{
	m_bWBCentralAmericanNative = bValue;
}
// R&R, ray, Correct Geographical Placement of Natives - END


void CreateOOSSavegame();

// function to write a log file containing various data useful to track down OOS issues.
// Never call this directly, call it by clicking a WIDGET_NETWORK_DESYNC button in python.
// This in turn will send PLAYER_ACTION_NETWORK_DESYNC_LOG_WRITE on the network to call this function in sync.
//
// The idea is that each player will generate a log and then a diff tool can tell which data went out of sync.
// If a game is in sync, the files should be identical.
void CvGame::writeDesyncLog()
{
	CvString filename = gDLL->getModName();
	filename.append(CvString::format("Desync log for player %d", getActivePlayer()));
	filename.append(".txt");

	// Using C style file writing
	// The reason is that C++ style apparently has a known bug in our compiler and it won't compile in some projects (like ours).
	// The precise cause is unknown, but the recommendation is to use C style file writing.
	FILE *f = fopen(filename.GetCString(), "w");
	if (f != NULL)
	{
		fprintf(f, "Turn slice: %d\n", this->getTurnSlice());

		CvString LogString;

		calculateSyncChecksum(&LogString);
		fprintf(f, "%s\n", LogString.c_str());

		for (PlayerTypes ePlayer = FIRST_PLAYER; ePlayer < NUM_PLAYER_TYPES; ++ePlayer)
		{
			CvPlayerAI const& kPlayer = CvPlayerAI::getPlayer(ePlayer);
			if (!kPlayer.isAlive())
				continue;
			kPlayer.writeDesyncLog(f);
		}

		GC.getMap().writeDesyncLog(f);

		fclose(f);
	}

	CreateOOSSavegame();

	// create a popup telling the task is done
	CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_DESYNC_LOG_COMPLETE);
	gDLL->getInterfaceIFace()->addPopup(pInfo, NO_PLAYER, true);
}

int CvGame::getRemainingForcedPeaceTurns() const
{
	const int gamespeedMod = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
	const int forcedPeaceTurns = GC.getDefineINT("COLONIAL_FORCED_PEACE_TURNS") * gamespeedMod / 100;
	const int diff = forcedPeaceTurns - GC.getGameINLINE().getElapsedGameTurns();
	return std::max(0, diff);
}

CvPlot* CvGame::getAnyEuropePlot() const
{
	for (PlayerTypes ePlayer = FIRST_PLAYER; ePlayer < NUM_PLAYER_TYPES; ePlayer++)
	{
		CvPlayer& kPlayer = GET_PLAYER(ePlayer);
		
		if (kPlayer.isEverAlive() && kPlayer.getCivCategoryTypes() == CIV_CATEGORY_EUROPEAN)
		{
			 CvPlot* const pStartingPlot = kPlayer.getStartingPlot();
			 if (pStartingPlot != NULL)
			 {
				 return pStartingPlot;
			 }
		}
	}
	
	return NULL;
}
