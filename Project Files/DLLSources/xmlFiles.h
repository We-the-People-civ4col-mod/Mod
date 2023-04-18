#pragma once

#include "CvEnums.h"

template <typename T>
struct xmlLocation;

// Art

template <>
struct xmlLocation<MainMenuTypes>
{
	static const char* file() { return "CIV4MainMenus"; }
	static const char* folder() { return "Art"; }
	static const char* path() { return "Civ4MainMenus/MainMenus/MainMenu"; }
};

template <>
struct xmlLocation<RiverModelTypes>
{
	static const char* file() { return "CIV4RiverModelInfos"; }
	static const char* folder() { return "Art"; }
	static const char* path() { return "Civ4RiverModelInfos/RiverModelInfos/RiverModelInfo"; }
};

template <>
struct xmlLocation<RouteModelTypes>
{
	static const char* file() { return "Civ4RouteModelInfos"; }
	static const char* folder() { return "Art"; }
	static const char* path() { return "Civ4RouteModelInfos/RouteModelInfos/RouteModelInfo"; }
};

// BasicInfos

template <>
struct xmlLocation<AttitudeTypes>
{
	static const char* file() { return "CIV4AttitudeInfos"; }
	static const char* folder() { return "BasicInfos"; }
	static const char* path() { return "Civ4AttitudeInfos/AttitudeInfos/AttitudeInfo"; }
};

template <>
struct xmlLocation<CalendarTypes>
{
	static const char* file() { return "CIV4CalendarInfos"; }
	static const char* folder() { return "BasicInfos"; }
	static const char* path() { return "Civ4CalendarInfos/CalendarInfos/CalendarInfo"; }
};

template <>
struct xmlLocation<ConceptTypes>
{
	static const char* file() { return "CIV4BasicInfos"; }
	static const char* folder() { return "BasicInfos"; }
	static const char* path() { return "Civ4BasicInfos/ConceptInfos/ConceptInfo"; }
};

template <>
struct xmlLocation<DenialTypes>
{
	static const char* file() { return "CIV4DenialInfos"; }
	static const char* folder() { return "BasicInfos"; }
	static const char* path() { return "Civ4DenialInfos/DenialInfos/DenialInfo"; }
};

template <>
struct xmlLocation<DomainTypes>
{
	static const char* file() { return "CIV4DomainInfos"; }
	static const char* folder() { return "BasicInfos"; }
	static const char* path() { return "Civ4DomainInfos/DomainInfos/DomainInfo"; }
};

template <>
struct xmlLocation<FatherCategoryTypes>
{
	static const char* file() { return "CIV4FatherCategoryInfos"; }
	static const char* folder() { return "BasicInfos"; }
	static const char* path() { return "Civ4FatherCategoryInfos/FatherCategoryInfos/FatherCategoryInfo"; }
};

template <>
struct xmlLocation<InvisibleTypes>
{
	static const char* file() { return "CIV4InvisibleInfos"; }
	static const char* folder() { return "BasicInfos"; }
	static const char* path() { return "Civ4InvisibleInfos/InvisibleInfos/InvisibleInfo"; }
};

template <>
struct xmlLocation<MemoryTypes>
{
	static const char* file() { return "CIV4MemoryInfos"; }
	static const char* folder() { return "BasicInfos"; }
	static const char* path() { return "Civ4MemoryInfos/MemoryInfos/MemoryInfo"; }
};

template <>
struct xmlLocation<MonthTypes>
{
	static const char* file() { return "CIV4MonthInfos"; }
	static const char* folder() { return "BasicInfos"; }
	static const char* path() { return "Civ4MonthInfos/MonthInfos/MonthInfo"; }
};

template <>
struct xmlLocation<SeasonTypes>
{
	static const char* file() { return "CIV4SeasonInfos"; }
	static const char* folder() { return "BasicInfos"; }
	static const char* path() { return "Civ4SeasonInfos/SeasonInfos/SeasonInfo"; }
};

template <>
struct xmlLocation<UnitAITypes>
{
	static const char* file() { return "CIV4UnitAIInfos"; }
	static const char* folder() { return "BasicInfos"; }
	static const char* path() { return "Civ4UnitAIInfos/UnitAIInfos/UnitAIInfo"; }
};

