//
//	FILE:	 CvMap.cpp
//	AUTHOR:  Soren Johnson
//	PURPOSE: Game map class
//-----------------------------------------------------------------------------
//	Copyright (c) 2004 Firaxis Games, Inc. All rights reserved.
//-----------------------------------------------------------------------------
//


#include "CvGameCoreDLL.h"
#include "CvMap.h"
#include "CvCity.h"
#include "CvGlobals.h"
#include "CvGameAI.h"
#include "CvPlayerAI.h"
#include "CvRandom.h"
#include "CvGameCoreUtils.h"
#include "CvFractal.h"
#include "CvPlot.h"
#include "CvGameCoreUtils.h"
#include "CvMap.h"
#include "CvMapGenerator.h"
#include "KmodPathFinder.h"
#include "FAStarNode.h"
#include "CvInitCore.h"
#include "CvInfos.h"
#include "FProfiler.h"
#include <queue>
#include <hash_set>

#include "CvDLLEngineIFaceBase.h"
#include "CvDLLIniParserIFaceBase.h"
#include "CvDLLFAStarIFaceBase.h"
#include "CvDLLFAStarIFaceBase.h"
#include "CvDLLPythonIFaceBase.h"

#include "CvSavegame.h"

// WTP - Plotregion - Nightinggale - start

// a plot region is a collection of connected plots, which all have something in common
// what they have in common is that they all have what is specified by the EnumMap given to the constructor
// written with terrains in mind, it can also handle features, improvements or what else CvPlot::getVariable can support
//
// PlotRegionMap is a collection of plot regions
// It will scan the entire map and create all the regions it can
// note that due to region merging, some might end up empty

PlotRegion::PlotRegion()
	: m_bEurope(false)
{}

bool PlotRegion::isEurope() const
{
	return m_bEurope;
}

int PlotRegion::getNumPlots() const
{
	return (int)m_aiPlots.size();
}

CvPlot* PlotRegion::getPlot(int i) const
{
	FAssert(i >= 0 && i < getNumPlots());
	return GC.getMap().plotByIndex(m_aiPlots[i]);
}

bool PlotRegion::isTerrainAdjacent(const EnumMap<TerrainTypes, bool> em) const
{
	const int iNumPlots = getNumPlots();
	for (int iPlot = 0; iPlot < iNumPlots; ++iPlot)
	{
		const CvPlot* pRegionPlot = getPlot(iPlot);
		for (DirectionTypes eDirection = FIRST_DIRECTION; eDirection < NUM_DIRECTION_TYPES; ++eDirection)
		{
			const CvPlot* pAdjacentPlot = plotDirection(pRegionPlot->getX_INLINE(), pRegionPlot->getY_INLINE(), eDirection);
			if (pAdjacentPlot != NULL && pAdjacentPlot->getTerrainType() != NO_TERRAIN)
			{
				if (em.get(pAdjacentPlot->getTerrainType()))
				{
					return true;
				}
			}
		}
	}
	return false;
}

void PlotRegion::add(int iPlot, std::vector<PlotRegion*>& plotRegions)
{
	if (plotRegions[iPlot] != NULL)
	{
		if (plotRegions[iPlot] != this)
		{
			plotRegions[iPlot]->merge(*this, plotRegions);
		}
	}
	else
	{
		plotRegions[iPlot] = this;
		m_aiPlots.push_back(iPlot);
	}
}

void PlotRegion::merge(PlotRegion& rhs, std::vector<PlotRegion*>& plotRegions)
{
	for (unsigned int i = 0; i < rhs.m_aiPlots.size(); ++i)
	{
		int iPlot = rhs.m_aiPlots[i];
		m_aiPlots.push_back(iPlot);
		plotRegions[iPlot] = this;
	}
	rhs.m_aiPlots.clear();
	if (rhs.m_bEurope)
	{
		m_bEurope = true;
	}
}

template<typename T>
PlotRegionMap::PlotRegionMap(const EnumMap<T, bool>& em)
{
	CvMap& kMap = GC.getMap();
	const int iNumPlots = kMap.numPlots();

	std::vector<PlotRegion*> plotRegions;
	plotRegions.assign(iNumPlots, NULL);

	// reserve memory for the highest amount of plot regions possible (in theory)
	// most likely overkill, but by having the memory reserved, we can be sure no new allocations will take place
	// whenever a new allocation takes place, all pointers to the vector elements will become invalid
	m_aRegions.reserve((iNumPlots/4)+1);


	for (int i = 0; i < iNumPlots; ++i)
	{
		CvPlot* pPlot = kMap.plotByIndex(i);
		const T eVar = pPlot->getVariable((T)0);

		if (eVar != (T)(-1) && em.get(eVar))
		{
			// rather than looping, specify the 4 directions of already looped plots
			// the rest of the directions aren't interesting as they haven't been looped through yet
			handlePlot(pPlot, -1, -1, plotRegions);
			handlePlot(pPlot, 0, -1, plotRegions);
			handlePlot(pPlot, 1, -1, plotRegions);
			handlePlot(pPlot, -1, 0, plotRegions);

			if (plotRegions[i] == NULL)
			{
				// create a new region
				m_aRegions.push_back(PlotRegion());
				//plotRegions[i] = &m_aRegions[m_aRegions.size() - 1];
				m_aRegions[m_aRegions.size() - 1].add(i, plotRegions);
			}
			if (pPlot->isEurope())
			{
				plotRegions[i]->m_bEurope = true;
			}
		}
	}
}

