// gameAI.cpp

#include "CvGameCoreDLL.h"
#include "CvGameAI.h"
#include "CvPlayerAI.h"
#include "CvTeamAI.h"
#include "CvGlobals.h"
#include "CvInfos.h"

#include "CvSavegame.h"


// Public Functions...

CvGameAI::CvGameAI()
{
	AI_reset();
}


CvGameAI::~CvGameAI()
{
	AI_uninit();
}


void CvGameAI::AI_init()
{
	AI_reset();

	//--------------------------------
	// Init other game data
}


void CvGameAI::AI_uninit()
{
}


void CvGameAI::AI_reset()
{
	AI_uninit();
	AI_resetSavedData();
}


void CvGameAI::AI_makeAssignWorkDirty()
{
	int iI;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			GET_PLAYER((PlayerTypes)iI).AI_makeAssignWorkDirty();
		}
	}
}


void CvGameAI::AI_updateAssignWork()
{
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (GET_TEAM(kLoopPlayer.getTeam()).isHuman() && kLoopPlayer.isAlive())
		{
			kLoopPlayer.AI_updateAssignWork();
		}
	}
}

int CvGameAI::AI_combatValue(UnitTypes eUnit)
{
	int iValue;

	iValue = 100;

	iValue *= GC.getUnitInfo(eUnit).getCombat();

	iValue /= getBestLandUnitCombat();

	return iValue;
}

int CvGameAI::AI_adjustedTurn(int iTurn)
{
	int iModifier = 0;
	iModifier += GC.getGameSpeedInfo(getGameSpeedType()).getGrowthPercent() - 100;
	iModifier += GC.getGameSpeedInfo(getGameSpeedType()).getTrainPercent() - 100;
	iModifier += GC.getGameSpeedInfo(getGameSpeedType()).getConstructPercent() - 100;
	iModifier /= 3;
	
	return (iTurn * (100 + iModifier)) / 100;
	
}


int CvGameAI::AI_turnsPercent(int iTurns, int iPercent)
{
	FAssert(iPercent > 0);
	if (iTurns != MAX_INT)
	{
		iTurns *= (iPercent);
		iTurns /= 100;
	}

	return std::max(1, iTurns);
}

int CvGameAI::AI_gameCompletePercent()
{
	return std::min(100, 100 * getGameTurn() / std::max(1,  getEstimateEndTurn()));
}

// declaring a bool to tell if a savegame is being loaded while being read only when read globally
bool bSavegameLoadingInProgress = false;
extern const bool& SAVEGAME_IS_LOADING = bSavegameLoadingInProgress;

void CvGameAI::postLoadFixes()
{
	bSavegameLoadingInProgress = false;
}

void CvGameAI::read(FDataStreamBase* pStream)
{
	bSavegameLoadingInProgress = true;
	CvSavegameReaderBase readerbase(pStream);
	CvSavegameReader reader(readerbase);

	read(reader);
}


void CvGameAI::write(FDataStreamBase* pStream)
{
	CvSavegameWriterBase writerbase(pStream);
	CvSavegameWriter writer(writerbase); 
	write(writer); 
	writerbase.WriteFile(); 
}

// Protected Functions...

// Private Functions...
