//
// Python wrapper class for CvPlayer
//
#include "CvGameCoreDLL.h"
#include "CyPlayer.h"
#include "CyUnit.h"
#include "CyCity.h"
#include "CyArea.h"
#include "CyPlot.h"
#include "CvPlayerAI.h"
//#include "CvEnums.h"
#include "CvCity.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CySelectionGroup.h"
#include "CvDLLPythonIFaceBase.h"
#include "CvGlobals.h"
#include "CyTradeRoute.h"
#include "CyTradeRouteGroup.h" // R&R mod, vetiarvind, trade groups
#include "CyData.h"


CyPlayer::CyPlayer() : m_pPlayer(NULL)
{
}
CyPlayer::CyPlayer(CvPlayer* pPlayer) : m_pPlayer(pPlayer)
{
}
int CyPlayer::startingPlotRange()
{
	return m_pPlayer ? m_pPlayer->startingPlotRange() : -1;
}
CyPlot* CyPlayer::findStartingPlot(bool bRandomize)
{
	return m_pPlayer ? new CyPlot(m_pPlayer->findStartingPlot(bRandomize)) : NULL;
}
CyCity* CyPlayer::initCity(int x, int y)
{
	return m_pPlayer ? new CyCity(m_pPlayer->initCity(Coordinates(x, y), true)) : NULL;
}
void CyPlayer::acquireCity(CyCity* pCity, bool bConquest, bool bTrade)
{
	if (m_pPlayer)
		m_pPlayer->acquireCity(pCity->getCity(), bConquest, bTrade);
}
void CyPlayer::killCities()
{
	if (m_pPlayer)
		m_pPlayer->killCities();
}
std::wstring CyPlayer::getNewCityName()
{
	if (m_pPlayer == NULL) return L"";
	return m_pPlayer->getNewCityName();
}
CyUnit* CyPlayer::initUnit(int /*UnitTypes*/ iIndex, int iProfession, int iX, int iY, UnitAITypes eUnitAI, DirectionTypes eFacingDirection, int iYieldStored)
{
	return m_pPlayer ? new CyUnit(m_pPlayer->initUnit((UnitTypes) iIndex, (ProfessionTypes) iProfession, iX, iY, eUnitAI, eFacingDirection, iYieldStored)) : NULL;
}
CyUnit* CyPlayer::initEuropeUnit(int /*UnitTypes*/ eUnit, UnitAITypes eUnitAI, DirectionTypes eFacingDirection)
{
	return m_pPlayer ? new CyUnit(m_pPlayer->initEuropeUnit((UnitTypes) eUnit, eUnitAI, eFacingDirection)) : NULL;
}
void CyPlayer::killUnits()
{
	if (m_pPlayer)
		m_pPlayer->killUnits();
}
bool CyPlayer::hasTrait(int /*TraitTypes*/ iIndex)
{
	return m_pPlayer ? m_pPlayer->hasTrait((TraitTypes) iIndex) : false;
}
bool CyPlayer::isHuman()
{
	return m_pPlayer ? m_pPlayer->isHuman() : false;
}
bool CyPlayer::isNative()
{
	return m_pPlayer ? m_pPlayer->isNative() : false;
}
std::wstring CyPlayer::getName()
{
	if (m_pPlayer == NULL) return L"";
	return m_pPlayer->getName();
}
std::wstring CyPlayer::getNameForm(int iForm)
{
	return m_pPlayer->getName(static_cast<uint>(iForm));
}
std::wstring CyPlayer::getNameKey()
{
	if (m_pPlayer == NULL) return L"";
	return m_pPlayer->getNameKey();
}
std::wstring CyPlayer::getCivilizationDescription(int iForm)
{
	if (m_pPlayer == NULL) return L"";
	return m_pPlayer->getCivilizationDescription(static_cast<uint>(iForm));
}
std::wstring CyPlayer::getCivilizationDescriptionKey()
{
	if (m_pPlayer == NULL) return L"";
	return m_pPlayer->getCivilizationDescriptionKey();
}
std::wstring CyPlayer::getCivilizationShortDescription(int iForm)
{
	if (m_pPlayer == NULL) return L"";
	return m_pPlayer->getCivilizationShortDescription(static_cast<uint>(iForm));
}
std::wstring CyPlayer::getCivilizationShortDescriptionKey()
{
	if (m_pPlayer == NULL) return L"";
	return m_pPlayer->getCivilizationShortDescriptionKey();
}
std::wstring CyPlayer::getCivilizationAdjective(int iForm)
{
	if (m_pPlayer == NULL) return L"";
	return m_pPlayer->getCivilizationAdjective(static_cast<uint>(iForm));
}
std::wstring CyPlayer::getCivilizationAdjectiveKey( )
{
	if (m_pPlayer == NULL) return L"";
	return m_pPlayer->getCivilizationAdjectiveKey();
}
std::wstring CyPlayer::getWorstEnemyName()
{
	if (m_pPlayer == NULL) return L"";
	return m_pPlayer->getWorstEnemyName();
}
int /*ArtStyleTypes*/ CyPlayer::getArtStyleType()
{
	return m_pPlayer ? (int) m_pPlayer->getArtStyleType() : -1;
}
std::string CyPlayer::getUnitButton(int eUnit)
{
	if (m_pPlayer == NULL) return "";
	return m_pPlayer->getUnitButton(static_cast<UnitTypes>(eUnit));
}
int CyPlayer::findBestFoundValue( )
{
	return m_pPlayer ? m_pPlayer->findBestFoundValue() : -1;
}
int CyPlayer::countNumCoastalCities()
{
	return m_pPlayer ? m_pPlayer->countNumCoastalCities() : -1;
}
int CyPlayer::countNumCoastalCitiesByArea(CyArea* pArea)
{
	return m_pPlayer ? m_pPlayer->countNumCoastalCitiesByArea(pArea->getArea()) : -1;
}
int CyPlayer::countTotalCulture()
{
	return m_pPlayer ? m_pPlayer->countTotalCulture() : -1;
}
int CyPlayer::countTotalYieldStored(int /*YieldTypes*/ eYield)
{
	return m_pPlayer ? m_pPlayer->countTotalYieldStored((YieldTypes) eYield) : -1;
}
int CyPlayer::countCityFeatures(int /*FeatureTypes*/ eFeature)
{
	return m_pPlayer ? m_pPlayer->countCityFeatures((FeatureTypes) eFeature) : -1;
}
int CyPlayer::countNumBuildings(int /*BuildingTypes*/ eBuilding)
{
	return m_pPlayer ? m_pPlayer->countNumBuildings((BuildingTypes) eBuilding) : -1;
}
bool CyPlayer::canContact(int /*PlayerTypes*/ ePlayer)
{
	return m_pPlayer ? m_pPlayer->canContact((PlayerTypes)ePlayer) : false;
}
void CyPlayer::contact(int /*PlayerTypes*/ ePlayer)
{
	if (m_pPlayer)
		m_pPlayer->contact((PlayerTypes)ePlayer);
}
bool CyPlayer::canTradeWith(int /*PlayerTypes*/ eWhoTo)
{
	return m_pPlayer ? m_pPlayer->canTradeWith((PlayerTypes)eWhoTo) : false;
}
bool CyPlayer::canTradeItem(int /*PlayerTypes*/ eWhoTo, TradeData item, bool bTestDenial)
{
	return m_pPlayer ? m_pPlayer->canTradeItem((PlayerTypes)eWhoTo, item, bTestDenial) : false;
}
DenialTypes CyPlayer::getTradeDenial(int /*PlayerTypes*/ eWhoTo, TradeData item)
{
	return m_pPlayer ? m_pPlayer->getTradeDenial((PlayerTypes)eWhoTo, item) : NO_DENIAL;
}
bool CyPlayer::canStopTradingWithTeam(int /*TeamTypes*/ eTeam)
{
	return m_pPlayer ? m_pPlayer->canStopTradingWithTeam((TeamTypes) eTeam) : false;
}
void CyPlayer::stopTradingWithTeam(int /*TeamTypes*/ eTeam)
{
	if (m_pPlayer)
		m_pPlayer->stopTradingWithTeam((TeamTypes) eTeam);
}
void CyPlayer::killAllDeals()
{
	if (m_pPlayer)
		m_pPlayer->killAllDeals();
}
bool CyPlayer::isTurnActive()
{
	return m_pPlayer ? m_pPlayer->isTurnActive() : false;
}
void CyPlayer::findNewCapital()
{
	if (m_pPlayer)
		m_pPlayer->findNewCapital();
}
bool CyPlayer::canRaze(CyCity* pCity)
{
	return m_pPlayer ? m_pPlayer->canRaze(pCity->getCity()) : false;
}
void CyPlayer::raze(CyCity* pCity)
{
	if (m_pPlayer)
		m_pPlayer->raze(pCity->getCity());
}
void CyPlayer::disband(CyCity* pCity)
{
	if (m_pPlayer)
		m_pPlayer->disband(pCity->getCity());
}
bool CyPlayer::canReceiveGoody(CyPlot* pPlot, int /*GoodyTypes*/ iIndex, CyUnit* pUnit)
{
	return m_pPlayer ? m_pPlayer->canReceiveGoody(pPlot->getPlot(), (GoodyTypes) iIndex, pUnit->getUnit()) : false;
}
void CyPlayer::receiveGoody(CyPlot* pPlot, int /*GoodyTypes*/ iIndex, CyUnit* pUnit)
{
	if (m_pPlayer)
		m_pPlayer->receiveGoody(pPlot->getPlot(), (GoodyTypes) iIndex, pUnit->getUnit());
}
void CyPlayer::doGoody(CyPlot* pPlot, CyUnit* pUnit)
{
	if (m_pPlayer)
		m_pPlayer->doGoody(pPlot->getPlot(), pUnit->getUnit());
}
bool CyPlayer::canFound(int iX, int iY)
{
	return m_pPlayer ? m_pPlayer->canFound(Coordinates(iX, iY)) : false;
}
void CyPlayer::found(int x, int y)
{
	if (m_pPlayer)
		m_pPlayer->found(Coordinates(x,y));
}
bool CyPlayer::canTrain(int /*UnitTypes*/ eUnit, bool bContinue, bool bTestVisible)
{
	return m_pPlayer ? m_pPlayer->canTrain((UnitTypes)eUnit, bContinue, bTestVisible) : false;
}
bool CyPlayer::canConstruct(int /*BuildingTypes*/eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost)
{
	return m_pPlayer ? m_pPlayer->canConstruct((BuildingTypes)eBuilding, bContinue, bTestVisible, bIgnoreCost) : false;
}
int CyPlayer::getUnitYieldProductionNeeded(int /*UnitTypes*/ eUnit, int /*YieldTypes*/ eYield) const
{
	return m_pPlayer ? m_pPlayer->getYieldProductionNeeded((UnitTypes) eUnit, (YieldTypes) eYield) : false;
}
int CyPlayer::getBuildingYieldProductionNeeded(int /*BuildingTypes*/ eBuilding, int /*YieldTypes*/ eYield) const
{
	return m_pPlayer ? m_pPlayer->getYieldProductionNeeded((BuildingTypes) eBuilding, (YieldTypes) eYield) : false;
}
int CyPlayer::getBuildingClassPrereqBuilding(int /*BuildingTypes*/ eBuilding, int /*BuildingClassTypes*/ ePrereqBuildingClass, int iExtra)
{
	return m_pPlayer ? m_pPlayer->getBuildingClassPrereqBuilding((BuildingTypes) eBuilding, (BuildingClassTypes) ePrereqBuildingClass, iExtra) : -1;
}
void CyPlayer::removeBuildingClass(int /*BuildingClassTypes*/ eBuildingClass)
{
	if (m_pPlayer)
		m_pPlayer->removeBuildingClass((BuildingClassTypes)eBuildingClass);
}
bool CyPlayer::canBuild(CyPlot* pPlot, int /*BuildTypes*/ eBuild, bool bTestEra, bool bTestVisible)
{
	return m_pPlayer ? m_pPlayer->canBuild(pPlot->getPlot(), (BuildTypes)eBuild, bTestEra, bTestVisible) : false;
}
int /*RouteTypes*/ CyPlayer::getBestRoute(CyPlot* pPlot) const
{
	return m_pPlayer ? (int) m_pPlayer->getBestRoute(NULL != pPlot ? pPlot->getPlot() : NULL) : -1;
}
int CyPlayer::getImprovementUpgradeRate() const
{
	return m_pPlayer ? m_pPlayer->getImprovementUpgradeRate() : -1;
}
int CyPlayer::calculateTotalYield(int /*YieldTypes*/ eYield)
{
	return m_pPlayer ? m_pPlayer->calculateTotalYield((YieldTypes)eYield) : -1;
}
bool CyPlayer::isCivic(int /*CivicTypes*/ eCivic)
{
	return m_pPlayer ? m_pPlayer->isCivic((CivicTypes)eCivic) : false;
}
bool CyPlayer::canDoCivics(int /*CivicTypes*/ eCivic)
{
	return m_pPlayer ? m_pPlayer->canDoCivics((CivicTypes)eCivic) : false;
}
int CyPlayer::greatGeneralThreshold()
{
	return m_pPlayer ? m_pPlayer->greatGeneralThreshold() : -1;
}
// R&R, ray, Great Admirals - START
int CyPlayer::greatAdmiralThreshold()
{
	return m_pPlayer ? m_pPlayer->greatAdmiralThreshold() : -1;
}
// R&R, ray, Great Admirals - END

