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
#include "CvDLLFAStarIFaceBase.h" // K-Mod (currently used in AI_isLandTarget)
#include <algorithm>
#include <numeric>

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
	m_aiWarPlanStateCounter = new int[MAX_TEAMS];
	m_aiAtWarCounter = new int[MAX_TEAMS];
	m_aiAtPeaceCounter = new int[MAX_TEAMS];
	m_aiHasMetCounter = new int[MAX_TEAMS];
	m_aiOpenBordersCounter = new int[MAX_TEAMS];
	m_aiDefensivePactCounter = new int[MAX_TEAMS];
	m_aiShareWarCounter = new int[MAX_TEAMS];
	m_aiWarSuccess = new int[MAX_TEAMS];
	m_aiEnemyPeacetimeTradeValue = new int[MAX_TEAMS];
	m_aiEnemyPeacetimeGrantValue = new int[MAX_TEAMS];
	m_aiDamages = new int[MAX_TEAMS];
	m_aeWarPlan = new WarPlanTypes[MAX_TEAMS];


	AI_reset();
}


CvTeamAI::~CvTeamAI()
{
	AI_uninit();

	SAFE_DELETE_ARRAY(m_aiWarPlanStateCounter);
	SAFE_DELETE_ARRAY(m_aiAtWarCounter);
	SAFE_DELETE_ARRAY(m_aiAtPeaceCounter);
	SAFE_DELETE_ARRAY(m_aiHasMetCounter);
	SAFE_DELETE_ARRAY(m_aiOpenBordersCounter);
	SAFE_DELETE_ARRAY(m_aiDefensivePactCounter);
	SAFE_DELETE_ARRAY(m_aiShareWarCounter);
	SAFE_DELETE_ARRAY(m_aiWarSuccess);
	SAFE_DELETE_ARRAY(m_aiEnemyPeacetimeTradeValue);
	SAFE_DELETE_ARRAY(m_aiEnemyPeacetimeGrantValue);
	SAFE_DELETE_ARRAY(m_aiDamages);
	SAFE_DELETE_ARRAY(m_aeWarPlan);
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

	m_eWorstEnemy = NO_TEAM;
	
	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_aiWarPlanStateCounter[iI] = 0;
		m_aiAtWarCounter[iI] = 0;
		m_aiAtPeaceCounter[iI] = 0;
		m_aiHasMetCounter[iI] = 0;
		m_aiOpenBordersCounter[iI] = 0;
		m_aiDefensivePactCounter[iI] = 0;
		m_aiShareWarCounter[iI] = 0;
		m_aiWarSuccess[iI] = 0;
		m_aiEnemyPeacetimeTradeValue[iI] = 0;
		m_aiEnemyPeacetimeGrantValue[iI] = 0;
		m_aiDamages[iI] = 0;
	}

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_aeWarPlan[iI] = NO_WARPLAN;
	}

	m_aiEnemyCityDistance.clear();
	m_aiEnemyUnitDistance.clear();
}


void CvTeamAI::AI_doTurnPre()
{
	AI_doCounter();
}


void CvTeamAI::AI_doTurnPost()
{
	AI_updateWorstEnemy();

	AI_updateAreaStragies(false);
	
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

void CvTeamAI::AI_updateAreaStragies(bool bTargets)
{
	CvArea* pLoopArea;
	int iLoop;

	if (!(GC.getGameINLINE().isFinalInitialized()))
	{
		return;
	}

	for(pLoopArea = GC.getMapINLINE().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMapINLINE().nextArea(&iLoop))
	{
		pLoopArea->setAreaAIType(getID(), AI_calculateAreaAIType(pLoopArea));
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

int CvTeamAI::AI_countMilitaryWeight(CvArea* pArea) const
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


bool CvTeamAI::AI_isPrimaryArea(CvArea* pArea) const
{
	int iI;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				if (GET_PLAYER((PlayerTypes)iI).AI_isPrimaryArea(pArea))
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

	for(pLoopArea = GC.getMapINLINE().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMapINLINE().nextArea(&iLoop))
	{
		if (AI_isPrimaryArea(pLoopArea))
		{
			if (GET_TEAM(eTeam).countNumCitiesByArea(pLoopArea))
			{
				return true;
			}
		}
	}

	return false;
}

// K-Mod. Return true if this team and eTeam have at least one primary area in common.
bool CvTeamAI::AI_hasSharedPrimaryArea(TeamTypes eTeam) const
{
	FAssert(eTeam != getID());

	const CvTeamAI& kTeam = GET_TEAM(eTeam);

	int iLoop;
	for (CvArea* pLoopArea = GC.getMapINLINE().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMapINLINE().nextArea(&iLoop))
	{
		if (AI_isPrimaryArea(pLoopArea) && kTeam.AI_isPrimaryArea(pLoopArea))
			return true;
	}
	return false;
}
// K-Mod end

int CvTeamAI::AI_calculatePlotWarValue(TeamTypes eTeam) const
{
	FAssert(eTeam != getID());

	int iValue = 0;

	for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

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

AreaAITypes CvTeamAI::AI_calculateAreaAIType(CvArea* pArea, bool bPreparingTotal) const
{
	PROFILE_FUNC();

	// K-Mod. This function originally had "!isWater()" wrapping all of the code.
	// I've changed it to be more readable.
	if (pArea->isWater())
	{
		return AREAAI_NEUTRAL;
	}

	/*
	if (isBarbarian())
	{
		if ((pArea->getNumCities() - pArea->getCitiesPerPlayer(BARBARIAN_PLAYER)) == 0)
		{
			return AREAAI_ASSAULT;
		}

		if ((countNumAIUnitsByArea(pArea, UNITAI_ATTACK) + countNumAIUnitsByArea(pArea, UNITAI_ATTACK_CITY) + countNumAIUnitsByArea(pArea, UNITAI_PILLAGE) + countNumAIUnitsByArea(pArea, UNITAI_ATTACK_AIR)) > (((AI_countMilitaryWeight(pArea) * 20) / 100) + 1))
		{
			return AREAAI_OFFENSIVE; // XXX does this ever happen?
		}

		return AREAAI_MASSING;
	}
	*/

	bool bRecentAttack = false;
	bool bTargets = false;
	bool bChosenTargets = false;
	bool bDeclaredTargets = false;

	bool bAssault = false;
	bool bPreparingAssault = false;

	// int iOffensiveThreshold = (bPreparingTotal ? 25 : 20); // K-Mod, I don't use this.
	int iAreaCities = countNumCitiesByArea(pArea);
	int iWarSuccessRating = AI_getWarSuccessRating(); // K-Mod

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (AI_getWarPlan((TeamTypes)iI) != NO_WARPLAN)
			{
				FAssert(((TeamTypes)iI) != getID());
				FAssert(isHasMet((TeamTypes)iI) || GC.getGameINLINE().isOption(GAMEOPTION_ALWAYS_WAR));

				if (AI_getWarPlan((TeamTypes)iI) == WARPLAN_ATTACKED_RECENT)
				{
					FAssert(isAtWar((TeamTypes)iI));
					bRecentAttack = true;
				}

				if ((GET_TEAM((TeamTypes)iI).countNumCitiesByArea(pArea) > 0) || (GET_TEAM((TeamTypes)iI).countNumUnitsByArea(pArea) > 4))
				{
					bTargets = true;

					if (AI_isChosenWar((TeamTypes)iI))
					{
						bChosenTargets = true;

						if ((isAtWar((TeamTypes)iI)) ? (AI_getAtWarCounter((TeamTypes)iI) < 10) : AI_isSneakAttackReady((TeamTypes)iI))
						{
							bDeclaredTargets = true;
						}
					}
				}
				else
				{
					bAssault = true;
					if (AI_isSneakAttackPreparing((TeamTypes)iI))
					{
						bPreparingAssault = true;
					}
				}
			}
		}
	}

	// K-Mod - based on idea from BBAI
	if (bTargets)
	{
		if (iAreaCities > 0 && getAtWarCount() > 0)
		{
			int iPower = countPowerByArea(pArea);
			int iEnemyPower = countEnemyPowerByArea(pArea);

			iPower *= 100 + iWarSuccessRating + (bChosenTargets ? 100 : 50);
			iEnemyPower *= 100;
			// it would be nice to put some personality modifiers into this. But this is a Team function. :(
			if (iPower < iEnemyPower)
			{
				return AREAAI_DEFENSIVE;
			}
		}
	}
	// K-Mod end

	if (bDeclaredTargets)
	{
		return AREAAI_OFFENSIVE;
	}

	if (bTargets)
	{
		/* BBAI code. -- This code has two major problems.
		* Firstly, it makes offense more likely when we are in more wars.
		* Secondly, it chooses offense based on how many offense units we have -- but offense units are built for offense areas!
		*
		// AI_countMilitaryWeight is based on this team's pop and cities ... if this team is the biggest, it will over estimate needed units
		int iMilitaryWeight = AI_countMilitaryWeight(pArea);
		int iCount = 1;

		for( int iJ = 0; iJ < MAX_CIV_TEAMS; iJ++ )
		{
		if( iJ != getID() && GET_TEAM((TeamTypes)iJ).isAlive() )
		{
		if( !(GET_TEAM((TeamTypes)iJ).isBarbarian() || GET_TEAM((TeamTypes)iJ).isMinorCiv()) )
		{
		if( AI_getWarPlan((TeamTypes)iJ) != NO_WARPLAN )
		{
		iMilitaryWeight += GET_TEAM((TeamTypes)iJ).AI_countMilitaryWeight(pArea);
		iCount++;

		if( GET_TEAM((TeamTypes)iJ).isAVassal() )
		{
		for( int iK = 0; iK < MAX_CIV_TEAMS; iK++ )
		{
		if( iK != getID() && GET_TEAM((TeamTypes)iK).isAlive() )
		{
		if( GET_TEAM((TeamTypes)iJ).isVassal((TeamTypes)iK) )
		{
		iMilitaryWeight += GET_TEAM((TeamTypes)iK).AI_countMilitaryWeight(pArea);
		}
		}
		}
		}
		}
		}
		}
		}

		iMilitaryWeight /= iCount;
		if ((countNumAIUnitsByArea(pArea, UNITAI_ATTACK) + countNumAIUnitsByArea(pArea, UNITAI_ATTACK_CITY) + countNumAIUnitsByArea(pArea, UNITAI_PILLAGE) + countNumAIUnitsByArea(pArea, UNITAI_ATTACK_AIR)) > (((iMilitaryWeight * iOffensiveThreshold) / 100) + 1))
		{
		return AREAAI_OFFENSIVE;
		}
		*/
		// K-Mod. I'm not sure how best to do this yet. Let me just try a rough idea for now.
		// I'm using AI_countMilitaryWeight; but what I really want is "border terriory which needs defending"
		int iOurRelativeStrength = 100 * countPowerByArea(pArea) / (AI_countMilitaryWeight(pArea) + 20);
		iOurRelativeStrength *= 100 + (bDeclaredTargets ? 30 : 0) + (bPreparingTotal ? -20 : 0) + iWarSuccessRating / 2;
		iOurRelativeStrength /= 100;
		int iEnemyRelativeStrength = 0;
		bool bEnemyCities = false;

		for (int iJ = 0; iJ < MAX_TEAMS; iJ++)
		{
			const CvTeamAI& kLoopTeam = GET_TEAM((TeamTypes)iJ);
			if (iJ != getID() && kLoopTeam.isAlive() && AI_getWarPlan((TeamTypes)iJ) != NO_WARPLAN)
			{
				int iPower = 100 * kLoopTeam.countPowerByArea(pArea);
				int iCommitment = (bPreparingTotal ? 30 : 20) + kLoopTeam.AI_countMilitaryWeight(pArea) * ((isAtWar((TeamTypes)iJ) ? 1 : 2) + kLoopTeam.getAtWarCount()) / 2;
				iPower /= iCommitment;
				iEnemyRelativeStrength += iPower;
				if (kLoopTeam.countNumCitiesByArea(pArea) > 0)
					bEnemyCities = true;
			}
		}
		if (bEnemyCities && iOurRelativeStrength > iEnemyRelativeStrength)
			return AREAAI_OFFENSIVE;
		// K-Mod end
	}

	if (bTargets)
	{
		/*
		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
		{
			CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);

			if (kPlayer.isAlive())
			{
				if (kPlayer.getTeam() == getID())
				{
					if (kPlayer.AI_isDoStrategy(AI_STRATEGY_DAGGER) || kPlayer.AI_isDoStrategy(AI_STRATEGY_FINAL_WAR))
					{
						if (pArea->getCitiesPerPlayer((PlayerTypes)iPlayer) > 0)
						{
							return AREAAI_MASSING;
						}
					}
				}
			}
		}
		*/
		if (bRecentAttack)
		{
			int iPower = countPowerByArea(pArea);
			int iEnemyPower = countEnemyPowerByArea(pArea);
			if (iPower > iEnemyPower)
			{
				return AREAAI_MASSING;
			}
			return AREAAI_DEFENSIVE;
		}
	}

	if (iAreaCities > 0)
	{
		if (countEnemyDangerByArea(pArea) > iAreaCities)
		{
			return AREAAI_DEFENSIVE;
		}
	}

	if (bChosenTargets)
	{
		return AREAAI_MASSING;
	}

	if (bTargets)
	{
		if (iAreaCities > (getNumMembers() * 3))
		{
			if (GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI) || GC.getGameINLINE().isOption(GAMEOPTION_ALWAYS_WAR) || (countPowerByArea(pArea) > ((countEnemyPowerByArea(pArea) * 3) / 2)))
			{
				return AREAAI_MASSING;
			}
		}
		return AREAAI_DEFENSIVE;
	}
	else
	{
		if (bAssault)
		{
			if (AI_isPrimaryArea(pArea))
			{
				if (bPreparingAssault)
				{
					return AREAAI_ASSAULT_MASSING;
				}
			}
			/*
			else if (countNumCitiesByArea(pArea) > 0)
			{
				return AREAAI_ASSAULT_ASSIST;
			}
			*/
			return AREAAI_ASSAULT;
		}
	}
	return AREAAI_NEUTRAL;
}

// K-Mod. return a rating of our war success between -99 and 99.
// -99 means we losing and have very little hope of surviving. 99 means we are soundly defeating our enemies. Zero is neutral (eg. no wars being fought).
int CvTeamAI::AI_getWarSuccessRating() const
{
	PROFILE_FUNC();
	// (Based on my code for Force Peace diplomacy voting.)

	/*
	int iMilitaryUnits = 0;
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kLoopPlayer.getTeam() == getID())
		{
			iMilitaryUnits += kLoopPlayer.getNumMilitaryUnits();
		}
	}
	*/
	//int iSuccessScale = iMilitaryUnits * GC.getDefineINT("WAR_SUCCESS_ATTACKING") / 5;
	const int iSuccessScale = 100;

	int iThisTeamPower = getPower();
	int iScore = 0;

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		const CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iI);
		if (iI != getID() && isAtWar((TeamTypes)iI) && kLoopTeam.isAlive() /*&& !kLoopTeam.isAVassal()*/)
		{
			int iThisTeamSuccess = AI_getWarSuccess((TeamTypes)iI);
			int iOtherTeamSuccess = kLoopTeam.AI_getWarSuccess(getID());

			int iOtherTeamPower = kLoopTeam.getPower();

			iScore += (iThisTeamSuccess + iSuccessScale) * iThisTeamPower;
			iScore -= (iOtherTeamSuccess + iSuccessScale) * iOtherTeamPower;
		}
	}
	iScore = range((100 * iScore) / std::max(1, iThisTeamPower*iSuccessScale * 5), -99, 99);
	return iScore;
}
// K-Mod end

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
		return ((GC.getMapINLINE().maxPlotDistance() * (iMaxDistance - ((iTotalDistance / iCount) - iMinDistance))) / iMaxDistance);
	}

	return 0;
}


