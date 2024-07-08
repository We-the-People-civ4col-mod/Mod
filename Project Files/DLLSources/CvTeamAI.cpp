// teamAI.cpp

#include "CvGameCoreDLL.h"
#include "CvTeamAI.h"
#include "CvPlayerAI.h"
#include "CvRandom.h"
#include "CvGlobals.h"
#include "CvGameCoreUtils.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvGameAI.h"
#include "CvInfos.h"
#include "FProfiler.h"
#include "CyArgsList.h"
#include "CvDLLPythonIFaceBase.h"

#include "CvSavegame.h"
#include "BetterBTSAI.h"
#include "CvGameTextMgr.h" // GAMETEXT singleton access

// statics

CvTeamAI* CvTeamAI::m_aTeams = NULL;

void CvTeamAI::initStatics()
{
	m_aTeams = new CvTeamAI[MAX_TEAMS];
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_aTeams[iI].m_eID = ((TeamTypes)iI);
	}
}

void CvTeamAI::freeStatics()
{
	SAFE_DELETE_ARRAY(m_aTeams);
}

// inlined for performance reasons
DllExport CvTeamAI& CvTeamAI::getTeamNonInl(TeamTypes eTeam)
{
	return getTeam(eTeam);
}


// Public Functions...

CvTeamAI::CvTeamAI()
{
	AI_reset();
}


CvTeamAI::~CvTeamAI()
{
	AI_uninit();
}


void CvTeamAI::AI_init()
{
	AI_reset();

	//--------------------------------
	// Init other game data
}


void CvTeamAI::AI_uninit()
{
}


void CvTeamAI::AI_reset()
{
	AI_uninit();
	AI_resetSavedData();
}

void CvTeamAI::AI_doTurnPre()
{
	AI_doCounter();
}


void CvTeamAI::AI_doTurnPost()
{
	m_strengthMemory.decay(); // advc.158

	AI_updateWorstEnemy();

	AI_updateAreaStrategies(false);
	
	AI_doTactics();

	if (isHuman())
	{
		return;
	}

	AI_doWar();
}


void CvTeamAI::AI_makeAssignWorkDirty()
{
	int iI;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				GET_PLAYER((PlayerTypes)iI).AI_makeAssignWorkDirty();
			}
		}
	}
}

// TAC - AI Assault Sea - koma13, jdog5000(BBAI) - START
// Find plot strength of teammates and potentially vassals
int CvTeamAI::AI_getOurPlotStrength(CvPlot* pPlot, int iRange, bool bDefensiveBonuses, bool bTestMoves, bool bIncludeVassals) const
{
	int iI;
	int iPlotStrength = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iPlotStrength += GET_PLAYER((PlayerTypes)iI).AI_getOurPlotStrength(pPlot,iRange,bDefensiveBonuses,bTestMoves);
			}
		}
	}

	return iPlotStrength;
}
// TAC - AI Assault Sea - koma13, jdog5000(BBAI) - END

void CvTeamAI::AI_updateAreaStrategies(bool bTargets)
{
	CvArea* pLoopArea;
	int iLoop;

	if (!(GC.getGameINLINE().isFinalInitialized()))
	{
		return;
	}

	for(pLoopArea = GC.getMap().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop))
	{
		pLoopArea->setAreaAIType(getID(), AI_calculateAreaAIType(*pLoopArea));
	}

	if (bTargets)
	{
		AI_updateAreaTargets();
	}
}


void CvTeamAI::AI_updateAreaTargets()
{
	int iI;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				GET_PLAYER((PlayerTypes)iI).AI_updateAreaTargets();
			}
		}
	}
}

int CvTeamAI::AI_countMilitaryWeight(CvArea const* pArea) const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iCount += GET_PLAYER((PlayerTypes)iI).AI_militaryWeight(pArea);
			}
		}
	}

	return iCount;
}


bool CvTeamAI::AI_isAnyCapitalAreaAlone() const
{
	int iI;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				if (GET_PLAYER((PlayerTypes)iI).AI_isCapitalAreaAlone())
				{
					return true;
				}
			}
		}
	}

	return false;
}


bool CvTeamAI::AI_isPrimaryArea(CvArea const& kArea) const
{
	int iI;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				if (GET_PLAYER((PlayerTypes)iI).AI_isPrimaryArea(kArea))
				{
					return true;
				}
			}
		}
	}

	return false;
}


bool CvTeamAI::AI_hasCitiesInPrimaryArea(TeamTypes eTeam) const
{
	CvArea* pLoopArea;
	int iLoop;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	for(pLoopArea = GC.getMap().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop))
	{
		if (AI_isPrimaryArea(*pLoopArea))
		{
			if (GET_TEAM(eTeam).countNumCitiesByArea(*pLoopArea))
			{
				return true;
			}
		}
	}

	return false;
}


/*  advc.104s (note): If UWAI is enabled, AI_doWar may adjust (i.e. overwrite) the
	result of this calculation through UWAI::Team::alignAreaAI. */
AreaAITypes CvTeamAI::AI_calculateAreaAIType(CvArea const& kArea, bool bPreparingTotal) const
{
	PROFILE_FUNC();

	if (kArea.isWater())
		return AREAAI_NEUTRAL; // K-Mod (no functional change)

	// WTP: Keeping the old (refactored) code for the natives
	if (hasNativePlayer())
	{
		int iBestValue = 0;
		WarPlanTypes eBestWarplan = NO_WARPLAN;
		TeamTypes eBestTeam = NO_TEAM;

		//Trying new code for natives only.
		for (TeamTypes eTeam = FIRST_TEAM; eTeam < MAX_TEAMS; ++eTeam)
		{
			if (GET_TEAM(eTeam).isAlive())
			{
				const WarPlanTypes eWarplan = AI_getWarPlan(eTeam);
				if (eWarplan != NO_WARPLAN)
				{
					FAssert(eTeam != getID());
					FAssert(isHasMet(eTeam) || GC.getGameINLINE().isOption(GAMEOPTION_ALWAYS_WAR));
					if ((GET_TEAM(eTeam).countNumCitiesByArea(kArea) > 0) || (GET_TEAM(eTeam).countNumUnitsByArea(kArea) > 4))
					{
						const int iValue = AI_warplanStrength(eWarplan);
						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							eBestWarplan = eWarplan;
							eBestTeam = eTeam;
						}
					}
				}
			}
		}

		if (eBestWarplan == WARPLAN_TOTAL || eBestWarplan == WARPLAN_PREPARING_TOTAL)
		{
			return AREAAI_OFFENSIVE;
		}

		if (eBestWarplan == WARPLAN_ATTACKED || eBestWarplan == WARPLAN_ATTACKED_RECENT)
		{
			if ((countPowerByArea(kArea) * 60) > (AI_countEnemyPowerByArea(kArea) * 100))
			{
				if (AI_getAttitude(eBestTeam) == ATTITUDE_FURIOUS)
				{
					return AREAAI_OFFENSIVE;
				}
				else
				{
					return AREAAI_BALANCED;
				}
			}
			else
			{
				return AREAAI_DEFENSIVE;
			}
		}

		if (AI_countEnemyPowerByArea(kArea) > 0)
		{
			return AREAAI_BALANCED;
		}

		return AREAAI_NEUTRAL;
	}

	// WTP: AdvCiv version used for Europeans below

	bool bRecentAttack = false;
	bool bTargets = false;
	bool bChosenTargets = false;
	bool bDeclaredTargets = false;

	bool bAssault = false;
	bool bPreparingAssault = false;

	// int iOffensiveThreshold = (bPreparingTotal ? 25 : 20); // K-Mod, I don't use this.
	int iAreaCities = countNumCitiesByArea(kArea);
	int iWarSuccessRating = AI_getWarSuccessRating(); // K-Mod

	for (TeamTypes eTeam = FIRST_TEAM; eTeam < NUM_TEAM_TYPES; ++eTeam)
	{
		const CvTeamAI& kEnemy = GET_TEAM(eTeam);
		const TeamTypes ourTeam = getID();
		if (!kEnemy.isHasMet(ourTeam) || !kEnemy.isAlive() || !kEnemy.AI_isColonialPower())
			continue;

		CvTeam const& kTarget = GET_TEAM(eTeam);
		TeamTypes const eTarget = kTarget.getID();
		if (AI_getWarPlan(eTarget) == NO_WARPLAN)
			continue;

		if (AI_getWarPlan(eTarget) == WARPLAN_ATTACKED_RECENT)
		{
			FAssert(isAtWar(eTarget));
			bRecentAttack = true;
		}

		if (kTarget.countNumCitiesByArea(kArea) > 0 &&
			//|| GET_TEAM((TeamTypes)iI).countNumUnitsByArea(kArea) > 4)
		/*  advc.104s: Replacing the above. Setting AreaAI to ASSAULT won't stop
			the AI from fighting any landed units. Need to focus on cities.
			isLandTarget makes sure that there are reachable cities. Still check
			city count for efficiency (there can be a lot of land areas to
			calculate AI types for). */
			AI_isLandTarget(eTarget))
		{
			bTargets = true;
			if (AI_isChosenWar(eTarget))
			{
				bChosenTargets = true;

				if (isAtWar(eTarget) ?
					(AI_getAtWarCounter(eTarget) < 10) :
					AI_isSneakAttackReady(eTarget))
				{
					bDeclaredTargets = true;
				}
			}
		}
		else
		{
			bAssault = true;
			if (AI_isSneakAttackPreparing(eTarget))
				bPreparingAssault = true;
		}
	}

	// K-Mod - based on idea from BBAI
	if (bTargets && iAreaCities > 0 && getNumWars() > 0)
	{
		int iPower = countPowerByArea(kArea);
		int iEnemyPower = AI_countEnemyPowerByArea(kArea);
		iPower *=
			AI_limitedWarPowerRatio() + // advc.107: was 100 flat
			// (addressing the K-Mod comment below)
			iWarSuccessRating + //(bChosenTargets ? 100 : 50)
			(bChosenTargets || !bRecentAttack ? 100 : 70); // advc.107
		iEnemyPower *= 100;
		/*  it would be nice to put some personality modifiers into this.
			But this is a Team function. :( */
		if (iPower < iEnemyPower)
			return AREAAI_DEFENSIVE;
	} // K-Mod end

	if (bDeclaredTargets)
		return AREAAI_OFFENSIVE;

	if (bTargets)
	{
		/* BBAI code. -- This code has two major problems.
		* Firstly, it makes offense more likely when we are in more wars.
		* Secondly, it chooses offense based on how many offense units we have --
		* but offense units are built for offense areas!
		*
		// AI_countMilitaryWeight is based on this team's pop and cities ...
		// if this team is the biggest, it will over estimate needed units
		int iMilitaryWeight = AI_countMilitaryWeight(&kArea);
		int iCount = 1;
		for (int iJ = 0; iJ < MAX_CIV_TEAMS; iJ++) {
			if (iJ != getID() && GET_TEAM((TeamTypes)iJ).isAlive()) {
				if (!(GET_TEAM((TeamTypes)iJ).isBarbarian() ||
						GET_TEAM((TeamTypes)iJ).isMinorCiv())) {
					if (AI_getWarPlan((TeamTypes)iJ) != NO_WARPLAN) {
						iMilitaryWeight += GET_TEAM((TeamTypes)iJ).
								AI_countMilitaryWeight(&kArea);
						iCount++;
						if (GET_TEAM((TeamTypes)iJ).isAVassal()) {
							for (int iK = 0; iK < MAX_CIV_TEAMS; iK++) {
								if (iK != getID() && GET_TEAM((TeamTypes)iK).isAlive()) {
									if (GET_TEAM((TeamTypes)iJ).isVassal((TeamTypes)iK)) {
										iMilitaryWeight += GET_TEAM((TeamTypes)iK).
												AI_countMilitaryWeight(&kArea);
		} } } } } } } }
		iMilitaryWeight /= iCount;
		if (countNumAIUnitsByArea(kArea, UNITAI_ATTACK) +
				countNumAIUnitsByArea(kArea, UNITAI_ATTACK_CITY) +
				countNumAIUnitsByArea(kArea, UNITAI_PILLAGE) +
				countNumAIUnitsByArea(kArea, UNITAI_ATTACK_AIR) >
				(iMilitaryWeight * iOffensiveThreshold) / 100 + 1)
			return AREAAI_OFFENSIVE;*/
			/*  K-Mod. I'm not sure how best to do this yet. Let me just try a rough
				idea for now. I'm using AI_countMilitaryWeight; but what I really
				want is "border territory which needs defending" */
		int iOurRelativeStrength = 100 * countPowerByArea(kArea) /
			(AI_countMilitaryWeight(&kArea) + 20);
		iOurRelativeStrength *= 100 + (bDeclaredTargets ? 30 : 0) +
			(bPreparingTotal ? -20 : 0) + iWarSuccessRating / 2;
		iOurRelativeStrength /= 100;
		int iEnemyRelativeStrength = 0;
		bool bEnemyCities = false;

		FOR_EACH_CIV_ALIVE_AND_KNOWN_POTENTIAL_ENEMY_OF(kOurTeam, kLoopTeam)
		{
			if (AI_getWarPlan(kLoopTeam.getID()) == NO_WARPLAN)
				continue;

			int iPower = 100 * kLoopTeam.countPowerByArea(kArea);
			int iCommitment = (bPreparingTotal ? 30 : 20) +
				kLoopTeam.AI_countMilitaryWeight(&kArea) *
				((isAtWar(kLoopTeam.getID()) ? 1 : 2) +
					kLoopTeam.getNumWars(/*true, true*/)) / 2;
			iPower /= iCommitment;
			iEnemyRelativeStrength += iPower;
			if (kLoopTeam.countNumCitiesByArea(kArea) > 0)
				bEnemyCities = true;
		}
		if (bEnemyCities && iOurRelativeStrength > iEnemyRelativeStrength)
			return AREAAI_OFFENSIVE;
		// K-Mod end
	}

	if (bTargets)
	{
		for (PlayerTypes ePlayer = FIRST_PLAYER; ePlayer < NUM_PLAYER_TYPES; ++ePlayer)
		{
			const CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
			if (!kPlayer.isAlive() && TEAMID(ePlayer) != getID())
				continue;

			CvPlayerAI* itMember = &GET_PLAYER(ePlayer);

			if (itMember->AI_isDoStrategy(AI_STRATEGY_DAGGER) ||
				itMember->AI_isDoStrategy(AI_STRATEGY_FINAL_WAR))
			{
				if (kArea.getCitiesPerPlayer(itMember->getID()) > 0)
					return AREAAI_MASSING;
			}
		}
		if (bRecentAttack)
		{
			int iPower = countPowerByArea(kArea);
			int iEnemyPower = AI_countEnemyPowerByArea(kArea);
			if (iPower > iEnemyPower)
				return AREAAI_MASSING;
			return AREAAI_DEFENSIVE;
		}
	}
	// advc.107: 2*iAreaCities (from MNAI)
	if (iAreaCities > 0 && AI_countEnemyDangerByArea(kArea) > 2 * iAreaCities)
		return AREAAI_DEFENSIVE;

	if (bChosenTargets)
		return AREAAI_MASSING;

	if (bTargets)
	{
		if (iAreaCities > getNumMembers() * 3)
		{
			if (GC.getGame().isOption(GAMEOPTION_AGGRESSIVE_AI) ||
				GC.getGame().isOption(GAMEOPTION_ALWAYS_WAR) ||
				(countPowerByArea(kArea) * 2 >
					AI_countEnemyPowerByArea(kArea) * 3))
			{
				return AREAAI_MASSING;
			}
		}
		return AREAAI_DEFENSIVE;
	}
	else if (bAssault)
	{
		if (AI_isPrimaryArea(kArea))
		{
			if (bPreparingAssault)
				return AREAAI_ASSAULT_MASSING;
		}
		else if (countNumCitiesByArea(kArea) > 0)
			return AREAAI_ASSAULT_ASSIST;
		return AREAAI_ASSAULT;
	}
	return AREAAI_NEUTRAL;
}

