//
// globals.cpp
// Author -	Mustafa Thamer
//
#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "CvRandom.h"
#include "CvGameAI.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvMap.h"
#include "CvPlayerAI.h"
#include "CvTeamAI.h"
#include "CvInfos.h"
#include "CvDLLUtilityIFaceBase.h"
#include "CvArtFileMgr.h"
#include "CvDLLXMLIFaceBase.h"
#include "CvPlayerAI.h"
#include "CvInfoWater.h"
#include "CvGameTextMgr.h"
#include "FProfiler.h"
#include "FVariableSystem.h"
#include "CvInitCore.h"

#define COPY(dst, src, typeName) \
	{ \
		int iNum = sizeof(src)/sizeof(typeName); \
		dst = new typeName[iNum]; \
		for (int i =0;i<iNum;i++) \
			dst[i] = src[i]; \
	}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

CvGlobals gGlobals;

//
// CONSTRUCTOR
//
CvGlobals::CvGlobals() :
m_bGraphicsInitialized(false),
m_bLogging(false),
m_bRandLogging(false),
m_bOverwriteLogs(false),
m_bSynchLogging(false),
m_pkMainMenu(NULL),
m_iNewPlayers(0),
m_bZoomOut(false),
m_bZoomIn(false),
m_bLoadGameFromFile(false),
m_pFMPMgr(NULL),
m_asyncRand(NULL),
m_interface(NULL),
m_game(NULL),
m_messageQueue(NULL),
m_hotJoinMsgQueue(NULL),
m_messageControl(NULL),
m_messageCodes(NULL),
m_dropMgr(NULL),
m_portal(NULL),
m_setupData(NULL),
m_initCore(NULL),
m_statsReporter(NULL),
m_map(NULL),
m_diplomacyScreen(NULL),
m_mpDiplomacyScreen(NULL),
m_pathFinder(NULL),
m_interfacePathFinder(NULL),
m_stepFinder(NULL),
m_routeFinder(NULL),
m_coastalRouteFinder(NULL),
m_borderFinder(NULL),
m_areaFinder(NULL),
m_pDLL(NULL),
m_aiPlotDirectionX(NULL),
m_aiPlotDirectionY(NULL),
m_aiPlotCardinalDirectionX(NULL),
m_aiPlotCardinalDirectionY(NULL),
m_aiCityPlotX(NULL),
m_aiCityPlotY(NULL),
m_aiCityPlotPriority(NULL),
m_aeTurnLeftDirection(NULL),
m_aeTurnRightDirection(NULL),
m_VarSystem(NULL),
m_iMOVE_DENOMINATOR(0),
m_iFOOD_CONSUMPTION_PER_POPULATION(0),
m_iMAX_HIT_POINTS(0),
m_iHILLS_EXTRA_DEFENSE(0),
m_iRIVER_ATTACK_MODIFIER(0),
m_iAMPHIB_ATTACK_MODIFIER(0),
m_iHILLS_EXTRA_MOVEMENT(0),
m_iPEAK_EXTRA_MOVEMENT(0),
m_iMAX_PLOT_LIST_ROWS(0),
m_iUNIT_MULTISELECT_MAX(0),
m_iEVENT_MESSAGE_TIME(0),
m_iROUTE_FEATURE_GROWTH_MODIFIER(0),
m_iFEATURE_GROWTH_MODIFIER(0),
m_iMIN_CITY_RANGE(0),
m_iCITY_MAX_NUM_BUILDINGS(0),
m_iLAKE_MAX_AREA_SIZE(0),
m_iMIN_WATER_SIZE_FOR_OCEAN(0),
m_iFORTIFY_MODIFIER_PER_TURN(0),
m_iMAX_CITY_DEFENSE_DAMAGE(0),
m_iPEAK_SEE_THROUGH_CHANGE(0),
m_iHILLS_SEE_THROUGH_CHANGE(0),
m_iSEAWATER_SEE_FROM_CHANGE(0),
m_iPEAK_SEE_FROM_CHANGE(0),
m_iHILLS_SEE_FROM_CHANGE(0),
m_iMAX_REBEL_YIELD_MODIFIER(0),
m_iNEW_CAPACITY(0),				//VET NewCapacity - 1/3
// TAC - AI Improved Navel AI - koma13 - START
m_iAI_TRANSPORT_DANGER_RANGE(0),
m_iAI_LOST_TRANSPORT_MEMORY_COUNT(0),
// TAC - AI Improved Navel AI - koma13 - END

// R&R, ray, caching globals from Global Defines Alt - START
// Caching Vanilla variables	
m_PLOT_VISIBILITY_RANGE(0),
m_UNIT_VISIBILITY_RANGE(0),
m_MIN_CITY_YIELD_DECAY(0),
m_CITY_YIELD_DECAY_PERCENT(0),
m_IMMIGRATION_THRESHOLD(0),
m_IMMIGRATION_THRESHOLD_INCREASE(0),
m_TAX_TRADE_THRESHOLD(0),
m_TAX_TRADE_THRESHOLD_TAX_RATE_PERCENT(0),
m_TAX_INCREASE_CHANCE(0),
// TAC reduced REF
m_ENABLE_REDUCED_REF_FOR_AI(0),
m_REDUCED_REF_PERCENT(0),
// Domestic Market
m_PRICE_DIFF_EUROPE_DOMESTIC_LUXURY_GOODS(0),
m_DOMESTIC_SALES_MESSAGES(0),
// Wild Animals	
m_WILD_ANIMAL_LAND_TERRAIN_NATIVE_WEIGHT(0),
m_WILD_ANIMAL_LAND_UNIT_VARIATION_WEIGHT(0),
m_WILD_ANIMAL_SEA_TERRAIN_NATIVE_WEIGHT(0),
m_WILD_ANIMAL_SEA_UNIT_VARIATION_WEIGHT(0),
m_WILD_ANIMAL_REWARD_RANDOM_BASE(0),
// Max Cross Limit
m_IMMIGRATION_MAX_CROSS_LIMIT(0),
// NBMOD REF
m_NBMOD_REF_ENABLE(0),
m_NBMOD_REF_RANDOM_SHIPS(0),
m_NBMOD_REF_REAL_WEAPONS(0),
m_NBMOD_REF_WEAPONS_BONUS(0),
m_NBMOD_REF_REVOLUTION_PERCENT_ENABLE(0),
m_NBMOD_REF_MAX_TAX_ENABLE(0),
m_NBMOD_REF_WEAPONS_BONUS_PERCENT(0),
m_NBMOD_REF_HORSES_BONUS_PERCENT(0),
m_NBMOD_REF_WEAPONS_WEIGHT(0),
m_NBMOD_REF_HORSES_WEIGHT(0),
m_NBMOD_REF_NUM_UNITS_PERCENT(0),
m_NBMOD_REF_POPULATION_WEIGHT(0),
m_NBMOD_REF_TURN_WEIGHT(0),
m_NBMOD_REF_KING_PANIC_WEIGHT(0),
m_NBMOD_REF_START_VALUE_PERCENT(0),
m_NBMOD_REF_MAP_COEFFICIENT(0),
m_NBMOD_REF_DEBUG(0),
// NBMOD Education
m_EDUCATION_THRESHOLD_INCREASE(0),
m_EDUCATION_THRESHOLD(0),
// TAC Education
m_USE_TAC_EDUCATION_COST_SYSTEM(0),
m_TAC_EDUCATION_RANK1(0),
m_TAC_EDUCATION_RANK2(0),
m_TAC_EDUCATION_RANK3(0),
// NBMod Tax
m_MAX_TAX_RATE(0),
m_MIN_TAX_RATE(0),
m_INCREASE_MAX_TAX_RATE(0),
m_DECREASE_MAX_TAX_RATE(0),
// TAC AI hurries Immigrants
m_AI_HURRY_IMMIGRANTS(0),
// TAC LbD
m_LBD_BASE_CHANCE_EXPERT(0),
m_LBD_CHANCE_INCREASE_EXPERT(0),
m_LBD_PRE_ROUNDS_EXPERT(0),
m_LBD_KI_MOD_EXPERT(0),
m_LBD_BASE_CHANCE_FREE(0),
m_LBD_CHANCE_INCREASE_FREE(0),
m_LBD_PRE_ROUNDS_FREE(0),
m_LBD_CHANCE_MOD_FREE_CRIMINAL(0),
m_LBD_CHANCE_MOD_FREE_SERVANT(0),
m_LBD_CHANCE_ESCAPE(0),
m_LBD_CHANCE_MOD_ESCAPE_CRIMINAL(0),
m_LBD_CHANCE_MOD_ESCAPE_SERVANT(0),
// WTP, ray, LbD Slaves Revolt and Free - START
m_LBD_CHANCE_REVOLT(0),
m_LBD_CHANCE_MOD_REVOLT_SLAVE(0),
m_LBD_CHANCE_MOD_REVOLT_CRIMINAL(0),
// R&R, ray, getting Veterans or Free through Combat Experience
m_LBD_MIN_EXPERIENCE_VETERAN_BY_COMBAT(0),
m_LBD_MIN_EXPERIENCE_FREE_BY_COMBAT(0),
// TAC price diff raw to produced
m_PRICE_DIFF_MAN_TO_RAW(0),
// TAC KI patch
m_AI_REBELS_FOR_REVOLUTION(0),
m_UNIT_REVOLUTION_RATIO(0),
m_AI_FREE_CITY_DEFENDERS(0),
// TAC capturing Ships
m_BASE_CHANCE_CAPTURING_SHIPS(0),
// Min Rounds and Base Chances for DLL-Diplo-Events
m_MIN_ROUND_NATIVE_MERCS(0),
m_MIN_ROUND_EUROPE_WARS(0),
m_BASE_CHANCE_EUROPE_WARS(0),
m_BASE_CHANCE_CHURCH_WAR(0),
m_MIN_ROUND_CHURCH_CONTACT(0),
m_CHURCH_CONTACT_CHANCE(0),
m_MIN_ROUND_CHURCH_WAR(0),
m_BASE_CHANCE_EUROPE_PEACE(0),
m_MIN_ROUND_PRISONS_CROWDED(0),
m_PRISONS_CROWDED_CHANCE(0),
m_MIN_ROUND_REVOLUTIONARY_NOBLE(0),
m_REVOLUTIONARY_NOBLE_CHANCE(0),
m_MIN_ROUND_SMUGGLERS_AVAILABLE(0),
m_MIN_ROUND_PIRATES(0),
m_BASE_CHANCE_SMUGGLERS_AVAILABLE(0),
m_BASE_CHANCE_PIRATES(0),
m_BASE_CHANCE_CONTINENTAL_GUARD(0),
m_BASE_CHANCE_MORTAR(0),
m_BASE_CHANCE_RANGERS_AVAILABLE(0),
m_BASE_CHANCE_CONQUISTADORS_AVAILABLE(0),
m_MIN_ROUND_NATIVE_SLAVE(0),
m_MIN_ROUND_AFRICAN_SLAVES(0),
m_AFRICAN_SLAVES_CHANCE(0),
m_MIN_ROUND_BISHOP(0),
m_BISHOP_CHANCE(0),
m_PRICE_MILITIA(0),
m_MIN_CITY_SIZE_MILITIA_OR_UNREST(0),
m_CHANCE_MILITIA(0),
m_CHANCE_UNREST(0),
m_MIN_ROUND_STEALING_IMMIGRANT(0),
m_BASE_CHANCE_STEALING_IMMIGRANT(0),
// Timers for DLL-Diplo-Events
m_TIMER_NATIVE_MERC(0),
m_TIMER_EUROPEAN_WARS(0),
m_TIMER_EUROPEAN_PEACE(0),
m_TIMER_PRISONS_CROWDED(0),
m_TIMER_REVOLUTIONARY_NOBLE(0),
m_TIMER_BISHOP(0),
m_TIMER_CHURCH_DEMAND(0),
m_TIMER_CHURCH_WAR(0),
m_TIMER_SMUGGLING_SHIP(0),
m_TIMER_PIRATES(0),
m_TIMER_CONTINENTAL_GUARD(0),
m_TIMER_MORTAR(0),
m_TIMER_RANGER(0),
m_TIMER_CONQUISTADOR(0),
m_TIMER_NATIVE_SLAVE(0),
m_TIMER_AFRICAN_SLAVE(0),
m_TIMER_STEALING_IMMIGRANT(0),
// Native Raids
m_NATIVE_POTENTIAL_RAID_TARGET_THRESHOLD(0),
m_NATIVE_GOODS_RAID_PERCENT(0),
m_RANDOM_NATIVE_RAID_BASECHANCE(0),
m_NATIVE_SPARE_AI_TREASURE_CHANCE(0),
// Roundwise Native Income
m_PER_ROUND_PER_VILLAGE_INCOME_MAX(0),
m_PER_VILLAGE_FEATURE_GENERATION_LIMIT(0),
// New Storage
m_CUSTOMHOUSE_STRATEGIC_RAW_SELL_THRESHOLD(0),
m_CUSTOMHOUSE_STRATEGIC_PRODUCED_SELL_THRESHOLD(0),
m_CUSTOMHOUSE_RAW_SELL_THRESHOLD(0),
m_CUSTOMHOUSE_PRODUCED_SELL_THRESHOLD(0),
// Improvement TAX mechanism
m_TAX_TRADE_INCREASE_CHANCE_KING_ATTITUDE_BASE(0),
// R&R, ray, caching globals from Global Defines Alt - END

// cache ship profession - start - Nightinggale
m_PROFESSION_WHALING_BOAT_WORKING(-1),
m_PROFESSION_FISHING_BOAT_WORKING(-1),
// cache ship profession - end - Nightinggale
// R&R, ray, enhanced caching Whaling and Fishing - START
m_UNITCLASS_WHALING_BOAT(-1),
m_UNITCLASS_FISHING_BOAT(-1),
// R&R, ray, enhanced caching Whaling and Fishing - END

// R&R, ray, Health - START
m_MIN_POP_NEG_HEALTH(0),
m_POP_DIVISOR_NEG_HEALTH(0),
m_MAX_CITY_HEALTH(0),
m_LOWEST_CITY_HEALTH(0),
// R&R, ray, Health - END

// WTP, ray, Happiness - START
m_MIN_POP_NEG_HAPPINESS(0),
m_POP_DIVISOR_HAPPINESS(0),
m_PER_EUROPEAN_AT_WAR_UNHAPPINESS(0),
m_POP_DIVISOR_DEFENSE_UNHAPPINESS(0),
m_TAX_DIVISOR_UNHAPPINESS(0),

m_BASE_CHANCE_UNREST_UNHAPPINESS(0),
m_BASE_CHANCE_FESTIVITIES_HAPPINESS(0),
m_MIN_BALANCE_UNREST_UNHAPPINESS(0),
m_MIN_BALANCE_FESTIVITIES_HAPPINESS(0),
m_TURNS_UNREST_UNHAPPINESS(0),
m_FOUNDING_FAHTER_POINTS_FESTIVITIES_HAPPINESS(0),
m_TIMER_FESTIVITIES_OR_UNRESTS(0),
// WTP, ray, Happiness - END