int PlotRegionMap::getNumRegions() const
{
	return (int)m_aRegions.size();
}
const PlotRegion& PlotRegionMap::getRegion(int iIndex) const
{
	FAssert(iIndex >= 0 && iIndex < getNumRegions());
	return m_aRegions[iIndex];
}

void PlotRegionMap::handlePlot(CvPlot* pPlot, int iX, int iY, std::vector<PlotRegion*>& plotRegions)
{
	CvPlot* pOtherPlot = GC.getMap().plotINLINE(pPlot->getX_INLINE() + iX, pPlot->getY_INLINE() + iY);
	if (pOtherPlot != NULL)
	{
		int iIndex = pOtherPlot->getIndex();
		PlotRegion* pRegion = plotRegions[iIndex];
		if (pRegion != NULL)
		{
			pRegion->add(pPlot->getIndex(), plotRegions);
		}
	}
};
// WTP - Plotregion - Nightinggale - end


// Public Functions...

CvMap::CvMap()
{
	CvMapInitData defaultMapData;

	m_pMapPlots = NULL;

	reset(&defaultMapData);
}


CvMap::~CvMap()
{
	uninit();
}

// FUNCTION: init()
// Initializes the map.
// Parameters:
//	pInitInfo					- Optional init structure (used for WB load)
// Returns:
//	nothing.
void CvMap::init(CvMapInitData* pInitInfo/*=NULL*/)
{
	PROFILE_FUNC();
	gDLL->logMemState( CvString::format("CvMap::init begin - world size=%s, climate=%s, sealevel=%s, num custom options=%6",
		GC.getWorldInfo(GC.getInitCore().getWorldSize()).getDescription(),
		GC.getClimateInfo(GC.getInitCore().getClimate()).getDescription(),
		GC.getSeaLevelInfo(GC.getInitCore().getSeaLevel()).getDescription(),
		GC.getInitCore().getNumCustomMapOptions()).c_str() );

	gDLL->getPythonIFace()->callFunction(gDLL->getPythonIFace()->getMapScriptModule(), "beforeInit");

	//--------------------------------
	// Init saved data
	reset(pInitInfo);

	//--------------------------------
	// Init containers
	m_areas.init();

	//--------------------------------
	// Init non-saved data
	setup();

	//--------------------------------
	// Init other game data
	gDLL->logMemState("CvMap before init plots");
	m_pMapPlots = new CvPlot[numPlotsINLINE()];
	for (int iX = 0; iX < getGridWidthINLINE(); iX++)
	{
		gDLL->callUpdater();
		for (int iY = 0; iY < getGridHeightINLINE(); iY++)
		{
			CvPlot& kPlot = *plotSoren(iX, iY);
			kPlot.init(iX, iY);
		}
	}
	calculateAreas();
	gDLL->logMemState("CvMap after init plots");
}


void CvMap::uninit()
{
	SAFE_DELETE_ARRAY(m_pMapPlots);

	m_areas.uninit();

	FAStar* coastalRouteFinder = &GC.getCoastalRouteFinder();
	gDLL->getFAStarIFace()->destroy(coastalRouteFinder);
}

// FUNCTION: reset()
// Initializes data members that are serialized.
void CvMap::reset(CvMapInitData* pInitInfo)
{
	// reset plot max visual range
	resetVisibilityCache();

	// reset city catchment radius. It should be 1 plot unless explicitly requested otherwise.
	setCityCatchmentRadius(0);

	//--------------------------------
	// Uninit class
	uninit();

	// apply default values for variables in savegame
	resetSavedData();

	//
	// set grid size
	// initially set in terrain cell units
	//
	m_iGridWidth = (GC.getInitCore().getWorldSize() != NO_WORLDSIZE) ? GC.getWorldInfo(GC.getInitCore().getWorldSize()).getGridWidth (): 0;	//todotw:tcells wide
	m_iGridHeight = (GC.getInitCore().getWorldSize() != NO_WORLDSIZE) ? GC.getWorldInfo(GC.getInitCore().getWorldSize()).getGridHeight (): 0;

	// allow grid size override
	if (pInitInfo)
	{
		m_iGridWidth	= pInitInfo->m_iGridW;
		m_iGridHeight	= pInitInfo->m_iGridH;
	}
	else
	{
		// check map script for grid size override
		gDLL->getPythonIFace()->pythonGetGridSize(GC.getInitCore().getWorldSize(), &m_iGridWidth, &m_iGridHeight);
	}

	m_iLandPlots = 0;
	m_iOwnedPlots = 0;

	if (pInitInfo)
	{
		m_iTopLatitude = pInitInfo->m_iTopLatitude;
		m_iBottomLatitude = pInitInfo->m_iBottomLatitude;
	}
	else
	{
		// Check map script for latitude override (map script beats ini file)
		gDLL->getPythonIFace()->pythonGetLatitudes(&m_iTopLatitude, &m_iBottomLatitude);
	}

	m_iTopLatitude = std::min(m_iTopLatitude, 90);
	m_iTopLatitude = std::max(m_iTopLatitude, -90);
	m_iBottomLatitude = std::min(m_iBottomLatitude, 90);
	m_iBottomLatitude = std::max(m_iBottomLatitude, -90);

	m_iNextRiverID = 0;

	//
	// set wrapping
	//
	m_bWrapX = true;
	m_bWrapY = false;
	if (pInitInfo)
	{
		m_bWrapX = pInitInfo->m_bWrapX;
		m_bWrapY = pInitInfo->m_bWrapY;
	}
	else
	{
		// Check map script for wrap override (map script beats ini file)
		gDLL->getPythonIFace()->pythonGetWrapXY(&m_bWrapX, &m_bWrapY);
	}

	m_areas.removeAll();
}