int CvTeamAI::AI_calculateAdjacentLandPlots(TeamTypes eTeam) const
{
	CvPlot* pLoopPlot;
	int iCount;
	int iI;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	iCount = 0;

	for (iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		pLoopPlot = GC.getMap().plotByIndexINLINE(iI);

		if (!(pLoopPlot->isWater()))
		{
			if ((pLoopPlot->getTeam() == eTeam) && pLoopPlot->isAdjacentTeam(getID(), true))
			{
				iCount++;
			}
		}
	}

	return iCount;
}


int CvTeamAI::AI_calculatePlotWarValue(TeamTypes eTeam) const
{
	FAssert(eTeam != getID());

	int iValue = 0;

	for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);

		if (pLoopPlot->getTeam() == eTeam)
		{
			if (!pLoopPlot->isWater() && pLoopPlot->isAdjacentTeam(getID(), true))
			{
				iValue += 4;
			}
			BonusTypes eBonus = pLoopPlot->getBonusType();
			if (NO_BONUS != eBonus)
			{
				iValue += 40 * GC.getBonusInfo(eBonus).getAIObjective();
			}
		}
	}

	return iValue;
}


int CvTeamAI::AI_calculateCapitalProximity(TeamTypes eTeam) const
{
	CvCity* pOurCapitalCity;
	CvCity* pTheirCapitalCity;
	int iTotalDistance;
	int iCount;
	int iI, iJ;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	iTotalDistance = 0;
	iCount = 0;
	
	int iMinDistance = MAX_INT;
	int iMaxDistance = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				pOurCapitalCity = GET_PLAYER((PlayerTypes)iI).getPrimaryCity();

				if (pOurCapitalCity != NULL)
				{
					for (iJ = 0; iJ < MAX_PLAYERS; iJ++)
					{
						if (GET_PLAYER((PlayerTypes)iJ).isAlive())
						{
							if (GET_PLAYER((PlayerTypes)iJ).getTeam() != getID())
							{
								pTheirCapitalCity = GET_PLAYER((PlayerTypes)iJ).getPrimaryCity();

								if (pTheirCapitalCity != NULL)
								{
									int iDistance = (plotDistance(pOurCapitalCity->getX_INLINE(), pOurCapitalCity->getY_INLINE(), pTheirCapitalCity->getX_INLINE(), pTheirCapitalCity->getY_INLINE()) * (pOurCapitalCity->area() != pTheirCapitalCity->area() ? 3 : 2));
									if (GET_PLAYER((PlayerTypes)iJ).getTeam() == eTeam)
									{
										iTotalDistance += iDistance;
										iCount++;
									}
									iMinDistance = std::min(iDistance, iMinDistance);
									iMaxDistance = std::max(iDistance, iMaxDistance);
								}
							}
						}
					}
				}
			}
		}
	}
	
	if (iCount > 0)
	{
		FAssert(iMaxDistance > 0);
		return ((GC.getMap().maxPlotDistance() * (iMaxDistance - ((iTotalDistance / iCount) - iMinDistance))) / iMaxDistance);
	}

	return 0;
}


bool CvTeamAI::AI_isLandTarget(TeamTypes eTeam) const
{
	if (!AI_hasCitiesInPrimaryArea(eTeam))
	{
		return false;
	}

	if (AI_calculateAdjacentLandPlots(eTeam) < 8)
	{
		return false;
	}

	return true;
}

// this determines if eTeam or any of its allies are land targets of us
bool CvTeamAI::AI_isAllyLandTarget(TeamTypes eTeam) const
{
	for (int iTeam = 0; iTeam < MAX_TEAMS; iTeam++)
	{
		CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iTeam);
		if (iTeam != getID())
		{
			if (iTeam == eTeam || kLoopTeam.isDefensivePact(eTeam))
			{
				if (AI_isLandTarget((TeamTypes)iTeam))
				{
					return true;
				}
			}
		}
	}

	return false;
}


bool CvTeamAI::AI_shareWar(TeamTypes eTeam) const
{
	int iI;

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			if ((iI != getID()) && (iI != eTeam))
			{
				if (isAtWar((TeamTypes)iI) && GET_TEAM(eTeam).isAtWar((TeamTypes)iI))
				{
					// R&R, ray, fix for improved Attitude through shared war with Animals
					if(iI != GET_PLAYER(GC.getGameINLINE().getBarbarianPlayer()).getTeam())
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}


AttitudeTypes CvTeamAI::AI_getAttitude(TeamTypes eTeam, bool bForced) const
{
	int iAttitude;
	int iCount;
	int iI, iJ;

	// Erik: Sometimes we just can't help but be a bit introspective :P
	//FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");
	// K-Mod
	if (eTeam == getID())
		return ATTITUDE_FRIENDLY;
	// K-Mod end


	iAttitude = 0;
	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				for (iJ = 0; iJ < MAX_PLAYERS; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iJ).isAlive() && iI != iJ)
					{
						TeamTypes eTeamLoop = GET_PLAYER((PlayerTypes)iJ).getTeam();
						if (eTeamLoop == eTeam)
						{
							iAttitude += GET_PLAYER((PlayerTypes)iI).AI_getAttitude((PlayerTypes)iJ, bForced);
							iCount++;
						}
					}
				}
			}
		}
	}

	if (iCount > 0)
	{
		return ((AttitudeTypes)(iAttitude / iCount));
	}

	return ATTITUDE_CAUTIOUS;
}


int CvTeamAI::AI_getAttitudeVal(TeamTypes eTeam, bool bForced) const
{
	int iAttitudeVal;
	int iCount;
	int iI, iJ;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	iAttitudeVal = 0;
	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				for (iJ = 0; iJ < MAX_PLAYERS; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iJ).isAlive())
					{
						if (GET_PLAYER((PlayerTypes)iJ).getTeam() == eTeam)
						{
							iAttitudeVal += GET_PLAYER((PlayerTypes)iI).AI_getAttitudeVal((PlayerTypes)iJ, bForced);
							iCount++;
						}
					}
				}
			}
		}
	}

	if (iCount > 0)
	{
		return (iAttitudeVal / iCount);
	}

	return 0;
}


int CvTeamAI::AI_getMemoryCount(TeamTypes eTeam, MemoryTypes eMemory) const
{
	int iMemoryCount;
	int iCount;
	int iI, iJ;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	iMemoryCount = 0;
	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				for (iJ = 0; iJ < MAX_PLAYERS; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iJ).isAlive())
					{
						if (GET_PLAYER((PlayerTypes)iJ).getTeam() == eTeam)
						{
							iMemoryCount += GET_PLAYER((PlayerTypes)iI).AI_getMemoryCount(((PlayerTypes)iJ), eMemory);
							iCount++;
						}
					}
				}
			}
		}
	}

	if (iCount > 0)
	{
		return (iMemoryCount / iCount);
	}

	return 0;
}

int CvTeamAI::AI_startWarVal(TeamTypes eTarget, WarPlanTypes eWarPlan,
	bool bConstCache) const // advc.001n
{
	PROFILE_FUNC();

	int iValue;
	
	int iValidity = AI_targetValidity(eTarget);
	if (iValidity == 0)
	{
		return 0;
	}

	iValue = AI_calculatePlotWarValue(eTarget);

	iValue += (3 * AI_calculateCapitalProximity(eTarget)) / ((iValue > 0) ? 2 : 3);
	
	int iClosenessValue = AI_teamCloseness(eTarget);
	if (iClosenessValue == 0)
	{
		iValue /= 4;
	}
	iValue += iClosenessValue / 4;
	
	iValue *= iValidity;
	iValue /= 100;
	
	//Domination...
	int iOurLandPercent = getTotalLand() * 100 / GC.getMap().getLandPlots();
	int iPercentOfDomination = 0;
	for (int iI = 0; iI < GC.getNumVictoryInfos(); iI++)
	{
		if (GC.getVictoryInfo((VictoryTypes)iI).getLandPercent() > 0)
		{
			iPercentOfDomination = 100 * iOurLandPercent / std::max(1, GC.getGameINLINE().getAdjustedLandPercent((VictoryTypes)iI));
		}
	}
	
	if (iPercentOfDomination > 75)
	{
		iValue *= (GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI) ? 6 : 4);
	}

	switch (AI_getAttitude(eTarget))
	{
	case ATTITUDE_FURIOUS:
		iValue *= 16;
		break;

	case ATTITUDE_ANNOYED:
		iValue *= 8;
		break;

	case ATTITUDE_CAUTIOUS:
		iValue *= 4;
		break;

	case ATTITUDE_PLEASED:
		iValue *= 2;
		break;

	case ATTITUDE_FRIENDLY:
		iValue *= 1;
		break;

	default:
		FAssert(false);
		break;
	}
	
	return iValue;
}


