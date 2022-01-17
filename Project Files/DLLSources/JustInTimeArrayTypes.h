#ifndef JUST_IN_TIME_ARRAY_TYPES_H
#define JUST_IN_TIME_ARRAY_TYPES_H
#pragma once

#include "JustInTimeArray.h"

template<class T>
class BonusArray: public JustInTimeArray<T>
{
public:
	BonusArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_BONUS, eDefault){};
};

template<class T>
class BuildArray: public JustInTimeArray<T>
{
public:
	BuildArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_BUILD, eDefault){};
};

template<class T>
class BuildingArray: public JustInTimeArray<T>
{
public:
	BuildingArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_BUILDING, eDefault){};
};

template<class T>
class BuildingClassArray: public JustInTimeArray<T>
{
public:
	BuildingClassArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_BUILDINGCLASS, eDefault){};
};

template<class T>
class BuildingSpecialArray: public JustInTimeArray<T>
{
public:
	BuildingSpecialArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_BUILDING_SPECIAL, eDefault){};
};

template<class T>
class CensureArray: public JustInTimeArray<T>
{
public:
	CensureArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_CENSURE, eDefault){};
};

template<class T>
class CityTypeArray: public JustInTimeArray<T>
{
public:
	CityTypeArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_CITY_TYPES, eDefault){};
};

template<class T>
class CivCategoryArray : public JustInTimeArray<T>
{
public:
	CivCategoryArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_CIV_CATEGORY, eDefault) {};
};

template<class T>
class CivEffectArray: public JustInTimeArray<T>
{
public:
	CivEffectArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_CIV_EFFECT, eDefault){};
};

template<class T>
class CivicArray: public JustInTimeArray<T>
{
public:
	CivicArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_CIVIC, eDefault){};
};

template<class T>
class CivicOptionArray: public JustInTimeArray<T>
{
public:
	CivicOptionArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_CIVIC_OPTION, eDefault){};
};

template<class T>
class DomainArray: public JustInTimeArray<T>
{
public:
	DomainArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_DOMAIN, eDefault){};
};

template<class T>
class EuropeArray: public JustInTimeArray<T>
{
public:
	EuropeArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_EUROPE, eDefault){};
};

template<class T>
class EventCivEffectArray: public JustInTimeArray<T>
{
public:
	EventCivEffectArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_EVENT_CIVEFFECT, eDefault){};
};

template<class T>
class FatherArray: public JustInTimeArray<T>
{
public:
	FatherArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_FATHER, eDefault){};
};

template<class T>
class FatherPointArray: public JustInTimeArray<T>
{
public:
	FatherPointArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_FATHER_POINT, eDefault){};
};

template<class T>
class FeatureArray: public JustInTimeArray<T>
{
public:
	FeatureArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_FEATURE, eDefault){};
};

template<class T>
class GoodyArray: public JustInTimeArray<T>
{
public:
	GoodyArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_GOODY, eDefault){};
};

template<class T>
class HurryArray: public JustInTimeArray<T>
{
public:
	HurryArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_HURRY, eDefault){};
};

template<class T>
class ImprovementArray: public JustInTimeArray<T>
{
public:
	ImprovementArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_IMPROVEMENT, eDefault){};
};

template<class T>
class PerkArray: public JustInTimeArray<T>
{
public:
	PerkArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_PERK, eDefault){};
};

template<class T>
class ProfessionArray: public JustInTimeArray<T>
{
public:
	ProfessionArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_PROFESSION, eDefault){};
};

template<class T>
class PromotionArray: public JustInTimeArray<T>
{
public:
	PromotionArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_PROMOTION, eDefault){};
};

template<class T>
class RouteArray: public JustInTimeArray<T>
{
public:
	RouteArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_ROUTE, eDefault){};
};

template<class T>
class TechArray: public JustInTimeArray<T>
{
public:
	TechArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_TECH, eDefault){};
};

template<class T>
class TechCategoryArray: public JustInTimeArray<T>
{
public:
	TechCategoryArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_TECH_CATEGORY, eDefault){};
};

template<class T>
class TerrainArray: public JustInTimeArray<T>
{
public:
	TerrainArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_TERRAIN, eDefault){};
};

template<class T>
class TraitArray: public JustInTimeArray<T>
{
public:
	TraitArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_TRAIT, eDefault){};
};

template<class T>
class UnitArray: public JustInTimeArray<T>
{
public:
	UnitArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_UNIT, eDefault){};
};

template<class T>
class UnitAIArray: public JustInTimeArray<T>
{
public:
	UnitAIArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_UNITAI, eDefault){};
};

template<class T>
class UnitClassArray: public JustInTimeArray<T>
{
public:
	UnitClassArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_UNITCLASS, eDefault){};
};

template<class T>
class UnitCombatArray: public JustInTimeArray<T>
{
public:
	UnitCombatArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_UNITCOMBAT, eDefault){};
};

template<class T>
class YieldArray: public JustInTimeArray<T>
{
public:
	YieldArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_YIELD, eDefault){};
};

template<class T>
class YieldCargoArray: public JustInTimeArray<T>
{
public:
	YieldCargoArray(T eDefault = (T)0) : JustInTimeArray<T>(JIT_ARRAY_CARGO_YIELD, eDefault){};
};

#endif
