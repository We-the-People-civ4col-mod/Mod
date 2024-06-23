// selectionGroupAI.cpp

#include "CvGameCoreDLL.h"
#include "CvSelectionGroupAI.h"
#include "CvPlayerAI.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvTeamAI.h"
#include "CvDLLEntityIFaceBase.h"
#include "CvGameCoreUtils.h"
#include "FProfiler.h"
#include "CvInfos.h"
#include "CvTradeRoute.h"
//TAC Whaling, ray
#include "CvDLLInterfaceIFaceBase.h"
//End TAC Whaling, ray

#include "CvSavegame.h"
// Public Functions...

CvSelectionGroupAI::CvSelectionGroupAI()
{
	AI_reset();
}


CvSelectionGroupAI::~CvSelectionGroupAI()
{
	AI_uninit();
}


void CvSelectionGroupAI::AI_init()
{
	AI_reset();

	//--------------------------------
	// Init other game data
}


void CvSelectionGroupAI::AI_uninit()
{
}


void CvSelectionGroupAI::AI_reset()
{
	AI_uninit();
	AI_resetSavedData();
}


void CvSelectionGroupAI::AI_separate()
{
	CLLNode<IDInfo>* pEntityNode;
	CvUnit* pLoopUnit;

	pEntityNode = headUnitNode();

	while (pEntityNode != NULL)
	{
		pLoopUnit = ::getUnit(pEntityNode->m_data);
		pEntityNode = nextUnitNode(pEntityNode);

		if (pLoopUnit != NULL)
		{
			pLoopUnit->joinGroup(NULL);
			if (pLoopUnit->plot()->getTeam() == getTeam())
			{
				pLoopUnit->getGroup()->pushMission(MISSION_SKIP);
			}
		}
	}
}

void CvSelectionGroupAI::AI_seperateNonAI(UnitAITypes eUnitAI)
{
	CLLNode<IDInfo>* pEntityNode;
	CvUnit* pLoopUnit;

	pEntityNode = headUnitNode();

	while (pEntityNode != NULL)
	{
		pLoopUnit = ::getUnit(pEntityNode->m_data);
		pEntityNode = nextUnitNode(pEntityNode);
		if (pLoopUnit != NULL && pLoopUnit->AI_getUnitAIType() != eUnitAI)
		{
			pLoopUnit->joinGroup(NULL);
			if (pLoopUnit->plot()->getTeam() == getTeam())
			{
				pLoopUnit->getGroup()->pushMission(MISSION_SKIP);
			}
		}
	}
}

void CvSelectionGroupAI::AI_seperateAI(UnitAITypes eUnitAI)
{
	CLLNode<IDInfo>* pEntityNode;
	CvUnit* pLoopUnit;

	pEntityNode = headUnitNode();

	while (pEntityNode != NULL)
	{
		pLoopUnit = ::getUnit(pEntityNode->m_data);
		pEntityNode = nextUnitNode(pEntityNode);
		if (pLoopUnit != NULL && pLoopUnit->AI_getUnitAIType() == eUnitAI)
		{
			pLoopUnit->joinGroup(NULL);
			// TAC - AI Assault Sea - koma13, jdog5000(BBAI)
			// Was potential crash in use of plot() if group emptied
			//if (plot()->getTeam() == getTeam())
			if (pLoopUnit->plot()->getTeam() == getTeam())
			// TAC - AI Assault Sea - koma13, jdog5000(BBAI)
			{
				pLoopUnit->getGroup()->pushMission(MISSION_SKIP);
			}
		}
	}
}
// TAC - AI Assault Sea - koma13, jdog5000(BBAI)
void CvSelectionGroupAI::AI_separateEmptyTransports()
{
	CLLNode<IDInfo>* pEntityNode;
	CvUnit* pLoopUnit;

	pEntityNode = headUnitNode();

	while (pEntityNode != NULL)
	{
		pLoopUnit = ::getUnit(pEntityNode->m_data);
		pEntityNode = nextUnitNode(pEntityNode);
		if (pLoopUnit != NULL && (pLoopUnit->AI_getUnitAIType() == UNITAI_ASSAULT_SEA) && (pLoopUnit->getCargo() == 0))
		{
			pLoopUnit->joinGroup(NULL);
			if (pLoopUnit->plot()->getTeam() == getTeam())
			{
				pLoopUnit->getGroup()->pushMission(MISSION_SKIP);
			}
		}
	}
}
// TAC - AI Assault Sea - koma13, jdog5000(BBAI)

// Returns true if the group has become busy...
bool CvSelectionGroupAI::AI_update()
{
	CLLNode<IDInfo>* pEntityNode;
	CvUnit* pLoopUnit;
	bool bDead;
	bool bFollow;

	PROFILE_FUNC();

	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (!AI_isControlled())
	{
		return false;
	}

	if (getNumUnits() == 0)
	{
		return false;
	}

	if (isForceUpdate())
	{
		clearMissionQueue(); // XXX ???
		setActivityType(ACTIVITY_AWAKE);
		setForceUpdate(false);

		// if we are in the middle of attacking with a stack, cancel it
		AI_cancelGroupAttack();
	}

	FAssert(!(GET_PLAYER(getOwnerINLINE()).isAutoMoves()));

	int iTempHack = 0; // XXX

	bDead = false;

	bool bFailedAlreadyFighting = false;
	while ((m_bGroupAttack && !bFailedAlreadyFighting) || readyToMove())
	{
		iTempHack++;
		if (iTempHack > 100)
		{
			FAssert(false);
			CvUnit* pHeadUnit = getHeadUnit();
			if (NULL != pHeadUnit)
			{
				if (GC.getLogging())
				{
					char szOut[1024];
					CvWString szTempString;
					getUnitAIString(szTempString, pHeadUnit->AI_getUnitAIType());
					sprintf(szOut, "Unit stuck in loop: %S(%S)[%d, %d] (%S)", pHeadUnit->getName().GetCString(), GET_PLAYER(pHeadUnit->getOwnerINLINE()).getName(),
						pHeadUnit->getX_INLINE(), pHeadUnit->getY_INLINE(), szTempString.GetCString());
					gDLL->messageControlLog(szOut);
				}

				pHeadUnit->finishMoves();
			}
			break;
		}

		// if we want to force the group to attack, force another attack
		if (m_bGroupAttack)
		{
			m_bGroupAttack = false;

			groupAttack(CREATE_ASSERT_DATA, m_iGroupAttackX, m_iGroupAttackY, MOVE_DIRECT_ATTACK, bFailedAlreadyFighting);
		}
		// else pick AI action
		else
		{
			CvUnit* pHeadUnit = getHeadUnit();

			if (pHeadUnit == NULL || pHeadUnit->isDelayedDeath())
			{
				break;
			}

			if (pHeadUnit->AI_update())
			{
				// AI_update returns true when we should abort the loop and wait until next slice
				break;
			}
		}

		if (doDelayedDeath())
		{
			bDead = true;
			break;
		}

		// if no longer group attacking, and force separate is true, then bail, decide what to do after group is split up
		// (UnitAI of head unit may have changed)
		if (!m_bGroupAttack && AI_isForceSeparate())
		{
			AI_separate();	// pointers could become invalid...
			return true;
		}
	}

	if (!bDead)
	{
		CvUnit* pHeadUnit = getHeadUnit();
		if (pHeadUnit != NULL)
		{
			if ((pHeadUnit->getUnitTravelState() == UNIT_TRAVEL_STATE_IN_EUROPE || pHeadUnit->getUnitTravelState() == UNIT_TRAVEL_STATE_IN_AFRICA) && AI_isControlled())
			{
				pEntityNode = headUnitNode();

				while (pEntityNode != NULL)
				{
					pLoopUnit = ::getUnit(pEntityNode->m_data);
					pEntityNode = nextUnitNode(pEntityNode);

					if (pLoopUnit != NULL)
					{
						pLoopUnit->AI_europeUpdate();
					}
				}
			}
		}

		if (!isHuman())
		{
			bFollow = false;

			// if we not group attacking, then check for follow action
			if (!m_bGroupAttack)
			{
				pEntityNode = headUnitNode();

				while ((pEntityNode != NULL) && readyToMove(true))
				{
					pLoopUnit = ::getUnit(pEntityNode->m_data);
					pEntityNode = nextUnitNode(pEntityNode);

					if (pLoopUnit != NULL && pLoopUnit->canMove())
					{
						if (pLoopUnit->AI_follow())
						{
							bFollow = true;
							break;
						}
					}
				}
			}

			if (doDelayedDeath())
			{
				bDead = true;
			}

			if (!bDead)
			{
				if (!bFollow && readyToMove(true))
				{
					pushMission(MISSION_SKIP);
				}
			}
		}
	}

	if (bDead)
	{
		return true;
	}

	return (isBusy() || isCargoBusy());
}


