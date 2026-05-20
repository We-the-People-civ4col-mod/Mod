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

#include "CvGameCoreDLL.h"
#include "CyMapGenerator.h"
#include "CvGlobals.h"
#include "CvDLLPythonIFaceBase.h"
#include "CvMapGenerator.h"
#include "CyPlot.h"
#include "DesyncMonitor.h"

CvMapGenerator* CyMapGenerator::pointer(AssertCallerData data)
{
	FAssertWithCaller(data, CxDesyncMonitor::isAlwaysSync());
	FAssertWithCaller(data, m_pMapGenerator != NULL);
	return (CvMapGenerator*)m_pMapGenerator;
}

CyMapGenerator::CyMapGenerator() : m_pMapGenerator(&CvMapGenerator::GetInstance())
{
}

CyMapGenerator::CyMapGenerator(CvMapGenerator* pMapGenerator) : m_pMapGenerator(pMapGenerator)
{
}

bool CyMapGenerator::canPlaceBonusAt(int /*BonusTypes*/ eBonus, int iX, int iY, bool bIgnoreLatitude)
{
	return m_pMapGenerator ? pointer(CREATE_ASSERT_DATA)->canPlaceBonusAt((BonusTypes)eBonus, iX, iY, bIgnoreLatitude) : false;
}

bool CyMapGenerator::canPlaceGoodyAt(int /*ImprovementTypes*/ eImprovement, int iX, int iY)
{
	return m_pMapGenerator ? pointer(CREATE_ASSERT_DATA)->canPlaceGoodyAt((ImprovementTypes)eImprovement, iX, iY) : false;
}

void CyMapGenerator::addGameElements()
{
	if (m_pMapGenerator)
		pointer(CREATE_ASSERT_DATA)->addGameElements();
}

void CyMapGenerator::addLakes()
{
	if (m_pMapGenerator)
		pointer(CREATE_ASSERT_DATA)->addLakes();
}

void CyMapGenerator::addRivers()
{
	if (m_pMapGenerator)
		pointer(CREATE_ASSERT_DATA)->addRivers();
}

void CyMapGenerator::doRiver(CyPlot* pStartPlot, CardinalDirectionTypes eCardinalDirection)
{
	if (m_pMapGenerator)
		pointer(CREATE_ASSERT_DATA)->doRiver(pStartPlot->getPlot(), eCardinalDirection);
}

void CyMapGenerator::addFeatures()
{
	if (m_pMapGenerator)
		pointer(CREATE_ASSERT_DATA)->addFeatures();
}

//WTP, ray, Randomize Features Map Option - START
void CyMapGenerator::addFeaturesOnLand()
{
	if (m_pMapGenerator)
		pointer(CREATE_ASSERT_DATA)->addFeaturesOnLand();
}
//WTP, ray, Randomize Features Map Option - END

void CyMapGenerator::addFeaturesOnWater()
{
	if (m_pMapGenerator)
		pointer(CREATE_ASSERT_DATA)->addFeaturesOnWater();
}

void CyMapGenerator::addBonuses()
{
	if (m_pMapGenerator)
		pointer(CREATE_ASSERT_DATA)->addBonuses();
}

void CyMapGenerator::addUniqueBonusType(int /*BonusTypes*/ eBonusType)
{
	if (m_pMapGenerator)
		pointer(CREATE_ASSERT_DATA)->addUniqueBonusType((BonusTypes)eBonusType);
}

void CyMapGenerator::addNonUniqueBonusType(int /*BonusTypes*/ eBonusType)
{
	if (m_pMapGenerator)
		pointer(CREATE_ASSERT_DATA)->addNonUniqueBonusType((BonusTypes)eBonusType);
}

void CyMapGenerator::addGoodies()
{
	if (m_pMapGenerator)
		pointer(CREATE_ASSERT_DATA)->addGoodies();
}

void CyMapGenerator::addEurope()
{
	if (m_pMapGenerator)
		pointer(CREATE_ASSERT_DATA)->addEurope();
}

void CyMapGenerator::eraseRivers()
{
	if (m_pMapGenerator)
		pointer(CREATE_ASSERT_DATA)->eraseRivers();
}

void CyMapGenerator::eraseFeatures()
{
	if (m_pMapGenerator)
		pointer(CREATE_ASSERT_DATA)->eraseFeatures();
}

//WTP, ray, Randomize Features Map Option - START
void CyMapGenerator::eraseFeaturesOnLand()
{
	if (m_pMapGenerator)
		pointer(CREATE_ASSERT_DATA)->eraseFeaturesOnLand();
}
//WTP, ray, Randomize Features Map Option - END

void CyMapGenerator::eraseBonuses()
{
	if (m_pMapGenerator)
		pointer(CREATE_ASSERT_DATA)->eraseBonuses();
}

void CyMapGenerator::eraseGoodies()
{
	if (m_pMapGenerator)
		pointer(CREATE_ASSERT_DATA)->eraseGoodies();
}

void CyMapGenerator::eraseEurope()
{
	if (m_pMapGenerator)
		pointer(CREATE_ASSERT_DATA)->eraseEurope();
}

void CyMapGenerator::generateRandomMap()
{
	if (m_pMapGenerator)
		pointer(CREATE_ASSERT_DATA)->generateRandomMap();
}

void CyMapGenerator::generatePlotTypes()
{
	if (m_pMapGenerator)
		pointer(CREATE_ASSERT_DATA)->generatePlotTypes();
}

void CyMapGenerator::generateTerrain()
{
	if (m_pMapGenerator)
		pointer(CREATE_ASSERT_DATA)->generateTerrain();
}

void CyMapGenerator::afterGeneration()
{
	if (m_pMapGenerator)
		pointer(CREATE_ASSERT_DATA)->afterGeneration();
}

void CyMapGenerator::setPlotTypes(boost::python::list& listPlotTypes)
{
	if (!m_pMapGenerator)
	{
		return;
	}

	int* paiPlotTypes = NULL;
	gDLL->getPythonIFace()->putSeqInArray(listPlotTypes.ptr() /*src*/, &paiPlotTypes /*dst*/);
	pointer(CREATE_ASSERT_DATA)->setPlotTypes(paiPlotTypes);
	delete [] paiPlotTypes;
}

