#include "CvGameCoreDLL.h"


// if an array is a subset of another array, get the full array
JITarrayTypes GetBaseType(JITarrayTypes eType)
{
	switch (eType)
	{
	case JIT_ARRAY_CARGO_YIELD:        return JIT_ARRAY_YIELD;
	default:
		return eType;
	}
}

// TODO: why would the C++ code need to know which xml files are hardcoded?
bool isHardcodedArray(JITarrayTypes eType)
{
	switch (eType)
	{
	case JIT_ARRAY_GAME_OPTION:
	case JIT_ARRAY_MEMORY:
	case JIT_ARRAY_PLAYER_OPTION:
	case JIT_ARRAY_UNIT_AI:
		return true;
	}
	return false;
}

int getArrayLength(JITarrayTypes eType)
{
	switch (eType)
	{
	case JIT_ARRAY_ACHIEVE:            return NUM_ACHIEVE_TYPES;
	case JIT_ARRAY_ART_STYLE:          return GC.getNumArtStyleTypes();
	case JIT_ARRAY_BONUS:              return GC.getNumBonusInfos();
	case JIT_ARRAY_BUILD:              return GC.getNumBuildInfos();
	case JIT_ARRAY_BUILDING:           return GC.getNumBuildingInfos();
	case JIT_ARRAY_BUILDING_CLASS:     return GC.getNumBuildingClassInfos();
	case JIT_ARRAY_BUILDING_SPECIAL:   return GC.getNumSpecialBuildingInfos();
	case JIT_ARRAY_CONTACT:            return NUM_CONTACT_TYPES;
	case JIT_ARRAY_CIV_EFFECT:         return NUM_CIV_EFFECT_TYPES;
	case JIT_ARRAY_CIVIC:              return GC.getNumCivicInfos();
	case JIT_ARRAY_CIVIC_OPTION:       return GC.getNumCivicOptionInfos();
	case JIT_ARRAY_CIVILIZATION:       return GC.getNumCivilizationInfos();
	case JIT_ARRAY_CLIMATE:            return NUM_CLIMATE_TYPES;
	case JIT_ARRAY_COLOR:              return GC.getNumColorInfos();
	case JIT_ARRAY_CULTURE:            return GC.getNumCultureLevelInfos();
	case JIT_ARRAY_DIPLO:              return GC.getNumDiplomacyInfos();
	case JIT_ARRAY_DOMAIN:             return NUM_DOMAIN_TYPES;
	case JIT_ARRAY_ERA:                return GC.getNumEraInfos();
	case JIT_ARRAY_EMOTION:            return NUM_EMOTION_TYPES;
	case JIT_ARRAY_EMPHASIZE:          return GC.getNumEmphasizeInfos();
	case JIT_ARRAY_EUROPE:             return GC.getNumEuropeInfos();
	case JIT_ARRAY_EVENT:              return GC.getNumEventInfos();
	case JIT_ARRAY_EVENT_TRIGGER:      return GC.getNumEventTriggerInfos();
	case JIT_ARRAY_FATHER:             return GC.getNumFatherInfos();
	case JIT_ARRAY_FATHER_POINT:       return GC.getNumFatherPointInfos();
	case JIT_ARRAY_FEAT:               return NUM_FEAT_TYPES;
	case JIT_ARRAY_FEATURE:            return GC.getNumFeatureInfos();
	case JIT_ARRAY_GAME_OPTION:        return NUM_GAMEOPTION_TYPES;
	case JIT_ARRAY_GAME_SPEED:         return NUM_GAMESPEED_TYPES;
	case JIT_ARRAY_GOODY:              return GC.getNumGoodyInfos();
	case JIT_ARRAY_HANDICAP:           return GC.getNumHandicapInfos();
	case JIT_ARRAY_HURRY:              return GC.getNumHurryInfos();
	case JIT_ARRAY_IMPROVEMENT:        return GC.getNumImprovementInfos();
	case JIT_ARRAY_INVISIBLE:          return NUM_INVISIBLE_TYPES;
	case JIT_ARRAY_LEADER_HEAD:        return GC.getNumLeaderHeadInfos();
	case JIT_ARRAY_MEMORY:             return NUM_MEMORY_TYPES;
	case JIT_ARRAY_PLAYER_COLOR:       return GC.getNumPlayerColorInfos();
	case JIT_ARRAY_PLAYER_OPTION:      return NUM_PLAYEROPTION_TYPES;
	case JIT_ARRAY_PROFESSION:         return GC.getNumProfessionInfos();
	case JIT_ARRAY_PROMOTION:          return GC.getNumPromotionInfos();
	case JIT_ARRAY_ROUTE:              return GC.getNumRouteInfos();
	case JIT_ARRAY_SEA_LEVEL:          return NUM_SEALEVEL_TYPES;
	case JIT_ARRAY_STRATEGY:           return NUM_STRATEGY_TYPES;
	case JIT_ARRAY_TERRAIN:            return GC.getNumTerrainInfos();
	case JIT_ARRAY_TRADE_SCREEN:       return NUM_TRADESCREEN_TYPES;
	case JIT_ARRAY_TRAIT:              return GC.getNumTraitInfos();
	case JIT_ARRAY_UNIT:               return GC.getNumUnitInfos();
	case JIT_ARRAY_UNIT_AI:            return NUM_UNITAI_TYPES;
	case JIT_ARRAY_UNIT_CLASS:         return GC.getNumUnitClassInfos();
	case JIT_ARRAY_UNIT_COMBAT:        return GC.getNumUnitCombatInfos();
	case JIT_ARRAY_UNIT_SPECIAL:       return GC.getNumSpecialUnitInfos();
	case JIT_ARRAY_VICTORY:            return GC.getNumVictoryInfos();
	case JIT_ARRAY_WORLD_SIZE:         return NUM_WORLDSIZE_TYPES;
	case JIT_ARRAY_YIELD:              return NUM_YIELD_TYPES;
	case JIT_ARRAY_CARGO_YIELD:        return NUM_CARGO_YIELD_TYPES;
	case JIT_ARRAY_PLAYER:             return NUM_PLAYER_TYPES;
	case JIT_ARRAY_TEAM:               return NUM_TEAM_TYPES;
	case JIT_ARRAY_PLOT_TYPE:          return NUM_PLOT_TYPES;
	case JIT_ARRAY_CIV_CATEGORY:       return NUM_CIV_CATEGORY_TYPES;
	}
	FAssertMsg(false, "missing length case");
	return 0;
}

