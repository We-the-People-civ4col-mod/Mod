//
// published python interface for CyGlobalContext
// Author - Mustafa Thamer
//

#include "CvGameCoreDLL.h"
#include "CyMap.h"
#include "CyPlayer.h"
#include "CyGame.h"
#include "CyGlobalContext.h"
#include "CvRandom.h"
//#include "CvStructs.h"
#include "CvInfos.h"
#include "CyTeam.h"


void CyGlobalContextPythonInterface6(python::class_<CyGlobalContext>& x)
{
	OutputDebugString("Python Extension Module - CyGlobalContextPythonInterface5\n");

	x
		.def("getYieldInfo", &CyGlobalContext::getYieldInfo, python::return_value_policy<python::reference_existing_object>(), "(YieldID) - CvInfo for YieldID")

		.def("getNumRouteInfos", &CyGlobalContext::getNumRouteInfos, "() - Total Route Infos XML\\Misc\\CIV4RouteInfos.xml")
		.def("getRouteInfo", &CyGlobalContext::getRouteInfo, python::return_value_policy<python::reference_existing_object>(), "(RouteID) - CvInfo for RouteID")

		.def("getNumImprovementInfos", &CyGlobalContext::getNumImprovementInfos, "() - Total Improvement Infos XML\\Terrain\\CIV4ImprovementInfos.xml")
		.def("getImprovementInfo", &CyGlobalContext::getImprovementInfo, python::return_value_policy<python::reference_existing_object>(), "(ImprovementID) - CvInfo for ImprovementID")

		.def("getNumGoodyInfos", &CyGlobalContext::getNumGoodyInfos, "() - Total Goody Infos XML\\GameInfo\\CIV4GoodyInfos.xml")
		.def("getGoodyInfo", &CyGlobalContext::getGoodyInfo, python::return_value_policy<python::reference_existing_object>(), "(GoodyID) - CvInfo for GoodyID")

		.def("getNumBuildInfos", &CyGlobalContext::getNumBuildInfos, "() - Total Build Infos XML\\Units\\CIV4BuildInfos.xml")
		.def("getBuildInfo", &CyGlobalContext::getBuildInfo, python::return_value_policy<python::reference_existing_object>(), "(BuildID) - CvInfo for BuildID")

		.def("getNumHandicapInfos", &CyGlobalContext::getNumHandicapInfos, "() - Total Handicap Infos XML\\GameInfo\\CIV4HandicapInfos.xml")
		.def("getHandicapInfo", &CyGlobalContext::getHandicapInfo, python::return_value_policy<python::reference_existing_object>(), "(HandicapID) - CvInfo for HandicapID")

		.def("getNumGameSpeedInfos", &CyGlobalContext::getNumGameSpeedInfos, "() - Total Game speed Infos XML\\GameInfo\\CIV4GameSpeedInfo.xml")
		.def("getGameSpeedInfo", &CyGlobalContext::getGameSpeedInfo, python::return_value_policy<python::reference_existing_object>(), "(GameSpeed Info) - CvInfo for GameSpeedID")

		.def("getNumTurnTimerInfos", &CyGlobalContext::getNumTurnTimerInfos, "() - Total Turn timer Infos XML\\GameInfo\\CIV4TurnTimerInfo.xml")
		.def("getTurnTimerInfo", &CyGlobalContext::getTurnTimerInfo, python::return_value_policy<python::reference_existing_object>(), "(TurnTimer Info) - CvInfo for TurnTimerID")

		.def("getNumBuildingClassInfos", &CyGlobalContext::getNumBuildingClassInfos, "() - Total Building Class Infos XML\\Buildings\\CIV4BuildingClassInfos.xml")
		.def("getBuildingClassInfo", &CyGlobalContext::getBuildingClassInfo, python::return_value_policy<python::reference_existing_object>(), "(BuildingClassID) - CvInfo for BuildingClassID")

		.def("getNumBuildingInfos", &CyGlobalContext::getNumBuildingInfos, "() - Total Building Infos XML\\Buildings\\CIV4BuildingInfos.xml")
		.def("getBuildingInfo", &CyGlobalContext::getBuildingInfo, python::return_value_policy<python::reference_existing_object>(), "(BuildingID) - CvInfo for BuildingID")

		.def("getNumUnitClassInfos", &CyGlobalContext::getNumUnitClassInfos, "() - Total Unit Class Infos XML\\Units\\CIV4UnitClassInfos.xml")
		.def("getUnitClassInfo", &CyGlobalContext::getUnitClassInfo, python::return_value_policy<python::reference_existing_object>(), "(UnitClassID) - CvInfo for UnitClassID")

		.def("getNumUnitCombatInfos", &CyGlobalContext::getNumUnitCombatInfos, "() - Total Unit Combat Infos XML\\Units\\CIV4UnitCombatInfos.xml")
		.def("getUnitCombatInfo", &CyGlobalContext::getUnitCombatInfo, python::return_value_policy<python::reference_existing_object>(), "(UnitCombatID) - CvInfo for UnitCombatID")

		.def("getDomainInfo", &CyGlobalContext::getDomainInfo, python::return_value_policy<python::reference_existing_object>(), "(DomainID) - CvInfo for DomainID")

		.def("getNumActionInfos", &CyGlobalContext::getNumActionInfos, "() - Total Action Infos XML\\Units\\CIV4ActionInfos.xml")
		.def("getActionInfo", &CyGlobalContext::getActionInfo, python::return_value_policy<python::reference_existing_object>(), "(ActionID) - CvInfo for ActionID")
	;
}