template <>
struct xmlLocation<UnitCombatTypes>
{
	static const char* file() { return "CIV4UnitCombatInfos"; }
	static const char* folder() { return "BasicInfos"; }
	static const char* path() { return "Civ4UnitCombatInfos/UnitCombatInfos/UnitCombatInfo"; }
};

// Buildings

template <>
struct xmlLocation<BuildingTypes>
{
	static const char* file() { return "CIV4BuildingInfos"; }
	static const char* folder() { return "Buildings"; }
	static const char* path() { return "Civ4BuildingInfos/BuildingInfos/BuildingInfo"; }
};

template <>
struct xmlLocation<BuildingClassTypes>
{
	static const char* file() { return "CIV4BuildingClassInfos"; }
	static const char* folder() { return "Buildings"; }
	static const char* path() { return "Civ4BuildingClassInfos/BuildingClassInfos/BuildingClassInfo"; }
};

template <>
struct xmlLocation<SpecialBuildingTypes>
{
	static const char* file() { return "CIV4SpecialBuildingInfos"; }
	static const char* folder() { return "Buildings"; }
	static const char* path() { return "Civ4SpecialBuildingInfos/SpecialBuildingInfos/SpecialBuildingInfo"; }
};

// CivEffects

template <>
struct xmlLocation<CivEffectTypes>
{
	static const char* file() { return "CIV4CivEffectsInfos"; }
	static const char* folder() { return "CivEffects"; }
	static const char* path() { return "Civ4CivEffectInfos/CivEffectInfo"; }
};

// Civilizations

template <>
struct xmlLocation<AlarmTypes>
{
	static const char* file() { return "CIV4AlarmInfos"; }
	static const char* folder() { return "Civilizations"; }
	static const char* path() { return "Civ4AlarmInfos/AlarmInfos/AlarmInfo"; }
};

template <>
struct xmlLocation<CivilizationTypes>
{
	static const char* file() { return "CIV4CivilizationInfos"; }
	static const char* folder() { return "Civilizations"; }
	static const char* path() { return "Civ4CivilizationInfos/CivilizationInfos/CivilizationInfo"; }
};

template <>
struct xmlLocation<LeaderHeadTypes>
{
	static const char* file() { return "CIV4LeaderHeadInfos"; }
	static const char* folder() { return "Civilizations"; }
	static const char* path() { return "Civ4LeaderHeadInfos/LeaderHeadInfos/LeaderHeadInfo"; }
};

template <>
struct xmlLocation<TraitTypes>
{
	static const char* file() { return "CIV4TraitInfos"; }
	static const char* folder() { return "Civilizations"; }
	static const char* path() { return "Civ4TraitInfos/TraitInfos/TraitInfo"; }
};

template <>
struct xmlLocation<UnitArtStyleTypes>
{
	static const char* file() { return "CIV4UnitArtStyleTypeInfos"; }
	static const char* folder() { return "Civilizations"; }
	static const char* path() { return "Civ4UnitArtStyleTypeInfos/UnitArtStyleTypeInfos/UnitArtStyleTypeInfo"; }
};

// Events

template <>
struct xmlLocation<AchieveTypes>
{
	static const char* file() { return "CIV4AchieveInfos"; }
	static const char* folder() { return "Events"; }
	static const char* path() { return "Civ4AchieveInfos/AchieveInfo"; }
};

template <>
struct xmlLocation<EventTypes>
{
	static const char* file() { return "CIV4EventInfos"; }
	static const char* folder() { return "Events"; }
	static const char* path() { return "Civ4EventInfos/EventInfos/EventInfo"; }
};

template <>
struct xmlLocation<EventTriggerTypes>
{
	static const char* file() { return "CIV4EventTriggerInfos"; }
	static const char* folder() { return "Events"; }
	static const char* path() { return "Civ4EventTriggerInfos/EventTriggerInfos/EventTriggerInfo"; }
};

// GameInfo

template <>
struct xmlLocation<CivicTypes>
{
	static const char* file() { return "CIV4CivicInfos"; }
	static const char* folder() { return "GameInfo"; }
	static const char* path() { return "Civ4CivicInfos/CivicInfos/CivicInfo"; }
};