m_MAX_TREASURE_AMOUNT(0), // WTP, merge Treasures, of Raubwuerger
m_TRADE_POST_GOLD_PER_NATIVE(0), // WTP, ray, Native Trade Posts - START

m_fCAMERA_MIN_YAW(0),
m_fCAMERA_MAX_YAW(0),
m_fCAMERA_FAR_CLIP_Z_HEIGHT(0),
m_fCAMERA_MAX_TRAVEL_DISTANCE(0),
m_fCAMERA_START_DISTANCE(0),
m_fPLOT_SIZE(0),
m_fCAMERA_SPECIAL_PITCH(0),
m_fCAMERA_MAX_TURN_OFFSET(0),
m_fCAMERA_MIN_DISTANCE(0),
m_fCAMERA_UPPER_PITCH(0),
m_fCAMERA_LOWER_PITCH(0),
m_fFIELD_OF_VIEW(0),
m_fUNIT_MULTISELECT_DISTANCE(0),
m_iUSE_CAN_FOUND_CITIES_ON_WATER_CALLBACK(0),
m_iUSE_CANNOT_DO_CIVIC_CALLBACK(0),
m_iUSE_CAN_DO_CIVIC_CALLBACK(0),
m_iUSE_CANNOT_CONSTRUCT_CALLBACK(0),
m_iUSE_CAN_CONSTRUCT_CALLBACK(0),
m_iUSE_CAN_DECLARE_WAR_CALLBACK(0),
m_iUSE_GET_UNIT_COST_MOD_CALLBACK(0),
m_iUSE_GET_CITY_FOUND_VALUE_CALLBACK(0),
m_iUSE_CANNOT_HANDLE_ACTION_CALLBACK(0),
m_iUSE_CAN_BUILD_CALLBACK(0),
m_iUSE_CANNOT_TRAIN_CALLBACK(0),
m_iUSE_CAN_TRAIN_CALLBACK(0),
m_iUSE_UNIT_CANNOT_MOVE_INTO_CALLBACK(0),
m_iUSE_FINISH_TEXT_CALLBACK(0),
m_iUSE_ON_UNIT_SET_XY_CALLBACK(0),
m_iUSE_ON_UNIT_SELECTED_CALLBACK(0),
m_iUSE_ON_MISSIONARY_CONVERTED_UNIT_CALLBACK(0),
m_iUSE_ON_UPDATE_CALLBACK(0),
m_iUSE_ON_UNIT_CREATED_CALLBACK(0),
m_iUSE_ON_UNIT_LOST_CALLBACK(0),
// K-Mod \ RaR
m_bUSE_AI_UNIT_UPDATE_CALLBACK(false),
m_bUSE_AI_DO_DIPLO_CALLBACK(false),
m_bUSE_AI_DO_WAR_CALLBACK(false),
m_bUSE_DO_GROWTH_CALLBACK(false),
m_bUSE_DO_CULTURE_CALLBACK(false),
m_bUSE_DO_PLOT_CULTURE_CALLBACK(false),
m_bUSE_DO_PRODUCTION_CALLBACK(false),
m_bUSE_AI_CHOOSE_PRODUCTION_CALLBACK(false),
m_bUSE_DO_PILLAGE_GOLD_CALLBACK(false),
m_bUSE_GET_EXPERIENCE_NEEDED_CALLBACK(false),
m_bUSE_DO_COMBAT_CALLBACK(false),
// K-Mod \RaR end
m_paHints(NULL),
m_paMainMenus(NULL)
{
}

CvGlobals::~CvGlobals()
{
}

//
// allocate
//
void CvGlobals::init()
{
	//
	// These vars are used to initialize the globals.
	//

	int aiPlotDirectionX[NUM_DIRECTION_TYPES] =
	{
		0,	// DIRECTION_NORTH
		1,	// DIRECTION_NORTHEAST
		1,	// DIRECTION_EAST
		1,	// DIRECTION_SOUTHEAST
		0,	// DIRECTION_SOUTH
		-1,	// DIRECTION_SOUTHWEST
		-1,	// DIRECTION_WEST
		-1,	// DIRECTION_NORTHWEST
	};

	int aiPlotDirectionY[NUM_DIRECTION_TYPES] =
	{
		1,	// DIRECTION_NORTH
		1,	// DIRECTION_NORTHEAST
		0,	// DIRECTION_EAST
		-1,	// DIRECTION_SOUTHEAST
		-1,	// DIRECTION_SOUTH
		-1,	// DIRECTION_SOUTHWEST
		0,	// DIRECTION_WEST
		1,	// DIRECTION_NORTHWEST
	};

	int aiPlotCardinalDirectionX[NUM_CARDINALDIRECTION_TYPES] =
	{
		0,	// CARDINALDIRECTION_NORTH
		1,	// CARDINALDIRECTION_EAST
		0,	// CARDINALDIRECTION_SOUTH
		-1,	// CARDINALDIRECTION_WEST
	};

	int aiPlotCardinalDirectionY[NUM_CARDINALDIRECTION_TYPES] =
	{
		1,	// CARDINALDIRECTION_NORTH
		0,	// CARDINALDIRECTION_EAST
		-1,	// CARDINALDIRECTION_SOUTH
		0,	// CARDINALDIRECTION_WEST
	};

	int aiCityPlotX[NUM_CITY_PLOTS] =
	{
		0,
		0, 1, 1, 1, 0,-1,-1,-1,
#ifndef ONE_PLOT_CITY_RADIUS
		0, 1, 2, 2, 2, 1, 0,-1,-2,-2,-2,-1,	// R&R, ray, 2 Plot Radius
		2, 2,-2,-2, // R&R, ray, 2 Plot Radius
#endif
	};

	int aiCityPlotY[NUM_CITY_PLOTS] =
	{
		0,
		1, 1, 0,-1,-1,-1, 0, 1,
#ifndef ONE_PLOT_CITY_RADIUS
		2, 2, 1, 0,-1,-2,-2,-2,-1, 0, 1, 2,	// R&R, ray, 2 Plot Radius
		2,-2,-2, 2, // R&R, ray, 2 Plot Radius
#endif
	};

	int aiCityPlotPriority[NUM_CITY_PLOTS] =
	{
		0,
		1, 2, 1, 2, 1, 2, 1, 2,
#ifndef ONE_PLOT_CITY_RADIUS
		3, 4, 4, 3, 4, 4, 3, 4, 4, 3, 4, 4,	// R&R, ray, 2 Plot Radius
		4, 4, 4, 4, // R&R, ray, 2 Plot Radius
#endif
	};

	int aaiXYCityPlot[CITY_PLOTS_DIAMETER][CITY_PLOTS_DIAMETER] =
	{
#ifdef ONE_PLOT_CITY_RADIUS
		{6, 7, 8,},

		{5, 0, 1,},

		{4, 3, 2,},
#else
		{23, 17, 18, 19, 24,},	// R&R, ray, 2 Plot Radius - START

		{16, 6, 7, 8, 20,},

		{15, 5, 0, 1, 9,},

		{14, 4, 3, 2, 10,},

		{22, 13, 12, 11, 21,}	// R&R, ray, 2 Plot Radius - END
#endif
	};

	DirectionTypes aeTurnRightDirection[NUM_DIRECTION_TYPES] =
	{
		DIRECTION_NORTHEAST,	// DIRECTION_NORTH
		DIRECTION_EAST,				// DIRECTION_NORTHEAST
		DIRECTION_SOUTHEAST,	// DIRECTION_EAST
		DIRECTION_SOUTH,			// DIRECTION_SOUTHEAST
		DIRECTION_SOUTHWEST,	// DIRECTION_SOUTH
		DIRECTION_WEST,				// DIRECTION_SOUTHWEST
		DIRECTION_NORTHWEST,	// DIRECTION_WEST
		DIRECTION_NORTH,			// DIRECTION_NORTHWEST
	};

	DirectionTypes aeTurnLeftDirection[NUM_DIRECTION_TYPES] =
	{
		DIRECTION_NORTHWEST,	// DIRECTION_NORTH
		DIRECTION_NORTH,			// DIRECTION_NORTHEAST
		DIRECTION_NORTHEAST,	// DIRECTION_EAST
		DIRECTION_EAST,				// DIRECTION_SOUTHEAST
		DIRECTION_SOUTHEAST,	// DIRECTION_SOUTH
		DIRECTION_SOUTH,			// DIRECTION_SOUTHWEST
		DIRECTION_SOUTHWEST,	// DIRECTION_WEST
		DIRECTION_WEST,				// DIRECTION_NORTHWEST
	};

	DirectionTypes aaeXYDirection[DIRECTION_DIAMETER][DIRECTION_DIAMETER] =
	{
		DIRECTION_SOUTHWEST, DIRECTION_WEST,	DIRECTION_NORTHWEST,
		DIRECTION_SOUTH,     NO_DIRECTION,    DIRECTION_NORTH,
		DIRECTION_SOUTHEAST, DIRECTION_EAST,	DIRECTION_NORTHEAST,
	};

	FAssertMsg(gDLL != NULL, "Civ app needs to set gDLL");

	m_VarSystem = new FVariableSystem;
	m_asyncRand = new CvRandom;
	m_initCore = new CvInitCore;
	m_loadedInitCore = new CvInitCore;
	m_iniInitCore = new CvInitCore;

	gDLL->initGlobals();	// some globals need to be allocated outside the dll

	m_game = new CvGameAI;
	m_map = new CvMap;

	CvPlayerAI::initStatics();
	CvTeamAI::initStatics();

	m_pt3Origin = NiPoint3(0.0f, 0.0f, 0.0f);

	COPY(m_aiPlotDirectionX, aiPlotDirectionX, int);
	COPY(m_aiPlotDirectionY, aiPlotDirectionY, int);
	COPY(m_aiPlotCardinalDirectionX, aiPlotCardinalDirectionX, int);
	COPY(m_aiPlotCardinalDirectionY, aiPlotCardinalDirectionY, int);
	COPY(m_aiCityPlotX, aiCityPlotX, int);
	COPY(m_aiCityPlotY, aiCityPlotY, int);
	COPY(m_aiCityPlotPriority, aiCityPlotPriority, int);
	COPY(m_aeTurnLeftDirection, aeTurnLeftDirection, DirectionTypes);
	COPY(m_aeTurnRightDirection, aeTurnRightDirection, DirectionTypes);
	memcpy(m_aaiXYCityPlot, aaiXYCityPlot, sizeof(m_aaiXYCityPlot));
	memcpy(m_aaeXYDirection, aaeXYDirection,sizeof(m_aaeXYDirection));
}

//
// free
//
void CvGlobals::uninit()
{
	//
	// See also CvXMLLoadUtilityInit.cpp::CleanUpGlobalVariables()
	//
	SAFE_DELETE_ARRAY(m_aiPlotDirectionX);
	SAFE_DELETE_ARRAY(m_aiPlotDirectionY);
	SAFE_DELETE_ARRAY(m_aiPlotCardinalDirectionX);
	SAFE_DELETE_ARRAY(m_aiPlotCardinalDirectionY);
	SAFE_DELETE_ARRAY(m_aiCityPlotX);
	SAFE_DELETE_ARRAY(m_aiCityPlotY);
	SAFE_DELETE_ARRAY(m_aiCityPlotPriority);
	SAFE_DELETE_ARRAY(m_aeTurnLeftDirection);
	SAFE_DELETE_ARRAY(m_aeTurnRightDirection);

	CvPlayerAI::freeStatics();
	CvTeamAI::freeStatics();

	SAFE_DELETE(m_game);
	SAFE_DELETE(m_map);

	SAFE_DELETE(m_asyncRand);
	SAFE_DELETE(m_initCore);
	SAFE_DELETE(m_loadedInitCore);
	SAFE_DELETE(m_iniInitCore);
	gDLL->uninitGlobals();	// free globals allocated outside the dll
	SAFE_DELETE(m_VarSystem);

	// already deleted outside of the dll, set to null for safety
	m_messageQueue=NULL;
	m_hotJoinMsgQueue=NULL;
	m_messageControl=NULL;
	m_setupData=NULL;
	m_messageCodes=NULL;
	m_dropMgr=NULL;
	m_portal=NULL;
	m_statsReporter=NULL;
	m_interface=NULL;
	m_diplomacyScreen=NULL;
	m_mpDiplomacyScreen=NULL;
	m_pathFinder=NULL;
	m_interfacePathFinder=NULL;
	m_stepFinder=NULL;
	m_routeFinder=NULL;
	m_coastalRouteFinder = NULL;
	m_borderFinder=NULL;
	m_areaFinder=NULL;

	deleteInfoArrays();
}

void CvGlobals::clearTypesMap()
{
	infoTypeFromStringReset();
	if (m_VarSystem)
	{
		m_VarSystem->UnInit();
	}
}


CvDiplomacyScreen* CvGlobals::getDiplomacyScreen()
{
	return m_diplomacyScreen;
}

CMPDiplomacyScreen* CvGlobals::getMPDiplomacyScreen()
{
	return m_mpDiplomacyScreen;
}

CvMessageCodeTranslator& CvGlobals::getMessageCodes()
{
	return *m_messageCodes;
}

FMPIManager*& CvGlobals::getFMPMgrPtr()
{
	return m_pFMPMgr;
}

CvPortal& CvGlobals::getPortal()
{
	return *m_portal;
}

CvSetupData& CvGlobals::getSetupData()
{
	return *m_setupData;
}

CvInitCore& CvGlobals::getInitCore()
{
	return *m_initCore;
}

CvInitCore& CvGlobals::getLoadedInitCore()
{
	return *m_loadedInitCore;
}

CvInitCore& CvGlobals::getIniInitCore()
{
	return *m_iniInitCore;
}

CvStatsReporter& CvGlobals::getStatsReporter()
{
	return *m_statsReporter;
}

CvStatsReporter* CvGlobals::getStatsReporterPtr()
{
	return m_statsReporter;
}

CvInterface& CvGlobals::getInterface()
{
	return *m_interface;
}

CvInterface* CvGlobals::getInterfacePtr()
{
	return m_interface;
}

CvRandom& CvGlobals::getASyncRand()
{
	return *m_asyncRand;
}

CMessageQueue& CvGlobals::getMessageQueue()
{
	return *m_messageQueue;
}

CMessageQueue& CvGlobals::getHotMessageQueue()
{
	return *m_hotJoinMsgQueue;
}

CMessageControl& CvGlobals::getMessageControl()
{
	return *m_messageControl;
}

CvDropMgr& CvGlobals::getDropMgr()
{
	return *m_dropMgr;
}

FAStar& CvGlobals::getPathFinder()
{
	return *m_pathFinder;
}

FAStar& CvGlobals::getInterfacePathFinder()
{
	return *m_interfacePathFinder;
}

FAStar& CvGlobals::getStepFinder()
{
	return *m_stepFinder;
}

FAStar& CvGlobals::getRouteFinder()
{
	return *m_routeFinder;
}

FAStar& CvGlobals::getCoastalRouteFinder()
{
	return *m_coastalRouteFinder;
}

FAStar& CvGlobals::getBorderFinder()
{
	return *m_borderFinder;
}

FAStar& CvGlobals::getAreaFinder()
{
	return *m_areaFinder;
}

