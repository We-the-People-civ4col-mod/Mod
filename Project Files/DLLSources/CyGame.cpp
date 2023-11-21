//
// Python wrapper class for CvGame
//

#include "CvGameCoreDLL.h"
#include "CyGame.h"
#include "CvGameAI.h"
#include "CyGlobalContext.h"
#include "CyPlayer.h"
//#include "CvEnums.h"
#include "CyCity.h"
#include "CyDeal.h"
#include "CyReplayInfo.h"
#include "CvReplayInfo.h"
#include "CyPlot.h"
CyGame::CyGame() : m_pGame(NULL)
{
	m_pGame = &GC.getGameINLINE();
}
CyGame::CyGame(CvGame* pGame) : m_pGame(pGame)
{
}
CyGame::CyGame(CvGameAI* pGame) : m_pGame(pGame)
{
}
void CyGame::updateScore(bool bForce)
{
	if (m_pGame)
	{
		m_pGame->updateScore(bForce);
	}
}
void CyGame::cycleCities(bool bForward, bool bAdd)
{
	if (m_pGame)
		m_pGame->cycleCities(bForward, bAdd);
}
void CyGame::cycleSelectionGroups(bool bClear, bool bForward)
{
	if (m_pGame)
		m_pGame->cycleSelectionGroups(bClear, bForward);
}
bool CyGame::cyclePlotUnits(CyPlot* pPlot, bool bForward, bool bAuto, int iCount)
{
	return m_pGame ? m_pGame->cyclePlotUnits(pPlot->getPlot(), bForward, bAuto, iCount) : false;
}
void CyGame::selectionListMove(CyPlot* pPlot, bool bAlt, bool bShift, bool bCtrl)
{
	GC.getGameINLINE().selectionListMove(pPlot->getPlot(), bAlt, bShift, bCtrl);
}
void CyGame::selectionListGameNetMessage(int eMessage, int iData2, int iData3, int iData4, int iFlags, bool bAlt, bool bShift)
{
	GC.getGameINLINE().selectionListGameNetMessage(eMessage, iData2, iData3, iData4, iFlags, bAlt, bShift);
}
void CyGame::selectedCitiesGameNetMessage(int eMessage, int iData2, int iData3, int iData4, bool bOption, bool bAlt, bool bShift, bool bCtrl)
{
	GC.getGameINLINE().selectedCitiesGameNetMessage(eMessage, iData2, iData3, iData4, bOption, bAlt, bShift, bCtrl);
}
void CyGame::cityPushOrder(CyCity* pCity, OrderTypes eOrder, int iData, bool bAlt, bool bShift, bool bCtrl)
{
	GC.getGameINLINE().cityPushOrder(pCity->getCity(), eOrder, iData, bAlt, bShift, bCtrl);
}
int CyGame::getSymbolID(int iSymbol)
{
	if (m_pGame)
	{
		return GC.getSymbolID((FontSymbols)iSymbol);
	}
	return -1;
}
int CyGame::getProductionPerPopulation(int /*HurryTypes*/ eHurry)
{
	return m_pGame ? m_pGame->getProductionPerPopulation((HurryTypes) eHurry) : -1;
}
int CyGame::getAdjustedPopulationPercent(int /*VictoryTypes*/ eVictory)
{
	return m_pGame ? m_pGame->getAdjustedPopulationPercent((VictoryTypes) eVictory) : -1;
}
int CyGame::getAdjustedLandPercent(int /* VictoryTypes*/ eVictory)
{
	return m_pGame ? m_pGame->getAdjustedLandPercent((VictoryTypes) eVictory) : -1;
}
int CyGame::countCivPlayersAlive()
{
	return m_pGame ? m_pGame->countCivPlayersAlive() : -1;
}
int CyGame::countCivPlayersEverAlive()
{
	return m_pGame ? m_pGame->countCivPlayersEverAlive() : -1;
}
int CyGame::countCivTeamsAlive()
{
	return m_pGame ? m_pGame->countCivTeamsAlive() : -1;
}
int CyGame::countCivTeamsEverAlive()
{
	return m_pGame ? m_pGame->countCivTeamsEverAlive() : -1;
}
int CyGame::countHumanPlayersAlive()
{
	return m_pGame ? m_pGame->countHumanPlayersAlive() : -1;
}
int CyGame::countHumanPlayersEverAlive()
{
	return m_pGame ? m_pGame->countHumanPlayersEverAlive() : -1;
}
int CyGame::countTotalCivPower()
{
	return m_pGame ? m_pGame->countTotalCivPower() : -1;
}
int CyGame::getImprovementUpgradeTime(int /*ImprovementTypes*/ eImprovement)
{
	return m_pGame ? m_pGame->getImprovementUpgradeTime((ImprovementTypes) eImprovement) : -1;
}
int CyGame::getCurrentEra()
{
	return m_pGame ? (int) m_pGame->getCurrentEra() : (int) NO_ERA;
}
int CyGame::getActiveTeam()
{
	return m_pGame ? (int) m_pGame->getActiveTeam() : (int) NO_TEAM;
}
int /* CivilizationTypes */ CyGame::getActiveCivilizationType()
{
	return m_pGame ? (int) m_pGame->getActiveCivilizationType() : (int) NO_CIVILIZATION;
}
bool CyGame::isNetworkMultiPlayer()
{
	return m_pGame ? m_pGame->isNetworkMultiPlayer() : false;
}
bool CyGame::isGameMultiPlayer()
{
	return m_pGame ? m_pGame->isGameMultiPlayer() : false;
}
bool CyGame::isTeamGame()
{
	return m_pGame ? m_pGame->isTeamGame() : false;
}
bool CyGame::isModem()
{
	return m_pGame ? m_pGame->isModem() : true;	// err on the side of caution
}
void CyGame::setModem(bool bModem)
{
	if (m_pGame)
		m_pGame->setModem(bModem);
}
void CyGame::reviveActivePlayer()
{
	if (m_pGame)
		m_pGame->reviveActivePlayer();
}
int CyGame::getNumHumanPlayers()
{
	return m_pGame ? m_pGame->getNumHumanPlayers() : -1;
}
int CyGame::getGameTurn()
{
	return m_pGame ? m_pGame->getGameTurn() : -1;
}
void CyGame::setGameTurn(int iNewValue)
{
	if (m_pGame)
		m_pGame->setGameTurn(iNewValue);
}
int CyGame::getTurnYear(int iGameTurn)
{
	return  m_pGame ? m_pGame->getTurnYear(iGameTurn) : -1;
}
int CyGame::getGameTurnYear()
{
	return  m_pGame ? m_pGame->getGameTurnYear() : -1;
}
int CyGame::getElapsedGameTurns()
{
	return m_pGame ? m_pGame->getElapsedGameTurns() : -1;
}
int CyGame::getMaxTurns() const
{
	return (NULL != m_pGame ? m_pGame->getMaxTurns() : -1);
}
void CyGame::setMaxTurns(int iNewValue)
{
	if (NULL != m_pGame)
	{
		m_pGame->setMaxTurns(iNewValue);
	}
}
void CyGame::changeMaxTurns(int iChange)
{
	if (NULL != m_pGame)
	{
		m_pGame->changeMaxTurns(iChange);
	}
}
int CyGame::getMaxCityElimination() const
{
	return (NULL != m_pGame ? m_pGame->getMaxCityElimination() : -1);
}
void CyGame::setMaxCityElimination(int iNewValue)
{
	if (NULL != m_pGame)
	{
		m_pGame->setMaxCityElimination(iNewValue);
	}
}
int CyGame::getNumAdvancedStartPoints() const
{
	return (NULL != m_pGame ? m_pGame->getNumAdvancedStartPoints() : -1);
}
void CyGame::setNumAdvancedStartPoints(int iNewValue)
{
	if (NULL != m_pGame)
	{
		m_pGame->setNumAdvancedStartPoints(iNewValue);
	}
}
int CyGame::getStartTurn() const
{
	return (NULL != m_pGame ? m_pGame->getStartTurn() : -1);
}
int CyGame::getStartYear() const
{
	return (NULL != m_pGame ? m_pGame->getStartYear() : -1);
}
void CyGame::setStartYear(int iNewValue)
{
	if (NULL != m_pGame)
	{
		m_pGame->setStartYear(iNewValue);
	}
}
int CyGame::getEstimateEndTurn() const
{
	return (NULL != m_pGame ? m_pGame->getEstimateEndTurn() : -1);
}
void CyGame::setEstimateEndTurn(int iNewValue)
{
	if (NULL != m_pGame)
	{
		m_pGame->setEstimateEndTurn(iNewValue);
	}
}
int CyGame::getTurnSlice() const
{
	return (NULL != m_pGame ? m_pGame->getTurnSlice() : -1);
}
int CyGame::getMinutesPlayed() const
{
	return (NULL != m_pGame ? m_pGame->getMinutesPlayed() : 0);
}
int CyGame::getTargetScore() const
{
	return (NULL != m_pGame ? m_pGame->getTargetScore() : -1);
}
int /*TurnTimerTypes*/ CyGame::getTurnTimerType() const
{
	return (NULL != m_pGame ? m_pGame->getTurnTimerType() : NO_TURNTIMER);
}
int CyGame::getNumGameTurnActive()
{
	return m_pGame ? m_pGame->getNumGameTurnActive() : -1;
}
int CyGame::countNumHumanGameTurnActive()
{
	return m_pGame ? m_pGame->countNumHumanGameTurnActive() : -1;
}
int CyGame::getNumCities()
{
	return m_pGame ? m_pGame->getNumCities() : -1;
}
int CyGame::getTotalPopulation()
{
	return m_pGame ? m_pGame->getTotalPopulation() : -1;
}
int CyGame::getMaxPopulation() const
{
	return (NULL != m_pGame ? m_pGame->getMaxPopulation() : 0);
}
int CyGame::getMaxLand() const
{
	return (NULL != m_pGame ? m_pGame->getMaxLand() : 0);
}
int CyGame::getMaxFather() const
{
	return (NULL != m_pGame ? m_pGame->getMaxFather() : 0);
}
int CyGame::getInitPopulation() const
{
	return (NULL != m_pGame ? m_pGame->getInitPopulation() : 0);
}
int CyGame::getInitLand() const
{
	return (NULL != m_pGame ? m_pGame->getInitLand() : 0);
}
int CyGame::getInitFather() const
{
	return (NULL != m_pGame ? m_pGame->getInitFather() : 0);
}
int CyGame::getAIAutoPlay() const
{
	return (NULL != m_pGame ? m_pGame->getAIAutoPlay() : 0);
}
void CyGame::setAIAutoPlay(int iNewValue)
{
	if (m_pGame)
		m_pGame->setAIAutoPlay(iNewValue);
}
bool CyGame::isScoreDirty() const
{
	return m_pGame ? m_pGame->isScoreDirty() : false;
}
void CyGame::setScoreDirty(bool bNewValue)
{
	if (m_pGame)
		m_pGame->setScoreDirty(bNewValue);
}
bool CyGame::isDebugMode() const
{
	return m_pGame ? m_pGame->isDebugMode() : false;
}
void CyGame::toggleDebugMode()
{
	if (m_pGame)
		m_pGame->toggleDebugMode();
}
int CyGame::getPitbossTurnTime()
{
	return m_pGame ? m_pGame->getPitbossTurnTime() : -1;
}
void CyGame::setPitbossTurnTime(int iHours)
{
	if (m_pGame)
		m_pGame->setPitbossTurnTime(iHours);
}
bool CyGame::isHotSeat()
{
	return m_pGame ? m_pGame->isHotSeat() : false;
}
bool CyGame::isPbem()
{
	return m_pGame ? m_pGame->isPbem() : false;
}
bool CyGame::isPitboss()
{
	return m_pGame ? m_pGame->isPitboss() : false;
}
bool CyGame::isSimultaneousTeamTurns()
{
	return m_pGame ? m_pGame->isSimultaneousTeamTurns() : false;
}
bool CyGame::isFinalInitialized()
{
	return m_pGame ? m_pGame->isFinalInitialized() : false;
}
int /*PlayerTypes*/ CyGame::getActivePlayer()
{
	return m_pGame ? (int)m_pGame->getActivePlayer() : -1;
}
void CyGame::setActivePlayer(int /*PlayerTypes*/ eNewValue, bool bForceHotSeat)
{
	if (m_pGame)
		m_pGame->setActivePlayer((PlayerTypes)eNewValue, bForceHotSeat);
}
int CyGame::getPausePlayer()
{
	return m_pGame ? m_pGame->getPausePlayer() : -1;
}
bool CyGame::isPaused()
{
	return m_pGame ? m_pGame->isPaused() : false;
}
int CyGame::getBestLandUnitCombat()
{
	return m_pGame ? m_pGame->getBestLandUnitCombat() : -1;
}
int /*TeamTypes*/ CyGame::getWinner()
{
	return m_pGame ? (int)m_pGame->getWinner() : -1;
}
int /*VictoryTypes*/ CyGame::getVictory()
{
	return m_pGame ? (int)m_pGame->getVictory() : -1;
}
void CyGame::setWinner(int /*TeamTypes*/ eNewWinner, int /*VictoryTypes*/ eNewVictory)
{
	if (m_pGame)
		m_pGame->setWinner((TeamTypes) eNewWinner, (VictoryTypes) eNewVictory);
}
int /*GameStateTypes*/ CyGame::getGameState()
{
	return m_pGame ? (int)m_pGame->getGameState() : -1;
}
int /* HandicapTypes */ CyGame::getHandicapType()
{
	return m_pGame ? (int) m_pGame->getHandicapType() : (int) NO_HANDICAP;
}
CalendarTypes CyGame::getCalendar() const
{
	return m_pGame ? m_pGame->getCalendar() : CALENDAR_DEFAULT;
}
int /*EraTypes*/ CyGame::getStartEra()
{
	return m_pGame ? m_pGame->getStartEra() : -1;
}
int /*GameSpeedTypes*/ CyGame::getGameSpeedType()
{
	return m_pGame ? m_pGame->getGameSpeedType() : -1;
}
int CyGame::getCargoYieldCapacity()
{
	return m_pGame ? m_pGame->getCargoYieldCapacity() : -1;
}
int /*PlayerTypes*/ CyGame::getRankPlayer(int iRank)
{
	return m_pGame ? m_pGame->getRankPlayer(iRank) : -1;
}
int CyGame::getPlayerScore(int /*PlayerTypes*/ ePlayer)
{
	return m_pGame ? m_pGame->getPlayerScore((PlayerTypes)ePlayer) : -1;
}
int /*TeamTypes*/ CyGame::getRankTeam(int iRank)
{
	return m_pGame ? m_pGame->getRankTeam(iRank) : -1;
}
int CyGame::getTeamRank(int /*TeamTypes*/ eTeam)
{
	return m_pGame ? m_pGame->getTeamRank((TeamTypes)eTeam) : -1;
}
int CyGame::getTeamScore(int /*TeamTypes*/ eTeam)
{
	return m_pGame ? m_pGame->getTeamScore((TeamTypes)eTeam) : -1;
}
bool CyGame::isOption(int /*GameOptionTypes*/ eIndex)
{
	return m_pGame ? m_pGame->isOption((GameOptionTypes)eIndex) : -1;
}
void CyGame::setOption(int /*GameOptionTypes*/ eIndex, bool bEnabled)
{
	if (m_pGame)
		m_pGame->setOption((GameOptionTypes)eIndex, bEnabled);
}
bool CyGame::isMPOption(int /*MultiplayerOptionTypes*/ eIndex)
{
	return m_pGame ? m_pGame->isMPOption((MultiplayerOptionTypes)eIndex) : -1;
}
bool CyGame::isForcedControl(int /*ForceControlTypes*/ eIndex)
{
	return m_pGame ? m_pGame->isForcedControl((ForceControlTypes)eIndex) : -1;
}
int CyGame::getUnitCreatedCount(int /*UnitTypes*/ eIndex)
{
	return m_pGame ? m_pGame->getUnitCreatedCount((UnitTypes)eIndex) : -1;
}
int CyGame::getUnitClassCreatedCount(int /*UnitClassTypes*/ eIndex)
{
	return m_pGame ? m_pGame->getUnitClassCreatedCount((UnitClassTypes)eIndex) : -1;
}
int CyGame::getBuildingClassCreatedCount(int /*BuildingClassTypes*/ eIndex)
{
	return m_pGame ? m_pGame->getBuildingClassCreatedCount((BuildingClassTypes) eIndex) : -1;
}
bool CyGame::isVictoryValid(int /*VictoryTypes*/ eIndex)
{
	return m_pGame ? m_pGame->isVictoryValid((VictoryTypes)eIndex) : false;
}
bool CyGame::isSpecialUnitValid(int /*SpecialUnitTypes*/ eSpecialUnitType)
{
	return m_pGame ? m_pGame->isSpecialUnitValid((SpecialUnitTypes)eSpecialUnitType) : false;
}
void CyGame::makeSpecialUnitValid(int /*SpecialUnitTypes*/ eSpecialUnitType)
{
	if (m_pGame)
		m_pGame->makeSpecialUnitValid((SpecialUnitTypes) eSpecialUnitType);
}
bool CyGame::isSpecialBuildingValid(int /*SpecialBuildingTypes*/ eIndex)
{
	return m_pGame ? m_pGame->isSpecialBuildingValid((SpecialBuildingTypes)eIndex) : false;
}
void CyGame::makeSpecialBuildingValid(int /*SpecialBuildingTypes*/ eIndex)
{
	if (m_pGame)
		m_pGame->makeSpecialBuildingValid((SpecialBuildingTypes) eIndex);
}

