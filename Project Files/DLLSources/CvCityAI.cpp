// cityAI.cpp

#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "CvGameCoreUtils.h"
#include "CvCityAI.h"
#include "CvGameAI.h"
#include "CvPlot.h"
#include "CvArea.h"
#include "CvPlayerAI.h"
#include "CvTeamAI.h"
#include "CyCity.h"
#include "CyArgsList.h"
#include "CvInfos.h"
#include "FProfiler.h"

#include "CvDLLPythonIFaceBase.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLFAStarIFaceBase.h"

#define BUILDINGFOCUS_NO_RECURSION			(1 << 31)
#define BUILDINGFOCUS_BUILD_ANYTHING		(1 << 30)
#define BUILDINGFOCUS_MILITARY				(1 << 29)	// TAC - AI Buildings - koma13

#define YIELD_DISCOUNT_TURNS 			10

// Public Functions...

CvCityAI::CvCityAI()
{
	m_aiYieldOutputWeight = new int[NUM_YIELD_TYPES];
	m_aiNeededYield = new int[NUM_YIELD_TYPES];
	m_aiTradeBalance = new int[NUM_YIELD_TYPES];
	m_aiYieldAdvantage = new int[NUM_YIELD_TYPES];
	
	m_aiEmphasizeYieldCount = new int[NUM_YIELD_TYPES];
	m_bForceEmphasizeCulture = false;
	m_aiPlayerCloseness = new int[MAX_PLAYERS];

	m_abEmphasize = NULL;

	AI_reset();
}


CvCityAI::~CvCityAI()
{
	AI_uninit();

	SAFE_DELETE_ARRAY(m_aiYieldOutputWeight);
	SAFE_DELETE_ARRAY(m_aiNeededYield);
	SAFE_DELETE_ARRAY(m_aiTradeBalance);
	SAFE_DELETE_ARRAY(m_aiYieldAdvantage);
	SAFE_DELETE_ARRAY(m_aiEmphasizeYieldCount);
	SAFE_DELETE_ARRAY(m_aiPlayerCloseness);
}


void CvCityAI::AI_init()
{
	AI_reset();

	//--------------------------------
	// Init other game data
	AI_assignWorkingPlots();

	AI_updateBestBuild();

	AI_assignDesiredYield();
	
	m_iFoundValue = plot()->getFoundValue(getOwner());
}


void CvCityAI::AI_uninit()
{
	SAFE_DELETE_ARRAY(m_abEmphasize);
}


// FUNCTION: AI_reset()
// Initializes data members that are serialized.
void CvCityAI::AI_reset()
{
	int iI;

	AI_uninit();
	
	m_iGiftTimer = 0;
	m_iTradeTimer = 0; // R&R, ray, Natives Trading - START
	m_eDesiredYield = NO_YIELD;
	
	m_iTargetSize = 0;
	m_iFoundValue = 0;

	m_iEmphasizeAvoidGrowthCount = 0;
	m_bForceEmphasizeCulture = false;

	m_bPort = false;
	m_bAssignWorkDirty = false;
	m_bChooseProductionDirty = false;
	
	m_iWorkforceHack = 0;

	m_routeToCity.reset();
	
	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_aiYieldOutputWeight[iI] = 0;
		m_aiNeededYield[iI] = 0;
		m_aiTradeBalance[iI] = 0;
		m_aiYieldAdvantage[iI] = 0;
	}	

	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_aiEmphasizeYieldCount[iI] = 0;
	}

	for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		m_aiBestBuildValue[iI] = NO_BUILD;
	}

	for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		m_aeBestBuild[iI] = NO_BUILD;
	}
	
	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_aiPlayerCloseness[iI] = 0;
	}
	m_iCachePlayerClosenessTurn = -1;
	m_iCachePlayerClosenessDistance = -1;
	
	m_iNeededFloatingDefenders = -1;
	m_iNeededFloatingDefendersCacheTurn = -1;

	m_iWorkersNeeded = 0;
	m_iWorkersHave = 0;

	FAssertMsg(m_abEmphasize == NULL, "m_abEmphasize not NULL!!!");
	FAssertMsg(GC.getNumEmphasizeInfos() > 0,  "GC.getNumEmphasizeInfos() is not greater than zero but an array is being allocated in CvCityAI::AI_reset");
	m_abEmphasize = new bool[GC.getNumEmphasizeInfos()];
	for (iI = 0; iI < GC.getNumEmphasizeInfos(); iI++)
	{
		m_abEmphasize[iI] = false;
	}
}


void CvCityAI::AI_doTurn()
{
	PROFILE_FUNC();
	
	AI_doTradedYields();
	
	if (!isHuman())
	{
		AI_updateRequiredYieldLevels();
	}

	AI_updateWorkersNeededHere();

	AI_updateBestBuild();

	AI_updateRouteToCity();
	
	if (AI_getGiftTimer() > 0)
	{
		AI_changeGiftTimer(-1);
	}

	// R&R, ray, Natives Trading - START
	if (AI_getTradeTimer() > 0)
	{
		AI_changeTradeTimer(-1);
	}
	// R&R, ray, Natives Trading - END

	if (isHuman())
	{
	    if (isProductionAutomated())
	    {
	        AI_doHurry();	        
	    }
		return;
	}
	
	AI_doHurry();

	AI_doEmphasize();
}

//struct PopUnit
//{
//	CvUnit* m_pUnit;
//	ProfessionTypes m_eIdealProfession;
//	
//	int calculateValue() const
//	{
//		int iValue = 100;
//		if (eIdealProfession != NO_PROFESSION)
//		{
//			iValue += 100;
//			if (GC.getProfessionInfo(eIdealProfession).getYieldProduced() == YIELD_FOOD)
//			{
//				iValue += 50;
//			}
//		}
//		return iValue;
//	}
//	
//	bool operator < (const PopUnit& rhs) const
//	{
//		return calculateValue() < rhs.calculateValue();
//	}
//};

void CvCityAI::AI_assignWorkingPlots()
{
	PROFILE_FUNC();
	
	if (isOccupation())
	{
	    return;
	}

	if (getPopulation() == 0)
	{
		return;
	}
	
	AI_assignCityPlot();

	GET_PLAYER(getOwnerINLINE()).AI_manageEconomy();
	AI_updateNeededYields();

	//remove non-city people
	removeNonCityPopulationUnits();


	/*
	Citizen Algorithm:
	Take all citizens which aren't locked in place, put them in a pool.
	Now take the first citizen from the pool and place it on the highest value plot,
	unless the existing worker has even higher value. 
	If a worker is displaced, it is returned to the pool.
	Take the next citizen from the pool..
	Once the pool is empty, the workforce is optimally allocated.
	
	This algorithm will find a good workforce
	it's kind of expensive, but not THAT expensive with only 8 plots per colony
	and the population numbers being low.
	*/
	std::deque<CvUnit*> citizens;
		
	for (int iPass = 0; iPass < 3; ++iPass)
	{
		for (uint i = 0; i < m_aPopulationUnits.size(); ++i)
		{
			CvUnit* pUnit = m_aPopulationUnits[i];
			if (!pUnit->isColonistLocked())
			{
				ProfessionTypes eIdealProfession = pUnit->AI_getIdealProfession();
				if (eIdealProfession != NO_PROFESSION)
				{
					if (!pUnit->canHaveProfession(eIdealProfession, true, NULL))
					{
						eIdealProfession = NO_PROFESSION;
					}
				}
				if (eIdealProfession != NO_PROFESSION)
				{
					// R&R, ray , MYCP partially based on code of Aymerick - START
					if (GC.getProfessionInfo(eIdealProfession).getYieldsConsumed(0) != NO_YIELD)
					// R&R, ray , MYCP partially based on code of Aymerick - END
					{
						if (iPass == 0)
						{
							citizens.push_back(pUnit);
						}
					}
					else
					{
						if (iPass == 1)
						{
							citizens.push_back(pUnit);
						}
					}
				}
				else
				{
					if (iPass == 2)
					{
						citizens.push_back(pUnit);
					}
				}
			}
		}
	}
		
	uint iMaxIterations = citizens.size() * 4;
	
	uint iCount = 0;
	while (!citizens.empty())
	{
		CvUnit* pUnit = citizens.back();
		citizens.pop_back();
		FAssert (pUnit != NULL);
		
		CvPlot* pWorkedPlot =  getPlotWorkedByUnit(pUnit);
		if (pWorkedPlot != NULL)
		{
			clearUnitWorkingPlot(pWorkedPlot);
		}
		
		CvUnit* pOldUnit = AI_assignToBestJob(pUnit);
		if (pOldUnit != NULL)
		{
			if (std::find(citizens.begin(), citizens.end(), pOldUnit) == citizens.end())
			{
				citizens.push_front(pOldUnit);
			}
		}
		iCount++;
		if (iCount > iMaxIterations)
		{
			FAssertMsg(false, "AI plot assignment confusion");
			break;
		}
	}
	
	if (isNative())
	{
		AI_setAssignWorkDirty(false);
		return;
	}
	

	//Now see if swapping citizens will help.
	for (uint i = 0; i < m_aPopulationUnits.size(); ++i)
	{
		CvUnit* pUnit = m_aPopulationUnits[i];
		if (pUnit != NULL)
		{
			if (!pUnit->isColonistLocked())
			{
				if (pUnit->getProfession() != NO_PROFESSION)
				{
					AI_juggleColonist(pUnit);
				}
			}
		}
	}
	
	AI_setAssignWorkDirty(false);

	if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && isCitySelected())
	{
		gDLL->getInterfaceIFace()->setDirty(CitizenButtons_DIRTY_BIT, true);
	}
}


void CvCityAI::AI_updateAssignWork()
{
	if (AI_isAssignWorkDirty())
	{
		AI_assignWorkingPlots();
	}
}


bool CvCityAI::AI_avoidGrowth() const
{
	PROFILE_FUNC();

	if (AI_isEmphasizeAvoidGrowth())
	{
		return true;
	}
	
	return false;
}

void CvCityAI::AI_setAvoidGrowth(bool bNewValue)
{
	bool bCurrentValue = AI_isEmphasizeAvoidGrowth();
	
	if (bCurrentValue == bNewValue)
	{
		return;
	}
	
	for (int i = 0; i < GC.getNumEmphasizeInfos(); ++i)
	{
		if (GC.getEmphasizeInfo((EmphasizeTypes)i).isAvoidGrowth())
		{
			AI_setEmphasize((EmphasizeTypes)i, bNewValue);
		}
	}
}


bool CvCityAI::AI_ignoreGrowth() const
{
	PROFILE_FUNC();

	if (AI_getEmphasizeYieldCount(YIELD_FOOD) <= 0)
	{
		if (!AI_foodAvailable((isHuman()) ? 0 : 1))
		{
			return true;
		}
	}

	return false;
}


void CvCityAI::AI_chooseProduction()
{
	PROFILE_FUNC();

	CvArea* pWaterArea;
	
	CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());

	if (isProduction())
	{
		if (getProduction() > 0)
		{
			// if less than 3 turns left, keep building current item
			if (getProductionTurnsLeft() <= 3)
			{
				return;
			}
		}
		clearOrderQueue();
	}
	
	// only clear the dirty bit if we actually do a check, multiple items might be queued
	AI_setChooseProductionDirty(false);

	if (GC.getUSE_AI_CHOOSE_PRODUCTION_CALLBACK()) // K-Mod. block unused python callbacks
	{
		// allow python to handle it
		CyCity* pyCity = new CyCity(this);
		CyArgsList argsList;
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
		long lResult = 0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "AI_chooseProduction", argsList.makeFunctionArgs(), &lResult);
		delete pyCity;	// python fxn must not hold on to this pointer
		if (lResult == 1)
		{
			return;
		}
	}

	CvArea* pArea = area();
	pWaterArea = waterArea();
	bool bMaybeWaterArea = false;
	
	if (pWaterArea != NULL)
	{
		bMaybeWaterArea = true;
		if (!GET_TEAM(getTeam()).AI_isWaterAreaRelevant(pWaterArea))
		{
			pWaterArea = NULL;
		}
	}
	
	if (!isNative())
	{
		if ((GC.getGame().getGameTurn() - getGameTurnAcquired()) > 5)	// TAC - AI Training - koma13
		{
			int iAreaCities = pArea->getCitiesPerPlayer(getOwnerINLINE());
			if (iAreaCities > 1)
			{
				if ((pArea->getNumAIUnits(getOwnerINLINE(), UNITAI_WAGON) + pArea->getNumTrainAIUnits(getOwnerINLINE(), UNITAI_WAGON)) < (iAreaCities / 2))
				{
					if (AI_chooseUnit(UNITAI_WAGON))
					{
						return;
					}
				}
			}

			// Erik: Only consider building a coastal transport if at least one other city is coastally reachable and the water area is valid
			if (pWaterArea != NULL && AI_hasCoastalRoute())
			{
				if ((GET_PLAYER(getOwnerINLINE()).AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_TRANSPORT_COAST)) < kPlayer.countNumCoastalCities() / 2)
				{
					if (AI_chooseUnit(UNITAI_TRANSPORT_COAST))
					{
						return;
					}
				}
			}
		}
	}

	if (AI_chooseBuilding(0, MAX_INT, 8))
	{
		return;
	}
	
	if (isNative())
	{
		if (AI_chooseUnit(UNITAI_DEFENSIVE, false))
		{
			return;
		}
	}
	
	if (AI_chooseUnit(NO_UNITAI, false))
	{
		return;
	}

	// TAC - AI Buildings - koma13 - START
	
	/*
	if (AI_chooseBuilding(BUILDINGFOCUS_BUILD_ANYTHING, MAX_INT, 8))
	{	
		return;
	}
	*/

	if (isNative() || isBestPortCity())
	{
		if (AI_chooseBuilding(BUILDINGFOCUS_BUILD_ANYTHING, MAX_INT, 8))
		{	
			return;
		}
	}

	// TAC - AI Buildings - koma13 - END

	if (AI_chooseUnit(NO_UNITAI, true))
	{
		return;
	}

	// TAC - AI produces FF Points - koma13 - START
	if (AI_chooseConvince())
	{
		return;
	}
	// TAC - AI produces FF Points - koma13 - END
	
	// TAC - AI Buildings - koma13 - START
	if (AI_chooseBuilding(BUILDINGFOCUS_MILITARY, MAX_INT, 8))
	{
		return;
	}
	// TAC - AI Buildings - koma13 - END
	
	//colonies should always be building something
	FAssertMsg(isNative(), "AI not building anything.");
}


UnitTypes CvCityAI::AI_bestUnit(bool bAsync, UnitAITypes* peBestUnitAI, bool bPickAny) const
{
	int aiUnitAIVal[NUM_UNITAI_TYPES] = {};
	UnitTypes eUnit = NO_UNIT;
	UnitTypes eBestUnit = NO_UNIT;

	int iBestValue = 0;
	int iI;

	if (peBestUnitAI != NULL)
	{
		*peBestUnitAI = NO_UNITAI;
	}

	for (iI = 0; iI < NUM_UNITAI_TYPES; iI++)
	{
		if (bAsync)
		{
			aiUnitAIVal[iI] += GC.getASyncRand().get(25, "AI Best UnitAI ASYNC");
		}
		else
		{
			//aiUnitAIVal[iI] += GC.getGameINLINE().getSorenRandNum(100, "AI Best UnitAI");
			//Erik: Less initial randomness for unit selection
			aiUnitAIVal[iI] += GC.getGameINLINE().getSorenRandNum(25, "AI Best UnitAI");
		}
	}

	for (iI = 0; iI < NUM_UNITAI_TYPES; iI++)
	{
		// Erik: Note that no leader is currently making use of this
		aiUnitAIVal[iI] *= std::max(0, (GC.getLeaderHeadInfo(getPersonalityType()).getUnitAIWeightModifier(iI) + 100));
		aiUnitAIVal[iI] /= 100;
		
		if (!bPickAny)
		{
			aiUnitAIVal[iI] *= GET_PLAYER(getOwnerINLINE()).AI_unitAIValueMultipler((UnitAITypes)iI);
			aiUnitAIVal[iI] /= 100;
		}
	}

	// TAC - AI Training - koma13 - START
	if (GET_PLAYER(getOwnerINLINE()).getParent() != NO_PLAYER)
	{
		for (iI = 0; iI < NUM_UNITAI_TYPES; iI++)
		{
			switch((UnitAITypes)iI)
			{
			case UNITAI_WAGON:
				{
					const int iAreaCities = area()->getCitiesPerPlayer(getOwnerINLINE());
					if ((iAreaCities <= 1) || ((area()->getNumAIUnits(getOwnerINLINE(), UNITAI_WAGON) + area()->getNumTrainAIUnits(getOwnerINLINE(), UNITAI_WAGON)) > (iAreaCities*(iAreaCities - 1)) / 2))
					{
						aiUnitAIVal[iI] = 0;
					}
					else
					{
						aiUnitAIVal[iI] = GET_PLAYER(getOwnerINLINE()).AI_unitAIValueMultipler((UnitAITypes)iI);
					}
				}
				break;
		
			case UNITAI_TRANSPORT_COAST:
				{
					CvArea* const pWaterArea = waterArea();
					
					int iValue = 0;
					
					if (pWaterArea != NULL && AI_hasCoastalRoute())
					{
						const int iAreaCities = area()->getCitiesPerPlayer(getOwnerINLINE());
						if (GET_PLAYER(getOwnerINLINE()).AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_TRANSPORT_COAST) <= iAreaCities / 2)
						{
							iValue = GET_PLAYER(getOwnerINLINE()).AI_unitAIValueMultipler((UnitAITypes)iI);
						}
					}
					aiUnitAIVal[iI] = iValue;
				}
				break;

			case UNITAI_DEFENSIVE:
				if (GET_PLAYER(getOwnerINLINE()).AI_totalDefendersNeeded(NULL, area(), true) <= 0)
				{
					aiUnitAIVal[iI] = 0;
				}
				break;
				
			case UNITAI_ASSAULT_SEA:
			case UNITAI_ESCORT_SEA:
				if (!GET_PLAYER(getOwnerINLINE()).AI_prepareAssaultSea())
				{
					aiUnitAIVal[iI] = 0;
				}
				break;
						
			case UNITAI_WORKER_SEA:
			case UNITAI_TRANSPORT_SEA:
			case UNITAI_PIRATE_SEA:
				if (GET_PLAYER(getOwnerINLINE()).AI_unitAIValueMultipler((UnitAITypes)iI) == 0)
				{
					aiUnitAIVal[iI] = 0;
				}
				break;
				
			default:
				break;
			}
		}
	}
	// TAC - AI Training - koma13 - END

	for (iI = 0; iI < NUM_UNITAI_TYPES; iI++)
	{
		if (aiUnitAIVal[iI] > iBestValue)
		{
			eUnit = AI_bestUnitAI(((UnitAITypes)iI), bAsync);

			if (eUnit != NO_UNIT)
			{
				iBestValue = aiUnitAIVal[iI];
				eBestUnit = eUnit;
				if (peBestUnitAI != NULL)
				{
					*peBestUnitAI = ((UnitAITypes)iI);
				}
			}
		}
	}

	return eBestUnit;
}


UnitTypes CvCityAI::AI_bestUnitAI(UnitAITypes eUnitAI, bool bAsync) const
{
	UnitTypes eLoopUnit;
	UnitTypes eBestUnit;
	int iValue;
	int iBestValue;
	int iOriginalValue;
	int iBestOriginalValue;
	int iI, iJ, iK;
	

	FAssertMsg(eUnitAI != NO_UNITAI, "UnitAI is not assigned a valid value");

	iBestOriginalValue = 0;

	for (iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		eLoopUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI)));

		if (eLoopUnit != NO_UNIT)
		{
			if (!isHuman() || (GC.getUnitInfo(eLoopUnit).getDefaultUnitAIType() == eUnitAI))
			{		
				if (canTrain(eLoopUnit))
				{
					iOriginalValue = GET_PLAYER(getOwnerINLINE()).AI_unitValue(eLoopUnit, eUnitAI, area());

					if (iOriginalValue > iBestOriginalValue)
					{
						iBestOriginalValue = iOriginalValue;
					}
				}
			}
		}
	}

	iBestValue = 0;
	eBestUnit = NO_UNIT;

	for (iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		eLoopUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI)));

		if (eLoopUnit != NO_UNIT)
		{
			if (!isHuman() || (GC.getUnitInfo(eLoopUnit).getDefaultUnitAIType() == eUnitAI))
			{
				
				if (canTrain(eLoopUnit))
				{
					iValue = GET_PLAYER(getOwnerINLINE()).AI_unitValue(eLoopUnit, eUnitAI, area());

					if (iValue > ((iBestOriginalValue * 2) / 3))
					{
						iValue *= (getProductionExperience(eLoopUnit) + 10);
						iValue /= 10;

						//free promotions. slow?
						//only 1 promotion per source is counted (ie protective isn't counted twice)
						int iPromotionValue = 0;
						//buildings
						for (iJ = 0; iJ < GC.getNumPromotionInfos(); iJ++)
						{
							if (isFreePromotion((PromotionTypes)iJ) && !GC.getUnitInfo(eLoopUnit).getFreePromotions((PromotionTypes)iJ))
							{
								if ((GC.getUnitInfo(eLoopUnit).getUnitCombatType() != NO_UNITCOMBAT) && GC.getPromotionInfo((PromotionTypes)iJ).getUnitCombat(GC.getUnitInfo(eLoopUnit).getUnitCombatType()))
								{
									iPromotionValue += 15;
									break;
								}
							}
						}

						//special to the unit
						for (iJ = 0; iJ < GC.getNumPromotionInfos(); iJ++)
						{
							if (GC.getUnitInfo(eLoopUnit).getFreePromotions(iJ))
							{
								iPromotionValue += 15;
								break;
							}
						}

						//traits
						for (iJ = 0; iJ < GC.getNumTraitInfos(); iJ++)
						{
							if (hasTrait((TraitTypes)iJ))
							{
								for (iK = 0; iK < GC.getNumPromotionInfos(); iK++)
								{
									if (GC.getTraitInfo((TraitTypes)iJ).isFreePromotion(iK))
									{
										if ((GC.getUnitInfo(eLoopUnit).getUnitCombatType() != NO_UNITCOMBAT) && GC.getTraitInfo((TraitTypes)iJ).isFreePromotionUnitCombat(GC.getUnitInfo(eLoopUnit).getUnitCombatType()))
										{
											iPromotionValue += 15;
											break;
										}
									}
								}
							}
						}

						iValue *= (iPromotionValue + 100);
						iValue /= 100;

						if (bAsync)
						{
							iValue *= (GC.getASyncRand().get(50, "AI Best Unit ASYNC") + 100);
							iValue /= 100;
						}
						else
						{
							iValue *= (GC.getGameINLINE().getSorenRandNum(50, "AI Best Unit") + 100);
							iValue /= 100;
						}


						iValue *= (GET_PLAYER(getOwnerINLINE()).getNumCities() * 2);
						iValue /= (GET_PLAYER(getOwnerINLINE()).getUnitClassCountPlusMaking((UnitClassTypes)iI) + GET_PLAYER(getOwnerINLINE()).getNumCities() + 1);

						FAssert((MAX_INT / 1000) > iValue);
						iValue *= 1000;

						iValue /= std::max(1, (4 + getProductionTurnsLeft(eLoopUnit, 0)));

						iValue = std::max(1, iValue);

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							eBestUnit = eLoopUnit;
						}
					}
				}
			}
		}
	}

	return eBestUnit;
}


BuildingTypes CvCityAI::AI_bestBuilding(int iFocusFlags, int iMaxTurns, bool bAsync) const
{
	return AI_bestBuildingThreshold(iFocusFlags, iMaxTurns, /*iMinThreshold*/ 0, bAsync);
}

/// <summary>Determine if there's a coastal route to another city. Both cities must be in different areas (cannot share continent/island)</summary>
bool CvCityAI::AI_hasCoastalRoute() const
{
	gDLL->getFAStarIFace()->ForceReset(&GC.getCoastalRouteFinder());

	// Erik: determine if it makes sense to build a coastal transport
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());

	int iLoop;
	for (CvCity* pLoopCity = kOwner.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kOwner.nextCity(&iLoop))
	{
		if (pLoopCity != this)
		{
			// Determine if these cities share a common water area
			if (waterArea() == pLoopCity->waterArea())
			{
				// Check if there's a coastal / cultural route between the cities
				if (gDLL->getFAStarIFace()->GeneratePath(&GC.getCoastalRouteFinder(), getX_INLINE(), getY_INLINE(), pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE(), false, getOwnerINLINE(), true))
				{
					// We found a valid path
					return true;
				}
			}
		}
	}

	return false;
}


BuildingTypes CvCityAI::AI_bestBuildingThreshold(int iFocusFlags, int iMaxTurns, int iMinThreshold, bool bAsync) const
{
	bool bAreaAlone = GET_PLAYER(getOwnerINLINE()).AI_isAreaAlone(area());

	int iProductionRank = findYieldRateRank(YIELD_HAMMERS);

	int iBestValue = 0;
	BuildingTypes eBestBuilding = NO_BUILDING;

	for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		BuildingTypes eLoopBuilding = ((BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(iI)));

		if ((eLoopBuilding != NO_BUILDING) && (!isHasConceptualBuilding(eLoopBuilding)))
		{
			if (canConstruct(eLoopBuilding))
			{
				int iValue = AI_buildingValue(eLoopBuilding, iFocusFlags);

				if (iValue > 0)
				{
					int iTurnsLeft = getProductionTurnsLeft(eLoopBuilding, 0);

					if (bAsync)
					{
						iValue *= (GC.getASyncRand().get(25, "AI Best Building ASYNC") + 100);
						iValue /= 100;
					}
					else
					{
						iValue *= (GC.getGameINLINE().getSorenRandNum(25, "AI Best Building") + 100);
						iValue /= 100;
					}

					iValue += getBuildingProduction(eLoopBuilding);
					
					
					bool bValid = ((iMaxTurns <= 0) ? true : false);
					if (!bValid)
					{
						bValid = (iTurnsLeft <= GC.getGameINLINE().AI_turnsPercent(iMaxTurns, GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getConstructPercent()));
					}

					if (bValid)
					{
						iValue = iValue / 2 + iValue / (1 + iTurnsLeft);

						iValue = std::max(1, iValue);

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							eBestBuilding = eLoopBuilding;
						}
					}
				}
			}
		}
	}
	return eBestBuilding;
}

