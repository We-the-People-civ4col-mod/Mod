#include "CvGameCoreDLL.h"

#include "CvSavegame.h"

// set the default values
const int defaultX = 0;
const int defaultY = 0;
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

const byte defaulteRiverCrossing = 0;


enum SavegameVariableTypes
{
	Save_END,
	Save_Coordinates,
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

	Save_bmRiverCrossing,

	Save_plotCity,
	Save_workingCity,
	Save_workingCityOverride,

	Save_Revealed,
	Save_RevealedImprovementRouteSingle,
	Save_RevealedImprovementRouteArray,

	Save_DangerMap,
	Save_Culture,
	Save_CultureRangeForts,
	Save_FoundValue,
	Save_PlayerCityRadiusCount,
	Save_VisibilityCount,
	Save_RevealedOwner,

	Save_CultureRangeCities,
	Save_InvisibleVisibilityCount,

	Save_ScriptData,

	Save_BuildProgress,

	Save_Units,

	NUM_SAVE_ENUM_VALUES,
};

int getNumSavedEnumValuesPlot()
{
	return NUM_SAVE_ENUM_VALUES;
}

const char* getSavedEnumNamePlot(SavegameVariableTypes eType)
{
	switch (eType)
	{
	case Save_END: return "Save_END";
	case Save_Coordinates: return "Save_Coordinates";
	case Save_Area: return "Save_Area";
	case Save_FeatureVarity: return "Save_FeatureVarity";

	case Save_OwnershipDuration: return "Save_OwnershipDuration";
	case Save_ImprovementDuration: return "Save_ImprovementDuration";
	case Save_UpgradeProgress: return "Save_UpgradeProgress";
	case Save_ForceUnownedTimer: return "Save_ForceUnownedTimer";
	case Save_CityRadiusCount: return "Save_CityRadiusCount";
	case Save_RiverID: return "Save_RiverID";
	case Save_MinOriginalStartDist: return "Save_MinOriginalStartDist";
	case Save_RiverCrossingCount: return "Save_RiverCrossingCount";
	case Save_DistanceToOcean: return "Save_DistanceToOcean";
	case Save_Crumbs: return "Save_Crumbs";

	case Save_CanalValue: return "Save_CanalValue";
	case Save_ChokeValue: return "Save_ChokeValue";
	case Save_DefenseDamage: return "Save_DefenseDamage";
	case Save_Bombarded: return "Save_Bombarded";

	case Save_StartingPlot: return "Save_StartingPlot";
	case Save_Hills: return "Save_Hills";
	case Save_NOfRiver: return "Save_NOfRiver";
	case Save_WOfRiver: return "Save_WOfRiver";
	case Save_PotentialCityWork: return "Save_PotentialCityWork";

	case Save_ePlotType: return "Save_ePlotType";
	case Save_eTerrainType: return "Save_eTerrainType";
	case Save_eFeatureType: return "Save_eFeatureType";
	case Save_eBonusType: return "Save_eBonusType";
	case Save_eImprovementType: return "Save_eImprovementType";
	case Save_eRouteType: return "Save_eRouteType";
	case Save_eOwner: return "Save_eOwner";
	case Save_eRiverNSDirection: return "Save_eRiverNSDirection";
	case Save_eRiverWEDirection: return "Save_eRiverWEDirection";
	case Save_eEurope: return "Save_eEurope";
	case Save_bmRiverCrossing: return "Save_bmRiverCrossing";

	case Save_plotCity: return "Save_plotCity";
	case Save_workingCity: return "Save_workingCity";
	case Save_workingCityOverride: return "Save_workingCityOverride";

	case Save_Revealed: return "Save_Revealed";
	case Save_RevealedImprovementRouteSingle: return "Save_RevealedImprovementRouteSingle";
	case Save_RevealedImprovementRouteArray: return "Save_RevealedImprovementRouteArray";

	case Save_DangerMap: return "Save_DangerMap";
	case Save_Culture: return "Save_Culture";
	case Save_CultureRangeForts: return "Save_CultureRangeForts";
	case Save_FoundValue: return "Save_FoundValue";
	case Save_PlayerCityRadiusCount: return "Save_PlayerCityRadiusCount";

	case Save_VisibilityCount: return "Save_VisibilityCount";
	case Save_RevealedOwner: return "Save_RevealedOwner";

	case Save_CultureRangeCities: return "Save_CultureRangeCities";
	case Save_InvisibleVisibilityCount: "Save_InvisibleVisibilityCount";

	case Save_ScriptData: return "Save_ScriptData";

	case Save_BuildProgress: return "Save_BuildProgress";

	case Save_Units: return "Save_Units";
	}
	FAssertMsg(0, "Missing case");
	return "";
}

