//
// Python wrapper class for CvCity
//
#include "CvGameCoreDLL.h"
#include "CyCity.h"
#include "CvCity.h"
#include "CyPlot.h"
#include "CyArea.h"
#include "CyUnit.h"
#include "CvDLLPythonIFaceBase.h"
#include "CvGlobals.h"

CyCity::CyCity() : m_pCity(NULL)
{

}

CyCity::CyCity(CvCity* pCity) : m_pCity(pCity)
{

}

void CyCity::kill()
{
	if (m_pCity)
		m_pCity->kill();
}

void CyCity::createGreatGeneral(int /*UnitTypes*/ eGreatGeneralUnit, bool bIncrementExperience)
{
	if (m_pCity)
		m_pCity->createGreatGeneral((UnitTypes) eGreatGeneralUnit, bIncrementExperience);
}

// R&R, ray, Great Admirals - START
void CyCity::createGreatAdmiral(int /*UnitTypes*/ eGreatAdmiralUnit, bool bIncrementExperience)
{
	if (m_pCity)
		m_pCity->createGreatAdmiral((UnitTypes) eGreatAdmiralUnit, bIncrementExperience);
}
// R&R, ray, Great Admirals - END

// WTP, ray, Lieutenants and Captains - START
void CyCity::createBraveLieutenant(int /*UnitTypes*/ eBraveLieutenantUnit)
{
	if (m_pCity)
		m_pCity->createBraveLieutenant((UnitTypes) eBraveLieutenantUnit);
}
void CyCity::createCapableCaptain(int /*UnitTypes*/ eCapableCaptainUnit)
{
	if (m_pCity)
		m_pCity->createCapableCaptain((UnitTypes) eCapableCaptainUnit);
}
// WTP, ray, Lieutenants and Captains - END

void CyCity::doTask(int /*TaskTypes*/ eTask, int iData1, int iData2, bool bOption)
{
	if (m_pCity)
		m_pCity->doTask((TaskTypes)eTask, iData1, iData2, bOption);
}

void CyCity::chooseProduction(int /*UnitTypes*/ eTrainUnit, int /*BuildingTypes*/ eConstructBuilding, bool bFinish, bool bFront)
{
	if (m_pCity)
		m_pCity->chooseProduction((UnitTypes) eTrainUnit, (BuildingTypes) eConstructBuilding, bFinish, bFront);
}

int CyCity::getCityPlotIndex(CyPlot* pPlot)
{
	return m_pCity ? m_pCity->getCityPlotIndex(pPlot->getPlot()) : -1;
}

CyPlot* CyCity::getCityIndexPlot(int iIndex)
{
	return m_pCity ? new CyPlot(m_pCity->getCityIndexPlot(static_cast<CityPlotTypes>(iIndex))) : NULL;
}

bool CyCity::canWork(CyPlot* pPlot)
{
	return m_pCity ? m_pCity->canWork(pPlot ? pPlot->getPlot() : NULL) : false;
}

void CyCity::clearWorkingOverride(int iIndex)
{
	if (m_pCity)
		m_pCity->clearWorkingOverride(iIndex);
}

int CyCity::countNumImprovedPlots()
{
	return m_pCity ? m_pCity->countNumImprovedPlots() : -1;
}

int CyCity::countNumWaterPlots()
{
	return m_pCity ? m_pCity->countNumWaterPlots() : -1;
}

int CyCity::countNumRiverPlots()
{
	return m_pCity ? m_pCity->countNumRiverPlots() : -1;
}

int CyCity::findPopulationRank()
{
	return m_pCity ? m_pCity->findPopulationRank() : -1;
}

int CyCity::findBaseYieldRateRank(int /*YieldTypes*/ eYield)
{
	return m_pCity ? m_pCity->findBaseYieldRateRank((YieldTypes) eYield) : -1;
}

int CyCity::findYieldRateRank(int /*YieldTypes*/ eYield)
{
	return m_pCity ? m_pCity->findYieldRateRank((YieldTypes) eYield) : -1;
}

int /*UnitTypes*/ CyCity::allUpgradesAvailable(int /*UnitTypes*/ eUnitType, int iUpgradeCount)
{
	return m_pCity ? m_pCity->allUpgradesAvailable((UnitTypes) eUnitType, iUpgradeCount) : -1;
}

bool CyCity::canTrain( int /*UnitTypes*/ eUnit, bool bContinue, bool bTestVisible )
{
	return m_pCity ? m_pCity->canTrain((UnitTypes)eUnit, bContinue, bTestVisible) : false;
}

bool CyCity::canConstruct( int /*BuildingTypes*/ eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost)
{
	return m_pCity ? m_pCity->canConstruct((BuildingTypes)eBuilding, bContinue, bTestVisible, bIgnoreCost) : false;
}

bool CyCity::canConvince( int /*FatherPointTypes*/ iFatherPointType, bool bContinue, bool bTestVisible)
{
	return m_pCity ? m_pCity->canConvince((FatherPointTypes)iFatherPointType, bContinue, bTestVisible) : false;
}

int CyCity::getFoodTurnsLeft()
{
	return m_pCity ? m_pCity->getFoodTurnsLeft() : 0;
}

bool CyCity::isProduction()
{
	return m_pCity ? m_pCity->isProduction() : false;
}

bool CyCity::isProductionUnit()
{
	return m_pCity ? m_pCity->isProductionUnit() : false;
}

bool CyCity::isProductionBuilding()
{
	return m_pCity ? m_pCity->isProductionBuilding() : false;
}

bool CyCity::canContinueProduction(OrderData order)
{
	return m_pCity ? m_pCity->canContinueProduction(order) : false;
}

int CyCity::getProductionExperience(int /*UnitTypes*/ eUnit)
{
	return m_pCity ? m_pCity->getProductionExperience((UnitTypes) eUnit) : -1;
}

void CyCity::addProductionExperience(CyUnit* pUnit)
{
	if (m_pCity)
		m_pCity->addProductionExperience(pUnit->getUnit());
}

int /*UnitTypes*/ CyCity::getProductionUnit()
{
	return m_pCity ? m_pCity->getProductionUnit() : NO_UNIT;
}

int /*UnitAITypes*/  CyCity::getProductionUnitAI()
{
	return m_pCity ? m_pCity->getProductionUnitAI() : NO_UNIT;
}