NiPoint3& CvGlobals::getPt3Origin()
{
	return m_pt3Origin;
}

std::vector<CvInterfaceModeInfo*>& CvGlobals::getInterfaceModeInfo()		// For Moose - XML Load Util and CvInfos
{
	return m_paInterfaceModeInfo;
}

CvInterfaceModeInfo& CvGlobals::getInterfaceModeInfo(InterfaceModeTypes e)
{
	FAssert(e > -1);
	FAssert(e < NUM_INTERFACEMODE_TYPES);
	return *(m_paInterfaceModeInfo[e]);
}

NiPoint3& CvGlobals::getPt3CameraDir()
{
	return m_pt3CameraDir;
}

bool& CvGlobals::getLogging()
{
	return m_bLogging;
}

bool& CvGlobals::getRandLogging()
{
	return m_bRandLogging;
}

bool& CvGlobals::getSynchLogging()
{
	return m_bSynchLogging;
}

bool& CvGlobals::overwriteLogs()
{
	return m_bOverwriteLogs;
}

int* CvGlobals::getPlotDirectionX()
{
	return m_aiPlotDirectionX;
}

int* CvGlobals::getPlotDirectionY()
{
	return m_aiPlotDirectionY;
}

int* CvGlobals::getPlotCardinalDirectionX()
{
	return m_aiPlotCardinalDirectionX;
}

int* CvGlobals::getPlotCardinalDirectionY()
{
	return m_aiPlotCardinalDirectionY;
}

int* CvGlobals::getCityPlotX()
{
	return m_aiCityPlotX;
}

int* CvGlobals::getCityPlotY()
{
	return m_aiCityPlotY;
}

int* CvGlobals::getCityPlotPriority()
{
	return m_aiCityPlotPriority;
}

int CvGlobals::getXYCityPlot(int i, int j)
{
	FAssertMsg(i < CITY_PLOTS_DIAMETER, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < CITY_PLOTS_DIAMETER, "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return m_aaiXYCityPlot[i][j];
}

DirectionTypes* CvGlobals::getTurnLeftDirection()
{
	return m_aeTurnLeftDirection;
}

DirectionTypes CvGlobals::getTurnLeftDirection(int i)
{
	FAssertMsg(i < NUM_DIRECTION_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aeTurnLeftDirection[i];
}

DirectionTypes* CvGlobals::getTurnRightDirection()
{
	return m_aeTurnRightDirection;
}

DirectionTypes CvGlobals::getTurnRightDirection(int i)
{
	FAssertMsg(i < NUM_DIRECTION_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aeTurnRightDirection[i];
}

DirectionTypes CvGlobals::getXYDirection(int i, int j)
{
	FAssertMsg(i < DIRECTION_DIAMETER, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < DIRECTION_DIAMETER, "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return m_aaeXYDirection[i][j];
}

int CvGlobals::getNumWorldInfos()
{
	return (int)m_paWorldInfo.size();
}

std::vector<CvWorldInfo*>& CvGlobals::getWorldInfo()
{
	return m_paWorldInfo;
}

CvWorldInfo& CvGlobals::getWorldInfo(WorldSizeTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumWorldInfos());
	return *(m_paWorldInfo[e]);
}

/////////////////////////////////////////////
// CLIMATE
/////////////////////////////////////////////

int CvGlobals::getNumClimateInfos()
{
	return (int)m_paClimateInfo.size();
}

std::vector<CvClimateInfo*>& CvGlobals::getClimateInfo()
{
	return m_paClimateInfo;
}

CvClimateInfo& CvGlobals::getClimateInfo(ClimateTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumClimateInfos());
	return *(m_paClimateInfo[e]);
}

/////////////////////////////////////////////
// SEALEVEL
/////////////////////////////////////////////

int CvGlobals::getNumSeaLevelInfos()
{
	return (int)m_paSeaLevelInfo.size();
}

std::vector<CvSeaLevelInfo*>& CvGlobals::getSeaLevelInfo()
{
	return m_paSeaLevelInfo;
}

CvSeaLevelInfo& CvGlobals::getSeaLevelInfo(SeaLevelTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumSeaLevelInfos());
	return *(m_paSeaLevelInfo[e]);
}

/////////////////////////////////////////////
// EUROPE
/////////////////////////////////////////////

int CvGlobals::getNumEuropeInfos()
{
	return (int)m_paEuropeInfo.size();
}

std::vector<CvEuropeInfo*>& CvGlobals::getEuropeInfo()
{
	return m_paEuropeInfo;
}

CvEuropeInfo& CvGlobals::getEuropeInfo(EuropeTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumEuropeInfos());
	return *(m_paEuropeInfo[e]);
}

int CvGlobals::getNumHints()
{
	return (int)m_paHints.size();
}

std::vector<CvInfoBase*>& CvGlobals::getHints()
{
	return m_paHints;
}

CvInfoBase& CvGlobals::getHints(int i)
{
	return *(m_paHints[i]);
}

int CvGlobals::getNumMainMenus()
{
	return (int)m_paMainMenus.size();
}

std::vector<CvMainMenuInfo*>& CvGlobals::getMainMenus()
{
	return m_paMainMenus;
}

CvMainMenuInfo& CvGlobals::getMainMenus(int i)
{
	if (i >= getNumMainMenus())
	{
		return *(m_paMainMenus[0]);
	}

	return *(m_paMainMenus[i]);
}

int CvGlobals::getNumColorInfos()
{
	return (int)m_paColorInfo.size();
}

std::vector<CvColorInfo*>& CvGlobals::getColorInfo()
{
	return m_paColorInfo;
}

CvColorInfo& CvGlobals::getColorInfo(ColorTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumColorInfos());
	return *(m_paColorInfo[e]);
}


int CvGlobals::getNumPlayerColorInfos()
{
	return (int)m_paPlayerColorInfo.size();
}

std::vector<CvPlayerColorInfo*>& CvGlobals::getPlayerColorInfo()
{
	return m_paPlayerColorInfo;
}

CvPlayerColorInfo& CvGlobals::getPlayerColorInfo(PlayerColorTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumPlayerColorInfos());
	return *(m_paPlayerColorInfo[e]);
}

int CvGlobals::getNumRouteModelInfos()
{
	return (int)m_paRouteModelInfo.size();
}

std::vector<CvRouteModelInfo*>& CvGlobals::getRouteModelInfo()
{
	return m_paRouteModelInfo;
}

CvRouteModelInfo& CvGlobals::getRouteModelInfo(int i)
{
	FAssert(i > -1);
	FAssert(i < GC.getNumRouteModelInfos());
	return *(m_paRouteModelInfo[i]);
}

int CvGlobals::getNumRiverModelInfos()
{
	return (int)m_paRiverModelInfo.size();
}

std::vector<CvRiverModelInfo*>& CvGlobals::getRiverModelInfo()
{
	return m_paRiverModelInfo;
}

CvRiverModelInfo& CvGlobals::getRiverModelInfo(int i)
{
	FAssert(i > -1);
	FAssert(i < GC.getNumRiverModelInfos());
	return *(m_paRiverModelInfo[i]);
}

int CvGlobals::getNumWaterPlaneInfos()
{
	return (int)m_paWaterPlaneInfo.size();
}

std::vector<CvWaterPlaneInfo*>& CvGlobals::getWaterPlaneInfo()
{
	return m_paWaterPlaneInfo;
}

CvWaterPlaneInfo& CvGlobals::getWaterPlaneInfo(int i)
{
	FAssert(i > -1);
	FAssert(i < GC.getNumWaterPlaneInfos());
	return *(m_paWaterPlaneInfo[i]);
}

int CvGlobals::getNumTerrainPlaneInfos()
{
	return (int)m_paTerrainPlaneInfo.size();
}

std::vector<CvTerrainPlaneInfo*>& CvGlobals::getTerrainPlaneInfo()
{
	return m_paTerrainPlaneInfo;
}

CvTerrainPlaneInfo& CvGlobals::getTerrainPlaneInfo(int i)
{
	FAssert(i > -1);
	FAssert(i < GC.getNumTerrainPlaneInfos());
	return *(m_paTerrainPlaneInfo[i]);
}

int CvGlobals::getNumCameraOverlayInfos()
{
	return (int)m_paCameraOverlayInfo.size();
}

std::vector<CvCameraOverlayInfo*>& CvGlobals::getCameraOverlayInfo()
{
	return m_paCameraOverlayInfo;
}

CvCameraOverlayInfo& CvGlobals::getCameraOverlayInfo(int i)
{
	FAssert(i > -1);
	FAssert(i < GC.getNumCameraOverlayInfos());
	return *(m_paCameraOverlayInfo[i]);
}

int CvGlobals::getNumAnimationPathInfos()
{
	return (int)m_paAnimationPathInfo.size();
}

std::vector<CvAnimationPathInfo*>& CvGlobals::getAnimationPathInfo()
{
	return m_paAnimationPathInfo;
}

CvAnimationPathInfo& CvGlobals::getAnimationPathInfo(AnimationPathTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumAnimationPathInfos());
	return *(m_paAnimationPathInfo[e]);
}

int CvGlobals::getNumAnimationCategoryInfos()
{
	return (int)m_paAnimationCategoryInfo.size();
}

std::vector<CvAnimationCategoryInfo*>& CvGlobals::getAnimationCategoryInfo()
{
	return m_paAnimationCategoryInfo;
}

CvAnimationCategoryInfo& CvGlobals::getAnimationCategoryInfo(AnimationCategoryTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumAnimationCategoryInfos());
	return *(m_paAnimationCategoryInfo[e]);
}

int CvGlobals::getNumEntityEventInfos()
{
	return (int)m_paEntityEventInfo.size();
}

std::vector<CvEntityEventInfo*>& CvGlobals::getEntityEventInfo()
{
	return m_paEntityEventInfo;
}

CvEntityEventInfo& CvGlobals::getEntityEventInfo(EntityEventTypes e)
{
	FAssert( e > -1 );
	FAssert( e < GC.getNumEntityEventInfos() );
	return *(m_paEntityEventInfo[e]);
}

int CvGlobals::getNumEffectInfos()
{
	return (int)m_paEffectInfo.size();
}

std::vector<CvEffectInfo*>& CvGlobals::getEffectInfo()
{
	return m_paEffectInfo;
}

CvEffectInfo& CvGlobals::getEffectInfo(int i)
{
	FAssert(i > -1);
	FAssert(i < GC.getNumEffectInfos());
	return *(m_paEffectInfo[i]);
}


int CvGlobals::getNumAttachableInfos()
{
	return (int)m_paAttachableInfo.size();
}

std::vector<CvAttachableInfo*>& CvGlobals::getAttachableInfo()
{
	return m_paAttachableInfo;
}

CvAttachableInfo& CvGlobals::getAttachableInfo(int i)
{
	FAssert(i > -1);
	FAssert(i < GC.getNumAttachableInfos());
	return *(m_paAttachableInfo[i]);
}

int CvGlobals::getNumUnitFormationInfos()
{
	return (int)m_paUnitFormationInfo.size();
}

std::vector<CvUnitFormationInfo*>& CvGlobals::getUnitFormationInfo()		// For Moose - CvUnitEntity
{
	return m_paUnitFormationInfo;
}

CvUnitFormationInfo& CvGlobals::getUnitFormationInfo(int i)
{
	FAssert(i > -1);
	FAssert(i < GC.getNumUnitFormationInfos());
	return *(m_paUnitFormationInfo[i]);
}

// TEXT
int CvGlobals::getNumGameTextXML()
{
	return (int)m_paGameTextXML.size();
}

std::vector<CvGameText*>& CvGlobals::getGameTextXML()
{
	return m_paGameTextXML;
}

// Landscape INFOS
int CvGlobals::getNumLandscapeInfos()
{
	return (int)m_paLandscapeInfo.size();
}

std::vector<CvLandscapeInfo*>& CvGlobals::getLandscapeInfo()
{
	return m_paLandscapeInfo;
}

CvLandscapeInfo& CvGlobals::getLandscapeInfo(int iIndex)
{
	FAssert(iIndex > -1);
	FAssert(iIndex < GC.getNumLandscapeInfos());
	return *(m_paLandscapeInfo[iIndex]);
}

int CvGlobals::getActiveLandscapeID()
{
	return m_iActiveLandscapeID;
}

void CvGlobals::setActiveLandscapeID(int iLandscapeID)
{
	m_iActiveLandscapeID = iLandscapeID;
}


int CvGlobals::getNumTerrainInfos()
{
	return (int)m_paTerrainInfo.size();
}

std::vector<CvTerrainInfo*>& CvGlobals::getTerrainInfo()		// For Moose - XML Load Util, CvInfos, CvTerrainTypeWBPalette
{
	return m_paTerrainInfo;
}

CvTerrainInfo& CvGlobals::getTerrainInfo(TerrainTypes eTerrainNum)
{
	FAssert(eTerrainNum > -1);
	FAssert(eTerrainNum < (int)m_paTerrainInfo.size());
	return *(m_paTerrainInfo[eTerrainNum]);
}

int CvGlobals::getNumBonusInfos()
{
	return (int)m_paBonusInfo.size();
}

std::vector<CvBonusInfo*>& CvGlobals::getBonusInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paBonusInfo;
}

CvBonusInfo& CvGlobals::getBonusInfo(BonusTypes eBonusNum)
{
	FAssert(eBonusNum > -1);
	FAssert(eBonusNum < (int)m_paBonusInfo.size());
	return *(m_paBonusInfo[eBonusNum]);
}

int CvGlobals::getNumFeatureInfos()
{
	return (int)m_paFeatureInfo.size();
}

std::vector<CvFeatureInfo*>& CvGlobals::getFeatureInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paFeatureInfo;
}

CvFeatureInfo& CvGlobals::getFeatureInfo(FeatureTypes eFeatureNum)
{
	FAssert(eFeatureNum > -1);
	FAssert(eFeatureNum < (int)m_paFeatureInfo.size());
	return *(m_paFeatureInfo[eFeatureNum]);
}

int CvGlobals::getNumCivilizationInfos()
{
	return (int)m_paCivilizationInfo.size();
}

std::vector<CvCivilizationInfo*>& CvGlobals::getCivilizationInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paCivilizationInfo;
}

CvCivilizationInfo& CvGlobals::getCivilizationInfo(CivilizationTypes eCivilizationNum)
{
	FAssert(eCivilizationNum > -1);
	FAssert(eCivilizationNum < (int)m_paCivilizationInfo.size());
	return *(m_paCivilizationInfo[eCivilizationNum]);
}


int CvGlobals::getNumLeaderHeadInfos()
{
	return (int)m_paLeaderHeadInfo.size();
}

std::vector<CvLeaderHeadInfo*>& CvGlobals::getLeaderHeadInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paLeaderHeadInfo;
}

CvLeaderHeadInfo& CvGlobals::getLeaderHeadInfo(LeaderHeadTypes eLeaderHeadNum)
{
	FAssert(eLeaderHeadNum > -1);
	FAssert(eLeaderHeadNum < GC.getNumLeaderHeadInfos());
	return *(m_paLeaderHeadInfo[eLeaderHeadNum]);
}