// FUNCTION: setup()
// Initializes all data that is not serialized but needs to be initialized after loading.
void CvMap::setup()
{
	PROFILE_FUNC();

	KmodPathFinder::InitHeuristicWeights(); // K-Mod
	CvDLLFAStarIFaceBase& kAStar = *gDLL->getFAStarIFace(); // advc

	kAStar.Initialize(&GC.getPathFinder(), getGridWidthINLINE(), getGridHeightINLINE(), isWrapXINLINE(), isWrapYINLINE(), pathDestValid, pathHeuristic, pathCost, pathValid, pathAdd, NULL, NULL);
	kAStar.Initialize(&GC.getInterfacePathFinder(), getGridWidthINLINE(), getGridHeightINLINE(), isWrapXINLINE(), isWrapYINLINE(), pathDestValid, pathHeuristic, pathCost, pathValid, pathAdd, NULL, NULL);
	kAStar.Initialize(&GC.getStepFinder(), getGridWidthINLINE(), getGridHeightINLINE(), isWrapXINLINE(), isWrapYINLINE(), stepDestValid, stepHeuristic, stepCost, stepValid, stepAdd, NULL, NULL);
	kAStar.Initialize(&GC.getRouteFinder(), getGridWidthINLINE(), getGridHeightINLINE(), isWrapXINLINE(), isWrapYINLINE(), NULL, NULL, NULL, routeValid, NULL, NULL, NULL);
	kAStar.Initialize(&GC.getBorderFinder(), getGridWidthINLINE(), getGridHeightINLINE(), isWrapXINLINE(), isWrapYINLINE(), NULL, NULL, NULL, borderValid, NULL, NULL, NULL);
	kAStar.Initialize(&GC.getAreaFinder(), getGridWidthINLINE(), getGridHeightINLINE(), isWrapXINLINE(), isWrapYINLINE(), NULL, NULL, NULL, areaValid, NULL, joinArea, NULL);
}


//////////////////////////////////////
// graphical only setup
//////////////////////////////////////
void CvMap::setupGraphical()
{
	if (!GC.IsGraphicsInitialized())
		return;

	PROFILE_FUNC();

	if (m_pMapPlots != NULL)
	{
		gDLL->getEngineIFace()->RebuildAllTileArt();
		for (int iI = 0; iI < numPlotsINLINE(); iI++)
		{
			gDLL->callUpdater();	// allow windows msgs to update
			plotByIndexINLINE(iI)->setupGraphical();
		}
	}
}


void CvMap::erasePlots()
{
	for (int iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->erase();
	}
}


void CvMap::setRevealedPlots(TeamTypes eTeam, bool bNewValue, bool bTerrainOnly)
{
	PROFILE_FUNC();

	for (int iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->setRevealed(eTeam, bNewValue, bTerrainOnly, NO_TEAM);
	}
}


void CvMap::setAllPlotTypes(PlotTypes ePlotType)
{
	//float startTime = (float) timeGetTime();

	for(int i=0;i<numPlotsINLINE();i++)
	{
		plotByIndexINLINE(i)->setPlotType(ePlotType, false, false);
	}

	recalculateAreas();

	//rebuild landscape
	gDLL->getEngineIFace()->RebuildAllPlots();

	//mark minimap as dirty
	gDLL->getEngineIFace()->SetDirty(MinimapTexture_DIRTY_BIT, true);
	gDLL->getEngineIFace()->SetDirty(GlobeTexture_DIRTY_BIT, true);

	//float endTime = (float) timeGetTime();
	//OutputDebugString(CvString::format("[Jason] setAllPlotTypes: %f\n", endTime - startTime).c_str());
}


// XXX generalize these funcs? (macro?)
void CvMap::doTurn()
{
	PROFILE_FUNC();

	for (int iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->doTurn();
	}
}


void CvMap::updateFlagSymbols()
{
	PROFILE_FUNC();

	for (int iI = 0; iI < numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = plotByIndexINLINE(iI);

		if (pLoopPlot->isFlagDirty())
		{
			pLoopPlot->updateFlagSymbol();
			pLoopPlot->setFlagDirty(false);
		}
	}
}


void CvMap::updateFog()
{
	for (int iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->updateFog();
	}
}


void CvMap::updateVisibility()
{
	for (int iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->updateVisibility();
	}
}


void CvMap::updateSymbolVisibility()
{
	for (int iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->updateSymbolVisibility();
	}
}


void CvMap::updateSymbols()
{
	for (int iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->updateSymbols();
	}
}


void CvMap::updateMinimapColor()
{
	for (int iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->updateMinimapColor();
	}
}


void CvMap::updateSight(bool bIncrement)
{
	for (int iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->updateSight(bIncrement);
	}
}

void CvMap::updateCenterUnit()
{
	PROFILE_FUNC();

	for (int iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->updateCenterUnit();
	}
}


void CvMap::updateWorkingCity()
{
	for (int iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->updateWorkingCity();
	}
}