BuildingTypes CvCityAI::AI_bestBuildingIgnoreRequirements(int iFocusFlags, int iMaxTurns) const
{
	
	int iBestValue = 0;
	BuildingTypes eBestBuilding = NO_BUILDING;

	for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		BuildingTypes eLoopBuilding = ((BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(iI)));

		if ((eLoopBuilding != NO_BUILDING) && (!isHasConceptualBuilding(eLoopBuilding)))
		{
			if (canConstruct(eLoopBuilding), true, true, true)
			{
				int iValue = AI_buildingValue(eLoopBuilding, iFocusFlags);
				
				if (getProductionBuilding() == eLoopBuilding)
				{
					iValue *= 125;
					iValue /= 100;
				}
				
				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					eBestBuilding = eLoopBuilding;
				}
			}
		}
	}
	
	return eBestBuilding;
}

// TAC - AI Buildings - koma13 - START
bool CvCityAI::AI_isProductionBuilding(BuildingTypes eBuilding, bool bMajorCity) const
{
	CvBuildingInfo& kBuildingInfo = GC.getBuildingInfo(eBuilding);
	BuildingClassTypes eBuildingClass = (BuildingClassTypes)kBuildingInfo.getBuildingClassType();
	
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	
	int iBestValue = -1;
	BuildingTypes eBestExisting = NO_BUILDING;
	
	for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); ++iBuildingClass)
	{
		BuildingTypes eLoopBuilding = (BuildingTypes) GC.getCivilizationInfo(GET_PLAYER(getOwnerINLINE()).getCivilizationType()).getCivilizationBuildings(iBuildingClass);
		if ((NO_BUILDING != eLoopBuilding) && (eLoopBuilding != eBuilding))
		{
			CvBuildingInfo& kLoopBuilding = GC.getBuildingInfo(eLoopBuilding);
			if (kLoopBuilding.getSpecialBuildingType() == kBuildingInfo.getSpecialBuildingType())
			{
				if (isHasConceptualBuilding(eLoopBuilding))
				{
					int iValue = kLoopBuilding.getSpecialBuildingPriority();
					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						eBestExisting = eLoopBuilding;
					}
				}
			}
		}
	}

	YieldTypes eYieldProduced = NO_YIELD;
	YieldTypes eYieldConsumed = NO_YIELD;

	ProfessionTypes eProfessionConsumed = NO_PROFESSION;
	ProfessionTypes eProfessionProduced = NO_PROFESSION;

	for (int i = 0; i < GC.getNumProfessionInfos(); ++i)
	{
		CvProfessionInfo& kProfession = GC.getProfessionInfo((ProfessionTypes)i);
			
		if (GC.getCivilizationInfo(kOwner.getCivilizationType()).isValidProfession(i))
		{
			if (kProfession.getSpecialBuilding() == kBuildingInfo.getSpecialBuildingType())
			{
				// R&R, ray , MYCP partially based on code of Aymerick - START
				eYieldProduced = (YieldTypes)kProfession.getYieldsProduced(0);
				eYieldConsumed = (YieldTypes)kProfession.getYieldsConsumed(0);
				// R&R, ray , MYCP partially based on code of Aymerick - END
				
				eProfessionProduced = (ProfessionTypes)i;

				if (eYieldProduced != NO_YIELD && eYieldConsumed != NO_YIELD)
				{
					for (int k = 0; k < GC.getNumProfessionInfos(); ++k)
					{
						CvProfessionInfo& kProfessionConsumed = GC.getProfessionInfo((ProfessionTypes)k);
			
						if (GC.getCivilizationInfo(kOwner.getCivilizationType()).isValidProfession(k))
						{
							// R&R, ray , MYCP partially based on code of Aymerick - START
							if ((YieldTypes)kProfessionConsumed.getYieldsProduced(0) == (YieldTypes)kProfession.getYieldsConsumed(0))
							// R&R, ray , MYCP partially based on code of Aymerick - END
							{
								eProfessionConsumed = (ProfessionTypes)k;
								break;
							}
						}
					}
				}

				break;
			}
		}
	}
	
	if (eProfessionConsumed != NO_PROFESSION)
	{
		if ((eYieldConsumed != YIELD_TOOLS) && hasOtherProductionBuilding(eBuilding, 1))
		{
			return false;
		}

		int iLoop;
		for (CvCity* pLoopCity = kOwner.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kOwner.nextCity(&iLoop))
		{
			if (pLoopCity != this && !pLoopCity->isBestPortCity())
			{
				if(pLoopCity->isProductionBuilding())
				{
					CvBuildingInfo& kProductionBuilding = GC.getBuildingInfo(pLoopCity->getProductionBuilding());
					if (kProductionBuilding.getSpecialBuildingType() == kBuildingInfo.getSpecialBuildingType())
					{
						return false;
					}
				}

				for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); ++iBuildingClass)
				{
					BuildingTypes eLoopBuilding = (BuildingTypes) GC.getCivilizationInfo(GET_PLAYER(getOwnerINLINE()).getCivilizationType()).getCivilizationBuildings(iBuildingClass);
					if (NO_BUILDING != eLoopBuilding)
					{
						CvBuildingInfo& kLoopBuilding = GC.getBuildingInfo(eLoopBuilding);
						if (kLoopBuilding.getSpecialBuildingType() == kBuildingInfo.getSpecialBuildingType())
						{
							if (pLoopCity->isHasConceptualBuilding(eLoopBuilding))
							{
								return false;
							}
						}
					}
				}
			}
		}

		int iAdvantage = 0;
		int iBestYield = 0;
		int iWorstYield = MAX_INT;
			
		for (CvCity* pLoopCity = kOwner.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kOwner.nextCity(&iLoop))
		{
			if (pLoopCity != NULL && !pLoopCity->isBestPortCity())
			{
				if ((eYieldConsumed == YIELD_TOOLS) || !pLoopCity->hasOtherProductionBuilding(eBuilding, 1))
				{
					int iOutput = pLoopCity->getPotentialProductionOutput(eProfessionConsumed);
					if (iOutput > 0)
					{
						iBestYield = std::max(iBestYield, iOutput);
						iWorstYield = std::min(iWorstYield, iOutput);
					}
				}
			}
		}						
		
		if (iBestYield > 0)
		{
			iAdvantage = (100 * getPotentialProductionOutput(eProfessionConsumed)) / iBestYield;
			
			if ((eYieldConsumed != YIELD_TOOLS) && (iBestYield == iWorstYield))
			{
				iAdvantage *= 99;
				iAdvantage /= 100;
			}
		}
		
		if (eBestExisting == NO_BUILDING)
		{
			if (iAdvantage >= 100 || eYieldConsumed == NO_YIELD)
			{
				return true;
			}
		}
		else
		{
			if ((iAdvantage >= 100) || professionCount(eProfessionProduced) > 0)
			{
				return true;
			}
		}
	}

	return false;
}
// TAC - AI Buildings - koma13 - END

int CvCityAI::AI_buildingValue(BuildingTypes eBuilding, int iFocusFlags) const
{
	//
	bool bIsStarted = getBuildingProduction(eBuilding) > 0;
	
	CvBuildingInfo& kBuildingInfo = GC.getBuildingInfo(eBuilding);
	BuildingClassTypes eBuildingClass = (BuildingClassTypes)kBuildingInfo.getBuildingClassType();
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	
	bool bIsMilitary = false;
	
	int iValue = 0;
	
	// Custom_House_Mod Start
	bool bIsBestPortCity = isBestPortCity();
	// Custom_House_Mod End
	// TAC - AI Buildings - koma13 - START
	//bIsMajorCity = AI_isMajorCity()
	bool bIsMajorCity = AI_isMajorCity() || bIsBestPortCity;
	bool bProductionBuilding = AI_isProductionBuilding(eBuilding, bIsMajorCity);
	int iCount = kOwner.getBuildingClassCountPlusMaking(eBuildingClass);
	// TAC - AI Buildings - koma13 - END
	
	if (kBuildingInfo.getYieldStorage() != 0)
	{
		int iCityCapacity = getMaxYieldCapacity();
		
		if (isHasBuilding(eBuilding))
		{
			iCityCapacity += kBuildingInfo.getYieldStorage() * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getStoragePercent() / 100;
		}
				
		int iTotalExcess = 0;
		int iHighestPercentFull = 0;

//VET NewCapacity - begin 1/7
		if (GC.getNEW_CAPACITY())
		{
			int iExcess;
			const int iCoef = 100;
			iExcess = (iCityCapacity - getTotalYieldStored()) * iCoef;
			if (iExcess < 1)
				{iExcess = iCoef / 2;}
			iTotalExcess = (iCityCapacity * iCoef) / iExcess;
			iHighestPercentFull = std::max(iHighestPercentFull, 100 * getTotalYieldStored() / iCityCapacity);
		}
		else
		{
//VET NewCapacity - end 1/7
			for (int i = 0; i < NUM_YIELD_TYPES; ++i)
			{
				YieldTypes eLoopYield = (YieldTypes)i;

				if ((eLoopYield != YIELD_FOOD) && (eLoopYield != YIELD_LUMBER) && (eLoopYield != YIELD_STONE) && GC.getYieldInfo(eLoopYield).isCargo())
				{
					int iExcess = getYieldStored(eLoopYield) - iCityCapacity;
					if (iExcess > 0)
					{
						iTotalExcess += iExcess;
					}
					iHighestPercentFull = std::max(iHighestPercentFull, 100 * getYieldStored(eLoopYield) / iCityCapacity);
				}
			}
//VET NewCapacity - begin 2/7
		}
//VET NewCapacity - end 2/7
		int iTempValue = kBuildingInfo.getYieldStorage();
		
		iValue += iTempValue / 3;
		iValue += iHighestPercentFull;
		iValue += 10 * iTotalExcess;
		// Custom_House_Mod Start
		if (bIsBestPortCity)
		{
			iValue *= 2;
		}
		// Custom_House_Mod END
		bIsMilitary = true;
	}
	
	if (kBuildingInfo.getDefenseModifier() != 0)
	{
		bool bAtWar = GET_TEAM(getTeam()).getAnyWarPlanCount();
		int iCityDefense = getDefenseModifier();
		int iDefense = kBuildingInfo.getDefenseModifier();
		
		iValue += ((iDefense - iCityDefense) * getPopulation()) / (iCityDefense + 1);
		if (bAtWar)
		{
			if (iCityDefense <= 60)
			{
				iValue += 100;
			}
		}
		bIsMilitary = true;
	}
	
	if (kBuildingInfo.isWorksWater())
	{
		if (!isWorksWater())
		{
			for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
			{
				CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);
				if (pLoopPlot != NULL)
				{
					if (pLoopPlot->isWater())
					{
						iValue += 8 * std::max(0, pLoopPlot->getYield(YIELD_FOOD) - GC.getFOOD_CONSUMPTION_PER_POPULATION());
					}
				}
			}
		}
	}
	
	bool bIsGoodProfession = false;
	bool bIsBadProfession = false;
	// TAC - AI Buildings - koma13 - START
	bool bSpecialBuildingLimit = false;

	YieldTypes eYieldConsumed = NO_YIELD;
	YieldTypes eYieldProduced = NO_YIELD;
	// TAC - AI Buildings - koma13 - END

	for (int iI = 0; iI < GC.getNumProfessionInfos(); iI++)
	{
		ProfessionTypes eLoopProfession = (ProfessionTypes)iI;
		if (GC.getCivilizationInfo(getCivilizationType()).isValidProfession(eLoopProfession))
		{
			CvProfessionInfo& kLoopProfession = GC.getProfessionInfo(eLoopProfession);
		
			if (kLoopProfession.getSpecialBuilding() == kBuildingInfo.getSpecialBuildingType())
			{
				// TAC - AI Buildings - koma13 - START
				// R&R, ray , MYCP partially based on code of Aymerick - START
				eYieldConsumed = (YieldTypes)kLoopProfession.getYieldsConsumed(0);
				eYieldProduced = (YieldTypes) kLoopProfession.getYieldsProduced(0);
				// R&R, ray , MYCP partially based on code of Aymerick - END

				if (eYieldProduced == YIELD_CROSSES || eYieldProduced == YIELD_EDUCATION)
				{
					int iSpecialBuilding = kBuildingInfo.getSpecialBuildingType();
					CvCity * pCity = kOwner.getCity(getID());
					// R&R adjustment for Crosses Buildings - START
					// bSpecialBuildingLimit = (!pCity->isHasSpecialBuilding(iSpecialBuilding) && (kOwner.getSpecialBuildingCount(iSpecialBuilding, true) >= 2));
					int iTargetAmountOfSpecialbuilding = 2; // So this will be used for education and for crosses if only a few cities exist
					if (eYieldProduced == YIELD_CROSSES)
					{ 
						if (kOwner.getNumCities() >4) 
						{
							iTargetAmountOfSpecialbuilding = kOwner.getNumCities() / 2; // so only every second city will build church
						}
					}
					bSpecialBuildingLimit = (!pCity->isHasSpecialBuilding(iSpecialBuilding) && (kOwner.getSpecialBuildingCount(iSpecialBuilding, true) >= iTargetAmountOfSpecialbuilding));
					// R&R adjustment for Crosses Buildings - END

				}
				
				//if ((eYieldProduced != NO_YIELD) && !kOwner.AI_isYieldFinalProduct(eYieldProduced) && (eYieldProduced != YIELD_HAMMERS) || bIsMajorCity)
				if (((eYieldConsumed == NO_YIELD) && !bSpecialBuildingLimit) || bProductionBuilding || bIsBestPortCity)
				// TAC - AI Buildings - koma13 - END
				{
					int iHighestOutput = kOwner.AI_highestProfessionOutput(eLoopProfession, this);
					int iOutput = kBuildingInfo.getProfessionOutput();
					
					int iModifiedOutput = iOutput;
					if(eYieldProduced != NO_YIELD)
					{
						iModifiedOutput *= 100 + kBuildingInfo.getYieldModifier(eYieldProduced);
						iModifiedOutput /= 100;
					}
						
					if (iModifiedOutput > iHighestOutput)
					{
						if (iOutput != 0)
						{
							int iRawYieldProduced = getRawYieldProduced(eYieldProduced);
							if (iOutput > 0)
							{
								int iTempValue = AI_estimateYieldValue(eYieldProduced, iModifiedOutput);
								if (iRawYieldProduced > 0)
								{
									iTempValue *= 150 + 10 * iRawYieldProduced;
									iTempValue /= 100;
								}
								if (bIsStarted || getPopulation() >= 5)
								{
									if ((eYieldProduced == YIELD_HAMMERS) && !kOwner.AI_isStrategy(STRATEGY_CASH_FOCUS))
									{
										// R&R, ray, AI builds stronger - START
										if(isNative())
										{
											iTempValue *= 100 + 20 * (getPopulation() - 4); // old code
										}
										else
										{
											iTempValue *= 100 + 20 * getPopulation();
										}
										// R&R, ray, AI builds stronger - END
										iTempValue /= 100;
									}
								}
								
								if (eYieldConsumed != NO_YIELD)
								{
									int iAvailable = getRawYieldProduced(eYieldConsumed) + std::max(0, AI_getTradeBalance(eYieldConsumed));
									
									if (iAvailable < iOutput)
									{
										int iMax = std::max(1, GC.getGameINLINE().getCargoYieldCapacity());
										
										int iPercent = 100 * getYieldStored(eYieldConsumed) / iMax;
										iPercent = std::max(iPercent, 100 * kOwner.AI_getBestPlotYield(eYieldConsumed) / iOutput);
										
										if (iPercent > 100)
										{
											iPercent = 100 + (iPercent - 100) / 2;
										}

										iTempValue *= iPercent;
										iTempValue /= 100;
									}
								}
								if ((eYieldProduced == YIELD_HORSES))
								{
									if (kOwner.isNative())
									{
										iTempValue *= 10;
									}
									else
									{
										iTempValue /= 10;
									}
								}
								
								if (iHighestOutput == 0)
								{
									int iMultiplier = 150;
									for (int i = 0; i < getPopulation(); ++i)
									{
										CvUnit* pLoopUnit = getPopulationUnitByIndex(i);
										if (pLoopUnit->AI_getIdealProfession() == eLoopProfession)
										{
											iMultiplier += 100;
										}
									}
									iTempValue *= iMultiplier;
									iTempValue /= 100;
								}
								
								if (eYieldProduced == YIELD_HORSES || eYieldProduced == YIELD_BLADES || eYieldProduced == YIELD_MUSKETS || eYieldProduced == YIELD_CANNONS || eYieldProduced == YIELD_TOOLS || eYieldProduced == YIELD_FOOD)
								{
									bIsMilitary = true;
								}
								iValue += iTempValue;
								bIsGoodProfession = true;
							}
						}
					}
					else
					{
						bIsBadProfession = true;
					}
				}
			}
		}
	}
	
	if (bIsGoodProfession)
	{
		if (getPopulation() < 3)
		{
			iValue *= 1 + getPopulation();
			iValue /= 4;
		}
	}
	
	// TAC - AI Buildings - koma13 - START
	//if (!bIsBadProfession)
	if (!bIsBadProfession && !bSpecialBuildingLimit)
	// TAC - AI Buildings - koma13 - END
	{
		//XXX - underlying gameplay may be changed...
		for (int i = 0; i < NUM_YIELD_TYPES; ++i)
		{
			YieldTypes eLoopYield = (YieldTypes)i;
			
			int iAdded = 0;
			
			// TAC - AI Buildings - koma13 - START
			//iAdded += kBuildingInfo.getYieldChange(eLoopYield);
			//iAdded += getBuildingYieldChange((BuildingClassTypes)kBuildingInfo.getBuildingClassType(), eLoopYield);
			//iAdded += kOwner.getBuildingYieldChange((BuildingClassTypes)kBuildingInfo.getBuildingClassType(), eLoopYield);
				
			if (!GC.getYieldInfo(eLoopYield).isCargo())
			{
				iAdded += kBuildingInfo.getYieldChange(eLoopYield);
				iAdded += getBuildingYieldChange((BuildingClassTypes)kBuildingInfo.getBuildingClassType(), eLoopYield);
				iAdded += kOwner.getBuildingYieldChange((BuildingClassTypes)kBuildingInfo.getBuildingClassType(), eLoopYield);
			}
			// TAC - AI Buildings - koma13 - END
			
			if (kBuildingInfo.getYieldModifier(eLoopYield) > 0)
			{
				int iRaw = getRawYieldProduced(eLoopYield);
				if ((eLoopYield == YIELD_BELLS) && kOwner.AI_isStrategy(STRATEGY_FAST_BELLS) && (kOwner.AI_findBestCity() == this))
				{
					iRaw += std::max(iRaw, std::min(8, getPopulation()));
				}
				iAdded += ((2 * getRawYieldProduced(eLoopYield)) * kBuildingInfo.getYieldModifier(eLoopYield)) / 100;
			}
			
			if (iAdded != 0)
			{
				if (eLoopYield == YIELD_HORSES || eLoopYield == YIELD_BLADES || eLoopYield == YIELD_MUSKETS || eLoopYield == YIELD_CANNONS || eLoopYield == YIELD_TOOLS || eLoopYield == YIELD_FOOD)
				{
					bIsMilitary = true;
				}
				iValue += AI_estimateYieldValue(eLoopYield, iAdded);
			}
			
			if (kBuildingInfo.getSeaPlotYieldChange(i) != 0)
			{
				int iYieldChange = kBuildingInfo.getSeaPlotYieldChange(i);
				int iTempValue = 0;
				
				int iFood = 0;
				int iNumLandPlots = 0;
				
				for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
				{
					CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);
					if ((pLoopPlot != NULL) && (pLoopPlot->getWorkingCity() == this))
					{
						if (pLoopPlot->isWater())
						{
							iTempValue += iYieldChange;
							if (pLoopPlot->isBeingWorked())
							{
								iTempValue += iYieldChange;
							}
							if (pLoopPlot->getBonusType() != NO_BONUS)
							{
								iTempValue += iYieldChange * 3;
							}
						}
						else if (iI != CITY_HOME_PLOT)
						{
							iFood += pLoopPlot->getYield(YIELD_FOOD);
							iNumLandPlots++;
						}
					}
				}
				
				iTempValue = AI_estimateYieldValue(eLoopYield, iTempValue);
				
				if (eLoopYield == YIELD_FOOD && iTempValue > 0 && iNumLandPlots > 0)
				{
					if (iFood / iNumLandPlots < 2)
					{
						iTempValue += 10;
						iTempValue += iNumLandPlots * 4 - iFood * 2;
					}
				}
				
				iValue += iTempValue;
				bIsMilitary = true;
			}

			// R&R, ray, Landplot Yields - START
			if (kBuildingInfo.getLandPlotYieldChange(i) != 0)
			{
				int iYieldChange = kBuildingInfo.getLandPlotYieldChange(i);
				int iTempValue = 0;
				
				int iFood = 0;
				int iNumLandPlots = 0;
				
				for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
				{
					CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);
					if ((pLoopPlot != NULL) && (pLoopPlot->getWorkingCity() == this))
					{
						if (!pLoopPlot->isWater()) // here we check for Land
						{
							iTempValue += iYieldChange;
							if (pLoopPlot->isBeingWorked())
							{
								iTempValue += iYieldChange;
							}
							if (pLoopPlot->getBonusType() != NO_BONUS)
							{
								iTempValue += iYieldChange * 3;
							}
						}
					}
				}
				
				iTempValue = AI_estimateYieldValue(eLoopYield, iTempValue);
				
				if (eLoopYield == YIELD_FOOD && iTempValue > 0 && iNumLandPlots > 0)
				{
					if (iFood / iNumLandPlots < 2)
					{
						iTempValue += 10;
						iTempValue += iNumLandPlots * 4 - iFood * 2;
					}
				}
				
				iValue += iTempValue;
				bIsMilitary = true;
			}
			// R&R, ray, Landplot Yields - END
		}
	}
	
	if ((bIsMajorCity) || (iFocusFlags & BUILDINGFOCUS_MILITARY))	// TAC - AI Buildings - koma13
	{
		int iUnitsTrainedCount = 0;
		
		for (int i = 0; i < GC.getNumUnitInfos(); ++i)
		{
			if (GC.getUnitInfo((UnitTypes)i).getPrereqBuilding() == kBuildingInfo.getBuildingClassType())
			{
				iUnitsTrainedCount++;
			}
		}
		
		if (iUnitsTrainedCount > 0)
		{
			int iBuildingCount = kOwner.getBuildingClassCountPlusMaking(eBuildingClass);
			
			// TAC - AI Buildings - koma13 - START
			//int iTargetBuildingCount = 1 + kOwner.getNumCities() / 10;
			int iTargetBuildingCount = 1 + kOwner.getNumCities() / 5;
			
			//if (iBuildingCount < iTargetBuildingCount)
			if ((iBuildingCount < iTargetBuildingCount) || (kOwner.getGold() > 25000) || (iFocusFlags & BUILDINGFOCUS_MILITARY))
			{
				//iValue += 5 * calculateNetYield(YIELD_HAMMERS);
				iValue += std::max(10, 5 * calculateNetYield(YIELD_HAMMERS));
			}
			// TAC - AI Buildings - koma13 - END
			bIsMilitary = true;
		}
	}
	
	// TAC - AI Buildings - koma13 - START
	//if (bIsMajorCity && !(iFocusFlags & BUILDINGFOCUS_NO_RECURSION))
	if (!(iFocusFlags & BUILDINGFOCUS_NO_RECURSION))
	// TAC - AI Buildings - koma13 - END
	{
		for (int i = 0; i < GC.getNumBuildingInfos(); ++i)
		{
			BuildingTypes eLoopBuilding = (BuildingTypes)i;
			if (!isHasBuilding(eLoopBuilding))
			{
				CvBuildingInfo& kLoopBuilding = GC.getBuildingInfo(eLoopBuilding);
				
				if (kLoopBuilding.isBuildingClassNeededInCity(kBuildingInfo.getBuildingClassType()))
				{
					// TAC - AI Buildings - koma13 - START
					//bool bOthersNeeded = false;
					bool bOthersNeeded = ((kLoopBuilding.getAIWeight() > 0) || AI_isProductionBuilding(eLoopBuilding, bIsMajorCity));
					if (!bOthersNeeded && bIsMajorCity)
					{
						for (int j = 0; j < GC.getNumBuildingInfos(); ++j)
						{
							BuildingTypes eLoopBuilding2 = (BuildingTypes)j;
							if ((eLoopBuilding2 != eBuilding) && isHasBuilding(eLoopBuilding2))
							{
								if (kLoopBuilding.isBuildingClassNeededInCity(GC.getBuildingInfo(eLoopBuilding2).getBuildingClassType()))
								{
									bOthersNeeded = true;
									break;
								}
							}
						}
					}
					// TAC - AI Buildings - koma13 - END
					if (bOthersNeeded)
					{
						iValue += AI_buildingValue(eLoopBuilding, iFocusFlags | BUILDINGFOCUS_NO_RECURSION) / 3;
					}
				}
			}
		}
	}
	
	if ((kBuildingInfo.getSpecialBuildingType() != NO_SPECIALBUILDING) && kBuildingInfo.getYieldStorage() == 0)
	{
		if (!isHasConceptualBuilding(eBuilding))//Prevents recursion.
		{
			int iBestValue = -1;
			BuildingTypes eBestExisting = NO_BUILDING;
			for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); ++iBuildingClass)
			{
				BuildingTypes eLoopBuilding = (BuildingTypes) GC.getCivilizationInfo(GET_PLAYER(getOwnerINLINE()).getCivilizationType()).getCivilizationBuildings(iBuildingClass);
				if ((NO_BUILDING != eLoopBuilding) && (eLoopBuilding != eBuilding))
				{
					CvBuildingInfo& kLoopBuilding = GC.getBuildingInfo(eLoopBuilding);
					if (kLoopBuilding.getSpecialBuildingType() == kBuildingInfo.getSpecialBuildingType())
					{
						if (isHasConceptualBuilding(eLoopBuilding))
						{
							int iValue = kLoopBuilding.getSpecialBuildingPriority();
							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								eBestExisting = eLoopBuilding;
							}
						}
					}
				}
			}
			if (eBestExisting != NO_BUILDING)
			{
				iValue -= AI_buildingValue(eBestExisting, iFocusFlags);
			}
		}
	}
	
	//increase building value if only needs hammers
	if (!isHasConceptualBuilding(eBuilding) && (iFocusFlags & BUILDINGFOCUS_BUILD_ANYTHING))
	{
		iValue += 10;

		bool bNonHammerCost = false;
		for (int i = 0; i < NUM_YIELD_TYPES; ++i)
		{
			if ((kBuildingInfo.getYieldCost(i) > 0) && (i != YIELD_HAMMERS))
			{
				bNonHammerCost = true;
				break;
			}
		}

		if (!bNonHammerCost)
		{
			iValue += 10;
		}
	}
	
	// TAC - AI Buildings - koma13 - START
	if (!isHasConceptualBuilding(eBuilding))
	{
		iValue += kBuildingInfo.getAIWeight();
		
		if (bProductionBuilding)
		{
			iValue += 100 + 10 * getRawYieldProduced(eYieldConsumed);
		}
	}
	
	//return iValue;
	return std::max(0, iValue);
	// TAC - AI Buildings - koma13 - END
}