// This function has been completely rewritten for K-Mod. The original BtS code, and the BBAI code have been deleted.
bool CvTeamAI::AI_isLandTarget(TeamTypes eTeam) const
{
	PROFILE_FUNC();
	const CvTeamAI& kOtherTeam = GET_TEAM(eTeam);

	int iLoop;
	for (CvArea* pLoopArea = GC.getMapINLINE().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMapINLINE().nextArea(&iLoop))
	{
		if (AI_isPrimaryArea(pLoopArea) && kOtherTeam.AI_isPrimaryArea(pLoopArea))
			return true;
	}

	for (PlayerTypes i = (PlayerTypes)0; i < MAX_PLAYERS; i = (PlayerTypes)(i + 1))
	{
		const CvPlayerAI& kOurPlayer = GET_PLAYER(i);
		if (kOurPlayer.getTeam() != getID() || !kOurPlayer.isAlive())
			continue;

		int iL1;
		for (CvCity* pOurCity = kOurPlayer.firstCity(&iL1); pOurCity; pOurCity = kOurPlayer.nextCity(&iL1))
		{
			if (!kOurPlayer.AI_isPrimaryArea(pOurCity->area()))
				continue;
			// city in a primary area.
			for (PlayerTypes j = (PlayerTypes)0; j < MAX_PLAYERS; j = (PlayerTypes)(j + 1))
			{
				const CvPlayerAI& kTheirPlayer = GET_PLAYER(j);
				if (kTheirPlayer.getTeam() != eTeam || !kTheirPlayer.isAlive() || !kTheirPlayer.AI_isPrimaryArea(pOurCity->area()))
					continue;

				std::vector<TeamTypes> teamVec;
				teamVec.push_back(getID());
				teamVec.push_back(eTeam);
				FAStar* pTeamStepFinder = gDLL->getFAStarIFace()->create();
				gDLL->getFAStarIFace()->Initialize(pTeamStepFinder, GC.getMapINLINE().getGridWidthINLINE(), GC.getMapINLINE().getGridHeightINLINE(), GC.getMapINLINE().isWrapXINLINE(), GC.getMapINLINE().isWrapYINLINE(), stepDestValid, stepHeuristic, stepCost, teamStepValid, stepAdd, NULL, NULL);
				gDLL->getFAStarIFace()->SetData(pTeamStepFinder, &teamVec);

				int iL2;
				for (CvCity* pTheirCity = kTheirPlayer.firstCity(&iL2); pTheirCity; pTheirCity = kTheirPlayer.nextCity(&iL2))
				{
					if (pTheirCity->area() != pOurCity->area())
						continue;


					if (gDLL->getFAStarIFace()->GeneratePath(pTeamStepFinder, pOurCity->getX_INLINE(), pOurCity->getY_INLINE(), pTheirCity->getX_INLINE(), pTheirCity->getY_INLINE(), false, 0, true))
					{
						// good.
						gDLL->getFAStarIFace()->destroy(pTeamStepFinder);
						return true;
					}
				}
				gDLL->getFAStarIFace()->destroy(pTeamStepFinder);
			}
		}
	}

	return false;
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

// K-Mod. New war evaluation functions. WIP
// Very rough estimate of what would be gained by conquering the target - in units of Gold/turn (kind of).
int CvTeamAI::AI_warSpoilsValue(TeamTypes eTarget, WarPlanTypes eWarPlan) const
{
	PROFILE_FUNC();

	FAssert(eTarget != getID());
	const CvTeamAI& kTargetTeam = GET_TEAM(eTarget);
	bool bAggresive = GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI);

	// Deny factor: the percieved value of denying the enemy team of its resources (generally)
	int iDenyFactor = bAggresive
		? 40 - AI_getAttitudeWeight(eTarget) / 3
		: 20 - AI_getAttitudeWeight(eTarget) / 2;

	iDenyFactor += AI_getWorstEnemy() == eTarget ? 20 : 0; // (in addition to attitude pentalities)

	/*
	if (kTargetTeam.AI_isAnyMemberDoVictoryStrategyLevel3())
	{
		if (kTargetTeam.AI_isAnyMemberDoVictoryStrategyLevel4())
		{
			iDenyFactor += AI_isAnyMemberDoVictoryStrategyLevel4() ? 50 : 30;
		}

		if (bAggresive || AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_CONQUEST3))
		{
			iDenyFactor += 20;
		}

		if (GC.getGameINLINE().getTeamRank(eTarget) < GC.getGameINLINE().getTeamRank(getID()))
		{
			iDenyFactor += 10;
		}
	}
	if (AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_CONQUEST4 | AI_VICTORY_DOMINATION4))
	{
		iDenyFactor += 20;
	}
	*/

	int iRankDelta = GC.getGameINLINE().getTeamRank(getID()) - GC.getGameINLINE().getTeamRank(eTarget);
	if (iRankDelta > 0)
	{
		int iRankHate = 0;
		/*
		for (PlayerTypes eLoopPlayer = (PlayerTypes)0; eLoopPlayer < MAX_PLAYERS; eLoopPlayer = (PlayerTypes)(eLoopPlayer + 1))
		{
			const CvPlayerAI& kLoopPlayer = GET_PLAYER(eLoopPlayer);
			if (kLoopPlayer.getTeam() == getID() && kLoopPlayer.isAlive())
				iRankHate += GC.getLeaderHeadInfo(kLoopPlayer.getLeaderType()).getWorseRankDifferenceAttitudeChange(); // generally around 0-3
		}
		*/

		if (iRankHate > 0)
		{
			int iTotalTeams = GC.getGameINLINE().countCivTeamsEverAlive();
			iDenyFactor += (100 - AI_getAttitudeWeight(eTarget)) * (iRankHate * iRankDelta + (iTotalTeams + 1) / 2) / std::max(1, 8 * (iTotalTeams + 1)*getAliveCount());
			// that's a max of around 200 * 3 / 8. ~ 75
		}
	}

	//bool bImminentVictory = kTargetTeam.AI_getLowestVictoryCountdown() >= 0;

	bool bTotalWar = eWarPlan == WARPLAN_TOTAL || eWarPlan == WARPLAN_PREPARING_TOTAL;
	bool bOverseasWar = !AI_hasSharedPrimaryArea(eTarget);

	int iGainedValue = 0;
	int iDeniedValue = 0;

	// Cities & Land
	int iPopCap = 2 + getTotalPopulation() / std::max(1, getNumCities()); // max number of plots to consider the value of.
	int iYieldMultiplier = 0; // multiplier for the value of plot yields.
	/*
	for (PlayerTypes eLoopPlayer = (PlayerTypes)0; eLoopPlayer < MAX_PLAYERS; eLoopPlayer = (PlayerTypes)(eLoopPlayer + 1))
	{
		const CvPlayerAI& kLoopPlayer = GET_PLAYER(eLoopPlayer);
		if (kLoopPlayer.getTeam() != getID() || !kLoopPlayer.isAlive())
			continue;
		int iFoodMulti = kLoopPlayer.AI_averageYieldMultiplier(YIELD_FOOD);
		iYieldMultiplier += kLoopPlayer.AI_averageYieldMultiplier(YIELD_PRODUCTION) * iFoodMulti / 100;
		iYieldMultiplier += kLoopPlayer.AI_averageYieldMultiplier(YIELD_COMMERCE) * iFoodMulti / 100;
	}
	*/
	iYieldMultiplier /= std::max(1, 2 * getAliveCount());
	// now.. here's a bit of ad-hoccery.
	// the actual yield multiplayer is not the only thing that goes up as the game progresses.
	// the raw produce of land also tends to increase, as improvements become more powerful. Therefore...:
	iYieldMultiplier = iYieldMultiplier * (1 + GET_PLAYER(getLeaderID()).getCurrentEra() + GC.getNumEraInfos()) / std::max(1, GC.getNumEraInfos());
	//

	std::set<int> close_areas; // set of area IDs for which the enemy has cities close to ours.
	for (PlayerTypes eLoopPlayer = (PlayerTypes)0; eLoopPlayer < MAX_PLAYERS; eLoopPlayer = (PlayerTypes)(eLoopPlayer + 1))
	{
		const CvPlayerAI& kLoopPlayer = GET_PLAYER(eLoopPlayer);
		if (kLoopPlayer.getTeam() != eTarget || !kLoopPlayer.isAlive())
			continue;

		int iLoop;
		for (CvCity* pLoopCity = kLoopPlayer.firstCity(&iLoop); pLoopCity; pLoopCity = kLoopPlayer.nextCity(&iLoop))
		{
			/*
			if (!AI_deduceCitySite(pLoopCity))
				continue;
			*/
			int iCityValue = pLoopCity->getPopulation();

			bool bCoastal = pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN());
			iCityValue += bCoastal ? 2 : 0;

			// plots
			std::vector<int> plot_values;
			for (int i = 1; i < NUM_CITY_PLOTS; i++) // don't count city plot
			{
				CvPlot* pLoopPlot = pLoopCity->getCityIndexPlot(i);
				if (!pLoopPlot || !pLoopPlot->isRevealed(getID(), false) || pLoopPlot->getWorkingCity() != pLoopCity)
					continue;

				if (pLoopPlot->isWater() && !(bCoastal && pLoopPlot->calculateNatureYield(YIELD_FOOD, getID()) >= GC.getFOOD_CONSUMPTION_PER_POPULATION()))
					continue;

				// This is a very rough estimate of the value of the plot. It's a bit ad-hoc. I'm sorry about that, but I want it to be fast.
				//BonusTypes eBonus = pLoopPlot->getBonusType(getID());
				int iPlotValue = 0;
				iPlotValue += 3 * pLoopPlot->calculateBestNatureYield(YIELD_FOOD, getID()); // don't ignore floodplains
				//iPlotValue += 2 * pLoopPlot->calculateNatureYield(YIELD_PRODUCTION, getID(), true); // ignore forest
				iPlotValue += GC.getTerrainInfo(pLoopPlot->getTerrainType()).getYield(YIELD_FOOD) >= GC.getFOOD_CONSUMPTION_PER_POPULATION() ? 1 : 0; // bonus for grassland
				iPlotValue += pLoopPlot->isRiver() ? 1 : 0;
				/*
				if (pLoopPlot->getBonusType(getID()) != NO_BONUS)
					iPlotValue = iPlotValue * 3 / 2;
				iPlotValue += pLoopPlot->getYield(YIELD_COMMERCE) / 2; // include some value for existing towns.
				*/
				plot_values.push_back(iPlotValue);
			}
			std::partial_sort(plot_values.begin(), plot_values.begin() + std::min(iPopCap, (int)plot_values.size()), plot_values.end(), std::greater<int>());
			iCityValue = std::accumulate(plot_values.begin(), plot_values.begin() + std::min(iPopCap, (int)plot_values.size()), iCityValue);
			iCityValue = iCityValue * iYieldMultiplier / 100;

			// holy city value
			/*
			for (ReligionTypes i = (ReligionTypes)0; i < GC.getNumReligionInfos(); i = (ReligionTypes)(i + 1))
			{
				if (pLoopCity->isHolyCity(i))
					iCityValue += std::max(0, GC.getGameINLINE().countReligionLevels(i) / (pLoopCity->hasShrine(i) ? 1 : 2) - 4);
				// note: the -4 at the end is mostly there to offset the 'wonder' value that will be added later.
				// I don't want to double-count the value of the shrine, and the religion without the shrine isn't worth much anyway.
			}
			// corp HQ value
			for (CorporationTypes i = (CorporationTypes)0; i < GC.getNumCorporationInfos(); i = (CorporationTypes)(i + 1))
			{
				if (pLoopCity->isHeadquarters(i))
					iCityValue += std::max(0, 2 * GC.getGameINLINE().countCorporationLevels(i) - 4);
			}

			// wonders
			iCityValue += 4 * pLoopCity->getNumActiveWorldWonders();

			// denied
			iDeniedValue += iCityValue * iDenyFactor / 100;
			if (2 * pLoopCity->getCulture(eLoopPlayer) > pLoopCity->getCultureThreshold(GC.getGameINLINE().culturalVictoryCultureLevel()))
			{
				iDeniedValue += (kLoopPlayer.AI_isDoVictoryStrategy(AI_VICTORY_CULTURE4) ? 100 : 30) * iDenyFactor / 100;
			}
			if (bImminentVictory && pLoopCity->isCapital())
			{
				iDeniedValue += 200 * iDenyFactor / 100;
			}
			*/

			// gained
			int iGainFactor = 0;
			if (bTotalWar)
			{
				if (AI_isPrimaryArea(pLoopCity->area()))
					iGainFactor = 70;
				else
				{
					if (bOverseasWar && GET_PLAYER(pLoopCity->getOwnerINLINE()).AI_isPrimaryArea(pLoopCity->area()))
						iGainFactor = 45;
					else
						iGainFactor = 30;

					//iGainFactor += AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_CONQUEST3 | AI_VICTORY_DOMINATION2) ? 10 : 0;
				}
			}
			else
			{
				if (AI_isPrimaryArea(pLoopCity->area()))
					iGainFactor = 40;
				else
					iGainFactor = 25;
			}
			/*
			if (pLoopCity->AI_highestTeamCloseness(getID()) > 0)
			{
				iGainFactor += 30;
				close_areas.insert(pLoopCity->getArea());
			}
			*/
			iGainedValue += iCityValue * iGainFactor / 100;
			//
		}
	}

	/*
	// Resources
	std::vector<int> bonuses(GC.getNumBonusInfos(), 0); // percentage points
	for (int i = 0; i < GC.getMapINLINE().numPlotsINLINE(); i++)
	{
		CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(i);

		if (pLoopPlot->getTeam() == eTarget)
		{
			// note: There are ways of knowning that the team has resources even if the plot cannot be seen; so my handling of isRevealed is a bit ad-hoc.
			BonusTypes eBonus = pLoopPlot->getNonObsoleteBonusType(getID());
			if (eBonus != NO_BONUS)
			{
				if (pLoopPlot->isRevealed(getID(), false) && AI_isPrimaryArea(pLoopPlot->area()))
					bonuses[eBonus] += bTotalWar ? 60 : 20;
				else
					bonuses[eBonus] += bTotalWar ? 20 : 0;
			}
		}
	}
	for (BonusTypes i = (BonusTypes)0; i < GC.getNumBonusInfos(); i = (BonusTypes)(i + 1))
	{
		if (bonuses[i] > 0)
		{
			int iBonusValue = 0;
			int iMissing = 0;
			for (PlayerTypes eLoopPlayer = (PlayerTypes)0; eLoopPlayer < MAX_PLAYERS; eLoopPlayer = (PlayerTypes)(eLoopPlayer + 1))
			{
				const CvPlayerAI& kLoopPlayer = GET_PLAYER(eLoopPlayer);
				if (kLoopPlayer.getTeam() == getID() && kLoopPlayer.isAlive())
				{
					iBonusValue += kLoopPlayer.AI_bonusVal(i, 0, true);
					if (kLoopPlayer.getNumAvailableBonuses(i) == 0)
						iMissing++;
				}
			}
			iBonusValue += GC.getBonusInfo(i).getAIObjective(); // (support for mods.)
			iBonusValue = iBonusValue * getNumCities() * (std::min(100 * iMissing, bonuses[i]) + std::max(0, bonuses[i] - 100 * iMissing) / 8) / std::max(1, 400 * getAliveCount());
			//
			iGainedValue += iBonusValue;
			// ignore denied value.
		}
	}
	*/
	// magnify the gained value based on how many of the target's cities are in close areas
	int iCloseCities = 0;
	for (std::set<int>::iterator it = close_areas.begin(); it != close_areas.end(); ++it)
	{
		CvArea* pLoopArea = GC.getMapINLINE().getArea(*it);
		if (AI_isPrimaryArea(pLoopArea))
		{
			for (PlayerTypes eLoopPlayer = (PlayerTypes)0; eLoopPlayer < MAX_PLAYERS; eLoopPlayer = (PlayerTypes)(eLoopPlayer + 1))
			{
				const CvPlayerAI& kLoopPlayer = GET_PLAYER(eLoopPlayer);
				if (kLoopPlayer.getTeam() == eTarget)
					iCloseCities += pLoopArea->getCitiesPerPlayer(eLoopPlayer);
			}
		}
	}
	iGainedValue *= 75 + 50 * iCloseCities / std::max(1, kTargetTeam.getNumCities());
	iGainedValue /= 100;

	/*
	// amplify the gained value if we are aiming for a conquest or domination victory
	if (AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_CONQUEST2 | AI_VICTORY_DOMINATION2))
		iGainedValue = iGainedValue * 4 / 3;
	*/
	
	// reduce the gained value based on how many other teams are at war with the target
	// similar to the way the target's strength estimate is reduced.
	iGainedValue *= 2;
	iGainedValue /= (isAtWar(eTarget) ? 1 : 2) + kTargetTeam.getAtWarCount();

	return iGainedValue + iDeniedValue;
}

