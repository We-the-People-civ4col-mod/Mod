#include "CvGameCoreDLL.h"
#include "CyArea.h"
#include "CyTeam.h"
#include "CyUnit.h"

#include <boost/python/class.hpp>
namespace python = boost::python;

//
// published python interface for CyTeam
//
void CyTeamPythonInterface()
{
	OutputDebugString("Python Extension Module - CyTeamPythonInterface\n");
	python::class_<CyTeam>("CyTeam")
		// PatchMod: Victorys START
		.def("getTotalProductionRate", &CyTeam::getTotalProductionRate)
		// PatchMod: Victorys END
		.def("isNone", &CyTeam::isNone, "bool () - is this instance valid?")
		.def("addTeam", &CyTeam::addTeam, "void (int /*TeamTypes*/ eTeam)")
		.def("canChangeWarPeace", &CyTeam::canChangeWarPeace, "bool (int /*TeamTypes*/ eTeam)")
		.def("canDeclareWar", &CyTeam::canDeclareWar, "bool (int /*TeamTypes*/ eTeam)")
		.def("declareWar", &CyTeam::declareWar, "void (int /*TeamTypes*/ eTeam, bool bNewDiplo, int /*WarPlanTypes*/ eWarPlan) - Forces your team to declare War on iTeam")
		.def("makePeace", &CyTeam::makePeace, "void (int /*TeamTypes*/ eTeam) - Forces peace between your team and iTeam")
		.def("canContact", &CyTeam::canContact, "bool (int /*TeamTypes*/ eTeam)")
		.def("meet", &CyTeam::meet, "void (int /*TeamTypes*/ eTeam, bool bNewDiplo) - forces team to meet iTeam")
		.def("signOpenBorders", &CyTeam::signOpenBorders, "void (int /*TeamTypes*/ eTeam)")
		.def("signDefensivePact", &CyTeam::signDefensivePact, "void (int /*TeamTypes*/ eTeam)")
		.def("getAssets", &CyTeam::getAssets, "int ()")
		.def("getPower", &CyTeam::getPower, "int ()")
		.def("getDefensivePower", &CyTeam::getDefensivePower, "int ()")
		.def("getAtWarCount", &CyTeam::getAtWarCount, "int ()")
		.def("getWarPlanCount", &CyTeam::getWarPlanCount, "int (int /*WarPlanTypes*/ eWarPlan)")
		.def("getAnyWarPlanCount", &CyTeam::getAnyWarPlanCount, "int ()")
		.def("getChosenWarCount", &CyTeam::getChosenWarCount, "int ()")
		.def("getHasMetCivCount", &CyTeam::getHasMetCivCount, "int ()")
		.def("hasMetHuman", &CyTeam::hasMetHuman, "bool ()")
		.def("getDefensivePactCount", &CyTeam::getDefensivePactCount, "int ()")
		.def("getUnitClassMaking", &CyTeam::getUnitClassMaking, "int (int (UnitClassTypes) eUnitClass)")
		.def("getUnitClassCountPlusMaking", &CyTeam::getUnitClassCountPlusMaking, "int (int (UnitClassTypes) eUnitClass)")
		.def("getBuildingClassMaking", &CyTeam::getBuildingClassMaking, "int (int (BuildingClassTypes) eBuildingClass)")
		.def("getBuildingClassCountPlusMaking", &CyTeam::getBuildingClassCountPlusMaking, "int (int (BuildingClassTypes) eBuildingClass)")
		.def("countTotalCulture", &CyTeam::countTotalCulture, "int ()")
		.def("countNumUnitsByArea", &CyTeam::countNumUnitsByArea, "int (CyArea* pArea)")
		.def("countNumCitiesByArea", &CyTeam::countNumCitiesByArea, "int (CyArea* pArea)")
		.def("countTotalPopulationByArea", &CyTeam::countTotalPopulationByArea, "int (CyArea* pArea)")
		//.def("countPowerByArea", &CyTeam::countPowerByArea, "int (CyArea* pArea)")
		//.def("countEnemyPowerByArea", &CyTeam::countEnemyPowerByArea, "int (CyArea* pArea)")
		.def("countNumAIUnitsByArea", &CyTeam::countNumAIUnitsByArea, "int (CyArea* pArea, int /*UnitAITypes*/ eUnitAI)")
		//.def("countEnemyDangerByArea", &CyTeam::countEnemyDangerByArea, "int (CyArea* pArea)")
		.def("getFatherPointCost", &CyTeam::getFatherPointCost, "int (int /*FatherTypes*/ eFather, int /*FatherPointTypes*/ ePointType)")
		// TAC - Python Export - koma13 - START
		.def("isFatherIgnore", &CyTeam::isFatherIgnore, "bool (int /*FatherTypes*/)")
		// TAC - Python Export - koma13 - END
		.def("canConvinceFather", &CyTeam::canConvinceFather, "bool (int /*FatherTypes*/)")
		.def("getFatherPoints", &CyTeam::getFatherPoints, "int (int /*FatherPointTypes*/ ePointType)")
		.def("changeFatherPoints", &CyTeam::changeFatherPoints, "void (int /*FatherPointTypes*/ ePointType, int iChange)")
		.def("isHuman", &CyTeam::isHuman, "bool () - is human team?")
		.def("getLeaderID", &CyTeam::getLeaderID, "int (PlayerTypes) ()")
		.def("getHandicapType", &CyTeam::getHandicapType, "int (HandicapTypes) ()")
		.def("getName", &CyTeam::getName, "str ()")
		.def("getNumMembers", &CyTeam::getNumMembers, "int (); # of people on team")
		.def("isAlive", &CyTeam::isAlive, "bool ()")
		.def("isEverAlive", &CyTeam::isEverAlive, "bool ()")
		.def("getNumCities", &CyTeam::getNumCities, "int (); # of cities controlled by team")
		.def("getTotalPopulation", &CyTeam::getTotalPopulation, "int (); # of citizens controlled by team")
		.def("getTotalLand", &CyTeam::getTotalLand, "int ()")
		.def("getMapTradingCount", &CyTeam::getMapTradingCount, "int ()")
		.def("isMapTrading", &CyTeam::isMapTrading, "bool () - map is ready")
		.def("changeMapTradingCount", &CyTeam::changeMapTradingCount, "void (iChange)")
		.def("getGoldTradingCount", &CyTeam::getGoldTradingCount, "int ()")
		.def("isGoldTrading", &CyTeam::isGoldTrading, "bool () - gold trading?")
		.def("changeGoldTradingCount", &CyTeam::changeGoldTradingCount, "void (iChange)")
		.def("getOpenBordersTradingCount", &CyTeam::getOpenBordersTradingCount, "int ()")
		.def("isOpenBordersTrading", &CyTeam::isOpenBordersTrading, "bool ()")
		.def("changeOpenBordersTradingCount", &CyTeam::changeOpenBordersTradingCount, "void (iChange)")
		.def("getDefensivePactTradingCount", &CyTeam::getDefensivePactTradingCount, "int ()")
		.def("isDefensivePactTrading", &CyTeam::isDefensivePactTrading, "bool ()")
		.def("changeDefensivePactTradingCount", &CyTeam::changeDefensivePactTradingCount, "void (iChange)")
		.def("getPermanentAllianceTradingCount", &CyTeam::getPermanentAllianceTradingCount, "int ()")
		.def("isPermanentAllianceTrading", &CyTeam::isPermanentAllianceTrading, "bool ()")
		.def("changePermanentAllianceTradingCount", &CyTeam::changePermanentAllianceTradingCount, "void (iChange)")
		.def("isMapCentering", &CyTeam::isMapCentering, "bool () - map is centered")
		.def("setMapCentering", &CyTeam::setMapCentering, "void (bNewValue)")
		.def("getID", &CyTeam::getID, "int () - team ID")
		.def("isHasMet", &CyTeam::isHasMet, "bool (int /*TeamTypes*/ eIndex)")
		.def("isAtWar", &CyTeam::isAtWar, "bool (int /*TeamTypes*/ eIndex)")
		.def("isPermanentWarPeace", &CyTeam::isPermanentWarPeace, "bool (int /*TeamTypes*/ eIndex)")
		.def("setPermanentWarPeace", &CyTeam::setPermanentWarPeace, "void (int /*TeamTypes*/ eIndex, bool bNewValue)")
		.def("isOpenBorders", &CyTeam::isOpenBorders, "bool (TeamTypes)")
		.def("isForcePeace", &CyTeam::isForcePeace, "bool (TeamTypes)")
		.def("isDefensivePact", &CyTeam::isDefensivePact, "bool (TeamTypes)")
		.def("getUnitClassCount", &CyTeam::getUnitClassCount, "int (int (UnitClassTypes) eIndex)")
		.def("getBuildingClassCount", &CyTeam::getBuildingClassCount, "bool (int /*BuildingClassTypes*/ eIndex)")
		.def("AI_shareWar", &CyTeam::AI_shareWar, "bool (TeamTypes)")
		.def("AI_setWarPlan", &CyTeam::AI_setWarPlan, "void (int /*TeamTypes*/ eIndex, int /*WarPlanTypes*/ eNewValue)")
		.def("AI_getAtWarCounter", &CyTeam::AI_getAtWarCounter, "int (TeamTypes)")
		.def("AI_getAtPeaceCounter", &CyTeam::AI_getAtPeaceCounter, "int (TeamTypes)")
		.def("AI_getWarSuccess", &CyTeam::AI_getWarSuccess, "int (TeamTypes)")
		.def("getRebelPercent", &CyTeam::getRebelPercent, "int ()")
		.def("doRevolution", &CyTeam::doRevolution, "void ()")
		.def("canDoRevolution", &CyTeam::canDoRevolution, "bool ()")
		.def("isParentOf", &CyTeam::isParentOf, "bool (TeamTypes)")
		;
}