// R&R, ray, Goody Enhancement - START
bool CyGame::isUniqueGoodyValid(int /*GoodyTypes*/ eIndex)
{
	return m_pGame ? m_pGame->isUniqueGoodyValid((GoodyTypes)eIndex) : false;
}
void CyGame::setUniqueGoodyValid(int /*GoodyTypes*/ eIndex, bool bValid)
{
	if (m_pGame)
		m_pGame->setUniqueGoodyValid((GoodyTypes) eIndex, bValid);
}
// R&R, ray, Goody Enhancement

bool CyGame::isInAdvancedStart()
{
	return m_pGame ? m_pGame->isInAdvancedStart() : false;
}
std::string CyGame::getScriptData() const
{
	return m_pGame ? m_pGame->getScriptData() : "";
}
void CyGame::setScriptData(std::string szNewValue)
{
	if (m_pGame)
		m_pGame->setScriptData(szNewValue);
}
void CyGame::setName(char const* szNewValue)
{
	if (m_pGame)
		m_pGame->setName(szNewValue);
}
std::wstring CyGame::getName()
{
	return m_pGame ? m_pGame->getName() : "";
}
int CyGame::getIndexAfterLastDeal()
{
	return m_pGame ? m_pGame->getIndexAfterLastDeal() : -1;
}
int CyGame::getNumDeals()
{
	return m_pGame ? m_pGame->getNumDeals() : -1;
}
CyDeal* CyGame::getDeal(int iID)
{
	if (m_pGame)
	{
		return new CyDeal(m_pGame->getDeal(iID));
	}
	else
	{
		return NULL;
	}
}
CyDeal* CyGame::addDeal()
{
	if (m_pGame)
	{
		return new CyDeal(m_pGame->addDeal());
	}
	else
	{
		return NULL;
	}
}
void CyGame::deleteDeal(int iID)
{
	if (m_pGame)
	{
		m_pGame->deleteDeal(iID);
	}
}
CvRandom& CyGame::getMapRand()
{
	FAssert(m_pGame);
	return (m_pGame->getMapRand());
}
int CyGame::getMapRandNum(int iNum, char const* pszLog)
{
	return m_pGame ? m_pGame->getMapRandNum(iNum, pszLog) : -1;
}
CvRandom& CyGame::getSorenRand()
{
	FAssert(m_pGame);
	return (m_pGame->getSorenRand());
}
int CyGame::getSorenRandNum(int iNum, char const* pszLog)
{
	return m_pGame ? m_pGame->getSorenRandNum(iNum, pszLog) : -1;
}
int CyGame::calculateSyncChecksum()
{
	return m_pGame ? m_pGame->calculateSyncChecksum(NULL) : -1;
}
int CyGame::calculateOptionsChecksum()
{
	return m_pGame ? m_pGame->calculateOptionsChecksum() : -1;
}
// JS - can't access protected member declared in class CvGame
bool CyGame::GetWorldBuilderMode() const				// remove once CvApp is exposed
{
	return gDLL->GetWorldBuilderMode();
}
bool CyGame::isPitbossHost() const				// remove once CvApp is exposed
{
	return gDLL->IsPitbossHost();
}
int CyGame::getCurrentLanguage() const				// remove once CvApp is exposed
{
	// Used by the language dropdown menu in game option GUI (exe hardcoded)
	// 

	for (int i = 0; i < CvGameText::getNumLanguagesStatic(); ++i)
	{
		if (CvGameText::getLanguageAtIndex(i) == gDLL->getCurrentLanguage())
		{
			return i;
		}
	}
	// We normally won't end up here, but switching to another mod or manual changing xml/ini might make it possible.
	// Since we need to return something, just use the first language.
	return 0;
}
void CyGame::setCurrentLanguage(int iNewLanguage)			// remove once CvApp is exposed
{
	// The user selected a new language form the options menu (which is inside the exe)
	
	// First tell CvGameText that a new language is selected.
	// The reason is explained in CvGameText::getNumLanguages()
	CvGameText::setChangeLanguage();
	// Convert the menu index to the index used in the filesystem/ini file and store this value.
	gDLL->setCurrentLanguage(CvGameText::getLanguageAtIndex(iNewLanguage));

	// clean up unused strings
	GC.cleanInfoStrings();
}
int CyGame::getReplayMessageTurn(int i) const
{
	return (NULL != m_pGame ? m_pGame->getReplayMessageTurn(i) : -1);
}
ReplayMessageTypes CyGame::getReplayMessageType(int i) const
{
	return (NULL != m_pGame ? m_pGame->getReplayMessageType(i) : NO_REPLAY_MESSAGE);
}
int CyGame::getReplayMessagePlotX(int i) const
{
	return (NULL != m_pGame ? m_pGame->getReplayMessagePlotX(i) : -1);
}
int CyGame::getReplayMessagePlotY(int i) const
{
	return (NULL != m_pGame ? m_pGame->getReplayMessagePlotY(i) : -1);
}
int CyGame::getReplayMessagePlayer(int i) const
{
	return (NULL != m_pGame ? m_pGame->getReplayMessagePlayer(i) : -1);
}
ColorTypes CyGame::getReplayMessageColor(int i) const
{
	return (NULL != m_pGame ? m_pGame->getReplayMessageColor(i) : NO_COLOR);
}
std::wstring CyGame::getReplayMessageText(int i) const
{
	return (NULL != m_pGame ? m_pGame->getReplayMessageText(i) : L"");
}
uint CyGame::getNumReplayMessages() const
{
	return (NULL != m_pGame ? m_pGame->getNumReplayMessages() : 0);
}
CyReplayInfo* CyGame::getReplayInfo() const
{
	return (NULL != m_pGame ? (new CyReplayInfo(m_pGame->getReplayInfo())) : NULL);
}
void CyGame::saveReplay(int iPlayer)
{
	if (m_pGame)
	{
		m_pGame->saveReplay((PlayerTypes)iPlayer);
	}
}
void CyGame::addPlayer(int eNewPlayer, int eLeader, int eCiv)
{
	if (m_pGame)
	{
		m_pGame->addPlayer((PlayerTypes)eNewPlayer, (LeaderHeadTypes)eLeader, (CivilizationTypes)eCiv);
	}
}
void CyGame::setPlotExtraYield(int iX, int iY, int /*YieldTypes*/ eYield, int iExtraYield)
{
	if (m_pGame)
	{
		m_pGame->setPlotExtraYield(iX, iY, (YieldTypes)eYield, iExtraYield);
	}
}
bool CyGame::isCivEverActive(int /*CivilizationTypes*/ eCivilization)
{
	return (NULL != m_pGame ? m_pGame->isCivEverActive((CivilizationTypes)eCivilization) : false);
}
bool CyGame::isLeaderEverActive(int /*LeaderHeadTypes*/ eLeader)
{
	return (NULL != m_pGame ? m_pGame->isLeaderEverActive((LeaderHeadTypes)eLeader) : false);
}
bool CyGame::isUnitEverActive(int /*UnitTypes*/ eUnit)
{
	return (NULL != m_pGame ? m_pGame->isUnitEverActive((UnitTypes)eUnit) : false);
}
bool CyGame::isBuildingEverActive(int /*BuildingTypes*/ eBuilding)
{
	return (NULL != m_pGame ? m_pGame->isBuildingEverActive((BuildingTypes)eBuilding) : false);
}
bool CyGame::isEventActive(int /*EventTriggerTypes*/ eTrigger)
{
	return (NULL != m_pGame ? m_pGame->isEventActive((EventTriggerTypes)eTrigger) : false);
}
int /*TeamTypes*/ CyGame::getFatherTeam(int /*FatherTypes*/ eFather)
{
	return (NULL != m_pGame ? m_pGame->getFatherTeam((FatherTypes)eFather) : -1);
}
int CyGame::getFatherGameTurn(int /*FatherTypes*/ eFather)
{
	return (NULL != m_pGame ? m_pGame->getFatherGameTurn((FatherTypes)eFather) : -1);
}
void CyGame::setFatherTeam(int /*FatherTypes*/ eFather, int /*TeamTypes*/ eTeam)
{
	if (m_pGame)
	{
		m_pGame->setFatherTeam((FatherTypes) eFather, (TeamTypes) eTeam);
	}
}
int CyGame::getFatherCategoryPosition(int /*FatherTypes*/ eFather)
{
	return (NULL != m_pGame ? m_pGame->getFatherCategoryPosition((FatherTypes)eFather) : -1);
}