// WTP, ray, increase threshold if more than X units waiting on the docks - START
int CyPlayer::getImmigrationThresholdModifierFromUnitsWaitingOnDock()
{
	return m_pPlayer ? m_pPlayer->getImmigrationThresholdModifierFromUnitsWaitingOnDock() : -1;
}
// WTP, ray, increase threshold if more than X units waiting on the docks - END

int CyPlayer::immigrationThreshold()
{
	return m_pPlayer ? m_pPlayer->immigrationThreshold() : -1;
}
int CyPlayer::revolutionEuropeUnitThreshold()
{
	return m_pPlayer ? m_pPlayer->revolutionEuropeUnitThreshold() : -1;
}
CyPlot* CyPlayer::getStartingPlot()
{
	if (!m_pPlayer)
	{
		return NULL;
	}
	return new CyPlot(m_pPlayer->getStartingPlot());
}
void CyPlayer::setStartingPlot(CyPlot* pPlot, bool bUpdateStartDist)
{
	if (!m_pPlayer)
	{
		return;
	}
	m_pPlayer->setStartingPlot(NULL != pPlot ? pPlot->getPlot() : NULL, bUpdateStartDist);
}
int CyPlayer::getTotalPopulation()
{
	return m_pPlayer ? m_pPlayer->getTotalPopulation() : -1;
}
int CyPlayer::getAveragePopulation()
{
	return m_pPlayer ? m_pPlayer->getAveragePopulation() : -1;
}
long CyPlayer::getRealPopulation()
{
	return m_pPlayer ? m_pPlayer->getRealPopulation() : -1;
}
int CyPlayer::getTotalLand()
{
	return m_pPlayer ? m_pPlayer->getTotalLand() : -1;
}
int CyPlayer::getTotalLandScored()
{
	return m_pPlayer ? m_pPlayer->getTotalLandScored() : -1;
}
int CyPlayer::getGold()
{
	return m_pPlayer ? m_pPlayer->getGold() : -1;
}
void CyPlayer::setGold(int iNewValue)
{
	if (m_pPlayer)
		m_pPlayer->setGold(iNewValue);
}
void CyPlayer::changeGold(int iChange)
{
	if (m_pPlayer)
	{
		OOS_LOG_3("Python change gold", m_pPlayer->getID(), iChange);
		m_pPlayer->changeGold(iChange);
	}
}
int CyPlayer::getAdvancedStartPoints()
{
	return m_pPlayer ? m_pPlayer->getAdvancedStartPoints() : -1;
}
void CyPlayer::setAdvancedStartPoints(int iNewValue)
{
	if (m_pPlayer)
		m_pPlayer->setAdvancedStartPoints(iNewValue);
}
void CyPlayer::changeAdvancedStartPoints(int iChange)
{
	if (m_pPlayer)
		m_pPlayer->changeAdvancedStartPoints(iChange);
}
int CyPlayer::getAdvancedStartUnitCost(int /*UnitTypes*/ eUnit, bool bAdd, CyPlot* pPlot)
{
	return m_pPlayer ? m_pPlayer->getAdvancedStartUnitCost((UnitTypes) eUnit, bAdd, NULL != pPlot ? pPlot->getPlot() : NULL) : -1;
}
int CyPlayer::getAdvancedStartCityCost(bool bAdd, CyPlot* pPlot)
{
	return m_pPlayer ? m_pPlayer->getAdvancedStartCityCost(bAdd, NULL != pPlot ? pPlot->getPlot() : NULL) : -1;
}
int CyPlayer::getAdvancedStartPopCost(bool bAdd, CyCity* pCity)
{
	return m_pPlayer ? m_pPlayer->getAdvancedStartPopCost(bAdd, pCity->getCity()) : -1;
}
int CyPlayer::getAdvancedStartCultureCost(bool bAdd, CyCity* pCity)
{
	return m_pPlayer ? m_pPlayer->getAdvancedStartCultureCost(bAdd, pCity->getCity()) : -1;
}
int CyPlayer::getAdvancedStartBuildingCost(int /*BuildingTypes*/ eBuilding, bool bAdd, CyCity* pCity)
{
	return m_pPlayer ? m_pPlayer->getAdvancedStartBuildingCost((BuildingTypes) eBuilding, bAdd, pCity->getCity()) : -1;
}
int CyPlayer::getAdvancedStartImprovementCost(int /*ImprovementTypes*/ eImprovement, bool bAdd, CyPlot* pPlot)
{
	return m_pPlayer ? m_pPlayer->getAdvancedStartImprovementCost((ImprovementTypes) eImprovement, bAdd, NULL != pPlot ? pPlot->getPlot() : NULL) : -1;
}
int CyPlayer::getAdvancedStartRouteCost(int /*RouteTypes*/ eRoute, bool bAdd, CyPlot* pPlot)
{
	return m_pPlayer ? m_pPlayer->getAdvancedStartRouteCost((RouteTypes) eRoute, bAdd, NULL != pPlot ? pPlot->getPlot() : NULL) : -1;
}
int CyPlayer::getAdvancedStartVisibilityCost(bool bAdd, CyPlot* pPlot)
{
	return m_pPlayer ? m_pPlayer->getAdvancedStartVisibilityCost(bAdd, NULL != pPlot ? pPlot->getPlot() : NULL) : -1;
}
void CyPlayer::createGreatGeneral(int eGreatGeneralUnit, bool bIncrementExperience, int iX, int iY)
{
	if (m_pPlayer)
	{
		m_pPlayer->createGreatGeneral((UnitTypes)eGreatGeneralUnit, bIncrementExperience, Coordinates(iX, iY));
	}
}
int CyPlayer::getGreatGeneralsCreated()
{
	return m_pPlayer ? m_pPlayer->getGreatGeneralsCreated() : -1;
}
int CyPlayer::getGreatGeneralsThresholdModifier()
{
	return m_pPlayer ? m_pPlayer->getGreatGeneralsThresholdModifier() : -1;
}