int /*BuildingTypes*/ CyCity::getProductionBuilding()
{
	return m_pCity ? m_pCity->getProductionBuilding() : NO_BUILDING;
}

std::wstring CyCity::getProductionName()
{
	return m_pCity ? m_pCity->getProductionName() : L"";
}

int CyCity::getGeneralProductionTurnsLeft()
{
	return m_pCity ? m_pCity->getGeneralProductionTurnsLeft() : -1;
}

std::wstring CyCity::getProductionNameKey()
{
	return m_pCity ? m_pCity->getProductionNameKey() : L"";
}

int CyCity::getFirstUnitOrder(int /*UnitTypes*/ eUnit)
{
	return m_pCity ? m_pCity->getFirstUnitOrder((UnitTypes)eUnit) : -1;
}

int CyCity::getFirstBuildingOrder(int /*BuildingTypes*/ eBuilding)
{
	return m_pCity ? m_pCity->getFirstBuildingOrder((BuildingTypes)eBuilding) : -1;
}

int CyCity::getNumTrainUnitAI(int /*UnitAITypes*/ eUnitAI)
{
	return m_pCity ? m_pCity->getNumTrainUnitAI((UnitAITypes) eUnitAI) : -1;
}

int CyCity::getProduction()
{
	return m_pCity ? m_pCity->getProduction() : -1;
}

int CyCity::getProductionNeeded(int /*YieldTypes*/ eYield)
{
	return m_pCity ? m_pCity->getProductionNeeded((YieldTypes) eYield) : -1;
}

int CyCity::getProductionTurnsLeft()
{
	return m_pCity ? m_pCity->getProductionTurnsLeft() : -1;
}

int CyCity::getUnitProductionTurnsLeft(int /*UnitTypes*/ iUnit, int iNum)
{
	return m_pCity ? m_pCity->getProductionTurnsLeft((UnitTypes) iUnit, iNum) : -1;
}

int CyCity::getBuildingProductionTurnsLeft(int /*BuildingTypes*/ iBuilding, int iNum)
{
	return m_pCity ? m_pCity->getProductionTurnsLeft((BuildingTypes) iBuilding, iNum) : -1;
}

void CyCity::setProduction(int iNewValue)
{
	if (m_pCity)
		m_pCity->setProduction(iNewValue);
}

void CyCity::changeProduction(int iChange)
{
	if (m_pCity)
		m_pCity->changeProduction(iChange);
}

int CyCity::getProductionModifier()
{
	return m_pCity ? m_pCity->getProductionModifier() : -1;
}

int CyCity::getCurrentProductionDifference(bool bOverflow)
{
	return m_pCity ? m_pCity->getCurrentProductionDifference(bOverflow) : -1;
}

int CyCity::getUnitProductionModifier(int /*UnitTypes*/ iUnit)
{
	return m_pCity ? m_pCity->getProductionModifier((UnitTypes)iUnit) : -1;
}

int CyCity::getBuildingProductionModifier(int /*BuildingTypes*/ iBuilding)
{
	return m_pCity ? m_pCity->getProductionModifier((BuildingTypes)iBuilding) : -1;
}

bool CyCity::canHurry(int /*HurryTypes*/ iHurry, bool bTestVisible)
{
	return m_pCity ? m_pCity->canHurry((HurryTypes)iHurry, bTestVisible) : false;
}

void CyCity::hurry(int /*HurryTypes*/ iHurry)
{
	if (m_pCity)
		m_pCity->hurry((HurryTypes)iHurry);
}

int /* HandicapTypes */ CyCity::getHandicapType()
{
	return m_pCity ? m_pCity->getHandicapType() : NO_HANDICAP;
}

int /* CivilizationTypes */ CyCity::getCivilizationType()
{
	return m_pCity ? m_pCity->getCivilizationType() : NO_CIVILIZATION;
}

int /* LeaderHeadTypes */ CyCity::getPersonalityType()
{
	return m_pCity ? m_pCity->getPersonalityType() : NO_LEADER;
}

int /* ArtStyleTypes */ CyCity::getArtStyleType()
{
	return m_pCity ? m_pCity->getArtStyleType() : -1;
}

int /* CitySizeTypes */ CyCity::getCitySizeType()
{
	return m_pCity ? m_pCity->getCitySizeType() : -1;
}

bool CyCity::hasTrait(int /*TraitTypes*/ iTrait)
{
	return m_pCity ? m_pCity->hasTrait((TraitTypes) iTrait) : false;
}

bool CyCity::isHuman()
{
	return m_pCity ? m_pCity->isHuman() : false;
}

bool CyCity::isVisible(int /*TeamTypes*/ eTeam, bool bDebug)
{
	return m_pCity ? m_pCity->isVisible((TeamTypes) eTeam, bDebug) : false;
}

bool CyCity::isCapital()
{
	return m_pCity ? m_pCity->isCapital() : false;
}

bool CyCity::isCoastal(int iMinWaterSize)
{
	return m_pCity ? m_pCity->isCoastal(iMinWaterSize) : false;
}

bool CyCity::isDisorder()
{
	return m_pCity ? m_pCity->isDisorder() : false;
}

int CyCity::extraPopulation()
{
	return m_pCity ? m_pCity->extraPopulation() : -1;
}

int CyCity::foodConsumption(int iExtra)
{
	return m_pCity ? m_pCity->foodConsumption(iExtra) : -1;
}

int CyCity::foodDifference()
{
	return m_pCity ? m_pCity->foodDifference() : -1;
}

int CyCity::growthThreshold()
{
	return m_pCity ? m_pCity->growthThreshold() : -1;
}

int CyCity::productionLeft()
{
	return m_pCity ? m_pCity->productionLeft() : -1;
}

int CyCity::hurryCost(bool bExtra, bool bIgnoreNew)
{
	return m_pCity ? m_pCity->hurryCost(bExtra, bIgnoreNew) : -1;
}

int CyCity::hurryGold(int /*HurryTypes*/ iHurry)
{
	return m_pCity ? m_pCity->hurryGold((HurryTypes)iHurry) : -1;
}

int CyCity::hurryPopulation(int /*HurryTypes*/ iHurry)
{
	return m_pCity ? m_pCity->hurryPopulation((HurryTypes)iHurry) : -1;
}