// Returns attack odds out of 100 (the higher, the better...)
int CvSelectionGroupAI::AI_attackOdds(const CvPlot* pPlot, bool bPotentialEnemy) const
{
	PROFILE_FUNC();

	CvUnit* pAttacker;

	FAssert(getOwnerINLINE() != NO_PLAYER);

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD                      02/21/10                                jdog5000      */
	/*                                                                                              */
	/* Efficiency, Lead From Behind                                                                 */
	/************************************************************************************************/
	// From Lead From Behind by UncutDragon
	// original
	//if (pPlot->getBestDefender(NO_PLAYER, getOwnerINLINE(), NULL, !bPotentialEnemy, bPotentialEnemy) == NULL)
	// modified
	if (!pPlot->hasDefender(false, NO_PLAYER, getOwnerINLINE(), NULL, !bPotentialEnemy, bPotentialEnemy))
		/************************************************************************************************/
		/* BETTER_BTS_AI_MOD                       END                                                  */
		/************************************************************************************************/
	{
		return 100;
	}

	int iOdds = 0;
	pAttacker = AI_getBestGroupAttacker(pPlot, bPotentialEnemy, iOdds);

	if (pAttacker == NULL)
	{
		return 0;
	}

	return iOdds;
}

/*	K-Mod. A new odds-adjusting function to replace CvUnitAI::AI_finalOddsThreshold.
	(note: I would like to put this in CvSelectionGroupAI ... but - well -
	I don't need to say it, right?)
	advc.003u: I think CvUnitAI::AI_getGroup solves karadoc's problem; so - moved. */
int CvSelectionGroupAI::AI_getWeightedOdds(CvPlot const* pPlot, bool bPotentialEnemy)
{
	PROFILE_FUNC();
	int iOdds = -1;
	CvUnitAI const* pAttacker = AI_getBestGroupAttacker(pPlot, bPotentialEnemy, iOdds);
	if (pAttacker == NULL)
		return 0;
	/*
	CvPlot::DefenderFilters defFilters(getOwner(), pAttacker,
		!bPotentialEnemy, bPotentialEnemy,
		true, false); // advc.028, advc.089 (same as in CvUnitAI::AI_attackOdds)
	*/
	//CvUnit const* pDefender = pPlot->getBestDefender(NO_PLAYER, defFilters);
	CvUnit const* pDefender = pPlot->getBestDefender(NO_PLAYER, getOwnerINLINE(), 
		pAttacker, !bPotentialEnemy, bPotentialEnemy);
	if (pDefender == NULL)
		return 100;

	/*	<advc.114b> We shouldn't adjust the odds based on an optimistic estimate
		(increased by AttackOddsChange) b/c that leads to Warriors attacking Tanks -
		high difference in production cost and non-negligible optimistic odds.
		I'm subtracting the AttackOddsChange temporarily;
		adding them back in after the adjustments are done.
		(A more elaborate fix would avoid adding them in the first place.) */
	int const iAttackOddsChange = GET_PLAYER(getOwner()).AI_getAttackOddsChange();
	iOdds -= iAttackOddsChange;
	/*	Require a stack of at least 3 if actual odds are below 1%. Should
		matter mostly for Barbarians, hence only this primitive condition
		(not checking if the other units could actually attack etc.). */
	if (iOdds == 0 && getNumUnits() < 3)
		return 0;
	// </advc.114b>
	// advc: The bulk of the computation is still in CvUnitAI
	int iAdjustedOdds = pAttacker->AI_opportuneOdds(iOdds, *pDefender);

	/*  one more thing... unfortunately, the sea AI isn't evolved enough
		to do without something as painful as this... */
	if (getDomainType() == DOMAIN_SEA && !hasCargo())
	{
		// I'm sorry about this. I really am. I'll try to make it better one day...
		int iDefenders = pAttacker->AI_countEnemyDefenders(*pPlot);
		iAdjustedOdds *= 2 + getNumUnits();
		iAdjustedOdds /= 3 + std::min(iDefenders / 2, getNumUnits());
	}

	iAdjustedOdds += iAttackOddsChange; // advc.114b
	return range(iAdjustedOdds, 1, 99);
}

CvUnitAI* CvSelectionGroupAI::AI_getBestGroupAttacker(const CvPlot* pPlot, 
	bool bPotentialEnemy, int& iUnitOdds, bool bForce, bool bNoBlitz) const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnitAI* pBestUnit = NULL;
	int iValue;
	int iBestValue;
	int iOdds;
	int iBestOdds;

	iBestValue = 0;
	iBestOdds = 0;
	
	pUnitNode = headUnitNode();

	bool bIsHuman = (pUnitNode != NULL) ? GET_PLAYER(::getUnit(pUnitNode->m_data)->getOwnerINLINE()).isHuman() : true;

	while (pUnitNode != NULL)
	{
		CvUnitAI* pLoopUnit = static_cast<CvUnitAI*>(::getUnit(pUnitNode->m_data));
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit != NULL && !pLoopUnit->isDead())
		{
			bool bCanAttack = false;
			bCanAttack = pLoopUnit->canAttack();

			if (bCanAttack && bNoBlitz && pLoopUnit->isBlitz() && pLoopUnit->isMadeAttack())
			{
				bCanAttack = false;
			}

			// attack bug fix - start - Nightinggale
			if (bCanAttack && !pLoopUnit->canMoveInto(*pPlot, true))
			{
				// Don't let the AI attack with this unit as it can't move into the plot in question.
				bCanAttack = false;
			}
			// attack bug fix - end - Nightinggale

			if (bCanAttack)
			{
				if (bForce || pLoopUnit->canMove())
				{
					if (bForce || pLoopUnit->canMoveInto(*pPlot, /*bAttack*/ true, /*bDeclareWar*/ bPotentialEnemy))
					{
						iOdds = pLoopUnit->AI_attackOdds(pPlot, bPotentialEnemy);

						iValue = iOdds;
						FAssertMsg(iValue > 0, "iValue is expected to be greater than 0");

						// if non-human, prefer the last unit that has the best value (so as to avoid splitting the group)
						if (iValue > iBestValue || (!bIsHuman && iValue > 0 && iValue == iBestValue))
						{
							iBestValue = iValue;
							iBestOdds = iOdds;
							pBestUnit = pLoopUnit;
						}
					}
				}
			}
		}
	}

	iUnitOdds = iBestOdds;
	return pBestUnit;
}

CvUnitAI* CvSelectionGroupAI::AI_getBestGroupSacrifice(const CvPlot* pPlot, bool bPotentialEnemy, bool bForce, bool bNoBlitz) const
{
	int iBestValue = 0;
	CvUnitAI* pBestUnit = NULL;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnitAI* pLoopUnit = static_cast<CvUnitAI*>(::getUnit(pUnitNode->m_data));
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit != NULL && !pLoopUnit->isDead())
		{
			bool bCanAttack = false;
			bCanAttack = pLoopUnit->canAttack();

			if (bCanAttack && bNoBlitz && pLoopUnit->isBlitz() && pLoopUnit->isMadeAttack())
			{
				bCanAttack = false;
			}

			if (bCanAttack)
			{
				if (bForce || pLoopUnit->canMove())
				{
					if (bForce || pLoopUnit->canMoveInto(*pPlot, true))
					{
                        int iValue = pLoopUnit->AI_sacrificeValue(pPlot);
						FAssertMsg(iValue > 0, "iValue is expected to be greater than 0");

						// we want to pick the last unit of highest value, so pick the last unit with a good value
						if (iValue >= iBestValue)
						{
							iBestValue = iValue;
							pBestUnit = pLoopUnit;
						}
					}
				}
			}
		}
	}

	return pBestUnit;
}

// Returns ratio of strengths of stacks times 100
// (so 100 is even ratio, numbers over 100 mean this group is more powerful than the stack on a plot)
int CvSelectionGroupAI::AI_compareStacks(const CvPlot* pPlot, bool bPotentialEnemy, bool bCheckCanAttack, bool bCheckCanMove) const
{
	FAssert(pPlot != NULL);

	int	compareRatio;
	DomainTypes eDomainType = getDomainType();

	// choose based on the plot, not the head unit (mainly for transport carried units)
	if (pPlot->isWater())
		eDomainType = DOMAIN_SEA;
	else
		eDomainType = DOMAIN_LAND;

	compareRatio = AI_sumStrength(pPlot, eDomainType, bCheckCanAttack, bCheckCanMove);
	compareRatio *= 100;

	PlayerTypes eOwner = getOwnerINLINE();
	if (eOwner == NO_PLAYER)
	{
		eOwner = getHeadOwner();
	}
	FAssert(eOwner != NO_PLAYER);

	int defenderSum = pPlot->AI_sumStrength(NO_PLAYER, getOwnerINLINE(), eDomainType, true, !bPotentialEnemy, bPotentialEnemy);
	compareRatio /= std::max(1, defenderSum);

	return compareRatio;
}