int CvTeamAI::AI_endWarVal(TeamTypes eTeam) const // XXX this should consider area power...
{
	FAssert(eTeam != getID());
	FAssert(isAtWar(eTeam));

	CvTeamAI const& kOurTeam = GET_TEAM(getID());
	CvTeamAI const& kWarTeam = GET_TEAM(eTeam); // K-Mod

	int iValue = 100;
	iValue += getNumCities() * 3;
	iValue += kWarTeam.getNumCities() * 3;
	iValue += getTotalPopulation();
	iValue += kWarTeam.getTotalPopulation();
	iValue += (kWarTeam.AI_getWarSuccess(getID()) * 20); // .uround();

	int iOurPower = std::max(1, getPower());
	int iTheirPower = std::max(1, kWarTeam.getDefensivePower(getID())); 
#if 0
	{	// <kekm.39> Multiplying by iTheirPower can overflow
		scaled rPowMult(iTheirPower + 10, iOurPower + iTheirPower + 10);
		iValue = (iValue * rPowMult).uround(); // </kekm.39>
	}
#endif
	WarPlanTypes const eWarPlan = AI_getWarPlan(eTeam);
	/*	if we are not human, do we want to continue war for strategic reasons?
		only check if our power is at least 120% of theirs */
	if (!isHuman() && iOurPower > 120 * iTheirPower / 100)
	{
		bool bDagger = false;
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			const CvPlayerAI& kLoopPlayer = GET_PLAYER((PlayerTypes)iI); // K-Mod
			if (kLoopPlayer.isAlive() && kLoopPlayer.getTeam() == getID())
			{
				if (kLoopPlayer.AI_isDoStrategy(AI_STRATEGY_DAGGER))
					bDagger = true;
			}
		}
		if (bDagger)
		{
			// if dagger, value peace at 90% * power ratio
			iValue *= 9 * iTheirPower;
			iValue /= 10 * iOurPower;
		}
		/*	for now, we will always do the land mass check for domination
			if we have more than half the land, then value peace at 90% * land ratio */
		int iLandRatio = getTotalLand() * 100 / std::max(1, kWarTeam.getTotalLand());
		if (iLandRatio > 120)
		{
			iValue *= 9 * 100;
			iValue /= 10 * iLandRatio;
		}
	}

	if ((!isHuman() && eWarPlan == WARPLAN_TOTAL) ||
		(!kWarTeam.isHuman() && kWarTeam.AI_getWarPlan(getID()) == WARPLAN_TOTAL))
	{
		iValue *= 2;
	}
	else if ((!isHuman() && eWarPlan == WARPLAN_DOGPILE && kWarTeam.getNumWars() > 1) ||
		(!kWarTeam.isHuman() &&
			kWarTeam.AI_getWarPlan(getID()) == WARPLAN_DOGPILE && getNumWars() > 1))
	{
		iValue *= 3;
		iValue /= 2;
	}

	// Do we have a big stack en route?
	int iOurAttackers = 0;
	//for (MemberAIIter it(getID()); it.hasNext(); ++it)
	FOR_EACH_TEAM_PLAYER_MEMBER(kOurTeam, kPlayer)
		iOurAttackers += kPlayer.AI_enemyTargetMissions(eTeam);
	int iTheirAttackers = 0;
	FOR_EACH_AREA_VAR(pLoopArea)
		iTheirAttackers += AI_countEnemyDangerByArea(*pLoopArea, eTeam);

	int iAttackerRatio = (100 * iOurAttackers) /
		std::max(1 + GC.getGame().getCurrentEra(), iTheirAttackers);

	if (GC.getGame().isOption(GAMEOPTION_AGGRESSIVE_AI))
	{
		iValue *= 150;
		iValue /= range(iAttackerRatio, 150, 900);
	}
	else
	{
		iValue *= 200;
		iValue /= range(iAttackerRatio, 200, 600);
	}
	// BETTER_BTS_AI_MOD: END
	return AI_roundTradeVal(iValue);
}

// eTeam is the team that we're evaluating
int CvTeamAI::AI_mapTradeVal(TeamTypes eTeam) const
{
	int iValue = 0;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");
	
	for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		const CvPlot* const pLoopPlot = GC.getMap().plotByIndexINLINE(iI);

		// If we haven't revealed the plot and the other team has
		if (!(pLoopPlot->isRevealed(getID(), false)) && pLoopPlot->isRevealed(eTeam, false))
		{
			int iBonusMultiplier = 1;
				
			const BonusTypes eBonus = pLoopPlot->getBonusType();
			if (NO_BONUS != eBonus)
			{
				// Value plots with bonus resources more
				iBonusMultiplier = 2;
			}
		
			int iPlotValue = 0;

			if (pLoopPlot->isWater())
			{
				iPlotValue = 1;
			}
			else
			{
				iPlotValue = 5;
			}

			iValue += (iPlotValue * iBonusMultiplier);

			// This eliminates the possible exploit where the human player would sell their maps
			// just prior to popping a goodie or visiting a village
			if (isHuman())
			{
				if (!pLoopPlot->isRevealedGoody(getID()) && pLoopPlot->isRevealedGoody(eTeam))
				{
					iValue += 250;
				}

				const CvCity* pCity = pLoopPlot->getPlotCity();
				if (pCity != NULL)
				{
					if (!pCity->isScoutVisited(getID()))
					{
						iValue += 50;
					}
				}
			}
		}
	}

	iValue /= 10;

	iValue -= (iValue % GC.getDefineINT("DIPLOMACY_VALUE_REMAINDER"));

	// R&R, ray, Map Trade Value
	iValue = iValue * GC.getDefineINT("RaR_MAP_PRICE_MODIFIER") / 100;

	if (isHuman())
	{
		// Human players generally get more value out of map knowledge so
		// the AI will charge them more for this advantage :P
		return std::max(iValue, GC.getDefineINT("DIPLOMACY_VALUE_REMAINDER")) * 2;
	}
	else
	{
		return iValue;
	}
}


DenialTypes CvTeamAI::AI_mapTrade(TeamTypes eTeam) const
{
	PROFILE_FUNC();

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	if (isHuman())
	{
		return NO_DENIAL;
	}

	if (isAtWar(eTeam))
	{
		return NO_DENIAL;
	}

	if (AI_getWorstEnemy() == eTeam)
	{
		return DENIAL_WORST_ENEMY;
	}

	AttitudeTypes eAttitude = AI_getAttitude(eTeam);

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				if (eAttitude <= GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getMapRefuseAttitudeThreshold())
				{
					return DENIAL_ATTITUDE;
				}
			}
		}
	}

	return NO_DENIAL;
}


int CvTeamAI::AI_makePeaceTradeVal(TeamTypes ePeaceTeam, TeamTypes eTeam) const
{
	int iModifier;
	int iValue;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(ePeaceTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(GET_TEAM(ePeaceTeam).isAlive(), "GET_TEAM(ePeaceTeam).isAlive is expected to be true");
	FAssertMsg(atWar(ePeaceTeam, eTeam), "eTeam should be at war with ePeaceTeam");

	iValue = (50 + GC.getGameINLINE().getGameTurn());
	iValue += ((GET_TEAM(eTeam).getNumCities() + GET_TEAM(ePeaceTeam).getNumCities()) * 8);

	iModifier = 0;

	switch ((GET_TEAM(eTeam).AI_getAttitude(ePeaceTeam) + GET_TEAM(ePeaceTeam).AI_getAttitude(eTeam)) / 2)
	{
	case ATTITUDE_FURIOUS:
		iModifier += 400;
		break;

	case ATTITUDE_ANNOYED:
		iModifier += 200;
		break;

	case ATTITUDE_CAUTIOUS:
		iModifier += 100;
		break;

	case ATTITUDE_PLEASED:
		iModifier += 50;
		break;

	case ATTITUDE_FRIENDLY:
		break;

	default:
		FAssert(false);
		break;
	}

	iValue *= std::max(0, (iModifier + 100));
	iValue /= 100;

	iValue *= 40;
	iValue /= (GET_TEAM(eTeam).AI_getAtWarCounter(ePeaceTeam) + 10);

	iValue -= (iValue % GC.getDefineINT("DIPLOMACY_VALUE_REMAINDER"));

	if (isHuman())
	{
		return std::max(iValue, GC.getDefineINT("DIPLOMACY_VALUE_REMAINDER"));
	}
	else
	{
		return iValue;
	}
}


DenialTypes CvTeamAI::AI_makePeaceTrade(TeamTypes ePeaceTeam, TeamTypes eTeam) const
{
	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(ePeaceTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(GET_TEAM(ePeaceTeam).isAlive(), "GET_TEAM(ePeaceTeam).isAlive is expected to be true");
	FAssertMsg(isAtWar(ePeaceTeam), "should be at war with ePeaceTeam");

	if (GET_TEAM(ePeaceTeam).isHuman())
	{
		return DENIAL_CONTACT_THEM;
	}

	if (isHuman())
	{
		return NO_DENIAL;
	}


	// Erik: If the AI is fighting the revolutionary war it will not consider
	// making peace before its kings has lost
	
	if (isInRevolution() && GET_TEAM(ePeaceTeam).hasEuropePlayer())
	{
		return DENIAL_VICTORY;
	}

	if (!canChangeWarPeace(ePeaceTeam))
	{
		return DENIAL_PEACE_NOT_POSSIBLE_US;
	}

	if (AI_endWarVal(ePeaceTeam) > (GET_TEAM(ePeaceTeam).AI_endWarVal(getID()) * 2))
	{
		return DENIAL_CONTACT_THEM;
	}
	
    int iLandRatio = ((getTotalLand() * 100) / std::max(20, GET_TEAM(eTeam).getTotalLand()));
    if (iLandRatio > 250)
    {
		return DENIAL_VICTORY;
	}

	return NO_DENIAL;
}


int CvTeamAI::AI_declareWarTradeVal(TeamTypes eWarTeam, TeamTypes eTeam) const
{
	PROFILE_FUNC();

	int iModifier;
	int iValue;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(eWarTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(GET_TEAM(eWarTeam).isAlive(), "GET_TEAM(eWarTeam).isAlive is expected to be true");
	FAssertMsg(!atWar(eWarTeam, eTeam), "eTeam should be at peace with eWarTeam");

	iValue = 0;
	iValue += (GET_TEAM(eWarTeam).getNumCities() * 10);
	iValue += (GET_TEAM(eWarTeam).getTotalPopulation() * 2);

	iModifier = 0;

	if (GET_TEAM(eTeam).AI_isNative() && GET_TEAM(eWarTeam).AI_isKing())
	{
		return 0;
	}

	switch (GET_TEAM(eTeam).AI_getAttitude(eWarTeam))
	{
	case ATTITUDE_FURIOUS:
		break;

	case ATTITUDE_ANNOYED:
		iModifier += 25;
		break;

	case ATTITUDE_CAUTIOUS:
		iModifier += 50;
		break;

	case ATTITUDE_PLEASED:
		iModifier += 150;
		break;

	case ATTITUDE_FRIENDLY:
		iModifier += 400;
		break;

	default:
		FAssert(false);
		break;
	}

	iValue *= std::max(0, (iModifier + 100));
	iValue /= 100;

	int iTheirPower = GET_TEAM(eTeam).getPower();
	int iWarTeamPower = GET_TEAM(eWarTeam).getPower();

	iValue *= 50 + ((100 * iWarTeamPower) / (iTheirPower + iWarTeamPower + 1));
	iValue /= 100;

	if (!(GET_TEAM(eTeam).AI_isAllyLandTarget(eWarTeam)))
	{
		iValue *= 2;
	}

	if (!isAtWar(eWarTeam))
	{
		iValue *= 3;
	}
	else
	{
		iValue *= 150;
		iValue /= 100 + ((50 * std::min(100, (100 * AI_getWarSuccess(eWarTeam)) / (8 + getTotalPopulation()))) / 100);
	}
	
	iValue += (GET_TEAM(eTeam).getNumCities() * 20);
	iValue += (GET_TEAM(eTeam).getTotalPopulation() * 15);
	
	if (isAtWar(eWarTeam))
	{
		switch (GET_TEAM(eTeam).AI_getAttitude(getID()))
		{
		case ATTITUDE_FURIOUS:
		case ATTITUDE_ANNOYED:
		case ATTITUDE_CAUTIOUS:
			iValue *= 100;
			break;

		case ATTITUDE_PLEASED:
			iValue *= std::max(75, 100 - getAtWarCount() * 10);
			break;

		case ATTITUDE_FRIENDLY:
			iValue *= std::max(50, 100 - getAtWarCount() * 20);
			break;

		default:
			FAssert(false);
			break;
		}
		iValue /= 100;
	}
	
	if (GET_TEAM(eWarTeam).getAtWarCount() == 0)
	{
		iValue *= 2;
	
		for (int iI = 0; iI < MAX_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				if (iI != getID() && iI != eWarTeam && iI != eTeam)
				{
					if (GET_TEAM(eWarTeam).isDefensivePact((TeamTypes)iI))
					{
						iValue += (GET_TEAM((TeamTypes)iI).getNumCities() * 30);
						iValue += (GET_TEAM((TeamTypes)iI).getTotalPopulation() * 20);
					}
				}
			}
		}
	}

	iValue *= 60 + (140 * GC.getGameINLINE().getGameTurn()) / std::max(1, GC.getGameINLINE().getEstimateEndTurn());
	iValue /= 100;
	
	iValue -= (iValue % GC.getDefineINT("DIPLOMACY_VALUE_REMAINDER"));

	if (isHuman())
	{
		return std::max(iValue, GC.getDefineINT("DIPLOMACY_VALUE_REMAINDER"));
	}
	else
	{
		return iValue;
	}
}


DenialTypes CvTeamAI::AI_declareWarTrade(TeamTypes eWarTeam, TeamTypes eTeam, bool bConsiderPower) const
{
	PROFILE_FUNC();

	AttitudeTypes eAttitude;
	AttitudeTypes eAttitudeThem;
	bool bLandTarget;
	int iI;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(eWarTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(GET_TEAM(eWarTeam).isAlive(), "GET_TEAM(eWarTeam).isAlive is expected to be true");
	FAssertMsg(!isAtWar(eWarTeam), "should be at peace with eWarTeam");

	if (GET_TEAM(eWarTeam).isDefensivePact(eTeam))
	{
		return DENIAL_JOKING;
	}

	if (isHuman())
	{
		return NO_DENIAL;
	}

	if (!canDeclareWar(eWarTeam))
	{
		return DENIAL_WAR_NOT_POSSIBLE_US;
	}

	if (getAnyWarPlanCount() > 0)
	{
		return DENIAL_TOO_MANY_WARS;
	}
	
	if (AI_isNative())
	{
		if (GET_TEAM(eWarTeam).isParentOf(eTeam))
		{
			if (atWar(eWarTeam, eTeam))
			{
				if (AI_getAttitude(eTeam) >= ATTITUDE_FRIENDLY)
				{
					return NO_DENIAL;
				}
			}
		}
	}

	if (bConsiderPower)
	{
		bLandTarget = AI_isAllyLandTarget(eWarTeam);

		if ((GET_TEAM(eWarTeam).getDefensivePower() / ((bLandTarget) ? 2 : 1)) >
			(getPower() + ((atWar(eWarTeam, eTeam)) ? GET_TEAM(eTeam).getPower() : 0)))
		{
			if (bLandTarget)
			{
				return DENIAL_POWER_THEM;
			}
			else
			{
				return DENIAL_NO_GAIN;
			}
		}
	}
	
	bool bConcerned = false;
	bool bVeryConcerned = false;
	if (hasColonialPlayer())
	{
		CvTeamAI& kWarTeam = GET_TEAM(eWarTeam);
		if (kWarTeam.AI_getAttitude(getID()) < ATTITUDE_CAUTIOUS)
		{
			bConcerned = true;
			if (kWarTeam.AI_getAttitude(getID()) < ATTITUDE_ANNOYED)
			{
				bVeryConcerned = true;
			}
		}
	}
	
	if (!bConcerned)
	{
		eAttitude = AI_getAttitude(eTeam);

		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
				{
					if (eAttitude <= GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getDeclareWarRefuseAttitudeThreshold())
					{
						return DENIAL_ATTITUDE;
					}
				}
			}
		}
	}

	if (!bVeryConcerned)
	{
		eAttitudeThem = AI_getAttitude(eWarTeam);

		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
				{
					if (eAttitudeThem > GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getDeclareWarThemRefuseAttitudeThreshold())
					{
						return DENIAL_ATTITUDE_THEM;
					}
				}
			}
		}
	}
	
	return NO_DENIAL;
}