int CyCity::hurryProduction(int /*HurryTypes*/ iHurry)
{
	return m_pCity ? m_pCity->hurryProduction((HurryTypes)iHurry) : -1;
}

int CyCity::maxHurryPopulation()
{
	return m_pCity ? m_pCity->maxHurryPopulation() : -1;
}

int CyCity::cultureDistance(int iDX, int iDY)
{
	return m_pCity ? m_pCity->cultureDistance(iDX, iDY) : -1;
}

bool CyCity::isHasBuilding(int /*BuildingTypes*/ iIndex)
{
	return m_pCity ? m_pCity->isHasBuilding((BuildingTypes) iIndex) : false;
}

int CyCity::getProfessionOutput(int /*ProfessionTypse*/ iIndex, CyUnit* pUnit)
{
	return m_pCity ? m_pCity->getProfessionOutput((ProfessionTypes) iIndex, pUnit->getUnit()) : -1;
}

int CyCity::getID()
{
	return m_pCity ? m_pCity->getID() : -1;
}

int CyCity::getX()
{
	return m_pCity ? m_pCity->getX_INLINE() : -1;
}

int CyCity::getY()
{
	return m_pCity ? m_pCity->getY_INLINE() : -1;
}

bool CyCity::at(int iX, int iY)
{
	return m_pCity ? m_pCity->at(iX, iY) : false;
}

bool CyCity::atPlot(CyPlot* pPlot)
{
	return m_pCity ? m_pCity->at(pPlot->getPlot()) : false;
}

CyPlot* CyCity::plot()
{
	return	m_pCity ? new CyPlot(m_pCity->plot()) : NULL;
}

CyArea* CyCity::area()
{
	return	m_pCity ? new CyArea(m_pCity->area()) : NULL;
}

CyArea* CyCity::waterArea()
{
	return	m_pCity ? new CyArea(m_pCity->waterArea()) : NULL;
}

CyPlot* CyCity::getRallyPlot()
{
	return	m_pCity ? new CyPlot(m_pCity->getRallyPlot()) : NULL;
}

int CyCity::getGameTurnFounded()
{
	return m_pCity ? m_pCity->getGameTurnFounded() : -1;
}

int CyCity::getGameTurnAcquired()
{
	return m_pCity ? m_pCity->getGameTurnAcquired() : -1;
}

int CyCity::getPopulation()
{
	return m_pCity ? m_pCity->getPopulation() : -1;
}

void CyCity::setPopulation(int iNewValue)
{
	if (m_pCity)
		m_pCity->setPopulation(iNewValue);
}

void CyCity::changePopulation(int iChange)
{
	if (m_pCity)
		m_pCity->changePopulation(iChange);
}

long CyCity::getRealPopulation()
{
	return m_pCity ? m_pCity->getRealPopulation() : -1;
}

int CyCity::getHighestPopulation()
{
	return m_pCity ? m_pCity->getHighestPopulation() : -1;
}

void CyCity::setHighestPopulation(int iNewValue)
{
	if (m_pCity)
		m_pCity->setHighestPopulation(iNewValue);
}

bool CyCity::isWorksWater()
{
	return m_pCity ? m_pCity->isWorksWater() : false;
}

void CyCity::changeHealRate(int iChange)
{
	if (m_pCity)
		m_pCity->changeHealRate(iChange);
}

int CyCity::getFood()
{
	return m_pCity ? m_pCity->getFood() : -1;
}

void CyCity::setFood(int iNewValue)
{
	if (m_pCity)
		m_pCity->setFood(iNewValue);
}

void CyCity::changeFood(int iChange)
{
	if (m_pCity)
		m_pCity->changeFood(iChange);
}

int CyCity::getFoodKept()
{
	return m_pCity ? m_pCity->getFoodKept() : -1;
}

int CyCity::getMaxFoodKeptPercent()
{
	return m_pCity ? m_pCity->getMaxFoodKeptPercent() : -1;
}

int CyCity::getOverflowProduction()
{
	return m_pCity ? m_pCity->getOverflowProduction() : -1;
}

void CyCity::setOverflowProduction(int iNewValue)
{
	if (m_pCity)
		m_pCity->setOverflowProduction(iNewValue);
}

int CyCity::getMilitaryProductionModifier()
{
	return m_pCity ? m_pCity->getMilitaryProductionModifier() : -1;
}

int CyCity::getBuildingDefense()
{
	return m_pCity ? m_pCity->getBuildingDefense() : -1;
}

int CyCity::getBuildingBombardDefense()
{
	return m_pCity ? m_pCity->getBuildingBombardDefense() : -1;
}

int CyCity::getFreeExperience()
{
	return m_pCity ? m_pCity->getFreeExperience() : -1;
}

int CyCity::getDefenseDamage()
{
	return m_pCity ? m_pCity->getDefenseDamage() : -1;
}

void CyCity::changeDefenseDamage(int iChange)
{
	if (m_pCity)
		m_pCity->changeDefenseDamage(iChange);
}

bool CyCity::isBombardable(CyUnit* pUnit)
{
	return m_pCity ? m_pCity->isBombardable(pUnit->getUnit()) : false;
}

int CyCity::getTotalDefense()
{
	return m_pCity ? m_pCity->getTotalDefense() : -1;
}

int CyCity::getDefenseModifier()
{
	return m_pCity ? m_pCity->getDefenseModifier() : -1;
}

int CyCity::getOccupationTimer()
{
	return m_pCity ? m_pCity->getOccupationTimer() : -1;
}

bool CyCity::isOccupation()
{
	return m_pCity ? m_pCity->isOccupation() : false;
}

void CyCity::setOccupationTimer(int iNewValue)
{
	if (m_pCity)
		m_pCity->setOccupationTimer(iNewValue);
}

void CyCity::changeOccupationTimer(int iChange)
{
	if (m_pCity)
		m_pCity->changeOccupationTimer(iChange);
}

int CyCity::getCultureUpdateTimer()
{
	return m_pCity ? m_pCity->getCultureUpdateTimer() : -1;
}

void CyCity::changeCultureUpdateTimer(int iChange)
{
	if (m_pCity)
		m_pCity->changeCultureUpdateTimer(iChange);
}

bool CyCity::isNeverLost()
{
	return m_pCity ? m_pCity->isNeverLost() : false;
}

