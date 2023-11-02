#include "CvGameCoreDLL.h"
#include "CyPlayer.h"
#include "CyUnit.h"
#include "CyCity.h"
#include "CyPlot.h"
#include "CySelectionGroup.h"
#include "CyArea.h"
#include "CyTradeRoute.h"
#include "CyTradeRouteGroup.h"

#include <boost/python/manage_new_object.hpp>
#include <boost/python/class.hpp>
#include <boost/python/scope.hpp>

//
// published python interface for CyPlayer
//
void CyPlayerPythonInterface2(python::class_<CyPlayer>& x)
{
	OutputDebugString("Python Extension Module - CyPlayerPythonInterface2\n");
	// set the docstring of the current module scope
	python::scope().attr("__doc__") = "Civilization IV Player Class";
	x
		// PatchMod: Achievements START
		.def("isAchieveGained", &CyPlayer::isAchieveGained)
		.def("getAchieveYear", &CyPlayer::getAchieveYear)
		// PatchMod: Achievements END
		.def("AI_updateFoundValues", &CyPlayer::AI_updateFoundValues, "void (bool bStartingLoc)")
		.def("AI_foundValue", &CyPlayer::AI_foundValue, "int (int, int, int, bool)")
		.def("AI_demandRebukedWar", &CyPlayer::AI_demandRebukedWar, "bool (int /*PlayerTypes*/)")
		.def("AI_getAttitude", &CyPlayer::AI_getAttitude, "AttitudeTypes (int /*PlayerTypes*/) - Gets the attitude of the player towards the player passed in")
		// R&R, Robert Surcouf, No More Variables Hidden game option START
        .def("AI_getAttitudeVal", &CyPlayer::AI_getAttitudeVal, "int (int /*PlayerTypes*/) - Gets the attitude VALUE of the player towards the player passed in")
        // R&R, Robert Surcouf, No More Variables Hidden game option END
		.def("AI_unitValue", &CyPlayer::AI_unitValue, "int (int /*UnitTypes*/ eUnit, int /*UnitAITypes*/ eUnitAI, CyArea* pArea)")
		.def("AI_civicValue", &CyPlayer::AI_civicValue, "int (int /*CivicTypes*/ eCivic)")
		.def("AI_totalUnitAIs", &CyPlayer::AI_totalUnitAIs, "int (int /*UnitAITypes*/ eUnitAI)")
		.def("AI_totalAreaUnitAIs", &CyPlayer::AI_totalAreaUnitAIs, "int (CyArea* pArea, int /*UnitAITypes*/ eUnitAI)")
		.def("AI_totalWaterAreaUnitAIs", &CyPlayer::AI_totalWaterAreaUnitAIs, "int (CyArea* pArea, int /*UnitAITypes*/ eUnitAI)")
		.def("AI_getNumAIUnits", &CyPlayer::AI_getNumAIUnits, "int (UnitAIType) - Returns # of UnitAITypes the player current has of UnitAIType")
		.def("AI_getAttitudeExtra", &CyPlayer::AI_getAttitudeExtra, "int (int /*PlayerTypes*/ eIndex) - Returns the extra attitude for this player - usually scenario specific")
		.def("AI_setAttitudeExtra", &CyPlayer::AI_setAttitudeExtra, "void (int /*PlayerTypes*/ eIndex, int iNewValue) - Sets the extra attitude for this player - usually scenario specific")
		.def("AI_changeAttitudeExtra", &CyPlayer::AI_changeAttitudeExtra, "void (int /*PlayerTypes*/ eIndex, int iChange) - Changes the extra attitude for this player - usually scenario specific")
		.def("AI_getMemoryCount", &CyPlayer::AI_getMemoryCount, "int (/*PlayerTypes*/ eIndex1, /*MemoryTypes*/ eIndex2)")
		.def("AI_changeMemoryCount", &CyPlayer::AI_changeMemoryCount, "void (/*PlayerTypes*/ eIndex1, /*MemoryTypes*/ eIndex2, int iChange)")
		.def("AI_getExtraGoldTarget", &CyPlayer::AI_getExtraGoldTarget, "int ()")
		.def("AI_setExtraGoldTarget", &CyPlayer::AI_setExtraGoldTarget, "void (int)")
		.def("getScoreHistory", &CyPlayer::getScoreHistory, "int (int iTurn)")
		.def("getEconomyHistory", &CyPlayer::getEconomyHistory, "int (int iTurn)")
		.def("getIndustryHistory", &CyPlayer::getIndustryHistory, "int (int iTurn)")
		.def("getAgricultureHistory", &CyPlayer::getAgricultureHistory, "int (int iTurn)")
		.def("getPowerHistory", &CyPlayer::getPowerHistory, "int (int iTurn)")
		.def("getCultureHistory", &CyPlayer::getCultureHistory, "int (int iTurn)")
		.def("getScriptData", &CyPlayer::getScriptData, "str () - Get stored custom data (via pickle)")
		.def("setScriptData", &CyPlayer::setScriptData, "void (str) - Set stored custom data (via pickle)")
		.def("AI_maxGoldTrade", &CyPlayer::AI_maxGoldTrade, "int (int)")
		.def("forcePeace", &CyPlayer::forcePeace, "void (int)")
		.def("getHighestTradedYield", &CyPlayer::getHighestTradedYield, "YieldTypes ()")
		.def("getHighestStoredYieldCityId", &CyPlayer::getHighestStoredYieldCityId, "int (int /*YieldTypes*/)")
		.def("getCrossesStored", &CyPlayer::getCrossesStored, "int ()")
		.def("getBellsStored", &CyPlayer::getBellsStored, "int ()")
		/** NBMOD TAX **/
		.def("NBMOD_GetMaxTaxRate", &CyPlayer::NBMOD_GetMaxTaxRate, "int ()")
		/** NBMOD TAX **/
		// TAC - Python Export - Ray - START
		.def("NBMOD_IncreaseMaxTaxRate", &CyPlayer::NBMOD_IncreaseMaxTaxRate, "void ()")
		.def("NBMOD_DecreaseMaxTaxRate", &CyPlayer::NBMOD_DecreaseMaxTaxRate, "void ()")
		// TAC - Python Export - Ray - END
		// R&R, ray, Bargaining - Start
		.def("tryGetNewBargainPriceSell", &CyPlayer::tryGetNewBargainPriceSell, "bool ()")
		.def("tryGetNewBargainPriceBuy", &CyPlayer::tryGetNewBargainPriceBuy, "bool ()")
		// R&R, ray, Bargaining - End
		.def("getTaxRate", &CyPlayer::getTaxRate, "int ()")
		.def("changeTaxRate", &CyPlayer::changeTaxRate, "void (int)")
		.def("canTradeWithEurope", &CyPlayer::canTradeWithEurope, "bool ()")
		.def("getSellToEuropeProfit", &CyPlayer::getSellToEuropeProfit, "int ()")
		.def("getYieldSellPrice", &CyPlayer::getYieldSellPrice, "int (eYield)")
		.def("getYieldBuyPrice", &CyPlayer::getYieldBuyPrice, "int (eYield)")
		.def("setYieldBuyPrice", &CyPlayer::setYieldBuyPrice, "void (eYield, int iValue, bool bMessage)")
		.def("sellYieldUnitToEurope", &CyPlayer::sellYieldUnitToEurope, "void (CyUnit* pUnit, int iAmount, int iCommission)")
		.def("buyYieldUnitFromEurope", &CyPlayer::buyYieldUnitFromEurope, python::return_value_policy<python::manage_new_object>(), "CyUnit (int /*YieldTypes*/ eYield, int iAmount, CyUnit* pTransport)")
		.def("buyYieldUnitFromAfrica", &CyPlayer::buyYieldUnitFromAfrica, python::return_value_policy<python::manage_new_object>(), "CyUnit (int /*YieldTypes*/ eYield, int iAmount, CyUnit* pTransport)") // R&R, ray, Africa
		.def("buyYieldUnitFromPortRoyal", &CyPlayer::buyYieldUnitFromPortRoyal, python::return_value_policy<python::manage_new_object>(), "CyUnit (int /*YieldTypes*/ eYield, int iAmount, CyUnit* pTransport)") // R&R, ray, Port Royal
		.def("getEuropeUnitBuyPrice", &CyPlayer::getEuropeUnitBuyPrice, "int (int (UnitTypes))")
		.def("buyEuropeUnit", &CyPlayer::buyEuropeUnit, python::return_value_policy<python::manage_new_object>(), "CyUnit (int /*UnitTypes*/ eUnit)")
		.def("getYieldBoughtTotal", &CyPlayer::getYieldBoughtTotal, "int (int /*YieldTypes*/ eYield)")
		.def("getYieldBoughtTotalAfrica", &CyPlayer::getYieldBoughtTotalAfrica, "int (int /*YieldTypes*/ eYield)") // WTP, ray, Yields Traded Total for Africa and Port Royal - START
		.def("getYieldBoughtTotalPortRoyal", &CyPlayer::getYieldBoughtTotalPortRoyal, "int (int /*YieldTypes*/ eYield)") // WTP, ray, Yields Traded Total for Africa and Port Royal - START	 
		.def("getNumRevolutionEuropeUnits", &CyPlayer::getNumRevolutionEuropeUnits, "int ()")
		.def("getRevolutionEuropeUnit", &CyPlayer::getRevolutionEuropeUnit, "int (int iIndex)")
		.def("getRevolutionEuropeProfession", &CyPlayer::getRevolutionEuropeProfession, "int (int iIndex)")
		.def("getDocksNextUnit", &CyPlayer::getDocksNextUnit, "int (int iIndex)")
		/*** TRIANGLETRADE 10/16/08 by DPII ***/
		.def("getNumAfricaUnits", &CyPlayer::getNumAfricaUnits, "int ()")
		.def("getAfricaUnit", &CyPlayer::getAfricaUnit, python::return_value_policy<python::manage_new_object>(), "CyUnit* (int iIndex)")
		.def("getAfricaUnitById", &CyPlayer::getAfricaUnitById, python::return_value_policy<python::manage_new_object>(), "CyUnit* (int iId)")
		.def("loadUnitFromAfrica", &CyPlayer::loadUnitFromAfrica, "void (CyUnit* pUnit, CyUnit* pTransport)")
		.def("unloadUnitToAfrica", &CyPlayer::unloadUnitToAfrica, "void (CyUnit* pUnit)")
		.def("getAfricaUnitBuyPrice", &CyPlayer::getAfricaUnitBuyPrice, "int (int (UnitTypes))")
		.def("buyAfricaUnit", &CyPlayer::buyAfricaUnit, python::return_value_policy<python::manage_new_object>(), "CyUnit (int /*UnitTypes*/ eUnit)")
		.def("canTradeWithAfrica", &CyPlayer::canTradeWithAfrica, "bool ()")
		.def("getSellToAfricaProfit", &CyPlayer::getSellToAfricaProfit, "int ()")
		.def("isYieldAfricaTradable", &CyPlayer::isYieldAfricaTradable, "bool ()")
		.def("getNumShips", &CyPlayer::getNumShips, "int ()") // WTP, ray, easily counting Ships - START
		// R&R, ray, Africa
		.def("getYieldAfricaSellPrice", &CyPlayer::getYieldAfricaSellPrice, "int (eYield)")
		.def("getYieldAfricaBuyPrice", &CyPlayer::getYieldAfricaBuyPrice, "int (eYield)")
		.def("getYieldAfricaBuyPriceNoModifier", &CyPlayer::getYieldAfricaBuyPriceNoModifier, "int (eYield)")
		.def("setYieldAfricaBuyPrice", &CyPlayer::setYieldAfricaBuyPrice, "void (eYield, int iValue, bool bMessage)")
		// R&R, ray, Port Royal
		.def("getNumPortRoyalUnits", &CyPlayer::getNumPortRoyalUnits, "int ()")
		.def("getPortRoyalUnit", &CyPlayer::getPortRoyalUnit, python::return_value_policy<python::manage_new_object>(), "CyUnit* (int iIndex)")
		.def("getPortRoyalUnitById", &CyPlayer::getPortRoyalUnitById, python::return_value_policy<python::manage_new_object>(), "CyUnit* (int iId)")
		.def("loadUnitFromPortRoyal", &CyPlayer::loadUnitFromPortRoyal, "void (CyUnit* pUnit, CyUnit* pTransport)")
		.def("unloadUnitToPortRoyal", &CyPlayer::unloadUnitToPortRoyal, "void (CyUnit* pUnit)")
		.def("getPortRoyalUnitBuyPrice", &CyPlayer::getPortRoyalUnitBuyPrice, "int (int (UnitTypes))")
		.def("buyPortRoyalUnit", &CyPlayer::buyPortRoyalUnit, python::return_value_policy<python::manage_new_object>(), "CyUnit (int /*UnitTypes*/ eUnit)")
		.def("canTradeWithPortRoyal", &CyPlayer::canTradeWithPortRoyal, "bool ()")
		.def("getSellToPortRoyalProfit", &CyPlayer::getSellToPortRoyalProfit, "int ()")
		.def("isYieldPortRoyalTradable", &CyPlayer::isYieldPortRoyalTradable, "bool ()")
		.def("getYieldPortRoyalSellPrice", &CyPlayer::getYieldPortRoyalSellPrice, "int (eYield)")
		.def("getYieldPortRoyalBuyPrice", &CyPlayer::getYieldPortRoyalBuyPrice, "int (eYield)")
		.def("getYieldPortRoyalBuyPriceNoModifier", &CyPlayer::getYieldPortRoyalBuyPriceNoModifier, "int (eYield)")
		.def("setYieldPortRoyalBuyPrice", &CyPlayer::setYieldPortRoyalBuyPrice, "void (eYield, int iValue, bool bMessage)")
		// R&R, ray, Port Royal - END
		/* R&R mod, vetiarvind, trade groups - start */
		.def("addTradeRouteGroup", &CyPlayer::addTradeRouteGroup, "int (const std::wstring groupName)")
		.def("editTradeRouteGroup", &CyPlayer::editTradeRouteGroup, "bool (int iId, const std::wstring groupName)")
		.def("removeTradeRouteGroup", &CyPlayer::removeTradeRouteGroup, "bool (int iId)")
		.def("getNumTradeGroups", &CyPlayer::getNumTradeGroups, "int ()")
		.def("getTradeGroup", &CyPlayer::getTradeGroup, python::return_value_policy<python::manage_new_object>(), "CyTradeRouteGroup* (int iIndex)")		
		/* R&R mod, vetiarvind, trade groups - end */
		
		
		
		;
}
