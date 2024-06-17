// selectionGroup.cpp

#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "CvSelectionGroup.h"
#include "CvGameAI.h"
#include "CvPlayerAI.h"
#include "CvTeamAI.h"
#include "CvUnit.h"
#include "CvGameCoreUtils.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvDLLEntityIFaceBase.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLFAStarIFaceBase.h"
#include "FAStarNode.h"
#include "CvInfos.h"
#include "FProfiler.h"
#include "CvDLLEventReporterIFaceBase.h"
#include "CyPlot.h"
#include "CySelectionGroup.h"
#include "CyArgsList.h"
#include "CvDLLPythonIFaceBase.h"
#include "CvPopupInfo.h"
#include "CvTradeRoute.h"
#include "CvSavegame.h"
#include "KmodPathFinder.h"
#include "BetterBTSAI.h"

KmodPathFinder CvSelectionGroup::path_finder; // K-Mod

// Public Functions...

CvSelectionGroup::CvSelectionGroup()
{
	reset(0, NO_PLAYER, true);
}


CvSelectionGroup::~CvSelectionGroup()
{
	uninit();
}


void CvSelectionGroup::init(int iID, PlayerTypes eOwner)
{
	//--------------------------------
	// Init saved data
	reset(iID, eOwner);

	//--------------------------------
	// Init non-saved data

	//--------------------------------
	// Init other game data
	AI_init();
}


void CvSelectionGroup::uninit()
{
	m_units.clear();
	m_aTradeRoutes.clear();
	m_missionQueue.clear();
}


// FUNCTION: reset()
// Initializes data members that are serialized.
void CvSelectionGroup::reset(int iID, PlayerTypes eOwner, bool bConstructorCall)
{
	//--------------------------------
	// Uninit class
	uninit();
	resetSavedData(iID, eOwner);

	m_bIsBusyCache = false;

	if (!bConstructorCall)
	{
		AI_reset();
	}
}


void CvSelectionGroup::kill()
{
	FAssert(getOwnerINLINE() != NO_PLAYER);
	FAssertMsg(getID() != FFreeList::INVALID_INDEX, "getID() is not expected to be equal with FFreeList::INVALID_INDEX");
	FAssertMsg(getNumUnits() == 0, "The number of units is expected to be 0");

	GET_PLAYER(getOwnerINLINE()).removeGroupCycle(getID());

	GET_PLAYER(getOwnerINLINE()).deleteSelectionGroup(getID());
}