void CyCity::setNeverLost(int iNewValue)
{
	if (m_pCity)
		m_pCity->setNeverLost(iNewValue);
}

bool CyCity::isBombarded()
{
	return m_pCity ? m_pCity->isBombarded(): false;
}

void CyCity::setBombarded(int iNewValue)
{
	if (m_pCity)
		m_pCity->setBombarded(iNewValue);
}

void CyCity::setAllCitizensAutomated(bool bAutomated)
{
	if (m_pCity)
		m_pCity->setAllCitizensAutomated(bAutomated);
}

bool CyCity::isProductionAutomated()
{
	return m_pCity ? m_pCity->isProductionAutomated() : false;
}

void CyCity::setProductionAutomated(bool bNewValue)
{
	if (m_pCity)
	{
		m_pCity->setProductionAutomated(bNewValue, false);
	}
}

bool CyCity::isWallOverride() const
{
	return m_pCity ? m_pCity->isWallOverride() : false;
}

void CyCity::setWallOverride(bool bOverride)
{
	if (m_pCity)
		m_pCity->setWallOverride(bOverride);
}

void CyCity::setCitySizeBoost(int iBoost)
{
	if (m_pCity)
		m_pCity->setCitySizeBoost(iBoost);
}

int /*PlayerTypes*/ CyCity::getOwner()
{
	return m_pCity ? m_pCity->getOwnerINLINE() : NO_PLAYER;
}

int /*TeamTypes*/ CyCity::getTeam()
{
	return m_pCity ? m_pCity->getTeam() : NO_TEAM;
}

int /*PlayerTypes*/ CyCity::getPreviousOwner()
{
	return m_pCity ? m_pCity->getPreviousOwner() : NO_PLAYER;
}

int /*PlayerTypes*/ CyCity::getOriginalOwner()
{
	return m_pCity ? m_pCity->getOriginalOwner() : NO_PLAYER;
}

int /*CultureLevelTypes*/ CyCity::getCultureLevel()
{
	return m_pCity ? m_pCity->getCultureLevel() : NO_CULTURELEVEL;
}

int CyCity::getCultureThreshold()
{
	return m_pCity ? m_pCity->getCultureThreshold() : -1;
}

int CyCity::getSeaPlotYield(int /*YieldTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getSeaPlotYield((YieldTypes) eIndex) : -1;
}

int CyCity::getRiverPlotYield(int /*YieldTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getRiverPlotYield((YieldTypes) eIndex) : -1;
}

int CyCity::getBaseRawYieldProduced(int /*YieldTypes*/ eIndex)
{
	return m_pCity ? m_pCity->yields().getBaseRawYieldProduced((YieldTypes)eIndex) : -1;
}

int CyCity::getRawYieldProduced(int /*YieldTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getRawYieldProduced((YieldTypes)eIndex) : -1;
}

int CyCity::getRawYieldConsumed(int /*YieldTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getRawYieldConsumed((YieldTypes)eIndex) : -1;
}

int CyCity::getBaseYieldRateModifier(int /*YieldTypes*/ eIndex, int iExtra)
{
	return m_pCity ? m_pCity->getBaseYieldRateModifier((YieldTypes)eIndex, iExtra) : -1;
}

int CyCity::getYieldRate(int /*YieldTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getYieldRate((YieldTypes)eIndex) : -1;
}

int CyCity::getCultureRate()
{
	return m_pCity ? m_pCity->getCultureRate() : -1;
}

int CyCity::getYieldRateModifier(int /*YieldTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getYieldRateModifier((YieldTypes)eIndex) : -1;
}

int CyCity::getDomainFreeExperience(int /*DomainTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getDomainFreeExperience((DomainTypes)eIndex) : -1;
}

int CyCity::getDomainProductionModifier(int /*DomainTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getDomainProductionModifier((DomainTypes)eIndex) : -1;
}

int CyCity::getCulture(int /*PlayerTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getCulture((PlayerTypes)eIndex) : -1;
}

int CyCity::countTotalCulture()
{
	return m_pCity ? m_pCity->countTotalCulture() : -1;
}

PlayerTypes CyCity::findHighestCulture()
{
	return m_pCity ? m_pCity->findHighestCulture() : NO_PLAYER;
}

int CyCity::calculateCulturePercent(int /*PlayerTypes*/ eIndex)
{
	return m_pCity ? m_pCity->calculateCulturePercent((PlayerTypes)eIndex) : -1;
}

int CyCity::calculateTeamCulturePercent(int /*TeamTypes*/ eIndex)
{
	return m_pCity ? m_pCity->calculateTeamCulturePercent((TeamTypes)eIndex) : -1;
}

void CyCity::setCulture(int /*PlayerTypes*/ eIndex, int iNewValue, bool bPlots)
{
	if (m_pCity)
		m_pCity->setCulture((PlayerTypes)eIndex, iNewValue, bPlots);
}

void CyCity::changeCulture(int /*PlayerTypes*/ eIndex, int iChange, bool bPlots)
{
	if (m_pCity)
		m_pCity->changeCulture((PlayerTypes)eIndex, iChange, bPlots);
}

int CyCity::getTotalYieldStored() const
{
	return m_pCity ? m_pCity->getTotalYieldStored() : -1;
}

int CyCity::getYieldStored(int /*YieldTypes*/ eYield) const
{
	return m_pCity ? m_pCity->getYieldStored((YieldTypes)eYield) : -1;
}

void CyCity::setYieldStored(int /*YieldTypes*/ eYield, int iValue)
{
	if (m_pCity)
		m_pCity->setYieldStored((YieldTypes)eYield, iValue);
}

void CyCity::changeYieldStored(int /*YieldTypes*/ eYield, int iChange)
{
	if (m_pCity)
		m_pCity->changeYieldStored((YieldTypes)eYield, iChange);
}

int CyCity::getYieldRushed(int /*YieldTypes*/ eYield) const
{
	return m_pCity ? m_pCity->getYieldRushed((YieldTypes)eYield) : -1;
}

int CyCity::calculateNetYield(int /*YieldTypes*/ eYield)
{
	return m_pCity ? m_pCity->calculateNetYield((YieldTypes)eYield) : -1;
}

int CyCity::calculateActualYieldProduced(int /*YieldTypes*/ eYield) const
{
	return m_pCity ? m_pCity->calculateActualYieldProduced((YieldTypes)eYield) : -1;
}

