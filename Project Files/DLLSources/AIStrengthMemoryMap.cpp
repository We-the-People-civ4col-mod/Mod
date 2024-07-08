#include "CvGameCoreDLL.h"
#include "AIStrengthMemoryMap.h"
#include "CvTeam.h"
#include "CvMap.h"

// advc.158: New implementation file; see comment in header.

void AIStrengthMemoryMap::init(PlotNumTypes eMapSize, TeamTypes eTeam)
{
	FAssert(eMapSize > 0);
	FAssert(eTeam != NO_TEAM);
	/*m_aiMap.clear();
	m_aiMap.resize(eMapSize, 0);*/
	m_map.clear();
	m_eTeam = eTeam;
}


void AIStrengthMemoryMap::reset()
{
	//m_aiMap.clear();
	m_map.clear();
}


int AIStrengthMemoryMap::get(CvPlot const& kPlot) const
{
	return get(kPlot.plotNum());
}


void AIStrengthMemoryMap::set(CvPlot const& kPlot, int iNewValue)
{
	PlotNumTypes ePlot = kPlot.plotNum();
	/*FAssertBounds(0, m_aiMap.size(), ePlot);
	m_aiMap[ePlot] = iNewValue;*/
	m_map[ePlot] = iNewValue;
}


void AIStrengthMemoryMap::decay()
{
	PROFILE_FUNC();
	/*if (m_aiMap.size() != GC.getMap().numPlots())
		return;*/
	CvTeam const& kTeam = GET_TEAM(m_eTeam);
	// K-Mod: reduce by 4% (arbitrary number), rounding down.
	int const iDecayPercent = 4;
	/*for (int i = 0; i < GC.getMap().numPlots(); i++)
	{
		if (m_aiMap[i] == 0)
			continue;
		FAssert(m_aiMap[i] > 0);
		CvPlot const& kPlot = GC.getMap().getPlotByIndex(i);*/
	PlotStrengthMap::iterator it = m_map.begin();
	while (it != m_map.end())
	{
		CvPlot const& kPlot = GC.getMap().getPlotByIndex(it->first);
		if (kPlot.isVisible(m_eTeam) &&
			!kPlot.isVisibleEnemyUnit(kTeam.getLeaderID()))
		{
			it = m_map.erase(it);
			//m_aiMap[i] = 0;
		}
		//else m_aiMap[i] = ((100 - iDecayPercent) * m_aiMap[i]) / 100;
		else
		{
			it->second = ((100 - iDecayPercent) * it->second) / 100;
			++it;
		}
	}
}