int CvTeamAI::AI_openBordersTradeVal(TeamTypes eTeam) const
{
	return (getNumCities() + GET_TEAM(eTeam).getNumCities());
}


DenialTypes CvTeamAI::AI_openBordersTrade(TeamTypes eTeam) const
{
	PROFILE_FUNC();

	AttitudeTypes eAttitude;
	int iI;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	if (isHuman())
	{
		return NO_DENIAL;
	}

	if (AI_shareWar(eTeam))
	{
		return NO_DENIAL;
	}
	
	if (AI_getMemoryCount(eTeam, MEMORY_CANCELLED_OPEN_BORDERS) > 0)
	{
		return DENIAL_RECENT_CANCEL;
	}

	if (AI_getWorstEnemy() == eTeam)
	{
		return DENIAL_WORST_ENEMY;
	}

	eAttitude = AI_getAttitude(eTeam);

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				if (eAttitude <= GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getOpenBordersRefuseAttitudeThreshold())
				{
					return DENIAL_ATTITUDE;
				}
			}
		}
	}

	return NO_DENIAL;
}


int CvTeamAI::AI_defensivePactTradeVal(TeamTypes eTeam) const
{
	return ((getNumCities() + GET_TEAM(eTeam).getNumCities()) * 3);
}


DenialTypes CvTeamAI::AI_defensivePactTrade(TeamTypes eTeam) const
{
	PROFILE_FUNC();

	AttitudeTypes eAttitude;
	int iI;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	if (isHuman())
	{
		return NO_DENIAL;
	}

	if (GC.getGameINLINE().countCivTeamsAlive() == 2)
	{
		return DENIAL_NO_GAIN;
	}

	if (AI_getWorstEnemy() == eTeam)
	{
		return DENIAL_WORST_ENEMY;
	}

	eAttitude = AI_getAttitude(eTeam);

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				if (eAttitude <= GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getDefensivePactRefuseAttitudeThreshold())
				{
					return DENIAL_ATTITUDE;
				}
			}
		}
	}

	return NO_DENIAL;
}

// eTeam is the team that we're considering to ally with
DenialTypes CvTeamAI::AI_permanentAllianceTrade(TeamTypes eTeam) const
{
	PROFILE_FUNC();

	AttitudeTypes eAttitude;
	int iI;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	if (isHuman())
	{
		return NO_DENIAL;
	}

	if (AI_getWorstEnemy() == eTeam)
	{
		return DENIAL_WORST_ENEMY;
	}

	if ((getPower() + GET_TEAM(eTeam).getPower()) > (GC.getGameINLINE().countTotalCivPower() / 2))
	{
		if (getPower() > GET_TEAM(eTeam).getPower())
		{
			return DENIAL_POWER_US;
		}
		else
		{
			return DENIAL_POWER_YOU;
		}
	}

	if ((AI_getDefensivePactCounter(eTeam) + AI_getShareWarCounter(eTeam)) < 40)
	{
		return DENIAL_NOT_ALLIED;
	}

	eAttitude = AI_getAttitude(eTeam);

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				if (eAttitude <= GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getPermanentAllianceRefuseAttitudeThreshold())
				{
					return DENIAL_ATTITUDE;
				}
			}
		}
	}

	if (!GET_TEAM(eTeam).isInRevolution() && GC.getDefineINT("PERMANENT_ALLIANCE_REQUIRES_REVOLUTION"))
	{
		return DENIAL_NO_GAIN;
	}

	return NO_DENIAL;
}


TeamTypes CvTeamAI::AI_getWorstEnemy() const
{
	return m_eWorstEnemy;
}


void CvTeamAI::AI_updateWorstEnemy()
{
	PROFILE_FUNC();

	TeamTypes eBestTeam;
	int iValue;
	int iBestValue;
	int iI;

	iBestValue = MAX_INT;
	eBestTeam = NO_TEAM;

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			// R&R, ray, fix for Wild Animals not to be considered as Worst Enemy
			if (iI != getID() && (TeamTypes)iI != GET_PLAYER(GC.getGameINLINE().getBarbarianPlayer()).getTeam()) 
			{
				if (isHasMet((TeamTypes)iI))
				{
					if (AI_getAttitude((TeamTypes)iI) < ATTITUDE_CAUTIOUS)
					{
						iValue = AI_getAttitudeVal((TeamTypes)iI);

						if (iValue < iBestValue)
						{
							iBestValue = iValue;
							eBestTeam = ((TeamTypes)iI);
						}
					}
				}
			}
		}
	}

	m_eWorstEnemy = eBestTeam;
}


int CvTeamAI::AI_getWarPlanStateCounter(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_iWarPlanStateCounter.get(eIndex);
}


void CvTeamAI::AI_setWarPlanStateCounter(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_em_iWarPlanStateCounter.set(eIndex, iNewValue);
	FAssert(AI_getWarPlanStateCounter(eIndex) >= 0);
}


void CvTeamAI::AI_changeWarPlanStateCounter(TeamTypes eIndex, int iChange)
{
	AI_setWarPlanStateCounter(eIndex, (AI_getWarPlanStateCounter(eIndex) + iChange));
}


int CvTeamAI::AI_getAtWarCounter(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_iAtWarCounter.get(eIndex);
}


void CvTeamAI::AI_setAtWarCounter(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_em_iAtWarCounter.set(eIndex, iNewValue);
	FAssert(AI_getAtWarCounter(eIndex) >= 0);
}


void CvTeamAI::AI_changeAtWarCounter(TeamTypes eIndex, int iChange)
{
	AI_setAtWarCounter(eIndex, (AI_getAtWarCounter(eIndex) + iChange));
}


int CvTeamAI::AI_getAtPeaceCounter(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_iAtPeaceCounter.get(eIndex);
}


void CvTeamAI::AI_setAtPeaceCounter(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_em_iAtPeaceCounter.set(eIndex, iNewValue);
	FAssert(AI_getAtPeaceCounter(eIndex) >= 0);
}


void CvTeamAI::AI_changeAtPeaceCounter(TeamTypes eIndex, int iChange)
{
	AI_setAtPeaceCounter(eIndex, (AI_getAtPeaceCounter(eIndex) + iChange));
}


int CvTeamAI::AI_getHasMetCounter(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_iHasMetCounter.get(eIndex);
}


void CvTeamAI::AI_setHasMetCounter(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_em_iHasMetCounter.set(eIndex, iNewValue);
	FAssert(AI_getHasMetCounter(eIndex) >= 0);
}


void CvTeamAI::AI_changeHasMetCounter(TeamTypes eIndex, int iChange)
{
	AI_setHasMetCounter(eIndex, (AI_getHasMetCounter(eIndex) + iChange));
}


int CvTeamAI::AI_getOpenBordersCounter(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_iOpenBordersCounter.get(eIndex);
}


void CvTeamAI::AI_setOpenBordersCounter(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_em_iOpenBordersCounter.set(eIndex, iNewValue);
	FAssert(AI_getOpenBordersCounter(eIndex) >= 0);
}


void CvTeamAI::AI_changeOpenBordersCounter(TeamTypes eIndex, int iChange)
{
	AI_setOpenBordersCounter(eIndex, (AI_getOpenBordersCounter(eIndex) + iChange));
}


int CvTeamAI::AI_getDefensivePactCounter(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_iDefensivePactCounter.get(eIndex);
}


void CvTeamAI::AI_setDefensivePactCounter(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_em_iDefensivePactCounter.set(eIndex, iNewValue);
	FAssert(AI_getDefensivePactCounter(eIndex) >= 0);
}


void CvTeamAI::AI_changeDefensivePactCounter(TeamTypes eIndex, int iChange)
{
	AI_setDefensivePactCounter(eIndex, (AI_getDefensivePactCounter(eIndex) + iChange));
}


int CvTeamAI::AI_getShareWarCounter(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_iShareWarCounter.get(eIndex);
}


void CvTeamAI::AI_setShareWarCounter(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_em_iShareWarCounter.set(eIndex, iNewValue);
	FAssert(AI_getShareWarCounter(eIndex) >= 0);
}


void CvTeamAI::AI_changeShareWarCounter(TeamTypes eIndex, int iChange)
{
	AI_setShareWarCounter(eIndex, (AI_getShareWarCounter(eIndex) + iChange));
}


int CvTeamAI::AI_getWarSuccess(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_iWarSuccess.get(eIndex);
}


void CvTeamAI::AI_setWarSuccess(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_em_iWarSuccess.set(eIndex, iNewValue);
	FAssert(AI_getWarSuccess(eIndex) >= 0);
}


void CvTeamAI::AI_changeWarSuccess(TeamTypes eIndex, int iChange)
{
	AI_setWarSuccess(eIndex, (AI_getWarSuccess(eIndex) + iChange));
}


int CvTeamAI::AI_getEnemyPeacetimeTradeValue(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_iEnemyPeacetimeTradeValue.get(eIndex);
}


void CvTeamAI::AI_setEnemyPeacetimeTradeValue(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_em_iEnemyPeacetimeTradeValue.set(eIndex, iNewValue);
	FAssert(AI_getEnemyPeacetimeTradeValue(eIndex) >= 0);
}


void CvTeamAI::AI_changeEnemyPeacetimeTradeValue(TeamTypes eIndex, int iChange)
{
	AI_setEnemyPeacetimeTradeValue(eIndex, (AI_getEnemyPeacetimeTradeValue(eIndex) + iChange));
}


int CvTeamAI::AI_getEnemyPeacetimeGrantValue(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_iEnemyPeacetimeGrantValue.get(eIndex);
}


void CvTeamAI::AI_setEnemyPeacetimeGrantValue(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_em_iEnemyPeacetimeGrantValue.set(eIndex, iNewValue);
	FAssert(AI_getEnemyPeacetimeGrantValue(eIndex) >= 0);
}