int CyCity::calculateActualYieldConsumed(int /*YieldTypes*/ eYield) const
{
	return m_pCity ? m_pCity->calculateActualYieldConsumed((YieldTypes)eYield) : -1;
}
// R&R, Robert Surcouf, Domestic Market display START

int CyCity::getYieldBuyPrice(int /*YieldTypes*/ eYield) const
{
	return m_pCity ? m_pCity->getYieldBuyPrice((YieldTypes)eYield) : -1;
}
int CyCity::getYieldDemand(int /*YieldTypes*/ eYield) const
{
	if (eYield >= 0 && eYield < NUM_CARGO_YIELD_TYPES)
	{
		return m_pCity ? m_pCity->getYieldDemand((YieldTypes)eYield) : -1;
	}
	return -1;
}
//R&R, Robert Surcouf, Domestic Market display END

bool CyCity::isEverOwned(int /*PlayerTypes*/ eIndex)
{
	return m_pCity ? m_pCity->isEverOwned((PlayerTypes)eIndex) : false;
}
// R&R, Robert Surcouf, No More Variables Hidden game option START
PlayerTypes CyCity::getMissionaryPlayer() const
{
	return m_pCity ? m_pCity->getMissionaryPlayer() :NO_PLAYER;
}
int CyCity::getMissionaryRate() const
{
	return m_pCity ? m_pCity->getMissionaryRate() : -1;
}
// R&R, Robert Surcouf, No More Variables Hidden game option END

// WTP, ray, Native Trade Posts - START
PlayerTypes CyCity::getTradePostPlayer() const
{
	return m_pCity ? m_pCity->getTradePostPlayer() :NO_PLAYER;
}
int CyCity::getNativeTradeRate() const
{
	return m_pCity ? m_pCity->getNativeTradeRate() : -1;
}
// WTP, ray, Native Trade Posts - START

bool CyCity::isRevealed(int /*TeamTypes */eIndex, bool bDebug)
{
	return m_pCity ? m_pCity->isRevealed((TeamTypes)eIndex, bDebug) : false;
}

void CyCity::setRevealed(int /*TeamTypes*/ eIndex, bool bNewValue)
{
	if (m_pCity)
		m_pCity->setRevealed((TeamTypes)eIndex, bNewValue);
}

std::wstring CyCity::getName()
{
	return m_pCity ? m_pCity->getName() : L"";
}

std::wstring CyCity::getNameForm(int iForm)
{
	return m_pCity ? m_pCity->getName((uint)iForm) : L"";
}

std::wstring CyCity::getNameKey()
{
	return m_pCity ? m_pCity->getNameKey() : L"";
}

void CyCity::setName(std::wstring szNewValue, bool bFound)
{
	if (m_pCity)
		m_pCity->setName(CvWString(szNewValue), bFound);
}

int CyCity::getBuildingProduction(int /*BuildingTypes*/ iIndex)
{
	return m_pCity ? m_pCity->getBuildingProduction((BuildingTypes) iIndex) : -1;
}

void CyCity::setBuildingProduction(int /*BuildingTypes*/ iIndex, int iNewValue)
{
	if (m_pCity)
		m_pCity->setBuildingProduction((BuildingTypes) iIndex, iNewValue);
}

void CyCity::changeBuildingProduction(int /*BuildingTypes*/ iIndex, int iChange)
{
	if (m_pCity)
		m_pCity->changeBuildingProduction((BuildingTypes) iIndex, iChange);
}

int CyCity::getBuildingProductionTime(int /*BuildingTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getBuildingProductionTime((BuildingTypes)eIndex) : -1;
}

void CyCity::setBuildingProductionTime(int /*BuildingTypes*/ eIndex, int iNewValue)
{
	if (m_pCity)
		m_pCity->setBuildingProductionTime((BuildingTypes)eIndex, iNewValue);
}

void CyCity::changeBuildingProductionTime(int /*BuildingTypes*/ eIndex, int iChange)
{
	if (m_pCity)
		m_pCity->changeBuildingProductionTime((BuildingTypes) eIndex, iChange);
}

int CyCity::getBuildingOriginalOwner(int /*BuildingTypes*/ iIndex)
{
	return m_pCity ? m_pCity->getBuildingOriginalOwner((BuildingTypes) iIndex) : -1;
}

int CyCity::getBuildingOriginalTime(int /*BuildingTypes*/ iIndex)
{
	return m_pCity ? m_pCity->getBuildingOriginalTime((BuildingTypes) iIndex) : -1;
}

int CyCity::getUnitProduction(int iIndex)
{
	return m_pCity ? m_pCity->getUnitProduction((UnitTypes) iIndex) : -1;
}

void CyCity::setUnitProduction(int iIndex, int iNewValue)
{
	if (m_pCity)
		m_pCity->setUnitProduction((UnitTypes)iIndex, iNewValue);
}

void CyCity::changeUnitProduction(int /*UnitTypes*/ iIndex, int iChange)
{
	if (m_pCity)
		m_pCity->changeUnitProduction((UnitTypes) iIndex, iChange);
}

int CyCity::getUnitCombatFreeExperience(int /*UnitCombatTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getUnitCombatFreeExperience((UnitCombatTypes) eIndex) : -1;
}

int CyCity::getFreePromotionCount(int /*PromotionTypes*/ eIndex)
{
	return m_pCity ? m_pCity->getFreePromotionCount((PromotionTypes) eIndex) : -1;
}

bool CyCity::isFreePromotion(int /*PromotionTypes*/ eIndex)
{
	return m_pCity ? m_pCity->isFreePromotion((PromotionTypes) eIndex) : false;
}

bool CyCity::isHasRealBuilding(int /*BuildingTypes*/ iIndex)
{
	BuildingTypes eBuilding = static_cast<BuildingTypes>(iIndex);
	if (m_pCity && isInRange(eBuilding))
	{
		return m_pCity->isHasRealBuilding(eBuilding);
	}
	return false;
}

void CyCity::setHasRealBuilding(int /*BuildingTypes*/ iIndex, bool bNewValue)
{
	if (m_pCity)
		m_pCity->setHasRealBuilding((BuildingTypes) iIndex, bNewValue);
}

bool CyCity::isHasFreeBuilding(int /*BuildingTypes*/ iIndex)
{
	return m_pCity ? m_pCity->isHasFreeBuilding((BuildingTypes) iIndex) : false;
}