// R&R, ray, Great Admirals - START
void CyPlayer::createGreatAdmiral(int eGreatAdmiralUnit, bool bIncrementExperience, int iX, int iY)
{
	if (m_pPlayer)
	{
		m_pPlayer->createGreatAdmiral((UnitTypes)eGreatAdmiralUnit, bIncrementExperience, Coordinates(iX, iY));
	}
}
int CyPlayer::getGreatAdmiralsCreated()
{
	return m_pPlayer ? m_pPlayer->getGreatAdmiralsCreated() : -1;
}
int CyPlayer::getGreatAdmiralsThresholdModifier()
{
	return m_pPlayer ? m_pPlayer->getGreatAdmiralsThresholdModifier() : -1;
}
// R&R, ray, Great Admirals -END

// WTP, ray, Lieutenants and Captains - START
void CyPlayer::createBraveLieutenant(int eBraveLieutenantUnit, int iX, int iY)
{
	if (m_pPlayer)
	{
		m_pPlayer->createBraveLieutenant((UnitTypes)eBraveLieutenantUnit, Coordinates(iX, iY));
	}
}
void CyPlayer::createCapableCaptain(int eCapableCaptainUnit, int iX, int iY)
{
	if (m_pPlayer)
	{
		m_pPlayer->createCapableCaptain((UnitTypes)eCapableCaptainUnit, Coordinates(iX, iY));
	}
}
// WTP, ray, Lieutenants and Captains - END

int CyPlayer::getGreatGeneralRateModifier()
{
	return m_pPlayer ? m_pPlayer->getGreatGeneralRateModifier() : -1;
}
int CyPlayer::getDomesticGreatGeneralRateModifier()
{
	return m_pPlayer ? m_pPlayer->getDomesticGreatGeneralRateModifier() : -1;
}
int CyPlayer::getFreeExperience()
{
	return m_pPlayer ? m_pPlayer->getFreeExperience() : -1;
}
int CyPlayer::getWorkerSpeedModifier()
{
	return m_pPlayer ? m_pPlayer->getWorkerSpeedModifier() : -1;
}
int CyPlayer::getImprovementUpgradeRateModifier()
{
	return m_pPlayer ? m_pPlayer->getImprovementUpgradeRateModifier() : -1;
}
int CyPlayer::getMilitaryProductionModifier()
{
	return m_pPlayer ? m_pPlayer->getMilitaryProductionModifier() : -1;
}
int CyPlayer::getCityDefenseModifier()
{
	return m_pPlayer ? m_pPlayer->getCityDefenseModifier() : -1;
}
int CyPlayer::getHighestUnitLevel()
{
	return m_pPlayer ? m_pPlayer->getHighestUnitLevel() : -1;
}
bool CyPlayer::getExpInBorderModifier()
{
	return m_pPlayer ? m_pPlayer->getExpInBorderModifier() : false;
}
int CyPlayer::getLevelExperienceModifier() const
{
	return m_pPlayer ? m_pPlayer->getLevelExperienceModifier() : -1;
}
CyCity* CyPlayer::getCapitalCity()
{
	return m_pPlayer ? new CyCity(m_pPlayer->getCapitalCity()) : NULL;
}
int CyPlayer::getCitiesLost()
{
	return m_pPlayer ? m_pPlayer->getCitiesLost() : -1;
}
int CyPlayer::getAssets()
{
	return m_pPlayer ? m_pPlayer->getAssets() : -1;
}
void CyPlayer::changeAssets(int iChange)
{
	if (m_pPlayer)
	{
		OOS_LOG("python change assets", iChange);
		m_pPlayer->changeAssets(iChange);
	}
}
int CyPlayer::getPower()
{
	return m_pPlayer ? m_pPlayer->getPower() : -1;
}
int CyPlayer::getPopScore()
{
	return m_pPlayer ? m_pPlayer->getPopScore() : -1;
}
int CyPlayer::getLandScore()
{
	return m_pPlayer ? m_pPlayer->getLandScore() : -1;
}
int CyPlayer::getFatherScore()
{
	return m_pPlayer ? m_pPlayer->getFatherScore() : -1;
}
int CyPlayer::getTotalTimePlayed()
{
	return m_pPlayer ? m_pPlayer->getTotalTimePlayed() : -1;
}
bool CyPlayer::isAlive()
{
	return m_pPlayer ? m_pPlayer->isAlive() : false;
}
bool CyPlayer::isEverAlive()
{
	return m_pPlayer ? m_pPlayer->isEverAlive() : false;
}
bool CyPlayer::isExtendedGame()
{
	return m_pPlayer ? m_pPlayer->isExtendedGame() : false;
}
bool CyPlayer::isFoundedFirstCity()
{
	return m_pPlayer ? m_pPlayer->isFoundedFirstCity() : false;
}
int CyPlayer::getID() const
{
	return m_pPlayer ? m_pPlayer->getID() : NO_PLAYER;
}
WidgetTypes CyPlayer::getWikiWidget() const
{
	return WIDGET_MISSION_CHAR;
}
int CyPlayer::getChar() const
{
	return m_pPlayer ? GC.getCivilizationInfo(GET_PLAYER(m_pPlayer->getID()).getCivilizationType()).getMissionaryChar() : -1;
}
int /* HandicapTypes */ CyPlayer::getHandicapType()
{
	return m_pPlayer ? (int) m_pPlayer->getHandicapType() : -1;
}
int /* CivilizationTypes */  CyPlayer::getCivilizationType()
{
	return m_pPlayer ? (int) m_pPlayer->getCivilizationType() : NO_CIVILIZATION;
}
int /*LeaderHeadTypes*/ CyPlayer::getLeaderType()
{
	return m_pPlayer ? (int) m_pPlayer->getLeaderType() : -1;
}
int /*LeaderHeadTypes*/ CyPlayer::getPersonalityType()
{
	return m_pPlayer ? (int) m_pPlayer->getPersonalityType() : -1;
}
void CyPlayer::setPersonalityType(int /*LeaderHeadTypes*/ eNewValue)
{
	if (m_pPlayer)
		m_pPlayer->setPersonalityType((LeaderHeadTypes) eNewValue);
}
int /*ErasTypes*/ CyPlayer::getCurrentEra()
{
	return m_pPlayer ? (int) m_pPlayer->getCurrentEra() : NO_ERA;
}
void CyPlayer::setCurrentEra(int /*EraTypes*/ iNewValue)
{
	if (m_pPlayer)
		m_pPlayer->setCurrentEra((EraTypes) iNewValue);
}
int /*PlayerTypes*/ CyPlayer::getParent()
{
	return m_pPlayer ? m_pPlayer->getParent() : -1;
}

int CyPlayer::getTeam()
{
	return m_pPlayer ? m_pPlayer->getTeam() : -1;
}
int /*PlayerColorTypes*/ CyPlayer::getPlayerColor()
{
	return m_pPlayer ? (int) m_pPlayer->getPlayerColor() : NO_COLOR;
}
int CyPlayer::getPlayerTextColorR()
{
	return m_pPlayer ? m_pPlayer->getPlayerTextColorR() : -1;
}
int CyPlayer::getPlayerTextColorG()
{
	return m_pPlayer ? m_pPlayer->getPlayerTextColorG() : -1;
}
int CyPlayer::getPlayerTextColorB()
{
	return m_pPlayer ? m_pPlayer->getPlayerTextColorB() : -1;
}
int CyPlayer::getPlayerTextColorA()
{
	return m_pPlayer ? m_pPlayer->getPlayerTextColorA() : -1;
}
int CyPlayer::getSeaPlotYield(YieldTypes eIndex)
{
	return m_pPlayer ? (int) m_pPlayer->getSeaPlotYield(eIndex) : 0;
}
// R&R, Robert Surcouf, No More Variables Hidden game option START
int CyPlayer::getYieldTradedTotal(YieldTypes eIndex)
{
	return m_pPlayer ? m_pPlayer->getYieldTradedTotal(eIndex) : -1;
}
int CyPlayer::getYieldTradedTotalINT(int /*YieldTypes*/ eIndex)
{
	return m_pPlayer ? m_pPlayer->getYieldTradedTotal((YieldTypes)eIndex) : -1;
}
// R&R, Robert Surcouf, No More Variables Hidden game option END

// WTP, ray, Yields Traded Total for Africa and Port Royal - START
int CyPlayer::getYieldTradedTotalAfrica(YieldTypes eIndex)
{
	return m_pPlayer ? m_pPlayer->getYieldTradedTotalAfrica(eIndex) : -1;
}
int CyPlayer::getYieldTradedTotalINTAfrica(int /*YieldTypes*/ eIndex)
{
	return m_pPlayer ? m_pPlayer->getYieldTradedTotalAfrica((YieldTypes)eIndex) : -1;
}

int CyPlayer::getYieldTradedTotalPortRoyal(YieldTypes eIndex)
{
	return m_pPlayer ? m_pPlayer->getYieldTradedTotalPortRoyal(eIndex) : -1;
}
int CyPlayer::getYieldTradedTotalINTPortRoyal(int /*YieldTypes*/ eIndex)
{
	return m_pPlayer ? m_pPlayer->getYieldTradedTotalPortRoyal((YieldTypes)eIndex) : -1;
}
// WTP, ray, Yields Traded Total for Africa and Port Royal - END

// R&R, vetiarvind, Price dependent tax rate change - START
int CyPlayer::getYieldScoreTotalINT(int /*YieldTypes*/ eIndex)
{
	return m_pPlayer ? m_pPlayer->getYieldScoreTotal((YieldTypes)eIndex) : -1;
}
// R&R, vetiarvind, Price dependent tax rate change - END