void CvMap::updateMinOriginalStartDist(CvArea* pArea)
{
	PROFILE_FUNC();

	CvPlot* pStartingPlot;
	CvPlot* pLoopPlot;
	int iDist;
	int iI, iJ;

	for (iI = 0; iI < numPlotsINLINE(); iI++)
	{
		pLoopPlot = plotByIndexINLINE(iI);

		if (pLoopPlot->area() == pArea)
		{
			pLoopPlot->setMinOriginalStartDist(-1);
		}
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		pStartingPlot = GET_PLAYER((PlayerTypes)iI).getStartingPlot();

		if (pStartingPlot != NULL)
		{
			if (pStartingPlot->area() == pArea)
			{
				for (iJ = 0; iJ < numPlotsINLINE(); iJ++)
				{
					pLoopPlot = plotByIndexINLINE(iJ);

					if (pLoopPlot->area() == pArea)
					{

						//iDist = GC.getMapINLINE().calculatePathDistance(pStartingPlot, pLoopPlot);
						iDist = stepDistance(pStartingPlot->getX_INLINE(), pStartingPlot->getY_INLINE(), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE());

						if (iDist != -1)
						{
						    //int iCrowDistance = plotDistance(pStartingPlot->getX_INLINE(), pStartingPlot->getY_INLINE(), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE());
						    //iDist = std::min(iDist,  iCrowDistance * 2);
							if ((pLoopPlot->getMinOriginalStartDist() == -1) || (iDist < pLoopPlot->getMinOriginalStartDist()))
							{
								pLoopPlot->setMinOriginalStartDist(iDist);
							}
						}
					}
				}
			}
		}
	}
}


void CvMap::updateYield()
{
	for (int iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->updateYield(true);
	}
}

void CvMap::updateCulture()
{
	for (int iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->updateCulture(true);
	}
}

void CvMap::verifyUnitValidPlot()
{
	for (int iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->verifyUnitValidPlot();
	}
}

CvPlot* CvMap::syncRandPlot(int iFlags, int iArea, int iMinUnitDistance, int iTimeout) const
{
	CvPlot* pPlot = NULL;
	int iCount = 0;

	while (iCount < iTimeout)
	{
		CvPlot* const pTestPlot = plotSoren(GC.getGameINLINE().getSorenRandNum(getGridWidthINLINE(), "Rand Plot Width"), GC.getGameINLINE().getSorenRandNum(getGridHeightINLINE(), "Rand Plot Height"));

		FAssertMsg(pTestPlot != NULL, "TestPlot is not assigned a valid value");

		if ((iArea == -1) || (pTestPlot->getArea() == iArea))
		{
			bool bValid = true;

			if (bValid)
			{
				if (iMinUnitDistance != -1)
				{
					for (int iDX = -(iMinUnitDistance); iDX <= iMinUnitDistance; iDX++)
					{
						for (int iDY = -(iMinUnitDistance); iDY <= iMinUnitDistance; iDY++)
						{
							CvPlot* pLoopPlot = plotXY(pTestPlot->getX_INLINE(), pTestPlot->getY_INLINE(), iDX, iDY);

							if (pLoopPlot != NULL)
							{
								if (pLoopPlot->isUnit())
								{
									bValid = false;
								}
							}
						}
					}
				}
			}

			if (bValid)
			{
				if (iFlags & RANDPLOT_LAND)
				{
					if (pTestPlot->isWater())
					{
						bValid = false;
					}
				}
			}

			if (bValid)
			{
				if (iFlags & RANDPLOT_UNOWNED)
				{
					if (pTestPlot->isOwned())
					{
						bValid = false;
					}
				}
			}

			if (bValid)
			{
				if (iFlags & RANDPLOT_ADJACENT_UNOWNED)
				{
					if (pTestPlot->isAdjacentOwned())
					{
						bValid = false;
					}
				}
			}

			if (bValid)
			{
				if (iFlags & RANDPLOT_ADJACENT_LAND)
				{
					if (!(pTestPlot->isAdjacentToLand()))
					{
						bValid = false;
					}
				}
			}

			if (bValid)
			{
				if (iFlags & RANDPLOT_PASSIBLE)
				{
					if (pTestPlot->isImpassable())
					{
						bValid = false;
					}
				}
			}

			if (bValid)
			{
				if (iFlags & RANDPLOT_NOT_VISIBLE_TO_CIV)
				{
					if (pTestPlot->isVisibleToCivTeam())
					{
						bValid = false;
					}
				}
			}

			if (bValid)
			{
				if (iFlags & RANDPLOT_NOT_CITY)
				{
					if (pTestPlot->isCity())
					{
						bValid = false;
					}
				}
			}

			if (bValid)
			{
				pPlot = pTestPlot;
				break;
			}
		}

		iCount++;
	}

	return pPlot;
}


CvCity* CvMap::findCity(int iX, int iY, PlayerTypes eOwner, TeamTypes eTeam, bool bSameArea, bool bCoastalOnly, TeamTypes eTeamAtWarWith, DirectionTypes eDirection, CvCity* pSkipCity)
{
	return findCity(Coordinates(iX, iY), eOwner, eTeam, bSameArea, bCoastalOnly, eTeamAtWarWith, eDirection, pSkipCity);
}