int CvTeamAI::AI_warCommitmentCost(TeamTypes eTarget, WarPlanTypes eWarPlan) const
{
	int iTotalCost = 100;
#if 0
	PROFILE_FUNC();
	// Things to consider:
	//
	// risk of losing cities
	// relative unit strength
	// relative current power
	// productivity
	// war weariness
	// diplomacy
	// opportunity cost (need for expansion, research, etc.)

	// For most parts of the calculation, it is actually more important to consider the master of the target rather than the target itself.
	// (this is important for defensive pacts; and it also affects relative power, relative productivity, and so on.)
	//TeamTypes eTargetMaster = GET_TEAM(eTarget).getMasterTeam();
	//FAssert(eTargetMaster == eTarget || GET_TEAM(eTarget).isAVassal());

	//const CvTeamAI& kTargetMasterTeam = GET_TEAM(eTargetMaster);

	bool bTotalWar = eWarPlan == WARPLAN_TOTAL || eWarPlan == WARPLAN_PREPARING_TOTAL;
	bool bAttackWar = bTotalWar || (eWarPlan == WARPLAN_DOGPILE /*&& kTargetMasterTeam.getAtWarCount(true) + (isAtWar(eTarget) ? 0 : 1) > 1*/);
	bool bPendingDoW = !isAtWar(eTarget) && eWarPlan != WARPLAN_ATTACKED && eWarPlan != WARPLAN_ATTACKED_RECENT;

	int iTotalCost = 0;

	// Estimate of military production costs
	
	{
		// Base commitment for a war of this type.
		int iCommitmentPerMil = bTotalWar ? 540 : 250;
		const int iBaseline = -25; // this value will be added to iCommitmentPerMil at the end of the calculation.

								   // scale based on our current strength relative to our enemies.
								   // cf. with code in AI_calculateAreaAIType
		{
			int iWarSuccessRating = isAtWar(eTarget) ? AI_getWarSuccessRating() : 0;
			int iOurRelativeStrength = 100 * getPower() / (AI_countMilitaryWeight(0) + 20); // whether to include vassals is a tricky issue...
			// Sum the relative strength for all enemies, including existing wars and wars with civs attached to the target team.
			int iEnemyRelativeStrength = 0;
			
			int iFreePowerBonus = GC.getUnitInfo(GC.getGameINLINE().getBestLandUnit()).getPowerValue() * 2;
			for (TeamTypes i = (TeamTypes)0; i < MAX_TEAMS; i = (TeamTypes)(i + 1))
			{
				const CvTeamAI& kLoopTeam = GET_TEAM(i);
				if (!kLoopTeam.isAlive() || i == getID() /*|| kLoopTeam.isVassal(getID())*/)
					continue;

				// note: this still doesn't count vassal siblings. (ie. if the target is a vassal, this will not count the master's other vassals.)
				// also, the vassals of defensive pact civs are currently not counted either.
				if (isAtWar(i)  /*|| i == eTargetMaster || kLoopTeam.isVassal(eTargetMaster) || (bPendingDoW && kLoopTeam.isDefensivePact(eTargetMaster))*/)
				{
					// the + power is meant to account for the fact that the target may get stronger while we are planning for war - especially early in the game.
					// use a slightly reduced value if we're actually not intending to attack this target. (full weight to all enemies in defensive wars)
					int iWeight = !bAttackWar || isAtWar(i) || i == eTarget ? 100 : 80;
					iEnemyRelativeStrength += iWeight * ((isAtWar(i) ? 0 : iFreePowerBonus) + kLoopTeam.getPower()) / (((isAtWar(i) ? 1 : 2) + kLoopTeam.getAtWarCount())*kLoopTeam.AI_countMilitaryWeight(0) / 2 + 20);
				}
			}
			//

			//iCommitmentPerMil = iCommitmentPerMil * (100 * iEnemyRelativeStrength) / std::max(1, iOurRelativeStrength * (100+iWarSuccessRating/2));
			iCommitmentPerMil = iCommitmentPerMil * iEnemyRelativeStrength / std::max(1, iOurRelativeStrength);
		}

		// scale based on the relative size of our civilizations.
		int iOurProduction = AI_estimateTotalYieldRate(YIELD_PRODUCTION); // (note: this is separate from our total production, because I use it again a bit later.)
		{
			int iOurTotalProduction = iOurProduction * 100;
			int iEnemyTotalProduction = 0;
			const int iVassalFactor = 60; // only count some reduced percentage of vassal production.
										  // Note: I've chosen not to count the production of the target's other enemies.

			for (TeamTypes i = (TeamTypes)0; i < MAX_TEAMS; i = (TeamTypes)(i + 1))
			{
				const CvTeamAI& kLoopTeam = GET_TEAM(i);
				if (!kLoopTeam.isAlive() || i == getID()) // our team is already counted.
					continue;
				/*
				if (kLoopTeam.isVassal(getID()))
					iOurTotalProduction += kLoopTeam.AI_estimateTotalYieldRate(YIELD_PRODUCTION) * iVassalFactor;
				else if (i == eTargetMaster)
					iEnemyTotalProduction += kLoopTeam.AI_estimateTotalYieldRate(YIELD_PRODUCTION) * 100;
				else if (kLoopTeam.isVassal(eTargetMaster))
					iEnemyTotalProduction += kLoopTeam.AI_estimateTotalYieldRate(YIELD_PRODUCTION) * iVassalFactor;
				
				else if (isAtWar(i) || (bPendingDoW && kLoopTeam.isDefensivePact(eTargetMaster)))
					iEnemyTotalProduction += kLoopTeam.AI_estimateTotalYieldRate(YIELD_PRODUCTION) * 100;
				*/
			}

			iCommitmentPerMil *= 6 * iEnemyTotalProduction + iOurTotalProduction;
			iCommitmentPerMil /= std::max(1, iEnemyTotalProduction + 6 * iOurTotalProduction);
		}

		// scale based on the relative strengths of our units
		{
			int iEnemyUnit = std::max(30, kTargetMasterTeam.getTypicalUnitValue(NO_UNITAI, DOMAIN_LAND));
			int iOurAttackUnit = std::max(30, getTypicalUnitValue(UNITAI_ATTACK, DOMAIN_LAND));
			int iOurDefenceUnit = std::max(30, getTypicalUnitValue(UNITAI_CITY_DEFENSE, DOMAIN_LAND));
			int iHighScale = 30 + 70 * std::max(iOurAttackUnit, iOurDefenceUnit) / iEnemyUnit;
			int iLowScale = 10 + 90 * std::min(iOurAttackUnit, iOurDefenceUnit) / iEnemyUnit;

			iCommitmentPerMil = std::min(iCommitmentPerMil, 300) * 100 / iHighScale + std::max(0, iCommitmentPerMil - 300) * 100 / iLowScale;

			// Adjust for overseas wars
			if (!AI_hasSharedPrimaryArea(eTarget))
			{
				int iOurNavy = getTypicalUnitValue(NO_UNITAI, DOMAIN_SEA);
				int iEnemyNavy = std::max(1, kTargetMasterTeam.getTypicalUnitValue(NO_UNITAI, DOMAIN_SEA)); // (using max here to avoid div-by-zero later on)

																											// rescale (unused)
																											/* {
																											int x = std::max(2, iOurNavy + iEnemyNavy) / 2;
																											iOurNavy = iOurNavy * 100 / x;
																											iEnemyNavy = iEnemyNavy * 100 / x;
																											} */

																											// Note: Commitment cost is currently meant to take into account risk as well as resource requirements.
																											//       But with overseas wars, the relative strength of navy units effects these things differently.
																											//       If our navy is much stronger than theirs, then our risk is low but we still need to commit a
																											//       just as much resources to win the land-war for an invasion.
																											//       If their navy is stronger than ours, our risk is high and our resources will be higher too.
																											//
																											//       The current calculations are too simplistic to explicitly specify all that stuff.
				if (bTotalWar)
				{
					//iCommitmentPerMil = iCommitmentPerMil * (4*iOurNavy + 5*iEnemyNavy) / (8*iOurNavy + 1*iEnemyNavy);
					//iCommitmentPerMil = iCommitmentPerMil * 200 / std::min(200, iLowScale + iHighScale);
					//
					iCommitmentPerMil = iCommitmentPerMil * 200 / std::min(240, (iLowScale + iHighScale) * (9 * iOurNavy + 1 * iEnemyNavy) / (6 * iOurNavy + 4 * iEnemyNavy));
				}
				else
					iCommitmentPerMil = iCommitmentPerMil * (1 * iOurNavy + 4 * iEnemyNavy) / (4 * iOurNavy + 1 * iEnemyNavy);
			}
		}

		// increase the cost for distant targets...
		if (AI_teamCloseness(eTarget) == 0)
		{
			// ... in the early game.
			if (getNumCities() < GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getTargetNumCities() * getAliveCount())
				iCommitmentPerMil = iCommitmentPerMil * 3 / 2;
			/* else
			iCommitmentPerMil = iCommitmentPerMil * 5/4; */

			// ... and for total war
			if (bTotalWar)
				iCommitmentPerMil = iCommitmentPerMil * 5 / 4;
		}

		iCommitmentPerMil += iBaseline; // The baseline should be a negative value which represents some amount of "free" commitment.

		if (iCommitmentPerMil > 0)
		{
			// iCommitmentPerMil will be multiplied by a rough estimate of the total resources this team could devote to war.
			int iCommitmentPool = iOurProduction * 3 + AI_estimateTotalYieldRate(YIELD_COMMERCE); // cf. AI_yieldWeight
																								  // Note: it would probably be good to take into account the expected increase in unit spending - but that's a bit tricky.

																								  // sometimes are resources are more in demand than other times...
			int iPoolMultiplier = 0;
			for (PlayerTypes eLoopPlayer = (PlayerTypes)0; eLoopPlayer < MAX_PLAYERS; eLoopPlayer = (PlayerTypes)(eLoopPlayer + 1))
			{
				const CvPlayerAI& kLoopPlayer = GET_PLAYER(eLoopPlayer);
				if (kLoopPlayer.getTeam() == getID() && kLoopPlayer.isAlive())
				{
					iPoolMultiplier += 100;
					// increase value if we are still trying to expand peacefully. Now, the minimum site value is pretty arbitrary...
					int iSites = kLoopPlayer.AI_getNumPrimaryAreaCitySites(kLoopPlayer.AI_getMinFoundValue() * 2); // note, there's a small cap on the number of sites, around 3.
					if (iSites > 0)
					{
						iPoolMultiplier += (50 + 50 * range(GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getTargetNumCities() - kLoopPlayer.getNumCities(), 0, iSites)) / (bTotalWar ? 2 : 1);
					}
				}
			}
			iPoolMultiplier /= std::max(1, getAliveCount());
			iCommitmentPool = iCommitmentPool * iPoolMultiplier / 100;

			// Don't pick a fight if we're expecting to beat them to a peaceful victory.
			/*
			if (!AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_DOMINATION4 | AI_VICTORY_CONQUEST4))
			{
				if (AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_CULTURE4) ||
					(AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_SPACE4) && !GET_TEAM(eTarget).AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_CULTURE4 | AI_VICTORY_SPACE4)) ||
					(AI_getLowestVictoryCountdown() > 0 && (GET_TEAM(eTarget).AI_getLowestVictoryCountdown() < 0 || AI_getLowestVictoryCountdown() < GET_TEAM(eTarget).AI_getLowestVictoryCountdown())))
				{
					iCommitmentPool *= 2;
				}
			}
			*/
			iTotalCost += iCommitmentPerMil * iCommitmentPool / 1000;
		}
	}

	/*
	// war weariness
	int iTotalWw = 0;
	for (TeamTypes i = (TeamTypes)0; i < MAX_TEAMS; i = (TeamTypes)(i + 1))
	{
		const CvTeamAI& kLoopTeam = GET_TEAM(i);
		if (kLoopTeam.isAlive() && (i == eTargetMaster || kLoopTeam.isVassal(eTargetMaster) || (bPendingDoW && kLoopTeam.isDefensivePact(eTargetMaster))))
			iTotalWw += getWarWeariness(i, true) / 100;
	}
	
	// note: getWarWeariness has units of anger per 100,000 population, and it is customary to divide it by 100 immediately
	if (iTotalWw > 50)
	{
		int iS = isAtWar(eTarget) ? -1 : 1;
		int iWwCost = 0;
		for (PlayerTypes eLoopPlayer = (PlayerTypes)0; eLoopPlayer < MAX_PLAYERS; eLoopPlayer = (PlayerTypes)(eLoopPlayer + 1))
		{
			const CvPlayerAI& kLoopPlayer = GET_PLAYER(eLoopPlayer);
			if (kLoopPlayer.getTeam() == getID() && kLoopPlayer.isAlive())
			{
				int iEstimatedPercentAnger = kLoopPlayer.getModifiedWarWearinessPercentAnger(iTotalWw) / 10; // (ugly, I know. But that's just how it's done.)
																											 // note. Unfortunately, we haven't taken the effect of jails into account.
				iWwCost += iS * kLoopPlayer.getNumCities() * kLoopPlayer.AI_getHappinessWeight(iS * iEstimatedPercentAnger * (100 + kLoopPlayer.getWarWearinessModifier()) / 100, 0, true) / 20;
			}
		}
		iTotalCost += iWwCost;
	}
	*/
	// Note: diplomacy cost is handled elsewhere
#endif
	return iTotalCost;
}

