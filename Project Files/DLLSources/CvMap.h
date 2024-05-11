#pragma once

#ifndef CIV4_MAP_H
#define CIV4_MAP_H

//
//	FILE:	 CvMap.h
//	AUTHOR:  Soren Johnson
//	PURPOSE: Game map class
//-----------------------------------------------------------------------------
//	Copyright (c) 2004 Firaxis Games, Inc. All rights reserved.
//-----------------------------------------------------------------------------
//


#include "CvArea.h"
#include "CvPlot.h"


class FAStar;


inline int coordRange(int iCoord, int iRange, bool bWrap)
{
	if (bWrap)
	{
		if (iRange != 0)
		{
			if (iCoord < 0 )
			{
				return (iRange + (iCoord % iRange));
			}
			else if (iCoord >= iRange)
			{
				return (iCoord % iRange);
			}
		}
	}

	return iCoord;
}


// WTP - Plotregion - Nightinggale - start
class PlotRegion
{
	//template<typename T>
	friend class PlotRegionMap;
public:
	PlotRegion();

	bool isEurope() const;
	int getNumPlots() const;
	CvPlot* getPlot(int i) const;
	bool isTerrainAdjacent(const EnumMap<TerrainTypes, bool> em) const;

protected:
	void add(int iPlot, std::vector<PlotRegion*>& plotRegions);
	void merge(PlotRegion& rhs, std::vector<PlotRegion*>& plotRegions);
	bool m_bEurope;
	std::vector<int> m_aiPlots;
};


class PlotRegionMap : private boost::noncopyable
{
public:
	template<typename T>
	PlotRegionMap(const EnumMap<T, bool>& em);

	int getNumRegions() const;
	const PlotRegion& getRegion(int iIndex) const;

protected:
	void handlePlot(CvPlot* pPlot, int iX, int iY, std::vector<PlotRegion*>& plotRegions);
	std::vector<PlotRegion> m_aRegions;
};
// WTP - Plotregion - Nightinggale - end

//
// holds initialization info
//
struct CvMapInitData
{
	int m_iGridW;						// in game plots
	int m_iGridH;						// in game plots
	int m_iTopLatitude;
	int m_iBottomLatitude;

	bool m_bWrapX;
	bool m_bWrapY;

	CvMapInitData(int iGridW=0, int iGridH=0, int iTopLatitude=90, int iBottomLatitude=-90, bool bWrapX=false, bool bWrapY=false) :
		m_iGridH(iGridH),m_iGridW(iGridW),m_iTopLatitude(iTopLatitude),m_iBottomLatitude(iBottomLatitude),m_bWrapY(bWrapY),m_bWrapX(bWrapX)
	{ }
};


//
// CvMap
//
class CvSelectionGroup;
class CvMap
{

	friend class CyMap;

public:

	CvMap();
	virtual ~CvMap();

	DllExport void init(CvMapInitData* pInitData=NULL);
	DllExport void setupGraphical();
	DllExport void reset(CvMapInitData* pInitData);

protected:

	void uninit();
	void setup();

public:
	DllExport void erasePlots();
	void setRevealedPlots(TeamTypes eTeam, bool bNewValue, bool bTerrainOnly = false);
	void setAllPlotTypes(PlotTypes ePlotType);

	void doTurn();

	DllExport void updateFlagSymbols();

	DllExport void updateFog();
	void updateVisibility();
	DllExport void updateSymbolVisibility();
	void updateSymbols();
	DllExport void updateMinimapColor();
	void updateSight(bool bIncrement);
	DllExport void updateCenterUnit();
	void updateWorkingCity();
	void updateMinOriginalStartDist(CvArea* pArea);
	void updateYield();
	void updateCulture();

	void verifyUnitValidPlot();

	CvPlot* syncRandPlot(int iFlags = 0, int iArea = -1, int iMinUnitDistance = -1, int iTimeout = 100) const;

