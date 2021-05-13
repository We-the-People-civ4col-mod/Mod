#include "CvGameCoreDLL.h"

#include "CvSavegame.h"

// set the default values
	const int defaultID = 0 ;
	const int defaultX = 0 ;
	const int defaultY = 0 ;
	const int defaultRallyX = INVALID_PLOT_COORD;
	const int defaultRallyY = INVALID_PLOT_COORD;
	const int defaultGameTurnFounded = 0 ;
	const int defaultGameTurnAcquired = 0 ;
	const int defaultHighestPopulation = 0 ;
	const int defaultWorkingPopulation = 0 ;
	const int defaultNumBuildings = 0 ;
	const int defaultHealRate = 0 ;
	const int defaultFoodKept = 0 ;
	const int defaultMaxFoodKeptPercent = 0 ;
	const int defaultOverflowProduction = 0 ;
	const int defaultMilitaryProductionModifier = 0 ;
	const int defaultBuildingDefense = 0 ;
	const int defaultBuildingBombardDefense = 0 ;
	const int defaultFreeExperience = 0 ;
	const int defaultDefenseDamage = 0 ;
	const int defaultLastDefenseDamage = 0 ;
	const int defaultOccupationTimer = 0 ;
	const int defaultCultureUpdateTimer = 0 ;
	const int defaultCitySizeBoost = 0 ;
	const int defaultHammers = 0 ;
	const int defaultMissionaryRate = 0 ;
	const int defaultNativeTradeRate = 0; // WTP, ray, Native Trade Posts - START
	const int defaultTradePostGold = 0; // WTP, ray, Native Trade Posts - START
	const int defaultWorksWaterCount = 0 ;
	const int defaultRebelSentiment = 0 ;
	const int defaultCityHealth = 0 ; // R&R, ray, Health
	const int defaultCityHappiness = 0; // WTP, ray, Happiness - START
	const int defaultCityUnHappiness = 0; // WTP, ray, Happiness - START
	const int defaultCityTimerFestivitiesOrUnrest = 0; // WTP, ray, Happiness - START
	const int defaultTeachUnitMultiplier = 100 ;
	const int defaultEducationThresholdMultiplier = 100 ;
	const int defaultPopulationRank = -1;

	const bool defaultStirredUp = false; // R&R, ray , Stirring Up Natives
	const bool defaultNeverLost = true;
	const bool defaultBombarded = false;
	const bool defaultProductionAutomated = false;
	const bool defaultWallOverride = false;
	const bool defaultPopulationRankValid = false;

	const PlayerTypes defaultOwner = NO_PLAYER;
	const PlayerTypes defaultPreviousOwner = NO_PLAYER;
	const PlayerTypes defaultOriginalOwner = NO_PLAYER;
	const CultureLevelTypes defaultCultureLevel = NO_CULTURELEVEL;
	const UnitClassTypes defaultTeachUnitClass = NO_UNITCLASS;

	const PlayerTypes defaultMissionaryPlayer = NO_PLAYER;
	const PlayerTypes defaultTradePostPlayer = NO_PLAYER; // WTP, ray, Native Trade Posts - START
	const YieldTypes defaultPreferredYieldAtCityPlot = NO_YIELD;

// 
enum SavegameVariableTypes
{
	CitySave_END,
	CitySave_ID,
	CitySave_X,
	CitySave_Y,
	CitySave_RallyX,
	CitySave_RallyY,
	CitySave_GameTurnFounded,
	CitySave_GameTurnAcquired,
	CitySave_HighestPopulation,
	CitySave_WorkingPopulation,
	CitySave_NumBuildings,
	CitySave_HealRate,
	CitySave_FoodKept,
	CitySave_MaxFoodKeptPercent,
	CitySave_OverflowProduction,
	CitySave_MilitaryProductionModifier,
	CitySave_BuildingDefense,
	CitySave_BuildingBombardDefense,
	CitySave_FreeExperience,
	CitySave_DefenseDamage,
	CitySave_LastDefenseDamage,
	CitySave_OccupationTimer,
	CitySave_CultureUpdateTimer,
	CitySave_CitySizeBoost,
	CitySave_Hammers,
	CitySave_MissionaryRate,
	CitySave_NativeTradeRate, // WTP, ray, Native Trade Posts - START
	CitySave_WorksWaterCount,
	CitySave_RebelSentiment,
	CitySave_CityHealth, // R&R, ray, Health
	CitySave_CityHappiness, // WTP, ray, Happiness - START
	CitySave_CityUnHappiness, // WTP, ray, Happiness - START
	CitySave_TeachUnitMultiplier,
	CitySave_EducationThresholdMultiplier,
	CitySave_PopulationRank,

	CitySave_StirredUp, // R&R, ray , Stirring Up Natives
	CitySave_NeverLost,
	CitySave_Bombarded,
	CitySave_ProductionAutomated,
	CitySave_WallOverride,
	CitySave_PopulationRankValid,

	CitySave_Owner,
	CitySave_PreviousOwner,
	CitySave_OriginalOwner,
	CitySave_CultureLevel,
	CitySave_TeachUnitClass,