/*  K-Mod. I've removed bCheckMove, and changed bCheckCanAttack to include checks
	for moves, and for hasAlreadyAttacked / blitz */
	/*  advc.159: No longer simply a sum of combat strength values; see the comment
		above CvPlayerAI::AI_localDefenceStrength. */
int CvSelectionGroupAI::AI_sumStrengthInternal(const CvPlot* pAttackedPlot,
	DomainTypes eDomainType, bool bCheckCanAttack) const
{
	FAssert(eDomainType != DOMAIN_IMMOBILE); // advc: Air combat strength isn't counted
	// <K-Mod>
	bool const bDefenders = (pAttackedPlot ?
		pAttackedPlot->isVisibleEnemyUnit(getHeadOwner()) : false);
	//bool const bCountCollateral = (pAttackedPlot && pAttackedPlot != plot()); // </K-Mod>
	//int const iBaseCollateral = (bCountCollateral ?
	//	estimateCollateralWeight(pAttackedPlot, getTeam()) : 0);
	int	iSum = 0;
	FOR_EACH_UNITAI_IN(pUnit, *this)
	{
		if (pUnit->isDead() ||
			// advc.opt: (If we want to count air units, then this'll have to be removed.)
			!pUnit->canFight())
		{
			continue;
		}
		if (eDomainType != NO_DOMAIN && pUnit->getDomainType() != eDomainType)
			continue; // advc: Moved up
		// K-Mod. (original checks deleted.)
		if (bCheckCanAttack)
		{
			// advc.opt: currEffectiveStr is 0 for air units anyway
			/*if (pUnit->getDomainType() == DOMAIN_AIR)
			{
				if (!pUnit->canAirAttack() || !pUnit->canMove() ||
					(pAttackedPlot != NULL && bDefenders &&
					!pUnit->canMoveInto(*pAttackedPlot, true, true)))
				{
					continue; // can't attack.
				}
			}
			else*/
			if (!pUnit->canAttack() || !pUnit->canMove() ||
				(pAttackedPlot && bDefenders &&
					!pUnit->canMoveInto(*pAttackedPlot, true, true)) ||
				//(!pUnit->isBlitz() && pUnit->isMadeAttack())
				//pUnit->isMadeAllAttacks()) // advc.164
				pUnit->isMadeAttack())
			{
				continue; // can't attack.
			}
		} // K-Mod end

		// iSum += pLoopUnit->currEffectiveStr(pAttackedPlot, pLoopUnit);
		/*	K-Mod estimate the value of first strike
			and the attack power of collateral units.
			(cf with calculation in CvPlayerAI::AI_localAttackStrength) */
			/*  <advc.159> Call AI_currEffectiveStr instead of currEffectiveStr.
				Adjustments for first strikes and collateral damage moved into
				that new function. */
		/*
		int const iUnitStr = pUnit->AI_currEffectiveStr(pAttackedPlot, pUnit,
			bCountCollateral, iBaseCollateral, bCheckCanAttack);
		*/
		// WTP: No collateral damage support
		int const iUnitStr = pUnit->AI_currEffectiveStr(pAttackedPlot, pUnit,
			false, 0, bCheckCanAttack);
		// </advc.159>
		iSum += iUnitStr;
		// K-Mod end
	}
	return iSum;
}

void CvSelectionGroupAI::AI_queueGroupAttack(int iX, int iY)
{
	m_bGroupAttack = true;

	m_iGroupAttackX = iX;
	m_iGroupAttackY = iY;
}

inline void CvSelectionGroupAI::AI_cancelGroupAttack()
{
	m_bGroupAttack = false;
}

inline bool CvSelectionGroupAI::AI_isGroupAttack()
{
	return m_bGroupAttack;
}

bool CvSelectionGroupAI::AI_isControlled() const
{
	return (!isHuman() || isAutomated());
}


bool CvSelectionGroupAI::AI_isDeclareWarInternal(const CvPlot* pPlot) const
{
	FAssert(getHeadUnit() != NULL);

	if (isHuman())
	{
		return false;
	}
	else
	{
		bool bLimitedWar = false;
		if (pPlot != NULL)
		{
			TeamTypes ePlotTeam = pPlot->getTeam();
			if (ePlotTeam != NO_TEAM)
			{
				WarPlanTypes eWarplan = GET_TEAM(getTeam()).AI_getWarPlan(ePlotTeam);
				if (eWarplan == WARPLAN_LIMITED)
				{
					bLimitedWar = true;
				}
			}
		}

		CvUnit* pHeadUnit = getHeadUnit();

		if (pHeadUnit != NULL)
		{
			switch (pHeadUnit->AI_getUnitAIType())
			{
			case UNITAI_UNKNOWN:
			case UNITAI_ANIMAL: // R&R, ray, Wild Animals
			case UNITAI_ANIMAL_SEA: // R&R, ray, Wild Animals
			case UNITAI_FLEEING: // R&R, ray, Fleeing Units
			case UNITAI_COLONIST:
			case UNITAI_SETTLER:
			case UNITAI_WORKER:
			case UNITAI_MISSIONARY:
			case UNITAI_TRADER: // WTP, ray, Native Trade Posts - START
			case UNITAI_SCOUT:
			case UNITAI_WAGON:
			case UNITAI_TREASURE:
			case UNITAI_YIELD:
			case UNITAI_GENERAL:
				return false;
				break;

			case UNITAI_DEFENSIVE:
			case UNITAI_OFFENSIVE:
			case UNITAI_COUNTER:
			case UNITAI_ATTACK_CITY:
				return true;
				break;
			//TAC Whaling, ray
			case UNITAI_WORKER_SEA:
			//End TAC Whaling, ray
			case UNITAI_TRANSPORT_SEA:
			case UNITAI_TRANSPORT_COAST:
				return false;
				break;

			case UNITAI_ASSAULT_SEA:
			case UNITAI_COMBAT_SEA:
			case UNITAI_PIRATE_SEA:
			case UNITAI_ESCORT_SEA:			// TAC - AI Escort Sea - koma13
				return true;
				break;

			default:
				FAssert(false);
				break;
			}
		}
	}

	return false;
}


CvPlot* CvSelectionGroupAI::AI_getMissionAIPlotInternal() const
{
	return GC.getMap().plotSoren(m_iMissionAIX, m_iMissionAIY);
}


bool CvSelectionGroupAI::AI_isForceSeparateInternal() const
{
	return m_bForceSeparate;
}


void CvSelectionGroupAI::AI_makeForceSeparate()
{
	m_bForceSeparate = true;
}


MissionAITypes CvSelectionGroupAI::AI_getMissionAITypeInternal() const
{
	return m_eMissionAIType;
}


void CvSelectionGroupAI::AI_setMissionAI(MissionAITypes eNewMissionAI, CvPlot* pNewPlot, CvUnit* pNewUnit)
{
	m_eMissionAIType = eNewMissionAI;

	if (pNewPlot != NULL)
	{
		m_iMissionAIX = pNewPlot->getX_INLINE();
		m_iMissionAIY = pNewPlot->getY_INLINE();
	}
	else
	{
		m_iMissionAIX = INVALID_PLOT_COORD;
		m_iMissionAIY = INVALID_PLOT_COORD;
	}

	if (pNewUnit != NULL)
	{
		m_missionAIUnit = pNewUnit->getIDInfo();
	}
	else
	{
		m_missionAIUnit.reset();
	}
}


CvUnit* CvSelectionGroupAI::AI_getMissionAIUnit()
{
	return getUnit(m_missionAIUnit);
}

bool CvSelectionGroupAI::AI_isFull()
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	if (getNumUnits() > 0)
	{
		UnitAITypes eUnitAI = getHeadUnitAI();
		// do two passes, the first pass, we ignore units with speical cargo
		int iSpecialCargoCount = 0;
		int iCargoCount = 0;

		// first pass, count but ignore special cargo units
		pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);
			if (pLoopUnit != NULL && pLoopUnit->AI_getUnitAIType() == eUnitAI)
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
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);

				if (pLoopUnit != NULL && pLoopUnit->AI_getUnitAIType() == eUnitAI)
				{
					if (!(pLoopUnit->isFull()))
					{
						return false;
					}
				}
			}
		}

		return true;
	}

	return false;
}


