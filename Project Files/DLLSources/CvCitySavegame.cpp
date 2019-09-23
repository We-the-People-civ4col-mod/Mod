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

	const bool defaultStirredUp = false; // R&R, ray , Stirring Up Natives
	const bool defaultNeverLost = true;
	const bool defaultBombarded = false;
	const bool defaultProductionAutomated = false;
	const bool defaultWallOverride = false;


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

	CitySave_StirredUp, // R&R, ray , Stirring Up Natives
	CitySave_NeverLost,
	CitySave_Bombarded,
	CitySave_ProductionAutomated,
	CitySave_WallOverride,

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

	m_bStirredUp = defaultStirredUp; // R&R, ray , Stirring Up Natives
	m_bNeverLost = defaultNeverLost;
	m_bBombarded = defaultBombarded;
	m_bProductionAutomated = defaultProductionAutomated;
	m_bWallOverride = defaultWallOverride;
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

		case CitySave_StirredUp: reader.Read(m_bStirredUp); break; // R&R, ray , Stirring Up Natives
		case CitySave_NeverLost: reader.Read(m_bNeverLost); break;
		case CitySave_Bombarded: reader.Read(m_bBombarded); break;
		case CitySave_ProductionAutomated: reader.Read(m_bProductionAutomated); break;
		case CitySave_WallOverride: reader.Read(m_bWallOverride); break;
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

	writer.Write(CitySave_StirredUp, m_bStirredUp, defaultStirredUp); // R&R, ray , Stirring Up Natives
	writer.Write(CitySave_NeverLost, m_bNeverLost, defaultNeverLost);
	writer.Write(CitySave_Bombarded, m_bBombarded, defaultBombarded);
	writer.Write(CitySave_ProductionAutomated, m_bProductionAutomated, defaultProductionAutomated);
	writer.Write(CitySave_WallOverride, m_bWallOverride, defaultWallOverride);

	writer.Write(CitySave_END);
}
