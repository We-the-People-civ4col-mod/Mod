#include "CvGameCoreDLL.h"

#include "CvCityYields.h"

CvCityYields::CvCityYields(CvCity& city)
	: m_city(city)
{
	// WARNING: m_city can't be used in the constructor as the CvCity constructor is still running
}

int CvCity::getBaseRawYieldProduced(YieldTypes eYieldType) const
{
	PROFILE_FUNC();

	FAssertMsg(NO_PLAYER != getOwnerINLINE(), "City must have an owner");

	if (NO_YIELD == eYieldType)
	{
		return 0;
	}
	CvYieldInfo& info = GC.getYieldInfo(eYieldType);

	if (isOccupation())
	{
		return 0;
	}

	if (NO_PLAYER == getOwnerINLINE())
	{
		return 0;
	}
	CvPlayer& owner = GET_PLAYER(getOwnerINLINE());

	//indoor professions
	int iCityYieldProduction = 0;
	for (int i = 0; i < getPopulation(); ++i)
	{
		CvUnit* pUnit = getPopulationUnitByIndex(i);
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
						iCityYieldProduction += getProfessionOutput(eProfession, pUnit);
					}
				}
				else
				{
					YieldTypes eYieldProduced = (YieldTypes)kProfessionInfo.getYieldsProduced(0);
					if (eYieldProduced == eYieldType)
					{
						iCityYieldProduction += getProfessionOutput(eProfession, pUnit);
					}
				}
				// R&R, ray , MYCP partially based on code of Aymerick - END
			}
		}
	}

	//outdoor professions
	int iPlotYieldProduction = 0;
	for (int i = 0; i < NUM_CITY_PLOTS; ++i)
	{
		CvPlot* pPlot = getCityIndexPlot(i);
		if (pPlot != NULL)
		{
			if (isUnitWorkingPlot(i))
			{
				//WTP, ray, Slave Hunter and Slave Master - START
				//iPlotYieldProduction += pPlot->getYield(eYieldType);
				if (isNative())
				{
					// normal previous logic
					iPlotYieldProduction += pPlot->getYield(eYieldType);
				}

				else
				{
					int iSlaveWorkerProductionBonus = getSlaveWorkerProductionBonus();
					if (iSlaveWorkerProductionBonus > 0)
					{
						int iProductionModifier = 100;
						CvUnit* pUnit = getUnitWorkingPlot(pPlot);
						if (pUnit != NULL && pUnit->getUnitInfo().LbD_canEscape() && pUnit->getUnitInfo().getYieldChange(eYieldType) > 0)
						{
							iProductionModifier += iSlaveWorkerProductionBonus;
						}
						iPlotYieldProduction += pPlot->getYield(eYieldType) * iProductionModifier / 100;
						//WTP, ray, Slave Hunter and Slave Master - END
					}
					else
					{
						// normal previous logic
						iPlotYieldProduction += pPlot->getYield(eYieldType);
					}
				}
				//WTP, ray, Slave Hunter and Slave Master - END
			}
		}
	}

	//building extra
	int iBuildingYieldProduced = 0;
	CvCivilizationInfo& civilizationInfo = GC.getCivilizationInfo(getCivilizationType());
	for (int i = 0; i < GC.getNumBuildingClassInfos(); ++i)
	{
		BuildingTypes eBuilding = (BuildingTypes)civilizationInfo.getCivilizationBuildings(i);

		if (eBuilding != NO_BUILDING && isHasBuilding(eBuilding))
		{
			iBuildingYieldProduced += GC.getBuildingInfo(eBuilding).getYieldChange(eYieldType);
			iBuildingYieldProduced += getBuildingYieldChange((BuildingClassTypes)i, eYieldType);
			iBuildingYieldProduced += owner.getBuildingYieldChange((BuildingClassTypes)i, eYieldType);
		}
	}

	return (iCityYieldProduction + iPlotYieldProduction + iBuildingYieldProduced);
}