void CvTeamAI::AI_changeEnemyPeacetimeGrantValue(TeamTypes eIndex, int iChange)
{
	AI_setEnemyPeacetimeGrantValue(eIndex, (AI_getEnemyPeacetimeGrantValue(eIndex) + iChange));
}


int CvTeamAI::AI_getDamages(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_iDamages.get(eIndex);
}


void CvTeamAI::AI_setDamages(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_em_iDamages.set(eIndex, iNewValue);
}


void CvTeamAI::AI_changeDamages(TeamTypes eIndex, int iChange)
{
	AI_setDamages(eIndex, (AI_getDamages(eIndex) + iChange));
}

void CvTeamAI::AI_doDamages(TeamTypes eTeam, bool bPeace)
{
	if (bPeace)
	{
		AI_setDamages(eTeam, 0);
		return;
	}
	
	int iDamages = 0;
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		PlayerTypes ePlayer = (PlayerTypes)iI;
		CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
		if (kPlayer.isAlive())
		{
			if (kPlayer.getTeam() == getID())
			{
				if (kPlayer.isNative())
				{
					iDamages += kPlayer.AI_calculateDamages(eTeam);
					iDamages += 100;
				}
			}
		}
	}
	
	AI_setDamages(eTeam, iDamages);
}


WarPlanTypes CvTeamAI::AI_getWarPlan(TeamTypes eIndex) const
{
	if (eIndex == NO_TEAM)
	{
		return NO_WARPLAN;
	}

	FAssert(eIndex < MAX_TEAMS);
	FAssert(eIndex != getID() || m_em_eWarPlan.get(eIndex) == NO_WARPLAN);
	return m_em_eWarPlan.get(eIndex);
}


bool CvTeamAI::AI_isChosenWar(TeamTypes eIndex) const
{
	switch (AI_getWarPlan(eIndex))
	{
	case WARPLAN_ATTACKED_RECENT:
	case WARPLAN_ATTACKED:
		return false;
		break;
	case WARPLAN_PREPARING_LIMITED:
	case WARPLAN_PREPARING_TOTAL:
	case WARPLAN_LIMITED:
	case WARPLAN_TOTAL:
	case WARPLAN_DOGPILE:
	case WARPLAN_EXTORTION:
		return true;
		break;
	}

	return false;
}

bool CvTeamAI::AI_isSneakAttackReady(TeamTypes eIndex) const
{
	//return (AI_isChosenWar(eIndex) && !(AI_isSneakAttackPreparing(eIndex))); // BtS
	// K-Mod (advc: originally in an overloaded function)
	if (eIndex != NO_TEAM)
		return !isAtWar(eIndex) && AI_isChosenWar(eIndex) && !AI_isSneakAttackPreparing(eIndex); // K-Mod
	
	FOR_EACH_MAJOR_CIV(kOurTeam, kOtherTeam)
	{
		if (AI_isSneakAttackReady(kOtherTeam.getID()))
			return true;
	}
	return false;
	// K-Mod end
}

bool CvTeamAI::AI_isSneakAttackPreparing(TeamTypes eIndex) const
{
	if (eIndex != NO_TEAM)
	{
		const WarPlanTypes eWarPlan = AI_getWarPlan(eIndex); // advc.104j
		return (eWarPlan == WARPLAN_PREPARING_LIMITED || eWarPlan == WARPLAN_PREPARING_TOTAL);
	}

	for (TeamTypes eTeam = FIRST_TEAM; eTeam < NUM_TEAM_TYPES; ++eTeam)
	{
		const CvTeamAI& kEnemy = GET_TEAM(eTeam);
		const TeamTypes ourTeam = getID();
		if (eTeam == ourTeam || !kEnemy.isHasMet(ourTeam) || !kEnemy.isAlive() || !kEnemy.AI_isColonialPower())
			continue;
		if (AI_isSneakAttackPreparing(eTeam))
			return true;
	}
	return false;
}

void CvTeamAI::AI_setWarPlan(TeamTypes eIndex, WarPlanTypes eNewValue, bool bWar)
{
	int iI;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
		
	if (AI_getWarPlan(eIndex) != eNewValue)
	{
		if (bWar || !isAtWar(eIndex))
		{
			m_em_eWarPlan.set(eIndex, eNewValue);

			if (gTeamLogLevel >= 1)
			{
				CvWStringBuffer buf;
				GAMETEXT.getWarplanString(buf, eNewValue);
				logBBAI("Team %d (%S) CvTeamAI::AI_setWarPlan (%S) against team %d (%S)", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), buf.getCString(), eIndex, GET_PLAYER(GET_TEAM(eIndex).getLeaderID()).getCivilizationDescription(0));
			}
			AI_setWarPlanStateCounter(eIndex, 0);

			AI_updateAreaStrategies();

			for (iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
					{
						if (!(GET_PLAYER((PlayerTypes)iI).isHuman()))
						{
							GET_PLAYER((PlayerTypes)iI).AI_makeProductionDirty();
							if (GET_PLAYER((PlayerTypes)iI).isNative())
							{
								GET_PLAYER((PlayerTypes)iI).AI_doNativeArmy(eIndex);
							}
						}
					}
				}
			}
		}
		AI_doTactics();
	}
}

//if this number is over 0 the teams are "close"
//this may be expensive to run, kinda O(N^2)...
int CvTeamAI::AI_teamCloseness(TeamTypes eIndex, int iMaxDistance) const
{
	PROFILE_FUNC();
	int iI, iJ;
	
	if (iMaxDistance == -1)
	{
		iMaxDistance = 5;
	}
	
	FAssert(eIndex != getID());
	int iValue = 0;
	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				for (iJ = 0; iJ < MAX_PLAYERS; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iJ).isAlive())
					{
						if (GET_PLAYER((PlayerTypes)iJ).getTeam() == eIndex)
						{
							iValue += GET_PLAYER((PlayerTypes)iI).AI_playerCloseness((PlayerTypes)iJ, iMaxDistance);
						}
					}
				}
			}
		}
	}
	
	return iValue;	
}

int CvTeamAI::AI_targetValidity(TeamTypes eTeam) const
{
	PROFILE_FUNC();
	int iI, iJ;
	
	FAssert(eTeam != getID());
	int iValue = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				for (iJ = 0; iJ < MAX_PLAYERS; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iJ).isAlive())
					{
						if (GET_PLAYER((PlayerTypes)iJ).getTeam() == eTeam)
						{
							iValue = std::max(iValue, GET_PLAYER((PlayerTypes)iI).AI_targetValidity((PlayerTypes)iJ));
						}
					}
				}
			}
		}
	}
	
	return iValue;	
}


void CvTeamAI::read(FDataStreamBase* pStream)
{
	CvSavegameReaderBase readerbase(pStream);
	CvSavegameReader reader(readerbase);

	read(reader);
}


void CvTeamAI::write(FDataStreamBase* pStream)
{
	CvSavegameWriterBase writerbase(pStream);
	CvSavegameWriter writer(writerbase);
	write(writer);
	writerbase.WriteFile();
}

// <advc.012>
int CvTeamAI::AI_plotDefense(CvPlot const& kPlot, bool bIgnoreBuilding,
	bool bGarrisonStrength) const // advc.500b
{
	TeamTypes eAttacker = NO_TEAM;
	/*  We could also be attacked in p by a second war enemy that doesn't own the
	plot; impossible to predict. An attack by the plot owner is far more likely
	though. */
	if (kPlot.getOwner() != NO_PLAYER && GET_TEAM(getID()).isAtWar(kPlot.getTeam()))
		eAttacker = kPlot.getTeam();
	/* WTP: Not supported yet
	return kPlot.defenseModifier(getID(), bIgnoreBuilding, eAttacker,
		bGarrisonStrength); // advc.500b
	*/
	return kPlot.defenseModifier(getID(), bIgnoreBuilding);

} // </advc.012>

// Protected Functions...
int CvTeamAI::AI_maxWarRand() const
{
	int iRand = 0;
	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kPlayer.isAlive())
		{
			if (kPlayer.getTeam() == getID())
			{
				if (kPlayer.isNative())
				{
					int iOurPop = getTotalPopulation();
					int iTheirPop = GET_TEAM(kPlayer.getTeam()).getTotalPopulation();
					
					int iModifier = 100;
					if (iTheirPop > iOurPop / 4)
					{
						iModifier = (100 * (1 + iOurPop)) / ( 1 + iTheirPop);
					}
					else
					{
						iModifier += 1000000;//Maybe someone will win the lottery, hahah.
					}
					
					iModifier *= 100;
					iModifier /= 100 + 2 * std::max(0, kPlayer.AI_getOverpopulationPercent());
					iRand += (iModifier * GC.getLeaderHeadInfo(kPlayer.getPersonalityType()).getMaxWarRand()) / 100;
				}
				else
				{
					iRand += GC.getLeaderHeadInfo(kPlayer.getPersonalityType()).getMaxWarRand();
				}
				iCount++;
			}
		}
	}

	if (iCount > 0)
	{
		iRand /= iCount;
	}

	return iRand;
}


int CvTeamAI::AI_maxWarNearbyPowerRatio() const
{
	int iRand = 0;
	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kPlayer.isAlive())
		{
			if (kPlayer.getTeam() == getID())
			{
				iRand += GC.getLeaderHeadInfo(kPlayer.getPersonalityType()).getMaxWarNearbyPowerRatio();
				iCount++;
			}
		}
	}

	if (iCount > 1)
	{
		iRand /= iCount;
	}

	return iRand;
}


int CvTeamAI::AI_maxWarDistantPowerRatio() const
{
	int iRand;
	int iCount;
	int iI;

	iRand = 0;
	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iRand += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getMaxWarDistantPowerRatio();
				iCount++;
			}
		}
	}

	if (iCount > 1)
	{
		iRand /= iCount;
	}

	return iRand;
}


int CvTeamAI::AI_maxWarMinAdjacentLandPercent() const
{
	int iRand = 0;
	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iRand += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getMaxWarMinAdjacentLandPercent();
				iCount++;
			}
		}
	}

	if (iCount > 0)
	{
		iRand /= iCount;
	}

	return iRand;
}


int CvTeamAI::AI_limitedWarRand() const
{
	int iRand;
	int iCount;
	int iI;

	iRand = 0;
	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iRand += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getLimitedWarRand();
				iCount++;
			}
		}
	}

	if (iCount > 0)
	{
		iRand /= iCount;
	}

	return iRand;
}


int CvTeamAI::AI_limitedWarPowerRatio() const
{
	int iRand;
	int iCount;
	int iI;

	iRand = 0;
	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iRand += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getLimitedWarPowerRatio();
				iCount++;
			}
		}
	}

	if (iCount > 0)
	{
		iRand /= iCount;
	}

	return iRand;
}


int CvTeamAI::AI_dogpileWarRand() const
{
	int iRand;
	int iCount;
	int iI;

	iRand = 0;
	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iRand += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getDogpileWarRand();
				iCount++;
			}
		}
	}

	if (iCount > 0)
	{
		iRand /= iCount;
	}

	return iRand;
}


int CvTeamAI::AI_makePeaceRand() const
{
	int iRand = 0;
	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iRand += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getMakePeaceRand();
				iCount++;
			}
		}
	}

	if (iCount > 0)
	{
		iRand /= iCount;
	}

	return iRand;
}


int CvTeamAI::AI_noWarAttitudeProb(AttitudeTypes eAttitude) const
{
	int iProb;
	int iCount;
	int iI;

	iProb = 0;
	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iProb += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getNoWarAttitudeProb(eAttitude);
				iCount++;
			}
		}
	}

	if (iCount > 1)
	{
		iProb /= iCount;
	}

	return iProb;
}


void CvTeamAI::AI_doCounter()
{
	int iI;

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (iI != getID())
			{
				AI_changeWarPlanStateCounter(((TeamTypes)iI), 1);

				if (isAtWar((TeamTypes)iI))
				{
					AI_changeAtWarCounter(((TeamTypes)iI), 1);
				}
				else
				{
					AI_changeAtPeaceCounter(((TeamTypes)iI), 1);
				}

				if (isHasMet((TeamTypes)iI))
				{
					AI_changeHasMetCounter(((TeamTypes)iI), 1);
				}

				if (isOpenBorders((TeamTypes)iI))
				{
					AI_changeOpenBordersCounter(((TeamTypes)iI), 1);
				}

				if (isDefensivePact((TeamTypes)iI))
				{
					AI_changeDefensivePactCounter(((TeamTypes)iI), 1);
				}
				else
				{
					if (AI_getDefensivePactCounter((TeamTypes)iI) > 0)
					{
						AI_changeDefensivePactCounter(((TeamTypes)iI), -1);
					}
				}

				if (isHasMet((TeamTypes)iI))
				{
					if (AI_shareWar((TeamTypes)iI))
					{
						AI_changeShareWarCounter(((TeamTypes)iI), 1);
					}
				}
			}
		}
	}
}