template <>
struct xmlLocation<CivicOptionTypes>
{
	static const char* file() { return "CIV4CivicOptionInfos"; }
	static const char* folder() { return "GameInfo"; }
	static const char* path() { return "Civ4CivicOptionInfos/CivicOptionInfos/CivicOptionInfo"; }
};

template <>
struct xmlLocation<ClimateTypes>
{
	static const char* file() { return "CIV4ClimateInfo"; }
	static const char* folder() { return "GameInfo"; }
	static const char* path() { return "Civ4ClimateInfo/ClimateInfos/ClimateInfo"; }
};

template <>
struct xmlLocation<CultureLevelTypes>
{
	static const char* file() { return "CIV4CultureLevelInfo"; }
	static const char* folder() { return "GameInfo"; }
	static const char* path() { return "Civ4CultureLevelInfo/CultureLevelInfos/CultureLevelInfo"; }
};

template <>
struct xmlLocation<CursorTypes>
{
	static const char* file() { return "CIV4CursorInfo"; }
	static const char* folder() { return "GameInfo"; }
	static const char* path() { return "Civ4CursorInfo/CursorInfos/CursorInfo"; }
};

template <>
struct xmlLocation<DiplomacyTypes>
{
	static const char* file() { return "CIV4DiplomacyInfos"; }
	static const char* folder() { return "GameInfo"; }
	static const char* path() { return "Civ4DiplomacyInfos/DiplomacyInfos/DiplomacyInfo"; }
};

template <>
struct xmlLocation<EraTypes>
{
	static const char* file() { return "CIV4EraInfos"; }
	static const char* folder() { return "GameInfo"; }
	static const char* path() { return "Civ4EraInfos/EraInfos/EraInfo"; }
};

template <>
struct xmlLocation<EmphasizeTypes>
{
	static const char* file() { return "CIV4EmphasizeInfo"; }
	static const char* folder() { return "GameInfo"; }
	static const char* path() { return "Civ4EmphasizeInfo/EmphasizeInfos/EmphasizeInfo"; }
};

template <>
struct xmlLocation<EuropeTypes>
{
	static const char* file() { return "CIV4EuropeInfo"; }
	static const char* folder() { return "GameInfo"; }
	static const char* path() { return "Civ4EuropeInfo/EuropeInfos/EuropeInfo"; }
};

template <>
struct xmlLocation<FatherTypes>
{
	static const char* file() { return "CIV4FatherInfos"; }
	static const char* folder() { return "GameInfo"; }
	static const char* path() { return "Civ4FatherInfos/FatherInfos/FatherInfo"; }
};

template <>
struct xmlLocation<FatherPointTypes>
{
	static const char* file() { return "CIV4FatherPointInfos"; }
	static const char* folder() { return "GameInfo"; }
	static const char* path() { return "Civ4FatherPointInfos/FatherPointInfos/FatherPointInfo"; }
};

template <>
struct xmlLocation<ForceControlTypes>
{
	static const char* file() { return "CIV4ForceControlInfos"; }
	static const char* folder() { return "GameInfo"; }
	static const char* path() { return "Civ4ForceControlInfos/ForceControlInfos/ForceControlInfo"; }
};

template <>
struct xmlLocation<GameOptionTypes>
{
	static const char* file() { return "CIV4GameOptionInfos"; }
	static const char* folder() { return "GameInfo"; }
	static const char* path() { return "Civ4GameOptionInfos/GameOptionInfos/GameOptionInfo"; }
};

template <>
struct xmlLocation<GameSpeedTypes>
{
	static const char* file() { return "CIV4GameSpeedInfo"; }
	static const char* folder() { return "GameInfo"; }
	static const char* path() { return "Civ4GameSpeedInfo/GameSpeedInfos/GameSpeedInfo"; }
};

template <>
struct xmlLocation<GoodyTypes>
{
	static const char* file() { return "CIV4GoodyInfo"; }
	static const char* folder() { return "GameInfo"; }
	static const char* path() { return "Civ4GoodyInfo/GoodyInfos/GoodyInfo"; }
};