	CitySave_CustomHouseSellThreshold,
	CitySave_CustomHouseNeverSell,
	CitySave_OrderedStudents,
	CitySave_OrderedStudentsRepeat,
	CitySave_tradeImports,
	CitySave_tradeExports,
	CitySave_tradeThreshold,
	CitySave_tradeImportsMaintain,
	CitySave_tradeStopAutoImport,
	CitySave_tradeMaxThreshold,

	CitySave_MissionaryPlayer,
	CitySave_TradePostPlayer, // WTP, ray, Native Trade Posts - START
	CitySave_PreferredYieldAtCityPlot,

	CitySave_LandPlotYield,
	CitySave_SeaPlotYield,
	CitySave_RiverPlotYield,
	CitySave_YieldRateModifier,
	CitySave_YieldStored,
	CitySave_YieldRushed,
	CitySave_YieldBuyPrice,

	CitySave_BaseYieldRank,
	CitySave_BaseYieldRankValid,
	CitySave_YieldRank,
	CitySave_YieldRankValid,

	CitySave_BuildingProduction,
	CitySave_BuildingProductionTime,
	CitySave_BuildingOriginalOwner,
	CitySave_BuildingOriginalTime,
	CitySave_UnitProduction,
	CitySave_UnitProductionTime,
	CitySave_SpecialistWeights,
	CitySave_UnitCombatFreeExperience,
	CitySave_FreePromotionCount,
	CitySave_HasRealBuilding,
	CitySave_HasFreeBuilding,
	CitySave_Name,
	CitySave_ScriptData,

	CitySave_PopulationUnits,
	CitySave_EventsOccured,
	CitySave_BuildingYieldChange,

	CitySave_Culture,
	CitySave_EverOwned,
	CitySave_Revealed,
	CitySave_ScoutVisited,

	CitySave_DomainFreeExperience,
	CitySave_DomainProductionModifier,
	
	CitySave_orderQueue,

	CitySave_WorkingPlot,
	CitySave_TradePostGold,

	CitySave_CityTimerFestivitiesOrUnrest, // WTP, ray, Happiness - START

	CitySave_tradeAutoExports,

	NUM_CITYSAVE_ENUM_VALUES,
};