const CvInfoBase* getBaseInfo(JITarrayTypes eType, int iIndex)
{
	JITarrayTypes eBaseType = GetBaseType(eType);

	switch (eBaseType)
	{
	case JIT_ARRAY_ACHIEVE:            return &GC.getAchieveInfo((AchieveTypes)iIndex);
	case JIT_ARRAY_BONUS:              return &GC.getBonusInfo((BonusTypes)iIndex);
	case JIT_ARRAY_BUILD:              return &GC.getBuildInfo((BuildTypes)iIndex);
	case JIT_ARRAY_BUILDING:           return &GC.getBuildingInfo((BuildingTypes)iIndex);
	case JIT_ARRAY_BUILDING_CLASS:     return &GC.getBuildingClassInfo((BuildingClassTypes)iIndex);
	case JIT_ARRAY_BUILDING_SPECIAL:   return &GC.getSpecialBuildingInfo((SpecialBuildingTypes)iIndex);
	case JIT_ARRAY_CIV_EFFECT:         return &GC.getCivEffectInfo(static_cast<CivEffectTypes>(iIndex));
	case JIT_ARRAY_CIVIC:              return &GC.getCivicInfo((CivicTypes)iIndex);
	case JIT_ARRAY_CIVIC_OPTION:       return &GC.getCivicOptionInfo((CivicOptionTypes)iIndex);
	case JIT_ARRAY_CIVILIZATION:       return &GC.getCivilizationInfo((CivilizationTypes)iIndex);
	case JIT_ARRAY_CLIMATE:            return &GC.getClimateInfo((ClimateTypes)iIndex);
	case JIT_ARRAY_COLOR:              return &GC.getColorInfo((ColorTypes)iIndex);
	case JIT_ARRAY_CULTURE:            return &GC.getCultureLevelInfo((CultureLevelTypes)iIndex);
	case JIT_ARRAY_DIPLO:              return &GC.getDiplomacyInfo(iIndex);
	case JIT_ARRAY_DOMAIN:             return &GC.getDomainInfo((DomainTypes)iIndex);
	case JIT_ARRAY_ERA:                return &GC.getEraInfo((EraTypes)iIndex);
	case JIT_ARRAY_EMPHASIZE:          return &GC.getEmphasizeInfo((EmphasizeTypes)iIndex);
	case JIT_ARRAY_EUROPE:             return &GC.getEuropeInfo((EuropeTypes)iIndex);
	case JIT_ARRAY_EVENT:              return &GC.getEventInfo((EventTypes)iIndex);
	case JIT_ARRAY_EVENT_TRIGGER:      return &GC.getEventTriggerInfo((EventTriggerTypes)iIndex);
	case JIT_ARRAY_FATHER:             return &GC.getFatherInfo((FatherTypes)iIndex);
	case JIT_ARRAY_FATHER_POINT:       return &GC.getFatherPointInfo((FatherPointTypes)iIndex);
	case JIT_ARRAY_FEATURE:            return &GC.getFeatureInfo((FeatureTypes)iIndex);
	case JIT_ARRAY_GAME_OPTION:        return &GC.getGameOptionInfo((GameOptionTypes)iIndex);
	case JIT_ARRAY_GAME_SPEED:         return &GC.getGameSpeedInfo((GameSpeedTypes)iIndex);
	case JIT_ARRAY_GOODY:              return &GC.getGoodyInfo((GoodyTypes)iIndex);
	case JIT_ARRAY_HANDICAP:           return &GC.getHandicapInfo((HandicapTypes)iIndex);
	case JIT_ARRAY_HURRY:              return &GC.getHurryInfo((HurryTypes)iIndex);
	case JIT_ARRAY_IMPROVEMENT:        return &GC.getImprovementInfo((ImprovementTypes)iIndex);
	case JIT_ARRAY_INVISIBLE:          return &GC.getInvisibleInfo((InvisibleTypes)iIndex);
	case JIT_ARRAY_LEADER_HEAD:        return &GC.getLeaderHeadInfo((LeaderHeadTypes)iIndex);
	case JIT_ARRAY_MEMORY:             return &GC.getMemoryInfo((MemoryTypes)iIndex);
	case JIT_ARRAY_PLAYER_COLOR:       return &GC.getPlayerColorInfo((PlayerColorTypes)iIndex);
	case JIT_ARRAY_PLAYER_OPTION:      return &GC.getPlayerOptionInfo((PlayerOptionTypes)iIndex);
	case JIT_ARRAY_PROFESSION:         return &GC.getProfessionInfo((ProfessionTypes)iIndex);
	case JIT_ARRAY_PROMOTION:          return &GC.getPromotionInfo((PromotionTypes)iIndex);
	case JIT_ARRAY_ROUTE:              return &GC.getRouteInfo((RouteTypes)iIndex);
	case JIT_ARRAY_SEA_LEVEL:          return &GC.getSeaLevelInfo((SeaLevelTypes)iIndex);
	case JIT_ARRAY_TERRAIN:            return &GC.getTerrainInfo((TerrainTypes)iIndex);
	case JIT_ARRAY_TRADE_SCREEN:       return &GC.getTradeScreenInfo((TradeScreenTypes)iIndex);
	case JIT_ARRAY_TRAIT:              return &GC.getTraitInfo((TraitTypes)iIndex);
	case JIT_ARRAY_UNIT:               return &GC.getUnitInfo((UnitTypes)iIndex);
	case JIT_ARRAY_UNIT_AI:            return &GC.getUnitAIInfo((UnitAITypes)iIndex);
	case JIT_ARRAY_UNIT_CLASS:         return &GC.getUnitClassInfo((UnitClassTypes)iIndex);
	case JIT_ARRAY_UNIT_COMBAT:        return &GC.getUnitCombatInfo((UnitCombatTypes)iIndex);
	case JIT_ARRAY_UNIT_SPECIAL:       return &GC.getSpecialUnitInfo((SpecialUnitTypes)iIndex);
	case JIT_ARRAY_VICTORY:            return &GC.getVictoryInfo((VictoryTypes)iIndex);
	case JIT_ARRAY_WORLD_SIZE:         return &GC.getWorldInfo((WorldSizeTypes)iIndex);
	case JIT_ARRAY_YIELD:              return &GC.getYieldInfo((YieldTypes)iIndex);
	}
	FAssertMsg(false, "missing info case");
	return NULL;
}