// assign everything to default values
void CvPlot::resetSavedData()
{
	m_coord.set(defaultX, defaultY);
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

	m_bmRiverCrossing = defaulteRiverCrossing;

	m_plotCity.reset();
	m_workingCity.reset();
	m_workingCityOverride.reset();

	m_em_bRevealed.reset();
	m_aeRevealedImprovementRouteTypes.reset();
}

void CvPlot::read(CvSavegameReader reader)
{
	reader.AssignClassType(SAVEGAME_CLASS_PLOT);

	// Init data before load
	// This will ensure that all variables not included in the savegame will have default values
	reset();

	CvString szClassName;
	if (reader.isDebug())
	{
		reader.Read(szClassName);
	}

	// loop read all the variables
	// As long as each variable has a UnitSavegameVariables "header", order doesn't matter.
	// Variables can be read in any order and any number of variables can be skipped.
	bool bContinue = true;
	while (bContinue)
	{
		SavegameVariableTypes eType;
		reader.Read(eType);

		switch (eType)
		{
		case Save_END:
			bContinue = false;
			break;

		case Save_Coordinates:             reader.Read(m_coord                      ); break;
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
		case Save_bmRiverCrossing:         reader.Read(m_bmRiverCrossing            ); break;
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

		case Save_Revealed:                reader.Read(m_em_bRevealed               ); break;

		case Save_RevealedImprovementRouteSingle:
		{
			// saving out of date data for a team
			TeamTypes eTeam;
			ImprovementTypes eImprovement;
			RouteTypes eRoute;
			reader.Read(eTeam);
			reader.Read(eImprovement);
			reader.Read(eRoute);
			m_aeRevealedImprovementRouteTypes.set(eTeam, eImprovement, eRoute);
		} break;


		case Save_RevealedImprovementRouteArray:
		{
			// what follows now is a PlayerBoolArray with all the teams, which have their knowledge up to date
			// loop this array and assign the already loaded improvement and route
			// note that not being in the array shouldn't alter the memory at all
			// it's either out of date (set in Save_RevealedImprovementRouteSingle) or no info (default value)
			// writing something means overwriting what is stored using Save_RevealedImprovementRouteSingle.
			EnumMap<TeamTypes, bool> eTeamArray;
			const ImprovementTypes eImprovement = getImprovementType();
			const RouteTypes eRoute = getRouteType();
			reader.Read(eTeamArray);
			for (TeamTypes eTeam = FIRST_TEAM; eTeam < NUM_TEAM_TYPES; ++eTeam)
			{
				if (eTeamArray.get(eTeam))
				{
					m_aeRevealedImprovementRouteTypes.set(eTeam, eImprovement, eRoute);
				}
			}
		} break;

		// PlayerArrays
		case Save_Culture                  : reader.Read(m_em_iCulture)                       ; break;
		case Save_CultureRangeForts        : reader.Read(m_em_iCultureRangeForts)             ; break;
		case Save_DangerMap                : reader.Read(m_em_iDangerMap)                     ; break;
		case Save_FoundValue               : reader.Read(m_em_iFoundValue)                    ; break;
		case Save_PlayerCityRadiusCount    : reader.Read(m_em_iPlayerCityRadiusCount)         ; break;
		case Save_VisibilityCount          : reader.Read(m_em_iVisibilityCount)               ; break;
		case Save_RevealedOwner            : reader.Read(m_em_eRevealedOwner)                 ; break;

		case Save_CultureRangeCities       : reader.Read(m_em2_iCultureRangeCities)           ; break;
		case Save_InvisibleVisibilityCount : reader.Read(m_em2_iInvisibleVisibilityCount)     ; break;

		case Save_ScriptData               : reader.Read(m_szScriptData)                      ; break;

		case Save_BuildProgress            : reader.Read(m_em_iBuildProgress)                 ; break;

		case Save_Units                    : reader.Read(m_units)                             ; break;

		default:
			FAssertMsg(false, "Unhandled savegame enum");
			break;
		}
	}

	// Loading done. Set up the cache (if any).
	updateImpassable();

	setSeeFromLevelCache();
	setSeeThroughLevelCache();
	setPlotVisibilityCache();
	setUnitVisibilityBonusCache();
}