int CvCityAI::AI_neededSeaWorkers() const
{
	CvArea* pWaterArea;
	int iNeededSeaWorkers = 0;

	pWaterArea = waterArea();
	
	if (pWaterArea == NULL)
	{
		return 0;
	}
	
	bool bNeedRoute = false;
	
	if (bNeedRoute)
	{
		iNeededSeaWorkers++;
	}

	return iNeededSeaWorkers;
}


bool CvCityAI::AI_isDefended(int iExtra) const
{
	PROFILE_FUNC();

	return ((AI_numDefenders(true, !isNative()) + iExtra) >= AI_neededDefenders()); // XXX check for other team's units?
}


int CvCityAI::AI_neededDefenders() const
{
	PROFILE_FUNC();
	int iDefenders = 0;
	AreaAITypes eAreaAI = area()->getAreaAIType(getTeam());
	
	if (isNative())
	{
		int iNeeded = 2 + (getPopulation() + 1) / 2;
		
		if (eAreaAI == AREAAI_OFFENSIVE)
		{
			iNeeded--;
		}
		else if (eAreaAI == AREAAI_DEFENSIVE)
		{
			iNeeded++;
		}
		return iNeeded;
	}
	
	if (GET_PLAYER(getOwnerINLINE()).AI_isKing())
	{
		return 2 + getHighestPopulation() / 2;
	}

	// R&R, ray, AI improvements
	// Colonial AI should use its colonist for enconomy during first period
	int gamespeedMod = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
	int forcedPeaceTurns= GC.getDefineINT("COLONIAL_FORCED_PEACE_TURNS");
	forcedPeaceTurns = forcedPeaceTurns * gamespeedMod /100;
	if (GC.getGameINLINE().getElapsedGameTurns() < forcedPeaceTurns )	{
		return 0;
	}

	iDefenders = 2;
	
	// TAC - AI City Defense - koma13 - START
	/*
	if (GET_PLAYER(getOwnerINLINE()).AI_isStrategy(STRATEGY_REVOLUTION_PREPARING))
	{
		if (plot()->getNearestEurope() != NO_EUROPE)
		{
			iDefenders += 2;
			if (GET_PLAYER(getOwnerINLINE()).AI_isStrategy(STRATEGY_REVOLUTION_DECLARING))
			{
				iDefenders += 3;
			}
		}
	}
			
	iDefenders += getPopulation() / 2;
	*/

	if (GET_PLAYER(getOwnerINLINE()).AI_isStrategy(STRATEGY_REVOLUTION_PREPARING))
	{
		if (plot()->getNearestEurope() != NO_EUROPE)
		{
			iDefenders += 1;
			if (GET_PLAYER(getOwnerINLINE()).AI_isStrategy(STRATEGY_REVOLUTION_DECLARING))
			{
				iDefenders += 2;
			}
		}
	}
			
	if (isBestPortCity())
	{
		iDefenders++;
	}
	
	if (getPopulation() > 9)
	{
		iDefenders++;
	}
	// TAC - AI City Defense - koma13 - END

	// R&R, improvements for AI defense
	if (GET_PLAYER(getOwnerINLINE()).AI_getPlotDanger(plot(), 2, false) > 0)
	{
		iDefenders++;
	}
	return iDefenders;
}

int CvCityAI::AI_numDefenders(bool bDefenseOnly, bool bIncludePotential) const
{
	int iNum = plot()->plotCount(PUF_canDefendGroupHead, -1, -1, getOwnerINLINE(), NO_TEAM, bDefenseOnly ? PUF_isCityAIType : NULL);
	if (bIncludePotential)
	{
		iNum += AI_numPotentialDefenders();
	}
	return iNum;
}

int CvCityAI::AI_numPotentialDefenders() const
{
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	int iMaxEquipable = 0;
	for (int i = 0; i < GC.getNumProfessionInfos(); ++i)
	{
		ProfessionTypes eLoopProfession = (ProfessionTypes)i;
		CvProfessionInfo& kProfession = GC.getProfessionInfo(eLoopProfession);
		
		if (GC.getCivilizationInfo(kOwner.getCivilizationType()).isValidProfession(eLoopProfession))	// TAC - AI City Defense - koma13
		{
			if (kOwner.AI_professionValue(eLoopProfession, UNITAI_DEFENSIVE) > 0)
			{
				// TAC - AI City Defense - koma13 - START
				//int iEquipable = getPopulation();
				int iEquipable = std::max(0, getPopulation() - 1);
				// TAC - AI City Defense - koma13 - END
				if (kOwner.hasContentsYieldEquipmentAmount(eLoopProfession)) // cache CvPlayer::getYieldEquipmentAmount - Nightinggale
				{
					for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
					{
						int iAmount = kOwner.getYieldEquipmentAmount(eLoopProfession, (YieldTypes)iYield);
					
						if (iAmount > 0)
						{
							// TAC - AI Buildings - koma13 - START
							//iEquipable = std::min(iEquipable, getYieldStored((YieldTypes)iYield) / iAmount);
							int iYieldsAvailabe = std::min(getYieldStored((YieldTypes)iYield), getMaxYieldCapacity() * 60 / 100);
							iEquipable = std::min(iEquipable, iYieldsAvailabe / iAmount);
							// TAC - AI Buildings - koma13 - END
						}
					}
				}
				
				iMaxEquipable = std::max(iEquipable, iMaxEquipable);
			}
		}
	}
	
	return iMaxEquipable;
}

int CvCityAI::AI_minDefenders() const
{
	int iDefenders = 1;
	
	int iEra = GET_PLAYER(getOwnerINLINE()).getCurrentEra();
	if (iEra > 0)
	{
		iDefenders++;
	}
	if (((iEra - GC.getGame().getStartEra() / 2) >= GC.getNumEraInfos() / 2) && isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
	{
		iDefenders++;
	}
	
	return iDefenders;
}
	
int CvCityAI::AI_neededFloatingDefenders()
{
	if (m_iNeededFloatingDefendersCacheTurn != GC.getGame().getGameTurn())
	{
		AI_updateNeededFloatingDefenders();
	}
	return m_iNeededFloatingDefenders;	
}

void CvCityAI::AI_updateNeededFloatingDefenders()
{
	int iFloatingDefenders = GET_PLAYER(getOwnerINLINE()).AI_getTotalFloatingDefendersNeeded(area());
		
	int iTotalThreat = std::max(1, GET_PLAYER(getOwnerINLINE()).AI_getTotalAreaCityThreat(area()));
	
	iFloatingDefenders -= area()->getCitiesPerPlayer(getOwnerINLINE());
	
	iFloatingDefenders *= AI_cityThreat();
	iFloatingDefenders += (iTotalThreat / 2);
	iFloatingDefenders /= iTotalThreat;
	
	m_iNeededFloatingDefenders = iFloatingDefenders;
	m_iNeededFloatingDefendersCacheTurn = GC.getGame().getGameTurn();
}

bool CvCityAI::AI_isDanger() const
{
	return GET_PLAYER(getOwnerINLINE()).AI_getPlotDanger(plot(), 2, false);
}


int CvCityAI::AI_getEmphasizeAvoidGrowthCount() const
{
	return m_iEmphasizeAvoidGrowthCount;
}


bool CvCityAI::AI_isEmphasizeAvoidGrowth() const
{
	return (AI_getEmphasizeAvoidGrowthCount() > 0);
}


bool CvCityAI::AI_isAssignWorkDirty() const
{
	return m_bAssignWorkDirty;
}


void CvCityAI::AI_setAssignWorkDirty(bool bNewValue)
{
	m_bAssignWorkDirty = bNewValue;
}


bool CvCityAI::AI_isChooseProductionDirty() const
{
	return m_bChooseProductionDirty;
}


void CvCityAI::AI_setChooseProductionDirty(bool bNewValue)
{
	m_bChooseProductionDirty = bNewValue;
}


CvCity* CvCityAI::AI_getRouteToCity() const
{
	return getCity(m_routeToCity);
}


void CvCityAI::AI_updateRouteToCity()
{
	CvCity* pLoopCity;
	CvCity* pBestCity;
	int iValue;
	int iBestValue;
	int iLoop;
	int iI;

	gDLL->getFAStarIFace()->ForceReset(&GC.getRouteFinder());

	iBestValue = MAX_INT;
	pBestCity = NULL;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).getTeam() == getTeam())
		{
			for (pLoopCity = GET_PLAYER((PlayerTypes)iI).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iI).nextCity(&iLoop))
			{
				if (pLoopCity != this)
				{
					if (pLoopCity->area() == area())
					{
						if (!(gDLL->getFAStarIFace()->GeneratePath(&GC.getRouteFinder(), getX_INLINE(), getY_INLINE(), pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE(), false, getOwnerINLINE(), true)))
						{
							iValue = plotDistance(getX_INLINE(), getY_INLINE(), pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE());

							if (iValue < iBestValue)
							{
								iBestValue = iValue;
								pBestCity = pLoopCity;
							}
						}
					}
				}
			}
		}
	}

	if (pBestCity != NULL)
	{
		m_routeToCity = pBestCity->getIDInfo();
	}
	else
	{
		m_routeToCity.reset();
	}
}


int CvCityAI::AI_getEmphasizeYieldCount(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiEmphasizeYieldCount[eIndex];
}

bool CvCityAI::AI_isEmphasize(EmphasizeTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumEmphasizeInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(m_abEmphasize != NULL, "m_abEmphasize is not expected to be equal with NULL");
	return m_abEmphasize[eIndex];
}


void CvCityAI::AI_setEmphasize(EmphasizeTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumEmphasizeInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (AI_isEmphasize(eIndex) != bNewValue)
	{
		m_abEmphasize[eIndex] = bNewValue;

		if (GC.getEmphasizeInfo(eIndex).isAvoidGrowth())
		{
			m_iEmphasizeAvoidGrowthCount += ((AI_isEmphasize(eIndex)) ? 1 : -1);
			FAssert(AI_getEmphasizeAvoidGrowthCount() >= 0);
		}

		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			int iYieldChange = GC.getEmphasizeInfo(eIndex).getYieldChange(iI);
			if (iYieldChange != 0)
			{
				m_aiEmphasizeYieldCount[iI] += ((AI_isEmphasize(eIndex)) ? iYieldChange : -iYieldChange);
			}
		}

		AI_assignWorkingPlots();

		if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(Domestic_Advisor_DIRTY_BIT, true);
		}
	}
}

void CvCityAI::AI_forceEmphasizeCulture(bool bNewValue)
{
	if (m_bForceEmphasizeCulture != bNewValue)
	{
		m_bForceEmphasizeCulture = bNewValue;

		m_aiEmphasizeYieldCount[YIELD_CROSSES] += (bNewValue ? 1 : -1);
		FAssert(m_aiEmphasizeYieldCount[YIELD_CROSSES] >= 0);
	}
}


int CvCityAI::AI_getBestBuildValue(int iIndex) const
{
	FAssertMsg(iIndex >= 0, "iIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(iIndex < NUM_CITY_PLOTS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiBestBuildValue[iIndex];
}


int CvCityAI::AI_totalBestBuildValue(CvArea* pArea) const
{
	CvPlot* pLoopPlot;
	int iTotalValue;
	int iI;

	iTotalValue = 0;

	for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		if (iI != CITY_HOME_PLOT)
		{
			pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);

			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->area() == pArea)
				{
					if ((pLoopPlot->getImprovementType() == NO_IMPROVEMENT) || !(GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_SAFE_AUTOMATION) && !(pLoopPlot->getImprovementType() == (GC.getDefineINT("RUINS_IMPROVEMENT")))))
					{
						iTotalValue += AI_getBestBuildValue(iI);
					}
				}
			}
		}
	}

	return iTotalValue;
}

int CvCityAI::AI_clearFeatureValue(int iIndex) const
{
	CvPlot* pPlot = plotCity(getX_INLINE(), getY_INLINE(), iIndex);
	FAssert(pPlot != NULL);
	
	FeatureTypes eFeature = pPlot->getFeatureType();
	FAssert(eFeature != NO_FEATURE);
	
	CvFeatureInfo& kFeatureInfo = GC.getFeatureInfo(eFeature);
	
	int iValue = 0;
	iValue += kFeatureInfo.getYieldChange(YIELD_FOOD) * 100;
	
	if (iValue > 0 && pPlot->isBeingWorked())
	{
		iValue *= 3;
		iValue /= 2;
	}
	if (iValue != 0)
	{
		BonusTypes eBonus = pPlot->getBonusType();
		if (eBonus != NO_BONUS)
		{
			iValue *= 3;
		}
	}
		
	if (iValue > 0)
	{
		if (pPlot->getImprovementType() != NO_IMPROVEMENT)
		{
			if (GC.getImprovementInfo(pPlot->getImprovementType()).isRequiresFeature())
			{
				iValue += 500;
			}
		}
		
		if (GET_PLAYER(getOwnerINLINE()).getAdvancedStartPoints() >= 0)
		{
			iValue += 400;
		}
	}
	
	return -iValue;
}
	
BuildTypes CvCityAI::AI_getBestBuild(int iIndex) const
{
	FAssertMsg(iIndex >= 0, "iIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(iIndex < NUM_CITY_PLOTS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aeBestBuild[iIndex];
}


int CvCityAI::AI_countBestBuilds(CvArea* pArea) const
{
	CvPlot* pLoopPlot;
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		if (iI != CITY_HOME_PLOT)
		{
			pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);

			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->area() == pArea)
				{
					if (AI_getBestBuild(iI) != NO_BUILD)
					{
						iCount++;
					}
				}
			}
		}
	}

	return iCount;
}


// Improved worker AI provided by Blake - thank you!
void CvCityAI::AI_updateBestBuild()
{
	PROFILE_FUNC();
	
	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		m_aiBestBuildValue[iI] = 0;
		m_aeBestBuild[iI] = NO_BUILD;

		if (iI != CITY_HOME_PLOT)
		{
			CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);

			if (NULL != pLoopPlot && pLoopPlot->getWorkingCity() == this)
			{
				AI_bestPlotBuild(pLoopPlot, &(m_aiBestBuildValue[iI]), &(m_aeBestBuild[iI]));
				
				if (m_aiBestBuildValue[iI] > 0)
				{
					FAssert(m_aeBestBuild[iI] != NO_BUILD);
				}
				if (m_aeBestBuild[iI] != NO_BUILD)
				{
					FAssert(m_aiBestBuildValue[iI] > 0);
				}
			}
		}
	}
}

// Protected Functions...

void CvCityAI::AI_doHurry(bool bForce)
{
	PROFILE_FUNC();
	FAssert(!isHuman() || isProductionAutomated());
	
	
	if (getProduction() == 0)
	{
		return;
	}

	HurryTypes eGoldHurry = NO_HURRY;
	for (int i = 0; i < GC.getNumHurryInfos(); ++i)
	{
		if (GC.getHurryInfo((HurryTypes)i).getGoldPerProduction() > 0)
		{
			eGoldHurry = (HurryTypes)i;
			break;
		}
	}
	
	int iHurryValue = 0;
	
	
	if (getProduction() >= getProductionNeeded(YIELD_HAMMERS))
	{
		iHurryValue += 100;	
	}
	
	bool bCritical = false;
	
	if (getProductionUnit() != NO_UNIT)
	{
		// Transport units are important so give them high priority
		if (getProductionUnitAI() == UNITAI_WAGON)
		{
			if (area()->getNumAIUnits(getOwnerINLINE(), UNITAI_WAGON) == 0)
			{
				iHurryValue += 100;
			}
		}
		if (getProductionUnitAI() == UNITAI_TRANSPORT_COAST)
		{
			CvArea* const pWaterArea = waterArea();
			
			if (pWaterArea != NULL)
			{ 
				if (GET_PLAYER(getOwnerINLINE()).AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_TRANSPORT_COAST) == 0)
				{
					iHurryValue += 100;
				}
			}
		}

		// TAC - AI purchases military units - koma13 - START
		iHurryValue += 100;

		if (AI_isDanger())
		{
			if (getProductionUnitAI() == UNITAI_DEFENSIVE)
			{
				bCritical = true;
			}
		}
		// TAC - AI purchases military units - koma13 - END
	}
	else if (getProductionBuilding() != NO_BUILDING)
	{
		iHurryValue += AI_buildingValue(getProductionBuilding());
		if (getDefenseModifier() == 0)
		{
			iHurryValue += GC.getBuildingInfo(getProductionBuilding()).getDefenseModifier() * 2;
			if (AI_isDanger())
			{
				bCritical = true;
			}
		}
	}
	
	int iThreshold = 50;
	if (getPopulation() > 3)
	{
		iThreshold -= 3 * (getPopulation() - 2);
		iThreshold = std::max(20, iThreshold);
	}
	
	bool bAffordable = GET_PLAYER(getOwnerINLINE()).AI_getHurrySpending() < GET_PLAYER(getOwnerINLINE()).AI_getTotalIncome() / 2;
	if (getHurryYieldDeficit(eGoldHurry, YIELD_LUMBER) == 0)
	{
		iHurryValue += 25;
		if (bAffordable)
		{
			iHurryValue += 25;
		}
	}
	
	// TAC - AI Buildings - koma13 - START
	//if (!bCritical && (iHurryValue < iThreshold))
	if (!bCritical && (iHurryValue < iThreshold) && (GET_PLAYER(getOwnerINLINE()).getGold() <= 25000))
	// TAC - AI Buildings - koma13 - END
	{
		if (getPopulation() < 4)
		{
			return;
		}
		
		if (GC.getGameINLINE().getSorenRandNum(100, "AI Hurry") > 25)
		{
			return;
		}
		
		if (bAffordable)
		{
			return;
		}
	}
	

	for (int i = 0; i < GC.getNumHurryInfos(); ++i)
	{
		if (canHurry((HurryTypes)i))
		{
			hurry((HurryTypes)i);
			return;
		}
	}
	
	return;
}

void CvCityAI::AI_doNativeTrade()
{
	//Each turn a random yield (weighted by quantity) will be 
	//instantly delivered to a random city (weighted inversely by quantity)
	
	int iBestYieldValue = 0;
	YieldTypes eBestYield = NO_YIELD;

	
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		YieldTypes eYield = (YieldTypes)iI;
		
		// TAC - AI Native Hammer Bugfix - START
		//if (GC.getYieldInfo(eYield).getNativeSellPrice() == -1)
		if ((GC.getYieldInfo(eYield).getNativeSellPrice() == -1) && (eYield != YIELD_HAMMERS))
		// TAC - AI Native Hammer Bugfix - END
		{
			int iValue = getYieldStored(eYield);
			if (iValue > 0)
			{
				int iProduced = getRawYieldProduced(eYield);
				if (iValue > AI_getRequiredYieldLevel(eYield) || iProduced > 0)
				{
					if (eYield == YIELD_FOOD)
					{
						iValue /= 5;
					}
					else if ((eYield == YIELD_HORSES) || (eYield == YIELD_MUSKETS))
					{
						iValue *= 2;
					}
					else if (eYield == YIELD_LUMBER)
					{
						iValue *= 2;
					}
					
					iValue = 1 + GC.getGameINLINE().getSorenRandNum(iValue, "AI best Yield to Trade");
					if (iValue > iBestYieldValue)
					{
						iBestYieldValue = iValue;
						eBestYield = eYield;
					}
				}
			}
		}
	}
	
	if (eBestYield == NO_YIELD)
	{
		return;
	}
	
	CvCity* pBestCity = NULL;
	int iBestCityValue = 0;
	
	CvYieldInfo& kBestYield = GC.getYieldInfo(eBestYield);
	CvPlayer& kOwner = GET_PLAYER(getOwner());
	int iLoop;
	CvCity* pLoopCity;
	for (pLoopCity = kOwner.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kOwner.nextCity(&iLoop))
	{
		if (pLoopCity != this)
		{
		
			int iValue = 10 * std::max(0, pLoopCity->AI_getRequiredYieldLevel(eBestYield) - pLoopCity->getYieldStored(eBestYield));
//VET NewCapacity - begin 3/7
			//iValue = std::max(iValue, pLoopCity->getMaxYieldCapacity() - pLoopCity->getYieldStored(eBestYield));
			if (GC.getNEW_CAPACITY())
				{iValue = std::max(iValue, pLoopCity->getMaxYieldCapacity() - pLoopCity->getTotalYieldStored());}
			else
				{iValue = std::max(iValue, pLoopCity->getMaxYieldCapacity() - pLoopCity->getYieldStored(eBestYield));}
//VET NewCapacity - end 3/7
			if (eBestYield == YIELD_HORSES)
			{
				iValue *= 3 + pLoopCity->foodDifference();
				iValue /= 3;
			}
			if (iValue > 0)
			{
				int iYieldNeeded = pLoopCity->AI_getRequiredYieldLevel(eBestYield) - pLoopCity->getYieldStored(eBestYield);
				if (iYieldNeeded > 0)
				{
					iValue *= 5;
					if (pLoopCity->getYieldStored(eBestYield) > 0)
					{
						//HUGELY bias in favor of nearly-full cities
						iValue *= (88 / iYieldNeeded);
					}
				}
				int iDistance = plotDistance(getX_INLINE(), getY_INLINE(), pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE());
				
				iValue *= 50 + GC.getGameINLINE().getSorenRandNum(50, "AI best city to trade yield to");
				iValue /= 5 + iDistance;	
				
				
				if (iValue > iBestCityValue)
				{
					iBestCityValue = iValue;
					pBestCity = pLoopCity;
				}
			}
		}
	}
	
	if (pBestCity == NULL)
	{
		return;
	}
	int iChange = 0;
	if (pBestCity->AI_getRequiredYieldLevel(eBestYield) > pBestCity->getYieldStored(eBestYield))
	{
		iChange = pBestCity->AI_getRequiredYieldLevel(eBestYield) - pBestCity->getYieldStored(eBestYield);
		iChange = std::min(iChange, getYieldStored(eBestYield));
	}
	else
	{
		iChange = getYieldStored(eBestYield) - AI_getRequiredYieldLevel(eBestYield);
		if (AI_getRequiredYieldLevel(eBestYield) == 0)
		{
			iChange /= 2;
		}
	}

	changeYieldStored(eBestYield, -iChange);
	pBestCity->changeYieldStored(eBestYield, iChange);
}

void CvCityAI::AI_doNative()
{
	AI_doNativeTrade();
	
	FAssert(isNative());
	CvPlayer& kPlayer = GET_PLAYER(getOwner());
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		YieldTypes eYield = (YieldTypes)iI;
		if ((eYield != YIELD_FOOD) && (eYield != YIELD_HORSES) && (eYield != YIELD_MUSKETS))
		{
			int iTotalStored = kPlayer.countTotalYieldStored(eYield);
			int iMaxStored = kPlayer.getNumCities() * GC.getGameINLINE().getCargoYieldCapacity();
			iMaxStored *= GC.getYieldInfo(eYield).getNativeConsumptionPercent();
			
			int iDestructionModifier = 50 + ((50 * iTotalStored) / iMaxStored);

			int iAmountLost = 0;
			int iPercent = 3;
			iPercent += GC.getGame().getSorenRandNum(8, "AI doNative destroy yield percent");

			iAmountLost += ((getYieldStored(eYield) * iPercent) + 50) / 100;

	
			if (GC.getGame().getSorenRandNum(100, "AI doNative destroy more yield") < (iDestructionModifier / 3))
			{
				iAmountLost += std::min(getYieldStored(eYield), getPopulation());
			}

			iAmountLost = std::min(iAmountLost, getYieldStored(eYield));

			if (iAmountLost > 0)
			{
				changeYieldStored(eYield, -iAmountLost);
				int iSellPrice = GC.getYieldInfo(eYield).getNativeSellPrice();
				if (iSellPrice > 0)
				{
					kPlayer.changeGold(((iAmountLost * iSellPrice) * 4) / 100);
				}
			}
		}
	}
