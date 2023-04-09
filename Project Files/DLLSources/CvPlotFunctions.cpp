#include "CvGameCoreDLL.h"

#include "CvPlotFunctions.h"

RevealedPlotData::RevealedPlotData()
	: eRoute(NO_ROUTE)
	, eImprovement(NO_IMPROVEMENT)
{
}

RevealedPlotDataArray::RevealedPlotDataArray()
	: m_pArray(NULL)
{
}

RevealedPlotDataArray::~RevealedPlotDataArray()
{
	reset();
}

void RevealedPlotDataArray::allocate()
{
	FAssert(!isAllocated());
	m_pArray = new RevealedPlotData[MAX_TEAMS];
}

void RevealedPlotDataArray::reset()
{
	SAFE_DELETE_ARRAY(m_pArray);
}

bool Coordinates::isOnMap() const
{
	// don't use x() and y() here as those two will assert check against this function, creating an infinite loop.
	// exploit lazy evaluation to bail out once one condition is false, like don't check Y once X has already failed.
	return !isInvalidPlotCoord()
		&& (m_iX >= 0 && m_iX < GC.getMap().getGridWidthINLINE())
		&& (m_iY >= 0 && m_iY < GC.getMap().getGridHeightINLINE());
}

CvPlot* Coordinates::plot() const
{
	return isOnMap() ? GC.getMap().plotINLINE(*this) : NULL;
}

int Coordinates::plotNum() const
{
	FAssert(isOnMap());
	return GC.getMap().plotNumINLINE(*this);
}

Coordinates Coordinates::invalidCoord()
{
	Coordinates n;
	n.resetInvalid();
	return n;
}

Coordinates Coordinates::nullCoord()
{
	// Coordinates n;
	// n.set();
	// return n;
	return Coordinates();
}
