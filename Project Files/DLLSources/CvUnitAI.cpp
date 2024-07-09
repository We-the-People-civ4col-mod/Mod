// unitAI.cpp

#include "CvGameCoreDLL.h"
#include "CvUnitAI.h"
#include "CvMap.h"
#include "CvArea.h"
#include "CvPlot.h"
#include "CvGlobals.h"
#include "CvGameAI.h"
#include "CvTeamAI.h"
#include "CvPlayerAI.h"
#include "CvGameCoreUtils.h"
#include "CvRandom.h"
#include "CyUnit.h"
#include "CyArgsList.h"
#include "CvDLLPythonIFaceBase.h"
#include "CvInfos.h"
#include "FProfiler.h"
#include "FAStarNode.h"

// interface uses
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLFAStarIFaceBase.h"

#include "CvSavegame.h"
#include "BetterBTSAI.h"

#include <utility>

#define FOUND_RANGE				(7)

#define MOVE_PRIORITY_MAX 			2000
#define MOVE_PRIORITY_HIGH 			1500
#define MOVE_PRIORITY_MEDIUM		1000
#define MOVE_PRIORITY_LOW			500
#define MOVE_PRIORITY_MIN			1

namespace {
	const int BBAI_ATTACK_CITY_STACK_RATIO = 110;
	const int BBAI_SKIP_BOMBARD_BASE_STACK_RATIO = 300;
	const int BBAI_SKIP_BOMBARD_MIN_STACK_RATIO = 140;

	// Map these to WTP equivalents
	const UnitAITypes UNITAI_CITY_DEFENSE = UNITAI_DEFENSIVE;
	const UnitAITypes UNITAI_ATTACK = UNITAI_OFFENSIVE;
}

// Public Functions...

CvUnitAI::CvUnitAI() :
	m_eUnitAIType(NO_UNITAI)
{
	AI_reset();
}


CvUnitAI::~CvUnitAI()
{
	AI_uninit();
}


void CvUnitAI::AI_init(int iBirthmark)
{
	AI_reset();

	//--------------------------------
	// Init other game data
	AI_setBirthmark(GC.getGameINLINE().getSorenRandNum(10000, "AI Unit Birthmark"));

	// Preserve old AI behavioir that allows dangerous paths until
	// we ensure that it can work correctly with both settings
	if (!isHuman())
	{
		setAllowDangerousPath(true);
	}
	AI_setBirthmark(iBirthmark);
}


void CvUnitAI::AI_uninit()
{
}


void CvUnitAI::AI_reset()
{
	AI_uninit();

	AI_resetSavedData();
	m_iLastAIChangeTurn= GC.getGameINLINE().getGameTurn();
	m_bHasYielded = false; // No serialized
}

// AI_update returns true when we should abort the loop and wait until next slice
bool CvUnitAI::AI_update()
{
	PROFILE_FUNC();

	FAssertMsg(getUnitTravelState() != NO_UNIT_TRAVEL_STATE || canMove(), "canMove is expected to be true");
	FAssertMsg(isGroupHead(), "isGroupHead is expected to be true"); // XXX is this a good idea???

	//getGroup()->resetPath();
	CvSelectionGroup::path_finder.Reset();

	m_bHasYielded = false;

	// allow python to handle it
	if (GC.getUSE_AI_UNIT_UPDATE_CALLBACK()) // K-Mod. block unused python callbacks
	{
		// allow python to handle it
		CyUnit* pyUnit = new CyUnit(this);
		CyArgsList argsList;
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pyUnit));	// pass in unit class
		long lResult=0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "AI_unitUpdate", argsList.makeFunctionArgs(), &lResult);
		delete pyUnit;	// python fxn must not hold on to this pointer
		if (lResult == 1)
		{
			return false;
		}
	}

	// Now done by the group update
	/*
	if (getUnitTravelState() != NO_UNIT_TRAVEL_STATE)
	{
		AI_europeUpdate();
		return false;
	}
	*/

	int iOldMovePriority = AI_getMovePriority();

	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	if (!AI_afterAttack())
	{
		if (getGroup()->isAutomated() && (getGroup()->getAutomateType() != AUTOMATE_FULL))
		{
			switch (getGroup()->getAutomateType())
			{
			case AUTOMATE_BUILD:
				AI_workerMove();
				break;

			case AUTOMATE_CITY:
				AI_cityAutomated();
				if (!isInGroup())
				{
					return true;
				}
				break;

			case AUTOMATE_EXPLORE:
				switch (getDomainType())
				{
				case DOMAIN_SEA:
					AI_exploreSeaMove();
					break;

				case DOMAIN_LAND:
					if (canSpeakWithChief(NULL))
					{
						AI_scoutMove();
					}
					else
					{
						AI_exploreMove();
					}
					break;

				default:
					FAssert(false);
					break;
				}
				break;

			case AUTOMATE_SAIL_TO_EUROPE:
				AI_automateSailTo(kSailToEurope);
				break;
			/*** TRIANGLETRADE 10/28/08 by DPII ***/
			case AUTOMATE_SAIL_TO_AFRICA:
				AI_automateSailTo(kSailToAfrica);
				break;
			/**************************************/
			// R&R, ray, Port Royal
			case AUTOMATE_SAIL_TO_PORT_ROYAL:
				AI_automateSailTo(kSailToPortRoyal);
				break;
			// R&R, ray, Port Royal - END
			case AUTOMATE_TRANSPORT_ROUTES:
				AI_transportMoveRoutes();
				break;

			case AUTOMATE_TRANSPORT_FULL:
				AI_transportMoveFull();
				break;
			//TAC Whaling, ray
			case AUTOMATE_WORKER_SEA:
				AI_workerSeaMove();
				break;
			//End TAC Whaling, ray
			case AUTOMATE_FULL:
				FAssert(false);
				break;
			default:
				FAssert(false);
				break;
			}

			if (getGroup() == NULL)
			{
				return true;
			}

			// if no longer automated, then we want to bail
			return (!getGroup()->isAutomated());
		}
		else if (canMove() || isCargo())
		{
			FAssert(getGroup() != NULL);
			switch (AI_getUnitAIType())
			{
			case UNITAI_UNKNOWN:
				getGroup()->pushMission(MISSION_SKIP);
				break;
			// < JAnimals Mod Start >
			case UNITAI_ANIMAL:
			case UNITAI_ANIMAL_SEA:
				AI_animalMove();
				break;
			// < JAnimals Mod End >

			case UNITAI_FLEEING: // R&R, ray, Fleeing Units
				AI_FleeingMove();
				break;

			case UNITAI_COLONIST:
				AI_colonistMove();
				break;

			case UNITAI_SETTLER:
				AI_settlerMove();
				break;

			case UNITAI_WORKER:
				AI_workerMove();
				break;

			case UNITAI_MISSIONARY:
				AI_missionaryMove();
				break;
			// WTP, ray, Native Trade Posts - START
			case UNITAI_TRADER:
				AI_nativeTraderMove();
				break;

			case UNITAI_SCOUT:
				AI_scoutMove();
				break;

			case UNITAI_WAGON:
				AI_transportMoveFull();
				break;

			case UNITAI_TREASURE:
				AI_treasureMove();
				break;

			case UNITAI_YIELD:
				AI_yieldUhMove();
				break;

			case UNITAI_GENERAL:
				AI_generalMove();
				break;

			case UNITAI_DEFENSIVE:
				if (kOwner.isNative())
				{
					AI_defensiveBraveMove();
				}
				else if (kOwner.AI_isKing())
				{
					AI_imperialSoldierMove();
				}
				else
				{
					AI_cityDefenseMove();
				}
				break;

			case UNITAI_OFFENSIVE:
				if (kOwner.isNative())
				{
					AI_offensiveBraveMove();
				}
				else if (kOwner.AI_isKing())
				{
					AI_imperialCannonMove();
				}
				//WTP, Protected Hostile Goodies - START
				else if (GC.getGameINLINE().getBarbarianPlayer() == getOwnerINLINE())
				{
					if(plot()->isGoodyForSpawningHostileCriminals() || plot()->isGoodyForSpawningHostileNatives())
					{
						if (plot()->getNumDefenders(getOwnerINLINE()) <= 2)
						{
							getGroup()->pushMission(MISSION_SKIP);
							return true;
						}
					}
                    if (canMove()) // don't try to attack if the unit already used all movement points - Nightinggale
						AI_attackMove();
				}
				//WTP, Protected Hostile Goodies - END
				else
				{
					AI_attackMove();
				}
				break;

			case UNITAI_ATTACK_CITY:
				AI_attackCityMove();
				break;

			case UNITAI_COUNTER:
				if (kOwner.isNative())
				{
					AI_counterBraveMove();
				}
				else if (kOwner.AI_isKing())
				{
					AI_imperialMountedMove();
				}
				else
				{
					AI_counterMove();
				}
				break;
			//TAC Whaling, ray
			case UNITAI_WORKER_SEA:
				AI_workerSeaMove();
				break;
			//End TAC Whaling, ray
			case UNITAI_TRANSPORT_SEA:
				AI_transportSeaMove();
				break;

			case UNITAI_ASSAULT_SEA:
				AI_assaultSeaMove();
				break;

			case UNITAI_COMBAT_SEA:
				if (kOwner.AI_isKing())
				{
					AI_imperialShipMove();
				}
				else
				{
					AI_combatSeaMove();
				}
				break;

			case UNITAI_PIRATE_SEA:
				AI_pirateMove();
				break;

			// TAC - AI Escort Sea - koma13 - START
			case UNITAI_ESCORT_SEA:
				AI_escortSeaMove();
				break;
			// TAC - AI  Sea - koma13 - END

			case UNITAI_TRANSPORT_COAST:
				AI_transportCoastMove();
				break;

			default:
				FAssert(false);
				break;
			}
		}
	}

	if (isDead() || isDelayedDeath() || getGroup() == NULL)
	{
		return true;
	}

	/*
	if (!isHuman())
	{
		if (AI_getMovePriority() == iOldMovePriority)
		{
			if (canMove() && (getGroup()->getActivityType() == ACTIVITY_MISSION || getGroup()->getActivityType() == ACTIVITY_AWAKE))
			{
				AI_setMovePriority(AI_getMovePriority() - (MOVE_PRIORITY_MAX / 10));
			}
			else
			{
				AI_setMovePriority(0);
			}
		}
	}
	*/

	if (m_bHasYielded)
		return true;
	else
		return false;
}

// AI_update returns true when we should abort the loop and wait until next slice
bool CvUnitAI::AI_europeUpdate()
{
	PROFILE_FUNC();

	if (getDomainType() == DOMAIN_LAND)
	{
		return false;//XXX maybe units should load onto ships...
	}

	if (getUnitTravelTimer() > 0)
	{
		return false;
	}
	else
	{
		if (getGroup()->isAutomated() && (getGroup()->getAutomateType() == AUTOMATE_FULL))
		{
			if (getUnitTravelState() == UNIT_TRAVEL_STATE_IN_EUROPE)
			{
				AI_europe();
				return false;
			}
		}
		if (getGroup()->isAutomated() && (getGroup()->getAutomateType() == AUTOMATE_FULL))
		{
			if (getUnitTravelState() == UNIT_TRAVEL_STATE_IN_AFRICA)
			{
				AI_africa();
				return false;
			}
		}
	}

	//if (!(getGroup()->isAutomated() && (getGroup()->getAutomateType() != AUTOMATE_FULL)))
	if (getGroup()->AI_isControlled())
	{
		if (isHurt() && (healRate(plot()) > 0))
		{
			// BUG?: even when hurt we should sell yields?
			if (hasCargo() && getUnitTravelState() == UNIT_TRAVEL_STATE_IN_EUROPE)
			{
				// If we're carrying any units we may as well unload them
				// so that other ships can transport them while we heal
				AI_unloadUnits(EUROPE);
			}

			return false;
		}
		switch (AI_getUnitAIType())
		{
		case UNITAI_UNKNOWN:
		case UNITAI_ANIMAL: // < JAnimals Mod Start >
		case UNITAI_ANIMAL_SEA:
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
		case UNITAI_DEFENSIVE:
		case UNITAI_OFFENSIVE:
		case UNITAI_COUNTER:
		    break;
		//TAC Whaling, ray
		case UNITAI_WORKER_SEA:
			if (getUnitTravelState() == UNIT_TRAVEL_STATE_IN_EUROPE)
			{
				crossOcean(UNIT_TRAVEL_STATE_FROM_EUROPE);
			}
			break;
		//End TAC Whaling, ray
		case UNITAI_TRANSPORT_SEA:
			if (getUnitTravelState() == UNIT_TRAVEL_STATE_IN_EUROPE)
			{
				AI_europe();
			}
			if (getUnitTravelState() == UNIT_TRAVEL_STATE_IN_AFRICA)
			{
				AI_africa();
			}
			break;

		case UNITAI_ASSAULT_SEA:
		case UNITAI_COMBAT_SEA:
			if (GET_PLAYER(getOwnerINLINE()).AI_isKing())
			{
				if (hasCargo() && getUnitTravelState() == UNIT_TRAVEL_STATE_IN_EUROPE)
				{
					crossOcean(UNIT_TRAVEL_STATE_FROM_EUROPE);
				}
			}
			else
			{
				// Erik: We have to deal correctly with combat ships
				// like the sloop which may travel to a port with cargo
				if (getUnitTravelState() == UNIT_TRAVEL_STATE_IN_EUROPE)
				{
					// TAC - AI Assault Sea Fix - koma13 - START
					if (hasCargo())
					{
						AI_europe();
					}
					// TAC - AI Assault Sea Fix - koma13 - END
					crossOcean(UNIT_TRAVEL_STATE_FROM_EUROPE);
				}

				if (getUnitTravelState() == UNIT_TRAVEL_STATE_IN_AFRICA)
				{
					// TAC - AI Assault Sea Fix - koma13 - START
					if (hasCargo())
					{
						AI_africa();
					}
					// TAC - AI Assault Sea Fix - koma13 - END
					crossOcean(UNIT_TRAVEL_STATE_FROM_AFRICA);
				}
			}
			break;
		case UNITAI_PIRATE_SEA:
			if (getUnitTravelState() == UNIT_TRAVEL_STATE_IN_EUROPE)
			{
				if (hasCargo())
				{
					AI_sellYieldUnits(EUROPE);
				}
				crossOcean(UNIT_TRAVEL_STATE_FROM_EUROPE);
			}
			else if (getUnitTravelState() == UNIT_TRAVEL_STATE_IN_AFRICA)
			{
				if (hasCargo())
				{
					AI_sellYieldUnits(AFRICA);
				}
				crossOcean(UNIT_TRAVEL_STATE_FROM_AFRICA);
			}
			break;
		case UNITAI_ESCORT_SEA:		// TAC - AI Escort Sea - koma13
			crossOcean(UNIT_TRAVEL_STATE_FROM_EUROPE);
		    break;

		case UNITAI_TRANSPORT_COAST:
			FAssertMsg(false, "UNITAI_TRANSPORT_COAST not supported for ships in Europe");
			break;

		default:
			FAssert(false);
			break;
		}
	}

	AI_setMovePriority(0);
	getGroup()->pushMission(MISSION_SKIP);
	return false;
}

bool CvUnitAI::AI_follow()
{
	FErrorMsg("Unexpected EXE call: CvUnitAI::AI_follow");
	return false;
}

// Returns true if took an action or should wait to move later...
/*	K-Mod. I've basically rewritten this function.
	bFirst should be "true" if this is the first unit in the group to use this follow function.
	the point is that there are some calculations and checks in here
	which only depend on the group, not the unit,
	so for efficiency we should only check them once. */
bool CvUnitAI::AI_follow(bool bFirst)
{
	//getGroup()->resetPath();
	//CvSelectionGroup::path_finder.Reset();

	if (AI_followBombard())
	{
		return true;
	}

	if (bFirst && getGroup()->getHeadUnitAI() == UNITAI_ATTACK_CITY)
	{
		/*	note: AI_stackAttackCity will check which of our units can attack
			when comparing stacks; and it will issue the attack order using
			MOVE_DIRECT ATTACK, which will execute without waiting for
			the entire group to have movement points. */
		if (AI_stackAttackCity()) // automatic threshold
			return true;
	}

	/*	I've changed attack-follow code so that it will
	only attack with a single unit, not the whole group. */
	if (bFirst && AI_cityAttack(1, 65, NO_MOVEMENT_FLAGS, true))
		return true;
	if (bFirst)
	{
		bool bMoveGroup = false; // to large groups to leave some units behind.
		if (getGroup()->getNumUnits() >= 16)
		{
			int iCanMove = 0;
			FOR_EACH_UNIT_IN(pLoopUnit, *getGroup())
			{
				if (pLoopUnit->canMove())
					iCanMove++;
			}
			// if 4/5 of our group can still move.
			bMoveGroup = (5 * iCanMove >= 4 * getGroup()->getNumUnits() || iCanMove >= 20);
		}
		if (AI_anyAttack(1, isEnemy(getPlot()) ? 65 : 70, NO_MOVEMENT_FLAGS,
			bMoveGroup ? 0 : 2, true, true))
		{
			return true;
		}
	}

	if (isEnemy(*plot()))
	{
		if (canPillage(plot()))
		{
			getGroup()->pushMission(MISSION_PILLAGE);
			return true;
		}
	}

	if (canFound(NULL))
	{
		if (area()->getBestFoundValue(getOwnerINLINE()) > 0)
		{
			if (AI_foundRange(FOUND_RANGE, true))
			{
				return true;
			}
		}
	}

	return false;
}


// XXX what if a unit gets stuck b/c of it's UnitAIType???
// XXX is this function costing us a lot? (it's recursive...)
void CvUnitAI::AI_upgrade()
{
	PROFILE_FUNC();

	FAssertMsg(!isHuman(), "isHuman did not return false as expected");
	FAssertMsg(AI_getUnitAIType() != NO_UNITAI, "AI_getUnitAIType() is not expected to be equal with NO_UNITAI");

	CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());
	UnitAITypes eUnitAI = AI_getUnitAIType();
	CvArea* pArea = area();

	int iCurrentValue = kPlayer.AI_unitValue(getUnitType(), eUnitAI, pArea);

	for (int iPass = 0; iPass < 2; iPass++)
	{
		int iBestValue = 0;
		UnitTypes eBestUnit = NO_UNIT;

		for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
		{
			if ((iPass > 0) || GC.getUnitInfo((UnitTypes)iI).getUnitAIType(AI_getUnitAIType()))
			{
				int iNewValue = kPlayer.AI_unitValue(((UnitTypes)iI), eUnitAI, pArea);
				if ((iPass == 0 || iNewValue > 0) && iNewValue > iCurrentValue)
				{
					if (canUpgrade((UnitTypes)iI))
					{
						int iValue = (1 + GC.getGameINLINE().getSorenRandNum(10000, "AI Upgrade"));

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							eBestUnit = ((UnitTypes)iI);
						}
					}
				}
			}
		}

		if (eBestUnit != NO_UNIT)
		{
			upgrade(eBestUnit);
			doDelayedDeath();
			return;
		}
	}
}


void CvUnitAI::AI_promote()
{
	PROFILE_FUNC();

	// K-Mod. A quick check to see if we can rule out all promotions in one hit, before we go through them one by one.
	if (!isPromotionReady())
		return; // can't get any normal promotions. (see CvUnit::canPromote)
	// K-Mod end

	int iBestValue = 0;
	PromotionTypes eBestPromotion = NO_PROMOTION;

	for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		if (canPromote((PromotionTypes)iI, -1))
		{
			int iValue = AI_promotionValue((PromotionTypes)iI);

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestPromotion = ((PromotionTypes)iI);
			}
		}
	}

	if (eBestPromotion != NO_PROMOTION)
	{
		promote(eBestPromotion, -1);
		AI_promote();
	}
}


int CvUnitAI::AI_groupFirstValInternal() const
{
	switch (AI_getUnitAIType())
	{
	case UNITAI_UNKNOWN:
		FAssert(false);
		break;
	// < JAnimals Mod Start >
	case UNITAI_ANIMAL:
	case UNITAI_ANIMAL_SEA:
		return 23;
		break;
	// < JAnimals Mod End >
	case UNITAI_FLEEING: // R&R, ray, Fleeing Units
		return 27;
		break;

	case UNITAI_COLONIST:
		return 21;
		break;

	case UNITAI_SETTLER:
		return 50;
		break;

	case UNITAI_WORKER:
		return 6;
		break;

	case UNITAI_MISSIONARY:
		return 1;
		break;

	case UNITAI_TRADER: // WTP, ray, Native Trade Posts - START
		return 2;
		break;

	case UNITAI_SCOUT:
		return 24;
		break;

	case UNITAI_WAGON:
		return 25;
		break;

	case UNITAI_TREASURE:
		return 26;
		break;
	case UNITAI_YIELD:
		return 1;
		break;

	case UNITAI_GENERAL:
		return 30;
		break;

	case UNITAI_DEFENSIVE:
		return 14;
		break;

	case UNITAI_ATTACK_CITY:
		return 16;
		break;
	
	case UNITAI_OFFENSIVE:
		return 15;
		break;

	case UNITAI_COUNTER:
		return 18;
		break;
	//TAC Whaling, ray
	case UNITAI_WORKER_SEA:
		return 1;
		break;
	//End TAC Whaling, ray
	case UNITAI_TRANSPORT_SEA:
		return 20;
		break;

	case UNITAI_TRANSPORT_COAST:
		return 19;
		break;

	case UNITAI_ASSAULT_SEA:
		return 14;
		break;

	case UNITAI_COMBAT_SEA:
		return 18;
		break;

	case UNITAI_PIRATE_SEA:
		return 16;
		break;

	// TAC - AI Escort Sea - koma13 - START
	case UNITAI_ESCORT_SEA:
		return 1;
		break;
	// TAC - AI Escort Sea - koma13 - START

	default:
		FAssert(false);
		break;
	}

	return 0;
}


int CvUnitAI::AI_groupSecondVal()
{
	return (baseCombatStr());
}


// Returns attack odds out of 100 (the higher, the better...)
// Withdrawal odds included in returned value
int CvUnitAI::AI_attackOdds(const CvPlot* pPlot, bool bPotentialEnemy) const
{
	CvUnit* pDefender;
	int iOurStrength;
	int iTheirStrength;
	int iOurFirepower;
	int iTheirFirepower;
	int iBaseOdds;
	int iStrengthFactor;
	int iDamageToUs;
	int iDamageToThem;
	int iNeededRoundsUs;
	int iNeededRoundsThem;

	pDefender = pPlot->getBestDefender(NO_PLAYER, getOwnerINLINE(), this, !bPotentialEnemy, bPotentialEnemy);

	CvCity* pCity = pPlot->getPlotCity();
	if (pCity != NULL)
	{
		pDefender = pCity->getBestDefender(NULL, pDefender, this);
	}

	if (pDefender == NULL)
	{
		return 100;
	}

	iOurStrength = currCombatStr(NULL, NULL);
	iOurFirepower = currFirepower(NULL, NULL);

	if (iOurStrength == 0)
	{
		return 1;
	}

	iTheirStrength = pDefender->currCombatStr(pPlot, this);
	iTheirFirepower = pDefender->currFirepower(pPlot, this);


	FAssert((iOurStrength + iTheirStrength) > 0);
	FAssert((iOurFirepower + iTheirFirepower) > 0);

	iBaseOdds = (100 * iOurStrength) / (iOurStrength + iTheirStrength);
	if (iBaseOdds == 0)
	{
		return 1;
	}

	iStrengthFactor = ((iOurFirepower + iTheirFirepower + 1) / 2);

	iDamageToUs = std::max(1,((GLOBAL_DEFINE_COMBAT_DAMAGE * (iTheirFirepower + iStrengthFactor)) / (iOurFirepower + iStrengthFactor)));
	iDamageToThem = std::max(1,((GLOBAL_DEFINE_COMBAT_DAMAGE * (iOurFirepower + iStrengthFactor)) / (iTheirFirepower + iStrengthFactor)));

	iNeededRoundsUs = (std::max(0, pDefender->currHitPoints()) + iDamageToThem - 1 ) / iDamageToThem;
	iNeededRoundsThem = (std::max(0, currHitPoints()) + iDamageToUs - 1 ) / iDamageToUs;

	iNeededRoundsUs = std::max(1, iNeededRoundsUs);
	iNeededRoundsThem = std::max(1, iNeededRoundsThem);

	int iRoundsDiff = iNeededRoundsUs - iNeededRoundsThem;
	if (iRoundsDiff > 0)
	{
		iTheirStrength *= (1 + iRoundsDiff);
	}
	else
	{
		iOurStrength *= (1 - iRoundsDiff);
	}

	int iOdds = (((iOurStrength * 100) / (iOurStrength + iTheirStrength)));
	iOdds += ((100 - iOdds) * (std::min(100, iOdds * 2) * withdrawalProbability() / 100)) / 100;
	iOdds += GET_PLAYER(getOwnerINLINE()).AI_getAttackOddsChange();

	return std::max(1, std::min(iOdds, 99));
}


// Returns true if the unit found a build for this city...
bool CvUnitAI::AI_bestCityBuild(CvCity* pCity, CvPlot** ppBestPlot, BuildTypes* peBestBuild, CvPlot* pIgnorePlot, CvUnit* pUnit)
{
	PROFILE_FUNC();

	BuildTypes eBuild;
	int iValue;
	int iBestValue;
	int iI;

	iBestValue = 0;
	BuildTypes eBestBuild = NO_BUILD;
	CvPlot* pBestPlot = NULL;

	const CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());

	// K-Mod. hack: For the AI, I want to use the standard pathfinder, CvUnit::generatePath.
	// but this function is also used to give action recommendations for the player
	// - and for that I do not want to disrupt the standard pathfinder. (because I'm paranoid about OOS bugs.)
	KmodPathFinder alt_finder;
	KmodPathFinder& pathFinder = getGroup()->AI_isControlled() ? CvSelectionGroup::path_finder : alt_finder;
	if (getGroup()->AI_isControlled())
	{
		// standard settings. cf. CvUnit::generatePath
		pathFinder.SetSettings(getGroup(), 0);
	}
	else
	{
		// like I said - this is only for action recommendations. It can be rough.
		pathFinder.SetSettings(getGroup(), 0, 5, GLOBAL_DEFINE_MOVE_DENOMINATOR);
	}

	for (int iPass = 0; iPass < 2; iPass++)
	{
		for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
			CvPlot* pLoopPlot = plotCity(pCity->getX_INLINE(), pCity->getY_INLINE(), iI);

			if (pLoopPlot != NULL)
			{
				if (AI_plotValid(pLoopPlot))
				{
					if (pLoopPlot != pIgnorePlot)
					{
						if (pLoopPlot->getImprovementType() == NO_IMPROVEMENT || !kPlayer.isOption(PLAYEROPTION_SAFE_AUTOMATION) && !GC.getImprovementInfo(pLoopPlot->getImprovementType()).isAINoRemove())
						{
							iValue = pCity->AI_getBestBuildValue(iI);

							if (iValue > iBestValue)
							{
								eBuild = pCity->AI_getBestBuild(iI);
								FAssertMsg(eBuild < GC.getNumBuildInfos(), "Invalid Build");

								if (eBuild != NO_BUILD)
								{
									if (0 == iPass)
									{
										iBestValue = iValue;
										pBestPlot = pLoopPlot;
										eBestBuild = eBuild;
									}
									else if (canBuild(pLoopPlot, eBuild))
									{
										if (!(pLoopPlot->isVisibleEnemyUnit(this)))
										{
											/* original colo code	
											int iPathTurns = 0;
											if (generatePath(pLoopPlot, 0, true, &iPathTurns))
											{
												// XXX take advantage of range (warning... this could lead to some units doing nothing...)
												int iMaxWorkers = 1;
												if (getPathFinder().GetFinalMoves() == 0)
												{
													iPathTurns++;
												}
												else if (iPathTurns <= 1)
												{
													iMaxWorkers = AI_calculatePlotWorkersNeeded(pLoopPlot, eBuild);
												}
												if (pUnit != NULL)
												{
													if (pUnit->plot()->isCity() && iPathTurns == 1 && getPathFinder().GetFinalMoves() > 0)
													{
														iMaxWorkers += 10;
													}
												} */
											// K-Mod. basically the same thing, but using pathFinder.
											if (pathFinder.GeneratePath(pLoopPlot))
											{
												const int iPathTurns = pathFinder.GetPathTurns() + (pathFinder.GetFinalMoves() == 0 ? 1 : 0);
												int iMaxWorkers = iPathTurns > 1 ? 1 : AI_calculatePlotWorkersNeeded(pLoopPlot, eBuild);
												if (pUnit && pUnit->plot()->isCity() && iPathTurns == 1)
													iMaxWorkers += 10;
											// K-Mod end

												if (GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BUILD, getGroup()) < iMaxWorkers)
												{
													//XXX this could be improved greatly by
													//looking at the real build time and other factors
													//when deciding whether to stack.
													iValue /= iPathTurns;

													iBestValue = iValue;
													pBestPlot = pLoopPlot;
													eBestBuild = eBuild;
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

		if (0 == iPass)
		{
			if (eBestBuild != NO_BUILD)
			{
				FAssert(pBestPlot != NULL);
				/* original bts code
				int iPathTurns;
				if ((generatePath(pBestPlot, 0, true, &iPathTurns)) && canBuild(pBestPlot, eBestBuild)
					&& !(pBestPlot->isVisibleEnemyUnit(this)))
				{
					int iMaxWorkers = 1;
					if (pUnit != NULL)
					{
						if (pUnit->plot()->isCity())
						{
							iMaxWorkers += 10;
						}
					}
					if (getPathFinder().GetFinalMoves() == 0)
					{
						iPathTurns++;
					}
					else if (iPathTurns <= 1)
					{
						iMaxWorkers = AI_calculatePlotWorkersNeeded(pBestPlot, eBestBuild);
					} */
				// K-Mod. basically the same thing, but using pathFinder.
				if (pathFinder.GeneratePath(pBestPlot))
				{
					const int iPathTurns = pathFinder.GetPathTurns() + (pathFinder.GetFinalMoves() == 0 ? 1 : 0);
					int iMaxWorkers = iPathTurns > 1 ? 1 : AI_calculatePlotWorkersNeeded(pBestPlot, eBestBuild);
					if (pUnit && pUnit->plot()->isCity() && iPathTurns == 1)
						iMaxWorkers += 10;
				// K-Mod end
					const int iWorkerCount = GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pBestPlot, MISSIONAI_BUILD, getGroup());
					if (iWorkerCount < iMaxWorkers)
					{
						//Good to go.
						break;
					}
				}
				eBestBuild = NO_BUILD;
				iBestValue = 0;
			}
		}
	}

	if (NO_BUILD != eBestBuild)
	{
		FAssert(NULL != pBestPlot);
		if (ppBestPlot != NULL)
		{
			*ppBestPlot = pBestPlot;
		}
		if (peBestBuild != NULL)
		{
			*peBestBuild = eBestBuild;
		}
	}


	return (NO_BUILD != eBestBuild);
}


bool CvUnitAI::AI_isCityAIType() const
{
	return (AI_getUnitAIType() == UNITAI_DEFENSIVE || 
		//advc.rom (Afforess): count units on guard mission as city defenders
		AI_getGroup()->AI_getMissionAITypeInternal() == MISSIONAI_GUARD_CITY);
}


int CvUnitAI::AI_getBirthmark() const
{
	return m_iBirthmark;
}


void CvUnitAI::AI_setBirthmark(int iNewValue)
{
	m_iBirthmark = iNewValue;
}


UnitAITypes CvUnitAI::AI_getUnitAIType() const
{
	return m_eUnitAIType;
}


// XXX make sure this gets called...
void CvUnitAI::AI_setUnitAIType(UnitAITypes eNewValue)
{
	if (AI_getUnitAIType() != eNewValue)
	{
		FAssertMsg(eNewValue != UNITAI_SETTLER || (eNewValue == UNITAI_SETTLER && canFound(NULL)),
			"Unit must be able to found to have this UNITAI!");	
		FAssertMsg(eNewValue != UNITAI_WORKER || (eNewValue == UNITAI_WORKER && workRate(true) > 0),
			"Unit must have non-zero work rate to have this UNITAI!");

		const bool bOnMap = (getX_INLINE() != INVALID_PLOT_COORD) && (getY_INLINE() != INVALID_PLOT_COORD);

		if (AI_getUnitAIType() != NO_UNITAI)
		{
			if (bOnMap)
			{
				area()->changeNumAIUnits(getOwnerINLINE(), AI_getUnitAIType(), -1);
			}
			GET_PLAYER(getOwnerINLINE()).AI_changeNumAIUnits(AI_getUnitAIType(), -1);
		}

		m_eUnitAIType = eNewValue;

		if (bOnMap)
		{
			area()->changeNumAIUnits(getOwnerINLINE(), AI_getUnitAIType(), 1);

			// If eNewValue is NO_UNITAI then the unit is joining a city and hence
			// there is no need to join a new group
			if (eNewValue != NO_UNITAI)
				joinGroup(NULL);

			if (getTransportUnit() != NULL)
			{
				getGroup()->setActivityType(ACTIVITY_SLEEP);
			}
		}
		GET_PLAYER(getOwnerINLINE()).AI_changeNumAIUnits(AI_getUnitAIType(), 1);

		const int iCurrentTurn = GC.getGameINLINE().getGameTurn();
		if (getGameTurnCreated() != iCurrentTurn)
		{
			m_iLastAIChangeTurn = iCurrentTurn;
		}
	}
}

UnitAIStates CvUnitAI::AI_getUnitAIState() const
{
	return m_eUnitAIState;
}

void CvUnitAI::AI_setUnitAIState(UnitAIStates eNewValue)
{

	if (AI_getUnitAIState() != eNewValue)
	{
		m_eUnitAIState = eNewValue;
	}

	int iCurrentTurn = GC.getGameINLINE().getGameTurn();
	if (getGameTurnCreated() != iCurrentTurn)
	{
		m_iLastAIChangeTurn = iCurrentTurn;
	}
}

int CvUnitAI::AI_sacrificeValue(const CvPlot* pPlot) const
{
	int iDenominator = 100 + cityDefenseModifier();
	int iValue  = 128 * (currEffectiveStr(pPlot, ((pPlot == NULL) ? NULL : this))) + iDenominator - 1;  // round up
	iValue /= iDenominator;

	iValue *= 100 + withdrawalProbability();
	int iCostValue = 0;
	for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
	{
		iCostValue += m_pUnitInfo->getYieldCost(eYield);
	}
	iDenominator = std::max(1, (1 + iCostValue));
	iCostValue += iDenominator - 1;  // round up
	iValue /= iDenominator;

	iDenominator = 10 + getExperience();
	iValue += iDenominator - 1;  // round up
	iValue /= iDenominator;

    return iValue;
}

// Protected Functions...

// < JAnimals Mod Start >
void CvUnitAI::AI_animalMove()
{
	PROFILE_FUNC();

	if (isHurt())
	{
		if (AI_heal())
		{
			return;
		}
	}

	//WTP, Protected Hostile Goodies - START
	// protect Goody if not yet protected
	if (plot()->isGoodyForSpawningHostileAnimals())
	{
		if(plot()->getNumDefenders(getOwnerINLINE()) <= 2)
		{
			getGroup()->pushMission(MISSION_FORTIFY);
			return;
		}
	}
	//WTP, Protected Hostile Goodies - END

	if (AI_animalAttack())
	{
		return;
	}

	if (getDomainType() == GC.getInfoTypeForString("DOMAIN_SEA"))
	{
		if (AI_animalSeaPatrol())
		{
			return;
		}
	}
	else if (getDomainType() == GC.getInfoTypeForString("DOMAIN_LAND"))
	{
		if (AI_animalLandPatrol())
		{
			return;
		}
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_animalAttack()
{
	PROFILE_FUNC();

	int iRand;
	iRand = getUnitInfo().getAnimalAttackWeight();
	if (iRand < 1)
	{
		return false;
	}
	//iRand = GC.getDefineINT("WILD_ANIMAL_ATTACK_WEIGHT");
	iRand = GC.getGameINLINE().getSorenRandNum(iRand, "Wild Animal Attack");

	if (iRand < 90)
	{
		if (AI_anyAttack(1, 50))
		//if (AI_smartAttack(1, 0, 10, NULL))
		{
			return true;
		}
	}

	if (iRand < 50)
	{
		if (AI_anyAttack(1, 40))
		{
			return true;
		}
	}

	if (iRand < 30)
	{
		if (AI_anyAttack(1, 20))
		{
			return true;
		}
	}

	if (iRand < 15)
	{
		if (AI_anyAttack(1, 10))
		{
			return true;
		}
	}

	if (iRand < 5)
	{
		if (AI_anyAttack(1, 0))
		{
			return true;
		}
	}
	return false;
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_animalLandPatrol()
{
	PROFILE_FUNC();

	const CvPlot* pBestPlot;
	int iValue, iRand;
	int iBestValue;
	iBestValue = 0;
	pBestPlot = NULL;

	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		const CvPlot* const pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (AI_plotValid(pAdjacentPlot))
			{
				//if (!pAdjacentPlot->isVisibleEnemyUnit(this))
				if (!pAdjacentPlot->isCity() && !pAdjacentPlot->isEurope() && pAdjacentPlot->getNumUnits() == 0)
				{
					if (canMoveInto(*pAdjacentPlot))
					{
						iValue = 0;
						iRand = getUnitInfo().getAnimalPatrolWeight();

						if (iRand > 0)
						{
							iValue += (1 + GC.getGameINLINE().getSorenRandNum(iRand, "Wild Land Animal Patrol - Base Weight"));

							if (pAdjacentPlot->getTerrainType() != NO_TERRAIN)
							{
								if (getUnitInfo().getTerrainNative(pAdjacentPlot->getTerrainType()))
								{
									iRand = GC.getWILD_ANIMAL_LAND_TERRAIN_NATIVE_WEIGHT();
									iValue += (1 + GC.getGameINLINE().getSorenRandNum(iRand, "Wild Land Animal Patrol - Terrain Weight"));
								}
							}

							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestPlot = pAdjacentPlot;
							}
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		FAssert(!atPlot(pBestPlot));
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE());
		return true;
	}

	return false;
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_animalSeaPatrol()
{
	PROFILE_FUNC();

	const CvPlot* pBestPlot;
	int iBestValue;
	iBestValue = 0;
	pBestPlot = NULL;

	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		const CvPlot* const pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (AI_plotValid(pAdjacentPlot))
			{
				if (!pAdjacentPlot->isCity() && !pAdjacentPlot->isEurope() && pAdjacentPlot->getNumUnits() == 0)
				{
					if (canMoveInto(*pAdjacentPlot))
					{
						const int iRand = getUnitInfo().getAnimalPatrolWeight();
						if (iRand > 0)
						{
							const int iValue = (1 + GC.getGameINLINE().getSorenRandNum(iRand, "Wild Sea Animal Patrol - Base Weight"));

							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestPlot = pAdjacentPlot;
							}
 						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		FAssert(!atPlot(pBestPlot));
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE());
		return true;
	}

	return false;
}
// < JAnimals Mod End >


// R&R, ray, Fleeing Units - START
void CvUnitAI::AI_FleeingMove()
{
	PROFILE_FUNC();

	CvPlot* pAdjacentPlot;
	CvPlot* pBestPlot;
	int iValue, iRand;
	int iBestValue;
	int iI;

	iBestValue = 0;
	pBestPlot = NULL;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (AI_plotValid(pAdjacentPlot))
			{
				if (!pAdjacentPlot->isVisibleEnemyUnit(this) && !pAdjacentPlot->isCity())
				{
					if (generatePath(pAdjacentPlot, 0, true))
					{
						iValue = 0;
						iRand = 100;

						iValue += (1 + GC.getGameINLINE().getSorenRandNum(iRand, "Fleeing - Base Weight"));
						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							pBestPlot = getPathEndTurnPlot();
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		FAssert(!atPlot(pBestPlot));
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_IGNORE_DANGER);
	}
	else
	{
		getGroup()->pushMission(MISSION_SKIP);		
	}
}
// R&R, ray, Fleeing Units - FALSE

void CvUnitAI::AI_colonistMove()
{
	if (isCargo())
	{
		if (plot()->isCity())
		{ 
			unload(); // checks canUnload internally
			return; // Need to return so that we run the non-cargo logic further below
		}

		// Skip our turn If the transport is already on a mission 
		if (getTransportUnit()->getGroup()->AI_getMissionAIType() != NO_MISSIONAI)
			getGroup()->pushMission(MISSION_SKIP);
		if (AI_joinOptimalCity())
			return;
		if (AI_joinCity())
			return;
		if (AI_joinCity(-1, /*bRequireJoinable*/false))
			return;
		getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	const bool bDanger = GET_PLAYER(getOwnerINLINE()).AI_getPlotDanger(plot(), 2) > 0;

	// Retreat when in danger and outside friendly territory
	if (bDanger && plot()->getOwner() != getOwner())
	{
		if (AI_retreatToCity(false, MAX_INT))
			return;
	}

	if ((m_pUnitInfo->getLearnTime() >= 0) && (getProfession() == GC.getCivilizationInfo(getCivilizationType()).getDefaultProfession()))
	{
		if (GC.getGameINLINE().AI_gameCompletePercent() < 25)
		{
		if (AI_learn())
		{
			return;
		}
	}
	}

	if (AI_joinOptimalCity())
	{
		return;
	}

	if (AI_joinCity())
	{
		return;
	}

	// Consider jobs that we would be a decent fit for
	if (AI_changeUnitAIType(99))
	{
		return;
	}

	/*
	CvCity* pCity = plot()->getPlotCity();
	if (pCity != NULL)
	{
		// We might want to swap places with a citizen
		if (AI_betterJob())
		{
			return;
		}
	}
	*/

	// Consider any job to avoid being unemployed :(
	if (AI_changeUnitAIType(49))
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitAI::AI_settlerMove()
{
	PROFILE_FUNC();

	CvPlayerAI const& kOwner = GET_PLAYER(getOwner()); // K-Mod
	const bool bDanger = GET_PLAYER(getOwnerINLINE()).AI_getUnitDanger(this, 2, false, false);
	int iMinFoundValue = (GC.getGame().getGameTurn() > 20) ? 1 : 2;

	if (isCargo())
	{
		// Skip our turn If the transport is already on a mission 
		if (getTransportUnit()->getGroup()->AI_getMissionAIType() != NO_MISSIONAI)
			getGroup()->pushMission(MISSION_SKIP);

		if (AI_settlerSeaTransport(iMinFoundValue))
		{
			return;
		}

		// If we cannot find any suitable city spot, disembark
		// so that we don't needlessly occupy a cargo hold
		// TODO: Check if the ship has other cargo
		if (canUnload())
			unload();

		getGroup()->pushMission(MISSION_SKIP);
		return;
	}
	
	if (isNative())
	{
		if (AI_foundRange(7))
		{
			return;
		}
		if (AI_found())
		{
			return;
		}
		if (GC.getGame().getGameTurn() - AI_getLastAIChangeTurn() > 10)
		{
			AI_setUnitAIType(UNITAI_DEFENSIVE);
			return;
		}
	}

	if ((GET_PLAYER(getOwnerINLINE()).getNumCities() == 0) && (plot()->getNearestEurope() != NO_EUROPE))
	{
		if (canFound(plot()))
		{
			found();
			return;
		}
	}

	//bool const bDanger = kOwner.AI_isAnyPlotDanger(getPlot()); // advc.opt
	// K-Mod
	if (bDanger)
	{
		if (!getGroup()->canDefend() ||
			100 * kOwner.AI_localAttackStrength(plot()) >
			80 * AI_getGroup()->AI_sumStrength(0))
			// K-Mod end
		{	// flee
			joinGroup(NULL);
			if (AI_retreatToCity())
				return;
		}
	}

	if (AI_found(iMinFoundValue))
	{
		return;
	}

	// K-Mod: sometimes an unescorted settler will join up with an escort mid-mission..
	//if (iAreaBestFoundValue + iOtherBestFoundValue > 0) // advc.040: But surely not if we have nowhere to settle
	if (getGroup()->getNumUnits() == 1)
	{
		CvPlayerAI const& kOwner = GET_PLAYER(getOwner()); // K-Mod

		FOR_EACH_GROUPAI_VAR(pLoopGroup, kOwner)
		{
			if (pLoopGroup == getGroup() || pLoopGroup->getNumUnits() <= 0)
				continue;
			if (pLoopGroup->AI_getMissionAIUnit() == this &&
				pLoopGroup->AI_getMissionAIType() == MISSIONAI_GROUP)
			{
				int iPathTurns = MAX_INT;
				generatePath(&pLoopGroup->getPlot(), MOVE_NO_ENEMY_TERRITORY, true, &iPathTurns, 2);
				if (iPathTurns > 2)
					continue;
				CvPlot& kEndTurnPlot = *getPathEndTurnPlot();
				if (at(kEndTurnPlot))
				{
					//getGroup()->pushMission(MISSION_SKIP, 0, 0, 0, false, false, MISSIONAI_GROUP, pEndTurnPlot);
					pLoopGroup->mergeIntoGroup(getGroup());
					FAssert(getGroup()->getNumUnits() > 1);
					FAssert(getGroup()->getHeadUnitAIType() == UNITAI_SETTLER);
					logBBAI("CvUnitAI::AI_settlerMove() mergeIntoGroup");
					AI_setMovePriority(1); // Yield may be needed since groups may have changed ?
				}
				else
				{
					CvSelectionGroupAI* pGroup = AI_getGroup(); // advc
					// if we were on our way to a site, keep the current mission plot.
					if (pGroup->AI_getMissionAIType() == MISSIONAI_FOUND &&
						pGroup->AI_getMissionAIPlot() != NULL)
					{
						pushGroupMoveTo(kEndTurnPlot, static_cast<MovementFlags>(MOVE_NO_ENEMY_TERRITORY), false, false,
							MISSIONAI_FOUND, pGroup->AI_getMissionAIPlot());

					}
					else
					{
						pushGroupMoveTo(kEndTurnPlot, static_cast<MovementFlags>(MOVE_NO_ENEMY_TERRITORY), false, false,
							MISSIONAI_GROUP, NULL, pLoopGroup->getHeadUnit());
					}
				}
				//AI_setMovePriority(0); // Yield may be needed since groups may have changed ?
				return;
			}
		}
	} // K-Mod end

	// If no site is viable and we are escorted, do some exploration
	if (getGroup()->getNumUnits() > 1 && AI_explore(false))
	{
		return;
	}

	// Settlers should always be on their way towards a spot to settle
	// So if we get here, this is evidence that we may need to be picked
	// up by a ship
	if (AI_requestPickup())
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (canJoinCity(plot()))
	{
		joinCity();
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitAI::AI_workerMove()
{
	PROFILE_FUNC();

	if (isCargo())
	{
		// Skip our turn If the transport is already on a mission 
		if (getTransportUnit()->getGroup()->AI_getMissionAIType() != NO_MISSIONAI)
			getGroup()->pushMission(MISSION_SKIP);

		if (AI_unloadWhereNeeded())
		{
			return;
		}

		getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	if (AI_breakAutomation())
	{
		return;
	}

	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	CvCity* pCity = (plot()->getOwnerINLINE() == getOwnerINLINE()) ? plot()->getPlotCity() : NULL;
	bool bDanger = GET_PLAYER(getOwnerINLINE()).AI_getUnitDanger(this, 2, false, false);
	if (bDanger)
	{
		if (pCity != NULL)
		{
			if (canJoinCity(plot()))
			{
				joinCity();
				return;
			}
		}
		if (AI_retreatToCity())
		{
			return;
		}
		if (AI_safety())
		{
			return;
		}
	}

	/*
	if ((kOwner.getNumCities() == 0) && (kOwner.AI_getNumAIUnits(UNITAI_SETTLER) == 0))
	{
		if (canFound(NULL))
		{
			AI_setUnitAIType(UNITAI_SETTLER);
			AI_settlerMove();
			return;
		}
	}
	
	if (!isHuman())
	{
		if ((pCity != NULL) && (pCity->getOwnerINLINE() == getOwnerINLINE()))
		{
			if (AI_betterJob())
			{
				return;
			}
		}
	}
	*/

	if (isCargo())
	{
		if (AI_unloadWhereNeeded())
		{
			return;
		}

		getGroup()->pushMission(MISSION_SKIP);
		return;
	}


	bool bCanRoute;

	bCanRoute = canBuildRoute();

	pCity = getHomeCity();

	if (pCity == NULL)
	{
		if (plot()->getOwnerINLINE() == getOwnerINLINE())
		{
			pCity = plot()->getWorkingCity();
		}
	}

	if (pCity != NULL)
	{
		if (pCity->AI_getWorkersNeeded() > 0)
		{
			if (AI_improveCity(pCity))
			{
				if (getHomeCity() != pCity)
				{
					setHomeCity(pCity);
				}
				return;
			}
		}
	}

	if (AI_nextCityToImprove(pCity))
	{
		return;
	}
#if 0
	//R&R mod, vetiarvind, Super Forts merge begin *canal* *choke*

	if (GC.getGame().getSorenRandNum(5, "AI Worker build Fort with Priority"))
	{
		CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());
		bool bCanal = kPlayer.countNumCoastalCities() > 0;

		if (AI_fortTerritory(bCanal))
		{
			return;
		}
	}
	// Super Forts end
#endif

	if (pCity != NULL)
	{
		if (pCity->AI_getWorkersNeeded() == 0)
		{
			if (AI_improveCity(pCity))
			{
				if (getHomeCity() != pCity)
				{
					setHomeCity(pCity);
				}
				return;
			}
		}
	}

	AI_setUnitAIType(UNITAI_COLONIST);
	return;
}

void CvUnitAI::AI_missionaryMove()
{
	PROFILE_FUNC();

	if (AI_breakAutomation())
	{
		return;
	}

	if (isCargo())
	{
		// Skip our turn If the transport is already on a mission 
		if (getTransportUnit()->getGroup()->AI_getMissionAIType() != NO_MISSIONAI)
			getGroup()->pushMission(MISSION_SKIP);

		if (AI_unloadWhereNeeded())
		{
			return;
		}

		getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	CvCity* pCity = (plot()->getOwnerINLINE() == getOwnerINLINE()) ? plot()->getPlotCity() : NULL;
	bool bDanger = GET_PLAYER(getOwnerINLINE()).AI_getUnitDanger(this, 2, false, false);
	if (bDanger)
	{
		if (pCity != NULL)
		{
			if (canJoinCity(plot()))
			{
				joinCity();
				return;
			}
		}
		if (AI_retreatToCity())
		{
			return;
		}
		if (AI_safety())
		{
			return;
		}
	}

	if (!isHuman() && pCity != NULL)
	{
		if (AI_betterJob())
		{
			return;
		}
	}

	//Cheesey exploit?
	if (canLearn())
	{
		UnitTypes eTeachUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(plot()->getPlotCity()->getTeachUnitClass());
		if (getUnitInfo().getMissionaryRateModifier() < GC.getUnitInfo(eTeachUnit).getMissionaryRateModifier())
		{
			learn();
			return;
		}
	}

	pCity = plot()->getPlotCity();
	if (pCity != NULL)
	{
		if (!pCity->isScoutVisited(getTeam()))
		{
			if (canSpeakWithChief(plot()))
			{
				speakWithChief();
				FAssert(pCity->isScoutVisited(getTeam()));
			}
		}
	}

	if (AI_spreadReligion())
	{
		return;
	}

	if (area()->getNumUnrevealedTiles(getTeam()) > 0)
	{
		if (AI_exploreOpenBorders(4))
		{
			return;
		}

		if (AI_explore(true))
		{
			return;
		}
	}

	//We should try and convert profession to something else
	//When this is possible.

	if (AI_retreatToCity())
	{
		AI_setUnitAIType(UNITAI_COLONIST);
		GET_PLAYER(getOwnerINLINE()).AI_changeNumRetiredAIUnits(UNITAI_MISSIONARY, 1);
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}



// WTP, ray, Native Trade Posts - START
void CvUnitAI::AI_nativeTraderMove()
{
	PROFILE_FUNC();

	if (isCargo())
	{
		// Skip our turn If the transport is already on a mission 
		if (getTransportUnit()->getGroup()->AI_getMissionAIType() != NO_MISSIONAI)
			getGroup()->pushMission(MISSION_SKIP);

		if (AI_unloadWhereNeeded())
		{
			return;
		}
	
		getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	if (AI_breakAutomation())
	{
		return;
	}

	CvCity* pCity = (plot()->getOwnerINLINE() == getOwnerINLINE()) ? plot()->getPlotCity() : NULL;
	bool bDanger = GET_PLAYER(getOwnerINLINE()).AI_getUnitDanger(this, 2, false, false);
	if (bDanger)
	{
		if (pCity != NULL)
		{
			if (canJoinCity(plot()))
			{
				joinCity();
				return;
			}
		}
		if (AI_retreatToCity())
		{
			return;
		}
		if (AI_safety())
		{
			return;
		}
	}

	if (!isHuman() && pCity != NULL)
	{
		if (AI_betterJob())
		{
			return;
		}
	}

	pCity = plot()->getPlotCity();
	if (pCity != NULL)
	{
		if (!pCity->isScoutVisited(getTeam()))
		{
			if (canSpeakWithChief(plot()))
			{
				speakWithChief();
				FAssert(pCity->isScoutVisited(getTeam()));
			}
		}
	}

	if (AI_spreadTradePosts())
	{
		return;
	}

	if (area()->getNumUnrevealedTiles(getTeam()) > 0)
	{
		if (AI_exploreOpenBorders(4))
		{
			return;
		}

		if (AI_explore(true))
		{
			return;
		}
	}

	//We should try and convert profession to something else
	//When this is possible.

	if (AI_retreatToCity())
	{
		AI_setUnitAIType(UNITAI_COLONIST);
		GET_PLAYER(getOwnerINLINE()).AI_changeNumRetiredAIUnits(UNITAI_MISSIONARY, 1);
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}
// WTP, ray, Native Trade Posts - END




void CvUnitAI::AI_scoutMove()
{
	CvCity* pCity = plot()->getPlotCity();

	if (AI_breakAutomation())
	{
		return;
	}

	if (isCargo())
	{
		// TAC - AI Explore from Ship endless loop fix - koma13 - START
		/*
		if (canUnload())
		{
			if (plot()->area()->getNumUnrevealedTiles(getTeam()) > 0)
			{
				unload();
				return;
			}
		}
		*/
		if (AI_exploreFromShip(6))
		{
			return;
		}

		if (AI_unloadWhereNeeded())
		{
			return;
		}
		/*
		if (canUnload())
		{
			unload();
			return;
		}
		*/

		if ((pCity != NULL) && (pCity->getOwnerINLINE() == getOwnerINLINE()))
		{
			AI_setUnitAIType(UNITAI_COLONIST);
			GET_PLAYER(getOwnerINLINE()).AI_changeNumRetiredAIUnits(UNITAI_SCOUT, 1);
		}
		else
		{
			getGroup()->pushMission(MISSION_SKIP);
		}
		// TAC - AI Explore from Ship endless loop fix - koma13 - END

		return;
	}

	if (!isHuman())
	{
		if ((pCity != NULL) && (pCity->getOwnerINLINE() == getOwnerINLINE()))
		{
			if (AI_betterJob())
			{
				return;
			}
		}
	}

	if (isCargo())
	{
		if (AI_unloadWhereNeeded())
		{
			return;
		}
		getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	if (AI_heal())
	{
		return;
	}

	if (pCity != NULL)
	{
		if (!isHuman())
		{
			if (GET_PLAYER(getOwnerINLINE()).AI_professionSuitability(getUnitType(), getProfession()) < 100)
			{
				if (canClearSpecialty())
				{
					clearSpecialty();
					return;
				}
			}
		}
		if (!pCity->isScoutVisited(getTeam()))
		{
			if (canSpeakWithChief(plot()))
			{
				speakWithChief();
				FAssert(pCity->isScoutVisited(getTeam()));
				return;
			}
		}
		if (canLearn())
		{
			UnitTypes eTeachUnit = getLearnUnitType(plot());

			if (GET_PLAYER(getOwnerINLINE()).AI_professionSuitability(eTeachUnit, getProfession()) > 100)
			{
				learn();
				return;
			}
		}
	}

	// TAC - AI Scouts - koma13 - START
	if (AI_anyAttack(2, 75))
	{
		return;
	}
	// TAC - AI Scouts - koma13 - END

	if (AI_exploreRange(4))
	{
		return;
	}

	if (AI_goody())
	{
		return;
	}

	if (area()->getNumUnrevealedTiles(getTeam()) > 0)
	{
		if (AI_explore())
		{
			return;
		}
	}

	if (!isHuman())
	{
		bool bRequestPickup = false;
		CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
		CvArea* pLoopArea;
		int iLoop;
		for(pLoopArea = GC.getMap().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop))
		{
			if (!pLoopArea->isWater())
			{
				if ((pLoopArea->getNumUnrevealedTiles(getTeam()) > 7) && pLoopArea->getNumAIUnits(getOwnerINLINE(), UNITAI_SCOUT) == 0)
				{
					bRequestPickup = true;
					break;
				}
			}
		}

		if (bRequestPickup)
		{
			if (AI_requestPickup())
			{
				return;
			}
		}

		/*
		{	//Become a Colonist
			if (m_pUnitInfo->getUnitAIType(UNITAI_COLONIST))
			{
				AI_setUnitAIType(UNITAI_COLONIST);
				return;
			}
		}
		*/
	}
	else
	{
		FAssert(isAutomated());
		if (plot()->getOwnerINLINE() == getOwnerINLINE())
		{
			getGroup()->setAutomateType(NO_AUTOMATE);
			return;
		}
	}

	if (AI_retreatToCity())
	{
		return;
	}

	
	if (isHuman()) 
	{
		const CvWString szBuffer(gDLL->getText("TXT_KEY_SCOUT_FINISHED_EXPLORING"));
		gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), NULL, MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), true, true);
		getGroup()->setAutomateType(NO_AUTOMATE);
	}
	else
	{
		getGroup()->pushMission(MISSION_SKIP);
	}

	return;
}

void CvUnitAI::AI_treasureMove()
{
	PROFILE_FUNC();

	if (isCargo())
	{
		getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	// WTP, merge Treasures, of Raubwuerger - START
	// implementation of ray, AI logic for Merge Treasures
	// will trigger anyways only in save Cities or Native Villages
	if (canMergeTreasures())
	{
		mergeTreasures();
		logBBAI("INFO: Treasure merged for Player %S Unit %d: Unit stuck in loop: %S(%S)[%d, %d]",
			GET_PLAYER(getOwnerINLINE()).getCivilizationDescription(), getID(),
			getName().GetCString(), GET_PLAYER(getOwnerINLINE()).getName(),
			getX_INLINE(), getY_INLINE());
		AI_setMovePriority(0);
		return;
	}
	// WTP, merge Treasures, of Raubwuerger - END

	if (AI_loadAdjacent(plot(), false))
	{
		return;
	}

	// R&R, ray, Natives raiding party - START
	//bool bDanger = GET_PLAYER(getOwnerINLINE()).AI_getUnitDanger(this, 2, false, false);
	bool bDanger = GET_PLAYER(getOwnerINLINE()).AI_getUnitDanger(this, 2);
	// R&R, ray, Natives raiding party - END
	if (bDanger)
	{
		if (AI_retreatToCity())
		{
			return;
		}

		if (AI_safety())
		{
			return;
		}
	}

	CvCity* pCity = (plot()->getOwnerINLINE() == getOwnerINLINE()) ? plot()->getPlotCity() : NULL;

	bool bAtWar = GET_TEAM(getTeam()).getAnyWarPlanCount();

	if (pCity != NULL)
	{
		// R&R, ray , Native Treasures - START
		if(GET_PLAYER(getOwnerINLINE()).isNative())
		{
			int iGoldForNativeTreasure = getYieldStored();
			OOS_LOG("Native trader gold", iGoldForNativeTreasure);
			GET_PLAYER(getOwnerINLINE()).changeGold(iGoldForNativeTreasure);
			kill(false);
			return;
		}
		// R&R, ray , Native Treasures - END

		if (!GET_PLAYER(getOwnerINLINE()).AI_hasSeaTransport(this) || bAtWar)
		{
			if (canKingTransport())
			{
				kingTransport(true);
				return;
			}
		}
	}

	if (AI_treasureRetreat(MAX_INT))
	{
		return;
	}

	if (AI_requestPickup())
	{
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitAI::AI_yieldUhMove()
{
	FAssert(isCargo());

	if (AI_getUnitAIState() == UNITAI_STATE_SELL_TO_NATIVES)
	{
		if (AI_yieldNativeDestination())
		{
			return;
		}
		if (GC.getGameINLINE().getGameTurn() - getGameTurnCreated() > 6)
		{
			GET_PLAYER(getOwnerINLINE()).AI_clearStrategy(STRATEGY_SELL_TO_NATIVES);
			AI_setUnitAIState(UNITAI_STATE_PURCHASED);
		}
		else
		{
			AI_setMovePriority(0);
			return;
		}
	}
	if (AI_yieldDestination())
	{
		AI_setMovePriority(MOVE_PRIORITY_MIN);
		return;
	}

	AI_setMovePriority(0);
	return;
}

void CvUnitAI::AI_generalMove()
{
	PROFILE_FUNC();

	std::vector<UnitAITypes> aeUnitAITypes;

	aeUnitAITypes.clear();
	// Erik: Do not consider defensive units
	//aeUnitAITypes.push_back(UNITAI_DEFENSIVE);
	aeUnitAITypes.push_back(UNITAI_OFFENSIVE);
	aeUnitAITypes.push_back(UNITAI_ATTACK_CITY);
	//aeUnitAITypes.push_back(UNITAI_COUNTER);

	// Erik: These are required to get the AI to
	// use their great admirals
	//aeUnitAITypes.push_back(UNITAI_ASSAULT_SEA);
	aeUnitAITypes.push_back(UNITAI_COMBAT_SEA);
	// Erik: Let's avoid great pirates :P
	//aeUnitAITypes.push_back(UNITAI_PIRATE_SEA);

	if (AI_lead(aeUnitAITypes))
	{
		return;
	}

	if (AI_retreatToCity(false, MAX_INT))
	{
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitAI::AI_counterMove()
{
	if (isCargo())
	{
		AI_handleEmbarkedMilitary();
		return;
	}

	PROFILE_FUNC();
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	bool bDanger = kOwner.AI_getPlotDanger(plot(), 2, false, true);

	if (!isHuman() && getProfession() != NO_PROFESSION)
	{
		CvCity* pCity = plot()->getPlotCity();
		if ((pCity != NULL) && (pCity->getOwnerINLINE() == getOwnerINLINE()))
		{
			if (isCargo())
			{
				if (AI_allowedToJoin(pCity))
				{
					pCity->addPopulationUnit(this, NO_PROFESSION);
					return;
				}
			}

			if (!bDanger && ((getFortifyTurns() == 0) || (GC.getGameINLINE().getSorenRandNum(10, "AI better job")  == 0)))
			{
				if (AI_betterJob())
				{
					return;
				}
				if (canClearSpecialty())
				{
					clearSpecialty();
					return;
				}
			}
		}
	}

	CvPlot* pMissionPlot = getGroup()->AI_getMissionAIPlot();

	if (bDanger)
	{
		if (AI_anyAttack(3, 60))
		{
			return;
		}

		if (!(kOwner.AI_isStrategy(STRATEGY_REVOLUTION)))
		{
			if (AI_leaveAttack(1, 30, 120))
			{
				return;
			}
		}

		if (isHurt())
		{
			if (AI_heal())
			{
				return;
			}
		}

		if (bDanger && !(kOwner.AI_isStrategy(STRATEGY_REVOLUTION)))
		{
			if (AI_counter(6))
			{
				return;
			}
		}

		if (AI_guardCityCounter())
		{
			return;
		}
	}

	CvCity* pCity = plot()->getPlotCity();

	if (pCity != NULL && AI_allowedToJoin(pCity))
	{
		pCity->addPopulationUnit(this, NO_PROFESSION);
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitAI::AI_defensiveBraveMove()
{
	FAssert(canMove());
	bool bAtWar = GET_TEAM(getTeam()).getAnyWarPlanCount();

	if (GC.getGameINLINE().getSorenRandNum(10, "AI upgrade unit profession") == 0)
	{
		AI_upgradeProfession();
	}

	AreaAITypes eAreaAI = area()->getAreaAIType(getTeam());

	FAssert(eAreaAI == AREAAI_NEUTRAL || eAreaAI == AREAAI_OFFENSIVE || eAreaAI == AREAAI_BALANCED || eAreaAI == AREAAI_DEFENSIVE);

	CvCity* pCity = (plot()->getOwner() == getOwner()) ? plot()->getPlotCity() : NULL;

	CvPlayerAI& kOwner = GET_PLAYER(getOwner());

	bool bDanger = kOwner.AI_getPlotDanger(plot(), 2, false);

	if (pCity != NULL)
	{
		if (AI_guardCityBestDefender())
		{
			return;
		}

		if (bDanger)
		{
			if (AI_leaveAttack(1, 20 + GC.getGameINLINE().getSorenRandNum(30, "Native leave city attack"), 120))
			{
				return;
			}
		}

		if (pCity->AI_isDefended(-2))
		{
			if (AI_joinCityBrave())
			{
				return;
			}
		}

		if (getHomeCity() == NULL)
		{
			setHomeCity(pCity);
		}
		else if (pCity != getHomeCity())
		{
			if (getHomeCity()->AI_isDefended())
			{
				if (!pCity->AI_isDefended())
				{
					setHomeCity(pCity);
				}
			}
		}

		if (pCity == getHomeCity())
		{
			if (pCity->AI_isDefended(-1))
			{
				if (AI_joinCityBrave())
				{
					return;
				}

				// R&R, ray, Natives raiding party - START
				if (kOwner.AI_hasPotentialRaidTarget())
				{
					if (GC.getGameINLINE().getGameTurn() > 30)
					{
						if (kOwner.AI_findTargetCity(pCity->area()) != NULL)
						{
							int iNumPotentialRaids = kOwner.AI_getNumPotentialRaids();
							int iNumRaidingParties = kOwner.AI_countNumHomedUnits(NULL, UNITAI_DEFENSIVE, UNITAI_STATE_RAIDING_PARTY);

							if (iNumPotentialRaids > iNumRaidingParties)
							{
								AI_setUnitAIState(UNITAI_STATE_RAIDING_PARTY);
							}
							else if (GC.getGame().getSorenRandNum(100, "AI native start raiding party") < GC.getRANDOM_NATIVE_RAID_BASECHANCE())
							{
								int iRand = GC.getLeaderHeadInfo(GET_PLAYER(getOwner()).getPersonalityType()).getContactRand(CONTACT_NATIVE_RAID);
								//iRand += std::max(0, 40 - GC.getGameINLINE().getGameTurn());

								if (iRand > GC.getGame().getSorenRandNum(100 * iNumRaidingParties, "AI native raiding party"))
								{
									AI_setUnitAIState(UNITAI_STATE_RAIDING_PARTY);
								}
							}
						}
					}
				}

				if (AI_getUnitAIState() != UNITAI_STATE_RAIDING_PARTY)
				{
					// R&R, ray, Natives Trading - START
					if (pCity->AI_canMakeGift() || pCity->AI_canMakeTrade())
					{
						if (pCity->AI_canMakeGift())
						{
							int iRand = GC.getLeaderHeadInfo(GET_PLAYER(getOwnerINLINE()).getPersonalityType()).getContactRand(CONTACT_YIELD_GIFT);
							iRand += std::max(0, 40 - GC.getGameINLINE().getGameTurn());
							if (iRand > GC.getGame().getSorenRandNum(100 * plot()->plotCount(PUF_isUnitAIType, UNITAI_DEFENSIVE, -1, getOwner()), "AI native bear gifts"))
							{
								AI_setUnitAIState(UNITAI_STATE_BEARING_GIFTS);
								pCity->AI_setGiftTimer(GC.getLeaderHeadInfo(GET_PLAYER(getOwnerINLINE()).getPersonalityType()).getContactDelay(CONTACT_YIELD_GIFT));
							}
						}

						if (pCity->AI_canMakeTrade() && AI_getUnitAIState() != UNITAI_STATE_BEARING_GIFTS)
						{
							int iRandTrade = GC.getLeaderHeadInfo(GET_PLAYER(getOwnerINLINE()).getPersonalityType()).getContactRand(CONTACT_YIELD_GIFT);
							iRandTrade = iRandTrade * 2;
							if (iRandTrade > GC.getGame().getSorenRandNum(100 * plot()->plotCount(PUF_isUnitAIType, UNITAI_DEFENSIVE, -1, getOwner()), "AI native trade"))
							{
								AI_setUnitAIState(UNITAI_STATE_BEARING_TRADE);
								pCity->AI_setTradeTimer(GC.getLeaderHeadInfo(GET_PLAYER(getOwnerINLINE()).getPersonalityType()).getContactDelay(CONTACT_YIELD_GIFT) / 2);
							}
						}
					}
					// R&R, ray, Natives Trading - END
					else if (GC.getGame().getSorenRandNum(10, "AI native start wandering") == 0)
					{
						if (kOwner.AI_countNumHomedUnits(pCity, NO_UNITAI, UNITAI_STATE_WANDER) < 2)
						{
							AI_setUnitAIState(UNITAI_STATE_WANDER);
							//fall through.
						}
					}
				}
				// R&R, ray, Natives raiding party - END
			}
		}
	}

	if (AI_getUnitAIState() == UNITAI_STATE_BEARING_GIFTS)
	{
		if (plot()->isCity() && (!GET_PLAYER(plot()->getOwner()).isNative()))
		{
			//Make gift and do diplomacy
			GET_PLAYER(getOwnerINLINE()).AI_nativeYieldGift(this);
			AI_setUnitAIState(UNITAI_STATE_RETURN_HOME);

			finishMoves();
			return;
		}
		else
		{
			if (AI_travelToEuropeColony(ATTITUDE_CAUTIOUS, ATTITUDE_FRIENDLY, 5)) //XXX XMLize the range?
			{
				return;
			}

			AI_setUnitAIState(UNITAI_STATE_RETURN_HOME);
		}
	}

	// R&R, ray, Natives Trading -- START
	if (AI_getUnitAIState() == UNITAI_STATE_BEARING_TRADE)
	{
		if (plot()->isCity() && (!GET_PLAYER(plot()->getOwner()).isNative()))
		{
			//trade
			GET_PLAYER(getOwnerINLINE()).AI_nativeTrade(this);
			AI_setUnitAIState(UNITAI_STATE_RETURN_HOME);

			finishMoves();
			return;
		}
		else
		{
			if (AI_travelToEuropeColony(ATTITUDE_CAUTIOUS, ATTITUDE_FRIENDLY, 5)) //XXX XMLize the range?
			{
				return;
			}

			AI_setUnitAIState(UNITAI_STATE_RETURN_HOME);
		}
	}
	// R&R, ray, Natives Trading -- END

	if (AI_getUnitAIState() == UNITAI_STATE_RETURN_HOME)
	{
		if (plot()->isCity() && (plot()->getPlotCity() == getHomeCity()))
		{
			AI_setUnitAIState(UNITAI_STATE_DEFAULT);
		}
		else if (AI_guardHomeColony())
		{
			return;
		}
	}

	if (AI_getUnitAIState() == UNITAI_STATE_WANDER)
	{
		// R&R, ray, Natives raiding party - START
		if (AI_nativeRaidTreasureUnit())
		{
			AI_setUnitAIState(UNITAI_STATE_WANDER);
			return;
		}
		// R&R, ray, Natives raiding party - END

		if (AI_bravePatrol())
		{
			return;
		}
		if ((kOwner.AI_cityDistance(plot()) > 6) || (GC.getGameINLINE().getGameTurn() - AI_getLastAIChangeTurn() > 8))
		{
			AI_setUnitAIState(UNITAI_STATE_RETURN_HOME);
		}
	}

	// R&R, ray, Natives raiding party - START
	if (AI_getUnitAIState() == UNITAI_STATE_RAIDING_PARTY)
	{
		if (plot()->isCity() && (plot()->getPlotCity() == kOwner.AI_findTargetCity(area())))
		{
			if (raidCity(plot()->getPlotCity()))
			{
				AI_setUnitAIState(UNITAI_STATE_RETURN_HOME);

				finishMoves();
				return;
			}
		}

		// R&R, ray, Monasteries and Forts - START
		if (AI_nativeRaidFort())
		{
			return;
		}
		// R&R, ray, Monasteries and Forts - END

		if (AI_nativeRaidTreasureUnit())
		{
			return;
		}

		if (AI_travelToEuropeColonyRaidTarget(ATTITUDE_FURIOUS, GC.getLeaderHeadInfo(GET_PLAYER(getOwnerINLINE()).getPersonalityType()).getNativeRaidAttitudeThreshold(), 5, MOVE_DIRECT_RAID))
		{
			return;
		}

		if (AI_nativeGoToRaidTargetCity(MOVE_DIRECT_RAID, 5, AI_nativePickRaidTargetCity(MOVE_DIRECT_RAID, 5)))
		{
			return;
		}

		if (AI_targetCityNative(MOVE_DIRECT_RAID))
		{
			return;
		}

		if (AI_bravePatrol())
		{
			return;
		}

		if ((kOwner.AI_cityDistance(plot()) > 12) || (GC.getGameINLINE().getGameTurn() - AI_getLastAIChangeTurn() > 30))
		{
			AI_setUnitAIState(UNITAI_STATE_RETURN_HOME);
		}
	}
	// R&R, ray, Natives raiding party - END

	if (kOwner.getCitiesLost() > 0)
	{
		if (kOwner.AI_getNumAIUnits(UNITAI_SETTLER) == 0)
		{
			if (area()->getBestFoundValue(getOwnerINLINE()) > 0)
			{
				if (kOwner.getTotalPopulation() >= 3 * kOwner.getNumCities())
				{
					if (pCity == NULL || pCity->AI_isDefended(-1))
					{
						AI_setUnitAIType(UNITAI_SETTLER);
						AI_settlerMove();
						return;
					}
				}
			}
		}
	}

	if (AI_group(UNITAI_DEFENSIVE))
	{
		// Group with other defenders
		return;
	}

	if (AI_guardHomeColony())
	{
		return;
	}

	if (AI_findNewHomeColony())
	{
		return;
	}

	if (getHomeCity() == NULL)
	{
		kill(true);//One way to deal with the homeless problem.
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitAI::AI_offensiveBraveMove()
{
	FAssert(canMove());
	bool bAtWar = GET_TEAM(getTeam()).getAnyWarPlanCount();

	if (GC.getGameINLINE().getSorenRandNum(10, "AI upgrade unit profession") == 0)
	{
		AI_upgradeProfession();
	}

	AreaAITypes eAreaAI = area()->getAreaAIType(getTeam());

	FAssert(eAreaAI == AREAAI_NEUTRAL || eAreaAI == AREAAI_OFFENSIVE || eAreaAI == AREAAI_BALANCED || eAreaAI == AREAAI_DEFENSIVE);

	CvCity* pCity = (plot()->getOwner() == getOwner()) ? plot()->getPlotCity() : NULL;

	CvPlayerAI& kOwner = GET_PLAYER(getOwner());

	const int iLoiterDistance = 2;

	bool bDanger = kOwner.AI_getPlotDanger(plot(), 2, true, true) > 0;

	if (eAreaAI == AREAAI_OFFENSIVE || eAreaAI == AREAAI_BALANCED || eAreaAI == AREAAI_DEFENSIVE)
	{
		if (pCity != NULL)
		{
			if (bDanger)
			{
				if (AI_leaveAttack(2, 1 + GC.getGameINLINE().getSorenRandNum(30, "AI native leave attack"), 120))
				{
					return;
				}
			}

			if (pCity->AI_isDefended(-1))
			{
				AI_setUnitAIState(UNITAI_STATE_ADVANCING);
			}
			else
			{
				AI_setUnitAIType(UNITAI_DEFENSIVE);
				getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_CITY, NULL);
				return;
			}
		}

		if (AI_getUnitAIState() == UNITAI_STATE_RETREATING)
		{
			if (!bDanger)
			{
				AI_setUnitAIState(UNITAI_STATE_ADVANCING);
			}
			else
			{
				if (AI_pillageRange(0))
				{
					return;
				}
				if (AI_anyAttack(1, 49))
				{
					return;
				}

				if (AI_guardHomeColony())
				{
					return;
				}

				if (AI_findNewHomeColony())
				{
					return;
				}
			}
		}

		if (AI_getUnitAIState() == UNITAI_STATE_GROUPING)
		{
			if (AI_anyAttack(1, 30))
			{
				return;
			}

			if (AI_pillageRange(1))
			{
				return;
			}
			if (AI_loiter(iLoiterDistance, iLoiterDistance, true))
			{
				return;
			}
		}

		if (AI_getUnitAIState() == UNITAI_STATE_CHARGING)
		{
			if (AI_anyAttack(1, 40))
			{
				return;
			}
			if (AI_pillageRange(0))
			{
				return;
			}
			if (AI_cityAttack(1, 10))
			{
				return;
			}
			if (AI_advance(true))
			{
				return;
			}
		}

		if (AI_getUnitAIState() == UNITAI_STATE_ADVANCING)
		{
			if (AI_anyAttack(1, 30))
			{
				return;
			}

			if (AI_pillageRange(1))
			{
				return;
			}

			CvTeamAI& kTeam = GET_TEAM(getTeam());
			if (AI_advance(true))
			{
				if (kTeam.AI_enemyCityDistance(plot()) <= iLoiterDistance)
				{
					AI_setUnitAIState(UNITAI_STATE_GROUPING);
				}
				return;
			}

			if (kTeam.AI_enemyCityDistance(plot()) <= iLoiterDistance)
			{
				AI_setUnitAIState(UNITAI_STATE_GROUPING);
			}
		}
	}
	else
	{
		AI_setUnitAIType(UNITAI_DEFENSIVE);
		return;
	}

	if (AI_guardHomeColony())
	{
		return;
	}

	if (AI_findNewHomeColony())
	{
		return;
	}

	if (getHomeCity() == NULL)
	{
		kill(true);//One way to deal with the homeless problem.
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitAI::AI_counterBraveMove()
{
	FAssert(canMove());
	bool bAtWar = GET_TEAM(getTeam()).getAnyWarPlanCount();

	if (GC.getGameINLINE().getSorenRandNum(10, "AI upgrade unit profession") == 0)
	{
		AI_upgradeProfession();
	}

	AreaAITypes eAreaAI = area()->getAreaAIType(getTeam());

	FAssert(eAreaAI == AREAAI_NEUTRAL || eAreaAI == AREAAI_OFFENSIVE || eAreaAI == AREAAI_BALANCED || eAreaAI == AREAAI_DEFENSIVE);

	CvCity* pCity = (plot()->getOwner() == getOwner()) ? plot()->getPlotCity() : NULL;

	CvPlayerAI& kOwner = GET_PLAYER(getOwner());

	const int iLoiterDistance = 2;

	bool bDanger = kOwner.AI_getPlotDanger(plot(), 2, true, true) > 0;

	if (eAreaAI == AREAAI_OFFENSIVE || eAreaAI == AREAAI_BALANCED || eAreaAI == AREAAI_DEFENSIVE)
	{
		if (pCity != NULL)
		{
			if (bDanger)
			{
				if (AI_leaveAttack(2, 10 + GC.getGameINLINE().getSorenRandNum(30, "AI native leave attack"), 120))
				{
					return;
				}
			}

			if (!pCity->AI_isDefended(-1))
			{
				AI_setUnitAIType(UNITAI_DEFENSIVE);
				getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_CITY, NULL);
				return;
			}
		}

		if (AI_pillageRange(1))
		{
			return;
		}

		if (AI_anyAttack(2, 20 + GC.getGameINLINE().getSorenRandNum(20, "AI counter attack")))
		{
			return;
		}

		if (AI_counter(5))
		{
			return;
		}
	}
	else
	{
		AI_setUnitAIType(UNITAI_DEFENSIVE);
		return;
	}

	if (AI_guardHomeColony())
	{
		return;
	}

	if (AI_findNewHomeColony())
	{
		return;
	}

	if (getHomeCity() == NULL)
	{
		kill(true);//One way to deal with the homeless problem.
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitAI::AI_transportMove()
{
	PROFILE_FUNC();

	CvCity* pCity = NULL;
	if (plot()->getPlotCity() != NULL)
	{
		if (plot()->getOwner() == getOwner())
		{
			pCity = plot()->getPlotCity();
		}
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitAI::AI_transportMoveRoutes()
{

	if (AI_breakAutomation())
	{
		return;
	}

	if (AI_getUnitAIState() == UNITAI_STATE_SAIL)
	{
		if (AI_sailToEurope())
		{
			return;
		}
	}

	if (getGroup()->AI_tradeRoutes())
	{
		return;
	}

	if (AI_getUnitAIState() == UNITAI_STATE_SAIL)
	{
		if (AI_sailToEurope())
		{
			return;
		}
	}

	if (AI_retreatToCity())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitAI::AI_transportMoveFull()
{
	if (AI_breakAutomation())
	{
		return;
	}

	bool bEenish = false;
	if (AI_getUnitAIState() == UNITAI_STATE_SAIL)
	{
		if (AI_sailToEurope())
		{
			return;
		}
		bEenish = true;
	}

	if (getGroup()->AI_tradeRoutes())
	{
		return;
	}

	if (AI_getUnitAIState() == UNITAI_STATE_SAIL)
	{
		FAssertMsg(!bEenish, "Now this is confusing!!!");
		if (AI_sailToEurope())
		{
			return;
		}
	}

	if (AI_retreatToCity())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;

}

void CvUnitAI::AI_imperialShipMove()
{

	CvPlayerAI& kOwner = GET_PLAYER(getOwner());
	int iMovePriority = AI_getMovePriority();
    bool bHasCargo = getGroup()->hasCargo();

	CvCity* pPlotCity = plot()->getPlotCity();
	if (pPlotCity != NULL && pPlotCity->getOwnerINLINE() != getOwnerINLINE())
	{
		pPlotCity = NULL;
	}

	if (GET_TEAM(getTeam()).getAnyWarPlanCount() == 0)
	{
		if (pPlotCity != NULL)
		{
			getGroup()->pushMission(MISSION_SKIP);
			return;
		}
		if (AI_retreatToCity())
		{
			return;
		}
		getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	if (getGroup()->AI_getMissionAIType() == MISSIONAI_EXPLORE)
    {
    	if (AI_anyAttack(1, 1))
    	{
    		return;
    	}

    	if (GC.getGame().getSorenRandNum(10, "AI wander aimlessly") != 0)
    	{
			if (AI_wanderAroundAimlessly())
			{
				return;
			}
    	}
    }

    //Move into range and bombard.
	if (bHasCargo)
	{
		if (AI_imperialSeaAssault())
		{
			return;
		}
	}
	else
	{
		if (isHurt())
		{
			if (AI_anyAttack(1, 70))
			{
				return;
			}

			// TAC - AI Revolution - koma13 - START
			//if (AI_sailToEurope())
			if (AI_retreatToCity())
			// TAC - AI Revolution - koma13 - END
			{
				return;
			}
		}
		if (AI_anyAttack(1, 30))
		{
			return;
		}
	}

	if (!kOwner.AI_isStrategy(STRATEGY_BUILDUP))
	{
		if (plot()->isAdjacentToLand())
		{
			if (canAttack())
			{
				if (AI_seaBombardRange(1))
				{
					return;
				}
			}

			if (AI_blockade(2))
			{
				return;
			}
		}
	}

	// TAC - AI Revolution - koma13 - START
	/*
	if (!bHasCargo)
	{
		if (kOwner.getNumEuropeUnits() > 0)
		{
			if (AI_sailToEurope())
			{
				if (kOwner.AI_isStrategy(STRATEGY_SMALL_WAVES) && (kOwner.AI_getStrategyDuration(STRATEGY_SMALL_WAVES) > 5))
				{
					kOwner.AI_clearStrategy(STRATEGY_SMALL_WAVES);
					kOwner.AI_setStrategy(STRATEGY_BUILDUP);
				}
				return;
			}
		}
	}
	*/
	// TAC - AI Revolution - koma13 - END

	if (AI_wanderAroundAimlessly())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitAI::AI_imperialSoldierMove()
{
	if (isCargo())
	{
		if (canMove() && AI_anyAttack(1, 50))
		{
			return;
		}
		if (AI_disembark(false))
		{
			return;
		}

		getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	CvCity* pPlotCity = plot()->getPlotCity();
	if (pPlotCity != NULL && pPlotCity->getOwnerINLINE() != getOwnerINLINE())
	{
		pPlotCity = NULL;
	}

	if (GET_TEAM(getTeam()).getAnyWarPlanCount() == 0)
    {
    	if (pPlotCity != NULL)
    	{
    		getGroup()->pushMission(MISSION_SKIP);
    		return;
    	}
    	if (AI_retreatToCity())
    	{
    		return;
    	}
    	getGroup()->pushMission(MISSION_SKIP);
    	return;
    }

	if (pPlotCity != NULL)
	{
		if (!pPlotCity->AI_isDefended(-1) || (isHurt()))
		{
			if (AI_smartAttack(1, 50, 95, NULL))
			{
				return;
			}
			getGroup()->pushMission(MISSION_SKIP);
			return;
		}
	}

	if (isHurt())
	{
		if (AI_retreat(4))
		{
			return;
		}
	}

	if (AI_anyAttack(1, 30))
	{
		return;
	}

	if (baseMoves() > 1)
	{
		if (AI_pillageRange(1))
		{
			return;
		}
	}

	int iAttackThreshold = GET_PLAYER(getOwnerINLINE()).AI_shouldAttackAdjacentCity(plot()) ? 0 : 30;
	if (AI_cityAttack(1, iAttackThreshold))
	{
		return;
	}

	if (AI_advance(false))
	{
		return;
	}

	if (AI_advance(true))
	{
		return;
	}

    if (AI_retreatToCity(false))
    {
        return;
    }

    getGroup()->pushMission(MISSION_SKIP);
    return;
}

void CvUnitAI::AI_imperialMountedMove()
{
	if (isCargo())
	{
		if (canMove() && AI_anyAttack(1, 50))
		{
			return;
		}
		if (AI_disembark(false))
		{
			return;
		}

		getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	CvCity* pPlotCity = plot()->getPlotCity();
	if (pPlotCity != NULL && pPlotCity->getOwnerINLINE() != getOwnerINLINE())
	{
		pPlotCity = NULL;
	}

	if (GET_TEAM(getTeam()).getAnyWarPlanCount() == 0)
    {
    	if (pPlotCity != NULL)
    	{
    		getGroup()->pushMission(MISSION_SKIP);
    		return;
    	}
    	if (AI_retreatToCity())
    	{
    		return;
    	}
    	getGroup()->pushMission(MISSION_SKIP);
    	return;
    }

	if (pPlotCity != NULL)
	{
		if (!pPlotCity->AI_isDefended(-1) || (isHurt()))
		{
			if (AI_smartAttack(1, 50, 95, NULL))
			{
				return;
			}
			getGroup()->pushMission(MISSION_SKIP);
			return;
		}
	}

	if (isHurt())
	{
		if (AI_retreat(4))
		{
			return;
		}
	}

	if (AI_anyAttack(1, 30))
	{
		return;
	}

	if (baseMoves() > 1)
	{
		if (AI_pillageRange(1))
		{
			return;
		}
	}

	int iAttackThreshold = GET_PLAYER(getOwnerINLINE()).AI_shouldAttackAdjacentCity(plot()) ? 0 : 40;
	if (AI_cityAttack(1, iAttackThreshold))
	{
		return;
	}

	if (AI_advance(false))
	{
		return;
	}

	if (AI_advance(true))
	{
		return;
	}

	if (AI_retreatToCity(false))
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}
void CvUnitAI::AI_imperialCannonMove()
{

	if (isCargo())
	{
		if (AI_disembark(false))
		{
			return;
		}

		getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	CvCity* pPlotCity = plot()->getPlotCity();
	if (pPlotCity != NULL && pPlotCity->getOwnerINLINE() != getOwnerINLINE())
	{
		pPlotCity = NULL;
	}

	if (GET_TEAM(getTeam()).getAnyWarPlanCount() == 0)
	{
		if (pPlotCity != NULL)
		{
			getGroup()->pushMission(MISSION_SKIP);
			return;
		}
		if (AI_retreatToCity())
		{
			return;
		}
		getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	if (pPlotCity != NULL)
	{
		if (!pPlotCity->AI_isDefended(-1) || (isHurt()))
		{
			if (AI_smartAttack(1, 50, 95, NULL))
			{
				return;
			}
			getGroup()->pushMission(MISSION_SKIP);
			return;
		}
	}

	if (isHurt())
	{
		if (AI_retreat(4))
		{
			return;
		}
	}

	if (AI_bombardCity())
	{
		return;
	}

	if (isHurt())
	{
		if (AI_retreat(4))
		{
			return;
		}
	}

	int iAttackThreshold = GET_PLAYER(getOwnerINLINE()).AI_shouldAttackAdjacentCity(plot()) ? 0 : 30;

	if (AI_cityAttack(1, iAttackThreshold))
	{
		return;
	}

	if (AI_anyAttack(1, 50))
	{
		return;
	}

	if (AI_advance(false))
	{
		return;
	}

	if (AI_advance(true))
	{
		return;
	}

	if (AI_retreatToCity(false))
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

//TAC Whaling, ray
void CvUnitAI::AI_workerSeaMove()
{
	PROFILE_FUNC();

	//koma13
	/*
	bool bDanger = GET_PLAYER(getOwnerINLINE()).AI_getUnitDanger(this, 2) || GET_PLAYER(getOwnerINLINE()).AI_getWaterDanger(plot(), 2);
	if (bDanger)
	{
		if (AI_retreatToCity())
		{
			return;
		}

		if (AI_safety())
		{
			return;
		}
	}*/

	if (AI_retreatFromDanger())
	{
		return;
	}
	//koma13
	if (isHurt())
	{
		if (AI_heal())
		{
			return;
		}

		if (AI_retreatToCity())
		{
			return;
		}
	}

	if (AI_gatherResource())
	{
		return;
	}

	if (isFullToBrim())
	{
		//koma13
		//if (AI_transportReturnToPort(true, getHomeCity()))
		if (AI_transportReturnToPort(true, NULL))
		{
			return;
		}
	}

	//koma13
	if (isWhalingBoat() || isFishingBoat()) // R&R, ray, High Sea Fishing - START
	{
		if (AI_moveToWhale()) // works as well for fish
		{
			return;
		}
	}

	else
	{
		if (AI_exploreRessource())
		{
			return;
		}
	}

	if (AI_exploreHighSeas(1))
	{
		return;
	}

	// R&R, ray, High Sea Fishing
	// if (isWhalingBoat())
	if (isWhalingBoat() || isFishingBoat())
	{
		if (AI_exploreOcean(1))
		{
			return;
		}

		if (AI_exploreDeep())
		{
			return;
		}
	}
	else
	{
		if (AI_exploreCoast(2))
		{
			return;
		}

		if (AI_exploreOcean(1))
		{
			return;
		}

		if (AI_exploreDeep())
		{
			return;
		}
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}
//End TAC Whaling, ray


// Erik: Determine whether a non-free unit is more expensive
// than the default/unrestricted free unit
int CvUnitAI::AI_getCostDifferenceFreeVsSlave() const
{
	int iEuropeMin = INT_MAX;
	int iAfricaMin = INT_MAX;

	const CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());

	for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		const UnitTypes eLoopUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI)));

		if (eLoopUnit != NO_UNIT)
		{
			const CvUnitInfo& kUnitInfo = GC.getUnitInfo(eLoopUnit);

			// Find the cheapeast european unit that cannot escape
			if (kUnitInfo.getDefaultUnitAIType() == UNITAI_COLONIST && kUnitInfo.getEuropeCost() > 0 && !kUnitInfo.LbD_canEscape())
			{
				if (kOwner.getEuropeUnitBuyPrice(eLoopUnit) < iEuropeMin)
				{
					iEuropeMin = kOwner.getEuropeUnitBuyPrice(eLoopUnit);
				}
			}

			// Find the cheapest african unit that can escape
			if (kUnitInfo.getDefaultUnitAIType() == UNITAI_COLONIST && kUnitInfo.getAfricaCost() > 0 && kUnitInfo.LbD_canEscape())
			{
				if (kOwner.getAfricaUnitBuyPrice(eLoopUnit) < iAfricaMin)
				{
					iAfricaMin = kOwner.getAfricaUnitBuyPrice(eLoopUnit);
				}
			}
		}
	}

	// We must have found units matching our requirements
	FAssert(iEuropeMin < INT_MAX);
	FAssert(iAfricaMin < INT_MAX);

	return iEuropeMin - iAfricaMin;
}

/// <summary>Find the best port given the conditions</summary>
bool CvUnitAI::AI_sailToPreferredPort(bool bMove)
{
	if (!hasAnyUnitInCargo())
	{
		CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());

		// In general we should prefer Europe, but we will consider going to Africa if:
		// 1) We cannot fill our transport ship with more than half the capacity of colonists for the return journey, rounded down
		//const bool bAfricaRatio = (kOwner.getNumEuropeUnits() / static_cast<double>(cargoSpace()) <= 0.5);

		// 2) Our goods must have a higher value in Africa than in Europe
		const int iAfricaBetterValue = getCargoValue(AFRICA) - getCargoValue(EUROPE);

		// 3) The price of a slave must be less than a free colonist
		const int iPriceDifference = AI_getCostDifferenceFreeVsSlave();

		// We should expand on this in a future iteration but this will suffice for now
		//if (bAfricaRatio && bAfricaBetterValue && iPriceDifference > 0)
		if (iAfricaBetterValue > 0 && iPriceDifference > 0)
		{
			return AI_sailToAfrica(bMove);
		}
		// Pick Africa if much better value
		else if (iAfricaBetterValue >= 100 * cargoSpace())
		{
			return AI_sailToAfrica(bMove);
		}
		else
		{
			return AI_sailToEurope(bMove);
		}
	}
	else
	{
		// We cannot sail to a port given these conditions
		return false;
	}
}

void CvUnitAI::AI_transportCoastMove()
{
	//const bool bEmpty = !getGroup()->hasCargo();

	if (AI_breakAutomation())
	{
		return;
	}

	// Erik: Colonist transportation has not been implemented yet
	// TODO: Determine a suitable range, maybe 2 full moves ?
	/*
	if (AI_getUnitAIState() == UNITAI_STATE_PICKUP)
	{
		if (AI_respondToPickup(10))
		{
			AI_setUnitAIState(UNITAI_STATE_PICKUP);
			return;
		}

		if (AI_deliverUnits())
		{
			return;
		}

		if (bEmpty)
		{
			AI_setUnitAIState(UNITAI_STATE_DEFAULT);
		}
	}
	*/
	if (getGroup()->AI_tradeRoutes())
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitAI::AI_transportSeaMove()
{
	PROFILE_FUNC();

	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	bool bEmpty = !getGroup()->hasCargo();

	// If we don't have any cities and
	// there are no settler in the new world, something may be very wrong.
	// Check if we have a settler waiting in Europe that we can use to
	// restart our fledgling empire
	if (kOwner.getNumCities() == 0)
	{
		// Note: Does not include units waiting in Europe,
		//
		const int iTotalSettlerCount = kOwner.AI_totalUnitAIs(UNITAI_SETTLER);
		if (iTotalSettlerCount > 0)
		{
			int iEuropeSettlerCount = 0;

			for (int i = 0; i < kOwner.getNumEuropeUnits(); ++i)
			{
				const CvUnit* pUnit = kOwner.getEuropeUnit(i);

				if (pUnit->AI_getUnitAIType() == UNITAI_SETTLER)
				{
					++iEuropeSettlerCount;
				}
			}

			if (iTotalSettlerCount == iEuropeSettlerCount)
			{
				// All our settlers are in Europe,
				// Dump all cargo (units) and head straight to Europe
				AI_sailToEurope(true);
				return;
			}
		}

	}

	// TAC - AI Improved Naval AI - koma13 - START
	if (AI_retreatFromDanger())
	{
		return;
	}
	// TAC - AI Improved Naval AI - koma13 - END

	CvCity* pCity = plot()->getPlotCity();

	if (pCity != NULL && AI_goodyRange(baseMoves(), /*bIgnoreCity*/true))
	{
		return;
	}


	if (kOwner.AI_totalUnitAIs(UNITAI_TREASURE) > 0)
	{
		if (AI_respondToPickup(20, UNITAI_TREASURE))
		{
			AI_setUnitAIState(UNITAI_STATE_SAIL);
			return;
		}
	}

	if (pCity != NULL)
	{
		if (AI_tradeWithCity())
		{
			return;
		}

		if (plot()->getOwner() == getOwner())
		{
			pCity = plot()->getPlotCity();
		}
	}

	// TAC - AI Improved Naval AI - koma13 - START
	bool bAtWar = GET_TEAM(getTeam()).getAnyWarPlanCount();
	int iExtra = (kOwner.AI_isStrategy(STRATEGY_REVOLUTION_PREPARING) && !bAtWar) ? cargoSpace() : 0;
	bool bPickupUnitsFromEurope = ((kOwner.AI_cargoSpaceToEurope(getGroup()) + iExtra) < kOwner.getNumEuropeUnits());
	// TAC - AI Improved Naval AI - koma13 - END

	int iGoodsCount = 0;
	UnitAIStates eStartingState = AI_getUnitAIState();
	if (AI_getUnitAIState() == UNITAI_STATE_SAIL)
	{
		if (AI_deliverUnits(UNITAI_SETTLER))
		{
			return;
		}

		if (AI_deliverUnits(UNITAI_COLONIST))
		{
			return;
		}

		if (AI_deliverUnits())
		{
			return;
		}

		if (AI_continueMission(-1, MISSIONAI_SAIL_TO_EUROPE, MOVE_BUST_FOG))
		{
			return;
		}

		if (AI_continueMission(-1, MISSIONAI_SAIL_TO_AFRICA, MOVE_BUST_FOG))
		{
			return;
		}

		if (bPickupUnitsFromEurope || hasCargo())	// TAC - AI Improved Naval AI - koma13
		{
			if (AI_sailToPreferredPort(false))
			{
				return;
			}
		}


		if (kOwner.AI_totalUnitAIs(UNITAI_TREASURE) > 0)
		{
			if (AI_respondToPickup(2, UNITAI_TREASURE))
			{
				return;
			}
		}

		// If we happen to be in a city we own, attempt to load as many goods as possible
		if (pCity != NULL && pCity->getOwner() == getOwner())
		{
			AI_collectGoods();
		}

		if (kOwner.AI_totalUnitAIs(UNITAI_TREASURE) > 0)
		{
			if (AI_respondToPickup(10, UNITAI_TREASURE))
			{
				return;
			}
		}

		if (isFull())
		{
			if (AI_sailToPreferredPort(true))
			{
				return;
			}
		}
		// Else we try to pick up more goods
		else
		{
			// Custom_House_Mod Start
			if (AI_travelToPort(15, 6))
			{
				return;
			}
			// Custom_House_Mod End
		}

		// This is the same condition as earlier, but
		// at this point we may have gained cargo that we should sell

		// TAC - AI Improved Naval AI - koma13 - START
		//if (hasCargo() || (kOwner.getNumEuropeUnits() > 0))
		if (hasCargo() || bPickupUnitsFromEurope)
			// TAC - AI Improved Naval AI - koma13 - END
		{
			if (AI_sailToPreferredPort(true))
			{
				return;
			}
		}

		AI_setUnitAIState(UNITAI_STATE_DEFAULT);
	}

	if (AI_deliverUnits())
		return;

	if (AI_getUnitAIState() == UNITAI_STATE_PICKUP)
	{
		if (AI_respondToPickup(2))
		{
			return;
		}

		if (AI_deliverUnits())
		{
			return;
		}

		if (bEmpty)
		{
			AI_setUnitAIState(UNITAI_STATE_DEFAULT);
		}
	}

	if (bEmpty)
	{
		if (AI_respondToPickup(5))
		{
			AI_setUnitAIState(UNITAI_STATE_PICKUP);
			return;
		}
	}

	int iNativeSaleGoods = 0;
	int iSettlerCount = 0;
	if (!bEmpty)
	{
		CLLNode<IDInfo>* pUnitNode;
		CvUnit* pLoopUnit;
		CvPlot* pPlot;
		int iCount;

		iCount = 0;

		pPlot = plot();

		pUnitNode = pPlot->headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = pPlot->getUnitNodeLoop(pUnitNode);

			if (pLoopUnit != NULL && pLoopUnit->getTransportUnit() == this)
			{
				if (pLoopUnit->getYield() != NO_YIELD)
				{
					FAssert(pLoopUnit->getYieldStored() > 0);
					iGoodsCount++;
					if (pLoopUnit->AI_getUnitAIState() == UNITAI_STATE_SELL_TO_NATIVES)
					{
						iNativeSaleGoods++;
					}
				}
				if (pLoopUnit->AI_getUnitAIType() == UNITAI_SETTLER)
				{
					iSettlerCount++;
				}
			}
		}
	}

	if (kOwner.AI_totalUnitAIs(UNITAI_TREASURE) > 0)
	{
		if (AI_respondToPickup(MAX_INT, UNITAI_TREASURE))
		{
			AI_setUnitAIState(UNITAI_STATE_SAIL);
			return;
		}

	}

	if (bEmpty && GC.getGameINLINE().getSorenRandNum(100, "AI Respond to Pickup 1") < 25)
	{
		if (AI_respondToPickup())
		{
			AI_setUnitAIState(UNITAI_STATE_PICKUP);
			return;
		}
	}

	bool bRoutes = (AI_getUnitAIType() == UNITAI_TRANSPORT_SEA);
	if (iNativeSaleGoods == 0 && iGoodsCount > 0)
	{
		if (getGroup()->AI_tradeRoutes())
		{
			return;
		}
		bRoutes = false;
	}

	// TAC - AI Improved Naval AI - koma13 - START
	//if (iSettlerCount > 0 || iNativeSaleGoods > 0 || ((kOwner.getNumEuropeUnits() == 0) && (kOwner.AI_countYieldWaiting() < 3)))
	if (iSettlerCount > 0 || iNativeSaleGoods > 0 || (!bPickupUnitsFromEurope && (kOwner.AI_countYieldWaiting() < 3)))
	// TAC - AI Improved Naval AI - koma13 - END
	{
		if (AI_exploreCoast(2))
		{
			return;
		}
		if (AI_exploreOcean(1))
		{
			return;
		}
		if (AI_exploreDeep())
		{
			return;
		}
	}

	int iCargoWaiting = kOwner.AI_countYieldWaiting();

	// TAC - AI Improved Naval AI - koma13 - START
	//if (iCargoWaiting < (cargoSpace() * GC.getGameINLINE().getCargoYieldCapacity()))
	if (iCargoWaiting < cargoSpace())
	// TAC - AI Improved Naval AI - koma13 - END
	{
		if (bRoutes)
		{
			if (AI_isObsoleteTradeShip())
			{
				if (getGroup()->AI_tradeRoutes())
				{
					return;
				}
				bRoutes = false;
			}
		}
	}

	if (pCity != NULL)
	{
		if (AI_collectGoods())
		{
			AI_setUnitAIState(UNITAI_STATE_SAIL);
			return;
		}
	}

	if (!hasAnyUnitInCargo() && !isFull())
	{
		if (AI_travelToPort(40))
		{
			AI_setUnitAIState(UNITAI_STATE_SAIL);
			return;
		}
	}

	if (iNativeSaleGoods > 0)
	{
		if (AI_exploreCoast(2))
		{
			return;
		}
		if (AI_exploreDeep())
		{
			return;
		}
	}

	// TAC - AI Improved Naval AI - koma13 - START
	//if ((GET_PLAYER(getOwnerINLINE()).getNumEuropeUnits() > 0) || isFull())
	if (bPickupUnitsFromEurope || isFull())
	// TAC - AI Improved Naval AI - koma13 - END
	{
		FAssert(AI_getUnitAIState() != UNITAI_STATE_SAIL);
		AI_setUnitAIState(UNITAI_STATE_SAIL);
		return;
	}

	CvArea* pArea = area();
	if (!pArea->isWater())
	{
		pArea = plot()->waterArea();
	}
	FAssert(pArea != NULL);
	if (pArea != NULL && pArea->getNumTiles() - pArea->getNumRevealedTiles(getTeam()) > 0)
	{
		if (AI_exploreCoast(2))
		{
			return;
		}
		if (AI_exploreDeep())
		{
			return;
		}
	}

	if (bRoutes)
	{
		if (getGroup()->AI_tradeRoutes())
		{
			return;
		}
	}

	if (AI_retreatToCity(true))
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

// Attempt to determine if a great admiral is waiting in a city
// if this is the case, we travel there
bool CvUnitAI::AI_joinGreatAdmiral()
{
	const CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());

	int iLoop;
	// TODO: replace by group!
	for (CvUnit* pLoopUnit = kOwner.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kOwner.nextUnit(&iLoop))
	{
		if (pLoopUnit->getUnitType() == kOwner.getUnitType(UNITCLASS_GREAT_ADMIRAL) ||
			pLoopUnit->getUnitType() == kOwner.getUnitType(UNITCLASS_CAPABLE_CAPTAIN))
		{
			CvPlot* pPlot = pLoopUnit->plot();
			if (pPlot == NULL || !pPlot->isCity())
				continue;

			if (kOwner.AI_isAnyPlotTargetMissionAI(*pPlot, MISSIONAI_LEAD, getGroup()))
				return false;

			if (!atPlot(pPlot))
			{
				// TODO: If there are multiple cities available, choose the closes (best) one
				//FAssert(!atPlot(pBestPlot));		
				pushGroupMoveTo(*pPlot, static_cast<MovementFlags>(MOVE_IGNORE_DANGER), false, false,
					MISSIONAI_LEAD, pPlot);
				return true;
			}
			else
			{
				getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_LEAD, pPlot, pLoopUnit);
				return true;
			}
		}
	}

	return false;
}

void CvUnitAI::AI_assaultSeaMove()
{
	PROFILE_FUNC();

	FAssert(AI_getUnitAIType() == UNITAI_ASSAULT_SEA);

	bool bEmpty = !getGroup()->hasCargo();
	bool bFull = (getGroup()->AI_isFull() && (getGroup()->getCargo() > 0));

	if (plot()->isCity(true))
	{
		int iOurDefense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(),0,true,false,true);
		int iEnemyOffense = GET_PLAYER(getOwnerINLINE()).AI_getEnemyPlotStrength(plot(),2,false,false);

		if( getDamage() > 0 )	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if( iEnemyOffense > iOurDefense/8 || iOurDefense == 0 )
		{
			if( iEnemyOffense > iOurDefense/4 || iOurDefense == 0 ) //prioritize getting outta there
			{
				if( !bEmpty )
				{
					getGroup()->unloadAll();
				}

				if (AI_anyAttack(1, 65))
				{
					return;
				}

				// Retreat to primary area first
				if (AI_retreatToCity(true))
				{
					return;
				}

				if (AI_retreatToCity())
				{
					return;
				}

				if (AI_safety())
				{
					return;
				}
			}

			if( !bFull && !bEmpty )
			{
				getGroup()->unloadAll();
				getGroup()->pushMission(MISSION_SKIP);
				return;
			}
		}
	}

	if (bEmpty)
	{
		if (AI_anyAttack(1, 65))
		{
			return;
		}
		if (AI_anyAttack(1, 45))
		{
			return;
		}
	}

	bool bReinforce = false;
	bool bAttack = false;
	bool bNoWarPlans = (GET_TEAM(getTeam()).getAnyWarPlanCount() == 0);
	bool bAttackNatives = false;
	bool bLandWar = false;
	bool bIsNative = isNative();

	// Count forts as cities
	bool bIsCity = plot()->isCity(true);

	// Cargo if already at war
	int iTargetReinforcementSize = (bIsNative ? AI_stackOfDoomExtra() : 2);

	// Cargo to launch a new invasion
	int iTargetInvasionSize = 2*iTargetReinforcementSize;

	int iCargo = getGroup()->getCargo();
	int iEscorts = getGroup()->countNumUnitAIType(UNITAI_ESCORT_SEA) + getGroup()->countNumUnitAIType(UNITAI_COMBAT_SEA);

	AreaAITypes eAreaAIType = area()->getAreaAIType(getTeam());
	bLandWar = !bIsNative && ((eAreaAIType == AREAAI_OFFENSIVE) || (eAreaAIType == AREAAI_BALANCED) || (eAreaAIType == AREAAI_DEFENSIVE) || (eAreaAIType == AREAAI_MASSING));

	// Plot danger case handled above

	if( hasCargo() && (getUnitAICargo(UNITAI_SETTLER) > 0 || getUnitAICargo(UNITAI_WORKER) > 0 || getUnitAICargo(UNITAI_COLONIST) > 0) )
	{
		// Dump inappropriate load at first oppurtunity after pick up
		if( bIsCity && (plot()->getOwnerINLINE() == getOwnerINLINE()) )
		{
			getGroup()->unloadAll();
			getGroup()->pushMission(MISSION_SKIP);
			return;
		}
		else
		{
			if( !isFull() )
			{
				if(AI_respondToPickup(1, NO_UNITAI))
				{
					return;
				}
			}

			if (AI_retreatToCity(true))
			{
				return;
			}

			if (AI_retreatToCity())
			{
				return;
			}
		}
	}

	if (bIsCity)
	{
		CvCity* pCity = plot()->getPlotCity();

		if( (iCargo > 0) )
		{
			if( pCity != NULL )
			{
				if( (GC.getGameINLINE().getGameTurn() - pCity->getGameTurnAcquired()) <= 1 )
				{
					if( pCity->getPreviousOwner() != NO_PLAYER )
					{
						// Just captured city, probably from naval invasion.  If area targets, drop cargo and leave so as to not to be lost in quick counter attack
						if( GET_TEAM(getTeam()).AI_countEnemyPowerByArea(*plot()->area()) > 0 )
						{
							getGroup()->unloadAll();

							if( iEscorts > 2 )
							{
								if( getGroup()->countNumUnitAIType(UNITAI_ESCORT_SEA) > 1 && getGroup()->countNumUnitAIType(UNITAI_COMBAT_SEA) > 0 )
								{
									getGroup()->AI_seperateAI(UNITAI_COMBAT_SEA);
									iEscorts = getGroup()->countNumUnitAIType(UNITAI_ESCORT_SEA);
								}
							}

							iCargo = getGroup()->getCargo();
						}
					}
				}
			}
		}

		if( (iCargo > 0) && (iEscorts == 0) )
		{
			if (AI_group(UNITAI_ASSAULT_SEA,-1,-1,-1,/*bIgnoreFaster*/true,false,false,/*iMaxPath*/1,false,/*bCargoOnly*/true,false,MISSIONAI_ASSAULT))
			{
				return;
			}

			if( plot()->plotCount(PUF_isUnitAIType, UNITAI_ESCORT_SEA, -1, getOwnerINLINE(), NO_TEAM, PUF_isGroupHead, -1, -1) > 0 )
			{
				// Loaded but with no escort, wait for escorts in plot to join us
				getGroup()->pushMission(MISSION_SKIP);
				return;
			}

			MissionAITypes eMissionAIType = MISSIONAI_GROUP;
			if( (GET_PLAYER(getOwnerINLINE()).AI_unitTargetMissionAIs(this, &eMissionAIType, 1, getGroup(), 3) > 0) || (GET_PLAYER(getOwnerINLINE()).AI_getWaterDanger(plot(), 4, false) > 0) )
			{
				// Loaded but with no escort, wait for others joining us soon or avoid dangerous waters
				getGroup()->pushMission(MISSION_SKIP);
				return;
			}
		}

		if (bLandWar)
		{
			if ( iCargo > 0 )
			{
				if( (eAreaAIType == AREAAI_DEFENSIVE) || (pCity != NULL && pCity->AI_isDanger()))
				{
					// Unload cargo when on defense or if small load of troops and can reach enemy city over land (generally less risky)
					getGroup()->unloadAll();
					getGroup()->pushMission(MISSION_SKIP);
					return;
				}
			}

			if ((iCargo >= iTargetReinforcementSize))
			{
				getGroup()->AI_separateEmptyTransports();

				if( !(getGroup()->hasCargo()) )
				{
					// this unit was empty group leader
					getGroup()->pushMission(MISSION_SKIP);
					return;
				}

				// Send ready transports
				if (AI_assaultSeaReinforce(false))
				{
					return;
				}

				if( iCargo >= iTargetInvasionSize )
				{
					if (AI_assaultSeaTransport(false))
					{
						return;
					}
				}
			}
		}
		else
		{
			if ( (eAreaAIType == AREAAI_ASSAULT) )
			{
				if( iCargo >= iTargetInvasionSize )
				{
					bAttack = true;
				}
			}

			if(eAreaAIType == AREAAI_ASSAULT)
			{
				if( (bFull && iCargo > cargoSpace()) || (iCargo >= iTargetReinforcementSize) )
				{
					bReinforce = true;
				}
			}
		}

		if( !bAttack && !bReinforce && (plot()->getTeam() == getTeam()) )
		{
			if( iEscorts > 3 && iEscorts > (2*getGroup()->countNumUnitAIType(UNITAI_ASSAULT_SEA)) )
			{
				// If we have too many escorts, try freeing some for others
				getGroup()->AI_seperateAI(UNITAI_COMBAT_SEA);

				iEscorts = getGroup()->countNumUnitAIType(UNITAI_ESCORT_SEA);
				if( iEscorts > 3 && iEscorts > (2*getGroup()->countNumUnitAIType(UNITAI_ASSAULT_SEA)) )
				{
					getGroup()->AI_seperateAI(UNITAI_ESCORT_SEA);
				}
			}
		}

		MissionAITypes eMissionAIType = MISSIONAI_GROUP;
		if( GET_PLAYER(getOwnerINLINE()).AI_unitTargetMissionAIs(this, &eMissionAIType, 1, getGroup(), 1) > 0 )
		{
			// Wait for units which are joining our group this turn
			getGroup()->pushMission(MISSION_SKIP);
			return;
		}

		if( !bFull )
		{
			if( bAttack )
			{
				eMissionAIType = MISSIONAI_LOAD_ASSAULT;
				if( GET_PLAYER(getOwnerINLINE()).AI_unitTargetMissionAIs(this, &eMissionAIType, 1, getGroup(), 1) > 0 )
				{
					// Wait for cargo which will load this turn
					getGroup()->pushMission(MISSION_SKIP);
					return;
				}
			}
			else if( GET_PLAYER(getOwnerINLINE()).AI_unitTargetMissionAIs(this, MISSIONAI_LOAD_ASSAULT) > 0 )
			{
				// Wait for cargo which is on the way
				getGroup()->pushMission(MISSION_SKIP);
				return;
			}
		}

		if( !bAttack && !bReinforce )
		{
			if ( iCargo > 0 )
			{
				if (AI_group(UNITAI_ASSAULT_SEA,-1,-1,-1,/*bIgnoreFaster*/true,false,false,/*iMaxPath*/5,false,/*bCargoOnly*/true,false,MISSIONAI_ASSAULT))
				{
					return;
				}
			}
			else if (plot()->getTeam() == getTeam() && getGroup()->getNumUnits() > 1)
			{
				CvCity* pCity = plot()->getPlotCity();
				if( pCity != NULL && (GC.getGameINLINE().getGameTurn() - pCity->getGameTurnAcquired()) > 10 )
				{
					if( pCity->plot()->plotCount(PUF_isAvailableUnitAITypeGroupie, UNITAI_OFFENSIVE, -1, getOwnerINLINE()) < iTargetReinforcementSize )
					{
						// Not attacking, no cargo so release any escorts, attack ships, etc and split transports
						getGroup()->AI_makeForceSeparate();
					}
				}
			}
		}
	}

	if (!bIsCity)
	{
		if( iCargo >= iTargetInvasionSize )
		{
			bAttack = true;
		}

		if ((iCargo >= iTargetReinforcementSize) || (bFull && iCargo > cargoSpace()))
		{
			bReinforce = true;
		}

		CvPlot* pAdjacentPlot = NULL;
		for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));
			if( pAdjacentPlot != NULL )
			{
				if( iCargo > 0 )
				{
					CvCity* pAdjacentCity = pAdjacentPlot->getPlotCity();
					if( pAdjacentCity != NULL && pAdjacentCity->getOwner() == getOwnerINLINE() && pAdjacentCity->getPreviousOwner() != NO_PLAYER )
					{
						if( (GC.getGameINLINE().getGameTurn() - pAdjacentCity->getGameTurnAcquired()) < 5 )
						{
							// If just captured city and we have some cargo, dump units in city
							getGroup()->pushMission(MISSION_MOVE_TO, pAdjacentPlot->getX_INLINE(), pAdjacentPlot->getY_INLINE(), 0, false, false, MISSIONAI_ASSAULT, pAdjacentPlot);
							return;
						}
					}
				}
				else
				{
					if (pAdjacentPlot->isOwned() && isEnemy(pAdjacentPlot->getTeam()))
					{
						if( pAdjacentPlot->getNumDefenders(getOwnerINLINE()) > 2 )
						{
							// if we just made a dropoff in enemy territory, release sea bombard units to support invaders
							if (getGroup()->countNumUnitAIType(UNITAI_COMBAT_SEA) > 0)
							{
								bool bMissionPushed = false;

								if (AI_seaBombardRange(1))
								{
									bMissionPushed = true;
								}

								CvSelectionGroup* pOldGroup = getGroup();

								//Release any Warships to finish the job.
								getGroup()->AI_seperateAI(UNITAI_COMBAT_SEA);

								// Fixed bug in next line with checking unit type instead of unit AI
								if (pOldGroup == getGroup() && AI_getUnitAIType() == UNITAI_ASSAULT_SEA)
								{
									// Need to be sure all units can move
									if( getGroup()->canAllMove() )
									{
										if (AI_retreatToCity(true))
										{
											bMissionPushed = true;
										}
									}
								}

								if (bMissionPushed)
								{
									return;
								}
							}
						}
					}
				}
			}
		}

		if(iCargo > 0)
		{
			MissionAITypes eMissionAIType = MISSIONAI_GROUP;
			if( GET_PLAYER(getOwnerINLINE()).AI_unitTargetMissionAIs(this, &eMissionAIType, 1, getGroup(), 1) > 0 )
			{
				if( iEscorts < GET_PLAYER(getOwnerINLINE()).AI_getWaterDanger(plot(), 2, false) )
				{
					// Wait for units which are joining our group this turn (hopefully escorts)
					getGroup()->pushMission(MISSION_SKIP);
					return;
				}
			}
		}
	}

	if (bIsNative)
	{
		if (getGroup()->isFull() || iCargo > iTargetInvasionSize)
		{
			if (AI_assaultSeaTransport(false))
			{
				return;
			}
		}
		else
		{
			if (AI_pickup(UNITAI_OFFENSIVE, 5))
			{
				return;
			}

			if (AI_retreatToCity())
			{
				return;
			}

			if( AI_safety() )
			{
				return;
			}

			getGroup()->pushMission(MISSION_SKIP);
			return;
		}
	}
	else
	{
		if (bAttack || bReinforce)
		{
			if( bIsCity )
			{
				getGroup()->AI_separateEmptyTransports();
			}

			if( !(getGroup()->hasCargo()) )
			{
				// this unit was empty group leader
				getGroup()->pushMission(MISSION_SKIP);
				return;
			}

			FAssert(getGroup()->hasCargo());

			//BBAI TODO: Check that group has escorts, otherwise usually wait

			if( bAttack )
			{
				if( bReinforce && (AI_getBirthmark()%2 == 0) )
				{
					if (AI_assaultSeaReinforce())
					{
						return;
					}
					bReinforce = false;
				}

				if (AI_assaultSeaTransport())
				{
					return;
				}
			}

			// If not enough troops for own invasion,
			if( bReinforce )
			{
				if (AI_assaultSeaReinforce())
				{
					return;
				}
			}
		}

		if( bNoWarPlans && (iCargo >= iTargetReinforcementSize) )
		{
			bAttackNatives = true;

			getGroup()->AI_separateEmptyTransports();

			if( !(getGroup()->hasCargo()) )
			{
				// this unit was empty group leader
				getGroup()->pushMission(MISSION_SKIP);
				return;
			}

			FAssert(getGroup()->hasCargo());
			if (AI_assaultSeaReinforce(bAttackNatives))
			{
				return;
			}

			FAssert(getGroup()->hasCargo());
			if (AI_assaultSeaTransport(bAttackNatives))
			{
				return;
			}
		}
	}

	if ((bFull || bReinforce) && !bAttack)
	{
		// Group with nearby transports with units on board
		if (AI_group(UNITAI_ASSAULT_SEA, -1, /*iMaxOwnUnitAI*/ -1, -1, true, false, false, 2, false, true, false, MISSIONAI_ASSAULT))
		{
			return;
		}

		if (AI_group(UNITAI_ASSAULT_SEA, -1, -1, -1, true, false, false, 10, false, true, false, MISSIONAI_ASSAULT))
		{
			return;
		}
	}
	else if( !bFull )
	{
		bool bHasOneLoad = (getGroup()->getCargo() >= cargoSpace());
		bool bHasCargo = getGroup()->hasCargo();

		if (AI_pickup(UNITAI_OFFENSIVE, (bHasOneLoad ? 3 : 7)))
		{
			return;
		}

		if (AI_pickup(UNITAI_COUNTER, (bHasOneLoad ? 3 : 7)))
		{
			return;
		}

		if( !bHasCargo )
		{
			if(AI_respondToPickup(MAX_INT, UNITAI_OFFENSIVE))
			{
				return;
			}

			if(AI_respondToPickup(MAX_INT, UNITAI_COUNTER))
			{
				return;
			}

			if( (getGroup()->countNumUnitAIType(AI_getUnitAIType()) == 1) )
			{
				// Try picking up any thing
				if(AI_respondToPickup(MAX_INT))
				{
					return;
				}
			}
		}
	}

	if (bIsCity && bLandWar && getGroup()->hasCargo())
	{
		// Enemy units in this player's territory
		if( GET_PLAYER(getOwnerINLINE()).AI_countNumAreaHostileUnits(area(),true,false,false,false) > (getGroup()->getCargo()/2))
		{
			getGroup()->unloadAll();
			getGroup()->pushMission(MISSION_SKIP);
			return;
		}
	}

	if (AI_retreatToCity(true))
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}
// TAC - AI Assault Sea - koma13, jdog5000(BBAI) - END

// TAC - AI Escort Sea - koma13, jdog5000(BBAI) - START
void CvUnitAI::AI_escortSeaMove()
{
	PROFILE_FUNC();

	if (AI_goodyRange(baseMoves(), /*bIgnoreCity*/true))
	{
		return;
	}

	if (plot()->isCity(true)) //prioritize getting outta there
	{
		int iOurDefense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(),0,true,false,true);
		int iEnemyOffense = GET_PLAYER(getOwnerINLINE()).AI_getEnemyPlotStrength(plot(),2,false,false);

		if( getDamage() > 0 )	// extra risk to leaving when wounded
		{
			iOurDefense *= 2;
		}

		if( iEnemyOffense > iOurDefense/4  || iOurDefense == 0)
		{
			if (AI_anyAttack(1, 60))
			{
				return;
			}

			if (AI_group(UNITAI_ASSAULT_SEA, -1, /*iMaxOwnUnitAI*/ 1, -1, /*bIgnoreFaster*/ true, false, false, /*iMaxPath*/ baseMoves()))
			{
				return;
			}

			if (AI_retreatToCity())
			{
				return;
			}

			if (AI_safety())
			{
				return;
			}
		}
	}

	if (AI_anyAttack(1, 55))
	{
		return;
	}

	if (AI_group(UNITAI_ASSAULT_SEA, -1, /*iMaxOwnUnitAI*/ 0, -1, /*bIgnoreFaster*/ true, false, false, /*iMaxPath*/ 3))
	{
		return;
	}

	if (AI_group(UNITAI_ASSAULT_SEA, 1, /*iMaxOwnUnitAI*/ 0, /*iMinUnitAI*/ -1, /*bIgnoreFaster*/ true))
	{
		return;
	}

	if (AI_group(UNITAI_ASSAULT_SEA, -1, /*iMaxOwnUnitAI*/ 2, /*iMinUnitAI*/ -1, /*bIgnoreFaster*/ true))
	{
		return;
	}

	// Group only with large flotillas first
	if (AI_group(UNITAI_ASSAULT_SEA, -1, /*iMaxOwnUnitAI*/ 4, /*iMinUnitAI*/ 3, /*bIgnoreFaster*/ true))
	{
		return;
	}

	if (isHurt())
	{
		if (AI_heal())
		{
			return;
		}

		if (AI_retreatToCity())
		{
			return;
		}
	}

	// If nothing else useful to do, escort nearby large flotillas even if they're faster
	// Gives Caravel escorts something to do during the Galleon/pre-Frigate era
	if (AI_group(UNITAI_ASSAULT_SEA, -1, /*iMaxOwnUnitAI*/ 4, /*iMinUnitAI*/ 3, /*bIgnoreFaster*/ false, false, false, 4, false, true))
	{
		return;
	}

	if (AI_group(UNITAI_ASSAULT_SEA, -1, /*iMaxOwnUnitAI*/ 2, /*iMinUnitAI*/ -1, /*bIgnoreFaster*/ false, false, false, 1, false, true))
	{
		return;
	}

	// Pull back to primary area if it's not too far so primary area cities know you exist
	// and don't build more, unnecessary escorts
	if (AI_retreatToCity(true, 6))
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

// TAC - AI Escort Sea - koma13, jdog5000(BBAI) - END

void CvUnitAI::AI_combatSeaMove()
{
	if (AI_goodyRange(baseMoves(), /*bIgnoreCity*/true))
	{
		return;
	}

	// Erik: Check if there's an admiral waiting
	if (AI_joinGreatAdmiral())
		return;

	// TAC - AI Improved Naval AI - koma13 - START
	//if (AI_anyAttack(2, 49))
	if (AI_anyAttack(1, 49))
	// TAC - AI Improved Naval AI - koma13 - END
	{
		return;
	}

	if (isHurt())
	{
		if (AI_heal())
		{
			return;
		}

		if (AI_retreatToCity())
		{
			return;
		}
	}

	// TAC - AI Improved Naval AI - koma13 - START
	if (AI_moveTowardsDanger())
	{
		return;
	}
	// TAC - AI Improved Naval AI - koma13 - END

	if (plot()->getDistanceToOcean() == 0)
	{
		if (AI_moveTowardsVictimCity())
		{
			return;
		}
	}

	if (AI_moveTowardsOcean(1))
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitAI::AI_pirateMove()
{
	if (AI_anyAttack(2, 49))
	{
		return;
	}

	if (isHurt())
	{
		if (AI_heal())
		{
			return;
		}

		if (AI_retreatToCity())
		{
			return;
		}
	}

	if (AI_shouldRun())
	{
		AI_setUnitAIState(UNITAI_STATE_RETREATING);
	}

	if (AI_getUnitAIState() == UNITAI_STATE_RETREATING)
	{
		if (AI_retreatToCity())
		{
			if (plot()->isCity())
			{
				AI_setUnitAIState(UNITAI_STATE_DEFAULT);
				getGroup()->pushMission(MISSION_SKIP);
				return;
			}
			return;
		}
	}

	if (AI_continueMission(0, MISSIONAI_PIRACY, 0))
	{
		return;
	}

	if (plot()->getDistanceToOcean() == 0)
	{
		if (AI_moveTowardsVictimCity())
		{
			return;
		}
	}

	if (AI_exploreCoast(2))
	{
		return;
	}

	if (AI_exploreOcean(1))
	{
		return;
	}

	if (AI_exploreDeep())
	{
		return;
	}

	if (AI_moveTowardsOcean(1))
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}




void CvUnitAI::AI_exploreMove()
{
	PROFILE_FUNC();

	if (AI_breakAutomation())
	{
		return;
	}

	if (!isHuman() && canAttack())
	{
		if (AI_cityAttack(1, 60))
		{
			return;
		}

		if (AI_anyAttack(1, 70))
		{
			return;
		}
	}

	if (getDamage() > 0)
	{
		// R&R, Robert Surcouf, Damage on Storm plots, Start
		//getGroup()->pushMission(MISSION_HEAL);
		//return;
		if ((plot()->getFeatureType() == NO_FEATURE) || (GC.getFeatureInfo(plot()->getFeatureType()).getTurnDamage() == 0))
		{
			getGroup()->pushMission(MISSION_HEAL);
			return;
		}
		// R&R, Robert Surcouf, Damage on Storm plots, End
	}

	if (AI_goody())
	{
		return;
	}

	if (AI_exploreRange(3))
	{
		return;
	}

	if (AI_patrol())
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitAI::AI_exploreSeaMove()
{
	PROFILE_FUNC();

	if (AI_breakAutomation())
	{
		return;
	}

	if (AI_goodyRange(baseMoves(), /*bIgnoreCity*/true))
	{
		return;
	}

	if (AI_exploreCoast(2))
	{
		return;
	}

	if (AI_exploreOcean(1))
	{
		return;
	}

	if (AI_exploreDeep())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

void CvUnitAI::AI_networkAutomated()
{
	FAssertMsg(canBuildRoute(), "canBuildRoute is expected to be true");

	if (!(getGroup()->canDefend()))
	{
		if (GET_PLAYER(getOwnerINLINE()).AI_getPlotDanger(plot()) > 0)
		{
			if (AI_retreatToCity()) // XXX maybe not do this??? could be working productively somewhere else...
			{
				return;
			}
		}
	}

	if (AI_routeTerritory(true))
	{
		return;
	}

	if (AI_routeCity())
	{
		return;
	}

	if (AI_routeTerritory())
	{
		return;
	}

	if (AI_retreatToCity())
	{
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


void CvUnitAI::AI_cityAutomated()
{
	CvCity* pCity;

	if (!(getGroup()->canDefend()))
	{
		if (GET_PLAYER(getOwnerINLINE()).AI_getPlotDanger(plot()) > 0)
		{
			if (AI_retreatToCity()) // XXX maybe not do this??? could be working productively somewhere else...
			{
				return;
			}
		}
	}

	pCity = NULL;

	if (plot()->getOwnerINLINE() == getOwnerINLINE())
	{
		pCity = plot()->getWorkingCity();
	}

	if (pCity == NULL)
	{
		pCity = GC.getMap().findCity(getX_INLINE(), getY_INLINE(), getOwnerINLINE()); // XXX do team???
	}

	if (pCity != NULL)
	{
		if (AI_improveCity(pCity))
		{
			return;
		}
	}

	if (AI_retreatToCity())
	{
		if (canJoinCity(plot()))
		{
			joinCity();
		}
		return;
	}

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}

// R&R, ray, Natives raiding party - START
bool CvUnitAI::AI_travelToEuropeColonyRaidTarget(int iMinAttitude, int iMaxAttitude, int iRange, int iFlags)
{
	int iX, iY;
	if (iRange == -1)
	{
		iRange = 5;
	}

	CvCity* pTargetCity = NULL;
	CvPlot* pBestPlot = NULL;
	int iBestValue = 0;

	CvPlayer& kOwner = GET_PLAYER(getOwner());

	for (iX = -iRange; iX <= iRange; iX++)
	{
		for (iY = -iRange; iY <= iRange; iY++)
		{
			//Should this always be centered on the home city?
			CvPlot* pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iX, iY);
			if (NULL != pLoopPlot)
			{
				if (pLoopPlot->isCity() && !atPlot(pLoopPlot))
				{
					CvPlayer& kLoopPlayer = GET_PLAYER(pLoopPlot->getOwner());
					if (!kLoopPlayer.isNative())
					{
						int iAttitude = kOwner.AI_getAttitude(kLoopPlayer.getID());
						if (iAttitude >= iMinAttitude && iAttitude <= iMaxAttitude)
						{
							int iPathTurns;
							if (generatePath(pLoopPlot, 0, true, &iPathTurns))
							{
								int iValue = 5000;
								iValue += GC.getGame().getSorenRandNum(10000, "AI target colony");
								iValue /= 3 + iPathTurns;

								if (iValue > iBestValue)
								{
									iBestValue = iValue;
									pBestPlot = pLoopPlot;

									if (iPathTurns == 1)
									{
										pTargetCity = pBestPlot->getPlotCity();
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		bool bIsDefended = false;

		if (pTargetCity != NULL)
		{
			if (pTargetCity->AI_numDefenders(true, true) > 0)
			{
				bIsDefended = true;
			}
		}

		FAssert(!atPlot(pBestPlot));
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), (bIsDefended ? iFlags : 0));
		return true;
	}

	return false;
}
// R&R, ray, Natives raiding party - END

bool CvUnitAI::AI_travelToEuropeColony(int iMinAttitude, int iMaxAttitude, int iRange)
{
	int iX, iY;
	if (iRange == -1)
	{
		iRange = 5;
	}

	CvPlot* pBestPlot = NULL;
	int iBestValue = 0;

	CvPlayer& kOwner = GET_PLAYER(getOwner());

	for (iX = -iRange; iX <= iRange; iX++)
	{
		for (iY = -iRange; iY <= iRange; iY++)
		{
			//Should this always be centered on the home city?
			CvPlot* pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iX, iY);

			if (NULL != pLoopPlot)
			{
				if (pLoopPlot->isCity())
				{
					CvPlayer& kLoopPlayer = GET_PLAYER(pLoopPlot->getOwner());

					if (!kLoopPlayer.isNative())
					{
						int iAttitude = kOwner.AI_getAttitude(kLoopPlayer.getID());
						if (iAttitude >= iMinAttitude && iAttitude <= iMaxAttitude)
						{
							int iPathTurns;

							if (generatePath(pLoopPlot, 0, true, &iPathTurns))
							{
								int iValue = 5000;
								iValue += GC.getGame().getSorenRandNum(10000, "AI target colony");

								iValue /= 3 + iPathTurns;

								if (iValue > iBestValue)
								{
									iBestValue = iValue;
									pBestPlot = pLoopPlot;
								}
							}
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		FAssert(!atPlot(pBestPlot));
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE());
		return true;
	}

	return false;
}

bool CvUnitAI::AI_guardHomeColony()
{
	if (getHomeCity() != NULL)
	{
		CvPlot* pHomePlot = getHomeCity()->plot();
		if (atPlot(pHomePlot))
		{
				getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_CITY, NULL);
				return true;
			}
			else
			{
				if (generatePath(pHomePlot, 0, true))
				{
					getGroup()->pushMission(MISSION_MOVE_TO, pHomePlot->getX_INLINE(), pHomePlot->getY_INLINE(), 0, false, false, MISSIONAI_GUARD_CITY, NULL);
					return true;
				}
			}
		}
		return false;
	}

bool CvUnitAI::AI_findNewHomeColony()
{
	int iBestValue = MAX_INT;
	CvCity* pBestCity = NULL;

	CvPlayer& kPlayer = GET_PLAYER(getOwnerINLINE());
	int iLoop;
	for (CvCity* pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
	{
		int iPathTurns;
		if (generatePath(pLoopCity->plot(), 0, true, &iPathTurns))
		{
			int iValue = iPathTurns * (25 + GC.getGameINLINE().getSorenRandNum(75, "AI Find New Home"));

			if (iValue < iBestValue)
			{
				iBestValue = iValue;
				pBestCity = pLoopCity;
			}
		}
	}

	if (pBestCity != NULL)
	{
		setHomeCity(pBestCity);
		if (generatePath(getHomeCity()->plot(), 0, true))
		{
			getGroup()->pushMission(MISSION_MOVE_TO, getHomeCity()->getX_INLINE(), getHomeCity()->getY_INLINE());
			return true;
		}
	}

	return false;
}

bool CvUnitAI::AI_europeBuyNativeYields()
{
	bool bPurchased = false;
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	CvPlayer& kPlayerEurope = GET_PLAYER(kOwner.getParent());


	std::vector<int> yields;

	yields.push_back(YIELD_MUSKETS);
	if (GC.getGameINLINE().getSorenRandNum(2, "AI buy horses") == 0)
	{
		yields.push_back(YIELD_HORSES);
	}

	GC.getGameINLINE().getSorenRand().shuffleArray(yields, NULL);

	for (uint i = 0; i < yields.size(); ++i)
	{
		YieldTypes eLoopYield = (YieldTypes)yields[i];
		if (kOwner.isYieldEuropeTradable(eLoopYield))
		{
			int iAmount = getLoadYieldAmount(eLoopYield);
			int iPrice = iAmount * kPlayerEurope.getYieldSellPrice(eLoopYield);

			int iAvailableGold = kOwner.getGold() - 50;

			if (iPrice > iAvailableGold)
			{
				iAmount = iAvailableGold / kPlayerEurope.getYieldSellPrice(eLoopYield);
			}
			if (iAmount > 0)
			{
				CvUnit* pYieldUnit = kOwner.buyYieldUnitFromEurope(eLoopYield, iAmount, this);
				if (pYieldUnit != NULL)
				{
					pYieldUnit->AI_setUnitAIState(UNITAI_STATE_SELL_TO_NATIVES);
				}
				bPurchased = true;
			}
		}
	}

	return bPurchased;
}



bool CvUnitAI::AI_europeBuyYields()
{
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	CvPlayer& kPlayerEurope = GET_PLAYER(kOwner.getParent());

	int aiYields[NUM_YIELD_TYPES];
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		aiYields[iYield] = 0;
	}

	int aiTotalYields[NUM_YIELD_TYPES];
	kOwner.calculateTotalYields(aiTotalYields);

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		YieldTypes eLoopYield = (YieldTypes)iYield;
		if (kOwner.AI_shouldBuyFromEurope(eLoopYield))
		{
			if (kOwner.isYieldEuropeTradable(eLoopYield))
			{
				int iLoop;
				CvCity* pLoopCity;
				for (pLoopCity = kOwner.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kOwner.nextCity(&iLoop))
				{
					int iNeeded = pLoopCity->getMaintainLevel(eLoopYield);
					iNeeded -= pLoopCity->getYieldStored(eLoopYield);
					if (iNeeded > 0)
					{
						aiYields[eLoopYield] += iNeeded;
					}
				}
			}
		}
	}

	std::vector<int> yieldRandomizer(NUM_YIELD_TYPES);
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		yieldRandomizer[iYield] = iYield;
	}

	GC.getGameINLINE().getSorenRand().shuffleArray(yieldRandomizer, NULL);

	for (int iPass = 0; iPass < 2; ++iPass)
	{
		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			YieldTypes eLoopYield = (YieldTypes)yieldRandomizer[iYield];
			if (aiYields[eLoopYield] > 0)
			{
				int iMax = getLoadYieldAmount(eLoopYield);
				int iMin = std::min(iMax, aiYields[eLoopYield]);


				int iMinPrice = iMin * kPlayerEurope.getYieldSellPrice(eLoopYield);
				int iMaxPrice = iMax * kPlayerEurope.getYieldSellPrice(eLoopYield);

				int iAmount = 0;
				int iAvailableGold = kOwner.getGold() - 50;

				if (iMinPrice > iAvailableGold)
				{
					return false;
				}
				if (iMaxPrice <= iAvailableGold)
				{
					iAmount = iMax;
				}
				else
				{
					iAmount = iMin;
				}
				if (iAmount > 0)
				{
					CvUnit* pYieldUnit = kOwner.buyYieldUnitFromEurope(eLoopYield, iAmount, this);
					if (pYieldUnit != NULL)
					{
						pYieldUnit->AI_setUnitAIState(UNITAI_STATE_PURCHASED);
						aiYields[eLoopYield] -= iAmount;
						// TAC - AI Economy - koma13 - START
						int iCost = iAmount * kPlayerEurope.getYieldSellPrice(eLoopYield);
						kOwner.AI_changeEuropeYieldSpending(iCost);
						// TAC - AI Economy - koma13 - END
					}
					if (isFull())
					{
						break;
					}
				}
			}
		}
	}

	return false;
}

bool CvUnitAI::AI_africa()
{
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());

	if (kOwner.getParent() == NO_PLAYER)
		return false;

	AI_sellYieldUnits(AFRICA);

	const int iEstimatedUnemploymentCount = kOwner.AI_estimateUnemploymentCount();

	if (iEstimatedUnemploymentCount < kOwner.getNumCities() * 2)
	{
		for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
		{
			UnitTypes eLoopUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI)));
			CvUnitInfo& kUnitInfo = GC.getUnitInfo(eLoopUnit);

			if (kUnitInfo.getDefaultUnitAIType() == UNITAI_COLONIST && kUnitInfo.getAfricaCost() > 0)
			{
				// Attempt to buy slaves as long as we have have enough gold and free cargo slots
				const int iPotentialSlavesToBuy = cargoSpace() - getCargo();

				for (int i=0; i < iPotentialSlavesToBuy; i++)
				{
					if (kOwner.buyAfricaUnit(eLoopUnit, 100) == NULL)
						// Early exit since we could not buy anymore
						break;
					logBBAI("CvUnitAI::AI_africa player %S hurries %d units since iEstimatedUnemploymentCount is %d",
						kOwner.getCivilizationDescription(), i, iEstimatedUnemploymentCount);
				}

				// No need to consider other units
				break;
			}
		}
	}

	//Pick up units from Africa (FIFO)
	std::deque<CvUnit*> aUnits;
	for (int i = 0; i < kOwner.getNumAfricaUnits(); ++i)
	{
		aUnits.push_back(kOwner.getAfricaUnit(i));
	}

	while (!aUnits.empty() && !isFull())
	{
		CvUnit* pUnit = aUnits.front();
		aUnits.pop_front();
		if (pUnit->canLoadUnit(this, plot(), false))
		{
			kOwner.loadUnitFromAfrica(pUnit, this);
			if (getGroup()->getAutomateType() == AUTOMATE_FULL)
			{
				FAssert(pUnit->getGroup() != NULL);
				pUnit->getGroup()->setAutomateType(AUTOMATE_FULL);
			}
		}
	}

	// R&R, ray, fix for inactive AI - START
	CvPlot* pStartingPlot = GET_PLAYER(getOwnerINLINE()).getStartingPlot();
	// R&R, ray, fix for inactive AI - END

	// TAC - AI Improved Naval AI - koma13 - START
	if (kOwner.getNumCities() > 0)
	{
		CvPlot* pNewPlot = AI_bestDestinationPlot();

		if (pNewPlot == NULL)
		{
			// R&R, ray, fix for inactive AI - START
			if (pStartingPlot != NULL)
			{
				pNewPlot = pStartingPlot;
			}

			// old code now in else
			else
			{
				getGroup()->pushMission(MISSION_SKIP);
				return true;
			}
			// R&R, ray, fix for inactive AI - END
		}

		if (plot() != pNewPlot)
		{
			setXY(pNewPlot->getX_INLINE(), pNewPlot->getY_INLINE());
		}
	}
	// TAC - AI Improved Naval AI - koma13 - END

	// R&R, ray, fix for inactive AI - START
	else if (plot() == NULL && pStartingPlot != NULL)
	{
		setXY(pStartingPlot->getX_INLINE(), pStartingPlot->getY_INLINE());
	}
	// R&R, ray, fix for inactive AI - END

	FAssert(plot()->isEurope());
	FAssert(canCrossOcean(plot(), UNIT_TRAVEL_STATE_FROM_AFRICA));

	crossOcean(UNIT_TRAVEL_STATE_FROM_AFRICA);
	finishMoves();
	return true;
}

bool CvUnitAI::AI_europe()
{
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());

	AI_sellYieldUnits(EUROPE);
	AI_unloadUnits(EUROPE);

	// TAC - AI King no Europe trading bugfix - koma13 - START
	//kOwner.AI_doEurope();
	if (kOwner.getParent() != NO_PLAYER)
	// TAC - AI King no Europe trading bugfix - koma13 - END
	{
		kOwner.AI_doEurope();
	}

	if (!kOwner.getNumCities() == 0)
	{	// only do this if we have not lost all our cities!
		// Otherwise we'll get our transport units stuck in a loop to Europe and back
	// TAC - AI purchases military units - koma13 - START
		for (int i = 0; i < kOwner.getNumEuropeUnits(); ++i)
		{
			CvUnit* pLoopUnit = kOwner.getEuropeUnit(i);
			if (!isFull() && pLoopUnit->canAttack())
			{
				if (pLoopUnit->canLoadUnit(this, plot(), false))
				{
					bool bLoadUnit = (GET_TEAM(getTeam()).getAnyWarPlanCount() > 0) ? true : false;

					if (!bLoadUnit && (pLoopUnit->AI_getUnitAIType() == UNITAI_DEFENSIVE))
					{
						int iUndefended = 0;
						int iNeeded = kOwner.AI_totalDefendersNeeded(&iUndefended);

						if (iUndefended > 0)
						{
							if (GC.getGameINLINE().getSorenRandNum(100, "AI load military unit?") < 50)
							{
								bLoadUnit = true;
							}
						}
					}

					if (bLoadUnit)
					{
						kOwner.loadUnitFromEurope(pLoopUnit, this);
						if (getGroup()->getAutomateType() == AUTOMATE_FULL)
						{
							FAssert(pLoopUnit->getGroup() != NULL);
							pLoopUnit->getGroup()->setAutomateType(AUTOMATE_FULL);
						}
					}
				}
			}
		}
	}
	// TAC - AI purchases military units - koma13 - END

	const int iEstimatedUnemploymentCount = kOwner.AI_estimateUnemploymentCount();

	if (kOwner.AI_shouldHurryUnit() && iEstimatedUnemploymentCount < kOwner.getNumCities() * 2)
	{
		const int iPotentialColonistsToHurry = std::max(0, cargoSpace() - getCargo() - kOwner.getNumEuropeUnits());

		if (iPotentialColonistsToHurry > 0)
		{
			kOwner.AI_hurryBestDockUnits(std::min(GLOBAL_DEFINE_DOCKS_NEXT_UNITS, iPotentialColonistsToHurry));
			logBBAI("CvUnitAI::AI_europe player %S hurries %d units since iEstimatedUnemploymentCount is %d", 
				kOwner.getCivilizationDescription(),
				iPotentialColonistsToHurry, iEstimatedUnemploymentCount);
		}
	}
	// Erik: Pick up the most valuable units first (e.g. statesmen)
	// and leave the less valuable units (e.g. servants) for last
	kOwner.sortEuropeUnits();

	//Pick up units from Europe (FIFO)
	std::deque<CvUnit*> aUnits;
	for (int i = 0; i < kOwner.getNumEuropeUnits(); ++i)
	{
		aUnits.push_back(kOwner.getEuropeUnit(i));
	}

	// Pick up settlers first
	// TODO: Refactor this into a list of priorities
	while (!aUnits.empty() && !isFull())
	{
		CvUnit* pUnit = aUnits.front();
		aUnits.pop_front();
		if (pUnit->canLoadUnit(this, plot(), false) && pUnit->AI_getUnitAIType() == UNITAI_SETTLER)
		{
			kOwner.loadUnitFromEurope(pUnit, this);
			if (getGroup()->getAutomateType() == AUTOMATE_FULL)
			{
				FAssert(pUnit->getGroup() != NULL);
				pUnit->getGroup()->setAutomateType(AUTOMATE_FULL);
			}
		}
	}

	//Pick up units from Europe (FIFO)
	for (int i = 0; i < kOwner.getNumEuropeUnits(); ++i)
	{
		aUnits.push_back(kOwner.getEuropeUnit(i));
	}

	while (!aUnits.empty() && !isFull())
	{
		CvUnit* pUnit = aUnits.front();
		aUnits.pop_front();
		if (pUnit->canLoadUnit(this, plot(), false))
		{
			kOwner.loadUnitFromEurope(pUnit, this);
			if (getGroup()->getAutomateType() == AUTOMATE_FULL)
			{
				FAssert(pUnit->getGroup() != NULL);
				pUnit->getGroup()->setAutomateType(AUTOMATE_FULL);
			}
		}
	}

	if (kOwner.getParent() != NO_PLAYER)	// TAC - AI King no Europe trading bugfix - koma13
	{
		// TAC - AI Economy - koma13 - START
		/*
		if (kOwner.AI_isStrategy(STRATEGY_SELL_TO_NATIVES) && GC.getGameINLINE().getSorenRandNum(2, "AI sell to natives") == 0)
		{
			if ((AI_getUnitAIType() == UNITAI_TRANSPORT_SEA) || (AI_getUnitAIType() == UNITAI_ASSAULT_SEA && GET_TEAM(getTeam()).getAnyWarPlanCount() == 0))
			{
				if (cargoSpace() < 5)
				{
					AI_europeBuyNativeYields();
				}
			}
		}
		*/
		// TAC - AI Economy - koma13 - END

		if (kOwner.AI_getEuropeYieldSpending() < kOwner.AI_getTotalIncome() / 12)	// TAC - AI Economy - koma13
		{
			if (!kOwner.AI_isStrategy(STRATEGY_MILITARY_BUILDUP))	// TAC - AI Military Buildup - koma13
			{
				AI_europeBuyYields();
			}
		}
	}

	// R&R, ray, fix for inactive AI - START
	CvPlot* pStartingPlot = GET_PLAYER(getOwnerINLINE()).getStartingPlot();
	// R&R, ray, fix for inactive AI - END

	// TAC - AI Improved Naval AI - koma13 - START
	if (kOwner.getNumCities() > 0)
	{
		CvPlot* pNewPlot = AI_bestDestinationPlot();

		if (pNewPlot == NULL)
		{
			// R&R, ray, fix for inactive AI - START
			if (pStartingPlot != NULL)
			{
				pNewPlot = pStartingPlot;
			}

			// old code now in else
			else
			{
				getGroup()->pushMission(MISSION_SKIP);
				return true;
			}
			// R&R, ray, fix for inactive AI - END
		}

		if (plot() != pNewPlot)
		{
			setXY(pNewPlot->getX_INLINE(), pNewPlot->getY_INLINE());
		}
	}
	// TAC - AI Improved Naval AI - koma13 - END

	// R&R, ray, fix for inactive AI - START
	else if(plot() == NULL && pStartingPlot != NULL)
	{
		setXY(pStartingPlot->getX_INLINE(), pStartingPlot->getY_INLINE());
	}
	// R&R, ray, fix for inactive AI - END

	FAssert(plot()->isEurope());
	FAssert(canCrossOcean(plot(), UNIT_TRAVEL_STATE_FROM_EUROPE));

	crossOcean(UNIT_TRAVEL_STATE_FROM_EUROPE);
	finishMoves();
	return true;
}

bool CvUnitAI::AI_europeAssaultSea()
{
	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());

	AI_sellYieldUnits(EUROPE);

	//Pick up units from Europe (FIFO)
	while (kOwner.getNumEuropeUnits() > 0)
	{
		if (isFull())
		{
			break;
		}

		CvUnit* pUnit = kOwner.getEuropeUnit(0);
		kOwner.loadUnitFromEurope(pUnit, this);
		if (getGroup()->getAutomateType() == AUTOMATE_FULL)
		{
			FAssert(pUnit->getGroup() != NULL);
			pUnit->getGroup()->setAutomateType(AUTOMATE_FULL);
		}
	}

	FAssert(plot()->isEurope());
	FAssert(canCrossOcean(plot(), UNIT_TRAVEL_STATE_FROM_EUROPE));

	crossOcean(UNIT_TRAVEL_STATE_FROM_EUROPE);
	finishMoves();
	return true;
}

void CvUnitAI::AI_automateSailTo(const SailToHelper& sth)
{
	if (AI_breakAutomation())
	{
		return;
	}

	AI_sailTo(sth, true);
}

bool CvUnitAI::AI_sailToEurope(bool bMove)
{
	return AI_sailTo(kSailToEurope, bMove);
}

/*** TRIANGLETRADE 10/28/08 by DPII ***/
bool CvUnitAI::AI_sailToAfrica(bool bMove)
{
	return AI_sailTo(kSailToAfrica, bMove);
}
/******************************************************/

// R&R, ray, Port Royal
bool CvUnitAI::AI_sailToPortRoyal(bool bMove)
{
	return AI_sailTo(kSailToPortRoyal, bMove);
}
// R&R, ray, Port Royal - END

/// <summary> Attempts to sail (off the map) to the specified port.
/// <param name="sth">A helper struct that contains port travel constants required by the AI</param>
/// <param name="bMove">If true, allows the unit to attempt to generate a path to the closest Euro plot, If false only check the current plot.
/// in both cases, the unit will sail off to the port if it has at least a single movement point left</param>
/// <param name="bIgnoreDanger">Optional, defaults to true. Pass the bIgnoreDanger to the underlying pathfinder</param>
bool CvUnitAI::AI_sailTo(const SailToHelper& sth, bool bMove, bool bIgnoreDanger)
{
	if (canCrossOcean(plot(), sth.unitTravelState))
	{
		crossOcean(sth.unitTravelState);
		if (AI_getUnitAIState() == UNITAI_STATE_SAIL)
		{
			AI_setUnitAIState(UNITAI_STATE_DEFAULT);
		}

		if (getGroup()->getAutomateType() == sth.automateType)
		{
			getGroup()->setAutomateType(NO_AUTOMATE);
		}
		return true;
	}

	if (!bMove)
	{
		return false;
	}

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestMissionPlot = NULL;
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());

	for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		CvPlot* const pLoopPlot = GC.getMap().plotByIndexINLINE(iI);

		// Unrevealed plots need not be considered, also removes cheating!
		if (!pLoopPlot->isRevealed(getTeam(), false))
			continue;

		// Only a small subset of plots allow sailing to ports
		if (!pLoopPlot->isEurope())
			continue;

		// Check for water danger. Note: Must be the same condition as used by pf
		if (kOwner.AI_getWaterDanger(pLoopPlot, -1) > 0)
			continue;

		if (pLoopPlot->getTurnDamage() > 0)
			continue;

		if (canCrossOcean(pLoopPlot, sth.unitTravelState))
		{
			int iPathTurns;
			if (generatePath(pLoopPlot, MOVE_BUST_FOG, true, &iPathTurns/*, bIgnoreDanger*/))
			{
				int iValue = 10000;
				iValue /= 100 + getPathCost();

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = getPathEndTurnPlot();
					pBestMissionPlot = pLoopPlot;
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_BUST_FOG, false, false, sth.missionAI, pBestMissionPlot);
		if (plot()->isEurope())
		{
			if (canCrossOcean(plot(), sth.unitTravelState))
			{
				crossOcean(sth.unitTravelState);
				if (AI_getUnitAIState() == UNITAI_STATE_SAIL)
				{
					AI_setUnitAIState(UNITAI_STATE_DEFAULT);
				}
				if (getGroup()->getAutomateType() == sth.automateType)
				{
					getGroup()->setAutomateType(NO_AUTOMATE);
				}
			}
		}
		return true;
	}
	return false;
}

#if 0
bool CvUnitAI::AI_sailTo(const SailToHelper& sth, bool bMove, bool bIgnoreDanger)
{
	if (canCrossOcean(plot(), sth.unitTravelState))
	{
		crossOcean(sth.unitTravelState);
		if (AI_getUnitAIState() == UNITAI_STATE_SAIL)
		{
			AI_setUnitAIState(UNITAI_STATE_DEFAULT);
		}

		if (getGroup()->getAutomateType() == sth.automateType)
		{
			getGroup()->setAutomateType(NO_AUTOMATE);
		}
		return true;
	}

	if (!bMove)
	{
		return false;
	}

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestMissionPlot = NULL;
	bool bRequireIgnoreDanger = false;


	for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		CvPlot* const pLoopPlot = GC.getMap().plotByIndexINLINE(iI);

		// Unrevealed plots need not be considered, also removes cheating!
		if (!pLoopPlot->isRevealed(getTeam(), false))
			continue;

		// Only a small subset of plots allow sailing to ports
		if (!pLoopPlot->isEurope())
			continue;

		// Check for water danger. Note: Must be the same condition as used by pf
		if (kPlayer.AI_getWaterDanger(pPlot, -1, true, true, false) > 0)
		{
			// Target plot has danger but we may be strong enough
			pLoopSelectionGroup->AI_sumStrength(
				pCity->plot(), DOMAIN_LAND);

			bRequireIgnoreDanger = true;

		else
			// Too dangerous, try another path?

		}



		// Prefer a safe path but consider moving despite danger if we're strong enough
		if (pLoopPlot->isVisibleEnemyDefender(this)
		{
#if 0
			if (GET_PLAYER(getOwnerINLINE()).AI_localDefenceStrength(pLoopPlot, NO_TEAM,
				DOMAIN_SEA, 0, true, false, pSelectionGroup->isHuman());
				> GET_PLAYER(getOwnerINLINE()).AI_localAttackStrength(pLoopPlot, NO_TEAM,
					DOMAIN_SEA, 1, true, false, false, NULL))
#endif	
				if (AI_currEffectiveStr)
				{
					// Path has danger but we should still be able to make it
					bRequireIgnoreDanger = true;
				}
				else
					continue;
		}

		// Don't consider plot with damage for now until we allow the AI to move into these
		// plots
		if (pLoopPlot->getTurnDamage() > 0)
			continue;

		if (canCrossOcean(pLoopPlot, sth.unitTravelState))
		{
			int iPathTurns;
			if (generatePath(pLoopPlot, bRequireIgnoreDanger ? MOVE_IGNORE_DANGER : MOVE_BUST_FOG, true, &iPathTurns))
			{
				int iValue = 10000;
				iValue /= 100 + getPathCost();

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = getPathEndTurnPlot();
					pBestMissionPlot = pLoopPlot;
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_BUST_FOG, false, false, sth.missionAI, pBestMissionPlot);
		if (plot()->isEurope())
		{
			if (canCrossOcean(plot(), sth.unitTravelState))
			{
				crossOcean(sth.unitTravelState);
				if (AI_getUnitAIState() == UNITAI_STATE_SAIL)
				{
					AI_setUnitAIState(UNITAI_STATE_DEFAULT);
				}
				if (getGroup()->getAutomateType() == sth.automateType)
				{
					getGroup()->setAutomateType(NO_AUTOMATE);
				}
			}
		}
		return true;
	}
	return false;
}
// R&R, ray, Port Royal - END

#endif
// TAC - AI Improved Naval AI - koma13 - START
CvPlot* CvUnitAI::findNearbyOceanPlot(const CvPlot& kPlot) const
{
	CvPlot* pBestPlot = NULL;
	int iBestValue = 0;

	for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);

		// Erik: Filter out plots with impassable terrain and\or feature
		// We first deal with the unconditional plots (which can never be a valid target)
		// and then we check unit specific limitations. The latter is not currently
		// used by the XML but should nevertheless be checked from a strict correctness
		// perspective
		if (pLoopPlot->isImpassable())
			continue;

		if (m_pUnitInfo->getTerrainImpassable(pLoopPlot->getTerrainType()))
			continue;

		if (pLoopPlot->getTurnDamage() > 0)
			continue;

		if (pLoopPlot->getFeatureType() != NO_FEATURE)
		{
			if (m_pUnitInfo->getFeatureImpassable(pLoopPlot->getFeatureType()))
				continue;
		}

		if (AI_plotValid(pLoopPlot) && !pLoopPlot->isVisibleEnemyDefender(this))
		{
			if (pLoopPlot->isRevealed(getTeam(), false))
			{
				if (canCrossOcean(pLoopPlot, UNIT_TRAVEL_STATE_FROM_EUROPE))
				{
					int iPathTurns;
					if (getGroup()->generatePath(&kPlot, pLoopPlot, 0, true, &iPathTurns))
					{
						int iValue = 10000;
						iValue /= 100 + getPathCost();

						if ((kPlot.getY_INLINE() == pLoopPlot->getY_INLINE()) ? (iValue >= iBestValue) : (iValue > iBestValue))
						{
							iBestValue = iValue;
							pBestPlot = pLoopPlot;
						}
					}
				}
			}
		}
	}

	return pBestPlot;
}
// TAC - AI Improved Naval AI - koma13 - END

bool CvUnitAI::AI_travelToPort(int iMinPercent, int iMaxPath)
{
	PROFILE_FUNC();
	//Later on I'll make it so it can completely full yield units, for now though not.
	FAssert(!isFull());
	FAssert(iMinPercent > 0);
	FAssert(iMaxPath > 0);

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestMissionPlot = NULL;	// TAC - AI Improved Naval AI - koma13

	const CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	const CvPlayerAI& kEuropePlayer = GET_PLAYER(kOwner.getParent());
	int iLoop;
	for (CvCity* pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop))
	{
		if (!atPlot(pLoopCity->plot()) && AI_plotValid(pLoopCity->plot()))
		{
			if (!(pLoopCity->plot()->isVisibleEnemyUnit(this)))
			{
				// TAC - AI Improved Naval AI - koma13 - START
				//if (GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_TRANSPORT_SEA, getGroup(), 1) == 0)
				if (pLoopCity->isBestPortCity() || (GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_TRANSPORT_SEA, getGroup(), 1) == 0))
				// TAC - AI Improved Naval AI - koma13 - END
				{

					int iPathTurns;
					if (generatePath(pLoopCity->plot(), MOVE_NO_ENEMY_TERRITORY | MOVE_AVOID_ENEMY_WEIGHT_3, true, &iPathTurns, iMaxPath))
					{
						int iBestYieldValue = 0;
						//Nothing too fancy, just find the best yield and best quantity.
						for (int i = 0; i < NUM_YIELD_TYPES; i++)
						{
							const YieldTypes eYield = (YieldTypes)i;
							if (kOwner.isYieldEuropeTradable(eYield))
							{
								if (kOwner.AI_isYieldForSale(eYield))
								{
									const int iAvailable = std::max(0, pLoopCity->getYieldStored(eYield) - pLoopCity->getMaintainLevel(eYield));
									if (iAvailable >= ((GC.getGameINLINE().getCargoYieldCapacity() * iMinPercent) / 100))
									{
										const int iYieldValue = iAvailable * kEuropePlayer.getYieldBuyPrice(eYield);
										iBestYieldValue = std::max(iYieldValue, iBestYieldValue);
									}
								}
							}
						}

						if (iBestYieldValue > 0)
						{
							int iValue = (100000 * iBestYieldValue);

							iValue /= 100 + getPathCost();
							iValue /= 3 + pLoopCity->plot()->getDistanceToOcean();

							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestPlot = getPathEndTurnPlot();
								pBestMissionPlot = pLoopCity->plot();	// TAC - AI Improved Naval AI - koma13
							}
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		// TAC - AI Improved Naval AI - koma13 - START
		//getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), 0, false, false, MISSIONAI_TRANSPORT_SEA, pBestPlot);
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), 0, false, false, MISSIONAI_TRANSPORT_SEA, pBestMissionPlot);
		// TAC - AI Improved Naval AI - koma13 - END
		return true;
	}

	return false;
}

bool CvUnitAI::AI_collectGoods()
{
	bool bLoaded = false;
	CvCity* pCity = plot()->getPlotCity();

	FAssert(pCity != NULL);
	FAssert(pCity->getOwner() == getOwner()); // team?

	CvPlayerAI& kOwner = GET_PLAYER(getOwner());
	CvPlayerAI& kEuropePlayer = GET_PLAYER(kOwner.getParent());

	//First try and load any additional cargo.
	for (int i = 0; i < NUM_YIELD_TYPES; i++)
	{
		YieldTypes eYield = (YieldTypes)i;
		if (kOwner.isYieldEuropeTradable(eYield))
		{
			if (kOwner.AI_isYieldForSale(eYield))
			{
				YieldTypes eYield = (YieldTypes)i;
				int iYieldStored = getLoadedYieldAmount(eYield) % GC.getGameINLINE().getCargoYieldCapacity();

				if (iYieldStored > 0)
				{
					if (pCity->getYieldStored(eYield) > 0)
					{
						loadYield(eYield, false);
						bLoaded = true;
					}
				}
			}
		}
	}

	//Now load full units (or all which is available)
	while (!isFull())
	{
		YieldTypes eBestYield = NO_YIELD;
		int iBestYieldValue = 0;

		for (int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			YieldTypes eYield = (YieldTypes)i;
			if (kOwner.isYieldEuropeTradable(eYield))
			{
				if (kOwner.AI_isYieldForSale(eYield))
				{
					int iStored = getMaxLoadYieldAmount(eYield);
					if (iStored > (GC.getGameINLINE().getCargoYieldCapacity() / 10))
					{
						int iYieldValue = iStored * kEuropePlayer.getYieldBuyPrice(eYield);
						if (iYieldValue > iBestYieldValue)
						{
							iBestYieldValue =iYieldValue;
						eBestYield = eYield;
					}
				}
			}
		}
		}

		if (eBestYield == NO_YIELD)
		{
			break;
		}

		loadYield(eBestYield, false);
		bLoaded = true;
	}

	return bLoaded;
}

// Returns true if a cargo unit has pushed a mission for us, false otherwise. If eUnitAI is valid, then only units with this UnitAI will be allowed to push a mission.
bool CvUnitAI::AI_deliverUnits(UnitAITypes eUnitAI)
{
	// Note: This function should become obsolete after we migrate the unit AI functions to control the transport
	
	if (!hasAnyUnitInCargo())
		return false;

	CvUnit* pColonistUnit = NULL;

	int iBestValue = 0;
	CvPlot* pBestDestination = NULL;
	CvPlot* pBestMissionPlot = NULL;

	CvPlot* pBestPlot = NULL;	// TAC - AI Improved Naval AI - koma13
	MissionAITypes eMissionAI = NO_MISSIONAI;


	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pPlot;
	int iCount;
	UnitAITypes ai = NO_UNITAI;

	iCount = 0;

	pPlot = plot();

	pUnitNode = pPlot->headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = pPlot->getUnitNodeLoop(pUnitNode);

		if (pLoopUnit != NULL && pLoopUnit->getTransportUnit() == this)
		{
			if (eUnitAI == NO_UNITAI || eUnitAI == pLoopUnit->AI_getUnitAIType())
			{
				CvPlot* pDestination = NULL;
				CvPlot* pMissionPlot = NULL;

				if (pLoopUnit->getGroup()->AI_getMissionAIType() != NO_MISSIONAI)
				{
					CvPlot* pDestination = pLoopUnit->getGroup()->AI_getMissionAIPlot();
					FAssert(!pDestination->isWater());

					if ((pDestination != NULL) && (pDestination != plot())) //Don't humor foolishness x[.
					{
						int iPathTurns;

						if (generatePath(pDestination, 0, true, &iPathTurns))
						{
							int iValue = 100000;
							iValue /= 100 + getPathCost();

							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestDestination = canMoveInto(*pDestination) ? pDestination : getGroup()->getPathSecondLastPlot();

								pBestPlot = (getGroup()->getPathEndTurnPlot() == pDestination) ? pBestDestination : getGroup()->getPathEndTurnPlot();	// TAC - AI Improved Naval AI - koma13
								ai = pLoopUnit->getGroup()->getHeadUnit()->AI_getUnitAIType(); // Assert debugging
								eMissionAI = pLoopUnit->getGroup()->AI_getMissionAIType();
							}
						}
					}
				}
			}
		}
	}

	// On the initial turn the ship may have move before the cargo which is  
	if (pBestPlot == NULL || pBestDestination == NULL)
	{
		//m_bHasYielded = true; // Mark the unit as waiting for a higher priority unit to move (this will 
		//AI_setMovePriority(MOVE_PRIORITY_MIN);
		// Return true even though we haven't pushed a mission since we are yielding
		
		// Note: Exploring is necessary for the initial city in case the passenger have not found a viable target
		const CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());

		if (kOwner.getNumCities() == 0 && AI_exploreOcean(1))
		{
			return true;
		}
		else
		{
			// We have cargo units but they haven't decided on their destination. Let's
			// head to the nearest city so that we can unload them
			// Nope, let's try to yield to them first!
			AI_wakeCargo(NO_UNITAI, AI_getMovePriority() + 1);
			return true;
		}
		return false;
	}

	bool ok = false;

	// Check if pBestPlot is a city or coastal land

	if (pBestPlot->isCity())
		ok = true;

	if (!ok)
	{
		FOR_EACH_ADJ_PLOT(pBestPlot,
		{
			if (pAdjacentPlot->isCoastalLand())
			{
				ok = true;
				break;
			}
		})
	}
	FAssertMsg(ok, "Units cannot be delivered to a plot that lacks a city or coastal access!")
	if (!ok)
	{
		CvWString szString; 
		getUnitAIString(szString, ai);
		logBBAI("Warning: CvUnitAI::AI_deliverUnits ok is false for cargo UNITAI: %S", szString.GetCString());
	}

	/*
	if (iBestValue > 0)
	{
		// TAC - AI Improved Naval AI - koma13 - START
		//getGroup()->pushMission(MISSION_MOVE_TO, pBestDestination->getX_INLINE(), pBestDestination->getY_INLINE());
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), 0, false, false, NO_MISSIONAI, pBestDestination);
		// TAC - AI Improved Naval AI - koma13 - END

		bool bWaitForCargo = AI_wakeCargo(NO_UNITAI, AI_getMovePriority() + 1);

		if (bWaitForCargo)
		{
			if (canMove())
			{
				AI_setMovePriority(AI_getMovePriority() - 1);
			}
		}
		return true;
	}
	return false;
	*/
	if (iBestValue > 0)
	{
		if (atPlot(pBestPlot))
		{
			// BUG: Only wake the cargo if we're on land or we're adjacent to (coastal) land that the unit can move into!
			// TODO: Check return value!
			if (ok && AI_wakeCargo(NO_UNITAI, AI_getMovePriority() + 1))
			{
				logBBAI("CvUnitAI::AI_deliverUnits returning true for Unit %d without pushing order!", getID());
				// Need to yield here to let the cargo take its turn
				return true;
			}
			else if (!ok)
			{
				// In case the unit fails to act, just unload everything
				unloadAll(); // TODO: what about adjacent plots ?
			}
			return false;
		}
		else
		{
			getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), 0, false, false, eMissionAI, pBestDestination);
			return true;
		}
	}
	return false;


}
// TAC - AI Improved Naval AI - koma13 - START
CvPlot* CvUnitAI::AI_bestDestinationPlot(bool bIgnoreDanger) const
{
	const CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	CvPlot* const pPlot = plot();
	CvPlot* pBestPlot = NULL;
	int iBestValue = 0;

	int iLoop;
	for (CvCity* pCity = kOwner.firstCity(&iLoop); pCity != NULL; pCity = kOwner.nextCity(&iLoop))
	{
		CvPlot* const pCityPlot = pCity->plot();
		if (pCityPlot->isEuropeAccessable())
		{
			CvPlot* const pOceanPlot = findNearbyOceanPlot(*pCityPlot);
			const int iFlags = MOVE_BUST_FOG | (bIgnoreDanger ? MOVE_IGNORE_DANGER : 0);
			if (pOceanPlot != NULL && getGroup()->generatePath(pOceanPlot, pCityPlot, iFlags, true, NULL))
			{
				int iValue = (pCity->isBestPortCity() ? 2 : 1);

				CLLNode<IDInfo>* pUnitNode;
				CvArea* const pArea = pCityPlot->area();
				pUnitNode = pPlot->headUnitNode();

				while (pUnitNode != NULL)
				{
					CvUnit* const pLoopUnit = pPlot->getUnitNodeLoop(pUnitNode);

					if (pLoopUnit != NULL && pLoopUnit->getTransportUnit() == this)
					{
						int iTempValue = 0;

						const YieldTypes eYield = pLoopUnit->getYield();
						if (NO_YIELD != eYield)
						{
							iTempValue += 10 + 3 * std::max(0, pCity->getMaintainLevel(pLoopUnit->getYield()) - pCity->getYieldStored(pLoopUnit->getYield()));
							iTempValue += pCity->getMaxYieldCapacity() - pCity->getYieldStored(pLoopUnit->getYield());
						}
						else
						{
							iTempValue += 100000;
							int iNumAIUnits = pCityPlot->plotCount(PUF_isUnitAIType, pLoopUnit->AI_getUnitAIType(), -1, getOwnerINLINE());

							for (int i = 0; i < pCity->getPopulation(); ++i)
							{
								CvUnit* const pLoopCitizen = pCity->getPopulationUnitByIndex(i);
								if (pLoopUnit->getUnitType() == pLoopCitizen->getUnitType())
								{
									iNumAIUnits++;
								}
							}

							iTempValue *= 1 + pArea->getNumCities();
							iTempValue /= 1 + iNumAIUnits;

							if (pLoopUnit->AI_getUnitAIType() == UNITAI_OFFENSIVE)
							{
								CvArea* const pArea = pCity->area();
								if (pArea->getAreaAIType(getTeam()) == AREAAI_NEUTRAL)
								{
									iTempValue = 0;
								}
							}
						}

						iValue += iTempValue;
					}
				}

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = pOceanPlot;
				}
			}
		}
	}

	return pBestPlot;
}
// TAC - AI Improved Naval AI - koma13 - END

//Returns true if any units are loaded.
bool CvUnitAI::AI_loadUnits(UnitAITypes eUnitAI, MissionAITypes eMissionAI)
{
	CvPlot * pPlot = plot();
	CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();

	int iCount = 0;
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = pPlot->getUnitNodeLoop(pUnitNode);

		if (pLoopUnit != NULL && !pLoopUnit->isCargo())
		{
			if ((eUnitAI == NO_UNITAI) || (pLoopUnit->AI_getUnitAIType() == eUnitAI))
			{
				if ((eMissionAI == NO_MISSIONAI) || (pLoopUnit->getGroup()->AI_getMissionAIType() == eMissionAI))
				{
					if (pLoopUnit->canLoadUnit(this, plot(), true))
					{
						pLoopUnit->loadUnit(this);
						pLoopUnit->jumpTo(this->plot());
						iCount++;

						/*
						if (!isHuman())
						{
							GET_PLAYER(getOwnerINLINE()).AI_removeUnitFromMoveQueue(pLoopUnit);
						}
						*/
					}
				}
			}
		}
	}
	return (iCount > 0);
}

bool CvUnitAI::AI_wakeCargo(UnitAITypes eUnitAI, int iPriority)
{
	bool bWaitForCargo = false;
	CvPlot* pPlot = plot();

	CLLNode<IDInfo>*  pUnitNode = plot()->headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = plot()->getUnitNodeLoop(pUnitNode);

		if (pLoopUnit != NULL && pLoopUnit->isCargo() && (pLoopUnit->getTransportUnit()->getGroup() == getGroup()))
		{
			if (eUnitAI == NO_UNITAI || pLoopUnit->AI_getUnitAIType() == eUnitAI)
			{
				// TODO: Consider enabling this check to avoid a possible endless loop if the awakened
				// units fail to act
				//if (pLoopUnit->canMove())	// TAC - AI Improved Naval AI - koma13
				{
					pLoopUnit->AI_setMovePriority(AI_getMovePriority() + 1);
				}
				bWaitForCargo = true;
				m_bHasYielded = true; // Mark the unit as waiting for a higher priority unit to move (this will 
				// prevent AI_update() to be called again for this unit)
				
				logBBAI("CvUnitAI::AI_wakeCargo() Player %S Unit %d [%d, %d]",
					GET_PLAYER(getOwnerINLINE()).getCivilizationDescription(), 
					getID(), getX_INLINE(), getY_INLINE());
			}
		}
	}
	return bWaitForCargo;
}

//This function is used for units on board transports, essentially they may decide
//where they want to go.
//The mission MUST be one with a plot destination.
CvPlot* CvUnitAI::AI_determineDestination(CvPlot** ppMissionPlot, MissionTypes* peMission, MissionAITypes* peMissionAI)
{
	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;

	CvUnit* pTransport = getTransportUnit();
	CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());
	FAssert(pTransport != NULL);

	if (AI_getUnitAIType() == UNITAI_SCOUT)
	{

		for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
		{
			CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);
			const CvArea& kArea = pLoopPlot->getConstArea();
			if (!kArea.isWater() && kArea.getNumUnrevealedTiles(getTeam()) > 0)
			{
				if (pLoopPlot->isRevealed(getTeam(), false))
				{
					int iUnrevealedCount = 0;
					int iWaterCount = 0;
					for (int iJ = 0; iJ < NUM_DIRECTION_TYPES; iJ++)
					{
						CvPlot* pLoopPlot2 = plotDirection(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), (DirectionTypes)iJ);

						if (pLoopPlot2 != NULL)
						{
							if (!pLoopPlot2->isWater())
							{
								if (!pLoopPlot2->isRevealed(getTeam(), false))
								{
									iUnrevealedCount++;
									if (iWaterCount > 0)
									{
										break;
									}
								}
							}
							else
							{
								iWaterCount++;
								if (iUnrevealedCount > 0)
								{
									break;
								}
							}
						}
					}
					if ((iUnrevealedCount > 0) && (iWaterCount > 0))
					{
						if (kPlayer.AI_areaMissionAIs(kArea, MISSIONAI_EXPLORE, pTransport->getGroup()) == 0)
						{
							int iPathTurns;
							if (pTransport->generatePath(pLoopPlot, 0, true, &iPathTurns))
							{
								int iValue = 100000 / (100 + getPathCost());
								if (iValue > iBestValue)
								{
									iBestValue = iValue;
									pBestPlot = pTransport->canMoveInto(*pLoopPlot) ? pLoopPlot : pTransport->getGroup()->getPathSecondLastPlot();
									if (ppMissionPlot != NULL)
									{
										*ppMissionPlot = pLoopPlot;
									}
									if (peMissionAI != NULL)
									{
										*peMissionAI = MISSIONAI_EXPLORE;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	else if (AI_getUnitAIType() == UNITAI_COLONIST)
	{
		for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
		{
			CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);

			if ((pTransport == NULL && pLoopPlot->getArea() == getArea()) ||
				(pTransport != NULL && pLoopPlot->isCoastalLand()))
			{
				int iValue = 10000 * kPlayer.AI_estimatedColonistIncome(pLoopPlot, this);
				if (kPlayer.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_FOUND, pTransport->getGroup(), 4) == 0)
				{
					int iPathTurns;
					if (pTransport->generatePath(pLoopPlot, 0, true, &iPathTurns))
					{
						iValue *= 1000;
						iValue /= pTransport->baseMoves() * 100 + pTransport->getPathCost();

						iValue /= 3 + pLoopPlot->getDistanceToOcean();

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							pBestPlot = pTransport->canMoveInto(*pLoopPlot) ? pLoopPlot : pTransport->getGroup()->getPathSecondLastPlot();
							if (ppMissionPlot != NULL)
							{
								*ppMissionPlot = pLoopPlot;
							}
							if (peMissionAI != NULL)
							{
								*peMissionAI = MISSIONAI_FOUND;
							}
						}
					}
				}
			}
		}
	}

	if (pBestPlot == NULL)
	{
		int iLoop;
		CvCity* pLoopCity;
		for (pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
		{
			CvPlot* pLoopPlot = pLoopCity->plot();
			int iPathTurns = 0;
			if (generatePath(pLoopPlot, 0, true, &iPathTurns))
			{
				int iValue = 10000 * pLoopCity->getPopulation();
				iValue *= 10 + pLoopCity->plot()->getDistanceToOcean();
				iValue /= 100 + pTransport->getPathCost();

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = pTransport->canMoveInto(*pLoopPlot) ? pLoopPlot : pTransport->getGroup()->getPathSecondLastPlot();
					if (ppMissionPlot != NULL)
					{
						*ppMissionPlot = pLoopPlot;
					}
					if (peMissionAI != NULL)
					{
						*peMissionAI = NO_MISSIONAI;
					}
				}
			}
		}
	}

	return pBestPlot;
}

bool CvUnitAI::AI_moveFromTransport(CvPlot* pHintPlot)
{
    int iBestValue = 0;
    CvPlot* pBestPlot = NULL;

    for (int i = 0; i < NUM_DIRECTION_TYPES; ++i)
    {
        CvPlot* pLoopPlot = plotDirection(getX_INLINE(), getY_INLINE(), (DirectionTypes)i);
        if (pLoopPlot != NULL)
        {

            int iValue = 0;
            for (int iDirection = 0; iDirection < NUM_DIRECTION_TYPES; iDirection++)
            {
                CvPlot* pDirectionPlot = plotDirection(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), (DirectionTypes)iDirection);
                if (pDirectionPlot != NULL)
                {
                    if (pDirectionPlot->isCity())
                    {
                        if (atWar(getTeam(), pDirectionPlot->getTeam()))
                        {
                            iValue += pDirectionPlot->getPlotCity()->getPopulation() * 100;
                        }
                    }
                }
            }

            if (pLoopPlot->isCity())
            {
                if (pLoopPlot->getTeam() == getTeam() && !pLoopPlot->getPlotCity()->AI_isDefended())
                {
                    iValue += pLoopPlot->getPlotCity()->getPopulation() * 100;
                }
                else if (atWar(getTeam(), pLoopPlot->getTeam()))
                {
					if (pLoopPlot->getNumVisibleEnemyDefenders(this) == 0)
					{
						iValue += pLoopPlot->getPlotCity()->getPopulation() * 150;
					}
                }
            }
            else if (iValue > 0)
            {
				iValue += pLoopPlot->defenseModifier(getTeam());
            }

			if (iValue > 0)
			{
				if (generatePath(pLoopPlot, 0, true))
				{
					if (iValue > iBestValue)
					{
						pBestPlot = pLoopPlot;
						iBestValue = iValue;
					}
				}
            }
        }
    }

    if (pBestPlot != NULL)
    {
        getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), 0);
        return true;
    }
    return false;
}

bool CvUnitAI::AI_attackFromTransport(CvPlot* pHintPlot, int iLowOddsThreshold, int iHighOddsThreshold)
{
	if (AI_smartAttack(1, iLowOddsThreshold, iHighOddsThreshold, pHintPlot))
	{
		return true;
	}
	return false;
}

int CvUnitAI::AI_getMovePriority() const
{
	if (isDelayedDeath())
	{
		return 0;
	}
	return m_iMovePriority;
}

void CvUnitAI::AI_setMovePriority(int iNewValue)
{
	// Set priority only if group head
	//getGroup()

	m_iMovePriority = iNewValue;

	logBBAI("CvUnitAI::AI_setMovePriority Player %S Unit %d. %S(%S)[%d, %d] %s,%s",
		GET_PLAYER(getOwnerINLINE()).getCivilizationDescription(), getID(),
		getName().GetCString(), GET_PLAYER(getOwnerINLINE()).getName(),
		getX_INLINE(), getY_INLINE(), isOnMap() ? "isOnMap:true" : "isOnMap:false",
		isCargo() ? "isCargo:true" : "isCargo:false");


	if (AI_getMovePriority() <= 0)
	{
		GET_PLAYER(getOwnerINLINE()).AI_removeUnitFromMoveQueue(this);
	}
	else
	{
		GET_PLAYER(getOwnerINLINE()).AI_addUnitToMoveQueue(this);
	}
}

bool CvUnitAI::AI_hasAIChanged(int iNumTurns) const
{
	if (getGameTurnCreated() == AI_getLastAIChangeTurn())
	{
		return false;
	}

	if (GC.getGameINLINE().getGameTurn() - AI_getLastAIChangeTurn() < iNumTurns)
	{
		return true;
	}
	return false;
}

int CvUnitAI::AI_getLastAIChangeTurn() const
{
	return m_iLastAIChangeTurn;
}

void CvUnitAI::AI_doInitialMovePriority()
{
	if (!shouldUnitMove(this))
	{
		return;
	}
	int iMovePriority = 0;

	if (getGroup()->getActivityType() == ACTIVITY_MISSION)
	{
		return;
	}

	if (getUnitTravelState() == NO_UNIT_TRAVEL_STATE)
	{
		if (GET_PLAYER(getOwnerINLINE()).AI_isKing())
		{
			if (getDomainType() == DOMAIN_SEA)
			{
				iMovePriority = 1400;
			}
			else if (getDomainType() == DOMAIN_LAND)
			{
				iMovePriority = 700;


				if (!isCargo())
				{
					iMovePriority += 50;
				}
				if (canBombard(plot()))
				{
					iMovePriority += 100;
				}

				iMovePriority += baseCombatStr();
			}
		}
		else
		{
			if (isCargo())
			{
				if (canMove())
				{
					iMovePriority = MOVE_PRIORITY_MAX;
				}
				else if (getYield() != NO_YIELD)
				{
					if (AI_getUnitAIState() == UNITAI_STATE_PURCHASED)
					{
						iMovePriority = MOVE_PRIORITY_MAX;
					}
					else if (AI_getUnitAIState() == UNITAI_STATE_SELL_TO_NATIVES)
					{
						iMovePriority = MOVE_PRIORITY_MAX;
					}
				}
			}
			else if (canMove())
			{
				if (getDomainType() == DOMAIN_SEA)
				{
					iMovePriority = MOVE_PRIORITY_HIGH;
				}
				else
				{
					if (AI_getUnitAIType() == UNITAI_SETTLER)
					{
						iMovePriority = MOVE_PRIORITY_MAX;
					}
					else
					{
						iMovePriority = MOVE_PRIORITY_MEDIUM;
					}
				}
			}
		}
	}
	else if (getUnitTravelState() == UNIT_TRAVEL_STATE_IN_EUROPE || getUnitTravelState() == UNIT_TRAVEL_STATE_IN_AFRICA)
	{
		if (getDomainType() == DOMAIN_SEA)
		{
			iMovePriority = MOVE_PRIORITY_MIN;
		}
	}

	AI_setMovePriority(iMovePriority);
}

void CvUnitAI::AI_doFound()
{
	doFound(false);
}

ProfessionTypes CvUnitAI::AI_getOldProfession() const
{
	return m_eOldProfession;
}

void CvUnitAI::AI_setOldProfession(ProfessionTypes eProfession)
{
	m_eOldProfession = eProfession;
}

ProfessionTypes CvUnitAI::AI_getIdealProfession() const
{
	if (m_eIdealProfessionCache == INVALID_PROFESSION)
	{
		m_eIdealProfessionCache = GET_PLAYER(getOwnerINLINE()).AI_idealProfessionForUnit(getUnitType());
	}
	return m_eIdealProfessionCache;
}

// XXX make sure we include any new UnitAITypes...
int CvUnitAI::AI_promotionValue(PromotionTypes ePromotion)
{
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	CvPromotionInfo& kPromotion = GC.getPromotionInfo(ePromotion);

	int iValue = 0;

	UnitAITypes eUnitAI = AI_getUnitAIType();

	if (kPromotion.getVisibilityChange() != 0)
	{
		if ((eUnitAI == UNITAI_SCOUT) || (getDomainType() == DOMAIN_SEA))
		{
			iValue += 25 * kPromotion.getVisibilityChange();
		}
	}

	if (kPromotion.getMovesChange() != 0)
	{
		if (eUnitAI == UNITAI_TRANSPORT_SEA || eUnitAI == UNITAI_TRANSPORT_COAST)
		{
			iValue += 50 * kPromotion.getMovesChange();
		}
		else if (eUnitAI != UNITAI_DEFENSIVE)
		{
			iValue += 25 * kPromotion.getMovesChange();
		}
	}
	if (kPromotion.getMoveDiscountChange() > 0)
	{
		if (baseMoves() > 1)
		{
			iValue += (eUnitAI == UNITAI_SCOUT) ? 25 : 10;
		}
	}
	if (kPromotion.getWithdrawalChange() != 0)
	{
		if (eUnitAI == UNITAI_OFFENSIVE)
		{
			iValue += kPromotion.getWithdrawalChange() * 150;
			iValue /= (100 - withdrawalProbability());
		}
	}
	if (kPromotion.getCargoChange() != 0)
	{
		if (eUnitAI == UNITAI_TRANSPORT_SEA || eUnitAI == UNITAI_WAGON || eUnitAI == UNITAI_TRANSPORT_COAST)
		{
			iValue += kPromotion.getCargoChange() * 50;
		}
	}
	if (kPromotion.getBombardRateChange() != 0)
	{
		if ((eUnitAI == UNITAI_COMBAT_SEA) || (eUnitAI == UNITAI_OFFENSIVE))
		{
			iValue += kPromotion.getBombardRateChange();
		}
	}
	if (kPromotion.getEnemyHealChange() != 0)
	{
		if (eUnitAI == UNITAI_OFFENSIVE)
		{
			iValue += kPromotion.getEnemyHealChange();
		}
	}
	if (kPromotion.getNeutralHealChange() != 0)
	{
		if (eUnitAI == UNITAI_OFFENSIVE || eUnitAI == UNITAI_COUNTER)
		{
			iValue += kPromotion.getNeutralHealChange();
		}
	}
	if (kPromotion.getFriendlyHealChange() != 0)
	{
		iValue += kPromotion.getFriendlyHealChange();
	}
	if (kPromotion.getSameTileHealChange() != 0)
	{
		int iTotalUnits = 0;
		if (kOwner.AI_countPromotions(ePromotion, plot(), 2, &iTotalUnits) == 0)
		{
			if (iTotalUnits > 3)
			{
				iValue += 10 + iTotalUnits;
			}
		}
	}
	if (kPromotion.getAdjacentTileHealChange() != 0)
	{
		int iTotalUnits = 0;
		if (kOwner.AI_countPromotions(ePromotion, plot(), 2, &iTotalUnits) == 0)
		{
			if (iTotalUnits > 10)
			{
				iValue += 30;
			}
		}
	}

	if (kPromotion.getCombatPercent() != 0)
	{
		if (eUnitAI == UNITAI_COUNTER || eUnitAI == UNITAI_OFFENSIVE)
		{
			iValue += kPromotion.getCombatPercent() * 2;
		}
		else if (eUnitAI == UNITAI_ATTACK_CITY)
		{
			iValue += kPromotion.getCombatPercent() / 2; // Prefer city attack first
		}
		else
		{
			iValue += kPromotion.getCombatPercent();
		}
	}

	if (kPromotion.getCityAttackPercent() != 0)
	{
		if (eUnitAI == UNITAI_ATTACK_CITY)
		{
			iValue += kPromotion.getCityAttackPercent() * (10 + cityAttackModifier()) / 10;
		}
	}

	if (kPromotion.getCityDefensePercent() != 0)
	{
		if (eUnitAI == UNITAI_DEFENSIVE)
		{
			iValue += kPromotion.getCityDefensePercent() * (10 + cityDefenseModifier()) / 10;
		}
	}
	if (kPromotion.getHillsAttackPercent() != 0)
	{
		if ((eUnitAI == UNITAI_COUNTER) && (kOwner.AI_countPromotions(ePromotion, plot(), 1) == 0))
		{
			iValue += kPromotion.getHillsAttackPercent() / 2;
		}
	}
	if (kPromotion.getHillsDefensePercent() != 0)
	{
		if ((eUnitAI == UNITAI_COUNTER) && kOwner.AI_countPromotions(ePromotion, plot(), 1) == 0)
		{
			iValue += kPromotion.getHillsDefensePercent() / 2;
		}
	}
	if (kPromotion.getDomesticBonusPercent() != 0)
	{
		if (eUnitAI == UNITAI_DEFENSIVE)
		{
			iValue += kPromotion.getDomesticBonusPercent() / 4;
		}
	}
	// Promotions below are generally worthless
	/*
	if (kPromotion.getPillageChange() != 0)
	{
		if (eUnitAI == UNITAI_OFFENSIVE)
		{
			iValue += kPromotion.getPillageChange() / 4;
		}
	}
	
	//WTP, ray, Animal Promotions increase gold from Animals - START
	if (kPromotion.getAnimalGoldChange() != 0)
	{
		if (eUnitAI == UNITAI_OFFENSIVE)
		{
			iValue += kPromotion.getAnimalGoldChange()  / 8; // not as valuable as Pillaging
		}
	}
	//WTP, ray, Animal Promotions increase gold from Animals - END
	
	//WTP, ray, Slave Hunter and Slave Master - START
	if (kPromotion.getSlaveRevoltReductionBonus() != 0)
	{
		if (eUnitAI == UNITAI_DEFENSIVE)
		{
			iValue += kPromotion.getSlaveRevoltReductionBonus()  / 4;
		}
	}

	if (kPromotion.getSlaveWorkerProductionBonus() != 0)
	{
		if (eUnitAI == UNITAI_DEFENSIVE)
		{
			iValue += kPromotion.getSlaveWorkerProductionBonus()  / 4;
		}
	}
	//WTP, ray, Slave Hunter and Slave Masters - END
	*/
	if (kPromotion.getExperiencePercent() != 0)
	{
		if (eUnitAI == UNITAI_ATTACK_CITY || UNITAI_OFFENSIVE || eUnitAI == UNITAI_DEFENSIVE)
		{
			iValue += kPromotion.getExperiencePercent() / 4;
		}
	}
	
	if (kPromotion.isBlitz())
	{
		const int iExtraAttacks = baseMoves() - 1;

		if (iExtraAttacks > 0)
		{
			if ((eUnitAI != UNITAI_DEFENSIVE) && canAttack())
			{
				iValue += iExtraAttacks * 20;
			}
		}
	}

	if (kPromotion.isAmphib())
	{
		if (eUnitAI == UNITAI_OFFENSIVE)
		{
			iValue += 4;
		}
	}

	if (kPromotion.isRiver())
	{
		if (eUnitAI == UNITAI_OFFENSIVE)
		{
			iValue += 4;
		}
	}

	if (kPromotion.isEnemyRoute())
	{
		iValue += 10;
	}

	if (kPromotion.isAlwaysHeal())
	{
		if (eUnitAI != UNITAI_DEFENSIVE)
		{
			iValue += 25;
		}
	}

	if (isHillsDoubleMove())
	{
		if (eUnitAI == UNITAI_COUNTER)
		{
			iValue += 20;
		}
	}

	if (eUnitAI == UNITAI_COUNTER)
	{
		if (kOwner.AI_countPromotions(ePromotion, plot(), 1) == 0)
		{
			for (int i = 0; i < GC.getNumTerrainInfos(); ++i)
			{
				int iPercent = kPromotion.getTerrainAttackPercent(i) + kPromotion.getTerrainDefensePercent(i);
				if (kPromotion.getTerrainDoubleMove(i))
				{
					iPercent += 10;
				}
				if (iPercent > 0)
				{
					iPercent *= 20 + terrainAttackModifier((TerrainTypes)i) + terrainDefenseModifier((TerrainTypes)i);
					iPercent /= 20;

					int iCount = 0;
					for (int j = 0; j < NUM_DIRECTION_TYPES; ++j)
					{
						CvPlot* pLoopPlot = plotDirection(getX_INLINE(), getY_INLINE(), (DirectionTypes)j);
						if (pLoopPlot != NULL)
						{
							if (pLoopPlot->getTerrainType() == i)
							{
								++iCount;
							}
						}
					}
					if (iCount > 0)
					{
						iValue += iPercent / 3 + iCount * iPercent / NUM_DIRECTION_TYPES;
					}
				}
			}

			for (int i = 0; i < GC.getNumFeatureInfos(); ++i)
			{
				int iPercent = kPromotion.getFeatureAttackPercent(i) + kPromotion.getFeatureDefensePercent(i);
				if (kPromotion.getFeatureDoubleMove(i))
				{
					iPercent += 10;
				}
				if (iPercent > 0)
				{
					iPercent *= 20 + featureAttackModifier((FeatureTypes)i) + featureDefenseModifier((FeatureTypes)i);
					iPercent /= 20;

					int iCount = 0;
					for (int j = 0; j < NUM_DIRECTION_TYPES; ++j)
					{
						CvPlot* pLoopPlot = plotDirection(getX_INLINE(), getY_INLINE(), (DirectionTypes)j);
						if (pLoopPlot != NULL)
						{
							if (pLoopPlot->getFeatureType() == i)
							{
								++iCount;
							}
						}
					}
					if (iCount > 0)
					{
						iValue += iPercent / 3 + iCount * iPercent / NUM_DIRECTION_TYPES;
					}
				}
			}
		}
	}

	for (int i = 0; i < GC.getNumUnitClassInfos(); ++i)
	{
		if (kPromotion.getUnitClassAttackModifier(i) > 0)
		{
			if (eUnitAI == UNITAI_COUNTER || eUnitAI == UNITAI_OFFENSIVE)
			{
				iValue += kPromotion.getUnitClassAttackModifier(i) / 3;
			}
		}
		if (kPromotion.getUnitClassDefenseModifier(i) > 0)
		{
			if (eUnitAI == UNITAI_DEFENSIVE)
			{
				iValue += kPromotion.getUnitClassDefenseModifier(i) / 3;
			}
		}
	}
	if (eUnitAI == UNITAI_COUNTER || eUnitAI == UNITAI_DEFENSIVE)
	{
		for (int i = 0; i < GC.getNumUnitCombatInfos(); ++i)
		{
			iValue += kPromotion.getUnitCombatModifierPercent(i) / 2;
		}
	}
	for (int i = 0; i < NUM_DOMAIN_TYPES; ++i)
	{
		if (kPromotion.getDomainModifierPercent(i) > 0)
		{
			if (getDomainType() == i)
			{
				iValue += kPromotion.getDomainModifierPercent(i);
			}
		}
	}

	if (iValue > 0)
	{
		iValue += GC.getGameINLINE().getSorenRandNum(15, "AI Promote");
	}
	return iValue;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_shadow(UnitAITypes eUnitAI, int iMax, int iMaxRatio, bool bWithCargoOnly)
{
	PROFILE_FUNC();

	CvUnit* pLoopUnit;
	CvUnit* pBestUnit;
	int iPathTurns;
	int iValue;
	int iBestValue;
	int iLoop;

	iBestValue = 0;
	pBestUnit = NULL;

	for(pLoopUnit = GET_PLAYER(getOwnerINLINE()).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(getOwnerINLINE()).nextUnit(&iLoop))
	{
		if (pLoopUnit != this)
		{
			if (AI_plotValid(pLoopUnit->plot()))
			{
				if (pLoopUnit->isGroupHead())
				{
					if (!(pLoopUnit->isCargo()))
					{
						if (pLoopUnit->AI_getUnitAIType() == eUnitAI)
						{
							if (pLoopUnit->getGroup()->baseMoves() <= getGroup()->baseMoves())
							{
								if (!bWithCargoOnly || pLoopUnit->getGroup()->hasCargo())
								{
									int iShadowerCount = GET_PLAYER(getOwnerINLINE()).AI_unitTargetMissionAIs(pLoopUnit, MISSIONAI_SHADOW, getGroup());
									if (((-1 == iMax) || (iShadowerCount < iMax)) &&
										 ((-1 == iMaxRatio) || (iShadowerCount == 0) || (((100 * iShadowerCount) / std::max(1, pLoopUnit->getGroup()->countNumUnitAIType(eUnitAI))) <= iMaxRatio)))
									{
										if (!(pLoopUnit->plot()->isVisibleEnemyUnit(this)))
										{
											if (generatePath(pLoopUnit->plot(), 0, true, &iPathTurns))
											{
												//if (iPathTurns <= iMaxPath) XXX
												{
													iValue = 1 + pLoopUnit->getGroup()->getCargo();
													iValue *= 1000;
													iValue /= 1 + iPathTurns;

													if (iValue > iBestValue)
													{
														iBestValue = iValue;
														pBestUnit = pLoopUnit;
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
	}

	if (pBestUnit != NULL)
	{
		if (atPlot(pBestUnit->plot()))
		{
			getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_SHADOW, NULL, pBestUnit);
			return true;
		}
		else
		{
			getGroup()->pushMission(MISSION_MOVE_TO_UNIT, pBestUnit->getOwnerINLINE(), pBestUnit->getID(), 0, false, false, MISSIONAI_SHADOW, NULL, pBestUnit);
			return true;
		}
	}

	return false;
}


bool CvUnitAI::AI_unloadWhereNeeded(int iMaxPath)
{
	FAssert(isCargo());

	const CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());

	CvUnit* pTransportUnit = getTransportUnit();

	// TAC - AI Assault Sea - koma13 - START
	if (pTransportUnit->AI_getUnitAIType() == UNITAI_ASSAULT_SEA)
	{
		if (pTransportUnit->getGroup()->AI_getMissionAIType() == MISSIONAI_ASSAULT)
		{
			return false;
		}

		AreaAITypes eAreaAIType = area()->getAreaAIType(getTeam());

		if (eAreaAIType == AREAAI_ASSAULT || eAreaAIType == AREAAI_ASSAULT_MASSING)
		{
			return false;
		}
	}
	// TAC - AI Assault Sea - koma13 - END

	int iBestValue = 0;
	
	// TAC - AI City Defense - koma13 - START
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int iCount;

	iCount = 0;

	pUnitNode = plot()->headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = plot()->getUnitNodeLoop(pUnitNode);

		if (pLoopUnit != NULL && pLoopUnit->getOwnerINLINE() == getOwnerINLINE())
		{
			if (pLoopUnit->getTransportUnit() == pTransportUnit)
			{
				if (pLoopUnit->AI_getUnitAIType() == AI_getUnitAIType())
				{
					iCount++;
				}
			}
		}
	}
	// TAC - AI City Defense - koma13 - END

	CvPlot* pEndTurnPlot = NULL;
	CvPlot* pDestinationPlot = NULL;

	FOREACH_CITY_OF_OWNER(pCity, kOwner)
	{
		CvPlot* const pLoopPlot = pCity->plot();

		int iPathTurns = 0;

		if (pTransportUnit->generatePath(pLoopPlot, MOVE_SAFE_TERRITORY, false, &iPathTurns, iMaxPath))
		{
			CvArea* pArea = pLoopPlot->area();

			int iValue = 1000;
			int iNumAIUnits = pArea->getNumAIUnits(getOwnerINLINE(), AI_getUnitAIType()) - ((pArea == area()) ? iCount : 0);
			FAssert(iNumAIUnits >= 0);

			// dirty dirty workaround to avoid a crash
			// TODO make a proper fix
			if (iNumAIUnits < 0)
			{
				iNumAIUnits = 0;
			}

			iValue *= 1 + pArea->getNumCities();
			iValue /= 1 + iNumAIUnits;
			iValue = std::max(0, iValue - iPathTurns);
				
			if (kOwner.AI().AI_unitAIIsCombat(AI_getUnitAIType()))
			{
				if (pArea->getAreaAIType(getTeam()) == AREAAI_NEUTRAL)
				{
					iValue /= GET_PLAYER(getOwnerINLINE()).AI_isPrimaryArea(*pArea) ? 10 : 30;
				}
			}
			// TAC - AI City Defense - koma13 - END
			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pEndTurnPlot = getPathEndTurnPlot();
				pDestinationPlot = pLoopPlot;
			}
		}
	}

	if (pEndTurnPlot != NULL && pDestinationPlot != NULL)
	{
		FAssert(!pEndTurnPlot->isImpassable() || canMoveImpassable());

		// Check if we can reach the destination plot on this turn
		if (pEndTurnPlot == pDestinationPlot || ::stepDistance(pEndTurnPlot, pDestinationPlot) == 1)
		{
			if (at(*pDestinationPlot))
			{
				FAssert(pEndTurnPlot->isCity());
				// We're in a city, unload so the cargo can take its turn (unload will set the transported units priority above 
				// the transport so they can move this turn)
				pTransportUnit->getGroup()->unloadAll();
				return true;
			}
			else if (at(*pEndTurnPlot) && !pTransportUnit->canMove() && canMove())
			{
				// We're adjacent to the destination plot so let's disembark if we can move. Note: MOVE_IGNORE_DANGER will ensure that the move does not get cancelled
				// once the automission starts
				getGroup()->pushMission(MISSION_MOVE_TO, pDestinationPlot->getX_INLINE(), pDestinationPlot->getY_INLINE(),
					MOVE_IGNORE_DANGER, false, false, MISSIONAI_FOUND, pDestinationPlot);
			}
			else
			{
				// We can reach the destination with an amfib move this turn
				pTransportUnit->getGroup()->pushMission(MISSION_MOVE_TO, pDestinationPlot->getX_INLINE(), pDestinationPlot->getY_INLINE(),
					NO_MOVEMENT_FLAGS, false, false, MISSIONAI_FOUND, pDestinationPlot);
				getGroup()->pushMission(MISSION_SKIP);
				return true;
			}
		}
		else
		{
			pTransportUnit->getGroup()->pushMission(MISSION_MOVE_TO, pEndTurnPlot->getX_INLINE(), pEndTurnPlot->getY_INLINE(),
				NO_MOVEMENT_FLAGS, false, false, MISSIONAI_FOUND, pDestinationPlot);
			getGroup()->pushMission(MISSION_SKIP);
			return true;
		}
	}
	return false;

	/*
	if ((pBestPlot != NULL) && (pBestJoinPlot != NULL))
	{
		if (atPlot(pBestJoinPlot))
		{
			unload();
			return true;
		}
		else
		{ 
			getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_UNLOAD);
			AI_setMovePriority(1);
			return true;
		}
	}
	return false;
	*/
}

bool CvUnitAI::AI_betterJob()
{

	CvCity* const pCity = plot()->getPlotCity();
	if (pCity == NULL)
	{
		return false;
	}
	if (pCity->getOwnerINLINE() != getOwnerINLINE())
	{
		return false;
	}

	const ProfessionTypes eOriginalProfession = getProfession();

	// Check if the unit can have its original profession assigned when leaving the city
	// This prevents the crash that was reported in issue #394
	if (!canHaveProfession(eOriginalProfession, false, plot(), /*bForceCheck*/true))
	{
		FAssertMsg(false, "Illegal Profession");
		return false;
	}

	// TODO: Try to remove this
	pCity->AI_setWorkforceHack(true);

	std::vector<CvUnit*> units;
	const int iOriginalMovePriority = AI_getMovePriority();
	const UnitAITypes eOriginalAI = AI_getUnitAIType();

	bool bJoined=false;
	if (canJoinCity(plot()))
	{
		bJoined=true;
		pCity->addPopulationUnit(this, NO_PROFESSION);

		if (gUnitLogLevel >= 1)
		{
			logBBAI(" Player %S Unit %S better job joining city %S with profession %S", GET_PLAYER(getOwnerINLINE()).getCivilizationDescription(),
				getName().GetCString(), pCity->getName().GetCString(), getNameAndProfession().GetCString());
		}
	}

	CvPlot* const pPlot = plot();
	CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* const pLoopUnit = pPlot->getUnitNodeLoop(pUnitNode);

		if (pLoopUnit != NULL && pLoopUnit != this && pLoopUnit->getOwnerINLINE() == getOwnerINLINE())
		{
			if (!pLoopUnit->AI_hasAIChanged(5) && pLoopUnit->canJoinCity(pPlot))
			{
				const ProfessionTypes eLoopProfession = pLoopUnit->getProfession();

				// Defensive check since it has been observed that units with NO_PROFESSION can be in this list!
				// If this is the case then we'd end up with an AV further below when passing NO_PROFESSION to GC.getProfessionInfo
				// TODO: Consider adding an assert check to track down why this is happening
				if (eLoopProfession == NO_PROFESSION)
					continue;

				if (pLoopUnit->canHaveProfession(eOriginalProfession, true, NULL, /*bForceCheck*/true) && canHaveProfession(eLoopProfession, true, NULL, /*bForceCheck*/true))
				{
					units.push_back(pLoopUnit);
				}
			}
		}
	}

	for (int i = 0; i < pCity->getPopulation(); ++i)
	{
		CvUnit* const pLoopUnit = pCity->getPopulationUnitByIndex(i);
		if (pLoopUnit != this && pLoopUnit->getProfession() != NO_PROFESSION)
		{
			if (pLoopUnit != this)
			{
				if (!pLoopUnit->AI_hasAIChanged(5))
				{
					if (pLoopUnit->canHaveProfession(eOriginalProfession, true, NULL, /*bForceCheck*/true) && canHaveProfession(pLoopUnit->getProfession(), true, NULL, /*bForceCheck*/true))
					{
						units.push_back(pLoopUnit);
					}
				}
			}
		}
	}

	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	const int iSuitability = kOwner.AI_professionSuitability(this, eOriginalProfession, plot());

	int iBestValue = 0;
	CvUnit* pBestUnit = NULL;

	for (uint i = 0; i < units.size(); ++i)
	{
		CvUnit* const pLoopUnit = units[i];
		const ProfessionTypes eLoopProfession = pLoopUnit->getProfession();

		const CvPlot* const pLoopPlot = GC.getProfessionInfo(eLoopProfession).isWorkPlot() ? pCity->getPlotWorkedByUnit(pLoopUnit) : plot();
		int iOtherSuitability = kOwner.AI_professionSuitability(pLoopUnit, eOriginalProfession, pLoopPlot);
		int iValue = 0;

		if ((iSuitability < 100) && (iOtherSuitability >= 100))
		{
			iValue = iOtherSuitability - iSuitability;
		}

		const int iOriginalValue = iSuitability + kOwner.AI_professionSuitability(pLoopUnit, eLoopProfession, pLoopPlot);
		const int iNewValue = kOwner.AI_professionSuitability(this, eLoopProfession, pLoopPlot) + kOwner.AI_professionSuitability(pLoopUnit, eOriginalProfession, pLoopPlot);
		iValue = std::max(iValue, iNewValue - iOriginalValue);

		if (iValue > iBestValue)
		{
			iBestValue = iValue;
			pBestUnit = pLoopUnit;
		}
	}

	if (bJoined)
	{
		pCity->removePopulationUnit(CREATE_ASSERT_DATA, this, false, eOriginalProfession);
		AI_setUnitAIType(eOriginalAI);
		AI_setMovePriority(iOriginalMovePriority);

		if (gUnitLogLevel >= 1)
		{
			CvWString szTempString;
			getUnitAIString(szTempString, AI_getUnitAIType());

			logBBAI(" Player %S Unit %S better job leaving city with Profession %S and UnitAI (%S)", GET_PLAYER(getOwnerINLINE()).getCivilizationDescription(),
				getName().GetCString(), getNameAndProfession().GetCString(), szTempString.GetCString());
		}
	}

	if (pBestUnit != NULL)
	{
		kOwner.AI_swapUnitJobs(this, pBestUnit);
		m_iLastAIChangeTurn = GC.getGameINLINE().getGameTurn();
	}

	pCity->AI_setWorkforceHack(false);

	if (pBestUnit != NULL && hasMoved() && getGroup() != NULL)
	{
		getGroup()->pushMission(MISSION_SKIP);
		return true;
	}

	return (pBestUnit == NULL) ? false : true;
}

bool CvUnitAI::AI_upgradeProfession()
{
	if (getProfession() == NO_PROFESSION)
	{
		return false;
	}
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	ProfessionTypes eBestProfession = NO_PROFESSION;
	CvProfessionInfo& kCurrentProfession = GC.getProfessionInfo(getProfession());

	int iBestValue = 0;

	for (int i = 0; i < GC.getNumProfessionInfos(); ++i)
	{
		ProfessionTypes eLoopProfession = (ProfessionTypes)i;
		if (eLoopProfession != getProfession())
		{
			if (kCurrentProfession.isCitizen() == GC.getProfessionInfo(eLoopProfession).isCitizen())
			{
				if (canHaveProfession(eLoopProfession, false, plot()))
				{
					int iValue = kOwner.AI_professionValue(eLoopProfession, AI_getUnitAIType());
					if (iValue > iBestValue)
					{
						eBestProfession = eLoopProfession;
						iBestValue = iValue;
					}
				}
			}
		}
	}

	if (eBestProfession == NO_PROFESSION)
	{
		return false;
	}

	if (iBestValue <= kOwner.AI_professionValue(getProfession(), AI_getUnitAIType()))
	{
		return false;
	}

	UnitAITypes eCurrentAI = AI_getUnitAIType();
	setProfession(eBestProfession);
	AI_setUnitAIType(eCurrentAI);
	return true;
}

bool CvUnitAI::AI_changeUnitAIType(int iMinMultiplier)
{
	CvCity* pCity = plot()->getPlotCity();
	if (pCity == NULL || pCity->getOwnerINLINE() != getOwnerINLINE())
	{
		return false;
	}

	int iBestValue = 0;
	ProfessionTypes eBestProfession = NO_PROFESSION;
	UnitAITypes eBestUnitAI = NO_UNITAI;

	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());

	for (int iUnitAI = 0; iUnitAI < NUM_UNITAI_TYPES; ++iUnitAI)
	{
		UnitAITypes eLoopUnitAI = (UnitAITypes) iUnitAI;

		if (eLoopUnitAI != AI_getUnitAIType())
		{

			int iMultiplier = kOwner.AI_unitAIValueMultipler(eLoopUnitAI);

			if (iMultiplier > iMinMultiplier)
			{
				for (int iProfession = 0; iProfession < GC.getNumProfessionInfos(); ++iProfession)
				{
					ProfessionTypes eLoopProfession = (ProfessionTypes)iProfession;

					if (canHaveProfession(eLoopProfession, false, plot()))
					{
						int iProfessionValue = kOwner.AI_professionValue(eLoopProfession, eLoopUnitAI);
						if (iProfessionValue > 0)
						{
							iProfessionValue = kOwner.AI_professionSuitability(this, eLoopProfession, plot(), eLoopUnitAI);
							iProfessionValue /= 100;
							if (iProfessionValue > iMinMultiplier)
							{
								int iValue = iProfessionValue * iMultiplier;

								if (iValue > iBestValue)
								{
									iBestValue = iValue;
									eBestProfession = eLoopProfession;
									eBestUnitAI = eLoopUnitAI;
								}
							}
						}
					}
				}
			}
		}
	}

	if (eBestUnitAI != NO_UNITAI)
	{
		setProfession(eBestProfession);
		AI_setUnitAIType(eBestUnitAI);

		if (gUnitLogLevel >= 1)
		{
			CvWString szTempString;
			getUnitAIString(szTempString, AI_getUnitAIType());

			logBBAI(" Player %S Unit %S changes to Profession %S and UnitAI (%S)", GET_PLAYER(getOwnerINLINE()).getCivilizationDescription(),
				getName().GetCString(), getNameAndProfession().GetCString(), szTempString.GetCString());
		}


		return true;
	}
	return false;
}


bool CvUnitAI::AI_advance(bool bAttack)
{

	int iBestValue = MAX_INT;
	CvPlot* pBestPlot = NULL;
	CvTeamAI& kTeam = GET_TEAM(getTeam());
	for (int i = 0; i < NUM_DIRECTION_TYPES; ++i)
	{
		CvPlot* pDirectionPlot = plotDirection(getX_INLINE(), getY_INLINE(), (DirectionTypes)i);
		if (pDirectionPlot != NULL)
		{
			if (bAttack ? canMoveOrAttackInto(pDirectionPlot, false) : canMoveInto(*pDirectionPlot, false))
			{
				int iValue = kTeam.AI_enemyCityDistance(pDirectionPlot);
				if (iValue >= 0)
				{
					iValue *= 100;
					iValue -= GC.getGameINLINE().getSorenRandNum(50, "AI best advance plot");
					iValue -= std::min(50, pDirectionPlot->defenseModifier(getTeam()));

					if (iValue < iBestValue)
					{
						iBestValue = iValue;
						pBestPlot = pDirectionPlot;
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), 0, false, false);
		return true;
	}
	return false;
}

bool CvUnitAI::AI_loiter(int iMinDistance, int iMaxDistance, bool bAttack)
{
	int iBestValue = MAX_INT;
	CvPlot* pBestPlot = NULL;
	CvTeamAI& kTeam = GET_TEAM(getTeam());
	for (int iX = -1; iX <= 1; ++iX)
	{
		for (int iY = -1; iY <= 1; ++iY)
		{
			CvPlot* pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iX, iY);
			if (pLoopPlot != NULL)
			{
				if (atPlot(pLoopPlot) || (bAttack ? canMoveOrAttackInto(pLoopPlot, false) : canMoveInto(*pLoopPlot, false)))
				{
					int iEnemyDistance = kTeam.AI_enemyCityDistance(pLoopPlot);
					if (iEnemyDistance >= 0)
					{
						int iValue = 0;
						if (iEnemyDistance < iMinDistance)
						{
							iValue = iMinDistance - iEnemyDistance;
						}
						else if (iEnemyDistance > iMaxDistance)
						{
							iValue = iEnemyDistance - iMaxDistance;
						}
						iValue *= 100;
						iValue += 100;
						iValue -= GC.getGameINLINE().getSorenRandNum(50, "AI best loiter plot");
						iValue -= std::min(50, pLoopPlot->defenseModifier(getTeam()));

						if (iValue < iBestValue)
						{
							iBestValue = iValue;
							pBestPlot = pLoopPlot;
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		if (atPlot(pBestPlot))
		{
			getGroup()->pushMission(MISSION_SKIP);
		}
		else
		{
			getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), 0, false, false);
		}
		return true;
	}
	return false;
}

bool CvUnitAI::AI_retreat(int iMaxDistance)
{
	int iRange = iMaxDistance;

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;

	for (int iX = -iRange; iX <= iRange; iX++)
	{
		for (int iY = -iRange; iY <= iRange; iY++)
		{
			CvPlot* pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iX, iY);
			if ((pLoopPlot != NULL) && !atPlot(pLoopPlot))
			{
				if (pLoopPlot->isCity() && pLoopPlot->getOwnerINLINE() == getOwnerINLINE())
				{
					if (AI_plotValid(pLoopPlot))
					{
						int iPathTurns = 0;
						if (generatePath(pLoopPlot, 0, true, &iPathTurns))
						{
							int iValue = pLoopPlot->getPlotCity()->getHighestPopulation() * 100;

							iValue /= iPathTurns + 1;

							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestPlot = pLoopPlot;
							}
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), 0, false, false);
		return true;
	}
	return false;
}

bool CvUnitAI::AI_counter(int iTether)
{
	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvTeamAI& kTeam = GET_TEAM(getTeam());
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());

	for (int i = 0; i < NUM_DIRECTION_TYPES; ++i)
	{
		CvPlot* pDirectionPlot = plotDirection(getX_INLINE(), getY_INLINE(), (DirectionTypes)i);
		if (pDirectionPlot != NULL)
		{
			if (canMoveInto(*pDirectionPlot, false))
			{
				int iFriendlyCityDistance = kOwner.AI_cityDistance(pDirectionPlot);

				if (iFriendlyCityDistance <= iTether)
				{
					int iValue = 1 + GC.getGameINLINE().getSorenRandNum(100, "AI counter patrol");

					int iEnemyUnitDistance = kTeam.AI_enemyUnitDistance(pDirectionPlot);
					int iEnemyCityDistance = kTeam.AI_enemyCityDistance(pDirectionPlot);

					int iDefense = pDirectionPlot->defenseModifier(getTeam());

					if (iEnemyUnitDistance == 1)
					{
						if (iDefense == 0)
						{
							iValue /= 10;
						}
						iValue *= 100 + 4 * iDefense;

						iValue /= 2 + iFriendlyCityDistance;
					}
					else
					{
						iValue *= 100;
						iValue /= 1 + std::max(0, iEnemyUnitDistance);
						iValue /= 3 + iEnemyCityDistance;
					}

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestPlot = pDirectionPlot;
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), 0, false, false);
		return true;
	}
	return false;
}

bool CvUnitAI::AI_shouldRun()
{
	int iRange = 3;

	for (int iX = -iRange; iX <= iRange; ++iX)
	{
		for (int iY = -iRange; iY <= iRange; ++iY)
		{
			CvPlot* pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iX, iY);
			if (pLoopPlot != NULL && pLoopPlot->isVisible(getTeam(), false))
			{
				for (int i = 0; i < pLoopPlot->getNumUnits(); ++i)
				{
					CvUnit* pLoopUnit = pLoopPlot->getUnitByIndex(i);
					if (isEnemy(pLoopUnit->getTeam()))
					{
						if (pLoopUnit->canAttack() && pLoopUnit->getDomainType() == getDomainType())
						{

							int iTheirStrength = pLoopUnit->currCombatStr(NULL, NULL);
							int iTheirFirepower = pLoopUnit->currFirepower(NULL, NULL);

							if (iTheirStrength == 0)
							{
								return 1;
							}

							int iOurStrength = currCombatStr(pLoopPlot, pLoopUnit);
							int iOurFirepower = currFirepower(pLoopPlot, pLoopUnit);

							if (iTheirStrength * iTheirFirepower > iOurStrength * iOurFirepower)
							{
								return true;
							}
						}
					}
				}
			}
		}
	}
	return false;
}

//Fast routine for revealing water
bool CvUnitAI::AI_seaPatrol()
{
	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;

	for (int iDirection = 0; iDirection < NUM_DIRECTION_TYPES; iDirection++)
	{
		CvPlot* pLoopPlot = plotDirection(getX_INLINE(), getY_INLINE(), (DirectionTypes)iDirection);
		int iPathTurns;
		if (generatePath(pLoopPlot, 0, true, &iPathTurns))
		{
			if (iPathTurns <= 1)
			{
				int iValue = 150 / (1 + abs(pLoopPlot->getDistanceToOcean() - 3));
				iValue += GC.getGameINLINE().getSorenRandNum(50, "AI sea patrol");

				if (!isCardinalDirection((DirectionTypes)iDirection))
				{
					iValue *= 4;
					iValue /= 3;
				}

				if (pLoopPlot->isAdjacentToLand())
				{
					iValue /= 2;
				}

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = pLoopPlot;
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), 0, false, false, MISSIONAI_PIRACY, pBestPlot);
	}
	return false;
}

bool CvUnitAI::AI_moveTowardsOcean(int iRange)
{
	int iBestValue = MAX_INT;
	CvPlot* pBestPlot = NULL;
	for (int iX = -iRange; iX <= iRange; iX++)
	{
		for (int iY = -iRange; iY <= iRange; iY++)
		{
			if (iX != 0 || iY != 0)
			{
				CvPlot* pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iX, iY);
				if (pLoopPlot != NULL)
				{
					if (pLoopPlot->getDistanceToOcean() < plot()->getDistanceToOcean())
					{
						int iPathTurns;
						if (generatePath(pLoopPlot, MOVE_BUST_FOG, true, &iPathTurns) && iPathTurns <= 1)
						{
							int iPlotValue = 100 * pLoopPlot->getDistanceToOcean();
							iPlotValue += GC.getGameINLINE().getSorenRandNum(50, "AI move to ocean");

							if (iPlotValue < iBestValue)
							{
								iBestValue = iPlotValue;
								pBestPlot = pLoopPlot;
							}
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		pushGroupMoveTo(*pBestPlot, MOVE_BUST_FOG | MOVE_IGNORE_DANGER, false, false,
			MISSIONAI_DEFEND, pBestPlot);
		return true;
	}
	return false;
}

// TAC - AI Improved Naval AI - koma13 - START
bool CvUnitAI::AI_moveTowardsDanger(int iMaxPath)
{
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());

	CvPlot* pBestPlot = NULL;
	CvPlot* pBestMissionPlot = NULL;

	int iDanger;
	int iIncoming;

	int iRange = 0; //GC.getAI_TRANSPORT_DANGER_RANGE();
	int iBestPathTurns = MAX_INT;

	for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);

		if ((pLoopPlot != NULL) && AI_plotValid(pLoopPlot) && !atPlot(pLoopPlot))
		{
			int iPathTurns;
			if (generatePath(pLoopPlot, MOVE_THROUGH_ENEMY, true, &iPathTurns, iMaxPath))
			{
				iDanger = kOwner.AI_getWaterDanger(pLoopPlot, iRange, true, false, true);
				iIncoming = kOwner.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_PIRACY, getGroup());

				if ((iDanger > iIncoming) || ((iPathTurns < pLoopPlot->getDangerMap(getOwnerINLINE())) && (iIncoming == 0)))
				{
					if (iPathTurns < iBestPathTurns)
					{
						iBestPathTurns = iPathTurns;
						pBestPlot = (iPathTurns < 3) ? getGroup()->getPathFirstPlot() : getPathEndTurnPlot();
						pBestMissionPlot = pLoopPlot;
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		FAssert(!atPlot(pBestPlot));

		if (pBestMissionPlot == pBestPlot)
		{
			pBestMissionPlot->setDangerMap(getOwnerINLINE(), 0);
		}

		pushGroupMoveTo(*pBestPlot, MOVE_IGNORE_DANGER, false, false,
			MISSIONAI_COUNTER_PIRACY, pBestMissionPlot);
		return true;
	}

	return false;
}

bool CvUnitAI::AI_moveTowardsVictimCity()
{
	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestMissionPlot = NULL;

	// Erik: I've replaced the looping over all plots with a loop through all cities for efficiency
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
	{
		const CvPlayerAI& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);

		if (!kLoopPlayer.isAlive())
			continue;

		int iLoop;
		for (CvCity* pCity = kLoopPlayer.firstCity(&iLoop); pCity != NULL; pCity = kLoopPlayer.nextCity(&iLoop))
		{
			const CvPlot* const pCityPlot = pCity->plot();

			// Erik: Let's reduce the cheating a bit by only considering cities that we actually know about
			if (!pCityPlot->isRevealed(getTeam(), false))
				continue;

			// Erik: The old code checked for the victim player being on OUR OWN team, which had to be a mistake!
			if (pCity->getTeam() != getTeam() || isEnemy(pCity->getTeam()))
			{
				if (!pCity->isNative())
				{
					if (pCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
					{
						int iCityValue = 1 + GC.getGameINLINE().getSorenRandNum(pCity->getPopulation() * 1000, "AI Piracy City Target");
						for (int iDirection = 0; iDirection < NUM_DIRECTION_TYPES; iDirection++)
						{
							CvPlot* pDirectionPlot = plotDirection(pCityPlot->getX_INLINE(), pCityPlot->getY_INLINE(), (DirectionTypes)iDirection);
							if ((pDirectionPlot != NULL) && pDirectionPlot->isWater())
							{
								int iPathTurns;
								if (generatePath(pDirectionPlot, MOVE_THROUGH_ENEMY, true, &iPathTurns))
								{
									int iValue = iCityValue / (pDirectionPlot->getDistanceToOcean() + 1);
									iValue += pCityPlot->getCrumbs();
									iValue /= 2 + std::max(3, iPathTurns);
									if (iPathTurns < 3)
									{
										iValue /= 5 - iPathTurns;
									}
									CvPlayer& kCityOwner = GET_PLAYER(pCity->getOwnerINLINE());
									if (kCityOwner.countNumCoastalCitiesByArea(area()) == 1)
									{
										iValue *= 3;
										iValue /= 2;
									}

									if (iValue > iBestValue)
									{
										iBestValue = iValue;
										pBestPlot = getGroup()->getPathFirstPlot();
										pBestMissionPlot = pDirectionPlot;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		FAssert(!atPlot(pBestPlot));
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_IGNORE_DANGER, false, false, MISSIONAI_PIRACY, pBestMissionPlot);
		return true;
	}

	return false;
}



bool CvUnitAI::AI_spreadReligion()
{
	PROFILE_FUNC();

	FAssert(GC.getProfessionInfo(getProfession()).getMissionaryRate() > 0);

	// WTP, ray, AI imporevment - START
	// AI was running around with Missionaries and Native Traders too much
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());

	if (canEstablishMission())
	{
		establishMission();
		kOwner.AI_changeNumRetiredAIUnits(UNITAI_MISSIONARY, 1); // line moved
		return true;
	}
	// WTP, ray, AI imporevment - END

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestSpreadPlot = NULL;

	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
	{
		CvPlayerAI& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (kLoopPlayer.isAlive())
		{
			if (kLoopPlayer.canHaveMission(getOwnerINLINE()) && getMissionarySuccessPercent() > 50)
			{
				// Do not consider cities belonging to players that we have a war plan against
				if (GET_TEAM(getTeam()).AI_getWarPlan(kLoopPlayer.getTeam()) != NO_WARPLAN)
					continue;

				if (kLoopPlayer.AI_getAttitude(getOwnerINLINE()) >= ATTITUDE_ANNOYED)
				{
					int iLoop;
					for (CvCity* pLoopCity = kLoopPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kLoopPlayer.nextCity(&iLoop))
					{
						CvPlot* pLoopPlot = pLoopCity->plot();
						if (pLoopPlot->isRevealed(getTeam(), false))
						{
							if (pLoopPlot->getArea() == getArea())
							{
								if ((pLoopCity->getMissionaryCivilization() != getCivilizationType()) && (pLoopPlot->calculateCulturePercent(pLoopPlot->getOwnerINLINE()) == 100))
								{
									if (kOwner.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_SPREAD, getGroup()) == 0)
									{
										int iPathTurns;
										if (generatePath(pLoopPlot, MOVE_NO_ENEMY_TERRITORY, true, &iPathTurns))
										{
											if (iPathTurns < 10)
											{
												int iValue = pLoopCity->getPopulation();

												if (pLoopPlot->getCulture(getOwnerINLINE()) > 0)
												{
													iValue /= 4;
												}

												iValue *= 100 + GC.getGameINLINE().getSorenRandNum(25, "AI best spread plot");
												iValue /= 400 + getPathCost();
												if (iValue > iBestValue)
												{
													iBestValue = iValue;
													pBestSpreadPlot = pLoopPlot;
													pBestPlot = getPathEndTurnPlot();
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


	if ((pBestPlot != NULL) && (pBestSpreadPlot != NULL))
	{
		// WTP, ray, for safety I added this
		// I noticed that if 2 Missionaris are placed on a Native Village at the same time, both try to establish a Mission
		if (atPlot(pBestSpreadPlot) && canEstablishMission())
		{
			establishMission();
			kOwner.AI_changeNumRetiredAIUnits(UNITAI_MISSIONARY, 1);
			return true;
		}
		else
		{
			FAssert(!atPlot(pBestPlot));
			getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), 0, false, false, MISSIONAI_SPREAD, pBestSpreadPlot);
			return true;
		}
	}

	return false;
}

// WTP, ray, Native Trade Posts - START
bool CvUnitAI::AI_spreadTradePosts()
{
	PROFILE_FUNC();

	// WTP, ray, AI imporevment - START
	// AI was running around with Missionaries and Native Traders too much
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());

	if (canEstablishTradePost())
	{
		establishTradePost();
		kOwner.AI_changeNumRetiredAIUnits(UNITAI_TRADER, 1); // line moved
		return true;
	}
	// WTP, ray, AI imporevment - END

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestSpreadPlot = NULL;

	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
	{
		CvPlayerAI& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (kLoopPlayer.isAlive())
		{
			if (kLoopPlayer.canHaveTradePost(getOwnerINLINE()) && getNativeTradePostSuccessPercent() > 50)
			{
				// Do not consider cities belonging to players that we have a war plan against
				if (GET_TEAM(getTeam()).AI_getWarPlan(kLoopPlayer.getTeam()) != NO_WARPLAN)
					continue;

				if (kLoopPlayer.AI_getAttitude(getOwnerINLINE()) >= ATTITUDE_ANNOYED)
				{
					int iLoop;
					for (CvCity* pLoopCity = kLoopPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kLoopPlayer.nextCity(&iLoop))
					{
						CvPlot* pLoopPlot = pLoopCity->plot();
						if (pLoopPlot->isRevealed(getTeam(), false))
						{
							if (pLoopPlot->getArea() == getArea())
							{
								if ((pLoopCity->getTradePostCivilization() != getCivilizationType()) && (pLoopPlot->calculateCulturePercent(pLoopPlot->getOwnerINLINE()) == 100))
								{
									if (kOwner.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_SPREAD, getGroup()) == 0)
									{
										int iPathTurns;
										if (generatePath(pLoopPlot, MOVE_NO_ENEMY_TERRITORY, true, &iPathTurns))
										{
											if (iPathTurns < 10)
											{
												int iValue = pLoopCity->getPopulation();

												if (pLoopPlot->getCulture(getOwnerINLINE()) > 0)
												{
													iValue /= 4;
												}

												iValue *= 100 + GC.getGameINLINE().getSorenRandNum(25, "AI best spread plot");
												iValue /= 400 + getPathCost();
												if (iValue > iBestValue)
												{
													iBestValue = iValue;
													pBestSpreadPlot = pLoopPlot;
													pBestPlot = getPathEndTurnPlot();
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


	if ((pBestPlot != NULL) && (pBestSpreadPlot != NULL))
	{
		// WTP, ray, for safety I added this
		// I noticed that if 2 Native Traders are placed on a Native Village at the same time, both try to establish a Trade Post
		if (atPlot(pBestSpreadPlot) && canEstablishTradePost())
		{
			establishTradePost();
			kOwner.AI_changeNumRetiredAIUnits(UNITAI_TRADER, 1);
			return true;
		}
		else
		{
			FAssert(!atPlot(pBestPlot));
			getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), 0, false, false, MISSIONAI_SPREAD, pBestSpreadPlot);
			return true;
		}
	}

	return false;
}
// WTP, ray, Native Trade Posts - END




bool CvUnitAI::AI_learn(int iRange)
{
	FAssert(getUnitInfo().getLearnTime() >= 0);

	CvCity* pCity = plot()->getPlotCity();
	if (pCity != NULL)
	{
		if (!pCity->isScoutVisited(getTeam()))
		{
			if (canSpeakWithChief(plot()))
			{
				speakWithChief();
				FAssert(pCity->isScoutVisited(getTeam()));
			}
		}

		if (canLearn())
		{
			learn();
			return true;
		}
	}

	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;

	for (int iX = -iRange; iX <= iRange; iX++)
	{
		for (int iY = -iRange; iY <= iRange; iY++)
		{
			CvPlot* pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iX, iY);
			if (pLoopPlot != NULL)
			{
				CvCity* pPlotCity = pLoopPlot->getPlotCity();
				if (pPlotCity != NULL)
				{
					if (pPlotCity->getArea() == getArea())
					{
						if (GET_PLAYER(pPlotCity->getOwnerINLINE()).isNative() && pPlotCity->isScoutVisited(getTeam()))
						{
							// Do not consider cities belonging to players that we have a war plan against
							if (GET_TEAM(getTeam()).AI_getWarPlan(GET_PLAYER(pPlotCity->getOwnerINLINE()).getTeam()) != NO_WARPLAN)
								continue;

							if (!isEnemy(pPlotCity->getTeam()))
							{
								if (!pLoopPlot->isVisibleEnemyUnit(this))
								{
									UnitTypes eLearnUnitType = getLearnUnitType(pLoopPlot);
									if (eLearnUnitType != NO_UNIT)
									{
										int iProfessionSuitability = 100;
										if (AI_getOldProfession() != NO_PROFESSION)
										{
											iProfessionSuitability = kOwner.AI_professionSuitability(eLearnUnitType, AI_getOldProfession());
										}
										if (iProfessionSuitability > 100 || kOwner.getUnitClassCountPlusMaking(pPlotCity->getTeachUnitClass()) == 0)
										{
											if (iProfessionSuitability > 100 || kOwner.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_LEARN, getGroup()) == 0)
											{
												int iPathTurns;
												if (generatePath(pLoopPlot, MOVE_NO_ENEMY_TERRITORY, true, &iPathTurns))
												{
													int iValue = iProfessionSuitability;
													if (iProfessionSuitability > 100)
													{
														iValue *= 2;
													}

													iValue /= 5 + iPathTurns;

													if (iValue > iBestValue)
													{
														iBestValue = iValue;
														pBestPlot = pLoopPlot;
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
	}

	if (pBestPlot != NULL)
	{
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_NO_ENEMY_TERRITORY, false, false, MISSIONAI_LEARN, pBestPlot);
		return true;
	}
	return false;
}

bool CvUnitAI::AI_requestPickup(int iMaxPath)
{
	PROFILE_FUNC();

	//Are we ready to load?
	CvPlayerAI& kOwner= GET_PLAYER(getOwnerINLINE());

	if (getGroup()->AI_getMissionAIType() == MISSIONAI_AWAIT_PICKUP)
	{
		CvPlot* pMissionPlot =  getGroup()->AI_getMissionAIPlot();
		FAssert(pMissionPlot != NULL);

		if (kOwner.AI_plotTargetMissionAIs(pMissionPlot, MISSIONAI_PICKUP, NULL, 1) > 0)
		{
			if (AI_continueMission(0, MISSIONAI_AWAIT_PICKUP, MOVE_NO_ENEMY_TERRITORY, false))
			{
				return true;
			}
		}
	}

	CvPlot* pStartingPlot = GET_PLAYER(getOwnerINLINE()).getStartingPlot();
	if (pStartingPlot == NULL || !pStartingPlot->isWater())
	{
		return false;
	}

	CvPlot* pBestPlot = NULL;
	CvPlot* pBestMissionPlot = NULL;
	int iBestValue = MAX_INT;

	MissionAITypes paMissionAIs[] = {MISSIONAI_PICKUP, MISSIONAI_AWAIT_PICKUP};
	int iMissionAICount = 2;

	for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);

		if (pLoopPlot->getArea() == getArea())
		{
			CvArea* pWaterArea = pLoopPlot->waterArea();
			if (pWaterArea != NULL && pWaterArea->getNumAIUnits(getOwnerINLINE(), UNITAI_TRANSPORT_SEA) > 0)
			{
				int iOceanDist = pLoopPlot->getDistanceToOcean();
				if (iOceanDist < 20)
				{
					if (atPlot(pLoopPlot) || generatePath(pLoopPlot, 0, true))
					{
						int iValue = 300 + (atPlot(pLoopPlot) ? 0 : getPathCost());
						int iClosestTargetRange;
						int iCount = kOwner.AI_plotTargetMissionAIs(pLoopPlot, paMissionAIs, iMissionAICount, iClosestTargetRange, getGroup(), 1);
						iValue /= 1 + iCount;

						if (pLoopPlot->isFriendlyCity(*this, true))
						{
							iValue /= 2;
						}

						iValue *= iOceanDist;
						if (iValue < iBestValue)
						{
							iBestValue = iValue;
							pBestPlot = atPlot(pLoopPlot) ? pLoopPlot : getPathEndTurnPlot();
							pBestMissionPlot = pLoopPlot;
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		if (atPlot(pBestPlot))
		{
			getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_AWAIT_PICKUP, pBestMissionPlot);
		}
		else
		{
			getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), 0, false, false, MISSIONAI_AWAIT_PICKUP, pBestMissionPlot);
		}
		return true;
	}
	return false;
}

//This is very fast when no units need picking up.
bool CvUnitAI::AI_respondToPickup(int iMaxPath, UnitAITypes eUnitAI)
{
	PROFILE_FUNC();
	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestMissionPlot = NULL;

	CvArea* pWaterArea = plot()->waterArea();
	int iLoop;
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	for(CvSelectionGroup* pLoopSelectionGroup = kOwner.firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = kOwner.nextSelectionGroup(&iLoop))
	{
		if (pLoopSelectionGroup->getNumUnits() > 0)
		{
			if (pLoopSelectionGroup->AI_getMissionAIType() == MISSIONAI_AWAIT_PICKUP)
			{
				if ((eUnitAI == NO_UNITAI) || (pLoopSelectionGroup->getHeadUnit()->AI_getUnitAIType() == eUnitAI))
				{
					int iMaxPathTurns = iMaxPath;
					CvUnit* pHeadUnit = pLoopSelectionGroup->getHeadUnit();
					FAssert(pHeadUnit != NULL);

					CvPlot* pMissionPlot = pLoopSelectionGroup->AI_getMissionAIPlot();
					if (pMissionPlot != NULL)
					{
						if ((stepDistance(pMissionPlot->getX_INLINE(), pMissionPlot->getY_INLINE(), getX_INLINE(), getY_INLINE()) / std::max(1, baseMoves())) <= iMaxPathTurns)
						{

							bool bValid = true;

							{
								CvPlot* pPlot = plot();
								CLLNode<IDInfo>* pUnitNode = pLoopSelectionGroup->headUnitNode();

								int iCount = 0;
								while (pUnitNode != NULL)
								{
									CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
									pUnitNode = pLoopSelectionGroup->nextUnitNode(pUnitNode);

									if (pLoopUnit != NULL && !pLoopUnit->canLoadUnit(this, plot(), false))
									{
										bValid = false;
										break;
									}
								}
							}

							// TAC - AI Improved Naval AI - koma13 - START
							if ((pLoopSelectionGroup->getNumUnits() >= 3) && (cargoSpaceAvailable() < 3))
							{
								bValid = false;
							}
							// TAC - AI Improved Naval AI - koma13 - END

							int iBestDirectionValue = MAX_INT;

							if (bValid)
							{
								CvPlot* pLoopPlot = NULL;
								int iPathTurns = 0;
								if ((atPlot(pMissionPlot) || (canMoveInto(*pMissionPlot) && generatePath(pMissionPlot, MOVE_NO_ENEMY_TERRITORY, true, &iPathTurns))) && (iPathTurns <= iMaxPathTurns))
								{
									pLoopPlot = pMissionPlot;
								}
								else
								{
									for (int iDirection = 0; iDirection < NUM_DIRECTION_TYPES; iDirection++)
									{
										CvPlot* pDirectionPlot = plotDirection(pMissionPlot->getX_INLINE(), pMissionPlot->getY_INLINE(), (DirectionTypes)iDirection);
										if (pDirectionPlot != NULL)
										{
											if (pDirectionPlot->getArea() == pWaterArea->getID())
											{
												if (generatePath(pDirectionPlot, MOVE_NO_ENEMY_TERRITORY, true, &iPathTurns, iMaxPathTurns))
												{
													int iValue = getPathCost();

													if (iValue < iBestDirectionValue)
													{
														iBestDirectionValue = iValue;
														pLoopPlot = pDirectionPlot;
													}
												}
											}
										}
									}
								}
								if (pLoopPlot != NULL)
								{
									int iValue = 500;
									if (pHeadUnit->AI_getUnitAIType() == UNITAI_TREASURE)
									{
										iValue += pHeadUnit->getYieldStored();
									}

									iValue *= 1000;
									iValue /= 100 + ((iPathTurns == 0) ? 1 : iBestDirectionValue);

									int iDistance = stepDistance(pLoopSelectionGroup->getX(), pLoopSelectionGroup->getY(), pMissionPlot->getX_INLINE(), pMissionPlot->getY_INLINE());

									iValue *= 100;
									iValue /= 100 + ((100 * iDistance) / pLoopSelectionGroup->baseMoves());
									if (pHeadUnit->AI_getUnitAIType() == UNITAI_TREASURE)
									{
										iValue *= 3;
									}

									// TAC - AI Improved Naval AI - koma13 - START
									int iIncoming = GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_PICKUP, getGroup());
									int iMaxIncoming = (pLoopSelectionGroup->getNumUnits() > 3) ? (pLoopSelectionGroup->getNumUnits() / 3) : 0;

									if (iIncoming <= iMaxIncoming)
									{
									// TAC - AI Improved Naval AI - koma13 - END
										if (iValue > iBestValue)
										{
											iBestValue = iValue;
											pBestPlot = pLoopPlot;
											pBestMissionPlot = pLoopPlot;
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

	if (pBestPlot != NULL)
	{
		if (atPlot(pBestPlot))
		{
			int iOldCargo = getCargo();
			if (AI_pickupAdjacantUnits())
			{
				getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_PICKUP, pBestMissionPlot);
				return true;
			}
			if (getCargo() > iOldCargo)
			{
				return true;
			}
			return false;
		}
		else
		{
			getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_NO_ENEMY_TERRITORY, false, false, MISSIONAI_PICKUP, pBestMissionPlot);
			return true;
		}
	}
	return false;
}

bool CvUnitAI::AI_pickupAdjacantUnits()
{
	std::vector<CvUnit*> units;

	for (int iX = -1; iX <= 1; ++iX)
	{
		for (int iY = -1; iY <= 1; ++iY)
		{
			CvPlot* pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iX, iY);
			if (pLoopPlot != NULL)
			{
				CLLNode<IDInfo>* pUnitNode = pLoopPlot->headUnitNode();

				int iCount = 0;
				while (pUnitNode != NULL)
				{
					CvUnit* pLoopUnit = pLoopPlot->getUnitNodeLoop(pUnitNode);

					if (pLoopUnit != NULL && pLoopUnit->getGroup()->AI_getMissionAIType() == MISSIONAI_AWAIT_PICKUP && pLoopUnit->canLoadUnit(this, plot(), false))
					{
						units.push_back(pLoopUnit);
					}
				}
			}
		}
	}

	if (units.empty())
	{
		return false;
	}

	while (!units.empty() && !isFull())
	{
		CvUnit* pLoopUnit = units.back();
		units.pop_back();

		pLoopUnit->joinGroup(NULL);
		pLoopUnit->AI_setMovePriority(AI_getMovePriority() + 1);
		if (pLoopUnit->atPlot(plot()))
		{
			pLoopUnit->loadUnit(this);
		}
		else
		{
			pLoopUnit->getGroup()->pushMission(MISSION_MOVE_TO, getX_INLINE(), getY_INLINE());
		}
	}
	return false;
}

bool CvUnitAI::AI_continueMission(int iAbortDistance, MissionAITypes eValidMissionAI, int iFlags, bool bStepwise)
{
	CvPlot* pMissionPlot = getGroup()->AI_getMissionAIPlot();
	MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();
	if (pMissionPlot == NULL || eMissionAI == NO_MISSIONAI)
	{
		return false;
	}

	if (eMissionAI != NO_MISSIONAI && eMissionAI != eValidMissionAI)
	{
		return false;
	}

	if (stepDistance(getX_INLINE(), getY_INLINE(), pMissionPlot->getX_INLINE(), pMissionPlot->getY_INLINE()) <= iAbortDistance)
	{
		return false;
	}

	if (atPlot(pMissionPlot))
	{
		return false;
	}
	else if (generatePath(pMissionPlot, iFlags, true))
	{
		CvPlot* pNextPlot = bStepwise ? getGroup()->getPathFirstPlot() : getGroup()->getPathEndTurnPlot();
		if ((pNextPlot == NULL) || atPlot(pNextPlot))
		{
			return false;
		}

		getGroup()->pushMission(MISSION_MOVE_TO, pNextPlot->getX_INLINE(), pNextPlot->getY_INLINE(), iFlags, false, false, eMissionAI, pMissionPlot);
		return true;
	}

	return false;
}

bool CvUnitAI::AI_breakAutomation()
{
	if (!isAutomated())
	{
		return false;
	}

	bool bBreak = false;
	if ((getGroup()->getAutomateType() == AUTOMATE_TRANSPORT_ROUTES) || (getGroup()->getAutomateType() == AUTOMATE_TRANSPORT_FULL))
	{
		CLLNode<IDInfo>* pUnitNode = plot()->headUnitNode();
		CvUnit* pLoopUnit;
		while (pUnitNode != NULL)
		{
			pLoopUnit = plot()->getUnitNodeLoop(pUnitNode);
			if (pLoopUnit != NULL && pLoopUnit->isCargo())
			{
				if (pLoopUnit->getTransportUnit()->getGroup() == getGroup())
				{
					if ((pLoopUnit->getYield() == NO_YIELD) || (pLoopUnit->AI_getUnitAIState() == UNITAI_STATE_PURCHASED))
					{
						bBreak = true;
					}
				}
			}
		}
	}

	if (!bBreak && GET_PLAYER(getOwnerINLINE()).AI_getUnitDanger(this, 2, false, false))
	{
		// TAC - Trade Routes Advisor - koma13 - START
		//bBreak = true;
		if (!isIgnoreDanger())
		{
			bBreak = true;
		}
		// TAC - Trade Routes Advisor - koma13 - END
	}

	if (bBreak)
	{
		getGroup()->setAutomateType(NO_AUTOMATE);
		return true;
	}
	return false;
}

// TAC - AI Assault Sea - koma13, jdog5000(BBAI) - START
/*
// Returns true if a group was joined or a mission was pushed...
bool CvUnitAI::AI_group(UnitAITypes eUnitAI, int iMaxGroup, int iMaxOwnUnitAI, int iMinUnitAI, bool bIgnoreFaster, bool bIgnoreOwnUnitType, bool bStackOfDoom, int iMaxPath, bool bAllowRegrouping)
{
	PROFILE_FUNC();

	CvUnit* pLoopUnit;
	CvUnit* pBestUnit;
	int iPathTurns;
	int iValue;
	int iBestValue;
	int iLoop;

	// if we are on a transport, then do not regroup
	if (isCargo())
	{
		return false;
	}

	if (!bAllowRegrouping)
	{
		if (getGroup()->getNumUnits() > 1)
		{
			return false;
		}
	}

	if ((getDomainType() == DOMAIN_LAND) && !m_pUnitInfo->isCanMoveAllTerrain())
	{
		if (area()->getNumAIUnits(getOwnerINLINE(), eUnitAI) == 0)
		{
			return false;
		}
	}

	if (!AI_canGroupWithAIType(eUnitAI))
	{
		return false;
	}

	iBestValue = MAX_INT;
	pBestUnit = NULL;

	CvSelectionGroup* pThisGroup = getGroup();
	for(pLoopUnit = GET_PLAYER(getOwnerINLINE()).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(getOwnerINLINE()).nextUnit(&iLoop))
	{
		CvSelectionGroup* pLoopGroup = pLoopUnit->getGroup();
		CvPlot* pPlot = pLoopUnit->plot();
		if (AI_plotValid(pPlot))
		{
			if (iMaxPath > 0 || pPlot == plot())
			{
				if (!isEnemy(pPlot->getTeam()))
				{
					if (AI_allowGroup(pLoopUnit, eUnitAI))
					{
						if ((iMaxGroup == -1) || ((pLoopGroup->getNumUnits() + GET_PLAYER(getOwnerINLINE()).AI_unitTargetMissionAIs(pLoopUnit, MISSIONAI_GROUP, getGroup())) <= (iMaxGroup + ((bStackOfDoom) ? AI_stackOfDoomExtra() : 0))))
						{
							if ((iMaxOwnUnitAI == -1) || (pLoopGroup->countNumUnitAIType(AI_getUnitAIType()) <= (iMaxOwnUnitAI + ((bStackOfDoom) ? AI_stackOfDoomExtra() : 0))))
							{
								if ((iMinUnitAI == -1) || (pLoopGroup->countNumUnitAIType(eUnitAI) >= iMinUnitAI))
								{
									if (!bIgnoreFaster || (pLoopUnit->getGroup()->baseMoves() <= baseMoves()))
									{
										if (!(pLoopUnit->plot()->isVisibleEnemyUnit(this)))
										{
											if (!bIgnoreOwnUnitType || (pLoopUnit->getUnitType() != getUnitType()))
											{
												if (!(pPlot->isVisibleEnemyUnit(this)))
												{
													if (generatePath(pPlot, 0, true, &iPathTurns))
													{
														if (iPathTurns <= iMaxPath)
														{
															iValue = 1000 * (iPathTurns + 1);
															iValue *= 4 + pLoopGroup->getCargo();
															iValue /= pLoopGroup->getNumUnits();


															if (iValue < iBestValue)
															{
																iBestValue = iValue;
																pBestUnit = pLoopUnit;
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
			}
		}
	}

	if (pBestUnit != NULL)
	{
		if (atPlot(pBestUnit->plot()))
		{
			joinGroup(pBestUnit->getGroup());
			return true;
		}
		else
		{
			getGroup()->pushMission(MISSION_MOVE_TO_UNIT, pBestUnit->getOwnerINLINE(), pBestUnit->getID(), 0, false, false, MISSIONAI_GROUP, NULL, pBestUnit);
			return true;
		}
	}

	return false;
}
*/

// Added new options to aid transport grouping
// Returns true if a group was joined or a mission was pushed...
bool CvUnitAI::AI_group(UnitAITypes eUnitAI, int iMaxGroup, int iMaxOwnUnitAI, int iMinUnitAI, bool bIgnoreFaster, bool bIgnoreOwnUnitType, bool bStackOfDoom, int iMaxPath, bool bAllowRegrouping, bool bWithCargoOnly, bool bInCityOnly, MissionAITypes eIgnoreMissionAIType)
{
	PROFILE_FUNC();

	CvUnit* pLoopUnit;
	CvUnit* pBestUnit;
	int iPathTurns;
	int iValue;
	int iBestValue;
	int iLoop;

	// if we are on a transport, then do not regroup
	if (isCargo())
	{
		return false;
	}

	if (!bAllowRegrouping)
	{
		if (getGroup()->getNumUnits() > 1)
		{
			return false;
		}
	}

	if (getDomainType() == DOMAIN_LAND)
	{
		if (area()->getNumAIUnits(getOwnerINLINE(), eUnitAI) == 0)
		{
			return false;
		}
	}

	if (!AI_canGroupWithAIType(eUnitAI))
	{
		return false;
	}

	int iOurImpassableCount = 0;
	CLLNode<IDInfo>* pUnitNode = getGroup()->headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pImpassUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = getGroup()->nextUnitNode(pUnitNode);

		if (pImpassUnit != NULL)
		{
			iOurImpassableCount = std::max(iOurImpassableCount, GET_PLAYER(getOwnerINLINE()).AI_unitImpassableCount(pImpassUnit->getUnitType()));
		}
	}

	iBestValue = MAX_INT;
	pBestUnit = NULL;

	// Loop over groups, ai_allowgroup blocks non-head units anyway
	CvSelectionGroup* pLoopGroup = NULL;
	for(pLoopGroup = GET_PLAYER(getOwnerINLINE()).firstSelectionGroup(&iLoop); pLoopGroup != NULL; pLoopGroup = GET_PLAYER(getOwnerINLINE()).nextSelectionGroup(&iLoop))
	{
		pLoopUnit = pLoopGroup->getHeadUnit();
		if( pLoopUnit == NULL )
		{
			continue;
		}

		CvPlot* pPlot = pLoopUnit->plot();
		if (AI_plotValid(pPlot))
		{
			if (iMaxPath > 0 || pPlot == plot())
			{
				if (!isEnemy(pPlot->getTeam()))
				{
					if (AI_allowGroup(pLoopUnit, eUnitAI))
					{
						if ((iMaxGroup == -1) || ((pLoopGroup->getNumUnits() + GET_PLAYER(getOwnerINLINE()).AI_unitTargetMissionAIs(pLoopUnit, MISSIONAI_GROUP, getGroup())) <= (iMaxGroup + ((bStackOfDoom) ? AI_stackOfDoomExtra() : 0))))
						{
							// TAC - AI Attack City - koma13 - START
							//if ((iMaxOwnUnitAI == -1) || (pLoopGroup->countNumUnitAIType(AI_getUnitAIType()) <= (iMaxOwnUnitAI + ((bStackOfDoom) ? AI_stackOfDoomExtra() : 0))))
							if ((iMaxOwnUnitAI == -1) || ((pLoopGroup->countNumUnitAIType(AI_getUnitAIType()) + GET_PLAYER(getOwnerINLINE()).AI_unitTargetMissionAIs(pLoopUnit, MISSIONAI_GROUP, getGroup(), AI_getUnitAIType())) <= (iMaxOwnUnitAI + ((bStackOfDoom) ? AI_stackOfDoomExtra() : 0))))
							// TAC - AI Attack City - koma13 - END
							{
								if ((iMinUnitAI == -1) || (pLoopGroup->countNumUnitAIType(eUnitAI) >= iMinUnitAI))
								{
									if (!bIgnoreFaster || (pLoopGroup->baseMoves() <= baseMoves()))
									{
										if (!bIgnoreOwnUnitType || (pLoopUnit->getUnitType() != getUnitType()))
										{
											if (!bWithCargoOnly || pLoopUnit->getGroup()->hasCargo())
											{
												if( !bInCityOnly || pLoopUnit->plot()->isCity() )
												{
													if( (eIgnoreMissionAIType == NO_MISSIONAI) || (eIgnoreMissionAIType != pLoopUnit->getGroup()->AI_getMissionAIType()) )
													{
														if (!(pPlot->isVisibleEnemyUnit(this)))
														{
															if( iOurImpassableCount > 0 || AI_getUnitAIType() == UNITAI_ASSAULT_SEA )
															{
																int iTheirImpassableCount = 0;
																pUnitNode = pLoopGroup->headUnitNode();
																while (pUnitNode != NULL)
																{
																	CvUnit* pImpassUnit = ::getUnit(pUnitNode->m_data);
																	pUnitNode = pLoopGroup->nextUnitNode(pUnitNode);

																	if (pLoopUnit != NULL)
																	{
																		iTheirImpassableCount = std::max(iTheirImpassableCount, GET_PLAYER(getOwnerINLINE()).AI_unitImpassableCount(pImpassUnit->getUnitType()));
																	}
																}

																if( iOurImpassableCount != iTheirImpassableCount )
																{
																	continue;
																}
															}

															if (generatePath(pPlot, 0, true, &iPathTurns, iMaxPath))
															{
																iValue = 1000 * (iPathTurns + 1);
																iValue *= 4 + pLoopGroup->getCargo();
																iValue /= pLoopGroup->getNumUnits();

																if (iValue < iBestValue)
																{
																	iBestValue = iValue;
																	pBestUnit = pLoopUnit;
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
				}
			}
		}
	}

	if (pBestUnit != NULL)
	{
		if (atPlot(pBestUnit->plot()))
		{
			joinGroup(pBestUnit->getGroup());
			return true;
		}
		else
		{
			getGroup()->pushMission(MISSION_MOVE_TO_UNIT, pBestUnit->getOwnerINLINE(), pBestUnit->getID(), 0, false, false, MISSIONAI_GROUP, NULL, pBestUnit);
			return true;
		}
	}

	return false;
}
// TAC - AI Assault Sea - koma13, jdog5000(BBAI) - END

bool CvUnitAI::AI_groupMergeRange(UnitAITypes eUnitAI, int iMaxRange, bool bBiggerOnly, bool bAllowRegrouping, bool bIgnoreFaster)
{
	PROFILE_FUNC();


 	// if we are on a transport, then do not regroup
	if (isCargo())
	{
		return false;
	}

   if (!bAllowRegrouping)
	{
		if (getGroup()->getNumUnits() > 1)
		{
			return false;
		}
	}

	if ((getDomainType() == DOMAIN_LAND) && !m_pUnitInfo->isCanMoveAllTerrain())
	{
		if (area()->getNumAIUnits(getOwnerINLINE(), eUnitAI) == 0)
		{
			return false;
		}
	}

	if (!AI_canGroupWithAIType(eUnitAI))
	{
		return false;
	}

	// cached values
	CvPlot* pPlot = plot();
	CvSelectionGroup* pGroup = getGroup();

	// best match
	CvUnit* pBestUnit = NULL;
	int iBestValue = MAX_INT;
	// iterate over plots at each range
	for (int iDX = -(iMaxRange); iDX <= iMaxRange; iDX++)
	{
		for (int iDY = -(iMaxRange); iDY <= iMaxRange; iDY++)
		{
			CvPlot* pLoopPlot = plotXY(pPlot->getX_INLINE(), pPlot->getY_INLINE(), iDX, iDY);

			if (pLoopPlot != NULL && pLoopPlot->getArea() == pPlot->getArea())
			{
				CLLNode<IDInfo>* pUnitNode = pLoopPlot->headUnitNode();
				while (pUnitNode != NULL)
				{
					CvUnit* pLoopUnit = pLoopPlot->getUnitNodeLoop(pUnitNode);

					if (pLoopUnit != NULL)
					{
						CvSelectionGroup* pLoopGroup = pLoopUnit->getGroup();

						if (AI_allowGroup(pLoopUnit, eUnitAI))
						{
							if (!bIgnoreFaster || (pLoopUnit->getGroup()->baseMoves() <= baseMoves()))
							{
								if (!bBiggerOnly || (pLoopGroup->getNumUnits() >= pGroup->getNumUnits()))
								{
									int iPathTurns;
									if (generatePath(pLoopPlot, 0, true, &iPathTurns))
									{
										if (iPathTurns <= (iMaxRange + 2))
										{
											int iValue = 1000 * (iPathTurns + 1);
											iValue /= pLoopGroup->getNumUnits();

											if (iValue < iBestValue)
											{
												iBestValue = iValue;
												pBestUnit = pLoopUnit;
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

	if (pBestUnit != NULL)
	{
		if (atPlot(pBestUnit->plot()))
		{
			pGroup->mergeIntoGroup(pBestUnit->getGroup());
			return true;
		}
		else
		{
			pGroup->pushMission(MISSION_MOVE_TO_UNIT, pBestUnit->getOwnerINLINE(), pBestUnit->getID(), 0, false, false, MISSIONAI_GROUP, NULL, pBestUnit);
			return true;
		}
	}

	return false;
}

// Returns true if we loaded onto a transport or a mission was pushed...
bool CvUnitAI::AI_load(UnitAITypes eUnitAI, MissionAITypes eMissionAI,
	UnitAITypes eTransportedUnitAI, int iMinCargo, int iMinCargoSpace,
	int iMaxCargoSpace, int iMaxCargoOurUnitAI, MovementFlags eFlags,
	int iMaxPath,
	/*  BETTER_BTS_AI_MOD, War tactics AI, Unit AI, 04/18/10, jdog5000
		(and various changes in the body) */  // advc: Restructured (untangled) the body a bit
	int iMaxTransportPath)
{
	PROFILE_FUNC();

	if (getCargo() > 0)
	{
		return false;
	}

	if (isCargo())
	{
		getGroup()->pushMission(MISSION_SKIP);
		return true;
	}
	CvUnit* pBestUnit = AI_findTransport(eUnitAI, eFlags, iMaxPath, eTransportedUnitAI, iMinCargo, iMinCargoSpace, iMaxCargoSpace, iMaxCargoOurUnitAI); // K-Mod
	//if( pBestUnit != NULL && iMaxTransportPath < MAX_INT )
	if (pBestUnit != NULL && iMaxTransportPath < MAX_INT && (eUnitAI == UNITAI_ASSAULT_SEA)) // K-Mod
	{
		// Can transport reach enemy in requested time
		bool bFoundEnemyPlotInRange = false;
		int iPathTurns;
		int iRange = iMaxTransportPath * pBestUnit->baseMoves();
		CvPlot* pAdjacentPlot = NULL;
		// K-Mod. use a separate pathfinder for the transports, so that we don't reset our current path data.
		KmodPathFinder temp_finder;
		temp_finder.SetSettings(CvPathSettings(pBestUnit->getGroup(), eFlags & MOVE_DECLARE_WAR, iMaxTransportPath, GLOBAL_DEFINE_MOVE_DENOMINATOR));
		//

		for (int iDX = -iRange; (iDX <= iRange && !bFoundEnemyPlotInRange); iDX++)
		{
			for (int iDY = -iRange; (iDY <= iRange && !bFoundEnemyPlotInRange); iDY++)
			{
				//CvPlot* pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);
				CvPlot* pLoopPlot = plotXY(pBestUnit->getX_INLINE(), pBestUnit->getY_INLINE(), iDX, iDY); // K-Mod

				if (pLoopPlot != NULL)
				{
					if (pLoopPlot->isCoastalLand())
					{
						if (pLoopPlot->isOwned())
						{
							if (isPotentialEnemy(pLoopPlot->getTeam(), pLoopPlot) && !isBarbarian())
							{
								if (pLoopPlot->area()->getCitiesPerPlayer(pLoopPlot->getOwnerINLINE()) > 0)
								{
									// Transport cannot enter land plot without cargo, so generate path only works properly if
									// land units are already loaded

									for (int iI = 0; (iI < NUM_DIRECTION_TYPES && !bFoundEnemyPlotInRange); iI++)
									{
										pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), (DirectionTypes)iI);
										if (pAdjacentPlot != NULL)
										{
											if (pAdjacentPlot->isWater())
											{
												//if( pBestUnit->generatePath(pAdjacentPlot, 0, true, &iPathTurns, iMaxTransportPath) )
												if (temp_finder.GeneratePath(pAdjacentPlot))
												{
													/* if (pBestUnit->getPathLastNode()->m_iData1 == 0)
													{
														iPathTurns++;
													}*/
													iPathTurns = temp_finder.GetPathTurns() + (temp_finder.GetFinalMoves() == 0 ? 1 : 0); // K-Mod

													if (iPathTurns <= iMaxTransportPath)
													{
														bFoundEnemyPlotInRange = true;
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

		if (!bFoundEnemyPlotInRange)
		{
			pBestUnit = NULL;
		}
	}

	if (pBestUnit != NULL)
	{
		if (atPlot(pBestUnit->plot()))
		{
			CvSelectionGroup* pRemainderGroup = NULL; // K-Mod renamed from 'pOtherGroup'
			getGroup()->setTransportUnit(pBestUnit, &pRemainderGroup); // XXX is this dangerous (not pushing a mission...) XXX air units?

			// If part of large group loaded, then try to keep loading the rest
			if (eUnitAI == UNITAI_ASSAULT_SEA && eMissionAI == MISSIONAI_LOAD_ASSAULT)
			{
				if (pRemainderGroup != NULL && pRemainderGroup->getNumUnits() > 0)
				{
					if (pRemainderGroup->getHeadUnitAI() == AI_getUnitAIType())
					{
						if (pRemainderGroup->getHeadUnit()->AI().AI_load(eUnitAI, eMissionAI, eTransportedUnitAI, iMinCargo, iMinCargoSpace, iMaxCargoSpace, iMaxCargoOurUnitAI, eFlags, 0, iMaxTransportPath))
							pRemainderGroup->AI().AI_setForceSeparate(false); // K-Mod
					}
					else if (eTransportedUnitAI == NO_UNITAI && iMinCargo < 0 && iMinCargoSpace < 0 && iMaxCargoSpace < 0 && iMaxCargoOurUnitAI < 0)
					{
						if (pRemainderGroup->getHeadUnit()->AI().AI_load(eUnitAI, eMissionAI, NO_UNITAI, -1, -1, -1, -1, eFlags, 0, iMaxTransportPath))
							pRemainderGroup->AI().AI_setForceSeparate(false); // K-Mod
					}
				}
			}
			// K-Mod - just for efficiency, I'll take care of the force separate stuff here.
			if (pRemainderGroup && pRemainderGroup->AI_isForceSeparate())
				pRemainderGroup->AI_separate();
			// K-Mod end

			return true;
		}
		else
		{
			// BBAI TODO: To split or not to split?
			// K-Mod. How about we this:
			// Split the group only if it is going to take more than 1 turn to get to the transport.
			if (generatePath(pBestUnit->plot(), eFlags, true, 0, 1))
			{
				// only 1 turn. Don't split.
				getGroup()->pushMission(MISSION_MOVE_TO_UNIT, pBestUnit->getOwnerINLINE(), pBestUnit->getID(), eFlags, false, false, eMissionAI, NULL, pBestUnit);
				return true;
			}
			else
			{
				// (bbai code. split the group)
				int iCargoSpaceAvailable = pBestUnit->cargoSpaceAvailable(getSpecialUnitType(), getDomainType());
				FAssertMsg(iCargoSpaceAvailable > 0, "best unit has no space");

				// split our group to fit on the transport
				CvSelectionGroup* pRemainderGroup = NULL;
				CvSelectionGroup* pSplitGroup = getGroup()->splitGroup(iCargoSpaceAvailable, this, &pRemainderGroup);
				FAssertMsg(pSplitGroup, "splitGroup failed");
				FAssertMsg(getGroupID() == pSplitGroup->getID(), "splitGroup failed to put head unit in the new group");

				if (pSplitGroup != NULL)
				{
					CvPlot* pOldPlot = pSplitGroup->plot();
					pSplitGroup->pushMission(MISSION_MOVE_TO_UNIT, pBestUnit->getOwnerINLINE(), pBestUnit->getID(), eFlags, false, false, eMissionAI, NULL, pBestUnit);
					/* bool bMoved = (pSplitGroup->plot() != pOldPlot);
					if (!bMoved && pOtherGroup != NULL)
					{
						joinGroup(pOtherGroup);
					}
					return bMoved;
					*/ // K-Mod. (that block is obsolete)
					// K-Mod - just for efficiency, I'll take care of the force separate stuff here.
					if (pRemainderGroup && pRemainderGroup->AI_isForceSeparate())
						pRemainderGroup->AI_separate();
					// K-Mod end
					return true;
				}
			}
			// K-Mod end
		}
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_guardCityBestDefender()
{
	CvCity* pCity;
	CvPlot* pPlot;

	pPlot = plot();
	pCity = pPlot->getPlotCity();

	if (pCity != NULL)
	{
		if (pCity->getOwnerINLINE() == getOwnerINLINE())
		{
			if (pPlot->getBestDefender(getOwnerINLINE()) == this)
			{
				getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_CITY, NULL);
				return true;
			}
		}
	}

	return false;
}

/*	K-Mod. This function was so full of useless cruft
	and duplicated code and double-counting mistakes...
	I've deleted the bulk of the old code, and rewritten it
	to be much much simpler - and also better. */
bool CvUnitAI::AI_guardCity(bool bLeave, bool bSearch, int iMaxPath, MovementFlags eFlags)
{
	PROFILE_FUNC();

	FAssert(getDomainType() == DOMAIN_LAND);
	FAssert(canDefend());

	CvPlot const* pEndTurnPlot = NULL;
	CvPlot const* pBestGuardPlot = NULL;

	CvPlot const& kPlot = getPlot();
	CvCityAI const* pCity = kPlot.AI_getPlotCity();
	CvPlayerAI const& kOwner = GET_PLAYER(getOwner());
	if (pCity != NULL && pCity->getOwner() == getOwner())
	{
		int iExtra = -1; // additional defenders needed.
		if (!bLeave || pCity->AI_isDanger())
			iExtra = (bSearch ? 0 : kOwner.AI_getPlotDangerInternal(&kPlot, 2));

		int const iHave = kPlot.plotCount(PUF_canDefendGroupHead, -1, -1, kOwner.getID(),
			// -1 because this unit is being counted as a defender
			NO_TEAM, AI_isCityAIType() ? PUF_isCityAIType : NULL) - 1;

#if 0
		/*	<advc.052> Code added to CvCityAI allows a settler at size 2, but the
			AI often doesn't have an escort available that early. Let's say that
			one defender in the first city is OK if we haven't met a human yet. */
		if (iExtra < 0 && iHave > 0 && kOwner.getNumCities() == 1 &&
			TeamIter<HUMAN, OTHER_KNOWN_TO>::count(getTeam()) <= 0)
		{
			if (AI_group(UNITAI_SETTLE, /*iMaxGroup*/ 1, -1, -1, false, false, false,
				/*iMaxPath*/ 0, /*bAllowRegrouping*/ true))
			{
				return true;
			}
		} // </advc.052>
#endif
		int iNeed = pCity->AI_neededDefenders() + iExtra;
		if (iHave < iNeed)
		{	// don't bother searching. We're staying here.
			bSearch = false;
			pEndTurnPlot = &kPlot;
			pBestGuardPlot = &kPlot;
		}
	}

	if (bSearch)
	{
		int iBestValue = 0;
		//bool const bMoveAllTerrain = getGroup()->canMoveAllTerrain(); // advc
		FOR_EACH_CITYAI(pLoopCity, kOwner) // advc: Flattened the body of this loop
		{
			/*if (!AI_plotValid(pLoopCity->plot()))
				continue;*/
				// advc.opt: This function is only called for land units; the BBAI check suffices.
				// BBAI efficiency: check area for land units
				/*if(getDomainType() == DOMAIN_LAND)
				if (!isArea(pLoopCity->getArea()) && !bMoveAllTerrain)*/
				// advc.030: Replacing the BBAI check
			if (!AI_canEnterByLand(*pLoopCity->area()))
				continue;

			//if (!pLoopCity->AI_isDefended((!AI_isCityAIType() ? pLoopCity->getPlot().plotCount(PUF_canDefendGroupHead, -1, -1, getOwner(), NO_TEAM, PUF_isNotCityAIType) : 0)))
			// K-Mod
			int iDefendersNeeded = pLoopCity->AI_neededDefenders(/*true*/);
			int iDefendersHave = pLoopCity->getPlot().plotCount(
				PUF_canDefendGroupHead, -1, -1, getOwner(),
				NO_TEAM, AI_isCityAIType() ? PUF_isCityAIType : NULL);
			if (pCity == pLoopCity)
				iDefendersHave -= getGroup()->getNumUnits();
			// K-Mod end
			/*  <advc.139> Reinforce city despite evac if group large enough.
				Don't want cities to be abandoned unnecessarily just b/c few units
				were garrisoned when the enemy stack arrived, but don't want them
				to move in and out either.
				CvCityAI::AI_updateSafety already checks for potential defenders
				within 3 tiles of the city. If this stack is farther away than that,
				it'll probably not arrive in time to save the city, but it might,
				or could quickly retake the city. */
			int iDelta = iDefendersNeeded - iDefendersHave;
			if (iDelta <= 0)
				continue; // No functional change from BtS
#if 0
			if (pLoopCity->AI_isEvacuating() &&
				iDelta > fixp(0.75) * getGroup()->getNumUnits())
			{
				continue;
			} // </advc.139>
#endif
			/*if (pLoopCity->getPlot().isVisibleEnemyUnit(this)) // advc.opt: It's our city
				continue;*/

			if (GC.getGame().getGameTurn() - pLoopCity->getGameTurnAcquired() >= 10 &&
				kOwner.AI_plotTargetMissionAIsEx(pLoopCity->getPlot(),
					MISSIONAI_GUARD_CITY, getGroup(), /*<advc.opt>*/ 0, 2 /*</advc.opt>*/) >= 2)
			{
				continue;
			}
			int iPathTurns;
			if (at(pLoopCity->getPlot()) || !generatePath(&pLoopCity->getPlot(),
				eFlags, true, &iPathTurns, iMaxPath))
			{
				continue;
			}
			if (iPathTurns > iMaxPath)
				continue;

			int iValue = 1000 * (1 + iDefendersNeeded - iDefendersHave);
			iValue /= 1 + iPathTurns + iDefendersHave;
			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pEndTurnPlot = getPathEndTurnPlot();
				pBestGuardPlot = pLoopCity->plot();
				FAssert(!atPlot(pEndTurnPlot));
				if (iMaxPath == 1 || iBestValue >= 500)
					break; // we found a good city. No need to waste any more time looking.
			}
		}
	}

	if (pEndTurnPlot == NULL || pBestGuardPlot == NULL)
		return false;

	CvSelectionGroup* const pOldGroup = getGroup();
	CvUnit* pEjectedUnit = AI_getGroup()->AI_ejectBestDefender(plot());
	if (pEjectedUnit == NULL)
	{
		FErrorMsg("AI_ejectBestDefender failed to choose a candidate for AI_guardCity.");
		pEjectedUnit = this;
		if (getGroup()->getNumUnits() > 0)
			joinGroup(NULL);
	}
	FAssert(pEjectedUnit != NULL);
	// If the unit is not suited for defense, do not use MISSIONAI_GUARD_CITY.
	MissionAITypes eMissionAI = (pEjectedUnit->noDefensiveBonus() ?
		NO_MISSIONAI : MISSIONAI_GUARD_CITY);
	if (at(*pBestGuardPlot))
	{
		pEjectedUnit->getGroup()->pushMission(MISSION_SKIP, -1, -1, NO_MOVEMENT_FLAGS,
			false, false, eMissionAI, 0);
	}
	else
	{
		FAssert(bSearch);
		FAssert(!at(*pEndTurnPlot));
		pEjectedUnit->pushGroupMoveTo(*pEndTurnPlot, eFlags, false, false,
			eMissionAI, (CvPlot*)pBestGuardPlot);
	}
	return (pEjectedUnit->getGroup() == pOldGroup || pEjectedUnit == this);
}

// Legacy TAC variant that supports defenders being cargo
bool CvUnitAI::AI_cargoGuardCityMinDefender()
{
	bool bIncludePotential = !GET_PLAYER(getOwnerINLINE()).isNative();
	if (bIncludePotential && area()->getAreaAIType(getTeam()) != AREAAI_NEUTRAL)
	{
		bIncludePotential = false;
	}
	// TAC - AI City Defense - koma13 - START
	CvCity* pPlotCity = plot()->getPlotCity();
	if (pPlotCity != NULL)
	{
		if (pPlotCity->getOwnerINLINE() == getOwnerINLINE())
		{
			if (pPlotCity->AI_numDefenders(true,bIncludePotential) <= 1)
			{
				return false;
			}
		}
	}
	// TAC - AI City Defense - koma13 - END

	CvUnit* pTransportUnit = getTransportUnit();

	bool bForceTransport = false;
	if (plot()->isCity() && (plot()->getOwnerINLINE() != getOwnerINLINE()))
	{
		bForceTransport = true;
	}

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;

	CvPlot* pBestMissionPlot = NULL;	// TAC - AI City Defense - koma13

	bool bBestIsTransport = false;

	int iLoop;
	bool bTransportPath = false;
	for (CvCity* pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop))
	{
		CvPlot* pLoopPlot = pLoopCity->plot();
		if (!atPlot(pLoopPlot) && AI_plotValid(pLoopPlot))
		{
			int iDefenders = pLoopCity->AI_numDefenders(true,bIncludePotential);

			if (iDefenders == 0)
			{
				int iIncoming = GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_GUARD_CITY, getGroup());

				if (iIncoming == 0)
				{
					bool bTransport = (pTransportUnit != NULL);
					if (!bForceTransport && canUnload() && (pLoopPlot->area() == area()))
					{
						bTransport = false;
					}
					int iPathTurns = 0;
					bool bValid;
					if (bTransport)
					{
						// TAC - AI Improved Naval AI - koma13 - START
						//bValid = pTransportUnit->generatePath(pLoopPlot, 0, bTransportPath, &iPathTurns);
						bValid = pTransportUnit->generatePath(pLoopPlot, 0, bTransportPath, &iPathTurns, false);
						// TAC - AI Improved Naval AI - koma13 - END
					}
					else
					{
						bValid = generatePath(pLoopPlot, MOVE_NO_ENEMY_TERRITORY, !bTransportPath, &iPathTurns);
						//bValid = generatePath(pLoopPlot, MOVE_THROUGH_ENEMY, !bTransportPath, &iPathTurns);
					}
					bTransportPath = bTransport;
					if (bValid)
					{
						int iValue = (pLoopCity->AI_neededDefenders() + pLoopCity->getPopulation()) * 1000;
						iValue /= 1 + iPathTurns;

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							// TAC - AI City Defense - koma13 - START
							//pBestPlot = pLoopPlot;
							pBestPlot = getPathEndTurnPlot();
							pBestMissionPlot = pLoopPlot;
							// TAC - AI City Defense - koma13 - END

							bBestIsTransport = bTransport;
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		if (bBestIsTransport)
		{
			// TAC - AI City Defense - koma13 - START
			//getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_FOUND, pBestPlot);
			getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_CITY, pBestMissionPlot);
			// TAC - AI City Defense - koma13 - END
			return true;
		}
		else
		{
			FAssert(!atPlot(pBestPlot));
			// TAC - AI City Defense - koma13 - START
			//getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_NO_ENEMY_TERRITORY, false, false, MISSIONAI_FOUND, pBestPlot);
			getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_NO_ENEMY_TERRITORY, false, false, MISSIONAI_GUARD_CITY, pBestMissionPlot);
			// TAC - AI City Defense - koma13 - END
			return true;
		}
	}
	return false;
}

bool CvUnitAI::AI_guardCityCounter(int iMaxPath)
{
	CvUnit* pTransportUnit = getTransportUnit();

	bool bForceTransport = false;
	if (plot()->isCity() && (plot()->getOwnerINLINE() != getOwnerINLINE()))
	{
		bForceTransport = true;
	}

	CvTeamAI& kTeam = GET_TEAM(getTeam());
	int iCurrentDistance = kTeam.AI_enemyUnitDistance(plot());
	if (iCurrentDistance < 2)
	{
		return false;
	}

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	bool bBestIsTransport = false;

	int iLoop;
	CvCity* pLoopCity;
	for (pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop))
	{
		CvPlot* pLoopPlot = pLoopCity->plot();
		if (!atPlot(pLoopPlot) && AI_plotValid(pLoopPlot))
		{
			int iEnemyDistance = kTeam.AI_enemyUnitDistance(pLoopPlot);
			if (iEnemyDistance != -1)
			{
				int iValue = 1000 / (1 + iEnemyDistance);

				if (iEnemyDistance < iCurrentDistance)
				{
					iValue *= 2;
				}

				int iPathTurns = 0;
				if (atPlot(pLoopPlot) || generatePath(pLoopPlot, 0, true, &iPathTurns, iMaxPath))
				if (iPathTurns < iMaxPath)
				{
					// TAC - AI Counter Units - koma13 - START
					//iValue /= 3 + iPathTurns;
					iValue = std::max(0, iValue - iPathTurns);
					// TAC - AI Counter Units - koma13 - END

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestPlot = pLoopPlot;
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		if (atPlot(pBestPlot))
		{
			return false;
		}
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE());
		return true;
	}
	return false;
}

int CvUnitAI::AI_getPlotDefendersNeeded(CvPlot* pPlot, int iExtra)
{
	int iNeeded = iExtra;

	// Super Forts begin *AI_defense*


	CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());


	if (kPlayer.AI_getPlotDanger(pPlot) > 0)
	{
		++iNeeded;
		if ((kPlayer.AI_getPlotCanalValue(pPlot) > 0)
			|| (kPlayer.AI_getPlotChokeValue(pPlot) > 0))
		{
			++iNeeded;
		}
	}

	// Super Forts end

	/* Original Code
	int iDefense = pPlot->defenseModifier(getTeam());

	iNeeded += (iDefense + 25) / 50;

	if (iNeeded == 0)
	{
		return 0;
	}

	int iNumHostiles = 0;
	int iNumPlots = 0;

	int iRange = 2;
	for (int iX = -iRange; iX <= iRange; iX++)
	{
		for (int iY = -iRange; iY <= iRange; iY++)
		{
			CvPlot* pLoopPlot = plotXY(pPlot->getX_INLINE(), pPlot->getY_INLINE(), iX, iY);
			if (pLoopPlot != NULL)
			{
				iNumHostiles += pLoopPlot->getNumVisibleEnemyDefenders(this);
				if ((pLoopPlot->getTeam() != getTeam()) || pLoopPlot->isCoastalLand())
				{
				    iNumPlots++;
                    if (isEnemy(pLoopPlot->getTeam()))
                    {
                        iNumPlots += 4;
                    }
				}
			}
		}
	}

	if ((iNumHostiles == 0) && (iNumPlots < 4))
	{
		if (iNeeded > 1)
		{
			iNeeded = 1;
		}
		else
		{
			iNeeded = 0;
		}
	} */

	return iNeeded;
}

bool CvUnitAI::AI_guardFort(bool bSearch)
{
	PROFILE_FUNC();

	if (plot()->getOwnerINLINE() == getOwnerINLINE())
	{
		ImprovementTypes eImprovement = plot()->getImprovementType();
		if (eImprovement != NO_IMPROVEMENT)
		{
			if (GC.getImprovementInfo(eImprovement).isActsAsCity())
			{
				// Super Forts begin *AI_defense* - just tweaked a number here (iExtra now 1 instead of 0)
				if (plot()->plotCount(PUF_isCityAIType, -1, -1, getOwnerINLINE()) <= AI_getPlotDefendersNeeded(plot(), 1))
				// Super Forts end
				{
					getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_BONUS, plot());
					return true;
				}
			}
		}
	}

	if (!bSearch)
	{
		return false;
	}

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestGuardPlot = NULL;

	for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);

		if (AI_plotValid(pLoopPlot) && !atPlot(pLoopPlot))
		{
			if (pLoopPlot->getOwnerINLINE() == getOwnerINLINE())
			{
				ImprovementTypes eImprovement = pLoopPlot->getImprovementType();
				if (eImprovement != NO_IMPROVEMENT)
				{
					if (GC.getImprovementInfo(eImprovement).isActsAsCity())
					{
						// Super Forts begin *AI_defense* - just tweaked a number here (iExtra now 1 instead of 0)
						int iValue = AI_getPlotDefendersNeeded(pLoopPlot, 1);
						// Super Forts end

						if (iValue > 0)
						{
							if (!(pLoopPlot->isVisibleEnemyUnit(this)))
							{
								if (GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_GUARD_BONUS, getGroup()) < iValue)
								{
									int iPathTurns;
									if (generatePath(pLoopPlot, 0, true, &iPathTurns))
									{
										iValue *= 1000;

										iValue /= (iPathTurns + 2);

										if (iValue > iBestValue)
										{
											iBestValue = iValue;
											pBestPlot = getPathEndTurnPlot();
											pBestGuardPlot = pLoopPlot;
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

	if ((pBestPlot != NULL) && (pBestGuardPlot != NULL))
	{
		if (atPlot(pBestGuardPlot))
		{
			getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_BONUS, pBestGuardPlot);
			return true;
		}
		else
		{
			FAssert(!atPlot(pBestPlot));
			getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), 0, false, false, MISSIONAI_GUARD_BONUS, pBestGuardPlot);
			return true;
		}
	}

	return false;
}

// Super Forts begin *AI_defense* - this is designed to ensure each fort has one defender (or improvements that require a fortified unit to upgrade)
bool CvUnitAI::AI_guardFortMinDefender(bool bSearch)
{
	PROFILE_FUNC();

	if (plot()->getOwnerINLINE() == getOwnerINLINE())
	{
		ImprovementTypes eImprovement = plot()->getImprovementType();
		if (eImprovement != NO_IMPROVEMENT)
		{
			if (GC.getImprovementInfo(eImprovement).isFort()) //R&R mod, vetiarvind, super forts merge - changedonly count forts and not monasteries
			{
				if (plot()->plotCount(PUF_isCityAIType, -1, -1, getOwnerINLINE()) <= 1)
				{
					getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_BONUS, plot());
					return true;
				}
			}
		}
	}

	if (!bSearch)
	{
		return false;
	}

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestGuardPlot = NULL;

	for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);
		if(pLoopPlot == NULL) continue;		//R&R mod, vetiarvind, super forts merge
		if (AI_plotValid(pLoopPlot) && !atPlot(pLoopPlot))
		{
			if (pLoopPlot->getOwnerINLINE() == getOwnerINLINE())
			{
				ImprovementTypes eImprovement = pLoopPlot->getImprovementType();
				if (eImprovement != NO_IMPROVEMENT)
				{
					if (GC.getImprovementInfo(eImprovement).isFort()) //R&R mod, vetiarvind, changed to only specify forts
					{
						if (!(pLoopPlot->isVisibleEnemyUnit(this)))
						{
							if (pLoopPlot->plotCount(PUF_isCityAIType, -1, -1, getOwnerINLINE()) == 0)
							{
								if (GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_GUARD_BONUS, getGroup()) == 0)
								{
									int iPathTurns;
									if (generatePath(pLoopPlot, 0, true, &iPathTurns))
									{
										int iValue = 1000;

										iValue /= (iPathTurns + 2);

										if (iValue > iBestValue)
										{
											iBestValue = iValue;
											pBestPlot = getPathEndTurnPlot();
											pBestGuardPlot = pLoopPlot;
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

	if ((pBestPlot != NULL) && (pBestGuardPlot != NULL))
	{
		if (atPlot(pBestGuardPlot))
		{
			getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_BONUS, pBestGuardPlot);
			return true;
		}
		else
		{
			FAssert(!atPlot(pBestPlot));
			getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), 0, false, false, MISSIONAI_GUARD_BONUS, pBestGuardPlot);
			return true;
		}
	}

	return false;
}
// Super Forts end

// Returns true if a mission was pushed...
bool CvUnitAI::AI_chokeDefend()
{
	FAssert(AI_isCityAIType());
	// XXX what about amphib invasions?

	CvCityAI const* pCity = getPlot().AI_getPlotCity();
	if (pCity != NULL && pCity->getOwner() == getOwner() &&
		pCity->AI_neededDefenders() > 1 &&
		pCity->AI_isDefended(pCity->plot()->plotCount(
			PUF_canDefendGroupHead, -1, -1, getOwner(), NO_TEAM, PUF_isNotCityAIType)))
	{
		int const iDangerThresh = 4;
		int iPlotDanger = GET_PLAYER(getOwner()).AI_getPlotDanger(plot(), 3, true);
			// advc.opt: Stop counting at thresh WTP: limit threshold not supported atm
		if (iPlotDanger <= iDangerThresh)
		{
			if (AI_anyAttack(1, 65, NO_MOVEMENT_FLAGS,
				//std::max(0, (iPlotDanger - 1))
				iPlotDanger > 1 ? 2 : 0)) // K-Mod
			{
				return true;
			}
		}
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_heal(int iDamagePercent, int iMaxPath)
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pEntityNode;
	std::vector<CvUnit*> aeDamagedUnits;
	CvSelectionGroup* pGroup;
	CvUnit* pLoopUnit;
	int iTotalDamage;
	int iTotalHitpoints;
	int iHurtUnitCount;
	bool bRetreat;

	// R&R, Robert Surcouf, Damage on Storm plots, Start
	if (plot()->getFeatureType() != NO_FEATURE)
	{
		if (GC.getFeatureInfo(plot()->getFeatureType()).getTurnDamage() != 0)
		{
			//Pass through
			//(actively seeking a safe spot may result in unit getting stuck)
			return false;
		}
	}
	// R&R, Robert Surcouf, Damage on Storm plots, End

	pGroup = getGroup();

	if (iDamagePercent == 0)
	{
		iDamagePercent = 10;
	}

	bRetreat = false;

	if (getDomainType() == DOMAIN_SEA)
	{
		// TAC - AI Improved Naval AI - koma13 - START
		if (AI_retreatFromDanger())
		{
			return true;
		}
		// TAC - AI Improved Naval AI - koma13 - END

		if (plot()->getDistanceToOcean() < 10)
		{
			if (AI_sailToEurope())
			{
				return true;
			}
		}
	}

    if (getGroup()->getNumUnits() == 1)
	{
	    if (getDamage() > 0)
        {

            if (plot()->isCity() || (healTurns(plot()) == 1))
            {
                if (!(isAlwaysHeal()))
                {
                    getGroup()->pushMission(MISSION_HEAL);
                    return true;
                }
            }
        }
        return false;
	}

	iMaxPath = std::min(iMaxPath, 2);

	pEntityNode = getGroup()->headUnitNode();

    iTotalDamage = 0;
    iTotalHitpoints = 0;
    iHurtUnitCount = 0;
	while (pEntityNode != NULL)
	{
		pLoopUnit = ::getUnit(pEntityNode->m_data);
		FAssert(pLoopUnit != NULL);
		pEntityNode = pGroup->nextUnitNode(pEntityNode);

		if (pLoopUnit == NULL)
		{
			continue;
		}

		int iDamageThreshold = (pLoopUnit->maxHitPoints() * iDamagePercent) / 100;

		if (NO_UNIT != getLeaderUnitType())
		{
			iDamageThreshold /= 2;
		}

		if (pLoopUnit->getDamage() > 0)
		{
		    iHurtUnitCount++;
		}
		iTotalDamage += pLoopUnit->getDamage();
		iTotalHitpoints += pLoopUnit->maxHitPoints();


		if (pLoopUnit->getDamage() > iDamageThreshold)
		{
			bRetreat = true;

			if (!(pLoopUnit->hasMoved()))
			{
				if (!(pLoopUnit->isAlwaysHeal()))
				{
					if (pLoopUnit->healTurns(pLoopUnit->plot()) <= iMaxPath)
					{
					    aeDamagedUnits.push_back(pLoopUnit);
					}
				}
			}
		}
	}
	if (iHurtUnitCount == 0)
	{
	    return false;
	}

	bool bPushedMission = false;
    if (plot()->isCity() && (plot()->getOwnerINLINE() == getOwnerINLINE()))
	{
		FAssertMsg(((int) aeDamagedUnits.size()) <= iHurtUnitCount, "damaged units array is larger than our hurt unit count");

		for (unsigned int iI = 0; iI < aeDamagedUnits.size(); iI++)
		{
			CvUnit* pUnitToHeal = aeDamagedUnits[iI];
			pUnitToHeal->joinGroup(NULL);
			pUnitToHeal->getGroup()->pushMission(MISSION_HEAL);

			// note, removing the head unit from a group will force the group to be completely split if non-human
			if (pUnitToHeal == this)
			{
				bPushedMission = true;
			}

			iHurtUnitCount--;
		}
	}

	if ((iHurtUnitCount * 2) > pGroup->getNumUnits())
	{
		FAssertMsg(pGroup->getNumUnits() > 0, "group now has zero units");

	    if (AI_moveIntoCity(2))
		{
			return true;
		}
		else if (healRate(plot()) > 10)
	    {
            pGroup->pushMission(MISSION_HEAL);
            return true;
	    }
	}

	return bPushedMission;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_afterAttack()
{
	if (isHuman())
	{
		return false;
	}

	if (!isMadeAttack())
	{
		return false;
	}

	if (!canFight())
	{
		return false;
	}

	if (isBlitz())
	{
		return false;
	}

	if (getDomainType() == DOMAIN_LAND)
	{
		if (AI_guardCity(false))
		{
			return true;
		}
	}

	if (AI_pillageRange(1))
	{
		return true;
	}

	if (AI_retreatToCity(false, 1))
	{
		return true;
	}

	if (AI_hide())
	{
		return true;
	}

	if (AI_pillageRange(2))
	{
		return true;
	}

	if (AI_defend())
	{
		return true;
	}

	if (AI_safety())
	{
		return true;
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_lead(std::vector<UnitAITypes>& aeUnitAITypes)
{
	PROFILE_FUNC();

	FAssertMsg(!isHuman(), "isHuman did not return false as expected");
	FAssertMsg(AI_getUnitAIType() != NO_UNITAI, "AI_getUnitAIType() is not expected to be equal with NO_UNITAI");
	FAssert(NO_PLAYER != getOwnerINLINE());

	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());

	bool bNeedLeader = false;
	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		CvTeamAI& kLoopTeam = GET_TEAM((TeamTypes)iI);
		if (isEnemy((TeamTypes)iI))
		{
			if (kLoopTeam.countNumUnitsByArea(*area()) > 0)
			{
				bNeedLeader = true;
				break;
			}
		}
	}

	CvUnit* pBestUnit = NULL;
	CvPlot* pBestPlot = NULL;

	if (bNeedLeader)
	{
		int iBestStrength = 0;
		int iLoop;
		for (CvUnit* pLoopUnit = kOwner.firstUnit(&iLoop); pLoopUnit; pLoopUnit = kOwner.nextUnit(&iLoop))
		{
			for (uint iI = 0; iI < aeUnitAITypes.size(); iI++)
			{
				if (pLoopUnit->AI_getUnitAIType() == aeUnitAITypes[iI] || NO_UNITAI == aeUnitAITypes[iI])
				{
					if (canLead(pLoopUnit->plot(), pLoopUnit->getID()))
					{
						if (AI_plotValid(pLoopUnit->plot()))
						{
							if (!(pLoopUnit->plot()->isVisibleEnemyUnit(this)))
							{
								int iPathTurns;
								// Erik: Great people should avoid dangerous paths
								if (generatePath(pLoopUnit->plot(), MOVE_NO_ENEMY_TERRITORY | MOVE_AVOID_ENEMY_WEIGHT_3, true, &iPathTurns/*, false*/))
								{
									// pick the unit with the highest current strength
									int iCombatStrength = pLoopUnit->currCombatStr(NULL, NULL);
									if (iCombatStrength > iBestStrength)
									{
										iBestStrength = iCombatStrength;
										pBestUnit = pLoopUnit;
										pBestPlot = getPathEndTurnPlot();
									}
								}
							}
						}
					}
					break;
				}
			}
		}
	}

	if (pBestPlot)
	{
		if (atPlot(pBestPlot) && pBestUnit)
		{
			getGroup()->pushMission(MISSION_LEAD, pBestUnit->getID());
			return true;
		}
		else
		{
			FAssert(!atPlot(pBestPlot));
			getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE());
			return true;
		}
	}

	return false;
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_bravePatrol()
{
	PROFILE_FUNC();
	FAssert(canMove());

	CvPlot* pAdjacentPlot;
	CvPlot* pBestPlot;
	int iValue;
	int iBestValue;
	int iI;

	iBestValue = 0;
	pBestPlot = NULL;

	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());

	int iOldDistance = kOwner.AI_cityDistance(plot());

	for (iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (AI_plotValid(pAdjacentPlot))
			{
				if (!(pAdjacentPlot->isVisibleEnemyUnit(this)))
				{
					iValue = 0;

					if (pAdjacentPlot->isRevealedGoody(getTeam()))
					{
						iValue -= 10000;
					}

					if (getHomeCity() != NULL)
					{
						int iDistance = kOwner.AI_cityDistance(plot());

						if (iDistance > iOldDistance)
						{
							iValue += 2500;
						}
						else if (iDistance == iOldDistance)
						{
							iValue += 1000;
						}
					}

					if (iValue > 0)
					{
						iValue = (1 + GC.getGameINLINE().getSorenRandNum(iValue, "AI Patrol"));

						if (iValue > iBestValue)
						{
							if (generatePath(pAdjacentPlot, 0, true))
							{
								iBestValue = iValue;
								pBestPlot = getPathEndTurnPlot();
								FAssert(!atPlot(pBestPlot));
							}
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		FAssert(!atPlot(pBestPlot));
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE());
		return true;
	}

	return false;
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_patrol()
{
	PROFILE_FUNC();

	CvPlot* pAdjacentPlot;
	CvPlot* pBestPlot;
	int iValue;
	int iBestValue;
	int iI;

	iBestValue = 0;
	pBestPlot = NULL;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (AI_plotValid(pAdjacentPlot))
			{
				if (!(pAdjacentPlot->isVisibleEnemyUnit(this)))
				{
					if (generatePath(pAdjacentPlot, 0, true))
					{
						iValue = (1 + GC.getGameINLINE().getSorenRandNum(10000, "AI Patrol"));

							if (pAdjacentPlot->isRevealedGoody(getTeam()))
							{
								iValue += 100000;
							}

							if (pAdjacentPlot->getOwnerINLINE() == getOwnerINLINE())
							{
								iValue += 10000;
							}

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							pBestPlot = getPathEndTurnPlot();
							FAssert(!atPlot(pBestPlot));
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		FAssert(!atPlot(pBestPlot));
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE());
		return true;
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_defend()
{
	PROFILE_FUNC();

	if (AI_defendPlot(plot()))
	{
		getGroup()->pushMission(MISSION_SKIP);
		return true;
	}

	CvPlot* pBestPlot = NULL;
	int iBestValue = 0;
	//for (SquareIter it(*this, AI_searchRange(1), false); it.hasNext(); ++it)
	FOR_EACH_NON_CENTER_PLOT_IN_RANGE_OF(this, AI_searchRange(1),
	{
		CvPlot& p = *pLoopPlot;
		if (!AI_plotValid(&p) || !AI_defendPlot(&p) || p.isVisibleEnemyUnit(this))
			continue;
		int iPathTurns;
		if (!generatePath(&p, NO_MOVEMENT_FLAGS, true, &iPathTurns, 1))
			continue;
		/*if (iPathTurns != 1)
			continue;*/ // advc: redundant
		int iValue = 1 + SyncRandNum(10000);
		if (iValue > iBestValue)
		{
			iBestValue = iValue;
			pBestPlot = &p;
		}
	}) // FOR_EACH

	if (pBestPlot == NULL)
		return false;

	// BETTER_BTS_AI_MOD, Unit AI, 12/06/08, jdog5000: START
	if (!pBestPlot->isCity() && getGroup()->getNumUnits() > 1)
	{	//getGroup()->AI_makeForceSeparate();
		joinGroup(NULL); // K-Mod. (AI_makeForceSeparate is a complete waste of time here.)
	} // BETTER_BTS_AI_MOD: END

	pushGroupMoveTo(*pBestPlot, NO_MOVEMENT_FLAGS, false, false, MISSIONAI_DEFEND);
	return true;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_safety()
{
	PROFILE_FUNC();

	CvPlot* pBestPlot = 0;
	int iBestValue = 0;
	bool bIgnoreDanger = false;
	// <advc.306>
	bool const bBarbarianSeaUnit = (isBarbarian() && getDomainType() == DOMAIN_SEA);
	int const iSearchTurns = (bBarbarianSeaUnit ? 3 : 1); // </advc.306>
	int const iSearchRange = AI_searchRange(iSearchTurns);
	bool const bEnemyTerritory = isEnemy(getPlot());
	//for (iPass = 0; iPass < 2; iPass++)
	do // K-Mod. What's the point of the first pass if it is just ignored? (see break condition at the end)
	{
		FOR_EACH_PLOT_IN_RANGE_OF(this, iSearchRange,
		{
			CvPlot& p = *pLoopPlot;
			if (!AI_plotValid(&p) || p.isVisibleEnemyUnit(this))
				continue;
			int iPathTurns;
			if (!generatePath(&p,
				bIgnoreDanger ? MOVE_IGNORE_DANGER : NO_MOVEMENT_FLAGS,
				true, &iPathTurns, iSearchTurns))
			{
				continue;
			}
			/*  <advc.306> Consider only unobserved plots (and cities) safe for
				Barbarian sea units. This should help them receive spawned cargo. */
			if (bBarbarianSeaUnit && p.isWater() && p.isVisibleToCivTeam())
				continue; // </advc.306>
			int iCount = 0;
			FOR_EACH_UNIT_ON(pLoopUnit, &p)
			{
				if (pLoopUnit->getOwner() != getOwner() || !pLoopUnit->canDefend())
					continue;
				CvUnit const* pHeadUnit = pLoopUnit->getGroup()->getHeadUnit();
				FAssert(pHeadUnit != NULL);
				FAssert(getGroup()->getHeadUnit() == this);
				if (pHeadUnit != this)
				{
					if (pHeadUnit->isWaiting() || !pHeadUnit->canMove())
					{
						FAssert(pLoopUnit != this);
						FAssert(pHeadUnit != getGroup()->getHeadUnit());
						iCount++;
					}
				}
			}
			int iValue = iCount * 100;
			//iValue += p.defenseModifier(getTeam(), false);
			iValue += AI_plotDefense(&p); // advc.012
			// K-Mod
			if (bEnemyTerritory ? !isEnemy(p) : (p.getTeam() == getTeam()))
				iValue += 30;
			if (p.isValidRoute(this, /* advc.001i: */ false))
				iValue += 25; // K-Mod end
			if (at(p))
				iValue += 50;
			else iValue += SyncRandNum(50);
			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestPlot = &p;
			}
		}) // FOR_EACH
		// K-Mod
		if (pBestPlot == NULL)
		{
			if (bIgnoreDanger)
				break; // no suitable plot, even when ignoring danger
			else bIgnoreDanger = true; // try harder next time
		} // K-Mod end
	} while (pBestPlot == NULL);

	if (pBestPlot != NULL)
	{
		if (at(*pBestPlot))
		{
			getGroup()->pushMission(MISSION_SKIP);
			return true;
		}
		else
		{
			pushGroupMoveTo(*pBestPlot,
				bIgnoreDanger ? MOVE_IGNORE_DANGER : NO_MOVEMENT_FLAGS);
			return true;
		}
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_hide()
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvUnit* pHeadUnit;
	CvPlot* pLoopPlot;
	CvPlot* pBestPlot;
	bool bValid;
	int iSearchRange;
	int iPathTurns;
	int iValue;
	int iBestValue;
	int iCount;
	int iDX, iDY;
	int iI;

	if (getInvisibleType() == NO_INVISIBLE)
	{
		return false;
	}

	iSearchRange = AI_searchRange(1);

	iBestValue = 0;
	pBestPlot = NULL;

	for (iDX = -(iSearchRange); iDX <= iSearchRange; iDX++)
	{
		for (iDY = -(iSearchRange); iDY <= iSearchRange; iDY++)
		{
			pLoopPlot	= plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

			if (pLoopPlot != NULL)
			{
				if (AI_plotValid(pLoopPlot))
				{
					bValid = true;

					for (iI = 0; iI < MAX_TEAMS; iI++)
					{
						if (GET_TEAM((TeamTypes)iI).isAlive())
						{
							if (pLoopPlot->isInvisibleVisible(((TeamTypes)iI), getInvisibleType()))
							{
								bValid = false;
								break;
							}
						}
					}

					if (bValid)
					{
						if (!(pLoopPlot->isVisibleEnemyUnit(this)))
						{
							if (generatePath(pLoopPlot, 0, true, &iPathTurns))
							{
								if (iPathTurns <= 1)
								{
									iCount = 1;

									pUnitNode = pLoopPlot->headUnitNode();

									while (pUnitNode != NULL)
									{
										pLoopUnit = pLoopPlot->getUnitNodeLoop(pUnitNode);

										if (pLoopUnit != NULL && pLoopUnit->getOwnerINLINE() == getOwnerINLINE())
										{
											if (pLoopUnit->canDefend())
											{
												pHeadUnit = pLoopUnit->getGroup()->getHeadUnit();
												FAssert(pHeadUnit != NULL);
												FAssert(getGroup()->getHeadUnit() == this);

												if (pHeadUnit != this)
												{
													if (pHeadUnit->isWaiting() || !(pHeadUnit->canMove()))
													{
														FAssert(pLoopUnit != this);
														FAssert(pHeadUnit != getGroup()->getHeadUnit());
														iCount++;
													}
												}
											}
										}
									}

									iValue = (iCount * 100);

									iValue += pLoopPlot->defenseModifier(getTeam());

									if (atPlot(pLoopPlot))
									{
										iValue += 50;
									}
									else
									{
										iValue += GC.getGameINLINE().getSorenRandNum(50, "AI Hide");
									}

									if (iValue > iBestValue)
									{
										iBestValue = iValue;
										pBestPlot = pLoopPlot;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		if (atPlot(pBestPlot))
		{
			getGroup()->pushMission(MISSION_SKIP);
			return true;
		}
		else
		{
			getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE());
			return true;
		}
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_goody(bool bIgnoreCity)
{
	PROFILE_FUNC();

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestExplorePlot = NULL;

	for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		PROFILE("AI_explore 1");

		CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);

		if (AI_plotValid(pLoopPlot))
		{
			int iValue = 0;

			if (pLoopPlot->isRevealedGoody(getTeam()))
			{
				iValue += 10000;
			}

			if (!bIgnoreCity && pLoopPlot->isCity())
			{
				if (!pLoopPlot->getPlotCity()->isScoutVisited(getTeam()))
				{
					if (canSpeakWithChief(pLoopPlot))
					{
						iValue += 10000;
					}
				}
			}

			if (iValue > 0)
			{
				if (!(pLoopPlot->isVisibleEnemyUnit(this)))
				{
					if (GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_EXPLORE, getGroup(), 3) == 0)
					{
						int iPathTurns = 0;
						if (!atPlot(pLoopPlot) && generatePath(pLoopPlot, MOVE_NO_ENEMY_TERRITORY, true, &iPathTurns))
						{
							iValue += GC.getGameINLINE().getSorenRandNum(250 * abs(xDistance(getX_INLINE(), pLoopPlot->getX_INLINE())) + abs(yDistance(getY_INLINE(), pLoopPlot->getY_INLINE())), "AI explore");

							iValue /= 3 + std::max(1, iPathTurns);

							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestPlot = getPathEndTurnPlot();
								pBestExplorePlot = pLoopPlot;
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestExplorePlot != NULL))
	{
		FAssert(!atPlot(pBestPlot));
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_NO_ENEMY_TERRITORY, false, false, MISSIONAI_EXPLORE, pBestExplorePlot);
		return true;
	}

	return false;

}

bool CvUnitAI::AI_goodyRange(int iRange, bool bIgnoreCity)
{
	PROFILE_FUNC();

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestExplorePlot = NULL;

	for (int iX = -iRange; iX <= iRange; iX++)
	{
		for (int iY = -iRange; iY <= iRange; iY++)
		{
			CvPlot* pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iX, iY);

			if (pLoopPlot != NULL && AI_plotValid(pLoopPlot))
			{
				int iValue = 0;

				if (pLoopPlot->isRevealedGoody(getTeam()))
				{
					iValue += 10000;
				}

				if (!bIgnoreCity && pLoopPlot->isCity())
				{
					if (!pLoopPlot->getPlotCity()->isScoutVisited(getTeam()))
					{
						if (canSpeakWithChief(pLoopPlot))
						{
							iValue += 10000;
						}
					}
				}

				if (iValue > 0)
				{
					if (!(pLoopPlot->isVisibleEnemyUnit(this)))
					{
						if (GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_EXPLORE, getGroup(), 1) == 0)
						{
							int iPathTurns = 0;
							if (!atPlot(pLoopPlot) && generatePath(pLoopPlot, MOVE_NO_ENEMY_TERRITORY, true, &iPathTurns))
							{
								iValue += GC.getGameINLINE().getSorenRandNum(250 * abs(xDistance(getX_INLINE(), pLoopPlot->getX_INLINE())) + abs(yDistance(getY_INLINE(), pLoopPlot->getY_INLINE())), "AI explore");

								iValue /= 3 + std::max(1, iPathTurns);

								// Only consider goodies that we can reach this turn!
								if (iValue > iBestValue && getPathFinder().GetPathTurns() == 1)
								{
									iBestValue = iValue;
									pBestPlot = getPathEndTurnPlot();
									pBestExplorePlot = pLoopPlot;
								}
							}
						}
					}
				}
			}
		}
	}


	if ((pBestPlot != NULL) && (pBestExplorePlot != NULL))
	{
		FAssert(!atPlot(pBestPlot));
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_NO_ENEMY_TERRITORY, false, false, MISSIONAI_EXPLORE, pBestExplorePlot);
		return true;
	}

	return false;

}

bool CvUnitAI::AI_isValidExplore(CvPlot* pPlot) const
{
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	if (kOwner.getNumCities() == 0 || kOwner.AI_getNumAIUnits(AI_getUnitAIType()) > 1)
	{
		return true;
	}
	int iMaxDistance = MAX_INT;
	int iGameTurn = GC.getGameINLINE().getGameTurn();
	if (iGameTurn < 30)
	{
		iMaxDistance = 10 + iGameTurn / 5;

		if (getDomainType() == DOMAIN_SEA)
		{
			iMaxDistance += iGameTurn / 2;
		}
	}

	if (getDomainType() == DOMAIN_SEA || pPlot->area()->getCitiesPerPlayer(getOwnerINLINE()) > 0)
	{
		if (kOwner.AI_cityDistance(pPlot) > iMaxDistance)
		{
			return false;
		}
	}

	return true;
}

int CvUnitAI::AI_explorePlotValue(CvPlot* pPlot, bool bImportantOnly) const
{
	int iValue = 0;
	if (pPlot->isRevealedGoody(getTeam()))
	{
		iValue += 50000;
	}

	if (pPlot->isCity())
	{
		if (!pPlot->getPlotCity()->isScoutVisited(getTeam()))
		{
			if (canSpeakWithChief(pPlot))
			{
				iValue += 50000;
			}
		}
	}

	if (iValue == 0)
	{
		if (!AI_isValidExplore(pPlot))
		{
			return 0;
		}
	}

	const CvTeam& kTeam = GET_TEAM(getTeam()); // K-Mod

	if (!bImportantOnly || iValue > 0)
	{
		for (int i = 0; i < NUM_DIRECTION_TYPES; i++)
		{
			CvPlot* pAdjacentPlot = plotDirection(pPlot->getX_INLINE(), pPlot->getY_INLINE(), ((DirectionTypes)i));

			if (pAdjacentPlot != NULL)
			{
				if (!(pAdjacentPlot->isRevealed(getTeam(), false)))
				{
					iValue += 1000;
					if (pPlot->isWater() != pAdjacentPlot->isWater())
					{
						iValue += 1000;
					}

					if (pAdjacentPlot->isRiver())
					{
						iValue += 500;
					}
				}

				// K-Mod. Try to meet teams that we have seen through map trading
				if (pAdjacentPlot->getRevealedOwner(kTeam.getID(), false) != NO_PLAYER) // note: revealed team can be set before the plot is actually revealed.
				{
					if (!kTeam.isHasMet(pAdjacentPlot->getRevealedTeam(kTeam.getID(), false)))
						iValue += 1000;
				}
				// K-Mod end
			}
		}
	}

	return iValue;
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_explore(bool bFavorOpenBorders)
{
	PROFILE_FUNC();

	CvPlot* pBestPlot;
	CvPlot* pBestExplorePlot;
	int iPathTurns;
	int iBestValue;
	int iI;

	iBestValue = 0;
	pBestPlot = NULL;
	pBestExplorePlot = NULL;

	bool bNoContact = (GC.getGameINLINE().countCivTeamsAlive() > GET_TEAM(getTeam()).getHasMetCivCount());

	for (iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		PROFILE("AI_explore 1");

		CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);

		if (AI_plotValid(pLoopPlot))
		{
			int iValue = AI_explorePlotValue(pLoopPlot, false);

			if (iValue > 0)
			{
				if (!(pLoopPlot->isVisibleEnemyUnit(this)))
				{
					if (GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_EXPLORE, getGroup(), 3) == 0)
					{
						if (!atPlot(pLoopPlot) && generatePath(pLoopPlot, MOVE_NO_ENEMY_TERRITORY, true, &iPathTurns))
						{
							iValue += GC.getGameINLINE().getSorenRandNum(250 * abs(xDistance(getX_INLINE(), pLoopPlot->getX_INLINE())) + abs(yDistance(getY_INLINE(), pLoopPlot->getY_INLINE())), "AI explore");

							if (pLoopPlot->isAdjacentToLand())
							{
								iValue += 10000;
							}

							if (bFavorOpenBorders && pLoopPlot->isOwned())
							{
								iValue *= 2;
							}

							iValue /= 3 + std::max(1, iPathTurns);

							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestPlot = pLoopPlot->isRevealedGoody(getTeam()) ? getPathEndTurnPlot() : pLoopPlot;
								pBestExplorePlot = pLoopPlot;
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestExplorePlot != NULL))
	{
		FAssert(!atPlot(pBestPlot));
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_NO_ENEMY_TERRITORY, false, false, MISSIONAI_EXPLORE, pBestExplorePlot);
		return true;
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_exploreRange(int iRange)
{
	OOS_LOG("CvUnitAI::AI_exploreRange start", getID());

	PROFILE_FUNC();

	if (canSpeakWithChief(plot()))
	{
		if (!plot()->getPlotCity()->isScoutVisited(getTeam()))
		{
			speakWithChief();
			return true;
		}
	}

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestExplorePlot = NULL;


	for (int iX = -iRange; iX <= iRange; iX++)
	{
		for (int iY = -iRange; iY <= iRange; iY++)
		{
			CvPlot* pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iX, iY);
			if ((pLoopPlot != NULL) && !atPlot(pLoopPlot))
			{
				if (pLoopPlot->isRevealed(getTeam(), false))
				{
					int iValue = AI_explorePlotValue(pLoopPlot, false);

					if (iValue > 0)
					{
						if ((pLoopPlot->isVisibleEnemyUnit(this)))
							continue;

						if (GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_EXPLORE, getGroup(), 3) == 0)
						{
							if (generatePath(pLoopPlot, MOVE_BUST_FOG, true))
							{
								if (getPathFinder().GetFinalMoves() == 0)
								{
									iValue += pLoopPlot->seeFromLevel() * 500;
								}
								iValue *= 100;
								iValue += GC.getGame().getSorenRandNum(100, "AI Explore Range");
								iValue /= 100 + getPathCost();

								if (iValue > iBestValue)
								{
									iBestValue = iValue;
									pBestExplorePlot = pLoopPlot;
									pBestPlot = getPathEndTurnPlot();
								}
							}
						}
					}
				}
			}
		}
	}

	OOS_LOG("CvUnitAI::AI_exploreRange end", getID());

	if (pBestPlot != NULL)
	{
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_BUST_FOG, false, false, MISSIONAI_EXPLORE, pBestExplorePlot);
		return true;
	}
	return false;
}

bool CvUnitAI::AI_exploreFromShip(int iMaxPath)
{
	PROFILE_FUNC();

	FAssert(isCargo());
	FAssert(iMaxPath > 0);
	int iBestValue = 0;
	CvPlot* pEndTurnPlot = NULL;
	CvPlot* pDestinationPlot = NULL;
	
	CvUnit* const pTransportUnit = getTransportUnit();
	const int iRange = iMaxPath * pTransportUnit->baseMoves();
	bool bTransportPath = false;
	for (int iX = -iRange; iX <= iRange; iX++)
	{
		for (int iY = -iRange; iY <= iRange; iY++)
		{
			CvPlot* pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iX, iY);
			if ((pLoopPlot != NULL) && !atPlot(pLoopPlot) && !pLoopPlot->isWater())
			{
				if (!pLoopPlot->isRevealed(getTeam(), false))
					continue;

				if (!pLoopPlot->isCoastalLand())
					continue;
	
				int iValue = AI_explorePlotValue(pLoopPlot, false);

				CvArea* const pArea = pLoopPlot->area();
				if (pArea->isEuropePlayer())
					iValue /= 2;

				if (pArea->isIsland())
					continue;

				//iValue += std::max(40, pArea->getNumUnrevealedTiles(getTeam())) * 50;
				// TAC - AI Explore from Ship endless loop fix - koma13 - END

				if (pArea->getNumAIUnits(getOwnerINLINE(), UNITAI_SCOUT) == 0)
					iValue *= 5;

				if (iValue > 0)
				{
					if (GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_EXPLORE, getGroup(), 3) == 0)
					{
						int iPathTurns = 0;
						if (pTransportUnit->generatePath(pLoopPlot, 0, bTransportPath, &iPathTurns, iMaxPath))
						{
							iValue *= 100;
							iValue /= 100 + getPathCost();

							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pDestinationPlot = pLoopPlot;
								pEndTurnPlot = pTransportUnit->getPathEndTurnPlot();
							}
						}
					}
				}
			}
		}
	}

	if (pEndTurnPlot != NULL && pDestinationPlot != NULL)
	{
		FAssert(!pEndTurnPlot->isImpassable() || canMoveImpassable());

		// Check if we can reach the destination plot on this turn
		if (pEndTurnPlot == pDestinationPlot || ::stepDistance(pEndTurnPlot, pDestinationPlot) == 1)
		{
			if (at(*pDestinationPlot))
			{
				FAssert(pEndTurnPlot->isCity());
				// We're in a city, unload so the cargo can take its turn (unload will set the transported units priority above 
				// the transport so they can move this turn)
				pTransportUnit->getGroup()->unloadAll();
				return true;
			}
			else if (at(*pEndTurnPlot) && !pTransportUnit->canMove() && canMove())
			{
				// We're adjacent to the destination plot so let's disembark if we can move. Note: MOVE_IGNORE_DANGER will ensure that the move does not get cancelled
				// once the automission starts
				getGroup()->pushMission(MISSION_MOVE_TO, pDestinationPlot->getX_INLINE(), pDestinationPlot->getY_INLINE(),
					MOVE_IGNORE_DANGER, false, false, MISSIONAI_EXPLORE, pDestinationPlot);
			}
			else
			{
				// We can reach the destination with an amfib move this turn
				pTransportUnit->getGroup()->pushMission(MISSION_MOVE_TO, pDestinationPlot->getX_INLINE(), pDestinationPlot->getY_INLINE(),
					NO_MOVEMENT_FLAGS, false, false, MISSIONAI_EXPLORE, pDestinationPlot);
				getGroup()->pushMission(MISSION_SKIP);
				return true;
			}
		}
		else
		{
			pTransportUnit->getGroup()->pushMission(MISSION_MOVE_TO, pEndTurnPlot->getX_INLINE(), pEndTurnPlot->getY_INLINE(),
				NO_MOVEMENT_FLAGS, false, false, MISSIONAI_EXPLORE, pDestinationPlot);
			getGroup()->pushMission(MISSION_SKIP);
			return true;
		}
	}
	return false;
}

bool CvUnitAI::AI_exploreCoast(int iRange)
{
	PROFILE_FUNC();

	CvPlot* pBestPlot = NULL;
	CvPlot* pBestExplorePlot = NULL;
	int iBestValue = 0;

	for (int iDX = -iRange; iDX <= iRange; iDX++)
	{
		for (int iDY = -iRange; iDY <= iRange; iDY++)
		{
			CvPlot* pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

			if (pLoopPlot != NULL)
			{
				if (AI_plotValid(pLoopPlot) && !atPlot(pLoopPlot) && AI_isValidExplore(pLoopPlot))
				{
					int iValue = 0;
					int iLandCount = 0;
					int iUnrevealedLandCount = 0;
					int iUnrevealedCount = 0;
					int iImpassableCount = 0;

					if (!pLoopPlot->isVisibleEnemyDefender(this))
					{
						for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
						{
							CvPlot* pAdjacentPlot = plotDirection(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), ((DirectionTypes)iI));

							if (pAdjacentPlot != NULL)
							{
								if (!pAdjacentPlot->isRevealed(getTeam(), false))
								{
									iUnrevealedCount++;
									if (pLoopPlot->isRevealed(getTeam(), false))
									{
										if (!pLoopPlot->isWater())
										{
											iUnrevealedLandCount++;
										}
									}
								}
								if (!pAdjacentPlot->isWater())
								{
									iLandCount++;
								}
							}
						}
					}

					if ((iUnrevealedCount > 2) || (iUnrevealedLandCount > 0))
					{
						iValue += iLandCount * ((iUnrevealedCount == 1) ? 50 : 100);
					}

					if (iValue > 0)
					{
						iValue *= 1000;
						iValue += GC.getGame().getSorenRandNum(100, "AI explore");

						iValue *= 4;
						iValue /= 4 + pLoopPlot->getDistanceToOcean();
					}

					if (iValue > 0)
					{
						int iPathTurns;
						if (generatePath(pLoopPlot, MOVE_BUST_FOG, true, &iPathTurns))
						{
							if (iPathTurns <= 2)
							{
								//iValue /= 1 + stepDistance(iDX, iDY, 0, 0);
								iValue *= 10000;
								iValue /= 100 + getPathCost();
								if (iValue > iBestValue)
								{
									iBestValue = iValue;
									pBestExplorePlot = pLoopPlot;
									pBestPlot = getGroup()->getPathFirstPlot();
								}
							}
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_BUST_FOG, false, false, MISSIONAI_EXPLORE, pBestExplorePlot);
		return true;
	}

	return false;
}

bool CvUnitAI::AI_exploreOcean(int iRange)
{
	PROFILE_FUNC();

	CvPlot* pBestPlot = NULL;
	CvPlot* pBestExplorePlot = NULL;
	int iBestValue = 0;
	DirectionTypes preferredDirection = GET_PLAYER(getOwnerINLINE()).getPreferredStartingDirection();

	for (int iDX = -iRange; iDX <= iRange; iDX++)
	{
		for (int iDY = -iRange; iDY <= iRange; iDY++)
		{
			CvPlot* pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

			if (pLoopPlot != NULL)
			{
				if (AI_plotValid(pLoopPlot) && !atPlot(pLoopPlot) && AI_isValidExplore(pLoopPlot))
				{
					int iValue = 0;
					int iUnrevealedCount = 0;
					int iImpassableCount = 0;
					int iEuropeCount = 0;

					if (!pLoopPlot->isVisibleEnemyDefender(this))
					{
						if (pLoopPlot->isRevealed(getTeam(), true))
						{
							for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
							{
								CvPlot* pAdjacentPlot = plotDirection(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), ((DirectionTypes)iI));

								if (pAdjacentPlot != NULL)
								{
									if (!pAdjacentPlot->isRevealed(getTeam(), false))
									{
										iUnrevealedCount++;
									}
									if (pLoopPlot->isEurope() && pAdjacentPlot->isEurope())
									{
										iEuropeCount++;
									}

									if (pAdjacentPlot->isImpassable())
									{
										iImpassableCount++;
									}
								}
							}
						}
					}

					if (iUnrevealedCount > 2)
					{
						iValue += iUnrevealedCount * 100;
						iValue /= 1 + iImpassableCount;
						iValue /= 1 + iEuropeCount;
					}

					if (GC.getGameINLINE().getGameTurn() < 10)
					{
						DirectionTypes currentDirection = getDirectionFrom_dX_dY(iDX, iDY);
						iValue += ((NUM_DIRECTION_TYPES / 2) - getDirectionDiff(currentDirection, preferredDirection)) * 100;
					}

					if (iValue > 0)
					{
						iValue *= 1000;
						iValue += GC.getGame().getSorenRandNum(100, "AI explore");
					}


					if (iValue > iBestValue)
					{
						if (generatePath(pLoopPlot, MOVE_BUST_FOG, true))
						{
							iBestValue = iValue;
							pBestExplorePlot = pLoopPlot;
							pBestPlot = getPathEndTurnPlot();
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_BUST_FOG, false, false, MISSIONAI_EXPLORE, pBestExplorePlot);
		return true;
	}

	return false;
}

bool CvUnitAI::AI_exploreDeep()
{
	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestExplorePlot = NULL;

	for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);

		if (AI_plotValid(pLoopPlot) && !atPlot(pLoopPlot) && AI_isValidExplore(pLoopPlot))
		{
			int iValue = 0;
			int iUnrevealedCount = 0;
			int iLandCount = 0;
			int iWaterCount = 0;
			int iImpassableCount = 0;
			int iEuropeCount = 0;
			if (pLoopPlot->isRevealed(getTeam(), false) && !pLoopPlot->isVisibleEnemyDefender(this))
			{
				for (int iJ = 0; iJ < NUM_DIRECTION_TYPES; iJ++)
				{
					PROFILE("AI_explore 2");

					CvPlot* pAdjacentPlot = plotDirection(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), ((DirectionTypes)iJ));

					if (pAdjacentPlot != NULL)
					{
						if (!(pAdjacentPlot->isRevealed(getTeam(), false)))
						{
							iUnrevealedCount++;
						}
						if (pAdjacentPlot->isWater())
						{
							iWaterCount++;
							if (pAdjacentPlot->isImpassable())
							{
								iImpassableCount++;
							}
							if (pAdjacentPlot->isEurope() && pLoopPlot->isEurope())
							{
								iEuropeCount++;
							}
						}
						else
						{
							iLandCount++;
						}
					}
				}

				if ((iUnrevealedCount > 2) || ((iUnrevealedCount > 0) && (iLandCount > 0)))
				{
					iValue += iLandCount * 100;
					iValue += iWaterCount * 30;

					if (iLandCount == 0)
					{
						iValue /= 1 + 2 * iImpassableCount;
					}
					iValue /= 1 + iEuropeCount;
				}

				if (iValue > 0)
				{
					if (!(pLoopPlot->isVisibleEnemyUnit(this)))
					{
						if (GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_EXPLORE, getGroup(), 3) == 0)
						{
							int iPathTurns;
							if (generatePath(pLoopPlot, MOVE_BUST_FOG, true, &iPathTurns))
							{
								iValue += GC.getGameINLINE().getSorenRandNum(10, "AI find land deep search");

								iValue *= 5;
								iValue /= 5 + pLoopPlot->getDistanceToOcean();

								iValue *= 10000;

								iValue /= 100 + getPathCost();

								if (iValue > iBestValue)
								{
									iBestValue = iValue;
									pBestPlot = getPathEndTurnPlot();
									pBestExplorePlot = pLoopPlot;
								}
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestExplorePlot != NULL))
	{
		FAssert(!atPlot(pBestPlot));
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_BUST_FOG, false, false, MISSIONAI_EXPLORE, pBestExplorePlot);
		return true;
	}

	return false;
}

bool CvUnitAI::AI_exploreOpenBorders(int iRange)
{
	PROFILE_FUNC();

	CvPlot* pBestPlot = NULL;
	CvPlot* pBestExplorePlot = NULL;
	int iBestValue = 0;
	CvTeamAI& kTeam = GET_TEAM(getTeam());
	for (int iDX = -iRange; iDX <= iRange; iDX++)
	{
		for (int iDY = -iRange; iDY <= iRange; iDY++)
		{
			CvPlot* pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

			if (pLoopPlot != NULL)
			{
				if (AI_plotValid(pLoopPlot) && !atPlot(pLoopPlot) && AI_isValidExplore(pLoopPlot))
				{
					if (!pLoopPlot->isVisibleEnemyDefender(this))
					{
						int iValue = 0;
						for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
						{
							CvPlot* pAdjacentPlot = plotDirection(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), ((DirectionTypes)iI));

							if (pAdjacentPlot != NULL)
							{
								if (!pAdjacentPlot->isRevealed(getTeam(), false))
								{
									iValue += 100;
									if (pAdjacentPlot->isWater())
									{
										iValue += 50;
									}
									if (pAdjacentPlot->isRiver())
									{
										iValue += 50;
									}
									if (pAdjacentPlot->isOwned() && !isEnemy(pAdjacentPlot->getTeam()))
									{
										iValue += 200;
										if (kTeam.isOpenBorders(pAdjacentPlot->getTeam()))
										{
											iValue += 300;
										}
									}
								}
							}
						}

						if (iValue > 0)
						{
							iValue *= 100;
							iValue += GC.getGame().getSorenRandNum(100, "AI explore");
						}

						if (iValue > 0)
						{
							int iPathTurns;
							if (generatePath(pLoopPlot, MOVE_BUST_FOG | MOVE_NO_ENEMY_TERRITORY, true, &iPathTurns))
							{
								iValue *= 10000;
								iValue /= 100 + getPathCost();
								if (iValue > iBestValue)
								{
									iBestValue = iValue;
									pBestExplorePlot = pLoopPlot;
									pBestPlot = getGroup()->getPathFirstPlot();
								}
							}
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_BUST_FOG | MOVE_NO_ENEMY_TERRITORY, false, false, MISSIONAI_EXPLORE, pBestExplorePlot);
		return true;
	}

	return false;
}

//TAC Whaling, ray
// Returns true if a mission was pushed...
bool CvUnitAI::AI_exploreHighSeas(int iRange)
{
	PROFILE_FUNC();

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestExplorePlot = NULL;

	for (int iDX = -iRange; iDX <= iRange; iDX++)
	{
		for (int iDY = -iRange; iDY <= iRange; iDY++)
		{
			CvPlot* pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);
			if (pLoopPlot != NULL)
			{
				if (AI_plotValid(pLoopPlot) && !atPlot(pLoopPlot) && AI_isValidExplore(pLoopPlot))
				{
					int iValue = 0;
					int iUnrevealedCount = 0;
					int iImpassableCount = 0;
					int iOceanBonusCount = 0;
					int iCoastalCount = 0;
					int iLandCount = 0;

					if (!pLoopPlot->isVisibleEnemyDefender(this))
					{
						if (pLoopPlot->isRevealed(getTeam(), true))
						{
							for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
							{
								CvPlot* pAdjacentPlot = plotDirection(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), ((DirectionTypes)iI));

								if (pAdjacentPlot != NULL)
								{
									if (pAdjacentPlot->isWater() && (!pAdjacentPlot->isHills() || !pAdjacentPlot->isPeak()))
									{
										if (!pAdjacentPlot->isRevealed(getTeam(), false))
										{
											iUnrevealedCount++;
										}

										if (pAdjacentPlot->isImpassable())
										{
											iImpassableCount++;
										}

										if (pAdjacentPlot->getTerrainType() == (TerrainTypes)GC.getDefineINT("DEEP_WATER_TERRAIN"))
										{
											if (pAdjacentPlot->getBonusType() != NO_BONUS)
											{
												iOceanBonusCount++;
											}
										}
										else
										{
											iCoastalCount++;
										}
									}
									else
									{
										iLandCount++;
									}
								}
							}
						}
					}

					if (iUnrevealedCount > 2)
					{
						iValue += (iUnrevealedCount + iOceanBonusCount) * 100;
						iValue /= 1 + iImpassableCount;
						iValue /= 1 + iLandCount;
						iValue /= 1 + iCoastalCount;
					}

					if (iValue > 0)
					{
						iValue *= 1000;
						iValue += GC.getGame().getSorenRandNum(100, "AI explore");
					}

					if (iValue > iBestValue)
					{
						if (generatePath(pLoopPlot, MOVE_BUST_FOG, true))
						{
							iBestValue = iValue;
							pBestPlot = getPathEndTurnPlot();
							pBestExplorePlot = pLoopPlot;
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_BUST_FOG, false, false, MISSIONAI_WORKER_SEA, pBestExplorePlot);
		return true;
	}

	return false;
}

//koma13
bool CvUnitAI::AI_moveToWhale()
{
	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestExplorePlot = NULL;

	for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);
		if (AI_plotValid(pLoopPlot))
		{
			if (pLoopPlot->isRevealed(getTeam(), false) && !pLoopPlot->isVisibleEnemyDefender(this))
			{
				int iPlotValue = 0;
				if (canGatherResource(pLoopPlot))
				{
					if (GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_WORKER_SEA, getGroup(), 3) == 0)
					{
						int iPathTurns;
						if (generatePath(pLoopPlot, MOVE_BUST_FOG, true, &iPathTurns))
						{
							iPlotValue += 100;
							iPlotValue += GC.getGameINLINE().getSorenRandNum(10, "AI find seafood deep search");
							iPlotValue *= 100;
							iPlotValue /= 100 + GET_PLAYER(getOwner()).AI_cityDistance(pLoopPlot);
							iPlotValue *= 10000;
							iPlotValue /= 100 + getPathCost();
							if (iPlotValue > iBestValue)
							{
								iBestValue = iPlotValue;
								pBestPlot = getPathEndTurnPlot();
								pBestExplorePlot = pLoopPlot;
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestExplorePlot != NULL))
	{
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_BUST_FOG, false, false, MISSIONAI_WORKER_SEA, pBestExplorePlot);
		return true;
	}

	return false;
}

bool CvUnitAI::AI_exploreRessource()
{
	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestExplorePlot = NULL;

	for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);

		if (AI_plotValid(pLoopPlot) && AI_isValidExplore(pLoopPlot))
		{
			int iPlotValue = AI_exploreRessourcePlotValue(pLoopPlot);
			if (iPlotValue > 0)
			{
				if (pLoopPlot->isRevealed(getTeam(), false) && !pLoopPlot->isVisibleEnemyDefender(this))
				{
					BonusTypes eBonus = pLoopPlot->getBonusType();
					// TODO: change this to ask XML for a list of bonuses the unit can harvest
					if (isWhalingBoat())
					{
						if ((pLoopPlot->waterArea()->getNumBonuses(BONUS_WHALE) + pLoopPlot->waterArea()->getNumBonuses(BONUS_HUMPBACK_WHALE)) == 0)
						{
							iPlotValue = 100;
						}
						else if (eBonus == BONUS_WHALE || eBonus == BONUS_HUMPBACK_WHALE)
						{
							iPlotValue += 300;
						}
					}
					// R&R, ray, High Sea Fishing - START
					else if (isFishingBoat())
					{
						if ((pLoopPlot->waterArea()->getNumBonuses(BONUS_HIGH_SEA_FISH) + pLoopPlot->waterArea()->getNumBonuses(BONUS_HIGH_SEA_FISH2) + pLoopPlot->waterArea()->getNumBonuses(BONUS_HIGH_SEA_FISH3) + pLoopPlot->waterArea()->getNumBonuses(BONUS_HIGH_SEA_FISH4)) == 0)
						{
							iPlotValue = 100;
						}
						else if (eBonus == BONUS_HIGH_SEA_FISH || eBonus == BONUS_HIGH_SEA_FISH2 || eBonus == BONUS_HIGH_SEA_FISH3 || eBonus == BONUS_HIGH_SEA_FISH4)
						{
							iPlotValue += 300;
						}
					}
					// R&R, ray, High Sea Fishing - END
				}

				if (iPlotValue > 0)
				{
					if (!(pLoopPlot->isVisibleEnemyUnit(this)))
					{
						if (GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_WORKER_SEA, getGroup(), 3) == 0)
						{
							int iPathTurns;
							if (generatePath(pLoopPlot, MOVE_BUST_FOG, true, &iPathTurns))
							{
								iPlotValue += GC.getGameINLINE().getSorenRandNum(10, "AI find seafood deep search");

								iPlotValue *= 100;
								iPlotValue /= 100 + GET_PLAYER(getOwner()).AI_cityDistance(pLoopPlot);

								iPlotValue *= 10000;
								iPlotValue /= 100 + getPathCost();

								if (iPlotValue > iBestValue)
								{
									iBestValue = iPlotValue;
									pBestPlot = getPathEndTurnPlot();
									pBestExplorePlot = pLoopPlot;
								}
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestExplorePlot != NULL))
	{
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_BUST_FOG, false, false, MISSIONAI_WORKER_SEA, pBestExplorePlot);
		return true;
	}

	return false;
}

int CvUnitAI::AI_exploreRessourcePlotValue(CvPlot* pPlot)
{
	int iValue = 0;

	BonusTypes eBonus = pPlot->getBonusType();

	if (eBonus != NO_BONUS && canGatherResource(pPlot))
	{
		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			int iYieldChange = GC.getBonusInfo(pPlot->getBonusType()).getYieldChange(iYield);
			if (iYieldChange > 0)
			{
				if (getUnitInfo().canGatherYield(iYield))
				{
					iValue += iYieldChange * 100;
				}
			}
		}
	}

	if (iValue > 0)
	{
		if (pPlot->getTerrainType() != (TerrainTypes)GC.getDefineINT("DEEP_WATER_TERRAIN"))
		{
			iValue *= 3;
			iValue /= 4;
		}
	}

	return iValue;
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_gatherResource()
{
	if (canGatherResource(plot()))
	{
		BonusTypes eFishBonus = plot()->getBonusType();
		if (isWhalingBoat() && eFishBonus != NO_BONUS && GC.getBonusInfo(eFishBonus).isWhalingboatWorkable())
		{
			getGroup()->pushMission(MISSION_WHALING);
		}
		else if (isFishingBoat() && eFishBonus != NO_BONUS && GC.getBonusInfo(eFishBonus).isFishingboatWorkable())
		{
			getGroup()->pushMission(MISSION_FISHING);
		}
		return true;
	}

	return false;
}

//End TAC Whaling, ray

// TAC - AI Attack City - koma13 - START

/*
// Returns true if a mission was pushed...
bool CvUnitAI::AI_targetCity(int iFlags)
{
	PROFILE_FUNC();

	CvCity* pTargetCity;
	CvCity* pLoopCity;
	CvCity* pBestCity;
	CvPlot* pAdjacentPlot;
	CvPlot* pBestPlot;
	int iPathTurns;
	int iValue;
	int iBestValue;
	int iLoop;
	int iI;

	iBestValue = 0;
	pBestCity = NULL;

	pTargetCity = area()->getTargetCity(getOwnerINLINE());

	if (pTargetCity != NULL)
	{
		if (AI_potentialEnemy(pTargetCity->getTeam(), pTargetCity->plot()))
		{
			if (!atPlot(pTargetCity->plot()) && generatePath(pTargetCity->plot(), iFlags, true))
			{
				pBestCity = pTargetCity;
			}
		}
	}

	if (pBestCity == NULL)
	{
		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				for (pLoopCity = GET_PLAYER((PlayerTypes)iI).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iI).nextCity(&iLoop))
				{
					if (AI_plotValid(pLoopCity->plot()) && AI_potentialEnemy(GET_PLAYER((PlayerTypes)iI).getTeam(), pLoopCity->plot()))
					{
						if (!atPlot(pLoopCity->plot()) && generatePath(pLoopCity->plot(), iFlags, true, &iPathTurns))
						{
							iValue = 0;

							iValue = GET_PLAYER(getOwnerINLINE()).AI_targetCityValue(pLoopCity, false, false);

							iValue *= 1000;

							if ((area()->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE))
							{
								if (pLoopCity->calculateCulturePercent(getOwner()) < 75)
								{
									iValue /= 2;
								}
							}

							iValue /= (4 + iPathTurns*iPathTurns);

							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestCity = pLoopCity;
							}
						}
					}
				}
			}
		}
	}

	if (pBestCity != NULL)
	{
		iBestValue = 0;
		pBestPlot = NULL;

		if (0 == (iFlags & MOVE_THROUGH_ENEMY))
		{
			for (iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				pAdjacentPlot = plotDirection(pBestCity->getX_INLINE(), pBestCity->getY_INLINE(), ((DirectionTypes)iI));

				if (pAdjacentPlot != NULL)
				{
					if (AI_plotValid(pAdjacentPlot))
					{
						if (!(pAdjacentPlot->isVisibleEnemyUnit(this)))
						{
							if (generatePath(pAdjacentPlot, iFlags, true, &iPathTurns))
							{
								iValue = std::max(0, (pAdjacentPlot->defenseModifier(getTeam()) + 100));

								if (!(pAdjacentPlot->isRiverCrossing(directionXY(pAdjacentPlot, pBestCity->plot()))))
								{
									iValue += (12 * -(GC.getRIVER_ATTACK_MODIFIER()));
								}

								if (!isEnemy(pAdjacentPlot->getTeam(), pAdjacentPlot))
								{
									iValue += 100;
								}

								iValue = std::max(1, iValue);

								iValue *= 1000;

								iValue /= (iPathTurns + 1);

								if (iValue > iBestValue)
								{
									iBestValue = iValue;
									pBestPlot = getPathEndTurnPlot();
								}
							}
						}
					}
				}
			}
		}


		else
		{
			pBestPlot =  pBestCity->plot();
		}

		if (pBestPlot != NULL)
		{
			FAssert(!(pBestCity->at(pBestPlot)) || 0 != (iFlags & MOVE_THROUGH_ENEMY)); // no suicide missions...
			if (!atPlot(pBestPlot))
			{
				getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), iFlags);
				return true;
			}
		}
	}

	return false;
}
*/

bool CvUnitAI::AI_targetCityNative(int iFlags)
{
	PROFILE_FUNC();


	//Native logic is pretty simple overall.
	//They move up to a city. Then hang around if extortion. If not extortion, they attack.

	CvCity* pTargetCity;
	CvCity* pLoopCity;
	CvCity* pBestCity;
	CvPlot* pBestPlot;
	CvPlot* pAdjacentPlot;
	int iPathTurns;
	int iValue;
	int iBestValue;
	int iLoop;
	int iI;

	iBestValue = 0;
	pBestCity = NULL;

	pTargetCity = area()->AI_getTargetCity(getOwnerINLINE());

	if (pTargetCity != NULL)
	{
		if (AI_potentialEnemy(pTargetCity->getTeam(), pTargetCity->plot()))
		{
			if (!atPlot(pTargetCity->plot()) && generatePath(pTargetCity->plot(), iFlags, true))
			{
				pBestCity = pTargetCity;
			}
		}
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			for (pLoopCity = GET_PLAYER((PlayerTypes)iI).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iI).nextCity(&iLoop))
			{
				if (AI_plotValid(pLoopCity->plot()) && AI_potentialEnemy(GET_PLAYER((PlayerTypes)iI).getTeam(), pLoopCity->plot()))
				{
					if (!atPlot(pLoopCity->plot()) && generatePath(pLoopCity->plot(), iFlags, true, &iPathTurns))
					{
						iValue = 0;

						iValue = GET_PLAYER(getOwnerINLINE()).AI_targetCityValue(pLoopCity, false, false);

						iValue *= 1000;

						if (pLoopCity == pBestCity)
						{
							iValue *= 2;
						}

						iValue /= (4 + iPathTurns*iPathTurns);

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							pBestCity = pLoopCity;
						}
					}
				}
			}
		}
	}

	if (pBestCity != NULL)
	{
		iBestValue = 0;
		pBestPlot = NULL;

		if (0 == (iFlags & MOVE_THROUGH_ENEMY))
		{
			for (iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				pAdjacentPlot = plotDirection(pBestCity->getX_INLINE(), pBestCity->getY_INLINE(), ((DirectionTypes)iI));

				if (pAdjacentPlot != NULL)
				{
					if (AI_plotValid(pAdjacentPlot))
					{
						if (!(pAdjacentPlot->isVisibleEnemyUnit(this)))
						{
							if (generatePath(pAdjacentPlot, iFlags, true, &iPathTurns))
							{
								iValue = std::max(0, (pAdjacentPlot->defenseModifier(getTeam()) + 100));

								if (!(pAdjacentPlot->isRiverCrossing(directionXY(pAdjacentPlot, pBestCity->plot()))))
								{
									iValue += (12 * -(GC.getRIVER_ATTACK_MODIFIER()));
								}

								if (!isEnemy(pAdjacentPlot->getTeam(), pAdjacentPlot))
								{
									iValue += 100;
								}

								iValue = std::max(1, iValue);

								iValue *= 1000;

								iValue /= (iPathTurns + 1);

								if (iValue > iBestValue)
								{
									iBestValue = iValue;
									pBestPlot = getPathEndTurnPlot();
								}
							}
						}
					}
				}
			}
		}
		else
		{
			pBestPlot =  pBestCity->plot();
		}

		if (pBestPlot != NULL)
		{
			FAssert(!(pBestCity->at(pBestPlot)) || 0 != (iFlags & MOVE_THROUGH_ENEMY)); // no suicide missions...
			if (!atPlot(pBestPlot) && canMoveInto(*pBestPlot, true)) // bugfix: don't attack plots the unit can't enter - Nightinggale
			{
				getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), iFlags);
				return true;
			}
		}
	}

	return false;
}


// R&R, ray, Natives raiding party - START
// Returns target city
CvCity* CvUnitAI::AI_nativePickRaidTargetCity(int iFlags, int iMaxPathTurns)
{
	PROFILE_FUNC();

	CvCity* pTargetCity;
	CvCity* pLoopCity;
	CvCity* pBestCity;
	int iPathTurns;
	int iValue;
	int iBestValue;
	int iLoop;
	int iI;

	iBestValue = 0;
	pBestCity = NULL;

	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	pTargetCity = area()->AI_getTargetCity(getOwnerINLINE());

	// Don't always go after area target ... don't know how far away it is...
	/* Native_AI_Temp */
	/*
	if (pTargetCity != NULL)
	{
		if (AI_potentialEnemy(pTargetCity->getTeam(), pTargetCity->plot()) || kOwner.AI_isPotentialRaidTarget(pTargetCity->getOwner))
		{
			if (!atPlot(pTargetCity->plot()) && generatePath(pTargetCity->plot(), iFlags, true))
			{
				pBestCity = pTargetCity;
			}
		}
	}
	*/

	if (pBestCity == NULL)
	{
		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (kOwner.AI_isPotentialRaidTarget((PlayerTypes)iI))
				{
					for (pLoopCity = GET_PLAYER((PlayerTypes)iI).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iI).nextCity(&iLoop))
					{
						if (AI_plotValid(pLoopCity->plot()) && (pLoopCity->area() == area()))
						{
							if (!atPlot(pLoopCity->plot()) && generatePath(pLoopCity->plot(), iFlags, true, &iPathTurns, iMaxPathTurns))
							{
								// If city is visible and our force already in position is dominantly powerful or we have a huge force already on the way, pick a different target
								if (iPathTurns > 2 && pLoopCity->isVisible(getTeam(), false))
								{
									/* Native_AI_Temp */
									//int iOurOffense = kOwner.AI_getOurPlotStrength(pLoopCity->plot(), 2, false, false);
									//int iEnemyDefense = kOwner.AI_getEnemyPlotStrength(pLoopCity->plot(), 1, true, false);

									int iRaidingPartyStrength = 100 * kOwner.AI_getOurPlotStrength(pLoopCity->plot(), 1, false, false);
									int iTargetDefenseStrength = kOwner.AI_getEnemyPlotStrength(pLoopCity->plot(), 1, true, false);
									int iTargetCityDefense = std::max(1, pLoopCity->getDefenseModifier());

									if (iRaidingPartyStrength >= (iTargetCityDefense * iTargetDefenseStrength))
									{
										continue;
									}

									/*
									if (kOwner.AI_cityTargetUnitsByPath(pLoopCity, getGroup(), iPathTurns) > (3 * pLoopCity->plot()->getNumVisibleEnemyDefenders(this)))
									{
										continue;
									}
									*/
									/* Native_AI_Temp */
								}

								iValue = 0;

								if (AI_getUnitAIState() == UNITAI_STATE_RAIDING_PARTY)
								{
									iValue = kOwner.AI_targetCityValue(pLoopCity, false, false);
								}
								else
								{
									iValue = kOwner.AI_targetCityValue(pLoopCity, true, true);
								}

								if (pLoopCity == pTargetCity)
								{
									iValue *= 3;
								}

								if ((area()->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE))
								{
									iValue *= 50 + pLoopCity->calculateCulturePercent(getOwnerINLINE());
									iValue /= 50;
								}

								iValue *= 1000;

								/*
								// If city is minor civ, less interesting
								if (GET_PLAYER(pLoopCity->getOwnerINLINE()).isMinorCiv() || GET_PLAYER(pLoopCity->getOwnerINLINE()).isBarbarian())
								{
									iValue /= 2;
								}

								// If stack has poor bombard, direct towards lower defense cities
								iPathTurns += std::min(12, getGroup()->getBombardTurns(pLoopCity) / 4);
								*/

								iValue /= (4 + iPathTurns * iPathTurns);

								if (iValue > iBestValue)
								{
									iBestValue = iValue;
									pBestCity = pLoopCity;
								}
							}
						}
					}
				}
			}
		}
	}

	return pBestCity;
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_nativeGoToRaidTargetCity(int iFlags, int iMaxPathTurns, CvCity* pTargetCity)
{
	PROFILE_FUNC();

	CvPlot* pAdjacentPlot;
	CvPlot* pBestPlot;
	int iPathTurns;
	int iValue;
	int iBestValue;
	int iI;

	if (pTargetCity == NULL)
	{
		pTargetCity = AI_nativePickRaidTargetCity(iFlags, iMaxPathTurns);
	}

	if (pTargetCity != NULL)
	{
		PROFILE("CvUnitAI::AI_targetCity plot attack");
		iBestValue = 0;
		pBestPlot = NULL;

		if (0 == (iFlags & MOVE_DIRECT_RAID))
		{
			for (iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				pAdjacentPlot = plotDirection(pTargetCity->getX_INLINE(), pTargetCity->getY_INLINE(), ((DirectionTypes)iI));
				if (pAdjacentPlot != NULL)
				{
					if (AI_plotValid(pAdjacentPlot))
					{
						if (!(pAdjacentPlot->isVisibleEnemyUnit(this)))
						{
							if (generatePath(pAdjacentPlot, iFlags, true, &iPathTurns, iMaxPathTurns))
							{
								iValue = std::max(0, (pAdjacentPlot->defenseModifier(getTeam(), false) + 100));

								if (!(pAdjacentPlot->isRiverCrossing(directionXY(pAdjacentPlot, pTargetCity->plot()))))
								{
									iValue += (12 * -(GC.getRIVER_ATTACK_MODIFIER()));
								}

								if (!isEnemy(pAdjacentPlot->getTeam(), pAdjacentPlot))
								{
									iValue += 100;
								}

								if (atPlot(pAdjacentPlot))
								{
									iValue += 50;
								}

								iValue = std::max(1, iValue);
								iValue *= 1000;
								iValue /= (iPathTurns + 1);

								if (iValue > iBestValue)
								{
									iBestValue = iValue;
									pBestPlot = getPathEndTurnPlot();
								}
							}
						}
					}
				}
			}
		}
		else
		{
			pBestPlot = pTargetCity->plot();
		}

		if (pBestPlot != NULL)
		{
			FAssert(!(pTargetCity->at(pBestPlot)) || 0 != (iFlags & MOVE_DIRECT_RAID)); // no suicide missions...
			if (!atPlot(pBestPlot))
			{
				getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), iFlags);
				return true;
			}
		}
	}

	return false;
}
// R&R, ray, Natives raiding party - END


bool CvUnitAI::AI_extortCity()
{
	int iBestValue = 0;
	CvCity* pBestCity = NULL;

	for (int iDirection = 0; iDirection < NUM_DIRECTION_TYPES; ++iDirection)
	{
		CvPlot* pLoopPlot = plotDirection(getX_INLINE(), getY_INLINE(), (DirectionTypes)iDirection);
		if (pLoopPlot != NULL)
		{
			if (pLoopPlot->isCity())
			{
				if (AI_potentialEnemy(pLoopPlot->getTeam(), pLoopPlot))
				{
					CvTeamAI& kPlotTeam = GET_TEAM(pLoopPlot->getTeam());

					if (GET_TEAM(getTeam()).AI_getWarPlan(pLoopPlot->getTeam()) == WARPLAN_EXTORTION)
					{
						int iValue = GC.getGame().getSorenRandNum(100, "AI choose extort city");

						if (iValue > iBestValue)
						{
							iValue = iBestValue;
							pBestCity = pLoopPlot->getPlotCity();
						}
					}
				}
			}
		}
	}

	if (pBestCity != NULL)
	{
		getGroup()->pushMission(MISSION_SKIP);
		return true;
	}
	return false;
}


// TAC - AI Attack City - koma13, jdog5000(BBAI) - START

//// Returns true if a mission was pushed...
//bool CvUnitAI::AI_bombardCity()
//{
//	CvCity* pBombardCity;
//
//	if (canBombard(plot()))
//	{
//		pBombardCity = bombardTarget(plot());
//		FAssertMsg(pBombardCity != NULL, "BombardCity is not assigned a valid value");
//
//		// do not bombard cities with no defenders
//		int iDefenderStrength = pBombardCity->plot()->AI_sumStrength(NO_PLAYER, getOwnerINLINE(), DOMAIN_LAND, /*bDefensiveBonuses*/ true, /*bTestAtWar*/ true, false);
//		if (iDefenderStrength == 0)
//		{
//			return false;
//		}
//
//		// do not bombard cities if we have overwhelming odds
//		int iAttackOdds = getGroup()->AI_attackOdds(pBombardCity->plot(), /*bPotentialEnemy*/ true);
//		if (iAttackOdds > 95)
//		{
//			return false;
//		}
//
//		// could also do a compare stacks call here if we wanted, the downside of that is that we may just have a lot more units
//		// we may not want to suffer high casualties just to save a turn
//		//getGroup()->AI_compareStacks(pBombardCity->plot(), /*bPotentialEnemy*/ true, /*bCheckCanAttack*/ true, /*bCheckCanMove*/ true);
//		//int iOurStrength = pBombardCity->plot()->AI_sumStrength(getOwnerINLINE(), NO_PLAYER, DOMAIN_LAND, false, false, false)
//
//		if (pBombardCity->getDefenseDamage() < ((GC.getMAX_CITY_DEFENSE_DAMAGE() * 3) / 4))
//		{
//			getGroup()->pushMission(MISSION_BOMBARD);
//			return true;
//		}
//	}
//
//	return false;
//}

/*	BETTER_BTS_AI_MOD, 03/29/10, jdog5000 (War tactics AI, Efficiency):
	Returns target city. K-Mod: heavily edited */
CvCity* CvUnitAI::AI_pickTargetCity(MovementFlags eFlags, int iMaxPathTurns,
	bool bHuntBarbs)
{
	PROFILE_FUNC();

	CvCity* pBestCity = NULL;
	int iBestValue = 0;
	CvTeamAI const& kOurTeam = GET_TEAM(getTeam()); // advc
	// K-Mod
	CvPlayerAI const& kOwner = GET_PLAYER(getOwner());
	int iOurOffence = -1; // We calculate this for the first city only.
	CvUnit* pBestTransport = NULL;
	/*	iLoadTurns < 0 implies we should look for a transport;
		otherwise, it is the number of turns to reach the transport.
		Also, we only consider using transports if we aren't in enemy territory. */
	int iLoadTurns = (isEnemy(getPlot()) ? MAX_INT : -1);
	//GroupPathFinder transportPath;
	KmodPathFinder& kTransportPath = CvSelectionGroup::getClearPathFinder(); // advc.opt
	// K-Mod end

	/*
	CvCity* pTargetCity =  // advc.300:
		(isBarbarian() && getArea().getCitiesPerPlayer(BARBARIAN_PLAYER) <= 0 ? NULL :
			getArea().AI_getTargetCity(getOwner()));
	*/
	CvCity* pTargetCity = area()->AI_getTargetCity(getOwner());

	for (int i = 0; i < (bHuntBarbs ? MAX_PLAYERS : GC.getMAX_PLAYERS()); i++)
	{
		CvPlayer const& kTargetPlayer = GET_PLAYER((PlayerTypes)i);
		if (!kTargetPlayer.isAlive() ||
			!kOurTeam.AI_mayAttack(kTargetPlayer.getTeam()))
		{
			continue;
		}
		FOREACH_CITY_OF_OWNER(pLoopCity, kTargetPlayer)
		{
			if (!pLoopCity->isArea(*area()))
				//|| !AI_plotValid(pLoopCity->plot()) // advc.opt: area check suffices
			{
				continue;
			}
			if (!AI_mayAttack(kTargetPlayer.getTeam(), pLoopCity->getPlot()))
				continue;
			if (kOwner.AI_deduceCitySite(*pLoopCity))
			{
				// K-Mod. Look for either a direct land path, or a sea transport path.
				int iPathTurns = MAX_INT;
				bool const bLandPath = generatePath(pLoopCity->plot(), eFlags, true,
					&iPathTurns, iMaxPathTurns);
				// TODO: the check for costal is not enough, we have to consider shallow etc.
				if (pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()) && (pBestTransport != NULL || iLoadTurns < 0))
				{
					// add a random bias in favour of land paths, so that not all stacks try to use boats.
					//int iLandBias =AI_getBirthmark()%6 +(AI_getBirthmark() % (bLandPath ? 3 : 6) ? 6 : 1);
					/*	<advc.001> I guess the intention is to roll a 6-sided die
						(birthmark%6 plus at least 1 in the end) with another 5 added for
						some portion of units - depending on whether there is a land path. */
					int iLandBias = (AI_getBirthmark() % 6) + 1;
					/*	Now, should 2 in 3 units have increased land bias
						when there is a land path and otherwise 5 in 6?
						You'd think that there should be more land bias
						when there is a land path. So I'm going to flip the condition,
						i.e. 1 in 3 units have increased land bias
						when there is a land path and otherwise 1 in 6. */
					if ((AI_getBirthmark() % (bLandPath ? 3 : 6)) == 0)
						iLandBias += 5;
					/*  (I also wonder if such randomization would fit better near the
						"we have to walk" comment in AI_attackCityMove. AI_pickTargetCity
						only decides which units target which city, not how they
						ultimately move there.) */ // </advc.001>
					if (pBestTransport != NULL && iPathTurns > iLandBias + 2)
					{
						pBestTransport = AI_findTransport(UNITAI_ASSAULT_SEA, eFlags,
							std::min(iMaxPathTurns, iPathTurns));
						if (pBestTransport != NULL)
						{
							generatePath(&pBestTransport->getPlot(), eFlags, true, &iLoadTurns);
							FAssert(iLoadTurns > 0 && iLoadTurns < MAX_INT);
							iLoadTurns += iLandBias;
							FAssert(iLoadTurns > 0);
						}
						// just to indicate the we shouldn't look for a transport again.
						else iLoadTurns = MAX_INT;
					}
					int iMaxTransportTurns = std::min(iMaxPathTurns, iPathTurns) - iLoadTurns;
					if (pBestTransport != NULL && iMaxTransportTurns > 0)
					{
						kTransportPath.SetSettings(pBestTransport->getGroup(),
							eFlags & MOVE_DECLARE_WAR, iMaxTransportTurns,
							GLOBAL_DEFINE_MOVE_DENOMINATOR);
						if (kTransportPath.GeneratePath(&pLoopCity->getPlot()))
						{
							// faster by boat
							FAssert(kTransportPath.GetPathTurns() + iLoadTurns <= iPathTurns);
							iPathTurns = kTransportPath.GetPathTurns() + iLoadTurns;
						}
					}
				}

				if (iPathTurns >= iMaxPathTurns)
					continue;
				/*	If city is visible and our force already in position
					is dominantly powerful or we have a huge force
					already on the way, pick a different target */
				int iEnemyDefence = -1; // used later.
				int iOffenceEnRoute = kOwner.AI_cityTargetStrengthByPath(
					pLoopCity, getGroup(), iPathTurns);
				if (pLoopCity->isVisible(kOurTeam.getID(), false))
				{
					iEnemyDefence = kOwner.AI_localDefenceStrength(
						pLoopCity->plot(), NO_TEAM, DOMAIN_LAND,
						/*	advc.001: Probably a remnant of bDefensiveBonuses=true
							in BBAI's AI_getEnemyPlotStrength. */
							//true,
						iPathTurns > 1 ? 2 : 0);
					if (iPathTurns > 2)
					{
						int iAttackRatio = ((GC.getMAX_CITY_DEFENSE_DAMAGE() -
							pLoopCity->getDefenseDamage()) *
							BBAI_SKIP_BOMBARD_BASE_STACK_RATIO +
							pLoopCity->getDefenseDamage() *
							BBAI_SKIP_BOMBARD_MIN_STACK_RATIO) /
							std::max(1, GC.getMAX_CITY_DEFENSE_DAMAGE());
						if (100 * iOffenceEnRoute > iAttackRatio * iEnemyDefence)
							continue;
					}
				}
				if (iOurOffence == -1)
				{
					/*  note: with bCheckCanAttack=false, AI_sumStrength should be
						roughly the same regardless of which city we are targeting.
						... except if lots of our units have a hills-attack promotion
						or something like that. */
					iOurOffence = AI_getGroup()->AI_sumStrength(pLoopCity->plot());
				}
				FAssert(iOurOffence > 0);
				int iTotalOffence = iOurOffence + iOffenceEnRoute;

				int iValue = 0;
				if (AI_getUnitAIType() == UNITAI_ATTACK_CITY) //lemming?
					iValue = kOwner.AI_targetCityValue(pLoopCity, false, false);
				else iValue = kOwner.AI_targetCityValue(pLoopCity, true, true);
				// adjust value based on defensive bonuses
				{
					int iMod = std::min(8, AI_getGroup()->AI_getBombardTurns(pLoopCity)) *
						pLoopCity->getDefenseModifier(/*false*/) / 8
						+ (pLoopCity->getPlot().isHills() ?
							GLOBAL_DEFINE_HILLS_EXTRA_DEFENSE : 0);
					iValue *= std::max(25, 125 - iMod);
					iValue /= 25; // the denominator is arbitrary, and unimportant.
					/*  note: the value reduction from high defences
						which are bombardable should not be more than
						the value reduction from simply having higher iPathTurns. */
				}
				// prefer cities which are close to the main target.
				if (pLoopCity == pTargetCity)
					iValue *= 2;
				else if (pTargetCity != NULL)
				{
					int iStepsFromTarget = stepDistance(pLoopCity->getX(), pLoopCity->getY(),
						pTargetCity->getX(), pTargetCity->getY());
					iValue *= 124 - 2 * std::min(12, iStepsFromTarget);
					iValue /= 100;
				}
				if (area()->getAreaAIType(kOurTeam.getID()) == AREAAI_DEFENSIVE)
				{
					iValue *= 100 + pLoopCity->calculateCulturePercent(getOwner()); // was 50
					iValue /= 125; // was 50 (unimportant)
				}
				/*  boost value if we can see that the city is poorly defended,
					or if our existing armies need help there */
				if (pLoopCity->isVisible(kOurTeam.getID(), false) && iPathTurns < 6)
				{
					FAssert(iEnemyDefence != -1);
					if (iOffenceEnRoute * 3 > iEnemyDefence && iOffenceEnRoute < iEnemyDefence)
					{
						iValue *= 100 + (9 * iTotalOffence > 10 * iEnemyDefence ? 30 : 15);
						iValue /= 100;
					}
					else if (iOurOffence > iEnemyDefence)
					{
						// don't boost it by too much, otherwise human players will exploit us. :(
						int iCap = 100 + 100 * (6 - iPathTurns) / 5;
						iValue *= std::min(iCap, 100 * iOurOffence / std::max(1, iEnemyDefence));
						iValue /= 100;
						/*	an additional bonus if we're already adjacent
							(we can afford to be generous with this bonus,
							because the enemy has no time to bring in reinforcements) */
						if (iPathTurns <= 1)
						{
							iValue *= std::min(300, 150 * iOurOffence / std::max(1, iEnemyDefence));
							iValue /= 100;
						}
					}
				}
				/*	Reduce the value if we can see, or remember,
					that the city is well defended.
					Note. This adjustment can be more heavy-handed
					because it is harder to feign strong defence than weak defence.
					advc (note): Barbarians are never dissuaded (no strength memory). */
				iEnemyDefence = kOurTeam.AI_strengthMemory().get(pLoopCity->getPlot());
				if (iEnemyDefence > iTotalOffence)
				{
					/*	a more sensitive adjustment than usual
						(w/ modifier on the denominator),
						so as not to be too deterred before bombarding. */
					iEnemyDefence *= 130;
					iEnemyDefence /= 130 + (bombardRate() > 0 ? pLoopCity->getDefenseModifier(/*false*/) : 0);
					WarPlanTypes eWarPlan = kOurTeam.AI_getWarPlan(pLoopCity->getTeam());
					/*	If we aren't fully committed to the war, then focus on
						taking easy cities - but try not to be completely predictable. */
					bool bCherryPick = (eWarPlan == WARPLAN_LIMITED ||
						eWarPlan == WARPLAN_PREPARING_LIMITED ||
						eWarPlan == WARPLAN_DOGPILE);
					bCherryPick = bCherryPick && (AI_unitBirthmarkHash(
						GC.getGame().getElapsedGameTurns() / 4) % 4 != 0);

					int iBase = (bCherryPick ? 100 : 110);
					/*	an uneven comparison, just in case we can get
						some air support or other help somehow. */
					if (100 * iEnemyDefence > iBase * iTotalOffence)
					{
						iValue *= bCherryPick ?
							std::max(20, (3 * iBase * iTotalOffence - iEnemyDefence) /
								(2 * iEnemyDefence)) :
							std::max(33, iBase * iTotalOffence / iEnemyDefence);
						iValue /= 100;
					}
				}
				/*	A const-random component, so that the AI
					doesn't always go for the same city. */
				iValue *= 80 + AI_unitPlotHash(pLoopCity->plot()) % 41;
				iValue /= 100;
				iValue *= 1000;
				// If city is minor civ, less interesting
				/*
				if (GET_PLAYER(pLoopCity->getOwner()).isMinorCiv() ||
					GET_PLAYER(pLoopCity->getOwner()).isBarbarian())
				{
				*/
				if (GET_PLAYER(pLoopCity->getOwner()).isNative())
				{
					//iValue /= 2;
					iValue /= 3; // K-Mod
				}
				// If stack has poor bombard, direct towards lower defense cities
				//iPathTurns += std::min(12, getGroup()->getBombardTurns(pLoopCity)/4);
				//iPathTurns += bombardRate() > 0 ? std::min(5, getGroup()->getBombardTurns(pLoopCity)/3) : 0; // K-Mod
				// (already taken into account.)

				iValue /= 8 + SQR(iPathTurns); // was 4+
				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestCity = pLoopCity;
				}
			} // end if revealed.
			/*	K-Mod. If no city in the area is revealed,
				then assume the AI is able to deduce the position of the closest city. */
			else if (iBestValue == 0 /* && !pLoopCity->isBarbarian()*/ && (pBestCity == NULL ||
				stepDistance(getX(), getY(), pBestCity->getX(), pBestCity->getY()) >
				stepDistance(getX(), getY(), pLoopCity->getX(), pLoopCity->getY())))
			{
				if (generatePath(&pLoopCity->getPlot(), eFlags, true, 0, iMaxPathTurns))
					pBestCity = pLoopCity;
			}
			// K-Mod end
		}
	}

	return pBestCity;
}

/*	BETTER_BTS_AI_MOD, 03/29/10, jdog5000 (War tactics AI, Efficiency):
	(K-Mod has apparently merged BBAI's AI_goToTargetBarbCity into this) */
bool CvUnitAI::AI_goToTargetCity(MovementFlags eFlags,  // advc: some refactoring
	int iMaxPathTurns, CvCity* pTargetCity)
{
	PROFILE_FUNC();

	if (pTargetCity == NULL)
		pTargetCity = AI_pickTargetCity(eFlags, iMaxPathTurns);
	if (pTargetCity == NULL)
		return false;
	FAssert(pTargetCity->isArea(*area())); // advc: This function isn't for naval assault

	CvPlot* pEndTurnPlot = NULL; // K-Mod
	CvPlot* pBestPlot = NULL;
	if (!(eFlags & MOVE_THROUGH_ENEMY))
	{
		int iBestValue = 0;
		FOR_EACH_ADJ_PLOT(pTargetCity->plot(), 
		{
			if (//!AI_plotValid(pAdjacentPlot)
				!isArea(*pAdjacentPlot->area())) // advc.opt
			{
				continue;
			}
			// K-Mod TODO: consider fighting for the best plot.
			// <advc.083> For a start, let's check for EnemyDefender instead of EnemyUnit.
			if (pAdjacentPlot->isVisibleEnemyDefender(this) &&
				/*  Make sure that Barbarians can't be staved off by surrounding
					cities with units. AI civs don't seem to have that problem. */
				!isBarbarian())
			{
				continue; // </advc.083>
			}
			int iPathTurns;
			if (!generatePath(pAdjacentPlot, eFlags, true, &iPathTurns, iMaxPathTurns))
				continue;
			if (iPathTurns <= iMaxPathTurns &&
				/*  advc.083: This was previously asserted after the loop ("no suicide missions...")
					but not actually guaranteed by the loop. If the pathfinder thinks
					that it's OK to move through the city, then we might as well
					pick a suboptimal (but nearby) plot to attack from. */
				!pTargetCity->at(getPathEndTurnPlot()))
			{
				int iValue = std::max(0, 100 +
					//pAdjacentPlot->defenseModifier(getTeam(), false)
					AI_plotDefense(pAdjacentPlot)); // advc.012
				if (!pAdjacentPlot->isRiverCrossing(
					directionXY(*pAdjacentPlot, pTargetCity->getPlot())))
				{
					iValue += (-12 * GLOBAL_DEFINE_RIVER_ATTACK_MODIFIER);
				}
				if (!isEnemy(*pAdjacentPlot))
					iValue += 100;
				if (at(*pAdjacentPlot))
					iValue += 50;
				iValue = std::max(1, iValue);
				iValue *= 1000;
				iValue /= iPathTurns + 1;
				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					//pBestPlot = &getPathEndTurnPlot();
					// K-Mod
					pBestPlot = pAdjacentPlot;
					pEndTurnPlot = getPathEndTurnPlot();
					// K-Mod end
				}
			}
		}) // FOR_EACH
	}
	else
	{
		pBestPlot = pTargetCity->plot();
		/*	K-Mod. As far as I know, nothing actually uses MOVE_THROUGH_ENEMY here..
			but that doesn't mean we should let the code be wrong. */
		int iPathTurns;
		if (!generatePath(pBestPlot, eFlags, true, &iPathTurns, iMaxPathTurns) ||
			iPathTurns > iMaxPathTurns)
		{
			return false;
		}
		pEndTurnPlot = getPathEndTurnPlot();
		// K-mod end
	}

	if (pBestPlot == NULL || at(*pEndTurnPlot))
		return false;

	/*  <advc.001t> Needed when called from AI_attackMove. Attack stacks aren't supposed
		to declare war, and they shouldn't move into enemy cities when war is imminent. */
	if (!(eFlags & MOVE_DECLARE_WAR) && GET_TEAM(getTeam()).
		AI_isSneakAttackReady(pTargetCity->getTeam()))
	{
		/*
		TeamTypes eBestPlotTeam = pBestPlot->getTeam();
		if (eBestPlotTeam != NO_TEAM && GET_TEAM(eBestPlotTeam).getMasterTeam() ==
			GET_TEAM(pTargetCity->getTeam()).getMasterTeam())
		*/ 
		const TeamTypes eBestPlotTeam = pBestPlot->getTeam();
		if (eBestPlotTeam != NO_TEAM && eBestPlotTeam ==
			GET_TEAM(pTargetCity->getTeam()).getID())
		{
			return false;
		}
	} // </advc.001t>

	//pushGroupMoveTo(*pBestPlot, eFlags);
	// K-Mod start
	if (AI_considerPathDOW(pEndTurnPlot, eFlags))
	{	// <advc.163>
		if (!canMove())
			return true; // </advc.163>
		/*  regenerate the path, just in case we want to take a different route after the DOW
			(but don't bother recalculating the best destination)
			Note. if the best destination happens to be on the border,
			and has a stack of defenders on it, this will make us attack them.
			That's bad. I'll try to fix that in the future. */
		if (!generatePath(pBestPlot, eFlags, false))
			return false;
		CvPlot* pEnemyPlot = pEndTurnPlot; // advc.001t
		pEndTurnPlot = getPathEndTurnPlot();
		// <advc.139> Don't move through city that is about to be lost
		CvCityAI const* pPlotCity = &pEndTurnPlot->getPlotCity()->AI();
		if (pPlotCity != NULL && pPlotCity->AI_isEvacuating())
			return false; // </advc.139>
		// <advc.001t>
		if (!isEnemy(*pEndTurnPlot))
		{
			// This will trigger a few times in most games
			/*FAssertMsg(isEnemy(pEndTurnPlot->getTeam()),
				"Known issue: AI may change its mind about the path to the target city "
				"after declaring war; temporary fix: stick to the original path.");*/
			if (isEnemy(*pEnemyPlot))
				pEndTurnPlot = pEnemyPlot;
			else FAssert(isEnemy(pEnemyPlot->getTeam()));
			/*  If the else... assert fails, it's probably b/c the stack has multiple
				moves and there is an intermediate tile that requires a DoW. So this
				can be fine. Could check this through getPathFinder().GetEndNode()
				like it's done in AI_considerPathDOW -- tbd.? */
		} // </advc.001t>
	}
	pushGroupMoveTo(*pEndTurnPlot,
		// I'm going to use MISSIONAI_ASSAULT signal to our spies and other units that we're attacking this city.
		eFlags, false, false, MISSIONAI_ASSAULT, pTargetCity->plot());
	// K-Mod end
	return true;
}

bool CvUnitAI::AI_pillageAroundCity(CvCity* pTargetCity, int iBonusValueThreshold,
	MovementFlags eFlags, int iMaxPathTurns)
{
	PROFILE_FUNC();
	// K-Mod
	if (!isEnemy(pTargetCity->getTeam()) &&
		!AI_getGroup()->AI_isDeclareWar(pTargetCity->plot()))
	{
		return false;
	} // K-Mod end
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestPillagePlot = NULL;
	int iBestValue = 0;
	//for (CityPlotIter it(*pTargetCity); it.hasNext(); ++it)
	FOR_EACH_NON_CENTER_CITY_PLOT(it, pTargetCity)
	{
		CvPlot& kPlot = *it;
		if (/*AI_plotValid(kPlot)*/AI_canEnterByLand(*kPlot.area()) && // advc.opt
			kPlot.getTeam() == pTargetCity->getTeam() && // advc.opt: Moved up
			/*!kPlot.isBarbarian() && */AI_mayAttack(kPlot) &&
			canPillage(&kPlot) && !kPlot.isVisibleEnemyUnit(this) &&
			!GET_PLAYER(getOwner()).AI_isAnyPlotTargetMissionAI(
				kPlot, MISSIONAI_PILLAGE, getGroup()))
		{
			int iPathTurns;
			if (generatePath(&kPlot, eFlags, true, &iPathTurns, iMaxPathTurns))
			{
				if (getPathFinder().getFinalMoves() == 0)
					iPathTurns++;
				if (iPathTurns <= iMaxPathTurns)
				{
					int iValue = AI_pillageValue(&kPlot);
					// <advc.083> Don't use a big stack to pillage every road
					if (iValue <= getGroup()->getNumUnits() * 4)
						continue; // </advc.083> 
					iValue *= (1000 + 30 *
						/*  advc.012: This seems to be about a single unit, so
							noDefensiveBonus should be checked.
							A hill bias might make sense b/c of Iron and Copper,
							but that's for AI_pillageValue to decide. */
						(noDefensiveBonus() ? 0 : AI_plotDefense(&kPlot)));
					//(pLoopPlot->defenseModifier(getTeam(),false));
					//iValue /= (iPathTurns + 1);
					iValue /= std::max(1, iPathTurns); // K-Mod

					/*	if not at war with this plot owner, then devalue plot
						if we are already inside this owner's borders
						(because declaring war will pop us some unknown distance away) */
					if (!isEnemy(kPlot) && getPlot().getTeam() == kPlot.getTeam())
						iValue /= 10;
					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestPlot = getPathEndTurnPlot();
						pBestPillagePlot = &kPlot;
					}
				}
			}
		}
	}

	if (pBestPlot != NULL && pBestPillagePlot != NULL)
	{
		/*if (atPlot(pBestPillagePlot) && !isEnemy(pBestPillagePlot->getTeam())) {
			//getGroup()->groupDeclareWar(pBestPillagePlot, true);
			// rather than declare war, just find something else to do, since we may already be deep in enemy territory
			return false;
		}*/ // BtS - disabled by K-Mod. (also see new code at top.)
		// K-Mod
		FAssert(AI_getGroup()->AI_isDeclareWar(/* advc: */ pBestPillagePlot));
		if (AI_considerPathDOW(pBestPlot, eFlags))
		{	// <advc.163>
			if (!canMove())
				return true; // </advc.163>
			int iPathTurns;
			if (!generatePath(pBestPillagePlot, eFlags, true, &iPathTurns))
				return false;
			pBestPlot = getPathEndTurnPlot();
		} // K-Mod end
		if (at(*pBestPillagePlot))
		{
			/*	advc.083: K-Mod had turned this check into an assertion.
				Seems that it can fail in rare situations when passing through
				foreign borders while war is (becomes?) imminent, and then it's
				better to keep moving and delay the DoW. */
			if (isEnemy(*pBestPillagePlot))
			{
				getGroup()->pushMission(MISSION_PILLAGE, -1, -1, NO_MOVEMENT_FLAGS,
					false, false, MISSIONAI_PILLAGE, pBestPillagePlot);
				return true;
			}
		}
		else
		{
			pushGroupMoveTo(*pBestPlot, eFlags, false, false,
				MISSIONAI_PILLAGE, pBestPillagePlot);
			return true;
		}
	}

	return false;
}

/*	This function has been completely rewritten (and greatly simplified) for K-Mod
	(previously revised by BBAI) */
bool CvUnitAI::AI_bombardCity()
{
	// check if we need to declare war before bombarding!
	FOR_EACH_ADJ_PLOT(plot(),
	{
		if (!pAdjacentPlot->isCity())
			continue;
		if (AI_considerDOW(*pAdjacentPlot))
		{	// <advc.163>
			if (!canMove())
				return true; // </advc.163>
		}
		break; // assume there can only be one city adjacent to us
	}) // FOR_EACH

	if (!canBombard(&getPlot()))
		return false;

	CvCity* pBombardCity = bombardTarget(&getPlot());

	FAssert(pBombardCity != NULL);

	int const iAttackOdds = AI_getGroup()->AI_attackOdds(pBombardCity->plot(), true);
	int iBase = BBAI_SKIP_BOMBARD_BASE_STACK_RATIO;
	int iMin = BBAI_SKIP_BOMBARD_MIN_STACK_RATIO;
	int const iBombardTurns = AI_getGroup()->AI_getBombardTurns(pBombardCity);
	// <advc.004c>
	if (iBombardTurns <= 0)
		return false; // </advc.004c>
	iBase = (iBase * (GC.getMAX_CITY_DEFENSE_DAMAGE() - pBombardCity->getDefenseDamage()) +
		iMin * pBombardCity->getDefenseDamage()) /
		std::max(1, GC.getMAX_CITY_DEFENSE_DAMAGE());
	int iThreshold = (iBase * (100 - iAttackOdds) +
		(1 + iBombardTurns / 2) * iMin * iAttackOdds) /
		(100 + (iBombardTurns / 2) * iAttackOdds);
	int iComparison = AI_getGroup()->AI_compareStacks(pBombardCity->plot(), true);
	if (iComparison > iThreshold)
	{
		if (gUnitLogLevel > 2) logBBAI("      Stack skipping bombard of %S with compare %d, starting odds %d, bombard turns %d, threshold %d", pBombardCity->getName().GetCString(), iComparison, iAttackOdds, iBombardTurns, iThreshold);
		return false;
	}
	// <advc.004c>
	CvUnit* pBombardUnit = AI_getGroup()->AI_bestUnitForMission(MISSION_BOMBARD);
	if (pBombardUnit == NULL)
	{
		FErrorMsg("canBombard but no bombard unit found");
		return false;
	}
	// (Not sure if other types of groups would manage to reunite)
	if (AI_getUnitAIType() == UNITAI_ATTACK_CITY)
		pBombardUnit->joinGroup(NULL);
	pBombardUnit-> // </advc.004c>
		getGroup()->pushMission(MISSION_BOMBARD,
			/* <K-Mod> */ -1, -1, NO_MOVEMENT_FLAGS, false, false,
			MISSIONAI_ASSAULT, pBombardCity->plot()); // </K-Mod>
	return true;
}

// This function has been been heavily edited for K-Mod.
bool CvUnitAI::AI_cityAttack(int iRange, int iOddsThreshold,
	// advc (comment): No caller uses eFlags anymore (not since K-Mod 1.15)
	MovementFlags eFlags, bool bFollow)
{
	PROFILE_FUNC();

	FAssert(canMove());

	CvPlot* pBestPlot = NULL;
	int iBestValue = 0;
	bool const bDeclareWar = (eFlags & MOVE_DECLARE_WAR);
	//for (SquareIter it(*this, bFollow ? 1 : AI_searchRange(iRange), false);
	//	it.hasNext(); ++it)
	FOR_EACH_NON_CENTER_PLOT_IN_RANGE_OF(plot(), bFollow ? 1 : AI_searchRange(iRange),
	{
		CvPlot& p = *pLoopPlot;

		//if (!AI_plotValid(p)) continue; // advc.opt
		if (p.isCity() && /* advc.opt: */ AI_canEnterByLand(*p.area()) &&
			(bDeclareWar ? AI_mayAttack(p.getTeam(), p) : isEnemy(p)))
		{
			int iPathTurns;
			if ((bFollow ? canMoveOrAttackInto(&p, bDeclareWar) :
				generatePath(&p, eFlags, true, &iPathTurns, iRange)))
			{
				int iValue = (!AI_isAnyEnemyDefender(p) ? 100 :
					AI_getGroup()->AI_getWeightedOdds(&p, true));
				if (iValue >= iOddsThreshold)
				{
					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestPlot = (bFollow ? &p : getPathEndTurnPlot());
					}
				}
			}
		}
	}) // FOR_EACH

	if (pBestPlot != NULL)
	{
		FAssert(!at(*pBestPlot));
		// K-Mod
		if (AI_considerPathDOW(pBestPlot, eFlags))
		{	// <advc.163>
			if (!canMove())
				return true; // </advc.163>
			// after DOW, we might not be able to get to our target this turn... but try anyway.
			if (!generatePath(pBestPlot, eFlags, false))
				return false;
			if (bFollow && pBestPlot != getPathEndTurnPlot())
				return false;
			pBestPlot = getPathEndTurnPlot();
		}
		if (bFollow && !AI_isAnyEnemyDefender(*pBestPlot))
		{
			FAssert(pBestPlot->getPlotCity() != 0);
			// we need to ungroup this unit so that we can move into the city.
			joinGroup(NULL);
			bFollow = false;
		}
		// K-Mod end
		//pushGroupMoveTo(*pBestPlot, eFlags | (bFollow ?
		//	static_cast<MovementFlags>(MOVE_DIRECT_ATTACK | MOVE_SINGLE_ATTACK) : NO_MOVEMENT_FLAGS));
		const int iAttackFlags = MOVE_DIRECT_ATTACK | MOVE_SINGLE_ATTACK | (int)eFlags;
		pushGroupMoveTo(*pBestPlot,(bFollow ?
			static_cast<MovementFlags>(iAttackFlags) : NO_MOVEMENT_FLAGS));
		return true;
	}

	return false;
}

// Replacing K-Mod's (incorrectly implemented) ROUND_DIVIDE (CvGlobals.h)
inline int round(int iDividend, int iDivisor)
{
	int iSign = ((iDividend ^ iDivisor) >= 0 ? 1 : -1);
	return (iDividend + iSign * iDivisor / 2) / iDivisor;
}

/*	This function has been been written for K-Mod.
	(it started getting messy, so I deleted most of the old code)
	bFollow implies AI_follow conditions - ie. not everyone in the group can move,
	and this unit might not be the group leader. */
bool CvUnitAI::AI_anyAttack(int iRange, int iOddsThreshold, MovementFlags eFlags,
	int iMinStack, bool bAllowCities, bool bFollow)
{
	PROFILE_FUNC();

	FAssert(canMove());

	/*
	if (AI_rangeAttack(iRange))
	{
		return true;
	}
	*/

	// Workaround for macro parser not dealing with angle brackets during expansion
	typedef std::pair<int, int> IntPair;

	int const iSearchRange = (bFollow ? 1 : AI_searchRange(iRange));
	// <advc.128> Within this range, the AI is able see to units on hidden tiles.
	int const iSearchRangeRand = std::max(1,
		round(iSearchRange * /*m_iSearchRangeRandPercent*/100, 100)); // </advc.128>
	bool const bDeclareWar = (eFlags & MOVE_DECLARE_WAR);
	CvPlot* pBestPlot = NULL;
	int iBestOdds = iOddsThreshold - 1; // advc
	CvTeamAI const& kOurTeam = GET_TEAM(getTeam());
	//for (SquareIter it(*this, iSearchRange, false); it.hasNext(); ++it)

	// WTP: Left out stuff related to barbarians and non-lethal combat limit units
	FOR_EACH_NON_CENTER_PLOT_IN_RANGE_OF(plot(), iSearchRange, {
		CvPlot & p = *pLoopPlot;
		if (!AI_plotValid(&p))
			continue;
		if (!bAllowCities && p.isCity())
			continue;

		// <advc.128>
		//if (it.currStepDist() > iSearchRangeRand && !p.isVisible(getTeam(), false))
		if (::stepDistance(plot(), &p) > iSearchRangeRand && !p.isVisible(getTeam(), false))
		{
			continue;
		} // </advc.128>
		if (bDeclareWar ? (!kOurTeam.AI_mayAttack(p) &&
			(!p.isCity() || !AI_mayAttack(p.getPlotCity()->getTeam(), p))) :
			(!p.isVisibleEnemyUnit(this) && !isEnemyCity(p)))
		{
			continue;
		}

		int iEnemyDefenders = (bDeclareWar ?
			AI_countEnemyDefenders(p) :
			p.getNumVisibleEnemyDefenders(this));
		// <advc.033>
		if (isAlwaysHostile(&p))
		{
			IntPair iiDefendersAll = AI_countPiracyTargets(p);
			if (iiDefendersAll.second <= 0)
				continue;
			iEnemyDefenders = iiDefendersAll.first;
		} // </advc.033>
		if (iEnemyDefenders < iMinStack)
			continue;

		if (bFollow ?
			getGroup()->canMoveOrAttackInto(p, bDeclareWar, true) :
			generatePath(&p, eFlags, true, 0, iRange))
		{
			// 101 for cities, because that's a better thing to capture.
			int iOdds = (iEnemyDefenders == 0 ? (p.isCity() ? 101 : 100) :
				AI_getGroup()->AI_getWeightedOdds(&p, false));
			if (iOdds > iBestOdds)
			{
				iBestOdds = iOdds;
				pBestPlot = (bFollow ? &p : getPathEndTurnPlot());
			}
		}
	}) // FOR_EACH_PLOT_IN_RANGE_OF
	
	if (pBestPlot == NULL)
		return false;

	FAssert(!at(*pBestPlot));
	// K-Mod
	if (AI_considerPathDOW(pBestPlot, eFlags))
	{	// <advc.163>
		if (!canMove())
			return true; // </advc.163>
		/*	after DOW, we might not be able to get to our target this turn...
			but try anyway. */
		if (!generatePath(pBestPlot, eFlags))
			return false;
		if (bFollow && pBestPlot != getPathEndTurnPlot())
			return false;
		pBestPlot = getPathEndTurnPlot();
	}
	if (bFollow && AI_isAnyEnemyDefender(*pBestPlot))
	{
		/*	we need to ungroup to capture the undefended unit / city.
			(because not everyone in our group can move) */
		joinGroup(NULL);
		bFollow = false;
	}
	// K-Mod end

	const int iAttackFlags = MOVE_DIRECT_ATTACK | MOVE_SINGLE_ATTACK | (int)eFlags;
	pushGroupMoveTo(*pBestPlot, (bFollow ?
		static_cast<MovementFlags>(iAttackFlags) : NO_MOVEMENT_FLAGS));
	/*
	pushGroupMoveTo(*pBestPlot, eFlags | (bFollow ?
		MOVE_DIRECT_ATTACK | MOVE_SINGLE_ATTACK : NO_MOVEMENT_FLAGS));
	*/
	return true;
}

//Attack only if the odds fall between two thresholds.
//This is to prevent "baiting" and is intended to be used with multiple passes.
bool CvUnitAI::AI_smartAttack(int iRange, int iLowOddsThreshold, int iHighOddsThreshold, CvPlot* pHintPlot)
{
    PROFILE_FUNC();

	CvPlot* pLoopPlot;
	CvPlot* pBestPlot;
	int iSearchRange;
	int iPathTurns;
	int iValue;
	int iBestValue;
	int iDX, iDY;

	FAssert(canMove());

	iSearchRange = isCargo() ? 1 : AI_searchRange(iRange);

	iBestValue = 0;
	pBestPlot = NULL;

	for (iDX = -(iSearchRange); iDX <= iSearchRange; iDX++)
	{
		for (iDY = -(iSearchRange); iDY <= iSearchRange; iDY++)
		{
			pLoopPlot	= plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

			if (pLoopPlot != NULL)
			{
				if (canMoveOrAttackInto(pLoopPlot, true))
				{
					if (pLoopPlot->isVisibleEnemyUnit(this) || (pLoopPlot->isCity() && AI_potentialEnemy(pLoopPlot->getTeam(), pLoopPlot) && (GET_TEAM(getTeam()).AI_getWarPlan(pLoopPlot->getTeam()) != WARPLAN_EXTORTION)))
					{
						if (!atPlot(pLoopPlot) && (generatePath(pLoopPlot, 0, true, &iPathTurns) && (iPathTurns <= iRange)))
						{
							{
								iValue = getGroup()->AI_attackOdds(pLoopPlot, true);

								if (iValue >= iLowOddsThreshold)
								{
								    if (pLoopPlot->isCity() ||  (iValue <= iHighOddsThreshold))
                                    {
                                        if (pHintPlot == pLoopPlot)
                                        {
                                            iValue *= 3;
                                            iValue /= 2;
                                        }
                                        if (iValue > iBestValue)
                                        {
                                            iBestValue = iValue;
                                            pBestPlot =  getPathEndTurnPlot();
                                            FAssert(!atPlot(pBestPlot));
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

	if (pBestPlot != NULL)
	{
		FAssert(!atPlot(pBestPlot));
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), 0);
		return true;
	}
    return false;
}
// TAC - AI Attack City - koma13 - END

// (heavily edited for K-Mod)
bool CvUnitAI::AI_leaveAttack(int iRange, int iOddsThreshold, int iStrengthThreshold)
{
	FAssert(canMove());
	CvPlayerAI const& kOwner = GET_PLAYER(getOwner()); // K-Mod
	// <advc.300>
	if (isBarbarian() && iOddsThreshold > 1)
		iOddsThreshold /= 2; // </advc.300>

	CvPlot* pBestPlot = NULL;
	int iBestValue = 0;
	CvCity const* pCity = getPlot().getPlotCity();
	if (pCity != NULL && pCity->getOwner() == getOwner())
	{
		/*int iOurStrength = GET_PLAYER(getOwner()).AI_getOurPlotStrength(plot(), 0, false, false);
		int iEnemyStrength = GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), 2, false, false);*/
		// K-Mod
		int iOurDefence = kOwner.AI_localDefenceStrength(plot(), getTeam());
		int iEnemyStrength = kOwner.AI_localAttackStrength(plot(), NO_TEAM, DOMAIN_LAND, 2);
		// K-Mod end
		if (iEnemyStrength > 0)
		{
			if (iOurDefence * 100 / iEnemyStrength < iStrengthThreshold)
			{
				/*	K-Mod.
					We should only heed to the threshold if we
					either have enough defence to hold the city,
					or we don't have enough attack force to wipe the enemy out.
					(otherwise, we are better off attacking than defending.) */
				if (iEnemyStrength < iOurDefence ||
					kOwner.AI_localAttackStrength(
						plot(), getTeam(), DOMAIN_LAND, 0, false, false, true)
					< kOwner.AI_localDefenceStrength(
						plot(), NO_TEAM, DOMAIN_LAND, 2, false)) // K-Mod end
				{
					return false;
				}
			}
			if (getPlot().plotCount(PUF_canDefendGroupHead, -1, -1, getOwner(),
				NO_TEAM, PUF_isDomainType, DOMAIN_LAND) // advc.001s
				<= getGroup()->getNumUnits())
			{
				return false;
			}
		}
	}
	//for (SquareIter it(*this, iRange, false); it.hasNext(); ++it)
	FOR_EACH_PLOT_IN_RANGE_OF(plot(), iRange,
	{
		//CvPlot const& p = *it;
		CvPlot const& p = *pLoopPlot;

		if (/*!AI_plotValid(p)*/!isArea(*p.area())) // advc.opt
			continue;

		/*if (p.isVisibleEnemyUnit(this) || (p.isCity() && AI_potentialEnemy(p.getTeam(), &p))) {
			if (p.getNumVisibleEnemyDefenders(this) > 0)*/ // BtS
		if (!p.isVisibleEnemyDefender(this)) // K-Mod
			continue;
		if (!generatePath(&p, NO_MOVEMENT_FLAGS, true, 0, iRange))
			continue;
		/*	<advc.114f> Enter hostile territory only if we can attack straight away
			or if we'll have moves left for seeking safety */
		if (getPathFinder().getPathTurns() > 1 &&
			getPathFinder().getFinalMoves() <= 0)
		{
			if (p.isOwned() && GET_TEAM(p.getTeam()).isAtWar(getTeam()))
				continue;
			/*	Don't make multi-turn moves in non-hostile territory either
				if this is a non-lethal group. Can still damage them if and when
				they come closer. */
				/*
				bool bAnyLethal = false;
				FOR_EACH_UNIT_IN(pGroupUnit, *getGroup())
				{
					if (pGroupUnit->combatLimit() >= 100)
					{
						bAnyLethal = true;
						break;
					}
				}
				if (!bAnyLethal)
					continue;
				*/
		} // </advc.114f>
		//iValue = getGroup()->AI_attackOdds(&p, true);
		int iValue = AI_getGroup()->AI_getWeightedOdds(&p, false); // K-Mod
		if (iValue >= iOddsThreshold && // K-Mod
			iValue > iBestValue)
		{
			iBestValue = iValue;
			pBestPlot = getPathEndTurnPlot();
		}
	}) // FOR_EACH

	if (pBestPlot != NULL)
	{
		// K-Mod note: no AI_considerDOW here.
		pushGroupMoveTo(*pBestPlot, NO_MOVEMENT_FLAGS, false, false,
			MISSIONAI_COUNTER_ATTACK);
		return true;
	}

	return false;
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_seaBombardRange(int iMaxRange)
{
	PROFILE_FUNC();

	// cached values
	CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());
	CvPlot* pPlot = plot();
	CvSelectionGroup* pGroup = getGroup();

	// can any unit in this group bombard?
	bool bHasBombardUnit = false;
	bool bBombardUnitCanBombardNow = false;
	CLLNode<IDInfo>* pUnitNode = pGroup->headUnitNode();
	while (pUnitNode != NULL && !bBombardUnitCanBombardNow)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pGroup->nextUnitNode(pUnitNode);

		if (pLoopUnit != NULL && pLoopUnit->bombardRate() > 0)
		{
			bHasBombardUnit = true;

			if (pLoopUnit->canMove() && !pLoopUnit->isMadeAttack())
			{
				bBombardUnitCanBombardNow = true;
			}
		}
	}

	if (!bHasBombardUnit)
	{
		return false;
	}

	// best match
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestBombardPlot = NULL;
	int iBestValue = 0;

	// iterate over plots at each range
	for (int iDX = -(iMaxRange); iDX <= iMaxRange; iDX++)
	{
		for (int iDY = -(iMaxRange); iDY <= iMaxRange; iDY++)
		{
			CvPlot* pLoopPlot = plotXY(pPlot->getX_INLINE(), pPlot->getY_INLINE(), iDX, iDY);

			if (pLoopPlot != NULL && AI_plotValid(pLoopPlot))
			{
				CvCity* pBombardCity = bombardTarget(pLoopPlot);

				if (pBombardCity != NULL && isEnemy(pBombardCity->getTeam(), pLoopPlot) && pBombardCity->getDefenseDamage() < GC.getMAX_CITY_DEFENSE_DAMAGE())
				{
					int iPathTurns;
					if (generatePath(pLoopPlot, 0, true, &iPathTurns))
					{
						int iValue = 1;

						iValue += (kPlayer.AI_plotTargetMissionAIs(pBombardCity->plot(), MISSIONAI_ASSAULT, NULL, 2) * 3);
						iValue += (kPlayer.AI_adjacentPotentialAttackers(pBombardCity->plot(), true));

						if (iValue > 0)
						{
							iValue *= 1000;

							iValue /= getPathCost() + 50;

							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestPlot = getPathEndTurnPlot();
								pBestBombardPlot = pLoopPlot;
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestBombardPlot != NULL))
	{
		if (atPlot(pBestBombardPlot))
		{
			// if we are at the plot from which to bombard, and we have a unit that can bombard this turn, do it
			if (bBombardUnitCanBombardNow && pGroup->canBombard(pBestBombardPlot))
			{
				getGroup()->pushMission(MISSION_BOMBARD, -1, -1, 0, false, false, MISSIONAI_BOMBARD, pBestBombardPlot);
			}
			// otherwise, skip until next turn, when we will surely bombard
			else
			{
				getGroup()->pushMission(MISSION_SKIP);
			}

			return true;
		}
		else
		{
			FAssert(!atPlot(pBestPlot));
			getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), 0, false, false, MISSIONAI_BOMBARD, pBestBombardPlot);
			return true;
		}
	}

	return false;
}

bool CvUnitAI::AI_blockade(int iRange)
{
	PROFILE_FUNC();

	int iMaxRange = iRange;

	// best match
	CvPlot* pBestPlot = NULL;
	int iBestValue = 0;

	// iterate over plots at each range
	for (int iDX = -(iMaxRange); iDX <= iMaxRange; iDX++)
	{
		for (int iDY = -(iMaxRange); iDY <= iMaxRange; iDY++)
		{
			CvPlot* pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

			if (pLoopPlot != NULL && AI_plotValid(pLoopPlot))
			{
				CvCity* pBlockadeCity = pLoopPlot->getWorkingCity();
				if (pBlockadeCity != NULL && isEnemy(pBlockadeCity->getTeam(), pLoopPlot) && pBlockadeCity->getDefenseDamage() < GC.getMAX_CITY_DEFENSE_DAMAGE())
				{
					int iPathTurns;
					if (atPlot(pLoopPlot) || generatePath(pLoopPlot, 0, true, &iPathTurns))
					{
						int iValue = atPlot(pLoopPlot) ? 1200 : 1000;
						if (pLoopPlot->getBonusType() != NO_BONUS)
						{
							iValue += 1000;
						}
						iValue /= 1 + pLoopPlot->plotCount(PUF_canDefend, -1, -1, getOwnerINLINE());
						iValue += GC.getGameINLINE().getSorenRandNum(100, "AI blockade plot");

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							pBestPlot = getPathEndTurnPlot();
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		if (atPlot(pBestPlot))
		{
			if (canBombard(pBestPlot))
			{
				getGroup()->pushMission(MISSION_BOMBARD);
			}
			else
			{
				getGroup()->pushMission(MISSION_SKIP);
			}
			return true;
		}
		else
		{
			getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE());
			return true;
		}
	}

	return false;
}



// Returns true if a mission was pushed...
bool CvUnitAI::AI_pillage()
{
	PROFILE_FUNC();

	if (canPillage(plot()) && AI_canPillage(*plot()))
	{
		pillage();
		return true;
	}

	CvPlot* pBestPillagePlot = NULL;
	int iPathTurns;
	int iValue;
	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;

	for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);

		if (AI_plotValid(pLoopPlot))
		{
			if (potentialWarAction(pLoopPlot))
			{
			    CvCity * pWorkingCity = pLoopPlot->getWorkingCity();

			    if (pWorkingCity != NULL)
			    {
                    if (!(pWorkingCity == area()->AI_getTargetCity(getOwnerINLINE())) && canPillage(pLoopPlot))
                    {
                        if (!(pLoopPlot->isVisibleEnemyUnit(this)))
                        {
                            if (GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_PILLAGE, getGroup(), 1) == 0)
                            {
                                if (generatePath(pLoopPlot, 0, true, &iPathTurns))
                                {
                                    iValue = AI_pillageValue(pLoopPlot);

                                    iValue *= 1000;

                                    iValue /= (iPathTurns + 1);

									// if not at war with this plot owner, then devalue plot if we already inside this owner's borders
									// (because declaring war will pop us some unknown distance away)
									if (!isEnemy(pLoopPlot->getTeam()) && plot()->getTeam() == pLoopPlot->getTeam())
									{
										iValue /= 10;
									}

                                    if (iValue > iBestValue)
                                    {
                                        iBestValue = iValue;
                                        pBestPlot = getPathEndTurnPlot();
                                        pBestPillagePlot = pLoopPlot;
                                    }
                                }
                            }
                        }
                    }
			    }
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestPillagePlot != NULL))
	{
		if (atPlot(pBestPillagePlot) && !isEnemy(pBestPillagePlot->getTeam()))
		{
			//getGroup()->groupDeclareWar(pBestPillagePlot, true);
			// rather than declare war, just find something else to do, since we may already be deep in enemy territory
			return false;
		}

		if (atPlot(pBestPillagePlot))
		{
			if (isEnemy(pBestPillagePlot->getTeam()))
			{
				getGroup()->pushMission(MISSION_PILLAGE, -1, -1, 0, false, false, MISSIONAI_PILLAGE, pBestPillagePlot);
				return true;
			}
		}
		else
		{
			FAssert(!atPlot(pBestPlot));
			getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), 0, false, false, MISSIONAI_PILLAGE, pBestPillagePlot);
			return true;
		}
	}

	return false;
}

bool CvUnitAI::AI_canPillage(CvPlot& kPlot) const
{
	if (isEnemy(kPlot.getTeam(), &kPlot))
	{
		return true;
	}

	if (!kPlot.isOwned())
	{
		return true;
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_pillageRange(int iRange, int iBonusValueThreshold, MovementFlags eFlags)
{
	PROFILE_FUNC();

	CvPlot* pBestPlot = NULL;
	CvPlot* pBestPillagePlot = NULL;
	int iBestValue = 0;
	//for (SquareIter it(*this, AI_searchRange(iRange)); it.hasNext(); ++it)
	FOR_EACH_PLOT_IN_RANGE_OF(plot(), AI_searchRange(iRange),
	{
		CvPlot& p = *pLoopPlot;
		if (!AI_plotValid(&p) || p.isBarbarian())
		{
			continue; // advc (and some other shortcuts below)
		}
		// <advc.033>
		if (isAlwaysHostile() && p.isOwned()/* &&
			!GET_PLAYER(getOwner()).AI_isPiracyTarget(p.getOwner())*/)
		{
			continue;
		} // </advc.033>
		CvCity* pWorkingCity = p.getWorkingCity();
		if (pWorkingCity == NULL || !AI_mayAttack(p)) // advc.opt: Attack check moved down
			continue;
		if ((pWorkingCity != area()->AI_getTargetCity(getOwner()) ||
			/*  advc.001: Barbarians should not exclude any city from pillaging.
				(Bugfix obsolete b/c Barbarians no longer have a target city.) */
			isBarbarian()) &&
			canPillage(&p))
		{
			int iPathTurns;
			if (p.isVisibleEnemyUnit(this) ||
				GET_PLAYER(getOwner()).AI_isAnyPlotTargetMissionAI(
					p, MISSIONAI_PILLAGE, getGroup()) ||
				!generatePath(&p, eFlags, true, &iPathTurns, iRange))
			{
				continue;
			}
			if (getPathFinder().getFinalMoves() == 0)
				iPathTurns++;

			if (iPathTurns <= iRange)
			{
				int iValue = AI_pillageValue(&p);
				iValue *= 1000;
				iValue /= (iPathTurns + 1);

				// if not at war with this plot owner, then devalue plot if we already inside this owner's borders
				// (because declaring war will pop us some unknown distance away)
				if (!isEnemy(p) && getPlot().getTeam() == p.getTeam())
					iValue /= 10;
				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = getPathEndTurnPlot();
					pBestPillagePlot = &p;
				}
			}
		}
	}) // FOR_EACH

	if (pBestPlot != NULL && pBestPillagePlot != NULL)
	{
		if (atPlot(pBestPillagePlot) && !isEnemy(*pBestPillagePlot))
		{
			//getGroup()->groupDeclareWar(pBestPillagePlot, true);
			// rather than declare war, just find something else to do, since we may already be deep in enemy territory
			return false;
		}

		if (at(*pBestPillagePlot))
		{
			if (isEnemy(*pBestPillagePlot))
			{
				getGroup()->pushMission(MISSION_PILLAGE, -1, -1, NO_MOVEMENT_FLAGS,
					false, false, MISSIONAI_PILLAGE, pBestPillagePlot);
				return true;
			}
		}
		else
		{
			pushGroupMoveTo(*pBestPlot, eFlags, false, false,
				MISSIONAI_PILLAGE, pBestPillagePlot);
			return true;
		}
	}

	return false;
}

bool CvUnitAI::AI_maraud(int iRange)
{
	PROFILE_FUNC();
	//The basic goal of the maraud function, is to penetrate deeper into hostile
	//territory, but staying away from cities.

	int iSearchRange = iRange;//AI_searchRange(iRange);

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestMaraudPlot = NULL;

	CvCity* pNearestCity = GC.getMap().findCity(getX_INLINE(), getY_INLINE(), getOwnerINLINE(), NO_TEAM, true);

	if (pNearestCity == NULL)
	{
		return false;
	}

	int iCurrentDist = stepDistance(getX_INLINE(), getY_INLINE(), pNearestCity->getX_INLINE(), pNearestCity->getY_INLINE());

	for (int iDX = -(iSearchRange); iDX <= iSearchRange; iDX++)
	{
		for (int iDY = -(iSearchRange); iDY <= iSearchRange; iDY++)
		{
			CvPlot* pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

			if ((pLoopPlot != NULL) && !atPlot(pLoopPlot))
			{
				if (AI_potentialEnemy(pLoopPlot->getTeam(), pLoopPlot))
				{
					int iPathTurns;
					if (generatePath(pLoopPlot, 0, true, &iPathTurns))
					{
						int iNewDist = stepDistance(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), pNearestCity->getX_INLINE(), pNearestCity->getY_INLINE());

						int iValue = 200 + GC.getGame().getSorenRandNum((iNewDist - iCurrentDist) * 100, "AI maraud");

						if (canPillage(pLoopPlot))
						{
							iValue += 100;
						}

						for (int iDirection = 0; iDirection < NUM_DIRECTION_TYPES; ++iDirection)
						{
							CvPlot* pDirectionPlot = plotDirection(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), (DirectionTypes)iDirection);
							if (pDirectionPlot != NULL)
							{
								if (pDirectionPlot->isOwned() && AI_potentialEnemy(pDirectionPlot->getTeam(), pDirectionPlot))
								{
									if (pDirectionPlot->isCity())
									{
										iValue -= 200;
									}
									else if (pDirectionPlot->getRouteType() != NO_ROUTE)
									{
										iValue -= 25;
									}
								}
							}
						}

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							pBestMaraudPlot = pLoopPlot;
							pBestPlot = getPathEndTurnPlot();
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), 0, false, false, MISSIONAI_PILLAGE, pBestMaraudPlot);
		return true;
	}

	return false;
}

bool CvUnitAI::AI_hostileShuffle()
{
	int iBestAdjacentPlotValue = 0;
	CvPlot* pBestAdjacentPlot = NULL;
	for (int iDirection = 0; iDirection < NUM_DIRECTION_TYPES; ++iDirection)
	{
		CvPlot* pDirectionPlot = plotDirection(getX_INLINE(), getY_INLINE(), (DirectionTypes)iDirection);
		if (pDirectionPlot != NULL)
		{
			if (isEnemy(pDirectionPlot->getTeam(), pDirectionPlot))
			{
				if (pDirectionPlot->isCity())
				{
					int iValue =  GET_PLAYER(getOwnerINLINE()).AI_targetCityValue(pDirectionPlot->getPlotCity(), false, false);
					if (iValue > iBestAdjacentPlotValue)
					{
						pBestAdjacentPlot = pDirectionPlot;
						iBestAdjacentPlotValue = iValue;
					}
				}
			}
		}
	}
	if (pBestAdjacentPlot == NULL)
	{
		return false;
	}

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	for (int iX = -1; iX < 1; ++iX)
	{
		for (int iY = -1; iY <= 1; ++iY)
		{
			CvPlot* pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iX, iY);
			if (pLoopPlot != NULL)
			{
				if (stepDistance(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), pBestAdjacentPlot->getX_INLINE(), pBestAdjacentPlot->getY_INLINE()) <= 1)
				{
					if (atPlot(pLoopPlot) || canMoveInto(*pLoopPlot))
					{
						int iValue = pLoopPlot->defenseModifier(getTeam());
						if (!pLoopPlot->isRiverCrossing(directionXY(pLoopPlot, pBestAdjacentPlot)))
						{
							iValue += 50;
						}
						if (pLoopPlot->isBeingWorked())
						{
							iValue += 25;
						}
						iValue += GC.getGameINLINE().getSorenRandNum(75, "AI hostile shuffle");

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							pBestPlot = pLoopPlot;
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		if (atPlot(pBestPlot))
		{
			getGroup()->pushMission(MISSION_SKIP);
		}
		else
		{
			getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE());
		}
		return true;
	}
	return false;
}

bool CvUnitAI::AI_wanderAroundAimlessly()
{
	//I dedicate this function to all seekers of anything in life.

	CvPlot* pMissionPlot = getGroup()->AI_getMissionAIPlot();

	CvMap& kMap = GC.getMap();
	while ((pMissionPlot == NULL) || atPlot(pMissionPlot))
	{
		pMissionPlot = kMap.plot(GC.getGameINLINE().getSorenRandNum(kMap.getGridWidthINLINE(), "AI wander X"), GC.getGameINLINE().getSorenRandNum(kMap.getGridWidthINLINE(), "AI wander Y"));
	}

	int iCurrentDistance = plotDistance(getX_INLINE(), getY_INLINE(), pMissionPlot->getX_INLINE(), pMissionPlot->getY_INLINE());

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	if (pMissionPlot != NULL)
	{
		for (int i = 0; i < NUM_DIRECTION_TYPES; ++i)
		{
			CvPlot* pLoopPlot = plotDirection(getX_INLINE(), getY_INLINE(), (DirectionTypes)i);
			if (pLoopPlot != NULL)
			{
				int iDistance = plotDistance(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), pMissionPlot->getX_INLINE(), pMissionPlot->getY_INLINE());
				if (iDistance < iCurrentDistance)
				{
					if (AI_plotValid(pLoopPlot))
					{
						int iValue = GC.getGameINLINE().getSorenRandNum(100, "AI wander aimlessly");

						for (int j = 0; j < NUM_DIRECTION_TYPES; ++j)
						{
							CvPlot* pDirectionPlot = plotDirection(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), (DirectionTypes)i);
							if (pDirectionPlot != NULL)
							{
								if (!pDirectionPlot->isVisible(getTeam(), false))
								{
									iValue += 5;
								}
							}
						}

						if ((iValue > iBestValue) && generatePath(pLoopPlot))
						{
							iBestValue = iValue;
							pBestPlot = pLoopPlot;
						}
					}
				}
			}
		}
	}

	if (pBestPlot == NULL)
	{
		return false;
	}

	getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), 0, false, false, MISSIONAI_EXPLORE, pMissionPlot);
	return true;
}

int CvUnitAI::AI_foundValue(CvPlot* pPlot)
{
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());

	// Do not settle the first city on an island
	if (kOwner.getNumCities() == 0 && pPlot->area()->isIsland())
	{
		return 0;
	}

	int iValue = 0;
	if ((kOwner.getNumCities() == 0) && (GC.getGameINLINE().getGameTurn() < 20))
	{
		iValue = kOwner.AI_foundValue(pPlot->getX_INLINE(), pPlot->getY_INLINE());
	}
	else
	{
		iValue = pPlot->getFoundValue(getOwnerINLINE());
	}

	// TAC - AI City sites - koma13 - START
	if (iValue > 0)
	{
		ProfessionTypes eProfession = AI_getIdealProfession();
		if (eProfession != NO_PROFESSION)
		{
			CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);
			if (kProfession.isWorkPlot())
			{
				// R&R, ray , MYCP partially based on code of Aymerick - START
				for (int i = 0; i < kProfession.getNumYieldsProduced(); i++)
				{
					YieldTypes eYieldProduced = (YieldTypes)kProfession.getYieldsProduced(i);
					FAssert(eYieldProduced != NO_YIELD);
					if (eYieldProduced != NO_YIELD)
					{
						bool bHasBonus = false;
						int iBestAmount = 0;
						for (int iI = 0; iI < NUM_CITY_PLOTS; ++iI)
						{
							if (iI != CITY_HOME_PLOT)
							{
								CvPlot* pLoopPlot = plotCity(pPlot->getX_INLINE(), pPlot->getY_INLINE(), iI);
								if (pLoopPlot != NULL && (!pLoopPlot->isOwned() || GET_PLAYER(pLoopPlot->getOwnerINLINE()).isNative()))
								{
									iBestAmount = std::max(iBestAmount, pLoopPlot->calculateBestNatureYield(eYieldProduced, getTeam()));
									if (pLoopPlot->getBonusType() != NO_BONUS)
									{
										if (GC.getBonusInfo(pLoopPlot->getBonusType()).getYieldChange(eYieldProduced) > 0)
										{
											bHasBonus = true;
										}
									}
								}
							}
						}
						if (iBestAmount > 0)
						{
							iValue += 20 * kOwner.AI_yieldValue(eYieldProduced, true, iBestAmount + 1);
							if (bHasBonus)
							{
								iValue *= 4;
								iValue /= 3;
							}
						}
						else
						{
							iValue *= 2;
							iValue /= 3;
						}
					}
				}
			// R&R, ray , MYCP partially based on code of Aymerick - END
			}
		}
	}

	return iValue;

	// TAC - AI City sites - koma13 - END
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_found(int iMinValue, MovementFlags eFlags)
{
	PROFILE_FUNC();

	if (!canFound(NULL))
	{
		return false;
	}

	CvUnit* pTransportUnit = getTransportUnit();

	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestFoundPlot = NULL;
	bool bBestIsTransport = false;
	bool bTransportPath = false;

	for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);
		if (pLoopPlot->isRevealed(getTeam(), false) && ((pTransportUnit != NULL) || AI_plotValid(pLoopPlot)))
		{
			int iValue = AI_foundValue(pLoopPlot); // checks canFound internally
			if (iValue <= iMinValue)
				continue;
			if (pLoopPlot->isVisibleEnemyUnit(this))
				continue;		
			if (!(kOwner.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_FOUND, getGroup(), 0) == 0))
				continue;
			if (isCargo() && !pLoopPlot->isCoastalLand())
				continue;

			int iPathTurns = 0;
			bool bValid = atPlot(pLoopPlot);
			bool bTransport = false;

			if (!bValid)
			{
				bTransport = pTransportUnit != NULL;
				if (bTransport)
				{
					bValid = pTransportUnit->generatePath(pLoopPlot, eFlags, false, &iPathTurns);
				}
				else
				{
					// Due to the step <= 1 check, we know that we're adjacent to land. Check if we can move into the plot
					//bValid = canMoveInto(pLoopPlot); NO, let try for an amphibious landing first 
					bValid = generatePath(pLoopPlot, eFlags, false, &iPathTurns);
				}
			}

			if (bValid)
			{
				iValue *= 10;
				iValue /= 5 + iPathTurns;
				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = atPlot(pLoopPlot) ? pLoopPlot : (bTransport ? pTransportUnit->getPathEndTurnPlot() : getPathEndTurnPlot());
					pBestFoundPlot = pLoopPlot;
					bBestIsTransport = bTransport;
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestFoundPlot != NULL))
	{
		if (bBestIsTransport)
		{
			// Let the ship do an amphibious landing
			pTransportUnit->getGroup()->pushMission(MISSION_MOVE_TO, pBestFoundPlot->getX_INLINE(), pBestFoundPlot->getY_INLINE(), 
				0, false, false, MISSIONAI_FOUND, pBestFoundPlot);
			return true;
		}
		else
		{
			if (atPlot(pBestFoundPlot))
			{
				getGroup()->pushMission(MISSION_FOUND, -1, -1, 0, false, false, MISSIONAI_FOUND, pBestFoundPlot);
				return true;
			}
			else
			{
				FAssert(!atPlot(pBestPlot));
				getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), 0, false, false, MISSIONAI_FOUND, pBestFoundPlot);
				return true;
			}
		}
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_foundRange(int iRange, bool bFollow)
{
	PROFILE_FUNC();

	CvPlot* pLoopPlot;
	CvPlot* pBestPlot;
	CvPlot* pBestFoundPlot;
	int iSearchRange;
	int iPathTurns;
	int iBestValue;
	int iDX, iDY;

	iSearchRange = AI_searchRange(iRange);

	iBestValue = 0;
	pBestPlot = NULL;
	pBestFoundPlot = NULL;

	for (iDX = -(iSearchRange); iDX <= iSearchRange; iDX++)
	{
		for (iDY = -(iSearchRange); iDY <= iSearchRange; iDY++)
		{
			pLoopPlot	= plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

			if (pLoopPlot != NULL)
			{
				if (AI_plotValid(pLoopPlot) && (pLoopPlot != plot() || GET_PLAYER(getOwnerINLINE()).AI_getPlotDanger(pLoopPlot, 1) <= pLoopPlot->plotCount(PUF_canDefend, -1, -1, getOwnerINLINE())))
				{
					if (canFound(pLoopPlot))
					{
						int iValue = AI_foundValue(pLoopPlot);

						if (iValue > iBestValue)
						{
							if (!(pLoopPlot->isVisibleEnemyUnit(this)))
							{
								if (GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_FOUND, getGroup(), 3) == 0)
								{
									if (generatePath(pLoopPlot, MOVE_SAFE_TERRITORY, true, &iPathTurns))
									{
										if (iPathTurns <= iRange)
										{
											iBestValue = iValue;
											pBestPlot = getPathEndTurnPlot();
											pBestFoundPlot = pLoopPlot;
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

	if ((pBestPlot != NULL) && (pBestFoundPlot != NULL))
	{
		if (atPlot(pBestFoundPlot))
		{
			getGroup()->pushMission(MISSION_FOUND, -1, -1, 0, false, false, MISSIONAI_FOUND, pBestFoundPlot);
			return true;
		}
		else if (!bFollow)
		{
			FAssert(!atPlot(pBestPlot));
			getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_SAFE_TERRITORY, false, false, MISSIONAI_FOUND, pBestFoundPlot);
			return true;
		}
	}

	return false;
}
bool CvUnitAI::AI_joinCityBrave()
{
	CvCity* pCity = plot()->getPlotCity();
	if (pCity == NULL || pCity->getOwnerINLINE() != getOwnerINLINE())
	{
		return false;
	}

	int iProduction = pCity->getRawYieldProduced(YIELD_FOOD);
	int iConsumption = pCity->getRawYieldConsumed(YIELD_FOOD);

	if (((iProduction / 2) + 1) >= (iConsumption + GLOBAL_DEFINE_FOOD_CONSUMPTION_PER_POPULATION))
	{
		if (canJoinCity(plot()))
		{
			joinCity();
			pCity->AI_setTargetSize(pCity->getPopulation());
			AI_setMovePriority(0);
			return true;
		}
	}
	return false;
}

// Determine if we should join any city in this area where we can be employed
bool CvUnitAI::AI_joinCity(int iMaxPath, bool bRequireJoinable)
{
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());

	CvUnit* pTransportUnit = getTransportUnit();

	bool bForceTransport = false;
	if (plot()->isCity() && (plot()->getOwnerINLINE() != getOwnerINLINE()))
	{
		bForceTransport = true;
	}

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestJoinPlot = NULL;
	ProfessionTypes eBestProfession = NO_PROFESSION;

	const ProfessionTypes eOptimalProfession = kOwner.AI_idealProfessionForUnit(getUnitType());

	int iLoop;
	bool bTransportPath = false;
	for (CvCity* pCity = kOwner.firstCity(&iLoop); pCity != NULL; pCity = kOwner.nextCity(&iLoop))
	{
		bool bTransport = pTransportUnit != NULL;
		if (bTransport && !pCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
			continue;

		CvPlot* pLoopPlot = pCity->plot();

		if (!bRequireJoinable || canJoinCity(pLoopPlot, true))
		{
			int iPathTurns = 0;

			if (!bForceTransport && canUnload() && (pLoopPlot->area() == area()))
			{
				bTransport = false;
			}

			bool bValid = atPlot(pLoopPlot);

			if (!bValid)
			{
				if (bTransport)
				{
					// TAC - AI Improved Naval AI - koma13 - START
					//bValid = pTransportUnit->generatePath(pLoopPlot, 0, bTransportPath, &iPathTurns);
					bValid = pTransportUnit->generatePath(pLoopPlot, 0, bTransportPath, &iPathTurns, iMaxPath);
					// TAC - AI Improved Naval AI - koma13 - END
				}
				else
				{
					bValid = generatePath(pLoopPlot, MOVE_NO_ENEMY_TERRITORY, !bTransportPath, &iPathTurns, iMaxPath);
				}
				bTransportPath = bTransport;
			}

			if (bValid)
			{
				ProfessionTypes eProfession;
				int iValue = pCity->AI_unitJoinCityValue(this, &eProfession);

				const int iIncoming = kOwner.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_FOUND, getGroup());
				// TAC - AI Economy- koma13 - START
				//int iSizeGap = pCity->AI_getTargetSize() - (pCity->getPopulation() + iIncoming, 0);
				const int iSizeGap = pCity->AI_getTargetSize() - (pCity->getPopulation() + iIncoming);
				// TAC - AI Economy- koma13 - END
				if (iSizeGap > 0)
				{
					int iModifier = iSizeGap * (eOptimalProfession == NO_PROFESSION) ? 20 : 5;
					iModifier += (pCity->getPopulation() + iIncoming == 1) ? 100 : 25;
					iValue *= 100 + iModifier;
					iValue /= 100;
				}
				// TAC - AI Economy- koma13 - START
				else if (iSizeGap < 0)
				{
					// Even though this city is undesireable it's still better than having idle colonists
					iValue /= 2;
				}
				// TAC - AI Economy- koma13 - END

				iValue *= 100;
				iValue /= 100 + 10 * iPathTurns;

				if (atPlot(pLoopPlot))
				{
					iValue *= 100 + std::max(0, (15 - pCity->getPopulation()) * 2) + ((eOptimalProfession == NO_PROFESSION) ? 25 : 10);
					iValue /= 100;
				}

				if (iValue > iBestValue || !bRequireJoinable)
				{
					iBestValue = iValue;
					pBestPlot = atPlot(pLoopPlot) ? pLoopPlot : pLoopPlot;
					pBestJoinPlot = pLoopPlot;
					eBestProfession = eProfession;
				}
			}
		}
	}


	if (pBestJoinPlot == NULL)
	{
		CvCity* pPlotCity = plot()->getPlotCity();
		if (pPlotCity != NULL && (pPlotCity->getOwnerINLINE() == getOwnerINLINE()))
		{
			if (canUnload())
			{
				unload();
				return true;
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestJoinPlot != NULL))
	{
		if (atPlot(pBestJoinPlot))
		{
			if (!canJoinCity(plot(), true))
			{
				if (getTransportUnit() != NULL)
				{
					unload();
				}
				getGroup()->pushMission(MISSION_SKIP);
				return true;
			}

			AI_setMovePriority(0);
			plot()->getPlotCity()->addPopulationUnit(this, eBestProfession);
			return true;
		}
		else if ((pTransportUnit != NULL) && !(!bForceTransport && canUnload() && (pBestJoinPlot->area() == area())))
		{
			getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_FOUND, pBestJoinPlot);
			return true;
		}
		else
		{
			FAssert(!atPlot(pBestPlot));
			getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_NO_ENEMY_TERRITORY, false, false, MISSIONAI_FOUND, pBestJoinPlot);
			return true;
		}
	}
	return false;
}


bool CvUnitAI::AI_joinOptimalCity()
{
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());

	CvUnit* pTransportUnit = getTransportUnit();

	bool bForceTransport = false;
	if (plot()->isCity() && (plot()->getOwnerINLINE() != getOwnerINLINE()))
	{
		bForceTransport = true;
	}

	ProfessionTypes eOptimalProfession = kOwner.AI_idealProfessionForUnit(getUnitType());

	if (eOptimalProfession == NO_PROFESSION)
	{
		return false;
	}


	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestJoinPlot = NULL;


	int iLoop;
	bool bTransportPath = false;
	for (CvCity* pCity = kOwner.firstCity(&iLoop); pCity != NULL; pCity = kOwner.nextCity(&iLoop))
	{
		bool bTransport = pTransportUnit != NULL;
		if (bTransport && !pCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
			continue;

		CvPlot* pLoopPlot = pCity->plot();

		if (canJoinCity(pLoopPlot, true))
		{
			int iPathTurns = 0;
			
			if (!bForceTransport && canUnload() && (pLoopPlot->area() == area()))
			{
				bTransport = false;
			}

			bool bValid = atPlot(pLoopPlot);

			if (!bValid)
			{
				if (bTransport)
				{
					// TAC - AI Improved Naval AI - koma13 - START
					//bValid = pTransportUnit->generatePath(pLoopPlot, 0, bTransportPath, &iPathTurns);
					bValid = pTransportUnit->generatePath(pLoopPlot, 0, bTransportPath, &iPathTurns);
					// TAC - AI Improved Naval AI - koma13 - END
				}
				else
			{
					bValid = generatePath(pLoopPlot, MOVE_NO_ENEMY_TERRITORY, !bTransportPath, &iPathTurns);
				}
				bTransportPath = bTransport;
			}

			if (bValid)
			{
					int iValue = kOwner.AI_professionBasicValue(eOptimalProfession, getUnitType(), pCity);

					if (iValue > 0)
					{
						if (AI_getIdealProfession() != NO_PROFESSION)
						{
							bool bUnique = true;
							// R&R, ray , MYCP partially based on code of Aymerick - START
							for (int i = 0; i < GC.getProfessionInfo(AI_getIdealProfession()).getNumYieldsProduced(); i++)
							{
								YieldTypes eYieldProduced = (YieldTypes)GC.getProfessionInfo(AI_getIdealProfession()).getYieldsProduced(i);
								if (eYieldProduced == YIELD_HAMMERS || eYieldProduced == YIELD_BELLS)
								{
									for (int j = 0; j < pCity->getPopulation(); ++j)
									{
										CvUnit* pLoopUnit = pCity->getPopulationUnitByIndex(j);
										if (pLoopUnit->AI_getIdealProfession() == AI_getIdealProfession())
										{
											bUnique = false;
											break;
										}
									}
									if (bUnique)
									{
										if (pCity->AI_getYieldAdvantage(YIELD_EDUCATION) >= 100)
										{
											iValue *= 2;
										}
									}
								}
							}
							// R&R, ray , MYCP partially based on code of Aymerick - END
						}

						iValue += 50;
						iValue -= iPathTurns;

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							pBestPlot = pLoopPlot;
							pBestJoinPlot = pLoopPlot;
						}
					}
				}
			}
		}


	if (pBestJoinPlot == NULL)
	{
		CvCity* pPlotCity = plot()->getPlotCity();
		if (pPlotCity != NULL && (pPlotCity->getOwnerINLINE() == getOwnerINLINE()))
		{
			if (canUnload())
			{
				unload();
				return true;
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestJoinPlot != NULL))
	{
		if (atPlot(pBestJoinPlot))
		{
			if (canJoinCity(plot()))
			{
				AI_setMovePriority(0);
				joinCity();
				return true;
			}
			else
			{
				getGroup()->pushMission(MISSION_SKIP);
				return true;
			}

			if (AI_betterJob())
			{
				return true;
			}
			return true;
		}
		else if ((pTransportUnit != NULL) && !(!bForceTransport && canUnload() && (pBestJoinPlot->area() == area())))
		{
			getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_FOUND, pBestJoinPlot);
			return true;
		}
		else
		{
			FAssert(!atPlot(pBestPlot));
			getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_NO_ENEMY_TERRITORY, false, false, MISSIONAI_FOUND, pBestJoinPlot);
			return true;
		}
	}
	return false;
}

bool CvUnitAI::AI_joinCityDefender()
{
	CvCity* pCity = plot()->getPlotCity();
	if (pCity == NULL || pCity->getOwnerINLINE() != getOwnerINLINE())
	{
		return false;
	}
	if (getProfession() == NO_PROFESSION)
	{
		return false;
	}
	if (!AI_allowedToJoin(pCity))
	{
		return false;
	}

	CvProfessionInfo& kProfession = GC.getProfessionInfo(getProfession());

	int iCapacity = pCity->getMaxYieldCapacity();
	if (GET_PLAYER(getOwnerINLINE()).hasContentsYieldEquipmentAmount(getProfession())) // cache CvPlayer::getYieldEquipmentAmount - Nightinggale
	{
		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			int iAmount = GET_PLAYER(getOwnerINLINE()).getYieldEquipmentAmount(getProfession(), (YieldTypes)iYield);
			if (iAmount > 0)
			{
				if ((pCity->getTotalYieldStored() + (iAmount * 2)) > iCapacity)
				{
					return false;
				}
			}
		}
	}

	const bool bJoined = joinCity();
	if (bJoined)
		AI_setMovePriority(0);
	return bJoined;
}

bool CvUnitAI::AI_yieldDestination(int iMaxPath)
{
	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());

	CvCity* pPlotCity = plot()->getPlotCity();
	if (pPlotCity != NULL)
	{
		AI_setMovePriority(0);
		unload();
		return true;
	}

	CvUnit* pTransportUnit = getTransportUnit();
	FAssert(pTransportUnit != NULL);
	if (pTransportUnit == NULL)
	{
		return false;
	}

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;

	int iLoop;
	CvCity* pCity;
	for (pCity = kOwner.firstCity(&iLoop); pCity != NULL; pCity = kOwner.nextCity(&iLoop))
	{
		CvPlot* pLoopPlot = pCity->plot();
		if (!atPlot(pLoopPlot))
		{
			int iPathTurns;
			if (pTransportUnit->generatePath(pLoopPlot, 0, true, &iPathTurns, iMaxPath))
			{
				int iValue = 10 + 3 * std::max(0, pCity->getMaintainLevel(getYield()) - pCity->getYieldStored(getYield()));
				iValue += pCity->getMaxYieldCapacity() - pCity->getTotalYieldStored();
				iValue = iValue / (iPathTurns + 3);

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = pLoopPlot;
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_FOUND, pBestPlot);
		return true;
	}
	return false;
}

bool CvUnitAI::AI_yieldNativeDestination(int iMaxPath)
{
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	CvUnit* pTransportUnit = getTransportUnit();
	FAssert(pTransportUnit != NULL);
	if (pTransportUnit == NULL)
	{
		return false;
	}

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;

	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		CvPlayerAI& kLoopPlayer = GET_PLAYER((PlayerTypes)i);
		if (kLoopPlayer.isAlive() && kLoopPlayer.isNative() && !atWar(getTeam(), kLoopPlayer.getTeam()))
		{
			if (kLoopPlayer.AI_maxGoldTrade(getOwnerINLINE()) > 0)
			{
				if (kOwner.AI_getAttitude((PlayerTypes)i) >= ATTITUDE_CAUTIOUS)
				{
					int iLoop;
					CvCity* pCity;
					for (pCity = kLoopPlayer.firstCity(&iLoop); pCity != NULL; pCity = kLoopPlayer.nextCity(&iLoop))
					{
						CvPlot* pLoopPlot = pCity->plot();

						if (!atPlot(pLoopPlot) && pLoopPlot->isRevealed(getTeam(), false))
						{

							int iPathTurns = 0;
							if (pTransportUnit->generatePath(pLoopPlot, 0, true, &iPathTurns, iMaxPath))
							{
								int iSellPrice = kLoopPlayer.AI_yieldTradeVal(getYield(), getTransportUnit()->getIDInfo(), getOwnerINLINE());
								iSellPrice = std::min(iSellPrice, kLoopPlayer.AI_maxGoldTrade(getOwnerINLINE()));
								int iBuyPrice = (GET_PLAYER(kOwner.getParent()).getYieldSellPrice(getYield()) - 1) * getYieldStored();

								if (iSellPrice >= iBuyPrice)
								{
									int iValue = 100 * iSellPrice / (iPathTurns + 2);
									{

										if (iValue > iBestValue)
										{
											iBestValue = iValue;
											pBestPlot = pLoopPlot;
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

	if (pBestPlot != NULL)
	{
		getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_TRANSPORT_SEA, pBestPlot);
		return true;
	}
	return false;
}

bool CvUnitAI::AI_tradeWithCity()
{
	bool bSaleMade = false;
	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());

	CvCity* pPlotCity = plot()->getPlotCity();
	if (pPlotCity != NULL)
	{
		CvPlayerAI& kPlotPlayer = GET_PLAYER(pPlotCity->getOwner());
		if (kPlotPlayer.isNative())
		{
			if (canTradeYield(plot()))
			{
				CLLNode<IDInfo>* pUnitNode = plot()->headUnitNode();
				//Sell to city.
				std::vector<CvUnit*> apUnits;
				while (pUnitNode != NULL)
				{
					CvUnit* pLoopUnit = plot()->getUnitNodeLoop(pUnitNode);

					if (pLoopUnit != NULL && pLoopUnit->getTransportUnit() == this)
					{
						YieldTypes eYield = pLoopUnit->getYield();
						if (pLoopUnit->isGoods())
						{
							if (pLoopUnit->AI_getUnitAIState() == UNITAI_STATE_SELL_TO_NATIVES)
							{
								apUnits.push_back(pLoopUnit);
							}
						}
					}
				}

				for (uint i = 0; i < apUnits.size(); ++i)
				{
					int iGold = std::min(kPlotPlayer.AI_maxGoldTrade(getOwnerINLINE()), kPlotPlayer.AI_yieldTradeVal(apUnits[i]->getYield(), getIDInfo(), getOwnerINLINE()));

					CLinkList<TradeData> theirList;

					CLinkList<TradeData> ourList;

					TradeData item;
					setTradeItem(&item, TRADE_YIELD, apUnits[i]->getYield(), &getIDInfo());
					ourList.insertAtEnd(item);

					setTradeItem(&item, TRADE_GOLD, iGold, &getIDInfo());
					theirList.insertAtEnd(item);

					GC.getGameINLINE().implementDeal(getOwnerINLINE(), pPlotCity->getOwnerINLINE(), &ourList, &theirList);
					bSaleMade = true;
				}
			}
		}
	}
	return bSaleMade;
}

// TAC - AI Assault Sea - koma13, jdog5000(BBAI) - START
// Returns true if a mission was pushed...
bool CvUnitAI::AI_assaultSeaTransport(bool bNative)
{
	PROFILE_FUNC();

	bool bIsAttackCity = (getUnitAICargo(UNITAI_OFFENSIVE) > 0);

	FAssert(getGroup()->hasCargo());
	//FAssert(bIsAttackCity || getGroup()->getUnitAICargo(UNITAI_ATTACK) > 0);

	if (!canCargoAllMove())
	{
		return false;
	}

	std::vector<CvUnit*> aGroupCargo;
	CLLNode<IDInfo>* pUnitNode = plot()->headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = plot()->getUnitNodeLoop(pUnitNode);
		if (pLoopUnit != NULL)
		{
			CvUnit* pTransport = pLoopUnit->getTransportUnit();
			if (pTransport != NULL && pTransport->getGroup() == getGroup())
			{
				aGroupCargo.push_back(pLoopUnit);
			}
		}
	}

	int iCargo = getGroup()->getCargo();
	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestAssaultPlot = NULL;

	for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);

		if (pLoopPlot->isCoastalLand())
		{
			if (pLoopPlot->isOwned())
			{
				if (bNative || !GET_PLAYER(pLoopPlot->getOwnerINLINE()).isNative())
				{
					if (isPotentialEnemy(pLoopPlot->getTeam(), pLoopPlot))
					{
						int iTargetCities = pLoopPlot->area()->getCitiesPerPlayer(pLoopPlot->getOwnerINLINE());
						if (iTargetCities > 0)
						{
							bool bCanCargoAllUnload = true;
							int iVisibleEnemyDefenders = pLoopPlot->getNumVisibleEnemyDefenders(this);

							if (iVisibleEnemyDefenders > 0 || pLoopPlot->isCity())
							{
								for (uint i = 0; i < aGroupCargo.size(); ++i)
								{
									CvUnit* pAttacker = aGroupCargo[i];
									if( iVisibleEnemyDefenders > 0 )
									{
										const bool bDefender = pLoopPlot->hasDefender(true, NO_PLAYER, pAttacker->getOwnerINLINE(), pAttacker, true);

										if (!bDefender == NULL || !pAttacker->canAttack())
										{
											bCanCargoAllUnload = false;
											break;
										}
									}
								}
							}

							if (bCanCargoAllUnload)
							{
								int iPathTurns;

								if (generatePath(pLoopPlot, MOVE_AVOID_ENEMY_WEIGHT_3, true, &iPathTurns))
								{
									int iValue = 1;

									if (!bIsAttackCity)
									{
										iValue += (AI_pillageValue(pLoopPlot) * 10);
									}

									int iAssaultsHere = GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_ASSAULT, getGroup());

									iValue += (iAssaultsHere * 100);

									CvCity* pCity = pLoopPlot->getPlotCity();

									if (pCity == NULL)
									{
										for (int iJ = 0; iJ < NUM_DIRECTION_TYPES; iJ++)
										{
											CvPlot* pAdjacentPlot = plotDirection(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), ((DirectionTypes)iJ));

											if (pAdjacentPlot != NULL)
											{
												pCity = pAdjacentPlot->getPlotCity();

												if (pCity != NULL)
												{
													if (pCity->getOwnerINLINE() == pLoopPlot->getOwnerINLINE())
													{
														break;
													}
													else
													{
														pCity = NULL;
													}
												}
											}
										}
									}

									if (pCity != NULL)
									{
										FAssert(isPotentialEnemy(pCity->getTeam(), pLoopPlot));

										if (!(pLoopPlot->isRiverCrossing(directionXY(pLoopPlot, pCity->plot()))))
										{
											iValue += (50 * -(GC.getRIVER_ATTACK_MODIFIER()));
										}

										iValue += 15 * (pLoopPlot->defenseModifier(getTeam(), false));
										iValue += 1000;
										iValue += (GET_PLAYER(getOwnerINLINE()).AI_adjacentPotentialAttackers(pCity->plot()) * 200);

										// Continue attacking in area we have already captured cities
										if( pCity->area()->getCitiesPerPlayer(getOwnerINLINE()) > 0 )
										{
											if( pCity->AI_playerCloseness(getOwnerINLINE()) > 5 )
											{
												iValue *= 3;
												iValue /= 2;
											}
										}

										if (iPathTurns == 1)
										{
											iValue += GC.getGameINLINE().getSorenRandNum(50, "AI Assault");
										}
									}

									FAssert(iPathTurns > 0);

									if (iPathTurns == 1)
									{
										if (pCity != NULL)
										{
											if (pCity->area()->getNumCities() > 1)
											{
												iValue *= 2;
											}
										}
									}

									iValue *= 1000;

									if (iTargetCities <= iAssaultsHere)
									{
										iValue /= 2;
									}

									if (iTargetCities == 1)
									{
										if (iCargo > 7)
										{
											iValue *= 3;
											iValue /= iCargo - 4;
										}
									}

									if (pLoopPlot->isCity())
									{
										if (iVisibleEnemyDefenders * 3 > iCargo)
										{
											iValue /= 10;
										}
										else
										{
											// Assume non-visible city is properly defended
											iValue *= iCargo;
											iValue /= std::max(pLoopPlot->getPlotCity()->AI_neededDefenders(), (iVisibleEnemyDefenders * 3));
										}
									}
									else
									{
										if (0 == iVisibleEnemyDefenders)
										{
											iValue *= 4;
											iValue /= 3;
										}
										else
										{
											iValue /= iVisibleEnemyDefenders;
										}
									}

									// if more than 3 turns to get there, then put some randomness into our preference of distance
									// +/- 33%
									if (iPathTurns > 3)
									{
										int iPathAdjustment = GC.getGameINLINE().getSorenRandNum(67, "AI Assault Target");

										iPathTurns *= 66 + iPathAdjustment;
										iPathTurns /= 100;
									}

									iValue /= (iPathTurns + 1);

									if (iValue > iBestValue)
									{
										iBestValue = iValue;
										pBestPlot = getPathEndTurnPlot();
										pBestAssaultPlot = pLoopPlot;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestAssaultPlot != NULL))
	{
		FAssert(!(pBestPlot->isImpassable()));

		// Cancel missions of all those coming to join departing transport
		CvSelectionGroup* pLoopGroup = NULL;
		int iLoop = 0;
		CvPlayer& kPlayer = GET_PLAYER(getOwnerINLINE());

		for(pLoopGroup = kPlayer.firstSelectionGroup(&iLoop); pLoopGroup != NULL; pLoopGroup = kPlayer.nextSelectionGroup(&iLoop))
		{
			if( pLoopGroup != getGroup() )
			{
				if( pLoopGroup->AI_getMissionAIType() == MISSIONAI_GROUP && pLoopGroup->getHeadUnitAI() == AI_getUnitAIType() )
				{
					CvUnit* pMissionUnit = pLoopGroup->AI_getMissionAIUnit();

					if( pMissionUnit != NULL && pMissionUnit->getGroup() == getGroup() )
					{
						pLoopGroup->clearMissionQueue();
					}
				}
			}
		}

		if ((pBestPlot == pBestAssaultPlot) || (stepDistance(pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), pBestAssaultPlot->getX_INLINE(), pBestAssaultPlot->getY_INLINE()) == 1))
		{
			if (atPlot(pBestAssaultPlot))
			{
				getGroup()->unloadAll(); // XXX is this dangerous (not pushing a mission...) XXX air units?
				return true;
			}
			else
			{
				getGroup()->pushMission(MISSION_MOVE_TO, pBestAssaultPlot->getX_INLINE(), pBestAssaultPlot->getY_INLINE(), MOVE_AVOID_ENEMY_WEIGHT_3, false, false, MISSIONAI_ASSAULT, pBestAssaultPlot);
				return true;
			}
		}
		else
		{
			FAssert(!atPlot(pBestPlot));
			getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_AVOID_ENEMY_WEIGHT_3, false, false, MISSIONAI_ASSAULT, pBestAssaultPlot);
			return true;
		}
	}

	return false;
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_assaultSeaReinforce(bool bNative)
{
	PROFILE_FUNC();

	bool bIsAttackCity = (getUnitAICargo(UNITAI_OFFENSIVE) > 0);

	FAssert(getGroup()->hasCargo());

	if (!canCargoAllMove())
	{
		return false;
	}

	if( !(getGroup()->canAllMove()) )
	{
		return false;
	}

	std::vector<CvUnit*> aGroupCargo;
	CLLNode<IDInfo>* pUnitNode = plot()->headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = plot()->getUnitNodeLoop(pUnitNode);
		if (pLoopUnit != NULL)
		{
			CvUnit* pTransport = pLoopUnit->getTransportUnit();
			if (pTransport != NULL && pTransport->getGroup() == getGroup())
			{
				aGroupCargo.push_back(pLoopUnit);
			}
		}
	}

	int iCargo = getGroup()->getCargo();
	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	CvPlot* pBestAssaultPlot = NULL;
	CvArea* pWaterArea = plot()->waterArea();
	bool bCity = plot()->isCity(true,getTeam());

	bool bCanMoveAllTerrain = false;

	int iTargetCities;
	int iOurFightersHere;
	int iPathTurns;
	int iValue;

	// Loop over nearby plots for groups in enemy territory to reinforce
	int iRange = 2* baseMoves();
	int iDX, iDY;
	for (iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for (iDY = -(iRange); iDY <= iRange; iDY++)
		{
			CvPlot* pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

			if( pLoopPlot != NULL )
			{
				if (pLoopPlot->isOwned())
				{
					if (isEnemy(pLoopPlot->getTeam(), pLoopPlot))
					{
						if ( bCanMoveAllTerrain || (pWaterArea != NULL && pLoopPlot->isAdjacentToArea(pWaterArea)) )
						{
							iTargetCities = pLoopPlot->area()->getCitiesPerPlayer(pLoopPlot->getOwnerINLINE());

							if (iTargetCities > 0)
							{
								iOurFightersHere = pLoopPlot->getNumDefenders(getOwnerINLINE());

								if( iOurFightersHere > 2 )
								{
									iPathTurns;
									if (generatePath(pLoopPlot, MOVE_AVOID_ENEMY_WEIGHT_3, true, &iPathTurns))
									{
										if( iPathTurns <= 2 )
										{
											CvPlot* pEndTurnPlot = getPathEndTurnPlot();

											iValue = 10*iTargetCities;
											iValue += 8*iOurFightersHere;
											iValue += 3*GET_PLAYER(getOwnerINLINE()).AI_adjacentPotentialAttackers(pLoopPlot);

											iValue *= 100;

											iValue /= (iPathTurns + 1);

											if (iValue > iBestValue)
											{
												iBestValue = iValue;
												pBestPlot = pEndTurnPlot;
												pBestAssaultPlot = pLoopPlot;
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

	// Loop over other transport groups, looking for synchronized landing
	if ((pBestPlot == NULL) && (pBestAssaultPlot == NULL))
	{
		int iLoop;
		for(CvSelectionGroup* pLoopSelectionGroup = GET_PLAYER(getOwnerINLINE()).firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = GET_PLAYER(getOwnerINLINE()).nextSelectionGroup(&iLoop))
		{
			if (pLoopSelectionGroup != getGroup())
			{
				if (pLoopSelectionGroup->AI_getMissionAIType() == MISSIONAI_ASSAULT)
				{
					CvPlot* pLoopPlot = pLoopSelectionGroup->AI_getMissionAIPlot();

					if( pLoopPlot != NULL )
					{
						if (pLoopPlot->isOwned())
						{
							if (isPotentialEnemy(pLoopPlot->getTeam(), pLoopPlot))
							{
								if ( bCanMoveAllTerrain || (pWaterArea != NULL && pLoopPlot->isAdjacentToArea(pWaterArea)) )
								{
									iTargetCities = pLoopPlot->area()->getCitiesPerPlayer(pLoopPlot->getOwnerINLINE());
									if (iTargetCities > 0)
									{
										int iAssaultsHere = pLoopSelectionGroup->getCargo();

										if( iAssaultsHere > 2 )
										{
											iPathTurns;
											if (generatePath(pLoopPlot, MOVE_AVOID_ENEMY_WEIGHT_3, true, &iPathTurns))
											{
												CvPlot* pEndTurnPlot = getPathEndTurnPlot();

												int iOtherPathTurns = MAX_INT;
												if (pLoopSelectionGroup->generatePath(pLoopSelectionGroup->plot(), pLoopPlot, MOVE_AVOID_ENEMY_WEIGHT_3, true, &iOtherPathTurns))
												{
													// We need to get there the turn after they do, +1 required whether
													// they move first or we do
													iOtherPathTurns += 1;
												}
												else
												{
													// Should never happen ...
													continue;
												}

												if( (iPathTurns >= iOtherPathTurns) && (iPathTurns < iOtherPathTurns + 5) )
												{
													bool bCanCargoAllUnload = true;
													int iVisibleEnemyDefenders = pLoopPlot->getNumVisibleEnemyDefenders(this);
													if (iVisibleEnemyDefenders > 0 || pLoopPlot->isCity())
													{
														for (uint i = 0; i < aGroupCargo.size(); ++i)
														{
															CvUnit* pAttacker = aGroupCargo[i];
															const bool bDefender = pLoopPlot->hasDefender(true, NO_PLAYER, pAttacker->getOwnerINLINE(), pAttacker, true);

															if (!bDefender || !pAttacker->canAttack())
															{
																bCanCargoAllUnload = false;
																break;
															}
														}
													}

													iValue = (iAssaultsHere * 5);
													iValue += iTargetCities*10;

													iValue *= 100;

													// if more than 3 turns to get there, then put some randomness into our preference of distance
													// +/- 33%
													if (iPathTurns > 3)
													{
														int iPathAdjustment = GC.getGameINLINE().getSorenRandNum(67, "AI Assault Target");

														iPathTurns *= 66 + iPathAdjustment;
														iPathTurns /= 100;
													}

													iValue /= (iPathTurns + 1);

													if (iValue > iBestValue)
													{
														iBestValue = iValue;
														pBestPlot = pEndTurnPlot;
														pBestAssaultPlot = pLoopPlot;
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
	}

	// Reinforce our cities in need
	if ((pBestPlot == NULL) && (pBestAssaultPlot == NULL))
	{
		int iLoop;
		CvCity* pLoopCity;

		for (pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop))
		{
			if( bCanMoveAllTerrain || (pWaterArea != NULL && (pLoopCity->waterArea() == pWaterArea)) )
			{
				iValue = 0;
				if(pLoopCity->area()->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE)
				{
					iValue = 3;
				}
				else if(pLoopCity->area()->getAreaAIType(getTeam()) == AREAAI_OFFENSIVE)
				{
					iValue = 2;
				}
				else if(pLoopCity->area()->getAreaAIType(getTeam()) == AREAAI_MASSING)
				{
					iValue = 1;
				}

				if( iValue > 0 )
				{
					bool bCityDanger = pLoopCity->AI_isDanger();
					if( (bCity && pLoopCity->area() != area()) || bCityDanger || ((GC.getGameINLINE().getGameTurn() - pLoopCity->getGameTurnAcquired()) < 10 && pLoopCity->getPreviousOwner() != NO_PLAYER) )
					{
						int iOurPower = std::max(1, pLoopCity->area()->getPower(getOwnerINLINE()));
						// Enemy power includes barb power
						int iEnemyPower = GET_TEAM(getTeam()).AI_countEnemyPowerByArea(*pLoopCity->area());

						// Don't send troops to areas we are dominating already
						// Don't require presence of enemy cities, just a dangerous force
						if( iOurPower < (3*iEnemyPower) )
						{
							iPathTurns;

							if (generatePath(pLoopCity->plot(), MOVE_AVOID_ENEMY_WEIGHT_3, true, &iPathTurns))
							{
								iValue *= 10*pLoopCity->AI_cityThreat();

								iValue += 20 * GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_ASSAULT, getGroup());

								iValue *= std::min(iEnemyPower, 3*iOurPower);
								iValue /= iOurPower;

								iValue *= 100;

								// if more than 3 turns to get there, then put some randomness into our preference of distance
								// +/- 33%
								if (iPathTurns > 3)
								{
									int iPathAdjustment = GC.getGameINLINE().getSorenRandNum(67, "AI Assault Target");

									iPathTurns *= 66 + iPathAdjustment;
									iPathTurns /= 100;
								}

								iValue /= (iPathTurns + 6);

								if (iValue > iBestValue)
								{
									iBestValue = iValue;
									pBestPlot = (bCityDanger ? getPathEndTurnPlot() : pLoopCity->plot());
									pBestAssaultPlot = pLoopCity->plot();
								}
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestAssaultPlot != NULL))
	{
		FAssert(!(pBestPlot->isImpassable()));

		// Cancel missions of all those coming to join departing transport
		CvSelectionGroup* pLoopGroup = NULL;
		int iLoop = 0;
		CvPlayer& kPlayer = GET_PLAYER(getOwnerINLINE());

		for(pLoopGroup = kPlayer.firstSelectionGroup(&iLoop); pLoopGroup != NULL; pLoopGroup = kPlayer.nextSelectionGroup(&iLoop))
		{
			if( pLoopGroup != getGroup() )
			{
				if( pLoopGroup->AI_getMissionAIType() == MISSIONAI_GROUP && pLoopGroup->getHeadUnitAI() == AI_getUnitAIType() )
				{
					CvUnit* pMissionUnit = pLoopGroup->AI_getMissionAIUnit();

					if( pMissionUnit != NULL && pMissionUnit->getGroup() == getGroup() )
					{
						pLoopGroup->clearMissionQueue();
					}
				}
			}
		}

		if ((pBestPlot == pBestAssaultPlot) || (stepDistance(pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), pBestAssaultPlot->getX_INLINE(), pBestAssaultPlot->getY_INLINE()) == 1))
		{
			if (atPlot(pBestAssaultPlot))
			{
				getGroup()->unloadAll(); // XXX is this dangerous (not pushing a mission...) XXX air units?
				return true;
			}
			else
			{
				getGroup()->pushMission(MISSION_MOVE_TO, pBestAssaultPlot->getX_INLINE(), pBestAssaultPlot->getY_INLINE(), MOVE_AVOID_ENEMY_WEIGHT_3, false, false, MISSIONAI_ASSAULT, pBestAssaultPlot);
				return true;
			}
		}
		else
		{
			FAssert(!atPlot(pBestPlot));
			getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_AVOID_ENEMY_WEIGHT_3, false, false, MISSIONAI_ASSAULT, pBestAssaultPlot);
			return true;
		}
	}

	return false;
}
// TAC - AI Assault Sea - koma13, jdog5000(BBAI) - END

// Returns true if a mission was pushed...
bool CvUnitAI::AI_settlerSeaFerry()
{
	PROFILE_FUNC();

	FAssert(getCargo() > 0);
	FAssert(getUnitAICargo(UNITAI_SETTLER) > 0);

	if (!canCargoAllMove())
	{
		return false;
	}

	CvArea* pWaterArea = plot()->waterArea();
	FAssertMsg(pWaterArea != NULL, "Ship out of water?");

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;

	CvCity* pLoopCity;
	int iLoop;
	for (pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop))
	{
		int iValue = pLoopCity->AI_getWorkersNeeded();
		if (iValue > 0)
		{
			iValue -= GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_FOUND, getGroup());
			if (iValue > 0)
			{
				int iPathTurns;
				if (generatePath(pLoopCity->plot(), 0, true, &iPathTurns))
				{
					iValue += std::max(0, (GET_PLAYER(getOwnerINLINE()).AI_neededWorkers(pLoopCity->area()) - GET_PLAYER(getOwnerINLINE()).AI_totalAreaUnitAIs(pLoopCity->area(), UNITAI_WORKER)));
					iValue *= 1000;
					iValue /= 4 + iPathTurns;
					if (atPlot(pLoopCity->plot()))
					{
						iValue += 100;
					}
					else
					{
						iValue += GC.getGame().getSorenRandNum(100, "AI settler sea ferry");
					}
					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestPlot = pLoopCity->plot();
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		if (atPlot(pBestPlot))
		{
			unloadAll(); // XXX is this dangerous (not pushing a mission...)
			return true;
		}
		else
		{
			getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), 0, false, false, MISSIONAI_FOUND, pBestPlot);
			return true;
		}
	}
	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_specialSeaTransportMissionary()
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;
	CvCity* pCity;
	CvUnit* pMissionaryUnit;
	CvUnit* pLoopUnit;
	CvPlot* pLoopPlot;
	CvPlot* pPlot;
	CvPlot* pBestPlot;
	CvPlot* pBestSpreadPlot;
	int iPathTurns;
	int iValue;
	int iBestValue;
	int iI;

	FAssert(getCargo() > 0);
	FAssert(getUnitAICargo(UNITAI_MISSIONARY) > 0);

	if (!canCargoAllMove())
	{
		return false;
	}

	pPlot = plot();

	pMissionaryUnit = NULL;

	pUnitNode = pPlot->headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = pPlot->getUnitNodeLoop(pUnitNode);

		if (pLoopUnit != NULL && pLoopUnit->getTransportUnit() == this)
		{
			if (pLoopUnit->AI_getUnitAIType() == UNITAI_MISSIONARY)
			{
				pMissionaryUnit = pLoopUnit;
				break;
			}
		}
	}

	if (pMissionaryUnit == NULL)
	{
		return false;
	}

	iBestValue = 0;
	pBestPlot = NULL;
	pBestSpreadPlot = NULL;

	// XXX what about non-coastal cities?
	for (iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		pLoopPlot = GC.getMap().plotByIndexINLINE(iI);

		if (pLoopPlot->isCoastalLand())
		{
			pCity = pLoopPlot->getPlotCity();

			if (pCity != NULL)
			{
				iValue = 0;

				if (iValue > 0)
				{
					if (!(pLoopPlot->isVisibleEnemyUnit(this)))
					{
						if (GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_SPREAD, getGroup()) == 0)
						{
							if (generatePath(pLoopPlot, 0, true, &iPathTurns))
							{
								iValue *= pCity->getPopulation();

								if (pCity->getOwnerINLINE() == getOwnerINLINE())
								{
									iValue *= 4;
								}
								else if (pCity->getTeam() == getTeam())
								{
									iValue *= 3;
								}

								FAssert(iPathTurns > 0);

								if (iPathTurns == 1)
								{
									iValue *= 2;
								}

								iValue *= 1000;

								iValue /= (iPathTurns + 1);

								if (iValue > iBestValue)
								{
									iBestValue = iValue;
									pBestPlot = getPathEndTurnPlot();
									pBestSpreadPlot = pLoopPlot;
								}
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestSpreadPlot != NULL))
	{
		FAssert(!(pBestPlot->isImpassable()) || canMoveImpassable());

		if ((pBestPlot == pBestSpreadPlot) || (stepDistance(pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), pBestSpreadPlot->getX_INLINE(), pBestSpreadPlot->getY_INLINE()) == 1))
		{
			if (atPlot(pBestSpreadPlot))
			{
				unloadAll(); // XXX is this dangerous (not pushing a mission...)
				return true;
			}
			else
			{
				getGroup()->pushMission(MISSION_MOVE_TO, pBestSpreadPlot->getX_INLINE(), pBestSpreadPlot->getY_INLINE(), 0, false, false, MISSIONAI_SPREAD, pBestSpreadPlot);
				return true;
			}
		}
		else
		{
			FAssert(!atPlot(pBestPlot));
			getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), 0, false, false, MISSIONAI_SPREAD, pBestSpreadPlot);
			return true;
		}
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_connectPlot(CvPlot* pPlot, int iRange)
{
	PROFILE_FUNC();

	FAssert(canBuildRoute());

	if (!(pPlot->isVisibleEnemyUnit(this)))
	{
		if (GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pPlot, MISSIONAI_BUILD, getGroup(), iRange) == 0)
		{
			if (generatePath(pPlot, MOVE_SAFE_TERRITORY, true))
			{
				getGroup()->pushMission(MISSION_ROUTE_TO, pPlot->getX_INLINE(), pPlot->getY_INLINE(), MOVE_SAFE_TERRITORY, false, false, MISSIONAI_BUILD, pPlot);
				return true;
			}
		}
	}

	return false;
}

bool CvUnitAI::AI_improveCity(CvCity* pCity)
{
	PROFILE_FUNC();

	CvPlot* pBestPlot = NULL;
	BuildTypes eBestBuild = NO_BUILD;
	if (!AI_bestCityBuild(pCity, &pBestPlot, &eBestBuild, NULL, this))
		return false; // advc
	FAssert(pBestPlot != NULL);
	
	MissionTypes eMission = MISSION_MOVE_TO;
	if (getPlot().getWorkingCity() != pCity /*||
		GC.getInfo(eBestBuild).getRoute() != NO_ROUTE*/) // advc.121: Walk don't route
	{
		eMission = MISSION_ROUTE_TO;
	}
	else
	{
		if (generatePath(pBestPlot) && getPathFinder().getPathTurns() == 1 &&
			getPathFinder().getFinalMoves() == 0)
		{
			if (pBestPlot->isRoute())
				eMission = MISSION_ROUTE_TO;
		}
		else if (!getPlot().isRoute())
		{
			int iPlotMoveCost = 0;
			iPlotMoveCost = ((plot()->getFeatureType() == NO_FEATURE) ? GC.getTerrainInfo(plot()->getTerrainType()).getMovementCost() : GC.getFeatureInfo(plot()->getFeatureType()).getMovementCost());

			if (plot()->isHills())
				iPlotMoveCost += GLOBAL_DEFINE_HILLS_EXTRA_MOVEMENT;
			if (plot()->isPeak())
				iPlotMoveCost += GLOBAL_DEFINE_PEAK_EXTRA_MOVEMENT;
			if (iPlotMoveCost > 1)
				eMission = MISSION_ROUTE_TO;
		}
	}

	eBestBuild = AI_betterPlotBuild(pBestPlot, eBestBuild);

	// WTP: Builds generally cost gold so this check is necessary
	if (canBuild(pBestPlot, eBestBuild))
	{
		getGroup()->pushMission(eMission,
			pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(),
			NO_MOVEMENT_FLAGS, false, false,
			MISSIONAI_BUILD, pBestPlot);
		getGroup()->pushMission(MISSION_BUILD,
			eBestBuild, -1, NO_MOVEMENT_FLAGS,
			/*(getGroup()->getLengthMissionQueue() > 0)*/ true, // K-Mod
			false, MISSIONAI_BUILD, pBestPlot);
	}
	else
	{
		// Not enough gold :(
		getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_NO_GOLD, pBestPlot);
	}
	
	return true;
}


// Unused
#if 0
bool CvUnitAI::AI_improveLocalPlot(int iRange, CvCity* pIgnoreCity)
{

	int iX, iY;

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	BuildTypes eBestBuild = NO_BUILD;

	for (iX = -iRange; iX <= iRange; iX++)
	{
		for (iY = -iRange; iY <= iRange; iY++)
		{
			CvPlot* pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iX, iY);
			if ((pLoopPlot != NULL) && (pLoopPlot->isCityRadius()))
			{
				CvCity* pCity = pLoopPlot->getWorkingCity();
				if ((NULL != pCity) && (pCity->getOwnerINLINE() == getOwnerINLINE()))
				{
					if ((NULL == pIgnoreCity) || (pCity != pIgnoreCity))
					{
						if (AI_plotValid(pLoopPlot))
						{
							int iIndex = pCity->getCityPlotIndex(pLoopPlot);
							if (iIndex != CITY_HOME_PLOT)
							{
								if (((NULL == pIgnoreCity) || ((pCity->AI_getWorkersNeeded() > 0) && (pCity->AI_getWorkersHave() < (1 + pCity->AI_getWorkersNeeded() * 2 / 3)))) && (pCity->AI_getBestBuild(iIndex) != NO_BUILD))
								{
									if (canBuild(pLoopPlot, pCity->AI_getBestBuild(iIndex)))
									{
										bool bAllowed = true;

										if (GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_SAFE_AUTOMATION))
										{
											// TODO: figure out if forts should be protected here too
											if (pLoopPlot->getImprovementType() != NO_IMPROVEMENT && GC.getImprovementInfo(pLoopPlot->getImprovementType()).isAINoRemove())
											{
												bAllowed = false;
											}
										}

										if (bAllowed)
										{
											if (pLoopPlot->getImprovementType() != NO_IMPROVEMENT && GC.getBuildInfo(pCity->AI_getBestBuild(iIndex)).getImprovement() != NO_IMPROVEMENT)
											{
												bAllowed = false;
											}
										}

										if (bAllowed)
										{
											int iValue = pCity->AI_getBestBuildValue(iIndex);
											int iPathTurns;
											if (generatePath(pLoopPlot, 0, true, &iPathTurns))
											{
												int iMaxWorkers = 1;
												if (plot() == pLoopPlot)
												{
													iValue *= 3;
													iValue /= 2;
												}
												else if (getPathFinder().GetFinalMoves() == 0)
												{
													iPathTurns++;
												}
												else if (iPathTurns <= 1)
												{
													iMaxWorkers = AI_calculatePlotWorkersNeeded(pLoopPlot, pCity->AI_getBestBuild(iIndex));
												}

												if (GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BUILD, getGroup()) < iMaxWorkers)
												{
													iValue *= 1000;
													iValue /= 1 + iPathTurns;

													if (iValue > iBestValue)
													{
														iBestValue = iValue;
														pBestPlot = pLoopPlot;
														eBestBuild = pCity->AI_getBestBuild(iIndex);
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
	}

	if (pBestPlot != NULL)
	{
	    FAssertMsg(eBestBuild != NO_BUILD, "BestBuild is not assigned a valid value");
	    FAssertMsg(eBestBuild < GC.getNumBuildInfos(), "BestBuild is assigned a corrupt value");

		FAssert(pBestPlot->getWorkingCity() != NULL);
		if (NULL != pBestPlot->getWorkingCity())
		{
			pBestPlot->getWorkingCity()->AI_changeWorkersHave(+1);

			if (plot()->getWorkingCity() != NULL)
			{
				plot()->getWorkingCity()->AI_changeWorkersHave(-1);
			}
		}
		MissionTypes eMission = MISSION_MOVE_TO;

		int iPathTurns;
		if (generatePath(pBestPlot, 0, true, &iPathTurns) && (getPathFinder().GetPathTurns() == 1) && (getPathFinder().GetFinalMoves() == 0))
		{
			if (pBestPlot->getRouteType() != NO_ROUTE)
			{
				eMission = MISSION_ROUTE_TO;
			}
		}
		else if (plot()->getRouteType() == NO_ROUTE)
		{
			int iPlotMoveCost = 0;
			iPlotMoveCost = ((plot()->getFeatureType() == NO_FEATURE) ? 
				GC.getTerrainInfo(plot()->getTerrainType()).getMovementCost() : 
				GC.getFeatureInfo(plot()->getFeatureType()).getMovementCost());

			if (plot()->isHills())
				iPlotMoveCost += GLOBAL_DEFINE_HILLS_EXTRA_MOVEMENT;
			if (plot()->isPeak())
				iPlotMoveCost += GLOBAL_DEFINE_PEAK_EXTRA_MOVEMENT;
			if (iPlotMoveCost > 1)
				eMission = MISSION_ROUTE_TO;
		}

		eBestBuild = AI_betterPlotBuild(pBestPlot, eBestBuild);

		getGroup()->pushMission(eMission, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), 0, false, false, MISSIONAI_BUILD, pBestPlot);
		getGroup()->pushMission(MISSION_BUILD, eBestBuild, -1, 0, (getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pBestPlot);
		return true;
	}

	return false;
}
#endif

// Returns true if a mission was pushed...
bool CvUnitAI::AI_nextCityToImprove(CvCity* pCity)
{
	PROFILE_FUNC();

	CvCity* pLoopCity;
	CvPlot* pPlot;
	CvPlot* pBestPlot;
	BuildTypes eBuild;
	BuildTypes eBestBuild;
	int iPathTurns;
	int iValue;
	int iBestValue;
	int iLoop;

	iBestValue = 0;
	eBestBuild = NO_BUILD;
	pBestPlot = NULL;

	for (pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop))
	{
		if (pLoopCity != pCity)
		{
			int iWorkersNeeded = pLoopCity->AI_getWorkersNeeded();
			int iWorkersHave = pLoopCity->AI_getWorkersHave();

			iValue = std::max(0, iWorkersNeeded - iWorkersHave) * 100;
			iValue += iWorkersNeeded * 10;
			iValue *= (iWorkersNeeded + 1);
			iValue /= (iWorkersHave + 1);

			if (iValue > 0)
			{
				if (AI_bestCityBuild(pLoopCity, &pPlot, &eBuild, NULL, this))
				{
					FAssert(pPlot != NULL);
					FAssert(eBuild != NO_BUILD);

					iValue *= 1000;

					if (pLoopCity->isCapital())
					{
					    iValue *= 2;
					}

					generatePath(pPlot, 0, true, &iPathTurns);
					iValue /= (iPathTurns + 1);

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						eBestBuild = eBuild;
						pBestPlot = pPlot;
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		gDLL->getFAStarIFace()->ForceReset(&GC.getRouteFinder());

		CvCity* pThisCity = plot()->getWorkingCity();
		CvCity* pOtherCity = pBestPlot->getWorkingCity();

		if (pOtherCity != NULL)
		{
			setHomeCity(pOtherCity);
		}

		if (pThisCity != NULL && pOtherCity != NULL)
		{
			if (!(gDLL->getFAStarIFace()->GeneratePath(&GC.getRouteFinder(), pThisCity->getX_INLINE(), pThisCity->getY_INLINE(), pOtherCity->getX_INLINE(), pOtherCity->getY_INLINE(), false, getOwnerINLINE(), true)))
			{
				getGroup()->pushMission(MISSION_MOVE_TO, pThisCity->getX_INLINE(), pThisCity->getY_INLINE(), NO_MOVEMENT_FLAGS, false, false, MISSIONAI_BUILD, pOtherCity->plot());
				getGroup()->pushMission(MISSION_ROUTE_TO, pOtherCity->getX_INLINE(), pOtherCity->getY_INLINE(), NO_MOVEMENT_FLAGS, true, false, MISSIONAI_BUILD, pOtherCity->plot());
				return true;
			}
		}

		eBestBuild = AI_betterPlotBuild(pBestPlot, eBestBuild);

		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), NO_MOVEMENT_FLAGS, false, false, MISSIONAI_BUILD, pBestPlot);
		getGroup()->pushMission(MISSION_BUILD,
			eBestBuild, -1, NO_MOVEMENT_FLAGS,
			//(getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pBestPlot);
			true, false, MISSIONAI_BUILD, pBestPlot); // K-Mod
		return true;
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_fortTerritory(bool bCanal)
{
	int iBestValue = 0;
	BuildTypes eBestBuild = NO_BUILD;
	CvPlot* pBestPlot = NULL;

	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);

		if (AI_plotValid(pLoopPlot))
		{
			if (pLoopPlot->getOwnerINLINE() == getOwnerINLINE()
				// Super Forts *canal* *choke* begin
				|| (pLoopPlot->getOwnerINLINE() == NO_PLAYER && pLoopPlot->isRevealed(getTeam(), false))
				// Super Forts end
				) // XXX team???
			{
				if (pLoopPlot->getImprovementType() == NO_IMPROVEMENT
					// Super Forts *canal* *choke* begin
					|| !pLoopPlot->isCityRadius())
					// Super Forts end
				{
					int iValue = 0;
					iValue += bCanal ? kOwner.AI_getPlotCanalValue(pLoopPlot) : 0;

					// Super Forts *choke* begin
					iValue += kOwner.AI_getPlotChokeValue(pLoopPlot);

					int iMinAcceptableValue = 0;
					if(pLoopPlot->getOwnerINLINE() == NO_PLAYER)
					{	// Don't go outside borders for low values
						iMinAcceptableValue += 150;
					}

					//if (iValue > 0) -- original
					if (iValue > iMinAcceptableValue)
					//super forts end
					{
						int iBestTempBuildValue = MAX_INT;
						BuildTypes eBestTempBuild = NO_BUILD;

						// Super forts begin
						int iPlotValue = iValue;
						iPlotValue += bCanal ? 0 : kOwner.AI_getPlotCanalValue(pLoopPlot) / 4;
						// Super Forts end

						for (int iJ = 0; iJ < GC.getNumBuildInfos(); iJ++)
						{
							BuildTypes eBuild = ((BuildTypes)iJ);
							FAssertMsg(eBuild < GC.getNumBuildInfos(), "Invalid Build");

							if (GC.getBuildInfo(eBuild).getImprovement() != NO_IMPROVEMENT)
							{
								if (GC.getImprovementInfo((ImprovementTypes)(GC.getBuildInfo(eBuild).getImprovement())).isActsAsCity())
								{
								    if (GC.getImprovementInfo((ImprovementTypes)(GC.getBuildInfo(eBuild).getImprovement())).getDefenseModifier() > 0)
								    {
                                        if (canBuild(pLoopPlot, eBuild))
                                        {
                                            iValue = 10000;

                                            iValue /= (GC.getBuildInfo(eBuild).getTime() + 1);

                                            if (iValue < iBestTempBuildValue)
                                            {
                                                iBestTempBuildValue = iValue;
                                                eBestTempBuild = eBuild;
                                            }
                                        }
                                    }
								}
							}
						}

						if (eBestTempBuild != NO_BUILD)
						{
							if (!(pLoopPlot->isVisibleEnemyUnit(this)))
							{
								bool bValid = true;

								if (GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_LEAVE_FORESTS))
								{
									if (pLoopPlot->getFeatureType() != NO_FEATURE)
									{
										if (GC.getBuildInfo(eBestTempBuild).isFeatureRemove(pLoopPlot->getFeatureType()))
										{
											bValid = false;
										}
									}
								}

								if (bValid)
								{

									if (GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BUILD, getGroup(), 1/*3*/) == 0) //super forts - change 3 to 1
									{
										int iPathTurns;
										if (generatePath(pLoopPlot, 0, true, &iPathTurns))
										{

											iValue = iPlotValue * 100; //iValue *= 1000; --original		// Super Forts *canal* *choke*
											iValue /= (iPathTurns + 1);


											// Super Forts begin *canal* *choke*
											if(pLoopPlot->getOwnerINLINE() == NO_PLAYER)
											{
												CvCity* pNearestCity = GC.getMap().findCity(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), getOwnerINLINE(), NO_TEAM, false);
												if((pNearestCity == NULL) ||
													(plotDistance(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), pNearestCity->getX_INLINE(), pNearestCity->getY_INLINE()) > GLOBAL_DEFINE_AI_WORKER_MAX_DISTANCE_FROM_CITY_OUT_BORDERS) ||
													(iPathTurns > (GLOBAL_DEFINE_AI_WORKER_MAX_DISTANCE_FROM_CITY_OUT_BORDERS / 2)))
												{
													iValue = 0;
												}
											}
											// Super Forts end

											if (iValue > iBestValue)
											{
												iBestValue = iValue;
												eBestBuild = eBestTempBuild;
												pBestPlot = pLoopPlot;
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

	if (pBestPlot != NULL)
	{
		FAssertMsg(eBestBuild != NO_BUILD, "BestBuild is not assigned a valid value");
		FAssertMsg(eBestBuild < GC.getNumBuildInfos(), "BestBuild is assigned a corrupt value");

		getGroup()->pushMission(MISSION_ROUTE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), 0, false, false, MISSIONAI_BUILD, pBestPlot);
		getGroup()->pushMission(MISSION_BUILD, eBestBuild, -1, 0, (getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pBestPlot);

		return true;
	}
	return false;
}

//returns true if a mission is pushed
//if eBuild is NO_BUILD, assumes a route is desired.
bool CvUnitAI::AI_improvePlot(CvPlot* pPlot, BuildTypes eBuild)
{
	FAssert(pPlot != NULL);

	if (eBuild != NO_BUILD)
	{
		FAssertMsg(eBuild < GC.getNumBuildInfos(), "BestBuild is assigned a corrupt value");

		eBuild = AI_betterPlotBuild(pPlot, eBuild);
		if (!atPlot(pPlot))
		{
			getGroup()->pushMission(MISSION_MOVE_TO, pPlot->getX_INLINE(), pPlot->getY_INLINE(), 0, false, false, MISSIONAI_BUILD, pPlot);
		}
		getGroup()->pushMission(MISSION_BUILD, eBuild, -1, 0, (getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pPlot);

		return true;
	}
	else if (canBuildRoute())
	{
		if (AI_connectPlot(pPlot))
		{
			return true;
		}
	}

	return false;

}

BuildTypes CvUnitAI::AI_betterPlotBuild(CvPlot* pPlot, BuildTypes eBuild)
{
	FAssert(pPlot != NULL);
	FAssert(eBuild != NO_BUILD);
	bool bBuildRoute = false;
	bool bClearFeature = false;

	FeatureTypes eFeature = pPlot->getFeatureType();

	CvBuildInfo& kOriginalBuildInfo = GC.getBuildInfo(eBuild);

	if (kOriginalBuildInfo.getRoute() != NO_ROUTE)
	{
		return eBuild;
	}

	int iWorkersNeeded = AI_calculatePlotWorkersNeeded(pPlot, eBuild);

	if ((pPlot->getBonusType() == NO_BONUS) && (pPlot->getWorkingCity() != NULL))
	{
		iWorkersNeeded = std::max(1, std::min(iWorkersNeeded, pPlot->getWorkingCity()->AI_getWorkersHave()));
	}

	if (eFeature != NO_FEATURE)
	{
		CvFeatureInfo& kFeatureInfo = GC.getFeatureInfo(eFeature);

		// Don't destroy worked plots with an improvement
		if (!pPlot->isBeingWorked() || pPlot->getImprovementType() == NO_IMPROVEMENT)
		{
			if (kOriginalBuildInfo.isFeatureRemove(eFeature))
			{
				if ((kOriginalBuildInfo.getImprovement() == NO_IMPROVEMENT) || (!pPlot->isBeingWorked() || (kFeatureInfo.getYieldChange(YIELD_FOOD)) <= 0))
				{
					bClearFeature = true;
				}
			}
		}

		if ((kFeatureInfo.getMovementCost() > 1) && (iWorkersNeeded > 1))
		{
			bBuildRoute = true;
		}
	}

	if (pPlot->getBonusType() != NO_BONUS)
	{
		bBuildRoute = true;
	}
	else if (pPlot->isHills())
	{
		if ((GLOBAL_DEFINE_HILLS_EXTRA_MOVEMENT > 0) && (iWorkersNeeded > 1))
		{
			bBuildRoute = true;
		}
	}

	if (pPlot->getRouteType() != NO_ROUTE)
	{
		bBuildRoute = false;
	}

	BuildTypes eBestBuild = NO_BUILD;
	int iBestValue = 0;
	for (int iBuild = 0; iBuild < GC.getNumBuildInfos(); iBuild++)
	{
		BuildTypes eBuild = ((BuildTypes)iBuild);
		CvBuildInfo& kBuildInfo = GC.getBuildInfo(eBuild);


		RouteTypes eRoute = (RouteTypes)kBuildInfo.getRoute();
		if ((bBuildRoute && (eRoute != NO_ROUTE)) || (bClearFeature && kBuildInfo.isFeatureRemove(eFeature)))
		{
			if (canBuild(pPlot, eBuild))
			{
				int iValue = 10000;

				if (bBuildRoute && (eRoute != NO_ROUTE))
				{
					iValue *= (1 + GC.getRouteInfo(eRoute).getValue());
					iValue /= 2;

					if (pPlot->getBonusType() != NO_BONUS)
					{
						iValue *= 2;
					}

					if (pPlot->getWorkingCity() != NULL)
					{
						iValue *= 2 + iWorkersNeeded + ((pPlot->isHills() && (iWorkersNeeded > 1)) ? 2 * GLOBAL_DEFINE_HILLS_EXTRA_MOVEMENT : 0);
						iValue /= 3;
					}
					ImprovementTypes eImprovement = (ImprovementTypes)kOriginalBuildInfo.getImprovement();
					if (eImprovement != NO_IMPROVEMENT)
					{
						int iRouteMultiplier = ((GC.getImprovementInfo(eImprovement).getRouteYieldChanges(eRoute, YIELD_FOOD)) * 100);
						iValue *= 100 + iRouteMultiplier;
						iValue /= 100;
					}
				}

				iValue /= (kBuildInfo.getTime() + 1);

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					eBestBuild = eBuild;
				}
			}
		}
	}

	if (eBestBuild == NO_BUILD)
	{
		return eBuild;
	}
	return eBestBuild;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_routeCity()
{
	PROFILE_FUNC();

	CvCity* pRouteToCity;
	CvCity* pLoopCity;
	int iLoop;

	FAssert(canBuildRoute());

	for (pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop))
	{
		if (AI_plotValid(pLoopCity->plot()))
		{
			pRouteToCity = pLoopCity->AI_getRouteToCity();

			if (pRouteToCity != NULL)
			{
				if (!(pLoopCity->plot()->isVisibleEnemyUnit(this)))
				{
					if (generatePath(pLoopCity->plot(), MOVE_SAFE_TERRITORY, true))
					{
						if (!(pRouteToCity->plot()->isVisibleEnemyUnit(this)))
						{
							if (GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pRouteToCity->plot(), MISSIONAI_BUILD, getGroup()) == 0)
							{
								if (generatePath(pRouteToCity->plot(), MOVE_SAFE_TERRITORY, true))
								{
									getGroup()->pushMission(MISSION_MOVE_TO, pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE(), MOVE_SAFE_TERRITORY, false, false, MISSIONAI_BUILD, pRouteToCity->plot());
									getGroup()->pushMission(MISSION_ROUTE_TO, pRouteToCity->getX_INLINE(), pRouteToCity->getY_INLINE(), MOVE_SAFE_TERRITORY, (getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pRouteToCity->plot());

									return true;
								}
							}
						}
					}
				}
			}
		}
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_routeTerritory(bool bImprovementOnly)
{
	PROFILE_FUNC();

	CvPlot* pLoopPlot;
	CvPlot* pBestPlot;
	ImprovementTypes eImprovement;
	RouteTypes eBestRoute;
	bool bValid;
	int iPathTurns;
	int iValue;
	int iBestValue;
	int iI, iJ;

	// XXX how do we make sure that we can build roads???

	FAssert(canBuildRoute());

	iBestValue = 0;
	pBestPlot = NULL;

	for (iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		pLoopPlot = GC.getMap().plotByIndexINLINE(iI);

		if (AI_plotValid(pLoopPlot))
		{
			if (pLoopPlot->getOwnerINLINE() == getOwnerINLINE()) // XXX team???
			{
				eBestRoute = GET_PLAYER(getOwnerINLINE()).getBestRoute(pLoopPlot);

				if (eBestRoute != NO_ROUTE)
				{
					if (eBestRoute != pLoopPlot->getRouteType())
					{
						if (bImprovementOnly)
						{
							bValid = false;

							eImprovement = pLoopPlot->getImprovementType();

							if (eImprovement != NO_IMPROVEMENT)
							{
								for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
								{
									if (GC.getImprovementInfo(eImprovement).getRouteYieldChanges(eBestRoute, iJ) > 0)
									{
										bValid = true;
										break;
									}
								}
							}
						}
						else
						{
							bValid = true;
						}

						if (bValid)
						{
							if (!(pLoopPlot->isVisibleEnemyUnit(this)))
							{
								if (GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BUILD, getGroup(), 1) == 0)
								{
									if (generatePath(pLoopPlot, MOVE_SAFE_TERRITORY, true, &iPathTurns))
									{
										iValue = 10000;

										iValue /= (iPathTurns + 1);

										if (iValue > iBestValue)
										{
											iBestValue = iValue;
											pBestPlot = pLoopPlot;
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

	if (pBestPlot != NULL)
	{
		getGroup()->pushMission(MISSION_ROUTE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_SAFE_TERRITORY, false, false, MISSIONAI_BUILD, pBestPlot);
		return true;
	}

	return false;
}


// Returns true if a mission was pushed...
bool CvUnitAI::AI_travelToUpgradeCity()
{
	// is there a city which can upgrade us?
	CvCity* pUpgradeCity = getUpgradeCity(/*bSearch*/ true);
	if (pUpgradeCity != NULL)
	{
		// cache some stuff
		CvPlot* pPlot = plot();
		bool bSeaUnit = (getDomainType() == DOMAIN_SEA);

		// if we at the upgrade city, stop, wait to get upgraded
		if (pUpgradeCity->plot() == pPlot)
		{
			getGroup()->pushMission(MISSION_SKIP);
			return true;
		}

		// find the closest city
		CvCity* pClosestCity = pPlot->getPlotCity();
		bool bAtClosestCity = (pClosestCity != NULL);
		if (pClosestCity == NULL)
		{
			pClosestCity = pPlot->getWorkingCity();
		}
		if (pClosestCity == NULL)
		{
			pClosestCity = GC.getMap().findCity(getX_INLINE(), getY_INLINE(), NO_PLAYER, getTeam(), true, bSeaUnit);
		}

		// can we path to the upgrade city?
		int iUpgradeCityPathTurns;
		CvPlot* pThisTurnPlot = NULL;
		bool bCanPathToUpgradeCity = generatePath(pUpgradeCity->plot(), 0, true, &iUpgradeCityPathTurns);
		if (bCanPathToUpgradeCity)
		{
			pThisTurnPlot = getPathEndTurnPlot();
		}

		// if we close to upgrade city, head there
		if (NULL != pThisTurnPlot && NULL != pClosestCity && (pClosestCity == pUpgradeCity || iUpgradeCityPathTurns < 4))
		{
			FAssert(!atPlot(pThisTurnPlot));
			getGroup()->pushMission(MISSION_MOVE_TO, pThisTurnPlot->getX_INLINE(), pThisTurnPlot->getY_INLINE());
			return true;
		}

		// path to the upgrade city
		if (NULL != pThisTurnPlot)
		{
			FAssert(!atPlot(pThisTurnPlot));
			getGroup()->pushMission(MISSION_MOVE_TO, pThisTurnPlot->getX_INLINE(), pThisTurnPlot->getY_INLINE());
			return true;
		}
	}

	return false;
}

// Returns true if a mission was pushed...
bool CvUnitAI::AI_retreatToCity(bool bPrimary, int iMaxPath)
{
	PROFILE_FUNC(); // (advc: iMaxPath mostly unused here; changing that could save time.)

	//int iCurrentDanger = GET_PLAYER(getOwner()).AI_getPlotDanger(plot());
	int const iCurrentDanger = (getGroup()->alwaysInvisible() ? 0 : // K-Mod
		GET_PLAYER(getOwner()).AI_getPlotDangerInternal(plot()));

	CvCityAI const* pCity = getPlot().AI_getPlotCity();
	if (iCurrentDanger <= 0 && pCity != NULL &&
		pCity->getOwner() == getOwner())
	{
		if (!bPrimary || GET_PLAYER(getOwner()).AI_isPrimaryArea(*pCity->area()))
		{
			//if (!bPrioritiseAirlift || pCity->getMaxAirlift() > 0)
			{	//if (!pCity->getPlot().isVisibleEnemyUnit(this)) {
				getGroup()->pushMission(MISSION_SKIP);
				return true;
			}
		}
	}
	//for (iPass = 0; iPass < 4; iPass++)
	/*  K-Mod. originally; pass 0 required the dest to have less plot danger
		unless the unit could fight;
		pass 1 was just an ordinary move;
		pass 2 was a 1 turn move with "ignore plot danger" and
		pass 3 was the full iMaxPath with ignore plot danger.
		I've changed it so that if the unit can fight, the pass 0 just skipped
		(because it's the same as the pass 1) and
		pass 2 is always skipped because it's a useless test.
		-- and I've renumbered the passes. */
	CvPlot* pBestPlot = NULL;
	int iShortestPath = MAX_INT;
	// <advc.139>
	//bool bEvac = (pCity != NULL && pCity->AI_isEvacuating());
	//bool bSafe = (pCity != NULL && pCity->AI_isSafe());
	// </advc.139>
	// <advc>
	int iPass = 0; // Used after the loop
	CvPlayerAI const& kOwner = GET_PLAYER(getOwner()); // </advc>
	for (iPass = ((getGroup()->canDefend() &&
		getDomainType() == DOMAIN_LAND) // advc.001s
		? 1 : 0); iPass < 3; iPass++)
	{
		//bool bNeedsAirlift = false;
		FOR_EACH_CITYAI(pLoopCity, kOwner)
		{
			if (!AI_plotValid(pLoopCity->plot()))
				continue;
			if (bPrimary && !kOwner.AI_isPrimaryArea(*pLoopCity->area()))
				continue;
			//if (bNeedsAirlift && pLoopCity->getMaxAirlift() == 0)
			//	continue;
			// <advc.139>
			/*  When evacuating, exclude other cities that also evacuate
				(and exclude the current city). */
			//if (bEvac && pLoopCity->AI_isEvacuating())
			//	continue;
			/*  Avoid path and danger computation if we already know that we're safer
				where we are. */
			if (/*!pLoopCity->AI_isSafe() && (bSafe || */iCurrentDanger <= 0 ||
				/*  Even when threatened at sea, a ship won't seek refuge in
					an unsafe city. */
				getDomainType() != DOMAIN_LAND)//)
			{
				continue;
			} // </advc.139>
			int iPathTurns = -1;
			if (generatePath(pLoopCity->plot(),
				iPass >= 2 ? MOVE_IGNORE_DANGER : NO_MOVEMENT_FLAGS, // was iPass >= 3
				true, &iPathTurns, iMaxPath))
			{/* (comment by jdog5000, 08/19/09)
				Water units can't defend a city
				Any unthreatened city acceptable on 0th pass, solves problem where sea units
				would oscillate in and out of threatened city because they had iCurrentDanger = 0
				on turns outside city */
				if (iPass > 0 || kOwner.AI_getPlotDangerInternal(pLoopCity->plot()) <= iCurrentDanger)
				{
					if (iPathTurns < iShortestPath)
					{
						iShortestPath = iPathTurns;
						pBestPlot = getPathEndTurnPlot();
					}
				}
			}
		}

		if (pBestPlot != NULL)
			break;

		if (getGroup()->alwaysInvisible())
			break;
	}

	if (pBestPlot != NULL)
	{
		if (at(*pBestPlot))
		{
			getGroup()->pushMission(MISSION_SKIP, -1, -1, NO_MOVEMENT_FLAGS,
				false, false, MISSIONAI_RETREAT);
		}
		else
		{
			pushGroupMoveTo(*pBestPlot,
				/*	was iPass >= 3
					advc (caveat): Flags here need to be consistent with those in the loop */
				iPass >= 2 ? MOVE_IGNORE_DANGER : NO_MOVEMENT_FLAGS,
				false, false, MISSIONAI_RETREAT);
		}
		return true;
	}

	if (pCity != NULL && pCity->getTeam() == getTeam())
	{
		if (/*bEvac && */at(*pCity->plot()) && // scorched earth
			// Let's not be a griefer if we'll be dead
			kOwner.getNumCities() > 1 &&
			m_pUnitInfo->getUnitCaptureClassType() != NO_UNITCLASS)
		{
			/*	(Would be nice to check which player is about to capture the city -
				e.g. Barbarians don't capture units - but that's not worth the
				implementation effort.) */
			//scaled rScrapOdds = per100(GC.getDefineINT(CvGlobals::
			//	BASE_UNIT_CAPTURE_CHANCE)) / 2;
			// Be more reluctant to scrap expensive units
			//rScrapOdds *= 50; // production cost baseline
			//rScrapOdds /= kOwner.getProductionNeeded(getUnitType()) /
			//	per100(GC.getInfo(GC.getGame().getGameSpeedType()).getTrainPercent());
			//if (SyncRandSuccess(rScrapOdds))
			//{
			//	scrap();
			//	return true;
			//}
		} // </advc.010>
		getGroup()->pushMission(MISSION_SKIP, -1, -1, NO_MOVEMENT_FLAGS,
			false, false, MISSIONAI_RETREAT);
		return true;
	}

	return false;
}

// TAC - AI Improved Naval AI - koma13 - START
bool CvUnitAI::AI_retreatFromDanger()
{
	const CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());

	if (kOwner.isHuman())
	{
		return false;
	}

	if (getUnitTravelState() != NO_UNIT_TRAVEL_STATE)
	{
		return false;
	}

	if (!isHurt())
	{
		int iWaterDanger = kOwner.AI_getWaterDanger(plot(), GC.getAI_TRANSPORT_DANGER_RANGE(), true, true, true);
		if (iWaterDanger == 0)
		{
			return false;
		}
	}

	if (AI_sailToEurope(false))
	{
		return true;
	}

	if (plot()->getPlotCity() != NULL)
	{
		getGroup()->pushMission(MISSION_SKIP);
		return true;
	}

	CvPlot* pLoopPlot;
	CvPlot* pBestPlot = NULL;

	int iBestValue = 0;
	int iMovesLeft = movesLeft() / GLOBAL_DEFINE_MOVE_DENOMINATOR;
	int iDX, iDY;

	MissionAITypes eMissionAI = getGroup()->AI_getMissionAIType();

	for (iDX = -(iMovesLeft); iDX <= iMovesLeft; iDX++)
	{
		for (iDY = -(iMovesLeft); iDY <= iMovesLeft; iDY++)
		{
			pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);
			if (pLoopPlot != NULL)
			{
				if (AI_plotValid(pLoopPlot) && !atPlot(pLoopPlot))
				{
					if (!(pLoopPlot->isVisibleEnemyUnit(this)))
					{
						int iPathTurns;
						if (generatePath(pLoopPlot, MOVE_IGNORE_DANGER, true, &iPathTurns))
						{
							if (iPathTurns <= 1)
							{
								int iValue = 0;
								int iPathLength = getGroup()->getPathLength();

								if (iPathLength < iMovesLeft)
								{
									if (canCrossOcean(pLoopPlot, UNIT_TRAVEL_STATE_TO_EUROPE))
									{
										iValue += (eMissionAI == MISSIONAI_SAIL_TO_EUROPE) ? 1000 : 500;
									}
									if (canCrossOcean(pLoopPlot, UNIT_TRAVEL_STATE_TO_AFRICA))
									{
										iValue += (eMissionAI == MISSIONAI_SAIL_TO_AFRICA) ? 1000 : 500;
									}
								}
								if (pLoopPlot->getPlotCity() != NULL)
								{
									iValue += (eMissionAI != MISSIONAI_SAIL_TO_EUROPE) ? 1000 : 500;
									iValue += (eMissionAI != MISSIONAI_SAIL_TO_AFRICA) ? 1000 : 500;
								}

								if (iValue > iBestValue)
								{
									iBestValue = iValue;
									pBestPlot = getPathEndTurnPlot();
								}
							}
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		if (pBestPlot->getEurope() != NO_EUROPE)
		{
			if (AI_sailToEurope())
			{
				return true;
			}
		}
			
		if (!atPlot(pBestPlot))
		{ 
			getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_IGNORE_DANGER, false, false, NO_MISSIONAI, pBestPlot);
			return true;
		}
	}

	return false;
}
// TAC - AI Improved Naval AI - koma13 - END

bool CvUnitAI::AI_treasureRetreat(int iMaxPathTurns)
{
	PROFILE_FUNC();


	int iBestValue = MAX_INT;
	CvPlot* pBestMissionPlot = NULL;
	CvPlot* pBestPlot = NULL;

	CvPlot* pStartingPlot = GET_PLAYER(getOwnerINLINE()).getStartingPlot();
	if (pStartingPlot == NULL || !pStartingPlot->isWater())
	{
		return false;
	}
	EuropeTypes eMainEurope = pStartingPlot->getNearestEurope();
	if (eMainEurope == NO_EUROPE)
	{
		return false;
	}

	for (int iPass = 0; iPass < 2; iPass++)
	{
		int iLoop;
		for (CvCity* pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop))
		{
			CvPlot* pLoopPlot = pLoopCity->plot();
			if (AI_plotValid(pLoopPlot))
			{
				if (!(pLoopPlot->isVisibleEnemyUnit(this)))
				{
					if (pLoopPlot->getNearestEurope() == eMainEurope)
					{
						int iPathTurns = 0;
						if (atPlot(pLoopPlot) || generatePath(pLoopPlot, ((iPass > 0) ? MOVE_IGNORE_DANGER : 0), true, &iPathTurns, iMaxPathTurns))
						{
							int iValue = 3 + pLoopPlot->getDistanceToOcean();

							iValue *= 3 + iPathTurns;

							if (iValue < iBestValue)
							{
								iBestValue = iValue;
								pBestPlot = atPlot(pLoopPlot) ? pLoopPlot : getPathEndTurnPlot();
								pBestMissionPlot = pLoopPlot;
							}
						}
					}
				}
			}
		}

		if (pBestPlot != NULL)
		{
			break;
		}

		if (getGroup()->alwaysInvisible())
		{
			break;
		}
	}

	if (pBestPlot != NULL)
	{
		if (atPlot(pBestPlot))
		{
			getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_AWAIT_PICKUP, pBestMissionPlot);
			return true;
		}
		else
		{
			getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), ((iPass > 0) ? MOVE_IGNORE_DANGER : 0), false, false, MISSIONAI_AWAIT_PICKUP, pBestMissionPlot);
			return true;
		}
	}

	return false;
}

// Returns true if a mission was pushed...
//koma13
//bool CvUnitAI::AI_pickup(UnitAITypes eUnitAI, int iMaxPathTurns)
bool CvUnitAI::AI_pickup(UnitAITypes eUnitAI, int iMaxPathTurns)
//end
{
	PROFILE_FUNC();

	CvCity* pCity;
	CvCity* pLoopCity;
	CvPlot* pBestPlot;
	CvPlot* pBestPickupPlot;
	int iPathTurns;
	int iValue;
	int iBestValue;
	int iLoop;

	FAssert(cargoSpace() > 0);
	if (0 == cargoSpace())
	{
		return false;
	}

	pCity = plot()->getPlotCity();

	if (pCity != NULL)
	{
		if (pCity->getOwnerINLINE() == getOwnerINLINE())
		{
			bool bAnyLoaded = false;
			int iCount = 0;

			CvPlot* pPlot = pCity->plot();
			CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();

			while (pUnitNode != NULL)
			{
				CvUnit* pLoopUnit = pPlot->getUnitNodeLoop(pUnitNode);

				if (pLoopUnit != NULL && pLoopUnit->AI_getUnitAIType() == eUnitAI)
				{
					if (cargoSpace() >= pLoopUnit->getUnitInfo().getRequiredTransportSize())
					{
						iCount++;
					}
				}
			}

			if (iCount > 0)
			{
				if (iCount > 0)
				{
					if (AI_loadUnits(eUnitAI, MISSIONAI_AWAIT_PICKUP))
					{
						return true;
					}
				}
				if ((AI_getUnitAIType() != UNITAI_ASSAULT_SEA) || pCity->AI_isDefended(-1))
				{
					getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_PICKUP, pCity->plot());
					return true;
				}
			}
		}
	}

	iBestValue = 0;
	pBestPlot = NULL;
	pBestPickupPlot = NULL;

	for (pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop))
	{
		if (AI_plotValid(pLoopCity->plot()))
		{

			if (!GET_PLAYER(getOwnerINLINE()).AI_unitAIIsCombat(eUnitAI) || pLoopCity->AI_isDefended(-1))
			{
				//int iCount = pLoopCity->plot()->plotCount(PUF_isUnitAIType, eUnitAI, -1, getOwnerINLINE());
				int iCount = 0;
				CvPlot* pPlot = pLoopCity->plot();
				CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();

				while (pUnitNode != NULL)
				{
					CvUnit* pLoopUnit = pPlot->getUnitNodeLoop(pUnitNode);

					if (pLoopUnit != NULL && pLoopUnit->AI_getUnitAIType() == eUnitAI)
					{
						if (cargoSpace() >= pLoopUnit->getUnitInfo().getRequiredTransportSize())
						{
							iCount++;
						}
					}
				}

				iValue = iCount * 10;

				if (pLoopCity->getProductionUnitAI() == eUnitAI)
				{
					CvUnitInfo& kUnitInfo = GC.getUnitInfo(pLoopCity->getProductionUnit());
					iValue++;
					iCount++;
				}

				if (iValue > 0)
				{
					iValue += pLoopCity->getPopulation();

					if (!(pLoopCity->plot()->isVisibleEnemyUnit(this)))
					{
						if (GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_PICKUP, getGroup()) < ((iCount + (cargoSpace() - 1)) / cargoSpace()))
						{
							if (!atPlot(pLoopCity->plot()) && generatePath(pLoopCity->plot(), 0, true, &iPathTurns, iMaxPathTurns))
							{
								//koma13
								iValue *= 1000;

								iValue /= (iPathTurns + 1);

								if (iValue > iBestValue)
								{
									iBestValue = iValue;
									pBestPlot = pLoopCity->plot();
									pBestPickupPlot = pLoopCity->plot();
								}
								//end
							}
						}
					}
				}
			}
		}
	}

	if ((pBestPlot != NULL) && (pBestPickupPlot != NULL))
	{
		FAssert(!atPlot(pBestPlot));
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), 0, false, false, MISSIONAI_PICKUP, pBestPickupPlot);
		return true;
	}

	return false;
}

bool CvUnitAI::AI_moveToStagingCity()
{
	CvCity* pLoopCity;
	CvPlot* pBestPlot;

	int iPathTurns;
	int iValue;
	int iBestValue;
	int iLoop;

	iBestValue = 0;
	pBestPlot = NULL;

	int iWarCount = 0;
	TeamTypes eTargetTeam = NO_TEAM;
	CvTeam& kTeam = GET_TEAM(getTeam());
	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if ((iI != getTeam()) && GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (kTeam.AI_isSneakAttackPreparing((TeamTypes)iI))
			{
				eTargetTeam = (TeamTypes)iI;
				iWarCount++;
			}
		}
	}
	if (iWarCount > 1)
	{
		eTargetTeam = NO_TEAM;
	}


	for (pLoopCity = GET_PLAYER(getOwner()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwner()).nextCity(&iLoop))
	{
		// TAC - AI Assault Sea - koma13, jdog5000(BBAI) - START
		// BBAI efficiency: check same area
		if ((pLoopCity->area() == area()) && AI_plotValid(pLoopCity->plot()))
		{
			// BBAI TODO: Need some knowledge of whether this is a good city to attack from ... only get that
			// indirectly from threat.
			iValue = pLoopCity->AI_cityThreat();

			// Have attack stacks in assault areas move to coastal cities for faster loading
			if( (area()->getAreaAIType(getTeam()) == AREAAI_ASSAULT) || (area()->getAreaAIType(getTeam()) == AREAAI_ASSAULT_MASSING) )
			{
				CvArea* pWaterArea = pLoopCity->waterArea();
				if( pWaterArea != NULL && GET_TEAM(getTeam()).AI_isWaterAreaRelevant(pWaterArea) )
				{
					// BBAI TODO:  Need a better way to determine which cities should serve as invasion launch locations

					// Inertia so units don't just chase transports around the map
					iValue = iValue/2;
					if( pLoopCity->area()->getAreaAIType(getTeam()) == AREAAI_ASSAULT )
					{
						// If in assault, transports may be at sea ... tend to stay where they left from
						// to speed reinforcement
						iValue += pLoopCity->plot()->plotCount(PUF_isAvailableUnitAITypeGroupie, UNITAI_OFFENSIVE, -1, getOwnerINLINE());
					}

					// Attraction to cities which are serving as launch/pickup points
					iValue += 3*pLoopCity->plot()->plotCount(PUF_isUnitAIType, UNITAI_ASSAULT_SEA, -1, getOwnerINLINE());
					iValue += 2*pLoopCity->plot()->plotCount(PUF_isUnitAIType, UNITAI_ESCORT_SEA, -1, getOwnerINLINE());
					iValue += 5*GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopCity->plot(), MISSIONAI_PICKUP);
				}
				else
				{
					iValue = iValue/8;
				}
			}

			if (iValue*200 > iBestValue)
			// TAC - AI Assault Sea - koma13, jdog5000(BBAI) - END
			{
				if (generatePath(pLoopCity->plot(), 0, true, &iPathTurns))
				{
					iValue *= 1000;
					iValue /= (5 + iPathTurns);
					if ((pLoopCity->plot() != plot()) && pLoopCity->isVisible(eTargetTeam, false))
					{
						iValue /= 2;
					}

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestPlot = getPathEndTurnPlot();
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		if (atPlot(pBestPlot))
		{
			getGroup()->pushMission(MISSION_SKIP);
			return true;
		}
		else
		{
			getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE());
			return true;
		}
	}

	return false;
}

bool CvUnitAI::AI_disembark(bool bEnemyCity)
{
	CvTeamAI& kTeam = GET_TEAM(getTeam());
	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;
	for (int i = 0; i < NUM_DIRECTION_TYPES; ++i)
	{
		CvPlot* pLoopPlot = plotDirection(getX_INLINE(), getY_INLINE(), (DirectionTypes)i);

		if ((pLoopPlot != NULL) && (!bEnemyCity || !pLoopPlot->isEnemyCity(*this)))
		{
			int iValue = 0;

			int iEnemyDistance = kTeam.AI_enemyCityDistance(pLoopPlot);

			if (iEnemyDistance != -1)
			{
				int iOdds = pLoopPlot->isVisibleEnemyDefender(this) ? AI_attackOdds(pLoopPlot, false) : 100;

				iValue = 10000 / std::max(1, iEnemyDistance);

				iValue *= iOdds;
				iValue /= 100;

				iValue += 1 + GC.getGameINLINE().getSorenRandNum(100, "AI disembark");
				if (generatePath(pLoopPlot, 0, true))
				{
					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestPlot = pLoopPlot;
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE());
		return true;
	}
	return false;
}

bool CvUnitAI::AI_imperialSeaAssault()
{
    int iBestValue = 0;
    CvPlot* pBestPlot = NULL;
    CvPlot* pBestAssaultPlot = NULL;

    CvTeamAI& kTeam = GET_TEAM(getTeam());

	CvPlot* pTargetPlot = NULL;

	if (GET_PLAYER(getOwnerINLINE()).AI_isStrategy(STRATEGY_CONCENTRATED_ATTACK))
	{
		pTargetPlot = GC.getMap().plotByIndexINLINE(GET_PLAYER(getOwnerINLINE()).AI_getStrategyData(STRATEGY_CONCENTRATED_ATTACK));
	}

    for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);

		if (!pLoopPlot->isWater() && !pLoopPlot->isImpassable())
		{
            int iWaterCount = 0;
            CvPlot* pCityPlot = NULL;
            int iBestCityValue = 0;

			int iValue = 0;

			int iCityDistance = kTeam.AI_enemyCityDistance(pLoopPlot);

			if ((iCityDistance != -1) && (iCityDistance < 8))
			{
				iValue = 8 - iCityDistance;

				if (iValue > 0)
				{
					iValue += 4;
					iValue *= 100;
					int iPathTurns = 0;
					if (generatePath(pLoopPlot, MOVE_THROUGH_ENEMY, true, &iPathTurns))
					{
						iValue *= 2 + GC.getGame().getSorenRandNum(50, "AI imperial sea assualt1") + GC.getGame().getSorenRandNum(50, "AI imperial sea assualt2");

						if (pTargetPlot != NULL)
						{
							iValue *= 3;
							iValue /= std::max(3, stepDistance(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), pTargetPlot->getX_INLINE(), pTargetPlot->getY_INLINE()));
						}

						iValue *= 100;
						iValue /= getPathCost() + baseMoves() * 350;

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							pBestAssaultPlot = pLoopPlot;
							pBestPlot = getGroup()->getPathSecondLastPlot();
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		if (atPlot(pBestPlot))
		{
			if (AI_wakeCargo(NO_UNITAI, AI_getMovePriority() + 1))
			{
				return true;
			}
			return false;
		}
		else
		{
			getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), 0, false, false, MISSIONAI_ASSAULT, pBestAssaultPlot);
			if (atPlot(pBestPlot))
			{
				AI_wakeCargo(NO_UNITAI, AI_getMovePriority() + 1);
			}
			return true;
		}
	}
	return false;
}


/*  Returns true if a mission was pushed
	-- or we should wait for another unit to bombard... */
bool CvUnitAI::AI_followBombard()
{
	if (canBombard(&getPlot()))
	{
		getGroup()->pushMission(MISSION_BOMBARD);
		return true;
	}

	// K-Mod note: I've disabled the following code because it seems like a timewaster with very little benefit.
	// The code checks if we are standing next to a city, and then checks if we have any other readyToMove group
	// next to the same city which can bombard... if so, return true.
	// I suppose the point of the code is to block our units from issuing a follow-attack order if we still have
	// some bombarding to do. -- But in my opinion, such checks, if we want them, should be done by the attack code.
	/*if (getDomainType() == DOMAIN_LAND) {
		for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++) {
			CvPlot* pAdjacentPlot1 = plotDirection(getX(), getY(), ((DirectionTypes)iI));
			if (pAdjacentPlot1 != NULL) {
				if (pAdjacentPlot1->isCity()) {
					if (AI_potentialEnemy(pAdjacentPlot1->getTeam(), pAdjacentPlot1)) {
						for (int iJ = 0; iJ < NUM_DIRECTION_TYPES; iJ++) {
							pAdjacentPlot2 = plotDirection(pAdjacentPlot1->getX(), pAdjacentPlot1->getY(), ((DirectionTypes)iJ));
							if (pAdjacentPlot2 != NULL) {
								CLLNode<IDInfo>* pUnitNode = pAdjacentPlot2->headUnitNode();
								while (pUnitNode != NULL) {
									pLoopUnit = ::getUnit(pUnitNode->m_data);
									pUnitNode = pAdjacentPlot2->nextUnitNode(pUnitNode);
									if (pLoopUnit->getOwner() == getOwner()) {
										if (pLoopUnit->canBombard(pAdjacentPlot2)) {
											if (pLoopUnit->isGroupHead()) {
												if (pLoopUnit->getGroup() != getGroup()) {
													if (pLoopUnit->getGroup()->readyToMove())
														return true;
	} } } } } } } } } } } }*/ // BtS

	return false;
}

// Returns true if the unit has found a potential enemy...
bool CvUnitAI::AI_potentialEnemy(TeamTypes eTeam, const CvPlot* pPlot)
{
	if (getGroup()->AI_isDeclareWar(pPlot))
	{
		return isPotentialEnemy(eTeam, pPlot);
	}
	else
	{
		return isEnemy(eTeam, pPlot);
	}
}


// Returns true if this plot needs some defense...
bool CvUnitAI::AI_defendPlot(CvPlot* pPlot)
{
	if (!canDefend(pPlot))
		return false;

	CvCityAI const* pCity = pPlot->AI_getPlotCity();
	if (pCity != NULL)
	{
		if (pCity->getOwner() == getOwner())
		{
			if (pCity->AI_isDanger())
				return true;
		}
	}
	else
	{
		if (/*getGroupSize() == 1 &&*/ // advc.010
			pPlot->plotCount(PUF_canDefendGroupHead, -1, -1, getOwnerINLINE(),
				// advc.001s: Want up to 1 defender per domain type
				NO_TEAM, PUF_isDomainType, getDomainType())
			<= (atPlot(pPlot) ? 1 : 0))
		{
			/*if (pPlot->plotCount(PUF_cannotDefend, -1, -1, getOwner()) > 0)
				return true*/
				// <advc.010>
			int iTotalProductionCost = 0;
			FOR_EACH_UNIT_ON(pUnit, pPlot)
			{
				if (pUnit->getOwner() == getOwner() && !pUnit->canDefend() &&
					/*  advc.001s: A land unit can defend non-land units in a Fort,
						but not vice versa. */
					(getDomainType() == DOMAIN_LAND ||
						pUnit->getDomainType() == getDomainType()))
				{
					if (pUnit->hasCargo() /*|| pUnit->isFound()*/)
						return true;
					/*
					int iProduction = pUnit->getUnitInfo().getProductionCost();
					if (iProduction <= 0) // special unit
						return true;
					iTotalProductionCost += iProduction;
					if (iTotalProductionCost * 5 > m_pUnitInfo->getProductionCost() * 3)
						return true;
					*/
				}
			} // </advc.010>
			/*if (pPlot->defenseModifier(getTeam(), false) >= 50 && pPlot->isRoute() && pPlot->getTeam() == getTeam())
				return true;*/ // (commented out by the BtS expansion)
		}
	}

	return false;
}


int CvUnitAI::AI_pillageValue(CvPlot* pPlot)
{
	FAssert(canPillage(pPlot) || (getGroup()->getCargo() > 0));

	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());
	if (!kOwner.isNative() && !(pPlot->isOwned()))
	{
		return 0;
	}

	// TAC - AI Keep neutral routes fix - koma13 - START
	if (pPlot->isRoute() && !pPlot->isOwned())
	{
		return 0;
	}
	// TAC - AI Keep neutral routes fix - koma13 - END

	int iValue = 0;

	if (pPlot->isRoute())
	{
		iValue++;

		for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			CvPlot* pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

			if (pAdjacentPlot != NULL && pAdjacentPlot->getTeam() == pPlot->getTeam())
			{
				if (pAdjacentPlot->isCity())
				{
					iValue += 10;
				}

				if (!(pAdjacentPlot->isRoute()))
				{
					if (!(pAdjacentPlot->isWater()) && !(pAdjacentPlot->isImpassable()))
					{
						iValue += 2;
					}
				}

				if ((pPlot->getCrumbs() > 0) && isNative() && isEnemy(pAdjacentPlot->getTeam()))
				{
					iValue += 5;
				}
			}
		}
	}

	ImprovementTypes eImprovement;
	if (pPlot->getImprovementDuration() > ((pPlot->isWater()) ? 20 : 5))
	{
		eImprovement = pPlot->getImprovementType();
	}
	else
	{
		eImprovement = pPlot->getRevealedImprovementType(getTeam(), false);
	}

	if (eImprovement != NO_IMPROVEMENT)
	{
		if (pPlot->getWorkingCity() != NULL)
		{
			iValue += (pPlot->calculateImprovementYieldChange(eImprovement, YIELD_FOOD, pPlot->getOwnerINLINE()) * 5);
		}

		iValue += GC.getImprovementInfo(eImprovement).getPillageGold();
	}

	iValue *= 100 + 2 * pPlot->defenseModifier(getTeam());
	iValue /= 100;

	return iValue;
}


int CvUnitAI::AI_searchRange(int iRange) const
{
	if (iRange == 0)
	{
		return 0;
	}

	if (flatMovementCost() || (getDomainType() == DOMAIN_SEA))
	{
		return (iRange * baseMoves());
	}
	else
	{
		return ((iRange + 1) * (baseMoves() + 1));
	}
}


// XXX at some point test the game with and without this function...
bool CvUnitAI::AI_plotValid(const CvPlot* pPlot) const
{
	PROFILE_FUNC();

	switch (getDomainType())
	{
	case DOMAIN_SEA:
		if (pPlot->isWater() || m_pUnitInfo->isCanMoveAllTerrain())
		{
			return true;
		}
		else if (pPlot->isFriendlyCity(*this, true) && pPlot->isCoastalLand())
		{
			return true;
		}
		break;

	case DOMAIN_LAND:
		//WTP, ray, Large Rivers - making sure that AI generally considers Large Rivers valid for Land Plots
		// if (pPlot->getArea() == getArea() || m_pUnitInfo->isCanMoveAllTerrain())
		if (pPlot->getArea(DOMAIN_LAND) == getArea() || m_pUnitInfo->isCanMoveAllTerrain())
		{
			return true;
		}
		break;

	case DOMAIN_IMMOBILE:
		FAssert(false);
		break;

	default:
		FAssert(false);
		break;
	}

	return false;
}


int CvUnitAI::AI_stackOfDoomExtra()
{
	// TAC - AI Attack City - koma13 - START
	// return ((AI_getBirthmark() % (1 + GET_PLAYER(getOwnerINLINE()).getCurrentEra())) + 4);
	int iStackOfDoom = (AI_getBirthmark() % (1 + GET_PLAYER(getOwnerINLINE()).getCurrentEra())) + 4;
	bool bReady = (GET_PLAYER(getOwnerINLINE()).AI_totalUnitAIs(UNITAI_OFFENSIVE) >= iStackOfDoom);

	return (bReady ? iStackOfDoom : 3);
	// TAC - AI Attack City - koma13 - END
}

bool CvUnitAI::AI_moveIntoCity(int iRange)
{
    PROFILE_FUNC();

	CvPlot* pLoopPlot;
	CvPlot* pBestPlot;
	int iSearchRange = iRange;
	int iPathTurns;
	int iValue;
	int iBestValue;
	int iDX, iDY;

	FAssert(canMove());

	iBestValue = 0;
	pBestPlot = NULL;

	if (plot()->isCity())
	{
	    return false;
	}

	iSearchRange = AI_searchRange(iRange);

	for (iDX = -(iSearchRange); iDX <= iSearchRange; iDX++)
	{
		for (iDY = -(iSearchRange); iDY <= iSearchRange; iDY++)
		{
			pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

			if (pLoopPlot != NULL)
			{
				if (AI_plotValid(pLoopPlot) && (!isEnemy(pLoopPlot->getTeam(), pLoopPlot)))
				{
					if (pLoopPlot->isCity() || (pLoopPlot->isCity(true)))
					{
						if (canMoveInto(*pLoopPlot, false) && (generatePath(pLoopPlot, 0, true, &iPathTurns) && (iPathTurns <= 1)))
						{
							iValue = 1;
							if (pLoopPlot->getPlotCity() != NULL)
							{
								iValue += pLoopPlot->getPlotCity()->getPopulation();
							}
							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestPlot = getPathEndTurnPlot();
								FAssert(!atPlot(pBestPlot));
							}
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		FAssert(!atPlot(pBestPlot));
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE());
		return true;
	}

	return false;
}

// R&R, ray, Natives raiding party - START
bool CvUnitAI::AI_nativeRaidTreasureUnit()
{
	PROFILE_FUNC();
	FAssert(canMove());

	CvPlot* pAdjacentPlot;
	CvPlot* pBestPlot;
	int iBestValue;

	iBestValue = 0;
	pBestPlot = NULL;

	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));
		if (pAdjacentPlot != NULL)
		{
			if (!atPlot(pAdjacentPlot) && AI_plotValid(pAdjacentPlot) && pAdjacentPlot->isVisible(getTeam(), false))
			{
				if (!pAdjacentPlot->isCity())
				{
					int iTreasureCount = 0;
					int iDefenderCount = 0;

					CvUnit* pTreasureUnit = NULL;
					CLLNode<IDInfo>* pUnitNode = pAdjacentPlot->headUnitNode();

					while (pUnitNode != NULL)
					{
						CvUnit* pLoopUnit = pAdjacentPlot->getUnitNodeLoop(pUnitNode);

						if (pLoopUnit != NULL && pLoopUnit->getTeam() != getTeam())
						{
							if (pLoopUnit->getUnitInfo().isTreasure())
							{
								iTreasureCount++;
								pTreasureUnit = pLoopUnit;
							}
							else
							{
								iDefenderCount++;
							}
						}
					}

					if (pTreasureUnit != NULL)
					{
						// R&R, ray, improvement to raiding treasures - START
						CvPlayerAI& raidOwner = GET_PLAYER(getOwner());
						PlayerTypes treasureOwner = pTreasureUnit->getOwner();

						// Natives do not raid treasures of players they like
						if(raidOwner.AI_getAttitude(treasureOwner) > GC.getLeaderHeadInfo(raidOwner.getPersonalityType()).getNativeRaidAttitudeThreshold())
						{
							return false;
						}

						if(!(GET_PLAYER(treasureOwner).isHuman()))
						{
							int spareAItreasureRand = GC.getGameINLINE().getSorenRandNum(100, "Native not raid AI treasure");
							int spareAItreasureChance = GC.getNATIVE_SPARE_AI_TREASURE_CHANCE();

							if (spareAItreasureChance > spareAItreasureRand)
							{
								return false;
							}
						}
						// R&R, ray, improvement to raiding treasures - END

						if (iDefenderCount == 0)
						{
							int iValue = pTreasureUnit->getYieldStored();
							if (iValue > 0)
							{
								if (iValue > iBestValue)
								{
									if (generatePath(pAdjacentPlot, 0, true))
									{
										iBestValue = iValue;
										pBestPlot = getPathEndTurnPlot();
										FAssert(!atPlot(pBestPlot));
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		FAssert(!atPlot(pBestPlot));

		if (AI_getUnitAIState() != UNITAI_STATE_RAIDING_PARTY)
		{
			AI_setUnitAIState(UNITAI_STATE_RAIDING_PARTY);
		}

		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_DIRECT_RAID);
		return true;
	}

	return false;
}
// R&R, ray, Natives raiding party - END

// R&R, ray, Monasteries and Forts - START
bool CvUnitAI::AI_nativeRaidFort()
{
	PROFILE_FUNC();
	FAssert(canMove());

	CvPlot* pAdjacentPlot;
	CvPlot* pBestPlot;

	pBestPlot = NULL;

	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));
		if (pAdjacentPlot != NULL)
		{
			if (!atPlot(pAdjacentPlot) && AI_plotValid(pAdjacentPlot) && pAdjacentPlot->isVisible(getTeam(), false))
			{
				if (pAdjacentPlot->isFort())
				{
					if (generatePath(pAdjacentPlot, 0, true))
					{
						pBestPlot = getPathEndTurnPlot();
						FAssert(!atPlot(pBestPlot));
					}
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		FAssert(!atPlot(pBestPlot));

		if (AI_getUnitAIState() != UNITAI_STATE_RAIDING_PARTY)
		{
			AI_setUnitAIState(UNITAI_STATE_RAIDING_PARTY);
		}

		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_DIRECT_RAID);
		return true;
	}

	return false;
}
// R&R, ray, Monasteries and Forts - END

bool CvUnitAI::AI_poach()
{
	CvPlot* pLoopPlot;
	int iX, iY;

	int iBestPoachValue = 0;
	CvPlot* pBestPoachPlot = NULL;
	TeamTypes eBestPoachTeam = NO_TEAM;

	if (!GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI))
	{
		return false;
	}

	if (GET_TEAM(getTeam()).getNumMembers() > 1)
	{
		return false;
	}

	int iNoPoachRoll = GET_PLAYER(getOwnerINLINE()).AI_totalUnitAIs(UNITAI_WORKER);
	iNoPoachRoll += GET_PLAYER(getOwnerINLINE()).getNumCities();
	iNoPoachRoll = std::max(0, (iNoPoachRoll - 1) / 2);
	if (GC.getGameINLINE().getSorenRandNum(iNoPoachRoll, "AI Poach") > 0)
	{
		return false;
	}

	if (GET_TEAM(getTeam()).getAnyWarPlanCount() > 0)
	{
		return false;
	}

	FAssert(canAttack());



	int iRange = 1;
	//Look for a unit which is non-combat
	//and has a capture unit type
	for (iX = -iRange; iX <= iRange; iX++)
	{
		for (iY = -iRange; iY <= iRange; iY++)
		{
			if (iX != 0 && iY != 0)
			{
				pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iX, iY);
				if ((pLoopPlot != NULL) && (pLoopPlot->getTeam() != getTeam()) && pLoopPlot->isVisible(getTeam(), false))
				{
					int iPoachCount = 0;
					int iDefenderCount = 0;
					CvUnit* pPoachUnit = NULL;
					CLLNode<IDInfo>* pUnitNode = pLoopPlot->headUnitNode();
					while (pUnitNode != NULL)
					{
						CvUnit* pLoopUnit = pLoopPlot->getUnitNodeLoop(pUnitNode);
						if (pLoopUnit != NULL
							&& pLoopUnit->getTeam() != getTeam()
							&& GET_TEAM(getTeam()).canDeclareWar(pLoopUnit->getTeam()))
						{
							if (!pLoopUnit->canDefend())
							{
								if (pLoopUnit->getCaptureUnitType(GET_PLAYER(getOwnerINLINE()).getCivilizationType()) != NO_UNIT)
								{
									iPoachCount++;
									pPoachUnit = pLoopUnit;
								}
							}
							else
							{
								iDefenderCount++;
							}
						}
					}

					if (pPoachUnit != NULL)
					{
						if (iDefenderCount == 0)
						{
							int iValue = iPoachCount * 100;
							iValue -= iNoPoachRoll * 25;
							if (iValue > iBestPoachValue)
							{
								iBestPoachValue = iValue;
								pBestPoachPlot = pLoopPlot;
								eBestPoachTeam = pPoachUnit->getTeam();
							}
						}
					}
				}
			}
		}
	}

	if (pBestPoachPlot != NULL)
	{
		//No war roll.
		if (!GET_TEAM(getTeam()).AI_performNoWarRolls(eBestPoachTeam))
		{
			GET_TEAM(getTeam()).declareWar(eBestPoachTeam, true, WARPLAN_LIMITED);

			FAssert(!atPlot(pBestPoachPlot));
			getGroup()->pushMission(MISSION_MOVE_TO, pBestPoachPlot->getX_INLINE(), pBestPoachPlot->getY_INLINE(), MOVE_DIRECT_ATTACK);
			return true;
		}

	}

	return false;
}

bool CvUnitAI::AI_choke(int iRange, bool bDefensive, MovementFlags eFlags)
{
	PROFILE_FUNC();

	int iPercentDefensive;
	int iDefCount = 0;

	FOR_EACH_UNIT_IN(pLoopUnit, *getGroup())
	{
		iDefCount += pLoopUnit->noDefensiveBonus() ? 0 : 1;

	}
	iPercentDefensive = 100 * iDefCount / getGroup()->getNumUnits();

	CvPlot* pBestPlot = 0;
	CvPlot* pEndTurnPlot = 0;
	int iBestValue = //0
		// advc.083: Don't park a big stack for little gain
		(bDefensive ? 0 : 6 * getGroup()->getNumUnits());
	// <advc.300> Don't use more than a couple of units for choking Barbarians
	//bool const bSmallGroup = (getGroup()->getNumUnits() <=
	//	GET_PLAYER(getOwner()).AI_getCurrEraFactor() + 1); // </advc.300>
	//for (SquareIter it(*this, iRange); it.hasNext(); ++it)
	FOR_EACH_PLOT_IN_RANGE_OF(plot(), iRange,
	{
		//CvPlot& p = *it;
		CvPlot& p = *pLoopPlot;

		if (!p.isOwned() || !isEnemy(p.getTeam()) || p.isVisibleEnemyUnit(this))
			continue;
		// <advc.300>
		//if (p.getOwner() == BARBARIAN_PLAYER && !bSmallGroup)
			continue; // </advc.300>

		int iPathTurns;
		if (p.getWorkingCity() == NULL ||
			!generatePath(&p, eFlags, true, &iPathTurns, iRange))
		{
			continue;
		}
		FAssert(p.getWorkingCity()->getTeam() == p.getTeam());
		//pLoopPlot->defenseModifier(getTeam(), false) // K-Mod
		int iValue = (bDefensive ? /* advc.012: */ AI_plotDefense(&p) - 15 : 0);
		/*
		if (p.getBonusType(getTeam()) != NO_BONUS)
		{
			iValue = GET_PLAYER(p.getOwner()).AI_bonusVal(p.getBonusType(), 0);
		}
		*/
		//iValue += p.getYield(YIELD_PRODUCTION) * 9; // was 10
		iValue += p.getYield(YIELD_FOOD) * 12; // was 10
		//iValue += p.getYield(YIELD_COMMERCE) * 5;

		if (at(p) && canPillage(&p))
			iValue += AI_pillageValue(&p) / (bDefensive ? 2 : 1);
		if (iValue <= 0)
			continue;

		iValue *= (bDefensive ? 25 : 50) + iPercentDefensive * pLoopPlot->defenseModifier(getTeam(), false) / 100;

		if (bDefensive)
		{
			// for defensive, we care a lot about path turns
			iValue *= 10;
			iValue /= std::max(1, iPathTurns);
		}
		else
		{
			// otherwise we just want to block as many tiles as possible
			iValue *= 10;
			iValue /= std::max(1, p.getNumDefenders(getOwner()) +
				(at(p) ? 0 : getGroup()->getNumUnits()));
		}
		if (iValue > iBestValue)
		{
			pBestPlot = &p;
			pEndTurnPlot = getPathEndTurnPlot();
			iBestValue = iValue;
		}
	}) // FOR_EACH
	if (pBestPlot == NULL)
		return false;

	FAssert(pBestPlot->getWorkingCity());
	CvPlot* pChokedCityPlot = pBestPlot->getWorkingCity()->plot();
	if (atPlot(pBestPlot))
	{
		FAssert(atPlot(pEndTurnPlot));
		if (canPillage(plot()))
		{
			getGroup()->pushMission(MISSION_PILLAGE, -1, -1,
				eFlags, false, false, MISSIONAI_CHOKE, pChokedCityPlot);
		}
		else
		{
			getGroup()->pushMission(MISSION_SKIP, -1, -1,
				eFlags, false, false, MISSIONAI_CHOKE, pChokedCityPlot);
		}
		return true;
	}
	pushGroupMoveTo(*pEndTurnPlot, eFlags, false, false,
		MISSIONAI_CHOKE, pChokedCityPlot);
	return true;
}

bool CvUnitAI::AI_solveBlockageProblem(CvPlot* pDestPlot, bool bDeclareWar)
{
	FAssert(pDestPlot != NULL);


	if (pDestPlot != NULL)
	{
		FAStarNode* pStepNode;

		CvPlot* pSourcePlot = plot();

		if (gDLL->getFAStarIFace()->GeneratePath(&GC.getStepFinder(), pSourcePlot->getX_INLINE(), pSourcePlot->getY_INLINE(), pDestPlot->getX_INLINE(), pDestPlot->getY_INLINE(), false, 0, true))
		{
			pStepNode = gDLL->getFAStarIFace()->GetLastNode(&GC.getStepFinder());

			while (pStepNode != NULL)
			{
				CvPlot* pStepPlot = GC.getMap().plotSoren(pStepNode->m_iX, pStepNode->m_iY);
				if (canMoveOrAttackInto(pStepPlot) && generatePath(pStepPlot, 0, true))
				{
					if (bDeclareWar && pStepNode->m_pPrev != NULL)
					{
						CvPlot* pPlot = GC.getMap().plotSoren(pStepNode->m_pPrev->m_iX, pStepNode->m_pPrev->m_iY);
						if (pPlot->getTeam() != NO_TEAM)
						{
							if (!canMoveInto(*pPlot, true, true))
							{
								if (!isPotentialEnemy(pPlot->getTeam(), pPlot))
								{
									CvTeamAI& kTeam = GET_TEAM(getTeam());
									if (kTeam.canDeclareWar(pPlot->getTeam()))
									{
										WarPlanTypes eWarPlan = WARPLAN_LIMITED;
										WarPlanTypes eExistingWarPlan = kTeam.AI_getWarPlan(pDestPlot->getTeam());
										if (eExistingWarPlan != NO_WARPLAN)
										{
											if ((eExistingWarPlan == WARPLAN_TOTAL) || (eExistingWarPlan == WARPLAN_PREPARING_TOTAL))
											{
												eWarPlan = WARPLAN_TOTAL;
											}

											if (!kTeam.isAtWar(pDestPlot->getTeam()))
											{
												kTeam.AI_setWarPlan(pDestPlot->getTeam(), NO_WARPLAN);
											}
										}
										kTeam.AI_setWarPlan(pPlot->getTeam(), eWarPlan, true);
										// TAC - AI Attack City - koma13, jdog5000(BBAI) - START
										//return (AI_targetCity());
										return (AI_goToTargetCity(static_cast<MovementFlags>(MOVE_AVOID_ENEMY_WEIGHT_2)));
										// TAC - AI Attack City - koma13, jdog5000(BBAI) - END
									}
								}
							}
						}
					}
					if (pStepPlot->isVisibleEnemyUnit(this))
					{
						FAssert(canAttack());
						CvPlot* pBestPlot = pStepPlot;
						//To prevent puppeteering attempt to barge through
						//if quite close
						if (getPathFinder().GetPathTurns() > 3)
						{
							pBestPlot = getPathEndTurnPlot();
						}

						FAssert(!atPlot(pBestPlot));
						getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_DIRECT_ATTACK);
						return true;
					}
				}
				pStepNode = pStepNode->m_pParent;
			}
		}
	}

	return false;
}

int CvUnitAI::AI_calculatePlotWorkersNeeded(CvPlot* pPlot, BuildTypes eBuild)
{
	int iBuildTime = pPlot->getBuildTime(eBuild) - pPlot->getBuildProgress(eBuild);
	int iWorkRate = workRate(true);

	if (iWorkRate <= 0)
	{
		FAssert(false);
		return 1;
	}
	int iTurns = iBuildTime / iWorkRate;

	if (iBuildTime > (iTurns * iWorkRate))
	{
		iTurns++;
	}

	int iNeeded = std::max(1, (iTurns + 2) / 3);

	if (pPlot->getBonusType() != NO_BONUS)
	{
		iNeeded *= 2;
	}
	return iNeeded;

}

bool CvUnitAI::AI_canGroupWithAIType(UnitAITypes eUnitAI) const
{
	if (eUnitAI != AI_getUnitAIType())
	{
		switch (eUnitAI)
		{
		case (UNITAI_ATTACK_CITY):
			if (getPlot().isCity() &&
				GC.getGame().getGameTurn()
				- getPlot().getPlotCity()->getGameTurnAcquired() <= 1)
			{
				return false;
			}
			break;
		}
	}
	return true;
}



bool CvUnitAI::AI_allowGroup(const CvUnit* pUnit, UnitAITypes eUnitAI) const
{
	CvSelectionGroup* pGroup = pUnit->getGroup();
	CvPlot* pPlot = pUnit->plot();

	if (pUnit == this)
	{
		return false;
	}

	if (!pUnit->isGroupHead())
	{
		return false;
	}

	if (pGroup == getGroup())
	{
		return false;
	}

	if (pUnit->isCargo())
	{
		return false;
	}

	if (pUnit->AI_getUnitAIType() != eUnitAI)
	{
		return false;
	}

	switch (pGroup->AI_getMissionAIType())
	{
	case MISSIONAI_GUARD_CITY:
		// do not join groups that are guarding cities
		// intentional fallthrough
	case MISSIONAI_LOAD_SETTLER:
	case MISSIONAI_LOAD_ASSAULT:
	case MISSIONAI_LOAD_SPECIAL:
		// do not join groups that are loading into transports (we might not fit and get stuck in loop forever)
		return false;
		break;
	default:
		break;
	}

	if (pGroup->getActivityType() == ACTIVITY_HEAL)
	{
		// do not attempt to join groups which are healing this turn
		// (healing is cleared every turn for automated groups, so we know we pushed a heal this turn)
		return false;
	}

	if (!canJoinGroup(pPlot, pGroup))
	{
		return false;
	}

	if (eUnitAI == UNITAI_COLONIST)
	{
		if (GET_PLAYER(getOwnerINLINE()).AI_getPlotDanger(pPlot, 3) > 0)
		{
			return false;
		}
	}
	else if (eUnitAI == UNITAI_ASSAULT_SEA)
	{
		if (!pGroup->hasCargo())
		{
			return false;
		}
	}

	if ((getGroup()->getHeadUnitAI() == UNITAI_DEFENSIVE))
	{
		if (plot()->isCity() && (plot()->getTeam() == getTeam()) && plot()->getBestDefender(getOwnerINLINE())->getGroup() == getGroup())
		{
			return false;
		}
	}

	if (plot()->getOwnerINLINE() == getOwnerINLINE())
	{
		CvPlot* pTargetPlot = pGroup->AI_getMissionAIPlot();

		if (pTargetPlot != NULL)
		{
			if (pTargetPlot->isOwned())
			{
				if (isPotentialEnemy(pTargetPlot->getTeam(), pTargetPlot))
				{
					//Do not join groups which have debarked on an offensive mission
					return false;
				}
			}
		}
	}

	if (pUnit->getInvisibleType() != NO_INVISIBLE)
	{
		if (getInvisibleType() == NO_INVISIBLE)
		{
			return false;
		}
	}

	return true;
}

bool CvUnitAI::AI_isOnMission()
{
	CvPlot* pMissionPlot = getGroup()->AI_getMissionAIPlot();
	if (pMissionPlot != NULL)
	{
		if (!atPlot(pMissionPlot))
		{
			return true;
		}
	}
	return false;
}

bool CvUnitAI::AI_isObsoleteTradeShip()
{
	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());

	int iLoop;
	CvUnit* pLoopUnit;
	for (pLoopUnit = kOwner.firstUnit(&iLoop); pLoopUnit; pLoopUnit = kOwner.nextUnit(&iLoop))
	{
		if (pLoopUnit->AI_getUnitAIType() == AI_getUnitAIType())
		{
			if (cargoSpace() < pLoopUnit->cargoSpace())
			{
				return true;
			}
			else if (cargoSpace() == pLoopUnit->cargoSpace())
			{
				if (getGameTurnCreated() > pLoopUnit->getGameTurnCreated())
				{
					return true;
				}
			}

		}
	}
	return false;
}

bool CvUnitAI::AI_loadAdjacent(CvPlot* pPlot, bool bTestCity)
{
	if (pPlot != NULL)
	{
		for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			CvPlot* pLoopPlot = plotDirection(pPlot->getX_INLINE(), pPlot->getY_INLINE(), ((DirectionTypes)iI));
			if (pLoopPlot != NULL)
			{
				if (canLoad(pLoopPlot, bTestCity))
				{
					getGroup()->pushMission(MISSION_MOVE_TO, pPlot->getX_INLINE(), pPlot->getY_INLINE());
					return true;
				}
			}
		}
	}
	return false;
}

bool CvUnitAI::AI_allowedToJoin(const CvCity* pCity) const
{
	if (!canJoinCity(pCity->plot()))
	{
		return false;
	}

	if (GC.getGame().getRemainingForcedPeaceTurns() == 0 && canAttack() && pCity->AI_neededDefenders() <= pCity->AI_numDefenders(false, true))
	{
		return false;
	}

	// Erik: Leaders and highly promoted units are better off as dedicated military units
	// and should not work inside a city
	if (isProfessionalMilitary())
	{
		return false;
	}

	if (GC.getGame().getRemainingForcedPeaceTurns() == 0)
	// TAC - AI Attack City - koma13 - START
	{
		if (AI_getUnitAIType() == UNITAI_OFFENSIVE)
		{
			return false;
		}
		//if (getExperiencePercent() > 0)
		//{
		//	return false;
		//}
	}
	// TAC - AI Attack City - koma13 - END


	return true;
}
// Private Functions...

//TAC Whaling, ray
bool CvUnitAI::AI_transportReturnToPort(bool bUnload, CvCity* pCity)
{
	if (AI_breakAutomation())
	{
		return false;
	}

	if (pCity == NULL)
	{
		if (GET_PLAYER(getOwnerINLINE()).getNumCities() == 0)
		{
			return false;
		}

		//koma13
		CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());
		CvCity* pBestCity = NULL;

		if (isWhalingBoat())
		{
			YieldTypes eYield = YIELD_WHALE_OIL;

			int iBestValue = MAX_INT;
			int iValue;
			int iLoop;

			for (CvCity* pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				if (AI_plotValid(pLoopCity->plot()))
				{
					if (pLoopCity->canProduceYield(eYield))
					{
						int iPathTurns;
						if (!atPlot(pLoopCity->plot()) && generatePath(pLoopCity->plot(), 0, true, &iPathTurns))
						{
							iValue = iPathTurns;
							if (iValue < iBestValue)
							{
								iBestValue = iValue;
								pBestCity = pLoopCity;
							}
						}
					}
				}
			}
		}

		else if (isFishingBoat())
		{
			int iBestValue = MAX_INT;
			int iValue;
			int iLoop;

			for (CvCity* pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				if (AI_plotValid(pLoopCity->plot()))
				{
					int iPathTurns;
					if (!atPlot(pLoopCity->plot()) && generatePath(pLoopCity->plot(), 0, true, &iPathTurns))
					{
						iValue = iPathTurns;
						if (iValue < iBestValue)
						{
							iBestValue = iValue;
							pBestCity = pLoopCity;
						}
					}

				}
			}
		}

		//koma13
		//pCity = (pBestCity != NULL ? pBestCity : kPlayer.AI_getPrimaryCity());
		pCity = (pBestCity != NULL ? pBestCity : kPlayer.AI_findBestPort());
	}
	else
	{
		if (plot() == pCity->plot())
		{
			if (bUnload)
			{
				unloadAll();
			}
			return false;
		}
	}

	if (AI_moveToCity(bUnload, pCity))
	{
		return true;
	}
	else
	{
		getGroup()->setAutomateType(NO_AUTOMATE);
		return false;
	}

	return false;
}

bool CvUnitAI::AI_moveToCity(bool bUnload, CvCity* pLoopCity)
{
	if (pLoopCity == NULL)
	{
		return false;
	}

	CvPlot* pBestPlot = NULL;
	int iPathTurns;
	int iValue;
	int iBestValue = MAX_INT;
	int iPass = 0;

	if (AI_plotValid(pLoopCity->plot()))
	{
		//koma13
		//if (GET_PLAYER(getOwnerINLINE()).AI_isPrimaryArea(pLoopCity->area()))
		{
			if (!(pLoopCity->plot()->isVisibleEnemyUnit(this)))
			{
				if (!atPlot(pLoopCity->plot()) && generatePath(pLoopCity->plot(), ((iPass > 1) ? MOVE_IGNORE_DANGER : 0), true, &iPathTurns))
				{
					if (iPathTurns <= MAX_INT)
					{
						iValue = iPathTurns;
						if (iValue < iBestValue)
						{
							iBestValue = iValue;
							pBestPlot = getPathEndTurnPlot();
						}
					}
				}
			}
		}
	}

	if (pBestPlot == NULL)
	{
		return false;
	}

	if (plot() == pLoopCity->plot())
	{
		if (bUnload)
		{
			unloadAll();
		}
	}

	if (!atPlot(pBestPlot))
	{
		getGroup()->pushMission(MISSION_MOVE_TO, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), MOVE_BUST_FOG, false, false, MISSIONAI_WORKER_SEA);
		return true;
	}

	return false;
}
//End TAC Whaling, ray

void CvUnitAI::AI_sellYieldUnits(Port port)
{
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	CvPlot* pPlot = plot();
	CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
	std::vector<CvUnit*> apUnits;

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = pPlot->getUnitNodeLoop(pUnitNode);

		if (pLoopUnit != NULL && pLoopUnit->getTransportUnit() == this)
		{
			if (pLoopUnit->isGoods() || pLoopUnit->getUnitInfo().isTreasure())
			{
				const YieldTypes eYield = pLoopUnit->getYield();

				if ((NO_YIELD == eYield) || kOwner.isYieldEuropeTradable(eYield))
				{
					apUnits.push_back(pLoopUnit);
				}
			}
		}
	}

	for (uint i = 0; i < apUnits.size(); ++i)
	{
		if (port == EUROPE)
		{
			kOwner.sellYieldUnitToEurope(apUnits[i], apUnits[i]->getYieldStored(), 0);
		}
		else if (port == AFRICA)
		{
			kOwner.sellYieldUnitToAfrica(apUnits[i], apUnits[i]->getYieldStored(), 0);
		}
	}
}

void CvUnitAI::AI_unloadUnits(Port port)
{
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	CvPlot* pPlot = plot();
	CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
	std::vector<CvUnit*> apUnits;

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = pPlot->getUnitNodeLoop(pUnitNode);

		if (pLoopUnit != NULL && pLoopUnit->getTransportUnit() == this)
		{
			if (!pLoopUnit->isGoods())
			{
				apUnits.push_back(pLoopUnit);
			}
		}
	}

	for (uint i = 0; i < apUnits.size(); ++i)
	{
		if (port == EUROPE)
		{
			kOwner.unloadUnitToEurope(apUnits[i]);
		}
		else if (port == AFRICA)
		{
			kOwner.unloadUnitToAfrica(apUnits[i]);
		}
	}
}

/*  advc.159: Like CvUnit::currEffectiveStr, but takes into account first strikes,
	collateral damage and that combat odds increase superlinearly with combat strength.
	The scale is arbitrary, i.e. one should only compare the returned values with each other. */
int CvUnitAI::AI_currEffectiveStr(CvPlot const* pPlot, CvUnit const* pOther,
	bool bCountCollateral, int iBaseCollateral, bool bCheckCanAttack,
	int iCurrentHP, bool bAssumePromotion) const // advc.139
{
	PROFILE_FUNC(); // Called frequently but not extremely so; fine as it is.
	int iCombatStrengthPercent = currEffectiveStr(pPlot, pOther, NULL, iCurrentHP);
	FAssertMsg(iCombatStrengthPercent > 0, "Non-combat unit?");

	// WTP: Not supported
#if 0
	/*  <K-Mod> (Moved from CvSelectionGroupAI::AI_sumStrength. Some of the code
		had been duplicated in CvPlayerAI::AI_localDefenceStrength, AI_localAttackStrength). */
		/*  first strikes are not counted in currEffectiveStr.
			actually, the value of first strikes is non-trivial to calculate...
			but we should do /something/ to take them into account. */
			/*  note. Most other parts of the code use 5% per first strike, but I figure
				we should go lower because this unit may get clobbered by collateral damage
				before fighting. */
				// (bCountCollateral means that we're the ones dealing collateral damage)
	int const iFirstStrikeMultiplier = (bCountCollateral ? 5 : 4);
	iCombatStrengthPercent *= 100 + iFirstStrikeMultiplier * firstStrikes() +
		(iFirstStrikeMultiplier / 2) * chanceFirstStrikes()
		+ (bAssumePromotion ? 7 : 0); // advc.139
	iCombatStrengthPercent /= 100;
	if (bCountCollateral && collateralDamage() > 0)
	{
		int iPossibleTargets = collateralDamageMaxUnits();
		if (bCheckCanAttack && pPlot != NULL && pPlot->isVisible(getTeam()))
		{
			iPossibleTargets = std::min(iPossibleTargets,
				pPlot->getNumVisibleEnemyDefenders(this) - 1);
		}
		// If !bCheckCanAttack, then lets not assume kPlot won't get more units on it.
		// advc: But let's put some cap on the number of targets
		else iPossibleTargets = std::min(10, iPossibleTargets);
		if (iPossibleTargets > 0)
		{
			/*	collateral damage is not trivial to calculate. This estimate is pretty rough.
				(Note: collateralDamage() and iBaseCollateral both include factors of 100.) */
			iCombatStrengthPercent += baseCombatStr() * iBaseCollateral *
				collateralDamage() * iPossibleTargets / 10000;
		}
	} // </K-Mod>
#endif

	FAssert(iCombatStrengthPercent < 100000); // A conservative guard against overflow
	/*  Generally, there's a good chance that a strong unit can kill a weak unit
		and then heal ("defeat in detail"). However, this function is used for
		evaluating imminent stack-on-stack combat. When we already know that
		a stack of stronger units is facing a larger stack of weaker units
		we need to assume a high chance of having to fight multiple enemies in a row. */
#if 0
	static scaled const rExponent = scaled::max(1,
		fixp(3 / 4.) * per100(GC.getDefineINT(CvGlobals::POWER_CORRECTION)));
	static scaled const rNormalizationFactor = (rExponent < fixp(1.05) ? 1 :
		// Pretty arbitrary; only need to weigh rounding errors against the danger of overflow.
		25000 / scaled(10000).pow(rExponent));
	/*  Make the AI overestimate weak units a little bit on the low and medium difficulty settings.
		(Not static b/c difficulty can change through load/ new game.) */
	scaled rExponentAdjusted = rExponent - scaled(std::max(0,
		50 - GC.getInfo(GC.getGame().getHandicapType()).getDifficulty()), 250);
	int iR = std::min(25000, // Guard against overflow problems for caller
		(scaled(iCombatStrengthPercent).pow(rExponentAdjusted) *
			rNormalizationFactor).round());
	return std::max(1, iR); // Don't round down to 0
#endif
	return iCombatStrengthPercent;
}

/*	advc.003u: Extracted from K-Mod's AI_getWeightedOdds, which I've moved to CvSelectionGroupAI.
	Adjusts the combat odds based on opportunity. */
int CvUnitAI::AI_opportuneOdds(int iActualOdds, CvUnit const& kDefender) const
{
	FAssert(canAttack());

	int const iOdds = iActualOdds; // abbreviate
	int r = iOdds;

	// WTP: Use the Europe price as proxy for unit cost if available. TODO: If we can
	//  produce the yields for the unit ourselves domestically, use that instead
	// adjust the values based on the relative production cost of the units.
	{
		int iOurCost = getUnitInfo().getEuropeCost();
		int iTheirCost = kDefender.getUnitInfo().getEuropeCost();
		if (iOurCost > 0 && iTheirCost > 0)
		{
			if (iOurCost > 0 && iTheirCost > 0 && iOurCost != iTheirCost)
			{
				//r += iOdds * (100 - iOdds) * 2 * iTheirCost / (iOurCost + iTheirCost) / 100;
				//r -= iOdds * (100 - iOdds) * 2 * iOurCost / (iOurCost + iTheirCost) / 100;
				int x = iOdds * (100 - iOdds) * 2 / (iOurCost + iTheirCost + 20);
				r += x * (iTheirCost - iOurCost) / 100;
			}
		}
	}

	CvPlot const& kDefenderPlot = *kDefender.plot();

	// adjust down if the enemy is on a defensive tile - we'd prefer to attack them on open ground.
	if (!kDefender.noDefensiveBonus())
	{
		r -= (100 - iOdds) * kDefenderPlot.defenseModifier(kDefender.getTeam(), false/*,
			getTeam()*/) // advc.012 WTP: Non-owners get the full defensive bonus
			/ (getDomainType() == DOMAIN_SEA ? 100 : 300);
	}

	// adjust the odds up if the enemy is wounded. We want to attack them now before they heal.
	r += iOdds * (100 - iOdds) * kDefender.getDamage() / (100 * kDefender.maxHitPoints());
	// adjust the odds down if our attacker is wounded - but only if healing is viable.
	if (isHurt() && healRate(&kDefenderPlot) > 10)
		r -= iOdds * (100 - iOdds) * getDamage() / (100 * maxHitPoints());

	// We're extra keen to take cites when we can...
	if (kDefenderPlot.isCity() && AI_countEnemyDefenders(kDefenderPlot) == 1)
		r += (100 - iOdds) / 3;

	// TODO: Adjust for survival odds

	return r;
}

/*	advc: Renamed from AI_potentialEnemy. Says whether this unit is allowed to
	attack units owned by eTeam, starting a war if necessary. */
bool CvUnitAI::AI_mayAttack(TeamTypes eTeam, CvPlot const& kPlot) const
{
	PROFILE_FUNC();

	if (isEnemy(eTeam, &kPlot))
		return true;
	if (AI_getGroup()->AI_isDeclareWarInternal(&kPlot))
	{
		return GET_TEAM(getTeam()).AI_mayAttack(eTeam); // advc
	}
	return false;
}

/*	advc: Replaces CvUnit::potentialWarAction, which did almost the same thing as
	CvUnitAI::AI_potentialEnemy (see above). I'm including the BtS implementation
	as a comment. */
bool CvUnitAI::AI_mayAttack(CvPlot const& kPlot) const
{
	if (!kPlot.isOwned())
		return false;
	return AI_mayAttack(kPlot.getTeam(), kPlot);
	// "returns true if unit can initiate a war action with plot (possibly by declaring war)"
	/*if (!kPlot.isOwned())
		return false;
	if (isEnemy(kPlot))
		return true;
	if (AI_getGroup()->AI_isDeclareWar(&kPlot) &&
		GET_TEAM(getTeam()).AI_getWarPlan(kPlot.getTeam()) != NO_WARPLAN)
	{
		return true;
	}
	return false;*/
}

/*	advc: Moved from CvUnit b/c this function eventually checks
	the war plans of the unit owner. Renamed from "isPotentialEnemy".
	The caller wants to know if this unit can currently be attacked
	in kPlot by units of eTeam or if it could soon be attacked (war imminent). */
bool CvUnitAI::AI_isPotentialEnemyOf(TeamTypes eTeam, CvPlot const& kPlot) const
{
	/*	This can be the BARBARIAN_TEAM is this unit isAlwaysHostile.
		Normally, it's the unit owner. */
	TeamTypes eCombatTeam = TEAMID(getCombatOwner(eTeam, kPlot));
	return GET_TEAM(eCombatTeam).AI_mayAttack(eTeam);
}

/*	advc: Moved from CvPlot::getNumVisiblePotentialEnemyDefenders.
	Counts current enemies of this unit and those on whom war is imminent. */
int CvUnitAI::AI_countEnemyDefenders(CvPlot const& kPlot) const
{
	return kPlot.plotCount(PUF_canDefendPotentialEnemy, getOwner(), isAlwaysHostile(&kPlot),
		NO_PLAYER, NO_TEAM, PUF_isVisible, getOwner());
}
// advc.opt: (plotCheck, apart from that, copy-pasted from above)
bool CvUnitAI::AI_isAnyEnemyDefender(CvPlot const& kPlot) const
{
	return (kPlot.plotCheck(PUF_canDefendPotentialEnemy, getOwner(), isAlwaysHostile(&kPlot),
		NO_PLAYER, NO_TEAM, PUF_isVisible, getOwner()) != NULL);
}

// <advc.003u>, advc.003s
CvSelectionGroupAI const* CvUnitAI::AI_getGroup() const
{
	return static_cast<const CvSelectionGroupAI*>(getGroup());
	//return GET_PLAYER(getOwner()).AI_getSelectionGroup(getGroupID());
}

CvSelectionGroupAI* CvUnitAI::AI_getGroup()
{
	return static_cast<CvSelectionGroupAI*>(getGroup());
	//return GET_PLAYER(getOwner()).AI_getSelectionGroup(getGroupID());
} // </advc.003u>

// K-Mod.
/*	bLocal is just to help with the efficiency of this function for short-range checks.
	It means that we should look only in nearby plots.
	the default (bLocal == false) is to look at every plot on the map! */
bool CvUnitAI::AI_defendTerritory(int iThreshold, MovementFlags eFlags,
	int iMaxPathTurns, bool bLocal)
{
	PROFILE_FUNC();

	const CvPlayerAI& kOwner = GET_PLAYER(getOwner());

	CvPlot* pEndTurnPlot = NULL;
	int iBestValue = 0;

	//for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	// I'm going to use a loop equivalent to the above when !bLocal; and a loop in a square around our unit if bLocal.
	int i = 0;
	int iRange = bLocal ? AI_searchRange(iMaxPathTurns) : 0;
	int iPlots = bLocal ? (2 * iRange + 1) * (2 * iRange + 1) : GC.getMap().numPlots();
	if (bLocal && iPlots >= GC.getMap().numPlots())
	{
		bLocal = false;
		iRange = 0;
		iPlots = GC.getMap().numPlots();
		// otherwise it's just silly.
	}
	FAssert(!bLocal || iRange > 0);
	TeamTypes const eTeam = getTeam(); // advc.opt
	while (i < iPlots)
	{
		CvPlot* pLoopPlot = (bLocal
			? plotXY(getX(), getY(), -iRange + i % (2 * iRange + 1), -iRange + i / (2 * iRange + 1))
			: GC.getMap().plotByIndex(i));
		i++; // for next cycle.

		if (pLoopPlot == NULL || pLoopPlot->getTeam() != eTeam ||
			!AI_plotValid(pLoopPlot) || !pLoopPlot->isVisibleEnemyUnit(this))
		{
			continue;
		}
		// <advc.033>
		/*
		if (isAlwaysHostile(pLoopPlot) && !AI_isAnyPiracyTarget(*pLoopPlot))
			continue;
		*/
		/*  This doesn't guarantee that the best defender will be a PiracyTarget,
			but at least we're going to attack a unit that is hanging out
			with a target. */ // </advc.033>
		int iPathTurns;
		if (generatePath(pLoopPlot, eFlags, true, &iPathTurns, iMaxPathTurns))
		{
			int iOdds = AI_getGroup()->AI_getWeightedOdds(pLoopPlot);
			int iValue = iOdds;
			if (iOdds > 0 && iOdds < 100 && iThreshold > 0)
			{
				int iOurAttack = kOwner.AI_localAttackStrength(pLoopPlot, eTeam,
					getDomainType(), 2, true, true, true);
				int iEnemyDefence = kOwner.AI_localDefenceStrength(pLoopPlot, NO_TEAM,
					getDomainType(), 0);
				if (iOurAttack > iEnemyDefence && iEnemyDefence > 0)
				{
					/*int iBonus = 100 - iOdds;
					iBonus -= iBonus * 4*iBonus / (4*iBonus + 100*(iOurAttack-iEnemyDefence)/iEnemyDefence);
					FAssert(iBonus >= 0 && iBonus <= 100 - iOdds);
					iValue += iBonus;*/
					iValue += 100 * (iOdds + 15) * (iOurAttack - iEnemyDefence) /
						((iThreshold + 100) * iEnemyDefence);
				}
			}
			if (iValue >= iThreshold)
			{
				//BonusTypes eBonus = pLoopPlot->getNonObsoleteBonusType(eTeam);
				iValue *= 100 /* + (eBonus == NO_BONUS ? 0 :
					3 * kOwner.AI_bonusVal(eBonus, 0) / 2)*/ +
					(pLoopPlot->getWorkingCity() ? 20 : 0);

				if (pLoopPlot->getOwner() != getOwner())
					iValue = 2 * iValue / 3;

				if (iPathTurns > 1)
					iValue /= iPathTurns + 2;

				if (iOdds >= iThreshold)
					iValue = 4 * iValue / 3;

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pEndTurnPlot = getPathEndTurnPlot();
				}
			}
		}
	}

	if (pEndTurnPlot != NULL)
	{
		pushGroupMoveTo(*pEndTurnPlot, eFlags, false, false, MISSIONAI_DEFEND);
		return true;
	}

	return false;
}

/*	iAttackThreshold is the minimum ratio for
	our attack / their defence.
	iRiskThreshold is the minimum ratio for
	their attack / our defence adjusted for stack size
	note: iSearchRange is /not/ the number of turns.
	It is the number of steps. iSearchRange < 1 means 'automatic'
	Only 1-turn moves are considered here. */
bool CvUnitAI::AI_stackVsStack(int iSearchRange, int iAttackThreshold, int iRiskThreshold,
	MovementFlags eFlags)
{
	PROFILE_FUNC();

	CvPlayerAI const& kOwner = GET_PLAYER(getOwner());

	//int iOurDefence = kOwner.AI_localDefenceStrength(plot(), getTeam());
	int const iOurDefence = AI_getGroup()->AI_sumStrengthInternal(NULL); // not counting defensive bonuses

	CvPlot* pBestPlot = NULL;
	int iBestValue = 0;
	//for (SquareIter it(*this, iSearchRange < 1 ? AI_searchRange(1) : iSearchRange, false);
	//	it.hasNext(); ++it)
	FOR_EACH_NON_CENTER_PLOT_IN_RANGE_OF(plot(), iSearchRange < 1 ? AI_searchRange(1) : iSearchRange,
	{
		CvPlot& p = *pLoopPlot;
		if (!AI_plotValid(&p))
			continue;
		int iEnemies = p.getNumVisibleEnemyDefenders(this);
		int iPathTurns;
		if (iEnemies > 0 && generatePath(&p, eFlags, true, &iPathTurns, 1))
		{
			int iEnemyAttack = kOwner.AI_localAttackStrength(&p, NO_TEAM,
				getDomainType(), 0, false);
			int iRiskRatio = 100 * iEnemyAttack / std::max(1, iOurDefence);
			// adjust risk ratio based on the relative numbers of units.
			iRiskRatio *= 50 + 50 * (getGroup()->getNumUnits() + 3) /
				std::min(iEnemies + 3, getGroup()->getNumUnits() + 3);
			iRiskRatio /= 100;
			//
			if (iRiskRatio < iRiskThreshold)
				continue;

			int iAttackRatio = AI_getGroup()->AI_compareStacks(&p, true);
			if (iAttackRatio < iAttackThreshold)
				continue;

			int iValue = iAttackRatio * iRiskRatio;
			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestPlot = &p;
				FAssert(pBestPlot == getPathEndTurnPlot());
			}
		}
	}) // FOR_EACH

	if (pBestPlot != NULL)
	{
		if (gUnitLogLevel >= 2) logBBAI("    Stack for player %d (%S) uses StackVsStack attack with value %d", getOwner(), GET_PLAYER(getOwner()).getCivilizationDescription(0), iBestValue);
		pushGroupMoveTo(*pBestPlot, eFlags, false, false,
			MISSIONAI_COUNTER_ATTACK, pBestPlot);
		return true;
	}

	return false;
} // K-Mod end

// K-Mod. One group function to rule them all.
bool CvUnitAI::AI_omniGroup(UnitAITypes eUnitAI, int iMaxGroup, int iMaxOwnUnitAI,
	bool bStackOfDoom, int iFlags, int iMaxPath, bool bMergeGroups, bool bSafeOnly,
	bool bIgnoreFaster, bool bIgnoreOwnUnitType, bool bBiggerOnly, int iMinUnitAI,
	bool bWithCargoOnly, bool bIgnoreBusyTransports)
{
	PROFILE_FUNC();

	iFlags &= ~MOVE_DECLARE_WAR; // Don't consider war when we just want to group

	if (isCargo())
		return false;

	if (!AI_canGroupWithAIType(eUnitAI))
		return false;

	if (getDomainType() == DOMAIN_LAND && /*!canMoveAllTerrain() &&*/
		area()->getNumAIUnits(getOwner(), eUnitAI) <= 0)
	{
		return false;
	}
	CvPlayerAI const& kOwner = GET_PLAYER(getOwner());
	/*	<advc.057> Except for assault groups, the head unit should have the
		most restrictive impassable types. */
	int const iOurGroupFirstVal = AI_groupFirstValInternal();
	
	uint uiOurMaxImpassables = kOwner.AI_unitImpassables(getUnitType());
	if (AI_getUnitAIType() == UNITAI_ASSAULT_SEA)
	{
		for (CLLNode<IDInfo> const* pUnitNode = // We're the head; already done.
			getGroup()->nextUnitNode(getGroup()->headUnitNode()); // </advc.057>
			pUnitNode != NULL; pUnitNode = getGroup()->nextUnitNode(pUnitNode))
		{
			CvUnit const& kImpassUnit = *::getUnit(pUnitNode->m_data);
			uiOurMaxImpassables = std::max(uiOurMaxImpassables,
				kOwner.AI_unitImpassables(kImpassUnit.getUnitType()));
		}
	}
	
	CvUnit* pBestUnit = NULL;
	int iBestValue = MAX_INT;
	FOR_EACH_GROUPAI_VAR(pLoopGroup, kOwner)
	{
		CvUnitAI* pLoopUnit = pLoopGroup->AI_getHeadUnit();
		if (pLoopUnit == NULL)
			continue;
		CvPlot const& kLoopPlot = pLoopUnit->getPlot();
		if (!AI_plotValid(&kLoopPlot))
			continue;
		if (iMaxPath == 0 && !atPlot(&kLoopPlot)) // advc.opt (tbd.): Should arguably treat iMaxPath==0 upfront
			continue;
		/*if (getDomainType() != DOMAIN_LAND || canMoveAllTerrain() ||
			kPlot.isArea(getArea())) {*/ // advc.opt: Redundant after AI_plotValid
		if (!AI_allowGroup(pLoopUnit, eUnitAI))
			continue;

		// K-Mod. I've restructed this wad of conditions so that it is easier for me to read. // advc: Made a few more edits - parts of it were still off-screen ...
		/*	((removed ((heaps) of parentheses) (etc)).)
			also, I've rearranged the order to be slightly faster for failed checks.
			Note: the iMaxGroups & OwnUnitAI check is apparently off-by-one.
			This is for backwards compatibility for the original code. */
		if ((!bSafeOnly || !isEnemy(kLoopPlot))
			&&
			(!bWithCargoOnly || pLoopUnit->getGroup()->hasCargo())
			&&
			(!bBiggerOnly || !bMergeGroups ||
				pLoopGroup->getNumUnits() >= getGroup()->getNumUnits())
			&&
			(!bIgnoreFaster || pLoopGroup->baseMoves() <= baseMoves())
			&&
			(!bIgnoreOwnUnitType || pLoopUnit->getUnitType() != getUnitType())
			&&
			(!bIgnoreBusyTransports || !pLoopGroup->hasCargo() ||
				(pLoopGroup->AI_getMissionAIType() != MISSIONAI_ASSAULT &&
					pLoopGroup->AI_getMissionAIType() != MISSIONAI_REINFORCE))
			&&
			(iMinUnitAI == -1 || pLoopGroup->countNumUnitAIType(eUnitAI) >= iMinUnitAI)
			&&
			(iMaxOwnUnitAI == -1 ||
				(bMergeGroups ? std::max(0, getGroup()->countNumUnitAIType(AI_getUnitAIType()) - 1) : 0) +
				pLoopGroup->countNumUnitAIType(AI_getUnitAIType()) <=
				iMaxOwnUnitAI + (bStackOfDoom ? AI_stackOfDoomExtra() : 0))
			&&
			(iMaxGroup == -1 || (bMergeGroups ? getGroup()->getNumUnits() - 1 : 0) +
				pLoopGroup->getNumUnits() +
				kOwner.AI_unitTargetMissionAIsInternal(pLoopUnit, MISSIONAI_GROUP, getGroup()) <=
				iMaxGroup + (bStackOfDoom ? AI_stackOfDoomExtra() : 0))
			&&
			(pLoopGroup->AI_getMissionAIType() != MISSIONAI_GUARD_CITY ||
				!pLoopGroup->getPlot().isCity() ||
				pLoopGroup->getPlot().plotCount(PUF_isMissionAIType, MISSIONAI_GUARD_CITY, -1, getOwner()) >
				pLoopGroup->getPlot().getPlotCity()->AI().AI_minDefenders())
			)
		{
			FAssert(!kLoopPlot.isVisibleEnemyUnit(this));
			#ifdef FASSERT_ENABLE
			if (kLoopPlot.isVisibleEnemyUnit(this))
			{
				if (this != NULL)
				{ 
					CvUnit& kDefender = *kLoopPlot.getVisibleEnemyDefender(this->getOwnerINLINE());

					logBBAI("	WARNING CvUnitAI::AI_omniGroup Unit %d %S %S on Plot [%d, %d], visible enemy: %d %S kLoopPlot [%d, %d]",
						getID(), getName().GetCString(), GET_PLAYER(pLoopUnit->getOwnerINLINE()).getName(),
						getX_INLINE(), getY_INLINE(), kDefender.getID(), kDefender.getName().GetCString(), 
						pLoopUnit->getX_INLINE(), pLoopUnit->getY_INLINE());
				}
				else
				{
					FErrorMsg("this is NULL!");
				}
			}
			#endif
			//if (iOurMaxImpassableCount > 0 || AI_getUnitAIType() == UNITAI_ASSAULT_SEA) { ...
			{	// <advc.057> Check their impassable count even if ours is 0
				CLLNode<IDInfo> const* pUnitNode = pLoopGroup->headUnitNode();
				CvUnitAI const& kHeadUnit = (::getUnit(pUnitNode->m_data))->AI();
				uint uiTheirMaxImpassables = kOwner.AI_unitImpassables(
					kHeadUnit.getUnitType());
				/*	Assault groups aren't always formed through this function;
					can't rely on head having the most impassable types. */
				if (kHeadUnit.AI_getUnitAIType() == UNITAI_ASSAULT_SEA)
				{
					for (pUnitNode = getGroup()->nextUnitNode(pUnitNode);
						pUnitNode != NULL; pUnitNode = getGroup()->nextUnitNode(pUnitNode))
					{
						CvUnit const& kUnit = *::getUnit(pUnitNode->m_data);
						uiTheirMaxImpassables = std::max(uiTheirMaxImpassables,
							kOwner.AI_unitImpassables(kUnit.getUnitType()));
					}
				}
				int const iTheirGroupFirstValue = kHeadUnit.AI_groupFirstValInternal();
				/*	Disallow the group if we can't rule out that the impassable count
					of the head will decrease. (Should really check for set inclusion,
					i.e. the set of impassables of the leader needs to include all
					impassables of the group.) */
				if ((iTheirGroupFirstValue >= iOurGroupFirstVal &&
					uiTheirMaxImpassables < uiOurMaxImpassables) ||
					(iTheirGroupFirstValue <= iOurGroupFirstVal &&
						uiTheirMaxImpassables > uiOurMaxImpassables))
				{
					continue;
				}
			} // </advc.057>
			int iPathTurns = 0;
			if (atPlot(&kLoopPlot) ||
				generatePath(&kLoopPlot, iFlags, true, &iPathTurns, iMaxPath))
			{
				int iCost = 100 * (iPathTurns * iPathTurns + 1);
				iCost *= 4 + pLoopGroup->getCargo();
				iCost /= 2 + pLoopGroup->getNumUnits();
				/*int iSizeMod = 10*std::max(getGroup()->getNumUnits(), pLoopGroup->getNumUnits());
				iSizeMod /= std::min(getGroup()->getNumUnits(), pLoopGroup->getNumUnits());
				iCost *= iSizeMod * iSizeMod;
				iCost /= 1000; */
				if (iCost < iBestValue)
				{
					iBestValue = iCost;
					pBestUnit = pLoopUnit;
				}
			}
		}
	}
	if (pBestUnit == NULL)
		return false; // advc

	if (!atPlot(pBestUnit->plot()))
	{
		if (!bMergeGroups && getGroup()->getNumUnits() > 1)
		{	/*	might as well leave our current group behind
				since they won't be merging anyway. */
			joinGroup(NULL);
		}
		getGroup()->pushMission(MISSION_MOVE_TO_UNIT, pBestUnit->getOwner(),
			pBestUnit->getID(), iFlags, false, false, MISSIONAI_GROUP, NULL, pBestUnit);
	}
	if (atPlot(pBestUnit->plot()))
	{
		if (bMergeGroups)
			getGroup()->mergeIntoGroup(pBestUnit->getGroup());
		else joinGroup(pBestUnit->getGroup());
	}
	return true;
} // K-Mod end

// This function has been heavily edited by K-Mod and by BBAI
void CvUnitAI::AI_attackCityMove()
{
	PROFILE_FUNC();

	if (isCargo())
	{
		AI_handleEmbarkedMilitary();
		return;
	}

	const CvArea& kArea = *area();

	AreaAITypes const eAreaAI = kArea.getAreaAIType(getTeam());
	CvPlayerAI const& kOwner = GET_PLAYER(getOwner()); // K-Mod
	//bool bLandWar = !isBarbarian() && ((eAreaAIType == AREAAI_OFFENSIVE) || (eAreaAIType == AREAAI_DEFENSIVE) || (eAreaAIType == AREAAI_MASSING));
	bool const bLandWar = (isBarbarian() ?
		// advc: moved up
		(kArea.getNumCities() > 0/*kArea.getCitiesPerPlayer(BARBARIAN_PLAYER)*/) :
		kOwner.AI_isLandWar(kArea)); // K-Mod
	bool const bAssault = !isBarbarian() && (eAreaAI == AREAAI_ASSAULT ||
		eAreaAI == AREAAI_ASSAULT_ASSIST || eAreaAI == AREAAI_ASSAULT_MASSING);
	bool const bTurtle = kOwner.AI_isDoStrategy(AI_STRATEGY_TURTLE);
	bool const bAlert1 = kOwner.AI_isDoStrategy(AI_STRATEGY_ALERT1);
	bool const bIgnoreFaster = (kOwner.AI_isDoStrategy(AI_STRATEGY_LAND_BLITZ) &&
		!bAssault && kArea.getCitiesPerPlayer(getOwner()) > 0);
	bool const bInCity = getPlot().isCity();

	if (bInCity && /* cdtw.9: */ getPlot().getTeam() == getTeam())
	{
		// force heal if we in our own city and damaged
		/*if (getGroup()->getNumUnits() == 1 && getDamage() > 0) {
			getGroup()->pushMission(MISSION_HEAL);
			return;
		}*/ // <advc.299>
		if (AI_singleUnitHeal(0, 0))
			return; // </advc.299>
		// <advc.114e> (from MNAI)
		if (AI_leaveAttack(1, 70, 150))
			return; // </advc.114e>
		/*if (bIgnoreFaster) {
			// BBAI TODO: split out slow units ... will need to test to make sure this doesn't cause loops
		}*/
		if ((GC.getGame().getGameTurn() - getPlot().getPlotCity()->getGameTurnAcquired()) <= 1 &&
			// cdtw.9: (comment from Dave_uk) only do this in our own cities though
			getPlot().getOwner() == getOwner())
		{
			CvSelectionGroupAI* pOldGroup = AI_getGroup();
			pOldGroup->AI_seperateNonAI(UNITAI_ATTACK_CITY);
			if (pOldGroup != getGroup())
				return;
		}
		// note. this will eject a unit to defend the city rather than using the whole group
		if (AI_guardCity(false))
			return;
		//if ((eAreaAIType == AREAAI_ASSAULT) || (eAreaAIType == AREAAI_ASSAULT_ASSIST))
	}

	bool const bEnemyTerritory = isEnemy(getPlot()); // advc: renamed from "bAtWar"

	bool bHuntBarbs = false;
	bool bReadyToAttack = false;
	if (isBarbarian())
		bReadyToAttack = (getGroup()->getNumUnits() >= 2/*3*/); // WTP: Barbs are generally weak
	// <advc.300>
	else if (!bTurtle)
	{
		int const iGroupSz = getGroup()->getNumUnits();
		int iBarbarianGarrison = 0; // kOwner.AI_estimateBarbarianGarrisonSize();
		if ((eAreaAI != AREAAI_DEFENSIVE && eAreaAI != AREAAI_OFFENSIVE && !bAlert1) ||
			iBarbarianGarrison < 2 /*  * kOwner.AI_getCurrEraFactor()*/)
		{
			bHuntBarbs = true;
		}
		//bReadyToAttack = (iGroupSz >= (bHuntBarbs ? 3 : AI_stackOfDoomExtra())); // BtS
		/*  Don't yet know if we'll actually target a Barbarian city, so it's hard to
			decide on the proper size of the attack stack. But if there is nothing else
			to attack, it's easy. */
		bool bHuntOnlyBarbs = (bHuntBarbs &&
			!GET_TEAM(getTeam()).AI_isSneakAttackReady() &&
			GET_TEAM(getTeam()).getNumWars() <= 0);
		if (!bHuntOnlyBarbs && iGroupSz >= AI_stackOfDoomExtra())
			bReadyToAttack = true;
		else if (bHuntOnlyBarbs &&
			iGroupSz >= 2 /*kOwner.AI_neededCityAttackersVsBarbarians()*/ &&
			// Don't send a giant stack. (Tbd.: Should perhaps split the group up then.)
			iGroupSz < 3 * iBarbarianGarrison)
		{
			bReadyToAttack = true;
		}
	} // </advc.300>

	if (bReadyToAttack)
	{
		/*	Check that stack has units which can capture cities
			(K-Mod, I've edited this section to distinguish between
			'no capture' and 'combat limit < 100') */
		bReadyToAttack = false;
		//int iNoCombatLimit = 0;
		int iCityCapture = 0;
		CvSelectionGroup const& kGroup = *getGroup();
		FOR_EACH_UNIT_IN(pLoopUnit, kGroup)
		{
			//if (!pLoopUnit->isOnlyDefensive())
			if (pLoopUnit->canAttack() && // K-Mod
				!pLoopUnit->getUnitInfo().isOnlyDefensive()/*isMostlyDefensive()*/) // advc.315
			{
				if (!pLoopUnit->isNoCityCapture())
					iCityCapture++;
				/*
				if (pLoopUnit->combatLimit() >= 100)
					iNoCombatLimit++;
				*/
				//if (iCityCapture > 5 || 3 * iCityCapture > kGroup.getNumUnits())
				if ((iCityCapture >= 3 || 2 * iCityCapture > kGroup.getNumUnits()) /*&&
					(iNoCombatLimit >= 6 || 3 * iNoCombatLimit > kGroup.getNumUnits())*/)
				{
					bReadyToAttack = true;
					break;
				}
			}
		}
	}

	/*	K-Mod. Try to be consistent in our usage of move flags,
		so that we don't cause unnecessary pathfinder resets.
		advc (note): There are a couple of exceptions where NO_MOVEMENT_FLAGS
		is used. I guess this was done on purpose(?). */
	MovementFlags const eMoveFlags = static_cast<MovementFlags>(MOVE_AVOID_ENEMY_WEIGHT_2 |
		(bReadyToAttack ? MOVE_ATTACK_STACK | MOVE_DECLARE_WAR : NO_MOVEMENT_FLAGS));

	// K-Mod. Barbarian stacks should be reckless and unpredictable.
	if (isBarbarian())
	{
		int iThreshold = SyncRandNum(150) + 20;
		if (AI_stackVsStack(1, iThreshold, 0, eMoveFlags))
			return;
	}

	//if (AI_groupMergeRange(UNITAI_ATTACK_CITY, 0, true, true, bIgnoreFaster))
	if (AI_omniGroup(UNITAI_ATTACK_CITY, -1, -1, true, eMoveFlags,
		0, true, false, bIgnoreFaster))
	{
		return;
	}

	CvCity* pTargetCity = NULL;
	if (isBarbarian())
		pTargetCity = AI_pickTargetCity(eMoveFlags, 10); // was 12 (K-Mod)
	else
	{
		//pTargetCity = AI_pickTargetCity(eMoveFlags, MAX_INT, bHuntBarbs);
		/*	K-Mod. Try to avoid picking a target city in cases where we
			clearly aren't ready. (just for efficiency.) */
		if (bReadyToAttack || bEnemyTerritory ||
			(!bInCity && getGroup()->getNumUnits() > 1))
		{
			pTargetCity = AI_pickTargetCity(eMoveFlags, MAX_INT, bHuntBarbs);
		}
	}

	/*	K-Mod. This is used to prevent the AI from oscillating
		between moving to attack moving to pillage. */
	bool bTargetTooStrong = false;
	// advc.114c: Moved up. Target strength ratio for city attack.
	int iAttackRatio = -1;

	int iStepDistToTarget = MAX_INT;
	// K-Mod note.: I've rearranged some parts of the code below, sometimes without comment.
	if (pTargetCity != NULL)
	{
		int iComparePostBombard =
			/*	advc.159: Avoid overflow when applying the modifier below.
				Will only get compared with attack ratio percentages, which
				should be 3-digit numbers. So 10k is a huge ceiling. */
			std::min(10000,
				AI_getGroup()->AI_compareStacks(pTargetCity->plot(), true));
		int iBombardTurns = AI_getGroup()->AI_getBombardTurns(pTargetCity);
		// K-Mod note: AI_compareStacks will try to use the AI memory if it can't see.
		{
			// K-Mod
			/*	The defense modifier is counted in AI_compareStacks.
				So if we add it again, we'd be double counting.
				I'm going to subtract defence, but unfortunately this will
				reduce based on the total rather than the base. */
			int iDefenseModifier = pTargetCity->getDefenseModifier(/*false*/);
			int iReducedModifier = iDefenseModifier;
			iReducedModifier *= std::min(20, iBombardTurns);
			iReducedModifier /= 20;
			int iBase = 210;
			if (pTargetCity->getPlot().isHills())
				iBase += GC.getHILLS_EXTRA_DEFENSE();
			iComparePostBombard *= iBase;
			iComparePostBombard /= std::max(1,
				// def. mod. < 200. I promise.
				iBase + iReducedModifier - iDefenseModifier);
			/*	iBase > 100 is to offset the over-reduction from compounding.
				With iBase == 200, bombarding a defence bonus of 100% will
				reduce effective defence by 50% */
		}

		iAttackRatio = BBAI_ATTACK_CITY_STACK_RATIO;
		int iAttackRatioSkipBombard = BBAI_SKIP_BOMBARD_MIN_STACK_RATIO;
		iStepDistToTarget = stepDistance(pTargetCity->plot(), plot());
		// K-Mod - I'm going to scale the attack ratio based on our war strategy
		if (isBarbarian())
			iAttackRatio = 80;
		else
		{
			int iAdjustment = 5;
			int iExtraAdjustBombard = 0; // advc.114c
			if (GET_TEAM(getTeam()).AI_getWarPlan(pTargetCity->getTeam()) == WARPLAN_LIMITED)
			{
				iAdjustment += 10;
				/*	advc.114c: Shouldn't be too unwilling to sacrifice siege units
					when fighting a limited war. Not crucial to conquer more. */
				iExtraAdjustBombard -= 5;
			}
			if (kOwner.AI_isDoStrategy(AI_STRATEGY_CRUSH))
				iAdjustment -= 10;
			if (iAdjustment >= 0 && pTargetCity == area()->AI_getTargetCity(getOwner()))
				iAdjustment -= 10;
			iAdjustment += range(
				(GET_TEAM(getTeam()).AI_getEnemyPowerPercent(true) - 100) / 12,
				-10, 0);
			if (iStepDistToTarget <= 1 && pTargetCity->isOccupation())
				iAdjustment += range(111 - (iAttackRatio + iAdjustment), -10, 0); // k146
			iAttackRatio += iAdjustment;
			iAttackRatioSkipBombard += iAdjustment + /* advc.114c: */ iExtraAdjustBombard;
			FAssert(iAttackRatioSkipBombard >= iAttackRatio);
			FAssert(iAttackRatio >= 100);
		} // K-Mod end

		bTargetTooStrong = (iComparePostBombard < iAttackRatio);
		if (iStepDistToTarget <= 2)
		{	// K-Mod. I've rearranged and rewritten most of this block - removing the bbai code.

			if (bTargetTooStrong)
			{
				if (AI_stackVsStack(2, iAttackRatio, 80, eMoveFlags))
					return;

				FAssert(getDomainType() == DOMAIN_LAND);
				int iOurOffense = kOwner.AI_localAttackStrength(
					plot(), getTeam(), DOMAIN_LAND, 1, false);
				int iEnemyOffense = kOwner.AI_localAttackStrength(
					plot(), NO_TEAM, DOMAIN_LAND, 2, false);

				// If in danger, seek defensive ground
				if (4 * iOurOffense < 3 * iEnemyOffense)
				{
					// including smaller groups
					if (AI_omniGroup(UNITAI_ATTACK_CITY, -1, -1, true, eMoveFlags,
						3, true, false, bIgnoreFaster, false, /*bBiggerOnly=*/false))
					{
						return;
					}
					if (iAttackRatio > 2 * iComparePostBombard &&
						4 * iEnemyOffense > 5 * kOwner.AI_localDefenceStrength(plot(), getTeam()))
					{
						/*	we don't have anywhere near enough attack power,
							and we are in serious danger.
							unfortunately, if we are "bReadyToAttack", we'll probably end up
							coming straight back here... */
						if (!bReadyToAttack && AI_retreatToCity())
							return;
					}
					if (AI_getGroup()->AI_getMissionAIType() == MISSIONAI_PILLAGE &&
						AI_plotDefense() > 0) // advc.012
						//getPlot().defenseModifier(getTeam(), false) > 0)
					{
						if (isEnemy(getPlot()) && canPillage(plot()))
						{
							getGroup()->pushMission(MISSION_PILLAGE, -1, -1, NO_MOVEMENT_FLAGS,
								false, false, MISSIONAI_PILLAGE, plot());
							return;
						}
					}
					if (AI_choke(2, true, eMoveFlags))
						return;
				}
				else
				{
					if (AI_omniGroup(UNITAI_ATTACK_CITY, -1, -1, true, eMoveFlags,
						3, true, false, bIgnoreFaster)) // bigger groups only
					{
						return;
					}
					if (canBombard(&getPlot()))
					{
						getGroup()->pushMission(MISSION_BOMBARD, -1, -1, NO_MOVEMENT_FLAGS,
							false, false, MISSIONAI_ASSAULT, pTargetCity->plot());
						return;
					}

					if (AI_omniGroup(UNITAI_ATTACK_CITY, -1, -1, true, eMoveFlags, 3,
						true, false, bIgnoreFaster, false, /*bBiggerOnly=*/false)) // any size
					{
						return;
					}
				}
			}

			if (iStepDistToTarget == 1)
			{
				/*	Consider getting into a better position for attack.
					only if we don't already have overwhelming force */
				if (iComparePostBombard < BBAI_SKIP_BOMBARD_BASE_STACK_RATIO &&
					(iComparePostBombard < iAttackRatioSkipBombard ||
						2 * pTargetCity->getDefenseDamage() < GC.getMAX_CITY_DEFENSE_DAMAGE() ||
						getPlot().isRiverCrossing(directionXY(getPlot(), pTargetCity->getPlot()))))
				{
					/*	Only move into attack position if we have a chance.
						Without this check, the AI can get stuck alternating
						between this, and pillage.
						I've tried to roughly take into account how much our ratio
						would improve by removing a river penalty. */
					if ((getGroup()->canBombard(&getPlot()) && iBombardTurns > 2) ||
						(getPlot().isRiverCrossing(directionXY(getPlot(), pTargetCity->getPlot())) &&
							150 * iComparePostBombard >=
							(150 + GC.getRIVER_ATTACK_MODIFIER() * iAttackRatio)))
					{
						if (AI_goToTargetCity(eMoveFlags, 2, pTargetCity))
							return;
					}
					// Note: bombard may skip if stack is powerful enough
					if (AI_bombardCity())
						return;
				}
				// we're satisfied with our position already. But we still want to consider bombarding.
				else if (iComparePostBombard >= iAttackRatio && AI_bombardCity())
					return;

				if (iComparePostBombard >= iAttackRatio)
				{
					// in position; and no desire to bombard.  So attack!
					if (AI_stackAttackCity(iAttackRatio))
						return;
				}
			}

			if (iComparePostBombard >= iAttackRatio &&
				AI_goToTargetCity(eMoveFlags, 4, pTargetCity))
			{
				return;
			}
		}
	}

	/*	K-Mod. Lets have some slightly smarter stack vs. stack AI.
		it would be nice to have some personality effection here...
		eg. protective leaders have a lower risk threshold.   -- Maybe later.
		Note. This stackVsStack stuff used to be a bit lower,
		after the group and the heal stuff. */
	if (getGroup()->getNumUnits() > 1)
	{
		if (bEnemyTerritory)
		{
			// note. if we are 2 steps from the target city, this check here is redundant. (see code above)
			if (AI_stackVsStack(1, 160, 95, eMoveFlags))
				return;
		}
		else
		{
			int const iSearchRange = 4;
			if (eAreaAI == AREAAI_DEFENSIVE && getPlot().getOwner() == getOwner())
			{
				if (AI_stackVsStack(iSearchRange, 110, 55, eMoveFlags))
					return;
				if (AI_stackVsStack(iSearchRange, 180, 30, eMoveFlags))
					return;
			}
			else if (AI_stackVsStack(iSearchRange, 130, 60, eMoveFlags))
				return;
		}
	}

	/*  K-Mod. The loading of units for assault needs to be before the following
		omnigroup - otherwise the units may leave the boat to join their friends. */
	if (bAssault && (pTargetCity == NULL || !pTargetCity->isArea(*area())))
	{
		if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI,
			-1, -1, -1, -1, eMoveFlags, /*iMaxPath=*/6)) // was 4
		{
			return;
		}
	} // K-Mod end
	{
		int iMaxGroupPath = 2;
		/*	<advc.083> Increase the range for joining another group when
			getting ready to attack takes long. Might be that everyone else
			is gathering at a different staging city. */
		if (getPlot().isCity() && GET_TEAM(getTeam()).AI_isAnyChosenWar() &&
			!isBarbarian() &&
			(eAreaAI == AREAAI_OFFENSIVE || eAreaAI == AREAAI_MASSING))
		{
			int iMaxWPAge = 0;
			//for (TeamIter<MAJOR_CIV, KNOWN_POTENTIAL_ENEMY_OF> itEnemy(getTeam());
			//	itEnemy.hasNext(); ++itEnemy)
			FOR_EACH_MAJOR_CIV_ALIVE_AND_KNOWN_POTENTIAL_ENEMY_OF_TEAM(getTeam(), kEnemy)
			{
				iMaxWPAge = std::max(iMaxWPAge,
					GET_TEAM(getTeam()).AI_getWarPlanStateCounter(kEnemy.getID()));
			}
			if (iMaxWPAge > 0)
			{
				/*	I think we want to check this periodically, not totally
					at random. */
				//int iIntervalRand = (scaled::hash(getGroup()->getID(), getOwner()) * 5).
				//	floor();
				//if (iMaxWPAge % (iIntervalRand + 8) == 0)
				iMaxGroupPath += 1 + SyncRandNum(3);
			}
		} // </advc.083>
		//if (AI_groupMergeRange(UNITAI_ATTACK_CITY, iMaxGroupPath, true, true, bIgnoreFaster))
		if (AI_omniGroup(UNITAI_ATTACK_CITY, -1, -1, true, eMoveFlags, iMaxGroupPath,
			true, false, bIgnoreFaster))
		{
			return;
		}
	}
	if (AI_heal(30, 1))
		return;

	/*if (collateralDamage() > 0 && getPlot().getOwner() == getOwner()) {
		if (AI_anyAttack(1, 45, eMoveFlags, 3, false, false))
			return;
		if (!bReadyToAttack) {
			if (AI_anyAttack(1, 25, eMoveFlags, 5, false))
				return;
		}
	}*/ // BtS

	//if (AI_anyAttack(1, 60, eMoveFlags, 0, false))
	/*	K-Mod (changed to allow cities, and to only use a single unit,
		but it is still a questionable move) */

	const int iAttackFlags = (int)eMoveFlags | MOVE_SINGLE_ATTACK;
	if (AI_anyAttack(1, 60, static_cast<MovementFlags>(iAttackFlags)))
		return;

	// K-Mod - replacing some stuff I moved / removed from the BBAI code
	if (pTargetCity != NULL && bTargetTooStrong &&
		iStepDistToTarget <= (bReadyToAttack ? 3 : 2))
	{
		/*	<advc.114c> Attrition warfare is miserable for both sides,
			rather take our chances if they're not terrible. */
		if (getGroup()->getNumUnits() > 3 /* + kOwner.AI_getCurrEraFactor() / 2 */ &&
			iAttackRatio > 100)
		{
			if (AI_stackAttackCity((iAttackRatio + 100) / 2))
				return;
		} // </advc.114c>
		// Pillage around enemy city
		if (generatePath(&pTargetCity->getPlot(), eMoveFlags, true, 0, 5))
		{
			/*	the above path check is just for efficiency.
				Otherwise we'd be checking every surrounding tile. */
			if (AI_pillageAroundCity(pTargetCity, 11, eMoveFlags, 2)) // was 3 turns
				return;

			if (AI_pillageAroundCity(pTargetCity, 0, eMoveFlags, 4)) // was 5 turns
				return;
		}
		// choke the city.
		if (iStepDistToTarget <= 2 && AI_choke(1, false, eMoveFlags))
			return;
		/*	if we're already standing right next to the city, then goToTargetCity can fail
			- and we might end up doing something stupid instead. So try again to choke. */
		if (iStepDistToTarget <= 1 && AI_choke(3, false, eMoveFlags))
			return;
	}

	/*	one more thing. Sometimes a single step can cause the AI to change its target city;
		and when it changes the target - and so sometimes they can get stuck in a loop where
		they step towards their target, change their mind, step back to pillage something, ...
		Here I've made a kludge to break that cycle: */
	if (AI_getGroup()->AI_getMissionAIType() == MISSIONAI_PILLAGE)
	{
		CvPlot* pMissionPlot = AI_getGroup()->AI_getMissionAIPlot();
		if (pMissionPlot != NULL && canPillage(pMissionPlot) &&
			isEnemy(*pMissionPlot))
		{
			if (atPlot(pMissionPlot))
			{
				getGroup()->pushMission(MISSION_PILLAGE, -1, -1, NO_MOVEMENT_FLAGS,
					false, false, MISSIONAI_PILLAGE, pMissionPlot);
				return;
			}
			if (generatePath(pMissionPlot, eMoveFlags, true, 0, 6))
			{
				/*  the max path turns is arbitrary, but it should be at least as
					big as the pillage sections higher up. */
				CvPlot& kEndTurnPlot = *getPathEndTurnPlot();
				// warning: this command may attack something. We haven't checked!
				pushGroupMoveTo(kEndTurnPlot, eMoveFlags, false, false,
					MISSIONAI_PILLAGE, pMissionPlot);
				return;
			}
		}
	}
	// K-Mod end

	if (bEnemyTerritory && (bTargetTooStrong || getGroup()->getNumUnits() <= 2))
	{
		if (AI_pillageRange(3, 11, eMoveFlags))
			return;
		if (AI_pillageRange(1, 0, eMoveFlags))
			return;
	}

	if (getPlot().getOwner() == getOwner())
	{
		/*if (!bLandWar) {
			if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, eMoveFlags, 4))
				return;
		}*/ // BtS - K-Mod: I've moved this to be above the omniGroup stuff, otherwise it just causes AI confusion.

		if (bReadyToAttack)
		{
			// Wait for units about to join our group
			/*MissionAITypes eMissionAIType = MISSIONAI_GROUP;
			int iJoiners = kOwner.AI_unitTargetMissionAIs(this, &eMissionAIType, 1, getGroup(), 2);
			if (iJoiners * 5 > getGroup()->getNumUnits()) {
				getGroup()->pushMission(MISSION_SKIP);
				return;
			}*/ // BBAI
			// K-Mod. If the target city is close, be less likely to wait for backup.
			int iPathTurns = 10;
			int iMaxWaitTurns = 3;
			if (pTargetCity != NULL &&
				generatePath(&pTargetCity->getPlot(), eMoveFlags, true,
					&iPathTurns, iPathTurns))
			{
				iMaxWaitTurns = (iPathTurns + 1) / 3;
			}
			MissionAITypes eMissionAIType = MISSIONAI_GROUP;
			int iJoiners = (iMaxWaitTurns > 0 ? kOwner.AI_unitTargetMissionAIs(
				this, &eMissionAIType, 1, getGroup(), iMaxWaitTurns) : 0);

			if (iJoiners * range(iPathTurns - 1, 2, 5) > getGroup()->getNumUnits())
			{	// (the mission is just for debug feedback)
				getGroup()->pushMission(MISSION_SKIP, -1, -1, NO_MOVEMENT_FLAGS,
					false, false, MISSIONAI_GROUP);
				return;
			}
			// K-Mod end
		}
		else
		{
			if (bTurtle)
			{
				// K-Mod
				if (AI_leaveAttack(1, 51, 100))
					return;
				if (AI_defendTerritory(70, eMoveFlags, 3))
					return;
				// K-Mod end
				if (AI_guardCity(/*false, true, 7, eMoveFlags*/))
					return;
			}
			else if (!isBarbarian() && eAreaAI == AREAAI_DEFENSIVE)
			{
				// Use smaller attack city stacks on defense
				// K-Mod
				if (AI_defendTerritory(65, eMoveFlags, 3))
					return;
				// K-Mod end
				if (AI_guardCity(/*false, true, 3, eMoveFlags*/))
					return;
			}

			int iTargetCount = kOwner.AI_unitTargetMissionAIsInternal(this, MISSIONAI_GROUP);
			if (iTargetCount * 5 > getGroup()->getNumUnits())
			{
				MissionAITypes eMissionAIType = MISSIONAI_GROUP;
				int iJoiners = kOwner.AI_unitTargetMissionAIs(
					this, &eMissionAIType, 1, getGroup(), 2);
				if (iJoiners * 5 > getGroup()->getNumUnits())
				{	// K-Mod (for debug feedback)
					getGroup()->pushMission(MISSION_SKIP, -1, -1, NO_MOVEMENT_FLAGS,
						false, false, MISSIONAI_GROUP);
					return;
				}
				if (AI_moveToStagingCity())
					return;
			}
		}
	}

	if (AI_heal(50, 3))
		return;

	if (!bEnemyTerritory)
	{
		if (AI_heal())
			return;
		if (getGroup()->getNumUnits() == 1 && getTeam() != getPlot().getTeam())
		{
			if (AI_retreatToCity())
				return;
		}
	}

	if (!bReadyToAttack && !noDefensiveBonus())
	{
		if (AI_guardCity(/*false, false, MAX_INT, eMoveFlags*/))
			return;
	}

	if (bReadyToAttack /* advc.083: */ && !bTargetTooStrong)
	{	// advc.opt: Moved into the bReadyToAttack branch
		bool bAnyWarPlan = GET_TEAM(getTeam()).AI_isAnyWarPlan();
		/* BBAI code
		if (isBarbarian()) {
			if (AI_goToTargetCity(eMoveFlags, 12))
				return;
			if (AI_pillageRange(3, 11, eMoveFlags))
				return;
			if (AI_pillageRange(1, 0, eMoveFlags))
				return;
		}
		else if (bHuntBarbs && AI_goToTargetBarbCity((bAnyWarPlan ? 7 : 12)))
			return;
		else if (bLandWar && pTargetCity != NULL)*/
		// K-Mod
		if (isBarbarian())
		{
			// target city has already been calculated.
			if (pTargetCity != NULL && AI_goToTargetCity(eMoveFlags, 12, pTargetCity))
				return;
			// <advc.300>
			int iSearchRange = 3;
			if (kArea.getAreaAIType(getTeam()) == AREAAI_ASSAULT)
				iSearchRange = 1; // </advc.300>
			if (AI_pillageRange(iSearchRange, 0, eMoveFlags))
				return;
		}
		else if (pTargetCity != NULL)
			// K-Mod end
		{
			// Before heading out, check whether to wait to allow unit upgrades
			if (bInCity && getPlot().getOwner() == getOwner() /* &&
				!GET_PLAYER(getOwner()).AI_isFinancialTrouble()
				!kOwner.AI_isFinancialTrouble() && !pTargetCity->isBarbarian()*/)
			{
				// Check if stack has units which can upgrade
				int iNeedUpgradeCount = 0;
				CvSelectionGroup const& kGroup = *getGroup();
				FOR_EACH_UNIT_IN(pLoopUnit, kGroup)
				{
					if (pLoopUnit->getUpgradeCity(false) == NULL)
						continue;
					iNeedUpgradeCount++;
					if (5 * iNeedUpgradeCount > kGroup.getNumUnits()) // was 8*
					{
						getGroup()->pushMission(MISSION_SKIP);
						return;
					}
				}
			}

			// K-Mod. (original bloated code deleted)
			// Estimate the number of turns required.
			int iPathTurns;
			if (!generatePath(&pTargetCity->getPlot(), eMoveFlags, true, &iPathTurns))
			{	// AI_pickTargetCity now allows boat-only paths, so this assertion no longer holds.
				//FErrorMsg("failed to find path to target city.");
				iPathTurns = 100;
			}
			if (/*!pTargetCity->isBarbarian() ||*/
				// don't bother with long-distance barb attacks
				iPathTurns < (bAnyWarPlan ? 7 : 12))
			{
				// See if we can get there faster by boat..
				if (iPathTurns > 5)// && !pTargetCity->isBarbarian())
				{
					/*  note: if the only land path to our target happens to go
						through a tough line of defence...
						we probably want to take the boat even if our iPathTurns is
						low. Here's one way to account for that:
						iPathTurns = std::max(iPathTurns, getPathLastNode()->
						m_iTotalCost / (2000*GC.getMOVE_DENOMINATOR()));
						Unfortunately, that "2000"... well I think you know what the
						problem is. So maybe next time. */
					int iLoadTurns = std::max(3, iPathTurns / 3 - 1); // k146
					int iMaxTransportTurns = iPathTurns - iLoadTurns - 2;
					if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI,
						-1, -1, -1, -1, eMoveFlags, iLoadTurns, iMaxTransportTurns))
					{
						return;
					}
				}
				// We have to walk.
				if (AI_goToTargetCity(eMoveFlags, MAX_INT, pTargetCity))
					return;

				if (bAnyWarPlan)
				{
					/*	advc.083: This can no longer occur. We shouldn't
						"just wait for reinforcements" paying supply costs and
						possibly leaving our own cities exposed. */
#if 0
						/*	We're at war, but we failed to walk to the target.
							Before we start wigging out, lets just check one more thing... */
					if (bTargetTooStrong && iStepDistToTarget == 1)
					{
						/*	we're standing outside the city already, but we can't capture it
							and we can't pillage or choke it.
							I guess we'll just wait for reinforcements to arrive. */
						if (AI_safety())
							return;
						getGroup()->pushMission(MISSION_SKIP);
						return;
					}
#endif

					//CvCity* pTargetCity =
					/*  advc: Don't shadow the pTargetCity variable that the rest of
						this function cares about (and which isn't necessarily in this
						unit's area).
						Note: In the code below, only airlifts care about pTargetCity,
						and that'll fail b/c AI_safety has already failed. */
					CvCity* pAreaTargetCity = kArea.AI_getTargetCity(getOwner());
					if (pAreaTargetCity != NULL)
					{	/*  advc: One way that this can happen: Owner is at war with a civ that
							it can only reach through the territory of a third party (no OB) and
							is preparing war against the third party.
							AI_pickTargetCity will then pick a city of the current war enemy, but
							the Area AI will be set to a non-ASSAULT type, meaning that AI_attackCityMove
							will (in vain) look for a land path. AI_solveBlockageProblem will then (always?)
							fail, and the unit won't move at all. This is probably for the best -- wait
							until the preparations are through. Difficult to avoid the assertions below. */
							/*  this is a last resort. I don't expect that we'll ever actually need it.
								(it's a pretty ugly function, so I /hope/ we don't need it.) */
						FErrorMsg("AI_attackCityMove is resorting to AI_solveBlockageProblem");
						if (AI_solveBlockageProblem(pAreaTargetCity->plot(),
							(GET_TEAM(getTeam()).getNumWars() <= 0)))
						{
							return;
						}  // advc.006:
						FErrorMsg("AI_solveBlockageProblem returned false");
					}
				}
			}
			// K-Mod end
		}
	}
	else
	{
		/*int iTargetCount = kOwner.AI_unitTargetMissionAIs(this, MISSIONAI_GROUP);
		if (iTargetCount * 4 > getGroup()->getNumUnits() || getGroup()->getNumUnits() + iTargetCount >= (bHuntBarbs ? 3 : AI_stackOfDoomExtra())) {
			MissionAITypes eMissionAIType = MISSIONAI_GROUP;
			int iJoiners = kOwner.AI_unitTargetMissionAIs(this, &eMissionAIType, 1, getGroup(), 2);
			if (6*iJoiners > getGroup()->getNumUnits()) {
				getGroup()->pushMission(MISSION_SKIP);
				return;
			}
			if (AI_safety())
				return;
		}*/ // BtS
		// K-Mod
		int iTargetCount = kOwner.AI_unitTargetMissionAIsInternal(this, MISSIONAI_GROUP);
		if (6 * iTargetCount > getGroup()->getNumUnits())
		{
			MissionAITypes eMissionAIType = MISSIONAI_GROUP;
			int iNearbyJoiners = kOwner.AI_unitTargetMissionAIs(
				this, &eMissionAIType, 1, getGroup(), 2);
			if (4 * iNearbyJoiners > getGroup()->getNumUnits())
			{
				getGroup()->pushMission(MISSION_SKIP);
				return;
			}
			if (AI_safety())
				return;
		} // K-Mod end

		if (bombardRate() > 0 && noDefensiveBonus())
		{
			/*	BBAI Notes: Add this stack lead by a bombard unit
				to a stack probably not lead by a bombard unit */
				/*	BBAI TODO: Some sense of minimum stack size?
					Can have big stack moving 10 turns to merge with tiny stacks */
					//if (AI_group(UNITAI_ATTACK_CITY, -1, -1, -1, bIgnoreFaster, true, true, /*iMaxPath*/ 10, /*bAllowRegrouping*/ true))
			if (AI_omniGroup(UNITAI_ATTACK_CITY, -1, -1, true, eMoveFlags, 10,
				true, getGroup()->getNumUnits() < 2, bIgnoreFaster, true, true))
			{
				return;
			}
		}
		else
		{
			//if (AI_group(UNITAI_ATTACK_CITY, AI_stackOfDoomExtra() * 2, -1, -1, bIgnoreFaster, true, true, /*iMaxPath*/ 10, /*bAllowRegrouping*/ false))
			if (AI_omniGroup(UNITAI_ATTACK_CITY, AI_stackOfDoomExtra() * 2,
				-1, true, eMoveFlags, 10,
				true, getGroup()->getNumUnits() < 2, bIgnoreFaster, false, true))
			{
				return;
			}
		}
	}

	if (getPlot().getOwner() == getOwner() && bLandWar)
	{
		if (GET_TEAM(getTeam()).getNumWars() > 0)
		{
			// if no land path to enemy cities, try getting there another way
			if (pTargetCity == NULL)
			{
				if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT,
					NO_UNITAI, -1, -1, -1, -1, eMoveFlags, 4))
				{
					return;
				}
			}
		}
	}

	// K-Mod
	if (AI_defendTerritory(70, eMoveFlags, 1, true))
		return;
	// K-Mod end
	if (AI_moveToStagingCity())
		return;
	if (AI_retreatToCity())
		return;
	// K-Mod
	if (AI_handleStranded())
		return;
	// K-Mod end
	if (AI_safety())
		return;
	getGroup()->pushMission(MISSION_SKIP);
}

// advc.299: Cut from AI_heal
bool CvUnitAI::AI_singleUnitHeal(int iMaxTurnsExposed, int iMaxTurnsOutsideCity)
{
	CvSelectionGroup& kGroup = *getGroup();

	if (isAlwaysHeal() || getDamage() <= 0 || kGroup.getNumUnits() != 1)
		return false;
	bool bHeal = false;
	//if (GET_TEAM(getTeam()).isCityHeal(getPlot())) // advc.299: was getPlot().isCity()
	if (plot()->isFriendlyCity(*this, /*bCheckImprovement*/true)) // advc.299: was getPlot().isCity()
		bHeal = true;
	else if (!isBarbarian())
	{
		int const iHealTurns = healTurns(plot());
		if (iHealTurns >= 20) // advc: Feature damage
			return false;
		if (iHealTurns <= iMaxTurnsExposed ||
			// <advc.299>
			(iHealTurns <= iMaxTurnsOutsideCity &&
				getPlot().defenseModifier(getTeam(), true) > 0) ||
			// If the group had urgent business, it probably wouldn't be automated.
			isAutomated()) // </advc.299>
		{
			bHeal = true;
		}
	}
	// <advc.306>
	if (!bHeal && isBarbarian())
	{
		int iHeal = healRate(plot());
		if (iHeal >= 5 /* && SyncRandSuccess(scaled(2 * iHeal,
			getDomainType() == DOMAIN_SEA ? 35 : 45))*/)
		{
			bHeal = true;
		}
	} // </advc.306>
	if (bHeal)
	{
		kGroup.pushMission(MISSION_HEAL, -1, -1, NO_MOVEMENT_FLAGS,
			false, false, MISSIONAI_HEAL);
		return true;
	}
	return false;
}

// This function has been significantly modified for K-Mod
bool CvUnitAI::AI_stackAttackCity(int iPowerThreshold)
{
	PROFILE_FUNC();

	FAssert(canMove());

	CvPlot const* pCityPlot = NULL;
	FOR_EACH_ADJ_PLOT(plot(),
	{
		CvPlot const& p = *pAdjacentPlot;
		//if (!AI_plotValid(p)) continue; // advc.opt: We're a land unit looking for an adjacent city
		if (!p.isCity()) // K-Mod. We want to attack a city. We don't care about guarded forts!
			//|| (pLoopPlot->isCity(true) && pLoopPlot->isVisibleEnemyUnit(this)))
		{
			continue;
		}
		if (AI_mayAttack(p.getTeam(), p))
		{
			if (//!at(p) && // advc: Exclude this from the beginning
				//((bFollow) ? canMoveInto(pLoopPlot, /*bAttack*/ true, /*bDeclareWar*/ true) : (generatePath(pLoopPlot, 0, true, &iPathTurns) && (iPathTurns <= iRange))))
				getGroup()->canMoveOrAttackInto(p, true, true))
			{
				// K-Mod
				if (iPowerThreshold < 0)
				{
					// basic threshold calculation.
					CvCity* pCity = p.getPlotCity();
					/*	This automatic threshold calculation is used by AI_follow;
						and so we can't assume this unit is the head of the group.
						... But I think it's fair to assume that, if our group
						has any bombard, the head unit will have it. */
					if (getGroup()->getHeadUnit()->bombardRate() > 0)
					{
						/*	if we can bombard, then we should do a rough calculation
							to give us a 'skip bombard' threshold. */
						iPowerThreshold = ((GC.getMAX_CITY_DEFENSE_DAMAGE() - pCity->getDefenseDamage()) *
							BBAI_SKIP_BOMBARD_BASE_STACK_RATIO +
							pCity->getDefenseDamage() * BBAI_SKIP_BOMBARD_MIN_STACK_RATIO) /
							std::max(1, GC.getMAX_CITY_DEFENSE_DAMAGE());
					}
					else
					{
						// if we have no bombard ability - just use the minimum threshold
						iPowerThreshold = BBAI_SKIP_BOMBARD_MIN_STACK_RATIO;
					}
					FAssert(iPowerThreshold >= BBAI_ATTACK_CITY_STACK_RATIO);
				} // K-Mod end
				if (AI_getGroup()->AI_compareStacks(&p, true) >= iPowerThreshold)
					pCityPlot = &p;
			}
		}
		// there can only be one city. advc (tbd.): Not true if MIN_CITY_RANGE==1.
		break;
	}) // FOR_EACH

	if (pCityPlot != NULL)
	{
		if (gUnitLogLevel >= 1 && pCityPlot->getPlotCity() != NULL)
		{
			logBBAI("    Stack for player %d (%S) decides to attack city %S with stack ratio %d", getOwner(), GET_PLAYER(getOwner()).getCivilizationDescription(0), pCityPlot->getPlotCity()->getName(0).GetCString(), AI_getGroup()->AI_compareStacks(pCityPlot, true));
			logBBAI("    City %S has defense modifier %d, %d with ignore building", pCityPlot->getPlotCity()->getName(0).GetCString(), pCityPlot->getPlotCity()->getDefenseModifier(/*false*/), pCityPlot->getPlotCity()->getDefenseModifier(/*true*/));
		}

		FAssert(!atPlot(pCityPlot));
		if (AI_considerDOW(*pCityPlot))
		{	// <advc.163>
			if (!canMove())
				return true;
		} // </advc.163>
		pushGroupMoveTo(*pCityPlot, pCityPlot->isVisibleEnemyDefender(this) ?
			static_cast<MovementFlags>(MOVE_DIRECT_ATTACK) : static_cast<MovementFlags>(NO_MOVEMENT_FLAGS));
		return true;
	}

	return false;
}

/*  K-Mod - test if we should declare war before moving to the target plot.
	(originally, DOW were made inside the unit movement mechanics.
	To me, that seems like a really dumb idea.) */
bool CvUnitAI::AI_considerDOW(CvPlot const& kPlot) // advc: param was CvPlot*
{
	CvTeamAI& kOurTeam = GET_TEAM(getTeam());
	TeamTypes ePlotTeam = kPlot.getTeam();

	//if (!canEnterArea(ePlotTeam, pPlot->area(), true))
	/*  Note: We might be a transport ship which ignores borders, but with escorts
		and cargo who don't ignore borders.
		So, we should check that the whole group can enter the borders.
		(There are faster ways to check, but this is good enough.)
		If it's an amphibious landing, lets just assume that our cargo will need a DoW! */
	if (!getGroup()->canEnterArea(ePlotTeam, *kPlot.area(), true) ||
		getGroup()->isAmphibPlot(&kPlot))
	{
		if (ePlotTeam != NO_TEAM && kOurTeam.AI_isSneakAttackReady(ePlotTeam))
		{	/*  advc.163: If the tile that we're on has flipped to the war target,
				the DoW is going to bump us out. Could catch this in AI_attackCityMove
				and other functions, and compute a new path. However, bumping ourselves
				may actually be the fastest way to reach the target, so I'm just
				going to go through with the DoW. */
				/*FAssert(!getPlot().isOwned() || GET_TEAM(getPlot().getTeam()).
						getMasterTeam() != ePlotTeam)*/
			if (kOurTeam.canDeclareWar(ePlotTeam))
			{
				//if (gUnitLogLevel > 0) logBBAI("    %S declares war on %S with AI_considerDOW (%S - %S).", kOurTeam.getName().GetCString(), GET_TEAM(ePlotTeam).getName().GetCString(), getName(0).GetCString(), GC.getInfo(AI_getUnitAIType()).getDescription());
				kOurTeam.declareWar(ePlotTeam, true, NO_WARPLAN);
				CvSelectionGroup::path_finder.Reset();
				return true;
			}
		}
	}
	return false;
}

/*  AI_considerPathDOW checks each plot on the path until the end of the turn.
	Sometimes the end plot is in friendly territory, but we need to declare war
	to actually get there. This situation is very rare, but unfortunately we
	have to check for it every time - because otherwise, when it happens,
	the AI will just get stuck. */
	// AI_considerPathDOW checks each plot on the path until the end of the turn.
	// Sometimes the end plot is in friendly territory, but we need to declare war to actually get there.
	// This situation is very rare, but unfortunately we have to check for it every time
	// - because otherwise, when it happens, the AI will just get stuck.
bool CvUnitAI::AI_considerPathDOW(CvPlot* pPlot, MovementFlags eFlags)
{
	PROFILE_FUNC();

	if (!(eFlags & MOVE_DECLARE_WAR))
		return false;

	if (!generatePath(pPlot, eFlags, true))
	{
		FAssertMsg(false, "AI_considerPathDOW didn't find a path.");
		return false;
	}

	bool bDOW = false;
	FAStarNode* pNode = getPathFinder().GetEndNode(); // TODO: rewrite so that GetEndNode isn't used.
	while (!bDOW && pNode)
	{
		// we need to check DOW even for moves several turns away - otherwise the actual move mission may fail to find a path.
		// however, I would consider it irresponsible to call this function for multi-move missions.
		// (note: amphibious landings may say 2 turns, even though it is really only 1...)
		FAssert(pNode->m_iData2 <= 1 || (pNode->m_iData2 == 2 && getGroup()->isAmphibPlot(GC.getMap().plotSoren(pNode->m_iX, pNode->m_iY))));
		const CvPlot* const p= GC.getMap().plotSoren(pNode->m_iX, pNode->m_iY);
		bDOW = AI_considerDOW(*p);
		pNode = pNode->m_pParent;
	}

	return bDOW;
}
// K-Mod end
// advc.012:
int CvUnitAI::AI_plotDefense(CvPlot const* pPlot) const
{
	// Don't check noDefensiveBonus here b/c this unit can be part of a stack
	//if(noDefensiveBonus()) return 0;
	if (pPlot == NULL)
		pPlot = plot();
	return GET_TEAM(getTeam()).AI_plotDefense(*pPlot);
}

/*  K-Mod
	Look for the nearest suitable transport. Return a pointer to the transport unit.
	(the bulk of this function was moved straight out of AI_load.
	I've fixed it up a bit, but I didn't write most of it.) */
CvUnit* CvUnitAI::AI_findTransport(UnitAITypes eUnitAI, MovementFlags eFlags,
	int iMaxPath, UnitAITypes ePassengerAI, int iMinCargo, int iMinCargoSpace,
	int iMaxCargoSpace, int iMaxCargoOurUnitAI)
{
	PROFILE_FUNC(); // advc.opt
	/*if (getDomainType() == DOMAIN_LAND && !canMoveAllTerrain()) {
		if (getArea().getNumAIUnits(getOwner(), eUnitAI) == 0)
			return false;
	}*/ // disabled, because this would exclude boats sailing on the coast.

	// K-Mod
	if (eUnitAI != NO_UNITAI && GET_PLAYER(getOwner()).AI_getNumAIUnits(eUnitAI) == 0)
		return NULL; // kmodx: was "false"
	// K-Mod end

	int iBestValue = MAX_INT;
	CvUnit* pBestUnit = 0;
	const int iLoadMissionAICount = 4;
	MissionAITypes aeLoadMissionAI[iLoadMissionAICount] = {
			MISSIONAI_LOAD_ASSAULT, MISSIONAI_LOAD_SETTLER,
			MISSIONAI_LOAD_SPECIAL/*, MISSIONAI_ATTACK_SPY*/};
	int iCurrentGroupSize = getGroup()->getNumUnits();
	CvPlayerAI const& kOwner = GET_PLAYER(getOwner());
	FOR_EACH_UNITAI_VAR(pTransport, kOwner)
	{	// <K-Mod>
		if (pTransport->cargoSpace() <= 0 || (!sameArea(*pTransport) &&
			!pTransport->getPlot().isAdjacentToArea(getArea())) ||
			//!canLoadOnto(*pTransport, pTransport->getPlot()))
			!canLoadUnit(pTransport, &pTransport->getPlot(), /*bCheckCity*/true))
		{
			continue;
		} // </K-Mod>
		UnitAITypes eTransportAI = pTransport->AI_getUnitAIType();
		if (eUnitAI != NO_UNITAI && eTransportAI != eUnitAI)
			continue;

		int iCargoSpaceAvailable = pTransport->cargoSpaceAvailable(
			getSpecialUnitType(), getDomainType());
		// advc.opt: Check for 0 space before counting TargetMissionAIs
		if (iCargoSpaceAvailable <= 0)
			continue;
		iCargoSpaceAvailable -= kOwner.AI_unitTargetMissionAIs(
			pTransport, aeLoadMissionAI, iLoadMissionAICount, getGroup());
		if (iCargoSpaceAvailable <= 0)
			continue;
		if ((ePassengerAI == NO_UNITAI ||
			pTransport->getUnitAICargo(ePassengerAI) > 0) &&
			(iMinCargo == -1 || pTransport->getCargo() >= iMinCargo))
		{	// <advc.040> Leave space for Settler and protection
			/*
			if (eTransportAI == UNITAI_SETTLER_SEA && eUnitAI == UNITAI_SETTLER_SEA &&
				(ePassengerAI == UNITAI_WORKER ||
					AI_getUnitAIType() == UNITAI_WORKER) &&
				pTransport->cargoSpace() -
				pTransport->getUnitAICargo(UNITAI_WORKER) <= 2)
			{
				continue;
			} // </advc.040>
			*/
			// Use existing count of cargo space available
			if ((iMinCargoSpace == -1 || iCargoSpaceAvailable >= iMinCargoSpace) &&
				(iMaxCargoSpace == -1 || iCargoSpaceAvailable <= iMaxCargoSpace))
			{
				if (iMaxCargoOurUnitAI == -1 ||
					pTransport->getUnitAICargo(AI_getUnitAIType()) <= iMaxCargoOurUnitAI)
				{	// <advc.046> Don't join a pickup-stranded mission
					CvUnit* u = pTransport->AI_getGroup()->AI_getMissionAIUnit();
					if (u != NULL && u->getPlot().getTeam() != getTeam() && !at(u->getPlot()))
					{
						continue;
					} // </advc.046>
					//if (!pLoopUnit->getPlot().isVisibleEnemyUnit(this)) { // advc.opt: It's our unit; enemies can't coexist.
					CvPlot* pUnitTargetPlot = pTransport->AI_getGroup()->AI_getMissionAIPlot();
					if (pUnitTargetPlot == NULL || pUnitTargetPlot->getTeam() == getTeam() ||
						(!pUnitTargetPlot->isOwned() ||
							!AI_isPotentialEnemyOf(pUnitTargetPlot->getTeam(), *pUnitTargetPlot)))
					{
						int iPathTurns = 0;
						if (at(pTransport->getPlot()) ||
							generatePath(&pTransport->getPlot(), eFlags, true,
								&iPathTurns, iMaxPath))
						{
							// prefer a transport that can hold as much of our group as possible
							int iValue = 5 * std::max(0,
								iCurrentGroupSize - iCargoSpaceAvailable) + iPathTurns;
							if (iValue < iBestValue)
							{
								iBestValue = iValue;
								pBestUnit = pTransport;
							}
						}
					}
				}
			}
		}
	}
	return pBestUnit;
} // K-Mod end

// K-Mod. A simple hash of the unit's birthmark.
/*	This is to be used for getting a 'random' number which depends on the unit
	but which does not vary from turn to turn. */
unsigned CvUnitAI::AI_unitBirthmarkHash(int iExtra) const
{
	unsigned iHash = AI_getBirthmark() + iExtra;
	iHash *= 2654435761; // golden ratio of 2^32;
	return iHash;
}

// another 'random' hash, but which depends on a particular plot
unsigned CvUnitAI::AI_unitPlotHash(const CvPlot* pPlot, int iExtra) const
{
	return AI_unitBirthmarkHash(pPlot->plotNum() + iExtra);
} // K-Mod end

/*  <advc.033> Counts units in kPlot that this unit could attack and returns
	the defender count and total unit count as a pair (iDefenders,iUnits). */
std::pair<int, int> CvUnitAI::AI_countPiracyTargets(CvPlot const& kPlot,
	bool bStopIfAnyTarget) const
{
	std::pair<int, int> iiDefTotal(0, 0);
	if (!isAlwaysHostile(&kPlot))
		//|| !kPlot.isVisible(getTeam(), false))// This is handled by searchRange
	{
		return iiDefTotal;
	}
	FOR_EACH_UNIT_ON_PLOT(&kPlot)
	{
		if (pLoopUnit->isInvisible(getTeam(), false))
			continue;
		// WTP: consider all players to be piracy targets
		/*
		if (!GET_PLAYER(getOwner()).AI_isPiracyTarget(pLoopUnit->getOwner()))
			continue;
		*/
		iiDefTotal.second++;
		if (bStopIfAnyTarget)
			return iiDefTotal;
		if (pLoopUnit->canDefend())
			iiDefTotal.first++;
	}
	return iiDefTotal;
}

/*	K-Mod: Decide whether or not this group is stranded.
	If they are stranded, try to walk towards the coast.
	If we're on the coast, wait to be rescued! */
bool CvUnitAI::AI_handleStranded(MovementFlags eFlags)
{
	PROFILE_FUNC();

	// <advc.001> No place to go
	if (GET_PLAYER(getOwner()).getNumCities() <= 0)
	{
		getGroup()->pushMission(MISSION_SKIP);
		return true;
	} // </advc.001>

	if (isCargo())
	{	// This is possible, in some rare cases, but I'm currently trying to pin down precisely what those cases are.
		//FErrorMsg("AI_handleStranded: this unit is already cargo."); // advc.006
		getGroup()->pushMission(MISSION_SKIP);
		return true;
	}

	if (isHuman())
		return false;

	const CvPlayerAI& kOwner = GET_PLAYER(getOwner());

	// return false if the group is not stranded.
	/*
	int iDummy = -1;
	if (area()->getNumAIUnits(getOwner(), UNITAI_SETTLE) > 0 &&
		kOwner.AI_getNumAreaCitySites(getArea(), iDummy) > 0)
	{
		return false;
	}
	*/

	const CvArea& kArea = *area();
	if (kArea.getNumCities() > 0)
	{
		//if (getPlot().getTeam() == getTeam())
		/*  advc.046: Don't see what good ownership of a teammate will do.
			Really need a path to one of our own cities. But, to save time,
			let's check (though rival borders could block the path): */
		if (getPlot().getOwner() == getOwner() &&
			kArea.getCitiesPerPlayer(getOwner()) > 0)
		{
			return false;
		}
		if (AI_getGroup()->AI_isHasPathToAreaPlayerCity(getOwner(), eFlags))
			return false;
		if (canFight() &&
			AI_getGroup()->AI_isHasPathToAreaEnemyCity(true, eFlags))
		{
			return false;
		}
	}

	// ok.. so the group is stranded.
	// Try to get to the coast.
	/*  advc.001: iMinWaterSize argument added to all three isCoastalLand checks in
		this function. Reaching a lake isn't good enough. */
	if (!getPlot().isCoastalLand(-1))
	{
		// maybe we were already on our way?
		CvPlot const* pMissionPlot = NULL;
		CvPlot const* pEndTurnPlot = NULL;
		if (AI_getGroup()->AI_getMissionAIType() == MISSIONAI_STRANDED)
		{
			pMissionPlot = AI_getGroup()->AI_getMissionAIPlotInternal();
			if (pMissionPlot != NULL && pMissionPlot->isCoastalLand(-1) &&
				!pMissionPlot->isVisibleEnemyUnit(this) &&
				generatePath(pMissionPlot, eFlags, true))
			{
				// The current mission looks good enough. Don't bother searching for a better option.
				pEndTurnPlot = getPathEndTurnPlot();
			}
			else
			{
				// the current mission plot is not suitable. We'll have to search.
				pMissionPlot = 0;
			}
		}
		if (!pMissionPlot)
		{
			// look for the clostest coastal plot in this area
			int iShortestPath = MAX_INT;

			for (int i = 0; i < GC.getMap().numPlots(); i++)
			{
				CvPlot const& kLoopPlot = GC.getMap().getPlotByIndex(i);
				if (kLoopPlot.isArea(kArea) && kLoopPlot.isCoastalLand(-1))
				{
					// TODO: check that the water isnt' blocked by ice.
					// advc.030 (comment): ^Should be guaranteed by pLoopPlot->isArea(getArea()) now
					int iPathTurns;
					if (generatePath(&kLoopPlot, eFlags, true, &iPathTurns, iShortestPath))
					{
						FAssert(iPathTurns <= iShortestPath);
						iShortestPath = iPathTurns;
						pEndTurnPlot = getPathEndTurnPlot();
						pMissionPlot = &kLoopPlot;
						if (iPathTurns <= 1)
							break;
					}
				}
			}
		}

		if (pMissionPlot != NULL)
		{
			pushGroupMoveTo(*pEndTurnPlot, eFlags, false, false,
				MISSIONAI_STRANDED, (CvPlot*)pMissionPlot); //TODO: Fix constness!
			return true;
		}
	}

	/*	Hopefully we're on the coast.
		(but we might not be - if we couldn't find a path to the coast)
		try to load into a passing boat
		Calling AI_load will check all of our boats; so before we do that,
		I'm going to just see if there are any boats on adjacent plots. */
	FOR_EACH_ADJ_PLOT(plot(),
	{
		//if (canLoadOntoAnyUnit(*pAdjacentPlot))
		if (canLoad(pAdjacentPlot, false))
		{
			/*	ok. there is something we can load into - but lets use the
				(slow) official function to actually issue the load command. */
			if (AI_load(NO_UNITAI, NO_MISSIONAI, NO_UNITAI, -1, -1, -1, -1, eFlags, 1))
				return true;
			else // if that didn't do it, nothing will
				break;
		}
	}) //FOR_EACH

	// raise the 'stranded' flag, and wait to be rescued.
	getGroup()->pushMission(MISSION_SKIP, -1, -1, NO_MOVEMENT_FLAGS,
		false, false, MISSIONAI_STRANDED, plot());
	return true;
}

void CvUnitAI::AI_cityDefenseMove()
{
	PROFILE_FUNC();

	if (isCargo())
	{
		AI_handleEmbarkedMilitary();
		return;
	}

	// BETTER_BTS_AI_MOD, Unit AI, Efficiency, 08/20/09, jdog5000: was AI_getPlotDanger
	bool const bDanger = (GET_PLAYER(getOwner()).AI_isAnyPlotDanger(getPlot(), 3));

	// BETTER_BTS_AI_MOD, Settler AI, 09/18/09, jdog5000: START
	if (!getPlot().isOwned())
	{
		if (AI_group(UNITAI_SETTLER, 1, -1, -1, false, false, false, 2, true))
		{
			return;
		}
	} // BETTER_BTS_AI_MOD: END

	if (bDanger)
	{
		if (AI_leaveAttack(1, 70, 140)) // was ,,175
		{
			return;
		}

		if (AI_chokeDefend())
		{
			return;
		}
	}

	if (AI_guardCityBestDefender())
	{
		return;
	}

	/*x
	if (!bDanger)
	{
		if (getPlot().getOwner() == getOwner())
		{
			if (AI_load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_SETTLE,
				-1, -1, 1, -1, MOVE_SAFE_TERRITORY, 1))
			{
				return;
			}
		}
	}
	*/

	if (AI_guardCityMinDefender(true))
	{
		return;
	}

	if (AI_guardCity(true))
	{
		return;
	}

	if (!bDanger)
	{
		if (AI_group(UNITAI_SETTLER, /*iMaxGroup*/ 1, -1, -1, false, false, false,
			/*iMaxPath*/ 2, /*bAllowRegrouping*/ true))
		{
			return;
		}

		if (AI_group(UNITAI_SETTLER, /*iMaxGroup*/ 2, -1, -1, false, false, false,
			/*iMaxPath*/ 2, /*bAllowRegrouping*/ true))
		{
			return;
		}

		/*
		if (getPlot().getOwner() == getOwner())
		{
			if (AI_load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_SETTLE,
				-1, -1, 1, -1, MOVE_SAFE_TERRITORY))
			{
				return;
			}
		}
		*/
	}

	const CvArea& kArea = *area();
	AreaAITypes eAreaAI = kArea.getAreaAIType(getTeam());
	if (eAreaAI == AREAAI_ASSAULT || eAreaAI == AREAAI_ASSAULT_MASSING ||
		eAreaAI == AREAAI_ASSAULT_ASSIST)
	{
		if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, UNITAI_ATTACK_CITY,
			-1, -1, -1, 0, MOVE_SAFE_TERRITORY))
		{
			return;
		}
	}

	if ((AI_getBirthmark() % 4) == 0)
	{
		if (AI_guardFort())
		{
			return;
		}
	}

	/*
	if (AI_guardCityAirlift())
	{
		return;
	}
	*/

	if (AI_guardCity(false, true, 1))
	{
		return;
	}

#if 0
	if (getPlot().getOwner() == getOwner())
	{
		if (AI_load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_SETTLE,
			3, -1, -1, -1, MOVE_SAFE_TERRITORY))
		{
			// will enter here if in danger
			return;
		}
	}
#endif
	// BETTER_BTS_AI_MOD, City AI, 04/02/10, jdog5000: join any city attacks in progress
	/*if (getPlot().getOwner() != getOwner()) {
		if (AI_groupMergeRange(UNITAI_ATTACK_CITY, 1, true, true))
			return;
	}*/ // disabled by K-Mod (how often do you think this is going to help us?)

	if (AI_guardCity(false, true))
	{
		return;
	}

	// BETTER_BTS_AI_MOD, Unit AI, 03/04/10, jdog5000: START
	if (!isBarbarian() && (kArea.getAreaAIType(getTeam()) == AREAAI_OFFENSIVE ||
		kArea.getAreaAIType(getTeam()) == AREAAI_MASSING))
	{
		bool bIgnoreFaster = false;
		if (GET_PLAYER(getOwner()).AI_isDoStrategy(AI_STRATEGY_LAND_BLITZ))
		{
			if (kArea.getAreaAIType(getTeam()) != AREAAI_ASSAULT)
			{
				bIgnoreFaster = true;
			}
		}

		if (AI_group(UNITAI_ATTACK_CITY, -1, 2, 4, bIgnoreFaster))
		{
			return;
		}
	}

	if (kArea.getAreaAIType(getTeam()) == AREAAI_ASSAULT)
	{
		if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, UNITAI_ATTACK_CITY,
			2, -1, -1, 1, MOVE_SAFE_TERRITORY))
		{
			return;
		}
	}
	// BETTER_BTS_AI_MOD: END

	if (AI_retreatToCity())
	{
		return;
	}

	// K-Mod
	if (AI_handleStranded())
		return;
	// K-Mod end

	if (AI_safety())
	{
		return;
	}

	getGroup()->pushMission(MISSION_SKIP);
}

bool CvUnitAI::AI_guardCityMinDefender(bool bSearch)
{
	PROFILE_FUNC();

	CvCityAI const* pPlotCity = getPlot().AI_getPlotCity();
	if (pPlotCity != NULL && pPlotCity->getOwner() == getOwner())
	{
		/*int iCityDefenderCount = pPlotCity->getPlot().plotCount(PUF_isUnitAIType, UNITAI_CITY_DEFENSE, -1, getOwner());
		if ((iCityDefenderCount - 1) < pPlotCity->AI_minDefenders()) {
			if ((iCityDefenderCount <= 2) || (SyncRandSuccessRatio(4, 5))) {
				getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_GUARD_CITY, NULL);
				return true;
			}
		}*/ // BtS
		// K-Mod
		// Note. For this check, we only count UNITAI_CITY_DEFENSE. But in the bSearch case, we count all guard_city units.
		int iDefendersHave = getPlot().plotCount(PUF_isMissionAIType, MISSIONAI_GUARD_CITY,
			-1, getOwner(), NO_TEAM, AI_getUnitAIType() == UNITAI_CITY_DEFENSE ?
			PUF_isUnitAIType : 0, UNITAI_CITY_DEFENSE);

		if (AI_getGroup()->AI_getMissionAIType() == MISSIONAI_GUARD_CITY)
			iDefendersHave--;

		if (iDefendersHave < pPlotCity->AI_minDefenders())
		{
			if (iDefendersHave <= 1 ||
				SyncRandNum(area()->getNumAIUnits(getOwner(), UNITAI_CITY_DEFENSE) + 5) > 1)
			{
				getGroup()->pushMission(isFortifyable() ? MISSION_FORTIFY : MISSION_SKIP,
					-1, -1, NO_MOVEMENT_FLAGS, false, false, MISSIONAI_GUARD_CITY, NULL);
				return true;
			}
		}
		// K-Mod end
	}

	if (bSearch)
	{
		int iBestValue = 0;
		CvPlot* pBestPlot = NULL;
		CvPlot* pBestGuardPlot = NULL;
		FOR_EACH_CITYAI(pLoopCity, GET_PLAYER(getOwner()))
		{
			//if (!AI_plotValid(pLoopCity->plot()))
			if (!AI_canEnterByLand(*pLoopCity->area())) // advc.opt (see comment in guardCity)
				continue;

			//int iDefendersHave = pLoopCity->getPlot().plotCount(PUF_isUnitAIType, UNITAI_CITY_DEFENSE, -1, getOwner());
			// K-Mod
			int iDefendersHave = pLoopCity->getPlot().plotCount(
				PUF_isMissionAIType, MISSIONAI_GUARD_CITY, -1, getOwner());
			if (pPlotCity == pLoopCity &&
				AI_getGroup()->AI_getMissionAIType() == MISSIONAI_GUARD_CITY)
			{
				iDefendersHave--;
			}
			// K-Mod end
			int iDefendersNeed = pLoopCity->AI_minDefenders();

			if (iDefendersHave < iDefendersNeed)
			{
				//if (!pLoopCity->getPlot().isVisibleEnemyUnit(this)) // advc.opt: It's our city
				if (!pLoopCity->AI_isEvacuating()) // advc.139
				{
					iDefendersHave += GET_PLAYER(getOwner()).AI_plotTargetMissionAIsInternal(
						pLoopCity->plot(), MISSIONAI_GUARD_CITY, getGroup());
					if (iDefendersHave < iDefendersNeed + 1)
					{
						int iPathTurns;
						//if (!atPlot(pLoopCity->plot()) && generatePath(pLoopCity->plot(), 0, true, &iPathTurns))
						// <K-Mod> (also deleted "if (iPathTurns < 10)")
						if (generatePath(pLoopCity->plot(),
							NO_MOVEMENT_FLAGS, true, &iPathTurns, 10)) // </K-Mod>
						{
							/*int iValue = (iDefendersNeed - iDefendersHave) * 20;
							iValue += 2 * std::min(15, iCurrentTurn - pLoopCity->getGameTurnAcquired());
							if (pLoopCity->isOccupation())
							iValue += 5;
							iValue -= iPathTurns;*/ // BtS
							// K-Mod
							int iValue = (iDefendersNeed - iDefendersHave) * 10;
							iValue += iDefendersHave <= 0 ? 10 : 0;

							iValue += 2 * pLoopCity->getCultureLevel();
							iValue += pLoopCity->getPopulation() / 3;
							iValue += pLoopCity->isOccupation() ? 8 : 0;
							iValue -= iPathTurns;
							// K-Mod end

							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestPlot = getPathEndTurnPlot();
								pBestGuardPlot = pLoopCity->plot();
							}
						}
					}
				}
			}
		}
		if (pBestPlot != NULL)
		{
			if (at(*pBestGuardPlot))
			{
				FAssert(pBestGuardPlot == pBestPlot);
				getGroup()->pushMission(isFortifyable() ? MISSION_FORTIFY : MISSION_SKIP,
					-1, -1, NO_MOVEMENT_FLAGS, false, false, MISSIONAI_GUARD_CITY, NULL);
				return true;
			}
			pushGroupMoveTo(*pBestPlot, NO_MOVEMENT_FLAGS, false, false,
				MISSIONAI_GUARD_CITY, pBestGuardPlot);
			return true;
		}
	}

	return false;
}

bool CvUnitAI::AI_isCargoOnCivilianTransport() const
{
	return isCargo() && (getTransportUnit()->AI_getUnitAIType() == UNITAI_TRANSPORT_SEA ||
		getTransportUnit()->AI_getUnitAIType() == UNITAI_TRANSPORT_COAST);
}

// This function has been heavily edited by K-Mod
void CvUnitAI::AI_attackMove()
{
	PROFILE_FUNC();

	if (isCargo())
	{
		AI_handleEmbarkedMilitary();
		return;
	}
	/*
	else {
		CvCity* const pCity = plot()->getPlotCity();
		if (pCity != NULL)
			unload(); // this checks canUnload internally
	}
	*/

	CvPlayerAI const& kOwner = GET_PLAYER(getOwner()); // K-Mod
	bool const bDanger = (kOwner.AI_isAnyPlotDanger(getPlot(), 3));
	const CvArea& kArea = *area();
	bool const bLandWar = kOwner.AI_isLandWar(kArea); // K-Mod

	// K-Mod note. We'll split the group up later if we need to. (bbai group splitting code deleted.)
	FAssert(getGroup()->countNumUnitAIType(UNITAI_ATTACK_CITY) == 0); // K-Mod. (I'm pretty sure this can't happen.)

	// Attack choking units
	// K-Mod (bbai code deleted)
	if (getPlot().getTeam() == getTeam() &&
		(bDanger || kArea.getAreaAIType(getTeam()) != AREAAI_NEUTRAL))
	{
		if (bDanger && getPlot().isCity())
		{
			if (AI_leaveAttack(2, 55, 105))
				return;
		}
		else
		{
			if (AI_defendTerritory(70, NO_MOVEMENT_FLAGS, 2, true))
				return;
		}
	}
	// K-Mod end

	{
		PROFILE("CvUnitAI::AI_attackMove() 1");

		// Guard a city we're in if it needs it
		if (AI_guardCity(true))
		{
			return;
		}

		/* if (!getPlot().isOwned()) {
			// Group with settler after naval drop
			if (AI_groupMergeRange(UNITAI_SETTLE, 2, true, false, false))
				return;
		} */ // disabled by K-Mod. This is redundant.

		if (!getPlot().isOwned() || getPlot().getOwner() == getOwner())
		{
			if (kArea.getCitiesPerPlayer(getOwner()) >
				kOwner.AI_totalAreaUnitAIsInternal(kArea, UNITAI_CITY_DEFENSE))
			{
				// Defend colonies in new world
				//if (AI_guardCity(true, true, 3))
				// <K-Mod>
				if (getGroup()->getNumUnits() == 1 ?
					AI_guardCityMinDefender(true) :
					AI_guardCity(true, true, 3)) // </K-Mod>
				{
					return;
				}
			}
		}

		if (AI_heal(30, 1))
		{
			return;
		}

		/* bts code (with omniGroup subbed in.)
		if (!bDanger) {
			//if (AI_group(UNITAI_SETTLE, 1, -1, -1, false, false, false, 3, true))
			if (AI_omniGroup(UNITAI_SETTLE, 1, -1, false, 0, 3, true, false))
				return;
			//if (AI_group(UNITAI_SETTLE, 2, -1, -1, false, false, false, 3, true))
			if (AI_omniGroup(UNITAI_SETTLE, 2, -1, false, 0, 3, false, false))
				return;
		}*/
		// K-Mod
		if (AI_omniGroup(UNITAI_SETTLER, 2, -1, false, NO_MOVEMENT_FLAGS,
			3, false, false, false, false, false))
		{
			return;
		} // K-Mod end

		if (AI_guardCity(false, true, 1))
		{
			return;
		}

		//join any city attacks in progress
		/*if (getPlot().isOwned() && getPlot().getOwner() != getOwner()) {
			if (AI_groupMergeRange(UNITAI_ATTACK_CITY, 1, true, true))
				return;
		}*/ // BtS
		// K-Mod
		if (isEnemy(getPlot()))
		{
			if (AI_omniGroup(UNITAI_ATTACK_CITY, -1, -1, true, NO_MOVEMENT_FLAGS,
				2, true, false))
			{
				return;
			}
		} // K-Mod end

		const AreaAITypes eAreaAIType = kArea.getAreaAIType(getTeam());

		if (bDanger)
		{
			// K-Mod
			if (getGroup()->getNumUnits() > 1 &&
				AI_stackVsStack(3, 110, 65, NO_MOVEMENT_FLAGS))
			{
				return;
			} // K-Mod end

			/*if (AI_cityAttack(1, 55))
				return;
			if (AI_anyAttack(1, 65))
				return;*/ // BtS
		}
		// K-Mod (moved from below, and replacing the disabled stuff above)
		if (AI_anyAttack(1, 70))
		{
			return;
		}
		// K-Mod end

		if (!noDefensiveBonus())
		{
			if (AI_guardCity(false, false))
			{
				return;
			}
		}

		if (!bDanger)
		{
			if (getPlot().getTeam() == getTeam()) // cdtw.9
			{
				bool bAssault = ((eAreaAIType == AREAAI_ASSAULT) || (eAreaAIType == AREAAI_ASSAULT_MASSING) || (eAreaAIType == AREAAI_ASSAULT_ASSIST));
				if (bAssault)
				{
					if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, UNITAI_ATTACK_CITY, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 4))
					{
						return;
					}
				}

				/*
				if (AI_load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, UNITAI_SETTLE, -1, -1, -1, 1, MOVE_SAFE_TERRITORY, 3))
				{
					return;
				}
				*/
				
				//bool bLandWar = ((eAreaAIType == AREAAI_OFFENSIVE) || (eAreaAIType == AREAAI_DEFENSIVE) || (eAreaAIType == AREAAI_MASSING));
				if (!bLandWar)
				{
					// Fill transports before starting new one, but not just full of our unit ai
					if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, 1, -1, -1, 1, MOVE_SAFE_TERRITORY, 4))
					{
						return;
					}

					// Pick new transport which has space for other unit ai types to join
					if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, 2, -1, -1, MOVE_SAFE_TERRITORY, 4))
					{
						return;
					}
				}

				if (kOwner.AI_isAnyUnitTargetMissionAI(*this, MISSIONAI_GROUP))
				{
					getGroup()->pushMission(MISSION_SKIP);
					return;
				}
			}
		}

		// Allow larger groups if outside territory
		if (getGroup()->getNumUnits() < 3)
		{
			if (getPlot().isOwned() && GET_TEAM(getTeam()).isAtWar(getPlot().getTeam()))
			{
				//if (AI_groupMergeRange(UNITAI_ATTACK, 1, true, true, true))
				if (AI_omniGroup(UNITAI_ATTACK, 3, -1, false, NO_MOVEMENT_FLAGS,
					1, true, false, true, false, false))
				{
					return;
				}
			}
		}

		if (AI_goodyRange(3, /*bIgnoreCity*/true))
		{
			return;
		}

		/* moved up
		if (AI_anyAttack(1, 70))
			return;*/
	}

	{
		PROFILE("CvUnitAI::AI_attackMove() 2");

		if (bDanger)
		{
			// K-Mod. This block has been rewritten. (original code deleted)

			// slightly more reckless than last time
			if (getGroup()->getNumUnits() > 1 &&
				AI_stackVsStack(3, 90, 40, NO_MOVEMENT_FLAGS))
			{
				return;
			}
			bool bAggressive = (kArea.getAreaAIType(getTeam()) != AREAAI_DEFENSIVE ||
				getGroup()->getNumUnits() > 1 || getPlot().getTeam() != getTeam());

			if (bAggressive && AI_pillageRange(1))
				return;

			if (getPlot().getTeam() == getTeam())
			{
				if (AI_defendTerritory(55, NO_MOVEMENT_FLAGS, 2, true))
				{
					return;
				}
			}
			else if (AI_anyAttack(1, 45))
			{
				return;
			}

			if (bAggressive && AI_pillageRange(3))
			{
				return;
			}

			if (getGroup()->getNumUnits() < 4 && isEnemy(getPlot()))
			{
				if (AI_choke(1))
				{
					return;
				}
			}

			if (bAggressive && AI_anyAttack(3, 40))
				return;
		}

		if (!isEnemy(getPlot()))
		{
			if (AI_heal())
			{
				return;
			}
		}

		//if ((kOwner.AI_getNumAIUnits(UNITAI_CITY_DEFENSE) > 0) || (GET_TEAM(getTeam()).getAtWarCount(true) > 0))
		// <K-Mod>
		if (!getPlot().isCity() ||
			getPlot().plotCount(PUF_isUnitAIType, UNITAI_CITY_DEFENSE, -1, getOwner()) > 0)
			// </K-Mod>
		{
			// BBAI TODO: If we're fast, maybe shadow an attack city stack and pillage off of it

			bool bIgnoreFaster = false;
			if (kOwner.AI_isDoStrategy(AI_STRATEGY_LAND_BLITZ))
			{
				if (kArea.getAreaAIType(getTeam()) != AREAAI_ASSAULT)
				{
					bIgnoreFaster = true;
				}
			}

			//if (AI_group(UNITAI_ATTACK_CITY, 1, 1, -1, bIgnoreFaster, true, true, 5))
			// K-Mod
			bool bAttackCity = (bLandWar &&
				(kArea.getAreaAIType(getTeam()) == AREAAI_OFFENSIVE ||
					(AI_getBirthmark() + GC.getGame().getGameTurn() / 8) % 5 <= 1));
			if (bAttackCity)
			{
				// strong merge strategy
				if (AI_omniGroup(UNITAI_ATTACK_CITY, -1, -1, true, NO_MOVEMENT_FLAGS,
					5, true, getGroup()->getNumUnits() < 2, bIgnoreFaster, false, false))
				{
					return;
				}
			}
			else
			{
				// weak merge strategy
				if (AI_omniGroup(UNITAI_ATTACK_CITY, -1, 2, true, NO_MOVEMENT_FLAGS,
					5, true, false, bIgnoreFaster, false, false))
				{
					return;
				}
			}
			// K-Mod end

			//if (AI_group(UNITAI_ATTACK, 1, 1, -1, true, true, false, 4))
			if (AI_omniGroup(UNITAI_ATTACK, 2, -1, false, NO_MOVEMENT_FLAGS,
				4, true, true, true, true, false))
			{
				return;
			}

			// BBAI TODO: Need group to be fast, need to ignore slower groups
			//if (GET_PLAYER(getOwner()).AI_isDoStrategy(AI_STRATEGY_FASTMOVERS))
			//{
			//	if (AI_group(UNITAI_ATTACK, /*iMaxGroup*/ 4, /*iMaxOwnUnitAI*/ 1, -1, true, false, false, /*iMaxPath*/ 3))
			//	{
			//		return;
			//	}
			//}

			//if (AI_group(UNITAI_ATTACK, 1, 1, -1, true, false, false, 1))
			if (AI_omniGroup(UNITAI_ATTACK, 1, 1, false, NO_MOVEMENT_FLAGS,
				1, true, true, false, false, false))
			{
				return;
			}

			// K-Mod. If we're feeling aggressive, then try to get closer to the enemy.
			if (bAttackCity && getGroup()->getNumUnits() > 1)
			{
				/*  advc.001t (Tbd.?): Maybe check CvSelectionGroupAI::AI_isDeclareWar
					and pass MOVE_DECLARE_WAR if true */
				if (AI_goToTargetCity(NO_MOVEMENT_FLAGS, 12))
					return;
			}
			// K-Mod end
		}

		/*if (getArea().getAreaAIType(getTeam()) == AREAAI_OFFENSIVE) {
			if (getGroup()->getNumUnits() > 1) {
				//if (AI_targetCity())
				if (AI_goToTargetCity(0, 12))
					return;
			}
		}*/ // BtS - disabled by K-Mod. (moved / changed)
		/* BBAI code
		else if (getArea().getAreaAIType(getTeam()) != AREAAI_DEFENSIVE) {
			if (getArea().getCitiesPerPlayer(BARBARIAN_PLAYER) > 0) {
				if (getGroup()->getNumUnits() >= GC.getInfo(GC.getGame().getHandicapType()).getBarbarianInitialDefenders()) {
					if (AI_goToTargetBarbCity(10))
						return;
				}
			}
		} */ // disabled by K-Mod. attack groups are currently limited to 2 units anyway. This test will never pass.

		if (AI_guardCity(false, true, 3))
		{
			return;
		}

		if (kOwner.getNumCities() > 1 && getGroup()->getNumUnits() == 1)
		{
			if (kArea.getAreaAIType(getTeam()) != AREAAI_DEFENSIVE)
			{
				if (kArea.getNumUnrevealedTiles(getTeam()) > 0)
				{
					if (kOwner.AI_areaMissionAIs(kArea, MISSIONAI_EXPLORE, getGroup()) <
						kOwner.AI_neededExplorers(kArea) + 1)
					{
						if (AI_exploreRange(3))
						{
							return;
						}

						if (AI_explore())
						{
							return;
						}
					}
				}
			}
		}

		if (AI_defendTerritory(45, NO_MOVEMENT_FLAGS, 7)) // K-Mod
		{
			return;
		}

		if (!bDanger && kArea.getAreaAIType(getTeam()) != AREAAI_DEFENSIVE)
		{
			if (getPlot().getTeam() == getTeam()) // cdtw.9
			{
				if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, 1, -1, -1, 1, MOVE_SAFE_TERRITORY, 4))
				{
					return;
				}

				if (GET_TEAM(getTeam()).getNumWars() > 0 && !AI_getGroup()->AI_isHasPathToAreaEnemyCity())
				{
					if (AI_load(UNITAI_ASSAULT_SEA, MISSIONAI_LOAD_ASSAULT, NO_UNITAI, -1, -1, -1, -1, MOVE_SAFE_TERRITORY, 4))
					{
						return;
					}
				}
			}
		}

		// K-Mod
		if (getGroup()->getNumUnits() >= 4 && getPlot().getTeam() == getTeam())
		{
			CvSelectionGroup* pRemainderGroup = NULL;
			CvSelectionGroup* pSplitGroup = getGroup()->splitGroup(2, 0, &pRemainderGroup);
			if (pSplitGroup != NULL)
				pSplitGroup->pushMission(MISSION_SKIP);
			if (pRemainderGroup != NULL)
			{
				CvSelectionGroupAI& kRemainderGroup = pRemainderGroup->AI(); // advc.003u
				if (kRemainderGroup.AI_isForceSeparate())
					kRemainderGroup.AI_separate();
				else kRemainderGroup.pushMission(MISSION_SKIP);
			}
			return;
		}
		// K-Mod end

		if (AI_defend())
		{
			return;
		}

		if (AI_travelToUpgradeCity())
		{
			return;
		}

		// K-Mod
		if (AI_handleStranded())
			return;
		// K-Mod end

		/* if (!bDanger && !isHuman() && getPlot().isCoastalLand() && kOwner.AI_unitTargetMissionAIs(this, MISSIONAI_PICKUP) > 0) {
			// If no other desirable actions, wait for pickup
			getGroup()->pushMission(MISSION_SKIP);
			return;
		} */ // disabled by K-Mod. We don't need this.

		if (AI_patrol())
		{
			return;
		}

		if (AI_retreatToCity())
		{
			return;
		}

		if (AI_safety())
		{
			return;
		}
	}

	getGroup()->pushMission(MISSION_SKIP);
}

// TODO: Finish this stub function
void CvUnitAI::considerCivilianDuty()
{
	bool bDanger = false;

	if (!isHuman() && getProfession() != NO_PROFESSION)
	{
		CvCity* pCity = plot()->getPlotCity();
		if ((pCity != NULL) && (pCity->getOwnerINLINE() == getOwnerINLINE()))
		{
			if (isCargo())
			{
				if (AI_allowedToJoin(pCity))
				{
					pCity->addPopulationUnit(this, NO_PROFESSION);
					return;
				}
			}

			if (!bDanger && ((getFortifyTurns() == 0) || (GC.getGameINLINE().getSorenRandNum(10, "AI better job") == 0)))
			{
				if (AI_betterJob())
				{
					return;
				}
				if (canClearSpecialty())
				{
					clearSpecialty();
					return;
				}
			}
		}
	}
}

// Deals with military units that are embarked on civilian vessels (as opposed to assault vessels)
void CvUnitAI::AI_handleEmbarkedMilitary()
{
	FAssertMsg(AI_isCargoOnCivilianTransport(), "Military unit not on expected vessel type!");

	// Skip our turn If the transport is already on a mission 
	if (getTransportUnit()->getGroup()->AI_getMissionAIType() != NO_MISSIONAI)
	{
		getGroup()->pushMission(MISSION_SKIP);
		return;
	}

	if (AI_cargoGuardCityMinDefender())
		return;

	if (AI_unloadWhereNeeded(INT_MAX))
		return;

	if (plot()->isCity())
	{
		unload(); // checks canUnload internally
	}

	getGroup()->pushMission(MISSION_SKIP);
	return;
}


bool CvUnitAI::AI_settlerSeaTransport(int iMinFoundValue)
{
	PROFILE_FUNC();

	FAssert(isCargo());

	CvUnit* const pTransportUnit = getTransportUnit();
	const CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	int iBestValue = 0;
	CvPlot* pEndTurnPlot = NULL;
	CvPlot* pDestinationPlot = NULL;
	
	for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);
		if (pLoopPlot->isRevealed(getTeam(), false) && ((pTransportUnit != NULL) || AI_plotValid(pLoopPlot)))
		{
			int iValue = AI_foundValue(pLoopPlot); // checks canFound internally
			if (iValue <= iMinFoundValue)
				continue;
			if (!pLoopPlot->isCoastalLand())
				continue;
			if (pLoopPlot->isVisibleEnemyUnit(this))
				continue;
			if (kOwner.AI_isAnyPlotTargetMissionAI(*pLoopPlot, MISSIONAI_FOUND, getGroup()))
				continue;
			if (!canMoveInto(*pLoopPlot))
				continue;
			
			int iPathTurns = 0;
			bool bTransport = false;

			if (pTransportUnit->generatePath(pLoopPlot, NO_MOVEMENT_FLAGS, false, &iPathTurns))
			{
				iValue *= 1000;
				iValue /= (2 + iPathTurns);
				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pEndTurnPlot = getPathEndTurnPlot();
					pDestinationPlot = pLoopPlot;
				}
			}
		}
	}

	if (pEndTurnPlot != NULL && pDestinationPlot != NULL)
	{
		FAssert(!pEndTurnPlot->isImpassable() || canMoveImpassable());

		// Check if we can reach the destination plot on this turn
		if (pEndTurnPlot == pDestinationPlot || ::stepDistance(pEndTurnPlot, pDestinationPlot) == 1)
		{
			if (at(*pDestinationPlot))
			{	
				FAssert(pEndTurnPlot->isCity());
				// We're in a city, unload so the cargo can take its turn (unload will set the transported units priority above 
				// the transport so they can move this turn)
				pTransportUnit->getGroup()->unloadAll();
				return true;
			}
			else if (at(*pEndTurnPlot) && !pTransportUnit->canMove() && canMove())
			{
				// We're adjacent to the destination plot so let's disembark if we can move. Note: MOVE_IGNORE_DANGER will ensure that the move does not get cancelled
				// once the automission starts
				getGroup()->pushMission(MISSION_MOVE_TO, pDestinationPlot->getX_INLINE(), pDestinationPlot->getY_INLINE(),
					MOVE_IGNORE_DANGER, false, false, MISSIONAI_FOUND, pDestinationPlot);
			}
			else
			{	
				// We can reach the destination with an amfib move this turn
				pTransportUnit->getGroup()->pushMission(MISSION_MOVE_TO, pDestinationPlot->getX_INLINE(), pDestinationPlot->getY_INLINE(),
					NO_MOVEMENT_FLAGS, false, false, MISSIONAI_FOUND, pDestinationPlot);
				getGroup()->pushMission(MISSION_SKIP);
				return true;
			}
		}
		else
		{
			pTransportUnit->getGroup()->pushMission(MISSION_MOVE_TO, pEndTurnPlot->getX_INLINE(), pEndTurnPlot->getY_INLINE(),
				NO_MOVEMENT_FLAGS, false, false, MISSIONAI_FOUND, pDestinationPlot);
			getGroup()->pushMission(MISSION_SKIP);
			return true;
		}
	}
	return false;
}

bool CvUnitAI::AI_colonistSeaTransport()
{
	return false;
}

#if 0
// advc.040:
bool CvUnitAI::AI_moveSettlerToCoast(int iMaxPathTurns)
{
	CvPlayerAI const& kOwner = GET_PLAYER(getOwner());
	CvCity* pCurrentCity = getPlot().getPlotCity();
	if (pCurrentCity == NULL)
		return false;
	int const iGroupSz = getGroupSize();
	CvCity const* pBest = NULL;
	CvPlot* pEndPlot = NULL;
	int iBest = 0;
	FOR_EACH_CITYAI(c, kOwner)
	{
		CvPlot* pCityPlot = c->plot();
		if (c == pCurrentCity || !c->isArea(getArea()) || !c->isCoastal() ||
			c->AI_isEvacuating() ||
			kOwner.AI_getPlotDanger(*pCityPlot, 3, false, iGroupSz) > iGroupSz - 1)
		{
			continue;
		}
		int iPathTurns = -1;
		if (generatePath(*pCityPlot, NO_MOVEMENT_FLAGS, true, &iPathTurns, iMaxPathTurns))
		{
			int iValue = 5 + iMaxPathTurns - iPathTurns;
			if (pCityPlot->plotCheck(PUF_isUnitAIType, UNITAI_SETTLER_SEA, -1, kOwner.getID()) != NULL)
				iValue += 100;
			if (iValue > iBest)
			{
				iBest = iValue;
				pBest = c;
				pEndPlot = &getPathEndTurnPlot();
			}
		}
	}
	if (pBest == NULL)
		return false;
	pushGroupMoveTo(*pEndPlot,
		iGroupSz > 1 ? NO_MOVEMENT_FLAGS : MOVE_SAFE_TERRITORY, false, false,
		MISSIONAI_LOAD_SETTLER, pBest->plot());
	return true;
}
#endif
