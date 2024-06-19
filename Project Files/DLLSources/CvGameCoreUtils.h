#pragma once

// utils.h

#ifndef CIV4_GAMECORE_UTILS_H
#define CIV4_GAMECORE_UTILS_H


//#include "CvStructs.h"
#include "CvGlobals.h"
#include "CvMap.h"


class CvPlot;
class CvCity;
class CvUnit;
class CvString;
class CvRandom;
class FAStarNode;
class FAStar;
class CvInfoBase;


#ifndef SQR
#define SQR(x) ( (x)*(x))
#endif

#undef max
#undef min

//sign function taken from FirePlace - JW
template<class T> __forceinline T getSign( T x ) { return (( x < 0 ) ? T(-1) : x > 0 ? T(1) : T(0)); };

inline int range(int iNum, int iLow, int iHigh)
{
	FAssertMsg(iHigh >= iLow, "High should be higher than low");

	if (iNum < iLow)
	{
		return iLow;
	}
	else if (iNum > iHigh)
	{
		return iHigh;
	}
	else
	{
		return iNum;
	}
}

inline float range(float fNum, float fLow, float fHigh)
{
	FAssertMsg(fHigh >= fLow, "High should be higher than low");

	if (fNum < fLow)
	{
		return fLow;
	}
	else if (fNum > fHigh)
	{
		return fHigh;
	}
	else
	{
		return fNum;
	}
}

inline int coordDistance(int iFrom, int iTo, int iRange, bool bWrap)
{
	if (bWrap && (abs(iFrom - iTo) > (iRange / 2)))
	{
		return (iRange - abs(iFrom - iTo));
	}

	return abs(iFrom - iTo);
}

inline int wrapCoordDifference(int iDiff, int iRange, bool bWrap)
{
	if (bWrap)
	{
		if (iDiff > (iRange / 2))
		{
			return (iDiff - iRange);
		}
		else if (iDiff < -(iRange / 2))
		{
			return (iDiff + iRange);
		}
	}

	return iDiff;
}

inline int xDistance(int iFromX, int iToX)
{
	return coordDistance(iFromX, iToX, GC.getMap().getGridWidthINLINE(), GC.getMap().isWrapXINLINE());
}

inline int yDistance(int iFromY, int iToY)
{
	return coordDistance(iFromY, iToY, GC.getMap().getGridHeightINLINE(), GC.getMap().isWrapYINLINE());
}

inline int dxWrap(int iDX)
{
	return wrapCoordDifference(iDX, GC.getMap().getGridWidthINLINE(), GC.getMap().isWrapXINLINE());
}

inline int dyWrap(int iDY)
{
	return wrapCoordDifference(iDY, GC.getMap().getGridHeightINLINE(), GC.getMap().isWrapYINLINE());
}

// 4 | 4 | 3 | 3 | 3 | 4 | 4
// -------------------------
// 4 | 3 | 2 | 2 | 2 | 3 | 4
// -------------------------
// 3 | 2 | 1 | 1 | 1 | 2 | 3
// -------------------------
// 3 | 2 | 1 | 0 | 1 | 2 | 3
// -------------------------
// 3 | 2 | 1 | 1 | 1 | 2 | 3
// -------------------------
// 4 | 3 | 2 | 2 | 2 | 3 | 4
// -------------------------
// 4 | 4 | 3 | 3 | 3 | 4 | 4
//
// Returns the distance between plots according to the pattern above...
inline int plotDistance(int iX1, int iY1, int iX2, int iY2)
{
	int iDX;
	int iDY;

	iDX = xDistance(iX1, iX2);
	iDY = yDistance(iY1, iY2);

	return (std::max(iDX, iDY) + (std::min(iDX, iDY) / 2));
}

inline int plotDistance(Coordinates c1, Coordinates c2)
{
	const int iDX = xDistance(c1.x(), c2.x());
	const int iDY = yDistance(c1.y(), c2.y());

	return (std::max(iDX, iDY) + (std::min(iDX, iDY) / 2));
}

inline int plotDistance(CvPlot *p1, CvPlot *p2)
{
	return plotDistance(p1->coord(), p2->coord());
}