int CyPlayer::getYieldRate(YieldTypes eIndex)
{
	return m_pPlayer ? m_pPlayer->getYieldRate(eIndex) : -1;
}
// WTP, ray, Happiness - START
int CyPlayer::getHappinessRate()
{
	return m_pPlayer ? m_pPlayer->getHappinessRate() : -1;
}
int CyPlayer::getUnHappinessRate()
{
	return m_pPlayer ? m_pPlayer->getUnHappinessRate() : -1;
}
// WTP, ray, Happiness - END

// WTP, ray, Crime and Law - START
int CyPlayer::getLawRate()
{
	return m_pPlayer ? m_pPlayer->getLawRate() : -1;
}
int CyPlayer::getCrimeRate()
{
	return m_pPlayer ? m_pPlayer->getCrimeRate() : -1;
}
// WTP, ray, Crime and Law - END

int CyPlayer::getYieldRateModifier(YieldTypes eIndex)
{
	return m_pPlayer ? m_pPlayer->getYieldRateModifier(eIndex) : 0;
}
int CyPlayer::getCapitalYieldRateModifier(YieldTypes eIndex)
{
	return m_pPlayer ? m_pPlayer->getCapitalYieldRateModifier(eIndex) : 0;
}
int CyPlayer::getExtraYieldThreshold(YieldTypes eIndex)
{
	return m_pPlayer ? m_pPlayer->getExtraYieldThreshold(eIndex) : 0;
}
bool CyPlayer::isYieldEuropeTradable(int /*YieldTypes*/ eIndex)
{
	return m_pPlayer ? m_pPlayer->isYieldEuropeTradable((YieldTypes)eIndex) : false;
}
void CyPlayer::setYieldEuropeTradable(int /*YieldTypes*/ eIndex, bool bTradeable)
{
	if (m_pPlayer)
		m_pPlayer->setYieldEuropeTradable((YieldTypes)eIndex, bTradeable);
}
bool CyPlayer::isFeatAccomplished(int /*FeatTypes*/ eIndex)
{
	return m_pPlayer ? m_pPlayer->isFeatAccomplished((FeatTypes)eIndex) : false;
}
void CyPlayer::setFeatAccomplished(int /*FeatTypes*/ eIndex, bool bNewValue)
{
	if (m_pPlayer)
		m_pPlayer->setFeatAccomplished((FeatTypes)eIndex, bNewValue);
}
bool CyPlayer::shouldDisplayFeatPopup(int /*FeatTypes*/ eIndex)
{
	return m_pPlayer ? m_pPlayer->shouldDisplayFeatPopup((FeatTypes)eIndex) : false;
}
bool CyPlayer::isOption(int /*PlayerOptionTypes*/ eIndex)
{
	return m_pPlayer ? m_pPlayer->isOption((PlayerOptionTypes)eIndex) : false;
}
void CyPlayer::setOption(int /*PlayerOptionTypes*/ eIndex, bool bNewValue)
{
	if (m_pPlayer)
		m_pPlayer->setOption((PlayerOptionTypes)eIndex, bNewValue);
}
bool CyPlayer::isPlayable()
{
	return m_pPlayer ? m_pPlayer->isPlayable() : false;
}
void CyPlayer::setPlayable(bool bNewValue)
{
	if (m_pPlayer)
		m_pPlayer->setPlayable(bNewValue);
}
int CyPlayer::getImprovementCount(int /*ImprovementTypes*/ iIndex)
{
	return m_pPlayer ? m_pPlayer->getImprovementCount((ImprovementTypes)iIndex) : -1;
}
bool CyPlayer::isBuildingFree(int /*BuildingTypes*/ iIndex)
{
	return m_pPlayer ? m_pPlayer->isBuildingFree((BuildingTypes)iIndex) : false;
}
int CyPlayer::getUnitClassCount(int /*UnitClassTypes*/ eIndex)
{
	return m_pPlayer ? m_pPlayer->getUnitClassCount((UnitClassTypes) eIndex) : NO_UNITCLASS;
}
int CyPlayer::getUnitClassMaking(int /*UnitClassTypes*/ eIndex)
{
	return m_pPlayer ? m_pPlayer->getUnitClassMaking((UnitClassTypes) eIndex) : -1;
}
int CyPlayer::getUnitClassCountPlusMaking(int /*UnitClassTypes*/ eIndex)
{
	return m_pPlayer ? m_pPlayer->getUnitClassCountPlusMaking((UnitClassTypes) eIndex) : -1;
}
int CyPlayer::getBuildingClassCount(int /*BuildingClassTypes*/ iIndex)
{
	return m_pPlayer ? m_pPlayer->getBuildingClassCount((BuildingClassTypes)iIndex) : -1;
}
int CyPlayer::getBuildingClassMaking(int /*BuildingClassTypes*/ iIndex)
{
	return m_pPlayer ? m_pPlayer->getBuildingClassMaking((BuildingClassTypes)iIndex) : -1;
}
int CyPlayer::getBuildingClassCountPlusMaking(int /*BuildingClassTypes*/ iIndex)
{
	return m_pPlayer ? m_pPlayer->getBuildingClassCountPlusMaking((BuildingClassTypes)iIndex) : -1;
}
int CyPlayer::getHurryCount(int /*HurryTypes*/ eIndex)
{
	return m_pPlayer ? m_pPlayer->getHurryCount((HurryTypes)eIndex) : (int) NO_HURRY;
}
bool CyPlayer::canHurry(int /*HurryTypes*/ eIndex)
{
	return m_pPlayer ? m_pPlayer->canHurry((HurryTypes)eIndex, -1) : (int) NO_HURRY;
}
int CyPlayer::getSpecialBuildingNotRequiredCount(int /*SpecialBuildingTypes*/ eIndex)
{
	return m_pPlayer ? m_pPlayer->getSpecialBuildingNotRequiredCount((SpecialBuildingTypes)eIndex) : -1;
}
bool CyPlayer::isSpecialBuildingNotRequired(int /*SpecialBuildingTypes*/ eIndex)
{
	return m_pPlayer ? m_pPlayer->isSpecialBuildingNotRequired((SpecialBuildingTypes)eIndex) : -1;
}
int CyPlayer::getBuildingYieldChange(int /*BuildingClassTypes*/ eBuildingClass, int /*YieldTypes*/ eYield)
{
	return m_pPlayer ? m_pPlayer->getBuildingYieldChange((BuildingClassTypes) eBuildingClass, (YieldTypes) eYield) : -1;
}
int /* CivicTypes */ CyPlayer::getCivic(int /*CivicOptionTypes*/ iIndex)
{
	return m_pPlayer ? m_pPlayer->getCivic((CivicOptionTypes)iIndex) : -1;
}
void CyPlayer::setCivic(int /*CivicOptionTypes*/ eIndex, int /*CivicTypes*/ eNewValue)
{
	if (m_pPlayer)
		m_pPlayer->setCivic((CivicOptionTypes) eIndex, (CivicTypes) eNewValue);
}
int CyPlayer::getCombatExperience() const
{
	if (m_pPlayer)
	{
		return m_pPlayer->getCombatExperience();
	}
	return -1;
}
void CyPlayer::changeCombatExperience(int iChange)
{
	if (m_pPlayer)
	{
		m_pPlayer->changeCombatExperience(iChange);
	}
}
void CyPlayer::setCombatExperience(int iExperience)
{
	if (m_pPlayer)
	{
		m_pPlayer->setCombatExperience(iExperience);
	}
}

// R&R, ray, Great Admirals - START
int CyPlayer::getSeaCombatExperience() const
{
	if (m_pPlayer)
	{
		return m_pPlayer->getSeaCombatExperience();
	}
	return -1;
}
void CyPlayer::changeSeaCombatExperience(int iChange)
{
	if (m_pPlayer)
	{
		m_pPlayer->changeSeaCombatExperience(iChange);
	}
}
void CyPlayer::setSeaCombatExperience(int iExperience)
{
	if (m_pPlayer)
	{
		m_pPlayer->setSeaCombatExperience(iExperience);
	}
}
// R&R, ray, Great Admirals - END

void CyPlayer::addCityName(std::wstring szName)
{
	if (m_pPlayer)
		m_pPlayer->addCityName(szName);
}
int CyPlayer::getNumCityNames()
{
	return m_pPlayer ? m_pPlayer->getNumCityNames() : -1;
}
std::wstring CyPlayer::getCityName(int iIndex)
{
	if (m_pPlayer == NULL) return L"";
	return m_pPlayer->getCityName(iIndex);
}
// returns tuple of (CyCity, iterOut)
python::tuple CyPlayer::firstCity(bool bRev)
{
	int iterIn = 0;
	CvCity* pvObj = m_pPlayer ? m_pPlayer->firstCity(&iterIn, bRev) : NULL;
	CyCity* pyObj = pvObj ? new CyCity(pvObj) : NULL;
	python::tuple tup=python::make_tuple(pyObj, iterIn);
	delete pyObj;
	return tup;
}
// returns tuple of (CyCity, iterOut)
python::tuple CyPlayer::nextCity(int iterIn, bool bRev)
{
	CvCity* pvObj = m_pPlayer ? m_pPlayer->nextCity(&iterIn, bRev) : NULL;
	CyCity* pyObj = pvObj ? new CyCity(pvObj) : NULL;
	python::tuple tup=python::make_tuple(pyObj, iterIn);
	delete pyObj;
	return tup;
}
int CyPlayer::getNumCities()
{
	return m_pPlayer ? m_pPlayer->getNumCities() : -1;
}
CyCity* CyPlayer::getCity(int iID)
{
	return m_pPlayer ? new CyCity(m_pPlayer->getCity(iID)) : NULL;
}
// returns tuple of (CyUnit, iterOut)
python::tuple CyPlayer::firstUnit()
{
	int iterIn = 0;
	CvUnit* pvUnit = m_pPlayer ? m_pPlayer->firstUnit(&iterIn) : NULL;
	CyUnit* pyUnit = pvUnit ? new CyUnit(pvUnit) : NULL;
	python::tuple tup=python::make_tuple(pyUnit, iterIn);
	delete pyUnit;
	return tup;
}
// returns tuple of (CyUnit, iterOut)
python::tuple CyPlayer::nextUnit(int iterIn)
{
	CvUnit* pvObj = m_pPlayer ? m_pPlayer->nextUnit(&iterIn) : NULL;
	CyUnit* pyObj = pvObj ? new CyUnit(pvObj) : NULL;
	python::tuple tup=python::make_tuple(pyObj, iterIn);
	delete pyObj;
	return tup;
}
int CyPlayer::getNumUnits()
{
	return m_pPlayer ? m_pPlayer->getNumUnits() : -1;
}