static const char* getArrayType(PlotTypes eType)
{
	switch (eType)
	{
	case PLOT_PEAK:  return "PLOT_PEAK";
	case PLOT_HILLS: return "PLOT_HILLS";
	case PLOT_LAND:  return "PLOT_LAND";
	case PLOT_OCEAN: return "PLOT_OCEAN";
	}
	BOOST_STATIC_ASSERT(NUM_PLOT_TYPES == 4);
	FAssert(false);
	return "";
}

static const char* getArrayType(CivCategoryTypes eType)
{
	switch (eType)
	{
	case CIV_CATEGORY_EUROPEAN:  return "CIV_CATEGORY_EUROPEAN";
	case CIV_CATEGORY_NATIVE:    return "CIV_CATEGORY_NATIVE";
	case CIV_CATEGORY_KING:      return "CIV_CATEGORY_KING";
	case CIV_CATEGORY_BARBARIAN: return "CIV_CATEGORY_BARBARIAN";
	case CIV_CATEGORY_CHURCH:    return "CIV_CATEGORY_CHURCH";
	case CIV_CATEGORY_NOT_SET:   return "CIV_CATEGORY_NOT_SET";
	}
	BOOST_STATIC_ASSERT(NUM_CIV_CATEGORY_TYPES == 6);
	FAssert(false);
	return "";
}

