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
		.def("getTeachUnitClass", &CyCity::getTeachUnitClass, "int /*UnitClass*/()")
		.def("getRebelPercent", &CyCity::getRebelPercent, "int ()")
		.def("getRebelSentiment", &CyCity::getRebelSentiment, "int ()")
		.def("setRebelSentiment", &CyCity::setRebelSentiment, "void (int)")

		// R&R, ray, Health - START
		.def("getCityHealth", &CyCity::getCityHealth, "int ()")
		.def("getCityHealthChange", &CyCity::getCityHealthChange, "int ()")
		.def("setCityHealth", &CyCity::setCityHealth, "void (int)")
		.def("changeCityHealth", &CyCity::changeCityHealth, "void (int)")
		// R&R, ray, Health - END

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
		;
}
