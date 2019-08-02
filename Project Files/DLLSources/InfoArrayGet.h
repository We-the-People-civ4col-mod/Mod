// get types for each enum type

#pragma once

#ifndef INFO_ARRAY_GET_H
#define INFO_ARRAY_GET_H


inline BonusTypes getBonus(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<BonusTypes>(getWithType(JIT_ARRAY_BONUS, iIndex, iTokenIndex));
}

inline BonusTypes get(BonusTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return getBonus(iIndex, iTokenIndex);
}

inline BuildingTypes getBuilding(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<BuildingTypes>(getWithType(JIT_ARRAY_BUILDING, iIndex, iTokenIndex));
}

inline BuildingTypes get(BuildingTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return getBuilding(iIndex, iTokenIndex);
}

inline BuildingClassTypes getBuildingClass(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<BuildingClassTypes>(getWithType(JIT_ARRAY_BUILDING_CLASS, iIndex, iTokenIndex));
}

inline BuildingClassTypes get(BuildingClassTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return getBuildingClass(iIndex, iTokenIndex);
}

inline SpecialBuildingTypes getBuildingSpecial(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<SpecialBuildingTypes>(getWithType(JIT_ARRAY_BUILDING_SPECIAL, iIndex, iTokenIndex));
}

inline SpecialBuildingTypes get(SpecialBuildingTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return getBuildingSpecial(iIndex, iTokenIndex);
}

/*
inline CivEffectTypes getCivEffect(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<CivEffectTypes>(getWithType(JIT_ARRAY_CIVEFFECT, iIndex, iTokenIndex));
}

inline CivEffectTypes get(CivEffectTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return getCivEffect(iIndex, iTokenIndex);
}
*/

inline CivicTypes getCivic(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<CivicTypes>(getWithType(JIT_ARRAY_CIVIC, iIndex, iTokenIndex));
}

inline CivicTypes get(CivicTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return getCivic(iIndex, iTokenIndex);
}

inline CivicOptionTypes getCivicOption(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<CivicOptionTypes>(getWithType(JIT_ARRAY_CIVIC_OPTION, iIndex, iTokenIndex));
}

inline CivicOptionTypes get(CivicOptionTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return getCivicOption(iIndex, iTokenIndex);
}

inline EraTypes getEra(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<EraTypes>(getWithType(JIT_ARRAY_ERA, iIndex, iTokenIndex));
}

inline EraTypes get(EraTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return getEra(iIndex, iTokenIndex);
}

inline EuropeTypes getEurope(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<EuropeTypes>(getWithType(JIT_ARRAY_EUROPE, iIndex, iTokenIndex));
}

inline EuropeTypes get(EuropeTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return getEurope(iIndex, iTokenIndex);
}

inline FatherTypes getFather(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<FatherTypes>(getWithType(JIT_ARRAY_FATHER, iIndex, iTokenIndex));
}

inline FatherTypes get(FatherTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return getFather(iIndex, iTokenIndex);
}

inline FatherPointTypes getFatherPoint(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<FatherPointTypes>(getWithType(JIT_ARRAY_FATHER_POINT, iIndex, iTokenIndex));
}

inline FatherPointTypes get(FatherPointTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return getFatherPoint(iIndex, iTokenIndex);
}

inline FeatureTypes getFeature(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<FeatureTypes>(getWithType(JIT_ARRAY_FEATURE, iIndex, iTokenIndex));
}

inline FeatureTypes get(FeatureTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return getFeature(iIndex, iTokenIndex);
}

inline GoodyTypes getGoody(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<GoodyTypes>(getWithType(JIT_ARRAY_GOODY, iIndex, iTokenIndex));
}

inline GoodyTypes get(GoodyTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return getGoody(iIndex, iTokenIndex);
}

inline HurryTypes getHurry(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<HurryTypes>(getWithType(JIT_ARRAY_HURRY, iIndex, iTokenIndex));
}

inline HurryTypes get(HurryTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return getHurry(iIndex, iTokenIndex);
}

inline ImprovementTypes getImprovement(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<ImprovementTypes>(getWithType(JIT_ARRAY_IMPROVEMENT, iIndex, iTokenIndex));
}

inline ImprovementTypes get(ImprovementTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return getImprovement(iIndex, iTokenIndex);
}

inline LeaderHeadTypes getLeaderHead(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<LeaderHeadTypes>(getWithType(JIT_ARRAY_LEADER_HEAD, iIndex, iTokenIndex));
}

inline LeaderHeadTypes get(LeaderHeadTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return getLeaderHead(iIndex, iTokenIndex);
}

inline ProfessionTypes getProfession(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<ProfessionTypes>(getWithType(JIT_ARRAY_PROFESSION, iIndex, iTokenIndex));
}

inline ProfessionTypes get(ProfessionTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return getProfession(iIndex, iTokenIndex);
}

inline PromotionTypes getPromotion(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<PromotionTypes>(getWithType(JIT_ARRAY_PROMOTION, iIndex, iTokenIndex));
}

inline PromotionTypes get(PromotionTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return getPromotion(iIndex, iTokenIndex);
}

inline RouteTypes getRoute(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<RouteTypes>(getWithType(JIT_ARRAY_ROUTE, iIndex, iTokenIndex));
}

inline RouteTypes get(RouteTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return getRoute(iIndex, iTokenIndex);
}

inline TerrainTypes getTerrain(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<TerrainTypes>(getWithType(JIT_ARRAY_TERRAIN, iIndex, iTokenIndex));
}

inline TerrainTypes get(TerrainTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return getTerrain(iIndex, iTokenIndex);
}

inline TraitTypes getTrait(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<TraitTypes>(getWithType(JIT_ARRAY_TRAIT, iIndex, iTokenIndex));
}

inline TraitTypes get(TraitTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return getTrait(iIndex, iTokenIndex);
}

inline UnitTypes getUnit(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<UnitTypes>(getWithType(JIT_ARRAY_UNIT, iIndex, iTokenIndex));
}

inline UnitTypes get(UnitTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return getUnit(iIndex, iTokenIndex);
}

inline UnitClassTypes getUnitClass(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<UnitClassTypes>(getWithType(JIT_ARRAY_UNIT_CLASS, iIndex, iTokenIndex));
}

inline UnitClassTypes get(UnitClassTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return getUnitClass(iIndex, iTokenIndex);
}

inline UnitCombatTypes getUnitCombat(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<UnitCombatTypes>(getWithType(JIT_ARRAY_UNIT_COMBAT, iIndex, iTokenIndex));
}

inline UnitCombatTypes get(UnitCombatTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return getUnitCombat(iIndex, iTokenIndex);
}

inline SpecialUnitTypes getUnitSpecial(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<SpecialUnitTypes>(getWithType(JIT_ARRAY_UNIT_SPECIAL, iIndex, iTokenIndex));
}

inline SpecialUnitTypes get(SpecialUnitTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return getUnitSpecial(iIndex, iTokenIndex);
}

inline YieldTypes getYield(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<YieldTypes>(getWithType(JIT_ARRAY_YIELD, iIndex, iTokenIndex));
}

inline YieldTypes get(YieldTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return getYield(iIndex, iTokenIndex);
}

#endif