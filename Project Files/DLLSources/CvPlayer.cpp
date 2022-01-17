// player.cpp

#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "CvArea.h"
#include "CvGameAI.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvRandom.h"
#include "CvTeamAI.h"
#include "CvGameCoreUtils.h"
#include "CvPlayerAI.h"
#include "CvPlayer.h"
#include "CvGameCoreUtils.h"
#include "CvArtFileMgr.h"
#include "CvDiploParameters.h"
#include "CvInitCore.h"
#include "CyArgsList.h"
#include "CvInfos.h"
#include "CvPopupInfo.h"
#include "CvDiploParameters.h"
#include "FProfiler.h"
#include "CvGameTextMgr.h"
#include "CyCity.h"
#include "CyPlot.h"
#include "CvTradeRoute.h"
#include "CvTradeRouteGroup.h" //R&R mod, vetiarvind, trade groups
#include <numeric>
#include <algorithm>

#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLEntityIFaceBase.h"
#include "CvDLLEngineIFaceBase.h"
#include "CvDLLFAStarIFaceBase.h"
#include "CvDLLEventReporterIFaceBase.h"
#include "CvDLLPythonIFaceBase.h"

#include "CvSavegame.h"


// Public Functions...

CvPlayer::CvPlayer()
{
	// R&R, ray, Bargaining - START
	m_bWillingToBargain = false;
	m_iTimeNoTrade = 0;
	// R&R, ray, Bargaining - END

	// R&R, ray, Timers Diplo Events - START
	m_iTimerNativeMerc = 0;
	m_iTimerEuropeanWars = 0;
	m_iTimerEuropeanPeace = 0;
	m_iTimerPrisonsCrowded = 0;
	m_iTimerRevolutionaryNoble = 0;
	m_iTimerBishop = 0;
	m_iTimerChurchDemand = 0;
	m_iTimerChurchWar = 0;
	m_iTimerSmugglingShip = 0;
	m_iTimerRanger = 0;
	m_iTimerConquistador = 0;
	m_iTimerPirates = 0;
	m_iTimerContinentalGuard = 0;
	m_iTimerMortar = 0;
	m_iTimerNativeSlave = 0;
	m_iTimerAfricanSlaves = 0;
	m_iTimerStealingImmigrant = 0;
	// R&R, ray, Timers Diplo Events - END

	m_iChurchFavoursReceived = 0; // R&R, ray, Church Favours

	// cache CvPlayer::getYieldEquipmentAmount - start - Nightinggale
	m_cache_YieldEquipmentAmount = new YieldArray<unsigned short>[GC.getNumProfessionInfos()];
	// cache CvPlayer::getYieldEquipmentAmount - end - Nightinggale
	reset(NO_PLAYER, true);
}


CvPlayer::~CvPlayer()
{
	uninit();
	// cache CvPlayer::getYieldEquipmentAmount - start - Nightinggale
	if (m_cache_YieldEquipmentAmount != NULL)
	{
		for (int iProfession = 0; iProfession < GC.getNumProfessionInfos(); iProfession++) {
			m_cache_YieldEquipmentAmount[iProfession].reset();
		}
		SAFE_DELETE_ARRAY(m_cache_YieldEquipmentAmount);
	}
	// cache CvPlayer::getYieldEquipmentAmount - end - Nightinggale
}


void CvPlayer::init(PlayerTypes eID)
{
	//--------------------------------
	// Init saved data
	reset(eID);

	// set the CivEffect cache
	CivEffect()->rebuildCivEffectCache();

    /** NBMOD TAX **/
    m_iMaxTaxRate = GC.getHandicapInfo(getHandicapType()).NBMOD_GetInitMaxTaxRate();

    /** NBMOD TAX **/

	//assign europe civilization as parent
	for (int iParent = 0; iParent < MAX_PLAYERS; ++iParent)
	{
		CvPlayer& kParent = GET_PLAYER((PlayerTypes) iParent);
		if(kParent.getCivilizationType() != NO_CIVILIZATION)
		{
			if(GC.getCivilizationInfo(kParent.getCivilizationType()).getDerivativeCiv() == getCivilizationType())
			{
				setParent((PlayerTypes) iParent);
				break;
			}
		}
	}

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		YieldTypes eYield = (YieldTypes) iYield;
		CvYieldInfo& kYield = GC.getYieldInfo(eYield);

		FAssert(kYield.getBuyPriceHigh() >= kYield.getBuyPriceLow());

		int iBuyPrice = kYield.getBuyPriceLow() + GC.getGameINLINE().getSorenRandNum(kYield.getBuyPriceHigh() - kYield.getBuyPriceLow() + 1, "Yield Price");
		setYieldBuyPrice(eYield, iBuyPrice, false);
	}

	// R&R, ray, Africa
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		YieldTypes eYield = (YieldTypes) iYield;
		CvYieldInfo& kYield = GC.getYieldInfo(eYield);

		FAssert(kYield.getAfricaBuyPriceHigh() >= kYield.getAfricaBuyPriceLow());

		int iAfricaBuyPrice = kYield.getAfricaBuyPriceLow() + GC.getGameINLINE().getSorenRandNum(kYield.getAfricaBuyPriceHigh() - kYield.getAfricaBuyPriceLow() + 1, "Yield Price");
		setYieldAfricaBuyPrice(eYield, iAfricaBuyPrice, false);
	}
	// R&R, ray, Africa - END
	
	// R&R, ray, Port Royal
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		YieldTypes eYield = (YieldTypes) iYield;
		CvYieldInfo& kYield = GC.getYieldInfo(eYield);

		FAssert(kYield.getPortRoyalBuyPriceHigh() >= kYield.getPortRoyalBuyPriceLow());

		int iPortRoyalBuyPrice = kYield.getPortRoyalBuyPriceLow() + GC.getGameINLINE().getSorenRandNum(kYield.getPortRoyalBuyPriceHigh() - kYield.getPortRoyalBuyPriceLow() + 1, "Yield Price");
		setYieldPortRoyalBuyPrice(eYield, iPortRoyalBuyPrice, false);
	}
	// R&R, ray, Port Royal - END

	//--------------------------------
	// Init containers
	m_cities.init();
	m_aTradeGroups.reset(); //R&R mod, vetiarvind, trade groups
	m_tradeRoutes.reset();
	m_units.reset();

	freeEuropeUnits();
	freeAfricaUnits(); /*** TRIANGLETRADE 10/23/08 by DPII ***/
	freePortRoyalUnits(); // R&R, ray, Port Royal

	m_selectionGroups.init();

	m_eventsTriggered.init();

	m_aDocksNextUnits.clear();

	//--------------------------------
	// Init non-saved data
	setupGraphical();

	//--------------------------------
	// Init other game data
	FAssert(getTeam() != NO_TEAM);
	GET_TEAM(getTeam()).changeNumMembers(1);

	if ((GC.getInitCore().getSlotStatus(getID()) == SS_TAKEN) || (GC.getInitCore().getSlotStatus(getID()) == SS_COMPUTER))
	{
		setAlive(true);

		//ray, fixing bug of player initialization braking traits impacting relations - START
		//code moved to initDiplomacy()
		/*
		FAssertMsg((GC.getNumTraitInfos() > 0), "GC.getNumTraitInfos() is less than or equal to zero but is expected to be larger than zero in CvPlayer::init");
		for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
		{
			if (GC.getCivilizationInfo(getCivilizationType()).hasTrait(iI))
			{
				processTrait((TraitTypes) iI, 1);
			}

			if (GC.getLeaderHeadInfo(getLeaderType()).hasTrait(iI))
			{
				processTrait((TraitTypes) iI, 1);
			}
		}

		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			updateExtraYieldThreshold((YieldTypes)iI);
			updateCityExtraYield((YieldTypes) iI);
		}
		*/
		//ray, fixing bug of player initialization braking traits impacting relations - END

		for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
		{
			setCivic(((CivicOptionTypes)iI), ((CivicTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationInitialCivics(iI))));
		}

		for (int iI = 0; iI < GC.getNumEventInfos(); iI++)
		{
			resetEventOccured((EventTypes)iI, false);
		}

		for (int iI = 0; iI < GC.getNumEventTriggerInfos(); iI++)
		{
			resetTriggerFired((EventTriggerTypes)iI);
		}
	}

	AI_init();

	Update_cache_YieldEquipmentAmount(); // cache CvPlayer::getYieldEquipmentAmount - Nightinggale
}


void CvPlayer::uninit()
{
	m_triggersFired.clear();

	// PatchMod: Achievements START
	m_achievesGained.clear();
	m_achievesTurn.clear();
	// PatchMod: Achievements END

	m_groupCycle.clear();
	m_aszCityNames.clear();

	m_cities.uninit();

	m_tradeRoutes.reset();
	m_aTradeGroups.reset(); //R&R mod, vetiarvind, trade groups
	m_units.reset();

	freeEuropeUnits();
	freeAfricaUnits(); /*** TRIANGLETRADE 10/23/08 by DPII ***/
	freePortRoyalUnits(); // R&R, ray, Port Royal

	m_selectionGroups.uninit();

	m_eventsTriggered.uninit();

	clearMessages();

	clearPopups();

	clearDiplomacy();
}


// FUNCTION: reset()
// Initializes data members that are serialized.
void CvPlayer::reset(PlayerTypes eID, bool bConstructorCall)
{
	//--------------------------------
	// Uninit class
	uninit();
	resetSavedData(eID,bConstructorCall);


    /** NBMOD REF **/
    m_bNBMOD_REF_Display = true;
    m_iNBMOD_REF_DisplayTurn = 0;
    /** NBMOD REF **/

	// Dale - AoD: AI Autoplay START
	m_bDisableHuman = false;
	// Dale - AoD: AI Autoplay END
	
	m_uiStartTime = 0;

	m_aszTradeMessages.clear();

	// TAC - Trade Messages - koma13 - START
	m_aeTradeMessageTypes.clear();
	m_aeTradeMessageYields.clear();
	m_aiTradeMessageAmounts.clear();
	m_aiTradeMessageCommissions.clear();
	// TAC - Trade Messages - koma13 - END

	if (!bConstructorCall)
	{
		AI_reset();
	}
	Update_cache_YieldEquipmentAmount(); // cache CvPlayer::getYieldEquipmentAmount - Nightinggale
}


//////////////////////////////////////
// graphical only setup
//////////////////////////////////////
void CvPlayer::setupGraphical()
{
	if (!GC.IsGraphicsInitialized())
		return;

	CvCity* pLoopCity;
	CvUnit* pLoopUnit;

	// Setup m_cities
	int iLoop;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->setupGraphical();
	}

	// Setup m_units
	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit->plot() != NULL)
		{
			pLoopUnit->setupGraphical();
		}
	}
}


void CvPlayer::initFreeState()
{
	setGold(0);
	// Barthoze : Time is the issue - START
	int iStartingGold = GC.getHandicapInfo(getHandicapType()).getStartingGold();
	iStartingGold += GC.getEraInfo(GC.getGameINLINE().getStartEra()).getStartingGold();

	// Gives an appropriate sum of money to trade with Natives, which depends on cargo size.
	iStartingGold *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getStoragePercent(); 
	iStartingGold /= 100;
	
	changeGold(iStartingGold);
    // Barthoze : Time is the issue - END
}


void CvPlayer::initFreeUnits()
{
	UnitTypes eLoopUnit;
	int iFreeCount;

	if ((GC.getGameINLINE().isOption(GAMEOPTION_ADVANCED_START) || GC.getCivilizationInfo(getCivilizationType()).getAdvancedStartPoints() > 0) && !GC.getCivilizationInfo(getCivilizationType()).isEurope())
	{
		int iPoints = GC.getCivilizationInfo(getCivilizationType()).getAdvancedStartPoints();

		if (NO_WORLDSIZE != GC.getInitCore().getWorldSize())
		{
			iPoints *= GC.getWorldInfo(GC.getInitCore().getWorldSize()).getAdvancedStartPointsMod();
			iPoints /= 100;
		}

		if (NO_GAMESPEED != GC.getInitCore().getGameSpeed())
		{
			iPoints *= GC.getGameSpeedInfo(GC.getInitCore().getGameSpeed()).getGrowthPercent();
			iPoints /= 100;
		}

		if (GC.getGameINLINE().isOption(GAMEOPTION_ADVANCED_START))
		{
			iPoints += GC.getGameINLINE().getNumAdvancedStartPoints();
		}

		iPoints *= GC.getHandicapInfo(getHandicapType()).getAdvancedStartPointsMod();
		iPoints /= 100;

		if (!isHuman())
		{
			iPoints *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIAdvancedStartPercent();
			iPoints /= 100;
		}

		setAdvancedStartPoints(iPoints);

		// Starting visibility
		CvPlot* pStartingPlot = getStartingPlot();
		if (NULL != pStartingPlot)
		{
			for (int iPlotLoop = 0; iPlotLoop < GC.getMapINLINE().numPlots(); ++iPlotLoop)
			{
				CvPlot* pPlot = GC.getMapINLINE().plotByIndex(iPlotLoop);

				if (plotDistance(pPlot->getX_INLINE(), pPlot->getY_INLINE(), pStartingPlot->getX_INLINE(), pStartingPlot->getY_INLINE()) <= GC.getDefineINT("ADVANCED_START_SIGHT_RANGE"))
				{
					pPlot->setRevealed(getTeam(), true, false, NO_TEAM);
				}
			}
		}

		if (!GC.getGameINLINE().isOption(GAMEOPTION_ADVANCED_START) && !isHuman() && (getAdvancedStartPoints() >= 0))
		{
			AI_doAdvancedStart();
		}
	}
	else
	{
		CvCivilizationInfo& kCivilizationInfo = GC.getCivilizationInfo(getCivilizationType());
		for (int iI = 0; iI < kCivilizationInfo.getNumCivilizationFreeUnits(); iI++)
		{
			int iLoopUnitClass = kCivilizationInfo.getCivilizationFreeUnitsClass(iI);
			ProfessionTypes eLoopUnitProfession = (ProfessionTypes) kCivilizationInfo.getCivilizationFreeUnitsProfession(iI);
			eLoopUnit = (UnitTypes)kCivilizationInfo.getCivilizationUnits(iLoopUnitClass);

			if (eLoopUnit != NO_UNIT)
			{
				iFreeCount = (GC.getEraInfo(GC.getGameINLINE().getStartEra()).getStartingUnitMultiplier() + ((!isHuman()) ? GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIStartingUnitMultiplier() : 0));

				for (int iJ = 0; iJ < iFreeCount; iJ++)
				{
					//add revolution units to children
					if (GC.getCivilizationInfo(getCivilizationType()).isEurope())
					{
						for (int iPlayer=0;iPlayer<MAX_PLAYERS;iPlayer++)
						{
							CvPlayer& kChild = GET_PLAYER((PlayerTypes) iPlayer);
							if(kChild.isAlive() && (kChild.getParent() == getID()))
							{
								kChild.addRevolutionEuropeUnit(eLoopUnit, eLoopUnitProfession);
							}
						}
					}
					else
					{
						CvUnit* pUnit = addFreeUnit(eLoopUnit, eLoopUnitProfession);
					}
				}
			}
		}

		/** NBMOD REF **/

		if (GC.getNBMOD_REF_ENABLE() == 1)
		{
			if (GC.getCivilizationInfo(getCivilizationType()).isEurope())
			{
				for (int iPlayer=0;iPlayer<MAX_PLAYERS;iPlayer++)
				{
					CvPlayer& kChild = GET_PLAYER((PlayerTypes) iPlayer);
					if(kChild.isAlive() && (kChild.getParent() == getID()))
					{
						// Start-KMW Berechnen
						int iKMW = kChild.NBMOD_REF_MakeStartValue();
						while(kChild.NBMOD_GetEuropeMilitaryValue() < iKMW)
						{
							kChild.NBMOD_AddEuropeRandomUnit(false); // Einheit hinzuf�gen
						}
						int iTransporting = kChild.NBMOD_GetNumEuropeTransporting(); // Wie viel Transportkapazit�t steht dem K�nig zur Verf�gung?
						int iNumUnits = kChild.NBMOD_GetNumEuropeUnits(); // Wie viele Landeinheiten besitzt der K�nig?

						// Errechnet wie vile Einheiten mit einer Welle zur Kolonie gelangen sollen
						iNumUnits = iNumUnits * GC.getNBMOD_REF_NUM_UNITS_PERCENT() / 100;

						// Ist die Anzahl der zu transportierenden Einheiten gr��er als die Transportkapazit�t?
						while(iNumUnits > iTransporting)
						{
							// Dann ein Schiff aufr�sten
							kChild.NBMOD_AddEuropeShipUnit(false);
							iTransporting = kChild.NBMOD_GetNumEuropeTransporting(); // Wie viel Transportkapazit�t steht dem K�nig zur Verf�gung?
						}

					}
				}
			}
		}

		/** NBMOD REF **/

		iFreeCount = GC.getEraInfo(GC.getGameINLINE().getStartEra()).getStartingDefenseUnits();
		iFreeCount += GC.getHandicapInfo(getHandicapType()).getStartingDefenseUnits();

		if (!isHuman())
		{
			iFreeCount += GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIStartingDefenseUnits();
		}

		if (iFreeCount > 0)
		{
			addFreeUnitAI(UNITAI_DEFENSIVE, iFreeCount);
		}

		iFreeCount = GC.getEraInfo(GC.getGameINLINE().getStartEra()).getStartingWorkerUnits();
		iFreeCount += GC.getHandicapInfo(getHandicapType()).getStartingWorkerUnits();

		if (!isHuman())
		{
			iFreeCount += GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIStartingWorkerUnits();
		}

		if (iFreeCount > 0)
		{
			addFreeUnitAI(UNITAI_SETTLER, iFreeCount);
		}

		iFreeCount = GC.getEraInfo(GC.getGameINLINE().getStartEra()).getStartingExploreUnits();
		iFreeCount += GC.getHandicapInfo(getHandicapType()).getStartingExploreUnits();

		if (!isHuman())
		{
			iFreeCount += GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIStartingExploreUnits();
		}

		if (iFreeCount > 0)
		{
			addFreeUnitAI(UNITAI_SCOUT, iFreeCount);
		}
	}
}

void CvPlayer::initImmigration()
{
	FAssert(getParent() != NO_PLAYER);
	m_aDocksNextUnits.clear();
	m_aDocksNextUnits.reserve(CivEffect()->getNumUnitsOnDock());
	verifyImmigration();
}

// Make sure the number of units on the dock matches what the player is supposed to have by add/remove units.
// Also replace any unit, which is no longer able to be available on the dock.
void CvPlayer::verifyImmigration()
{
	FAssert(!this->isEurope());
	FAssert(!this->isNative());

	// remove units, which should no longer be there.
	for (unsigned int i = 0; i < m_aDocksNextUnits.size(); ++i)
	{
		UnitTypes eUnit = m_aDocksNextUnits[i];
		if (eUnit != NO_UNIT && (!CivEffect()->canUseUnit(eUnit) || !CivEffect()->canUseImmigrant(eUnit)))
		{
			if (eUnit != NO_UNIT && (!CivEffect()->canUseUnit(eUnit) || !CivEffect()->canUseImmigrant(eUnit)))
			{
				m_aDocksNextUnits.erase(m_aDocksNextUnits.begin() + i);
				--i; // compensate for ++i as next iteration needs to use the same value for i
			}
		}
	}

	if (m_aDocksNextUnits.size() > CivEffect()->getNumUnitsOnDock())
	{
		// Too many units on the dock.
		// Remove from the right to match the number of units requested.
		m_aDocksNextUnits.resize(CivEffect()->getNumUnitsOnDock());
	}
	else
	{
		// Add units until the requested amount of units have appeared.
		while (m_aDocksNextUnits.size() < CivEffect()->getNumUnitsOnDock())
		{
			m_aDocksNextUnits.push_back(pickBestImmigrant());
		}
	}
}


void CvPlayer::addFreeUnitAI(UnitAITypes eUnitAI, int iCount)
{
	UnitTypes eBestUnit = NO_UNIT;
	ProfessionTypes eBestProfession = NO_PROFESSION;
	int iBestValue = 0;

	for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		UnitTypes eLoopUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI);

		if (eLoopUnit != NO_UNIT)
		{
			if (canTrain(eLoopUnit))
			{
				int iValue = AI_unitValue(eLoopUnit, eUnitAI, NULL);

				if (iValue > iBestValue)
				{
					eBestUnit = eLoopUnit;
					iBestValue = iValue;
					eBestProfession = GC.getUnitInfo(eLoopUnit).getDefaultProfession();
				}
			}
		}
	}

	if (eBestUnit != NO_UNIT)
	{
		for (int iI = 0; iI < iCount; iI++)
		{
			addFreeUnit(eBestUnit, eBestProfession, eUnitAI);
		}
	}
}


CvUnit* CvPlayer::addFreeUnit(UnitTypes eUnit, ProfessionTypes eProfession, UnitAITypes eUnitAI)
{
	CvPlot* pStartingPlot = getStartingPlot();
	if (pStartingPlot != NULL)
	{		
		pStartingPlot->setImprovementType(NO_IMPROVEMENT);//R&R, ray, bugfix for starting on Water Goody
		CvUnit* pUnit = initUnit(eUnit, eProfession, pStartingPlot->getX_INLINE(), pStartingPlot->getY_INLINE(), eUnitAI);
		return pUnit;
	}

	return NULL;
}


int CvPlayer::startingPlotRange() const
{
	int iRange;

	// PatchMod: Spread out start locs START
	iRange = (GC.getMapINLINE().maxStepDistance() + 60);
//	iRange = (GC.getMapINLINE().maxStepDistance() + 40);
	// PatchMod: Spread out start locs END

	iRange *= GC.getDefineINT("STARTING_DISTANCE_PERCENT");
	iRange /= 100;

	iRange *= (GC.getMapINLINE().getLandPlots() / (GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getTargetNumCities() * GC.getGameINLINE().countCivPlayersAlive()));
	iRange /= NUM_CITY_PLOTS;

	iRange += std::min(((GC.getMapINLINE().getNumAreas() + 1) / 2), GC.getGameINLINE().countCivPlayersAlive());

	long lResult=0;
	if (gDLL->getPythonIFace()->callFunction(gDLL->getPythonIFace()->getMapScriptModule(), "minStartingDistanceModifier", NULL, &lResult))
	{
		iRange *= std::max<int>(0, (lResult + 100));
		iRange /= 100;
	}

	return std::max(iRange, GC.getDefineINT("MIN_CIV_STARTING_DISTANCE"));
}


int CvPlayer::startingPlotDistanceFactor(CvPlot* pPlot, PlayerTypes ePlayer, int iRange) const
{
	PROFILE_FUNC();

	FAssert(ePlayer != getID());

	CvPlot* pStartingPlot;

	int iValue = 1000;

	pStartingPlot = getStartingPlot();

	if (pStartingPlot != NULL)
	{
		if (GC.getGameINLINE().isTeamGame())
		{
			if (GET_PLAYER(ePlayer).getTeam() == getTeam())
			{
				iRange *= GC.getDefineINT("OWN_TEAM_STARTING_MODIFIER");
				iRange /= 100;
			}
			else
			{
				iRange *= GC.getDefineINT("RIVAL_TEAM_STARTING_MODIFIER");
				iRange /= 100;
			}
		}

		int iDistance = stepDistance(pPlot->getX_INLINE(), pPlot->getY_INLINE(), pStartingPlot->getX_INLINE(), pStartingPlot->getY_INLINE());
		if (pStartingPlot->getArea() != pPlot->getArea())
		{
			iDistance *= 4;
			iDistance /= 3;
		}

		iValue *= iDistance;
		iValue /= iRange ;

	}

	return std::max(1, iValue);

}


// Returns the id of the best area, or -1 if it doesn't matter:
int CvPlayer::findStartingArea() const
{
	PROFILE_FUNC();

	long result = -1;
	if (gDLL->getPythonIFace()->pythonFindStartingArea(getID(), &result) && !gDLL->getPythonIFace()->pythonUsingDefaultImpl()) // Python override
	{
		if (result == -1 || GC.getMapINLINE().getArea(result) != NULL)
		{
			return result;
		}
		else
		{
			FAssertMsg(false, "python findStartingArea() must return -1 or the ID of a valid area");
		}
	}

	//not useful for water starts
	if(GC.getCivilizationInfo(getCivilizationType()).isWaterStart())
	{
		return -1;
	}

	int iBestValue = 0;
	int iBestArea = -1;
	int iValue;
	int iLoop = 0;

	CvArea *pLoopArea = NULL;

	// find best land area
	for(pLoopArea = GC.getMapINLINE().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMapINLINE().nextArea(&iLoop))
	{
		if (!(pLoopArea->isWater()))
		{
			// iNumPlayersOnArea is the number of players starting on the area, plus this player
			int iNumPlayersOnArea = (pLoopArea->getNumStartingPlots() + 1);
			int iTileValue = ((pLoopArea->calculateTotalBestNatureYield() + (pLoopArea->countCoastalLand() * 2) + pLoopArea->getNumRiverEdges() + (pLoopArea->getNumTiles())) + 1);
			iValue = iTileValue / iNumPlayersOnArea;

			iValue *= std::min(NUM_CITY_PLOTS + 1, pLoopArea->getNumTiles() + 1);
			iValue /= (NUM_CITY_PLOTS + 1);

			if (iNumPlayersOnArea <= 2)
			{
				iValue *= 4;
				iValue /= 3;
			}

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				iBestArea = pLoopArea->getID();
			}
		}
	}

	return iBestArea;
}


CvPlot* CvPlayer::findStartingPlot(bool bRandomize)
{
	PROFILE_FUNC();

	long result = -1;
	if (gDLL->getPythonIFace()->pythonFindStartingPlot(getID(), &result) && !gDLL->getPythonIFace()->pythonUsingDefaultImpl()) // Python override
	{
		CvPlot *pPlot = GC.getMapINLINE().plotByIndexINLINE(result);
		if (pPlot != NULL)
		{
			return pPlot;
		}
		else
		{
			FAssertMsg(false, "python findStartingPlot() returned an invalid plot index!");
		}
	}

	CvPlot* pLoopPlot;
	int iBestArea = -1;
	int iI;

	bool bNew = false;
	if (getStartingPlot() != NULL)
	{
		iBestArea = getStartingPlot()->getArea();
		setStartingPlot(NULL, true);
		bNew = true;
	}

	AI_updateFoundValues(true);//this sets all plots found values to -1

	if (!bNew)
	{
		iBestArea = findStartingArea();
	}

	//flood fill land distances O(numPlots)
	std::vector<short> aLandDistances(GC.getMapINLINE().numPlotsINLINE(), MAX_SHORT);

	{
		PROFILE("CvPlayer::findStartingPlot::landDistances");

		//initialize
		std::deque<int> aFillQueue;
		for(int i=0;i<GC.getMapINLINE().numPlotsINLINE();i++)
		{
			CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(i);
			if(!pLoopPlot->isWater())
			{
				aLandDistances[i] = 0;
				aFillQueue.push_back(i);
			}
		}

		//fill
		while(!aFillQueue.empty())
		{
			int iLoopIndex = aFillQueue.front();
			aFillQueue.pop_front();
			CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iLoopIndex);
			int iLoopDistance = aLandDistances[iLoopIndex] + 1;
			for(int iDirection=0;iDirection<NUM_DIRECTION_TYPES;iDirection++)
			{
				CvPlot* pAdjacentPlot = plotDirection(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), (DirectionTypes) iDirection);
				if(pAdjacentPlot != NULL)
				{
					int iAdjacentIndex = GC.getMapINLINE().plotNumINLINE(pAdjacentPlot->getX_INLINE(), pAdjacentPlot->getY_INLINE());
					if(iLoopDistance < aLandDistances[iAdjacentIndex])
					{
						aLandDistances[iAdjacentIndex] = iLoopDistance;
						aFillQueue.push_back(iAdjacentIndex);
					}
				}
			}
		}
	}

	bool bStartOnWater = GC.getCivilizationInfo(getCivilizationType()).isWaterStart();
	std::deque<bool> abEuropeStart;
	if (bStartOnWater)
	{
		for (int i = 0; i < GC.getNumEuropeInfos(); ++i)
		{
			abEuropeStart.push_back(GC.getEuropeInfo((EuropeTypes) i).isStart());
			gDLL->getPythonIFace()->pythonIsEuropeStart((EuropeTypes) i, &abEuropeStart[i]);
		}
	}

	for(int iPass = 0; iPass < 2; iPass++)
	{
		int iBestValue = 0;
		int iBestIndex = -1;

		std::vector<int> aiWeights(GC.getMapINLINE().numPlotsINLINE(), 0);
		for (iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
		{
			int iValue = 0;
			pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

			if ((iBestArea == -1) || (pLoopPlot->getArea() == iBestArea) || (iPass > 0))
			{
				if (bStartOnWater)
				{
					EuropeTypes eEurope = pLoopPlot->getEurope();
					if (eEurope != NO_EUROPE && abEuropeStart[eEurope])
					{
						iValue = 1;
						switch ((CardinalDirectionTypes)GC.getEuropeInfo(eEurope).getCardinalDirection())
						{
						case CARDINALDIRECTION_EAST:
						case CARDINALDIRECTION_WEST:
							iValue *= 1 + std::min(::plotDistance(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), pLoopPlot->getX_INLINE(), 0), startingPlotRange());
							iValue *= 1 + std::min(::plotDistance(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), pLoopPlot->getX_INLINE(), GC.getMapINLINE().getGridHeightINLINE() - 1), startingPlotRange() / 2);
							break;
						case CARDINALDIRECTION_NORTH:
						case CARDINALDIRECTION_SOUTH:
							iValue *= 1 + std::min(::plotDistance(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), 0, pLoopPlot->getY_INLINE()), startingPlotRange());
							iValue *= 1 + std::min(::plotDistance(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), GC.getMapINLINE().getGridWidthINLINE() - 1, pLoopPlot->getY_INLINE()), startingPlotRange() / 2);
							break;
						default:
							break;
						}
						int iMinPlotDistance = startingPlotRange();
						for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
						{
							CvPlayer& otherPlayer = GET_PLAYER((PlayerTypes) iPlayer);
							if ((iPlayer != getID()) && otherPlayer.isAlive())
							{
								CvPlot* pOtherPlot = otherPlayer.getStartingPlot();
								if(pOtherPlot != NULL)
								{
									if(GC.getCivilizationInfo(otherPlayer.getCivilizationType()).isWaterStart())
									{
										int iPlotDistance = plotDistance(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), pOtherPlot->getX_INLINE(), pOtherPlot->getY_INLINE());
										if (iPlotDistance < iMinPlotDistance)
										{
											iMinPlotDistance = iPlotDistance;
										}
									}
								}
							}
						}
						iValue *= 1 + iMinPlotDistance;

						iValue /= (aLandDistances[iI] + 1);
					}
				}
				else
				{
					//the distance factor is now done inside foundValue
					iValue = pLoopPlot->getFoundValue(getID());
				}

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					iBestIndex = iI;
				}
			}

			FAssert(iValue >= 0);
			aiWeights[iI] = std::max(0, iValue);
		}

		FAssert(std::accumulate(aiWeights.begin(), aiWeights.end(), 0) > 0);

		if (bRandomize)
		{
			iBestIndex = GC.getGameINLINE().getSorenRand().pickValue(aiWeights, "Randomizing start");
		}

		return GC.getMapINLINE().plotByIndexINLINE(iBestIndex);

		FAssertMsg(iPass != 0, "CvPlayer::findStartingPlot - could not find starting plot in first pass.");
	}

	FAssertMsg(false, "Could not find starting plot.");
	return NULL;
}


CvCity* CvPlayer::initCity(int iX, int iY, bool bBumpUnits)
{
	PROFILE_FUNC();

	CvCity* pCity = addCity();

	FAssertMsg(pCity != NULL, "City is not assigned a valid value");
	FAssertMsg(!(GC.getMapINLINE().plotINLINE(iX, iY)->isCity()), "No city is expected at this plot when initializing new city");

	pCity->init(pCity->getID(), getID(), iX, iY, bBumpUnits);

	return pCity;
}


void CvPlayer::acquireCity(CvCity* pOldCity, bool bConquest, bool bTrade)
{
	CLLNode<IDInfo>* pUnitNode;
	CvCity* pNewCity;
	CvUnit* pLoopUnit;
	CvPlot* pCityPlot;
	CvPlot* pLoopPlot;
	bool* pabHasRealBuilding;
	int* paiBuildingOriginalOwner;
	int* paiBuildingOriginalTime;
	CvWString szBuffer;
	CvWString szName;
	bool abEverOwned[MAX_PLAYERS];
	int aiCulture[MAX_PLAYERS];
	PlayerTypes eOldOwner;
	PlayerTypes eOriginalOwner;
	PlayerTypes eHighestCulturePlayer;
	BuildingTypes eBuilding;
	bool bForceUnowned;
	bool bRecapture;
	bool bRaze;
	bool bGift;
	int iRange;
	int iCaptureGold;
	int iGameTurnFounded;
	int iPopulation;
	int iHighestPopulation;
	int iOccupationTimer;
	int iTeamCulturePercent;
	int iDamage;
	int iDX, iDY;
	int iI;
	CLinkList<IDInfo> oldUnits;

	pCityPlot = pOldCity->plot();

	pUnitNode = pCityPlot->headUnitNode();

	while (pUnitNode != NULL)
	{
		oldUnits.insertAtEnd(pUnitNode->m_data);
		pUnitNode = pCityPlot->nextUnitNode(pUnitNode);
	}

	pUnitNode = oldUnits.head();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = oldUnits.next(pUnitNode);

		if (pLoopUnit && pLoopUnit->getTeam() != getTeam())
		{
			if (pLoopUnit->getDomainType() == DOMAIN_IMMOBILE)
			{
				pLoopUnit->kill(false);
			}
		}
	}

	if (bConquest)
	{
		iRange = pOldCity->getCultureLevel();

		for (iDX = -(iRange); iDX <= iRange; iDX++)
		{
			for (iDY = -(iRange); iDY <= iRange; iDY++)
			{
				if (pOldCity->cultureDistance(iDX, iDY) <= iRange)
				{
					pLoopPlot = plotXY(pOldCity->getX_INLINE(),pOldCity-> getY_INLINE(), iDX, iDY);

					if (pLoopPlot != NULL)
					{
						if (pLoopPlot->getOwnerINLINE() == pOldCity->getOwnerINLINE())
						{
							if (pLoopPlot->getNumCultureRangeCities(pOldCity->getOwnerINLINE()) == 1)
							{
								bForceUnowned = false;

								for (iI = 0; iI < MAX_PLAYERS; iI++)
								{
									if (GET_PLAYER((PlayerTypes)iI).isAlive())
									{
										if ((GET_PLAYER((PlayerTypes)iI).getTeam() != getTeam()) && (GET_PLAYER((PlayerTypes)iI).getTeam() != pOldCity->getTeam()))
										{
											if (pLoopPlot->getNumCultureRangeCities((PlayerTypes)iI) > 0)
											{
												bForceUnowned = true;
												break;
											}
										}
									}
								}

								if (bForceUnowned)
								{
									pLoopPlot->setForceUnownedTimer(2); // XXX
								}
							}
						}
					}
				}
			}
		}
	}

	if (pOldCity->getOriginalOwner() == pOldCity->getOwnerINLINE())
	{
		GET_PLAYER(pOldCity->getOriginalOwner()).changeCitiesLost(1);
	}
	else if (pOldCity->getOriginalOwner() == getID())
	{
		GET_PLAYER(pOldCity->getOriginalOwner()).changeCitiesLost(-1);
	}

	if (bConquest)
	{
		szBuffer = gDLL->getText("TXT_KEY_MISC_CAPTURED_CITY", pOldCity->getNameKey()).GetCString();
		gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CITYCAPTURE", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pOldCity->getX_INLINE(), pOldCity->getY_INLINE(), true, true);

		szName.Format(L"%s (%s)", pOldCity->getName().GetCString(), GET_PLAYER(pOldCity->getOwnerINLINE()).getName());

		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (iI != getID())
				{
					if (pOldCity->isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
					{
						szBuffer = gDLL->getText("TXT_KEY_MISC_CITY_CAPTURED_BY", szName.GetCString(), getCivilizationDescriptionKey());
						gDLL->getInterfaceIFace()->addMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CITYCAPTURED", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pOldCity->getX_INLINE(), pOldCity->getY_INLINE(), true, true);
					}
				}
			}
		}

		szBuffer = gDLL->getText("TXT_KEY_MISC_CITY_WAS_CAPTURED_BY", szName.GetCString(), getCivilizationDescriptionKey());
		GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szBuffer, pOldCity->getX_INLINE(), pOldCity->getY_INLINE(), (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));
	}

	iCaptureGold = 0;

	if (bConquest)
	{
		long lCaptureGold;
		// Use python to determine city capture gold amounts...
		lCaptureGold = 0;

		CyCity* pyOldCity = new CyCity(pOldCity);

		CyArgsList argsList;
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pyOldCity));	// pass in plot class

		// Erik: Cannot be blocked since its only implemented in python
		gDLL->getPythonIFace()->callFunction(PYGameModule, "doCityCaptureGold", argsList.makeFunctionArgs(),&lCaptureGold);
		delete pyOldCity;	// python fxn must not hold on to this pointer

		iCaptureGold = (int)lCaptureGold;
	}

	changeGold(iCaptureGold);

	pabHasRealBuilding = new bool[GC.getNumBuildingInfos()];
	paiBuildingOriginalOwner = new int[GC.getNumBuildingInfos()];
	paiBuildingOriginalTime = new int[GC.getNumBuildingInfos()];

	eOldOwner = pOldCity->getOwnerINLINE();
	eOriginalOwner = pOldCity->getOriginalOwner();
	PlayerTypes eOldPreviousOwner = pOldCity->getPreviousOwner();
	eHighestCulturePlayer = pOldCity->findHighestCulture();
	iGameTurnFounded = pOldCity->getGameTurnFounded();
	iPopulation = pOldCity->getPopulation();
	iHighestPopulation = pOldCity->getHighestPopulation();
	iOccupationTimer = pOldCity->getOccupationTimer();
	szName = pOldCity->getNameKey();
	iDamage = pOldCity->getDefenseDamage();
	int iOldCityId = pOldCity->getID();
	UnitClassTypes eTeachUnitClass = pOldCity->getTeachUnitClass();
	int iEducationThresholdMultiplier = pOldCity->getEducationThresholdMultiplier();

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		abEverOwned[iI] = pOldCity->isEverOwned((PlayerTypes)iI);
		aiCulture[iI] = pOldCity->getCulture((PlayerTypes)iI);
	}

	abEverOwned[getID()] = true;

	for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		pabHasRealBuilding[iI] = pOldCity->isHasRealBuilding((BuildingTypes)iI);
		paiBuildingOriginalOwner[iI] = pOldCity->getBuildingOriginalOwner((BuildingTypes)iI);
		paiBuildingOriginalTime[iI] = pOldCity->getBuildingOriginalTime((BuildingTypes)iI);
	}

	std::vector<BuildingYieldChange> aBuildingYieldChange;
	for (iI = 0; iI < GC.getNumBuildingClassInfos(); ++iI)
	{
		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			BuildingYieldChange kChange;
			kChange.eBuildingClass = (BuildingClassTypes)iI;
			kChange.eYield = (YieldTypes)iYield;
			kChange.iChange = pOldCity->getBuildingYieldChange((BuildingClassTypes)iI, (YieldTypes)iYield);
			if (0 != kChange.iChange)
			{
				aBuildingYieldChange.push_back(kChange);
			}
		}
	}

	int aiYieldsStored[NUM_YIELD_TYPES];
	for (iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		aiYieldsStored[iI] = pOldCity->getYieldStored((YieldTypes) iI);
	}


	//capture all population units
	std::vector<CvUnit *> aOldPopulationUnits;
	for (int i = 0; i < iPopulation; ++i)
	{
		CvUnit* pUnit = pOldCity->getPopulationUnitByIndex(0);
		bool bRemoved = pOldCity->removePopulationUnit(pUnit, false, NO_PROFESSION, bConquest);
		FAssert(bRemoved);
		aOldPopulationUnits.push_back(pUnit);
		GET_PLAYER(pOldCity->getOwnerINLINE()).getAndRemoveUnit(pUnit->getID());
	}
	FAssert(pOldCity->getPopulation() == 0);

	bRecapture = ((eHighestCulturePlayer != NO_PLAYER) ? (GET_PLAYER(eHighestCulturePlayer).getTeam() == getTeam()) : false);

	pOldCity->kill();

	//acquire old population units
	std::vector<CvUnit*> aNewPopulationUnits;
	for(int i=0;i<(int)aOldPopulationUnits.size();i++)
	{
		CvUnit* pOldUnit = aOldPopulationUnits[i];
		UnitTypes eNewUnitType = (UnitTypes) GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(pOldUnit->getUnitClassType());
		if (eNewUnitType != NO_UNIT)
		{
			CvUnit* pNewUnit = initUnit(eNewUnitType, NO_PROFESSION, pCityPlot->getX_INLINE(), pCityPlot->getY_INLINE(), pOldUnit->AI_getUnitAIType());
			pNewUnit->convert(pOldUnit, true); //kills old unit
			aNewPopulationUnits.push_back(pNewUnit);
		}
		else
		{
			pOldUnit->updateOwnerCache(-1);
			SAFE_DELETE(pOldUnit);
		}
	}

	if (bTrade)
	{
		for (iDX = -1; iDX <= 1; iDX++)
		{
			for (iDY = -1; iDY <= 1; iDY++)
			{
				pLoopPlot	= plotXY(pCityPlot->getX_INLINE(), pCityPlot->getY_INLINE(), iDX, iDY);

				if (pLoopPlot != NULL)
				{
					pLoopPlot->setCulture(eOldOwner, 0, false);
				}
			}
		}
	}

	pNewCity = initCity(pCityPlot->getX_INLINE(), pCityPlot->getY_INLINE(), !bConquest);

	FAssertMsg(pNewCity != NULL, "NewCity is not assigned a valid value");

	pNewCity->setPreviousOwner(eOldOwner);
	pNewCity->setOriginalOwner(eOriginalOwner);
	pNewCity->setGameTurnFounded(iGameTurnFounded);
	pNewCity->setTeachUnitClass(eTeachUnitClass);
	pNewCity->setEducationThresholdMultiplier(iEducationThresholdMultiplier);

	for (uint i = 0; i < aNewPopulationUnits.size(); i++)
	{
		pNewCity->addPopulationUnit(aNewPopulationUnits[i], NO_PROFESSION);
	}

	pNewCity->setHighestPopulation(iHighestPopulation);
	pNewCity->setName(szName);
	pNewCity->setNeverLost(false);
	pNewCity->changeDefenseDamage(iDamage);

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		pNewCity->setEverOwned(((PlayerTypes)iI), abEverOwned[iI]);
		pNewCity->setCulture(((PlayerTypes)iI), aiCulture[iI], false);
	}

	for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		bool bSetHasBuilding = false;

		if (pabHasRealBuilding[iI])
		{
			BuildingClassTypes eBuildingClass = (BuildingClassTypes)GC.getBuildingInfo((BuildingTypes)iI).getBuildingClassType();
			eBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(eBuildingClass);

			if (eBuilding != NO_BUILDING)
			{
				const CvBuildingInfo& kBuilding = GC.getBuildingInfo((BuildingTypes)iI);
				if (bTrade || !(kBuilding.isNeverCapture()))
				{
					if (pNewCity->isValidBuildingLocation(kBuilding))
					{
						if (!bConquest || bRecapture || pNewCity->isHasRealBuilding(eBuilding) || GC.getGameINLINE().getSorenRandNum(100, "Capture Probability") < kBuilding.getConquestProbability())
						{
							bSetHasBuilding = true;
						}
					}
				}

				pNewCity->setHasRealBuildingTimed(eBuilding, bSetHasBuilding, false, ((PlayerTypes)(paiBuildingOriginalOwner[iI])), paiBuildingOriginalTime[iI]);
			}
		}
	}

	for (iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		pNewCity->setYieldStored((YieldTypes) iI, aiYieldsStored[iI]);
	}

	for (std::vector<BuildingYieldChange>::iterator it = aBuildingYieldChange.begin(); it != aBuildingYieldChange.end(); ++it)
	{
		pNewCity->setBuildingYieldChange((*it).eBuildingClass, (*it).eYield, (*it).iChange);
	}

	if (bTrade)
	{
		if (!bRecapture)
		{
			pNewCity->changeOccupationTimer(iOccupationTimer);
		}
	}

	if (bConquest)
	{
		iTeamCulturePercent = pNewCity->calculateTeamCulturePercent(getTeam());

		if (iTeamCulturePercent < GC.getDefineINT("OCCUPATION_CULTURE_PERCENT_THRESHOLD"))
		{
			pNewCity->changeOccupationTimer(((GC.getDefineINT("BASE_OCCUPATION_TURNS") + ((pNewCity->getPopulation() * GC.getDefineINT("OCCUPATION_TURNS_POPULATION_PERCENT")) / 100)) * (100 - iTeamCulturePercent)) / 100);
		}

		GC.getMapINLINE().verifyUnitValidPlot();
	}

	pCityPlot->setRevealed(GET_PLAYER(eOldOwner).getTeam(), true, false, NO_TEAM);

	gDLL->getEventReporterIFace()->cityAcquired(eOldOwner, getID(), pNewCity, bConquest, bTrade);

	SAFE_DELETE_ARRAY(pabHasRealBuilding);
	SAFE_DELETE_ARRAY(paiBuildingOriginalOwner);
	SAFE_DELETE_ARRAY(paiBuildingOriginalTime);

	if (!bTrade)
	{
		TeamTypes eOldPreviousTeam = NO_TEAM;
		if (eOldPreviousOwner != NO_PLAYER)
		{
			eOldPreviousTeam = GET_PLAYER(eOldPreviousOwner).getTeam();
		}

		if (eOldPreviousTeam != getTeam() && eHighestCulturePlayer != getID())
		{
			for (int i = 0; i < GC.getNumFatherPointInfos(); ++i)
			{
				FatherPointTypes ePointType = (FatherPointTypes) i;
				int gameSpeedMod =  GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();

				changeFatherPoints(ePointType, GC.getFatherPointInfo(ePointType).getConquerCityPoints()*gameSpeedMod/100);
			}
		}
	}

	if (bConquest)
	{
		CyCity* pyCity = new CyCity(pNewCity);
		CyArgsList argsList;
		argsList.add(getID());	// Player ID
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
		long lResult=0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "canRazeCity", argsList.makeFunctionArgs(), &lResult);
		delete pyCity;	// python fxn must not hold on to this pointer

		if (lResult == 1)
		{
			//auto raze based on game rules
			if (pNewCity->isAutoRaze())
			{
				if (iCaptureGold > 0)
				{
					szBuffer = gDLL->getText("TXT_KEY_MISC_PILLAGED_CITY", iCaptureGold, pNewCity->getNameKey());
					gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CITYRAZE", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pNewCity->getX_INLINE(), pNewCity->getY_INLINE(), true, true);
				}

				pNewCity->doTask(TASK_RAZE);
			}
			else if (!isHuman())
			{
				AI_conquerCity(pNewCity); // could delete the pointer...
			}
			else
			{
				//popup raze option
				eHighestCulturePlayer = pNewCity->getLiberationPlayer(true);
				bRaze = canRaze(pNewCity);
				bGift = ((eHighestCulturePlayer != NO_PLAYER)
						&& (eHighestCulturePlayer != getID())
						&& ((getTeam() == GET_PLAYER(eHighestCulturePlayer).getTeam())
							|| GET_TEAM(getTeam()).isOpenBorders(GET_PLAYER(eHighestCulturePlayer).getTeam())));

				if (bRaze || bGift)
				{
					CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_RAZECITY);
					pInfo->setData1(pNewCity->getID());
					pInfo->setData2(eHighestCulturePlayer);
					pInfo->setData3(iCaptureGold);
					gDLL->getInterfaceIFace()->addPopup(pInfo, getID());
				}
				else
				{
					pNewCity->chooseProduction();
					gDLL->getEventReporterIFace()->cityAcquiredAndKept(GC.getGameINLINE().getActivePlayer(), pNewCity);
				}
			}
		}
	}
	else
	{
		if (pNewCity->getPopulation() == 0)
		{
			pNewCity->doTask(TASK_RAZE);
		}
		else if (!bTrade)
		{
			FAssertMsg(false, "No more culture flips");
			gDLL->getEventReporterIFace()->cityAcquiredAndKept(GC.getGameINLINE().getActivePlayer(), pNewCity);
		}
	}

	// Forcing events that deal with the old city not to expire just because we conquered that city
	for (CvEventMap::iterator it = m_mapEventsOccured.begin(); it != m_mapEventsOccured.end(); ++it)
	{
		EventTriggeredData &triggerData = (*it).second;
		if((triggerData.m_eOtherPlayer == eOldOwner) && (triggerData.m_iOtherPlayerCityId == iOldCityId))
		{
			triggerData.m_iOtherPlayerCityId = -1;
		}
	}
}


void CvPlayer::killCities()
{
	int iLoop;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->kill();
	}
	// Super Forts begin *culture* - Clears culture from forts when a player dies
	PlayerTypes ePlayer = getID();
	for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
		if (pLoopPlot != NULL && pLoopPlot->getOwner() == ePlayer) //R&R mod, vetiarvind, super forts merge - added null check
		{
			pLoopPlot->setOwner(pLoopPlot->calculateCulturalOwner(), true);
		}
	}
	// Super Forts end
}


const CvWString CvPlayer::getNewCityName() const
{
	for (int i = 0; i < getNumCityNames(); ++i)
	{
		if (isCityNameValid(getCityName(i), true))
		{
			return getCityName(i);
		}
	}

	CvWString szName;
	getCivilizationCityName(szName, getCivilizationType());
	if (!szName.empty())
	{
		return szName;
	}

	// Pick a name from another random civ
	std::vector<CivilizationTypes> aePriorityCivs;
	std::vector<CivilizationTypes> aeOtherCivs;
	int iRandOffset = GC.getGameINLINE().getSorenRandNum(GC.getNumCivilizationInfos(), "Place Units (Player)");
	for (int iI = 0; iI < GC.getNumCivilizationInfos(); iI++)
	{
		CivilizationTypes eLoopCiv = (CivilizationTypes) ((iI + iRandOffset) % GC.getNumCivilizationInfos());
		if (eLoopCiv != getCivilizationType())
		{
			if (GC.getCivilizationInfo(eLoopCiv).isNative() == isNative())
			{
				aePriorityCivs.push_back(eLoopCiv);
			}
			else
			{
				aeOtherCivs.push_back(eLoopCiv);
			}
		}
	}

	for (uint i = 0; i < aePriorityCivs.size(); ++i)
	{
		getCivilizationCityName(szName, aePriorityCivs[i]);
		if (!szName.empty())
		{
			return szName;
		}
	}

	for (uint i = 0; i < aeOtherCivs.size(); ++i)
	{
		getCivilizationCityName(szName, aeOtherCivs[i]);
		if (!szName.empty())
		{
			return szName;
		}
	}

	return L"TXT_KEY_CITY";
}

const CvWString& CvPlayer::addNewCityName()
{
	addCityName(getNewCityName());
	return getCityName(getNumCityNames() - 1);
}


void CvPlayer::getCivilizationCityName(CvWString& szBuffer, CivilizationTypes eCivilization) const
{
	for (int iI = 0; iI < GC.getCivilizationInfo(eCivilization).getNumCityNames(); iI++)
	{
		szBuffer = CvWString(GC.getCivilizationInfo(eCivilization).getCityNames(iI));
		if (isCityNameValid(szBuffer, true))
		{
			return;
		}
	}

	szBuffer.clear();
}


bool CvPlayer::isCityNameValid(const CvWString& szName, bool bTestDestroyed) const
{
	CvCity* pLoopCity;
	int iLoop;

	if (bTestDestroyed)
	{
		if (GC.getGameINLINE().isDestroyedCityName(szName))
		{
			return false;
		}

		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
			for (pLoopCity = kLoopPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kLoopPlayer.nextCity(&iLoop))
			{
				if (pLoopCity->getName() == gDLL->getText(szName))
				{
					return false;
				}
			}
		}
	}
	else
	{
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			if (pLoopCity->getName() == gDLL->getText(szName))
			{
				return false;
			}
		}
	}

	return true;
}


CvUnit* CvPlayer::initUnit(UnitTypes eUnit, ProfessionTypes eProfession, int iX, int iY, UnitAITypes eUnitAI, DirectionTypes eFacingDirection, int iYieldStored)
{
	PROFILE_FUNC();

	FAssertMsg(eUnit != NO_UNIT, "Unit is not assigned a valid value");

	CvUnit* pUnit = addUnit();
	FAssertMsg(pUnit != NULL, "Unit is not assigned a valid value");
	if (NULL != pUnit)
	{
		if (eUnitAI == NO_UNITAI && eProfession != NO_PROFESSION)
		{
			eUnitAI = (UnitAITypes) GC.getProfessionInfo(eProfession).getDefaultUnitAIType();
		}

		if (eUnitAI == NO_UNITAI)
		{
			eUnitAI = (UnitAITypes) GC.getUnitInfo(eUnit).getDefaultUnitAIType();
		}

		pUnit->init(pUnit->getID(), eUnit, eProfession, eUnitAI, getID(), iX, iY, eFacingDirection, iYieldStored);

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			if (gDLL->getInterfaceIFace()->getHeadSelectedUnit() == NULL)
			{
				gDLL->getInterfaceIFace()->changeCycleSelectionCounter(1);
			}
		}
	}

	return pUnit;
}

CvUnit* CvPlayer::initEuropeUnit(UnitTypes eUnit, UnitAITypes eUnitAI, DirectionTypes eFacingDirection)
{
	CvUnit* pUnit = initUnit(eUnit, GC.getUnitInfo(eUnit).getDefaultProfession(), INVALID_PLOT_COORD, INVALID_PLOT_COORD, eUnitAI, eFacingDirection);
	unloadUnitToEurope(pUnit);
	return pUnit;
}

//WTP, ray, Settler Professsion - START
//This is the function to init Units in Profession Settler in Europe
void CvPlayer::initEuropeSettler(bool bPayEquipment)
{
	// here we need to get the Profession Settler for initUnit call
	ProfessionTypes eSettlerProfession = NO_PROFESSION;
	int iEquipmentCosts = 0;
	for (ProfessionTypes eLoopProfession = FIRST_PROFESSION; eLoopProfession < NUM_PROFESSION_TYPES; ++eLoopProfession)
	{
		// storing the Loop Profession
		const CvProfessionInfo& kProfession = GC.getProfessionInfo(eLoopProfession);

		// checking if the profession is valid and can found settlements
		if (GC.getCivilizationInfo(getCivilizationType()).isValidProfession(eLoopProfession) && kProfession.canFound())
		{
			//  only if bPayEquipment - the for the Equipment is calculated. otherwise it stays 0
			if(bPayEquipment)
			{
				for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
				{
					int iYieldAmount = getYieldEquipmentAmount(eLoopProfession, eYield);
					iEquipmentCosts += iYieldAmount * getYieldSellPrice(eYield); // we only use the sell price
				}
			}

			eSettlerProfession = eLoopProfession;
			break; // we do not loop further
		}
	}
	
	if (eSettlerProfession != NO_PROFESSION)
	{
		// AI pays the calculated costs - wich are already calculated nicely just Sell Price
		// if bPayEquipment is false, these are 0
		// if it does not have the money it does not get the Settler in Europe
		if(getGold() >= iEquipmentCosts)
		{
			changeGold(-iEquipmentCosts);
			UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("DEFAULT_POPULATION_UNIT"));
			CvUnit* pUnit = initUnit(eUnit, eSettlerProfession, INVALID_PLOT_COORD, INVALID_PLOT_COORD, UNITAI_SETTLER, NO_DIRECTION);
			unloadUnitToEurope(pUnit);		
		}
	}
	return;
}
//WTP, ray, Settler Professsion - END


/*** TRIANGLETRADE 10/23/08 by DPII ***/
CvUnit* CvPlayer::initAfricaUnit(UnitTypes eUnit, UnitAITypes eUnitAI, DirectionTypes eFacingDirection)
{
	CvUnit* pUnit = initUnit(eUnit, GC.getUnitInfo(eUnit).getDefaultProfession(), INVALID_PLOT_COORD, INVALID_PLOT_COORD, eUnitAI, eFacingDirection);
	unloadUnitToAfrica(pUnit);
	return pUnit;
}
/***********************************/

// R&R, ray, Port Royal
CvUnit* CvPlayer::initPortRoyalUnit(UnitTypes eUnit, UnitAITypes eUnitAI, DirectionTypes eFacingDirection)
{
	CvUnit* pUnit = initUnit(eUnit, GC.getUnitInfo(eUnit).getDefaultProfession(), INVALID_PLOT_COORD, INVALID_PLOT_COORD, eUnitAI, eFacingDirection);
	unloadUnitToPortRoyal(pUnit);
	return pUnit;
}
// R&R, ray, Port Royal - END


void CvPlayer::killUnits()
{
	while (!m_units.empty())
	{
		m_units.begin()->second->kill(false);
	}
}


// XXX should pUnit be a CvSelectionGroup???
// Returns the next unit in the cycle...
CvSelectionGroup* CvPlayer::cycleSelectionGroups(CvUnit* pUnit, bool bForward, bool* pbWrap)
{
	CLLNode<int>* pSelectionGroupNode;
	CLLNode<int>* pFirstSelectionGroupNode;
	CvSelectionGroup* pLoopSelectionGroup;

	if (pbWrap != NULL)
	{
		*pbWrap = false;
	}

	pSelectionGroupNode = headGroupCycleNode();

	if (pUnit != NULL)
	{
		while (pSelectionGroupNode != NULL)
		{
			if (getSelectionGroup(pSelectionGroupNode->m_data) == pUnit->getGroup())
			{
				if (bForward)
				{
					pSelectionGroupNode = nextGroupCycleNode(pSelectionGroupNode);
				}
				else
				{
					pSelectionGroupNode = previousGroupCycleNode(pSelectionGroupNode);
				}
				break;
			}

			pSelectionGroupNode = nextGroupCycleNode(pSelectionGroupNode);
		}
	}

	if (pSelectionGroupNode == NULL)
	{
		if (bForward)
		{
			pSelectionGroupNode = headGroupCycleNode();
		}
		else
		{
			pSelectionGroupNode = tailGroupCycleNode();
		}

		if (pbWrap != NULL)
		{
			*pbWrap = true;
		}
	}

	if (pSelectionGroupNode != NULL)
	{
		pFirstSelectionGroupNode = pSelectionGroupNode;

		while (true)
		{
			pLoopSelectionGroup = getSelectionGroup(pSelectionGroupNode->m_data);
			FAssertMsg(pLoopSelectionGroup != NULL, "LoopSelectionGroup is not assigned a valid value");

			if (pLoopSelectionGroup->readyToSelect())
			{
				if (pUnit && pLoopSelectionGroup == pUnit->getGroup())
				{
					if (pbWrap != NULL)
					{
						*pbWrap = true;
					}
				}

				return pLoopSelectionGroup;
			}

			if (bForward)
			{
				pSelectionGroupNode = nextGroupCycleNode(pSelectionGroupNode);

				if (pSelectionGroupNode == NULL)
				{
					pSelectionGroupNode = headGroupCycleNode();

					if (pbWrap != NULL)
					{
						*pbWrap = true;
					}
				}
			}
			else
			{
				pSelectionGroupNode = previousGroupCycleNode(pSelectionGroupNode);

				if (pSelectionGroupNode == NULL)
				{
					pSelectionGroupNode = tailGroupCycleNode();

					if (pbWrap != NULL)
					{
						*pbWrap = true;
					}
				}
			}

			if (pSelectionGroupNode == pFirstSelectionGroupNode)
			{
				break;
			}
		}
	}

	return NULL;
}


bool CvPlayer::hasTrait(TraitTypes eTrait) const
{
	return (getTraitCount(eTrait) > 0);
}

int CvPlayer::getTraitCount(TraitTypes eTrait) const
{
	FAssert(validEnumRange(eTrait));
	return m_em_iTraitCount.get(eTrait);
}

void CvPlayer::changeTraitCount(TraitTypes eTrait, int iChange)
{
	FAssert(validEnumRange(eTrait));
	m_em_iTraitCount.add(eTrait, iChange);
	FAssert(getTraitCount(eTrait) >= 0);
}

int CvPlayer::getMercantileFactor() const
{
	int iMercantileFactor = 100;
	for (TraitTypes eTrait = FIRST_TRAIT; eTrait < NUM_TRAIT_TYPES; ++eTrait)
	{
		if (hasTrait(eTrait))
		{
			iMercantileFactor *= (100 + GC.getTraitInfo(eTrait).getMercantileFactor());
			iMercantileFactor /= 100;
		}
	}

	return iMercantileFactor - 100;
}

bool CvPlayer::isHuman() const
{
	return m_bHuman;
}

void CvPlayer::updateHuman()
{
	bool old_m_bHuman = m_bHuman; // cache CvPlayer::getYieldEquipmentAmount - Nightinggale
	if (getID() == NO_PLAYER)
	{
		m_bHuman = false;
	}
	else
	{
		m_bHuman = GC.getInitCore().getHuman(getID());
	}

	// Dale - AoD: AI Autoplay START
	if( m_bDisableHuman )
	{
		m_bHuman = false;
	}
	// Dale - AoD: AI Autoplay END

	// cache CvPlayer::getYieldEquipmentAmount - start - Nightinggale
	if (old_m_bHuman != m_bHuman)
	{
		Update_cache_YieldEquipmentAmount();
	}
	// cache CvPlayer::getYieldEquipmentAmount - Nightinggale
}

bool CvPlayer::isNative() const
{
	CivilizationTypes eCivilizationType = getCivilizationType();
	if(eCivilizationType == NO_CIVILIZATION)
	{
		return false;
	}

	return GC.getCivilizationInfo(eCivilizationType).isNative();
}

CivCategoryTypes CvPlayer::getCivCategoryTypes() const
{
	CivilizationTypes eCivilizationType = getCivilizationType();
	if (eCivilizationType == NO_CIVILIZATION)
	{
		return CIV_CATEGORY_NOT_SET;
	}

	return GC.getCivilizationInfo(eCivilizationType).getCivCategoryTypes();
}


bool CvPlayer::isAlwaysOpenBorders() const
{
	if(getCivilizationType() == NO_CIVILIZATION)
	{
		return false;
	}

	return GC.getCivilizationInfo(getCivilizationType()).isOpenBorders();
}

const wchar* CvPlayer::getName(uint uiForm) const
{
	if (isEmpty(gDLL->getPlayerName(getID(), uiForm)) || (GC.getGameINLINE().isMPOption(MPOPTION_ANONYMOUS) && isAlive() && GC.getGameINLINE().getGameState() == GAMESTATE_ON))
	{
		return GC.getLeaderHeadInfo(getLeaderType()).getDescription(uiForm);
	}
	else
	{
		return gDLL->getPlayerName(getID(), uiForm);
	}
}


const wchar* CvPlayer::getNameKey() const
{
	if (isEmpty(gDLL->getPlayerNameKey(getID())) || (GC.getGameINLINE().isMPOption(MPOPTION_ANONYMOUS) && isAlive()))
	{
		return GC.getLeaderHeadInfo(getLeaderType()).getTextKeyWide();
	}
	else
	{
		return gDLL->getPlayerNameKey(getID());
	}
}


const wchar* CvPlayer::getCivilizationDescription(uint uiForm) const
{
	if (isEmpty(gDLL->getCivDescription(getID(), uiForm)))
	{
		return GC.getCivilizationInfo(getCivilizationType()).getDescription(uiForm);
	}
	else
	{
		return gDLL->getCivDescription(getID(), uiForm);
	}
}


const wchar* CvPlayer::getCivilizationDescriptionKey() const
{
	if (isEmpty(gDLL->getCivDescriptionKey(getID())))
	{
		return GC.getCivilizationInfo(getCivilizationType()).getTextKeyWide();
	}
	else
	{
		return gDLL->getCivDescriptionKey(getID());
	}
}


const wchar* CvPlayer::getCivilizationShortDescription(uint uiForm) const
{
	if (isEmpty(gDLL->getCivShortDesc(getID(), uiForm)))
	{
		return GC.getCivilizationInfo(getCivilizationType()).getShortDescription(uiForm);
	}
	else
	{
		return gDLL->getCivShortDesc(getID(), uiForm);
	}
}


const wchar* CvPlayer::getCivilizationShortDescriptionKey() const
{
	if (isEmpty(gDLL->getCivShortDescKey(getID())))
	{
		return GC.getCivilizationInfo(getCivilizationType()).getShortDescriptionKey();
	}
	else
	{
		return gDLL->getCivShortDescKey(getID());
	}
}


const wchar* CvPlayer::getCivilizationAdjective(uint uiForm) const
{
	if (isEmpty(gDLL->getCivAdjective(getID(), uiForm)))
	{
		return GC.getCivilizationInfo(getCivilizationType()).getAdjective(uiForm);
	}
	else
	{
		return gDLL->getCivAdjective(getID(), uiForm);
	}
}

const wchar* CvPlayer::getCivilizationAdjectiveKey() const
{
	if (isEmpty(gDLL->getCivAdjectiveKey(getID())))
	{
		return GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey();
	}
	else
	{
		return gDLL->getCivAdjectiveKey(getID());
	}
}


const char* CvPlayer::getFlagDecal() const
{
	return GC.getCivilizationInfo(getCivilizationType()).getFlagTexture();
}

bool CvPlayer::isWhiteFlag() const
{
	return GC.getCivilizationInfo(getCivilizationType()).getArtInfo()->isWhiteFlag();
}

bool CvPlayer::isInvertFlag() const
{
	return GC.getCivilizationInfo(getCivilizationType()).getArtInfo()->isInvertFlag();
}

const CvWString CvPlayer::getWorstEnemyName() const
{
	TeamTypes eWorstEnemy;

	eWorstEnemy = GET_TEAM(getTeam()).AI_getWorstEnemy();

	if (eWorstEnemy != NO_TEAM)
	{
		return GET_TEAM(eWorstEnemy).getName();
	}

	return "";
}

ArtStyleTypes CvPlayer::getArtStyleType() const
{
	if (GC.getInitCore().getArtStyle(getID()) == NO_ARTSTYLE)
	{
		return ((ArtStyleTypes)(GC.getCivilizationInfo(getCivilizationType()).getArtStyleType()));
	}
	else
	{
		return GC.getInitCore().getArtStyle(getID());
	}
}

const TCHAR* CvPlayer::getUnitButton(UnitTypes eUnit) const
{
	//Androrc UnitArtStyles
//	return GC.getUnitInfo(eUnit).getArtInfo(0, NO_PROFESSION)->getButton();
	if (getCivilizationType() != NO_CIVILIZATION)
	{
		return GC.getUnitInfo(eUnit).getUnitArtStylesArtInfo(0, NO_PROFESSION, (UnitArtStyleTypes) GC.getCivilizationInfo(getCivilizationType()).getUnitArtStyleType())->getButton();
	}
	else
	{
	return GC.getUnitInfo(eUnit).getArtInfo(0, NO_PROFESSION)->getButton();
}

	//Androrc End
}

void CvPlayer::doTurn()
{
	PROFILE_FUNC();

	EXTRA_POWER_CHECK

	CvCity* pLoopCity;
	int iLoop;

	FAssertMsg(isAlive(), "isAlive is expected to be true");
	FAssertMsg(!hasBusyUnit() || GC.getGameINLINE().isMPOption(MPOPTION_SIMULTANEOUS_TURNS)  || GC.getGameINLINE().isSimultaneousTeamTurns(), "End of turn with busy units in a sequential-turn game");

	gDLL->getEventReporterIFace()->beginPlayerTurn( GC.getGameINLINE().getGameTurn(),  getID());

	doEra();

	EXTRA_POWER_CHECK

	doUpdateCacheOnTurn();

	EXTRA_POWER_CHECK

	GC.getGameINLINE().verifyDeals();

	EXTRA_POWER_CHECK

	AI_doTurnPre();

	EXTRA_POWER_CHECK

	AI_assignWorkingPlots();

	EXTRA_POWER_CHECK

	doGold();

	EXTRA_POWER_CHECK

	doBells();

	EXTRA_POWER_CHECK

	// doCrosses(); CBM 0.7.020 - moved to player's nature check below

	// PatchMod: Achievements START
	doAchievements(false);
	// PatchMod: Achievements END

	EXTRA_POWER_CHECK

	// TAC - LbD - Ray - START
	doLbD();
	// TAC - LbD - Ray - END
	
	EXTRA_POWER_CHECK

	// R&R, ray, changes to Wild Animals
	if (!GC.getGameINLINE().isBarbarianPlayer(getID()) && !GC.getGameINLINE().isChurchPlayer(getID()) && !isNative() && !isEurope() && isAlive())
	{

		doCrosses(); // CBM 0.7.020 inserted from above

		//WTP, ray, fixing precalcuated Diplo Event Issue - START
		//WTP, ray, we move this from here to Player AI
		/*
		checkForNativeMercs(); //TAC Native Mercs
		checkForNativeSlaves(); // R&R, ray, Native Slave, START
		checkForAfricanSlaves(); // R&R, ray, African Slaves, START
		checkForPrisonsCrowded(); // R&R, ray, Prisons Crowded - START
		checkForRevolutionaryNoble(); // R&R, ray, Revolutionary Noble - START
		checkForBishop(); // R&R, ray, Bishop - START
		checkForChurchContact(); // R&R, ray, the Church - START
		checkForChurchWar(); // R&R, ray, Church War
		checkForSmugglers(); // R&R, ray, Smuggling - START
		checkForRangers(); // R&R, ray, Rangers - START
		checkForConquistadors(); // R&R, ray, Conquistadors - START
		checkForPirates(); // R&R, ray, Pirates - START
		checkForEuropeanPeace(); // R&R, ray, European Peace, START
		checkForEuropeanWars(); //TAC European Wars
		checkForStealingImmigrant(); // R&R, Stealing Immigrant
		checkForContinentalGuard(); // R&R, ray, Continental Guard - START
		checkForMortar(); // R&R, ray, Mortar - START
		checkForMilitiaOrUnrest(); // R&R, ray, Militia or Unrest - START
		*/
		//WTP, ray, fixing precalcuated Diplo Event Issue - END

		// TAC - AI Economy - Ray - START
		if (!isHuman())
		{
			//koma13
			CvPlayerAI& kPlayerAI = GET_PLAYER((PlayerTypes) getID());
			kPlayerAI.AI_updateBestPortCities();

			redistributeWood();
			// R&R, ray, redistribute cannons and muskets
			redistributeCannonsAndMuskets();
		}
		// TAC - AI Economy - Ray - END
	}

	EXTRA_POWER_CHECK

	// R&R, ray, Bargaining - START
	decreaseTimeNoTrade();
	// R&R, ray, Bargaining - END

	EXTRA_POWER_CHECK

	// R&R Abandon City, ray START
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->getPopulation() == 0 )
		{
			CvWString szBuffer = gDLL->getText("CITY_ABANDONED", pLoopCity->getNameKey());
			gDLL->getInterfaceIFace()->addMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CITYCAPTURED", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE(), true, true);
			disband(pLoopCity, true);
		}
		else
		{
			pLoopCity->doTurn();
		}
	}
	// R&R  Abandon City, ray END

	// R&R, Little Extra Native Income, ray START
	if (isNative())
	{
		int iNumVillages = getNumCities();

		int villagePerRoundIncomeMax = GC.getPER_ROUND_PER_VILLAGE_INCOME_MAX();
		int villageIncomeLimit = GC.getPER_VILLAGE_FEATURE_GENERATION_LIMIT();

		int featureGenerationLimit = iNumVillages * villageIncomeLimit;
		int currentGold = getGold();

		if (currentGold < featureGenerationLimit)
		{
			int featureGenerationMax = iNumVillages * villagePerRoundIncomeMax;
			int nativeIncome = GC.getGameINLINE().getSorenRandNum(featureGenerationMax, "Small Native Income");
			changeGold(nativeIncome);
		}

	}
	// R&R, Little Extra Native Income, ray END

	EXTRA_POWER_CHECK

	verifyCivics();

	doPrices();
	doAfricaPrices(); // R&R, ray, Africa
	doPortRoyalPrices(); // R&R, ray, Port Royal

	EXTRA_POWER_CHECK

	doEvents();

	EXTRA_POWER_CHECK

	interceptEuropeUnits();

	updateEconomyHistory(GC.getGameINLINE().getGameTurn(), getGold());
	updateIndustryHistory(GC.getGameINLINE().getGameTurn(), calculateTotalYield(YIELD_HAMMERS));
	updateAgricultureHistory(GC.getGameINLINE().getGameTurn(), calculateTotalYield(YIELD_FOOD));
	updatePowerHistory(GC.getGameINLINE().getGameTurn(), getPower());
	updateCultureHistory(GC.getGameINLINE().getGameTurn(), countTotalCulture());
	expireMessages();  // turn log
	m_aszTradeMessages.clear();

	// TAC - Trade Messages - koma13 - START
	m_aeTradeMessageTypes.clear();
	m_aeTradeMessageYields.clear();
	m_aiTradeMessageAmounts.clear();
	m_aiTradeMessageCommissions.clear();
	// TAC - Trade Messages - koma13 - END

	gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);

	AI_doTurnPost();

	gDLL->getEventReporterIFace()->endPlayerTurn( GC.getGameINLINE().getGameTurn(),  getID());

	// keep the vanilla checkPower outside of EXTRA_POWER_CHECK. This way we will be informed if power is broken even without spending time on extra checks.
	FAssert(checkPower(false));
	FAssert(checkPopulation());
}

/** NBMOD TAX **/

int CvPlayer::NBMOD_GetMaxTaxRate() const
{
    return m_iMaxTaxRate;
}

/***************************************************************************/
/**                                                                       **/
/** int CvPlayer::NBMOD_IncreaseMaxTaxRate()                              **/
/**                                                                       **/
/** Diese Methode hebt die Steuergrenze.                                  **/
/**                                                                       **/
/***************************************************************************/

void CvPlayer::NBMOD_IncreaseMaxTaxRate()
{
    m_iMaxTaxRate = m_iMaxTaxRate + GC.getINCREASE_MAX_TAX_RATE();
    if (m_iMaxTaxRate > GC.getMAX_TAX_RATE())
    {
        m_iMaxTaxRate = GC.getMAX_TAX_RATE();
    }
    return ;
}

/***************************************************************************/
/**                                                                       **/
/** int CvPlayer::NBMOD_DecreaseMaxTaxRate()                              **/
/**                                                                       **/
/** Diese Methode senkt die Steuergrenze.                                 **/
/**                                                                       **/
/***************************************************************************/

void CvPlayer::NBMOD_DecreaseMaxTaxRate()
{

    m_iMaxTaxRate = m_iMaxTaxRate - GC.getDECREASE_MAX_TAX_RATE();
    if (m_iMaxTaxRate < GC.getMIN_TAX_RATE())
    {
        m_iMaxTaxRate = GC.getMIN_TAX_RATE();
    }

//NEUER CODE von RAY
	// ist die max_Steuerrate < aktueler_Steuerrate, wird die max_Steuerrate wieder auf aktueler_Steuerrate hochgesetzt
	if (m_iMaxTaxRate < getTaxRate())
	{
		m_iMaxTaxRate = getTaxRate();
	}
//Ende NEUER CODE von RAY

    return ;
}

/***************************************************************************/
/**                                                                       **/
/** int CvPlayer::NBMOD_GetNewTaxRate(int iWantedTax)                     **/
/**                                                                       **/
/** Diese Methode ermittelt die tats�chlich neue Steuer.                  **/
/**                                                                       **/
/** Parameter:                                                            **/
/**  - iWantedTax = gewollte Steuer                                       **/
/**                                                                       **/
/***************************************************************************/

int CvPlayer::NBMOD_GetNewTaxRate(int iWantedTax) const
{
	int iTaxRate = iWantedTax;

	// Ist die neue Steuerrate oberhalb der derzeutigen Grenze?
	if (iWantedTax > m_iMaxTaxRate)
	{
		// Steuerrate begrenzen
		iTaxRate = m_iMaxTaxRate;
	}

	// Ist die aktuelle Steuerrate schon oberhalb der Grenze
	if (getTaxRate() > m_iMaxTaxRate)
	{
		// Steuerrate unver�ndert lassen
		iTaxRate = getTaxRate();
	}

	return iTaxRate;
}
/** NBMOD TAX **/

void CvPlayer::doTurnUnits()
{
	PROFILE_FUNC();

	CvSelectionGroup* pLoopSelectionGroup;
	int iLoop;

	AI_doTurnUnitsPre();

	for (pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup != NULL; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
	{
		pLoopSelectionGroup->doDelayedDeath();
	}


	for (pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup != NULL; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
	{
		pLoopSelectionGroup->doTurn();
	}


	if (getParent() != NO_PLAYER)
	{
		CvPlayer& kEurope = GET_PLAYER(getParent());
		if (kEurope.isAlive() && kEurope.isEurope() && !::atWar(getTeam(), kEurope.getTeam()))
		{
			int iLowestCost = MAX_INT;
			UnitTypes eCheapestShip = NO_UNIT;
			for (int iUnitClass = 0; iUnitClass < GC.getNumUnitClassInfos(); ++iUnitClass)
			{
				UnitTypes eLoopUnit = (UnitTypes) GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iUnitClass);
				if (eLoopUnit != NO_UNIT)
				{
					// TAC - AI Improved Navel AI - koma13 - START
					//if (GC.getUnitInfo(eLoopUnit).getDomainType() == DOMAIN_SEA)
					if (GC.getUnitInfo(eLoopUnit).getDefaultUnitAIType() == UNITAI_TRANSPORT_SEA)
					// TAC - AI Improved Navel AI - koma13 - END
					{
						int iCost = getEuropeUnitBuyPrice(eLoopUnit);
						if (iCost < iLowestCost && iCost >= 0)
						{
							iLowestCost = iCost;
							eCheapestShip = eLoopUnit;
						}
					}
				}
			}

			if (eCheapestShip != NO_UNIT && getGold() < getEuropeUnitBuyPrice(eCheapestShip))
			{
				CvPlot* pBestPlot = getStartingPlot();
				CvPlot* pLoopPlot = NULL;
				int iBestPlotRand = 0;
				for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
				{
					pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
					if (pLoopPlot->isRevealed(getTeam(), false) && pBestPlot->getEurope() == pLoopPlot->getEurope())
					{
						int iPlotRand = (1 + GC.getGameINLINE().getSorenRandNum(1000, "Starting Plot"));
						if (iPlotRand > iBestPlotRand)
						{
							iBestPlotRand = iPlotRand;
							pBestPlot = pLoopPlot;
						}
					}
				}
				setStartingPlot(pBestPlot, true);

				bool bHasShip = false;
				int iLoop;
				for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
				{
					// TAC - AI Improved Navel AI - koma13 - START
					//if (pLoopUnit->getDomainType() == DOMAIN_SEA)
					if (pLoopUnit->AI_getUnitAIType() == UNITAI_TRANSPORT_SEA)
					// TAC - AI Improved Navel AI - koma13 - END
					{
						bHasShip = true;
						break;
					}
				}

				if (!bHasShip)
				{
					for (uint i = 0; i < m_aEuropeUnits.size(); ++i)
					{
						CvUnit* pLoopUnit = m_aEuropeUnits[i];
						// TAC - AI Improved Navel AI - koma13 - START
						//if (pLoopUnit->getDomainType() == DOMAIN_SEA)
						if (pLoopUnit->AI_getUnitAIType() == UNITAI_TRANSPORT_SEA)
						// TAC - AI Improved Navel AI - koma13 - END
						{
							bHasShip = true;
							break;
						}
					}
				}

				/*** TRIANGLETRADE 10/23/08 by DPII ***/
				if (!bHasShip)
				{
					for (uint i = 0; i < m_aAfricaUnits.size(); ++i)
					{
						CvUnit* pLoopUnit = m_aAfricaUnits[i];
						// TAC - AI Improved Navel AI - koma13 - START
						//if (pLoopUnit->getDomainType() == DOMAIN_SEA)
						if (pLoopUnit->AI_getUnitAIType() == UNITAI_TRANSPORT_SEA)
						// TAC - AI Improved Navel AI - koma13 - END
						{
							bHasShip = true;
							break;
						}
					}
				}
				/***********************************/

				// R&R, ray, Port Royal
				if (!bHasShip)
				{
					for (uint i = 0; i < m_aPortRoyalUnits.size(); ++i)
					{
						CvUnit* pLoopUnit = m_aPortRoyalUnits[i];
						// TAC - AI Improved Navel AI - koma13 - START
						//if (pLoopUnit->getDomainType() == DOMAIN_SEA)
						if (pLoopUnit->AI_getUnitAIType() == UNITAI_TRANSPORT_SEA)
						// TAC - AI Improved Navel AI - koma13 - END
						{
							bHasShip = true;
							break;
						}
					}
				}
				// R&R, ray, Port Royal - END

				if (!bHasShip)
				{
					buyEuropeUnit(eCheapestShip, 0);

					//change taxrate
					int iOldTaxRate = getTaxRate();

					/** NBMOD TAX **/
					/** Original
					int iNewTaxRate = std::min(99, iOldTaxRate + 1 + GC.getGameINLINE().getSorenRandNum(GC.getDefineINT("TAX_RATE_MAX_INCREASE"), "Tax Rate Increase for ship"));
					int iChange = iNewTaxRate - iOldTaxRate;
					changeTaxRate(iChange);

					/** NBMOD TAX**/

					int iNewTaxRate = NBMOD_GetNewTaxRate(std::min(99, iOldTaxRate + 1 + GC.getGameINLINE().getSorenRandNum(GC.getDefineINT("TAX_RATE_MAX_INCREASE"), "Tax Rate Increase for ship")));
					int iChange = iNewTaxRate - iOldTaxRate;

					if (iChange > 0)
					{
						changeTaxRate(iChange);
					}

					/** NBMOD TAX **/

					if (isHuman())
					{
						CvDiploParameters* pDiplo = new CvDiploParameters(kEurope.getID());
						pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_KING_GIFT_SHIP"));
						pDiplo->addDiploCommentVariable(iNewTaxRate);
						pDiplo->setAIContact(true);
						gDLL->beginDiplomacy(pDiplo, getID());
					}
				}
			}
		}
	}

	if (getID() == GC.getGameINLINE().getActivePlayer())
	{
		gDLL->getFAStarIFace()->ForceReset(&GC.getInterfacePathFinder());

		gDLL->getInterfaceIFace()->setDirty(Waypoints_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
	}

	gDLL->getInterfaceIFace()->setDirty(UnitInfo_DIRTY_BIT, true);

	AI_doTurnUnitsPost();
}


void CvPlayer::verifyCivics()
{
	for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
	{
		if (!canDoCivics(getCivic((CivicOptionTypes)iI)))
		{
			for (int iJ = 0; iJ < GC.getNumCivicInfos(); iJ++)
			{
				if (GC.getCivicInfo((CivicTypes)iJ).getCivicOptionType() == iI)
				{
					if (canDoCivics((CivicTypes)iJ))
					{
						setCivic(((CivicOptionTypes)iI), ((CivicTypes)iJ));
						break;
					}
				}
			}
		}
	}
}

void CvPlayer::updateYield()
{
	int iLoop;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->updateYield();
	}
}

void CvPlayer::updateCityPlotYield()
{
	int iLoop;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->plot()->updateYield(true);
	}
}


void CvPlayer::updateCitySight(bool bIncrement)
{
	int iLoop;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->plot()->updateSight(bIncrement);
	}
}

void CvPlayer::updateTimers()
{
	CvSelectionGroup* pLoopSelectionGroup;
	int iLoop;

	for(pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
	{
		pLoopSelectionGroup->updateTimers(); // could destroy the selection group...
	}

	// if a unit was busy, perhaps it was not quite deleted yet, give it one more try
	if (getNumSelectionGroups() > getNumUnits())
	{
		for(pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
		{
			pLoopSelectionGroup->doDelayedDeath(); // could destroy the selection group...
		}
	}

	FAssertMsg(getNumSelectionGroups() <= getNumUnits(), "The number of Units is expected not to exceed the number of Selection Groups");
}


bool CvPlayer::hasReadyUnit(bool bAny) const
{
	PROFILE_FUNC();

	CvSelectionGroup* pLoopSelectionGroup;
	int iLoop;

	for(pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
	{
		if (pLoopSelectionGroup->readyToMove(bAny))
		{
			return true;
		}
	}

	return false;
}


bool CvPlayer::hasAutoUnit() const
{
	PROFILE_FUNC();

	CvSelectionGroup* pLoopSelectionGroup;
	int iLoop;

	for(pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
	{
		if (pLoopSelectionGroup->readyToAuto())
		{
			return true;
		}
	}

	return false;
}


bool CvPlayer::hasBusyUnit() const
{
	PROFILE_FUNC();

	CvSelectionGroup* pLoopSelectionGroup;
	int iLoop;

	for(pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
	{
		if (pLoopSelectionGroup->isBusy())
		{
		    if (pLoopSelectionGroup->getNumUnits() == 0)
		    {
		        pLoopSelectionGroup->kill();
		        return false;
		    }

			return true;
		}
	}

	return false;
}

int CvPlayer::calculateScore(bool bFinal, bool bVictory) const
{
	PROFILE_FUNC();

	if (!isAlive())
	{
		return 0;
	}

	if (GET_TEAM(getTeam()).getNumMembers() == 0)
	{
		return 0;
	}

    // R&R, Robert Surcouf, No More Variables Hidden game option START
	//if (isNative())
	if (isNative()&& !GC.getGameINLINE().isOption(GAMEOPTION_NO_MORE_VARIABLES_HIDDEN))
	{
		return 0;
	}
    // R&R, Robert Surcouf, No More Variables Hidden game option END

	long iScore = 0;

	gDLL->getPythonIFace()->pythonCalculateScore(getID(), &iScore, bFinal, bVictory);

	iScore *= getScoreTaxFactor();
	iScore /= 100;

	return ((int)iScore);
}

int CvPlayer::getScoreTaxFactor() const
{
	return std::max(0, 100 - GC.getDefineINT("SCORE_TAX_FACTOR") * getTaxRate() / 100);
}

int CvPlayer::findBestFoundValue() const
{
	CvArea* pLoopArea;
	int iValue;
	int iBestValue;
	int iLoop;

	iBestValue = 0;

	for(pLoopArea = GC.getMapINLINE().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMapINLINE().nextArea(&iLoop))
	{
		iValue = pLoopArea->getBestFoundValue(getID());

		if (iValue > iBestValue)
		{
			iBestValue = iValue;
		}
	}

	return iBestValue;
}


int CvPlayer::upgradeAllPrice(UnitTypes eUpgradeUnit, UnitTypes eFromUnit)
{
	CvUnit* pLoopUnit;
	int iPrice;
	int iLoop;

	iPrice = 0;

	// Loop through units and determine the total power of this player's military
	for (pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit->getUnitType() == eFromUnit)
		{
			if (pLoopUnit->canUpgrade(eUpgradeUnit, true))
			{
				iPrice += pLoopUnit->upgradePrice(eUpgradeUnit);
			}
		}
	}

	return iPrice;
}


int CvPlayer::countNumCoastalCities() const
{
	CvCity* pLoopCity;
	int iCount;
	int iLoop;

	iCount = 0;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
		{
			iCount++;
		}
	}

	return iCount;
}


int CvPlayer::countNumCoastalCitiesByArea(CvArea* pArea) const
{
	CvCity* pLoopCity;
	int iCount;
	int iLoop;

	iCount = 0;

	int iAreaID = pArea->getID();
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
		{
			if ((pLoopCity->getArea() == iAreaID) || pLoopCity->plot()->isAdjacentToArea(iAreaID))
			{
				iCount++;
			}
		}
	}

	return iCount;
}


int CvPlayer::countTotalCulture() const
{
	int iLoop;
	int iCount = 0;

	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iCount += pLoopCity->getCulture(getID());
	}

	return (iCount);
}

int CvPlayer::countTotalYieldStored(YieldTypes eYield) const
{
	int iLoop;
	int iCount = 0;

	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iCount += pLoopCity->getYieldStored(eYield);
	}

	return (iCount);
}

int CvPlayer::countCityFeatures(FeatureTypes eFeature) const
{
	PROFILE_FUNC();

	CvCity* pLoopCity;
	CvPlot* pLoopPlot;
	int iCount;
	int iLoop;
	int iI;

	iCount = 0;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
			pLoopPlot = plotCity(pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE(), iI);

			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->getFeatureType() == eFeature)
				{
					iCount++;
				}
			}
		}
	}

	return iCount;
}


int CvPlayer::countNumBuildings(BuildingTypes eBuilding) const
{
	PROFILE_FUNC();

	CvCity* pLoopCity;
	int iCount;
	int iLoop;

	iCount = 0;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->isHasConceptualBuilding(eBuilding))
		{
			++iCount;
		}
	}

	return iCount;
}


bool CvPlayer::canContact(PlayerTypes ePlayer) const
{
	// R&R, ray, changes to Wild Animals - START
	if (GC.getGameINLINE().isBarbarianPlayer(ePlayer) || GC.getGameINLINE().isBarbarianPlayer(getID()))
	{
		return false;
	}
	// R&R, ray, changes to Wild Animals - END

	if (ePlayer == getID())
	{
		return false;
	}

	if (!isAlive() || !(GET_PLAYER(ePlayer).isAlive()))
	{
		return false;
	}

	if (getTeam() != GET_PLAYER(ePlayer).getTeam())
	{
		if (!(GET_TEAM(getTeam()).isHasMet(GET_PLAYER(ePlayer).getTeam())))
		{
			return false;
		}

		if (atWar(getTeam(), GET_PLAYER(ePlayer).getTeam()))
		{
			if (!(GET_TEAM(getTeam()).canChangeWarPeace(GET_PLAYER(ePlayer).getTeam())))
			{
				return false;
			}

			if (isHuman() || GET_PLAYER(ePlayer).isHuman())
			{
				if (GC.getGameINLINE().isOption(GAMEOPTION_ALWAYS_WAR))
				{
					return false;
				}
			}

			if (GET_TEAM(getTeam()).isParentOf(GET_PLAYER(ePlayer).getTeam()) || GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isParentOf(getTeam()))
			{
				return false;
			}
		}

		// R&R, ray, Continental Guard - START
		/*
		if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isParentOf(getTeam()))
		{
			if (getParent() != ePlayer)
			{
				return false;
			}
		}

		if (GET_TEAM(getTeam()).isParentOf(GET_PLAYER(ePlayer).getTeam()))
		{
			if (GET_PLAYER(ePlayer).getParent() != getID())
			{
				return false;
			}
		}

		if (isEurope() && GET_PLAYER(ePlayer).getParent() != getID())
		{
			return false;
		}

		if (GET_PLAYER(ePlayer).isEurope() && getParent() != ePlayer)
		{
			return false;
		}
		*/
	}

	return true;
}


void CvPlayer::contact(PlayerTypes ePlayer)
{
	CvDiploParameters* pDiplo;

	if (!canContact(ePlayer) || isTurnDone())
	{
		return;
	}

	if (GET_PLAYER(ePlayer).isHuman())
	{
		if (GC.getGameINLINE().isPbem() || GC.getGameINLINE().isHotSeat() || (GC.getGameINLINE().isPitboss() && !gDLL->isConnected(GET_PLAYER(ePlayer).getNetID())))
		{
			if (gDLL->isMPDiplomacy())
			{
				gDLL->beginMPDiplomacy(ePlayer, false, false);
			}
		}
		else
		{
			if (gDLL->getInterfaceIFace()->isFlashing(ePlayer))
			{
				if (!gDLL->getInterfaceIFace()->isDiplomacyLocked())
				{
					gDLL->getInterfaceIFace()->setDiplomacyLocked(true);
					gDLL->sendContactCiv(NETCONTACT_RESPONSE, ePlayer, gDLL->getInterfaceIFace()->getDiplomacyTransportId(ePlayer));
				}
			}
			else
			{
				gDLL->sendContactCiv(NETCONTACT_INITIAL, ePlayer, -1);
			}
		}
	}
	else
	{
		pDiplo = new CvDiploParameters(ePlayer);
		FAssert(pDiplo != NULL);
		if (gDLL->ctrlKey() && getParent() != ePlayer)
		{
			pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_TRADING"));
		}
		gDLL->beginDiplomacy(pDiplo, getID());
	}
}


void CvPlayer::handleDiploEvent(DiploEventTypes eDiploEvent, PlayerTypes ePlayer, int iData1, int iData2)
{
	CvCity* pCity;
	int iI;

	FAssertMsg(ePlayer != getID(), "shouldn't call this function on ourselves");

	switch (eDiploEvent)
	{
	case DIPLOEVENT_CONTACT:
		AI_setFirstContact(ePlayer, true);
		GET_PLAYER(ePlayer).AI_setFirstContact(getID(), true);
		break;

	case DIPLOEVENT_AI_CONTACT:
		break;

	case DIPLOEVENT_FAILED_CONTACT:
		AI_setFirstContact(ePlayer, true);
		GET_PLAYER(ePlayer).AI_setFirstContact(getID(), true);
		break;

	case DIPLOEVENT_GIVE_HELP:
		AI_changeMemoryCount(ePlayer, MEMORY_GIVE_HELP, 1);
		forcePeace(ePlayer);
		break;

	case DIPLOEVENT_REFUSED_HELP:
		AI_changeMemoryCount(ePlayer, MEMORY_REFUSED_HELP, 1);
		break;

	case DIPLOEVENT_ACCEPT_DEMAND:
		AI_changeMemoryCount(ePlayer, MEMORY_ACCEPT_DEMAND, 1);
		forcePeace(ePlayer);
		break;

	case DIPLOEVENT_REJECTED_DEMAND:
		FAssertMsg(GET_PLAYER(ePlayer).getTeam() != getTeam(), "shouldn't call this function on our own team");

		AI_changeMemoryCount(ePlayer, MEMORY_REJECTED_DEMAND, 1);

		if (AI_demandRebukedSneak(ePlayer))
		{
			GET_TEAM(getTeam()).AI_setWarPlan(GET_PLAYER(ePlayer).getTeam(), WARPLAN_PREPARING_LIMITED);
		}
		break;

	case DIPLOEVENT_DEMAND_WAR:
		FAssertMsg(GET_PLAYER(ePlayer).getTeam() != getTeam(), "shouldn't call this function on our own team");

		GET_TEAM(getTeam()).declareWar(GET_PLAYER(ePlayer).getTeam(), false, WARPLAN_LIMITED);
		break;

	case DIPLOEVENT_JOIN_WAR:
		AI_changeMemoryCount(ePlayer, MEMORY_ACCEPTED_JOIN_WAR, 1);
		GET_TEAM(GET_PLAYER(ePlayer).getTeam()).declareWar(((TeamTypes)iData1), false, WARPLAN_DOGPILE);

		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (GET_PLAYER((PlayerTypes)iI).getTeam() == ((TeamTypes)iData1))
				{
					GET_PLAYER((PlayerTypes)iI).AI_changeMemoryCount(getID(), MEMORY_HIRED_WAR_ALLY, 1);
				}
			}
		}
		break;

	case DIPLOEVENT_NO_JOIN_WAR:
		AI_changeMemoryCount(ePlayer, MEMORY_DENIED_JOIN_WAR, 1);
		break;

	case DIPLOEVENT_STOP_TRADING:
		AI_changeMemoryCount(ePlayer, MEMORY_ACCEPTED_STOP_TRADING, 1);
		GET_PLAYER(ePlayer).stopTradingWithTeam((TeamTypes)iData1);

		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (GET_PLAYER((PlayerTypes)iI).getTeam() == ((TeamTypes)iData1))
				{
					GET_PLAYER((PlayerTypes)iI).AI_changeMemoryCount(getID(), MEMORY_HIRED_TRADE_EMBARGO, 1);
				}
			}
		}
		break;

	case DIPLOEVENT_NO_STOP_TRADING:
		AI_changeMemoryCount(ePlayer, MEMORY_DENIED_STOP_TRADING, 1);
		break;

	case DIPLOEVENT_ASK_HELP:
		AI_changeMemoryCount(ePlayer, MEMORY_MADE_DEMAND_RECENT, 1);
		break;

	case DIPLOEVENT_MADE_DEMAND:
		if (AI_getMemoryCount(ePlayer, MEMORY_MADE_DEMAND) < 10)
		{
			AI_changeMemoryCount(ePlayer, MEMORY_MADE_DEMAND, 1);
		}
		AI_changeMemoryCount(ePlayer, MEMORY_MADE_DEMAND_RECENT, 1);
		break;

	case DIPLOEVENT_ACCEPT_TAX_RATE:
		GET_PLAYER(ePlayer).changeTaxRate(iData1);
		break;

	case DIPLOEVENT_REFUSE_TAX_RATE:
		{
			AI_changeMemoryCount(ePlayer, MEMORY_REFUSED_TAX, 1);
			CvPlayer& kPlayer = GET_PLAYER(ePlayer);
			YieldTypes eYield = (YieldTypes) iData1;
			kPlayer.setYieldEuropeTradable(eYield, false);
			// R&R, ray, Improvements to Tax Mechanism - START
			kPlayer.setYieldTradedTotal(eYield, 0);			
			kPlayer.setYieldScoreTotal(eYield, 0);// R&R, vetiarvind, price dependent tax rate change						 
			for (int i = 0; i < NUM_YIELD_TYPES; i++)
			{
				if (kPlayer.isYieldEuropeTradable((YieldTypes)i))
				{
					kPlayer.setYieldTradedTotal((YieldTypes)i, 0);
					kPlayer.setYieldScoreTotal((YieldTypes)i, 0);// R&R, vetiarvind, price dependent tax rate change					 
				}
			}
			// R&R, ray, Improvements to Tax Mechanism - END
			CvCity* pCity = kPlayer.getCity(iData2);
			if (pCity != NULL)
			{
				pCity->setYieldStored(eYield, 0);
				CvWString szMessage = gDLL->getText("TXT_KEY_BOSTON_TEA_PARTY", kPlayer.getCivilizationAdjectiveKey(), pCity->getNameKey(), GC.getYieldInfo(eYield).getTextKeyWide());
				gDLL->getInterfaceIFace()->addMessage(ePlayer, true, GC.getEVENT_MESSAGE_TIME(), szMessage, "AS2D_CITY_REVOLT", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pCity->getX_INLINE(), pCity->getY_INLINE(), true, true);
			}
		}
		break;

	case DIPLOEVENT_ACCEPT_KING_GOLD:
		GET_PLAYER(ePlayer).changeGold(-iData1);
		GET_PLAYER(ePlayer).doREFReduction(iData1);
		/** NBMOD TAX **/
		GET_PLAYER(ePlayer).NBMOD_DecreaseMaxTaxRate();

		/** NBMOD TAX **/
		break;

	case DIPLOEVENT_REFUSE_KING_GOLD:
		AI_changeMemoryCount(ePlayer, MEMORY_REFUSED_TAX, 1);
		/** NBMOD TAX **/
		GET_PLAYER(ePlayer).NBMOD_IncreaseMaxTaxRate();

		/** NBMOD TAX **/
		break;

	case DIPLOEVENT_LIVE_AMONG_NATIVES:
		{
			CvUnit* pUnit = GET_PLAYER(ePlayer).getUnit(iData1);
			if (pUnit != NULL)
			{
				pUnit->doLiveAmongNatives();
			}
		}
		break;

	case DIPLOEVENT_TRANSPORT_TREASURE:
		{
			CvUnit* pUnit = GET_PLAYER(ePlayer).getUnit(iData1);
			if (pUnit != NULL)
			{
				pUnit->doKingTransport();
			}
		}
		break;

	case DIPLOEVENT_FOUND_CITY:
		{
			CvUnit* pUnit = GET_PLAYER(ePlayer).getUnit(iData1);
			if (pUnit != NULL)
			{
				pUnit->doFound(iData2);
			}
		}
		break;

	//ray18
	case DIPLOEVENT_ACQUIRE_CITY:
		{
			CvUnit* pUnit = GET_PLAYER(ePlayer).getUnit(iData1);
			if (pUnit != NULL && iData2 != false)
			{
				pUnit->buyLandAfterAcquire();
			}
		}
		break;
	//Ende ray18

	//TAC Native Mercs
	case DIPLOEVENT_ACQUIRE_MERC:
		{
			int pricetopay = iData1;
			bool mightbeangry = iData2;

			if(mightbeangry) {
				pricetopay = pricetopay / 2;
			}

			GET_PLAYER(ePlayer).buyNativeMercs(getID(), pricetopay, mightbeangry);
		}
		break;
	//End TAC Native Mercs

	// R&R, ray, Native Slave, START
	case DIPLOEVENT_ACQUIRE_NATIVE_SLAVE:
		{
			int pricetopay = iData1;
			CvPlayer& kPlayer = GET_PLAYER(ePlayer);

			int availableGold = kPlayer.getGold();
			if (availableGold >= pricetopay)
			{
				//get City
				int iLoop;
				CvCity* locationToAppear = kPlayer.firstCity(&iLoop); 

				if (locationToAppear != NULL)
				{
					UnitTypes SlaveType = (UnitTypes)GC.getCivilizationInfo(kPlayer.getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_NATIVE_SLAVE"));
					CvUnit* SlaveUnit = kPlayer.initUnit(SlaveType, GC.getUnitInfo(SlaveType).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
				
					//pay 
					changeGold(pricetopay);
					kPlayer.changeGold(-pricetopay);

					CvWString szBuffer = gDLL->getText("TXT_KEY_BOUGHT_NATIVE_SLAVE", locationToAppear->getNameKey());
					gDLL->getInterfaceIFace()->addMessage(ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, SlaveUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), SlaveUnit->getX(), SlaveUnit->getY(), true, true);
				}
			}
		}
		break;
	// R&R, ray, Native Slave, END


	// R&R, ray, African Slaves - START
	case DIPLOEVENT_ACQUIRE_AFRICAN_SLAVES:
		{
			int iNumSlaves = iData1;
			FAssertMsg(iNumSlaves > 0, CvString::format("Trying to add %d African slaves", iNumSlaves).c_str());
			//int pricetopay = iData2;
			CvPlayer& kPlayer = GET_PLAYER(ePlayer);

			int baseslaveprice = GC.getDefineINT("BASE_AFRICAN_SLAVES_PRICE");
			int discount = AI_getAttitude(ePlayer, false) * 10;
			int totalslavesprice = baseslaveprice - discount;
			int gamespeedMod = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
			int pricetopay = iNumSlaves * totalslavesprice * gamespeedMod / 100;
			
			int availableGold = kPlayer.getGold();
			if (availableGold >= pricetopay && iNumSlaves > 0)
			{
				//get City
				int iLoop;
				CvCity* locationToAppear = kPlayer.firstCity(&iLoop);

				if (locationToAppear != NULL)
				{
					UnitTypes SlaveType = (UnitTypes)GC.getCivilizationInfo(kPlayer.getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_AFRICAN_SLAVE"));
					CvUnit* pSlaveUnit = NULL;
					for (int i=0; i < iNumSlaves; ++i)
					{
						pSlaveUnit = kPlayer.initUnit(SlaveType, GC.getUnitInfo(SlaveType).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
					}
					FAssert(pSlaveUnit != NULL);
					//pay the king
					changeGold(pricetopay);
					AI_changeAttitudeExtra(ePlayer, 1);
					kPlayer.changeGold(-pricetopay);

					CvWString szBuffer = gDLL->getText("TXT_KEY_BOUGHT_AFRICAN_SLAVE", locationToAppear->getNameKey());
					gDLL->getInterfaceIFace()->addMessage(ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, pSlaveUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pSlaveUnit->getX(), pSlaveUnit->getY(), true, true);
				}
			}
		}
		break;
	// R&R, ray, African Slaves - END

	// R&R, ray, Prisons Crowded - START
	case DIPLOEVENT_ACQUIRE_PRISONERS:
		{
			int iNumPrisoners = iData1;
			FAssertMsg(iNumPrisoners > 0, CvString::format("Trying to add %d prisoners", iNumPrisoners).c_str());
			//int pricetopay = iData2;
			CvPlayer& kPlayer = GET_PLAYER(ePlayer);

			int baseprisonerprice = GC.getDefineINT("BASE_PRISONER_PRICE");
			int discount = AI_getAttitude(ePlayer, false) * 10;
			int totalprisonersprice = baseprisonerprice - discount;
			int gamespeedMod = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
			int pricetopay = iNumPrisoners * totalprisonersprice * gamespeedMod / 100;

			int availableGold = kPlayer.getGold();
			if (availableGold >= pricetopay && iNumPrisoners > 0)
			{	

				//create the prisoners
				//get City
				int iLoop;
				CvCity* locationToAppear = kPlayer.firstCity(&iLoop);

				if (locationToAppear != NULL)
				{
					UnitTypes ePrisonerType = (UnitTypes)GC.getCivilizationInfo(kPlayer.getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_PRISONER"));
					CvUnit* pPrisonerUnit = NULL;
					for (int i=0; i < iNumPrisoners; ++i)
					{
						pPrisonerUnit = kPlayer.initUnit(ePrisonerType, GC.getUnitInfo(ePrisonerType).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
					}
					//pay the king
					changeGold(pricetopay);
					AI_changeAttitudeExtra(ePlayer, 1);
					if (availableGold < pricetopay)
					{
						pricetopay = availableGold;
					}
					kPlayer.changeGold(-pricetopay);

					CvWString szBuffer = gDLL->getText("TXT_KEY_BOUGHT_PRISONERS", locationToAppear->getNameKey());
					gDLL->getInterfaceIFace()->addMessage(ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, pPrisonerUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pPrisonerUnit->getX(), pPrisonerUnit->getY(), true, true);
				}
			}
		}
		break;
	// R&R, ray, Prisons Crowded - END

	// R&R, ray, Revolutionary Noble - START
	case DIPLOEVENT_ACQUIRE_NOBLE:
		{
			int pricetopay = iData1;
			CvPlayer& kPlayer = GET_PLAYER(ePlayer);

			int availableGold = kPlayer.getGold();
			if (availableGold >= pricetopay)
			{
				//create the noble
				//get City
				int iLoop;
				CvCity* locationToAppear = kPlayer.firstCity(&iLoop);

				if (locationToAppear != NULL)
				{

					UnitTypes NobleType = (UnitTypes)GC.getCivilizationInfo(kPlayer.getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_NOBLE"));
					CvUnit* NobleUnit;
					NobleUnit = kPlayer.initUnit(NobleType, GC.getUnitInfo(NobleType).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);

					//pay the king
					changeGold(pricetopay);
					AI_changeAttitudeExtra(ePlayer, 5);
					kPlayer.changeGold(-pricetopay);

					//add message
					CvWString szBuffer = gDLL->getText("TXT_KEY_BOUGHT_NOBLE", locationToAppear->getNameKey());
					gDLL->getInterfaceIFace()->addMessage(ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, NobleUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), NobleUnit->getX(), NobleUnit->getY(), true, true);
				}
			}
		}
		break;
	// R&R, ray, Revolutionary Noble - END

	// R&R, ray, Bishop - START
	case DIPLOEVENT_ACQUIRE_BISHOP:
		{
			int pricetopay = iData1;
			CvPlayer& kPlayer = GET_PLAYER(ePlayer);

			int availableGold = kPlayer.getGold();
			if (availableGold >= pricetopay)
			{
				//create the bishop
				//get City
				int iLoop;
				CvCity* locationToAppear = kPlayer.firstCity(&iLoop);

				if (locationToAppear != NULL)
				{

					UnitTypes BishopType = (UnitTypes)GC.getCivilizationInfo(kPlayer.getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_BISHOP"));
					CvUnit* BishopUnit;
					BishopUnit = kPlayer.initUnit(BishopType, GC.getUnitInfo(BishopType).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);

					//pay the church
					changeGold(pricetopay);
					kPlayer.changeGold(-pricetopay);

					//add message
					CvWString szBuffer = gDLL->getText("TXT_KEY_BOUGHT_BISHOP", locationToAppear->getNameKey());
					gDLL->getInterfaceIFace()->addMessage(ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, BishopUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), BishopUnit->getX(), BishopUnit->getY(), true, true);
				}
			}
		}
		break;
	// R&R, ray, Bishop - END

	// R&R, ray, the Church - START
	case DIPLOEVENT_PAY_CHURCH_DEMAND:
		{
			int pricetopay = iData1;
			CvPlayer& kPlayer = GET_PLAYER(ePlayer); // human

			int availableGold = kPlayer.getGold();
			if (availableGold >= pricetopay)
			{
				//pay the church
				changeGold(pricetopay);
				kPlayer.changeGold(-pricetopay);

				int iLoop;
				CvCity* pCity = kPlayer.firstCity(&iLoop);

				if (pCity != NULL)
				{
					//message general part
					CvWString szBuffer = gDLL->getText("TXT_KEY_PAYED_CHURCH_DEMAND");

					//effects of choice
					AI_changeAttitudeExtra(ePlayer, 1);

					int effectRand = GC.getGameINLINE().getSorenRandNum(6, "ChurchEffectRand");	
					switch (effectRand)
					{
						case 0:
							{
								// immigration threshold lowered
								kPlayer.setImmigrationThresholdMultiplier((kPlayer.getImmigrationThresholdMultiplier() * 90) / 100);
								szBuffer += gDLL->getText("TXT_KEY_CHURCH_REWARD_IMMIGRATION");

								// To prevent broken hurry button, do immigration, if Threshold got lowered below current crosses
								while (getCrossesStored() >= immigrationThreshold())
								{
									doImmigrant(GC.getGameINLINE().getSorenRandNum(CivEffect()->getNumUnitsOnDock(), "pick immigrant"), true);
								}

								break;
							}
						case 1:
							{
								//attitude of King improved
								GET_PLAYER(kPlayer.getParent()).AI_changeAttitudeExtra(ePlayer, 1); //attitude of King improved
								szBuffer += gDLL->getText("TXT_KEY_CHURCH_REWARD_ATTITUDE_KING");
								break;
							}
						case 2:
							{
								// attitude of other Europeans improved
								for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
								{
									CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes) iPlayer);
									if (kLoopPlayer.isAlive() && !kLoopPlayer.isNative() && !kLoopPlayer.isEurope() && kLoopPlayer.getID() != getID() && kLoopPlayer.getID() != kPlayer.getID())
									{
										kLoopPlayer.AI_changeAttitudeExtra(ePlayer, 1);
									}
								}
								szBuffer += gDLL->getText("TXT_KEY_CHURCH_REWARD_ATTITUDE_EUROPEANS");
								break;
							}
						case 3:
							{
								// immigration threshold lowered
								kPlayer.setImmigrationThresholdMultiplier((kPlayer.getImmigrationThresholdMultiplier() * 90) / 100);
								szBuffer += gDLL->getText("TXT_KEY_CHURCH_REWARD_IMMIGRATION");

								// To prevent broken hurry button, do immigration, if Threshold got lowered below current crosses
								while (getCrossesStored() >= immigrationThreshold())
								{
									doImmigrant(GC.getGameINLINE().getSorenRandNum(CivEffect()->getNumUnitsOnDock(), "pick immigrant"), true);
								}

								break;
							}
						case 4:
							{
								// new Missionary
								UnitTypes ChurchRewardUnitType = (UnitTypes)GC.getCivilizationInfo(kPlayer.getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_CHURCH_REWARD1"));
								CvUnit* ChurchRewardUnit;
								ChurchRewardUnit = kPlayer.initUnit(ChurchRewardUnitType, GC.getUnitInfo(ChurchRewardUnitType).getDefaultProfession(), pCity->getX_INLINE(), pCity->getY_INLINE(), NO_UNITAI);
								szBuffer += gDLL->getText("TXT_KEY_CHURCH_REWARD_MISSIONARY");
								break;
							}
						case 5:
							{
								// new Preacher or Priest
								UnitTypes ChurchRewardUnitType = (UnitTypes)GC.getCivilizationInfo(kPlayer.getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_CHURCH_REWARD2"));
								CvUnit* ChurchRewardUnit;
								ChurchRewardUnit = kPlayer.initUnit(ChurchRewardUnitType, GC.getUnitInfo(ChurchRewardUnitType).getDefaultProfession(), pCity->getX_INLINE(), pCity->getY_INLINE(), NO_UNITAI);
								szBuffer += gDLL->getText("TXT_KEY_PAYED_CHURCH_REWARD_PREACHER");
								break;
							}
						default: // just for safety
							break;
					}

					gDLL->getInterfaceIFace()->addMessage(ePlayer, true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pCity->getX_INLINE(), pCity->getY_INLINE(), true, true);
				}
			}
		}
		break;

	case DIPLOEVENT_REFUSE_CHURCH_DEMAND:
		{
			CvPlayer& kPlayer = GET_PLAYER(ePlayer); // human

			int iLoop;
			CvCity* pCity = kPlayer.firstCity(&iLoop);

			if (pCity != NULL)
			{

				//add message generalpart
				CvWString szBuffer = gDLL->getText("TXT_KEY_REFUSED_CHURCH_DEMAND");

				//effects of choice
				AI_changeAttitudeExtra(ePlayer, -1); // attitude of church

				int effectRand = GC.getGameINLINE().getSorenRandNum(5, "ChurchEffectRand");	
				switch (effectRand)
				{
					case 0:
						{
							// immigration threshold increased
							kPlayer.setImmigrationThresholdMultiplier(((kPlayer.getImmigrationThresholdMultiplier() * 110) / 100));
							szBuffer += gDLL->getText("TXT_KEY_CHURCH_PENALTY_IMMIGRATION");
							break;
						}
					case 1:
						{
							//attitude of King worsened
							GET_PLAYER(kPlayer.getParent()).AI_changeAttitudeExtra(ePlayer, -1);
							szBuffer += gDLL->getText("TXT_KEY_CHURCH_PENALTY_ATTITUDE_KING");
							break;
						}
					case 2:
						{
							// attitude of other Europeans worsened
							for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
							{
								CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes) iPlayer);
								if (kLoopPlayer.isAlive() && !kLoopPlayer.isNative() && !kLoopPlayer.isEurope() && kLoopPlayer.getID() != getID() && kLoopPlayer.getID() != kPlayer.getID())
								{
									kLoopPlayer.AI_changeAttitudeExtra(ePlayer, -1);
								}
							}	
							szBuffer += gDLL->getText("TXT_KEY_CHURCH_PENALTY_ATTITUDE_EUROPEANS");
							break;
						}
					case 3:
						{
							// unrest in capital
							int iLoop;
							CvCity* pCity = kPlayer.firstCity(&iLoop);

							if (pCity != NULL)
							{
								pCity->setOccupationTimer(2);
							}
							szBuffer += gDLL->getText("TXT_KEY_CHURCH_PENALTY_UNREST");
							break;
						}
					case 4:
						{
							// immigration threshold increased
							kPlayer.setImmigrationThresholdMultiplier(((kPlayer.getImmigrationThresholdMultiplier() * 110) / 100));
							szBuffer += gDLL->getText("TXT_KEY_CHURCH_PENALTY_IMMIGRATION");
							break;
						}
					default: // just for safety
						break;
				}

				gDLL->getInterfaceIFace()->addMessage(ePlayer, true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CITY_REVOLT", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pCity->getX_INLINE(), pCity->getY_INLINE(), true, true);
			}
		}
		break;
	// R&R, ray, the Church - END

	// R&R, ray, Church Favours - START
	case DIPLOEVENT_CHURCH_FAVOUR:
		{
			CvPlayer& kPlayer = GET_PLAYER(ePlayer); // human
			int pricetopay = kPlayer.getChurchFavourPrice();

			// for safety
			if (kPlayer.getGold() >= pricetopay)
			{
				//pay the church
				changeGold(pricetopay);
				kPlayer.changeGold(-pricetopay);

				//increase counter that affects price for church favour
				kPlayer.increaseNumChurchFavoursReceived();

				// check the case
				int iFavourCase = iData1;
				switch (iFavourCase)
				{
					// change attitude of King
					case 1:
					{
						GET_PLAYER(kPlayer.getParent()).AI_changeAttitudeExtra(ePlayer, 1); //attitude of King improved

						CvWString szBuffer = gDLL->getText("TXT_KEY_CHURCH_SPOKE_FAVOUR_KING");
						gDLL->getInterfaceIFace()->addMessage(ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), NULL, NULL, false, false);
						break;
					}
					// change attitude of selected European
					case 2:
					{
						CvPlayer& kEuropePlayer = GET_PLAYER((PlayerTypes) iData2);
						kEuropePlayer.AI_changeAttitudeExtra(ePlayer, 1); //attitude of selected European improved
		
						CvWString szBuffer = gDLL->getText("TXT_KEY_CHURCH_SPOKE_FAVOUR_EUROPEAN", kEuropePlayer.getNameKey());
						gDLL->getInterfaceIFace()->addMessage(ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), NULL, NULL, false, false);
						break;
					}
					default: // just for safety
						break;
				}
			}
			
		}
		break;
	// R&R, ray, Church Favours - END

	// R&R, ray, Church War - START
	case DIPLOEVENT_CHURCH_WAR:
		{
			//getting the Data from Diplo-Event
			PlayerTypes enemyID = (PlayerTypes) iData1; // Selection of Native to Declare War
			int choice = iData2; // the selection choice

			CvPlayer& kPlayer = GET_PLAYER(ePlayer); // Human Player
			CvPlayer& kNativePlayer = GET_PLAYER(enemyID); // Native to declare war

			// we have chosen to obey for the prestige
			if(choice == 1)
			{
				// reward
				AI_changeAttitudeExtra(ePlayer, 4); // Church Attitude improvment
				GET_PLAYER(kPlayer.getParent()).AI_changeAttitudeExtra(ePlayer, 2); // Parent Attitude improvment
				kPlayer.setImmigrationThresholdMultiplier((kPlayer.getImmigrationThresholdMultiplier() * 90) / 100); // lower immigration threshold

				//declaring limited war to Native
				GET_TEAM(kPlayer.getTeam()).declareWar(kNativePlayer.getTeam(),false, WARPLAN_LIMITED);

				//sending message
				CvWString szBuffer = gDLL->getText("TXT_KEY_CHURCH_WAR_ACCEPTED_PRESTIGE", kNativePlayer.getNameKey());
				gDLL->getInterfaceIFace()->addMessage(ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), NULL, NULL, false, false);
			}

			// we have chosen to obey but asked for troops
			else if(choice == 2)
			{
				//get City
				CvCity* pLoopCity = NULL;
				CvCity* locationToAppear = NULL;
				int iLoop;
				for (pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
				{
					if (pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
					{
						locationToAppear = pLoopCity;
						break;
					}
				}
				if (locationToAppear != NULL)
				{

					UnitTypes ChurchReinforcementTypeInfantery = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_CONQUISTADOR"));
					UnitTypes ChurchReinforcementTypeCavalery = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_MOUNTED_CONQUISTADOR"));
					UnitTypes ChurchReinforcementTypeArtillery = (UnitTypes)GC.getCivilizationInfo(GET_PLAYER(kPlayer.getParent()).getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_KING_REINFORCEMENT_ARTIL"));

					CvUnit* ChurchReinforcementUnitInfantery = kPlayer.initUnit(ChurchReinforcementTypeInfantery, GC.getUnitInfo(ChurchReinforcementTypeInfantery).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
					CvUnit* ChurchReinforcementUnitCavalery = kPlayer.initUnit(ChurchReinforcementTypeCavalery, GC.getUnitInfo(ChurchReinforcementTypeCavalery).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
					CvUnit* ChurchReinforcementUnitArtillery = kPlayer.initUnit(ChurchReinforcementTypeArtillery, GC.getUnitInfo(ChurchReinforcementTypeArtillery).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
					
					//declaring limited war
					GET_TEAM(kPlayer.getTeam()).declareWar(GET_PLAYER(enemyID).getTeam(),false, WARPLAN_LIMITED);

					//sending message
					CvWString szBuffer = gDLL->getText("TXT_KEY_CHURCH_WAR_ACCEPTED_TROOPS", kNativePlayer.getNameKey());
					gDLL->getInterfaceIFace()->addMessage(ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, ChurchReinforcementUnitInfantery->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), ChurchReinforcementUnitInfantery->getX(), ChurchReinforcementUnitInfantery->getY(), true, true);
				}
			}

			// we have chosen to disobey
			else
			{		
				//improve relationship to the potential Native enemy
				GET_PLAYER(enemyID).AI_changeAttitudeExtra(ePlayer, 2);

				AI_changeAttitudeExtra(ePlayer, -2); // Church Attitude worsened
				GET_PLAYER(kPlayer.getParent()).AI_changeAttitudeExtra(ePlayer, -1); // Parent Attitude worsened

				//sending message
				CvWString szBuffer = gDLL->getText("TXT_KEY_CHURCH_WAR_REFUSED", kNativePlayer.getNameKey());
				gDLL->getInterfaceIFace()->addMessage(ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), NULL, NULL, false, false);
			}
		}
		break;
	// R&R, ray, Church War - END

	// R&R, ray, Smuggling - START
	case DIPLOEVENT_ACQUIRE_SMUGGLERS:
		{
			int pricetopay = iData1;
			CvPlayer& kPlayer = GET_PLAYER(ePlayer);

			int availableGold = kPlayer.getGold();
			if (availableGold >= pricetopay)
			{
				//get City
				CvCity* pLoopCity = NULL;
				CvCity* locationToAppear = NULL;
				int iLoop;
				for (pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
				{
					if (pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
					{
						locationToAppear = pLoopCity;
						break;
					}			
				}

				if (locationToAppear!= NULL)
				{
					//create the smuggling ship
					UnitTypes SmugglingShipType = (UnitTypes)GC.getCivilizationInfo(kPlayer.getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_SMUGGLING_SHIP"));
					CvUnit* SmugglingShipUnit;
					SmugglingShipUnit = kPlayer.initUnit(SmugglingShipType, GC.getUnitInfo(SmugglingShipType).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
					//pay
					kPlayer.changeGold(-pricetopay);
					//add message
					CvWString szBuffer = gDLL->getText("TXT_KEY_BOUGHT_SMUGGLERS", locationToAppear->getNameKey());
					gDLL->getInterfaceIFace()->addMessage(ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, SmugglingShipUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), SmugglingShipUnit->getX(), SmugglingShipUnit->getY(), true, true);
				}
			}
		}
		break;
	// R&R, ray, Smuggling - END


	// R&R, ray, Rangers - START
	case DIPLOEVENT_ACQUIRE_RANGERS:
		{
			int pricetopay = iData1;
			CvPlayer& kPlayer = GET_PLAYER(ePlayer);

			int availableGold = kPlayer.getGold();
			if (availableGold >= pricetopay)
			{
				//get City
				CvCity* pLoopCity = NULL;
				CvCity* locationToAppear = NULL;
				int iLoop;
				for (pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
				{
					if (!pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
					{
						locationToAppear = pLoopCity;
						break;
					}
				}

				if (locationToAppear!= NULL)
				{
					//create the ranger
					UnitTypes RangerType = (UnitTypes)GC.getCivilizationInfo(kPlayer.getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_RANGER"));
					CvUnit* RangerUnit;
					RangerUnit = kPlayer.initUnit(RangerType, GC.getUnitInfo(RangerType).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
					//pay
					kPlayer.changeGold(-pricetopay);
					//add message
					CvWString szBuffer = gDLL->getText("TXT_KEY_TRAINED_RANGERS", locationToAppear->getNameKey());
					gDLL->getInterfaceIFace()->addMessage(ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, RangerUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), RangerUnit->getX(), RangerUnit->getY(), true, true);
				}
			}
		}
		break;
	// R&R, ray, Rangers - END


	// R&R, ray, Conquistadors - START
	case DIPLOEVENT_ACQUIRE_CONQUISTADORS:
		{
			int pricetopay = iData1;
			CvPlayer& kPlayer = GET_PLAYER(ePlayer);

			int availableGold = kPlayer.getGold();
			if (availableGold >= pricetopay)
			{
				//get City
				CvCity* pLoopCity = NULL;
				CvCity* locationToAppear = NULL;
				int iLoop;
				for (pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
				{
					if (pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
					{
						locationToAppear = pLoopCity;
						break;
					}
				}

				if (locationToAppear!= NULL)
				{
					//create the conquistador
					UnitTypes ConquistadorType;
					int conquistUnitRand = GC.getGameINLINE().getSorenRandNum(3, "Conquistadors Available");
					if (conquistUnitRand == 1)
					{
						ConquistadorType = (UnitTypes)GC.getCivilizationInfo(kPlayer.getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_MOUNTED_CONQUISTADOR"));
					}
					else
					{
						ConquistadorType = (UnitTypes)GC.getCivilizationInfo(kPlayer.getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_CONQUISTADOR"));
					}
					CvUnit* ConquistadorUnit;
					ConquistadorUnit = kPlayer.initUnit(ConquistadorType, GC.getUnitInfo(ConquistadorType).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
					//pay
					kPlayer.changeGold(-pricetopay);
					//add message
					CvWString szBuffer = gDLL->getText("TXT_KEY_HIRED_CONQUISTADORS", locationToAppear->getNameKey());
					gDLL->getInterfaceIFace()->addMessage(ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, ConquistadorUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), ConquistadorUnit->getX(), ConquistadorUnit->getY(), true, true);
				}
			}
		}
		break;
	// R&R, ray, Conquistadors - END


	// R&R, ray, Pirates - START
	case DIPLOEVENT_ACQUIRE_PIRATES:
		{
			int pricetopay = iData1;
			CvPlayer& kPlayer = GET_PLAYER(ePlayer);

			int availableGold = kPlayer.getGold();
			if (availableGold >= pricetopay)
			{
				//get City
				CvCity* pLoopCity = NULL;
				CvCity* locationToAppear = NULL;
				int iLoop;
				for (pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
				{
					if (pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()) && pLoopCity->plot()->hasAnyOtherWaterPlotsThanJustLargeRivers())
					{
						locationToAppear = pLoopCity;
						break;
					}
				}

				if (locationToAppear!= NULL)
				{
					//create the smuggling ship
					UnitTypes PirateShipType = (UnitTypes)GC.getCivilizationInfo(kPlayer.getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_PIRATE_FRIGATE"));
					CvUnit* PirateShipUnit;
					PirateShipUnit = kPlayer.initUnit(PirateShipType, GC.getUnitInfo(PirateShipType).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
					//pay
					kPlayer.changeGold(-pricetopay);
					//add message
					CvWString szBuffer = gDLL->getText("TXT_KEY_HIRED_PIRATES", locationToAppear->getNameKey());
					gDLL->getInterfaceIFace()->addMessage(ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, PirateShipUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), PirateShipUnit->getX(), PirateShipUnit->getY(), true, true);
				}
			}
		}
		break;

	case DIPLOEVENT_BRIBE_PIRATES:
		{
			int pricetopay = iData1 / 10;
			CvPlayer& kPlayer = GET_PLAYER(ePlayer);

			int availableGold = kPlayer.getGold();
			if (availableGold >= pricetopay)
			{
				kPlayer.changeGold(-pricetopay);
			}
			else
			{
				kPlayer.changeGold(-availableGold);
			}
		}
		break;

	case DIPLOEVENT_CREATE_ENEMY_PIRATES:
		{
			createEnemyPirates();
		}
		break;
	// R&R, ray, Pirates - END


	// R&R, ray, Continental Guard - START
	case DIPLOEVENT_ACQUIRE_CONTINENTAL_GUARD:
		{
			int pricetopay = iData1;
			CvPlayer& kPlayer = GET_PLAYER(ePlayer);

			int availableGold = kPlayer.getGold();
			if (availableGold >= pricetopay)
			{
				//get City
				CvCity* pLoopCity = NULL;
				CvCity* locationToAppear = NULL;
				int iLoop;
				for (pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
				{
					if (pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
					{
						locationToAppear = pLoopCity;
						break;
					}
				}

				if (locationToAppear!= NULL)
				{
					//create the Continental Guard
					UnitTypes ContinentalGuardType = (UnitTypes)GC.getCivilizationInfo(kPlayer.getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_CONTINENTAL_GUARD"));
					CvUnit* ContinentalGuardUnit;
					ContinentalGuardUnit = kPlayer.initUnit(ContinentalGuardType, GC.getUnitInfo(ContinentalGuardType).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
					//pay
					kPlayer.changeGold(-pricetopay);
					//add message
					CvWString szBuffer = gDLL->getText("TXT_KEY_ACQUIRED_CONTINENTAL_GUARD", locationToAppear->getNameKey());
					gDLL->getInterfaceIFace()->addMessage(ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, ContinentalGuardUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), ContinentalGuardUnit->getX(), ContinentalGuardUnit->getY(), true, true);
				}
			}
		}
		break;
	// R&R, ray, Continental Guard - END

	// R&R, ray, Mortar - START
	case DIPLOEVENT_ACQUIRE_MORTAR:
		{
			int pricetopay = iData1;
			CvPlayer& kPlayer = GET_PLAYER(ePlayer);

			int availableGold = kPlayer.getGold();
			if (availableGold >= pricetopay)
			{
				//get City
				CvCity* pLoopCity = NULL;
				CvCity* locationToAppear = NULL;
				int iLoop;
				for (pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
				{
					if (pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
					{
						locationToAppear = pLoopCity;
						break;
					}
				}

				if (locationToAppear!= NULL)
				{
					//create the Continental Guard
					UnitTypes MortarType = (UnitTypes)GC.getCivilizationInfo(kPlayer.getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_MORTAR"));
					CvUnit* MortarUnit;
					MortarUnit = kPlayer.initUnit(MortarType, GC.getUnitInfo(MortarType).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
					//pay
					kPlayer.changeGold(-pricetopay);
					//add message
					CvWString szBuffer = gDLL->getText("TXT_KEY_ACQUIRED_MORTAR", locationToAppear->getNameKey());
					gDLL->getInterfaceIFace()->addMessage(ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, MortarUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), MortarUnit->getX(), MortarUnit->getY(), true, true);
				}
			}
		}
		break;
	// R&R, ray, Mortar - END

	//TAC Revolution Support
	case DIPLOEVENT_REV_SUPPORT:
		{
			bool choosenLandSupport = iData1;
			int supportAmount;

			CvPlayer& kPlayer = GET_PLAYER(ePlayer);

			//get City
			CvCity* pLoopCity = NULL;
			CvCity* locationToAppear = NULL;
			int iLoop;
			for (pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				if (pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
				{
					locationToAppear = pLoopCity;
					break;
				}
			}

			if (locationToAppear != NULL)
			{
				UnitTypes DefaultSupportType;
				CvUnit* pSupportUnit = NULL;

				if(choosenLandSupport) {
					supportAmount = GC.getDefineINT("REV_SUPPORT_LAND");
					//getting UnitType by parent
					DefaultSupportType = (UnitTypes)GC.getCivilizationInfo(GET_PLAYER(kPlayer.getParent()).getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_REV_SUPPORT_LAND"));
				}

				else
				{
					supportAmount = GC.getDefineINT("REV_SUPPORT_SEA");
					DefaultSupportType = (UnitTypes)GC.getCivilizationInfo(GET_PLAYER(kPlayer.getParent()).getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_REV_SUPPORT_SEA"));
				}
				FAssert(supportAmount > 0);

				for (int i=0;i<supportAmount;i++)
				{
					pSupportUnit = kPlayer.initUnit(DefaultSupportType, GC.getUnitInfo(DefaultSupportType).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
				}

				//sending message
				CvWString szBuffer = gDLL->getText("TXT_KEY_REV_SUPPORT_ARRIVED", GC.getLeaderHeadInfo(GET_PLAYER(getParent()).getLeaderType()).getDescription());
				gDLL->getInterfaceIFace()->addMessage(ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, pSupportUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pSupportUnit->getX(), pSupportUnit->getY(), true, true);
			}
		}
		break;
	//End TAC Revolution Support

	//TAC European Wars
	case DIPLOEVENT_EUROPE_WAR:
		{
			//getting the Data from Diplo-Event
			PlayerTypes enemyID = (PlayerTypes) iData1;
			CvPlayer& kPlayer = GET_PLAYER(ePlayer);

			PlayerTypes parentID = kPlayer.getParent();
			int choice = iData2;

			// we have chosen to obey
			if(choice == 1)
			{
				//decreasing max tax rate
				int maxtaxdecrease = GC.getDefineINT("POS_TAX_EUROPE_WARS");
				int currentmaxtax = kPlayer.NBMOD_GetMaxTaxRate();

				int absolutMinTaxrate = GC.getMIN_TAX_RATE();
				while ((currentmaxtax - maxtaxdecrease) < kPlayer.NBMOD_GetMaxTaxRate() && kPlayer.NBMOD_GetMaxTaxRate() > absolutMinTaxrate && kPlayer.getTaxRate() < kPlayer.NBMOD_GetMaxTaxRate() )
				{
					kPlayer.NBMOD_DecreaseMaxTaxRate();
				}
				//declaring limited war
				GET_TEAM(kPlayer.getTeam()).declareWar(GET_PLAYER(enemyID).getTeam(),false, WARPLAN_LIMITED);

				int taxchange = currentmaxtax - kPlayer.NBMOD_GetMaxTaxRate();

				//sending message
				CvWString szBuffer = gDLL->getText("TXT_KEY_EUROPE_WAR_KING_DECREASED_MAX_TAX", taxchange);
				gDLL->getInterfaceIFace()->addMessage(ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), NULL, NULL, false, false);
			}

			// we have chosen to obey but asked for troops
			else if(choice == 2)
			{
				int reinforcementAmountLand = GC.getDefineINT("KING_REINFORCEMENT_LAND");
				int reinforcementAmountArtil = GC.getDefineINT("KING_REINFORCEMENT_ARTIL");
				int reinforcementAmountSea = GC.getDefineINT("KING_REINFORCEMENT_SEA");
				//getting UnitType by parent
				UnitTypes KingReinforcementTypeLand = (UnitTypes)GC.getCivilizationInfo(GET_PLAYER(kPlayer.getParent()).getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_KING_REINFORCEMENT_LAND"));
				UnitTypes KingReinforcementTypeArtil = (UnitTypes)GC.getCivilizationInfo(GET_PLAYER(kPlayer.getParent()).getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_KING_REINFORCEMENT_ARTIL"));
				UnitTypes KingReinforcementTypeSea = (UnitTypes)GC.getCivilizationInfo(GET_PLAYER(kPlayer.getParent()).getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_KING_REINFORCEMENT_SEA"));

				CvUnit* ReinforcementUnit = NULL;
				//get City
				CvCity* pLoopCity = NULL;
				CvCity* locationToAppear = NULL;
				int iLoop;
				for (pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
				{
					if (pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
					{
						locationToAppear = pLoopCity;
						break;
					}
				}
				if (locationToAppear != NULL)
				{
					for (int i=0;i<reinforcementAmountLand;i++)
					{
						ReinforcementUnit = kPlayer.initUnit(KingReinforcementTypeLand, GC.getUnitInfo(KingReinforcementTypeLand).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
					}

					for (int i=0;i<reinforcementAmountArtil;i++)
					{
						ReinforcementUnit = kPlayer.initUnit(KingReinforcementTypeArtil, GC.getUnitInfo(KingReinforcementTypeArtil).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
					}

					for (int i=0;i<reinforcementAmountSea;i++)
					{
						ReinforcementUnit = kPlayer.initUnit(KingReinforcementTypeSea, GC.getUnitInfo(KingReinforcementTypeSea).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
					}

					//declaring limited war
					GET_TEAM(kPlayer.getTeam()).declareWar(GET_PLAYER(enemyID).getTeam(),false, WARPLAN_LIMITED);

					//sending message
					CvWString szBuffer = gDLL->getText("TXT_KEY_EUROPE_WAR_KING_SENT_TROOPS");
					gDLL->getInterfaceIFace()->addMessage(ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, ReinforcementUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), ReinforcementUnit->getX(), ReinforcementUnit->getY(), true, true);
				}

				// WTP, ray, giving reinforcement to other Player as well - START
				CvUnit* ReinforcementOtherPlayerUnit = NULL;
				CvPlayer& otherPlayer = GET_PLAYER(enemyID);
				//get City
				CvCity* pLoopOtherPlayerCity = NULL;
				CvCity* locationOtherPlayerToAppear = NULL;
				int iLoopOther;
				for (pLoopOtherPlayerCity = otherPlayer.firstCity(&iLoopOther); pLoopOtherPlayerCity != NULL; pLoopOtherPlayerCity = otherPlayer.nextCity(&iLoopOther))
				{
					if (pLoopOtherPlayerCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
					{
						locationOtherPlayerToAppear = pLoopOtherPlayerCity;
						break;
					}
				}
				if (locationOtherPlayerToAppear != NULL)
				{
					for (int i=0;i<reinforcementAmountLand;i++)
					{
						ReinforcementOtherPlayerUnit = otherPlayer.initUnit(KingReinforcementTypeLand, GC.getUnitInfo(KingReinforcementTypeLand).getDefaultProfession(), locationOtherPlayerToAppear->getX_INLINE(), locationOtherPlayerToAppear->getY_INLINE(), NO_UNITAI);
					}

					for (int i=0;i<reinforcementAmountArtil;i++)
					{
						ReinforcementOtherPlayerUnit = otherPlayer.initUnit(KingReinforcementTypeArtil, GC.getUnitInfo(KingReinforcementTypeArtil).getDefaultProfession(), locationOtherPlayerToAppear->getX_INLINE(), locationOtherPlayerToAppear->getY_INLINE(), NO_UNITAI);
					}

					for (int i=0;i<reinforcementAmountSea;i++)
					{
						ReinforcementOtherPlayerUnit = otherPlayer.initUnit(KingReinforcementTypeSea, GC.getUnitInfo(KingReinforcementTypeSea).getDefaultProfession(), locationOtherPlayerToAppear->getX_INLINE(), locationOtherPlayerToAppear->getY_INLINE(), NO_UNITAI);
					}

					//sending message
					CvWString szBuffer = gDLL->getText("TXT_KEY_EUROPE_WAR_KING_SENT_TROOPS_OTHER_PLAYER");
					gDLL->getInterfaceIFace()->addMessage(enemyID, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, ReinforcementOtherPlayerUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), ReinforcementOtherPlayerUnit->getX(), ReinforcementOtherPlayerUnit->getY(), true, true);
				}
				// WTP, ray, giving reinforcement to other Player as well - END
			}

			// we have chosen to disobey
			else
			{
				//increasing max tax rate
				int maxtaxincrease = GC.getDefineINT("NEG_TAX_EUROPE_WARS");
				int currentmaxtax = kPlayer.NBMOD_GetMaxTaxRate();

				int absolutMaxTaxrate = GC.getMAX_TAX_RATE();
				while ((kPlayer.NBMOD_GetMaxTaxRate() < currentmaxtax + maxtaxincrease) && (kPlayer.NBMOD_GetMaxTaxRate() < absolutMaxTaxrate))
				{
					kPlayer.NBMOD_IncreaseMaxTaxRate();
				}
				//improve relationship to the potential enemy
				GET_PLAYER(enemyID).AI_changeAttitudeExtra(ePlayer, 2);

				int taxchange = kPlayer.NBMOD_GetMaxTaxRate() - currentmaxtax;

				//sending message
				CvWString szBuffer = gDLL->getText("TXT_KEY_EUROPE_WAR_KING_INCCREASED_MAX_TAX", taxchange, GC.getLeaderHeadInfo(GET_PLAYER(enemyID).getLeaderType()).getDescription());
				gDLL->getInterfaceIFace()->addMessage(ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), NULL, NULL, false, false);
			}
		}
		break;
	//TAC European Wars

	// R&R, Stealing Immigrant - START
	case DIPLOEVENT_STEALING_IMMIGRANT:
		{
			// get price
			int priceStealingImmigrant = GC.getDefineINT("BASE_STEALING_IMMIGRANT_PRICE");
			int gamespeedMod = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
			priceStealingImmigrant = priceStealingImmigrant * gamespeedMod / 100;

			//getting the Data from Diplo-Event
			PlayerTypes victimID = (PlayerTypes) iData1; 
			
			CvPlayer& victimPlayer = GET_PLAYER(victimID); // European we steal Immigrant
			CvPlayer& kPlayer = GET_PLAYER(ePlayer); // human Player

			if (kPlayer.getGold() >= priceStealingImmigrant)
			{
				// get random Unit from other European, we steal from
				int randomUnitSelectOnDock = GC.getGameINLINE().getSorenRandNum(CivEffect()->getNumUnitsOnDock(), "pick immigrant");
				UnitTypes eBestUnit = victimPlayer.getDocksNextUnit(randomUnitSelectOnDock);
				if (NO_UNIT != eBestUnit)
				{
					// not necessary, player won't notice anyways
					//victimPlayer.m_aDocksNextUnits[randomUnitSelectOnDock] = victimPlayer.pickBestImmigrant(); // change unit waiting on dock for victim

					CvUnit* pUnit = kPlayer.initEuropeUnit(eBestUnit); // init unit for human player

					// add message for human player
					CvWString szBuffer;
					szBuffer = gDLL->getText("TXT_KEY_STEALING_IMMIGRANT_SUCCESSFULL", GC.getCivilizationInfo(GET_PLAYER(victimPlayer.getParent()).getCivilizationType()).getShortDescriptionKey(), GC.getCivilizationInfo(getCivilizationType()).getShortDescriptionKey(), GC.getUnitInfo(eBestUnit).getTextKeyWide());
					gDLL->getInterfaceIFace()->addMessage(ePlayer, true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNIT_GREATPEOPLE", MESSAGE_TYPE_INFO, GC.getUnitInfo(eBestUnit).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"));

					gDLL->getInterfaceIFace()->setDirty(EuropeScreen_DIRTY_BIT, true);

					FAssert(pUnit != NULL);
					if(pUnit != NULL)
					{
						gDLL->getEventReporterIFace()->emmigrantAtDocks(ePlayer, pUnit->getID());
					}

					changeGold(priceStealingImmigrant); // pay parent
					kPlayer.changeGold(-priceStealingImmigrant); // substract gold from human player

					// chance that victim notices and gets angry
					int randForVictimNoticing = GC.getGameINLINE().getSorenRandNum(3, "Stealing Immigrant noticed");
					if (randForVictimNoticing >= 1)
					{
						victimPlayer.AI_changeAttitudeExtra(ePlayer, -1);
						CvWString szBuffer;
						szBuffer = gDLL->getText("TXT_KEY_STEALING_IMMIGRANT_NOTICED", GC.getCivilizationInfo(GET_PLAYER(victimPlayer.getParent()).getCivilizationType()).getShortDescriptionKey());
						gDLL->getInterfaceIFace()->addMessage(ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), NULL, NULL, false, false);
					}
				}
			}
		}
		break;
	// R&R, Stealing Immigrant - END

	// R&R  European PEACE
	case DIPLOEVENT_EUROPE_PEACE:
		{
			//getting the Data from Diplo-Event
			PlayerTypes enemyID = (PlayerTypes) iData1;
			CvPlayer& kPlayer = GET_PLAYER(ePlayer);

			PlayerTypes parentID = kPlayer.getParent();
			int choice = iData2;

			// we have chosen to obey
			if(choice == 1)
			{
				//make peace
				GET_TEAM(kPlayer.getTeam()).makePeace(GET_PLAYER(enemyID).getTeam(),true);

				UnitTypes Diplomat1UnitTypes =(UnitTypes)GC.getCivilizationInfo(kPlayer.getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_DIPLOMAT_1"));
				UnitTypes Diplomat2UnitTypes =(UnitTypes)GC.getCivilizationInfo(kPlayer.getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_DIPLOMAT_2"));

				//get City
				int iLoop;
				CvCity* locationToAppear = kPlayer.firstCity(&iLoop);

				//just for safety
				if (locationToAppear != NULL) {
					//creating the units
					CvUnit* Diplomat1Unit = kPlayer.initUnit(Diplomat1UnitTypes, GC.getUnitInfo(Diplomat1UnitTypes).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
					CvUnit* Diplomat2Unit = kPlayer.initUnit(Diplomat2UnitTypes, GC.getUnitInfo(Diplomat1UnitTypes).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
					CvWString szBuffer = gDLL->getText("TXT_KEY_EUROPE_PEACE_ACCEPTED", GC.getLeaderHeadInfo(GET_PLAYER(enemyID).getLeaderType()).getDescription());
					gDLL->getInterfaceIFace()->addMessage(ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), NULL, NULL, false, false);
				}
			}
			else {
				//nothing really happens only parent little disappointed
				AI_changeAttitudeExtra(ePlayer, -1);
				CvWString szBuffer = gDLL->getText("TXT_KEY_EUROPE_PEACE_DENIED", GC.getLeaderHeadInfo(GET_PLAYER(enemyID).getLeaderType()).getDescription());
				gDLL->getInterfaceIFace()->addMessage(ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), NULL, NULL, false, false);
			}
		}
		break;
	// R&R  END European PEACE

	// R&R, ray, Natives Trading - START
	case DIPLOEVENT_NATIVE_TRADE:
		{
			CvUnit* pUnit = getUnit(iData1);
			int choice = iData2;
			CvPlayer& kPlayer = GET_PLAYER(ePlayer);
		
			// bugfix start - Nightinggale
			// don't assume that unit and cities exist just because they existed when the window was added to the pop-up queue
			// failed assumtions leads to crashes
			if (pUnit != NULL)
			{
			// bugfix end
				CvCity* pHomeCity = pUnit->getHomeCity(); // the Native City
				CvCity* pOtherCity = pUnit->plot()->getPlotCity(); // European City

				// bugfix start - Nightinggale
				if (pOtherCity != NULL)
				{
				// bugfix end

					YieldTypes yieldToTrade = pUnit->getYieldForNativeTrade();
					int iAmountToTrade = pUnit->getAmountForNativeTrade();

					int priceToPay = GC.getYieldInfo(yieldToTrade).getNativeSellPrice() * iAmountToTrade;
					// we have chosen to accept
					if (choice == 1)
					{
						if (kPlayer.getGold() >= priceToPay)
						{
							// European pays gold amd receives goods
							kPlayer.changeGold(-priceToPay);
							pOtherCity->changeYieldStored(yieldToTrade, iAmountToTrade);

							// Native receives gold and looses goods
							changeGold(priceToPay);
							// bugfix start - Nightinggale
							if (pHomeCity != NULL)
							{
							// bugfix end
								// bugfix: ensure that the natives will not remove more yields than they have stored
								pHomeCity->changeYieldStored(yieldToTrade, -std::min(iAmountToTrade, pHomeCity->getYieldStored(yieldToTrade)));
							} // bugfix

							//add message
							CvWString szMessage = gDLL->getText("TXT_KEY_NATIVE_TRADE_FINISHED", iAmountToTrade, pOtherCity->getNameKey(), GC.getYieldInfo(yieldToTrade).getChar());
							gDLL->getInterfaceIFace()->addMessage(ePlayer, true, GC.getEVENT_MESSAGE_TIME(), szMessage, "AS2D_BUILD_BANK", MESSAGE_TYPE_MAJOR_EVENT, GC.getYieldInfo(yieldToTrade).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pOtherCity->getX_INLINE(), pOtherCity->getY_INLINE(), true, true);
						}
					}

					// we have chosen to decline
					if (choice == 2)
					{
						int angerRand =  GC.getGameINLINE().getSorenRandNum(5, "Decline Anger");
						if (angerRand <= 1)
						{
							AI_changeAttitudeExtra(ePlayer, -1);
						}
					}

			// bugfix start - Nightinggale
				}

				// R&R, vetiarvind, bug fix - START
				// clear Native Trade cache
				pUnit->setYieldForNativeTrade(NO_YIELD);
				pUnit->setAmountForNativeTrade(0);
				// R&R, vetiarvind, bug fix - END
			}
			// bugfix end
			// R&R, vetiarvind, bug fix - START
			//pUnit->setYieldForNativeTrade(NO_YIELD);
			//pUnit->setAmountForNativeTrade(0);
			// R&R, vetiarvind, bug fix - END
			

		}
		break;
	// R&R, ray, Natives Trading - END

	case DIPLOEVENT_FOUND_CITY_CHECK_NATIVES:
		{
			CvUnit* pUnit = GET_PLAYER(ePlayer).getUnit(iData1);
			if (pUnit != NULL)
			{
				pUnit->doFoundCheckNatives();
			}
		}
		break;

	case DIPLOEVENT_TARGET_CITY:
		pCity = GET_PLAYER((PlayerTypes)iData1).getCity(iData2);
		if (pCity != NULL)
		{
			pCity->area()->setTargetCity(getID(), pCity);
		}
		break;

	default:
		FAssert(false);
		break;
	}
}


bool CvPlayer::canTradeWith(PlayerTypes eWhoTo) const
{
	if (getParent() != eWhoTo)
	{
		CvPlayer& kWhoTo = GET_PLAYER(eWhoTo);

		if (atWar(getTeam(), kWhoTo.getTeam()))
		{
			return true;
		}

		if (GET_TEAM(getTeam()).isGoldTrading() || GET_TEAM(kWhoTo.getTeam()).isGoldTrading())
		{
			return true;
		}

		if (GET_TEAM(getTeam()).isMapTrading() || GET_TEAM(kWhoTo.getTeam()).isMapTrading())
		{
			return true;
		}

		if (GET_TEAM(getTeam()).isOpenBordersTrading() || GET_TEAM(kWhoTo.getTeam()).isOpenBordersTrading())
		{
			return true;
		}

		if (GET_TEAM(getTeam()).isDefensivePactTrading() || GET_TEAM(kWhoTo.getTeam()).isDefensivePactTrading())
		{
			return true;
		}

		if (GET_TEAM(getTeam()).isPermanentAllianceTrading() || GET_TEAM(kWhoTo.getTeam()).isPermanentAllianceTrading())
		{
			return true;
		}
	}

	return false;
}

bool CvPlayer::canReceiveTradeCity(PlayerTypes eFromPlayer) const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
	{
		return false;
	}

	if (isNative() && !GET_PLAYER(eFromPlayer).isNative())
	{
		return false;
	}

	return true;
}

bool CvPlayer::canTradeItem(PlayerTypes eWhoTo, TradeData item, bool bTestDenial) const
{
	CvPlayer& kWhoTo = GET_PLAYER(eWhoTo);

	if (isEurope() || kWhoTo.isEurope())
	{
		return false;
	}

	if (bTestDenial)
	{
		if (getTradeDenial(eWhoTo, item) != NO_DENIAL)
		{
			return false;
		}
	}

	switch (item.m_eItemType)
	{
	case TRADE_CITIES:
		{
			CvCity* pCityTraded = getCity(item.m_iData1);
			if (NULL != pCityTraded)
			{
				if (pCityTraded->canTradeAway(eWhoTo))
				{
					return true;
				}
			}
		}
		break;

	case TRADE_GOLD:
		if (GET_TEAM(getTeam()).isGoldTrading() || GET_TEAM(kWhoTo.getTeam()).isGoldTrading())
		{
			int iGold = getMaxGoldTrade(eWhoTo, item.m_kTransport);
			if (iGold > 0 && iGold >= item.m_iData1)
			{
				return true;
			}
		}
		break;

	case TRADE_YIELD:
		{
			YieldTypes eYield = (YieldTypes) item.m_iData1;
			CvUnit* pTransport = ::getUnit(item.m_kTransport);
			return (getTradeYieldAmount(eYield, pTransport) > 0);
		}
		break;

	case TRADE_MAPS:
		if (!isNative() && !kWhoTo.isNative())
		{
			if (getTeam() != kWhoTo.getTeam())
			{
				if (GET_TEAM(getTeam()).isMapTrading() || GET_TEAM(kWhoTo.getTeam()).isMapTrading())
				{
					return true;
				}
			}
		}
		break;

	case TRADE_PEACE:
		if (!(GET_TEAM(getTeam()).isHuman()))
		{
			if (GET_TEAM(getTeam()).isHasMet((TeamTypes)(item.m_iData1)) && GET_TEAM(kWhoTo.getTeam()).isHasMet((TeamTypes)(item.m_iData1)))
			{
				if (atWar(getTeam(), ((TeamTypes)(item.m_iData1))))
				{
					return true;
				}
			}
		}
		break;

	case TRADE_WAR:
		if (!(GET_TEAM(getTeam()).isHuman()))
		{
			if (GET_TEAM(getTeam()).isHasMet((TeamTypes)(item.m_iData1)) && GET_TEAM(kWhoTo.getTeam()).isHasMet((TeamTypes)(item.m_iData1)))
			{
				if (GET_TEAM(getTeam()).canDeclareWar((TeamTypes)(item.m_iData1)))
				{
					return true;
				}
			}
		}
		break;

	case TRADE_EMBARGO:
		if (!(GET_TEAM(getTeam()).isHuman()))
		{
			if (GET_TEAM(getTeam()).isHasMet((TeamTypes)(item.m_iData1)) && GET_TEAM(kWhoTo.getTeam()).isHasMet((TeamTypes)(item.m_iData1)))
			{
				if (canStopTradingWithTeam((TeamTypes)(item.m_iData1)))
				{
					return true;
				}
			}
		}
		break;

	case TRADE_OPEN_BORDERS:
		if (getTeam() != kWhoTo.getTeam())
		{
			if (!atWar(getTeam(), kWhoTo.getTeam()))
			{
				if (!(GET_TEAM(getTeam()).isOpenBorders(kWhoTo.getTeam())) && !isAlwaysOpenBorders() && !kWhoTo.isAlwaysOpenBorders())
				{
					if (GET_TEAM(getTeam()).isOpenBordersTrading() || GET_TEAM(kWhoTo.getTeam()).isOpenBordersTrading())
					{
						return true;
					}
				}
			}
		}
		break;

	case TRADE_DEFENSIVE_PACT:
		if (getParent() == NO_PLAYER || kWhoTo.getParent() == NO_PLAYER)
		{
			if (getTeam() != kWhoTo.getTeam())
			{
				if (!atWar(getTeam(), kWhoTo.getTeam()))
				{
					if (!(GET_TEAM(getTeam()).isDefensivePact(kWhoTo.getTeam())))
					{
						if (GET_TEAM(getTeam()).isDefensivePactTrading() || GET_TEAM(kWhoTo.getTeam()).isDefensivePactTrading())
						{
							if ((GET_TEAM(getTeam()).getAtWarCount() == 0) && (GET_TEAM(kWhoTo.getTeam()).getAtWarCount() == 0))
							{
								if (GET_TEAM(getTeam()).canSignDefensivePact(kWhoTo.getTeam()))
								{
									return true;
								}
							}
						}
					}
				}
			}
		}
		break;

	case TRADE_PERMANENT_ALLIANCE:
		if (getTeam() != kWhoTo.getTeam())
		{
			if (!atWar(getTeam(), kWhoTo.getTeam()))
			{
				if (GET_TEAM(getTeam()).isPermanentAllianceTrading() || GET_TEAM(kWhoTo.getTeam()).isPermanentAllianceTrading())
				{
					if ((GET_TEAM(getTeam()).getNumMembers() == 1) && (GET_TEAM(kWhoTo.getTeam()).getNumMembers() == 1))
					{
						return true;
					}
				}
			}
		}
		break;

	case TRADE_PEACE_TREATY:
		return true;
		break;
	}

	return false;
}


DenialTypes CvPlayer::getTradeDenial(PlayerTypes eWhoTo, TradeData item) const
{
	CvCity* pCity;
	CvPlayer& kWhoTo = GET_PLAYER(eWhoTo);

	switch (item.m_eItemType)
	{
	case TRADE_CITIES:
		pCity = getCity(item.m_iData1);
		if (pCity != NULL)
		{
			return AI_cityTrade(pCity, eWhoTo);
		}
		break;

	case TRADE_GOLD:
		break;

	case TRADE_YIELD:
		return AI_yieldTrade((YieldTypes) item.m_iData1, item.m_kTransport, eWhoTo);
		break;

	case TRADE_MAPS:
		return GET_TEAM(getTeam()).AI_mapTrade(kWhoTo.getTeam());
		break;

	case TRADE_PEACE:
		return GET_TEAM(getTeam()).AI_makePeaceTrade(((TeamTypes)(item.m_iData1)), kWhoTo.getTeam());
		break;

	case TRADE_WAR:
		return GET_TEAM(getTeam()).AI_declareWarTrade(((TeamTypes)(item.m_iData1)), kWhoTo.getTeam());
		break;

	case TRADE_EMBARGO:
		return AI_stopTradingTrade(((TeamTypes)(item.m_iData1)), eWhoTo);
		break;

	case TRADE_OPEN_BORDERS:
		return GET_TEAM(getTeam()).AI_openBordersTrade(kWhoTo.getTeam());
		break;

	case TRADE_DEFENSIVE_PACT:
		return GET_TEAM(getTeam()).AI_defensivePactTrade(kWhoTo.getTeam());
		break;

	case TRADE_PERMANENT_ALLIANCE:
		return GET_TEAM(getTeam()).AI_permanentAllianceTrade(kWhoTo.getTeam());
		break;

	case TRADE_PEACE_TREATY:
		break;
	}

	return NO_DENIAL;
}

bool CvPlayer::isTradingWithTeam(TeamTypes eTeam, bool bIncludeCancelable) const
{
	int iLoop;

	if (eTeam == getTeam())
	{
		return false;
	}

	for (CvDeal* pLoopDeal = GC.getGameINLINE().firstDeal(&iLoop); pLoopDeal != NULL; pLoopDeal = GC.getGameINLINE().nextDeal(&iLoop))
	{
		if (bIncludeCancelable || pLoopDeal->isCancelable(getID()))
		{
			if (!pLoopDeal->isPeaceDeal())
			{
				if ((pLoopDeal->getFirstPlayer() == getID()) && (GET_PLAYER(pLoopDeal->getSecondPlayer()).getTeam() == eTeam))
				{
					if (pLoopDeal->getLengthFirstTrades() > 0)
					{
						return true;
					}
				}

				if ((pLoopDeal->getSecondPlayer() == getID()) && (GET_PLAYER(pLoopDeal->getFirstPlayer()).getTeam() == eTeam))
				{
					if (pLoopDeal->getLengthSecondTrades() > 0)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}


bool CvPlayer::canStopTradingWithTeam(TeamTypes eTeam, bool bContinueNotTrading) const
{
	if (eTeam == getTeam())
	{
		return false;
	}

	if (!isTradingWithTeam(eTeam, false))
	{
		if (bContinueNotTrading && !isTradingWithTeam(eTeam, true))
		{
			return true;
		}

		return false;
	}

	return true;
}


void CvPlayer::stopTradingWithTeam(TeamTypes eTeam)
{
	CvDeal* pLoopDeal;
	int iLoop;
	int iI;

	FAssert(eTeam != getTeam());

	for(pLoopDeal = GC.getGameINLINE().firstDeal(&iLoop); pLoopDeal != NULL; pLoopDeal = GC.getGameINLINE().nextDeal(&iLoop))
	{
		if (pLoopDeal->isCancelable(getID()) && !(pLoopDeal->isPeaceDeal()))
		{
			if (((pLoopDeal->getFirstPlayer() == getID()) && (GET_PLAYER(pLoopDeal->getSecondPlayer()).getTeam() == eTeam)) ||
				  ((pLoopDeal->getSecondPlayer() == getID()) && (GET_PLAYER(pLoopDeal->getFirstPlayer()).getTeam() == eTeam)))
			{
				pLoopDeal->kill(true, getTeam());
			}
		}
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam)
			{
				GET_PLAYER((PlayerTypes)iI).AI_changeMemoryCount(getID(), MEMORY_STOPPED_TRADING, 1);
				GET_PLAYER((PlayerTypes)iI).AI_changeMemoryCount(getID(), MEMORY_STOPPED_TRADING_RECENT, 1);
			}
		}
	}
}


void CvPlayer::killAllDeals()
{
	CvDeal* pLoopDeal;
	int iLoop;

	for(pLoopDeal = GC.getGameINLINE().firstDeal(&iLoop); pLoopDeal != NULL; pLoopDeal = GC.getGameINLINE().nextDeal(&iLoop))
	{
		if ((pLoopDeal->getFirstPlayer() == getID()) || (pLoopDeal->getSecondPlayer() == getID()))
		{
			pLoopDeal->kill(true, getTeam());
		}
	}
}


void CvPlayer::findNewCapital()
{
	CvCity* pOldCapital;
	CvCity* pLoopCity;
	CvCity* pBestCity;
	BuildingTypes eCapitalBuilding;
	int iValue;
	int iBestValue;
	int iLoop;

	BuildingClassTypes eCapitalBuildingClass = (BuildingClassTypes) GC.getDefineINT("CAPITAL_BUILDINGCLASS");
	if(eCapitalBuildingClass == NO_BUILDINGCLASS)
	{
		return;
	}

	eCapitalBuilding = ((BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(eCapitalBuildingClass)));
	if (eCapitalBuilding == NO_BUILDING)
	{
		return;
	}

	pOldCapital = getCapitalCity();

	iBestValue = 0;
	pBestCity = NULL;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity != pOldCapital)
		{
			if (!pLoopCity->isHasRealBuilding(eCapitalBuilding))
			{
				iValue = (pLoopCity->getPopulation() * 4);

				iValue += pLoopCity->getYieldRate(YIELD_FOOD);
				iValue += pLoopCity->getCultureLevel();

				iValue *= (pLoopCity->calculateCulturePercent(getID()) + 100);
				iValue /= 100;

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestCity = pLoopCity;
				}
			}
		}
	}

	if (pBestCity != NULL)
	{
		if (pOldCapital != NULL)
		{
			pOldCapital->setHasRealBuilding(eCapitalBuilding, false);
		}
		FAssert(!(pBestCity->isHasRealBuilding(eCapitalBuilding)));
		pBestCity->setHasRealBuilding(eCapitalBuilding, true);
	}
}

bool CvPlayer::canRaze(CvCity* pCity) const
{
	if (!pCity->isAutoRaze())
	{
		if (GC.getGameINLINE().isOption(GAMEOPTION_NO_CITY_RAZING))
		{
			return false;
		}

		if (pCity->getOwnerINLINE() != getID())
		{
			return false;
		}

		if (pCity->calculateTeamCulturePercent(getTeam()) >= GC.getDefineINT("RAZING_CULTURAL_PERCENT_THRESHOLD"))
		{
			return false;
		}
	}

	CyCity* pyCity = new CyCity(pCity);
	CyArgsList argsList;
	argsList.add(getID());	// Player ID
	argsList.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
	long lResult=0;
	gDLL->getPythonIFace()->callFunction(PYGameModule, "canRazeCity", argsList.makeFunctionArgs(), &lResult);
	delete pyCity;	// python fxn must not hold on to this pointer
	if (lResult == 0)
	{
		return (false);
	}

	return true;
}


void CvPlayer::raze(CvCity* pCity)
{
	if (!canRaze(pCity))
	{
		return;
	}

	FAssert(pCity->getOwnerINLINE() == getID());

	PlayerTypes eHighestCulturePlayer = pCity->findHighestCulture();

	if (eHighestCulturePlayer != NO_PLAYER)
	{
		if (GET_PLAYER(eHighestCulturePlayer).getTeam() != getTeam())
		{
			GET_PLAYER(eHighestCulturePlayer).AI_changeMemoryCount(getID(), MEMORY_RAZED_CITY, 1);
			AI_changeMemoryCount(eHighestCulturePlayer, MEMORY_REVENGE_TAKEN, 1);
			if (isNative())
			{
				GET_TEAM(getTeam()).AI_changeDamages(pCity->getTeam(), -AI_cityTradeVal(pCity, eHighestCulturePlayer));
			}
		}
	}

	CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_DESTROYED_CITY", pCity->getNameKey());
	gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CITYRAZE", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pCity->getX_INLINE(), pCity->getY_INLINE(), true, true);

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (iI != getID())
			{
				if (pCity->isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
				{
					szBuffer = gDLL->getText("TXT_KEY_MISC_CITY_HAS_BEEN_RAZED_BY", pCity->getNameKey(), getCivilizationDescriptionKey());
					gDLL->getInterfaceIFace()->addMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CITYRAZED", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pCity->getX_INLINE(), pCity->getY_INLINE(), true, true);
				}
			}
		}
	}

	szBuffer = gDLL->getText("TXT_KEY_MISC_CITY_RAZED_BY", pCity->getNameKey(), getCivilizationDescriptionKey());
	GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szBuffer, pCity->getX_INLINE(), pCity->getY_INLINE(), (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));

	// Report this event
	gDLL->getEventReporterIFace()->cityRazed(pCity, getID());

	if (pCity->getPreviousOwner() != NO_PLAYER)
	{
		if (pCity->getHighestPopulation() > 1)
		{
			UnitClassTypes eUnitClass = (UnitClassTypes) GC.getCivilizationInfo(GET_PLAYER(pCity->getPreviousOwner()).getCivilizationType()).getCapturedCityUnitClass();

			if (eUnitClass != NO_UNITCLASS)
			{
				UnitTypes eUnit = (UnitTypes) GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(eUnitClass);

				if (eUnit != NO_UNIT)
				{
					for (int i = 0; i < pCity->getPopulation(); ++i)
					{
						// R&R, ray, Native Slaves, START
						if (GET_PLAYER(pCity->getPreviousOwner()).isNative())
						{
							UnitTypes SlaveType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_NATIVE_SLAVE"));
							initUnit(SlaveType, GC.getUnitInfo(SlaveType).getDefaultProfession(), pCity->getX_INLINE(), pCity->getY_INLINE());

						}
						else
						{
							initUnit(eUnit, GC.getUnitInfo(eUnit).getDefaultProfession(), pCity->getX_INLINE(), pCity->getY_INLINE());
						}
						// R&R, ray, Native Slaves, END
					}
				}
			}
		}

		int iTreasure = GC.getDefineINT("SETTLEMENT_TREASURE_YIELD") * pCity->getHighestPopulation();
		iTreasure = iTreasure * GC.getCivilizationInfo(GET_PLAYER(pCity->getPreviousOwner()).getCivilizationType()).getTreasure() / 100;
		iTreasure = iTreasure * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent() / 100;
		iTreasure = iTreasure / 2 + GC.getGameINLINE().getSorenRandNum(iTreasure / 2, "Treasure Gold 1") + GC.getGameINLINE().getSorenRandNum(iTreasure / 2, "Treasure Gold 2");

		int iTreasureMod = 100;
		for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); ++iTrait)
		{
			CvTraitInfo& kTrait = GC.getTraitInfo((TraitTypes) iTrait);
			// R&R, ray, for Trait Conquistador
			if (GET_PLAYER(pCity->getPreviousOwner()).isNative() && GET_PLAYER(pCity->getPreviousOwner()).hasTrait((TraitTypes) iTrait))
			{
				iTreasureMod += kTrait.getTreasureModifier();
			}
			// R&R, ray, for Trait Conquistador
			if (GET_PLAYER(pCity->getPreviousOwner()).isNative() && !isNative() && hasTrait((TraitTypes) iTrait))
			{
				iTreasureMod += kTrait.getTreasureModifier();
			}
		}
		iTreasure *= iTreasureMod;
		iTreasure /= 100;

		if (iTreasure != 0)
		{
			UnitClassTypes eUnitClass = (UnitClassTypes) GC.getDefineINT("TREASURE_UNITCLASS");

			if (eUnitClass != NO_UNITCLASS)
			{
				UnitTypes eUnit = (UnitTypes) GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(eUnitClass);

				if (eUnit != NO_UNIT)
				{
					FAssert(GC.getUnitInfo(eUnit).isTreasure());
					CvUnit* pTreasure = initUnit(eUnit, GC.getUnitInfo(eUnit).getDefaultProfession(), pCity->getX_INLINE(), pCity->getY_INLINE(), NO_UNITAI, NO_DIRECTION, iTreasure);
				}
			}
		}
	}

	if (pCity->findHighestCulture() != getID())
	{
		for (int i = 0; i < GC.getNumFatherPointInfos(); ++i)
		{
			FatherPointTypes ePointType = (FatherPointTypes) i;
			int gameSpeedMod =  GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
			int razeCityBonus = GC.getFatherPointInfo(ePointType).getRazeCityPoints() - GC.getFatherPointInfo(ePointType).getConquerCityPoints();
			changeFatherPoints(ePointType, razeCityBonus*gameSpeedMod/100 );
		}
	}

	disband(pCity);
}

void CvPlayer::disband(CvCity* pCity, bool bAbandon)
{
	if (getNumCities() == 1)
	{
		setFoundedFirstCity(false);
	}


	pCity->kill();


	// Early exit in case the city was razed
	if (!bAbandon)
	{
		GC.getGameINLINE().addDestroyedCityName(pCity->getNameKey());
		return;
	}

	CvPlot* const pCityPlot = pCity->plot();
	
	// No ruins for a city that was voluntarily abandoned
	pCityPlot->setImprovementType(NO_IMPROVEMENT);

	// Loop through all the plots of the city and subtract the free culture that the city itself added.
	// Culture added by buildings will be kept

	const PlayerTypes eCityOwner = pCity->getOwnerINLINE();

	// TODO: What about civs that get free culture producing buildings?
	if (pCityPlot->getCulture(eCityOwner) >= GC.getDefineINT("FREE_CITY_CULTURE"))
	{
		// Subtract the free culture from the plot. If the city every produced its own culture, that will remain
		pCityPlot->changeCulture(eCityOwner, -GC.getDefineINT("FREE_CITY_CULTURE"), /*bUpdateCulture*/true);
	}

	// It's sufficient to only consider the immediate plots to the city square
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* const pAdjacentPlot = plotDirection(pCityPlot->getX_INLINE(), pCityPlot->getY_INLINE(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->getCulture(eCityOwner) >= GC.getDefineINT("FREE_CITY_ADJACENT_CULTURE"))
			{
				pAdjacentPlot->changeCulture(eCityOwner, -GC.getDefineINT("FREE_CITY_ADJACENT_CULTURE"), /*bUpdateCulture*/true);
			}
		}
	}


	if (pCityPlot->getOwner() == NO_PLAYER)
	{
		// Remove road if we don't own the plot
		pCityPlot->setRouteType(NO_ROUTE);
	}
}


bool CvPlayer::canReceiveGoody(CvPlot* pPlot, GoodyTypes eGoody, const CvUnit* pUnit) const
{
	CvGoodyInfo& kGoody = GC.getGoodyInfo(eGoody);


	// R&R, ray, Goodies on Water - START
	if(pPlot->isWater() && !GC.getGoodyInfo(eGoody).isWaterGoody()) // no land goody on water
	{
		return false;
	}
	else if (!pPlot->isWater() && GC.getGoodyInfo(eGoody).isWaterGoody()) // no water goody on land
	{
		return false;
	}
	// R&R, ray, Goodies on Water - END

	if (kGoody.isWar())
	{
		if (!pPlot->isOwned())
		{
			return false;
		}

		if (!GET_TEAM(pPlot->getTeam()).canDeclareWar(getTeam()))
		{
			return false;
		}
	}

	if (kGoody.getExperience() > 0)
	{
		if ((pUnit == NULL) || !pUnit->canAcquirePromotionAny())
		{
			return false;
		}
	}

	if (kGoody.getDamagePrereq() > 0)
	{
		if ((pUnit == NULL) || (pUnit->getDamage() < ((pUnit->maxHitPoints() * kGoody.getDamagePrereq()) / 100)))
		{
			return false;
		}
	}
	if (kGoody.isBad())
	{
		if ((pUnit == NULL) || pUnit->isNoBadGoodies())
		{
			return false;
		}
	}

	if (kGoody.getUnitClassType() != NO_UNITCLASS)
	{
		UnitTypes eUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(kGoody.getUnitClassType())));

		if (eUnit == NO_UNIT)
		{
			return false;
		}

		if ((GC.getUnitInfo(eUnit).getCombat() > 0) && !(GC.getUnitInfo(eUnit).isOnlyDefensive()))
		{
			if (GC.getGameINLINE().isGameMultiPlayer())
			{
				return false;
			}
		}

		if (GC.getGameINLINE().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
		{
			if (GC.getUnitInfo(eUnit).isFound())
			{
				return false;
			}
		}

		// R&R, ray, Goodies on Water - START
		if (pPlot->isWater())
		{
			if (pUnit == NULL) 
			{
				return false;
			}
			else if (GC.getUnitInfo(eUnit).getDomainType() == DOMAIN_LAND && ((pUnit->cargoSpace() - pUnit->getCargo()) <= 0))
			{
				return false;
			}
			else if (pUnit->getUnitInfo().isHiddenNationality()) // R&R, ray, fix for Pirates, Privateers and Smuggles spawning units on water
			{
				return false;
			}
			else if (pUnit->getUnitInfo().isGatherBoat()) // R&R, ray, fix for Whaling Ships and Fishing Boats spawning units
			{
				return false;
			}
		}
	}

	if (kGoody.getTeachUnitClassType() != NO_UNITCLASS)
	{
		if (pUnit == NULL)
		{
			return false;
		}

		UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(kGoody.getTeachUnitClassType());

		if (eUnit == NO_UNIT)
		{
			return false;
		}

		if (pUnit->getUnitInfo().getLearnTime() < 0)
		{
			return false;
		}

		if (pUnit->getProfession() != GC.getUnitInfo(eUnit).getDefaultProfession())
		{
			return false;
		}
	}

	for (int i = 0; i < GC.getNumGoodyInfos(); ++i)
	{
		if (kGoody.getGoodyWeight(i) > 0)
		{
			for (int j = 0; j < GC.getNumGoodyInfos(); ++j)
			{
				if (GC.getGoodyInfo((GoodyTypes) i).getGoodyWeight(j) > 0)
				{
					return false;
				}
			}

			if (!canReceiveGoody(pPlot, (GoodyTypes) i, pUnit))
			{
				return false;
			}
		}
	}

	// R&R, ray, Goody Enhancement - START
	if (kGoody.isUnique())
	{
		if (!GC.getGameINLINE().isUniqueGoodyValid(eGoody))
		{
			return false;
		}
	}
	// R&R, ray, Goody Enhancement - END

	// WTP, ray, Unit spawning Goodies and Goody Huts - START
	// we have to catch invalid XML configurations first
	if (kGoody.isSpawnHostileUnitsAsXML() || kGoody.isSpawnHostileAnimals() || kGoody.isSpawnHostileNatives() || kGoody.isSpawnHostileCriminals())
	{
		// we also do not trigger that stuff for AI - it is too hard / unfair for it
		if(kGoody.getRandNumHostilesSpawned() == 0 || kGoody.getUnitClassType() == NO_UNITCLASS  || !isHuman())
		{
			return false;
		}

		// for Animals we also need to check the Terrain configured correctly for immersion:
		UnitTypes eUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(kGoody.getUnitClassType())));
		if (eUnit != NO_UNIT)
		{
			CvUnitInfo& eAnmialInfo = GC.getUnitInfo(eUnit);
			bool isAnimal = eAnmialInfo.isAnimal();
			bool isNativeTerrain = eAnmialInfo.getTerrainNative(pPlot->getTerrainType());
			if(isAnimal && !isNativeTerrain)
			{
				return false;
			}
		}

	}
	// then we also check min turn - only if not 0
	if (kGoody.getMinTurnValid() != 0)
	{
		int iMinTurnValid = kGoody.getMinTurnValid();
		int iGameSpeedModifier = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
		int iCurrentTurn = GC.getGameINLINE().getGameTurn();

		if (iCurrentTurn < (iMinTurnValid * iGameSpeedModifier))
		{
			return false;
		}
	}
	// WTP, ray, Unit spawning Goodies and Goody Huts - END

	return true;
}

void CvPlayer::receiveRandomGoody(CvPlot* pPlot, GoodyTypes eGoody, CvUnit* pUnit)
{
	int iTotalWeight = 0;
	GoodyTypes eBestGoody = NO_GOODY;
	for (int i = 0; i < GC.getNumGoodyInfos(); ++i)
	{
		iTotalWeight += GC.getGoodyInfo(eGoody).getGoodyWeight(i);
	}

	int iPick = GC.getGameINLINE().getSorenRandNum(iTotalWeight, "goody result");

	iTotalWeight = 0;
	for (int i = 0; i < GC.getNumGoodyInfos(); ++i)
	{
		iTotalWeight += GC.getGoodyInfo(eGoody).getGoodyWeight(i);
		if (iTotalWeight > iPick)
		{
			receiveGoody(pPlot, (GoodyTypes) i, pUnit);
			break;
		}
	}
}

int CvPlayer::receiveGoody(CvPlot* pPlot, GoodyTypes eGoody, CvUnit* pUnit)
{
	int iReturnValue = -1;

	FAssertMsg(canReceiveGoody(pPlot, eGoody, pUnit), "Instance is expected to be able to recieve goody");

	CvGoodyInfo& kGoody = GC.getGoodyInfo(eGoody);

	CvWString szBuffer = gDLL->getObjectText(kGoody.getAnnounceTextKey(), 0);

	int iGold = kGoody.getGold() + GC.getGameINLINE().getSorenRandNum(kGoody.getGoldRand1(), "Goody Gold 1") + GC.getGameINLINE().getSorenRandNum(kGoody.getGoldRand2(), "Goody Gold 2");
	iGold = iGold * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent() / 100;
	if ((pUnit != NULL) && pUnit->isNoBadGoodies())
	{
		iGold = iGold * GC.getDefineINT("NO_BAD_GOODIES_GOLD_PERCENT") / 100;
	}

	if (iGold != 0)
	{
		CvCity* pCity = pPlot->getPlotCity();
		if (pCity != NULL)
		{
			for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); ++iTrait)
			{
				TraitTypes eTrait = (TraitTypes) iTrait;
				if (GET_PLAYER(pCity->getOwnerINLINE()).hasTrait(eTrait) || hasTrait(eTrait))
				{
					iGold *= 100 + GC.getTraitInfo(eTrait).getChiefGoldModifier();
					iGold /= 100;
				}
			}
		}

		iReturnValue = iGold;

		//store gold in treasure unit instead
		UnitClassTypes eUnitClass = (UnitClassTypes) kGoody.getUnitClassType();
		bool bTreasure = false;
		if (eUnitClass != NO_UNITCLASS)
		{
			UnitTypes eTreasure = (UnitTypes) GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(eUnitClass);
			if((eTreasure != NO_UNIT) && GC.getUnitInfo(eTreasure).isTreasure())
			{
				bTreasure = true;
			}
		}

		if (!bTreasure)
		{
			changeGold(iGold);
			szBuffer += gDLL->getText("TXT_KEY_MISC_RECEIVED_GOLD", iGold);
		}
	}

	if (!szBuffer.empty())
	{
		gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, kGoody.getSound(), MESSAGE_TYPE_MINOR_EVENT, ARTFILEMGR.getImprovementArtInfo("ART_DEF_IMPROVEMENT_GOODY_HUT")->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pPlot->getX_INLINE(), pPlot->getY_INLINE());
	}

	// R&R, ray, Goody Enhancement
	int iGoodyImmigrants = kGoody.getImmigration();
	if (iGoodyImmigrants > 0)
	{
		for (int gi = 0; gi < iGoodyImmigrants; gi++)
		{
			int randomUnitSelectOnDock = GC.getGameINLINE().getSorenRandNum(CivEffect()->getNumUnitsOnDock(), "pick immigrant");
			UnitTypes eBestUnit = getDocksNextUnit(randomUnitSelectOnDock);
			if (NO_UNIT != eBestUnit)
			{
				m_aDocksNextUnits[randomUnitSelectOnDock] = pickBestImmigrant();
				CvUnit* pUnit = initEuropeUnit(eBestUnit);

				CvWString szBuffer;
				szBuffer = gDLL->getText("TXT_KEY_NEW_IMMIGRANT_AVAILABLE_SHORT", GC.getCivilizationInfo(GET_PLAYER(getParent()).getCivilizationType()).getShortDescriptionKey(),  GC.getUnitInfo(eBestUnit).getTextKeyWide());
				gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNIT_GREATPEOPLE", MESSAGE_TYPE_INFO, GC.getUnitInfo(eBestUnit).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"));

				gDLL->getInterfaceIFace()->setDirty(EuropeScreen_DIRTY_BIT, true);

				FAssert(pUnit != NULL);
				if(pUnit != NULL)
				{
					gDLL->getEventReporterIFace()->emmigrantAtDocks(getID(), pUnit->getID());
				}
			}
		}
	}

	int iRange = kGoody.getMapRange();
	if ((pUnit != NULL) && pUnit->isNoBadGoodies())
	{
		iRange = iRange * GC.getDefineINT("NO_BAD_GOODIES_GOLD_PERCENT") / 100;
	}

	if (iRange > 0)
	{
		int iOffset = kGoody.getMapOffset();

		CvPlot* pBestPlot = NULL;
		if (iOffset > 0)
		{
			int iBestValue = 0;

			for (int iDX = -(iOffset); iDX <= iOffset; iDX++)
			{
				for (int iDY = -(iOffset); iDY <= iOffset; iDY++)
				{
					CvPlot* pLoopPlot = plotXY(pPlot->getX_INLINE(), pPlot->getY_INLINE(), iDX, iDY);

					if (pLoopPlot != NULL)
					{
						if (!(pLoopPlot->isRevealed(getTeam(), false)))
						{
							int iValue = (1 + GC.getGameINLINE().getSorenRandNum(10000, "Goody Map"));

							iValue *= plotDistance(pPlot->getX_INLINE(), pPlot->getY_INLINE(), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE());

							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								pBestPlot = pLoopPlot;
							}
						}
					}
				}
			}
		}

		if (pBestPlot == NULL)
		{
			pBestPlot = pPlot;
		}

		for (int iDX = -(iRange); iDX <= iRange; iDX++)
		{
			for (int iDY = -(iRange); iDY <= iRange; iDY++)
			{
				CvPlot* pLoopPlot = plotXY(pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), iDX, iDY);

				if (pLoopPlot != NULL)
				{
					if (plotDistance(pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE()) <= iRange)
					{
						if (GC.getGameINLINE().getSorenRandNum(100, "Goody Map") < kGoody.getMapProb())
						{
							pLoopPlot->setRevealed(getTeam(), true, false, NO_TEAM);
						}
					}
				}
			}
		}
	}

	if (pUnit != NULL)
	{
		int iExperience = kGoody.getExperience();
		if (pUnit->isNoBadGoodies())
		{
			iExperience = iExperience * GC.getDefineINT("NO_BAD_GOODIES_EXPERIENCE_PERCENT") / 100;
		}
		pUnit->changeExperience(iExperience);
	}

	if (pUnit != NULL)
	{
		pUnit->changeDamage(-(kGoody.getHealing()));
	}

	// WTP, ray, Unit spawning Goodies and Goody Huts - START
	// this shall only trigger if we do not spawn hostiles instead - code below
	if (!kGoody.isSpawnHostileUnitsAsXML() && !kGoody.isSpawnHostileAnimals() && !kGoody.isSpawnHostileNatives() && !kGoody.isSpawnHostileCriminals())
	{
		if (kGoody.getUnitClassType() != NO_UNITCLASS)
		{
			UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(kGoody.getUnitClassType());

			if (eUnit != NO_UNIT)
			{
				CvUnit* pGoodyUnit = initUnit(eUnit, GC.getUnitInfo(eUnit).getDefaultProfession(), pPlot->getX_INLINE(), pPlot->getY_INLINE());

				//treasure
				if((pGoodyUnit != NULL) && (iGold != 0) && pGoodyUnit->getUnitInfo().isTreasure())
				{
					pGoodyUnit->setYieldStored(iGold);
				}
			}
		}
	}
	// WTP, ray, Unit spawning Goodies and Goody Huts - END

	if (kGoody.getTeachUnitClassType() != NO_UNITCLASS)
	{
		UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(kGoody.getTeachUnitClassType());

		if (eUnit != NO_UNIT)
		{
			CvUnit* pLearnUnit = initUnit(eUnit, pUnit->getProfession(), pPlot->getX_INLINE(), pPlot->getY_INLINE(), pUnit->AI_getUnitAIType());
			FAssert(pLearnUnit != NULL);
			pLearnUnit->joinGroup(pUnit->getGroup());
			pLearnUnit->convert(pUnit, true);
		}
	}

	if (kGoody.isWar())
	{
		GET_TEAM(pPlot->getTeam()).declareWar(getTeam(), true, WARPLAN_EXTORTION);
	}

	for (int i = 0; i < GC.getNumGoodyInfos(); ++i)
	{
		if (kGoody.getGoodyWeight(i) > 0)
		{
			if (isHuman())
			{
				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CHOOSE_GOODY, eGoody, GC.getMapINLINE().plotNumINLINE(pPlot->getX_INLINE(), pPlot->getY_INLINE()), pUnit ? pUnit->getID() : -1);
				gDLL->getInterfaceIFace()->addPopup(pInfo, getID(), true);
			}
			else
			{
				if (AI_chooseGoody(eGoody))
				{
					receiveRandomGoody(pPlot, eGoody, pUnit);
				}
			}
			break;
		}
	}

	// R&R, ray, Goody Enhancement - START
	if (pUnit != NULL)
	{
		if (GC.getGameINLINE().isUniqueGoodyValid(eGoody))
		{
			GC.getGameINLINE().setUniqueGoodyValid(eGoody, false);
		}
	}
	// R&R, ray, Goody Enhancement - FALSE

	// WTP, ray, Unit spawning Goodies and Goody Huts - START
	// code the logic here what these Goodies do
	// is is actually all the same - the XML tags are just used for immersive spwaning config
	if (kGoody.isSpawnHostileUnitsAsXML() || kGoody.isSpawnHostileAnimals() || kGoody.isSpawnHostileNatives() || kGoody.isSpawnHostileCriminals())
	{	

		int eHostileUnitClass = kGoody.getUnitClassType();
		PlayerTypes barbarianPlayerTypes = GC.getGameINLINE().getBarbarianPlayer();

		// for safety
		if (eHostileUnitClass != NO_UNITCLASS && barbarianPlayerTypes != NO_PLAYER)
		{
			// we check how many Hostiles we spawn
			int iRandomHostileUnitsToSpawn = kGoody.getRandNumHostilesSpawned();
			int iActualHostileUnitsToSpawn = GC.getGameINLINE().getSorenRandNum(iRandomHostileUnitsToSpawn, "Randomize Hostile Units Spawned");
			if (iActualHostileUnitsToSpawn == 0)
			{
				iActualHostileUnitsToSpawn = 1;
			}

			// we get UnitClass, Unit and Barbarian Player
			UnitTypes eUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(eHostileUnitClass)));
			CvPlayer& barbarianPlayer = GET_PLAYER(barbarianPlayerTypes);

			// check for eUnit to be safe
			if (eUnit != NO_UNIT)
			{
				// now we loop and spwan the Hostiles
				for(int iI=0; iI<iActualHostileUnitsToSpawn; iI++)
				{
					CvUnit* eHostileUnit= barbarianPlayer.initUnit(eUnit, GC.getUnitInfo(eUnit).getDefaultProfession(), pPlot->getX_INLINE(), pPlot->getY_INLINE(), NO_UNITAI);
					//treasures get loaded with Gold if configured in XML
					if((eHostileUnit != NULL) && (iGold != 0) && eHostileUnit->getUnitInfo().isTreasure())
					{
						eHostileUnit->setYieldStored(iGold);
					}
				}
			}
		}
	}
	// WTP, ray, Unit spawning Goodies and Goody Huts - END

	return iReturnValue;
}


void CvPlayer::doGoody(CvPlot* pPlot, CvUnit* pUnit)
{
	// Python override
	if (gDLL->getPythonIFace()->pythonDoGoody(getID(), pPlot, pUnit))
	{
		return;
	}
	else
	{
		FAssertMsg(pPlot->isGoody(), "pPlot->isGoody is expected to be true");

        // < JAnimals Mod Start >
		//if (!isNative())
		if (!isNative() && !GC.getGameINLINE().isBarbarianPlayer(getID()))
		// < JAnimals Mod End >
		{
			//WTP, Unit only Goodies - END 
			bool bCanTriggerUnitGoodies = pPlot->isGoodyForSpawningUnits();
			//WTP, Unit only Goodies - START

			// WTP, ray, Unit spawning Goodies and Goody Huts - START
			// ok, we need to store Infos for "Hostile Goodies, because they will be removed after this
			bool bCanTriggerHostileAnimalGoodies = pPlot->isGoodyForSpawningHostileAnimals();
			bool bCanTriggerHostileNativesGoodies = pPlot->isGoodyForSpawningHostileNatives();
			bool bCanTriggerHostileCriminalsGoodies = pPlot->isGoodyForSpawningHostileCriminals();
			// WTP, ray, Unit spawning Goodies and Goody Huts - END

			pPlot->removeGoody();

			std::vector<int> aGoodyFactors(GC.getNumGoodyInfos(), 1);
			for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); ++iTrait)
			{
				CvTraitInfo& kTraitInfo = GC.getTraitInfo((TraitTypes) iTrait);
				if (hasTrait((TraitTypes) iTrait))
				{
					for (int iGoody = 0; iGoody < GC.getNumGoodyInfos(); ++iGoody)
					{
						aGoodyFactors[iGoody] *= kTraitInfo.getGoodyFactor(iGoody);
					}
				}
			}

			int iBestValue = -1;
			GoodyTypes eBestGoody = NO_GOODY;
			for (int iI = 0; iI < GC.getHandicapInfo(getHandicapType()).getNumGoodies(); iI++)
			{
				GoodyTypes eGoody = (GoodyTypes) GC.getHandicapInfo(getHandicapType()).getGoodies(iI);

				FAssert(eGoody >= 0);
				FAssert(eGoody < GC.getNumGoodyInfos());

				if (canReceiveGoody(pPlot, eGoody, pUnit))
				{
					for (int iJ = 0; iJ < aGoodyFactors[eGoody]; ++iJ)
					{
						int iRandValue = GC.getGameINLINE().getSorenRandNum(1000, "Goodies");

						//WTP, Unit only Goodies - START
						bool bValid = false;
						CvGoodyInfo& uGoody = GC.getGoodyInfo(eGoody);

						// Case: Goody gives a Unit
						if (uGoody.getUnitClassType() != NO_UNITCLASS)
						{
							// special case Treasures
							if (uGoody.getUnitClassType() == (UnitClassTypes) GC.getDefineINT("TREASURE_UNITCLASS"))
							{
								bool isWaterGoody = uGoody.isWaterGoody();

								// in Water it needs to be a Unit Goody
								if (isWaterGoody && bCanTriggerUnitGoodies)
								{
									bValid = true;
								}

								// on land it shall be not a Unit Goody
								else if (!isWaterGoody && !bCanTriggerUnitGoodies)
								{
									bValid = true;
								}
							}

							// WTP, ray, Unit spawning Goodies and Goody Huts - START
							else if (uGoody.isSpawnHostileUnitsAsXML())
							{
								if (bCanTriggerUnitGoodies)
								{
									bValid = true;
								}
							}
							// Wild Animal should only spawned in Animal Goody Huts - allows to configure immersive spawning
							else if (uGoody.isSpawnHostileAnimals())
							{
								if (bCanTriggerHostileAnimalGoodies)
								{
									bValid = true;
								}
							}
							// Hostile Natives should only spawned in Hostile Native Goody Huts - allows to configure immersive spawning
							else if (uGoody.isSpawnHostileNatives())
							{
								if (bCanTriggerHostileNativesGoodies)
								{
									bValid = true;
								}
							}
							// Hostile Criminals should only spawned in Hostile Criminal Goody Huts - allows to configure immersive spawning
							else if (uGoody.isSpawnHostileCriminals())
							{
								if (bCanTriggerHostileCriminalsGoodies)
								{
									bValid = true;
								}
							}
							// WTP, ray, Unit spawning Goodies and Goody Huts - END

							// in all other Cases Unit Goodies may only be triggered if bCanTriggerUnitGoodies 
							// normal case for hostile and non-hostile Units
							else if (bCanTriggerUnitGoodies)
							{
								bValid = true;
							}

						}

						// Case: Goody gives other rewards
						else 
						{
							// Land or Water Goody?
							bool isWaterGoody = uGoody.isWaterGoody();

							// case Land Goody
							// Goodies that are not giving Units only valid for Land Goody Huts not giving Units
							if (!isWaterGoody && !bCanTriggerUnitGoodies)
							{
								bValid = true;	
							}
							// case Water Goody
							// to ensure that Water Unit Goodies do not only trigger Units - unless Cargo is full
							if (isWaterGoody && (
								!bCanTriggerUnitGoodies
								|| pUnit == NULL
								|| (pUnit->getUnitInfo().getSpecialCargo() != NO_SPECIALUNIT && pUnit->getUnitInfo().getSpecialCargo() != SPECIALUNIT_COLONIST_UNIT)
								|| (pUnit->cargoSpace() - pUnit->getCargo() <= 0)
								))
							{
								bValid = true;
							}
						}

						// so now we figured out the Goody is valid or not
						if (bValid)
						{
							if(iRandValue > iBestValue)
							{
								iBestValue = iRandValue;
								eBestGoody = eGoody;
							}
						}
						//WTP, Unit only Goodies - END
					}
				}
			}

			FAssert(eBestGoody != NO_GOODY);
			if(eBestGoody != NO_GOODY)
			{
				receiveGoody(pPlot, eBestGoody, pUnit);

				// Python Event
				gDLL->getEventReporterIFace()->goodyReceived(getID(), pPlot, pUnit, eBestGoody);
			}
		}
	}
}


bool CvPlayer::canFound(int iX, int iY, bool bTestVisible) const
{
	CvPlot* pPlot;
	CvPlot* pLoopPlot;
	bool bValid;
	int iRange;
	int iDX, iDY;

	if (!CivEffect()->canFoundCity())
	{
		return false;
	}

	pPlot = GC.getMapINLINE().plotINLINE(iX, iY);

	if (GC.getGameINLINE().isFinalInitialized())
	{
		if (GC.getGameINLINE().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
		{
			if (getNumCities() > 0)
			{
				return false;
			}
		}
	}

	if (pPlot->isImpassable())
	{
		return false;
	}

	if (pPlot->isPeak())
	{
		return false;
	}

	if (pPlot->getFeatureType() != NO_FEATURE)
	{
		if (GC.getFeatureInfo(pPlot->getFeatureType()).isNoCity())
		{
			return false;
		}
	}

	if (pPlot->isCity())
	{
		return false;
	}

	if (pPlot->isOwned() && (pPlot->getOwnerINLINE() != getID()))
	{
		if (!GET_PLAYER(pPlot->getOwnerINLINE()).isNative())
		{
			return false;
		}
	}

	bValid = false;

	if (!bValid)
	{
		if (GC.getTerrainInfo(pPlot->getTerrainType()).isFound())
		{
			bValid = true;
		}
	}

	if (!bValid)
	{
		if (GC.getTerrainInfo(pPlot->getTerrainType()).isFoundCoast())
		{
			if (pPlot->isCoastalLand())
			{
				bValid = true;
			}
		}
	}

	long lResult = 0;
	if(GC.getUSE_CAN_FOUND_CITIES_ON_WATER_CALLBACK())
	{
		CyArgsList argsList2;
		argsList2.add(iX);
		argsList2.add(iY);
		lResult=0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "canFoundCitiesOnWater", argsList2.makeFunctionArgs(), &lResult);
	}

	if (lResult == 1)
	{
		bValid = true;
	}
	else
	{
		if (pPlot->isWater())
		{
			return false;
		}
	}

	if (!bValid)
	{
		return false;
	}

	if (!bTestVisible)
	{
		iRange = GC.getMIN_CITY_RANGE();

		/// reduced city distance - start - Nightinggale
		if (GC.getGameINLINE().isOption(GAMEOPTION_REDUCED_CITY_DISTANCE))
		{
			bool bOwnsAll = true;
			for (iDX = -1; iDX <= 1 && bOwnsAll; iDX++)
			{
				for (iDY = -1; iDY <= 1 && bOwnsAll; iDY++)
				{
					pLoopPlot = plotXY(pPlot->getX_INLINE(), pPlot->getY_INLINE(), iDX, iDY);
					if (pLoopPlot != NULL && pLoopPlot->getOwnerINLINE() != getID())
					{
						bOwnsAll = false;
					}
				}
			}
			if (bOwnsAll)
			{
				--iRange;
			}
		}
		/// reduced city distance - end - Nightinggale

		for (iDX = -(iRange); iDX <= iRange; iDX++)
		{
			for (iDY = -(iRange); iDY <= iRange; iDY++)
			{
				pLoopPlot	= plotXY(pPlot->getX_INLINE(), pPlot->getY_INLINE(), iDX, iDY);

				if (pLoopPlot != NULL)
				{
					if (pLoopPlot->isCity())
					{
						if (pLoopPlot->area() == pPlot->area())
						{
							if (!GET_PLAYER(pLoopPlot->getOwnerINLINE()).isNative())
							{
								return false;
							}
						}
					}
				}
			}
		}
	}

	return true;
}


void CvPlayer::found(int iX, int iY)
{
	if (!canFound(iX, iY))
	{
		return;
	}

	CvCity* pCity = initCity(iX, iY, true);
	FAssertMsg(pCity != NULL, "City is not assigned a valid value");

	for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		BuildingTypes eLoopBuilding = ((BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(iI)));

		if (eLoopBuilding != NO_BUILDING)
		{
			if (GC.getBuildingInfo(eLoopBuilding).getFreeStartEra() != NO_ERA)
			{
				if (GC.getGameINLINE().getStartEra() >= GC.getBuildingInfo(eLoopBuilding).getFreeStartEra())
				{
					if (pCity->canConstruct(eLoopBuilding))
					{
						pCity->setHasRealBuilding(eLoopBuilding, true);
					}
				}
			}
		}
	}

	if (getAdvancedStartPoints() < 0)
	{
		if (isHuman())
		{
			pCity->chooseProduction();
		}
		else
		{
			pCity->doFoundMessage();
		}
	}

	gDLL->getEventReporterIFace()->cityBuilt(pCity);
}


bool CvPlayer::canTrain(UnitTypes eUnit, bool bContinue, bool bTestVisible, bool bIgnoreCost) const
{
	PROFILE_FUNC();

	UnitClassTypes eUnitClass;
	eUnitClass = ((UnitClassTypes)(GC.getUnitInfo(eUnit).getUnitClassType()));

	FAssert(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(eUnitClass) == eUnit);
	if (GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(eUnitClass) != eUnit)
	{
		return false;
	}

	if (!bIgnoreCost)
	{
		bool bCanBuild = false;
		for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
		{
			if (GC.getUnitInfo(eUnit).getYieldCost(eYield) > 0)
			{
				bCanBuild = true;
				break;
			}
		}

		if (!bCanBuild)
		{
			return false;
		}
	}

	if (GC.getGameINLINE().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
	{
		if (GC.getUnitInfo(eUnit).isFound())
		{
			return false;
		}
	}

	//if (!bTestVisible)
	{
		if (GC.getUnitInfo(eUnit).getSpecialUnitType() != NO_SPECIALUNIT)
		{
			if (!(GC.getGameINLINE().isSpecialUnitValid((SpecialUnitTypes)(GC.getUnitInfo(eUnit).getSpecialUnitType()))))
			{
				return false;
			}
		}
	}

	return true;
}


bool CvPlayer::canConstruct(BuildingTypes eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost) const
{
	BuildingClassTypes eBuildingClass;
	CvTeamAI& currentTeam = GET_TEAM(getTeam());

	eBuildingClass = ((BuildingClassTypes)(GC.getBuildingInfo(eBuilding).getBuildingClassType()));

	FAssert(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(eBuildingClass) == eBuilding);
	if (GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(eBuildingClass) != eBuilding)
	{
		return false;
	}

	if (!bIgnoreCost)
	{
		bool bCanBuild = false;
		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			if (GC.getBuildingInfo(eBuilding).getYieldCost(iYield) > 0)
			{
				bCanBuild = true;
				break;
			}
		}

		if (!bCanBuild)
		{
			return false;
		}
	}
	if (GC.getGameINLINE().countCivTeamsEverAlive() < GC.getBuildingInfo(eBuilding).getNumTeamsPrereq())
	{
		return false;
	}

	if (GC.getBuildingInfo(eBuilding).getVictoryPrereq() != NO_VICTORY)
	{
		if (!(GC.getGameINLINE().isVictoryValid((VictoryTypes)(GC.getBuildingInfo(eBuilding).getVictoryPrereq()))))
		{
			return false;
		}
	}

	if (GC.getBuildingInfo(eBuilding).getMaxStartEra() != NO_ERA)
	{
		if (GC.getGameINLINE().getStartEra() > GC.getBuildingInfo(eBuilding).getMaxStartEra())
		{
			return false;
		}
	}

	//if (!bTestVisible)
	{
		if (GC.getBuildingInfo(eBuilding).getSpecialBuildingType() != NO_SPECIALBUILDING)
		{
			if (!(GC.getGameINLINE().isSpecialBuildingValid((SpecialBuildingTypes)(GC.getBuildingInfo(eBuilding).getSpecialBuildingType()))))
			{
				return false;
			}
		}

		if (getNumCities() < GC.getBuildingInfo(eBuilding).getNumCitiesPrereq())
		{
			return false;
		}

		if (getHighestUnitLevel() < GC.getBuildingInfo(eBuilding).getUnitLevelPrereq())
		{
			return false;
		}
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			if (getBuildingClassCount((BuildingClassTypes)iI) < getBuildingClassPrereqBuilding(eBuilding, ((BuildingClassTypes)iI), ((bContinue) ? 0 : getBuildingClassMaking(eBuildingClass))))
			{
				return false;
			}
		}
		// R&R, ray, National Wonders
		if (GC.getBuildingInfo(eBuilding).isNationalWonder())
		{
			if (getBuildingClassCount(eBuildingClass) >= 1)
			{
				return false;
			}
		}

	}

	return true;
}

int CvPlayer::getYieldProductionNeeded(UnitTypes eUnit, YieldTypes eYield) const
{
	UnitClassTypes eUnitClass = (UnitClassTypes)GC.getUnitInfo(eUnit).getUnitClassType();
	FAssert(NO_UNITCLASS != eUnitClass);

	int iProductionNeeded = GC.getUnitInfo(eUnit).getYieldCost(eYield);

	// TAC - AI Wagon Train Cheat - koma13 - START
	if (!isHuman())
	{
		if ((eYield != YIELD_HAMMERS) && (GC.getUnitInfo(eUnit).getDefaultUnitAIType() == UNITAI_WAGON))
		{
			iProductionNeeded = 0;
		}
		if ((eYield != YIELD_HAMMERS) && (GC.getUnitInfo(eUnit).getDefaultUnitAIType() == UNITAI_TRANSPORT_COAST))
		{
			iProductionNeeded = 0;
		}
	}
	// TAC - AI Wagon Train Cheat - koma13 - END
	iProductionNeeded *= GC.getDefineINT("UNIT_PRODUCTION_PERCENT");
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getEraInfo(GC.getGameINLINE().getStartEra()).getTrainPercent();
	iProductionNeeded /= 100;

	if (!isHuman() && !isNative())
	{
		iProductionNeeded *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAITrainPercent();
		iProductionNeeded /= 100;

		iProductionNeeded *= std::max(0, ((GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIPerEraModifier() * getCurrentEra()) + 100));
		iProductionNeeded /= 100;
	}

	// Python cost modifier
	if(GC.getUSE_GET_UNIT_COST_MOD_CALLBACK())
	{
		CyArgsList argsList;
		argsList.add(getID());	// Player ID
		argsList.add((int)eUnit);
		argsList.add((int)eYield);
		long lResult=0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "getUnitCostMod", argsList.makeFunctionArgs(), &lResult);

		if (lResult > 1)
		{
			iProductionNeeded *= lResult;
			iProductionNeeded /= 100;
		}
	}

	return iProductionNeeded;
}


int CvPlayer::getYieldProductionNeeded(BuildingTypes eBuilding, YieldTypes eYield) const
{
	int iProductionNeeded = GC.getBuildingInfo(eBuilding).getYieldCost(eYield);

	iProductionNeeded *= GC.getDefineINT("BUILDING_PRODUCTION_PERCENT");
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getConstructPercent();
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getEraInfo(GC.getGameINLINE().getStartEra()).getConstructPercent();
	iProductionNeeded /= 100;

	iProductionNeeded *= 100 + getBuildingRequiredYieldModifier(eYield);
	iProductionNeeded /= 100;

	if (!isHuman())
	{
		iProductionNeeded *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIConstructPercent();
		iProductionNeeded /= 100;

		iProductionNeeded *= std::max(0, ((GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIPerEraModifier() * getCurrentEra()) + 100));
		iProductionNeeded /= 100;
	}

	return iProductionNeeded;
}


int CvPlayer::getProductionModifier(UnitTypes eUnit) const
{
	int iMultiplier = 0;

	if (GC.getUnitInfo(eUnit).isMilitaryProduction())
	{
		iMultiplier += getMilitaryProductionModifier();
	}

	for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			iMultiplier += GC.getUnitInfo(eUnit).getProductionTraits(iI);

			if (GC.getUnitInfo(eUnit).getSpecialUnitType() != NO_SPECIALUNIT)
			{
				iMultiplier += GC.getSpecialUnitInfo((SpecialUnitTypes) GC.getUnitInfo(eUnit).getSpecialUnitType()).getProductionTraits(iI);
			}
		}
	}

	return iMultiplier;
}

int CvPlayer::getProductionModifier(BuildingTypes eBuilding) const
{
	int iMultiplier = 0;
	for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			iMultiplier += GC.getBuildingInfo(eBuilding).getProductionTraits(iI);

			if (GC.getBuildingInfo(eBuilding).getSpecialBuildingType() != NO_SPECIALBUILDING)
			{
				iMultiplier += GC.getSpecialBuildingInfo((SpecialBuildingTypes) GC.getBuildingInfo(eBuilding).getSpecialBuildingType()).getProductionTraits(iI);
			}
		}
	}

	BuildingClassTypes eBuildingClass = (BuildingClassTypes) GC.getBuildingInfo(eBuilding).getBuildingClassType();
	for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		TraitTypes eTrait = (TraitTypes) iI;
		if (hasTrait(eTrait))
		{
			iMultiplier += GC.getTraitInfo(eTrait).getBuildingProductionModifier(eBuildingClass);
		}
	}

	return iMultiplier;
}

int CvPlayer::getBuildingClassPrereqBuilding(BuildingTypes eBuilding, BuildingClassTypes ePrereqBuildingClass, int iExtra) const
{
	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	int iPrereqs = kBuilding.getPrereqNumOfBuildingClass(ePrereqBuildingClass);

	// dont bother with the rest of the calcs if we have no prereqs
	if (iPrereqs < 1)
	{
		return 0;
	}

	BuildingClassTypes eBuildingClass = (BuildingClassTypes)kBuilding.getBuildingClassType();

	iPrereqs *= std::max(0, (GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getBuildingClassPrereqModifier() + 100));
	iPrereqs /= 100;

	iPrereqs *= (getBuildingClassCount((BuildingClassTypes)(GC.getBuildingInfo(eBuilding).getBuildingClassType())) + iExtra + 1);

	if (GC.getGameINLINE().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
	{
		iPrereqs = std::min(1, iPrereqs);
	}

	return iPrereqs;
}


void CvPlayer::removeBuildingClass(BuildingClassTypes eBuildingClass)
{
	CvCity* pLoopCity;
	BuildingTypes eBuilding;
	int iLoop;

	eBuilding = ((BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(eBuildingClass)));

	if (eBuilding != NO_BUILDING)
	{
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			if (pLoopCity->isHasRealBuilding(eBuilding))
			{
				pLoopCity->setHasRealBuilding(eBuilding, false);
				break;
			}
		}
	}
}

void CvPlayer::processTrait(TraitTypes eTrait, int iChange)
{
	CvTraitInfo& kTrait = GC.getTraitInfo(eTrait);

	changeTraitCount(eTrait, iChange);

	changeLevelExperienceModifier(kTrait.getLevelExperienceModifier() * iChange);
	changeGreatGeneralRateModifier(kTrait.getGreatGeneralRateModifier() * iChange);
	changeDomesticGreatGeneralRateModifier(kTrait.getDomesticGreatGeneralRateModifier() * iChange);

	changeNativeAngerModifier(kTrait.getNativeAngerModifier() * iChange);
	changeNativeCombatModifier(kTrait.getNativeCombatModifier() * iChange);
	changeMissionaryRateModifier(kTrait.getMissionaryModifier() * iChange);
	changeNativeTradeModifier(kTrait.getNativeTradeModifier() * iChange); // R&R, ray, new Attribute in Traits 

	for (int iProfession = 0; iProfession < GC.getNumProfessionInfos(); ++iProfession)
	{
		ProfessionTypes eProfession = (ProfessionTypes) iProfession;
		int iNewValue = getProfessionEquipmentModifier(eProfession) + kTrait.getProfessionEquipmentModifier(eProfession) * iChange;
		setProfessionEquipmentModifier(eProfession, iNewValue);
	}

	// R&R, ray, max tax decrease trait, START
	int maxtaxdecrease = kTrait.getMaxTaxRateThresholdDecrease();
	m_iMaxTaxRate = m_iMaxTaxRate - maxtaxdecrease;
	// R&R, ray, max tax decrease trait, END

	int iLoop;
	std::vector<CvUnit*> apUnits;
	for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		apUnits.push_back(pLoopUnit);
	}
	for (uint i = 0; i < m_aEuropeUnits.size(); ++i)
	{
		apUnits.push_back(m_aEuropeUnits[i]);
	}
	/*** TRIANGLETRADE 10/23/08 by DPII ***/
	for (uint i = 0; i < m_aAfricaUnits.size(); ++i)
	{
		apUnits.push_back(m_aAfricaUnits[i]);
	}
	/**************************************/
	// R&R, ray, Port Royal
	for (uint i = 0; i < m_aPortRoyalUnits.size(); ++i)
	{
		apUnits.push_back(m_aPortRoyalUnits[i]);
	}
	// R&R, ray, Port Royal - END
	for (uint i = 0; i < apUnits.size(); ++i)
	{
		CvUnit* pLoopUnit = apUnits[i];
		for (int iJ = 0; iJ < GC.getNumPromotionInfos(); iJ++)
		{
			if (kTrait.isFreePromotion(iJ))
			{
				if ((pLoopUnit->getUnitCombatType() != NO_UNITCOMBAT) && kTrait.isFreePromotionUnitCombat(pLoopUnit->getUnitCombatType()))
				{
					pLoopUnit->changeFreePromotionCount(((PromotionTypes)iJ), iChange);
				}
			}
		}
	}

	for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
	{
		changeYieldRateModifier(eYield, iChange * kTrait.getYieldModifier(eYield));
		changeBuildingRequiredYieldModifier(eYield, kTrait.getBuildingRequiredYieldModifier(eYield) * iChange);

		if (kTrait.isTaxYieldModifier(eYield))
		{
			changeTaxYieldModifierCount(eYield, iChange);
		}

		for (BuildingClassTypes eBuildingClass = FIRST_BUILDINGCLASS; eBuildingClass < GC.getNumBuildingClassInfos(); ++eBuildingClass)
		{
			changeBuildingYieldChange(eBuildingClass, eYield, iChange * kTrait.getBuildingYieldChange(eBuildingClass, eYield));
		}

		// update the plot yield caches
		// vanilla bug was present here. CityExtraYield would not apply even though it would call updateYield() if the trait changed the value.
		// updateYield() relies on updateCityExtraYield having set the cache correctly first
		//    Nightinggale
		if (kTrait.getCityExtraYield(eYield) != 0)
		{
			updateCityExtraYield(eYield);
		}
		if (kTrait.getExtraYieldThreshold(eYield) != 0)
		{
			updateExtraYieldThreshold(eYield);
		}
	}

	for (int iUnitClass = 0; iUnitClass < GC.getNumUnitClassInfos(); ++iUnitClass)
	{
		changeUnitMoveChange((UnitClassTypes) iUnitClass, iChange * kTrait.getUnitMoveChange(iUnitClass));
		changeUnitStrengthModifier((UnitClassTypes) iUnitClass, iChange * kTrait.getUnitStrengthModifier(iUnitClass));
	}

	for (int iProfession = 0; iProfession < GC.getNumProfessionInfos(); ++iProfession)
	{
		changeProfessionMoveChange((ProfessionTypes) iProfession, iChange * kTrait.getProfessionMoveChange(iProfession));
	}

	for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); ++iBuildingClass)
	{
		if (kTrait.isFreeBuildingClass(iBuildingClass))
		{
			BuildingTypes eFreeBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(iBuildingClass);
			if (eFreeBuilding != NO_BUILDING)
			{
				changeFreeBuildingCount(eFreeBuilding, iChange);
			}
		}
	}

	if (kTrait.getNativeAttitudeChange() != 0)
	{
		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes) iPlayer);

			if (kLoopPlayer.isAlive() && kLoopPlayer.isNative())
			{
				kLoopPlayer.AI_changeAttitudeExtra(getID(), kTrait.getNativeAttitudeChange());

				if (iChange > 0 && kTrait.getNativeAttitudeChange() > 0)
				{
					GET_TEAM(getTeam()).makePeace(kLoopPlayer.getTeam());
				}
			}
		}
	}

	// R&R, ray, new Attribute in Traits  - START
	if (kTrait.getEuropeanAttitudeChange() != 0)
	{
		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes) iPlayer);

			// Erik: Do not attempt to make peace with ourselves!
			if (kLoopPlayer.getTeam() == getTeam())
				continue;

			if (kLoopPlayer.isAlive() && !kLoopPlayer.isNative() && !kLoopPlayer.isEurope())
			{
				kLoopPlayer.AI_changeAttitudeExtra(getID(), kTrait.getEuropeanAttitudeChange());

				if (iChange > 0 && kTrait.getEuropeanAttitudeChange() > 0)
				{
					// R&R, ray, making peace with Animals fixed
					if (!GC.getGameINLINE().isBarbarianPlayer(kLoopPlayer.getID()))
					{
						GET_TEAM(getTeam()).makePeace(kLoopPlayer.getTeam());
					}
				}
			}
		}
	}

	if (kTrait.getKingAttitudeChange() != 0)
	{
		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes) iPlayer);

			if (kLoopPlayer.isAlive() && kLoopPlayer.isEurope())
			{
				kLoopPlayer.AI_changeAttitudeExtra(getID(), kTrait.getKingAttitudeChange());
			}
		}
	}
	// R&R, ray, new Attribute in Traits - END

	changeCityDefenseModifier(iChange * kTrait.getCityDefense());
}


void CvPlayer::processFather(FatherTypes eFather, int iChange)
{
	CvFatherInfo& kFatherInfo = GC.getFatherInfo(eFather);
	if (kFatherInfo.getTrait() != NO_TRAIT)
	{
		processTrait((TraitTypes) kFatherInfo.getTrait(), iChange);
	}
}

void CvPlayer::processFatherOnce(FatherTypes eFather)
{
	CvFatherInfo& kFatherInfo = GC.getFatherInfo(eFather);

	CivEffect()->applyCivEffect(kFatherInfo.getCivEffect());

	for (int iUnitClass = 0; iUnitClass < GC.getNumUnitClassInfos(); ++iUnitClass)
	{
		UnitTypes eUnit = (UnitTypes) GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iUnitClass);
		if (eUnit != NO_UNIT)
		{
			CvPlot* pPlot = NULL;
			int iLoop;
			for (CvCity* pCity = firstCity(&iLoop); pCity != NULL && pPlot == NULL; pCity = nextCity(&iLoop))
			{
				CvPlot* pCityPlot = pCity->plot();
				if (pCityPlot->isValidDomainForAction(eUnit))
				{
					pPlot = pCityPlot;
				}
			}

			//WTP, ray fix for issue Free Water Units - START
			CvPlot* pPortPlot = NULL;
			int iLoopWater; 
			for (CvCity* pPortCity = firstCity(&iLoopWater); pPortCity != NULL && pPortPlot == NULL; pPortCity = nextCity(&iLoopWater))
			{
				CvPlot* pPortCityPlot = pPortCity->plot();
				if (pPortCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()) && pPortCityPlot->isEuropeAccessable())
				{
					pPortPlot = pPortCityPlot;
				}
			}
			//WTP, ray fix for issue Free Water Units - END

			for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL && pPlot == NULL; pLoopUnit = nextUnit(&iLoop))
			{
				CvPlot* pUnitPlot = pLoopUnit->plot();
				if (pUnitPlot != NULL && pUnitPlot->isValidDomainForAction(eUnit))
				{
					pPlot = pUnitPlot;
				}
			}

			for (int i = 0; i < kFatherInfo.getFreeUnits(iUnitClass); ++i)
			{
				//WTP, ray fix for issue Free Water Units - START
				if (pPlot != NULL && GC.getUnitInfo(eUnit).getDomainType() == DOMAIN_LAND)
				// if (pPlot != NULL)
				//WTP, ray fix for issue Free Water Units - END
				{
					initUnit(eUnit, GC.getUnitInfo(eUnit).getDefaultProfession(), pPlot->getX_INLINE(), pPlot->getY_INLINE());
				}
				//WTP, ray fix for issue Free Water Units - START
				else if (pPortPlot != NULL && GC.getUnitInfo(eUnit).getDomainType() == DOMAIN_SEA)
				{
					initUnit(eUnit, GC.getUnitInfo(eUnit).getDefaultProfession(), pPortPlot->getX_INLINE(), pPortPlot->getY_INLINE());
				}
				//WTP, ray fix for issue Free Water Units - END
				else if (canTradeWithEurope())
				{
					CvPlot* pStartingPlot = getStartingPlot();
					if (GC.getUnitInfo(eUnit).getDomainType() == DOMAIN_SEA && pStartingPlot != NULL)
					{
						CvUnit* pUnit = initUnit(eUnit, GC.getUnitInfo(eUnit).getDefaultProfession(), INVALID_PLOT_COORD, INVALID_PLOT_COORD);
						if (pUnit != NULL)
						{
							pUnit->setUnitTravelState(UNIT_TRAVEL_STATE_IN_EUROPE, false);
							//add unit to map after setting Europe state so that it doesn't bump enemy units
							pUnit->addToMap(pStartingPlot->getX_INLINE(), pStartingPlot->getY_INLINE());
						}
					}
					else
					{
						initEuropeUnit(eUnit);
					}
				}
			}
		}
	}

	for (int iImprovement = 0; iImprovement < GC.getNumImprovementInfos(); ++iImprovement)
	{
		if (kFatherInfo.isRevealImprovement(iImprovement))
		{
			for (int i = 0; i < GC.getMapINLINE().numPlotsINLINE(); ++i)
			{
				CvPlot* pPlot = GC.getMapINLINE().plotByIndexINLINE(i);

				if (pPlot->getImprovementType() == iImprovement)
				{
					pPlot->setRevealed(getTeam(), true, false, NO_TEAM);
				}
			}
		}
	}

	changeFatherScore(1);
}

int CvPlayer::getFatherPointMultiplier() const
{
	return m_iFatherPointMultiplier;
}

void CvPlayer::setFatherPointMultiplier(int iValue)
{
	m_iFatherPointMultiplier = iValue;
}

void CvPlayer::changeFatherPoints(FatherPointTypes ePointType, int iChange)
{
	GET_TEAM(getTeam()).changeFatherPoints(ePointType, iChange * getFatherPointMultiplier() / 100);
}

bool CvPlayer::canBuild(const CvPlot* pPlot, BuildTypes eBuild, bool bTestEra, bool bTestVisible) const
{
	PROFILE_FUNC();

	// CivEffect check
	// Should be first because of lazy checking optimization. It's just a BoolArray lookup, hence very fast
	if (!CivEffect()->canUseBuild(eBuild))
	{
		return false;
	}

	if (!(pPlot->canBuild(eBuild, getID(), bTestVisible)))
	{
		return false;
	}
	if (!bTestVisible)
	{
		if (std::max(0, getGold()) < getBuildCost(pPlot, eBuild))
		{
			return false;
		}
	}

	return true;
}

// Returns the cost
int CvPlayer::getBuildCost(const CvPlot* pPlot, BuildTypes eBuild) const
{
	FAssert(eBuild >= 0 && eBuild < GC.getNumBuildInfos());

	int iCost;
	
	if (pPlot->getBuildProgress(eBuild) > 0)
	{
		iCost = 0;
	}

	else
	{
		iCost = std::max(0, GC.getBuildInfo(eBuild).getCost());

		for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); ++iTrait)
		{
			TraitTypes eTrait = (TraitTypes) iTrait;
			if (eTrait != NO_TRAIT)
			{
				if (hasTrait(eTrait))
				{
					iCost *= std::max(0, (100 + GC.getTraitInfo(eTrait).getImprovementPriceModifier()));
					iCost /= 100;
				}
			}
		}
	}									//Schmiddie, Improvement Price Modifier

	return iCost;
}


RouteTypes CvPlayer::getBestRoute(CvPlot* pPlot) const
{
	PROFILE_FUNC();

	RouteTypes eRoute;
	RouteTypes eBestRoute;
	int iValue;
	int iBestValue;
	int iI;

	iBestValue = 0;
	eBestRoute = NO_ROUTE;

	for (iI = 0; iI < GC.getNumBuildInfos(); iI++)
	{
		eRoute = ((RouteTypes)(GC.getBuildInfo((BuildTypes)iI).getRoute()));

		if (eRoute != NO_ROUTE)
		{
			if(pPlot != NULL)
			{
				if ((pPlot->getRouteType() == eRoute) || canBuild(pPlot, ((BuildTypes)iI)))
				{
					iValue = GC.getRouteInfo(eRoute).getValue();
					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						eBestRoute = eRoute;
					}
				}
			}
		}
	}

	return eBestRoute;
}


int CvPlayer::getImprovementUpgradeRate() const
{
	int iRate;

	iRate = 1; // XXX

	iRate *= std::max(0, (getImprovementUpgradeRateModifier() + 100));
	iRate /= 100;

	return iRate;
}


int CvPlayer::calculateTotalYield(YieldTypes eYield) const
{
	CvCity* pLoopCity;
	int iTotal = 0;
	int iLoop = 0;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iTotal += pLoopCity->calculateNetYield(eYield);
	}

	return iTotal;
}

void CvPlayer::calculateTotalYields(int aiYields[]) const
{
	CvCity* pLoopCity;
	int iTotal = 0;
	int iLoop = 0;

	for(int i=0;i<NUM_YIELD_TYPES;i++)
	{
		aiYields[i] = 0;
	}

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		int aiTempYields[NUM_YIELD_TYPES];
		pLoopCity->calculateNetYields(aiTempYields);
		for(int i=0;i<NUM_YIELD_TYPES;i++)
		{
			aiYields[i] += aiTempYields[i];
		}
	}
}

bool CvPlayer::isCivic(CivicTypes eCivic) const
{
	int iI;
	for (iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
	{
		if (getCivic((CivicOptionTypes)iI) == eCivic)
		{
			return true;
		}
	}
	return false;
}

bool CvPlayer::canDoCivics(CivicTypes eCivic) const
{
	PROFILE_FUNC();

	if (eCivic == NO_CIVIC)
	{
		return true;
	}

	// CivEffect check
	if (!CivEffect()->canUseCivic(eCivic))
	{
		return false;
	}

	if(GC.getUSE_CAN_DO_CIVIC_CALLBACK())
	{
		CyArgsList argsList;
		argsList.add(getID());
		argsList.add(eCivic);
		long lResult=0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "canDoCivic", argsList.makeFunctionArgs(), &lResult);
		if (lResult == 1)
		{
			return true;
		}
	}

	if(GC.getUSE_CANNOT_DO_CIVIC_CALLBACK())
	{
		CyArgsList argsList2; // XXX
		argsList2.add(getID());
		argsList2.add(eCivic);
		long lResult=0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "cannotDoCivic", argsList2.makeFunctionArgs(), &lResult);
		if (lResult == 1)
		{
			return false;
		}
	}

	return true;
}

int CvPlayer::greatGeneralThreshold() const
{
	int iThreshold = ((GC.getDefineINT("GREAT_GENERALS_THRESHOLD") * std::max(0, (getGreatGeneralsThresholdModifier() + 100))) / 100);

	iThreshold *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGreatGeneralPercent();
	iThreshold /= 100;
	iThreshold *= GC.getEraInfo(GC.getGameINLINE().getStartEra()).getGreatGeneralPercent();
	iThreshold /= 100;

	return std::max(1, iThreshold);
}
// R&R, ray, Great Admirals - START
int CvPlayer::greatAdmiralThreshold() const
{
	int iThreshold = ((GC.getDefineINT("GREAT_ADMIRALS_THRESHOLD") * std::max(0, (getGreatAdmiralsThresholdModifier() + 100))) / 100);

	iThreshold *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGreatGeneralPercent();
	iThreshold /= 100;
	iThreshold *= GC.getEraInfo(GC.getGameINLINE().getStartEra()).getGreatGeneralPercent();
	iThreshold /= 100;

	return std::max(1, iThreshold);
}
// R&R, ray, Great Admirals - END

int CvPlayer::immigrationThreshold() const
{
	int iThreshold = ((GC.getIMMIGRATION_THRESHOLD() * std::max(0, (getImmigrationThresholdMultiplier()))) / 100);

    // TAC - Immigration Max Cross Limit - koma13 - START
	int iMaxCrossLimit = GC.getIMMIGRATION_MAX_CROSS_LIMIT();

	//ray Korrektur Immigration Threshold
	if (iThreshold == 0)
	{
	    iThreshold = iMaxCrossLimit;
	}

	//ray kleine Korrektur
	//if (iThreshold >= iMaxCrossLimit)
	if (iThreshold > iMaxCrossLimit)
	{
	    iThreshold = iMaxCrossLimit;
	}
	// TAC - Immigration Max Cross Limit - koma13 - END

	for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); ++iTrait)
	{
		TraitTypes eTrait = (TraitTypes) iTrait;
		if (hasTrait(eTrait))
		{
			iThreshold *= 100 + GC.getTraitInfo(eTrait).getImmigrationThresholdModifier();
			iThreshold /= 100;
		}
	}


	iThreshold *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
	iThreshold /= 100;

	iThreshold *= GC.getEraInfo(GC.getGameINLINE().getStartEra()).getGrowthPercent();
	iThreshold /= 100;


	if (!isHuman())
	{
		iThreshold *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIGrowthPercent();
		iThreshold /= 100;
	}

	return std::max(1, iThreshold);
}

int CvPlayer::revolutionEuropeUnitThreshold() const
{
	int iThreshold = ((GC.getDefineINT("REVOLUTION_EUROPE_UNIT_THRESHOLD") * std::max(0, (getRevolutionEuropeUnitThresholdMultiplier()))) / 100);

	iThreshold *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
	iThreshold /= 100;

	iThreshold *= GC.getEraInfo(GC.getGameINLINE().getStartEra()).getTrainPercent();
	iThreshold /= 100;

	iThreshold *= GC.getHandicapInfo(getHandicapType()).getAITrainPercent();
	iThreshold /= 100;

	if (!isHuman())
	{
		iThreshold *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIKingUnitThresholdPercent();
		iThreshold /= 100;
	}

	return std::max(1, iThreshold);
}

CvPlot* CvPlayer::getStartingPlot() const
{
	return GC.getMapINLINE().plotSorenINLINE(m_iStartingX, m_iStartingY);
}


void CvPlayer::setStartingPlot(CvPlot* pNewValue, bool bUpdateStartDist)
{
	CvPlot* pOldStartingPlot;

	pOldStartingPlot = getStartingPlot();

	if (pOldStartingPlot != pNewValue)
	{
		if (pOldStartingPlot != NULL)
		{
			pOldStartingPlot->area()->changeNumStartingPlots(-1);

			if (bUpdateStartDist)
			{
				GC.getMapINLINE().updateMinOriginalStartDist(pOldStartingPlot->area());
			}
		}

		if (pNewValue == NULL)
		{
			m_iStartingX = INVALID_PLOT_COORD;
			m_iStartingY = INVALID_PLOT_COORD;
		}
		else
		{
			m_iStartingX = pNewValue->getX_INLINE();
			m_iStartingY = pNewValue->getY_INLINE();

			getStartingPlot()->area()->changeNumStartingPlots(1);

			if (bUpdateStartDist)
			{
				GC.getMapINLINE().updateMinOriginalStartDist(getStartingPlot()->area());
			}
		}
	}
}


int CvPlayer::getTotalPopulation() const
{
	return m_iTotalPopulation;
}


int CvPlayer::getAveragePopulation() const
{
	if (getNumCities() == 0)
	{
		return 0;
	}

	return ((getTotalPopulation() / getNumCities()) + 1);
}


void CvPlayer::changeTotalPopulation(int iChange)
{
	changePopScore(-getTotalPopulation());

	m_iTotalPopulation += iChange;
	FAssert(getTotalPopulation() >= 0);

	changePopScore(getTotalPopulation());
}


long CvPlayer::getRealPopulation() const
{
	__int64 iTotalPopulation = 0;
	int iLoop = 0;

	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iTotalPopulation += pLoopCity->getRealPopulation();
	}

	if (iTotalPopulation > MAX_INT)
	{
		iTotalPopulation = MAX_INT;
	}

	return ((long)(iTotalPopulation));
}


int CvPlayer::getTotalLand() const
{
	return m_iTotalLand;
}


void CvPlayer::changeTotalLand(int iChange)
{
	m_iTotalLand = (m_iTotalLand + iChange);
	FAssert(getTotalLand() >= 0);
}


int CvPlayer::getTotalLandScored() const
{
	return m_iTotalLandScored;
}


void CvPlayer::changeTotalLandScored(int iChange)
{
	if (iChange != 0)
	{
		changeLandScore(-getTotalLandScored());

		m_iTotalLandScored = (m_iTotalLandScored + iChange);
		FAssert(getTotalLandScored() >= 0);

		changeLandScore(getTotalLandScored());
	}
}


int CvPlayer::getGold() const
{
	return m_iGold;
}


void CvPlayer::setGold(int iNewValue)
{
	if (getGold() != iNewValue)
	{
		m_iGold = iNewValue;

		FAssert(getGold() >= 0);

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(MiscButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}

void CvPlayer::changeGold(int iChange)
{
	setGold(getGold() + iChange);
	if (iChange > 0)
	{
		GET_PLAYER(getID()).AI_changeTotalIncome(iChange);
	}
}

int CvPlayer::getExtraTradeMultiplier(PlayerTypes eOtherPlayer) const
{
	int iMultiplier = 100;
	if (!isHuman() && !GET_PLAYER(eOtherPlayer).isHuman())
	{
		iMultiplier += GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIExtraTradePercent();
	}
	return iMultiplier;
}

int CvPlayer::getAdvancedStartPoints() const
{
	return m_iAdvancedStartPoints;
}


void CvPlayer::setAdvancedStartPoints(int iNewValue)
{
	if (getAdvancedStartPoints() != iNewValue)
	{
		m_iAdvancedStartPoints = iNewValue;

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(MiscButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}

void CvPlayer::changeAdvancedStartPoints(int iChange)
{
	setAdvancedStartPoints(getAdvancedStartPoints() + iChange);
}

int CvPlayer::getGreatGeneralsCreated() const
{
	return m_iGreatGeneralsCreated;
}

void CvPlayer::incrementGreatGeneralsCreated()
{
	m_iGreatGeneralsCreated++;
}

int CvPlayer::getGreatGeneralsThresholdModifier() const
{
	return m_iGreatGeneralsThresholdModifier;
}

void CvPlayer::changeGreatGeneralsThresholdModifier(int iChange)
{
	m_iGreatGeneralsThresholdModifier += iChange;
}

// R&R, ray, Great Admirals - START
int CvPlayer::getGreatAdmiralsCreated() const
{
	return m_iGreatAdmiralsCreated;
}

void CvPlayer::incrementGreatAdmiralsCreated()
{
	m_iGreatAdmiralsCreated++;
}

int CvPlayer::getGreatAdmiralsThresholdModifier() const
{
	return m_iGreatAdmiralsThresholdModifier;
}

void CvPlayer::changeGreatAdmiralsThresholdModifier(int iChange)
{
	m_iGreatAdmiralsThresholdModifier += iChange;
}
// R&R, ray, Great Admirals - END

int CvPlayer::getGreatGeneralRateModifier() const
{
	return m_iGreatGeneralRateModifier;
}

void CvPlayer::changeGreatGeneralRateModifier(int iChange)
{
	m_iGreatGeneralRateModifier += iChange;
}


int CvPlayer::getDomesticGreatGeneralRateModifier() const
{
	return (GC.getDefineINT("COMBAT_EXPERIENCE_IN_BORDERS_PERCENT") + m_iDomesticGreatGeneralRateModifier);
}


void CvPlayer::changeDomesticGreatGeneralRateModifier(int iChange)
{
	m_iDomesticGreatGeneralRateModifier += iChange;
}


int CvPlayer::getImmigrationThresholdMultiplier() const
{
	return m_iImmigrationThresholdMultiplier;
}


void CvPlayer::setImmigrationThresholdMultiplier(int iValue)
{
	m_iImmigrationThresholdMultiplier = iValue;
}

int CvPlayer::getRevolutionEuropeUnitThresholdMultiplier() const
{
	return m_iRevolutionEuropeUnitThresholdMultiplier;
}

void CvPlayer::setRevolutionEuropeUnitThresholdMultiplier(int iValue)
{
	m_iRevolutionEuropeUnitThresholdMultiplier = iValue;
}

int CvPlayer::getKingNumUnitMultiplier() const
{
	return m_iKingNumUnitMultiplier;
}

void CvPlayer::setKingNumUnitMultiplier(int iValue)
{
	m_iKingNumUnitMultiplier = iValue;
}

int CvPlayer::getNativeAngerModifier() const
{
	return m_iNativeAngerModifier;
}

void CvPlayer::changeNativeAngerModifier(int iChange)
{
	m_iNativeAngerModifier += iChange;
}

int CvPlayer::getFreeExperience() const
{
	return m_iFreeExperience;
}


void CvPlayer::changeFreeExperience(int iChange)
{
	m_iFreeExperience += iChange;
}

int CvPlayer::getWorkerSpeedModifier() const
{
	return m_iWorkerSpeedModifier;
}


void CvPlayer::changeWorkerSpeedModifier(int iChange)
{
	m_iWorkerSpeedModifier += iChange;
}


int CvPlayer::getImprovementUpgradeRateModifier() const
{
	return m_iImprovementUpgradeRateModifier;
}


void CvPlayer::changeImprovementUpgradeRateModifier(int iChange)
{
	m_iImprovementUpgradeRateModifier += iChange;
}


int CvPlayer::getMilitaryProductionModifier() const
{
	return m_iMilitaryProductionModifier;
}


void CvPlayer::changeMilitaryProductionModifier(int iChange)
{
	m_iMilitaryProductionModifier += iChange;
}

int CvPlayer::getCityDefenseModifier() const
{
	return m_iCityDefenseModifier;
}

void CvPlayer::changeCityDefenseModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iCityDefenseModifier += iChange;
		setCityBillboardDirty(true);
	}
}

int CvPlayer::getHighestUnitLevel()	const
{
	return m_iHighestUnitLevel;
}


void CvPlayer::setHighestUnitLevel(int iNewValue)
{
	m_iHighestUnitLevel = iNewValue;
	FAssert(getHighestUnitLevel() >= 0);
}

int CvPlayer::getFatherOverflowBells() const
{
	return m_iFatherOverflowBells;
}

void CvPlayer::setFatherOverflowBells(int iNewValue)
{
	m_iFatherOverflowBells = iNewValue;
	FAssert(getFatherOverflowBells() >= 0);
}

void CvPlayer::changeFatherOverflowBells(int iChange)
{
	setFatherOverflowBells(getFatherOverflowBells() + iChange);
}

int CvPlayer::getExpInBorderModifier() const
{
	return m_iExpInBorderModifier;
}

void CvPlayer::changeExpInBorderModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iExpInBorderModifier += iChange;
		FAssert(getExpInBorderModifier() >= 0);
	}
}

int CvPlayer::getLevelExperienceModifier() const
{
	return m_iLevelExperienceModifier;
}

void CvPlayer::changeLevelExperienceModifier(int iChange)
{
	m_iLevelExperienceModifier += iChange;
}

CvCity* CvPlayer::getCapitalCity() const
{
	return getCity(m_iCapitalCityID);
}

CvCity* CvPlayer::getPrimaryCity() const
{
	CvCity* pCity = getCapitalCity();

	if (pCity == NULL)
	{
		int iIndex;
		pCity = firstCity(&iIndex);
	}

	return pCity;
}

void CvPlayer::setCapitalCity(CvCity* pNewCapitalCity)
{
	CvCity* pOldCapitalCity = getCapitalCity();

	if (pOldCapitalCity != pNewCapitalCity)
	{
		if (pNewCapitalCity != NULL)
		{
			m_iCapitalCityID = pNewCapitalCity->getID();
		}
		else
		{
			m_iCapitalCityID = FFreeList::INVALID_INDEX;
		}

		if (pOldCapitalCity != NULL)
		{
			pOldCapitalCity->setBillboardDirty(true);
		}
		if (pNewCapitalCity != NULL)
		{
			pNewCapitalCity->setBillboardDirty(true);
		}
	}
}

int CvPlayer::getCitiesLost() const
{
	return m_iCitiesLost;
}


void CvPlayer::changeCitiesLost(int iChange)
{
	m_iCitiesLost += iChange;
}

int CvPlayer::getAssets() const
{
	return m_iAssets;
}

void CvPlayer::changeAssets(int iChange)
{
	m_iAssets += iChange;
	FAssert(getAssets() >= 0);
}

int CvPlayer::getPower() const
{
	return m_iPower;
}

void CvPlayer::changePower(int iChange)
{
	m_iPower += iChange;
	FAssert(getPower() >= 0);
}


int CvPlayer::getPopScore() const
{
	return (m_iPopulationScore / std::max(1, GET_TEAM(getTeam()).getNumMembers()));
}

void CvPlayer::changePopScore(int iChange)
{
	if (iChange != 0)
	{
		m_iPopulationScore += iChange;
		FAssert(getPopScore() >= 0);

		GC.getGameINLINE().setScoreDirty(true);
	}
}


int CvPlayer::getLandScore() const
{
	return (m_iLandScore  / std::max(1, GET_TEAM(getTeam()).getNumMembers()));
}


void CvPlayer::changeLandScore(int iChange)
{
	if (iChange != 0)
	{
		m_iLandScore += iChange;
		FAssert(getLandScore() >= 0);

		GC.getGameINLINE().setScoreDirty(true);
	}
}

int CvPlayer::getFatherScore() const
{
	return m_iFatherScore;
}

void CvPlayer::changeFatherScore(int iChange)
{
	if (iChange != 0)
	{
		m_iFatherScore += iChange;
		FAssert(getFatherScore() >= 0);
		GC.getGameINLINE().setScoreDirty(true);
	}
}

int CvPlayer::getCombatExperience() const
{
	return m_iCombatExperience;
}

void CvPlayer::setCombatExperience(int iExperience)
{
	FAssert(iExperience >= 0);

	if (iExperience != getCombatExperience())
	{
		m_iCombatExperience = iExperience;

		int iExperienceThreshold = greatGeneralThreshold();
		if (getCombatExperience() >= iExperienceThreshold && iExperienceThreshold > 0)
		{
			// create great person
			CvCity* pBestCity = NULL;
			int iBestValue = MAX_INT;
			int iLoop;
			for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				int iValue = 4 * GC.getGameINLINE().getSorenRandNum(getNumCities(), "Warlord City Selection");

				for (int i = 0; i < NUM_YIELD_TYPES; i++)
				{
					iValue += pLoopCity->findYieldRateRank((YieldTypes)i);
				}
				iValue += pLoopCity->findPopulationRank();

				if (iValue < iBestValue)
				{
					pBestCity = pLoopCity;
					iBestValue = iValue;
				}
			}

			// R&R, ray, Great Admirals - START
			// has been modified
			if (pBestCity)
			{
				UnitTypes eGeneralUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_GREAT_GENERAL"));
				if (eGeneralUnit != NO_UNIT)
				{
					if (GC.getUnitInfo(eGeneralUnit).getLeaderExperience() > 0 || GC.getUnitInfo(eGeneralUnit).getLeaderPromotion() != NO_PROMOTION)
					{
						pBestCity->createGreatGeneral(eGeneralUnit, true);
						setCombatExperience(getCombatExperience() - iExperienceThreshold);
					}
				}	
			}
			// R&R, ray, Great Admirals - END
		}
	}
}

void CvPlayer::changeCombatExperience(int iChange)
{
	setCombatExperience(getCombatExperience() + iChange);

	for (int i = 0; i <  GC.getNumFatherPointInfos(); ++i)
	{
		FatherPointTypes ePointType = (FatherPointTypes) i;
        int gameSpeedMod =  GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();

		changeFatherPoints(ePointType, iChange * GC.getFatherPointInfo(ePointType).getExperiencePoints() *gameSpeedMod/100);
	}
}

// R&R, ray, Great Admirals - START
int CvPlayer::getSeaCombatExperience() const
{
	return m_iSeaCombatExperience;
}

void CvPlayer::setSeaCombatExperience(int iExperience)
{
	FAssert(iExperience >= 0);

	if (iExperience != getSeaCombatExperience())
	{
		m_iSeaCombatExperience = iExperience;

		int iExperienceThreshold = greatAdmiralThreshold();
		if (getSeaCombatExperience() >= iExperienceThreshold && iExperienceThreshold > 0)
		{
			// create great person
			CvCity* pBestCity = NULL;
			int iBestValue = MAX_INT;
			int iLoop;
			for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				int iValue = 4 * GC.getGameINLINE().getSorenRandNum(getNumCities(), "Warlord City Selection");

				for (int i = 0; i < NUM_YIELD_TYPES; i++)
				{
					iValue += pLoopCity->findYieldRateRank((YieldTypes)i);
				}
				iValue += pLoopCity->findPopulationRank();

				if (iValue < iBestValue)
				{
					pBestCity = pLoopCity;
					iBestValue = iValue;
				}
			}

			if (pBestCity)
			{				
				UnitTypes eAdmiralUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_GREAT_ADMIRAL"));
				if (eAdmiralUnit != NO_UNIT)
				{
					if (GC.getUnitInfo(eAdmiralUnit).getLeaderExperience() > 0 || GC.getUnitInfo(eAdmiralUnit).getLeaderPromotion() != NO_PROMOTION)
					{
						pBestCity->createGreatAdmiral(eAdmiralUnit, true);
						setSeaCombatExperience(getSeaCombatExperience() - iExperienceThreshold);
					}
				}
			}
		}
	}
}

void CvPlayer::changeSeaCombatExperience(int iChange)
{
	setSeaCombatExperience(getSeaCombatExperience() + iChange);

	for (int i = 0; i <  GC.getNumFatherPointInfos(); ++i)
	{
		FatherPointTypes ePointType = (FatherPointTypes) i;
		int gameSpeedMod =  GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
		int pointsGained = iChange * GC.getFatherPointInfo(ePointType).getExperiencePoints()*gameSpeedMod /100;
		changeFatherPoints(ePointType, pointsGained);
	}
}
// R&R, ray, Great Admirals - END

bool CvPlayer::isConnected() const
{
	return gDLL->isConnected( getNetID() );
}

int CvPlayer::getNetID() const
{
	return GC.getInitCore().getNetID(getID());
}


void CvPlayer::setNetID(int iNetID)
{
	GC.getInitCore().setNetID(getID(), iNetID);
}

void CvPlayer::sendReminder()
{
	CvWString szYearStr;

	// Only perform this step if we have a valid email address on record,
	// and we have provided information about how to send emails
	if ( !getPbemEmailAddress().empty() &&
		!gDLL->GetPitbossSmtpHost().empty() )
	{
		GAMETEXT.setTimeStr(szYearStr, GC.getGameINLINE().getGameTurn(), true);

		// Generate our arguments
		CyArgsList argsList;
		argsList.add(getPbemEmailAddress());
		argsList.add(gDLL->GetPitbossSmtpHost());
		argsList.add(gDLL->GetPitbossSmtpLogin());
		argsList.add(gDLL->GetPitbossSmtpPassword());
		argsList.add(GC.getGameINLINE().getName());
		argsList.add(GC.getGameINLINE().isMPOption(MPOPTION_TURN_TIMER));
		argsList.add(GC.getGameINLINE().getPitbossTurnTime());
		argsList.add(gDLL->GetPitbossEmail());
		argsList.add(szYearStr);

		// Now send our email via Python
		long iResult;
		bool bOK = gDLL->getPythonIFace()->callFunction(PYPitBossModule, "sendEmail", argsList.makeFunctionArgs(), &iResult);

		FAssertMsg( bOK, "Pitboss Python call to onSendEmail failed!" );
		FAssertMsg( iResult == 0, "Pitboss Python fn onSendEmail encountered an error" );
	}
}

uint CvPlayer::getStartTime() const
{
	return m_uiStartTime;
}


void CvPlayer::setStartTime(uint uiStartTime)
{
	m_uiStartTime = uiStartTime;
}


uint CvPlayer::getTotalTimePlayed() const
{
	return ((timeGetTime() - m_uiStartTime)/1000);
}


bool CvPlayer::isAlive() const
{
	return m_bAlive;
}

// R&R, ray, Bargaining - Start
bool CvPlayer::isWillingToBargain() const
{
	return m_bWillingToBargain;
}

void CvPlayer::setWillingToBargain(bool bNewValue)
{
	m_bWillingToBargain = bNewValue;
}

int CvPlayer::getTimeNoTrade() const
{
	 return m_iTimeNoTrade;
}

void CvPlayer::setTimeNoTrade(int bNewValue)
{
	m_iTimeNoTrade = bNewValue;
}

void CvPlayer::decreaseTimeNoTrade()
{
	if(m_iTimeNoTrade > 0)
	{
		m_iTimeNoTrade = (m_iTimeNoTrade - 1);
	}
	return ;
}
// R&R, ray, Bargaining - END

bool CvPlayer::isEverAlive() const
{
	return m_bEverAlive;
}


void CvPlayer::setAlive(bool bNewValue)
{
	CvWString szBuffer;
	int iI;

	if (isAlive() != bNewValue)
	{
		m_bAlive = bNewValue;

		GET_TEAM(getTeam()).changeAliveCount((isAlive()) ? 1 : -1);

		// Report event to Python
		gDLL->getEventReporterIFace()->setPlayerAlive(getID(), bNewValue);

		if (isAlive())
		{
			if (!isEverAlive())
			{
				m_bEverAlive = true;

				GET_TEAM(getTeam()).changeEverAliveCount(1);
			}

			if (getNumCities() == 0)
			{
				setFoundedFirstCity(false);
			}

			if (GC.getGameINLINE().isMPOption(MPOPTION_SIMULTANEOUS_TURNS) || (GC.getGameINLINE().getNumGameTurnActive() == 0) || (GC.getGameINLINE().isSimultaneousTeamTurns() && GET_TEAM(getTeam()).isTurnActive()))
			{
				setTurnActive(true);
			}

			gDLL->openSlot(getID());
		}
		else
		{
			killUnits();
			killCities();
			killAllDeals();

			// R&R, making peace with all factions, when dying - START
			if(!isEurope() && isEverAlive())
			{
				CvTeamAI& ownTeam= GET_TEAM(getTeam());

				for (int iTeam = 0; iTeam < MAX_TEAMS; ++iTeam)
				{				
					if (ownTeam.isAtWar((TeamTypes)iTeam))
					{
						if (ownTeam.canChangeWarPeace((TeamTypes)iTeam))
						{
							ownTeam.makePeace((TeamTypes) iTeam);
						}
					}		
				}
			}
			// R&R, making peace with all factions, when dying - END

			setTurnActive(false);

			gDLL->endMPDiplomacy();
			gDLL->endDiplomacy();

			if (!isHuman())
			{
				gDLL->closeSlot(getID());
			}

			if (!GC.getGameINLINE().isGameMultiPlayer())
			{
				if (GC.getGameINLINE().getActivePlayer() == getID() && GC.getGameINLINE().getAIAutoPlay() <= 0 && !gDLL->GetAutorun())
				{
					GC.getGameINLINE().setGameState(GAMESTATE_OVER);
				}
			}

			if (GC.getGameINLINE().getElapsedGameTurns() > 0)
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_CIV_DESTROYED", getCivilizationAdjectiveKey());

				for (iI = 0; iI < MAX_PLAYERS; iI++)
				{
					if (GET_PLAYER((PlayerTypes)iI).isAlive())
					{
						gDLL->getInterfaceIFace()->addMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVDESTROYED", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));
					}
				}

				GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szBuffer, -1, -1, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));
			}
		}

		GC.getGameINLINE().setScoreDirty(true);
	}
}


void CvPlayer::verifyAlive()
{
	bool bKill;
	bool bRespawnDeactivated;

	if (isAlive())
	{
		bKill = false;
		bRespawnDeactivated = false;

		if (!bKill)
		{
			if (getNumCities() == 0 && !isEurope() && getAdvancedStartPoints() < 0)
			{
				if (isNative() || getNumUnits() == 0)
				{
					bKill = true;
				}

				//WTP, ray, make AI elimination threshold XML configurable and also adjust to Gamespeed
				int iMinTurnForAIRespawningOff = GC.getDefineINT("KI_RESPAWN_OFF_MIN_TURN");
				int gameSpeedMod =  GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
				iMinTurnForAIRespawningOff = iMinTurnForAIRespawningOff * gameSpeedMod /100;

				// TAC - RESPAWN Option - Ray - Start
				// WTP, ray, fix for Colonial AI not being possible to eliminate a Colonial AI - Check for "No more Settler" removed because causing Problems with Settler is Europe
				if (!isNative() && !isHuman() && GC.getDefineINT("KI_RESPAWN_OFF") == 1 && GC.getGameINLINE().getGameTurn() > iMinTurnForAIRespawningOff && (getNumUnits() < 5 || AI_getNumAIUnits(UNITAI_TRANSPORT_SEA) == 0))
				{
					bKill = true;
					bRespawnDeactivated = true;
				}
				// TAC - RESPAWN Option - Ray - End
			}
		}

		//WTP, ray, Settler Professsion - START
		//only if player is still alive
		if (!bKill && !bRespawnDeactivated)
		{
			// Only Colonial Players, but not Kings, not Natives, not Animals, ...
			if (getParent() != NO_PLAYER)
			{
				// we check that there is no War of Independence and other small things
				CvPlayer& kEurope = GET_PLAYER(getParent());
				if(kEurope.isAlive() && kEurope.isEurope() && !::atWar(getTeam(), kEurope.getTeam()) && (GC.getGameINLINE().getAIAutoPlay() == 0 || GC.getGameINLINE().getActivePlayer() != getID()))
				{
					//we check if Player needs a new settler, because he lost all Cities and Units in Settler Profession
					int iNumPlayerCities = getNumCities();
					int iNumSettlers = AI_getNumAIUnits(UNITAI_SETTLER);

					//we also check if AI is landlocked and needs more cities
					CvPlayerAI& kPlayer = GET_PLAYER(getID());
					int iAIdesiredCities = kPlayer.AI_desiredCityCount();

					// no cities and no settlers check
					if (iNumPlayerCities == 0 && iNumSettlers == 0)
					{
						//for Human, we just init a Settler in Europe
						if(isHuman())
						{
							//create a Unit in Profession Settler in Europe - for free, but with tax increase
							initEuropeSettler(false);

							//sending a text message for King granting a Settler
							CvWString szBuffer = gDLL->getText("TXT_KEY_NO_MORE_SETTLER", getCivilizationShortDescriptionKey());
							gDLL->getInterfaceIFace()->addMessage((getID()), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));

							// King increases tax rate
							changeTaxRate(1);
						}
						// for AI we init Starting Units - because of Problems to transport Settlers from Europe
						else
						{
							//spawn Starting Units
							initFreeUnits();
							// King increases tax rate
							changeTaxRate(1);
						}
					}

					// ONLY for AI: landlock check - not necessary anymore if we already spawned in first if
					else if (!isHuman() && (iAIdesiredCities > (iNumPlayerCities + iNumSettlers)))
					{
						if (kPlayer.AI_isLandLocked())
						{
							//create a Unit in Profession Settler in Europe - having to pay equipment
							initEuropeSettler(true);
						}
					}

				}
			}
		}
		//WTP, ray, Settler Professsion - END

		if (bKill)
		{
			//don't kill colonist players
			// TAC - RESPAWN Option - Ray - Start -> Logik changed
			if(getParent() != NO_PLAYER)
			{
				CvPlayer& kEurope = GET_PLAYER(getParent());
				if(kEurope.isAlive() && kEurope.isEurope() && !::atWar(getTeam(), kEurope.getTeam()) && (GC.getGameINLINE().getAIAutoPlay() == 0 || GC.getGameINLINE().getActivePlayer() != getID()))
				{

					// TAC - RESPAWN Option - Ray - Start
					if(bRespawnDeactivated)
					{
						// When a player is killed, the game deletes all cities and units.
						// If a unit is removed while being in combat, the combat can't finish and the game freezes.
						int iLoop;
						for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
						{
							if (pLoopUnit->isCombat())
							{
								// A unit is in combat
								// Delay killing the player until the combat animation is over
								return;
							}
						}

						//setAlive(false);
						bKill = true;
						CvWString szBuffer = gDLL->getText("TXT_KEY_NO_MORE_RESPAWN", getCivilizationShortDescriptionKey());

						for (int iI = 0; iI < MAX_PLAYERS; iI++)
						{
							if (GET_PLAYER((PlayerTypes)iI).isAlive())
							{
								gDLL->getInterfaceIFace()->addMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVDESTROYED", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
							}
						}

						//koma13
						//destroying Missions of that player
						// WTP, ray, Native Trade Posts - START
						// now also destroying Trade Posts of the player
						for (int iI = 0; iI < MAX_PLAYERS; iI++)
						{
							if (GET_PLAYER((PlayerTypes)iI).isAlive())
							{
								CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes) iI);
								if (kLoopPlayer.isNative()) 
								{
									int iLoop;
									for (CvCity* pLoopCity = kLoopPlayer.firstCity(&iLoop); NULL != pLoopCity; pLoopCity = kLoopPlayer.nextCity(&iLoop))
									{
										if (pLoopCity->getMissionaryPlayer() == getID())
										{
											pLoopCity->setMissionaryPlayer(NO_PLAYER);
											pLoopCity->setMissionaryRate(0);
										}

										// WTP, ray, Native Trade Posts - START
										if (pLoopCity->getTradePostPlayer() == getID())
										{
											pLoopCity->setTradePostPlayer(NO_PLAYER);
											pLoopCity->setNativeTradeRate(0);
											pLoopCity->setNativeTradePostGold(0);
										}
									}
								}
							}
						}
						//end

					}
					//Alter Code in else
					else
					{
						initFreeUnits();


						if (isTurnActive())
						{
							setEndTurn(true);
						}

						//change taxrate
						int iOldTaxRate = getTaxRate();

						/** NBMOD TAX **/
						/** Original
						int iNewTaxRate = std::min(99, iOldTaxRate + 1 + GC.getGameINLINE().getSorenRandNum(GC.getDefineINT("TAX_RATE_MAX_INCREASE"), "Tax Rate Increase"));
						int iChange = iNewTaxRate - iOldTaxRate;
						changeTaxRate(iChange);
						**/

						int iNewTaxRate = NBMOD_GetNewTaxRate(std::min(99, iOldTaxRate + 1 + GC.getGameINLINE().getSorenRandNum(GC.getDefineINT("TAX_RATE_MAX_INCREASE"), "Tax Rate Increase")));
						int iChange = iNewTaxRate - iOldTaxRate;

						if (iChange > 0)
						{
						    changeTaxRate(iChange);
						}

						/** NBMOD TAX **/

						if (isHuman())
						{
							CvDiploParameters* pDiplo = new CvDiploParameters(kEurope.getID());
							pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_KING_REVIVE"));
							pDiplo->addDiploCommentVariable(iNewTaxRate);
							pDiplo->setAIContact(true);
							gDLL->beginDiplomacy(pDiplo, getID());
						}

						bKill = false;
					}
					// TAC - RESPAWN Option - Ray - End
				}
			}
		}

		if (!bKill)
		{
			if (!isNative() && !isEurope())
			{
				if (GC.getGameINLINE().getMaxCityElimination() > 0)
				{
					if (getCitiesLost() >= GC.getGameINLINE().getMaxCityElimination())
					{
						bKill = true;
					}
				}
			}
		}

		if (bKill)
		{
			// When a player is killed, the game deletes all cities and units.
			// If a unit is removed while being in combat, the combat can't finish and the game freezes.
			int iLoop;
			for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
			{
				if (pLoopUnit->isCombat())
				{
					// A unit is in combat
					// Delay killing the player until the combat animation is over
					return;
				}
			}
			setAlive(false);
		}
	}
	else
	{
		if ((getNumCities() > 0) || (getNumUnits() > 0))
		{
			setAlive(true);
		}
	}
}

bool CvPlayer::isTurnActive() const
{
	return m_bTurnActive;
}

void CvPlayer::setTurnActiveForPbem(bool bActive)
{
	FAssertMsg(GC.getGameINLINE().isPbem(), "You are using setTurnActiveForPbem. Are you sure you know what you're doing?");

	// does nothing more than to set the member variable before saving the game
	// the rest of the turn will be performed upon loading the game
	// This allows the player to browse the game in paused mode after he has generated the save
	if (isTurnActive() != bActive)
	{
		m_bTurnActive = bActive;
		GC.getGameINLINE().changeNumGameTurnActive(isTurnActive() ? 1 : -1);
	}
}


void CvPlayer::setTurnActive(bool bNewValue, bool bDoTurn)
{
	if (isTurnActive() != bNewValue)
	{
		m_bTurnActive = bNewValue;

		if (isTurnActive())
		{
			if (GC.getLogging())
			{
				if (gDLL->getChtLvl() > 0)
				{
					TCHAR szOut[1024];
					sprintf(szOut, "Player %d Turn ON\n", getID());
					gDLL->messageControlLog(szOut);
				}
			}

			FAssertMsg(isAlive(), "isAlive is expected to be true");

			setEndTurn(false);
			GC.getGameINLINE().resetTurnTimer();

			// If we are the Pitboss, send this player an email
			if ( gDLL->IsPitbossHost() )
			{
				// If this guy is not currently connected, try sending him an email
				if ( isHuman() && !isConnected() )
				{
					sendReminder();
				}
			}

			if ((GC.getGameINLINE().isHotSeat() || GC.getGameINLINE().isPbem()) && isHuman() && bDoTurn)
			{
				gDLL->getInterfaceIFace()->clearEventMessages();
				gDLL->getEngineIFace()->setResourceLayer(false);

				GC.getGameINLINE().setActivePlayer(getID());
			}

			GC.getGameINLINE().changeNumGameTurnActive(1);

			if (bDoTurn)
			{
				if (isAlive() && !isHuman() && (getAdvancedStartPoints() >= 0))
				{
					AI_doAdvancedStart();
				}

				if (GC.getGameINLINE().getElapsedGameTurns() > 0)
				{
					if (isAlive())
					{
						if (GC.getGameINLINE().isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
						{
							doTurn();
						}

						doTurnUnits();
					}
				}

				if ((getID() == GC.getGameINLINE().getActivePlayer()) && (GC.getGameINLINE().getElapsedGameTurns() > 0))
				{
					if (GC.getGameINLINE().isNetworkMultiPlayer())
					{
						gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_MISC_TURN_BEGINS").GetCString(), "AS2D_NEWTURN", MESSAGE_TYPE_DISPLAY_ONLY);
					}
					else
					{
						gDLL->getInterfaceIFace()->playGeneralSound("AS2D_NEWTURN");
					}
				}

				doWarnings();
			}

			if (getID() == GC.getGameINLINE().getActivePlayer())
			{
				if (gDLL->getInterfaceIFace()->getLengthSelectionList() == 0)
				{
					gDLL->getInterfaceIFace()->setCycleSelectionCounter(1);
				}

				gDLL->getInterfaceIFace()->setDirty(SelectionCamera_DIRTY_BIT, true);
			}
		}
		else
		{
			if (GC.getLogging())
			{
				if (gDLL->getChtLvl() > 0)
				{
					TCHAR szOut[1024];
					sprintf(szOut, "Player %d Turn OFF\n", getID());
					gDLL->messageControlLog(szOut);
				}
			}

			if (getID() == GC.getGameINLINE().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setForcePopup(false);
				gDLL->getInterfaceIFace()->clearQueuedPopups();
				gDLL->getInterfaceIFace()->flushTalkingHeadMessages();
			}

			GC.getGameINLINE().changeNumGameTurnActive(-1);

			if (bDoTurn)
			{
				if (!GC.getGameINLINE().isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
				{
					if (isAlive())
					{
						doTurn();
					}

					if ((GC.getGameINLINE().isPbem() || GC.getGameINLINE().isHotSeat()) && isHuman() && GC.getGameINLINE().countHumanPlayersAlive() > 1)
					{
						GC.getGameINLINE().setHotPbemBetweenTurns(true);
					}

					if (GC.getGameINLINE().isSimultaneousTeamTurns())
					{
						if (!GET_TEAM(getTeam()).isTurnActive())
						{
							for (int iI = (getTeam() + 1); iI < MAX_TEAMS; iI++)
							{
								if (GET_TEAM((TeamTypes)iI).isAlive())
								{
									GET_TEAM((TeamTypes)iI).setTurnActive(true);
									break;
								}
							}
						}
					}
					else
					{
						for (int iI = (getID() + 1); iI < MAX_PLAYERS; iI++)
						{
							if (GET_PLAYER((PlayerTypes)iI).isAlive())
							{
								if (GC.getGameINLINE().isPbem() && GET_PLAYER((PlayerTypes)iI).isHuman())
								{
									if (!GC.getGameINLINE().getPbemTurnSent())
									{
										gDLL->sendPbemTurn((PlayerTypes)iI);
									}
								}
								else
								{
									GET_PLAYER((PlayerTypes)iI).setTurnActive(true);
								}
								break;
							}
						}
					}
				}
			}
		}

		gDLL->getInterfaceIFace()->updateCursorType();

		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	}
}


bool CvPlayer::isAutoMoves() const
{
	return m_bAutoMoves;
}


void CvPlayer::setAutoMoves(bool bNewValue)
{
	MOD_PROFILE("CvPlayer::setAutoMoves");

	if (isAutoMoves() != bNewValue)
	{
		m_bAutoMoves = bNewValue;

		if (!isAutoMoves())
		{
			if (isEndTurn() || !isHuman())
			{
				setTurnActive(false);
			}
			else
			{
				if (getID() == GC.getGameINLINE().getActivePlayer())
				{
					gDLL->getInterfaceIFace()->setCycleSelectionCounter(1);
				}
			}
		}
	}
}


bool CvPlayer::isEndTurn() const
{
	return m_bEndTurn;
}


void CvPlayer::setEndTurn(bool bNewValue)
{
	if (isEndTurn() != bNewValue)
	{
		FAssertMsg(isTurnActive(), "isTurnActive is expected to be true");

		m_bEndTurn = bNewValue;

		if (isEndTurn())
		{
			setAutoMoves(true);
		}
	}
}

bool CvPlayer::isTurnDone() const
{
	// if this returns true, popups and diplomacy will wait to appear until next turn
	if (!GC.getGameINLINE().isPbem() && !GC.getGameINLINE().isHotSeat())
	{
		return false;
	}
	if (!isHuman() )
	{
		return true;
	}
	if (!isEndTurn())
	{
		return false;
	}
	return (!isAutoMoves());
}

bool CvPlayer::isExtendedGame() const
{
	return m_bExtendedGame;
}


void CvPlayer::makeExtendedGame()
{
	m_bExtendedGame = true;
}


bool CvPlayer::isFoundedFirstCity() const
{
	return m_bFoundedFirstCity;
}


void CvPlayer::setFoundedFirstCity(bool bNewValue)
{
	if (isFoundedFirstCity() != bNewValue)
	{
		m_bFoundedFirstCity = bNewValue;
	}
}

PlayerTypes CvPlayer::getID() const
{
	return m_eID;
}


HandicapTypes CvPlayer::getHandicapType() const
{
	return GC.getInitCore().getHandicap(getID());
}


CivilizationTypes CvPlayer::getCivilizationType() const
{
	return GC.getInitCore().getCiv(getID());
}


LeaderHeadTypes CvPlayer::getLeaderType() const
{
	return GC.getInitCore().getLeader(getID());
}


LeaderHeadTypes CvPlayer::getPersonalityType() const
{
	return m_ePersonalityType;
}


void CvPlayer::setPersonalityType(LeaderHeadTypes eNewValue)
{
	m_ePersonalityType = eNewValue;
}


EraTypes CvPlayer::getCurrentEra() const
{
	return m_eCurrentEra;
}



void CvPlayer::setCurrentEra(EraTypes eNewValue)
{
	CvCity* pLoopCity;
	CvPlot* pLoopPlot;
	int iLoop;
	int iI;

	if (getCurrentEra() != eNewValue)
	{
		EraTypes eOldEra = m_eCurrentEra;
		m_eCurrentEra = eNewValue;

		// apply CivEffects
		for (EraTypes eEra = static_cast<EraTypes>(eOldEra + 1); eEra <= eNewValue; ++eEra)
		{
			if (eEra >= FIRST_ERA && eEra < NUM_ERA_TYPES)
			{
				CivEffect()->applyCivEffect(GC.getEraInfo(eEra).getCivEffect());
			}
		}

		// remove era CivEffects in case the player goes back in eras
		// likely not needed, but it's better to be safe than sorry
		for (EraTypes eEra = static_cast<EraTypes>(eNewValue + 1); eEra <= eOldEra; ++eEra)
		{
			if (eEra >= FIRST_ERA && eEra < NUM_ERA_TYPES)
			{
				CivEffect()->applyCivEffect(GC.getEraInfo(eEra).getCivEffect(), - 1);
			}
		}

		if (GC.getGameINLINE().getActiveTeam() != NO_TEAM)
		{
			for (iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
			{
				pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
				if (pLoopPlot->getRevealedImprovementType(GC.getGameINLINE().getActiveTeam(), true) != NO_IMPROVEMENT)
				{
					if ((pLoopPlot->getOwnerINLINE() == getID()) || (!(pLoopPlot->isOwned()) && (getID() == GC.getGameINLINE().getActivePlayer())))
					{
						pLoopPlot->setLayoutDirty(true);
					}
				}
			}
		}

		// dirty all of this player's cities...
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			// transport feeder - start - Nightinggale
			pLoopCity->setAutoThresholdCache();
			// transport feeder - end - Nightinggale
			if (pLoopCity->getOwnerINLINE() == getID())
			{
				pLoopCity->setLayoutDirty(true);
			}
		}

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(Soundtrack_DIRTY_BIT, true);
		}
	}
}


PlayerTypes CvPlayer::getParent() const
{
	return m_eParent;
}

void CvPlayer::setParent(PlayerTypes eParent)
{
	m_eParent = eParent;
}

TeamTypes CvPlayer::getTeam() const
{
	return m_eTeamType;
}

void CvPlayer::updateTeamType()
{
	if(getID() == NO_PLAYER)
	{
		m_eTeamType = NO_TEAM;
	}
	else
	{
		m_eTeamType = GC.getInitCore().getTeam(getID());
	}
}

void CvPlayer::setTeam(TeamTypes eTeam)
{
	FAssert(eTeam != NO_TEAM);
	FAssert(getTeam() != NO_TEAM);

	GET_TEAM(getTeam()).changeNumMembers(-1);
	if (isAlive())
	{
		GET_TEAM(getTeam()).changeAliveCount(-1);
	}
	if (isEverAlive())
	{
		GET_TEAM(getTeam()).changeEverAliveCount(-1);
	}
	GET_TEAM(getTeam()).changeNumCities(-(getNumCities()));
	GET_TEAM(getTeam()).changeTotalLand(-(getTotalLand()));

	GC.getInitCore().setTeam(getID(), eTeam);

	GET_TEAM(getTeam()).changeNumMembers(1);
	if (isAlive())
	{
		GET_TEAM(getTeam()).changeAliveCount(1);
	}
	if (isEverAlive())
	{
		GET_TEAM(getTeam()).changeEverAliveCount(1);
	}
	GET_TEAM(getTeam()).changeNumCities(getNumCities());
	GET_TEAM(getTeam()).changeTotalLand(getTotalLand());
}

YieldTypes CvPlayer::getImmigrationConversion() const
{
	return m_eImmigrationConversion;
}

void CvPlayer::setImmigrationConversion(YieldTypes eConversion)
{
	m_eImmigrationConversion = eConversion;
}

PlayerColorTypes CvPlayer::getPlayerColor() const
{
	return GC.getInitCore().getColor(getID());
}

int CvPlayer::getPlayerTextColorR() const
{
	FAssertMsg(getPlayerColor() != NO_PLAYERCOLOR, "getPlayerColor() is not expected to be equal with NO_PLAYERCOLOR");
	return ((int)(GC.getColorInfo((ColorTypes) GC.getPlayerColorInfo(getPlayerColor()).getTextColorType()).getColor().r * 255));
}

int CvPlayer::getPlayerTextColorG() const
{
	FAssertMsg(getPlayerColor() != NO_PLAYERCOLOR, "getPlayerColor() is not expected to be equal with NO_PLAYERCOLOR");
	return ((int)(GC.getColorInfo((ColorTypes) GC.getPlayerColorInfo(getPlayerColor()).getTextColorType()).getColor().g * 255));
}

int CvPlayer::getPlayerTextColorB() const
{
	FAssertMsg(getPlayerColor() != NO_PLAYERCOLOR, "getPlayerColor() is not expected to be equal with NO_PLAYERCOLOR");
	return ((int)(GC.getColorInfo((ColorTypes) GC.getPlayerColorInfo(getPlayerColor()).getTextColorType()).getColor().b * 255));
}

int CvPlayer::getPlayerTextColorA() const
{
	FAssertMsg(getPlayerColor() != NO_PLAYERCOLOR, "getPlayerColor() is not expected to be equal with NO_PLAYERCOLOR");
	return ((int)(GC.getColorInfo((ColorTypes) GC.getPlayerColorInfo(getPlayerColor()).getTextColorType()).getColor().a * 255));
}

int CvPlayer::getSeaPlotYield(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_iSeaPlotYield.get(eIndex);
}

void CvPlayer::changeSeaPlotYield(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_em_iSeaPlotYield.add(eIndex, iChange);

		updateYield();
	}
}

int CvPlayer::getYieldRateModifier(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_iYieldRateModifier.get(eIndex);
}

int CvPlayer::getTaxYieldRateModifier(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (getTaxYieldModifierCount(eIndex) > 0)
	{
		return getTaxRate();
	}

	return 0;
}

void CvPlayer::changeYieldRateModifier(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_em_iYieldRateModifier.add(eIndex, iChange);

		invalidateYieldRankCache(eIndex);

		AI_makeAssignWorkDirty();

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getCapitalYieldRateModifier(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_iCapitalYieldRateModifier.get(eIndex);
}


void CvPlayer::changeCapitalYieldRateModifier(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_em_iCapitalYieldRateModifier.add(eIndex, iChange);

		invalidateYieldRankCache(eIndex);

		CvCity* pCapitalCity = getCapitalCity();
		if (pCapitalCity != NULL)
		{
			pCapitalCity->AI_setAssignWorkDirty(true);
			if (pCapitalCity->getTeam() == GC.getGameINLINE().getActiveTeam())
			{
				pCapitalCity->setBillboardDirty(true);
			}
		}
	}
}

int CvPlayer::getBuildingRequiredYieldModifier(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_iBuildingRequiredYieldModifier.get(eIndex);
}


void CvPlayer::changeBuildingRequiredYieldModifier(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_em_iBuildingRequiredYieldModifier.add(eIndex, iChange);
		// transport feeder - start - Nightinggale
		this->updateTransportThreshold(eIndex);
		// transport feeder - end - Nightinggale
	}
}

int CvPlayer::getCityExtraYield(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_iCityExtraYield.get(eIndex);
}

void CvPlayer::updateCityExtraYield(YieldTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	int iBestValue = 0;
	for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			iBestValue += GC.getTraitInfo((TraitTypes) iI).getCityExtraYield(eIndex);
		}
	}

	if (getCityExtraYield(eIndex) != iBestValue)
	{
		m_em_iCityExtraYield.set(eIndex, iBestValue);
		FAssert(getCityExtraYield(eIndex) >= 0);

		updateYield();
	}
}


int CvPlayer::getExtraYieldThreshold(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_iExtraYieldThreshold.get(eIndex);
}


void CvPlayer::updateExtraYieldThreshold(YieldTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	int iBestValue = 0;

	for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			if (GC.getTraitInfo((TraitTypes) iI).getExtraYieldThreshold(eIndex) > 0)
			{
				if ((iBestValue == 0) || (GC.getTraitInfo((TraitTypes) iI).getExtraYieldThreshold(eIndex) < iBestValue))
				{
					iBestValue = GC.getTraitInfo((TraitTypes) iI).getExtraYieldThreshold(eIndex);
				}
			}
		}
	}

	if (getExtraYieldThreshold(eIndex) != iBestValue)
	{
		m_em_iExtraYieldThreshold.set(eIndex, iBestValue);
		FAssert(getExtraYieldThreshold(eIndex) >= 0);

		updateYield();
	}
}


int CvPlayer::getYieldRate(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (getNumCities() == 0)
	{
		return 0;
	}

	int iTotalRate = GC.getCivilizationInfo(getCivilizationType()).getFreeYields(eIndex);
	int iLoop;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iTotalRate += pLoopCity->calculateNetYield(eIndex);
	}

	return iTotalRate;
}

// WTP, ray, Happiness - START
int CvPlayer::getHappinessRate() const
{
	if (getNumCities() == 0)
	{
		return 0;
	}

	int iTotalRate = GC.getCivilizationInfo(getCivilizationType()).getFreeYields(YIELD_HAPPINESS);
	int iLoop;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iTotalRate += pLoopCity->getCityHappiness();
	}

	iTotalRate = iTotalRate / getNumCities(); // we calculate the average because it is supposed to be a percentage later

	return iTotalRate;
}

int CvPlayer::getUnHappinessRate() const
{
	if (getNumCities() == 0)
	{
		return 0;
	}

	// small AI cheat to prevent issues 
	if (!isHuman())
	{
		return 0;
	}

	int iTotalRate = GC.getCivilizationInfo(getCivilizationType()).getFreeYields(YIELD_UNHAPPINESS);
	int iLoop;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iTotalRate += pLoopCity->getCityUnHappiness();
	}

	iTotalRate = iTotalRate / getNumCities(); // we calculate the average because it is supposed to be a percentage later

	return iTotalRate;
}
// WTP, ray, Happiness - END

bool CvPlayer::isYieldEuropeTradable(YieldTypes eYield) const
{
	FAssert(eYield >= 0 && eYield < NUM_YIELD_TYPES);

	if (getParent() == NO_PLAYER)
	{
		return false;
	}

	if (!GC.getYieldInfo(eYield).isCargo())
	{
		return false;
	}

	if (!canTradeWithEurope())
	{
		return false;
	}

	if (GET_PLAYER(getParent()).getYieldBuyPrice(eYield) <= 0)
	{
		return false;
	}

	return m_em_bYieldEuropeTradable.get(eYield);
}

void CvPlayer::setYieldEuropeTradable(YieldTypes eYield, bool bTradeable)
{
	FAssert(eYield >= 0 && eYield < NUM_YIELD_TYPES);

	bool bOldTradeable = isYieldEuropeTradable(eYield);

	m_em_bYieldEuropeTradable.set(eYield, bTradeable);

	if (bOldTradeable != isYieldEuropeTradable(eYield))
	{
		validateTradeRoutes();
	}
}

void CvPlayer::setYieldEuropeTradableAll()
{
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		setYieldEuropeTradable((YieldTypes)iI, true);
	}
}

bool CvPlayer::isFeatAccomplished(FeatTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_FEAT_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_bFeatAccomplished.get(eIndex);
}


void CvPlayer::setFeatAccomplished(FeatTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_FEAT_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_em_bFeatAccomplished.set(eIndex, bNewValue);
}

bool CvPlayer::shouldDisplayFeatPopup(FeatTypes eIndex) const
{
	if (isFeatAccomplished(eIndex))
	{
		return false;
	}

	if (GC.getGameINLINE().isNetworkMultiPlayer())
	{
		return false;
	}

	if (!isOption(PLAYEROPTION_ADVISOR_POPUPS))
	{
		return false;
	}

	if (!isHuman())
	{
		return false;
	}

	if (GC.getGameINLINE().getElapsedGameTurns() == 0)
	{
		return false;
	}

	return true;
}

bool CvPlayer::isOption(PlayerOptionTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_PLAYEROPTION_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_bOptions.get(eIndex);
}


void CvPlayer::setOption(PlayerOptionTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_PLAYEROPTION_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_em_bOptions.set(eIndex, bNewValue);
}

bool CvPlayer::isPlayable() const
{
	return GC.getInitCore().getPlayableCiv(getID());
}

void CvPlayer::setPlayable(bool bNewValue)
{
	GC.getInitCore().setPlayableCiv(getID(), bNewValue);
}

int CvPlayer::getImprovementCount(ImprovementTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumImprovementInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_iImprovementCount.get(eIndex);
}


void CvPlayer::changeImprovementCount(ImprovementTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumImprovementInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_em_iImprovementCount.add(eIndex, iChange);
	FAssert(getImprovementCount(eIndex) >= 0);
}


int CvPlayer::getFreeBuildingCount(BuildingTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_iFreeBuildingCount.get(eIndex);
}


bool CvPlayer::isBuildingFree(BuildingTypes eIndex)	const
{
	return (getFreeBuildingCount(eIndex) > 0);
}


void CvPlayer::changeFreeBuildingCount(BuildingTypes eIndex, int iChange)
{
	CvCity* pLoopCity;
	int iOldFreeBuildingCount;
	int iLoop;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		iOldFreeBuildingCount = getFreeBuildingCount(eIndex);

		m_em_iFreeBuildingCount.add(eIndex, iChange);
		FAssert(getFreeBuildingCount(eIndex) >= 0);

		if (iOldFreeBuildingCount == 0)
		{
			FAssertMsg(getFreeBuildingCount(eIndex) > 0, "getFreeBuildingCount(eIndex) is expected to be greater than 0");

			for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				pLoopCity->setHasFreeBuilding(eIndex, true);
			}
		}
		else if (getFreeBuildingCount(eIndex) == 0)
		{
			FAssertMsg(iOldFreeBuildingCount > 0, "iOldFreeBuildingCount is expected to be greater than 0");

			for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				pLoopCity->setHasFreeBuilding(eIndex, false);
			}
		}
	}
}

int CvPlayer::getUnitClassCount(UnitClassTypes eIndex) const
{
	FAssert(eIndex >= 0 && eIndex < GC.getNumUnitClassInfos());
	return m_em_iUnitClassCount.get(eIndex);
}

void CvPlayer::changeUnitClassCount(UnitClassTypes eIndex, int iChange)
{
	FAssert(eIndex >= 0 && eIndex < GC.getNumUnitClassInfos());
	m_em_iUnitClassCount.add(eIndex, iChange);
	FAssert(getUnitClassCount(eIndex) >= 0);
}

int CvPlayer::getUnitClassMaking(UnitClassTypes eIndex) const
{
	FAssert(eIndex >= 0 && eIndex < GC.getNumUnitClassInfos());
	return m_em_iUnitClassMaking.get(eIndex);
}


void CvPlayer::changeUnitClassMaking(UnitClassTypes eIndex, int iChange)
{
	FAssert(eIndex >= 0 && eIndex < GC.getNumUnitClassInfos());

	if (iChange != 0)
	{
		m_em_iUnitClassMaking.add(eIndex, iChange);
		FAssert(getUnitClassMaking(eIndex) >= 0);

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(Help_DIRTY_BIT, true);
		}
	}
}

int CvPlayer::getUnitClassImmigrated(UnitClassTypes eIndex) const
{
	FAssert(eIndex >= 0 && eIndex < GC.getNumUnitClassInfos());
	return m_em_iUnitClassImmigrated.get(eIndex);
}

void CvPlayer::changeUnitClassImmigrated(UnitClassTypes eIndex, int iChange)
{
	FAssert(eIndex >= 0 && eIndex < GC.getNumUnitClassInfos());

	if (iChange != 0)
	{
		m_em_iUnitClassImmigrated.add(eIndex, iChange);
		FAssert(getUnitClassImmigrated(eIndex) >= 0);
	}
}

int CvPlayer::getUnitClassCountPlusMaking(UnitClassTypes eIndex) const
{
	return (getUnitClassCount(eIndex) + getUnitClassMaking(eIndex));
}

int CvPlayer::getUnitMoveChange(UnitClassTypes eIndex) const
{
	FAssert(eIndex >= 0 && eIndex < GC.getNumUnitClassInfos());
	return m_em_iUnitMoveChange.get(eIndex);
}

void CvPlayer::changeUnitMoveChange(UnitClassTypes eIndex, int iChange)
{
	FAssert(eIndex >= 0 && eIndex < GC.getNumUnitClassInfos());
	if(iChange != 0)
	{
		m_em_iUnitMoveChange.add(eIndex, iChange);
	}
}

int CvPlayer::getUnitStrengthModifier(UnitClassTypes eIndex) const
{
	FAssert(eIndex >= 0 && eIndex < GC.getNumUnitClassInfos());
	return m_em_iUnitStrengthModifier.get(eIndex);
}

void CvPlayer::changeUnitStrengthModifier(UnitClassTypes eIndex, int iChange)
{
	FAssert(eIndex >= 0 && eIndex < GC.getNumUnitClassInfos());
	if(iChange != 0)
	{
		m_em_iUnitStrengthModifier.add(eIndex, iChange);
	}
}

int CvPlayer::getProfessionMoveChange(ProfessionTypes eIndex) const
{
	FAssert(eIndex >= 0 && eIndex < GC.getNumProfessionInfos());
	return m_em_iProfessionMoveChange.get(eIndex);
}

void CvPlayer::changeProfessionMoveChange(ProfessionTypes eIndex, int iChange)
{
	FAssert(eIndex >= 0 && eIndex < GC.getNumProfessionInfos());
	if(iChange != 0)
	{
		m_em_iProfessionMoveChange.add(eIndex, iChange);
	}
}

int CvPlayer::getProfessionCombatChange(ProfessionTypes eIndex) const
{
	FAssert(eIndex >= 0 && eIndex < GC.getNumProfessionInfos());
	return m_em_iProfessionCombatChange.get(eIndex);
}

void CvPlayer::changeProfessionCombatChange(ProfessionTypes eIndex, int iChange)
{
	FAssert(eIndex >= 0 && eIndex < GC.getNumProfessionInfos());
	if(iChange != 0)
	{
		int iLoop;

		for (CvCity* pCity = firstCity(&iLoop); pCity != NULL; pCity = nextCity(&iLoop))
		{
			pCity->AI_setWorkforceHack(true);
		}

		for (CvUnit* pUnit = firstUnit(&iLoop); pUnit != NULL; pUnit = nextUnit(&iLoop))
		{
			pUnit->processProfession(pUnit->getProfession(), -1, false);
		}

		for (uint i = 0; i < m_aEuropeUnits.size(); ++i)
		{
			m_aEuropeUnits[i]->processProfession(m_aEuropeUnits[i]->getProfession(), -1, false);
		}

		for (CvCity* pCity = firstCity(&iLoop); pCity != NULL; pCity = nextCity(&iLoop))
		{
			for (int i = 0; i < pCity->getPopulation(); ++i)
			{
				CvUnit* pUnit = pCity->getPopulationUnitByIndex(i);
				if (NULL != pUnit)
				{
					pUnit->processProfession(pUnit->getProfession(), -1, false);
				}
			}
		}

		m_em_iProfessionCombatChange.add(eIndex, iChange);

		for (CvUnit* pUnit = firstUnit(&iLoop); pUnit != NULL; pUnit = nextUnit(&iLoop))
		{
			pUnit->processProfession(pUnit->getProfession(), 1, false);
		}

		for (uint i = 0; i < m_aEuropeUnits.size(); ++i)
		{
			m_aEuropeUnits[i]->processProfession(m_aEuropeUnits[i]->getProfession(), 1, false);
		}

		for (CvCity* pCity = firstCity(&iLoop); pCity != NULL; pCity = nextCity(&iLoop))
		{
			for (int i = 0; i < pCity->getPopulation(); ++i)
			{
				CvUnit* pUnit = pCity->getPopulationUnitByIndex(i);
				if (NULL != pUnit)
				{
					pUnit->processProfession(pUnit->getProfession(), 1, false);
				}
			}
		}

		for (CvCity* pCity = firstCity(&iLoop); pCity != NULL; pCity = nextCity(&iLoop))
		{
			pCity->AI_setWorkforceHack(false);
		}
	}
}

int CvPlayer::getBuildingClassCount(BuildingClassTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_iBuildingClassCount.get(eIndex);
}

void CvPlayer::changeBuildingClassCount(BuildingClassTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_em_iBuildingClassCount.add(eIndex, iChange);
	FAssert(getBuildingClassCount(eIndex) >= 0);
}


int CvPlayer::getBuildingClassMaking(BuildingClassTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_iBuildingClassMaking.get(eIndex);
}


void CvPlayer::changeBuildingClassMaking(BuildingClassTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_em_iBuildingClassMaking.add(eIndex, iChange);
		FAssert(getBuildingClassMaking(eIndex) >= 0);

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(Help_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getBuildingClassCountPlusMaking(BuildingClassTypes eIndex) const
{
	return (getBuildingClassCount(eIndex) + getBuildingClassMaking(eIndex));
}

// TAC - AI Buildings - koma13 - START
int CvPlayer::getSpecialBuildingCount(int iIndex, bool bPlusMaking) const
{
	int iCount = 0;

	for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		BuildingTypes eLoopBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(iI);

		if (eLoopBuilding != NO_BUILDING)
		{
			if (GC.getBuildingInfo(eLoopBuilding).getSpecialBuildingType() == iIndex)
			{
				iCount += (bPlusMaking) ? getBuildingClassCountPlusMaking((BuildingClassTypes)iI) : getBuildingClassCount((BuildingClassTypes)iI);
			}
		}
	}

	return iCount;
}
// TAC - AI Buildings - koma13 - END
int CvPlayer::getHurryCount(HurryTypes eIndex) const
{
	FAssert(eIndex >= 0);
	FAssert(eIndex < GC.getNumHurryInfos());
	return m_em_iHurryCount.get(eIndex);
}


bool CvPlayer::canPopRush()
{
	return (m_iPopRushHurryCount > 0);
}

void CvPlayer::changeHurryCount(HurryTypes eIndex, int iChange)
{
	FAssert(eIndex >= 0);
	FAssert(eIndex < GC.getNumHurryInfos());

	int oldHurryCount = m_em_iHurryCount.get(eIndex);
	m_em_iHurryCount.add(eIndex, iChange);
	FAssert(getHurryCount(eIndex) >= 0);

	// if we just went from 0 to 1 (or the reverse)
	if ((oldHurryCount > 0) != (m_em_iHurryCount.get(eIndex) > 0))
	{
		// does this hurry reduce population?
		if (GC.getHurryInfo(eIndex).getProductionPerPopulation() > 0)
		{
			m_iPopRushHurryCount += iChange;
			FAssert(m_iPopRushHurryCount >= 0);
		}
	}
}
int CvPlayer::getSpecialBuildingNotRequiredCount(SpecialBuildingTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumSpecialBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_iSpecialBuildingNotRequiredCount.get(eIndex);
}


bool CvPlayer::isSpecialBuildingNotRequired(SpecialBuildingTypes eIndex) const
{
	return (getSpecialBuildingNotRequiredCount(eIndex) > 0);
}

void CvPlayer::changeSpecialBuildingNotRequiredCount(SpecialBuildingTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumSpecialBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_em_iSpecialBuildingNotRequiredCount.add(eIndex, iChange);
	FAssert(getSpecialBuildingNotRequiredCount(eIndex) >= 0);
}


CivicTypes CvPlayer::getCivic(CivicOptionTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumCivicOptionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_eCivics.get(eIndex);
}

void CvPlayer::setCivic(CivicOptionTypes eIndex, CivicTypes eNewValue)
{
	CivicTypes eOldCivic = getCivic(eIndex);

	if (eOldCivic != eNewValue)
	{
		m_em_eCivics.set(eIndex, eNewValue);

		if (eOldCivic != NO_CIVIC)
		{
			processCivics(eOldCivic, -1);
		}

		if (getCivic(eIndex) != NO_CIVIC)
		{
			processCivics(getCivic(eIndex), 1);
		}

		GC.getGameINLINE().AI_makeAssignWorkDirty();

		if (GC.getGameINLINE().isFinalInitialized())
		{
			if (gDLL->isDiplomacy() && (gDLL->getDiplomacyPlayer() == getID()))
			{
				gDLL->updateDiplomacyAttitude(true);
			}

			if (getCivic(eIndex) != NO_CIVIC)
			{
				if (getCivic(eIndex) != GC.getCivilizationInfo(getCivilizationType()).getCivilizationInitialCivics(eIndex))
				{
					for (int iI = 0; iI < MAX_PLAYERS; iI++)
					{
						if (GET_PLAYER((PlayerTypes)iI).isAlive())
						{
							if (GET_TEAM(getTeam()).isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam()))
							{
								CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_PLAYER_ADOPTED_CIVIC", getNameKey(), GC.getCivicInfo(getCivic(eIndex)).getTextKeyWide());
								gDLL->getInterfaceIFace()->addMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MAJOR_EVENT);
							}
						}
					}

					CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_PLAYER_ADOPTED_CIVIC", getNameKey(), GC.getCivicInfo(getCivic(eIndex)).getTextKeyWide());
					GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szBuffer);
				}
			}
		}
	}
}


int CvPlayer::getImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumImprovementInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");
	return m_em_iImprovementYieldChange[eIndex1].get(eIndex2);
}


void CvPlayer::changeImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumImprovementInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_em_iImprovementYieldChange[eIndex1].add(eIndex2, iChange);
		FAssert(getImprovementYieldChange(eIndex1, eIndex2) >= 0);

		updateYield();
	}
}

int CvPlayer::getBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield) const
{
	FAssert(eBuildingClass >= 0);
	FAssert(eBuildingClass < GC.getNumBuildingClassInfos());
	FAssert(eYield >= 0);
	FAssert(eYield < NUM_YIELD_TYPES);
	return m_em_iBuildingYieldChange[eBuildingClass].get(eYield);
}

void CvPlayer::changeBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield, int iChange)
{
	FAssert(eBuildingClass >= 0);
	FAssert(eBuildingClass < GC.getNumBuildingClassInfos());
	FAssert(eYield >= 0);
	FAssert(eYield < NUM_YIELD_TYPES);

	if (iChange != 0)
	{
		m_em_iBuildingYieldChange[eBuildingClass].add(eYield, iChange);
		FAssert(getBuildingYieldChange(eBuildingClass, eYield) >= 0);

		updateYield();
	}
}

int CvPlayer::getTaxYieldModifierCount(YieldTypes eYield) const
{
	FAssert(eYield > -1);
	FAssert(eYield < NUM_YIELD_TYPES);
	return m_em_iTaxYieldModifierCount.get(eYield);
}

void CvPlayer::changeTaxYieldModifierCount(YieldTypes eYield, int iChange)
{
	FAssert(eYield > -1);
	FAssert(eYield < NUM_YIELD_TYPES);
	m_em_iTaxYieldModifierCount.add(eYield, iChange);
}


// XXX should pUnit be a CvSelectionGroup???
void CvPlayer::updateGroupCycle(CvUnit* pUnit)
{
	CLLNode<IDInfo>* pUnitNode;
	CLLNode<int>* pSelectionGroupNode;
	CLLNode<int>* pBestSelectionGroupNode;
	CvSelectionGroup* pLoopSelectionGroup;
	CvUnit* pHeadUnit;
	CvUnit* pBeforeUnit;
	CvUnit* pAfterUnit;
	CvUnit* pLoopUnit;
	CvPlot* pPlot;
	int iValue;
	int iBestValue;

	if (!(pUnit->isOnMap()))
	{
		return;
	}

	FAssertMsg(pUnit->getGroup() != NULL, "Unit->getGroup() is not assigned a valid value");

	removeGroupCycle(pUnit->getGroupID());

	pPlot = pUnit->plot();

	pBeforeUnit = NULL;
	pAfterUnit = NULL;

	pUnitNode = pPlot->headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if (pLoopUnit->isGroupHead())
		{
			if (pLoopUnit != pUnit)
			{
				if (!isBeforeUnitCycle(pLoopUnit, pUnit))
				{
					pBeforeUnit = pLoopUnit;
					break;
				}
				else
				{
					pAfterUnit = pLoopUnit;
				}
			}
		}
	}

	pSelectionGroupNode = headGroupCycleNode();

	iBestValue = MAX_INT;
	pBestSelectionGroupNode = NULL;

	while (pSelectionGroupNode != NULL)
	{
		pLoopSelectionGroup = getSelectionGroup(pSelectionGroupNode->m_data);
		FAssertMsg(pLoopSelectionGroup != NULL, "LoopSelectionGroup is not assigned a valid value");

		pHeadUnit = pLoopSelectionGroup->getHeadUnit();

		if (pHeadUnit != NULL)
		{
			if (pBeforeUnit != NULL)
			{
				if (pBeforeUnit == pHeadUnit)
				{
					pBestSelectionGroupNode = pSelectionGroupNode;
					break;
				}
			}
			else if (pAfterUnit != NULL)
			{
				if (pAfterUnit == pHeadUnit)
				{
					pBestSelectionGroupNode = nextGroupCycleNode(pSelectionGroupNode);
					break;
				}
			}
			else
			{
				iValue = plotDistance(pUnit->getX_INLINE(), pUnit->getY_INLINE(), pHeadUnit->getX_INLINE(), pHeadUnit->getY_INLINE());

				if (iValue < iBestValue)
				{
					iBestValue = iValue;
					pBestSelectionGroupNode = pSelectionGroupNode;
				}
			}
		}

		pSelectionGroupNode = nextGroupCycleNode(pSelectionGroupNode);
	}

	if (pBestSelectionGroupNode != NULL)
	{
		m_groupCycle.insertBefore(pUnit->getGroupID(), pBestSelectionGroupNode);
	}
	else
	{
		m_groupCycle.insertAtEnd(pUnit->getGroupID());
	}
}


void CvPlayer::removeGroupCycle(int iID)
{
	CLLNode<int>* pSelectionGroupNode;

	pSelectionGroupNode = headGroupCycleNode();

	while (pSelectionGroupNode != NULL)
	{
		if (pSelectionGroupNode->m_data == iID)
		{
			pSelectionGroupNode = deleteGroupCycleNode(pSelectionGroupNode);
			break;
		}
		else
		{
			pSelectionGroupNode = nextGroupCycleNode(pSelectionGroupNode);
		}
	}
}

CLLNode<int>* CvPlayer::deleteGroupCycleNode(CLLNode<int>* pNode)
{
	return m_groupCycle.deleteNode(pNode);
}

CLLNode<int>* CvPlayer::nextGroupCycleNode(CLLNode<int>* pNode) const
{
	return m_groupCycle.next(pNode);
}

CLLNode<int>* CvPlayer::previousGroupCycleNode(CLLNode<int>* pNode) const
{
	return m_groupCycle.prev(pNode);
}

CLLNode<int>* CvPlayer::headGroupCycleNode() const
{
	return m_groupCycle.head();
}

CLLNode<int>* CvPlayer::tailGroupCycleNode() const
{
	return m_groupCycle.tail();
}
void CvPlayer::addCityName(const CvWString& szName)
{
	m_aszCityNames.push_back(szName);
}

int CvPlayer::getNumCityNames() const
{
	return m_aszCityNames.size();
}


const CvWString& CvPlayer::getCityName(int iIndex) const
{
	return m_aszCityNames[iIndex];
}

CvCity* CvPlayer::firstCity(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_cities.beginIter(pIterIdx) : m_cities.endIter(pIterIdx);
}

CvCity* CvPlayer::nextCity(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_cities.nextIter(pIterIdx) : m_cities.prevIter(pIterIdx);
}


int CvPlayer::getNumCities() const
{
	return m_cities.getCount();
}


CvCity* CvPlayer::getCity(int iID) const
{
	return(m_cities.getAt(iID));
}


CvCity* CvPlayer::addCity()
{
	return(m_cities.add());
}


void CvPlayer::deleteCity(int iID)
{
	m_cities.removeAt(iID);
}


CvUnit* CvPlayer::firstUnit(int *pIterIdx) const
{
	if (m_units.empty())
	{
		return NULL;
	}

	CvIdVector<CvUnitAI>::const_iterator it = m_units.begin();
	CvUnit* pUnit = it->second;

	++it;
	if (it == m_units.end())
	{
		*pIterIdx = -1;
	}
	else
	{
		*pIterIdx = it->first;
	}

	return pUnit;
}


CvUnit* CvPlayer::nextUnit(int *pIterIdx) const
{
	CvIdVector<CvUnitAI>::const_iterator it = m_units.find(*pIterIdx);

	if (it == m_units.end())
	{
		return NULL;
	}

	CvUnit* pUnit = it->second;

	++it;
	if (it == m_units.end())
	{
		*pIterIdx = -1;
	}
	else
	{
		*pIterIdx = it->first;
	}

	return pUnit;
}


int CvPlayer::getNumUnits() const
{
	return (int)(m_units.size());
}


CvUnit* CvPlayer::getUnit(int iID) const
{
    return (m_units.getById(iID));
}


CvUnit* CvPlayer::addUnit()
{
	return (m_units.addNew());
}

void CvPlayer::addExistingUnit(CvUnit *pUnit)
{
	CvUnitAI* pUnitAI = dynamic_cast<CvUnitAI*>(pUnit);
	FAssert(pUnitAI != NULL);
	m_units[pUnitAI->getID()] = pUnitAI;
}

void CvPlayer::deleteUnit(int iID)
{
	m_units.removeById(iID);
}

CvUnit* CvPlayer::getAndRemoveUnit(int iId)
{
	CvUnit* pUnit = m_units.getById(iId);
	if(pUnit != NULL)
	{
		pUnit->removeFromMap();
		m_units.erase(pUnit->getID());
	}

	return pUnit;
}

int CvPlayer::getNumEuropeUnits() const
{
	return m_aEuropeUnits.size();
}

CvUnit* CvPlayer::getEuropeUnit(int iIndex) const
{
	FAssert(iIndex >= 0);
	FAssert(iIndex < (int)m_aEuropeUnits.size());
	return m_aEuropeUnits[iIndex];
}

CvUnit* CvPlayer::getEuropeUnitById(int iId) const
{
	for (int i = 0; i < getNumEuropeUnits(); ++i)
	{
		CvUnit* pUnit = getEuropeUnit(i);
		if (pUnit->getID() == iId)
		{
			return pUnit;
		}
	}

	return NULL;
}

void CvPlayer::loadUnitFromEurope(CvUnit* pUnit, CvUnit* pTransport)
{
	FAssert(NULL != pUnit && NULL != pTransport);
	if (NULL != pUnit && NULL != pTransport)
	{
		if (pUnit->canLoadUnit(pTransport, pTransport->plot(), false))
		{
			//transfer to player
			std::vector<CvUnit*>::iterator it = std::remove(m_aEuropeUnits.begin(), m_aEuropeUnits.end(), pUnit);
			if(it == m_aEuropeUnits.end())
			{
				FAssertMsg(false, "Could not find europe unit.");
			}
			else
			{
				m_aEuropeUnits.erase(it);
				addExistingUnit(pUnit);
				FAssert(pUnit->getUnitTravelState() == UNIT_TRAVEL_STATE_IN_EUROPE);
				pUnit->addToMap(pTransport->getX_INLINE(), pTransport->getY_INLINE());
				pUnit->setTransportUnit(pTransport);
				gDLL->getInterfaceIFace()->setDirty(EuropeScreen_DIRTY_BIT, true);
			}
		}
	}
}

void CvPlayer::unloadUnitToEurope(CvUnit* pUnit)
{
	FAssert(pUnit != NULL);
	if (NULL != pUnit)
	{
		FAssertMsg(pUnit->getYield() == NO_YIELD, "Yields are meant to be sold not unloaded");
		FAssertMsg(pUnit->getYieldStored() == 0, "Yields and Treasures are meant to be sold, not unloaded");
		UnitAITypes eUnitAI = pUnit->AI_getUnitAIType();
		CvUnit* pTransferUnit = getAndRemoveUnit(pUnit->getID());
		FAssert(pTransferUnit == pUnit);
		m_aEuropeUnits.push_back(pTransferUnit);
		pTransferUnit->AI_setUnitAIType(eUnitAI);
		pTransferUnit->setUnitTravelState(UNIT_TRAVEL_STATE_IN_EUROPE, false);
		gDLL->getInterfaceIFace()->setDirty(EuropeScreen_DIRTY_BIT, true);
	}
}

void CvPlayer::transferUnitInEurope(CvUnit* pUnit, CvUnit* pTransport)
{
	FAssert(NULL != pUnit && NULL != pTransport);
	if (NULL != pUnit && NULL != pTransport)
	{
		if (pUnit->canLoadUnit(pTransport, pTransport->plot(), false))
		{
			pUnit->setTransportUnit(pTransport);
			gDLL->getInterfaceIFace()->setDirty(EuropeScreen_DIRTY_BIT, true);
		}
	}
}


/*** TRIANGLETRADE 10/23/08 by DPII ***/
int CvPlayer::getNumAfricaUnits() const
{
	return m_aAfricaUnits.size();
}

CvUnit* CvPlayer::getAfricaUnit(int iIndex) const
{
	FAssert(iIndex >= 0);
	FAssert(iIndex < (int)m_aAfricaUnits.size());
	return m_aAfricaUnits[iIndex];
}

CvUnit* CvPlayer::getAfricaUnitById(int iId) const
{
	for (int i = 0; i < getNumAfricaUnits(); ++i)
	{
		CvUnit* pUnit = getAfricaUnit(i);
		if (pUnit->getID() == iId)
		{
			return pUnit;
		}
	}
	return NULL;
}

void CvPlayer::loadUnitFromAfrica(CvUnit* pUnit, CvUnit* pTransport)
{
	FAssert(NULL != pUnit && NULL != pTransport);
	if (NULL != pUnit && NULL != pTransport)
	{
		if (pUnit->canLoadUnit(pTransport, pTransport->plot(), false))
		{
			//transfer to player
			std::vector<CvUnit*>::iterator it = std::remove(m_aAfricaUnits.begin(), m_aAfricaUnits.end(), pUnit);
			if(it == m_aAfricaUnits.end())
			{
				FAssertMsg(false, "Could not find Africa unit.");
			}
			else
			{
				m_aAfricaUnits.erase(it);
				addExistingUnit(pUnit);
				FAssert(pUnit->getUnitTravelState() == UNIT_TRAVEL_STATE_IN_AFRICA);
				pUnit->addToMap(pTransport->getX_INLINE(), pTransport->getY_INLINE());
				pUnit->setTransportUnit(pTransport);
				gDLL->getInterfaceIFace()->setDirty(AfricaScreen_DIRTY_BIT, true);
			}
		}
	}
}

void CvPlayer::unloadUnitToAfrica(CvUnit* pUnit)
{
	FAssert(pUnit != NULL);
	if (NULL != pUnit)
	{
		FAssertMsg(pUnit->getYield() == NO_YIELD, "Yields are meant to be sold not unloaded");
		FAssertMsg(pUnit->getYieldStored() == 0, "Yields and Treasures are meant to be sold, not unloaded");
		UnitAITypes eUnitAI = pUnit->AI_getUnitAIType();
		CvUnit* pTransferUnit = getAndRemoveUnit(pUnit->getID());
		FAssert(pTransferUnit == pUnit);
		m_aAfricaUnits.push_back(pTransferUnit);
		pTransferUnit->AI_setUnitAIType(eUnitAI);
		pTransferUnit->setUnitTravelState(UNIT_TRAVEL_STATE_IN_AFRICA, false);
		gDLL->getInterfaceIFace()->setDirty(AfricaScreen_DIRTY_BIT, true);
	}
}

void CvPlayer::transferUnitInAfrica(CvUnit* pUnit, CvUnit* pTransport)
{
	FAssert(NULL != pUnit && NULL != pTransport);
	if (NULL != pUnit && NULL != pTransport)
	{
		if (pUnit->canLoadUnit(pTransport, pTransport->plot(), false))
		{
			pUnit->setTransportUnit(pTransport);
			gDLL->getInterfaceIFace()->setDirty(AfricaScreen_DIRTY_BIT, true);
		}
	}
}
/*******************************************************/

// R&R, ray, Port Royal
int CvPlayer::getNumPortRoyalUnits() const
{
	return m_aPortRoyalUnits.size();
}

CvUnit* CvPlayer::getPortRoyalUnit(int iIndex) const
{
	FAssert(iIndex >= 0);
	FAssert(iIndex < (int)m_aPortRoyalUnits.size());
	return m_aPortRoyalUnits[iIndex];
}

CvUnit* CvPlayer::getPortRoyalUnitById(int iId) const
{
	for (int i = 0; i < getNumPortRoyalUnits(); ++i)
	{
		CvUnit* pUnit = getPortRoyalUnit(i);
		if (pUnit->getID() == iId)
		{
			return pUnit;
		}
	}
	return NULL;
}

void CvPlayer::loadUnitFromPortRoyal(CvUnit* pUnit, CvUnit* pTransport)
{
	FAssert(NULL != pUnit && NULL != pTransport);
	if (NULL != pUnit && NULL != pTransport)
	{
		if (pUnit->canLoadUnit(pTransport, pTransport->plot(), false))
		{
			//transfer to player
			std::vector<CvUnit*>::iterator it = std::remove(m_aPortRoyalUnits.begin(), m_aPortRoyalUnits.end(), pUnit);
			if(it == m_aPortRoyalUnits.end())
			{
				FAssertMsg(false, "Could not find Port Royal unit.");
			}
			else
			{
				m_aPortRoyalUnits.erase(it);
				addExistingUnit(pUnit);
				FAssert(pUnit->getUnitTravelState() == UNIT_TRAVEL_STATE_IN_PORT_ROYAL);
				pUnit->addToMap(pTransport->getX_INLINE(), pTransport->getY_INLINE());
				pUnit->setTransportUnit(pTransport);
				gDLL->getInterfaceIFace()->setDirty(PortRoyalScreen_DIRTY_BIT, true);
			}
		}
	}
}

void CvPlayer::unloadUnitToPortRoyal(CvUnit* pUnit)
{
	FAssert(pUnit != NULL);
	if (NULL != pUnit)
	{
		FAssertMsg(pUnit->getYield() == NO_YIELD, "Yields are meant to be sold not unloaded");
		FAssertMsg(pUnit->getYieldStored() == 0, "Yields and Treasures are meant to be sold, not unloaded");
		UnitAITypes eUnitAI = pUnit->AI_getUnitAIType();
		CvUnit* pTransferUnit = getAndRemoveUnit(pUnit->getID());
		FAssert(pTransferUnit == pUnit);
		m_aPortRoyalUnits.push_back(pTransferUnit);
		pTransferUnit->AI_setUnitAIType(eUnitAI);
		pTransferUnit->setUnitTravelState(UNIT_TRAVEL_STATE_IN_PORT_ROYAL, false);
		gDLL->getInterfaceIFace()->setDirty(PortRoyalScreen_DIRTY_BIT, true);
	}
}

void CvPlayer::transferUnitInPortRoyal(CvUnit* pUnit, CvUnit* pTransport)
{
	FAssert(NULL != pUnit && NULL != pTransport);
	if (NULL != pUnit && NULL != pTransport)
	{
		if (pUnit->canLoadUnit(pTransport, pTransport->plot(), false))
		{
			pUnit->setTransportUnit(pTransport);
			gDLL->getInterfaceIFace()->setDirty(PortRoyalScreen_DIRTY_BIT, true);
		}
	}
}
// R&R, ray, Port Royal - END

int CvPlayer::countNumTravelUnits(UnitTravelStates eState, DomainTypes eDomain) const
{
	int iCount = 0;
	int iLoop;
	for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		// RaR, ray, excluded Yields from calculation
		if ((eDomain == NO_DOMAIN || pLoopUnit->getDomainType() == eDomain) && pLoopUnit->AI_getUnitAIType() != UNITAI_YIELD)
		{
			if (pLoopUnit->getUnitTravelState() == eState)
			{
				++iCount;
			}
		}
	}

	return iCount;
}

int CvPlayer::countNumDomainUnits(DomainTypes eDomain) const
{
	int iCount = 0;
	int iLoop;
	for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		// RaR, ray, excluded Yields from calculation
		if ((eDomain == NO_DOMAIN || pLoopUnit->getDomainType() == eDomain) && pLoopUnit->AI_getUnitAIType() != UNITAI_YIELD)
		{
			++iCount;
		}
	}
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		for (int i = 0; i < pLoopCity->getPopulation(); ++i)
		{
			if (eDomain == NO_DOMAIN || pLoopCity->getPopulationUnitByIndex(i)->getDomainType() == eDomain)
			{
				++iCount;
			}
		}
	}
	for (uint i = 0; i < m_aEuropeUnits.size(); ++i)
	{
		if (eDomain == NO_DOMAIN || m_aEuropeUnits[i]->getDomainType() == eDomain)
		{
			++iCount;
		}
	}

	/*** TRIANGLETRADE 10/23/08 by DPII ***/
	for (uint i = 0; i < m_aAfricaUnits.size(); ++i)
	{
		if (eDomain == NO_DOMAIN || m_aAfricaUnits[i]->getDomainType() == eDomain)
		{
			++iCount;
		}
	}
	/**************************************/
	// R&R, ray, Port Royal
	for (uint i = 0; i < m_aPortRoyalUnits.size(); ++i)
	{
		if (eDomain == NO_DOMAIN || m_aPortRoyalUnits[i]->getDomainType() == eDomain)
		{
			++iCount;
		}
	}
	// R&R, ray, Port Royal - END

	return iCount;
}

CvSelectionGroup* CvPlayer::firstSelectionGroup(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_selectionGroups.beginIter(pIterIdx) : m_selectionGroups.endIter(pIterIdx);
}


CvSelectionGroup* CvPlayer::nextSelectionGroup(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_selectionGroups.nextIter(pIterIdx) : m_selectionGroups.prevIter(pIterIdx);
}


int CvPlayer::getNumSelectionGroups() const
{
	return m_selectionGroups.getCount();
}


CvSelectionGroup* CvPlayer::getSelectionGroup(int iID) const
{
	return ((CvSelectionGroup *)(m_selectionGroups.getAt(iID)));
}


CvSelectionGroup* CvPlayer::addSelectionGroup()
{
	return ((CvSelectionGroup *)(m_selectionGroups.add()));
}


void CvPlayer::deleteSelectionGroup(int iID)
{
	bool bRemoved = m_selectionGroups.removeAt(iID);

	FAssertMsg(bRemoved, "could not find group, delete failed");
}

EventTriggeredData* CvPlayer::firstEventTriggered(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_eventsTriggered.beginIter(pIterIdx) : m_eventsTriggered.endIter(pIterIdx);
}

EventTriggeredData* CvPlayer::nextEventTriggered(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_eventsTriggered.nextIter(pIterIdx) : m_eventsTriggered.prevIter(pIterIdx);
}

int CvPlayer::getNumEventsTriggered() const
{
	return m_eventsTriggered.getCount();
}

EventTriggeredData* CvPlayer::getEventTriggered(int iID) const
{
	return ((EventTriggeredData*)(m_eventsTriggered.getAt(iID)));
}

EventTriggeredData* CvPlayer::addEventTriggered()
{
	return ((EventTriggeredData*)(m_eventsTriggered.add()));
}

void CvPlayer::deleteEventTriggered(int iID)
{
	m_eventsTriggered.removeAt(iID);
}


void CvPlayer::addMessage(const CvTalkingHeadMessage& message)
{
	m_listGameMessages.push_back(message);
}


void CvPlayer::clearMessages()
{
	m_listGameMessages.clear();
}


const CvMessageQueue& CvPlayer::getGameMessages() const
{
	return (m_listGameMessages);
}


void CvPlayer::expireMessages()
{
	CvMessageQueue::iterator it = m_listGameMessages.begin();
	bool bFoundExpired = false;
	while(it != m_listGameMessages.end())
	{
		CvTalkingHeadMessage& message = *it;
		if (GC.getGameINLINE().getGameTurn() >= message.getExpireTurn())
		{
			it = m_listGameMessages.erase(it);
			bFoundExpired = true;
		}
		else
		{
			++it;
		}
	}
	if (bFoundExpired)
	{
		gDLL->getInterfaceIFace()->dirtyTurnLog(getID());
	}
}


void CvPlayer::addPopup(CvPopupInfo* pInfo, bool bFront)
{
	if (isHuman())
	{
		if (bFront)
		{
			m_listPopups.push_front(pInfo);
		}
		else
		{
			m_listPopups.push_back(pInfo);
		}
	}
	else
	{
		FAssertMsg(false, "Adding popup to AI");
		SAFE_DELETE(pInfo);
	}
}


void CvPlayer::clearPopups()
{
	CvPopupQueue::iterator it;
	for (it = m_listPopups.begin(); it != m_listPopups.end(); ++it)
	{
		CvPopupInfo* pInfo = *it;
		if (NULL != pInfo)
		{
			delete pInfo;
		}
	}
	m_listPopups.clear();
}


CvPopupInfo* CvPlayer::popFrontPopup()
{
	CvPopupInfo* pInfo = NULL;
	if (!m_listPopups.empty())
	{
		pInfo = m_listPopups.front();
		m_listPopups.pop_front();
	}
	return pInfo;
}


const CvPopupQueue& CvPlayer::getPopups() const
{
	return (m_listPopups);
}


void CvPlayer::addDiplomacy(CvDiploParameters* pDiplo)
{
	if (NULL != pDiplo)
	{
		m_listDiplomacy.push_back(pDiplo);
	}
}


void CvPlayer::clearDiplomacy()
{
	CvDiploQueue::iterator it;
	for (it = m_listDiplomacy.begin(); it != m_listDiplomacy.end(); ++it)
	{
		CvDiploParameters* pDiplo = *it;
		if (NULL != pDiplo)
		{
			delete pDiplo;
		}
	}
	m_listDiplomacy.clear();
}


const CvDiploQueue& CvPlayer::getDiplomacy() const
{
	return (m_listDiplomacy);
}


CvDiploParameters* CvPlayer::popFrontDiplomacy()
{
	CvDiploParameters* pDiplo = NULL;
	if (!m_listDiplomacy.empty())
	{
		pDiplo = m_listDiplomacy.front();
		m_listDiplomacy.pop_front();
	}
	return pDiplo;
}


int CvPlayer::getScoreHistory(int iTurn) const
{
	CvTurnScoreMap::const_iterator it = m_mapScoreHistory.find(iTurn);
	if (it != m_mapScoreHistory.end())
	{
		return (*it).second;
	}
	return 0;
}

void CvPlayer::updateScoreHistory(int iTurn, int iBestScore)
{
	m_mapScoreHistory[iTurn] = iBestScore;
}

int CvPlayer::getEconomyHistory(int iTurn) const
{
	CvTurnScoreMap::const_iterator it = m_mapEconomyHistory.find(iTurn);
	if (it != m_mapEconomyHistory.end())
	{
		return (*it).second;
	}
	return 0;
}

void CvPlayer::updateEconomyHistory(int iTurn, int iBestEconomy)
{
	m_mapEconomyHistory[iTurn] = iBestEconomy;
}

int CvPlayer::getIndustryHistory(int iTurn) const
{
	CvTurnScoreMap::const_iterator it = m_mapIndustryHistory.find(iTurn);
	if (it != m_mapIndustryHistory.end())
	{
		return (*it).second;
	}
	return 0;
}

void CvPlayer::updateIndustryHistory(int iTurn, int iBestIndustry)
{
	m_mapIndustryHistory[iTurn] = iBestIndustry;
}

int CvPlayer::getAgricultureHistory(int iTurn) const
{
	CvTurnScoreMap::const_iterator it = m_mapAgricultureHistory.find(iTurn);
	if (it != m_mapAgricultureHistory.end())
	{
		return (*it).second;
	}
	return 0;
}

void CvPlayer::updateAgricultureHistory(int iTurn, int iBestAgriculture)
{
	m_mapAgricultureHistory[iTurn] = iBestAgriculture;
}

int CvPlayer::getPowerHistory(int iTurn) const
{
	CvTurnScoreMap::const_iterator it = m_mapPowerHistory.find(iTurn);
	if (it != m_mapPowerHistory.end())
	{
		return (*it).second;
	}
	return 0;
}

void CvPlayer::updatePowerHistory(int iTurn, int iBestPower)
{
	m_mapPowerHistory[iTurn] = iBestPower;
}

int CvPlayer::getCultureHistory(int iTurn) const
{
	CvTurnScoreMap::const_iterator it = m_mapCultureHistory.find(iTurn);
	if (it != m_mapCultureHistory.end())
	{
		return (*it).second;
	}
	return 0;
}


int CvPlayer::addTradeRoute(const IDInfo& kSource, const IDInfo& kDestination, YieldTypes eYield)
{
	FAssert(eYield != NO_YIELD);

	CvCity* pSourceCity = ::getCity(kSource);
	FAssert(pSourceCity != NULL);
	if (pSourceCity == NULL)
	{
		return -1;
	}
	FAssert(pSourceCity->getTeam() == getTeam());

	CvCity* pDestinationCity = ::getCity(kDestination);
	FAssert(pDestinationCity != NULL || (kDestination.eOwner == getID() && kDestination.iID == CvTradeRoute::EUROPE_CITY_ID));
	FAssert(pDestinationCity == NULL || pDestinationCity->getTeam() == getTeam());

	if (kSource == kDestination)
	{
		return -1;
	}

	for (CvIdVector<CvTradeRoute>::iterator it = m_tradeRoutes.begin(); it != m_tradeRoutes.end(); ++it)
	{
		CvTradeRoute* pTradeRoute = it->second;
		if (pTradeRoute->getSourceCity() == kSource
			&& pTradeRoute->getDestinationCity() == kDestination
			&& pTradeRoute->getYield() == eYield)
		{
			return pTradeRoute->getID();
		}
	}

	CvTradeRoute* pTradeRoute = m_tradeRoutes.addNew();
	pTradeRoute->init(kSource, kDestination, eYield);
	pSourceCity->addExport(eYield, false);
	if (pDestinationCity != NULL)
	{
		pDestinationCity->addImport(eYield, false);
	}
	if (getID() == GC.getGameINLINE().getActivePlayer())
	{
		gDLL->getInterfaceIFace()->setDirty(Domestic_Advisor_DIRTY_BIT, true);
	}

	return pTradeRoute->getID();
}


bool CvPlayer::editTradeRoute(int iId, const IDInfo& kSource, const IDInfo& kDestination, YieldTypes eYield)
{
	CvTradeRoute* pTradeRoute = getTradeRoute(iId);
	if (pTradeRoute == NULL)
	{
		return false;
	}

	for (CvIdVector<CvTradeRoute>::iterator it = m_tradeRoutes.begin(); it != m_tradeRoutes.end(); ++it)
	{
		CvTradeRoute* pLoopTradeRoute = it->second;
		if (pLoopTradeRoute->getSourceCity() == kSource
			&& pLoopTradeRoute->getDestinationCity() == kDestination
			&& pLoopTradeRoute->getYield() == eYield)
		{
			return false;
		}
	}

	pTradeRoute->setSourceCity(kSource);
	pTradeRoute->setDestinationCity(kDestination);
	pTradeRoute->setYield(eYield);

	return true;
}


bool CvPlayer::removeTradeRoute(int iId, bool bRemoveImport) // auto traderoute - Nightinggale
{
	CvTradeRoute* pTradeRoute = getTradeRoute(iId);
	FAssert(pTradeRoute != NULL);
	if (pTradeRoute == NULL)
	{
		return false;
	}

	int iLoop;
	for(CvSelectionGroup* pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
	{
		pLoopSelectionGroup->assignTradeRoute(iId, false);
	}

	CvCity* pSourceCity = ::getCity(pTradeRoute->getSourceCity());
	CvCity* pDestinationCity = ::getCity(pTradeRoute->getDestinationCity());
	if(!m_tradeRoutes.removeById(iId))
	{
		return false;
	}

	if (pSourceCity != NULL)
	{
		pSourceCity->updateExports();
	}

	if (pDestinationCity != NULL && bRemoveImport) // auto traderoute - Nightinggale
	{
		pDestinationCity->updateImports();
	}

	if (getID() == GC.getGameINLINE().getActivePlayer())
	{
		gDLL->getInterfaceIFace()->setDirty(Domestic_Advisor_DIRTY_BIT, true);
	}

	return true;
}

CvTradeRoute* CvPlayer::getTradeRoute(int iId) const
{
	CvTradeRoute* pTradeRoute = m_tradeRoutes.getById(iId);
	FAssert(pTradeRoute == NULL || pTradeRoute->getDestinationCity().eOwner == getID() || pTradeRoute->getSourceCity().eOwner == getID());
	return pTradeRoute;
}

int CvPlayer::getNumTradeRoutes() const
{
	return m_tradeRoutes.size();
}

void CvPlayer::validateTradeRoutes()
{
	std::vector<CvTradeRoute*> aTradeRoutes;
	getTradeRoutes(aTradeRoutes);
	for (uint i = 0; i < aTradeRoutes.size(); ++i)
	{
		CvTradeRoute* pTradeRoute = aTradeRoutes[i];
		if (!pTradeRoute->checkValid(getID()))
		{
			removeTradeRoute(pTradeRoute->getID());
		}
	}

	//re-add missing europe destination routes
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes) iPlayer);
		if (canLoadYield((PlayerTypes) iPlayer))
		{
			for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
			{
				YieldTypes eYield = (YieldTypes) iYield;
				if (isYieldEuropeTradable(eYield))
				{
					int iLoop;
					for (CvCity* pLoopCity = kLoopPlayer.firstCity(&iLoop); NULL != pLoopCity; pLoopCity = kLoopPlayer.nextCity(&iLoop))
					{
						// TAC - AI Economy - koma13 - START
						//if (pLoopCity->isExport(eYield))
						if (pLoopCity->isExport(eYield) && pLoopCity->isBestPortCity())
						// TAC - AI Economy - koma13 - END
						{
							addTradeRoute(pLoopCity->getIDInfo(), IDInfo(getID(), CvTradeRoute::EUROPE_CITY_ID), eYield);
						}
					}
				}
			}
		}
	}
}

// slow
CvTradeRoute* CvPlayer::getTradeRouteByIndex(int iIndex) const
{
	FAssert(iIndex >= 0 && iIndex < getNumTradeRoutes());
	CvIdVector<CvTradeRoute>::const_iterator it;
	int i = 0;
	for (it = m_tradeRoutes.begin(); it != m_tradeRoutes.end(); ++it)
	{
		if (i++ == iIndex)
		{
			return it->second;
		}
	}

	return NULL;
}


void CvPlayer::getTradeRoutes(std::vector<CvTradeRoute*>& aTradeRoutes) const
{
	CvIdVector<CvTradeRoute>::const_iterator it;

	for (it = m_tradeRoutes.begin(); it != m_tradeRoutes.end(); ++it)
	{
		aTradeRoutes.push_back(it->second);
	}

}



bool CvPlayer::canLoadYield(PlayerTypes eCityPlayer) const
{
	if (eCityPlayer != getID())
	{
		return false;
	}

	return true;
}

bool CvPlayer::canUnloadYield(PlayerTypes eCityPlayer) const
{
	if (GET_PLAYER(eCityPlayer).getTeam() != getTeam())
	{
		return false;
	}

	return true;
}

void CvPlayer::updateCultureHistory(int iTurn, int iBestCulture)
{
	m_mapCultureHistory[iTurn] = iBestCulture;
}

std::string CvPlayer::getScriptData() const
{
	return m_szScriptData;
}

void CvPlayer::setScriptData(std::string szNewValue)
{
	m_szScriptData = szNewValue;
}

const CvString& CvPlayer::getPbemEmailAddress() const
{
	return GC.getInitCore().getEmail(getID());
}

void CvPlayer::setPbemEmailAddress(const char* szAddress)
{
	GC.getInitCore().setEmail(getID(), szAddress);
}

const CvString& CvPlayer::getSmtpHost() const
{
	return GC.getInitCore().getSmtpHost(getID());
}

void CvPlayer::setSmtpHost(const char* szHost)
{
	GC.getInitCore().setSmtpHost(getID(), szHost);
}

// Protected Functions...

void CvPlayer::doGold()
{
	CyArgsList argsList;
	argsList.add(getID());
	long lResult=0;
	gDLL->getPythonIFace()->callFunction(PYGameModule, "doGold", argsList.makeFunctionArgs(), &lResult);
	if (lResult == 1)
	{
		return;
	}

	int iGoldChange = 0;

	FAssert(isHuman() || ((getGold() + iGoldChange) >= 0));

	changeGold(iGoldChange);

}

/** NBMOD REF **/

/***************************************************************************/
/**                                                                       **/
/** int CvPlayer::NBMOD_GetEuropeMilitaryValue() const                    **/
/**                                                                       **/
/** Ermittelt den Milit�rwert der Europ�ischen Armee.                     **/
/**                                                                       **/
/***************************************************************************/

int CvPlayer::NBMOD_GetEuropeMilitaryValue() const
{
    float fEMW = 0.0;
    float fThisStrength = 0.0;

	int iSize = m_aEuropeRevolutionUnits.size();
	for (int iI = 0; iI < iSize; iI++)
	{
	    // Pr�fen ob das �berhaupt eine Einheit ist
		if (getRevolutionEuropeUnit(iI) != NO_UNIT)
		{
            // es darf sich um kein Schiff handeln
            if (GC.getUnitInfo(getRevolutionEuropeUnit(iI)).getDomainType() != DOMAIN_SEA)
            {
                // Die St�rke der Einheit ermitteln
                fThisStrength = (float)GC.getUnitInfo(getRevolutionEuropeUnit(iI)).getCombat();

                // Pr�fen, ob die Einheit �berhaupt einen Beruf hat
                if (getRevolutionEuropeProfession(iI) != NO_PROFESSION)
                {
                    fThisStrength += (float)GC.getProfessionInfo(getRevolutionEuropeProfession(iI)).getCombatChange();
                }

                // Die St�rke mit einem Gewicht versehen
                fThisStrength = fThisStrength * GC.getUnitInfo(getRevolutionEuropeUnit(iI)).NBMOD_GetStrengthWeight();

                fEMW += fThisStrength;
			}
	    }
	}

    fEMW += 0.5; // mathematisch korrekt Runden

    return (int)fEMW;
}

/***************************************************************************/
/**                                                                       **/
/** int CvPlayer::NBMOD_REF_GetStartValue() const                         **/
/**                                                                       **/
/** Gibt den Start-Milt�rwert des Spieles zur�ck.                         **/
/**                                                                       **/
/***************************************************************************/

int CvPlayer::NBMOD_REF_GetStartValue() const
{
    return m_iNBMOD_REF_StartValue;
}

/***************************************************************************/
/**                                                                       **/
/** int CvPlayer::NBMOD_REF_MakeStartValue()                              **/
/**                                                                       **/
/** Ermittelt den Milit�rwert beim Start des Spieles.                     **/
/**                                                                       **/
/***************************************************************************/

int CvPlayer::NBMOD_REF_MakeStartValue()
{
    double fValue = 0.0;
    fValue = sqrt(sqrt((double)GC.getMapINLINE().getGridHeightINLINE() * GC.getMapINLINE().getGridWidthINLINE()));
    fValue = fValue * GC.getNBMOD_REF_MAP_COEFFICIENT();
    fValue = fValue * GC.getHandicapInfo(getHandicapType()).NBMOD_GetREFWeight() / 100;
	// TAC - Reduced REF Option - koma13 - START
	if (GC.getGameINLINE().isOption(GAMEOPTION_REDUCED_REF) || (!isHuman() && GC.getENABLE_REDUCED_REF_FOR_AI()))
	{
		fValue = fValue * GC.getREDUCED_REF_PERCENT() / 100;
	}
	// TAC - Reduced REF Option - koma13 - END
    m_iNBMOD_REF_StartValue = (int)fValue;
    return m_iNBMOD_REF_StartValue;
}

/***************************************************************************/
/**                                                                       **/
/** int CvPlayer::NBMOD_GetColonialMilitaryValue() const                  **/
/**                                                                       **/
/** Ermittelt den Milit�rwert der Kolonialarmee.                          **/
/**                                                                       **/
/***************************************************************************/

int CvPlayer::NBMOD_GetColonialMilitaryValue() const
{
    double fKMW = 0.0;
    double fStrength = 0.0;

    int iStoredWeapons = 0;
    int iStoredHorses = 0;

    int iCities = 0;
    int iRevolutionWant = 0;

    int iBonusPercent = 0;

	CvUnit* pLoopUnit;
	CvCity* pLoopCity;

	int iLoop;

    // Die Gesamtbev�lkerung: Kolonisten * Gewicht
    fKMW += getTotalPopulation() * GC.getNBMOD_REF_POPULATION_WEIGHT();

	// Die Einheitenst�rke: Soldaten/Kanonen/Dragoner
	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
        if (GC.getNBMOD_REF_REAL_WEAPONS() == 1)
        {
            // Falls die Einheit Waffen/Pferde transportiert
            if (pLoopUnit->getYield() == YIELD_BLADES || pLoopUnit->getYield() == YIELD_MUSKETS || pLoopUnit->getYield() == YIELD_CANNONS)
            {
                iStoredWeapons += pLoopUnit->getYieldStored();
            }

            if (pLoopUnit->getYield() == YIELD_HORSES)
            {
                iStoredHorses += pLoopUnit->getYieldStored();
            }
        }

        if (GC.getUnitInfo(pLoopUnit->getUnitType()).getDomainType() != DOMAIN_SEA)
        {
            fStrength = (double) pLoopUnit->baseCombatStr();
            fStrength = fStrength * GC.getUnitInfo(pLoopUnit->getUnitType()).NBMOD_GetStrengthWeight();
            fKMW += fStrength;
		}
	}

	// Die aktuelle Rundenzahl
	fStrength = (double)GC.getGameINLINE().getGameTurn();
	  // Den Spielmodusmodifizierer dazurechnen
    fStrength = fStrength * 100.0/(double)(GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent());
      // Das Rundengewicht beachten (jede Runde wiegt X Punkte)
    fKMW += fStrength * GC.getNBMOD_REF_TURN_WEIGHT();

    // Die Anzahl der gelagerten Waffen
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
	    iCities++;
		iStoredWeapons += pLoopCity->getYieldStored(YIELD_BLADES);
		iStoredWeapons += pLoopCity->getYieldStored(YIELD_MUSKETS);
		iStoredWeapons += pLoopCity->getYieldStored(YIELD_CANNONS);
		iStoredHorses += pLoopCity->getYieldStored(YIELD_HORSES);
	}

	//koma13
	if (iCities == 0)
	{
		return 0;
	}

      // Den Spielmodusmodifizierer hinzurechnen
    iStoredWeapons = iStoredWeapons * 100/GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
    iStoredHorses = iStoredHorses * 100/GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();

    iStoredWeapons = iStoredWeapons / 50;
    iStoredHorses = iStoredHorses / 50;

    if (GC.getNBMOD_REF_WEAPONS_BONUS() == 1)
    {
        iStoredWeapons = iStoredWeapons / iCities;
        iStoredHorses = iStoredHorses / iCities;

        iBonusPercent += iStoredWeapons * GC.getNBMOD_REF_WEAPONS_BONUS_PERCENT();
        iBonusPercent += iStoredHorses * GC.getNBMOD_REF_HORSES_BONUS_PERCENT();
    }
    else
    {
        fKMW += iStoredWeapons * GC.getNBMOD_REF_WEAPONS_WEIGHT();
        fKMW += iStoredHorses * GC.getNBMOD_REF_HORSES_WEIGHT();
    }

    // Den Bonus dazuz�hlen

    fKMW = fKMW * (1.0 + (double)iBonusPercent/(double)100);

    // Der Unabh�ngigkeitswille

    if (GC.getNBMOD_REF_REVOLUTION_PERCENT_ENABLE() == 1)
    {
        iRevolutionWant = GET_TEAM(getTeam()).getRebelPercent() + 50;
        fKMW = fKMW * iRevolutionWant / 100;
    }

    // Max-Steuerrate
    if (GC.getNBMOD_REF_MAX_TAX_ENABLE() == 1)
    {
        fKMW = fKMW * NBMOD_GetMaxTaxRate() / 100;
    }

    // Der Aufschlag des K�nigs
    fKMW = fKMW * GC.getNBMOD_REF_KING_PANIC_WEIGHT();

    // Der Schwierigkeitsgrad
    fKMW = fKMW * GC.getHandicapInfo(getHandicapType()).NBMOD_GetREFWeight() / 100;

	// TAC - Reduced REF Option - koma13 - START
	if (GC.getGameINLINE().isOption(GAMEOPTION_REDUCED_REF) || (!isHuman() && GC.getENABLE_REDUCED_REF_FOR_AI()))
	{
		fKMW = fKMW * GC.getREDUCED_REF_PERCENT() / 100;
	}
	// TAC - Reduced REF Option - koma13 - END
    // Der Startwert zu Beginn des Spieles
    fKMW += NBMOD_REF_GetStartValue() * GC.getNBMOD_REF_START_VALUE_PERCENT() / 100;

    fKMW += 0.5; // mathematisch korrekt Runden

    return (int)fKMW;
}

/***************************************************************************/
/**                                                                       **/
/** void CvPlayer::NBMOD_AddEuropeRandomUnit(bool bDisplay)               **/
/**                                                                       **/
/** Erweitert die k�nigliche Armee um eine zuf�llige Einheit.             **/
/** Parameter:                                                            **/
/**  - bDisplay = gibt an, ob eine Nachricht angezeigt werden soll        **/
/**                                                                       **/
/***************************************************************************/

void CvPlayer::NBMOD_AddEuropeRandomUnit(bool bDisplay)
{
    if (NO_PLAYER != getParent())
	{
		CvPlayer& kParent = GET_PLAYER(getParent());
		FAssert(kParent.isEurope());

		CvCivilizationInfo& kCivilizationInfo = GC.getCivilizationInfo(kParent.getCivilizationType());
		int iNumFreeUnits = kCivilizationInfo.getNumCivilizationFreeUnits();
        std::vector<int> aiUnitWeights(iNumFreeUnits, 100);
		for (int i = 0; i < iNumFreeUnits; ++i)
		{
			int iUnitClass = kCivilizationInfo.getCivilizationFreeUnitsClass(i);
			UnitTypes eUnit = (UnitTypes) kCivilizationInfo.getCivilizationUnits(iUnitClass);
			if (eUnit == NO_UNIT)
			{
				aiUnitWeights[i] = 0;
			}
			else
			{

                // Fragt ab ob es ein Schiff ist und ob das zuf�llige Aufr�sten von Schiffen verboten ist
				if ((GC.getUnitInfo(eUnit).getDomainType() == DOMAIN_SEA) && (GC.getNBMOD_REF_RANDOM_SHIPS() != 1))
				{
					aiUnitWeights[i] = 0; // Es sollen keine Schiffe gebaut werden
				}
			}
		}

		if (iNumFreeUnits > 0)
		{
			int iIndex = GC.getGameINLINE().getSorenRand().pickValue(aiUnitWeights, "Pick Expeditionary force unit");
			int iUnitClass = kCivilizationInfo.getCivilizationFreeUnitsClass(iIndex);
			ProfessionTypes eUnitProfession = (ProfessionTypes) kCivilizationInfo.getCivilizationFreeUnitsProfession(iIndex);
			UnitTypes eUnit = (UnitTypes)kCivilizationInfo.getCivilizationUnits(iUnitClass);
			FAssert(eUnit != NO_UNIT);

			addRevolutionEuropeUnit(eUnit, eUnitProfession);

			const wchar* szUnitName;
			if (eUnitProfession != NO_PROFESSION)
			{
				szUnitName = GC.getProfessionInfo(eUnitProfession).getTextKeyWide();
			}
			else
			{
				szUnitName = GC.getUnitInfo(eUnit).getTextKeyWide();
			}

            if (bDisplay && m_bNBMOD_REF_Display)
            {
                CvWString szBuffer = gDLL->getText("TXT_KEY_NEW_EUROPE_ARMY", kParent.getCivilizationShortDescriptionKey(), getCivilizationShortDescriptionKey(), szUnitName, kParent.getCivilizationAdjectiveKey());
                gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNIT_GREATPEOPLE", MESSAGE_TYPE_INFO, GC.getUnitInfo(eUnit).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"));

                m_bNBMOD_REF_Display = false;
                m_iNBMOD_REF_DisplayTurn = 0;
            }
		}
    }

    return;
}

/***************************************************************************/
/**                                                                       **/
/** void CvPlayer::NBMOD_AddEuropeShipUnit(bool bDisplay)                 **/
/**                                                                       **/
/** Erweitert die k�nigliche Armee um eine zuf�llige Seeeinheit.          **/
/** Parameter:                                                            **/
/**  - bDisplay = gibt an, ob eine Nachricht angezeigt werden soll        **/
/**                                                                       **/
/***************************************************************************/

void CvPlayer::NBMOD_AddEuropeShipUnit(bool bDisplay)
{
    if (NO_PLAYER != getParent())
	{
		CvPlayer& kParent = GET_PLAYER(getParent());
		FAssert(kParent.isEurope());

		CvCivilizationInfo& kCivilizationInfo = GC.getCivilizationInfo(kParent.getCivilizationType());
		int iNumFreeUnits = kCivilizationInfo.getNumCivilizationFreeUnits();
        std::vector<int> aiUnitWeights(iNumFreeUnits, 100);
		for (int i = 0; i < iNumFreeUnits; ++i)
		{
			int iUnitClass = kCivilizationInfo.getCivilizationFreeUnitsClass(i);
			UnitTypes eUnit = (UnitTypes) kCivilizationInfo.getCivilizationUnits(iUnitClass);
			if (eUnit == NO_UNIT)
			{
				aiUnitWeights[i] = 0;
			}
			else
			{
				if (GC.getUnitInfo(eUnit).getDomainType() != DOMAIN_SEA)
				{
					aiUnitWeights[i] = 0; // Es sollen nur Schiffe gebaut werden
				}
			}
		}

		if (iNumFreeUnits > 0)
		{
			int iIndex = GC.getGameINLINE().getSorenRand().pickValue(aiUnitWeights, "Pick Expeditionary force unit");
			int iUnitClass = kCivilizationInfo.getCivilizationFreeUnitsClass(iIndex);
			ProfessionTypes eUnitProfession = (ProfessionTypes) kCivilizationInfo.getCivilizationFreeUnitsProfession(iIndex);
			UnitTypes eUnit = (UnitTypes)kCivilizationInfo.getCivilizationUnits(iUnitClass);
			FAssert(eUnit != NO_UNIT);

			addRevolutionEuropeUnit(eUnit, eUnitProfession);

			const wchar* szUnitName;
			if (eUnitProfession != NO_PROFESSION)
			{
				szUnitName = GC.getProfessionInfo(eUnitProfession).getTextKeyWide();
			}
			else
			{
				szUnitName = GC.getUnitInfo(eUnit).getTextKeyWide();
			}

            if (bDisplay)
            {
                CvWString szBuffer = gDLL->getText("TXT_KEY_NEW_EUROPE_ARMY", kParent.getCivilizationShortDescriptionKey(), getCivilizationShortDescriptionKey(), szUnitName, kParent.getCivilizationAdjectiveKey());
                gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNIT_GREATPEOPLE", MESSAGE_TYPE_INFO, GC.getUnitInfo(eUnit).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"));
            }
		}
    }

    return;
}


/***************************************************************************/
/**                                                                       **/
/** int CvPlayer::NBMOD_GetNumEuropeUnits() const                         **/
/**                                                                       **/
/** Ermittelt die Anzahl der Landeinheiten des K�nigs.                    **/
/**                                                                       **/
/***************************************************************************/
int CvPlayer::NBMOD_GetNumEuropeUnits() const
{
	int iNum = 0;
	int iSize = m_aEuropeRevolutionUnits.size();
	for (int iI = 0; iI < iSize; iI++)
	{
		if (GC.getUnitInfo(getRevolutionEuropeUnit(iI)).getDomainType() != DOMAIN_SEA)
		{
			iNum += GC.getUnitInfo(getRevolutionEuropeUnit(iI)).getRequiredTransportSize();
		}
	}
	return iNum;
}

/***************************************************************************/
/**                                                                       **/
/** int CvPlayer::NBMOD_GetNumEuropeTransporting() const                  **/
/**                                                                       **/
/** Ermittelt die Transportkapazit�t des K�nigs.                          **/
/**                                                                       **/
/***************************************************************************/
int CvPlayer::NBMOD_GetNumEuropeTransporting() const
{
	int iTransporting = 0;
	int iSize = m_aEuropeRevolutionUnits.size();
	for (int iI = 0; iI < iSize; iI++)
	{
		if (getRevolutionEuropeUnit(iI) != NO_UNIT)
		{
			if (GC.getUnitInfo(getRevolutionEuropeUnit(iI)).getDomainType() == DOMAIN_SEA)
			{
				iTransporting += GC.getUnitInfo(getRevolutionEuropeUnit(iI)).getCargoSpace();
			}
		}
	}
	return iTransporting;
}

/***************************************************************************/
/**                                                                       **/
/** int CvPlayer::NBMOD_GetEuropeShipStrength() const                     **/
/**                                                                       **/
/** Ermittelt die Schiffst�rke des K�nigs.                                **/
/**                                                                       **/
/***************************************************************************/
int CvPlayer::NBMOD_GetEuropeShipStrength() const
{
    float fStrength = 0.0;
    float fThisStrength = 0.0;

	int iSize = m_aEuropeRevolutionUnits.size();
	for (int iI = 0; iI < iSize; iI++)
	{
	    // Pr�fen ob das �berhaupt eine Einheit ist
		if (getRevolutionEuropeUnit(iI) != NO_UNIT)
		{

            if (GC.getUnitInfo(getRevolutionEuropeUnit(iI)).getDomainType() == DOMAIN_SEA)
            {
                // Die St�rke des Schiffes ermitteln
                fThisStrength = (float)GC.getUnitInfo(getRevolutionEuropeUnit(iI)).getCombat();

                // Pr�fen, ob die Einheit �berhaupt einen Beruf hat
                if (getRevolutionEuropeProfession(iI) != NO_PROFESSION)
                {
                    fThisStrength += (float)GC.getProfessionInfo(getRevolutionEuropeProfession(iI)).getCombatChange();
                }

                // Die St�rke mit einem Gewicht versehen
                fThisStrength = fThisStrength * GC.getUnitInfo(getRevolutionEuropeUnit(iI)).NBMOD_GetStrengthWeight();

                fStrength += fThisStrength;
            }
		}
	}

    fStrength += 0.5; // mathematisch korrekt Runden

    return (int)fStrength;
}

/***************************************************************************/
/**                                                                       **/
/** int CvPlayer::NBMOD_GetColonialShipStrength() const                   **/
/**                                                                       **/
/** Ermittelt die Schiffst�rke der Kolonie.                               **/
/**                                                                       **/
/***************************************************************************/
int CvPlayer::NBMOD_GetColonialShipStrength() const
{
    float fStrength = 0.0;
    float fThisStrength = 0.0;

	CvSelectionGroup* pLoopSelectionGroup;
	int iLoop;

	for(pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup != NULL; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
	{
		fThisStrength = pLoopSelectionGroup->NBMOD_GetShipStrength();

		fStrength += fThisStrength;
	}

    fStrength += 0.5; // mathematisch korrekt Runden

    return (int)fStrength;
}

/** NBMOD REF **/
//koma13
int CvPlayer::getEuropeMilitary() const
{
	int iEuropeMilitary = NBMOD_GetEuropeMilitaryValue();
	int iUnitRevolutionRatio = GC.getUNIT_REVOLUTION_RATIO();

	FAssert(iUnitRevolutionRatio > 0);

	iEuropeMilitary *= iUnitRevolutionRatio;

	return (iEuropeMilitary / 100);
}

void CvPlayer::doBells()
{
	if (getParent() == NO_PLAYER)
	{
		return;
	}

	int iBellsRate = getYieldRate(YIELD_BELLS);
	if (iBellsRate == 0)
	{
		return;
	}
	//add bells to political points
	for (int i = 0; i < GC.getNumFatherPointInfos(); ++i)
	{
		FatherPointTypes ePointType = (FatherPointTypes) i;
		changeFatherPoints(ePointType, iBellsRate * GC.getFatherPointInfo(ePointType).getYieldPoints(YIELD_BELLS));
	}

	//update revolution unit bells
	if (!GC.getEraInfo(getCurrentEra()).isRevolution())
	{
		changeBellsStored(iBellsRate);
		// Soll die �nderung des NBMODs benutzt werden
		if (GC.getNBMOD_REF_ENABLE() != 1)
		{
			if (getBellsStored() >= revolutionEuropeUnitThreshold() && iBellsRate > GC.getCivilizationInfo(getCivilizationType()).getFreeYields(YIELD_BELLS))
			{
				changeBellsStored(-revolutionEuropeUnitThreshold());
				setRevolutionEuropeUnitThresholdMultiplier((getRevolutionEuropeUnitThresholdMultiplier() * (100 + GC.getDefineINT("REVOLUTION_EUROPE_UNIT_THRESHOLD_INCREASE"))) / 100);
				setKingNumUnitMultiplier(getKingNumUnitMultiplier() * GC.getHandicapInfo(getHandicapType()).getKingNumUnitMultiplier() / 100);

				if (NO_PLAYER != getParent())
				{
					CvPlayer& kParent = GET_PLAYER(getParent());
					FAssert(kParent.isEurope());

					CvCivilizationInfo& kCivilizationInfo = GC.getCivilizationInfo(kParent.getCivilizationType());
					int iNumFreeUnits = kCivilizationInfo.getNumCivilizationFreeUnits();
					std::vector<int> aiUnitWeights(iNumFreeUnits, 100);
					for (int i = 0; i < iNumFreeUnits; ++i)
					{
						int iUnitClass = kCivilizationInfo.getCivilizationFreeUnitsClass(i);
						UnitTypes eUnit = (UnitTypes) kCivilizationInfo.getCivilizationUnits(iUnitClass);
						if (eUnit == NO_UNIT)
						{
							aiUnitWeights[i] = 0;
						}
						else
						{
							if (GC.getUnitInfo(eUnit).getDomainType() == DOMAIN_SEA)
							{
								aiUnitWeights[i] += std::max(-100, GC.getDefineINT("REVOLUTION_EUROPE_UNIT_SHIP_MODIFIER"));
							}
						}
					}

					if (iNumFreeUnits > 0)
					{
						int iNumUnits = std::max(1, GC.getDefineINT("KING_INITIAL_UNIT_INCREASE") * getKingNumUnitMultiplier() / 100);
						for (int i = 0; i < iNumUnits; ++i)
						{
							int iIndex = GC.getGameINLINE().getSorenRand().pickValue(aiUnitWeights, "Pick Expeditionary force unit");
							int iUnitClass = kCivilizationInfo.getCivilizationFreeUnitsClass(iIndex);
							ProfessionTypes eUnitProfession = (ProfessionTypes) kCivilizationInfo.getCivilizationFreeUnitsProfession(iIndex);
							UnitTypes eUnit = (UnitTypes)kCivilizationInfo.getCivilizationUnits(iUnitClass);
							FAssert(eUnit != NO_UNIT);
							addRevolutionEuropeUnit(eUnit, eUnitProfession);
						}

						CvWString szBuffer = gDLL->getText("TXT_KEY_NEW_EUROPE_ARMY_NEW", kParent.getCivilizationShortDescriptionKey(), getCivilizationShortDescriptionKey(), iNumUnits, kParent.getCivilizationAdjectiveKey());
						gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNIT_GREATPEOPLE", GC.getGameINLINE().isDebugMode() ? MESSAGE_TYPE_MAJOR_EVENT : MESSAGE_TYPE_INFO, GC.getLeaderHeadInfo(kParent.getLeaderType()).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"));
					}
				}
			}
		}
		else
		{
			bool bUnitGot = false;

            if (GC.getNBMOD_REF_DEBUG() == 1)
            {
                // DEBUG
                gDLL->getInterfaceIFace()->addMessage(getID() , true, GC.getEVENT_MESSAGE_TIME(), CvWString::format(L"(EMW) %d vs. (KMW) %d",NBMOD_GetEuropeMilitaryValue(), NBMOD_GetColonialMilitaryValue()), NULL, MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"));
                gDLL->getInterfaceIFace()->addMessage(getID() , true, GC.getEVENT_MESSAGE_TIME(), CvWString::format(L"Start-MW: %d",NBMOD_REF_GetStartValue()), NULL, MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"));
//            gDLL->getInterfaceIFace()->addMessage(getID() , true, GC.getEVENT_MESSAGE_TIME(), CvWString::format(L"Einheiten/Kapazit�t: %d/%d",NBMOD_GetNumEuropeUnits(),NBMOD_GetNumEuropeTransporting()), NULL, MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"));
            }

            // Zuerst �berlegt sich der K�nig, ob er seine Schlachtschiffe aufr�sten m�chte
            // Ist die Kolonialflotte st�rker als die k�bigliche Flotte?
            if (NBMOD_GetColonialShipStrength() > NBMOD_GetEuropeShipStrength())
            {

                // Dann ein Schiff aufr�sten
                NBMOD_AddEuropeShipUnit(true);
                bUnitGot = true;
            }
            // TAC - AI Revolution - koma13 - START
			/*
			else
				{
					int iTransporting = NBMOD_GetNumEuropeTransporting(); // Wie viel Transportkapazit�t steht dem K�nig zur Verf�gung?
					int iNumUnits = NBMOD_GetNumEuropeUnits(); // Wie viele Landeinheiten besitzt der K�nig?

					// Errechnet wie vile Einheiten mit einer Welle zur Kolonie gelangen sollen
					iNumUnits = iNumUnits * GC.getDefineINT("NBMOD_REF_NUM_UNITS_PERCENT") / 100;

					// Ist die Anzahl der zu transportierenden Einheiten gr��er als die Transportkapazit�t?
					if (iNumUnits > iTransporting)
					{
						// Dann ein Schiff aufr�sten
						NBMOD_AddEuropeShipUnit(true);
						bUnitGot = true;
					}
				}
			*/
			// TAC - AI Revolution - koma13 - END

            // Wenn noch kein Schiff aufger�stet wurde, dann �berlegt sich der K�nig, ob er seine Landeinheiten r�stet
            if (!(bUnitGot))
            {
                int iKMW = NBMOD_GetColonialMilitaryValue();
                int iEMW = NBMOD_GetEuropeMilitaryValue();

                float n = (float)iKMW/(float)iEMW;

                while (n > 1.1) // die Kolonie ist viel st�rker als der K�nig
                {

                    NBMOD_AddEuropeRandomUnit(true);

                    // die Werte neu berechnen
                    iKMW = NBMOD_GetColonialMilitaryValue();
                    iEMW = NBMOD_GetEuropeMilitaryValue();

                    n = (float)iKMW/(float)iEMW;
                    bUnitGot = true;

                }

                // Die Kolonie ist st�rker als der K�nig, aber es wurden noch keine Waffen hinzugef�gt
                if (n > 1 && bUnitGot == false)
                {
                    NBMOD_AddEuropeRandomUnit(true);
                    bUnitGot = true;
                }


				// TAC - AI Revolution - koma13 - START
				if (bUnitGot)
				{
					int iTransporting = NBMOD_GetNumEuropeTransporting(); // Wie viel Transportkapazit�t steht dem K�nig zur Verf�gung?
					int iNumUnits = NBMOD_GetNumEuropeUnits(); // Wie viele Landeinheiten besitzt der K�nig?

					// Errechnet wie vile Einheiten mit einer Welle zur Kolonie gelangen sollen
					iNumUnits = iNumUnits * GC.getNBMOD_REF_NUM_UNITS_PERCENT() / 100;

					// Ist die Anzahl der zu transportierenden Einheiten gr��er als die Transportkapazit�t?
					while(iNumUnits > iTransporting)
					{
						// Dann ein Schiff aufr�sten
						NBMOD_AddEuropeShipUnit(true);
						iTransporting = NBMOD_GetNumEuropeTransporting();
					}
				}
				// TAC - AI Revolution - koma13 - END
            }

            m_iNBMOD_REF_DisplayTurn++;

            if (m_iNBMOD_REF_DisplayTurn > 4)
            {
                m_bNBMOD_REF_Display = true;
            }
		}
	}
}
/** NBMOD REF **/


void CvPlayer::doCrosses()
{
	if (getParent() == NO_PLAYER)
	{
		return;
	}

	int iCrossRate = getYieldRate(YIELD_CROSSES);
	// WTP, ray, Happiness - START
	int iHappinessRate = getHappinessRate();
	int iUnHappinessRate = getUnHappinessRate();

	iCrossRate = (iCrossRate * (100 + iHappinessRate - iUnHappinessRate)) / 100; // this is percentage modifcation
	// WTP, ray, Happiness - EMD

	//add crosses to political points
	for (int i = 0; i < GC.getNumFatherPointInfos(); ++i)
	{
		FatherPointTypes ePointType = (FatherPointTypes) i;
		changeFatherPoints(ePointType, iCrossRate * GC.getFatherPointInfo(ePointType).getYieldPoints(YIELD_CROSSES));
	}

	if (getImmigrationConversion() == YIELD_CROSSES)
	{
		// WTP, ray, Happiness - START
		// changeCrossesStored(iCrossRate);
		if (iCrossRate > 0)
		{
			changeCrossesStored(iCrossRate);
		}
		// WTP, ray, Happiness - END

		// TAC - short messages for immigration after fist - RAY
		int imCount = 0;

		// TAC - Multiple Immigrants - Ray - START
		//if (getCrossesStored() >= immigrationThreshold())
		while (getCrossesStored() >= immigrationThreshold())
		// TAC - Multiple Immigrants - Ray - END
		{
			// TAC - short messages for immigration after fist - RAY
			if (imCount == 0) {
				doImmigrant(GC.getGameINLINE().getSorenRandNum(CivEffect()->getNumUnitsOnDock(), "pick immigrant"), false);
				imCount++;
			}
			else {
				doImmigrant(GC.getGameINLINE().getSorenRandNum(CivEffect()->getNumUnitsOnDock(), "pick immigrant"), true);
			}
		}
	}
}

void CvPlayer::doAdvancedStartAction(AdvancedStartActionTypes eAction, int iX, int iY, int iData, bool bAdd)
{
	if (getAdvancedStartPoints() < 0)
	{
		return;
	}

	CvPlot* pPlot = GC.getMap().plot(iX, iY);

	if (0 == getNumCities())
	{
		switch (eAction)
		{
		case ADVANCEDSTARTACTION_EXIT:
			//Try to build this player's empire
			if (getID() == GC.getGameINLINE().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setBusy(true);
			}
			AI_doAdvancedStart(true);
			if (getID() == GC.getGameINLINE().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setBusy(false);
			}
			break;
		case ADVANCEDSTARTACTION_AUTOMATE:
		case ADVANCEDSTARTACTION_CITY:
			break;
		default:
			// The first action must be to place a city
			// so players can lose by spending everything
			return;
		}
	}

	switch (eAction)
	{
	case ADVANCEDSTARTACTION_EXIT:
		changeGold(getAdvancedStartPoints());
		setAdvancedStartPoints(-1);
		if (GC.getGameINLINE().getActivePlayer() == getID())
		{
			gDLL->getInterfaceIFace()->setInAdvancedStart(false);
		}

		if (isHuman())
		{
			int iLoop;
			for (CvCity* pCity = firstCity(&iLoop); NULL != pCity; pCity = nextCity(&iLoop))
			{
				pCity->chooseProduction();
			}
		}
		break;
	case ADVANCEDSTARTACTION_AUTOMATE:
		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setBusy(true);
		}
		AI_doAdvancedStart(true);
		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setBusy(false);
		}
		break;
	case ADVANCEDSTARTACTION_UNIT:
		{
			if(pPlot == NULL)
				return;

			UnitTypes eUnit = (UnitTypes) iData;
			int iCost = getAdvancedStartUnitCost(eUnit, bAdd, pPlot);

			if (bAdd && iCost < 0)
			{
				return;
			}

			// Add unit to the map
			if (bAdd)
			{
				if (getAdvancedStartPoints() >= iCost)
				{
					CvUnit* pUnit = initUnit(eUnit, GC.getUnitInfo(eUnit).getDefaultProfession(), iX, iY);
					if (NULL != pUnit)
					{
						pUnit->finishMoves();
						changeAdvancedStartPoints(-iCost);
					}
				}
			}

			// Remove unit from the map
			else
			{
				// If cost is -1 we already know this unit isn't present
				if (iCost != -1)
				{
					CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
					while (pUnitNode != NULL)
					{
						CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
						pUnitNode = pPlot->nextUnitNode(pUnitNode);

						if (pLoopUnit->getUnitType() == eUnit)
						{
							pLoopUnit->kill(false);
							changeAdvancedStartPoints(iCost);
							return;
						}
					}
				}

				// Proper unit not found above, delete first found
				CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
				if (pUnitNode != NULL)
				{
					CvUnit* pUnit = ::getUnit(pUnitNode->m_data);

					iCost = getAdvancedStartUnitCost(pUnit->getUnitType(), false);
					FAssertMsg(iCost != -1, "If this is -1 then that means it's going to try to delete a unit which shouldn't exist");
					pUnit->kill(false);
					changeAdvancedStartPoints(iCost);
				}
			}

			if (getID() == GC.getGameINLINE().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setDirty(Advanced_Start_DIRTY_BIT, true);
			}
		}
		break;
	case ADVANCEDSTARTACTION_CITY:
		{
			if(pPlot == NULL)
				return;

			int iCost = getAdvancedStartCityCost(bAdd, pPlot);

			if (iCost < 0)
			{
				return;
			}

			// Add City to the map
			if (bAdd)
			{
				if (0 == getNumCities())
				{
					PlayerTypes eClosestPlayer = NO_PLAYER;
					int iMinDistance = MAX_INT;
					for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
					{
						CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);
						if (kPlayer.isAlive())
						{
							if (kPlayer.getTeam() == getTeam())
							{
								if (0 == kPlayer.getNumCities())
								{
									FAssert(kPlayer.getStartingPlot() != NULL);
									int iDistance = plotDistance(iX, iY, kPlayer.getStartingPlot()->getX_INLINE(), kPlayer.getStartingPlot()->getY_INLINE());
									if (iDistance < iMinDistance)
									{
										eClosestPlayer = kPlayer.getID();
										iMinDistance = iDistance;
									}
								}
							}
						}
					}
					FAssertMsg(eClosestPlayer != NO_PLAYER, "Self at a minimum should always be valid");
					if (eClosestPlayer != getID())
					{
						CvPlot* pTempPlot = GET_PLAYER(eClosestPlayer).getStartingPlot();
						GET_PLAYER(eClosestPlayer).setStartingPlot(getStartingPlot(), false);
						setStartingPlot(pTempPlot, false);
					}
				}
				if (getAdvancedStartPoints() >= iCost || 0 == getNumCities())
				{
					found(iX, iY);
					changeAdvancedStartPoints(-iCost);
					GC.getGameINLINE().updateColoredPlots();
					CvCity* pCity = pPlot->getPlotCity();
					if (pCity != NULL)
					{
						if(pCity->getPopulation() == 0)
						{
							pCity->setPopulation(1);
						}

						if (pCity->getPopulation() > 1)
						{
							pCity->setFood(pCity->growthThreshold() / 2);
						}
					}
				}
			}

			// Remove City from the map
			else
			{
				pPlot->setRouteType(NO_ROUTE);
				pPlot->getPlotCity()->kill();
				pPlot->setImprovementType(NO_IMPROVEMENT);
				changeAdvancedStartPoints(iCost);
			}

			if (getID() == GC.getGameINLINE().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setDirty(Advanced_Start_DIRTY_BIT, true);
			}
		}
		break;
	case ADVANCEDSTARTACTION_POP:
		{
			if(pPlot == NULL)
				return;

			CvCity* pCity = pPlot->getPlotCity();

			if (pCity != NULL)
			{
				int iCost = getAdvancedStartPopCost(bAdd, pCity);

				if (iCost < 0)
				{
					return;
				}

				// Add Pop to the City
				if (bAdd)
				{
					if (getAdvancedStartPoints() >= iCost)
					{
						pCity->changePopulation(1);
						changeAdvancedStartPoints(-iCost);
						if (pCity->getPopulation() > 1)
						{
							pCity->setFood(pCity->growthThreshold() / 2);
							pCity->setFoodKept((pCity->getFood() * pCity->getMaxFoodKeptPercent()) / 100);
						}
					}
				}

				// Remove Pop from the city
				else
				{
					pCity->changePopulation(-1);
					changeAdvancedStartPoints(iCost);
					if (pCity->getPopulation() == 1)
					{
						pCity->setFood(0);
						pCity->setFoodKept(0);
					}
				}
			}
		}
		break;
	case ADVANCEDSTARTACTION_CULTURE:
		{
			if(pPlot == NULL)
				return;

			CvCity* pCity = pPlot->getPlotCity();

			if (pCity != NULL)
			{
				int iCost = getAdvancedStartCultureCost(bAdd, pCity);

				if (iCost < 0)
				{
					return;
				}

				// Add Culture to the City
				if (bAdd)
				{
					if (getAdvancedStartPoints() >= iCost)
					{
						pCity->setCulture(getID(), pCity->getCultureThreshold(), true);
						changeAdvancedStartPoints(-iCost);
					}
				}

				// Remove Culture from the city
				else
				{
					CultureLevelTypes eLevel = (CultureLevelTypes)std::max(0, pCity->getCultureLevel() - 1);
					pCity->setCulture(getID(), pCity->getCultureThreshold(eLevel), true);
					changeAdvancedStartPoints(iCost);
				}
			}
		}
		break;
	case ADVANCEDSTARTACTION_BUILDING:
		{
			if(pPlot == NULL)
				return;

			CvCity* pCity = pPlot->getPlotCity();

			if (pCity != NULL)
			{
				BuildingTypes eBuilding = (BuildingTypes) iData;
				int iCost = getAdvancedStartBuildingCost(eBuilding, bAdd, pCity);

				if (iCost < 0)
				{
					return;
				}

				// Add Building to the City
				if (bAdd)
				{
					if (getAdvancedStartPoints() >= iCost)
					{
						pCity->setHasRealBuilding(eBuilding, true);
						changeAdvancedStartPoints(-iCost);
						if (GC.getBuildingInfo(eBuilding).getFoodKept() != 0)
						{
							pCity->setFoodKept((pCity->getFood() * pCity->getMaxFoodKeptPercent()) / 100);
						}
					}
				}

				// Remove Building from the map
				else
				{
					pCity->setHasRealBuilding(eBuilding, false);
					changeAdvancedStartPoints(iCost);
					if (GC.getBuildingInfo(eBuilding).getFoodKept() != 0)
					{
						pCity->setFoodKept((pCity->getFood() * pCity->getMaxFoodKeptPercent()) / 100);
					}
				}
			}

			if (getID() == GC.getGameINLINE().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setDirty(Advanced_Start_DIRTY_BIT, true);
			}
		}
		break;
	case ADVANCEDSTARTACTION_ROUTE:
		{
			if(pPlot == NULL)
				return;

			RouteTypes eRoute = (RouteTypes) iData;
			int iCost = getAdvancedStartRouteCost(eRoute, bAdd, pPlot);

			if (bAdd && iCost < 0)
			{
				return;
			}

			// Add Route to the plot
			if (bAdd)
			{
				if (getAdvancedStartPoints() >= iCost)
				{
					pPlot->setRouteType(eRoute);
					changeAdvancedStartPoints(-iCost);
				}
			}

			// Remove Route from the Plot
			else
			{
				if (pPlot->getRouteType() != eRoute)
				{
					eRoute = pPlot->getRouteType();
					iCost = getAdvancedStartRouteCost(eRoute, bAdd);
				}

				if (iCost < 0)
				{
					return;
				}

				pPlot->setRouteType(NO_ROUTE);
				changeAdvancedStartPoints(iCost);
			}

			if (getID() == GC.getGameINLINE().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setDirty(Advanced_Start_DIRTY_BIT, true);
			}
		}
		break;
	case ADVANCEDSTARTACTION_IMPROVEMENT:
		{
			if(pPlot == NULL)
				return;

			ImprovementTypes eImprovement = (ImprovementTypes) iData;
			int iCost = getAdvancedStartImprovementCost(eImprovement, bAdd, pPlot);

			if (bAdd && iCost < 0)
			{
				return;
			}

			// Add Improvement to the plot
			if (bAdd)
			{
				if (getAdvancedStartPoints() >= iCost)
				{
					if (pPlot->getFeatureType() != NO_FEATURE)
					{
						for (int iI = 0; iI < GC.getNumBuildInfos(); ++iI)
						{
							ImprovementTypes eLoopImprovement = ((ImprovementTypes)(GC.getBuildInfo((BuildTypes)iI).getImprovement()));

							if (eImprovement == eLoopImprovement)
							{
								if (GC.getBuildInfo((BuildTypes)iI).isFeatureRemove(pPlot->getFeatureType()) && canBuild(pPlot, (BuildTypes)iI))
								{
									pPlot->setFeatureType(NO_FEATURE);
									break;
								}
							}
						}
					}
					pPlot->setImprovementType(eImprovement);
					changeAdvancedStartPoints(-iCost);
				}
			}
			// Remove Improvement from the Plot
			else
			{
				if (pPlot->getImprovementType() != eImprovement)
				{
					eImprovement = pPlot->getImprovementType();
					iCost = getAdvancedStartImprovementCost(eImprovement, bAdd, pPlot);
				}
				if (iCost < 0)
				{
					return;
				}
				pPlot->setImprovementType(NO_IMPROVEMENT);
				changeAdvancedStartPoints(iCost);
			}

			if (getID() == GC.getGameINLINE().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setDirty(Advanced_Start_DIRTY_BIT, true);
			}
		}
		break;
	case ADVANCEDSTARTACTION_VISIBILITY:
		{
			if(pPlot == NULL)
				return;

			int iCost = getAdvancedStartVisibilityCost(bAdd, pPlot);

			if (iCost < 0)
			{
				return;
			}

			// Add Visibility to the plot
			if (bAdd)
			{
				if (getAdvancedStartPoints() >= iCost)
				{
					pPlot->setRevealed(getTeam(), true, true, NO_TEAM);
					changeAdvancedStartPoints(-iCost);
				}
			}

			// Remove Visibility from the Plot
			else
			{
				pPlot->setRevealed(getTeam(), false, true, NO_TEAM);
				changeAdvancedStartPoints(iCost);
			}
		}
		break;
	default:
		FAssert(false);
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing a unit
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartUnitCost(UnitTypes eUnit, bool bAdd, CvPlot* pPlot)
{
	int iLoop;
	int iNumUnitType = 0;

	if (0 == getNumCities())
	{
		return -1;
	}

	int iCost = 0;
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		iCost += getYieldProductionNeeded(eUnit, (YieldTypes) iYield);
	}

	iCost *= GC.getUnitInfo(eUnit).getAdvancedStartCost();
	if (iCost < 0)
	{
		return -1;
	}
	iCost /= 100;

	if (NULL == pPlot)
	{
		if (bAdd)
		{
			bool bValid = false;
			int iLoop;
			for (CvCity* pLoopCity = firstCity(&iLoop); NULL != pLoopCity; pLoopCity = nextCity(&iLoop))
			{
				if (pLoopCity->canTrain(eUnit))
				{
					bValid = true;
					break;
				}
			}

			if (!bValid)
			{
				return -1;
			}
		}
	}
	else
	{
		CvCity* pCity = NULL;

		if (0 == GC.getDefineINT("ADVANCED_START_ALLOW_UNITS_OUTSIDE_CITIES"))
		{
			pCity = pPlot->getPlotCity();

			if (NULL == pCity || pCity->getOwnerINLINE() != getID())
			{
				return -1;
			}

			iCost *= 100;
			iCost /= std::max(1, 100 + pCity->getProductionModifier(eUnit));
		}
		else
		{
			if (pPlot->getOwnerINLINE() != getID())
			{
				return -1;
			}

			iCost *= 100;
			iCost /= std::max(1, 100 + getProductionModifier(eUnit));
		}


		if (bAdd)
		{
			int iMaxUnitsPerCity = GC.getDefineINT("ADVANCED_START_MAX_UNITS_PER_CITY");
			if (iMaxUnitsPerCity >= 0)
			{
				if (getNumUnits() >= iMaxUnitsPerCity * getNumCities())
				{
					return -1;
				}
			}

			if (NULL != pCity)
			{
				if (!pCity->canTrain(eUnit))
				{
					return -1;
				}
			}
			else
			{
				if (!pPlot->canTrain(eUnit, false, false))
				{
					return -1;
				}

				if (pPlot->isImpassable() && !GC.getUnitInfo(eUnit).isCanMoveImpassable())
				{
					return -1;
				}

				if (pPlot->getFeatureType() != NO_FEATURE)
				{
					if (GC.getUnitInfo(eUnit).getFeatureImpassable(pPlot->getFeatureType()))
					{
						return -1;
					}
				}
				else
				{
					if (GC.getUnitInfo(eUnit).getTerrainImpassable(pPlot->getTerrainType()))
					{
						return -1;
					}
				}
			}
		}
		// Must be this unit at plot in order to remove
		else
		{
			bool bUnitFound = false;

			CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
			while (pUnitNode != NULL)
			{
				CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = pPlot->nextUnitNode(pUnitNode);

				if (pLoopUnit->getUnitType() == eUnit)
				{
					bUnitFound = true;
				}
			}

			if (!bUnitFound)
			{
				return -1;
			}
		}
	}

	// Increase cost if the XML defines that additional units will cost more
	if (0 != GC.getUnitInfo(eUnit).getAdvancedStartCostIncrease())
	{
		for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
		{
			if (pLoopUnit->getUnitType() == eUnit)
			{
				++iNumUnitType;
			}
		}

		if (!bAdd)
		{
			--iNumUnitType;
		}

		if (iNumUnitType > 0)
		{
			iCost *= 100 + GC.getUnitInfo(eUnit).getAdvancedStartCostIncrease() * iNumUnitType;
			iCost /= 100;
		}
	}

	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing a City
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartCityCost(bool bAdd, CvPlot* pPlot)
{
	int iNumCities = getNumCities();

	int iCost = getNewCityProductionValue();

	if (iCost < 0)
	{
		return -1;
	}

	// Valid plot?
	if (pPlot != NULL)
	{
		// Need valid plot to found on if adding
		if (bAdd)
		{
			if (!canFound(pPlot->getX(), pPlot->getY(), false))
			{
				return -1;
			}
		}
		// Need your own city present to remove
		else
		{
			if (pPlot->isCity())
			{
				if (pPlot->getPlotCity()->getOwnerINLINE() != getID())
				{
					return -1;
				}
			}
			else
			{
				return -1;
			}
		}

		// Is there a distance limit on how far a city can be placed from a player's start/another city?
		if (GC.getDefineINT("ADVANCED_START_CITY_PLACEMENT_MAX_RANGE") > 0)
		{
			PlayerTypes eClosestPlayer = NO_PLAYER;
			int iClosestDistance = MAX_INT;

			for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
			{
				CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);

				if (kPlayer.isAlive())
				{
					CvPlot* pStartingPlot = kPlayer.getStartingPlot();

					if (NULL != pStartingPlot)
					{
						int iDistance = ::plotDistance(pPlot->getX_INLINE(), pPlot->getY_INLINE(), pStartingPlot->getX_INLINE(), pStartingPlot->getY_INLINE());
						if (iDistance <= GC.getDefineINT("ADVANCED_START_CITY_PLACEMENT_MAX_RANGE"))
						{
							if (iDistance < iClosestDistance || (iDistance == iClosestDistance && getTeam() != kPlayer.getTeam()))
							{
								iClosestDistance = iDistance;
								eClosestPlayer = kPlayer.getID();
							}
						}
					}
				}
			}

			if (NO_PLAYER == eClosestPlayer || GET_PLAYER(eClosestPlayer).getTeam() != getTeam())
			{
				return -1;
			}
			//Only allow founding a city at someone elses start point if
			//We have no cities and they have no cities.
			if ((getID() != eClosestPlayer) && ((getNumCities() > 0) || (GET_PLAYER(eClosestPlayer).getNumCities() > 0)))
			{
				return -1;
			}
		}
	}

	// Increase cost if the XML defines that additional units will cost more
	if (0 != GC.getDefineINT("ADVANCED_START_CITY_COST_INCREASE"))
	{
		if (!bAdd)
		{
			--iNumCities;
		}

		if (iNumCities > 0)
		{
			iCost *= 100 + GC.getDefineINT("ADVANCED_START_CITY_COST_INCREASE") * iNumCities;
			iCost /= 100;
		}
	}

	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing Population
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartPopCost(bool bAdd, CvCity* pCity)
{
	if (0 == getNumCities())
	{
		return -1;
	}

	int iCost = (getGrowthThreshold(1) * GC.getDefineINT("ADVANCED_START_POPULATION_COST")) / 100;

	if (NULL != pCity)
	{
		if (pCity->getOwnerINLINE() != getID())
		{
			return -1;
		}

		int iPopulation = pCity->getPopulation();

		// Need to have Population to remove it
		if (!bAdd)
		{
			--iPopulation;

			if (iPopulation < GC.getDefineINT("INITIAL_CITY_POPULATION") + GC.getEraInfo(GC.getGameINLINE().getStartEra()).getFreePopulation())
			{
				return -1;
			}
		}

		iCost = (getGrowthThreshold(iPopulation) * GC.getDefineINT("ADVANCED_START_POPULATION_COST")) / 100;

		// Increase cost if the XML defines that additional Pop will cost more
		if (0 != GC.getDefineINT("ADVANCED_START_POPULATION_COST_INCREASE"))
		{
			--iPopulation;

			if (iPopulation > 0)
			{
				iCost *= 100 + GC.getDefineINT("ADVANCED_START_POPULATION_COST_INCREASE") * iPopulation;
				iCost /= 100;
			}
		}
	}

	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing Culture
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartCultureCost(bool bAdd, CvCity* pCity)
{
	if (0 == getNumCities())
	{
		return -1;
	}

	int iCost = GC.getDefineINT("ADVANCED_START_CULTURE_COST");
	if (iCost < 0)
	{
		return -1;
	}

	if (NULL != pCity)
	{
		if (pCity->getOwnerINLINE() != getID())
		{
			return -1;
		}

		// Need to have enough culture to remove it
		if (!bAdd)
		{
			if (pCity->getCultureLevel() <= 0)
			{
				return -1;
			}
		}

		int iCulture;
		if (bAdd)
		{
			iCulture = pCity->getCultureThreshold((CultureLevelTypes)(pCity->getCultureLevel() + 1)) - pCity->getCulture(getID());
		}
		else
		{
			iCulture = pCity->getCulture(getID()) - pCity->getCultureThreshold((CultureLevelTypes)(pCity->getCultureLevel() - 1));
		}

		iCost *= iCulture;
		iCost *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
		iCost /= 10000;
	}

	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing a Building from a city
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartBuildingCost(BuildingTypes eBuilding, bool bAdd, CvCity* pCity)
{
	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	if (0 == getNumCities())
	{
		return -1;
	}

	int iNumBuildingType = 0;

	int iCost = 0;
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		iCost += getYieldProductionNeeded(eBuilding, (YieldTypes) iYield);
	}

	iCost *= kBuilding.getAdvancedStartCost();
	if (iCost < 0)
	{
		return -1;
	}
	iCost /= 100;

	if (kBuilding.getFreeStartEra() != NO_ERA && GC.getGameINLINE().getStartEra() >=  kBuilding.getFreeStartEra())
	{
		// you get this building for free
		return -1;
	}

	if (NULL == pCity)
	{
		if (bAdd)
		{
			bool bValid = false;
			int iLoop;
			for (CvCity* pLoopCity = firstCity(&iLoop); NULL != pLoopCity; pLoopCity = nextCity(&iLoop))
			{
				if (pLoopCity->canConstruct(eBuilding))
				{
					bValid = true;
					break;
				}
			}

			if (!bValid)
			{
				return -1;
			}
		}
	}
	if (NULL != pCity)
	{
		if (pCity->getOwnerINLINE() != getID())
		{
			return -1;
		}

		iCost *= 100;
		iCost /= std::max(1, 100 + pCity->getProductionModifier(eBuilding));

		if (bAdd)
		{
			if (!pCity->canConstruct(eBuilding, true, false, false))
			{
				return -1;
			}
		}
		else
		{
			if (!pCity->isHasRealBuilding(eBuilding))
			{
				return -1;
			}

			// Check other buildings in this city and make sure none of them require this one
			for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
			{
				BuildingTypes eBuildingLoop = (BuildingTypes) iBuildingLoop;
				if (pCity->isHasRealBuilding(eBuildingLoop))
				{
					if (GC.getBuildingInfo(eBuildingLoop).isBuildingClassNeededInCity(kBuilding.getBuildingClassType()))
					{
						return -1;
					}
				}
			}
		}
	}

	// Increase cost if the XML defines that additional Buildings will cost more
	if (0 != GC.getBuildingInfo(eBuilding).getAdvancedStartCostIncrease())
	{
		iNumBuildingType = countNumBuildings(eBuilding);

		if (!bAdd)
		{
			--iNumBuildingType;
		}

		if (iNumBuildingType > 0)
		{
			iCost *= 100 + GC.getBuildingInfo(eBuilding).getAdvancedStartCostIncrease() * std::max(0, iNumBuildingType - getNumCities());
			iCost /= 100;
		}
	}

	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing Route
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartRouteCost(RouteTypes eRoute, bool bAdd, CvPlot* pPlot)
{
	if (0 == getNumCities())
	{
		return -1;
	}

	if (eRoute == NO_ROUTE)
	{
		return -1;
	}

	//WTP, Nightinggale, Large Rivers - START
	if (GC.getRouteInfo(eRoute).isGraphicalOnly())
	{
		return -1;
	}
	//WTP, Nightinggale, Large Rivers - END

	int iNumRoutes = 0;

	int iCost = GC.getRouteInfo(eRoute).getAdvancedStartCost();

	// This denotes cities may not be purchased through Advanced Start
	if (iCost < 0)
	{
		return -1;
	}

	iCost *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
	iCost /= 100;

	// No invalid plots!
	if (pPlot != NULL)
	{
		if (pPlot->isCity())
		{
			return -1;
		}

		if (bAdd)
		{
			if (pPlot->isImpassable() || pPlot->isWater())
			{
				return -1;
			}
			// Can't place twice
			if (pPlot->getRouteType() == eRoute)
			{
				return -1;
			}
		}
		else
		{
			// Need Route to remove it
			if (pPlot->getRouteType() != eRoute)
			{
				return -1;
			}
		}

		// Must be owned by me
		if (pPlot->getOwner() != getID())
		{
			return -1;
		}
	}
	// Increase cost if the XML defines that additional units will cost more
	if (0 != GC.getRouteInfo(eRoute).getAdvancedStartCostIncrease())
	{
		int iPlotLoop = 0;
		CvPlot* pPlot;

		for (iPlotLoop = 0; iPlotLoop < GC.getMapINLINE().numPlots(); iPlotLoop++)
		{
			pPlot = GC.getMapINLINE().plotByIndex(iPlotLoop);

			if (pPlot->getRouteType() == eRoute)
			{
				++iNumRoutes;
			}
		}

		if (!bAdd)
		{
			--iNumRoutes;
		}

		if (iNumRoutes > 0)
		{
			iCost *= 100 + GC.getRouteInfo(eRoute).getAdvancedStartCostIncrease() * iNumRoutes;
			iCost /= 100;
		}
	}

	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing Improvement
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartImprovementCost(ImprovementTypes eImprovement, bool bAdd, CvPlot* pPlot)
{
	if (eImprovement == NO_IMPROVEMENT)
	{
		return -1;
	}

	if (0 == getNumCities())
	{
		return -1;
	}

	int iNumImprovements = 0;
	int iCost = GC.getImprovementInfo(eImprovement).getAdvancedStartCost();

		// This denotes cities may not be purchased through Advanced Start
	if (iCost < 0)
	{
		return -1;
	}

	iCost *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
	iCost /= 100;

	// Can this Improvement be on our plot?
	if (pPlot != NULL)
	{
		if (bAdd)
		{
			// Valid Plot
			if (!pPlot->canHaveImprovement(eImprovement, getTeam(), false))
			{
				return -1;
			}

			bool bValid = false;

			for (int iI = 0; iI < GC.getNumBuildInfos(); ++iI)
			{
				CvBuildInfo& kBuild = GC.getBuildInfo((BuildTypes)iI);
				ImprovementTypes eLoopImprovement = ((ImprovementTypes)(kBuild.getImprovement()));
				if (eImprovement == eLoopImprovement && canBuild(pPlot, (BuildTypes)iI))
				{
					bValid = true;
					FeatureTypes eFeature = pPlot->getFeatureType();
					if (NO_FEATURE != eFeature && kBuild.isFeatureRemove(eFeature))
					{
						iCost += GC.getFeatureInfo(eFeature).getAdvancedStartRemoveCost();
					}
					break;
				}
			}
			if (!bValid)
			{
				return -1;
			}
			// Can't place twice
			if (pPlot->getImprovementType() == eImprovement)
			{
				return -1;
			}
		}
		else
		{
			// Need this improvement in order to remove it
			if (pPlot->getImprovementType() != eImprovement)
			{
				return -1;
			}
		}
		// Must be owned by me
		if (pPlot->getOwner() != getID())
		{
			return -1;
		}
	}
	// Increase cost if the XML defines that additional units will cost more
	if (0 != GC.getImprovementInfo(eImprovement).getAdvancedStartCostIncrease())
	{
		int iPlotLoop = 0;
		CvPlot* pPlot;
		for (iPlotLoop = 0; iPlotLoop < GC.getMapINLINE().numPlots(); iPlotLoop++)
		{
			pPlot = GC.getMapINLINE().plotByIndex(iPlotLoop);
			if (pPlot->getImprovementType() == eImprovement)
			{
				++iNumImprovements;
			}
		}
		if (!bAdd)
		{
			--iNumImprovements;
		}
		if (iNumImprovements > 0)
		{
			iCost *= 100 + GC.getImprovementInfo(eImprovement).getAdvancedStartCostIncrease() * iNumImprovements;
			iCost /= 100;
		}
	}

	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing Visibility
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartVisibilityCost(bool bAdd, CvPlot* pPlot)
{
	if (0 == getNumCities())
	{
		return -1;
	}

	int iNumVisiblePlots = 0;
	int iCost = GC.getDefineINT("ADVANCED_START_VISIBILITY_COST");

	// This denotes Visibility may not be purchased through Advanced Start
	if (iCost == -1)
	{
		return -1;
	}

	// Valid Plot?
	if (pPlot != NULL)
	{
		if (bAdd)
		{
			if (pPlot->isRevealed(getTeam(), false))
			{
				return -1;
			}
			if (!pPlot->isAdjacentRevealed(getTeam()))
			{
				return -1;
			}
		}
		else
		{
			if (!pPlot->isRevealed(getTeam(), false))
			{
				return -1;
			}
		}
	}

	// Increase cost if the XML defines that additional units will cost more
	if (0 != GC.getDefineINT("ADVANCED_START_VISIBILITY_COST_INCREASE"))
	{
		int iPlotLoop = 0;
		CvPlot* pPlot;

		for (iPlotLoop = 0; iPlotLoop < GC.getMapINLINE().numPlots(); iPlotLoop++)
		{
			pPlot = GC.getMapINLINE().plotByIndex(iPlotLoop);

			if (pPlot->isRevealed(getTeam(), false))
			{
				++iNumVisiblePlots;
			}
		}

		if (!bAdd)
		{
			--iNumVisiblePlots;
		}

		if (iNumVisiblePlots > 0)
		{
			iCost *= 100 + GC.getDefineINT("ADVANCED_START_VISIBILITY_COST_INCREASE") * iNumVisiblePlots;
			iCost /= 100;
		}
	}

	return iCost;
}

void CvPlayer::doWarnings()
{
	//update enemy unit in your territory glow
	int iLoop;
	for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		//update glow
		gDLL->getEntityIFace()->updateEnemyGlow(pLoopUnit->getUnitEntity());
	}

	//update enemy units close to your territory
	int iMaxCount = range(((getNumCities() + 4) / 7), 2, 5);
	for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		if (iMaxCount == 0)
		{
			break;
		}

		CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

		if (pLoopPlot->isAdjacentPlayer(getID()))
		{
			if (!(pLoopPlot->isCity()))
			{
				if (pLoopPlot->isVisible(getTeam(), false))
				{
					CvUnit *pUnit = pLoopPlot->getVisibleEnemyDefender(getID());
					if (pUnit != NULL)
					{
						CvCity* pNearestCity = GC.getMapINLINE().findCity(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), getID(), NO_TEAM, !(pLoopPlot->isWater()));
						if (pNearestCity != NULL)
						{
							// R&R, ray, changes to Wild Animals - START
							// Erik: Make sure that the unit is actually an animal (and not a pirate) when giving the warning about
							// animals being spotted
							if (pUnit->isBarbarian() && pUnit->getUnitInfo().isAnimal())
							{
								CvWString szBuffer = gDLL->getText("TXT_KEY_ANIMALS_SPOTTED", pNearestCity->getNameKey());
								gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_INFO, pUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), true, true);
								iMaxCount--;
							}
							else
							{
								CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_ENEMY_TROOPS_SPOTTED", pNearestCity->getNameKey());
								gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_ENEMY_TROOPS", MESSAGE_TYPE_INFO, pUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), true, true);
								iMaxCount--;
							}
							// R&R, ray, changes to Wild Animals - END
						}
					}
				}
			}
		}
	}
}


void CvPlayer::processCivics(CivicTypes eCivic, int iChange)
{
	CvCivicInfo& kCivicInfo = GC.getCivicInfo(eCivic);

	CivEffect()->applyCivEffect(kCivicInfo.getCivEffect(), iChange);

	changeGreatGeneralRateModifier(kCivicInfo.getGreatGeneralRateModifier() * iChange);
	changeDomesticGreatGeneralRateModifier(kCivicInfo.getDomesticGreatGeneralRateModifier() * iChange);
	changeFreeExperience(kCivicInfo.getFreeExperience() * iChange);
	changeWorkerSpeedModifier(kCivicInfo.getWorkerSpeedModifier() * iChange);
	changeImprovementUpgradeRateModifier(kCivicInfo.getImprovementUpgradeRateModifier() * iChange);
	changeMilitaryProductionModifier(kCivicInfo.getMilitaryProductionModifier() * iChange);
	changeExpInBorderModifier(kCivicInfo.getExpInBorderModifier() * iChange);
	changeNativeCombatModifier(kCivicInfo.getNativeCombatModifier() * iChange);
	changeDominateNativeBordersCount(kCivicInfo.isDominateNativeBorders() ? iChange : 0);
	changeRevolutionEuropeTradeCount(kCivicInfo.isRevolutionEuropeTrade() ? iChange : 0);
	setFatherPointMultiplier(getFatherPointMultiplier() + kCivicInfo.getFatherPointModifier() * iChange);

	if (kCivicInfo.getImmigrationConversion() != NO_YIELD)
	{
		if (iChange > 0)
		{
			setImmigrationConversion((YieldTypes) kCivicInfo.getImmigrationConversion());
		}
		else
		{
			setImmigrationConversion(YIELD_CROSSES);
		}
	}

	if (kCivicInfo.getNativeAttitudeChange() != 0)
	{
		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes) iPlayer);

			if (kLoopPlayer.isAlive() && kLoopPlayer.isNative())
			{
				kLoopPlayer.AI_changeAttitudeExtra(getID(), kCivicInfo.getNativeAttitudeChange() * iChange);

				if (iChange > 0 && kCivicInfo.getNativeAttitudeChange() > 0)
				{
					GET_TEAM(getTeam()).makePeace(kLoopPlayer.getTeam());
				}
			}
		}
	}

	for (int iProfession = 0; iProfession < GC.getNumProfessionInfos(); ++iProfession)
	{
		changeProfessionCombatChange((ProfessionTypes) iProfession, kCivicInfo.getProfessionCombatChange(iProfession) * iChange);
	}

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		changeYieldRateModifier(((YieldTypes)iI), (kCivicInfo.getYieldModifier(iI) * iChange));
		changeCapitalYieldRateModifier(((YieldTypes)iI), (kCivicInfo.getCapitalYieldModifier(iI) * iChange));
	}

	for (int iI = 0; iI < GC.getNumHurryInfos(); iI++)
	{
		changeHurryCount(((HurryTypes)iI), ((kCivicInfo.isHurry(iI)) ? iChange : 0));
	}

	for (int iI = 0; iI < GC.getNumSpecialBuildingInfos(); iI++)
	{
		changeSpecialBuildingNotRequiredCount(((SpecialBuildingTypes)iI), ((kCivicInfo.isSpecialBuildingNotRequired(iI)) ? iChange : 0));
	}

	for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	{
		for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			changeImprovementYieldChange(((ImprovementTypes)iI), ((YieldTypes)iJ), (kCivicInfo.getImprovementYieldChanges(iI, iJ) * iChange));
		}
	}

	for (int i = 0; i < kCivicInfo.getNumFreeUnitClasses(); ++i)
	{
		UnitTypes eUnit = (UnitTypes) GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(kCivicInfo.getFreeUnitClass(i));
		if (eUnit != NO_UNIT)
		{
			int iLoop;
			for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				if (iChange > 0)
				{
					int iTargetPercent = pLoopCity->getRebelPercent();
					for (int j = 0; j < iChange; ++j)
					{
						CvUnit* pUnit = initUnit(eUnit, GC.getUnitInfo(eUnit).getDefaultProfession(), pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE());
						if (pUnit != NULL)
						{
							pLoopCity->addPopulationUnit(pUnit, NO_PROFESSION);
						}
					}
					if (pLoopCity->getRebelPercent() < iTargetPercent)
					{
						pLoopCity->setRebelSentiment(iTargetPercent * pLoopCity->getPopulation());
					}
				}
				else if (iChange < 0)
				{
					int iRemoved = 0;

					// remove proper unit from city
					for (int j = 0; j < -iChange; ++j)
					{
						CvUnit* pUnit = pLoopCity->removeUnitType(eUnit, GC.getCivilizationInfo(getCivilizationType()).getDefaultProfession());
						if (pUnit != NULL)
						{
							pUnit->kill(false);
							++iRemoved;
						}
					}

					// remove proper unit from field
					for (int j = iRemoved; j < -iChange; ++j)
					{
						int iUnitLoop;
						for (CvUnit* pUnit = firstUnit(&iUnitLoop); pUnit != NULL; pUnit = nextUnit(&iLoop))
						{
							if (pUnit->getUnitType() == eUnit)
							{
								pUnit->kill(false);
								++iRemoved;
								break;
							}
						}
					}

					// remove any city population
					int iRemaining = -iChange - iRemoved;
					if (iRemaining > 0 && pLoopCity->getPopulation() > iRemaining)
					{
						pLoopCity->changePopulation(-iRemaining);
						iRemoved += iRemaining;
					}

					// remove any unit from field
					for (int j = iRemoved; j < -iChange; ++j)
					{
						int iUnitLoop;
						for (CvUnit* pUnit = firstUnit(&iUnitLoop); pUnit != NULL; pUnit = nextUnit(&iLoop))
						{
							pUnit->kill(false);
							++iRemoved;
							break;
						}
					}
				}
			}
		}
	}
}

void CvPlayer::showMissedMessages()
{
	CvMessageQueue::iterator it = m_listGameMessages.begin();
	while (it != m_listGameMessages.end())
	{
		CvTalkingHeadMessage& msg = *it;
		if (!msg.getShown())
		{
			msg.setShown(true);
			gDLL->getInterfaceIFace()->showMessage(msg);
		}
		++it;
	}
}

bool CvPlayer::isPbemNewTurn() const
{
	return m_bPbemNewTurn;
}

void CvPlayer::setPbemNewTurn(bool bNew)
{
	m_bPbemNewTurn = bNew;
}

void CvPlayer::createGreatGeneral(UnitTypes eGreatGeneralUnit, bool bIncrementExperience, int iX, int iY)
{
	CvUnit* pGreatUnit = initUnit(eGreatGeneralUnit, GC.getUnitInfo(eGreatGeneralUnit).getDefaultProfession(), iX, iY);
	if (NULL == pGreatUnit)
	{
		FAssert(false);
		return;
	}

	if (bIncrementExperience)
	{
		incrementGreatGeneralsCreated();

		changeGreatGeneralsThresholdModifier(GC.getDefineINT("GREAT_GENERALS_THRESHOLD_INCREASE"));

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getTeam())
			{
				GET_PLAYER((PlayerTypes)iI).changeGreatGeneralsThresholdModifier(GC.getDefineINT("GREAT_GENERALS_THRESHOLD_INCREASE_TEAM"));
			}
		}
	}


	CvPlot* pPlot = GC.getMapINLINE().plot(iX, iY);
	CvCity* pCity = pPlot->getPlotCity();
	CvWString szReplayMessage;

	if (pPlot)
	{
		// TAC, ray, Great General Names - START
		if (pCity)
		{
			CvWString szCity;
			szCity.Format(L"%s (%s)", pCity->getName().GetCString(), GET_PLAYER(pCity->getOwnerINLINE()).getName());
			//szReplayMessage = gDLL->getText("TXT_KEY_MISC_GP_BORN", pGreatUnit->getName().GetCString(), szCity.GetCString());
			szReplayMessage = gDLL->getText("TXT_KEY_MISC_GP_BORN", pGreatUnit->getNameKey(), pCity->getNameKey());
		}
		else
		{
			//szReplayMessage = gDLL->getText("TXT_KEY_MISC_GP_BORN_FIELD", pGreatUnit->getName().GetCString());
			szReplayMessage = gDLL->getText("TXT_KEY_MISC_GP_BORN_FIELD", pGreatUnit->getNameKey());
		}
		// TAC, ray, Great General Names - END
		GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szReplayMessage, iX, iY, (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"));
	}

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (pPlot->isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
			{
				gDLL->getInterfaceIFace()->addMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szReplayMessage, "AS2D_UNIT_GREATPEOPLE", MESSAGE_TYPE_MAJOR_EVENT, pGreatUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"), iX, iY, true, true);
			}
			else
			{
				CvWString szMessage = gDLL->getText("TXT_KEY_MISC_GP_BORN_SOMEWHERE", pGreatUnit->getName().GetCString());
				gDLL->getInterfaceIFace()->addMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szMessage, "AS2D_UNIT_GREATPEOPLE", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"));
			}
		}
	}
}

// R&R, ray, Great Admirals - START
void CvPlayer::createGreatAdmiral(UnitTypes eGreatAdmirallUnit, bool bIncrementExperience, int iX, int iY)
{
	CvUnit* pGreatUnit = initUnit(eGreatAdmirallUnit, GC.getUnitInfo(eGreatAdmirallUnit).getDefaultProfession(), iX, iY);
	if (NULL == pGreatUnit)
	{
		FAssert(false);
		return;
	}

	if (bIncrementExperience)
	{
		incrementGreatAdmiralsCreated();

		changeGreatAdmiralsThresholdModifier(GC.getDefineINT("GREAT_ADMIRALS_THRESHOLD_INCREASE"));

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getTeam())
			{
				GET_PLAYER((PlayerTypes)iI).changeGreatAdmiralsThresholdModifier(GC.getDefineINT("GREAT_ADMIRALS_THRESHOLD_INCREASE_TEAM"));
			}
		}
	}


	CvPlot* pPlot = GC.getMapINLINE().plot(iX, iY);
	CvCity* pCity = pPlot->getPlotCity();
	CvWString szReplayMessage;

	if (pPlot)
	{
		// TAC, ray, Great General Names - START
		if (pCity)
		{
			CvWString szCity;
			szCity.Format(L"%s (%s)", pCity->getName().GetCString(), GET_PLAYER(pCity->getOwnerINLINE()).getName());
			//szReplayMessage = gDLL->getText("TXT_KEY_MISC_GP_BORN", pGreatUnit->getName().GetCString(), szCity.GetCString());
			szReplayMessage = gDLL->getText("TXT_KEY_MISC_GP_BORN", pGreatUnit->getNameKey(), pCity->getNameKey());
		}
		else
		{
			//szReplayMessage = gDLL->getText("TXT_KEY_MISC_GP_BORN_FIELD", pGreatUnit->getName().GetCString());
			szReplayMessage = gDLL->getText("TXT_KEY_MISC_GP_BORN_FIELD", pGreatUnit->getNameKey());
		}
		// TAC, ray, Great General Names - END
		GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szReplayMessage, iX, iY, (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"));
	}

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (pPlot->isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
			{
				gDLL->getInterfaceIFace()->addMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szReplayMessage, "AS2D_UNIT_GREATPEOPLE", MESSAGE_TYPE_MAJOR_EVENT, pGreatUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"), iX, iY, true, true);
			}
			else
			{
				CvWString szMessage = gDLL->getText("TXT_KEY_MISC_GP_BORN_SOMEWHERE", pGreatUnit->getName().GetCString());
				gDLL->getInterfaceIFace()->addMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szMessage, "AS2D_UNIT_GREATPEOPLE", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"));
			}
		}
	}
}
// R&R, ray, Great Admirals - END

const EventTriggeredData* CvPlayer::getEventOccured(EventTypes eEvent) const
{
	FAssert(eEvent >= 0 && eEvent < GC.getNumEventInfos());

	CvEventMap::const_iterator it = m_mapEventsOccured.find(eEvent);

	if (it == m_mapEventsOccured.end())
	{
		return NULL;
	}

	return &((*it).second);
}

bool CvPlayer::isTriggerFired(EventTriggerTypes eEventTrigger) const
{
	return (std::find(m_triggersFired.begin(), m_triggersFired.end(), eEventTrigger) != m_triggersFired.end());
}

void CvPlayer::resetEventOccured(EventTypes eEvent, bool bAnnounce)
{
	FAssert(eEvent >= 0 && eEvent < GC.getNumEventInfos());

	CvEventMap::iterator it = m_mapEventsOccured.find(eEvent);

	if (it != m_mapEventsOccured.end())
	{
		expireEvent((*it).first, (*it).second, bAnnounce);
		m_mapEventsOccured.erase(it);
	}
}

void CvPlayer::setEventOccured(EventTypes eEvent, const EventTriggeredData& kEventTriggered, bool bOthers)
{
	FAssert(eEvent >= 0 && eEvent < GC.getNumEventInfos());

	m_mapEventsOccured[eEvent] = kEventTriggered;

	if (GC.getEventInfo(eEvent).isQuest())
	{
		CvWStringBuffer szMessageBuffer;
		szMessageBuffer.append(GC.getEventInfo(eEvent).getDescription());
		GAMETEXT.setEventHelp(szMessageBuffer, eEvent, kEventTriggered.getID(), getID());
		gDLL->getInterfaceIFace()->addQuestMessage(getID(), szMessageBuffer.getCString(), kEventTriggered.getID());
	}

	if (bOthers)
	{
		if (GC.getEventInfo(eEvent).isGlobal())
		{
			for (int i = 0; i < MAX_PLAYERS; i++)
			{
				if (i != getID())
				{
					GET_PLAYER((PlayerTypes)i).setEventOccured(eEvent, kEventTriggered, false);
				}
			}
		}
		else if (GC.getEventInfo(eEvent).isTeam())
		{
			for (int i = 0; i < MAX_PLAYERS; i++)
			{
				if (i != getID() && getTeam() == GET_PLAYER((PlayerTypes)i).getTeam())
				{
					GET_PLAYER((PlayerTypes)i).setEventOccured(eEvent, kEventTriggered, false);
				}
			}
		}
	}
}


const EventTriggeredData* CvPlayer::getEventCountdown(EventTypes eEvent) const
{
	FAssert(eEvent >= 0 && eEvent < GC.getNumEventInfos());

	CvEventMap::const_iterator it = m_mapEventCountdown.find(eEvent);

	if (it == m_mapEventCountdown.end())
	{
		return NULL;
	}

	return &((*it).second);
}

void CvPlayer::setEventCountdown(EventTypes eEvent, const EventTriggeredData& kEventTriggered)
{
	FAssert(eEvent >= 0 && eEvent < GC.getNumEventInfos());

	m_mapEventCountdown[eEvent] = kEventTriggered;
}

void CvPlayer::resetEventCountdown(EventTypes eEvent)
{
	FAssert(eEvent >= 0 && eEvent < GC.getNumEventInfos());

	CvEventMap::iterator it = m_mapEventCountdown.find(eEvent);

	if (it != m_mapEventCountdown.end())
	{
		m_mapEventCountdown.erase(it);
	}
}


void CvPlayer::resetTriggerFired(EventTriggerTypes eTrigger)
{
	std::vector<EventTriggerTypes>::iterator it = std::find(m_triggersFired.begin(), m_triggersFired.end(), eTrigger);

	if (it != m_triggersFired.end())
	{
		m_triggersFired.erase(it);
	}
}

void CvPlayer::setTriggerFired(const EventTriggeredData& kTriggeredData, bool bOthers, bool bAnnounce)
{
	FAssert(kTriggeredData.m_eTrigger >= 0 && kTriggeredData.m_eTrigger < GC.getNumEventTriggerInfos());

	CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(kTriggeredData.m_eTrigger);

	if (!isTriggerFired(kTriggeredData.m_eTrigger))
	{
		m_triggersFired.push_back(kTriggeredData.m_eTrigger);

		if (bOthers)
		{
			if (kTrigger.isGlobal())
			{
				for (int i = 0; i < MAX_PLAYERS; i++)
				{
					if (i != getID())
					{
						GET_PLAYER((PlayerTypes)i).setTriggerFired(kTriggeredData, false, false);
					}
				}
			}
			else if (kTrigger.isTeam())
			{
				for (int i = 0; i < MAX_PLAYERS; i++)
				{
					if (i != getID() && getTeam() == GET_PLAYER((PlayerTypes)i).getTeam())
					{
						GET_PLAYER((PlayerTypes)i).setTriggerFired(kTriggeredData, false, false);
					}
				}
			}
		}
	}

	if (!isEmpty(kTrigger.getPythonCallback()))
	{
		long lResult;

		CyArgsList argsList;
		argsList.add(gDLL->getPythonIFace()->makePythonObject(&kTriggeredData));

		gDLL->getPythonIFace()->callFunction(PYRandomEventModule, kTrigger.getPythonCallback(), argsList.makeFunctionArgs(), &lResult);
	}

	if (bAnnounce && !kTrigger.isTutorial())
	{
		CvPlot* pPlot = GC.getMapINLINE().plot(kTriggeredData.m_iPlotX, kTriggeredData.m_iPlotY);

		if (!kTriggeredData.m_szGlobalText.empty())
		{
			for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
			{
				CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);

				if (kLoopPlayer.isAlive())
				{
					if (GET_TEAM(kLoopPlayer.getTeam()).isHasMet(getTeam()) && (NO_PLAYER == kTriggeredData.m_eOtherPlayer || GET_TEAM(GET_PLAYER(kTriggeredData.m_eOtherPlayer).getTeam()).isHasMet(getTeam())))
					{
						bool bShowPlot = kTrigger.isShowPlot();

						if (bShowPlot)
						{
							if (kLoopPlayer.getTeam() != getTeam())
							{
								if (NULL == pPlot || !pPlot->isRevealed(kLoopPlayer.getTeam(), false))
								{
									bShowPlot = false;
								}
							}
						}

						if (bShowPlot)
						{
							gDLL->getInterfaceIFace()->addMessage((PlayerTypes)iPlayer, false, GC.getEVENT_MESSAGE_TIME(), kTriggeredData.m_szGlobalText, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), kTriggeredData.m_iPlotX, kTriggeredData.m_iPlotY, true, true);
						}
						else
						{
							gDLL->getInterfaceIFace()->addMessage((PlayerTypes)iPlayer, false, GC.getEVENT_MESSAGE_TIME(), kTriggeredData.m_szGlobalText, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT);
						}
					}
				}
			}

			GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), kTriggeredData.m_szGlobalText, kTriggeredData.m_iPlotX, kTriggeredData.m_iPlotY, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
		}
		else if (!kTriggeredData.m_szText.empty())
		{
			if (kTrigger.isShowPlot() && NULL != pPlot && pPlot->isRevealed(getTeam(), false))
			{
				gDLL->getInterfaceIFace()->addMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), kTriggeredData.m_szText, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), kTriggeredData.m_iPlotX, kTriggeredData.m_iPlotY, true, true);
			}
			else
			{
				gDLL->getInterfaceIFace()->addMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), kTriggeredData.m_szText, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"));
			}
		}
	}

	if (kTrigger.isTutorial())
	{
		gDLL->getInterfaceIFace()->addTutorialMessage(getID(), kTriggeredData.m_szText);
	}
}

EventTriggeredData* CvPlayer::initTriggeredData(EventTriggerTypes eEventTrigger, bool bFire, int iCityId, int iPlotX, int iPlotY, PlayerTypes eOtherPlayer, int iOtherPlayerCityId, int iUnitId, BuildingTypes eBuilding)
{

	CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(eEventTrigger);

	CvCity* pCity = getCity(iCityId);
	CvCity* pOtherPlayerCity = NULL;
	if (NO_PLAYER != eOtherPlayer)
	{
		pOtherPlayerCity = GET_PLAYER(eOtherPlayer).getCity(iOtherPlayerCityId);
	}
	CvPlot* pPlot = GC.getMapINLINE().plot(iPlotX, iPlotY);
	CvUnit* pUnit = getUnit(iUnitId);

	std::vector<CvPlot*> apPlots;
	bool bPickPlot = ::isPlotEventTrigger(eEventTrigger);
	if (kTrigger.isPickCity())
	{
		if (NULL == pCity)
		{
			pCity = pickTriggerCity(eEventTrigger);
		}

		if (NULL != pCity)
		{
			if (bPickPlot)
			{
				for (int iPlot = 0; iPlot < NUM_CITY_PLOTS; ++iPlot)
				{
					if (CITY_HOME_PLOT != iPlot)
					{
						CvPlot* pLoopPlot = pCity->getCityIndexPlot(iPlot);

						if (NULL != pLoopPlot)
						{
							if (pLoopPlot->canTrigger(eEventTrigger, getID()))
							{
								apPlots.push_back(pLoopPlot);
							}
						}
					}
				}
			}
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		const InfoArray<BuildingClassTypes>& ReqBuildings = kTrigger.getBuildingsRequired();
		if (kTrigger.getNumBuildings() > 0 && ReqBuildings.getLength() > 0)
		{
			int iFoundValid = 0;

			for (int i = 0; i < ReqBuildings.getLength(); ++i)
			{
				//if (kTrigger.getBuildingRequired(i) != NO_BUILDINGCLASS)
				{
					iFoundValid += getBuildingClassCount(ReqBuildings.getBuildingClass(i));
				}
			}

			if (iFoundValid < kTrigger.getNumBuildings())
			{
				return NULL;
			}
		}

		if (kTrigger.getMinPopulation() > 0)
		{
			if (getTotalPopulation() < kTrigger.getMinPopulation())
			{
				return NULL;
			}
		}

		if (kTrigger.getMaxPopulation() > 0)
		{
			if (getTotalPopulation() > kTrigger.getMaxPopulation())
			{
				return NULL;
			}
		}

		if (bPickPlot)
		{
			for (int iPlot = 0; iPlot < GC.getMapINLINE().numPlotsINLINE(); ++iPlot)
			{
				CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iPlot);

				if (pLoopPlot->canTrigger(eEventTrigger, getID()))
				{
					apPlots.push_back(pLoopPlot);
				}
			}
		}
	}

	if (NULL == pPlot)
	{
		if (apPlots.size() > 0)
		{
			int iChosen = GC.getGameINLINE().getSorenRandNum(apPlots.size(), "Event pick plot");
			pPlot = apPlots[iChosen];

			if (NULL == pCity)
			{
				pCity = GC.getMapINLINE().findCity(pPlot->getX_INLINE(), pPlot->getY_INLINE(), getID(), NO_TEAM, false);
			}
		}
		else
		{
			if (bPickPlot)
			{
				return NULL;
			}

			if (NULL != pCity)
			{
				pPlot = pCity->plot();
			}
		}
	}

	if (kTrigger.getNumBuildings() > 0)
	{
		if (NULL != pCity && NO_BUILDING == eBuilding)
		{
			const InfoArray<BuildingClassTypes>& ReqBuildings = kTrigger.getBuildingsRequired();
			std::vector<BuildingTypes> aeBuildings;
			for (int i = 0; i < ReqBuildings.getLength(); ++i)
			{
				//if (kTrigger.getBuildingRequired(i) != NO_BUILDINGCLASS)
				{
					BuildingTypes eTestBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(ReqBuildings.getBuildingClass(i));
					if (NO_BUILDING != eTestBuilding && pCity->isHasRealBuilding(eTestBuilding))
					{
						aeBuildings.push_back(eTestBuilding);
					}
				}
			}

			if (aeBuildings.size() > 0)
			{
				int iChosen = GC.getGameINLINE().getSorenRandNum(aeBuildings.size(), "Event pick building");
				eBuilding = aeBuildings[iChosen];
			}
			else
			{
				return NULL;
			}
		}
	}

	if (NULL == pUnit)
	{
		pUnit = pickTriggerUnit(eEventTrigger, pPlot, bPickPlot);
	}

	// WTP, ray, fix for allowing some nice events - START
	// fixed to trigger some nice City events with still using NumUnits
	// if (NULL == pUnit && kTrigger.getNumUnits() > 0)
	if (NULL == pUnit && kTrigger.getNumUnits() > 0 && kTrigger.isUnitsOnPlot())
	{
		return NULL;
	}

	else if (kTrigger.getNumUnits() > 0)
	{
		int iNumUnits = 0;	
		
		const InfoArray<UnitClassTypes>& ReqUnits = kTrigger.getUnitsRequired();
		for (int i = 0; i < ReqUnits.getLength(); ++i)
		{
			int iNumUnitsFound = getUnitClassCount(ReqUnits.getUnitClass(i));
			iNumUnits = iNumUnits + iNumUnitsFound;					
		}
				
		if (iNumUnits < kTrigger.getNumUnits())
		{
			return NULL;
		}
	}
	// WTP, ray, fix for allowing some nice events - END


	if (NULL == pPlot && NULL != pUnit)
	{
		pPlot = pUnit->plot();
	}

	if (NULL == pPlot && bPickPlot)
	{
		return NULL;
	}

	if (kTrigger.getNumUnitsGlobal() > 0)
	{
		int iNumUnits = 0;
		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);

			if (kLoopPlayer.isAlive())
			{
				int iLoop;
				for (CvUnit* pLoopUnit = kLoopPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kLoopPlayer.nextUnit(&iLoop))
				{
					if (MIN_INT != pLoopUnit->getTriggerValue(eEventTrigger, pPlot, true))
					{
						++iNumUnits;
					}
				}
			}
		}

		if (iNumUnits < kTrigger.getNumUnitsGlobal())
		{
			return NULL;
		}
	}

	if (kTrigger.getNumBuildingsGlobal() > 0)
	{
		int iNumBuildings = 0;
		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);

			if (kLoopPlayer.isAlive())
			{
				const InfoArray<BuildingClassTypes>& ReqBuildings = kTrigger.getBuildingsRequired();
				for (int i = 0; i < ReqBuildings.getLength(); ++i)
				{
					//if (kTrigger.getBuildingRequired(i) != NO_BUILDINGCLASS)
					{
						iNumBuildings += getBuildingClassCount(ReqBuildings.getBuildingClass(i));
					}
				}
			}
		}

		if (iNumBuildings < kTrigger.getNumBuildingsGlobal())
		{
			return NULL;
		}
	}

	if (kTrigger.isPickPlayer())
	{
		std::vector<PlayerTypes> aePlayers;
		std::vector<CvCity*> apCities;

		if (NO_PLAYER == eOtherPlayer)
		{
			for (int i = 0; i < MAX_PLAYERS; i++)
			{
				if (GET_PLAYER((PlayerTypes)i).canTrigger(eEventTrigger, getID()))
				{
					if (kTrigger.isPickOtherPlayerCity())
					{
						CvCity* pBestCity = NULL;

						if (NULL != pCity)
						{
							pBestCity = GC.getMapINLINE().findCity(pCity->getX_INLINE(), pCity->getY_INLINE(), (PlayerTypes)i);
						}
						else
						{
							pBestCity = GET_PLAYER((PlayerTypes)i).pickTriggerCity(eEventTrigger);
						}

						if (NULL != pBestCity)
						{
							apCities.push_back(pBestCity);
							aePlayers.push_back((PlayerTypes)i);
						}
					}
					else
					{
						apCities.push_back(NULL);
						aePlayers.push_back((PlayerTypes)i);
					}
				}
			}

			if (aePlayers.size() > 0)
			{
				int iChosen = GC.getGameINLINE().getSorenRandNum(aePlayers.size(), "Event pick player");
				eOtherPlayer = aePlayers[iChosen];
				pOtherPlayerCity = apCities[iChosen];
			}
			else
			{
				return NULL;
			}
		}
	}

	EventTriggeredData* pTriggerData = addEventTriggered();

	if (NULL != pTriggerData)
	{
		pTriggerData->m_eTrigger = eEventTrigger;
		pTriggerData->m_ePlayer = getID();
		pTriggerData->m_iTurn = GC.getGameINLINE().getGameTurn();
		pTriggerData->m_iCityId = (NULL != pCity) ? pCity->getID() : -1;
		pTriggerData->m_iPlotX = (NULL != pPlot) ? pPlot->getX_INLINE() : INVALID_PLOT_COORD;
		pTriggerData->m_iPlotY = (NULL != pPlot) ? pPlot->getY_INLINE() : INVALID_PLOT_COORD;
		pTriggerData->m_eOtherPlayer = eOtherPlayer;
		pTriggerData->m_iOtherPlayerCityId = (NULL != pOtherPlayerCity) ? pOtherPlayerCity->getID() : -1;
		pTriggerData->m_iUnitId = (NULL != pUnit) ? pUnit->getID() : -1;
		pTriggerData->m_eBuilding = eBuilding;
	}
	else
	{
		return NULL;
	}

	if (!isEmpty(kTrigger.getPythonCanDo()))
	{
		// Fastpath for event trigger callbacks:
		// Before we invoke the slow python callback, check if we can obtain the result directly
		if (0 == strcmp(kTrigger.getPythonCanDo(), "isPlayable"))
		{
			if (!isPlayable())
			{	
				return NULL;
			}
		}
		else if (0 == strcmp(kTrigger.getPythonCanDo(), "isHuman"))
		{
			if (!isHuman())
			{
				return NULL;
			}
		}
		else if (0 == strcmp(kTrigger.getPythonCanDo(), "TriggerChance"))
		{
			const EventTypes eEvent = kTrigger.getEvents().getEvent(0);
			const CvEventInfo& kEvent = GC.getEventInfo(eEvent);
			if (GC.getGameINLINE().getSorenRandNum(1000, "(c) TAC 2010 Events") >= kEvent.getGenericParameter(3))
			{
				return NULL;
			}
		}
		else
		{
			long lResult;

			CyArgsList argsList;
			argsList.add(gDLL->getPythonIFace()->makePythonObject(pTriggerData));

			gDLL->getPythonIFace()->callFunction(PYRandomEventModule, kTrigger.getPythonCanDo(), argsList.makeFunctionArgs(), &lResult);

			if (0 == lResult)
			{
				deleteEventTriggered(pTriggerData->getID());
				return NULL;
			}

			// python may change pTriggerData
			pCity = getCity(pTriggerData->m_iCityId);
			pPlot = GC.getMapINLINE().plot(pTriggerData->m_iPlotX, pTriggerData->m_iPlotY);
			pUnit = getUnit(pTriggerData->m_iUnitId);
			eOtherPlayer = pTriggerData->m_eOtherPlayer;
			if (NO_PLAYER != eOtherPlayer)
			{
				pOtherPlayerCity = GET_PLAYER(eOtherPlayer).getCity(pTriggerData->m_iOtherPlayerCityId);
			}
			eBuilding = pTriggerData->m_eBuilding;
		}
	}

	std::vector<CvWString> aszTexts;
	for (int i = 0; i < kTrigger.getNumTexts(); ++i)
	{
		if (NO_ERA == kTrigger.getTextEra(i) || kTrigger.getTextEra(i) == getCurrentEra())
		{
			aszTexts.push_back(kTrigger.getText(i));
		}
	}

	if (aszTexts.size() > 0)
	{
		int iText = GC.getGameINLINE().getSorenRandNum(aszTexts.size(), "Event Text choice");
		pTriggerData->m_szText = gDLL->getText(aszTexts[iText].GetCString(),
			eOtherPlayer != NO_PLAYER ? GET_PLAYER(eOtherPlayer).getCivilizationAdjectiveKey() : L"",
			NULL != pCity ? pCity->getNameKey() : L"",
			NULL != pUnit ? pUnit->getNameOrProfessionKey() : L"",
			L"",
			NO_BUILDING != eBuilding ? GC.getBuildingInfo(eBuilding).getTextKeyWide() : L"",
			NULL != pOtherPlayerCity ? pOtherPlayerCity->getNameKey() : L"",
			NULL != pPlot && NO_TERRAIN != pPlot->getTerrainType() ? GC.getTerrainInfo(pPlot->getTerrainType()).getTextKeyWide() : L"",
			NULL != pPlot && NO_IMPROVEMENT != pPlot->getImprovementType() ? GC.getImprovementInfo(pPlot->getImprovementType()).getTextKeyWide() : L"",
			NULL != pPlot && NO_BONUS != pPlot->getBonusType() ? GC.getBonusInfo(pPlot->getBonusType()).getTextKeyWide() : L"",
			NULL != pPlot && NO_ROUTE != pPlot->getRouteType() ? GC.getRouteInfo(pPlot->getRouteType()).getTextKeyWide() : L""
			);

	}
	else
	{
		pTriggerData->m_szText = L"";
	}

	if (kTrigger.getNumWorldNews() > 0)
	{
		int iText = GC.getGameINLINE().getSorenRandNum(kTrigger.getNumWorldNews(), "Trigger World News choice");

		pTriggerData->m_szGlobalText = gDLL->getText(kTrigger.getWorldNews(iText).GetCString(),
			getCivilizationAdjectiveKey(),
			NULL != pCity ? pCity->getNameKey() : L"",
			L"",
			eOtherPlayer != NO_PLAYER ? GET_PLAYER(eOtherPlayer).getCivilizationAdjectiveKey() : L"",
			NULL != pOtherPlayerCity ? pOtherPlayerCity->getNameKey() : L""
			);
	}
	else
	{
		pTriggerData->m_szGlobalText.clear();
	}

	if (bFire)
	{
		trigger(*pTriggerData);
	}

	return pTriggerData;
}


bool CvPlayer::canDoEvent(EventTypes eEvent, const EventTriggeredData& kTriggeredData) const
{
	if (eEvent == NO_EVENT)
	{
		FAssert(false);
		return false;
	}

	CvEventInfo& kEvent = GC.getEventInfo(eEvent);

	int iGold = std::min(getEventCost(eEvent, kTriggeredData.m_eOtherPlayer, false), getEventCost(eEvent, kTriggeredData.m_eOtherPlayer, true));
	// Time is the Issue
	if (iGold != 0)
	{
		if (iGold > 0 && NO_PLAYER != kTriggeredData.m_eOtherPlayer && kEvent.isGoldToPlayer())
		{
			if (GET_PLAYER(kTriggeredData.m_eOtherPlayer).getGold() < iGold)
			{
				return false;
			}
		}
		else if (iGold < 0)
		{
			if (getGold() < -iGold)
			{
				return false;
			}
		}
	}

	if (kEvent.getUnitClass() != NO_UNITCLASS)
	{
		UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(kEvent.getUnitClass());
		if (eUnit == NO_UNIT)
		{
			return false;
		}
	}

	if (kEvent.isCityEffect())
	{
		CvCity* pCity =	getCity(kTriggeredData.m_iCityId);
		if (NULL == pCity || !pCity->canApplyEvent(eEvent, kTriggeredData))
		{
			return false;
		}
	}
	else if (kEvent.isOtherPlayerCityEffect())
	{
		if (NO_PLAYER == kTriggeredData.m_eOtherPlayer)
		{
			return false;
		}

		CvCity* pCity = GET_PLAYER(kTriggeredData.m_eOtherPlayer).getCity(kTriggeredData.m_iOtherPlayerCityId);
		if (NULL == pCity || !pCity->canApplyEvent(eEvent, kTriggeredData))
		{
			return false;
		}
	}

	if (::isPlotEventTrigger(kTriggeredData.m_eTrigger))
	{
		CvPlot* pPlot = GC.getMapINLINE().plotINLINE(kTriggeredData.m_iPlotX, kTriggeredData.m_iPlotY);
		if (NULL != pPlot)
		{
			if (!pPlot->canApplyEvent(eEvent))
			{
				return false;
			}
		}
	}

	CvUnit* pUnit = getUnit(kTriggeredData.m_iUnitId);
	if (NULL != pUnit)
	{
		if (!pUnit->canApplyEvent(eEvent))
		{
			return false;
		}
	}

	if (NO_BONUS != kEvent.getBonusRevealed())
	{
		if (GET_TEAM(getTeam()).isForceRevealedBonus((BonusTypes)kEvent.getBonusRevealed()))
		{
			return false;
		}
	}

	if (0 != kEvent.getAttitudeModifier())
	{
		if (NO_PLAYER == kTriggeredData.m_eOtherPlayer)
		{
			return false;
		}

		if (GET_PLAYER(kTriggeredData.m_eOtherPlayer).getTeam() == getTeam())
		{
			return false;
		}

		if (GET_PLAYER(kTriggeredData.m_eOtherPlayer).isHuman())
		{
			if (0 == kEvent.getOurAttitudeModifier())
			{
				return false;
			}
		}
	}

	if (0 != kEvent.getTheirEnemyAttitudeModifier())
	{
		if (NO_PLAYER == kTriggeredData.m_eOtherPlayer)
		{
			return false;
		}

		TeamTypes eWorstEnemy = GET_TEAM(GET_PLAYER(kTriggeredData.m_eOtherPlayer).getTeam()).AI_getWorstEnemy();
		if (NO_TEAM == eWorstEnemy || eWorstEnemy == getTeam())
		{
			return false;
		}

		if (!GET_TEAM(eWorstEnemy).isAlive())
		{
			return false;
		}

		if (eWorstEnemy == getTeam())
		{
			return false;
		}
	}

	if (kEvent.isDeclareWar())
	{
		if (NO_PLAYER == kTriggeredData.m_eOtherPlayer)
		{
			return false;
		}

		if (!GET_TEAM(GET_PLAYER(kTriggeredData.m_eOtherPlayer).getTeam()).canDeclareWar(getTeam()) || !GET_TEAM(getTeam()).canDeclareWar(GET_PLAYER(kTriggeredData.m_eOtherPlayer).getTeam()))
		{
			return false;
		}
	}

	if (kEvent.isQuest())
	{
		for (EventTriggerTypes eTrigger = FIRST_EVENTTRIGGER; eTrigger < NUM_EVENTTRIGGER_TYPES; ++eTrigger)
		{
			CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(eTrigger);
			if (!kTrigger.isRecurring())
			{
				const InfoArray<EventTypes>& ReqEvents = kTrigger.getPrereqEvents();
				for (int i = 0; i < ReqEvents.getLength(); ++i)
				{
					if (ReqEvents.getEvent(i) == eEvent)
					{
						if (isTriggerFired(eTrigger))
						{
							return false;
						}
					}
				}
			}
		}
	}

	if (!isEmpty(kEvent.getPythonCanDo()))
	{
		// Fastpath for event trigger callbacks:
		// Before we invoke the slow python callback, check if we can obtain the result directly
		if (0 == strcmp(kEvent.getPythonCanDo(), "isPlayable"))
		{
			if (!isPlayable())
			{
				return false;
			}
		}
		else if (0 == strcmp(kEvent.getPythonCanDo(), "isHuman"))
		{
			if (!isHuman())
			{
				return false;
			}
		}
		else if (0 == strcmp(kEvent.getPythonCanDo(), "TriggerChance"))
		{
			if (GC.getGameINLINE().getSorenRandNum(1000, "(c) TAC 2010 Events") >= kEvent.getGenericParameter(3))
			{
				return false;
			}
		}
		else
		{
			long lResult;

			CyArgsList argsList;
			argsList.add(eEvent);
			argsList.add(gDLL->getPythonIFace()->makePythonObject(&kTriggeredData));

			gDLL->getPythonIFace()->callFunction(PYRandomEventModule, kEvent.getPythonCanDo(), argsList.makeFunctionArgs(), &lResult);

			if (0 == lResult)
			{
				return false;
			}
		}
	}

	return true;
}


void CvPlayer::applyEvent(EventTypes eEvent, int iEventTriggeredId, bool bUpdateTrigger)
{
	FAssert(eEvent != NO_EVENT);

	int iGrowthPercent = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
	
	EventTriggeredData* pTriggeredData = getEventTriggered(iEventTriggeredId);

	if (NULL == pTriggeredData)
	{
		deleteEventTriggered(iEventTriggeredId);
		return;
	}

	if (bUpdateTrigger)
	{
		setTriggerFired(*pTriggeredData, true);
	}

	if (!canDoEvent(eEvent, *pTriggeredData))
	{
		if (bUpdateTrigger)
		{
			deleteEventTriggered(iEventTriggeredId);
		}
		return;
	}

	setEventOccured(eEvent, *pTriggeredData);

	CvEventInfo& kEvent = GC.getEventInfo(eEvent);
	CvCity* pCity =	getCity(pTriggeredData->m_iCityId);
	CvCity* pOtherPlayerCity = NULL;

	if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
	{
		pOtherPlayerCity = GET_PLAYER(pTriggeredData->m_eOtherPlayer).getCity(pTriggeredData->m_iOtherPlayerCityId);
	}

	int iGold = getEventCost(eEvent, pTriggeredData->m_eOtherPlayer, false);
	int iRandomGold = getEventCost(eEvent, pTriggeredData->m_eOtherPlayer, true);
	iGold += GC.getGameINLINE().getSorenRandNum(iRandomGold - iGold + 1, "Event random gold");
	
	if (iGold != 0)
	{
		changeGold(iGold);

		if (NO_PLAYER != pTriggeredData->m_eOtherPlayer && kEvent.isGoldToPlayer())
		{
			GET_PLAYER(pTriggeredData->m_eOtherPlayer).changeGold(-iGold);
		}
	}

	if (kEvent.isDeclareWar())
	{
		if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
		{
			GET_TEAM(GET_PLAYER(pTriggeredData->m_eOtherPlayer).getTeam()).declareWar(getTeam(), false, WARPLAN_LIMITED);
		}
	}

	bool bClear = false;
	for (int iEvent = 0; iEvent < GC.getNumEventInfos(); ++iEvent)
	{
		if (kEvent.getClearEventChance(iEvent) > 0)
		{
			bClear = GC.getGameINLINE().getSorenRandNum(100, "Event Clear") < kEvent.getClearEventChance(iEvent);
			if (bClear)
			{
				if (kEvent.isGlobal())
				{
					for (int j = 0; j < MAX_PLAYERS; j++)
					{
						GET_PLAYER((PlayerTypes)j).resetEventOccured((EventTypes)iEvent, j != getID());
					}
				}
				else if (kEvent.isTeam())
				{
					for (int j = 0; j < MAX_PLAYERS; j++)
					{
						if (getTeam() == GET_PLAYER((PlayerTypes)j).getTeam())
						{
							GET_PLAYER((PlayerTypes)j).resetEventOccured((EventTypes)iEvent, j != getID());
						}
					}
				}
				else
				{
					resetEventOccured((EventTypes)iEvent, false);
				}
			}
		}
	}

	if (NULL != pCity && kEvent.isCityEffect())
	{
		pCity->applyEvent(eEvent, *pTriggeredData, bClear);
	}
	else if (NULL != pOtherPlayerCity && kEvent.isOtherPlayerCityEffect())
	{
		pOtherPlayerCity->applyEvent(eEvent, *pTriggeredData, bClear);
	}

	if (!kEvent.isCityEffect() && !kEvent.isOtherPlayerCityEffect())
	{
		if (kEvent.getNumBuildingYieldChanges() > 0)
		{
			int iLoop;
			for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); ++iBuildingClass)
			{
				for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
				{
					for (CvCity* pLoopCity = firstCity(&iLoop); NULL != pLoopCity; pLoopCity = nextCity(&iLoop))
					{
						pLoopCity->changeBuildingYieldChange((BuildingClassTypes)iBuildingClass, (YieldTypes)iYield, kEvent.getBuildingYieldChange(iBuildingClass, iYield));
					}
				}
			}
		}

		if (kEvent.getMaxPillage() > 0)
		{
			FAssert(kEvent.getMaxPillage() >= kEvent.getMinPillage());
			int iNumPillage = kEvent.getMinPillage() + GC.getGameINLINE().getSorenRandNum(kEvent.getMaxPillage() - kEvent.getMinPillage(), "Pick number of event pillaged plots");

			int iNumPillaged = 0;
			for (int i = 0; i < iNumPillage; ++i)
			{
				int iRandOffset = GC.getGameINLINE().getSorenRandNum(GC.getMapINLINE().numPlotsINLINE(), "Pick event pillage plot (any city)");
				for (int j = 0; j < GC.getMapINLINE().numPlotsINLINE(); ++j)
				{
					int iPlot = (j + iRandOffset) % GC.getMapINLINE().numPlotsINLINE();
					CvPlot* pPlot = GC.getMapINLINE().plotByIndexINLINE(iPlot);
					if (NULL != pPlot && pPlot->getOwnerINLINE() == getID() && pPlot->isCity())
					{
						if (NO_IMPROVEMENT != pPlot->getImprovementType() && !GC.getImprovementInfo(pPlot->getImprovementType()).isPermanent())
						{
							CvWString szBuffer = gDLL->getText("TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED", GC.getImprovementInfo(pPlot->getImprovementType()).getTextKeyWide());
							gDLL->getInterfaceIFace()->addMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGED", MESSAGE_TYPE_INFO, GC.getImprovementInfo(pPlot->getImprovementType()).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pPlot->getX_INLINE(), pPlot->getY_INLINE(), true, true);
							pPlot->setImprovementType(NO_IMPROVEMENT);
							++iNumPillaged;
							break;
						}
					}
				}
			}

			if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
			{
				CvWString szBuffer = gDLL->getText("TXT_KEY_EVENT_NUM_CITY_IMPROVEMENTS_DESTROYED", iNumPillaged, getCivilizationAdjectiveKey());
				gDLL->getInterfaceIFace()->addMessage(pTriggeredData->m_eOtherPlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGED", MESSAGE_TYPE_INFO);
			}
		}

		// rewritten food changes to avoid negative food - Nightinggale
		if (kEvent.getFood() != 0 || kEvent.getFoodPercent() != 0)
		{
			int iLoop;
			for (CvCity* pLoopCity = firstCity(&iLoop); NULL != pLoopCity; pLoopCity = nextCity(&iLoop))
			{
				// first get the food change
				// it can't be lower than negative current amount as this protects against negative food stored
				// note that there is no test to see if there is enough food prior to applying the event
				// it wouldn't make sense to require all cities to have the needed amount as that would make negative
				// amounts of food nearly impossible to trigger.
				int iFoodChange = std::max(kEvent.getFoodChange(pLoopCity), -pLoopCity->getFood());
				pLoopCity->changeFood(iFoodChange);
			}
		}

		if (kEvent.getPopulationChange() != 0)
		{
			int iLoop;

			for (CvCity* pLoopCity = firstCity(&iLoop); NULL != pLoopCity; pLoopCity = nextCity(&iLoop))
			{
				if (pLoopCity->getPopulation() + kEvent.getPopulationChange() > 0)
				{
					pLoopCity->changePopulation(kEvent.getPopulationChange());
				}
			}
		}

		if (kEvent.getCulture() != 0)
		{
			int iLoop;
			int iCulture = kEvent.getCulture();
			iCulture *= iGrowthPercent;
			iCulture /= 100;
			
			for (CvCity* pLoopCity = firstCity(&iLoop); NULL != pLoopCity; pLoopCity = nextCity(&iLoop))
			{
				
				if (pLoopCity->getCulture(pLoopCity->getOwnerINLINE()) + iCulture > 0)
				{
					pLoopCity->changeCulture(pLoopCity->getOwnerINLINE(), iCulture, true);
				}
			}
		}

		// R&R, ray, change for Health in Events
		if (kEvent.getHealth() != 0)
		{
			int iLoop;
			//Health is capped anyway, time is not the issue
			for (CvCity* pLoopCity = firstCity(&iLoop); NULL != pLoopCity; pLoopCity = nextCity(&iLoop))
			{
				pLoopCity->changeCityHealth(kEvent.getHealth());
			}
		}


		if (kEvent.getUnitClass() != NO_UNITCLASS)
		{
			UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(kEvent.getUnitClass());
			if (eUnit != NO_UNIT)
			{
				CvCity* pUnitCity = pCity;

				if (NULL == pUnitCity)
				{
					pUnitCity = getPrimaryCity();
				}

				if (NULL != pUnitCity)
				{
					for (int i = 0; i < kEvent.getNumUnits(); ++i)
					{
						initUnit(eUnit, GC.getUnitInfo(eUnit).getDefaultProfession(), pUnitCity->getX_INLINE(), pUnitCity->getY_INLINE());
					}
				}
			}
		}
	}

	CvPlot* pPlot = GC.getMapINLINE().plotINLINE(pTriggeredData->m_iPlotX, pTriggeredData->m_iPlotY);
	if (NULL != pPlot)
	{
		if (::isPlotEventTrigger(pTriggeredData->m_eTrigger))
		{
			FAssert(pPlot->canApplyEvent(eEvent));
			pPlot->applyEvent(eEvent);
		}
	}

	CvUnit* pUnit = getUnit(pTriggeredData->m_iUnitId);
	if (NULL != pUnit)
	{
		FAssert(pUnit->canApplyEvent(eEvent));
		pUnit->applyEvent(eEvent);   // might kill the unit
	}

	for (int i = 0; i < GC.getNumUnitCombatInfos(); ++i)
	{
		if (NO_PROMOTION != kEvent.getUnitCombatPromotion(i))
		{
			int iLoop;
			for (CvUnit* pLoopUnit = firstUnit(&iLoop); NULL != pLoopUnit; pLoopUnit = nextUnit(&iLoop))
			{
				if (pLoopUnit->getUnitCombatType() == i)
				{
					pLoopUnit->changeFreePromotionCount((PromotionTypes)kEvent.getUnitCombatPromotion(i), 1);
				}
			}

			setFreePromotion((UnitCombatTypes)i, (PromotionTypes)kEvent.getUnitCombatPromotion(i), true);
		}
	}

	for (int i = 0; i < GC.getNumUnitClassInfos(); ++i)
	{
		if (NO_PROMOTION != kEvent.getUnitClassPromotion(i))
		{
			int iLoop;
			for (CvUnit* pLoopUnit = firstUnit(&iLoop); NULL != pLoopUnit; pLoopUnit = nextUnit(&iLoop))
			{
				if (pLoopUnit->getUnitClassType() == i)
				{
					pLoopUnit->changeFreePromotionCount((PromotionTypes)kEvent.getUnitClassPromotion(i), 1);
				}
			}

			setFreePromotion((UnitClassTypes)i, (PromotionTypes)kEvent.getUnitClassPromotion(i), true);
		}
	}

	if (NO_BONUS != kEvent.getBonusRevealed())
	{
		GET_TEAM(getTeam()).setForceRevealedBonus((BonusTypes)kEvent.getBonusRevealed(), true);
	}

	if (0 != kEvent.getOurAttitudeModifier())
	{
		if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
		{
			if (kEvent.getOurAttitudeModifier() > 0)
			{
				AI_changeMemoryCount(pTriggeredData->m_eOtherPlayer, MEMORY_EVENT_GOOD_TO_US, kEvent.getOurAttitudeModifier());
			}
			else
			{
				AI_changeMemoryCount(pTriggeredData->m_eOtherPlayer, MEMORY_EVENT_BAD_TO_US, -kEvent.getOurAttitudeModifier());
			}
		}
	}

	if (0 != kEvent.getAttitudeModifier())
	{
		if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
		{
			if (kEvent.getAttitudeModifier() > 0)
			{
				GET_PLAYER(pTriggeredData->m_eOtherPlayer).AI_changeMemoryCount(getID(), MEMORY_EVENT_GOOD_TO_US, kEvent.getAttitudeModifier());
			}
			else
			{
				GET_PLAYER(pTriggeredData->m_eOtherPlayer).AI_changeMemoryCount(getID(), MEMORY_EVENT_BAD_TO_US, -kEvent.getAttitudeModifier());
			}
		}
	}

	if (0 != kEvent.getTheirEnemyAttitudeModifier())
	{
		if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
		{
			TeamTypes eWorstEnemy = GET_TEAM(GET_PLAYER(pTriggeredData->m_eOtherPlayer).getTeam()).AI_getWorstEnemy();
			if (NO_TEAM != eWorstEnemy)
			{
				for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
				{
					CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
					if (kLoopPlayer.isAlive() && kLoopPlayer.getTeam() == eWorstEnemy)
					{
						if (kEvent.getTheirEnemyAttitudeModifier() > 0)
						{
							kLoopPlayer.AI_changeMemoryCount(getID(), MEMORY_EVENT_GOOD_TO_US, kEvent.getTheirEnemyAttitudeModifier());
							AI_changeMemoryCount((PlayerTypes)iPlayer, MEMORY_EVENT_GOOD_TO_US, kEvent.getTheirEnemyAttitudeModifier());
						}
						else
						{
							kLoopPlayer.AI_changeMemoryCount(getID(), MEMORY_EVENT_BAD_TO_US, -kEvent.getTheirEnemyAttitudeModifier());
							AI_changeMemoryCount((PlayerTypes)iPlayer, MEMORY_EVENT_BAD_TO_US, -kEvent.getTheirEnemyAttitudeModifier());
						}
					}
				}
			}
		}
	}

	if (!isEmpty(kEvent.getPythonCallback()))
	{
		long lResult;

		CyArgsList argsList;
		argsList.add(eEvent);
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pTriggeredData));

		gDLL->getPythonIFace()->callFunction(PYRandomEventModule, kEvent.getPythonCallback(), argsList.makeFunctionArgs(), &lResult);
	}

	if (kEvent.getNumWorldNews() > 0)
	{
		int iText = GC.getGameINLINE().getSorenRandNum(kEvent.getNumWorldNews(), "Event World News choice");

		CvWString szGlobalText;

		TeamTypes eTheirWorstEnemy = NO_TEAM;
		if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
		{
			eTheirWorstEnemy = GET_TEAM(GET_PLAYER(pTriggeredData->m_eOtherPlayer).getTeam()).AI_getWorstEnemy();
		}

		szGlobalText = gDLL->getText(kEvent.getWorldNews(iText).GetCString(),
			getCivilizationAdjectiveKey(),
			NULL != pCity ? pCity->getNameKey() : L"",
			pTriggeredData->m_eOtherPlayer != NO_PLAYER ? GET_PLAYER(pTriggeredData->m_eOtherPlayer).getCivilizationAdjectiveKey() : L"",
			NULL != pOtherPlayerCity ? pOtherPlayerCity->getNameKey() : L"",
			L"",
			NO_TEAM != eTheirWorstEnemy ? GET_TEAM(eTheirWorstEnemy).getName().GetCString() : L""
			);

		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);

			if (kLoopPlayer.isAlive())
			{
				if (GET_TEAM(kLoopPlayer.getTeam()).isHasMet(getTeam()) && (NO_PLAYER == pTriggeredData->m_eOtherPlayer || GET_TEAM(GET_PLAYER(pTriggeredData->m_eOtherPlayer).getTeam()).isHasMet(getTeam())))
				{
					bool bShowPlot = GC.getEventTriggerInfo(pTriggeredData->m_eTrigger).isShowPlot();

					if (bShowPlot)
					{
						if (kLoopPlayer.getTeam() != getTeam())
						{
							if (NULL == pPlot || !pPlot->isRevealed(kLoopPlayer.getTeam(), false))
							{
								bShowPlot = false;
							}
						}
					}

					if (bShowPlot)
					{
						gDLL->getInterfaceIFace()->addMessage((PlayerTypes)iPlayer, false, GC.getEVENT_MESSAGE_TIME(), szGlobalText, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pTriggeredData->m_iPlotX, pTriggeredData->m_iPlotY, true, true);
					}
					else
					{
						gDLL->getInterfaceIFace()->addMessage((PlayerTypes)iPlayer, false, GC.getEVENT_MESSAGE_TIME(), szGlobalText, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT);
					}
				}
			}
		}

		GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szGlobalText, pTriggeredData->m_iPlotX, pTriggeredData->m_iPlotY, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
	}

	if (!isEmpty(kEvent.getLocalInfoTextKey()))
	{
		CvWString szLocalText;

		TeamTypes eTheirWorstEnemy = NO_TEAM;
		if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
		{
			eTheirWorstEnemy = GET_TEAM(GET_PLAYER(pTriggeredData->m_eOtherPlayer).getTeam()).AI_getWorstEnemy();
		}

		szLocalText = gDLL->getText(kEvent.getLocalInfoTextKey(),
			getCivilizationAdjectiveKey(),
			NULL != pCity ? pCity->getNameKey() : L"",
			pTriggeredData->m_eOtherPlayer != NO_PLAYER ? GET_PLAYER(pTriggeredData->m_eOtherPlayer).getCivilizationAdjectiveKey() : L"",
			NULL != pOtherPlayerCity ? pOtherPlayerCity->getNameKey() : L"",
			L"",
			NO_TEAM != eTheirWorstEnemy ? GET_TEAM(eTheirWorstEnemy).getName().GetCString() : L""
			);

			if (GC.getEventTriggerInfo(pTriggeredData->m_eTrigger).isShowPlot())
			{
				gDLL->getInterfaceIFace()->addMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szLocalText, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pTriggeredData->m_iPlotX, pTriggeredData->m_iPlotY, true, true);
			}
			else
			{
				gDLL->getInterfaceIFace()->addMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szLocalText, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"));
			}

	}

	if (!isEmpty(kEvent.getOtherPlayerPopup()))
	{
		// R&R, ray, small improvement
		if (NO_PLAYER != pTriggeredData->m_eOtherPlayer && GET_PLAYER(pTriggeredData->m_eOtherPlayer).isHuman())
		{
			CvWString szText = gDLL->getText(kEvent.getOtherPlayerPopup(),
				getCivilizationAdjectiveKey(),
				NULL != pCity ? pCity->getNameKey() : L"",
				pTriggeredData->m_eOtherPlayer != NO_PLAYER ? GET_PLAYER(pTriggeredData->m_eOtherPlayer).getCivilizationAdjectiveKey() : L"",
				NULL != pOtherPlayerCity ? pOtherPlayerCity->getNameKey() : L"",
				L""
				);

			CvPopupInfo* pInfo = new CvPopupInfo();

			if (NULL != pInfo)
			{
				pInfo->setText(szText);
				GET_PLAYER(pTriggeredData->m_eOtherPlayer).addPopup(pInfo);
			}
		}
	}

	bool bDeleteTrigger = bUpdateTrigger;

	for (int iEvent = 0; iEvent < GC.getNumEventInfos(); ++iEvent)
	{
		if (0 == kEvent.getAdditionalEventTime(iEvent))
		{
			if (kEvent.getAdditionalEventChance(iEvent) > 0)
			{
				if (canDoEvent((EventTypes)iEvent, *pTriggeredData))
				{
					if (GC.getGameINLINE().getSorenRandNum(100, "Additional Event") < kEvent.getAdditionalEventChance(iEvent))
					{
						applyEvent((EventTypes)iEvent, iEventTriggeredId, false);
					}
				}
			}
		}
		else
		{
			bool bSetTimer = true;
			if (kEvent.getAdditionalEventChance(iEvent) > 0)
			{
				if (GC.getGameINLINE().getSorenRandNum(100, "Additional Event 2") >= kEvent.getAdditionalEventChance(iEvent))
				{
					bSetTimer = false;
				}
			}

			if (bSetTimer)
			{
				EventTriggeredData kTriggered = *pTriggeredData;
				kTriggered.m_iTurn = (iGrowthPercent * kEvent.getAdditionalEventTime((EventTypes)iEvent)) / 100 + GC.getGameINLINE().getGameTurn();

				const EventTriggeredData* pExistingTriggered = getEventCountdown((EventTypes)iEvent);

				if (NULL != pExistingTriggered)
				{
					kTriggered.m_iTurn = std::min(kTriggered.m_iTurn, pExistingTriggered->m_iTurn);
				}

				setEventCountdown((EventTypes)iEvent, kTriggered);
				bDeleteTrigger = false;
			}
		}
	}

	if (bDeleteTrigger)
	{
		deleteEventTriggered(iEventTriggeredId);
	}
}
void CvPlayer::freeEuropeUnits()
{
	for (uint i = 0; i < m_aEuropeUnits.size(); ++i)
	{
		SAFE_DELETE(m_aEuropeUnits[i]);
	}
	m_aEuropeUnits.clear();
}

/*** TRIANGLETRADE 10/23/08 by DPII ***/
void CvPlayer::freeAfricaUnits()
{
	for (uint i = 0; i < m_aAfricaUnits.size(); ++i)
	{
		SAFE_DELETE(m_aAfricaUnits[i]);
	}
	m_aAfricaUnits.clear();
}
/**************************************/

// R&R, ray, Port Royal
void CvPlayer::freePortRoyalUnits()
{
	for (uint i = 0; i < m_aPortRoyalUnits.size(); ++i)
	{
		SAFE_DELETE(m_aPortRoyalUnits[i]);
	}
	m_aPortRoyalUnits.clear();
}
// R&R, ray, Port Royal - END

int CvPlayer::getEventCost(EventTypes eEvent, PlayerTypes eOtherPlayer, bool bRandom) const
{
	CvEventInfo& kEvent = GC.getEventInfo(eEvent);

	int iGold = kEvent.getGold();
	if (bRandom)
	{
		iGold += kEvent.getRandomGold();
	}
	int iGrowthPercent = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
	iGold *= iGrowthPercent;
	iGold /= 100;

	return iGold;

}


void CvPlayer::doEvents()
{
	MOD_PROFILE("CvPlayer::doEvents");

	if (GC.getGameINLINE().isOption(GAMEOPTION_NO_EVENTS))
	{
		return;
	}

	CvEventMap::iterator it = m_mapEventsOccured.begin();
	while (it != m_mapEventsOccured.end())
	{
		if (checkExpireEvent((*it).first, (*it).second))
		{
			expireEvent((*it).first, (*it).second, true);
			it = m_mapEventsOccured.erase(it);
		}
		else
		{
			++it;
		}
	}

	int gameSpeedMod =  GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();

	bool bNewEventEligible = true;
	if (GC.getGameINLINE().getElapsedGameTurns() < (GC.getDefineINT("FIRST_EVENT_DELAY_TURNS")*gameSpeedMod/100))
	{
		bNewEventEligible = false;
	}

	if (bNewEventEligible)
	{
		if (GC.getGameINLINE().getSorenRandNum(GC.getDefineINT("EVENT_PROBABILITY_ROLL_SIDES"), "Global event check") >= GC.getEraInfo(getCurrentEra()).getEventChancePerTurn())
		{
			bNewEventEligible = false;
		}
	}

	std::vector< std::pair<EventTriggeredData*, int> > aePossibleEventTriggerWeights;
	int iTotalWeight = 0;
	for (int i = 0; i < GC.getNumEventTriggerInfos(); ++i)
	{
		int iWeight = getEventTriggerWeight((EventTriggerTypes)i);
		if (iWeight < 0)
		{
			trigger((EventTriggerTypes)i);
		}
		else if (iWeight > 0 && bNewEventEligible)
		{
			EventTriggeredData* pTriggerData = initTriggeredData((EventTriggerTypes)i);
			if (NULL != pTriggerData)
			{
				iTotalWeight += iWeight;
				aePossibleEventTriggerWeights.push_back(std::make_pair(pTriggerData, iTotalWeight));
			}
		}
	}

	if (iTotalWeight > 0)
	{
		bool bFired = false;
		int iValue = GC.getGameINLINE().getSorenRandNum(iTotalWeight, "Event trigger");
		for (std::vector< std::pair<EventTriggeredData*, int> >::iterator it = aePossibleEventTriggerWeights.begin(); it != aePossibleEventTriggerWeights.end(); ++it)
		{
			EventTriggeredData* pTriggerData = (*it).first;
			if (NULL != pTriggerData)
			{
				if (iValue < (*it).second && !bFired)
				{
					trigger(*pTriggerData);
					bFired = true;
				}
				else
				{
					deleteEventTriggered(pTriggerData->getID());
				}
			}
		}
	}

	std::vector<int> aCleanup;
	for (int i = 0; i < GC.getNumEventInfos(); ++i)
	{
		const EventTriggeredData* pTriggeredData = getEventCountdown((EventTypes)i);
		if (NULL != pTriggeredData)
		{
			if (GC.getGameINLINE().getGameTurn() >= pTriggeredData->m_iTurn)
			{
				applyEvent((EventTypes)i, pTriggeredData->m_iId);
				resetEventCountdown((EventTypes)i);
				aCleanup.push_back(pTriggeredData->m_iId);
			}
		}
	}

	for (std::vector<int>::iterator it = aCleanup.begin(); it != aCleanup.end(); ++it)
	{
		bool bDelete = true;

		for (int i = 0; i < GC.getNumEventInfos(); ++i)
		{
			const EventTriggeredData* pTriggeredData = getEventCountdown((EventTypes)i);
			if (NULL != pTriggeredData)
			{
				if (pTriggeredData->m_iId == *it)
				{
					bDelete = false;
					break;
				}
			}
		}

		if (bDelete)
		{
			deleteEventTriggered(*it);
		}
	}
}


void CvPlayer::expireEvent(EventTypes eEvent, const EventTriggeredData& kTriggeredData, bool bFail)
{
	FAssert(getEventOccured(eEvent) == &kTriggeredData);
	FAssert(GC.getEventInfo(eEvent).isQuest() || GC.getGameINLINE().getGameTurn() - kTriggeredData.m_iTurn <= 4);

	if (GC.getEventInfo(eEvent).isQuest())
	{
		CvMessageQueue::iterator it;
		for (it = m_listGameMessages.begin(); it != m_listGameMessages.end(); ++it)
		{
			CvTalkingHeadMessage& message = *it;

			// the trigger ID is stored in the otherwise unused length field
			if (message.getLength() == kTriggeredData.getID())
			{
				m_listGameMessages.erase(it);
				gDLL->getInterfaceIFace()->dirtyTurnLog(getID());
				break;
			}
		}

		if (bFail)
		{
			gDLL->getInterfaceIFace()->addMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), gDLL->getText(GC.getEventInfo(eEvent).getQuestFailTextKey()), "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"));
		}
	}
}

bool CvPlayer::checkExpireEvent(EventTypes eEvent, const EventTriggeredData& kTriggeredData) const
{
	CvEventInfo& kEvent = GC.getEventInfo(eEvent);

	if (!isEmpty(kEvent.getPythonExpireCheck()))
	{
		long lResult;

		CyArgsList argsList;
		argsList.add(eEvent);
		argsList.add(gDLL->getPythonIFace()->makePythonObject(&kTriggeredData));

		gDLL->getPythonIFace()->callFunction(PYRandomEventModule, kEvent.getPythonExpireCheck(), argsList.makeFunctionArgs(), &lResult);

		if (0 != lResult)
		{
			return true;
		}
	}

	if (!kEvent.isQuest())
	{
		if (GC.getGameINLINE().getGameTurn() - kTriggeredData.m_iTurn > 2)
		{
			return true;
		}

		return false;
	}

	CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(kTriggeredData.m_eTrigger);

	FAssert(kTriggeredData.m_ePlayer != NO_PLAYER);

	CvPlayer& kPlayer = GET_PLAYER(kTriggeredData.m_ePlayer);

	if (NO_CIVIC != kTrigger.getCivic())
	{
		if (!kPlayer.isCivic((CivicTypes)kTrigger.getCivic()))
		{
			return true;
		}
	}

	if (kTriggeredData.m_iCityId != -1)
	{
		if (NULL == kPlayer.getCity(kTriggeredData.m_iCityId))
		{
			return true;
		}
	}

	if (kTriggeredData.m_iUnitId != -1)
	{
		if (NULL == kPlayer.getUnit(kTriggeredData.m_iUnitId))
		{
			return true;
		}
	}

	if (NO_PLAYER != kTriggeredData.m_eOtherPlayer)
	{
		if (!GET_PLAYER(kTriggeredData.m_eOtherPlayer).isAlive())
		{
			return true;
		}

		if (kTriggeredData.m_iOtherPlayerCityId != -1)
		{
			if (NULL == GET_PLAYER(kTriggeredData.m_eOtherPlayer).getCity(kTriggeredData.m_iOtherPlayerCityId))
			{
				return true;
			}
		}
	}
	return false;
}


void CvPlayer::trigger(EventTriggerTypes eTrigger)
{
	initTriggeredData(eTrigger, true);
}

void CvPlayer::trigger(const EventTriggeredData& kData)
{
	if (isHuman())
	{
		CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_EVENT, kData.getID());
		addPopup(pInfo, GC.getEventTriggerInfo(kData.m_eTrigger).isFrontPopup());
	}
	else
	{
		EventTypes eEvent = AI_chooseEvent(kData.getID());
		if (NO_EVENT != eEvent)
		{
			applyEvent(eEvent, kData.getID());
		}
	}
}

bool CvPlayer::canTrigger(EventTriggerTypes eTrigger, PlayerTypes ePlayer) const
{
	if (!isAlive())
	{
		return false;
	}

	if (getID() == ePlayer)
	{
		return false;
	}

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(eTrigger);

	if (getTeam() == kPlayer.getTeam())
	{
		return false;
	}

	if (!kTrigger.isPickPlayer())
	{
		return false;
	}

	if (!GET_TEAM(getTeam()).isHasMet(kPlayer.getTeam()))
	{
		return false;
	}

	if (isHuman() && kTrigger.isOtherPlayerAI())
	{
		return false;
	}

	if (!isNative() && kTrigger.isOtherPlayerNative())
	{
		return false;
	}

	if (!isPlayable() && kTrigger.isOtherPlayerPlayable())
	{
		return false;
	}

	if (GET_TEAM(getTeam()).isAtWar(kPlayer.getTeam()) != kTrigger.isOtherPlayerWar())
	{
		return false;
	}

	if (kTrigger.getOtherPlayerShareBorders() > 0)
	{
		int iCount = 0;

		for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); ++iI)
		{
			CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

			if (!pLoopPlot->isWater())
			{
				if ((pLoopPlot->getOwnerINLINE() == getID()) && pLoopPlot->isAdjacentPlayer(ePlayer, true))
				{
					++iCount;
				}
			}
		}

		if (iCount < kTrigger.getOtherPlayerShareBorders())
		{
			return false;
		}
	}

	return true;
}

CvCity* CvPlayer::pickTriggerCity(EventTriggerTypes eTrigger) const
{
	CvCity* pCity = NULL;
	std::vector<CvCity*> apCities;
	int iLoop;
	int iBestValue = MIN_INT;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		int iValue = pLoopCity->getTriggerValue(eTrigger);
		if (iValue > iBestValue)
		{
			if (NULL != pCity)
			{
				apCities.push_back(pCity);
			}

			iBestValue = iValue;
			pCity = pLoopCity;
		}
		else if (MIN_INT != iValue)
		{
			apCities.push_back(pLoopCity);
		}
	}

	if (NULL == pCity && apCities.size() > 0)
	{
		int iChosen = GC.getGameINLINE().getSorenRandNum(apCities.size(), "Event pick city");
		pCity = apCities[iChosen];
	}

	return pCity;
}

CvUnit* CvPlayer::pickTriggerUnit(EventTriggerTypes eTrigger, CvPlot* pPlot, bool bPickPlot) const
{
	CvUnit* pUnit = NULL;
	std::vector<CvUnit*> apUnits;
	int iLoop;
	int iBestValue = MIN_INT;
	for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		int iValue = pLoopUnit->getTriggerValue(eTrigger, pPlot, bPickPlot);

		if (iValue > iBestValue)
		{
			if (NULL != pUnit)
			{
				apUnits.push_back(pUnit);
			}

			iBestValue = iValue;
			pUnit = pLoopUnit;
		}
		else if (MIN_INT != iValue)
		{
			apUnits.push_back(pLoopUnit);
		}
	}

	if (NULL == pUnit && apUnits.size() > 0)
	{
		int iChosen = GC.getGameINLINE().getSorenRandNum(apUnits.size(), "Event pick unit");
		pUnit = apUnits[iChosen];
	}

	return pUnit;
}

int CvPlayer::getEventTriggerWeight(EventTriggerTypes eTrigger) const
{
	CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(eTrigger);

	// bail out if the player can't trigger due to civ category
	// do this first as the check is virtually instant meaning there is no point in checking anything else if this fails
	// particularly natives will reject events with this check, making natives checking event triggers more performant
	//    Nightinggale
	if (!kTrigger.canTriggerOnCivCategory(getCivCategoryTypes()))
	{
		return 0;
	}

	int gameSpeedMod =  GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent(); //XXX

	if (NO_HANDICAP != kTrigger.getMinDifficulty())
	{
		if (GC.getGameINLINE().getHandicapType() < kTrigger.getMinDifficulty())
		{
			return 0;
		}
	}

	if (kTrigger.isTutorial())
	{
		if (!isHuman() || !isOption(PLAYEROPTION_TUTORIAL))
		{
			return 0;
		}
	}

	if (kTrigger.isSinglePlayer() && GC.getGameINLINE().isGameMultiPlayer())
	{
		return 0;
	}

	if (!GC.getGameINLINE().isEventActive(eTrigger))
	{
		return 0;
	}
	if (!kTrigger.isRecurring())
	{
		if (isTriggerFired(eTrigger))
		{
			return 0;
		}
	}
	const InfoArray<EventTypes>& ReqEvents = kTrigger.getPrereqEvents();
	if (ReqEvents.getLength() > 0)
	{
		bool bFoundValid = true;
		for (int iI = 0; iI < ReqEvents.getLength(); iI++)
		{
			if (NULL == getEventOccured(ReqEvents.getEvent(iI)))
			{
				bFoundValid = false;
				break;
			}
		}

		if (!bFoundValid)
		{
			return 0;
		}
	}

	if (NO_CIVIC != kTrigger.getCivic())
	{
		bool bFoundValid = false;

		for (int iI = 0; iI < GC.getNumCivicOptionInfos(); ++iI)
		{
			if (getCivic((CivicOptionTypes)iI) == kTrigger.getCivic())
			{
				bFoundValid = true;
				break;
			}
		}

		if (!bFoundValid)
		{
			return 0;
		}
	}

	if (kTrigger.getMinTreasury() > 0)
	{
		if (getGold() < (kTrigger.getMinTreasury() * gameSpeedMod / 100)  )
		{
			return 0;
		}
	}

	if (GC.getMapINLINE().getNumLandAreas() < kTrigger.getMinMapLandmass())
	{
		return 0;
	}

	if (kTrigger.getMinOurLandmass() > 0 || kTrigger.getMaxOurLandmass() != -1)
	{
		int iNumLandmass = 0;

		int iLoop;
		for (CvArea* pArea = GC.getMapINLINE().firstArea(&iLoop); NULL != pArea; pArea = GC.getMapINLINE().nextArea(&iLoop))
		{
			if (!pArea->isWater())
			{
				if (pArea->getCitiesPerPlayer(getID()) > 0)
				{
					++iNumLandmass;
				}
			}
		}

		if (iNumLandmass < kTrigger.getMinOurLandmass())
		{
			return 0;
		}

		if (kTrigger.getMaxOurLandmass() != -1 && iNumLandmass > kTrigger.getMaxOurLandmass())
		{
			return 0;
		}
	}

	if (kTrigger.getProbability() < 0)
	{
		return kTrigger.getProbability();
	}

	int iProbability = kTrigger.getProbability();

	if (kTrigger.isProbabilityUnitMultiply() && kTrigger.getNumUnits() > 0)
	{
		int iNumUnits = 0;
		int iLoop;
		for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
		{
			if (MIN_INT != pLoopUnit->getTriggerValue(eTrigger, NULL, true))
			{
				++iNumUnits;
			}
		}

		iProbability *= iNumUnits;
	}

	if (kTrigger.isProbabilityBuildingMultiply() && kTrigger.getNumBuildings() > 0)
	{
		int iNumBuildings = 0;
		const InfoArray<BuildingClassTypes>& ReqBuildings = kTrigger.getBuildingsRequired();
		for (int i = 0; i < ReqBuildings.getLength(); ++i)
		{
			//if (kTrigger.getBuildingRequired(i) != NO_BUILDINGCLASS)
			{
				iNumBuildings += getBuildingClassCount(ReqBuildings.getBuildingClass(i));
			}
		}

		iProbability *= iNumBuildings;
	}

	return iProbability;
}

bool CvPlayer::isFreePromotion(UnitCombatTypes eUnitCombat, PromotionTypes ePromotion) const
{
	for (UnitCombatPromotionArray::const_iterator it = m_aFreeUnitCombatPromotions.begin(); it != m_aFreeUnitCombatPromotions.end(); ++it)
	{
		if ((*it).first == eUnitCombat && (*it).second == ePromotion)
		{
			return true;
		}
	}

	return false;
}

void CvPlayer::setFreePromotion(UnitCombatTypes eUnitCombat, PromotionTypes ePromotion, bool bFree)
{
	for (UnitCombatPromotionArray::iterator it = m_aFreeUnitCombatPromotions.begin(); it != m_aFreeUnitCombatPromotions.end(); ++it)
	{
		if ((*it).first == eUnitCombat && (*it).second == ePromotion)
		{
			if (!bFree)
			{
				m_aFreeUnitCombatPromotions.erase(it);
			}
			return;
		}
	}

	if (bFree)
	{
		m_aFreeUnitCombatPromotions.push_back(std::make_pair(eUnitCombat, ePromotion));
	}
}

bool CvPlayer::isFreePromotion(UnitClassTypes eUnitClass, PromotionTypes ePromotion) const
{
	for (UnitClassPromotionArray::const_iterator it = m_aFreeUnitClassPromotions.begin(); it != m_aFreeUnitClassPromotions.end(); ++it)
	{
		if ((*it).first == eUnitClass && (*it).second == ePromotion)
		{
			return true;
		}
	}

	return false;
}

void CvPlayer::setFreePromotion(UnitClassTypes eUnitClass, PromotionTypes ePromotion, bool bFree)
{
	for (UnitClassPromotionArray::iterator it = m_aFreeUnitClassPromotions.begin(); it != m_aFreeUnitClassPromotions.end(); ++it)
	{
		if ((*it).first == eUnitClass && (*it).second == ePromotion)
		{
			if (!bFree)
			{
				m_aFreeUnitClassPromotions.erase(it);
			}
			return;
		}
	}

	if (bFree)
	{
		m_aFreeUnitClassPromotions.push_back(std::make_pair(eUnitClass, ePromotion));
	}
}

// CACHE: cache frequently used values
///////////////////////////////////////
void CvPlayer::invalidatePopulationRankCache()
{
	int iLoop;
	CvCity* pLoopCity;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->invalidatePopulationRankCache();
	}
}

void CvPlayer::invalidateYieldRankCache(YieldTypes eYield)
{
	int iLoop;
	CvCity* pLoopCity;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->invalidateYieldRankCache();
	}
}

void CvPlayer::doUpdateCacheOnTurn()
{
	// add this back, after testing without it
	// invalidateYieldRankCache();
}

void CvPlayer::doEra()
{
	int iGameYear = GC.getGameINLINE().getGameTurnYear();
	EraTypes eBestEra = getCurrentEra();
	for (int i = eBestEra + 1; i < GC.getNumEraInfos(); i++)
	{
		EraTypes eLoopEra = (EraTypes) i;
		if(GC.getEraInfo(eLoopEra).isRevolution() == isInRevolution())
		{
			if(GC.getEraInfo(eLoopEra).getGameTurn() >= 0)
			{
				int iEraYear = GC.getGameINLINE().getTurnYear(GC.getEraInfo(eLoopEra).getGameTurn());

				if(iEraYear <= iGameYear)
				{
					eBestEra = eLoopEra;
				}
			}
			else
			{
				eBestEra = eLoopEra;
			}
		}
	}

	setCurrentEra(eBestEra);
}

PlayerTypes CvPlayer::pickConqueredCityOwner(const CvCity& kCity) const
{
	return getID();
}

void CvPlayer::forcePeace(PlayerTypes ePlayer)
{
	FAssert(GET_TEAM(getTeam()).canChangeWarPeace(GET_PLAYER(ePlayer).getTeam()));

	CLinkList<TradeData> playerList;
	CLinkList<TradeData> loopPlayerList;
	TradeData kTradeData;
	setTradeItem(&kTradeData, TRADE_PEACE_TREATY, 0, NULL);
	playerList.insertAtEnd(kTradeData);
	loopPlayerList.insertAtEnd(kTradeData);

	GC.getGameINLINE().implementDeal(getID(), ePlayer, &playerList, &loopPlayerList);
}

int CvPlayer::getNewCityProductionValue() const
{
	int iValue = 0;
	for (int iJ = 0; iJ < GC.getNumBuildingClassInfos(); iJ++)
	{
		BuildingTypes eBuilding = ((BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(iJ)));

		if (NO_BUILDING != eBuilding)
		{
			if (GC.getBuildingInfo(eBuilding).getFreeStartEra() != NO_ERA)
			{
				if (GC.getGameINLINE().getStartEra() >= GC.getBuildingInfo(eBuilding).getFreeStartEra())
				{
					for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
					{
						int iModifier = 0;
						if (iYield == YIELD_HAMMERS)
						{
							iModifier = getProductionModifier(eBuilding);
						}
						iValue += (100 * getYieldProductionNeeded(eBuilding, (YieldTypes)iYield)) / std::max(1, 100 + iModifier);
					}
				}
			}
		}
	}

	iValue *= 100 + GC.getDefineINT("NEW_CITY_BUILDING_VALUE_MODIFIER");
	iValue /= 100;

	iValue += (GC.getDefineINT("ADVANCED_START_CITY_COST") * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent()) / 100;

	int iPopulation = GC.getDefineINT("INITIAL_CITY_POPULATION") + GC.getEraInfo(GC.getGameINLINE().getStartEra()).getFreePopulation();
	for (int i = 1; i <= iPopulation; ++i)
	{
		iValue += (getGrowthThreshold(i) * GC.getDefineINT("ADVANCED_START_POPULATION_COST")) / 100;
	}

	return iValue;
}

int CvPlayer::getGrowthThreshold(int iPopulation) const
//Schmiddie, Growth Threshold Modifier START
{
	int iThreshold;

	iThreshold = (GC.getDefineINT("BASE_CITY_GROWTH_THRESHOLD") + (iPopulation * GC.getDefineINT("CITY_GROWTH_MULTIPLIER")));

	iThreshold *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
	iThreshold /= 100;

	iThreshold *= GC.getEraInfo(GC.getGameINLINE().getStartEra()).getGrowthPercent();
	iThreshold /= 100;

	for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); ++iTrait)
	{
		TraitTypes eTrait = (TraitTypes) iTrait;
		if (eTrait != NO_TRAIT)
		{
			if (hasTrait(eTrait))
			{
				iThreshold *= std::max(0, (100 + GC.getTraitInfo(eTrait).getPopGrowthThresholdModifier()));
				iThreshold /= 100;
			}
		}
	}								//Schmiddie, Growth Threshold Modifier ENDE

	if (isNative())
	{
		iThreshold *= GC.getDefineINT("NATIVE_GROWTH_THRESHOLD_MULTIPLIER");
		iThreshold /= 100;
	}
	else if (!isHuman())
	{
		iThreshold *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIGrowthPercent();
		iThreshold /= 100;

		iThreshold *= std::max(0, ((GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIPerEraModifier() * getCurrentEra()) + 100));
		iThreshold /= 100;
	}

	return std::max(1, iThreshold);
}

void CvPlayer::verifyUnitStacksValid()
{
	int iLoop;
	for(CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		pLoopUnit->verifyStackValid();
	}
}

CvCity* CvPlayer::getPopulationUnitCity(int iUnitId) const
{
	int iLoop;

	for (CvCity* pCity = firstCity(&iLoop); pCity != NULL; pCity = nextCity(&iLoop))
	{
		if (NULL != pCity->getPopulationUnitById(iUnitId))
		{
			return pCity;
		}
	}

	return NULL;
}

int CvPlayer::getYieldSellPrice(YieldTypes eYield) const
{
	FAssert(eYield >= 0);
	FAssert(eYield < NUM_YIELD_TYPES);

	return std::max(1, getYieldBuyPrice(eYield) + GC.getYieldInfo(eYield).getSellPriceDifference());
}

int CvPlayer::getYieldBuyPrice(YieldTypes eYield) const
{
	FAssert(eYield >= 0);
	FAssert(eYield < NUM_YIELD_TYPES);
	return m_em_iYieldBuyPrice.get(eYield);
}

void CvPlayer::setYieldBuyPrice(YieldTypes eYield, int iPrice, bool bMessage)
{
	FAssert(eYield >= 0);
	FAssert(eYield < NUM_YIELD_TYPES);

	// TAC - Price Limits - Ray - START
//	iPrice = std::max(iPrice, 1);

	// NEW PRICING MECHANISM to keep difference between manufactured goods and raw goods
	int price_diff = GC.getPRICE_DIFF_MAN_TO_RAW();
	int iOldPrice = getYieldBuyPrice(eYield);
	if (iPrice < iOldPrice)
	{
		switch (eYield)
							{
							case YIELD_ROPE:
								if (getYieldBuyPrice(eYield) - getYieldBuyPrice(YIELD_HEMP) <= price_diff)
								{
									eYield = YIELD_HEMP;
									iPrice = getYieldBuyPrice(YIELD_HEMP) - 1;
								}
								break;
							case YIELD_SAILCLOTH:
								if (getYieldBuyPrice(eYield) - getYieldBuyPrice(YIELD_HEMP) <= price_diff)
								{
									eYield = YIELD_HEMP;
									iPrice = getYieldBuyPrice(YIELD_HEMP) - 1;
								}
								break;
							case YIELD_GOLD:
								if (getYieldBuyPrice(eYield) - getYieldBuyPrice(YIELD_SILVER) <= price_diff)
								{
									eYield = YIELD_SILVER;
									iPrice = getYieldBuyPrice(YIELD_SILVER) - 1;
								}
								break;
							case YIELD_COCOA:
								if (getYieldBuyPrice(eYield) - getYieldBuyPrice(YIELD_COCOA_FRUITS) <= price_diff)
								{
									eYield = YIELD_COCOA_FRUITS;
									iPrice = getYieldBuyPrice(YIELD_COCOA_FRUITS) - 1;
								}
								break;
							case YIELD_COFFEE:
								if (getYieldBuyPrice(eYield) - getYieldBuyPrice(YIELD_COFFEE_BERRIES) <= price_diff)
								{
									eYield = YIELD_COFFEE_BERRIES;
									iPrice = getYieldBuyPrice(YIELD_COFFEE_BERRIES) - 1;
								}
								break;
							case YIELD_CIGARS:
								if (getYieldBuyPrice(eYield) - getYieldBuyPrice(YIELD_TOBACCO) <= price_diff)
								{
									eYield = YIELD_TOBACCO;
									iPrice = getYieldBuyPrice(YIELD_TOBACCO) - 1;
								}
								break;
							case YIELD_WOOL_CLOTH:
								if (getYieldBuyPrice(eYield) - getYieldBuyPrice(YIELD_WOOL) <= price_diff)
								{
									eYield = YIELD_WOOL;
									iPrice = getYieldBuyPrice(YIELD_WOOL) - 1;
								}
								break;
							case YIELD_CLOTH:
								if (getYieldBuyPrice(eYield) - getYieldBuyPrice(YIELD_COTTON) <= price_diff)
								{
									eYield = YIELD_COTTON;
									iPrice = getYieldBuyPrice(YIELD_COTTON) - 1;
								}
								break;
							case YIELD_COLOURED_CLOTH:
								if (getYieldBuyPrice(eYield) - (getYieldBuyPrice(YIELD_INDIGO) + getYieldBuyPrice(YIELD_CLOTH)) <= price_diff)
								{
									if (getYieldBuyPrice(YIELD_CLOTH) - getYieldBuyPrice(YIELD_INDIGO) <= price_diff)
									{
										eYield = YIELD_INDIGO;
										iPrice = getYieldBuyPrice(YIELD_INDIGO) - 1;
									}
									else
									{
										eYield = YIELD_CLOTH;
										iPrice = getYieldBuyPrice(YIELD_CLOTH) - 1;
									}
								}
								break;
							case YIELD_LEATHER:
								if (getYieldBuyPrice(eYield) - getYieldBuyPrice(YIELD_HIDES) <= price_diff)
								{
									eYield = YIELD_HIDES;
									iPrice = getYieldBuyPrice(YIELD_HIDES) - 1;
								}
								break;
							case YIELD_COATS:
								if (getYieldBuyPrice(eYield) - getYieldBuyPrice(YIELD_FUR) <= price_diff)
								{
									eYield = YIELD_FUR;
									iPrice = getYieldBuyPrice(YIELD_FUR) - 1;
								}
								break;
							case YIELD_PREMIUM_COATS:
								if (getYieldBuyPrice(eYield) - getYieldBuyPrice(YIELD_PREMIUM_FUR) <= price_diff)
								{
									eYield = YIELD_PREMIUM_FUR;
									iPrice = getYieldBuyPrice(YIELD_PREMIUM_FUR) - 1;
								}
								break;
							case YIELD_SALT:
								if (getYieldBuyPrice(eYield) - getYieldBuyPrice(YIELD_RAW_SALT) <= price_diff)
								{
									eYield = YIELD_RAW_SALT;
									iPrice = getYieldBuyPrice(YIELD_RAW_SALT) - 1;
								}
								break;
							case YIELD_SPICES:
								if (getYieldBuyPrice(eYield) - getYieldBuyPrice(YIELD_RED_PEPPER) <= price_diff)
								{
									eYield = YIELD_RED_PEPPER;
									iPrice = getYieldBuyPrice(YIELD_RED_PEPPER) - 1;
								}
								break;	
							case YIELD_BEER:
								if (getYieldBuyPrice(eYield) - getYieldBuyPrice(YIELD_BARLEY) <= price_diff)
								{
									eYield = YIELD_BARLEY;
									iPrice = getYieldBuyPrice(YIELD_BARLEY) - 1;
								}
								break;
							case YIELD_RUM:
								if (getYieldBuyPrice(eYield) - getYieldBuyPrice(YIELD_SUGAR) <= price_diff)
								{
									eYield = YIELD_SUGAR;
									iPrice = getYieldBuyPrice(YIELD_SUGAR) - 1;
								}
								break;
							case YIELD_WINE:
								if (getYieldBuyPrice(eYield) - getYieldBuyPrice(YIELD_GRAPES) <= price_diff)
								{
									eYield = YIELD_GRAPES;
									iPrice = getYieldBuyPrice(YIELD_GRAPES) - 1;
								}
								break;
							case YIELD_WHALE_OIL:
								if (getYieldBuyPrice(eYield) - getYieldBuyPrice(YIELD_WHALE_BLUBBER) <= price_diff)
								{
									eYield = YIELD_WHALE_BLUBBER;
									iPrice = getYieldBuyPrice(YIELD_WHALE_BLUBBER) - 1;
								}
								break;
							case YIELD_FURNITURE:
								if (getYieldBuyPrice(eYield) - getYieldBuyPrice(YIELD_VALUABLE_WOOD) <= price_diff)
								{
									eYield = YIELD_VALUABLE_WOOD;
									iPrice = getYieldBuyPrice(YIELD_VALUABLE_WOOD) - 1;
								}
								break;
							default:
								break;
							}
	}

	//Never let price fall below Minimum
	iPrice = std::max(iPrice, GC.getYieldInfo(eYield).getMinimumBuyPrice());

	// TAC - Price Limits - Ray - END

	if (iPrice != getYieldBuyPrice(eYield))
	{
		// TAC - Price Limits - Ray - START
		//int iOldPrice = getYieldBuyPrice(eYield);
		// TAC - Price Limits - Ray - END

		m_em_iYieldBuyPrice.set(eYield, iPrice);

		gDLL->getInterfaceIFace()->setDirty(EuropeScreen_DIRTY_BIT, true);

		if (bMessage)
		{
			CvWString szMessage;
			if (iPrice > iOldPrice)
			{
				szMessage = gDLL->getText("TXT_KEY_PRICE_RISE", GC.getYieldInfo(eYield).getTextKeyWide(), GC.getCivilizationInfo(getCivilizationType()).getShortDescriptionKey(), getYieldBuyPrice(eYield));
			}
			else
			{
				szMessage = gDLL->getText("TXT_KEY_PRICE_FALL", GC.getYieldInfo(eYield).getTextKeyWide(), GC.getCivilizationInfo(getCivilizationType()).getShortDescriptionKey(), getYieldBuyPrice(eYield));
			}

			// R&R, ray price messages only displayed to Colony, not all players, as long as no according features
			for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
			{
				CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes) iPlayer);
				// R&R, ray price messages only displayed to Colony, not all players, as long as no according features
				// if (kLoopPlayer.isAlive() && GET_TEAM(getTeam()).isHasMet(kLoopPlayer.getTeam()))
				if (kLoopPlayer.isAlive() && kLoopPlayer.getParent() == getID())
				{
					gDLL->getInterfaceIFace()->addMessage(kLoopPlayer.getID(), true, GC.getEVENT_MESSAGE_TIME(), szMessage, "AS2D_ADVISOR_SUGGEST", MESSAGE_TYPE_INFO, NULL);
				}
			}
		}
	}
}

void CvPlayer::sellYieldUnitToEurope(CvUnit* pUnit, int iAmount, int iCommission)
{
	FAssert(pUnit != NULL);
	if (pUnit != NULL)
	{
		FAssert(pUnit->isGoods());
		FAssert(getParent() != NO_PLAYER);
		CvPlayer& kPlayerEurope = GET_PLAYER(getParent());
		bool bDelayedDeath = (pUnit->getUnitTravelState() == NO_UNIT_TRAVEL_STATE);

		YieldTypes eYield = pUnit->getYield();
		if (NO_YIELD != eYield)
		{
			// R&R, ray, Smuggling - START
			bool bSmuggling = false;
			CvUnit* Transport = pUnit->getTransportUnit();
			if (Transport != NULL)
			{
				if(GC.getYieldInfo(eYield).isCargo() && Transport->getUnitClassType() == (UnitClassTypes)GC.getDefineINT("UNITCLASS_SMUGGLING_SHIP"))
				{
					bSmuggling = true;
				}
			}

			//if (isYieldEuropeTradable(eYield))
			if (isYieldEuropeTradable(eYield) || bSmuggling)
			// R&R, ray, Smuggling - END
			{
				iAmount = std::min(iAmount, pUnit->getYieldStored());

				// R&R, ray, Smuggling - START
				// int iProfit = getSellToEuropeProfit(eYield, iAmount * (100 - iCommission) / 100);
				int iProfit = 0;
				int iBribe = GC.getDefineINT("SMUGGLING_BRIBE_RATE");
				int iSellPrice;
				if (bSmuggling)
				{
					if (!isYieldEuropeTradable(eYield))
					{
						int minPrice = GC.getYieldInfo(eYield).getMinimumBuyPrice();
						iProfit = iAmount * minPrice * (100 - iBribe) / 100;
						iSellPrice = minPrice; // R&R, vetiarvind, Price dependent tax rate change
					}
					else if (iBribe > iCommission) // R&R, ray, small change from C.B.
					{
						iProfit = getSellToEuropeProfit(eYield, iAmount * (100 - iCommission) / 100);
						iSellPrice = -1; // R&R, vetiarvind, Price dependent tax rate change: -1 will use Europe Sale price
					}
					else
					{
						CvPlayer& kPlayerEurope = GET_PLAYER(getParent());
						int price = kPlayerEurope.getYieldBuyPrice(eYield);
						iProfit = iAmount * price  * (100 - iBribe) / 100;						
						iSellPrice = price; // R&R, vetiarvind, Price dependent tax rate change
					}
					
				}
				else
				{
					iProfit = getSellToEuropeProfit(eYield, iAmount * (100 - iCommission) / 100);
					iSellPrice = kPlayerEurope.getYieldBuyPrice(eYield); // R&R, vetiarvind, Price dependent tax rate change
				}
				// R&R, ray, Smuggling - END
				changeGold(iProfit * getExtraTradeMultiplier(kPlayerEurope.getID()) / 100);
				
				// R&R, vetiarvind, Price dependent tax rate change - Start
				//changeYieldTradedTotal(eYield, iAmount);
				//kPlayerEurope.changeYieldTradedTotal(eYield, iAmount);				
				changeYieldTradedTotal(eYield, iAmount, iSellPrice);
				kPlayerEurope.changeYieldTradedTotal(eYield, iAmount, iSellPrice);
				
				// R&R, vetiarvind, Price dependent tax rate change - End
				
				GC.getGameINLINE().changeYieldBoughtTotal(kPlayerEurope.getID(), eYield, -iAmount);

				pUnit->setYieldStored(pUnit->getYieldStored() - iAmount);
				if (pUnit->getYieldStored() <= 0)
				{
					pUnit->setTransportUnit(NULL);
					pUnit->kill(bDelayedDeath);
				}

				for (int i = 0; i < GC.getNumFatherPointInfos(); ++i)
				{
					FatherPointTypes ePointType = (FatherPointTypes) i;

					changeFatherPoints(ePointType, iProfit * GC.getFatherPointInfo(ePointType).getEuropeTradeGoldPointPercent() / 100);
				}

				CvWStringBuffer szMessage;
				// R&R, ray, Smuggling - START
				if (bSmuggling && !(iBribe > iCommission)) // R&R, ray, small change from C.B.
				{
					GAMETEXT.setEuropeYieldSoldHelp(szMessage, *this, eYield, iAmount, iBribe);
				}
				else
				{
					GAMETEXT.setEuropeYieldSoldHelp(szMessage, *this, eYield, iAmount, iCommission);
				}
				// R&R, ray, Smuggling - END
				m_aszTradeMessages.push_back(szMessage.getCString());

				// TAC - Trade Messages - koma13 - START
				m_aeTradeMessageTypes.push_back(TRADE_MESSAGE_EUROPE_YIELD_SOLD);
				m_aeTradeMessageYields.push_back(eYield);
				m_aiTradeMessageAmounts.push_back(iAmount);
				m_aiTradeMessageCommissions.push_back(iCommission);
				// TAC - Trade Messages - koma13 - END

				gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szMessage.getCString(), "AS2D_BUILD_BANK", MESSAGE_TYPE_LOG_ONLY);

				gDLL->getInterfaceIFace()->setDirty(EuropeScreen_DIRTY_BIT, true);

				gDLL->getEventReporterIFace()->yieldSoldToEurope(getID(), eYield, iAmount);
			}
		}
		else if (pUnit->getUnitInfo().isTreasure())
		{
			int iAmount = pUnit->getYieldStored();
			int iNetAmount = iAmount * (100 - iCommission) / 100;
			iNetAmount -= (iNetAmount * getTaxRate()) / 100;
			changeGold(iNetAmount * getExtraTradeMultiplier(kPlayerEurope.getID()) / 100);

			pUnit->setYieldStored(0);
			pUnit->setTransportUnit(NULL);
			pUnit->kill(bDelayedDeath);

			CvWStringBuffer szMessage;
			GAMETEXT.setEuropeYieldSoldHelp(szMessage, *this, eYield, iAmount, iCommission);
			m_aszTradeMessages.push_back(szMessage.getCString());

			// TAC - Trade Messages - koma13 - START
			m_aeTradeMessageTypes.push_back(TRADE_MESSAGE_TREASURE);
			m_aeTradeMessageYields.push_back(eYield);
			m_aiTradeMessageAmounts.push_back(iAmount);
			m_aiTradeMessageCommissions.push_back(iCommission);
			// TAC - Trade Messages - koma13 - END

			gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szMessage.getCString(), "AS2D_BUILD_BANK", MESSAGE_TYPE_LOG_ONLY);

			gDLL->getInterfaceIFace()->setDirty(EuropeScreen_DIRTY_BIT, true);
		}
	}

	// PatchMod: Check Europe prices after each trade START
	GET_PLAYER(getParent()).doPrices();
	// PatchMod: Check Europe prices after each trade END
}

CvUnit* CvPlayer::buyYieldUnitFromEurope(YieldTypes eYield, int iAmount, CvUnit* pTransport)
{
	if (!isYieldEuropeTradable(eYield))
	{
		return NULL;
	}

	FAssert(pTransport != NULL);
	if (NULL == pTransport)
	{
		return NULL;
	}

	int iAmountAvailable = pTransport->getLoadYieldAmount(eYield);
	iAmount = std::min(iAmount, iAmountAvailable);
	if(iAmount == 0)
	{
		FAssertMsg(false, "Can't load cargo yield.");
		return NULL;
	}

	FAssert(pTransport->getOwnerINLINE() == getID());
	FAssert(getParent() != NO_PLAYER);
	CvPlayer& kPlayerEurope = GET_PLAYER(getParent());
	int iYieldBuyPrice = kPlayerEurope.getYieldSellPrice(eYield);// R&R, vetiarvind, Price dependent tax rate change
	int iPrice = iAmount * iYieldBuyPrice;
	if (iPrice > getGold())
	{
		m_aszTradeMessages.push_back(gDLL->getText("EUROPE_SCREEN_BUY_UNIT_LACK_FUNDS", GC.getYieldInfo(eYield).getTextKeyWide(), iPrice));

		// TAC - Trade Messages - koma13 - START
		m_aeTradeMessageTypes.push_back(TRADE_MESSAGE_LACK_FUNDS);
		m_aeTradeMessageYields.push_back(NO_YIELD);
		m_aiTradeMessageAmounts.push_back(0);
		m_aiTradeMessageCommissions.push_back(0);
		// TAC - Trade Messages - koma13 - END

		gDLL->getInterfaceIFace()->setDirty(EuropeScreen_DIRTY_BIT, true);
		return NULL;
	}

	CvYieldInfo& kYield = GC.getYieldInfo(eYield);
	UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(kYield.getUnitClass());
	FAssert(NO_UNIT != eUnit);
	if (NO_UNIT == eUnit)
	{
		return NULL;
	}

	CvUnit* pUnit = initUnit(eUnit, GC.getUnitInfo(eUnit).getDefaultProfession(), INVALID_PLOT_COORD, INVALID_PLOT_COORD, NO_UNITAI, NO_DIRECTION, iAmount);
	FAssert(NULL != pUnit);
	if (NULL != pUnit)
	{
		pUnit->setUnitTravelState(UNIT_TRAVEL_STATE_IN_EUROPE, false);
		UnitAITypes eUnitAI = pUnit->AI_getUnitAIType();
		pUnit->removeFromMap(); //needs to match addToMap
		pUnit->addToMap(pTransport->getX_INLINE(), pTransport->getY_INLINE());
		pUnit->AI_setUnitAIType(eUnitAI);

		//unit possibly killed after joining other cargo
		if (!pUnit->setTransportUnit(pTransport))
		{
			pUnit = NULL;
		}

		changeGold(-iPrice);
		// R&R, vetiarvind, Price dependent tax rate change - Start
		int iBuyValue = iYieldBuyPrice >> 1; //buying should only contribute 50% of sell to tax incr. score
		changeYieldTradedTotal(eYield, iAmount, iBuyValue);
		kPlayerEurope.changeYieldTradedTotal(eYield, iAmount, iBuyValue);
		//changeYieldTradedTotal(eYield, iAmount);
		//kPlayerEurope.changeYieldTradedTotal(eYield, iAmount);
		// R&R, vetiarvind, Price dependent tax rate change - End

		GC.getGameINLINE().changeYieldBoughtTotal(kPlayerEurope.getID(), eYield, iAmount);

		CvWStringBuffer szMessage;
		GAMETEXT.setEuropeYieldBoughtHelp(szMessage, *this, eYield, iAmount);
		m_aszTradeMessages.push_back(szMessage.getCString());

		// TAC - Trade Messages - koma13 - START
		m_aeTradeMessageTypes.push_back(TRADE_MESSAGE_EUROPE_YIELD_BOUGHT);
		m_aeTradeMessageYields.push_back(eYield);
		m_aiTradeMessageAmounts.push_back(iAmount);
		m_aiTradeMessageCommissions.push_back(0);
		// TAC - Trade Messages - koma13 - END

		gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szMessage.getCString(), "AS2D_BUILD_BANK", MESSAGE_TYPE_LOG_ONLY);

		gDLL->getInterfaceIFace()->setDirty(EuropeScreen_DIRTY_BIT, true);

		gDLL->getEventReporterIFace()->yieldBoughtFromEurope(getID(), eYield, iAmount);
	}

	// PatchMod: Check Europe prices after each trade START
	GET_PLAYER(getParent()).doPrices();
	// PatchMod: Check Europe prices after each trade END

	return pUnit;
}

// TAC - AI purchases military units - koma13 - START
//int CvPlayer::getEuropeUnitBuyPrice(UnitTypes eUnit) const
int CvPlayer::getEuropeUnitBuyPrice(UnitTypes eUnit, bool bIncrease) const
// TAC - AI purchases military units - koma13 - END
{
	CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);

	int iCost = kUnit.getEuropeCost();

	bool bNegative = (iCost < 0);
	iCost = std::abs(iCost);

	// TAC - AI purchases military units - koma13 - START
	//iCost += GET_TEAM(getTeam()).getEuropeUnitsPurchased((UnitClassTypes) kUnit.getUnitClassType()) * kUnit.getEuropeCostIncrease();
	if (bIncrease)
	{
		iCost += GET_TEAM(getTeam()).getUnitsPurchasedHistory((UnitClassTypes) kUnit.getUnitClassType()) * kUnit.getEuropeCostIncrease();
	}
	// TAC - AI purchases military units - koma13 - END

	iCost *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
	iCost /= 100;

	iCost *= GC.getEraInfo(GC.getGameINLINE().getStartEra()).getTrainPercent();
	iCost /= 100;

	for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); ++iTrait)
	{
		if (hasTrait((TraitTypes) iTrait))
		{
			iCost *= std::max(0, (100 - GC.getTraitInfo((TraitTypes) iTrait).getRecruitPriceDiscount()));
			iCost /= 100;
		}
	}

	if (!isHuman())
	{
		iCost *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAITrainPercent();
		iCost /= 100;

		iCost *= std::max(0, ((GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIPerEraModifier() * getCurrentEra()) + 100));
		iCost /= 100;
	}

	if(bNegative)
	{
		iCost = std::min(-iCost, -1);
	}

	return iCost;
}

// R&R, ray, Africa
int CvPlayer::getYieldAfricaSellPrice(YieldTypes eYield) const
{
	FAssert(eYield >= 0);
	FAssert(eYield < NUM_YIELD_TYPES);

	return std::max(1, getYieldAfricaBuyPrice(eYield) + GC.getYieldInfo(eYield).getAfricaSellPriceDifference());
}

int CvPlayer::getYieldAfricaBuyPrice(YieldTypes eYield) const
{
	FAssert(eYield >= 0);
	FAssert(eYield < NUM_YIELD_TYPES);
	return m_em_iYieldAfricaBuyPrice.get(eYield);
}

void CvPlayer::setYieldAfricaBuyPrice(YieldTypes eYield, int iPrice, bool bMessage)
{
	FAssert(eYield >= 0);
	FAssert(eYield < NUM_YIELD_TYPES);

	// TAC - Price Limits - Ray - START
	//	iPrice = std::max(iPrice, 1);

	// NEW PRICING MECHANISM to keep difference between manufactured goods and raw goods
	int price_diff = GC.getPRICE_DIFF_MAN_TO_RAW();
	int iOldPrice = getYieldAfricaBuyPrice(eYield);
	if (iPrice < iOldPrice)
	{
		switch (eYield)
							{
							case YIELD_ROPE:
								if (getYieldAfricaBuyPrice(eYield) - getYieldAfricaBuyPrice(YIELD_HEMP) <= price_diff)
								{
									eYield = YIELD_HEMP;
									iPrice = getYieldAfricaBuyPrice(YIELD_HEMP) - 1;
								}
								break;
							case YIELD_SAILCLOTH:
								if (getYieldAfricaBuyPrice(eYield) - getYieldAfricaBuyPrice(YIELD_HEMP) <= price_diff)
								{
									eYield = YIELD_HEMP;
									iPrice = getYieldAfricaBuyPrice(YIELD_HEMP) - 1;
								}
								break;
							case YIELD_GOLD:
								if (getYieldAfricaBuyPrice(eYield) - getYieldAfricaBuyPrice(YIELD_SILVER) <= price_diff)
								{
									eYield = YIELD_SILVER;
									iPrice = getYieldAfricaBuyPrice(YIELD_SILVER) - 1;
								}
								break;
							case YIELD_COCOA:
								if (getYieldAfricaBuyPrice(eYield) - getYieldAfricaBuyPrice(YIELD_COCOA_FRUITS) <= price_diff)
								{
									eYield = YIELD_COCOA_FRUITS;
									iPrice = getYieldAfricaBuyPrice(YIELD_COCOA_FRUITS) - 1;
								}
								break;
							case YIELD_COFFEE:
								if (getYieldAfricaBuyPrice(eYield) - getYieldAfricaBuyPrice(YIELD_COFFEE_BERRIES) <= price_diff)
								{
									eYield = YIELD_COFFEE_BERRIES;
									iPrice = getYieldAfricaBuyPrice(YIELD_COFFEE_BERRIES) - 1;
								}
								break;
							case YIELD_CIGARS:
								if (getYieldAfricaBuyPrice(eYield) - getYieldAfricaBuyPrice(YIELD_TOBACCO) <= price_diff)
								{
									eYield = YIELD_TOBACCO;
									iPrice = getYieldAfricaBuyPrice(YIELD_TOBACCO) - 1;
								}
								break;
							case YIELD_WOOL_CLOTH:
								if (getYieldAfricaBuyPrice(eYield) - getYieldAfricaBuyPrice(YIELD_WOOL) <= price_diff)
								{
									eYield = YIELD_WOOL;
									iPrice = getYieldAfricaBuyPrice(YIELD_WOOL) - 1;
								}
								break;
							case YIELD_CLOTH:
								if (getYieldAfricaBuyPrice(eYield) - getYieldAfricaBuyPrice(YIELD_COTTON) <= price_diff)
								{
									eYield = YIELD_COTTON;
									iPrice = getYieldAfricaBuyPrice(YIELD_COTTON) - 1;
								}
								break;
							case YIELD_COLOURED_CLOTH:
								if (getYieldAfricaBuyPrice(eYield) - (getYieldAfricaBuyPrice(YIELD_INDIGO) + getYieldAfricaBuyPrice(YIELD_CLOTH)) <= price_diff)
								{
									if (getYieldAfricaBuyPrice(YIELD_CLOTH) - getYieldAfricaBuyPrice(YIELD_INDIGO) <= price_diff)
									{
										eYield = YIELD_INDIGO;
										iPrice = getYieldAfricaBuyPrice(YIELD_INDIGO) - 1;
									}
									else
									{
										eYield = YIELD_CLOTH;
										iPrice = getYieldAfricaBuyPrice(YIELD_CLOTH) - 1;
									}
								}
								break;
							case YIELD_LEATHER:
								if (getYieldAfricaBuyPrice(eYield) - getYieldAfricaBuyPrice(YIELD_HIDES) <= price_diff)
								{
									eYield = YIELD_HIDES;
									iPrice = getYieldAfricaBuyPrice(YIELD_HIDES) - 1;
								}
								break;
							case YIELD_COATS:
								if (getYieldAfricaBuyPrice(eYield) - getYieldAfricaBuyPrice(YIELD_FUR) <= price_diff)
								{
									eYield = YIELD_FUR;
									iPrice = getYieldAfricaBuyPrice(YIELD_FUR) - 1;
								}
								break;
							case YIELD_PREMIUM_COATS:
								if (getYieldAfricaBuyPrice(eYield) - getYieldAfricaBuyPrice(YIELD_PREMIUM_FUR) <= price_diff)
								{
									eYield = YIELD_PREMIUM_FUR;
									iPrice = getYieldAfricaBuyPrice(YIELD_PREMIUM_FUR) - 1;
								}
								break;
							case YIELD_SALT:
								if (getYieldAfricaBuyPrice(eYield) - getYieldAfricaBuyPrice(YIELD_RAW_SALT) <= price_diff)
								{
									eYield = YIELD_RAW_SALT;
									iPrice = getYieldAfricaBuyPrice(YIELD_RAW_SALT) - 1;
								}
								break;
							case YIELD_SPICES:
								if (getYieldAfricaBuyPrice(eYield) - getYieldAfricaBuyPrice(YIELD_RED_PEPPER) <= price_diff)
								{
									eYield = YIELD_RED_PEPPER;
									iPrice = getYieldAfricaBuyPrice(YIELD_RED_PEPPER) - 1;
								}
								break;	
							case YIELD_BEER:
								if (getYieldAfricaBuyPrice(eYield) - getYieldAfricaBuyPrice(YIELD_BARLEY) <= price_diff)
								{
									eYield = YIELD_BARLEY;
									iPrice = getYieldAfricaBuyPrice(YIELD_BARLEY) - 1;
								}
								break;
							case YIELD_RUM:
								if (getYieldAfricaBuyPrice(eYield) - getYieldAfricaBuyPrice(YIELD_SUGAR) <= price_diff)
								{
									eYield = YIELD_SUGAR;
									iPrice = getYieldAfricaBuyPrice(YIELD_SUGAR) - 1;
								}
								break;
							case YIELD_WINE:
								if (getYieldAfricaBuyPrice(eYield) - getYieldAfricaBuyPrice(YIELD_GRAPES) <= price_diff)
								{
									eYield = YIELD_GRAPES;
									iPrice = getYieldAfricaBuyPrice(YIELD_GRAPES) - 1;
								}
								break;
							case YIELD_WHALE_OIL:
								if (getYieldAfricaBuyPrice(eYield) - getYieldAfricaBuyPrice(YIELD_WHALE_BLUBBER) <= price_diff)
								{
									eYield = YIELD_WHALE_BLUBBER;
									iPrice = getYieldAfricaBuyPrice(YIELD_WHALE_BLUBBER) - 1;
								}
								break;
							case YIELD_FURNITURE:
								if (getYieldAfricaBuyPrice(eYield) - getYieldAfricaBuyPrice(YIELD_VALUABLE_WOOD) <= price_diff)
								{
									eYield = YIELD_VALUABLE_WOOD;
									iPrice = getYieldAfricaBuyPrice(YIELD_VALUABLE_WOOD) - 1;
								}
								break;
							default:
								break;
							}
	}

	//Never let price fall below Minimum
	iPrice = std::max(iPrice, GC.getYieldInfo(eYield).getMinimumBuyPrice());

	// TAC - Price Limits - Ray - END

	if (iPrice != getYieldAfricaBuyPrice(eYield))
	{
		// TAC - Price Limits - Ray - START
		//int iOldPrice = getYieldBuyPrice(eYield);
		// TAC - Price Limits - Ray - END

		m_em_iYieldAfricaBuyPrice.set(eYield, iPrice);

		gDLL->getInterfaceIFace()->setDirty(AfricaScreen_DIRTY_BIT, true);

		// no message because it would get too much
		/*if (bMessage)
		{
			CvWString szMessage;
			if (iPrice > iOldPrice)
			{
				szMessage = gDLL->getText("TXT_KEY_PRICE_RISE", GC.getYieldInfo(eYield).getTextKeyWide(), GC.getCivilizationInfo(getCivilizationType()).getShortDescriptionKey(), getYieldBuyPrice(eYield));
			}
			else
			{
				szMessage = gDLL->getText("TXT_KEY_PRICE_FALL", GC.getYieldInfo(eYield).getTextKeyWide(), GC.getCivilizationInfo(getCivilizationType()).getShortDescriptionKey(), getYieldBuyPrice(eYield));
			}

			// R&R, ray price messages only displayed to Colony, not all players, as long as no according features
			for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
			{
				CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes) iPlayer);
				// R&R, ray price messages only displayed to Colony, not all players, as long as no according features
				// if (kLoopPlayer.isAlive() && GET_TEAM(getTeam()).isHasMet(kLoopPlayer.getTeam()))
				if (kLoopPlayer.isAlive() && kLoopPlayer.getParent() == getID())
				{
					gDLL->getInterfaceIFace()->addMessage(kLoopPlayer.getID(), true, GC.getEVENT_MESSAGE_TIME(), szMessage, "AS2D_ADVISOR_SUGGEST", MESSAGE_TYPE_INFO, NULL);
				}
			}
		}*/
	}
}

CvUnit* CvPlayer::buyYieldUnitFromAfrica(YieldTypes eYield, int iAmount, CvUnit* pTransport)
{
	if (!isYieldAfricaTradable(eYield))
	{
		return NULL;
	}

	FAssert(pTransport != NULL);
	if (NULL == pTransport)
	{
		return NULL;
	}

	int iAmountAvailable = pTransport->getLoadYieldAmount(eYield);
	iAmount = std::min(iAmount, iAmountAvailable);
	if(iAmount == 0)
	{
		FAssertMsg(false, "Can't load cargo yield.");
		return NULL;
	}

	FAssert(pTransport->getOwnerINLINE() == getID());
	FAssert(getParent() != NO_PLAYER);
	CvPlayer& kPlayerEurope = GET_PLAYER(getParent());
	int iPrice = iAmount * kPlayerEurope.getYieldAfricaSellPrice(eYield);
	if (iPrice > getGold())
	{
		m_aszTradeMessages.push_back(gDLL->getText("EUROPE_SCREEN_BUY_UNIT_LACK_FUNDS", GC.getYieldInfo(eYield).getTextKeyWide(), iPrice));

		// TAC - Trade Messages - koma13 - START
		m_aeTradeMessageTypes.push_back(TRADE_MESSAGE_LACK_FUNDS);
		m_aeTradeMessageYields.push_back(NO_YIELD);
		m_aiTradeMessageAmounts.push_back(0);
		m_aiTradeMessageCommissions.push_back(0);
		// TAC - Trade Messages - koma13 - END

		gDLL->getInterfaceIFace()->setDirty(AfricaScreen_DIRTY_BIT, true);
		return NULL;
	}

	CvYieldInfo& kYield = GC.getYieldInfo(eYield);
	UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(kYield.getUnitClass());
	FAssert(NO_UNIT != eUnit);
	if (NO_UNIT == eUnit)
	{
		return NULL;
	}

	CvUnit* pUnit = initUnit(eUnit, GC.getUnitInfo(eUnit).getDefaultProfession(), INVALID_PLOT_COORD, INVALID_PLOT_COORD, NO_UNITAI, NO_DIRECTION, iAmount);
	FAssert(NULL != pUnit);
	if (NULL != pUnit)
	{
		pUnit->setUnitTravelState(UNIT_TRAVEL_STATE_IN_AFRICA, false);
		UnitAITypes eUnitAI = pUnit->AI_getUnitAIType();
		pUnit->removeFromMap(); //needs to match addToMap
		pUnit->addToMap(pTransport->getX_INLINE(), pTransport->getY_INLINE());
		pUnit->AI_setUnitAIType(eUnitAI);

		//unit possibly killed after joining other cargo
		if (!pUnit->setTransportUnit(pTransport))
		{
			pUnit = NULL;
		}

		changeGold(-iPrice);
		// R&R, vetiarvind, Price dependent tax rate change - Start
		int iBuyValue = kPlayerEurope.getYieldAfricaSellPrice(eYield) >> 1; //buying should only contribute 50% of sell to tax incr. score
		changeYieldTradedTotal(eYield, iAmount, iBuyValue);
		kPlayerEurope.changeYieldTradedTotal(eYield, iAmount, iBuyValue);
		//changeYieldTradedTotal(eYield, iAmount);
		//kPlayerEurope.changeYieldTradedTotal(eYield, iAmount);
		// R&R, vetiarvind, Price dependent tax rate change - End		
		GC.getGameINLINE().changeYieldBoughtTotal(kPlayerEurope.getID(), eYield, iAmount);

		CvWStringBuffer szMessage;
		GAMETEXT.setEuropeYieldBoughtHelp(szMessage, *this, eYield, iAmount);
		m_aszTradeMessages.push_back(szMessage.getCString());

		// TAC - Trade Messages - koma13 - START
		m_aeTradeMessageTypes.push_back(TRADE_MESSAGE_EUROPE_YIELD_BOUGHT);
		m_aeTradeMessageYields.push_back(eYield);
		m_aiTradeMessageAmounts.push_back(iAmount);
		m_aiTradeMessageCommissions.push_back(0);
		// TAC - Trade Messages - koma13 - END

		gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szMessage.getCString(), "AS2D_BUILD_BANK", MESSAGE_TYPE_LOG_ONLY);

		gDLL->getInterfaceIFace()->setDirty(AfricaScreen_DIRTY_BIT, true);

		gDLL->getEventReporterIFace()->yieldBoughtFromEurope(getID(), eYield, iAmount);
	}

	// PatchMod: Check Europe prices after each trade START
	GET_PLAYER(getParent()).doAfricaPrices();
	// PatchMod: Check Europe prices after each trade END

	return pUnit;
}

void CvPlayer::sellYieldUnitToAfrica(CvUnit* pUnit, int iAmount, int iCommission)
{
	FAssert(pUnit != NULL);
	if (pUnit != NULL)
	{
		FAssert(pUnit->isGoods());
		FAssert(getParent() != NO_PLAYER);
		CvPlayer& kPlayerEurope = GET_PLAYER(getParent());
		bool bDelayedDeath = (pUnit->getUnitTravelState() == NO_UNIT_TRAVEL_STATE);

		YieldTypes eYield = pUnit->getYield();
		if (NO_YIELD != eYield)
		{
			// R&R, ray, Smuggling - START
			bool bSmuggling = false;
			CvUnit* Transport = pUnit->getTransportUnit();
			if (Transport != NULL)
			{
				if(GC.getYieldInfo(eYield).isCargo() && Transport->getUnitClassType() == (UnitClassTypes)GC.getDefineINT("UNITCLASS_SMUGGLING_SHIP"))
				{
					bSmuggling = true;
				}
			}

			//if (isYieldEuropeTradable(eYield))
			if (isYieldAfricaTradable(eYield) || bSmuggling)
			// R&R, ray, Smuggling - END
			{
				iAmount = std::min(iAmount, pUnit->getYieldStored());

				// R&R, ray, Smuggling - START
				// int iProfit = getSellToEuropeProfit(eYield, iAmount * (100 - iCommission) / 100);
				int iProfit = 0;
				int iBribe = GC.getDefineINT("SMUGGLING_BRIBE_RATE");
				int iSellPrice = 0; // R&R, vetiarvind, Price dependent tax rate change
				if (bSmuggling)
				{
					if (!isYieldAfricaTradable(eYield))
					{
						int minPrice = GC.getYieldInfo(eYield).getMinimumBuyPrice();
						iProfit = iAmount * minPrice * (100 - iBribe) / 100;
						iSellPrice = minPrice;// R&R, vetiarvind, Price dependent tax rate change
					}
					else
					{
						CvPlayer& kPlayerEurope = GET_PLAYER(getParent());
						int price = kPlayerEurope.getYieldAfricaBuyPrice(eYield);
						iProfit = iAmount * price  * (100 - iBribe) / 100;
						iSellPrice = price;// R&R, vetiarvind, Price dependent tax rate change
					}
				}
				else
				{
					iProfit = getSellToAfricaProfit(eYield, iAmount * (100 - iCommission) / 100);
					iSellPrice = kPlayerEurope.getYieldAfricaBuyPrice(eYield);// R&R, vetiarvind, Price dependent tax rate change
				}
				// R&R, ray, Smuggling - END
				changeGold(iProfit * getExtraTradeMultiplier(kPlayerEurope.getID()) / 100);
				// R&R, vetiarvind, Price dependent tax rate change - Start				
				changeYieldTradedTotal(eYield, iAmount, iSellPrice);
				kPlayerEurope.changeYieldTradedTotal(eYield, iAmount, iSellPrice);
				//changeYieldTradedTotal(eYield, iAmount);
				//kPlayerEurope.changeYieldTradedTotal(eYield, iAmount);
				// R&R, vetiarvind, Price dependent tax rate change - End
				
				GC.getGameINLINE().changeYieldBoughtTotal(kPlayerEurope.getID(), eYield, -iAmount);

				pUnit->setYieldStored(pUnit->getYieldStored() - iAmount);
				if (pUnit->getYieldStored() <= 0)
				{
					pUnit->setTransportUnit(NULL);
					pUnit->kill(bDelayedDeath);
				}

				for (int i = 0; i < GC.getNumFatherPointInfos(); ++i)
				{
					FatherPointTypes ePointType = (FatherPointTypes) i;

					changeFatherPoints(ePointType, iProfit * GC.getFatherPointInfo(ePointType).getEuropeTradeGoldPointPercent() / 100);
				}

				CvWStringBuffer szMessage;
				// R&R, ray, Smuggling - START
				if (bSmuggling)
				{
					GAMETEXT.setEuropeYieldSoldHelp(szMessage, *this, eYield, iAmount, iBribe);
				}
				else
				{
					GAMETEXT.setEuropeYieldSoldHelp(szMessage, *this, eYield, iAmount, iCommission);
				}
				// R&R, ray, Smuggling - END
				m_aszTradeMessages.push_back(szMessage.getCString());

				// TAC - Trade Messages - koma13 - START
				m_aeTradeMessageTypes.push_back(TRADE_MESSAGE_EUROPE_YIELD_SOLD);
				m_aeTradeMessageYields.push_back(eYield);
				m_aiTradeMessageAmounts.push_back(iAmount);
				m_aiTradeMessageCommissions.push_back(iCommission);
				// TAC - Trade Messages - koma13 - END

				gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szMessage.getCString(), "AS2D_BUILD_BANK", MESSAGE_TYPE_LOG_ONLY);

				gDLL->getInterfaceIFace()->setDirty(AfricaScreen_DIRTY_BIT, true);

				gDLL->getEventReporterIFace()->yieldSoldToEurope(getID(), eYield, iAmount); // was this the problem ?
			}
		}
		else if (pUnit->getUnitInfo().isTreasure())
		{
			int iAmount = pUnit->getYieldStored();
			int iNetAmount = iAmount * (100 - iCommission) / 100;
			iNetAmount -= (iNetAmount * getTaxRate()) / 100;
			changeGold(iNetAmount * getExtraTradeMultiplier(kPlayerEurope.getID()) / 100);

			pUnit->setYieldStored(0);
			pUnit->setTransportUnit(NULL);
			pUnit->kill(bDelayedDeath);

			CvWStringBuffer szMessage;
			GAMETEXT.setEuropeYieldSoldHelp(szMessage, *this, eYield, iAmount, iCommission);
			m_aszTradeMessages.push_back(szMessage.getCString());

			// TAC - Trade Messages - koma13 - START
			m_aeTradeMessageTypes.push_back(TRADE_MESSAGE_TREASURE);
			m_aeTradeMessageYields.push_back(eYield);
			m_aiTradeMessageAmounts.push_back(iAmount);
			m_aiTradeMessageCommissions.push_back(iCommission);
			// TAC - Trade Messages - koma13 - END

			gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szMessage.getCString(), "AS2D_BUILD_BANK", MESSAGE_TYPE_LOG_ONLY);

			gDLL->getInterfaceIFace()->setDirty(AfricaScreen_DIRTY_BIT, true);
		}
	}

	// PatchMod: Check Africa prices after each trade START
	GET_PLAYER(getParent()).doAfricaPrices();
	// PatchMod: Check Africa prices after each trade END
}
// R&R, ray, Africa - END


// R&R, ray, Port Royal
int CvPlayer::getYieldPortRoyalSellPrice(YieldTypes eYield) const
{
	FAssert(eYield >= 0);
	FAssert(eYield < NUM_YIELD_TYPES);

	return std::max(1, getYieldPortRoyalBuyPrice(eYield) + GC.getYieldInfo(eYield).getPortRoyalSellPriceDifference());
}

int CvPlayer::getYieldPortRoyalBuyPrice(YieldTypes eYield) const
{
	FAssert(eYield >= 0);
	FAssert(eYield < NUM_YIELD_TYPES);
	return m_em_iYieldPortRoyalBuyPrice.get(eYield);
}

void CvPlayer::setYieldPortRoyalBuyPrice(YieldTypes eYield, int iPrice, bool bMessage)
{
	FAssert(eYield >= 0);
	FAssert(eYield < NUM_YIELD_TYPES);

	// TAC - Price Limits - Ray - START
	//	iPrice = std::max(iPrice, 1);

	// NEW PRICING MECHANISM to keep difference between manufactured goods and raw goods
	int price_diff = GC.getPRICE_DIFF_MAN_TO_RAW();
	int iOldPrice = getYieldPortRoyalBuyPrice(eYield);
	if (iPrice < iOldPrice)
	{
		switch (eYield)
							{
							case YIELD_ROPE:
								if (getYieldPortRoyalBuyPrice(eYield) - getYieldPortRoyalBuyPrice(YIELD_HEMP) <= price_diff)
								{
									eYield = YIELD_HEMP;
									iPrice = getYieldPortRoyalBuyPrice(YIELD_HEMP) - 1;
								}
								break;
							case YIELD_SAILCLOTH:
								if (getYieldPortRoyalBuyPrice(eYield) - getYieldPortRoyalBuyPrice(YIELD_HEMP) <= price_diff)
								{
									eYield = YIELD_HEMP;
									iPrice = getYieldPortRoyalBuyPrice(YIELD_HEMP) - 1;
								}
								break;
							case YIELD_GOLD:
								if (getYieldPortRoyalBuyPrice(eYield) - getYieldPortRoyalBuyPrice(YIELD_SILVER) <= price_diff)
								{
									eYield = YIELD_SILVER;
									iPrice = getYieldPortRoyalBuyPrice(YIELD_SILVER) - 1;
								}
								break;
							case YIELD_COCOA:
								if (getYieldPortRoyalBuyPrice(eYield) - getYieldPortRoyalBuyPrice(YIELD_COCOA_FRUITS) <= price_diff)
								{
									eYield = YIELD_COCOA_FRUITS;
									iPrice = getYieldPortRoyalBuyPrice(YIELD_COCOA_FRUITS) - 1;
								}
								break;
							case YIELD_COFFEE:
								if (getYieldPortRoyalBuyPrice(eYield) - getYieldPortRoyalBuyPrice(YIELD_COFFEE_BERRIES) <= price_diff)
								{
									eYield = YIELD_COFFEE_BERRIES;
									iPrice = getYieldPortRoyalBuyPrice(YIELD_COFFEE_BERRIES) - 1;
								}
								break;
							case YIELD_CIGARS:
								if (getYieldPortRoyalBuyPrice(eYield) - getYieldPortRoyalBuyPrice(YIELD_TOBACCO) <= price_diff)
								{
									eYield = YIELD_TOBACCO;
									iPrice = getYieldPortRoyalBuyPrice(YIELD_TOBACCO) - 1;
								}
								break;
							case YIELD_WOOL_CLOTH:
								if (getYieldPortRoyalBuyPrice(eYield) - getYieldPortRoyalBuyPrice(YIELD_WOOL) <= price_diff)
								{
									eYield = YIELD_WOOL;
									iPrice = getYieldPortRoyalBuyPrice(YIELD_WOOL) - 1;
								}
								break;
							case YIELD_CLOTH:
								if (getYieldPortRoyalBuyPrice(eYield) - getYieldPortRoyalBuyPrice(YIELD_COTTON) <= price_diff)
								{
									eYield = YIELD_COTTON;
									iPrice = getYieldPortRoyalBuyPrice(YIELD_COTTON) - 1;
								}
								break;
							case YIELD_COLOURED_CLOTH:
								if (getYieldPortRoyalBuyPrice(eYield) - (getYieldPortRoyalBuyPrice(YIELD_INDIGO) + getYieldPortRoyalBuyPrice(YIELD_CLOTH)) <= price_diff)
								{
									if (getYieldPortRoyalBuyPrice(YIELD_CLOTH) - getYieldPortRoyalBuyPrice(YIELD_INDIGO) <= price_diff)
									{
										eYield = YIELD_INDIGO;
										iPrice = getYieldPortRoyalBuyPrice(YIELD_INDIGO) - 1;
									}
									else
									{
										eYield = YIELD_CLOTH;
										iPrice = getYieldPortRoyalBuyPrice(YIELD_CLOTH) - 1;
									}
								}
								break;
							case YIELD_LEATHER:
								if (getYieldPortRoyalBuyPrice(eYield) - getYieldPortRoyalBuyPrice(YIELD_HIDES) <= price_diff)
								{
									eYield = YIELD_HIDES;
									iPrice = getYieldPortRoyalBuyPrice(YIELD_HIDES) - 1;
								}
								break;
							case YIELD_COATS:
								if (getYieldPortRoyalBuyPrice(eYield) - getYieldPortRoyalBuyPrice(YIELD_FUR) <= price_diff)
								{
									eYield = YIELD_FUR;
									iPrice = getYieldPortRoyalBuyPrice(YIELD_FUR) - 1;
								}
								break;
							case YIELD_PREMIUM_COATS:
								if (getYieldPortRoyalBuyPrice(eYield) - getYieldPortRoyalBuyPrice(YIELD_PREMIUM_FUR) <= price_diff)
								{
									eYield = YIELD_PREMIUM_FUR;
									iPrice = getYieldPortRoyalBuyPrice(YIELD_PREMIUM_FUR) - 1;
								}
								break;
							case YIELD_SALT:
								if (getYieldPortRoyalBuyPrice(eYield) - getYieldPortRoyalBuyPrice(YIELD_RAW_SALT) <= price_diff)
								{
									eYield = YIELD_RAW_SALT;
									iPrice = getYieldPortRoyalBuyPrice(YIELD_RAW_SALT) - 1;
								}
								break;
							case YIELD_SPICES:
								if (getYieldPortRoyalBuyPrice(eYield) - getYieldPortRoyalBuyPrice(YIELD_RED_PEPPER) <= price_diff)
								{
									eYield = YIELD_RED_PEPPER;
									iPrice = getYieldPortRoyalBuyPrice(YIELD_RED_PEPPER) - 1;
								}
								break;
							case YIELD_BEER:
								if (getYieldPortRoyalBuyPrice(eYield) - getYieldPortRoyalBuyPrice(YIELD_BARLEY) <= price_diff)
								{
									eYield = YIELD_BARLEY;
									iPrice = getYieldPortRoyalBuyPrice(YIELD_BARLEY) - 1;
								}
								break;
							case YIELD_RUM:
								if (getYieldPortRoyalBuyPrice(eYield) - getYieldPortRoyalBuyPrice(YIELD_SUGAR) <= price_diff)
								{
									eYield = YIELD_SUGAR;
									iPrice = getYieldPortRoyalBuyPrice(YIELD_SUGAR) - 1;
								}
								break;
							case YIELD_WINE:
								if (getYieldPortRoyalBuyPrice(eYield) - getYieldPortRoyalBuyPrice(YIELD_GRAPES) <= price_diff)
								{
									eYield = YIELD_GRAPES;
									iPrice = getYieldPortRoyalBuyPrice(YIELD_GRAPES) - 1;
								}
								break;
							case YIELD_WHALE_OIL:
								if (getYieldPortRoyalBuyPrice(eYield) - getYieldPortRoyalBuyPrice(YIELD_WHALE_BLUBBER) <= price_diff)
								{
									eYield = YIELD_WHALE_BLUBBER;
									iPrice = getYieldPortRoyalBuyPrice(YIELD_WHALE_BLUBBER) - 1;
								}
								break;
							case YIELD_FURNITURE:
								if (getYieldPortRoyalBuyPrice(eYield) - getYieldPortRoyalBuyPrice(YIELD_VALUABLE_WOOD) <= price_diff)
								{
									eYield = YIELD_VALUABLE_WOOD;
									iPrice = getYieldPortRoyalBuyPrice(YIELD_VALUABLE_WOOD) - 1;
								}
								break;
							default:
								break;
							}
	}

	//Never let price fall below Minimum
	iPrice = std::max(iPrice, GC.getYieldInfo(eYield).getMinimumBuyPrice());

	// TAC - Price Limits - Ray - END

	if (iPrice != getYieldPortRoyalBuyPrice(eYield))
	{
		// TAC - Price Limits - Ray - START
		//int iOldPrice = getYieldBuyPrice(eYield);
		// TAC - Price Limits - Ray - END

		m_em_iYieldPortRoyalBuyPrice.set(eYield, iPrice);

		gDLL->getInterfaceIFace()->setDirty(PortRoyalScreen_DIRTY_BIT, true);

		// no message because it would get too much
		/*if (bMessage)
		{
			CvWString szMessage;
			if (iPrice > iOldPrice)
			{
				szMessage = gDLL->getText("TXT_KEY_PRICE_RISE", GC.getYieldInfo(eYield).getTextKeyWide(), GC.getCivilizationInfo(getCivilizationType()).getShortDescriptionKey(), getYieldBuyPrice(eYield));
			}
			else
			{
				szMessage = gDLL->getText("TXT_KEY_PRICE_FALL", GC.getYieldInfo(eYield).getTextKeyWide(), GC.getCivilizationInfo(getCivilizationType()).getShortDescriptionKey(), getYieldBuyPrice(eYield));
			}

			// R&R, ray price messages only displayed to Colony, not all players, as long as no according features
			for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
			{
				CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes) iPlayer);
				// R&R, ray price messages only displayed to Colony, not all players, as long as no according features
				// if (kLoopPlayer.isAlive() && GET_TEAM(getTeam()).isHasMet(kLoopPlayer.getTeam()))
				if (kLoopPlayer.isAlive() && kLoopPlayer.getParent() == getID())
				{
					gDLL->getInterfaceIFace()->addMessage(kLoopPlayer.getID(), true, GC.getEVENT_MESSAGE_TIME(), szMessage, "AS2D_ADVISOR_SUGGEST", MESSAGE_TYPE_INFO, NULL);
				}
			}
		}*/
	}
}

CvUnit* CvPlayer::buyYieldUnitFromPortRoyal(YieldTypes eYield, int iAmount, CvUnit* pTransport)
{
	if (!isYieldPortRoyalTradable(eYield))
	{
		return NULL;
	}

	FAssert(pTransport != NULL);
	if (NULL == pTransport)
	{
		return NULL;
	}

	int iAmountAvailable = pTransport->getLoadYieldAmount(eYield);
	iAmount = std::min(iAmount, iAmountAvailable);
	if(iAmount == 0)
	{
		FAssertMsg(false, "Can't load cargo yield.");
		return NULL;
	}

	FAssert(pTransport->getOwnerINLINE() == getID());
	FAssert(getParent() != NO_PLAYER);
	CvPlayer& kPlayerEurope = GET_PLAYER(getParent());
	int iPrice = iAmount * kPlayerEurope.getYieldPortRoyalSellPrice(eYield);
	if (iPrice > getGold())
	{
		m_aszTradeMessages.push_back(gDLL->getText("EUROPE_SCREEN_BUY_UNIT_LACK_FUNDS", GC.getYieldInfo(eYield).getTextKeyWide(), iPrice));

		// TAC - Trade Messages - koma13 - START
		m_aeTradeMessageTypes.push_back(TRADE_MESSAGE_LACK_FUNDS);
		m_aeTradeMessageYields.push_back(NO_YIELD);
		m_aiTradeMessageAmounts.push_back(0);
		m_aiTradeMessageCommissions.push_back(0);
		// TAC - Trade Messages - koma13 - END

		gDLL->getInterfaceIFace()->setDirty(PortRoyalScreen_DIRTY_BIT, true);
		return NULL;
	}

	CvYieldInfo& kYield = GC.getYieldInfo(eYield);
	UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(kYield.getUnitClass());
	FAssert(NO_UNIT != eUnit);
	if (NO_UNIT == eUnit)
	{
		return NULL;
	}

	CvUnit* pUnit = initUnit(eUnit, GC.getUnitInfo(eUnit).getDefaultProfession(), INVALID_PLOT_COORD, INVALID_PLOT_COORD, NO_UNITAI, NO_DIRECTION, iAmount);
	FAssert(NULL != pUnit);
	if (NULL != pUnit)
	{
		pUnit->setUnitTravelState(UNIT_TRAVEL_STATE_IN_PORT_ROYAL, false);
		UnitAITypes eUnitAI = pUnit->AI_getUnitAIType();
		pUnit->removeFromMap(); //needs to match addToMap
		pUnit->addToMap(pTransport->getX_INLINE(), pTransport->getY_INLINE());
		pUnit->AI_setUnitAIType(eUnitAI);

		//unit possibly killed after joining other cargo
		if (!pUnit->setTransportUnit(pTransport))
		{
			pUnit = NULL;
		}

		changeGold(-iPrice);
		// R&R, vetiarvind, Price dependent tax rate change - Start
		int iBuyValue = kPlayerEurope.getYieldPortRoyalSellPrice(eYield) >> 1; //buying should contribute only 50% of sell value to tax score
		changeYieldTradedTotal(eYield, iAmount, iBuyValue);
		kPlayerEurope.changeYieldTradedTotal(eYield, iAmount, iBuyValue);
		//changeYieldTradedTotal(eYield, iAmount);
		//kPlayerEurope.changeYieldTradedTotal(eYield, iAmount);
		// R&R, vetiarvind, Price dependent tax rate change - End
		GC.getGameINLINE().changeYieldBoughtTotal(kPlayerEurope.getID(), eYield, iAmount);

		CvWStringBuffer szMessage;
		GAMETEXT.setEuropeYieldBoughtHelp(szMessage, *this, eYield, iAmount);
		m_aszTradeMessages.push_back(szMessage.getCString());

		// TAC - Trade Messages - koma13 - START
		m_aeTradeMessageTypes.push_back(TRADE_MESSAGE_EUROPE_YIELD_BOUGHT);
		m_aeTradeMessageYields.push_back(eYield);
		m_aiTradeMessageAmounts.push_back(iAmount);
		m_aiTradeMessageCommissions.push_back(0);
		// TAC - Trade Messages - koma13 - END

		gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szMessage.getCString(), "AS2D_BUILD_BANK", MESSAGE_TYPE_LOG_ONLY);

		gDLL->getInterfaceIFace()->setDirty(PortRoyalScreen_DIRTY_BIT, true);

		gDLL->getEventReporterIFace()->yieldBoughtFromEurope(getID(), eYield, iAmount);
	}

	// PatchMod: Check Europe prices after each trade START
	GET_PLAYER(getParent()).doPortRoyalPrices();
	// PatchMod: Check Europe prices after each trade END

	return pUnit;
}

void CvPlayer::sellYieldUnitToPortRoyal(CvUnit* pUnit, int iAmount, int iCommission)
{
	FAssert(pUnit != NULL);
	if (pUnit != NULL)
	{
		FAssert(pUnit->isGoods());
		FAssert(getParent() != NO_PLAYER);
		CvPlayer& kPlayerEurope = GET_PLAYER(getParent());
		bool bDelayedDeath = (pUnit->getUnitTravelState() == NO_UNIT_TRAVEL_STATE);

		YieldTypes eYield = pUnit->getYield();
		if (NO_YIELD != eYield)
		{
			// R&R, ray, Smuggling - START
			bool bSmuggling = false;
			// no smuggling in Port Royal
			//CvUnit* Transport = pUnit->getTransportUnit();
			//if (Transport != NULL)
			//{
			//	if(GC.getYieldInfo(eYield).isCargo() && Transport->getUnitClassType() == (UnitClassTypes)GC.getDefineINT("UNITCLASS_SMUGGLING_SHIP"))
			//	{
			//		bSmuggling = true;
			//	}
			//}

			//if (isYieldEuropeTradable(eYield))
			if (isYieldPortRoyalTradable(eYield) || bSmuggling)
			// R&R, ray, Smuggling - END
			{
				iAmount = std::min(iAmount, pUnit->getYieldStored());

				// R&R, ray, Smuggling - START
				// int iProfit = getSellToEuropeProfit(eYield, iAmount * (100 - iCommission) / 100);
				int iProfit = 0;
				int iBribe = GC.getDefineINT("SMUGGLING_BRIBE_RATE");
				int iSellPrice = 0; // R&R, vetiarvind, Price dependent tax rate change
				if (bSmuggling)
				{
					if (!isYieldPortRoyalTradable(eYield))
					{
						int minPrice = GC.getYieldInfo(eYield).getMinimumBuyPrice();
						iProfit = iAmount * minPrice * (100 - iBribe) / 100;
						iSellPrice = minPrice; // R&R, vetiarvind, Price dependent tax rate change
					}
					else
					{
						CvPlayer& kPlayerEurope = GET_PLAYER(getParent());
						int price = kPlayerEurope.getYieldPortRoyalBuyPrice(eYield);
						iProfit = iAmount * price  * (100 - iBribe) / 100;
						iSellPrice = price; // R&R, vetiarvind, Price dependent tax rate change
					}
				}
				else
				{
					iProfit = getSellToPortRoyalProfit(eYield, iAmount * (100 - iCommission) / 100);
					iSellPrice = kPlayerEurope.getYieldPortRoyalBuyPrice(eYield); // R&R, vetiarvind, Price dependent tax rate change
				}
				// R&R, ray, Smuggling - END
				changeGold(iProfit * getExtraTradeMultiplier(kPlayerEurope.getID()) / 100);
				
				// R&R, vetiarvind, Price dependent tax rate change - Start				
				changeYieldTradedTotal(eYield, iAmount, iSellPrice);
				kPlayerEurope.changeYieldTradedTotal(eYield, iAmount, iSellPrice);
				//changeYieldTradedTotal(eYield, iAmount);
				//kPlayerEurope.changeYieldTradedTotal(eYield, iAmount);
				// R&R, vetiarvind, Price dependent tax rate change - End
				GC.getGameINLINE().changeYieldBoughtTotal(kPlayerEurope.getID(), eYield, -iAmount);

				pUnit->setYieldStored(pUnit->getYieldStored() - iAmount);
				if (pUnit->getYieldStored() <= 0)
				{
					pUnit->setTransportUnit(NULL);
					pUnit->kill(bDelayedDeath);
				}

				for (int i = 0; i < GC.getNumFatherPointInfos(); ++i)
				{
					FatherPointTypes ePointType = (FatherPointTypes) i;

					changeFatherPoints(ePointType, iProfit * GC.getFatherPointInfo(ePointType).getEuropeTradeGoldPointPercent() / 100);
				}

				CvWStringBuffer szMessage;
				// R&R, ray, Smuggling - START
				if (bSmuggling)
				{
					GAMETEXT.setEuropeYieldSoldHelp(szMessage, *this, eYield, iAmount, iBribe);
				}
				else
				{
					GAMETEXT.setEuropeYieldSoldHelp(szMessage, *this, eYield, iAmount, iCommission);
				}
				// R&R, ray, Smuggling - END
				m_aszTradeMessages.push_back(szMessage.getCString());

				// TAC - Trade Messages - koma13 - START
				m_aeTradeMessageTypes.push_back(TRADE_MESSAGE_EUROPE_YIELD_SOLD);
				m_aeTradeMessageYields.push_back(eYield);
				m_aiTradeMessageAmounts.push_back(iAmount);
				m_aiTradeMessageCommissions.push_back(iCommission);
				// TAC - Trade Messages - koma13 - END

				gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szMessage.getCString(), "AS2D_BUILD_BANK", MESSAGE_TYPE_LOG_ONLY);

				gDLL->getInterfaceIFace()->setDirty(PortRoyalScreen_DIRTY_BIT, true);

				gDLL->getEventReporterIFace()->yieldSoldToEurope(getID(), eYield, iAmount); // was this the problem ?
			}
		}
		else if (pUnit->getUnitInfo().isTreasure())
		{
			int iAmount = pUnit->getYieldStored();
			int iNetAmount = iAmount * (100 - iCommission) / 100;
			iNetAmount -= (iNetAmount * getTaxRate()) / 100;
			changeGold(iNetAmount * getExtraTradeMultiplier(kPlayerEurope.getID()) / 100);

			pUnit->setYieldStored(0);
			pUnit->setTransportUnit(NULL);
			pUnit->kill(bDelayedDeath);

			CvWStringBuffer szMessage;
			GAMETEXT.setEuropeYieldSoldHelp(szMessage, *this, eYield, iAmount, iCommission);
			m_aszTradeMessages.push_back(szMessage.getCString());

			// TAC - Trade Messages - koma13 - START
			m_aeTradeMessageTypes.push_back(TRADE_MESSAGE_TREASURE);
			m_aeTradeMessageYields.push_back(eYield);
			m_aiTradeMessageAmounts.push_back(iAmount);
			m_aiTradeMessageCommissions.push_back(iCommission);
			// TAC - Trade Messages - koma13 - END

			gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szMessage.getCString(), "AS2D_BUILD_BANK", MESSAGE_TYPE_LOG_ONLY);

			gDLL->getInterfaceIFace()->setDirty(PortRoyalScreen_DIRTY_BIT, true);
		}
	}

	// PatchMod: Check Port Royal prices after each trade START
	GET_PLAYER(getParent()).doPortRoyalPrices();
	// PatchMod: Check Port Royal prices after each trade END
}
// R&R, ray, Port Royal - END

CvUnit* CvPlayer::buyEuropeUnit(UnitTypes eUnit, int iPriceModifier)
{
	FAssert(canTradeWithEurope());
	if (!canTradeWithEurope())
	{
		return NULL;
	}

	if (getEuropeUnitBuyPrice(eUnit) < 0)
	{
		return NULL;
	}

	int iPrice = getEuropeUnitBuyPrice(eUnit) * iPriceModifier / 100;
	if (iPrice > getGold())
	{
		m_aszTradeMessages.push_back(gDLL->getText("EUROPE_SCREEN_BUY_UNIT_LACK_FUNDS", GC.getUnitInfo(eUnit).getTextKeyWide(), iPrice));

		// TAC - Trade Messages - koma13 - START
		m_aeTradeMessageTypes.push_back(TRADE_MESSAGE_LACK_FUNDS);
		m_aeTradeMessageYields.push_back(NO_YIELD);
		m_aiTradeMessageAmounts.push_back(0);
		m_aiTradeMessageCommissions.push_back(0);
		// TAC - Trade Messages - koma13 - END

		gDLL->getInterfaceIFace()->setDirty(EuropeScreen_DIRTY_BIT, true);
		return NULL;
	}

	CvUnit* pUnit = NULL;
	CvPlot* pStartingPlot = getStartingPlot();
	if (GC.getUnitInfo(eUnit).getDomainType() == DOMAIN_SEA && pStartingPlot != NULL)
	{
		pUnit = initUnit(eUnit, GC.getUnitInfo(eUnit).getDefaultProfession(), INVALID_PLOT_COORD, INVALID_PLOT_COORD);
        if (pUnit != NULL)
		{
			pUnit->setUnitTravelState(UNIT_TRAVEL_STATE_IN_EUROPE, false);
			//add unit to map after setting Europe state so that it doesn't bump enemy units
			pUnit->addToMap(pStartingPlot->getX_INLINE(), pStartingPlot->getY_INLINE());
		}
	}
	else
	{
		pUnit = initEuropeUnit(eUnit);
	}

	FAssert(NULL != pUnit);
	if (NULL != pUnit)
	{
		changeGold(-iPrice);
		GET_TEAM(getTeam()).changeUnitsPurchasedHistory(pUnit->getUnitClassType(), 1);
		gDLL->getEventReporterIFace()->unitBoughtFromEurope(getID(), pUnit->getID());
	}

	return pUnit;
}

/*** TRIANGLETRADE 10/23/08 by DPII ***/
int CvPlayer::getAfricaUnitBuyPrice(UnitTypes eUnit) const
{
	CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);

	int iCost = kUnit.getAfricaCost();
	if (iCost < 0)
	{
		return iCost;
	}

	iCost += GET_TEAM(getTeam()).getUnitsPurchasedHistory((UnitClassTypes) kUnit.getUnitClassType()) * kUnit.getAfricaCostIncrease();

	iCost *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
	iCost /= 100;

	iCost *= GC.getEraInfo(GC.getGameINLINE().getStartEra()).getTrainPercent();
	iCost /= 100;

	/*
	for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); ++iTrait)
	{
		if (hasTrait((TraitTypes) iTrait))
		{
			iCost *= std::max(0, (100 - GC.getTraitInfo((TraitTypes) iTrait).getRecruitPriceDiscount()));
			iCost /= 100;
		}
	}*/

	if (!isHuman())
	{
		iCost *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAITrainPercent();
		iCost /= 100;

		iCost *= std::max(0, ((GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIPerEraModifier() * getCurrentEra()) + 100));
		iCost /= 100;
	}

	return iCost;
}

CvUnit* CvPlayer::buyAfricaUnit(UnitTypes eUnit, int iPriceModifier)
{
	FAssert(canTradeWithAfrica());
	if (!canTradeWithAfrica())
	{
		return NULL;
	}

	if (getAfricaUnitBuyPrice(eUnit) < 0)
	{
		return NULL;
	}

	int iPrice = getAfricaUnitBuyPrice(eUnit) * iPriceModifier / 100;
	if (iPrice > getGold())
	{
		m_aszTradeMessages.push_back(gDLL->getText("EUROPE_SCREEN_BUY_UNIT_LACK_FUNDS", GC.getUnitInfo(eUnit).getTextKeyWide(), iPrice));

		// TAC - Trade Messages - koma13 - START
		m_aeTradeMessageTypes.push_back(TRADE_MESSAGE_LACK_FUNDS);
		m_aeTradeMessageYields.push_back(NO_YIELD);
		m_aiTradeMessageAmounts.push_back(0);
		m_aiTradeMessageCommissions.push_back(0);
		// TAC - Trade Messages - koma13 - END

		gDLL->getInterfaceIFace()->setDirty(AfricaScreen_DIRTY_BIT, true);
		return NULL;
	}
	
	CvUnit* pUnit = NULL;
	CvPlot* pStartingPlot = getStartingPlot();
	if (GC.getUnitInfo(eUnit).getDomainType() == DOMAIN_SEA && pStartingPlot != NULL)
	{
		pUnit = initUnit(eUnit, GC.getUnitInfo(eUnit).getDefaultProfession(), INVALID_PLOT_COORD, INVALID_PLOT_COORD);
        if (pUnit != NULL)
		{
			pUnit->setUnitTravelState(UNIT_TRAVEL_STATE_IN_AFRICA, false);
			//add unit to map after setting Europe state so that it doesn't bump enemy units
			pUnit->addToMap(pStartingPlot->getX_INLINE(), pStartingPlot->getY_INLINE());
		}
	}
	else
	{
		pUnit = initAfricaUnit(eUnit);
	}

	FAssert(NULL != pUnit);
	if (NULL != pUnit)
	{
		changeGold(-iPrice);
		GET_TEAM(getTeam()).changeUnitsPurchasedHistory(pUnit->getUnitClassType(), 1);
		//gDLL->getEventReporterIFace()->unitBoughtFromEurope(getID(), pUnit->getID());
	}

	return pUnit;
}
/**************************************/

// R&R, ray, Port Royal
int CvPlayer::getPortRoyalUnitBuyPrice(UnitTypes eUnit) const
{
	CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);

	int iCost = kUnit.getPortRoyalCost();
	if (iCost < 0)
	{
		return iCost;
	}

	iCost += GET_TEAM(getTeam()).getUnitsPurchasedHistory((UnitClassTypes) kUnit.getUnitClassType()) * kUnit.getPortRoyalCostIncrease();

	iCost *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
	iCost /= 100;

	iCost *= GC.getEraInfo(GC.getGameINLINE().getStartEra()).getTrainPercent();
	iCost /= 100;

	/*
	for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); ++iTrait)
	{
		if (hasTrait((TraitTypes) iTrait))
		{
			iCost *= std::max(0, (100 - GC.getTraitInfo((TraitTypes) iTrait).getRecruitPriceDiscount()));
			iCost /= 100;
		}
	}*/

	if (!isHuman())
	{
		iCost *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAITrainPercent();
		iCost /= 100;

		iCost *= std::max(0, ((GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIPerEraModifier() * getCurrentEra()) + 100));
		iCost /= 100;
	}

	return iCost;
}

CvUnit* CvPlayer::buyPortRoyalUnit(UnitTypes eUnit, int iPriceModifier)
{
	FAssert(canTradeWithPortRoyal());
	if (!canTradeWithPortRoyal())
	{
		return NULL;
	}

	if (getPortRoyalUnitBuyPrice(eUnit) < 0)
	{
		return NULL;
	}

	int iPrice = getPortRoyalUnitBuyPrice(eUnit) * iPriceModifier / 100;
	if (iPrice > getGold())
	{
		m_aszTradeMessages.push_back(gDLL->getText("EUROPE_SCREEN_BUY_UNIT_LACK_FUNDS", GC.getUnitInfo(eUnit).getTextKeyWide(), iPrice));

		// TAC - Trade Messages - koma13 - START
		m_aeTradeMessageTypes.push_back(TRADE_MESSAGE_LACK_FUNDS);
		m_aeTradeMessageYields.push_back(NO_YIELD);
		m_aiTradeMessageAmounts.push_back(0);
		m_aiTradeMessageCommissions.push_back(0);
		// TAC - Trade Messages - koma13 - END

		gDLL->getInterfaceIFace()->setDirty(PortRoyalScreen_DIRTY_BIT, true);
		return NULL;
	}
	
	CvUnit* pUnit = NULL;
	CvPlot* pStartingPlot = getStartingPlot();
	if (GC.getUnitInfo(eUnit).getDomainType() == DOMAIN_SEA && pStartingPlot != NULL)
	{
		pUnit = initUnit(eUnit, GC.getUnitInfo(eUnit).getDefaultProfession(), INVALID_PLOT_COORD, INVALID_PLOT_COORD);
        if (pUnit != NULL)
		{
			pUnit->setUnitTravelState(UNIT_TRAVEL_STATE_IN_PORT_ROYAL, false);
			//add unit to map after setting Europe state so that it doesn't bump enemy units
			pUnit->addToMap(pStartingPlot->getX_INLINE(), pStartingPlot->getY_INLINE());
		}
	}
	else
	{
		pUnit = initPortRoyalUnit(eUnit);
	}

	FAssert(NULL != pUnit);
	if (NULL != pUnit)
	{
		changeGold(-iPrice);
		GET_TEAM(getTeam()).changeUnitsPurchasedHistory(pUnit->getUnitClassType(), 1);
		//gDLL->getEventReporterIFace()->unitBoughtFromEurope(getID(), pUnit->getID());
	}

	return pUnit;
}
// R&R, ray, Port Royal - END

void CvPlayer::buyUnitsFromKing()
{
	PlayerTypes eParent = getParent();
	if (eParent == NO_PLAYER)
	{
		return;
	}

	CvPlayer& kParent = GET_PLAYER(eParent);

	int iNumUnits;
	UnitTypes eUnit;
	ProfessionTypes eProfession;
	int iPrice = kParent.AI_militaryHelp(getID(), iNumUnits, eUnit, eProfession);

	int iPriceModifier = GC.getDefineINT("KING_BUY_UNIT_PRICE_MODIFIER");
	for (int i = 0; i < iNumUnits; ++i)
	{
		CvUnit* pUnit = buyEuropeUnit(eUnit, iPriceModifier);
		if (pUnit != NULL)
		{
			pUnit->setProfession(eProfession);
		}
	}

	if (iNumUnits > 0)
	{
		changeBellsStored(iNumUnits * getEuropeUnitBuyPrice(eUnit) * iPriceModifier / 100);
	}
}


int CvPlayer::getYieldTradedTotal(YieldTypes eYield) const
{
	FAssert(eYield >= 0);
	FAssert(eYield < NUM_YIELD_TYPES);

	return m_em_iYieldTradedTotal.get(eYield);
}

void CvPlayer::setYieldTradedTotal(YieldTypes eYield, int iValue)
{
	FAssert(eYield >= 0);
	FAssert(eYield < NUM_YIELD_TYPES);

	if(iValue != getYieldTradedTotal(eYield))
	{
		m_em_iYieldTradedTotal.set(eYield, iValue);
	}
}

// R&R, vetiarvind, Price dependent tax rate change - Start
int CvPlayer::getYieldScoreTotal(YieldTypes eYield) const
{
	FAssert(eYield >= 0);
	FAssert(eYield < NUM_YIELD_TYPES);

	return m_em_iYieldScoreTotal.get(eYield);
}

void CvPlayer::setYieldScoreTotal(YieldTypes eYield, int iValue)
{
	FAssert(eYield >= 0);
	FAssert(eYield < NUM_YIELD_TYPES);	

	m_em_iYieldScoreTotal.set(eYield, iValue);
	
}

void CvPlayer::changeYieldTradedTotal(YieldTypes eYield, int iChange, int iUnitPrice)
{
	if(iUnitPrice == -1)	//default parameter declared in header
	{		
			iUnitPrice = 10;//default score functionality 
			if (getParent() != NO_PLAYER)
			{
				CvPlayer& kEurope = GET_PLAYER(getParent());		
				if (kEurope.isEurope())
					iUnitPrice = kEurope.getYieldBuyPrice(eYield); 
			}		
	}
	
	double iMultiplier = iUnitPrice*0.1;
	setYieldScoreTotal(eYield, getYieldScoreTotal(eYield) + (int)(iChange*iMultiplier));
	setYieldTradedTotal(eYield, getYieldTradedTotal(eYield) + iChange);
}

/*
void CvPlayer::changeYieldTradedTotal(YieldTypes eYield, int iChange)
{
	setYieldTradedTotal(eYield, getYieldTradedTotal(eYield) + iChange);
}
*/

// R&R, vetiarvind, Price dependent tax rate change - END
void CvPlayer::changeYieldBoughtTotal(YieldTypes eYield, int iChange)
{
	setYieldBoughtTotal(eYield, getYieldBoughtTotal(eYield) + iChange);
}

YieldTypes CvPlayer::getHighestTradedYield() const
{
	YieldTypes eBestYield = NO_YIELD;
	for (int i = 0; i < NUM_YIELD_TYPES; i++)
	{
		YieldTypes eYield = (YieldTypes) i;
		if (isYieldEuropeTradable(eYield))
		{
			if (getCity(getHighestStoredYieldCityId(eYield)) != NULL)
			{
				if ((eBestYield == NO_YIELD) || (getYieldTradedTotal(eYield) > getYieldTradedTotal(eBestYield)))
				{
					eBestYield = eYield;
				}
			}
		}
	}

	return eBestYield;
}

int CvPlayer::getHighestStoredYieldCityId(YieldTypes eYield) const
{
	int iLoop;
	int iBestCityId = -1;
	int iBestAmount = 0;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		int iAmount = pLoopCity->getYieldStored(eYield);
		if(iAmount > iBestAmount)
		{
			iBestAmount = iAmount;
			iBestCityId = pLoopCity->getID();
		}
	}

	return iBestCityId;
}

int CvPlayer::getYieldBoughtTotal(YieldTypes eYield) const
{
	FAssert(eYield >= 0 && eYield < NUM_YIELD_TYPES);
	return m_em_iYieldBoughtTotal.get(eYield);
}

void CvPlayer::setYieldBoughtTotal(YieldTypes eYield, int iValue)
{
	FAssert(eYield >= 0 && eYield < NUM_YIELD_TYPES);
	m_em_iYieldBoughtTotal.set(eYield, iValue);
}

int CvPlayer::getCrossesStored() const
{
	return m_iCrossesStored;
}

void CvPlayer::changeCrossesStored(int iChange)
{
	m_iCrossesStored += iChange;
	FAssert(getCrossesStored() >= 0);
}

int CvPlayer::getBellsStored() const
{
	return m_iBellsStored;
}

void CvPlayer::changeBellsStored(int iChange)
{
	m_iBellsStored += iChange;
	FAssert(getBellsStored() >= 0);
}

int CvPlayer::getTaxRate() const
{
	return m_iTaxRate;
}

void CvPlayer::setTaxRate(int iValue)
{
	m_iTaxRate = iValue;
	FAssert(getTaxRate() >= 0);
}

void CvPlayer::changeTaxRate(int iChange)
{
	if (iChange != 0)
	{
		int iOldRate = getTaxRate();
		setTaxRate(iOldRate + iChange);

		//reset yields traded
		for(int i=0;i<NUM_YIELD_TYPES;i++)
		{
			setYieldTradedTotal((YieldTypes) i, 0);
			setYieldScoreTotal((YieldTypes) i, 0);// R&R, vetiarvind, price dependent tax rate change						 
		}

		PlayerTypes eParent = getParent();
		if (eParent != NO_PLAYER)
		{
			CvString szTextKey = (iOldRate < getTaxRate() ? "TXT_KEY_TAX_RATE_CHANGED" : "TXT_KEY_TAX_RATE_LOWERED");
			CvWString szBuffer = gDLL->getText(szTextKey.GetCString(), GET_PLAYER(eParent).getNameKey(), iOldRate, getTaxRate());
			gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_REVOLTSTART", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"));
		}
	}
}

int CvPlayer::getNativeCombatModifier() const
{
	return m_iNativeCombatModifier;
}

void CvPlayer::setNativeCombatModifier(int iValue)
{
	m_iNativeCombatModifier = iValue;
}

void CvPlayer::changeNativeCombatModifier(int iChange)
{
	setNativeCombatModifier(getNativeCombatModifier() + iChange);
}

int CvPlayer::getDominateNativeBordersCount() const
{
	return m_iDominateNativeBordersCount;
}

void CvPlayer::changeDominateNativeBordersCount(int iChange)
{
	if (iChange != 0)
	{
		m_iDominateNativeBordersCount += iChange;
		FAssert(getDominateNativeBordersCount() >= 0);

		GC.getMapINLINE().updateCulture();
	}
}

int CvPlayer::getRevolutionEuropeTradeCount() const
{
	return m_iRevolutionEuropeTradeCount;
}

void CvPlayer::changeRevolutionEuropeTradeCount(int iChange)
{
	if (iChange != 0)
	{
		m_iRevolutionEuropeTradeCount += iChange;
		FAssert(getRevolutionEuropeTradeCount() >= 0);

		validateTradeRoutes();

		if (GC.getGameINLINE().getActiveTeam() == getTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(ColoredPlots_DIRTY_BIT, true);
		}
	}
}

bool CvPlayer::canTradeWithEurope() const
{
	if (!isInRevolution())
	{
		return true;
	}

	if (getRevolutionEuropeTradeCount() > 0)
	{
		return true;
	}

	return false;
}


int CvPlayer::getSellToEuropeProfit(YieldTypes eYield, int iAmount) const
{
	if (getParent() == NO_PLAYER)
	{
		return 0;
	}

	if (!isYieldEuropeTradable(eYield))
	{
		return 0;
	}

	CvPlayer& kPlayerEurope = GET_PLAYER(getParent());

	int iPrice = iAmount * kPlayerEurope.getYieldBuyPrice(eYield);
	iPrice -= (iPrice * getTaxRate()) / 100;

	return iPrice;
}

void CvPlayer::doAction(PlayerActionTypes eAction, int iData1, int iData2, int iData3)
{
	switch (eAction)
	{
	case PLAYER_ACTION_BUY_EUROPE_UNIT:
		buyEuropeUnit((UnitTypes) iData1, 100);
		break;
	case PLAYER_ACTION_SELL_YIELD_UNIT:
		sellYieldUnitToEurope(getUnit(iData3), iData2, iData1);
		break;
	case PLAYER_ACTION_BUY_YIELD_UNIT:
		buyYieldUnitFromEurope((YieldTypes) iData1, iData2, getUnit(iData3));
		break;
	case PLAYER_ACTION_BUY_YIELD_UNIT_AFRICA: // R&R, ray, Africa
		buyYieldUnitFromAfrica((YieldTypes) iData1, iData2, getUnit(iData3));
		break;
	case PLAYER_ACTION_BUY_YIELD_UNIT_PORT_ROYAL: // R&R, ray, Port Royal
		buyYieldUnitFromPortRoyal((YieldTypes) iData1, iData2, getUnit(iData3));
		break;
	case PLAYER_ACTION_LOAD_UNIT_FROM_EUROPE:
		loadUnitFromEurope(getEuropeUnitById(iData1), getUnit(iData2));
		break;
	case PLAYER_ACTION_UNLOAD_UNIT_TO_EUROPE:
		unloadUnitToEurope(getUnit(iData1));
		break;
	case PLAYER_ACTION_TRANSFER_UNIT_IN_EUROPE:
		transferUnitInEurope(getUnit(iData1), getUnit(iData2));
		break;
	case PLAYER_ACTION_SET_OPTION:
		setOption((PlayerOptionTypes)iData1, (bool) iData2);
		break;
	case PLAYER_ACTION_AUTO_MOVES:
		setAutoMoves(true);
		break;
	case PLAYER_ACTION_TURN_COMPLETE:
		setEndTurn(true);
		break;
	case PLAYER_ACTION_APPLY_EVENT:
		applyEvent((EventTypes) iData1, iData2);
		break;
	case PLAYER_ACTION_CONVINCE_FATHER:
		GET_TEAM(getTeam()).convinceFather((FatherTypes) iData1, iData2 != 0);
		break;
	case PLAYER_ACTION_SET_CIVIC:
		setCivic((CivicOptionTypes) iData1, (CivicTypes) iData2);
		break;
	case PLAYER_ACTION_RECEIVE_GOODY:
		receiveRandomGoody(GC.getMapINLINE().plotByIndexINLINE(iData2), (GoodyTypes) iData1, getUnit(iData3));
		break;
	case PLAYER_ACTION_BUY_UNITS_FROM_KING:
		buyUnitsFromKing();
		break;
	case PLAYER_ACTION_HURRY:
		hurry((HurryTypes) iData1, iData2);
		break;
	case PLAYER_ACTION_EUROPE_CHANGE_PROFESSION:
		changeProfessionEurope(iData1, (ProfessionTypes) iData2);
		break;
	case PLAYER_ACTION_FEAT:
		setFeatAccomplished((FeatTypes) iData1, iData2 != 0);
		break;
	/*** TRIANGLETRADE 10/23/08 by DPII ***/
	case PLAYER_ACTION_BUY_AFRICA_UNIT:
		buyAfricaUnit((UnitTypes) iData1, 100);
		break;
	case PLAYER_ACTION_SELL_YIELD_UNIT_AFRICA:
		sellYieldUnitToAfrica(getUnit(iData3), iData2, iData1);
		break;
	case PLAYER_ACTION_LOAD_UNIT_FROM_AFRICA:
		loadUnitFromAfrica(getAfricaUnitById(iData1), getUnit(iData2));
		break;
	case PLAYER_ACTION_UNLOAD_UNIT_TO_AFRICA:
		unloadUnitToAfrica(getUnit(iData1));
		break;
	case PLAYER_ACTION_TRANSFER_UNIT_IN_AFRICA:
		transferUnitInAfrica(getUnit(iData1), getUnit(iData2));
		break;
	/**************************************/
	// R&R, ray, Port Royal
	case PLAYER_ACTION_BUY_PORT_ROYAL_UNIT:
		buyPortRoyalUnit((UnitTypes) iData1, 100);
		break;
	case PLAYER_ACTION_SELL_YIELD_UNIT_PORT_ROYAL:
		sellYieldUnitToPortRoyal(getUnit(iData3), iData2, iData1);
		break;
	case PLAYER_ACTION_LOAD_UNIT_FROM_PORT_ROYAL:
		loadUnitFromPortRoyal(getPortRoyalUnitById(iData1), getUnit(iData2));
		break;
	case PLAYER_ACTION_UNLOAD_UNIT_TO_PORT_ROYAL:
		unloadUnitToPortRoyal(getUnit(iData1));
		break;
	case PLAYER_ACTION_TRANSFER_UNIT_IN_PORT_ROYAL:
		transferUnitInPortRoyal(getUnit(iData1), getUnit(iData2));
		break;
	// R&R, ray, Port Royal - END
	case PLAYER_ACTION_NETWORK_DESYNC_LOG_WRITE:
		GC.getGameINLINE().writeDesyncLog();
		break;
	default:
		FAssertMsg(false, "Unknown action");
		break;
	}
}

int CvPlayer::getTradeYieldAmount(YieldTypes eYield, CvUnit* pTransport) const
{
	FAssert(pTransport != NULL);
	if (pTransport == NULL)
	{
		return 0;
	}

	if (!GC.getYieldInfo(eYield).isCargo())
	{
		return 0;
	}

	CvPlot *pPlot = pTransport->plot();
	FAssert(pPlot != NULL);

	int iAmount = 0;
	if (getID() == pTransport->getOwnerINLINE()) //offer yields on the transport
	{
		for (int i=0;i<pPlot->getNumUnits();i++)
		{
			CvUnit* pLoopUnit = pPlot->getUnitByIndex(i);
			if (pLoopUnit != NULL)
			{
				if (pLoopUnit->getTransportUnit() == pTransport)
				{
					if (pLoopUnit->getYield() == eYield)
					{
						iAmount += pLoopUnit->getYieldStored();
					}
				}
			}
		}
	}
	else //offer yields from the city
	{
		CvCity *pCity = pPlot->getPlotCity();
		FAssert(pCity != NULL);
		if (pCity == NULL)
		{
			return 0;
		}

		int iCityAmount = pCity->getYieldStored(eYield);
		int iAmountAvailable = pTransport->getLoadYieldAmount(eYield);
		iAmount = std::min(iCityAmount, iAmountAvailable);
	}

	return iAmount;
}

void CvPlayer::setCityBillboardDirty(bool bNewValue)
{
	int iLoop;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->setBillboardDirty(bNewValue);
	}
}

bool CvPlayer::isEurope() const
{
	if (getCivilizationType() == NO_CIVILIZATION)
	{
		return false;
	}

	if (GC.getCivilizationInfo(getCivilizationType()).getDerivativeCiv() == NO_CIVILIZATION)
	{
		return false;
	}

	return true;
}

bool CvPlayer::isInRevolution() const
{
	if (getParent() == NO_PLAYER)
	{
		return false;
	}

	if (!::atWar(getTeam(), GET_PLAYER(getParent()).getTeam()))
	{
		return false;
	}

	return true;
}

bool CvPlayer::checkIndependence() const
{
	if (getParent() == NO_PLAYER)
	{
		return false;
	}

	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kParent = GET_PLAYER((PlayerTypes) iPlayer);

		if (kParent.isAlive() && GET_TEAM(kParent.getTeam()).isParentOf(getTeam()))
		{
			// land units on map
			int iNumUnits = kParent.countNumTravelUnits(NO_UNIT_TRAVEL_STATE, DOMAIN_LAND);
			if (iNumUnits > 0)
			{
				return false;
			}

			// need both ships and land units
			int iNumLandUnits = kParent.countNumDomainUnits(DOMAIN_LAND);
			int iShips = kParent.countNumDomainUnits(DOMAIN_SEA);

			for (int i = 0; i < getNumRevolutionEuropeUnits(); ++i)
			{
				switch (GC.getUnitInfo(getRevolutionEuropeUnit(i)).getDomainType())
				{
				case DOMAIN_SEA:
					++iShips;
					break;
				case DOMAIN_LAND:
					++iNumLandUnits;
					break;
				default:
					break;
				}
			}

			if (iShips > 0 && iNumLandUnits > 0)
			{
				return false;
			}
		}
	}

	return true;
}


void CvPlayer::applyMissionaryPoints(CvCity* pCity)
{
	FAssert(pCity->getOwnerINLINE() == getID());
	PlayerTypes ePlayer = pCity->getMissionaryPlayer();
	if (ePlayer != NO_PLAYER)
	{
		int iModifier = 100 + getMissionaryRateModifier() + GET_PLAYER(ePlayer).getMissionaryRateModifier();
		changeMissionaryPoints(ePlayer, pCity->getMissionaryRate() * iModifier / 100);
		int iThreshold = missionaryThreshold(ePlayer);
		if (getMissionaryPoints(ePlayer) >= iThreshold)
		{
			//spawn converted native
			bool bUnitCreated = false;
			UnitClassTypes eUnitClass = (UnitClassTypes) GC.getCivilizationInfo(getCivilizationType()).getCapturedCityUnitClass();
			if (eUnitClass != NO_UNITCLASS)
			{
				UnitTypes eUnit = (UnitTypes) GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType()).getCivilizationUnits(eUnitClass);
				if (eUnit != NO_UNIT)
				{
					CvUnit* pUnit = GET_PLAYER(ePlayer).initUnit(eUnit, GC.getUnitInfo(eUnit).getDefaultProfession(), pCity->getX_INLINE(), pCity->getY_INLINE());
					if(pUnit != NULL)
					{
						bUnitCreated = true;
						gDLL->getEventReporterIFace()->missionaryConvertedUnit(pUnit);

						CvWString szBuffer = gDLL->getText("TXT_KEY_NATIVES_CONVERTED", pCity->getNameKey());
						gDLL->getInterfaceIFace()->addMessage(ePlayer, true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNIT_GREATPEOPLE", MESSAGE_TYPE_INFO, GC.getUnitInfo(eUnit).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"), pCity->getX_INLINE(), pCity->getY_INLINE(), true, true);

						changeMissionaryPoints(ePlayer, -iThreshold);
						setMissionaryThresholdMultiplier(ePlayer, (getMissionaryThresholdMultiplier(ePlayer) * (100 + GC.getDefineINT("MISSIONARY_THRESHOLD_INCREASE"))) / 100);
					}
				}
			}
			FAssert(bUnitCreated);
		}
	}
}

int CvPlayer::getMissionaryPoints(PlayerTypes ePlayer) const
{
	FAssert(ePlayer >= 0 && ePlayer < MAX_PLAYERS);
	return m_em_iMissionaryPoints.get(ePlayer);
}

void CvPlayer::changeMissionaryPoints(PlayerTypes ePlayer, int iChange)
{
	FAssert(ePlayer >= 0 && ePlayer < MAX_PLAYERS);
	if(iChange != 0)
	{
		m_em_iMissionaryPoints.add(ePlayer, iChange);
		FAssert(m_em_iMissionaryPoints.get(ePlayer) >= 0);
	}
}

int CvPlayer::getMissionaryThresholdMultiplier(PlayerTypes ePlayer) const
{
	FAssert(ePlayer >= 0 && ePlayer < MAX_PLAYERS);
	return m_em_iMissionaryThresholdMultiplier.get(ePlayer);
}

void CvPlayer::setMissionaryThresholdMultiplier(PlayerTypes ePlayer, int iValue)
{
	FAssert(ePlayer >= 0 && ePlayer < MAX_PLAYERS);
	m_em_iMissionaryThresholdMultiplier.set(ePlayer, iValue);
	FAssert(getMissionaryThresholdMultiplier(ePlayer) > 0);
}

int CvPlayer::missionaryThreshold(PlayerTypes ePlayer) const
{
	int iThreshold = ((GC.getDefineINT("MISSIONARY_THRESHOLD") * std::max(0, (getMissionaryThresholdMultiplier(ePlayer)))) / 100);

	iThreshold *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
	iThreshold /= 100;

	iThreshold *= GC.getEraInfo(GC.getGameINLINE().getStartEra()).getGrowthPercent();
	iThreshold /= 100;

	return std::max(1, iThreshold);
}

void CvPlayer::burnMissions(PlayerTypes ePlayer)
{
	int iLoop;
	for (CvCity* pCity = firstCity(&iLoop); pCity != NULL; pCity = nextCity(&iLoop))
	{
		if (pCity->getMissionaryPlayer() == ePlayer)
		{
			pCity->setMissionaryPlayer(NO_PLAYER);
		}
	}
}

bool CvPlayer::canHaveMission(PlayerTypes ePlayer) const
{
	if (!isNative())
	{
		return false;
	}

	if (!GET_PLAYER(ePlayer).isAlive())
	{
		return false;
	}

	if (GET_PLAYER(ePlayer).isNative())
	{
		return false;
	}

	if (::atWar(GET_PLAYER(ePlayer).getTeam(), getTeam()))
	{
		return false;
	}

	return true;
}

// WTP, ray, Native Trade Posts - START
bool CvPlayer::canHaveTradePost(PlayerTypes ePlayer) const
{
	if (!isNative())
	{
		return false;
	}

	if (!GET_PLAYER(ePlayer).isAlive())
	{
		return false;
	}

	if (GET_PLAYER(ePlayer).isNative())
	{
		return false;
	}

	if (::atWar(GET_PLAYER(ePlayer).getTeam(), getTeam()))
	{
		return false;
	}

	return true;
}

void CvPlayer::burnTradePosts(PlayerTypes ePlayer)
{
	int iLoop;
	for (CvCity* pCity = firstCity(&iLoop); pCity != NULL; pCity = nextCity(&iLoop))
	{
		if (pCity->getTradePostPlayer() == ePlayer)
		{
			pCity->setTradePostPlayer(NO_PLAYER);
			pCity->setNativeTradePostGold(0);
		}
	}
}
// WTP, ray, Native Trade Posts - END

void CvPlayer::validateMissions()
{
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		if (!canHaveMission((PlayerTypes) iPlayer))
		{
			burnMissions((PlayerTypes) iPlayer);
		}
	}
}

// WTP, ray, Native Trade Posts - START
void CvPlayer::validateTradePosts()
{
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		if (!canHaveTradePost((PlayerTypes) iPlayer))
		{
			burnTradePosts((PlayerTypes) iPlayer);
		}
	}
}
// WTP, ray, Native Trade Posts - END

int CvPlayer::getMissionaryRateModifier() const
{
	return m_iMissionaryRateModifier;
}

int CvPlayer::getNativeTradeModifier() const // R&R, ray, new Attribute in Traits
{
	return m_iNativeTradeModifier;
}

void  CvPlayer::changeMissionaryRateModifier(int iChange)
{
	m_iMissionaryRateModifier += iChange;
}

void  CvPlayer::changeNativeTradeModifier(int iChange) // R&R, ray, new Attribute in Traits
{
	m_iNativeTradeModifier += iChange;
}

int CvPlayer::getMissionarySuccessPercent() const
{
	return m_iMissionarySuccessPercent;
}

void CvPlayer::setMissionarySuccessPercent(int iValue)
{
	m_iMissionarySuccessPercent = iValue;
}

// WTP, ray, Native Trade Posts - START
int CvPlayer::getNativeTradePostSuccessPercent() const
{
	return m_iNativeTradePostSuccessPercent;
}

void CvPlayer::setNativeTradePostSuccessPercent(int iValue)
{
	m_iNativeTradePostSuccessPercent = iValue;
}

int CvPlayer::getRebelCombatPercent() const
{
	int iPercent = 100;

	for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); ++iTrait)
	{
		TraitTypes eTrait = (TraitTypes) iTrait;
		CvTraitInfo& kTrait = GC.getTraitInfo(eTrait);
		if (kTrait.getRebelCombatModifier() != 0)
		{
			if (hasTrait(eTrait))
			{
				iPercent += kTrait.getRebelCombatModifier();
			}
		}
	}

	iPercent *= GC.getDefineINT("MAX_REBEL_COMBAT_MULTIPLER");
	iPercent /= 100;

	return iPercent;
}

int CvPlayer::getProfessionEquipmentModifier(ProfessionTypes eProfession) const
{
	FAssert(eProfession >= 0 && eProfession < GC.getNumProfessionInfos());
	return m_em_iProfessionEquipmentModifier.get(eProfession);
}

void CvPlayer::setProfessionEquipmentModifier(ProfessionTypes eProfession, int iValue)
{
	FAssert(eProfession >= 0 && eProfession < GC.getNumProfessionInfos());
	int iChange = iValue - getProfessionEquipmentModifier(eProfession);
	if (iChange != 0)
	{
		std::vector<CvUnit*> aProfessionUnits;
		int iLoop;
		for (CvUnit* pUnit = firstUnit(&iLoop); pUnit != NULL; pUnit = nextUnit(&iLoop))
		{
			if (pUnit->getProfession() == eProfession)
			{
				aProfessionUnits.push_back(pUnit);
			}
		}
		for (uint i = 0; i < m_aEuropeUnits.size(); ++i)
		{
			CvUnit* pUnit = m_aEuropeUnits[i];
			if (pUnit->getProfession() == eProfession)
			{
				aProfessionUnits.push_back(pUnit);
			}
		}
		for (CvCity* pCity = firstCity(&iLoop); pCity != NULL; pCity = nextCity(&iLoop))
		{
			for (int i = 0; i < pCity->getPopulation(); ++i)
			{
				CvUnit* pUnit = pCity->getPopulationUnitByIndex(i);
				if (pUnit->getProfession() == eProfession)
				{
					aProfessionUnits.push_back(pUnit);
				}
			}
		}

		for (uint i = 0; i < aProfessionUnits.size(); ++i)
		{
			CvUnit* pUnit = aProfessionUnits[i];
			changeAssets(-pUnit->getAsset());
			int iPower = pUnit->getPower();
			changePower(-iPower);
			CvArea* pArea = pUnit->area();
			if (pArea != NULL)
			{
				pArea->changePower(getID(), -iPower);
			}
		}

		m_em_iProfessionEquipmentModifier.set(eProfession, iValue);

		for (uint i = 0; i < aProfessionUnits.size(); ++i)
		{
			CvUnit* pUnit = aProfessionUnits[i];
			changeAssets(pUnit->getAsset());
			int iPower = pUnit->getPower();
			changePower(iPower);
			CvArea* pArea = pUnit->area();
			if (pArea != NULL)
			{
				pArea->changePower(getID(), iPower);
			}
		}

		FAssert(getProfessionEquipmentModifier(eProfession) >= -100);
		Update_cache_YieldEquipmentAmount(eProfession); // cache CvPlayer::getYieldEquipmentAmount - Nightinggale
	}
}

// cache CvPlayer::getYieldEquipmentAmount - function rename - Nightinggale
int CvPlayer::getYieldEquipmentAmountUncached(ProfessionTypes eProfession, YieldTypes eYield) const
{
	FAssert(eProfession >= 0 && eProfession < GC.getNumProfessionInfos());
	FAssert(eYield >= 0 && eYield < NUM_YIELD_TYPES);

	int iAmount = GC.getProfessionInfo(eProfession).getYieldEquipmentAmount(eYield);

	iAmount *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
	iAmount /= 100;

	if (!isHuman())
	{
		iAmount *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAITrainPercent();
		iAmount /= 100;
	}

	iAmount *= 100 + getProfessionEquipmentModifier(eProfession);
	iAmount /= 100;

	return std::max(0, iAmount);
}

// cache CvPlayer::getYieldEquipmentAmount - start - Nightinggale
void CvPlayer::Update_cache_YieldEquipmentAmount(ProfessionTypes eProfession)
{
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++) {
		m_cache_YieldEquipmentAmount[eProfession].set(getYieldEquipmentAmountUncached(eProfession, (YieldTypes)iYield), iYield);
	}
	m_cache_YieldEquipmentAmount[eProfession].isEmpty(); // This will release the array if it's empty
}

void CvPlayer::Update_cache_YieldEquipmentAmount()
{
	///TKs Nightinggale fix
	if (m_eID <= NO_PLAYER || GC.getGameINLINE().getHandicapType() == NO_HANDICAP)
	{
		// Some update calls gets triggered during player init. They can safely be ignored.
		return;
	}

	for (int iProfession = 0; iProfession < GC.getNumProfessionInfos(); iProfession++) {
		Update_cache_YieldEquipmentAmount((ProfessionTypes)iProfession);
	}
}
// cache CvPlayer::getYieldEquipmentAmount - end - Nightinggale
bool CvPlayer::isProfessionValid(ProfessionTypes eProfession, UnitTypes eUnit) const
{
	if (eProfession != NO_PROFESSION)
	{
		// CivEffect check
		if (!CivEffect()->canUseProfession(eProfession))
		{
			return false;
		}

		if (!GC.getCivilizationInfo(getCivilizationType()).isValidProfession(eProfession))
		{
			return false;
		}
		//TAC Whaling, ray
		if (eUnit != NO_UNIT)
		{
			if (GC.getUnitInfo(eUnit).isOnlyDefensive())
			{
				// Natives cannot be combat professions
				CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);
				// WTP, ray, Native Trade Posts - START
				// Adjustment for Native Trader
				if ((!kProfession.isUnarmed() && kProfession.getCombatChange() > 0 && !kProfession.isScout()) || kProfession.getMissionaryRate() > 0 || kProfession.getNativeTradeRate() > 0)
				{
					return false;
				}
			}

			if (GC.getUnitInfo(eUnit).getDefaultProfession() == NO_PROFESSION)
			{
				if (!GC.getUnitInfo(eUnit).isGatherBoat())
				{
					return false;
				}
				// R&R, ray, High Sea Fishing
				else if (eProfession == (ProfessionTypes)GC.getPROFESSION_WHALING_BOAT_WORKING() && GC.getUnitInfo(eUnit).getUnitClassType() != (UnitClassTypes)GC.getUNITCLASS_WHALING_BOAT())
				{
					return false;
				}
				else if (eProfession == (ProfessionTypes)GC.getPROFESSION_FISHING_BOAT_WORKING() && GC.getUnitInfo(eUnit).getUnitClassType() != (UnitClassTypes)GC.getUNITCLASS_FISHING_BOAT())
				{
					return false;
				}
			}
		}
		// R&R, ray, High Sea Fishing
		//else if (eProfession == (ProfessionTypes)GC.getDefineINT("PROFESSION_WHALING_BOAT_WORKING"))
		else if (eProfession == (ProfessionTypes)GC.getPROFESSION_WHALING_BOAT_WORKING() || eProfession == (ProfessionTypes)GC.getPROFESSION_FISHING_BOAT_WORKING())
		{
			return false;
		}
		//End TAC Whaling, ray

	}

	return true;
}

void CvPlayer::doPrices()
{
	if (isEurope())
	{
		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			YieldTypes eYield = (YieldTypes) iYield;
			CvYieldInfo& kYield = GC.getYieldInfo(eYield);

			if (kYield.isCargo())
			{
				// R&R, Androrc Price Recovery
				GC.getGameINLINE().changeYieldBoughtTotal(getID(), eYield, kYield.getEuropeVolumeAttrition());
				//Androrc End

				int iBaseThreshold = kYield.getPriceChangeThreshold() * GC.getHandicapInfo(getHandicapType()).getEuropePriceThresholdMultiplier() * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent() / 10000;
				int iNewPrice = kYield.getBuyPriceLow() + GC.getGameINLINE().getSorenRandNum(kYield.getBuyPriceHigh() - kYield.getBuyPriceLow() + 1, "Price selection");
				iNewPrice += getYieldBoughtTotal(eYield) / std::max(1, iBaseThreshold);

				if (GC.getGameINLINE().getSorenRandNum(100, "Price correction") < kYield.getPriceCorrectionPercent() * std::abs(iNewPrice - getYieldBuyPrice(eYield)))
				{
					iNewPrice = std::min(iNewPrice, getYieldBuyPrice(eYield) + 1);
					iNewPrice = std::max(iNewPrice, getYieldBuyPrice(eYield) - 1);
					setYieldBuyPrice(eYield, iNewPrice, true);
				}
			}
		}
	}

	//do tax rate change
	if (!GC.getEraInfo(getCurrentEra()).isRevolution())
	{
		PlayerTypes eParent = getParent();
		if (eParent != NO_PLAYER)
		{
			if (getHighestTradedYield() != NO_YIELD)
			{
				// R&R, vetiarvind, Price dependent tax rate change - START				
				/*
				int iTotalTraded = 0;
				for (int i = 0; i < NUM_YIELD_TYPES; i++)
				{
					if (isYieldEuropeTradable((YieldTypes)i))
					{
						iTotalTraded += getYieldTradedTotal((YieldTypes) i);
					}
				}*/
				int iTotalScore = 0; 
				for (int i = 0; i < NUM_YIELD_TYPES; i++)
				{
					if (isYieldEuropeTradable((YieldTypes)i))
					{
						iTotalScore += getYieldScoreTotal((YieldTypes) i);
					}
				}
				// R&R, vetiarvind, Price dependent tax rate change - END

				//modify the traded threshold
				int iMultiplier = 100;
				for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); ++iTrait)
				{
					TraitTypes eTrait = (TraitTypes) iTrait;
					CvTraitInfo& kTrait = GC.getTraitInfo(eTrait);
					if (hasTrait(eTrait))
					{
						iMultiplier += kTrait.getTaxRateThresholdModifier();
					}
				}
				iMultiplier += getTaxRate() * GC.getTAX_TRADE_THRESHOLD_TAX_RATE_PERCENT() / 100;
				// R&R, ray, Improvements to Tax Mechanism
				//iMultiplier += GET_PLAYER(eParent).AI_getAttitudeVal(getID()) * GC.getDefineINT("TAX_TRADE_THRESHOLD_ATTITUDE_PERCENT");

				// R&R, vetiarvind, Price dependent tax rate change - START
				//if (iTotalTraded * 10000 > GC.getTAX_TRADE_THRESHOLD() * std::max(100, iMultiplier) * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent())
				
				//compare total traded with trade threshold
				if (iTotalScore * 10000 > GC.getTAX_TRADE_THRESHOLD() * std::max(100, iMultiplier) * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent())								
				{
					// R&R, vetiarvind, Price dependent tax rate change - END				
					//random chance to raise tax rate
					// R&R, ray, Improvements to Tax Mechanism - START
					int iTaxIncreaseChanceModifierFromKingAttitude = GET_PLAYER(eParent).AI_getAttitudeVal(getID()) * GC.getTAX_TRADE_INCREASE_CHANCE_KING_ATTITUDE_BASE();
					if (iTaxIncreaseChanceModifierFromKingAttitude > 50)
					{
						iTaxIncreaseChanceModifierFromKingAttitude = 50;
					}
					if (iTaxIncreaseChanceModifierFromKingAttitude < -50)
					{
						iTaxIncreaseChanceModifierFromKingAttitude = -50;
					}
					//if(GC.getGameINLINE().getSorenRandNum(100, "Tax rate increase") < GC.getDefineINT("TAX_INCREASE_CHANCE"))
					if(GC.getGameINLINE().getSorenRandNum(100 + iTaxIncreaseChanceModifierFromKingAttitude, "Tax rate increase") < GC.getTAX_INCREASE_CHANCE())
					// R&R, ray, Improvements to Tax Mechanism - END
					{
						int iOldTaxRate = getTaxRate();

						/** NBMOD TAX **/
						/** Original
						int iNewTaxRate = std::min(99, iOldTaxRate + 1 + GC.getGameINLINE().getSorenRandNum(GC.getDefineINT("TAX_RATE_MAX_INCREASE"), "Tax Rate Increase"));
						int iChange = iNewTaxRate - iOldTaxRate;

						if (isHuman())
						{
							CvDiploParameters* pDiplo = new CvDiploParameters(eParent);
							pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_KISS_PINKY"));
							pDiplo->addDiploCommentVariable(iOldTaxRate);
							pDiplo->addDiploCommentVariable(iNewTaxRate);
							pDiplo->setData(iChange);
							pDiplo->setAIContact(true);
							gDLL->beginDiplomacy(pDiplo, getID());
						}
						else
						{
							changeTaxRate(iChange);
						}
						**/

						int iNewTaxRate = NBMOD_GetNewTaxRate(std::min(99, iOldTaxRate + 1 + GC.getGameINLINE().getSorenRandNum(GC.getDefineINT("TAX_RATE_MAX_INCREASE"), "Tax Rate Increase")));
						int iChange = iNewTaxRate - iOldTaxRate;

						if (iChange > 0 )
						{
							if (isHuman())
							{
								CvDiploParameters* pDiplo = new CvDiploParameters(eParent);
								pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_KISS_PINKY"));
								pDiplo->addDiploCommentVariable(iOldTaxRate);
								pDiplo->addDiploCommentVariable(iNewTaxRate);
								pDiplo->setData(iChange);
								pDiplo->setAIContact(true);
								gDLL->beginDiplomacy(pDiplo, getID());
							}
							else
							{
								// TAC - AI TaxRate - koma13 - START
								//changeTaxRate(iChange);
								if (iOldTaxRate < GC.getHandicapInfo(getHandicapType()).getAIMaxTaxrate())
								{
									changeTaxRate(iChange);
								}
								// TAC - AI TaxRate - koma13 - END
							}
						}

						/** NBMOD TAX **/
					}
				}
			}
		}
	}
}

// R&R, ray, Africa
void CvPlayer::doAfricaPrices()
{
	if (isEurope())
	{
		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			YieldTypes eYield = (YieldTypes) iYield;
			CvYieldInfo& kYield = GC.getYieldInfo(eYield);

			if (kYield.isCargo())
			{
				int iBaseThreshold = kYield.getPriceChangeThreshold() * GC.getHandicapInfo(getHandicapType()).getEuropePriceThresholdMultiplier() * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent() / 10000;
				int iNewPrice = kYield.getAfricaBuyPriceLow() + GC.getGameINLINE().getSorenRandNum(kYield.getAfricaBuyPriceHigh() - kYield.getAfricaBuyPriceLow() + 1, "Price selection");
				iNewPrice += getYieldBoughtTotal(eYield) / std::max(1, iBaseThreshold); // maybe this should be changed

				if (GC.getGameINLINE().getSorenRandNum(100, "Price correction") < kYield.getPriceCorrectionPercent() * std::abs(iNewPrice - getYieldAfricaBuyPrice(eYield)))
				{
					iNewPrice = std::min(iNewPrice, getYieldAfricaBuyPrice(eYield) + 1);
					iNewPrice = std::max(iNewPrice, getYieldAfricaBuyPrice(eYield) - 1);
					setYieldAfricaBuyPrice(eYield, iNewPrice, true);
				}
			}
		}
	}
}
// R&R, ray, Africa - END

// R&R, ray, Port Royal
void CvPlayer::doPortRoyalPrices()
{
	if (isEurope())
	{
		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			YieldTypes eYield = (YieldTypes) iYield;
			CvYieldInfo& kYield = GC.getYieldInfo(eYield);

			if (kYield.isCargo())
			{
				int iBaseThreshold = kYield.getPriceChangeThreshold() * GC.getHandicapInfo(getHandicapType()).getEuropePriceThresholdMultiplier() * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent() / 10000;
				int iNewPrice = kYield.getPortRoyalBuyPriceLow() + GC.getGameINLINE().getSorenRandNum(kYield.getPortRoyalBuyPriceHigh() - kYield.getPortRoyalBuyPriceLow() + 1, "Price selection");
				iNewPrice += getYieldBoughtTotal(eYield) / std::max(1, iBaseThreshold); // maybe this should be changed

				if (GC.getGameINLINE().getSorenRandNum(100, "Price correction") < kYield.getPriceCorrectionPercent() * std::abs(iNewPrice - getYieldPortRoyalBuyPrice(eYield)))
				{
					iNewPrice = std::min(iNewPrice, getYieldPortRoyalBuyPrice(eYield) + 1);
					iNewPrice = std::max(iNewPrice, getYieldPortRoyalBuyPrice(eYield) - 1);
					setYieldPortRoyalBuyPrice(eYield, iNewPrice, true);
				}
			}
		}
	}
}
// R&R, ray, Port Royal - END

int CvPlayer::getNumRevolutionEuropeUnits() const
{
	return m_aEuropeRevolutionUnits.size();
}

UnitTypes CvPlayer::getRevolutionEuropeUnit(int i) const
{
	FAssert(i >= 0 && i < getNumRevolutionEuropeUnits());

	return m_aEuropeRevolutionUnits[i].first;
}

ProfessionTypes CvPlayer::getRevolutionEuropeProfession(int i) const
{
	FAssert(i >= 0 && i < getNumRevolutionEuropeUnits());

	return m_aEuropeRevolutionUnits[i].second;
}

void CvPlayer::addRevolutionEuropeUnit(UnitTypes eUnit, ProfessionTypes eProfession)
{
	m_aEuropeRevolutionUnits.push_back(std::make_pair(eUnit, eProfession));
}

void CvPlayer::clearRevolutionEuropeUnits()
{
	m_aEuropeRevolutionUnits.clear();
}

UnitTypes CvPlayer::getDocksNextUnit(int i) const
{
	if (!canTradeWithEurope())
	{
		return NO_UNIT;
	}

	FAssert(i < (int)m_aDocksNextUnits.size());
	return m_aDocksNextUnits[i];
}

UnitTypes CvPlayer::pickBestImmigrant()
{
	std::vector<int> aiWeights(NUM_UNIT_TYPES, 0);
	for (UnitTypes eUnit = FIRST_UNIT; eUnit < NUM_UNIT_TYPES; ++eUnit)
	{
		if (CivEffect()->canUseUnit(eUnit) && CivEffect()->canUseImmigrant(eUnit))
		{
			const CvUnitInfo& kInfo = GC.getUnitInfo(eUnit);
			int iWeight = kInfo.getImmigrationWeight();
			for (int i = 0; i < getUnitClassImmigrated(static_cast<UnitClassTypes>(kInfo.getUnitClassType())); ++i)
			{
				iWeight *= std::max(0, 100 - kInfo.getImmigrationWeightDecay());
				iWeight /= 100;
			}

			aiWeights[eUnit] += iWeight;
		}
	}

	UnitTypes eBestUnit = (UnitTypes) GC.getGameINLINE().getSorenRand().pickValue(aiWeights, "pick immigrant");
	FAssert(NO_UNIT != eBestUnit);
	if (eBestUnit != NO_UNIT)
	{
		changeUnitClassImmigrated((UnitClassTypes) GC.getUnitInfo(eBestUnit).getUnitClassType(), 1);
	}

	return eBestUnit;
}

bool CvPlayer::canHurry(HurryTypes eHurry, int iIndex) const
{
	CvHurryInfo& kHurry = GC.getHurryInfo(eHurry);

	if (getHurryCount(eHurry) > 0)
	{
		if (kHurry.getGoldPerCross() > 0)
		{
			if (canTradeWithEurope())
			{
				if (immigrationThreshold() > getCrossesStored())
				{
					if (getHurryGold(eHurry, iIndex) <= getGold())
					{
						return true;
					}
				}
			}
		}
		else
		{
			return true;
		}
	}

	return false;
}

void CvPlayer::hurry(HurryTypes eHurry, int iIndex)
{
	if (!canHurry(eHurry, iIndex))
	{
		int iPrice = getHurryGold(eHurry, iIndex);
		if (iPrice >= getGold())
		{
			m_aszTradeMessages.push_back(gDLL->getText("EUROPE_SCREEN_RECRUIT_UNIT_LACK_FUNDS", GC.getUnitInfo(getDocksNextUnit(iIndex)).getTextKeyWide(), iPrice));

			// TAC - Trade Messages - koma13 - START
			m_aeTradeMessageTypes.push_back(TRADE_MESSAGE_LACK_FUNDS);
			m_aeTradeMessageYields.push_back(NO_YIELD);
			m_aiTradeMessageAmounts.push_back(0);
			m_aiTradeMessageCommissions.push_back(0);
			// TAC - Trade Messages - koma13 - END

			gDLL->getInterfaceIFace()->setDirty(EuropeScreen_DIRTY_BIT, true);
		}
	}
	else
	{
		CvHurryInfo& kHurry = GC.getHurryInfo(eHurry);

		if (kHurry.getGoldPerCross() > 0)
		{
			changeGold(-getHurryGold(eHurry, iIndex));
			changeCrossesStored(immigrationThreshold() - getCrossesStored());
			// TAC - short messages for immigration after fist - RAY
			doImmigrant(iIndex, true);
		}
	}
}

int CvPlayer::getHurryGold(HurryTypes eHurry, int iIndex) const
{
	int iThreshold = immigrationThreshold();
	int iCrossesLeft = iThreshold - getCrossesStored();
	int iGold = GC.getHurryInfo(eHurry).getGoldPerCross() * iCrossesLeft;
	iGold += GC.getHurryInfo(eHurry).getFlatGold() * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent() / 100;
	if (iIndex != -1)
	{
		int iImmigrationPrice = std::abs(getEuropeUnitBuyPrice(getDocksNextUnit(iIndex))) * iCrossesLeft / std::max(1, iThreshold);
		iGold = std::min(iGold, iImmigrationPrice);
	}

	return iGold;
}

const wchar* CvPlayer::getHurryItemTextKey(HurryTypes eHurry, int iData) const
{
	CvHurryInfo& kHurry = GC.getHurryInfo(eHurry);

	if (kHurry.getGoldPerCross() > 0)
	{
		UnitTypes eUnit = getDocksNextUnit(iData);
		if (eUnit != NO_UNIT)
		{
			return GC.getUnitInfo(eUnit).getTextKeyWide();
		}
	}

	return L"";
}


void CvPlayer::doImmigrant(int iIndex, bool shortmessage)
{
	if (getParent() != NO_PLAYER && canTradeWithEurope())
	{
		UnitTypes eBestUnit = getDocksNextUnit(iIndex);
		if (NO_UNIT != eBestUnit)
		{
			m_aDocksNextUnits[iIndex] = pickBestImmigrant();

			changeCrossesStored(-immigrationThreshold());
			setImmigrationThresholdMultiplier((getImmigrationThresholdMultiplier() * (100 + GC.getIMMIGRATION_THRESHOLD_INCREASE())) / 100);

			CvUnit* pUnit = initEuropeUnit(eBestUnit);

			CvWString szBuffer;

			// TAC - short messages for immigration after fist - RAY
			if(shortmessage) {
				szBuffer = gDLL->getText("TXT_KEY_NEW_IMMIGRANT_AVAILABLE_SHORT", GC.getCivilizationInfo(GET_PLAYER(getParent()).getCivilizationType()).getShortDescriptionKey(),  GC.getUnitInfo(eBestUnit).getTextKeyWide());
			}

			else {
				szBuffer = gDLL->getText("TXT_KEY_NEW_IMMIGRANT_AVAILABLE", GC.getCivilizationInfo(GET_PLAYER(getParent()).getCivilizationType()).getShortDescriptionKey(),  GC.getUnitInfo(eBestUnit).getTextKeyWide());
			}

			gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNIT_GREATPEOPLE", MESSAGE_TYPE_INFO, GC.getUnitInfo(eBestUnit).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"));

			gDLL->getInterfaceIFace()->setDirty(EuropeScreen_DIRTY_BIT, true);

			FAssert(pUnit != NULL);
			if(pUnit != NULL)
			{
				gDLL->getEventReporterIFace()->emmigrantAtDocks(getID(), pUnit->getID());
			}
		}
	}
}

void CvPlayer::buyLand(CvPlot* pPlot, bool bFree)
{
	int iCulture = pPlot->getBuyCultureAmount(getID());
	int iGoldCost = bFree ? 0 : pPlot->getBuyPrice(getID());

	if (getGold() < iGoldCost)
	{
		return;
	}

	PlayerTypes eOldOwner = pPlot->getOwnerINLINE();
	if (eOldOwner != NO_PLAYER)
	{
		pPlot->setCulture(eOldOwner, 0, false);
		if (!GET_TEAM(pPlot->getTeam()).isAtWar(getTeam()))
		{
			GET_PLAYER(eOldOwner).changeGold((iGoldCost * GC.getDefineINT("BUY_PLOT_SELLER_INCOME_PERCENT")) / 100);
			GET_PLAYER(getID()).AI_changeGoldTradedTo(eOldOwner, iGoldCost);
		}
	}

	pPlot->changeCulture(getID(), iCulture, true);
	changeGold(-iGoldCost);
}

int CvPlayer::getNumTradeMessages() const
{
	return m_aszTradeMessages.size();
}

const wchar* CvPlayer::getTradeMessage(int i) const
{
	return m_aszTradeMessages[i].GetCString();
}

// TAC - Trade Messages - koma13 - START
TradeMessageTypes CvPlayer::getTradeMessageType(int i) const
{
	return m_aeTradeMessageTypes[i];
}

YieldTypes CvPlayer::getTradeMessageYield(int i) const
{
	return m_aeTradeMessageYields[i];
}

int CvPlayer::getTradeMessageAmount(int i) const
{
	return m_aiTradeMessageAmounts[i];
}

int CvPlayer::getTradeMessageCommission(int i) const
{
	return m_aiTradeMessageCommissions[i];
}
// TAC - Trade Messages - koma13 - END

void CvPlayer::buildTradeTable(PlayerTypes eOtherPlayer, CLinkList<TradeData>& ourList, const IDInfo& kTransport) const
{
	TradeData item;
	int iLoop;

	//	Gold
	setTradeItem(&item, TRADE_GOLD, 0, &kTransport);
	if (canTradeItem(eOtherPlayer, item))
	{
		ourList.insertAtEnd(item);
	}

	//	Maps
	setTradeItem(&item, TRADE_MAPS, 0, &kTransport);
	if (canTradeItem(eOtherPlayer, item))
	{
		ourList.insertAtEnd(item);
	}

	//	Open Borders
	setTradeItem(&item, TRADE_OPEN_BORDERS, 0, &kTransport);
	if (canTradeItem(eOtherPlayer, item))
	{
		ourList.insertAtEnd(item);
	}

	//	Defensive Pact
	setTradeItem(&item, TRADE_DEFENSIVE_PACT, 0, &kTransport);
	if (canTradeItem(eOtherPlayer, item))
	{
		ourList.insertAtEnd(item);
	}

	//	Permanent Alliance
	setTradeItem(&item, TRADE_PERMANENT_ALLIANCE, 0, &kTransport);
	if (canTradeItem(eOtherPlayer, item))
	{
		ourList.insertAtEnd(item);
	}

	if (::atWar(getTeam(), GET_PLAYER(eOtherPlayer).getTeam()))
	{
		//	We are at war, allow a peace treaty option
		setTradeItem(&item, TRADE_PEACE_TREATY, 0, &kTransport);
		ourList.insertAtEnd(item);
	}

	//	Initial build of the inventory lists and buttons.
	//	Go through all the possible headings
	for (int i = NUM_BASIC_ITEMS; i < NUM_TRADEABLE_HEADINGS; i++)
	{
		bool bFoundItemUs = false;

		//	Build what we need to build for this item
		switch (i)
		{
		case TRADE_YIELD:

			FAssert(0 < GC.getNUM_YIELD_TYPES());
			{
				CvUnit* pTransport = ::getUnit(kTransport);
				if (pTransport != NULL)
				{
					for (int j = 0; j < NUM_YIELD_TYPES; j++)
					{
						setTradeItem(&item, TRADE_YIELD, j, &kTransport);
						if (canTradeItem(eOtherPlayer, item))
						{
							bFoundItemUs = true;
							ourList.insertAtEnd(item);
						}
					}
				}
			}
			break;

		case TRADE_CITIES:
			for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				setTradeItem(&item, TRADE_CITIES, pLoopCity->getID(), &kTransport);
				if (canTradeItem(eOtherPlayer, item))
				{
					bFoundItemUs = true;
					ourList.insertAtEnd(item);
				}
			}
			break;

		case TRADE_PEACE:
			if (!isHuman())
			{
				for (int j = 0; j < MAX_TEAMS; j++)
				{
					if (GET_TEAM((TeamTypes)j).isAlive())
					{
						if (j != getTeam() && j != GET_PLAYER(eOtherPlayer).getTeam())
						{
							setTradeItem(&item, TRADE_PEACE, j, &kTransport);
							if (canTradeItem(eOtherPlayer, item))
							{
								ourList.insertAtEnd(item);
								bFoundItemUs = true;
							}
						}
					}
				}
			}
			break;

		case TRADE_WAR:
			if (!isHuman())
			{
				for (int j = 0; j < MAX_TEAMS; j++)
				{
					if (GET_TEAM((TeamTypes)j).isAlive())
					{
						if (j != getTeam() && j != GET_PLAYER(eOtherPlayer).getTeam())
						{
							setTradeItem(&item, TRADE_WAR, j, &kTransport);
							if (canTradeItem(eOtherPlayer, item))
							{
								ourList.insertAtEnd(item);
								bFoundItemUs = true;
							}
						}
					}
				}
			}
			break;

		case TRADE_EMBARGO:
			if (!isHuman())
			{
				for (int j = 0; j < MAX_TEAMS; j++)
				{
					if (GET_TEAM((TeamTypes)j).isAlive())
					{
						if (j != getTeam() && j != GET_PLAYER(eOtherPlayer).getTeam())
						{
							setTradeItem(&item, TRADE_EMBARGO, j, &kTransport);
							if (canTradeItem(eOtherPlayer, item))
							{
								ourList.insertAtEnd(item);
								bFoundItemUs = true;
							}
						}
					}
				}
			}
			break;
		}
	}
}

bool CvPlayer::getHeadingTradeString(PlayerTypes eOtherPlayer, TradeableItems eItem, CvWString& szString, CvString& szIcon) const
{
	szIcon.clear();

	switch ( eItem )
	{
	case TRADE_YIELD:
		szString = gDLL->getText("TXT_KEY_TRADE_YIELD_HEADING");
		break;

	case TRADE_CITIES:
		szString = gDLL->getText("TXT_KEY_TRADE_CITIES");
		break;

	case TRADE_PEACE:
		szString = gDLL->getText("TXT_KEY_TRADE_MAKE_PEACE_WITH");
		break;

	case TRADE_WAR:
		szString = gDLL->getText("TXT_KEY_TRADE_DECLARE_WAR_ON");
		break;

	case TRADE_EMBARGO:
		szString = gDLL->getText("TXT_KEY_TRADE_STOP_TRADING_WITH");
		break;

	default:
		szString.clear();
		return false;
		break;
	}

	return true;
}


bool CvPlayer::getItemTradeString(PlayerTypes eOtherPlayer, bool bOffer, bool bShowingCurrent, const TradeData& zTradeData, const IDInfo& kTransport, CvWString& szString, CvString& szIcon) const
{
	szIcon.clear();

	switch (zTradeData.m_eItemType)
	{
	case TRADE_GOLD:
		if (bOffer)
		{
			szString = gDLL->getText("TXT_KEY_TRADE_GOLD_NUM", zTradeData.m_iData1);
		}
		else
		{
			szString = gDLL->getText("TXT_KEY_TRADE_GOLD_NUM", getMaxGoldTrade(eOtherPlayer, kTransport));
		}
		break;
	case TRADE_MAPS:
		szString = gDLL->getText("TXT_KEY_TRADE_WORLD_MAP_STRING");
		break;
	case TRADE_OPEN_BORDERS:
		szString = gDLL->getText("TXT_KEY_TRADE_OPEN_BORDERS_STRING");
		break;
	case TRADE_DEFENSIVE_PACT:
		szString = gDLL->getText("TXT_KEY_TRADE_DEFENSIVE_PACT_STRING");
		break;
	case TRADE_PERMANENT_ALLIANCE:
		szString = gDLL->getText("TXT_KEY_TRADE_PERMANENT_ALLIANCE_STRING");
		break;
	case TRADE_PEACE_TREATY:
		szString = gDLL->getText("TXT_KEY_TRADE_PEACE_TREATY_STRING", GC.getDefineINT("PEACE_TREATY_LENGTH"));
		break;
	case TRADE_YIELD:
		{
			YieldTypes eYield = (YieldTypes) zTradeData.m_iData1;
			CvYieldInfo& kYield = GC.getYieldInfo(eYield);
			CvUnit* pTransport = ::getUnit(kTransport);
			FAssert(pTransport != NULL);
			if (pTransport != NULL)
			{
				int iAmount = bOffer ? GET_PLAYER(eOtherPlayer).getTradeYieldAmount(eYield, pTransport) : getTradeYieldAmount(eYield, pTransport);
				szString.Format( L"%s (%d%c)", kYield.getDescription(), iAmount, kYield.getChar());
			}
		}
		break;

	case TRADE_CITIES:
		{
			CvCity* pCity = bOffer ? GET_PLAYER(eOtherPlayer).getCity(zTradeData.m_iData1) : getCity(zTradeData.m_iData1);
			if (NULL != pCity)
			{
				if (pCity->getLiberationPlayer(false) == eOtherPlayer)
				{
					szString.Format(L"%s (%s)", pCity->getName().GetCString(), gDLL->getText("TXT_KEY_LIBERATE_CITY").GetCString());
				}
				else
				{
					szString = gDLL->getText("TXT_KEY_CITY_OF", pCity->getNameKey());
				}
			}
		}
		break;
	case TRADE_PEACE:
		if (bOffer)
		{
			szString = gDLL->getText("TXT_KEY_TRADE_PEACE_WITH");
			szString += GET_TEAM((TeamTypes)zTradeData.m_iData1).getName();
		}
		else
		{
			szString = GET_TEAM((TeamTypes)zTradeData.m_iData1).getName();
		}
		break;
	case TRADE_WAR:
		if (bOffer)
		{
			szString = gDLL->getText("TXT_KEY_TRADE_WAR_WITH");
			szString += GET_TEAM((TeamTypes)zTradeData.m_iData1).getName();
		}
		else
		{
			szString = GET_TEAM((TeamTypes)zTradeData.m_iData1).getName();
		}
		break;
	case TRADE_EMBARGO:
		if (bOffer)
		{
			szString = gDLL->getText("TXT_KEY_TRADE_STOP_TRADING_WITH");
			szString += L" " + GET_TEAM((TeamTypes)zTradeData.m_iData1).getName();
		}
		else
		{
			szString = GET_TEAM((TeamTypes)zTradeData.m_iData1).getName();
		}
		break;
	default:
		szString.clear();
		return false;
	}

	return true;
}

void CvPlayer::updateTradeList(PlayerTypes eOtherPlayer, CLinkList<TradeData>& ourInventory, const CLinkList<TradeData>& ourOffer, const CLinkList<TradeData>& theirOffer, const IDInfo& kTransport) const
{
	for (CLLNode<TradeData>* pNode = ourInventory.head(); pNode != NULL; pNode = ourInventory.next(pNode))
	{
		pNode->m_data.m_bHidden = false;

		// Don't show peace treaties when not at war
		if (!::atWar(getTeam(), GET_PLAYER(eOtherPlayer).getTeam()))
		{
			if (pNode->m_data.m_eItemType == TRADE_PEACE_TREATY)
			{
				pNode->m_data.m_bHidden = true;
			}
		}

		if (pNode->m_data.m_eItemType == TRADE_YIELD)
		{
			for (CLLNode<TradeData>* pOfferNode = theirOffer.head(); pOfferNode != NULL; pOfferNode = theirOffer.next(pOfferNode))
			{
				if (pOfferNode->m_data.m_eItemType == TRADE_YIELD)
				{
					pNode->m_data.m_bHidden = true;
					break;
				}
			}
		}
	}

	if (!isHuman() || !GET_PLAYER(eOtherPlayer).isHuman())  // everything allowed in human-human trades
	{
		CLLNode<TradeData>* pFirstOffer = ourOffer.head();
		if (pFirstOffer == NULL)
		{
			pFirstOffer = theirOffer.head();
		}

		if (pFirstOffer != NULL)
		{
			if (!CvDeal::isEndWar(pFirstOffer->m_data.m_eItemType) || !::atWar(getTeam(), GET_PLAYER(eOtherPlayer).getTeam()))
			{
				for (CLLNode<TradeData>* pNode = ourInventory.head(); pNode != NULL; pNode = ourInventory.next(pNode))
				{
					if (pFirstOffer->m_data.m_eItemType == TRADE_CITIES || pNode->m_data.m_eItemType == TRADE_CITIES)
					{
						pNode->m_data.m_bHidden = true;
					}
					else if (CvDeal::isAnnual(pFirstOffer->m_data.m_eItemType) != CvDeal::isAnnual(pNode->m_data.m_eItemType))
					{
						pNode->m_data.m_bHidden = true;
					}
				}
			}
		}
	}
}

int CvPlayer::getMaxGoldTrade(PlayerTypes eOtherPlayer, const IDInfo& kTransport) const
{
	CvCity* pTradeCity = NULL;
	CvUnit* pTransport = ::getUnit(kTransport);
	if (pTransport != NULL)
	{
		CvPlot* pPlot = pTransport->plot();
		if (pPlot != NULL)
		{
			pTradeCity = pPlot->getPlotCity();
		}
	}

	if (pTradeCity != NULL && pTradeCity->getOwnerINLINE() == getID())
	{
		return pTradeCity->AI_maxGoldTrade(eOtherPlayer);
	}
	else if (!isNative())
	{
		return AI_maxGoldTrade(eOtherPlayer);
	}

	return 0;
}

void CvPlayer::changeProfessionEurope(int iUnitId, ProfessionTypes eProfession)
{
	FAssert(getParent() != NO_PLAYER);
	CvUnit* pUnit = getEuropeUnitById(iUnitId);
	FAssert(pUnit != NULL);
	if (pUnit == NULL)
	{
		return;
	}

	FAssert(pUnit->canHaveProfession(eProfession, false));

	if (eProfession != pUnit->getProfession())
	{
		CvPlayer& kEurope = GET_PLAYER(getParent());
		int iCost = 0;
		for (int i = 0; i < NUM_YIELD_TYPES; ++i)
		{
			YieldTypes eYieldType = (YieldTypes) i;
			int iMissing = pUnit->getProfessionChangeYieldRequired(eProfession, eYieldType);
			if (iMissing > 0)
			{
				iCost += iMissing * kEurope.getYieldSellPrice(eYieldType);
				changeYieldTradedTotal(eYieldType, iMissing);
				kEurope.changeYieldTradedTotal(eYieldType, iMissing);
				GC.getGameINLINE().changeYieldBoughtTotal(kEurope.getID(), eYieldType, iMissing);
			}
			else if (iMissing < 0)
			{
				int iGold = getSellToEuropeProfit(eYieldType, -iMissing);
				iCost -= iGold;
				changeYieldTradedTotal(eYieldType, -iMissing);
				kEurope.changeYieldTradedTotal(eYieldType, -iMissing);
				GC.getGameINLINE().changeYieldBoughtTotal(kEurope.getID(), eYieldType, iMissing);

				for (int j = 0; j < GC.getNumFatherPointInfos(); ++j)
				{
					FatherPointTypes ePointType = (FatherPointTypes) j;

					changeFatherPoints(ePointType, iGold * GC.getFatherPointInfo(ePointType).getEuropeTradeGoldPointPercent() / 100);
				}

			}
		}
		pUnit->setProfession(eProfession);
		changeGold(-iCost);
		gDLL->getInterfaceIFace()->setDirty(EuropeScreen_DIRTY_BIT, true);
	}
}

bool CvPlayer::checkPopulation() const
{
	int iNumPopulation = 0;
	int iLoop;
	for (CvUnit* pLoopUnit = firstUnit(&iLoop); NULL != pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit->getUnitInfo().isFound())
		{
			++iNumPopulation;
		}
	}
	for (CvCity* pLoopCity = firstCity(&iLoop); NULL != pLoopCity; pLoopCity = nextCity(&iLoop))
	{
		iNumPopulation += pLoopCity->getPopulation();
	}
	for (uint i = 0; i < m_aEuropeUnits.size(); ++i)
	{
		if (m_aEuropeUnits[i]->getUnitInfo().isFound())
		{
			++iNumPopulation;
		}
	}

	/*** TRIANGLETRADE 10/23/08 by DPII ***/
	for (uint i = 0; i < m_aAfricaUnits.size(); ++i)
	{
		if (m_aAfricaUnits[i]->getUnitInfo().isFound())
		{
			++iNumPopulation;
		}
	}
	/**************************************/
	
	// R&R, ray, Port Royal
	for (uint i = 0; i < m_aPortRoyalUnits.size(); ++i)
	{
		if (m_aPortRoyalUnits[i]->getUnitInfo().isFound())
		{
			++iNumPopulation;
		}
	}
	// R&R, ray, Port Royal - END

	return (iNumPopulation == getTotalPopulation());
}

bool CvPlayer::checkPower(bool bReset)
{
	int iPower = 0;
	int iAsset = 0;
	std::map<int, int> mapAreaPower;
	int iLoop;
	for (CvUnit* pUnit = firstUnit(&iLoop); pUnit != NULL; pUnit = nextUnit(&iLoop))
	{
		int iUnitPower = pUnit->getPower();
		iPower += iUnitPower;
		// large rivers power fix - Nightinggale - start
		// use the area the unit is placed on
		// using CvUnit::area() might return the area from the plot next to the unit
		// this happens with land units on large rivers and ships on land (cities)
		//CvArea* pArea = pUnit->area();
		CvPlot* pPlot = pUnit->plot();
		CvArea* pArea = pPlot ? pPlot->area() : NULL;
		// large rivers power fix - Nightinggale - end
		if (pArea != NULL)
		{
			mapAreaPower[pArea->getID()] += iUnitPower;
		}
		iAsset += pUnit->getAsset();
	}
	for (uint i = 0; i < m_aEuropeUnits.size(); ++i)
	{
		iPower += m_aEuropeUnits[i]->getPower();
		iAsset += m_aEuropeUnits[i]->getAsset();
	}
	/*** TRIANGLETRADE 10/23/08 by DPII ***/
	for (uint i = 0; i < m_aAfricaUnits.size(); ++i)
	{
		iPower += m_aAfricaUnits[i]->getPower();
		iAsset += m_aAfricaUnits[i]->getAsset();
	}
	/**************************************/
	
	// R&R, ray, Port Royal
	for (uint i = 0; i < m_aPortRoyalUnits.size(); ++i)
	{
		iPower += m_aPortRoyalUnits[i]->getPower();
		iAsset += m_aPortRoyalUnits[i]->getAsset();
	}
	// R&R, ray, Port Royal

	for (CvCity* pCity = firstCity(&iLoop); pCity != NULL; pCity = nextCity(&iLoop))
	{
		int iCityPower = 0;
		int iCityAsset = 0;
		for (int i = 0; i < pCity->getPopulation(); ++i)
		{
			iCityPower += pCity->getPopulationUnitByIndex(i)->getPower();
			iCityAsset += pCity->getPopulationUnitByIndex(i)->getAsset();
		}

		for (int i = 0; i < GC.getNumBuildingInfos(); ++i)
		{
			BuildingTypes eBuilding = (BuildingTypes) i;
			if (pCity->isHasBuilding(eBuilding))
			{
				iCityPower += GC.getBuildingInfo(eBuilding).getPowerValue();
				iCityAsset += GC.getBuildingInfo(eBuilding).getAssetValue();
			}
		}

		for (int i = 0; i < NUM_YIELD_TYPES; ++i)
		{
			YieldTypes eYield = (YieldTypes) i;
			iCityPower += pCity->getYieldStored(eYield) * GC.getYieldInfo(eYield).getPowerValue();
			iCityAsset += pCity->getYieldStored(eYield) * GC.getYieldInfo(eYield).getAssetValue();
		}

		iPower += iCityPower;
		iAsset += iCityAsset;
		mapAreaPower[pCity->area()->getID()] += iCityPower;
	}

	bool bCheck = true;
	if (iPower != getPower())
	{
		if (bReset)
		{
			changePower(iPower - getPower());
		}
		bCheck = false;
	}

	if (iAsset != getAssets())
	{
		if (bReset)
		{
			changeAssets(iAsset - getAssets());
		}
		bCheck = false;
	}

	for (CvArea* pArea = GC.getMapINLINE().firstArea(&iLoop); pArea != NULL; pArea = GC.getMapINLINE().nextArea(&iLoop))
	{
		if (mapAreaPower[pArea->getID()] != pArea->getPower(getID()))
		{
			if (bReset)
			{
				pArea->changePower(getID(), mapAreaPower[pArea->getID()] - pArea->getPower(getID()));
			}
			bCheck = false;
		}
	}

	return bCheck;
}

void CvPlayer::doREFReduction(int iGold)
{
	setRevolutionEuropeUnitThresholdMultiplier(getRevolutionEuropeUnitThresholdMultiplier() * (100 + iGold * GC.getHandicapInfo(getHandicapType()).getKingGoldThresholdPercent() / 100) / 100);
}

void CvPlayer::interceptEuropeUnits()
{
	if (!canTradeWithEurope())
	{
		std::vector<CvUnit*> apEuropeUnits;
		int iLoop;
		for (CvUnit* pUnit = firstUnit(&iLoop); pUnit != NULL; pUnit = nextUnit(&iLoop))
		{
			// R&R, ray, adjustments for Africa
			//if (pUnit->getUnitTravelState() == UNIT_TRAVEL_STATE_TO_EUROPE || pUnit->getUnitTravelState() == UNIT_TRAVEL_STATE_IN_EUROPE)
			if (pUnit->getUnitTravelState() == UNIT_TRAVEL_STATE_TO_EUROPE || pUnit->getUnitTravelState() == UNIT_TRAVEL_STATE_IN_EUROPE || pUnit->getUnitTravelState() == UNIT_TRAVEL_STATE_TO_AFRICA || pUnit->getUnitTravelState() == UNIT_TRAVEL_STATE_IN_AFRICA)
			{
				if (!pUnit->isCargo())
				{
					apEuropeUnits.push_back(pUnit);
				}
			}
		}

		for (size_t i = 0; i < apEuropeUnits.size(); ++i)
		{
			CvUnit* pUnit = apEuropeUnits[i];
			CvPlot* pPlot = pUnit->plot();
			CvWString szMessage = gDLL->getText("TXT_KEY_YOU_UNITS_EUROPE_INTERCEPTED", pUnit->getNameOrProfessionKey());
			gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szMessage, "AS2D_COMBAT", MESSAGE_TYPE_DISPLAY_ONLY, pUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pPlot->getX_INLINE(), pPlot->getY_INLINE(), true);
			pUnit->kill(false);
		}

		for (uint i = 0; i < m_aEuropeUnits.size(); ++i)
		{
			m_aEuropeUnits[i]->updateOwnerCache(-1);
		}
		freeEuropeUnits();
	}
}

// Dale - AoD: AI Autoplay START
void CvPlayer::setDisableHuman( bool newVal )
{
	m_bDisableHuman = newVal;
}

bool CvPlayer::getDisableHuman( )
{
	return m_bDisableHuman;
}
// Dale - AoD: AI Autoplay END
// PatchMod: Achievements START
void CvPlayer::changeNumCombatsWon(int iChange)
{
	m_iNumCombatsWon += iChange;
}

int CvPlayer::getNumCombatsWon()
{
	return m_iNumCombatsWon;
}

void CvPlayer::changeNumSeaCombatsWon(int iChange) // R&R, ray, Achievments for Sea Combats
{
	m_iNumSeaCombatsWon += iChange;
}

int CvPlayer::getNumSeaCombatsWon() // R&R, ray, Achievments for Sea Combats
{
	return m_iNumSeaCombatsWon;
}

bool CvPlayer::isAchieveGained(AchieveTypes eAchieve) const
{
	return (std::find(m_achievesGained.begin(), m_achievesGained.end(), eAchieve) != m_achievesGained.end());
}

int CvPlayer::getAchieveYear(AchieveTypes eAchieve)
{
	int iIndex = 0;
	std::vector<AchieveTypes>::iterator it;
	for (it = m_achievesGained.begin(); it != m_achievesGained.end(); ++it)
	{
		if (*it == eAchieve)
		{
			return m_achievesTurn[iIndex];
		}
		iIndex++;
	}
	return 0;
}

void CvPlayer::gainAchievement(AchieveTypes eAchieve, bool bAnnounce, CvPlot* pPlot, PlayerTypes iPlayer)
{
	if (!isAchieveGained(eAchieve))
	{
		m_achievesGained.push_back(eAchieve);
		m_achievesTurn.push_back(GC.getGameINLINE().getGameTurn());
		if (bAnnounce)
		{
			if (isHuman())
			{
				CvWString szBuffer = gDLL->getText(GC.getAchieveInfo(eAchieve).getDescription());
				if (pPlot != NULL)
				{
					//Codeaenderung RAY
					//Alter Code
					//gDLL->getInterfaceIFace()->addMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pPlot->getX_INLINE(), pPlot->getY_INLINE(), true, true);
					//Neuer Code
					gDLL->getInterfaceIFace()->addMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), NULL, NULL, false, false);
					//Ende Codeaenderung RAY

				} else {
					//Codeaenderung RAY
					//Alter Code
					//gDLL->getInterfaceIFace()->addMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), NULL, NULL, true, true);
					//Neuer Code
					gDLL->getInterfaceIFace()->addMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), NULL, NULL, false, false);
					//Ende Codeaenderung RAY
				}
				if (!GC.getGameINLINE().isGameMultiPlayer())
				{
					CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_ACHIEVEMENTS);
					pInfo->setData1(eAchieve);
					pInfo->setData2(iPlayer);
					gDLL->getInterfaceIFace()->addPopup(pInfo);
				}
			}
		}
	}
}

bool CvPlayer::canGainAchievement() const
{
	const CvPlayerAI& kPlayer = GET_PLAYER(getID());

	// Only European players may gain achievements
	if (kPlayer.getParent() != NO_PLAYER)
		return true;
	// Natives, animals/pirates, kings and the church may not
	else
		return false;
}

void CvPlayer::doAchievements(bool afterMove)
{
	if (!canGainAchievement())
	{
		return;
	}

	if (GC.getGameINLINE().getGameTurn() < 1 && !afterMove)
	{
		return;
	}
	CvPlot* pPlot = NULL;
	CvCity* pCity = NULL;
	CvUnit* pUnit = NULL;
	bool bGained = false;
	int iI, iJ, iK;
	int count;
//	CvWString szBuffer = gDLL->getText("TXT_KEY_DALETEST", GC.getGameINLINE().getGameTurn());
//	gDLL->getInterfaceIFace()->addMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), NULL, NULL, true, true);
	for (iI = 0; iI < GC.getNumAchieveInfos(); iI++)
	{
		bGained = false;
		if (GC.getAchieveInfo((AchieveTypes)iI).isActive() && !isAchieveGained((AchieveTypes)iI))
		{
			if (!afterMove || (afterMove && GC.getAchieveInfo((AchieveTypes)iI).isAfterMove()))
			{
				if (GC.getAchieveInfo((AchieveTypes)iI).getLowTurnNumber() <= GC.getGameINLINE().getGameTurn() && GC.getAchieveInfo((AchieveTypes)iI).getHighTurnNumber() >= GC.getGameINLINE().getGameTurn())
				{
					if (GC.getAchieveInfo((AchieveTypes)iI).isLandDiscovered())
					{
						for (iJ = 0; iJ < GC.getMapINLINE().numPlotsINLINE(); iJ++)
						{
							pPlot = GC.getMapINLINE().plotByIndexINLINE(iJ);
							if (pPlot->isRevealed(getTeam(), false))
							{
								if (!pPlot->isWater())
								{
									bGained = true;
									iJ = GC.getMapINLINE().numPlotsINLINE();
								}
							}
						}
					}
					if (GC.getAchieveInfo((AchieveTypes)iI).isDiscoverEast())
					{
						for (iJ = 0; iJ < GC.getMapINLINE().numPlotsINLINE(); iJ++)
						{
							pPlot = GC.getMapINLINE().plotByIndexINLINE(iJ);
							if (pPlot->isRevealed(getTeam(), false))
							{
								// WTP, ray, fixing issue for Europe East / West Achievement
								// used 0 because EUROPE_EAST somehow did not work - related to Enum probably
								if (pPlot->isEurope() && pPlot->getEurope() == 0)
								{
									bGained = true;
									iJ = GC.getMapINLINE().numPlotsINLINE();
								}
							}
						}
					}
					if (GC.getAchieveInfo((AchieveTypes)iI).isDiscoverWest())
					{
						for (iJ = 0; iJ < GC.getMapINLINE().numPlotsINLINE(); iJ++)
						{
							pPlot = GC.getMapINLINE().plotByIndexINLINE(iJ);
							if (pPlot->isRevealed(getTeam(), false))
							{
								// WTP, ray, fixing issue for Europe East / West Achievement
								// used 1 because EUROPE_WEST somehow did not work - related to Enum probably
								if (pPlot->isEurope() && pPlot->getEurope() == 1)
								{
									bGained = true;
									iJ = GC.getMapINLINE().numPlotsINLINE();
								}
							}
						}
					}
					if (GC.getAchieveInfo((AchieveTypes)iI).getNumColonies() > 0)
					{
						if (getNumCities() >= GC.getAchieveInfo((AchieveTypes)iI).getNumColonies())
						{
							pPlot = NULL;
							bGained = true;
						}
					}
					if (GC.getAchieveInfo((AchieveTypes)iI).getNumCombatsWonNeeded() > 0)
					{
						if (getNumCombatsWon() >= GC.getAchieveInfo((AchieveTypes)iI).getNumCombatsWonNeeded())
						{
							pPlot = NULL;
							bGained = true;
						}
					}
					if (GC.getAchieveInfo((AchieveTypes)iI).getNumSeaCombatsWonNeeded() > 0) // R&R, ray, Achievments for Sea Combats
					{
						if (getNumSeaCombatsWon() >= GC.getAchieveInfo((AchieveTypes)iI).getNumSeaCombatsWonNeeded())
						{
							pPlot = NULL;
							bGained = true;
						}
					}
					if (GC.getAchieveInfo((AchieveTypes)iI).getNumEuroContacts() > 0)
					{
						count = 0;
						for (iK = 0; iK < MAX_PLAYERS; iK++)
						{
							CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iK);
							if(kPlayer.getCivilizationType() != NO_CIVILIZATION)
							{
								if (!kPlayer.isEurope() && kPlayer.getParent() != NO_PLAYER && kPlayer.getTeam() != getTeam() && GET_TEAM(getTeam()).isHasMet(kPlayer.getTeam()))
								{
                                    count++;
								}
							}
						}
						if (GC.getAchieveInfo((AchieveTypes)iI).getNumEuroContacts() <= count)
						{
							pPlot = NULL;
							bGained = true;
						}
					}
					if (GC.getAchieveInfo((AchieveTypes)iI).getNumNativeContacts() > 0)
					{
						count = 0;
						for (iK = 0; iK < MAX_PLAYERS; iK++)
						{
							CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iK);
							if(kPlayer.getCivilizationType() != NO_CIVILIZATION)
							{
								if (kPlayer.isNative()&& kPlayer.getTeam() != getTeam() && GET_TEAM(getTeam()).isHasMet(kPlayer.getTeam()))
								{
                                    count++;
								}
							}
						}
						if (GC.getAchieveInfo((AchieveTypes)iI).getNumNativeContacts() <= count)
						{
							pPlot = NULL;
							bGained = true;
						}
					}
					if (GC.getAchieveInfo((AchieveTypes)iI).getNumGoodsTraded() > 0)
					{
						count = 0;
						for (int iK = 0; iK < NUM_YIELD_TYPES; iK++)
						{
							count += getYieldTradedTotal((YieldTypes) iK);
						}
						if (count > GC.getAchieveInfo((AchieveTypes)iI).getNumGoodsTraded())
						{
							pPlot = NULL;
							bGained = true;
						}
					}
					for (iK = 0; iK < GC.getNumBuildingInfos(); iK++)
					{
						if (GC.getAchieveInfo((AchieveTypes)iI).getNumBuildings(iK) > 0)
						{
							count = 0;
							for (pCity = firstCity(&iJ); pCity != NULL; pCity = nextCity(&iJ))
							{

								// R&R, ray fixed Achievement System for Buildings - START
								int iSpecialBuildingTypeToCheck = GC.getBuildingInfo((BuildingTypes)iK).getSpecialBuildingType();
								int iMinSpecialBuildingPriorityToCheck = GC.getBuildingInfo((BuildingTypes)iK).getSpecialBuildingPriority();

								bool check = false;
								for (int xx = 0; xx < GC.getNumBuildingInfos(); xx++)
								{
									if (GC.getBuildingInfo((BuildingTypes)xx).getSpecialBuildingType() == iSpecialBuildingTypeToCheck && GC.getBuildingInfo((BuildingTypes)xx).getSpecialBuildingPriority() >= iMinSpecialBuildingPriorityToCheck && pCity->isHasBuilding((BuildingTypes)xx))
									{
										check = true;
									}
								}			

//								if (pCity->isHasSpecialBuilding(GC.getBuildingInfo((BuildingTypes)iK).getSpecialBuildingType()))
//								if (pCity->isHasBuilding((BuildingTypes)iK))
								if(check)
								// R&R, ray fixed Achievement System for Buildings - END
								{
									count++;
									pPlot = pCity->plot();
								}
							}
							if (GC.getAchieveInfo((AchieveTypes)iI).getNumBuildings(iK) <= count)
							{
								bGained = true;
							}
						}
					}
					for (iK = 0; iK < GC.getNumUnitInfos(); iK++)
					{
						if (GC.getAchieveInfo((AchieveTypes)iI).getNumUnits(iK) > 0)
						{
							count = 0;
							for (pUnit = firstUnit(&iJ); pUnit != NULL; pUnit = nextUnit(&iJ))
							{
								if (pUnit->getUnitType() == (UnitTypes)iK)
								{
									count++;
									pPlot = pUnit->plot();
								}
							}

							//koma13
							for (pCity = firstCity(&iJ); pCity != NULL; pCity = nextCity(&iJ))
							{
								for (int i = 0; i < pCity->getPopulation(); ++i)
								{
									CvUnit* pUnit = pCity->getPopulationUnitByIndex(i);
									if (NULL != pUnit)
									{
										if (pUnit->getUnitType() == (UnitTypes)iK)
										{
											count++;
											pPlot = pUnit->plot();
										}
									}
								}
							}

							if (GC.getAchieveInfo((AchieveTypes)iI).getNumUnits(iK) <= count)
							{
								bGained = true;
							}
						}
					}
					for (iK = 0; iK < GC.getNumProfessionInfos(); iK++)
					{
						if (GC.getAchieveInfo((AchieveTypes)iI).getNumProfessions(iK) > 0)
						{
							count = 0;
							for (pUnit = firstUnit(&iJ); pUnit != NULL; pUnit = nextUnit(&iJ))
							{
								if (pUnit->getProfession() == (ProfessionTypes)iK)
								{
									count++;
									pPlot = pUnit->plot();
								}
							}

							//koma13
							for (pCity = firstCity(&iJ); pCity != NULL; pCity = nextCity(&iJ))
							{
								for (int i = 0; i < pCity->getPopulation(); ++i)
								{
									CvUnit* pUnit = pCity->getPopulationUnitByIndex(i);
									if (NULL != pUnit)
									{
										if (pUnit->getProfession() == (ProfessionTypes)iK)
										{
											count++;
											pPlot = pUnit->plot();
										}
									}
								}
							}

							if (GC.getAchieveInfo((AchieveTypes)iI).getNumProfessions(iK) <= count)
							{
								bGained = true;
							}
						}
					}
					for (iK = 0; iK < NUM_YIELD_TYPES; iK++)
					{
						if (GC.getAchieveInfo((AchieveTypes)iI).getYieldProduced(iK) > 0)
						{
							count = getYieldRate((YieldTypes)iK);
							if (GC.getAchieveInfo((AchieveTypes)iI).getYieldProduced(iK) <= count)
							{
								pPlot = NULL;
								bGained = true;
							}
						}
					}
				}
			}
		}
		if (bGained)
		{
			gainAchievement((AchieveTypes)iI, true, pPlot, getID());
		}
	}
}
// PatchMod: Achievements END

// PatchMod: AI immigration boost START
// TAC - AI More Immigrants START
void CvPlayer::doAIImmigrant(int iIndex)
{
	if (getParent() != NO_PLAYER && canTradeWithEurope())
	{
		UnitTypes eBestUnit = getDocksNextUnit(iIndex);
		if (NO_UNIT != eBestUnit)
		{
			m_aDocksNextUnits[iIndex] = pickBestImmigrant();
			CvUnit* pUnit = initEuropeUnit(eBestUnit);
			CvWString szBuffer = gDLL->getText("TXT_KEY_NEW_IMMIGRANT_AVAILABLE", GC.getCivilizationInfo(GET_PLAYER(getParent()).getCivilizationType()).getShortDescriptionKey(),  GC.getUnitInfo(eBestUnit).getTextKeyWide());
			gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNIT_GREATPEOPLE", MESSAGE_TYPE_INFO, GC.getUnitInfo(eBestUnit).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"));
			gDLL->getInterfaceIFace()->setDirty(EuropeScreen_DIRTY_BIT, true);

			FAssert(pUnit != NULL);
			if(pUnit != NULL)
			{
				gDLL->getEventReporterIFace()->emmigrantAtDocks(getID(), pUnit->getID());
			}
		}
	}
}
// TAC - AI More Immigrants END
// PatchMod: AI immigration boost END

// TAC - AI Economy - Ray - START
// R&R, ray improvement redistribution
void CvPlayer::redistributeWood() {

	// do nothing if Player has no cities
	if (getNumCities() <1) {
		return;
	}

	//only do this every 10 rounds
	if (GC.getGame().getGameTurn() % 10 != 0)
	{
		return;
	}

	int citycount = getNumCities();

	//calculate total wood and stone of all cities
	int totalwood = 0;
	int totalstone = 0;
	int iTotalPopulation = 0;

	CvCity* pLoopCity;

	// Loop through cities first time to get all wood
	int iLoop;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		//add wood and stone
		totalwood += pLoopCity->getYieldStored(YIELD_LUMBER);
		totalstone += pLoopCity->getYieldStored(YIELD_STONE);
		//add population
		iTotalPopulation += pLoopCity->getPopulation();
		//reset Lumber and Stone to 0 because already added to storage variables
		pLoopCity->setYieldStored(YIELD_LUMBER, 0);
		pLoopCity->setYieldStored(YIELD_STONE, 0);
	}

	int woodToDisbributePerCitizen = totalwood / iTotalPopulation;
	int stoneToDisbributePerCitizen = totalstone / iTotalPopulation;

	// Loop through cities second time to distribute all wood
	int iLoop2;
	for (pLoopCity = firstCity(&iLoop2); pLoopCity != NULL; pLoopCity = nextCity(&iLoop2))
	{
		// bigger cities get more
		int iPopulationOfThisCity = pLoopCity->getPopulation();
		// R&R, ray, small fix
		int woodtodistribute = woodToDisbributePerCitizen * iPopulationOfThisCity;
		int stonetodistribute = stoneToDisbributePerCitizen * iPopulationOfThisCity;
		pLoopCity->setYieldStored(YIELD_LUMBER, woodtodistribute);
		pLoopCity->setYieldStored(YIELD_STONE, stonetodistribute);
		// substract the wood and stone distributed from the total amounts
		totalwood -= woodtodistribute;
		totalstone -= stonetodistribute;
	}

	// for safety, if there is some wood left, give it to the first city
	if (totalwood > 0)
	{
		int iLoop3;
		for (pLoopCity = firstCity(&iLoop3); pLoopCity != NULL; pLoopCity = nextCity(&iLoop3))
		{
			int woodalreadydistributed = pLoopCity->getYieldStored(YIELD_LUMBER);
			pLoopCity->setYieldStored(YIELD_LUMBER, totalwood + woodalreadydistributed);
			break;
		}
	}
	// for safety, if there is some stone left, give it to the first city
	if (totalstone > 0)
	{
		int iLoop4;
		for (pLoopCity = firstCity(&iLoop4); pLoopCity != NULL; pLoopCity = nextCity(&iLoop4))
		{
			int stonealreadydistributed = pLoopCity->getYieldStored(YIELD_STONE);
			pLoopCity->setYieldStored(YIELD_STONE, totalstone + stonealreadydistributed);
			break;
		}
	}

}
// TAC - AI Economy - Ray - END


// R&R, ray, redistribute cannons and muskets
void CvPlayer::redistributeCannonsAndMuskets()
{
	// do nothing if Player has no cities
	if (getNumCities() <1) {
		return;
	}

	//only do this every 5 rounds
	if (GC.getGame().getGameTurn() % 5 != 0)
	{
		return;
	}

	// Loop through cities
	int iLoop;
	CvCity* pLoopCity;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		int iAvailableMuskets = pLoopCity->getYieldStored(YIELD_CANNONS);
		int iAvailableCannons = pLoopCity->getYieldStored(YIELD_CANNONS);
		int iNeededCannons = pLoopCity->getProductionNeeded(YIELD_CANNONS);

		// R&R, ray, we now also redistribute Blades to Tools - START
		int iAvailableBlades = pLoopCity->getYieldStored(YIELD_BLADES);
		int iAvailableTools = pLoopCity->getYieldStored(YIELD_TOOLS);

		if (iAvailableBlades > 100 && iAvailableTools < 200)
		{
			int iBladesToRedistribute = iAvailableBlades - 100;
			pLoopCity->setYieldStored(YIELD_TOOLS, iAvailableTools + iBladesToRedistribute);
			pLoopCity->setYieldStored(YIELD_BLADES, iAvailableBlades - iBladesToRedistribute);
		}
		// R&R, ray, we now also redistribute Blades to Tools - END

		if (iNeededCannons > iAvailableCannons)
		{
			// change muskets to cannons if enough available but leave at least 100
			if (iAvailableMuskets > 100)
			{
				int iMusketsToPotentialyTransfer = iAvailableMuskets - 100;
				int iMusketsToReallyTransfer = 0;
				if (iMusketsToPotentialyTransfer >= iNeededCannons)
				{
					iMusketsToReallyTransfer = iNeededCannons;
				}
				else
				{
					iMusketsToReallyTransfer = iMusketsToPotentialyTransfer;
				}
				// change Muskets to Cannons
				pLoopCity->setYieldStored(YIELD_CANNONS, iAvailableCannons + iMusketsToReallyTransfer);
				pLoopCity->setYieldStored(YIELD_MUSKETS, iAvailableMuskets - iMusketsToReallyTransfer);
			}
		}
		else if (iAvailableCannons > iNeededCannons)
		{
			int iCannonsToTransfer = iAvailableCannons - iNeededCannons;
			// change Cannons to Muskets
			pLoopCity->setYieldStored(YIELD_CANNONS, iAvailableCannons - iCannonsToTransfer);
			pLoopCity->setYieldStored(YIELD_MUSKETS, iAvailableMuskets + iCannonsToTransfer);
		}

	}
}


// TAC - LbD - Ray - START
bool CvPlayer::LbD_try_become_expert(CvUnit* convUnit, int base, int increase, int pre_rounds, int l_level)
{
	// get data from Unit
	int workedRounds = convUnit->getLbDrounds();
	ProfessionTypes lastProfession = convUnit->getLastLbDProfession();
	ProfessionTypes currentProfession = convUnit->getProfession();

	//getting KI-Modifier
	int ki_modifier = GC.getLBD_KI_MOD_EXPERT();

	if (currentProfession != lastProfession)
	{
		convUnit->setLastLbDProfession(currentProfession);
		if(isHuman())
		{
			//workedRounds = 1;
			convUnit->setLbDrounds(1);
		}
		// here little cheat for AI to cope with feature
		// profession change does not reset worked rounds
		else
		{
			//workedRounds++;
			convUnit->setLbDrounds(workedRounds + 1);
		}
	}

	else
	{
		//workedRounds++;
		convUnit->setLbDrounds(workedRounds + 1);
	}

	if (workedRounds < pre_rounds)
	{
		return false;
	}

	//Schmiddie, added LbD modifier for Sophisticated Trait START
	int calculatedChance = (base + (workedRounds - pre_rounds) * increase / l_level);

	if(!isHuman())
	{
		calculatedChance = calculatedChance * ki_modifier / 100;
	}

	calculatedChance *= CivEffect()->getLearningByDoingModifier() / 100; // CivEffects - Nightinggale

								//Schmiddie, added LbD modifier for Sophisticated Trait ENDE

	//ray Multiplayer Random Fix
	//int randomValue = rand() % 1000 + 1;
	int randomValue = GC.getGameINLINE().getSorenRandNum(1000, "LbD Expert Map");

	// no Success if randomValue larger calculatedChance
	if (randomValue > calculatedChance)
	{
		return false;
	}

	int expert = GC.getProfessionInfo(convUnit->getProfession()).LbD_getExpert();

	UnitTypes expertUnitType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(expert);
	FAssert(expertUnitType != NO_UNIT);
	// R&R, ray, small fix
	//CvUnit* expertUnit = initUnit(expertUnitType, convUnit->getProfession(), convUnit->getX_INLINE(), convUnit->getY_INLINE(), convUnit->AI_getUnitAIType());
	CvUnit* expertUnit = initUnit(expertUnitType, GC.getCivilizationInfo(getCivilizationType()).getDefaultProfession(), convUnit->getX_INLINE(), convUnit->getY_INLINE());
	FAssert(expertUnit != NULL);
	expertUnit->joinGroup(convUnit->getGroup());
	expertUnit->convert(convUnit, true);

	// AddMessage
	CvWString szBuffer = gDLL->getText("TXT_KEY_LBD_EXPERT", expertUnit->getUnitInfo().getDescription());
	gDLL->getInterfaceIFace()->addMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, expertUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), expertUnit->getX(), expertUnit->getY(), true, true);

	return true;
}

bool CvPlayer::LbD_try_get_free(CvUnit* convUnit, int base, int increase, int pre_rounds, int mod_crim, int mod_serv, int l_level)
{
	// get data from Unit
	int workedRounds = convUnit->getLbDrounds();
	ProfessionTypes lastProfession = convUnit->getLastLbDProfession();
	ProfessionTypes currentProfession = convUnit->getProfession();

	if (currentProfession != lastProfession)
	{
		convUnit->setLastLbDProfession(currentProfession);
	}

	//workedRounds++;
	convUnit->setLbDrounds(workedRounds + 1);

	if (workedRounds < pre_rounds)
	{
		return false;
	}

	//cases criminal or servant
	int modcase = convUnit->getUnitInfo().getUnitClassType();

	//default case is servant
	int mod = mod_serv;

	// if criminal
	if (modcase == 2)
	{
		mod = mod_crim;
	}

	int calculatedChance = (base + (workedRounds - pre_rounds) * increase * l_level * mod);

	//ray Multiplayer Random Fix
	//int randomValue = rand() % 1000 + 1;
	int randomValue = GC.getGameINLINE().getSorenRandNum(1000, "LbD Free Map");

	// no Success if randomValue larger calculatedChance
	if (randomValue > calculatedChance)
	{
		return false;
	}

	// convert Unit to Free Settler
	UnitTypes DefaultUnitType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("DEFAULT_POPULATION_UNIT"));
	FAssert(DefaultUnitType != NO_UNIT);
	// R&R, ray, small fix
	//CvUnit* DefaultUnit = initUnit(DefaultUnitType, convUnit->getProfession(), convUnit->getX_INLINE(), convUnit->getY_INLINE(), convUnit->AI_getUnitAIType());
	CvUnit* DefaultUnit = initUnit(DefaultUnitType, GC.getUnitInfo(DefaultUnitType).getDefaultProfession(), convUnit->getX_INLINE(), convUnit->getY_INLINE());
	FAssert(DefaultUnit != NULL);
	DefaultUnit->joinGroup(convUnit->getGroup());
	DefaultUnit->convert(convUnit, true);
	// AddMessage
	CvWString szBuffer = gDLL->getText("TXT_KEY_LBD_FREE");
	gDLL->getInterfaceIFace()->addMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, DefaultUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), DefaultUnit->getX(), DefaultUnit->getY(), true, true);

	return true;
}


// Professions on Map
void CvPlayer::doLbD()
{
	//do nothing for Natives and Kings, Wild Animals or anything other than Colonial European
	//most easily checked with parent
	if (getParent() == NO_PLAYER)
	{
		return;
	}

	//getting global values for formula
	int base_chance_expert = GC.getLBD_BASE_CHANCE_EXPERT();
	int chance_increase_expert = GC.getLBD_CHANCE_INCREASE_EXPERT();
	int pre_rounds_expert = GC.getLBD_PRE_ROUNDS_EXPERT();
	int base_chance_free = GC.getLBD_BASE_CHANCE_FREE();
	int chance_increase_free = GC.getLBD_CHANCE_INCREASE_FREE();
	int pre_rounds_free = GC.getLBD_PRE_ROUNDS_FREE();
	int mod_free_criminal = GC.getLBD_CHANCE_MOD_FREE_CRIMINAL();
	int mod_free_servant = GC.getLBD_CHANCE_MOD_FREE_SERVANT();

	// R&R, ray, getting Veterans for Free through Combat Experience - START
	int iExperienceVeteran = GC.getLBD_EXPERIENCE_VETERAN();
	int iExperienceFree = GC.getLBD_EXPERIENCE_FREE();
	// R&R, ray, getting Veterans for Free through Combat Experience - END

	//getting GameSpeedModifiert in percent
	int train_percent = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();

	//moddifying Expert values with GameSpeed
	chance_increase_expert = chance_increase_expert / train_percent / 100;
	pre_rounds_expert = pre_rounds_expert * train_percent / 100;

	chance_increase_free = chance_increase_free / train_percent / 100;
	pre_rounds_free = pre_rounds_free * train_percent / 100;

	// loop through units
	CvUnit* pLoopUnit;
	int iLoop;
	for (pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		bool lbd_expert_successful = false;
		bool lbd_free_successful = false;

		// only do something for this unit if the profession does use LbD AND is in NO TravelState
		if (pLoopUnit->getProfession()!= NO_PROFESSION && GC.getProfessionInfo(pLoopUnit->getProfession()).LbD_isUsed() && pLoopUnit->getUnitTravelState() == NO_UNIT_TRAVEL_STATE)
		{
			//CvProfessionInfo eProfessionInfo = GC.getProfessionInfo(pLoopUnit->getProfession());

			// R&R, ray, getting Veterans or Free through Combat Experience - START
			// to filter out Pioneer
			if(!GC.getProfessionInfo(pLoopUnit->getProfession()).isUnarmed())
			{
				int iExperience = pLoopUnit->getExperience();

				if(pLoopUnit->getUnitInfo().LbD_canBecomeExpert())
				{
					if(iExperience >= iExperienceVeteran)
					{
						// convert Unit to Veteran Unit of the Profession
						int expert = GC.getProfessionInfo(pLoopUnit->getProfession()).LbD_getExpert();
						UnitTypes expertUnitType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(expert);
						CvUnit* expertUnit = initUnit(expertUnitType, GC.getCivilizationInfo(getCivilizationType()).getDefaultProfession(), pLoopUnit->getX_INLINE(), pLoopUnit->getY_INLINE());
						expertUnit->joinGroup(pLoopUnit->getGroup());
						expertUnit->convert(pLoopUnit, true);
						// AddMessage
						CvWString szBuffer = gDLL->getText("TXT_KEY_LBD_VETERAN_BY_MILITARY_SERVICE", expertUnit->getUnitInfo().getDescription());
						gDLL->getInterfaceIFace()->addMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, expertUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), expertUnit->getX(), expertUnit->getY(), true, true);

						lbd_expert_successful = true;
					}
				}

				if(pLoopUnit->getUnitInfo().LbD_canGetFree() && !lbd_expert_successful)
				{
					if(iExperience >= iExperienceFree)
					{
						// convert Unit to Free Settler
						UnitTypes DefaultUnitType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("DEFAULT_POPULATION_UNIT"));
						CvUnit* DefaultUnit = initUnit(DefaultUnitType, GC.getUnitInfo(DefaultUnitType).getDefaultProfession(), pLoopUnit->getX_INLINE(), pLoopUnit->getY_INLINE());
						DefaultUnit->joinGroup(pLoopUnit->getGroup());
						DefaultUnit->convert(pLoopUnit, true);
						// AddMessage
						CvWString szBuffer = gDLL->getText("TXT_KEY_LBD_FREE_BY_MILITARY_SERVICE");
						gDLL->getInterfaceIFace()->addMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, DefaultUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), DefaultUnit->getX(), DefaultUnit->getY(), true, true);
					}
				}

			}
			// R&R, ray, getting Veterans or Free through Combat Experience - END

			// old Code
			else
			{
				//get LearnLevel of profession
				int learn_level = GC.getProfessionInfo(pLoopUnit->getProfession()).LbD_getLearnLevel();
				// just for safety, catch possible XML mistakes which might break calculation
				if (learn_level == 0)
				{
					learn_level = 1;
				}

				// try to become expert pioneer or golddigger if poosible
				if(pLoopUnit->getUnitInfo().LbD_canBecomeExpert())
				{
					lbd_expert_successful = LbD_try_become_expert(pLoopUnit, base_chance_expert, chance_increase_expert, pre_rounds_expert, learn_level);
				}

				// try to become free if poosible
				if(pLoopUnit->getUnitInfo().LbD_canGetFree() && !lbd_expert_successful)
				{
					lbd_free_successful = LbD_try_get_free(pLoopUnit, base_chance_free, chance_increase_free, pre_rounds_free, mod_free_criminal, mod_free_servant, learn_level);
				}
			}
		}

	}

}
// TAC - LbD - Ray - END

//TAC Native Mercs
void CvPlayer::checkForNativeMercs()
{
	//check if min round for feature activation has been reached
	int gamespeedMod =  GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent(); // WTP, ray, small correction in balancing
	if(GC.getGame().getGameTurn() < GC.getMIN_ROUND_NATIVE_MERCS()* gamespeedMod / 100)
	{
		return;
	}

	if(m_iTimerNativeMerc > 0)
	{
		m_iTimerNativeMerc = (m_iTimerNativeMerc - 1);
		return;
	}

	if (getNumCities() == 0)
	{
		return;
	}

	//ok, now run through players
	for (int iPlayer=0;iPlayer<MAX_PLAYERS;iPlayer++)
	{
		CvPlayer& potentialMercPlayer = GET_PLAYER((PlayerTypes) iPlayer);

		//check if Player is native AND we can contact AND is willing to talk AND minMercAttitude reached AND for Safety checking Peace
		if(potentialMercPlayer.isNative() && canContact((PlayerTypes) iPlayer) && potentialMercPlayer.AI_isWillingToTalk(getID()) && potentialMercPlayer.AI_getAttitude(getID(), false) >= GC.getLeaderHeadInfo(potentialMercPlayer.getPersonalityType()).getMinAttitudeGiveMercs() && !GET_TEAM(getTeam()).isAtWar(potentialMercPlayer.getTeam()))
		{
			//checking random chance for merc
			int randomMercValue = GC.getGameINLINE().getSorenRandNum(1000, "Native Merc");
			int mercChance = GC.getLeaderHeadInfo(potentialMercPlayer.getPersonalityType()).getBaseChanceGiveMercs();

			if (mercChance > randomMercValue)
			{
				//calculating merc prices
				int basemercprice = GC.getDefineINT("BASE_NATIVE_MERC_PRICE");
				int discount = potentialMercPlayer.AI_getAttitude(getID(), false) * 100;
				if (discount > 500)
				{
					discount = 500;
				}
				int totalmercprice = basemercprice - discount;
				if (totalmercprice < basemercprice / 2)
				{
					totalmercprice = basemercprice / 2;
				}

				totalmercprice = totalmercprice * gamespeedMod / 100;
				int cheapmercprice = totalmercprice / 2;

				//simple logik for AI
				if (!isHuman())
				{
					CvPlayerAI& kPlayerAI = GET_PLAYER((PlayerTypes) getID());
					// TAC - AI Military Buildup - koma13
					if (!kPlayerAI.AI_isStrategy(STRATEGY_MILITARY_BUILDUP))
					{
						int iMercRand = GC.getGameINLINE().getSorenRandNum(100, "AI should buy Mercenaries?");
						if (iMercRand < GC.getDefineINT("AI_CHANCE_FOR_BUYING_MERCENARIES"))
						{
							int iMercPriceAI = totalmercprice * GC.getDefineINT("AI_PRICE_PERCENT_FOR_BUYING_MERCENARIES") / 100;
							if (getGold() > iMercPriceAI)
							{
								m_iTimerNativeMerc = GC.getTIMER_NATIVE_MERC() * gamespeedMod / 100; // WTP, ray, small correction in balancing
								buyNativeMercs(potentialMercPlayer.getID(), iMercPriceAI, false);
							}
						}
					}
				}
				//more complicated case for human
				else
				{
					m_iTimerNativeMerc = GC.getTIMER_NATIVE_MERC() * gamespeedMod / 100; // WTP, ray, small correction in balancing
					// handle this by DiploEvent
					CvDiploParameters* pDiplo = new CvDiploParameters(potentialMercPlayer.getID());

					//Player can afford normal price
					if(getGold() >= totalmercprice)
					{
						pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_ACQUIRE_MERC"));
						pDiplo->addDiploCommentVariable(totalmercprice);
					}
					//Player can afford only cheap price
					else if(getGold() >= cheapmercprice)
					{
						pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_ACQUIRE_MERC_ONLY_CHEAP"));
						pDiplo->addDiploCommentVariable(totalmercprice);
					}
					//Player cannot afford
					else
					{
						pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_ACQUIRE_MERC_CANT_AFFORD"));
						pDiplo->addDiploCommentVariable(totalmercprice);
					}
					pDiplo->setData(totalmercprice);
					pDiplo->setAIContact(true);
					gDLL->beginDiplomacy(pDiplo, getID());

					//break necesarry because otherwise negative accounts possible
					break;
				}
			}
		}
	}
	return;
}


// R&R, ray, Native Slave, START
void CvPlayer::checkForNativeSlaves()
{
	//check if min round for feature activation has been reached
	int gamespeedMod =  GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent(); // WTP, ray, small correction in balancing
	if(GC.getGame().getGameTurn() < GC.getMIN_ROUND_NATIVE_SLAVE()* gamespeedMod / 100)
	{
		return;
	}

	if(m_iTimerNativeSlave > 0)
	{
		m_iTimerNativeSlave = (m_iTimerNativeSlave - 1);
		return;
	}

	if (getNumCities() == 0)
	{
		return;
	}

	//ok, now run through players
	for (int iPlayer=0;iPlayer<MAX_PLAYERS;iPlayer++)
	{
		CvPlayer& potentialSlavePlayer = GET_PLAYER((PlayerTypes) iPlayer);

		//check if Player is native AND we can contact AND is willing to talk AND minMercAttitude reached AND for Safety checking Peace
		if(potentialSlavePlayer.isNative() && canContact((PlayerTypes) iPlayer) && potentialSlavePlayer.AI_isWillingToTalk(getID()) && potentialSlavePlayer.AI_getAttitude(getID(), false) >= GC.getLeaderHeadInfo(potentialSlavePlayer.getPersonalityType()).getMinAttitudeGiveNativeSlaves() && !GET_TEAM(getTeam()).isAtWar(potentialSlavePlayer.getTeam()))
		{
			//checking random chance for merc
			int randomSlaveValue = GC.getGameINLINE().getSorenRandNum(1000, "Native Slave");
			int slaveChance = GC.getLeaderHeadInfo(potentialSlavePlayer.getPersonalityType()).getBaseChanceGiveNativeSlaves();

			if (slaveChance > randomSlaveValue)
			{
				//calculating merc prices
				int baseslaveprice = GC.getDefineINT("BASE_NATIVE_SLAVE_PRICE");
				int discount = potentialSlavePlayer.AI_getAttitude(getID(), false) * 10;
				if (discount > 50)
				{
					discount = 50;
				}
				int totalslaveprice = baseslaveprice - discount;
				totalslaveprice = totalslaveprice * gamespeedMod / 100;
				
				//simple logik for AI
				if (!isHuman())
				{			
					CvPlayerAI& kPlayerAI = GET_PLAYER((PlayerTypes) getID());
					// TAC - AI Military Buildup - koma13
					if (!kPlayerAI.AI_isStrategy(STRATEGY_MILITARY_BUILDUP))
					{
						if (getGold() > totalslaveprice * 2)
						{
							//get City
							CvCity* pLoopCity = NULL;
							CvCity* locationToAppear = NULL;
							int iLoop;
							locationToAppear = firstCity(&iLoop);

							//for safety stop if no city
							if (locationToAppear == NULL)
							{
								return;
							}

							//exchanging gold
							potentialSlavePlayer.changeGold(totalslaveprice);
							changeGold(-totalslaveprice);
							m_iTimerNativeSlave = GC.getTIMER_NATIVE_SLAVE() * gamespeedMod / 100; // WTP, ray, small correction in balancing

							//creating unit
							UnitTypes DefaultSlaveUnitType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_NATIVE_SLAVE"));
							FAssert(DefaultSlaveUnitType != NO_UNIT);
							CvUnit* SlaveUnit = initUnit(DefaultSlaveUnitType, GC.getUnitInfo(DefaultSlaveUnitType).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
						}
					}
				}

				//more complicated case for human
				else
				{
					m_iTimerNativeSlave = GC.getTIMER_NATIVE_SLAVE() * gamespeedMod / 100; // WTP, ray, small correction in balancing
					// handle this by DiploEvent
					CvDiploParameters* pDiplo = new CvDiploParameters(potentialSlavePlayer.getID());

					//Player can afford price
					if(getGold() >= totalslaveprice)
					{
						pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_ACQUIRE_NATIVE_SLAVE"));
						pDiplo->addDiploCommentVariable(totalslaveprice);
					}
					//Player cannot afford
					else
					{
						pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_ACQUIRE_NATIVE_SLAVE_CANT_AFFORD"));
						pDiplo->addDiploCommentVariable(totalslaveprice);
					}
					pDiplo->setData(totalslaveprice);
					pDiplo->setAIContact(true);
					gDLL->beginDiplomacy(pDiplo, getID());

					//break necesarry because otherwise negative accounts possible
					break;
				}
			}
		}
	}
	return;
}


// R&R, ray, African Slaves - START
void CvPlayer::checkForAfricanSlaves()
{
	// not in Revolution
	if(isInRevolution())
	{
		return;
	}

	int iminround = GC.getMIN_ROUND_AFRICAN_SLAVES();
	int gamespeedMod = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
	iminround = iminround * gamespeedMod / 100;

	//check if min round for feature activation has been reached
	if(GC.getGame().getGameTurn() < iminround)
	{
		return;
	}

	if(m_iTimerAfricanSlaves > 0)
	{
		m_iTimerAfricanSlaves = (m_iTimerAfricanSlaves - 1);
		return;
	}

	//get City
	CvCity* pLoopCity = NULL;
	CvCity* locationToAppear = NULL;
	int iLoop;
	locationToAppear = firstCity(&iLoop); 

	//for safety
	if (locationToAppear == NULL)
	{
		return;
	}

	int randomSlaveValue = GC.getGameINLINE().getSorenRandNum(1000, "African Slaves");
	int slaveChance = GC.getAFRICAN_SLAVES_CHANCE();

	if (randomSlaveValue > slaveChance)
	{
		return;
	}

	CvPlayer& King = GET_PLAYER(getParent());

	//calculating number of prisoners and their price
	int numSlavesOffered = GC.getGameINLINE().getSorenRandNum(GC.getDefineINT("MAX_AFRICAN_SLAVES_OFFERED"), "AfricanSlavesNumber");
	if (numSlavesOffered == 0)
	{
		numSlavesOffered = 1;
	}
	int baseslaveprice = GC.getDefineINT("BASE_AFRICAN_SLAVES_PRICE");
	int discount = King.AI_getAttitude(getID(), false) * 10;
	int totalslavesprice = baseslaveprice - discount;
	totalslavesprice = numSlavesOffered * totalslavesprice * gamespeedMod / 100;

	//simple logik for AI
	if (!isHuman())
	{
		if (getGold() > totalslavesprice * 2)
		{
			CvPlayerAI& kPlayerAI = GET_PLAYER((PlayerTypes) getID());
			// TAC - AI Military Buildup - koma13
			if (!kPlayerAI.AI_isStrategy(STRATEGY_MILITARY_BUILDUP))
			{
				//create the prisoners
				UnitTypes SlaveType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_AFRICAN_SLAVE"));
				CvUnit* SlaveUnit;
				for (int i=0;i<numSlavesOffered;i++)
				{
					SlaveUnit = initUnit(SlaveType, GC.getUnitInfo(SlaveType).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
				}
				//pay the king
				King.changeGold(totalslavesprice);
				King.AI_changeAttitudeExtra(getID(), 1);
				changeGold(-totalslavesprice);
				m_iTimerAfricanSlaves = GC.getTIMER_AFRICAN_SLAVE()*gamespeedMod / 200; // African Slaves offered more frequently to AI
			}
		}
	}

	// Diplo-Event for Human
	else
	{
		m_iTimerAfricanSlaves = GC.getTIMER_AFRICAN_SLAVE()*gamespeedMod/100;
		// handle this by DiploEvent with own king
		CvDiploParameters* pDiplo = new CvDiploParameters(getParent());

		//Player can afford price
		if(getGold() >= totalslavesprice)
		{
			pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_ACQUIRE_AFRICAN_SLAVES"));
		}
		//Player cannot afford
		else
		{
			pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_ACQUIRE_AFRICAN_SLAVES_CANT_AFFORD"));
		}
		pDiplo->addDiploCommentVariable(totalslavesprice);
		pDiplo->setData(numSlavesOffered);
		pDiplo->setAIContact(true);
		gDLL->beginDiplomacy(pDiplo, getID());

	}
	return;
}
// R&R, ray, African Slaves - END

// R&R, ray, Prisons Crowded - START
void CvPlayer::checkForPrisonsCrowded()
{
	// not in Revolution
	if(isInRevolution())
	{
		return;
	}
    int gamespeedMod = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
	
	//check if min round for feature activation has been reached
	if(GC.getGame().getGameTurn() < GC.getMIN_ROUND_PRISONS_CROWDED()*gamespeedMod/100)
	{
		return;
	}

	if(m_iTimerPrisonsCrowded > 0)
	{
		m_iTimerPrisonsCrowded = (m_iTimerPrisonsCrowded - 1);
		return;
	}

	//get City
	CvCity* pLoopCity = NULL;
	CvCity* locationToAppear = NULL;
	int iLoop;
	locationToAppear = firstCity(&iLoop); 

    //for safety
	if (locationToAppear == NULL)
	{
		return;
	}

	int randomPrisonValue = GC.getGameINLINE().getSorenRandNum(1000, "PrisonsCrowded");
	int prisonChance = GC.getPRISONS_CROWDED_CHANCE();

	if (randomPrisonValue > prisonChance)
	{
		return;
	}

	CvPlayer& King = GET_PLAYER(getParent());

	//calculating number of prisoners and their price
	int numPrisonersOffered = GC.getGameINLINE().getSorenRandNum(GC.getDefineINT("MAX_PRISONERS_OFFERED"), "PrisonsCrowdedNumber");
	if (numPrisonersOffered == 0)
	{
		numPrisonersOffered = 1;
	}
	int baseprisonerprice = GC.getDefineINT("BASE_PRISONER_PRICE");
	int discount = King.AI_getAttitude(getID(), false) * 10;
	int totalprisonersprice = baseprisonerprice - discount;

	totalprisonersprice = numPrisonersOffered * totalprisonersprice * gamespeedMod / 100;

	//simple logik for AI
	if (!isHuman())
	{
		if (getGold() > totalprisonersprice * 2)
		{
			CvPlayerAI& kPlayerAI = GET_PLAYER((PlayerTypes) getID());
			// TAC - AI Military Buildup - koma13
			if (!kPlayerAI.AI_isStrategy(STRATEGY_MILITARY_BUILDUP))
			{
				//create the prisoners
				UnitTypes PrisonerType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_PRISONER"));
				CvUnit* PrisonerUnit;
				for (int i=0;i<numPrisonersOffered;i++)
				{
					PrisonerUnit = initUnit(PrisonerType, GC.getUnitInfo(PrisonerType).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
				}
				//pay the king
				King.changeGold(totalprisonersprice);
				King.AI_changeAttitudeExtra(getID(), 1);
				changeGold(-totalprisonersprice);
				m_iTimerPrisonsCrowded = GC.getTIMER_PRISONS_CROWDED()*gamespeedMod/100;
			}
		}
	}

	// Diplo-Event for Human
	else
	{
		m_iTimerPrisonsCrowded = GC.getTIMER_PRISONS_CROWDED()*gamespeedMod/100;
		// handle this by DiploEvent with own king
		CvDiploParameters* pDiplo = new CvDiploParameters(getParent());

		//Player can afford price
		if(getGold() >= totalprisonersprice)
		{
			pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_PRISONS_CROWDED"));
		}
		//Player cannot afford
		else
		{
			pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_PRISONS_CROWDED_CANT_AFFORD"));
		}
		pDiplo->addDiploCommentVariable(totalprisonersprice);
		pDiplo->setData(numPrisonersOffered);
		pDiplo->setAIContact(true);
		gDLL->beginDiplomacy(pDiplo, getID());

	}
	return;
}
// R&R, ray, Prisons Crowded - END

// R&R, ray, Revolutionary Noble - START
void CvPlayer::checkForRevolutionaryNoble()
{
	// not in Revolution
	if(isInRevolution())
	{
		return;
	}

    int gamespeedMod = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();

	//check if min round for feature activation has been reached
	if(GC.getGame().getGameTurn() <( GC.getMIN_ROUND_REVOLUTIONARY_NOBLE()*gamespeedMod/100))
	{
		return;
	}

	if(m_iTimerRevolutionaryNoble > 0)
	{
		m_iTimerRevolutionaryNoble = (m_iTimerRevolutionaryNoble - 1);
		return;
	}

	//get City
	CvCity* pLoopCity = NULL;
	CvCity* locationToAppear = NULL;
	int iLoop;
	locationToAppear = firstCity(&iLoop); 

    //for safety
	if (locationToAppear == NULL)
	{
		return;
	}

	int randomValue = GC.getGameINLINE().getSorenRandNum(1000, "Revolutionary Noble");
	int nobleChance = GC.getREVOLUTIONARY_NOBLE_CHANCE();

	if (randomValue > nobleChance)
	{
		return;
	}

	CvPlayer& King = GET_PLAYER(getParent());

	int basenobleprice = GC.getDefineINT("BASE_NOBLE_PRICE");
	int discount = King.AI_getAttitude(getID(), false) * 100;
	basenobleprice = basenobleprice - discount;
	basenobleprice = basenobleprice * gamespeedMod / 100;

	int randompart = basenobleprice / 2;
	randompart = GC.getGameINLINE().getSorenRandNum(randompart, "NobleRandomPart");
	int pricetopay = basenobleprice - randompart;

	//simple logik for AI
	if (!isHuman())
	{
		if (getGold() > pricetopay * 2)
		{
			CvPlayerAI& kPlayerAI = GET_PLAYER((PlayerTypes) getID());
			// TAC - AI Military Buildup - koma13
			if (!kPlayerAI.AI_isStrategy(STRATEGY_MILITARY_BUILDUP))
			{
				//create the noble
				UnitTypes NobleType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_NOBLE"));
				CvUnit* NobleUnit;
				NobleUnit = initUnit(NobleType, GC.getUnitInfo(NobleType).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
				//pay the king
				GET_PLAYER(getParent()).changeGold(pricetopay);
				GET_PLAYER(getParent()).AI_changeAttitudeExtra(getID(), 5);
				changeGold(-pricetopay);
				m_iTimerRevolutionaryNoble = GC.getTIMER_REVOLUTIONARY_NOBLE()*gamespeedMod/100;
			}
		}
	}

	// Diplo-Event for Human
	else
	{
		m_iTimerRevolutionaryNoble = GC.getTIMER_REVOLUTIONARY_NOBLE() * gamespeedMod / 100;
		// handle this by DiploEvent with own king
		CvDiploParameters* pDiplo = new CvDiploParameters(getParent());

		//Player can afford price
		if(getGold() >= pricetopay)
		{
			pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_REVOLUTIONARY_NOBLE"));
		}
		//Player cannot afford
		else
		{
			pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_REVOLUTIONARY_NOBLE_CANT_AFFORD"));
		}
		pDiplo->addDiploCommentVariable(pricetopay);
		pDiplo->setData(pricetopay);
		pDiplo->setAIContact(true);
		gDLL->beginDiplomacy(pDiplo, getID());
	}
	return;
}
// R&R, ray, Revolutionary Noble - END

// R&R, ray, BISHOP - START
void CvPlayer::checkForBishop()
{
	// only if church exists
	PlayerTypes eChurchPlayer = GC.getGameINLINE().getChurchPlayer();
	if (eChurchPlayer == NO_PLAYER)
    {
        return;
    }

	// not in Revolution
	if(isInRevolution())
	{
		return;
	}

	int iminround = GC.getMIN_ROUND_BISHOP();
	int gamespeedMod = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
	iminround = iminround * gamespeedMod / 100;


	//check if min round for feature activation has been reached
	if(GC.getGame().getGameTurn() < iminround)
	{
		return;
	}

	if(m_iTimerBishop > 0)
	{
		m_iTimerBishop = (m_iTimerBishop - 1);
		return;
	}

	if (getNumCities() <= getUnitClassCount((UnitClassTypes)GC.getDefineINT("UNITCLASS_BISHOP")))
	{
		return;
	}

	//get City
	CvCity* pLoopCity = NULL;
	CvCity* locationToAppear = NULL;
	int iLoop;
	locationToAppear = firstCity(&iLoop); 

    //for safety
	if (locationToAppear == NULL)
	{
		return;
	}

	int randomValue = GC.getGameINLINE().getSorenRandNum(1000, "Bishop");
	int bishopChance = GC.getBISHOP_CHANCE();

	if (randomValue > bishopChance)
	{
		return;
	}

	CvPlayer& Church = GET_PLAYER(eChurchPlayer);

	int basebishopprice = GC.getDefineINT("BASE_BISHOP_PRICE");
	int discount = Church.AI_getAttitude(getID(), false) * 100;
	basebishopprice = basebishopprice - discount;

	basebishopprice = basebishopprice * gamespeedMod / 100;

	int randompart = basebishopprice / 2;
	randompart = GC.getGameINLINE().getSorenRandNum(randompart, "BishopRandomPart");
	int pricetopay = basebishopprice - randompart;

	//simple logik for AI
	if (!isHuman())
	{
		if (getGold() > pricetopay * 2)
		{
			CvPlayerAI& kPlayerAI = GET_PLAYER((PlayerTypes) getID());
			// TAC - AI Military Buildup - koma13
			if (!kPlayerAI.AI_isStrategy(STRATEGY_MILITARY_BUILDUP))
			{
				//create the bishop
				UnitTypes BishopType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_BISHOP"));
				CvUnit* BishopUnit;
				BishopUnit = initUnit(BishopType, GC.getUnitInfo(BishopType).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
				//pay the king
				Church.changeGold(pricetopay);
				changeGold(-pricetopay);
				m_iTimerBishop = GC.getTIMER_BISHOP()*gamespeedMod/100;
			}
		}
	}

	// Diplo-Event for Human
	else
	{
		m_iTimerBishop = GC.getTIMER_BISHOP()*gamespeedMod/100;
		// handle this by DiploEvent with own king
		CvDiploParameters* pDiplo = new CvDiploParameters(eChurchPlayer);

		//Player can afford price
		if(getGold() >= pricetopay)
		{
			pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_BISHOP"));
		}
		//Player cannot afford
		else
		{
			pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_BISHOP_CANT_AFFORD"));
		}
		pDiplo->addDiploCommentVariable(pricetopay);
		pDiplo->setData(pricetopay);
		pDiplo->setAIContact(true);
		gDLL->beginDiplomacy(pDiplo, getID());
	}
	return;
}
// R&R, ray, BISHOP - END


void CvPlayer::buyNativeMercs(PlayerTypes sellingPlayer, int price, bool mightbeangry)
{
	CvPlayer& Seller = GET_PLAYER(sellingPlayer);

	//get City
	CvCity* pLoopCity = NULL;
	CvCity* locationToAppear = NULL;
	int iLoop;
	locationToAppear = firstCity(&iLoop); 

	//for safety stop if no city
	// R&R, ray safe mechanism to ensure that player gold never is negative
	if (locationToAppear == NULL || getGold() < price)
	{
		return;
	}

	//exchanging gold
	Seller.changeGold(price);
	changeGold(-price);

	//creating unit
	//UnitTypes DefaultMercUnitType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("DEFAULT_MERC_UNIT_CLASS_ID"));
	UnitTypes DefaultMercUnitType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_NATIVE_MERC"));
	//UnitTypes DefaultMercUnitType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("DEFAULT_POPULATION_UNIT"));
	FAssert(DefaultMercUnitType != NO_UNIT);
	CvUnit* MercUnit = initUnit(DefaultMercUnitType, GC.getUnitInfo(DefaultMercUnitType).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);

	//now lets see if this player is angry because of low price
	if(mightbeangry)
	{
		int randomAngryBecauseLowPrice = GC.getGameINLINE().getSorenRandNum(10, "Angry Low Native Merc Price");
		if (randomAngryBecauseLowPrice > 5)
		{
			Seller.AI_changeAttitudeExtra(getID(), -1);
		}
	}

	CvWString szBuffer = gDLL->getText("TXT_KEY_BOUGHT_NATIVE_MERC");
	gDLL->getInterfaceIFace()->addMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, MercUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), MercUnit->getX(), MercUnit->getY(), true, true);

}
//End TAC Native Mercs

//TAC Revolution Support
//function is called by doRevolution()
void CvPlayer::checkForRevolutionSupport()
{
	//check if player has started Revolution (just double checking)
	if(!isInRevolution())
	{
		return;
	}

	//get City
	CvCity* pLoopCity = NULL;
	CvCity* locationToAppear = NULL;
	int iLoop;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
		{
			locationToAppear = pLoopCity;
			break;
		}			
	}

	//for safety stop if no city
	if (locationToAppear == NULL)
	{
		return;
	}

	//ok, now run through players
	for (int iPlayer=0;iPlayer<MAX_PLAYERS;iPlayer++)
	{
		CvPlayer& potentialRevSupporter = GET_PLAYER((PlayerTypes) iPlayer);

		//check if Player is Colonist AND not self AND Player not in revolution himself AND we can contact AND is willing to talk AND we are not at war AND minSupport Attitude reached
		if(!potentialRevSupporter.isNative() && !potentialRevSupporter.isEurope() && potentialRevSupporter.getID() != getID() && !potentialRevSupporter.isInRevolution() && canContact((PlayerTypes) iPlayer) && potentialRevSupporter.AI_isWillingToTalk(getID()) && potentialRevSupporter.AI_getAttitude(getID(), false) >= GC.getDefineINT("MIN_ATTITUDE_FOR_SUPPORT") && !GET_TEAM(getTeam()).isAtWar(potentialRevSupporter.getTeam()))
		{
			// Erik: We also require that the player is on good terms with the potential supporter's king
			if (potentialRevSupporter.getParent() != NO_PLAYER && GET_PLAYER(potentialRevSupporter.getParent()).AI_getAttitude(getID(), false) >= GC.getDefineINT("MIN_ATTITUDE_FOR_SUPPORT"))
			{ 
				//checking random chance for merc
				int randomSupportValue = GC.getGameINLINE().getSorenRandNum(1000, "Rev Support");
				int supportChance = GC.getDefineINT("BASE_CHANCE_FOR_SUPPORT");

				if (supportChance > randomSupportValue)
				{
					//simple logik for AI
					if (!isHuman())
					{
						//randomize which support AI chooses
						int randomSupportChoice = GC.getGameINLINE().getSorenRandNum(2, "Rev Support Choice");
						int supportAmount;
						UnitTypes DefaultSupportType;

						if (randomSupportChoice > 1)
						{
							supportAmount = GC.getDefineINT("REV_SUPPORT_LAND");
							DefaultSupportType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_REV_SUPPORT_LAND"));
						}
						else
						{
							supportAmount = GC.getDefineINT("REV_SUPPORT_SEA");
							DefaultSupportType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_REV_SUPPORT_SEA"));
						}

						FAssert(DefaultSupportType != NO_UNIT);

						//creating the units
						for (int i=0;i<supportAmount;i++)
						{
							CvUnit* SupportUnit = initUnit(DefaultSupportType, GC.getUnitInfo(DefaultSupportType).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
						}
					}

					//more complicated case for human
					else
					{
						// handle this by DiploEvent
						CvDiploParameters* pDiplo = new CvDiploParameters(potentialRevSupporter.getID());
						pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_GET_REV_SUPPORT"));

						//getting Parent for Text
						pDiplo->addDiploCommentVariable(GC.getLeaderHeadInfo(GET_PLAYER(potentialRevSupporter.getParent()).getLeaderType()).getDescription());

						pDiplo->setAIContact(true);
						gDLL->beginDiplomacy(pDiplo, getID());
					}
				}
			}
		}
	}

	return;

}
//End TAC Revolution Support

//TAC European Wars
void CvPlayer::checkForEuropeanWars()
{
	// only do sth for Colonist-Players if not in revolution
	if(isInRevolution())
	{
		return;
	}

    int gamespeedMod = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
	int warTurn =  GC.getMIN_ROUND_EUROPE_WARS();
	warTurn = warTurn * gamespeedMod/100;

	//check if minimum round for european wars
	if(GC.getGame().getGameTurn() < warTurn)
	{
		return;
	}

	if(m_iTimerEuropeanWars > 0)
	{
		m_iTimerEuropeanWars = (m_iTimerEuropeanWars - 1);
		return;
	}

	int randomEuropeWarValue = GC.getGameINLINE().getSorenRandNum(1000, "European War");
	int warChance = GC.getBASE_CHANCE_EUROPE_WARS();

	// check if european war should occur
	if (warChance < randomEuropeWarValue)
	{
		return;
	}

	//get City
	CvCity* pLoopCity = NULL;
	CvCity* locationToAppear = NULL;
	int iLoop;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
		{
			locationToAppear = pLoopCity;
			break;
		}			
	}

	//for safety stop if no city
	if (locationToAppear == NULL)
	{
		return;
	}

	//ok, now get random player
	int randomEnemyID = GC.getGameINLINE().getSorenRandNum(MAX_PLAYERS - 1, "Europe War Enemy");
	CvPlayer& potentialEnemy = GET_PLAYER((PlayerTypes) randomEnemyID);

	// if potentialEnemy fulfills one of theses conditions do nothing
	if(getTeam() == potentialEnemy.getTeam() || potentialEnemy.isNative() || potentialEnemy.isEurope() || potentialEnemy.getID() == getID() || potentialEnemy.isInRevolution() || !canContact((PlayerTypes) randomEnemyID) || potentialEnemy.getNumCities() < 1 || potentialEnemy.getParent() == getParent())
	{
		return;
	}

	//for AI
	//giving free Unit if a war with that player
	if(!isHuman() && GET_TEAM(getTeam()).isAtWar(potentialEnemy.getTeam()))
	{
		//Randomize which unit AI gets
		int randomSupportChoice = GC.getGameINLINE().getSorenRandNum(3, "European Wars AI");
		UnitTypes KingReinforcementType;
		if (randomSupportChoice > 2)
		{
			KingReinforcementType = (UnitTypes)GC.getCivilizationInfo(GET_PLAYER(getParent()).getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_KING_REINFORCEMENT_SEA"));
		}
		else if (randomSupportChoice > 1)
		{
			KingReinforcementType = (UnitTypes)GC.getCivilizationInfo(GET_PLAYER(getParent()).getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_KING_REINFORCEMENT_ARTIL"));
		}
		else
		{
			KingReinforcementType = (UnitTypes)GC.getCivilizationInfo(GET_PLAYER(getParent()).getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_KING_REINFORCEMENT_LAND"));
		}
		FAssert(KingReinforcementType != NO_UNIT);
		CvUnit* ReinforcementUnit = initUnit(KingReinforcementType, GC.getUnitInfo(KingReinforcementType).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
		m_iTimerEuropeanWars = GC.getTIMER_EUROPEAN_WARS()*gamespeedMod/100;
	}

	//for Human
	//start logic, if not already at war
	else if(isHuman() && !GET_TEAM(getTeam()).isAtWar(potentialEnemy.getTeam()))
	{
		m_iTimerEuropeanWars = GC.getTIMER_EUROPEAN_WARS()*gamespeedMod/100;
		// handle this by DiploEvent with own king
		CvDiploParameters* pDiplo = new CvDiploParameters(getParent());
		pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_EUROPE_WAR"));

		//getting Parent of Enemy for Text
		pDiplo->addDiploCommentVariable(GC.getLeaderHeadInfo(GET_PLAYER(potentialEnemy.getParent()).getLeaderType()).getDescription());
		//setting ID of Enemy as Data
		pDiplo->setData(potentialEnemy.getID());
		pDiplo->setAIContact(true);
		gDLL->beginDiplomacy(pDiplo, getID());
	}
	return;
}
//End TAC European Wars

// R&R, Stealing Immigrant - START
void CvPlayer::checkForStealingImmigrant()
{
	// only do sth for Colonist-Players if not in revolution
	if(isInRevolution())
	{
		return;
	}

	int steal = GC.getMIN_ROUND_STEALING_IMMIGRANT() ;
	int gamespeedMod = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
	steal = steal * gamespeedMod /100;
	//check if minimum round for european wars
	if(GC.getGame().getGameTurn() <steal)
	{
		return;
	}

	if(m_iTimerStealingImmigrant > 0)
	{
		m_iTimerStealingImmigrant = (m_iTimerStealingImmigrant - 1);
		return;
	}

	int randomStealingImmigrantValue = GC.getGameINLINE().getSorenRandNum(1000, "StealingImmigrant");
	int stealingImmigrantChance = GC.getBASE_CHANCE_STEALING_IMMIGRANT();

	// check if dll diplo event should occur
	if (stealingImmigrantChance < randomStealingImmigrantValue)
	{
		return;
	}

	// get price
	int priceStealingImmigrant = GC.getDefineINT("BASE_STEALING_IMMIGRANT_PRICE");
	
	priceStealingImmigrant = priceStealingImmigrant * gamespeedMod / 100;

	// simple case for AI
	if(!isHuman())
	{
		// restart timer
		m_iTimerStealingImmigrant = GC.getTIMER_STEALING_IMMIGRANT() * gamespeedMod/100;

		// simply buy immigrant for cheaper price if AI has enough gold
		if (getGold() > priceStealingImmigrant * 3)
		{
			int randomUnitSelectOnDock = GC.getGameINLINE().getSorenRandNum(CivEffect()->getNumUnitsOnDock(), "pick immigrant");
			UnitTypes eBestUnit = getDocksNextUnit(randomUnitSelectOnDock);
			if (NO_UNIT != eBestUnit)
			{
				m_aDocksNextUnits[randomUnitSelectOnDock] = pickBestImmigrant();
				CvUnit* pUnit = initEuropeUnit(eBestUnit);

				//CvWString szBuffer;
				//szBuffer = gDLL->getText("TXT_KEY_NEW_IMMIGRANT_AVAILABLE_SHORT", GC.getCivilizationInfo(GET_PLAYER(getParent()).getCivilizationType()).getShortDescriptionKey(),  GC.getUnitInfo(eBestUnit).getTextKeyWide());
				//gDLL->getInterfaceIFace()->addMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNIT_GREATPEOPLE", MESSAGE_TYPE_INFO, GC.getUnitInfo(eBestUnit).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"));

				gDLL->getInterfaceIFace()->setDirty(EuropeScreen_DIRTY_BIT, true);

				FAssert(pUnit != NULL);
				if(pUnit != NULL)
				{
					gDLL->getEventReporterIFace()->emmigrantAtDocks(getID(), pUnit->getID());
				}

				changeGold(-priceStealingImmigrant);
			}
		}
	}

	// case for Human
	else
	{
		//ok, now get random player
		int randomVictimID = GC.getGameINLINE().getSorenRandNum(MAX_PLAYERS - 1, "Stealing Immigrant Victimm");
		CvPlayer& potentialVictim = GET_PLAYER((PlayerTypes) randomVictimID);

		// if potentialEnemy fulfills one of theses conditions do nothing
		if(getTeam() == potentialVictim.getTeam() || potentialVictim.isNative() || potentialVictim.isEurope() || potentialVictim.getID() == getID() || potentialVictim.isInRevolution() || !canContact((PlayerTypes) randomVictimID) || potentialVictim.getNumCities() < 1 || potentialVictim.getParent() == getParent() || GET_TEAM(getTeam()).isAtWar(potentialVictim.getTeam()))
		{
			return;
		}

		// restart timer
		m_iTimerStealingImmigrant = GC.getTIMER_STEALING_IMMIGRANT()*gamespeedMod/100;

		// DLL Diplo Event
		CvDiploParameters* pDiplo = new CvDiploParameters(getParent());

		// enough gold
		if(getGold() >= priceStealingImmigrant)
		{
			pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_STEALING_IMMIGRANT"));
		}

		// not enough gold
		else
		{
			pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_STEALING_IMMIGRANT_CANT_AFFORD"));
		}
	
		pDiplo->addDiploCommentVariable(GC.getCivilizationInfo(GET_PLAYER(potentialVictim.getParent()).getCivilizationType()).getShortDescriptionKey()); //getting Parent of Enemy for text
		pDiplo->addDiploCommentVariable(priceStealingImmigrant); // price for text

		//setting ID of Enemy as Data
		// price in event needs to be calculated as above
		pDiplo->setData(potentialVictim.getID());

		pDiplo->setAIContact(true);
		gDLL->beginDiplomacy(pDiplo, getID());		
	}

	return;
}
// R&R, Stealing Immigrant - END


// R&R, ray, Smuggling - START
void CvPlayer::checkForSmugglers()
{
	// Erik: AI cannot use smuggling ships / Port Royal
	if (!isHuman())
		return;

	// not in Revolution
	if(isInRevolution())
	{
		return;
	}
	int gamespeedMod = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
	int smugglers = GC.getMIN_ROUND_SMUGGLERS_AVAILABLE();
	smugglers = smugglers * gamespeedMod /100;
	if(GC.getGame().getGameTurn() < smugglers )
	{
		return;
	}

	if(m_iTimerSmugglingShip > 0)
	{
		m_iTimerSmugglingShip = (m_iTimerSmugglingShip - 1);
		return;
	}

	if(GC.getDefineINT("SMUGGLING_BRIBE_RATE") * 2 > getTaxRate())
	{
		return;
	}

	int pricetopay = GC.getDefineINT("PRICE_SMUGGLERS") *gamespeedMod / 100;
	if (getGold() < pricetopay)
	{
		return;
	}

	int randomSmugglersAvailableValue = GC.getGameINLINE().getSorenRandNum(1000, "Smugglers Available");
	int smugglersChance = GC.getBASE_CHANCE_SMUGGLERS_AVAILABLE();

	if (smugglersChance < randomSmugglersAvailableValue)
	{
		return;
	}

	if (countNumCoastalCities() <= getUnitClassCount((UnitClassTypes)GC.getDefineINT("UNITCLASS_SMUGGLING_SHIP")))
	{
		return;
	}

	//get City
	CvCity* pLoopCity = NULL;
	CvCity* locationToAppear = NULL;
	int iLoop;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
		{
			locationToAppear = pLoopCity;
			break;
		}			
	}

	if (locationToAppear == NULL)
	{
		return;
	}

	// simple logic for AI
	if (!isHuman())
	{
		CvPlayerAI& kPlayerAI = GET_PLAYER((PlayerTypes) getID());
		// TAC - AI Military Buildup - koma13
		if (!kPlayerAI.AI_isStrategy(STRATEGY_MILITARY_BUILDUP))
		{
			//create the smuggling ship
			UnitTypes SmugglingShipType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_SMUGGLING_SHIP"));
			CvUnit* SmugglingShipUnit;
			SmugglingShipUnit = initUnit(SmugglingShipType, GC.getUnitInfo(SmugglingShipType).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
			//pay
			changeGold(-pricetopay);
			m_iTimerSmugglingShip = GC.getTIMER_SMUGGLING_SHIP()*gamespeedMod/100;
		}
	}

	// DLL-Diplo-Event for Human
	else
	{
		m_iTimerSmugglingShip = GC.getTIMER_SMUGGLING_SHIP() * gamespeedMod/100;
		CvDiploParameters* pDiplo = new CvDiploParameters(getParent());
		pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_SMUGGLERS"));
		pDiplo->addDiploCommentVariable(pricetopay);
		pDiplo->setData(pricetopay);
		pDiplo->setAIContact(true);
		gDLL->beginDiplomacy(pDiplo, getID());
	}
	return;
}
// R&R, ray, Smuggling - END


// R&R, ray, Rangers - START
void CvPlayer::checkForRangers()
{	
	// not in Revolution
	if(isInRevolution())
	{
		return;
	}

	if(m_iTimerRanger > 0)
	{
		m_iTimerRanger = (m_iTimerRanger - 1);
		return;
	}

	int randomRangersAvailableValue = GC.getGameINLINE().getSorenRandNum(1000, "Rangers Available");
	int rangersChance = GC.getBASE_CHANCE_RANGERS_AVAILABLE();

	if (rangersChance < randomRangersAvailableValue)
	{
		return;
	}

	if ((getNumCities() - countNumCoastalCities()) <= getUnitClassCount((UnitClassTypes)GC.getDefineINT("UNITCLASS_RANGER")))
	{
		return;
	}

	//get City
	CvCity* pLoopCity = NULL;
	CvCity* locationToAppear = NULL;
	int iLoop;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (!pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
		{
			locationToAppear = pLoopCity;
			break;
		}
	}

	if (locationToAppear == NULL)
	{
		return;
	}

	int gamespeedMod = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
    
	int pricetopay = GC.getDefineINT("PRICE_RANGERS") * gamespeedMod / 100;
	
	// simple logic for AI
	if (!isHuman())
	{
		CvPlayerAI& kPlayerAI = GET_PLAYER((PlayerTypes) getID());
		// TAC - AI Military Buildup - koma13
		if (getGold() > pricetopay)
		{
			if (!kPlayerAI.AI_isStrategy(STRATEGY_MILITARY_BUILDUP))
			{
				m_iTimerRanger = GC.getTIMER_RANGER() * gamespeedMod/100;
				//create the ranger
				UnitTypes RangerType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_RANGER"));
				CvUnit* RangerUnit;
				RangerUnit = initUnit(RangerType, GC.getUnitInfo(RangerType).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
				//pay
				changeGold(-pricetopay);
			}
		}
	}

	// DLL-Diplo-Event for Human
	else
	{
		m_iTimerRanger = GC.getTIMER_RANGER() * gamespeedMod/100;
		CvDiploParameters* pDiplo = new CvDiploParameters(getParent());
		if (getGold() >= pricetopay)
		{
			pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_RANGERS"));
		}
		else
		{
			pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_RANGERS_CANT_AFFORD"));

		}
		pDiplo->addDiploCommentVariable(pricetopay);
		pDiplo->setData(pricetopay);
		pDiplo->setAIContact(true);
		gDLL->beginDiplomacy(pDiplo, getID());
	}
	return;
}
// R&R, ray, Rangers - END


// R&R, ray, Conquistadors - START
void CvPlayer::checkForConquistadors()
{	
	// not in Revolution
	if(isInRevolution())
	{
		return;
	}

	if(m_iTimerConquistador > 0)
	{
		m_iTimerConquistador = (m_iTimerConquistador - 1);
		return;
	}

	// check if at war with Natives
	bool hasWarWithNatives = false;
	TeamTypes ownTeamID = getTeam();
	CvTeam& ownTeam = GET_TEAM(ownTeamID);

	for (int iJ = 0; iJ < MAX_PLAYERS; iJ++)
	{
		CvPlayer& potentialNativeEnemy = GET_PLAYER((PlayerTypes)iJ);
		if (ownTeam.isAtWar(potentialNativeEnemy.getTeam()) && potentialNativeEnemy.isNative())
		{
			hasWarWithNatives = true;
			break;
		}	
	}

	if (!hasWarWithNatives)
	{
		return;
	}

	int randomConquistadorsAvailableValue = GC.getGameINLINE().getSorenRandNum(1000, "Conquistadors Available");
	int conquistadorsChance = GC.getBASE_CHANCE_CONQUISTADORS_AVAILABLE();

	if (conquistadorsChance < randomConquistadorsAvailableValue)
	{
		return;
	}


	//get City
	CvCity* pLoopCity = NULL;
	CvCity* locationToAppear = NULL;
	int iLoop;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
		{
			locationToAppear = pLoopCity;
			break;
		}
	}

	if (locationToAppear == NULL)
	{
		return;
	}

    int gamespeedMod = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();

	int pricetopay = GC.getDefineINT("PRICE_CONQUISTADORS") * gamespeedMod/ 100;

	// simple logic for AI
	if (!isHuman())
	{
		CvPlayerAI& kPlayerAI = GET_PLAYER((PlayerTypes) getID());
		// TAC - AI Military Buildup - koma13
		if (!kPlayerAI.AI_isStrategy(STRATEGY_MILITARY_BUILDUP))
		{	
			int iMercRand = GC.getGameINLINE().getSorenRandNum(100, "AI should buy Mercenaries?");
			if (iMercRand < GC.getDefineINT("AI_CHANCE_FOR_BUYING_MERCENARIES"))
			{
				int iMercPriceAI = pricetopay * GC.getDefineINT("AI_PRICE_PERCENT_FOR_BUYING_MERCENARIES") / 100;
				if (getGold() > iMercPriceAI)
				{								
					m_iTimerConquistador = GC.getTIMER_CONQUISTADOR() * gamespeedMod /100 ;
					//create the conquistador
					UnitTypes ConquistadorType;
					int conquistUnitRand = GC.getGameINLINE().getSorenRandNum(3, "Conquistadors Available");	
					if (conquistUnitRand == 1)
					{
						ConquistadorType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_MOUNTED_CONQUISTADOR"));
					}
					else
					{
						ConquistadorType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_CONQUISTADOR"));
					}
					CvUnit* ConquistadorUnit;
					ConquistadorUnit = initUnit(ConquistadorType, GC.getUnitInfo(ConquistadorType).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
					//pay
					changeGold(-iMercPriceAI);
				}
			}
		}
	}

	// DLL-Diplo-Event for Human
	else
	{
		m_iTimerConquistador = GC.getTIMER_CONQUISTADOR() * gamespeedMod/100;
		CvDiploParameters* pDiplo = new CvDiploParameters(getParent());
		if (getGold() >= pricetopay)
		{
			pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_CONQUISTADORS"));
		}
		else
		{
			pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_CONQUISTADORS_CANT_AFFORD"));
		}
		pDiplo->addDiploCommentVariable(pricetopay);
		pDiplo->setData(pricetopay);
		pDiplo->setAIContact(true);
		gDLL->beginDiplomacy(pDiplo, getID());
	}
	return;
}
// R&R, ray, Conquistadors - END

// R&R, ray, Pirates - START
void CvPlayer::checkForPirates()
{
	if (GC.getGameINLINE().getBarbarianPlayer() == NO_PLAYER)
    {
        return;
    }

	// not in Revolution
	if(isInRevolution())
	{
		return;
	}

	 int gamespeedMod = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();

	// R&R, ray, first appearance of Pirates in relation to Gamespeed
	int minround = GC.getMIN_ROUND_PIRATES() * gamespeedMod / 100;
	if(GC.getGame().getGameTurn() < minround)
	{
		return;
	}

	if(m_iTimerPirates > 0)
	{
		m_iTimerPirates = (m_iTimerPirates - 1);
		return;
	}

	int randomPiratesAvailableValue = GC.getGameINLINE().getSorenRandNum(1000, "Pirates Spawning");
	int piratesChance = GC.getBASE_CHANCE_PIRATES();

	if (piratesChance < randomPiratesAvailableValue)
	{
		return;
	}

	//get City
	CvCity* pLoopCity = NULL;
	CvCity* locationToAppear = NULL;
	int iLoop;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()) && pLoopCity->plot()->hasAnyOtherWaterPlotsThanJustLargeRivers())
		{
			locationToAppear = pLoopCity;
			break;
		}			
	}

	if (locationToAppear == NULL)
	{
		return;
	}

	int pricetopay = GC.getDefineINT("PRICE_PIRATES") * gamespeedMod / 100;
	if (getGold() < pricetopay)
	{
		if (!isHuman())
		{
			int randomAIPiratesSpawn = GC.getGameINLINE().getSorenRandNum(4, "Less Pirates for AI");
			if (randomAIPiratesSpawn <= 2)
			{
				createEnemyPirates();
			}
		}
		else
		{
			createEnemyPirates();
		}
		m_iTimerPirates = GC.getTIMER_PIRATES() * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent() / 100;
		return;
	}

	if (countNumCoastalCities() <= getUnitClassCount((UnitClassTypes)GC.getDefineINT("UNITCLASS_PIRATE_FRIGATE")))
	{
		return;
	}

	// simple logic for AI
	if (!isHuman())
	{
		CvPlayerAI& kPlayerAI = GET_PLAYER((PlayerTypes) getID());
		// TAC - AI Military Buildup - koma13
		if (!kPlayerAI.AI_isStrategy(STRATEGY_MILITARY_BUILDUP))
		{
			//create the pirate ship
			UnitTypes PirateShipType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_PIRATE_FRIGATE"));
			CvUnit* PirateShipUnit;
			PirateShipUnit = initUnit(PirateShipType, GC.getUnitInfo(PirateShipType).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
			//pay
			changeGold(-pricetopay);
			m_iTimerPirates = GC.getTIMER_PIRATES() * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent() / 100;
		}
	}

	// DLL-Diplo-Event for Human
	else
	{
		m_iTimerPirates = GC.getTIMER_PIRATES() * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent() / 100;
		CvDiploParameters* pDiplo = new CvDiploParameters(getParent());
		pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_PIRATES"));
		pDiplo->addDiploCommentVariable(pricetopay);
		pDiplo->setData(pricetopay);
		pDiplo->setAIContact(true);
		gDLL->beginDiplomacy(pDiplo, getID());
	}
	return;
}
// R&R, ray, Pirates - END

// R&R, ray, European Peace -START
void CvPlayer::checkForEuropeanPeace()
{
	// only do this for human Colonist-Players if not in revolution and if AW is not enabled
	if(isInRevolution() || !isHuman() || GC.getGameINLINE().isOption(GAMEOPTION_ALWAYS_WAR))
	{
		return;
	}

	if(m_iTimerEuropeanPeace > 0)
	{
		m_iTimerEuropeanPeace = (m_iTimerEuropeanPeace - 1);
		return;
	}

	int gamespeedMod = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
    
	int randomEuropePeaceValue = GC.getGameINLINE().getSorenRandNum(1000, "European Peace");
	int peaceChance = GC.getBASE_CHANCE_EUROPE_PEACE();

	// check if european peace should occur
	if (peaceChance < randomEuropePeaceValue)
	{
		return;
	}

	//get City
	CvCity* pLoopCity = NULL;
	CvCity* locationToAppear = NULL;
	int iLoop;
	locationToAppear = firstCity(&iLoop); 

    //for safety
	if (locationToAppear == NULL)
	{
		return;
	}

	//ok, now get random player
	int randomPeaceID = GC.getGameINLINE().getSorenRandNum(MAX_PLAYERS - 1, "Europe War Enemy To MAKE PEACE");
	CvPlayer& potentialEnemyToMakePeace = GET_PLAYER((PlayerTypes) randomPeaceID);
	TeamTypes enemyTeam = potentialEnemyToMakePeace.getTeam();
	// if potentialEnemy fulfills one of theses conditions do nothing
	// Most Important atWar and isWillingtoTalk

	if(!potentialEnemyToMakePeace.isAlive() || !GET_TEAM(getTeam()).isAtWar(enemyTeam) || !potentialEnemyToMakePeace.AI_isWillingToTalk(getID()) || getTeam() == potentialEnemyToMakePeace.getTeam() || potentialEnemyToMakePeace.isNative() || potentialEnemyToMakePeace.isEurope() || potentialEnemyToMakePeace.getID() == getID() || potentialEnemyToMakePeace.isInRevolution())
	{
		return;
	}

	//ok conditions checked now make diplo
	else
	{
		m_iTimerEuropeanPeace = GC.getTIMER_EUROPEAN_PEACE() * gamespeedMod /100;
		// handle this by DiploEvent with own king
		CvDiploParameters* pDiplo = new CvDiploParameters(getParent());
		pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_EUROPE_PEACE"));

		//getting Parent of Enemy for Text
		pDiplo->addDiploCommentVariable(GC.getLeaderHeadInfo(GET_PLAYER(potentialEnemyToMakePeace.getParent()).getLeaderType()).getDescription());
		//setting ID of Enemy as Data
		pDiplo->setData(potentialEnemyToMakePeace.getID());
		pDiplo->setAIContact(true);
		gDLL->beginDiplomacy(pDiplo, getID());
	}
	return;
}
// R&R, ray, European Peace - END

// R&R, ray, Bargaining - Start
bool CvPlayer::tryGetNewBargainPriceSell()
{
	PlayerTypes bargainPartner = (PlayerTypes) gDLL->getDiplomacyPlayer();
	int chanceToFail = GC.getDefineINT("BASE_CHANCE_BARGAIN_SELL_FAIL");
	int attitudeChanceImprovement = GC.getDefineINT("CHANCE_IMPROVEMENT_ATTITUDE");
	int timeNativesAngryMax = GC.getDefineINT("ROUNDS_NO_TALK_BECAUSE_BAD_BARGAINING");

	int attitudeLevel = GET_PLAYER(bargainPartner).AI_getAttitude(getID(), false);

	int randomBase = 1000 + attitudeChanceImprovement * attitudeLevel;

	int gamespeedMod = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
	/// random network fix - start - Nightinggale
	//int randomValue = GC.getGameINLINE().getSorenRandNum(randomBase, "Bargaining Sell");
	int randomValue = std::rand() % randomBase;
	/// random network fix - end - Nightinggale

	//case bargaining fails
	if (randomValue < chanceToFail)
	{
		int randomAngry = GC.getGameINLINE().getSorenRandNum(timeNativesAngryMax, "Natives Angry Buy");
		GET_PLAYER(bargainPartner).setTimeNoTrade(randomAngry*gamespeedMod/100);
		GET_PLAYER(bargainPartner).AI_changeAttitudeExtra(getID(), -1);
		GET_PLAYER(bargainPartner).setWillingToBargain(false);
		return false;
	}

	//case bargaining successfull
	else
	{
		GET_PLAYER(bargainPartner).setWillingToBargain(true);
		return true;
	}

}

bool CvPlayer::tryGetNewBargainPriceBuy()
{
	PlayerTypes bargainPartner = (PlayerTypes) gDLL->getDiplomacyPlayer();
	int chanceToFail = GC.getDefineINT("BASE_CHANCE_BARGAIN_BUY_FAIL");
	int attitudeChanceImprovement = GC.getDefineINT("CHANCE_IMPROVEMENT_ATTITUDE");
	int timeNativesAngryMax = GC.getDefineINT("ROUNDS_NO_TALK_BECAUSE_BAD_BARGAINING");

	int attitudeLevel = GET_PLAYER(bargainPartner).AI_getAttitude(getID(), false);
	int gamespeedMod = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
	int randomBase = 1000 + attitudeChanceImprovement * attitudeLevel;
	
	/// random network fix - start - Nightinggale
	//int randomValue = GC.getGameINLINE().getSorenRandNum(randomBase, "Bargaining Buy");
	int randomValue = std::rand() % randomBase;
	/// random network fix - end - Nightinggale

	//case bargaining fails
	if (randomValue < chanceToFail)
	{
		int randomAngry = GC.getGameINLINE().getSorenRandNum(timeNativesAngryMax*gamespeedMod/100, "Natives Angry Buy");
		GET_PLAYER(bargainPartner).setTimeNoTrade(randomAngry);
		GET_PLAYER(bargainPartner).AI_changeAttitudeExtra(getID(), -1);
		GET_PLAYER(bargainPartner).setWillingToBargain(false);
		return false;
	}

	//case bargaining successfull
	else
	{
		GET_PLAYER(bargainPartner).setWillingToBargain(true);
		return true;
	}

}
// R&R, ray, Bargaining - End

// TAC - TAC Interface - koma13 - START
void CvPlayer::toggleMultiRowPlotList()
{
	if (isHuman())
	{
		gDLL->getInterfaceIFace()->toggleMultiRowPlotList();
	}
}
// TAC - TAC Interface - koma13 - END

// R&R, Robert Surcouf, No More Variables Hidden game option START
int CvPlayer::AI_getAttitudeValue(PlayerTypes ePlayer)
{
	return GET_PLAYER(ePlayer).AI_getAttitudeVal(getID());
}
// R&R, Robert Surcouf, No More Variables Hidden game option END

// R&R, ray, Pirates - START
void CvPlayer::createEnemyPirates()
{
	CvPlayer& barbarianPlayer = GET_PLAYER(GC.getGameINLINE().getBarbarianPlayer());

	// we never want to have too many of these
	if (barbarianPlayer.getUnitClassCount((UnitClassTypes)GC.getDefineINT("UNITCLASS_PIRATE_FRIGATE")) > 6)
	{
		return;
	}

	//get City
	CvCity* pLoopCity = NULL;
	CvCity* cityToAttack = NULL;
	int iLoop;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
		{
			cityToAttack = pLoopCity;
			break;
		}
	}

	if (cityToAttack == NULL)
	{
		return;
	}

	// find plot
	CvPlot* pBestPlot = NULL;
	int iBestValue = MAX_INT;

	for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

		if (pLoopPlot->getTeam() == NO_TEAM && pLoopPlot->isWater() && pLoopPlot->getTerrainType() != TERRAIN_LARGE_RIVERS && pLoopPlot->getTerrainType() != TERRAIN_LAKE && pLoopPlot->area()->hasEurope() && pLoopPlot->getNumUnits() == 0)
		{
			int iValue = (plotDistance(cityToAttack->getX_INLINE(), cityToAttack->getY_INLINE(), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE()) * 2);
			if (pLoopPlot->area() != cityToAttack->area())
			{
				iValue *= 3;
			}	
			if (iValue < iBestValue)
			{
				iBestValue = iValue;
				pBestPlot = pLoopPlot;
			}
		}
	}

	// init Pirate Ship
	if (pBestPlot != NULL)
	{
		UnitTypes PirateShipType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_PIRATE_FRIGATE"));
		CvUnit* PirateShipUnit;
		PirateShipUnit = barbarianPlayer.initUnit(PirateShipType, GC.getUnitInfo(PirateShipType).getDefaultProfession(), pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), NO_UNITAI);
		PirateShipUnit->setBarbarian(true);
	}
}
// R&R, ray, Pirates - END

// R&R, ray, Continental Guard - START
void CvPlayer::checkForContinentalGuard()
{
	int gamespeedMod = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
	if(m_iTimerContinentalGuard > 0)
	{
		m_iTimerContinentalGuard = (m_iTimerContinentalGuard - 1);
		return;
	}

	int pricetopay = GC.getDefineINT("PRICE_CONTINENTAL_GUARD") * gamespeedMod / 100;
	if (getGold() < pricetopay)
	{
		return;
	}

	int randomSupportValue = GC.getGameINLINE().getSorenRandNum(1000, "Continental Guard");
	int supportChance = GC.getBASE_CHANCE_CONTINENTAL_GUARD();

	if (supportChance < randomSupportValue)
	{
		return;
	}

	// check if enough Palaces
	int countPalaces = getBuildingClassCount((BuildingClassTypes)GC.getDefineINT("BUILDINGCLASS_PALACE"));
	if (countPalaces <= getUnitClassCount((UnitClassTypes)GC.getDefineINT("UNITCLASS_CONTINENTAL_GUARD")))
	{
		return;
	}

	//get City
	CvCity* pLoopCity = NULL;
	CvCity* locationToAppear = NULL;
	int iLoop;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
		{
			locationToAppear = pLoopCity;
			break;
		}
	}

	//for safety stop if no city
	if (locationToAppear == NULL)
	{
		return;
	}

	PlayerTypes continentalGuardDonator = NO_PLAYER;
	int bestvalue = 0;

	//ok, now run through players and try to select one randomly
	for (int iPlayer=0;iPlayer<MAX_PLAYERS;iPlayer++)
	{
		CvPlayer& potentialRevSupporter = GET_PLAYER((PlayerTypes) iPlayer);
		//check if Player is Colonist AND not self AND Player not in revolution himself AND we can contact AND is willing to talk AND we are not at war AND minSupport Attitude reached
		if(!potentialRevSupporter.isNative() && !potentialRevSupporter.isEurope() && potentialRevSupporter.getID() != getID() && !potentialRevSupporter.isInRevolution() && canContact((PlayerTypes) iPlayer) && potentialRevSupporter.AI_isWillingToTalk(getID()) && potentialRevSupporter.AI_getAttitude(getID(), false) >= GC.getDefineINT("MIN_ATTITUDE_FOR_SUPPORT") && !GET_TEAM(getTeam()).isAtWar(potentialRevSupporter.getTeam()))
		{
			int randnum = GC.getGameINLINE().getSorenRandNum(100, "Pick Continental Guard Donator");
			if (randnum > bestvalue)
			{
				continentalGuardDonator = potentialRevSupporter.getParent();
				//continentalGuardDonator = (PlayerTypes) iPlayer;
				bestvalue = randnum;
			}
			
		}
	}

	if (continentalGuardDonator == NO_PLAYER)
	{
		return;
	}

	//simple logik for AI
	if (!isHuman())
	{
		CvPlayerAI& kPlayerAI = GET_PLAYER((PlayerTypes) getID());
		if (!kPlayerAI.AI_isStrategy(STRATEGY_MILITARY_BUILDUP))
		{
			//create the Continental Guard
			UnitTypes ContinentalGuardType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_CONTINENTAL_GUARD"));
			CvUnit* ContinentalGuardUnit;
			ContinentalGuardUnit = initUnit(ContinentalGuardType, GC.getUnitInfo(ContinentalGuardType).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
			//pay
			changeGold(-pricetopay);
			m_iTimerContinentalGuard = GC.getTIMER_CONTINENTAL_GUARD()*gamespeedMod/100;
		}
	}

	//more complicated case for human
	else
	{
		// handle this by DiploEvent
		CvDiploParameters* pDiplo = new CvDiploParameters(continentalGuardDonator);
		pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_DONATE_CONTINENTAL_GUARD"));
		pDiplo->addDiploCommentVariable(pricetopay);
		pDiplo->setData(pricetopay);
		pDiplo->setAIContact(true);
		gDLL->beginDiplomacy(pDiplo, getID());
		m_iTimerContinentalGuard = GC.getTIMER_CONTINENTAL_GUARD()*gamespeedMod/100;
	}

	return;
}
// R&R, ray, Continental Guard - END

// R&R, ray, Mortar - START
void CvPlayer::checkForMortar()
{
	if(m_iTimerMortar > 0)
	{
		m_iTimerMortar = (m_iTimerMortar - 1);
		return;
	}
	int gamespeedMod = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
	int pricetopay = GC.getDefineINT("PRICE_MORTAR") * gamespeedMod/ 100;
	if (getGold() < pricetopay)
	{
		return;
	}

	int randomSupportValue = GC.getGameINLINE().getSorenRandNum(1000, "Mortar Rand");
	int supportChance = GC.getBASE_CHANCE_MORTAR();

	if (supportChance < randomSupportValue)
	{
		return;
	}

	//get City
	CvCity* pLoopCity = NULL;
	CvCity* locationToAppear = NULL;
	int iLoop;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
		{
			locationToAppear = pLoopCity;
			break;
		}
	}

	//for safety stop if no city
	if (locationToAppear == NULL)
	{
		return;
	}

	// check if enough Citadels or Bastions exist
	int countPrereqBuildings = 0;
	int iLoopPre;
	CvCity* pLoopCityPre = NULL;
	for (pLoopCityPre = firstCity(&iLoopPre); pLoopCityPre != NULL; pLoopCityPre = nextCity(&iLoopPre))
	{
		if (pLoopCityPre->getBuildingDefense() > 120) // only possible if Citadel or Bastion in City
		{
			countPrereqBuildings = countPrereqBuildings +1;
		}
	}

	if (countPrereqBuildings <= getUnitClassCount((UnitClassTypes)GC.getDefineINT("UNITCLASS_MORTAR")))
	{
		return;
	}

	PlayerTypes mortarDonator = NO_PLAYER;
	int bestvalue = 0;

	//ok, now run through players and try to select one randomly
	for (int iPlayer=0;iPlayer<MAX_PLAYERS;iPlayer++)
	{
		CvPlayer& potentialRevSupporter = GET_PLAYER((PlayerTypes) iPlayer);
		//check if Player is Colonist AND not self AND Player not in revolution himself AND we can contact AND is willing to talk AND we are not at war AND minSupport Attitude reached
		if(!potentialRevSupporter.isNative() && !potentialRevSupporter.isEurope() && potentialRevSupporter.getID() != getID() && !potentialRevSupporter.isInRevolution() && canContact((PlayerTypes) iPlayer) && potentialRevSupporter.AI_isWillingToTalk(getID()) && potentialRevSupporter.AI_getAttitude(getID(), false) >= GC.getDefineINT("MIN_ATTITUDE_FOR_SUPPORT") && !GET_TEAM(getTeam()).isAtWar(potentialRevSupporter.getTeam()))
		{
			int randnum = GC.getGameINLINE().getSorenRandNum(100, "Pick Mortar Donator");
			if (randnum > bestvalue)
			{
				mortarDonator = potentialRevSupporter.getParent();
				bestvalue = randnum;
			}
			
		}
	}

	if (mortarDonator == NO_PLAYER)
	{
		return;
	}

	//simple logik for AI
	if (!isHuman())
	{
		CvPlayerAI& kPlayerAI = GET_PLAYER((PlayerTypes) getID());
		if (!kPlayerAI.AI_isStrategy(STRATEGY_MILITARY_BUILDUP))
		{
			//create the Mortar
			UnitTypes MortarType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_MORTAR"));
			CvUnit* MortarUnit;
			MortarUnit = initUnit(MortarType, GC.getUnitInfo(MortarType).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);
			//pay
			changeGold(-pricetopay);
			m_iTimerMortar = GC.getTIMER_MORTAR()*gamespeedMod/100;
		}
	}

	//more complicated case for human
	else
	{
		// handle this by DiploEvent
		CvDiploParameters* pDiplo = new CvDiploParameters(mortarDonator);
		pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_DONATE_MORTAR"));
		pDiplo->addDiploCommentVariable(pricetopay);
		pDiplo->setData(pricetopay);
		pDiplo->setAIContact(true);
		gDLL->beginDiplomacy(pDiplo, getID());
		m_iTimerMortar = GC.getTIMER_MORTAR()*gamespeedMod/100;
	}

	return;
}
// R&R, ray, Mortar - END

// R&R, ray, Militia or Unrest - START
void CvPlayer::checkForMilitiaOrUnrest()
{
	int gamespeedMod = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
	int pricetopay = GC.getPRICE_MILITIA() * gamespeedMod / 100;
	int minCitySize = GC.getMIN_CITY_SIZE_MILITIA_OR_UNREST();
	int chanceForMilitia = GC.getCHANCE_MILITIA();
	int chanceForUnrest = GC.getCHANCE_UNREST();
	UnitTypes MilitiaType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_MILITIA"));

	int iLoop;
	CvCity* pLoopCity = NULL;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->getPopulation() >= minCitySize && !pLoopCity->isDisorder()) // check if city big enough and not in disorder
		{
			if(pLoopCity->plot()->getNumDefenders(getID()) == 0) // is this city undefended
			{
				int foodQty = gamespeedMod;

				if(isHuman()) // case for Human
				{
					int randomMilitiaValue = GC.getGameINLINE().getSorenRandNum(1000, "Militia Rand");
					int randomUnrestValue = GC.getGameINLINE().getSorenRandNum(1000, "Unrest Rand");
					
					if (chanceForMilitia > randomMilitiaValue && getGold() >= pricetopay && pLoopCity->getYieldStored(YIELD_FOOD) > foodQty)
					{
						//create the Militia
						CvUnit* MilitiaUnit = initUnit(MilitiaType, GC.getUnitInfo(MilitiaType).getDefaultProfession(), pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE(), NO_UNITAI);
						//pay
						changeGold(-pricetopay);
						pLoopCity->changeYieldStored(YIELD_FOOD, -foodQty);
						// add message
						CvWString szBuffer = gDLL->getText("TXT_KEY_CITY_LEVIED_MILITIA_FOR_PROTECTION", pLoopCity->getNameKey());
						gDLL->getInterfaceIFace()->addMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, MilitiaUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), MilitiaUnit->getX(), MilitiaUnit->getY(), true, true);
					}
	
					else if(chanceForUnrest > randomUnrestValue)
					{
						int unrestTime = (pLoopCity->getPopulation() / 10);
						if (unrestTime < 2) 
						{
							unrestTime = 2;
						}
						pLoopCity->setOccupationTimer(unrestTime);
						// add message
						CvWString szBuffer = gDLL->getText("TXT_KEY_CITY_UNREST_BECAUSE_MISSING_PROTECTION", pLoopCity->getNameKey());
						gDLL->getInterfaceIFace()->addMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CITYCAPTURED", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE(), true, true);
					}
				}

				else // simple case for AI
				{
					pricetopay = pricetopay /2;
					if (getGold() > pricetopay && pLoopCity->getYieldStored(YIELD_FOOD) > foodQty)
					{
						//create the Militia
						CvUnit* MilitiaUnit = initUnit(MilitiaType, GC.getUnitInfo(MilitiaType).getDefaultProfession(), pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE(), NO_UNITAI);
						//pay
						changeGold(-pricetopay);
						pLoopCity->changeYieldStored(YIELD_FOOD, -foodQty);
					}
				}
			}
		}
	}
}
// R&R, ray, Militia or Unrest - END


// R&R, ray, the Church - START
void CvPlayer::checkForChurchContact()
{
	// only if church exists
	PlayerTypes eChurchPlayer = GC.getGameINLINE().getChurchPlayer();
	if (eChurchPlayer == NO_PLAYER)
    {
        return;
    }

	// only if not in revolution
	if(isInRevolution())
	{
		return;
	}

	int iminround = GC.getMIN_ROUND_CHURCH_CONTACT();
	int gamespeedMod = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
	iminround = iminround * gamespeedMod / 100;

	//check if min round for feature activation has been reached
	if(GC.getGame().getGameTurn() < iminround)
	{
		return;
	}

	if(m_iTimerChurchDemand > 0)
	{
		m_iTimerChurchDemand = (m_iTimerChurchDemand - 1);
		return;
	}

	//get City
	CvCity* pLoopCity = NULL;
	CvCity* locationToAppear = NULL;
	int iLoop;
	locationToAppear = firstCity(&iLoop); 

    //for safety
	if (locationToAppear == NULL)
	{
		return;
	}

	int randomValue = GC.getGameINLINE().getSorenRandNum(1000, "Church Contact");
	int churchContactChance = GC.getCHURCH_CONTACT_CHANCE();

	if (randomValue > churchContactChance)
	{
		return;
	}

	CvPlayer& ChurchPlayer = GET_PLAYER(eChurchPlayer);

	int goldDemandBase = GC.getDefineINT("BASE_CHURCH_DEMAND");
	int discount = ChurchPlayer.AI_getAttitude(getID(), false) * 10;
	goldDemandBase = (goldDemandBase - discount);

	// half of it is random, but random should not be too low
	int pricetopay = goldDemandBase/2 + GC.getGameINLINE().getSorenRandNum(goldDemandBase/2, "Church Demand");
	// modify by Gamespeed
	//int gamespeedMod = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
	pricetopay = pricetopay * gamespeedMod / 100;

	//simple logik for AI
	if (!isHuman())
	{
		if (getGold() > pricetopay * 2)
		{
			CvPlayerAI& kPlayerAI = GET_PLAYER((PlayerTypes) getID());
			// TAC - AI Military Buildup - koma13
			if (!kPlayerAI.AI_isStrategy(STRATEGY_MILITARY_BUILDUP))
			{
				// reward is to lower immigration threshold multiplier
				setImmigrationThresholdMultiplier(((getImmigrationThresholdMultiplier() * 90) / 100));

				// pay
				ChurchPlayer.changeGold(pricetopay);
				changeGold(-pricetopay);

				// reset timer
				m_iTimerChurchDemand = GC.getTIMER_CHURCH_DEMAND() * gamespeedMod /100;
			}
		}
	}

	// Diplo-Event for Human
	else
	{
		// only if it can be afforded
		if(getGold() >= pricetopay)
		{
			// reset timer
			m_iTimerChurchDemand = GC.getTIMER_CHURCH_DEMAND() * gamespeedMod/100;

			// handle this by DiploEvent with Church Player
			CvDiploParameters* pDiplo = new CvDiploParameters(eChurchPlayer);
			pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_CHURCH_DEMAND"));
			pDiplo->addDiploCommentVariable(pricetopay);
			pDiplo->setData(pricetopay);
			pDiplo->setAIContact(true);
			gDLL->beginDiplomacy(pDiplo, getID());
		}
	}
	return;
}
// R&R, ray, the Church - END

// R&R, ray, Church Favours - START
int CvPlayer::getNumChurchFavoursReceived()
{
	return m_iChurchFavoursReceived;
}

void CvPlayer::increaseNumChurchFavoursReceived()
{
	m_iChurchFavoursReceived = m_iChurchFavoursReceived + 1;
}

int CvPlayer::getChurchFavourPrice()
{
	int iChurchFavourPrice = GC.getDefineINT("BASE_CHURCH_FAVOUR_PRICE") + getNumChurchFavoursReceived() * GC.getDefineINT("CHURCH_FAVOUR_PRICE_INCREASE");
	int gamespeedMod = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
	iChurchFavourPrice  = iChurchFavourPrice  * gamespeedMod / 100;
	return iChurchFavourPrice;
}
// R&R, ray, Church Favours - END

// R&R, ray, Church War - START
void CvPlayer::checkForChurchWar()
{
	// only if church exists
	PlayerTypes eChurchPlayer = GC.getGameINLINE().getChurchPlayer();
	if (eChurchPlayer == NO_PLAYER)
    {
        return;
    }

	// only if not in revolution
	if(isInRevolution())
	{
		return;
	}

	//check if min round for feature activation has been reached
	int iminround = GC.getMIN_ROUND_CHURCH_WAR();

	// modification for GameSpeed
	int gamespeedMod = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
	iminround = iminround * gamespeedMod / 100;

	if(GC.getGame().getGameTurn() < iminround)
	{
		return;
	}

	if(m_iTimerChurchWar > 0)
	{
		m_iTimerChurchWar = (m_iTimerChurchWar - 1);
		return;
	}

	//get City
	CvCity* pLoopCity = NULL;
	CvCity* locationToAppear = NULL;
	int iLoop;
	locationToAppear = firstCity(&iLoop); 

    //for safety
	if (locationToAppear == NULL)
	{
		return;
	}
	

	// check randoms
	int randomValue = GC.getGameINLINE().getSorenRandNum(1000, "Church WAR");
	int churchWarChance = GC.getBASE_CHANCE_CHURCH_WAR();

	if (randomValue > churchWarChance)
	{
		return;
	}

	//ok, now get random player
	int randomEnemyID = GC.getGameINLINE().getSorenRandNum(MAX_PLAYERS - 1, "Church War Enemy");
	CvPlayer& potentialNativeEnemy = GET_PLAYER((PlayerTypes) randomEnemyID);

	// if potentialEnemy fulfills one of theses conditions do nothing
	if(!potentialNativeEnemy.isAlive() || !potentialNativeEnemy.isNative() || getTeam() == potentialNativeEnemy.getTeam() || !canContact((PlayerTypes) randomEnemyID))
	{
		return;
	}

	// now check the if that Native might have missions, becaus if it does, it is no vaild target
	CvCity* pNativeLoopCity;
	int iNativeLoop;

	for (pNativeLoopCity = potentialNativeEnemy.firstCity(&iNativeLoop); pNativeLoopCity != NULL; pNativeLoopCity = potentialNativeEnemy.nextCity(&iNativeLoop))
	{
		if (pNativeLoopCity->getMissionaryPlayer() != NO_PLAYER)
		{
			return;
		}
	}

	// simple logik for AI
	// we give the AI a free unit, if it is at war with that Native
	if(!isHuman() && GET_TEAM(getTeam()).isAtWar(potentialNativeEnemy.getTeam()))
	{
		//Randomize which unit AI gets
		int randomSupportChoice = GC.getGameINLINE().getSorenRandNum(3, "Church Wars AI");
		UnitTypes ChurchReinforcementType;
		if (randomSupportChoice > 1)
		{
			ChurchReinforcementType = (UnitTypes)GC.getCivilizationInfo(GET_PLAYER(getParent()).getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_KING_REINFORCEMENT_ARTIL"));
		}
		else
		{
			ChurchReinforcementType = (UnitTypes)GC.getCivilizationInfo(GET_PLAYER(getParent()).getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_MOUNTED_CONQUISTADOR"));
		}
		//FAssert(ChurchReinforcementType != NO_UNIT);
		CvUnit* ReinforcementUnit = initUnit(ChurchReinforcementType, GC.getUnitInfo(ChurchReinforcementType).getDefaultProfession(), locationToAppear->getX_INLINE(), locationToAppear->getY_INLINE(), NO_UNITAI);

		// reset timer
		m_iTimerChurchWar = GC.getTIMER_CHURCH_WAR()*gamespeedMod/100;
	}

	// diplo dialogue with Church for Human
	else if(isHuman() && !GET_TEAM(getTeam()).isAtWar(potentialNativeEnemy.getTeam()))
	{
		// reset timer
		m_iTimerChurchWar = GC.getTIMER_CHURCH_WAR()*gamespeedMod/100;

		CvDiploParameters* pDiplo = new CvDiploParameters(eChurchPlayer);
		pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_CHURCH_WAR"));

		//getting Parent of Enemy for Text
		pDiplo->addDiploCommentVariable(potentialNativeEnemy.getCivilizationDescription());
		//setting ID of Enemy as Data
		pDiplo->setData(potentialNativeEnemy.getID());
		pDiplo->setAIContact(true);
		gDLL->beginDiplomacy(pDiplo, getID());
	}

	return;
}
// R&R, ray, Church War - END

/*** TRIANGLETRADE 10/15/08 by DPII ***/
bool CvPlayer::canTradeWithAfrica() const
{
	//return true;
	return canTradeWithEurope(); // Africa Colony is also under the rule of your king
}

int CvPlayer::getSellToAfricaProfit(YieldTypes eYield, int iAmount) const
{
	if (getParent() == NO_PLAYER)
	{
		return 0;
	}

	if (!isYieldAfricaTradable(eYield))
	{
		return 0;
	}

	CvPlayer& kPlayerEurope = GET_PLAYER(getParent());

	int iPrice = iAmount * kPlayerEurope.getYieldAfricaBuyPrice(eYield);
	iPrice -= (iPrice * getTaxRate()) / 100;

	return iPrice;
}

bool CvPlayer::isYieldAfricaTradable(YieldTypes eYield) const
{
	FAssert(eYield >= 0 && eYield < NUM_YIELD_TYPES);

	if (getParent() == NO_PLAYER) // Africa is also under control of your King
	{
		return false;
	}

	if (!GC.getYieldInfo(eYield).isCargo())
	{
		return false;
	}

	if (!canTradeWithAfrica())
	{
		return false;
	}

	if (GET_PLAYER(getParent()).getYieldAfricaBuyPrice(eYield) <= 0)
	{
		return false;
	}

	return m_em_bYieldEuropeTradable.get(eYield);
}
/******************************************************/

// R&R, ray, Port Royal
bool CvPlayer::canTradeWithPortRoyal() const
{
	return true; // Port Royal is not under the rule of your king
}

int CvPlayer::getSellToPortRoyalProfit(YieldTypes eYield, int iAmount) const
{
	if (getParent() == NO_PLAYER)
	{
		return 0;
	}

	if (!isYieldPortRoyalTradable(eYield))
	{
		return 0;
	}

	CvPlayer& kPlayerEurope = GET_PLAYER(getParent());

	int iPrice = iAmount * kPlayerEurope.getYieldPortRoyalBuyPrice(eYield);
	int iBribe = GC.getDefineINT("SMUGGLING_BRIBE_RATE") * 2; // twice bribe rate of smuggling
	iPrice -= (iPrice * iBribe) / 100;

	return iPrice;
}

bool CvPlayer::isYieldPortRoyalTradable(YieldTypes eYield) const
{
	FAssert(eYield >= 0 && eYield < NUM_YIELD_TYPES);

	if (getParent() == NO_PLAYER) 
	{
		return false;
	}

	if (!GC.getYieldInfo(eYield).isCargo())
	{
		return false;
	}

	if (!canTradeWithPortRoyal())
	{
		return false;
	}

	if (GET_PLAYER(getParent()).getYieldPortRoyalBuyPrice(eYield) <= 0)
	{
		return false;
	}
	
	return true; // Port Royal is not under the rule of your king
	//return m_em_bYieldEuropeTradable[eYield];
}
// R&R, ray, Port Royal - END

// transport feeder - start - Nightinggale
void CvPlayer::updateTransportThreshold()
{
	int iLoop;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->setAutoThresholdCache();
	}
}

void CvPlayer::updateTransportThreshold(YieldTypes eYield)
{
	int iLoop;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->setAutoThresholdCache(eYield);
	}
}
// transport feeder - end - Nightinggale

// R&R mod, vetiarvind, trade groups - start

int CvPlayer::addTradeRouteGroup(const std::wstring groupName)
{
	for (CvIdVector<CvTradeRouteGroup>::iterator it = m_aTradeGroups.begin(); it != m_aTradeGroups.end(); ++it)
	{
		CvTradeRouteGroup* pTradeRouteGroup = it->second;
		if (pTradeRouteGroup->getName(0).compare(groupName) == 0)			
		{
			pTradeRouteGroup->clearRoutes();
			return pTradeRouteGroup->getID();
		}
	}

	CvTradeRouteGroup* pTradeRouteGroup = m_aTradeGroups.addNew();
	pTradeRouteGroup->setName(groupName.c_str());	
	if (getID() == GC.getGameINLINE().getActivePlayer())
	{
		gDLL->getInterfaceIFace()->setDirty(Domestic_Advisor_DIRTY_BIT, true);
	}

	return pTradeRouteGroup->getID();
}


bool CvPlayer::editTradeRouteGroup(int iId, const std::wstring groupName)
{
	CvTradeRouteGroup* pTradeRouteGroup = getTradeRouteGroup(iId);
	if (pTradeRouteGroup == NULL)
	{
		return false;
	}

	for (CvIdVector<CvTradeRouteGroup>::iterator it = m_aTradeGroups.begin(); it != m_aTradeGroups.end(); ++it)	
	{
		CvTradeRouteGroup* pLoopTradeRouteGroup = it->second;
		if (pTradeRouteGroup->getName(0).compare(groupName) == 0)		
		{
			return false;
		}
	}

	pTradeRouteGroup->setName(groupName.c_str());	

	return true;
}


bool CvPlayer::removeTradeRouteGroup(int iId)
{	
	return m_aTradeGroups.removeById(iId);	
}



CvTradeRouteGroup* CvPlayer::getTradeRouteGroupById(int tradeGroupId) const
{		
	return m_aTradeGroups.getById(tradeGroupId);
}

CvTradeRouteGroup* CvPlayer::getTradeRouteGroup(int iIndex) const
{		
	FAssert(iIndex >= 0 && iIndex < getNumTradeGroups());	
	int i = 0;
	for (CvIdVector<CvTradeRouteGroup>::const_iterator it = m_aTradeGroups.begin(); it != m_aTradeGroups.end(); ++it)
	{
		if (i++ == iIndex)
		{
			return it->second;
		}
	}

	return NULL;
}

int CvPlayer::getNumTradeGroups() const
{	
	return m_aTradeGroups.size();
}

// R&R mod, vetiarvind, trade groups - end


namespace
{
	bool compareUnitValue(CvUnit* pUnitA, CvUnit* pUnitB)
	{
		// Erik: Sort the units by the absolute value
		// since some units may have a negative cost
		// to signify that they cannot be purchased
		return abs(pUnitA->getUnitInfo().getEuropeCost()) > abs(pUnitB->getUnitInfo().getEuropeCost());
	}
}

void CvPlayer::sortEuropeUnits()
{
	std::sort(m_aEuropeUnits.begin(), m_aEuropeUnits.end(), compareUnitValue);
}

void CvPlayer::writeDesyncLog(FILE *f) const
{
	fprintf(f, "Player %d %S\n", getID(), getName());

	fprintf(f, "\tLand: %d\n", getTotalLand());
	fprintf(f, "\tPopScore: %d\n", getPopScore());
	fprintf(f, "\tUnits: %d\n", getNumUnits());
	fprintf(f, "\tNumTradeRoutes: %d\n", getNumTradeRoutes());
	fprintf(f, "\tPower: %d\n", getPower());
	fprintf(f, "\tTax Rate: %d\n", getTaxRate());
	fprintf(f, "\tPrizes\n");
	for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
	{
		const CvYieldInfo& kYield = GC.getYieldInfo(eYield);
		if (kYield.isCargo() && isYieldEuropeTradable(eYield))
		{
			fprintf(f, "\t\t%S: %d %d\n", kYield.getDescription(), getYieldBuyPrice(eYield), getYieldSellPrice(eYield));
		}
	}

	int iLoop;
	for (const CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		fprintf(f, "\tCity %d %S\n", iLoop, pLoopCity->getName().c_str());
		pLoopCity->writeDesyncLog(f);
	}
}

void CvPlayer::read(FDataStreamBase* pStream)
{
	CvSavegameReaderBase readerbase(pStream);
	CvSavegameReader reader(readerbase);

	read(reader);
}

void CvPlayer::write(FDataStreamBase* pStream)
{
	CvSavegameWriterBase writerbase(pStream);
	CvSavegameWriter writer(writerbase);
	write(writer);
	writerbase.WriteFile();
}