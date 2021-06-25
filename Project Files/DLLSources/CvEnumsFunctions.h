#pragma once

#ifndef CVENUMS_FUNCTIONS_h
#define CVENUMS_FUNCTIONS_h

// EnumsFunctions.h

#include "CvGameCoreDLL.h"
#include "CvEnums.h"
#include "EnumMap.h"
#include "InfoArray.h"

template <typename T>
struct VARINFO {}; // intentionally left blank. Use specialized structs only


#define ENUM_STRUCT_DECLARATION(ENUM_NAME, JIT_TYPE) \
template <> struct VARINFO<ENUM_NAME> \
{ \
	static const JITarrayTypes JIT = JIT_TYPE; \
};


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

#define SET_ENUM_VARIABLE_EXPANDED(type, get, jit, max, maxCompile) \
	SET_ENUM_OPERATORS(type) \
	ENUM_STRUCT_DECLARATION(type, jit) \
	SET_ARRAY_XML_ENUM(type, max, jit, maxCompile) \
	INFO_ARRAY_GET(type, get, jit)

#define SET_ENUM_VARIABLE_AND_FORBID_EXPANDED(type, get, jit, max, maxCompile) \
	SET_ENUM_OPERATORS_AND_FORBID(type) \
	ENUM_STRUCT_DECLARATION(type, jit) \
	SET_ARRAY_XML_ENUM(type, max, jit, maxCompile) \
	INFO_ARRAY_GET(type, get, jit)