const char* getArrayType(JITarrayTypes eType, int iIndex)
{
	if (iIndex == -1)
	{
		return "NONE";
	}

	// not all JIT arrays relies on XML data
	// return an empty string when data doesn't rely in CvBasicInfo

	switch (eType)
	{
	case JIT_ARRAY_ART_STYLE:          return GC.getArtStyleTypes((ArtStyleTypes)iIndex); // use the actual art style string for type
	case JIT_ARRAY_PLOT_TYPE:          return getArrayType(static_cast<PlotTypes>(iIndex));
	case JIT_ARRAY_CIV_CATEGORY:       return getArrayType(static_cast<CivCategoryTypes>(iIndex));
	}
	const CvInfoBase *pInfo = getBaseInfo(eType, iIndex);
	if (pInfo == NULL)
	{
		FAssertMsg(false, "Failed to find info class");
		return "";
	}
	return pInfo->getType();
}

CvWString getArrayTypeWide(JITarrayTypes eType, int iIndex)
{
	CvWString szType = getArrayType(eType, iIndex);
	return szType;
}

const char* getArrayName(JITarrayTypes eType)
{
	// give each type a unique string
	// this is used to identify if order has changed when loading a savegame
	// both read and write use this function meaning it will be compared to itself (no need to sync with anything else)

	JITarrayTypes eBaseType = GetBaseType(eType);
	
	switch (eBaseType)
	{
	case JIT_ARRAY_ACHIEVE:            return "Achieve";
	case JIT_ARRAY_ART_STYLE:          return "ArtStyle";
	case JIT_ARRAY_BONUS:              return "Bonus";
	case JIT_ARRAY_BUILD:              return "Build";
	case JIT_ARRAY_BUILDING:           return "Building";
	case JIT_ARRAY_BUILDING_CLASS:     return "BuildingClass";
	case JIT_ARRAY_BUILDING_SPECIAL:   return "SpecialBuilding";
	case JIT_ARRAY_CIV_EFFECT:         return "CivEffect";
	case JIT_ARRAY_CIVIC:              return "Civic";
	case JIT_ARRAY_CIVIC_OPTION:       return "CivicOption";
	case JIT_ARRAY_CIVILIZATION:       return "Civilization";
	case JIT_ARRAY_CLIMATE:            return "Cllimate";
	case JIT_ARRAY_COLOR:              return "Color";
	case JIT_ARRAY_CULTURE:            return "Culture";
	case JIT_ARRAY_DIPLO:              return "Diplo";
	case JIT_ARRAY_ERA:                return "Era";
	case JIT_ARRAY_EMPHASIZE:          return "Emphasize";
	case JIT_ARRAY_EUROPE:             return "Europe";
	case JIT_ARRAY_EVENT:              return "Event";
	case JIT_ARRAY_EVENT_TRIGGER:      return "EventTrigger";
	case JIT_ARRAY_FATHER:             return "Father";
	case JIT_ARRAY_FATHER_POINT:       return "FatherPoint";
	case JIT_ARRAY_FEATURE:            return "Feature";
	case JIT_ARRAY_GAME_OPTION:        return "GameOption";
	case JIT_ARRAY_GAME_SPEED:         return "GameSpeed";
	case JIT_ARRAY_GOODY:              return "Goody";
	case JIT_ARRAY_HANDICAP:           return "Handicap";
	case JIT_ARRAY_HURRY:              return "Hurry";
	case JIT_ARRAY_IMPROVEMENT:        return "Improvement";
	case JIT_ARRAY_INVISIBLE:          return "Invisible";
	case JIT_ARRAY_LEADER_HEAD:        return "LeaderHead";
	case JIT_ARRAY_MEMORY:             return "Memory";
	case JIT_ARRAY_PLAYER_COLOR:       return "PlayerColor";
	case JIT_ARRAY_PLAYER_OPTION:      return "PlayerOption";
	case JIT_ARRAY_PROFESSION:         return "Profession";
	case JIT_ARRAY_PROMOTION:          return "Promotion";
	case JIT_ARRAY_ROUTE:              return "Route";
	case JIT_ARRAY_SEA_LEVEL:          return "SeaLevel";
	case JIT_ARRAY_TERRAIN:            return "Terrain";
	case JIT_ARRAY_TRADE_SCREEN:       return "TradeScreen";
	case JIT_ARRAY_TRAIT:              return "Trait";
	case JIT_ARRAY_UNIT:               return "Unit";
	case JIT_ARRAY_UNIT_AI:            return "UnitAI";
	case JIT_ARRAY_UNIT_CLASS:         return "UnitClass";
	case JIT_ARRAY_UNIT_COMBAT:        return "UnitCombat";
	case JIT_ARRAY_UNIT_SPECIAL:       return "SpecialUnit";
	case JIT_ARRAY_VICTORY:            return "Victory";
	case JIT_ARRAY_WORLD_SIZE:         return "WorldSize";
	case JIT_ARRAY_YIELD:              return "Yield";
	case JIT_ARRAY_DOMAIN:             return "Domain";
	case JIT_ARRAY_PLOT_TYPE:          return "PlotType";
	case JIT_ARRAY_CIV_CATEGORY:       return "CivCategory";
	}
	FAssertMsg(false, "missing info case");
	return "";
}