int CyCity::getDominantBuilding(int /*SpecialBuildingTypes*/ iSpecialBuilding) const
{
	return m_pCity ? m_pCity->getDominantBuilding(static_cast<SpecialBuildingTypes>(iSpecialBuilding)) : -1;
}

void CyCity::clearOrderQueue()
{
	if (m_pCity)
		m_pCity->clearOrderQueue();
}

void CyCity::pushOrder(OrderTypes eOrder, int iData1, int iData2, bool bSave, bool bPop, bool bAppend, bool bForce)
{
	if (m_pCity)
		m_pCity->pushOrder(eOrder, iData1, iData2, bSave, bPop, bAppend, bForce);
}

void CyCity::popOrder(int iNum, bool bFinish, bool bChoose)
{
	if (m_pCity)
		m_pCity->popOrder(iNum, bFinish, bChoose);
}

int CyCity::getOrderQueueLength()
{
	return m_pCity ? m_pCity->getOrderQueueLength() : -1;
}

OrderData* CyCity::getOrderFromQueue(int iIndex)
{
	return m_pCity ? m_pCity->getOrderFromQueue(iIndex) : NULL;
}

void CyCity::setWallOverridePoints(const python::tuple& kPoints)
{
	if (!m_pCity)
		return;

	float* pPointsData = NULL;
	int iSeqLength = gDLL->getPythonIFace()->putSeqInArray(kPoints.ptr() /*src*/, &pPointsData /*dst*/);

	// copy to pairs vector
	std::vector< std::pair<float, float> > pointsVec;
	pointsVec.reserve(iSeqLength/2);
	int i;
	for(i=0;i<iSeqLength;i+=2)
	{
		std::pair<float, float> pr(pPointsData[i], pPointsData[i+1]);
		pointsVec.push_back(pr);
	}

	m_pCity->setWallOverridePoints(pointsVec);

	delete [] pPointsData;

}

python::tuple CyCity::getWallOverridePoints() const
{
	python::tuple tup = python::make_tuple();
	if (m_pCity)
	{
		std::vector< std::pair<float, float> > pointsVec = m_pCity->getWallOverridePoints();
		uint i;
		for(i=0;i<pointsVec.size();i++)
			tup += python::make_tuple(pointsVec[i].first, pointsVec[i].second);
	}

	return tup;
}

int CyCity::AI_getEmphasizeYieldCount(int /*YieldTypes*/ eYield) const
{
	return m_pCity ? m_pCity->AI_getEmphasizeYieldCount((YieldTypes) eYield) : 0;
}

bool CyCity::AI_avoidGrowth()
{
	return m_pCity ? m_pCity->AI_avoidGrowth() : false;
}

bool CyCity::AI_isEmphasize(int iEmphasizeType)
{
	return m_pCity ? m_pCity->AI_isEmphasize((EmphasizeTypes)iEmphasizeType) : false;
}

int CyCity::AI_countBestBuilds(CyArea* pArea)
{
	return m_pCity ? m_pCity->AI_countBestBuilds(pArea->getArea()) : -1;
}

int CyCity::AI_cityValue()
{
	return m_pCity ? m_pCity->AI_cityValue() : -1;
}
// R&R, Robert Surcouf, No More Variables Hidden game option START
int /*YieldTypes*/ CyCity::AI_getDesiredYield() const
{
	return m_pCity ? m_pCity->AI_getDesiredYield() : 0;
}
// R&R, Robert Surcouf, No More Variables Hidden game option END

std::string CyCity::getScriptData() const
{
	return m_pCity ? m_pCity->getScriptData() : "";
}

void CyCity::setScriptData(std::string szNewValue)
{
	if (m_pCity)
		m_pCity->setScriptData(szNewValue);
}

int CyCity::getBuildingYieldChange(int /*BuildingClassTypes*/ eBuildingClass, int /*YieldTypes*/ eYield) const
{
	return m_pCity ? m_pCity->getBuildingYieldChange((BuildingClassTypes)eBuildingClass, (YieldTypes)eYield) : 0;
}

void CyCity::setBuildingYieldChange(int /*BuildingClassTypes*/ eBuildingClass, int /*YieldTypes*/ eYield, int iChange)
{
	if (m_pCity)
	{
		m_pCity->setBuildingYieldChange((BuildingClassTypes)eBuildingClass, (YieldTypes)eYield, iChange);
	}
}

int CyCity::getLiberationPlayer(bool bConquest)
{
	return (m_pCity ? m_pCity->getLiberationPlayer(bConquest) : -1);
}

void CyCity::liberate(bool bConquest)
{
	if (m_pCity)
	{
		m_pCity->liberate(bConquest);
	}
}

int CyCity::getMaxYieldCapacity()
{
	return (m_pCity ? m_pCity->getMaxYieldCapacity() : -1);
}

bool CyCity::isScoutVisited(int /*TeamTypes*/ eTeam) const
{
	return (m_pCity ? m_pCity->isScoutVisited((TeamTypes) eTeam) : false);
}

CyUnit* CyCity::getPopulationUnitByIndex(int iUnitIndex)
{
	return m_pCity ? new CyUnit(m_pCity->getPopulationUnitByIndex(iUnitIndex)) : NULL;
}

CyUnit* CyCity::getPopulationUnitById(int iUnitID)
{
	return m_pCity ? new CyUnit(m_pCity->getPopulationUnitById(iUnitID)) : NULL;
}

int CyCity::getPopulationUnitIndex(CyUnit* pUnit)
{
	return m_pCity ? m_pCity->getPopulationUnitIndex(*pUnit->getUnit()) : -1;
}

bool CyCity::canTeach(int iUnit) const
{
	return m_pCity ? m_pCity->canTeach(static_cast<UnitTypes>(iUnit)) : false;
}

int CyCity::getTeachUnitClass()
{
	return m_pCity ? m_pCity->getTeachUnitClass() : -1;
}

// native advisor update - start - Nightinggale
int CyCity::getTeachUnit() const
{
	PlayerTypes ePlayer = GC.getGameINLINE().getActivePlayer();
	if (m_pCity == NULL || ePlayer == NO_PLAYER)
	{
		return NO_UNIT;
	}
	UnitClassTypes eClassType = m_pCity->getTeachUnitClass();
	if (eClassType == NO_UNITCLASS)
	{
		return NO_UNIT;
	}
	return GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType()).getCivilizationUnits(eClassType);
}
// native advisor update - end - Nightinggale