bool CvSelectionGroup::sentryAlert() const
{
	int iMaxRange = 0;
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = getUnitNodeLoop(pUnitNode);

		if (pLoopUnit == NULL)
		{
			continue;
		}

		int iRange = pLoopUnit->visibilityRange() + 1;

		if (iRange > iMaxRange)
		{
			iMaxRange = iRange;
		}
	}

	CvUnit* pHeadUnit = getHeadUnit();
	if (NULL != pHeadUnit)
	{
		for (int iX = -iMaxRange; iX <= iMaxRange; ++iX)
		{
			for (int iY = -iMaxRange; iY <= iMaxRange; ++iY)
			{
				CvPlot* pPlot = ::plotXY(pHeadUnit->getX_INLINE(), pHeadUnit->getY_INLINE(), iX, iY);
				if (NULL != pPlot)
				{
					if (pHeadUnit->plot()->canSeePlot(pPlot, pHeadUnit->getTeam(), iMaxRange - 1, NO_DIRECTION))
					{
						if (pPlot->isVisibleEnemyUnit(pHeadUnit))
						{
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}

/** NBMOD REF **/

/***************************************************************************/
/**                                                                       **/
/** float CvSelectionGroup::NBMOD_GetShipStrength() const                 **/
/**                                                                       **/
/** Ermittelt die Schiffst�rke der Gruppe.                                **/
/**                                                                       **/
/***************************************************************************/
float CvSelectionGroup::NBMOD_GetShipStrength() const
{
    float fStrength = 0;

   	if (getNumUnits() > 0)
	{

		CLLNode<IDInfo>* pUnitNode = headUnitNode();
		while (pUnitNode != NULL)
		{

			CvUnit* pLoopUnit = getUnitNodeLoop(pUnitNode);

			if (pLoopUnit != NULL)
			{
				fStrength += pLoopUnit->NBMOD_GetShipStrength();
			}
		}
	}

	return fStrength;
}

/** NBMOD REF **/

void CvSelectionGroup::doTurn()
{
	PROFILE_FUNC();

	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (getNumUnits() > 0)
	{
		bool bHurt = false;

		// do unit's turns (checking for damage)
		CLLNode<IDInfo>* pUnitNode = headUnitNode();
		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = getUnitNodeLoop(pUnitNode);

			if (pLoopUnit != NULL)
			{
				pLoopUnit->doTurn();

				if (pLoopUnit->isHurt())
				{
					bHurt = true;
				}
			}
		}

		ActivityTypes eActivityType = getActivityType();

		// wake unit if skipped last turn
		//		or healing and automated or no longer hurt (automated healing is one turn at a time)
		//		or on sentry and there is danger
		if ((eActivityType == ACTIVITY_HOLD) ||
			((eActivityType == ACTIVITY_HEAL) && (AI_isControlled() || !bHurt)) ||
			((eActivityType == ACTIVITY_SENTRY) && (sentryAlert())))
		{
			setActivityType(ACTIVITY_AWAKE);
		}

		if (AI_isControlled())
		{
			if ((getActivityType() != ACTIVITY_MISSION) || (!canFight() && (GET_PLAYER(getOwnerINLINE()).AI_getPlotDanger(plot(), 1) > 0)))
			{
				setForceUpdate(true);
			}
		}
		else
		{
			if (getActivityType() == ACTIVITY_MISSION)
			{
				if (GET_PLAYER(getOwnerINLINE()).AI_getPlotDanger(plot(), 1) > 0)
				{
					clearMissionQueue();
				}
			}
		}

		if (isHuman())
		{
			if (GC.getGameINLINE().isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
			{
				int iBestWaitTurns = 0;

				pUnitNode = headUnitNode();

				while (pUnitNode != NULL)
				{
					CvUnit* pLoopUnit = getUnitNodeLoop(pUnitNode);

					if (pLoopUnit == NULL)
					{
						continue;
					}

					const int iWaitTurns = GLOBAL_DEFINE_MIN_TIMER_UNIT_DOUBLE_MOVES - GC.getGameINLINE().getTurnSlice() - pLoopUnit->getLastMoveTurn();

					if (iWaitTurns > iBestWaitTurns)
					{
						iBestWaitTurns = iWaitTurns;
					}
				}

				setMissionTimer(std::max(iBestWaitTurns, getMissionTimer()));

				if (iBestWaitTurns > 0)
				{
					// Cycle selection if the current group is selected
					CvUnit* pSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();
					if (pSelectedUnit && pSelectedUnit->getGroup() == this)
					{
						gDLL->getInterfaceIFace()->selectGroup(pSelectedUnit, false, false, false);
					}
				}
			}
		}
	}

	doDelayedDeath();
}

bool CvSelectionGroup::showMoves() const
{
	if (GC.getGameINLINE().isMPOption(MPOPTION_SIMULTANEOUS_TURNS) || GC.getGameINLINE().isSimultaneousTeamTurns())
	{
		return false;
	}

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kLoopPlayer.isAlive())
		{
			if (kLoopPlayer.isHuman())
			{
				CvUnit* pHeadUnit = getHeadUnit();

				if (NULL != pHeadUnit)
				{
					if (pHeadUnit->isEnemy(kLoopPlayer.getTeam()))
					{
						if (kLoopPlayer.isOption(PLAYEROPTION_SHOW_ENEMY_MOVES))
						{
							return true;
						}
					}
					else
					{
						if (kLoopPlayer.isOption(PLAYEROPTION_SHOW_FRIENDLY_MOVES))
						{
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}


void CvSelectionGroup::updateTimers()
{
	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (getNumUnits() > 0)
	{
		bool bCombat = false;

		CLLNode<IDInfo>* pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = getUnitNodeLoop(pUnitNode);

			if (pLoopUnit != NULL && pLoopUnit->isCombat())
			{
				pLoopUnit->updateCombat();

				bCombat = true;
				break;
			}
		}

		if (!bCombat)
		{
			updateMission();
		}
	}

	doDelayedDeath();
}


// Returns true if group was killed...
bool CvSelectionGroup::doDelayedDeath()
{
	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (isBusy())
	{
		return false;
	}

	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = getUnitNodeLoop(pUnitNode);

		if (pLoopUnit != NULL)
		{
			pLoopUnit->doDelayedDeath();
		}
	}

	if (getNumUnits() == 0)
	{
		kill();
		return true;
	}

	return false;
}


void CvSelectionGroup::playActionSound()
{
	// Pitboss should not be playing sounds!
#ifndef PITBOSS

	CvUnit *pHeadUnit;
	int iScriptId = -1;

	pHeadUnit = getHeadUnit();
	if ( pHeadUnit )
	{
		iScriptId = pHeadUnit->getArtInfo(0)->getActionSoundScriptId();
	}

	if ( (iScriptId == -1) && pHeadUnit )
	{
		CvCivilizationInfo *pCivInfo;
		pCivInfo = &GC.getCivilizationInfo( pHeadUnit->getCivilizationType() );
		if ( pCivInfo )
		{
			iScriptId = pCivInfo->getActionSoundScriptId();
		}
	}

	if ( (iScriptId != -1) && pHeadUnit )
	{
		CvPlot *pPlot = GC.getMap().plotINLINE(pHeadUnit->getX_INLINE(),pHeadUnit->getY_INLINE());
		if ( pPlot )
		{
			gDLL->Do3DSound( iScriptId, pPlot->getPoint() );
		}
	}

#endif // n PITBOSS
}


void CvSelectionGroup::pushMission(MissionTypes eMission, int iData1, int iData2, int iFlags, bool bAppend, bool bManual, MissionAITypes eMissionAI, CvPlot* pMissionAIPlot, CvUnit* pMissionAIUnit)
{
	PROFILE_FUNC();

	MissionData mission;

	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (!bAppend)
	{
		if (isBusy())
		{
			return;
		}

		clearMissionQueue();
	}

	if (bManual)
	{
		setAutomateType(NO_AUTOMATE);
	}

	mission.eMissionType = eMission;
	mission.iData1 = iData1;
	mission.iData2 = iData2;
	mission.iFlags = iFlags;
	mission.iPushTurn = GC.getGameINLINE().getGameTurn();

	if (canAllMove()) // K-Mod. Do not set the AI mission type if this is just a "follow" command!
		AI_setMissionAI(eMissionAI, pMissionAIPlot, pMissionAIUnit);

	insertAtEndMissionQueue(mission, !bAppend);

	if (bManual)
	{
		if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
		{
			if (isBusy() && GC.getMissionInfo(eMission).isSound())
			{
				playActionSound();
			}

			gDLL->getInterfaceIFace()->setHasMovedUnit(true);
		}

		gDLL->getEventReporterIFace()->selectionGroupPushMission(this, eMission);

		doDelayedDeath();
	}
}


void CvSelectionGroup::popMission()
{
	CLLNode<MissionData>* pTailNode;

	FAssert(getOwnerINLINE() != NO_PLAYER);

	pTailNode = tailMissionQueueNode();

	if (pTailNode != NULL)
	{
		deleteMissionQueueNode(pTailNode);
	}
}


void CvSelectionGroup::autoMission()
{
	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (getNumUnits() > 0)
	{
		if (headMissionQueueNode() != NULL)
		{
			if (!isBusy())
			{
				if (isHuman() && GET_PLAYER(getOwnerINLINE()).AI_getPlotDanger(plot(), 1) > 0)
				{
					clearMissionQueue();
				}
				else
				{
					if (getActivityType() == ACTIVITY_MISSION)
					{
						continueMission();
					}
					else
					{
						startMission();
					}
				}
			}
		}
	}

	doDelayedDeath();
}


void CvSelectionGroup::updateMission()
{
	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (getMissionTimer() > 0)
	{
		changeMissionTimer(-1);

		if (getMissionTimer() == 0)
		{
			if (getActivityType() == ACTIVITY_MISSION)
			{
				continueMission();
			}
			else
			{
				if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
				{
					if (gDLL->getInterfaceIFace()->getHeadSelectedUnit() == NULL)
					{
						gDLL->getInterfaceIFace()->changeCycleSelectionCounter(1);
					}
				}
			}
		}
	}
}


CvPlot* CvSelectionGroup::lastMissionPlot()
{
	CLLNode<MissionData>* pMissionNode;
	CvUnit* pTargetUnit;

	pMissionNode = tailMissionQueueNode();

	while (pMissionNode != NULL)
	{
		switch (pMissionNode->m_data.eMissionType)
		{
		case MISSION_MOVE_TO:
		case MISSION_ROUTE_TO:
		case MISSION_ROUTE_TO_ROAD:
		case MISSION_ROUTE_TO_COUNTRY_ROAD:
			return GC.getMap().plotINLINE(pMissionNode->m_data.iData1, pMissionNode->m_data.iData2);
			break;

		case MISSION_MOVE_TO_UNIT:
			pTargetUnit = GET_PLAYER((PlayerTypes)pMissionNode->m_data.iData1).getUnit(pMissionNode->m_data.iData2);
			if (pTargetUnit != NULL)
			{
				return pTargetUnit->plot();
			}
			break;

		case MISSION_SKIP:
		case MISSION_SLEEP:
		case MISSION_FORTIFY:
		case MISSION_HEAL:
		case MISSION_SENTRY:
		case MISSION_BOMBARD:
		case MISSION_PILLAGE:
		case MISSION_FOUND:
		case MISSION_JOIN_CITY:
		case MISSION_BUILD:
		case MISSION_LEAD:
		//TAC Whaling, ray
		case MISSION_WHALING:
		case MISSION_FISHING:
		//End TAC Whaling, ray
			break;

		default:
			FAssert(false);
			break;
		}

		pMissionNode = prevMissionQueueNode(pMissionNode);
	}

	return plot();
}


bool CvSelectionGroup::canStartMission(int iMission, int iData1, int iData2, CvPlot* pPlot, bool bTestVisible, bool bUseCache)
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pTargetUnit;
	CvUnit* pLoopUnit;

	//cache isBusy
	if(bUseCache)
	{
		if(m_bIsBusyCache)
		{
			return false;
		}
	}
	else
	{
		if (isBusy())
		{
			return false;
		}
	}

	if (pPlot == NULL)
	{
		pPlot = plot();
	}

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = getUnitNodeLoop(pUnitNode);

		if (pLoopUnit == NULL)
		{
			continue;
		}

		switch (iMission)
		{
		case MISSION_MOVE_TO:
			if (!(pPlot->at(iData1, iData2)))
			{
				return true;
			}
			break;

		case MISSION_ROUTE_TO:
			{
				if (!(pPlot->at(iData1, iData2)) || canBuildRoute(pPlot))
				{
					return true;
				}
				break;
			}

		case MISSION_ROUTE_TO_ROAD:
			{
				if (!(pPlot->at(iData1, iData2)) || canBuildRoute(pPlot, ROUTE_ROAD))
				{
					return true;
				}
				break;
			}

		case MISSION_ROUTE_TO_COUNTRY_ROAD:
			{
				if (!(pPlot->at(iData1, iData2)) || canBuildRoute(pPlot, ROUTE_COUNTRY_ROAD))
				{
					return true;
				}
				break;
			}

		case MISSION_MOVE_TO_UNIT:
			FAssertMsg(iData1 != NO_PLAYER, "iData1 should be a valid Player");
			pTargetUnit = GET_PLAYER((PlayerTypes)iData1).getUnit(iData2);
			if ((pTargetUnit != NULL) && !(pTargetUnit->atPlot(pPlot)))
			{
				return true;
			}
			break;

		case MISSION_SKIP:
			if (pLoopUnit->canHold(pPlot))
			{
				return true;
			}
			break;

		case MISSION_SLEEP:
			if (pLoopUnit->canSleep(pPlot))
			{
				return true;
			}
			break;

		case MISSION_FORTIFY:
			if (pLoopUnit->canFortify(pPlot))
			{
				return true;
			}
			break;

		case MISSION_HEAL:
			if (pLoopUnit->canHeal(pPlot)
				// fix: can't heal unless heal rate at the plot is positive (read: not in storms) - Nightinggale
				&& pLoopUnit->healTurns(pLoopUnit->plot()) != MAX_INT)
			{
				return true;
			}
			break;

		case MISSION_SENTRY:
			if (pLoopUnit->canSentry(pPlot))
			{
				return true;
			}
			break;

		case MISSION_BOMBARD:
			if (pLoopUnit->canBombard(pPlot))
			{
				return true;
			}
			break;

		case MISSION_PILLAGE:
			if (pLoopUnit->canPillage(pPlot))
			{
				return true;
			}
			break;

		case MISSION_FOUND:
			if (pLoopUnit->canFound(pPlot, bTestVisible))
			{
				return true;
			}
			break;

		case MISSION_JOIN_CITY:
			if (pLoopUnit->canJoinCity(pPlot, bTestVisible))
			{
				return true;
			}
			break;

		case MISSION_BUILD:
            FAssertMsg(((BuildTypes)iData1) < GC.getNumBuildInfos(), "Invalid Build");
            if (pLoopUnit->canBuild(pPlot, ((BuildTypes)iData1), bTestVisible))
            {
                return true;
            }
			break;

		case MISSION_LEAD:
			if (pLoopUnit->canLead(pPlot, iData1))
			{
				return true;
			}
			break;

		case MISSION_BEGIN_COMBAT:
		case MISSION_END_COMBAT:
		case MISSION_SURRENDER:
		case MISSION_IDLE:
		case MISSION_DIE:
		case MISSION_DAMAGE:
		case MISSION_MULTI_SELECT:
		case MISSION_MULTI_DESELECT:
			break;
		//TAC Whaling, ray
		case MISSION_WHALING:
			if (pLoopUnit->isWhalingBoat() && pLoopUnit->canGatherResource(pPlot, bTestVisible))
			{
				return true;
			}
			break;
		//End TAC Whaling, ray
		// R&R, ray, High Sea Fishing - START
		case MISSION_FISHING:
			if (pLoopUnit->isFishingBoat() && pLoopUnit->canGatherResource(pPlot, bTestVisible))
			{
				return true;
			}
			break;
		// R&R, ray, High Sea Fishing - END

		default:
			FAssert(false);
			break;
		}
	}

	return false;
}


void CvSelectionGroup::startMission()
{
	FAssert(!isBusy());
	FAssert(getOwnerINLINE() != NO_PLAYER);
	FAssert(headMissionQueueNode() != NULL);

	if (!GC.getGameINLINE().isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
	{
		if (!GET_PLAYER(getOwnerINLINE()).isTurnActive())
		{
			if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
			{
				if (IsSelected())
				{
					gDLL->getInterfaceIFace()->changeCycleSelectionCounter(1);
				}
			}

			return;
		}
	}

	setActivityType(ACTIVITY_MISSION);

	bool bDelete = false;
//	bool bAction = false;
	bool bNotify = false;

	if (!canStartMission(headMissionQueueNode()->m_data.eMissionType, headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2, plot()))
	{
		bDelete = true;
	}
	else
	{
		FAssertMsg(GET_PLAYER(getOwnerINLINE()).isTurnActive() || GET_PLAYER(getOwnerINLINE()).isHuman(), "It's expected that either the turn is active for this player or the player is human");

		switch (headMissionQueueNode()->m_data.eMissionType)
		{
		case MISSION_MOVE_TO:
		case MISSION_ROUTE_TO:
		case MISSION_ROUTE_TO_ROAD:
		case MISSION_ROUTE_TO_COUNTRY_ROAD:
		case MISSION_MOVE_TO_UNIT:
			break;

		case MISSION_SKIP:
			setActivityType(ACTIVITY_HOLD);
			bDelete = true;
			break;

		case MISSION_SLEEP:
			setActivityType(ACTIVITY_SLEEP);
			bNotify = true;
			bDelete = true;
			break;

		case MISSION_FORTIFY:
			setActivityType(ACTIVITY_SLEEP);
			bNotify = true;
			bDelete = true;
			break;

		case MISSION_HEAL:
			setActivityType(ACTIVITY_HEAL);
			bNotify = true;
			bDelete = true;
			break;

		case MISSION_SENTRY:
			setActivityType(ACTIVITY_SENTRY);
			bNotify = true;
			bDelete = true;
			break;

		case MISSION_BOMBARD:
		case MISSION_PILLAGE:
		case MISSION_FOUND:
		case MISSION_JOIN_CITY:
		case MISSION_BUILD:
		case MISSION_LEAD:
		//TAC Whaling, ray
		case MISSION_WHALING:
		case MISSION_FISHING:
		//End TAC Whaling, ray
			break;

		default:
			FAssert(false);
			break;
		}

		if ( bNotify )
		{
			NotifyEntity( headMissionQueueNode()->m_data.eMissionType );
		}

		CLLNode<IDInfo>* pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = getUnitNodeLoop(pUnitNode);

			if (pLoopUnit != NULL && pLoopUnit->canMove())
			{
				switch (headMissionQueueNode()->m_data.eMissionType)
				{
				case MISSION_MOVE_TO:
				case MISSION_ROUTE_TO:
				case MISSION_ROUTE_TO_ROAD:
				case MISSION_ROUTE_TO_COUNTRY_ROAD:
				case MISSION_MOVE_TO_UNIT:
				case MISSION_SKIP:
				case MISSION_SLEEP:
				case MISSION_FORTIFY:
				case MISSION_HEAL:
				case MISSION_SENTRY:
				case MISSION_BOMBARD:
				case MISSION_PILLAGE:
				case MISSION_FOUND:
				case MISSION_JOIN_CITY:
				case MISSION_BUILD:
				case MISSION_LEAD:
				//TAC Whaling, ray
				case MISSION_WHALING:
				case MISSION_FISHING:
				//End TAC Whaling, ray
					break;

				default:
					FAssert(false);
					break;
				}

				if (getNumUnits() == 0)
				{
					break;
				}

				if (headMissionQueueNode() == NULL)
				{
					break;
				}
			}
		}
	}

	if ((getNumUnits() > 0) && (headMissionQueueNode() != NULL))
	{
		if (false /* bAction */)
		{
			if (isHuman())
			{
				if (plot()->isVisibleToWatchingHuman())
				{
					updateMissionTimer();
				}
			}
		}

		if (!isBusy())
		{
			if (bDelete)
			{
				if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
				{
					if (IsSelected())
					{
						gDLL->getInterfaceIFace()->changeCycleSelectionCounter((GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_QUICK_MOVES)) ? 1 : 2);
					}
				}

				deleteMissionQueueNode(headMissionQueueNode());
			}
			else if (getActivityType() == ACTIVITY_MISSION)
			{
				continueMission();
			}
		}
	}
}


void CvSelectionGroup::continueMission(int iSteps)
{
	CvUnit* pTargetUnit;
	bool bDone;
	bool bAction;

	FAssert(!isBusy());
	FAssert(headMissionQueueNode() != NULL);
	FAssert(getOwnerINLINE() != NO_PLAYER);
	FAssert(getActivityType() == ACTIVITY_MISSION);

	if (headMissionQueueNode() == NULL)
	{
		// just in case...
		setActivityType(ACTIVITY_AWAKE);
		return;
	}

	bDone = false;
	bAction = false;

	if (headMissionQueueNode()->m_data.iPushTurn == GC.getGameINLINE().getGameTurn() || (headMissionQueueNode()->m_data.iFlags & MOVE_THROUGH_ENEMY))
	{
		if (headMissionQueueNode()->m_data.eMissionType == MISSION_MOVE_TO)
		{
			bool bFailedAlreadyFighting;
			if (groupAttack(CREATE_ASSERT_DATA, headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2, headMissionQueueNode()->m_data.iFlags, bFailedAlreadyFighting))
			{
				bDone = true;
			}
		}
	}

	// extra crash protection, should never happen (but a previous bug in groupAttack was causing a NULL here)
	// while that bug is fixed, no reason to not be a little more careful
	if (headMissionQueueNode() == NULL)
	{
		setActivityType(ACTIVITY_AWAKE);
		return;
	}

	if (!bDone)
	{
		if (getNumUnits() > 0)
		{
			if (canAllMove())
			{
				MissionTypes eMission = headMissionQueueNode()->m_data.eMissionType;
				int iMissionData = headMissionQueueNode()->m_data.iData1;
				CLLNode<IDInfo>* pUnitNode = headUnitNode();
				bool bBreak = false;
				while (pUnitNode != NULL && !bBreak)
				{
					CvUnit* pLoopUnit = getUnitNodeLoop(pUnitNode);

					if (pLoopUnit != NULL && pLoopUnit->canMove())
					{
						switch (eMission)
						{
						case MISSION_FOUND:
							if (pLoopUnit->found())
							{
								bAction = true;
								bBreak = true;
							}
							bDone = true;
							break;
						case MISSION_JOIN_CITY:
							if (pLoopUnit->joinCity())
							{
								bAction = true;
							}
							bDone = true;
							break;
						case MISSION_BOMBARD:
							if (pLoopUnit->bombard())
							{
								bAction = true;
							}
							bDone = true;
							break;

						case MISSION_PILLAGE:
							if (pLoopUnit->pillage())
							{
								bAction = true;
							}
							bDone = true;
							break;

						case MISSION_LEAD:
							if (pLoopUnit->lead(iMissionData))
							{
								bAction = true;
							}
							bDone = true;
							break;
						//TAC Whaling, ray
						case MISSION_WHALING:
							if (pLoopUnit->isWhalingBoat() && pLoopUnit->gatherResource())
							{
								//bAction = true;
								bDone = true;
							}
							break;
						//End TAC Whaling, ray
						// R&R, ray, High Sea Fishing - START
						case MISSION_FISHING:
							if (pLoopUnit->isFishingBoat() && pLoopUnit->gatherResource())
							{
								//bAction = true;
								bDone = true;
							}
							break;
						// R&R, ray, High Sea Fishing - END

						default:
							break;
						}
					}
				}

				if (!bDone && (headMissionQueueNode() != NULL))
				{
					switch (headMissionQueueNode()->m_data.eMissionType)
					{
					case MISSION_MOVE_TO:
						if (groupPathTo(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2, headMissionQueueNode()->m_data.iFlags))
						{
							bAction = true;
							if (getActivityType() != ACTIVITY_MISSION)  // might have unloaded from transport
							{
								bDone = true;
							}

							if (getNumUnits() > 0)
							{
								if (!canAllMove())
								{
									if (headMissionQueueNode() != NULL)
									{
										if (groupAmphibMove(GC.getMap().plotINLINE(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2), headMissionQueueNode()->m_data.iFlags))
										{
											bAction = false;
											bDone = true;
										}
									}
								}
							}
						}
						else
						{
							bDone = true;
						}
						break;

					case MISSION_ROUTE_TO:
						if (groupRouteTo(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2, headMissionQueueNode()->m_data.iFlags))
						{
							bAction = true;
						}
						else
						{
							bDone = true;
						}
						break;

					case MISSION_ROUTE_TO_ROAD:
						if (groupRouteTo(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2, headMissionQueueNode()->m_data.iFlags, ROUTE_ROAD))
						{
							bAction = true;
						}
						else
						{
							bDone = true;
						}
						break;

					case MISSION_ROUTE_TO_COUNTRY_ROAD:
						if (groupRouteTo(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2, headMissionQueueNode()->m_data.iFlags, ROUTE_COUNTRY_ROAD))
						{
							bAction = true;
						}
						else
						{
							bDone = true;
						}
						break;


					case MISSION_MOVE_TO_UNIT:
						pTargetUnit = GET_PLAYER((PlayerTypes)headMissionQueueNode()->m_data.iData1).getUnit(headMissionQueueNode()->m_data.iData2);
						if (pTargetUnit != NULL)
						{
							if (AI_getMissionAIType() != MISSIONAI_SHADOW && AI_getMissionAIType() != MISSIONAI_GROUP)
							{
								if (!plot()->isOwned() || plot()->getOwnerINLINE() == getOwnerINLINE())
								{
									CvPlot* pMissionPlot = pTargetUnit->getGroup()->AI_getMissionAIPlot();
									if (pMissionPlot != NULL && NO_TEAM != pMissionPlot->getTeam())
									{
										if (pMissionPlot->isOwned() && pTargetUnit->isPotentialEnemy(pMissionPlot->getTeam(), pMissionPlot))
										{
											bAction = false;
											bDone = true;
											break;
										}
									}
								}
							}

							if (groupPathTo(pTargetUnit->getX_INLINE(), pTargetUnit->getY_INLINE(), headMissionQueueNode()->m_data.iFlags))
							{
								bAction = true;
							}
							else
							{
								bDone = true;
							}
						}
						else
						{
							bDone = true;
						}
						break;

					case MISSION_SKIP:
					case MISSION_SLEEP:
					case MISSION_FORTIFY:
					case MISSION_HEAL:
					case MISSION_SENTRY:
						FAssert(false);
						break;

					case MISSION_BOMBARD:
					case MISSION_PILLAGE:
					case MISSION_FOUND:
					case MISSION_JOIN_CITY:
					case MISSION_LEAD:
					//TAC Whaling, ray
					case MISSION_WHALING:
					case MISSION_FISHING:
					//End TAC Whaling, ray
						break;

					case MISSION_BUILD:
						if (!groupBuild(headMissionQueueNode()->m_data.eBuild))
						{
							bDone = true;
						}
						break;

					default:
						FAssert(false);
						break;
					}
				}
			}
		}
	}

	if ((getNumUnits() > 0) && (headMissionQueueNode() != NULL))
	{
		if (!bDone)
		{
			switch (headMissionQueueNode()->m_data.eMissionType)
			{
			case MISSION_MOVE_TO:
				headMissionQueueNode()->m_data.iFlags |= MOVE_HAS_STEPPED; // K-Mod
				if (at(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
				{
					bDone = true;
				}
				break;

			case MISSION_ROUTE_TO:
				if (at(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
				{
					if (!canBuildRoute(plot()))
					{
						bDone = true;
					}
				}
				break;

			case MISSION_ROUTE_TO_ROAD:
				if (at(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
				{
					if (!canBuildRoute(plot(), ROUTE_ROAD))
					{
						bDone = true;
					}
				}
				break;

			case MISSION_ROUTE_TO_COUNTRY_ROAD:
				if (at(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
				{
					if (!canBuildRoute(plot(), ROUTE_COUNTRY_ROAD))
					{
						bDone = true;
					}
				}
				break;

			case MISSION_MOVE_TO_UNIT:
				pTargetUnit = GET_PLAYER((PlayerTypes)headMissionQueueNode()->m_data.iData1).getUnit(headMissionQueueNode()->m_data.iData2);
				if ((pTargetUnit == NULL) || atPlot(pTargetUnit->plot()))
				{
					bDone = true;
				}
				break;

			case MISSION_SKIP:
			case MISSION_SLEEP:
			case MISSION_FORTIFY:
			case MISSION_HEAL:
			case MISSION_SENTRY:
				FAssert(false);
				break;

			case MISSION_BOMBARD:
			case MISSION_LEAD:
			case MISSION_PILLAGE:
			case MISSION_FOUND:
			case MISSION_JOIN_CITY:
				break;

			case MISSION_BUILD:
				// XXX what happens if two separate worker groups are both building the mine...
				/*if (plot()->getBuildType() != ((BuildTypes)(headMissionQueueNode()->m_data.iData1)))
				{
					bDone = true;
				}*/

			case MISSION_WHALING: //TAC Whaling, ray
			case MISSION_FISHING: // R&R, ray, High Sea Fishing
				break;

			default:
				FAssert(false);
				break;
			}
		}
	}

	if ((getNumUnits() > 0) && (headMissionQueueNode() != NULL))
	{
		if (bAction)
		{
			if (bDone || !canAllMove())
			{
				if (plot()->isVisibleToWatchingHuman())
				{
					updateMissionTimer(iSteps);

					if (showMoves())
					{
						if (GC.getGameINLINE().getActivePlayer() != NO_PLAYER)
						{
							if (getOwnerINLINE() != GC.getGameINLINE().getActivePlayer())
							{
								if (plot()->isActiveVisible(false) && !isInvisible(GC.getGameINLINE().getActiveTeam()))
								{
									gDLL->getInterfaceIFace()->lookAt(plot()->getPoint(), CAMERALOOKAT_NORMAL);
								}
							}
						}
					}
				}
			}
		}

		if (bDone)
		{
			if (!isBusy())
			{
				if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
				{
					if (IsSelected())
					{
						if ((headMissionQueueNode()->m_data.eMissionType == MISSION_MOVE_TO) ||
							(headMissionQueueNode()->m_data.eMissionType == MISSION_ROUTE_TO) ||
							(headMissionQueueNode()->m_data.eMissionType == MISSION_ROUTE_TO_ROAD) ||
							(headMissionQueueNode()->m_data.eMissionType == MISSION_ROUTE_TO_COUNTRY_ROAD) ||
							(headMissionQueueNode()->m_data.eMissionType == MISSION_MOVE_TO_UNIT))
						{
							gDLL->getInterfaceIFace()->changeCycleSelectionCounter((GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_QUICK_MOVES)) ? 1 : 2);
						}
					}
				}

				deleteMissionQueueNode(headMissionQueueNode());
			}
		}
		else
		{
			if (canAllMove())
			{
				continueMission(iSteps + 1);
			}
			else if (!isBusy())
			{
				if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
				{
					if (IsSelected())
					{
						gDLL->getInterfaceIFace()->changeCycleSelectionCounter(1);
					}
				}
			}
		}
	}
}


bool CvSelectionGroup::canDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible, bool bUseCache)
{
	PROFILE_FUNC();

	//cache isBusy
	if(bUseCache)
	{
		if(m_bIsBusyCache)
		{
			return false;
		}
	}
	else
	{
		if (isBusy())
		{
			return false;
		}
	}

	if(!canEverDoCommand(eCommand, iData1, iData2, bTestVisible, bUseCache))
		return false;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = getUnitNodeLoop(pUnitNode);

		if (pLoopUnit != NULL && pLoopUnit->canDoCommand(eCommand, iData1, iData2, bTestVisible, false))
		{
			return true;
		}
	}

	return false;
}

bool CvSelectionGroup::canEverDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible, bool bUseCache)
{
	if(eCommand == COMMAND_LOAD)
	{
		CLLNode<IDInfo>* pUnitNode = plot()->headUnitNode();

		while (pUnitNode != NULL)
		{
			CvUnit *pLoopUnit = getUnitNodeLoop(pUnitNode);

			if (pLoopUnit != NULL && !pLoopUnit->isFull())
			{
				return true;
			}
		}

		//no cargo space on this plot
		return false;
	}
	else if(eCommand == COMMAND_UNLOAD)
	{
		CLLNode<IDInfo>* pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			CvUnit *pLoopUnit = getUnitNodeLoop(pUnitNode);

			if (pLoopUnit != NULL && pLoopUnit->isCargo())
			{
				return true;
			}
		}

		//no loaded unit
		return false;
	}
	else if(eCommand == COMMAND_UPGRADE)
	{
		if(bUseCache)
		{
			//see if any of the different units can upgrade to this unit type
			for(int i=0;i<(int)m_aDifferentUnitCache.size();i++)
			{
				CvUnit *unit = m_aDifferentUnitCache[i];
				if(unit->canDoCommand(eCommand, iData1, iData2, bTestVisible, false))
					return true;
			}

			return false;
		}
	}
	// Erik: Coastal transports cannot travel to any foreign port
	else if (eCommand == COMMAND_SAIL_TO_AFRICA)
	{
		CLLNode<IDInfo>* pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			CvUnit *pLoopUnit = getUnitNodeLoop(pUnitNode);

			// TODO: Check against the ability to cross ocean plots instead
			if (pLoopUnit != NULL && pLoopUnit->getUnitInfo().getTerrainImpassable(TERRAIN_OCEAN))
			{
				return false;
			}
		}
	}

	return true;
}

void CvSelectionGroup::setupActionCache()
{
	//cache busy calculation
	m_bIsBusyCache = isBusy();

    //cache different unit types
	m_aDifferentUnitCache.erase(m_aDifferentUnitCache.begin(), m_aDifferentUnitCache.end());
	CLLNode<IDInfo> *pUnitNode = headUnitNode();
	while(pUnitNode != NULL)
	{
		CvUnit *unit = getUnitNodeLoop(pUnitNode);

		if (unit != NULL && unit->isReadyForUpgrade())
		{
			UnitTypes unitType = unit->getUnitType();
			bool bFound = false;
			for(int i=0;i<(int)m_aDifferentUnitCache.size();i++)
			{
				if(unitType == m_aDifferentUnitCache[i]->getUnitType())
				{
					bFound = true;
					break;
				}
			}

			if(!bFound)
				m_aDifferentUnitCache.push_back(unit);
		}
	}
}

// Returns true if one of the units can support the interface mode...
bool CvSelectionGroup::canDoInterfaceMode(InterfaceModeTypes eInterfaceMode)
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;

	FAssertMsg(eInterfaceMode != NO_INTERFACEMODE, "InterfaceMode is not assigned a valid value");

	if (isBusy())
	{
		return false;
	}

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = getUnitNodeLoop(pUnitNode);

		if (pLoopUnit == NULL)
		{
			continue;
		}

		switch (eInterfaceMode)
		{
		case INTERFACEMODE_GO_TO:
			if (getDomainType() != DOMAIN_IMMOBILE)
			{
				return true;
			}
			break;

		case INTERFACEMODE_GO_TO_TYPE:
			if (getDomainType() != DOMAIN_IMMOBILE)
			{
				if (pLoopUnit->plot()->plotCount(PUF_isUnitType, pLoopUnit->getUnitType(), -1, pLoopUnit->getOwnerINLINE()) > 1)
				{
					return true;
				}
			}
			break;

		case INTERFACEMODE_GO_TO_ALL:
			if (getDomainType() != DOMAIN_IMMOBILE)
			{
				if (pLoopUnit->plot()->plotCount(NULL, -1, -1, pLoopUnit->getOwnerINLINE()) > 1)
				{
					return true;
				}
			}
			break;

		case INTERFACEMODE_ROUTE_TO:
			if (pLoopUnit->workRate(true) > 0)
			{
				if (pLoopUnit->canBuildRoute())
				{
					return true;
				}
			}
			break;

		case INTERFACEMODE_ROUTE_TO_ROAD:
			if (pLoopUnit->workRate(true) > 0)
			{
				if (pLoopUnit->canBuildRoute(ROUTE_ROAD))
				{
					return true;
				}
			}
			break;

		case INTERFACEMODE_ROUTE_TO_COUNTRY_ROAD:
			if (pLoopUnit->workRate(true) > 0)
			{
				if (pLoopUnit->canBuildRoute(ROUTE_COUNTRY_ROAD))
				{
					return true;
				}
			}
			break;
		}
	}

	return false;
}


// Returns true if one of the units can execute the interface mode at the specified plot...
bool CvSelectionGroup::canDoInterfaceModeAt(InterfaceModeTypes eInterfaceMode, CvPlot* pPlot)
{
	FAssertMsg(eInterfaceMode != NO_INTERFACEMODE, "InterfaceMode is not assigned a valid value");

	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	if (pUnitNode != NULL)
	{
		return true;
	}

	return false;
}


bool CvSelectionGroup::isHuman() const
{
	if (getOwnerINLINE() != NO_PLAYER)
	{
		return GET_PLAYER(getOwnerINLINE()).isHuman();
	}

	return true;
}


bool CvSelectionGroup::isBusy()
{
	if (getNumUnits() == 0)
	{
		return false;
	}

	if (getMissionTimer() > 0)
	{
		return true;
	}

	CvPlot* pPlot = plot();

	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = getUnitNodeLoop(pUnitNode);

		if (pLoopUnit != NULL)
		{
			if (pLoopUnit->isCombat())
			{
				return true;
			}
		}
	}

	return false;
}


bool CvSelectionGroup::isCargoBusy()
{
	if (getNumUnits() == 0)
	{
		return false;
	}

	CvPlot* pPlot = plot();

	CLLNode<IDInfo>* pUnitNode1 = headUnitNode();

	while (pUnitNode1 != NULL)
	{
		CvUnit* pLoopUnit1 = getUnitNodeLoop(pUnitNode1);

		if (pLoopUnit1 != NULL)
		{
			if (pLoopUnit1->getCargo() > 0)
			{
				CLLNode<IDInfo>* pUnitNode2 = pPlot->headUnitNode();

				while (pUnitNode2 != NULL)
				{
					CvUnit* pLoopUnit2 = getUnitNodeLoop(pUnitNode2);

					if (pLoopUnit2 != NULL && pLoopUnit2->getTransportUnit() == pLoopUnit1)
					{
						if (pLoopUnit2->getGroup()->isBusy())
						{
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}


int CvSelectionGroup::baseMoves()
{
	int iBestValue = MAX_INT;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = getUnitNodeLoop(pUnitNode);

		if (pLoopUnit == NULL)
		{
			continue;
		}

		int iValue = pLoopUnit->baseMoves();

		if (iValue < iBestValue)
		{
			iBestValue = iValue;
		}
	}

	return iBestValue;
}


bool CvSelectionGroup::isWaiting() const
{
	switch (getActivityType())
	{
	case ACTIVITY_HOLD:
	case ACTIVITY_SLEEP:
	case ACTIVITY_HEAL:
	case ACTIVITY_SENTRY:
		return true;
	default:
		break;
	}

	return false;
}


bool CvSelectionGroup::isFull()
{
	CLLNode<IDInfo>* pUnitNode;

	if (getNumUnits() > 0)
	{
		// do two passes, the first pass, we ignore units with special cargo
		int iSpecialCargoCount = 0;
		int iCargoCount = 0;

		// first pass, count but ignore special cargo units
		pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = getUnitNodeLoop(pUnitNode);

			if (pLoopUnit != NULL)
			{
				if (pLoopUnit->cargoSpace() > 0)
				{
					iCargoCount++;
				}

				if (pLoopUnit->specialCargo() != NO_SPECIALUNIT)
				{
					iSpecialCargoCount++;
				}
				else if (!(pLoopUnit->isFull()))
				{
					return false;
				}
			}
		}

		// if every unit in the group has special cargo, then check those, otherwise, consider ourselves full
		if (iSpecialCargoCount >= iCargoCount)
		{
			pUnitNode = headUnitNode();
			while (pUnitNode != NULL)
			{
				CvUnit* pLoopUnit = getUnitNodeLoop(pUnitNode);

				if (pLoopUnit != NULL && !(pLoopUnit->isFull()))
				{
					return false;
				}
			}
		}

		return true;
	}

	return false;
}


bool CvSelectionGroup::hasCargo() const
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = getUnitNodeLoop(pUnitNode);

		if (pLoopUnit != NULL && pLoopUnit->hasCargo())
		{
			return true;
		}
	}

	return false;
}

int CvSelectionGroup::getCargo() const
{
	int iCargoCount = 0;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = getUnitNodeLoop(pUnitNode);

		if (pLoopUnit != NULL)
		{
			iCargoCount += pLoopUnit->getCargo();
		}
	}

	return iCargoCount;
}

// adds every unit on this plot which is cargo of our group to unitList
bool CvSelectionGroup::buildCargoUnitList(CLinkList<IDInfo>& unitList) const
{
	bool bUnitAdded = false;

	if (getNumUnits() > 0)
	{
		// special case the quick check
		if (getNumUnits() == 1 && !getHeadUnit()->hasCargo())
		{
			return false;
		}

		CvPlot* pPlot = plot();
		if (pPlot != NULL)
		{

			CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
			while (pUnitNode != NULL)
			{
				CvUnit* pCargoUnit = pPlot->getUnitNodeLoop(pUnitNode);

				if (pCargoUnit != NULL)
				{
					CvUnit* pTransportUnit = pCargoUnit->getTransportUnit();
					if (pTransportUnit != NULL && pTransportUnit->getGroup() == this)
					{
						unitList.insertAtEnd(pCargoUnit->getIDInfo());
						bUnitAdded = true;
					}
				}
			}
		}
	}

	return bUnitAdded;
}

bool CvSelectionGroup::canAllMove()
{

	if (getNumUnits() > 0)
	{
		CLLNode<IDInfo>* pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = getUnitNodeLoop(pUnitNode);

			FAssertMsg(pLoopUnit != NULL, "existing node, but NULL unit");

			if (pLoopUnit != NULL && !(pLoopUnit->canMove()))
			{
				return false;
			}
		}

		return true;
	}

	return false;
}


bool CvSelectionGroup::canAnyMove()
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);

		if (pLoopUnit != NULL && pLoopUnit->canMove())
		{
			return true;
		}
		pUnitNode = nextUnitNode(pUnitNode);
	}

	return false;
}

