// area.cpp

#include "CvGameCoreDLL.h"
#include "CvArea.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvGlobals.h"
#include "CvGameAI.h"
#include "CvPlayerAI.h"
#include "CvTeamAI.h"
#include "CvGameCoreUtils.h"
#include "CvInfos.h"

#include "CvDLLInterfaceIFaceBase.h"

// Public Functions...

CvArea::CvArea()
{
	m_aTargetCities = new IDInfo[MAX_PLAYERS];

	reset();
}


CvArea::~CvArea()
{
	SAFE_DELETE_ARRAY(m_aTargetCities);
}


void CvArea::init(int iID, bool bWater)
{
	reset();
	m_iID = iID;
	m_bWater = bWater;
}

void CvArea::setID(int iID)
{
	m_iID = iID;
}


int CvArea::calculateTotalBestNatureYield() const
{
	int iCount = 0;

	for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);

		if (pLoopPlot->getArea() == getID())
		{
			iCount += pLoopPlot->calculateTotalBestNatureYield(NO_TEAM);
		}
	}

	return iCount;
}


int CvArea::countCoastalLand() const
{
	if (isWater())
	{
		return 0;
	}

	int iCount = 0;

	for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);

		if (pLoopPlot->getArea() == getID())
		{
			if (pLoopPlot->isCoastalLand())
			{
				iCount++;
			}
		}
	}

	return iCount;
}


int CvArea::countNumUniqueBonusTypes() const
{
	int iCount = 0;

	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		if (getNumBonuses((BonusTypes)iI) > 0)
		{
			if (GC.getBonusInfo((BonusTypes)iI).isOneArea())
			{
				iCount++;
			}
		}
	}

	return iCount;
}


void CvArea::changeNumTiles(int iChange)
{
	bool bOldLake;

	if (iChange != 0)
	{
		bOldLake = isLake();

		m_iNumTiles += iChange;
		FAssert(getNumTiles() >= 0);

		if (bOldLake != isLake())
		{
			GC.getMap().updateYield();
		}
	}
}



void CvArea::changeNumOwnedTiles(int iChange)
{
	m_iNumOwnedTiles = (m_iNumOwnedTiles + iChange);
	FAssert(getNumOwnedTiles() >= 0);
	FAssert(getNumUnownedTiles() >= 0);
}


void CvArea::changeNumRiverEdges(int iChange)
{
	m_iNumRiverEdges += iChange;
	FAssert(getNumRiverEdges() >= 0);
}



void CvArea::changeNumStartingPlots(int iChange)
{
	m_iNumStartingPlots += iChange;
	FAssert(getNumStartingPlots() >= 0);
}


bool CvArea::hasEurope() const
{
	for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);

		if (pLoopPlot->getArea() == getID())
		{
			if (pLoopPlot->isEurope())
			{
				return true;
			}
		}
	}

	return false;
}

int CvArea::getUnitsPerPlayer(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	return m_em_iUnitsPerPlayer.get(eIndex);
}


void CvArea::changeUnitsPerPlayer(PlayerTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	m_iNumUnits += iChange;
	FAssert(getNumUnits() >= 0);
	m_em_iUnitsPerPlayer.add(eIndex, iChange);
	FAssert(getUnitsPerPlayer(eIndex) >= 0);
}


int CvArea::getCitiesPerPlayer(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	return m_em_iCitiesPerPlayer.get(eIndex);
}


void CvArea::changeCitiesPerPlayer(PlayerTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	m_iNumCities = (m_iNumCities + iChange);
	FAssert(getNumCities() >= 0);
	m_em_iCitiesPerPlayer.add(eIndex, iChange);
	FAssert(getCitiesPerPlayer(eIndex) >= 0);
}


int CvArea::getPopulationPerPlayer(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	return m_em_iPopulationPerPlayer.get(eIndex);
}


void CvArea::changePopulationPerPlayer(PlayerTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	m_em_iPopulationPerPlayer.add(eIndex, iChange);
	FAssert(getPopulationPerPlayer(eIndex) >= 0);
}


int CvArea::getPower(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	return m_em_iPower.get(eIndex);
}


void CvArea::changePower(PlayerTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	m_em_iPower.add(eIndex, iChange);
	FAssert(getPower(eIndex) >= 0);
}


int CvArea::getBestFoundValue(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	return m_em_iBestFoundValue.get(eIndex);
}


void CvArea::setBestFoundValue(PlayerTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	m_em_iBestFoundValue.set(eIndex, iNewValue);
	FAssert(getBestFoundValue(eIndex) >= 0);
}


int CvArea::getNumRevealedTiles(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	return m_em_iNumRevealedTiles.get(eIndex);
}



void CvArea::changeNumRevealedTiles(TeamTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	m_em_iNumRevealedTiles.add(eIndex, iChange);
	FAssert(getNumRevealedTiles(eIndex) >= 0);
}


AreaAITypes CvArea::getAreaAIType(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be < MAX_TEAMS");
	return m_em_eAreaAIType.get(eIndex);
}


void CvArea::setAreaAIType(TeamTypes eIndex, AreaAITypes eNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be < MAX_TEAMS");
	m_em_eAreaAIType.set(eIndex, eNewValue);
}


CvCity* CvArea::getTargetCity(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	return getCity(m_aTargetCities[eIndex]);
}


void CvArea::setTargetCity(PlayerTypes eIndex, CvCity* pNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");

	if (pNewValue != NULL)
	{
		m_aTargetCities[eIndex] = pNewValue->getIDInfo();
	}
	else
	{
		m_aTargetCities[eIndex].reset();
	}
}