CvCity* CvMap::findCity(Coordinates coord, PlayerTypes eOwner, TeamTypes eTeam, bool bSameArea, bool bCoastalOnly, TeamTypes eTeamAtWarWith, DirectionTypes eDirection, CvCity* pSkipCity)
{
	int iBestValue = MAX_INT;
	CvCity* pBestCity = NULL;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if ((eOwner == NO_PLAYER) || (iI == eOwner))
			{
				if ((eTeam == NO_TEAM) || (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam))
				{
					int iLoop;
					for (CvCity* pLoopCity = GET_PLAYER((PlayerTypes)iI).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iI).nextCity(&iLoop))
					{
						if (!bSameArea || (pLoopCity->area() == coord.plot()->area()) || (bCoastalOnly && (pLoopCity->waterArea() == coord.plot()->area())))
						{
							if (!bCoastalOnly || pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
							{
								if ((eTeamAtWarWith == NO_TEAM) || atWar(GET_PLAYER((PlayerTypes)iI).getTeam(), eTeamAtWarWith))
								{
									if ((eDirection == NO_DIRECTION) || (estimateDirection(dxWrap(pLoopCity->getX_INLINE() - coord.x()), dyWrap(pLoopCity->getY_INLINE() - coord.y())) == eDirection))
									{
										if ((pSkipCity == NULL) || (pLoopCity != pSkipCity))
										{
											int iValue = plotDistance(coord, pLoopCity->coord());

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
					}
				}
			}
		}
	}

	return pBestCity;
}


CvSelectionGroup* CvMap::findSelectionGroup(int iX, int iY, PlayerTypes eOwner, bool bReadyToSelect)
{
	int iBestValue = MAX_INT;
	CvSelectionGroup* pBestSelectionGroup = NULL;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if ((eOwner == NO_PLAYER) || (iI == eOwner))
			{
				int iLoop;
				for(CvSelectionGroup* pLoopSelectionGroup = GET_PLAYER((PlayerTypes)iI).firstSelectionGroup(&iLoop); pLoopSelectionGroup != NULL; pLoopSelectionGroup = GET_PLAYER((PlayerTypes)iI).nextSelectionGroup(&iLoop))
				{
					if (!bReadyToSelect || pLoopSelectionGroup->readyToSelect())
					{
						int iValue = plotDistance(iX, iY, pLoopSelectionGroup->getX(), pLoopSelectionGroup->getY());

						if (iValue < iBestValue)
						{
							iBestValue = iValue;
							pBestSelectionGroup = pLoopSelectionGroup;
						}
					}
				}
			}
		}
	}

	return pBestSelectionGroup;
}


CvArea* CvMap::findBiggestArea(bool bWater)
{
	int iBestValue = 0;
	CvArea* pBestArea = NULL;

	int iLoop;
	for (CvArea* pLoopArea = firstArea(&iLoop); pLoopArea != NULL; pLoopArea = nextArea(&iLoop))
	{
		if (pLoopArea->isWater() == bWater)
		{
			int iValue = pLoopArea->getNumTiles();

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestArea = pLoopArea;
			}
		}
	}

	return pBestArea;
}


int CvMap::getMapFractalFlags()
{
	int wrapX = 0;
	if (isWrapXINLINE())
	{
		wrapX = (int)CvFractal::FRAC_WRAP_X;
	}

	int wrapY = 0;
	if (isWrapYINLINE())
	{
		wrapY = (int)CvFractal::FRAC_WRAP_Y;
	}

	return (wrapX | wrapY);
}


