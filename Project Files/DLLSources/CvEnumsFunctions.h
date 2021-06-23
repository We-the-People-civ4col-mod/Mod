#pragma once

#ifndef CVENUMS_FUNCTIONS_h
#define CVENUMS_FUNCTIONS_h

// EnumsFunctions.h

#include "CvEnums.h"
#include "EnumMap.h"
#include "InfoArray.h"

// overloaded ++ for enum types

// copied concept from AdvCiv
// cause linker error if enum types are mixed
// unlike AdvCiv, this forbids all types, not just specific ones
// also mixing int and enums is a no go


#define VARIABLE_COMPARISON_CHECK_IMPLEMENTATION(varA, varB, operatorStr) \
template <typename T>                                                     \
static inline bool operator operatorStr (varA a, varB b)                  \
{                                                                         \
	const bool bTypeCheck = boost::is_same<varA, varB>::value;            \
	BOOST_STATIC_ASSERT(bTypeCheck);                                      \
	return (int)a operatorStr (int)b;                                     \
}

#define VARIABLE_COMPARISON_CHECK(variable)                               \
	VARIABLE_COMPARISON_CHECK_IMPLEMENTATION(variable, T, ==)             \
	VARIABLE_COMPARISON_CHECK_IMPLEMENTATION(T, variable, ==)             \
	VARIABLE_COMPARISON_CHECK_IMPLEMENTATION(variable, T, !=)             \
	VARIABLE_COMPARISON_CHECK_IMPLEMENTATION(T, variable, !=)             \
	VARIABLE_COMPARISON_CHECK_IMPLEMENTATION(variable, T, > )             \
	VARIABLE_COMPARISON_CHECK_IMPLEMENTATION(T, variable, > )             \
	VARIABLE_COMPARISON_CHECK_IMPLEMENTATION(variable, T, < )             \
	VARIABLE_COMPARISON_CHECK_IMPLEMENTATION(T, variable, < )             \
	VARIABLE_COMPARISON_CHECK_IMPLEMENTATION(variable, T, >=)             \
	VARIABLE_COMPARISON_CHECK_IMPLEMENTATION(T, variable, >=)             \
	VARIABLE_COMPARISON_CHECK_IMPLEMENTATION(variable, T, <=)             \
	VARIABLE_COMPARISON_CHECK_IMPLEMENTATION(T, variable, <=)    



// unforbidding is for when you for whatever reason want to disable the strict type check in a file
// use with care. It's a quick solution, but it does reduce the compiler's ability to detect bugs
// this might be needed if there is a lot of templates or similar
#define UNFORBID_COMPARISON_OPERATORS(Type1, Type2) \
	static inline bool operator==(Type1 a, Type2 b) {return (int)a == (int)b;} \
	static inline bool operator!=(Type1 a, Type2 b) {return (int)a != (int)b;} \
	static inline bool operator> (Type1 a, Type2 b) {return (int)a >  (int)b;} \
	static inline bool operator< (Type1 a, Type2 b) {return (int)a <  (int)b;} \
	static inline bool operator>=(Type1 a, Type2 b) {return (int)a >= (int)b;} \
	static inline bool operator<=(Type1 a, Type2 b) {return (int)a <= (int)b;} \



#define SET_ENUM_OPERATORS(EnumName)                  \
/* prefix */                                          \
static inline EnumName& operator++(EnumName& c)       \
{                                                     \
	c = (EnumName)(c + 1);                            \
	return c;                                         \
}                                                     \
/* postfix */                                         \
static inline EnumName operator++(EnumName& c, int)   \
{                                                     \
	EnumName cache = c;                               \
	c = (EnumName)(c + 1);                            \
	return cache;                                     \
}                                                     \
/* prefix */                                          \
static inline EnumName& operator--(EnumName& c)       \
{                                                     \
	c = (EnumName)(c - 1);                            \
	return c;                                         \
}                                                     \
/* postfix */                                         \
static inline EnumName operator--(EnumName& c, int)   \
{                                                     \
	EnumName cache = c;                               \
	c = (EnumName)(c - 1);                            \
	return cache;                                     \
}                                                     \
static inline EnumName operator+(const EnumName& A, const EnumName& B)     \
{                                                                          \
return (EnumName)((int)A + (int)B);                                        \
}                                                                          \
static inline EnumName operator-(const EnumName& A, const EnumName& B)     \
{                                                                          \
return (EnumName)((int)A - (int)B);                                        \
}

