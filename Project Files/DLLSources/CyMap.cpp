//
// Python wrapper class for CvMap
//

#include "CvGameCoreDLL.h"
#include "CyMap.h"
#include "CyPlot.h"
#include "CvMap.h"
#include "CyCity.h"
#include "CySelectionGroup.h"
#include "CyUnit.h"
#include "CyArea.h"
#include "CvGlobals.h"
#include "CvMapGenerator.h"
#include "CvInitCore.h"
#include "CyData.h"
#include "DesyncMonitor.h"

CvMap* CyMap::pointer(AssertCallerData data)
{
	FAssertWithCaller(data, CxDesyncMonitor::isAlwaysSync());
	FAssertWithCaller(data, m_pMap != NULL);
	return (CvMap*)m_pMap;
}

CyMap::CyMap() : m_pMap(&GC.getMap())
{
}

CyMap::CyMap(CvMap* pMap) : m_pMap(pMap)
{

}

void CyMap::erasePlots()
{
	if (m_pMap)
		pointer(CREATE_ASSERT_DATA)->erasePlots();
}

void CyMap::setRevealedPlots(int /*TeamTypes*/ eTeam, bool bNewValue, bool bTerrainOnly)
{
	if (m_pMap)
		pointer(CREATE_ASSERT_DATA)->setRevealedPlots((TeamTypes) eTeam, bNewValue, bTerrainOnly);
}

void CyMap::setAllPlotTypes(int /*PlotTypes*/ ePlotType)
{
	if (m_pMap)
		pointer(CREATE_ASSERT_DATA)->setAllPlotTypes((PlotTypes) ePlotType);
}

void CyMap::updateVisibility()
{
	if (m_pMap)
		pointer(CREATE_ASSERT_DATA)->updateVisibility();
}

CyPlot* CyMap::syncRandPlot(int iFlags, int iArea, int iMinUnitDistance, int iTimeout)
{
	return m_pMap ? new CyPlot(m_pMap->syncRandPlot(iFlags, iArea, iMinUnitDistance, iTimeout)) : NULL;
}

CyCity* CyMap::findCity(int iX, int iY, int /*PlayerTypes*/ eOwner, int /*TeamTypes*/ eTeam, bool bSameArea, bool bCoastalOnly, int /*TeamTypes*/ eTeamAtWarWith, int /*DirectionTypes*/ eDirection, CyCity* pSkipCity)
{
	return m_pMap ? new CyCity(pointer(CREATE_ASSERT_DATA)->findCity(iX, iY, (PlayerTypes)eOwner, (TeamTypes)eTeam, bSameArea, bCoastalOnly, ((TeamTypes)eTeamAtWarWith), (DirectionTypes)eDirection, pSkipCity->getCity())) : NULL;
}

CySelectionGroup* CyMap::findSelectionGroup(int iX, int iY, int /*PlayerTypes*/ eOwner, bool bReadyToSelect)
{
	return m_pMap ? new CySelectionGroup(pointer(CREATE_ASSERT_DATA)->findSelectionGroup(iX, iY, (PlayerTypes)eOwner, bReadyToSelect)) : NULL;
}

CyArea* CyMap::findBiggestArea(bool bWater)
{
	return m_pMap ? new CyArea(pointer(CREATE_ASSERT_DATA)->findBiggestArea(bWater)) : NULL;
}

int CyMap::getMapFractalFlags()
{
	return m_pMap ? pointer(CREATE_ASSERT_DATA)->getMapFractalFlags() : -1;
}

bool CyMap::findWater(CyPlot* pPlot, int iRange, bool bFreshWater)
{
	return m_pMap ? pointer(CREATE_ASSERT_DATA)->findWater(pPlot->getPlot(), iRange, bFreshWater) : false;
}

bool CyMap::isPlot(int iX, int iY)
{
	return m_pMap ? m_pMap->isPlotINLINE(iX, iY) : false;
}

int CyMap::numPlots()
{
	return m_pMap ? m_pMap->numPlotsINLINE() : -1;
}

int CyMap::plotNum(int iX, int iY)
{
	return m_pMap ? m_pMap->plotNumINLINE(iX, iY) : -1;
}

int CyMap::plotX(int iIndex)
{
	return m_pMap ? m_pMap->plotX(iIndex) : -1;
}