bool CvSelectionGroupAI::AI_launchAssault(CvPlot* pTargetCityPlot)
{
	std::multimap<int, CvUnit*, std::greater<int> > units;
	std::multimap<int, CvUnit*, std::greater<int> >::iterator units_it;

	CLLNode<IDInfo>* pUnitNode = plot()->headUnitNode();
	CvUnit* pLoopUnit;

    while (pUnitNode != NULL)
    {
		pLoopUnit = plot()->getUnitNodeLoop(pUnitNode);

        if (pLoopUnit != NULL && pLoopUnit->isCargo())
        {
            if (pLoopUnit->getTransportUnit()->getGroup() == this)
            {
                int iValue = pLoopUnit->baseCombatStr();
                if (pLoopUnit->canAttack())
                {
                    iValue *= 10;
                }
                iValue *= 100;

                units.insert(std::make_pair(iValue, pLoopUnit));
            }
        }
    }

    if (units.empty())
    {
        return false;
    }

    bool bAction = false;
    for (units_it = units.begin(); units_it != units.end(); ++units_it)
    {
        pLoopUnit = units_it->second;
		if (pLoopUnit->canMove() && pLoopUnit->canAttack())
		{
//			if (pLoopUnit->AI_attackFromTransport(NULL, 40, 80))
//			{
//			    bAction = true;
//			}
            int iPriority = 41;
            pLoopUnit->AI_setMovePriority(iPriority);
		}
    }
//    for (units_it = units.begin(); units_it != units.end(); ++units_it)
//    {
//        pLoopUnit = units_it->second;
//        if (pLoopUnit->canMove())
//        {
//            if (pLoopUnit->AI_moveFromTransport(NULL))
//            {
//                bAction = true;
//            }
//        }
//    }
//    for (units_it = units.begin(); units_it != units.end(); ++units_it)
//    {
//        pLoopUnit = units_it->second;
//		if (pLoopUnit->canMove() && pLoopUnit->canAttack())
//		{
//			if (pLoopUnit->AI_attackFromTransport(NULL, 0, 100))
//			{
//			    bAction = true;
//			}
//		}
//    }
    if (bAction)
    {
	    //pushMission(MISSION_SKIP);
	    return true;
    }

    return false;

}

void CvSelectionGroupAI::AI_groupBombard()
{
 	CLLNode<IDInfo>* pEntityNode = headUnitNode();
	CvUnit* pLoopUnit = NULL;

	while (pEntityNode != NULL)
	{
		pLoopUnit = ::getUnit(pEntityNode->m_data);
		pEntityNode = nextUnitNode(pEntityNode);

		if (pLoopUnit != NULL && pLoopUnit->canBombard(plot()))
		{
		    pLoopUnit->bombard();
		}
	}
}

// Erik: I've changed this function to return the total amount of yields stored
// rather than the amount of treasure since that wasn't used anyway
int CvSelectionGroupAI::AI_getYieldsLoaded(short* piYields) const
{
	FAssert(piYields != NULL);

	int iAmount = 0;

	CLinkList<IDInfo> unitList;
	buildCargoUnitList(unitList);

	CLLNode<IDInfo>* pUnitNode = unitList.head();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = unitList.next(pUnitNode);

		// Should not be possible, but let's add this for safety
		if (pLoopUnit == NULL)
			continue;

		if (pLoopUnit->getYieldStored() > 0)
		{
			if (!pLoopUnit->getUnitInfo().isTreasure() && pLoopUnit->getYield() != NO_YIELD)
			{
				piYields[pLoopUnit->getYield()] += pLoopUnit->getYieldStored();
				iAmount += pLoopUnit->getYieldStored();
			}
		}
	}

	return iAmount;
}

void CvSelectionGroupAI::processTradeRoute(CvTradeRoute* pRoute, std::map<IDInfo, int>& cityValues, std::vector<CvTradeRoute*>& routes, std::vector<int>& routeValues, std::vector<bool>& yieldsDelivered, std::vector<bool>& yieldsToUnload)
{
	routes.push_back(pRoute);
	routeValues.push_back(0);

	yieldsDelivered[pRoute->getYield()] = true;
	CvCity* pPlotCity = plot()->getPlotCity();

	if (pPlotCity != NULL && ::getCity(pRoute->getDestinationCity()) == pPlotCity)
	{
		yieldsToUnload[pRoute->getYield()] = true;
	}

	cityValues[pRoute->getSourceCity()] = 0;
	cityValues[pRoute->getDestinationCity()] = 0;
}

