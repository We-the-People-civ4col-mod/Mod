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
const bool defaultUseTwoPlotCities = false;

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

	Save_NumBonuses,
	Save_NumBonusesOnLand,

	Save_Areas,

	Save_Plots,
};

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
	m_bUseTwoPlotCities = defaultUseTwoPlotCities;

	m_ja_NumBonuses.reset();
	m_ja_NumBonusesOnLand.reset();
}

void CvMap::read(CvSavegameReader reader)
{
	// Init data before load
	// This will ensure that all variables not included in the savegame will have default values
	//reset();

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
		case Save_GridWidth:           reader.Read(m_iGridWidth);          break;
		case Save_GridHeight:          reader.Read(m_iGridHeight);         break;
		case Save_LandPlots:           reader.Read(m_iLandPlots);          break;
		case Save_OwnedPlots:          reader.Read(m_iOwnedPlots);         break;
		case Save_TopLatitude:         reader.Read(m_iTopLatitude);        break;
		case Save_BottomLatitude:      reader.Read(m_iBottomLatitude);     break;
		case Save_NextRiverID:         reader.Read(m_iNextRiverID);        break;

		case Save_WrapX:               reader.Read(m_bWrapX);              break;
		case Save_WrapY:               reader.Read(m_bWrapY);              break;
		
		case Save_NumBonuses:          reader.Read(m_ja_NumBonuses);       break;
		case Save_NumBonusesOnLand:    reader.Read(m_ja_NumBonusesOnLand); break;

		case Save_UseTwoPlotCities:
			reader.Read(m_bUseTwoPlotCities);
			setCityCatchmentRadius(m_bUseTwoPlotCities ? 1 : 0);
			break;

		case Save_Plots:
		{
			FAssertMsg(m_pMapPlots == NULL, "Memory leak");
			int iNumPlots = numPlotsINLINE();
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

}

void CvMap::write(CvSavegameWriter writer)
{
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
	writer.Write(Save_UseTwoPlotCities, m_bUseTwoPlotCities, defaultUseTwoPlotCities);

	writer.Write(Save_NumBonuses, m_ja_NumBonuses);
	writer.Write(Save_NumBonusesOnLand, m_ja_NumBonusesOnLand);

	if (numPlotsINLINE() > 0)
	{
		writer.Write(Save_Plots);
		int iNumPlots = numPlotsINLINE();
		for (int iI = 0; iI < iNumPlots; iI++)
		{
			m_pMapPlots[iI].write(writer);
		}
	}

#if 0
	// enable once CvPlot and CvArea are saved using CvSavegame
	writer.Write(Save_Areas);
	WriteStreamableFFreeListTrashArray(m_areas, writer);
#endif

	writer.Write(Save_END);
}
