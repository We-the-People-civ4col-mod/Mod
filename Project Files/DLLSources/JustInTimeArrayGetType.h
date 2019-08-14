#include "CvGameCoreDLL.h"

// Converts enum types to JITarrayTypes

// Intended usage: functions, which needs the JITarrayTypes for a template argument


static inline JITarrayTypes getJITarrayType(BonusTypes var)
{
	return JIT_ARRAY_BONUS;
}

static inline JITarrayTypes getJITarrayType(BuildTypes var)
{
	return JIT_ARRAY_BUILD;
}

static inline JITarrayTypes getJITarrayType(BuildingTypes var)
{
	return JIT_ARRAY_BUILDING;
}

static inline JITarrayTypes getJITarrayType(BuildingClassTypes var)
{
	return JIT_ARRAY_BUILDING_CLASS;
}

static inline JITarrayTypes getJITarrayType(SpecialBuildingTypes var)
{
	return JIT_ARRAY_BUILDING_SPECIAL;
}

static inline JITarrayTypes getJITarrayType(CivEffectTypes var)
{
	return JIT_ARRAY_CIV_EFFECT;
}

static inline JITarrayTypes getJITarrayType(CivicTypes var)
{
	return JIT_ARRAY_CIVIC;
}

static inline JITarrayTypes getJITarrayType(CivicOptionTypes var)
{
	return JIT_ARRAY_CIVIC_OPTION;
}

static inline JITarrayTypes getJITarrayType(ColorTypes var)
{
	return JIT_ARRAY_COLOR;
}

static inline JITarrayTypes getJITarrayType(CultureLevelTypes var)
{
	return JIT_ARRAY_CULTURE;
}

static inline JITarrayTypes getJITarrayType(EraTypes var)
{
	return JIT_ARRAY_ERA;
}

static inline JITarrayTypes getJITarrayType(EmphasizeTypes var)
{
	return JIT_ARRAY_EMPHASIZE;
}

static inline JITarrayTypes getJITarrayType(EuropeTypes var)
{
	return JIT_ARRAY_EUROPE;
}

static inline JITarrayTypes getJITarrayType(EventTriggerTypes var)
{
	return JIT_ARRAY_EVENT_TRIGGER;
}

static inline JITarrayTypes getJITarrayType(FatherTypes var)
{
	return JIT_ARRAY_FATHER;
}

static inline JITarrayTypes getJITarrayType(FatherPointTypes var)
{
	return JIT_ARRAY_FATHER_POINT;
}

static inline JITarrayTypes getJITarrayType(FeatureTypes var)
{
	return JIT_ARRAY_FEATURE;
}

static inline JITarrayTypes getJITarrayType(GoodyTypes var)
{
	return JIT_ARRAY_GOODY;
}

static inline JITarrayTypes getJITarrayType(HandicapTypes var)
{
	return JIT_ARRAY_HANDICAP;
}

static inline JITarrayTypes getJITarrayType(HurryTypes var)
{
	return JIT_ARRAY_HURRY;
}

static inline JITarrayTypes getJITarrayType(ImprovementTypes var)
{
	return JIT_ARRAY_IMPROVEMENT;
}

static inline JITarrayTypes getJITarrayType(LeaderHeadTypes var)
{
	return JIT_ARRAY_LEADER_HEAD;
}

static inline JITarrayTypes getJITarrayType(ProfessionTypes var)
{
	return JIT_ARRAY_PROFESSION;
}

static inline JITarrayTypes getJITarrayType(PromotionTypes var)
{
	return JIT_ARRAY_PROMOTION;
}

static inline JITarrayTypes getJITarrayType(RouteTypes var)
{
	return JIT_ARRAY_ROUTE;
}

static inline JITarrayTypes getJITarrayType(TerrainTypes var)
{
	return JIT_ARRAY_TERRAIN;
}

static inline JITarrayTypes getJITarrayType(TraitTypes var)
{
	return JIT_ARRAY_TRAIT;
}

static inline JITarrayTypes getJITarrayType(UnitTypes var)
{
	return JIT_ARRAY_UNIT;
}

static inline JITarrayTypes getJITarrayType(UnitAITypes var)
{
	return JIT_ARRAY_UNIT_AI;
}

static inline JITarrayTypes getJITarrayType(UnitClassTypes var)
{
	return JIT_ARRAY_UNIT_CLASS;
}

static inline JITarrayTypes getJITarrayType(UnitCombatTypes var)
{
	return JIT_ARRAY_UNIT_COMBAT;
}

static inline JITarrayTypes getJITarrayType(SpecialUnitTypes var)
{
	return JIT_ARRAY_UNIT_SPECIAL;
}

static inline JITarrayTypes getJITarrayType(YieldTypes var)
{
	return JIT_ARRAY_YIELD;
}

static inline JITarrayTypes getJITarrayType(YieldCategoryTypes var)
{
	return JIT_ARRAY_YIELD_CATEGORY;
}