int CvGlobals::getNumTraitInfos()
{
	return (int)m_paTraitInfo.size();
}

std::vector<CvTraitInfo*>& CvGlobals::getTraitInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paTraitInfo;
}

CvTraitInfo& CvGlobals::getTraitInfo(TraitTypes eTraitNum)
{
	FAssert(eTraitNum > -1);
	FAssert(eTraitNum < GC.getNumTraitInfos());
	return *(m_paTraitInfo[eTraitNum]);
}


int CvGlobals::getNumCursorInfos()
{
	return (int)m_paCursorInfo.size();
}

std::vector<CvCursorInfo*>& CvGlobals::getCursorInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paCursorInfo;
}

CvCursorInfo& CvGlobals::getCursorInfo(CursorTypes eCursorNum)
{
	FAssert(eCursorNum > -1);
	FAssert(eCursorNum < GC.getNumCursorInfos());
	return *(m_paCursorInfo[eCursorNum]);
}

int CvGlobals::getNumSlideShowInfos()
{
	return (int)m_paSlideShowInfo.size();
}

std::vector<CvSlideShowInfo*>& CvGlobals::getSlideShowInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paSlideShowInfo;
}

CvSlideShowInfo& CvGlobals::getSlideShowInfo(int iIndex)
{
	FAssert(iIndex > -1);
	FAssert(iIndex < GC.getNumSlideShowInfos());
	return *(m_paSlideShowInfo[iIndex]);
}

int CvGlobals::getNumSlideShowRandomInfos()
{
	return (int)m_paSlideShowRandomInfo.size();
}

std::vector<CvSlideShowRandomInfo*>& CvGlobals::getSlideShowRandomInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paSlideShowRandomInfo;
}

CvSlideShowRandomInfo& CvGlobals::getSlideShowRandomInfo(int iIndex)
{
	FAssert(iIndex > -1);
	FAssert(iIndex < GC.getNumSlideShowRandomInfos());
	return *(m_paSlideShowRandomInfo[iIndex]);
}

int CvGlobals::getNumWorldPickerInfos()
{
	return (int)m_paWorldPickerInfo.size();
}

std::vector<CvWorldPickerInfo*>& CvGlobals::getWorldPickerInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paWorldPickerInfo;
}

CvWorldPickerInfo& CvGlobals::getWorldPickerInfo(int iIndex)
{
	FAssert(iIndex > -1);
	FAssert(iIndex < GC.getNumWorldPickerInfos());
	return *(m_paWorldPickerInfo[iIndex]);
}

int CvGlobals::getNumUnitInfos()
{
	return (int)m_paUnitInfo.size();
}

std::vector<CvUnitInfo*>& CvGlobals::getUnitInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paUnitInfo;
}

CvUnitInfo& CvGlobals::getUnitInfo(UnitTypes eUnitNum)
{
	FAssert(eUnitNum > -1);
	FAssert(eUnitNum < GC.getNumUnitInfos());
	return *(m_paUnitInfo[eUnitNum]);
}

int CvGlobals::getNumSpecialUnitInfos()
{
	return (int)m_paSpecialUnitInfo.size();
}

std::vector<CvSpecialUnitInfo*>& CvGlobals::getSpecialUnitInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paSpecialUnitInfo;
}

CvSpecialUnitInfo& CvGlobals::getSpecialUnitInfo(SpecialUnitTypes eSpecialUnitNum)
{
	FAssert(eSpecialUnitNum > -1);
	FAssert(eSpecialUnitNum < GC.getNumSpecialUnitInfos());
	return *(m_paSpecialUnitInfo[eSpecialUnitNum]);
}


int CvGlobals::getNumConceptInfos()
{
	return (int)m_paConceptInfo.size();
}

std::vector<CvInfoBase*>& CvGlobals::getConceptInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paConceptInfo;
}

CvInfoBase& CvGlobals::getConceptInfo(ConceptTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumConceptInfos());
	return *(m_paConceptInfo[e]);
}
int CvGlobals::getNumCalendarInfos()
{
	return (int)m_paCalendarInfo.size();
}

std::vector<CvInfoBase*>& CvGlobals::getCalendarInfo()
{
	return m_paCalendarInfo;
}

CvInfoBase& CvGlobals::getCalendarInfo(CalendarTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumCalendarInfos());
	return *(m_paCalendarInfo[e]);
}


int CvGlobals::getNumSeasonInfos()
{
	return (int)m_paSeasonInfo.size();
}

std::vector<CvInfoBase*>& CvGlobals::getSeasonInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paSeasonInfo;
}

CvInfoBase& CvGlobals::getSeasonInfo(SeasonTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumSeasonInfos());
	return *(m_paSeasonInfo[e]);
}


int CvGlobals::getNumMonthInfos()
{
	return (int)m_paMonthInfo.size();
}

std::vector<CvInfoBase*>& CvGlobals::getMonthInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paMonthInfo;
}

CvInfoBase& CvGlobals::getMonthInfo(MonthTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumMonthInfos());
	return *(m_paMonthInfo[e]);
}


int CvGlobals::getNumDenialInfos()
{
	return (int)m_paDenialInfo.size();
}

std::vector<CvInfoBase*>& CvGlobals::getDenialInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paDenialInfo;
}

CvInfoBase& CvGlobals::getDenialInfo(DenialTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumDenialInfos());
	return *(m_paDenialInfo[e]);
}


int CvGlobals::getNumInvisibleInfos()
{
	return (int)m_paInvisibleInfo.size();
}

std::vector<CvInfoBase*>& CvGlobals::getInvisibleInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paInvisibleInfo;
}

CvInfoBase& CvGlobals::getInvisibleInfo(InvisibleTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumInvisibleInfos());
	return *(m_paInvisibleInfo[e]);
}


int CvGlobals::getNumFatherInfos()
{
	return (int)m_paFatherInfo.size();
}

std::vector<CvFatherInfo*>& CvGlobals::getFatherInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paFatherInfo;
}

CvFatherInfo& CvGlobals::getFatherInfo(FatherTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumFatherInfos());
	return *(m_paFatherInfo[e]);
}

int CvGlobals::getNumFatherPointInfos()
{
	return (int)m_paFatherPointInfo.size();
}

std::vector<CvFatherPointInfo*>& CvGlobals::getFatherPointInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paFatherPointInfo;
}

CvFatherPointInfo& CvGlobals::getFatherPointInfo(FatherPointTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumFatherPointInfos());
	return *(m_paFatherPointInfo[e]);
}

int CvGlobals::getNumUnitCombatInfos()
{
	return (int)m_paUnitCombatInfo.size();
}

std::vector<CvInfoBase*>& CvGlobals::getUnitCombatInfo()
{
	return m_paUnitCombatInfo;
}

CvInfoBase& CvGlobals::getUnitCombatInfo(UnitCombatTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumUnitCombatInfos());
	return *(m_paUnitCombatInfo[e]);
}


std::vector<CvInfoBase*>& CvGlobals::getDomainInfo()
{
	return m_paDomainInfo;
}

CvInfoBase& CvGlobals::getDomainInfo(DomainTypes e)
{
	FAssert(e > -1);
	FAssert(e < NUM_DOMAIN_TYPES);
	return *(m_paDomainInfo[e]);
}


std::vector<CvInfoBase*>& CvGlobals::getUnitAIInfo()
{
	return m_paUnitAIInfos;
}

CvInfoBase& CvGlobals::getUnitAIInfo(UnitAITypes eUnitAINum)
{
	FAssert(eUnitAINum >= 0);
	FAssert(eUnitAINum < NUM_UNITAI_TYPES);
	return *(m_paUnitAIInfos[eUnitAINum]);
}


std::vector<CvInfoBase*>& CvGlobals::getAttitudeInfo()
{
	return m_paAttitudeInfos;
}

CvInfoBase& CvGlobals::getAttitudeInfo(AttitudeTypes eAttitudeNum)
{
	FAssert(eAttitudeNum >= 0);
	FAssert(eAttitudeNum < NUM_ATTITUDE_TYPES);
	return *(m_paAttitudeInfos[eAttitudeNum]);
}


std::vector<CvInfoBase*>& CvGlobals::getMemoryInfo()
{
	return m_paMemoryInfos;
}

CvInfoBase& CvGlobals::getMemoryInfo(MemoryTypes eMemoryNum)
{
	FAssert(eMemoryNum >= 0);
	FAssert(eMemoryNum < NUM_MEMORY_TYPES);
	return *(m_paMemoryInfos[eMemoryNum]);
}

int CvGlobals::getNumFatherCategoryInfos()
{
	return (int)m_paFatherCategoryInfos.size();
}

std::vector<CvInfoBase*>& CvGlobals::getFatherCategoryInfo()
{
	return m_paFatherCategoryInfos;
}

CvInfoBase& CvGlobals::getFatherCategoryInfo(FatherCategoryTypes eFatherCategoryNum)
{
	FAssert(eFatherCategoryNum >= 0);
	FAssert(eFatherCategoryNum < GC.getNumFatherCategoryInfos());
	return *(m_paFatherCategoryInfos[eFatherCategoryNum]);
}

int CvGlobals::getNumGameOptionInfos()
{
	return (int)m_paGameOptionInfos.size();
}

std::vector<CvGameOptionInfo*>& CvGlobals::getGameOptionInfo()
{
	return m_paGameOptionInfos;
}

CvGameOptionInfo& CvGlobals::getGameOptionInfo(GameOptionTypes eGameOptionNum)
{
	FAssert(eGameOptionNum >= 0);
	FAssert(eGameOptionNum < GC.getNumGameOptionInfos());
	return *(m_paGameOptionInfos[eGameOptionNum]);
}

int CvGlobals::getNumMPOptionInfos()
{
	return (int)m_paMPOptionInfos.size();
}

std::vector<CvMPOptionInfo*>& CvGlobals::getMPOptionInfo()
{
	 return m_paMPOptionInfos;
}

CvMPOptionInfo& CvGlobals::getMPOptionInfo(MultiplayerOptionTypes eMPOptionNum)
{
	FAssert(eMPOptionNum >= 0);
	FAssert(eMPOptionNum < GC.getNumMPOptionInfos());
	return *(m_paMPOptionInfos[eMPOptionNum]);
}

int CvGlobals::getNumForceControlInfos()
{
	return (int)m_paForceControlInfos.size();
}

std::vector<CvForceControlInfo*>& CvGlobals::getForceControlInfo()
{
	return m_paForceControlInfos;
}

CvForceControlInfo& CvGlobals::getForceControlInfo(ForceControlTypes eForceControlNum)
{
	FAssert(eForceControlNum >= 0);
	FAssert(eForceControlNum < GC.getNumForceControlInfos());
	return *(m_paForceControlInfos[eForceControlNum]);
}

std::vector<CvPlayerOptionInfo*>& CvGlobals::getPlayerOptionInfo()
{
	return m_paPlayerOptionInfos;
}

CvPlayerOptionInfo& CvGlobals::getPlayerOptionInfo(PlayerOptionTypes ePlayerOptionNum)
{
	FAssert(ePlayerOptionNum >= 0);
	FAssert(ePlayerOptionNum < NUM_PLAYEROPTION_TYPES);
	return *(m_paPlayerOptionInfos[ePlayerOptionNum]);
}

std::vector<CvGraphicOptionInfo*>& CvGlobals::getGraphicOptionInfo()
{
	return m_paGraphicOptionInfos;
}

CvGraphicOptionInfo& CvGlobals::getGraphicOptionInfo(GraphicOptionTypes eGraphicOptionNum)
{
	FAssert(eGraphicOptionNum >= 0);
	FAssert(eGraphicOptionNum < NUM_GRAPHICOPTION_TYPES);
	return *(m_paGraphicOptionInfos[eGraphicOptionNum]);
}


std::vector<CvYieldInfo*>& CvGlobals::getYieldInfo()	// For Moose - XML Load Util
{
	return m_paYieldInfo;
}

CvYieldInfo& CvGlobals::getYieldInfo(YieldTypes eYieldNum)
{
	FAssert(eYieldNum > -1);
	FAssert(eYieldNum < NUM_YIELD_TYPES);
	return *(m_paYieldInfo[eYieldNum]);
}


int CvGlobals::getNumRouteInfos()
{
	return (int)m_paRouteInfo.size();
}

std::vector<CvRouteInfo*>& CvGlobals::getRouteInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paRouteInfo;
}

CvRouteInfo& CvGlobals::getRouteInfo(RouteTypes eRouteNum)
{
	FAssert(eRouteNum > -1);
	FAssert(eRouteNum < GC.getNumRouteInfos());
	return *(m_paRouteInfo[eRouteNum]);
}

int CvGlobals::getNumImprovementInfos()
{
	return (int)m_paImprovementInfo.size();
}

std::vector<CvImprovementInfo*>& CvGlobals::getImprovementInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paImprovementInfo;
}

CvImprovementInfo& CvGlobals::getImprovementInfo(ImprovementTypes eImprovementNum)
{
	FAssert(eImprovementNum > -1);
	FAssert(eImprovementNum < GC.getNumImprovementInfos());
	return *(m_paImprovementInfo[eImprovementNum]);
}

int CvGlobals::getNumGoodyInfos()
{
	return (int)m_paGoodyInfo.size();
}

std::vector<CvGoodyInfo*>& CvGlobals::getGoodyInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paGoodyInfo;
}

CvGoodyInfo& CvGlobals::getGoodyInfo(GoodyTypes eGoodyNum)
{
	FAssert(eGoodyNum > -1);
	FAssert(eGoodyNum < GC.getNumGoodyInfos());
	return *(m_paGoodyInfo[eGoodyNum]);
}

/*
	int CvGlobals::getNumBuildingInfos()
	Complexity: O( 1 )
*/
int CvGlobals::getNumBuildInfos()
{
	return (int)m_paBuildInfo.size();
}

std::vector<CvBuildInfo*>& CvGlobals::getBuildInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paBuildInfo;
}

/*
	CvBuildingInfo& CvGlobals::getBuildingInfo(BuildingTypes eBuildingNum)
	Complexity: O( 1 )
	Purpose:
		Get a buildings (CvBuildingInfo) data object.
*/
CvBuildInfo& CvGlobals::getBuildInfo(BuildTypes eBuildNum)
{
	FAssert(eBuildNum > -1);
	FAssert(eBuildNum < GC.getNumBuildInfos());
	return *(m_paBuildInfo[eBuildNum]);
}

int CvGlobals::getNumHandicapInfos()
{
	return (int)m_paHandicapInfo.size();
}

std::vector<CvHandicapInfo*>& CvGlobals::getHandicapInfo()	// Do NOT export outside of the DLL	// For Moose - XML Load Util
{
	return m_paHandicapInfo;
}

CvHandicapInfo& CvGlobals::getHandicapInfo(HandicapTypes eHandicapNum)
{
	FAssert(eHandicapNum > -1);
	FAssert(eHandicapNum < GC.getNumHandicapInfos());
	return *(m_paHandicapInfo[eHandicapNum]);
}

int CvGlobals::getNumGameSpeedInfos()
{
	return (int)m_paGameSpeedInfo.size();
}