template <>
struct xmlLocation<GraphicOptionTypes>
{
	static const char* file() { return "CIV4GraphicOptionInfos"; }
	static const char* folder() { return "GameInfo"; }
	static const char* path() { return "Civ4GraphicOptionInfos/GraphicOptionInfos/GraphicOptionInfo"; }
};

template <>
struct xmlLocation<HandicapTypes>
{
	static const char* file() { return "CIV4HandicapInfo"; }
	static const char* folder() { return "GameInfo"; }
	static const char* path() { return "Civ4HandicapInfo/HandicapInfos/HandicapInfo"; }
};

template <>
struct xmlLocation<HintTypes>
{
	static const char* file() { return "CIV4Hints"; }
	static const char* folder() { return "GameInfo"; }
	static const char* path() { return "Civ4Hints/HintInfos/HintInfo"; }
};

template <>
struct xmlLocation<HurryTypes>
{
	static const char* file() { return "CIV4HurryInfo"; }
	static const char* folder() { return "GameInfo"; }
	static const char* path() { return "Civ4HurryInfo/HurryInfos/HurryInfo"; }
};

template <>
struct xmlLocation<MultiplayerOptionTypes>
{
	static const char* file() { return "CIV4MPOptionInfos"; }
	static const char* folder() { return "GameInfo"; }
	static const char* path() { return "Civ4MPOptionInfos/MPOptionInfos/MPOptionInfo"; }
};

template <>
struct xmlLocation<PlayerOptionTypes>
{
	static const char* file() { return "CIV4PlayerOptionInfos"; }
	static const char* folder() { return "GameInfo"; }
	static const char* path() { return "Civ4PlayerOptionInfos/PlayerOptionInfos/PlayerOptionInfo"; }
};

template <>
struct xmlLocation<SeaLevelTypes>
{
	static const char* file() { return "CIV4SeaLevelInfo"; }
	static const char* folder() { return "GameInfo"; }
	static const char* path() { return "Civ4SeaLevelInfo/SeaLevelInfos/SeaLevelInfo"; }
};

template <>
struct xmlLocation<TradeScreenTypes>
{
	static const char* file() { return "CIV4TradeScreenInfo"; }
	static const char* folder() { return "GameInfo"; }
	static const char* path() { return "Civ4TradeScreenInfo/TradeScreenInfos/TradeScreenInfo"; }
};

template <>
struct xmlLocation<TurnTimerTypes>
{
	static const char* file() { return "CIV4TurnTimerInfo"; }
	static const char* folder() { return "GameInfo"; }
	static const char* path() { return "Civ4TurnTimerInfo/TurnTimerInfos/TurnTimerInfo"; }
};

template <>
struct xmlLocation<VictoryTypes>
{
	static const char* file() { return "CIV4VictoryInfo"; }
	static const char* folder() { return "GameInfo"; }
	static const char* path() { return "Civ4VictoryInfo/VictoryInfos/VictoryInfo"; }
};

template <>
struct xmlLocation<WorldSizeTypes>
{
	static const char* file() { return "CIV4WorldInfo"; }
	static const char* folder() { return "GameInfo"; }
	static const char* path() { return "Civ4WorldInfo/WorldInfos/WorldInfo"; }
};

// Interface

template <>
struct xmlLocation<ColorTypes>
{
	static const char* file() { return "CIV4ColorVals"; }
	static const char* folder() { return "Interface"; }
	static const char* path() { return "Civ4ColorVals/ColorVals/ColorVal"; }
};

template <>
struct xmlLocation<InterfaceModeTypes>
{
	static const char* file() { return "CIV4InterfaceModeInfos"; }
	static const char* folder() { return "Interface"; }
	static const char* path() { return "Civ4InterfaceModeInfos/InterfaceModeInfos/InterfaceModeInfo"; }
};

template <>
struct xmlLocation<PlayerColorTypes>
{
	static const char* file() { return "CIV4PlayerColorInfos"; }
	static const char* folder() { return "Interface"; }
	static const char* path() { return "Civ4PlayerColorInfos/PlayerColorInfos/PlayerColorInfo"; }
};

template <>
struct xmlLocation<SlideShowTypes>
{
	static const char* file() { return "CIV4SlideShowInfos"; }
	static const char* folder() { return "Interface"; }
	static const char* path() { return "Civ4SlideShowInfos/SlideShowInfos/SlideShowInfo"; }
};

