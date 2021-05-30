#include "CvGameCoreDLL.h"
#include "CvGameCoreUtils.h"
#include <algorithm>
#include "CvUnit.h"
#include "CvGameAI.h"
#include "CvPlayerAI.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvRandom.h"
#include "FAStarNode.h"
#include "CvCity.h"
#include "CvTeamAI.h"
#include "CvInfos.h"
#include "CvGlobals.h"
#include "FProfiler.h"
#include "CvInitCore.h"

#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLEntityIFaceBase.h"
#include "CvDLLFAStarIFaceBase.h"


#define PATH_MOVEMENT_WEIGHT									(1000)
#define PATH_RIVER_WEIGHT											(100)
#define PATH_CITY_WEIGHT											(100)
#define PATH_DEFENSE_WEIGHT										(10)
#define PATH_TERRITORY_WEIGHT									(3)
#define PATH_STEP_WEIGHT											(2)
#define PATH_STRAIGHT_WEIGHT									(1)
#define PATH_FOGBUST_WEIGHT										(100)
// R&R, Robert Surcouf, Damage on Storm plots, Start
#define PATH_DAMAGE_WEIGHT										(2000)
// R&R, Robert Surcouf, Damage on Storm plots, End
CvPlot* plotCity(int iX, int iY, int iIndex)
{
	return GC.getMapINLINE().plotINLINE((iX + GC.getCityPlotX()[iIndex]), (iY + GC.getCityPlotY()[iIndex]));
}

int plotCityXY(int iDX, int iDY)
{
	if ((abs(iDX) > CITY_PLOTS_RADIUS) || (abs(iDY) > CITY_PLOTS_RADIUS))
	{
		return -1;
	}
	else
	{
		return GC.getXYCityPlot((iDX + CITY_PLOTS_RADIUS), (iDY + CITY_PLOTS_RADIUS));
	}
}

int plotCityXY(const CvCity* pCity, const CvPlot* pPlot)
{
	return plotCityXY(dxWrap(pPlot->getX_INLINE() - pCity->getX_INLINE()), dyWrap(pPlot->getY_INLINE() - pCity->getY_INLINE()));
}

