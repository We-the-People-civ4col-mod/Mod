#include "CvGameCoreDLL.h"
#include "CyCity.h"
#include "CyUnit.h"
#include "CyPlot.h"
#include "CyArea.h"
#include "CvInfos.h"

//# include <boost/python/manage_new_object.hpp>
//# include <boost/python/return_value_policy.hpp>

//
// published python interface for CyCity
//

void CyCityPythonInterface3(python::class_<CyCity>& x)
{
	OutputDebugString("Python Extension Module - CyCityPythonInterface3\n");

	x
		.def("getOccupationTimer", &CyCity::getOccupationTimer, "int () - total # of turns remaining on occupation timer")
		.def("isOccupation", &CyCity::isOccupation, "bool () - is the city under occupation?")
		.def("setOccupationTimer", &CyCity::setOccupationTimer, "void (iNewValue) - set the Occupation Timer to iNewValue")
		.def("changeOccupationTimer", &CyCity::changeOccupationTimer, "void (iChange) - adjusts the Occupation Timer by iChange")
		.def("getCultureUpdateTimer", &CyCity::getCultureUpdateTimer, "int () - Culture Update Timer")
		.def("changeCultureUpdateTimer", &CyCity::changeCultureUpdateTimer, "void (iChange) - adjusts the Culture Update Timer by iChange")
		.def("isNeverLost", &CyCity::isNeverLost, "bool ()")
		.def("setNeverLost", &CyCity::setNeverLost, "void (iNewValue)")
		.def("isBombarded", &CyCity::isBombarded, "bool ()")
		.def("setBombarded", &CyCity::setBombarded, "void (iNewValue)")
		.def("setAllCitizensAutomated", &CyCity::setAllCitizensAutomated, "void (bool bAutomated)")
		.def("isProductionAutomated", &CyCity::isProductionAutomated, "bool () - is production under automation?")
		.def("setProductionAutomated", &CyCity::setProductionAutomated, "void (bool bNewValue) - set city production automation to bNewValue")
		.def("isWallOverride", &CyCity::isWallOverride, "bool isWallOverride()")
		.def("setWallOverride", &CyCity::setWallOverride, "setWallOverride(bool bOverride)")
		.def("setCitySizeBoost", &CyCity::setCitySizeBoost, "setCitySizeBoost(int iBoost)")
		.def("getOwner", &CyCity::getOwner, "int /*PlayerTypes*/ ()")
		.def("getTeam", &CyCity::getTeam, "int /*TeamTypes*/ ()")
		.def("getPreviousOwner", &CyCity::getPreviousOwner, "int /*PlayerTypes*/ ()")
		.def("getOriginalOwner", &CyCity::getOriginalOwner, "int /*PlayerTypes*/ ()")
		.def("getCultureLevel", &CyCity::getCultureLevel, "int /*CultureLevelTypes*/ ()")
		.def("getCultureThreshold", &CyCity::getCultureThreshold)
		.def("getSeaPlotYield", &CyCity::getSeaPlotYield, "int (int /*YieldTypes*/) - total YieldType for water plots")
		.def("getRiverPlotYield", &CyCity::getRiverPlotYield, "int (int /*YieldTypes*/) - total YieldType for river plots")

		.def("getBaseRawYieldProduced", &CyCity::getBaseRawYieldProduced, "int (int /*YieldTypes*/) - Yield Bonus produced in all cities")
		.def("getRawYieldProduced", &CyCity::getRawYieldProduced, "int (int /*YieldTypes*/) - Yield added to the city each turn")
		.def("getRawYieldConsumed", &CyCity::getRawYieldConsumed, "int (int /*YieldTypes*/) - Yield remoced from the city each turn")

		.def("getBaseYieldRateModifier", &CyCity::getBaseYieldRateModifier)
		.def("getYieldRate", &CyCity::getYieldRate, "int (int /*YieldTypes*/) - total value of YieldType")
		.def("getCultureRate", &CyCity::getCultureRate, "int ()")
		.def("getYieldRateModifier", &CyCity::getYieldRateModifier, "int (int /*YieldTypes*/) - yield rate modifier for YieldType")

		.def("getDomainFreeExperience", &CyCity::getDomainFreeExperience, "int (int /*DomainTypes*/)")
		.def("getDomainProductionModifier", &CyCity::getDomainProductionModifier, "int (int /*DomainTypes*/)")
		.def("getCulture", &CyCity::getCulture, "int /*PlayerTypes*/ ()")
		.def("countTotalCulture", &CyCity::countTotalCulture, "int ()")
		.def("findHighestCulture", &CyCity::findHighestCulture, "PlayerTypes ()")
		.def("calculateCulturePercent", &CyCity::calculateCulturePercent, "int (int eIndex)")
		.def("calculateTeamCulturePercent", &CyCity::calculateTeamCulturePercent, "int /*TeamTypes*/ ()")
		.def("setCulture", &CyCity::setCulture, "void (int PlayerTypes eIndex`, bool bPlots)")
		.def("changeCulture", &CyCity::changeCulture, "void (int PlayerTypes eIndex, int iChange, bool bPlots)")

		.def("getTotalYieldStored", &CyCity::getTotalYieldStored, "int ()")						//VET NewCapacity - 1/1
		.def("getYieldStored", &CyCity::getYieldStored, "int /*YieldTypes*/ ()")
		.def("setYieldStored", &CyCity::setYieldStored, "void (int YieldTypes eIndex, int)")
		.def("changeYieldStored", &CyCity::changeYieldStored, "void (int YieldTypes, int)")
		.def("getYieldRushed", &CyCity::getYieldRushed, "int /*YieldTypes*/ ()")
		.def("calculateNetYield", &CyCity::calculateNetYield, "int (int /*YieldTypes*/)")
		.def("calculateActualYieldProduced", &CyCity::calculateActualYieldProduced, "int (int /*YieldTypes*/)")
		.def("calculateActualYieldConsumed", &CyCity::calculateActualYieldConsumed, "int (int /*YieldTypes*/)")
		
		// R&R, Robert Surcouf, Domestic Market display START
		.def("getYieldBuyPrice", &CyCity::getYieldBuyPrice, "int (int /*YieldTypes*/)")
		.def("getYieldDemand", &CyCity::getYieldDemand, "int (int /*YieldTypes*/)")
		// R&R, Robert Surcouf, Domestic Market display END
		
		// R&R, Robert Surcouf, No More Variables Hidden game option START
		.def("getMissionaryPlayer", &CyCity::getMissionaryPlayer, "int /*PlayerTypes*/ ()")
		.def("getMissionaryRate", &CyCity::getMissionaryRate, "int ()")
		// R&R, Robert Surcouf, No More Variables Hidden game option END
		.def("isEverOwned", &CyCity::isEverOwned, "bool ()")
		;
}