// 3 | 3 | 3 | 3 | 3 | 3 | 3
// -------------------------
// 3 | 2 | 2 | 2 | 2 | 2 | 3
// -------------------------
// 3 | 2 | 1 | 1 | 1 | 2 | 3
// -------------------------
// 3 | 2 | 1 | 0 | 1 | 2 | 3
// -------------------------
// 3 | 2 | 1 | 1 | 1 | 2 | 3
// -------------------------
// 3 | 2 | 2 | 2 | 2 | 2 | 3
// -------------------------
// 3 | 3 | 3 | 3 | 3 | 3 | 3
//
// Returns the distance between plots according to the pattern above...
inline int stepDistance(int iX1, int iY1, int iX2, int iY2)
{
	return std::max(xDistance(iX1, iX2), yDistance(iY1, iY2));
}

inline int stepDistance(const Coordinates c1, const Coordinates c2)
{
	return stepDistance(c1.x(), c1.y(), c2.x(), c2.y());
}

inline int stepDistance(const CvPlot *p1, const CvPlot *p2)
{
	return stepDistance(p1->coord(), p2->coord());
}

inline CvPlot* plotDirection(int iX, int iY, DirectionTypes eDirection)
{
	if(eDirection == NO_DIRECTION)
	{
		return GC.getMap().plotINLINE(iX, iY);
	}
	else
	{
		return GC.getMap().plotINLINE((iX + GC.getPlotDirectionX()[eDirection]), (iY + GC.getPlotDirectionY()[eDirection]));
	}
}

inline CvPlot* plotCardinalDirection(int iX, int iY, CardinalDirectionTypes eCardinalDirection)
{
	return GC.getMap().plotINLINE((iX + GC.getPlotCardinalDirectionX()[eCardinalDirection]), (iY + GC.getPlotCardinalDirectionY()[eCardinalDirection]));
}

inline CvPlot* plotXY(int iX, int iY, int iDX, int iDY)
{
	return GC.getMap().plotINLINE((iX + iDX), (iY + iDY));
}

inline CvPlot* plotXY(Coordinates baseCoord, RelCoordinates relCoord)
{
	return (baseCoord + relCoord).plot();
}

inline DirectionTypes directionXY(int iDX, int iDY)
{
	if ((abs(iDX) > DIRECTION_RADIUS) || (abs(iDY) > DIRECTION_RADIUS))
	{
		return NO_DIRECTION;
	}
	else
	{
		return GC.getXYDirection((iDX + DIRECTION_RADIUS), (iDY + DIRECTION_RADIUS));
	}
}

inline DirectionTypes directionXY(const CvPlot* pFromPlot, const CvPlot* pToPlot)
{
	return directionXY(dxWrap(pToPlot->getX_INLINE() - pFromPlot->getX_INLINE()), dyWrap(pToPlot->getY_INLINE() - pFromPlot->getY_INLINE()));
}

inline DirectionTypes directionXY(CvPlot const& kFromPlot, CvPlot const& kToPlot) // advc: params as references
{
	return directionXY(
		dxWrap(kToPlot.getX() - kFromPlot.getX()),
		dyWrap(kToPlot.getY() - kFromPlot.getY()));
}

CvPlot* plotCity(int iX, int iY, int iIndex);
int plotCityXY(int iDX, int iDY);
int plotCityXY(const CvCity* pCity, const CvPlot* pPlot);

// macro to loop all plots within a certain range
// preferred approach if loop order doesn't matter as it is optimized for memory layout (max usage of CPU cache)
// produces iLoopX and iLoopY intended for CvMap::plotINLINE.
// WARNING: plotINLINE can return NULL. Never use such a pointer before checking if it's NULL first.
// NULL only happens if the plot is outside the map, but as a general rule the game shouldn't crash if that happens.
//   Nightinggale
#define LOOP_ADJACENT_PLOTS( iCenterX, iCenterY, iRange ) \
for (int iLoopY = iCenterY-iRange; iLoopY <= iCenterY+iRange; ++iLoopY) \
for (int iLoopX = iCenterX-iRange; iLoopX <= iCenterX+iRange; ++iLoopX)


DllExport bool isLeaderCivMatch(LeaderHeadTypes eLeader, CivilizationTypes eCiv, bool bHuman);

