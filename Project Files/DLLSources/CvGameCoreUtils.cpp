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


#define PATH_MOVEMENT_WEIGHT							(1000)
//#define PATH_RIVER_WEIGHT								(100)
#define PATH_RIVER_WEIGHT								(20) // K-Mod ( * river crossing penalty)
//#define PATH_CITY_WEIGHT								(100)
#define PATH_CITY_WEIGHT								(200) // K-Mod
//#define PATH_DEFENSE_WEIGHT							(10)
#define PATH_DEFENSE_WEIGHT								(4) // K-Mod. ( * defence bonus)
#define PATH_TERRITORY_WEIGHT							(5) // was 3
#define PATH_DOW_WEIGHT									(4) // advc.082
#define PATH_STEP_WEIGHT								(4) // was 2
#define PATH_STRAIGHT_WEIGHT							(2) // was 1
#define PATH_FOGBUST_WEIGHT							    (100)
// R&R, Robert Surcouf, Damage on Storm plots, Start
#define PATH_DAMAGE_WEIGHT								(500)
// R&R, Robert Surcouf, Damage on Storm plots, End
#define PATH_COMBAT_WEIGHT								(300) // K-Mod. penalty for having to fight along the way.

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

namespace
{
	// Returns true if no enemy was found near the kGroup's potential target plot x,y. False otherwise
	bool isPlotSafe(CvSelectionGroup& kGroup, int x, int y)
	{
		const CvPlayerAI& kPlayer = GET_PLAYER(kGroup.getHeadOwner());

		// in WTP (almost) all units can fight since they have non-zero combat strength.
		// Thus we need a different criteria for evaluating danger based on the unit role rather than
		// the unit's combat ability
		//if (!pSelectionGroup->canFight() && !pSelectionGroup->alwaysInvisible()) // BTS \ COLO default condition
		if (kPlayer.AI_needsProtection(kGroup.getHeadUnitAI()) && !kGroup.alwaysInvisible())
		{
			CvPlot* const pPlot = GC.getMapINLINE().plotINLINE(x, y);

			if (kGroup.getDomainType() == DOMAIN_SEA)
			{
				if (kPlayer.AI_getWaterDanger(pPlot, -1, true, true, false) > 0)
					return false;
			}

			if (kGroup.getDomainType() == DOMAIN_LAND)
			{
				if (kPlayer.AI_getPlotDanger(pPlot, 2, false) > 0)
					return false;
			}
		}

		// Plot is considered safe if not filtered(i.e. rejected) by any of the above functions
		return true;
	}

	// Utility function to quickly determine if the target plot is a sensible choice or not
	bool AI_allowMove(const CvSelectionGroup& kGroup, const CvPlot& kDestinationPlot)
	{
		if (kDestinationPlot.isGoody())
		{
			// Popping a land goody in owned terrioritory may result in a war declaration
			// unless the unit has the isNoBadGoodies flag set
			if (kGroup.getDomainType() == DOMAIN_LAND && kDestinationPlot.isOwned())
			{
				if (!kGroup.getHeadUnit()->isNoBadGoodies())
					return false;
			}
		}

		return true;
	}
}

