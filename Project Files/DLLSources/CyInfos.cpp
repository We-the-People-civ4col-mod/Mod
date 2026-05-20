#include "CvGameCoreDLL.h"
#include "CyInfos.h"


int CyActionInfo::getAutomateType() const
{
	return CvActionInfo::getAutomateType();
}

int CyActionInfo::getInterfaceModeType() const
{
	return CvActionInfo::getInterfaceModeType();
}

int CyActionInfo::getMissionType() const
{
	return CvActionInfo::getMissionType();
}

int CyActionInfo::getCommandType() const
{
	return CvActionInfo::getCommandType();
}

int CyActionInfo::getControlType() const
{
	return CvActionInfo::getControlType();
}

int CyCivilizationInfo::getDefaultPlayerColor() const
{
	return CvCivilizationInfo::getDefaultPlayerColor();
}

int CyCivilizationInfo::getArtStyleType() const
{
	return CvCivilizationInfo::getArtStyleType();
}

int CyCivilizationInfo::getUnitArtStyleType() const
{
	return CvCivilizationInfo::getUnitArtStyleType();
}

int CyCivilizationInfo::getDefaultProfession() const
{
	return CvCivilizationInfo::getDefaultProfession();
}

int CyCivilizationInfo::getNumCityNames() const
{
	return CvCivilizationInfo::getNumCityNames();
}

int CyCivilizationInfo::getNumLeaders() const
{
	return CvCivilizationInfo::getNumLeaders();
}

int CyCivilizationInfo::getSelectionSoundScriptId() const
{
	return CvCivilizationInfo::getSelectionSoundScriptId();
}

int CyCivilizationInfo::getActionSoundScriptId() const
{
	return CvCivilizationInfo::getActionSoundScriptId();
}

int CyCivilizationInfo::getAdvancedStartPoints() const
{
	return CvCivilizationInfo::getAdvancedStartPoints();
}

int CyCivilizationInfo::getAreaMultiplier() const
{
	return CvCivilizationInfo::getAreaMultiplier();
}

int CyCivilizationInfo::getDensityMultiplier() const
{
	return CvCivilizationInfo::getDensityMultiplier();
}

int CyCivilizationInfo::getTreasure() const
{
	return CvCivilizationInfo::getTreasure();
}

int CyCivilizationInfo::getFavoredTerrain() const
{
	return CvCivilizationInfo::getFavoredTerrain();
}

int CyCivilizationInfo::getCapturedCityUnitClass() const
{
	return CvCivilizationInfo::getCapturedCityUnitClass();
}

int CyCivilizationInfo::getCivilizationBuildings(int iBuildingClass) const
{
	return CvCivilizationInfo::getCivilizationBuildings(static_cast<BuildingClassTypes>(iBuildingClass));
}

int CyCivilizationInfo::getCivilizationUnits(int iUnitClass) const
{
	return CvCivilizationInfo::getCivilizationUnits(static_cast<UnitClassTypes>(iUnitClass));
}

int CyCivilizationInfo::getNumCivilizationFreeUnits() const
{
	return CvCivilizationInfo::getNumCivilizationFreeUnits();
}

int CyCivilizationInfo::getCivilizationFreeUnitsClass(int index) const
{
	return CvCivilizationInfo::getCivilizationFreeUnitsClass(index);
}

int CyCivilizationInfo::getCivilizationFreeUnitsProfession(int index) const
{
	return CvCivilizationInfo::getCivilizationFreeUnitsProfession(index);
}

int CyCivilizationInfo::getCivilizationInitialCivics(int iCivicOption) const
{
	return CvCivilizationInfo::getCivilizationInitialCivics(static_cast<CivicOptionTypes>(iCivicOption));
}

int CyCivilizationInfo::getFreeYields(int iYield) const
{
	return CvCivilizationInfo::getFreeYields(static_cast<YieldTypes>(iYield));
}

int CyCivilizationInfo::getTeachUnitClassWeight(int iUnitClass) const
{
	return CvCivilizationInfo::getTeachUnitClassWeight(static_cast<UnitClassTypes>(iUnitClass));
}

bool CyCivilizationInfo::isLeaders(int iLeaderHead) const
{
	return CvCivilizationInfo::isLeaders(static_cast<LeaderHeadTypes>(iLeaderHead));
}

bool CyCivilizationInfo::isCivilizationFreeBuildingClass(int iBuildingClass) const
{
	return CvCivilizationInfo::isCivilizationFreeBuildingClass(static_cast<BuildingClassTypes>(iBuildingClass));
}

bool CyCivilizationInfo::isValidProfession(int iProfession) const
{
	return CvCivilizationInfo::isValidProfession(static_cast<ProfessionTypes>(iProfession));
}

bool CyCivilizationInfo::hasTrait(int iTrait) const
{
	return CvCivilizationInfo::hasTrait(static_cast<TraitTypes>(iTrait));
}

int CyEventInfo::getUnitClass() const
{
	return CvEventInfo::getUnitClass();
}

int CyEventTriggerInfo::getNumUnits() const
{
	return unitTriggers().NumUnits;
}

int CyEventTriggerInfo::getNumUnitsGlobal() const
{
	return unitTriggers().NumUnitsGlobal;
}

int CyEventTriggerInfo::getUnitDamagedWeight() const
{
	return unitTriggers().DamagedWeight;
}

int CyEventTriggerInfo::getUnitDistanceWeight() const
{
	return unitTriggers().DistanceWeight;
}