// diplomatic costs for declaring war (somewhat arbitrary - to encourage the AI to attack its enemies, and the enemies of its friends.)
int CvTeamAI::AI_warDiplomacyCost(TeamTypes eTarget) const
{
	if (isAtWar(eTarget))
	{
		//FAssertMsg(false, "AI_warDiplomacyCost called when already at war."); // sometimes we call this function for debug purposes.
		return 0;
	}

	/*
	if (AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_CONQUEST4))
		return 0;
	*/
	
	const CvTeamAI& kTargetTeam = GET_TEAM(eTarget);

	// first, the cost of upsetting the team we are declaring war on.
	int iDiploPopulation = kTargetTeam.getTotalPopulation();
	int iDiploCost = 3 * iDiploPopulation * (100 + AI_getAttitudeWeight(eTarget)) / 200;

	// cost of upsetting their friends
	for (TeamTypes i = (TeamTypes)0; i < MAX_TEAMS; i = (TeamTypes)(i + 1))
	{
		const CvTeamAI& kLoopTeam = GET_TEAM(i);

		if (!kLoopTeam.isAlive() || i == getID() || i == eTarget)
			continue;

		if (isHasMet(i) && kTargetTeam.isHasMet(i) /*&& !kLoopTeam.isCapitulated()*/)
		{
			int iPop = kLoopTeam.getTotalPopulation();
			iDiploPopulation += iPop;
			if (kLoopTeam.AI_getAttitude(eTarget) >= ATTITUDE_PLEASED && AI_getAttitude(i) >= ATTITUDE_PLEASED)
			{
				iDiploCost += iPop * (100 + AI_getAttitudeWeight(i)) / 200;
			}
			else if (kLoopTeam.isAtWar(eTarget))
			{
				iDiploCost -= iPop * (100 + AI_getAttitudeWeight(i)) / 400;
			}
		}
	}

	// scale the diplo cost based the personality of the team.
	{
		int iPeaceWeight = 0;
		/*for (PlayerTypes eLoopPlayer = (PlayerTypes)0; eLoopPlayer < MAX_PLAYERS; eLoopPlayer = (PlayerTypes)(eLoopPlayer + 1))
		{
			const CvPlayerAI& kLoopPlayer = GET_PLAYER(eLoopPlayer);
			if (kLoopPlayer.getTeam() == getID() && kLoopPlayer.isAlive())
			{
				//Leaderheads are missing peace weight
				iPeaceWeight += kLoopPlayer.AI_getPeaceWeight(); // usually between 0-10.
			}
		}
		*/

		int iDiploWeight = 40;
		iDiploWeight += 10 * iPeaceWeight / getAliveCount();
		// This puts iDiploWeight somewhere around 50 - 250.
		if (GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI))
			iDiploWeight /= 2;
		/*
		if (AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_DIPLOMACY3))
			iDiploWeight += 50;
		if (AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_DIPLOMACY4))
			iDiploWeight += 50;
		if (AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_CONQUEST3)) // note: conquest4 ignores diplo completely.
			iDiploWeight /= 2;
		*/
		iDiploCost *= iDiploWeight;
		iDiploCost /= 100;
	}

	// Finally, reduce the value for large maps;
	// so that this diplomacy stuff doesn't become huge relative to other parts of the war evaluation.
	iDiploCost *= 3;
	iDiploCost /= std::max(5, GC.getWorldInfo((WorldSizeTypes)GC.getMapINLINE().getWorldSize()).getDefaultPlayers());

	return iDiploCost;
}
// K-Mod end.