// WTP, ray, easily counting Ships - START
int CyPlayer::getNumShips()
{
	return m_pPlayer ? m_pPlayer->getNumShips() : -1;
}
// WTP, ray, easily counting Ships - END

CyUnit* CyPlayer::getUnit(int iID)
{
	return m_pPlayer ? new CyUnit(m_pPlayer->getUnit(iID)) : NULL;
}
int CyPlayer::getNumEuropeUnits()
{
	return m_pPlayer ? m_pPlayer->getNumEuropeUnits() : -1;
}
CyUnit* CyPlayer::getEuropeUnit(int iIndex)
{
	return m_pPlayer ? new CyUnit(m_pPlayer->getEuropeUnit(iIndex)) : NULL;
}
CyUnit* CyPlayer::getEuropeUnitById(int iId)
{
	return m_pPlayer ? new CyUnit(m_pPlayer->getEuropeUnitById(iId)) : NULL;
}
void CyPlayer::loadUnitFromEurope(CyUnit* pUnit, CyUnit* pTransport)
{
	if (m_pPlayer)
	{
		m_pPlayer->loadUnitFromEurope(pUnit->getUnit(), pTransport->getUnit());
	}
}
void CyPlayer::unloadUnitToEurope(CyUnit* pUnit)
{
	if (m_pPlayer)
	{
		m_pPlayer->unloadUnitToEurope(pUnit->getUnit());
	}
}

// returns tuple of (CySelectionGroup, iterOut)
python::tuple CyPlayer::firstSelectionGroup(bool bRev)
{
	int iterIn = 0;
	CvSelectionGroup* pvObj = m_pPlayer ? m_pPlayer->firstSelectionGroup(&iterIn, bRev) : NULL;
	CySelectionGroup* pyObj = pvObj ? new CySelectionGroup(pvObj) : NULL;
	python::tuple tup=python::make_tuple(pyObj, iterIn);
	delete pyObj;
	return tup;
}
// returns tuple of (CySelectionGroup, iterOut)
python::tuple CyPlayer::nextSelectionGroup(int iterIn, bool bRev)
{
	CvSelectionGroup* pvObj = m_pPlayer ? m_pPlayer->nextSelectionGroup(&iterIn, bRev) : NULL;
	CySelectionGroup* pyObj = pvObj ? new CySelectionGroup(pvObj) : NULL;
	python::tuple tup=python::make_tuple(pyObj, iterIn);
	delete pyObj;
	return tup;
}
int CyPlayer::getNumSelectionGroups()
{
	return m_pPlayer ? m_pPlayer->getNumSelectionGroups() : -1;
}
CySelectionGroup* CyPlayer::getSelectionGroup(int iID)
{
	return m_pPlayer ? new CySelectionGroup(m_pPlayer->getSelectionGroup(iID)) : NULL;
}
int CyPlayer::countNumTravelUnits(int /*UnitTravelStates*/ eState, int /*DomainTypes*/ eDomain)
{
	return m_pPlayer ? m_pPlayer->countNumTravelUnits((UnitTravelStates) eState, (DomainTypes) eDomain) : -1;
}

void CyPlayer::trigger(/*EventTriggerTypes*/int eEventTrigger)
{
	if (m_pPlayer)
	{
		m_pPlayer->trigger((EventTriggerTypes)eEventTrigger);
	}
}
const EventTriggeredData* CyPlayer::getEventOccured(int /*EventTypes*/ eEvent) const
{
	return m_pPlayer ? m_pPlayer->getEventOccured((EventTypes)eEvent) : NULL;
}
void CyPlayer::resetEventOccured(int /*EventTypes*/ eEvent)
{
	if (m_pPlayer)
	{
		m_pPlayer->resetEventOccured((EventTypes)eEvent);
	}
}
EventTriggeredData* CyPlayer::getEventTriggered(int iID) const
{
	return m_pPlayer ? m_pPlayer->getEventTriggered(iID) : NULL;
}
EventTriggeredData* CyPlayer::initTriggeredData(int /*EventTriggerTypes*/ eEventTrigger, bool bFire, int iCityId, int iPlotX, int iPlotY, int /*PlayerTypes*/ eOtherPlayer, int iOtherPlayerCityId, int iUnitId, int /*BuildingTypes*/ eBuilding)
{
	return m_pPlayer ? m_pPlayer->initTriggeredData((EventTriggerTypes)eEventTrigger, bFire, iCityId, iPlotX, iPlotY, (PlayerTypes)eOtherPlayer, iOtherPlayerCityId, iUnitId, (BuildingTypes)eBuilding) : NULL;
}
int CyPlayer::getEventTriggerWeight(int /*EventTriggerTypes*/ eTrigger)
{
	return m_pPlayer ? m_pPlayer->getEventTriggerWeight((EventTriggerTypes)eTrigger) : NULL;
}
void CyPlayer::AI_updateFoundValues(bool bStartingLoc)
{
	if (m_pPlayer)
		m_pPlayer->AI_updateFoundValues(bStartingLoc);
}
int CyPlayer::AI_foundValue(int iX, int iY, int iMinUnitRange/* = -1*/, bool bStartingLoc/* = false*/)
{
	return m_pPlayer ? m_pPlayer->AI_foundValue(iX, iY, iMinUnitRange, bStartingLoc) : -1;
}
bool CyPlayer::AI_demandRebukedWar(int /*PlayerTypes*/ ePlayer)
{
	return m_pPlayer ? m_pPlayer->AI_demandRebukedWar((PlayerTypes)ePlayer) : false;
}
AttitudeTypes CyPlayer::AI_getAttitude(int /*PlayerTypes*/ ePlayer)
{
	return m_pPlayer ? m_pPlayer->AI_getAttitude((PlayerTypes)ePlayer) : NO_ATTITUDE;
}
// R&R, Robert Surcouf, No More Variables Hidden game option START
int CyPlayer::AI_getAttitudeVal(int /*PlayerTypes*/ ePlayer)
{
	return m_pPlayer ? m_pPlayer->AI_getAttitudeValue((PlayerTypes)ePlayer) : 0;
}
// R&R, Robert Surcouf, No More Variables Hidden game option END
int CyPlayer::AI_unitValue(int /*UnitTypes*/ eUnit, int /*UnitAITypes*/ eUnitAI, CyArea* pArea)
{
	return m_pPlayer ? m_pPlayer->AI_unitValue((UnitTypes)eUnit, (UnitAITypes)eUnitAI, pArea->getArea()) : -1;
}
int CyPlayer::AI_civicValue(int /*CivicTypes*/ eCivic)
{
	return m_pPlayer ? m_pPlayer->AI_civicValue((CivicTypes)eCivic) : -1;
}
int CyPlayer::AI_totalUnitAIs(int /*UnitAITypes*/ eUnitAI)
{
	return m_pPlayer ? m_pPlayer->AI_totalUnitAIs((UnitAITypes)eUnitAI) : -1;
}
int CyPlayer::AI_totalAreaUnitAIs(CyArea* pArea, int /*UnitAITypes*/ eUnitAI)
{
	return m_pPlayer ? m_pPlayer->AI_totalAreaUnitAIs(pArea->getArea(), (UnitAITypes)eUnitAI) : -1;
}
int CyPlayer::AI_totalWaterAreaUnitAIs(CyArea* pArea, int /*UnitAITypes*/ eUnitAI)
{
	return m_pPlayer ? m_pPlayer->AI_totalWaterAreaUnitAIs(pArea->getArea(), (UnitAITypes)eUnitAI) : -1;
}
int CyPlayer::AI_getNumAIUnits(int /*UnitAITypes*/ eIndex)
{
	return m_pPlayer ? m_pPlayer->AI_getNumAIUnits((UnitAITypes)eIndex) : NO_UNITAI;
}
int CyPlayer::AI_getAttitudeExtra(int /*PlayerTypes*/ eIndex)
{
	return m_pPlayer ? m_pPlayer->AI_getAttitudeExtra((PlayerTypes)eIndex) : -1;
}
void CyPlayer::AI_setAttitudeExtra(int /*PlayerTypes*/ eIndex, int iNewValue)
{
	if (m_pPlayer)
		m_pPlayer->AI_setAttitudeExtra((PlayerTypes)eIndex, iNewValue);
}
void CyPlayer::AI_changeAttitudeExtra(int /*PlayerTypes*/ eIndex, int iChange)
{
	if (m_pPlayer)
		m_pPlayer->AI_changeAttitudeExtra((PlayerTypes)eIndex, iChange);
}
int CyPlayer::AI_getMemoryCount(int /*PlayerTypes*/ eIndex1, int /*MemoryTypes*/ eIndex2)
{
	return m_pPlayer ? m_pPlayer->AI_getMemoryCount((PlayerTypes)eIndex1, (MemoryTypes)eIndex2) : -1;
}
void CyPlayer::AI_changeMemoryCount(int /*PlayerTypes*/ eIndex1, int /*MemoryTypes*/ eIndex2, int iChange)
{
	if (m_pPlayer)
		m_pPlayer->AI_changeMemoryCount((PlayerTypes)eIndex1, (MemoryTypes)eIndex2, iChange);
}
int CyPlayer::AI_getExtraGoldTarget() const
{
	return m_pPlayer ? m_pPlayer->AI_getExtraGoldTarget() : -1;
}
void CyPlayer::AI_setExtraGoldTarget(int iNewValue)
{
	if (m_pPlayer)
	{
		m_pPlayer->AI_setExtraGoldTarget(iNewValue);
	}
}

