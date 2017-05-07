#include "CvGameCoreDLL.h"
#include "CyUnit.h"
#include "CyCity.h"
#include "CyPlot.h"
#include "CyArea.h"
#include "CySelectionGroup.h"
#include "CyArtFileMgr.h"
#include "CvInfos.h"
//# include <boost/python/manage_new_object.hpp>
//# include <boost/python/return_value_policy.hpp>
//
// published python interface for CyUnit
//
void CyUnitPythonInterface1(python::class_<CyUnit>& x)
{
	OutputDebugString("Python Extension Module - CyUnitPythonInterface1\n");
	x
		.def("isNone", &CyUnit::isNone, "bool () - Is this a valid unit instance?")
		.def("convert", &CyUnit::convert, "void (CyUnit* pUnit)")
		.def("kill", &CyUnit::kill, "void (bool bDelay)")
		.def("NotifyEntity", &CyUnit::NotifyEntity, "void (int EntityEventType)")
		.def("isActionRecommended", &CyUnit::isActionRecommended, "int (int i)")
		.def("canDoCommand", &CyUnit::canDoCommand, "bool (eCommand, iData1, iData2, bTestVisible = False) - can the unit perform eCommand?")
		.def("doCommand", &CyUnit::doCommand, "void (eCommand, iData1, iData2) - force the unit to perform eCommand")
		.def("getPathEndTurnPlot", &CyUnit::getPathEndTurnPlot, python::return_value_policy<python::manage_new_object>(), "CyPlot* ()")
		
		// TAC - AI Improved Naval AI - koma13 - START
		//.def("generatePath", &CyUnit::generatePath, "bool (CyPlot* pToPlot, int iFlags = 0, bool bReuse = false, int* piPathTurns = NULL)")
		.def("generatePath", &CyUnit::generatePath, "bool (CyPlot* pToPlot, int iFlags = 0, bool bReuse = false, int* piPathTurns = NULL, bool bIgnoreDanger = true)")
		// TAC - AI Improved Naval AI - koma13 - END

		.def("canEnterTerritory", &CyUnit::canEnterTerritory, "bool (int (PlayerTypes) ePlayer, bool bIgnoreRightOfPassage)")
		.def("canEnterArea", &CyUnit::canEnterArea, "bool (int (PlayerTypes) ePlayer, CyArea* pArea, bool bIgnoreRightOfPassage)")
		.def("canMoveInto", &CyUnit::canMoveInto, "bool (CyPlot* pPlot, bool bAttack, bool bDeclareWar, bool bIgnoreLoad)")
		.def("canMoveOrAttackInto", &CyUnit::canMoveOrAttackInto, "bool (CyPlot* pPlot, bool bDeclareWar)")
		.def("canMoveThrough", &CyUnit::canMoveThrough, "bool (CyPlot* pPlot)")
		.def("jumpToNearestValidPlot", &CyUnit::jumpToNearestValidPlot, "bool ()")
		.def("canAutomate", &CyUnit::canAutomate, "bool (int (AutomateTypes) eAutomate)")
		.def("canScrap", &CyUnit::canScrap, "bool ()")
		.def("canGift", &CyUnit::canGift, "bool (bool bTestVisible)")
		.def("canLoadUnit", &CyUnit::canLoadUnit, "bool (CyUnit* pUnit, CyPlot* pPlot, bool bCheckCity)")
		.def("canLoad", &CyUnit::canLoad, "bool (bool bCheckCity)")
		.def("load", &CyUnit::load, "void (bool bCheckCity)")
		.def("canUnload", &CyUnit::canUnload, "bool ()")
		.def("canUnloadAll", &CyUnit::canUnloadAll, "bool ()")
		.def("canHold", &CyUnit::canHold, "bool (CyPlot* pPlot)")
		.def("canSleep", &CyUnit::canSleep, "bool (CyPlot* pPlot)")
		.def("canFortify", &CyUnit::canFortify, "bool (CyPlot* pPlot)")
		.def("canHeal", &CyUnit::canHeal, "bool (CyPlot* pPlot)")
		.def("canSentry", &CyUnit::canSentry, "bool (CyPlot* pPlot)")
		.def("bombardTarget", &CyUnit::bombardTarget, python::return_value_policy<python::manage_new_object>(), "CyCity* (CyPlot* pPlot)")
		.def("canBombard", &CyUnit::canBombard, "bool (CyPlot* pPlot)")
		.def("canPillage", &CyUnit::canPillage, "bool (CyPlot* pPlot)")
		.def("canFound", &CyUnit::canFound, "bool (CyPlot* pPlot, bool bTestVisible)")
		.def("canBuild", &CyUnit::canBuild, "bool (CyPlot* pPlot, int (BuildTypes) eBuild, bool bTestVisible)")
		.def("canLead", &CyUnit::canLead, "int (CyPlot* pPlot, int)")
		.def("lead", &CyUnit::lead, "bool (int)")
		.def("canGiveExperience", &CyUnit::canGiveExperience, "int (CyPlot* pPlot)")
		.def("giveExperience", &CyUnit::giveExperience, "bool ()")
		.def("canPromote", &CyUnit::canPromote, "bool (ePromotion, iLeaderUnitId)")
		.def("promote", &CyUnit::promote, "bool (ePromotion)")
		.def("canKingTransport", &CyUnit::canKingTransport, "bool ()")
		.def("canLearn", &CyUnit::canLearn, "bool ()")
		.def("canEstablishMission", &CyUnit::canEstablishMission, "bool ()")
		.def("canSpeakWithChief", &CyUnit::canSpeakWithChief, "bool (CyPlot* pPlot)")
		.def("canTradeYield", &CyUnit::canTradeYield, "bool (CyPlot* pPlot)")
		.def("upgradePrice", &CyUnit::upgradePrice, "int (UnitTypes eUnit)")
		.def("upgradeAvailable", &CyUnit::upgradeAvailable, "bool (int /*UnitTypes*/ eFromUnit, int /*UnitClassTypes*/ eToUnitClass, int iCount)")
		.def("canUpgrade", &CyUnit::canUpgrade, "bool (int /*UnitTypes*/ eUnit, bool bTestVisible)")
		.def("hasUpgrade", &CyUnit::hasUpgrade, "bool (bool bSearch)")
		.def("getHandicapType", &CyUnit::getHandicapType, "int ()")
		.def("getCivilizationType", &CyUnit::getCivilizationType, "int ()")
		.def("getSpecialUnitType", &CyUnit::getSpecialUnitType, "int ()")
		.def("getCaptureUnitType", &CyUnit::getCaptureUnitType, "int (int /*CivilizationTypes*/ eCivilization)")
		.def("getUnitCombatType", &CyUnit::getUnitCombatType, "int ()")
		.def("getDomainType", &CyUnit::getDomainType, "int ()")
		.def("getInvisibleType", &CyUnit::getInvisibleType, "int (InvisibleTypes) ()")
		.def("getNumSeeInvisibleTypes", &CyUnit::getNumSeeInvisibleTypes, "int ()")
		.def("getSeeInvisibleType", &CyUnit::getSeeInvisibleType, "int (InvisibleTypes) (int)")
		.def("isHuman", &CyUnit::isHuman, "bool ()")
		.def("visibilityRange", &CyUnit::visibilityRange, "int ()")
		.def("baseMoves", &CyUnit::baseMoves, "int ()")
		.def("movesLeft", &CyUnit::movesLeft, "int ()")
		.def("maxMoves", &CyUnit::maxMoves, "int ()")
		.def("canMove", &CyUnit::canMove, "bool ()")
		.def("hasMoved", &CyUnit::hasMoved, "bool ()")
		.def("canBuildRoute", &CyUnit::canBuildRoute, "bool ()")
		.def("getBuildType", &CyUnit::getBuildType, "int (BuildTypes) ()")
		.def("workRate", &CyUnit::workRate, "int (bool bMax)")
		.def("isNoBadGoodies", &CyUnit::isNoBadGoodies, "bool ()")
		.def("isOnlyDefensive", &CyUnit::isOnlyDefensive, "bool ()")
		.def("isNoUnitCapture", &CyUnit::isNoUnitCapture, "bool ()")
		.def("isNoCityCapture", &CyUnit::isNoCityCapture, "bool ()")
		.def("isRivalTerritory", &CyUnit::isRivalTerritory, "bool ()")
		.def("canCoexistWithEnemyUnit", &CyUnit::canCoexistWithEnemyUnit, "bool (int)")
		.def("isFighting", &CyUnit::isFighting, "bool ()")
		.def("isAttacking", &CyUnit::isAttacking, "bool ()")
		.def("isDefending", &CyUnit::isDefending, "bool ()")
		.def("isCombat", &CyUnit::isCombat, "bool ()")
		.def("maxHitPoints", &CyUnit::maxHitPoints, "bool ()")
		.def("currHitPoints", &CyUnit::currHitPoints, "bool ()")
		.def("isHurt", &CyUnit::isHurt, "bool ()")
		.def("isDead", &CyUnit::isDead, "bool ()")
		.def("setBaseCombatStr", &CyUnit::setBaseCombatStr, "void (int)")
		.def("baseCombatStr", &CyUnit::baseCombatStr, "int ()")
		.def("maxCombatStr", &CyUnit::maxCombatStr, "int (CyPlot* pPlot, CyUnit* pAttacker)")
		.def("currCombatStr", &CyUnit::currCombatStr, "int (CyPlot* pPlot, CyUnit* pAttacker)")
		.def("currFirepower", &CyUnit::currFirepower, "int (CyPlot* pPlot, CyUnit* pAttacker)")
		.def("maxCombatStrFloat", &CyUnit::maxCombatStrFloat, "float (CyPlot* pPlot, CyUnit* pAttacker)")
		.def("currCombatStrFloat", &CyUnit::currCombatStrFloat, "float (CyPlot* pPlot, CyUnit* pAttacker)")
		.def("canFight", &CyUnit::canFight, "bool ()")
		.def("canAttack", &CyUnit::canAttack, "bool ()")
		.def("canDefend", &CyUnit::canDefend, "bool (CyPlot* pPlot)")
		// TAC - LbD - Ray - START
		.def("getLbDrounds", &CyUnit::getLbDrounds, "int ()")
		.def("getLastLbDProfession", &CyUnit::getLastLbDProfession, "int ()")
		// TAC - LbD - Ray - END
		;
}