bool CvSelectionGroup::hasMoved()
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);

		if (pLoopUnit != NULL && pLoopUnit->hasMoved())
		{
			return true;
		}
		pUnitNode = nextUnitNode(pUnitNode);
	}

	return false;
}


bool CvSelectionGroup::canEnterTerritory(PlayerTypes ePlayer, bool bIgnoreRightOfPassage) const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	if (getNumUnits() > 0)
	{
		pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);

			if (pLoopUnit != NULL && !(pLoopUnit->canEnterTerritory(ePlayer, bIgnoreRightOfPassage)))
			{
				return false;
			}
			pUnitNode = nextUnitNode(pUnitNode);
		}

		return true;
	}

	return false;
}

bool CvSelectionGroup::canEnterArea(PlayerTypes ePlayer, const CvArea* pArea, bool bIgnoreRightOfPassage) const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	if (getNumUnits() > 0)
	{
		pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);

			if (pLoopUnit != NULL && !(pLoopUnit->canEnterArea(ePlayer, pArea, bIgnoreRightOfPassage)))
			{
				return false;
			}
			pUnitNode = nextUnitNode(pUnitNode);
		}

		return true;
	}

	return false;
}


bool CvSelectionGroup::canMoveInto(CvPlot* pPlot, bool bAttack)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	if (getNumUnits() > 0)
	{
		pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);

			if (pLoopUnit != NULL && pLoopUnit->canMoveInto(*pPlot, bAttack))
			{
				return true;
			}
			pUnitNode = nextUnitNode(pUnitNode);
		}
	}

	return false;
}

