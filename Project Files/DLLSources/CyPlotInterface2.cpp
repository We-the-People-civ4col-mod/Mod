#include "CvGameCoreDLL.h"
#include "CyPlot.h"
#include "CyCity.h"
#include "CyArea.h"
#include "CyUnit.h"
#include "CvPlot.h"

#include <boost/python/manage_new_object.hpp>
#include <boost/python/class.hpp>

//
// published python interface for CyPlot
//
void CyPlotPythonInterface2(python::class_<CyPlot>& x)
{
	OutputDebugString("Python Extension Module - CyPlotPythonInterface2\n");
	x
		.def("isNOfRiver", &CyPlot::isNOfRiver, "bool ()")
		.def("setNOfRiver", &CyPlot::setNOfRiver, "void (bool bNewValue, CardinalDirectionTypes eRiverDir)")
		.def("isWOfRiver", &CyPlot::isWOfRiver, "bool ()")
		.def("setWOfRiver", &CyPlot::setWOfRiver, "void (bool bNewValue, CardinalDirectionTypes eRiverDir)")
		.def("getRiverWEDirection", &CyPlot::getRiverWEDirection, "CardinalDirectionTypes ()")
		.def("getRiverNSDirection", &CyPlot::getRiverNSDirection, "CardinalDirectionTypes ()")
		.def("isPotentialCityWork", &CyPlot::isPotentialCityWork, "bool ()")
		.def("isPotentialCityWorkForArea", &CyPlot::isPotentialCityWorkForArea, "bool (CyArea* pArea)")
		.def("isFlagDirty", &CyPlot::isFlagDirty, "bool ()")
		.def("setFlagDirty", &CyPlot::setFlagDirty, "void (bool bNewValue)")
		.def("getOwner", &CyPlot::getOwner, "int ()")
		.def("setOwner", &CyPlot::setOwner, "void (int /*PlayerTypes*/ eNewValue)")
		.def("getPlotType", &CyPlot::getPlotType, "int ()")
		.def("isWater", &CyPlot::isWater, "bool ()")
		.def("isEurope", &CyPlot::isEurope, "bool ()")
		.def("getEurope", &CyPlot::getEurope, "int /*EuropeTypes*/ ()")
		.def("setEurope", &CyPlot::setEurope, "void (int /*EuropeTypes*/)")
		.def("isFlatlands", &CyPlot::isFlatlands, "bool ()")
		.def("isHills", &CyPlot::isHills, "bool ()")
		.def("isPeak", &CyPlot::isPeak, "bool ()")
		.def("setPlotType", &CyPlot::setPlotType, "void (PlotTypes eNewValue, bool bRecalculate, bool bRebuildGraphics)")
		.def("getTerrainType", &CyPlot::getTerrainType, "int ()")
		.def("setTerrainType", &CyPlot::setTerrainType, "void (TerrainTypes eNewValue, bool bRecalculate, bool bRebuildGraphics)")
		.def("getFeatureType", &CyPlot::getFeatureType, "int ()")
		.def("setFeatureType", &CyPlot::setFeatureType, "void (int /*FeatureTypes*/ eNewValue, int iVariety)")
		.def("getBonusType", &CyPlot::getBonusType, "int ()")
		.def("setBonusType", &CyPlot::setBonusType, "void (int eNewValue)")
		.def("getImprovementType", &CyPlot::getImprovementType, "int ()")
		.def("setImprovementType", &CyPlot::setImprovementType, "void (int eNewValue)")
		.def("getRouteType", &CyPlot::getRouteType, "int ()")
		.def("setRouteType", &CyPlot::setRouteType, "void (int (RouteTypes) eNewValue)")
		.def("getPlotCity", &CyPlot::getPlotCity, python::return_value_policy<python::manage_new_object>(), "CyCity* ()")
		.def("getWorkingCity", &CyPlot::getWorkingCity, python::return_value_policy<python::manage_new_object>(), "CyCity* ()")
		.def("getWorkingCityOverride", &CyPlot::getWorkingCityOverride, python::return_value_policy<python::manage_new_object>(), "CyCity* ()")
		.def("getRiverID", &CyPlot::getRiverID, "int ()")
		.def("setRiverID", &CyPlot::setRiverID, "void (int)")
		.def("getMinOriginalStartDist", &CyPlot::getMinOriginalStartDist, "int ()")
		.def("getRiverCrossingCount", &CyPlot::getRiverCrossingCount, "int ()")
		.def("getYield", &CyPlot::getYield, "int (YieldTypes eIndex)")
		.def("calculateNatureYield", &CyPlot::calculateNatureYield, "int (int (YieldTypes) eYield, int (TeamTypes) eTeam, bool bIgnoreFeature)")
		.def("calculateBestNatureYield", &CyPlot::calculateBestNatureYield, "int (int (YieldTypes) eYield, int (TeamTypes) eTeam)")
		.def("calculateTotalBestNatureYield", &CyPlot::calculateTotalBestNatureYield, "int (int (TeamTypes) eTeam)")
		.def("calculateImprovementYieldChange", &CyPlot::calculateImprovementYieldChange, "int (int (ImprovementTypes) eImprovement, int (YieldTypes) eYield, int (PlayerTypes) ePlayer, bool bOptimal)")
		.def("calculateYield", &CyPlot::calculateYield, "int (YieldTypes eYield, bool bDisplay)")
		.def("hasYield", &CyPlot::hasYield, "bool ()")
		.def("getCulture", &CyPlot::getCulture, "int (int /*PlayerTypes*/ eIndex)")
		.def("countTotalCulture", &CyPlot::countTotalCulture, "int ()")
		.def("findHighestCultureTeam", &CyPlot::findHighestCultureTeam, "int /*TeamTypes*/ ()")
		.def("calculateCulturePercent", &CyPlot::calculateCulturePercent, "int (int /*PlayerTypes*/ eIndex)")
		.def("calculateTeamCulturePercent", &CyPlot::calculateTeamCulturePercent, "int (int /*TeamTypes*/ eIndex)")
		.def("setCulture", &CyPlot::setCulture, "void (int /*PlayerTypes*/ eIndex, int iNewValue, bool bUpdate)")
		.def("changeCulture", &CyPlot::changeCulture, "void (int /*PlayerTypes*/ eIndex, int iChange, bool bUpdate)")
		.def("getFoundValue", &CyPlot::getFoundValue, "int (int /*PlayerTypes*/ eIndex)")
		.def("isBestAdjacentFound", &CyPlot::isBestAdjacentFound, "bool (int /*PlayerTypes*/ eIndex)")
		.def("getPlayerCityRadiusCount", &CyPlot::getPlayerCityRadiusCount, "int (int /*PlayerTypes*/ eIndex)")
		.def("isPlayerCityRadius", &CyPlot::isPlayerCityRadius, "bool (int /*PlayerTypes*/ eIndex)")
		.def("getVisibilityCount", &CyPlot::getVisibilityCount, "int (int /*TeamTypes*/ eTeam)")
		.def("getRevealedOwner", &CyPlot::getRevealedOwner, "int (int (TeamTypes) eTeam, bool bDebug)")
		.def("getRevealedTeam", &CyPlot::getRevealedTeam, "int (int /*TeamTypes*/ eTeam, bool bDebug)")
		.def("isRiverCrossing", &CyPlot::isRiverCrossing, "bool (DirectionTypes eIndex)")
		.def("isRevealed", &CyPlot::isRevealed, "bool (int /*TeamTypes*/ eTeam, bool bDebug)")
		.def("setRevealed", &CyPlot::setRevealed, "void (int /*TeamTypes*/ eTeam, bool bNewValue, bool bTerrainOnly, int /*TeamTypes*/ eFromTeam)")
		.def("getRevealedImprovementType", &CyPlot::getRevealedImprovementType, "int (int /*TeamTypes*/ eTeam, bool bDebug)")
		.def("getRevealedRouteType", &CyPlot::getRevealedRouteType, "int (int /*TeamTypes*/ eTeam, bool bDebug)")
		.def("getBuildProgress", &CyPlot::getBuildProgress, "int (int /*BuildTypes*/ eBuild)")
		.def("changeBuildProgress", &CyPlot::changeBuildProgress, "bool (int /*BuildTypes*/ eBuild, int iChange, int /*TeamTypes*/ eTeam)")
		.def("getCultureRangeCities", &CyPlot::getCultureRangeCities, "int (int /*PlayerTypes*/ eOwnerIndex, int iRangeIndex)")
		.def("isCultureRangeCity", &CyPlot::isCultureRangeCity, "bool (int /*PlayerTypes*/ eOwnerIndex, int iRangeIndex)")
		.def("getInvisibleVisibilityCount", &CyPlot::getInvisibleVisibilityCount, "int (int (TeamTypes eTeam), int (InvisibleTypes) eInvisible)")
		.def("isInvisibleVisible", &CyPlot::isInvisibleVisible, "int (int (TeamTypes eTeam), int (InvisibleTypes) eInvisible)")
		.def("changeInvisibleVisibilityCount", &CyPlot::changeInvisibleVisibilityCount, "int (int (TeamTypes eTeam), int (InvisibleTypes) eInvisible, int iChange)")
		.def("getNumUnits", &CyPlot::getNumUnits, "int ()")
		.def("getUnit", &CyPlot::getUnit, python::return_value_policy<python::manage_new_object>(), "CyUnit* (int iIndex)")
		.def("getScriptData", &CyPlot::getScriptData, "str () - Get stored custom data")
		.def("setScriptData", &CyPlot::setScriptData, "void (str) - Set stored custom data")
		// Super Forts begin *canal* *choke*
		.def("getCanalValue", &CyPlot::getCanalValue, "int ()")
		.def("calculateCanalValue", &CyPlot::calculateCanalValue, "void ()")
		.def("getChokeValue", &CyPlot::getChokeValue, "int ()")
		.def("calculateChokeValue", &CyPlot::calculateChokeValue, "void ()")
		// Super Forts end

		// WTP, ray, helper methods for Python Event System - Spawning Units and Barbarians on Plots - START
		.def("spawnPlayerUnitOnPlot", &CyPlot::spawnPlayerUnitOnPlot, "void (int /*PlayerTypes*/ iPlayer, int /*UnitClassTypes*/ iIndex)")
		.def("spawnBarbarianUnitOnPlot", &CyPlot::spawnBarbarianUnitOnPlot, "void (int /*UnitClassTypes*/ iIndex)")
		.def("spawnPlayerUnitOnAdjacentPlot", &CyPlot::spawnPlayerUnitOnAdjacentPlot, "void (int /*PlayerTypes*/ iPlayer, int /*UnitClassTypes*/ iIndex)")
		.def("spawnBarbarianUnitOnAdjacentPlot", &CyPlot::spawnBarbarianUnitOnAdjacentPlot, "void (int /*UnitClassTypes*/ iIndex)")
		.def("isPlayerUnitOnAdjacentPlot", &CyPlot::isPlayerUnitOnAdjacentPlot, "bool (int /*PlayerTypes*/ iPlayer, int /*UnitClassTypes*/ iIndex)")
		.def("isBarbarianUnitOnAdjacentPlot", &CyPlot::isBarbarianUnitOnAdjacentPlot, "bool (int /*UnitClassTypes*/ iIndex)")
		// WTP, ray, helper methods for Python Event System - Spawning Units and Barbarians on Plots - END

	;
}
