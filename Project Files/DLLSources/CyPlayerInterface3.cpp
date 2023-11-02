#include "CvGameCoreDLL.h"
#include "CyPlayer.h"
#include "CyUnit.h"
#include "CyCity.h"
#include "CyPlot.h"
#include "CySelectionGroup.h"
#include "CyArea.h"
#include "CyTradeRoute.h"

#include <boost/python/manage_new_object.hpp>
#include <boost/python/class.hpp>
#include <boost/python/scope.hpp>

//
// published python interface for CyPlayer
//
void CyPlayerPythonInterface3(python::class_<CyPlayer>& x)
{
	OutputDebugString("Python Extension Module - CyPlayerPythonInterface3\n");
	// set the docstring of the current module scope
	python::scope().attr("__doc__") = "Civilization IV Player Class";
	x
		.def("doGoody", &CyPlayer::doGoody, "void (CyPlot* pPlot, CyUnit* pUnit)")
		.def("canFound", &CyPlayer::canFound, "bool (int iX, int iY)")
		.def("found", &CyPlayer::found, "void (int iX, int iY)")
		.def("canTrain", &CyPlayer::canTrain, "bool (int eUnit, bool bContinue, bool bTestVisible)")
		.def("canConstruct", &CyPlayer::canConstruct, "bool (int /*BuildingTypes*/eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost)")
		.def("getBuildingClassPrereqBuilding", &CyPlayer::getBuildingClassPrereqBuilding, "int (int /*BuildingTypes*/ eBuilding, int /*BuildingClassTypes*/ ePrereqBuildingClass, iExtra) -")
		.def("getUnitYieldProductionNeeded", &CyPlayer::getUnitYieldProductionNeeded, "int getUnitYieldProductionNeeded(int /*UnitTypes*/ eUnit, int /*YieldTypes*/ eYield)")
		.def("getBuildingYieldProductionNeeded", &CyPlayer::getBuildingYieldProductionNeeded, "int getBuildingYieldProductionNeeded(int /*BuildingTypes*/ eBuilding, int /*YieldTypes*/ eYield)")
		.def("removeBuildingClass", &CyPlayer::removeBuildingClass, "void (int /*BuildingClassTypes*/ eBuildingClass)")
		.def("canBuild", &CyPlayer::canBuild, "bool (CyPlot* pPlot, int (BuildTypes) eBuild, bool bTestEra, bool bTestVisible)")
		.def("calculateTotalYield", &CyPlayer::calculateTotalYield, "int (int /*YieldTypes*/ eYield) - Returns the total sum of all city yield")
		.def("isCivic", &CyPlayer::isCivic, "bool (int (CivicTypes) eCivic)")
		.def("canDoCivics", &CyPlayer::canDoCivics, "bool (int (CivicTypes) eCivic)")
		.def("greatGeneralThreshold", &CyPlayer::greatGeneralThreshold, "int ()")
		.def("greatAdmiralThreshold", &CyPlayer::greatAdmiralThreshold, "int ()")
		.def("getImmigrationThresholdModifierFromUnitsWaitingOnDock", &CyPlayer::getImmigrationThresholdModifierFromUnitsWaitingOnDock, "int ()") // WTP, ray, increase threshold if more than X units waiting on the docks - START
		.def("immigrationThreshold", &CyPlayer::immigrationThreshold, "int ()")
		.def("revolutionEuropeUnitThreshold", &CyPlayer::revolutionEuropeUnitThreshold, "int ()")
		.def("getStartingPlot", &CyPlayer::getStartingPlot, python::return_value_policy<python::manage_new_object>(), "CyPlot* ()")
		.def("setStartingPlot", &CyPlayer::setStartingPlot, "void (CyPlot*, bool) - sets the player's starting plot")
		.def("getTotalPopulation", &CyPlayer::getTotalPopulation, "int ()")
		.def("getAveragePopulation", &CyPlayer::getAveragePopulation, "int ()")
		.def("getRealPopulation", &CyPlayer::getRealPopulation, "long int ()")
		.def("getTotalLand", &CyPlayer::getTotalLand, "int ()")
		.def("getTotalLandScored", &CyPlayer::getTotalLandScored, "int ()")
		.def("getGold", &CyPlayer::getGold, "int ()")
		.def("setGold", &CyPlayer::setGold, "void (int iNewValue)")
		.def("changeGold", &CyPlayer::changeGold, "void (int iChange)")
		.def("getAdvancedStartPoints", &CyPlayer::getAdvancedStartPoints, "int ()")
		.def("setAdvancedStartPoints", &CyPlayer::setAdvancedStartPoints, "void (int iNewValue)")
		.def("changeAdvancedStartPoints", &CyPlayer::changeAdvancedStartPoints, "void (int iChange)")
		.def("getAdvancedStartUnitCost", &CyPlayer::getAdvancedStartUnitCost, "int (int (UnitTypes) eUnit, bool bAdd, CyPlot* pPlot)")
		.def("getAdvancedStartCityCost", &CyPlayer::getAdvancedStartCityCost, "int (int (bool bAdd, CyPlot* pPlot)")
		.def("getAdvancedStartPopCost", &CyPlayer::getAdvancedStartPopCost, "int (int (bool bAdd, CyCity* pCity)")
		.def("getAdvancedStartCultureCost", &CyPlayer::getAdvancedStartCultureCost, "int (int (bool bAdd, CyCity* pCity)")
		.def("getAdvancedStartBuildingCost", &CyPlayer::getAdvancedStartBuildingCost, "int (int (BuildingTypes) eUnit, bool bAdd, CyCity* pCity)")
		.def("getAdvancedStartImprovementCost", &CyPlayer::getAdvancedStartImprovementCost, "int (int (ImprovementTypes) eImprovement, bool bAdd, CyPlot* pPlot)")
		.def("getAdvancedStartRouteCost", &CyPlayer::getAdvancedStartRouteCost, "int (int (RouteTypes) eUnit, bool bAdd, CyPlot* pPlot)")
		.def("getAdvancedStartVisibilityCost", &CyPlayer::getAdvancedStartVisibilityCost, "int (bool bAdd, CyPlot* pPlot)")
		.def("createGreatGeneral", &CyPlayer::createGreatGeneral, "void (int /*UnitTypes*/ eUnit, bool bIncrementThreshold, int iX, int iY)")
		.def("getGreatGeneralsCreated", &CyPlayer::getGreatGeneralsCreated, "int ()")
		.def("getGreatGeneralsThresholdModifier", &CyPlayer::getGreatGeneralsThresholdModifier, "int ()")
		.def("createGreatAdmiral", &CyPlayer::createGreatAdmiral, "void (int /*UnitTypes*/ eUnit, bool bIncrementThreshold, int iX, int iY)")
		.def("getGreatAdmiralsCreated", &CyPlayer::getGreatAdmiralsCreated, "int ()")
		.def("getGreatAdmiralsThresholdModifier", &CyPlayer::getGreatAdmiralsThresholdModifier, "int ()")
		.def("getGreatGeneralRateModifier", &CyPlayer::getGreatGeneralRateModifier, "int ()")
		.def("getDomesticGreatGeneralRateModifier", &CyPlayer::getDomesticGreatGeneralRateModifier, "int ()")
		.def("getFreeExperience", &CyPlayer::getFreeExperience, "int ()")
		.def("getWorkerSpeedModifier", &CyPlayer::getWorkerSpeedModifier, "int ()")
		.def("getImprovementUpgradeRateModifier", &CyPlayer::getImprovementUpgradeRateModifier, "int ()")
		.def("getMilitaryProductionModifier", &CyPlayer::getMilitaryProductionModifier, "int ()")
		.def("getCityDefenseModifier", &CyPlayer::getCityDefenseModifier, "int ()")
		.def("getHighestUnitLevel", &CyPlayer::getHighestUnitLevel, "int ()")
		.def("getExpInBorderModifier", &CyPlayer::getExpInBorderModifier, "bool ()")
		.def("getLevelExperienceModifier", &CyPlayer::getLevelExperienceModifier, "int ()")
		.def("getCapitalCity", &CyPlayer::getCapitalCity, python::return_value_policy<python::manage_new_object>(), "CyCity* (int iID)")
		.def("getCitiesLost", &CyPlayer::getCitiesLost, "int ()")
		.def("getAssets", &CyPlayer::getAssets, "int ()")
		.def("changeAssets", &CyPlayer::changeAssets, "void (int iChange)")
		.def("getPower", &CyPlayer::getPower, "int ()")
		.def("getPopScore", &CyPlayer::getPopScore, "int ()")
		.def("getLandScore", &CyPlayer::getLandScore, "int ()")
		.def("getFatherScore", &CyPlayer::getFatherScore, "int ()")
		.def("getTotalTimePlayed", &CyPlayer::getTotalTimePlayed, "int ()")
		.def("isAlive", &CyPlayer::isAlive, "bool ()")
		.def("isEverAlive", &CyPlayer::isEverAlive, "bool ()")
		.def("isExtendedGame", &CyPlayer::isExtendedGame, "bool ()")
		.def("isFoundedFirstCity", &CyPlayer::isFoundedFirstCity, "bool ()")
		.def("getID", &CyPlayer::getID, "int ()")
		.def("getWikiWidget", &CyPlayer::getWikiWidget, "int ()")
		.def("getChar", &CyPlayer::getChar, "int ()")
		.def("getHandicapType", &CyPlayer::getHandicapType, "int ()")
		.def("getCivilizationType", &CyPlayer::getCivilizationType, "int ()")
		.def("getLeaderType", &CyPlayer::getLeaderType, "int ()")
		.def("getPersonalityType", &CyPlayer::getPersonalityType, "int ()")
		.def("setPersonalityType", &CyPlayer::setPersonalityType, "void (int /*LeaderHeadTypes*/ eNewValue)")
		.def("getCurrentEra", &CyPlayer::getCurrentEra, "int ()")
		.def("setCurrentEra", &CyPlayer::setCurrentEra, "void (int /*EraTypes*/ iNewValue)")
		.def("getParent", &CyPlayer::getParent, "int ()")
		.def("getTeam", &CyPlayer::getTeam, "int ()")
		.def("isTurnActive", &CyPlayer::isTurnActive, "bool ()")
		.def("getPlayerColor", &CyPlayer::getPlayerColor, "int (PlayerColorTypes) () - returns the color ID of the player")
		.def("getPlayerTextColorR", &CyPlayer::getPlayerTextColorR, "int ()")
		.def("getPlayerTextColorG", &CyPlayer::getPlayerTextColorG, "int ()")
		.def("getPlayerTextColorB", &CyPlayer::getPlayerTextColorB, "int ()")
		.def("getPlayerTextColorA", &CyPlayer::getPlayerTextColorA, "int ()")
		.def("getSeaPlotYield", &CyPlayer::getSeaPlotYield, "int (YieldTypes eIndex)")
		// R&R, Robert Surcouf, No More Variables Hidden game option START
        .def("getYieldTradedTotal", &CyPlayer::getYieldTradedTotal, "int (YieldTypes eIndex)")
		.def("getYieldTradedTotalINT", &CyPlayer::getYieldTradedTotalINT, "int (int /*YieldTypes*/ eIndex)")
		// WTP, ray, Yields Traded Total for Africa and Port Royal - START
		.def("getYieldTradedTotalAfrica", &CyPlayer::getYieldTradedTotalAfrica, "int (YieldTypes eIndex)")
		.def("getYieldTradedTotalINTAfrica", &CyPlayer::getYieldTradedTotalINTAfrica, "int (int /*YieldTypes*/ eIndex)")
		.def("getYieldTradedTotalPortRoyal", &CyPlayer::getYieldTradedTotalPortRoyal, "int (YieldTypes eIndex)")
		.def("getYieldTradedTotalINTPortRoyal", &CyPlayer::getYieldTradedTotalINTPortRoyal, "int (int /*YieldTypes*/ eIndex)")
		// WTP, ray, Yields Traded Total for Africa and Port Royal - END
		.def("getMissionaryPoints", &CyPlayer::getMissionaryPoints, "int (int /*PlayerTypes*/ ePlayer)") 
		.def("missionaryThreshold", &CyPlayer::missionaryThreshold, "int (int /*PlayerTypes*/ ePlayer)") 
		.def("getMissionaryRateModifier", &CyPlayer::getMissionaryRateModifier, "int (int /*PlayerTypes*/ ePlayer)")
		//WTP, ray Kings Used Ship - START
		.def("getRandomUsedShipClassTypeID", &CyPlayer::getRandomUsedShipClassTypeID, "int ()") 
		.def("getUsedShipPrice", &CyPlayer::getUsedShipPrice, "int (int iUsedShipClassType)") 
		.def("isKingWillingToTradeUsedShips", &CyPlayer::isKingWillingToTradeUsedShips, "bool ()")
		.def("resetCounterForUsedShipDeals", &CyPlayer::resetCounterForUsedShipDeals, "void ()")
		//WTP, ray Kings Used Ship - END

		// WTP, ray, Foreign Kings, buy Immigrants - START
		.def("getRandomForeignImmigrantClassTypeID", &CyPlayer::getRandomForeignImmigrantClassTypeID, "int ()") 
		.def("getForeignImmigrantPrice", &CyPlayer::getForeignImmigrantPrice, "int (int iForeignImmigrantClassType, int iEuropeKingID)") 
		.def("isForeignKingWillingToTradeImmigrants", &CyPlayer::isForeignKingWillingToTradeImmigrants, "bool (int iEuropeKingID)")
		.def("resetCounterForForeignImmigrantsDeals", &CyPlayer::resetCounterForForeignImmigrantsDeals, "void ()")
		// WTP, ray, Foreign Kings, buy Immigrants - END

		// R&R, Robert Surcouf, No More Variables Hidden game option END		
		.def("getYieldScoreTotalINT", &CyPlayer::getYieldScoreTotalINT, "int (int /*YieldTypes*/ eIndex)") // R&R, vetiarvind, Price dependent tax rate change
		.def("getChurchFavourPrice", &CyPlayer::getChurchFavourPrice, "int ()") // R&R, ray, Church Favours
		.def("getYieldRate", &CyPlayer::getYieldRate, "int (YieldTypes eIndex)")
		.def("getYieldRateModifier", &CyPlayer::getYieldRateModifier, "int (YieldTypes eIndex)")
		.def("getHappinessRate", &CyPlayer::getHappinessRate, "int ()") // WTP, ray, Happiness - START
		.def("getUnHappinessRate", &CyPlayer::getUnHappinessRate, "int ()") // WTP, ray, Happiness - START
		.def("getLawRate", &CyPlayer::getLawRate, "int ()") // WTP, ray, Crime and Law - START
		.def("getCrimeRate", &CyPlayer::getCrimeRate, "int ()") // WTP, ray, Crime and Law - START
		.def("getCapitalYieldRateModifier", &CyPlayer::getCapitalYieldRateModifier, "int (YieldTypes eIndex)")
		.def("getExtraYieldThreshold", &CyPlayer::getExtraYieldThreshold, "int (YieldTypes eIndex)")
		.def("isYieldEuropeTradable", &CyPlayer::isYieldEuropeTradable, "bool ()")
		.def("setYieldEuropeTradable", &CyPlayer::setYieldEuropeTradable, "void ()")
		.def("isFeatAccomplished", &CyPlayer::isFeatAccomplished, "bool ()")
		.def("setFeatAccomplished", &CyPlayer::setFeatAccomplished, "void ()")
		.def("shouldDisplayFeatPopup", &CyPlayer::shouldDisplayFeatPopup, "bool ()")
		.def("isOption", &CyPlayer::isOption, "bool ()")
		.def("setOption", &CyPlayer::setOption, "void ()")
		.def("isPlayable", &CyPlayer::isPlayable, "bool ()")
		.def("setPlayable", &CyPlayer::setPlayable, "void ()")
		.def("getImprovementCount", &CyPlayer::getImprovementCount, "int (int /*ImprovementTypes*/ iIndex)")
		.def("isBuildingFree", &CyPlayer::isBuildingFree, "bool (int /*BuildingTypes*/ eIndex)")
		.def("getUnitClassCount", &CyPlayer::getUnitClassCount, "int (int (UnitClassTypes) eIndex)")
		.def("getUnitClassMaking", &CyPlayer::getUnitClassMaking, "int (int (UnitClassTypes) eIndex)")
		.def("getUnitClassCountPlusMaking", &CyPlayer::getUnitClassCountPlusMaking, "int (int (UnitClassTypes) eIndex)")
		.def("getBuildingClassCount", &CyPlayer::getBuildingClassCount, "int (int /*BuildingClassTypes*/ eIndex)")
		.def("getBuildingClassMaking", &CyPlayer::getBuildingClassMaking, "int (int /*BuildingClassTypes*/ iIndex)")
		.def("getBuildingClassCountPlusMaking", &CyPlayer::getBuildingClassCountPlusMaking, "int (int /*BuildingClassTypes*/ iIndex)")
		.def("getHurryCount", &CyPlayer::getHurryCount, "int (int (HurryTypes) eIndex)")
		.def("canHurry", &CyPlayer::canHurry, "int (int (HurryTypes) eIndex)")
		.def("getSpecialBuildingNotRequiredCount", &CyPlayer::getSpecialBuildingNotRequiredCount, "int (int (SpecialBuildingTypes) eIndex)")
		.def("isSpecialBuildingNotRequired", &CyPlayer::isSpecialBuildingNotRequired, "int (int (SpecialBuildingTypes) eIndex)")
		.def("getBuildingYieldChange", &CyPlayer::getBuildingYieldChange, "int (BuildingClassTypes eBuildingClass, YieldTypes eYield)")
		.def("getCivic", &CyPlayer::getCivic, "int /*CivicTypes*/ (int /*CivicOptionTypes*/ iIndex)")
		.def("setCivic", &CyPlayer::setCivic, "void (int iCivicOptionType, int iCivicType)")
		// TAC - Trade Messages - koma13 - START
		.def("getTradeMessageType", &CyPlayer::getTradeMessageType, "int /*TradeMessageTypes*/ (int)")
		.def("getTradeMessageYield", &CyPlayer::getTradeMessageYield, "int /*YieldTypes*/ (int)")
		.def("getTradeMessageAmount", &CyPlayer::getTradeMessageAmount, "int (int)")
		.def("getTradeMessageCommission", &CyPlayer::getTradeMessageCommission, "int (int)")
		.def("toggleMultiRowPlotList", &CyPlayer::toggleMultiRowPlotList, "void ()")
		;
}