bool isLeaderCivMatch(LeaderHeadTypes eLeader, CivilizationTypes eCiv, bool bHuman)
{
	CvCivilizationInfo& kCivilization = GC.getCivilizationInfo(eCiv);
	if (kCivilization.isLeaders(eLeader))
	{
		return true;
	}

	if (GC.getInitCore().getOption(GAMEOPTION_LEAD_ANY_CIV))
	{
		if (bHuman ? kCivilization.isPlayable() : kCivilization.isAIPlayable())
		{
			for (int iLoopCiv = 0; iLoopCiv < GC.getNumCivilizationInfos(); ++iLoopCiv)
			{
				CvCivilizationInfo& kLoopCiv = GC.getCivilizationInfo((CivilizationTypes) iLoopCiv);
				if (kLoopCiv.isLeaders(eLeader))
				{
					if (bHuman ? kLoopCiv.isPlayable() : kLoopCiv.isAIPlayable())
					{
						if (kCivilization.isNative() == kLoopCiv.isNative())
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

CardinalDirectionTypes getOppositeCardinalDirection(CardinalDirectionTypes eDir)
{
	return (CardinalDirectionTypes)((eDir + 2) % NUM_CARDINALDIRECTION_TYPES);
}

DirectionTypes cardinalDirectionToDirection(CardinalDirectionTypes eCard)
{
	switch (eCard)
	{
	case CARDINALDIRECTION_NORTH:
		return DIRECTION_NORTH;
	case CARDINALDIRECTION_EAST:
		return DIRECTION_EAST;
	case CARDINALDIRECTION_SOUTH:
		return DIRECTION_SOUTH;
	case CARDINALDIRECTION_WEST:
		return DIRECTION_WEST;
	}
	return NO_DIRECTION;
}

bool isCardinalDirection(DirectionTypes eDirection)
{
	switch( eDirection )
	{
	case DIRECTION_EAST:
	case DIRECTION_NORTH:
	case DIRECTION_SOUTH:
	case DIRECTION_WEST:
		return true;
	}
	return false;
}

DirectionTypes estimateDirection(int iDX, int iDY)
{
	const int displacementSize = 8;
	static float sqrt2 = 1 / sqrt(2.0f);
	//													N			NE			E			SE				S			SW				W			NW
	static float displacements[displacementSize][2] = {{0, 1}, {sqrt2, sqrt2}, {1, 0}, {sqrt2, -sqrt2}, {0, -1}, {-sqrt2, -sqrt2}, {-1, 0}, {-sqrt2, sqrt2}};
	float maximum = 0;
	int maximumIndex = -1;
	for(int i=0;i<displacementSize;i++)
	{
		float dotProduct = iDX * displacements[i][0] + iDY * displacements[i][1];
		if(dotProduct > maximum)
		{
			maximum = dotProduct;
			maximumIndex = i;
		}
	}

	return (DirectionTypes) maximumIndex;
}

DirectionTypes estimateDirection(const CvPlot* pFromPlot, const CvPlot* pToPlot)
{
	return estimateDirection(dxWrap(pToPlot->getX_INLINE() - pFromPlot->getX_INLINE()), dyWrap(pToPlot->getY_INLINE() - pFromPlot->getY_INLINE()));
}


float directionAngle( DirectionTypes eDirection )
{
	switch( eDirection )
	{
	case DIRECTION_NORTHEAST:	return fM_PI * 0.25f;
	case DIRECTION_EAST:			return fM_PI * 0.5f;
	case DIRECTION_SOUTHEAST:	return fM_PI * 0.75f;
	case DIRECTION_SOUTH:			return fM_PI * 1.0f;
	case DIRECTION_SOUTHWEST:	return fM_PI * 1.25f;
	case DIRECTION_WEST:			return fM_PI * 1.5f;
	case DIRECTION_NORTHWEST:	return fM_PI * 1.75f;
	default:
	case DIRECTION_NORTH:			return 0.0f;
	}
}

bool atWar(TeamTypes eTeamA, TeamTypes eTeamB)
{
	if (eTeamA == NO_TEAM || eTeamB == NO_TEAM)
	{
		return false;
	}
	else if (eTeamA == UNKNOWN_TEAM || eTeamB == UNKNOWN_TEAM)
	{
		return true;
	}
	else
	{
		FAssert(GET_TEAM(eTeamA).isAtWar(eTeamB) == GET_TEAM(eTeamB).isAtWar(eTeamA));
		FAssert((eTeamA != eTeamB) || !(GET_TEAM(eTeamA).isAtWar(eTeamB)));
		return GET_TEAM(eTeamA).isAtWar(eTeamB);
	}
}

bool isPotentialEnemy(TeamTypes eOurTeam, TeamTypes eTheirTeam)
{
	if (atWar(eOurTeam, eTheirTeam))
	{
		return true;
	}

	if (eOurTeam != NO_TEAM)
	{
		if (GET_TEAM(eOurTeam).AI_isSneakAttackReady(eTheirTeam))
		{
			return true;
		}

	}

	return false;
}

CvCity* getCity(const IDInfo& city)
{
	if ((city.eOwner >= 0) && city.eOwner < MAX_PLAYERS)
	{
		return (GET_PLAYER((PlayerTypes)city.eOwner).getCity(city.iID));
	}

	return NULL;
}

CvUnit* getUnit(const IDInfo& unit)
{
	if ((unit.eOwner >= 0) && unit.eOwner < MAX_PLAYERS)
	{
		return (GET_PLAYER((PlayerTypes)unit.eOwner).getUnit(unit.iID));
	}

	return NULL;
}

bool isBeforeUnitCycle(const CvUnit* pFirstUnit, const CvUnit* pSecondUnit)
{
	FAssert(pFirstUnit != NULL);
	FAssert(pSecondUnit != NULL);
	FAssert(pFirstUnit != pSecondUnit);

	if (pFirstUnit->getOwnerINLINE() != pSecondUnit->getOwnerINLINE())
	{
		return (pFirstUnit->getOwnerINLINE() < pSecondUnit->getOwnerINLINE());
	}

	if (pFirstUnit->getDomainType() != pSecondUnit->getDomainType())
	{
		return (pFirstUnit->getDomainType() < pSecondUnit->getDomainType());
	}

	if (pFirstUnit->baseCombatStr() != pSecondUnit->baseCombatStr())
	{
		return (pFirstUnit->baseCombatStr() > pSecondUnit->baseCombatStr());
	}

	if (pFirstUnit->getUnitType() != pSecondUnit->getUnitType())
	{
		return (pFirstUnit->getUnitType() > pSecondUnit->getUnitType());
	}

	if (pFirstUnit->getLevel() != pSecondUnit->getLevel())
	{
		return (pFirstUnit->getLevel() > pSecondUnit->getLevel());
	}

	if (pFirstUnit->getExperience() != pSecondUnit->getExperience())
	{
		return (pFirstUnit->getExperience() > pSecondUnit->getExperience());
	}

	return (pFirstUnit->getID() < pSecondUnit->getID());
}

bool shouldMoveBefore(const CvUnit* pUnitA, const CvUnit* pUnitB)
{
	// Apparently it's possible for one of the pointer to be NULL so I've added
	// some extra protection
	FAssert(pUnitA);
	FAssert(pUnitB);

	const int iMovePriorityA = (pUnitA != NULL) ? pUnitA->AI_getMovePriority() : 0;
	const int iMovePriorityB = (pUnitB != NULL) ? pUnitB->AI_getMovePriority() : 0;
	
	const int iDiff = iMovePriorityA - iMovePriorityB;
	if (iDiff > 0)
	{
		return false;
	}
	else if (iDiff == 0)
	{
		return (pUnitA->getID() < pUnitB->getID());
	}
	return true;
}

bool shouldUnitMove(const CvUnit* pUnit)
{
	if (pUnit->isDead() || pUnit->isDelayedDeath())
	{
		return false;
	}
	if (!pUnit->isOnMap())
	{
		if (!((pUnit->getUnitTravelState() == UNIT_TRAVEL_STATE_IN_EUROPE) || (pUnit->getUnitTravelState() == UNIT_TRAVEL_STATE_IN_AFRICA)))
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	return true;
}

ImprovementTypes finalImprovementUpgrade(ImprovementTypes eImprovement, int iCount)
{
	FAssertMsg(eImprovement != NO_IMPROVEMENT, "Improvement is not assigned a valid value");

	if (iCount > GC.getNumImprovementInfos())
	{
		return NO_IMPROVEMENT;
	}

	if (GC.getImprovementInfo(eImprovement).getImprovementUpgrade() != NO_IMPROVEMENT)
	{
		return finalImprovementUpgrade(((ImprovementTypes)(GC.getImprovementInfo(eImprovement).getImprovementUpgrade())), (iCount + 1));
	}
	else
	{
		return eImprovement;
	}
}

// FUNCTION: getBinomialCoefficient
// Needed for getCombatOdds
// Returns int value, being the possible number of combinations
// of k draws out of a population of n
// Written by DeepO
__int64 getBinomialCoefficient(int iN, int iK)
{
	__int64 iTemp = 1;
	//take advantage of symmetry in combination, eg. 15C12 = 15C3
	iK = std::min(iK, iN - iK);

	//eg. 15C3 = (15 * 14 * 13) / (1 * 2 * 3) = 15 / 1 * 14 / 2 * 13 / 3 = 455
	for(int i=1;i<=iK;i++)
		iTemp = (iTemp * (iN - i + 1)) / i;

	// Make sure iTemp fits in an integer (and thus doesn't overflow)
	FAssert(iTemp < MAX_INT);

	return iTemp;
}


// FUNCTION: getCombatOdds
// Calculates combat odds, given two units
// Returns value from 0-1000
// Written by DeepO
int getCombatOdds(CvUnit* pAttacker, CvUnit* pDefender)
{
	// setup battle, calculate strengths and odds
	//////

	int iAttackerStrength = pAttacker->currCombatStr(NULL, NULL);
	int iAttackerFirepower = pAttacker->currFirepower(NULL, NULL);

	int iDefenderStrength = pDefender->currCombatStr(pDefender->plot(), pAttacker);
	int iDefenderFirepower = pDefender->currFirepower(pDefender->plot(), pAttacker);

	FAssert((iAttackerStrength + iDefenderStrength) > 0);
	FAssert((iAttackerFirepower + iDefenderFirepower) > 0);

	int iDefenderOdds = ((GC.getDefineINT("COMBAT_DIE_SIDES") * iDefenderStrength) / (iAttackerStrength + iDefenderStrength));
	if (iDefenderOdds == 0)
	{
		return 1000;
	}
	int iAttackerOdds = GC.getDefineINT("COMBAT_DIE_SIDES") - iDefenderOdds;
	if (iAttackerOdds == 0)
	{
		return 0;
	}

	int iStrengthFactor = ((iAttackerFirepower + iDefenderFirepower + 1) / 2);

	// calculate damage done in one round
	//////

	int iDamageToAttacker = std::max(1,((GC.getDefineINT("COMBAT_DAMAGE") * (iDefenderFirepower + iStrengthFactor)) / (iAttackerFirepower + iStrengthFactor)));
	int iDamageToDefender = std::max(1,((GC.getDefineINT("COMBAT_DAMAGE") * (iAttackerFirepower + iStrengthFactor)) / (iDefenderFirepower + iStrengthFactor)));

	// calculate needed rounds.
	// Needed rounds = round_up(health/damage)
	//////

	int iNeededRoundsAttacker = (std::max(0, pDefender->currHitPoints()) + iDamageToDefender - 1 ) / iDamageToDefender;
	int iNeededRoundsDefender = (pAttacker->currHitPoints() + iDamageToAttacker - 1 ) / iDamageToAttacker;
	int iMaxRounds = iNeededRoundsAttacker + iNeededRoundsDefender - 1;

	// Erik: This number should be in XML!
	int iMaxCombatRounds = 7;

	int iLastCombatRound = std::min(iMaxRounds, iMaxCombatRounds);

	float fOdds = 0;

	// Erik: I've modified this so we only compute the odds until the last round
	for (int iI4 = iNeededRoundsAttacker; iI4 <= iLastCombatRound; iI4++)
	{
		// Erik: Old comment was totally wrong LOL
		// odds of exactly iI4 out of iLastCombatRound draws.
		// f(k;n,p)=C(n,k)*(p^k)*((1-p)^(n-k))
		// this needs to be in floating point math
		//////

		fOdds += ((float)getBinomialCoefficient(iLastCombatRound, iI4)) * pow((((float)iAttackerOdds) / GC.getDefineINT("COMBAT_DIE_SIDES")), iI4) * pow((1.0f - (((float)iAttackerOdds) / GC.getDefineINT("COMBAT_DIE_SIDES"))), (iLastCombatRound - iI4));
	}

	return ((int)(1000.0 * (fOdds + 0.0005f)));
}

int getCombatOddsDraw(CvUnit* pAttacker, CvUnit* pDefender)
{
	// setup battle, calculate strengths and odds
	//////


	// Erik: TODO, Cache this variable
	const int iCombatDieSides = GC.getDefineINT("COMBAT_DIE_SIDES");

	int iAttackerStrength = pAttacker->currCombatStr(NULL, NULL);
	int iAttackerFirepower = pAttacker->currFirepower(NULL, NULL);

	int iDefenderStrength = pDefender->currCombatStr(pDefender->plot(), pAttacker);
	int iDefenderFirepower = pDefender->currFirepower(pDefender->plot(), pAttacker);

	FAssert((iAttackerStrength + iDefenderStrength) > 0);
	FAssert((iAttackerFirepower + iDefenderFirepower) > 0);

	int iDefenderOdds = ((GC.getDefineINT("COMBAT_DIE_SIDES") * iDefenderStrength) / (iAttackerStrength + iDefenderStrength));
	if (iDefenderOdds == 0)
	{
		return 1000;
	}
	int iAttackerOdds = GC.getDefineINT("COMBAT_DIE_SIDES") - iDefenderOdds;
	if (iAttackerOdds == 0)
	{
		return 0;
	}

	int iStrengthFactor = ((iAttackerFirepower + iDefenderFirepower + 1) / 2);

	// calculate damage done in one round
	//////

	int iDamageToAttacker = std::max(1, ((GC.getDefineINT("COMBAT_DAMAGE") * (iDefenderFirepower + iStrengthFactor)) / (iAttackerFirepower + iStrengthFactor)));
	int iDamageToDefender = std::max(1, ((GC.getDefineINT("COMBAT_DAMAGE") * (iAttackerFirepower + iStrengthFactor)) / (iDefenderFirepower + iStrengthFactor)));

	// calculate needed rounds.
	// Needed rounds = round_up(health/damage)
	//////

	int iNeededRoundsAttacker = (std::max(0, pDefender->currHitPoints()) + iDamageToDefender - 1) / iDamageToDefender;
	int iNeededRoundsDefender = (pAttacker->currHitPoints() + iDamageToAttacker - 1) / iDamageToAttacker;

	// Erik: This is not true anymore since the limited combat rounds feature was introduced
	int iMaxRounds = iNeededRoundsAttacker + iNeededRoundsDefender - 1;

	// Erik: This number should be in XML!
	int iMaxCombatRounds = 7;

	int iLastCombatRound = std::min(iMaxRounds, iMaxCombatRounds);

	// Now we determine the chance of a draw
	// The number of combat rounds must be equal to the combat limit (currently 7) and there cannot have been a winner
	// during this round.
	// The interval for draw outcomes is one less than iNeededRoundsAttacker for victory and iLastCombatRound - iNeededRoundsDefender + 1 turns for victory (Inclusive)
	// Other outcomes need not be considered

	float fDrawOdds = 0;

	for (int i = (iLastCombatRound - iNeededRoundsDefender + 1); i < iNeededRoundsAttacker; i++)
	{
		// odds of no winner during the last round
		fDrawOdds += ((float)getBinomialCoefficient(iLastCombatRound, i)) * pow((((float)iAttackerOdds) / GC.getDefineINT("COMBAT_DIE_SIDES")), i) * pow((1.0f - (((float)iAttackerOdds) / GC.getDefineINT("COMBAT_DIE_SIDES"))), (iLastCombatRound - i));
	}

	return ((int)(1000.0 * (fDrawOdds + 0.0005f)));
}

void setTradeItem(TradeData* pItem, TradeableItems eItemType, int iData1, const IDInfo* pTransport)
{
	pItem->m_eItemType = eItemType;
	pItem->m_iData1 = iData1;
	if (pTransport != NULL)
	{
		pItem->m_kTransport = *pTransport;
	}
	else
	{
		pItem->m_kTransport.reset();
	}
	pItem->m_bOffering = false;
	pItem->m_bHidden = false;
}

bool isPlotEventTrigger(EventTriggerTypes eTrigger)
{
	CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(eTrigger);

	if (kTrigger.getNumPlotsRequired() > 0)
	{
		if (kTrigger.getPlotType() != NO_PLOT)
		{
			return true;
		}

		if (kTrigger.getFeaturesRequired().getLength() > 0)
		{
			return true;
		}

		if (kTrigger.getTerrainsRequired().getLength() > 0)
		{
			return true;
		}

		if (kTrigger.getImprovementsRequired().getLength() > 0)
		{
			return true;
		}

		if (kTrigger.getRoutesRequired().getLength() > 0)
		{
			return true;
		}

		if (kTrigger.isUnitsOnPlot() && kTrigger.getUnitsRequired().getLength() > 0)
		{
			return true;
		}

		if (kTrigger.isPrereqEventCity() && !kTrigger.isPickCity())
		{
			return true;
		}
	}

	return false;
}
void clear(char* szString)
{
	szString[0] = '\0';
}

void clear(wchar* szString)
{
	szString[0] = L'\0';
}

void clear(std::wstring& szString)
{
	szString.clear();
}

void clear(std::string& szString)
{
	szString.clear();
}

void safecpy(CvWString& szDest, const CvWString& szSource, int iMaxLen)
{
	szDest = szSource;
	if (szDest.size()>(uint)iMaxLen)
		szDest[iMaxLen-1]=0;
}

void safecpy(char * szDest, const char * szSource, int iMaxLen)
{
	if (szSource)
	{
		strncpy(szDest, szSource, iMaxLen-1);
		szDest[iMaxLen-1] = 0;
	}
	else
	{
		szDest[0] = '\0';
	}
}
void safecpy(wchar * szDest, const wchar * szSource, int iMaxLen)
{
	if (szSource)
	{
		wcsncpy(szDest, szSource, iMaxLen-1);
		szDest[iMaxLen-1] = L'\0';
	}
	else
	{
		szDest[0] = L'\0';
	}
}

bool isEmpty(const char* szString)
{
	return (szString[0] == '\0');
}

bool isEmpty(const std::string& szStr)
{
	return (szStr.empty() || szStr[0] == '\0');
}

bool isEmpty(const wchar* szString)
{
	return (szString[0] == L'\0');
}

bool isEmpty(const std::wstring& szStr)
{
	return (szStr.empty() || szStr[0] == L'\0');
}

void setListHelp(wchar* szBuffer, const wchar* szStart, const wchar* szItem, const wchar* szSeparator, bool bFirst)
{
	if (bFirst)
	{
		wcscat(szBuffer, szStart);
	}
	else
	{
		wcscat(szBuffer, szSeparator);
	}

	wcscat(szBuffer, szItem);
}

void setListHelp(CvWString& szBuffer, const wchar* szStart, const wchar* szItem, const wchar* szSeparator, bool bFirst)
{
	if (bFirst)
	{
		szBuffer += szStart;
	}
	else
	{
		szBuffer += szSeparator;
	}

	szBuffer += szItem;
}

void setListHelp(CvWStringBuffer& szBuffer, const wchar* szStart, const wchar* szItem, const wchar* szSeparator, bool bFirst)
{
	if (bFirst)
	{
		szBuffer.append(szStart);
	}
	else
	{
		szBuffer.append(szSeparator);
	}

	szBuffer.append(szItem);
}

bool PUF_isGroupHead(const CvUnit* pUnit, int iData1, int iData2)
{
	return (pUnit->isGroupHead());
}

bool PUF_isPlayer(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	return (pUnit->getOwnerINLINE() == iData1);
}

bool PUF_isVisualTeam(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	PlayerTypes eOwner = pUnit->getVisualOwner((TeamTypes) iData2);
	if (eOwner == UNKNOWN_PLAYER)
	{
		return false;
	}
	return (GET_PLAYER(eOwner).getTeam() == iData1);
}

bool PUF_isCombatTeam(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	FAssertMsg(iData2 != -1, "Invalid data argument, should be >= 0");

	return (pUnit->getCombatTeam((TeamTypes)iData2, pUnit->plot()) == iData1 && !pUnit->isInvisible((TeamTypes)iData2, false, false));
}

bool PUF_isOtherTeam(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	TeamTypes eTeam = GET_PLAYER((PlayerTypes)iData1).getTeam();
	if (pUnit->canCoexistWithEnemyUnit(eTeam))
	{
		return false;
	}

	return (pUnit->getTeam() != eTeam);
}

bool PUF_isEnemy(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	FAssertMsg(iData2 != -1, "Invalid data argument, should be >= 0");

	TeamTypes eOtherTeam = GET_PLAYER((PlayerTypes)iData1).getTeam();
	TeamTypes eOurTeam = pUnit->getCombatTeam(eOtherTeam, pUnit->plot());

	if (pUnit->canCoexistWithEnemyUnit(eOtherTeam))
	{
		return false;
	}

	return (iData2 ? eOtherTeam != eOurTeam : atWar(eOtherTeam, eOurTeam));
}

bool PUF_isVisible(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	return !(pUnit->isInvisible(GET_PLAYER((PlayerTypes)iData1).getTeam(), false));
}

bool PUF_isVisibleDebug(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	return !(pUnit->isInvisible(GET_PLAYER((PlayerTypes)iData1).getTeam(), true));
}

bool PUF_canSiege(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	return pUnit->canSiege(GET_PLAYER((PlayerTypes)iData1).getTeam());
}

bool PUF_isPotentialEnemy(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	FAssertMsg(iData2 != -1, "Invalid data argument, should be >= 0");

	TeamTypes eOtherTeam = GET_PLAYER((PlayerTypes)iData1).getTeam();
	TeamTypes eOurTeam = pUnit->getCombatTeam(eOtherTeam, pUnit->plot());

	if (pUnit->canCoexistWithEnemyUnit(eOtherTeam))
	{
		return false;
	}
	return (iData2 ? eOtherTeam != eOurTeam : isPotentialEnemy(eOtherTeam, eOurTeam));
}

bool PUF_canDeclareWar( const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	FAssertMsg(iData2 != -1, "Invalid data argument, should be >= 0");

	TeamTypes eOtherTeam = GET_PLAYER((PlayerTypes)iData1).getTeam();
	TeamTypes eOurTeam = pUnit->getCombatTeam(eOtherTeam, pUnit->plot());

	if (pUnit->canCoexistWithEnemyUnit(eOtherTeam))
	{
		return false;
	}

	return (iData2 ? false : GET_TEAM(eOtherTeam).canDeclareWar(eOurTeam));
}

bool PUF_canDefend(const CvUnit* pUnit, int iData1, int iData2)
{
	return pUnit->canDefend();
}

bool PUF_cannotDefend(const CvUnit* pUnit, int iData1, int iData2)
{
	return !(pUnit->canDefend());
}

bool PUF_canDefendGroupHead(const CvUnit* pUnit, int iData1, int iData2)
{
	return (PUF_canDefend(pUnit, iData1, iData2) && PUF_isGroupHead(pUnit, iData1, iData2));
}

bool PUF_canDefendEnemy(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	FAssertMsg(iData2 != -1, "Invalid data argument, should be >= 0");
	return (PUF_canDefend(pUnit, iData1, iData2) && PUF_isEnemy(pUnit, iData1, iData2));
}

bool PUF_canDefendPotentialEnemy(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	return (PUF_canDefend(pUnit, iData1, iData2) && PUF_isPotentialEnemy(pUnit, iData1, iData2));
}

bool PUF_isFighting(const CvUnit* pUnit, int iData1, int iData2)
{
	return pUnit->isFighting();
}

bool PUF_isDomainType(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	return (pUnit->getDomainType() == iData1);
}

bool PUF_isUnitType(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	return (pUnit->getUnitType() == iData1);
}

bool PUF_isUnitAIType(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	return (pUnit->AI_getUnitAIType() == iData1);
}

// R&R, ray, Natives raiding party - START
bool PUF_isUnitAIStateType(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	FAssertMsg(iData2 != -1, "Invalid data argument, should be >= 0");
	return ((pUnit->AI_getUnitAIType() == iData1) && (pUnit->AI_getUnitAIState() == iData2));
}
// R&R, ray, Natives raiding party - END

bool PUF_isCityAIType(const CvUnit* pUnit, int iData1, int iData2)
{
	return pUnit->AI_isCityAIType();
}

bool PUF_isNotCityAIType(const CvUnit* pUnit, int iData1, int iData2)
{
	return !(PUF_isCityAIType(pUnit, iData1, iData2));
}

bool PUF_isSelected(const CvUnit* pUnit, int iData1, int iData2)
{
	return pUnit->IsSelected();
}

bool PUF_makeInfoBarDirty(CvUnit* pUnit, int iData1, int iData2)
{
	pUnit->setInfoBarDirty(true);
	return true;
}

bool PUF_isNoMission(const CvUnit* pUnit, int iData1, int iData2)
{
	return (pUnit->getGroup()->getActivityType() != ACTIVITY_MISSION);
}

// TAC - AI Assault Sea - koma13, jdog5000(BBAI) - START
bool PUF_isAvailableUnitAITypeGroupie(const CvUnit* pUnit, int iData1, int iData2)
{
	return ((PUF_isUnitAITypeGroupie(pUnit,iData1,iData2)) && !(pUnit->isCargo()));
}

bool PUF_isUnitAITypeGroupie(const CvUnit* pUnit, int iData1, int iData2)
{
	CvUnit* pGroupHead = pUnit->getGroup()->getHeadUnit();
	return (PUF_isUnitAIType(pGroupHead,iData1,iData2));
}
// TAC - AI Assault Sea - koma13, jdog5000(BBAI) - END

int pathDestValid(int iToX, int iToY, const void* pointer, FAStar* finder)
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode1;
	CLLNode<IDInfo>* pUnitNode2;
	CvSelectionGroup* pSelectionGroup;
	CvUnit* pLoopUnit1;
	CvUnit* pLoopUnit2;
	CvPlot* pToPlot;
	bool bAIControl;
	bool bValid;

	pToPlot = GC.getMapINLINE().plotSorenINLINE(iToX, iToY);
	FAssert(pToPlot != NULL);

	pSelectionGroup = ((CvSelectionGroup *)pointer);

	if (pSelectionGroup->atPlot(pToPlot))
	{
		return TRUE;
	}

	if (pSelectionGroup->getDomainType() == DOMAIN_IMMOBILE)
	{
		return FALSE;
	}

	bAIControl = pSelectionGroup->AI_isControlled();

	if (bAIControl)
	{
		if (!(gDLL->getFAStarIFace()->GetInfo(finder) & MOVE_IGNORE_DANGER))
		{
			if (!(pSelectionGroup->canFight()) && !(pSelectionGroup->alwaysInvisible()))
			{
				if (GET_PLAYER(pSelectionGroup->getHeadOwner()).AI_getPlotDanger(pToPlot) > 0)
				{
					return FALSE;
				}
			}
		}

		// Bug? This may prevent AI units from considering land areas connected by river fords etc.
		/*
		if (pSelectionGroup->getDomainType() == DOMAIN_LAND)
		{
			int iGroupAreaID = pSelectionGroup->getArea();
			if (pToPlot->getArea() != iGroupAreaID)
			{
				if (!(pToPlot->isAdjacentToArea(iGroupAreaID)))
				{
					return FALSE;
				}
			}
		}
		*/
	}

	if (bAIControl || pToPlot->isRevealed(pSelectionGroup->getHeadTeam(), false))
	{
		if (pSelectionGroup->isAmphibPlot(pToPlot))
		{
			pUnitNode1 = pSelectionGroup->headUnitNode();

			while (pUnitNode1 != NULL)
			{
				pLoopUnit1 = ::getUnit(pUnitNode1->m_data);
				pUnitNode1 = pSelectionGroup->nextUnitNode(pUnitNode1);

				if ((pLoopUnit1->getCargo() > 0) && (pLoopUnit1->domainCargo() == DOMAIN_LAND))
				{
					bValid = false;

					pUnitNode2 = pLoopUnit1->plot()->headUnitNode();

					while (pUnitNode2 != NULL)
					{
						pLoopUnit2 = ::getUnit(pUnitNode2->m_data);
						pUnitNode2 = pLoopUnit1->plot()->nextUnitNode(pUnitNode2);

						if (pLoopUnit2->getTransportUnit() == pLoopUnit1)
						{
							if (pLoopUnit2->isGroupHead())
							{
								if (pLoopUnit2->getGroup()->canMoveOrAttackInto(pToPlot, (pSelectionGroup->AI_isDeclareWar(pToPlot) || (gDLL->getFAStarIFace()->GetInfo(finder) & MOVE_DECLARE_WAR))))
								{
									bValid = true;
									break;
								}
							}
						}
					}

					if (bValid)
					{
						return TRUE;
					}
				}
			}

			return FALSE;
		}
		else
		{
			if (!(pSelectionGroup->canMoveOrAttackInto(pToPlot, (pSelectionGroup->AI_isDeclareWar(pToPlot) || (gDLL->getFAStarIFace()->GetInfo(finder) & MOVE_DECLARE_WAR)))))
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}


int pathHeuristic(int iFromX, int iFromY, int iToX, int iToY)
{
	return (stepDistance(iFromX, iFromY, iToX, iToY) * PATH_MOVEMENT_WEIGHT);
}


int pathCost(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	CLLNode<IDInfo>* pUnitNode;
	CvSelectionGroup* pSelectionGroup;
	CvUnit* pLoopUnit;
	CvPlot* pFromPlot;
	CvPlot* pToPlot;
	int iWorstCost;
	int iCost;
	int iWorstMovesLeft;
	int iMovesLeft;
	int iWorstMax;
	int iMax;

	pFromPlot = GC.getMapINLINE().plotSorenINLINE(parent->m_iX, parent->m_iY);
	FAssert(pFromPlot != NULL);
	pToPlot = GC.getMapINLINE().plotSorenINLINE(node->m_iX, node->m_iY);
	FAssert(pToPlot != NULL);

	pSelectionGroup = ((CvSelectionGroup *)pointer);

	//iWorstCost = MAX_INT;
	iWorstCost = 0;
	iWorstMovesLeft = MAX_INT;
	iWorstMax = MAX_INT;

	const int iFlags = gDLL->getFAStarIFace()->GetInfo(finder);

	pUnitNode = pSelectionGroup->headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pSelectionGroup->nextUnitNode(pUnitNode);

		if (parent->m_iData1 > 0)
		{
			iMax = parent->m_iData1;
		}
		else
		{
			iMax = pLoopUnit->maxMoves();
		}

		iCost = pToPlot->movementCost(pLoopUnit, pFromPlot);

		iMovesLeft = std::max(0, (iMax - iCost));

		if (iMovesLeft <= iWorstMovesLeft)
		{
			if ((iMovesLeft < iWorstMovesLeft) || (iMax <= iWorstMax))
			{
				bool bIsRevealed = pToPlot->isRevealed(pLoopUnit->getTeam(), false);

				if (iMovesLeft == 0)
				{
					iCost = (PATH_MOVEMENT_WEIGHT * iMax);

					if (bIsRevealed)
					{
						if (pToPlot->getTeam() != pLoopUnit->getTeam())
						{
							iCost += PATH_TERRITORY_WEIGHT;
						}
					}

					// R&R, Robert Surcouf, Damage on Storm plots, Start
					if (pToPlot->getFeatureType() != NO_FEATURE)
					{
						const int iTurnDamage = GC.getFeatureInfo(pToPlot->getFeatureType()).getTurnDamage();

						if (iTurnDamage > 0)
						{
							iCost += (PATH_DAMAGE_WEIGHT * std::max(0, iTurnDamage)) / GC.getMAX_HIT_POINTS();
						}
					}

					// R&R, Robert Surcouf, Damage on Storm plots, End
					// TAC - AI Assault Sea - koma13, jdog5000(BBAI) - START
					// Add additional cost for ending turn in or adjacent to enemy territory based on flags
					if (iFlags & MOVE_AVOID_ENEMY_WEIGHT_3)
					{
						if (pToPlot->isOwned() && ((GET_TEAM(pSelectionGroup->getHeadTeam()).AI_getWarPlan(pToPlot->getTeam()) != NO_WARPLAN) || (pToPlot->getTeam() != pLoopUnit->getTeam() && pLoopUnit->isAlwaysHostile(pToPlot))))
						{
							iCost *= 3;
						}
						else
						{
							CvPlot* pAdjacentPlot;
							
							for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
							{
								pAdjacentPlot = plotDirection(pToPlot->getX_INLINE(), pToPlot->getY_INLINE(), ((DirectionTypes)iI));

								if( pAdjacentPlot != NULL )
								{
									if (pAdjacentPlot->isOwned() && (atWar(pAdjacentPlot->getTeam(), pSelectionGroup->getHeadTeam()) || (pAdjacentPlot->getTeam() != pLoopUnit->getTeam() && pLoopUnit->isAlwaysHostile(pAdjacentPlot))))
									{
										iCost *= 3;
										iCost /= 2;
									}
								}
							}
						}
					}
					else if (iFlags & MOVE_AVOID_ENEMY_WEIGHT_2)
					{
						if (pToPlot->isOwned() && ((GET_TEAM(pSelectionGroup->getHeadTeam()).AI_getWarPlan(pToPlot->getTeam()) != NO_WARPLAN) || (pToPlot->getTeam() != pLoopUnit->getTeam() && pLoopUnit->isAlwaysHostile(pToPlot))))
						{
							iCost *= 2;
						}
						else
						{
							CvPlot* pAdjacentPlot;
							int iI;
							for (iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
							{
								pAdjacentPlot = plotDirection(pToPlot->getX_INLINE(), pToPlot->getY_INLINE(), ((DirectionTypes)iI));
								
								if( pAdjacentPlot != NULL )
								{
									if (pAdjacentPlot->isOwned() && (atWar(pAdjacentPlot->getTeam(), pSelectionGroup->getHeadTeam()) || (pAdjacentPlot->getTeam() != pLoopUnit->getTeam() && pLoopUnit->isAlwaysHostile(pAdjacentPlot))))
									{
										iCost *= 4;
										iCost /= 3;
									}
								}
							}
						}
					}
				// TAC - AI Assault Sea - koma13, jdog5000(BBAI) - END
				}
				else
				{
					iCost = (PATH_MOVEMENT_WEIGHT * iCost);
				}

				if (pLoopUnit->canFight() && bIsRevealed)
				{
					if (iMovesLeft == 0)
					{
						iCost += (PATH_DEFENSE_WEIGHT * std::max(0, (200 - ((pLoopUnit->noDefensiveBonus()) ? 0 : pToPlot->defenseModifier(pLoopUnit->getTeam())))));
					}

					if (pSelectionGroup->AI_isControlled())
					{
						if (pLoopUnit->canAttack())
						{
							if (gDLL->getFAStarIFace()->IsPathDest(finder, pToPlot->getX_INLINE(), pToPlot->getY_INLINE()))
							{
								if (pToPlot->isVisibleEnemyDefender(pLoopUnit))
								{
									iCost += (PATH_DEFENSE_WEIGHT * std::max(0, (200 - ((pLoopUnit->noDefensiveBonus()) ? 0 : pFromPlot->defenseModifier(pLoopUnit->getTeam())))));

									if (!(pFromPlot->isCity()))
									{
										iCost += PATH_CITY_WEIGHT;
									}

									if (pFromPlot->isRiverCrossing(directionXY(pFromPlot, pToPlot)))
									{
										if (!(pLoopUnit->isRiver()))
										{
											iCost += (PATH_RIVER_WEIGHT * -(GC.getRIVER_ATTACK_MODIFIER()));
											iCost += (PATH_MOVEMENT_WEIGHT * iMovesLeft);
										}
									}
								}
							}
						}
					}
				}

				if (pSelectionGroup->AI_isControlled())
				{
					if (iFlags & MOVE_BUST_FOG)
					{
						for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
						{
							CvPlot* pLoopPlot = plotDirection(pToPlot->getX_INLINE(), pToPlot->getY_INLINE(), (DirectionTypes)iI);
							if (pLoopPlot != NULL)
							{
								if (pLoopPlot->isRevealed(pSelectionGroup->getTeam(), false))
								{
									iCost += PATH_FOGBUST_WEIGHT;
								}
							}
						}
					}
				}

				//if (iCost < iWorstCost)
				if (iCost > iWorstCost) // K-Mod. (no comment)
				{
					iWorstCost = iCost;
					iWorstMovesLeft = iMovesLeft;
					iWorstMax = iMax;
				}
			}
		}
	}

	//FAssert(iWorstCost != MAX_INT);

	iWorstCost += PATH_STEP_WEIGHT;

	if ((pFromPlot->getX_INLINE() != pToPlot->getX_INLINE()) && (pFromPlot->getY_INLINE() != pToPlot->getY_INLINE()))
	{
		iWorstCost += PATH_STRAIGHT_WEIGHT;
	}

	FAssert(iWorstCost > 0);

	return iWorstCost;
}


int pathValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	CvSelectionGroup* pSelectionGroup;
	CvPlot* pFromPlot;
	CvPlot* pToPlot;
	bool bAIControl;

	if (parent == NULL)
	{
		return TRUE;
	}

	pFromPlot = GC.getMapINLINE().plotSorenINLINE(parent->m_iX, parent->m_iY);
	FAssert(pFromPlot != NULL);
	pToPlot = GC.getMapINLINE().plotSorenINLINE(node->m_iX, node->m_iY);
	FAssert(pToPlot != NULL);

	pSelectionGroup = ((CvSelectionGroup *)pointer);

	// XXX might want to take this out...
	if (pSelectionGroup->getDomainType() == DOMAIN_SEA)
	{
		if (pFromPlot->isWater() && pToPlot->isWater())
		{
			if (!(GC.getMapINLINE().plotINLINE(parent->m_iX, node->m_iY)->isWater()) && !(GC.getMapINLINE().plotINLINE(node->m_iX, parent->m_iY)->isWater()))
			{
				return FALSE;
			}
		}
	}

	if (pSelectionGroup->atPlot(pFromPlot))
	{
		return TRUE;
	}

	const int info = gDLL->getFAStarIFace()->GetInfo(finder);

	if (info & MOVE_SAFE_TERRITORY)
	{
		if (!(pFromPlot->isRevealed(pSelectionGroup->getHeadTeam(), false)))
		{
			return FALSE;
		}

		if (pFromPlot->isOwned())
		{
			if (pFromPlot->getTeam() != pSelectionGroup->getHeadTeam())
			{
				return FALSE;
			}
		}
	}

	if (info & MOVE_NO_ENEMY_TERRITORY)
	{
		if (pFromPlot->isOwned())
		{
			if (atWar(pFromPlot->getTeam(), pSelectionGroup->getHeadTeam()))
			{
				return FALSE;
			}
		}
	}

	bAIControl = pSelectionGroup->AI_isControlled();

	if (bAIControl)
	{
		if ((parent->m_iData2 > 1) || (parent->m_iData1 == 0))
		{
			if (!(info & MOVE_IGNORE_DANGER))
			{
				if (!(pSelectionGroup->canFight()) && !(pSelectionGroup->alwaysInvisible()))
				{
					if (GET_PLAYER(pSelectionGroup->getHeadOwner()).AI_getPlotDanger(pFromPlot) > 0)
					{
						return FALSE;
					}
				}
			}
		}
	}

	if (bAIControl || pFromPlot->isRevealed(pSelectionGroup->getHeadTeam(), false))
	{
		if (info & MOVE_THROUGH_ENEMY)
		{
			if (!(pSelectionGroup->canMoveOrAttackInto(pFromPlot)))
			{
				return FALSE;
			}
		}
		else
		{
			if (!(pSelectionGroup->canMoveThrough(pFromPlot)))
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}


int pathAdd(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;
	CvSelectionGroup* pSelectionGroup;
	CvUnit* pLoopUnit;
	CvPlot* pFromPlot;
	CvPlot* pToPlot;
	int iStartMoves;
	int iMoves;
	int iTurns;

	pSelectionGroup = ((CvSelectionGroup *)pointer);

	if (data == ASNC_INITIALADD)
	{
		iMoves = MAX_INT;
		iTurns = 1;

		pUnitNode = pSelectionGroup->headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = pSelectionGroup->nextUnitNode(pUnitNode);

			iMoves = std::min(iMoves, pLoopUnit->movesLeft());
		}
	}
	else
	{
		pFromPlot = GC.getMapINLINE().plotSorenINLINE(parent->m_iX, parent->m_iY);
		FAssertMsg(pFromPlot != NULL, "FromPlot is not assigned a valid value");
		pToPlot = GC.getMapINLINE().plotSorenINLINE(node->m_iX, node->m_iY);
		FAssertMsg(pToPlot != NULL, "ToPlot is not assigned a valid value");

		iStartMoves = parent->m_iData1;
		iTurns = parent->m_iData2;

		if (iStartMoves == 0)
		{
			iTurns++;
		}

		iMoves = MAX_INT;

		pUnitNode = pSelectionGroup->headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = pSelectionGroup->nextUnitNode(pUnitNode);

			iMoves = std::min(iMoves, std::max(0, ((iStartMoves > 0) ? iStartMoves : pLoopUnit->maxMoves()) - pToPlot->movementCost(pLoopUnit, pFromPlot)));
		}
	}

	FAssertMsg(iMoves >= 0, "iMoves is expected to be non-negative (invalid Index)");

	node->m_iData1 = iMoves;
	node->m_iData2 = iTurns;

	return 1;
}


int stepDestValid(int iToX, int iToY, const void* pointer, FAStar* finder)
{
	PROFILE_FUNC();

	CvPlot* pFromPlot;
	CvPlot* pToPlot;

	pFromPlot = GC.getMapINLINE().plotSorenINLINE(gDLL->getFAStarIFace()->GetStartX(finder), gDLL->getFAStarIFace()->GetStartY(finder));
	FAssert(pFromPlot != NULL);
	pToPlot = GC.getMapINLINE().plotSorenINLINE(iToX, iToY);
	FAssert(pToPlot != NULL);

	if (pFromPlot->area() != pToPlot->area())
	{
		return FALSE;
	}

	return TRUE;
}


int stepHeuristic(int iFromX, int iFromY, int iToX, int iToY)
{
	return stepDistance(iFromX, iFromY, iToX, iToY);
}


int stepCost(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	return 1;
}


int stepValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	CvPlot* pNewPlot;

	if (parent == NULL)
	{
		return TRUE;
	}

	pNewPlot = GC.getMapINLINE().plotSorenINLINE(node->m_iX, node->m_iY);

	if (pNewPlot->isImpassable())
	{
		return FALSE;
	}
	

	// Super Forts begin *choke* 
	int iInvalidPlot = gDLL->getFAStarIFace()->GetInfo(finder);
	if(iInvalidPlot > 0)
	{
		// 1 is subtracted because 1 was added earlier to avoid a conflict with index 0
		if(pNewPlot == GC.getMapINLINE().plotByIndexINLINE((iInvalidPlot - 1)))
		{
			return FALSE;
		}
	}
	// Super Forts end - Note to mergers: Make sure you also include the code from Better BTS AI below this


/* 	BETTER_BTS_AI_MOD Bugfix	12/12/08 by jdog5000 - START	*/

/* original code
	if (GC.getMapINLINE().plotSorenINLINE(parent->m_iX, parent->m_iY)->area() != pNewPlot->area())
	{
		return FALSE;
	}
*/
	CvPlot* pFromPlot = GC.getMapINLINE().plotSorenINLINE(parent->m_iX, parent->m_iY);
	if (pFromPlot->area() != pNewPlot->area())
	{
		return FALSE;
	}

	// Don't count diagonal hops across land isthmus
	if (pFromPlot->isWater() && pNewPlot->isWater())
	{
		if (!(GC.getMapINLINE().plotINLINE(parent->m_iX, node->m_iY)->isWater()) && !(GC.getMapINLINE().plotINLINE(node->m_iX, parent->m_iY)->isWater()))
		{
			return FALSE;
		}
	}

/* 	BETTER_BTS_AI_MOD	END		*/


	return TRUE;
}


int stepAdd(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	if (data == ASNC_INITIALADD)
	{
		node->m_iData1 = 0;
	}
	else
	{
		node->m_iData1 = (parent->m_iData1 + 1);
	}

	FAssertMsg(node->m_iData1 >= 0, "node->m_iData1 is expected to be non-negative (invalid Index)");

	return 1;
}


int routeValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	CvPlot* pNewPlot;
	PlayerTypes ePlayer;

	if (parent == NULL)
	{
		return TRUE;
	}

	pNewPlot = GC.getMapINLINE().plotSorenINLINE(node->m_iX, node->m_iY);

	ePlayer = ((PlayerTypes)(gDLL->getFAStarIFace()->GetInfo(finder)));

	if (!(pNewPlot->isOwned()) || (pNewPlot->getTeam() == GET_PLAYER(ePlayer).getTeam()))
	{
		if (pNewPlot->getRouteType() == GET_PLAYER(ePlayer).getBestRoute(pNewPlot))
		{
			return TRUE;
		}
	}

	return FALSE;
}

int coastalRouteValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	CvPlot* pNewPlot;
	
	if (parent == NULL)
	{
		return true;
	}

	pNewPlot = GC.getMapINLINE().plotSorenINLINE(node->m_iX, node->m_iY);

	const PlayerTypes ePlayer = ((PlayerTypes)(gDLL->getFAStarIFace()->GetInfo(finder)));

	// Erik: It's ok to check for a city here since cities they have to be separated by at least 1-non city plot
	// Erik: TODO: check that it's our own city! When we implement trading with natives\Europeans, we can allow it
	if (pNewPlot->isCity())
	{
		return true;
	}

	const TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

	if (pNewPlot->isWater() && pNewPlot->isRevealed(eTeam, false) && !pNewPlot->isImpassable())
	{	
		const FeatureTypes featureType = pNewPlot->getFeatureType();

		// Erik: If the plot has a feature it can't be impassable
		if (featureType != NO_FEATURE)
		{
			const CvFeatureInfo& kFeatureInfo = GC.getFeatureInfo(featureType);
		
			// Erik: Just in case impassable terrain is added to the game (unused in WTP)
			if (!kFeatureInfo.isImpassable())
			{
				return false;
			}
		}

		//WTP, ray, Large Rivers
		// if (pNewPlot->getTerrainType() == TERRAIN_COAST || pNewPlot->getTeam() == eTeam)
		if (pNewPlot->getTerrainType() == TERRAIN_COAST || pNewPlot->getTerrainType() == TERRAIN_LARGE_RIVERS || pNewPlot->getTeam() == eTeam)
		{
			return true;
		}
	}

	return false;
}

int borderValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	CvPlot* pNewPlot;
	PlayerTypes ePlayer;

	if (parent == NULL)
	{
		return TRUE;
	}

	pNewPlot = GC.getMapINLINE().plotSorenINLINE(node->m_iX, node->m_iY);

	ePlayer = ((PlayerTypes)(gDLL->getFAStarIFace()->GetInfo(finder)));

	if (pNewPlot->getTeam() == GET_PLAYER(ePlayer).getTeam())
	{
		return TRUE;
	}

	return FALSE;
}


int areaValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	if (parent == NULL)
	{
		return TRUE;
	}

	return ((GC.getMapINLINE().plotSorenINLINE(parent->m_iX, parent->m_iY)->isWater() == GC.getMapINLINE().plotSorenINLINE(node->m_iX, node->m_iY)->isWater()) ? TRUE : FALSE);
}

int joinArea(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	if (data == ASNL_ADDCLOSED)
	{
		GC.getMapINLINE().plotSorenINLINE(node->m_iX, node->m_iY)->setArea(gDLL->getFAStarIFace()->GetInfo(finder));
	}

	return 1;
}

bool isPickableName(const TCHAR* szName)
{
	if (szName)
	{
		int iLen = _tcslen(szName);

		if (!_tcsicmp(&szName[iLen-6], "NOPICK"))
		{
			return false;
		}
	}

	return true;
}

int getTurnYearForGame(int iGameTurn, int iStartYear, CalendarTypes eCalendar, GameSpeedTypes eSpeed)
{
	return (getTurnMonthForGame(iGameTurn, iStartYear, eCalendar, eSpeed) / GC.getNumMonthInfos());
}

int getTurnMonthForGame(int iGameTurn, int iStartYear, CalendarTypes eCalendar, GameSpeedTypes eSpeed)
{
	int iTurnMonth;
	int iTurnCount;
	int iI;

	iTurnMonth = iStartYear * GC.getNumMonthInfos();

	switch (eCalendar)
	{
	case CALENDAR_DEFAULT:
		iTurnCount = 0;

		for (iI = 0; iI < GC.getGameSpeedInfo(eSpeed).getNumTurnIncrements(); iI++)
		{
			if (iGameTurn > (iTurnCount + GC.getGameSpeedInfo(eSpeed).getGameTurnInfo(iI).iNumGameTurnsPerIncrement))
			{
				iTurnMonth += (GC.getGameSpeedInfo(eSpeed).getGameTurnInfo(iI).iMonthIncrement * GC.getGameSpeedInfo(eSpeed).getGameTurnInfo(iI).iNumGameTurnsPerIncrement);
				iTurnCount += GC.getGameSpeedInfo(eSpeed).getGameTurnInfo(iI).iNumGameTurnsPerIncrement;
			}
			else
			{
				iTurnMonth += (GC.getGameSpeedInfo(eSpeed).getGameTurnInfo(iI).iMonthIncrement * (iGameTurn - iTurnCount));
				iTurnCount += (iGameTurn - iTurnCount);
				break;
			}
		}

		if (iGameTurn > iTurnCount)
		{
			iTurnMonth += (GC.getGameSpeedInfo(eSpeed).getGameTurnInfo(GC.getGameSpeedInfo(eSpeed).getNumTurnIncrements() - 1).iMonthIncrement * (iGameTurn - iTurnCount));
		}
		break;

	case CALENDAR_BI_YEARLY:
		iTurnMonth += (2 * iGameTurn * GC.getNumMonthInfos());
		break;

	case CALENDAR_YEARS:
	case CALENDAR_TURNS:
		iTurnMonth += iGameTurn * GC.getNumMonthInfos();
		break;

	case CALENDAR_SEASONS:
		iTurnMonth += (iGameTurn * GC.getNumMonthInfos()) / GC.getNumSeasonInfos();
		break;

	case CALENDAR_MONTHS:
		iTurnMonth += iGameTurn;
		break;

	case CALENDAR_WEEKS:
		iTurnMonth += iGameTurn / GC.getDefineINT("WEEKS_PER_MONTHS");
		break;

	default:
		FAssert(false);
	}

	return iTurnMonth;
}


void boolsToString(const bool* pBools, int iNumBools, CvString* szOut)
{
	*szOut = "";
	int i;
	for(i=0;i<iNumBools;i++)
	{
		*szOut += pBools[i] ? "1" : "0";
	}
}

//
// caller must call SAFE_DELETE_ARRAY on ppBools
//
void stringToBools(const char* szString, int* iNumBools, bool** ppBools)
{
	FAssertMsg(szString, "null string");
	if (szString)
	{
		*iNumBools = strlen(szString);
		*ppBools = new bool[*iNumBools];
		int i;
		for(i=0;i<*iNumBools;i++)
		{
			(*ppBools)[i] = (szString[i]=='1');
		}
	}
}

// these string functions should only be used under chipotle cheat code (not internationalized)

void getDirectionTypeString(CvWString& szString, DirectionTypes eDirectionType)
{
	switch (eDirectionType)
	{
	case NO_DIRECTION: szString = L"NO_DIRECTION"; break;

	case DIRECTION_NORTH: szString = L"north"; break;
	case DIRECTION_NORTHEAST: szString = L"northeast"; break;
	case DIRECTION_EAST: szString = L"east"; break;
	case DIRECTION_SOUTHEAST: szString = L"southeast"; break;
	case DIRECTION_SOUTH: szString = L"south"; break;
	case DIRECTION_SOUTHWEST: szString = L"southwest"; break;
	case DIRECTION_WEST: szString = L"west"; break;
	case DIRECTION_NORTHWEST: szString = L"northwest"; break;

	default: szString = CvWString::format(L"UNKNOWN_DIRECTION(%d)", eDirectionType); break;
	}
}

void getCardinalDirectionTypeString(CvWString& szString, CardinalDirectionTypes eDirectionType)
{
	getDirectionTypeString(szString, cardinalDirectionToDirection(eDirectionType));
}

void getActivityTypeString(CvWString& szString, ActivityTypes eActivityType)
{
	switch (eActivityType)
	{
	case NO_ACTIVITY: szString = L"NO_ACTIVITY"; break;

	case ACTIVITY_AWAKE: szString = L"ACTIVITY_AWAKE"; break;
	case ACTIVITY_HOLD: szString = L"ACTIVITY_HOLD"; break;
	case ACTIVITY_SLEEP: szString = L"ACTIVITY_SLEEP"; break;
	case ACTIVITY_HEAL: szString = L"ACTIVITY_HEAL"; break;
	case ACTIVITY_SENTRY: szString = L"ACTIVITY_SENTRY"; break;
	case ACTIVITY_MISSION: szString = L"ACTIVITY_MISSION"; break;

	default: szString = CvWString::format(L"UNKNOWN_ACTIVITY(%d)", eActivityType); break;
	}
}

void getMissionTypeString(CvWString& szString, MissionTypes eMissionType)
{
	switch (eMissionType)
	{
	case NO_MISSION: szString = L"NO_MISSION"; break;

	case MISSION_MOVE_TO: szString = L"MISSION_MOVE_TO"; break;
	case MISSION_ROUTE_TO: szString = L"MISSION_ROUTE_TO"; break;
	case MISSION_MOVE_TO_UNIT: szString = L"MISSION_MOVE_TO_UNIT"; break;
	case MISSION_SKIP: szString = L"MISSION_SKIP"; break;
	case MISSION_SLEEP: szString = L"MISSION_SLEEP"; break;
	case MISSION_FORTIFY: szString = L"MISSION_FORTIFY"; break;
	case MISSION_HEAL: szString = L"MISSION_HEAL"; break;
	case MISSION_SENTRY: szString = L"MISSION_SENTRY"; break;
	case MISSION_BOMBARD: szString = L"MISSION_BOMBARD"; break;
	case MISSION_PILLAGE: szString = L"MISSION_PILLAGE"; break;
	case MISSION_FOUND: szString = L"MISSION_FOUND"; break;
	case MISSION_JOIN_CITY: szString = L"MISSION_JOIN_CITY"; break;
	case MISSION_BUILD: szString = L"MISSION_BUILD"; break;
	case MISSION_LEAD: szString = L"MISSION_LEAD"; break;

	case MISSION_BEGIN_COMBAT: szString = L"MISSION_BEGIN_COMBAT"; break;
	case MISSION_END_COMBAT: szString = L"MISSION_END_COMBAT"; break;
	case MISSION_SURRENDER: szString = L"MISSION_SURRENDER"; break;
	case MISSION_CAPTURED: szString = L"MISSION_CAPTURED"; break;
	case MISSION_IDLE: szString = L"MISSION_IDLE"; break;
	case MISSION_DIE: szString = L"MISSION_DIE"; break;
	case MISSION_DAMAGE: szString = L"MISSION_DAMAGE"; break;
	case MISSION_MULTI_SELECT: szString = L"MISSION_MULTI_SELECT"; break;
	case MISSION_MULTI_DESELECT: szString = L"MISSION_MULTI_DESELECT"; break;
	case MISSION_WHALING: szString = L"MISSION_WHALING"; break; //TAC Whaling, ray
	case MISSION_FISHING: szString = L"MISSION_FISHING"; break; // R&R, ray, High Sea Fishing

	default: szString = CvWString::format(L"UNKOWN_MISSION(%d)", eMissionType); break;
	}
}

void getMissionAIString(CvWString& szString, MissionAITypes eMissionAI)
{
	switch (eMissionAI)
	{
	case NO_MISSIONAI: szString = L"NO_MISSIONAI"; break;

	case MISSIONAI_SHADOW: szString = L"MISSIONAI_SHADOW"; break;
	case MISSIONAI_GROUP: szString = L"MISSIONAI_GROUP"; break;
	case MISSIONAI_LOAD_ASSAULT: szString = L"MISSIONAI_LOAD_ASSAULT"; break;
	case MISSIONAI_LOAD_SETTLER: szString = L"MISSIONAI_LOAD_SETTLER"; break;
	case MISSIONAI_LOAD_SPECIAL: szString = L"MISSIONAI_LOAD_SPECIAL"; break;
	case MISSIONAI_GUARD_CITY: szString = L"MISSIONAI_GUARD_CITY"; break;
	case MISSIONAI_GUARD_BONUS: szString = L"MISSIONAI_GUARD_BONUS"; break;
	case MISSIONAI_GUARD_TRADE_NET: szString = L"MISSIONAI_GUARD_TRADE_NET"; break;
	case MISSIONAI_SPREAD: szString = L"MISSIONAI_SPREAD"; break;
	case MISSIONAI_EXPLORE: szString = L"MISSIONAI_EXPLORE"; break;
	case MISSIONAI_PILLAGE: szString = L"MISSIONAI_PILLAGE"; break;
	case MISSIONAI_FOUND: szString = L"MISSIONAI_FOUND"; break;
	case MISSIONAI_BUILD: szString = L"MISSIONAI_BUILD"; break;
	case MISSIONAI_ASSAULT: szString = L"MISSIONAI_ASSAULT"; break;
	case MISSIONAI_PICKUP: szString = L"MISSIONAI_PICKUP"; break;
	case MISSIONAI_AWAIT_PICKUP: szString = L"MISSIONAI_AWAIT_PICKUP"; break;
	case MISSIONAI_SAIL_TO_EUROPE: szString = L"MISSIONAI_SAIL_TO_EUROPE"; break;
	case MISSIONAI_SAIL_FROM_EUROPE: szString = L"MISSIONAI_SAIL_FROM_EUROPE"; break;
	case MISSIONAI_WORKER_SEA: szString = L"MISSIONAI_WORKER_SEA"; break; //TAC Whaling, ray
	case MISSIONAI_TRANSPORT: szString = L"MISSIONAI_TRANSPORT"; break;
	case MISSIONAI_TRANSPORT_SEA: szString = L"MISSIONAI_TRANSPORT_SEA"; break;
	case MISSIONAI_PIRACY: szString = L"MISSIONAI_PIRACY"; break;
	case MISSIONAI_BOMBARD: szString = L"MISSIONAI_BOMBARD"; break;
	case MISSIONAI_LEARN: szString = L"MISSIONAI_LEARN"; break;
	case MISSIONAI_SAIL_TO_AFRICA: szString = L"MISSIONAI_SAIL_TO_AFRICA"; break;
	case MISSIONAI_SAIL_FROM_AFRICA: szString = L"MISSIONAI_SAIL_FROM_EUROPE"; break;
	case MISSIONAI_SAIL_TO_PORT_ROYAL: szString = L"MISSIONAI_SAIL_TO_PORT_ROYAL"; break;
	case MISSIONAI_SAIL_FROM_PORT_ROYAL: szString = L"MISSIONAI_SAIL_FROM_PORT_ROYAL"; break;

	default: szString = CvWString::format(L"UNKOWN_MISSION_AI(%d)", eMissionAI); break;
	}
}

void getUnitAIString(CvWString& szString, UnitAITypes eUnitAI)
{
	// note, GC.getUnitAIInfo(eUnitAI).getDescription() is a international friendly way to get string (but it will be longer)

	switch (eUnitAI)
	{
	case NO_UNITAI: szString = L"no unitAI"; break;
	case UNITAI_UNKNOWN: szString = L"unknown"; break;
	case UNITAI_ANIMAL: szString = L"animal"; break; // R&R, ray, Wild Animals
	case UNITAI_ANIMAL_SEA: szString = L"animal sea"; break; // R&R, ray, Wild Animals
	case UNITAI_FLEEING: szString = L"fleeing"; break; // R&R, ray, Fleeing Units
	case UNITAI_COLONIST: szString = L"colonist"; break;
	case UNITAI_SETTLER: szString = L"settler"; break;
	case UNITAI_WORKER: szString = L"worker"; break;
	case UNITAI_MISSIONARY: szString = L"missionary"; break;
	case UNITAI_TRADER: szString = L"trade"; break; // WTP, ray, Native Trade Posts - START
	case UNITAI_SCOUT: szString = L"scout"; break;
	case UNITAI_WAGON: szString = L"wagon"; break;
	case UNITAI_TREASURE: szString = L"treasure"; break;
	case UNITAI_YIELD: szString = L"yield"; break;
	case UNITAI_GENERAL: szString = L"general"; break;
	case UNITAI_DEFENSIVE: szString = L"defensive"; break;
	case UNITAI_OFFENSIVE: szString = L"offensive"; break;
	case UNITAI_COUNTER: szString = L"counter"; break;
	case UNITAI_WORKER_SEA: szString = L"worker sea"; break; //TAC Whaling, ray
	case UNITAI_TRANSPORT_SEA: szString = L"transport sea"; break;
	case UNITAI_TRANSPORT_COAST: szString = L"transport coast"; break;
	case UNITAI_ASSAULT_SEA: szString = L"assault sea"; break;
	case UNITAI_COMBAT_SEA: szString = L"combat sea"; break;
	case UNITAI_PIRATE_SEA: szString = L"pirate sea"; break;
	case UNITAI_ESCORT_SEA: szString = L"escort sea"; break;	// TAC - AI Escort Sea - koma13
	default: szString = CvWString::format(L"unknown(%d)", eUnitAI); break;
	}
}

CvWString getUnitAIStateString(UnitAIStates eUnitAIState)
{
	CvWString szString;

	switch (eUnitAIState)
	{
	case NO_UNITAI_STATE: szString = L"NO_UNITAI_STATE"; break;
	case UNITAI_STATE_DEFAULT: szString = L"UNITAI_STATE_DEFAULT"; break;
	case UNITAI_STATE_BEARING_GIFTS: szString = L"UNITAI_STATE_BEARING_GIFTS"; break;
	case UNITAI_STATE_RETURN_HOME: szString = L"UNITAI_STATE_RETURN_HOME"; break;
	case UNITAI_STATE_WANDER: szString = L"UNITAI_STATE_WANDER"; break;
	case UNITAI_STATE_RAIDING_PARTY: szString = L"UNITAI_STATE_RAIDING_PARTY"; break;
	case UNITAI_STATE_BEARING_TRADE: szString = L"UNITAI_STATE_BEARING_TRADE"; break;
	case UNITAI_STATE_GUARDING: szString = L"UNITAI_STATE_GUARDING"; break;
	case UNITAI_STATE_ADVANCING: szString = L"UNITAI_STATE_ADVANCING"; break;
	case UNITAI_STATE_GROUPING: szString = L"UNITAI_STATE_GROUPING"; break;
	case UNITAI_STATE_CHARGING: szString = L"UNITAI_STATE_CHARGING"; break;
	case UNITAI_STATE_RETREATING: szString = L"UNITAI_STATE_RETREATING"; break;
	case UNITAI_STATE_PICKUP: szString = L"UNITAI_STATE_PICKUP"; break;
	case UNITAI_STATE_PURCHASED: szString = L"UNITAI_STATE_PURCHASED"; break;
	case UNITAI_STATE_SELL_TO_NATIVES: szString = L"UNITAI_STATE_SELL_TO_NATIVES"; break;
	case UNITAI_STATE_SAIL: szString = L"UNITAI_STATE_SAIL"; break;
	default: szString = CvWString::format(L"UNKOWN_UNITAI_STATE(%d)", eUnitAIState); break;
	}

	return szString;
}


/// post load function - start - Nightinggale
//
// This function is called whenever a savegame finish loading
//
void postLoadGameFixes()
{
#ifndef CHECK_GLOBAL_CONSTANTS
	GAME_IS_STARTING_UP = false;
#endif

	// deal with plots
	CvMap& kMap = GC.getMapINLINE();
	const int iNumPlots = kMap.numPlotsINLINE();
	
	// reset visibility count as it is garbage right now
	// this might not be needed anymore, but keep it just to be safe
	// Nightinggale
	for (int iI = 0; iI < iNumPlots; ++iI)
	{
		kMap.plotByIndexINLINE(iI)->m_em_iVisibilityCount.reset();
	}

	// set proper cache for all plots (doable now that the entire savegame have been loaded
	for (int iI = 0; iI < iNumPlots; ++iI)
	{
		kMap.plotByIndexINLINE(iI)->postLoadFixes();
	}
}
/// post load function - end - Nightinggale

/// one/two city plot radius
enum DefineFlags
{
	DEFINE_FLAG_ONE_PLOT_CITY_RADIUS         = 1 << 0,
};


int getDefineFlagsForDLL()
{
	int iDefineFlags = 0;
#if 0 // ONE_PLOT_CITY_RADIUS
	// value no longer used
	iDefineFlags += DEFINE_FLAG_ONE_PLOT_CITY_RADIUS;
#endif
	return iDefineFlags;
}

CvString getCompileFlags(int iDefineFlags)
{
	CvString szString(gDLL->getText("TXT_KEY_DLL_FLAG_ERROR"));

	if (iDefineFlags & DEFINE_FLAG_ONE_PLOT_CITY_RADIUS)
	{
		szString.append(" -DONE_PLOT_CITY_RADIUS");
	}

	return szString;
}
// city radius end

bool CvDLLUtilityIFaceBase::isOOSVisible() const
{
	long lResult = 0;
	gDLL->getPythonIFace()->callFunction(PYCPPModule, "isOOSVisible", NULL, &lResult);
	return lResult != 0;
}
