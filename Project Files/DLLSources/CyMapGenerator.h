#pragma once

#ifndef CyMapGenerator_h
#define CyMapGenerator_h

//
//	FILE:	 CyMapGenerator.h
//	AUTHOR:  Mustafa Thamer
//	PURPOSE:
//			Python wrapper class for CvMapGenerator
//
//-----------------------------------------------------------------------------
//	Copyright (c) 2005 Firaxis Games, Inc. All rights reserved.
//-----------------------------------------------------------------------------
//

#include <boost/python/list.hpp>

class CvMapGenerator;
class CyPlot;
class CyMapGenerator
{
public:
	CyMapGenerator();
	CyMapGenerator(CvMapGenerator* pMapGenerator);		// Call from C++
	CvMapGenerator* getMapGenerator() { return m_pMapGenerator;	}	// Call from C++
	bool isNone() { return (m_pMapGenerator==NULL); }


	bool canPlaceBonusAt(int /*BonusTypes*/ eBonus, int iX, int iY, bool bIgnoreLatitude);
	bool canPlaceGoodyAt(int /*ImprovementTypes*/ eImprovement, int iX, int iY);

	void addGameElements();

	void addLakes();
	void addRivers();
	void doRiver(CyPlot* pStartPlot, CardinalDirectionTypes eCardinalDirection);
	void addFeatures();
	void addFeaturesOnLand(); //WTP, ray, Randomize Features Map Option
	void addFeaturesOnWater();
	void addBonuses();
	void addUniqueBonusType(int /*BonusTypes*/ eBonusType);
	void addNonUniqueBonusType(int /*BonusTypes*/ eBonusType);
	void addGoodies();
	void addEurope();

	void eraseRivers();
	void eraseFeatures();
	void eraseFeaturesOnLand(); //WTP, ray, Randomize Features Map Option
	void eraseBonuses();
	void eraseGoodies();
	void eraseEurope();

	void generateRandomMap();

	void generatePlotTypes();
	void generateTerrain();

	void afterGeneration();

	void setPlotTypes(boost::python::list& listPlotTypes);

protected:
	CvMapGenerator* m_pMapGenerator;
};

#endif	// CyMapGenerator_h
