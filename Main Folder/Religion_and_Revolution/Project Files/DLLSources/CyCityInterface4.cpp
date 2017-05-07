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

void CyCityPythonInterface4(python::class_<CyCity>& x)
{
	OutputDebugString("Python Extension Module - CyCityPythonInterface4\n");

	x
		.def("isRevealed", &CyCity::isRevealed, "bool (int /*TeamTypes*/ eIndex, bool bDebug)")
		.def("setRevealed", &CyCity::setRevealed, "void (int /*TeamTypes*/ eIndex, bool bNewValue)")
		.def("getName", &CyCity::getName, "string () - city name")
		.def("getNameForm", &CyCity::getNameForm, "string () - city name")
		.def("getNameKey", &CyCity::getNameKey, "string () - city name")
		.def("setName", &CyCity::setName, "void (TCHAR szNewValue, bool bFound) - sets the name to szNewValue")
		.def("getBuildingProduction", &CyCity::getBuildingProduction, "int (BuildingID) - current production towards BuildingID")
		.def("setBuildingProduction", &CyCity::setBuildingProduction, "void (BuildingID, iNewValue) - set progress towards BuildingID as iNewValue")
		.def("changeBuildingProduction", &CyCity::changeBuildingProduction, "void (BuildingID, iChange) - adjusts progress towards BuildingID by iChange")
		.def("getBuildingProductionTime", &CyCity::getBuildingProductionTime, "int (int eIndex)")
		.def("setBuildingProductionTime", &CyCity::setBuildingProductionTime, "int (int eIndex, int iNewValue)")
		.def("changeBuildingProductionTime", &CyCity::changeBuildingProductionTime, "int (int eIndex, int iChange)")
		.def("getBuildingOriginalOwner", &CyCity::getBuildingOriginalOwner, "int (BuildingType) - index of original building owner")
		.def("getBuildingOriginalTime", &CyCity::getBuildingOriginalTime, "int (BuildingType) - original build date")
		.def("getUnitProduction", &CyCity::getUnitProduction, "int (UnitID) - gets current production towards UnitID")
		.def("setUnitProduction", &CyCity::setUnitProduction, "void (UnitID, iNewValue) - sets production towards UnitID as iNewValue")
		.def("changeUnitProduction", &CyCity::changeUnitProduction, "void (UnitID, iChange) - adjusts production towards UnitID by iChange")

		.def("getUnitCombatFreeExperience", &CyCity::getUnitCombatFreeExperience, "int (int /*UnitCombatTypes*/ eIndex)")
		.def("getFreePromotionCount", &CyCity::getFreePromotionCount, "int (int /*PromotionTypes*/ eIndex)")
		.def("isFreePromotion", &CyCity::isFreePromotion, "bool (int /*PromotionTypes*/ eIndex)")

		.def("isHasRealBuilding", &CyCity::isHasRealBuilding, "bool (BuildingID)")
		.def("setHasRealBuilding", &CyCity::setHasRealBuilding, "(BuildingID, bool)")
		.def("isHasFreeBuilding", &CyCity::isHasFreeBuilding, "bool (BuildingID)")

		.def("clearOrderQueue", &CyCity::clearOrderQueue, "void ()")
		.def("pushOrder", &CyCity::pushOrder, "void (OrderTypes eOrder, int iData1, int iData2, bool bSave, bool bPop, bool bAppend, bool bForce)")
		.def("popOrder", &CyCity::popOrder, "int (int iNum, bool bFinish, bool bChoose)")
		.def("getOrderQueueLength", &CyCity::getOrderQueueLength, "void ()")
		.def("getOrderFromQueue", &CyCity::getOrderFromQueue, python::return_value_policy<python::manage_new_object>(), "OrderData* (int iIndex)")

		.def("setWallOverridePoints", &CyCity::setWallOverridePoints, "setWallOverridePoints(const python::tuple& kPoints)")
		.def("getWallOverridePoints", &CyCity::getWallOverridePoints, "python::tuple getWallOverridePoints()")

		.def("AI_avoidGrowth", &CyCity::AI_avoidGrowth, "bool ()")
		.def("AI_isEmphasize", &CyCity::AI_isEmphasize, "bool (int iEmphasizeType)")
		.def("AI_getEmphasizeYieldCount", &CyCity::AI_getEmphasizeYieldCount, "int (int iEmphasizeType)")
		.def("AI_countBestBuilds", &CyCity::AI_countBestBuilds, "int (CyArea* pArea)")
		.def("AI_cityValue", &CyCity::AI_cityValue, "int ()")
		// R&R, Robert Surcouf, No More Variables Hidden game option START
		.def("AI_getDesiredYield", &CyCity::AI_getDesiredYield, "int /*YieldTypes*/ ()")
		// R&R, Robert Surcouf, No More Variables Hidden game option END
		;
}