int CyPlayer::getScoreHistory(int iTurn) const
{
	return (NULL != m_pPlayer ? m_pPlayer->getScoreHistory(iTurn) : 0);
}
int CyPlayer::getEconomyHistory(int iTurn) const
{
	return (NULL != m_pPlayer ? m_pPlayer->getEconomyHistory(iTurn) : 0);
}
int CyPlayer::getIndustryHistory(int iTurn) const
{
	return (NULL != m_pPlayer ? m_pPlayer->getIndustryHistory(iTurn) : 0);
}
int CyPlayer::getAgricultureHistory(int iTurn) const
{
	return (NULL != m_pPlayer ? m_pPlayer->getAgricultureHistory(iTurn) : 0);
}
int CyPlayer::getPowerHistory(int iTurn) const
{
	return (NULL != m_pPlayer ? m_pPlayer->getPowerHistory(iTurn) : 0);
}
int CyPlayer::getCultureHistory(int iTurn) const
{
	return (NULL != m_pPlayer ? m_pPlayer->getCultureHistory(iTurn) : 0);
}

int CyPlayer::addTradeRoute(int iSourceCityOwner, int iSourceCityId, int iDestinationCityOwner, int iDestinationCityId, int /*YieldTypes*/ eYield)
{
	return m_pPlayer ? m_pPlayer->addTradeRoute(IDInfo((PlayerTypes)iSourceCityOwner, iSourceCityId), IDInfo((PlayerTypes)iDestinationCityOwner, iDestinationCityId), (YieldTypes) eYield) : -1;
}

bool CyPlayer::removeTradeRoute(int iId)
{
	return m_pPlayer ? m_pPlayer->removeTradeRoute(iId) : false;
}

CyTradeRoute* CyPlayer::getTradeRoute(int iId) const
{
	return new CyTradeRoute(m_pPlayer ? m_pPlayer->getTradeRoute(iId) : NULL);
}

int CyPlayer::getNumTradeRoutes() const
{
	return m_pPlayer ? m_pPlayer->getNumTradeRoutes() : -1;
}

CyTradeRoute* CyPlayer::getTradeRouteByIndex(int iIndex) const
{
	return new CyTradeRoute(m_pPlayer ? m_pPlayer->getTradeRouteByIndex(iIndex) : NULL);
}

bool CyPlayer::editTradeRoute(int iId, int iSourceCityOwner, int iSourceCityId, int iDestinationCityOwner, int iDestinationCityId, int /*YieldTypes*/ eYield)
{
	return m_pPlayer ? m_pPlayer->editTradeRoute(iId, IDInfo((PlayerTypes)iSourceCityOwner, iSourceCityId), IDInfo((PlayerTypes)iDestinationCityOwner, iDestinationCityId), (YieldTypes) eYield) : false;
}

bool CyPlayer::canLoadYield(int /*PlayerTypes*/ eCityPlayer) const
{
	return m_pPlayer ? m_pPlayer->canLoadYield((PlayerTypes) eCityPlayer) : false;
}

bool CyPlayer::canUnloadYield(int /*PlayerTypes*/ eCityPlayer) const
{
	return m_pPlayer ? m_pPlayer->canUnloadYield((PlayerTypes) eCityPlayer) : false;
}

int CyPlayer::getYieldEquipmentAmount(int /*ProfessionTypes*/ eProfession, int /*YieldTypes*/ eYield)
{
	return m_pPlayer ? m_pPlayer->getYieldEquipmentAmount((ProfessionTypes) eProfession, (YieldTypes) eYield) : -1;
}

std::string CyPlayer::getScriptData() const
{
	if (m_pPlayer == NULL) return "";
	return m_pPlayer->getScriptData();
}
void CyPlayer::setScriptData(std::string szNewValue)
{
	if (m_pPlayer)
		m_pPlayer->setScriptData(szNewValue);
}
int CyPlayer::AI_maxGoldTrade(int iPlayer)
{
	CvPlayerAI* pPlayer = dynamic_cast<CvPlayerAI*>(m_pPlayer);
	if (pPlayer)
	{
		return (pPlayer->AI_maxGoldTrade((PlayerTypes)iPlayer));
	}
	return 0;
}
void CyPlayer::forcePeace(int iPlayer)
{
	if (m_pPlayer)
		m_pPlayer->forcePeace((PlayerTypes)iPlayer);
}
int CyPlayer::getHighestTradedYield()
{
	return m_pPlayer ? m_pPlayer->getHighestTradedYield() : -1;
}
int CyPlayer::getHighestStoredYieldCityId(int /*YieldTypes*/ eYield)
{
	return m_pPlayer ? m_pPlayer->getHighestStoredYieldCityId((YieldTypes) eYield) : -1;
}
int CyPlayer::getCrossesStored()
{
	return m_pPlayer ? m_pPlayer->getCrossesStored() : -1;
}
int CyPlayer::getBellsStored()
{
	return m_pPlayer ? m_pPlayer->getBellsStored() : -1;
}
// R&R, Robert Surcouf, No More Variables Hidden game option START
int CyPlayer::getMissionaryPoints(int /*PlayerTypes*/ ePlayer) const
{
	return m_pPlayer ? m_pPlayer->getMissionaryPoints((PlayerTypes) ePlayer) : -1;
}
int CyPlayer::missionaryThreshold(int /*PlayerTypes*/ ePlayer) const
{
	return m_pPlayer ? m_pPlayer->missionaryThreshold((PlayerTypes) ePlayer) : -1;
}
int CyPlayer::getMissionaryRateModifier() const
{
	return m_pPlayer ? m_pPlayer->getMissionaryRateModifier() : -1;
}
// R&R, Robert Surcouf, No More Variables Hidden game option END

//WTP, ray Kings Used Ship - START
int CyPlayer::getRandomUsedShipClassTypeID() const
{
	return m_pPlayer ? m_pPlayer->getRandomUsedShipClassTypeID() : -1;
}

int CyPlayer::getUsedShipPrice(int /*UnitClassTypes*/ iUsedShipClassType) const
{
	return m_pPlayer ? m_pPlayer->getUsedShipPrice((UnitClassTypes)iUsedShipClassType) : -1;
}

bool CyPlayer::isKingWillingToTradeUsedShips() const
{
	return m_pPlayer ? m_pPlayer->isKingWillingToTradeUsedShips() : false;
}

void CyPlayer::resetCounterForUsedShipDeals()
{
	if (m_pPlayer)
	{
		m_pPlayer->resetCounterForUsedShipDeals();
	}
}
//WTP, ray Kings Used Ship - END


// WTP, ray, Foreign Kings, buy Immigrants - START
int CyPlayer::getRandomForeignImmigrantClassTypeID(int iKingID) const
{
	return m_pPlayer ? m_pPlayer->getRandomForeignImmigrantClassTypeID(iKingID) : -1;
}

int CyPlayer::getForeignImmigrantPrice(int /*UnitClassTypes*/ iForeignImmigrantClassType, int iEuropeKingID) const
{
	return m_pPlayer ? m_pPlayer->getForeignImmigrantPrice((UnitClassTypes)iForeignImmigrantClassType, iEuropeKingID) : -1;
}

bool CyPlayer::isForeignKingWillingToTradeImmigrants(int iEuropeKingID) const
{
	return m_pPlayer ? m_pPlayer->isForeignKingWillingToTradeImmigrants(iEuropeKingID) : false;
}

void CyPlayer::resetCounterForForeignImmigrantsDeals()
{
	if (m_pPlayer)
	{
		m_pPlayer->resetCounterForForeignImmigrantsDeals();
	}
}
// WTP, ray, Foreign Kings, buy Immigrants - END

// R&R, ray, Church Favours - START
int CyPlayer::getChurchFavourPrice() const
{
	return m_pPlayer ? m_pPlayer->getChurchFavourPrice() : -1;
}
// R&R, ray, Church Favours - END

/** NBMOD TAX **/
int CyPlayer::NBMOD_GetMaxTaxRate() const
{
	return m_pPlayer ? m_pPlayer->NBMOD_GetMaxTaxRate() : -1;
}
/** NBMOD TAX **/

// R&R, ray, Bargaining - Start
bool CyPlayer::tryGetNewBargainPriceSell()
{
	return m_pPlayer ? m_pPlayer->tryGetNewBargainPriceSell() : false;
}

bool CyPlayer::tryGetNewBargainPriceBuy()
{
	return m_pPlayer ? m_pPlayer->tryGetNewBargainPriceBuy() : false;
}
// R&R, ray, Bargaining - End

