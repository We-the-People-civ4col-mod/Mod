#include "CvGameCoreDLL.h"
#include "CvMapGenerator.h"
#include "CvMap.h"
#include "CvFractal.h"
#include "CvPlot.h"
#include "CvGlobals.h"
#include "CvRandom.h"
#include "CvGameCoreUtils.h"
#include "CvGameAI.h"
#include "CvInfos.h"
#include "CvDLLPythonIFaceBase.h"
#include "FProfiler.h"

//
// static
//
CvMapGenerator* CvMapGenerator::m_pInst = NULL;

//
// singleton accessor
//
CvMapGenerator& CvMapGenerator::GetInstance()
{
	if (m_pInst==NULL)
	{
		m_pInst = new CvMapGenerator;
	}
	return *m_pInst;
}


CvMapGenerator::CvMapGenerator()
{
}


CvMapGenerator::~CvMapGenerator()
{
}


bool CvMapGenerator::canPlaceBonusAt(BonusTypes eBonus, int iX, int iY, bool bIgnoreLatitude)
{
	PROFILE_FUNC();

	CvArea* pArea;
	CvPlot* pPlot;
	CvPlot* pLoopPlot;
	int iRange;
	int iDX, iDY;
	int iI;

	pPlot = GC.getMap().plotINLINE(iX, iY);
	pArea = pPlot->area();

	if (!(pPlot->canHaveBonus(eBonus, bIgnoreLatitude)))
	{
		return false;
	}

	long result = 0;
	if (gDLL->getPythonIFace()->pythonCanPlaceBonusAt(pPlot, &result) && !gDLL->getPythonIFace()->pythonUsingDefaultImpl()) // Python override
	{
		if (result >= 0)
		{
			return result;
		}
		else
		{
			FAssertMsg(false, "canPlaceBonusAt() must return >= 0");
		}
	}

	for (iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(iX, iY, ((DirectionTypes)iI));

		if (pLoopPlot != NULL)
		{
			if ((pLoopPlot->getBonusType() != NO_BONUS) && (pLoopPlot->getBonusType() != eBonus))
			{
				return false;
			}
		}
	}

	CvBonusInfo& pInfo = GC.getBonusInfo(eBonus);

	if (pPlot->isWater())
	{
		if (((GC.getMap().getNumBonusesOnLand(eBonus) * 100) / (GC.getMap().getNumBonuses(eBonus) + 1)) < pInfo.getMinLandPercent())
		{
			return false;
		}
	}

	// Make sure there are none of the same bonus nearby:

	iRange = pInfo.getUniqueRange();

	for (iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for (iDY = -(iRange); iDY <= iRange; iDY++)
		{
			pLoopPlot	= plotXY(iX, iY, iDX, iDY);

			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->area() == pArea)
				{
					if (plotDistance(iX, iY, pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE()) <= iRange)
					{
						if (pLoopPlot->getBonusType() == eBonus)
						{
							return false;
						}
					}
				}
			}
		}
	}

	return true;
}


bool CvMapGenerator::canPlaceGoodyAt(ImprovementTypes eImprovement, int iX, int iY)
{
	PROFILE_FUNC();

	CvPlot* pPlot;

	FAssertMsg(eImprovement != NO_IMPROVEMENT, "Improvement is not assigned a valid value");
	FAssertMsg(GC.getImprovementInfo(eImprovement).isGoody(), "ImprovementType eImprovement is expected to be a goody");

	if (GC.getGameINLINE().isOption(GAMEOPTION_NO_GOODY_HUTS))
	{
		return false;
	}

	pPlot = GC.getMap().plotINLINE(iX, iY);

	if (!(pPlot->canHaveImprovement(eImprovement, NO_TEAM)))
	{
		return false;
	}

	const FeatureTypes eFeature = pPlot->getFeatureType();

	// Erik: Do not place goodies\shipwrecks on impassable features (e.g. ice)
	if (eFeature != NO_FEATURE && GC.getFeatureInfo(eFeature).isImpassable())
	{
		return false;
	}

	long result = 0;
	if (gDLL->getPythonIFace()->pythonCanPlaceGoodyAt(pPlot, &result) && !gDLL->getPythonIFace()->pythonUsingDefaultImpl()) // Python override
	{
		if (result >= 0)
		{
			return result;
		}
		else
		{
			FAssertMsg(false, "pythonGetRiverAltitude() must return >= 0");
		}
	}

	if (pPlot->getImprovementType() != NO_IMPROVEMENT)
	{
		return false;
	}

	if (pPlot->getBonusType() != NO_BONUS)
	{
		return false;
	}

	if (pPlot->isImpassable())
	{
		return false;
	}

	int iUniqueRange = GC.getImprovementInfo(eImprovement).getGoodyUniqueRange();
	for (int iDX = -iUniqueRange; iDX <= iUniqueRange; iDX++)
	{
		for (int iDY = -iUniqueRange; iDY <= iUniqueRange; iDY++)
		{
			CvPlot *pLoopPlot	= plotXY(iX, iY, iDX, iDY);
			if (pLoopPlot != NULL && pLoopPlot->getImprovementType() == eImprovement)
			{
				return false;
			}
		}
	}

	return true;
}


void CvMapGenerator::addGameElements()
{
	addRivers();
	gDLL->logMemState("CvMapGen after add rivers");

	addLakes();
	gDLL->logMemState("CvMapGen after add lakes");

	addFeatures();
	gDLL->logMemState("CvMapGen after add features");

	addEurope();
	gDLL->logMemState("CvMapGen after add Europe");

	// Call for Python to make map modifications after it's been generated
	afterGeneration();

	// Erik: The FaireWeather* map script(s) create(s) ice in afterGeneration
	// so we have to place bonuses and goodies after it completes

	addBonuses();
	gDLL->logMemState("CvMapGen after add bonuses");

	addGoodies();
	gDLL->logMemState("CvMapGen after add goodies");
}