// K-Mod. Complete remake of the function.
int CvTeamAI::AI_startWarVal(TeamTypes eTarget, WarPlanTypes eWarPlan) const
{
	//TeamTypes eTargetMaster = GET_TEAM(eTarget).getMasterTeam(); // we need this for checking defensive pacts.
	bool bPendingDoW = !isAtWar(eTarget) && eWarPlan != WARPLAN_ATTACKED && eWarPlan != WARPLAN_ATTACKED_RECENT;
	int iTotalValue = AI_warSpoilsValue(eTarget, eWarPlan) - AI_warCommitmentCost(eTarget, eWarPlan) - (bPendingDoW ? AI_warDiplomacyCost(eTarget) : 0);

	// Call AI_warSpoilsValue for each additional enemy team involved in this war.
	// NOTE: a single call to AI_warCommitmentCost should include the cost of fighting all of these enemies.
	for (TeamTypes i = (TeamTypes)0; i < MAX_TEAMS; i = (TeamTypes)(i + 1))
	{
		if (i == getID() || i == eTarget)
			continue;

		const CvTeam& kLoopTeam = GET_TEAM(i);

		if (!kLoopTeam.isAlive() /*|| kLoopTeam.isVassal(getID())*/ || isAtWar(i))
			continue;
		
		/*
		if (kLoopTeam.isVassal(eTarget) || GET_TEAM(eTarget).isVassal(i))
		{
			iTotalValue += AI_warSpoilsValue(i, WARPLAN_DOGPILE) - (bPendingDoW ? AI_warDiplomacyCost(i) : 0);
		}
		
		else if (bPendingDoW && kLoopTeam.isDefensivePact(eTargetMaster))
		{
			FAssert(!isAtWar(eTarget));
			iTotalValue += AI_warSpoilsValue(i, WARPLAN_ATTACKED); // note: no diplo cost for this b/c it isn't us declaring war.
		}
		*/
	}
	return iTotalValue;
}
// K-Mod end