//"Check plots for wetlands or seaWater.  Returns true if found"
bool CvMap::findWater(CvPlot* pPlot, int iRange, bool bFreshWater)
{
	PROFILE_FUNC();

	for (int iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for (int iDY = -(iRange); iDY <= iRange; iDY++)
		{
			CvPlot* pLoopPlot	= plotXY(pPlot->getX_INLINE(), pPlot->getY_INLINE(), iDX, iDY);

			if (pLoopPlot != NULL)
			{
				if (bFreshWater)
				{
					if (pLoopPlot->isRiver())
					{
						return true;
					}
				}
				else
				{
					if (pLoopPlot->isWater())
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}


bool CvMap::isPlot(int iX, int iY) const
{
	//return isPlotINLINE(iX, iY);
	Coordinates coord(iX, iY);
	return isPlotINLINE(coord);
}


int CvMap::numPlots() const
{
	return numPlotsINLINE();
}


int CvMap::plotNum(int iX, int iY) const
{
	return plotNumINLINE(iX, iY);
}


int CvMap::plotX(int iIndex) const
{
	return (iIndex % getGridWidthINLINE());
}


int CvMap::plotY(int iIndex) const
{
	return (iIndex / getGridWidthINLINE());
}


int CvMap::pointXToPlotX(float fX)
{
	float fWidth, fHeight;
	gDLL->getEngineIFace()->GetLandscapeGameDimensions(fWidth, fHeight);
	return (int)(((fX + (fWidth/2.0f)) / fWidth) * getGridWidthINLINE());
}


float CvMap::plotXToPointX(int iX)
{
	float fWidth, fHeight;
	gDLL->getEngineIFace()->GetLandscapeGameDimensions(fWidth, fHeight);
	return ((iX * fWidth) / ((float)getGridWidthINLINE())) - (fWidth / 2.0f) + (GC.getPLOT_SIZE() / 2.0f);
}


int CvMap::pointYToPlotY(float fY)
{
	float fWidth, fHeight;
	gDLL->getEngineIFace()->GetLandscapeGameDimensions(fWidth, fHeight);
	return (int)(((fY + (fHeight/2.0f)) / fHeight) * getGridHeightINLINE());
}


float CvMap::plotYToPointY(int iY)
{
	float fWidth, fHeight;
	gDLL->getEngineIFace()->GetLandscapeGameDimensions(fWidth, fHeight);
	return ((iY * fHeight) / ((float)getGridHeightINLINE())) - (fHeight / 2.0f) + (GC.getPLOT_SIZE() / 2.0f);
}


float CvMap::getWidthCoords()
{
	return (GC.getPLOT_SIZE() * ((float)getGridWidthINLINE()));
}


float CvMap::getHeightCoords()
{
	return (GC.getPLOT_SIZE() * ((float)getGridHeightINLINE()));
}


int CvMap::maxPlotDistance()
{
	return std::max(1, plotDistance(0, 0, ((isWrapXINLINE()) ? (getGridWidthINLINE() / 2) : (getGridWidthINLINE() - 1)), ((isWrapYINLINE()) ? (getGridHeightINLINE() / 2) : (getGridHeightINLINE() - 1))));
}


int CvMap::maxStepDistance()
{
	return std::max(1, stepDistance(0, 0, ((isWrapXINLINE()) ? (getGridWidthINLINE() / 2) : (getGridWidthINLINE() - 1)), ((isWrapYINLINE()) ? (getGridHeightINLINE() / 2) : (getGridHeightINLINE() - 1))));
}


int CvMap::getGridWidth() const
{
	return getGridWidthINLINE();
}


int CvMap::getGridHeight() const
{
	return getGridHeightINLINE();
}


int CvMap::getLandPlots()
{
	return m_iLandPlots;
}


void CvMap::changeLandPlots(int iChange)
{
	m_iLandPlots = (m_iLandPlots + iChange);
	FAssert(getLandPlots() >= 0);
}


int CvMap::getOwnedPlots()
{
	return m_iOwnedPlots;
}


void CvMap::changeOwnedPlots(int iChange)
{
	m_iOwnedPlots = (m_iOwnedPlots + iChange);
	FAssert(getOwnedPlots() >= 0);
}


int CvMap::getTopLatitude()
{
	return m_iTopLatitude;
}


int CvMap::getBottomLatitude()
{
	return m_iBottomLatitude;
}


int CvMap::getNextRiverID()
{
	return m_iNextRiverID;
}


void CvMap::incrementNextRiverID()
{
	m_iNextRiverID++;
}


bool CvMap::isWrapX()
{
	return isWrapXINLINE();
}


bool CvMap::isWrapY()
{
	return isWrapYINLINE();
}

bool CvMap::isWrap()
{
	return isWrapINLINE();
}

WorldSizeTypes CvMap::getWorldSize()
{
	return GC.getInitCore().getWorldSize();
}


ClimateTypes CvMap::getClimate()
{
	return GC.getInitCore().getClimate();
}


SeaLevelTypes CvMap::getSeaLevel()
{
	return GC.getInitCore().getSeaLevel();
}



int CvMap::getNumCustomMapOptions()
{
	return GC.getInitCore().getNumCustomMapOptions();
}


CustomMapOptionTypes CvMap::getCustomMapOption(int iOption)
{
	return GC.getInitCore().getCustomMapOption(iOption);
}


int CvMap::getNumBonuses(BonusTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_ja_NumBonuses.get(eIndex);
}


void CvMap::changeNumBonuses(BonusTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_ja_NumBonuses.add(iChange, eIndex);
	FAssert(getNumBonuses(eIndex) >= 0);
}


int CvMap::getNumBonusesOnLand(BonusTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_ja_NumBonusesOnLand.get(eIndex);
}


void CvMap::changeNumBonusesOnLand(BonusTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_ja_NumBonusesOnLand.add(iChange, eIndex);
	FAssert(getNumBonusesOnLand(eIndex) >= 0);
}


CvPlot* CvMap::plotByIndex(int iIndex) const
{
	return plotByIndexINLINE(iIndex);
}


CvPlot* CvMap::plot(int iX, int iY) const
{
	return plotINLINE(iX, iY);
}


CvPlot* CvMap::pointToPlot(float fX, float fY)
{
	return plotINLINE(pointXToPlotX(fX), pointYToPlotY(fY));
}


int CvMap::getIndexAfterLastArea()
{
	return m_areas.getIndexAfterLast();
}


int CvMap::getNumAreas()
{
	return m_areas.getCount();
}


int CvMap::getNumLandAreas()
{
	CvArea* pLoopArea;
	int iNumLandAreas;
	int iLoop;

	iNumLandAreas = 0;

	for(pLoopArea = GC.getMap().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop))
	{
		if (!(pLoopArea->isWater()))
		{
			iNumLandAreas++;
		}
	}

	return iNumLandAreas;
}


CvArea* CvMap::getArea(int iID)
{
	return m_areas.getAt(iID);
}


CvArea* CvMap::addArea()
{
	return m_areas.add();
}


void CvMap::deleteArea(int iID)
{
	m_areas.removeAt(iID);
}


CvArea* CvMap::firstArea(int *pIterIdx, bool bRev)
{
	return !bRev ? m_areas.beginIter(pIterIdx) : m_areas.endIter(pIterIdx);
}


CvArea* CvMap::nextArea(int *pIterIdx, bool bRev)
{
	return !bRev ? m_areas.nextIter(pIterIdx) : m_areas.prevIter(pIterIdx);
}


void CvMap::recalculateAreas()
{
	PROFILE_FUNC();

	int iI;

	for (iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->setArea(FFreeList::INVALID_INDEX);
	}

	m_areas.removeAll();

	calculateAreas();
}


void CvMap::resetPathDistance()
{
	gDLL->getFAStarIFace()->ForceReset(&GC.getStepFinder());
}


// Super Forts begin *canal* *choke*
int CvMap::calculatePathDistance(CvPlot *pSource, CvPlot *pDest, CvPlot *pInvalidPlot)
//int CvMap::calculatePathDistance(CvPlot *pSource, CvPlot *pDest) -- original
// Super Forts end
{
	FAStarNode* pNode;

	if (pSource == NULL || pDest == NULL)
	{
		return -1;
	}

	// Super Forts begin *canal* *choke*
	// 1 must be added because 0 is already being used as the default value for iInfo in GeneratePath()
	int iInvalidPlot = (pInvalidPlot == NULL) ? 0 : GC.getMap().plotNum(pInvalidPlot->getX_INLINE(), pInvalidPlot->getY_INLINE()) + 1;

	if (gDLL->getFAStarIFace()->GeneratePath(&GC.getStepFinder(), pSource->getX_INLINE(), pSource->getY_INLINE(), pDest->getX_INLINE(), pDest->getY_INLINE(), false, iInvalidPlot, true))
//  if (gDLL->getFAStarIFace()->GeneratePath(&GC.getStepFinder(), pSource->getX_INLINE(), pSource->getY_INLINE(), pDest->getX_INLINE(), pDest->getY_INLINE(), false, 0, true)) -- original
	// Super Forts end
	{
		pNode = gDLL->getFAStarIFace()->GetLastNode(&GC.getStepFinder());

		if (pNode != NULL)
		{
			return pNode->m_iData1;
		}
	}

	return -1; // no passable path exists
}

// Super Forts begin *canal* *choke*
void CvMap::calculateCanalAndChokePoints()
{
	int iI;
	for(iI = 0; iI < numPlotsINLINE(); iI++)
	{
		plotByIndexINLINE(iI)->calculateCanalValue();
		plotByIndexINLINE(iI)->calculateChokeValue();
	}
}
// Super Forts end

// autodetect lakes - start
void CvMap::updateWaterPlotTerrainTypes()
{
	EnumMap<TerrainTypes, bool> em;
	em.set(TERRAIN_COAST, true);
	em.set(TERRAIN_OCEAN, true);
	em.set(TERRAIN_SHALLOW_COAST, true);
	em.set(TERRAIN_LAKE, true);
	em.set(TERRAIN_ICE_LAKE, true);

	PlotRegionMap regions(em);

	// EnumMap to detect snow in regions, for potential Ice Lake
	EnumMap<TerrainTypes, bool> emIce;
	emIce.set(TERRAIN_SNOW, true);

	// WTP, ray we check there is no hot Terrain adjacent to Ice Lake
	EnumMap<TerrainTypes, bool> emHot;
	emHot.set(TERRAIN_PLAINS, true);
	emHot.set(TERRAIN_DESERT, true);
	emHot.set(TERRAIN_SHRUBLAND, true);
	emHot.set(TERRAIN_SAVANNAH, true);
	emHot.set(TERRAIN_MARSH, true);

	const int iNumRegions = regions.getNumRegions();
	for (int iRegion = 0; iRegion < iNumRegions; ++iRegion)
	{
		const PlotRegion& kRegion = regions.getRegion(iRegion);
		const int iNumPlots = kRegion.getNumPlots();

		bool bLake = !kRegion.isEurope() && kRegion.getNumPlots() < 50;
		bool bIceLake = false;
		bool bTooHotForIceLake = false;

		// possible checks for Ice Lake
		if (bLake)
		{
			// emIce now holds the terrains, which may trigger ice lakes
			bIceLake = kRegion.isTerrainAdjacent(emIce);
			// emHot now holds the terrains, which may still prevent ice lakes
			// only check if Ice lake is possible - thus not wasting performance
			if (bIceLake)
			{
				bTooHotForIceLake = kRegion.isTerrainAdjacent(emHot);
			}
		}

		for (int iPlot = 0; iPlot < iNumPlots; ++iPlot)
		{
			if (bLake)
			{
				const TerrainTypes eLakeTerrain = (bIceLake && !bTooHotForIceLake) ? TERRAIN_ICE_LAKE : TERRAIN_LAKE;
				kRegion.getPlot(iPlot)->setTerrainType(eLakeTerrain);
			}
			else // ocean
			{
				kRegion.getPlot(iPlot)->setCoastline();
			}
		}
	}
}
// autodetect lakes - end

int CvMap::getNumPlots(TerrainTypes eTerrain) const
{
	int iCount = 0;
	const int iNumPlots = numPlotsINLINE();
	for (int iPlot = 0; iPlot < iNumPlots; ++iPlot)
	{
		if (plotByIndexINLINE(iPlot)->getTerrainType() == eTerrain)
		{
			iCount++;
		}
	}
	return iCount;
}

//
// read object from a stream
// used during load
//
void CvMap::read(FDataStreamBase* pStream)
{
	CvMapInitData defaultMapData;

	// Init data before load
	reset(&defaultMapData);

	CvSavegameReaderBase readerbase(pStream);
	CvSavegameReader reader(readerbase);
	read(reader);

	setup();
}

// save object to a stream
// used during save
//
void CvMap::write(FDataStreamBase* pStream)
{
	CvSavegameWriterBase writerbase(pStream);
	CvSavegameWriter writer(writerbase);
	write(writer);
	writerbase.WriteFile();
}


//
// used for loading WB maps
//
void CvMap::rebuild(int iGridW, int iGridH, int iTopLatitude, int iBottomLatitude, bool bWrapX, bool bWrapY, WorldSizeTypes eWorldSize, ClimateTypes eClimate, SeaLevelTypes eSeaLevel, int iNumCustomMapOptions, CustomMapOptionTypes * aeCustomMapOptions)
{
	CvMapInitData initData(iGridW, iGridH, iTopLatitude, iBottomLatitude, bWrapX, bWrapY);

	// Set init core data
	GC.getInitCore().setWorldSize(eWorldSize);
	GC.getInitCore().setClimate(eClimate);
	GC.getInitCore().setSeaLevel(eSeaLevel);
	GC.getInitCore().setCustomMapOptions(iNumCustomMapOptions, aeCustomMapOptions);

	// Init map
	init(&initData);
}


//////////////////////////////////////////////////////////////////////////
// Protected Functions...
//////////////////////////////////////////////////////////////////////////

namespace
{
//TODO: Use FOR_EACH_ENUM
void visitPlot(CvPlot* pPlot, std::queue<CvPlot*>& plotQueue, stdext::hash_set<CvPlot*>& visited)
{
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pAdjacentPlot = plotDirection(pPlot->getX_INLINE(), pPlot->getY_INLINE(), ((DirectionTypes)iI));

		if (pAdjacentPlot == NULL)
			continue;

		if (visited.find(pAdjacentPlot) != visited.end())
			continue;

		if (pAdjacentPlot->isWater() && pAdjacentPlot->hasLargeRiver() || !pAdjacentPlot->isWater())
		{
			visited.insert(pAdjacentPlot);
			plotQueue.push(pAdjacentPlot);
		}
	}
}

// Large rivers aware area assigner
void calculateLandAreaBfs(CvPlot* pPlot, int iArea, stdext::hash_set<CvPlot*>& visited)
{
	// Stores the plot index
	std::queue<CvPlot*> plotQueue;

	plotQueue.push(pPlot);
	visited.insert(pPlot);

	while (!plotQueue.empty())
	{
		CvPlot* pPlot = plotQueue.front();
		plotQueue.pop();

		// Land areas spread across large river without assigning the large river plot to the land area
		if (pPlot->isWater() && pPlot->hasLargeRiver())
		{
			// Add all adjacent land plots to the queue but do not set the area of the large river plot
			visitPlot(pPlot, plotQueue, visited);
		}
		else if (!pPlot->isWater())
		{
			// Add all adjacent land plots or large river to the queue and set the area for this land plot
			pPlot->setArea(iArea);
			visitPlot(pPlot, plotQueue, visited);
		}
	}
	// Done with this area
}
} // end anon namespace

void CvMap::calculateAreas()
{
	PROFILE_FUNC();

	// The old logic is now only used for non-large river water plots
	for (int iI = 0; iI < numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = plotByIndexINLINE(iI);

		if (pLoopPlot->isWater())
		{
			gDLL->callUpdater();
			FAssertMsg(pLoopPlot != NULL, "LoopPlot is not assigned a valid value");

			if (pLoopPlot->getArea() == FFreeList::INVALID_INDEX)
			{
				CvArea* pArea = addArea();
				pArea->init(pArea->getID(), pLoopPlot->isWater());
				const int iArea = pArea->getID();
				pLoopPlot->setArea(iArea);
				gDLL->getFAStarIFace()->GeneratePath(&GC.getAreaFinder(), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), -1, -1, pLoopPlot->isWater(), iArea);
			}
		}
	}

	// This section only considers land plots
	stdext::hash_set<CvPlot*> visited;

	for (int iI = 0; iI < numPlotsINLINE(); iI++)
	{
		CvPlot*  pLoopPlot = plotByIndexINLINE(iI);

		if (pLoopPlot->isWater())
			continue;

		if (visited.find(pLoopPlot) != visited.end())
			continue;

		gDLL->callUpdater();
		FAssertMsg(pLoopPlot != NULL, "LoopPlot is not assigned a valid value");

		if (pLoopPlot->getArea() == FFreeList::INVALID_INDEX)
		{
			CvArea* pArea = addArea();
			pArea->init(pArea->getID(), pLoopPlot->isWater());
			const int iArea = pArea->getID();
			calculateLandAreaBfs(pLoopPlot, iArea, visited);
		}
	}
}

void CvMap::writeDesyncLog(FILE *f)
{
	for (int i = 0; i < numPlotsINLINE(); ++i)
	{
		fprintf(f, "Plot %d\n", i);
		plotByIndexINLINE(i)->writeDesyncLog(f);
	}
}

char CvMap::getCityCatchmentRadius() const
{
	return NUM_CITY_PLOTS == NUM_CITY_PLOTS_1_PLOT ? 1 : 2;
}

void CvMap::setCityCatchmentRadius(int iSetting)
{
	GC.setCityCatchmentRadius(iSetting);
}

bool CvMap::hasStream() const
{
	return m_bHasStream;
}

void CvMap::setStreamFlag()
{
	m_bHasStream = true;
}

// Private Functions...
