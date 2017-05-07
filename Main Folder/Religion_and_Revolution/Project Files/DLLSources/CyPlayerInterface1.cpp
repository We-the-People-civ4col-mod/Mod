#include "CvGameCoreDLL.h"
#include "CyPlayer.h"
#include "CyUnit.h"
#include "CyCity.h"
#include "CyPlot.h"
#include "CySelectionGroup.h"
#include "CyArea.h"
//# include <boost/python/manage_new_object.hpp>
//# include <boost/python/return_value_policy.hpp>
//# include <boost/python/scope.hpp>
//
// published python interface for CyPlayer
//
void CyPlayerPythonInterface1(python::class_<CyPlayer>& x)
{
	OutputDebugString("Python Extension Module - CyPlayerPythonInterface1\n");
	// set the docstring of the current module scope
	python::scope().attr("__doc__") = "Civilization IV Player Class";
	x
		.def("isNone", &CyPlayer::isNone, "checks for a null player")
		.def("startingPlotRange", &CyPlayer::startingPlotRange, "int ()")
		.def("findStartingPlot", &CyPlayer::findStartingPlot, python::return_value_policy<python::manage_new_object>(), "findStartingPlot(bool bRandomize) - Finds a starting plot for player")
		.def("initCity", &CyPlayer::initCity, python::return_value_policy<python::manage_new_object>(), "initCity( plotX, plotY ) - spawns a city at x,y")
		.def("acquireCity", &CyPlayer::acquireCity, "void (CyCity* pCity, bool bConquest, bool bTrade)")
		.def("killCities", &CyPlayer::killCities, "void ()")
		.def("getNewCityName", &CyPlayer::getNewCityName, "wstring ()")
		.def("initUnit", &CyPlayer::initUnit, python::return_value_policy<python::manage_new_object>(), "CyUnit* initUnit(UnitTypes iIndex, ProfessionTypes eProfession, plotX, plotY, UnitAITypes, DirectionTypes, iYieldStored)  - place Unit at X,Y   NOTE: Always use UnitAITypes.NO_UNITAI")
		.def("initEuropeUnit", &CyPlayer::initEuropeUnit, python::return_value_policy<python::manage_new_object>(), "CyUnit* initEuropeUnit(UnitTypes iIndex, UnitAITypes, DirectionTypes)")
		.def("killUnits", &CyPlayer::killUnits, "void ()")
		.def("hasTrait", &CyPlayer::hasTrait, "bool hasTrait(int /*TraitTypes*/ iIndex)")
		.def("isHuman", &CyPlayer::isHuman, "bool ()")
		.def("isNative", &CyPlayer::isNative, "bool ()")
		.def("getName", &CyPlayer::getName, "str ()")
		.def("getNameForm", &CyPlayer::getNameForm, "str ()")
		.def("getNameKey", &CyPlayer::getNameKey, "str ()")
		.def("getCivilizationDescription", &CyPlayer::getCivilizationDescription, "str() - returns the Civilization Description String")
		.def("getCivilizationShortDescription", &CyPlayer::getCivilizationShortDescription, "str() - returns the short Civilization Description")
		.def("getCivilizationDescriptionKey", &CyPlayer::getCivilizationDescriptionKey, "str() - returns the Civilization Description String")
		.def("getCivilizationShortDescriptionKey", &CyPlayer::getCivilizationShortDescriptionKey, "str() - returns the short Civilization Description")
		.def("getCivilizationAdjective", &CyPlayer::getCivilizationAdjective, "str() - returns the Civilization name in adjective form")
		.def("getCivilizationAdjectiveKey", &CyPlayer::getCivilizationAdjectiveKey, "str() - returns the Civilization name in adjective form")
		.def("getWorstEnemyName", &CyPlayer::getWorstEnemyName, "str () - returns the name of the worst enemy")
		.def("getArtStyleType", &CyPlayer::getArtStyleType, " int () - Returns the ArtStyleType for this player (e.g. European)")
		.def("getUnitButton", &CyPlayer::getUnitButton, " string (int eUnit) - Returns the unit button for this player")
		.def("findBestFoundValue", &CyPlayer::findBestFoundValue, " int () - Finds best found value")
		.def("countNumCoastalCities", &CyPlayer::countNumCoastalCities, "int ()")
		.def("countNumCoastalCitiesByArea", &CyPlayer::countNumCoastalCitiesByArea, "(int (CyArea* pArea)")
		.def("countTotalCulture", &CyPlayer::countTotalCulture, "int ()")
		.def("countTotalYieldStored", &CyPlayer::countTotalYieldStored, "int (int /*YieldTypes*/)")
		.def("countCityFeatures", &CyPlayer::countCityFeatures, "int (int /*FeatureTypes*/ eFeature) - Returns ?")
		.def("countNumBuildings", &CyPlayer::countNumBuildings, "int (int /*BuildingTypes*/ eBuilding) - Returns the number of buildings?")
		.def("canContact", &CyPlayer::canContact, "bool (int ePlayer)")
		.def("contact", &CyPlayer::contact, "void (int ePlayer)")
		.def("canTradeWith", &CyPlayer::canTradeWith, "bool (int ePlayer)")
		.def("canTradeItem", &CyPlayer::canTradeItem, "bool (int ePlayer, bool bTestDenial)")
		.def("getTradeDenial", &CyPlayer::getTradeDenial, "DenialTypes (int eWhoTo, TradeData item)")
		.def("canStopTradingWithTeam", &CyPlayer::canStopTradingWithTeam, "int (int /*TeamTypes*/ eTeam)")
		.def("stopTradingWithTeam", &CyPlayer::stopTradingWithTeam, "int (int /*TeamTypes*/ eTeam)")
		.def("killAllDeals", &CyPlayer::killAllDeals, "void ()")
		.def("findNewCapital", &CyPlayer::findNewCapital, "void ()")
		.def("canRaze", &CyPlayer::canRaze, "bool (CyCity pCity)")
		.def("raze", &CyPlayer::raze, "void (CyCity pCity)")
		.def("disband", &CyPlayer::disband, "void (CyCity pCity)")
		.def("canReceiveGoody", &CyPlayer::canReceiveGoody, "bool (CyPlot* pPlot, int /*GoodyTypes*/ eGoody, CyUnit* pUnit)")
		.def("receiveGoody", &CyPlayer::receiveGoody, "void (CyPlot* pPlot, int /*GoodyTypes*/ eGoody, CyUnit* pUnit)")
		;
}
