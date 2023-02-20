#pragma once

#ifndef CIV4_PLOT_FUNCTIONS_H
#define CIV4_PLOT_FUNCTIONS_H



//
// add helper classes for CvPlot
//
// These classes helps simplifying the code in CvPlot as memory management is moved to other classes.
// Also instead of the vanilla approach with one array for each variable in CvPlot,
// this approach will make one array of teams where each entry is a list of variables for that team.
// This will make better use of CPU cache (hence faster) whenever a team need more than one variable for that team (the most common usage)
//


// data container for each team
// the data might as well be public because it's only used by RevealedPlotDataArray where it's private
// this means no public/global access despite everything being public
class RevealedPlotData
{
public:
	ImprovementTypes eImprovement : 16;
	RouteTypes eRoute : 16;

	RevealedPlotData();
};

// Array controller for instances of RevealedPlotData
// It can be used like a variable in CvPlot because it has automated memory management (no need to free etc)
// It's essentially just a bunch of get/set functions, but the outside world doesn't have to consider if the array is allocated or not.
// In that way it's similar to JustInTimeArray where unallocated arrays can be used and assumed to contain just default values.
class RevealedPlotDataArray
{
public:
	RevealedPlotDataArray();
	~RevealedPlotDataArray();

	ImprovementTypes getImprovement(TeamTypes eTeam) const;
	RouteTypes       getRoute      (TeamTypes eTeam) const;

	void set(TeamTypes eTeam, ImprovementTypes eImprovement                   );
	void set(TeamTypes eTeam,                                RouteTypes eRoute);
	void set(TeamTypes eTeam, ImprovementTypes eImprovement, RouteTypes eRoute);

	bool isAllocated() const;
	void reset();

private:
	void allocate();
	RevealedPlotData* m_pArray;
};

//
// inline functions for performance reasons
//

inline ImprovementTypes RevealedPlotDataArray::getImprovement(TeamTypes eTeam) const
{
	FAssert(eTeam >= 0 && eTeam < MAX_TEAMS);
	return m_pArray ? m_pArray[eTeam].eImprovement : NO_IMPROVEMENT;
}

inline RouteTypes RevealedPlotDataArray::getRoute(TeamTypes eTeam) const
{
	FAssert(eTeam >= 0 && eTeam < MAX_TEAMS);
	return m_pArray ? m_pArray[eTeam].eRoute : NO_ROUTE;
}

inline void RevealedPlotDataArray::set(TeamTypes eTeam, ImprovementTypes eImprovement)
{
	FAssert(eTeam >= 0 && eTeam < MAX_TEAMS);
	if (!isAllocated())
	{
		if (eImprovement == NO_IMPROVEMENT)
		{
			return;
		}
		allocate();
	}
	m_pArray[eTeam].eImprovement = eImprovement;
}

inline void RevealedPlotDataArray::set(TeamTypes eTeam, RouteTypes eRoute)
{
	FAssert(eTeam >= 0 && eTeam < MAX_TEAMS);
	if (!isAllocated())
	{
		if (eRoute == NO_ROUTE)
		{
			return;
		}
		allocate();
	}
	m_pArray[eTeam].eRoute = eRoute;
}

inline void RevealedPlotDataArray::set(TeamTypes eTeam, ImprovementTypes eImprovement, RouteTypes eRoute)
{
	FAssert(eTeam >= 0 && eTeam < MAX_TEAMS);
	if (!isAllocated())
	{
		if (eImprovement == NO_IMPROVEMENT && eRoute == NO_ROUTE)
		{
			return;
		}
		allocate();
	}
	m_pArray[eTeam].eImprovement = eImprovement;
	m_pArray[eTeam].eRoute = eRoute;
}

inline bool RevealedPlotDataArray::isAllocated() const
{
	return m_pArray != NULL;
}


class FDirCoord
{
public:
	FDirCoord(DirectionTypes direction);
	~FDirCoord();

	int x() const;
	int y() const;

protected:
	const int m_iX;
	const int m_iY;
};

FDirCoord::FDirCoord(DirectionTypes direction) :
	m_iX(GC.getPlotDirectionX()[direction]),
	m_iY(GC.getPlotDirectionY()[direction])
{
}

FDirCoord::~FDirCoord()
{

}

inline int FDirCoord::x() const {
	return m_iX;
}

inline int FDirCoord::y() const {
	return m_iY;
}


class FCoord
{
public:
	FCoord(int iX, int iY)
	{
		reset(iX, iY);
	}
	FCoord()
	{
		reset(0, 0);
	}
	~FCoord()
	{
	}

	inline void reset(int iX, int iY);
	inline void resetInvalid();

	int x() const;
	int y() const;

	FCoord neighbour(DirectionTypes direction) const;
	bool hasNeighbour(DirectionTypes direction) const;

	inline CvPlot *plot();
	bool isOnMap() const;
	inline bool isInvalidPlotCoord() const;

protected:
	int m_iX;
	int m_iY;
};

inline bool FCoord::isInvalidPlotCoord() const
{
	return (m_iX == INVALID_PLOT_COORD && m_iY == INVALID_PLOT_COORD);
}

inline bool operator==(const FCoord &l, const FCoord &r)
{
	FAssert ((l.isOnMap() || l.isInvalidPlotCoord()) && (r.isOnMap() || r.isInvalidPlotCoord()));
	return (l.x()==r.x() && l.y()==r.y());
}

inline bool operator!=(const FCoord &l, const FCoord &r)
{
	FAssert ((l.isOnMap() || l.isInvalidPlotCoord()) && (r.isOnMap() || r.isInvalidPlotCoord()));
	return ((l.x()!=r.x() || l.y()!=r.y()));
}

inline FCoord operator+(const FCoord &l, const FDirCoord &r)
{
	FAssert (l.isValid());
	return FCoord(l.x()+r.x(), l.y()+r.y());
}

inline void FCoord::reset(int iX, int iY)
{
	m_iX = iX;
	m_iY = iY;
}

inline void FCoord::resetInvalid()
{
	m_iX = INVALID_PLOT_COORD;
	m_iY = INVALID_PLOT_COORD;
}

inline int FCoord::x() const
{
	FAssert (isOnMap() || isInvalidPlotCoord());
	return m_iX;
}

inline int FCoord::y() const
{
	FAssert (isOnMap() || isInvalidPlotCoord());
	return m_iY;
}

FCoord FCoord::neighbour(DirectionTypes direction) const
{
	FAssert (isOnMap());
	FCoord n = *this + FDirCoord(direction);
	return n;
}

bool FCoord::hasNeighbour(DirectionTypes direction) const
{
	FAssert (isOnMap());
	FCoord n = *this + FDirCoord(direction);
	return n.isOnMap();
}

#endif