bool CvSelectionGroup::canMoveOrAttackInto(CvPlot* pPlot, bool bDeclareWar)
{
	// K-Mod. (hack to avoid breaking the DllExport) advc: 2x const, CvPlot&
	return canMoveOrAttackInto(*pPlot, bDeclareWar, false);
}

bool CvSelectionGroup::canMoveOrAttackInto(CvPlot const& kPlot, bool bDeclareWar,
	bool bCheckMoves, bool bAssumeVisible) const // K-Mod
{
	if (getNumUnits() <= 0)
		return false;

	bool bVisible = bAssumeVisible || kPlot.isVisible(getHeadTeam(), false); // K-Mod
	for (CLLNode<IDInfo> const* pUnitNode = headUnitNode(); pUnitNode != NULL;
		pUnitNode = nextUnitNode(pUnitNode))
	{
		CvUnit const* pLoopUnit = ::getUnit(pUnitNode->m_data);
		//if (pLoopUnit->canMoveOrAttackInto(pPlot, bDeclareWar))
		if (pLoopUnit != NULL && 
			(!bCheckMoves || pLoopUnit->canMove()) && // K-Mod
			(bVisible ? pLoopUnit->canMoveOrAttackInto(&kPlot, bDeclareWar) :
				pLoopUnit->canMoveInto(kPlot, false, bDeclareWar, false/*, false*/)))
		{
			return true;
		}
	}
	return false;
}


bool CvSelectionGroup::canMoveThrough(CvPlot const& kPlot, bool bDeclareWar, bool bAssumeVisible) const
{
	if (getNumUnits() <= 0)
		return false;

	for (CLLNode<IDInfo> const* pUnitNode = headUnitNode(); pUnitNode != NULL;
		pUnitNode = nextUnitNode(pUnitNode))
	{
		CvUnit const* pLoopUnit = ::getUnit(pUnitNode->m_data);
		//if (!pLoopUnit->canMoveThrough(kPlot))
		if (pLoopUnit != NULL && !pLoopUnit->canMoveInto(kPlot, false, bDeclareWar, true/*, bAssumeVisible*/)) // K-Mod
		{
			return false;
		}
	}
	return true;
}


bool CvSelectionGroup::canFight() const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);

		if (pLoopUnit != NULL && pLoopUnit->canFight())
		{
			return true;
		}
		pUnitNode = nextUnitNode(pUnitNode);
	}

	return false;
}


bool CvSelectionGroup::canDefend()
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);

		if (pLoopUnit != NULL && pLoopUnit->canDefend())
		{
			return true;
		}
		pUnitNode = nextUnitNode(pUnitNode);
	}

	return false;
}

bool CvSelectionGroup::canBombard(const CvPlot* pPlot)
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);

		if (pLoopUnit != NULL && pLoopUnit->canBombard(pPlot))
		{
			return true;
		}
		pUnitNode = nextUnitNode(pUnitNode);
	}

	return false;
}

bool CvSelectionGroup::visibilityRange()
{
	int iMaxRange = 0;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);

		if (pLoopUnit != NULL)
		{
			int iRange = pLoopUnit->visibilityRange();
			if (iRange > iMaxRange)
			{
				iMaxRange = iRange;
			}
		}
		pUnitNode = nextUnitNode(pUnitNode);
	}

	return iMaxRange;
}

// TAC - AI Attack City - koma13, jdog5000(BBAI) - START
//
// Approximate how many turns this group would take to reduce pCity's defense modifier to zero
//
int CvSelectionGroup::getBombardTurns(CvCity* pCity)
{
	PROFILE_FUNC();

	//bool bHasBomber = (getOwnerINLINE() != NO_PLAYER ? (GET_PLAYER(getOwnerINLINE()).AI_calculateTotalBombard(DOMAIN_AIR) > 0) : false);
	bool bHasBomber = false;
	bool bIgnoreBuildingDefense = bHasBomber;
	int iTotalBombardRate = (bHasBomber ? 16 : 0);
	int iUnitBombardRate = 0;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit != NULL && pLoopUnit->bombardRate() > 0)
		{
			iUnitBombardRate = pLoopUnit->bombardRate();

			/*
			if( pLoopUnit->ignoreBuildingDefense() )
			{
				bIgnoreBuildingDefense = true;
			}
			else
			*/
			{
				iUnitBombardRate *= std::max(25, (100 - pCity->getBuildingBombardDefense()));
				iUnitBombardRate /= 100;
			}

			iTotalBombardRate += iUnitBombardRate;
		}
	}


	//if( pCity->getTotalDefense(bIgnoreBuildingDefense) == 0 )
	if( pCity->getTotalDefense() == 0 )
	{
		return 0;
	}

	//int iBombardTurns = pCity->getTotalDefense(bIgnoreBuildingDefense);
	int iBombardTurns = pCity->getTotalDefense();

	if( iTotalBombardRate > 0 )
	{
		iBombardTurns = (GC.getMAX_CITY_DEFENSE_DAMAGE() - pCity->getDefenseDamage());
		//iBombardTurns *= pCity->getTotalDefense(false);
		iBombardTurns *= pCity->getTotalDefense();
		iBombardTurns += (GC.getMAX_CITY_DEFENSE_DAMAGE() * iTotalBombardRate) - 1;
		iBombardTurns /= std::max(1, (GC.getMAX_CITY_DEFENSE_DAMAGE() * iTotalBombardRate));
	}

	//if( gUnitLogLevel > 2 ) logBBAI("      Bombard of %S will take %d turns at rate %d and current damage %d with bombard def %d", pCity->getName().GetCString(), iBombardTurns, iTotalBombardRate, pCity->getDefenseDamage(), (bIgnoreBuildingDefense ? 0 : pCity->getBuildingBombardDefense()));

	return iBombardTurns;
}
// TAC - AI Attack City - koma13, jdog5000(BBAI) - END

bool CvSelectionGroup::isPromotionReady() const
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		if (pLoopUnit != NULL && pLoopUnit->isPromotionReady())
		{
			return true;
		}
		pUnitNode = nextUnitNode(pUnitNode);
	}

	return false;
}

bool CvSelectionGroup::canChangeProfession() const
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		if (pLoopUnit != NULL)
		{
			for (ProfessionTypes eProfession = FIRST_PROFESSION; eProfession < NUM_PROFESSION_TYPES; ++eProfession)
			{
				if (eProfession != pLoopUnit->getProfession() && pLoopUnit->canHaveProfession(eProfession, false))
				{
					return true;
				}
			}
		}
		pUnitNode = nextUnitNode(pUnitNode);
	}

	return false;
}

void CvSelectionGroup::unloadAll()
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit != NULL)
		{
			pLoopUnit->unloadAll();
		}
	}
}

bool CvSelectionGroup::alwaysInvisible() const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	if (getNumUnits() > 0)
	{
		pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);

			if (pLoopUnit != NULL && !(pLoopUnit->alwaysInvisible()))
			{
				return false;
			}
			pUnitNode = nextUnitNode(pUnitNode);
		}

		return true;
	}

	return false;
}


bool CvSelectionGroup::isInvisible(TeamTypes eTeam) const
{
	if (getNumUnits() > 0)
	{
		CLLNode<IDInfo>* pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);

			if (pLoopUnit != NULL && !pLoopUnit->isInvisible(eTeam, false))
			{
				return false;
			}
			pUnitNode = nextUnitNode(pUnitNode);
		}

		return true;
	}

	return false;
}


int CvSelectionGroup::countNumUnitAIType(UnitAITypes eUnitAI)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int iCount;

	FAssertMsg(headUnitNode() != NULL, "headUnitNode() is not expected to be equal with NULL");

	iCount = 0;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit == NULL)
		{
			continue;
		}

		// count all units if NO_UNITAI passed in
		if (NO_UNITAI == eUnitAI || pLoopUnit->AI_getUnitAIType() == eUnitAI)
		{
			iCount++;
		}
	}

	return iCount;
}

bool CvSelectionGroup::IsSelected()
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);

		if (pLoopUnit != NULL && pLoopUnit->IsSelected())
		{
			return true;
		}
		pUnitNode = nextUnitNode(pUnitNode);
	}

	return false;
}


void CvSelectionGroup::NotifyEntity(MissionTypes eMission)
{
	CLLNode<IDInfo>* pUnitNode;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		if (pLoopUnit != NULL)
		{
			pLoopUnit->NotifyEntity(eMission);
		}
		pUnitNode = nextUnitNode(pUnitNode);
	}
}


int CvSelectionGroup::getX() const
{
	CvUnit* pHeadUnit;

	pHeadUnit = getHeadUnit();

	if (pHeadUnit != NULL)
	{
		return getHeadUnit()->getX_INLINE();
	}
	else
	{
		return INVALID_PLOT_COORD;
	}
}


int CvSelectionGroup::getY() const
{
	CvUnit* pHeadUnit;

	pHeadUnit = getHeadUnit();

	if (pHeadUnit != NULL)
	{
		return getHeadUnit()->getY_INLINE();
	}
	else
	{
		return INVALID_PLOT_COORD;
	}
}


bool CvSelectionGroup::at(int iX, int iY) const
{
	return((getX() == iX) && (getY() == iY));
}


bool CvSelectionGroup::atPlot( const CvPlot* pPlot) const
{
	return (plot() == pPlot);
}


CvPlot* CvSelectionGroup::plot() const
{
	CvUnit* pHeadUnit;

	pHeadUnit = getHeadUnit();

	if (pHeadUnit != NULL)
	{
		return getHeadUnit()->plot();
	}
	else
	{
		return NULL;
	}
}


int CvSelectionGroup::getArea() const
{
	CvUnit* pHeadUnit;

	pHeadUnit = getHeadUnit();

	if (pHeadUnit != NULL)
	{
		return getHeadUnit()->getArea();
	}
	else
	{
		return NULL;
	}
}

CvArea* CvSelectionGroup::area() const
{
	CvUnit* pHeadUnit;

	pHeadUnit = getHeadUnit();

	if (pHeadUnit != NULL)
	{
		return getHeadUnit()->area();
	}
	else
	{
		return NULL;
	}
}


DomainTypes CvSelectionGroup::getDomainType() const
{
	CvUnit* pHeadUnit;

	pHeadUnit = getHeadUnit();

	if (pHeadUnit != NULL)
	{
		return getHeadUnit()->getDomainType();
	}
	else
	{
		return NO_DOMAIN;
	}
}


