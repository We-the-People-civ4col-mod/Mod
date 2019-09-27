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
	const int defaultWorksWaterCount = 0 ;
	const int defaultRebelSentiment = 0 ;
	const int defaultCityHealth = 0 ; // R&R, ray, Health
	const int defaultTeachUnitMultiplier = 100 ;
	const int defaultEducationThresholdMultiplier = 100 ;
	const int defaultPopulationRank = -1;

	const bool defaultStirredUp = false; // R&R, ray , Stirring Up Natives
	const bool defaultNeverLost = true;
	const bool defaultBombarded = false;
	const bool defaultProductionAutomated = false;
	const bool defaultWallOverride = false;
	const bool defaultPopulationRankValid = false;

	const PlayerTypes defaultOwner = PlayerTypes();
	const PlayerTypes defaultPreviousOwner = NO_PLAYER;
	const PlayerTypes defaultOriginalOwner = PlayerTypes();
	const CultureLevelTypes defaultCultureLevel = NO_CULTURELEVEL;
	const UnitClassTypes defaultTeachUnitClass = NO_UNITCLASS;

	const PlayerTypes defaultMissionaryPlayer = NO_PLAYER;
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
	CitySave_WorksWaterCount,
	CitySave_RebelSentiment,
	CitySave_CityHealth, // R&R, ray, Health
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
	
	NUM_SAVE_ENUM_VALUES,
};

int getNumSavedEnumValuesCity()
{
	return NUM_SAVE_ENUM_VALUES;
}

// assign everything to default values
void CvCity::resetSavedData()
{
	m_iID = defaultID;
	m_iX = defaultX;
	m_iY = defaultY;
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
	m_iWorksWaterCount = defaultWorksWaterCount;
	m_iRebelSentiment = defaultRebelSentiment;
	m_iCityHealth = defaultCityHealth; // R&R, ray, Health
	m_iTeachUnitMultiplier = defaultTeachUnitMultiplier;
	m_iEducationThresholdMultiplier = defaultEducationThresholdMultiplier;
	m_iPopulationRank = defaultPopulationRank;

	m_bStirredUp = defaultStirredUp; // R&R, ray , Stirring Up Natives
	m_bNeverLost = defaultNeverLost;
	m_bBombarded = defaultBombarded;
	m_bProductionAutomated = defaultProductionAutomated;
	m_bWallOverride = defaultWallOverride;
	m_bPopulationRankValid = defaultPopulationRankValid;

	m_eOwner = defaultOwner;
	m_ePreviousOwner = defaultPreviousOwner;
	m_eOriginalOwner = defaultOriginalOwner;
	m_eCultureLevel = defaultCultureLevel;
	m_eTeachUnitClass = defaultTeachUnitClass;

	ma_aiCustomHouseSellThreshold.reset();
	ba_aiCustomHouseNeverSell.reset();
	ma_OrderedStudents.reset();
	ba_OrderedStudentsRepeat.reset();
	ba_tradeImports.reset();
	ba_tradeExports.reset();
	ma_tradeThreshold.reset();
 	ba_tradeImportsMaintain.reset();
	ba_tradeStopAutoImport.reset();
	ma_tradeMaxThreshold.reset();

	m_eMissionaryPlayer = defaultMissionaryPlayer;
	m_ePreferredYieldAtCityPlot = defaultPreferredYieldAtCityPlot;

	m_aiLandPlotYield.reset();
	m_aiSeaPlotYield.reset();
	m_aiRiverPlotYield.reset();
	m_aiYieldRateModifier.reset();
	m_aiYieldStored.reset();
	m_iTotalYieldStored = 0; //not stored
	m_aiYieldRushed.reset();
	m_aiYieldBuyPrice.reset();

	m_aiBaseYieldRank.reset();
	m_abBaseYieldRankValid.reset();
	m_aiYieldRank.reset();
	m_abYieldRankValid.reset();

	m_paiBuildingProduction.reset();
	m_paiBuildingProductionTime.reset();
	m_paiBuildingOriginalOwner.reset();
	m_paiBuildingOriginalTime.reset();
	m_paiUnitProduction.reset();
	m_paiUnitProductionTime.reset();
	m_aiSpecialistWeights.reset();
	m_paiUnitCombatFreeExperience.reset();
	m_paiFreePromotionCount.reset();
	m_pabHasRealBuilding.reset();
	m_pabHasFreeBuilding.reset();

	m_szName.clear();
	m_szScriptData.clear();
}