const char* getSavedEnumNameCity(SavegameVariableTypes eType)
{
	switch (eType)
	{
		case CitySave_END: return "CitySave_END";
		case CitySave_ID: return "CitySave_ID";
		case CitySave_X: return "CitySave_X";
		case CitySave_Y: return "CitySave_Y";
		case CitySave_RallyX: return "CitySave_RallyX";
		case CitySave_RallyY: return "CitySave_RallyY";
		case CitySave_GameTurnFounded: return "CitySave_GameTurnFounded";
		case CitySave_GameTurnAcquired: return "CitySave_GameTurnAcquired";
		case CitySave_HighestPopulation: return "CitySave_HighestPopulation";
		case CitySave_WorkingPopulation: return "CitySave_WorkingPopulation";
		case CitySave_NumBuildings: return "CitySave_NumBuildings";
		case CitySave_HealRate: return "CitySave_HealRate";
		case CitySave_FoodKept: return "CitySave_FoodKept";
		case CitySave_MaxFoodKeptPercent: return "CitySave_MaxFoodKeptPercent";
		case CitySave_OverflowProduction: return "CitySave_OverflowProduction";
		case CitySave_MilitaryProductionModifier: return "CitySave_MilitaryProductionModifier";
		case CitySave_BuildingDefense: return "CitySave_BuildingDefense";
		case CitySave_BuildingBombardDefense: return "CitySave_BuildingBombardDefense";
		case CitySave_FreeExperience: return "CitySave_FreeExperience";
		case CitySave_DefenseDamage: return "CitySave_DefenseDamage";
		case CitySave_LastDefenseDamage: return "CitySave_LastDefenseDamage";
		case CitySave_OccupationTimer: return "CitySave_OccupationTimer";
		case CitySave_CultureUpdateTimer: return "CitySave_CultureUpdateTimer";
		case CitySave_CitySizeBoost: return "CitySave_CitySizeBoost";
		case CitySave_Hammers: return "CitySave_Hammers";
		case CitySave_MissionaryRate: return "CitySave_MissionaryRate";
		case CitySave_NativeTradeRate: return "CitySave_NativeTradeRate"; // WTP, ray, Native Trade Posts - START
		case CitySave_TradePostGold: return "CitySave_TradePostGold"; // WTP, ray, Native Trade Posts - START
		case CitySave_WorksWaterCount: return "CitySave_WorksWaterCount";
		case CitySave_RebelSentiment: return "CitySave_RebelSentiment";
		case CitySave_CityHealth: return "CitySave_CityHealth";
		case CitySave_CityHappiness: return "CitySave_m_iCityHappiness"; // WTP, ray, Happiness - START
		case CitySave_CityUnHappiness: return "CitySave_m_iCityUnhappiness"; // WTP, ray, Happiness - START
		case CitySave_CityTimerFestivitiesOrUnrest: return "CitySave_CityTimerFestivitiesOrUnrest"; // WTP, ray, Happiness - START
		case CitySave_TeachUnitMultiplier: return "CitySave_TeachUnitMultiplier";
		case CitySave_EducationThresholdMultiplier: return "CitySave_EducationThresholdMultiplier";
		case CitySave_PopulationRank: return "CitySave_PopulationRank";

		case CitySave_StirredUp: return "CitySave_StirredUp";
		case CitySave_NeverLost: return "CitySave_NeverLost";
		case CitySave_Bombarded: return "CitySave_Bombarded";
		case CitySave_ProductionAutomated: return "CitySave_ProductionAutomated";
		case CitySave_WallOverride: return "CitySave_WallOverride";
		case CitySave_PopulationRankValid: return "CitySave_PopulationRankValid";

		case CitySave_Owner: return "CitySave_Owner";
		case CitySave_PreviousOwner: return "CitySave_PreviousOwner";
		case CitySave_OriginalOwner: return "CitySave_OriginalOwner";
		case CitySave_CultureLevel: return "CitySave_CultureLevel";
		case CitySave_TeachUnitClass: return "CitySave_TeachUnitClass";

		case CitySave_CustomHouseSellThreshold: return "CitySave_CustomHouseSellThreshold";
		case CitySave_CustomHouseNeverSell: return "CitySave_CustomHouseNeverSell";
		case CitySave_OrderedStudents: return "CitySave_OrderedStudents";
		case CitySave_OrderedStudentsRepeat: return "CitySave_OrderedStudentsRepeat";
		case CitySave_tradeImports: return "CitySave_tradeImports";
		case CitySave_tradeExports: return "CitySave_tradeExports";
		case CitySave_tradeAutoExports: return "CitySave_tradeAutoExports";
		case CitySave_tradeThreshold: return "CitySave_tradeThreshold";
		case CitySave_tradeImportsMaintain: return "CitySave_tradeImportsMaintain";
		case CitySave_tradeStopAutoImport: return "CitySave_tradeStopAutoImport";
		case CitySave_tradeMaxThreshold: return "CitySave_tradeMaxThreshold";

		case CitySave_MissionaryPlayer: return "CitySave_MissionaryPlayer";
		case CitySave_TradePostPlayer: return "CitySave_TradePostPlayer"; // WTP, ray, Native Trade Posts - START
		case CitySave_PreferredYieldAtCityPlot: return "CitySave_PreferredYieldAtCityPlot";

		case CitySave_LandPlotYield: return "CitySave_LandPlotYield";
		case CitySave_SeaPlotYield: return "CitySave_SeaPlotYield";
		case CitySave_RiverPlotYield: return "CitySave_RiverPlotYield";
		case CitySave_YieldRateModifier: return "CitySave_YieldRateModifier";
		case CitySave_YieldStored: return "CitySave_YieldStored";
		case CitySave_YieldRushed: return "CitySave_YieldRushed";
		case CitySave_YieldBuyPrice: return "CitySave_YieldBuyPrice";

		case CitySave_BaseYieldRank: return "CitySave_BaseYieldRank";
		case CitySave_BaseYieldRankValid: return "CitySave_BaseYieldRankValid";
		case CitySave_YieldRank: return "CitySave_YieldRank";
		case CitySave_YieldRankValid: return "CitySave_YieldRankValid";

		case CitySave_BuildingProduction: return "CitySave_BuildingProduction";
		case CitySave_BuildingProductionTime: return "CitySave_BuildingProductionTime";
		case CitySave_BuildingOriginalOwner: return "CitySave_BuildingOriginalOwner";
		case CitySave_BuildingOriginalTime: return "CitySave_BuildingOriginalTime";
		case CitySave_UnitProduction: return "CitySave_UnitProduction";
		case CitySave_UnitProductionTime: return "CitySave_UnitProductionTime";
		case CitySave_SpecialistWeights: return "CitySave_SpecialistWeights";
		case CitySave_UnitCombatFreeExperience: return "CitySave_UnitCombatFreeExperience";
		case CitySave_FreePromotionCount: return "CitySave_FreePromotionCount";
		case CitySave_HasRealBuilding: return "CitySave_HasRealBuilding";
		case CitySave_HasFreeBuilding: return "CitySave_HasFreeBuilding";
		case CitySave_Name: return "CitySave_Name";
		case CitySave_ScriptData: return "CitySave_ScriptData";

		case CitySave_PopulationUnits: return "CitySave_PopulationUnits";
		case CitySave_EventsOccured: return "CitySave_EventsOccured";
		case CitySave_BuildingYieldChange: return "CitySave_BuildingYieldChange";

		case CitySave_Culture: return "CitySave_Culture";
		case CitySave_EverOwned: return "CitySave_EverOwned";
		case CitySave_Revealed: return "CitySave_Revealed";
		case CitySave_ScoutVisited: return "CitySave_ScoutVisited";

		case CitySave_DomainFreeExperience: return "CitySave_DomainFreeExperience";
		case CitySave_DomainProductionModifier: return "CitySave_DomainProductionModifier";

		case CitySave_orderQueue: return "CitySave_orderQueue";

		case CitySave_WorkingPlot: return "CitySave_WorkingPlot";
	}
	return "";
}

int getNumSavedEnumValuesCity()
{
	return NUM_CITYSAVE_ENUM_VALUES;
}

