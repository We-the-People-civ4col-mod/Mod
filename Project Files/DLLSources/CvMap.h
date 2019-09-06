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

	DllExport CvMap();
	DllExport virtual ~CvMap();

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
	DllExport void updateVisibility();
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

	CvPlot* syncRandPlot(int iFlags = 0, int iArea = -1, int iMinUnitDistance = -1, int iTimeout = 100);

	DllExport CvCity* findCity(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, bool bSameArea = true, bool bCoastalOnly = false, TeamTypes eTeamAtWarWith = NO_TEAM, DirectionTypes eDirection = NO_DIRECTION, CvCity* pSkipCity = NULL);
	DllExport CvSelectionGroup* findSelectionGroup(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, bool bReadyToSelect = false);

	CvArea* findBiggestArea(bool bWater);

	int getMapFractalFlags();
	bool findWater(CvPlot* pPlot, int iRange, bool bFreshWater);

	DllExport bool isPlot(int iX, int iY) const;
#ifdef _USRDLL
	inline int isPlotINLINE(int iX, int iY) const
	{
		return ((iX >= 0) && (iX < getGridWidthINLINE()) && (iY >= 0) && (iY < getGridHeightINLINE()));
	}
#endif
	DllExport int numPlots() const;
#ifdef _USRDLL
	inline int numPlotsINLINE() const
	{
		return getGridWidthINLINE() * getGridHeightINLINE();
	}
#endif
	DllExport int plotNum(int iX, int iY) const;
#ifdef _USRDLL
	inline int plotNumINLINE(int iX, int iY) const
	{
		return ((iY * getGridWidthINLINE()) + iX);
	}
#endif
	int plotX(int iIndex) const;
	int plotY(int iIndex) const;

	DllExport int pointXToPlotX(float fX);
	DllExport float plotXToPointX(int iX);

	DllExport int pointYToPlotY(float fY);
	DllExport float plotYToPointY(int iY);

	float getWidthCoords();
	float getHeightCoords();
	int maxPlotDistance();
	int maxStepDistance();

	DllExport int getGridWidth() const;
#ifdef _USRDLL
	inline int getGridWidthINLINE() const
	{
		return m_iGridWidth;
	}
#endif
	DllExport int getGridHeight() const;
#ifdef _USRDLL
	inline int getGridHeightINLINE() const
	{
		return m_iGridHeight;
	}
#endif
	DllExport int getLandPlots();
	void changeLandPlots(int iChange);
	DllExport int getOwnedPlots();
	void changeOwnedPlots(int iChange);
	int getTopLatitude();
	int getBottomLatitude();

	int getNextRiverID();
	void incrementNextRiverID();

	DllExport bool isWrapX();
#ifdef _USRDLL
	inline bool isWrapXINLINE() const
	{
		return m_bWrapX;
	}
#endif
	DllExport bool isWrapY();
#ifdef _USRDLL
	inline bool isWrapYINLINE() const
	{
		return m_bWrapY;
	}
#endif
	DllExport bool isWrap();
#ifdef _USRDLL
	inline bool isWrapINLINE() const
	{
		return m_bWrapX || m_bWrapY;
	}
#endif
	DllExport WorldSizeTypes getWorldSize();
	DllExport ClimateTypes getClimate();
	DllExport SeaLevelTypes getSeaLevel();

	DllExport int getNumCustomMapOptions();
	DllExport CustomMapOptionTypes getCustomMapOption(int iOption);

	int getNumBonuses(BonusTypes eIndex);
	void changeNumBonuses(BonusTypes eIndex, int iChange);
	int getNumBonusesOnLand(BonusTypes eIndex);
	void changeNumBonusesOnLand(BonusTypes eIndex, int iChange);
	DllExport CvPlot* plotByIndex(int iIndex) const;
#ifdef _USRDLL
	inline CvPlot* plotByIndexINLINE(int iIndex) const
	{
		return (((iIndex >= 0) && (iIndex < (getGridWidthINLINE() * getGridHeightINLINE()))) ? &(m_pMapPlots[iIndex]) : NULL);
	}
#endif
	DllExport CvPlot* plot(int iX, int iY) const;
#ifdef _USRDLL
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
	__forceinline CvPlot* plotSorenINLINE(int iX, int iY) const
	{
		if ((iX == INVALID_PLOT_COORD) || (iY == INVALID_PLOT_COORD))
		{
			return NULL;
		}
		return &(m_pMapPlots[plotNumINLINE(iX, iY)]);
	}
#endif
	DllExport CvPlot* pointToPlot(float fX, float fY);
	int getIndexAfterLastArea();
	DllExport int getNumAreas();
	DllExport int getNumLandAreas();

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

	// Serialization:
	DllExport virtual void read(FDataStreamBase* pStream);
	DllExport virtual void write(FDataStreamBase* pStream);
	void rebuild(int iGridW, int iGridH, int iTopLatitude, int iBottomLatitude, bool bWrapX, bool bWrapY, WorldSizeTypes eWorldSize, ClimateTypes eClimate, SeaLevelTypes eSeaLevel, int iNumCustomMapOptions, CustomMapOptionTypes * eCustomMapOptions);

	void writeDesyncLog(FILE *f);

	int getCityCatchmentRadius() const;
	void setCityCatchmentRadius(int iSetting);

protected:

	void resetSavedData();
	void read(CvSavegameReader& reader);
	void write(CvSavegameWriter& writer);

	int m_iGridWidth;
	int m_iGridHeight;
	int m_iLandPlots;
	int m_iOwnedPlots;
	int m_iTopLatitude;
	int m_iBottomLatitude;
	int m_iNextRiverID;

	bool m_bWrapX;
	bool m_bWrapY;

	bool m_bUseTwoPlotCities;

	BonusArray<int> m_ja_NumBonuses;
	BonusArray<int> m_ja_NumBonusesOnLand;

	CvPlot* m_pMapPlots;

	FFreeListTrashArray<CvArea> m_areas;

	void calculateAreas();

};

#endif