//	
//	AreaAITypes eAreaAI = area()->getAreaAIType(getTeam());
//
//	if (eAreaAI != AREAAI_NEUTRAL)
//	{
//		int iLowestPopulation = getHighestPopulation();
//
//		if (eAreaAI == AREAAI_OFFENSIVE)
//		{
//			iLowestPopulation *= 49;
//		}
//		else if (eAreaAI == AREAAI_BALANCED)
//		{
//			iLowestPopulation *= 85;
//		}
//		else if (eAreaAI == AREAAI_DEFENSIVE)
//		{
//			iLowestPopulation *= 64;
//		}
//		else
//		{
//			iLowestPopulation *= 100;
//		}
//		iLowestPopulation /= 100;
//		
//		iLowestPopulation = std::max(iLowestPopulation, AI_getTargetSize() - ((eAreaAI == AREAAI_OFFENSIVE) ? 1 : 0));
//		
//		if (getPopulation() > iLowestPopulation)
//		{
//			ProfessionTypes eBraveProfession = GET_PLAYER(getOwnerINLINE()).AI_idealProfessionForUnitAIType(UNITAI_DEFENSIVE, this);
//			if (eBraveProfession != NO_PROFESSION)
//			{
//				for (int i = 0; i < getPopulation(); ++i)
//				{
//					CvUnit* pUnit = getPopulationUnitByIndex(i);
//					if (pUnit != NULL)
//					{
//						if (pUnit->canHaveProfession(eBraveProfession, false))
//						{
//							alterUnitProfession(pUnit->getID(), eBraveProfession);
//							break;
//						}
//					}
//				}
//			}
//		}
//	}
	
}

void CvCityAI::AI_resetTradedYields()
{
	for (int i = 0; i < NUM_YIELD_TYPES; i++)
	{
		YieldTypes eLoopYield = (YieldTypes)i;
		m_aiTradeBalance[eLoopYield] = 0;
	}
}

//This should only be called once per turn.
void CvCityAI::AI_doTradedYields()
{
	
	int iDiscountPercent = 100 - 100 / YIELD_DISCOUNT_TURNS;
	iDiscountPercent -= 2;
	
	for (int i = 0; i < NUM_YIELD_TYPES; i++)
	{
		YieldTypes eLoopYield = (YieldTypes)i;
		
		if (GC.getYieldInfo(eLoopYield).isCargo())
		{
			m_aiTradeBalance[eLoopYield] *= iDiscountPercent;
			m_aiTradeBalance[eLoopYield] /= 100;
		}
	}
}

// Improved use of emphasize by Blake, to go with his whipping strategy - thank you!
void CvCityAI::AI_doEmphasize()
	{
	PROFILE_FUNC();

	FAssert(!isHuman());

	for (int iI = 0; iI < GC.getNumEmphasizeInfos(); iI++)
	{
		AI_setEmphasize(((EmphasizeTypes)iI), false);
	}
}

// Erik: Split this into two functions, one that finds the best build and another that queues the build
bool CvCityAI::AI_chooseBuild()
{
	//These are now directly comparable.
	int iBestValue = 0;
	BuildingTypes eBestBuilding = NO_BUILDING;
	UnitTypes eBestUnit = NO_UNIT;
	int iFocusFlags = 0;
	
	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());
	for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		BuildingTypes eLoopBuilding = ((BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(iI)));

		if ((eLoopBuilding != NO_BUILDING) && (!isHasConceptualBuilding(eLoopBuilding)))
		{
			if (canConstruct(eLoopBuilding))
			{
				int iValue = AI_buildingValue(eLoopBuilding, iFocusFlags);

				if (iValue > 0)
				{
					int iTurnsLeft = getProductionTurnsLeft(eLoopBuilding, 0);


					iValue *= (GC.getGameINLINE().getSorenRandNum(25, "AI Best Building") + 100);
					iValue /= 100;

					iValue += getBuildingProduction(eLoopBuilding);
					

					FAssert((MAX_INT / 1000) > iValue);
					iValue *= 1000;
					iValue /= std::max(1, (iTurnsLeft + 3));

					iValue = std::max(1, iValue);

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						eBestBuilding = eLoopBuilding;
					}
				}
			}
		}
	}
	
	for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		UnitTypes eLoopUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI)));

		if (eLoopUnit != NO_UNIT)
		{
			if (canTrain(eLoopUnit))
			{
				UnitAITypes eUnitAI = NO_UNITAI;
				int iValue = GET_PLAYER(getOwnerINLINE()).AI_unitEconomicValue(eLoopUnit, &eUnitAI, this);

				iValue *= (GC.getGameINLINE().getSorenRandNum(25, "AI Best Unit") + 100);
				iValue /= 100;

//				iValue *= (GET_PLAYER(getOwnerINLINE()).getNumCities() * 2);
//				iValue /= (GET_PLAYER(getOwnerINLINE()).getUnitClassCountPlusMaking((UnitClassTypes)iI) + GET_PLAYER(getOwnerINLINE()).getNumCities() + 1);

				FAssert((MAX_INT / 1000) > iValue);
				iValue *= 1000;
		
				iValue /= std::max(1, (4 + getProductionTurnsLeft(eLoopUnit, 0)));

				iValue = std::max(1, iValue);

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					eBestUnit = eLoopUnit;
					eBestBuilding = NO_BUILDING;
				}
			}
		}
	}	
	
	if (eBestBuilding != NULL)
	{
		FAssert(eBestUnit == NULL);
		pushOrder(ORDER_TRAIN, eBestUnit, NO_UNITAI, false, false, false);
		
	}
	else if (eBestUnit != NULL)
	{
		FAssert(eBestBuilding == NULL);
		pushOrder(ORDER_CONSTRUCT, eBestBuilding, -1, false, false, false);
		
	}

	return false;
	
}

bool CvCityAI::AI_chooseUnit(UnitAITypes eUnitAI, bool bPickAny)
{
	UnitTypes eBestUnit;

	if (eUnitAI != NO_UNITAI)
	{
		eBestUnit = AI_bestUnitAI(eUnitAI);
	}
	else
	{
		eBestUnit = AI_bestUnit(false, &eUnitAI, bPickAny);
	}

	if (eBestUnit != NO_UNIT)
	{
		pushOrder(ORDER_TRAIN, eBestUnit, eUnitAI, false, false, false);
		return true;
	}

	return false;
}

bool CvCityAI::AI_chooseUnit(UnitTypes eUnit, UnitAITypes eUnitAI)
{
	if (eUnit != NO_UNIT)
	{
		
		pushOrder(ORDER_TRAIN, eUnit, eUnitAI, false, false, false);
		return true;
	}
	return false;	
}


bool CvCityAI::AI_chooseDefender()
{

	if (plot()->plotCheck(PUF_isUnitAIType, UNITAI_DEFENSIVE, -1, getOwnerINLINE()) == NULL)
	{
		if (AI_chooseUnit(UNITAI_DEFENSIVE))
		{
			return true;
		}
	}

	if (AI_chooseUnit(UNITAI_COUNTER))
	{
		return true;
	}

	return false;
}

bool CvCityAI::AI_chooseLeastRepresentedUnit(UnitTypeWeightArray &allowedTypes)
{
	int iValue;

	UnitTypeWeightArray::iterator it;
	
 	std::multimap<int, UnitAITypes, std::greater<int> > bestTypes;
 	std::multimap<int, UnitAITypes, std::greater<int> >::iterator best_it;
 	
 	
	for (it = allowedTypes.begin(); it != allowedTypes.end(); ++it)
	{
		iValue = (*it).second;
		iValue *= 750 + GC.getGameINLINE().getSorenRandNum(250, "AI choose least represented unit");
		iValue /= 1 + GET_PLAYER(getOwnerINLINE()).AI_totalAreaUnitAIs(area(), (*it).first);
		bestTypes.insert(std::make_pair(iValue, (*it).first));
	}
 	
 	for (best_it = bestTypes.begin(); best_it != bestTypes.end(); ++best_it)
 	{
		if (AI_chooseUnit(best_it->second))
		{
			return true;
		}
 	}
	return false;
}

bool CvCityAI::AI_bestSpreadUnit(bool bMissionary, int iBaseChance, UnitTypes* eBestSpreadUnit, int* iBestSpreadUnitValue) const
{
	CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());
	CvTeamAI& kTeam = GET_TEAM(getTeam());
	CvGame& kGame = GC.getGame();
	
	FAssert(eBestSpreadUnit != NULL && iBestSpreadUnitValue != NULL);
	
	return (*eBestSpreadUnit != NULL);
}

bool CvCityAI::AI_chooseBuilding(int iFocusFlags, int iMaxTurns, int iMinThreshold)
{
	BuildingTypes eBestBuilding;

	eBestBuilding = AI_bestBuildingThreshold(iFocusFlags, iMaxTurns, iMinThreshold);

	if (eBestBuilding != NO_BUILDING)
	{
		pushOrder(ORDER_CONSTRUCT, eBestBuilding, -1, false, false, false);
		return true;
	}

	return false;
}

//TAC - AI produces FF Points - koma13 - START
bool CvCityAI::AI_chooseConvince()
{
	if (GET_PLAYER(getOwnerINLINE()).getParent() == NO_PLAYER)
	{
		return false;
	}

	FatherPointTypes eBestFatherPoint;

	eBestFatherPoint = AI_bestFatherPoint();

	if (eBestFatherPoint != NO_FATHER_POINT_TYPE)
	{
		if (canConvince(eBestFatherPoint))
		{
			pushOrder(ORDER_CONVINCE,  eBestFatherPoint, -1 ,false, false, false);
			return true;
		}
	}
	
	return false;
}

FatherPointTypes CvCityAI::AI_bestFatherPoint() const
{
	CvTeam& kTeam = GET_TEAM(getTeam());
	
	FatherPointTypes eBestTotalPoint = NO_FATHER_POINT_TYPE;
	
	int iBestTotalValue = MAX_INT;
	
	for (int iFather = 0; iFather < GC.getNumFatherInfos(); ++iFather)
	{
		FatherTypes eFather = (FatherTypes) iFather;
		if ((eFather != NO_FATHER) && (GC.getGameINLINE().getFatherTeam(eFather) == NO_TEAM))
		{
			if (!kTeam.isFatherIgnore(eFather))
			{
				FatherPointTypes eBestPoint = NO_FATHER_POINT_TYPE;
				int iBestValue = 0;
				int iTotalValue = 0;
				for (int iI = 0; iI < GC.getNumFatherPointInfos(); iI++)
				{
					FatherPointTypes ePoint = (FatherPointTypes) iI;
					if ((ePoint != NO_FATHER_POINT_TYPE) && (canConvince(ePoint)))
					{
						if (kTeam.getFatherPointCost(eFather, ePoint) > 0)
						{
							int iValue = kTeam.getFatherPointCost(eFather, ePoint) - kTeam.getFatherPoints(ePoint);
							
							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								eBestPoint = ePoint;
							}
							iTotalValue += std::max(0, iValue);
						}
					}
				}
				if ((iTotalValue > 0) && (iTotalValue < iBestTotalValue))
				{
					iBestTotalValue = iTotalValue;
					eBestTotalPoint = eBestPoint;
				}
			}
		}
	}
	
	return eBestTotalPoint;
}
//TAC - AI produces FF Points - koma13 - END

// Returns true if a worker was added to a plot...
bool CvCityAI::AI_addBestCitizen()
{
	PROFILE_FUNC();

	bool bAvoidGrowth = AI_avoidGrowth();
	bool bIgnoreGrowth = AI_ignoreGrowth();

	int iUnitId = getNextFreeUnitId();
	CvUnit* pUnit = getPopulationUnitById(iUnitId);
	ProfessionTypes eCurrentProfession = pUnit->getProfession();
	if ((NO_PROFESSION == eCurrentProfession) || !GC.getProfessionInfo(eCurrentProfession).isWorkPlot() || (GC.getProfessionInfo(eCurrentProfession).isWorkPlot()))
	{
		int iBestValue = 0;
		int iBestPlot = -1;
		ProfessionTypes eBestProfession = NO_PROFESSION;
		
		for (int i=0;i<GC.getNumProfessionInfos();i++)
		{
			ProfessionTypes eLoopProfession = (ProfessionTypes) i;
			if (GC.getCivilizationInfo(getCivilizationType()).isValidProfession(eLoopProfession))
			{
				if (GC.getProfessionInfo(eLoopProfession).isCitizen())
				{
					if (pUnit->canHaveProfession(eLoopProfession, false))
					{
						if (GC.getProfessionInfo(eLoopProfession).isWorkPlot())
						{
							for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
							{
								if (iI != CITY_HOME_PLOT)
								{
									CvPlot* pLoopPlot = getCityIndexPlot(iI);

									if (pLoopPlot != NULL)
									{
										if (!isUnitWorkingPlot(iI) || (getUnitWorkingPlot(iI) == pUnit))
										{
											if (canWork(pLoopPlot))
											{
												int iValue = AI_professionValue(eLoopProfession, pUnit, pLoopPlot, NULL);
												
												if (iValue > iBestValue)
												{
													eBestProfession = eLoopProfession;
													iBestValue = iValue;
													iBestPlot = iI;
												}
											}
										}
									}
								}
							}
						}
						else
						{
							int iValue = AI_professionValue(eLoopProfession, pUnit, NULL, NULL);
							if (iValue > iBestValue)
							{
								eBestProfession = eLoopProfession;
								iBestValue = iValue;
								iBestPlot = -1;
							}
						}
					}
				}
			}
		}

		pUnit->setProfession(eBestProfession);
		eCurrentProfession = pUnit->getProfession();
		if(eCurrentProfession == NO_PROFESSION)
		{
			//FAssertMsg(false, "Could not assign citizen any profession.");
			return false;
		}
		
		if (iBestPlot != -1)
		{
			FAssert(GC.getProfessionInfo(eCurrentProfession).isWorkPlot());
			if (getUnitWorkingPlot(iBestPlot) != pUnit)
			{
				setUnitWorkingPlot(iBestPlot, iUnitId);
			}
		}
		return true;
	}

	//already assigned to valid indoor profession
	if(!GC.getProfessionInfo(eCurrentProfession).isWorkPlot())
	{
		return true;
	}

	int iBestPlot = AI_bestProfessionPlot(eCurrentProfession, pUnit);
	
	if (iBestPlot != -1)
	{
		if (getUnitWorkingPlot(iBestPlot) != pUnit)
		{
			setUnitWorkingPlot(iBestPlot, iUnitId);
		}
		return true;
	}

	return false;
}


// Returns true if a worker was removed from a plot...
bool CvCityAI::AI_removeWorstCitizen()
{
	CvPlot* pLoopPlot;
	bool bAvoidGrowth;
	bool bIgnoreGrowth;
	int iWorstPlot;
	int iValue;
	int iWorstValue;
	int iI;
	
	bAvoidGrowth = AI_avoidGrowth();
	bIgnoreGrowth = AI_ignoreGrowth();

	iWorstValue = MAX_INT;
	iWorstPlot = -1;

	// check all the plots we working
	for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		if (iI != CITY_HOME_PLOT)
		{
			if (isUnitWorkingPlot(iI))
			{
				pLoopPlot = getCityIndexPlot(iI);

				if (pLoopPlot != NULL)
				{
					CvUnit* pUnit = getUnitWorkingPlot(iI);
					if (pUnit == NULL)
					{
						FAssert(false);
					}
					iValue = AI_professionValue(pUnit->getProfession(), pUnit, pLoopPlot, NULL);

					if (iValue < iWorstValue)
					{
						iWorstValue = iValue;
						iWorstPlot = iI;
					}
				}
			}
		}
	}

	if (iWorstPlot != -1)
	{
		clearUnitWorkingPlot(iWorstPlot);
		return true;
	}
	
	return false;
}

bool CvCityAI::AI_removeWorstPopulationUnit(bool bDelete)
{
	for (int i = (int) m_aPopulationUnits.size() - 1; i >= 0; --i)
	{
		ProfessionTypes eEjectProfession = (ProfessionTypes) GC.getCivilizationInfo(getCivilizationType()).getDefaultProfession();
		if (m_aPopulationUnits[i]->canHaveProfession(eEjectProfession, false))
		{
			if (removePopulationUnit(m_aPopulationUnits[i], bDelete, eEjectProfession))
			{
				return true;
			}
		}
	}

	return false;
}

CvUnit* CvCityAI::AI_bestPopulationUnit(UnitAITypes eUnitAI, ProfessionTypes eProfession)
{
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	if (eProfession == NO_PROFESSION)
	{
		eProfession = kOwner.AI_idealProfessionForUnitAIType(eUnitAI, this);
	}

	FAssert(eProfession != NO_PROFESSION);
	if (eProfession == NO_PROFESSION)
	{
		return NULL;
	}

	FAssert(!GC.getProfessionInfo(eProfession).isCitizen());
	
	int iBestValue = 0;
	CvUnit* pBestUnit = NULL;
	for (uint i = 0; i < m_aPopulationUnits.size(); ++i)
	{
		CvUnit* pLoopUnit = getPopulationUnitByIndex(i);
		if (pLoopUnit->canHaveProfession(eProfession, false))
		{
			int iValue = kOwner.AI_professionSuitability(pLoopUnit, eProfession, plot(), eUnitAI);
			
			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestUnit = pLoopUnit;
			}
		}
	}
	if (pBestUnit != NULL)
	{
		removePopulationUnit(pBestUnit, false, eProfession);
		pBestUnit->AI_setUnitAIType(eUnitAI);
	}
	
	return pBestUnit;
}

void CvCityAI::AI_juggleCitizens()
{
	return; //do not juggle citizens...
	
	bool bAvoidGrowth = AI_avoidGrowth();
	bool bIgnoreGrowth = AI_ignoreGrowth();
	
	// one at a time, remove the worst citizen, then add the best citizen
	// until we add back the same one we removed
	for (int iPass = 0; iPass < 2; iPass++)
	{
		bool bCompletedChecks = false;
		int iCount = 0;

		std::vector<int> aWorstPlots;

		while (!bCompletedChecks)
		{
			int iLowestValue = MAX_INT;
			int iWorstPlot = -1;
			int iValue;

			for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
			{
				if (iI != CITY_HOME_PLOT)
				{
					if (isUnitWorkingPlot(iI))
					{
						CvPlot* pLoopPlot = getCityIndexPlot(iI);

						if (pLoopPlot != NULL)
						{
								iValue = AI_plotValue(pLoopPlot, bAvoidGrowth, /*bRemove*/ true, /*bIgnoreFood*/ false, bIgnoreGrowth, (iPass == 0));

								// use <= so that we pick the last one that is lowest, to avoid infinite loop with AI_addBestCitizen
								if (iValue <= iLowestValue)
								{
									iLowestValue = iValue;
									iWorstPlot = iI;
								}
							}
						}
					}
				}

			// if no worst plot, or we looped back around and are trying to remove the first plot we removed, stop
			if (iWorstPlot == -1 || std::find(aWorstPlots.begin(), aWorstPlots.end(), iWorstPlot) != aWorstPlots.end())
			{
				bCompletedChecks = true;
			}
			else
			{
				// if this the first worst plot, remember it
				aWorstPlots.push_back(iWorstPlot);

				clearUnitWorkingPlot(iWorstPlot);

				if (AI_addBestCitizen())
				{
					if (isUnitWorkingPlot(iWorstPlot))
					{
						bCompletedChecks = true;
					}
				}
			}

			iCount++;
			if (iCount > (NUM_CITY_PLOTS + 1))
			{
				FAssertMsg(false, "infinite loop");
				break; // XXX
			}
		}

		if ((iPass == 0) && (foodDifference() >= 0))
		{
			//good enough, the starvation code
			break;
		}
	}
}

//Returns the displaced unit, if any.
CvUnit* CvCityAI::AI_assignToBestJob(CvUnit* pUnit, bool bIndoorOnly)
{
	int iBestValue = 0;
	int iBestPlot = -1;
	ProfessionTypes eBestProfession = NO_PROFESSION;
	
	for (int i=0;i<GC.getNumProfessionInfos();i++)
	{
		ProfessionTypes eLoopProfession = (ProfessionTypes) i;
		if (GC.getCivilizationInfo(getCivilizationType()).isValidProfession(eLoopProfession))
		{
			if (GC.getProfessionInfo(eLoopProfession).isCitizen())
			{
				if (pUnit->canHaveProfession(eLoopProfession, true))
				{
					if (GC.getProfessionInfo(eLoopProfession).isWorkPlot())
					{
						if (!bIndoorOnly)
						{
							for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
							{
								if (iI != CITY_HOME_PLOT)
								{
									CvPlot* pLoopPlot = getCityIndexPlot(iI);

									if (pLoopPlot != NULL)
									{
										if (canWork(pLoopPlot))
										{
											int iValue = AI_professionValue(eLoopProfession, pUnit, pLoopPlot, NULL);
											//if (iValue > iBestValue)	// TAC - AI Economy - koma13
											{
												bool bValid = true;
												
												if (pLoopPlot->isBeingWorked())
												{
													CvUnit* pWorkingUnit = getUnitWorkingPlot(iI);
													FAssert(pWorkingUnit != pUnit);
													// TAC - AI Economy - koma13 - START
													if (pWorkingUnit != NULL && !pWorkingUnit->isColonistLocked())
													{
														iValue = AI_professionValue(eLoopProfession, pUnit, pLoopPlot, pWorkingUnit);
													}
													// TAC - AI Economy - koma13 - END
													if ((pWorkingUnit->isColonistLocked() || (iValue <= AI_professionValue(pWorkingUnit->getProfession(), pWorkingUnit, pLoopPlot, NULL))))
													{
														bValid = false;
													}
												}
												// TAC - AI Economy - koma13 - START
												//if (bValid)
												if (bValid && (iValue > iBestValue))
												// TAC - AI Economy - koma13 - END
												{
													eBestProfession = eLoopProfession;
													iBestValue = iValue;
													iBestPlot = iI;
												}
											}
										}
									}
								}
							}
						}
					}
					else
					{
						int iValue = AI_professionValue(eLoopProfession, pUnit, NULL, NULL);
						//if (iValue > iBestValue)	// TAC - AI Economy - koma13
						{
							bool bValid = true;
							if (!pUnit->canHaveProfession(eLoopProfession, false))
							{
								CvUnit* pWorstUnit = AI_getWorstProfessionUnit(eLoopProfession);
								// R&R, ray, removed unnecessary Assert from developing feature
								// FAssert(pWorstUnit != pUnit && pWorstUnit != NULL);
								// TAC - AI Economy - koma13 - START
								if (pWorstUnit != NULL && !pWorstUnit->isColonistLocked())
								{
									iValue = AI_professionValue(eLoopProfession, pUnit, NULL, pWorstUnit);
								}
								// TAC - AI Economy - koma13 - END
								if (pWorstUnit == NULL || pWorstUnit->isColonistLocked() || (iValue <= AI_professionValue(eLoopProfession, pWorstUnit, NULL, NULL)))
								{
									bValid = false;
								}
							}

							// TAC - AI Economy - koma13 - START
							//if (bValid)
							if (bValid && iValue > iBestValue)
							// TAC - AI Economy - koma13 - END
							{
								eBestProfession = eLoopProfession;
								iBestValue = iValue;
								iBestPlot = -1;
							}
						}
					}
				}
			}
		}
	}

	if(eBestProfession == NO_PROFESSION)
	{
		if (getPopulation() > 1)
		{
			bool bSuccess = removePopulationUnit(pUnit, false, (ProfessionTypes) GC.getCivilizationInfo(getCivilizationType()).getDefaultProfession());
			FAssertMsg(bSuccess, "Failed to remove useless citizen");
		}
		return NULL;
	}
		
	CvUnit* pDisplacedUnit = NULL;
	
	if (!GC.getProfessionInfo(eBestProfession).isWorkPlot())
	{
		if (!pUnit->canHaveProfession(eBestProfession, false))
		{
			pDisplacedUnit = AI_getWorstProfessionUnit(eBestProfession);
			FAssert(pDisplacedUnit != NULL);
			// TAC - AI Economy - koma13 - START
			//FAssert(AI_professionValue(eBestProfession, pUnit, getCityIndexPlot(iBestPlot), NULL) > AI_professionValue(pDisplacedUnit->getProfession(), pDisplacedUnit, getCityIndexPlot(iBestPlot), NULL));
			FAssert(AI_professionValue(eBestProfession, pUnit, getCityIndexPlot(iBestPlot), pDisplacedUnit) > AI_professionValue(pDisplacedUnit->getProfession(), pDisplacedUnit, getCityIndexPlot(iBestPlot), NULL));
			// TAC - AI Economy - koma13 - END
		}
	}
	else
	{
		FAssert(iBestPlot != -1);
		if (isUnitWorkingPlot(iBestPlot))
		{
			pDisplacedUnit = getUnitWorkingPlot(iBestPlot);
			FAssert(pDisplacedUnit != NULL);
			// TAC - AI Economy - koma13 - START
			//FAssert(AI_professionValue(eBestProfession, pUnit, getCityIndexPlot(iBestPlot), NULL) > AI_professionValue(pDisplacedUnit->getProfession(), pDisplacedUnit, getCityIndexPlot(iBestPlot), NULL));
			FAssert(AI_professionValue(eBestProfession, pUnit, getCityIndexPlot(iBestPlot), pDisplacedUnit) > AI_professionValue(pDisplacedUnit->getProfession(), pDisplacedUnit, getCityIndexPlot(iBestPlot), NULL));
			// TAC - AI Economy - koma13 - END
			clearUnitWorkingPlot(iBestPlot);
		}
	}

	if (pDisplacedUnit != NULL)
	{
		pDisplacedUnit->setProfession(NO_PROFESSION);
	}

	pUnit->setProfession(eBestProfession);

	FAssert(!isUnitWorkingAnyPlot(pUnit));
	if (iBestPlot != -1)
	{
		setUnitWorkingPlot(iBestPlot, pUnit->getID());
	}
	
	FAssert(iBestPlot != -1 || !GC.getProfessionInfo(eBestProfession).isWorkPlot());
	FAssert(iBestPlot == -1 || isUnitWorkingAnyPlot(pUnit));
	
	return pDisplacedUnit;
}	

//iValueA1 - Value of passed unit with original profession.
//iValueA2 - Value of passed unit with loop profession.
//iValueB1  - Value of loop unit with original profession.
//iValueB2 - Value of loop unit with loop profession.