template <>
struct xmlLocation<SlideShowRandomTypes>
{
	static const char* file() { return "CIV4SlideShowRandomInfos"; }
	static const char* folder() { return "Interface"; }
	static const char* path() { return "Civ4SlideShowRandomInfos/SlideShowRandomInfos/SlideShowRandomInfo"; }
};

template <>
struct xmlLocation<WorldPickerTypes>
{
	static const char* file() { return "CIV4WorldPickerInfos"; }
	static const char* folder() { return "Interface"; }
	static const char* path() { return "Civ4WorldPickerInfos/WorldPickerInfos/WorldPickerInfo"; }
};

// Misc

template <>
struct xmlLocation<AttachableTypes>
{
	static const char* file() { return "CIV4AttachableInfos"; }
	static const char* folder() { return "Misc"; }
	static const char* path() { return "Civ4AttachableInfos/AttachableInfos/AttachableInfo"; }
};

template <>
struct xmlLocation<CameraOverlayTypes>
{
	static const char* file() { return "CIV4CameraOverlayInfos"; }
	static const char* folder() { return "Misc"; }
	static const char* path() { return "Civ4CameraOverlayInfos/CameraOverlayInfos/CameraOverlayInfo"; }
};

template <>
struct xmlLocation<DetailManagerTypes>
{
	static const char* file() { return "CIV4DetailManager"; }
	static const char* folder() { return "Misc"; }
	static const char* path() { return "DetailManagerInfos/Fader"; }
};

template <>
struct xmlLocation<EffectTypes>
{
	static const char* file() { return "CIV4EffectInfos"; }
	static const char* folder() { return "Misc"; }
	static const char* path() { return "Civ4EffectInfos/EffectInfos/EffectInfo"; }
};

template <>
struct xmlLocation<RouteTypes>
{
	static const char* file() { return "Civ4RouteInfos"; }
	static const char* folder() { return "Misc"; }
	static const char* path() { return "Civ4RouteInfos/RouteInfos/RouteInfo"; }
};

template <>
struct xmlLocation<TerrainPlaneTypes>
{
	static const char* file() { return "CIV4TerrainPlaneInfos"; }
	static const char* folder() { return "Misc"; }
	static const char* path() { return "Civ4TerrainPlaneInfos/TerrainPlaneInfos/TerrainPlaneInfo"; }
};

template <>
struct xmlLocation<WaterPlaneTypes>
{
	static const char* file() { return "CIV4WaterPlaneInfos"; }
	static const char* folder() { return "Misc"; }
	static const char* path() { return "Civ4WaterPlaneInfos/WaterPlaneInfos/WaterPlaneInfo"; }
};

// Terrain

template <>
struct xmlLocation<BonusTypes>
{
	static const char* file() { return "CIV4BonusInfos"; }
	static const char* folder() { return "Terrain"; }
	static const char* path() { return "Civ4BonusInfos/BonusInfos/BonusInfo"; }
};

template <>
struct xmlLocation<FeatureTypes>
{
	static const char* file() { return "Civ4FeatureInfos"; }
	static const char* folder() { return "Terrain"; }
	static const char* path() { return "Civ4FeatureInfos/FeatureInfos/FeatureInfo"; }
};

template <>
struct xmlLocation<ImprovementTypes>
{
	static const char* file() { return "CIV4ImprovementInfos"; }
	static const char* folder() { return "Terrain"; }
	static const char* path() { return "Civ4ImprovementInfos/ImprovementInfos/ImprovementInfo"; }
};

template <>
struct xmlLocation<TerrainTypes>
{
	static const char* file() { return "CIV4TerrainInfos"; }
	static const char* folder() { return "Terrain"; }
	static const char* path() { return "Civ4TerrainInfos/TerrainInfos/TerrainInfo"; }
};

template <>
struct xmlLocation<LandscapeTypes>
{
	static const char* file() { return "CIV4TerrainSettings"; }
	static const char* folder() { return "Terrain"; }
	static const char* path() { return "Civ4TerrainSettings/LandscapeInfos/LandscapeInfo"; }
};