#define SET_ENUM_VARIABLE(lower, upper) \
	SET_ENUM_VARIABLE_EXPANDED(##lower##Types, get##lower, JIT_ARRAY_##upper, NUM_##upper##_TYPES, COMPILE_TIME_NUM_##upper##_TYPES)

#define SET_ENUM_VARIABLE_AND_FORBID(lower, upper) \
	SET_ENUM_VARIABLE_AND_FORBID_EXPANDED(##lower##Types, get##lower, JIT_ARRAY_##upper, NUM_##upper##_TYPES, COMPILE_TIME_NUM_##upper##_TYPES)

// disable the check for a specific enum type in a single file
#define DEFINE_ENUM_INT_COMPARISON(EnumName)       \
	UNFORBID_COMPARISON_OPERATORS(EnumName, int)   \
	UNFORBID_COMPARISON_OPERATORS(int, EnumName)

template<class T>
static bool validEnumRange(T eValue)
{
	return eValue >= (T)0 && eValue < (T)getArrayLength(getJITarrayType(eValue));
}

// enum operators (with JIT types)

#define JIT_ARRAY_SPECIALBUILDING JIT_ARRAY_BUILDING_SPECIAL 
#define CultureTypes CultureLevelTypes
#define JIT_ARRAY_CULTURELEVEL JIT_ARRAY_CULTURE
#define JIT_ARRAY_EVENTTRIGGER JIT_ARRAY_EVENT_TRIGGER
#define JIT_ARRAY_GAMEOPTION JIT_ARRAY_GAME_OPTION
#define JIT_ARRAY_GAMESPEED JIT_ARRAY_GAME_SPEED
#define JIT_ARRAY_PLAYERCOLOR JIT_ARRAY_PLAYER_COLOR
#define JIT_ARRAY_PLAYEROPTION JIT_ARRAY_PLAYER_OPTION
#define JIT_ARRAY_SEALEVEL JIT_ARRAY_SEA_LEVEL
#define JIT_ARRAY_SPECIALUNIT JIT_ARRAY_UNIT_SPECIAL
#define JIT_ARRAY_WORLDSIZE JIT_ARRAY_WORLD_SIZE

SET_ENUM_VARIABLE(Achieve, ACHIEVE)
SET_ENUM_VARIABLE(ArtStyle, ARTSTYLE)
SET_ENUM_VARIABLE(Bonus, BONUS)
SET_ENUM_VARIABLE(Build, BUILD)
SET_ENUM_VARIABLE(Building, BUILDING)
SET_ENUM_VARIABLE(BuildingClass, BUILDINGCLASS)
SET_ENUM_VARIABLE(SpecialBuilding, SPECIALBUILDING)
SET_ENUM_VARIABLE(CivCategory, CIV_CATEGORY)
SET_ENUM_VARIABLE(CivEffect, CIV_EFFECT)
SET_ENUM_VARIABLE(Civic, CIVIC)
SET_ENUM_VARIABLE(CivicOption, CIVICOPTION)
SET_ENUM_VARIABLE(Civilization, CIVILIZATION)
SET_ENUM_VARIABLE(Climate, CLIMATE)
SET_ENUM_VARIABLE(Color, COLOR)
SET_ENUM_VARIABLE(Contact, CONTACT)
SET_ENUM_VARIABLE(Culture, CULTURELEVEL)
SET_ENUM_VARIABLE_AND_FORBID(Diplomacy, DIPLOMACY)
SET_ENUM_VARIABLE(Domain, DOMAIN)
SET_ENUM_VARIABLE(Emotion, EMOTION)
SET_ENUM_VARIABLE(Era, ERA)
SET_ENUM_VARIABLE(Emphasize, EMPHASIZE)
SET_ENUM_VARIABLE(Europe, EUROPE)
SET_ENUM_VARIABLE(Event, EVENT)
SET_ENUM_VARIABLE(EventTrigger, EVENTTRIGGER)
SET_ENUM_VARIABLE(Father, FATHER)
SET_ENUM_VARIABLE(FatherPoint, FATHER_POINT)
SET_ENUM_VARIABLE(Feat, FEAT)
SET_ENUM_VARIABLE(Feature, FEATURE)
SET_ENUM_VARIABLE(GameOption, GAMEOPTION)
SET_ENUM_VARIABLE(GameSpeed, GAMESPEED)
SET_ENUM_VARIABLE(Goody, GOODY)
SET_ENUM_VARIABLE(Handicap, HANDICAP)
SET_ENUM_VARIABLE(Hurry, HURRY)
SET_ENUM_VARIABLE(Improvement, IMPROVEMENT)
SET_ENUM_VARIABLE(Invisible, INVISIBLE)
SET_ENUM_VARIABLE(LeaderHead, LEADER)
SET_ENUM_VARIABLE(Memory, MEMORY)
SET_ENUM_VARIABLE(Player, PLAYER)
SET_ENUM_VARIABLE(PlayerColor, PLAYERCOLOR)
SET_ENUM_VARIABLE(PlayerOption, PLAYEROPTION)
SET_ENUM_VARIABLE_EXPANDED(PlotTypes, getPlotType, JIT_ARRAY_PLOT_TYPE, NUM_PLOT_TYPES, NUM_PLOT_TYPES)
SET_ENUM_VARIABLE(Profession, PROFESSION)
SET_ENUM_VARIABLE(Promotion, PROMOTION)
SET_ENUM_VARIABLE(Route, ROUTE)
SET_ENUM_VARIABLE(SeaLevel, SEALEVEL)
SET_ENUM_VARIABLE(Strategy, STRATEGY)
SET_ENUM_VARIABLE(Team, TEAM)
SET_ENUM_VARIABLE(Terrain, TERRAIN)
SET_ENUM_VARIABLE_AND_FORBID(Trait, TRAIT)
SET_ENUM_VARIABLE(Unit, UNIT)
SET_ENUM_VARIABLE(UnitAI, UNITAI)
SET_ENUM_VARIABLE(UnitClass, UNITCLASS)
SET_ENUM_VARIABLE(UnitCombat, UNITCOMBAT)
SET_ENUM_VARIABLE(SpecialUnit, SPECIALUNIT)
SET_ENUM_VARIABLE(Victory, VICTORY)
SET_ENUM_VARIABLE(WorldSize, WORLDSIZE)
SET_ENUM_VARIABLE(Yield, YIELD)

#undef JIT_ARRAY_SPECIALBUILDING 
#undef CultureTypes
#undef JIT_ARRAY_CULTURELEVEL
#undef JIT_ARRAY_EVENTTRIGGER
#undef JIT_ARRAY_GAMEOPTION
#undef JIT_ARRAY_GAMESPEED
#undef JIT_ARRAY_PLAYERCOLOR
#undef JIT_ARRAY_PLAYEROPTION
#undef JIT_ARRAY_SEALEVEL
#undef JIT_ARRAY_SPECIALUNIT
#undef JIT_ARRAY_WORLDSIZE



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
SET_ENUM_OPERATORS_AND_FORBID(AttachableTypes);
SET_ENUM_OPERATORS(AttitudeTypes);
SET_ENUM_OPERATORS_AND_FORBID(AudioTag);
SET_ENUM_OPERATORS(AutomateTypes);
SET_ENUM_OPERATORS_AND_FORBID(AxisTypes);
SET_ENUM_OPERATORS_AND_FORBID(ButtonPopupTypes);
SET_ENUM_OPERATORS_AND_FORBID(ButtonStyles);
SET_ENUM_OPERATORS(CalendarTypes);
SET_ENUM_OPERATORS_AND_FORBID(CameraLookAtTypes);
SET_ENUM_OPERATORS_AND_FORBID(CameraMovementSpeeds);
SET_ENUM_OPERATORS_AND_FORBID(CameraOverlayTypes);
SET_ENUM_OPERATORS(CardinalDirectionTypes);
SET_ENUM_OPERATORS_AND_FORBID(ChatTargetTypes);
SET_ENUM_OPERATORS_AND_FORBID(CivilopediaPageTypes);
SET_ENUM_OPERATORS_AND_FORBID(CivLoginStates);
SET_ENUM_OPERATORS(CityPlotTypes);
SET_ENUM_OPERATORS_AND_FORBID(CitySizeTypes);
SET_ENUM_OPERATORS(CommandTypes);
SET_ENUM_OPERATORS(ConceptTypes);
SET_ENUM_OPERATORS(ControlTypes);
SET_ENUM_OPERATORS(CursorTypes);
SET_ENUM_OPERATORS_AND_FORBID(CustomMapOptionTypes);
SET_ENUM_OPERATORS(DenialTypes);
SET_ENUM_OPERATORS_AND_FORBID(DiploCommentTypes);
SET_ENUM_OPERATORS_AND_FORBID(DiploEventTypes);
SET_ENUM_OPERATORS(DiplomacyPowerTypes);
SET_ENUM_OPERATORS(DirectionTypes);
SET_ENUM_OPERATORS(EffectTypes);
SET_ENUM_OPERATORS_AND_FORBID(EndTurnButtonStates);
SET_ENUM_OPERATORS_AND_FORBID(EngineDirtyBits);
SET_ENUM_OPERATORS(EntityEventTypes);
SET_ENUM_OPERATORS_AND_FORBID(EventContextTypes);
SET_ENUM_OPERATORS(FatherCategoryTypes);
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
SET_ENUM_OPERATORS_AND_FORBID(GamePwdTypes);
SET_ENUM_OPERATORS_AND_FORBID(GameStateTypes);
SET_ENUM_OPERATORS_AND_FORBID(GameType);
SET_ENUM_OPERATORS_AND_FORBID(GenericButtonSizes);
SET_ENUM_OPERATORS_AND_FORBID(GraphicLevelTypes);
SET_ENUM_OPERATORS(GraphicOptionTypes);
SET_ENUM_OPERATORS_AND_FORBID(HealthBarTypes);
SET_ENUM_OPERATORS_AND_FORBID(HelpTextTypes);
SET_ENUM_OPERATORS_AND_FORBID(HitTestTypes);
SET_ENUM_OPERATORS_AND_FORBID(InitializationStates);
SET_ENUM_OPERATORS_AND_FORBID(ImageShapes);
SET_ENUM_OPERATORS_AND_FORBID(InfoBarTypes);
SET_ENUM_OPERATORS_AND_FORBID(InterfaceDirtyBits);
SET_ENUM_OPERATORS_AND_FORBID(InterfaceMessageTypes);
SET_ENUM_OPERATORS(InterfaceModeTypes);
SET_ENUM_OPERATORS_AND_FORBID(InterfaceVisibility);
SET_ENUM_OPERATORS_AND_FORBID(InitStates);
SET_ENUM_OPERATORS(JITarrayTypes);
SET_ENUM_OPERATORS_AND_FORBID(JustificationTypes);
SET_ENUM_OPERATORS_AND_FORBID(LeaderheadAction);
SET_ENUM_OPERATORS_AND_FORBID(LoadType);
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
SET_ENUM_OPERATORS_AND_FORBID(PlotIndicatorVisibilityFlags);
SET_ENUM_OPERATORS_AND_FORBID(PlotLandscapeLayers);
SET_ENUM_OPERATORS_AND_FORBID(PlotStyles);
SET_ENUM_OPERATORS_AND_FORBID(PopupControlLayout);
SET_ENUM_OPERATORS_AND_FORBID(PopupEventTypes);
SET_ENUM_OPERATORS_AND_FORBID(PopupStates);
SET_ENUM_OPERATORS_AND_FORBID(ProbabilityTypes);
SET_ENUM_OPERATORS_AND_FORBID(ReplayMessageTypes);
SET_ENUM_OPERATORS_AND_FORBID(ResourceLayerOptions);
SET_ENUM_OPERATORS_AND_FORBID(RotationTypes);
SET_ENUM_OPERATORS_AND_FORBID(SaveGameTypes);
SET_ENUM_OPERATORS(SeasonTypes);
SET_ENUM_OPERATORS_AND_FORBID(SlotClaim);
SET_ENUM_OPERATORS_AND_FORBID(SlotStatus);
SET_ENUM_OPERATORS_AND_FORBID(TabGroupTypes);
SET_ENUM_OPERATORS_AND_FORBID(TableStyles);
SET_ENUM_OPERATORS_AND_FORBID(TaskTypes);
SET_ENUM_OPERATORS_AND_FORBID(TerrainGroupTypes);
SET_ENUM_OPERATORS_AND_FORBID(TileArtTypes);
SET_ENUM_OPERATORS_AND_FORBID(ToolTipAlignTypes);
SET_ENUM_OPERATORS(TradeableItems);
SET_ENUM_OPERATORS_AND_FORBID(TradeMessageTypes);
SET_ENUM_OPERATORS_AND_FORBID(TradeScreenTypes);
SET_ENUM_OPERATORS(TurnTimerTypes);
SET_ENUM_OPERATORS(UnitAIStates);
SET_ENUM_OPERATORS(UnitArtStyleTypes);
SET_ENUM_OPERATORS_AND_FORBID(UnitLayerOptionTypes);
SET_ENUM_OPERATORS(UnitTravelStates);
SET_ENUM_OPERATORS_AND_FORBID(VersionTypes);
SET_ENUM_OPERATORS_AND_FORBID(VoiceTargetTypes);
SET_ENUM_OPERATORS_AND_FORBID(VoteResultTypes);
SET_ENUM_OPERATORS_AND_FORBID(VoteStatusTypes);
SET_ENUM_OPERATORS(WarPlanTypes);
SET_ENUM_OPERATORS_AND_FORBID(WidgetTypes);
SET_ENUM_OPERATORS_AND_FORBID(WrapDirection);
SET_ENUM_OPERATORS_AND_FORBID(WorldBuilderPopupTypes);
SET_ENUM_OPERATORS_AND_FORBID(ZoomLevelTypes);



#define DECLARE_CUSTOM_INT(type, getType) \
	ENUM_STRUCT_DECLARATION(type, JIT_ARRAY_INT) \
	INFO_ARRAY_GET_INT(type, getType, JIT_ARRAY_INT)

DECLARE_CUSTOM_INT(int, getInt);
DECLARE_CUSTOM_INT(short, getInt);

ENUM_STRUCT_DECLARATION(JIT_NoneTypes, JIT_ARRAY_NO_TYPE);


#endif	// CVENUMS_h