CvUnit* CvCityAI::AI_juggleColonist(CvUnit* pUnit)
{
	ProfessionTypes eProfession = pUnit->getProfession();
	CvPlot* pPlot = getPlotWorkedByUnit(pUnit);
	
	CvUnit* pBestUnit = NULL;
	int iBestValue = 0;
	
	AI_setWorkforceHack(true);
	for (uint i = 0; i < m_aPopulationUnits.size(); ++i)
	{
		CvUnit* pLoopUnit = m_aPopulationUnits[i];
		if ((pLoopUnit != NULL) && (pUnit != pLoopUnit))
		{
			if (!pLoopUnit->isColonistLocked())
			{
				CvPlot* pLoopPlot = getPlotWorkedByUnit(pLoopUnit);
				ProfessionTypes eLoopProfession = pLoopUnit->getProfession();
				
				if (pLoopUnit->canHaveProfession(eProfession, true, pPlot) && pUnit->canHaveProfession(eLoopProfession, true, pLoopPlot))
				{
					int iValueA1 = AI_professionValue(eProfession, pUnit, pPlot, pLoopUnit);
					int iValueB1 = AI_professionValue(eLoopProfession, pLoopUnit, pLoopPlot, pUnit);

					int iValueA2 = AI_professionValue(eLoopProfession, pUnit, pLoopPlot, pLoopUnit);
					int iValueB2 = AI_professionValue(eProfession, pLoopUnit, pPlot, pUnit);

					//if ((iValueA2 > iValueA1 && iValueB2 >= iValueB1) || (iValueA2 >= iValueA1 && iValueB2 > iValueB1))
					{
						int iValue = (iValueA2 - iValueA1) + (iValueB2 - iValueB1);
						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							pBestUnit = pLoopUnit;
						}
					}
				}
			}
		}
	}
	AI_setWorkforceHack(false);
	
	if (pBestUnit != NULL)
	{
		AI_swapUnits(pUnit, pBestUnit);		
	}
	
	return pBestUnit;
	
}

void CvCityAI::AI_swapUnits(CvUnit* pUnitA, CvUnit* pUnitB)
{
	ProfessionTypes eProfessionA = pUnitA->getProfession();
	CvPlot* pPlotA = getPlotWorkedByUnit(pUnitA);
	
	ProfessionTypes eProfessionB = pUnitB->getProfession();
	CvPlot* pPlotB = getPlotWorkedByUnit(pUnitB);
	
	//remove from plot
	if (pPlotA != NULL)
	{
		clearUnitWorkingPlot(pPlotA);
	}
	if (pPlotB != NULL)
	{
		clearUnitWorkingPlot(pPlotB);
	}

	//remove from building
	
	pUnitA->setProfession(NO_PROFESSION);
	pUnitB->setProfession(NO_PROFESSION);

	pUnitA->setProfession(eProfessionB);
	if (pPlotB != NULL)
	{
		setUnitWorkingPlot(pPlotB, pUnitA->getID());
	}
	
	pUnitB->setProfession(eProfessionA);
	if (pPlotA != NULL)
	{
		setUnitWorkingPlot(pPlotA, pUnitB->getID());
	}
}

const int MAX_INPUT_YIELDS = 2;

// Erik: In case of multiple input yields, we store
// the contribution of each input
struct ProfessionValue
{
	int iIncome;
	int iTarget;
	int iYieldOutput;
	int iYieldInput;
	int iExtraYieldOutput;
	int iNetYield;
	int iNetValue;
};

int CvCityAI::AI_professionValue(ProfessionTypes eProfession, const CvUnit* pUnit, const CvPlot* pPlot, const CvUnit* pDisplaceUnit) const
{
	if (eProfession == NO_PROFESSION)
	{
		return 0;
	}

	CvProfessionInfo& kProfessionInfo = GC.getProfessionInfo(eProfession);
	
	
	int iIncome = 0;
	int iTarget = 0;
	int iYieldOutput = 0;
	int iYieldInput = 0;
	int iExtraYieldOutput = 0;
	int iNetYield = 0;
	int iNetValue = 0;

	YieldTypes eYieldProducedType = NO_YIELD;
	YieldTypes eYieldConsumedType = NO_YIELD;
	
	FAssert(pUnit != NULL);

	if (!pUnit->isOnMap())
	{
		if (!pUnit->canHaveProfession(eProfession, pDisplaceUnit != NULL, pPlot))
		{
			return 0;
		}
	}

	// R&R, ray , MYCP partially based on code of Aymerick - START
	if (kProfessionInfo.getYieldsProduced(0) == YIELD_EDUCATION)
	{
		if (pUnit->getUnitInfo().getStudentWeight() <= 0)
		{
			return 0;
		}
	}
	// R&R, ray , MYCP partially based on code of Aymerick - END
	if (!GC.getProfessionInfo(eProfession).isCitizen())
	{
		return 0;
	}
	
	CvUnitInfo& kUnit = GC.getUnitInfo(pUnit->getUnitType());
	
	if (GC.getProfessionInfo(eProfession).isWorkPlot())
	{
		if (pPlot == NULL)
		{
			return 0;
		}
		FAssert(canWork(pPlot));
		
		iYieldOutput = pPlot->calculatePotentialProfessionYieldAmount(eProfession, pUnit, false);
		// R&R, ray , MYCP partially based on code of Aymerick - START
		eYieldProducedType = (YieldTypes) kProfessionInfo.getYieldsProduced(0);
		// R&R, ray , MYCP partially based on code of Aymerick - END	
		if ((eYieldProducedType != NO_YIELD) && kUnit.getYieldChange(eYieldProducedType) > 0)
		{
			int iYieldChange = kUnit.getYieldChange(eYieldProducedType);
			if (pPlot->getBonusType() != NO_BONUS && GC.getBonusInfo(pPlot->getBonusType()).getYieldChange(eYieldProducedType) > 0)
			{
				iYieldChange += kUnit.getBonusYieldChange(eYieldProducedType);
				
			}
			if (pPlot->isWater())
			{
				if (kProfessionInfo.isWater() && kUnit.isWaterYieldChanges())
				{
					iExtraYieldOutput += iYieldChange;
				}
			}
			else
			{
				if (!kProfessionInfo.isWater() && kUnit.isLandYieldChanges())
				{
					iExtraYieldOutput += iYieldChange;
				}
			}
		}
	}
	else
	{
		FAssertMsg(pPlot == NULL, "passing in a plot for an indoors profession? Why?");
		
		iYieldOutput = getProfessionOutput(eProfession, pUnit);
		iYieldInput = getProfessionInput(eProfession, pUnit);
		// R&R, ray , MYCP partially based on code of Aymerick - START
		eYieldConsumedType = (YieldTypes) kProfessionInfo.getYieldsConsumed(0);
		eYieldProducedType = (YieldTypes) kProfessionInfo.getYieldsProduced(0);
		// R&R, ray , MYCP partially based on code of Aymerick - END
		if (eYieldProducedType != NO_YIELD)
		{
			iExtraYieldOutput += kUnit.getYieldChange(eYieldProducedType);
		}
	}

	if (eYieldProducedType == NO_YIELD)
	{
		FAssert(iYieldOutput == 0);
		return 0;
	}
	
	// R&R, ray, AI improvement
	/*
	if (eYieldProducedType == YIELD_HORSES && !isNative())
	{
		return 0;//XXX :P.
	}
	*/
	
	if (iYieldOutput == 0)
	{
		return 0;
	}
	// Currently, the only yield to require multiple inputs is YIELD_COLOURED_CLOTH (inputs: YIELD_INDIGO and YIELD_CLOTH)
	// so we do not support anything beyond that at the moment. We should consider asserting much earlier though
	FAssertMsg(kProfessionInfo.getNumYieldsConsumed() <= MAX_INPUT_YIELDS, "More than 2 input yields are not supported");

	ProfessionValue professionValue[MAX_INPUT_YIELDS];
	
	// Erik: If more than one yield is consumed, we have to separately evaluate
	// their contribution to the output
	for (int i = 0; i < kProfessionInfo.getNumYieldsConsumed(); i++)
	{
		ProfessionValue &pv = professionValue[i];
		
		pv.iYieldInput = iYieldInput;
		pv.iYieldOutput = iYieldOutput;
		pv.iExtraYieldOutput = iExtraYieldOutput;

		eYieldConsumedType = (YieldTypes)kProfessionInfo.getYieldsConsumed(i);
		
		pv.iYieldOutput *= getBaseYieldRateModifier(eYieldProducedType);
		pv.iYieldOutput /= 100;

		// R&R, ray, fix for CTD because of negative City Health could modify 1 to 0 - START
		pv.iYieldOutput = std::max(1, pv.iYieldOutput);
		// R&R, ray, fix for CTD because of negative City Health could modify 1 to 0 - START- END

		pv.iNetYield = getBaseRawYieldProduced(eYieldProducedType);

		CvUnit* pOldUnit = NULL;
		if (GC.getProfessionInfo(eProfession).isWorkPlot())
		{
			CvPlot* pWorkedPlot = getPlotWorkedByUnit(pUnit);
			if (pWorkedPlot != NULL)
			{
				pv.iNetYield -= pWorkedPlot->getYield(eYieldProducedType);

				if ((kProfessionInfo.isWater() && kUnit.isWaterYieldChanges()) || !kProfessionInfo.isWater() && kUnit.isLandYieldChanges())
				{
					if (pWorkedPlot->getBonusType() != NO_BONUS && GC.getBonusInfo(pWorkedPlot->getBonusType()).getYieldChange(eYieldProducedType) > 0)
					{
						pv.iExtraYieldOutput += kUnit.getBonusYieldChange(eYieldProducedType);
					}
				}
			}

			if (pPlot != NULL && pPlot->isBeingWorked() && pWorkedPlot != pPlot)
			{
				pv.iNetYield -= pPlot->getYield(eYieldProducedType);
				pOldUnit = getUnitWorkingPlot(pPlot);
			}
		}
		else
		{
			ProfessionTypes eWorkedProfession = pUnit->getProfession();
			if (eWorkedProfession != NO_PROFESSION)
			{
				// R&R, ray , MYCP partially based on code of Aymerick - START
				if (GC.getProfessionInfo(eWorkedProfession).getYieldsProduced(0) == eYieldProducedType)
				{
					pv.iNetYield -= getProfessionOutput(eWorkedProfession, pUnit);
				}

				if (GC.getProfessionInfo(eWorkedProfession).getYieldsConsumed(0) == eYieldProducedType)
				{
					pv.iNetYield += getProfessionInput(eWorkedProfession, pUnit);
				}
				// R&R, ray , MYCP partially based on code of Aymerick - END
			}
		}

		// R&R, ray, fix for CTD because of negative City Health could modify 1 to 0 - START
		// iNetYield *= getBaseYieldRateModifier(eYieldProducedType);
		// iNetYield /= 100;
		if (pv.iNetYield != 0)
		{
			pv.iNetYield *= getBaseYieldRateModifier(eYieldProducedType);
			pv.iNetYield /= 100;
			pv.iNetYield = std::max(1, pv.iNetYield);
		}
		// R&R, ray, fix for CTD because of negative City Health could modify 1 to 0 - END

		pv.iNetYield -= getRawYieldConsumed(eYieldProducedType);

		int iOutputValue = 0;
		int iInputValue = 0;
		CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());

		if (!kProfessionInfo.isWorkPlot() && (eYieldProducedType != YIELD_EDUCATION))
		{
			int iConsumedAlready = (eYieldConsumedType == NO_YIELD) ? 0 : getRawYieldConsumed(eYieldConsumedType);
			int iRealInputAvailable = (eYieldConsumedType == NO_YIELD) ? 0 : getRawYieldProduced(eYieldConsumedType) - iConsumedAlready;

			if (eYieldConsumedType != NO_YIELD)
			{
				bool bDontDiplace = false;
				if (pPlot != NULL)
				{
					if (pPlot->isBeingWorked())
					{
						CvUnit* pWorkingUnit = getUnitWorkingPlot(pPlot);
						if (pWorkingUnit != pUnit)
						{
							iRealInputAvailable -= pPlot->getYield(eYieldConsumedType);
							if (pWorkingUnit == pDisplaceUnit)
							{
								bDontDiplace = true;
							}
						}
					}
				}

				if (!bDontDiplace)
				{
					if (pDisplaceUnit != NULL)
					{
						if (pDisplaceUnit->getProfession() == eProfession)
						{
							iRealInputAvailable += getProfessionOutput(eProfession, pDisplaceUnit);
						}
					}
				}

				if (pUnit->getProfession() == eProfession)
				{
					iRealInputAvailable += getProfessionInput(eProfession, pUnit);
				}
			}

			int iEstimatedInputAvailable = 0;

			if (eYieldConsumedType != NO_YIELD)
			{
				iEstimatedInputAvailable += iRealInputAvailable + getYieldStored(eYieldConsumedType) / 10;

				int iImports = AI_getTradeBalance(eYieldConsumedType);
				if (iImports > 0)
				{
					iEstimatedInputAvailable += std::min(iImports, getYieldStored(eYieldConsumedType));
				}
			}

			if (eYieldConsumedType == NO_YIELD || ((iRealInputAvailable + getYieldStored(eYieldConsumedType)) > 0 && iEstimatedInputAvailable > 0))
			{
				CvUnit* pIdealAssignedUnit = NULL;
				CvUnit* pIdealUnassignedUnit = NULL;
				int iProfessionCount = 0;
				for (int i = 0; i < getPopulation(); ++i)
				{
					CvUnit* pLoopUnit = getPopulationUnitByIndex(i);
					if (pLoopUnit->getProfession() == eProfession)
					{
						iProfessionCount++;
					}
					if (pLoopUnit->AI_getIdealProfession() == eProfession)
					{
						if (pLoopUnit->getProfession() == eProfession)
						{
							pIdealAssignedUnit = pLoopUnit;
						}
						else if (!pLoopUnit->isColonistLocked())
						{
							pIdealUnassignedUnit = pLoopUnit;
						}
					}
				}

				if ((pDisplaceUnit != NULL) && (pDisplaceUnit != pUnit))
				{
					if (pDisplaceUnit->getProfession() == eProfession)
					{
						iProfessionCount--;
					}
				}
				if (pUnit->getProfession() == eProfession)
				{
					iProfessionCount--;
				}
				FAssert(iProfessionCount >= 0);

				if (eYieldConsumedType == NO_YIELD)
				{
					//iOutputValue += 100 * kOwner.AI_yieldValue(eYieldProducedType, true, pv.iYieldOutput);
					iOutputValue += 100 * AI_estimateYieldValue(eYieldProducedType, pv.iYieldOutput);
					


				}
				else
				{
					FAssert(pv.iYieldInput > 0);
					//iOutputValue += 100 * kOwner.AI_yieldValue(eYieldProducedType) * pv.iYieldOutput * std::min(pv.iYieldInput, iEstimatedInputAvailable) / std::max(1, pv.iYieldInput);
					iOutputValue += 100 * AI_estimateYieldValue(eYieldProducedType, 1) * pv.iYieldOutput * std::min(pv.iYieldInput, iEstimatedInputAvailable) / std::max(1, pv.iYieldInput);
					//iInputValue += 100 * kOwner.AI_yieldValue(eYieldConsumedType, false) * std::min(pv.iYieldInput, iEstimatedInputAvailable);
					iInputValue += 100 * AI_estimateYieldValue(eYieldConsumedType, 1) * std::min(pv.iYieldInput, iEstimatedInputAvailable);
				}


				if (pIdealUnassignedUnit != NULL)
				{
					if (pUnit->AI_getIdealProfession() != eProfession)
					{
						iOutputValue /= 3;
					}
				}

				//If the ideal Unit isn't assigned to this profession. What right does this unit have?
				if (pIdealAssignedUnit != NULL)
				{
					if (pIdealAssignedUnit->AI_getIdealProfession() != eProfession)
					{
						iOutputValue /= 3;

						iOutputValue *= kOwner.AI_professionSuitability(pUnit->getUnitType(), eProfession);
						iOutputValue /= std::max(1, kOwner.AI_professionSuitability(pIdealAssignedUnit->getUnitType(), eProfession));
					}
				}

				if (eYieldConsumedType != NO_YIELD)
				{
					//Strongly discourage conversion of raw materials by poorly qualified units.
					if (iEstimatedInputAvailable < pv.iYieldInput)
					{
						if (iRealInputAvailable + getYieldStored(eYieldConsumedType) < pv.iYieldInput)
						{
							iOutputValue /= 4;
						}

						if (pUnit->AI_getIdealProfession() != eProfession && pIdealUnassignedUnit != NULL)
						{
							iOutputValue *= iEstimatedInputAvailable;
							iOutputValue /= pv.iYieldInput;
						}
					}
				}
				else
				{
					if (pIdealAssignedUnit != NULL)
					{
						//Somewhat discourage employment by poorly qualified units.
						if (pUnit->AI_getIdealProfession() != eProfession)
						{
							if (pIdealAssignedUnit->getProfession() == eProfession)
							{
								if (eYieldProducedType == YIELD_CROSSES)
								{
									iOutputValue *= 50;
									iOutputValue /= 100;
								}
								else
								{
									iOutputValue *= 75;
									iOutputValue /= 100;
								}
							}
						}
					}
				}

				// TAC - AI Economy - koma13 - START
				/*
				if (eYieldProducedType == YIELD_BELLS && kOwner.AI_isStrategy(STRATEGY_FAST_BELLS))
				{
					if ((iProfessionCount == 0) && (getPopulation() > 3))
					{
						iOutputValue *= 2;
					}
				}
				*/

				// Note that currently there are no professions that produce culture
				if (eYieldProducedType == YIELD_CULTURE)
				{
					int iCulturePressure = AI_calculateCulturePressure();

					if (iCulturePressure > 0)
					{
						iOutputValue *= 2;

						if (iCulturePressure > 100)
						{
							iOutputValue *= 100 + iCulturePressure;
							iOutputValue /= 100;
						}
					}
					/*
					else if (kOwner.AI_isStrategy(STRATEGY_FAST_BELLS) || getCultureLevel() < 2)
					{
						if ((iProfessionCount == 0) && (getPopulation() > 3))
						{
							iOutputValue *= (getCultureLevel() < 2 ? 9 : 3);
						}
					}
					*/
				}
				// TAC - AI Economy - koma13 - END
			}
		}
		else
		{
			//iOutputValue += (100 * pv.iYieldOutput + 25 * pv.iExtraYieldOutput) * kOwner.AI_yieldValue(eYieldProducedType);
			iOutputValue += (100 * pv.iYieldOutput + 25 * pv.iExtraYieldOutput) * AI_estimateYieldValue(eYieldProducedType, 1);
		}

		iOutputValue *= AI_getYieldOutputWeight(eYieldProducedType);
		iOutputValue /= 100;

		if (isNative())
		{
			// R&R, ray, AI improvement for MYP
			if (GC.getProfessionInfo(eProfession).getNumYieldsProduced() > 1)
			{
				return ((iOutputValue - iInputValue) * 150) / 100;
			}
			else
			{
				return (iOutputValue - iInputValue);
			}
		}

		if (eYieldProducedType != NO_YIELD)
		{
			iOutputValue *= 100 + (kOwner.AI_professionSuitability(pUnit, eProfession, pPlot) - 100) / 2;
			iOutputValue /= 100;

			if (eYieldConsumedType != NO_YIELD)
			{
				iOutputValue *= 50 + AI_getYieldAdvantage(eYieldProducedType);
				iOutputValue /= 150;
			}
		}

		if (eYieldConsumedType != NO_YIELD && eYieldConsumedType != YIELD_FOOD)
		{
			//VET NewCapacity - begin 4/7
			if (GC.getNEW_CAPACITY())
			{
				if (getTotalYieldStored() > getMaxYieldCapacity())
				{
					iInputValue /= 5;
				}
			}
			else
			{
				if (getYieldStored(eYieldConsumedType) > getMaxYieldCapacity())
				{
					iInputValue /= 5;
				}
			}
			//VET NewCapacity - end 4/7
		}

		if ((eYieldProducedType != YIELD_FOOD) && GC.getYieldInfo(eYieldProducedType).isCargo())
		{
			int iNeededYield = AI_getNeededYield(eYieldProducedType) - pv.iNetYield;

			if (iNeededYield > 0)
			{
				int iTraded = AI_getTradeBalance(eYieldProducedType);
				if (iTraded > 0)
				{
					iNeededYield = std::max(1, iNeededYield - iTraded);
				}

				iNeededYield = std::min(iNeededYield, pv.iYieldOutput);
				//VET NewCapacity - begin 5/7
							//int iExtraValue = iNeededYield * (50 + 100 * (getMaxYieldCapacity() - getYieldStored(eYieldProducedType)) / getMaxYieldCapacity());
				int iExtraValue;
				if (GC.getNEW_CAPACITY())
				{
					iExtraValue = iNeededYield * (50 + 100 * (getMaxYieldCapacity() - getTotalYieldStored()) / getMaxYieldCapacity());
				}
				else
				{
					iExtraValue = iNeededYield * (50 + 100 * (getMaxYieldCapacity() - getYieldStored(eYieldProducedType)) / getMaxYieldCapacity());
				}
				//VET NewCapacity - end 5/7
				iExtraValue *= AI_getYieldOutputWeight(eYieldProducedType);
				iExtraValue /= 100;
				iOutputValue += iExtraValue;
			}

			int iPercentWasted = 0;
			if ((pv.iNetYield + pv.iYieldOutput > 0) && GC.getYieldInfo(eYieldProducedType).isCargo())
			{
				int iSpareCapacity = std::max(0, getMaxYieldCapacity() - (getYieldStored(eYieldProducedType) + pv.iNetYield));
				//VET NewCapacity - begin 6/7
							//int iExcess = getYieldStored(eYieldProducedType) + (iNetYield + iYieldOutput) - getMaxYieldCapacity();
				int iExcess;
				if (GC.getNEW_CAPACITY())
				{
					iExcess = getTotalYieldStored() + (pv.iNetYield + pv.iYieldOutput) - getMaxYieldCapacity();
				}
				else
				{
					iExcess = getYieldStored(eYieldProducedType) + (pv.iNetYield + pv.iYieldOutput) - getMaxYieldCapacity();
				}
				//VET NewCapacity - end 6/7
				int iLoss = 0;
				if (iExcess > 0)
				{
					iLoss = std::max(GC.getCITY_YIELD_DECAY_PERCENT() * iExcess / 100, GC.getMIN_CITY_YIELD_DECAY());
					iLoss = std::min(iLoss, iExcess);
				}

				iPercentWasted = 100 - (100 * std::max(0, pv.iYieldOutput - iLoss)) / pv.iYieldOutput;
			}

			if (iPercentWasted > 0)
			{
				int iStubbornness = 10;
				if (pUnit->AI_getIdealProfession() == eProfession)
				{
					iStubbornness += 15;
				}
				if (!isHuman())
				{
					iStubbornness *= 2;
				}

				iOutputValue = (iOutputValue * (100 - iPercentWasted)) + iStubbornness * iOutputValue * iPercentWasted / 100;
				iOutputValue /= 100;
			}
		}

		iOutputValue /= 100;
		iInputValue /= 100;

		if (kProfessionInfo.isWorkPlot() && pPlot != NULL)
		{
			if (pPlot->getBonusType() != NO_BONUS)
			{
				CvBonusInfo& kBonus = GC.getBonusInfo(pPlot->getBonusType());
				if (kBonus.getYieldChange(eYieldProducedType) <= 0)
				{
					for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
					{
						iOutputValue -= kBonus.getYieldChange(iYield);
					}
				}
			}
		}

		if (eYieldProducedType == YIELD_FOOD)
		{
			int iBaseFood = pv.iNetYield;

			int iDifference = (iBaseFood + getYieldStored(YIELD_FOOD));

			if (iDifference < 0)
			{
				iOutputValue += (50 * std::min(pv.iYieldOutput, -iDifference));
			}
		}

		pv.iNetValue = (iOutputValue - iInputValue);

		//int iMinProfessionValue = kOwner.AI_yieldValue(YIELD_FOOD, true, GC.getFOOD_CONSUMPTION_PER_POPULATION());
		int iMinProfessionValue = AI_estimateYieldValue(YIELD_FOOD, GC.getFOOD_CONSUMPTION_PER_POPULATION());
		if (pv.iNetValue <= iMinProfessionValue)
		{
			// TAC - AI Economy - koma13 - START
			//if (!isHuman())
			if (!isHuman() && (eYieldConsumedType != NO_YIELD))
				// TAC - AI Economy - koma13 - END
			{
				return 0;
			}
			else
			{
				pv.iNetValue /= 2;
			}
		}

		if (pOldUnit != NULL && pOldUnit->getProfession() != eProfession && pOldUnit->getProfession() != NO_PROFESSION)
		{
			const CvPlot* pOldPlot = NULL;
			if (GC.getProfessionInfo(pOldUnit->getProfession()).isWorkPlot())
			{
				pOldPlot = pPlot;
			}

			if (pv.iNetValue <= AI_professionValue(pOldUnit->getProfession(), pOldUnit, pOldPlot, NULL))
			{
				return 0;
			}
		}

		// R&R, ray, AI improvement for MYP
		if (GC.getProfessionInfo(eProfession).getNumYieldsProduced() > 1)
		{
			pv.iNetValue = (pv.iNetValue * 150) / 100;
		}
	}

	int iCombinedValue = professionValue[0].iNetValue;

	if (kProfessionInfo.getNumYieldsConsumed() > 1)
	{
		// This input was not available, return early
		if (professionValue[0].iNetValue == 0)
			return 0;

		// Erik: In the case of multiple input yields, we use
		// the minimum value resulting from the evaluation of the
		// input yield (if any input is missing / does not contribute, we return early)
		// This is not strictly correct, but to achieve that we'd have to rewrite 
		// most of the function and make it far more sophisticated
		for (int i = 1; i < kProfessionInfo.getNumYieldsConsumed(); i++)
		{
			if (professionValue[i].iNetValue == 0)
				return 0;

			iCombinedValue = std::min(iCombinedValue, professionValue[i].iNetValue);
		}
	}

	return std::max(0, iCombinedValue);
}