bool CvSelectionGroupAI::AI_tradeRoutes()
{
	PROFILE_FUNC();

	const IDInfo kEurope(getOwnerINLINE(), CvTradeRoute::EUROPE_CITY_ID);

	CvCity* pPlotCity = plot()->getPlotCity();
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	std::set<int>::iterator it;

	std::map<IDInfo, int> cityValues;

	std::vector<CvTradeRoute*> routes;
	std::vector<int> routeValues;

	std::vector<bool> yieldsDelivered(NUM_YIELD_TYPES, false);
	std::vector<bool> yieldsToUnload(NUM_YIELD_TYPES, false);
	std::vector<int> yieldsOnBoard(NUM_YIELD_TYPES, false);

	// R&R mod, vetiarvind, max yield import limit - start
	const bool bIgnoreDanger = getIgnoreDangerStatus();
	// R&R mod, vetiarvind, max yield import limit - end

	// Erik: We need to determine if we're dealing with a coastal transport
	bool bCoastalTransport = false;
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit != NULL)
		{
			UnitAITypes aiType = pLoopUnit->AI_getUnitAIType();

			if (aiType == UNITAI_TRANSPORT_COAST)
			{
				bCoastalTransport = true;
				break;
			}
		}
	}

	if (!isHuman() || (getAutomateType() == AUTOMATE_TRANSPORT_FULL))
	{
		std::vector<CvTradeRoute*> aiRoutes;
		kOwner.getTradeRoutes(aiRoutes);
		for (uint i = 0; i < aiRoutes.size(); ++i)
		{
			CvTradeRoute* pRoute = aiRoutes[i];

			// transport feeder - start - Nightinggale
			CvCity* pDestinationCity = ::getCity(pRoute->getDestinationCity());
			if (pDestinationCity != NULL && pDestinationCity->isAutoImportStopped(pRoute->getYield()))
			{
				// ignore trade routes where destination is using feeder service and is full
				continue;
			}
			// transport feeder - end - Nightinggale

			// traderoute fix - start - Nightinggale
			if (isHuman() && (pRoute->getDestinationCity().eOwner != getOwnerINLINE() || (pRoute->getDestinationCity() == kEurope)))
			{
				// humans can't transport to allied cities with fully automated transports
				// human transport can't go to Europe automatically
				continue;
			}
			// traderoute fix - end - Nightinggale

			// Erik: Coastal transports cannot have europe as their destination
			if (bCoastalTransport && (pRoute->getDestinationCity().eOwner != getOwnerINLINE() || (pRoute->getDestinationCity() == kEurope)))
				continue;

			CvCity* pSourceCity = ::getCity(pRoute->getSourceCity());
			CvArea* pSourceWaterArea = pSourceCity->waterArea();
			// R&R, vetiarvind, max trade capacity  - start
			const DomainTypes domainType = getDomainType();
			// R&R, vetiarvind, max trade capacity  - end

			if ((pSourceCity != NULL) && ((domainType != DOMAIN_SEA) || (pSourceWaterArea != NULL)))
			{
				int iSourceArea = (domainType == DOMAIN_SEA) ? pSourceWaterArea->getID() : pSourceCity->getArea();
				if (domainType == DOMAIN_SEA ? plot()->isAdjacentToArea(iSourceArea) : (iSourceArea == getArea()) ||
					domainType == DOMAIN_LAND && plot()->getTerrainType() == TERRAIN_LARGE_RIVERS && plot()->isAdjacentToArea(pSourceCity->getArea()))
				{
					if ((domainType == DOMAIN_SEA) || (pRoute->getDestinationCity() != kEurope))
					{
						processTradeRoute(pRoute, cityValues, routes, routeValues, yieldsDelivered, yieldsToUnload);
					}
				}
				// TODO: Need to path find if the area check fails like below
			}
		}
	}
	else //human or non-automate-full case: just uses the unit's routes but the same logic as above..can be refactored as a function
	{
		for (it = m_aTradeRoutes.begin(); it != m_aTradeRoutes.end(); ++it)
		{
			CvTradeRoute* pRoute = kOwner.getTradeRoute(*it);
			CvCity* pSourceCity = ::getCity(pRoute->getSourceCity());
			if (pSourceCity != NULL)
			{
				const DomainTypes domainType = getDomainType();

				CvArea* pSourceWaterArea = pSourceCity->waterArea();
				if (getDomainType() != DOMAIN_SEA || pSourceWaterArea != NULL) //land or good water..this if block is basically the same code as the AI_full_automate case
				{
					int iSourceArea = (getDomainType() == DOMAIN_SEA) ? pSourceWaterArea->getID() : pSourceCity->getArea();
					if (domainType == DOMAIN_SEA ? plot()->isAdjacentToArea(iSourceArea) : (iSourceArea == getArea()) ||
						domainType == DOMAIN_LAND && plot()->getTerrainType() == TERRAIN_LARGE_RIVERS && plot()->isAdjacentToArea(pSourceCity->getArea()))
					{
						if ((domainType == DOMAIN_SEA) || (pRoute->getDestinationCity() != kEurope))
						{
							processTradeRoute(pRoute, cityValues, routes, routeValues, yieldsDelivered, yieldsToUnload);
						}
					}
					else
					{
						// Due to the introduction of the large river feature, the area check may not be sufficient. This is the case
						// if the source city is in a different area, connected by river fords \ ferry stations
						const bool res = generatePath(plot(), pSourceCity->plot(), (bIgnoreDanger ? MOVE_IGNORE_DANGER : MOVE_NO_ENEMY_TERRITORY), true);

						if (res)
						{
							processTradeRoute(pRoute, cityValues, routes, routeValues, yieldsDelivered, yieldsToUnload);
						}
						else
						{
							FAssertMsg(false, "Unexpected : Unit can't run trade route it's assigned to");
						}
					}

				}
			}
		}
	} // human or non-automate-full

	if ((pPlotCity != NULL) && hasCargo())
	{
		std::vector<CvUnit*> units;

		//Unload everything which we should unload here, or can't unload anywhere...
		CLLNode<IDInfo>* pUnitNode = plot()->headUnitNode();
		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = plot()->getUnitNodeLoop(pUnitNode);

			if (pLoopUnit != NULL)
			{
				YieldTypes eYield = pLoopUnit->getYield();
				CvUnit* pTransport = pLoopUnit->getTransportUnit();

				if ((eYield != NO_YIELD) && pTransport != NULL && (yieldsToUnload[eYield] || !(yieldsDelivered[eYield])))
				{
					if (pTransport->getGroup() == this && pLoopUnit->canUnload())
					{
						units.push_back(pLoopUnit);
					}
				}
			}
		}

		for (uint i = 0; i < units.size(); ++i)
		{
			// R&R mod, vetiarvind, max yield import limit - start
			//units[i]->unload();
			unloadToCity(pPlotCity, units[i], UnloadMode::Force);
			// R&R mod, vetiarvind, max yield import limit - end
		}

		// It may happen that the destination has stopped importing the yield, if so search for
		// another nearby city that imports the yield. If that fails, force unload the yield
		// as a last resort if not a single city imports this yield so that we can free up
		// the storage space
		// Note that this is very rate since the presence of a port city in any given area
		// will act as a "sink" for excess yields of any type

	}

	short aiYieldsLoaded[NUM_YIELD_TYPES];

	std::fill(aiYieldsLoaded, aiYieldsLoaded + NUM_YIELD_TYPES, 0);
	const bool bNoCargo = (AI_getYieldsLoaded(aiYieldsLoaded) == 0);

	if (!bNoCargo)
	{
		//We need to iterate over every destination city and see if we can unload.
		for (uint i = 0; i < routes.size(); ++i)
		{
			CvCity* pDestinationCity = ::getCity(routes[i]->getDestinationCity());
			if ((pDestinationCity != NULL) && (pDestinationCity != pPlotCity))
			{
				// R&R mod, vetiarvind, max yield import limit - start

				YieldTypes eYield = routes[i]->getYield();
				int yieldsToUnload = aiYieldsLoaded[eYield];
				if(pDestinationCity != NULL && pDestinationCity->getMaxImportAmount(eYield) > 0)
				{
					int turnsToReach = 0;
					generatePath(plot(), pDestinationCity->plot(), (bIgnoreDanger ? MOVE_IGNORE_DANGER : MOVE_NO_ENEMY_TERRITORY), true, &turnsToReach);
					yieldsToUnload = std::min(yieldsToUnload, estimateYieldsToLoad(pDestinationCity, 9999, eYield, turnsToReach, 0));
				}
				//int iRouteValue = kOwner.AI_transferYieldValue(routes[i]->getDestinationCity(), routes[i]->getYield(), aiYieldsLoaded[routes[i]->getYield()]);
				int iRouteValue = kOwner.AI_transferYieldValue(routes[i]->getDestinationCity(), routes[i]->getYield(), yieldsToUnload);
				// R&R mod, vetiarvind, max yield import limit - end

				if (iRouteValue > 0)
				{
					cityValues[routes[i]->getDestinationCity()] += iRouteValue;
					routeValues[i] += iRouteValue;
				}
			}
		}
	}

	bool bNoRoute = true;

	//We need to iterate over every source city, and see if there's anything which needs moving to the respective destination city.
	//We apply some bias to the city we are presently at, but not too much - sometimes empty runs need to be made...
	//Basically this looks at the entire NEXT trade run (source-city to dest-city), with some bias given towards
	//starting it from pPlotCity as sourceCity.
	//If we are carrying cargo, only count cities where we can unload.
	for (uint i = 0; i < routes.size(); ++i)
	{
		CvCity* pSourceCity = ::getCity(routes[i]->getSourceCity());

		if ((pSourceCity != NULL) && (bNoCargo || (cityValues[routes[i]->getSourceCity()] > 0)))
		{
			CvCity* pDestinationCity = ::getCity(routes[i]->getDestinationCity());
			YieldTypes eYield = routes[i]->getYield();

			// transport feeder - start - Nightinggale
			//int iAmount = pSourceCity->getYieldStored(eYield) - pSourceCity->getMaintainLevel(eYield);
			int iAmount = pSourceCity->getYieldStored(eYield) - pSourceCity->getAutoMaintainThreshold(eYield);
			// transport feeder - end - Nightinggale
			// R&R mod, vetiarvind, max yield import limit - start
			if(pDestinationCity != NULL && pDestinationCity->getMaxImportAmount(eYield) > 0)
			{
				int turnsToReachToSource = 0, turnsToReachFromSourceToDest = 0;
				const bool bSourceOk = generatePath(pSourceCity->plot(), plot(), (bIgnoreDanger ? MOVE_IGNORE_DANGER : MOVE_NO_ENEMY_TERRITORY), true, &turnsToReachToSource);
				const bool bDestOk = generatePath(pSourceCity->plot(), pDestinationCity->plot(), (bIgnoreDanger ? MOVE_IGNORE_DANGER : MOVE_NO_ENEMY_TERRITORY), true, &turnsToReachFromSourceToDest);

				if (!(bSourceOk && bDestOk))
					// We require both of these paths to be valid. If not, we skip this route
					continue;

				// At least one destination is reachable
				bNoRoute = false;

				// Erik: If we can travel from the current plot to the source, and then from source to destination in the same turn,
				// we have to make sure that we don't overestimate the amount that we should load
				int turnsRequired;
				if (turnsToReachToSource == turnsToReachFromSourceToDest)
				{
					// No need to add both legs of the journey since that would have the transport load too much cargo for the destination city
					turnsRequired = turnsToReachToSource + turnsToReachFromSourceToDest - 2;
					// In case generatePath could ever return 0
					turnsRequired = std::max(0, turnsRequired);
				}
				else
				{
					// Slightly underestimate the cargo we should carry (we cannot get this 100% correct since it depends on fractional movement,
					// other transports, consumption changes at the destination etc.
					turnsRequired = std::max(turnsToReachToSource, turnsToReachFromSourceToDest);
				}

				iAmount = estimateYieldsToLoad(pDestinationCity, iAmount, eYield, turnsRequired, aiYieldsLoaded[eYield]);
			}
			// Note that Europe has no import limit!
			else if (pDestinationCity == NULL && routes[i]->getDestinationCity() == kEurope)
			{
				// This is a Europe trade-route, exempt it from the reachability criteria
				// TODO: Check that there is actually a route to Europe!
				bNoRoute = false;
			}

			// R&R mod, vetiarvind, max yield import limit - end
			if (iAmount > 0)
			{

				int iExportValue = kOwner.AI_transferYieldValue(routes[i]->getSourceCity(), routes[i]->getYield(), -iAmount);
				int iImportValue = kOwner.AI_transferYieldValue(routes[i]->getDestinationCity(), routes[i]->getYield(), iAmount);
				int iRouteValue = (iExportValue + iImportValue + 2 * std::min(iExportValue, iImportValue)) / 4;


				if (pSourceCity == pPlotCity)
				{
					cityValues[routes[i]->getDestinationCity()] += 2 * iRouteValue;
				}
				else
				{
					cityValues[routes[i]->getSourceCity()] += iRouteValue;
				}

				routeValues[i] = iRouteValue;
			}
		}
	}

	// We bail if there's not a single viable destination
	// Transports with cargo should carry on because they might need to drop it off somewhere before they can start on normal traderoutes
	if (bNoRoute && bNoCargo)
		return false;

	// TAC - Trade Routes Advisor - koma13 - START
	// R&R mod, vetiarvind, max yield import limit - start
	// refactor contents of this block to set bIgnoreDanger into getIgnoreDangerStatus()
	// R&R mod, vetiarvind, max yield import limit - end
	// TAC - Trade Routes Advisor - koma13 - END

	IDInfo kBestDestination(NO_PLAYER, -1);
	int iBestDestinationValue = 0;
	//select best destination
	for (std::map<IDInfo, int>::iterator it = cityValues.begin(); it != cityValues.end(); ++it)
	{
		int iValue = it->second;

		if (iValue > 0)
		{
			CvCity* pCity = ::getCity(it->first);
			if (pCity != NULL)
			{
				FAssert(!atPlot(pCity->plot()));
				// TAC - Trade Routes Advisor - koma13 - START
				//if (generatePath(plot(), pCity->plot(), MOVE_NO_ENEMY_TERRITORY, true))

				CvPlot* pDestinationCityPlot = pCity->plot();

				int iTurns;
				if (generatePath(plot(), pDestinationCityPlot, (bIgnoreDanger ? MOVE_IGNORE_DANGER : MOVE_NO_ENEMY_TERRITORY), true, &iTurns))
				// TAC - Trade Routes Advisor - koma13 - END
				{
					iValue /= 1 + kOwner.AI_plotTargetMissionAIs(pDestinationCityPlot, MISSIONAI_TRANSPORT, this, 0);

					if (bCoastalTransport)
					{
						// Erik: Eventually these tuning factors should not be hard-coded!
						const int CoastalTransportRangeThreshold = 4;
						const int CoastalTransportDifferentAreaMultiplier = 2;

						// Erik: If this is a coastal transport, avoid routes that are much longer than land routes unless there are no other wagons in this area

						// Erik: If the destination city is in a different area, then this is a more attractive route (wagons cannot cross coast!)
						// Note: We don't have to check for the areas being coastally reachable, since that already checked

						const CvArea* pDestinationArea = pDestinationCityPlot->area();
						const CvArea* pPlotArea = plot()->area();

						if (pDestinationArea != pPlotArea)
						{
							// Erik: Double the value of the route. Even if we encourage transportion between areas we don't want to sail around the world!
							// so we make that less attractive
							iValue *= CoastalTransportDifferentAreaMultiplier;
							iValue /=  std::max(1, iTurns - (CoastalTransportRangeThreshold * CoastalTransportDifferentAreaMultiplier));
						}
						else
						{
							if (pPlotArea->getNumAIUnits(getOwnerINLINE(), UNITAI_WAGON) > 0)
							{
								// Erik: Longer routes are less attractive for coastal transports
								iValue /= std::max(1, iTurns - CoastalTransportRangeThreshold);
							}
						}
					}
				}
				else
				{
					iValue = 0;
				}
			}

			if (iValue > iBestDestinationValue)
			{
				iBestDestinationValue = iValue;
				kBestDestination = it->first;
			}
		}
	}

	if ((pPlotCity != NULL) && (kBestDestination.eOwner != NO_PLAYER))
	{
		//We need to keep looping and recalculating
		//For example a city might have "101" of an item, we want to move the first 100 but not the 1.
		//But it could also have 200, in which case we might want 2 loads of 100...
		//But it could also have 200 of two resources, and we'd want to move 100 of each...
		while (!isFull())
		{
			int iBestRoute = -1;
			int iBestRouteValue = 0;
			// R&R mod, vetiarvind, max yield import limit - start
			int iBestRouteYieldAmount = 0;
			bool bRouteComplete = true;
			bool bImportLimitUsed = false;
			// R&R mod, vetiarvind, max yield import limit - end
			//Now, for any trade routes which this group is assigned to, try to pick up cargo here.
			for (uint i = 0; i < routes.size(); ++i)
			{
				// Erik: This is redundant. Instead of looping through all routes, just find the subset of routes to the best destination city

				CvCity* pSourceCity = ::getCity(routes[i]->getSourceCity());
				if ((pSourceCity != NULL && pSourceCity == pPlotCity) // R&R mod, vetiarvind, max yield import limit (move plot==srcCity check outside as optmztn)
					&& (routes[i]->getDestinationCity() == kBestDestination))
				{
					CvCity* pDestinationCity = ::getCity(routes[i]->getDestinationCity());
					YieldTypes eYield = routes[i]->getYield();

					// transport feeder - start - Nightinggale
					//int iAmount = pSourceCity->getYieldStored(eYield) - pSourceCity->getMaintainLevel(eYield);
					int iAmount = pSourceCity->getYieldStored(eYield) - pSourceCity->getAutoMaintainThreshold(eYield);
					// transport feeder - end - Nightinggale


					// R&R mod, vetiarvind, max yield import limit - start
					int iOriginalAmount = iAmount;
					int bDestinationHasImportLimit = pDestinationCity != NULL && pDestinationCity->getMaxImportAmount(eYield) > 0;
					if(bDestinationHasImportLimit)
					{
						int turnsToReach = 0;
						iOriginalAmount = iAmount = std::min(GC.getGameINLINE().getCargoYieldCapacity(), iAmount);
						const bool r1 = generatePath(pSourceCity->plot(), pDestinationCity->plot(), (bIgnoreDanger ? MOVE_IGNORE_DANGER : MOVE_NO_ENEMY_TERRITORY), true, &turnsToReach);
						FAssertMsg(r1, "Path must be valid!");
						// Erik: If the destination can be reached in the same turn, subtract a turn
						turnsToReach = std::max(0, turnsToReach - 1);
						iAmount = estimateYieldsToLoad(pDestinationCity, iAmount, eYield, turnsToReach, aiYieldsLoaded[eYield]);
					}

					// R&R mod, vetiarvind, max yield import limit - end
					if (iAmount > 0)
					{
						int iExportValue = kOwner.AI_transferYieldValue(routes[i]->getSourceCity(), routes[i]->getYield(), -iAmount);
						int iImportValue = kOwner.AI_transferYieldValue(routes[i]->getDestinationCity(), routes[i]->getYield(), iAmount);
						int iRouteValue = (iExportValue + iImportValue + 2 * std::min(iExportValue, iImportValue)) / 4;

						if (iRouteValue > iBestRouteValue)
						{
							iBestRouteValue = iRouteValue;
							iBestRoute = i;
							// R&R mod, vetiarvind, max yield import limit - start
							iBestRouteYieldAmount = iAmount;
							bImportLimitUsed = bDestinationHasImportLimit;
							// R&R mod, vetiarvind, max yield import limit - end
						}
					}
				}
			} //end of for loop of routes

			if (iBestRouteValue > 0)
			{
				CLLNode<IDInfo>* pUnitNode = headUnitNode();
				while (pUnitNode != NULL)
				{
					CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
					pUnitNode = nextUnitNode(pUnitNode);

					if (pLoopUnit != NULL)
					{
						if (pLoopUnit->canLoadYield(plot(), routes[iBestRoute]->getYield(), false) ) 			// R&R mod, vetiarvind, max yield import limit fix
						{
							//pLoopUnit->loadYield(routes[iBestRoute]->getYield(), false);
							int loaded = 0;
							if(bImportLimitUsed)
								loaded = pLoopUnit->loadYieldAmount(routes[iBestRoute]->getYield(), iBestRouteYieldAmount, false);
							else
								loaded = pLoopUnit->loadYield(routes[iBestRoute]->getYield(), false);

							// R&R mod, vetiarvind, max yield import limit - end
							aiYieldsLoaded[routes[iBestRoute]->getYield()] += loaded;
							break;
						}
					}
				}
			}
			else
			{
				break;
			}
		}
		//XXX fill hold.
	}

	if ((kBestDestination.eOwner == NO_PLAYER) && hasCargo())
	{
		// Transport group is full and can't find any destination
		CvCity* pCity = kOwner.AI_findBestPort();
		if (pCity != NULL && !atPlot(pCity->plot()))
		{
			// check that the city is reachable
			int iTurns = 0;
			if (!generatePath(plot(), pCity->plot(), (bIgnoreDanger ? MOVE_IGNORE_DANGER : MOVE_NO_ENEMY_TERRITORY), true, &iTurns))
			{
				// no path. Bail out instead of trying to add a movement command
				// if the command to move is given, then movement will fail and this function is called again
				// in other words trying to move will cause an infinite loop (bug #416)
				return false;
			}
			kBestDestination = pCity->getIDInfo();
		}
	}

	//As a final step, we could consider loading yields which would be useful as parts of delivery runs...
	if (kBestDestination != kEurope)
	{
		CvCity* pBestDestinationCity = ::getCity(kBestDestination);
		if (pBestDestinationCity != NULL)
		{
			FAssert(!atPlot(pBestDestinationCity->plot()));
			// TAC - Trade Routes Advisor - koma13 - START
			//pushMission(MISSION_MOVE_TO, pBestDestinationCity->getX_INLINE(), pBestDestinationCity->getY_INLINE(), MOVE_NO_ENEMY_TERRITORY, false, false, MISSIONAI_TRANSPORT, pBestDestinationCity->plot());
			pushMission(MISSION_MOVE_TO, pBestDestinationCity->getX_INLINE(), pBestDestinationCity->getY_INLINE(), (bIgnoreDanger ? MOVE_IGNORE_DANGER : MOVE_NO_ENEMY_TERRITORY), false, false, MISSIONAI_TRANSPORT, pBestDestinationCity->plot());
			// TAC - Trade Routes Advisor - koma13 - END
			if (atPlot(pBestDestinationCity->plot()))
			{
				//Unload any goods if required (we can always pick them back up if this is an i+e city).
				std::vector<CvUnit*> units;

				CLLNode<IDInfo>* pUnitNode = plot()->headUnitNode();
				CvUnit* pLoopUnit;
				while (pUnitNode != NULL)
				{
					pLoopUnit = plot()->getUnitNodeLoop(pUnitNode);

					if (pLoopUnit != NULL)
					{
						const YieldTypes eYield = pLoopUnit->getYield();

						if ((eYield != NO_YIELD) && pLoopUnit->isCargo())
						{
							if (pLoopUnit->getTransportUnit()->getGroup() == this && pLoopUnit->canUnload())
							{
								units.push_back(pLoopUnit);
							}
						}
					}
				}
				for (uint i = 0; i < units.size(); ++i)
				{
					// R&R mod, vetiarvind, max yield import limit - start
					unloadToCity(pBestDestinationCity, units[i]);
					//units[i]->unload();
					// R&R mod, vetiarvind, max yield import limit - end
				}
			}
			return true;
		}
	}
	else
	{
		if (isHuman())
		{
			// Erik: Is this really ok for a coastal transport ?
			getHeadUnit()->AI_setUnitAIState(UNITAI_STATE_SAIL);
		}
	}

	return false;
}