CardinalDirectionTypes getOppositeCardinalDirection(CardinalDirectionTypes eDir);
DirectionTypes cardinalDirectionToDirection(CardinalDirectionTypes eCard);
DllExport bool isCardinalDirection(DirectionTypes eDirection);
DirectionTypes estimateDirection(int iDX, int iDY);
DllExport DirectionTypes estimateDirection(const CvPlot* pFromPlot, const CvPlot* pToPlot);
DllExport float directionAngle(DirectionTypes eDirection);
int getDirectionDiff(DirectionTypes direction1, DirectionTypes direction2);
DirectionTypes getDirectionFrom_dX_dY(int dX, int dY);
bool atWar(TeamTypes eTeamA, TeamTypes eTeamB);
bool isPotentialEnemy(TeamTypes eOurTeam, TeamTypes eTheirTeam);

DllExport CvCity* getCity(const IDInfo& city);
DllExport CvUnit* getUnit(const IDInfo& unit);

bool isBeforeUnitCycle(const CvUnit* pFirstUnit, const CvUnit* pSecondUnit);

ImprovementTypes finalImprovementUpgrade(ImprovementTypes eImprovement, int iCount = 0);

__int64 getBinomialCoefficient(int iN, int iK);
int getCombatOdds(CvUnit* pAttacker, CvUnit* pDefender);
int getCombatOddsDraw(CvUnit* pAttacker, CvUnit* pDefender);


DllExport void setTradeItem(TradeData* pItem, TradeableItems eItemType, int iData1, const IDInfo* pTransport);

bool isPlotEventTrigger(EventTriggerTypes eTrigger);
DllExport void clear(wchar* szString);
DllExport void clear(char* szString);
DllExport void clear(std::string& szString);
DllExport void clear(std::wstring& szString);
DllExport void safecpy(char * szDest, const char * szSource, int iMaxLen);
DllExport void safecpy(wchar * szDest, const wchar * szSource, int iMaxLen);
DllExport void safecpy(CvWString& szDest, const CvWString& szSource, int iMaxLen);
DllExport bool isEmpty(const char* szString);
DllExport bool isEmpty(const std::string& szStr);
DllExport bool isEmpty(const wchar* szString);
DllExport bool isEmpty(const std::wstring& szStr);
void setListHelp(wchar* szBuffer, const wchar* szStart, const wchar* szItem, const wchar* szSeparator, bool bFirst);
void setListHelp(CvWString& szBuffer, const wchar* szStart, const wchar* szItem, const wchar* szSeparator, bool bFirst);
void setListHelp(CvWStringBuffer& szBuffer, const wchar* szStart, const wchar* szItem, const wchar* szSeparator, bool bFirst);