CvWString getArrayNameWide(JITarrayTypes eType)
{
	CvWString szType = getArrayName(eType);
	return szType;
}

const char* getArrayPrefix(JITarrayTypes eType)
{
	// returns the prefix for each array type
	// each type of that array has to start with the prefix
	
	JITarrayTypes eBaseType = GetBaseType(eType);

	switch (eBaseType)
	{
	case JIT_ARRAY_ACHIEVE:            return "ACHIEVE_";
	case JIT_ARRAY_ART_STYLE:          return "ARTSTYLE_";
	case JIT_ARRAY_BONUS:              return "BONUS_";
	case JIT_ARRAY_BUILD:              return "BUILD_";
	case JIT_ARRAY_BUILDING:           return "BUILDING_";
	case JIT_ARRAY_BUILDING_CLASS:     return "BUILDINGCLASS_";
	case JIT_ARRAY_BUILDING_SPECIAL:   return "SPECIALBUILDING_";
	case JIT_ARRAY_CIVIC:              return "CIVIC_";
	case JIT_ARRAY_CIVIC_OPTION:       return "CIVICOPTION_";
	case JIT_ARRAY_CIVILIZATION:       return "CIVILIZATION_";
	case JIT_ARRAY_CIV_EFFECT:         return "CIV_EFFECT_";
	case JIT_ARRAY_CLIMATE:            return "CLIMATE_";
	case JIT_ARRAY_COLOR:              return "COLOR_";
	case JIT_ARRAY_CULTURE:            return "CULTURELEVEL_";
	case JIT_ARRAY_DIPLO:              return ""; // intentionally left blank. Types starts with USER_ or AI_ prefixes
	case JIT_ARRAY_ERA:                return "ERA_";
	case JIT_ARRAY_EMPHASIZE:          return "EMPHASIZE_";
	case JIT_ARRAY_EUROPE:             return "EUROPE_";
	case JIT_ARRAY_EVENT:              return "EVENT_";
	case JIT_ARRAY_EVENT_TRIGGER:      return "EVENTTRIGGER_";
	case JIT_ARRAY_FATHER:             return "FATHER_";
	case JIT_ARRAY_FATHER_POINT:       return "FATHER_POINT_";
	case JIT_ARRAY_FEATURE:            return "FEATURE_";
	case JIT_ARRAY_GAME_OPTION:        return "GAMEOPTION_";
	case JIT_ARRAY_GAME_SPEED:         return "GAMESPEED_";
	case JIT_ARRAY_GOODY:              return "GOODY_";
	case JIT_ARRAY_HANDICAP:           return "HANDICAP_";
	case JIT_ARRAY_HURRY:              return "HURRY_";
	case JIT_ARRAY_IMPROVEMENT:        return "IMPROVEMENT_";
	case JIT_ARRAY_INVISIBLE:          return "INVISIBLE_";
	case JIT_ARRAY_LEADER_HEAD:        return "LEADER_";
	case JIT_ARRAY_MEMORY:             return "MEMORY_";
	case JIT_ARRAY_PLAYER_COLOR:       return "PLAYERCOLOR_";
	case JIT_ARRAY_PLAYER_OPTION:      return "PLAYEROPTION_";
	case JIT_ARRAY_PROFESSION:         return "PROFESSION_";
	case JIT_ARRAY_PROMOTION:          return "PROMOTION_";
	case JIT_ARRAY_ROUTE:              return "ROUTE_";
	case JIT_ARRAY_SEA_LEVEL:          return "SEALEVEL_";
	case JIT_ARRAY_TERRAIN:            return "TERRAIN_";
	case JIT_ARRAY_TRADE_SCREEN:       return "TRADE_SCREEN_";
	case JIT_ARRAY_TRAIT:              return "TRAIT_";
	case JIT_ARRAY_UNIT:               return "UNIT_";
	case JIT_ARRAY_UNIT_AI:            return "UNITAI_";
	case JIT_ARRAY_UNIT_CLASS:         return "UNITCLASS_";
	case JIT_ARRAY_UNIT_COMBAT:        return "UNITCOMBAT_";
	case JIT_ARRAY_UNIT_SPECIAL:       return "SPECIALUNIT_";
	case JIT_ARRAY_VICTORY:            return "VICTORY_";
	case JIT_ARRAY_WORLD_SIZE:         return "WORLDSIZE_";
	case JIT_ARRAY_YIELD:              return "YIELD_";
	case JIT_ARRAY_DOMAIN:             return "DOMAIN_";

	}
	FAssertMsg(false, "missing info case");
	return "";
}

