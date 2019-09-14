#include "CvGameCoreDLL.h"

#include "CvSavegame.h"

// set the default values
const short defaultX = 0;
const short defaultY = 0;
const int defaultArea = FFreeList::INVALID_INDEX;
const short defaultFeatureVarity = 0;

const short defaultOwnershipDuration = 0;
const short defaultImprovementDuration = 0;
const short defaultUpgradeProgress = 0;
const short defaultForceUnownedTimer = 0;
const short defaultCityRadiusCount = 0;
const int defaultRiverID = -1;
const short defaultMinOriginalStartDist = -1;
const short defaultRiverCrossingCount = 0;
const short defaultDistanceToOcean = 0; // value to skip saving for water plots. Actual default set in reset()
const short defaultCrumbs = 0;

const int defaultCanalValue = 0;
const int defaultChokeValue = 0;
const int defaultDefenseDamage = 0;
const bool defaultBombarded = false;

const bool defaultStartingPlot = false;
const bool defaultHills = false;
const bool defaultNOfRiver = false;
const bool defaultWOfRiver = false;
const bool defaultPotentialCityWork = false;

const PlotTypes defaultePlotType = PLOT_OCEAN;
const TerrainTypes defaulteTerrainType = NO_TERRAIN;
const FeatureTypes defaulteFeatureType = NO_FEATURE;
const BonusTypes defaulteBonusType = NO_BONUS;
const ImprovementTypes defaulteImprovementType = NO_IMPROVEMENT;
const RouteTypes defaulteRouteType = NO_ROUTE;
const PlayerTypes defaulteOwner = NO_PLAYER;
const CardinalDirectionTypes defaulteRiverNSDirection = NO_CARDINALDIRECTION;
const CardinalDirectionTypes defaulteRiverWEDirection = NO_CARDINALDIRECTION;
const EuropeTypes defaulteEurope = NO_EUROPE;


enum SavegameVariableTypes
{
	Save_END,
	Save_X,
	Save_Y,
	Save_Area,
	Save_FeatureVarity,

	Save_OwnershipDuration,
	Save_ImprovementDuration,
	Save_UpgradeProgress,
	Save_ForceUnownedTimer,
	Save_CityRadiusCount,
	Save_RiverID,
	Save_MinOriginalStartDist,
	Save_RiverCrossingCount,
	Save_DistanceToOcean,
	Save_Crumbs,

	Save_CanalValue,
	Save_ChokeValue,
	Save_DefenseDamage,
	Save_Bombarded,

	Save_StartingPlot,
	Save_Hills,
	Save_NOfRiver,
	Save_WOfRiver,
	Save_PotentialCityWork,

	Save_ePlotType,
	Save_eTerrainType,
	Save_eFeatureType,
	Save_eBonusType,
	Save_eImprovementType,
	Save_eRouteType,
	Save_eOwner,
	Save_eRiverNSDirection,
	Save_eRiverWEDirection,
	Save_eEurope,

	Save_plotCity,
	Save_workingCity,
	Save_workingCityOverride,

	Save_aiYield,
	Save_Revealed,
};

// assign everything to default values
void CvPlot::resetSavedData()
{
	m_iX = defaultX;
	m_iY = defaultY;
	m_iArea = defaultArea;
	m_iFeatureVariety = defaultFeatureVarity;

	m_iOwnershipDuration = defaultOwnershipDuration;
	m_iImprovementDuration = defaultImprovementDuration;
	m_iUpgradeProgress = defaultUpgradeProgress;
	m_iForceUnownedTimer = defaultForceUnownedTimer;
	m_iCityRadiusCount = defaultCityRadiusCount;
	m_iRiverID = defaultRiverID;
	m_iMinOriginalStartDist = defaultMinOriginalStartDist;
	m_iRiverCrossingCount = defaultRiverCrossingCount;
	m_iDistanceToOcean = defaultDistanceToOcean;
	m_iCrumbs = defaultCrumbs;

	// Super Forts begin *canal* *choke*
	m_iCanalValue = defaultCanalValue;
	m_iChokeValue = defaultChokeValue;
	// Super Forts end
	// Super Forts begin *bombard*
	m_iDefenseDamage = defaultDefenseDamage;
	m_bBombarded = defaultBombarded;
	//Super Forts end

	m_bStartingPlot = defaultStartingPlot;
	m_bHills = defaultHills;
	m_bNOfRiver = defaultNOfRiver;
	m_bWOfRiver = defaultWOfRiver;
	m_bPotentialCityWork = defaultPotentialCityWork;

	m_ePlotType = defaultePlotType;
	m_eTerrainType = defaulteTerrainType;
	m_eFeatureType = defaulteFeatureType;
	m_eBonusType = defaulteBonusType;
	m_eImprovementType = defaulteImprovementType;
	m_eRouteType = defaulteRouteType;
	m_eOwner = defaulteOwner;
	m_eRiverNSDirection = defaulteRiverNSDirection;
	m_eRiverWEDirection = defaulteRiverWEDirection;
	m_eEurope = defaulteEurope;

	m_plotCity.reset();
	m_workingCity.reset();
	m_workingCityOverride.reset();

	m_pab_Revealed.reset();
}