// PlotUnitFunc's...
bool PUF_isGroupHead( const CvUnit* pUnit, int iData1 = -1, int iData2 = -1);
bool PUF_isPlayer( const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_isVisualTeam( const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_isCombatTeam(const CvUnit* pUnit, int iData1, int iData2);
bool PUF_isOtherTeam( const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_isEnemy( const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_isVisible( const CvUnit* pUnit, int iData1, int iData2 = -1);
DllExport bool PUF_isVisibleDebug(const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_canSiege( const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_isPotentialEnemy( const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_canDeclareWar( const CvUnit* pUnit, int iData1 = -1, int iData2 = -1);
bool PUF_canDefend( const CvUnit* pUnit, int iData1 = -1, int iData2 = -1);
bool PUF_cannotDefend( const CvUnit* pUnit, int iData1 = -1, int iData2 = -1);
bool PUF_canDefendGroupHead( const CvUnit* pUnit, int iData1 = -1, int iData2 = -1);
bool PUF_canDefendEnemy( const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_canDefendPotentialEnemy( const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_isFighting( const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_isUnitType( const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_isDomainType( const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_isUnitAIType( const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_isMissionAIType(CvUnit const* pUnit, int iMissionAI, int iDummy = -1); // K-Mod
// R&R, ray, Natives raiding party - START
bool PUF_isUnitAIStateType( const CvUnit* pUnit, int iData1, int iData2 = -1);
// R&R, ray, Natives raiding party - END
bool PUF_isCityAIType( const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_isNotCityAIType( const CvUnit* pUnit, int iData1, int iData2 = -1);
bool PUF_isSelected( const CvUnit* pUnit, int iData1 = -1, int iData2 = -1);
bool PUF_makeInfoBarDirty(CvUnit* pUnit, int iData1 = -1, int iData2 = -1);
bool PUF_isNoMission(const CvUnit* pUnit, int iData1 = -1, int iData2 = -1);
// TAC - AI Assault Sea - koma13, jdog5000(BBAI) - START
bool PUF_isAvailableUnitAITypeGroupie(const CvUnit* pUnit, int iData1, int iData2);
bool PUF_isUnitAITypeGroupie(const CvUnit* pUnit, int iData1, int iData2);
// TAC - AI Assault Sea - koma13, jdog5000(BBAI) - END

/// post load function - start - Nightinggale
void postLoadGameFixes();
/// post load function - end - Nightinggale

// Inet Stuff
void sendGameStats(wchar* pURL);

// FAStarFunc...
int pathDestValid(int iToX, int iToY, const void* pointer, FAStar* finder);
int pathHeuristic(int iFromX, int iFromY, int iToX, int iToY);
int pathCost(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
int pathValid_join(FAStarNode* parent, FAStarNode* node, CvSelectionGroup* pSelectionGroup, int iFlags); // K-Mod
int pathValid_source(FAStarNode* parent, CvSelectionGroup* pSelectionGroup, int iFlags); // K-Mod
int pathValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
int pathAdd(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
int stepDestValid(int iToX, int iToY, const void* pointer, FAStar* finder);
int stepHeuristic(int iFromX, int iFromY, int iToX, int iToY);
int stepValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
int stepCost(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
int stepAdd(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
int routeValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
int borderValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
int areaValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);
int joinArea(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder);

bool isPickableName(char const* szName);

int getTurnMonthForGame(int iGameTurn, int iStartYear, CalendarTypes eCalendar, GameSpeedTypes eSpeed);
int getTurnYearForGame(int iGameTurn, int iStartYear, CalendarTypes eCalendar, GameSpeedTypes eSpeed);

DllExport void boolsToString(const bool* pBools, int iNumBools, CvString* szOut);
DllExport void stringToBools(const char* szString, int* iNumBools, bool** ppBools);

void getDirectionTypeString(CvWString& szString, DirectionTypes eDirectionType);
void getCardinalDirectionTypeString(CvWString& szString, CardinalDirectionTypes eDirectionType);
void getActivityTypeString(CvWString& szString, ActivityTypes eActivityType);
void getMissionTypeString(CvWString& szString, MissionTypes eMissionType);
void getMissionAIString(CvWString& szString, MissionAITypes eMissionAI);
void getUnitAIString(CvWString& szString, UnitAITypes eUnitAI);
CvWString getUnitAIStateString(UnitAIStates eUnitAIState);
CvWString getStrategyString(StrategyTypes eStrategy);

bool shouldMoveBefore(const CvUnit* pUnitA, const CvUnit* pUnitB);
bool shouldUnitMove(const CvUnit* pUnit);

class CvShouldMoveBefore
{
public:
	CvShouldMoveBefore(PlayerTypes ePlayer) : m_ePlayer(ePlayer) {}

	bool operator()(int iUnitIdA, int iUnitIdB) const
	{
		return shouldMoveBefore(getUnit(IDInfo(m_ePlayer, iUnitIdA)), getUnit(IDInfo(m_ePlayer, iUnitIdB)));
	}
private:
	PlayerTypes m_ePlayer;
};

class CvShouldUnitMove
{
public:
	CvShouldUnitMove(PlayerTypes ePlayer) : m_ePlayer(ePlayer) {}

	bool operator()(int iUnitId) const
	{
		return shouldUnitMove(getUnit(IDInfo(m_ePlayer, iUnitId)));
	}
private:
	PlayerTypes m_ePlayer;
};


/// one/two city plot radius
int getDefineFlagsForDLL();
CvString getCompileFlags(int iDefineFlags);
// city radius end

bool generatePathForHypotheticalUnit(const CvPlot* pFrom, const CvPlot* pTo, PlayerTypes ePlayer, UnitTypes eUnit, int iFlags = 0, int iMaxTurns = -1);

#endif