int pathDestValid(int iToX, int iToY, const void* pointer, FAStar* finder)
{

	PROFILE_FUNC();

	CvPlot const& kToPlot = GC.getMap().getPlot(iToX, iToY);

	//pSelectionGroup = ((CvSelectionGroup *)pointer);
	// K-Mod
	CvSelectionGroup* pSelectionGroup = finder ? (CvSelectionGroup*)pointer : ((CvPathSettings*)pointer)->pGroup;
	int iFlags = finder ? gDLL->getFAStarIFace()->GetInfo(finder) : ((CvPathSettings*)pointer)->iFlags;
	// K-Mod end

	if (pSelectionGroup->atPlot(&kToPlot))
		return TRUE;

	if (pSelectionGroup->getDomainType() == DOMAIN_IMMOBILE)
		return FALSE;

	bool const bAIControl = pSelectionGroup->AI_isControlled();

	if (bAIControl)
	{	/*  BETTER_BTS_AI_MOD, Efficiency, 11/04/09, jdog5000: START
		switch order as AI_getAnyPlotDanger is more expensive */
		if (pSelectionGroup->getDomainType() == DOMAIN_LAND)
		{
			CvArea const& kGroupArea = *pSelectionGroup->area();
			if (!kToPlot.isArea(kGroupArea) &&
				/*!pSelectionGroup->canMoveAllTerrain() &&*/
				!kToPlot.isAdjacentToArea(kGroupArea.getID()))
			{
				return FALSE;
			}
		}	
		if (!(iFlags & MOVE_IGNORE_DANGER))
		{
			if (!isPlotSafe(*pSelectionGroup, iToX, iToY))
				return FALSE;		

			if (!AI_allowMove(*pSelectionGroup, kToPlot))
				return FALSE;
		}
		// BETTER_BTS_AI_MOD: END
	}

	if (bAIControl || kToPlot.isRevealed(pSelectionGroup->getHeadTeam(), false))
	{
		if (pSelectionGroup->isAmphibPlot(&kToPlot))
		{
			for (CLLNode<IDInfo> const* pUnitNode1 = pSelectionGroup->headUnitNode();
				pUnitNode1 != NULL; pUnitNode1 = pSelectionGroup->nextUnitNode(pUnitNode1))
			{
				CvUnit const* pLoopUnit1 = ::getUnit(pUnitNode1->m_data);
				if (pLoopUnit1->getCargo() > 0 && pLoopUnit1->domainCargo() == DOMAIN_LAND)
				{
					bool bValid = false;
					for (CLLNode<IDInfo>* pUnitNode2 = pLoopUnit1->plot()->headUnitNode();
						pUnitNode2 != NULL; pUnitNode2 = pLoopUnit1->plot()->nextUnitNode(pUnitNode2))
					{
						CvUnit const* pLoopUnit2 = ::getUnit(pUnitNode2->m_data);
						if (pLoopUnit2->getTransportUnit() == pLoopUnit1)
						{
							if (pLoopUnit2->isGroupHead())
							{
								//if (pLoopUnit2->getGroup()->canMoveOrAttackInto(pToPlot, (pSelectionGroup->AI_isDeclareWar(kToPlot) || (iFlags & MOVE_DECLARE_WAR))))
								if (pLoopUnit2->getGroup()->canMoveOrAttackInto(kToPlot, iFlags & MOVE_DECLARE_WAR, false, bAIControl)) // K-Mod. The new AI must be explicit about declaring war.
								{
									bValid = true;
									break;
								}
							}
						}
					}
					if (bValid)
						return TRUE;
				}
			}

			return FALSE;
		}
		else
		{
			if (!pSelectionGroup->canMoveOrAttackInto(kToPlot,
				//pSelectionGroup->AI_isDeclareWar(pToPlot) || (iFlags & MOVE_DECLARE_WAR))
				// K-Mod. The new AI must be explicit about declaring war.
				iFlags & MOVE_DECLARE_WAR, false, bAIControl))
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

#if 0
int pathCost(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pFromPlot;
	CvPlot* pToPlot;
	int iWorstCost;
	int iCost;
	int iWorstMovesLeft;
	int iMovesLeft;
	int iWorstMax;
	int iMax;

	pFromPlot = GC.getMapINLINE().plotSoren(parent->m_iX, parent->m_iY);
	FAssert(pFromPlot != NULL);
	pToPlot = GC.getMapINLINE().plotSoren(node->m_iX, node->m_iY);
	FAssert(pToPlot != NULL);

	//CvSelectionGroup* pSelectionGroup = ((CvSelectionGroup *)pointer);
	// K-Mod
	CvSelectionGroup* pSelectionGroup = finder ? (CvSelectionGroup*)pointer : ((CvPathSettings*)pointer)->pGroup;
	const int iFlags = finder ? gDLL->getFAStarIFace()->GetInfo(finder) : ((CvPathSettings*)pointer)->iFlags;
	// K-Mod end

	//iWorstCost = MAX_INT;
	iWorstCost = 0;
	iWorstMovesLeft = MAX_INT;
	iWorstMax = MAX_INT;

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
							/*
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
							*/
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
#endif

// This function has been completely rewritten for K-Mod. (the rewrite includes some bug fixes as well as some new features)
int pathCost(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	//PROFILE_FUNC(); // advc.003o

	CvMap const& m = GC.getMap(); // advc: ... and use CvPlot references:
	CvPlot const& kFromPlot = m.getPlot(parent->m_iX, parent->m_iY);
	CvPlot const& kToPlot = m.getPlot(node->m_iX, node->m_iY);

	//CvSelectionGroup* pSelectionGroup = ((CvSelectionGroup *)pointer);
	// K-Mod
	CvSelectionGroup* pSelectionGroup = finder ? (CvSelectionGroup*)pointer : ((CvPathSettings*)pointer)->pGroup;
	int iFlags = finder ? gDLL->getFAStarIFace()->GetInfo(finder) : ((CvPathSettings*)pointer)->iFlags;
	// K-Mod end


	int iWorstCost = 0;
	int iWorstMovesLeft = MAX_INT;
	int iWorstMaxMoves = MAX_INT;

	TeamTypes eTeam = pSelectionGroup->getHeadTeam();
	// <advc.035>
	//int const iFlipModifierDiv = 7;
	//int iFlipModifier = iFlipModifierDiv;
	// </advc.035>
	// K-Mod
	int iExploreModifier = 3; // (in thirds)
#if 0
	if (!kToPlot.isRevealed(eTeam))
	{
		if (pSelectionGroup->getAutomateType() == AUTOMATE_EXPLORE ||
			(!pSelectionGroup->isHuman() && (pSelectionGroup->getHeadUnitAIType() == UNITAI_EXPLORE || pSelectionGroup->getHeadUnitAIType() == UNITAI_EXPLORE_SEA)))
		{
			iExploreModifier = 2; // lower cost to encourage exploring unrevealed areas
		}
		else if (!kFromPlot.isRevealed(eTeam))
		{
			iExploreModifier = 4; // higher cost to discourage pathfinding deep into the unknown
		}
	}
#endif
	// K-Mod end
	// <advc.035>
#if 0
	else if (GC.getDefineBOOL(CvGlobals::OWN_EXCLUSIVE_RADIUS) &&
		(iFlags & MOVE_DECLARE_WAR) && eTeam != BARBARIAN_TEAM)
	{
		PlayerTypes const eSecondOwner = kToPlot.getSecondOwner();
		PlayerTypes const eFirstOwner = kToPlot.getOwner();
		if (eSecondOwner != NO_PLAYER && eFirstOwner != NO_PLAYER &&
			((pSelectionGroup->getDomainType() == DOMAIN_SEA) == kToPlot.isWater()))
		{	// Avoid tiles that flip from us to the enemy upon DoW
			if (TEAMID(eFirstOwner) == eTeam && (GET_TEAM(eTeam).isHuman() ?
				(!GET_TEAM(eTeam).isFriendlyTerritory(TEAMID(eSecondOwner)) &&
					!GET_TEAM(eTeam).isAtWar(TEAMID(eSecondOwner))) :
				GET_TEAM(eTeam).AI_isSneakAttackReady(TEAMID(eSecondOwner))))
				iFlipModifier++;
			// Seek out enemy tiles that will flip to us upon DoW
			if (TEAMID(eSecondOwner) == eTeam && (GET_TEAM(eTeam).isHuman() ?
				(!GET_TEAM(eTeam).isFriendlyTerritory(TEAMID(eFirstOwner)) &&
					!GET_TEAM(eTeam).isAtWar(TEAMID(eFirstOwner))) :
				GET_TEAM(eTeam).AI_isSneakAttackReady(TEAMID(eFirstOwner))))
				iFlipModifier--;
			/*  This could be done much more accurately, taking into account
			vassal agreements, defensive pacts, and going through the entire
			selection group, but I worry about the performance, and it's OK
			if it doesn't always work correctly. */
		}
	} // </advc.035>
#endif
	{
		for (CLLNode<IDInfo> const* pUnitNode = pSelectionGroup->headUnitNode();
			pUnitNode != NULL; pUnitNode = pSelectionGroup->nextUnitNode(pUnitNode))
		{
			CvUnit const* pLoopUnit = ::getUnit(pUnitNode->m_data);
			int iMoveCost = kToPlot.movementCost(pLoopUnit, &kFromPlot,
				false); // advc.001i
			//FAssert(pLoopUnit->getDomainType() != DOMAIN_AIR);
#if GLOBAL_DEFINE_USE_CLASSIC_MOVEMENT_SYSTEM == 1
			int iMaxMoves = parent->m_iData1 > 0 ? parent->m_iData1 : pLoopUnit->maxMoves();
			int iMovesLeft = std::max(0, (iMaxMoves - iMoveCost));

			iWorstMovesLeft = std::min(iWorstMovesLeft, iMovesLeft);
			iWorstMaxMoves = std::min(iWorstMaxMoves, iMaxMoves);

			int iCost = PATH_MOVEMENT_WEIGHT * (iMovesLeft == 0 ? iMaxMoves : iMoveCost);
			iCost = (iCost * iExploreModifier) / 3;
			//iCost = (iCost * iFlipModifier) / iFlipModifierDiv; // advc.035
			if (iCost > iWorstCost)
			{
				iWorstCost = iCost;
				iWorstMovesLeft = iMovesLeft;
				iWorstMaxMoves = iMaxMoves;
			}
#else
			int iCost = PATH_MOVEMENT_WEIGHT * iMoveCost;
			iCost = (iCost * iExploreModifier) / 3;
			//iCost = (iCost * iFlipModifier) / iFlipModifierDiv; // advc.035
			if (iCost > iWorstCost)
			{
				iWorstCost = iCost;
			}
#endif
		}
	}

	iWorstCost += PATH_STEP_WEIGHT;

	// symmetry breaking. This is meant to prevent two paths from having equal cost.
	// (If two paths have equal cost, sometimes the interface shows one path and the units follow the other. This is bad.)
	/* original K-Mod symmetry breaking. (extra cost for turning a corner)
	if (parent->m_pParent)
	{
	const int map_width = m.getGridWidth();
	const int map_height = m.getGridHeight();

	#define WRAP_X(x) ((x) - ((x) > map_width/2 ? map_width : 0) + ((x) < -map_width/2 ? map_width : 0))
	#define WRAP_Y(y) ((y) - ((y) > map_height/2 ? map_height : 0) + ((y) < -map_height/2 ? map_height : 0))

	int start_x = parent->m_pParent->m_iX;
	int start_y = parent->m_pParent->m_iY;

	int dx1 = WRAP_X(pFromPlot->getX() - start_x);
	int dy1 = WRAP_Y(pFromPlot->getY() - start_y);
	int dx2 = WRAP_X(pToPlot->getX() - start_x);
	int dy2 = WRAP_Y(pToPlot->getY() - start_y);

	// cross product. (greater than zero => sin(angle) > 0 => angle > 0)
	int cross = dx1 * dy2 - dx2 * dy1;
	if (cross > 0)
	iWorstCost += PATH_ASYMMETRY_WEIGHT; // turning left
	else if (cross < 0)
	iWorstCost -= PATH_ASYMMETRY_WEIGHT; // turning right

	// woah - hang on. Does that say /minus/ asym weight?
	// Doesn't this guy know that bad things happen if the total weight is negative?
	// ...
	// take a breath.
	#if PATH_STEP_WEIGHT < PATH_ASYMMETRY_WEIGHT
	#error "I'm sorry, but I must demand that step weight be greater than asym weight."
	#endif
	// I think we're going to be ok.

	#undef WRAP_X
	#undef WRAP_Y
	} */

	// Unfortunately, the above code is not sufficient to fix the symmetry problem.
	// Here's a new method, which combines symmetry breaking with the old "straight path" effect.
	// Note: symmetry breaking is not important for AI controlled units.

	// It's actually marginally better strategy to move diagonally - for mapping reasons.
	// So let the AI prefer diagonal movement.
	// However, diagonal zig-zags will probably seem unnatural and weird to humans who are just trying to move in a straight line.
	// So let the pathfinding for human groups prefer cardinal movement.
	bool const bAIControl = pSelectionGroup->AI_isControlled();
	if (bAIControl)
	{
		if (kFromPlot.getX() == kToPlot.getX() || kFromPlot.getY() == kToPlot.getY())
			iWorstCost += PATH_STRAIGHT_WEIGHT;
	}
	else
	{
		if (kFromPlot.getX() != kToPlot.getX() && kFromPlot.getY() != kToPlot.getY())
			iWorstCost += PATH_STRAIGHT_WEIGHT * (1 + (node->m_iX + node->m_iY) % 2);
		iWorstCost += (node->m_iX + node->m_iY + 1) % 3;
	}
	// unfortunately, this simple method may have problems at the world-wrap boundries.
	// It's difficult to tell when to correct for wrap effects and when not to, because as soon as the
	// unit starts moving, the start position of the path changes, and so it's no longer posible to tell
	// whether or not the unit started on the other side of the boundry.  Drat.

	// end symmetry breaking.

	if (!kToPlot.isRevealed(eTeam, false))
		return iWorstCost;
#if 0
	// WTP: Not supported yet due to the sheer effort required to port everything from AdvCiv atm.
	// the cost of battle...
	if (iFlags & MOVE_ATTACK_STACK)
	{
		FAssert(bAIControl); // only the AI uses MOVE_ATTACK_STACK
		FAssert(pSelectionGroup->getDomainType() == DOMAIN_LAND);

		int iEnemyDefence = 0;

		if (kToPlot.isVisible(eTeam))
		{	/*  <advc.001> In the rare case that the AI plans war while animals
			still roam the map, the DefenceStrength computation will crash
			when it gets to the point where the UnitCombatType is accessed.
			(Actually, not so exotic b/c advc.300 allows animals to survive
			in continents w/o civ cities.) */
			CvUnit* pUnit = kToPlot.getUnitByIndex(0);
			if (pUnit != NULL && !pUnit->isAnimal()) // </advc.001>
			{
				iEnemyDefence = GET_PLAYER(pSelectionGroup->getOwner()).
					AI_localDefenceStrength(&kToPlot, NO_TEAM,
						pSelectionGroup->getDomainType(), 0, true, false,
						pSelectionGroup->isHuman());
			}
		}
		else
		{
			// plot not visible. use memory
			iEnemyDefence = GET_TEAM(eTeam).AI_getStrengthMemory(kToPlot.getX(), kToPlot.getY());
		}

		if (iEnemyDefence > 0)
		{
			iWorstCost += PATH_DAMAGE_WEIGHT;
			int iAttackRatio = std::max(10, 100 * pSelectionGroup->AI().AI_sumStrength(&kToPlot) / iEnemyDefence);
			// Note. I half intend to have pathValid return false whenever the above ratio is less than 100.
			// I just haven't done that yet, mostly because I'm worried about performance.
			if (iAttackRatio < 400)
			{
				iWorstCost += PATH_MOVEMENT_WEIGHT * GC.getMOVE_DENOMINATOR() * (400 - iAttackRatio) /
					std::min(150, iAttackRatio);
			}
			// else, don't worry about it too much.
		}
	} //
#endif
	  // <advc.082>
	TeamTypes eToPlotTeam = kToPlot.getTeam();
	/*  The AVOID_ENEMY code in the no-moves-left branch below doesn't stop the AI
	from trying to move _through_ enemy territory and thus declaring war
	earlier than necessary */
	if (bAIControl && (iFlags & MOVE_DECLARE_WAR) && eToPlotTeam != NO_TEAM &&
		eToPlotTeam != eTeam && GET_TEAM(eTeam).AI_isSneakAttackReady(eToPlotTeam))
		iWorstCost += PATH_DOW_WEIGHT;
	// </advc.082>
	if (iWorstMovesLeft <= 0)
	{
		if (eToPlotTeam != eTeam)
		{
			iWorstCost += PATH_TERRITORY_WEIGHT;
		}

		// Damage caused by features (for mods)
		if (PATH_DAMAGE_WEIGHT != 0)
		{
			const FeatureTypes featureType = kToPlot.getFeatureType();

			if (featureType != NO_FEATURE)
			{
				const CvFeatureInfo& kFeatureInfo = GC.getFeatureInfo(kToPlot.getFeatureType());

				//GC.getFeatureInfo(pToPlot->getFeatureType()).getTurnDamage()
				iWorstCost += PATH_DAMAGE_WEIGHT *
					std::max(0, kFeatureInfo.getTurnDamage()) /
					GC.getMAX_HIT_POINTS();
			}

			/* WTP: Not supported 
			if (kToPlot.getExtraMovePathCost() > 0)
			{
				iWorstCost += (PATH_MOVEMENT_WEIGHT * kToPlot.getExtraMovePathCost());
			}
			*/
		}
		// defence modifiers
		int iDefenceMod = 0;
		int iDefenceCount = 0;
		int iFromDefenceMod = 0; // defence bonus for our attacking units left behind
		int iAttackWeight = 0;
		int iAttackCount = 0;
		int iEnemies = kToPlot.getNumVisibleEnemyDefenders(pSelectionGroup->getHeadUnit());

		for (CLLNode<IDInfo> const* pUnitNode = pSelectionGroup->headUnitNode();
			pUnitNode != NULL; pUnitNode = pSelectionGroup->nextUnitNode(pUnitNode))
		{
			CvUnit const* pLoopUnit = ::getUnit(pUnitNode->m_data);
			if (!pLoopUnit->canFight())
				continue; // advc
			iDefenceCount++;
			if (pLoopUnit->canDefend(&kToPlot))
			{
				iDefenceMod += pLoopUnit->noDefensiveBonus() ? 0 :
					//pToPlot->defenseModifier(eTeam, false);
					GET_TEAM(eTeam).AI_plotDefense(kToPlot); // advc.012
			}
			else iDefenceMod -= 100; // we don't want to be here.

									 // K-Mod note. the above code doesn't count all defensive bonuses, unfortunately.
									 // We could count everything like this:
									 /*CombatDetails combat_details;
									 pLoopUnit->maxCombatStr(pToPlot, NULL, &combat_details);
									 iDefenceMod += combat_details.iModifierTotal;*/
									 // but that seems like overkill. I'm worried it would be too slow.

									 // defence for units who stay behind after attacking an enemy.
			if (iEnemies > 0)
			{
				// For human-controlled units, only apply the following effects for multi-step moves.
				// (otherwise this might prevent the user from attacking from where they want to attack from.)
				if (bAIControl || parent->m_iKnownCost != 0 || iFlags & MOVE_HAS_STEPPED)
				{
					iAttackCount++;
					iFromDefenceMod += pLoopUnit->noDefensiveBonus() ? 0 : kFromPlot.defenseModifier(eTeam, false);

					if (!kFromPlot.isCity())
					{
						iAttackWeight += PATH_CITY_WEIGHT;
						// it's done this way around rather than subtracting when in a city so that the overall adjustment can't be negative.
					}
					if (pLoopUnit->canAttack() && !pLoopUnit->isRiver() && kFromPlot.isRiverCrossing(directionXY(&kFromPlot, &kToPlot)))
					{
						iAttackWeight -= PATH_RIVER_WEIGHT * GC.getRIVER_ATTACK_MODIFIER(); // Note, river modifier will be negative.
																												//iAttackMod -= (PATH_MOVEMENT_WEIGHT * iMovesLeft);
					}
				}
				// If this is a direct attack move from a human player, make sure it is the best value move possible. (This allows humans to choose which plot they attack from.)
				// (note: humans have no way of ordering units to attack units en-route, so the fact that this is an attack move means we are at the destination.)
				else if (pLoopUnit->canAttack()) // parent->m_iKnownCost == 0 && !(iFlags & MOVE_HAS_STEPPED) && !bAIControl
					return PATH_STEP_WEIGHT; // DONE!
			}
		}
		//
		if (iAttackCount > 0)
		{
			// scale attack weights down if not all our units will need to fight.
			iAttackWeight *= std::min(iAttackCount, iEnemies);
			iAttackWeight /= iAttackCount;
			iFromDefenceMod *= std::min(iAttackCount, iEnemies);
			iFromDefenceMod /= iAttackCount;
			iAttackCount = std::min(iAttackCount, iEnemies);
		}
		//
		iWorstCost += PATH_DEFENSE_WEIGHT * std::max(0, (iDefenceCount * 200 - iDefenceMod) / std::max(1, iDefenceCount));
		iWorstCost += PATH_DEFENSE_WEIGHT * std::max(0, (iAttackCount * 200 - iFromDefenceMod) / std::max(1, iAttackCount));
		iWorstCost += std::max(0, iAttackWeight) / std::max(1, iAttackCount);
		// if we're in enemy territory, consider the sum of our defensive bonuses as well as the average
		if (kToPlot.isOwned() && atWar(eToPlotTeam, eTeam))
		{
			iWorstCost += PATH_DEFENSE_WEIGHT * std::max(0, (iDefenceCount * 200 - iDefenceMod) / 5);
			iWorstCost += PATH_DEFENSE_WEIGHT * std::max(0, (iAttackCount * 200 - iFromDefenceMod) / 5);
			iWorstCost += std::max(0, iAttackWeight) / 5;
		}

		// additional cost for ending turn in or adjacent to enemy territory based on flags (based on BBAI)
		if (iFlags & (MOVE_AVOID_ENEMY_WEIGHT_2 | MOVE_AVOID_ENEMY_WEIGHT_3))
		{
			if (kToPlot.isOwned() && GET_TEAM(eTeam).AI_getWarPlan(eToPlotTeam) != NO_WARPLAN)
			{
				iWorstCost *= (iFlags & MOVE_AVOID_ENEMY_WEIGHT_3) ? 3 : 2;
			}
			else
			{
				CvPlot* pAdjacentPlot;
				for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
				{
					pAdjacentPlot = plotDirection(kToPlot.getX(), kToPlot.getY(), (DirectionTypes)iI);
					if (pAdjacentPlot != NULL)
					{
						if (pAdjacentPlot->isOwned() && atWar(pAdjacentPlot->getTeam(), pSelectionGroup->getHeadTeam()))
						{
							if (iFlags & MOVE_AVOID_ENEMY_WEIGHT_3)
							{
								iWorstCost *= 3;
								iWorstCost /= 2;
							}
							else
							{
								iWorstCost *= 4;
								iWorstCost /= 3;
							}
						}
					}
				}
			}
		}
	}

	FAssert(iWorstCost > 0);

	return iWorstCost;
}

int pathValid_join(FAStarNode* parent, FAStarNode* node, CvSelectionGroup* pSelectionGroup, int iFlags)  // advc: style changes
{
	CvMap const& kMap = GC.getMap();
	CvPlot const& kFromPlot = kMap.getPlot(parent->m_iX, parent->m_iY);
	CvPlot const& kToPlot = kMap.getPlot(node->m_iX, node->m_iY);
	// Ship can't cross isthmus 
	if (pSelectionGroup->getDomainType() == DOMAIN_SEA &&
		kFromPlot.isWater() && kToPlot.isWater() &&
		!kMap.getPlot(kFromPlot.getX(), kToPlot.getY()).isWater() &&
		!kMap.getPlot(kToPlot.getX(), kFromPlot.getY()).isWater() /*&&
		!pSelectionGroup->canMoveAllTerrain()*/) // WTP: Not supported
		return FALSE;
	return TRUE;
}

int pathValid_source(FAStarNode* parent, CvSelectionGroup* pSelectionGroup, int iFlags)  // advc: some style changes
{
	//PROFILE_FUNC(); // advc.003o
	CvPlot const& kFromPlot = GC.getMap().getPlot(parent->m_iX, parent->m_iY);
	//CvPlot* pToPlot = GC.getMap().plotSoren(node->m_iX, node->m_iY);

	if (pSelectionGroup->atPlot(&kFromPlot))
		return TRUE;

	if (iFlags & MOVE_SAFE_TERRITORY)
	{
		if (kFromPlot.isOwned() && kFromPlot.getTeam() != pSelectionGroup->getHeadTeam())
			return FALSE;

		if (!kFromPlot.isRevealed(pSelectionGroup->getHeadTeam(), false))
			return FALSE;
	}
	// <advc.049> No new AI routes in human territory (but upgrade to railroad OK)
	if (iFlags & MOVE_ROUTE_TO)
	{
		if (kFromPlot.getRevealedRouteType(pSelectionGroup->getHeadTeam(), false) == NO_ROUTE &&
			!pSelectionGroup->isHuman())
		{
			PlayerTypes eOwner = kFromPlot.getOwner();
			if (eOwner != NO_PLAYER && GET_PLAYER(eOwner).isHuman())
				return FALSE;
		}
	} // </advc.049>
	
	if (iFlags & MOVE_NO_ENEMY_TERRITORY && kFromPlot.isOwned() &&
		atWar(kFromPlot.getTeam(), pSelectionGroup->getHeadTeam()))
		return FALSE;

	bool const bAIControl = pSelectionGroup->AI_isControlled();
	
	if (bAIControl)
	{
		if (parent->m_iData2 > 1 || parent->m_iData1 == 0)
		{
			if (!(iFlags & MOVE_IGNORE_DANGER))
			{
				if (!isPlotSafe(*pSelectionGroup, kFromPlot.getX_INLINE(), kFromPlot.getY_INLINE()))
					return FALSE;
			}
		}
	}

	if (bAIControl || kFromPlot.isRevealed(pSelectionGroup->getHeadTeam(), false))
	{
		if (iFlags & (MOVE_THROUGH_ENEMY /* K-Mod: */ | MOVE_ATTACK_STACK))
		{
			if (!pSelectionGroup->canMoveOrAttackInto(kFromPlot,
				// K-Mod:
				iFlags & MOVE_DECLARE_WAR && !pSelectionGroup->isHuman()))
				return FALSE;
		}
		else
		{
			if (!pSelectionGroup->canMoveThrough(kFromPlot,
				// K-Mod
				iFlags & MOVE_DECLARE_WAR && !pSelectionGroup->isHuman(),
				iFlags & MOVE_ASSUME_VISIBLE || !pSelectionGroup->isHuman()))
				// K-Mod end
				return FALSE;
		}
	}
	// K-Mod. Note: it's currently difficult to extract the vision-cheating part of this AI,
	// because the AI needs to cheat inside canMoveOrAttackInto for its other cheating parts to work...
	//  .. anyway, here is the beginnings of what the code might look like without the cheats. (it's unfinished)
#if 0
	if (kFromPlot.isRevealed(pSelectionGroup->getHeadTeam(), false))
	{
		PROFILE("pathValid move through");
		CvTeamAI& kTeam = GET_TEAM(pSelectionGroup->getHeadTeam());

		int iEnemyDefence;
		if (kFromPlot.isVisible(pSelectionGroup->getHeadTeam(), false))
		{
			iEnemyDefence = GET_PLAYER(pSelectionGroup->getOwner()).AI_localDefenceStrength(pToPlot, NO_TEAM, pSelectionGroup->getDomainType(), 0, true, false, pSelectionGroup->isHuman());
		}
		else
		{
			iEnemyDefence = kTeam.AI_getStrengthMemory(&kFromPlot);
		}

		if (kTeam.AI_getStrengthMemory(&kFromPlot) > 0 && iFlags & (MOVE_THROUGH_ENEMY | MOVE_ATTACK_STACK))
		{
			if (!pSelectionGroup->canMoveOrAttackInto(kFromPlot) ||
				(iFlags & MOVE_ATTACK_STACK && pSelectionGroup->AI_sumStrength(&kFromPlot) < iEnemyDefence))
			{
				return FALSE;
			}
		}
		else
		{
			if (!pSelectionGroup->canMoveThrough(kFromPlot))
			{
				return FALSE;
			}
		}
	}
#endif
	// K-Mod end

	return TRUE;
}

int pathValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	PROFILE_FUNC();

	if (parent == NULL)
		return TRUE;
	// advc: Was unused (apart from an assertion)
	/*CvPlot* pFromPlot = ...;
	CvPlot* pToPlot = ...; */
	//pSelectionGroup = ((CvSelectionGroup *)pointer);
	// K-Mod
	CvSelectionGroup* pSelectionGroup = finder ? (CvSelectionGroup*)pointer :
		((CvPathSettings*)pointer)->pGroup;
	int iFlags = finder ? gDLL->getFAStarIFace()->GetInfo(finder) :
		((CvPathSettings*)pointer)->iFlags;
	// K-Mod end

	if (!pathValid_join(parent, node, pSelectionGroup, iFlags))
		return FALSE;

	//bResult = pathValidInternal(parent, node, data, pPathSettings, finder);
	return pathValid_source(parent, pSelectionGroup, iFlags);
}


int pathAdd(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{

	//PROFILE_FUNC(); // advc.003o
	//CvSelectionGroup* pSelectionGroup = ((CvSelectionGroup *)pointer);
	// K-Mod
	CvSelectionGroup* pSelectionGroup = finder ? (CvSelectionGroup*)pointer : ((CvPathSettings*)pointer)->pGroup;
	int iFlags = finder ? gDLL->getFAStarIFace()->GetInfo(finder) : ((CvPathSettings*)pointer)->iFlags;
	// K-Mod end
	FAssert(pSelectionGroup->getNumUnits() > 0);

	int iTurns = 1;
	int iMoves = MAX_INT;
	const bool bMoveMaxMoves = (iFlags & MOVE_MAX_MOVES);

	if (pSelectionGroup->maxMoves() == 0)
	// there is a unit in the group that cannot move at all e.g. construction supplies
	{
		node->m_iData1 = 0;
		node->m_iData2 = MAX_INT;

		return 0;
	}

	if (data == ASNC_INITIALADD)
	{
		// K-Mod. I've moved the code from here into separate functions.
		iMoves = bMoveMaxMoves ? pSelectionGroup->maxMoves() : pSelectionGroup->movesLeft();
		
		#if GLOBAL_DEFINE_USE_CLASSIC_MOVEMENT_SYSTEM == 0
			while (iMoves <= 0)
			{
				++iTurns;
				iMoves += pSelectionGroup->maxMoves();
			}
		#endif

		// K-Mod end
	}
	else
	{	// advc: use plot references
		CvPlot const& kFromPlot = GC.getMap().getPlot(parent->m_iX, parent->m_iY);
		CvPlot const& kToPlot = GC.getMap().getPlot(node->m_iX, node->m_iY);

		iMoves = parent->m_iData1;
		iTurns = parent->m_iData2;
		/*if (iStartMoves == 0)
		iTurns++;
		for (CLLNode<IDInfo>* pUnitNode = pSelectionGroup->headUnitNode(); pUnitNode != NULL; pUnitNode = pSelectionGroup->nextUnitNode(pUnitNode)) {
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		int iUnitMoves = (iStartMoves == 0 ? pLoopUnit->maxMoves() : iStartMoves);
		iUnitMoves -= pToPlot->movementCost(pLoopUnit, &kFromPlot);
		iUnitMoves = std::max(iUnitMoves, 0);
		iMoves = std::min(iMoves, iUnitMoves);
		}*/ // BtS
		// K-Mod. The original code would give incorrect results for groups where one unit had more moves but also had higher move cost.
		// (eg. the most obvious example is when a group with 1-move units and 2-move units is moving on a railroad. - In this situation,
		//  the original code would consistently underestimate the remaining moves at every step.)
		#if GLOBAL_DEFINE_USE_CLASSIC_MOVEMENT_SYSTEM == 1
			const bool bNewTurn = iMoves == 0;
 
			if (bNewTurn)
			{
				++iTurns;
				iMoves = pSelectionGroup->maxMoves();
			}
		#else // GLOBAL_DEFINE_USE_CLASSIC_MOVEMENT_SYSTEM == 0
		// add how many turns it takes before the unit can move again
			while (iMoves <= 0)
			{
				++iTurns;
				iMoves += pSelectionGroup->maxMoves();
			}
		#endif

		CLLNode<IDInfo> const* pUnitNode = pSelectionGroup->headUnitNode();
		int iMoveCost = kToPlot.movementCost(::getUnit(pUnitNode->m_data), &kFromPlot/*,
			false*/); // advc.001i
		bool bUniformCost = true;

		for (pUnitNode = pSelectionGroup->nextUnitNode(pUnitNode);
			bUniformCost && pUnitNode != NULL; pUnitNode = pSelectionGroup->nextUnitNode(pUnitNode))
		{
			CvUnit const* pLoopUnit = ::getUnit(pUnitNode->m_data);
			int iLoopCost = kToPlot.movementCost(pLoopUnit, &kFromPlot/*,
				false*/); // advc.001i
			if (iLoopCost != iMoveCost)
				bUniformCost = false;
		}

		if (bUniformCost)
		{
			// the simple, normal case
			#if GLOBAL_DEFINE_USE_CLASSIC_MOVEMENT_SYSTEM == 1
				iMoves = std::max(0, iMoves - iMoveCost);
			#else
				iMoves -= iMoveCost;
			#endif
		}
		else
		{
			PROFILE("pathAdd - non-uniform cost");
			// Move costs are uneven for units in this group.
			// To be sure of the true movement cost for the group, we need to calculate the movement cost for each unit for every step in this turn.
			std::vector<const CvPlot*> plot_list; // will be traversed in reverse order
			plot_list.push_back(&kToPlot);
			plot_list.push_back(&kFromPlot);
			FAStarNode* pStartNode = parent;
			while (pStartNode->m_iData2 == iTurns && pStartNode->m_pParent)
			{
				pStartNode = pStartNode->m_pParent;
				plot_list.push_back(GC.getMap().plotSoren(pStartNode->m_iX, pStartNode->m_iY));
			}
			iMoves = MAX_INT;
			const bool bMaxMoves = pStartNode->m_iData1 == 0 || bMoveMaxMoves;

			for (pUnitNode = pSelectionGroup->headUnitNode(); pUnitNode != NULL; pUnitNode = pSelectionGroup->nextUnitNode(pUnitNode))
			{
				CvUnit const* pLoopUnit = ::getUnit(pUnitNode->m_data);
				int iUnitMoves = bMaxMoves ? pLoopUnit->maxMoves() : pLoopUnit->movesLeft();
				for (size_t i = plot_list.size() - 1; i > 0; i--)
				{
					iUnitMoves -= plot_list[i - 1]->movementCost(pLoopUnit, plot_list[i]/*,
						false*/); // advc.001i
					FAssert(iUnitMoves > 0 || i == 1);
				}
				#if GLOBAL_DEFINE_USE_CLASSIC_MOVEMENT_SYSTEM == 1
					iUnitMoves = std::max(iUnitMoves, 0);
				#endif
				iMoves = std::min(iMoves, iUnitMoves);
			}
		}
		// K-Mod end
	}

	#if GLOBAL_DEFINE_USE_CLASSIC_MOVEMENT_SYSTEM == 1
		FAssertMsg(iMoves >= 0, "iMoves is expected to be non-negative (invalid Index)");
	#endif

	node->m_iData1 = iMoves;
	node->m_iData2 = iTurns > 0 ? iTurns : 1; // this avoids saving 0 turns, which might (?) lead to a div by 0 crash

	return 1;
}


int stepDestValid(int iToX, int iToY, const void* pointer, FAStar* finder)
{
	PROFILE_FUNC();

	CvPlot* pFromPlot;
	CvPlot* pToPlot;

	pFromPlot = GC.getMapINLINE().plotSoren(gDLL->getFAStarIFace()->GetStartX(finder), gDLL->getFAStarIFace()->GetStartY(finder));
	FAssert(pFromPlot != NULL);
	pToPlot = GC.getMapINLINE().plotSoren(iToX, iToY);
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

	pNewPlot = GC.getMapINLINE().plotSoren(node->m_iX, node->m_iY);

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
	CvPlot* pFromPlot = GC.getMapINLINE().plotSoren(parent->m_iX, parent->m_iY);
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

	pNewPlot = GC.getMapINLINE().plotSoren(node->m_iX, node->m_iY);

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

	pNewPlot = GC.getMapINLINE().plotSoren(node->m_iX, node->m_iY);

	const PlayerTypes ePlayer = ((PlayerTypes)(gDLL->getFAStarIFace()->GetInfo(finder)));

	// Erik: It's ok to check for a city here since cities they have to be separated by at least 1-non city plot
	// Erik: TODO: check that it's our own city! When we implement trading with natives\Europeans, we can allow it
	if (pNewPlot->isCity())
	{
		return true;
	}

	// WTP, ray, Canal - START
	if (pNewPlot->getImprovementType() != NO_IMPROVEMENT && GC.getImprovementInfo(pNewPlot->getImprovementType()).isCanal())
	{
		return true;
	}
	// WTP, ray, Canal - END

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
		//WTP, ray, Lakes
		if (pNewPlot->getTerrainType() == TERRAIN_COAST || pNewPlot->getTerrainType() == TERRAIN_SHALLOW_COAST || pNewPlot->getTerrainType() == TERRAIN_LARGE_RIVERS || pNewPlot->getTerrainType() == TERRAIN_LAKE || pNewPlot->getTerrainType() == TERRAIN_ICE_LAKE || pNewPlot->getTeam() == eTeam)
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

	pNewPlot = GC.getMapINLINE().plotSoren(node->m_iX, node->m_iY);

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

	const CvPlot* const pParentPlot = GC.getMapINLINE().plotSoren(parent->m_iX, parent->m_iY);
	const CvPlot* const pNodePlot = GC.getMapINLINE().plotSoren(node->m_iX, node->m_iY);
	return (pParentPlot->isWater() == pNodePlot->isWater() ? TRUE : FALSE);
}

int joinArea(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	if (data == ASNL_ADDCLOSED)
	{
		GC.getMapINLINE().plotSoren(node->m_iX, node->m_iY)->setArea(gDLL->getFAStarIFace()->GetInfo(finder));
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
	kMap.updateWaterPlotTerrainTypes(); // autodetect lakes
	const int iNumPlots = kMap.numPlotsINLINE();
	
	// reset visibility count as it is garbage right now
	// this might not be needed anymore, but keep it just to be safe
	// Nightinggale
	for (int iI = 0; iI < iNumPlots; ++iI)
	{
		kMap.plotByIndexINLINE(iI)->m_em_iVisibilityCount.reset();
	}

	// set proper cache for all plots (doable now that the entire savegame have been loaded)
	for (int iI = 0; iI < iNumPlots; ++iI)
	{
		kMap.plotByIndexINLINE(iI)->postLoadFixes();
	}

	// deal with players

	for (PlayerTypes ePlayer = FIRST_PLAYER; ePlayer < NUM_PLAYER_TYPES; ++ePlayer)
	{
		// deal with each players' cities
		
		int iLoop;
		for (CvCity* pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
		{
			pLoopCity->updateSlaveWorkerProductionBonus();
		}
		
		CvPlayer& kPlayer = GET_PLAYER(ePlayer);

		// calculate power and assets
		kPlayer.checkPower(true);
	
		// prepare list of city professions (aka citizens)
		if (kPlayer.getCivilizationType() != NO_CIVILIZATION)
		{
			kPlayer.m_validCityJobProfessions.clear();
			// Notes from Nightinggale
			// The xml data (all files) is ready at the end of CvXMLLoadUtility::readXMLfiles when bFirst is False
			// CvXMLLoadUtility::readXMLfiles is in CvXMLLoadUtilitySet.cpp
			// 
			// The CivEffect branch affects this in two ways:
			// 1: it adds CvPlayer::canUseProfession (which might change during the game due to CivEffects)
			// 2: it adds a class where the intended purpose is precisely what kValidCityJobs does (but it stores ProfessionTypes, not int) 
			//    Combine those two and CvPlayer can generate/cache kValidCityJobs whenever CivEffects change (rare event)
			//
			// Proposal: split kValidCityJobs into two and use bIndoorOnly to pick which one to use.
			//    Looks like a simple way to reduce the number of professions to look at and overhead is minimal if cached in CvPlayer.
			for (ProfessionTypes eProfession = FIRST_PROFESSION; eProfession < NUM_PROFESSION_TYPES; ++eProfession)
			{
				if (GC.getCivilizationInfo(kPlayer.getCivilizationType()).isValidProfession(eProfession) && GC.getProfessionInfo(eProfession).isCitizen())
				{
					kPlayer.m_validCityJobProfessions.push_back(eProfession);
				}
			}
		}
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

CvWString getStrategyString(StrategyTypes eStrategy)
{
	CvWString szString;

	switch (eStrategy)
	{
	case NO_STRATEGY: szString = L"NO_STRATEGY"; break;
	case STRATEGY_SMALL_WAVES: szString = L"STRATEGY_SMALL_WAVES"; break;
	case STRATEGY_BUILDUP: szString = L"STRATEGY_BUILDUP"; break;
	case STRATEGY_CONCENTRATED_ATTACK: szString = L"STRATEGY_CONCENTRATED_ATTACK"; break;
	case STRATEGY_DISTRIBUTED_ATTACK: szString = L"STRATEGY_DISTRIBUTED_ATTACK"; break;
	case STRATEGY_DIE_FIGHTING: szString = L"STRATEGY_DIE_FIGHTING"; break;
	case STRATEGY_FAST_BELLS: szString = L"STRATEGY_FAST_BELLS"; break;
	case STRATEGY_CASH_FOCUS: szString = L"STRATEGY_CASH_FOCUS"; break;
	case STRATEGY_SELL_TO_NATIVES: szString = L"STRATEGY_SELL_TO_NATIVES"; break;
	case STRATEGY_DENSE_CITY_SPACING: szString = L"STRATEGY_DENSE_CITY_SPACING"; break;
	case STRATEGY_MILITARY_BUILDUP: szString = L"STRATEGY_MILITARY_BUILDUP"; break;
	case STRATEGY_REVOLUTION_PREPARING: szString = L"STRATEGY_REVOLUTION_PREPARING"; break;
	case STRATEGY_REVOLUTION_DECLARING: szString = L"STRATEGY_REVOLUTION_DECLARING"; break;
	case STRATEGY_REVOLUTION: szString = L"STRATEGY_REVOLUTION"; break;
	default: szString = CvWString::format(L"UNKOWN_UNITAI_STATE(%d)", eStrategy); break;
	}

	return szString;
}