CvUnit* CvSelectionGroupAI::AI_ejectBestDefender(CvPlot* pDefendPlot)
{
	CvUnitAI* pBestUnit = NULL;
	int iBestUnitValue = 0;
	FOR_EACH_UNITAI_VAR_IN(pUnit, *this)
	{
		//if (pUnit->noDefensiveBonus()) continue;
		// commented out by K-Mod. The noDefBonus thing is already taken into account.
		/*  advc.159: Call AI_currEffectiveStr instead of currEffectiveStr
			And reduce the precision multiplier from 100 to 10. */
		int iValue = pUnit->AI_currEffectiveStr(pDefendPlot) * 10;
		//if (GET_TEAM(getTeam()).isCityDefense(*pDefendPlot)) // advc
		if (pDefendPlot->isCity(true, getTeam()))
		{
			iValue *= 100 + pUnit->cityDefenseModifier();
			iValue /= 100;
		}

		iValue *= 100;
		//iValue /= (100 + pUnit->cityAttackModifier() + pUnit->getExtraCityAttackPercent());
		// advc.mnai: (Note that cityAttackModifier includes ExtraCityAttackPercent)
		iValue /= 100 + std::max(-50, 2 * pUnit->cityAttackModifier());

		iValue /= 2 + (pUnit->getLevel() *
			// advc.mnai:
			(pUnit->AI_getUnitAIType() == UNITAI_ATTACK_CITY ? 2 : 1));
		if (iValue > iBestUnitValue)
		{
			iBestUnitValue = iValue;
			pBestUnit = pUnit;
		}
	}
	if (pBestUnit != NULL && getNumUnits() > 1)
		pBestUnit->joinGroup(NULL);
	return pBestUnit;
}

