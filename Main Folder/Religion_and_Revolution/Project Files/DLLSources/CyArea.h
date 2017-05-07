#pragma once

#ifndef CyArea_h
#define CyArea_h

//
// Python wrapper class for CvArea
//

class CyCity;
class CvArea;
class CyArea
{
public:

	CyArea();
	CyArea(CvArea* pArea);					// Call from C++
	CvArea* getArea() { return m_pArea;	}	// Call from C++
	bool isNone() { return (m_pArea==NULL); }

	int calculateTotalBestNatureYield();
	int countCoastalLand();
	int countNumUniqueBonusTypes();
	int getID();
	int getNumTiles();
	bool isLake();
	int getNumOwnedTiles();
	int getNumUnownedTiles();
	int getNumRiverEdges();
	int getNumCities();
	int getNumUnits();
	int getNumStartingPlots();
	bool isWater();

	int getUnitsPerPlayer(int /*PlayerTypes*/ eIndex);
	int getCitiesPerPlayer(int /*PlayerTypes*/ eIndex);
	int getPopulationPerPlayer(int /*PlayerTypes*/ eIndex);
	int getPower(int /*PlayerTypes*/ eIndex);
	int getBestFoundValue(int /*PlayerTypes*/ eIndex);

	int getNumRevealedTiles(int /*TeamTypes*/ eIndex);
	int getNumUnrevealedTiles(int /*TeamTypes*/ eIndex);

	int /*AreaAITypes*/ getAreaAIType(int /*TeamTypes*/ eIndex);
	CyCity* getTargetCity(int /*PlayerTypes*/ eIndex);
	int getYieldRateModifier(int /*PlayerTypes*/ eIndex1, int /*YieldTypes*/ eIndex2);
	int getNumTrainAIUnits(int /*PlayerTypes*/ eIndex1, int /*UnitAITypes*/ eIndex2);
	int getNumAIUnits(int /*PlayerTypes*/ eIndex1, int /*UnitAITypes*/ eIndex2);

	int getNumBonuses(int /*BonusTypes*/ eBonus);
	int getNumTotalBonuses();
	int getNumImprovements(int /*ImprovementTypes*/ eImprovement);

protected:

	CvArea* m_pArea;
};

#endif	// #ifndef CyArea
