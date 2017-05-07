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

void CyCityPythonInterface2(python::class_<CyCity>& x)
{
	OutputDebugString("Python Extension Module - CyCityPythonInterface2\n");

	x
		.def("hasTrait", &CyCity::hasTrait, "bool (TraitID) - does owner have TraitID?")
		.def("isHuman", &CyCity::isHuman, "bool () - is owner human?")
		.def("isVisible", &CyCity::isVisible, "bool (int /*TeamTypes*/ eTeam, bool bDebug)")

		.def("isCapital", &CyCity::isCapital, "bool () - is city the owners capital?")
		.def("isCoastal", &CyCity::isCoastal, "bool (int) - is the city on the coast?")
		.def("isDisorder", &CyCity::isDisorder, "bool () - is the city in disorder?")
		.def("extraPopulation", &CyCity::extraPopulation, "int () - # of extra/available citizens")
		.def("foodConsumption", &CyCity::foodConsumption, "int (int iExtra)")
		.def("foodDifference", &CyCity::foodDifference, "int () - result of getYieldRate(Food) - foodConsumption()")
		.def("growthThreshold", &CyCity::growthThreshold, "int () - value needed for growth")
		.def("productionLeft", &CyCity::productionLeft, "int ()")
		.def("hurryCost", &CyCity::hurryCost, "int (bool bExtra, bool bIgnoreNew)")
		.def("hurryGold", &CyCity::hurryGold, "int (HurryID) - total value of gold when hurrying")
		.def("hurryPopulation", &CyCity::hurryPopulation, "int (HurryID) - value of each pop when hurrying")
		.def("hurryProduction", &CyCity::hurryProduction, "int (HurryID)")
		.def("maxHurryPopulation", &CyCity::maxHurryPopulation, "int ()")
		.def("cultureDistance", &CyCity::cultureDistance, "int (iDX, iDY) - culture distance")
		.def("isHasBuilding", &CyCity::isHasBuilding, "bool (int iBuildingID)")
		.def("getProfessionOutput", &CyCity::getProfessionOutput, "int (int /*ProfessionTypes*/ iProfession, CyUnit* pUnit)")

		.def("getID", &CyCity::getID, "int () - index ID # for the city - use with pPlayer.getCity(ID) to obtain city instance")
		.def("getX", &CyCity::getX, "int () - X coordinate for the cities plot")
		.def("getY", &CyCity::getY, "int () - Y coordinate for the cities plot")
		.def("at", &CyCity::at, "bool (iX, iY) - is the city at (iX, iY) ?")
		.def("atPlot", &CyCity::atPlot, "bool (CyPlot) - is pPlot the cities plot?")
		.def("plot", &CyCity::plot, python::return_value_policy<python::manage_new_object>(), "CyPlot () - returns cities plot instance")
		.def("area", &CyCity::area, python::return_value_policy<python::manage_new_object>(), "CyArea() () - returns CyArea instance for location of city")
		.def("waterArea", &CyCity::waterArea, python::return_value_policy<python::manage_new_object>(), "CyArea* ()")
		.def("getRallyPlot", &CyCity::getRallyPlot, python::return_value_policy<python::manage_new_object>(), "CyPlot () - returns city's rally plot instance")
		.def("getGameTurnFounded", &CyCity::getGameTurnFounded, "int () - GameTurn the city was founded")

		.def("getGameTurnAcquired", &CyCity::getGameTurnAcquired, "int ()")
		.def("getPopulation", &CyCity::getPopulation, "int () - total city population")
		.def("setPopulation", &CyCity::setPopulation, "void (int iNewValue) - sets the city population to iNewValue")
		.def("changePopulation", &CyCity::changePopulation, "void (int iChange) - adjusts the city population by iChange")
		.def("getRealPopulation", &CyCity::getRealPopulation, "int () - total city population in \"real\" numbers")
		.def("getHighestPopulation", &CyCity::getHighestPopulation, "int () ")
		.def("setHighestPopulation", &CyCity::setHighestPopulation, "void (iNewValue)")
		.def("isWorksWater", &CyCity::isWorksWater, "bool () - is city able to work water plots?")
		.def("changeHealRate", &CyCity::changeHealRate, "void (int iChange) - changes the heal rate of this city to iChange")

		.def("getFood", &CyCity::getFood, "int () - stored food")
		.def("setFood", &CyCity::setFood, "void (iNewValue) - set stored food to iNewValue")
		.def("changeFood", &CyCity::changeFood, "void (iChange) - adjust stored food by iChange")
		.def("getFoodKept", &CyCity::getFoodKept, "int ()")
		.def("getMaxFoodKeptPercent", &CyCity::getMaxFoodKeptPercent, "int ()")
		.def("getOverflowProduction", &CyCity::getOverflowProduction, "int () - value of overflow production")
		.def("setOverflowProduction", &CyCity::setOverflowProduction, "void (iNewValue) - set overflow production to iNewValue")
		.def("getMilitaryProductionModifier", &CyCity::getMilitaryProductionModifier, "int () - value of adjustments to military production")
		.def("getBuildingDefense", &CyCity::getBuildingDefense, "int () - building defense")
		.def("getBuildingBombardDefense", &CyCity::getBuildingBombardDefense, "int () - building defense")
		.def("getFreeExperience", &CyCity::getFreeExperience, "int () - # of free experience newly trained units receive")
		.def("getDefenseDamage", &CyCity::getDefenseDamage, "int () - value of damage city defenses can receive")
		.def("changeDefenseDamage", &CyCity::changeDefenseDamage, "void (iChange) - adjust damage value by iChange")
		.def("isBombardable", &CyCity::isBombardable, "bool (CyUnit* pUnit)")
		.def("getTotalDefense", &CyCity::getTotalDefense, "int ()")
		.def("getDefenseModifier", &CyCity::getDefenseModifier, "int ()")

		
		;
}
