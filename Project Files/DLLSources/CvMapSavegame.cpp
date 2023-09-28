#include "CvGameCoreDLL.h"

#include "CvSavegame.h"

// set the default values
const int defaultGridWidth = 0;
const int defaultGridHeight = 0;
const int defaultLandPlots = 0;
const int defaultOwnedPlots = 0;
const int defaultTopLatitude = 0;
const int defaultBottomLatitude = 0;
const int defaultNextRiverID = 0;

const bool defaultWrapX = false;
const bool defaultWrapY = false;

enum SavegameVariableTypes
{
	Save_END,
	Save_GridWidth,
	Save_GridHeight,
	Save_LandPlots,
	Save_OwnedPlots,
	Save_TopLatitude,
	Save_BottomLatitude,
	Save_NextRiverID,

	Save_WrapX,
	Save_WrapY,
	Save_UseTwoPlotCities,

	Save_Areas,

	Save_Plots,

	NUM_SAVE_ENUM_VALUES,
};

int getNumSavedEnumValuesMap()
{
	return NUM_SAVE_ENUM_VALUES;
}

const char* getSavedEnumNameMap(SavegameVariableTypes eType)
{
	switch (eType)
	{
	case Save_END: return "Save_END";
	case Save_GridWidth: return "Save_GridWidth";
	case Save_GridHeight: return "Save_GridHeight";
	case Save_LandPlots: return "Save_LandPlots";
	case Save_OwnedPlots: return "Save_OwnedPlots";
	case Save_TopLatitude: return "Save_TopLatitude";
	case Save_BottomLatitude: return "Save_BottomLatitude";
	case Save_NextRiverID: return "Save_NextRiverID";

	case Save_WrapX: return "Save_WrapX";
	case Save_WrapY: return "Save_WrapY";
	case Save_UseTwoPlotCities: return "Save_UseTwoPlotCities";

	case Save_Areas: return "Save_Areas";
	case Save_Plots: return "Save_Plots";
	}
	FAssertMsg(0, "Missing case");
	return "";
}

// assign everything to default values
void CvMap::resetSavedData()
{
	m_iGridWidth        = defaultGridWidth;
	m_iGridHeight       = defaultGridHeight;
	m_iLandPlots        = defaultLandPlots;
	m_iOwnedPlots       = defaultOwnedPlots;
	m_iTopLatitude      = defaultTopLatitude;
	m_iBottomLatitude   = defaultBottomLatitude;
	m_iNextRiverID      = defaultNextRiverID;

	m_bWrapX            = defaultWrapX;
	m_bWrapY            = defaultWrapY;

	m_ja_NumBonuses.reset();
	m_ja_NumBonusesOnLand.reset();
}

void CvMap::read(CvSavegameReader reader)
{
	reader.AssignClassType(SAVEGAME_CLASS_MAP);

	// Init data before load
	// This will ensure that all variables not included in the savegame will have default values
	//reset();

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
		case Save_GridWidth:           reader.Read(m_iGridWidth);          break;
		case Save_GridHeight:          reader.Read(m_iGridHeight);         break;
		case Save_LandPlots:           reader.Read(m_iLandPlots);          break;
		case Save_OwnedPlots:          reader.Read(m_iOwnedPlots);         break;
		case Save_TopLatitude:         reader.Read(m_iTopLatitude);        break;
		case Save_BottomLatitude:      reader.Read(m_iBottomLatitude);     break;
		case Save_NextRiverID:         reader.Read(m_iNextRiverID);        break;

		case Save_WrapX:               reader.Read(m_bWrapX);              break;
		case Save_WrapY:               reader.Read(m_bWrapY);              break;
		
		case Save_UseTwoPlotCities:
		{
			char iBuffer = 0;
			reader.Read(iBuffer);
			setCityCatchmentRadius(iBuffer);
			break;
		}
		case Save_Plots:
		{
			FAssertMsg(m_pMapPlots == NULL, "Memory leak");
			const int iNumPlots = numPlotsINLINE();
			m_pMapPlots = new CvPlot[iNumPlots];
			for (int iI = 0; iI < iNumPlots; ++iI)
			{
				m_pMapPlots[iI].read(reader);
			}
			break;
		}
		case Save_Areas:
			// call the read of the free list CvArea class allocations
			ReadStreamableFFreeListTrashArray(m_areas, reader);
			break;

		default:
			FAssertMsg(false, "Unhandled savegame enum");
			break;
		}
	}
	
	// Loading done. Set up the cache (if any).

	{
		// Assign cache for bonuses and improvements in areas and map
		// Those data are saved in CvPlot, not all 3 classes
		const int iNumPlots = numPlotsINLINE();
		for (int iI = 0; iI < iNumPlots; ++iI)
		{
			const CvPlot& kPlot = m_pMapPlots[iI];
			const BonusTypes eBonus = kPlot.getBonusType();
			const ImprovementTypes eImprovement = kPlot.getImprovementType();

			if (eBonus != NO_BONUS || eImprovement != NO_IMPROVEMENT)
			{
				CvArea* pArea = getArea(kPlot.getArea());
				if (eBonus != NO_BONUS)
				{
					pArea->changeNumBonuses(eBonus, 1);
					changeNumBonuses(eBonus, 1);
					if (!kPlot.isWater())
					{
						changeNumBonusesOnLand(eBonus, 1);
					}
				}
				if (eImprovement != NO_IMPROVEMENT)
				{
					pArea->changeNumImprovements(eImprovement, 1);
				}
			}
		}
	}
}

void CvMap::write(CvSavegameWriter writer)
{
	LogIntentHelper helper(writer, "CvMap");

	writer.AssignClassType(SAVEGAME_CLASS_MAP);

	if (writer.isDebug())
	{
		writer.Write("CvMap");
	}

	// Write the data.
	// Use WriteSwitch since it will automatically include WriteSwitch in the savegame.
	// Also it will not save anything if the variable and the default values are identical.
	// If nothing is saved, the loading code will use the default values.
	// Less data saved/loaded means smaller savegames.

	writer.Write(Save_GridWidth, m_iGridWidth, defaultGridWidth);
	writer.Write(Save_GridHeight, m_iGridHeight, defaultGridHeight);
	writer.Write(Save_LandPlots, m_iLandPlots, defaultLandPlots);
	writer.Write(Save_OwnedPlots, m_iOwnedPlots, defaultOwnedPlots);
	writer.Write(Save_TopLatitude, m_iTopLatitude, defaultTopLatitude);
	writer.Write(Save_BottomLatitude, m_iBottomLatitude, defaultBottomLatitude);
	writer.Write(Save_NextRiverID, m_iNextRiverID, defaultNextRiverID);

	writer.Write(Save_WrapX, m_bWrapX, defaultWrapX);
	writer.Write(Save_WrapY, m_bWrapY, defaultWrapY);
	writer.Write(Save_UseTwoPlotCities, this->getCityCatchmentRadius(), (char)0);

	if (numPlotsINLINE() > 0)
	{
		writer.Write(Save_Plots);
		int iNumPlots = numPlotsINLINE();
		for (int iI = 0; iI < iNumPlots; iI++)
		{
			m_pMapPlots[iI].write(writer);
		}
	}

	writer.Write(Save_Areas);
	WriteStreamableFFreeListTrashArray(m_areas, writer);

	writer.Write(Save_END);
}