void CvTeamAI::AI_doWar()
{
	PROFILE_FUNC();

	CvArea* pLoopArea;
	TeamTypes eBestTeam;
	bool bAreaValid;
	bool bShareValid;
	bool bOffensiveValid;
	int iNoWarRoll;
	int iOurPower;
	int iDogpilePower;
	int iValue;
	int iBestValue;
	int iPass;
	int iLoop;
	int iI, iJ;

	FAssert(!isHuman());

	if (GC.getUSE_AI_DO_WAR_CALLBACK()) // K-Mod. block unused python callbacks
	{ 
		// allow python to handle it
		CyArgsList argsList;
		argsList.add(getID());
		long lResult=0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "AI_doWar", argsList.makeFunctionArgs(), &lResult);
		if (lResult == 1)
		{
			return;
		}
	}

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			WarPlanTypes eWarPlan = AI_getWarPlan((TeamTypes)iI);
			if (eWarPlan != NO_WARPLAN)
			{
				int iTimeModifier = 100;
				{
					/*
					int iOurPower = getPower();
					int iTheirPower = GET_TEAM((TeamTypes)iI).getPower();
					
					int iRatio = (1 + iOurPower) / (1 + iTheirPower);
					
					if (iRatio > 200)
					{
						iTimeModifier *= 100;
						iTimeModifier /= iRatio - 100;
					}
					*/
					
					iTimeModifier *= 50 + GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
					iTimeModifier /= 150;
					FAssert(iTimeModifier > 0);
				}
					
				if (eWarPlan == WARPLAN_ATTACKED_RECENT)
				{
					FAssert(isAtWar((TeamTypes)iI));

					if (AI_getAtWarCounter((TeamTypes)iI) > ((GET_TEAM((TeamTypes)iI).AI_isLandTarget(getID())) ? 9 : 3))
					{
						AI_setWarPlan(((TeamTypes)iI), WARPLAN_ATTACKED);
					}
				}
				else if (eWarPlan == WARPLAN_PREPARING_LIMITED)
				{
					FAssert(canDeclareWar((TeamTypes)iI));

					if (AI_getWarPlanStateCounter((TeamTypes)iI) > ((5 * iTimeModifier) / 100))
					{
						AI_setWarPlan(((TeamTypes)iI), WARPLAN_LIMITED);
					}
				}
				else if (eWarPlan == WARPLAN_EXTORTION)
				{
					if (AI_getWarPlanStateCounter((TeamTypes)iI) > ((6 * iTimeModifier) / 100))
					{
						AI_setWarPlan(((TeamTypes)iI), WARPLAN_LIMITED);
					}
				}
				else if (eWarPlan == WARPLAN_PREPARING_TOTAL)
				{
					FAssert(canDeclareWar((TeamTypes)iI));

					if (AI_getWarPlanStateCounter((TeamTypes)iI) > ((10 * iTimeModifier) / 100))
					{
						bAreaValid = true;
						bShareValid = false;

						for(pLoopArea = GC.getMap().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop))
						{
							if (AI_isPrimaryArea(*pLoopArea))
							{
								if (GET_TEAM((TeamTypes)iI).countNumCitiesByArea(*pLoopArea) > 0)
								{
									bShareValid = true;

									bOffensiveValid = false;

									if (AI_calculateAreaAIType(*pLoopArea, true) == AREAAI_OFFENSIVE)
									{
										bOffensiveValid = true;
									}

									if (!bOffensiveValid)
									{
										bAreaValid = false;
									}
								}
							}
						}

						if (bAreaValid || !bShareValid)
						{
							AI_setWarPlan(((TeamTypes)iI), WARPLAN_TOTAL);
						}
						else if (AI_getWarPlanStateCounter((TeamTypes)iI) > ((20 * iTimeModifier) / 100))
						{
							AI_setWarPlan(((TeamTypes)iI), NO_WARPLAN);
						}
					}
				}
			}
		}
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				GET_PLAYER((PlayerTypes)iI).AI_doPeace();
			}
		}
	}
	
	int iNumMembers = getNumMembers();
	
	bool bNativeTeam = hasNativePlayer() && !hasColonialPlayer();
	// if at war, check for making peace
	if (getAtWarCount() > 0) // XXX
	{
		int iPeaceRand = AI_makePeaceRand();
		if (bNativeTeam)
		{
			iPeaceRand *= GC.getGameINLINE().getGameTurn() + GC.getGameINLINE().getEstimateEndTurn() / 5;
			iPeaceRand /= std::max(1, GC.getGameINLINE().getEstimateEndTurn());
		}
		
		if (GC.getGameINLINE().getSorenRandNum(iPeaceRand, "AI Make Peace") == 0)
		{
			for (iI = 0; iI < MAX_TEAMS; iI++)
			{
				CvTeamAI& kLoopTeam = GET_TEAM((TeamTypes)iI);
				if (kLoopTeam.isAlive())
				{
					if (iI != getID())
					{
						if (!(kLoopTeam.isHuman()))
						{
							if (canContact((TeamTypes)iI))
							{
								if (isAtWar((TeamTypes)iI))
								{
									if (AI_isChosenWar((TeamTypes)iI))
									{
										if (AI_getAtWarCounter((TeamTypes)iI) > 10)
										{
											if (!AI_isLandTarget((TeamTypes)iI))
											{
												if (AI_getAtWarCounter((TeamTypes)iI) > 10)
												{
													makePeace((TeamTypes)iI);
												}
											}
										}
										if (AI_getAtWarCounter((TeamTypes)iI) > 20)
										{

											if (AI_getAtWarCounter((TeamTypes)iI) > ((AI_getWarPlan((TeamTypes)iI) == WARPLAN_TOTAL) ? 30 : 20))
											{
												int iOurValue = AI_endWarVal((TeamTypes)iI);
												int iTheirValue = kLoopTeam.AI_endWarVal(getID());
												if ((iOurValue > (iTheirValue / (bNativeTeam ? 5 : 3))) && (iTheirValue > (iOurValue / (bNativeTeam ? 5 : 3))))
												{
													makePeace((TeamTypes)iI);
													break;
												}
											}

											if (AI_getWarPlan((TeamTypes)iI) == WARPLAN_DOGPILE)
											{
												if (kLoopTeam.getAtWarCount() == 1)
												{
													makePeace((TeamTypes)iI);
													break;
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
	// if no war plans, consider starting one!
	else if ((getAnyWarPlanCount() == 0) && (GC.getGame().getGameTurn() > 25))
	{
		bool bAggressive = GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI);

	    // if random in this range is 0, we go to war of this type (so lower numbers are higher probablity)
		// average of everyone on our team
		int iMaxWarRand = AI_maxWarRand();
	    int iLimitedWarRand = AI_limitedWarRand();
	    int iDogpileWarRand = AI_dogpileWarRand();
	    

	    int iWarRandThreshold = (GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI) ? 20 : 10);
	    iWarRandThreshold /= 3;
	    iWarRandThreshold += GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI) ? 1 : 0;
		
		// overall war check (quite frequently true)
		bool bMakeWarChecks = false;
		

		// random overall war chance (at noble+ difficulties this is 100%)
		if (GC.getGameINLINE().getSorenRandNum(100, "AI Declare War 1") < GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIDeclareWarProb())
		{
			// if non-agressive, random based on number of cities (the more cities, the less war)
			if (bAggressive || (GC.getGameINLINE().getSorenRandNum(4, "AI Declare War 2") != 0))
			{
				bMakeWarChecks = true;
			}
		}
		
		int iGameTurn = GC.getGameINLINE().getGameTurn();
		int iEndTurn = GC.getGameINLINE().getEstimateEndTurn();
		
		int iLimitedWarTurn = (iEndTurn * 20) / 100;
		int iTotalWarTurn = (iEndTurn * 40) / 100;
		
		iLimitedWarTurn *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getNativePacifismPercent();
		iLimitedWarTurn /= 100;
		iTotalWarTurn *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getNativePacifismPercent();
		iTotalWarTurn /= 100;
		
		if (bMakeWarChecks)
		{
			iOurPower = getPower();

			if (bAggressive)
			{
				iOurPower *= 4;
				iOurPower /= 3;
			}

			if (iGameTurn > iTotalWarTurn && GC.getGameINLINE().getSorenRandNum(iMaxWarRand, "AI Maximum War") <= iWarRandThreshold)
			{
				iNoWarRoll = GC.getGameINLINE().getSorenRandNum(100, "AI No War") - 0;
				iNoWarRoll = range(iNoWarRoll + (bAggressive ? 10 : 0), 0, 99);

				iBestValue = 0;
				eBestTeam = NO_TEAM;

				for (iPass = 0; iPass < 3; iPass++)
				{
					for (iI = 0; iI < MAX_TEAMS; iI++)
					{
						if (GET_TEAM((TeamTypes)iI).isAlive())
						{
							if (iI != getID())
							{
								if (isHasMet((TeamTypes)iI))
								{
									if (canDeclareWar((TeamTypes)iI))
									{
										if (iNoWarRoll >= AI_noWarAttitudeProb(AI_getAttitude((TeamTypes)iI)))
										{
											int iDefensivePower = (GET_TEAM((TeamTypes)iI).getDefensivePower() * 2) / 3;
											
											if (iDefensivePower < ((iOurPower * ((iPass > 1) ? AI_maxWarDistantPowerRatio() : AI_maxWarNearbyPowerRatio())) / 100))
											{
												// XXX make sure they share an area....

												FAssertMsg(iI != getID(), "Expected not to be declaring war on self (DOH!)");

												if ((iPass > 1) || (AI_isLandTarget((TeamTypes)iI) || AI_isAnyCapitalAreaAlone()))
												{
													if ((iPass > 0) || (AI_calculateAdjacentLandPlots((TeamTypes)iI) >= ((getTotalLand() * AI_maxWarMinAdjacentLandPercent()) / 100)))
													{
														iValue = AI_startWarVal((TeamTypes)iI);

														if (iValue > iBestValue)
														{
															iBestValue = iValue;
															eBestTeam = ((TeamTypes)iI);
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

					if (eBestTeam != NO_TEAM)
					{
						AI_setWarPlan(eBestTeam, WARPLAN_PREPARING_TOTAL);
						break;
					}
				}
			}
			else if (iGameTurn > iLimitedWarTurn && GC.getGameINLINE().getSorenRandNum(iLimitedWarRand, "AI Limited War") == 0)
			{
				iNoWarRoll = GC.getGameINLINE().getSorenRandNum(100, "AI No War") - 10;
				iNoWarRoll = range(iNoWarRoll + (bAggressive ? 10 : 0), 0, 99);

				iBestValue = 0;
				eBestTeam = NO_TEAM;

				for (iI = 0; iI < MAX_TEAMS; iI++)
				{
					if (GET_TEAM((TeamTypes)iI).isAlive())
					{
						if (iI != getID())
						{
							if (isHasMet((TeamTypes)iI))
							{
								if (canDeclareWar((TeamTypes)iI))
								{
									if (iNoWarRoll >= AI_noWarAttitudeProb(AI_getAttitude((TeamTypes)iI)))
									{
										if (AI_isLandTarget((TeamTypes)iI) || (AI_isAnyCapitalAreaAlone() && GET_TEAM((TeamTypes)iI).AI_isAnyCapitalAreaAlone()))
										{
											if (GET_TEAM((TeamTypes)iI).getDefensivePower() < ((iOurPower * AI_limitedWarPowerRatio()) / 100))
											{
												iValue = AI_startWarVal((TeamTypes)iI);

												if (iValue > iBestValue)
												{
													FAssert(!AI_shareWar((TeamTypes)iI));
													iBestValue = iValue;
													eBestTeam = ((TeamTypes)iI);
												}
											}
										}
									}
								}
							}
						}
					}
				}

				if (eBestTeam != NO_TEAM)
				{
					AI_setWarPlan(eBestTeam, WARPLAN_PREPARING_LIMITED);
				}
			}
			else if	(GC.getGameINLINE().getSorenRandNum(iDogpileWarRand, "AI Dogpile War") == 0)
			{
				iNoWarRoll = GC.getGameINLINE().getSorenRandNum(100, "AI No War") - 20;
				iNoWarRoll = range(iNoWarRoll + (bAggressive ? 10 : 0), 0, 99);

				iBestValue = 0;
				eBestTeam = NO_TEAM;

				for (iI = 0; iI < MAX_TEAMS; iI++)
				{
					if (GET_TEAM((TeamTypes)iI).isAlive())
					{
						if (iI != getID())
						{
							if (isHasMet((TeamTypes)iI))
							{
								if (canDeclareWar((TeamTypes)iI))
								{
									if (iNoWarRoll >= AI_noWarAttitudeProb(AI_getAttitude((TeamTypes)iI)))
									{
										if (GET_TEAM((TeamTypes)iI).getAtWarCount() > 0)
										{
											if (AI_isLandTarget((TeamTypes)iI))
											{
												iDogpilePower = iOurPower;

												for (iJ = 0; iJ < MAX_TEAMS; iJ++)
												{
													if (GET_TEAM((TeamTypes)iJ).isAlive())
													{
														if (iJ != iI)
														{
															if (atWar(((TeamTypes)iJ), ((TeamTypes)iI)))
															{
																iDogpilePower += GET_TEAM((TeamTypes)iJ).getPower();
															}
														}
													}
												}

												FAssert(GET_TEAM((TeamTypes)iI).getPower() == GET_TEAM((TeamTypes)iI).getDefensivePower());

												if (((GET_TEAM((TeamTypes)iI).getDefensivePower() * 3) / 2) < iDogpilePower)
												{
													iValue = AI_startWarVal((TeamTypes)iI);

													if (iValue > iBestValue)
													{
														FAssert(!AI_shareWar((TeamTypes)iI));
														iBestValue = iValue;
														eBestTeam = ((TeamTypes)iI);
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

				if (eBestTeam != NO_TEAM)
				{
					AI_setWarPlan(eBestTeam, WARPLAN_DOGPILE);
				}
			}
		}
	}
}

//returns true if war is veto'd by rolls.
bool CvTeamAI::AI_performNoWarRolls(TeamTypes eTeam)
{
	
	if (GC.getGameINLINE().getSorenRandNum(100, "AI Declare War 1") > GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIDeclareWarProb())
	{
		return true;
	}
	
	if (GC.getGameINLINE().getSorenRandNum(100, "AI No War") <= AI_noWarAttitudeProb(AI_getAttitude(eTeam)))
	{
		return true;		
	}
	
	
	
	return false;	
}

int CvTeamAI::AI_getAttitudeWeight(TeamTypes eTeam)
{
	int iAttitudeWeight = 0;
	switch (AI_getAttitude(eTeam))
	{
	case ATTITUDE_FURIOUS:
		iAttitudeWeight = -100;
		break;
	case ATTITUDE_ANNOYED:
		iAttitudeWeight = -40;
		break;
	case ATTITUDE_CAUTIOUS:
		iAttitudeWeight = -5;
		break;
	case ATTITUDE_PLEASED:
		iAttitudeWeight = 50;
		break;
	case ATTITUDE_FRIENDLY:
		iAttitudeWeight = 100;			
		break;
	}
	
	return iAttitudeWeight;
}

bool CvTeamAI::AI_isWaterAreaRelevant(CvArea* pArea)
{
	int iTeamCities = 0;
	int iOtherTeamCities = 0;
	
	if (GC.getMap().findBiggestArea(true) == pArea)
	{
		return true;
	}
	
	//An area is deemed relevant if it has at least 2 cities of our and different teams.
	
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
	{
		CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		
		if ((iTeamCities < 2 && (kPlayer.getTeam() == getID())) || (iOtherTeamCities < 2 && (kPlayer.getTeam() != getID())))
		{
			int iLoop;
			CvCity* pLoopCity;
			
			for (pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				if (pLoopCity->plot()->isAdjacentToArea(pArea->getID()))
				{
					if (kPlayer.getTeam() == getID())
					{
						iTeamCities++;
						if (iTeamCities >= 2)
						{
							break;
						}
					}
					else
					{
						iOtherTeamCities++;
						if (iOtherTeamCities >= 2)
						{
							break;
						}
					}
				}				
			}
		}
		if (iTeamCities >= 2 && iOtherTeamCities >= 2)
		{
			return true;
		}
	}
	return false;
}

bool CvTeamAI::AI_isExploringNeeded(CvUnit* pUnit) const
{
	//This function will probably allow the AI to perform some limited.. um.. divination
	
	int iRevealedLandPlots = 0;
	int iUnrevealedLandPlots = 0;
	int iNumCities = 0;
	
	iNumCities = getNumCities();
	
	CvArea* pArea = pUnit->area();
	if (!pArea->isWater())
	{
		if (pUnit->getDomainType() == DOMAIN_SEA)
		{
			pArea = pUnit->plot()->waterArea();
		}
		else
		{
			return (pArea->getNumTiles() > pArea->getNumRevealedTiles(getID()));
		}
	}
	
	FAssert(pArea != NULL);
	if(pArea == NULL)
	{
		return false;
	}
	
	for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);
		if (!pLoopPlot->isWater())
		{
			if (pLoopPlot->isAdjacentToArea(pArea))
			{
				if (pLoopPlot->isRevealed(getID(), false))
				{
					iRevealedLandPlots++;
				}
				else
				{
					iUnrevealedLandPlots++;
				}
			}
		}
	}
	
	if (iUnrevealedLandPlots == 0)
	{
		return false;
	}
	
	if (((iNumCities * 3 + 2) * NUM_CITY_PLOTS) / 2 > iRevealedLandPlots)
	{
		return true;
	}

//	CvArea* pLoopArea;
//	int iLoop;
//	for(pLoopArea = GC.getMapINLINE().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMapINLINE().nextArea(&iLoop))
//	{
//		if (!pLoopArea->isWater())
//		{
//			iRevealedLandPlots += pLoopArea->getNumRevealedTiles(getID());
//			iLandPlots += pLoopArea->getNumTiles();
//		}
//	}
//	
//	iUnrevealedLandPlots = iLandPlots - iRevealedLandPlots;
//	

	
	return false;
}

void CvTeamAI::AI_doTactics()
{
	if (getAnyWarPlanCount() == 0)
	{
		m_aiEnemyCityDistance.clear();
		m_aiEnemyUnitDistance.clear();
		return;
	}
	
	m_aiEnemyCityDistance.resize(GC.getMap().numPlotsINLINE());
	m_aiEnemyUnitDistance.resize(GC.getMap().numPlotsINLINE());
	
	CvMap& kMap = GC.getMap();
	std::deque<int> plotQueue;
	for(int iI = 0; iI < kMap.numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = kMap.plotByIndexINLINE(iI);

		if ((pLoopPlot->isCity() && (AI_getWarPlan(pLoopPlot->getTeam()) != NO_WARPLAN))
			&& !(GET_PLAYER(pLoopPlot->getOwnerINLINE()).isNative() && AI_isKing()))
		{
				m_aiEnemyCityDistance[iI] = 0;
				plotQueue.push_back(iI);
		}
		else
		{
			m_aiEnemyCityDistance[iI] = -1;
		}
	}
	
	int iVisits = 0;
	while (!plotQueue.empty())
	{
		iVisits++;
		int iPlot = plotQueue.front();
		CvPlot* pPlot = kMap.plotByIndexINLINE(iPlot);
		plotQueue.pop_front();
		
		int iDistance = m_aiEnemyCityDistance[iPlot];
		iDistance += 1;

		for (int iDirection = 0; iDirection < NUM_DIRECTION_TYPES; iDirection++)
		{
			CvPlot* pDirectionPlot = plotDirection(pPlot->getX_INLINE(), pPlot->getY_INLINE(), (DirectionTypes)iDirection);
			if (pDirectionPlot != NULL)
			{
				bool bValid = false;
				if ((pDirectionPlot->isWater() && pPlot->isWater() && pPlot->isAdjacentWaterPassable(pDirectionPlot))
					|| (!pDirectionPlot->isWater() && !pPlot->isWater())
						|| (pDirectionPlot->isWater() && pPlot->isCity()))
				{
				

					int iPlotNum = kMap.plotNumINLINE(pDirectionPlot->getX_INLINE(), pDirectionPlot->getY_INLINE());
					if ((iDistance < m_aiEnemyCityDistance[iPlotNum]) || (m_aiEnemyCityDistance[iPlotNum] == -1))
					{
						m_aiEnemyCityDistance[iPlotNum] = iDistance;
						plotQueue.push_back(iPlotNum);
					}
				}
			}
		}
	}
	
	PlayerTypes eLeaderID = getLeaderID();
	plotQueue.clear();
	for(int iI = 0; iI < kMap.numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = kMap.plotByIndexINLINE(iI);

		if (pLoopPlot->isVisibleEnemyUnit(eLeaderID))
		{
				m_aiEnemyUnitDistance[iI] = 0;
				plotQueue.push_back(iI);
		}
		else
		{
			m_aiEnemyUnitDistance[iI] = -1;
		}
	}
	
	iVisits = 0;
	while (!plotQueue.empty())
	{
		iVisits++;
		int iPlot = plotQueue.front();
		CvPlot* pPlot = kMap.plotByIndexINLINE(iPlot);
		plotQueue.pop_front();
		
		int iDistance = m_aiEnemyUnitDistance[iPlot];
		iDistance += 1;

		for (int iDirection = 0; iDirection < NUM_DIRECTION_TYPES; iDirection++)
		{
			CvPlot* pDirectionPlot = plotDirection(pPlot->getX_INLINE(), pPlot->getY_INLINE(), (DirectionTypes)iDirection);
			if (pDirectionPlot != NULL)
			{
				bool bValid = false;
				if ((pDirectionPlot->isWater() && pPlot->isWater() && pPlot->isAdjacentWaterPassable(pDirectionPlot))
					|| (!pDirectionPlot->isWater() && !pPlot->isWater())
						|| (pDirectionPlot->isWater() && !pPlot->isWater()))
				{
					int iPlotNum = kMap.plotNumINLINE(pDirectionPlot->getX_INLINE(), pDirectionPlot->getY_INLINE());
					if ((iDistance < m_aiEnemyUnitDistance[iPlotNum]) || (m_aiEnemyUnitDistance[iPlotNum] == -1))
					{
						m_aiEnemyUnitDistance[iPlotNum] = iDistance;
						plotQueue.push_back(iPlotNum);
					}
				}
			}
		}
	}
}

short CvTeamAI::AI_enemyCityDistance(CvPlot* pPlot) const
{
	if (m_aiEnemyCityDistance.empty())
	{
		return -1;
	}
	return m_aiEnemyCityDistance[GC.getMap().plotNumINLINE(pPlot->getX_INLINE(), pPlot->getY_INLINE())];
}

short CvTeamAI::AI_enemyUnitDistance(CvPlot* pPlot) const
{
	if (m_aiEnemyUnitDistance.empty())
	{
		return -1;
	}
	return m_aiEnemyUnitDistance[GC.getMap().plotNumINLINE(pPlot->getX_INLINE(), pPlot->getY_INLINE())];
}

//Greed is a function of relative wealth and relative power.
int CvTeamAI::AI_getGreed(TeamTypes eTeam) const
{
	CvTeam& kOtherTeam = GET_TEAM(eTeam);
	
	int iValue = 100;
	
	iValue *= kOtherTeam.getAssets() + 1;
	iValue /= getAssets() + 1;
	
	iValue *= getPower() + 1;
	iValue /= kOtherTeam.getPower() + 1;
	
	return iValue;	
}

int CvTeamAI::AI_getAnger(TeamTypes eTeam) const
{
	int iValue = 0;
	int iAttitude = AI_getAttitudeVal(eTeam);
	
	if (iAttitude < 0)
	{
		iValue = 50 + iAttitude * -15;
	}
	else
	{
		iValue = std::max(0, 50 + iAttitude * -5);
	}
	return iValue;
}

int CvTeamAI::AI_getFear(TeamTypes eTeam) const
{
	CvTeamAI& kOtherTeam = GET_TEAM(eTeam);
	int iValue = 100;
	iValue *= 1 + kOtherTeam.getPower();
	iValue /= 1 + getPower();
	
	if (iValue > 100)
	{
		int iAttitude = -kOtherTeam.AI_getAttitudeVal(eTeam);
		if (iAttitude > 0)
		{
			int iTempValue = -iValue - 100;

			iTempValue *= 100 + 10 * iAttitude;

			iValue += iTempValue;
		}	
	}
	return iValue;
}
	
int CvTeamAI::AI_warplanStrength(WarPlanTypes eWarplan) const
{
	int iValue = 0;
	switch (eWarplan)
	{
		case WARPLAN_ATTACKED_RECENT:
			iValue = 100;
			break;
		
		case WARPLAN_ATTACKED:
			iValue = 50;
			break;
		
		case WARPLAN_PREPARING_LIMITED:
			iValue = 50;
			break;
		
		case WARPLAN_PREPARING_TOTAL:
			iValue = 70;
			break;
		
		case WARPLAN_LIMITED:
			iValue = 50;
			break;
		
		case WARPLAN_TOTAL:
			iValue = 95;
			break;
		
		case WARPLAN_DOGPILE:
			iValue = 80;
			break;
		
		case WARPLAN_EXTORTION:
			iValue = 40;
			break;

		default:
			FAssertMsg(false, "Bad Warplan");
			break;
	}
	
	return iValue;
}

bool CvTeamAI::AI_isNative() const
{
	for (int iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && (GET_PLAYER((PlayerTypes)iI).getTeam() == getID()))
		{
			if (!GET_PLAYER((PlayerTypes)iI).isNative())
			{
				return false;
			}
		}
	}
	
	return true;
}

bool CvTeamAI::AI_isKing() const
{
	return hasEuropePlayer();
}

/*	advc.opt: Replacing global isPotentialEnemy (CvGameCoreUtils). Not much faster here -
	mostly avoids some NO_TEAM checks -, but also makes more sense as a CvTeamAI member.
	I've also redirected some CvUnitAI::AI_isPotentialEnemy calls here. That function
	allows alwaysHostile units to attack w/o war plan; this function doesn't. */
bool CvTeamAI::AI_mayAttack(TeamTypes eDefender) const
{
	if (isAtWar(eDefender))
		return true;
	TeamTypes const eDefenderMaster = getID(); // GET_TEAM(eDefender).getMasterTeam(); // advc.104j
	//if (!AI_isSneakAttackReady(eDefender))
	// advc.opt: Avoid isAtWar and NO_TEAM check
	if (!AI_isImminentWarPlan(AI_getWarPlan(/* advc.104j: */ eDefenderMaster)))
		return false;
	// (War w/ eDefenderMaster and peace w/ eDefender can happen while resolving DoW)
	FAssertMsg(/*!isAVassal() ||*/ isAtWar(eDefenderMaster), "Vassal shouldn't have imminent undeclared war plan");
	/*	UNOFFICIAL_PATCH, Bugfix, General AI, 05/05/09, jdog5000: START
		Fixes bug where AI would launch invasion while unable to declare war
		which caused units to be bumped once forced peace expired */
		//return canDeclareWar(eDefender);
		/*	advc.opt: Avoid the canEventuallyDeclareWar check - the war plan
			has to be abandoned if we can't ever declare. */
	return (!isForcePeace(eDefenderMaster) && // advc.104j
		!isForcePeace(eDefender));
}

// advc: Replacing K-Mod's CvPlot::isVisiblePotentialEnemyUnit
bool CvTeamAI::AI_mayAttack(CvPlot const& kPlot) const
{
	/*	Unfortunately, PUF_isPotentialEnemy requires a PlayerTypes argument.
		Would have to change several PUF... signatures to remedy that. */
	PlayerTypes const ePlayer = getLeaderID();
	// <K-Mod>
	return (kPlot.plotCheck(PUF_isPotentialEnemy, ePlayer, false,
		NO_PLAYER, NO_TEAM, PUF_isVisible, ePlayer) != NULL); // </K-Mod>
}

// Returns true if eTeam is an actual colonial power that we can be in a real war with
// other Europeans, natives and our king (after DOI) qualify
// the church, barbarians/animals and other kings do not (and ofc our own team!)
// eTeam must be passed to check if at war with the king. 
// TODO: result should be cached!
bool CvTeamAI::AI_isColonialPower() const
{
	// TODO: AI_isNative seems to be a duplicate of hasNativePlayer?
	// TODO: Merge these loops!	
	return (AI_isNative() || hasColonialPlayer());
}

bool CvTeamAI::AI_isColonialOrBarbarianPower() const
{
	// TODO: AI_isNative seems to be a duplicate of hasNativePlayer?
	// TODO: Merge these loops!	
	return (AI_isNative() || hasColonialPlayer() || isBarbarian());
}

// Private Functions...

/*  BETTER_BTS_AI_MOD, War Strategy AI, 03/20/10, jdog5000: START
	Compute power of enemies as percentage of our power */
int CvTeamAI::AI_getEnemyPowerPercent(bool bConsiderOthers) const
{
	int iEnemyPower = 0;
	
	FOR_EACH_CIV_ALIVE_AND_KNOWN_POTENTIAL_ENEMY_OF(kOurTeam, kEnemy)
	{
		if (isAtWar(kEnemy.getID()))
		{
			int iTempPower = 220 * kEnemy.getPower();
			iTempPower /= (AI_hasCitiesInPrimaryArea(kEnemy.getID()) ? 2 : 3);
			//iTempPower /= (kEnemy.isMinorCiv() ? 3 : 1);
			iTempPower /= std::max(1, (bConsiderOthers ?
				kEnemy.getNumWars() : 1));
			iEnemyPower += iTempPower;
		}
		else if (AI_isChosenWar(kEnemy.getID())) // Haven't declared war yet
			/*  advc.104j: getDefensivePower counts vassals already.
				If planning war against multiple civs, DP allies could also be
				double counted (fixme). Could collect the war enemies in a std::set
				in a first pass; though it sucks to implement the vassal/DP logic
				multiple times (already in getDefensivePower and MilitaryAnalyst).
				Also, the computation for bConsiderOthers above can be way off. 
			*/
		{
			int iTempPower = 240 * kEnemy.getDefensivePower(getID());
			iTempPower /= (AI_hasCitiesInPrimaryArea(kEnemy.getID()) ? 2 : 3);
			iTempPower /= 1 + (bConsiderOthers ? kEnemy.getNumWars() : 0);
			iEnemyPower += iTempPower;
		}
	}

	//return (iEnemyPower/std::max(1, (isAVassal() ? getCurrentMasterPower(true) : getPower(true))));
	// K-Mod - Lets not rely too much on our vassals...
	int iOurPower = getPower();
	return iEnemyPower / std::max(1, iOurPower);
	// K-Mod end
}

// advc.105, advc.104:
bool CvTeamAI::AI_isPushover(TeamTypes ePotentialEnemy) const
{
	/*	Stricter checks for human potential enemy? But there's already
		some code for dealing with humans at some of the call sites ... */
	CvTeam const& kPotentialEnemy = GET_TEAM(ePotentialEnemy);
	int iTheirCities = kPotentialEnemy.getNumCities();
	int iOurCities = getNumCities();
	return (((iTheirCities <= 1 && iOurCities >= 3) ||
		4 * iTheirCities < iOurCities) &&
		10 * kPotentialEnemy.getPower() < 4 * getPower());
}

// K-Mod: return true if is fair enough for the AI to know there is a city here
bool CvTeamAI::AI_deduceCitySite(CvCity const& kCity) const
{
	PROFILE_FUNC();

	if (kCity.isRevealed(getID(), false))
		return true;

	/*	The rule is this: if we can see more than n plots of the nth culture ring,
		we can deduce where the city is. */

	int iPoints = 0;
	int const iLevel = kCity.getCultureLevel();
	//for (SquareIter itPlot(kCity.getPlot(), iLevel, false); itPlot.hasNext(); ++itPlot)
	FOR_EACH_NON_CENTER_PLOT_IN_RANGE_OF(kCity.plot(), iLevel,
	{
		CvPlot* itPlot = pLoopPlot;
		//int iDist = CvCity::cultureDistance(itPlot.currXDist(), itPlot.currYDist());
		const int iDist = kCity.cultureDistance(iDX, iDY);
		if (iDist > iLevel)
			continue;
		if (itPlot->getRevealedOwner(getID(), false) == kCity.getOwner())
		{
			/*	if multiple cities have their plot in their range,
				then that will make it harder to deduce the precise city location. */
			iPoints += 1 + std::max(0, 1 + iLevel
				- iDist - itPlot->getNumCultureRangeCities(kCity.getOwner()));
			if (iPoints > iLevel)
				return true;
		}
	}) // FOR_EACH
	return false;
}

// advc.105:
bool CvTeamAI::AI_isAnyChosenWar() const
{
	for (int iLoopWarPlan = NO_WARPLAN + 1;  iLoopWarPlan < NUM_WARPLAN_TYPES; ++iLoopWarPlan)
	{
		const WarPlanTypes eLoopWarPlan = static_cast<WarPlanTypes>(iLoopWarPlan);
		if (AI_isChosenWarPlan(eLoopWarPlan) && AI_getNumWarPlans(eLoopWarPlan) > 0)
			return true;
	}
	return false;
}

// advc.105: Body cut from AI_isChosenWar
bool CvTeamAI::AI_isChosenWarPlan(WarPlanTypes eWarPlanType)
{
	switch (eWarPlanType)
	{
	case WARPLAN_ATTACKED_RECENT:
	case WARPLAN_ATTACKED:
		return false;
	case WARPLAN_PREPARING_LIMITED:
	case WARPLAN_PREPARING_TOTAL:
	case WARPLAN_LIMITED:
	case WARPLAN_TOTAL:
	case WARPLAN_DOGPILE:
		return true;
	default: return false; // NO_WARPLAN
	}
}

/*	K-Mod: return a rating of our war success between -99 and 99.
	-99 means we losing and have very little hope of surviving.
	99 means we are soundly defeating our enemies.
	Zero is neutral (eg. no wars being fought).
	(Based on K-Mod code for Force Peace diplomacy voting.)
	Replacing AI_getWarSuccessCapitulationRatio
	(BETTER_BTS_AI_MOD, 03/20/10, jdog5000: War Strategy AI). */
int CvTeamAI::AI_getWarSuccessRating() const
{
	PROFILE_FUNC();
	// (Based on my code for Force Peace diplomacy voting.)

	int iMilitaryUnits = 0;
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kLoopPlayer.getTeam() == getID())
		{
			iMilitaryUnits += kLoopPlayer.AI().countNumMilitaryUnits(); // getNumMilitaryUnits();
		}
	}
	int iSuccessScale = iMilitaryUnits * GC.getDefineINT("WAR_SUCCESS_ATTACKING") / 5;

	int iThisTeamPower = getPower();
	int iScore = 0;

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		const CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iI);
		if (iI != getID() && isAtWar((TeamTypes)iI) && kLoopTeam.isAlive())
		{
			int iThisTeamSuccess = AI_getWarSuccess((TeamTypes)iI);
			int iOtherTeamSuccess = kLoopTeam.AI_getWarSuccess(getID());

			int iOtherTeamPower = kLoopTeam.getPower();

			iScore += (iThisTeamSuccess + iSuccessScale) * iThisTeamPower;
			iScore -= (iOtherTeamSuccess + iSuccessScale) * iOtherTeamPower;
		}
	}
	iScore = range((100 * iScore) / std::max(1, iThisTeamPower * iSuccessScale * 5), -99, 99);
	return iScore;
}

int CvTeamAI::AI_countEnemyPowerByArea(CvArea const& kArea) const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() != getID())
			{
				/************************************************************************************************/
				/* BETTER_BTS_AI_MOD                      01/11/09                                jdog5000      */
				/*                                                                                              */
				/* General AI                                                                                   */
				/************************************************************************************************/
				/* original BTS code
								if (isAtWar(GET_PLAYER((PlayerTypes)iI).getTeam()))
				*/
				// Count planned wars as well
				if (AI_getWarPlan(GET_PLAYER((PlayerTypes)iI).getTeam()) != NO_WARPLAN)
					/************************************************************************************************/
					/* BETTER_BTS_AI_MOD                       END                                                  */
					/************************************************************************************************/
				{
					iCount += kArea.getPower((PlayerTypes)iI);
				}
			}
		}
	}

	return iCount;
}

// K-Mod: (Note: this includes barbarian cities.)
int CvTeamAI::AI_countEnemyCitiesByArea(CvArea const& kArea) const // advc.003u: Moved from CvTeam
{
	int iCount = 0;
	for (PlayerTypes i = (PlayerTypes)0; i < MAX_PLAYERS; i = (PlayerTypes)(i + 1))
	{
		const CvPlayer& kLoopPlayer = GET_PLAYER(i);
		if (kLoopPlayer.isAlive() && AI_getWarPlan(kLoopPlayer.getTeam()) != NO_WARPLAN)
			iCount += kArea.getCitiesPerPlayer(i);
	}
	return iCount; // advc.001: was 'return 0'
}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      05/19/10                                jdog5000      */
/*                                                                                              */
/* War strategy AI                                                                              */
/************************************************************************************************/
int CvTeamAI::AI_countEnemyDangerByArea(CvArea const& kArea, TeamTypes eEnemyTeam) const
{
	PROFILE_FUNC();

	int iCount = 0;

	for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);

		if (pLoopPlot != NULL)
		{
			if (pLoopPlot->area() == &kArea)
			{
				if (pLoopPlot->getTeam() == getID())
				{
					iCount += pLoopPlot->plotCount(PUF_canDefendEnemy, getLeaderID(), false, NO_PLAYER, eEnemyTeam, PUF_isVisible, getLeaderID());
				}
			}
		}
	}

	return iCount;
}

// advc.104k: Same procedure as in BtS mostly
int CvTeamAI::AI_roundTradeVal(int iVal) const
{
	int rem = GLOBAL_DEFINE_DIPLOMACY_VALUE_REMAINDER;
	iVal -= iVal % rem;
	/*  Not sure if this lower bound is really needed. The BtS code
		(see CvPlayerAI::AI_roundTradeVal) doesn't have it. */
	if (isHuman())
		return std::max(iVal, rem);
	return iVal;
}