int CyCity::getRebelPercent()
{
	return m_pCity ? m_pCity->getRebelPercent() : -1;
}

int CyCity::getRebelSentiment() const
{
	return m_pCity ? m_pCity->getRebelSentiment() : -1;
}

void CyCity::setRebelSentiment(int iValue)
{
	if (m_pCity)
	{
		m_pCity->setRebelSentiment(iValue);
	}
}

// WTP, ray, new Harbour System - START
int CyCity::getCityHarbourSpace() const
{
	return m_pCity ? m_pCity->getCityHarbourSpace() : -1;
}

int CyCity::getCityHarbourSpaceUsed() const
{
	return m_pCity ? m_pCity->getCityHarbourSpaceUsed() : -1;
}

bool CyCity::bShouldShowCityHarbourSystem() const
{
	return m_pCity ? m_pCity->bShouldShowCityHarbourSystem() : -1;
}
// WTP, ray, new Harbour System - END

// WTP, ray, new Barracks System - START
int CyCity::getCityBarracksSpace() const
{
	return m_pCity ? m_pCity->getCityBarracksSpace() : -1;
}

int CyCity::getCityBarracksSpaceUsed() const
{
	return m_pCity ? m_pCity->getCityBarracksSpaceUsed() : -1;
}

bool CyCity::bShouldShowCityBarracksSystem() const
{
	return m_pCity ? m_pCity->bShouldShowCityBarracksSystem() : -1;
}
// WTP, ray, new Barracks System - END


// R&R, ray, Health - START
int CyCity::getCityHealth() const
{
	return m_pCity ? m_pCity->getCityHealth() : -1;
}

int CyCity::getCityHealthChange() const
{
	return m_pCity ? m_pCity->getCityHealthChange() : -1;
}

void CyCity::setCityHealth(int iValue)
{
	if (m_pCity)
	{
		m_pCity->setCityHealth(iValue);
	}
}

void CyCity::changeCityHealth(int iValue)
{
	if (m_pCity)
	{
		m_pCity->changeCityHealth(iValue);
	}
}
// R&R, ray, Health - END


// WTP, ray, helper methods for Python Event System - Spawning Units and Barbarians on Plots - START
void CyCity::spawnOwnPlayerUnitOnPlotOfCity(int iIndex) const
{
	if (m_pCity)
		m_pCity->spawnOwnPlayerUnitOnPlotOfCity(iIndex);
}

void CyCity::spawnBarbarianUnitOnPlotOfCity(int iIndex) const
{
	if (m_pCity)
		m_pCity->spawnBarbarianUnitOnPlotOfCity(iIndex);
}

void CyCity::spawnOwnPlayerUnitOnAdjacentPlotOfCity(int iIndex) const
{
	if (m_pCity)
		m_pCity->spawnOwnPlayerUnitOnAdjacentPlotOfCity(iIndex);
}

void CyCity::spawnBarbarianUnitOnAdjacentPlotOfCity(int iIndex) const
{
	if (m_pCity)
		m_pCity->spawnBarbarianUnitOnAdjacentPlotOfCity(iIndex);
}

bool CyCity::isOwnPlayerUnitOnAdjacentPlotOfCity(int iIndex) const
{
	return m_pCity ? m_pCity->isOwnPlayerUnitOnAdjacentPlotOfCity(iIndex) : false;
}

bool CyCity::isBarbarianUnitOnAdjacentPlotOfCity(int iIndex) const
{
	return m_pCity ? m_pCity->isBarbarianUnitOnAdjacentPlotOfCity(iIndex) : false;
}
// WTP, ray, helper methods for Python Event System - Spawning Units and Barbarians on Plots - END


// WTP, ray, Happiness - START
int CyCity::getCityHappiness() const
{
	return m_pCity ? m_pCity->getCityHappiness() : -1;
}

void CyCity::setCityHappiness(int iValue)
{
	if (m_pCity)
	{
		m_pCity->setCityHappiness(iValue);
	}
}

void CyCity::updateCityHappiness()
{
	if (m_pCity)
	{
		m_pCity->updateCityHappiness();
	}
}

int CyCity::getCityUnHappiness() const
{
	return m_pCity ? m_pCity->getCityUnHappiness() : -1;
}

void CyCity::setCityUnHappiness(int iValue)
{
	if (m_pCity)
	{
		m_pCity->setCityUnHappiness(iValue);
	}
}

void CyCity::updateCityUnHappiness()
{
	if (m_pCity)
	{
		m_pCity->updateCityUnHappiness();
	}
}

int CyCity::getHappinessFromCrosses() const
{
	return m_pCity ? m_pCity->getHappinessFromCrosses() : -1;
}

int CyCity::getHappinessFromBells() const
{
	return m_pCity ? m_pCity->getHappinessFromBells() : -1;
}

int CyCity::getHappinessFromHealth() const
{
	return m_pCity ? m_pCity->getHappinessFromHealth() : -1;
}

int CyCity::getHappinessFromCulture() const
{
	return m_pCity ? m_pCity->getHappinessFromCulture() : -1;
}

int CyCity::getHappinessFromLaw() const
{
	return m_pCity ? m_pCity->getHappinessFromLaw() : -1;
}

int CyCity::getHappinessFromEducation() const
{
	return m_pCity ? m_pCity->getHappinessFromEducation() : -1;
}

int CyCity::getHappinessFromDomesticDemandsFulfilled() const
{
	return m_pCity ? m_pCity->getHappinessFromDomesticDemandsFulfilled() : -1;
}

int CyCity::getHappinessFromTreaties() const
{
	return m_pCity ? m_pCity->getHappinessFromTreaties() : -1;
}

int CyCity::getUnhappinessFromPopulation() const
{
	return m_pCity ? m_pCity->getUnhappinessFromPopulation() : -1;
}

int CyCity::getUnhappinessFromCrime() const
{
	return m_pCity ? m_pCity->getUnhappinessFromCrime() : -1;
}

int CyCity::getUnhappinessFromSlavery() const
{
	return m_pCity ? m_pCity->getUnhappinessFromSlavery() : -1;
}

