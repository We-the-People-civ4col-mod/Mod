#pragma once

#ifndef CIV4_MAPGENERATOR_H
#define CIV4_MAPGENERATOR_H

//#include "CvEnums.h"

#pragma warning( disable: 4251 )		// needs to have dll-interface to be used by clients of class

class CvFractal;
class CvPlot;
class CvArea;

class CvMapGenerator
{
public:
	DllExport static CvMapGenerator& GetInstance();
	DllExport static void FreeInstance() { SAFE_DELETE(m_pInst); }
	CvMapGenerator();
	virtual ~CvMapGenerator();

	bool canPlaceBonusAt(BonusTypes eBonus, int iX, int iY, bool bIgnoreLatitude);
	bool canPlaceGoodyAt(ImprovementTypes eImprovement, int iX, int iY);

	// does all of the below "add..." functions:
	DllExport void addGameElements();

	void addLakes();
	DllExport void addRivers();
	void doRiver(CvPlot* pStartPlot, CardinalDirectionTypes eLastCardinalDirection=NO_CARDINALDIRECTION, CardinalDirectionTypes eOriginalCardinalDirection=NO_CARDINALDIRECTION, int iThisRiverID=-1);
	bool addRiver(CvPlot *pFreshWaterPlot);
	DllExport void addFeatures();
	void addFeaturesOnLand(); //WTP, ray, Randomize Features Map Option
	void addFeaturesOnWater();
	DllExport void addBonuses();
	void addUniqueBonusType(BonusTypes eBonusType);
	void addNonUniqueBonusType(BonusTypes eBonusType);
	DllExport void addGoodies();
	void addEurope();

	DllExport void eraseRivers();
	DllExport void eraseFeatures();
	void eraseFeaturesOnLand(); //WTP, ray, Randomize Features Map Option
	DllExport void eraseBonuses();
	DllExport void eraseGoodies();
	DllExport void eraseEurope();

	DllExport void generateRandomMap();

	void generatePlotTypes();
	void generateTerrain();

	void afterGeneration();

	void setPlotTypes(const int* paiPlotTypes);

protected:

	// Utility functions for roughenHeights()
	int getRiverValueAtPlot(CvPlot* pPlot);
	int calculateNumBonusesToAdd(BonusTypes eBonusType);

private:
	static CvMapGenerator* m_pInst;

};
#endif