	DllExport CvCity* findCity(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, bool bSameArea = true, bool bCoastalOnly = false, TeamTypes eTeamAtWarWith = NO_TEAM, DirectionTypes eDirection = NO_DIRECTION, CvCity* pSkipCity = NULL);
	CvCity* findCity(Coordinates coord, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, bool bSameArea = true, bool bCoastalOnly = false, TeamTypes eTeamAtWarWith = NO_TEAM, DirectionTypes eDirection = NO_DIRECTION, CvCity* pSkipCity = NULL);
	CvSelectionGroup* findSelectionGroup(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, bool bReadyToSelect = false);

	CvArea* findBiggestArea(bool bWater);

	int getMapFractalFlags();
	bool findWater(CvPlot* pPlot, int iRange, bool bFreshWater);

	DllExport bool isPlot(int iX, int iY) const;
	inline int isPlotINLINE(int iX, int iY) const
	{
		return ((iX >= 0) && (iX < getGridWidthINLINE()) && (iY >= 0) && (iY < getGridHeightINLINE()));
	}
	inline int isPlotINLINE(Coordinates coord) const
	{
		return coord.isOnMap();
	}

	DllExport int numPlots() const;
	inline int numPlotsINLINE() const
	{
		return getGridWidthINLINE() * getGridHeightINLINE();
	}

	DllExport int plotNum(int iX, int iY) const;
	inline int plotNumINLINE(int iX, int iY) const
	{
		return ((iY * getGridWidthINLINE()) + iX);
	}
	inline int plotNumINLINE(Coordinates coord) const
	{
		return ((coord.y() * getGridWidthINLINE()) + coord.x());
	}

	int plotX(int iIndex) const;
	int plotY(int iIndex) const;

	int pointXToPlotX(float fX);
	DllExport float plotXToPointX(int iX);

	int pointYToPlotY(float fY);
	DllExport float plotYToPointY(int iY);

	float getWidthCoords();
	float getHeightCoords();
	int maxPlotDistance();
	int maxStepDistance();

	DllExport int getGridWidth() const;
	inline int getGridWidthINLINE() const
	{
		return m_iGridWidth;
	}

	DllExport int getGridHeight() const;
	inline int getGridHeightINLINE() const
	{
		return m_iGridHeight;
	}

	int getLandPlots();
	void changeLandPlots(int iChange);
	int getOwnedPlots();
	void changeOwnedPlots(int iChange);
	int getTopLatitude();
	int getBottomLatitude();

	int getNextRiverID();
	void incrementNextRiverID();

	DllExport bool isWrapX();
	inline bool isWrapXINLINE() const
	{
		return m_bWrapX;
	}

	DllExport bool isWrapY();
	inline bool isWrapYINLINE() const
	{
		return m_bWrapY;
	}

	DllExport bool isWrap();
	inline bool isWrapINLINE() const
	{
		return m_bWrapX || m_bWrapY;
	}

	DllExport WorldSizeTypes getWorldSize();
	ClimateTypes getClimate();
	SeaLevelTypes getSeaLevel();

	int getNumCustomMapOptions();
	CustomMapOptionTypes getCustomMapOption(int iOption);

	int getNumBonuses(BonusTypes eIndex);
	void changeNumBonuses(BonusTypes eIndex, int iChange);
	int getNumBonusesOnLand(BonusTypes eIndex);
	void changeNumBonusesOnLand(BonusTypes eIndex, int iChange);

	DllExport CvPlot* plotByIndex(int iIndex) const;
	inline CvPlot* plotByIndexINLINE(int iIndex) const
	{
		return (((iIndex >= 0) && (iIndex < (getGridWidthINLINE() * getGridHeightINLINE()))) ? &(m_pMapPlots[iIndex]) : NULL);
	}

	DllExport CvPlot* plot(int iX, int iY) const;
	__forceinline CvPlot* plotINLINE(int iX, int iY) const
	{
		if ((iX == INVALID_PLOT_COORD) || (iY == INVALID_PLOT_COORD))
		{
			return NULL;
		}
		int iMapX = coordRange(iX, getGridWidthINLINE(), isWrapXINLINE());
		int iMapY = coordRange(iY, getGridHeightINLINE(), isWrapYINLINE());
		return ((isPlotINLINE(iMapX, iMapY)) ? &(m_pMapPlots[plotNumINLINE(iMapX, iMapY)]) : NULL);
	}