// Protected Functions...

// R&R mod, vetiarvind, max yield import limit - start
bool CvSelectionGroupAI::getIgnoreDangerStatus() const
{
	bool bIgnoreDanger = false;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit != NULL && !pLoopUnit->isCargo())
		{
			if (pLoopUnit->isIgnoreDanger())
			{
				bIgnoreDanger = true;
				break;
			}
		}
	}
	return bIgnoreDanger;
}


// R&R mod, vetiarvind, max yield import limit - end
// Private Functions...
// R&R mod, vetiarvind, max yield import limit - start
int CvSelectionGroupAI::estimateYieldsToLoad(CvCity* pDestinationCity, int maxYieldsToLoad, YieldTypes eYield, int turnsToReach, int alreadyLoaded) const
{
	if(maxYieldsToLoad <= 0) return 0; // R&R mod, vetiarvind, max yield import limit fix
	int yieldsToLoad = maxYieldsToLoad;

	int importLimit = pDestinationCity->getMaxImportAmount(eYield);
	if(importLimit > 0)
	{
		int stored = pDestinationCity->getYieldStored(eYield);
		int yieldRate = pDestinationCity->getYieldRate(eYield);
		yieldsToLoad = std::min(yieldsToLoad,
			std::max(0, importLimit - (stored + alreadyLoaded + yieldRate * turnsToReach)));
	}
	return yieldsToLoad;
}

void CvSelectionGroupAI::unloadToCity(CvCity* pCity, CvUnit* unit, UnloadMode um)
{
		if (um == UnloadMode::NoForce && pCity->getMaxImportAmount(unit->getYield()) > 0)
		{
			int totalStored = unit->getYieldStored();
			int toUnload = estimateYieldsToLoad(pCity, totalStored, unit->getYield(), 0, 0);
			if(toUnload <= 0)
				return;
			if(toUnload < totalStored)
				unit->unloadStoredAmount(toUnload);
			else
				unit->unload();
		}
		else
			unit->unload();
}

// R&R mod, vetiarvind, max yield import limit - end

// <advc.003u> Based on CvSelectionGroup::getHeadUnit
CvUnitAI const* CvSelectionGroupAI::AI_getHeadUnit() const
{
	CLLNode<IDInfo> const* pNode = headUnitNode();
	return (pNode != NULL ? static_cast<CvUnitAI const*>(::getUnit(pNode->m_data)) : NULL);
}


CvUnitAI* CvSelectionGroupAI::AI_getHeadUnit()
{
	CLLNode<IDInfo>* pNode = headUnitNode();
	return (pNode != NULL ? static_cast<CvUnitAI*>(::getUnit(pNode->m_data)) : NULL);
} // </advc.003u>

inline int uceil(int iDividend, int iDivisor)
{
	FAssert(iDividend >= 0 && iDivisor > 0);
	return (iDividend + iDivisor - 1) / iDivisor;
}

/*	BETTER_BTS_AI_MOD, 08/19/09 and 03/30/10, jdog5000 (General AI): START
	(advc: Moved from CvSelectionGroup) */
