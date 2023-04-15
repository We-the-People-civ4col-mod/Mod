#include "CvGameCoreDLL.h"

#include "CvCityYields.h"

CvCityYields::CvCityYields(CvCity& city)
	: m_city(city)
{
	// WARNING: m_city can't be used in the constructor as the CvCity constructor is still running
}

int CvCityYields::getBaseRawYieldProduced(YieldTypes eYieldType) const
{
	PROFILE_FUNC();

	FAssertMsg(NO_PLAYER != m_city.getOwnerINLINE(), "City must have an owner");

	if (NO_YIELD == eYieldType)
	{
		return 0;
	}

	if (m_city.isOccupation())
	{
		return 0;
	}

	if (NO_PLAYER == m_city.getOwnerINLINE())
	{
		return 0;
	}


	int iIndoor = getBaseRawYieldProducedIndoor(eYieldType);
	int iOutdoor = getBaseRawYieldProducedPlots(eYieldType);
	int iBuildings = getBaseRawYieldProducedBuildings(eYieldType);
	int iLeader = getBaseRawYieldProducedLeader(eYieldType);

	return iIndoor + iOutdoor + iBuildings + iLeader;
}

int CvCityYields::getBaseRawYieldProducedIndoor(YieldTypes eYieldType) const
{
	if (m_city.isOccupation())
	{
		return 0;
	}

	int iCityYieldProduction = 0;
	for (int i = 0; i < m_city.getPopulation(); ++i)
	{
		CvUnit* pUnit = m_city.getPopulationUnitByIndex(i);
		if (NULL != pUnit)
		{
			ProfessionTypes eProfession = pUnit->getProfession();
			if (NO_PROFESSION != eProfession)
			{
				CvProfessionInfo& kProfessionInfo = GC.getProfessionInfo(eProfession);
				// R&R, ray , MYCP partially based on code of Aymerick - START
				if (kProfessionInfo.getNumYieldsProduced() > 1)
				{
					YieldTypes eYieldProduced = (YieldTypes)kProfessionInfo.getYieldsProduced(0);
					YieldTypes eYieldProduced2 = (YieldTypes)kProfessionInfo.getYieldsProduced(1);
					if (eYieldProduced == eYieldType || eYieldProduced2 == eYieldType)
					{
						iCityYieldProduction += m_city.getProfessionOutput(eProfession, pUnit);
					}
				}
				else
				{
					YieldTypes eYieldProduced = (YieldTypes)kProfessionInfo.getYieldsProduced(0);
					if (eYieldProduced == eYieldType)
					{
						iCityYieldProduction += m_city.getProfessionOutput(eProfession, pUnit);
					}
				}
				// R&R, ray , MYCP partially based on code of Aymerick - END
			}
		}
	}
	return iCityYieldProduction;
}

int CvCityYields::getBaseRawYieldProducedPlots(YieldTypes eYieldType) const
{
	if (m_city.isOccupation())
	{
		return 0;
	}

	const int iSlaveWorkerProductionBonus = m_city.getSlaveWorkerProductionBonus();
	int iPlotYieldProduction = 0;
	for (int i = 0; i < NUM_CITY_PLOTS; ++i)
	{
		CvPlot* pPlot = m_city.getCityIndexPlot(i);
		if (pPlot != NULL)
		{
			if (m_city.isUnitWorkingPlot(i))
			{
				int iPlotYieldProductionThisPlot = pPlot->getYield(eYieldType);
				if (!m_city.isNative())
				{
					CvUnit* const pUnit = m_city.getUnitWorkingPlot(pPlot);
					if (pUnit != NULL && pUnit->isForcedLaborer()) // could change this later to account for "forced labor factor"
					{
						if (iSlaveWorkerProductionBonus > 0 && pUnit->getUnitInfo().getYieldChange(eYieldType) > 0)
						{
							iPlotYieldProductionThisPlot *= (100 + iSlaveWorkerProductionBonus);
							iPlotYieldProductionThisPlot /= 100;
						}
					}
				}

				iPlotYieldProduction += iPlotYieldProductionThisPlot;
			}
		}
	}
	return iPlotYieldProduction;
}

int CvCityYields::getBaseRawYieldProducedBuildings(YieldTypes eYieldType) const
{
	if (m_city.isOccupation())
	{
		return 0;
	}

	int iBuildingYieldProduced = 0;
	const CvPlayer& owner = GET_PLAYER(m_city.getOwnerINLINE());
	const CvCivilizationInfo& civilizationInfo = GC.getCivilizationInfo(owner.getCivilizationType());

	for (BuildingClassTypes eBuildingClass = FIRST_BUILDINGCLASS; eBuildingClass < NUM_BUILDINGCLASS_TYPES; ++eBuildingClass)
	{
		BuildingTypes eBuilding = (BuildingTypes)civilizationInfo.getCivilizationBuildings(eBuildingClass);

		if (eBuilding != NO_BUILDING && m_city.isHasBuilding(eBuilding))
		{
			iBuildingYieldProduced += GC.getBuildingInfo(eBuilding).getYieldChange(eYieldType);
			iBuildingYieldProduced += m_city.getBuildingYieldChange(eBuildingClass, eYieldType);
			iBuildingYieldProduced += owner.getBuildingYieldChange(eBuildingClass, eYieldType);
		}
	}

	return iBuildingYieldProduced;
}

int CvCityYields::getBaseRawYieldProducedLeader(YieldTypes eYieldType) const
{
	return GET_PLAYER(m_city.getOwnerINLINE()).getCityExtraYield(eYieldType);
}
