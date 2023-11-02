#include "CvGameCoreDLL.h"
#include "CyGame.h"
#include "CvRandom.h"
#include "CyCity.h"
#include "CyReplayInfo.h"

#include <boost/python/class.hpp>

//
// published python interface for CyGame
//

void CyGamePythonInterface3(python::class_<CyGame>& x)
{
	OutputDebugString("Python Extension Module - CyGamePythonInterface3\n");

	x
		.def("saveReplay", &CyGame::saveReplay)
		.def("addPlayer", &CyGame::addPlayer, "void (int eNewPlayer, int eLeader, int eCiv)")
		.def("setPlotExtraYield", &CyGame::setPlotExtraYield, "void (int iX, int iY, int /*YieldTypes*/ eYield, int iExtraYield)")
		.def("isCivEverActive", &CyGame::isCivEverActive, "bool (int /*CivilizationTypes*/ eCivilization)")
		.def("isLeaderEverActive", &CyGame::isLeaderEverActive, "bool (int /*LeaderHeadTypes*/ eLeader)")
		.def("isUnitEverActive", &CyGame::isUnitEverActive, "bool (int /*UnitTypes*/ eUnit)")
		.def("isBuildingEverActive", &CyGame::isBuildingEverActive, "bool (int /*BuildingTypes*/ eBuilding)")
		.def("isEventActive", &CyGame::isEventActive, "bool (int /*EventTriggerTypes*/ eTrigger)")
		.def("getFatherTeam", &CyGame::getFatherTeam, "int (int /*FatherTypes*/ eFather)")
		.def("getFatherGameTurn", &CyGame::getFatherGameTurn, "int (int /*FatherTypes*/ eFather)")
		.def("setFatherTeam", &CyGame::setFatherTeam, "void (int /*FatherTypes*/ eFather, int /*TeamTypes*/ eTeam)")
		.def("getFatherCategoryPosition", &CyGame::getFatherCategoryPosition, "int (int /*FatherTypes*/ eFather)")
		.def("getBarbarianPlayer", &CyGame::getBarbarianPlayer, "int /*PlayerTypes*/ ()") // < JAnimals Mod Start >
		.def("hasBarbarianPlayer", &CyGame::hasBarbarianPlayer, "bool ()") // < JAnimals Mod Start >
		.def("setBarbarianPlayer", &CyGame::setBarbarianPlayer, "void (int /*PlayerTypes*/ eNewValue)") // < JAnimals Mod Start >
		.def("isBarbarianPlayer", &CyGame::isBarbarianPlayer, "bool (int /*PlayerTypes*/ ePlayer)") // < JAnimals Mod Start >
		.def("getNextPlayerType", &CyGame::getNextPlayerType, "int /*PlayerTypes*/ ()") // < JAnimals Mod Start >
		.def("getChurchPlayer", &CyGame::getChurchPlayer, "int /*PlayerTypes*/ ()") // R&R, ray, the Church - START
		.def("hasChurchPlayer", &CyGame::hasChurchPlayer, "bool ()") // R&R, ray, the Church - START
		.def("setChurchPlayer", &CyGame::setChurchPlayer, "void (int /*PlayerTypes*/ eNewValue)") // R&R, ray, the Church - START
		.def("isChurchPlayer", &CyGame::isChurchPlayer, "bool (int /*PlayerTypes*/ ePlayer)") // R&R, ray, the Church - START
		;
}