void CvPlot::write(CvSavegameWriter writer) const
{
	LogIntentHelper helper(writer, "CvPlot");

	writer.AssignClassType(SAVEGAME_CLASS_PLOT);

	if (writer.isDebug())
	{
		writer.Write("CvPlot");
	}

	// Write the data.
	// Use WriteSwitch since it will automatically include WriteSwitch in the savegame.
	// Also it will not save anything if the variable and the default values are identical.
	// If nothing is saved, the loading code will use the default values.
	// Less data saved/loaded means smaller savegames.

	// m_pPlotArea not saved

	// m_bShowCitySymbols not saved
	// m_bFlagDirty not saved
	// m_bPlotLayoutDirty not saved
	// m_bLayoutStateWorked not saved
	// m_bImpassable not saved

	writer.Write(Save_Coordinates, m_coord);
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

	writer.Write(Save_bmRiverCrossing, m_bmRiverCrossing, defaulteRiverCrossing);


	writer.Write(Save_plotCity, m_plotCity);
	writer.Write(Save_workingCity, m_workingCity);
	writer.Write(Save_workingCityOverride, m_workingCityOverride);

	// m_em_iYield recalculated on load

	writer.Write(Save_Revealed, m_em_bRevealed);

	if (m_aeRevealedImprovementRouteTypes.isAllocated())
	{
		EnumMap<TeamTypes, bool> eTeamArray;
		const ImprovementTypes ePlotImprovement = getImprovementType();
		const RouteTypes ePlotRoute = getRouteType();

		for (TeamTypes eTeam = FIRST_TEAM; eTeam < NUM_TEAM_TYPES; ++eTeam)
		{
			const ImprovementTypes eImprovement = m_aeRevealedImprovementRouteTypes.getImprovement(eTeam);
			const RouteTypes eRoute = m_aeRevealedImprovementRouteTypes.getRoute(eTeam);

			if (eImprovement != NO_IMPROVEMENT || eRoute != NO_ROUTE)
			{
				// team believes there is something on the plot. It needs to be saved what they believe the plot to contain
				// not saving anything makes the team believe there is nothing (including not explored)
				if (eImprovement == ePlotImprovement && eRoute == ePlotRoute)
				{
					// the team's knowledge is up to date
					// save a single bit, which states this
					eTeamArray.set(eTeam, true);
				}
				else
				{
					// team believes there is something on the plot, but it's not up to date
					// the only way to save this is to save what the team believes explicitly
					writer.Write(Save_RevealedImprovementRouteSingle);
					writer.Write(eTeam);
					writer.Write(eImprovement);
					writer.Write(eRoute);
				}
			}
		}
		// save which teams have knowledge, which is up to date
		writer.Write(Save_RevealedImprovementRouteArray, eTeamArray);
	}

	writer.Write(Save_DangerMap, m_em_iDangerMap);
	writer.Write(Save_Culture, m_em_iCulture);
	writer.Write(Save_CultureRangeForts, m_em_iCultureRangeForts);
	writer.Write(Save_FoundValue, m_em_iFoundValue);
	writer.Write(Save_PlayerCityRadiusCount, m_em_iPlayerCityRadiusCount);
	// m_em_iVisibilityCount recalculated on load
	writer.Write(Save_RevealedOwner, m_em_eRevealedOwner);

	writer.Write(Save_CultureRangeCities, m_em2_iCultureRangeCities);
	writer.Write(Save_InvisibleVisibilityCount, m_em2_iInvisibleVisibilityCount);

	writer.Write(Save_ScriptData, m_szScriptData);

	writer.Write(Save_BuildProgress, m_em_iBuildProgress);

	writer.Write(Save_Units, m_units);

	writer.Write(Save_END);
}