// assign everything to default values
void CvCity::resetSavedData(int iID, PlayerTypes eOwner, int iX, int iY, bool bConstructorCall)
{
	m_iID = iID,
	m_iX = iX;
	m_iY = iY;
	m_iRallyX = defaultRallyX;
	m_iRallyY = defaultRallyY;
	m_iGameTurnFounded = defaultGameTurnFounded;
	m_iGameTurnAcquired = defaultGameTurnAcquired;
	m_iHighestPopulation = defaultHighestPopulation;
	m_iWorkingPopulation = defaultWorkingPopulation;
	m_iNumBuildings = defaultNumBuildings;
	m_iHealRate = defaultHealRate;
	m_iFoodKept = defaultFoodKept;
	m_iMaxFoodKeptPercent = defaultMaxFoodKeptPercent;
	m_iOverflowProduction = defaultOverflowProduction;
	m_iMilitaryProductionModifier = defaultMilitaryProductionModifier;
	m_iBuildingDefense = defaultBuildingDefense;
	m_iBuildingBombardDefense = defaultBuildingBombardDefense;
	m_iFreeExperience = defaultFreeExperience;
	m_iDefenseDamage = defaultDefenseDamage;
	m_iLastDefenseDamage = defaultLastDefenseDamage;
	m_iOccupationTimer = defaultOccupationTimer;
	m_iCultureUpdateTimer = defaultCultureUpdateTimer;
	m_iCitySizeBoost = defaultCitySizeBoost;
	m_iHammers = defaultHammers;
	m_iMissionaryRate = defaultMissionaryRate;
	m_iNativeTradeRate = defaultNativeTradeRate; // WTP, ray, Native Trade Posts - START
	m_iTradePostGold = defaultTradePostGold; // WTP, ray, Native Trade Posts - START
	m_iWorksWaterCount = defaultWorksWaterCount;
	m_iRebelSentiment = defaultRebelSentiment;
	m_iCityHealth = defaultCityHealth; // R&R, ray, Health
	m_iCityHappiness = defaultCityHappiness; // WTP, ray, Happiness - START
	m_iCityUnHappiness = defaultCityUnHappiness; // WTP, ray, Happiness - START
	m_iCityTimerFestivitiesOrUnrest = defaultCityTimerFestivitiesOrUnrest; // WTP, ray, Happiness - START
	m_iTeachUnitMultiplier = defaultTeachUnitMultiplier;
	m_iEducationThresholdMultiplier = defaultEducationThresholdMultiplier;
	m_iPopulationRank = defaultPopulationRank;

	m_bStirredUp = defaultStirredUp; // R&R, ray , Stirring Up Natives
	m_bNeverLost = defaultNeverLost;
	m_bBombarded = defaultBombarded;
	m_bProductionAutomated = defaultProductionAutomated;
	m_bWallOverride = defaultWallOverride;
	m_bPopulationRankValid = defaultPopulationRankValid;

	m_eOwner = eOwner;
	m_ePreviousOwner = defaultPreviousOwner;
	m_eOriginalOwner = defaultOriginalOwner;
	m_eCultureLevel = defaultCultureLevel;
	m_eTeachUnitClass = defaultTeachUnitClass;

	m_em_iCustomHouseSellThreshold.reset();
	m_em_bCustomHouseNeverSell.reset();
	m_em_iOrderedStudents.reset();
	m_em_bOrderedStudentsRepeat.reset();
	m_em_bTradeImports.reset();
	m_em_bTradeExports.reset();
	m_em_iTradeThreshold.reset();
 	m_em_bTradeImportsMaintain.reset();
	m_em_bTradeAutoExport.reset();
	m_em_bTradeStopAutoImport.reset();
	m_em_iTradeMaxThreshold.reset();

	m_eMissionaryPlayer = defaultMissionaryPlayer;
	m_eTradePostPlayer = defaultTradePostPlayer; // WTP, ray, Native Trade Posts - START
	m_ePreferredYieldAtCityPlot = defaultPreferredYieldAtCityPlot;

	m_em_iLandPlotYield.reset();
	m_em_iSeaPlotYield.reset();
	m_em_iRiverPlotYield.reset();
	m_em_iYieldRateModifier.reset();
	m_em_iYieldStored.reset();
	m_iTotalYieldStored = 0; //not stored
	m_em_iYieldRushed.reset();
	m_em_iYieldBuyPrice.reset();

	m_em_iBaseYieldRank.reset();
	m_em_bBaseYieldRankValid.reset();
	m_em_iYieldRank.reset();
	m_em_bYieldRankValid.reset();

	m_em_iBuildingProduction.reset();
	m_em_iBuildingProductionTime.reset();
	m_em_eBuildingOriginalOwner.reset();
	m_em_iBuildingOriginalTime.reset();
	m_em_iUnitProduction.reset();
	m_em_iUnitProductionTime.reset();
	m_em_iSpecialistWeights.reset();
	m_em_iUnitCombatFreeExperience.reset();
	m_em_iFreePromotionCount.reset();
	m_em_bHasRealBuilding.reset();
	m_em_bHasFreeBuilding.reset();

	m_szName.clear();
	m_szScriptData.clear();

	m_aPopulationUnits.clear();

	m_em_iCulture.reset();
	m_em_bEverOwned.reset();
	m_em_bRevealed.reset();
	m_em_bScoutVisited.reset();

	m_em_iDomainFreeExperience.reset();
	m_em_iDomainProductionModifier.reset();

	m_orderQueue.clear();

	m_em_iWorkingPlot.reset();
	m_aEventsOccured.clear();
	m_aBuildingYieldChange.clear();
}