	__forceinline CvPlot* plotINLINE(Coordinates coord) const
	{
		if(!coord.isOnMap())
			{
				return NULL;
			}
		return &(m_pMapPlots[plotNumINLINE(coord)]);
	}

	__forceinline CvPlot* plotSoren(int iX, int iY) const // advc.inl: Renamed from plotSorenINLINE
	{
		if (iX == INVALID_PLOT_COORD || iY == INVALID_PLOT_COORD)
			return NULL;
		FAssert(isPlot(iX, iY)); // advc: Assertion added
		return &(m_pMapPlots[plotNum(iX, iY)]);
	} // <advc.inl> Even faster and less confusingly named; replacing the above in most places.

	__forceinline CvPlot* plotSoren(Coordinates coord) const
	{
		if (coord.isInvalidPlotCoord())
			return NULL;
		FAssert(isPlotINLINE(coord)); // advc: Assertion added
		return &(m_pMapPlots[plotNumINLINE(coord)]);
	}

	__forceinline CvPlot& getPlot(int x, int y) const
	{
		FAssert(isPlot(x, y));
		return m_pMapPlots[plotNum(x, y)];
	} // </advc.inl>

	__forceinline CvPlot& getPlot(Coordinates coord) const
	{
		FAssert(isPlotINLINE(coord));
		return m_pMapPlots[plotNumINLINE(coord)];
	} // </advc.inl>

	DllExport CvPlot* pointToPlot(float fX, float fY);
	int getIndexAfterLastArea();
	int getNumAreas();
	int getNumLandAreas();

	CvArea* getArea(int iID);
	CvArea* addArea();
	void deleteArea(int iID);
	// iteration
	CvArea* firstArea(int *pIterIdx, bool bRev=false);
	CvArea* nextArea(int *pIterIdx, bool bRev=false);

	void recalculateAreas();

	void resetPathDistance();
	// Super Forts begin *canal* *choke*
	//int calculatePathDistance(CvPlot *pSource, CvPlot *pDest); //original
	int calculatePathDistance(CvPlot *pSource, CvPlot *pDest, CvPlot *pInvalidPlot = NULL);	// Exposed to Python
	void calculateCanalAndChokePoints();	// Exposed to Python
	// Super Forts end

	void updateWaterPlotTerrainTypes(); // autodetect lakes

	int getNumPlots(TerrainTypes eTerrain) const;

	// Serialization:
	virtual void read(FDataStreamBase* pStream);
	virtual void write(FDataStreamBase* pStream);
	void rebuild(int iGridW, int iGridH, int iTopLatitude, int iBottomLatitude, bool bWrapX, bool bWrapY, WorldSizeTypes eWorldSize, ClimateTypes eClimate, SeaLevelTypes eSeaLevel, int iNumCustomMapOptions, CustomMapOptionTypes * eCustomMapOptions);

	void writeDesyncLog(FILE *f);

	char getCityCatchmentRadius() const;
	void setCityCatchmentRadius(int iSetting);

protected:

	void resetVisibilityCache() const; // defined in CvPlot.cpp

	void resetSavedData();
	void read(CvSavegameReader reader);
	void write(CvSavegameWriter writer);

	int m_iGridWidth;
	int m_iGridHeight;
	int m_iLandPlots;
	int m_iOwnedPlots;
	int m_iTopLatitude;
	int m_iBottomLatitude;
	int m_iNextRiverID;

	bool m_bWrapX;
	bool m_bWrapY;

	BonusArray<int> m_ja_NumBonuses;
	BonusArray<int> m_ja_NumBonusesOnLand;

	CvPlot* m_pMapPlots;

	FFreeListTrashArray<CvArea> m_areas;

	void calculateAreas();

};

#endif