int CvCityAI::AI_professionBasicOutput(ProfessionTypes eProfession, UnitTypes eUnit, const CvPlot* pPlot) const
{
	FAssert(NO_PROFESSION != eProfession);
	if (NO_PROFESSION == eProfession)
	{
		return 0;
	}

	CvProfessionInfo& kProfessionInfo = GC.getProfessionInfo(eProfession);

	// R&R, ray , MYCP partially based on code of Aymerick - START
	YieldTypes eYieldProduced = (YieldTypes) kProfessionInfo.getYieldsProduced(0);
	// R&R, ray , MYCP partially based on code of Aymerick - END
	if (NO_YIELD == eYieldProduced)
	{
		return 0;
	}

	int iProfessionOutput = 0;
	if (kProfessionInfo.isWorkPlot())
	{
		iProfessionOutput = pPlot->calculatePotentialYield(eYieldProduced, getOwnerINLINE(), pPlot->getImprovementType(), true, pPlot->getRouteType(), eUnit, false);
		if (pPlot->getFeatureType() != NO_FEATURE)
		{
			iProfessionOutput = std::max(iProfessionOutput, pPlot->calculatePotentialYield(eYieldProduced, getOwnerINLINE(), pPlot->getImprovementType(), false, pPlot->getRouteType(), eUnit, false));
		}
	}
	else
	{

		SpecialBuildingTypes eSpecialBuilding = (SpecialBuildingTypes) kProfessionInfo.getSpecialBuilding();
		if (eSpecialBuilding == NO_SPECIALBUILDING)
		{
			return 0;
		}
		
		int iModifier = 100;
		int iExtra = 0;
		// TAC - AI Economy - koma13 - START
		//if (eUnit != NULL)
		if (eUnit != NO_UNIT)
		// TAC - AI Economy - koma13 - END
		{
			iModifier += GC.getUnitInfo(eUnit).getYieldModifier(eYieldProduced);
			iExtra += GC.getUnitInfo(eUnit).getYieldChange(eYieldProduced);
		}

		for (int i = 0; i < GC.getNumBuildingInfos(); i++)
		{
			BuildingTypes eBuilding = (BuildingTypes) i;
			if (GC.getBuildingInfo(eBuilding).getSpecialBuildingType() == eSpecialBuilding)
			{
				if (isHasBuilding(eBuilding))
				{
					int iBuildingOutput = (GC.getBuildingInfo(eBuilding).getProfessionOutput() + iExtra) * iModifier / 100;
					if (iBuildingOutput > iProfessionOutput)
					{
						iProfessionOutput = iBuildingOutput;
					}
				}
			}
		}
	}


	return iProfessionOutput;
}


CvUnit* CvCityAI::AI_getWorstProfessionUnit(ProfessionTypes eProfession) const
{
	int iWorstOutput = MAX_INT;
	CvUnit* pWorstUnit = NULL;
	for (uint i = 0; i < m_aPopulationUnits.size(); ++i)
	{
		CvUnit* pOldUnit = m_aPopulationUnits[i];
		if (pOldUnit->getProfession() == eProfession)
		{
			int iOutput = getProfessionOutput(eProfession, pOldUnit);
			if (iOutput < iWorstOutput)
			{
				iWorstOutput = iOutput;
				pWorstUnit = pOldUnit;
			}
		}
	}

	return pWorstUnit;
}	

int CvCityAI::AI_unitJoinCityValue(CvUnit* pUnit, ProfessionTypes* peNewProfession) const
{
	int iBestValue = 0;
	int iBestPlot = -1;
	ProfessionTypes eBestProfession = NO_PROFESSION;
	
	for (int i=0;i<GC.getNumProfessionInfos();i++)
	{
		ProfessionTypes eLoopProfession = (ProfessionTypes) i;
		if (GC.getCivilizationInfo(getCivilizationType()).isValidProfession(eLoopProfession))
		{
			if (GC.getProfessionInfo(eLoopProfession).isCitizen())
			{
				if (pUnit->canHaveProfession(eLoopProfession, false, plot()))
				{
					if (GC.getProfessionInfo(eLoopProfession).isWorkPlot())
					{
						for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
						{
							if (iI != CITY_HOME_PLOT)
							{
								CvPlot* pLoopPlot = getCityIndexPlot(iI);

								if (pLoopPlot != NULL)
								{
									if (!isUnitWorkingPlot(iI))
									{
										if (canWork(pLoopPlot))
										{
											int iValue = AI_professionValue(eLoopProfession, pUnit, pLoopPlot, NULL);
											
											if (iValue > iBestValue)
											{
												eBestProfession = eLoopProfession;
												iBestValue = iValue;
												iBestPlot = iI;
											}
										}
									}
								}
							}
						}
					}
					else
					{
						int iValue = AI_professionValue(eLoopProfession, pUnit, NULL, NULL);
						if (iValue > iBestValue)
						{
							eBestProfession = eLoopProfession;
							iBestValue = iValue;
							iBestPlot = -1;
						}
					}
				}
			}
		}
	}
	
	int iFood = 0;
	for (int i = 0; i < NUM_CITY_PLOTS; ++i)
	{
		CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), i);
		if (pLoopPlot != NULL)
		{
			iFood += std::max(0, pLoopPlot->getYield(YIELD_FOOD) - GC.getFOOD_CONSUMPTION_PER_POPULATION());
		}
	}
	if (iFood < getPopulation())
	{
		iBestValue *= 6 + iFood;
		iBestValue /= 6 + getPopulation() * GC.getFOOD_CONSUMPTION_PER_POPULATION();
	}
	else if (iFood > getPopulation())
	{
		iBestValue *= 4 + iFood;
		iBestValue /= 4 + getPopulation() * GC.getFOOD_CONSUMPTION_PER_POPULATION();
	}
	
	if (peNewProfession != NULL)
	{
		*peNewProfession = eBestProfession;
	}
	
	return iBestValue;	
}

int CvCityAI::AI_unitJoinReplaceValue(CvUnit* pUnit, CvUnit** pReplaceUnit) const
{
	int iBestValue = 0;
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	for (int i = 0; i < getPopulation(); ++i)
	{
		CvUnit* pLoopUnit = getPopulationUnitByIndex(i);
		FAssert(pLoopUnit != NULL);

		if (pLoopUnit->getProfession() != NO_PROFESSION)
		{
		
		
			CvPlot* pPlot = getPlotWorkedByUnit(pLoopUnit);
			if (pPlot == NULL)
			{
				pPlot = plot();
			}
			
			int iExistingValue = kOwner.AI_professionSuitability(pLoopUnit, pLoopUnit->getProfession(), pPlot);
			
			int iNewValue = kOwner.AI_professionSuitability(pUnit, pLoopUnit->getProfession(), pPlot);
			
			int iValue = iNewValue - iExistingValue;
			
			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				if (pReplaceUnit != NULL)
				{
					*pReplaceUnit = pLoopUnit;
				}
			}
		}
	}
	
	return iBestValue;
}

ProfessionTypes CvCityAI::AI_bestPlotProfession(const CvUnit* pUnit, const CvPlot* pPlot) const
{
	FAssert(pUnit != NULL);
	FAssert(pPlot != NULL);
	
	ProfessionTypes eBestProfession = NO_PROFESSION;
	int iBestValue = -1;
	for (int iI = 0; iI < GC.getNumProfessionInfos(); iI++)
	{
		ProfessionTypes eLoopProfession = (ProfessionTypes)iI;
		if (GC.getCivilizationInfo(getCivilizationType()).isValidProfession(eLoopProfession))
		{
			if (GC.getProfessionInfo(eLoopProfession).isWorkPlot())
			{
				int iValue = AI_professionValue(eLoopProfession, pUnit, pPlot, NULL);
				if (iValue > iBestValue)
				{
					eBestProfession = eLoopProfession;
					iBestValue = iValue;
				}
			}
		}
	}

	return eBestProfession;
}

int CvCityAI::AI_bestProfessionPlot(ProfessionTypes eProfession, const CvUnit* pUnit) const
{
	FAssert(pUnit != NULL);
	FAssert(eProfession != NO_PROFESSION);
	
	int iBestValue = 0;
	int iBestPlot = -1;
	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		if (iI != CITY_HOME_PLOT)
		{
			CvPlot* pLoopPlot = getCityIndexPlot(iI);

			if (pLoopPlot != NULL)
			{
				if (!isUnitWorkingPlot(iI) || (getUnitWorkingPlot(iI) == pUnit))
				{
					if (canWork(pLoopPlot))
					{
						int iValue = AI_professionValue(eProfession, pUnit, pLoopPlot, NULL);
						
						// Erik: Make sure that we pick a plot even if it has no apparent economic value
						// since the caller of this function cannot deal with the case that no plot was found
						if (iValue >= iBestValue)
						{
							iBestValue = iValue;
							iBestPlot = iI;
						}
					}
				}
			}
		}
	}
	
	return iBestPlot;	
}

	
bool CvCityAI::AI_canMakeGift() const
{
	return (AI_getGiftTimer() <= 0);
}

int CvCityAI::AI_getGiftTimer() const
{
	return m_iGiftTimer;
}

void CvCityAI::AI_setGiftTimer(int iNewValue)
{
	m_iGiftTimer = iNewValue;	
	FAssert(AI_getGiftTimer() >= 0);
}

void CvCityAI::AI_changeGiftTimer(int iChange)
{
	if (iChange != 0)
	{
		AI_setGiftTimer(AI_getGiftTimer() + iChange);
	}
}

// R&R, ray, Natives Trading - START
bool CvCityAI::AI_canMakeTrade() const
{
	return (AI_getTradeTimer() <= 0);
}

int CvCityAI::AI_getTradeTimer() const
{
	return m_iTradeTimer;
}

void CvCityAI::AI_setTradeTimer(int iNewValue)
{
	m_iTradeTimer = iNewValue;	
	FAssert(AI_getTradeTimer() >= 0);
}

void CvCityAI::AI_changeTradeTimer(int iChange)
{
	if (iChange != 0)
	{
		AI_setTradeTimer(AI_getTradeTimer() + iChange);
	}
}
// R&R, ray, Natives Trading - END
	
int CvCityAI::AI_maxGoldTrade(PlayerTypes ePlayer) const
{
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
		return kOwner.AI_maxGoldTrade(ePlayer);
	}

int CvCityAI::AI_calculateAlarm(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex expected to be < MAX_PLAYERS");

	AlarmTypes eAlarm = (AlarmTypes) GC.getLeaderHeadInfo(GET_PLAYER(getOwnerINLINE()).getLeaderType()).getAlarmType();
	if (eAlarm == NO_ALARM)
	{
		return 0;
	}
	
	CvAlarmInfo& kAlarm = GC.getAlarmInfo(eAlarm);
	
	int iPositiveAlarm = 0;
	int iNegativeAlarm = 0;
	
	int iRange = kAlarm.getRange();
	for (int iX = -iRange; iX <= iRange; iX++)
	{
		for (int iY = -iRange; iY <= iRange; iY++)
		{
			int iDistance = plotDistance(iX, iY, 0, 0);
			if (iDistance <= iRange)
			{
				CvPlot* pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iX, iY);
				if (pLoopPlot != NULL)
				{
					int iPlotAlarm = 0;
					CvCity* pLoopCity = pLoopPlot->getPlotCity();
					if (pLoopCity != NULL)
					{
						if (pLoopCity->getOwner() == eIndex)
						{
							iPlotAlarm += kAlarm.getColony();
							iPlotAlarm += pLoopCity->getPopulation() * kAlarm.getPopulation();
						}
					}

					iPlotAlarm = iPlotAlarm * std::max(0, iRange - iDistance + 1) / std::max(1, iRange + 1);

					iPositiveAlarm += iPlotAlarm;
				}
			}
		}
	}
	
	//Religion
	if (getMissionaryPlayer() != NO_PLAYER)
	{
		if (GET_PLAYER(getMissionaryPlayer()).getCivilizationType() == GET_PLAYER(eIndex).getCivilizationType())
		{
			int iModifier = 100 + GET_PLAYER(eIndex).getMissionaryRateModifier() + GET_PLAYER(getOwnerINLINE()).getMissionaryRateModifier();
			iNegativeAlarm += getMissionaryRate() * kAlarm.getMissionary() * iModifier / 100;
		}
	}

	int iModifier = 100; 
	iModifier += GET_PLAYER(eIndex).getNativeAngerModifier();
	iModifier = std::max(0, iModifier);

	iPositiveAlarm *= iModifier;
	iPositiveAlarm /= 100;
	
	return (iPositiveAlarm + iNegativeAlarm);
}

const int YIELD_BELLS_BASE_VALUE = 10;
const int YIELD_HAMMERS_BASE_VALUE = 10;
const int YIELD_TOOLS_BASE_VALUE = 10;



int CvCityAI::AI_estimateYieldValue(YieldTypes eYield, int iAmount) const
{
	int iValue = iAmount * GET_PLAYER(getOwnerINLINE()).AI_yieldValue(eYield);
	
	switch (eYield)
	{

		case YIELD_FOOD:
		// Strategic resource, no reduction
		case YIELD_HORSES:
		case YIELD_COCOA:
		case YIELD_COFFEE:
		case YIELD_CIGARS:
		case YIELD_WOOL_CLOTH:
		case YIELD_CLOTH:
		case YIELD_COLOURED_CLOTH:
		case YIELD_LEATHER:
		case YIELD_COATS:
		case YIELD_PREMIUM_COATS:
		case YIELD_SALT:
		case YIELD_SPICES:
		case YIELD_BEER:
		case YIELD_RUM:
		case YIELD_WINE:
		case YIELD_WHALE_OIL:
		case YIELD_FURNITURE:
			break;
		// We punish overproduction of input yields 
		case YIELD_LUMBER:
		case YIELD_STONE:
		case YIELD_HEMP:
		case YIELD_ORE:
		case YIELD_SHEEP:
		case YIELD_CATTLE:
		case YIELD_COCA_LEAVES:
		case YIELD_COCOA_FRUITS:
		case YIELD_COFFEE_BERRIES:
		case YIELD_TOBACCO:
		case YIELD_WOOL:
		case YIELD_COTTON:
		case YIELD_INDIGO:
		case YIELD_HIDES:
		case YIELD_FUR:
		case YIELD_PREMIUM_FUR:
		case YIELD_RAW_SALT:
		case YIELD_RED_PEPPER:
		case YIELD_BARLEY:
		case YIELD_SUGAR:
		case YIELD_GRAPES:
		case YIELD_WHALE_BLUBBER:
		case YIELD_VALUABLE_WOOD:
		{
			PlayerTypes eParent = GET_PLAYER(getOwnerINLINE()).getParent();

			if (eParent != NO_PLAYER)
			{
				CvPlayer& kParent = GET_PLAYER(eParent);

				// We value the yield by what we would have had to pay for it in
				// a port as long as we only have a small amount of it
				const int iBaselineAmount = 50;

				// We want to encourage the production of a least a small
				// amount of any given yield since that makes it more
				// likely that it can be used as input
				if (getYieldStored(eYield) <= iBaselineAmount)
				{
					const int iBestBuyPrice = std::min(kParent.getYieldSellPrice(eYield), kParent.getYieldAfricaSellPrice(eYield));
					iValue = iAmount * iBestBuyPrice;
				}
				else
				{ 
					// Erik: For every 100 units in excess of the maintain level + baseline,
					// decrement the estimated value by 1 to "punish" overproduction
					// Note: This should only be applied to input yields and 
					// not final products like muskets etc.
					const int iExcessSurplus = std::max(0, getYieldStored(eYield) - getMaintainLevel(eYield)) - iBaselineAmount;
					const int iReductionFactor = iExcessSurplus / 100;
					// TODO: Consider domestic prices as well
					const int iBestSellPrice = std::max(kParent.getYieldBuyPrice(eYield), kParent.getYieldAfricaBuyPrice(eYield));
					iValue = iAmount * std::max(1, iBestSellPrice - iReductionFactor);
				}
			}
		}
		break;
		case YIELD_TRADE_GOODS:
		// These are strategic yields and their price is never reduced
		case YIELD_ROPE:
		case YIELD_SAILCLOTH:
			break;
		case YIELD_TOOLS:
		{
			const int populationMultiplier = std::max(1U, m_aPopulationUnits.size() / 5);
			iValue = static_cast<int>(iAmount * YIELD_TOOLS_BASE_VALUE + populationMultiplier);
		}
		break;
		case YIELD_BLADES:
		case YIELD_MUSKETS:
		// Previous metals are never reduced in value
		case YIELD_SILVER:
		case YIELD_GOLD:
		case YIELD_GEMS:
		case YIELD_LUXURY_GOODS:
		break;
		case YIELD_HAMMERS:
		{
			const int populationMultiplier = std::max(1U, m_aPopulationUnits.size() / 5);
			iValue = static_cast<int>(iAmount * YIELD_HAMMERS_BASE_VALUE + populationMultiplier);
		}
		break;
		case YIELD_BELLS:
		{
			// Erik: Estimate the value of bells based on this formula: 
			// If rebel sentiment is <50%: 
			//   Liberty bell value = (10 + pop/5 ) * (150% - rebel sentiment%) 
			// Else
			//  Liberty bell value = (10 + pop/5)
				
			// Note that the doubles are necessary for the calculation to round correctly
			const int rebelPercent = getRebelPercent();
			const double rebelFactor = (125 - getRebelPercent()) / (double)100;
			const double populationMultiplier = std::max(1U, m_aPopulationUnits.size() / 5);
				
			iValue = static_cast<int>(iAmount * ((YIELD_BELLS_BASE_VALUE + populationMultiplier) * (getRebelPercent() < 50 ? rebelFactor : 1.0)));
		}
		break;
		case YIELD_CROSSES:
			break;
		case YIELD_CULTURE:
			break;
		case YIELD_HEALTH:
			break;
		case YIELD_EDUCATION:
			break;
		case YIELD_HAPPINESS: // WTP, ray, Happiness - START
			break;
		case YIELD_UNHAPPINESS: // WTP, ray, Happiness - START
			break;
		case YIELD_CANNONS:
			// Erik: Since the AI cannot use this yield for military purposes, I've decided to
			// block it so that production is not diverted to it. (cannons are usually just sold in Europe
			// without this fix) 
			iValue = 0;
			break;
		default:
			FAssert(false);
	}

	return iValue;
}

//Note that 0 means the camp should be disbanded in some way...
int CvCityAI::AI_getTargetSize() const
{
	return m_iTargetSize;	
}

void CvCityAI::AI_setTargetSize(int iNewValue)
{
	m_iTargetSize = iNewValue;	
}

//Yield inflow is the weight to put on delivering goods here
int CvCityAI::AI_getYieldOutputWeight(YieldTypes eYield) const
{
	FAssertMsg(eYield > NO_YIELD, "Index out of bounds");
	FAssertMsg(eYield < NUM_YIELD_TYPES, "Index out of bounds");
	
	return m_aiYieldOutputWeight[eYield];
}

void CvCityAI::AI_setYieldOutputWeight(YieldTypes eYield, int iNewValue)
{
	FAssertMsg(eYield > NO_YIELD, "Index out of bounds");
	FAssertMsg(eYield < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(iNewValue >= 0, "Weight should be positive");
	
	m_aiYieldOutputWeight[eYield] = iNewValue;	
}
	
int CvCityAI::AI_getNeededYield(YieldTypes eYield) const
{
	FAssertMsg(eYield > NO_YIELD, "Index out of bounds");
	FAssertMsg(eYield < NUM_YIELD_TYPES, "Index out of bounds");
	
	return m_aiNeededYield[eYield];
	
}

void CvCityAI::AI_setNeededYield(YieldTypes eYield, int iNewValue)
{
	FAssertMsg(eYield > NO_YIELD, "Index out of bounds");
	FAssertMsg(eYield < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(iNewValue > 0, "Negative needed yield makes no sense");
	
	m_aiNeededYield[eYield] = iNewValue;
}


int CvCityAI::AI_getTradeBalance(YieldTypes eYield) const
{
	FAssertMsg(eYield > NO_YIELD, "Index out of bounds");
	FAssertMsg(eYield < NUM_YIELD_TYPES, "Index out of bounds");
	
	int iAdjustment = 100 + 300 / (2 + YIELD_DISCOUNT_TURNS);
	
	return (m_aiTradeBalance[eYield] * iAdjustment) / (YIELD_DISCOUNT_TURNS * 100);
}
	
void CvCityAI::AI_changeTradeBalance(YieldTypes eYield, int iAmount)
{
	FAssertMsg(eYield > NO_YIELD, "Index out of bounds");
	FAssertMsg(eYield < NUM_YIELD_TYPES, "Index out of bounds");
	
	m_aiTradeBalance[eYield] += iAmount;
}

int CvCityAI::AI_getYieldAdvantage(YieldTypes eYield) const
{
	FAssertMsg(eYield > NO_YIELD, "Index out of bounds");
	FAssertMsg(eYield < NUM_YIELD_TYPES, "Index out of bounds");
	return m_aiYieldAdvantage[eYield];
}

void CvCityAI::AI_setYieldAdvantage(YieldTypes eYield, int iNewValue)
{
	FAssertMsg(eYield > NO_YIELD, "Index out of bounds");
	FAssertMsg(eYield < NUM_YIELD_TYPES, "Index out of bounds");
	m_aiYieldAdvantage[eYield] = iNewValue;
}

void CvCityAI::AI_assignDesiredYield()
{
	YieldTypes eBestYield = NO_YIELD;

	if (isNative())
	{
		int iBestValue = 0;
		for (int i = 0; i < NUM_YIELD_TYPES; ++i)
		{
			YieldTypes eYield = (YieldTypes) i;
			int iValue = GC.getYieldInfo(eYield).getNativeBuyPrice();
			if (iValue > 0)
			{
				if ((getYieldStored(eYield) == 0) && !canProduceYield(eYield))
				{
					iValue += 10 + GC.getYieldInfo(eYield).getNativeHappy();
					iValue *= 1 + GC.getGameINLINE().getSorenRandNum(100, "City Desired Yield");
					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						eBestYield = eYield;
					}
				}
			}
		}
	}

	if (m_eDesiredYield != eBestYield)
	{
		m_eDesiredYield = eBestYield;
		setBillboardDirty(true);

		if (eBestYield != NO_YIELD)
		{
			CvWString szMessage = gDLL->getText("TXT_KEY_DESIRED_YIELD_CHANGE", GET_PLAYER(getOwnerINLINE()).getCivilizationAdjectiveKey(), getNameKey(), GC.getYieldInfo(eBestYield).getTextKeyWide());
			for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
			{
				CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayer);
				if (kPlayer.isAlive() && kPlayer.getID() != getOwnerINLINE())
				{
					if (isScoutVisited(kPlayer.getTeam()))
					{
						gDLL->getInterfaceIFace()->addMessage((PlayerTypes) iPlayer, false, GC.getEVENT_MESSAGE_TIME(), szMessage, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(eBestYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), true, true);
					}
				}
			}
		}
	}
}

YieldTypes CvCityAI::AI_getDesiredYield() const
{
	return m_eDesiredYield;
}

void CvCityAI::AI_updateNeededYields()
{
	//This function has been updated to be invariant of the current workforce allocation.
	for (int i = 0; i < NUM_YIELD_TYPES; i++)
	{
		m_aiNeededYield[i] = 0;
	}

	for (uint i = 0; i < m_aPopulationUnits.size(); ++i)
	{
		CvUnit* pLoopUnit = m_aPopulationUnits[i];
		if (pLoopUnit != NULL)
		{
			if (pLoopUnit->isColonistLocked())
			{
				if (pLoopUnit->getProfession() != NO_PROFESSION)
				{
					// R&R, ray , MYCP partially based on code of Aymerick - START
					YieldTypes eConsumedYield = (YieldTypes)GC.getProfessionInfo(pLoopUnit->getProfession()).getYieldsConsumed(0);
					// R&R, ray , MYCP partially based on code of Aymerick - END
					if (eConsumedYield != NO_YIELD)
					{
						m_aiNeededYield[eConsumedYield] += getProfessionInput(pLoopUnit->getProfession(), pLoopUnit);					
					}
				}
			}
			else
			{
				ProfessionTypes eIdealProfession = pLoopUnit->AI_getIdealProfession();
				
				if (eIdealProfession != NO_PROFESSION && pLoopUnit->canHaveProfession(eIdealProfession, true, NULL))
				{
					// R&R, ray , MYCP partially based on code of Aymerick - START
					YieldTypes eConsumedYield = (YieldTypes)GC.getProfessionInfo(eIdealProfession).getYieldsConsumed(0);
					// R&R, ray , MYCP partially based on code of Aymerick - END
					m_aiNeededYield[eConsumedYield] += getProfessionInput(eIdealProfession, pLoopUnit);					
				}
			}
		}
	}
	
	//Now, buildings.
	for (int iI = 0; iI < GC.getNumProfessionInfos(); iI++)
	{
		ProfessionTypes eLoopProfession = (ProfessionTypes)iI;
		if (GC.getCivilizationInfo(getCivilizationType()).isValidProfession(eLoopProfession))
		{
			CvProfessionInfo& kLoopProfession = GC.getProfessionInfo(eLoopProfession);
			if (kLoopProfession.isCitizen())
			{
				if (!kLoopProfession.isWorkPlot())
				{
					// R&R, ray , MYCP partially based on code of Aymerick - START
					YieldTypes eYieldProduced = (YieldTypes)kLoopProfession.getYieldsProduced(0);
					YieldTypes eYieldConsumed = (YieldTypes)kLoopProfession.getYieldsConsumed(0);
					// R&R, ray , MYCP partially based on code of Aymerick - END	
					if (eYieldConsumed != NO_YIELD)
					{
						if (AI_getYieldAdvantage(eYieldProduced) == 100)
						{
							m_aiNeededYield[eYieldProduced] = std::max(m_aiNeededYield[eYieldProduced], getNumProfessionBuildingSlots(eLoopProfession) * getProfessionInput(eLoopProfession, NULL));
						}
					}
				}
			}
		}
	}
}