void CvCity::read(CvSavegameReader reader)
{
	reader.AssignClassType(SAVEGAME_CLASS_CITY);

	// Init data before load
	// This will ensure that all variables not included in the savegame will have default values
	reset();
	// loop read all the variables
	// As long as each variable has a CitySavegameVariables "header", order doesn't matter.
	// Variables can be read in any order and any number of variables can be skipped.
	bool bContinue = true;
	while (bContinue)
	{
		SavegameVariableTypes eType;
		reader.Read(eType);

		switch (eType)
		{
		case CitySave_END:
			bContinue = false;
			break;
		case CitySave_ID: reader.Read(m_iID); break;
		case CitySave_X: reader.Read(m_iX); break;
		case CitySave_Y: reader.Read(m_iY); break;
		case CitySave_RallyX: reader.Read(m_iRallyX); break;
		case CitySave_RallyY: reader.Read(m_iRallyY); break;
		case CitySave_GameTurnFounded: reader.Read(m_iGameTurnFounded); break;
		case CitySave_GameTurnAcquired: reader.Read(m_iGameTurnAcquired); break;
		case CitySave_HighestPopulation: reader.Read(m_iHighestPopulation); break;
		case CitySave_WorkingPopulation: reader.Read(m_iWorkingPopulation); break;
		case CitySave_NumBuildings: reader.Read(m_iNumBuildings); break;
		case CitySave_HealRate: reader.Read(m_iHealRate); break;
		case CitySave_FoodKept: reader.Read(m_iFoodKept); break;
		case CitySave_MaxFoodKeptPercent: reader.Read(m_iMaxFoodKeptPercent); break;
		case CitySave_OverflowProduction: reader.Read(m_iOverflowProduction); break;
		case CitySave_MilitaryProductionModifier: reader.Read(m_iMilitaryProductionModifier); break;
		case CitySave_BuildingDefense: reader.Read(m_iBuildingDefense); break;
		case CitySave_BuildingBombardDefense: reader.Read(m_iBuildingBombardDefense); break;
		case CitySave_FreeExperience: reader.Read(m_iFreeExperience); break;
		case CitySave_DefenseDamage: reader.Read(m_iDefenseDamage); break;
		case CitySave_LastDefenseDamage: reader.Read(m_iLastDefenseDamage); break;
		case CitySave_OccupationTimer: reader.Read(m_iOccupationTimer); break;
		case CitySave_CultureUpdateTimer: reader.Read(m_iCultureUpdateTimer); break;
		case CitySave_CitySizeBoost: reader.Read(m_iCitySizeBoost); break;
		case CitySave_Hammers: reader.Read(m_iHammers); break;
		case CitySave_MissionaryRate: reader.Read(m_iMissionaryRate); break;
		case CitySave_NativeTradeRate: reader.Read(m_iNativeTradeRate); break;
		case CitySave_TradePostGold: reader.Read(m_iTradePostGold); break;
		case CitySave_WorksWaterCount: reader.Read(m_iWorksWaterCount); break;
		case CitySave_RebelSentiment: reader.Read(m_iRebelSentiment); break;
		case CitySave_CityHealth: reader.Read(m_iCityHealth); break; // R&R, ray, Health
		case CitySave_CityHappiness: reader.Read(m_iCityHappiness); break; // WTP, ray, Happiness - START
		case CitySave_CityUnHappiness: reader.Read(m_iCityUnHappiness); break; // WTP, ray, Happiness - START
		case CitySave_CityTimerFestivitiesOrUnrest: reader.Read(m_iCityTimerFestivitiesOrUnrest); break; // WTP, ray, Happiness - START
		case CitySave_TeachUnitMultiplier: reader.Read(m_iTeachUnitMultiplier); break;
		case CitySave_EducationThresholdMultiplier: reader.Read(m_iEducationThresholdMultiplier); break;
		case CitySave_PopulationRank: reader.Read(m_iPopulationRank); break;

		case CitySave_StirredUp: reader.Read(m_bStirredUp); break; // R&R, ray , Stirring Up Natives
		case CitySave_NeverLost: reader.Read(m_bNeverLost); break;
		case CitySave_Bombarded: reader.Read(m_bBombarded); break;
		case CitySave_ProductionAutomated: reader.Read(m_bProductionAutomated); break;
		case CitySave_WallOverride: reader.Read(m_bWallOverride); break;
		case CitySave_PopulationRankValid: reader.Read(m_bPopulationRankValid); break;

		case CitySave_Owner: reader.Read(m_eOwner); break;
		case CitySave_PreviousOwner: reader.Read(m_ePreviousOwner); break;
		case CitySave_OriginalOwner: reader.Read(m_eOriginalOwner); break;
		case CitySave_CultureLevel: reader.Read(m_eCultureLevel); break;
		case CitySave_TeachUnitClass: reader.Read(m_eTeachUnitClass); break;


		case CitySave_CustomHouseSellThreshold: reader.Read(m_em_iCustomHouseSellThreshold); break;
		case CitySave_CustomHouseNeverSell: reader.Read(m_em_bCustomHouseNeverSell); break;
		case CitySave_OrderedStudents: reader.Read(m_em_iOrderedStudents); break;
		case CitySave_OrderedStudentsRepeat: reader.Read(m_em_bOrderedStudentsRepeat); break;
		case CitySave_tradeImports: reader.Read(m_em_bTradeImports); break;
		case CitySave_tradeExports: reader.Read(m_em_bTradeExports); break;
		case CitySave_tradeAutoExports: reader.Read(m_em_bTradeAutoExport); break;
		case CitySave_tradeThreshold: reader.Read(m_em_iTradeThreshold); break;
		case CitySave_tradeImportsMaintain: reader.Read(m_em_bTradeImportsMaintain); break;
		case CitySave_tradeStopAutoImport: reader.Read(m_em_bTradeStopAutoImport); break;
		case CitySave_tradeMaxThreshold: reader.Read(m_em_iTradeMaxThreshold); break;

		case CitySave_MissionaryPlayer: reader.Read(m_eMissionaryPlayer); break;
		case CitySave_TradePostPlayer: reader.Read(m_eTradePostPlayer); break; // WTP, ray, Native Trade Posts - START
		case CitySave_PreferredYieldAtCityPlot: reader.Read(m_ePreferredYieldAtCityPlot); break;

		case CitySave_LandPlotYield: reader.Read(m_em_iLandPlotYield); break;
		case CitySave_SeaPlotYield: reader.Read(m_em_iSeaPlotYield); break;
		case CitySave_RiverPlotYield: reader.Read(m_em_iRiverPlotYield); break;
		case CitySave_YieldRateModifier: reader.Read(m_em_iYieldRateModifier); break;
		case CitySave_YieldStored: reader.Read(m_em_iYieldStored);
			for(int i=3;i<NUM_YIELD_TYPES;i++)//without YIELD_FOOD, YIELD_LUMBER, YIELD_STONE
			{
				if (GC.getYieldInfo((YieldTypes)i).isCargo())
					{m_iTotalYieldStored += m_em_iYieldStored.get((YieldTypes)i);}
			} break;
		case CitySave_YieldRushed: reader.Read(m_em_iYieldRushed); break;
		case CitySave_YieldBuyPrice: reader.Read(m_em_iYieldBuyPrice); break;

		case CitySave_BaseYieldRank: reader.Read(m_em_iBaseYieldRank); break;
		case CitySave_BaseYieldRankValid: reader.Read(m_em_bBaseYieldRankValid); break;
		case CitySave_YieldRank: reader.Read(m_em_iYieldRank); break;
		case CitySave_YieldRankValid: reader.Read(m_em_bYieldRankValid); break;

		case CitySave_BuildingProduction: reader.Read(m_em_iBuildingProduction); break;
		case CitySave_BuildingProductionTime: reader.Read(m_em_iBuildingProductionTime); break;
		case CitySave_BuildingOriginalOwner: reader.Read(m_em_eBuildingOriginalOwner); break;
		case CitySave_BuildingOriginalTime: reader.Read(m_em_iBuildingOriginalTime); break;
		case CitySave_UnitProduction: reader.Read(m_em_iUnitProduction); break;
		case CitySave_UnitProductionTime: reader.Read(m_em_iUnitProductionTime); break;
		case CitySave_SpecialistWeights: reader.Read(m_em_iSpecialistWeights); break;
		case CitySave_UnitCombatFreeExperience: reader.Read(m_em_iUnitCombatFreeExperience); break;
		case CitySave_FreePromotionCount: reader.Read(m_em_iFreePromotionCount); break;
		case CitySave_HasRealBuilding: reader.Read(m_em_bHasRealBuilding); break;
		case CitySave_HasFreeBuilding: reader.Read(m_em_bHasFreeBuilding); break;

		case CitySave_Name: reader.Read(m_szName); break;
	 	case CitySave_ScriptData: reader.Read(m_szScriptData); break;
		case CitySave_PopulationUnits: reader.Read(m_aPopulationUnits); break;
		case CitySave_EventsOccured: reader.Read(m_aEventsOccured); break;
		case CitySave_BuildingYieldChange: reader.Read(m_aBuildingYieldChange); break;
		case CitySave_Culture: reader.Read(m_em_iCulture); break;
		case CitySave_EverOwned: reader.Read(m_em_bEverOwned); break;
		case CitySave_Revealed: reader.Read(m_em_bRevealed); break;
		case CitySave_ScoutVisited: reader.Read(m_em_bScoutVisited); break;
		
		case CitySave_DomainFreeExperience: reader.Read(m_em_iDomainFreeExperience); break;
		case CitySave_DomainProductionModifier: reader.Read(m_em_iDomainProductionModifier); break;

		case CitySave_orderQueue: reader.Read(m_orderQueue); break;

		case CitySave_WorkingPlot: reader.Read(m_em_iWorkingPlot); break;

		}
		
	}
	// BUG WORKAROUND. Reset any yield, which stores negative amount
	// ideally this shouldn't be here, but it makes the asserts and error message
	// trigger when the bug triggers again rather than triggering if the bug happened prior to saving
	// This increases the odds of catching the bug in the act
	// We can always remove this later once the negative storage bug has been fixed
	//    Nightinggale
	for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
	{
		m_em_iYieldStored.keepMax(eYield, 0);
	}

	UpdateBuildingAffectedCache(); // building affected cache - Nightinggale
	this->setAutoThresholdCache(); // transport feeder - Nightinggale
	cache_storageLossTradeValues_usingRawData(); //caching storage loss trade values
	
}