std::vector<CvGameSpeedInfo*>& CvGlobals::getGameSpeedInfo()	// Do NOT export outside of the DLL	// For Moose - XML Load Util
{
	return m_paGameSpeedInfo;
}

CvGameSpeedInfo& CvGlobals::getGameSpeedInfo(GameSpeedTypes eGameSpeedNum)
{
	FAssert(eGameSpeedNum > -1);
	FAssert(eGameSpeedNum < GC.getNumGameSpeedInfos());
	return *(m_paGameSpeedInfo[eGameSpeedNum]);
}

int CvGlobals::getNumAlarmInfos()
{
	return (int)m_paAlarmInfo.size();
}

std::vector<CvAlarmInfo*>& CvGlobals::getAlarmInfo()	// Do NOT export outside of the DLL	// For Moose - XML Load Util
{
	return m_paAlarmInfo;
}

CvAlarmInfo& CvGlobals::getAlarmInfo(AlarmTypes eAlarm)
{
	FAssert(eAlarm > -1);
	FAssert(eAlarm < GC.getNumAlarmInfos());
	return *(m_paAlarmInfo[eAlarm]);
}

int CvGlobals::getNumTurnTimerInfos()
{
	return (int)m_paTurnTimerInfo.size();
}

std::vector<CvTurnTimerInfo*>& CvGlobals::getTurnTimerInfo()	// Do NOT export outside of the DLL	// For Moose - XML Load Util
{
	return m_paTurnTimerInfo;
}

CvTurnTimerInfo& CvGlobals::getTurnTimerInfo(TurnTimerTypes eTurnTimerNum)
{
	FAssert(eTurnTimerNum > -1);
	FAssert(eTurnTimerNum < GC.getNumTurnTimerInfos());
	return *(m_paTurnTimerInfo[eTurnTimerNum]);
}

int CvGlobals::getNumBuildingClassInfos()
{
	return (int)m_paBuildingClassInfo.size();
}

std::vector<CvBuildingClassInfo*>& CvGlobals::getBuildingClassInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paBuildingClassInfo;
}

CvBuildingClassInfo& CvGlobals::getBuildingClassInfo(BuildingClassTypes eBuildingClassNum)
{
	FAssert(eBuildingClassNum > -1);
	FAssert(eBuildingClassNum < GC.getNumBuildingClassInfos());
	return *(m_paBuildingClassInfo[eBuildingClassNum]);
}

int CvGlobals::getNumBuildingInfos()
{
	return (int)m_paBuildingInfo.size();
}

std::vector<CvBuildingInfo*>& CvGlobals::getBuildingInfo()	// For Moose - XML Load Util, CvInfos, CvCacheObject
{
	return m_paBuildingInfo;
}

CvBuildingInfo& CvGlobals::getBuildingInfo(BuildingTypes eBuildingNum)
{
	FAssert(eBuildingNum > -1);
	FAssert(eBuildingNum < GC.getNumBuildingInfos());
	return *(m_paBuildingInfo[eBuildingNum]);
}

int CvGlobals::getNumSpecialBuildingInfos()
{
	return (int)m_paSpecialBuildingInfo.size();
}

std::vector<CvSpecialBuildingInfo*>& CvGlobals::getSpecialBuildingInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paSpecialBuildingInfo;
}

CvSpecialBuildingInfo& CvGlobals::getSpecialBuildingInfo(SpecialBuildingTypes eSpecialBuildingNum)
{
	FAssert(eSpecialBuildingNum > -1);
	FAssert(eSpecialBuildingNum < GC.getNumSpecialBuildingInfos());
	return *(m_paSpecialBuildingInfo[eSpecialBuildingNum]);
}

int CvGlobals::getNumUnitClassInfos()
{
	return (int)m_paUnitClassInfo.size();
}

std::vector<CvUnitClassInfo*>& CvGlobals::getUnitClassInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paUnitClassInfo;
}

CvUnitClassInfo& CvGlobals::getUnitClassInfo(UnitClassTypes eUnitClassNum)
{
	FAssert(eUnitClassNum > -1);
	FAssert(eUnitClassNum < GC.getNumUnitClassInfos());
	return *(m_paUnitClassInfo[eUnitClassNum]);
}

int CvGlobals::getNumActionInfos()
{
	return (int)m_paActionInfo.size();
}

std::vector<CvActionInfo*>& CvGlobals::getActionInfo()	// For Moose - XML Load Util
{
	return m_paActionInfo;
}

CvActionInfo& CvGlobals::getActionInfo(int i)
{
	FAssertMsg(i < getNumActionInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return *(m_paActionInfo[i]);
}

std::vector<CvMissionInfo*>& CvGlobals::getMissionInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paMissionInfo;
}

CvMissionInfo& CvGlobals::getMissionInfo(MissionTypes eMissionNum)
{
	FAssert(eMissionNum > -1);
	FAssert(eMissionNum < NUM_MISSION_TYPES);
	return *(m_paMissionInfo[eMissionNum]);
}

std::vector<CvControlInfo*>& CvGlobals::getControlInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paControlInfo;
}

CvControlInfo& CvGlobals::getControlInfo(ControlTypes eControlNum)
{
	FAssert(eControlNum > -1);
	FAssert(eControlNum < NUM_CONTROL_TYPES);
	FAssert(m_paControlInfo.size() > 0);
	return *(m_paControlInfo[eControlNum]);
}

std::vector<CvCommandInfo*>& CvGlobals::getCommandInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paCommandInfo;
}

CvCommandInfo& CvGlobals::getCommandInfo(CommandTypes eCommandNum)
{
	FAssert(eCommandNum > -1);
	FAssert(eCommandNum < NUM_COMMAND_TYPES);
	return *(m_paCommandInfo[eCommandNum]);
}

int CvGlobals::getNumAutomateInfos()
{
	return (int)m_paAutomateInfo.size();
}

std::vector<CvAutomateInfo*>& CvGlobals::getAutomateInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paAutomateInfo;
}

CvAutomateInfo& CvGlobals::getAutomateInfo(int iAutomateNum)
{
	FAssertMsg(iAutomateNum < getNumAutomateInfos(), "Index out of bounds");
	FAssertMsg(iAutomateNum > -1, "Index out of bounds");
	return *(m_paAutomateInfo[iAutomateNum]);
}

int CvGlobals::getNumPromotionInfos()
{
	return (int)m_paPromotionInfo.size();
}

std::vector<CvPromotionInfo*>& CvGlobals::getPromotionInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paPromotionInfo;
}

CvPromotionInfo& CvGlobals::getPromotionInfo(PromotionTypes ePromotionNum)
{
	FAssert(ePromotionNum > -1);
	FAssert(ePromotionNum < GC.getNumPromotionInfos());
	return *(m_paPromotionInfo[ePromotionNum]);
}

int CvGlobals::getNumProfessionInfos()
{
	return (int)m_paProfessionInfo.size();
}

std::vector<CvProfessionInfo*>& CvGlobals::getProfessionInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paProfessionInfo;
}

CvProfessionInfo& CvGlobals::getProfessionInfo(ProfessionTypes eProfessionNum)
{
	FAssert(eProfessionNum > -1);
	FAssert(eProfessionNum < (int)m_paProfessionInfo.size());
	return *(m_paProfessionInfo[eProfessionNum]);
}
int CvGlobals::getNumCivicOptionInfos()
{
	return (int)m_paCivicOptionInfo.size();
}

std::vector<CvInfoBase*>& CvGlobals::getCivicOptionInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paCivicOptionInfo;
}

CvInfoBase& CvGlobals::getCivicOptionInfo(CivicOptionTypes eCivicOptionNum)
{
	FAssert(eCivicOptionNum > -1);
	FAssert(eCivicOptionNum < GC.getNumCivicOptionInfos());
	return *(m_paCivicOptionInfo[eCivicOptionNum]);
}

int CvGlobals::getNumCivicInfos()
{
	return (int)m_paCivicInfo.size();
}

std::vector<CvCivicInfo*>& CvGlobals::getCivicInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paCivicInfo;
}

CvCivicInfo& CvGlobals::getCivicInfo(CivicTypes eCivicNum)
{
	FAssert(eCivicNum > -1);
	FAssert(eCivicNum < GC.getNumCivicInfos());
	return *(m_paCivicInfo[eCivicNum]);
}

int CvGlobals::getNumDiplomacyInfos()
{
	return (int)m_paDiplomacyInfo.size();
}

std::vector<CvDiplomacyInfo*>& CvGlobals::getDiplomacyInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paDiplomacyInfo;
}

CvDiplomacyInfo& CvGlobals::getDiplomacyInfo(int iDiplomacyNum)
{
	FAssertMsg(iDiplomacyNum < getNumDiplomacyInfos(), "Index out of bounds");
	FAssertMsg(iDiplomacyNum > -1, "Index out of bounds");
	return *(m_paDiplomacyInfo[iDiplomacyNum]);
}

int CvGlobals::getNumEraInfos()
{
	return (int)m_aEraInfo.size();
}

std::vector<CvEraInfo*>& CvGlobals::getEraInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_aEraInfo;
}

CvEraInfo& CvGlobals::getEraInfo(EraTypes eEraNum)
{
	FAssert(eEraNum > -1);
	FAssert(eEraNum < GC.getNumEraInfos());
	return *(m_aEraInfo[eEraNum]);
}

int CvGlobals::getNumHurryInfos()
{
	return (int)m_paHurryInfo.size();
}

std::vector<CvHurryInfo*>& CvGlobals::getHurryInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paHurryInfo;
}

CvHurryInfo& CvGlobals::getHurryInfo(HurryTypes eHurryNum)
{
	FAssert(eHurryNum > -1);
	FAssert(eHurryNum < GC.getNumHurryInfos());
	return *(m_paHurryInfo[eHurryNum]);
}

int CvGlobals::getNumEmphasizeInfos()
{
	return (int)m_paEmphasizeInfo.size();
}

std::vector<CvEmphasizeInfo*>& CvGlobals::getEmphasizeInfo()	// For Moose - XML Load Util
{
	return m_paEmphasizeInfo;
}

CvEmphasizeInfo& CvGlobals::getEmphasizeInfo(EmphasizeTypes eEmphasizeNum)
{
	FAssert(eEmphasizeNum > -1);
	FAssert(eEmphasizeNum < GC.getNumEmphasizeInfos());
	return *(m_paEmphasizeInfo[eEmphasizeNum]);
}

int CvGlobals::getNumCultureLevelInfos()
{
	return (int)m_paCultureLevelInfo.size();
}

std::vector<CvCultureLevelInfo*>& CvGlobals::getCultureLevelInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paCultureLevelInfo;
}

CvCultureLevelInfo& CvGlobals::getCultureLevelInfo(CultureLevelTypes eCultureLevelNum)
{
	FAssert(eCultureLevelNum > -1);
	FAssert(eCultureLevelNum < GC.getNumCultureLevelInfos());
	return *(m_paCultureLevelInfo[eCultureLevelNum]);
}

int CvGlobals::getNumVictoryInfos()
{
	return (int)m_paVictoryInfo.size();
}

std::vector<CvVictoryInfo*>& CvGlobals::getVictoryInfo()	// For Moose - XML Load Util, CvInfos
{
	return m_paVictoryInfo;
}

CvVictoryInfo& CvGlobals::getVictoryInfo(VictoryTypes eVictoryNum)
{
	FAssert(eVictoryNum > -1);
	FAssert(eVictoryNum < GC.getNumVictoryInfos());
	return *(m_paVictoryInfo[eVictoryNum]);
}

int CvGlobals::getNumEventTriggerInfos()
{
	return (int)m_paEventTriggerInfo.size();
}

std::vector<CvEventTriggerInfo*>& CvGlobals::getEventTriggerInfo()
{
	return m_paEventTriggerInfo;
}

CvEventTriggerInfo& CvGlobals::getEventTriggerInfo(EventTriggerTypes eEventTrigger)
{
	FAssert(eEventTrigger > -1);
	FAssert(eEventTrigger < GC.getNumEventTriggerInfos());
	return *(m_paEventTriggerInfo[eEventTrigger]);
}

int CvGlobals::getNumEventInfos()
{
	return (int)m_paEventInfo.size();
}

std::vector<CvEventInfo*>& CvGlobals::getEventInfo()
{
	return m_paEventInfo;
}

CvEventInfo& CvGlobals::getEventInfo(EventTypes eEvent)
{
	FAssert(eEvent > -1);
	FAssert(eEvent < GC.getNumEventInfos());
	return *(m_paEventInfo[eEvent]);
}

int& CvGlobals::getNumEntityEventTypes()
{
	return m_iNumEntityEventTypes;
}

CvString*& CvGlobals::getEntityEventTypes()
{
	return m_paszEntityEventTypes;
}

CvString& CvGlobals::getEntityEventTypes(EntityEventTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumEntityEventTypes());
	return m_paszEntityEventTypes[e];
}

int& CvGlobals::getNumAnimationOperatorTypes()
{
	return m_iNumAnimationOperatorTypes;
}

CvString*& CvGlobals::getAnimationOperatorTypes()
{
	return m_paszAnimationOperatorTypes;
}

CvString& CvGlobals::getAnimationOperatorTypes(AnimationOperatorTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumAnimationOperatorTypes());
	return m_paszAnimationOperatorTypes[e];
}

CvString*& CvGlobals::getFunctionTypes()
{
	return m_paszFunctionTypes;
}

CvString& CvGlobals::getFunctionTypes(FunctionTypes e)
{
	FAssert(e > -1);
	FAssert(e < NUM_FUNC_TYPES);
	return m_paszFunctionTypes[e];
}

int& CvGlobals::getNumArtStyleTypes()
{
	return m_iNumArtStyleTypes;
}

CvString*& CvGlobals::getArtStyleTypes()
{
	return m_paszArtStyleTypes;
}

CvString& CvGlobals::getArtStyleTypes(ArtStyleTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumArtStyleTypes());
	return m_paszArtStyleTypes[e];
}

//Androrc UnitArtStyles
int CvGlobals::getNumUnitArtStyleTypeInfos()
{
    return (int)m_paUnitArtStyleTypeInfo.size();
}

std::vector<CvUnitArtStyleTypeInfo*>& CvGlobals::getUnitArtStyleTypeInfo()
{
	return m_paUnitArtStyleTypeInfo;
}

CvUnitArtStyleTypeInfo& CvGlobals::getUnitArtStyleTypeInfo(UnitArtStyleTypes eUnitArtStyleTypeNum)
{
	FAssert(eUnitArtStyleTypeNum > -1);
	FAssert(eUnitArtStyleTypeNum < GC.getNumUnitArtStyleTypeInfos());
	return *(m_paUnitArtStyleTypeInfo[eUnitArtStyleTypeNum]);
}
//Androrc End

int& CvGlobals::getNumCitySizeTypes()
{
	return m_iNumCitySizeTypes;
}

CvString*& CvGlobals::getCitySizeTypes()
{
	return m_paszCitySizeTypes;
}