void CvMapGenerator::addLakes()
{
	PROFILE_FUNC();

	if (gDLL->getPythonIFace()->pythonAddLakes() && !gDLL->getPythonIFace()->pythonUsingDefaultImpl())
	{
		return; // Python override
	}

	gDLL->NiTextOut("Adding Lakes...");
	CvPlot* pLoopPlot;
	int iI;

	for (iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		gDLL->callUpdater();
		pLoopPlot = GC.getMap().plotByIndexINLINE(iI);
		FAssertMsg(pLoopPlot != NULL, "LoopPlot is not assigned a valid value");

		if (!(pLoopPlot->isWater()))
		{
			if (!(pLoopPlot->isCoastalLand()))
			{
				if (!(pLoopPlot->isRiver()))
				{
					if (GC.getGameINLINE().getMapRandNum(GC.getDefineINT("LAKE_PLOT_RAND"), "addLakes") == 0)
					{
						pLoopPlot->setPlotType(PLOT_OCEAN);
					}
				}
			}
		}
	}
}

void CvMapGenerator::addRivers()
{
	PROFILE_FUNC();

	if (gDLL->getPythonIFace()->pythonAddRivers() && !gDLL->getPythonIFace()->pythonUsingDefaultImpl())
	{
		return; // Python override
	}

	gDLL->NiTextOut("Adding Rivers...");
	CvPlot* pLoopPlot;
	CvPlot* pStartPlot;
	int iPass;
	int iRiverSourceRange;
	int iSeaWaterRange;
	int iI;

	for (iPass = 0; iPass < 4; iPass++)
	{
		if (iPass <= 1)
		{
			iRiverSourceRange = GC.getDefineINT("RIVER_SOURCE_MIN_RIVER_RANGE");
		}
		else
		{
			iRiverSourceRange = (GC.getDefineINT("RIVER_SOURCE_MIN_RIVER_RANGE") / 2);
		}

		if (iPass <= 1)
		{
			iSeaWaterRange = GC.getDefineINT("RIVER_SOURCE_MIN_SEAWATER_RANGE");
		}
		else
		{
			iSeaWaterRange = (GC.getDefineINT("RIVER_SOURCE_MIN_SEAWATER_RANGE") / 2);
		}

		for (iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
		{
			gDLL->callUpdater();
			pLoopPlot = GC.getMap().plotByIndexINLINE(iI);
			FAssertMsg(pLoopPlot != NULL, "LoopPlot is not assigned a valid value");

			if (!(pLoopPlot->isWater()))
			{
				if (((iPass == 0) && (pLoopPlot->isHills() || pLoopPlot->isPeak())) ||
					  ((iPass == 1) && !(pLoopPlot->isCoastalLand()) && (GC.getGameINLINE().getMapRandNum(8, "addRivers") == 0)) ||
					  ((iPass == 2) && (pLoopPlot->isHills() || pLoopPlot->isPeak()) && (pLoopPlot->area()->getNumRiverEdges() < ((pLoopPlot->area()->getNumTiles() / GC.getDefineINT("PLOTS_PER_RIVER_EDGE")) + 1))) ||
					  ((iPass == 3) && (pLoopPlot->area()->getNumRiverEdges() < ((pLoopPlot->area()->getNumTiles() / GC.getDefineINT("PLOTS_PER_RIVER_EDGE")) + 1))))
				{
					if (!(GC.getMap().findWater(pLoopPlot, iRiverSourceRange, true)))
					{
						if (!(GC.getMap().findWater(pLoopPlot, iSeaWaterRange, false)))
						{
							pStartPlot = pLoopPlot->getInlandCorner();

							if (pStartPlot != NULL)
							{
								doRiver(pStartPlot);
							}
						}
					}
				}
			}
		}
	}
}

// pStartPlot = the plot at whose SE corner the river is starting
//
void CvMapGenerator::doRiver(CvPlot *pStartPlot, CardinalDirectionTypes eLastCardinalDirection, CardinalDirectionTypes eOriginalCardinalDirection, int iThisRiverID)
{
	if (iThisRiverID == -1)
	{
		iThisRiverID = GC.getMap().getNextRiverID();
		GC.getMap().incrementNextRiverID();
	}

	int iOtherRiverID = pStartPlot->getRiverID();
	if (iOtherRiverID != -1 && iOtherRiverID != iThisRiverID)
	{
		return; // Another river already exists here; can't branch off of an existing river!
	}

	CvPlot *pRiverPlot = NULL;
	CvPlot *pAdjacentPlot = NULL;

	CardinalDirectionTypes eBestCardinalDirection = NO_CARDINALDIRECTION;

	if (eLastCardinalDirection==CARDINALDIRECTION_NORTH)
	{
		pRiverPlot = pStartPlot;
		if (pRiverPlot == NULL)
		{
			return;
		}
		pAdjacentPlot = plotCardinalDirection(pRiverPlot->getX_INLINE(), pRiverPlot->getY_INLINE(), CARDINALDIRECTION_EAST);
		if ((pAdjacentPlot == NULL) || pRiverPlot->isWOfRiver() || pRiverPlot->isWater() || pAdjacentPlot->isWater())
		{
			return;
		}

		pStartPlot->setRiverID(iThisRiverID);
		pRiverPlot->setWOfRiver(true, eLastCardinalDirection);
		pRiverPlot = plotCardinalDirection(pRiverPlot->getX_INLINE(), pRiverPlot->getY_INLINE(), CARDINALDIRECTION_NORTH);
	}
	else if (eLastCardinalDirection==CARDINALDIRECTION_EAST)
	{
		pRiverPlot = plotCardinalDirection(pStartPlot->getX_INLINE(), pStartPlot->getY_INLINE(), CARDINALDIRECTION_EAST);
		if (pRiverPlot == NULL)
		{
			return;
		}
		pAdjacentPlot = plotCardinalDirection(pRiverPlot->getX_INLINE(), pRiverPlot->getY_INLINE(), CARDINALDIRECTION_SOUTH);
		if ((pAdjacentPlot == NULL) || pRiverPlot->isNOfRiver() || pRiverPlot->isWater() || pAdjacentPlot->isWater())
		{
			return;
		}

		pStartPlot->setRiverID(iThisRiverID);
		pRiverPlot->setNOfRiver(true, eLastCardinalDirection);
	}
	else if (eLastCardinalDirection==CARDINALDIRECTION_SOUTH)
	{
		pRiverPlot = plotCardinalDirection(pStartPlot->getX_INLINE(), pStartPlot->getY_INLINE(), CARDINALDIRECTION_SOUTH);
		if (pRiverPlot == NULL)
		{
			return;
		}
		pAdjacentPlot = plotCardinalDirection(pRiverPlot->getX_INLINE(), pRiverPlot->getY_INLINE(), CARDINALDIRECTION_EAST);
		if ((pAdjacentPlot == NULL) || pRiverPlot->isWOfRiver() || pRiverPlot->isWater() || pAdjacentPlot->isWater())
		{
			return;
		}

		pStartPlot->setRiverID(iThisRiverID);
		pRiverPlot->setWOfRiver(true, eLastCardinalDirection);
	}

	else if (eLastCardinalDirection==CARDINALDIRECTION_WEST)
	{
		pRiverPlot = pStartPlot;
		if (pRiverPlot == NULL)
		{
			return;
		}
		pAdjacentPlot = plotCardinalDirection(pRiverPlot->getX_INLINE(), pRiverPlot->getY_INLINE(), CARDINALDIRECTION_SOUTH);
		if ((pAdjacentPlot == NULL) || pRiverPlot->isNOfRiver() || pRiverPlot->isWater() || pAdjacentPlot->isWater())
		{
			return;
		}

		pStartPlot->setRiverID(iThisRiverID);
		pRiverPlot->setNOfRiver(true, eLastCardinalDirection);
		pRiverPlot = plotCardinalDirection(pRiverPlot->getX_INLINE(), pRiverPlot->getY_INLINE(), CARDINALDIRECTION_WEST);
	}
	else
	{
		//FAssertMsg(false, "Illegal direction type");
		// River is starting here, set the direction in the next step
		pRiverPlot = pStartPlot;

		long result = 0;
		if (gDLL->getPythonIFace()->pythonGetRiverStartCardinalDirection(pRiverPlot, &result) && !gDLL->getPythonIFace()->pythonUsingDefaultImpl()) // Python override
		{
			if (result >= 0)
			{
				eBestCardinalDirection = ((CardinalDirectionTypes)result);
			}
			else
			{
				FAssertMsg(false, "python pythonGetRiverStartCardinalDirection() must return >= 0");
			}
		}
	}

	if (pRiverPlot == NULL)
	{
		return; // The river has flowed off the edge of the map. All is well.
	}
	else if (pRiverPlot->hasCoastAtSECorner())
	{
		return; // The river has flowed into the ocean. All is well.
	}

	if (eBestCardinalDirection == NO_CARDINALDIRECTION)
	{
		int iBestValue = MAX_INT;

		for (int iI = 0; iI < NUM_CARDINALDIRECTION_TYPES; iI++)
		{
			if (getOppositeCardinalDirection((CardinalDirectionTypes)iI) != eOriginalCardinalDirection)
			{
				if (getOppositeCardinalDirection((CardinalDirectionTypes)iI) != eLastCardinalDirection)
				{
					CvPlot* pAdjacentPlot;
					pAdjacentPlot = plotCardinalDirection(pRiverPlot->getX_INLINE(), pRiverPlot->getY_INLINE(), ((CardinalDirectionTypes)iI));
					if (pAdjacentPlot != NULL)
					{
						int iValue = getRiverValueAtPlot(pAdjacentPlot);
						if (iValue < iBestValue)
						{
							iBestValue = iValue;
							eBestCardinalDirection = (CardinalDirectionTypes)iI;
						}
					}
				}
			}
		}
	}

	if (eBestCardinalDirection != NO_CARDINALDIRECTION)
	{
		if  (eOriginalCardinalDirection	== NO_CARDINALDIRECTION)
		{
			eOriginalCardinalDirection = eBestCardinalDirection;
		}
		doRiver(pRiverPlot, eBestCardinalDirection, eOriginalCardinalDirection, iThisRiverID);
	}
}
//Note from Blake:
//Iustus wrote this function, it ensures that a new river actually
//creates fresh water on the passed plot. Quite useful really
//Altouh I veto'd it's use since I like that you don't always
//get fresh water starts.
// pFreshWaterPlot = the plot we want to give a fresh water river
//
bool CvMapGenerator::addRiver(CvPlot* pFreshWaterPlot)
{
	FAssertMsg(pFreshWaterPlot != NULL, "NULL plot parameter");

	// cannot have a river flow next to water
	if (pFreshWaterPlot->isWater())
	{
		return false;
	}

	// if it already has a fresh water river, then success! we done
	if (pFreshWaterPlot->isRiver())
	{
		return true;
	}

	bool bSuccess = false;

	// randomize the order of directions
	std::vector<int> aiShuffle(NUM_CARDINALDIRECTION_TYPES);
	GC.getGameINLINE().getMapRand().shuffleSequence(aiShuffle, NULL);

	// make two passes, once for each flow direction of the river
	int iNWFlowPass = GC.getGameINLINE().getMapRandNum(2, "addRiver");
	for (int iPass = 0; !bSuccess && iPass <= 1; iPass++)
	{
		// try placing a river edge in each direction, in random order
		for (int iI = 0; !bSuccess && iI < NUM_CARDINALDIRECTION_TYPES; iI++)
		{
			CardinalDirectionTypes eRiverDirection = NO_CARDINALDIRECTION;
			CvPlot *pRiverPlot = NULL;

			switch (aiShuffle[iI])
			{
			case CARDINALDIRECTION_NORTH:
				if (iPass == iNWFlowPass)
				{
					pRiverPlot = plotDirection(pFreshWaterPlot->getX_INLINE(), pFreshWaterPlot->getY_INLINE(), DIRECTION_NORTH);
					eRiverDirection = CARDINALDIRECTION_WEST;
				}
				else
				{
					pRiverPlot = plotDirection(pFreshWaterPlot->getX_INLINE(), pFreshWaterPlot->getY_INLINE(), DIRECTION_NORTHWEST);
					eRiverDirection = CARDINALDIRECTION_EAST;
				}
				break;

			case CARDINALDIRECTION_EAST:
				if (iPass == iNWFlowPass)
				{
					pRiverPlot = pFreshWaterPlot;
					eRiverDirection = CARDINALDIRECTION_NORTH;
				}
				else
				{
					pRiverPlot = plotDirection(pFreshWaterPlot->getX_INLINE(), pFreshWaterPlot->getY_INLINE(), DIRECTION_NORTH);
					eRiverDirection = CARDINALDIRECTION_SOUTH;
				}
				break;

			case CARDINALDIRECTION_SOUTH:
				if (iPass == iNWFlowPass)
				{
					pRiverPlot = pFreshWaterPlot;
					eRiverDirection = CARDINALDIRECTION_WEST;
				}
				else
				{
					pRiverPlot = plotDirection(pFreshWaterPlot->getX_INLINE(), pFreshWaterPlot->getY_INLINE(), DIRECTION_WEST);
					eRiverDirection = CARDINALDIRECTION_EAST;
				}
				break;

			case CARDINALDIRECTION_WEST:
				if (iPass == iNWFlowPass)
				{
					pRiverPlot = plotDirection(pFreshWaterPlot->getX_INLINE(), pFreshWaterPlot->getY_INLINE(), DIRECTION_WEST);
					eRiverDirection = CARDINALDIRECTION_NORTH;
				}
				else
				{
					pRiverPlot = plotDirection(pFreshWaterPlot->getX_INLINE(), pFreshWaterPlot->getY_INLINE(), DIRECTION_NORTHWEST);
					eRiverDirection = CARDINALDIRECTION_SOUTH;
				}
				break;

			default:
				FAssertMsg(false, "invalid cardinal direction");
			}

			if (pRiverPlot != NULL && !pRiverPlot->hasCoastAtSECorner())
			{
				// try to make the river
				doRiver(pRiverPlot, eRiverDirection, eRiverDirection, -1);

				// if it succeeded, then we will be a river now!
				if (pFreshWaterPlot->isRiver())
				{
					bSuccess = true;
				}
			}
		}
	}

	return bSuccess;
}


void CvMapGenerator::addFeatures()
{
	PROFILE_FUNC();

	CvPlot* pPlot;
	int iI, iJ;

	if (gDLL->getPythonIFace()->pythonAddFeatures() && !gDLL->getPythonIFace()->pythonUsingDefaultImpl())
	{
		// R&R, create Winds and Storms on first turn, ray, START
		//only run for Ocean Plots, we do not want to override other features
		for (iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
		{
			pPlot = GC.getMap().plotByIndexINLINE(iI);
			FAssert(pPlot != NULL);
			//ray, ensure that Storms do not destroy other Features
			if (pPlot->isWater() && pPlot->getFeatureType() != NO_FEATURE)
			{
				for (iJ = 0; iJ < GC.getNumFeatureInfos(); iJ++)
				{
					if (pPlot->canHaveFeature((FeatureTypes)iJ))
					{
						if (GC.getGameINLINE().getMapRandNum(10000, "addFeaturesAtPlot") < GC.getFeatureInfo((FeatureTypes)iJ).getAppearanceProbability())
						{
							pPlot->setFeatureType((FeatureTypes)iJ);
						}
					}
				}
			}
		}
		// R&R, create Winds and Storms on first turn, ray, END
		return; // Python override
	}

	for (iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		pPlot = GC.getMap().plotByIndexINLINE(iI);
		FAssert(pPlot != NULL);

		for (iJ = 0; iJ < GC.getNumFeatureInfos(); iJ++)
		{
			if (pPlot->canHaveFeature((FeatureTypes)iJ))
			{
				if (GC.getGameINLINE().getMapRandNum(10000, "addFeaturesAtPlot") < GC.getFeatureInfo((FeatureTypes)iJ).getAppearanceProbability())
				{
					pPlot->setFeatureType((FeatureTypes)iJ);
				}
			}
		}
	}
}


//WTP, ray, Randomize Features Map Option - START
void CvMapGenerator::addFeaturesOnLand()
{
	PROFILE_FUNC();

	CvPlot* pPlot;
	int iI, iJ;

	for (iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		pPlot = GC.getMap().plotByIndexINLINE(iI);
		FAssert(pPlot != NULL);
		// only for Land Plots
		if(!pPlot->isWater())
		{
			//WTP, ray, make sure some Plots stay without Terrain Feature
			int iPlotsWithoutFeature = GLOBAL_DEFINE_BASE_TERRAINS_WITHOUT_FEATURE_PERCENTAGE * 100;
			if (GC.getGameINLINE().getMapRandNum(10000, "addFeaturesOnLand") > iPlotsWithoutFeature)
			{
				for (iJ = 0; iJ < GC.getNumFeatureInfos(); iJ++)
				{
					if (pPlot->canHaveFeature((FeatureTypes)iJ))
					{
						if (GC.getGameINLINE().getMapRandNum(10000, "addFeaturesOnLand") < GC.getFeatureInfo((FeatureTypes)iJ).getAppearanceProbability())
						{
							pPlot->setFeatureType((FeatureTypes)iJ);
						}
					}
				}
			}
		}
	}
}
//WTP, ray, Randomize Features Map Option - END

void CvMapGenerator::addFeaturesOnWater()
{
	PROFILE_FUNC();

	CvPlot* pPlot;
	int iI, iJ;

	for (iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		pPlot = GC.getMap().plotByIndexINLINE(iI);
		FAssert(pPlot != NULL);
		// only for Water Plots
		if (pPlot->isWater())
		{
			const FeatureTypes eFeature = pPlot->getFeatureType();

			if (eFeature != NO_FEATURE && GC.getFeatureInfo(eFeature).isImpassable())
				continue;

			for (iJ = 0; iJ < GC.getNumFeatureInfos(); iJ++)
			{
				if (pPlot->canHaveFeature((FeatureTypes)iJ))
				{
					if (GC.getGameINLINE().getMapRandNum(10000, "addFeaturesOnWater") < GC.getFeatureInfo((FeatureTypes)iJ).getAppearanceProbability())
					{
						pPlot->setFeatureType((FeatureTypes)iJ);
					}
				}
			}
		}
	}
}

void CvMapGenerator::addBonuses()
{
	PROFILE_FUNC();
	gDLL->NiTextOut("Adding Bonuses...");

	if (gDLL->getPythonIFace()->pythonAddBonuses() && !gDLL->getPythonIFace()->pythonUsingDefaultImpl())
	{
		return; // Python override
	}

	for (int iOrder = 0; iOrder < GC.getNumBonusInfos(); iOrder++)
	{
		for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			gDLL->callUpdater();
			if (GC.getBonusInfo((BonusTypes)iI).getPlacementOrder() == iOrder)
			{
				if (gDLL->getPythonIFace()->pythonAddBonusType((BonusTypes)iI) && !gDLL->getPythonIFace()->pythonUsingDefaultImpl())
				{
					; // Python override
				}
				else
				{
					if (GC.getBonusInfo((BonusTypes)iI).isOneArea())
					{
						addUniqueBonusType((BonusTypes)iI);
					}
					else
					{
						addNonUniqueBonusType((BonusTypes)iI);
					}
				}
			}
		}
	}
}

void CvMapGenerator::addUniqueBonusType(BonusTypes eBonusType)
{
	int* piAreaTried = new int[GC.getMap().getNumAreas()];

	for (int iI = 0; iI < GC.getMap().getNumAreas(); iI++)
	{
		piAreaTried[iI] = FFreeList::INVALID_INDEX;
	}

	CvBonusInfo& pBonusInfo = GC.getBonusInfo(eBonusType);

	int iBonusCount = calculateNumBonusesToAdd(eBonusType);

	bool bIgnoreLatitude = false;
	gDLL->getPythonIFace()->pythonIsBonusIgnoreLatitudes(&bIgnoreLatitude);

	FAssertMsg(pBonusInfo.isOneArea(), "addUniqueBonusType called with non-unique bonus type");

	while (true)
	{
		int iBestValue = 0;
		int iLoop = 0;
		CvArea *pBestArea = NULL;
		CvArea *pLoopArea = NULL;

		for(pLoopArea = GC.getMap().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop))
		{
			bool bTried = false;

			for (int iI = 0; iI < GC.getMap().getNumAreas(); iI++)
			{
				if (pLoopArea->getID() == piAreaTried[iI])
				{
					bTried = true;
					break;
				}
			}

			if (!bTried)
			{
				int iNumUniqueBonusesOnArea = pLoopArea->countNumUniqueBonusTypes() + 1; // number of unique bonuses starting on the area, plus this one
				int iNumTiles = pLoopArea->getNumTiles();
				int iValue = iNumTiles / iNumUniqueBonusesOnArea;

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestArea = pLoopArea;
				}
			}
		}

		if (pBestArea == NULL)
		{
			break; // can't place bonus on any area
		}

		for (int iI = 0; iI < GC.getMap().getNumAreas(); iI++)
		{
			if (piAreaTried[iI] == FFreeList::INVALID_INDEX)
			{
				piAreaTried[iI] = pBestArea->getID();
				break;
			}
		}

		// Place the bonuses:

		std::vector<int> aiShuffle(GC.getMap().numPlotsINLINE());
		GC.getGameINLINE().getMapRand().shuffleSequence(aiShuffle, "addUniqueBonusType shuffle");

		for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
		{
			CvPlot* pPlot = GC.getMap().plotByIndexINLINE(aiShuffle[iI]);
			FAssertMsg(pPlot != NULL, "addUniqueBonusType(): pPlot is null");

			if (GC.getMap().getNumBonuses(eBonusType) >= iBonusCount)
			{
				break; // We already have enough
			}

			if (pBestArea == pPlot->area())
			{
				if (canPlaceBonusAt(eBonusType, pPlot->getX_INLINE(), pPlot->getY_INLINE(), bIgnoreLatitude))
				{
					pPlot->setBonusType(eBonusType);

					for (int iDX = -(pBonusInfo.getGroupRange()); iDX <= pBonusInfo.getGroupRange(); iDX++)
					{
						for (int iDY = -(pBonusInfo.getGroupRange()); iDY <= pBonusInfo.getGroupRange(); iDY++)
						{
							if (GC.getMap().getNumBonuses(eBonusType) < iBonusCount)
							{
								CvPlot* pLoopPlot	= plotXY(pPlot->getX_INLINE(), pPlot->getY_INLINE(), iDX, iDY);

								if (pLoopPlot != NULL && (pLoopPlot->area() == pBestArea))
								{
									if (canPlaceBonusAt(eBonusType, pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), bIgnoreLatitude))
									{
										if (GC.getGameINLINE().getMapRandNum(100, "addUniqueBonusType") < pBonusInfo.getGroupRand())
										{
											pLoopPlot->setBonusType(eBonusType);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	SAFE_DELETE_ARRAY(piAreaTried);
}

void CvMapGenerator::addNonUniqueBonusType(BonusTypes eBonusType)
{
	int iBonusCount = calculateNumBonusesToAdd(eBonusType);

	if (iBonusCount == 0)
	{
		return;
	}

	std::vector<int> aiShuffle(GC.getMap().numPlotsINLINE());
	GC.getGameINLINE().getMapRand().shuffleSequence(aiShuffle, "addNonUniqueBonusType shuffle");

	CvBonusInfo& pBonusInfo = GC.getBonusInfo(eBonusType);

	bool bIgnoreLatitude = false;
	gDLL->getPythonIFace()->pythonIsBonusIgnoreLatitudes(&bIgnoreLatitude);

	CvPlot* pPlot = NULL;
	for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		pPlot = GC.getMap().plotByIndexINLINE(aiShuffle[iI]);
		if (canPlaceBonusAt(eBonusType, pPlot->getX_INLINE(), pPlot->getY_INLINE(), bIgnoreLatitude))
		{
			pPlot->setBonusType(eBonusType);
			iBonusCount--;

			for (int iDX = -(pBonusInfo.getGroupRange()); iDX <= pBonusInfo.getGroupRange(); iDX++)
			{
				for (int iDY = -(pBonusInfo.getGroupRange()); iDY <= pBonusInfo.getGroupRange(); iDY++)
				{
					if (iBonusCount > 0)
					{
						CvPlot* pLoopPlot	= plotXY(pPlot->getX_INLINE(), pPlot->getY_INLINE(), iDX, iDY);

						if (pLoopPlot != NULL)
						{
							if (canPlaceBonusAt(eBonusType, pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), bIgnoreLatitude))
							{
								if (GC.getGameINLINE().getMapRandNum(100, "addNonUniqueBonusType") < pBonusInfo.getGroupRand())
								{
									pLoopPlot->setBonusType(eBonusType);
									iBonusCount--;
								}
							}
						}
					}
				}
			}

			FAssertMsg(iBonusCount >= 0, "iBonusCount must be >= 0");

			if (iBonusCount == 0)
			{
				break;
			}
		}
	}
}


void CvMapGenerator::addGoodies()
{
	PROFILE_FUNC();

	if (gDLL->getPythonIFace()->pythonAddGoodies() && !gDLL->getPythonIFace()->pythonUsingDefaultImpl())
	{
		return; // Python override
	}

	gDLL->NiTextOut("Adding Goodies...");

	if (GC.getEraInfo(GC.getGameINLINE().getStartEra()).isNoGoodies())
	{
		return;
	}

	int iNumPlots = GC.getMap().numPlotsINLINE();
	std::vector<int> aiShuffle(iNumPlots);
	GC.getGameINLINE().getMapRand().shuffleSequence(aiShuffle, "addNonUniqueBonusType shuffle");

	for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	{
		if (GC.getImprovementInfo((ImprovementTypes)iI).isGoody() && GC.getImprovementInfo((ImprovementTypes)iI).getTilesPerGoody() > 0)
		{
			for (int iJ = 0; iJ < iNumPlots; iJ++)
			{
				gDLL->callUpdater();
				CvPlot *pPlot = GC.getMap().plotByIndexINLINE(aiShuffle[iJ]);
				FAssertMsg(pPlot, "pPlot is expected not to be NULL");
				//if (!(pPlot->isWater())) // R&R, ray, Goodies on Water 
				//{
					CvArea *pArea = GC.getMap().getArea(pPlot->getArea());
					FAssertMsg(pArea, "pArea is expected not to be NULL");
					if (pArea->getNumImprovements((ImprovementTypes)iI) < ((pArea->getNumTiles() + (GC.getImprovementInfo((ImprovementTypes)iI).getTilesPerGoody() / 2)) / GC.getImprovementInfo((ImprovementTypes) iI).getTilesPerGoody()))
					{
						if (canPlaceGoodyAt(((ImprovementTypes)iI), pPlot->getX_INLINE(), pPlot->getY_INLINE()))
						{
							pPlot->setImprovementType((ImprovementTypes)iI);
						}
					}
				//}
			}
		}
	}
}

void CvMapGenerator::addEurope()
{
	PROFILE_FUNC();
	gDLL->NiTextOut("Adding Europe...");

	for (int iEurope = 0; iEurope < GC.getNumEuropeInfos(); ++iEurope)
	{
		EuropeTypes eEurope = (EuropeTypes) iEurope;
		CvEuropeInfo& kEurope = GC.getEuropeInfo(eEurope);
		int iWidthPercent = kEurope.getWidthPercent();
		gDLL->getPythonIFace()->pythonGetEuropeWidthPercent(eEurope, &iWidthPercent);
		int iMinLandDistance = kEurope.getMinLandDistance();
		gDLL->getPythonIFace()->pythonGetEuropeMinLandDistance(eEurope, &iMinLandDistance);

		//try several times until at least one start europe is found
		bool bAnyEuropeFound = false;
		for ( ; iMinLandDistance >= 0 && !bAnyEuropeFound; iMinLandDistance--)
		{
			for (int i = 0; i < GC.getMap().numPlotsINLINE(); ++i)
			{
				CvPlot* pPlot = GC.getMap().plotByIndexINLINE(i);

				if (pPlot->isWater() && !pPlot->isEurope())
				{
					bool bEurope = false;
					switch (kEurope.getCardinalDirection())
					{
					case CARDINALDIRECTION_EAST:
						bEurope = (pPlot->getX_INLINE() > (100 - iWidthPercent) * GC.getMap().getGridWidthINLINE() / 100);
						break;
					case CARDINALDIRECTION_WEST:
						bEurope = (pPlot->getX_INLINE() < iWidthPercent * GC.getMap().getGridWidthINLINE() / 100);
						break;
					case CARDINALDIRECTION_NORTH:
						bEurope = (pPlot->getY_INLINE() > (100 - iWidthPercent) * GC.getMap().getGridHeightINLINE() / 100);
						break;
					case CARDINALDIRECTION_SOUTH:
						bEurope = (pPlot->getY_INLINE() < iWidthPercent * GC.getMap().getGridHeightINLINE() / 100);
						break;
					default:
						FAssertMsg(false, "Invalid direction");
						break;
					}

					for (int i = -iMinLandDistance; i <= iMinLandDistance && bEurope; i++)
					{
						for (int j = -iMinLandDistance; j <= iMinLandDistance && bEurope; j++)
						{
							CvPlot* pLoopPlot = ::plotXY(pPlot->getX_INLINE(), pPlot->getY_INLINE(), i, j);
							if (pLoopPlot != NULL)
							{
								if (!pLoopPlot->isWater())
								{
									bEurope = false;
								}
							}
						}
					}

					if (bEurope)
					{
						if (pPlot->getFeatureType() != NO_FEATURE && GC.getFeatureInfo(pPlot->getFeatureType()).isImpassable())
						{
							pPlot->setFeatureType(NO_FEATURE);
						}

						if (pPlot->isImpassable())
						{
							bEurope = false;
						}
					}

					if (bEurope)
					{
						pPlot->setEurope(eEurope);
						bAnyEuropeFound = true;
					}
				}
			}
		}
	}
}


void CvMapGenerator::eraseRivers()
{
	for (int i = 0; i < GC.getMap().numPlotsINLINE(); i++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexINLINE(i);
		if (pPlot->isNOfRiver())
		{
			pPlot->setNOfRiver(false, NO_CARDINALDIRECTION);
		}
		if (pPlot->isWOfRiver())
		{
			pPlot->setWOfRiver(false, NO_CARDINALDIRECTION);
		}
	}
}

void CvMapGenerator::eraseFeatures()
{
	for (int i = 0; i < GC.getMap().numPlotsINLINE(); i++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexINLINE(i);
		pPlot->setFeatureType(NO_FEATURE);
	}
}

//WTP, ray, Randomize Features Map Option - START
void CvMapGenerator::eraseFeaturesOnLand()
{
	for (int i = 0; i < GC.getMap().numPlotsINLINE(); i++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexINLINE(i);
		// only for Land Plots
		if(!pPlot->isWater())
		{
			pPlot->setFeatureType(NO_FEATURE);
		}
	}
}
//WTP, ray, Randomize Features Map Option - END

void CvMapGenerator::eraseBonuses()
{
	for (int i = 0; i < GC.getMap().numPlotsINLINE(); i++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexINLINE(i);
		pPlot->setBonusType(NO_BONUS);
	}
}

void CvMapGenerator::eraseGoodies()
{
	for (int i = 0; i < GC.getMap().numPlotsINLINE(); i++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexINLINE(i);
		if (pPlot->isGoody())
		{
			pPlot->removeGoody();
		}
	}
}

void CvMapGenerator::eraseEurope()
{
	for (int i = 0; i < GC.getMap().numPlotsINLINE(); i++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexINLINE(i);
		pPlot->setEurope(NO_EUROPE);
	}
}

//------------------------------------------------------------------------------------------------
//
// Call python function to generate random map
// It will call applyMapData when it's done
//

void CvMapGenerator::generateRandomMap()
{
	PROFILE_FUNC();

	gDLL->getPythonIFace()->callFunction(gDLL->getPythonIFace()->getMapScriptModule(), "beforeGeneration");

	if (gDLL->getPythonIFace()->pythonGenerateRandomMap() && !gDLL->getPythonIFace()->pythonUsingDefaultImpl())
	{
		return; // Python override
	}

	char buf[256];

	sprintf(buf, "Generating Random Map %S, %S...", gDLL->getMapScriptName().GetCString(), GC.getWorldInfo(GC.getMap().getWorldSize()).getDescription());
	gDLL->NiTextOut(buf);

	generatePlotTypes();
	generateTerrain();
}

void CvMapGenerator::generatePlotTypes()
{
	int* paiPlotTypes = new int[GC.getMap().numPlotsINLINE()];

	int iNumPlots = GC.getMap().numPlotsINLINE();

	std::vector<int> plotTypesOut;
	if (gDLL->getPythonIFace()->pythonGeneratePlotTypes(plotTypesOut) && !gDLL->getPythonIFace()->pythonUsingDefaultImpl()) // Python override
	{
		FAssertMsg((int)plotTypesOut.size() == iNumPlots, "python generatePlotTypes() should return list with length numPlotsINLINE");
		for (int iI = 0; iI < iNumPlots; iI++)
		{
			paiPlotTypes[iI] = plotTypesOut[iI];
		}
	}
	else
	{
		for (int iI = 0; iI < iNumPlots; iI++)
		{
			paiPlotTypes[iI] = PLOT_LAND;
		}
	}

	setPlotTypes(paiPlotTypes);

	SAFE_DELETE_ARRAY(paiPlotTypes);
}

void CvMapGenerator::generateTerrain()
{
	PROFILE_FUNC();

	std::vector<int> terrainMapOut;
	if (gDLL->getPythonIFace()->pythonGenerateTerrainTypes(terrainMapOut) && !gDLL->getPythonIFace()->pythonUsingDefaultImpl()) // Python override
	{
		int iNumPlots = GC.getMap().numPlotsINLINE();
		FAssertMsg((int)terrainMapOut.size() == iNumPlots, "python generateTerrain() should return a list with length CyMap().getNumPoints()");
		// Generate terrain for each plot
		for (int iI = 0; iI < iNumPlots; iI++)
		{
			gDLL->callUpdater();
			GC.getMap().plotByIndexINLINE(iI)->setTerrainType(((TerrainTypes)(terrainMapOut[iI])), false, false);
		}
	}
}

// Allows for user-defined Python Actions for map generation after it's already been created
void CvMapGenerator::afterGeneration()
{
	PROFILE_FUNC();

	gDLL->getPythonIFace()->callFunction(gDLL->getPythonIFace()->getMapScriptModule(), "afterGeneration");
}

void CvMapGenerator::setPlotTypes(const int* paiPlotTypes)
{
	CvPlot* pLoopPlot;
	int iNumPlots;

	iNumPlots = GC.getMap().numPlotsINLINE();

	for (int iI = 0; iI < iNumPlots; iI++)
	{
		gDLL->callUpdater();
		GC.getMap().plotByIndexINLINE(iI)->setPlotType(((PlotTypes)(paiPlotTypes[iI])), false, false);
	}

	GC.getMap().recalculateAreas();

	for (int iI = 0; iI < iNumPlots; iI++)
	{
		gDLL->callUpdater();
		pLoopPlot = GC.getMap().plotByIndexINLINE(iI);

		if (pLoopPlot->isWater())
		{
			if (pLoopPlot->isAdjacentToLand())
			{
				//WTP, ray, Large Rivers - START
				//WTP, ray, Lakes - START
				//we do not want to change Large Rivers to Coast, only Ocean
				//we also do not want to change Lake to Coast, only Ocean
				if (pLoopPlot->getTerrainType() != TERRAIN_LARGE_RIVERS && pLoopPlot->getTerrainType() != TERRAIN_LAKE && pLoopPlot->getTerrainType() != TERRAIN_ICE_LAKE  && pLoopPlot->getTerrainType() != TERRAIN_SHALLOW_COAST)
				{
					//WTP, ray, Safety Check for Deep Water Coast if there is Ocean or Deep Coast adjacent - START
					// case 1: There is Deep Water Coast or Ocean adjacent
					// we generate "Deep Water Coast"
					if (pLoopPlot->hasOtherAdjacentOceanOrDeepWaterCoast())
					{
						pLoopPlot->setTerrainType(TERRAIN_COAST, false, false);
					}
					// case 2: There seems to be just land or shallow coast around
					// we genearte "Shallow Water Coast" - to avoid dead locked Deep Water Coast Cities
					else
					{
						pLoopPlot->setTerrainType(TERRAIN_SHALLOW_COAST, false, false);
					}
					//WTP, ray, Safety Check for Deep Water Coast if there is Ocean or Deep Coast adjacent - END
				}
				//WTP, ray, Large Rivers - end
			}
			else
			{
				pLoopPlot->setTerrainType(TERRAIN_OCEAN, false, false);
			}
		}
	}
}

// Protected functions:

int CvMapGenerator::getRiverValueAtPlot(CvPlot* pPlot)
{
	CvPlot* pAdjacentPlot;
	CvRandom riverRand;
	int iSum;
	int iI;

	FAssert(pPlot != NULL);

	long result = 0;
	if (gDLL->getPythonIFace()->pythonGetRiverAltitude(pPlot, &result) && !gDLL->getPythonIFace()->pythonUsingDefaultImpl()) // Python override
	{
		if (result >= 0)
		{
			return result;
		}
		else
		{
			FAssertMsg(false, "pythonGetRiverAltitude() must return >= 0");
		}
	}

	iSum = result;

	iSum += ((NUM_PLOT_TYPES - pPlot->getPlotType()) * 20);

	for (iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pAdjacentPlot = plotDirection(pPlot->getX_INLINE(), pPlot->getY_INLINE(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			iSum += (NUM_PLOT_TYPES - pAdjacentPlot->getPlotType());
		}
		else
		{
			iSum += (NUM_PLOT_TYPES * 10);
		}
	}

	riverRand.init((pPlot->getX_INLINE() * 43251267) + (pPlot->getY_INLINE() * 8273903));

	iSum += (riverRand.get(10, "River Rand"));

	return iSum;
}

int CvMapGenerator::calculateNumBonusesToAdd(BonusTypes eBonusType)
{
	CvBonusInfo& pBonusInfo = GC.getBonusInfo(eBonusType);

	// Calculate iBonusCount, the amount of this bonus to be placed:

	int iRand1 = GC.getGameINLINE().getMapRandNum(pBonusInfo.getRandAppearance1(), "calculateNumBonusesToAdd-1");
	int iRand2 = GC.getGameINLINE().getMapRandNum(pBonusInfo.getRandAppearance2(), "calculateNumBonusesToAdd-2");
	int iRand3 = GC.getGameINLINE().getMapRandNum(pBonusInfo.getRandAppearance3(), "calculateNumBonusesToAdd-3");
	int iRand4 = GC.getGameINLINE().getMapRandNum(pBonusInfo.getRandAppearance4(), "calculateNumBonusesToAdd-4");
	int iBaseCount = pBonusInfo.getConstAppearance() + iRand1 + iRand2 + iRand3 + iRand4;

	bool bIgnoreLatitude = false;
	gDLL->getPythonIFace()->pythonIsBonusIgnoreLatitudes(&bIgnoreLatitude);

	// Calculate iNumPossible, the number of plots that are eligible to have this bonus:

	int iLandTiles = 0;
	if (pBonusInfo.getTilesPer() > 0)
	{
		int iNumPossible = 0;
		for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
		{
			CvPlot* pPlot = GC.getMap().plotByIndexINLINE(iI);
			if (pPlot->canHaveBonus(eBonusType, bIgnoreLatitude))
			{
				iNumPossible++;
			}
		}
		iLandTiles = (iNumPossible / pBonusInfo.getTilesPer());
	}

	int iPlayers = (GC.getGameINLINE().countCivPlayersAlive() * pBonusInfo.getPercentPerPlayer()) / 100;
	int iBonusCount = (iBaseCount * (iLandTiles + iPlayers)) / 100;
	iBonusCount = std::max(1, iBonusCount);
	return iBonusCount;
}