// Approximate how many turns this group would take to reduce pCity's defense to zero
int CvSelectionGroupAI::AI_getBombardTurns(CvCity const* pCity) const
{
	PROFILE_FUNC();
	bool const bHasBomber = false; // (getOwner() != NO_PLAYER ?
	//		GET_PLAYER(getOwner()).AI_isDomainBombard(DOMAIN_AIR) : false);
	int iTotalBombardRate = (bHasBomber ? 16 : 0);
	bool bIgnoreBuildingDefense = bHasBomber;
	int iUnitBombardRate = 0;
	FOR_EACH_UNIT_IN(pUnit, *this)
	{
		if (pUnit->bombardRate() <= 0)
			continue;
		iUnitBombardRate = pUnit->bombardRate();
		/* WTP: Not supported-
		if (pUnit->ignoreBuildingDefense())
			bIgnoreBuildingDefense = true;
		else
		*/ 
		{
			iUnitBombardRate *= std::max(25, 100 - pCity->getBuildingBombardDefense());
			iUnitBombardRate /= 100;
		}
		iTotalBombardRate += iUnitBombardRate;
	}
	// advc (minor bugfix?): BBAI had not passed bIgnoreBuildingDefense consistently
	int const iTotalDefense = pCity->getTotalDefense(/*bIgnoreBuildingDefense*/);
	if (iTotalDefense <= 0)
		return 0;
	int const iHP = GC.getMAX_CITY_DEFENSE_DAMAGE() - pCity->getDefenseDamage();
	if (iHP <= 0)
		return 0;
	int iBombardTurns = uceil(iHP * iTotalDefense,
			std::max(1, GC.getMAX_CITY_DEFENSE_DAMAGE() * iTotalBombardRate));
	//if (gUnitLogLevel > 2) logBBAI("      Bombard of %S will take %d turns at rate %d and current damage %d with bombard def %d", pCity->getName().GetCString(), iBombardTurns, iTotalBombardRate, pCity->getDefenseDamage(), (bIgnoreBuildingDefense ? 0 : pCity->getBuildingBombardDefense()));
	return iBombardTurns;
}

// advc: Param renamed from bIgnoreMinors b/c it also causes Barbarians to be ignored
bool CvSelectionGroupAI::AI_isHasPathToAreaEnemyCity(bool bMajorOnly,
	MovementFlags eFlags, int iMaxPathTurns) const
{
	PROFILE_FUNC();
	//int iPass = 0; // advc: unused
	//for (PlayerIter<ALIVE> it; it.hasNext(); ++it)
	for (PlayerTypes ePlayer = FIRST_PLAYER; ePlayer < NUM_PLAYER_TYPES; ++ePlayer)
	{
		const CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
		if (!kPlayer.isAlive())
			continue;

		if (bMajorOnly && kPlayer.isNative() /*!it->isMajorCiv()*/)
			continue;
		if (GET_TEAM(getTeam()).AI_mayAttack(kPlayer.getTeam()) &&
			AI_isHasPathToAreaPlayerCity(kPlayer.getID(), eFlags, iMaxPathTurns))
		{
			return true;
		}
	}
	return false;
}

bool CvSelectionGroupAI::AI_isHasPathToAreaPlayerCity(PlayerTypes ePlayer,
	MovementFlags eFlags, int iMaxPathTurns) const
{
	PROFILE_FUNC();
	// <advc> Instead of relying on the area checks to fail when the group has no area
	if (getNumUnits() <= 0)
		return false; // </advc>
	FOREACH_CITY_OF_OWNER(pLoopCity, GET_PLAYER(ePlayer))
	{
		if (pLoopCity->isArea(*area()))
		{
			int iPathTurns;
			if (generatePath(&getPlot(), pLoopCity->plot(), eFlags, true,
				&iPathTurns, iMaxPathTurns))
			{
				if (iMaxPathTurns < 0 || iPathTurns <= iMaxPathTurns)
					return true;
			}
		}
	}
	return false;
}

namespace
{
	int overallUnitValue(CvUnit const& kUnit)
	{	// Crude ...
		return kUnit.getUnitInfo().getEuropeCost() *
			(100 + 6 * kUnit.getExperience());
	}

	template <typename T>
	const T& clamp(const T& value, const T& low, const T& high)
	{
		if (value < low) return low;
		if (value > high) return high;
		return value;
	}
}

/*	advc.004c: (Not const b/c it needs to return a non-const unit.
	Ideally, there would be a const version returning a const unit,
	but that would lead to a lot of duplicate code.) */
CvUnit* CvSelectionGroupAI::AI_bestUnitForMission(MissionTypes eMission,
	CvPlot const* pMissionPlot, std::vector<int> const* pUnitsToSkip)
{
	PROFILE_FUNC(); // advc (neither frequently called nor expensive)
	CvPlot const& kAt = getPlot();
	bool bEasyCityCapture = false;
	CvCity const* pTargetCity = (pMissionPlot == NULL ? NULL :
		pMissionPlot->getPlotCity());
	int iDefenders = -1;
	if (eMission == MISSION_BOMBARD)
	{
		FOR_EACH_UNIT_IN(pUnit, *this)
		{
			pTargetCity = pUnit->bombardTarget(&kAt);
			if (pTargetCity != NULL)
				break;
		}
		if (pTargetCity != NULL)
		{
			pMissionPlot = pTargetCity->plot();
			iDefenders = pMissionPlot->plotCount(
				PUF_canDefendEnemy, getOwner(), false);
			if (!isHuman())
			{	// Visibility cheat, but saves time.
				bEasyCityCapture = pTargetCity->AI().AI_isEvacuating();
			}
			else
			{
				FAssertMsg(iDefenders > 0, "AI bombards undefended city");
				int iAttackers = 0;
				FOR_EACH_UNIT_IN(pUnit, kAt)
				{
					if (!pUnit->canBombard(&kAt) &&
						pUnit->canMoveOrAttackInto(pMissionPlot))
					{
						iAttackers++;
						if (iAttackers >= 2 * iDefenders)
							break;
					}
				}
				if (iAttackers >= iDefenders)
				{
					int rStackCmp = AI_compareStacks(pMissionPlot, true, true);
					if (rStackCmp > 150 &&
						/*	NB: If iAtt==iDef, odds needs to be very favorable
							for an immediate conquest. */
						(iAttackers * 100 / std::max(iDefenders, 1)) * rStackCmp > 250)
					{	// (Assuming that the city gets bombarded to 0)
						bEasyCityCapture = true;
					}
				}
			}
		}
	}
	CvUnit* pBestUnit = NULL;
	int rMaxPriority = INT_MIN;
	FOR_EACH_UNITAI_VAR_IN(pUnit, *this)
	{
		if (!pUnit->canMove() ||
			(pUnitsToSkip != NULL &&
				std::find(pUnitsToSkip->begin(), pUnitsToSkip->end(),
					pUnit->getID()) != pUnitsToSkip->end()))
		{
			continue;
		}

		int rPriority = 0; // Initialize rPriority to a default value to avoid the warning

		switch (eMission)
		{
		case MISSION_PILLAGE:
		{	// K-Mod code cut from startMission
			/*	K-Mod. Let fast units carry out the pillage action first.
				(This is based on the idea from BBAI, which had a buggy implementation.) */
			if (!pUnit->canPillage(&kAt))
				continue;
			rPriority = 3;
			if (pUnit->bombardRate() > 0)
				rPriority--;
			if (pUnit->isMadeAttack())
				rPriority++;
			if (pUnit->isHurt() && !pUnit->hasMoved())
				rPriority--;
			// <advc.004c>
			rPriority *= 10000;
			rPriority -= overallUnitValue(*pUnit);
			// </advc.004c>
			//iPriority = (3 + iPriority) * pUnit->movesLeft() / 3;
			// advc.004c: Add 3 upfront. Don't see what good the division would do.
			rPriority *= pUnit->movesLeft();
			break;
		}
		case MISSION_BOMBARD:
		{
			if (!pUnit->canBombard(&kAt))
				continue;
			/*	Some baseline to avoid precision problem when getting
				too close to 0 through divisions and multiplications */
			rPriority = 1000;
			if (bEasyCityCapture)
				rPriority *= pUnit->currHitPoints();
			int const iBombard = pUnit->damageToBombardTarget(kAt);
			// bIgnoreBuilding=false b/c iBombard already reflects that
			int const iCurrDefense = pTargetCity->getDefenseModifier(/*false*/);
			int const iWaste = std::max(0, iBombard - iCurrDefense);
			if (isHuman())
			{	// Derive human intent from promotions
				int rDeltaBombard = (pUnit->getExtraBombardRate() - iWaste) -
					(/*pUnit->getExtraCollateralDamage() +*/
						pUnit->getExtraCityAttackPercent()) / 5;
				rPriority *= 1 + clamp(5 * rDeltaBombard, -90, 100) / 100;
			}
			rPriority *= std::max(1, 15 + iBombard - iWaste);
			int rOdds = pUnit->AI_attackOdds(pMissionPlot, false);
			rPriority *= (100 - rOdds);
			//rPriority /= 1 + per100(pUnit->collateralDamage());
			//rPriority /= 15 + std::min(iDefenders, pUnit->collateralDamageMaxUnits());
			/*	(CollateralDamageLimit gets ignored by all AI code so far,
				so I'm not going to bother with it here either.) */
			break;
		}
		default: FErrorMsg("Mission type not supported by bestUnitForMission");
		}
		if (rPriority > rMaxPriority)
		{
			rMaxPriority = rPriority;
			pBestUnit = pUnit;
		}
	}
	return pBestUnit;
}