void CvCity::read(CvSavegameReader reader)
{
	reader.AssignClassType(SAVEGAME_CLASS_CITY);

	// Init data before load
	// This will ensure that all variables not included in the savegame will have default values
	reset();
	resetSavedData();
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
		case CitySave_WorksWaterCount: reader.Read(m_iWorksWaterCount); break;
		case CitySave_RebelSentiment: reader.Read(m_iRebelSentiment); break;
		case CitySave_CityHealth: reader.Read(m_iCityHealth); break; // R&R, ray, Health
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


		case CitySave_CustomHouseSellThreshold: reader.Read(ma_aiCustomHouseSellThreshold); break;
		case CitySave_CustomHouseNeverSell: reader.Read(ba_aiCustomHouseNeverSell); break;
		case CitySave_OrderedStudents: reader.Read(ma_OrderedStudents); break;
		case CitySave_OrderedStudentsRepeat: reader.Read(ba_OrderedStudentsRepeat); break;
		case CitySave_tradeImports: reader.Read(ba_tradeImports); break;
		case CitySave_tradeExports: reader.Read(ba_tradeExports); break;
		case CitySave_tradeThreshold: reader.Read(ma_tradeThreshold); break;
		case CitySave_tradeImportsMaintain: reader.Read(ba_tradeImportsMaintain); break;
		case CitySave_tradeStopAutoImport: reader.Read(ba_tradeStopAutoImport); break;
		case CitySave_tradeMaxThreshold: reader.Read(ma_tradeMaxThreshold); break;

		case CitySave_MissionaryPlayer: reader.Read(m_eMissionaryPlayer); break;
		case CitySave_PreferredYieldAtCityPlot: reader.Read(m_ePreferredYieldAtCityPlot); break;

		case CitySave_LandPlotYield: reader.Read(m_aiLandPlotYield); break;
		case CitySave_SeaPlotYield: reader.Read(m_aiSeaPlotYield); break;
		case CitySave_RiverPlotYield: reader.Read(m_aiRiverPlotYield); break;
		case CitySave_YieldRateModifier: reader.Read(m_aiYieldRateModifier); break;
		case CitySave_YieldStored: reader.Read(m_aiYieldStored);
			for(int i=3;i<NUM_YIELD_TYPES;i++)//without YIELD_FOOD, YIELD_LUMBER, YIELD_STONE
			{
				if (GC.getYieldInfo((YieldTypes)i).isCargo())
					{m_iTotalYieldStored += m_aiYieldStored.get(i);}
			} break;
		case CitySave_YieldRushed: reader.Read(m_aiYieldRushed); break;
		case CitySave_YieldBuyPrice: reader.Read(m_aiYieldBuyPrice); break;

		case CitySave_BaseYieldRank: reader.Read(m_aiBaseYieldRank); break;
		case CitySave_BaseYieldRankValid: reader.Read(m_abBaseYieldRankValid); break;
		case CitySave_YieldRank: reader.Read(m_aiYieldRank); break;
		case CitySave_YieldRankValid: reader.Read(m_abYieldRankValid); break;

		case CitySave_BuildingProduction: reader.Read(m_paiBuildingProduction); break;
		case CitySave_BuildingProductionTime: reader.Read(m_paiBuildingProductionTime); break;
		case CitySave_BuildingOriginalOwner: reader.Read(m_paiBuildingOriginalOwner); break;
		case CitySave_BuildingOriginalTime: reader.Read(m_paiBuildingOriginalTime); break;
		case CitySave_UnitProduction: reader.Read(m_paiUnitProduction); break;
		case CitySave_UnitProductionTime: reader.Read(m_paiUnitProductionTime); break;
		case CitySave_SpecialistWeights: reader.Read(m_aiSpecialistWeights); break;
		case CitySave_UnitCombatFreeExperience: reader.Read(m_paiUnitCombatFreeExperience); break;
		case CitySave_FreePromotionCount: reader.Read(m_paiFreePromotionCount); break;
		case CitySave_HasRealBuilding: reader.Read(m_pabHasRealBuilding); break;
		case CitySave_HasFreeBuilding: reader.Read(m_pabHasFreeBuilding); break;

		case CitySave_Name: reader.Read(m_szName); break;
	 	case CitySave_ScriptData: reader.Read(m_szScriptData); break;
		}
	}
	
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
	writer.Write(CitySave_WorksWaterCount, m_iWorksWaterCount, defaultWorksWaterCount);
	writer.Write(CitySave_RebelSentiment, m_iRebelSentiment, defaultRebelSentiment);
	writer.Write(CitySave_CityHealth, m_iCityHealth, defaultCityHealth); // R&R, ray, Health
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

	writer.Write(CitySave_CustomHouseSellThreshold,ma_aiCustomHouseSellThreshold);
	writer.Write(CitySave_CustomHouseNeverSell,ba_aiCustomHouseNeverSell);
	writer.Write(CitySave_OrderedStudents,ma_OrderedStudents);
	writer.Write(CitySave_OrderedStudentsRepeat,ba_OrderedStudentsRepeat);
	writer.Write(CitySave_tradeImports, ba_tradeImports);
	writer.Write(CitySave_tradeExports, ba_tradeExports);
	writer.Write(CitySave_tradeThreshold, ma_tradeThreshold);
 	writer.Write(CitySave_tradeImportsMaintain, ba_tradeImportsMaintain);
	writer.Write(CitySave_tradeStopAutoImport, ba_tradeStopAutoImport);
	writer.Write(CitySave_tradeMaxThreshold, ma_tradeMaxThreshold);

	writer.Write(CitySave_MissionaryPlayer, m_eMissionaryPlayer, defaultMissionaryPlayer);
	writer.Write(CitySave_PreferredYieldAtCityPlot, m_ePreferredYieldAtCityPlot, defaultPreferredYieldAtCityPlot);
		
	writer.Write(CitySave_LandPlotYield, m_aiLandPlotYield);
	writer.Write(CitySave_SeaPlotYield, m_aiSeaPlotYield);
	writer.Write(CitySave_RiverPlotYield, m_aiRiverPlotYield);
	writer.Write(CitySave_YieldRateModifier, m_aiYieldRateModifier);
	writer.Write(CitySave_YieldStored, m_aiYieldStored);
	writer.Write(CitySave_YieldRushed, m_aiYieldRushed);
	writer.Write(CitySave_YieldBuyPrice, m_aiYieldBuyPrice);

	writer.Write(CitySave_BaseYieldRank, m_aiBaseYieldRank);
	writer.Write(CitySave_BaseYieldRankValid, m_abBaseYieldRankValid);
	writer.Write(CitySave_YieldRank, m_aiYieldRank);
	writer.Write(CitySave_YieldRankValid, m_abYieldRankValid);

	
	writer.Write(CitySave_BuildingProduction, m_paiBuildingProduction);
	writer.Write(CitySave_BuildingProductionTime, m_paiBuildingProductionTime);
	writer.Write(CitySave_BuildingOriginalOwner, m_paiBuildingOriginalOwner);
	writer.Write(CitySave_BuildingOriginalTime, m_paiBuildingOriginalTime);
	writer.Write(CitySave_UnitProduction, m_paiUnitProduction);
	writer.Write(CitySave_UnitProductionTime, m_paiUnitProductionTime);
	writer.Write(CitySave_SpecialistWeights, m_aiSpecialistWeights);
	writer.Write(CitySave_UnitCombatFreeExperience, m_paiUnitCombatFreeExperience);
	writer.Write(CitySave_FreePromotionCount, m_paiFreePromotionCount);
	writer.Write(CitySave_HasRealBuilding, m_pabHasRealBuilding);
	writer.Write(CitySave_HasFreeBuilding, m_pabHasFreeBuilding);

	writer.Write(CitySave_Name, m_szName);
	writer.Write(CitySave_ScriptData, m_szScriptData);

	writer.Write(CitySave_END);
}
