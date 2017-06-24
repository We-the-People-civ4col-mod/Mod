//
// Python wrapper class for CvTeam
// updated 6-5
//
#include "CvGameCoreDLL.h"
#include "CyTeam.h"
#include "CyArea.h"
#include "CvTeam.h"
#include "CyUnit.h"
CyTeam::CyTeam() : m_pTeam(NULL)
{
}
CyTeam::CyTeam(CvTeam* pTeam) : m_pTeam(pTeam)
{
}
void CyTeam::addTeam(int /*TeamTypes*/ eTeam)
{
	if (m_pTeam)
		m_pTeam->addTeam((TeamTypes)eTeam);
}
bool CyTeam::canChangeWarPeace(int /*TeamTypes*/ eTeam)
{
	return m_pTeam ? m_pTeam->canChangeWarPeace((TeamTypes)eTeam) : false;
}
bool CyTeam::canDeclareWar(int /*TeamTypes*/ eTeam)
{
	return m_pTeam ? m_pTeam->canDeclareWar((TeamTypes)eTeam) : false;
}
void CyTeam::declareWar(int /*TeamTypes*/ eTeam, bool bNewDiplo, int /*WarPlanTypes*/ eWarPlan)
{
	if (m_pTeam)
		m_pTeam->declareWar((TeamTypes)eTeam, bNewDiplo, (WarPlanTypes)eWarPlan);
}
void CyTeam::makePeace(int /*TeamTypes*/ eTeam)
{
	if (m_pTeam)
		m_pTeam->makePeace((TeamTypes)eTeam);
}
bool CyTeam::canContact(int /*TeamTypes*/ eTeam)
{
	return m_pTeam ? m_pTeam->canContact((TeamTypes)eTeam) : false;
}
void CyTeam::meet(int /*TeamTypes*/ eTeam, bool bNewDiplo)
{
	if (m_pTeam)
		m_pTeam->meet((TeamTypes)eTeam, bNewDiplo);
}
void CyTeam::signOpenBorders(int /*TeamTypes*/ eTeam)
{
	if (m_pTeam)
		m_pTeam->signOpenBorders((TeamTypes)eTeam);
}
void CyTeam::signDefensivePact(int /*TeamTypes*/ eTeam)
{
	if (m_pTeam)
		m_pTeam->signDefensivePact((TeamTypes)eTeam);
}
int CyTeam::getAssets()
{
	return m_pTeam ? m_pTeam->getAssets() : -1;
}
int CyTeam::getPower()
{
	return m_pTeam ? m_pTeam->getPower() : -1;
}
int CyTeam::getDefensivePower()
{
	return m_pTeam ? m_pTeam->getDefensivePower() : -1;
}
int CyTeam::getAtWarCount()
{
	return m_pTeam ? m_pTeam->getAtWarCount() : -1;
}
int CyTeam::getWarPlanCount(int /*WarPlanTypes*/ eWarPlan)
{
	return m_pTeam ? m_pTeam->getWarPlanCount((WarPlanTypes) eWarPlan) : -1;
}
int CyTeam::getAnyWarPlanCount()
{
	return m_pTeam ? m_pTeam->getAnyWarPlanCount() : -1;
}
int CyTeam::getChosenWarCount()
{
	return m_pTeam ? m_pTeam->getChosenWarCount() : -1;
}
int CyTeam::getHasMetCivCount()
{
	return m_pTeam ? m_pTeam->getHasMetCivCount() : -1;
}
bool CyTeam::hasMetHuman()
{
	return m_pTeam ? m_pTeam->hasMetHuman() : false;
}
int CyTeam::getDefensivePactCount()
{
	return m_pTeam ? m_pTeam->getDefensivePactCount() : -1;
}
int CyTeam::getUnitClassMaking(int /*UnitClassTypes*/ eUnitClass)
{
	return m_pTeam ? m_pTeam->getUnitClassMaking((UnitClassTypes)eUnitClass) : -1;
}
int CyTeam::getUnitClassCountPlusMaking(int /*UnitClassTypes*/ eUnitClass)
{
	return m_pTeam ? m_pTeam->getUnitClassCountPlusMaking((UnitClassTypes)eUnitClass) : -1;
}
int CyTeam::getBuildingClassMaking(int /*BuildingClassTypes*/ eBuildingClass)
{
	return m_pTeam ? m_pTeam->getBuildingClassMaking((BuildingClassTypes)eBuildingClass) : -1;
}
int CyTeam::getBuildingClassCountPlusMaking(int /*BuildingClassTypes*/ eBuildingClass)
{
	return m_pTeam ? m_pTeam->getBuildingClassCountPlusMaking((BuildingClassTypes)eBuildingClass) : -1;
}
int CyTeam::countTotalCulture()
{
	return m_pTeam ? m_pTeam->countTotalCulture() : -1;
}
int CyTeam::countNumUnitsByArea(CyArea* pArea)
{
	return m_pTeam ? m_pTeam->countNumUnitsByArea(pArea->getArea()) : -1;
}
int CyTeam::countNumCitiesByArea(CyArea* pArea)
{
	return m_pTeam ? m_pTeam->countNumCitiesByArea(pArea->getArea()) : -1;
}
int CyTeam::countTotalPopulationByArea(CyArea* pArea)
{
	return m_pTeam ? m_pTeam->countTotalPopulationByArea(pArea->getArea()) : -1;
}
int CyTeam::countPowerByArea(CyArea* pArea)
{
	return m_pTeam ? m_pTeam->countPowerByArea(pArea->getArea()) : -1;
}
int CyTeam::countEnemyPowerByArea(CyArea* pArea)
{
	return m_pTeam ? m_pTeam->countEnemyPowerByArea(pArea->getArea()) : -1;
}
int CyTeam::countNumAIUnitsByArea(CyArea* pArea, int /*UnitAITypes*/ eUnitAI)
{
	return m_pTeam ? m_pTeam->countNumAIUnitsByArea(pArea->getArea(), (UnitAITypes) eUnitAI) : -1;
}
int CyTeam::countEnemyDangerByArea(CyArea* pArea)
{
	return m_pTeam ? m_pTeam->countEnemyDangerByArea(pArea->getArea()) : -1;
}
int CyTeam::getFatherPointCost(int /*FatherTypes*/ eFather, int /*FatherPointTypes*/ ePointType)
{
	return m_pTeam ? m_pTeam->getFatherPointCost((FatherTypes) eFather, (FatherPointTypes) ePointType) : -1;
}
int CyTeam::getFatherPoints(int /*FatherPointTypes*/ ePointType)
{
	return m_pTeam ? m_pTeam->getFatherPoints((FatherPointTypes) ePointType) : -1;
}