bool CvCityAI::AI_shouldImportYield(YieldTypes eYield) const
{
	if (AI_getNeededYield(eYield) > 0)
	{
		int iInput = getYieldStored(eYield) / 10;
		if (iInput < 10)
		{
			if ((iInput + getRawYieldProduced(eYield) - getRawYieldConsumed(eYield)) < AI_getNeededYield(eYield))
			{
				return true;
			}
		}
	}
	
	return false;	
}

bool CvCityAI::AI_shouldExportYield(YieldTypes eYield) const
{
	if (GET_PLAYER(getOwnerINLINE()).AI_isYieldFinalProduct(eYield))
	{
		return true;
	}

	// TAC - AI Economy - koma13 - START
	if (GET_PLAYER(getOwnerINLINE()).AI_isYieldForSale(eYield))
	{
		return true;
	}
	// TAC - AI Economy - koma13 - END

	if ((GET_PLAYER(getOwnerINLINE()).AI_shouldBuyFromEurope(eYield)) || eYield == YIELD_LUMBER || eYield == YIELD_STONE)
	{
		return false;
	}
	
	if ((getYieldStored(eYield) * 100) / GC.getGameINLINE().getCargoYieldCapacity() > 75)
	{
		if (AI_getNeededYield(eYield) == 0)
		{
			return true;
		}
		else
		{
			if (getRawYieldProduced(eYield) > getRawYieldConsumed(eYield))
			{
				return true;
			}
		}
	}
	return false;
}

int CvCityAI::AI_getTransitYield(YieldTypes eYield) const
{
	//This could(should?) be cached.
	int iTotal = 0;
	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());
	CvUnit* pLoopUnit;
	int iLoop;
	for (pLoopUnit = kOwner.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kOwner.nextUnit(&iLoop))
	{
		if (pLoopUnit->getYield() == eYield && pLoopUnit->getYieldStored() > 0)
		{
			if (AI_shouldImportYield(eYield))
			{
				FAssert(pLoopUnit->isCargo());
				CvUnit* pTransport = pLoopUnit->getTransportUnit();
				if (pTransport != NULL)
				{
					CvPlot* pMissionPlot = pTransport->getGroup()->AI_getMissionAIPlot();
					MissionAITypes eMissionAI = pTransport->getGroup()->AI_getMissionAIType();
					if ((eMissionAI == MISSIONAI_TRANSPORT) || (eMissionAI == MISSIONAI_TRANSPORT_SEA))
					{
						if (pMissionPlot == plot())
						{
							iTotal += pLoopUnit->getYieldStored();
						}
					}
				}
			}
		}
	}

	return iTotal;
}

int CvCityAI::AI_getFoodGatherable(int iPop, int iPlotFoodThreshold) const
{
	if (iPop == -1)
	{
		iPop = MAX_INT;
	}
	std::vector<int> yields;
	int iTotal = 0;
	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);
		if (pLoopPlot != NULL)
		{
			if (iI == CITY_HOME_PLOT)
			{
				iTotal += pLoopPlot->getYield(YIELD_FOOD);		
			}
			else
			{
				if ((pLoopPlot->getBonusType() == NO_BONUS) || GC.getBonusInfo(pLoopPlot->getBonusType()).getYieldChange(YIELD_FOOD) > 0)
				{
					if (canWork(pLoopPlot))
					{
						int iYield = pLoopPlot->getYield(YIELD_FOOD);
						yields.push_back(iYield);
					}
				}
			}
		}
	}
	
	std::sort(yields.begin(), yields.end(), std::greater<int>());
	for (int iI = 0; iI < (int)yields.size(); iI++)
	{
		if (iI > iPop)
		{
			break;
		}
		else
		{
			iTotal += yields[iI];
		}
	}
	return iTotal;
}

bool CvCityAI::AI_isPort() const
{
	return m_bPort;	
}

void CvCityAI::AI_setPort(bool iNewValue)
{
	m_bPort = iNewValue;
}

bool CvCityAI::AI_potentialPlot(short* piYields) const
{
	int iNetFood = piYields[YIELD_FOOD] - GC.getFOOD_CONSUMPTION_PER_POPULATION();

	if (iNetFood < 0)
	{
 		if (piYields[YIELD_FOOD] == 0)
		{
			return false;
		}
	}

	return true;
}

int CvCityAI::AI_getFoundValue()
{
	return m_iFoundValue;
}

int CvCityAI::AI_getRequiredYieldLevel(YieldTypes eYield)
{
	FAssertMsg(eYield > NO_YIELD, "Index out of bounds");
	FAssertMsg(eYield < NUM_YIELD_TYPES, "Index out of bounds");
//VET NewCapacity - begin 7/7
	// R&R, ray, improvement
	if (GC.getNEW_CAPACITY() && (getTotalYieldStored() > getMaxYieldCapacity() / 2) )
	//if (GC.getNEW_CAPACITY())
	{
		int iCargoYields = 0;
		for (int iYield = 1; iYield < NUM_YIELD_TYPES; iYield++)
		{
			if ((getYieldStored((YieldTypes)iYield) > 0) && (GC.getYieldInfo(eYield).isCargo()))
			{iCargoYields++;}
		}
		if (iCargoYields < 1)
			{iCargoYields = 1;}
		return (getMaintainLevel(eYield) / iCargoYields);
	}
//VET NewCapacity - end 7/7
	return getMaintainLevel(eYield);
}

void CvCityAI::AI_updateRequiredYieldLevels()
{
	int aiLevels[NUM_YIELD_TYPES];
	for (int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		aiLevels[iI] = 0;
	}
	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	// TAC - AI Economy - koma13 - START
	/*
	int iBestValue = 0;
	ProfessionTypes eBestProfession = NO_PROFESSION;

	for (int iI = 0; iI < GC.getNumProfessionInfos(); ++iI)
	{
		ProfessionTypes eLoopProfession = (ProfessionTypes)iI;
		CvProfessionInfo& kProfession = GC.getProfessionInfo(eLoopProfession);
		
		if (!(kProfession.isCitizen() || kProfession.isWorkPlot()))
		{
			if (GC.getCivilizationInfo(getCivilizationType()).isValidProfession(eLoopProfession))
			{
				int iValue = kPlayer.AI_professionValue(eLoopProfession, UNITAI_DEFENSIVE);
				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					eBestProfession = eLoopProfession;
				}
				for (int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
				{
					YieldTypes eYield = (YieldTypes)iI;
					
					int iRequired = GET_PLAYER(getOwnerINLINE()).getYieldEquipmentAmount(eLoopProfession, eYield);
					int iPercent = 0;
					
					if (kPlayer.AI_isStrategy(STRATEGY_REVOLUTION_PREPARING))
					{
						if (eYield == YIELD_MUSKETS || eYield == YIELD_HORSES)
						{
							iPercent = 50;
							if (kPlayer.AI_isStrategy(STRATEGY_REVOLUTION_DECLARING))
							{
								iPercent = 75;
							}
						}
					}
					iRequired = std::max(iRequired, getMaxYieldCapacity() * iPercent / 100);
					iRequired = std::min(iRequired, getMaxYieldCapacity());
					aiLevels[eYield] = std::max(aiLevels[eYield], iRequired);
				}
			}
		}
	}
	
	if (eBestProfession != NO_PROFESSION)
	{
		int iNeeded = AI_neededDefenders();
		iNeeded -= AI_numDefenders(true, false);
		
		iNeeded = std::max(1, iNeeded);
		
		for (int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
		{
			YieldTypes eYield = (YieldTypes)iI;
			
			int iRequired = GET_PLAYER(getOwnerINLINE()).getYieldEquipmentAmount(eBestProfession, eYield);
			
			aiLevels[eYield] = std::max(aiLevels[eYield], iRequired * iNeeded);
		}
	}
	*/
	
	int iPercent = 60;

	// R&R, ray, fix for new storage capacity
	if (GC.getNEW_CAPACITY())
	{
		aiLevels[YIELD_MUSKETS] = getMaxYieldCapacity() / 5 * iPercent / 100;
	}
	else
	{
		aiLevels[YIELD_MUSKETS] = getMaxYieldCapacity() * iPercent / 100;
	}

	//for (int iPass = 0; iPass < 2; ++iPass)
	{
		//BuildingTypes eBuilding = (iPass == 0) ? getProductionBuilding() : AI_bestBuildingIgnoreRequirements();
		BuildingTypes eBuilding = getProductionBuilding();
		if (eBuilding != NO_BUILDING)
		{
			CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
			for (int i = 0; i < NUM_YIELD_TYPES; ++i)
			{
				int iAmount = kBuilding.getYieldCost(i);
				aiLevels[i] = std::max(iAmount, aiLevels[i]);
			}
		}

		UnitTypes eUnit = getProductionUnit();
		if (eUnit != NO_UNIT)
		{
			CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);
			for (int i = 0; i < NUM_YIELD_TYPES; ++i)
			{
				int iAmount = kUnit.getYieldCost(i);
				aiLevels[i] = std::max(iAmount, aiLevels[i]);
			}
		}
	}
	
	/*
	if (AI_getTargetSize() > 3)
	{
		aiLevels[YIELD_LUMBER] = std::max(aiLevels[YIELD_LUMBER], getMaxYieldCapacity() / 2);		
	}
	*/
	// TAC - AI Economy - koma13 - END

	for (int i = 0; i < NUM_YIELD_TYPES; ++i)
	{
		setMaintainLevel((YieldTypes)i, std::max(getMaintainLevel((YieldTypes)i), aiLevels[i]));
	}
}

bool CvCityAI::AI_foodAvailable(int iExtra) const
{
	PROFILE_FUNC();

	CvPlot* pLoopPlot;
	bool abPlotAvailable[NUM_CITY_PLOTS];
	int iFoodCount;
	int iPopulation;
	int iBestPlot;
	int iValue;
	int iBestValue;
	int iI;

	iFoodCount = 0;

	for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		abPlotAvailable[iI] = false;
	}

	for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		pLoopPlot = getCityIndexPlot(iI);

		if (pLoopPlot != NULL)
		{
			if (iI == CITY_HOME_PLOT)
			{
				iFoodCount += pLoopPlot->calculatePotentialYield(YIELD_FOOD, NULL, false);
			}
			else if ((pLoopPlot->getWorkingCity() == this) && AI_potentialPlot(pLoopPlot->getYield()))
			{
				abPlotAvailable[iI] = true;
			}
		}
	}

	iPopulation = (getPopulation() + iExtra);

	while (iPopulation > 0)
	{
		iBestValue = 0;
		iBestPlot = CITY_HOME_PLOT;

		for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
			if (abPlotAvailable[iI])
			{
				iValue = getCityIndexPlot(iI)->calculatePotentialYield(YIELD_FOOD, NULL, false);

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					iBestPlot = iI;
				}
			}
		}

		if (iBestPlot != CITY_HOME_PLOT)
		{
			iFoodCount += iBestValue;
			abPlotAvailable[iBestPlot] = false;
		}
		else
		{
			break;
		}

		iPopulation--;
	}

	if (iFoodCount < foodConsumption(iExtra))
	{
		return false;
	}

	return true;
}


int CvCityAI::AI_yieldValue(short* piYields, bool bAvoidGrowth, bool bRemove, bool bIgnoreFood, bool bIgnoreGrowth, bool bIgnoreStarvation, bool bWorkerOptimization) const
{
	int iValue = 0;

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (piYields[iI] != 0)
		{
			iValue +=  AI_estimateYieldValue((YieldTypes)iI, piYields[iI]);
		}
	}
			
	return iValue;
}


int CvCityAI::AI_plotValue(const CvPlot* pPlot, bool bAvoidGrowth, bool bRemove, bool bIgnoreFood, bool bIgnoreGrowth, bool bIgnoreStarvation) const
{
	PROFILE_FUNC();

	short aiYields[NUM_YIELD_TYPES];
	ImprovementTypes eCurrentImprovement;
	ImprovementTypes eFinalImprovement;
	int iYieldDiff;
	int iValue;
	int iI;
	int iTotalDiff;

	iValue = 0;
	iTotalDiff = 0;

	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		aiYields[iI] = pPlot->calculatePotentialYield((YieldTypes)iI, NULL, false);
	}

	eCurrentImprovement = pPlot->getImprovementType();
	eFinalImprovement = NO_IMPROVEMENT;

	if (eCurrentImprovement != NO_IMPROVEMENT)
	{
		eFinalImprovement = finalImprovementUpgrade(eCurrentImprovement);
	}
	
	int iYieldValue = (AI_yieldValue(aiYields, bAvoidGrowth, bRemove, bIgnoreFood, bIgnoreGrowth, bIgnoreStarvation) * 100);
	if (eFinalImprovement != NO_IMPROVEMENT)
	{
		for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			iYieldDiff = (pPlot->calculateImprovementYieldChange(eFinalImprovement, ((YieldTypes)iI), getOwnerINLINE()) - pPlot->calculateImprovementYieldChange(eCurrentImprovement, ((YieldTypes)iI), getOwnerINLINE()));
			aiYields[iI] += iYieldDiff;
		}
		int iFinalYieldValue = (AI_yieldValue(aiYields, bAvoidGrowth, bRemove, bIgnoreFood, bIgnoreGrowth, bIgnoreStarvation) * 100);
		
		if (iFinalYieldValue > iYieldValue)
		{
			iYieldValue = (40 * iYieldValue + 60 * iFinalYieldValue) / 100;
		}
		else
		{
			iYieldValue = (60 * iYieldValue + 40 * iFinalYieldValue) / 100;
		}
	}
	// unless we are emph food (and also food not production)
	if (AI_getEmphasizeYieldCount(YIELD_FOOD) <= 0)
	{
		// if this plot is super bad (less than 2 food and less than combined 2 prod
		if (!AI_potentialPlot(aiYields))
		{
			// undervalue it even more!
			iYieldValue /= 16;
		}
	}
	iValue += iYieldValue;

	if (eCurrentImprovement != NO_IMPROVEMENT)
	{
		if (pPlot->getBonusType() == NO_BONUS) // XXX double-check CvGame::doFeature that the checks are the same...
		{
			for (iI = 0; iI < GC.getNumBonusInfos(); iI++)
			{
					if (GC.getImprovementInfo(eCurrentImprovement).getImprovementBonusDiscoverRand(iI) > 0)
					{
						iValue += 35;
					}
				}
			}
		}

	if ((eCurrentImprovement != NO_IMPROVEMENT) && (GC.getImprovementInfo(pPlot->getImprovementType()).getImprovementUpgrade() != NO_IMPROVEMENT))
	{
		iValue += 200;
		int iUpgradeTime = (GC.getGameINLINE().getImprovementUpgradeTime(eCurrentImprovement));
		if (iUpgradeTime > 0) //assert this?
		{
			int iUpgradePenalty = (100 * (iUpgradeTime - pPlot->getUpgradeProgress()));
			iUpgradePenalty *= (iTotalDiff * 5);
			iUpgradePenalty /= std::max(1, GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getGrowthPercent());
			iValue -= iUpgradePenalty;
		}		
	}

	return iValue;
}


int CvCityAI::AI_experienceWeight() const
{
	return ((getProductionExperience() + getDomainFreeExperience(DOMAIN_SEA)) * 2);
}


int CvCityAI::AI_plotYieldValue(const CvPlot* pPlot, int* piYields) const
{
	FAssert(piYields != NULL);
	int iValue = 0;
	
	int iBestValue = 0;
	
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
	{
		YieldTypes eYield = (YieldTypes)iJ;
		
		if (piYields[eYield] > 0)
		{
			// TAC - AI More Food - koma13 - START
			//int iTempValue = (1 + piYields[eYield]) * kOwner.AI_yieldValue(eYield);
			int iTempValue = (1 + piYields[eYield]) * kOwner.AI_yieldValue(eYield, true, 1, true);
			// TAC - AI More Food - koma13 - END
			
			bool bImportant = false;
			
			if (pPlot->isBeingWorked())
			{
				if (pPlot->getYield(eYield) > 0)
				{
					iTempValue *= 2;
					bImportant = true;
				}
			}
			
			iValue += iTempValue;
			iBestValue = std::max(iBestValue, iTempValue);
		}
	}
	iValue += iBestValue * 2;
	
	return iValue;
}

// Improved worker AI provided by Blake - thank you!
void CvCityAI::AI_bestPlotBuild(const CvPlot* pPlot, int* piBestValue, BuildTypes* peBestBuild) const
{
	PROFILE_FUNC();
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());

	if (piBestValue != NULL)
	{
		*piBestValue = 0;
	}
	if (peBestBuild != NULL)
	{
		*peBestBuild = NO_BUILD;
	}

	if (pPlot->getWorkingCity() != this)
	{
		return;
	}
	
	FAssertMsg(pPlot->getOwnerINLINE() == getOwnerINLINE(), "pPlot must be owned by this city's owner");
	
	BuildTypes eForcedBuild = NO_BUILD;
	
	{	//If a worker is already building a build, force that Build.
		CLLNode<IDInfo>* pUnitNode;
		CvUnit* pLoopUnit;

		pUnitNode = pPlot->headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = pPlot->nextUnitNode(pUnitNode);

			if (pLoopUnit->getBuildType() != NO_BUILD)
			{
				if (GC.getBuildInfo(pLoopUnit->getBuildType()).getImprovement() != NO_IMPROVEMENT)
				{
					eForcedBuild = pLoopUnit->getBuildType();
					break;
				}
			}
		}
	}
	
	int aiCurrentYields[NUM_YIELD_TYPES];
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		YieldTypes eYield = (YieldTypes)iI;
		aiCurrentYields[iI] = pPlot->calculateNatureYield(eYield, getTeam(), false);
		if (pPlot->getImprovementType() != NO_IMPROVEMENT)
		{
			aiCurrentYields[iI] += (pPlot->calculateImprovementYieldChange(pPlot->getImprovementType(), eYield, getOwnerINLINE(), false));
		}
		
		if (pPlot->getRouteType() != NO_ROUTE)
		{
			if (aiCurrentYields[iI] > 0)
			{
				aiCurrentYields[iI] += GC.getRouteInfo(pPlot->getRouteType()).getYieldChange(eYield);
			}
		}
		//Zero out particulary bad yields.
		if (aiCurrentYields[eYield] > 0)
		{
			if ((eYield == YIELD_FOOD) && aiCurrentYields[eYield] <= GC.getFOOD_CONSUMPTION_PER_POPULATION())
			{
				aiCurrentYields[eYield] = 0;
			}
			// R&R, ray, AI builds Improvements wiser - START
			//else if (eYield != YIELD_FOOD)
			else if (eYield != YIELD_FOOD && eYield != YIELD_LUMBER && eYield != YIELD_FUR)
			// R&R, ray, AI builds Improvements wiser - END
			{
				if (aiCurrentYields[eYield] <= (kOwner.AI_getBestPlotYield(eYield) / 2))
				{
					aiCurrentYields[eYield] = 0;
				}
			}
		}
	}
	
	int iCurrentValue = AI_plotYieldValue(pPlot, aiCurrentYields);

	int iBestValue = 0;
	BuildTypes eBestBuild = NO_BUILD;
	
	int aiFinalYields[NUM_YIELD_TYPES];	
	FeatureTypes eFeature = (FeatureTypes)pPlot->getFeatureType();
	ImprovementTypes eImprovement = pPlot->getImprovementType();	
	
	bool bIgnoreFeature = true;
	if (eFeature != NO_FEATURE)
	{
		CvFeatureInfo& kFeature = GC.getFeatureInfo(eFeature);
		for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
		{
			YieldTypes eYield = (YieldTypes)iYield;
			
			if (kFeature.getYieldChange(eYield) > 0)
			{
				int iYield = pPlot->calculateNatureYield(eYield, getTeam(), false);
				
				if (iYield > ((kOwner.AI_getBestPlotYield(eYield) * 2) / 3))
				{
					bIgnoreFeature = false;
					break;
				}
			}
		}
		
		if (pPlot->getBonusType() != NO_BONUS)
		{
			CvBonusInfo& kBonus = GC.getBonusInfo(pPlot->getBonusType());
			for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
			{
				YieldTypes eYield = (YieldTypes)iYield;
				
				if (kBonus.getYieldChange(eYield) > 0)
				{
					if (kFeature.getYieldChange(eYield) < 0)
					{
						bIgnoreFeature = true;
						break;
					}
					else if (kBonus.isFeature(eFeature))
					{
						bIgnoreFeature = false;
					}
				}
			}
		}
	}
	
	for (int iI = 0; iI < GC.getNumBuildInfos(); iI++)
	{
		BuildTypes eBuild = (BuildTypes)iI;
		
		CvBuildInfo& kBuild = GC.getBuildInfo(eBuild);
		bool bValid = GET_PLAYER(getOwnerINLINE()).canBuild(pPlot, eBuild, true, true);
		
		if (bValid && eForcedBuild != NO_BUILD && eForcedBuild != eBuild)
		{
			CvBuildInfo& kForcedBuild = GC.getBuildInfo(eForcedBuild);
			
			if ((kBuild.getImprovement() != NO_IMPROVEMENT) && (kForcedBuild.getImprovement() != NO_IMPROVEMENT))
			{
				bValid = false;
			}
			else if ((kBuild.getRoute() != NO_ROUTE) && (kForcedBuild.getRoute() != NO_ROUTE))
			{
				bValid = false;
			}
			else if ((eFeature != NO_FEATURE) && kBuild.isFeatureRemove(eFeature) && kForcedBuild.isFeatureRemove(eFeature))
			{
				bValid = false;
			}
		}
		
		if (bValid)
		{
			bool bCaution = false;
			
			ImprovementTypes eImprovement = (ImprovementTypes)kBuild.getImprovement();
			RouteTypes eRoute = (RouteTypes)kBuild.getRoute();
			bool bRemoveFeature = false;
			if (eFeature != NO_FEATURE)
			{
				bRemoveFeature = kBuild.isFeatureRemove(eFeature);
			}

			for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
			{
				YieldTypes eYield = (YieldTypes)iYield;
				aiFinalYields[eYield] = pPlot->getYieldWithBuild(eBuild, eYield, true);
				
				//Zero out particulary bad yields.
				if (aiFinalYields[eYield] > 0)
				{
					if ((eYield == YIELD_FOOD) && aiFinalYields[eYield] <= GC.getFOOD_CONSUMPTION_PER_POPULATION())
					{
						aiFinalYields[eYield] = 0;
					}

					// R&R, ray, AI builds Improvements wiser - START
					//else if (eYield != YIELD_FOOD)
					else if (eYield != YIELD_FOOD && eYield != YIELD_LUMBER && eYield != YIELD_FUR)
					// R&R, ray, AI builds Improvements wiser - END
					{
						if (aiFinalYields[eYield] <= (kOwner.AI_getBestPlotYield(eYield) / 2))
						{
							aiFinalYields[eYield] = 0;
						}
					}
				}
			}
			
			int iValue = AI_plotYieldValue(pPlot, aiFinalYields);
			
			if (kBuild.getRoute() != NO_ROUTE)
			{
				if (pPlot->getCrumbs() > 0)
				{
					bool bValid = true;
					for (int i = 0; i < NUM_CARDINALDIRECTION_TYPES; ++i)
					{
						CvPlot* pLoopPlot = ::plotCardinalDirection(pPlot->getX_INLINE(), pPlot->getY_INLINE(), (CardinalDirectionTypes)i);
						if (pLoopPlot != NULL && pLoopPlot->getCrumbs() > 2 * pPlot->getCrumbs())
						{
							bValid = false;
							break;
						}
					}
					if (bValid)
					{
						iValue += 100;
					}
				}
			}
			
			if ((eFeature != NO_FEATURE) && bIgnoreFeature && (eImprovement != NO_IMPROVEMENT))
			{
				CvImprovementInfo& kImprovement = GC.getImprovementInfo(eImprovement);
				if (kImprovement.getFeatureMakesValid(eFeature))
				{
					iValue /= 2;
				}
				else if (bRemoveFeature)
				{
					iValue *= 2;
					if (pPlot->getBonusType() != NO_BONUS)
					{
						iValue *= 2;
					}
				}
			}
			
			if (iValue > iCurrentValue)
			{
				if (bRemoveFeature)
				{
					bool bUnique = true;
					for (int i = 0; i < NUM_CITY_PLOTS; ++i)
					{
						CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), i);
						if (pLoopPlot != NULL && pLoopPlot != pPlot)
						{
							if (pLoopPlot->getFeatureType() == eFeature)
							{
								bUnique = false;
								break;
							}
						}
					}
					
					if (bUnique)
					{
						CvFeatureInfo& kFeature = GC.getFeatureInfo(eFeature);
						for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
						{
							YieldTypes eYield = (YieldTypes)iYield;
							if (kFeature.getYieldChange(eYield) > 0)
							{
								int iBestYield = 0;
								CvPlot* pBestYieldPlot = kOwner.AI_getBestWorkedYieldPlot(eYield);
								if (pBestYieldPlot != NULL)
								{
									iBestYield = std::max(iBestYield, pBestYieldPlot->calculateBestNatureYield(eYield, getTeam()));
								}
	
								pBestYieldPlot = kOwner.AI_getBestUnworkedYieldPlot(eYield);
								if (pBestYieldPlot != NULL)
								{
									iBestYield = std::max(iBestYield, pBestYieldPlot->calculateBestNatureYield(eYield, getTeam()));
								}
								
								if (pPlot->calculateBestNatureYield(eYield, getTeam()) >= iBestYield)
								{
									iValue *= 75;
									iValue /= 100;
								}
							}
						}
					}
				}

				if (iValue > iCurrentValue)
				{
					if (!isHuman())
					{
						if (eImprovement != NO_IMPROVEMENT)
						{
							iValue *= std::max(0, (GC.getLeaderHeadInfo(getPersonalityType()).getImprovementWeightModifier(eImprovement) + 100));
							iValue /= 100;
						}
					}
				
					if (eFeature != NO_FEATURE)
					{
						if (kBuild.isFeatureRemove(eFeature))
						{
							CvCity* pCity = NULL;
							for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
							{
								YieldTypes eYield = (YieldTypes) iYield;
								if (GC.getYieldInfo(eYield).isCargo())
								{
									iValue += pPlot->getFeatureYield(eBuild, eYield, getTeam(), &pCity) * 2;
								}
							}
							FAssert(pCity == this);
							//XXX update this once chops are saner (likely the chop yield type is defined)
							
							if (pPlot->getBonusType() != NO_BONUS)
							{
								iValue /= 2;
								//XXX Traditionally in Col, removing a feature destroys the bonus.							
							}
						}
					}
				
					if (eBuild == eForcedBuild)
					{
						iValue *= 125;
						iValue /= 100;
					}
				
					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						eBestBuild = eBuild;
					}
				}
			}
		}
	}
	
	if (eBestBuild != NO_BUILD)
	{
		CvBuildInfo& kBestBuild = GC.getBuildInfo(eBestBuild);
		if (eFeature != NO_FEATURE)
		{
			if (kBestBuild.isFeatureRemove(eFeature))
			{
				int iBestTime = kBestBuild.getTime();
				
				for (int iBuild = 0; iBuild < GC.getNumBuildInfos(); ++iBuild)
				{
					CvBuildInfo& kLoopBuild = GC.getBuildInfo((BuildTypes)iBuild);
					if (kLoopBuild.isFeatureRemove(eFeature))
					{
						if (kLoopBuild.getTime() < iBestTime)
						{
							eBestBuild = (BuildTypes)iBuild;
							iBestTime = kLoopBuild.getTime();
						}
					}
				}
			}
		}
	}
	
	if (eBestBuild != NO_BUILD)
	{
		FAssertMsg(iBestValue > 0, "iBestValue is expected to be greater than 0");
		
		if (piBestValue != NULL)
		{
			*piBestValue = iBestValue;
		}
		if (peBestBuild != NULL)
		{
			*peBestBuild = eBestBuild;
		}
	}
}