bool CvSelectionGroup::canBuildRoute(CvPlot* pPlot, RouteTypes ePreferredRoute) const
{
// if ePreferredRoute == NO_ROUTE, then return true for any route that can be built!

	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		const CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit == NULL)
		{
			continue;
		}

		for (BuildTypes eLoopBuild = FIRST_BUILD; eLoopBuild < NUM_BUILD_TYPES; eLoopBuild++)
		{
			if (GC.getBuildInfo(eLoopBuild).isRoute())
			{
				if (ePreferredRoute == NO_ROUTE && pLoopUnit->canBuild(pPlot, eLoopBuild))
				{
					// there is a route type that can be built
					return true;
				}
				if (GC.getBuildInfo(eLoopBuild).getRoute() == ePreferredRoute && pLoopUnit->canBuild(pPlot, eLoopBuild))
				{
					// the reqeusted route type can be built
					return true;
				}
			}
		}
	}
	// we have not found a buildable route type
	return false;
}


BuildTypes CvSelectionGroup::getBestBuildRouteBuild(CvPlot *pPlot, RouteTypes ePreferredRoute) const
{
	// if ePreferredRoute is NO_ROUTE, this will return the best available one
	// if ePreferredRoute is a specific route type, this function will return it if it can be build by this group
	BuildTypes eBuild;
	getBestBuildRoute(pPlot, &eBuild, ePreferredRoute);
	return eBuild;
}


RouteTypes CvSelectionGroup::getBestBuildRoute(CvPlot* pPlot, BuildTypes* peBestBuild, RouteTypes ePreferredRoute) const
{
	PROFILE_FUNC();

	RouteTypes eBestRoute = NO_ROUTE;
	int iBestValue = 0;
	if (peBestBuild != NULL)
	{
		*peBestBuild = NO_BUILD;
	}

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		const CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit == NULL)
		{
			continue;
		}

		for (BuildTypes eLoopBuild = FIRST_BUILD; eLoopBuild < NUM_BUILD_TYPES; eLoopBuild++)
		{
			if (GC.getBuildInfo(eLoopBuild).isRoute())
			{
				if(ePreferredRoute == NO_ROUTE)
				{
					if (pLoopUnit->canBuild(pPlot, eLoopBuild))
					{
						RouteTypes eRoute = ((RouteTypes)(GC.getBuildInfo(eLoopBuild).getRoute()));

						int iValue = GC.getRouteInfo(eRoute).getValue();

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							eBestRoute = eRoute;
							if (peBestBuild != NULL)
							{
								*peBestBuild = eLoopBuild;
							}
						}
					}
				}
				else // ePreferredRoute is a specific route type!
				{
					RouteTypes eRoute = ((RouteTypes)(GC.getBuildInfo(eLoopBuild).getRoute()));
					if ((eRoute == ePreferredRoute ||
							GC.getRouteInfo(ePreferredRoute).getValue() > GC.getRouteInfo(eRoute).getValue() ||
							(eRoute == ROUTE_RAFT_STATION))
							&& pLoopUnit->canBuild(pPlot, eLoopBuild))
					{
						// if a certain route type was requested and we have found a unit to build it, just return that type
						if (peBestBuild != NULL)
						{
							*peBestBuild = eLoopBuild;
						}
						return eRoute;
					}
				}
			}
		}
	}
	return eBestRoute;
}


// Returns true if group was bumped...
bool CvSelectionGroup::groupDeclareWar(CvPlot* pPlot, bool bForce)
{
	CvTeamAI& kTeam = GET_TEAM(getTeam());
	TeamTypes ePlotTeam = pPlot->getTeam();

	if (!AI_isDeclareWar(pPlot))
	{
		return false;
	}

	int iNumUnits = getNumUnits();

	if (bForce || !canEnterArea(pPlot->getOwnerINLINE(), pPlot->area(), true))
	{
		if (ePlotTeam != NO_TEAM && kTeam.AI_isSneakAttackReady(ePlotTeam))
		{
			if (kTeam.canDeclareWar(ePlotTeam))
			{
				kTeam.declareWar(ePlotTeam, true, NO_WARPLAN);
			}
		}
	}

	return (iNumUnits != getNumUnits());
}

// R&R, ray, Natives raiding party - START
// R&R, ray, Heavily modified, use complete method
// Returns true if attack was made...
bool CvSelectionGroup::groupAttack(AssertCallerData assertData, int iX, int iY, int iFlags, bool& bFailedAlreadyFighting)
{
	FAssertWithCaller(assertData, !isBusy()); // K-Mod

	CvPlot* pDestPlot = GC.getMap().plot(iX, iY);

	// K-Mod. Rather than clearing the existing path data; use a temporary pathfinder.
	KmodPathFinder final_path;
	final_path.SetSettings(this, iFlags & ~MOVE_DECLARE_WAR);
	/*if (iFlags & MOVE_THROUGH_ENEMY) {
	if (generatePath(plot(), pDestPlot, iFlags))
	pDestPlot = getPathFirstPlot();
	}*/ // BtS
	// K-Mod
	if (iFlags & (MOVE_THROUGH_ENEMY | MOVE_ATTACK_STACK) && !(iFlags & MOVE_DIRECT_ATTACK))
	{
		if (final_path.GeneratePath(pDestPlot))
		{
			pDestPlot = final_path.GetPathFirstPlot();
		}
	} // K-Mod end

	FAssertMsg(pDestPlot != NULL, "DestPlot is not assigned a valid value");

	if (getNumUnits() <= 0)
		return false; // advc

	if (/*getDomainType0) != DOMAIN_AIR && */stepDistance(getX(), getY(), pDestPlot->getX(), pDestPlot->getY()) != 1)
		return false; // advc

	bool bStack = (isHuman() && GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_STACK_ATTACK));
	bool bAttack = false;
	// R&R, ray, Natives raiding party
	bool bForceNoWar = (iFlags & MOVE_DIRECT_RAID);
	// R&R, ray, Natives raiding party

	bFailedAlreadyFighting = false;

	if (getNumUnits() > 0)
	{
		//if (stepDistance(getX(), getY(), pDestPlot->getX_INLINE(), pDestPlot->getY_INLINE()) == 1)
		{
			//if ((iFlags & MOVE_DIRECT_ATTACK) || (iFlags & MOVE_DIRECT_RAID) || (generatePath(plot(), pDestPlot, iFlags) && (getPathFirstPlot() == pDestPlot)))
			// K-Mod.
			if (iFlags & (MOVE_THROUGH_ENEMY | MOVE_ATTACK_STACK | MOVE_DIRECT_ATTACK | MOVE_DIRECT_RAID) ||
				/*getDomainType() == DOMAIN_AIR ||*/ (final_path.GeneratePath(pDestPlot) &&
					final_path.GetPathFirstPlot() == pDestPlot)) // K-Mod end
			{
				// R&R, ray, Natives raiding party
				int iAttackOdds;
				//CvUnit* pBestAttackUnit = AI_getBestGroupAttacker(pDestPlot, true, iAttackOdds);
				CvUnit* pBestAttackUnit = AI_getBestGroupAttacker(pDestPlot, true, iAttackOdds, bForceNoWar);
				// R&R, ray, Natives raiding party

				if (pBestAttackUnit)
				{
					// R&R, ray, Natives raiding party
					//CvUnit* pBestDefender = pDestPlot->getBestDefender(NO_PLAYER, getOwnerINLINE(), pBestAttackUnit, true);
					CvUnit* pBestDefender = pDestPlot->getBestDefender(NO_PLAYER, getOwnerINLINE(), pBestAttackUnit, !bForceNoWar);
					// R&R, ray, Natives raiding party

					//eject city defender if better
					CvCity* pCity = pDestPlot->getPlotCity();
					if (pCity != NULL)
					{
						pBestDefender = pCity->ejectBestDefender(pBestDefender, pBestAttackUnit);
					}

					// if there are no defenders, do not attack
					if (NULL == pBestDefender)
					{
						return false;
					}

					bool bNoBlitz = (!pBestAttackUnit->isBlitz() || !pBestAttackUnit->isMadeAttack());

					// R&R, ray, Natives raiding party
					if (!bForceNoWar)
					{
						if (groupDeclareWar(pDestPlot))
						{
							return true;
						}
					}
					// R&R, ray, Natives raiding party

					while (true)
					{
						// R&R, ray, Natives raiding party
						//pBestAttackUnit = AI_getBestGroupAttacker(pDestPlot, false, iAttackOdds, false, bNoBlitz);
						pBestAttackUnit = AI_getBestGroupAttacker(pDestPlot, false, iAttackOdds, bForceNoWar, bNoBlitz);
						// R&R, ray, Natives raiding party

						if (pBestAttackUnit == NULL)
						{
							break;
						}

						if (iAttackOdds < 68)
						{
							// R&R, ray, Natives raiding party
							//CvUnit * pBestSacrifice = AI_getBestGroupSacrifice(pDestPlot, false, false, bNoBlitz);
							CvUnit * pBestSacrifice = AI_getBestGroupSacrifice(pDestPlot, false, bForceNoWar, bNoBlitz);
							// R&R, ray, Natives raiding party

							if (pBestSacrifice != NULL)
							{
								pBestAttackUnit = pBestSacrifice;
							}
						}

						bAttack = true;

						if (GC.getUSE_DO_COMBAT_CALLBACK()) // K-Mod. block unused python callbacks
						{
							CySelectionGroup* pyGroup = new CySelectionGroup(this);
							CyPlot* pyPlot = new CyPlot(pDestPlot);
							CyArgsList argsList;
							argsList.add(gDLL->getPythonIFace()->makePythonObject(pyGroup));	// pass in Selection Group class
							argsList.add(gDLL->getPythonIFace()->makePythonObject(pyPlot));	// pass in Plot class
							long lResult = 0;
							gDLL->getPythonIFace()->callFunction(PYGameModule, "doCombat", argsList.makeFunctionArgs(), &lResult);
							delete pyGroup;	// python fxn must not hold on to this pointer
							delete pyPlot;	// python fxn must not hold on to this pointer
							if (lResult == 1)
							{
								break;
							}
						}
						if (getNumUnits() > 1)
						{
							if (pBestAttackUnit->plot()->isFighting() || pDestPlot->isFighting())
							{
								bFailedAlreadyFighting = true;
							}
							else
							{
								pBestAttackUnit->attack(pDestPlot, bStack);
							}
						}
						else
						{
							pBestAttackUnit->attack(pDestPlot, false);
							break;
						}

						if (bFailedAlreadyFighting || !bStack)
						{
							// if this is AI stack, follow through with the attack to the end
							if (!isHuman() && getNumUnits() > 1 &&
								// K-Mod: if this is AI stack, follow through with the attack to the end
								!(iFlags & MOVE_SINGLE_ATTACK))
							{
								AI_queueGroupAttack(iX, iY);
							}
							break;
						}
					}
				}
			}
		}
	}

	return bAttack;
}
// R&R, ray, Natives raiding party - END

// Most of this function has been restructured / edited for K-Mod.
void CvSelectionGroup::groupMove(CvPlot* pPlot, bool bCombat, CvUnit* pCombatUnit, bool bEndMove)
{
	//PROFILE_FUNC();
	FAssert(!isBusy());

	// K-Mod. Some variables to help us regroup appropriately if not everyone can move.
	CvSelectionGroup* pStaticGroup = 0;
	UnitAITypes eHeadAI = getHeadUnitAI();

	// Copy the list of units to move. (Units may be bumped or killed during the move process; which could mess up the group.)
	std::vector<IDInfo> originalGroup;

	if (pCombatUnit)
		originalGroup.push_back(pCombatUnit->getIDInfo());

	for (CLLNode<IDInfo>* pUnitNode = headUnitNode(); pUnitNode != NULL; pUnitNode = nextUnitNode(pUnitNode))
	{
		if (pCombatUnit == NULL || pUnitNode->m_data != pCombatUnit->getIDInfo())
		{
			originalGroup.push_back(pUnitNode->m_data);
		}
	}
	FAssert(originalGroup.size() == getNumUnits());
	// K-Mod end

	//while (pUnitNode != NULL)
	for (std::vector<IDInfo>::iterator it = originalGroup.begin(); it != originalGroup.end(); ++it) // K-Mod
	{
		//CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		//pUnitNode = nextUnitNode(pUnitNode);
		CvUnit* pLoopUnit = ::getUnit(*it);

		//if ((pLoopUnit->canMove() && ((bCombat && (!(pLoopUnit->isNoCapture()) || !(pPlot->isEnemyCity(*pLoopUnit)))) ? pLoopUnit->canMoveOrAttackInto(pPlot) : pLoopUnit->canMoveInto(pPlot))) || (pLoopUnit == pCombatUnit))
		// K-Mod
		if (pLoopUnit == NULL)
			continue;
		if (pLoopUnit->canMove() && (bCombat ? pLoopUnit->canMoveOrAttackInto(pPlot) : pLoopUnit->canMoveInto(*pPlot)))
		{
			pLoopUnit->move(pPlot, true);
		}
		else
		{
			/* original bts code
			pLoopUnit->joinGroup(NULL, true);
			pLoopUnit->ExecuteMove(((float)(GC.getMissionInfo(MISSION_MOVE_TO).getTime() * gDLL->getMillisecsPerTurn())) / 1000.0f, false); */

			// K-Mod. all units left behind should stay in the same group. (unless it would mean a change of group AI)
			// (Note: it is important that units left behind are not in the original group.
			// The later code assumes that the original group has moved, and if it hasn't, there will be an infinite loop.)
			if (pStaticGroup)
				pLoopUnit->joinGroup(pStaticGroup, true);
			else
			{
				pLoopUnit->joinGroup(0, true);
				if (isHuman() || pLoopUnit->AI_getUnitAIType() == eHeadAI)
					pStaticGroup = pLoopUnit->getGroup();
				// else -- wwe could track the ungrouped units; but I don't think there's much point.
			}
			//
		}
		// K-Mod. If the unit is no longer in the original group; then display it's movement animation now.
		if (pLoopUnit->getGroupID() != getID())
			pLoopUnit->ExecuteMove(((float)(GC.getMissionInfo(MISSION_MOVE_TO).getTime() * gDLL->getMillisecsPerTurn())) / 1000.0f, false);
	}

	// Execute move animation for units still in this group.
	if (bEndMove || !canAllMove())
	{
		CLLNode<IDInfo>* pUnitNode = headUnitNode();
		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			try
			{
				// note: pLoopUnit might be NULL. Checking for NULL is faster than a try-catch, particularly if it fails
				pLoopUnit->ExecuteMove(((float)(GC.getMissionInfo(MISSION_MOVE_TO).getTime() * gDLL->getMillisecsPerTurn())) / 1000.0f, false);
			}
			catch (...)
			{
				logBBAI(" pLoopUnit->ExecuteMove exception!");
			}
		}
	}
}

