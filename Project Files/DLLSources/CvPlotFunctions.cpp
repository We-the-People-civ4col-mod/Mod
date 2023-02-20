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

CvPlot *FCoord::plot()
{
	return GC.getMapINLINE().plotINLINE(*this);
}

bool FCoord::isOnMap() const
{
	if(isInvalidPlotCoord())
		{
			return false;
		}
	bool bValidX = (x() >= 0 && x() < GC.getMapINLINE().getGridWidthINLINE());
	bool bValidY = (y() >= 0 && y() < GC.getMapINLINE().getGridHeightINLINE());
	return bValidX && bValidY;
}