// R&R, ray, Correct Geographical Placement of Natives - START
bool CyGame::isWBNorthAmericanNative()
{
	return (NULL != m_pGame ? m_pGame->isWBNorthAmericanNative() : -1);
}

void CyGame::setWBNorthAmericanNative(bool bValue)
{
	if (m_pGame)
	{
		m_pGame->setWBNorthAmericanNative(bValue);
	}
}

bool CyGame::isWBSouthAmericanNative()
{
	return (NULL != m_pGame ? m_pGame->isWBSouthAmericanNative() : -1);
}

void CyGame::setWBSouthAmericanNative(bool bValue)
{
	if (m_pGame)
	{
		m_pGame->setWBSouthAmericanNative(bValue);
	}
}

bool CyGame::isWBCentralAmericanNative()
{
	return (NULL != m_pGame ? m_pGame->isWBCentralAmericanNative() : -1);
}

void CyGame::setWBCentralAmericanNative(bool bValue)
{
	if (m_pGame)
	{
		m_pGame->setWBCentralAmericanNative(bValue);
	}
}
// R&R, ray, Correct Geographical Placement of Natives - END

// < JAnimals Mod Start >
int /*PlayerTypes*/ CyGame::getBarbarianPlayer()
{
	return (NULL != m_pGame ? m_pGame->getBarbarianPlayer() : -1);
}