void CvCity::write(CvSavegameWriter writer)
{
	writer.AssignClassType(SAVEGAME_CLASS_CITY);

	// Write the data.
	// Use WriteSwitch since it will automatically include WriteSwitch in the savegame.
	// Also it will not save anything if the variable and the default values are identical.
	// If nothing is saved, the loading code will use the default values.
	// Less data saved/loaded means smaller savegames.
	writer.Write(CitySave_ID, m_iID, defaultID);
	writer.Write(CitySave_X, m_iX, defaultX);
	writer.Write(CitySave_Y, m_iY, defaultY);
	writer.Write(CitySave_RallyX, m_iRallyX, defaultRallyX);
	writer.Write(CitySave_RallyY, m_iRallyY, defaultRallyY);
	writer.Write(CitySave_GameTurnFounded, m_iGameTurnFounded, defaultGameTurnFounded);
	writer.Write(CitySave_GameTurnAcquired, m_iGameTurnAcquired, defaultGameTurnAcquired);
	writer.Write(CitySave_HighestPopulation, m_iHighestPopulation, defaultHighestPopulation);
	writer.Write(CitySave_WorkingPopulation, m_iWorkingPopulation, defaultWorkingPopulation);
	writer.Write(CitySave_NumBuildings, m_iNumBuildings, defaultNumBuildings);
	writer.Write(CitySave_HealRate, m_iHealRate, defaultHealRate);
	writer.Write(CitySave_FoodKept, m_iFoodKept, defaultFoodKept);
	writer.Write(CitySave_MaxFoodKeptPercent, m_iMaxFoodKeptPercent, defaultMaxFoodKeptPercent);
	writer.Write(CitySave_OverflowProduction, m_iOverflowProduction, defaultOverflowProduction);
	writer.Write(CitySave_MilitaryProductionModifier, m_iMilitaryProductionModifier, defaultMilitaryProductionModifier);
	writer.Write(CitySave_BuildingDefense, m_iBuildingDefense, defaultBuildingDefense);
	writer.Write(CitySave_BuildingBombardDefense, m_iBuildingBombardDefense, defaultBuildingBombardDefense);
	writer.Write(CitySave_FreeExperience, m_iFreeExperience, defaultFreeExperience);
	writer.Write(CitySave_DefenseDamage, m_iDefenseDamage, defaultDefenseDamage);
	writer.Write(CitySave_LastDefenseDamage, m_iLastDefenseDamage, defaultLastDefenseDamage);
	writer.Write(CitySave_OccupationTimer, m_iOccupationTimer, defaultOccupationTimer);
	writer.Write(CitySave_CultureUpdateTimer, m_iCultureUpdateTimer, defaultCultureUpdateTimer);
	writer.Write(CitySave_CitySizeBoost, m_iCitySizeBoost, defaultCitySizeBoost);
	writer.Write(CitySave_Hammers, m_iHammers, defaultHammers);
	writer.Write(CitySave_MissionaryRate, m_iMissionaryRate, defaultMissionaryRate);
	writer.Write(CitySave_NativeTradeRate, m_iNativeTradeRate, defaultNativeTradeRate);
	writer.Write(CitySave_TradePostGold, m_iTradePostGold, defaultTradePostGold);
	writer.Write(CitySave_WorksWaterCount, m_iWorksWaterCount, defaultWorksWaterCount);
	writer.Write(CitySave_RebelSentiment, m_iRebelSentiment, defaultRebelSentiment);
	writer.Write(CitySave_CityHealth, m_iCityHealth, defaultCityHealth); // R&R, ray, Health
	writer.Write(CitySave_CityHappiness, m_iCityHappiness, defaultCityHappiness); // WTP, ray, Happiness - START
	writer.Write(CitySave_CityUnHappiness, m_iCityUnHappiness, defaultCityUnHappiness); // WTP, ray, Happiness - START
	writer.Write(CitySave_CityTimerFestivitiesOrUnrest, m_iCityTimerFestivitiesOrUnrest, defaultCityTimerFestivitiesOrUnrest); // WTP, ray, Happiness - START
	writer.Write(CitySave_TeachUnitMultiplier, m_iTeachUnitMultiplier, defaultTeachUnitMultiplier);
	writer.Write(CitySave_EducationThresholdMultiplier, m_iEducationThresholdMultiplier, defaultEducationThresholdMultiplier);
	writer.Write(CitySave_PopulationRank, m_iPopulationRank, defaultPopulationRank);

	writer.Write(CitySave_StirredUp, m_bStirredUp, defaultStirredUp); // R&R, ray , Stirring Up Natives
	writer.Write(CitySave_NeverLost, m_bNeverLost, defaultNeverLost);
	writer.Write(CitySave_Bombarded, m_bBombarded, defaultBombarded);
	writer.Write(CitySave_ProductionAutomated, m_bProductionAutomated, defaultProductionAutomated);
	writer.Write(CitySave_WallOverride, m_bWallOverride, defaultWallOverride);
	writer.Write(CitySave_PopulationRankValid, m_bPopulationRankValid, defaultPopulationRankValid);

	writer.Write(CitySave_Owner, m_eOwner, defaultOwner);
	writer.Write(CitySave_PreviousOwner, m_ePreviousOwner, defaultPreviousOwner); 
	writer.Write(CitySave_OriginalOwner, m_eOriginalOwner, defaultOriginalOwner); 
	writer.Write(CitySave_CultureLevel, m_eCultureLevel, defaultCultureLevel);
	writer.Write(CitySave_TeachUnitClass, m_eTeachUnitClass, defaultTeachUnitClass);

	writer.Write(CitySave_CustomHouseSellThreshold,m_em_iCustomHouseSellThreshold);
	writer.Write(CitySave_CustomHouseNeverSell,m_em_bCustomHouseNeverSell);
	writer.Write(CitySave_OrderedStudents,m_em_iOrderedStudents);
	writer.Write(CitySave_OrderedStudentsRepeat,m_em_bOrderedStudentsRepeat);
	writer.Write(CitySave_tradeImports, m_em_bTradeImports);
	writer.Write(CitySave_tradeExports, m_em_bTradeExports);
	writer.Write(CitySave_tradeAutoExports, m_em_bTradeAutoExport);
	writer.Write(CitySave_tradeThreshold, m_em_iTradeThreshold);
 	writer.Write(CitySave_tradeImportsMaintain, m_em_bTradeImportsMaintain);
	writer.Write(CitySave_tradeStopAutoImport, m_em_bTradeStopAutoImport);
	writer.Write(CitySave_tradeMaxThreshold, m_em_iTradeMaxThreshold);

	writer.Write(CitySave_MissionaryPlayer, m_eMissionaryPlayer, defaultMissionaryPlayer);
	writer.Write(CitySave_TradePostPlayer, m_eTradePostPlayer, defaultTradePostPlayer); // WTP, ray, Native Trade Posts - START
	writer.Write(CitySave_PreferredYieldAtCityPlot, m_ePreferredYieldAtCityPlot, defaultPreferredYieldAtCityPlot);
		
	writer.Write(CitySave_LandPlotYield, m_em_iLandPlotYield);
	writer.Write(CitySave_SeaPlotYield, m_em_iSeaPlotYield);
	writer.Write(CitySave_RiverPlotYield, m_em_iRiverPlotYield);
	writer.Write(CitySave_YieldRateModifier, m_em_iYieldRateModifier);
	writer.Write(CitySave_YieldStored, m_em_iYieldStored);
	writer.Write(CitySave_YieldRushed, m_em_iYieldRushed);
	writer.Write(CitySave_YieldBuyPrice, m_em_iYieldBuyPrice);

	writer.Write(CitySave_BaseYieldRank, m_em_iBaseYieldRank);
	writer.Write(CitySave_BaseYieldRankValid, m_em_bBaseYieldRankValid);
	writer.Write(CitySave_YieldRank, m_em_iYieldRank);
	writer.Write(CitySave_YieldRankValid, m_em_bYieldRankValid);

	
	writer.Write(CitySave_BuildingProduction, m_em_iBuildingProduction);
	writer.Write(CitySave_BuildingProductionTime, m_em_iBuildingProductionTime);
	writer.Write(CitySave_BuildingOriginalOwner, m_em_eBuildingOriginalOwner);
	writer.Write(CitySave_BuildingOriginalTime, m_em_iBuildingOriginalTime);
	writer.Write(CitySave_UnitProduction, m_em_iUnitProduction);
	writer.Write(CitySave_UnitProductionTime, m_em_iUnitProductionTime);
	writer.Write(CitySave_SpecialistWeights, m_em_iSpecialistWeights);
	writer.Write(CitySave_UnitCombatFreeExperience, m_em_iUnitCombatFreeExperience);
	writer.Write(CitySave_FreePromotionCount, m_em_iFreePromotionCount);
	writer.Write(CitySave_HasRealBuilding, m_em_bHasRealBuilding);
	writer.Write(CitySave_HasFreeBuilding, m_em_bHasFreeBuilding);

	writer.Write(CitySave_Name, m_szName);
	writer.Write(CitySave_ScriptData, m_szScriptData);
	writer.Write(CitySave_PopulationUnits, m_aPopulationUnits);
	writer.Write(CitySave_EventsOccured, m_aEventsOccured);
	writer.Write(CitySave_BuildingYieldChange, m_aBuildingYieldChange);
	writer.Write(CitySave_Culture, m_em_iCulture);
	writer.Write(CitySave_EverOwned, m_em_bEverOwned);
	writer.Write(CitySave_Revealed, m_em_bRevealed);
	writer.Write(CitySave_ScoutVisited, m_em_bScoutVisited);

	writer.Write(CitySave_DomainFreeExperience, m_em_iDomainFreeExperience);
	writer.Write(CitySave_DomainProductionModifier, m_em_iDomainProductionModifier);

	writer.Write(CitySave_orderQueue, m_orderQueue);

	writer.Write(CitySave_WorkingPlot, m_em_iWorkingPlot);

	writer.Write(CitySave_END);
}