// Returns true if move was made...
bool CvSelectionGroup::groupPathTo(int iX, int iY, int iFlags)
{
	KmodPathFinder final_path; // K-Mod
	CvPlot* const pOriginPlot = plot(); // K-Mod

	if (at(iX, iY))
	{
		return false; // XXX is this necessary?
	}

	FAssert(!isBusy());
	FAssert(getOwnerINLINE() != NO_PLAYER);
	FAssert(headMissionQueueNode() != NULL);

	CvPlot* const pDestPlot = GC.getMap().plotINLINE(iX, iY);
	FAssertMsg(pDestPlot != NULL, "DestPlot is not assigned a valid value");

	FAssertMsg(canAllMove(), "canAllMove is expected to be true");

	/*
	if (!generatePath(plot(), pDestPlot, iFlags))
	{
		return false;
	}

	pPathPlot = getPathFirstPlot(); // BtS
	*/

	// K-Mod. I've added & ~MOVE_DECLARE_WAR so that if we need to declare war at this point, and haven't yet done so,
	// the move will fail here rather than splitting the group inside groupMove.
	// Also, I've change it to use a different pathfinder, to avoid clearing the path data - and to avoid OOS errors.
	final_path.SetSettings(this, iFlags & ~MOVE_DECLARE_WAR);
	if (!final_path.GeneratePath(pDestPlot))
		return false;

	CvPlot* pPathPlot = final_path.GetPathFirstPlot();
	// K-Mod end

	if (groupAmphibMove(pPathPlot, iFlags))
	{
		return false;
	}
	/*
	bool bForce = false;
	MissionAITypes eMissionAI = AI_getMissionAIType();
	if (eMissionAI == MISSIONAI_PILLAGE)
		bForce = true;
	if (groupDeclareWar(pPathPlot, bForce))
		return false;*/ // BtS
	// Disabled by K-Mod. AI war decisions have no business being here.

	bool bEndMove = false;
	if(pPathPlot == pDestPlot)
	{
		bEndMove = true;
	}

	//groupMove(pPathPlot, iFlags & MOVE_THROUGH_ENEMY, NULL, bEndMove);
	groupMove(pPathPlot, false, NULL, bEndMove); // K-Mod

	FAssert(getNumUnits() == 0 || atPlot(pPathPlot)); // K-Mod

	// K-Mod.
	if (!AI_isControlled() && !bEndMove)
	{
		//If the step we just took will make us change our path to something longer, then cancel the move.
		// This prevents units from wasting all their moves by trying to walk around enemy units.
		// Don't do this in new movement system if not all units can move (this will cancel movement missions)
		// for selection groups with mixed promotions (I have no idea how else to fix this)
		FAssert(final_path.IsPathComplete());
		std::pair<int, int> old_moves = std::make_pair(final_path.GetPathTurns(), -final_path.GetFinalMoves());
		if ((GLOBAL_DEFINE_USE_CLASSIC_MOVEMENT_SYSTEM || canAllMove()) &&
			(!final_path.GeneratePath(pDestPlot)
			|| std::make_pair(final_path.GetPathTurns(), -final_path.GetFinalMoves()) > old_moves))
		{
			clearMissionQueue();
		}
		// Also, if the step we just took causes us to backtrack - its probably because we've lost vision of a unit that was blocking the path.
		// Apply the MOVE_ASSUME_VISIBLE flag, so that we remember to go the long way around.
		else if (final_path.GetPathFirstPlot() == pOriginPlot)
		{
			headMissionQueueNode()->m_data.iFlags |= MOVE_ASSUME_VISIBLE;
		}
	}
	// K-Mod end

	return true;
}


// Returns true if move was made... --> WTP, jooe, 2023-04-17: really?
bool CvSelectionGroup::groupRouteTo(int iX, int iY, int iFlags, RouteTypes ePreferredRoute)
{
	if (!AI_isControlled() || (getLengthMissionQueue() == 1))
	{
		CvPlot* pPlot = plot();
		BuildTypes eBestBuild = getBestBuildRouteBuild(pPlot, ePreferredRoute);
		if (eBestBuild != NO_BUILD)
		{
			groupBuild(eBestBuild);
			return true;
		}
	}

	return groupPathTo(iX, iY, iFlags);
}


// Returns true if build should continue...
bool CvSelectionGroup::groupBuild(BuildTypes eBuild)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pPlot;
	bool bContinue;

	FAssert(getOwnerINLINE() != NO_PLAYER);
	FAssertMsg(eBuild < GC.getNumBuildInfos(), "Invalid Build");

	bContinue = false;

	pPlot = plot();

	ImprovementTypes eImprovement = (ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement();
	if (eImprovement != NO_IMPROVEMENT)
	{
		if (AI_isControlled())
		{
			if (GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_SAFE_AUTOMATION))
			{
				if ((pPlot->getImprovementType() != NO_IMPROVEMENT) && (pPlot->getImprovementType() != (ImprovementTypes)(GC.getDefineINT("RUINS_IMPROVEMENT"))))
				{
					if (GC.getImprovementInfo(eImprovement).getImprovementPillage() != NO_IMPROVEMENT)
					{
						return false;
					}
				}
			}
		}
	}

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);

		FAssertMsg(pLoopUnit->atPlot(pPlot), "pLoopUnit is expected to be at pPlot");

		if (pLoopUnit != NULL && pLoopUnit->canBuild(pPlot, eBuild))
		{
			bContinue = true;

			if (pLoopUnit->build(eBuild))
			{
				bContinue = false;
				break;
			}
		}
		pUnitNode = nextUnitNode(pUnitNode);
	}

	return bContinue;
}

void CvSelectionGroup::setTransportUnit(CvUnit* pTransportUnit)
{
	// if we are loading
	if (pTransportUnit != NULL)
	{
		CvUnit* pHeadUnit = getHeadUnit();
		FAssertMsg(pHeadUnit != NULL, "non-zero group without head unit");

		int iCargoSpaceAvailable = pTransportUnit->cargoSpaceAvailable(pHeadUnit->getSpecialUnitType(), pHeadUnit->getDomainType());

		// if no space at all, give up
		if (iCargoSpaceAvailable < 1)
		{
			return;
		}

		// if there is space, but not enough to fit whole group, then split us, and set on the new group
		if (iCargoSpaceAvailable < getNumUnits())
		{
			CvSelectionGroup* pSplitGroup = splitGroup(iCargoSpaceAvailable);
			pSplitGroup->setTransportUnit(pTransportUnit);
			return;
		}

		FAssertMsg(iCargoSpaceAvailable >= getNumUnits(), "cargo size too small");

		// setTransportUnit removes the unit from the current group (at least currently), so we have to be careful in the loop here
		// so, loop until we do not load one
		bool bLoadedOne;
		do
		{
			bLoadedOne = false;

			// loop over all the units, finding one to load
			CLLNode<IDInfo>* pUnitNode = headUnitNode();
			while (pUnitNode != NULL && !bLoadedOne)
			{
				CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);

				// just in case implementation of setTransportUnit changes, check to make sure this unit is not already loaded
				if (pLoopUnit != NULL && pLoopUnit->getTransportUnit() != pTransportUnit)
				{
					// if there is room, load the unit and stop the loop (since setTransportUnit ungroups this unit currently)
					bool bSpaceAvailable = pTransportUnit->cargoSpaceAvailable(pLoopUnit->getSpecialUnitType(), pLoopUnit->getDomainType());
					if (bSpaceAvailable)
					{
						pLoopUnit->setTransportUnit(pTransportUnit);
						bLoadedOne = true;
					}
				}
			}
		}
		while (bLoadedOne);
	}
	// otherwise we are unloading
	else
	{
		// loop over all the units, unloading them
		CLLNode<IDInfo>* pUnitNode = headUnitNode();
		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			if (pLoopUnit != NULL)
			{
				// unload unit
				pLoopUnit->setTransportUnit(NULL);
			}
		}
	}
}


bool CvSelectionGroup::isAmphibPlot(const CvPlot* pPlot) const
{
	bool bFriendly = true;
	CvUnit* pUnit = getHeadUnit();
	if (NULL != pUnit)
	{
		bFriendly = pPlot->isFriendlyCity(*pUnit, true);
	}

	return ((getDomainType() == DOMAIN_SEA) && pPlot->isCoastalLand() && !bFriendly);
}


// Returns true if attempted an amphib landing...
bool CvSelectionGroup::groupAmphibMove(CvPlot* pPlot, int iFlags)
{
	CLLNode<IDInfo>* pUnitNode1;
	CLLNode<IDInfo>* pUnitNode2;
	CvUnit* pLoopUnit1;
	CvUnit* pLoopUnit2;
	bool bLanding = false;

	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (groupDeclareWar(pPlot))
	{
		return true;
	}

	if (isAmphibPlot(pPlot))
	{
		if (stepDistance(getX(), getY(), pPlot->getX_INLINE(), pPlot->getY_INLINE()) == 1)
		{
			pUnitNode1 = headUnitNode();

			while (pUnitNode1 != NULL)
			{
				pLoopUnit1 = ::getUnit(pUnitNode1->m_data);
				pUnitNode1 = nextUnitNode(pUnitNode1);

				if (pLoopUnit1 != NULL && (pLoopUnit1->getCargo() > 0) && (pLoopUnit1->domainCargo() == DOMAIN_LAND))
				{
					std::vector<CvSelectionGroup*> aCargoGroups;

					pUnitNode2 = plot()->headUnitNode();
					while (pUnitNode2 != NULL)
					{
						pLoopUnit2 = plot()->getUnitNodeLoop(pUnitNode2);

						if (pLoopUnit2 != NULL && pLoopUnit2->getTransportUnit() == pLoopUnit1)
						{
							if (pLoopUnit2->isGroupHead())
							{
								aCargoGroups.push_back(pLoopUnit2->getGroup());
							}
						}
					}

					std::vector<CvSelectionGroup*>::iterator it;
					for (it = aCargoGroups.begin(); it != aCargoGroups.end(); ++it)
					{
						CvSelectionGroup* pGroup = *it;
						if (pGroup->canAllMove())
						{
							FAssert(!pGroup->at(pPlot->getX_INLINE(), pPlot->getY_INLINE()));
							pGroup->pushMission(MISSION_MOVE_TO, pPlot->getX_INLINE(), pPlot->getY_INLINE(), (MOVE_IGNORE_DANGER | iFlags));
							bLanding = true;
						}
					}
				}
			}
		}
	}

	return bLanding;
}


bool CvSelectionGroup::readyToSelect(bool bAny)
{
	return (readyToMove(bAny) && !isAutomated() && isOnMap());
}


bool CvSelectionGroup::readyToMove(bool bAny)
{
	return (((bAny) ? canAnyMove() : canAllMove()) && (headMissionQueueNode() == NULL) && (getActivityType() == ACTIVITY_AWAKE) && !isBusy() && !isCargoBusy());
}


bool CvSelectionGroup::readyToAuto()
{
	return (canAllMove() && (headMissionQueueNode() != NULL));
}

bool CvSelectionGroup::isOnMap() const
{
	CvUnit* pUnit = getHeadUnit();
	if (pUnit != NULL)
	{
		return (pUnit->isOnMap());
	}

	return false;
}


int CvSelectionGroup::getID() const
{
	return m_iID;
}


void CvSelectionGroup::setID(int iID)
{
	m_iID = iID;
}


PlayerTypes CvSelectionGroup::getOwner() const
{
	return getOwnerINLINE();
}


TeamTypes CvSelectionGroup::getTeam() const
{
	if (getOwnerINLINE() != NO_PLAYER)
	{
		return GET_PLAYER(getOwnerINLINE()).getTeam();
	}

	return NO_TEAM;
}


int CvSelectionGroup::getMissionTimer() const
{
	return m_iMissionTimer;
}


void CvSelectionGroup::setMissionTimer(int iNewValue)
{
	FAssert(getOwnerINLINE() != NO_PLAYER);

	m_iMissionTimer = iNewValue;
	FAssert(getMissionTimer() >= 0);
}


void CvSelectionGroup::changeMissionTimer(int iChange)
{
	setMissionTimer(getMissionTimer() + iChange);
}


void CvSelectionGroup::updateMissionTimer(int iSteps)
{
	CvUnit* pTargetUnit;
	CvPlot* pTargetPlot;
	int iTime;

	if (!isHuman() && !showMoves())
	{
		iTime = 0;
	}
	else if (headMissionQueueNode() != NULL)
	{
		iTime = GC.getMissionInfo((MissionTypes)(headMissionQueueNode()->m_data.eMissionType)).getTime();

		if ((headMissionQueueNode()->m_data.eMissionType == MISSION_MOVE_TO) ||
				(headMissionQueueNode()->m_data.eMissionType == MISSION_ROUTE_TO) ||
				(headMissionQueueNode()->m_data.eMissionType == MISSION_ROUTE_TO_ROAD) ||
				(headMissionQueueNode()->m_data.eMissionType == MISSION_ROUTE_TO_COUNTRY_ROAD) ||
				(headMissionQueueNode()->m_data.eMissionType == MISSION_MOVE_TO_UNIT))
		{
			if (headMissionQueueNode()->m_data.eMissionType == MISSION_MOVE_TO_UNIT)
			{
				pTargetUnit = GET_PLAYER((PlayerTypes)headMissionQueueNode()->m_data.iData1).getUnit(headMissionQueueNode()->m_data.iData2);
				if (pTargetUnit != NULL)
				{
					pTargetPlot = pTargetUnit->plot();
				}
				else
				{
					pTargetPlot = NULL;
				}
			}
			else
			{
				pTargetPlot = GC.getMap().plotINLINE(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2);
			}

			if (atPlot(pTargetPlot))
			{
				iTime += iSteps;
			}
			else
			{
				iTime = std::min(iTime, 2);
			}
		}

		if (isHuman() && (isAutomated() || (GET_PLAYER((GC.getGameINLINE().isNetworkMultiPlayer()) ? getOwnerINLINE() : GC.getGameINLINE().getActivePlayer()).isOption(PLAYEROPTION_QUICK_MOVES))))
		{
			iTime = std::min(iTime, 1);
		}
	}
	else
	{
		iTime = 0;
	}

	setMissionTimer(iTime);
}


