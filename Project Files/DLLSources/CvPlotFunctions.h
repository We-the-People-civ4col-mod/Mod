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

#endif
