#include "CvGameCoreDLL.h"
#include "CyGame.h"
#include "CvRandom.h"
#include "CyCity.h"
#include "CyReplayInfo.h"
#include "CyPlot.h"

#include <boost/python/class.hpp>

//
// published python interface for CyGame
//

void CyGamePythonInterface1(python::class_<CyGame>& x)
{
	OutputDebugString("Python Extension Module - CyGamePythonInterface1\n");

	x
		// R&R, ray, Correct Geographical Placement of Natives - START
		.def("isWBNorthAmericanNative", &CyGame::isWBNorthAmericanNative)
		.def("setWBNorthAmericanNative", &CyGame::setWBNorthAmericanNative)
		.def("isWBSouthAmericanNative", &CyGame::isWBSouthAmericanNative)
		.def("setWBSouthAmericanNative", &CyGame::setWBSouthAmericanNative)
		.def("isWBCentralAmericanNative", &CyGame::isWBCentralAmericanNative)
		.def("setWBCentralAmericanNative", &CyGame::setWBCentralAmericanNative)
		// R&R, ray, Correct Geographical Placement of Natives - END
		.def("isNone", &CyGame::isNone, "CyGame* () - is the instance valid?")
		.def("updateScore", &CyGame::updateScore, "void (bool bForce)")
		.def("cycleCities", &CyGame::cycleCities, "void (bool bForward, bool bAdd)")
		.def("cycleSelectionGroups", &CyGame::cycleSelectionGroups, "void (bool bClear, bool bForward)")
		.def("cyclePlotUnits", &CyGame::cyclePlotUnits, "bool (CyPlot* pPlot, bool bForward, bool bAuto, int iCount)")
		.def("selectionListMove", &CyGame::selectionListMove, "void (CyPlot* pPlot, bool bAlt, bool bShift, bool bCtrl)")
		.def("selectionListGameNetMessage", &CyGame::selectionListGameNetMessage, "void (int eMessage, int iData2, int iData3, int iData4, int iFlags, bool bAlt, bool bShift)")
		.def("selectedCitiesGameNetMessage", &CyGame::selectedCitiesGameNetMessage, "void (int eMessage, int iData2, int iData3, int iData4, bool bOption, bool bAlt, bool bShift, bool bCtrl)")
		.def("cityPushOrder", &CyGame::cityPushOrder, "void (CyCity* pCity, OrderTypes eOrder, int iData, bool bAlt, bool bShift, bool bCtrl)")
		.def("getSymbolID", &CyGame::getSymbolID, "int (int iSymbol)")
		.def("getProductionPerPopulation", &CyGame::getProductionPerPopulation, "int (int /*HurryTypes*/ eHurry)")
		.def("getAdjustedPopulationPercent", &CyGame::getAdjustedPopulationPercent, "int (int eVictory)")
		.def("getAdjustedLandPercent", &CyGame::getAdjustedLandPercent, "int (int eVictory)")
		.def("countCivPlayersAlive", &CyGame::countCivPlayersAlive, "int ()")
		.def("countCivPlayersEverAlive", &CyGame::countCivPlayersEverAlive, "int ()")
		.def("countCivTeamsAlive", &CyGame::countCivTeamsAlive, "int ()")
		.def("countCivTeamsEverAlive", &CyGame::countCivTeamsEverAlive, "int ()")
		.def("countHumanPlayersAlive", &CyGame::countHumanPlayersAlive, "int ()")
		.def("countHumanPlayersEverAlive", &CyGame::countHumanPlayersEverAlive, "int ()")
		.def("countTotalCivPower", &CyGame::countTotalCivPower, "int ()")
		.def("getImprovementUpgradeTime", &CyGame::getImprovementUpgradeTime, "int (int /*ImprovementTypes*/ eImprovement)")
		.def("getCurrentEra", &CyGame::getCurrentEra, "int /*EratTypes*/ ()")
		.def("getActiveTeam", &CyGame::getActiveTeam, "int () - returns ID for the group")
		.def("getActiveCivilizationType", &CyGame::getActiveCivilizationType, "int () - returns CivilizationID" )
		.def("isNetworkMultiPlayer", &CyGame::isNetworkMultiPlayer, "bool () - NetworkMultiplayer()? ")
		.def("isGameMultiPlayer", &CyGame::isGameMultiPlayer, "bool () - GameMultiplayer()? ")
		.def("isTeamGame", &CyGame::isTeamGame, "bool ()")
		.def("getNumHumanPlayers", &CyGame::getNumHumanPlayers, "int () - # of human players in-game")
		.def("isModem", &CyGame::isModem, "bool () - Using a modem? ")
		.def("setModem", &CyGame::setModem, "void (bool bModem) - Use a modem! (or don't)")
		.def("reviveActivePlayer", &CyGame::reviveActivePlayer, "void ()")
		.def("getGameTurn", &CyGame::getGameTurn, "int () - current game turn")
		.def("setGameTurn", &CyGame::setGameTurn, "void (iNewValue) - set current game turn")
		.def("getTurnYear", &CyGame::getTurnYear, "int (iGameTurn) - turn Time")
		.def("getGameTurnYear", &CyGame::getGameTurnYear)
		.def("getElapsedGameTurns", &CyGame::getElapsedGameTurns, "int () - Elapsed turns thus far")
		.def("getMaxTurns", &CyGame::getMaxTurns)
		.def("setMaxTurns", &CyGame::setMaxTurns)
		.def("changeMaxTurns", &CyGame::changeMaxTurns)
		.def("getMaxCityElimination", &CyGame::getMaxCityElimination)
		.def("setMaxCityElimination", &CyGame::setMaxCityElimination)
		.def("getNumAdvancedStartPoints", &CyGame::getNumAdvancedStartPoints)
		.def("setNumAdvancedStartPoints", &CyGame::setNumAdvancedStartPoints)
		.def("getStartTurn", &CyGame::getStartTurn, "int () - Returns the starting Turn (0 unless a scenario or advanced era start)")
		.def("getStartYear", &CyGame::getStartYear, "int () - Returns the starting year (e.g. 1492)")
		.def("setStartYear", &CyGame::setStartYear, "void () - Sets the starting year (e.g. 1492)")
		.def("getEstimateEndTurn", &CyGame::getEstimateEndTurn)
		.def("setEstimateEndTurn", &CyGame::setEstimateEndTurn)
		.def("getTurnSlice", &CyGame::getTurnSlice)
		.def("getMinutesPlayed", &CyGame::getMinutesPlayed, "Returns the number of minutes since the game began")
		.def("getTargetScore", &CyGame::getTargetScore)
		.def("getTurnTimerType", &CyGame::getTurnTimerType)
		.def("getNumGameTurnActive", &CyGame::getNumGameTurnActive)
		.def("countNumHumanGameTurnActive", &CyGame::countNumHumanGameTurnActive)
		.def("getNumCities", &CyGame::getNumCities, "int () - total cities in Game")
		.def("getTotalPopulation", &CyGame::getTotalPopulation, "int () - total game population")
		.def("getMaxPopulation", &CyGame::getMaxPopulation)
		.def("getMaxLand", &CyGame::getMaxLand)
		.def("getMaxFather", &CyGame::getMaxFather)
		.def("getInitPopulation", &CyGame::getInitPopulation)
		.def("getInitLand", &CyGame::getInitLand)
		.def("getInitFather", &CyGame::getInitFather)
		.def("getAIAutoPlay", &CyGame::getAIAutoPlay)
		.def("setAIAutoPlay", &CyGame::setAIAutoPlay)
		.def("isScoreDirty", &CyGame::isScoreDirty, "bool ()")
		.def("setScoreDirty", &CyGame::setScoreDirty)
		.def("isDebugMode", &CyGame::isDebugMode, "bool () - is the game in Debug Mode?")
		.def("toggleDebugMode", &CyGame::toggleDebugMode)
		.def("getPitbossTurnTime", &CyGame::getPitbossTurnTime, "int ()")
		.def("setPitbossTurnTime", &CyGame::setPitbossTurnTime, "void (int)")
		.def("isHotSeat", &CyGame::isHotSeat, "bool ()")
		.def("isPbem", &CyGame::isPbem, "bool ()")
		.def("isPitboss", &CyGame::isPitboss, "bool ()")
		.def("isSimultaneousTeamTurns", &CyGame::isSimultaneousTeamTurns, "bool ()")
		.def("isFinalInitialized", &CyGame::isFinalInitialized, "bool () - Returns whether or not the game initialization process has ended (game has started)")
		.def("getActivePlayer", &CyGame::getActivePlayer, "returns index of the active player")
		.def("setActivePlayer", &CyGame::setActivePlayer, "void (int /*PlayerTypes*/ eNewValue, bool bForceHotSeat)")
		.def("getPausePlayer", &CyGame::getPausePlayer, "int () - will get who paused us")
		.def("isPaused", &CyGame::isPaused, "bool () - will say if the game is paused")
		.def("getBestLandUnitCombat", &CyGame::getBestLandUnitCombat, "int ()")
		.def("getWinner", &CyGame::getWinner)
		.def("getVictory", &CyGame::getVictory)
		.def("setWinner", &CyGame::setWinner)
		.def("getGameState", &CyGame::getGameState)

		;
}