bool CvSelectionGroup::isForceUpdate()
{
	return m_bForceUpdate;
}


void CvSelectionGroup::setForceUpdate(bool bNewValue)
{
	m_bForceUpdate = bNewValue;
}


ActivityTypes CvSelectionGroup::getActivityType() const
{
	return m_eActivityType;
}


void CvSelectionGroup::setActivityType(ActivityTypes eNewValue)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pPlot;

	FAssert(getOwnerINLINE() != NO_PLAYER);

	ActivityTypes eOldActivity = getActivityType();

	if (eOldActivity != eNewValue)
	{
		pPlot = plot();

		m_eActivityType = eNewValue;

		if (getActivityType() != ACTIVITY_MISSION)
		{
			pUnitNode = headUnitNode();

			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);

				if (pLoopUnit != NULL)
				{
					pLoopUnit->NotifyEntity(MISSION_IDLE);
				}
			}

			if (getTeam() == GC.getGameINLINE().getActiveTeam())
			{
				if (pPlot != NULL)
				{
					pPlot->setFlagDirty(true);
				}
			}
		}

		if (pPlot == gDLL->getInterfaceIFace()->getSelectionPlot())
		{
			gDLL->getInterfaceIFace()->setDirty(PlotListButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		}
	}
}


AutomateTypes CvSelectionGroup::getAutomateType() const
{
	return m_eAutomateType;
}


bool CvSelectionGroup::isAutomated() const
{
	return (getAutomateType() != NO_AUTOMATE);
}


void CvSelectionGroup::setAutomateType(AutomateTypes eNewValue)
{
	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (getAutomateType() != eNewValue)
	{
		m_eAutomateType = eNewValue;

		clearMissionQueue();
		setActivityType(ACTIVITY_AWAKE);

		// if canceling automation, cancel on cargo as well
		if (eNewValue == NO_AUTOMATE)
		{
			getHeadUnit()->AI_setUnitAIState(UNITAI_STATE_DEFAULT);

			CvPlot* pPlot = plot();
			if (pPlot != NULL)
			{
				CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
				while (pUnitNode != NULL)
				{
					CvUnit* pCargoUnit = pPlot->getUnitNodeLoop(pUnitNode);

					if (pCargoUnit != NULL)
					{
						CvUnit* pTransportUnit = pCargoUnit->getTransportUnit();
						if (pTransportUnit != NULL && pTransportUnit->getGroup() == this)
						{
							pCargoUnit->getGroup()->setAutomateType(NO_AUTOMATE);
						}
					}
				}
			}
		}

		//clear all trade routes
		if (eNewValue != AUTOMATE_TRANSPORT_ROUTES)
		{
			clearTradeRoutes();
		}
	}
}


// Disabled by K-Mod. (This function is deprecated.)
/* FAStarNode* CvSelectionGroup::getPathLastNode() const
{
//return path_finder.GetEndNode();
//return gDLL->getFAStarIFace()->GetLastNode(&GC.getPathFinder());
} */


CvPlot* CvSelectionGroup::getPathFirstPlot() const
{
	return path_finder.GetPathFirstPlot();
}


CvPlot* CvSelectionGroup::getPathEndTurnPlot() const
{
	return path_finder.GetPathEndTurnPlot();
}

//This number is fairly large
//1 plot = 100
int CvSelectionGroup::getPathCost() const
{
	FAStarNode* pNode;
	//pNode = getPathEndTurnPlot();

	pNode = path_finder.GetEndNode();

	if (pNode != NULL)
	{
		int iCost = pNode->m_iTotalCost;
		iCost *= 100;
		iCost /= (1000 * GLOBAL_DEFINE_MOVE_DENOMINATOR);

		return iCost;
	}
	return MAX_INT;
}

CvPlot* CvSelectionGroup::getPathSecondLastPlot() const
{
	//FAStarNode* pNode = getPathLastNode();
	FAStarNode* pNode = path_finder.GetEndNode();

	if (pNode->m_pParent == NULL)
	{
		return GC.getMap().plotSoren(pNode->m_iX, pNode->m_iY);
	}

	return GC.getMap().plotSoren(pNode->m_pParent->m_iX, pNode->m_pParent->m_iY);
}

// TAC - AI Improved Naval AI - koma13 - START
CvPlot* CvSelectionGroup::getPathPlotByIndex(int iIndex) const
{
	FAssert(iIndex >= 0);
	FAssert(iIndex <= getPathLength());

	FAStarNode* pNode;

	//pNode = getPathLastNode();
	pNode = path_finder.GetEndNode();

	int i = 0;

	while (pNode != NULL)
	{
		if (iIndex == i)
		{
			return GC.getMap().plotSoren(pNode->m_iX, pNode->m_iY);
		}

		pNode = pNode->m_pParent;
		i++;
	}

	FAssert(false);
	return NULL;
}

int CvSelectionGroup::getPathLength() const
{
	FAStarNode* pNode;

	//pNode = getPathLastNode();
	pNode = path_finder.GetEndNode();

	if (pNode != NULL)
	{
		if (pNode->m_pParent == NULL)
		{
			return 0;
		}

		int iLength = 0;

		while (pNode != NULL)
		{
			iLength++;

			if (pNode->m_pParent->m_pParent == NULL)
			{
				return iLength;
			}

			pNode = pNode->m_pParent;
		}
	}

	FAssert(false);

	return -1;
}
// TAC - AI Improved Naval AI - koma13 - END

// Note that the bReuse parameter is ignored due to internal cacheing and hence there is no need to request it.
// If a temporary path finder is used, be aware that the life time of the pf is restricted to the duration of
// generatePath and thus attempting to access the internal pf state to get the nodes will be invalid!
bool CvSelectionGroup::generatePath(const CvPlot* pFromPlot, const CvPlot* pToPlot, int iFlags, bool bReuse, int* piPathTurns, int iMaxPath, bool bUseTempFinder, bool bCalledFromPython) const
{
	PROFILE("CvSelectionGroup::generatePath()");

	if (pFromPlot == NULL || pToPlot == NULL) {
		FAssert(false);
		return false;
	}

	FAssert(bCalledFromPython || AI().AI_isControlledInternal() || (iFlags & MOVE_MAX_MOVES));
	FAssert(!bUseTempFinder || !bReuse);

	KmodPathFinder tempFinder;
	KmodPathFinder& pathFinder = (bUseTempFinder ? tempFinder : path_finder);
	pathFinder.SetSettings(this, iFlags, iMaxPath, bUseTempFinder ? GLOBAL_DEFINE_MOVE_DENOMINATOR : 0);

	const bool bSuccess = pathFinder.GeneratePath(pFromPlot->getX(), pFromPlot->getY(), pToPlot->getX(), pToPlot->getY());

	if (piPathTurns != NULL) {
		*piPathTurns = MAX_INT;
		if (bSuccess) {
			*piPathTurns = pathFinder.GetPathTurns();
		}
	}

	return bSuccess;
}

/* void CvSelectionGroup::resetPath() const
{
//path_finder.Reset(); // note. the K-Mod finder doesn't need resetting in all the same places.
gDLL->getFAStarIFace()->ForceReset(&GC.getPathFinder());
} */


void CvSelectionGroup::clearUnits()
{
	CLLNode<IDInfo>* pUnitNode;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pUnitNode = deleteUnitNode(pUnitNode);
	}
}


// Returns true if the unit is added...
bool CvSelectionGroup::addUnit(CvUnit* pUnit, bool bMinimalChange)
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	bool bAdded;

	if (!(pUnit->canJoinGroup(pUnit->plot(), this)))
	{
		return false;
	}

	bAdded = false;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		if (pLoopUnit != NULL && (
			(pUnit->AI().AI_groupFirstValInternal() > pLoopUnit->AI().AI_groupFirstValInternal()) ||
			  ((pUnit->AI().AI_groupFirstValInternal() == pLoopUnit->AI().AI_groupFirstValInternal()) &&
				 (pUnit->AI_groupSecondVal() > pLoopUnit->AI_groupSecondVal()))))
		{
			m_units.insertBefore(pUnit->getIDInfo(), pUnitNode);
			bAdded = true;
			break;
		}
		pUnitNode = nextUnitNode(pUnitNode);
	}

	if (!bAdded)
	{
		m_units.insertAtEnd(pUnit->getIDInfo());
	}

	if(!bMinimalChange)
	{
		if (getOwnerINLINE() == NO_PLAYER)
		{
			if (getNumUnits() > 0)
			{
				pUnitNode = headUnitNode();
				while (pUnitNode != NULL)
				{
					CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
					if (pLoopUnit != NULL)
					{
						pLoopUnit->NotifyEntity(MISSION_MULTI_SELECT);
					}
					pUnitNode = nextUnitNode(pUnitNode);
				}
			}
		}
	}

	return true;
}


void CvSelectionGroup::removeUnit(CvUnit* pUnit)
{
	CLLNode<IDInfo>* pUnitNode;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		if (::getUnit(pUnitNode->m_data) == pUnit)
		{
			deleteUnitNode(pUnitNode);
			break;
		}
		else
		{
			pUnitNode = nextUnitNode(pUnitNode);
		}
	}
}


CLLNode<IDInfo>* CvSelectionGroup::deleteUnitNode(CLLNode<IDInfo>* pNode)
{
	CLLNode<IDInfo>* pNextUnitNode;

	if (getOwnerINLINE() != NO_PLAYER)
	{
		setAutomateType(NO_AUTOMATE);
		clearMissionQueue();

		switch (getActivityType())
		{
		case ACTIVITY_SLEEP:
			break;
		default:
			setActivityType(ACTIVITY_AWAKE);
			break;
		}
	}

	pNextUnitNode = m_units.deleteNode(pNode);

	return pNextUnitNode;
}

CvUnit* CvSelectionGroup::getUnitNodeLoop(CLLNode<IDInfo>*& pUnitNode) const
{
	CvUnit *pUnit = ::getUnit(pUnitNode->m_data);
	FAssertMsg(pUnit != NULL, "Selection group has a NULL unit");
	pUnitNode = nextUnitNode(pUnitNode);
	return pUnit;
}

CvUnit* CvSelectionGroup::getUnitNodeLoop(const CLLNode<IDInfo>*& pUnitNode) const
{
	CvUnit *pUnit = ::getUnit(pUnitNode->m_data);
	FAssertMsg(pUnit != NULL, "Selection group has a NULL unit");
	pUnitNode = nextUnitNode(pUnitNode);
	return pUnit;
}

int CvSelectionGroup::getNumUnits() const
{
	return m_units.getLength();
}

void CvSelectionGroup::mergeIntoGroup(CvSelectionGroup* pSelectionGroup)
{
	CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());

	// merge groups, but make sure we do not change the head unit AI
	// this means that if a new unit is going to become the head, change its AI to match, if possible
	// AI_setUnitAIType removes the unit from the current group (at least currently), so we have to be careful in the loop here
	// so, loop until we have not changed unit AIs
	bool bChangedUnitAI;
	do
	{
		bChangedUnitAI = false;

		// loop over all the units, moving them to the new group,
		// stopping if we had to change a unit AI, because doing so removes that unit from our group, so we have to start over
		CLLNode<IDInfo>* pUnitNode = headUnitNode();
		while (pUnitNode != NULL && !bChangedUnitAI)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			if (pLoopUnit != NULL)
			{
				UnitAITypes eUnitAI = pLoopUnit->AI_getUnitAIType();

				// if the unitAIs are different, and the loop unit has a higher val, then the group unitAI would change
				// change this UnitAI to the old group UnitAI if possible
				CvUnit* pNewHeadUnit = pSelectionGroup->getHeadUnit();
				UnitAITypes eNewHeadUnitAI = pSelectionGroup->getHeadUnitAI();
				if (pNewHeadUnit!= NULL && eUnitAI != eNewHeadUnitAI && pLoopUnit->AI().AI_groupFirstVal() > pNewHeadUnit->AI().AI_groupFirstVal())
				{
					// non-zero AI_unitValue means that this UnitAI is valid for this unit (that is the check used everywhere)
					if (kPlayer.AI_unitValue(pLoopUnit->getUnitType(), eNewHeadUnitAI, NULL) > 0)
					{
						// this will remove pLoopUnit from the current group
						pLoopUnit->AI_setUnitAIType(eNewHeadUnitAI);

						bChangedUnitAI = true;
					}
				}

				pLoopUnit->joinGroup(pSelectionGroup);
			}
		}
	}
	while (bChangedUnitAI);
}