void CvPlot::read(CvSavegameReader reader)
{
	// Init data before load
	// This will ensure that all variables not included in the savegame will have default values
	reset();

	// loop read all the variables
	// As long as each variable has a UnitSavegameVariables "header", order doesn't matter.
	// Variables can be read in any order and any number of variables can be skipped.
	bool bContinue = true;
	while (bContinue)
	{
		SavegameVariableTypes eType = reader.ReadSwitch();

		switch (eType)
		{
		case Save_END:
			bContinue = false;
			break;

		case Save_X:                       reader.Read(m_iX                         ); break;
		case Save_Y:                       reader.Read(m_iY                         ); break;
		case Save_Area:                    reader.Read(m_iArea                      ); break;
		case Save_FeatureVarity:           reader.Read(m_iFeatureVariety            ); break;

		case Save_OwnershipDuration:       reader.Read(m_iOwnershipDuration         ); break;
		case Save_ImprovementDuration:     reader.Read(m_iImprovementDuration       ); break;
		case Save_UpgradeProgress:         reader.Read(m_iUpgradeProgress           ); break;
		case Save_ForceUnownedTimer:       reader.Read(m_iForceUnownedTimer         ); break;
		case Save_CityRadiusCount:         reader.Read(m_iCityRadiusCount           ); break;
		case Save_RiverID:                 reader.Read(m_iRiverID                   ); break;
		case Save_MinOriginalStartDist:    reader.Read(m_iMinOriginalStartDist      ); break;
		case Save_RiverCrossingCount:      reader.Read(m_iRiverCrossingCount        ); break;
		case Save_DistanceToOcean:         reader.Read(m_iDistanceToOcean           ); break;
		case Save_Crumbs:                  reader.Read(m_iCrumbs                    ); break;

		case Save_CanalValue:              reader.Read(m_iCanalValue                ); break;
		case Save_ChokeValue:              reader.Read(m_iChokeValue                ); break;
		case Save_DefenseDamage:           reader.Read(m_iDefenseDamage             ); break;
		case Save_Bombarded:               m_bBombarded            = reader.ReadBitfield(m_bBombarded               ); break;

		case Save_StartingPlot:            m_bStartingPlot         = reader.ReadBitfield(m_bStartingPlot            ); break;
		case Save_Hills:                   m_bHills                = reader.ReadBitfield(m_bHills                   ); break;
		case Save_NOfRiver:                m_bNOfRiver             = reader.ReadBitfield(m_bNOfRiver                ); break;
		case Save_WOfRiver:                m_bWOfRiver             = reader.ReadBitfield(m_bWOfRiver                ); break;
		case Save_PotentialCityWork:       m_bPotentialCityWork    = reader.ReadBitfield(m_bPotentialCityWork       ); break;

		case Save_ePlotType:               m_ePlotType             = reader.ReadBitfield(m_ePlotType                ); break;
		case Save_eTerrainType:            m_eTerrainType          = reader.ReadBitfield(m_eTerrainType             ); break;
		case Save_eFeatureType:            m_eFeatureType          = reader.ReadBitfield(m_eFeatureType             ); break;
		case Save_eBonusType:              m_eBonusType            = reader.ReadBitfield(m_eBonusType               ); break;
		case Save_eImprovementType:        m_eImprovementType      = reader.ReadBitfield(m_eImprovementType         ); break;
		case Save_eRouteType:              m_eRouteType            = reader.ReadBitfield(m_eRouteType               ); break;
		case Save_eOwner:                  m_eOwner                = reader.ReadBitfield(m_eOwner                   ); break;
		case Save_eRiverNSDirection:       m_eRiverNSDirection     = reader.ReadBitfield(m_eRiverNSDirection        ); break;
		case Save_eRiverWEDirection:       m_eRiverWEDirection     = reader.ReadBitfield(m_eRiverWEDirection        ); break;
		case Save_eEurope:                 m_eEurope               = reader.ReadBitfield(m_eEurope                  ); break;
		
		case Save_plotCity:                reader.Read(m_plotCity                   ); break;
		case Save_workingCity:             reader.Read(m_workingCity                ); break;
		case Save_workingCityOverride:     reader.Read(m_workingCityOverride        ); break;

		case Save_Revealed:                reader.Read(m_pab_Revealed               ); break;

		case Save_aiYield:
		{
			// copy YieldArray into a short array
			// this way 
			YieldArray<short> temp_yield;
			reader.Read(temp_yield);
			for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
			{
				m_aiYield[eYield] = temp_yield.get(eYield);
			}
		}
		}
	}
	
	// Loading done. Set up the cache (if any).
	updateImpassable();
}

