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


class DirCoordinates
{
public:
	DirCoordinates(DirectionTypes direction);
	~DirCoordinates();

	int x() const
	{
		return m_iX;
	}
	int y() const
	{
		return m_iY;
	}

protected:
	const int m_iX;
	const int m_iY;
};

inline DirCoordinates::DirCoordinates(DirectionTypes direction) :
	m_iX(GC.getPlotDirectionX()[direction]),
	m_iY(GC.getPlotDirectionY()[direction])
{
}

inline DirCoordinates::~DirCoordinates()
{
}

class RelCoordinates
{
public:
	explicit RelCoordinates(int iX, int iY) :
	m_iX(iX),
	m_iY(iY)
	{
	}
	~RelCoordinates()
	{
	}

	int x() const
	{
		return m_iX;
	}
	int y() const
	{
		return m_iY;
	}

protected:
	const int m_iX;
	const int m_iY;
};

class Coordinates
{
	friend class CvSavegameReader;
	friend class CvSavegameWriter;
public:
	explicit Coordinates(int iX = 0, int iY = 0)
	{
		set(iX, iY);
	}
	~Coordinates()
	{
	}

	inline void set(int iX = 0, int iY = 0);
	inline void resetInvalid();

	int x() const
	{
		return m_iX;
	}
	int y() const
	{
		return m_iY;
	}

	inline Coordinates neighbour(DirectionTypes direction) const;
	inline CvPlot* neighbourPlot(DirectionTypes direction) const;
	inline bool hasNeighbour(DirectionTypes direction) const;

	CvPlot* plot() const;
	int plotNum() const;
	inline bool isOnMap() const;
	inline bool isInvalidPlotCoord() const;

	static Coordinates invalidCoord();
	static Coordinates nullCoord();

protected:
	int m_iX;
	int m_iY;
};

inline bool Coordinates::isInvalidPlotCoord() const
{
	return (m_iX == INVALID_PLOT_COORD && m_iY == INVALID_PLOT_COORD);
}

inline bool operator==(const Coordinates &l, const Coordinates &r)
{
	FAssert ((l.isOnMap() || l.isInvalidPlotCoord()) && (r.isOnMap() || r.isInvalidPlotCoord()));
	return (l.x()==r.x() && l.y()==r.y());
}

inline bool operator!=(const Coordinates &l, const Coordinates &r)
{
	FAssert ((l.isOnMap() || l.isInvalidPlotCoord()) && (r.isOnMap() || r.isInvalidPlotCoord()));
	return ((l.x()!=r.x() || l.y()!=r.y()));
}

inline Coordinates operator+(const Coordinates &l, const DirCoordinates &r)
{
	FAssert (l.isOnMap());
	return Coordinates(l.x()+r.x(), l.y()+r.y());
}

inline Coordinates operator+(const Coordinates &l, const RelCoordinates &r)
{
	FAssert (l.isOnMap());
	return Coordinates(l.x()+r.x(), l.y()+r.y());
}

inline void Coordinates::set(int iX, int iY)
{
	m_iX = iX;
	m_iY = iY;
	if (!isOnMap())
	{
		resetInvalid();
	}
}

inline void Coordinates::resetInvalid()
{
	m_iX = INVALID_PLOT_COORD;
	m_iY = INVALID_PLOT_COORD;
}

inline Coordinates Coordinates::neighbour(DirectionTypes direction) const
{
	FAssert (isOnMap());
	const Coordinates n = *this + DirCoordinates(direction);
	return n;
}

inline CvPlot* Coordinates::neighbourPlot(DirectionTypes direction) const
{
	FAssert (isOnMap());
	const Coordinates n = *this + DirCoordinates(direction);
	return n.plot();
}

inline bool Coordinates::hasNeighbour(DirectionTypes direction) const
{
	FAssert (isOnMap());
	Coordinates n = *this + DirCoordinates(direction);
	return n.isOnMap();
}

#endif