int getIndexForType(JITarrayTypes eType, const char* pTypeString)
{
	int iLength = getArrayLength(eType);

	for (int i = 0; i < iLength; ++i)
	{
		if (!strcmp(getArrayType(eType, i), pTypeString))
		{
			return i;
		}
	}

	return -1;
}

int getIndexForTypeAddingPrefix(JITarrayTypes eType, const char* pTypeString)
{
	if (strlen(pTypeString) == 0)
	{
		return -1;
	}

	CvString szType = getArrayPrefix(eType);
	szType.append(pTypeString);
	return getIndexForType(eType, szType);
}

const char* getArrayTypeWithoutPrefix(JITarrayTypes eType, int iIndex)
{
	if (iIndex == -1)
	{
		return "";
	}

	const char* szType = getArrayType(eType, iIndex);
	const char* szPrefix = getArrayPrefix(eType);
	int iLength = strlen(szPrefix);
	return szType + iLength;
}

JITarrayTypes getJITArrayTypeFromString(const char* szType)
{
	for (JITarrayTypes eType = FIRST_JIT_ARRAY; eType < NUM_JITarrayTypes; ++eType)
	{
		if (strcmp(szType, getArrayName(eType)) == 0)
		{
			return eType;
		}
	}
	return NO_JIT_ARRAY_TYPE;
}