template <>
struct xmlLocation<YieldTypes>
{
	static const char* file() { return "CIV4YieldInfos"; }
	static const char* folder() { return "Terrain"; }
	static const char* path() { return "Civ4YieldInfos/YieldInfos/YieldInfo"; }
};

// Units

template <>
struct xmlLocation<AnimationTypes>
{
	static const char* file() { return "CIV4AnimationInfos"; }
	static const char* folder() { return "Units"; }
	static const char* path() { return "Civ4AnimationInfos/AnimationCategories/AnimationCategory"; }
};

template <>
struct xmlLocation<AnimationPathTypes>
{
	static const char* file() { return "CIV4AnimationPathInfos"; }
	static const char* folder() { return "Units"; }
	static const char* path() { return "Civ4AnimationPathInfos/AnimationPaths/AnimationPath"; }
};

template <>
struct xmlLocation<AutomateTypes>
{
	static const char* file() { return "CIV4AutomateInfos"; }
	static const char* folder() { return "Units"; }
	static const char* path() { return "Civ4AutomateInfos/AutomateInfos/AutomateInfo"; }
};

template <>
struct xmlLocation<BuildTypes>
{
	static const char* file() { return "CIV4BuildInfos"; }
	static const char* folder() { return "Units"; }
	static const char* path() { return "Civ4BuildInfos/BuildInfos/BuildInfo"; }
};

template <>
struct xmlLocation<CommandTypes>
{
	static const char* file() { return "CIV4CommandInfos"; }
	static const char* folder() { return "Units"; }
	static const char* path() { return "Civ4CommandInfos/CommandInfos/CommandInfo"; }
};

template <>
struct xmlLocation<ControlTypes>
{
	static const char* file() { return "CIV4ControlInfos"; }
	static const char* folder() { return "Units"; }
	static const char* path() { return "Civ4ControlInfos/ControlInfos/ControlInfo"; }
};

template <>
struct xmlLocation<EntityEventTypes>
{
	static const char* file() { return "CIV4EntityEventInfos"; }
	static const char* folder() { return "Units"; }
	static const char* path() { return "Civ4EntityEventInfos/EntityEventInfos/EntityEventInfo"; }
};

template <>
struct xmlLocation<FormationTypes>
{
	static const char* file() { return "CIV4FormationInfos"; }
	static const char* folder() { return "Units"; }
	static const char* path() { return "UnitFormations/UnitFormation"; }
};

template <>
struct xmlLocation<MissionTypes>
{
	static const char* file() { return "CIV4MissionInfos"; }
	static const char* folder() { return "Units"; }
	static const char* path() { return "Civ4MissionInfos/MissionInfos/MissionInfo"; }
};

template <>
struct xmlLocation<ProfessionTypes>
{
	static const char* file() { return "CIV4ProfessionInfos"; }
	static const char* folder() { return "Units"; }
	static const char* path() { return "Civ4ProfessionInfos/ProfessionInfos/ProfessionInfo"; }
};

template <>
struct xmlLocation<PromotionTypes>
{
	static const char* file() { return "CIV4PromotionInfos"; }
	static const char* folder() { return "Units"; }
	static const char* path() { return "Civ4PromotionInfos/PromotionInfos/PromotionInfo"; }
};

template <>
struct xmlLocation<SpecialUnitTypes>
{
	static const char* file() { return "CIV4SpecialUnitInfos"; }
	static const char* folder() { return "Units"; }
	static const char* path() { return "Civ4SpecialUnitInfos/SpecialUnitInfos/SpecialUnitInfo"; }
};

template <>
struct xmlLocation<UnitTypes>
{
	static const char* file() { return "CIV4UnitInfos"; }
	static const char* folder() { return "Units"; }
	static const char* path() { return "Civ4UnitInfos/UnitInfos/UnitInfo"; }
};

template <>
struct xmlLocation<UnitClassTypes>
{
	static const char* file() { return "CIV4UnitClassInfos"; }
	static const char* folder() { return "Units"; }
	static const char* path() { return "Civ4UnitClassInfos/UnitClassInfos/UnitClassInfo"; }
};


template <typename T>
struct xmlLocation
{
	// shouldn't be able to use one, which isn't mentioned in this file
	BOOST_STATIC_ASSERT(0);
};
