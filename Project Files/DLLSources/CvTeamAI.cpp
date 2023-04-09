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

	for(pLoopArea = GC.getMap().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop))
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

	for(pLoopArea = GC.getMap().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop))
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


AreaAITypes CvTeamAI::AI_calculateAreaAIType(CvArea* pArea, bool bPreparingTotal) const
{
	PROFILE_FUNC();

	bool bRecentAttack;
	bool bTargets;
	bool bChosenTargets;
	bool bDeclaredTargets;
	int iOffensiveThreshold;
	int iAreaCities;
	int iI;

	if (hasNativePlayer() && !(pArea->isWater()))
	{
		int iBestValue = 0;
		WarPlanTypes eBestWarplan = NO_WARPLAN;
		TeamTypes eBestTeam = NO_TEAM;
		
		//Trying new code for natives only.
		for (iI = 0; iI < MAX_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				TeamTypes eLoopTeam = (TeamTypes)iI;
				
				WarPlanTypes eWarplan = AI_getWarPlan((TeamTypes)iI);
				
				if (eWarplan != NO_WARPLAN)
				{
					FAssert(((TeamTypes)iI) != getID());
					FAssert(isHasMet((TeamTypes)iI) || GC.getGameINLINE().isOption(GAMEOPTION_ALWAYS_WAR));

					if ((GET_TEAM((TeamTypes)iI).countNumCitiesByArea(pArea) > 0) || (GET_TEAM((TeamTypes)iI).countNumUnitsByArea(pArea) > 4))
					{
						int iValue = AI_warplanStrength(eWarplan);
						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							eBestWarplan = eWarplan;
							eBestTeam = eLoopTeam;
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
			if ((countPowerByArea(pArea) * 60) > (countEnemyPowerByArea(pArea) * 100))
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
		
		if (countEnemyPowerByArea(pArea) > 0)
		{
			return AREAAI_BALANCED;
		}
		
		return AREAAI_NEUTRAL;
	}
	else if (!(pArea->isWater()))
	{
		bRecentAttack = false;
		bTargets = false;
		bChosenTargets = false;
		bDeclaredTargets = false;

		bool bAssault = false;
		bool bPreparingAssault = false;

		if (bPreparingTotal)
		{
			iOffensiveThreshold = 25;
		}
		else
		{
			iOffensiveThreshold = 20;
		}

		iAreaCities = countNumCitiesByArea(pArea);
		
		bool bTotalTargets = false;

		for (iI = 0; iI < MAX_TEAMS; iI++)
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

		if (bDeclaredTargets)
		{
			return AREAAI_OFFENSIVE;
		}

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

                if (bPreparingAssault)
				{
					return AREAAI_ASSAULT_MASSING;
				}


				return AREAAI_ASSAULT;
			}
		}
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

int CvTeamAI::AI_startWarVal(TeamTypes eTeam) const
{
	PROFILE_FUNC();

	int iValue;
	
	int iValidity = AI_targetValidity(eTeam);
	if (iValidity == 0)
	{
		return 0;
	}

	iValue = AI_calculatePlotWarValue(eTeam);

	iValue += (3 * AI_calculateCapitalProximity(eTeam)) / ((iValue > 0) ? 2 : 3);
	
	int iClosenessValue = AI_teamCloseness(eTeam);
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

	switch (AI_getAttitude(eTeam))
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


bool CvTeamAI::AI_isSneakAttackPreparing(TeamTypes eIndex) const
{
	return ((AI_getWarPlan(eIndex) == WARPLAN_PREPARING_LIMITED) || (AI_getWarPlan(eIndex) == WARPLAN_PREPARING_TOTAL));
}

/*
bool CvTeamAI::AI_isSneakAttackReady(TeamTypes eIndex) const
{
	return (AI_isChosenWar(eIndex) && !(AI_isSneakAttackPreparing(eIndex)));
}
*/

bool CvTeamAI::AI_isSneakAttackReady(TeamTypes eIndex) const
{
	//return (AI_isChosenWar(eIndex) && !(AI_isSneakAttackPreparing(eIndex))); // BtS
	// K-Mod (advc: originally in an overloaded function)
	if (eIndex != NO_TEAM)
		return !isAtWar(eIndex) && AI_isChosenWar(eIndex) && !AI_isSneakAttackPreparing(eIndex); // K-Mod
	/*
	for (TeamIter<MAJOR_CIV> it; it.hasNext(); ++it)
	{
		if (AI_isSneakAttackReady(it->getID()))
			return true;
	}
	*/

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (AI_isSneakAttackReady((TeamTypes)iI))
		{
			return true;
		}
	}

	return false;
	// K-Mod end
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
							if (AI_isPrimaryArea(pLoopArea))
							{
								if (GET_TEAM((TeamTypes)iI).countNumCitiesByArea(pLoopArea) > 0)
								{
									bShareValid = true;

									bOffensiveValid = false;

									if (AI_calculateAreaAIType(pLoopArea, true) == AREAAI_OFFENSIVE)
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

// Private Functions...
