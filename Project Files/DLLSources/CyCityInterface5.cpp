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

void CyCityPythonInterface5(python::class_<CyCity>& x)
{
	OutputDebugString("Python Extension Module - CyCityPythonInterface5\n");

	x
		.def("getScriptData", &CyCity::getScriptData, "str () - Get stored custom data (via pickle)")
		.def("setScriptData", &CyCity::setScriptData, "void (str) - Set stored custom data (via pickle)")

		.def("getBuildingYieldChange", &CyCity::getBuildingYieldChange, "int (int /*BuildingClassTypes*/ eBuildingClass, int /*YieldTypes*/ eYield)")
		.def("setBuildingYieldChange", &CyCity::setBuildingYieldChange, "void (int /*BuildingClassTypes*/ eBuildingClass, int /*YieldTypes*/ eYield, int iChange)")

		.def("getLiberationPlayer", &CyCity::getLiberationPlayer, "int ()")
		.def("liberate", &CyCity::liberate, "void ()")

		.def("isScoutVisited", &CyCity::isScoutVisited, "bool isScoutVisited(int /*TeamTypes*/ eTeam)")
		.def("getMaxYieldCapacity", &CyCity::getMaxYieldCapacity, "int ()")

		.def("getPopulationUnitByIndex", &CyCity::getPopulationUnitByIndex, python::return_value_policy<python::manage_new_object>(), "CyUnit* getPopulationUnitByIndex(int)")
		.def("getPopulationUnitById", &CyCity::getPopulationUnitById, python::return_value_policy<python::manage_new_object>(), "CyUnit* getPopulationUnitById(int)")
		.def("getPopulationUnitIndex", &CyCity::getPopulationUnitIndex, "int (CyUnit* pUnit)")
		.def("canTeach", &CyCity::canTeach, "bool (int /*UnitTypes*/ iUnit)")
		.def("getTeachUnitClass", &CyCity::getTeachUnitClass, "int /*UnitClass*/()")
		.def("getRebelPercent", &CyCity::getRebelPercent, "int ()")
		.def("getRebelSentiment", &CyCity::getRebelSentiment, "int ()")
		.def("setRebelSentiment", &CyCity::setRebelSentiment, "void (int)")

		// WTP, ray, new Harbour System - START
		.def("getCityHarbourSpace", &CyCity::getCityHarbourSpace, "int ()")
		.def("getCityHarbourSpaceUsed", &CyCity::getCityHarbourSpaceUsed, "int ()")
		.def("bShouldShowCityHarbourSystem", &CyCity::bShouldShowCityHarbourSystem, "bool ()")
		// WTP, ray, new Harbour System - END

		// WTP, ray, new Barracks System - START
		.def("getCityBarracksSpace", &CyCity::getCityBarracksSpace, "int ()")
		.def("getCityBarracksSpaceUsed", &CyCity::getCityBarracksSpaceUsed, "int ()")
		.def("bShouldShowCityBarracksSystem", &CyCity::bShouldShowCityBarracksSystem, "bool ()")
		// WTP, ray, new Barracks System - END

		// R&R, ray, Health - START
		.def("getCityHealth", &CyCity::getCityHealth, "int ()")
		.def("getCityHealthChange", &CyCity::getCityHealthChange, "int ()")
		.def("setCityHealth", &CyCity::setCityHealth, "void (int)")
		.def("changeCityHealth", &CyCity::changeCityHealth, "void (int)")
		// R&R, ray, Health - END

		// WTP, ray, helper methods for Python Event System - Spawning Units and Barbarians on Plots - START
		.def("spawnOwnPlayerUnitOnPlotOfCity", &CyCity::spawnOwnPlayerUnitOnPlotOfCity, "void (int /*UnitClassTypes*/ iIndex)")
		.def("spawnBarbarianUnitOnPlotOfCity", &CyCity::spawnBarbarianUnitOnPlotOfCity, "void (int /*UnitClassTypes*/ iIndex)")
		.def("spawnOwnPlayerUnitOnAdjacentPlotOfCity", &CyCity::spawnOwnPlayerUnitOnAdjacentPlotOfCity, "void (int /*UnitClassTypes*/ iIndex)")
		.def("spawnBarbarianUnitOnAdjacentPlotOfCity", &CyCity::spawnBarbarianUnitOnAdjacentPlotOfCity, "void (int /*UnitClassTypes*/ iIndex)")
		.def("isOwnPlayerUnitOnAdjacentPlotOfCity", &CyCity::isOwnPlayerUnitOnAdjacentPlotOfCity, "bool (int /*UnitClassTypes*/ iIndex)")
		.def("isBarbarianUnitOnAdjacentPlotOfCity", &CyCity::isBarbarianUnitOnAdjacentPlotOfCity, "bool (int /*UnitClassTypes*/ iIndex)")
		// WTP, ray, helper methods for Python Event System - Spawning Units and Barbarians on Plots - END

		// WTP, ray, Happiness - START
		.def("getCityHappiness", &CyCity::getCityHappiness, "int ()")
		.def("setCityHappiness", &CyCity::setCityHappiness, "void (int)")
		.def("updateCityHappiness", &CyCity::updateCityHappiness, "void ()")

		.def("getCityUnHappiness", &CyCity::getCityUnHappiness, "int ()")
		.def("setCityUnHappiness", &CyCity::setCityUnHappiness, "void (int)")
		.def("updateCityUnHappiness", &CyCity::updateCityUnHappiness, "void ()")

		.def("getHappinessFromCrosses", &CyCity::getHappinessFromCrosses, "int ()")
		.def("getHappinessFromBells", &CyCity::getHappinessFromBells, "int ()")
		.def("getHappinessFromHealth", &CyCity::getHappinessFromHealth, "int ()")
		.def("getHappinessFromCulture", &CyCity::getHappinessFromCulture, "int ()")
		.def("getHappinessFromLaw", &CyCity::getHappinessFromLaw, "int ()")
		.def("getHappinessFromEducation", &CyCity::getHappinessFromEducation, "int ()")
		.def("getHappinessFromDomesticDemandsFulfilled", &CyCity::getHappinessFromDomesticDemandsFulfilled, "int ()")
		.def("getHappinessFromTreaties", &CyCity::getHappinessFromTreaties, "int ()")

		.def("getUnhappinessFromPopulation", &CyCity::getUnhappinessFromPopulation, "int ()")
		.def("getUnhappinessFromCrime", &CyCity::getUnhappinessFromCrime, "int ()")
		.def("getUnhappinessFromSlavery", &CyCity::getUnhappinessFromSlavery, "int ()")
		.def("getUnhappinessFromWars", &CyCity::getUnhappinessFromWars, "int ()")
		.def("getUnhappinessFromMissingDefense", &CyCity::getUnhappinessFromMissingDefense, "int ()")
		.def("getUnhappinessFromTaxRate", &CyCity::getUnhappinessFromTaxRate, "int ()")
		// WTP, ray, Happiness - END

		// WTP, ray, Crime and Law - START
		.def("getCityLaw", &CyCity::getCityLaw, "int ()")
		.def("setCityLaw", &CyCity::setCityLaw, "void (int)")
		.def("updateCityLaw", &CyCity::updateCityLaw, "void ()")

		.def("getCityCrime", &CyCity::getCityCrime, "int ()")
		.def("setCityCrime", &CyCity::setCityCrime, "void (int)")
		.def("updateCityCrime", &CyCity::updateCityCrime, "void ()")

		.def("getLawFromCityDefenders", &CyCity::getLawFromCityDefenders, "int ()")
		.def("getLawFromCrosses", &CyCity::getLawFromCrosses, "int ()")
		.def("getCrimeFromPopulation", &CyCity::getCrimeFromPopulation, "int ()")
		.def("getCrimeFromUnhappiness", &CyCity::getCrimeFromUnhappiness, "int ()")
		.def("getCrimeFromWars", &CyCity::getCrimeFromWars, "int ()")
		.def("getCrimBonusFactorFromOverflow", &CyCity::getCrimBonusFactorFromOverflow, "int ()")
		// WTP, ray, Crime and Law - END

		.def("isEuropeAccessable", &CyCity::isEuropeAccessable, "bool ()") // WTP, ray, fix for SailTo - for the City

		.def("getUnitWorkingPlot", &CyCity::getUnitWorkingPlot, python::return_value_policy<python::manage_new_object>(), "CyUnit* getUnitWorkingPlot(int /*PlotIndex*/)")
		.def("addPopulationUnit", &CyCity::addPopulationUnit, "void addPopulationUnit(CyUnit* pUnit, int /*ProfessionTypes*/ eProfession)")
		.def("removePopulationUnit", &CyCity::removePopulationUnit, "removePopulationUnit(CyUnit* pUnit, bool bDelete, int /*ProfessionTypes*/ eProfession)")

		.def("isExport", &CyCity::isExport, "bool isExport(int /*YieldTypes*/ eYield)")
		.def("addExport", &CyCity::addExport, "void addExport(int /*YieldTypes*/ eYield)")
		.def("removeExport", &CyCity::removeExport, "void removeExport(int /*YieldTypes*/ eYield)")
		.def("isImport", &CyCity::isImport, "bool isImport(int /*YieldTypes*/ eYield) const")
		.def("addImport", &CyCity::addImport, "void addImport(int /*YieldTypes*/ eYield)")
		.def("removeImport", &CyCity::removeImport, "void removeImport(int /*YieldTypes*/ eYield)")
		.def("getMaintainLevel", &CyCity::getMaintainLevel, "int getMaintainLevel(int /*YieldTypes*/ eYield)")
		// R&R mod, vetiarvind, max yield import limit - start			
		.def("getImportsLimit", &CyCity::getImportsLimit, "int getImportsLimit(int /*YieldTypes*/ eYield)")
		// R&R mod, vetiarvind, max yield import limit - end
		// transport feeder - start - Nightinggale
		.def("isImportFeeder", &CyCity::isImportFeeder, "bool isImportFeeder(int /*YieldTypes*/ eYield) const")
		.def("isAutoImportStopped", &CyCity::isAutoImportStopped, "bool isAutoImportStopped(int /*YieldTypes*/ eYield) const")
		.def("getAutoMaintainThreshold", &CyCity::getAutoMaintainThreshold, "int getAutoMaintainThreshold(int /*YieldTypes*/ eYield) const")
		// transport feeder - end - Nightinggale

		.def("isOrderWaitingForYield", &CyCity::isOrderWaitingForYield, "python::tuple isOrderWaitingForYield(int /*YieldTypes*/ eYield)")
		.def("getCenterPlotTerrainType", &CyCity::getCenterPlotTerrainType, "int /*TerrainTypes*/ getCenterPlotTerrainType()")// WTP, ray, Center Plot specific Backgrounds - Start
		
		.def("isAccessibleByShip", &CyCity::isAccessibleByShip, "bool isAccessibleByShip(CyUnit* pUnit)")
		;
}
