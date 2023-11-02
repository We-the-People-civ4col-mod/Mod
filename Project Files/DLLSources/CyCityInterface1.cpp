#include "CvGameCoreDLL.h"
#include "CyCity.h"
#include "CyUnit.h"
#include "CyPlot.h"
#include "CyArea.h"
#include "CvInfos.h"

#include <boost/python/manage_new_object.hpp>
#include <boost/python/class.hpp>

//
// published python interface for CyCity
//

void CyCityPythonInterface1(python::class_<CyCity>& x)
{
	OutputDebugString("Python Extension Module - CyCityPythonInterface1\n");

	x
		.def("isNone", &CyCity::isNone, "void () - is the instance valid?")
		.def("kill", &CyCity::kill, "void () - kill the city")
		.def("doTask", &CyCity::doTask, "void (int eTaskTypes, int iData1, int iData2, bool bOption) - Enacts the TaskType passed")
		.def("chooseProduction", &CyCity::chooseProduction, "void (int /*UnitTypes*/ eTrainUnit, int /*BuildingTypes*/ eConstructBuilding, bool bFinish, bool bFront) - Chooses production for a city")
		
		.def("createGreatGeneral", &CyCity::createGreatGeneral, "void (int /*UnitTypes*/ eGreatGeneralUnit, bool bIncrementThreshold) - Creates a great General in this city and whether it should increment the threshold to the next level")
		.def("createGreatAdmiral", &CyCity::createGreatAdmiral, "void (int /*UnitTypes*/ eGreatAdmiralUnit, bool bIncrementThreshold) - Creates a great Admiral in this city and whether it should increment the threshold to the next level")

		.def("getCityPlotIndex", &CyCity::getCityPlotIndex, "int (CyPlot* pPlot)")
		.def("getCityIndexPlot", &CyCity::getCityIndexPlot, python::return_value_policy<python::manage_new_object>(), "CyPlot* (int iIndex)")
		.def("canWork", &CyCity::canWork, "bool (CyPlot*) - can the city work the plot?")
		.def("clearWorkingOverride", &CyCity::clearWorkingOverride, "void (int iIndex)")
		.def("countNumImprovedPlots", &CyCity::countNumImprovedPlots, "int ()")
		.def("countNumWaterPlots", &CyCity::countNumWaterPlots, "int ()")
		.def("countNumRiverPlots", &CyCity::countNumRiverPlots, "int ()")

		.def("findPopulationRank", &CyCity::findPopulationRank, "int ()")
		.def("findBaseYieldRateRank", &CyCity::findBaseYieldRateRank, "int (int /*YieldTypes*/ eYield)")
		.def("findYieldRateRank", &CyCity::findYieldRateRank, "int (int /*YieldTypes*/ eYield)")

		.def("allUpgradesAvailable", &CyCity::allUpgradesAvailable, "int UnitTypes (int eUnit, int iUpgradeCount)")
		.def("canTrain", &CyCity::canTrain, "bool (int eUnit, bool bContinue, bool bTestVisible)")
		.def("canConstruct", &CyCity::canConstruct, "bool (int eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost)")
		.def("canConvince", &CyCity::canConvince, "bool (int eFatherPointType, bool bContinue, bool bTestVisible)")
		.def("getFoodTurnsLeft", &CyCity::getFoodTurnsLeft, "int () - how many food turns remain?")
		.def("isProduction", &CyCity::isProduction, "bool () - is city producing?")
		.def("isProductionUnit", &CyCity::isProductionUnit, "bool () - is city training a unit?")
		.def("isProductionBuilding", &CyCity::isProductionBuilding, "bool () - is city constructing a building?")

		.def("canContinueProduction", &CyCity::canContinueProduction, "bool (OrderData order)")
		.def("getProductionExperience", &CyCity::getProductionExperience, "int (int /*UnitTypes*/ eUnit)")
		.def("addProductionExperience", &CyCity::addProductionExperience, "void (CyUnit* pUnit)")
		.def("getProductionUnit", &CyCity::getProductionUnit, "UnitID () - ID for unit that is being trained")
		.def("getProductionUnitAI", &CyCity::getProductionUnitAI, "int eUnitAIType ()")
		.def("getProductionBuilding", &CyCity::getProductionBuilding, "BuildingID () - ID for building that is under construction")
		.def("getProductionName", &CyCity::getProductionName, "str () - description of item that the city is working on")
		.def("getGeneralProductionTurnsLeft", &CyCity::getGeneralProductionTurnsLeft, "int - # of production turns left for the top order node item in a city...")
		.def("getProductionNameKey", &CyCity::getProductionNameKey, "str () - description of item that the city is working on")
		.def("getFirstUnitOrder", &CyCity::getFirstUnitOrder, "int (int /*UnitTypes*/ eUnit)")
		.def("getFirstBuildingOrder", &CyCity::getFirstBuildingOrder, "int (int /*BuildingTypes*/ eBuilding)")
		.def("getProduction", &CyCity::getProduction, "int () - returns the current production towards whatever is top of this city's OrderQueue")
		.def("getProductionNeeded", &CyCity::getProductionNeeded, "int (int /*YieldTypes*/) - # of production needed to complete construction")
		.def("getProductionTurnsLeft", &CyCity::getProductionTurnsLeft, "int () - # of turns remaining until item is completed")
		.def("getUnitProductionTurnsLeft", &CyCity::getUnitProductionTurnsLeft, "int (UnitID, int iNum) - # of turns remaining to complete UnitID")
		.def("getBuildingProductionTurnsLeft", &CyCity::getBuildingProductionTurnsLeft, "int (BuildingID, int iNum) - # of turns remaining to complete UnitID")
		.def("setProduction", &CyCity::setProduction, "void (int iNewValue)")
		.def("changeProduction", &CyCity::changeProduction, "void (int iChange)")
		.def("getProductionModifier", &CyCity::getProductionModifier, "int () - multiplier (if any) for item being produced")
		.def("getCurrentProductionDifference", &CyCity::getCurrentProductionDifference, "int (bool bOverflow)")
		.def("getUnitProductionModifier", &CyCity::getUnitProductionModifier, "int (UnitID) - production multiplier for UnitID")
		.def("getBuildingProductionModifier", &CyCity::getBuildingProductionModifier, "int (BuildingID) - production multiplier for BuildingID")
		
		.def("canHurry", &CyCity::canHurry, "bool (HurryTypes eHurry, bool bTestVisible = 0) - can player eHurry in this city?")
		.def("hurry", &CyCity::hurry, "void (HurryTypes eHurry) - forces the city to rush production using eHurry")
		.def("getHandicapType", &CyCity::getHandicapType, "HandicapType () - owners difficulty level")
		.def("getCivilizationType", &CyCity::getCivilizationType, "CivilizationID () - owners CivilizationID")
		.def("getPersonalityType", &CyCity::getPersonalityType, "int /*LeaderHeadTypes*/ ()")
		.def("getArtStyleType", &CyCity::getArtStyleType, "int /*ArtStyleTypes*/ ()")
		.def("getCitySizeType", &CyCity::getCitySizeType, "int /*CitySizeTypes*/ ()")

		// native advisor update - start - Nightinggale
		.def("getTeachUnit", &CyCity::getTeachUnit, "int /*UnitType*/ ()")
		// native advisor update - end - Nightinggale
		;
}