CvString& CvGlobals::getCitySizeTypes(int i)
{
	FAssertMsg(i < getNumCitySizeTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paszCitySizeTypes[i];
}

CvString*& CvGlobals::getContactTypes()
{
	return m_paszContactTypes;
}

CvString& CvGlobals::getContactTypes(ContactTypes e)
{
	FAssert(e > -1);
	FAssert(e < NUM_CONTACT_TYPES);
	return m_paszContactTypes[e];
}

CvString*& CvGlobals::getDiplomacyPowerTypes()
{
	return m_paszDiplomacyPowerTypes;
}

CvString& CvGlobals::getDiplomacyPowerTypes(DiplomacyPowerTypes e)
{
	FAssert(e > -1);
	FAssert(e < NUM_DIPLOMACYPOWER_TYPES);
	return m_paszDiplomacyPowerTypes[e];
}

CvString*& CvGlobals::getAutomateTypes()
{
	return m_paszAutomateTypes;
}

CvString& CvGlobals::getAutomateTypes(AutomateTypes e)
{
	FAssert(e > -1);
	FAssert(e < NUM_AUTOMATE_TYPES);
	return m_paszAutomateTypes[e];
}

CvString*& CvGlobals::getDirectionTypes()
{
	return m_paszDirectionTypes;
}

CvString& CvGlobals::getDirectionTypes(AutomateTypes e)
{
	FAssert(e > -1);
	FAssert(e < NUM_DIRECTION_TYPES);
	return m_paszDirectionTypes[e];
}

int& CvGlobals::getNumFootstepAudioTypes()
{
	return m_iNumFootstepAudioTypes;
}

CvString*& CvGlobals::getFootstepAudioTypes()
{
	return m_paszFootstepAudioTypes;
}

CvString& CvGlobals::getFootstepAudioTypes(int i)
{
	FAssertMsg(i < getNumFootstepAudioTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paszFootstepAudioTypes[i];
}

int CvGlobals::getFootstepAudioTypeByTag(CvString strTag)
{
	int iIndex = -1;

	if ( strTag.GetLength() <= 0 )
	{
		return iIndex;
	}

	for ( int i = 0; i < m_iNumFootstepAudioTypes; i++ )
	{
		if ( strTag.CompareNoCase(m_paszFootstepAudioTypes[i]) == 0 )
		{
			iIndex = i;
			break;
		}
	}

	return iIndex;
}

CvString*& CvGlobals::getFootstepAudioTags()
{
	return m_paszFootstepAudioTags;
}

CvString& CvGlobals::getFootstepAudioTags(int i)
{
//	FAssertMsg(i < getNumFootstepAudioTags(), "Index out of bounds")
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paszFootstepAudioTags[i];
}

void CvGlobals::setCurrentXMLFile(const TCHAR* szFileName)
{
	m_szCurrentXMLFile = szFileName;
}

CvString& CvGlobals::getCurrentXMLFile()
{
	return m_szCurrentXMLFile;
}

FVariableSystem* CvGlobals::getDefinesVarSystem()
{
	return m_VarSystem;
}

void CvGlobals::cacheGlobals()
{
	m_iMOVE_DENOMINATOR = getDefineINT("MOVE_DENOMINATOR");
	m_iFOOD_CONSUMPTION_PER_POPULATION = getDefineINT("FOOD_CONSUMPTION_PER_POPULATION");
	m_iMAX_HIT_POINTS = getDefineINT("MAX_HIT_POINTS");
	m_iHILLS_EXTRA_DEFENSE = getDefineINT("HILLS_EXTRA_DEFENSE");
	m_iRIVER_ATTACK_MODIFIER = getDefineINT("RIVER_ATTACK_MODIFIER");
	m_iAMPHIB_ATTACK_MODIFIER = getDefineINT("AMPHIB_ATTACK_MODIFIER");
	m_iHILLS_EXTRA_MOVEMENT = getDefineINT("HILLS_EXTRA_MOVEMENT");
	m_iPEAK_EXTRA_MOVEMENT = getDefineINT("PEAK_EXTRA_MOVEMENT");
	m_iMAX_PLOT_LIST_ROWS = getDefineINT("MAX_PLOT_LIST_ROWS");
	m_iUNIT_MULTISELECT_MAX = getDefineINT("UNIT_MULTISELECT_MAX");
	m_iEVENT_MESSAGE_TIME = getDefineINT("EVENT_MESSAGE_TIME");
	m_iROUTE_FEATURE_GROWTH_MODIFIER = getDefineINT("ROUTE_FEATURE_GROWTH_MODIFIER");
	m_iFEATURE_GROWTH_MODIFIER = getDefineINT("FEATURE_GROWTH_MODIFIER");
	m_iMIN_CITY_RANGE = getDefineINT("MIN_CITY_RANGE");
	m_iCITY_MAX_NUM_BUILDINGS = getDefineINT("CITY_MAX_NUM_BUILDINGS");
	m_iLAKE_MAX_AREA_SIZE = getDefineINT("LAKE_MAX_AREA_SIZE");
	m_iMIN_WATER_SIZE_FOR_OCEAN = getDefineINT("MIN_WATER_SIZE_FOR_OCEAN");
	m_iFORTIFY_MODIFIER_PER_TURN = getDefineINT("FORTIFY_MODIFIER_PER_TURN");
	m_iMAX_CITY_DEFENSE_DAMAGE = getDefineINT("MAX_CITY_DEFENSE_DAMAGE");
	m_iPEAK_SEE_THROUGH_CHANGE = getDefineINT("PEAK_SEE_THROUGH_CHANGE");
	m_iHILLS_SEE_THROUGH_CHANGE = getDefineINT("HILLS_SEE_THROUGH_CHANGE");
	m_iSEAWATER_SEE_FROM_CHANGE = getDefineINT("SEAWATER_SEE_FROM_CHANGE");
	m_iPEAK_SEE_FROM_CHANGE = getDefineINT("PEAK_SEE_FROM_CHANGE");
	m_iHILLS_SEE_FROM_CHANGE = getDefineINT("HILLS_SEE_FROM_CHANGE");
	m_iMAX_REBEL_YIELD_MODIFIER = getDefineINT("MAX_REBEL_YIELD_MODIFIER");
	m_iNEW_CAPACITY = getDefineINT("NEW_CAPACITY");				//VET NewCapacity - 2/3
	// TAC - AI Improved Navel AI - koma13 - START
	m_iAI_TRANSPORT_DANGER_RANGE = getDefineINT("AI_TRANSPORT_DANGER_RANGE");
	m_iAI_LOST_TRANSPORT_MEMORY_COUNT = getDefineINT("AI_LOST_TRANSPORT_MEMORY_COUNT");
	// TAC - AI Improved Navel AI - koma13 - END

	m_fCAMERA_MIN_YAW = getDefineFLOAT("CAMERA_MIN_YAW");
	m_fCAMERA_MAX_YAW = getDefineFLOAT("CAMERA_MAX_YAW");
	m_fCAMERA_FAR_CLIP_Z_HEIGHT = getDefineFLOAT("CAMERA_FAR_CLIP_Z_HEIGHT");
	m_fCAMERA_MAX_TRAVEL_DISTANCE = getDefineFLOAT("CAMERA_MAX_TRAVEL_DISTANCE");
	m_fCAMERA_START_DISTANCE = getDefineFLOAT("CAMERA_START_DISTANCE");
	m_fPLOT_SIZE = getDefineFLOAT("PLOT_SIZE");
	m_fCAMERA_SPECIAL_PITCH = getDefineFLOAT("CAMERA_SPECIAL_PITCH");
	m_fCAMERA_MAX_TURN_OFFSET = getDefineFLOAT("CAMERA_MAX_TURN_OFFSET");
	m_fCAMERA_MIN_DISTANCE = getDefineFLOAT("CAMERA_MIN_DISTANCE");
	m_fCAMERA_UPPER_PITCH = getDefineFLOAT("CAMERA_UPPER_PITCH");
	m_fCAMERA_LOWER_PITCH = getDefineFLOAT("CAMERA_LOWER_PITCH");
	m_fFIELD_OF_VIEW = getDefineFLOAT("FIELD_OF_VIEW");
	m_fUNIT_MULTISELECT_DISTANCE = getDefineFLOAT("UNIT_MULTISELECT_DISTANCE");

	m_iUSE_CAN_FOUND_CITIES_ON_WATER_CALLBACK = getDefineINT("USE_CAN_FOUND_CITIES_ON_WATER_CALLBACK");
	m_iUSE_CANNOT_DO_CIVIC_CALLBACK = getDefineINT("USE_CANNOT_DO_CIVIC_CALLBACK");
	m_iUSE_CAN_DO_CIVIC_CALLBACK = getDefineINT("USE_CAN_DO_CIVIC_CALLBACK");
	m_iUSE_CANNOT_CONSTRUCT_CALLBACK = getDefineINT("USE_CANNOT_CONSTRUCT_CALLBACK");
	m_iUSE_CAN_CONSTRUCT_CALLBACK = getDefineINT("USE_CAN_CONSTRUCT_CALLBACK");
	m_iUSE_CAN_DECLARE_WAR_CALLBACK = getDefineINT("USE_CAN_DECLARE_WAR_CALLBACK");
	m_iUSE_GET_UNIT_COST_MOD_CALLBACK = getDefineINT("USE_GET_UNIT_COST_MOD_CALLBACK");
	m_iUSE_GET_BUILDING_COST_MOD_CALLBACK = getDefineINT("USE_GET_BUILDING_COST_MOD_CALLBACK");
	m_iUSE_GET_CITY_FOUND_VALUE_CALLBACK = getDefineINT("USE_GET_CITY_FOUND_VALUE_CALLBACK");
	m_iUSE_CANNOT_HANDLE_ACTION_CALLBACK = getDefineINT("USE_CANNOT_HANDLE_ACTION_CALLBACK");
	m_iUSE_CAN_BUILD_CALLBACK = getDefineINT("USE_CAN_BUILD_CALLBACK");
	m_iUSE_CANNOT_TRAIN_CALLBACK = getDefineINT("USE_CANNOT_TRAIN_CALLBACK");
	m_iUSE_CAN_TRAIN_CALLBACK = getDefineINT("USE_CAN_TRAIN_CALLBACK");
	m_iUSE_UNIT_CANNOT_MOVE_INTO_CALLBACK = getDefineINT("USE_UNIT_CANNOT_MOVE_INTO_CALLBACK");
	m_iUSE_FINISH_TEXT_CALLBACK = getDefineINT("USE_FINISH_TEXT_CALLBACK");
	m_iUSE_ON_UNIT_SET_XY_CALLBACK = getDefineINT("USE_ON_UNIT_SET_XY_CALLBACK");
	m_iUSE_ON_UNIT_SELECTED_CALLBACK = getDefineINT("USE_ON_UNIT_SELECTED_CALLBACK");
	m_iUSE_ON_MISSIONARY_CONVERTED_UNIT_CALLBACK = getDefineINT("USE_ON_MISSIONARY_CONVERTED_UNIT_CALLBACK");
	m_iUSE_ON_UPDATE_CALLBACK = getDefineINT("USE_ON_UPDATE_CALLBACK");
	m_iUSE_ON_UNIT_CREATED_CALLBACK = getDefineINT("USE_ON_UNIT_CREATED_CALLBACK");
	m_iUSE_ON_UNIT_LOST_CALLBACK = getDefineINT("USE_ON_UNIT_LOST_CALLBACK");

	// R&R, ray, caching globals from Global Defines Alt - START
	// Caching Vanilla variables	
	m_PLOT_VISIBILITY_RANGE = getDefineINT("PLOT_VISIBILITY_RANGE");
	m_UNIT_VISIBILITY_RANGE = getDefineINT("UNIT_VISIBILITY_RANGE");
	m_MIN_CITY_YIELD_DECAY = getDefineINT("MIN_CITY_YIELD_DECAY");
	m_CITY_YIELD_DECAY_PERCENT = getDefineINT("CITY_YIELD_DECAY_PERCENT");
	m_IMMIGRATION_THRESHOLD = getDefineINT("IMMIGRATION_THRESHOLD");
	m_IMMIGRATION_THRESHOLD_INCREASE = getDefineINT("IMMIGRATION_THRESHOLD_INCREASE");
	m_TAX_TRADE_THRESHOLD = getDefineINT("TAX_TRADE_THRESHOLD");
	m_TAX_TRADE_THRESHOLD_TAX_RATE_PERCENT = getDefineINT("TAX_TRADE_THRESHOLD_TAX_RATE_PERCENT");
	m_TAX_INCREASE_CHANCE = getDefineINT("TAX_INCREASE_CHANCE");
	// TAC reduced REF
	m_ENABLE_REDUCED_REF_FOR_AI = getDefineINT("ENABLE_REDUCED_REF_FOR_AI");
	m_REDUCED_REF_PERCENT = getDefineINT("REDUCED_REF_PERCENT");
	// Domestic Market
	m_PRICE_DIFF_EUROPE_DOMESTIC_LUXURY_GOODS = getDefineINT("PRICE_DIFF_EUROPE_DOMESTIC_LUXURY_GOODS");
	m_DOMESTIC_SALES_MESSAGES = getDefineINT("DOMESTIC_SALES_MESSAGES");
	// Wild Animals	
	m_WILD_ANIMAL_LAND_TERRAIN_NATIVE_WEIGHT = getDefineINT("WILD_ANIMAL_LAND_TERRAIN_NATIVE_WEIGHT");
	m_WILD_ANIMAL_LAND_UNIT_VARIATION_WEIGHT = getDefineINT("WILD_ANIMAL_LAND_UNIT_VARIATION_WEIGHT");
	m_WILD_ANIMAL_SEA_TERRAIN_NATIVE_WEIGHT = getDefineINT("WILD_ANIMAL_SEA_TERRAIN_NATIVE_WEIGHT");
	m_WILD_ANIMAL_SEA_UNIT_VARIATION_WEIGHT = getDefineINT("WILD_ANIMAL_SEA_UNIT_VARIATION_WEIGHT");
	m_WILD_ANIMAL_REWARD_RANDOM_BASE= getDefineINT("WILD_ANIMAL_REWARD_RANDOM_BASE");
	// Max Cross Limit
	m_IMMIGRATION_MAX_CROSS_LIMIT = getDefineINT("IMMIGRATION_MAX_CROSS_LIMIT");
	// NBMOD REF
	m_NBMOD_REF_ENABLE = getDefineINT("NBMOD_REF_ENABLE");
	m_NBMOD_REF_RANDOM_SHIPS = getDefineINT("NBMOD_REF_RANDOM_SHIPS");
	m_NBMOD_REF_REAL_WEAPONS = getDefineINT("NBMOD_REF_REAL_WEAPONS");
	m_NBMOD_REF_WEAPONS_BONUS = getDefineINT("NBMOD_REF_WEAPONS_BONUS");
	m_NBMOD_REF_REVOLUTION_PERCENT_ENABLE = getDefineINT("NBMOD_REF_REVOLUTION_PERCENT_ENABLE");
	m_NBMOD_REF_MAX_TAX_ENABLE = getDefineINT("NBMOD_REF_MAX_TAX_ENABLE");
	m_NBMOD_REF_WEAPONS_BONUS_PERCENT = getDefineINT("NBMOD_REF_WEAPONS_BONUS_PERCENT");
	m_NBMOD_REF_HORSES_BONUS_PERCENT = getDefineINT("NBMOD_REF_HORSES_BONUS_PERCENT");
	m_NBMOD_REF_WEAPONS_WEIGHT = getDefineFLOAT("NBMOD_REF_WEAPONS_WEIGHT");
	m_NBMOD_REF_HORSES_WEIGHT = getDefineFLOAT("NBMOD_REF_HORSES_WEIGHT");
	m_NBMOD_REF_NUM_UNITS_PERCENT = getDefineINT("NBMOD_REF_NUM_UNITS_PERCENT");
	m_NBMOD_REF_POPULATION_WEIGHT = getDefineFLOAT("NBMOD_REF_POPULATION_WEIGHT");
	m_NBMOD_REF_TURN_WEIGHT = getDefineFLOAT("NBMOD_REF_TURN_WEIGHT");
	m_NBMOD_REF_KING_PANIC_WEIGHT = getDefineFLOAT("NBMOD_REF_KING_PANIC_WEIGHT");
	m_NBMOD_REF_START_VALUE_PERCENT = getDefineINT("NBMOD_REF_START_VALUE_PERCENT");
	m_NBMOD_REF_MAP_COEFFICIENT = getDefineINT("NBMOD_REF_MAP_COEFFICIENT");
	m_NBMOD_REF_DEBUG = getDefineINT("NBMOD_REF_DEBUG");
	// NBMOD Education
	m_EDUCATION_THRESHOLD_INCREASE= getDefineINT("EDUCATION_THRESHOLD_INCREASE");
	m_EDUCATION_THRESHOLD = getDefineINT("EDUCATION_THRESHOLD");
	// TAC Education
	m_USE_TAC_EDUCATION_COST_SYSTEM = getDefineINT("USE_TAC_EDUCATION_COST_SYSTEM");
	m_TAC_EDUCATION_RANK1 = getDefineFLOAT("TAC_EDUCATION_RANK1");
	m_TAC_EDUCATION_RANK2 = getDefineFLOAT("TAC_EDUCATION_RANK2");
	m_TAC_EDUCATION_RANK3 = getDefineFLOAT("TAC_EDUCATION_RANK3");
	// NBMod Tax
	m_MAX_TAX_RATE = getDefineINT("MAX_TAX_RATE");
	m_MIN_TAX_RATE = getDefineINT("MIN_TAX_RATE");
	m_INCREASE_MAX_TAX_RATE = getDefineINT("INCREASE_MAX_TAX_RATE");
	m_DECREASE_MAX_TAX_RATE = getDefineINT("DECREASE_MAX_TAX_RATE");
	// TAC AI hurries Immigrants
	m_AI_HURRY_IMMIGRANTS = getDefineINT("AI_HURRY_IMMIGRANTS");
	// TAC LbD
	m_LBD_BASE_CHANCE_EXPERT = getDefineINT("LBD_BASE_CHANCE_EXPERT");
	m_LBD_CHANCE_INCREASE_EXPERT = getDefineINT("LBD_CHANCE_INCREASE_EXPERT");
	m_LBD_PRE_ROUNDS_EXPERT = getDefineINT("LBD_PRE_ROUNDS_EXPERT");
	m_LBD_KI_MOD_EXPERT = getDefineINT("LBD_KI_MOD_EXPERT");
	m_LBD_BASE_CHANCE_FREE = getDefineINT("LBD_BASE_CHANCE_FREE");
	m_LBD_CHANCE_INCREASE_FREE = getDefineINT("LBD_CHANCE_INCREASE_FREE");
	m_LBD_PRE_ROUNDS_FREE = getDefineINT("LBD_PRE_ROUNDS_FREE");
	m_LBD_CHANCE_MOD_FREE_CRIMINAL = getDefineINT("LBD_CHANCE_MOD_FREE_CRIMINAL");
	m_LBD_CHANCE_MOD_FREE_SERVANT = getDefineINT("LBD_CHANCE_MOD_FREE_SERVANT");
	m_LBD_CHANCE_ESCAPE = getDefineINT("LBD_CHANCE_ESCAPE");
	m_LBD_CHANCE_MOD_ESCAPE_CRIMINAL = getDefineINT("LBD_CHANCE_MOD_ESCAPE_CRIMINAL");
	m_LBD_CHANCE_MOD_ESCAPE_SERVANT = getDefineINT("LBD_CHANCE_MOD_ESCAPE_SERVANT");
	// WTP, ray, LbD Slaves Revolt and Free - START
	m_LBD_CHANCE_REVOLT= getDefineINT("LBD_CHANCE_REVOLT");
	m_LBD_CHANCE_MOD_REVOLT_SLAVE = getDefineINT("LBD_CHANCE_MOD_REVOLT_SLAVE");
	m_LBD_CHANCE_MOD_REVOLT_CRIMINAL = getDefineINT("LBD_CHANCE_MOD_REVOLT_CRIMINAL");
	// R&R, ray, getting Veterans or Free through Combat Experience
	m_LBD_MIN_EXPERIENCE_VETERAN_BY_COMBAT = getDefineINT("LBD_MIN_EXPERIENCE_VETERAN_BY_COMBAT");
	m_LBD_MIN_EXPERIENCE_FREE_BY_COMBAT = getDefineINT("LBD_MIN_EXPERIENCE_FREE_BY_COMBAT");
	// TAC price diff raw to produced
	m_PRICE_DIFF_MAN_TO_RAW = getDefineINT("PRICE_DIFF_MAN_TO_RAW");
	// TAC KI patch
	m_AI_REBELS_FOR_REVOLUTION = getDefineINT("AI_REBELS_FOR_REVOLUTION");
	m_UNIT_REVOLUTION_RATIO = getDefineINT("UNIT_REVOLUTION_RATIO");
	m_AI_FREE_CITY_DEFENDERS = getDefineINT("AI_FREE_CITY_DEFENDERS");
	// TAC capturing Ships
	m_BASE_CHANCE_CAPTURING_SHIPS = getDefineINT("BASE_CHANCE_CAPTURING_SHIPS");
	// Min Rounds and Base Chances for DLL-Diplo-Events
	m_MIN_ROUND_NATIVE_MERCS = getDefineINT("MIN_ROUND_NATIVE_MERCS");
	m_MIN_ROUND_EUROPE_WARS = getDefineINT("MIN_ROUND_EUROPE_WARS");
	m_BASE_CHANCE_EUROPE_WARS = getDefineINT("BASE_CHANCE_EUROPE_WARS");
	m_BASE_CHANCE_CHURCH_WAR = getDefineINT("BASE_CHANCE_CHURCH_WAR");
	m_MIN_ROUND_CHURCH_CONTACT = getDefineINT("MIN_ROUND_CHURCH_CONTACT");
	m_CHURCH_CONTACT_CHANCE = getDefineINT("CHURCH_CONTACT_CHANCE");
	m_MIN_ROUND_CHURCH_WAR = getDefineINT("MIN_ROUND_CHURCH_WAR");
	m_BASE_CHANCE_EUROPE_PEACE = getDefineINT("BASE_CHANCE_EUROPE_PEACE");
	m_MIN_ROUND_PRISONS_CROWDED = getDefineINT("MIN_ROUND_PRISONS_CROWDED");
	m_PRISONS_CROWDED_CHANCE = getDefineINT("PRISONS_CROWDED_CHANCE");
	m_MIN_ROUND_REVOLUTIONARY_NOBLE = getDefineINT("MIN_ROUND_REVOLUTIONARY_NOBLE");
	m_REVOLUTIONARY_NOBLE_CHANCE = getDefineINT("REVOLUTIONARY_NOBLE_CHANCE");
	m_MIN_ROUND_SMUGGLERS_AVAILABLE = getDefineINT("MIN_ROUND_SMUGGLERS_AVAILABLE");
	m_MIN_ROUND_PIRATES = getDefineINT("MIN_ROUND_PIRATES");
	m_BASE_CHANCE_SMUGGLERS_AVAILABLE = getDefineINT("BASE_CHANCE_SMUGGLERS_AVAILABLE");
	m_BASE_CHANCE_PIRATES = getDefineINT("BASE_CHANCE_PIRATES");
	m_BASE_CHANCE_CONTINENTAL_GUARD = getDefineINT("BASE_CHANCE_CONTINENTAL_GUARD");
	m_BASE_CHANCE_MORTAR = getDefineINT("BASE_CHANCE_MORTAR");
	m_BASE_CHANCE_RANGERS_AVAILABLE = getDefineINT("BASE_CHANCE_RANGERS_AVAILABLE");
	m_BASE_CHANCE_CONQUISTADORS_AVAILABLE = getDefineINT("BASE_CHANCE_CONQUISTADORS_AVAILABLE");
	m_MIN_ROUND_NATIVE_SLAVE = getDefineINT("MIN_ROUND_NATIVE_SLAVE");
	m_MIN_ROUND_AFRICAN_SLAVES = getDefineINT("MIN_ROUND_AFRICAN_SLAVES");
	m_AFRICAN_SLAVES_CHANCE = getDefineINT("AFRICAN_SLAVES_CHANCE");
	m_MIN_ROUND_BISHOP = getDefineINT("MIN_ROUND_BISHOP");
	m_BISHOP_CHANCE = getDefineINT("BISHOP_CHANCE");
	m_PRICE_MILITIA = getDefineINT("PRICE_MILITIA");
	m_MIN_CITY_SIZE_MILITIA_OR_UNREST = getDefineINT("MIN_CITY_SIZE_MILITIA_OR_UNREST");
	m_CHANCE_MILITIA = getDefineINT("CHANCE_MILITIA");
	m_CHANCE_UNREST = getDefineINT("CHANCE_UNREST");
	m_MIN_ROUND_STEALING_IMMIGRANT = getDefineINT("MIN_ROUND_STEALING_IMMIGRANT");
	m_BASE_CHANCE_STEALING_IMMIGRANT = getDefineINT("BASE_CHANCE_STEALING_IMMIGRANT");
	// Timers for DLL-Diplo-Events
	m_TIMER_NATIVE_MERC = getDefineINT("TIMER_NATIVE_MERC");
	m_TIMER_EUROPEAN_WARS = getDefineINT("TIMER_EUROPEAN_WARS");
	m_TIMER_EUROPEAN_PEACE = getDefineINT("TIMER_EUROPEAN_PEACE");
	m_TIMER_PRISONS_CROWDED = getDefineINT("TIMER_PRISONS_CROWDED");
	m_TIMER_REVOLUTIONARY_NOBLE = getDefineINT("TIMER_REVOLUTIONARY_NOBLE");
	m_TIMER_BISHOP = getDefineINT("TIMER_BISHOP");
	m_TIMER_CHURCH_DEMAND = getDefineINT("TIMER_CHURCH_DEMAND");
	m_TIMER_CHURCH_WAR = getDefineINT("TIMER_CHURCH_WAR");
	m_TIMER_SMUGGLING_SHIP = getDefineINT("TIMER_SMUGGLING_SHIP");
	m_TIMER_PIRATES = getDefineINT("TIMER_PIRATES");
	m_TIMER_CONTINENTAL_GUARD = getDefineINT("TIMER_CONTINENTAL_GUARD");
	m_TIMER_MORTAR = getDefineINT("TIMER_MORTAR");
	m_TIMER_RANGER = getDefineINT("TIMER_RANGER");
	m_TIMER_CONQUISTADOR = getDefineINT("TIMER_CONQUISTADOR");
	m_TIMER_NATIVE_SLAVE = getDefineINT("TIMER_NATIVE_SLAVE");
	m_TIMER_AFRICAN_SLAVE = getDefineINT("TIMER_AFRICAN_SLAVE");
	m_TIMER_STEALING_IMMIGRANT = getDefineINT("TIMER_STEALING_IMMIGRANT");
	// Native Raids
	m_NATIVE_POTENTIAL_RAID_TARGET_THRESHOLD = getDefineINT("NATIVE_POTENTIAL_RAID_TARGET_THRESHOLD");
	m_NATIVE_GOODS_RAID_PERCENT = getDefineINT("NATIVE_GOODS_RAID_PERCENT");
	m_RANDOM_NATIVE_RAID_BASECHANCE = getDefineINT("RANDOM_NATIVE_RAID_BASECHANCE");
	m_NATIVE_SPARE_AI_TREASURE_CHANCE = getDefineINT("NATIVE_SPARE_AI_TREASURE_CHANCE");
	// Roundwise Native Income
	m_PER_ROUND_PER_VILLAGE_INCOME_MAX = getDefineINT("PER_ROUND_PER_VILLAGE_INCOME_MAX");
	m_PER_VILLAGE_FEATURE_GENERATION_LIMIT = getDefineINT("PER_VILLAGE_FEATURE_GENERATION_LIMIT");
	// New Storage
	m_CUSTOMHOUSE_STRATEGIC_RAW_SELL_THRESHOLD = getDefineINT("CUSTOMHOUSE_STRATEGIC_RAW_SELL_THRESHOLD");
	m_CUSTOMHOUSE_STRATEGIC_PRODUCED_SELL_THRESHOLD = getDefineINT("CUSTOMHOUSE_STRATEGIC_PRODUCED_SELL_THRESHOLD");
	m_CUSTOMHOUSE_RAW_SELL_THRESHOLD = getDefineINT("CUSTOMHOUSE_RAW_SELL_THRESHOLD");
	m_CUSTOMHOUSE_PRODUCED_SELL_THRESHOLD = getDefineINT("CUSTOMHOUSE_PRODUCED_SELL_THRESHOLD");
	// Improvement TAX mechanism
	m_TAX_TRADE_INCREASE_CHANCE_KING_ATTITUDE_BASE = getDefineINT("TAX_TRADE_INCREASE_CHANCE_KING_ATTITUDE_BASE");
	// R&R, ray, Health
	m_MIN_POP_NEG_HEALTH = getDefineINT("MIN_POP_NEG_HEALTH");
	m_POP_DIVISOR_NEG_HEALTH = getDefineINT("POP_DIVISOR_NEG_HEALTH");
	m_MAX_CITY_HEALTH = getDefineINT("MAX_CITY_HEALTH");
	m_LOWEST_CITY_HEALTH = getDefineINT("LOWEST_CITY_HEALTH");
	// R&R, ray, caching globals from Global Defines Alt - END

	// WTP, ray, Happiness - START
	m_MIN_POP_NEG_HAPPINESS = getDefineINT("MIN_POP_NEG_HAPPINESS");
	m_POP_DIVISOR_HAPPINESS = getDefineINT("POP_DIVISOR_HAPPINESS");
	m_PER_EUROPEAN_AT_WAR_UNHAPPINESS = getDefineINT("PER_EUROPEAN_AT_WAR_UNHAPPINESS");
	m_POP_DIVISOR_DEFENSE_UNHAPPINESS = getDefineINT("POP_DIVISOR_DEFENSE_UNHAPPINESS");
	m_TAX_DIVISOR_UNHAPPINESS = getDefineINT("TAX_DIVISOR_UNHAPPINESS");

	m_BASE_CHANCE_UNREST_UNHAPPINESS = getDefineINT("BASE_CHANCE_UNREST_UNHAPPINESS");
	m_BASE_CHANCE_FESTIVITIES_HAPPINESS = getDefineINT("BASE_CHANCE_FESTIVITIES_HAPPINESS");
	m_MIN_BALANCE_UNREST_UNHAPPINESS = getDefineINT("MIN_BALANCE_UNREST_UNHAPPINESS");
	m_MIN_BALANCE_FESTIVITIES_HAPPINESS = getDefineINT("MIN_BALANCE_FESTIVITIES_HAPPINESS");
	m_TURNS_UNREST_UNHAPPINESS = getDefineINT("TURNS_UNREST_UNHAPPINESS");
	m_FOUNDING_FAHTER_POINTS_FESTIVITIES_HAPPINESS = getDefineINT("FOUNDING_FAHTER_POINTS_FESTIVITIES_HAPPINESS");
	m_TIMER_FESTIVITIES_OR_UNRESTS = getDefineINT("TIMER_FESTIVITIES_OR_UNRESTS");
	// WTP, ray, Happiness - END

	m_MAX_TREASURE_AMOUNT = getDefineINT("MAX_TREASURE_AMOUNT"); // WTP, merge Treasures, of Raubwuerger
	m_TRADE_POST_GOLD_PER_NATIVE = getDefineINT("TRADE_POST_GOLD_PER_NATIVE"); // WTP, ray, Native Trade Posts - START

	// K-Mod \ RaR
	m_bUSE_AI_UNIT_UPDATE_CALLBACK = getDefineINT("USE_AI_UNIT_UPDATE_CALLBACK") != 0;
	m_bUSE_AI_DO_DIPLO_CALLBACK = getDefineINT("USE_AI_DO_DIPLO_CALLBACK") != 0;
	m_bUSE_AI_DO_WAR_CALLBACK = getDefineINT("USE_AI_DO_WAR_CALLBACK") != 0;	
	m_bUSE_DO_GROWTH_CALLBACK = getDefineINT("USE_DO_GROWTH_CALLBACK") != 0;
	m_bUSE_DO_CULTURE_CALLBACK = getDefineINT("USE_DO_CULTURE_CALLBACK") != 0;
	m_bUSE_DO_PLOT_CULTURE_CALLBACK = getDefineINT("USE_DO_PLOT_CULTURE_CALLBACK") != 0;
	m_bUSE_DO_PRODUCTION_CALLBACK = getDefineINT("USE_DO_PRODUCTION_CALLBACK") != 0;
	m_bUSE_AI_CHOOSE_PRODUCTION_CALLBACK = getDefineINT("USE_AI_CHOOSE_PRODUCTION_CALLBACK") != 0;
	m_bUSE_DO_PILLAGE_GOLD_CALLBACK = getDefineINT("USE_DO_PILLAGE_GOLD_CALLBACK") != 0;
	m_bUSE_GET_EXPERIENCE_NEEDED_CALLBACK = getDefineINT("USE_GET_EXPERIENCE_NEEDED_CALLBACK") != 0;
	m_bUSE_DO_COMBAT_CALLBACK = getDefineINT("USE_DO_COMBAT_CALLBACK") != 0;
	// K-Mod end \ RaR
}

int CvGlobals::getDefineINT( const char * szName ) const
{
	int iReturn = 0;
	GC.getDefinesVarSystem()->GetValue( szName, iReturn );
	return iReturn;
}

float CvGlobals::getDefineFLOAT( const char * szName ) const
{

	if (0 == strcmp(szName, "MINIMAP_PARABOLA_Y_INTERCEPT_PERCENT"))
	{
		static const float val = getDefineFLOAT("MINIMAP_PARABOLA_Y_INTERCEPT_PERCENT");
		return val;
	}
	else if (0 == strcmp(szName, "SADDLE_PARABOLA_MIN_Y_PERCENT"))
	{
		static const float val = getDefineFLOAT("SADDLE_PARABOLA_MIN_Y_PERCENT");
		return val;
	}
	else if (0 == strcmp(szName, "SADDLE_PARABOLA_MIN_X_PERCENT"))
	{
		static const float val = getDefineFLOAT("SADDLE_PARABOLA_MIN_X_PERCENT");
		return val;
	}
	else if (0 == strcmp(szName, "SADDLE_PARABOLA_Y_INTERCEPT_PERCENT"))
	{
		static const float val = getDefineFLOAT("SADDLE_PARABOLA_Y_INTERCEPT_PERCENT");
		return val;
	}
	else if (0 == strcmp(szName, "MINIMAP_PARABOLA_MIN_X_PERCENT"))
	{
		static const float val = getDefineFLOAT("MINIMAP_PARABOLA_MIN_X_PERCENT");
		return val;
	}
	else if (0 == strcmp(szName, "MINIMAP_PARABOLA_MIN_Y_PERCENT"))
	{
		static const float val = getDefineFLOAT("MINIMAP_PARABOLA_MIN_Y_PERCENT");
		return val;
	}
	else
	{
		float fReturn = 0;
		GC.getDefinesVarSystem()->GetValue( szName, fReturn );
		return fReturn;
	}
}

const char * CvGlobals::getDefineSTRING( const char * szName ) const
{
	const char * szReturn = NULL;
	GC.getDefinesVarSystem()->GetValue( szName, szReturn );
	return szReturn;
}

void CvGlobals::setDefineINT( const char * szName, int iValue )
{
	GC.getDefinesVarSystem()->SetValue( szName, iValue );
	cacheGlobals();
}

void CvGlobals::setDefineFLOAT( const char * szName, float fValue )
{
	GC.getDefinesVarSystem()->SetValue( szName, fValue );
	cacheGlobals();
}

void CvGlobals::setDefineSTRING( const char * szName, const char * szValue )
{
	GC.getDefinesVarSystem()->SetValue( szName, szValue );
	cacheGlobals();
}

void CvGlobals::deleteInfoArrays()
{
	for(int i=0;i<(int)m_aInfoVectors.size();i++)
	{
		deleteInfoArray(*m_aInfoVectors[i]);
	}

	//Androrc UnitArtStyles
	//this probably isn't necessary, due to the loop just before, which wasn't present in the BTS code; however, I want to be safe.
	deleteInfoArray(m_paUnitArtStyleTypeInfo);
	//Androrc End

	SAFE_DELETE_ARRAY(GC.getEntityEventTypes());
	SAFE_DELETE_ARRAY(GC.getAnimationOperatorTypes());
	SAFE_DELETE_ARRAY(GC.getFunctionTypes());
	SAFE_DELETE_ARRAY(GC.getArtStyleTypes());
	SAFE_DELETE_ARRAY(GC.getCitySizeTypes());
	SAFE_DELETE_ARRAY(GC.getContactTypes());
	SAFE_DELETE_ARRAY(GC.getDiplomacyPowerTypes());
	SAFE_DELETE_ARRAY(GC.getAutomateTypes());
	SAFE_DELETE_ARRAY(GC.getDirectionTypes());
	SAFE_DELETE_ARRAY(GC.getFootstepAudioTypes());
	SAFE_DELETE_ARRAY(GC.getFootstepAudioTags());

	clearTypesMap();
	m_aInfoVectors.clear();
}


//
// Global Infos Hash Map
//

int CvGlobals::getInfoTypeForString(const char* szType, bool hideAssert) const
{
	FAssertMsg(szType, "null info type string");

	// R&R, ray, save performance without unnecessaray logging - START
	if (strcmp(szType, "NONE")==0 || strcmp(szType, "")==0)
	{
		return -1;
	}
	// R&R, ray, save performance without unnecessaray logging - END

	InfosMap::const_iterator it = m_infosMap.find(szType);
	if (it!=m_infosMap.end())
	{
		return it->second;
	}

	if(!hideAssert)
	{
		CvString szError;
		szError.Format("info type %s not found, Current XML file is: %s", szType, GC.getCurrentXMLFile().GetCString());
		FAssertMsg(strcmp(szType, "NONE")==0 || strcmp(szType, "")==0, szError.c_str());
		gDLL->logMsg("xml.log", szError);
	}

	return -1;
}

void CvGlobals::setInfoTypeFromString(const char* szType, int idx)
{
	FAssertMsg(szType, "null info type string");
#ifdef _DEBUG
	InfosMap::const_iterator it = m_infosMap.find(szType);
	int iExisting = (it!=m_infosMap.end()) ? it->second : -1;
	FAssertMsg(iExisting==-1 || iExisting==idx || strcmp(szType, "ERROR")==0, CvString::format("xml info type entry %s already exists", szType).c_str());
#endif
	m_infosMap[szType] = idx;
}

void CvGlobals::infoTypeFromStringReset()
{
	m_infosMap.clear();
}

void CvGlobals::addToInfosVectors(void *infoVector)
{
	std::vector<CvInfoBase *> *infoBaseVector = (std::vector<CvInfoBase *> *) infoVector;
	for (unsigned int i = 0; i < m_aInfoVectors.size(); ++i)
	{
		if (m_aInfoVectors[i] == infoBaseVector)
		{
			// don't add the same twice
			return;
		}
	}
	m_aInfoVectors.push_back(infoBaseVector);
}

void CvGlobals::infosReset()
{
	for(int i=0;i<(int)m_aInfoVectors.size();i++)
	{
		std::vector<CvInfoBase *> *infoBaseVector = m_aInfoVectors[i];
		for(int j=0;j<(int)infoBaseVector->size();j++)
			infoBaseVector->at(j)->reset();
	}
}

int CvGlobals::getNumDirections() const { return NUM_DIRECTION_TYPES; }
int CvGlobals::getNumResourceLayers() const { return NUM_RESOURCE_LAYERS; }
int CvGlobals::getNumUnitLayerOptionTypes() const { return NUM_UNIT_LAYER_OPTION_TYPES; }
int CvGlobals::getNumGameOptions() const { return NUM_GAMEOPTION_TYPES; }
int CvGlobals::getNumMPOptions() const { return NUM_MPOPTION_TYPES; }
int CvGlobals::getNumSpecialOptions() const { return NUM_SPECIALOPTION_TYPES; }
int CvGlobals::getNumGraphicOptions() const { return NUM_GRAPHICOPTION_TYPES; }
int CvGlobals::getNumTradeableItems() const { return NUM_TRADEABLE_ITEMS; }
int CvGlobals::getNumBasicItems() const { return NUM_BASIC_ITEMS; }
int CvGlobals::getNumTradeableHeadings() const { return NUM_TRADEABLE_HEADINGS; }
int CvGlobals::getNumCommandInfos() const { return NUM_COMMAND_TYPES; }
int CvGlobals::getNumControlInfos() const { return NUM_CONTROL_TYPES; }
int CvGlobals::getNumMissionInfos() const { return NUM_MISSION_TYPES; }
int CvGlobals::getNumPlayerOptionInfos() const { return NUM_PLAYEROPTION_TYPES; }
int CvGlobals::getMaxNumSymbols() const { return MAX_NUM_SYMBOLS; }
int CvGlobals::getNumGraphicLevels() const { return NUM_GRAPHICLEVELS; }


//
// non-inline versions
//
CvMap& CvGlobals::getMap() { return *m_map; }
CvGameAI& CvGlobals::getGame() { return *m_game; }
CvGameAI *CvGlobals::getGamePointer(){ return m_game; }

int CvGlobals::getMaxCivPlayers() const
{
	return MAX_PLAYERS;
}

bool CvGlobals::IsGraphicsInitialized() const { return m_bGraphicsInitialized;}
void CvGlobals::SetGraphicsInitialized(bool bVal) { m_bGraphicsInitialized = bVal;}
void CvGlobals::setInterface(CvInterface* pVal) { m_interface = pVal; }
void CvGlobals::setDiplomacyScreen(CvDiplomacyScreen* pVal) { m_diplomacyScreen = pVal; }
void CvGlobals::setMPDiplomacyScreen(CMPDiplomacyScreen* pVal) { m_mpDiplomacyScreen = pVal; }
void CvGlobals::setMessageQueue(CMessageQueue* pVal) { m_messageQueue = pVal; }
void CvGlobals::setHotJoinMessageQueue(CMessageQueue* pVal) { m_hotJoinMsgQueue = pVal; }
void CvGlobals::setMessageControl(CMessageControl* pVal) { m_messageControl = pVal; }
void CvGlobals::setSetupData(CvSetupData* pVal) { m_setupData = pVal; }
void CvGlobals::setMessageCodeTranslator(CvMessageCodeTranslator* pVal) { m_messageCodes = pVal; }
void CvGlobals::setDropMgr(CvDropMgr* pVal) { m_dropMgr = pVal; }
void CvGlobals::setPortal(CvPortal* pVal) { m_portal = pVal; }
void CvGlobals::setStatsReport(CvStatsReporter* pVal) { m_statsReporter = pVal; }
void CvGlobals::setPathFinder(FAStar* pVal) { m_pathFinder = pVal; }
void CvGlobals::setInterfacePathFinder(FAStar* pVal) { m_interfacePathFinder = pVal; }
void CvGlobals::setStepFinder(FAStar* pVal) { m_stepFinder = pVal; }
void CvGlobals::setRouteFinder(FAStar* pVal) { m_routeFinder = pVal; }
void CvGlobals::setCoastalRouteFinder(FAStar* pVal) { m_coastalRouteFinder = pVal; }

void CvGlobals::setBorderFinder(FAStar* pVal) { m_borderFinder = pVal; }
void CvGlobals::setAreaFinder(FAStar* pVal) { m_areaFinder = pVal; }
CvDLLUtilityIFaceBase* CvGlobals::getDLLIFaceNonInl() { return m_pDLL; }

// PatchMod: Achievements START
int CvGlobals::getNumAchieveInfos()
{
	return (int)m_paAchieveInfo.size();
}

std::vector<CvAchieveInfo*>& CvGlobals::getAchieveInfo()
{
	return m_paAchieveInfo;
}

CvAchieveInfo& CvGlobals::getAchieveInfo(AchieveTypes eAchieve)
{
	FAssert(eAchieve > -1);
	FAssert(eAchieve < GC.getNumAchieveInfos());
	return *(m_paAchieveInfo[eAchieve]);
}
// PatchMod: Achievements END

// string cleanup
// Some xml files have references to strategy, pedia and so on for strings, which doesn't exist
// This call will remove them from memory.
void CvGlobals::cleanInfoStrings()
{
	if (GAMETEXT.getCurrentLanguage() == CvGameText::getLanguageID("Tag"))
	{
		// The Tag language reveals the TXT_KEYS
		// Removing unused strings would be detected as all strings
		return;
	}

	for (unsigned int i = 0; i < m_aInfoVectors.size(); ++i)
	{
		if (m_aInfoVectors[i]->size() > 0)
		{
			if (dynamic_cast<CvActionInfo*>((*m_aInfoVectors[i])[0]))
			{
				// action infos not supported for string cleanup.
				// Won't fix because they don't need cleaning in the first place.
				continue;
			}

			for (unsigned int j = 0; j < m_aInfoVectors[i]->size(); ++j)
			{
				(*m_aInfoVectors[i])[j]->cleanStrings();
			}
		}
	}
}