// XXX this should consider area power...
int CvTeamAI::AI_endWarVal(TeamTypes eTeam) const
{
	int iValue;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(isAtWar(eTeam), "Current AI Team instance is expected to be at war with eTeam");



	iValue = 100;

	iValue += (getNumCities() * 2);
	iValue += (GET_TEAM(eTeam).getNumCities() * 2);

	iValue += getTotalPopulation();
	iValue += GET_TEAM(eTeam).getTotalPopulation();
	
	iValue += (GET_TEAM(eTeam).AI_getWarSuccess(getID()) * 10);
	
	iValue += GET_TEAM(eTeam).AI_getDamages(getID());

	int iOurPower = std::max(1, getPower());
	int iTheirPower = std::max(1, GET_TEAM(eTeam).getDefensivePower());

	int iModifier = 50;
	
	iModifier *= iTheirPower;
	iModifier /= std::max(1, iOurPower + iTheirPower + 10);
	iModifier += 50;
	
	iValue *= iModifier;
	iValue /= 100;
	
	if (GET_TEAM(eTeam).AI_isNative())
	{
		if (GET_TEAM(eTeam).AI_isChosenWar(getID()))
		{
			iValue *= 25 + GET_TEAM(eTeam).AI_targetValidity(getID());
			iValue /= 125;
		}
	}
	
	/*WarPlanTypes eWarPlan = AI_getWarPlan(eTeam);

	// XXX count units in enemy territory...

	if ((!(isHuman()) && (eWarPlan == WARPLAN_TOTAL)) ||
		  (!(GET_TEAM(eTeam).isHuman()) && (GET_TEAM(eTeam).AI_getWarPlan(getID()) == WARPLAN_TOTAL)))
	{
		iValue *= 2;
	}
	else if ((!(isHuman()) && (eWarPlan == WARPLAN_DOGPILE) && (GET_TEAM(eTeam).getAtWarCount(true) > 1)) ||
		       (!(GET_TEAM(eTeam).isHuman()) && (GET_TEAM(eTeam).AI_getWarPlan(getID()) == WARPLAN_DOGPILE) && (getAtWarCount(true) > 1)))
	{
		iValue *= 3;
		iValue /= 2;
	}*/

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

// eTeam is the team that we're evaluating
int CvTeamAI::AI_mapTradeVal(TeamTypes eTeam) const
{
	int iValue = 0;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");
	
	for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		const CvPlot* const pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

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
	// making peave before its kings has lost
	
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
	return m_aiWarPlanStateCounter[eIndex];
}


void CvTeamAI::AI_setWarPlanStateCounter(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiWarPlanStateCounter[eIndex] = iNewValue;
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
	return m_aiAtWarCounter[eIndex];
}


void CvTeamAI::AI_setAtWarCounter(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiAtWarCounter[eIndex] = iNewValue;
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
	return m_aiAtPeaceCounter[eIndex];
}


void CvTeamAI::AI_setAtPeaceCounter(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiAtPeaceCounter[eIndex] = iNewValue;
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
	return m_aiHasMetCounter[eIndex];
}


void CvTeamAI::AI_setHasMetCounter(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiHasMetCounter[eIndex] = iNewValue;
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
	return m_aiOpenBordersCounter[eIndex];
}


void CvTeamAI::AI_setOpenBordersCounter(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiOpenBordersCounter[eIndex] = iNewValue;
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
	return m_aiDefensivePactCounter[eIndex];
}


void CvTeamAI::AI_setDefensivePactCounter(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiDefensivePactCounter[eIndex] = iNewValue;
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
	return m_aiShareWarCounter[eIndex];
}


void CvTeamAI::AI_setShareWarCounter(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiShareWarCounter[eIndex] = iNewValue;
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
	return m_aiWarSuccess[eIndex];
}


void CvTeamAI::AI_setWarSuccess(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiWarSuccess[eIndex] = iNewValue;
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
	return m_aiEnemyPeacetimeTradeValue[eIndex];
}


void CvTeamAI::AI_setEnemyPeacetimeTradeValue(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiEnemyPeacetimeTradeValue[eIndex] = iNewValue;
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
	return m_aiEnemyPeacetimeGrantValue[eIndex];
}


void CvTeamAI::AI_setEnemyPeacetimeGrantValue(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiEnemyPeacetimeGrantValue[eIndex] = iNewValue;
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
	return m_aiDamages[eIndex];
}


void CvTeamAI::AI_setDamages(TeamTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiDamages[eIndex] = iNewValue;
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
	FAssert(eIndex != getID() || m_aeWarPlan[eIndex] == NO_WARPLAN);
	return m_aeWarPlan[eIndex];
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


bool CvTeamAI::AI_isSneakAttackPreparing(TeamTypes eIndex) const
{
	return ((AI_getWarPlan(eIndex) == WARPLAN_PREPARING_LIMITED) || (AI_getWarPlan(eIndex) == WARPLAN_PREPARING_TOTAL));
}


bool CvTeamAI::AI_isSneakAttackReady(TeamTypes eIndex) const
{
	//return (AI_isChosenWar(eIndex) && !(AI_isSneakAttackPreparing(eIndex)));
	return !isAtWar(eIndex) && AI_isChosenWar(eIndex) && !AI_isSneakAttackPreparing(eIndex); // K-Mod
}

// K-Mod
bool CvTeamAI::AI_isSneakAttackReady() const
{
	for (int i = 0; i < MAX_TEAMS; i++)
	{
		if (AI_isSneakAttackReady((TeamTypes)i))
			return true;
	}
	return false;
}
// K-Mod end

void CvTeamAI::AI_setWarPlan(TeamTypes eIndex, WarPlanTypes eNewValue, bool bWar)
{
	int iI;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (AI_getWarPlan(eIndex) != eNewValue)
	{
		if (bWar || !isAtWar(eIndex))
		{
			m_aeWarPlan[eIndex] = eNewValue;

			AI_setWarPlanStateCounter(eIndex, 0);

			AI_updateAreaStragies();

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
	CvTeam::read(pStream);

	uint uiFlag=0;
	pStream->Read(&uiFlag);	// flags for expansion

	uint iSize;
	pStream->Read(&iSize);
	if (iSize > 0)
	{
		m_aiEnemyCityDistance.resize(iSize);
		pStream->Read(iSize, &m_aiEnemyCityDistance[0]);
	}
	if (uiFlag > 0)
	{
		pStream->Read(&iSize);
		if (iSize > 0)
		{
			m_aiEnemyUnitDistance.resize(iSize);
			pStream->Read(iSize, &m_aiEnemyUnitDistance[0]);
		}
	}
	pStream->Read(MAX_TEAMS, m_aiWarPlanStateCounter);
	pStream->Read(MAX_TEAMS, m_aiAtWarCounter);
	pStream->Read(MAX_TEAMS, m_aiAtPeaceCounter);
	pStream->Read(MAX_TEAMS, m_aiHasMetCounter);
	pStream->Read(MAX_TEAMS, m_aiOpenBordersCounter);
	pStream->Read(MAX_TEAMS, m_aiDefensivePactCounter);
	pStream->Read(MAX_TEAMS, m_aiShareWarCounter);
	pStream->Read(MAX_TEAMS, m_aiWarSuccess);
	pStream->Read(MAX_TEAMS, m_aiEnemyPeacetimeTradeValue);
	pStream->Read(MAX_TEAMS, m_aiEnemyPeacetimeGrantValue);
	pStream->Read(MAX_TEAMS, m_aiDamages);

	pStream->Read(MAX_TEAMS, (int*)m_aeWarPlan);
	pStream->Read((int*)&m_eWorstEnemy);
}


void CvTeamAI::write(FDataStreamBase* pStream)
{
	CvTeam::write(pStream);

	uint uiFlag=1;
	pStream->Write(uiFlag);		// flag for expansion

	pStream->Write(m_aiEnemyCityDistance.size());
	if (!m_aiEnemyCityDistance.empty())
	{
		pStream->Write(m_aiEnemyCityDistance.size(), &m_aiEnemyCityDistance[0]);
	}
	pStream->Write(m_aiEnemyUnitDistance.size());
	if (!m_aiEnemyUnitDistance.empty())
	{
		pStream->Write(m_aiEnemyUnitDistance.size(), &m_aiEnemyUnitDistance[0]);
	}
	pStream->Write(MAX_TEAMS, m_aiWarPlanStateCounter);
	pStream->Write(MAX_TEAMS, m_aiAtWarCounter);
	pStream->Write(MAX_TEAMS, m_aiAtPeaceCounter);
	pStream->Write(MAX_TEAMS, m_aiHasMetCounter);
	pStream->Write(MAX_TEAMS, m_aiOpenBordersCounter);
	pStream->Write(MAX_TEAMS, m_aiDefensivePactCounter);
	pStream->Write(MAX_TEAMS, m_aiShareWarCounter);
	pStream->Write(MAX_TEAMS, m_aiWarSuccess);
	pStream->Write(MAX_TEAMS, m_aiEnemyPeacetimeTradeValue);
	pStream->Write(MAX_TEAMS, m_aiEnemyPeacetimeGrantValue);
	pStream->Write(MAX_TEAMS, m_aiDamages);

	pStream->Write(MAX_TEAMS, (int*)m_aeWarPlan);
	pStream->Write(m_eWorstEnemy);
}

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

	CvGame& kGame = GC.getGameINLINE(); // K-Mod

										/* FAssert(!isHuman());
										FAssert(!isBarbarian());
										FAssert(!isMinorCiv());

										if (isAVassal())
										{
										return;
										} */ // disabled by K-Mod. All civs still need to do some basic updates.

										// allow python to handle it
	if (GC.getUSE_AI_DO_WAR_CALLBACK()) // K-Mod. block unused python callbacks
	{
		CyArgsList argsList;
		argsList.add(getID());
		long lResult = 0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "AI_doWar", argsList.makeFunctionArgs(), &lResult);
		if (lResult == 1)
		{
			return;
		}
	}

	int iEnemyPowerPercent = AI_getEnemyPowerPercent();

	// K-Mod note: This first section also used for vassals, and for human players.
	for (TeamTypes eLoopTeam = (TeamTypes)0; eLoopTeam < MAX_TEAMS; eLoopTeam = (TeamTypes)(eLoopTeam + 1))
	{
		if (!GET_TEAM(eLoopTeam).isAlive())
			continue;
		if (!isHasMet(eLoopTeam))
			continue;
		if (AI_getWarPlan(eLoopTeam) == NO_WARPLAN)
			continue;

		int iTimeModifier = 100; // preperation time modifier
		int iAbandonTimeModifier = 100; // deadline for attack modifier
		iAbandonTimeModifier *= 50 + GC.getGameSpeedInfo(kGame.getGameSpeedType()).getTrainPercent();
		iAbandonTimeModifier /= 150;
		// (more adjustments to the time modifiers will come later)

		if (AI_getWarPlan(eLoopTeam) == WARPLAN_ATTACKED_RECENT)
		{
			FAssert(isAtWar(eLoopTeam));

			if (AI_getAtWarCounter(eLoopTeam) > ((GET_TEAM(eLoopTeam).AI_isLandTarget(getID())) ? 9 : 3))
			{
				/*
				if (gTeamLogLevel >= 1)
				{
					logBBAI("      Team %d (%S) switching WARPLANS against team %d (%S) from ATTACKED_RECENT to ATTACKED with enemy power percent %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), eLoopTeam, GET_PLAYER(GET_TEAM(eLoopTeam).getLeaderID()).getCivilizationDescription(0), iEnemyPowerPercent);
				}
				*/
				AI_setWarPlan(eLoopTeam, WARPLAN_ATTACKED);
			}
		}

		// K-Mod
		if (isHuman() /*|| isAVassal()*/)
		{
			CvTeamAI& kOurMaster = GET_TEAM(getMasterTeam());
			if (!isAtWar(eLoopTeam))
			{
				FAssert(AI_getWarPlan(eLoopTeam) == WARPLAN_PREPARING_TOTAL || AI_getWarPlan(eLoopTeam) == WARPLAN_LIMITED);
				if (isHuman() || kOurMaster.isHuman())
				{
					if (AI_getWarPlanStateCounter(eLoopTeam) > 20 * iAbandonTimeModifier / 100)
					{
						/*
						if (gTeamLogLevel >= 1)
						{
							logBBAI("      Team %d (%S) abandoning WARPLANS against team %d (%S) due to human / vassal timeout", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), eLoopTeam, GET_PLAYER(GET_TEAM(eLoopTeam).getLeaderID()).getCivilizationDescription(0));
						}
						AI_setWarPlan(eLoopTeam, NO_WARPLAN);
						*/
					}
				}
				else
				{
					if (kOurMaster.AI_getWarPlan(eLoopTeam) == NO_WARPLAN)
					{
						/*
						if (gTeamLogLevel >= 1)
						{
							logBBAI("      Team %d (%S) abandoning WARPLANS against team %d (%S) due to AI master's warplan cancelation", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), eLoopTeam, GET_PLAYER(GET_TEAM(eLoopTeam).getLeaderID()).getCivilizationDescription(0));
						}
						*/
						AI_setWarPlan(eLoopTeam, NO_WARPLAN);
					}
				}
			}

			continue; // Nothing else required for vassals and human players
		}
		// K-Mod

		if (!isAtWar(eLoopTeam)) // K-Mod. time / abandon modifiers are only relevant for war preparations. We don't need them if we are already at war.
		{
			int iThreshold = (80 * AI_maxWarNearbyPowerRatio()) / 100;

			if (iEnemyPowerPercent < iThreshold)
			{
				iTimeModifier *= iEnemyPowerPercent;
				iTimeModifier /= iThreshold;
			}
			// K-Mod
			// intercontinental wars need more prep time
			if (!AI_hasCitiesInPrimaryArea(eLoopTeam))
			{
				iTimeModifier *= 5;
				iTimeModifier /= 4;
				iAbandonTimeModifier *= 5;
				iAbandonTimeModifier /= 4;
				// maybe in the future I'll count the number of local cities and the number of overseas cities
				// and use it to make a more appropriate modifier... but not now.
			}
			else
			{
				//with crush strategy, use just 2/3 of the prep time.
				//int iCrushMembers = AI_countMembersWithStrategy(AI_STRATEGY_CRUSH);
				iTimeModifier *= 3 * (getNumMembers() /* - iCrushMembers) + 2 * iCrushMembers*/);
				iTimeModifier /= 3;
			}
			// K-Mod end

			iTimeModifier *= 50 + GC.getGameSpeedInfo(kGame.getGameSpeedType()).getTrainPercent();
			iTimeModifier /= 150;

			FAssert(iTimeModifier >= 0);
		}

		//bool bEnemyVictoryLevel4 = GET_TEAM(eLoopTeam).AI_isAnyMemberDoVictoryStrategyLevel4();

		if (AI_getWarPlan(eLoopTeam) == WARPLAN_PREPARING_LIMITED)
		{
			//FAssert(canEventuallyDeclareWar(eLoopTeam));

			if (AI_getWarPlanStateCounter(eLoopTeam) > ((5 * iTimeModifier) / (bEnemyVictoryLevel4 ? 400 : 100)))
			{
				if (AI_startWarVal(eLoopTeam, WARPLAN_LIMITED) > 0) // K-Mod. Last chance to change our mind if circumstances have changed
				{
					/*
					if (gTeamLogLevel >= 1)
					{
						logBBAI("      Team %d (%S) switching WARPLANS against team %d (%S) from PREPARING_LIMITED to LIMITED after %d turns with enemy power percent %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), eLoopTeam, GET_PLAYER(GET_TEAM(eLoopTeam).getLeaderID()).getCivilizationDescription(0), AI_getWarPlanStateCounter(eLoopTeam), iEnemyPowerPercent);
					}
					*/
					AI_setWarPlan(eLoopTeam, WARPLAN_LIMITED);
				}
				// K-Mod
				else
				{
					/*
					if (gTeamLogLevel >= 1)
					{
						logBBAI("      Team %d (%S) abandoning WARPLAN_LIMITED against team %d (%S) after %d turns with enemy power percent %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), eLoopTeam, GET_PLAYER(GET_TEAM(eLoopTeam).getLeaderID()).getCivilizationDescription(0), AI_getWarPlanStateCounter(eLoopTeam), iEnemyPowerPercent);
					}
					*/
				}
				// K-Mod end
			}
		}
		else if (AI_getWarPlan(eLoopTeam) == WARPLAN_LIMITED || AI_getWarPlan(eLoopTeam) == WARPLAN_DOGPILE)
		{
			if (!isAtWar(eLoopTeam))
			{
				//FAssert(canEventuallyDeclareWar(eLoopTeam));

				bool bActive = false;
				for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
				{
					if (GET_PLAYER((PlayerTypes)iPlayer).getTeam() == getID())
					{
						if (GET_PLAYER((PlayerTypes)iPlayer).AI_enemyTargetMissions(eLoopTeam) > 0)
						{
							bActive = true;
							break;
						}
					}
				}

				if (!bActive)
				{
					if (AI_getWarPlanStateCounter(eLoopTeam) > ((15 * iAbandonTimeModifier) / (100)))
					{
						/*
						if (gTeamLogLevel >= 1)
						{
							logBBAI("      Team %d (%S) abandoning WARPLAN_LIMITED or WARPLAN_DOGPILE against team %d (%S) after %d turns with enemy power percent %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), eLoopTeam, GET_PLAYER(GET_TEAM(eLoopTeam).getLeaderID()).getCivilizationDescription(0), AI_getWarPlanStateCounter(eLoopTeam), iEnemyPowerPercent);
						}
						*/
						AI_setWarPlan(eLoopTeam, NO_WARPLAN);
					}
				}

				if (AI_getWarPlan(eLoopTeam) == WARPLAN_DOGPILE)
				{
					if (GET_TEAM(eLoopTeam).getAtWarCount() == 0)
					{
						/*
						if (gTeamLogLevel >= 1)
						{
							logBBAI("      Team %d (%S) abandoning WARPLAN_DOGPILE against team %d (%S) after %d turns because enemy has no war with enemy power percent %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), eLoopTeam, GET_PLAYER(GET_TEAM(eLoopTeam).getLeaderID()).getCivilizationDescription(0), AI_getWarPlanStateCounter(eLoopTeam), iEnemyPowerPercent);
						}
						*/
						AI_setWarPlan(eLoopTeam, NO_WARPLAN);
					}
				}
			}
		}
		else if (AI_getWarPlan(eLoopTeam) == WARPLAN_PREPARING_TOTAL)
		{
			//FAssert(canEventuallyDeclareWar(eLoopTeam));

			if (AI_getWarPlanStateCounter(eLoopTeam) > ((10 * iTimeModifier) / 100)); // (bEnemyVictoryLevel4 ? 400 : 100)))
			{
				bool bAreaValid = false;
				bool bShareValid = false;

				int iLoop;
				for (CvArea* pLoopArea = GC.getMapINLINE().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMapINLINE().nextArea(&iLoop))
				{
					if (AI_isPrimaryArea(pLoopArea))
					{
						if (GET_TEAM(eLoopTeam).countNumCitiesByArea(pLoopArea) > 0)
						{
							bShareValid = true;

							AreaAITypes eAreaAI = AI_calculateAreaAIType(pLoopArea, true);

							/* BBAI code
							if ( eAreaAI == AREAAI_DEFENSIVE)
							{
							bAreaValid = false;
							}
							else if( eAreaAI == AREAAI_OFFENSIVE )
							{
							bAreaValid = true;
							} */
							// K-Mod. Doing it that way means the order the areas are checked is somehow important...
							if (eAreaAI == AREAAI_OFFENSIVE)
							{
								bAreaValid = true; // require at least one offense area
							}
							else if (eAreaAI == AREAAI_DEFENSIVE)
							{
								bAreaValid = false;
								break; // false if there are _any_ defence areas
							}
							// K-Mod end
						}
					}
				}

				if (((bAreaValid && iEnemyPowerPercent < 140) || (!bShareValid && iEnemyPowerPercent < 110) || GET_TEAM(eLoopTeam).AI_getLowestVictoryCountdown() >= 0) &&
					AI_startWarVal(eLoopTeam, WARPLAN_TOTAL) > 0) // K-Mod. Last chance to change our mind if circumstances have changed
				{
					/*
					if (gTeamLogLevel >= 1)
					{
						logBBAI("      Team %d (%S) switching WARPLANS against team %d (%S) from PREPARING_TOTAL to TOTAL after %d turns with enemy power percent %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), eLoopTeam, GET_PLAYER(GET_TEAM(eLoopTeam).getLeaderID()).getCivilizationDescription(0), AI_getWarPlanStateCounter(eLoopTeam), iEnemyPowerPercent);
					}
					*/
					AI_setWarPlan(eLoopTeam, WARPLAN_TOTAL);
				}
				else if (AI_getWarPlanStateCounter(eLoopTeam) >((20 * iAbandonTimeModifier) / 100))
				{
					/*
					if (gTeamLogLevel >= 1)
					{
						logBBAI("      Team %d (%S) abandoning WARPLAN_TOTAL_PREPARING against team %d (%S) after %d turns with enemy power percent %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), eLoopTeam, GET_PLAYER(GET_TEAM(eLoopTeam).getLeaderID()).getCivilizationDescription(0), AI_getWarPlanStateCounter(eLoopTeam), iEnemyPowerPercent);
					}
					*/
					AI_setWarPlan(eLoopTeam, NO_WARPLAN);
				}
			}
		}
		else if (AI_getWarPlan(eLoopTeam) == WARPLAN_TOTAL)
		{
			if (!isAtWar(eLoopTeam))
			{
				//FAssert(canEventuallyDeclareWar(eLoopTeam));

				bool bActive = false;
				for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
				{
					if (GET_PLAYER((PlayerTypes)iPlayer).getTeam() == getID())
					{
						if (GET_PLAYER((PlayerTypes)iPlayer).AI_enemyTargetMissions(eLoopTeam) > 0)
						{
							bActive = true;
							break;
						}
					}
				}

				if (!bActive)
				{
					if (AI_getWarPlanStateCounter(eLoopTeam) > ((25 * iAbandonTimeModifier) / (100)))
					{
						/*
						if (gTeamLogLevel >= 1)
						{
							logBBAI("      Team %d (%S) abandoning WARPLAN_TOTAL against team %d (%S) after %d turns with enemy power percent %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), eLoopTeam, GET_PLAYER(GET_TEAM(eLoopTeam).getLeaderID()).getCivilizationDescription(0), AI_getWarPlanStateCounter(eLoopTeam), iEnemyPowerPercent);
						}
						*/
						AI_setWarPlan(eLoopTeam, NO_WARPLAN);
					}
				}
			}
		}
	}

	// K-Mod. This is the end of the basics updates.
	// The rest of the stuff is related to making peace deals, and planning future wars.
	//if (isHuman() || isBarbarian() || isMinorCiv() || isAVassal())
	if (isHuman() || animals || archbishop  || not our king)
		return;
	// K-Mod end

	for (PlayerTypes i = (PlayerTypes)0; i < MAX_PLAYERS; i = (PlayerTypes)(i + 1))
	{
		CvPlayerAI& kLoopPlayer = GET_PLAYER(i);

		if (kLoopPlayer.isAlive())
		{
			if (kLoopPlayer.getTeam() == getID())
			{
				kLoopPlayer.AI_doPeace();
			}
		}
	}

	int iNumMembers = getNumMembers();
	/* original bts code
	int iHighUnitSpendingPercent = 0;
	int iLowUnitSpendingPercent = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
	if (GET_PLAYER((PlayerTypes)iI).isAlive())
	{
	if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
	{
	int iUnitSpendingPercent = (GET_PLAYER((PlayerTypes)iI).calculateUnitCost() * 100) / std::max(1, GET_PLAYER((PlayerTypes)iI).calculatePreInflatedCosts());
	iHighUnitSpendingPercent += (std::max(0, iUnitSpendingPercent - 7) / 2);
	iLowUnitSpendingPercent += iUnitSpendingPercent;
	}
	}
	}

	iHighUnitSpendingPercent /= iNumMembers;
	iLowUnitSpendingPercent /= iNumMembers; */ // K-Mod, this simply wasn't being used anywhere.

	// K-Mod. Gather some data...
	bool bAtWar = false;
	bool bTotalWarPlan = false;
	bool bAnyWarPlan = false;
	bool bLocalWarPlan = false;
	for (TeamTypes eLoopTeam = (TeamTypes)0; eLoopTeam < MAX_TEAMS; eLoopTeam = (TeamTypes)(eLoopTeam + 1))
	{
		if (GET_TEAM(eLoopTeam).isAlive() && !GET_TEAM(eLoopTeam).isMinorCiv())
		{
			bAtWar = bAtWar || isAtWar(eLoopTeam);

			switch (AI_getWarPlan(eLoopTeam))
			{
			case NO_WARPLAN:
				break;
			case WARPLAN_PREPARING_TOTAL:
			case WARPLAN_TOTAL:
				bTotalWarPlan = true;
			default: // all other warplans
				bLocalWarPlan = bLocalWarPlan || AI_isLandTarget(eLoopTeam);
				bAnyWarPlan = true;
				break;
			}
		}
	}
	// K-Mod end

	// if at war, check for making peace
	// Note: this section relates to automatic peace deals for inactive wars.
	if (bAtWar && kGame.getSorenRandNum(AI_makePeaceRand(), "AI Make Peace") == 0)
	{
		for (TeamTypes eLoopTeam = (TeamTypes)0; eLoopTeam < MAX_TEAMS; eLoopTeam = (TeamTypes)(eLoopTeam + 1))
		{
			if (!GET_TEAM(eLoopTeam).isAlive())
				continue;
			if (eLoopTeam == getID())
				continue;
			if (GET_TEAM(eLoopTeam).isHuman())
				continue;

			if (!isAtWar(eLoopTeam))
				continue;
			if (!AI_isChosenWar(eLoopTeam))
				continue;
			if (!canContact(eLoopTeam, true))
				continue;

			FAssert(!(GET_TEAM(eLoopTeam).isMinorCiv()));

			if (AI_getAtWarCounter(eLoopTeam) > std::max(10, (14 * GC.getGameSpeedInfo(kGame.getGameSpeedType()).getVictoryDelayPercent()) / 100))
			{
				// If nothing is happening in war
				if (AI_getWarSuccess(eLoopTeam) + GET_TEAM(eLoopTeam).AI_getWarSuccess(getID()) < 2 * GC.getDefineINT("WAR_SUCCESS_ATTACKING"))
				{
					if ((kGame.getSorenRandNum(8, "AI Make Peace 1") == 0))
					{
						bool bValid = true;

						for (PlayerTypes i = (PlayerTypes)0; i < MAX_PLAYERS; i = (PlayerTypes)(i + 1))
						{
							CvPlayerAI& kLoopPlayer = GET_PLAYER(i);
							if (kLoopPlayer.getTeam() == getID())
							{
								if (kLoopPlayer.AI_enemyTargetMissions(eLoopTeam) > 0)
								{
									bValid = false;
									break;
								}
							}

							if (kLoopPlayer.getTeam() == eLoopTeam)
							{
								//MissionAITypes eMissionAI = MISSIONAI_ASSAULT;
								if (kLoopPlayer.AI_enemyTargetMissions(getID()) > 0)
								{
									bValid = false;
									break;
								}
							}
						}

						if (bValid)
						{
							makePeace(eLoopTeam);
							/*
							if (gTeamLogLevel >= 1)
							{
								logBBAI("  Team %d (%S) making peace due to time and no fighting", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0));
							}
							*/
							continue;
						}
					}
				}

				// Fought to a long draw
				if (AI_getAtWarCounter(eLoopTeam) > ((((AI_getWarPlan(eLoopTeam) == WARPLAN_TOTAL) ? 40 : 30) * GC.getGameSpeedInfo(kGame.getGameSpeedType()).getVictoryDelayPercent()) / 100))
				{
					int iOurValue = AI_endWarVal(eLoopTeam);
					int iTheirValue = GET_TEAM(eLoopTeam).AI_endWarVal(getID());
					if ((iOurValue > (iTheirValue / 2)) && (iTheirValue > (iOurValue / 2)))
					{
						/*
						if (gTeamLogLevel >= 1)
						{
							logBBAI("  Team %d (%S) making peace due to time and endWarVal %d vs their %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), iOurValue, iTheirValue);
						}
						*/
						makePeace(eLoopTeam);
						continue;
					}
				}

				// All alone in what we thought was a dogpile
				if (AI_getWarPlan(eLoopTeam) == WARPLAN_DOGPILE)
				{
					if (GET_TEAM(eLoopTeam).getAtWarCount() == 1)
					{
						int iOurValue = AI_endWarVal(eLoopTeam);
						int iTheirValue = GET_TEAM(eLoopTeam).AI_endWarVal(getID());
						if ((iTheirValue > (iOurValue / 2)))
						{
							/*
							if (gTeamLogLevel >= 1)
							{
								logBBAI("  Team %d (%S) making peace due to being only dog-piler left", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0));
							}
							*/
							makePeace(eLoopTeam);
							continue;
						}
					}
				}
			}
		}
	}

	// if no war plans, consider starting one!

	//if (getAnyWarPlanCount(true) == 0 || iEnemyPowerPercent < 45)
	// K-Mod. Some more nuance to the conditions for considering war
	// First condition: only consider a new war if there are no current wars that need more attention. (local total war, or a war we aren't winning)
	bool bConsiderWar = !bAnyWarPlan || (iEnemyPowerPercent < 45 && !(bLocalWarPlan && bTotalWarPlan) && AI_getWarSuccessRating() >(bTotalWarPlan ? 40 : 15));
	// Second condition: don't consider war very early in the game. It would be unfair on human players to rush them with our extra starting units and techs!
	bConsiderWar = bConsiderWar &&
		(kGame.isOption(GAMEOPTION_AGGRESSIVE_AI) ||
			kGame.getElapsedGameTurns() >= GC.getGameSpeedInfo(kGame.getGameSpeedType()).getBarbPercent() * 30 / 100 ||
			kGame.getNumCivCities() > GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getTargetNumCities() * kGame.countCivPlayersAlive() / 2);
	// (Perhaps the no-war turn threshold should depend on the game difficulty level; but I don't think it would make much difference.)
	if (bConsiderWar)
		// K-mod end
	{
		bool bAggressive = kGame.isOption(GAMEOPTION_AGGRESSIVE_AI);

		int iFinancialTroubleCount = 0;
		int iDaggerCount = 0;
		int iGetBetterUnitsCount = 0;
		for (PlayerTypes i = (PlayerTypes)0; i < MAX_PLAYERS; i = (PlayerTypes)(i + 1))
		{
			CvPlayerAI& kLoopPlayer = GET_PLAYER(i);

			if (kLoopPlayer.isAlive())
			{
				if (kLoopPlayer.getTeam() == getID())
				{
					if (kLoopPlayer.AI_isDoStrategy(AI_STRATEGY_DAGGER)
						|| kLoopPlayer.AI_isDoVictoryStrategy(AI_VICTORY_CONQUEST3)
						|| kLoopPlayer.AI_isDoVictoryStrategy(AI_VICTORY_DOMINATION4))
					{
						iDaggerCount++;
						bAggressive = true;
					}

					if (kLoopPlayer.AI_isDoStrategy(AI_STRATEGY_GET_BETTER_UNITS))
					{
						iGetBetterUnitsCount++;
					}

					if (kLoopPlayer.AI_isFinancialTrouble())
					{
						iFinancialTroubleCount++;
					}
				}
			}
		}

		// if random in this range is 0, we go to war of this type (so lower numbers are higher probablity)
		// average of everyone on our team
		int iTotalWarRand;
		int iLimitedWarRand;
		int iDogpileWarRand;
		AI_getWarRands(iTotalWarRand, iLimitedWarRand, iDogpileWarRand);

		int iTotalWarThreshold;
		int iLimitedWarThreshold;
		int iDogpileWarThreshold;
		AI_getWarThresholds(iTotalWarThreshold, iLimitedWarThreshold, iDogpileWarThreshold);

		// we oppose war if half the non-dagger teammates in financial trouble
		bool bFinancesOpposeWar = false;
		if ((iFinancialTroubleCount - iDaggerCount) >= std::max(1, getNumMembers() / 2))
		{
			// this can be overridden by by the pro-war booleans
			bFinancesOpposeWar = true;
		}

		// if agressive, we may start a war to get money
		bool bFinancesProTotalWar = false;
		bool bFinancesProLimitedWar = false;
		bool bFinancesProDogpileWar = false;
		if (iFinancialTroubleCount > 0)
		{
			// do we like all out wars?
			if (iDaggerCount > 0 || iTotalWarRand < 100)
			{
				bFinancesProTotalWar = true;
			}

			// do we like limited wars?
			if (iLimitedWarRand < 100)
			{
				bFinancesProLimitedWar = true;
			}

			// do we like dogpile wars?
			if (iDogpileWarRand < 100)
			{
				bFinancesProDogpileWar = true;
			}
		}
		bool bFinancialProWar = (bFinancesProTotalWar || bFinancesProLimitedWar || bFinancesProDogpileWar);

		// overall war check (quite frequently true)
		bool bMakeWarChecks = false;
		if ((iGetBetterUnitsCount - iDaggerCount) * 3 < iNumMembers * 2)
		{
			if (bFinancialProWar || !bFinancesOpposeWar)
			{
				// random overall war chance (at noble+ difficulties this is 100%)
				if (kGame.getSorenRandNum(100, "AI Declare War 1") < GC.getHandicapInfo(kGame.getHandicapType()).getAIDeclareWarProb())
				{
					bMakeWarChecks = true;
				}
			}
		}

		if (bMakeWarChecks)
		{
			int iOurPower = getPower();

			if (bAggressive && (getAnyWarPlanCount() == 0))
			{
				iOurPower *= 4;
				iOurPower /= 3;
			}

			iOurPower *= (100 - iEnemyPowerPercent);
			iOurPower /= 100;

			if ((bFinancesProTotalWar || !bFinancesOpposeWar) &&
				(kGame.getSorenRandNum(iTotalWarRand, "AI Maximum War") <= iTotalWarThreshold))
			{
				int iNoWarRoll = kGame.getSorenRandNum(100, "AI No War");
				iNoWarRoll = range(iNoWarRoll + (bAggressive ? 10 : 0) + (bFinancesProTotalWar ? 10 : 0) - (20 * iGetBetterUnitsCount) / iNumMembers, 0, 99);

				int iBestValue = 10; // K-Mod. I've set the starting value above zero just as a buffer against close-calls which end up being negative value in the near future.
				TeamTypes eBestTeam = NO_TEAM;

				for (int iPass = 0; iPass < 3; iPass++)
				{
					for (int iI = 0; iI < MAX_TEAMS; iI++)
					{
						if (canEventuallyDeclareWar((TeamTypes)iI) && AI_haveSeenCities((TeamTypes)iI))
						{
							TeamTypes eLoopMasterTeam = GET_TEAM((TeamTypes)iI).getMasterTeam(); // K-Mod (plus all changes which refer to this variable).
							bool bVassal = eLoopMasterTeam != iI;

							if (bVassal && !AI_isOkayVassalTarget((TeamTypes)iI))
								continue;

							if (iNoWarRoll >= AI_noWarAttitudeProb(AI_getAttitude((TeamTypes)iI)) && (!bVassal || iNoWarRoll >= AI_noWarAttitudeProb(AI_getAttitude(eLoopMasterTeam))))
							{
								int iDefensivePower = (GET_TEAM((TeamTypes)iI).getDefensivePower(getID()) * 2) / 3;

								if (iDefensivePower < ((iOurPower * ((iPass > 1) ? AI_maxWarDistantPowerRatio() : AI_maxWarNearbyPowerRatio())) / 100))
								{
									// XXX make sure they share an area....

									//FAssertMsg(!(GET_TEAM((TeamTypes)iI).isBarbarian()), "Expected to not be declaring war on the barb civ");
									FAssertMsg(iI != getID(), "Expected not to be declaring war on self (DOH!)");

									if ((iPass > 1 && !bLocalWarPlan) || AI_isLandTarget((TeamTypes)iI) || AI_isAnyCapitalAreaAlone() || GET_TEAM((TeamTypes)iI).AI_isAnyMemberDoVictoryStrategyLevel4())
									{
										if ((iPass > 0) || (AI_calculateAdjacentLandPlots((TeamTypes)iI) >= ((getTotalLand() * AI_maxWarMinAdjacentLandPercent()) / 100)) || GET_TEAM((TeamTypes)iI).AI_isAnyMemberDoVictoryStrategyLevel4())
										{
											int iValue = AI_startWarVal((TeamTypes)iI, WARPLAN_TOTAL);

											/*
											if (iValue > 0 && gTeamLogLevel >= 2)
											{
												logBBAI("      Team %d (%S) considering starting TOTAL warplan with team %d with value %d on pass %d with %d adjacent plots", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), iI, iValue, iPass, AI_calculateAdjacentLandPlots((TeamTypes)iI));
											}
											*/
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

					if (eBestTeam != NO_TEAM)
					{
						/*
						if (gTeamLogLevel >= 1)
						{
							logBBAI("    Team %d (%S) starting TOTAL warplan preparations against team %d on pass %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), eBestTeam, iPass);
						}
						*/
						AI_setWarPlan(eBestTeam, (iDaggerCount > 0) ? WARPLAN_TOTAL : WARPLAN_PREPARING_TOTAL);
						break;
					}
				}
			}
			/************************************************************************************************/
			/* UNOFFICIAL_PATCH                       01/02/09                                jdog5000      */
			/*                                                                                              */
			/* Bugfix                                                                                       */
			/************************************************************************************************/
			else if ((bFinancesProLimitedWar || !bFinancesOpposeWar) &&
				(kGame.getSorenRandNum(iLimitedWarRand, "AI Limited War") <= iLimitedWarThreshold))
				/************************************************************************************************/
				/* UNOFFICIAL_PATCH                        END                                                  */
				/************************************************************************************************/
			{
				int iNoWarRoll = kGame.getSorenRandNum(100, "AI No War") - 10;
				iNoWarRoll = range(iNoWarRoll + (bAggressive ? 10 : 0) + (bFinancesProLimitedWar ? 10 : 0), 0, 99);

				int iBestValue = 0;
				TeamTypes eBestTeam = NO_TEAM;

				for (int iI = 0; iI < MAX_TEAMS; iI++)
				{
					if (canEventuallyDeclareWar((TeamTypes)iI) && AI_haveSeenCities((TeamTypes)iI))
					{
						//TeamTypes eLoopMasterTeam = GET_TEAM((TeamTypes)iI).getMasterTeam(); // K-Mod (plus all changes which refer to this variable).
						//bool bVassal = eLoopMasterTeam != iI;

						/*
						if (bVassal && !AI_isOkayVassalTarget((TeamTypes)iI))
							continue;
						*/
						
						if (iNoWarRoll >= AI_noWarAttitudeProb(AI_getAttitude((TeamTypes)iI)) && (!bVassal || iNoWarRoll >= AI_noWarAttitudeProb(AI_getAttitude(eLoopMasterTeam))))
						{
							if (AI_isLandTarget((TeamTypes)iI) || (AI_isAnyCapitalAreaAlone() && GET_TEAM((TeamTypes)iI).AI_isAnyCapitalAreaAlone()))
							{
								if (GET_TEAM((TeamTypes)iI).getDefensivePower(getID()) < ((iOurPower * AI_limitedWarPowerRatio()) / 100))
								{
									int iValue = AI_startWarVal((TeamTypes)iI, WARPLAN_LIMITED);

									/*
									if (iValue > 0 && gTeamLogLevel >= 2)
									{
										logBBAI("      Team %d (%S) considering starting LIMITED warplan with team %d with value %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), iI, iValue);
									}
									*/
									if (iValue > iBestValue)
									{
										//FAssert(!AI_shareWar((TeamTypes)iI)); // disabled by K-Mod. (It isn't always true.)
										iBestValue = iValue;
										eBestTeam = ((TeamTypes)iI);
									}
								}
							}
						}
					}
				}

				if (eBestTeam != NO_TEAM)
				{
					/*
					if (gTeamLogLevel >= 1)
					{
						logBBAI("    Team %d (%S) starting LIMITED warplan preparations against team %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), eBestTeam);
					}
					*/
					AI_setWarPlan(eBestTeam, (iDaggerCount > 0) ? WARPLAN_LIMITED : WARPLAN_PREPARING_LIMITED);
				}
			}
			else if ((bFinancesProDogpileWar || !bFinancesOpposeWar) &&
				(kGame.getSorenRandNum(iDogpileWarRand, "AI Dogpile War") <= iDogpileWarThreshold))
			{
				int iNoWarRoll = kGame.getSorenRandNum(100, "AI No War") - 20;
				iNoWarRoll = range(iNoWarRoll + (bAggressive ? 10 : 0) + (bFinancesProDogpileWar ? 10 : 0), 0, 99);

				int iBestValue = 0;
				TeamTypes eBestTeam = NO_TEAM;

				for (int iI = 0; iI < MAX_TEAMS; iI++)
				{
					if (canDeclareWar((TeamTypes)iI) && AI_haveSeenCities((TeamTypes)iI))
					{
						//TeamTypes eLoopMasterTeam = GET_TEAM((TeamTypes)iI).getMasterTeam(); // K-Mod (plus all changes which refer to this variable).
						//bool bVassal = eLoopMasterTeam != iI;
						/*
						if (bVassal && !AI_isOkayVassalTarget((TeamTypes)iI))
							continue;
						*/
						if (iNoWarRoll >= AI_noWarAttitudeProb(AI_getAttitude((TeamTypes)iI)) && (!bVassal || iNoWarRoll >= AI_noWarAttitudeProb(AI_getAttitude(eLoopMasterTeam))))
						{
							if (GET_TEAM((TeamTypes)iI).getAtWarCount(true) > 0)
							{
								if (AI_isLandTarget((TeamTypes)iI) || GET_TEAM((TeamTypes)iI).AI_isAnyMemberDoVictoryStrategyLevel4())
								{
									int iDogpilePower = iOurPower;

									for (int iJ = 0; iJ < MAX_TEAMS; iJ++)
									{
										if (GET_TEAM((TeamTypes)iJ).isAlive())
										{
											if (iJ != iI)
											{
												if (atWar(((TeamTypes)iJ), ((TeamTypes)iI)))
												{
													iDogpilePower += GET_TEAM((TeamTypes)iJ).getPower(false);
												}
											}
										}
									}

									//FAssert(GET_TEAM((TeamTypes)iI).getPower(true) == GET_TEAM((TeamTypes)iI).getDefensivePower(getID()) || GET_TEAM((TeamTypes)iI).isAVassal());

									if (((GET_TEAM((TeamTypes)iI).getDefensivePower(getID()) * 3) / 2) < iDogpilePower)
									{
										int iValue = AI_startWarVal((TeamTypes)iI, WARPLAN_DOGPILE);

										/*
										if (iValue > 0 && gTeamLogLevel >= 2)
										{
											logBBAI("      Team %d (%S) considering starting DOGPILE warplan with team %d with value %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), iI, iValue);
										}
										*/
										if (iValue > iBestValue)
										{
											//FAssert(!AI_shareWar((TeamTypes)iI)); // disabled by K-Mod. (why is this even here?)
											iBestValue = iValue;
											eBestTeam = ((TeamTypes)iI);
										}
									}
								}
							}
						}
					}
				}

				if (eBestTeam != NO_TEAM)
				{
					/*
					if (gTeamLogLevel >= 1)
					{
						logBBAI("  Team %d (%S) starting DOGPILE warplan preparations with team %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), eBestTeam);
					}
					*/
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

int CvTeamAI::AI_getAttitudeWeight(TeamTypes eTeam) const
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
	
	for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
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
	
	m_aiEnemyCityDistance.resize(GC.getMapINLINE().numPlotsINLINE());
	m_aiEnemyUnitDistance.resize(GC.getMapINLINE().numPlotsINLINE());
	
	CvMap& kMap = GC.getMapINLINE();
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
	return m_aiEnemyCityDistance[GC.getMapINLINE().plotNumINLINE(pPlot->getX_INLINE(), pPlot->getY_INLINE())];
}

short CvTeamAI::AI_enemyUnitDistance(CvPlot* pPlot) const
{
	if (m_aiEnemyUnitDistance.empty())
	{
		return -1;
	}
	return m_aiEnemyUnitDistance[GC.getMapINLINE().plotNumINLINE(pPlot->getX_INLINE(), pPlot->getY_INLINE())];
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

int CvTeamAI::AI_calculateAdjacentLandPlots(TeamTypes eTeam) const
{
	PROFILE_FUNC();

	CvPlot* pLoopPlot;
	int iCount;
	int iI;

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	iCount = 0;

	for (iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

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


// Private Functions...
