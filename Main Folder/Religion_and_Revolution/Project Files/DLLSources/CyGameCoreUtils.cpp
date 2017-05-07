#include "CvGameCoreDLL.h"
#include "CyGameCoreUtils.h"
#include "CvGameCoreUtils.h"
#include "CyPlot.h"
#include "CyCity.h"
#include "CyUnit.h"
int cyIntRange(int iNum, int iLow, int iHigh)
{
	return range(iNum, iLow, iHigh);
}
float cyFloatRange(float fNum, float fLow, float fHigh)
{
	return range(fNum, fLow, fHigh);
}
int cyDxWrap(int iDX)
{
	return dxWrap(iDX);
}
int cyDyWrap(int iDY)
{
	return dyWrap(iDY);
}
int cyPlotDistance(int iX, int iY, int iX2, int iY2)
{
	return plotDistance(iX, iY, iX2, iY2);
}
int cyStepDistance(int iX1, int iY1, int iX2, int iY2)
{
	return stepDistance(iX1, iY1, iX2, iY2);
}
CyPlot* cyPlotDirection(int iX, int iY, DirectionTypes eDirection)
{
	return new CyPlot(plotDirection(iX, iY, eDirection));
}
CyPlot* cyPlotCardinalDirection(int iX, int iY, CardinalDirectionTypes eCardDirection)
{
	return new CyPlot(plotCardinalDirection(iX, iY, eCardDirection));
}
CyPlot* cysPlotCardinalDirection(int iX, int iY, CardinalDirectionTypes eCardDirection)
{	static CyPlot plot;
plot.setPlot(plotCardinalDirection(iX, iY, eCardDirection));
return &plot;
}
CyPlot* cyPlotXY(int iX, int iY, int iDX, int iDY)
{
	return new CyPlot(plotXY(iX, iY, iDX, iDY));
}
CyPlot* cysPlotXY(int iX, int iY, int iDX, int iDY)
{
	static CyPlot plot;
	plot.setPlot(plotXY(iX, iY, iDX, iDY));
	return &plot;
}
DirectionTypes cyDirectionXYFromInt(int iDX, int iDY)
{
	return directionXY(iDX, iDY);
}
DirectionTypes cyDirectionXYFromPlot(CyPlot* pFromPlot, CyPlot* pToPlot)
{
	return directionXY(pFromPlot->getPlot(), pToPlot->getPlot());
}
CyPlot* cyPlotCity(int iX, int iY, int iIndex)
{
	return new CyPlot(plotCity(iX, iY, iIndex));
}
int cyPlotCityXYFromInt(int iDX, int iDY)
{
	return plotCityXY(iDX, iDY);
}
int cyPlotCityXYFromCity(CyCity* pCity, CyPlot* pPlot)
{
	return plotCityXY(pCity->getCity(), pPlot->getPlot());
}
CardinalDirectionTypes cyGetOppositeCardinalDirection(CardinalDirectionTypes eCardDirection)
{
	return getOppositeCardinalDirection(eCardDirection);
}
DirectionTypes cyCardinalDirectionToDirection(CardinalDirectionTypes eCard)
{
	return cardinalDirectionToDirection(eCard);
}
bool cyIsCardinalDirection(DirectionTypes eDirection)
{
	return isCardinalDirection(eDirection);
}
DirectionTypes cyEstimateDirection(int iDX, int iDY)
{
	return estimateDirection(iDX, iDY);
}
bool cyAtWar(int /*TeamTypes*/ eTeamA, int /*TeamTypes*/ eTeamB)
{
	return atWar((TeamTypes)eTeamA, (TeamTypes)eTeamB);
}
bool cyIsPotentialEnemy(int /*TeamTypes*/ eOurTeam, int /*TeamTypes*/ eTheirTeam)
{
	return isPotentialEnemy((TeamTypes)eOurTeam, (TeamTypes)eTheirTeam);
}
CyCity* cyGetCity(IDInfo city)
{
	return new CyCity(getCity(city));
}
CyUnit* cyGetUnit(IDInfo unit)
{
	return new CyUnit(getUnit(unit));
}
int /*ImprovementTypes*/ cyFinalImprovementUpgrade(int /*ImprovementTypes*/ eImprovement, int iCount)
{
	return finalImprovementUpgrade((ImprovementTypes) eImprovement, iCount);
}
int cyGetCombatOdds(CyUnit* pAttacker, CyUnit* pDefender)
{
	return getCombatOdds(pAttacker->getUnit(), pDefender->getUnit());
}