int CvArea::getYieldRateModifier(PlayerTypes eIndex1, YieldTypes eIndex2) const
{
	FAssert(validEnumRange(eIndex1));
	FAssert(validEnumRange(eIndex2));
	return m_em2_iYieldRateModifier[eIndex1].get(eIndex2);
}


void CvArea::changeYieldRateModifier(PlayerTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	FAssert(validEnumRange(eIndex1));
	FAssert(validEnumRange(eIndex2));

	if (iChange != 0)
	{
		m_em2_iYieldRateModifier[eIndex1].add(eIndex2, iChange);

		GET_PLAYER(eIndex1).invalidateYieldRankCache(eIndex2);

		GET_PLAYER(eIndex1).AI_makeAssignWorkDirty();

		if (GET_PLAYER(eIndex1).getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


int CvArea::getNumTrainAIUnits(PlayerTypes eIndex1, UnitAITypes eIndex2) const
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be >= 0");
	FAssertMsg(eIndex1 < MAX_PLAYERS, "eIndex1 is expected to be < MAX_PLAYERS");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be >= 0");
	FAssertMsg(eIndex2 < NUM_UNITAI_TYPES, "eIndex2 is expected to be < NUM_UNITAI_TYPES");
	return m_em2_iNumTrainAIUnits[eIndex1].get(eIndex2);
}


void CvArea::changeNumTrainAIUnits(PlayerTypes eIndex1, UnitAITypes eIndex2, int iChange)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be >= 0");
	FAssertMsg(eIndex1 < MAX_PLAYERS, "eIndex1 is expected to be < MAX_PLAYERS");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be >= 0");
	FAssertMsg(eIndex2 < NUM_UNITAI_TYPES, "eIndex2 is expected to be < NUM_UNITAI_TYPES");
	m_em2_iNumTrainAIUnits[eIndex1].add(eIndex2, iChange);
	FAssert(getNumTrainAIUnits(eIndex1, eIndex2) >= 0);
}


int CvArea::getNumAIUnits(PlayerTypes eIndex1, UnitAITypes eIndex2) const
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be >= 0");
	FAssertMsg(eIndex1 < MAX_PLAYERS, "eIndex1 is expected to be < MAX_PLAYERS");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be >= 0");
	FAssertMsg(eIndex2 < NUM_UNITAI_TYPES, "eIndex2 is expected to be < NUM_UNITAI_TYPES");
	return m_em2_iNumAIUnits[eIndex1].get(eIndex2);
}


void CvArea::changeNumAIUnits(PlayerTypes eIndex1, UnitAITypes eIndex2, int iChange)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be >= 0");
	FAssertMsg(eIndex1 < MAX_PLAYERS, "eIndex1 is expected to be < MAX_PLAYERS");
	if (eIndex2 != NO_UNITAI)
	{
		m_em2_iNumAIUnits[eIndex1].add(eIndex2, iChange);
		FAssert(getNumAIUnits(eIndex1, eIndex2) >= 0);
	}
}


int CvArea::getNumBonuses(BonusTypes eBonus) const
{
	FAssertMsg(eBonus >= 0, "eBonus expected to be >= 0");
	FAssertMsg(eBonus < GC.getNumBonusInfos(), "eBonus expected to be < GC.getNumBonusInfos");
	return m_em_iNumBonuses.get(eBonus);
}


int CvArea::getNumTotalBonuses() const
{
	int iTotal = 0;

	for (BonusTypes eBonus = FIRST_BONUS; eBonus < NUM_BONUS_TYPES; ++eBonus)
	{
		iTotal += getNumBonuses(eBonus);
	}

	return iTotal;
}


void CvArea::changeNumBonuses(BonusTypes eBonus, int iChange)
{
	FAssertMsg(eBonus >= 0, "eBonus expected to be >= 0");
	FAssertMsg(eBonus < GC.getNumBonusInfos(), "eBonus expected to be < GC.getNumBonusInfos");
	m_em_iNumBonuses.add(eBonus, iChange);
	FAssert(getNumBonuses(eBonus) >= 0);
}


int CvArea::getNumImprovements(ImprovementTypes eImprovement) const
{
	FAssertMsg(eImprovement >= 0, "eImprovement expected to be >= 0");
	FAssertMsg(eImprovement < GC.getNumImprovementInfos(), "eImprovement expected to be < GC.getNumImprovementInfos");
	return m_em_iNumImprovements.get(eImprovement);
}


void CvArea::changeNumImprovements(ImprovementTypes eImprovement, int iChange)
{
	FAssertMsg(eImprovement >= 0, "eImprovement expected to be >= 0");
	FAssertMsg(eImprovement < GC.getNumImprovementInfos(), "eImprovement expected to be < GC.getNumImprovementInfos");
	m_em_iNumImprovements.add(eImprovement, iChange);
	FAssert(getNumImprovements(eImprovement) >= 0);
}

// TAC - AI Explore from Ship endless loop fix - koma13 - START
bool CvArea::isEuropePlayer() const
{
	if (isWater())
	{
		return false;
	}

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayerAI& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kLoopPlayer.isAlive() && !kLoopPlayer.isNative())
		{
			if (this->getCitiesPerPlayer((PlayerTypes)iI) > 0)
			{
				return true;
			}
		}
	}

	return false;
}
// TAC - AI Explore from Ship endless loop fix - koma13 - END

// Protected Functions...