int CvCityAI::AI_cityValue() const
{
	
	AreaAITypes eAreaAI = area()->getAreaAIType(getTeam());
    if ((eAreaAI == AREAAI_OFFENSIVE) || (eAreaAI == AREAAI_MASSING) || (eAreaAI == AREAAI_DEFENSIVE))
    {
        return 0;
    }
    
	int iValue = 0;
	
	iValue += getYieldRate(YIELD_BELLS);
	iValue += getYieldRate(YIELD_CULTURE);
	iValue += getYieldRate(YIELD_HAMMERS);

	return iValue;
}

int CvCityAI::AI_calculateCulturePressure() const
{
	int iValue = 0;
	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);
		if (pLoopPlot != NULL)
		{
			if (pLoopPlot->getOwnerINLINE() == NO_PLAYER)
			{
			iValue++;
			}
			else
			{
				int iTempValue = pLoopPlot->calculateCulturePercent(getOwnerINLINE());
				if (iTempValue == 100)
				{
					//do nothing
				}
				else if ((iTempValue == 0) || (iTempValue > 75))
				{
					iValue++;
				}
				else
				{
					iTempValue = (100 - iTempValue);
					FAssert(iTempValue > 0);
					FAssert(iTempValue <= 100);

					if (iI != CITY_HOME_PLOT)
					{
						iTempValue *= 4;
						iTempValue /= NUM_CITY_PLOTS;
					}

					if ((iTempValue > 80) && (pLoopPlot->getOwnerINLINE() == getID()))
					{
						//captured territory special case
						iTempValue *= (100 - iTempValue);
						iTempValue /= 100;
					}

					if (pLoopPlot->getTeam() == getTeam())
					{
						iTempValue /= 2;
					}
					else
					{
						iTempValue *= 2;
					}

					iValue += iTempValue;
				}
			}
		}
	}

	return iValue;
}

int CvCityAI::AI_calculateWaterWorldPercent() const
{
	int iI;
	int iWaterPercent = 0;
	int iTeamCityCount = 0;
	int iOtherCityCount = 0;
	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (iI == getTeam())
			{
				iTeamCityCount += GET_TEAM((TeamTypes)iI).countNumCitiesByArea(area());
			}
			else
			{
				iOtherCityCount += GET_TEAM((TeamTypes)iI).countNumCitiesByArea(area());
			}
		}
	}

	if (iOtherCityCount == 0)
	{
		iWaterPercent = 100;
	}
	else
	{
		iWaterPercent = 100 - ((iTeamCityCount + iOtherCityCount) * 100) / std::max(1, (GC.getGame().getNumCities()));
	}

	iWaterPercent *= 50;
	iWaterPercent /= 100;

	iWaterPercent += (50 * (2 + iTeamCityCount)) / (2 + iTeamCityCount + iOtherCityCount);

	iWaterPercent = std::max(1, iWaterPercent);

	return iWaterPercent;
}

//Please note, takes the yield multiplied by 100
int CvCityAI::AI_getYieldMagicValue(const int* piYieldsTimes100) const
{
	FAssert(piYieldsTimes100 != NULL);

	int iPopEats = GC.getFOOD_CONSUMPTION_PER_POPULATION();
	iPopEats *= 100;

	int iValue = (piYieldsTimes100[YIELD_FOOD] * 100 - iPopEats * 102);
	iValue /= 100;
	return iValue;
}

//The magic value is basically "Look at this plot, is it worth working"
//-50 or lower means the plot is worthless in a "workers kill yourself" kind of way.
//-50 to -1 means the plot isn't worth growing to work - might be okay with emphasize though.
//Between 0 and 50 means it is marginal.
//50-100 means it's okay.
//Above 100 means it's definitely decent - seriously question ever not working it.
//This function deliberately doesn't use emphasize settings.
int CvCityAI::AI_getPlotMagicValue(const CvPlot* pPlot, bool bWorkerOptimization) const
{
	int aiYields[NUM_YIELD_TYPES];
	ImprovementTypes eCurrentImprovement;
	ImprovementTypes eFinalImprovement;
	int iI;
	int iYieldDiff;

	FAssert(pPlot != NULL);

	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if ((bWorkerOptimization) && (pPlot->getWorkingCity() == this) && (AI_getBestBuild(getCityPlotIndex(pPlot)) != NO_BUILD))
		{
			aiYields[iI] = pPlot->getYieldWithBuild(AI_getBestBuild(getCityPlotIndex(pPlot)), (YieldTypes)iI, true);
		}
		else
		{
			aiYields[iI] = pPlot->calculatePotentialYield((YieldTypes)iI, NULL, false) * 100;
		}
	}

	eCurrentImprovement = pPlot->getImprovementType();

	if (eCurrentImprovement != NO_IMPROVEMENT)
	{
		eFinalImprovement = finalImprovementUpgrade(eCurrentImprovement);

		if ((eFinalImprovement != NO_IMPROVEMENT) && (eFinalImprovement != eCurrentImprovement))
		{
			for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				iYieldDiff = 100 * pPlot->calculateImprovementYieldChange(eFinalImprovement, ((YieldTypes)iI), getOwnerINLINE());
				iYieldDiff -= 100 * pPlot->calculateImprovementYieldChange(eCurrentImprovement, ((YieldTypes)iI), getOwnerINLINE());
				aiYields[iI] += iYieldDiff / 2;
			}
		}
	}

	return AI_getYieldMagicValue(aiYields);
}

//useful for deciding whether or not to grow... or whether the city needs terrain
//improvement.
int CvCityAI::AI_countGoodTiles(bool bUnworkedOnly, int iThreshold, bool bWorkerOptimization) const
{
	CvPlot* pLoopPlot;
	int iI;
	int iCount;

	iCount = 0;
	for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		pLoopPlot = plotCity(getX_INLINE(),getY_INLINE(), iI);
		if ((iI != CITY_HOME_PLOT) && (pLoopPlot != NULL))
		{
			if (pLoopPlot->getWorkingCity() == this)
			{
				if (!bUnworkedOnly || !(pLoopPlot->isBeingWorked()))
				{
					if (AI_getPlotMagicValue(pLoopPlot) > iThreshold)
					{
						iCount++;
					}
				}
			}
		}
	}
	return iCount;
}

int CvCityAI::AI_calculateTargetCulturePerTurn()
{
	return 1;
}
	
// +1/+3/+5 plot based on base food yield (1/2/3)
// +4 if being worked.
// +4 if a bonus.
// Unworked ocean ranks very lowly. Unworked lake ranks at 3. Worked lake at 7.
// Worked bonus in ocean ranks at like 11
int CvCityAI::AI_buildingSpecialYieldChangeValue(BuildingTypes eBuilding, YieldTypes eYield) const
{
	int iI;
	CvPlot* pLoopPlot;
	int iValue = 0;
	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	int iWorkedCount = 0;

	int iYieldChange = kBuilding.getSeaPlotYieldChange(eYield);
	if (iYieldChange > 0)
	{
		int iWaterCount = 0;
		for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
			if (iI != CITY_HOME_PLOT)
			{
				pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);
				if ((pLoopPlot != NULL) && (pLoopPlot->getWorkingCity() == this))
				{
					if (pLoopPlot->isWater())
					{
						iWaterCount++;
						int iFood = pLoopPlot->calculatePotentialYield(YIELD_FOOD, NULL, false);
						iFood += (eYield == YIELD_FOOD) ? iYieldChange : 0;

						iValue += std::max(0, iFood * 2 - 1);
						if (pLoopPlot->isBeingWorked())
						{
							iValue += 4;
							iWorkedCount++;
						}
						iValue += ((pLoopPlot->getBonusType() != NO_BONUS) ? 8 : 0);
					}
				}
			}
		}
	}
	if (iWorkedCount == 0)
	{
		if (getPopulation() > 2)
		{
			iValue /= 2;
		}
	}

	return iValue;
}

int CvCityAI::AI_countNumBonuses(BonusTypes eBonus, bool bIncludeOurs, bool bIncludeNeutral, int iOtherCultureThreshold, bool bLand, bool bWater) const
{
	CvPlot* pLoopPlot;
	BonusTypes eLoopBonus;
	int iI;
	int iCount = 0;
	for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);
		if (pLoopPlot != NULL)
		{
			if ((pLoopPlot->area() == area()) || (bWater && pLoopPlot->isWater()))
			{
				eLoopBonus = pLoopPlot->getBonusType();
				if (eLoopBonus != NO_BONUS)
				{
					if ((eBonus == NO_BONUS) || (eBonus == eLoopBonus))
					{
						if (bIncludeOurs && (pLoopPlot->getOwnerINLINE() == getOwnerINLINE()) && (pLoopPlot->getWorkingCity() == this))
						{
							iCount++;                    
						}
						else if (bIncludeNeutral && (!pLoopPlot->isOwned()))
						{
							iCount++;
						}
						else if ((iOtherCultureThreshold > 0) && (pLoopPlot->isOwned() && (pLoopPlot->getOwnerINLINE() != getOwnerINLINE())))
						{
							if ((pLoopPlot->getCulture(pLoopPlot->getOwnerINLINE()) - pLoopPlot->getCulture(getOwnerINLINE())) < iOtherCultureThreshold)
							{
								iCount++;
							}                        
						}
					}
				}
			}
		}
	}


	return iCount;

}

int CvCityAI::AI_playerCloseness(PlayerTypes eIndex, int iMaxDistance) const
{
	FAssert(GET_PLAYER(eIndex).isAlive());
	FAssert(eIndex != getID());
	
	if ((m_iCachePlayerClosenessTurn != GC.getGame().getGameTurn())
		|| (m_iCachePlayerClosenessDistance != iMaxDistance))
	{
		AI_cachePlayerCloseness(iMaxDistance);
	}
	
	return m_aiPlayerCloseness[eIndex];
}

void CvCityAI::AI_cachePlayerCloseness(int iMaxDistance) const
{
	PROFILE_FUNC();
	CvCity* pLoopCity;
	int iI;
	int iLoop;
	int iValue;
	int iTempValue;
	int iBestValue;
	
	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && 
			((GET_TEAM(getTeam()).isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam()))))
		{
			iValue = 0;
			iBestValue = 0;
			for (pLoopCity = GET_PLAYER((PlayerTypes)iI).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iI).nextCity(&iLoop))
			{
				int iDistance = stepDistance(getX_INLINE(), getY_INLINE(), pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE());
				if (area() != pLoopCity->area())
				{
					iDistance += 1;
					iDistance /= 2;
				}
				if (iDistance <= iMaxDistance)
				{
					if (getArea() == pLoopCity->getArea())
					{
						int iPathDistance = GC.getMap().calculatePathDistance(plot(), pLoopCity->plot());
						if (iPathDistance > 0)
						{
							iDistance = iPathDistance;
						}
						else
						{

						}
					}
					if (iDistance <= iMaxDistance)
					{
						// TAC - AI City Defense - koma13 - START
						// Weight by population of both cities, not just pop of other city
						//iTempValue = 20 + 2*pLoopCity->getPopulation();
						iTempValue = 20 + pLoopCity->getPopulation() + getPopulation();

						iTempValue *= (1 + (iMaxDistance - iDistance));
						iTempValue /= (1 + iMaxDistance);
						// TAC - AI City Defense - koma13 - END

						//reduce for small islands.
						int iAreaCityCount = pLoopCity->area()->getNumCities();
						iTempValue *= std::min(iAreaCityCount, 5);
						iTempValue /= 5;
						if (iAreaCityCount < 3)
						{
							iTempValue /= 2;
						}

						iValue += iTempValue;
						iBestValue = std::max(iBestValue, iTempValue);
					}
				}
			}
			m_aiPlayerCloseness[iI] = (iBestValue + iValue / 4);
		}
	}
	
	m_iCachePlayerClosenessTurn = GC.getGame().getGameTurn();
	m_iCachePlayerClosenessDistance = iMaxDistance;
}

int CvCityAI::AI_cityThreat(bool bDangerPercent) const
{
	PROFILE_FUNC();
	int iValue = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if ((iI != getOwner()) && GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			int iTempValue = AI_playerCloseness((PlayerTypes)iI, 5);
			
			// TAC - AI City Defense - koma13 - START
			if (iTempValue > 0)
			{
				if (atWar(getTeam(), GET_PLAYER((PlayerTypes)iI).getTeam()))
				{
					iTempValue *= 300;
				}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      01/04/09                                jdog5000      */
/*                                                                                              */
/* War tactics AI                                                                               */
/************************************************************************************************/
				// Beef up border security before starting war, but not too much
				else if ( GET_TEAM(getTeam()).AI_getWarPlan(GET_PLAYER((PlayerTypes)iI).getTeam()) != NO_WARPLAN )
				{
					iTempValue *= 180;
				}
				// Extra trust of Vassals, regardless of relations
//				else if ( GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isVassal(getTeam()) )
//				{
//					iTempValue *= 30;
//				}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/				
				else
				{
					switch (GET_PLAYER(getOwnerINLINE()).AI_getAttitude((PlayerTypes)iI))
					{
					case ATTITUDE_FURIOUS:
						iTempValue *= 180;
						break;

					case ATTITUDE_ANNOYED:
						iTempValue *= 130;
						break;

					case ATTITUDE_CAUTIOUS:
						iTempValue *= 100;
						break;

					case ATTITUDE_PLEASED:
						iTempValue *= 50;
						break;

					case ATTITUDE_FRIENDLY:
						iTempValue *= 20;
						break;

					default:
						FAssert(false);
						break;
					}
				}
				
				iTempValue /= 100;
				iValue += iTempValue;
			}
			// TAC - AI City Defense - koma13 - END
		}
	}
	
	if (isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
	{
		iValue += 6;
	}
	
	iValue += 2 * GET_PLAYER(getOwnerINLINE()).AI_getPlotDanger(plot(), 3, false);
	
	return iValue;
}

//Workers have/needed is not intended to be a strict
//target but rather an indication.
//if needed is at least 1 that means a worker
//will be doing something useful
int CvCityAI::AI_getWorkersHave() const
{
	return m_iWorkersHave;
}

int CvCityAI::AI_getWorkersNeeded() const
{
	return m_iWorkersNeeded;
}

void CvCityAI::AI_changeWorkersHave(int iChange)
{
	m_iWorkersHave += iChange;
	m_iWorkersHave = std::max(0, m_iWorkersHave);
}
	
//This needs to be serialized for human workers.
void CvCityAI::AI_updateWorkersNeededHere()
{
	CvPlot* pLoopPlot;
	
	int iWorkedUnimprovedCount = 0;
	int iUnimprovedBonusCount = 0;
	
	int iValue = 0;
	
	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		pLoopPlot = getCityIndexPlot(iI);

		if (NULL != pLoopPlot && pLoopPlot->getWorkingCity() == this)
		{
			if (pLoopPlot->getArea() == getArea())
			{
				if (iI != CITY_HOME_PLOT)
				{
					if (AI_getBestBuild(iI) != NO_BUILD)
					{
						if (pLoopPlot->isBeingWorked())
						{
							iValue += 40;
							if (pLoopPlot->getBonusType() != NO_BONUS)
							{
								iValue += 60;
							}
						}
						else
						{
							if (pLoopPlot->getBonusType() != NO_BONUS)
							{
								iValue += 50;
							}
						}
					}
				}
			}
		}
	}
	
	if (iValue == 0)
	{
		m_iWorkersNeeded = 0;
	}
	else
	{
		m_iWorkersNeeded = std::max(1, iValue / 100);
	}
}

BuildingTypes CvCityAI::AI_bestAdvancedStartBuilding(int iPass) const
{
	return AI_bestBuildingThreshold(0, 0, std::max(0, 20 - iPass * 5));
}

void CvCityAI::AI_educateStudent(int iUnitId)
{
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	UnitTypes eBestUnit = NO_UNIT;
	int iBestValue = 0;
	for (int i = 0; i < GC.getNumUnitInfos(); ++i)
	{
		int iTuition = getSpecialistTuition((UnitTypes) i);
		if (GET_PLAYER(getOwnerINLINE()).getGold() >= iTuition && iTuition >= 0)
		{
			UnitTypes eLoopUnit = (UnitTypes)i;
			CvUnitInfo& kUnit = GC.getUnitInfo(eLoopUnit);

			UnitClassTypes eUnitClassType = (UnitClassTypes)kUnit.getUnitClassType(); // R&R, ray, changed

			int iValue = 50;
			
			int iNeeded = kOwner.AI_getNumCityUnitsNeeded(eLoopUnit);
			int iHave = kOwner.getUnitClassCount(eUnitClassType); // R&R, ray, changed
			
			if (iNeeded < iHave)
			{
				iValue += 100 + 50 * (iNeeded - iHave);
				
				iValue *= 100 + kUnit.getYieldModifier(YIELD_BELLS);
				iValue /= 100;
				
				iValue *= 100 + (3 * kUnit.getYieldModifier(YIELD_HAMMERS) / 2);
				iValue /= 100;
			}
			else
			{
				ProfessionTypes eIdealProfession = kOwner.AI_idealProfessionForUnit(eLoopUnit);
				if (eIdealProfession != NO_PROFESSION)
				{
					// R&R, ray , MYCP partially based on code of Aymerick - START
					YieldTypes eYieldProducedType = (YieldTypes)GC.getProfessionInfo(eIdealProfession).getYieldsProduced(0);
					// R&R, ray , MYCP partially based on code of Aymerick - END
					if (eYieldProducedType != NO_YIELD)
					{
						if (eYieldProducedType == YIELD_FOOD)
						{
							iValue *= 2; // R&R, ray, changed
						}
					}
				}
			}
			
			iValue *= 40 + GC.getGameINLINE().getSorenRandNum(60, "AI best educate unit");
			
			// TAC - AI Military Buildup - koma13 - START
			if ((iTuition == 0) || !kOwner.AI_isStrategy(STRATEGY_MILITARY_BUILDUP))
			{
				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					eBestUnit = eLoopUnit;
				}
			}
			// TAC - AI Military Buildup - koma13 - END
		}
	}

	if (eBestUnit != NO_UNIT)
	{
		educateStudent(iUnitId, eBestUnit);
	}
}

//This suppresses certain checks to all the workforce allocation algorithm to run
//more smoothly (ignore time-consuming checks and swaps)
void CvCityAI::AI_setWorkforceHack(bool bNewValue)
{
	m_iWorkforceHack += (bNewValue ? 1 : -1);
}

bool CvCityAI::AI_isWorkforceHack()
{
	return (m_iWorkforceHack > 0);
}


bool CvCityAI::AI_isMajorCity() const
{
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		if (AI_getYieldAdvantage((YieldTypes)iYield) >= 100)
		{
			return true;
		}
	}
	
	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());
	int iHigherPopulationCount = 0;
	
	int iLoop;
	for (CvCity* pLoopCity = kOwner.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kOwner.nextCity(&iLoop))
	{
		if (pLoopCity != this)
		{
			if (pLoopCity->getPopulation() > getPopulation())
			{
				iHigherPopulationCount++;
			}
			else if (pLoopCity->getPopulation() == getPopulation())
			{
				if (pLoopCity->getGameTurnAcquired() < getGameTurnAcquired())
				{
					iHigherPopulationCount++;
				}
			}
		}
	}
	
	if (100 * iHigherPopulationCount / kOwner.getNumCities() <= 20)
	{
		return true;
	}
	
	return false;
}


// Choose the yield with the highest export value for the city plot 
// TODO: Extend this by considering deficit input yields for slotworkers
void CvCityAI::AI_assignCityPlot()
{
	// Natives are not supported yet
	if (isNative())
		return;

	int iBestValue = 0;
	YieldTypes eBestYield = NO_YIELD;

	const CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());

	for (uint i = 0; i < NUM_YIELD_TYPES; i++)
	{
		const YieldTypes eYield = (YieldTypes)i;
		const int iYield = plot()->calculatePotentialCityYield(eYield, this);

		if (iYield > 0)
		{
			const int iValue = kPlayer.AI_getYieldBestExportPrice(eYield) * iYield;

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestYield = eYield;
			}

		}
	}

	if (eBestYield != NO_YIELD)
	{
		setPreferredYieldAtCityPlot(eBestYield);
	}
}


//
//
//
void CvCityAI::read(FDataStreamBase* pStream)
{
	CvCity::read(pStream);

	uint uiFlag=0;
	pStream->Read(&uiFlag);	// flags for expansion

	pStream->Read(&m_iGiftTimer);
	pStream->Read(&m_iTradeTimer); // R&R, ray, Natives Trading - START
	pStream->Read((int*)&m_eDesiredYield);
	pStream->Read(&m_iTargetSize);
	pStream->Read(&m_iFoundValue);
	
	pStream->Read(NUM_YIELD_TYPES, m_aiYieldOutputWeight);
	pStream->Read(NUM_YIELD_TYPES, m_aiNeededYield);
	pStream->Read(NUM_YIELD_TYPES, m_aiTradeBalance);
	pStream->Read(NUM_YIELD_TYPES, m_aiYieldAdvantage);

	pStream->Read(&m_iEmphasizeAvoidGrowthCount);
	
	pStream->Read(&m_bPort);
	pStream->Read(&m_bAssignWorkDirty);
	pStream->Read(&m_bChooseProductionDirty);

	m_routeToCity.read(pStream);
	
	pStream->Read(NUM_YIELD_TYPES, m_aiEmphasizeYieldCount);
	pStream->Read(&m_bForceEmphasizeCulture);
	pStream->Read(NUM_CITY_PLOTS, m_aiBestBuildValue);
	pStream->Read(NUM_CITY_PLOTS, (int*)m_aeBestBuild);
	pStream->Read(GC.getNumEmphasizeInfos(), m_abEmphasize);
	pStream->Read(&m_iCachePlayerClosenessTurn);
	pStream->Read(&m_iCachePlayerClosenessDistance);
	pStream->Read(MAX_PLAYERS, m_aiPlayerCloseness);
	pStream->Read(&m_iNeededFloatingDefenders);
	pStream->Read(&m_iNeededFloatingDefendersCacheTurn);
	pStream->Read(&m_iWorkersNeeded);
	pStream->Read(&m_iWorkersHave);
}

//
//
//
void CvCityAI::write(FDataStreamBase* pStream)
{
	CvCity::write(pStream);

	uint uiFlag=0;
	pStream->Write(uiFlag);		// flag for expansion

	pStream->Write(m_iGiftTimer);
	pStream->Write(m_iTradeTimer); // R&R, ray, Natives Trading - START
	pStream->Write(m_eDesiredYield);
	pStream->Write(m_iTargetSize);
	pStream->Write(m_iFoundValue);
	
	pStream->Write(NUM_YIELD_TYPES, m_aiYieldOutputWeight);
	pStream->Write(NUM_YIELD_TYPES, m_aiNeededYield);
	pStream->Write(NUM_YIELD_TYPES, m_aiTradeBalance);
	pStream->Write(NUM_YIELD_TYPES, m_aiYieldAdvantage);

	pStream->Write(m_iEmphasizeAvoidGrowthCount);
	
	pStream->Write(m_bPort);
	pStream->Write(m_bAssignWorkDirty);
	pStream->Write(m_bChooseProductionDirty);

	m_routeToCity.write(pStream);

	pStream->Write(NUM_YIELD_TYPES, m_aiEmphasizeYieldCount);
	pStream->Write(m_bForceEmphasizeCulture);
	pStream->Write(NUM_CITY_PLOTS, m_aiBestBuildValue);
	pStream->Write(NUM_CITY_PLOTS, (int*)m_aeBestBuild);
	pStream->Write(GC.getNumEmphasizeInfos(), m_abEmphasize);
	pStream->Write(m_iCachePlayerClosenessTurn);
	pStream->Write(m_iCachePlayerClosenessDistance);
	pStream->Write(MAX_PLAYERS, m_aiPlayerCloseness);
	pStream->Write(m_iNeededFloatingDefenders);
	pStream->Write(m_iNeededFloatingDefendersCacheTurn);
	pStream->Write(m_iWorkersNeeded);
	pStream->Write(m_iWorkersHave);
}
