#include "CvGameCoreDLL.h"

#include "CvSavegame.h"

const int DefaultiID = 0;
const int DefaultiNumTiles = 0;
const int DefaultiNumOwnedTiles = 0;
const int DefaultiNumRiverEdges = 0;
const int DefaultiNumUnits = 0;
const int DefaultiNumCities = 0;
const int DefaultiNumStartingPlots = 0;
const bool DefaultbWater = false;

// set the default values
enum SavegameVariableTypes
{
	Save_END,
	Save_ID,
	Save_NumTiles,
	Save_NumOwnedTiles,
	Save_NumRiverEdges,
	Save_NumUnits,
	Save_NumCities,
	Save_NumStartingPlots,
	Save_bWater,

	Save_UnitsPerPlayer,
	Save_CitiesPerPlayer,
	Save_PopulationPerPlayer,
	Save_Power,
	Save_BestFoundValue,
	Save_NumRevealedTiles,
	Save_AreaAIType,

	Save_YieldRateModifier,
	Save_NumTrainAIUnits,
	Save_iNumAIUnits,

	Save_TargetCities,

	NUM_SAVE_ENUM_VALUES,
};

int getNumSavedEnumValuesArea()
{
	return NUM_SAVE_ENUM_VALUES;
}

// assign everything to default values
void CvArea::reset()
{
	m_iID                  = DefaultiID;
	m_iNumTiles            = DefaultiNumTiles;
	m_iNumOwnedTiles       = DefaultiNumOwnedTiles;
	m_iNumRiverEdges       = DefaultiNumRiverEdges;
	m_iNumUnits            = DefaultiNumUnits;
	m_iNumCities           = DefaultiNumCities;
	m_iNumStartingPlots    = DefaultiNumStartingPlots;
	m_bWater               = DefaultbWater;

	m_em_iUnitsPerPlayer       .reset();
	m_em_iCitiesPerPlayer      .reset();
	m_em_iPopulationPerPlayer  .reset();
	m_em_iPower                .reset();
	m_em_iBestFoundValue       .reset();
	m_em_iNumRevealedTiles     .reset();
	m_em_eAreaAIType           .reset();
	m_em2_iYieldRateModifier   .reset();
	m_em2_iNumTrainAIUnits     .reset();
	m_em2_iNumAIUnits          .reset();
	m_em_iNumBonuses           .reset();
	m_em_iNumImprovements      .reset();

	for (int iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		m_aTargetCities[iI].reset();
	}
}

void CvArea::read(CvSavegameReader reader)
{
	reader.AssignClassType(SAVEGAME_CLASS_AREA);

	// Init data before load
	// This will ensure that all variables not included in the savegame will have default values
	reset();

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
			case Save_ID                  : reader.Read(m_iID                      ); break;
			case Save_NumTiles            : reader.Read(m_iNumTiles                ); break;
			case Save_NumOwnedTiles       : reader.Read(m_iNumOwnedTiles           ); break;
			case Save_NumRiverEdges       : reader.Read(m_iNumRiverEdges           ); break;
			case Save_NumUnits            : reader.Read(m_iNumUnits                ); break;
			case Save_NumCities           : reader.Read(m_iNumCities               ); break;
			case Save_NumStartingPlots    : reader.Read(m_iNumStartingPlots        ); break;
			case Save_bWater              : reader.Read(m_bWater                   ); break;

			case Save_UnitsPerPlayer      : reader.Read(m_em_iUnitsPerPlayer       ); break;
			case Save_CitiesPerPlayer     : reader.Read(m_em_iCitiesPerPlayer      ); break;
			case Save_PopulationPerPlayer : reader.Read(m_em_iPopulationPerPlayer  ); break;
			case Save_Power               : reader.Read(m_em_iPower                ); break;
			case Save_BestFoundValue      : reader.Read(m_em_iBestFoundValue       ); break;
			case Save_NumRevealedTiles    : reader.Read(m_em_iNumRevealedTiles     ); break;
			case Save_AreaAIType          : reader.Read(m_em_eAreaAIType           ); break;

			case Save_YieldRateModifier   : reader.Read(m_em2_iYieldRateModifier   ); break;
			case Save_NumTrainAIUnits     : reader.Read(m_em2_iNumTrainAIUnits     ); break;
			case Save_iNumAIUnits         : reader.Read(m_em2_iNumAIUnits          ); break;


			case Save_TargetCities:
			{
				for (int iI = 0; iI < MAX_PLAYERS; ++iI)
				{
					m_aTargetCities[iI].read(reader);
				}
			}
				break;
			case Save_END:
				bContinue = false;
				break;

		}
	}
	
	// Loading done. Set up the cache (if any).

}

void CvArea::write(CvSavegameWriter writer)
{
	writer.AssignClassType(SAVEGAME_CLASS_AREA);

	// Write the data.
	// Use WriteSwitch since it will automatically include WriteSwitch in the savegame.
	// Also it will not save anything if the variable and the default values are identical.
	// If nothing is saved, the loading code will use the default values.
	// Less data saved/loaded means smaller savegames.

	writer.Write(Save_ID                   , m_iID                  , DefaultiID                  );
	writer.Write(Save_NumTiles             , m_iNumTiles            , DefaultiNumTiles            );
	writer.Write(Save_NumOwnedTiles        , m_iNumOwnedTiles       , DefaultiNumOwnedTiles       );
	writer.Write(Save_NumRiverEdges        , m_iNumRiverEdges       , DefaultiNumRiverEdges       );
	writer.Write(Save_NumUnits             , m_iNumUnits            , DefaultiNumUnits            );
	writer.Write(Save_NumCities            , m_iNumCities           , DefaultiNumCities           );
	writer.Write(Save_NumStartingPlots     , m_iNumStartingPlots    , DefaultiNumStartingPlots    );
	writer.Write(Save_bWater               , m_bWater               , DefaultbWater               );


	writer.Write(Save_UnitsPerPlayer       , m_em_iUnitsPerPlayer        );
	writer.Write(Save_CitiesPerPlayer      , m_em_iCitiesPerPlayer       );
	writer.Write(Save_PopulationPerPlayer  , m_em_iPopulationPerPlayer   );
	//writer.Write(Save_Power                , m_em_iPower                 ); // recalculated on load
	writer.Write(Save_BestFoundValue       , m_em_iBestFoundValue        );
	writer.Write(Save_NumRevealedTiles     , m_em_iNumRevealedTiles      );
	writer.Write(Save_AreaAIType           , m_em_eAreaAIType            );

	writer.Write(Save_YieldRateModifier    , m_em2_iYieldRateModifier    );
	writer.Write(Save_NumTrainAIUnits      , m_em2_iNumTrainAIUnits      );
	writer.Write(Save_iNumAIUnits          , m_em2_iNumAIUnits           );

	// m_em_iNumBonuses not saved
	// m_em_iNumImprovements not saved

	writer.Write(Save_TargetCities);
	for (int iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		m_aTargetCities[iI].write(writer);
	}

	writer.Write(Save_END);
}