int CyMap::plotY(int iIndex)
{
	return m_pMap ? m_pMap->plotY(iIndex) : -1;
}

int CyMap::getGridWidth()
{
	return m_pMap->getGridWidthINLINE();
}

int CyMap::getGridHeight()
{
	return m_pMap->getGridHeightINLINE();
}

int CyMap::getLandPlots()
{
	return m_pMap ? m_pMap->getLandPlots() : -1;
}

int CyMap::getOwnedPlots()
{
	return m_pMap ? m_pMap->getOwnedPlots() : -1;
}

int CyMap::getTopLatitude()
{
	return m_pMap ? m_pMap->getTopLatitude() : -1;
}

int CyMap::getBottomLatitude()
{
	return m_pMap ? m_pMap->getBottomLatitude() : -1;
}

int CyMap::getNextRiverID()
{
	return m_pMap ? m_pMap->getNextRiverID() : -1;
}

void CyMap::incrementNextRiverID()
{
	if (m_pMap)
		pointer(CREATE_ASSERT_DATA)->incrementNextRiverID();
}

bool CyMap::isWrapX()
{
	return m_pMap ? m_pMap->isWrapXINLINE() : false;
}

bool CyMap::isWrapY()
{
	return m_pMap ? m_pMap->isWrapYINLINE() : false;
}

std::wstring CyMap::getMapScriptName()
{
	return GC.getInitCore().getMapScriptName().GetCString();
}

WorldSizeTypes CyMap::getWorldSize()
{
	return m_pMap ? m_pMap->getWorldSize() : NO_WORLDSIZE;
}

ClimateTypes CyMap::getClimate()
{
	return m_pMap ? m_pMap->getClimate() : NO_CLIMATE;
}

SeaLevelTypes CyMap::getSeaLevel()
{
	return m_pMap ? m_pMap->getSeaLevel() : NO_SEALEVEL;
}

int CyMap::getNumCustomMapOptions()
{
	return m_pMap ? m_pMap->getNumCustomMapOptions() : 0;
}

CustomMapOptionTypes CyMap::getCustomMapOption(int iOption)
{
	return m_pMap ? m_pMap->getCustomMapOption(iOption) : NO_CUSTOM_MAPOPTION;
}

int CyMap::getNumBonuses(int /* BonusTypes */ eIndex)
{
	return m_pMap ? m_pMap->getNumBonuses((BonusTypes)eIndex) : -1;
}

int CyMap::getNumBonusesOnLand(int /* BonusTypes */ eIndex)
{
	return m_pMap ? m_pMap->getNumBonusesOnLand((BonusTypes)eIndex) : -1;
}

void CyMap::updateWaterPlotTerrainTypes()
{
	if (m_pMap)
		pointer(CREATE_ASSERT_DATA)->updateWaterPlotTerrainTypes();
}

CyPlot* CyMap::plotByIndex(int iIndex)
{
	return m_pMap ? new CyPlot(m_pMap->plotByIndexINLINE(iIndex)) : NULL;
}

//
// static version
//
CyPlot* CyMap::sPlotByIndex(int iIndex)
{
	static CyPlot plot;
	if (m_pMap)
	{
		plot.setPlot(m_pMap->plotByIndexINLINE(iIndex));
		return &plot;
	}
	return NULL;
}

CyPlot* CyMap::plot(int iX, int iY)
{
	return new CyPlot(m_pMap->plotINLINE(iX, iY));
}

//
// static version
//
CyPlot* CyMap::sPlot(int iX, int iY)
{
	static CyPlot p;
	p.setPlot(m_pMap->plotINLINE(iX, iY));
	return &p;
}

CyPlot* CyMap::pointToPlot(float fX, float fY)
{
	return m_pMap ? new CyPlot(pointer(CREATE_ASSERT_DATA)->pointToPlot(fX, fY)) : NULL;
}

int CyMap::getIndexAfterLastArea()
{
	return m_pMap ? m_pMap->getIndexAfterLastArea() : -1;
}

int CyMap::getNumAreas()
{
	return m_pMap ? m_pMap->getNumAreas() : -1;
}

int CyMap::getNumLandAreas()
{
	return m_pMap ? m_pMap->getNumLandAreas() : -1;
}

CyArea* CyMap::getArea(int iID)
{
	return m_pMap ? new CyArea(pointer(CREATE_ASSERT_DATA)->getArea(iID)) : NULL;
}