int CyEventTriggerInfo::getUnitExperienceWeight() const
{
	return unitTriggers().ExperienceWeight;
}

bool CyEventTriggerInfo::isUnitsOnPlot() const
{
	return unitTriggers().OnPlot;
}

int CyFeatureInfo::getYieldChange(int iYield) const
{
	return CvFeatureInfo::getYieldChange(static_cast<YieldTypes>(iYield));
}

int CyFeatureInfo::getRiverYieldIncrease(int iYield) const
{
	return CvFeatureInfo::getRiverYieldIncrease(static_cast<YieldTypes>(iYield));
}

bool CyFeatureInfo::isTerrain(int iTerrain) const
{
	return CvFeatureInfo::isTerrain(static_cast<TerrainTypes>(iTerrain));
}

int CyGoodyInfo::getUnitClassType() const
{
	return CvGoodyInfo::getUnitClassType();
}

int CyLeaderHeadInfo::getContactRand(int iContact) const
{
	return CvLeaderHeadInfo::getContactRand(static_cast<ContactTypes>(iContact));
}

int CyLeaderHeadInfo::getContactDelay(int iContact) const
{
	return CvLeaderHeadInfo::getContactDelay(static_cast<ContactTypes>(iContact));
}

int CyLeaderHeadInfo::getMemoryDecayRand(int iMemory) const
{
	return CvLeaderHeadInfo::getMemoryDecayRand(static_cast<MemoryTypes>(iMemory));
}

int CyLeaderHeadInfo::getMemoryAttitudePercent(int iMemory) const
{
	return CvLeaderHeadInfo::getMemoryAttitudePercent(static_cast<MemoryTypes>(iMemory));
}

int CyLeaderHeadInfo::getNoWarAttitudeProb(int iAttitude) const
{
	return CvLeaderHeadInfo::getNoWarAttitudeProb(static_cast<AttitudeTypes>(iAttitude));
}

int CyLeaderHeadInfo::getUnitAIWeightModifier(int iUnitAI) const
{
	return CvLeaderHeadInfo::getUnitAIWeightModifier(static_cast<UnitAITypes>(iUnitAI));
}

int CyLeaderHeadInfo::getImprovementWeightModifier(int iImprovement) const
{
	return CvLeaderHeadInfo::getImprovementWeightModifier(static_cast<ImprovementTypes>(iImprovement));
}

bool CyLeaderHeadInfo::hasTrait(int iTrait) const
{
	return CvLeaderHeadInfo::hasTrait(static_cast<TraitTypes>(iTrait));
}

int CyProfessionInfo::getPediaUnitGraphics() const
{
	return CvProfessionInfo::getPediaUnitGraphics();
}

int CyTerrainInfo::getRiverYieldIncrease(int iYield) const
{
	return CvTerrainInfo::getRiverYieldIncrease(static_cast<YieldTypes>(iYield));
}

int CyTerrainInfo::getYield(int iYield) const
{
	return CvTerrainInfo::getYield(static_cast<YieldTypes>(iYield));
}

int CyUnitInfo::getDefaultProfession() const
{
	return CvUnitInfo::getDefaultProfession();
}

int CyUnitInfo::getLeaderPromotion() const
{
	return CvUnitInfo::getLeaderPromotion();
}

int CyUnitInfo::getUnitClassType() const
{
	return CvUnitInfo::getUnitClassType();
}

const CvArtInfoUnit* CyUnitInfo::getUnitArtStylesArtInfo(int index, int iProfession, int iStyle) const
{
	return CvUnitInfo::getUnitArtStylesArtInfo(index, static_cast<ProfessionTypes>(iProfession), static_cast<UnitArtStyleTypes>(iStyle));
}

int CyYieldInfo::getBuyPriceLow() const
{
	return price(TRADE_LOCATION_EUROPE).buyLow;
}
int CyYieldInfo::getBuyPriceHigh() const
{
	return price(TRADE_LOCATION_EUROPE).buyHigh;
}
int CyYieldInfo::getSellPriceDifference() const
{
	return price(TRADE_LOCATION_EUROPE).sellPriceDifference;
}
int CyYieldInfo::getAfricaBuyPriceLow() const
{
	return price(TRADE_LOCATION_AFRICA).buyLow;
}
int CyYieldInfo::getAfricaBuyPriceHigh() const
{
	return price(TRADE_LOCATION_AFRICA).buyHigh;
}
int CyYieldInfo::getAfricaSellPriceDifference() const
{
	return price(TRADE_LOCATION_AFRICA).sellPriceDifference;
}
int CyYieldInfo::getPortRoyalBuyPriceLow() const
{
	return price(TRADE_LOCATION_PORT_ROYAL).buyLow;
}
int CyYieldInfo::getPortRoyalBuyPriceHigh() const
{
	return price(TRADE_LOCATION_PORT_ROYAL).buyHigh;
}
int CyYieldInfo::getPortRoyalSellPriceDifference() const
{
	return price(TRADE_LOCATION_PORT_ROYAL).sellPriceDifference;
}

int CyYieldInfo::getUnitClass() const
{
	return CvYieldInfo::getUnitClass();
}

const char* CyAchieveInfo::getPictureFileName() const
{
	return CvAchieveInfo::getPictureFileName();
}

const wchar* CyAchieveInfo::getHeaderTextKey() const
{
	return CvAchieveInfo::getHeaderTextKey();
}

const wchar* CyAchieveInfo::getBodyTextKey() const
{
	return CvAchieveInfo::getBodyTextKey();
}