// TAC - Python Export - Ray - START
void CyPlayer::NBMOD_IncreaseMaxTaxRate()
{
	if (m_pPlayer)
		m_pPlayer->NBMOD_IncreaseMaxTaxRate();
}

void CyPlayer::NBMOD_DecreaseMaxTaxRate()
{
	if (m_pPlayer)
		m_pPlayer->NBMOD_DecreaseMaxTaxRate();
}
// TAC - Python Export - Ray - END


int CyPlayer::getTaxRate()
{
	return m_pPlayer ? m_pPlayer->getTaxRate() : -1;
}
void CyPlayer::changeTaxRate(int iChange)
{
	if (m_pPlayer)
		m_pPlayer->changeTaxRate(iChange);
}
bool CyPlayer::canTradeWithEurope()
{
	return m_pPlayer ? m_pPlayer->canTradeWithEurope() : false;
}
int CyPlayer::getSellToEuropeProfit(int /*YieldTypes*/ eYield, int iAmount)
{
	return m_pPlayer ? m_pPlayer->getSellToEuropeProfit((YieldTypes) eYield, iAmount) : -1;
}
int CyPlayer::getYieldSellPrice(int /*YieldTypes*/ eYield)
{
	return m_pPlayer ? m_pPlayer->getYieldSellPrice((YieldTypes) eYield) : -1;
}
int CyPlayer::getYieldBuyPrice(int /*YieldTypes*/ eYield)
{
	return m_pPlayer ? m_pPlayer->getYieldBuyPrice((YieldTypes) eYield) : -1;
}
void CyPlayer::setYieldBuyPrice(int /*YieldTypes*/ eYield, int iPrice, bool bMessage)
{
	if (m_pPlayer)
		m_pPlayer->setYieldBuyPrice((YieldTypes)eYield, iPrice, bMessage);
}
// R&R, ray, Africa
int CyPlayer::getYieldAfricaSellPrice(int /*YieldTypes*/ eYield)
{
	return m_pPlayer ? m_pPlayer->getYieldAfricaSellPrice((YieldTypes) eYield) : -1;
}
int CyPlayer::getYieldAfricaBuyPrice(int /*YieldTypes*/ eYield)
{
	return m_pPlayer ? m_pPlayer->getYieldAfricaBuyPrice((YieldTypes) eYield) : -1;
}
int CyPlayer::getYieldAfricaBuyPriceNoModifier(int /*YieldTypes*/ eYield)
{
	return m_pPlayer ? m_pPlayer->getYieldAfricaBuyPriceNoModifier((YieldTypes) eYield) : -1;
}

void CyPlayer::setYieldAfricaBuyPrice(int /*YieldTypes*/ eYield, int iPrice, bool bMessage)
{
	if (m_pPlayer)
		m_pPlayer->setYieldAfricaBuyPrice((YieldTypes)eYield, iPrice, bMessage);
}
CyUnit* CyPlayer::buyYieldUnitFromAfrica(int /*YieldTypes*/ eYield, int iAmount, CyUnit* pTransport)
{
	return m_pPlayer ? (new CyUnit(m_pPlayer->buyYieldUnitFromAfrica((YieldTypes) eYield, iAmount, pTransport->getUnit()))) : NULL;
}
// R&R, ray, Africa - END
// R&R, ray, Port Royal
int CyPlayer::getYieldPortRoyalSellPrice(int /*YieldTypes*/ eYield)
{
	return m_pPlayer ? m_pPlayer->getYieldPortRoyalSellPrice((YieldTypes) eYield) : -1;
}
int CyPlayer::getYieldPortRoyalBuyPrice(int /*YieldTypes*/ eYield)
{
	return m_pPlayer ? m_pPlayer->getYieldPortRoyalBuyPrice((YieldTypes) eYield) : -1;
}
int CyPlayer::getYieldPortRoyalBuyPriceNoModifier(int /*YieldTypes*/ eYield)
{
	return m_pPlayer ? m_pPlayer->getYieldPortRoyalBuyPriceNoModifier((YieldTypes) eYield) : -1;
}
void CyPlayer::setYieldPortRoyalBuyPrice(int /*YieldTypes*/ eYield, int iPrice, bool bMessage)
{
	if (m_pPlayer)
		m_pPlayer->setYieldPortRoyalBuyPrice((YieldTypes)eYield, iPrice, bMessage);
}
CyUnit* CyPlayer::buyYieldUnitFromPortRoyal(int /*YieldTypes*/ eYield, int iAmount, CyUnit* pTransport)
{
	return m_pPlayer ? (new CyUnit(m_pPlayer->buyYieldUnitFromPortRoyal((YieldTypes) eYield, iAmount, pTransport->getUnit()))) : NULL;
}
// R&R, ray, Port Royal - END
void CyPlayer::sellYieldUnitToEurope(CyUnit* pUnit, int iAmount, int iCommission)
{
	if (m_pPlayer)
		m_pPlayer->sellYieldUnitToEurope(pUnit->getUnit(), iAmount, iCommission);
}
CyUnit* CyPlayer::buyYieldUnitFromEurope(int /*YieldTypes*/ eYield, int iAmount, CyUnit* pTransport)
{
	return m_pPlayer ? (new CyUnit(m_pPlayer->buyYieldUnitFromEurope((YieldTypes) eYield, iAmount, pTransport->getUnit()))) : NULL;
}
int CyPlayer::getEuropeUnitBuyPrice(int /*UnitTypes*/ eUnit)
{
	return m_pPlayer ? m_pPlayer->getEuropeUnitBuyPrice((UnitTypes) eUnit) : -1;
}
CyUnit* CyPlayer::buyEuropeUnit(int /*UnitTypes*/ eUnit)
{
	return m_pPlayer ? (new CyUnit(m_pPlayer->buyEuropeUnit((UnitTypes) eUnit, 100))) : NULL;
}
int CyPlayer::getYieldBoughtTotal(int /*YieldTypes*/ eYield) const
{
	return m_pPlayer ? m_pPlayer->getYieldBoughtTotal((YieldTypes) eYield) : 0;
}

// WTP, ray, Yields Traded Total for Africa and Port Royal - START
int CyPlayer::getYieldBoughtTotalAfrica(int /*YieldTypes*/ eYield) const
{
	return m_pPlayer ? m_pPlayer->getYieldBoughtTotalAfrica((YieldTypes) eYield) : 0;
}
int CyPlayer::getYieldBoughtTotalPortRoyal(int /*YieldTypes*/ eYield) const
{
	return m_pPlayer ? m_pPlayer->getYieldBoughtTotalPortRoyal((YieldTypes) eYield) : 0;
}
// WTP, ray, Yields Traded Total for Africa and Port Royal - END

int CyPlayer::getNumRevolutionEuropeUnits() const
{
	return m_pPlayer ? m_pPlayer->getNumRevolutionEuropeUnits() : -1;
}
int CyPlayer::getRevolutionEuropeUnit(int iIndex) const
{
	return (int) m_pPlayer ? m_pPlayer->getRevolutionEuropeUnit(iIndex) : -1;
}
int CyPlayer::getRevolutionEuropeProfession(int iIndex) const
{
	return (int) m_pPlayer ? m_pPlayer->getRevolutionEuropeProfession(iIndex) : -1;
}
bool CyPlayer::isEurope() const
{
	return m_pPlayer ? m_pPlayer->isEurope() : false;
}
bool CyPlayer::isInRevolution() const
{
	return m_pPlayer ? m_pPlayer->isInRevolution() : false;
}
int CyPlayer::getDocksNextUnit(int iIndex) const
{
	if (m_pPlayer)
	{
		if (iIndex >= 0 && iIndex < static_cast<int>(m_pPlayer->CivEffect().getNumUnitsOnDock()))
		{
			return m_pPlayer->getDocksNextUnit(iIndex);
		}
	}
	return NO_UNIT;

}
void CyPlayer::addRevolutionEuropeUnit(int /*UnitTypes*/ eUnit, int /*ProfessionTypes*/ eProfession)
{
	if (m_pPlayer)
	{
		m_pPlayer->addRevolutionEuropeUnit((UnitTypes) eUnit, (ProfessionTypes) eProfession);
	}
}
int CyPlayer::getNumTradeMessages() const
{
	return m_pPlayer ? m_pPlayer->getNumTradeMessages() : -1;
}
std::wstring CyPlayer::getTradeMessage(int i) const
{
	if (m_pPlayer == NULL) return L"";
	return m_pPlayer->getTradeMessage(i);
}

// TAC - Trade Messages - koma13 - START
int /*TradeMessageTypes*/ CyPlayer::getTradeMessageType(int i) const
{
	return m_pPlayer ? m_pPlayer->getTradeMessageType(i) : NO_TRADE_MESSAGE;
}
int /*YieldTypes*/ CyPlayer::getTradeMessageYield(int i) const
{
	return m_pPlayer ? m_pPlayer->getTradeMessageYield(i) : NO_YIELD;
}
int CyPlayer::getTradeMessageAmount(int i) const
{
	return m_pPlayer ? m_pPlayer->getTradeMessageAmount(i) : -1;
}
int CyPlayer::getTradeMessageCommission(int i) const
{
	return m_pPlayer ? m_pPlayer->getTradeMessageCommission(i) : -1;
}
// TAC - Trade Messages - koma13 - END

// PatchMod: Achievements START
bool CyPlayer::isAchieveGained(int /*AchieveTypes*/ eAchieve)
{
	return m_pPlayer ? m_pPlayer->isAchieveGained((AchieveTypes) eAchieve) : -1;
}