// split this group into two new groups, one of iSplitSize, the other the remaining units
// split up each unit AI type as evenly as possible
CvSelectionGroup* CvSelectionGroup::splitGroup(int iSplitSize, CvUnit* pNewHeadUnit)
{
	FAssertMsg(iSplitSize > 0, "non-positive splitGroup size");
	if (!(iSplitSize > 0))
	{
		return NULL;
	}

	// are we already small enough?
	if (getNumUnits() <= iSplitSize)
	{
		return this;
	}

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	CvUnit* pOldHeadUnit = ::getUnit(pUnitNode->m_data);
	FAssertMsg(pOldHeadUnit != NULL, "non-zero group without head unit");
	if (pOldHeadUnit == NULL)
	{
		return NULL;
	}

	UnitAITypes eOldHeadAI = pOldHeadUnit->AI_getUnitAIType();

	// if pNewHeadUnit NULL, then we will use our current head to head the new split group of target size
	if (pNewHeadUnit == NULL)
	{
		pNewHeadUnit = pOldHeadUnit;
	}

	// the AI of the new head (the remainder will get the AI of the old head)
	// UnitAITypes eNewHeadAI = pNewHeadUnit->AI_getUnitAIType();

	// pRemainderHeadUnit is the head unit of the group that contains the remainder of units
	CvUnit* pRemainderHeadUnit = NULL;

	// if the new head is not the old head, then make the old head the remainder head
	bool bSplitingHead = (pOldHeadUnit == pNewHeadUnit);
	if (!bSplitingHead)
	{
		pRemainderHeadUnit = pOldHeadUnit;
	}

	// try to find remainder head with same AI as head, if we cannot find one, we will split the rest of the group up
	if (pRemainderHeadUnit == NULL)
	{
		// loop over all the units
		pUnitNode = headUnitNode();
		while (pUnitNode != NULL && pRemainderHeadUnit == NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			if (pLoopUnit != NULL && pLoopUnit != pNewHeadUnit)
			{
				UnitAITypes eLoopUnitAI = pLoopUnit->AI_getUnitAIType();
				if (eLoopUnitAI == eOldHeadAI)
				{
					pRemainderHeadUnit = pLoopUnit;
				}
			}
		}
	}

	CvSelectionGroup* pSplitGroup = NULL;
	CvSelectionGroup* pRemainderGroup = NULL;

	// make the new group for the new head
	pNewHeadUnit->joinGroup(NULL);
	pSplitGroup = pNewHeadUnit->getGroup();
	FAssertMsg(pSplitGroup != NULL, "join resulted in NULL group");

	// make a new group for the remainder, if non-null
	if (pRemainderHeadUnit != NULL)
	{
		pRemainderHeadUnit->joinGroup(NULL);
		pRemainderGroup = pRemainderHeadUnit->getGroup();
		FAssertMsg(pRemainderGroup != NULL, "join resulted in NULL group");
	}

	// loop until this group is empty, trying to move different AI types each time


	//Exhibit of why i HATE iustus code sometimes
	//unsigned int unitAIBitField = 0;
	//setBit(unitAIBitField, eNewHeadAI);

	bool abUnitAIField[NUM_UNITAI_TYPES];
	for (int iI = 0; iI < NUM_UNITAI_TYPES; iI++)
	{
		abUnitAIField[iI] = false;
	}

	while (getNumUnits())
	{
		UnitAITypes eTargetUnitAI = NO_UNITAI;

		// loop over all the units, find the next different UnitAI and move one of each
		bool bDestinationSplit = (pSplitGroup->getNumUnits() < iSplitSize);
		pUnitNode = headUnitNode();
		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			if (pLoopUnit != NULL)
			{
				UnitAITypes eLoopUnitAI = pLoopUnit->AI_getUnitAIType();

				// if we have not found a new type to move, is this a new unitai?
				// note, if there eventually are unitAIs above 31, we will just always move those, which is fine
				if (eTargetUnitAI == NO_UNITAI && !abUnitAIField[eLoopUnitAI])
				{
					eTargetUnitAI =  eLoopUnitAI;
					abUnitAIField[eLoopUnitAI] = true;
				}

				// is this the right UnitAI?
				if (eLoopUnitAI == eTargetUnitAI)
				{
					// move this unit to the appropriate group (if pRemainderGroup NULL, it gets its own group)
					pLoopUnit->joinGroup(bDestinationSplit ? pSplitGroup : pRemainderGroup);

					// if we moved to remainder, try for next unit AI
					if (!bDestinationSplit)
					{
						eTargetUnitAI = NO_UNITAI;

						bDestinationSplit = (pSplitGroup->getNumUnits() < iSplitSize);
					}
					else
					{
						// next unit goes to the remainder group
						bDestinationSplit = false;
					}
				}
			}

		}

		// clear bitfield, all types are valid again
		for (int iI = 0; iI < NUM_UNITAI_TYPES; iI++)
		{
			abUnitAIField[iI] = false;
		}
	}

	FAssertMsg(pSplitGroup->getNumUnits() <= iSplitSize, "somehow our split group is too large");

	return pSplitGroup;
}


//------------------------------------------------------------------------------------------------
// FUNCTION:    CvSelectionGroup::getUnitIndex
//! \brief      Returns the index of the given unit in the selection group
//! \param      pUnit The unit to find the index of within the group
//! \retval     The zero-based index of the unit within the group, or -1 if it is not in the group.
//------------------------------------------------------------------------------------------------
int CvSelectionGroup::getUnitIndex(CvUnit* pUnit, int maxIndex /* = -1 */) const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int iIndex;

	iIndex = 0;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit == pUnit)
		{
			return iIndex;
		}

		iIndex++;

		//early out if not interested beyond maxIndex
		if((maxIndex >= 0) && (iIndex >= maxIndex))
			return -1;
	}

	return -1;
}

CLLNode<IDInfo>* CvSelectionGroup::headUnitNode() const
{
	return m_units.head();
}


CvUnit* CvSelectionGroup::getHeadUnit() const
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	if (pUnitNode != NULL)
	{
		return ::getUnit(pUnitNode->m_data);
	}
	else
	{
		return NULL;
	}
}

CvUnit* CvSelectionGroup::getUnitAt(int index) const
{
	int numUnits = getNumUnits();
	if(index >= numUnits)
	{
		FAssertMsg(false, "[Jason] Selectiongroup unit index out of bounds.");
		return NULL;
	}
	else
	{
		CLLNode<IDInfo>* pUnitNode = headUnitNode();
		for (int i = 0; i < index; i++)
		{
			pUnitNode = nextUnitNode(pUnitNode);
		}

		CvUnit *pUnit = ::getUnit(pUnitNode->m_data);
		return pUnit;
	}
}


UnitAITypes CvSelectionGroup::getHeadUnitAI() const
{
	CvUnit* pHeadUnit;

	pHeadUnit = getHeadUnit();

	if (pHeadUnit != NULL)
	{
		return pHeadUnit->AI_getUnitAIType();
	}

	return NO_UNITAI;
}


PlayerTypes CvSelectionGroup::getHeadOwner() const
{
	CvUnit* pHeadUnit;

	pHeadUnit = getHeadUnit();

	if (pHeadUnit != NULL)
	{
		return pHeadUnit->getOwnerINLINE();
	}

	return NO_PLAYER;
}


TeamTypes CvSelectionGroup::getHeadTeam() const
{
	CvUnit* pHeadUnit;

	pHeadUnit = getHeadUnit();

	if (pHeadUnit != NULL)
	{
		return pHeadUnit->getTeam();
	}

	return NO_TEAM;
}


void CvSelectionGroup::clearMissionQueue()
{
	FAssert(getOwnerINLINE() != NO_PLAYER);

	deactivateHeadMission();

	m_missionQueue.clear();

	if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && IsSelected())
	{
		gDLL->getInterfaceIFace()->setDirty(Waypoints_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
	}
}


int CvSelectionGroup::getLengthMissionQueue() const
{
	return m_missionQueue.getLength();
}


MissionData* CvSelectionGroup::getMissionFromQueue(int iIndex) const
{
	CLLNode<MissionData>* pMissionNode;

	pMissionNode = m_missionQueue.nodeNum(iIndex);

	if (pMissionNode != NULL)
	{
		return &(pMissionNode->m_data);
	}
	else
	{
		return NULL;
	}
}


void CvSelectionGroup::insertAtEndMissionQueue(MissionData mission, bool bStart)
{
	FAssert(getOwnerINLINE() != NO_PLAYER);

	m_missionQueue.insertAtEnd(mission);

	if ((getLengthMissionQueue() == 1) && bStart)
	{
		activateHeadMission();
	}

	if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && IsSelected())
	{
		gDLL->getInterfaceIFace()->setDirty(Waypoints_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
	}
}


CLLNode<MissionData>* CvSelectionGroup::deleteMissionQueueNode(CLLNode<MissionData>* pNode)
{
	CLLNode<MissionData>* pNextMissionNode;

	FAssertMsg(pNode != NULL, "Node is not assigned a valid value");
	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (pNode == headMissionQueueNode())
	{
		deactivateHeadMission();
	}

	pNextMissionNode = m_missionQueue.deleteNode(pNode);

	if (pNextMissionNode == headMissionQueueNode())
	{
		activateHeadMission();
	}

	if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && IsSelected())
	{
		gDLL->getInterfaceIFace()->setDirty(Waypoints_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
	}

	return pNextMissionNode;
}


CLLNode<MissionData>* CvSelectionGroup::nextMissionQueueNode(CLLNode<MissionData>* pNode) const
{
	return m_missionQueue.next(pNode);
}


CLLNode<MissionData>* CvSelectionGroup::prevMissionQueueNode(CLLNode<MissionData>* pNode) const
{
	return m_missionQueue.prev(pNode);
}


CLLNode<MissionData>* CvSelectionGroup::headMissionQueueNode() const
{
	return m_missionQueue.head();
}


CLLNode<MissionData>* CvSelectionGroup::tailMissionQueueNode() const
{
	return m_missionQueue.tail();
}


int CvSelectionGroup::getMissionType(int iNode) const
{
	int iCount = 0;
	CLLNode<MissionData>* pMissionNode;

	pMissionNode = headMissionQueueNode();

	while (pMissionNode != NULL)
	{
		if ( iNode == iCount )
		{
			return pMissionNode->m_data.eMissionType;
		}

		iCount++;

		pMissionNode = nextMissionQueueNode(pMissionNode);
	}

	return -1;
}


int CvSelectionGroup::getMissionData1(int iNode) const
{
	int iCount = 0;
	CLLNode<MissionData>* pMissionNode;

	pMissionNode = headMissionQueueNode();

	while (pMissionNode != NULL)
	{
		if ( iNode == iCount )
		{
			return pMissionNode->m_data.iData1;
		}

		iCount++;

		pMissionNode = nextMissionQueueNode(pMissionNode);
	}

	return -1;
}


int CvSelectionGroup::getMissionData2(int iNode) const
{
	int iCount = 0;
	CLLNode<MissionData>* pMissionNode;

	pMissionNode = headMissionQueueNode();

	while (pMissionNode != NULL)
	{
		if ( iNode == iCount )
		{
			return pMissionNode->m_data.iData2;
		}

		iCount++;

		pMissionNode = nextMissionQueueNode(pMissionNode);
	}

	return -1;
}

// Protected Functions...

void CvSelectionGroup::activateHeadMission()
{
	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (headMissionQueueNode() != NULL)
	{
		if (!isBusy())
		{
			startMission();
		}
	}
}


void CvSelectionGroup::deactivateHeadMission()
{
	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (headMissionQueueNode() != NULL)
	{
		if (getActivityType() == ACTIVITY_MISSION)
		{
			setActivityType(ACTIVITY_AWAKE);
		}

		setMissionTimer(0);

		if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
		{
			if (IsSelected())
			{
				gDLL->getInterfaceIFace()->changeCycleSelectionCounter(1);
			}
		}
	}
}

bool CvSelectionGroup::canAssignTradeRoute(int iRouteID, bool bReusePath) const
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);

		if (pLoopUnit != NULL && pLoopUnit->canAssignTradeRoute(iRouteID, bReusePath))
		{
			return true;
		}
		pUnitNode = nextUnitNode(pUnitNode);
	}

	return false;
}

void CvSelectionGroup::assignTradeRoute(int iRouteID, bool bAssign)
{
	if (GET_PLAYER(getOwnerINLINE()).getTradeRoute(iRouteID) == NULL)
	{
		FAssertMsg(false, "Trade route not found");
		return;
	}

	bool bPreviousEmpty = m_aTradeRoutes.empty();

	if (bAssign)
	{
		m_aTradeRoutes.insert(iRouteID);
	}
	else
	{
		m_aTradeRoutes.erase(iRouteID);
	}

	bool bNewEmpty = m_aTradeRoutes.empty();
	if(bNewEmpty != bPreviousEmpty)
	{
		if (bNewEmpty)
		{
			setAutomateType(NO_AUTOMATE);
		}
		else
		{
			setAutomateType(AUTOMATE_TRANSPORT_ROUTES);
		}
	}
	gDLL->getInterfaceIFace()->setDirty(Domestic_Advisor_DIRTY_BIT, true);
}

bool CvSelectionGroup::isAssignedTradeRoute(int iRouteId) const
{
	return (m_aTradeRoutes.find(iRouteId) != m_aTradeRoutes.end());
}

void CvSelectionGroup::clearTradeRoutes()
{
	m_aTradeRoutes.clear();
	gDLL->getInterfaceIFace()->setDirty(Domestic_Advisor_DIRTY_BIT, true);
}

void CvSelectionGroup::speakWithChief()
{
	CvUnit* pBestUnit = NULL;
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);

		if (pLoopUnit != NULL)
		{
			if (pLoopUnit->canSpeakWithChief(plot())) //first best
			{
				pBestUnit = pLoopUnit;
			}

			if (pLoopUnit->isNoBadGoodies()) //found absolute best
			{
				pBestUnit = pLoopUnit;
				break;
			}
		}
		pUnitNode = nextUnitNode(pUnitNode);
	}

	if (pBestUnit != NULL)
	{
		pBestUnit->speakWithChief();
	}
}

void CvSelectionGroup::read(FDataStreamBase* pStream)
{
	CvSavegameReaderBase readerbase(pStream);
	CvSavegameReader reader(readerbase);

	read(reader);
}

void CvSelectionGroup::write(FDataStreamBase* pStream)
{
	CvSavegameWriterBase writerbase(pStream);
	CvSavegameWriter writer(writerbase);
	write(writer);
	writerbase.WriteFile();
}

// K-Mod
int CvSelectionGroup::maxMoves() const
{
	int iMoves = MAX_INT; // (was 0 - see comment below)
	for (CLLNode<IDInfo> const* pUnitNode = headUnitNode(); pUnitNode != NULL; pUnitNode = nextUnitNode(pUnitNode))
	{
		CvUnit const* pLoopUnit = ::getUnit(pUnitNode->m_data);
		if (pLoopUnit != NULL)
		{
			iMoves = std::min(iMoves, pLoopUnit->maxMoves());
			// note: in the original code, this used std::max -- I'm pretty sure that was just a mistake. I don't know why they'd want to use that.
		}
	}
	return iMoves;
}

int CvSelectionGroup::movesLeft() const
{
	int iMoves = MAX_INT;
	for (CLLNode<IDInfo> const* pUnitNode = headUnitNode(); pUnitNode != NULL; pUnitNode = nextUnitNode(pUnitNode))
	{
		CvUnit const* pLoopUnit = ::getUnit(pUnitNode->m_data);
		if (pLoopUnit != NULL)
		{
			iMoves = std::min(iMoves, pLoopUnit->movesLeft());
		}
	}
	return iMoves;
} // K-Mod end
