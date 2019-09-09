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
}

void CvPlot::read(CvSavegameReader& reader)
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
		case Save_Bombarded:               reader.Read(m_bBombarded                 ); break;

		case Save_StartingPlot:            m_bStartingPlot         = reader.ReadBitfield(m_bStartingPlot            ); break;
		case Save_Hills:                   m_bHills                = reader.ReadBitfield(m_bHills                   ); break;
		case Save_NOfRiver:                m_bNOfRiver             = reader.ReadBitfield(m_bNOfRiver                ); break;
		case Save_WOfRiver:                m_bWOfRiver             = reader.ReadBitfield(m_bWOfRiver                ); break;
		case Save_PotentialCityWork:       m_bPotentialCityWork    = reader.ReadBitfield(m_bPotentialCityWork       ); break;

		}
	}
	
	// Loading done. Set up the cache (if any).

}

void CvPlot::write(CvSavegameWriter& writer)
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


	writer.Write(Save_END);
}