int CyPlayer::getAchieveYear(int /*AchieveTypes*/ eAchieve)
{
	return m_pPlayer ? m_pPlayer->getAchieveYear((AchieveTypes) eAchieve) : -1;
}
// PatchMod: Achievements END
// TAC - TAC Interface - koma13 - START
void CyPlayer::toggleMultiRowPlotList()
{
	if (m_pPlayer)
		m_pPlayer->toggleMultiRowPlotList();
}
// TAC - TAC Interface - koma13 - END

/*** TRIANGLETRADE 10/15/08 by DPII ***/
int CyPlayer::getNumAfricaUnits()
{
	return m_pPlayer ? m_pPlayer->getNumAfricaUnits() : -1;
}
CyUnit* CyPlayer::getAfricaUnit(int iIndex)
{
	return m_pPlayer ? new CyUnit(m_pPlayer->getAfricaUnit(iIndex)) : NULL;
}
CyUnit* CyPlayer::getAfricaUnitById(int iId)
{
	return m_pPlayer ? new CyUnit(m_pPlayer->getAfricaUnitById(iId)) : NULL;
}
void CyPlayer::loadUnitFromAfrica(CyUnit* pUnit, CyUnit* pTransport)
{
	if (m_pPlayer)
	{
		m_pPlayer->loadUnitFromAfrica(pUnit->getUnit(), pTransport->getUnit());
	}
}
void CyPlayer::unloadUnitToAfrica(CyUnit* pUnit)
{
	if (m_pPlayer)
	{
		m_pPlayer->unloadUnitToAfrica(pUnit->getUnit());
	}
}
int CyPlayer::getAfricaUnitBuyPrice(int /*UnitTypes*/ eUnit)
{
	return m_pPlayer ? m_pPlayer->getAfricaUnitBuyPrice((UnitTypes) eUnit) : -1;
}
CyUnit* CyPlayer::buyAfricaUnit(int /*UnitTypes*/ eUnit)
{
	return m_pPlayer ? (new CyUnit(m_pPlayer->buyAfricaUnit((UnitTypes) eUnit, 100))) : NULL;
}
bool CyPlayer::canTradeWithAfrica()
{
	return m_pPlayer ? m_pPlayer->canTradeWithAfrica() : false;
}
int CyPlayer::getSellToAfricaProfit(int /*YieldTypes*/ eYield, int iAmount)
{
	return m_pPlayer ? m_pPlayer->getSellToAfricaProfit((YieldTypes) eYield, iAmount) : -1;
}
bool CyPlayer::isYieldAfricaTradable(int /*YieldTypes*/ eIndex)
{
	return m_pPlayer ? m_pPlayer->isYieldAfricaTradable((YieldTypes)eIndex) : false;
}

//WTP, ray, Colonial Intervention In Native War - START
int CyPlayer::getIDSecondPlayerFrenchNativeWar()
{
	return m_pPlayer ? m_pPlayer->getIDSecondPlayerFrenchNativeWar() : -1;
}
//WTP, ray, Colonial Intervention In Native War - END

/**************************************/

// R&R, ray, Port Royal
int CyPlayer::getNumPortRoyalUnits()
{
	return m_pPlayer ? m_pPlayer->getNumPortRoyalUnits() : -1;
}
CyUnit* CyPlayer::getPortRoyalUnit(int iIndex)
{
	return m_pPlayer ? new CyUnit(m_pPlayer->getPortRoyalUnit(iIndex)) : NULL;
}
CyUnit* CyPlayer::getPortRoyalUnitById(int iId)
{
	return m_pPlayer ? new CyUnit(m_pPlayer->getPortRoyalUnitById(iId)) : NULL;
}
void CyPlayer::loadUnitFromPortRoyal(CyUnit* pUnit, CyUnit* pTransport)
{
	if (m_pPlayer)
	{
		m_pPlayer->loadUnitFromPortRoyal(pUnit->getUnit(), pTransport->getUnit());
	}
}
void CyPlayer::unloadUnitToPortRoyal(CyUnit* pUnit)
{
	if (m_pPlayer)
	{
		m_pPlayer->unloadUnitToPortRoyal(pUnit->getUnit());
	}
}
int CyPlayer::getPortRoyalUnitBuyPrice(int /*UnitTypes*/ eUnit)
{
	return m_pPlayer ? m_pPlayer->getPortRoyalUnitBuyPrice((UnitTypes) eUnit) : -1;
}
CyUnit* CyPlayer::buyPortRoyalUnit(int /*UnitTypes*/ eUnit)
{
	return m_pPlayer ? (new CyUnit(m_pPlayer->buyPortRoyalUnit((UnitTypes) eUnit, 100))) : NULL;
}
bool CyPlayer::canTradeWithPortRoyal()
{
	return m_pPlayer ? m_pPlayer->canTradeWithPortRoyal() : false;
}
int CyPlayer::getSellToPortRoyalProfit(int /*YieldTypes*/ eYield, int iAmount)
{
	return m_pPlayer ? m_pPlayer->getSellToPortRoyalProfit((YieldTypes) eYield, iAmount) : -1;
}
bool CyPlayer::isYieldPortRoyalTradable(int /*YieldTypes*/ eIndex)
{
	return m_pPlayer ? m_pPlayer->isYieldPortRoyalTradable((YieldTypes)eIndex) : false;
}
/**************************************/
// R&R mod, vetiarvind, trade groups - start
int CyPlayer::getLoadedTradeGroup()
{
	return m_loadedTradeGroup;
}

void CyPlayer::setLoadedTradeGroup(int iId)
{
	m_loadedTradeGroup = iId;
}

int CyPlayer::getLastUpdatedTradeGroup()
{
	return m_lastUpdatedTradegroup;
}

int CyPlayer::addTradeRouteGroup(const std::wstring groupName)
{
	return m_pPlayer ? m_lastUpdatedTradegroup = m_pPlayer->addTradeRouteGroup(groupName) : -1;
}
bool CyPlayer::editTradeRouteGroup(int iId, const std::wstring groupName)
{
	return m_pPlayer ? m_pPlayer->editTradeRouteGroup(iId, groupName) : false;
}
bool CyPlayer::removeTradeRouteGroup(int iId)
{
	return m_pPlayer ? m_pPlayer->removeTradeRouteGroup(iId) : false;
}

int CyPlayer::getNumTradeGroups() const
{
	return m_pPlayer ? m_pPlayer->getNumTradeGroups() : 0;
}
CyTradeRouteGroup* CyPlayer::getTradeGroup(int iIndex)
{
	return m_pPlayer ? new CyTradeRouteGroup(m_pPlayer->getTradeRouteGroup(iIndex)) : NULL;
}

// R&R mod, vetiarvind, trade groups - end

CyInfoArray* CyPlayer::getSpecialBuildingTypes() const
{
	// Currently a bit pointless, but here there is a single location to alter all of the python code using this should we need to update.

	EnumMap<SpecialBuildingTypes, bool, true> em;

	return new CyInfoArray(em);
}

CyInfoArray* CyPlayer::getStoredYieldTypes() const
{
	// Currently a bit pointless, but here there is a single location to alter all of the python code using this should we need to update.

	EnumMap<YieldTypes, bool, true> em;

	for (YieldTypes eYield = em.FIRST; eYield <= em.LAST; ++eYield)
	{
		if (eYield >= NUM_CARGO_YIELD_TYPES // only show cargo yields
			|| (m_pPlayer && !m_pPlayer->CivEffect().canUseYield(eYield))) // remove yields not used by the player
		{
			em.set(eYield, false);
		}
	}

	return new CyInfoArray(em);
}

CyInfoArray* CyPlayer::getDomesticDemandYieldTypes() const
{
	EnumMap<YieldTypes, bool> em;

	const InfoArray<YieldTypes>& array = GC.getDomesticDemandYieldTypes();

	for (int i = 0; i < array.getLength(); ++i)
	{
		const YieldTypes eYield = array.get(i);
		if (!m_pPlayer || m_pPlayer->CivEffect().canUseYield(eYield))
		{
			em.set(eYield, true);
		}
	}

	return new CyInfoArray(em);
}

CyInfoArray* CyPlayer::getTeachUnitTypes(int iTeachLevel) const
{
	EnumMap<UnitTypes, bool> em;

	if (m_pPlayer != NULL)
	{
		const CvPlayerCivEffect& kPlayer = m_pPlayer->CivEffect();
		for (UnitTypes eUnit = em.FIRST; eUnit <= em.LAST; ++eUnit)
		{
			if (kPlayer.canUseUnit(eUnit) && GC.getUnitInfo(eUnit).NBMOD_GetTeachLevel() == iTeachLevel)
			{
				em.set(eUnit, true);
			}

		}
	}
	return new CyInfoArray(em);
}

int CyPlayer::getMaxTeachLevel() const
{
	int iLevel = 0;
	if (m_pPlayer != NULL)
	{
		const CvPlayerCivEffect& kPlayer = m_pPlayer->CivEffect();
		for (UnitTypes eUnit = FIRST_UNIT; eUnit < NUM_UNIT_TYPES; ++eUnit)
		{
			if (kPlayer.canUseUnit(eUnit))
			{
				const int iUnitLevel = GC.getUnitInfo(eUnit).NBMOD_GetTeachLevel();
				if (iUnitLevel > iLevel && iUnitLevel < 100)
				{
					iLevel = iUnitLevel;
				}
			}
		}
	}
	return iLevel;
}

// CivEffect
int CyPlayer::getCivEffectCount(CivEffectTypes eCivEffect) const
{
	return m_pPlayer ? m_pPlayer->CivEffect().getCivEffectCount(eCivEffect) : 0;
}


unsigned int CyPlayer::getNumUnitsOnDock() const
{
	return m_pPlayer ? m_pPlayer->CivEffect().getNumUnitsOnDock() : 0;
}