void CyMap::recalculateAreas()
{
	if (m_pMap)
		pointer(CREATE_ASSERT_DATA)->recalculateAreas();
}

void CyMap::resetPathDistance()
{
	if (m_pMap)
		pointer(CREATE_ASSERT_DATA)->resetPathDistance();
}

int CyMap::calculatePathDistance(CyPlot* pSource, CyPlot* pDest)
{
	if (m_pMap)
		return pointer(CREATE_ASSERT_DATA)->calculatePathDistance(pSource->getPlot(), pDest->getPlot());
	return -1;
}

void CyMap::rebuild(int iGridW, int iGridH, int iTopLatitude, int iBottomLatitude, bool bWrapX, bool bWrapY, WorldSizeTypes eWorldSize, ClimateTypes eClimate, SeaLevelTypes eSeaLevel, int iNumCustomMapOptions, CustomMapOptionTypes * aeCustomMapOptions)
{
	if (m_pMap)
	{
		pointer(CREATE_ASSERT_DATA)->rebuild(iGridW, iGridH, iTopLatitude, iBottomLatitude, bWrapX, bWrapY, eWorldSize, eClimate, eSeaLevel, iNumCustomMapOptions, aeCustomMapOptions);
	}
}

void CyMap::regenerateGameElements()
{
	if (m_pMap)
	{
		CvMapGenerator* pMapGen = &CvMapGenerator::GetInstance();
		pMapGen->eraseRivers();
		pMapGen->eraseFeatures();
		pMapGen->eraseBonuses();
		pMapGen->eraseGoodies();
		pMapGen->eraseEurope();
		pMapGen->addGameElements();
	}
}

void CyMap::updateFog()
{
	if (m_pMap)
	{
		pointer(CREATE_ASSERT_DATA)->updateFog();
	}
}

void CyMap::updateMinimapColor()
{
	if (m_pMap)
	{
		pointer(CREATE_ASSERT_DATA)->updateMinimapColor();
	}
}

void CyMap::updateMinOriginalStartDist(CyArea* pArea)
{
	if (m_pMap)
	{
		pointer(CREATE_ASSERT_DATA)->updateMinOriginalStartDist(pArea->getArea());
	}
}

// Super Forts begin *canal* *choke*
void CyMap::calculateCanalAndChokePoints()
{
	if(m_pMap)
	{
		pointer(CREATE_ASSERT_DATA)->calculateCanalAndChokePoints();
	}
}
// Super Forts end

void closeWBcacheSetup();
// autodetect lakes - start
void CyMap::closeWB()
{
	closeWBcacheSetup();
}
// autodetect lakes - end

int CyMap::getCityCatchmentRadius() const
{
	if (m_pMap)
	{
		return m_pMap->getCityCatchmentRadius();
	}
	return 0;
}

void CyMap::setCityCatchmentRadiusMapMaker(int iSetting)
{
	if (m_pMap)
	{
		pointer(CREATE_ASSERT_DATA)->setCityCatchmentRadius(iSetting + 1);
	}
}

void CyMap::setCityCatchmentRadiusNoMapMaker(int iSetting)
{
	if (m_pMap)
	{
		pointer(CREATE_ASSERT_DATA)->setCityCatchmentRadius(iSetting);
	}
}

CyInfoArray* CyMap::getTerrainCount(bool bHills) const
{
	EnumMap<TerrainTypes, int> em;

	if (m_pMap)
	{
		for (int iPlot = 0; iPlot < m_pMap->numPlotsINLINE(); iPlot++)
		{
			const CvPlot* pPlot = m_pMap->plotByIndexINLINE(iPlot);
			if (pPlot != NULL)
			{
				TerrainTypes eTerrain = pPlot->getTerrainType();
				if (eTerrain != NO_TERRAIN)
				{
					if ((bHills && pPlot->isHills()) || (!bHills && !pPlot->isHills() && !pPlot->isPeak()))
					em.add(eTerrain, 1);
				}
				if (bHills)
				{
					if (pPlot->isHills())
					{
						em.add(TERRAIN_HILL, 1);
					}
					if (pPlot->isPeak())
					{
						em.add(TERRAIN_PEAK, 1);
					}
				}
			}
		}
	}

	return new CyInfoArray(em);
}