// TAC - Python Export - koma13 - START
bool CyTeam::isFatherIgnore(int /*FatherTypes*/ eFather) const
{
	return m_pTeam ? m_pTeam->isFatherIgnore((FatherTypes) eFather) : false;
}
// TAC - Python Export - koma13 - END

bool CyTeam::canConvinceFather(int /*FatherTypes*/ eFather) const
{
	return m_pTeam ? m_pTeam->canConvinceFather((FatherTypes) eFather) : false;
}
void CyTeam::changeFatherPoints(int /*FatherPointTypes*/ ePointType, int iChange)
{
	if (m_pTeam)
		m_pTeam->changeFatherPoints((FatherPointTypes) ePointType, iChange);
}
bool CyTeam::isHuman()
{
	return m_pTeam ? m_pTeam->isHuman() : false;
}
int /*PlayerTypes*/ CyTeam::getLeaderID()
{
	return m_pTeam ? m_pTeam->getLeaderID() : -1;
}
int /*HandicapTypes*/ CyTeam::getHandicapType()
{
	return m_pTeam ? m_pTeam->getHandicapType() : -1;
}
std::wstring CyTeam::getName()
{
	return m_pTeam ? m_pTeam->getName() : L"";
}
int CyTeam::getNumMembers()
{
	return m_pTeam ? m_pTeam->getNumMembers() : -1;
}
bool CyTeam::isAlive()
{
	return m_pTeam ? m_pTeam->isAlive() : false;
}
bool CyTeam::isEverAlive()
{
	return m_pTeam ? m_pTeam->isEverAlive() : false;
}
int CyTeam::getNumCities()
{
	return m_pTeam ? m_pTeam->getNumCities() : -1;
}
int CyTeam::getTotalPopulation()
{
	return m_pTeam ? m_pTeam->getTotalPopulation() : -1;
}
int CyTeam::getTotalLand()
{
	return m_pTeam ? m_pTeam->getTotalLand() : -1;
}
int CyTeam::getMapTradingCount()
{
	return m_pTeam ? m_pTeam->getMapTradingCount() : -1;
}
bool CyTeam::isMapTrading()
{
	return m_pTeam ? m_pTeam->isMapTrading() : false;
}
void CyTeam::changeMapTradingCount(int iChange)
{
	if (m_pTeam)
		m_pTeam->changeMapTradingCount(iChange);
}
int CyTeam::getGoldTradingCount()
{
	return m_pTeam ? m_pTeam->getGoldTradingCount() : -1;
}
bool CyTeam::isGoldTrading()
{
	return m_pTeam ? m_pTeam->isGoldTrading() : false;
}
void CyTeam::changeGoldTradingCount(int iChange)
{
	if (m_pTeam)
		m_pTeam->changeGoldTradingCount(iChange);
}
int CyTeam::getOpenBordersTradingCount()
{
	return m_pTeam ? m_pTeam->getOpenBordersTradingCount() : -1;
}
bool CyTeam::isOpenBordersTrading()
{
	return m_pTeam ? m_pTeam->isOpenBordersTrading() : false;
}
void CyTeam::changeOpenBordersTradingCount(int iChange)
{
	if (m_pTeam)
		m_pTeam->changeOpenBordersTradingCount(iChange);
}
int CyTeam::getDefensivePactTradingCount()
{
	return m_pTeam ? m_pTeam->getDefensivePactTradingCount() : -1;
}
bool CyTeam::isDefensivePactTrading()
{
	return m_pTeam ? m_pTeam->isDefensivePactTrading() : false;
}
void CyTeam::changeDefensivePactTradingCount(int iChange)
{
	if (m_pTeam)
		m_pTeam->changeDefensivePactTradingCount(iChange);
}
int CyTeam::getPermanentAllianceTradingCount()
{
	return m_pTeam ? m_pTeam->getPermanentAllianceTradingCount() : -1;
}
bool CyTeam::isPermanentAllianceTrading()
{
	return m_pTeam ? m_pTeam->isPermanentAllianceTrading() : false;
}
void CyTeam::changePermanentAllianceTradingCount(int iChange)
{
	if (m_pTeam)
		m_pTeam->changePermanentAllianceTradingCount(iChange);
}
bool CyTeam::isMapCentering()
{
	return m_pTeam ? m_pTeam->isMapCentering() : false;
}
void CyTeam::setMapCentering(bool bNewValue)
{
	if (m_pTeam)
		m_pTeam->setMapCentering(bNewValue);
}
int CyTeam::getID()
{
	return m_pTeam ? m_pTeam->getID() : -1;
}
bool CyTeam::isHasMet(int /*TeamTypes*/ eIndex)
{
	return m_pTeam ? m_pTeam->isHasMet((TeamTypes)eIndex) : false;
}
bool CyTeam::isAtWar(int /*TeamTypes*/ iIndex)
{
	if (iIndex == NO_TEAM) return false;
	return m_pTeam ? m_pTeam->isAtWar((TeamTypes)iIndex) : false;
}
bool CyTeam::isPermanentWarPeace(int /*TeamTypes*/ eIndex)
{
	return m_pTeam ? m_pTeam->isPermanentWarPeace((TeamTypes)eIndex) : false;
}
void CyTeam::setPermanentWarPeace(int /*TeamTypes*/ eIndex, bool bNewValue)
{
	if (m_pTeam)
		m_pTeam->setPermanentWarPeace((TeamTypes)eIndex, bNewValue);
}
bool CyTeam::isOpenBorders(int /*TeamTypes*/ eIndex)
{
	return m_pTeam ? m_pTeam->isOpenBorders((TeamTypes)eIndex) : false;
}
bool CyTeam::isForcePeace(int /*TeamTypes*/ eIndex)
{
	return m_pTeam ? m_pTeam->isForcePeace((TeamTypes)eIndex) : false;
}
bool CyTeam::isDefensivePact(int /*TeamTypes*/ eIndex)
{
	return m_pTeam ? m_pTeam->isDefensivePact((TeamTypes)eIndex) : false;
}
int CyTeam::getUnitClassCount(int /*UnitClassTypes*/ eIndex)
{
	return m_pTeam ? m_pTeam->getUnitClassCount((UnitClassTypes)eIndex) : -1;
}
int CyTeam::getBuildingClassCount(int /*BuildingClassTypes*/ eIndex)
{
	return m_pTeam ? m_pTeam->getBuildingClassCount((BuildingClassTypes)eIndex) : -1;
}
bool CyTeam::AI_shareWar(int /*TeamTypes*/ eTeam)
{
	return m_pTeam ? m_pTeam->AI_shareWar((TeamTypes)eTeam) : false;
}
void CyTeam::AI_setWarPlan(int /*TeamTypes*/ eIndex, int /*WarPlanTypes*/ eNewValue)
{
	if (m_pTeam)
	{
		m_pTeam->AI_setWarPlan((TeamTypes)eIndex, (WarPlanTypes)eNewValue);
	}
}
int CyTeam::AI_getAtWarCounter(int /*TeamTypes*/ eTeam) const
{
	return m_pTeam ? m_pTeam->AI_getAtWarCounter((TeamTypes)eTeam) : -1;
}
int CyTeam::AI_getAtPeaceCounter(int /*TeamTypes*/ eTeam) const
{
	return m_pTeam ? m_pTeam->AI_getAtPeaceCounter((TeamTypes)eTeam) : -1;
}
int CyTeam::AI_getWarSuccess(int /*TeamTypes*/ eIndex) const
{
	return m_pTeam ? m_pTeam->AI_getWarSuccess((TeamTypes)eIndex) : -1;
}
int CyTeam::getRebelPercent() const
{
	return m_pTeam ? m_pTeam->getRebelPercent() : -1;
}
void CyTeam::doRevolution()
{
	if (m_pTeam)
	{
		m_pTeam->doRevolution();
	}
}
bool CyTeam::canDoRevolution() const
{
	return m_pTeam ? m_pTeam->canDoRevolution() : false;
}
bool CyTeam::isParentOf(int /*TeamTypes*/ eChildTeam)
{
	return m_pTeam ? m_pTeam->isParentOf((TeamTypes) eChildTeam) : false;
}

// PatchMod: Victorys START
int CyTeam::getTotalProductionRate()
{
	return m_pTeam ? m_pTeam->getTotalProductionRate() : 0;
}
// PatchMod: Victorys END