void CvPlot::write(CvSavegameWriter writer)
{
	// Write the data.
	// Use WriteSwitch since it will automatically include WriteSwitch in the savegame.
	// Also it will not save anything if the variable and the default values are identical.
	// If nothing is saved, the loading code will use the default values.
	// Less data saved/loaded means smaller savegames.

	writer.Write(Save_X, m_iX, defaultX);
	writer.Write(Save_Y, m_iY, defaultY);
	writer.Write(Save_Area, m_iArea, defaultArea);
	writer.Write(Save_FeatureVarity, m_iFeatureVariety, defaultFeatureVarity);

	writer.Write(Save_OwnershipDuration, m_iOwnershipDuration, defaultOwnershipDuration);
	writer.Write(Save_ImprovementDuration, m_iImprovementDuration, defaultImprovementDuration);
	writer.Write(Save_UpgradeProgress, m_iUpgradeProgress, defaultUpgradeProgress);
	writer.Write(Save_ForceUnownedTimer, m_iForceUnownedTimer, defaultForceUnownedTimer);
	writer.Write(Save_CityRadiusCount, m_iCityRadiusCount, defaultCityRadiusCount);
	writer.Write(Save_RiverID, m_iRiverID, defaultRiverID);
	writer.Write(Save_MinOriginalStartDist, m_iMinOriginalStartDist, defaultMinOriginalStartDist);
	writer.Write(Save_RiverCrossingCount, m_iRiverCrossingCount, defaultRiverCrossingCount);
	writer.Write(Save_DistanceToOcean, m_iDistanceToOcean, defaultDistanceToOcean);
	writer.Write(Save_Crumbs, m_iCrumbs, defaultCrumbs);

	// Super Forts begin *canal* *choke*
	writer.Write(Save_CanalValue, m_iCanalValue, defaultCanalValue);
	writer.Write(Save_ChokeValue, m_iChokeValue, defaultChokeValue);
	// Super Forts end
	// Super Forts begin *bombard*
	writer.Write(Save_DefenseDamage, m_iDefenseDamage, defaultDefenseDamage);
	writer.Write(Save_Bombarded, m_bBombarded, defaultBombarded);
	//Super Forts end

	writer.Write(Save_StartingPlot, m_bStartingPlot, defaultStartingPlot);
	writer.Write(Save_Hills, m_bHills, defaultHills);
	writer.Write(Save_NOfRiver, m_bNOfRiver, defaultNOfRiver);
	writer.Write(Save_WOfRiver, m_bWOfRiver, defaultWOfRiver);
	writer.Write(Save_PotentialCityWork, m_bPotentialCityWork, defaultPotentialCityWork);
	
	writer.Write(Save_ePlotType, m_ePlotType, defaultePlotType);
	writer.Write(Save_eTerrainType, m_eTerrainType, defaulteTerrainType);
	writer.Write(Save_eFeatureType, m_eFeatureType, defaulteFeatureType);
	writer.Write(Save_eBonusType, m_eBonusType, defaulteBonusType);
	writer.Write(Save_eImprovementType, m_eImprovementType, defaulteImprovementType);
	writer.Write(Save_eRouteType, m_eRouteType, defaulteRouteType);
	writer.Write(Save_eOwner, m_eOwner, defaulteOwner);
	writer.Write(Save_eRiverNSDirection, m_eRiverNSDirection, defaulteRiverNSDirection);
	writer.Write(Save_eRiverWEDirection, m_eRiverWEDirection, defaulteRiverWEDirection);
	writer.Write(Save_eEurope, m_eEurope, defaulteEurope);
	
	writer.Write(Save_plotCity, m_plotCity);
	writer.Write(Save_workingCity, m_workingCity);
	writer.Write(Save_workingCityOverride, m_workingCityOverride);

	// save a YieldArray instead of a short array, which happens to be of length NUM_YIELD_TYPES
	YieldArray<short> temp_yield;
	for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
	{
		temp_yield.set(m_aiYield[eYield], eYield);
	}
	writer.Write(Save_aiYield, temp_yield);

	writer.Write(Save_Revealed, m_pab_Revealed);

	writer.Write(Save_END);
}