int CyCity::getUnhappinessFromWars() const
{
	return m_pCity ? m_pCity->getUnhappinessFromWars() : -1;
}

int CyCity::getUnhappinessFromMissingDefense() const
{
	return m_pCity ? m_pCity->getUnhappinessFromMissingDefense() : -1;
}

int CyCity::getUnhappinessFromTaxRate() const
{
	return m_pCity ? m_pCity->getUnhappinessFromTaxRate() : -1;
}
// WTP, ray, Happiness - END

// WTP, ray, Crime and Law - START
int CyCity::getCityLaw() const
{
	return m_pCity ? m_pCity->getCityLaw() : -1;
}

void CyCity::setCityLaw(int iValue)
{
	if (m_pCity)
	{
		m_pCity->setCityLaw(iValue);
	}
}

void CyCity::updateCityLaw()
{
	if (m_pCity)
	{
		m_pCity->updateCityLaw();
	}
}

int CyCity::getCityCrime() const
{
	return m_pCity ? m_pCity->getCityCrime() : -1;
}

void CyCity::setCityCrime(int iValue)
{
	if (m_pCity)
	{
		m_pCity->setCityCrime(iValue);
	}
}

void CyCity::updateCityCrime()
{
	if (m_pCity)
	{
		m_pCity->updateCityCrime();
	}
}

int CyCity::getLawFromCityDefenders() const
{
	return m_pCity ? m_pCity->getLawFromCityDefenders() : -1;
}

int CyCity::getLawFromCrosses() const
{
	return m_pCity ? m_pCity->getLawFromCrosses() : -1;
}

int CyCity::getCrimeFromPopulation() const
{
	return m_pCity ? m_pCity->getCrimeFromPopulation() : -1;
}

int CyCity::getCrimeFromUnhappiness() const
{
	return m_pCity ? m_pCity->getCrimeFromUnhappiness() : -1;
}

int CyCity::getCrimeFromWars() const
{
	return m_pCity ? m_pCity->getCrimeFromWars() : -1;
}

int CyCity::getCrimBonusFactorFromOverflow() const
{
	return m_pCity ? m_pCity->getCrimBonusFactorFromOverflow() : -1;
}
// WTP, ray, Crime and Law - END

// WTP, ray, fix for SailTo - for the City - START
bool CyCity::isEuropeAccessable() const
{
	return m_pCity ? m_pCity->isEuropeAccessable() : false;
}
// WTP, ray, fix for SailTo - for the City - END

CyUnit* CyCity::getUnitWorkingPlot(int iPlotIndex)
{
	return m_pCity ? new CyUnit(m_pCity->getUnitWorkingPlot(static_cast<CityPlotTypes>(iPlotIndex))) : NULL;
}

void CyCity::addPopulationUnit(CyUnit* pUnit, int /*ProfessionTypes*/ eProfession)
{
	if (m_pCity)
	{
		m_pCity->addPopulationUnit(pUnit->getUnit(), (ProfessionTypes) eProfession);
	}
}

bool CyCity::removePopulationUnit(CyUnit* pUnit, bool bDelete, int /*ProfessionTypes*/ eProfession)
{
	return m_pCity ? m_pCity->removePopulationUnit(CREATE_ASSERT_DATA, pUnit->getUnit(), bDelete, (ProfessionTypes) eProfession) : false;
}

bool CyCity::isExport(int /*YieldTypes*/ eYield) const
{
	return m_pCity ? m_pCity->isExport((YieldTypes) eYield) : false;
}

void CyCity::addExport(int /*YieldTypes*/ eYield)
{
	if (m_pCity != NULL)
	{
		m_pCity->addExport((YieldTypes) eYield);
	}
}

void CyCity::removeExport(int /*YieldTypes*/ eYield)
{
	if (m_pCity != NULL)
	{
		m_pCity->removeExport((YieldTypes) eYield);
	}
}

bool CyCity::isImport(int /*YieldTypes*/ eYield) const
{
	return m_pCity ? m_pCity->isImport((YieldTypes) eYield) : false;
}

void CyCity::addImport(int /*YieldTypes*/ eYield)
{
	if (m_pCity != NULL)
	{
		m_pCity->addImport((YieldTypes) eYield);
	}
}

void CyCity::removeImport(int /*YieldTypes*/ eYield)
{
	if (m_pCity != NULL)
	{
		m_pCity->removeImport((YieldTypes) eYield);
	}
}

int CyCity::getMaintainLevel(int /*YieldTypes*/ eYield) const
{
	return m_pCity ? m_pCity->getMaintainLevel((YieldTypes) eYield) : -1;
}
// R&R mod, vetiarvind, max yield import limit - start
int CyCity::getImportsLimit(int /*YieldTypes*/ eYield) const
{
	return m_pCity ? m_pCity->getImportsLimit((YieldTypes) eYield) : -1;
}
// R&R mod, vetiarvind, max yield import limit - end
// transport feeder - start - Nightinggale
bool CyCity::isImportFeeder(int /*YieldTypes*/ eYield) const
{
	return m_pCity ? m_pCity->getImportsMaintain((YieldTypes) eYield) : false;
}

bool CyCity::isAutoImportStopped(int /*YieldTypes*/ eYield) const
{
	return m_pCity ? m_pCity->isAutoImportStopped((YieldTypes) eYield) : false;
}

int CyCity::getAutoMaintainThreshold(int /*YieldTypes*/ eYield) const
{
	return m_pCity ? m_pCity->getAutoMaintainThreshold((YieldTypes) eYield) : 0;
}
// transport feeder - end - Nightinggale

python::tuple CyCity::isOrderWaitingForYield(int /*YieldTypes*/ eYield)
{
	if (m_pCity)
	{
		std::vector< std::pair<OrderTypes, int> > aOrders;
		m_pCity->getOrdersWaitingForYield(aOrders, (YieldTypes) eYield, false, 0);
		if(!aOrders.empty())
		{
			return python::make_tuple(aOrders[0].first, aOrders[0].second);
		}
	}

	return python::make_tuple();
}

// WTP, ray, Center Plot specific Backgrounds - Start
int /*TerrainTypes*/ CyCity::getCenterPlotTerrainType() const
{
	return m_pCity ? m_pCity->getCenterPlotTerrainType() : -1;
}
// WTP, ray, Center Plot specific Backgrounds - END

