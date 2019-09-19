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