#define SET_ENUM_OPERATORS_AND_FORBID(EnumName)       \
	VARIABLE_COMPARISON_CHECK(EnumName)               \
	SET_ENUM_OPERATORS(EnumName)



SET_ENUM_OPERATORS(AchieveTypes);
SET_ENUM_OPERATORS_AND_FORBID(ActionSubTypes);
SET_ENUM_OPERATORS_AND_FORBID(ActivationTypes);
SET_ENUM_OPERATORS_AND_FORBID(ActivityTypes);
SET_ENUM_OPERATORS_AND_FORBID(AdvancedStartActionTypes);
SET_ENUM_OPERATORS(AlarmTypes);
SET_ENUM_OPERATORS(AnimationCategoryTypes);
SET_ENUM_OPERATORS(AnimationPathTypes);
SET_ENUM_OPERATORS(AnimationOperatorTypes);
SET_ENUM_OPERATORS_AND_FORBID(AnimationTypes);
SET_ENUM_OPERATORS_AND_FORBID(AreaBorderLayers);
SET_ENUM_OPERATORS(AreaAITypes);
SET_ENUM_OPERATORS(ArtStyleTypes);
SET_ENUM_OPERATORS_AND_FORBID(AttachableTypes);
SET_ENUM_OPERATORS(AttitudeTypes);
SET_ENUM_OPERATORS_AND_FORBID(AudioTag);
SET_ENUM_OPERATORS(AutomateTypes);
SET_ENUM_OPERATORS_AND_FORBID(AxisTypes);
SET_ENUM_OPERATORS(BonusTypes);
SET_ENUM_OPERATORS_AND_FORBID(ButtonPopupTypes);
SET_ENUM_OPERATORS_AND_FORBID(ButtonStyles);
SET_ENUM_OPERATORS(BuildingClassTypes);
SET_ENUM_OPERATORS(BuildingTypes);
SET_ENUM_OPERATORS(BuildTypes);
SET_ENUM_OPERATORS(CalendarTypes);
SET_ENUM_OPERATORS_AND_FORBID(CameraLookAtTypes);
SET_ENUM_OPERATORS_AND_FORBID(CameraMovementSpeeds);
SET_ENUM_OPERATORS_AND_FORBID(CameraOverlayTypes);
SET_ENUM_OPERATORS(CardinalDirectionTypes);
SET_ENUM_OPERATORS_AND_FORBID(ChatTargetTypes);
SET_ENUM_OPERATORS(CivEffectTypes);
SET_ENUM_OPERATORS(CivicOptionTypes);
SET_ENUM_OPERATORS(CivicTypes);
SET_ENUM_OPERATORS(CivilizationTypes);
SET_ENUM_OPERATORS_AND_FORBID(CivilopediaPageTypes);
SET_ENUM_OPERATORS_AND_FORBID(CivLoginStates);
SET_ENUM_OPERATORS(CityPlotTypes);
SET_ENUM_OPERATORS_AND_FORBID(CitySizeTypes);
SET_ENUM_OPERATORS(ClimateTypes);
SET_ENUM_OPERATORS(ColorTypes);
SET_ENUM_OPERATORS(CommandTypes);
SET_ENUM_OPERATORS(ConceptTypes);
SET_ENUM_OPERATORS(ContactTypes);
SET_ENUM_OPERATORS(ControlTypes);
SET_ENUM_OPERATORS(CultureLevelTypes);
SET_ENUM_OPERATORS(CursorTypes);
SET_ENUM_OPERATORS_AND_FORBID(CustomMapOptionTypes);
SET_ENUM_OPERATORS(DenialTypes);
SET_ENUM_OPERATORS_AND_FORBID(DiploCommentTypes);
SET_ENUM_OPERATORS_AND_FORBID(DiploEventTypes);
SET_ENUM_OPERATORS(DiplomacyPowerTypes);
SET_ENUM_OPERATORS(DiplomacyTypes);
SET_ENUM_OPERATORS(DirectionTypes);
SET_ENUM_OPERATORS(DomainTypes);
SET_ENUM_OPERATORS(EffectTypes);
SET_ENUM_OPERATORS(EmotionTypes);
SET_ENUM_OPERATORS(EmphasizeTypes);
SET_ENUM_OPERATORS_AND_FORBID(EndTurnButtonStates);
SET_ENUM_OPERATORS_AND_FORBID(EngineDirtyBits);
SET_ENUM_OPERATORS(EntityEventTypes);
SET_ENUM_OPERATORS(EraTypes);
SET_ENUM_OPERATORS(EuropeTypes);
SET_ENUM_OPERATORS_AND_FORBID(EventContextTypes);
SET_ENUM_OPERATORS(EventTriggerTypes);
SET_ENUM_OPERATORS(EventTypes);
SET_ENUM_OPERATORS(FatherCategoryTypes);
SET_ENUM_OPERATORS(FatherPointTypes);
SET_ENUM_OPERATORS(FatherTypes);
SET_ENUM_OPERATORS(FeatureTypes);
SET_ENUM_OPERATORS(FeatTypes);
SET_ENUM_OPERATORS_AND_FORBID(FlyoutTypes);
SET_ENUM_OPERATORS_AND_FORBID(FogOfWarModeTypes);
SET_ENUM_OPERATORS_AND_FORBID(FogTypes);
SET_ENUM_OPERATORS(FontSymbols);
SET_ENUM_OPERATORS_AND_FORBID(FontTypes);
SET_ENUM_OPERATORS_AND_FORBID(FOWUpdateTypes);
SET_ENUM_OPERATORS(ForceControlTypes);
SET_ENUM_OPERATORS_AND_FORBID(FootstepAudioTags);
SET_ENUM_OPERATORS_AND_FORBID(FootstepAudioTypes);
SET_ENUM_OPERATORS(FunctionTypes);
SET_ENUM_OPERATORS(GameMessageTypes);
SET_ENUM_OPERATORS_AND_FORBID(GameMode);
SET_ENUM_OPERATORS(GameOptionTypes);
SET_ENUM_OPERATORS_AND_FORBID(GamePwdTypes);
SET_ENUM_OPERATORS(GameSpeedTypes);
SET_ENUM_OPERATORS_AND_FORBID(GameStateTypes);
SET_ENUM_OPERATORS_AND_FORBID(GameType);
SET_ENUM_OPERATORS_AND_FORBID(GenericButtonSizes);
SET_ENUM_OPERATORS(GoodyTypes);
SET_ENUM_OPERATORS_AND_FORBID(GraphicLevelTypes);
SET_ENUM_OPERATORS(GraphicOptionTypes);
SET_ENUM_OPERATORS(HandicapTypes);
SET_ENUM_OPERATORS_AND_FORBID(HealthBarTypes);
SET_ENUM_OPERATORS_AND_FORBID(HelpTextTypes);
SET_ENUM_OPERATORS_AND_FORBID(HitTestTypes);
SET_ENUM_OPERATORS(HurryTypes);
SET_ENUM_OPERATORS_AND_FORBID(InitializationStates);
SET_ENUM_OPERATORS_AND_FORBID(ImageShapes);
SET_ENUM_OPERATORS(ImprovementTypes);
SET_ENUM_OPERATORS_AND_FORBID(InfoBarTypes);
SET_ENUM_OPERATORS_AND_FORBID(InterfaceDirtyBits);
SET_ENUM_OPERATORS_AND_FORBID(InterfaceMessageTypes);
SET_ENUM_OPERATORS(InterfaceModeTypes);
SET_ENUM_OPERATORS_AND_FORBID(InterfaceVisibility);
SET_ENUM_OPERATORS(InvisibleTypes);
SET_ENUM_OPERATORS_AND_FORBID(InitStates);
SET_ENUM_OPERATORS(JITarrayTypes);
SET_ENUM_OPERATORS_AND_FORBID(JustificationTypes);
SET_ENUM_OPERATORS_AND_FORBID(LeaderheadAction);
SET_ENUM_OPERATORS(LeaderHeadTypes);
SET_ENUM_OPERATORS_AND_FORBID(LoadType);
SET_ENUM_OPERATORS(MemoryTypes);
SET_ENUM_OPERATORS_AND_FORBID(MenuScreenType);
SET_ENUM_OPERATORS_AND_FORBID(MinimapModeTypes);
SET_ENUM_OPERATORS_AND_FORBID(MissionAITypes);
SET_ENUM_OPERATORS(MissionTypes);
SET_ENUM_OPERATORS(MonthTypes);
SET_ENUM_OPERATORS(MultiplayerOptionTypes);
SET_ENUM_OPERATORS_AND_FORBID(NetContactTypes);
SET_ENUM_OPERATORS_AND_FORBID(OrderTypes);
SET_ENUM_OPERATORS_AND_FORBID(PanelStyles);
SET_ENUM_OPERATORS_AND_FORBID(PlayerActionTypes);
SET_ENUM_OPERATORS(PlayerColorTypes);
SET_ENUM_OPERATORS(PlayerOptionTypes);
SET_ENUM_OPERATORS(PlayerTypes);
SET_ENUM_OPERATORS_AND_FORBID(PlotIndicatorVisibilityFlags);
SET_ENUM_OPERATORS_AND_FORBID(PlotLandscapeLayers);
SET_ENUM_OPERATORS_AND_FORBID(PlotStyles);
SET_ENUM_OPERATORS(PlotTypes);
SET_ENUM_OPERATORS_AND_FORBID(PopupControlLayout);
SET_ENUM_OPERATORS_AND_FORBID(PopupEventTypes);
SET_ENUM_OPERATORS_AND_FORBID(PopupStates);
SET_ENUM_OPERATORS_AND_FORBID(ProbabilityTypes);
SET_ENUM_OPERATORS(ProfessionTypes);
SET_ENUM_OPERATORS(PromotionTypes);
SET_ENUM_OPERATORS_AND_FORBID(ReplayMessageTypes);
SET_ENUM_OPERATORS_AND_FORBID(ResourceLayerOptions);
SET_ENUM_OPERATORS_AND_FORBID(RotationTypes);
SET_ENUM_OPERATORS(RouteTypes);
SET_ENUM_OPERATORS_AND_FORBID(SaveGameTypes);
SET_ENUM_OPERATORS(SeaLevelTypes);
SET_ENUM_OPERATORS(SeasonTypes);
SET_ENUM_OPERATORS_AND_FORBID(SlotClaim);
SET_ENUM_OPERATORS_AND_FORBID(SlotStatus);
SET_ENUM_OPERATORS(SpecialBuildingTypes);
SET_ENUM_OPERATORS(SpecialUnitTypes);
SET_ENUM_OPERATORS(StrategyTypes);
SET_ENUM_OPERATORS_AND_FORBID(TabGroupTypes);
SET_ENUM_OPERATORS_AND_FORBID(TableStyles);
SET_ENUM_OPERATORS_AND_FORBID(TaskTypes);
SET_ENUM_OPERATORS(TeamTypes);
SET_ENUM_OPERATORS_AND_FORBID(TerrainGroupTypes);
SET_ENUM_OPERATORS(TerrainTypes);
SET_ENUM_OPERATORS_AND_FORBID(TileArtTypes);
SET_ENUM_OPERATORS_AND_FORBID(ToolTipAlignTypes);
SET_ENUM_OPERATORS(TradeableItems);
SET_ENUM_OPERATORS_AND_FORBID(TradeMessageTypes);
SET_ENUM_OPERATORS_AND_FORBID(TradeScreenTypes);
SET_ENUM_OPERATORS_AND_FORBID(TraitTypes);
SET_ENUM_OPERATORS(TurnTimerTypes);
SET_ENUM_OPERATORS(UnitAIStates);
SET_ENUM_OPERATORS(UnitAITypes);
SET_ENUM_OPERATORS(UnitArtStyleTypes);
SET_ENUM_OPERATORS(UnitClassTypes);
SET_ENUM_OPERATORS(UnitCombatTypes);
SET_ENUM_OPERATORS_AND_FORBID(UnitLayerOptionTypes);
SET_ENUM_OPERATORS(UnitTravelStates);
SET_ENUM_OPERATORS(UnitTypes);
SET_ENUM_OPERATORS_AND_FORBID(VersionTypes);
SET_ENUM_OPERATORS(VictoryTypes);
SET_ENUM_OPERATORS_AND_FORBID(VoiceTargetTypes);
SET_ENUM_OPERATORS_AND_FORBID(VoteResultTypes);
SET_ENUM_OPERATORS_AND_FORBID(VoteStatusTypes);
SET_ENUM_OPERATORS(WarPlanTypes);
SET_ENUM_OPERATORS_AND_FORBID(WidgetTypes);
SET_ENUM_OPERATORS_AND_FORBID(WrapDirection);
SET_ENUM_OPERATORS_AND_FORBID(WorldBuilderPopupTypes);
SET_ENUM_OPERATORS(WorldSizeTypes);
SET_ENUM_OPERATORS(YieldTypes);
SET_ENUM_OPERATORS_AND_FORBID(ZoomLevelTypes);

// disable the check for a specific enum type in a single file
#define DEFINE_ENUM_INT_COMPARISON(EnumName)       \
	UNFORBID_COMPARISON_OPERATORS(EnumName, int)   \
	UNFORBID_COMPARISON_OPERATORS(int, EnumName)

template<class T>
static bool validEnumRange(T eValue)
{
	return eValue >= (T)0 && eValue < (T)getArrayLength(getJITarrayType(eValue));
}

#endif	// CVENUMS_h
