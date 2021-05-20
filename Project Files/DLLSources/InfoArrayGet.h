// get types for each enum type

#pragma once

#ifndef INFO_ARRAY_GET_H
#define INFO_ARRAY_GET_H


inline BonusTypes _getBonus(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<BonusTypes>(getWithType(JIT_ARRAY_BONUS, iIndex, iTokenIndex));
}

inline BonusTypes _get(BonusTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return _getBonus(iIndex, iTokenIndex);
}

inline BuildingTypes _getBuilding(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<BuildingTypes>(getWithType(JIT_ARRAY_BUILDING, iIndex, iTokenIndex));
}

inline BuildingTypes get(BuildingTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return _getBuilding(iIndex, iTokenIndex);
}

inline BuildingClassTypes _getBuildingClass(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<BuildingClassTypes>(getWithType(JIT_ARRAY_BUILDING_CLASS, iIndex, iTokenIndex));
}

inline BuildingClassTypes _get(BuildingClassTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return _getBuildingClass(iIndex, iTokenIndex);
}

inline SpecialBuildingTypes _getBuildingSpecial(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<SpecialBuildingTypes>(getWithType(JIT_ARRAY_BUILDING_SPECIAL, iIndex, iTokenIndex));
}

inline SpecialBuildingTypes _get(SpecialBuildingTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return _getBuildingSpecial(iIndex, iTokenIndex);
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

inline CivicTypes _getCivic(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<CivicTypes>(getWithType(JIT_ARRAY_CIVIC, iIndex, iTokenIndex));
}

inline CivicTypes _get(CivicTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return _getCivic(iIndex, iTokenIndex);
}

inline CivicOptionTypes _getCivicOption(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<CivicOptionTypes>(getWithType(JIT_ARRAY_CIVIC_OPTION, iIndex, iTokenIndex));
}

inline CivicOptionTypes _get(CivicOptionTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return _getCivicOption(iIndex, iTokenIndex);
}

inline EraTypes _getEra(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<EraTypes>(getWithType(JIT_ARRAY_ERA, iIndex, iTokenIndex));
}

inline EraTypes _get(EraTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return _getEra(iIndex, iTokenIndex);
}

inline EuropeTypes _getEurope(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<EuropeTypes>(getWithType(JIT_ARRAY_EUROPE, iIndex, iTokenIndex));
}

inline EuropeTypes _get(EuropeTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return _getEurope(iIndex, iTokenIndex);
}

inline FatherTypes _getFather(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<FatherTypes>(getWithType(JIT_ARRAY_FATHER, iIndex, iTokenIndex));
}

inline FatherTypes _get(FatherTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return _getFather(iIndex, iTokenIndex);
}

inline FatherPointTypes _getFatherPoint(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<FatherPointTypes>(getWithType(JIT_ARRAY_FATHER_POINT, iIndex, iTokenIndex));
}

inline FatherPointTypes _get(FatherPointTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return _getFatherPoint(iIndex, iTokenIndex);
}

inline FeatureTypes _getFeature(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<FeatureTypes>(getWithType(JIT_ARRAY_FEATURE, iIndex, iTokenIndex));
}

inline FeatureTypes _get(FeatureTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return _getFeature(iIndex, iTokenIndex);
}

inline GoodyTypes _getGoody(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<GoodyTypes>(getWithType(JIT_ARRAY_GOODY, iIndex, iTokenIndex));
}

inline GoodyTypes _get(GoodyTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return _getGoody(iIndex, iTokenIndex);
}

inline HurryTypes _getHurry(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<HurryTypes>(getWithType(JIT_ARRAY_HURRY, iIndex, iTokenIndex));
}

inline HurryTypes _get(HurryTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return _getHurry(iIndex, iTokenIndex);
}

inline ImprovementTypes _getImprovement(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<ImprovementTypes>(getWithType(JIT_ARRAY_IMPROVEMENT, iIndex, iTokenIndex));
}

inline ImprovementTypes _get(ImprovementTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return _getImprovement(iIndex, iTokenIndex);
}

inline LeaderHeadTypes _getLeaderHead(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<LeaderHeadTypes>(getWithType(JIT_ARRAY_LEADER_HEAD, iIndex, iTokenIndex));
}

inline LeaderHeadTypes _get(LeaderHeadTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return _getLeaderHead(iIndex, iTokenIndex);
}

inline ProfessionTypes _getProfession(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<ProfessionTypes>(getWithType(JIT_ARRAY_PROFESSION, iIndex, iTokenIndex));
}

inline ProfessionTypes _get(ProfessionTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return _getProfession(iIndex, iTokenIndex);
}

inline PromotionTypes _getPromotion(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<PromotionTypes>(getWithType(JIT_ARRAY_PROMOTION, iIndex, iTokenIndex));
}

inline PromotionTypes _get(PromotionTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return _getPromotion(iIndex, iTokenIndex);
}

inline RouteTypes _getRoute(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<RouteTypes>(getWithType(JIT_ARRAY_ROUTE, iIndex, iTokenIndex));
}

inline RouteTypes _get(RouteTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return _getRoute(iIndex, iTokenIndex);
}

inline TerrainTypes _getTerrain(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<TerrainTypes>(getWithType(JIT_ARRAY_TERRAIN, iIndex, iTokenIndex));
}

inline TerrainTypes _get(TerrainTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return _getTerrain(iIndex, iTokenIndex);
}

inline TraitTypes _getTrait(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<TraitTypes>(getWithType(JIT_ARRAY_TRAIT, iIndex, iTokenIndex));
}

inline TraitTypes _get(TraitTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return _getTrait(iIndex, iTokenIndex);
}

inline UnitTypes _getUnit(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<UnitTypes>(getWithType(JIT_ARRAY_UNIT, iIndex, iTokenIndex));
}

inline UnitTypes _get(UnitTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return _getUnit(iIndex, iTokenIndex);
}

inline UnitClassTypes _getUnitClass(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<UnitClassTypes>(getWithType(JIT_ARRAY_UNIT_CLASS, iIndex, iTokenIndex));
}

inline UnitClassTypes _get(UnitClassTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return _getUnitClass(iIndex, iTokenIndex);
}

inline UnitCombatTypes _getUnitCombat(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<UnitCombatTypes>(getWithType(JIT_ARRAY_UNIT_COMBAT, iIndex, iTokenIndex));
}

inline UnitCombatTypes _get(UnitCombatTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return _getUnitCombat(iIndex, iTokenIndex);
}

inline SpecialUnitTypes _getUnitSpecial(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<SpecialUnitTypes>(getWithType(JIT_ARRAY_UNIT_SPECIAL, iIndex, iTokenIndex));
}

inline SpecialUnitTypes _get(SpecialUnitTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return _getUnitSpecial(iIndex, iTokenIndex);
}

inline YieldTypes _getYield(int iIndex, int iTokenIndex = 0) const
{
	return static_cast<YieldTypes>(getWithType(JIT_ARRAY_YIELD, iIndex, iTokenIndex));
}

inline YieldTypes _get(YieldTypes eType, int iIndex, int iTokenIndex = 0) const
{
	return _getYield(iIndex, iTokenIndex);
}

#endif