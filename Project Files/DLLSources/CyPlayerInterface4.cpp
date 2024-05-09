#include "CvGameCoreDLL.h"
#include "CyPlayer.h"
#include "CyUnit.h"
#include "CyCity.h"
#include "CyPlot.h"
#include "CySelectionGroup.h"
#include "CyArea.h"
#include "CyTradeRoute.h"
#include "CyData.h"

#include <boost/python/manage_new_object.hpp>
#include <boost/python/class.hpp>
#include <boost/python/scope.hpp>

//
// published python interface for CyPlayer
//
void CyPlayerPythonInterface4(python::class_<CyPlayer>& x)
{
	OutputDebugString("Python Extension Module - CyPlayerPythonInterface4\n");
	// set the docstring of the current module scope
	python::scope().attr("__doc__") = "Civilization IV Player Class";
	x
		.def("getCombatExperience", &CyPlayer::getCombatExperience, "int () - Combat experience used to produce Warlords")
		.def("changeCombatExperience", &CyPlayer::changeCombatExperience, "void (int) - Combat experience used to produce Warlords")
		.def("setCombatExperience", &CyPlayer::setCombatExperience, "void (int) - Combat experience used to produce Warlords")
		.def("getSeaCombatExperience", &CyPlayer::getSeaCombatExperience, "int () - Combat experience used to produce Admirals")
		.def("changeSeaCombatExperience", &CyPlayer::changeSeaCombatExperience, "void (int) - Combat experience used to produce Admirals")
		.def("setSeaCombatExperience", &CyPlayer::setSeaCombatExperience, "void (int) - Combat experience used to produce Admirals")
		.def("addCityName", &CyPlayer::addCityName, "void (std::wstring szName)")
		.def("getNumCityNames", &CyPlayer::getNumCityNames, "int ()")
		.def("getCityName", &CyPlayer::getCityName, "std::wstring (int iIndex)")
		.def("firstCity", &CyPlayer::firstCity, "tuple(CyCity, int iterOut) (bool bReverse) - gets the first city")
		.def("nextCity", &CyPlayer::nextCity, "tuple(CyCity, int iterOut) (int iterIn, bool bReverse) - gets the next city")
		.def("getNumCities", &CyPlayer::getNumCities, "int ()")
		.def("getCity", &CyPlayer::getCity, python::return_value_policy<python::manage_new_object>(), "CyCity* (int iID)")
		.def("firstUnit", &CyPlayer::firstUnit, "tuple(CyUnit, int iterOut) (bool bReverse) - gets the first unit")
		.def("nextUnit", &CyPlayer::nextUnit, "tuple(CyUnit, int iterOut) (int iterIn, bool bReverse) - gets the next unit")
		.def("getNumUnits", &CyPlayer::getNumUnits, "int ()")
		.def("getUnit", &CyPlayer::getUnit, python::return_value_policy<python::manage_new_object>(), "CyUnit* (int iID)")
		.def("getNumEuropeUnits", &CyPlayer::getNumEuropeUnits, "int ()")
		.def("getEuropeUnit", &CyPlayer::getEuropeUnit, python::return_value_policy<python::manage_new_object>(), "CyUnit* (int iIndex)")
		.def("getEuropeUnitById", &CyPlayer::getEuropeUnitById, python::return_value_policy<python::manage_new_object>(), "CyUnit* (int iId)")
		.def("loadUnitFromEurope", &CyPlayer::loadUnitFromEurope, "void (CyUnit* pUnit, CyUnit* pTransport)")
		.def("unloadUnitToEurope", &CyPlayer::unloadUnitToEurope, "void (CyUnit* pUnit)")
		.def("firstSelectionGroup", &CyPlayer::firstSelectionGroup, "tuple(CySelectionGroup, int iterOut) (bool bReverse) - gets the first selectionGroup")
		.def("nextSelectionGroup", &CyPlayer::nextSelectionGroup, "tuple(CySelectionGroup, int iterOut) (int iterIn, bool bReverse) - gets the next selectionGroup")
		.def("getNumSelectionGroups", &CyPlayer::getNumSelectionGroups, "int ()")
		.def("getSelectionGroup", &CyPlayer::getSelectionGroup, python::return_value_policy<python::manage_new_object>(), "CvSelectionGroup* (int iID)")
		.def("countNumTravelUnits", &CyPlayer::countNumTravelUnits, "int (int /*UnitTravelStates*/ eState, int /*DomainTypes*/ eDomain)")
		.def("trigger", &CyPlayer::trigger, "void (/*EventTriggerTypes*/int eEventTrigger)")
		.def("getEventOccured", &CyPlayer::getEventOccured, python::return_value_policy<python::reference_existing_object>(), "EventTriggeredData* (int /*EventTypes*/ eEvent)")
		.def("resetEventOccured", &CyPlayer::resetEventOccured, "void (int /*EventTypes*/ eEvent)")
		.def("getEventTriggered", &CyPlayer::getEventTriggered, python::return_value_policy<python::reference_existing_object>(), "EventTriggeredData* (int iID)")
		.def("initTriggeredData", &CyPlayer::initTriggeredData, python::return_value_policy<python::reference_existing_object>(), "EventTriggeredData* (int eEventTrigger, bool bFire, int iCityId, int iPlotX, int iPlotY, PlayerTypes eOtherPlayer, int iOtherPlayerCityId, int iUnitId, BuildingTypes eBuilding)")
		.def("getEventTriggerWeight", &CyPlayer::getEventTriggerWeight, "int getEventTriggerWeight(int eEventTrigger)")
		.def("isEurope", &CyPlayer::isEurope, "bool ()")
		.def("isInRevolution", &CyPlayer::isInRevolution, "bool ()")
		.def("addTradeRoute", &CyPlayer::addTradeRoute, "int (int iSourceCityOwner, int iSourceCityId, int iDestinationCityOwner, int iDestinationCityId, int /*YieldTypes*/ eYield)")
		.def("removeTradeRoute", &CyPlayer::removeTradeRoute, "bool (int iId)")
		.def("getTradeRoute", &CyPlayer::getTradeRoute, python::return_value_policy<python::manage_new_object>(), "CyTradeRoute (int iId)")
		.def("getNumTradeRoutes", &CyPlayer::getNumTradeRoutes, "int ()")
		.def("getTradeRouteByIndex", &CyPlayer::getTradeRouteByIndex, python::return_value_policy<python::manage_new_object>(), "CyTradeRoute (int iIndex)")
		.def("editTradeRoute", &CyPlayer::editTradeRoute, "void (int iId, int iSourceCityOwner, int iSourceCityId, int iDestinationCityOwner, int iDestinationCityId, int /*YieldTypes*/ eYield)")
		.def("canLoadYield", &CyPlayer::canLoadYield, "	bool canLoadYield(int /*PlayerTypes*/ eCityPlayer)")
		.def("canUnloadYield", &CyPlayer::canUnloadYield, "bool canUnloadYield(int /*PlayerTypes*/ eCityPlayer) ")
		.def("getYieldEquipmentAmount", &CyPlayer::getYieldEquipmentAmount, "int (int iProfession, int iYield)")
		.def("addRevolutionEuropeUnit", &CyPlayer::addRevolutionEuropeUnit, "void addRevolutionEuropeUnit(int /*UnitTypes*/ eUnit, int /*ProfessionTypes*/ eProfession)")
		.def("getNumTradeMessages", &CyPlayer::getNumTradeMessages, "int ()")
		.def("getTradeMessage", &CyPlayer::getTradeMessage, "string (int)")

		.def("getSpecialBuildingTypes", &CyPlayer::getSpecialBuildingTypes, python::return_value_policy<python::manage_new_object>(), "CyInfoArray ()")
		.def("getStoredYieldTypes", &CyPlayer::getStoredYieldTypes, python::return_value_policy<python::manage_new_object>(), "CyInfoArray ()")
		.def("getDomesticDemandYieldTypes", &CyPlayer::getDomesticDemandYieldTypes, python::return_value_policy<python::manage_new_object>(), "CyInfoArray ()")
		.def("getTeachUnitTypes", &CyPlayer::getTeachUnitTypes, python::return_value_policy<python::manage_new_object>(), "CyInfoArray (int)")
		.def("getMaxTeachLevel", &CyPlayer::getMaxTeachLevel, "int ()")

		.def("getCivEffectCount", &CyPlayer::getCivEffectCount, "int (CivEffectTypes eCivEffect)")
		.def("getNumUnitsOnDock", &CyPlayer::getNumUnitsOnDock, "unsigned int ()")
		;
}