bool CyGame::hasBarbarianPlayer()
{
	return (NULL != m_pGame ? m_pGame->hasBarbarianPlayer() : false);
}

void CyGame::setBarbarianPlayer(int /*PlayerTypes*/ eNewValue)
{
	if (m_pGame)
	{
		m_pGame->setBarbarianPlayer((PlayerTypes) eNewValue);
	}
}

bool CyGame::isBarbarianPlayer(int /*PlayerTypes*/ ePlayer)
{
	return (NULL != m_pGame ? m_pGame->isBarbarianPlayer((PlayerTypes) ePlayer) : false);
}

int /*PlayerTypes*/ CyGame::getNextPlayerType()
{
	return (NULL != m_pGame ? m_pGame->getNextPlayerType() : -1);
}
// < JAnimals Mod End >


// R&R, ray, the Church - START
int /*PlayerTypes*/ CyGame::getChurchPlayer()
{
	return (NULL != m_pGame ? m_pGame->getChurchPlayer() : -1);
}

bool CyGame::hasChurchPlayer()
{
	return (NULL != m_pGame ? m_pGame->hasChurchPlayer() : false);
}

void CyGame::setChurchPlayer(int /*PlayerTypes*/ eNewValue)
{
	if (m_pGame)
	{
		m_pGame->setChurchPlayer((PlayerTypes) eNewValue);
	}
}

bool CyGame::isChurchPlayer(int /*PlayerTypes*/ ePlayer)
{
	return (NULL != m_pGame ? m_pGame->isChurchPlayer((PlayerTypes) ePlayer) : false);
}
// R&R, ray, the Church - END
