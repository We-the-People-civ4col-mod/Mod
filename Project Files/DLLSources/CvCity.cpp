// city.cpp

#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "CvCity.h"
#include "CvArea.h"
#include "CvGameAI.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvTeamAI.h"
#include "CvGameCoreUtils.h"
#include "CvPlayerAI.h"
#include "CvUnit.h"
#include "CvInfos.h"
#include "CvRandom.h"
#include "CvArtFileMgr.h"
#include "CvPopupInfo.h"
#include "CyCity.h"
#include "CyArgsList.h"
#include "FProfiler.h"
#include "CvGameTextMgr.h"
#include "CvTradeRoute.h"
#include <numeric>

// interfaces used
#include "CvDLLEngineIFaceBase.h"
#include "CvDLLPythonIFaceBase.h"
#include "CvDLLEntityIFaceBase.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLEventReporterIFaceBase.h"

// Public Functions...

CvCity::CvCity()
{
	m_aiLandPlotYield = new int[NUM_YIELD_TYPES]; // R&R, ray, Landplot Yields
	m_aiSeaPlotYield = new int[NUM_YIELD_TYPES];
	m_aiRiverPlotYield = new int[NUM_YIELD_TYPES];
	m_aiYieldRateModifier = new int[NUM_YIELD_TYPES];
	m_aiYieldStored = new int[NUM_YIELD_TYPES];
	m_aiYieldRushed = new int[NUM_YIELD_TYPES];
	// R&R, Androrc, Domestic Market
	m_aiYieldBuyPrice = new int[NUM_YIELD_TYPES];
	//Androrc End

	m_aiDomainFreeExperience = new int[NUM_DOMAIN_TYPES];
	m_aiDomainProductionModifier = new int[NUM_DOMAIN_TYPES];

	m_aiCulture = new int[MAX_PLAYERS];
	m_abEverOwned = new bool[MAX_PLAYERS];
	m_abRevealed = new bool[MAX_TEAMS];
	m_abScoutVisited = new bool[MAX_TEAMS];

	m_paiBuildingProduction = NULL;
	m_paiBuildingProductionTime = NULL;
	m_paiBuildingOriginalOwner = NULL;
	m_paiBuildingOriginalTime = NULL;
	m_paiUnitProduction = NULL;
	m_paiUnitProductionTime = NULL;
	m_aiSpecialistWeights = NULL;
	m_paiUnitCombatFreeExperience = NULL;
	m_paiFreePromotionCount = NULL;
	m_pabHasRealBuilding = NULL;
	m_pabHasFreeBuilding = NULL;

	m_paiWorkingPlot = NULL;

	m_paTradeCities = NULL;

	CvDLLEntity::createCityEntity(this);		// create and attach entity to city

	m_aiBaseYieldRank = new int[NUM_YIELD_TYPES];
	m_abBaseYieldRankValid = new bool[NUM_YIELD_TYPES];
	m_aiYieldRank = new int[NUM_YIELD_TYPES];
	m_abYieldRankValid = new bool[NUM_YIELD_TYPES];

	m_ePreferredYieldAtCityPlot = NO_YIELD;

	reset(0, NO_PLAYER, 0, 0, true);
}

CvCity::~CvCity()
{
	CvDLLEntity::removeEntity();			// remove entity from engine
	CvDLLEntity::destroyEntity();			// delete CvCityEntity and detach from us

	uninit();

	SAFE_DELETE_ARRAY(m_aiBaseYieldRank);
	SAFE_DELETE_ARRAY(m_abBaseYieldRankValid);
	SAFE_DELETE_ARRAY(m_aiYieldRank);
	SAFE_DELETE_ARRAY(m_abYieldRankValid);

	SAFE_DELETE_ARRAY(m_aiLandPlotYield); // R&R, ray, Landplot Yields
	SAFE_DELETE_ARRAY(m_aiSeaPlotYield);
	SAFE_DELETE_ARRAY(m_aiRiverPlotYield);
	SAFE_DELETE_ARRAY(m_aiYieldRateModifier);
	SAFE_DELETE_ARRAY(m_aiYieldStored);
	SAFE_DELETE_ARRAY(m_aiYieldRushed);
	// R&R, Androrc, Domestic Market
	SAFE_DELETE_ARRAY(m_aiYieldBuyPrice);
	//Androrc End
	SAFE_DELETE_ARRAY(m_aiDomainFreeExperience);
	SAFE_DELETE_ARRAY(m_aiDomainProductionModifier);
	SAFE_DELETE_ARRAY(m_aiCulture);
	SAFE_DELETE_ARRAY(m_abEverOwned);
	SAFE_DELETE_ARRAY(m_abRevealed);
	SAFE_DELETE_ARRAY(m_abScoutVisited);
}


void CvCity::init(int iID, PlayerTypes eOwner, int iX, int iY, bool bBumpUnits)
{
	std::vector<int> aOldAttitude(MAX_PLAYERS, 0);
	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes) i );
		if (kPlayer.isAlive())
		{
			kPlayer.AI_invalidateCloseBordersAttitudeCache();
			aOldAttitude[i] = kPlayer.AI_getStolenPlotsAttitude(eOwner);
		}
	}

	CvPlot* pAdjacentPlot;
	CvPlot* pPlot;
	BuildingTypes eLoopBuilding;
	
	pPlot = GC.getMapINLINE().plotINLINE(iX, iY);

	//--------------------------------
	// Log this event
	if (GC.getLogging())
	{
		if (gDLL->getChtLvl() > 0)
		{
			TCHAR szOut[1024];
			sprintf(szOut, "Player %d City %d built at %d:%d\n", eOwner, iID, iX, iY);
			gDLL->messageControlLog(szOut);
		}
	}

	//--------------------------------
	// Init saved data
	reset(iID, eOwner, pPlot->getX_INLINE(), pPlot->getY_INLINE());
	// R&R, ray, adjustment Domestic Markets for Luxury Goods needed
	// R&R, Androrc, Domestic Market
	if(!isNative())
	{
		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			YieldTypes eYield = (YieldTypes) iYield;
			CvYieldInfo& kYield = GC.getYieldInfo(eYield);
			FAssert(kYield.getBuyPriceHigh() >= kYield.getBuyPriceLow());

			// Luxury Goods should also give a little profit
			if (eYield == YIELD_LUXURY_GOODS)
			{
				int iBuyPrice = GET_PLAYER(getOwnerINLINE()).getYieldSellPrice(eYield) + GC.getPRICE_DIFF_EUROPE_DOMESTIC_LUXURY_GOODS();
				setYieldBuyPrice(eYield, iBuyPrice);
			}
			else
			{
				int iBuyPrice = kYield.getBuyPriceLow() + GC.getGameINLINE().getSorenRandNum(kYield.getBuyPriceHigh() - kYield.getBuyPriceLow() + 1, "Yield Price");
				setYieldBuyPrice(eYield, iBuyPrice);
			}
		}
	}
	//Androrc End

	// R&R, ray, finishing Custom House Screen
	// initializing default data
	if(!isNative())
	{
		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			YieldTypes eYield = (YieldTypes) iYield;
			CvYieldInfo& kYield = GC.getYieldInfo(eYield);
			int iGameSpeedModifier = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getStoragePercent() / 100; // R&R, ray adjusting defaults of Custom House Screen to Gamespeed
			if (kYield.isCargo())
			{
				// strategic raw
				if (eYield == YIELD_ORE || eYield == YIELD_HEMP)
				{
					ma_aiCustomHouseSellThreshold.set(GC.getCUSTOMHOUSE_STRATEGIC_RAW_SELL_THRESHOLD() * iGameSpeedModifier, eYield);
				}
				// strategic produced
				else if ( eYield == YIELD_TOOLS || eYield == YIELD_BLADES || eYield == YIELD_MUSKETS || eYield == YIELD_CANNONS || eYield == YIELD_SHEEP || eYield == YIELD_CATTLE || eYield == YIELD_HORSES || eYield == YIELD_ROPE || eYield == YIELD_SAILCLOTH || eYield == YIELD_TRADE_GOODS || eYield == YIELD_LUXURY_GOODS)
				{
					ma_aiCustomHouseSellThreshold.set(GC.getCUSTOMHOUSE_STRATEGIC_PRODUCED_SELL_THRESHOLD() * iGameSpeedModifier, eYield);
				}

				// raw
				else if ( eYield == YIELD_HIDES || eYield == YIELD_FUR || eYield == YIELD_PREMIUM_FUR || eYield == YIELD_BARLEY || eYield == YIELD_SUGAR || eYield == YIELD_GRAPES || eYield == YIELD_COCOA_FRUITS || eYield == YIELD_COFFEE_BERRIES || eYield == YIELD_RAW_SALT || eYield == YIELD_RED_PEPPER || eYield == YIELD_WOOL || eYield == YIELD_COTTON || eYield == YIELD_INDIGO || eYield == YIELD_TOBACCO || eYield == YIELD_WHALE_BLUBBER || eYield == YIELD_VALUABLE_WOOD)
				{
					ma_aiCustomHouseSellThreshold.set(GC.getCUSTOMHOUSE_RAW_SELL_THRESHOLD() * iGameSpeedModifier, eYield);
				}

				// produced
				else if (eYield == YIELD_COCA_LEAVES || eYield == YIELD_SILVER || eYield == YIELD_GOLD || eYield == YIELD_GEMS || eYield == YIELD_COCOA || eYield == YIELD_COFFEE || eYield == YIELD_SALT ||  eYield == YIELD_SPICES || eYield == YIELD_WOOL_CLOTH || eYield == YIELD_CLOTH || eYield == YIELD_COLOURED_CLOTH || eYield == YIELD_LEATHER || eYield == YIELD_COATS || eYield == YIELD_PREMIUM_COATS || eYield == YIELD_BEER || eYield == YIELD_RUM || eYield == YIELD_WINE || eYield == YIELD_CIGARS || eYield == YIELD_WHALE_OIL || eYield == YIELD_FURNITURE)
				{
					ma_aiCustomHouseSellThreshold.set(GC.getCUSTOMHOUSE_PRODUCED_SELL_THRESHOLD() * iGameSpeedModifier, eYield);
				}

				// default for safety
				else
				{
					ma_aiCustomHouseSellThreshold.set(getYieldStored(eYield) * iGameSpeedModifier / 2, eYield);
				}

				if (eYield == YIELD_FOOD || eYield == YIELD_LUMBER || eYield == YIELD_STONE)
				{
					ma_aiCustomHouseNeverSell.set(true, eYield);
				}
				else
				{
					ma_aiCustomHouseNeverSell.set(false, eYield);
				}
			}
		}
	}
	// R&R, ray, finishing Custom House Screen END

	//--------------------------------
	// Init non-saved data
	setupGraphical();

	//--------------------------------
	// Init other game data
	setName(GET_PLAYER(getOwnerINLINE()).addNewCityName());

	//pick teach unit
	int iBestValue = 0;

	if (isNative()) // R&R, ray, code improvement
	{
		setTeachUnitClass(bestTeachUnitClass());
	}
	
	setEverOwned(getOwnerINLINE(), true);

	updateCultureLevel();

	if (pPlot->getCulture(getOwnerINLINE()) < GC.getDefineINT("FREE_CITY_CULTURE"))
	{
		pPlot->setCulture(getOwnerINLINE(), GC.getDefineINT("FREE_CITY_CULTURE"), bBumpUnits);
	}
	pPlot->setOwner(getOwnerINLINE(), bBumpUnits);
	pPlot->setPlotCity(this);

	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->getCulture(getOwnerINLINE()) < GC.getDefineINT("FREE_CITY_ADJACENT_CULTURE"))
			{
				pAdjacentPlot->setCulture(getOwnerINLINE(), GC.getDefineINT("FREE_CITY_ADJACENT_CULTURE"), bBumpUnits);
			}
			pAdjacentPlot->updateCulture(bBumpUnits);
		}
	}

	CyArgsList argsList;
	argsList.add(iX);
	argsList.add(iY);
	long lResult=0;
	gDLL->getPythonIFace()->callFunction(PYGameModule, "citiesDestroyFeatures", argsList.makeFunctionArgs(), &lResult);

	if (lResult == 1)
	{
		if (pPlot->getFeatureType() != NO_FEATURE)
		{
			pPlot->setFeatureType(NO_FEATURE);
		}
	}

	pPlot->setImprovementType(NO_IMPROVEMENT);
	pPlot->updateCityRoute();

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (pPlot->isVisible(((TeamTypes)iI), false))
			{
				setRevealed(((TeamTypes)iI), true);
			}
		}
	}

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (GET_PLAYER(getOwnerINLINE()).isBuildingFree((BuildingTypes)iI))
		{
			setHasFreeBuilding(((BuildingTypes)iI), true);
		}
	}

	area()->changeCitiesPerPlayer(getOwnerINLINE(), 1);

	GET_TEAM(getTeam()).changeNumCities(1);

	GC.getGameINLINE().changeNumCities(1);

	setGameTurnFounded(GC.getGameINLINE().getGameTurn());
	setGameTurnAcquired(GC.getGameINLINE().getGameTurn());

	pPlot->updateYield(false);
	setYieldRateDirty();

	changePopulation(GC.getDefineINT("INITIAL_CITY_POPULATION") + GC.getEraInfo(GC.getGameINLINE().getStartEra()).getFreePopulation());

	GC.getMapINLINE().updateWorkingCity();

	GC.getGameINLINE().AI_makeAssignWorkDirty();

	GET_PLAYER(getOwnerINLINE()).setFoundedFirstCity(true);

	if (GC.getGameINLINE().isFinalInitialized() || isNative())
	{
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			if (GC.getCivilizationInfo(getCivilizationType()).isCivilizationFreeBuildingClass(iI))
			{
				eLoopBuilding = ((BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(iI)));

				if (eLoopBuilding != NO_BUILDING)
				{
					if (GET_PLAYER(getOwnerINLINE()).getNumCities() == 1 || !GC.getBuildingInfo(eLoopBuilding).isCapital())
					{
						setHasRealBuilding(eLoopBuilding, true);
					}
				}
			}
		}

		if (!isHuman())
		{
			changeOverflowProduction(GC.getDefineINT("INITIAL_AI_CITY_PRODUCTION"), 0);
		}
	}
	
	
	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes) i );
		
		if (kPlayer.isAlive())
		{
			kPlayer.AI_invalidateCloseBordersAttitudeCache();
			kPlayer.AI_diplomaticHissyFit(getOwnerINLINE(), kPlayer.AI_getStolenPlotsAttitude(eOwner) - aOldAttitude[i]);
		}
	}

	UpdateBuildingAffectedCache(); // building affected cache - Nightinggale
	
	GET_PLAYER(getOwnerINLINE()).AI_invalidateDistanceMap();
	AI_init();
}


void CvCity::uninit()
{
	SAFE_DELETE_ARRAY(m_paiBuildingProduction);
	SAFE_DELETE_ARRAY(m_paiBuildingProductionTime);
	SAFE_DELETE_ARRAY(m_paiBuildingOriginalOwner);
	SAFE_DELETE_ARRAY(m_paiBuildingOriginalTime);
	SAFE_DELETE_ARRAY(m_paiUnitProduction);
	SAFE_DELETE_ARRAY(m_paiUnitProductionTime);
	SAFE_DELETE_ARRAY(m_aiSpecialistWeights);
	// Teacher List - start - Nightinggale
	ma_OrderedStudents.reset();
	ma_OrderedStudentsRepeat.reset();
	// Teacher List - end - Nightinggale
	SAFE_DELETE_ARRAY(m_paiUnitCombatFreeExperience);
	SAFE_DELETE_ARRAY(m_paiFreePromotionCount);
	SAFE_DELETE_ARRAY(m_pabHasRealBuilding);
	SAFE_DELETE_ARRAY(m_pabHasFreeBuilding);

	SAFE_DELETE_ARRAY(m_paiWorkingPlot);

	SAFE_DELETE_ARRAY(m_paTradeCities);

	m_orderQueue.clear();

	//free unit population
	for (uint i = 0; i < m_aPopulationUnits.size(); ++i)
	{
		SAFE_DELETE(m_aPopulationUnits[i]);
	}
	m_aPopulationUnits.clear();

	// traderoute just-in-time - start - Nightinggale
	ma_tradeImports.reset();
	ma_tradeExports.reset();
	ma_tradeThreshold.reset();
	// traderoute just-in-time - end - Nightinggale
	ma_tradeMaxThreshold.reset(); // R&R mod, vetiarvind, max yield import limit
	// transport feeder - start - Nightinggale
	ma_tradeImportsMaintain.reset();
	// transport feeder - end - Nightinggale
}

// FUNCTION: reset()
// Initializes data members that are serialized.
void CvCity::reset(int iID, PlayerTypes eOwner, int iX, int iY, bool bConstructorCall)
{
	int iI;

	//--------------------------------
	// Uninit class
	uninit();

	m_iID = iID;
	m_iX = iX;
	m_iY = iY;
	m_iRallyX = INVALID_PLOT_COORD;
	m_iRallyY = INVALID_PLOT_COORD;
	m_iGameTurnFounded = 0;
	m_iGameTurnAcquired = 0;
	m_iHighestPopulation = 0;
	m_iWorkingPopulation = 0;
	m_iNumBuildings = 0;
	m_iHealRate = 0;
	m_iFoodKept = 0;
	m_iMaxFoodKeptPercent = 0;
	m_iOverflowProduction = 0;
	m_iMilitaryProductionModifier = 0;
	m_iBuildingDefense = 0;
	m_iBuildingBombardDefense = 0;
	m_iFreeExperience = 0;
	m_iDefenseDamage = 0;
	m_iLastDefenseDamage = 0;
	m_iOccupationTimer = 0;
	m_iCultureUpdateTimer = 0;
	m_iCitySizeBoost = 0;
	m_iHammers = 0;
	m_iMissionaryRate = 0;
	m_iNativeTradeRate = 0; // WTP, ray, Native Trade Posts - START
	m_iTradePostGold = 0; // WTP, ray, Native Trade Posts - START
	m_bStirredUp = false; // R&R, ray , Stirring Up Natives - START
	m_iWorksWaterCount = 0;
	m_iRebelSentiment = 0;
	m_iCityHealth = 0; // R&R, ray, Health
	m_iCityHappiness = 0; // WTP, ray, Happiness - START
	m_iCityUnHappiness = 0; // WTP, ray, Happiness - START
	m_iCityTimerFestivitiesOrUnrest = 0; // WTP, ray, Happiness - START
	m_iTeachUnitMultiplier = 100;
	m_iEducationThresholdMultiplier = 100;
	m_iTotalYieldStored = 0; //VET NewCapacity - 1/9

	m_bNeverLost = true;
	m_bBombarded = false;
	m_bProductionAutomated = false;
	m_bWallOverride = false;
	m_bInfoDirty = true;
	m_bLayoutDirty = false;

	m_eOwner = eOwner;
	m_ePreviousOwner = NO_PLAYER;
	m_eOriginalOwner = eOwner;
	m_eCultureLevel = NO_CULTURELEVEL;
	m_eTeachUnitClass = NO_UNITCLASS;
	m_eMissionaryPlayer = NO_PLAYER;
	m_eTradePostPlayer = NO_PLAYER; // WTP, ray, Native Trade Posts - START

	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_aiLandPlotYield[iI] = 0; // R&R, ray, Landplot Yields
		m_aiSeaPlotYield[iI] = 0;
		m_aiRiverPlotYield[iI] = 0;
		m_aiYieldRateModifier[iI] = 0;
		m_aiYieldStored[iI] = 0;
		m_aiYieldRushed[iI] = 0;
		// R&R, Androrc, Domestic Market
		m_aiYieldBuyPrice[iI] = 0;
		//Androrc End
	}
	// R&R, ray, finishing Custom House Screen
	ma_aiCustomHouseSellThreshold.reset();
	ma_aiCustomHouseNeverSell.reset();
	// R&R, ray, finishing Custom House Screen END

	for (iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		m_aiDomainFreeExperience[iI] = 0;
		m_aiDomainProductionModifier[iI] = 0;
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_aiCulture[iI] = 0;
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_abEverOwned[iI] = false;
	}

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_abRevealed[iI] = false;
		m_abScoutVisited[iI] = false;
	}

	clear(m_szName);
	m_szScriptData = "";

	m_bPopulationRankValid = false;
	m_iPopulationRank = -1;

	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_abBaseYieldRankValid[iI] = false;
		m_abYieldRankValid[iI] = false;
		m_aiBaseYieldRank[iI] = -1;
		m_aiYieldRank[iI] = -1;
	}

	if (!bConstructorCall)
	{
		FAssertMsg((0 < GC.getNumBuildingInfos()),  "GC.getNumBuildingInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		//m_ppBuildings = new CvBuilding *[GC.getNumBuildingInfos()];
		m_paiBuildingProduction = new int[GC.getNumBuildingInfos()];
		m_paiBuildingProductionTime = new int[GC.getNumBuildingInfos()];
		m_paiBuildingOriginalOwner = new int[GC.getNumBuildingInfos()];
		m_paiBuildingOriginalTime = new int[GC.getNumBuildingInfos()];
		m_pabHasRealBuilding = new bool[GC.getNumBuildingInfos()];
		m_pabHasFreeBuilding = new bool[GC.getNumBuildingInfos()];
		for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			//m_ppBuildings[iI] = NULL;
			m_paiBuildingProduction[iI] = 0;
			m_paiBuildingProductionTime[iI] = 0;
			m_paiBuildingOriginalOwner[iI] = -1;
			m_paiBuildingOriginalTime[iI] = MIN_INT;
			m_pabHasRealBuilding[iI] = false;
			m_pabHasFreeBuilding[iI] = false;
		}

		FAssertMsg((0 < GC.getNumUnitInfos()),  "GC.getNumUnitInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		m_paiUnitProduction = new int[GC.getNumUnitInfos()];
		m_paiUnitProductionTime = new int[GC.getNumUnitInfos()];
		m_aiSpecialistWeights = new int[GC.getNumUnitInfos()];
		for (iI = 0;iI < GC.getNumUnitInfos();iI++)
		{
			m_paiUnitProduction[iI] = 0;
			m_paiUnitProductionTime[iI] = 0;
			m_aiSpecialistWeights[iI] = 0;
		}

		FAssertMsg((0 < GC.getNumUnitCombatInfos()),  "GC.getNumUnitCombatInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		m_paiUnitCombatFreeExperience = new int[GC.getNumUnitCombatInfos()];
		for (iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
		{
			m_paiUnitCombatFreeExperience[iI] = 0;
		}

		FAssertMsg((0 < GC.getNumPromotionInfos()),  "GC.getNumPromotionInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		m_paiFreePromotionCount = new int[GC.getNumPromotionInfos()];
		for (iI = 0; iI < GC.getNumPromotionInfos(); iI++)
		{
			m_paiFreePromotionCount[iI] = 0;
		}

		FAssertMsg((0 < NUM_CITY_PLOTS),  "NUM_CITY_PLOTS is not greater than zero but an array is being allocated in CvCity::reset");
		m_paiWorkingPlot = new int[NUM_CITY_PLOTS];
		for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
			m_paiWorkingPlot[iI] = -1;
		}

		m_aEventsOccured.clear();
		m_aBuildingYieldChange.clear();
	}

	//storage loss trading (aka customs house and related things)
	initCacheStorageLossTradeValues();


	if (!bConstructorCall)
	{
		AI_reset();
	}
}


//////////////////////////////////////
// graphical only setup
//////////////////////////////////////
void CvCity::setupGraphical()
{
	if (!GC.IsGraphicsInitialized())
	{
		return;
	}

	CvDLLEntity::setup();

	setBillboardDirty(true);
	setLayoutDirty(true);
}

void CvCity::kill()
{
	CvPlot* pPlot = plot();

	removeTradeRoutes();

	if (isNative())
	{
		std::vector<CvUnit*> aUnits;
		CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
		while (pUnitNode)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = plot()->nextUnitNode(pUnitNode);

			if (pLoopUnit->getUnitTravelState() == UNIT_TRAVEL_STATE_LIVE_AMONG_NATIVES)
			{
				aUnits.push_back(pLoopUnit);
			}
		}

		for (uint i = 0; i < aUnits.size(); ++i)
		{
			aUnits[i]->kill(false);
		}
	}

	if (isCitySelected())
	{
		gDLL->getInterfaceIFace()->clearSelectedCities();
	}

	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		CvPlot* pLoopPlot = getCityIndexPlot(iI);

		if (pLoopPlot != NULL)
		{
			if (pLoopPlot->getWorkingCityOverride() == this)
			{
				pLoopPlot->setWorkingCityOverride(NULL);
			}
		}
	}

	setCultureLevel(NO_CULTURELEVEL);

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		setHasRealBuilding(((BuildingTypes)iI), false);
		setHasFreeBuilding(((BuildingTypes)iI), false);
	}

	for (int i = 0; i < NUM_YIELD_TYPES; ++i)
	{
		setYieldStored((YieldTypes) i, 0);
	}

	for (uint i = 0; i < m_aPopulationUnits.size(); ++i)
	{
		m_aPopulationUnits[i]->setProfession(NO_PROFESSION);
		m_aPopulationUnits[i]->updateOwnerCache(-1);
	}

	clearOrderQueue();

	//remove home plot yields
	setYieldRateDirty();

	pPlot->setPlotCity(NULL);

	area()->changeCitiesPerPlayer(getOwnerINLINE(), -1);

	GET_TEAM(getTeam()).changeNumCities(-1);

	GC.getGameINLINE().changeNumCities(-1);

	FAssert(!isProduction());

	PlayerTypes eOwner = getOwnerINLINE();

	pPlot->setImprovementType((ImprovementTypes)(GC.getDefineINT("RUINS_IMPROVEMENT")));

	gDLL->getEventReporterIFace()->cityLost(this);

	GET_PLAYER(getOwnerINLINE()).AI_invalidateDistanceMap();
	GET_PLAYER(getOwnerINLINE()).deleteCity(getID());

	pPlot->updateCulture(true);

	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pAdjacentPlot = plotDirection(pPlot->getX_INLINE(), pPlot->getY_INLINE(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			pAdjacentPlot->updateCulture(true);
		}
	}

	GC.getMapINLINE().updateWorkingCity();

	GC.getGameINLINE().AI_makeAssignWorkDirty();

	if (eOwner == GC.getGameINLINE().getActivePlayer())
	{
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
	}
}


void CvCity::doTurn()
{
	PROFILE_FUNC();

	CvPlot* pLoopPlot;
	int iI;

	if (!isBombarded())
	{
		changeDefenseDamage(-(GC.getDefineINT("CITY_DEFENSE_DAMAGE_HEAL_RATE")));
	}

	setLastDefenseDamage(getDefenseDamage());
	setBombarded(false);

	AI_doTurn();

	bool bAllowNoProduction = !doCheckProduction();

	doSpecialists();
	doYields();
	doGrowth();
	doCulture();
	doPlotCulture(false, getOwnerINLINE(), getCultureRate());
	doProduction(bAllowNoProduction);
	
	if (!isNative())
	{
		doExtraCityDefenseAttacks(); // R&R, ray, Extra City Defense Attacks
		doEntertainmentBuildings(); // R&R, ray, Entertainment Buildings
		doLbD(); // TAC - LBD - Ray - START
		doPrices(); // R&R, Androrc Domestic Market
		doCityHealth(); // R&R, ray, Health
		// WTP, ray, Happiness - START
		updateCityHappiness();
		updateCityUnHappiness();
		changeCityTimerFestivitiesOrUnrest(-1);
		doCityHappiness();
		doCityUnHappiness();
		// WTP, ray, Happiness - END
	}
	
	doDecay();

	doMissionaries();

	// WTP, ray, Native Trade Posts - START
	if (isNative())
	{
		doNativeTradePost();
	}
	// WTP, ray, Native Trade Posts - END

	doRebelSentiment();

	if (!isDisorder())
	{
		for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
			pLoopPlot = getCityIndexPlot(iI);

			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->getWorkingCity() == this)
				{
					if (pLoopPlot->isBeingWorked())
					{
						pLoopPlot->doImprovement();
					}
				}
			}
		}
	}

	if (getCultureUpdateTimer() > 0)
	{
		changeCultureUpdateTimer(-1);
	}

	if (getOccupationTimer() > 0)
	{
		changeOccupationTimer(-1);
		// WTP, ray, Change for Request "Occupation has ended" - START
		// WTP, ray, if Occupation has ended now, we will send a message that Occupation is over
		if (isOccupation() == FALSE)
		{
			gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_CITY_OCCUPATION_ENDED", getNameKey()), "AS2D_POSITIVE_DINK", MESSAGE_TYPE_INFO, GC.getYieldInfo(YIELD_CULTURE).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE(), true, true);
		}
		// WTP, ray, Change for Request "Occupation has ended" - START
	}

	for (uint i = 0; i < m_aPopulationUnits.size(); ++i)
	{
		m_aPopulationUnits[i]->doTurn();
	}

	// ONEVENT - Do turn
	gDLL->getEventReporterIFace()->cityDoTurn(this, getOwnerINLINE());
}


bool CvCity::isCitySelected() const
{
	return gDLL->getInterfaceIFace()->isCitySelected(this);
}


bool CvCity::canBeSelected() const
{
	if ((getTeam() == GC.getGameINLINE().getActiveTeam()) || GC.getGameINLINE().isDebugMode())
	{
		return true;
	}

	return false;
}


void CvCity::updateSelectedCity()
{
	CvPlot* pLoopPlot;
	int iI;

	for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		pLoopPlot = getCityIndexPlot(iI);

		if (pLoopPlot != NULL)
		{
			pLoopPlot->updateShowCitySymbols();
		}
	}
}


void CvCity::updateYield()
{
	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		CvPlot* pLoopPlot = getCityIndexPlot(iI);

		if (pLoopPlot != NULL)
		{
			pLoopPlot->updateYield(true);
		}
	}
}


// XXX kill this?
void CvCity::updateVisibility()
{
	PROFILE_FUNC();

	if (!GC.IsGraphicsInitialized())
	{
		return;
	}

	FAssert(GC.getGameINLINE().getActiveTeam() != NO_TEAM);

	CvDLLEntity::setVisible(isRevealed(GC.getGameINLINE().getActiveTeam(), true));
}


void CvCity::createGreatGeneral(UnitTypes eGreatGeneralUnit, bool bIncrementExperience)
{
	GET_PLAYER(getOwnerINLINE()).createGreatGeneral(eGreatGeneralUnit, bIncrementExperience, getX_INLINE(), getY_INLINE());
}

// R&R, ray, Great Admirals - START
void CvCity::createGreatAdmiral(UnitTypes eGreatAdmiralUnit, bool bIncrementExperience)
{
	GET_PLAYER(getOwnerINLINE()).createGreatAdmiral(eGreatAdmiralUnit, bIncrementExperience, getX_INLINE(), getY_INLINE());
}
// R&R, ray, Great Admirals - END

void CvCity::doTask(TaskTypes eTask, int iData1, int iData2, bool bOption, bool bAlt, bool bShift, bool bCtrl)
{
	switch (eTask)
	{
	case TASK_RAZE:
		GET_PLAYER(getOwnerINLINE()).raze(this);
		break;

	case TASK_GIFT:
		ejectTeachUnits();
		if (isNative() && !GET_PLAYER((PlayerTypes)iData1).isNative())
		{
			for (int i = 0; i < NUM_CITY_PLOTS; ++i)
			{
				CvPlot* pLoopPlot = ::plotCity(getX_INLINE(), getY_INLINE(), i);
				if (pLoopPlot != NULL)
				{
					pLoopPlot->setCulture(getOwnerINLINE(), 0, false);
				}
			}

			while (getPopulation() > 1)
			{
				if (!AI_removeWorstPopulationUnit(false))
				{
					FAssertMsg(false, "Could not eject unit");
					break;
				}
			}

			GET_PLAYER(getOwnerINLINE()).disband(this);
		}
		else
		{
			GET_PLAYER((PlayerTypes)iData1).acquireCity(this, false, true);
		}
		break;

	case TASK_LIBERATE:
		ejectTeachUnits();
		liberate(iData1 != 0);
		break;

	case TASK_SET_ALL_CITIZENS_AUTOMATED:
		setAllCitizensAutomated(iData1 != 0);
		break;

	case TASK_SET_CITIZEN_AUTOMATED:
		setCitizenAutomated(iData1);
		break;

	case TASK_SET_AUTOMATED_PRODUCTION:
		setProductionAutomated(bOption, bAlt && bShift && bCtrl);
		break;

	case TASK_SET_EMPHASIZE:
		AI_setEmphasize(((EmphasizeTypes)iData1), bOption);
		break;

	case TASK_CHANGE_WORKING_PLOT:
		alterUnitWorkingPlot(iData1, iData2, bOption);
		break;

	//Androrc Multiple Professions per Building
	case TASK_CHANGE_WORKING_BUILDING:
		alterUnitWorkingBuilding((BuildingTypes) iData1, iData2, bOption);
		break;
	//Androrc End

	case TASK_CHANGE_PROFESSION:
		alterUnitProfession(iData1, (ProfessionTypes) iData2);
		break;

	case TASK_EJECT_TO_TRANSPORT:
		ejectToTransport(iData1, iData2);
		break;

	case TASK_REPLACE_CITIZEN:
		replaceCitizen(iData1, iData2, bOption);
		break;

	case TASK_CLEAR_WORKING_OVERRIDE:
		clearWorkingOverride(iData1);
		break;

	case TASK_HURRY:
		hurry((HurryTypes)iData1);
		break;

	case TASK_CLEAR_ORDERS:
		clearOrderQueue();
		break;

	case TASK_RALLY_PLOT:
		setRallyPlot(GC.getMapINLINE().plotINLINE(iData1, iData2));
		break;

	case TASK_CLEAR_RALLY_PLOT:
		setRallyPlot(NULL);
		break;

	case TASK_YIELD_IMPORT:
		if (iData2 != 0)
		{
			addImport((YieldTypes) iData1);
		}
		else
		{
			removeImport((YieldTypes) iData1);
		}
		break;

	case TASK_YIELD_EXPORT:
		if (iData2 != 0)
		{
			addExport((YieldTypes) iData1);
		}
		else
		{
			removeExport((YieldTypes) iData1);
		}
		break;

	case TASK_YIELD_LEVEL:
		setMaintainLevel((YieldTypes) iData1, iData2);
		break;

	// transport feeder - start - Nightinggale
	case TASK_YIELD_TRADEROUTE:
		setAutoExport((YieldTypes)iData1, bCtrl); // auto traderoute - Nightinggale

		// change all traderoute settings for a single yield in one go as setImportsMaintain() needs to be called after updating the other settings
		if (bOption || bShift)
		{
			// enable import when requested by import or feeder service checkboxes (just one of them is enough)
			addImport((YieldTypes) iData1);
		}
		else
		{
			removeImport((YieldTypes) iData1);
		}

		if (bAlt)
		{
			addExport((YieldTypes) iData1);
		}
		else
		{
			removeExport((YieldTypes) iData1);
		}

		// R&R mod, vetiarvind, max yield import limit - start | implement Nightinggale's bit twiddling optimization
		{
			// see CvDLLButtonPopup::OnOkClicked() for details on how those ints are stored
			int iMaintainLevel    =  iData2 & 0xFFFF;
			int iImportLimitLevel =  (iData2 >> 16) & 0xFFFF;
			setImportsLimit((YieldTypes) iData1, iImportLimitLevel);
			setMaintainLevel((YieldTypes) iData1, iMaintainLevel);
		}
		// R&R mod, vetiarvind, max yield import limit - end

		
		setImportsMaintain((YieldTypes) iData1, bShift);
		if (!bOption)
		{
			// just in case import and import feeder are both turned off at the same time
			removeImport((YieldTypes) iData1);
		}
		doAutoExport((YieldTypes) iData1); // auto traderoute - Nightinggale
		break;
	// transport feeder - end - Nightinggale

	// auto traderoute - start - Nightinggale
	case TASK_AUTO_TRADEROUTE:
		handleAutoTraderouteSetup(bOption, bAlt, bShift);
		break;
	// auto traderoute - end - Nightinggale

	case TASK_CLEAR_SPECIALTY:
		{
			CvUnit* pUnit = GET_PLAYER(getOwnerINLINE()).getUnit(iData1);
			if (pUnit == NULL)
			{
				pUnit = getPopulationUnitById(iData1);
			}

			if (pUnit != NULL)
			{
				pUnit->clearSpecialty();
			}
		}
		break;

	case TASK_EDUCATE:
		educateStudent(iData1, (UnitTypes) iData2);
		break;

	case TASK_PUSH_TRAIN_UNIT:
		if (canTrain((UnitTypes) iData1))
		{
			pushOrder(ORDER_TRAIN, iData1, iData2, false, false, false);
			if (checkRequiredYields(ORDER_TRAIN, iData1) && getProduction() >= getProductionNeeded(YIELD_HAMMERS))
			{
				popOrder(0, true, false);
			}
		}
		break;

	case TASK_PUSH_CONSTRUCT_BUILDING:
		if (canConstruct((BuildingTypes) iData1))
		{
			pushOrder(ORDER_CONSTRUCT, iData1, iData2, false, false, false);
			if (checkRequiredYields(ORDER_CONSTRUCT, iData1) && getProduction() >= getProductionNeeded(YIELD_HAMMERS))
			{
				popOrder(0, true, false);
			}
		}
		break;

	case TASK_CHEAT:
		doCheat(bAlt, bShift, bCtrl);
		break;

	// custom house - network fix - start - Nightinggale
	case TASK_CHANGE_CUSTOM_HOUSE_SETTINGS:
		setCustomHouseNeverSell((YieldTypes)iData1, bOption);
		setCustomHouseSellThreshold((YieldTypes)iData1, iData2);
		break;
	// custom house - network fix - end - Nightinggale

	// Teacher List - start - Nightinggale
	case TASK_CHANGE_ORDERED_STUDENTS:
		setOrderedStudents((UnitTypes)iData1, iData2, bOption, bAlt, bShift);
		break;
	// Teacher List - end - Nightinggale

	case TASK_CHOOSE_CITY_PLOT_YIELD:
		setPreferredYieldAtCityPlot(static_cast<YieldTypes>(iData1));
		break;

	default:
		FAssertMsg(false, "eTask failed to match a valid option");
		break;
	}
}


void CvCity::chooseProduction(UnitTypes eTrainUnit, BuildingTypes eConstructBuilding, bool bFinish, bool bFront)
{
	CvPopupInfo* pPopupInfo = new CvPopupInfo(BUTTONPOPUP_CHOOSEPRODUCTION);
	pPopupInfo->setData1(getID());
	pPopupInfo->setOption1(bFinish);

	if (eTrainUnit != NO_UNIT)
	{
		pPopupInfo->setData2(ORDER_TRAIN);
		pPopupInfo->setData3(eTrainUnit);
	}
	else if (eConstructBuilding != NO_BUILDING)
	{
		pPopupInfo->setData2(ORDER_CONSTRUCT);
		pPopupInfo->setData3(eConstructBuilding);
	}
	else
	{
		pPopupInfo->setData2(NO_ORDER);
		pPopupInfo->setData3(NO_UNIT);
	}

	gDLL->getInterfaceIFace()->addPopup(pPopupInfo, getOwnerINLINE(), false, bFront);
}


int CvCity::getCityPlotIndex(const CvPlot* pPlot) const
{
	return plotCityXY(this, pPlot);
}


CvPlot* CvCity::getCityIndexPlot(int iIndex) const
{
	return plotCity(getX_INLINE(), getY_INLINE(), iIndex);
}


bool CvCity::canWork(const CvPlot* pPlot) const
{
	if (pPlot == NULL)
	{
		return false;
	}

	if (pPlot->getWorkingCity() != this)
	{
		return false;
	}

	FAssertMsg(getCityPlotIndex(pPlot) != -1, "getCityPlotIndex(pPlot) is expected to be assigned (not -1)");

	if (pPlot->plotCheck(PUF_canSiege, getOwnerINLINE()) != NULL)
	{
		return false;
	}

	if (pPlot->isWater())
	{
		if (!isWorksWater() && !GC.getCivilizationInfo(getCivilizationType()).isWaterWorks())
		{
			return false;
		}
	}

	if (!(pPlot->hasYield()))
	{
		return false;
	}

	return true;
}


void CvCity::verifyWorkingPlot(int iPlotIndex)
{
	FAssert(iPlotIndex >= 0 && iPlotIndex < NUM_CITY_PLOTS);

	CvUnit* pUnit = getUnitWorkingPlot(iPlotIndex);
	if (pUnit != NULL)
	{
		CvPlot* pPlot = getCityIndexPlot(iPlotIndex);

		if (pPlot != NULL)
		{
			bool bCanWork = canWork(pPlot);

			if (!bCanWork)
			{
				pUnit->setColonistLocked(false);
			}

			//check if we have outdoor profession
			if (!bCanWork || NO_PROFESSION == pUnit->getProfession() || !GC.getProfessionInfo(pUnit->getProfession()).isWorkPlot())
			{
				clearUnitWorkingPlot(iPlotIndex);
				AI_setAssignWorkDirty(true);
			}
		}
	}
}


void CvCity::verifyWorkingPlots()
{
	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		verifyWorkingPlot(iI);
	}
}

int CvCity::getNumAvailableWorkPlots() const
{
	int iNumCanWorkPlots = 0;

	for (int iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
	{
		if (iJ != CITY_HOME_PLOT)
		{
			if (!isUnitWorkingPlot(iJ))
			{
				CvPlot* pLoopPlot = getCityIndexPlot(iJ);

				if (pLoopPlot != NULL)
				{
					if (canWork(pLoopPlot))
					{
						++iNumCanWorkPlots;
					}
				}
			}
		}
	}

	return iNumCanWorkPlots;
}

void CvCity::clearWorkingOverride(int iIndex)
{
	CvPlot* pPlot;

	pPlot = getCityIndexPlot(iIndex);

	if (pPlot != NULL)
	{
		pPlot->setWorkingCityOverride(NULL);
	}
}


int CvCity::countNumImprovedPlots(ImprovementTypes eImprovement, bool bPotential) const
{
	CvPlot* pLoopPlot;
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		pLoopPlot = getCityIndexPlot(iI);

		if (pLoopPlot != NULL)
		{
			if (pLoopPlot->getWorkingCity() == this)
			{
				if (eImprovement != NO_IMPROVEMENT)
				{
					if (pLoopPlot->getImprovementType() == eImprovement ||
						(bPotential && pLoopPlot->canHaveImprovement(eImprovement, getTeam())))
					{
						++iCount;
					}
				}
				else if (pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
				{
					iCount++;
				}
			}
		}
	}

	return iCount;
}


int CvCity::countNumWaterPlots() const
{
	CvPlot* pLoopPlot;
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		pLoopPlot = getCityIndexPlot(iI);

		if (pLoopPlot != NULL)
		{
			if (pLoopPlot->getWorkingCity() == this)
			{
				if (pLoopPlot->isWater())
				{
					iCount++;
				}
			}
		}
	}

	return iCount;
}

int CvCity::countNumRiverPlots() const
{
	int iCount = 0;

	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		CvPlot* pLoopPlot = getCityIndexPlot(iI);

		if (pLoopPlot != NULL)
		{
			if (pLoopPlot->getWorkingCity() == this)
			{
				if (pLoopPlot->isRiver())
				{
					++iCount;
				}
			}
		}
	}

	return iCount;
}

int CvCity::getNumProfessionBuildingSlots(ProfessionTypes eProfession) const
{
	FAssert(!GC.getProfessionInfo(eProfession).isWorkPlot());
	CvProfessionInfo& kProfessionInfo = GC.getProfessionInfo(eProfession);
	SpecialBuildingTypes eSpecialBuilding = (SpecialBuildingTypes) kProfessionInfo.getSpecialBuilding();
	if (eSpecialBuilding == NO_SPECIALBUILDING)
	{
		return 0;
	}

	int iTotalSlots = 0;
	for (int i = 0; i < GC.getNumBuildingInfos(); ++i)
	{
		BuildingTypes eBuilding = (BuildingTypes) i;
		CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
		if (kBuilding.getSpecialBuildingType() == eSpecialBuilding)
		{
			if (isHasBuilding(eBuilding))
			{
				iTotalSlots += kBuilding.getMaxWorkers();
			}
		}
	}

	return iTotalSlots;
}

bool CvCity::isAvailableProfessionSlot(ProfessionTypes eProfession, const CvUnit* pUnit) const
{
	if (GC.getProfessionInfo(eProfession).isCitizen())
	{
		if (GC.getProfessionInfo(eProfession).isWorkPlot())
		{
			if (!isUnitWorkingAnyPlot(pUnit) && getNumAvailableWorkPlots() == 0)
			{
				return false;
			}
		}
		else
		{
			//Androrc Multiple Professions per Building
			/*
			int iSlots = getNumProfessionBuildingSlots(eProfession);

			for (uint i = 0; i < m_aPopulationUnits.size() && iSlots > 0; ++i)
			{
				CvUnit* pLoopUnit = m_aPopulationUnits[i];
				if (pUnit != pLoopUnit)
				{
					if (pLoopUnit->getProfession() == eProfession)
					{
						--iSlots;
					}
				}
			}

			if (iSlots <= 0)
			{
				FAssert(iSlots == 0);
				return false;
			}
			*/
			bool bHasBuilding = false;
			for (int i = 0; i < GC.getNumBuildingInfos(); ++i)
			{
				BuildingTypes eBuilding = (BuildingTypes) i;
				if (GC.getBuildingInfo(eBuilding).getSpecialBuildingType() == GC.getProfessionInfo(eProfession).getSpecialBuilding() && isHasBuilding(eBuilding))
				{
					bHasBuilding = true;
					if (!isAvailableBuildingSlot(eBuilding, pUnit))
					{
						return false;
					}
				}
			}
			if (!bHasBuilding)
			{
				return false;
			}
			//Androrc End
		}
	}

	return true;
}

//Androrc Multiple Professions per Building
//get the available slots per building, instead of per profession
bool CvCity::isAvailableBuildingSlot(BuildingTypes eBuilding, const CvUnit* pUnit) const
{
	int iSlots = 0;

	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	if (isHasBuilding(eBuilding))
	{
		iSlots += kBuilding.getMaxWorkers();
	}

	for (uint i = 0; i < m_aPopulationUnits.size() && iSlots > 0; ++i)
	{
		CvUnit* pLoopUnit = m_aPopulationUnits[i];
		if (pUnit != pLoopUnit)
		{
			if (pLoopUnit->getProfession() != NO_PROFESSION && GC.getProfessionInfo(pLoopUnit->getProfession()).getSpecialBuilding() == kBuilding.getSpecialBuildingType())
			{
				--iSlots;
			}
		}
	}

	if (iSlots <= 0)
	{
		FAssert(iSlots == 0);
		return false;
	}

	return true;
}
//Androrc End

int CvCity::professionCount(ProfessionTypes eProfession) const
{
	int iCount = 0;
	for (uint i = 0; i < m_aPopulationUnits.size(); ++i)
	{
		CvUnit* pLoopUnit = m_aPopulationUnits[i];
		if (pLoopUnit->getProfession() == eProfession)
		{
			++iCount;
		}
	}
	return iCount;
}

int CvCity::findPopulationRank() const
{
	if (!m_bPopulationRankValid)
	{
		int iRank = 1;

		int iLoop;
		CvCity* pLoopCity;
		for (pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop))
		{
			if ((pLoopCity->getPopulation() > getPopulation()) ||
				((pLoopCity->getPopulation() == getPopulation()) && (pLoopCity->getID() < getID())))
			{
				iRank++;
			}
		}

		// shenanigans are to get around the const check
		m_bPopulationRankValid = true;
		m_iPopulationRank = iRank;
	}

	return m_iPopulationRank;
}


int CvCity::findBaseYieldRateRank(YieldTypes eYield) const
{
	if (!m_abBaseYieldRankValid[eYield])
	{
		int iRate = getYieldRate(eYield);

		int iRank = 1;

		int iLoop;
		CvCity* pLoopCity;
		for (pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop))
		{
			int iYieldRate = pLoopCity->getYieldRate(eYield);
			if ((iYieldRate > iRate) || ((iYieldRate == iRate) && (pLoopCity->getID() < getID())))
			{
				iRank++;
			}
		}

		m_abBaseYieldRankValid[eYield] = true;
		m_aiBaseYieldRank[eYield] = iRank;
	}

	return m_aiBaseYieldRank[eYield];
}


int CvCity::findYieldRateRank(YieldTypes eYield) const
{
	if (!m_abYieldRankValid[eYield])
	{
		int iRate = getYieldRate(eYield);

		int iRank = 1;

		int iLoop;
		CvCity* pLoopCity;
		for (pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop))
		{
			if ((pLoopCity->getYieldRate(eYield) > iRate) ||
				((pLoopCity->getYieldRate(eYield) == iRate) && (pLoopCity->getID() < getID())))
			{
				iRank++;
			}
		}

		m_abYieldRankValid[eYield] = true;
		m_aiYieldRank[eYield] = iRank;
	}

	return m_aiYieldRank[eYield];
}


// Returns one of the upgrades...
UnitTypes CvCity::allUpgradesAvailable(UnitTypes eUnit, int iUpgradeCount) const
{
	UnitTypes eUpgradeUnit;
	UnitTypes eTempUnit;
	UnitTypes eLoopUnit;
	bool bUpgradeFound;
	bool bUpgradeAvailable;
	bool bUpgradeUnavailable;
	int iI;

	FAssertMsg(eUnit != NO_UNIT, "eUnit is expected to be assigned (not NO_UNIT)");

	if (iUpgradeCount > GC.getNumUnitClassInfos())
	{
		return NO_UNIT;
	}

	eUpgradeUnit = NO_UNIT;

	bUpgradeFound = false;
	bUpgradeAvailable = false;
	bUpgradeUnavailable = false;

	for (iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		if (GC.getUnitInfo(eUnit).getUpgradeUnitClass(iI))
		{
			eLoopUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI);

			if (eLoopUnit != NO_UNIT)
			{
				bUpgradeFound = true;

				eTempUnit = allUpgradesAvailable(eLoopUnit, (iUpgradeCount + 1));

				if (eTempUnit != NO_UNIT)
				{
					eUpgradeUnit = eTempUnit;
					bUpgradeAvailable = true;
				}
				else
				{
					bUpgradeUnavailable = true;
				}
			}
		}
	}

	if (iUpgradeCount > 0)
	{
		if (bUpgradeFound && bUpgradeAvailable)
		{
			FAssertMsg(eUpgradeUnit != NO_UNIT, "eUpgradeUnit is expected to be assigned (not NO_UNIT)");
			return eUpgradeUnit;
		}

		if (canTrain(eUnit, false, false, false, true))
		{
			return eUnit;
		}
	}
	else
	{
		if (bUpgradeFound && !bUpgradeUnavailable)
		{
			return eUpgradeUnit;
		}
	}

	return NO_UNIT;
}

bool CvCity::canTrain(UnitTypes eUnit, bool bContinue, bool bTestVisible, bool bIgnoreCost, bool bIgnoreUpgrades) const
{
	if (eUnit == NO_UNIT)
	{
		return false;
	}

	if(GC.getUSE_CAN_TRAIN_CALLBACK())
	{
		CyCity* pyCity = new CyCity((CvCity*)this);
		CyArgsList argsList;
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
		argsList.add(eUnit);
		argsList.add(bContinue);
		argsList.add(bTestVisible);
		argsList.add(bIgnoreCost);
		argsList.add(bIgnoreUpgrades);
		long lResult=0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "canTrain", argsList.makeFunctionArgs(), &lResult);
		delete pyCity;	// python fxn must not hold on to this pointer
		if (lResult == 1)
		{
			return true;
		}
	}

	if (!(GET_PLAYER(getOwnerINLINE()).canTrain(eUnit, bContinue, bTestVisible, bIgnoreCost)))
	{
		return false;
	}

	if (!bIgnoreUpgrades)
	{
		if (allUpgradesAvailable(eUnit) != NO_UNIT)
		{
			return false;
		}
	}

	if (!plot()->canTrain(eUnit, bContinue, bTestVisible))
	{
		return false;
	}

	if(GC.getUSE_CANNOT_TRAIN_CALLBACK())
	{
		CyCity *pyCity = new CyCity((CvCity*)this);
		CyArgsList argsList2; // XXX
		argsList2.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
		argsList2.add(eUnit);
		argsList2.add(bContinue);
		argsList2.add(bTestVisible);
		argsList2.add(bIgnoreCost);
		argsList2.add(bIgnoreUpgrades);
		long lResult=0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "cannotTrain", argsList2.makeFunctionArgs(), &lResult);
		delete pyCity;	// python fxn must not hold on to this pointer
		if (lResult == 1)
		{
			return false;
		}
	}

	return true;
}

bool CvCity::canTrain(UnitCombatTypes eUnitCombat) const
{
	for (int i = 0; i < GC.getNumUnitClassInfos(); i++)
	{
		UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(GET_PLAYER(getOwnerINLINE()).getCivilizationType()).getCivilizationUnits(i);

		if (NO_UNIT != eUnit)
		{
			if (GC.getUnitInfo(eUnit).getUnitCombatType() == eUnitCombat)
			{
				if (canTrain(eUnit))
				{
					return true;
				}
			}
		}
	}

	return false;
}


bool CvCity::canConstruct(BuildingTypes eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost) const
{
	if (eBuilding == NO_BUILDING)
	{
		return false;
	}

	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	if(GC.getUSE_CAN_CONSTRUCT_CALLBACK())
	{
		CyCity* pyCity = new CyCity((CvCity*)this);
		CyArgsList argsList;
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
		argsList.add(eBuilding);
		argsList.add(bContinue);
		argsList.add(bTestVisible);
		argsList.add(bIgnoreCost);
		long lResult=0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "canConstruct", argsList.makeFunctionArgs(), &lResult);
		delete pyCity;	// python fxn must not hold on to this pointer
		if (lResult == 1)
		{
			return true;
		}
	}

	// TAC - AI Buildings - koma13 - START
	if (!isHuman())
	{
		int iMinSize = kBuilding.getAICitySize();

		if (iMinSize < 0)
		{
			return false;
		}

		if (iMinSize > getPopulation())
		{
			return false;
		}
		if (!isNative() && kBuilding.isWater())
		{
			if (isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()) && !plot()->isEuropeAccessable())
			{
				for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
				{
					UnitTypes eLoopUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI)));
					if (eLoopUnit != NO_UNIT)
					{
						CvUnitInfo& kUnit = GC.getUnitInfo(eLoopUnit);
						if (kUnit.getDomainType() == DOMAIN_SEA)
						{
							if ((kUnit.getPrereqBuilding() == kBuilding.getBuildingClassType()) || kUnit.isPrereqOrBuilding(kBuilding.getBuildingClassType()))
							{
								return false;
							}
						}
					}
				}
			}
		}
	}
	// TAC - AI Buildings - koma13 - END

	if (!(GET_PLAYER(getOwnerINLINE()).canConstruct(eBuilding, bContinue, bTestVisible, bIgnoreCost)))
	{
		return false;
	}

	if (kBuilding.isGraphicalOnly())
	{
		return false;
	}

	if (isHasConceptualBuilding(eBuilding))
	{
		return false;
	}

	if (!isDominantSpecialBuilding(eBuilding))
	{
		return false;
	}

	if (!isValidBuildingLocation(eBuilding))
	{
		return false;
	}

	if (plot()->getLatitude() > kBuilding.getMaxLatitude())
	{
		return false;
	}

	if (plot()->getLatitude() < kBuilding.getMinLatitude())
	{
		return false;
	}

	for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		if (kBuilding.isBuildingClassNeededInCity(iI))
		{
			BuildingTypes ePrereqBuilding = ((BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(iI)));

			if (ePrereqBuilding != NO_BUILDING)
			{
				if (!isHasConceptualBuilding(ePrereqBuilding))
				{
					return false;
				}
			}
		}
	}

	if (!bTestVisible)
	{
		if (!bContinue)
		{
			if (getFirstBuildingOrder(eBuilding) != -1)
			{
				return false;
			}
		}
	}

	if(GC.getUSE_CANNOT_CONSTRUCT_CALLBACK())
	{
		CyCity *pyCity = new CyCity((CvCity*)this);
		CyArgsList argsList2; // XXX
		argsList2.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
		argsList2.add(eBuilding);
		argsList2.add(bContinue);
		argsList2.add(bTestVisible);
		argsList2.add(bIgnoreCost);
		long lResult=0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "cannotConstruct", argsList2.makeFunctionArgs(), &lResult);
		delete pyCity;	// python fxn must not hold on to this pointer
		if (lResult == 1)
		{
			return false;
		}
	}
	
	return true;
}

bool CvCity::canConvince(FatherPointTypes eFatherPoint, bool bContinue, bool bTestVisible) const
{
	if (isNative())
	{
		return false;
	}

	if (GC.getFatherPointInfo(eFatherPoint).getProductionConversionPoints() <= 0)
	{
		return false;
	}

	std::vector<FatherTypes> aFathers;
	GC.getGameINLINE().getRemainingFathers(eFatherPoint, aFathers);
	if (aFathers.empty())
	{
		return false;
	}

	return true;
}


int CvCity::getFoodTurnsLeft() const
{
	int iFoodLeft;
	int iTurnsLeft;

	iFoodLeft = (growthThreshold() - getFood());

	if (foodDifference() <= 0)
	{
		return iFoodLeft;
	}

	iTurnsLeft = (iFoodLeft / foodDifference());

	if ((iTurnsLeft * foodDifference()) <  iFoodLeft)
	{
		iTurnsLeft++;
	}

	return std::max(1, iTurnsLeft);
}

bool CvCity::isProduction() const
{
	return (headOrderQueueNode() != NULL);
}

bool CvCity::isProductionUnit() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		return (pOrderNode->m_data.eOrderType == ORDER_TRAIN);
	}

	return false;
}


bool CvCity::isProductionBuilding() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		return (pOrderNode->m_data.eOrderType == ORDER_CONSTRUCT);
	}

	return false;
}

bool CvCity::isProductionConvince() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		return (pOrderNode->m_data.eOrderType == ORDER_CONVINCE);
	}

	return false;
}


bool CvCity::canContinueProduction(OrderData order) const
{
	switch (order.eOrderType)
	{
	case ORDER_TRAIN:
		return canTrain((UnitTypes)(order.iData1), true);
		break;

	case ORDER_CONSTRUCT:
		return canConstruct((BuildingTypes)(order.iData1), true);
		break;

	case ORDER_CONVINCE:
		return canConvince((FatherPointTypes) order.iData1, true);
		break;

	default:
		FAssertMsg(false, "order.eOrderType failed to match a valid option");
		break;
	}

	return false;
}


int CvCity::getProductionExperience(UnitTypes eUnit) const
{
	int iExperience;

	iExperience = getFreeExperience();
	iExperience += GET_PLAYER(getOwnerINLINE()).getFreeExperience();

	if (eUnit != NO_UNIT)
	{
		if (GC.getUnitInfo(eUnit).getUnitCombatType() != NO_UNITCOMBAT)
		{
			iExperience += getUnitCombatFreeExperience((UnitCombatTypes)(GC.getUnitInfo(eUnit).getUnitCombatType()));
		}
		iExperience += getDomainFreeExperience((DomainTypes)(GC.getUnitInfo(eUnit).getDomainType()));
	}

	return std::max(0, iExperience);
}


void CvCity::addProductionExperience(CvUnit* pUnit)
{
	if (pUnit->canAcquirePromotionAny())
	{
		pUnit->changeExperience(getProductionExperience(pUnit->getUnitType()));
	}

	for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		if (isFreePromotion((PromotionTypes)iI))
		{
			if ((pUnit->getUnitCombatType() != NO_UNITCOMBAT) && GC.getPromotionInfo((PromotionTypes)iI).getUnitCombat(pUnit->getUnitCombatType()))
			{
				pUnit->setHasRealPromotion(((PromotionTypes)iI), true);
			}
		}
	}

	pUnit->testPromotionReady();
}


UnitTypes CvCity::getProductionUnit() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return ((UnitTypes)(pOrderNode->m_data.iData1));
			break;

		case ORDER_CONSTRUCT:
		case ORDER_CONVINCE:
			break;

		default:
			FAssertMsg(false, "pOrderNode->m_data.eOrderType failed to match a valid option");
			break;
		}
	}

	return NO_UNIT;
}


UnitAITypes CvCity::getProductionUnitAI() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return ((UnitAITypes)(pOrderNode->m_data.iData2));
			break;

		case ORDER_CONSTRUCT:
		case ORDER_CONVINCE:
			break;

		default:
			FAssertMsg(false, "pOrderNode->m_data.eOrderType failed to match a valid option");
			break;
		}
	}

	return NO_UNITAI;
}


BuildingTypes CvCity::getProductionBuilding() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
		case ORDER_CONVINCE:
			break;

		case ORDER_CONSTRUCT:
			return ((BuildingTypes)(pOrderNode->m_data.iData1));
			break;

		default:
			FAssertMsg(false, "pOrderNode->m_data.eOrderType failed to match a valid option");
			break;
		}
	}

	return NO_BUILDING;
}

FatherPointTypes CvCity::getProductionFatherPoint() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
		case ORDER_CONSTRUCT:
			break;

		case ORDER_CONVINCE:
			return ((FatherPointTypes)(pOrderNode->m_data.iData1));
			break;

		default:
			FAssertMsg(false, "pOrderNode->m_data.eOrderType failed to match a valid option");
			break;
		}
	}

	return NO_FATHER_POINT_TYPE;
}

const CvInfoBase* CvCity::getProductionInfo() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return &GC.getUnitInfo((UnitTypes) pOrderNode->m_data.iData1);
			break;

		case ORDER_CONSTRUCT:
			return &GC.getBuildingInfo((BuildingTypes) pOrderNode->m_data.iData1);
			break;

		case ORDER_CONVINCE: // Lumber Mill is building Founding Father Points -- DO NOT TOUCH
			return &GC.getFatherPointInfo((FatherPointTypes) pOrderNode->m_data.iData1);
			break;

		default:
			FAssertMsg(false, "pOrderNode->m_data.eOrderType failed to match a valid option");
			break;
		}
	}

	return NULL;
}


const wchar* CvCity::getProductionName() const
{
	const CvInfoBase* pInfo = getProductionInfo();
	if (pInfo != NULL)
	{
		return pInfo->getDescription();
	}

	return L"";
}


int CvCity::getGeneralProductionTurnsLeft() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return getProductionTurnsLeft((UnitTypes)pOrderNode->m_data.iData1, 0);
			break;

		case ORDER_CONSTRUCT:
			return getProductionTurnsLeft((BuildingTypes)pOrderNode->m_data.iData1, 0);
			break;

		case ORDER_CONVINCE:
			return 0;
			break;

		default:
			FAssertMsg(false, "pOrderNode->m_data.eOrderType failed to match a valid option");
			break;
		}
	}

	return 0;
}

const wchar* CvCity::getProductionNameKey() const
{
	const CvInfoBase* pInfo = getProductionInfo();
	if (pInfo != NULL)
	{
		return pInfo->getTextKeyWide();
	}

	return L"";
}

int CvCity::getFirstUnitOrder(UnitTypes eUnit) const
{
	int iCount = 0;

	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	while (pOrderNode != NULL)
	{
		if (pOrderNode->m_data.eOrderType == ORDER_TRAIN)
		{
			if (pOrderNode->m_data.iData1 == eUnit)
			{
				return iCount;
			}
		}

		iCount++;

		pOrderNode = nextOrderQueueNode(pOrderNode);
	}

	return -1;
}


int CvCity::getFirstBuildingOrder(BuildingTypes eBuilding) const
{
	int iCount = 0;

	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	while (pOrderNode != NULL)
	{
		if (pOrderNode->m_data.eOrderType == ORDER_CONSTRUCT)
		{
			if (pOrderNode->m_data.iData1 == eBuilding)
			{
				return iCount;
			}
		}

		iCount++;

		pOrderNode = nextOrderQueueNode(pOrderNode);
	}

	return -1;
}

int CvCity::getNumTrainUnitAI(UnitAITypes eUnitAI) const
{
	int iCount = 0;

	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	while (pOrderNode != NULL)
	{
		if (pOrderNode->m_data.eOrderType == ORDER_TRAIN)
		{
			if (pOrderNode->m_data.iData2 == eUnitAI)
			{
				iCount++;
			}
		}

		pOrderNode = nextOrderQueueNode(pOrderNode);
	}

	return iCount;
}


int CvCity::getProduction() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return getUnitProduction((UnitTypes)(pOrderNode->m_data.iData1));
			break;

		case ORDER_CONSTRUCT:
			return getBuildingProduction((BuildingTypes)(pOrderNode->m_data.iData1));
			break;

		case ORDER_CONVINCE:
			break;

		default:
			FAssertMsg(false, "pOrderNode->m_data.eOrderType failed to match a valid option");
			break;
		}
	}

	return 0;
}


int CvCity::getProductionNeededUncached(YieldTypes eYield) const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return getYieldProductionNeeded((UnitTypes)pOrderNode->m_data.iData1, eYield);
			break;

		case ORDER_CONSTRUCT:
			return getYieldProductionNeeded((BuildingTypes)pOrderNode->m_data.iData1, eYield);
			break;

		case ORDER_CONVINCE:
			if (GC.getYieldInfo(eYield).isCargo())
			{
				return 0;
			}
			else
			{
				return MAX_INT;
			}
			break;

		default:
			FAssertMsg(false, "pOrderNode->m_data.eOrderType failed to match a valid option");
			break;
		}
	}

	return MAX_INT;
}

int CvCity::getYieldProductionNeeded(UnitTypes eUnit, YieldTypes eYield) const
{
	return GET_PLAYER(getOwnerINLINE()).getYieldProductionNeeded(eUnit, eYield);
}

int CvCity::getYieldProductionNeeded(BuildingTypes eBuilding, YieldTypes eYield) const
{
	int iProductionNeeded = GET_PLAYER(getOwnerINLINE()).getYieldProductionNeeded(eBuilding, eYield);

	// Python cost modifier
	if (GC.getUSE_GET_BUILDING_COST_MOD_CALLBACK())
	{
		CyArgsList argsList;
		argsList.add(getOwnerINLINE());	// Player ID
		argsList.add(getID());	// City ID
		argsList.add(eBuilding);	// Building ID
		argsList.add(eYield);
		long lResult=0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "getBuildingCostMod", argsList.makeFunctionArgs(), &lResult);

		if (lResult > 1)
		{
			iProductionNeeded *= lResult;
			iProductionNeeded /= 100;
		}
	}

	return iProductionNeeded;
}

int CvCity::getProductionTurnsLeft() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return getProductionTurnsLeft(((UnitTypes)(pOrderNode->m_data.iData1)), 0);
			break;

		case ORDER_CONSTRUCT:
			return getProductionTurnsLeft(((BuildingTypes)(pOrderNode->m_data.iData1)), 0);
			break;

		case ORDER_CONVINCE:
			break;

		default:
			FAssertMsg(false, "pOrderNode->m_data.eOrderType failed to match a valid option");
			break;
		}
	}

	return MAX_INT;
}


int CvCity::getProductionTurnsLeft(UnitTypes eUnit, int iNum) const
{
	int iProduction = 0;

	int iFirstUnitOrder = getFirstUnitOrder(eUnit);

	if ((iFirstUnitOrder == -1) || (iFirstUnitOrder == iNum))
	{
		iProduction += getUnitProduction(eUnit);
	}

	int iProductionNeeded = getYieldProductionNeeded(eUnit, YIELD_HAMMERS);
	int iProductionModifier = getProductionModifier(eUnit);

	return getProductionTurnsLeft(iProductionNeeded, iProduction, getProductionDifference(iProductionModifier, (iNum == 0), false), getProductionDifference(iProductionModifier, false, false));
}


int CvCity::getProductionTurnsLeft(BuildingTypes eBuilding, int iNum) const
{
	int iProduction;
	int iFirstBuildingOrder;
	int iProductionNeeded;
	int iProductionModifier;

	iProduction = 0;

	iFirstBuildingOrder = getFirstBuildingOrder(eBuilding);

	if ((iFirstBuildingOrder == -1) || (iFirstBuildingOrder == iNum))
	{
		iProduction += getBuildingProduction(eBuilding);
	}

	iProductionNeeded = getYieldProductionNeeded(eBuilding, YIELD_HAMMERS);

	iProductionModifier = getProductionModifier(eBuilding);

	return getProductionTurnsLeft(iProductionNeeded, iProduction, getProductionDifference(iProductionModifier, (iNum == 0), false), getProductionDifference(iProductionModifier, false, false));
}


int CvCity::getProductionTurnsLeft(int iProductionNeeded, int iProduction, int iFirstProductionDifference, int iProductionDifference) const
{
	int iProductionLeft;
	int iTurnsLeft;

	iProductionLeft = std::max(0, (iProductionNeeded - iProduction - iFirstProductionDifference));

	if (iProductionDifference == 0)
	{
		return iProductionLeft + 1;
	}

	iTurnsLeft = (iProductionLeft / iProductionDifference);

	if ((iTurnsLeft * iProductionDifference) < iProductionLeft)
	{
		iTurnsLeft++;
	}

	iTurnsLeft++;

	return std::max(1, iTurnsLeft);
}


void CvCity::setProduction(int iNewValue)
{
	if (isProductionUnit())
	{
		setUnitProduction(getProductionUnit(), iNewValue);
	}
	else if (isProductionBuilding())
	{
		setBuildingProduction(getProductionBuilding(), iNewValue);
	}
}


void CvCity::changeProduction(int iChange)
{
	if (isProductionUnit())
	{
		changeUnitProduction(getProductionUnit(), iChange);
	}
	else if (isProductionBuilding())
	{
		changeBuildingProduction(getProductionBuilding(), iChange);
	}
}


int CvCity::getProductionModifier() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return getProductionModifier((UnitTypes)(pOrderNode->m_data.iData1));
			break;

		case ORDER_CONSTRUCT:
			return getProductionModifier((BuildingTypes)(pOrderNode->m_data.iData1));
			break;

		case ORDER_CONVINCE:
			break;

		default:
			FAssertMsg(false, "pOrderNode->m_data.eOrderType failed to match a valid option");
			break;
		}
	}

	return 0;
}


int CvCity::getProductionModifier(UnitTypes eUnit) const
{
	int iMultiplier = GET_PLAYER(getOwnerINLINE()).getProductionModifier(eUnit);

	iMultiplier += getDomainProductionModifier((DomainTypes)(GC.getUnitInfo(eUnit).getDomainType()));

	if (GC.getUnitInfo(eUnit).isMilitaryProduction())
	{
		iMultiplier += getMilitaryProductionModifier();
	}

	return std::max(0, iMultiplier);
}


int CvCity::getProductionModifier(BuildingTypes eBuilding) const
{
	int iMultiplier = GET_PLAYER(getOwnerINLINE()).getProductionModifier(eBuilding);

	return std::max(0, iMultiplier);
}


int CvCity::getProductionDifference(int iProductionModifier, bool bOverflow, bool bUseStoredHammers) const
{
	if (isDisorder())
	{
		return 0;
	}

	int iNetYield = (bUseStoredHammers ? getYieldStored(YIELD_HAMMERS) : calculateNetYield(YIELD_HAMMERS));
	if (bOverflow)
	{
		iNetYield += getOverflowProduction();
	}

	return (iNetYield * (100 + iProductionModifier) / 100);
}


int CvCity::getCurrentProductionDifference(bool bOverflow) const
{
	return getProductionDifference(getProductionModifier(), !isProductionConvince() && bOverflow, false);
}

int CvCity::getStoredProductionDifference() const
{
	return getProductionDifference(getProductionModifier(), !isProductionConvince(), true);
}

int CvCity::getExtraProductionDifference(int iExtra, int iModifier) const
{
	return ((iExtra * (getBaseYieldRateModifier(YIELD_HAMMERS) + iModifier)) / 100);
}

bool CvCity::canHurry(HurryTypes eHurry, bool bTestVisible) const
{
	if (!GC.getHurryInfo(eHurry).isCity())
	{
		return false;
	}

	if (!GET_PLAYER(getOwnerINLINE()).canHurry(eHurry, -1))
	{
		return false;
	}

	if (isDisorder())
	{
		return false;
	}

	if (getProduction() >= getProductionNeeded(YIELD_HAMMERS))
	{
		bool bYieldBuyable = false;
		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			YieldTypes eYield = (YieldTypes) iYield;
			if (GC.getYieldInfo(eYield).isCargo())
			{
				if (getHurryYieldDeficit(eHurry, eYield) > 0)
				{
					if (GET_PLAYER(getOwnerINLINE()).getParent() != NO_PLAYER && GET_PLAYER(getOwnerINLINE()).isYieldEuropeTradable(eYield))
					{
						bYieldBuyable = true;
						break;
					}
				}
			}
		}
		if (!bYieldBuyable)
		{
			return false;
		}
	}

	if (!bTestVisible)
	{
		if (!isProductionUnit() && !isProductionBuilding())
		{
			return false;
		}

		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			YieldTypes eYield = (YieldTypes) iYield;
			if (GC.getYieldInfo(eYield).isCargo())
			{
				if (getHurryYieldDeficit(eHurry, eYield) > 0)
				{
					if (GET_PLAYER(getOwnerINLINE()).getParent() == NO_PLAYER || !GET_PLAYER(getOwnerINLINE()).isYieldEuropeTradable(eYield))
					{
						return false;
					}
				}
			}
		}

		if (GET_PLAYER(getOwnerINLINE()).getGold() < hurryGold(eHurry))
		{
			return false;
		}

		if (maxHurryPopulation() < hurryPopulation(eHurry))
		{
			return false;
		}
		// TAC - AI Military Buildup - koma13 - START
		CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());
		if (!kPlayer.isHuman())
		{
			if (kPlayer.AI_isStrategy(STRATEGY_MILITARY_BUILDUP))
			{
				if (getProductionBuilding() != NO_BUILDING)
				{
					return false;
				}

				UnitTypes eProductionUnit = getProductionUnit();
				if (eProductionUnit != NO_UNIT)
				{
					UnitAITypes eDefaultUnitAI = (UnitAITypes)GC.getUnitInfo(eProductionUnit).getDefaultUnitAIType();
				
					if ((eDefaultUnitAI != kPlayer.AI_bestBuildupUnitAI()) && (eDefaultUnitAI != UNITAI_WAGON))
					{
						return false;
					}
				}
			}
		}
		// TAC - AI Military Buildup - koma13 - END
	}

	return true;
}

void CvCity::hurry(HurryTypes eHurry)
{
	if (!canHurry(eHurry))
	{
		return;
	}

	int iHurryGold = hurryGold(eHurry);
	int iHurryPopulation = hurryPopulation(eHurry);

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		YieldTypes eYield = (YieldTypes) iYield;
		changeYieldRushed(eYield, getHurryYieldDeficit(eHurry, eYield));

		if (GC.getHurryInfo(eHurry).getProductionYieldConsumed() == eYield)
		{
			int iYieldConsumed = hurryCost(false, true) * GC.getHurryInfo(eHurry).getProductionYieldPercent() / 100;
			int iRushed = std::min(iYieldConsumed, getYieldRushed(eYield));
			changeYieldRushed(eYield, -iRushed);
			changeYieldStored(eYield, iRushed - iYieldConsumed);
		}
	}

	changeProduction(hurryProduction(eHurry));

	// TAC - AI Military Buildup - koma13 - START
	CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());
	if (kPlayer.AI_isStrategy(STRATEGY_MILITARY_BUILDUP))
	{
		UnitTypes eProductionUnit = getProductionUnit();
		if (eProductionUnit != NO_UNIT)
		{
			UnitAITypes eDefaultUnitAI = (UnitAITypes)GC.getUnitInfo(eProductionUnit).getDefaultUnitAIType();
		
			if (eDefaultUnitAI == kPlayer.AI_bestBuildupUnitAI())
			{
				kPlayer.AI_clearStrategy(STRATEGY_MILITARY_BUILDUP);
			}
		}
	}
	// TAC - AI Military Buildup - koma13 - END
	GET_PLAYER(getOwnerINLINE()).changeGold(-iHurryGold);
	GET_PLAYER(getOwnerINLINE()).AI_changeHurrySpending(iHurryGold);
	changePopulation(-iHurryPopulation);

	if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && isCitySelected())
	{
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(ResourceTable_DIRTY_BIT, true);
	}

	// Python Event
	gDLL->getEventReporterIFace()->cityHurry(this, eHurry);
}


void CvCity::processBuilding(BuildingTypes eBuilding, int iChange)
{
	FAssertMsg(iChange == 1 || iChange == -1, "The value of iChange has to be either 1 or -1.")

	//RWL Railroads and Trainstations
	if (GC.getBuildingInfo(eBuilding).getSpecialBuildingType() == 26) // easiest way to identify Trainstation
	{
		CvPlot* pPlot = plot();
		pPlot->setRouteType((RouteTypes)2);
	}
	//RWL END Railroads and Trainstations

	// R&R, ray, Plastered Road
	if (GC.getBuildingInfo(eBuilding).getAICitySize() == 4) // easiest way to identify Townhall
	{
		CvPlot* pPlot = plot();
		if (pPlot->getRouteType() != 2)
		{
			pPlot->setRouteType((RouteTypes)1);
		}
	}
	// R&R, ray, Plastered Road -END

	if (GC.getBuildingInfo(eBuilding).getFreePromotion() != NO_PROMOTION)
	{
		changeFreePromotionCount(((PromotionTypes)(GC.getBuildingInfo(eBuilding).getFreePromotion())), iChange);
	}
	changeFreeExperience(GC.getBuildingInfo(eBuilding).getFreeExperience() * iChange);
	changeMaxFoodKeptPercent(GC.getBuildingInfo(eBuilding).getFoodKept() * iChange);
	changeHealRate(GC.getBuildingInfo(eBuilding).getHealRateChange() * iChange);
	changeMilitaryProductionModifier(GC.getBuildingInfo(eBuilding).getMilitaryProductionModifier() * iChange);
	changeWorksWaterCount((GC.getBuildingInfo(eBuilding).isWorksWater()) ? iChange : 0);
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		changeLandPlotYield(((YieldTypes)iI), (GC.getBuildingInfo(eBuilding).getLandPlotYieldChange(iI) * iChange)); // R&R, ray, Landplot Yields
		changeSeaPlotYield(((YieldTypes)iI), (GC.getBuildingInfo(eBuilding).getSeaPlotYieldChange(iI) * iChange));
		changeRiverPlotYield(((YieldTypes)iI), (GC.getBuildingInfo(eBuilding).getRiverPlotYieldChange(iI) * iChange));
		changeYieldRateModifier(((YieldTypes)iI), (GC.getBuildingInfo(eBuilding).getYieldModifier(iI) * iChange));
	}
	setYieldRateDirty();
	for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
	{
		changeUnitCombatFreeExperience(((UnitCombatTypes)iI), GC.getBuildingInfo(eBuilding).getUnitCombatFreeExperience(iI) * iChange);
	}
	for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		changeDomainFreeExperience(((DomainTypes)iI), GC.getBuildingInfo(eBuilding).getDomainFreeExperience(iI) * iChange);
		changeDomainProductionModifier(((DomainTypes)iI), GC.getBuildingInfo(eBuilding).getDomainProductionModifier(iI) * iChange);
	}
	GET_PLAYER(getOwnerINLINE()).changeAssets(GC.getBuildingInfo(eBuilding).getAssetValue() * iChange);
	area()->changePower(getOwnerINLINE(), (GC.getBuildingInfo(eBuilding).getPowerValue() * iChange));
	GET_PLAYER(getOwnerINLINE()).changePower(GC.getBuildingInfo(eBuilding).getPowerValue() * iChange);
	changeBuildingDefense(GC.getBuildingInfo(eBuilding).getDefenseModifier() * iChange);
	changeBuildingBombardDefense(GC.getBuildingInfo(eBuilding).getBombardDefenseModifier() * iChange);
	GET_TEAM(getTeam()).changeBuildingClassCount((BuildingClassTypes)GC.getBuildingInfo(eBuilding).getBuildingClassType(), iChange);
	GET_PLAYER(getOwnerINLINE()).changeBuildingClassCount((BuildingClassTypes)GC.getBuildingInfo(eBuilding).getBuildingClassType(), iChange);
	setLayoutDirty(true);
}

HandicapTypes CvCity::getHandicapType() const
{
	return GET_PLAYER(getOwnerINLINE()).getHandicapType();
}

CivilizationTypes CvCity::getCivilizationType() const
{
	return GET_PLAYER(getOwnerINLINE()).getCivilizationType();
}

LeaderHeadTypes CvCity::getPersonalityType() const
{
	return GET_PLAYER(getOwnerINLINE()).getPersonalityType();
}

ArtStyleTypes CvCity::getArtStyleType() const
{
	return GET_PLAYER(getOwnerINLINE()).getArtStyleType();
}

CitySizeTypes CvCity::getCitySizeType() const
{
	return ((CitySizeTypes)(range((getPopulation() / 7), 0, (NUM_CITYSIZE_TYPES - 1))));
}

bool CvCity::hasTrait(TraitTypes eTrait) const
{
	return GET_PLAYER(getOwnerINLINE()).hasTrait(eTrait);
}

bool CvCity::isHuman() const
{
	return GET_PLAYER(getOwnerINLINE()).isHuman();
}

bool CvCity::isNative() const
{
	return GET_PLAYER(getOwnerINLINE()).isNative();
}

bool CvCity::isVisible(TeamTypes eTeam, bool bDebug) const
{
	return plot()->isVisible(eTeam, bDebug);
}

bool CvCity::isCapital() const
{
	return (GET_PLAYER(getOwnerINLINE()).getCapitalCity() == this);
}

bool CvCity::isCoastal(int iMinWaterSize) const
{
	return plot()->isCoastalLand(iMinWaterSize);
}

bool CvCity::isDisorder() const
{
	return (isOccupation());
}

int CvCity::extraPopulation() const
{
	int iNumExtra = 0;
	for (uint i = 0; i < m_aPopulationUnits.size(); ++i)
	{
		CvUnit* pUnit = m_aPopulationUnits[i];
		ProfessionTypes eUnitProfession = pUnit->getProfession();
		if (eUnitProfession == NO_PROFESSION) //idle, needs a profession
		{
			iNumExtra++;
		}
		else if(GC.getProfessionInfo(eUnitProfession).isWorkPlot() && !isUnitWorkingAnyPlot(pUnit)) //needs to be working, but not working yet
		{
			iNumExtra++;
		}
	}

	return iNumExtra;
}

int CvCity::foodConsumption(int iExtra) const
{
	return ((getPopulation() + iExtra) * GC.getFOOD_CONSUMPTION_PER_POPULATION());
}

int CvCity::foodDifference() const
{
	if (isDisorder())
	{
		return 0;
	}

	return calculateNetYield(YIELD_FOOD);
}

int CvCity::growthThreshold() const
{
	return (GET_PLAYER(getOwnerINLINE()).getGrowthThreshold(getPopulation()) * (100 - getCityHealth() - getCityHappiness() + getCityUnHappiness()) / 100); // R&R, ray, Health // WTP, ray, Happiness - START
}

int CvCity::productionLeft() const
{
	return (getProductionNeeded(YIELD_HAMMERS) - getProduction());
}

int CvCity::getHurryCostModifier(bool bIgnoreNew) const
{
	int iModifier = 100;
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			iModifier = getHurryCostModifier((UnitTypes) pOrderNode->m_data.iData1, bIgnoreNew);
			break;

		case ORDER_CONSTRUCT:
			iModifier = getHurryCostModifier((BuildingTypes) pOrderNode->m_data.iData1, bIgnoreNew);
			break;

		case ORDER_CONVINCE:
			break;

		default:
			FAssertMsg(false, "pOrderNode->m_data.eOrderType did not match a valid option");
			break;
		}
	}

	return iModifier;
}

int CvCity::getHurryCostModifier(UnitTypes eUnit, bool bIgnoreNew) const
{
	return getHurryCostModifier(GC.getUnitInfo(eUnit).getHurryCostModifier(), getUnitProduction(eUnit), bIgnoreNew);
}

int CvCity::getHurryCostModifier(BuildingTypes eBuilding, bool bIgnoreNew) const
{
	return getHurryCostModifier(GC.getBuildingInfo(eBuilding).getHurryCostModifier(), getBuildingProduction(eBuilding), bIgnoreNew);
}

int CvCity::getHurryCostModifier(int iBaseModifier, int iProduction, bool bIgnoreNew) const
{
	int iModifier = 100;
	iModifier *= std::max(0, iBaseModifier + 100);
	iModifier /= 100;

	if (iProduction == 0 && !bIgnoreNew)
	{
		iModifier *= std::max(0, (GC.getDefineINT("NEW_HURRY_MODIFIER") + 100));
		iModifier /= 100;
	}

	return iModifier;
}


int CvCity::hurryCost(bool bExtra, bool bIgnoreNew) const
{
	return (getHurryCost(bExtra, productionLeft(), getHurryCostModifier(bIgnoreNew), getProductionModifier()));
}

int CvCity::getHurryCost(bool bExtra, int iProductionLeft, int iHurryModifier, int iModifier) const
{
	int iProduction = (iProductionLeft * iHurryModifier + 99) / 100; // round up

	if (bExtra)
	{
		int iExtraProduction = getExtraProductionDifference(iProduction, iModifier);
		if (iExtraProduction > 0)
		{
			int iAdjustedProd = iProduction * iProduction;
			
			// round up
			iProduction = (iAdjustedProd + (iExtraProduction - 1)) / iExtraProduction;
		}
	}

	return std::max(0, iProduction);
}

int CvCity::hurryGold(HurryTypes eHurry) const
{
	int iGold = hurryCost(false, false) * GC.getHurryInfo(eHurry).getGoldPerProduction();
	iGold += GC.getHurryInfo(eHurry).getFlatGold() * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent() / 100;

	PlayerTypes eParent = GET_PLAYER(getOwnerINLINE()).getParent();
	if (eParent != NO_PLAYER)
	{
		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			YieldTypes eYield = (YieldTypes) iYield;
			iGold += getHurryYieldDeficit(eHurry, eYield) * GET_PLAYER(eParent).getYieldSellPrice(eYield) * GC.getHurryInfo(eHurry).getYieldCostEuropePercent() / 100;
		}
	}
	
	if (!isHuman())
	{
		iGold *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIHurryPercent();
		iGold /= 100;
	}

	return std::max(0, iGold);
}

int CvCity::getHurryYieldDeficit(HurryTypes eHurry, YieldTypes eYield) const
{
	if (!GC.getYieldInfo(eYield).isCargo())
	{
		return 0;
	}

	return std::max(0, getHurryYieldNeeded(eHurry, eYield) - getYieldStored(eYield) - getYieldRushed(eYield));
}

int CvCity::getHurryYieldNeeded(HurryTypes eHurry, YieldTypes eYield) const
{
	if (!GC.getYieldInfo(eYield).isCargo())
	{
		return 0;
	}

	int iAmountNeeded = getProductionNeeded(eYield);
	if (GC.getHurryInfo(eHurry).getProductionYieldConsumed() == eYield)
	{
		iAmountNeeded += hurryCost(false, true) * GC.getHurryInfo(eHurry).getProductionYieldPercent() / 100;
	}

	return iAmountNeeded;
}


int CvCity::hurryPopulation(HurryTypes eHurry) const
{
	return (getHurryPopulation(eHurry, hurryCost(true, false)));
}

int CvCity::getHurryPopulation(HurryTypes eHurry, int iHurryCost) const
{
	if (GC.getHurryInfo(eHurry).getProductionPerPopulation() == 0)
	{
		return 0;
	}

	int iPopulation = (iHurryCost - 1) / GC.getGameINLINE().getProductionPerPopulation(eHurry);

	return std::max(1, (iPopulation + 1));
}

int CvCity::hurryProduction(HurryTypes eHurry) const
{
	int iProduction;

	if (GC.getHurryInfo(eHurry).getProductionPerPopulation() > 0)
	{
		int iExtra = hurryPopulation(eHurry) * GC.getGameINLINE().getProductionPerPopulation(eHurry);
		iProduction = 100 * getExtraProductionDifference(iExtra, getProductionModifier()) / std::max(1, getHurryCostModifier());
		FAssert(iProduction >= productionLeft());
	}
	else
	{
		iProduction = productionLeft();
	}

	return iProduction;
}


int CvCity::maxHurryPopulation() const
{
	return (getPopulation() / 2);
}

int CvCity::hurryYield(HurryTypes eHurry, YieldTypes eYield) const
{
	return getHurryYieldNeeded(eHurry, eYield) - getHurryYieldDeficit(eHurry, eYield);
}

int CvCity::cultureDistance(int iDX, int iDY) const
{
	return std::max(1, plotDistance(0, 0, iDX, iDY));
}

/*
	bool CvCity::isHasBuilding(BuildingTypes eIndex) const
	Complexity: O( n )
		n ... number of possible buildings in a building slot
	Purpose:
		Check if a building is present in the city and if it is the "highest" built one in its building slot.
*/
bool CvCity::isHasBuilding(BuildingTypes eIndex) const
{
	PROFILE_FUNC();
	FAssertMsg(eIndex != NO_BUILDING, "BuildingType eIndex is expected to not be NO_BUILDING");
	return (isHasConceptualBuilding(eIndex) && isDominantSpecialBuilding(eIndex));
}

// TAC - AI Economy - koma13 - START
bool CvCity::isBestPortCity() const
{
	return AI_isPort();
}

// TAC - AI Economy - koma13 - END
bool CvCity::isHasBuildingClass(BuildingClassTypes eIndex) const
{
	FAssert(eIndex != NO_BUILDINGCLASS);

	BuildingTypes eBuilding = (BuildingTypes) GC.getCivilizationInfo(GET_PLAYER(getOwnerINLINE()).getCivilizationType()).getCivilizationBuildings(eIndex);
	if(eBuilding == NO_BUILDING)
	{
		return false;
	}

	return isHasBuilding(eBuilding);
}

int CvCity::getProfessionOutput(ProfessionTypes eProfession, const CvUnit* pUnit, BuildingTypes* peBuilding) const
{
	if (peBuilding != NULL)
	{
		*peBuilding = NO_BUILDING;
	}

	FAssert(NO_PROFESSION != eProfession);
	if (NO_PROFESSION == eProfession)
	{
		return 0;
	}

	CvProfessionInfo& kProfessionInfo = GC.getProfessionInfo(eProfession);
	SpecialBuildingTypes eSpecialBuilding = (SpecialBuildingTypes) kProfessionInfo.getSpecialBuilding();
	if (eSpecialBuilding == NO_SPECIALBUILDING)
	{
		return 0;
	}
	// R&R, ray , MYCP partially based on code of Aymerick - START
	YieldTypes eYieldProduced = (YieldTypes) kProfessionInfo.getYieldsProduced(0);
	// R&R, ray , MYCP partially based on code of Aymerick - END
	if (NO_YIELD == eYieldProduced)
	{
		return 0;
	}

	int iModifier = 100;
	if (pUnit != NULL)
	{
		iModifier += GC.getUnitInfo(pUnit->getUnitType()).getYieldModifier(eYieldProduced);
	}

	int iExtra = 0;
	if (pUnit != NULL)
	{
		iExtra += GC.getUnitInfo(pUnit->getUnitType()).getYieldChange(eYieldProduced);
	}

	int iProfessionOutput = 0;
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
					if (peBuilding != NULL)
					{
						*peBuilding = eBuilding;
					}
				}
			}
		}
	}

	return iProfessionOutput;
}

int CvCity::getProfessionInput(ProfessionTypes eProfession, const CvUnit* pUnit) const
{
	FAssert(NO_PROFESSION != eProfession);
	if (NO_PROFESSION == eProfession)
	{
		return 0;
	}

	CvProfessionInfo& kProfessionInfo = GC.getProfessionInfo(eProfession);
	// R&R, ray , MYCP partially based on code of Aymerick - START
	for (int i = 0; i < kProfessionInfo.getNumYieldsConsumed(); i++)
	{
		if (kProfessionInfo.getYieldsConsumed(i) == NO_YIELD)
		{
			return 0;
		}
	}
	// R&R, ray , MYCP partially based on code of Aymerick - END

	return getProfessionOutput(eProfession, pUnit);
}
// R&R, ray , MYCP partially based on code of Aymerick - START
int CvCity::getProfessionInputs(ProfessionTypes eProfession, const CvUnit* pUnit) const
{
	FAssert(NO_PROFESSION != eProfession);
	if (NO_PROFESSION == eProfession)
	{
		return 0;
	}

	CvProfessionInfo& kProfessionInfo = GC.getProfessionInfo(eProfession);
	int iProfessionInput = 0;
	for (int i = 0; i < kProfessionInfo.getNumYieldsConsumed(); i++)
	{
		if (kProfessionInfo.getYieldsConsumed(i) != NO_YIELD)
		{
			iProfessionInput += getProfessionOutput(eProfession, pUnit);
		}
	}

	return iProfessionInput;
}
// R&R, ray , MYCP partially based on code of Aymerick - END
BuildingTypes CvCity::getYieldBuilding(YieldTypes eYield) const
{
	for (int iProfession = 0; iProfession < GC.getNumProfessionInfos(); iProfession++)
	{
		ProfessionTypes eProfession = (ProfessionTypes) iProfession;
		// R&R, ray , MYCP partially based on code of Aymerick - START
		if(GC.getProfessionInfo(eProfession).getYieldsProduced(0) == eYield)
		// R&R, ray , MYCP partially based on code of Aymerick - END
		{
			SpecialBuildingTypes eSpecialBuilding = (SpecialBuildingTypes) GC.getProfessionInfo(eProfession).getSpecialBuilding();
			if(eSpecialBuilding != NO_SPECIALBUILDING)
			{
				for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); ++iBuilding)
				{
					BuildingTypes eBuilding = (BuildingTypes) iBuilding;
					if(isHasBuilding(eBuilding))
					{
						if (GC.getBuildingInfo(eBuilding).getSpecialBuildingType() == eSpecialBuilding)
						{
							return eBuilding;
						}
					}
				}
			}
		}
	}

	return NO_BUILDING;
}

int CvCity::getID() const
{
	return m_iID;
}


int CvCity::getIndex() const
{
	return (getID() & FLTA_INDEX_MASK);
}


IDInfo CvCity::getIDInfo() const
{
	IDInfo city(getOwnerINLINE(), getID());
	return city;
}


void CvCity::setID(int iID)
{
	m_iID = iID;
}


int CvCity::getX() const
{
	return m_iX;
}


int CvCity::getY() const
{
	return m_iY;
}


bool CvCity::at(int iX,  int iY) const
{
	return ((getX_INLINE() == iX) && (getY_INLINE() == iY));
}


bool CvCity::at(CvPlot* pPlot) const
{
	return (plot() == pPlot);
}


CvPlot* CvCity::plot() const
{
	return GC.getMapINLINE().plotSorenINLINE(getX_INLINE(), getY_INLINE());
}

int CvCity::getArea() const
{
	return plot()->getArea();
}

CvArea* CvCity::area() const
{
	return plot()->area();
}


CvArea* CvCity::waterArea() const
{
	return plot()->waterArea();
}


CvPlot* CvCity::getRallyPlot() const
{
	return GC.getMapINLINE().plotSorenINLINE(m_iRallyX, m_iRallyY);
}


void CvCity::setRallyPlot(CvPlot* pPlot)
{
	if (getRallyPlot() != pPlot)
	{
		if (pPlot != NULL)
		{
			m_iRallyX = pPlot->getX_INLINE();
			m_iRallyY = pPlot->getY_INLINE();
		}
		else
		{
			m_iRallyX = INVALID_PLOT_COORD;
			m_iRallyY = INVALID_PLOT_COORD;
		}

		if (isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(ColoredPlots_DIRTY_BIT, true);
		}
	}
}


int CvCity::getGameTurnFounded() const
{
	return m_iGameTurnFounded;
}


void CvCity::setGameTurnFounded(int iNewValue)
{
	if (getGameTurnFounded() != iNewValue)
	{
		m_iGameTurnFounded = iNewValue;
		FAssert(getGameTurnFounded() >= 0);

		GC.getMapINLINE().updateWorkingCity();
	}
}


int CvCity::getGameTurnAcquired() const
{
	return m_iGameTurnAcquired;
}


void CvCity::setGameTurnAcquired(int iNewValue)
{
	m_iGameTurnAcquired = iNewValue;
	FAssert(getGameTurnAcquired() >= 0);
}

// WTP, ray, Native Trade Posts - START
int CvCity::getNativeTradePostGold() const
{
	return m_iTradePostGold;
}

void CvCity::setNativeTradePostGold(int iNewValue)
{
	m_iTradePostGold = iNewValue;
}

void CvCity::changeNativeTradePostGold(int iChange)
{
	setNativeTradePostGold(getNativeTradePostGold() + iChange);
}
// WTP, ray, Native Trade Posts - END

int CvCity::getPopulation() const
{
	return (int) m_aPopulationUnits.size();
}


void CvCity::setPopulation(int iNewValue)
{
	int iOldPopulation = getPopulation();

	if (iOldPopulation != iNewValue)
	{
		//adding more population
		for (int i = iOldPopulation; i < iNewValue; i++)
		{
			UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("DEFAULT_POPULATION_UNIT"));

			if (NO_UNIT != eUnit)
			{
				CvUnit* pUnit = GET_PLAYER(getOwnerINLINE()).initUnit(eUnit, NO_PROFESSION, getX_INLINE(), getY_INLINE());

				addPopulationUnit(pUnit, NO_PROFESSION);
			}
		}

		//removing population
		for (int i = iNewValue; i < iOldPopulation; i++)
		{
			AI_removeWorstPopulationUnit(true);
		}

		FAssert(getPopulation() >= 0);
	}
}

void CvCity::updatePopulation(int iOldPopulation)
{
	GET_PLAYER(getOwnerINLINE()).invalidatePopulationRankCache();

	if (getPopulation() > getHighestPopulation())
	{
		setHighestPopulation(getPopulation());
	}

	area()->changePopulationPerPlayer(getOwnerINLINE(), (getPopulation() - iOldPopulation));

	plot()->updateYield(true);

	GET_PLAYER(getOwnerINLINE()).AI_makeAssignWorkDirty();

	setBillboardDirty(true);
	setLayoutDirty(true);

	plot()->plotAction(PUF_makeInfoBarDirty);

	if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && isCitySelected())
	{
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
	}

	//updateGenericBuildings();
}

void CvCity::changePopulation(int iChange)
{
	setPopulation(getPopulation() + iChange);
}


long CvCity::getRealPopulation() const
{
	return (getPopulation() * 100L);
}

int CvCity::getHighestPopulation() const
{
	return m_iHighestPopulation;
}


void CvCity::setHighestPopulation(int iNewValue)
{
 	m_iHighestPopulation = iNewValue;
	FAssert(getHighestPopulation() >= 0);
}


int CvCity::getWorksWaterCount() const
{
	return m_iWorksWaterCount;
}

bool CvCity::isWorksWater() const
{
	return (getWorksWaterCount() > 0);
}

void CvCity::changeWorksWaterCount(int iChange)
{
	if (iChange != 0)
	{
		m_iWorksWaterCount += iChange;
		FAssert(getWorksWaterCount() >= 0);

		AI_setAssignWorkDirty(true);
	}
}

int CvCity::getHealRate() const
{
	return m_iHealRate * (100 + getCityHealth()) / 100; // R&R, ray, Health - START
}


void CvCity::changeHealRate(int iChange)
{
	m_iHealRate = (m_iHealRate + iChange);
	FAssert(getHealRate() >= 0);
}

int CvCity::getFood() const
{
	return getYieldStored(YIELD_FOOD);
}


void CvCity::setFood(int iNewValue)
{
	setYieldStored(YIELD_FOOD, iNewValue);
}


void CvCity::changeFood(int iChange)
{
	setFood(getFood() + iChange);
}


int CvCity::getFoodKept() const
{
	return m_iFoodKept;
}


void CvCity::setFoodKept(int iNewValue)
{
	m_iFoodKept = iNewValue;
}


void CvCity::changeFoodKept(int iChange)
{
	setFoodKept(getFoodKept() + iChange);
}


int CvCity::getMaxFoodKeptPercent() const
{
	return m_iMaxFoodKeptPercent;
}


void CvCity::changeMaxFoodKeptPercent(int iChange)
{
	m_iMaxFoodKeptPercent = (m_iMaxFoodKeptPercent + iChange);
	FAssert(getMaxFoodKeptPercent() >= 0);
}


int CvCity::getOverflowProduction() const
{
	return m_iOverflowProduction;
}

void CvCity::setOverflowProduction(int iNewValue)
{
	m_iOverflowProduction = iNewValue;
	FAssert(getOverflowProduction() >= 0);
}

void CvCity::changeOverflowProduction(int iChange, int iProductionModifier)
{
	setOverflowProduction(getOverflowProduction() + iChange);
}

int CvCity::getMilitaryProductionModifier()	const
{
	return m_iMilitaryProductionModifier;
}

void CvCity::changeMilitaryProductionModifier(int iChange)
{
	m_iMilitaryProductionModifier = (m_iMilitaryProductionModifier + iChange);
}


int CvCity::getBuildingDefense() const
{
	return m_iBuildingDefense;
}


void CvCity::changeBuildingDefense(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingDefense = (m_iBuildingDefense + iChange);
		FAssert(getBuildingDefense() >= 0);

		setBillboardDirty(true);

		plot()->plotAction(PUF_makeInfoBarDirty);
	}
}


int CvCity::getBuildingBombardDefense() const
{
	return m_iBuildingBombardDefense;
}


void CvCity::changeBuildingBombardDefense(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingBombardDefense += iChange;
		FAssert(getBuildingBombardDefense() >= 0);
	}
}


int CvCity::getFreeExperience() const
{
	return m_iFreeExperience;
}


void CvCity::changeFreeExperience(int iChange)
{
	m_iFreeExperience = (m_iFreeExperience + iChange);
	FAssert(getFreeExperience() >= 0);
}


int CvCity::getDefenseDamage() const
{
	return m_iDefenseDamage;
}

void CvCity::changeDefenseDamage(int iChange)
{
	if (iChange != 0)
	{
		m_iDefenseDamage = range((m_iDefenseDamage + iChange), 0, GC.getMAX_CITY_DEFENSE_DAMAGE());

		if (iChange > 0)
		{
			setBombarded(true);
		}

		setBillboardDirty(true);

		plot()->plotAction(PUF_makeInfoBarDirty);
	}
}

void CvCity::changeDefenseModifier(int iChange)
{
	if (iChange != 0)
	{
		int iTotalDefense = getTotalDefense();

		if (iTotalDefense > 0)
		{
			changeDefenseDamage(-(GC.getMAX_CITY_DEFENSE_DAMAGE() * iChange) / iTotalDefense);
		}
	}
}


int CvCity::getLastDefenseDamage() const
{
	return m_iLastDefenseDamage;
}


void CvCity::setLastDefenseDamage(int iNewValue)
{
	m_iLastDefenseDamage = iNewValue;
}


bool CvCity::isBombardable(const CvUnit* pUnit) const
{
	if (NULL != pUnit && !pUnit->isEnemy(getTeam()))
	{
		return false;
	}

	return (getDefenseModifier() > 0);
}


int CvCity::getTotalDefense() const
{
	return (getBuildingDefense() + GET_PLAYER(getOwnerINLINE()).getCityDefenseModifier());
}


int CvCity::getDefenseModifier() const
{
	if (isOccupation())
	{
		return 0;
	}

	return ((getTotalDefense() * (GC.getMAX_CITY_DEFENSE_DAMAGE() - getDefenseDamage())) / GC.getMAX_CITY_DEFENSE_DAMAGE());
}


int CvCity::getOccupationTimer() const
{
	return m_iOccupationTimer;
}


bool CvCity::isOccupation() const
{
	return (getOccupationTimer() > 0);
}


void CvCity::setOccupationTimer(int iNewValue)
{
	bool bOldOccupation;

	if (getOccupationTimer() != iNewValue)
	{
		bOldOccupation = isOccupation();

		m_iOccupationTimer = iNewValue;
		FAssert(getOccupationTimer() >= 0);

		if (bOldOccupation != isOccupation())
		{
			updateCultureLevel();

			AI_setAssignWorkDirty(true);
		}

		setBillboardDirty(true);
	}
}


void CvCity::changeOccupationTimer(int iChange)
{
	setOccupationTimer(getOccupationTimer() + iChange);
}


int CvCity::getCultureUpdateTimer() const
{
	return m_iCultureUpdateTimer;
}


void CvCity::setCultureUpdateTimer(int iNewValue)
{
	m_iCultureUpdateTimer = iNewValue;
	FAssert(getOccupationTimer() >= 0);
}


void CvCity::changeCultureUpdateTimer(int iChange)
{
	setCultureUpdateTimer(getCultureUpdateTimer() + iChange);
}


int CvCity::getCitySizeBoost() const
{
	return m_iCitySizeBoost;
}


void CvCity::setCitySizeBoost(int iBoost)
{
	if (getCitySizeBoost() != iBoost)
	{
		m_iCitySizeBoost = iBoost;

		setLayoutDirty(true);
	}
}


bool CvCity::isNeverLost() const
{
	return m_bNeverLost;
}


void CvCity::setNeverLost(bool bNewValue)
{
	m_bNeverLost = bNewValue;
}


bool CvCity::isBombarded() const
{
	return m_bBombarded;
}


void CvCity::setBombarded(bool bNewValue)
{
	m_bBombarded = bNewValue;
}

void CvCity::setAllCitizensAutomated(bool bAutomated)
{
	for (uint i = 0; i < m_aPopulationUnits.size(); ++i)
	{
		CvUnit* pUnit = m_aPopulationUnits[i];
		if (pUnit != NULL)
		{
			pUnit->setColonistLocked(!bAutomated);
		}
	}

	if (bAutomated)
	{
		AI_assignWorkingPlots();
	}

	if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && isCitySelected())
	{
		gDLL->getInterfaceIFace()->setDirty(CitizenButtons_DIRTY_BIT, true);
	}
}

void CvCity::setCitizenAutomated(int iUnitId)
{
	CvUnit* pUnit = getPopulationUnitById(iUnitId);
	if (pUnit != NULL)
	{
		pUnit->setColonistLocked(false);

		AI_assignWorkingPlots();
		if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		}
	}
}

bool CvCity::isProductionAutomated() const
{
	return m_bProductionAutomated;
}


void CvCity::setProductionAutomated(bool bNewValue, bool bClear)
{
	if (isProductionAutomated() != bNewValue)
	{
		m_bProductionAutomated = bNewValue;

		if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);

			// if automated and not network game and all 3 modifiers down, clear the queue and choose again
			if (bNewValue && bClear)
			{
				clearOrderQueue();
			}
		}
		
		if (!isProduction())
		{
			AI_chooseProduction();
		}
	}
}


bool CvCity::isWallOverride() const
{
	return m_bWallOverride;
}


void CvCity::setWallOverride(bool bOverride)
{
	if (isWallOverride() != bOverride)
	{
		m_bWallOverride = bOverride;

		setLayoutDirty(true);
	}
}


bool CvCity::isBillboardDirty() const
{
	return m_bInfoDirty;
}


void CvCity::setBillboardDirty(bool bNewValue)
{
	m_bInfoDirty = bNewValue;
}


bool CvCity::isLayoutDirty() const
{
	return m_bLayoutDirty;
}


void CvCity::setLayoutDirty(bool bNewValue)
{
	m_bLayoutDirty = bNewValue;
}


PlayerTypes CvCity::getOwner() const
{
	return getOwnerINLINE();
}


PlayerTypes CvCity::getPreviousOwner() const
{
	return m_ePreviousOwner;
}


void CvCity::setPreviousOwner(PlayerTypes eNewValue)
{
	m_ePreviousOwner = eNewValue;
}


PlayerTypes CvCity::getOriginalOwner() const
{
	return m_eOriginalOwner;
}


void CvCity::setOriginalOwner(PlayerTypes eNewValue)
{
	m_eOriginalOwner = eNewValue;
}


TeamTypes CvCity::getTeam() const
{
	return GET_PLAYER(getOwnerINLINE()).getTeam();
}


CultureLevelTypes CvCity::getCultureLevel() const
{
	return m_eCultureLevel;
}


int CvCity::getCultureThreshold() const
{
	return getCultureThreshold(getCultureLevel());
}

int CvCity::getCultureThreshold(CultureLevelTypes eLevel) const
{
	if (eLevel == NO_CULTURELEVEL)
	{
		return 1;
	}

	CultureLevelTypes eCultureLevel = (CultureLevelTypes) std::min(eLevel + 1, GC.getNumCultureLevelInfos() - 1);
	return std::max(1, GC.getGameINLINE().getCultureLevelThreshold(eCultureLevel, getOwnerINLINE()));
}


void CvCity::setCultureLevel(CultureLevelTypes eNewValue)
{
	CvPlot* pLoopPlot;
	CvWString szBuffer;
	CultureLevelTypes eOldValue;
	int iCultureRange;
	int iDX, iDY;
	int iI;

	eOldValue = getCultureLevel();

	if (eOldValue != eNewValue)
	{
		m_eCultureLevel = eNewValue;

		if (eOldValue != NO_CULTURELEVEL)
		{
			for (iDX = -eOldValue; iDX <= eOldValue; iDX++)
			{
				for (iDY = -eOldValue; iDY <= eOldValue; iDY++)
				{
					iCultureRange = cultureDistance(iDX, iDY);

					if (iCultureRange > getCultureLevel())
					{
						if (iCultureRange <= eOldValue)
						{
							FAssert(iCultureRange <= GC.getNumCultureLevelInfos());

							pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

							if (pLoopPlot != NULL)
							{
								pLoopPlot->changeCultureRangeCities(getOwnerINLINE(), iCultureRange, -1);
							}
						}
					}
				}
			}
		}

		if (getCultureLevel() != NO_CULTURELEVEL)
		{
			for (iDX = -getCultureLevel(); iDX <= getCultureLevel(); iDX++)
			{
				for (iDY = -getCultureLevel(); iDY <= getCultureLevel(); iDY++)
				{
					iCultureRange = cultureDistance(iDX, iDY);

					if (iCultureRange > eOldValue)
					{
						if (iCultureRange <= getCultureLevel())
						{
							FAssert(iCultureRange <= GC.getNumCultureLevelInfos());

							pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

							if (pLoopPlot != NULL)
							{
								pLoopPlot->changeCultureRangeCities(getOwnerINLINE(), iCultureRange, 1);
							}
						}
					}
				}
			}
		}

		//update all affected plots
		int iRange = std::max(getCultureLevel(), eOldValue) + GC.getDefineINT("BUY_PLOT_CULTURE_RANGE");
		for (iDX = -iRange; iDX <= iRange; iDX++)
		{
			for (iDY = -iRange; iDY <= iRange; iDY++)
			{
				pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

				if (pLoopPlot != NULL)
				{
					pLoopPlot->updateCulture(true);
				}
			}
		}

		if (GC.getGameINLINE().isFinalInitialized())
		{
			YieldTypes eCultureYield = (YieldTypes) GC.getDefineINT("CULTURE_YIELD");
			if ((getCultureLevel() > eOldValue) && (getCultureLevel() > 1) && eCultureYield != NO_YIELD)
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_BORDERS_EXPANDED", getNameKey());
				gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CULTUREEXPANDS", MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(eCultureYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), true, true);

				if (getCultureLevel() == (GC.getNumCultureLevelInfos() - 1))
				{
					for (iI = 0; iI < MAX_PLAYERS; iI++)
					{
						if (GET_PLAYER((PlayerTypes)iI).isAlive())
						{
							if (isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
							{
								szBuffer = gDLL->getText("TXT_KEY_MISC_CULTURE_LEVEL", getNameKey(), GC.getCultureLevelInfo(getCultureLevel()).getTextKeyWide());
								gDLL->getInterfaceIFace()->addMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CULTURELEVEL", MESSAGE_TYPE_MAJOR_EVENT, GC.getYieldInfo(eCultureYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
							}
							else
							{
								szBuffer = gDLL->getText("TXT_KEY_MISC_CULTURE_LEVEL_UNKNOWN", GC.getCultureLevelInfo(getCultureLevel()).getTextKeyWide());
								gDLL->getInterfaceIFace()->addMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CULTURELEVEL", MESSAGE_TYPE_MAJOR_EVENT, GC.getYieldInfo(eCultureYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
							}
						}
					}
				}

				// ONEVENT - Culture growth
				gDLL->getEventReporterIFace()->cultureExpansion(this, getOwnerINLINE());
			}
		}
	}
}


void CvCity::updateCultureLevel()
{
	if (getCultureUpdateTimer() > 0)
	{
		return;
	}

	CultureLevelTypes eCultureLevel = ((CultureLevelTypes)0);

	//if (!isOccupation()) // R&R mod, vetiarvind, bug fix for units "disappearing" during disorder
	{
		for (int iI = (GC.getNumCultureLevelInfos() - 1); iI > 0; iI--)
		{
			if (getCulture(getOwnerINLINE()) >= GC.getGameINLINE().getCultureLevelThreshold((CultureLevelTypes) iI, getOwnerINLINE()))
			{
				eCultureLevel = ((CultureLevelTypes)iI);
				break;
			}
		}
	}

	setCultureLevel(eCultureLevel);
}

// R&R, ray, Landplot Yields - START
int CvCity::getLandPlotYield(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	return m_aiLandPlotYield[eIndex];
}


void CvCity::changeLandPlotYield(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if (iChange != 0)
	{
		m_aiLandPlotYield[eIndex] += iChange;
		m_aiLandPlotYield[eIndex] = std::max(m_aiLandPlotYield[eIndex], 0);
		updateYield();
	}
}
// R&R, ray, Landplot Yields -END

int CvCity::getSeaPlotYield(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	return m_aiSeaPlotYield[eIndex];
}


void CvCity::changeSeaPlotYield(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if (iChange != 0)
	{
		m_aiSeaPlotYield[eIndex] += iChange;
		m_aiSeaPlotYield[eIndex] = std::max(m_aiSeaPlotYield[eIndex], 0);
		updateYield();
	}
}


int CvCity::getRiverPlotYield(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	return m_aiRiverPlotYield[eIndex];
}


void CvCity::changeRiverPlotYield(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if (iChange != 0)
	{
		m_aiRiverPlotYield[eIndex] += iChange;
		m_aiRiverPlotYield[eIndex] = std::max(m_aiRiverPlotYield[eIndex], 0);
		updateYield();
	}
}


int CvCity::getBaseRawYieldProduced(YieldTypes eYieldType) const
{
	PROFILE_FUNC();

	FAssertMsg(NO_PLAYER != getOwnerINLINE(), "City must have an owner");

	if (NO_YIELD == eYieldType)
	{
		return 0;
	}
	CvYieldInfo& info = GC.getYieldInfo(eYieldType);

	if (isOccupation())
	{
		return 0;
	}

	if (NO_PLAYER == getOwnerINLINE())
	{
		return 0;
	}
	CvPlayer& owner = GET_PLAYER(getOwnerINLINE());

	//indoor professions
	int iCityYieldProduction = 0;
	for (int i = 0; i < getPopulation(); ++i)
	{
		CvUnit* pUnit = getPopulationUnitByIndex(i);
		if (NULL != pUnit)
		{
			ProfessionTypes eProfession = pUnit->getProfession();
			if (NO_PROFESSION != eProfession)
			{
				CvProfessionInfo& kProfessionInfo = GC.getProfessionInfo(eProfession);
				// R&R, ray , MYCP partially based on code of Aymerick - START
				if (kProfessionInfo.getNumYieldsProduced() > 1)
				{
					YieldTypes eYieldProduced = (YieldTypes) kProfessionInfo.getYieldsProduced(0);
					YieldTypes eYieldProduced2 = (YieldTypes) kProfessionInfo.getYieldsProduced(1);
					if(eYieldProduced == eYieldType || eYieldProduced2 == eYieldType)
					{
						iCityYieldProduction += getProfessionOutput(eProfession, pUnit);
					}
				}
				else
				{
					YieldTypes eYieldProduced = (YieldTypes) kProfessionInfo.getYieldsProduced(0);
					if(eYieldProduced == eYieldType)
					{
						iCityYieldProduction += getProfessionOutput(eProfession, pUnit);
					}
				}
				// R&R, ray , MYCP partially based on code of Aymerick - END
			}
		}
	}

	//outdoor professions
	int iPlotYieldProduction = 0;
	for (int i = 0; i < NUM_CITY_PLOTS; ++i)
	{
		CvPlot* pPlot = getCityIndexPlot(i);
		if (pPlot != NULL)
		{
			if(isUnitWorkingPlot(i))
			{
				iPlotYieldProduction += pPlot->getYield(eYieldType);
			}
		}
	}

	//building extra
	int iBuildingYieldProduced = 0;
	CvCivilizationInfo& civilizationInfo = GC.getCivilizationInfo(getCivilizationType());
	for (int i = 0; i < GC.getNumBuildingClassInfos(); ++i)
	{
		BuildingTypes eBuilding = (BuildingTypes) civilizationInfo.getCivilizationBuildings(i);

		if (eBuilding != NO_BUILDING && isHasBuilding(eBuilding))
		{
			iBuildingYieldProduced += GC.getBuildingInfo(eBuilding).getYieldChange(eYieldType);
			iBuildingYieldProduced += getBuildingYieldChange((BuildingClassTypes) i, eYieldType);
			iBuildingYieldProduced += owner.getBuildingYieldChange((BuildingClassTypes) i, eYieldType);
		}
	}

	return (iCityYieldProduction + iPlotYieldProduction + iBuildingYieldProduced);
}

int CvCity::getRawYieldProduced(YieldTypes eYieldType) const
{
	// R&R, ray, safety check for negative Yield Modifiers from Health - START
	int iRawYield = getBaseRawYieldProduced(eYieldType);

	if (iRawYield == 0)
	{
		return 0;
	}
	else if (iRawYield == 1) // we will not allow 1 to be modified to 0
	{
		return std::max(1, (iRawYield * getBaseYieldRateModifier(eYieldType)) / 100);
	}
	else
	{
		return (iRawYield * getBaseYieldRateModifier(eYieldType)) / 100;
	}
	// R&R, ray, safety check for negative Yield Modifiers from Health - END
}

int CvCity::getRawYieldConsumed(YieldTypes eYieldType) const
{
	PROFILE_FUNC();

	if (isOccupation())
	{
		return 0;
	}

	int iYieldConsumed = 0;
	if (eYieldType == YIELD_FOOD)
	{
		iYieldConsumed = getPopulation() * GC.getFOOD_CONSUMPTION_PER_POPULATION();
	}

	for (int i = 0; i < getPopulation(); ++i)
	{
		CvUnit* pUnit = getPopulationUnitByIndex(i);
		if (NULL != pUnit)
		{
			ProfessionTypes eProfession = pUnit->getProfession();
			if (NO_PROFESSION != eProfession)
			{
				CvProfessionInfo& kProfessionInfo = GC.getProfessionInfo(eProfession);
				// R&R, ray , MYCP partially based on code of Aymerick - START
				for (int j = 0; j < kProfessionInfo.getNumYieldsConsumed(); j++)
				{
					if (kProfessionInfo.getYieldsConsumed(j) == eYieldType)
					{
						int iProfessionYield = getProfessionInput(eProfession, pUnit);
						if (iProfessionYield != 0)
						{
							iYieldConsumed += iProfessionYield;
						}
					}
				}
				// R&R, ray , MYCP partially based on code of Aymerick - END
			}
		}
	}
	return iYieldConsumed;
}

int CvCity::getBaseYieldRateModifier(YieldTypes eIndex, int iExtra) const
{
	int iModifier;

	iModifier = getYieldRateModifier(eIndex);

	if (area() != NULL)
	{
		iModifier += area()->getYieldRateModifier(getOwnerINLINE(), eIndex);
	}

	iModifier += GET_PLAYER(getOwnerINLINE()).getYieldRateModifier(eIndex);

	iModifier += GET_PLAYER(getOwnerINLINE()).getTaxYieldRateModifier(eIndex);

	iModifier += getRebelPercent() * GC.getMAX_REBEL_YIELD_MODIFIER() / 100;
	
	// R&R, ray, Health
	iModifier += getCityHealth(); // negative CityHealth is possible

	if (isCapital())
	{
		iModifier += GET_PLAYER(getOwnerINLINE()).getCapitalYieldRateModifier(eIndex);
	}

	iModifier += iExtra;

	// note: player->invalidateYieldRankCache() must be called for anything that is checked here
	// so if any extra checked things are added here, the cache needs to be invalidated

	return std::max(0, (iModifier + 100));
}


int CvCity::getYieldRate(YieldTypes eIndex) const
{
	if (isOccupation())
	{
		return 0;
	}

	return (getRawYieldProduced(eIndex) - getRawYieldConsumed(eIndex));
}

int CvCity::getCultureRate() const
{
	YieldTypes eYield = (YieldTypes) GC.getDefineINT("CULTURE_YIELD");
	if (eYield == NO_YIELD)
	{
		return 0;
	}

	int totalNewCulture = 0;
	int cultureFromPopulation = 0;
	
	if (!isNative()) // R&R, ray, Culture from Population not for Natives
	{
		cultureFromPopulation = getPopulation() / 3; 
	}
	totalNewCulture = cultureFromPopulation + calculateNetYield(eYield);

	return totalNewCulture;
}

void CvCity::setYieldRateDirty()
{
	if (getTeam() == GC.getGameINLINE().getActiveTeam())
	{
		GET_PLAYER(getOwnerINLINE()).setCityBillboardDirty(true);
		GET_PLAYER(getOwnerINLINE()).invalidateYieldRankCache(NO_YIELD);

		if (isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true );
		}
	}
}

int CvCity::getYieldRateModifier(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	return m_aiYieldRateModifier[eIndex];
}


void CvCity::changeYieldRateModifier(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if (iChange != 0)
	{
		m_aiYieldRateModifier[eIndex] += iChange;
		FAssert(getYieldRateModifier(eIndex) >= 0);

		GET_PLAYER(getOwnerINLINE()).invalidateYieldRankCache(eIndex);

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setBillboardDirty(true);
		}
	}
}
// TAC - AI Economy - koma13 - START
int CvCity::getProductionOutput(ProfessionTypes eProfession) const
{
	FAssert(eProfession != NO_PROFESSION);
	FAssert(GC.getProfessionInfo(eProfession).isCitizen());

	CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);
	
	int iOutput = 0;

	if (kProfession.isWorkPlot())
	{
		for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
			if (iI != CITY_HOME_PLOT)
			{
				CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);
				if ((pLoopPlot != NULL) && (pLoopPlot->getWorkingCity() == this))
				{
					iOutput += AI_professionBasicOutput(eProfession, NO_UNIT, pLoopPlot);
				}
			}
		}
	}
	else
	{
		iOutput = AI_professionBasicOutput(eProfession, NO_UNIT, NULL);
	}

	return iOutput;
}

int CvCity::getPotentialProductionOutput(ProfessionTypes eProfession) const
{
	FAssert(eProfession != NO_PROFESSION);
	FAssert(GC.getProfessionInfo(eProfession).isCitizen());

	CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);
	
	// R&R, ray , MYCP partially based on code of Aymerick - START
	YieldTypes eYieldProduced = (YieldTypes)kProfession.getYieldsProduced(0);
	// R&R, ray , MYCP partially based on code of Aymerick - END
	if (eYieldProduced == NO_YIELD)
	{
		return 0;
	}

	int iOutput = 0;

	if (kProfession.isWorkPlot())
	{
		for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
			if (iI != CITY_HOME_PLOT)
			{
				CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);
				if ((pLoopPlot != NULL) && (pLoopPlot->getWorkingCity() == this))
				{
					YieldTypes eBestYield = NO_YIELD;
					int iBestOutput = 0;
					
					for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
					{
						if ((iJ != YIELD_FOOD) && (iJ != YIELD_LUMBER))
						{
							int iYield = pLoopPlot->calculateBestNatureYield((YieldTypes) iJ, getTeam());
							if (iYield > iBestOutput)
							{
								eBestYield = (YieldTypes) iJ;
								iBestOutput = iYield;
							}
						}
					}
					
					if (eBestYield == eYieldProduced)
					{
						iOutput += iBestOutput;
					}
				}
			}
		}
	}
	else
	{
		for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); ++iBuildingClass)
		{
			BuildingTypes eLoopBuilding = (BuildingTypes) GC.getCivilizationInfo(GET_PLAYER(getOwnerINLINE()).getCivilizationType()).getCivilizationBuildings(iBuildingClass);
			if (NO_BUILDING != eLoopBuilding)
			{
				CvBuildingInfo& kLoopBuilding = GC.getBuildingInfo(eLoopBuilding);
				if (kLoopBuilding.getSpecialBuildingType() == kProfession.getSpecialBuilding())
				{
					iOutput = kLoopBuilding.getProfessionOutput() * getNumProfessionBuildingSlots(eProfession);
					break;
				}
			}
		}
	}

	return iOutput;
}

bool CvCity::hasOtherProductionBuilding(BuildingTypes eBuilding, int iMax) const
{
	FAssert(eBuilding != NO_BUILDING)

	CvBuildingInfo& kBuildingInfo = GC.getBuildingInfo(eBuilding);
	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());

	int iCount = 0;

	for (int i = 0; i < GC.getNumProfessionInfos(); ++i)
	{
		CvProfessionInfo& kProfession = GC.getProfessionInfo((ProfessionTypes)i);
		
		if (GC.getCivilizationInfo(kOwner.getCivilizationType()).isValidProfession(i))
		{
			if (kProfession.getSpecialBuilding() != kBuildingInfo.getSpecialBuildingType())
			{
				// R&R, ray , MYCP partially based on code of Aymerick - START
				YieldTypes eYieldProduced = (YieldTypes)kProfession.getYieldsProduced(0);
				// R&R, ray , MYCP partially based on code of Aymerick - END
				
				if (eYieldProduced != NO_YIELD && GC.getYieldInfo(eYieldProduced).isCargo())
				{
					for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); ++iBuildingClass)
					{
						BuildingTypes eLoopBuilding = (BuildingTypes) GC.getCivilizationInfo(GET_PLAYER(getOwnerINLINE()).getCivilizationType()).getCivilizationBuildings(iBuildingClass);
						if (NO_BUILDING != eLoopBuilding)
						{
							CvBuildingInfo& kLoopBuilding = GC.getBuildingInfo(eLoopBuilding);
							if (kLoopBuilding.getSpecialBuildingType() == kProfession.getSpecialBuilding())
							{
								if (isHasConceptualBuilding(eLoopBuilding))
								{
									if (kLoopBuilding.getSpecialBuildingPriority() > 0)
									{
										iCount++;

										if (iCount > iMax)
										{
											return true;
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

	return false;
}
// TAC - AI Economy - koma13 - END
int CvCity::getDomainFreeExperience(DomainTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex expected to be < NUM_DOMAIN_TYPES");
	return m_aiDomainFreeExperience[eIndex];
}


void CvCity::changeDomainFreeExperience(DomainTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex expected to be < NUM_DOMAIN_TYPES");
	m_aiDomainFreeExperience[eIndex] = (m_aiDomainFreeExperience[eIndex] + iChange);
	FAssert(getDomainFreeExperience(eIndex) >= 0);
}


int CvCity::getDomainProductionModifier(DomainTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex expected to be < NUM_DOMAIN_TYPES");
	return m_aiDomainProductionModifier[eIndex];
}


void CvCity::changeDomainProductionModifier(DomainTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex expected to be < NUM_DOMAIN_TYPES");
	m_aiDomainProductionModifier[eIndex] = (m_aiDomainProductionModifier[eIndex] + iChange);
}


int CvCity::getCulture(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex expected to be < MAX_PLAYERS");
	return m_aiCulture[eIndex];
}

int CvCity::countTotalCulture() const
{
	int iTotalCulture;
	int iI;

	iTotalCulture = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			iTotalCulture += getCulture((PlayerTypes)iI);
		}
	}

	return iTotalCulture;
}


PlayerTypes CvCity::findHighestCulture() const
{
	PlayerTypes eBestPlayer;
	int iValue;
	int iBestValue;
	int iI;

	iBestValue = 0;
	eBestPlayer = NO_PLAYER;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			iValue = getCulture((PlayerTypes)iI);

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestPlayer = ((PlayerTypes)iI);
			}
		}
	}

	return eBestPlayer;
}


int CvCity::calculateCulturePercent(PlayerTypes eIndex) const
{
	int iTotalCulture;

	iTotalCulture = countTotalCulture();

	if (iTotalCulture > 0)
	{
		return ((getCulture(eIndex) * 100) / iTotalCulture);
	}

	return 0;
}


int CvCity::calculateTeamCulturePercent(TeamTypes eIndex) const
{
	int iTeamCulturePercent;
	int iI;

	iTeamCulturePercent = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == eIndex)
			{
				iTeamCulturePercent += calculateCulturePercent((PlayerTypes)iI);
			}
		}
	}

	return iTeamCulturePercent;
}


void CvCity::setCulture(PlayerTypes eIndex, int iNewValue, bool bPlots)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex expected to be < MAX_PLAYERS");

	if (getCulture(eIndex) != iNewValue)
	{
		m_aiCulture[eIndex] = iNewValue;
		FAssert(getCulture(eIndex) >= 0);

		updateCultureLevel();

		if (bPlots)
		{
			doPlotCulture(true, eIndex, 0);
		}
	}
}


void CvCity::changeCulture(PlayerTypes eIndex, int iChange, bool bPlots)
{
	setCulture(eIndex, (getCulture(eIndex) + iChange), bPlots);
}

//VET NewCapacity - begin 2/9
int CvCity::getTotalYieldStored() const
	{return m_iTotalYieldStored;}

void CvCity::setTotalYieldStored(int iValue)
	{m_iTotalYieldStored = iValue;}

void CvCity::changeTotalYieldStored(int iChange)
	{setTotalYieldStored(getTotalYieldStored() + iChange);}
//VET NewCapacity - end 2/9

int CvCity::getYieldStored(YieldTypes eYield) const
{
	FAssertMsg(eYield >= 0, "eYield expected to be >= 0");
	FAssertMsg(eYield < NUM_YIELD_TYPES	, "eYield expected to be < NUM_YIELD_TYPES");
	return m_aiYieldStored[eYield];
}

void CvCity::setYieldStored(YieldTypes eYield, int iValue)
{
	FAssertMsg(eYield >= 0, "eYield expected to be >= 0");
	FAssertMsg(eYield < NUM_YIELD_TYPES	, "eYield expected to be < NUM_YIELD_TYPES");
	FAssert(iValue >= 0 || eYield == YIELD_FOOD);

	int iChange = iValue - getYieldStored(eYield);
	if (iChange != 0)
	{
//VET NewCapacity - begin 3/9
		if ((eYield != YIELD_FOOD) && (eYield != YIELD_LUMBER) && (eYield != YIELD_STONE) && GC.getYieldInfo(eYield).isCargo())
			{changeTotalYieldStored(iChange);}
//VET NewCapacity - end 3/9
		m_aiYieldStored[eYield] = iValue;

		if (!AI_isWorkforceHack())
		{
			checkCompletedBuilds(eYield, iChange);
		}

		GET_PLAYER(getOwnerINLINE()).changePower(GC.getYieldInfo(eYield).getPowerValue() * iChange);
		GET_PLAYER(getOwnerINLINE()).changeAssets(GC.getYieldInfo(eYield).getAssetValue() * iChange);
		area()->changePower(getOwnerINLINE(), GC.getYieldInfo(eYield).getPowerValue() * iChange);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setBillboardDirty(true);

			if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setDirty(ResourceTable_DIRTY_BIT, true);
				gDLL->getInterfaceIFace()->setDirty(CitizenButtons_DIRTY_BIT, true);
				gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
			}
		}
		// R&R, Androrc, Livestock Breeding
		//if (GC.getYieldInfo(eYield).isLivestock())
		if (GC.getYieldInfo(eYield).isLivestock() && (isHuman() || isNative())) // R&R, ray, Livestock Breeding, for AI
		{
			for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
			{
				CvPlot* pLoopPlot = getCityIndexPlot(iI);

				if (pLoopPlot != NULL)
				{
					if (pLoopPlot->getWorkingCity() == this)
					{
						pLoopPlot->updateYield(true);
						// R&R, ray , MYCP partially based on code of Aymerick - START
						if (getYieldStored(eYield) <= 0 && getUnitWorkingPlot(pLoopPlot) != NULL && eYield == GC.getProfessionInfo(getUnitWorkingPlot(pLoopPlot)->getProfession()).getYieldsProduced(0))
						// R&R, ray , MYCP partially based on code of Aymerick - END
						{
							getUnitWorkingPlot(pLoopPlot)->setColonistLocked(false);
							clearUnitWorkingPlot(pLoopPlot);
						}
					}
				}
			}
		}
		// R&R, Androrc, Livestock Breeding, END
		// transport feeder - start - Nightinggale
		checkImportsMaintain(eYield);
		// transport feeder - end - Nightinggale
	}
}

void CvCity::changeYieldStored(YieldTypes eYield, int iChange)
{
	setYieldStored(eYield, getYieldStored(eYield) + iChange);
}

int CvCity::getYieldRushed(YieldTypes eYield) const
{
	FAssert(eYield >= 0 && eYield < NUM_YIELD_TYPES);
	return m_aiYieldRushed[eYield];
}

void CvCity::changeYieldRushed(YieldTypes eYield, int iChange)
{
	FAssert(eYield >= 0 && eYield < NUM_YIELD_TYPES);
	m_aiYieldRushed[eYield] += iChange;
	FAssert(getYieldRushed(eYield) >= 0);
}

// R&R, ray , MYCP partially based on code of Aymerick - START
// heavily adjusted
void CvCity::calculateNetYields(int aiYields[NUM_YIELD_TYPES], int* aiProducedYields, int* aiConsumedYields, bool bPrintWarning) const
{
	PROFILE_FUNC();
	
	int aiConsumed[NUM_YIELD_TYPES];
	int aiProduced[NUM_YIELD_TYPES];
	if (aiProducedYields == NULL)
	{
		aiProducedYields = aiProduced;
	}
	if (aiConsumedYields == NULL)
	{
		aiConsumedYields = aiConsumed;
	}

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		YieldTypes eYield = (YieldTypes) iYield;
		aiConsumedYields[iYield] = getRawYieldConsumed(eYield);
		aiProducedYields[iYield] = getBaseRawYieldProduced(eYield);
		aiYields[iYield] = getYieldStored(eYield) - aiConsumedYields[iYield] + aiProducedYields[iYield] * getBaseYieldRateModifier(eYield) / 100;
	}

	std::set<ProfessionTypes> setUnsatisfiedProfessions;
	// TAC - Messages - Ray - START
	std::set<ProfessionTypes> setAlmostUnsatisfiedProfessions;
	std::set<ProfessionTypes> setBoredProfessions;
	// TAC - Messages - Ray - END

	if (!isOccupation())
	{
		std::vector<int> aiYieldsAvailable;
		for (uint i = 0; i < m_aPopulationUnits.size(); ++i)
		{
			CvUnit* pUnit = m_aPopulationUnits[i];
			if (pUnit->getProfession() != NO_PROFESSION)
			{
				aiYieldsAvailable.push_back(getProfessionInput(pUnit->getProfession(), pUnit));
			}
			else
			{
				aiYieldsAvailable.push_back(0);
			}
		}
		// TAC - Messages PART 2 - Ray - BEGIN
		for (int iUnitIndex = 0; iUnitIndex < (int)m_aPopulationUnits.size(); ++iUnitIndex)
		{
			CvUnit* pUnit = m_aPopulationUnits[iUnitIndex];
			
			if (pUnit->getProfession() != NO_PROFESSION)
			{
				CvProfessionInfo& kProfession = GC.getProfessionInfo(pUnit->getProfession());
				if (kProfession.isWorkPlot())
				{
					YieldTypes eYieldProduced = (YieldTypes) kProfession.getYieldsProduced(0);
					CvPlot* pCurrentWorkedPlot = getPlotWorkedByUnit(pUnit);
				
					if (pCurrentWorkedPlot != NULL)
					{
						int availableAmountOfYield = pCurrentWorkedPlot->calculatePotentialProfessionYieldAmount(pUnit->getProfession(), pUnit, false);
						if (availableAmountOfYield == 0 && bPrintWarning)
						{
							CvWString szBuffer = gDLL->getText("TXT_KEY_NO_RAW_ON_FIELD", getNameKey(),GC.getYieldInfo(eYieldProduced).getChar());
							gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(eYieldProduced).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);
						}
					}
				}
			}
		}
		// TAC - Messages PART 2 - Ray - END
		for (uint i = 0; i < m_aPopulationUnits.size(); ++i)
		{
			bool bFound = false;
			for (int iUnitIndex = 0; iUnitIndex < (int)m_aPopulationUnits.size(); ++iUnitIndex)
			{
				CvUnit* pUnit = m_aPopulationUnits[iUnitIndex];
				if (aiYieldsAvailable[iUnitIndex] > 0)
				{
					CvProfessionInfo& kProfession = GC.getProfessionInfo(pUnit->getProfession());
					//case: two yields are consumed, one yield produced
					if (kProfession.getNumYieldsConsumed() == 2)
					{
						YieldTypes eYieldConsumed = (YieldTypes) kProfession.getYieldsConsumed(0);
						YieldTypes eYieldConsumed2 = (YieldTypes) kProfession.getYieldsConsumed(1);
						int iYieldStored = aiYields[eYieldConsumed];
						int iYieldStored2 = aiYields[eYieldConsumed2];
						if (iYieldStored < 0 || iYieldStored2 < 0)
						{
							// TAC - Messages - Ray - START
							/*
							if (bPrintWarning)
							{
								setUnsatisfiedProfessions.insert(pUnit->getProfession());
							}
							*/
							
							if (bPrintWarning && (aiProducedYields[eYieldConsumed] == 0 || aiProducedYields[eYieldConsumed2] == 0))
							{
								setUnsatisfiedProfessions.insert(pUnit->getProfession());
							}
							if (bPrintWarning && (aiProducedYields[eYieldConsumed] > 0 || aiProducedYields[eYieldConsumed2] > 0))
							{
								setBoredProfessions.insert(pUnit->getProfession());
							}
							// TAC - Messages - Ray - END
							bFound = true;
							YieldTypes eYieldProduced = (YieldTypes) kProfession.getYieldsProduced(0);
							// in the case we have two yields consumed, there is no case with two yields produced
							if (eYieldProduced != NO_YIELD)
							{
								int iDeficit1 = std::min(-iYieldStored, aiYieldsAvailable[iUnitIndex]); // consumed yield 1
								int iDeficit2 = std::min(-iYieldStored2, aiYieldsAvailable[iUnitIndex]); // consumed yield 2

								// R&R, fix for moving goods
								//int iDeficit = std::min(iDeficit1, iDeficit2); // get the smaller of theses
								int iDeficit = std::max(iDeficit1, iDeficit2);

								aiYieldsAvailable[iUnitIndex] -= iDeficit;
								aiProducedYields[eYieldProduced] -= iDeficit;
								aiConsumedYields[eYieldConsumed] -= iDeficit;
								aiConsumedYields[eYieldConsumed2] -= iDeficit;
								aiYields[eYieldProduced] = getYieldStored(eYieldProduced) - aiConsumedYields[eYieldProduced] + aiProducedYields[eYieldProduced] * getBaseYieldRateModifier(eYieldProduced) / 100;
								aiYields[eYieldConsumed] = getYieldStored(eYieldConsumed) - aiConsumedYields[eYieldConsumed] + aiProducedYields[eYieldConsumed] * getBaseYieldRateModifier(eYieldConsumed) / 100;
								aiYields[eYieldConsumed2] = getYieldStored(eYieldConsumed2) - aiConsumedYields[eYieldConsumed2] + aiProducedYields[eYieldConsumed2] * getBaseYieldRateModifier(eYieldConsumed2) / 100;
							}
							else
							{
								FAssertMsg(false, "Could not find matching production for negative yield rate.");
							}
						}
						// TAC - Messages - Ray - START
						else if (((iYieldStored + aiProducedYields[eYieldConsumed] * getBaseYieldRateModifier(eYieldConsumed) / 100 * 2) < (aiConsumedYields[(YieldTypes) kProfession.getYieldsConsumed(0)] * 2)) && (iYieldStored > 0) ||
							((iYieldStored2 + aiProducedYields[eYieldConsumed2] * getBaseYieldRateModifier(eYieldConsumed2) / 100 * 2) < (aiConsumedYields[(YieldTypes) kProfession.getYieldsConsumed(1)] * 2)) && (iYieldStored2 > 0))
						{
							if (bPrintWarning)
							{
								setAlmostUnsatisfiedProfessions.insert(pUnit->getProfession());
							}
						}
						// TAC - Messages - Ray - END
					}

					//case: one yield is consumed
					else if (kProfession.getNumYieldsConsumed() == 1)
					{
						YieldTypes eYieldConsumed = (YieldTypes) kProfession.getYieldsConsumed(0);
						int iYieldStored = aiYields[eYieldConsumed];
						if (iYieldStored < 0)
						{
							// TAC - Messages - Ray - START
							/*
							if (bPrintWarning)
							{
								setUnsatisfiedProfessions.insert(pUnit->getProfession());
							}
							*/
							
							if (bPrintWarning && aiProducedYields[eYieldConsumed] == 0)
							{
								setUnsatisfiedProfessions.insert(pUnit->getProfession());
							}
							if (bPrintWarning && aiProducedYields[eYieldConsumed] > 0)
							{
								setBoredProfessions.insert(pUnit->getProfession());
							}
							// TAC - Messages - Ray - END
							bFound = true;
							//case: two yields are produced
							if (kProfession.getNumYieldsProduced() == 2)
							{
								YieldTypes eYieldProduced = (YieldTypes) kProfession.getYieldsProduced(0);
								YieldTypes eYieldProduced2 = (YieldTypes) kProfession.getYieldsProduced(1);
								int iDeficit = std::min(-iYieldStored, aiYieldsAvailable[iUnitIndex]);
								aiYieldsAvailable[iUnitIndex] -= iDeficit;
								aiConsumedYields[eYieldConsumed] -= iDeficit;
								aiProducedYields[eYieldProduced] -= iDeficit;
								aiProducedYields[eYieldProduced2] -= iDeficit;
								aiYields[eYieldProduced] = getYieldStored(eYieldProduced) - aiConsumedYields[eYieldProduced] + aiProducedYields[eYieldProduced] * getBaseYieldRateModifier(eYieldProduced) / 100;
								aiYields[eYieldProduced2] = getYieldStored(eYieldProduced2) - aiConsumedYields[eYieldProduced2] + aiProducedYields[eYieldProduced2] * getBaseYieldRateModifier(eYieldProduced2) / 100;
								aiYields[eYieldConsumed] = getYieldStored(eYieldConsumed) - aiConsumedYields[eYieldConsumed] + aiProducedYields[eYieldConsumed] * getBaseYieldRateModifier(eYieldConsumed) / 100;
							}
							//case: one yield is produced (Standard)
							else if (kProfession.getNumYieldsProduced() == 1)
							{
								YieldTypes eYieldProduced = (YieldTypes) kProfession.getYieldsProduced(0);
								int iDeficit = std::min(-iYieldStored, aiYieldsAvailable[iUnitIndex]);
								aiYieldsAvailable[iUnitIndex] -= iDeficit;
								aiConsumedYields[eYieldConsumed] -= iDeficit;
								aiProducedYields[eYieldProduced] -= iDeficit;
								aiYields[eYieldProduced] = getYieldStored(eYieldProduced) - aiConsumedYields[eYieldProduced] + aiProducedYields[eYieldProduced] * getBaseYieldRateModifier(eYieldProduced) / 100;
								aiYields[eYieldConsumed] = getYieldStored(eYieldConsumed) - aiConsumedYields[eYieldConsumed] + aiProducedYields[eYieldConsumed] * getBaseYieldRateModifier(eYieldConsumed) / 100;
							}
							else
							{
								FAssertMsg(false, "Could not find matching production for negative yield rate.");
							}
						}
						// TAC - Messages - Ray - START
						else if (((iYieldStored + aiProducedYields[eYieldConsumed] * getBaseYieldRateModifier(eYieldConsumed) / 100 * 2) < (aiConsumedYields[(YieldTypes) kProfession.getYieldsConsumed(0)] * 2)) && (iYieldStored > 0))
						{
							if (bPrintWarning)
							{
								setAlmostUnsatisfiedProfessions.insert(pUnit->getProfession());
							}
						}
						// TAC - Messages - Ray - END
					}

				}
			}

			if(!bFound)
			{
				break;
			}
		}
	}

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		FAssert((iYield == YIELD_FOOD) || (aiYields[iYield] >= 0));
		aiYields[iYield] -= getYieldStored((YieldTypes) iYield);
	}

	// Immigration
	int iImmigration = 0;
	YieldTypes eImmigrationYield = GET_PLAYER(getOwnerINLINE()).getImmigrationConversion();
	if (eImmigrationYield != YIELD_CROSSES)
	{
		aiYields[eImmigrationYield] += aiYields[YIELD_CROSSES];
	}
	
	for (std::set<ProfessionTypes>::iterator it = setUnsatisfiedProfessions.begin(); it != setUnsatisfiedProfessions.end(); ++it)
	{
		CvProfessionInfo& kProfession = GC.getProfessionInfo(*it);
		if(kProfession.getNumYieldsConsumed() > 0)
		{
			YieldTypes eYieldConsumed1 = (YieldTypes) kProfession.getYieldsConsumed(0);
			YieldTypes eYieldProduced = (YieldTypes) kProfession.getYieldsProduced(0);
			if (eYieldConsumed1 != NO_YIELD)
			{
				YieldTypes eYieldConsumed = eYieldConsumed1;
				if(kProfession.getNumYieldsConsumed() == 2)
				{
					YieldTypes eYieldConsumed2 = (YieldTypes) kProfession.getYieldsConsumed(1);
					if (eYieldConsumed2 != NO_YIELD && (aiYields[eYieldConsumed2] < aiYields[eYieldConsumed]))
					{
						 eYieldConsumed = eYieldConsumed2;
					}
				}
				CvWString szBuffer = gDLL->getText("TXT_KEY_NO_RAW", getNameKey(),GC.getYieldInfo(eYieldConsumed).getChar(), GC.getYieldInfo(eYieldProduced).getChar());
				gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(eYieldConsumed).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);
			}
		}
	}

	// TAC - Messages - Ray - START
	for (std::set<ProfessionTypes>::iterator it = setAlmostUnsatisfiedProfessions.begin(); it != setAlmostUnsatisfiedProfessions.end(); ++it)
	{
		CvProfessionInfo& kProfession = GC.getProfessionInfo(*it);
		if(kProfession.getNumYieldsConsumed() > 0)
		{
			YieldTypes eYieldConsumed1 = (YieldTypes) kProfession.getYieldsConsumed(0);
			YieldTypes eYieldProduced = (YieldTypes) kProfession.getYieldsProduced(0);
			if (eYieldConsumed1 != NO_YIELD)
			{
				YieldTypes eYieldConsumed = eYieldConsumed1;
				if(kProfession.getNumYieldsConsumed() == 2)
				{
					YieldTypes eYieldConsumed2 = (YieldTypes) kProfession.getYieldsConsumed(1);
					if (eYieldConsumed2 != NO_YIELD && (aiYields[eYieldConsumed2] < aiYields[eYieldConsumed]))
					{
						eYieldConsumed = eYieldConsumed2;
					}
				}
				CvWString szBuffer = gDLL->getText("TXT_KEY_ALMOST_NO_RAW", getNameKey(),GC.getYieldInfo(eYieldConsumed).getChar(), GC.getYieldInfo(eYieldProduced).getChar());
				gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(eYieldConsumed).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);
			}
		}
	}

	for (std::set<ProfessionTypes>::iterator it = setBoredProfessions.begin(); it != setBoredProfessions.end(); ++it)
	{
		CvProfessionInfo& kProfession = GC.getProfessionInfo(*it);
		if(kProfession.getNumYieldsConsumed() > 0)
		{
			YieldTypes eYieldConsumed1 = (YieldTypes) kProfession.getYieldsConsumed(0);
			YieldTypes eYieldProduced = (YieldTypes) kProfession.getYieldsProduced(0);
			if (eYieldConsumed1 != NO_YIELD)
			{
				YieldTypes eYieldConsumed = eYieldConsumed1;
				if(kProfession.getNumYieldsConsumed() == 2)
				{
					YieldTypes eYieldConsumed2 = (YieldTypes) kProfession.getYieldsConsumed(1);
					if (eYieldConsumed2 != NO_YIELD && (aiYields[eYieldConsumed2] < aiYields[eYieldConsumed]))
					{
						 eYieldConsumed = eYieldConsumed2;
					}
					CvWString szBuffer = gDLL->getText("TXT_KEY_NOT_ENOUGH_RAW", getNameKey(),GC.getYieldInfo(eYieldConsumed).getChar(), GC.getYieldInfo(eYieldProduced).getChar());
					gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(eYieldConsumed).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);
				}
			}
		}
	}
	// TAC - Messages - Ray - END
}
// R&R, ray , MYCP partially based on code of Aymerick - END

int CvCity::calculateNetYield(YieldTypes eYield) const
{
	FAssert(eYield < NUM_YIELD_TYPES && eYield >= 0);
	int aiYields[NUM_YIELD_TYPES];
	calculateNetYields(aiYields);
	return aiYields[eYield];
}

int CvCity::calculateActualYieldProduced(YieldTypes eYield) const
{
	FAssert(eYield < NUM_YIELD_TYPES && eYield >= 0);
	int aiYields[NUM_YIELD_TYPES];
	int aiYieldsProduced[NUM_YIELD_TYPES];
	calculateNetYields(aiYields, aiYieldsProduced);
	return aiYieldsProduced[eYield];
}

int CvCity::calculateActualYieldConsumed(YieldTypes eYield) const
{
	FAssert(eYield < NUM_YIELD_TYPES && eYield >= 0);
	int aiYields[NUM_YIELD_TYPES];
	int aiYieldsProduced[NUM_YIELD_TYPES];
	int aiYieldsConsumed[NUM_YIELD_TYPES];
	calculateNetYields(aiYields, aiYieldsProduced, aiYieldsConsumed);
	return aiYieldsConsumed[eYield];
}

void CvCity::initCacheStorageLossTradeValues()
{
	m_iStorageLossSellPercentage = 0;
	m_bIgnoresBoycott = false;
	m_bHasUnlockedStorageLossTradeSettings = false;
}

/*
	void CvCity::cache_storageLossTradeValues_usingCachedData(BuildingTypes eBuilding)
	Complexity: O( 1 )
	Purpose:
		Cache new values for customs related variables, if the given building has better stats.
*/
void CvCity::cache_storageLossTradeValues_usingCachedData(BuildingTypes eBuilding)
{
	FAssert(eBuilding >= 0);
	FAssert(eBuilding <= GC.getNumBuildingInfos());

	//Caching storage loss trade values for natives makes no sense, as they never do storage loss selling. But the check if this is a native city has to be done outside of this function.

	//get a reference to the building object
	CvBuildingInfo &refBuildingInfo = GC.getBuildingInfo(eBuilding);

	//Use values for sell percantage, boycott ignoring and unlocking the trade settings, if they are better than the current ones.

	int i_building_StorageLossSellPercentage = refBuildingInfo.getStorageLossSellPercentage();
	if (i_building_StorageLossSellPercentage > m_iStorageLossSellPercentage) { m_iStorageLossSellPercentage = i_building_StorageLossSellPercentage; }

	m_bIgnoresBoycott = m_bIgnoresBoycott || refBuildingInfo.getIgnoresBoycott();
	m_bHasUnlockedStorageLossTradeSettings = m_bHasUnlockedStorageLossTradeSettings || refBuildingInfo.getUnlocksStorageLossTradeSettings();
}

/*
	void CvCity::cache_storageLossTradeValues_usingRawData()
	Complexity: O( n^2 )
	Complexity detailed: O( n*m )
		n ... from function isHasBuilding
		m ... number of BuildingClassInfos
	Purpose:
		Cache customs related values from the raw data.
	Usage:
		When there is no other way to determine the new values of the customs related variables from the cached values and the input.
	Description:
		Assumes higher tier buildings have better stats than their lower tier replacements.
*/
void CvCity::cache_storageLossTradeValues_usingRawData()
{
	//Caching storage loss trade values for natives makes no sense, as they never do storage loss selling. But the check if this is a native city has to be done outside of this function.

	//Init cache
	initCacheStorageLossTradeValues();

	if (!isHuman())
	{
		//If it is an AI player, ...

		//... apply the minimum values derived from the human players handicap level ...
		m_iStorageLossSellPercentage = GC.getHandicapInfo(getHandicapType()).getAIMinimumStorageLossSellPercentage();
	}

	//For human and AI players alike:
	//Iterate over all buildings buildable by this civilization, ...
	int iNumBuildingClasses = GC.getNumBuildingClassInfos();
	for (int i = 0; i < iNumBuildingClasses; i++)
	{
		BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(i);
		//... check if the iterator building is present in the city and if it is highest tier building in its building slot ...
		if (eBuilding != NO_BUILDING && isHasBuilding(eBuilding))
		{
			//... and apply the values from that building, if they are better than the current values.
			cache_storageLossTradeValues_usingCachedData(eBuilding);
		}
	}
}

/*
	void CvCity::updateCacheStorageLossTradingValues(BuildingTypes eBuilding, bool bWasAdded)
	Complexity: O( n^2 )
	Complexity detailed:
		n^2			... from cache_storageLossTradeValues_usingRawData()
	Paramaters:
		eBuilding	...	Enumerator/index of the building.
		bWasAdded	...	true if the function was called because a building was added. false if removed.
	Purpose:
		To update the cache of storage loss trade related values, after a building was added or removed to or from the city.
*/
void CvCity::updateCacheStorageLossTradingValues(BuildingTypes eBuilding, bool bWasAdded)
{
	//Only do this if it is a non native city, because natives never sell storage loss.
	if (!isNative())
	{
		//Determine how we are going to update the cache.
		if (bWasAdded)
		{
			//The building was added ...
			// ... see if its values are better than the current ones.
			cache_storageLossTradeValues_usingCachedData(eBuilding);
		}
		else
		{
			//The building was removed.
			//As we only know the best values, but we don't know which building(s) gave us these best values, we have to rebuild the cache from scratch.
			cache_storageLossTradeValues_usingRawData();
		}
	}
}

bool CvCity::isEverOwned(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex expected to be < MAX_PLAYERS");
	return m_abEverOwned[eIndex];
}


void CvCity::setEverOwned(PlayerTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex expected to be < MAX_PLAYERS");
	m_abEverOwned[eIndex] = bNewValue;
}


bool CvCity::isRevealed(TeamTypes eIndex, bool bDebug) const
{
	if (bDebug && GC.getGameINLINE().isDebugMode())
	{
		return true;
	}
	else
	{
		FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
		FAssertMsg(eIndex < MAX_TEAMS, "eIndex expected to be < MAX_TEAMS");

		return m_abRevealed[eIndex];
	}
}


void CvCity::setRevealed(TeamTypes eIndex, bool bNewValue)
{
	CvPlot* pLoopPlot;
	int iI;

	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex expected to be < MAX_TEAMS");

	if (isRevealed(eIndex, false) != bNewValue)
	{
		m_abRevealed[eIndex] = bNewValue;

		updateVisibility();

		if (eIndex == GC.getGameINLINE().getActiveTeam())
		{
			for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
			{
				pLoopPlot = getCityIndexPlot(iI);

				if (pLoopPlot != NULL)
				{
					pLoopPlot->updateSymbols();
				}
			}
		}
	}
}


const wchar* CvCity::getNameKey() const
{
	return m_szName;
}


const CvWString CvCity::getName(uint uiForm) const
{
	return gDLL->getObjectText(m_szName, uiForm, true);
}


void CvCity::setName(const wchar* szNewValue, bool bFound)
{
	CvWString szName(szNewValue);
	gDLL->stripSpecialCharacters(szName);

	if (!isEmpty(szName))
	{
		if (GET_PLAYER(getOwnerINLINE()).isCityNameValid(szName, false))
		{
			m_szName = szName;

			setBillboardDirty(true);

			if (isCitySelected())
			{
				gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
			}
		}
		if (bFound)
		{
			doFoundMessage();
		}
	}
}


void CvCity::doFoundMessage()
{
	CvWString szBuffer;

	szBuffer = gDLL->getText("TXT_KEY_MISC_CITY_HAS_BEEN_FOUNDED", getNameKey());
	gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, -1, szBuffer, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), MESSAGE_TYPE_MAJOR_EVENT, NULL, NO_COLOR, getX_INLINE(), getY_INLINE());

	szBuffer = gDLL->getText("TXT_KEY_MISC_CITY_IS_FOUNDED", getNameKey());
	GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_CITY_FOUNDED, getOwnerINLINE(), szBuffer, getX_INLINE(), getY_INLINE(), (ColorTypes)GC.getInfoTypeForString("COLOR_ALT_HIGHLIGHT_TEXT"));
}


std::string CvCity::getScriptData() const
{
	return m_szScriptData;
}


void CvCity::setScriptData(std::string szNewValue)
{
	m_szScriptData = szNewValue;
}


int CvCity::getBuildingProduction(BuildingTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");
	return m_paiBuildingProduction[eIndex];
}


void CvCity::setBuildingProduction(BuildingTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");

	if (getBuildingProduction(eIndex) != iNewValue)
	{
		m_paiBuildingProduction[eIndex] = iNewValue;
		FAssert(getBuildingProduction(eIndex) >= 0);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setBillboardDirty(true);
		}

		if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(Domestic_Advisor_DIRTY_BIT, true);
		}
	}
}


void CvCity::changeBuildingProduction(BuildingTypes eIndex, int iChange)
{
	setBuildingProduction(eIndex, (getBuildingProduction(eIndex) + iChange));
}


int CvCity::getBuildingProductionTime(BuildingTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");
	return m_paiBuildingProductionTime[eIndex];
}


void CvCity::setBuildingProductionTime(BuildingTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");
	m_paiBuildingProductionTime[eIndex] = iNewValue;
	FAssert(getBuildingProductionTime(eIndex) >= 0);
}


void CvCity::changeBuildingProductionTime(BuildingTypes eIndex, int iChange)
{
	setBuildingProductionTime(eIndex, (getBuildingProductionTime(eIndex) + iChange));
}


int CvCity::getBuildingOriginalOwner(BuildingTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");
	return m_paiBuildingOriginalOwner[eIndex];
}


int CvCity::getBuildingOriginalTime(BuildingTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");
	return m_paiBuildingOriginalTime[eIndex];
}


int CvCity::getUnitProduction(UnitTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");
	return m_paiUnitProduction[eIndex];
}


void CvCity::setUnitProduction(UnitTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");

	if (getUnitProduction(eIndex) != iNewValue)
	{
		m_paiUnitProduction[eIndex] = iNewValue;
		FAssert(getUnitProduction(eIndex) >= 0);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			setBillboardDirty(true);
		}

		if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(Domestic_Advisor_DIRTY_BIT, true);
		}
	}
}


void CvCity::changeUnitProduction(UnitTypes eIndex, int iChange)
{
	setUnitProduction(eIndex, (getUnitProduction(eIndex) + iChange));
}


int CvCity::getUnitProductionTime(UnitTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");
	return m_paiUnitProductionTime[eIndex];
}


void CvCity::setUnitProductionTime(UnitTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");
	m_paiUnitProductionTime[eIndex] = iNewValue;
	FAssert(getUnitProductionTime(eIndex) >= 0);
}


void CvCity::changeUnitProductionTime(UnitTypes eIndex, int iChange)
{
	setUnitProductionTime(eIndex, (getUnitProductionTime(eIndex) + iChange));
}


int CvCity::getUnitCombatFreeExperience(UnitCombatTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitCombatInfos(), "eIndex expected to be < GC.getNumUnitCombatInfos()");
	return m_paiUnitCombatFreeExperience[eIndex];
}


void CvCity::changeUnitCombatFreeExperience(UnitCombatTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitCombatInfos(), "eIndex expected to be < GC.getNumUnitCombatInfos()");
	m_paiUnitCombatFreeExperience[eIndex] = (m_paiUnitCombatFreeExperience[eIndex] + iChange);
	FAssert(getUnitCombatFreeExperience(eIndex) >= 0);
}


int CvCity::getFreePromotionCount(PromotionTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumPromotionInfos(), "eIndex expected to be < GC.getNumPromotionInfos()");
	return m_paiFreePromotionCount[eIndex];
}


bool CvCity::isFreePromotion(PromotionTypes eIndex) const
{
	return (getFreePromotionCount(eIndex) > 0);
}


void CvCity::changeFreePromotionCount(PromotionTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumPromotionInfos(), "eIndex expected to be < GC.getNumPromotionInfos()");
	m_paiFreePromotionCount[eIndex] = (m_paiFreePromotionCount[eIndex] + iChange);
	FAssert(getFreePromotionCount(eIndex) >= 0);
}


CvUnit* CvCity::getUnitWorkingPlot(int iPlotIndex) const
{
	FAssertMsg(iPlotIndex >= 0, "iPlotIndex expected to be >= 0");
	FAssertMsg(iPlotIndex < NUM_CITY_PLOTS, "iPlotIndex expected to be < NUM_CITY_PLOTS");

	int iUnitId = m_paiWorkingPlot[iPlotIndex];
	if (-1 != iUnitId)
	{
		return getPopulationUnitById(iUnitId);
	}

	return NULL;
}


bool CvCity::isUnitWorkingPlot(int iPlotIndex) const
{
	FAssertMsg(iPlotIndex >= 0, "iPlotIndex expected to be >= 0");
	FAssertMsg(iPlotIndex < NUM_CITY_PLOTS, "iPlotIndex expected to be < NUM_CITY_PLOTS");

	if (CITY_HOME_PLOT == iPlotIndex)
	{
		return true;
	}

	if (-1 != m_paiWorkingPlot[iPlotIndex])
	{
		return true;
	}

	return false;
}


bool CvCity::isUnitWorkingPlot(const CvPlot* pPlot) const
{
	int iIndex = getCityPlotIndex(pPlot);

	if (iIndex != -1)
	{
		return isUnitWorkingPlot(iIndex);
	}

	return false;
}

bool CvCity::isUnitWorkingAnyPlot(const CvUnit* pUnit) const
{
	for(int i=0;i<NUM_CITY_PLOTS;i++)
	{
		int iUnitId = m_paiWorkingPlot[i];
		if(iUnitId != -1)
		{
			if(getPopulationUnitById(iUnitId) == pUnit)
			{
				return true;
			}
		}
	}

	return false;
}

CvUnit* CvCity::getUnitWorkingPlot(const CvPlot* pPlot) const
{
	int iIndex = getCityPlotIndex(pPlot);

	if (iIndex != -1)
	{
		return getUnitWorkingPlot(iIndex);
	}

	return NULL;
}


void CvCity::setUnitWorkingPlot(int iPlotIndex, int iUnitId)
{
	FAssertMsg(iPlotIndex >= 0, "iPlotIndex expected to be >= 0");
	FAssertMsg(iPlotIndex < NUM_CITY_PLOTS, "iPlotIndex expected to be < NUM_CITY_PLOTS");
	FAssertMsg(iUnitId >= 0, "iUnitId expected to be >= 0");
	FAssert(iPlotIndex != CITY_HOME_PLOT);

	if (isUnitWorkingPlot(iPlotIndex))
	{
		FAssertMsg(false, "Trying to work a plot that's already being worked");
		return;
	}

	CvPlot* pPlot = getCityIndexPlot(iPlotIndex);

	if (pPlot != NULL)
	{
		//make sure unit is outdoor profession
		CvUnit* pUnit = getPopulationUnitById(iUnitId);
		FAssert(pUnit != NULL);
		if (pUnit == NULL)
		{
			return;
		}
		ProfessionTypes eUnitProfession = pUnit->getProfession();
		
		FAssert(pUnit->isColonistLocked() || !isUnitWorkingAnyPlot(pUnit));
		if (pUnit->isColonistLocked())
		{
			//assign profession that produces yields
			if((NO_PROFESSION == eUnitProfession) || !GC.getProfessionInfo(eUnitProfession).isWorkPlot() || pPlot->calculatePotentialProfessionYieldAmount(eUnitProfession, pUnit, false) == 0)
			{
				ProfessionTypes eBestProfession = NO_PROFESSION;
				int iBestYieldAmount = 0;
				for(int i=0;i<GC.getNumProfessionInfos();i++)
				{
					ProfessionTypes eLoopProfession = (ProfessionTypes) i;
					if (GET_PLAYER(getOwnerINLINE()).isProfessionValid(eLoopProfession, pUnit->getUnitType()))
					{
						if(GC.getProfessionInfo(eLoopProfession).isWorkPlot())
						{
							int iLoopYieldAmount = pPlot->calculatePotentialProfessionYieldAmount(eLoopProfession, pUnit, false);
							if(iLoopYieldAmount > iBestYieldAmount)
							{
								eBestProfession = eLoopProfession;
								iBestYieldAmount = iLoopYieldAmount;
							}
						}
					}
				}

				// Erik: No assert since it is possible that a unit would produce 0 yield in any plot profession 
				//FAssert(eBestProfession != NO_PROFESSION);
				if(eBestProfession != NO_PROFESSION)
				{
					pUnit->setProfession(eBestProfession);
				}
			}

			//unassign current worked plot
			CvPlot* pCurrentWorkedPlot = getPlotWorkedByUnit(pUnit);
			if(pCurrentWorkedPlot != NULL)
			{
				clearUnitWorkingPlot(pCurrentWorkedPlot);
			}
		}
		
		m_paiWorkingPlot[iPlotIndex] = iUnitId;

		FAssertMsg(pPlot->getWorkingCity() == this, "WorkingCity is expected to be this");

		pPlot->updateYield(false);

		setYieldRateDirty();

		pPlot->updatePlotBuilder();

		if ((getTeam() == GC.getGameINLINE().getActiveTeam()) || GC.getGameINLINE().isDebugMode())
		{
			pPlot->updateSymbolDisplay();
		}	
		
		if (isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true );
			gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(ColoredPlots_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(CitizenButtons_DIRTY_BIT, true);
		}
	}
}

void CvCity::clearUnitWorkingPlot(int iPlotIndex)
{
	FAssertMsg(iPlotIndex >= 0, "iPlotIndex expected to be >= 0");
	FAssertMsg(iPlotIndex < NUM_CITY_PLOTS, "iPlotIndex expected to be < NUM_CITY_PLOTS");
	FAssert(iPlotIndex != CITY_HOME_PLOT);

	if (!isUnitWorkingPlot(iPlotIndex))
	{
		return;
	}

	CvPlot* pPlot = getCityIndexPlot(iPlotIndex);
	if (pPlot != NULL)
	{
		m_paiWorkingPlot[iPlotIndex] = -1;
		
		FAssertMsg(pPlot->getWorkingCity() == this, "WorkingCity is expected to be this");

		setYieldRateDirty();

		pPlot->updateYield(false);

		pPlot->updatePlotBuilder();

		if ((getTeam() == GC.getGameINLINE().getActiveTeam()) || GC.getGameINLINE().isDebugMode())
		{
			pPlot->updateSymbolDisplay();
		}	

		if (isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true );
			gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(ColoredPlots_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(CitizenButtons_DIRTY_BIT, true);
		}
	}
}

void CvCity::clearUnitWorkingPlot(CvPlot* pPlot)
{
	clearUnitWorkingPlot(getCityPlotIndex(pPlot));
}


void CvCity::setUnitWorkingPlot(const CvPlot* pPlot, int iUnitId)
{
	setUnitWorkingPlot(getCityPlotIndex(pPlot), iUnitId);
}


void CvCity::alterUnitWorkingPlot(int iPlotIndex, int iUnitId, bool bAskProfession)
{
	FAssertMsg(iPlotIndex >= 0, "iIndex expected to be >= 0");
	FAssertMsg(iPlotIndex < NUM_CITY_PLOTS, "iIndex expected to be < NUM_CITY_PLOTS");

	if (iPlotIndex == CITY_HOME_PLOT)
	{
		return;
	}

	CvPlot* pPlot = getCityIndexPlot(iPlotIndex);
	if (pPlot == NULL)
	{
		return;
	}

	CvUnit* pUnit = NULL;
	if (iUnitId != -1)
	{
		pUnit = getPopulationUnitById(iUnitId);
		if (pUnit == NULL)
		{
			pUnit = GET_PLAYER(getOwnerINLINE()).getUnit(iUnitId);
			if (pUnit != NULL)
			{
				if (!pUnit->canJoinCity(plot()))
				{
					return;
				}
				addPopulationUnit(pUnit, NO_PROFESSION);
			}
		}
	}

	if (!canWork(pPlot) && pPlot->getOwnerINLINE() == getOwnerINLINE() && pPlot->getWorkingCityOverride() != this)
	{
		pPlot->setWorkingCityOverride(this);
	}

	if (canWork(pPlot))
	{
		if (pUnit != NULL)
		{
			pUnit->setColonistLocked(true);
		}

		CvUnit* pUnitWorkingPlot = getUnitWorkingPlot(iPlotIndex);
		if (NULL != pUnitWorkingPlot)
		{
			clearUnitWorkingPlot(iPlotIndex);
		}
		else //not worked
		{
			FAssert(iUnitId != -1);
			setUnitWorkingPlot(iPlotIndex, iUnitId);
		}

		if (isHuman() && bAskProfession)
		{
			if (pUnit != NULL)
			{
				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CHOOSE_PROFESSION, getID(), pUnit->getID(), 1);
				gDLL->getInterfaceIFace()->addPopup(pInfo, getOwnerINLINE(), true, true);
			}
		}
	}
}

//Androrc Multiple Professions per Building
void CvCity::alterUnitWorkingBuilding(BuildingTypes eBuilding, int iUnitId, bool bAskProfession)
{
	FAssertMsg(eBuilding >= 0, "eBuilding expected to be >= 0");
	FAssertMsg(eBuilding < GC.getNumBuildingInfos(), "eBuilding expected to be < GC.getNumBuildingInfos()");

	//shouldn't be used for the AI at least for now; it can already handle multiple professions in a building
	if (!isHuman())
	{
		return;
	}

	if (!isHasBuilding(eBuilding))
	{
		return;
	}

	CvUnit* pUnit = NULL;
	if (iUnitId != -1)
	{
		pUnit = getPopulationUnitById(iUnitId);
		if (pUnit == NULL)
		{
			pUnit = GET_PLAYER(getOwnerINLINE()).getUnit(iUnitId);
			if (pUnit != NULL)
			{
				if (!pUnit->canJoinCity(plot()))
				{
					return;
				}
				addPopulationUnit(pUnit, NO_PROFESSION);
			}
		}
	}

	if (isAvailableBuildingSlot(eBuilding, pUnit))
	{
		if (pUnit != NULL)
		{
			pUnit->setColonistLocked(true);
		}

		//code based on the one in setUnitWorkingPlot()
		FAssert(pUnit->isColonistLocked());
		ProfessionTypes eBestProfession = NO_PROFESSION; //transferred from below
		if (pUnit->isColonistLocked())
		{
			//assign profession that produces yields
			int iBestYieldAmount = 0;
			for(int i=0;i<GC.getNumProfessionInfos();i++)
			{
				ProfessionTypes eLoopProfession = (ProfessionTypes) i;
				if (GET_PLAYER(getOwnerINLINE()).isProfessionValid(eLoopProfession, pUnit->getUnitType()) && pUnit->canHaveProfession(eLoopProfession, true))
				{
					if(GC.getProfessionInfo(eLoopProfession).getSpecialBuilding() == GC.getBuildingInfo(eBuilding).getSpecialBuildingType())
					{
						int iLoopYieldAmount = getProfessionOutput(eLoopProfession, pUnit);
						// R&R, ray , MYCP partially based on code of Aymerick - START
						if (GC.getProfessionInfo(eLoopProfession).getYieldsConsumed(0) != NO_YIELD)
						// R&R, ray , MYCP partially based on code of Aymerick - END
						{
							// R&R, ray , MYCP partially based on code of Aymerick - START
							int iUnproducedAmount = (getYieldStored((YieldTypes) GC.getProfessionInfo(eLoopProfession).getYieldsConsumed(0)) + getYieldRate((YieldTypes) GC.getProfessionInfo(eLoopProfession).getYieldsConsumed(0))) - getProfessionInput(eLoopProfession, pUnit);
							// R&R, ray , MYCP partially based on code of Aymerick - END
							if (iUnproducedAmount < 0)
							{
								iLoopYieldAmount += (iUnproducedAmount / (getProfessionInput(eLoopProfession, pUnit) / getProfessionOutput(eLoopProfession, pUnit)));
								//Androrc Multiple Professions per Building 1.3 Start
								if (iLoopYieldAmount < 0)
								{
									iLoopYieldAmount = 0;
								}
								//Androrc Multiple Professions per Building 1.3 End
							}
						}
						if(iLoopYieldAmount >= iBestYieldAmount)
						{
							eBestProfession = eLoopProfession;
							iBestYieldAmount = iLoopYieldAmount;
						}
					}
				}
			}
                        //deleted assert here that no longer served a purpose
			if(eBestProfession != NO_PROFESSION)
			{
				pUnit->setProfession(eBestProfession);
			}
			else
			{
				pUnit->setColonistLocked(false);
			}
		}

		if (isHuman() && bAskProfession && eBestProfession != NO_PROFESSION)
		{
			if (pUnit != NULL)
			{
				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CHOOSE_PROFESSION, getID(), pUnit->getID(), 2);
				gDLL->getInterfaceIFace()->addPopup(pInfo, getOwnerINLINE(), true, true);
			}
		}
	}
}
//Androrc End

void CvCity::alterUnitProfession(int iUnitId, ProfessionTypes eProfession)
{
	CvUnit* pUnit = getPopulationUnitById(iUnitId);
	if(pUnit != NULL)
	{
		if (pUnit->canHaveProfession(eProfession, false))
		{
			pUnit->setColonistLocked(true);
			pUnit->setProfession(eProfession);
			
			if (GC.getProfessionInfo(eProfession).isWorkPlot())
			{
				if (!isUnitWorkingAnyPlot(pUnit))
				{
					int iBestPlot = AI_bestProfessionPlot(eProfession, pUnit);
					if (iBestPlot != -1)
					{
						FAssert(!isUnitWorkingPlot(iBestPlot));
						setUnitWorkingPlot(iBestPlot, iUnitId);
					}
				}
			}
		}
	}
}

void CvCity::ejectToTransport(int iUnitId, int iTransportId)
{
	CvUnit* pUnit = getPopulationUnitById(iUnitId);
	CvUnit* pTransport = GET_PLAYER(getOwnerINLINE()).getUnit(iTransportId);

	if (pUnit != NULL && pTransport != NULL)
	{
		if (pUnit->canLoadUnit(pTransport, pUnit->plot(), true))
		{
			if (removePopulationUnit(pUnit, false, (ProfessionTypes) GC.getCivilizationInfo(GET_PLAYER(getOwnerINLINE()).getCivilizationType()).getDefaultProfession()))
			{
				pUnit->loadUnit(pTransport);
			}
		}
	}	
}

void CvCity::replaceCitizen(int iUnitId, int iReplacedUnitId, bool bAskProfession)
{
	CvUnit* pUnit = getPopulationUnitById(iUnitId);
	if (pUnit == NULL)
	{
		pUnit = GET_PLAYER(getOwnerINLINE()).getUnit(iUnitId);
		// TAC - Clear Specialty Fix - koma13 - START
		//if (pUnit == NULL || !pUnit->canJoinCity(plot()))
		if (pUnit == NULL || !pUnit->canJoinCity(plot(), false, true))
		// TAC - Clear Specialty Fix - koma13 - END
		{
			return;
		}
		addPopulationUnit(pUnit, NO_PROFESSION);
	}

	CvUnit* pReplacedUnit = getPopulationUnitById(iReplacedUnitId);
	if (pReplacedUnit == NULL)
	{
		return;
	}

	CvPlot* pPlot = getPlotWorkedByUnit(pReplacedUnit);
	if (pPlot != NULL)
	{
		pUnit->setColonistLocked(true);
		clearUnitWorkingPlot(pPlot);
		setUnitWorkingPlot(pPlot, iUnitId);
		pReplacedUnit->setColonistLocked(false);
	}
	else
	{
		ProfessionTypes eProfession = pReplacedUnit->getProfession();
		if (pUnit->canHaveProfession(eProfession, true))
		{
			if (!isAvailableProfessionSlot(eProfession, pUnit))
			{
				pReplacedUnit->setProfession(NO_PROFESSION);
				pReplacedUnit->setColonistLocked(false);
			}
			pUnit->setProfession(eProfession);
			pUnit->setColonistLocked(true);
		}
	}

	AI_assignWorkingPlots();

	if (bAskProfession && isHuman())
	{
		//Androrc Multiple Professions per Building
//		CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CHOOSE_PROFESSION, getID(), pUnit->getID(), 1);
//		gDLL->getInterfaceIFace()->addPopup(pInfo, getOwnerINLINE(), true, true);
		if (GC.getProfessionInfo(pUnit->getProfession()).getSpecialBuilding() == NO_SPECIALBUILDING)
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CHOOSE_PROFESSION, getID(), pUnit->getID(), 1);
			gDLL->getInterfaceIFace()->addPopup(pInfo, getOwnerINLINE(), true, true);
		}
		else
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CHOOSE_PROFESSION, getID(), pUnit->getID(), 2);
			gDLL->getInterfaceIFace()->addPopup(pInfo, getOwnerINLINE(), true, true);
		}
		//Androrc End
	}
}

int CvCity::getNextFreeUnitId() const
{
	for (int i = 0; i < (int)m_aPopulationUnits.size(); ++i)
	{
		CvUnit* pUnit = m_aPopulationUnits[i];
		ProfessionTypes eUnitProfession = pUnit->getProfession();
		if (eUnitProfession == NO_PROFESSION) //idle citizen
		{
			return pUnit->getID();
		}
		else if (GC.getProfessionInfo(eUnitProfession).isWorkPlot() && !isUnitWorkingAnyPlot(pUnit)) //outdoor profession, but not actively working land
		{
			return pUnit->getID();
		}
	}

	FAssertMsg(false, "Could not find free unit id.");
	return -1;
}


/*
	bool CvCity::isHasRealBuilding(BuildingTypes eIndex)
	complexity: O( 1 )
	Purpose:
		Check if a building (BuildingType) is present in the city, that had been built in the city
		(in contrast to free buildings that were given to the city as result of an event).
*/
bool CvCity::isHasRealBuilding(BuildingTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");
	return m_pabHasRealBuilding[eIndex];
}


void CvCity::setHasRealBuilding(BuildingTypes eIndex, bool bNewValue)
{
	setHasRealBuildingTimed(eIndex, bNewValue, true, getOwnerINLINE(), GC.getGameINLINE().getGameTurnYear());
}

/*
	void CvCity::setHasRealBuildingTimed(BuildingTypes eIndex, bool bNewValue, bool bFirst, PlayerTypes eOriginalOwner, int iOriginalTime)
	Complexity: ???
	Parameters:
		eIndex ... Index that corresponds to the list of BuildingTypes
		bNewValue ... true if the building is added, false if removed
		bFirst ... true if this function was called, because the building was constructed just now. false if the function was called for other purposes, eg because the city was captured.
		eOriginalOwner ... The variable, that indicates the original owner, will be set to this value.
		iOriginalTime ... The variable, that holds the time of construction, will be set to this value.
	Purpose:
		To add or remove a building, which was constructed in the city, to or from the cities list of buildings.
*/
void CvCity::setHasRealBuildingTimed(BuildingTypes eIndex, bool bNewValue, bool bFirst, PlayerTypes eOriginalOwner, int iOriginalTime)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");

	//Only do something if the building is to be added and not present in the city. Or present and to be removed.
	if (bNewValue != isHasRealBuilding(eIndex))
	{
		//Iterate over all the buildings in the game and build a temporary cache which indicates if a building exists in the city AND has the highest tier in its building slot.
		std::deque<bool> abOldBuildings(GC.getNumBuildingInfos());
		for (int i = 0; i < GC.getNumBuildingInfos(); ++i)
		{
			abOldBuildings[i] = isHasBuilding((BuildingTypes) i);
		}

		//Update the cities array for "real" buildings.
		m_pabHasRealBuilding[eIndex] = bNewValue;
		setYieldRateDirty();

		//Set values for player who built this building and game year depending on if it exists or not.
		if (isHasRealBuilding(eIndex))
		{
			m_paiBuildingOriginalOwner[eIndex] = eOriginalOwner;
			m_paiBuildingOriginalTime[eIndex] = iOriginalTime;
		}
		else
		{
			m_paiBuildingOriginalOwner[eIndex] = NO_PLAYER;
			m_paiBuildingOriginalTime[eIndex] = MIN_INT;
		}

		//Iterate over all the buildings in the game ...
		for (int i = 0; i < GC.getNumBuildingInfos(); ++i)
		{
			BuildingTypes eBuilding = (BuildingTypes) i;

			//... check if there is a difference between the array with the cached building state and the cities array for "real" buildings and if it is the highest tier in its building slot.
			if (abOldBuildings[eBuilding] != isHasBuilding(eBuilding))
			{
				//... and process the iterator building regarding adding or removing
				processBuilding(eBuilding, abOldBuildings[eBuilding] ? -1 : 1);
			}
		}

		//Check if the capital was moved and set something regarding the python interface ???
		if (bNewValue)
		{
			//If a building was added ...

			//... check if it was constructed just now.
			if (bFirst)
			{
				//Check if it is the building, that defines the capital ...
				if (GC.getBuildingInfo(eIndex).isCapital())
				{
					//Set this city to be the capital.
					GET_PLAYER(getOwnerINLINE()).setCapitalCity(this);
				}

				//The following is something unknown, but revealed to the python interface ???
				GC.getGameINLINE().incrementBuildingClassCreatedCount((BuildingClassTypes)(GC.getBuildingInfo(eIndex).getBuildingClassType()));
			}
		}

		// update cache
		UpdateBuildingAffectedCache(); // building affected cache - Nightinggale
		updateCacheStorageLossTradingValues(eIndex, bNewValue);
	}
}


/*
	bool CvCity::isHasFreeBuilding(BuildingTypes eIndex)
	Complexity: O( 1 )
	Purpose:
		Check if a building (BuildingType) that has been given by an event is present in the city.
		Eg a free stockade in every city, when a particular founding father joined the congress.
*/
bool CvCity::isHasFreeBuilding(BuildingTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");
	return m_pabHasFreeBuilding[eIndex];
}


/*
	void CvCity::setHasFreeBuilding(BuildingTypes eIndex, bool bNewValue)
	Complexity: ???
	Parameters:
		eIndex ... Index that corresponds to the list of BuildingTypes
		bNewValue ... true if the building is added, false if removed
	Purpose:
		To add or remove a building, which is given to the city as "free" building, to or from the cities list of "free" buildings.
*/
void CvCity::setHasFreeBuilding(BuildingTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");

	//Only do something if the building is to be added and not present in the city. Or present and to be removed.
	if (isHasFreeBuilding(eIndex) != bNewValue)
	{
		//Iterate over all the buildings in the game and build a temporary cache which indicates if a building exists in the city AND has the highest tier in its building slot.
		std::deque<bool> abOldBuildings(GC.getNumBuildingInfos());
		for (int i = 0; i < GC.getNumBuildingInfos(); ++i)
		{
			abOldBuildings[i] = isHasBuilding((BuildingTypes) i);
		}

		//Update the cities array for "free" buildings.
		m_pabHasFreeBuilding[eIndex] = bNewValue;
		setYieldRateDirty();

		//Iterate over all the buildings in the game ...
		for (int i = 0; i < GC.getNumBuildingInfos(); ++i)
		{
			BuildingTypes eBuilding = (BuildingTypes) i;

			//... check if there is a difference between the array with the cached building state and the cities array for "free" buildings and if it is the highest tier in its building slot.
			if (abOldBuildings[eBuilding] != isHasBuilding(eBuilding))
			{
				//... and process the iterator building regarding adding or removing
				processBuilding(eBuilding, abOldBuildings[eBuilding] ? -1 : 1);
			}
		}
		UpdateBuildingAffectedCache(); // building affected cache - Nightinggale
		updateCacheStorageLossTradingValues(eIndex, bNewValue);
	}
}

/*
	bool CvCity::isHasConceptualBuilding(BuildingTypes eIndex) const
	Complexity: O( 1 )
	Purpose:
		Check if a building (BuildingType) is present in the city.
*/
bool CvCity::isHasConceptualBuilding(BuildingTypes eIndex) const
{
	return (isHasRealBuilding(eIndex) || isHasFreeBuilding(eIndex));
}

/*
	bool CvCity::isDominantSpecialBuilding(BuildingTypes eIndex) const
	Complexity: O( n )
		n ... number of possible buildings in a building slot
	Purpose:
		Check if a building is the building of highest tier built in its building slot.
*/
bool CvCity::isDominantSpecialBuilding(BuildingTypes eIndex) const
{
	FAssert((eIndex >= 0) && (eIndex < GC.getNumBuildingInfos()));
	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eIndex);
	
	//Walk through all the possible buildings in the building slot of the given building ...
	//... and check if the given building is the building with the highest tier (SpecialBuildingPriority), built in that slot.
	BuildingTypes eNextBuilding = (BuildingTypes) kBuilding.getIndexOf_NextBuildingType_In_SpecialBuilding();
	while (eNextBuilding != eIndex)
	{
		CvBuildingInfo& kNextBuilding = GC.getBuildingInfo(eNextBuilding);
		if (isHasConceptualBuilding(eNextBuilding))
		{
			if (kBuilding.getSpecialBuildingPriority() < kNextBuilding.getSpecialBuildingPriority())
			{
				return false;
			}
		}

		eNextBuilding = (BuildingTypes) kNextBuilding.getIndexOf_NextBuildingType_In_SpecialBuilding();
	}

	return true;
}

void CvCity::clearOrderQueue()
{
	while (headOrderQueueNode() != NULL)
	{
		popOrder(0);
	}

	if ((getTeam() == GC.getGameINLINE().getActiveTeam()) || GC.getGameINLINE().isDebugMode())
	{
		setBillboardDirty(true);
	}
}


void CvCity::pushOrder(OrderTypes eOrder, int iData1, int iData2, bool bSave, bool bPop, bool bAppend, bool bForce)
{
	if (bPop)
	{
		popOrder(0);
	}

	bool bValid = false;

	switch (eOrder)
	{
	case ORDER_TRAIN:
		if (canTrain((UnitTypes)iData1) || bForce)
		{
			if (iData2 == -1)
			{
				iData2 = GC.getUnitInfo((UnitTypes)iData1).getDefaultUnitAIType();
			}

			GET_PLAYER(getOwnerINLINE()).changeUnitClassMaking(((UnitClassTypes)(GC.getUnitInfo((UnitTypes) iData1).getUnitClassType())), 1);

			area()->changeNumTrainAIUnits(getOwnerINLINE(), ((UnitAITypes)iData2), 1);
			GET_PLAYER(getOwnerINLINE()).AI_changeNumTrainAIUnits(((UnitAITypes)iData2), 1);

			bValid = true;
			gDLL->getEventReporterIFace()->cityBuildingUnit(this, (UnitTypes)iData1);
		}
		break;

	case ORDER_CONSTRUCT:
		if (canConstruct((BuildingTypes)iData1) || bForce)
		{
			GET_PLAYER(getOwnerINLINE()).changeBuildingClassMaking(((BuildingClassTypes)(GC.getBuildingInfo((BuildingTypes) iData1).getBuildingClassType())), 1);

			bValid = true;
			gDLL->getEventReporterIFace()->cityBuildingBuilding(this, (BuildingTypes)iData1);
		}
		break;

	case ORDER_CONVINCE:
		if (canConvince((FatherPointTypes)iData1) || bForce)
		{
			bValid = true;
		}
		break;

	default:
		FAssertMsg(false, "iOrder did not match a valid option");
		break;
	}

	if (!bValid)
	{
		return;
	}

	OrderData order;
	order.eOrderType = eOrder;
	order.iData1 = iData1;
	order.iData2 = iData2;
	order.bSave = bSave;

	if (bAppend)
	{
		m_orderQueue.insertAtEnd(order);
	}
	else
	{
		stopHeadOrder();
		m_orderQueue.insertAtBeginning(order);
	}

	if (!bAppend || (getOrderQueueLength() == 1))
	{
		startHeadOrder();
	}

	setAutoThresholdCache(); // transport feeder - Nightinggale

	if ((getTeam() == GC.getGameINLINE().getActiveTeam()) || GC.getGameINLINE().isDebugMode())
	{
		setBillboardDirty(true);

		if (isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true );
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(PlotListButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(Domestic_Advisor_DIRTY_BIT, true);
		}
	}
}


void CvCity::popOrder(int iNum, bool bFinish, bool bChoose)
{
	CvUnit* pUnit;
	CvPlot* pRallyPlot;
	CvWString szBuffer;
	CvString szSound;
	UnitAITypes eTrainAIUnit;
	bool bStart;
	bool bMessage;
	int iCount;
	int iProductionNeeded;
	int iOverflow;

	if (iNum == -1)
	{
		iNum = (getOrderQueueLength() - 1);
	}

	iCount = 0;

	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	while (pOrderNode != NULL)
	{
		if (iCount == iNum)
		{
			break;
		}

		iCount++;

		pOrderNode = nextOrderQueueNode(pOrderNode);
	}

	if (pOrderNode == NULL)
	{
		return;
	}

	if (bFinish && !processRequiredYields(iNum))
	{
		bFinish = false;
	}

	if (bFinish && pOrderNode->m_data.bSave)
	{
		pushOrder(pOrderNode->m_data.eOrderType, pOrderNode->m_data.iData1, pOrderNode->m_data.iData2, true, false, true);
	}

	UnitTypes eTrainUnit = NO_UNIT;
	BuildingTypes eConstructBuilding = NO_BUILDING;

	switch (pOrderNode->m_data.eOrderType)
	{
	case ORDER_TRAIN:
		eTrainUnit = ((UnitTypes)(pOrderNode->m_data.iData1));
		eTrainAIUnit = ((UnitAITypes)(pOrderNode->m_data.iData2));
		FAssertMsg(eTrainUnit != NO_UNIT, "eTrainUnit is expected to be assigned a valid unit type");
		FAssertMsg(eTrainAIUnit != NO_UNITAI, "eTrainAIUnit is expected to be assigned a valid unit AI type");

		GET_PLAYER(getOwnerINLINE()).changeUnitClassMaking(((UnitClassTypes)(GC.getUnitInfo(eTrainUnit).getUnitClassType())), -1);

		area()->changeNumTrainAIUnits(getOwnerINLINE(), eTrainAIUnit, -1);
		GET_PLAYER(getOwnerINLINE()).AI_changeNumTrainAIUnits(eTrainAIUnit, -1);

		if (bFinish)
		{
			iProductionNeeded = getYieldProductionNeeded(eTrainUnit, YIELD_HAMMERS);

			iOverflow = std::max(0, getUnitProduction(eTrainUnit) - iProductionNeeded);
			changeOverflowProduction(iOverflow, getProductionModifier(eTrainUnit));
			setUnitProduction(eTrainUnit, 0);

			pUnit = GET_PLAYER(getOwnerINLINE()).initUnit(eTrainUnit, (ProfessionTypes) GC.getUnitInfo(eTrainUnit).getDefaultProfession(), getX_INLINE(), getY_INLINE(), eTrainAIUnit);
			FAssertMsg(pUnit != NULL, "pUnit is expected to be assigned a valid unit object");

			pUnit->finishMoves();

			addProductionExperience(pUnit);

			pRallyPlot = getRallyPlot();

			if (pRallyPlot != NULL)
			{
				pUnit->getGroup()->pushMission(MISSION_MOVE_TO, pRallyPlot->getX_INLINE(), pRallyPlot->getY_INLINE());
			}

			gDLL->getEventReporterIFace()->unitBuilt(this, pUnit);
		}
		break;

	case ORDER_CONSTRUCT:
		eConstructBuilding = ((BuildingTypes)(pOrderNode->m_data.iData1));

		GET_PLAYER(getOwnerINLINE()).changeBuildingClassMaking(((BuildingClassTypes)(GC.getBuildingInfo(eConstructBuilding).getBuildingClassType())), -1);

		if (bFinish)
		{
			iProductionNeeded = getYieldProductionNeeded(eConstructBuilding, YIELD_HAMMERS);
			int iOverflow = std::max(0, getBuildingProduction(eConstructBuilding) - iProductionNeeded);
			changeOverflowProduction(iOverflow, getProductionModifier(eConstructBuilding));
			setBuildingProduction(eConstructBuilding, 0);
			setHasRealBuilding(eConstructBuilding, true);

			for (int i = 0; i < GC.getNumFatherPointInfos(); ++i)
			{
				FatherPointTypes ePointType = (FatherPointTypes) i;
				int gameSpeedMod =  GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
				int  buildBonus =  GC.getFatherPointInfo(ePointType).getBuildingPoints(GC.getBuildingInfo(eConstructBuilding).getBuildingClassType());
				buildBonus = buildBonus * gameSpeedMod /100;

				GET_PLAYER(getOwnerINLINE()).changeFatherPoints(ePointType, buildBonus);
			}

			gDLL->getEventReporterIFace()->buildingBuilt(this, eConstructBuilding);
		}
		break;

	case ORDER_CONVINCE:
		break;

	default:
		FAssertMsg(false, "pOrderNode->m_data.eOrderType is not a valid option");
		break;
	}

	if (pOrderNode == headOrderQueueNode())
	{
		bStart = true;
		stopHeadOrder();
	}
	else
	{
		bStart = false;
	}

	m_orderQueue.deleteNode(pOrderNode);
	pOrderNode = NULL;

	setAutoThresholdCache(); // transport feeder - Nightinggale

	if (bStart)
	{
		startHeadOrder();
	}
	if ((getTeam() == GC.getGameINLINE().getActiveTeam()) || GC.getGameINLINE().isDebugMode())
	{
		setBillboardDirty(true);

		if (isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true );
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(Domestic_Advisor_DIRTY_BIT, true);
		}
	}

	bMessage = false;

	if (bChoose)
	{
		if (getOrderQueueLength() == 0)
		{
			if (!isHuman() || isProductionAutomated())
			{
				AI_chooseProduction();
			}
			else
			{
				chooseProduction(eTrainUnit, eConstructBuilding, bFinish);

				bMessage = true;
			}
		}
	}

	LPCSTR szIcon = NULL;

	if (bFinish && !bMessage)
	{
		if (eTrainUnit != NO_UNIT)
		{
			szBuffer = gDLL->getText("TXT_KEY_MISC_TRAINED_UNIT_IN", GC.getUnitInfo(eTrainUnit).getTextKeyWide(), getNameKey());
			//Androrc UnitArtStyles
//			szSound = GC.getUnitInfo(eTrainUnit).getArtInfo(0, NO_PROFESSION)->getTrainSound();
			szSound = GC.getUnitInfo(eTrainUnit).getUnitArtStylesArtInfo(0, NO_PROFESSION, (UnitArtStyleTypes) GC.getCivilizationInfo(GET_PLAYER(getOwnerINLINE()).getCivilizationType()).getUnitArtStyleType())->getTrainSound();
			//Androrc End
			szIcon = GET_PLAYER(getOwnerINLINE()).getUnitButton(eTrainUnit);
		}
		else if (eConstructBuilding != NO_BUILDING)
		{
			szBuffer = gDLL->getText("TXT_KEY_MISC_CONSTRUCTED_BUILD_IN", GC.getBuildingInfo(eConstructBuilding).getTextKeyWide(), getNameKey());
			szSound = GC.getBuildingInfo(eConstructBuilding).getConstructSound();
			szIcon = GC.getBuildingInfo(eConstructBuilding).getButton();
		}

		if (isProductionUnit() || isProductionBuilding())
		{
			szBuffer += gDLL->getText("TXT_KEY_MISC_WORK_HAS_BEGUN", getProductionNameKey());
		}

		gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, szSound, MESSAGE_TYPE_MINOR_EVENT, szIcon, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), true, true);
	}

	setAutoThresholdCache(); // transport feeder - Nightinggale

	if ((getTeam() == GC.getGameINLINE().getActiveTeam()) || GC.getGameINLINE().isDebugMode())
	{
		setBillboardDirty(true);

		if (isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true );
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(PlotListButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(Domestic_Advisor_DIRTY_BIT, true);
		}
	}
}

bool CvCity::checkRequiredYields(OrderTypes eOrder, int iData1) const
{
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		YieldTypes eYield = (YieldTypes) iYield;
		if (GC.getYieldInfo(eYield).isCargo())
		{	
			int iAmount = 0;
			switch (eOrder)
			{
			case ORDER_TRAIN:
				iAmount = GET_PLAYER(getOwnerINLINE()).getYieldProductionNeeded((UnitTypes) iData1, eYield);
				break;
			case ORDER_CONSTRUCT:
				iAmount = GET_PLAYER(getOwnerINLINE()).getYieldProductionNeeded((BuildingTypes) iData1, eYield);
				break;
			default:
				break;
			}

			if (iAmount > getYieldStored(eYield) + getYieldRushed(eYield))
			{
				return false;
			}
		}
	}

	return true;
}

void CvCity::checkCompletedBuilds(YieldTypes eYield, int iChange)
{
	if (iChange > 0)
	{
		if (GC.getYieldInfo(eYield).isCargo())
		{
			std::vector< std::pair<OrderTypes, int> > aOrders;
			getOrdersWaitingForYield(aOrders, eYield, true, iChange);

			if (!aOrders.empty())
			{
				if (isHuman() && !isProductionAutomated())
				{
					bool bFound = false;
					const CvPopupQueue& aPopups = GET_PLAYER(getOwnerINLINE()).getPopups();
					for (CvPopupQueue::const_iterator it = aPopups.begin(); it != aPopups.end() && !bFound; ++it)
					{
						CvPopupInfo* pPopup = *it;
						if (pPopup->getButtonPopupType() == BUTTONPOPUP_CHOOSE_YIELD_BUILD && pPopup->getData1() == getID() && pPopup->getData2() == eYield)
						{
							bFound = true;
						}
					}

					if (!bFound)
					{
						CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CHOOSE_YIELD_BUILD, getID(), eYield);
						gDLL->getInterfaceIFace()->addPopup(pInfo, getOwnerINLINE());
					}
				}
				else
				{
					// just pick the first one
					pushOrder(aOrders[0].first, aOrders[0].second, NO_UNITAI, false, false, false);
					if (checkRequiredYields(aOrders[0].first, aOrders[0].second) && getProduction() >= getProductionNeeded(YIELD_HAMMERS))
					{
						popOrder(0, true, true);
					}
				}
			}
		}
	}
}

bool CvCity::processRequiredYields(int iNum)
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
	
	for (int iCount = 0; pOrderNode != NULL; ++iCount)
	{
		if (iCount == iNum)
		{
			break;
		}
		pOrderNode = nextOrderQueueNode(pOrderNode);
	}

	if (pOrderNode == NULL)
	{
		return false;
	}

	if (!checkRequiredYields(pOrderNode->m_data.eOrderType, pOrderNode->m_data.iData1))
	{
		return false;
	}

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		YieldTypes eYield = (YieldTypes) iYield;
		if (GC.getYieldInfo(eYield).isCargo())
		{	
			int iNeeded = 0;
			switch (pOrderNode->m_data.eOrderType)
			{
			case ORDER_TRAIN:
				iNeeded = GET_PLAYER(getOwnerINLINE()).getYieldProductionNeeded((UnitTypes) pOrderNode->m_data.iData1, eYield);
				break;
			case ORDER_CONSTRUCT:
				iNeeded = GET_PLAYER(getOwnerINLINE()).getYieldProductionNeeded((BuildingTypes) pOrderNode->m_data.iData1, eYield);
				break;
			default:
				break;
			}

			int iRushedChange = std::min(iNeeded, getYieldRushed(eYield));
			changeYieldRushed(eYield, -iRushedChange);
			changeYieldStored(eYield, iRushedChange - iNeeded);
		}
	}

	return true;
}

void CvCity::getOrdersWaitingForYield(std::vector< std::pair<OrderTypes, int> >& aOrders, YieldTypes eYield, bool bYieldsComplete, int iChange) const
{
	int iStored = getYieldStored(eYield) + getYieldRushed(eYield);

	for (int iUnit = 0; iUnit < GC.getNumUnitInfos(); ++iUnit)
	{
		UnitTypes eUnit = (UnitTypes) iUnit;
		if (getUnitProduction(eUnit) > 0)
		{
			int iNeeded = getYieldProductionNeeded(eUnit, eYield);
			if ((iStored >= iNeeded && iStored - iChange < iNeeded && checkRequiredYields(ORDER_TRAIN, eUnit)) || !bYieldsComplete)
			{
				if (canTrain(eUnit) && GET_PLAYER(getOwnerINLINE()).getYieldProductionNeeded(eUnit, eYield) > 0)
				{
					bool bFound = false;
					for  (CLLNode<OrderData>* pOrderNode = headOrderQueueNode(); pOrderNode != NULL; pOrderNode = nextOrderQueueNode(pOrderNode))
					{
						if (pOrderNode->m_data.eOrderType == ORDER_TRAIN && pOrderNode->m_data.iData1 == eUnit)
						{
							bFound = true;
							break;
						}
					}

					if (!bFound)
					{
						aOrders.push_back( std::make_pair(ORDER_TRAIN, eUnit));
					}
				}
			}
		}
	}

	for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); ++iBuilding)
	{
		BuildingTypes eBuilding = (BuildingTypes) iBuilding;
		if (getBuildingProduction(eBuilding) > 0)
		{
			int iNeeded = getYieldProductionNeeded(eBuilding, eYield);
			if ((iStored >= iNeeded && iStored - iChange < iNeeded && checkRequiredYields(ORDER_CONSTRUCT, eBuilding)) || !bYieldsComplete)
			{
				if (canConstruct(eBuilding) && GET_PLAYER(getOwnerINLINE()).getYieldProductionNeeded(eBuilding, eYield) > 0)
				{
					bool bFound = false;
					for  (CLLNode<OrderData>* pOrderNode = headOrderQueueNode(); pOrderNode != NULL; pOrderNode = nextOrderQueueNode(pOrderNode))
					{
						if (pOrderNode->m_data.eOrderType == ORDER_CONSTRUCT && pOrderNode->m_data.iData1 == eBuilding)
						{
							bFound = true;
							break;
						}
					}

					if (!bFound)
					{
						aOrders.push_back( std::make_pair(ORDER_CONSTRUCT, eBuilding));
					}
				}
			}
		}
	}
}

void CvCity::startHeadOrder()
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		AI_setAssignWorkDirty(true);
	}
}


void CvCity::stopHeadOrder()
{
}


int CvCity::getOrderQueueLength() const
{
	return m_orderQueue.getLength();
}


OrderData* CvCity::getOrderFromQueue(int iIndex)
{
	CLLNode<OrderData>* pOrderNode;

	pOrderNode = m_orderQueue.nodeNum(iIndex);

	if (pOrderNode != NULL)
	{
		return &(pOrderNode->m_data);
	}
	else
	{
		return NULL;
	}
}


CLLNode<OrderData>* CvCity::nextOrderQueueNode(CLLNode<OrderData>* pNode) const
{
	return m_orderQueue.next(pNode);
}


CLLNode<OrderData>* CvCity::headOrderQueueNode() const
{
	return m_orderQueue.head();
}


CLLNode<OrderData>* CvCity::tailOrderQueueNode() const
{
	return m_orderQueue.tail();
}

void CvCity::setWallOverridePoints(const std::vector< std::pair<float, float> >& kPoints)
{
	m_kWallOverridePoints = kPoints;
	setLayoutDirty(true);
}


const std::vector< std::pair<float, float> >& CvCity::getWallOverridePoints() const
{
	return m_kWallOverridePoints;
}

// Protected Functions...

void CvCity::doGrowth()
{
	int iDiff;

	if (GC.getUSE_DO_GROWTH_CALLBACK()) // K-Mod. block unused python callbacks
	{		
		CyCity* pyCity = new CyCity(this);
		CyArgsList argsList;
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
		long lResult=0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "doGrowth", argsList.makeFunctionArgs(), &lResult);
		delete pyCity;	// python fxn must not hold on to this pointer
		if (lResult == 1)
		{
			return;
		}
	}

	iDiff = foodDifference();

	changeFood(iDiff);
	changeFoodKept(iDiff);

	setFoodKept(range(getFoodKept(), 0, ((growthThreshold() * getMaxFoodKeptPercent()) / 100)));

	if (getFood() >= growthThreshold())
	{
		if (AI_isEmphasizeAvoidGrowth())
		{
			setFood(growthThreshold());
		}
		else
		{

			UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("DEFAULT_POPULATION_UNIT"));
			if (NO_UNIT != eUnit)
			{
				CvUnit* pUnit = GET_PLAYER(getOwnerINLINE()).initUnit(eUnit, (ProfessionTypes) GC.getCivilizationInfo(GET_PLAYER(getOwnerINLINE()).getCivilizationType()).getDefaultProfession(), getX_INLINE(), getY_INLINE());

				changeFood(-(std::max(0, (growthThreshold() - getFoodKept()))));
			}

			gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_CITY_GROWTH", getNameKey()), "AS2D_POSITIVE_DINK", MESSAGE_TYPE_INFO, GC.getYieldInfo(YIELD_FOOD).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE(), true, true);


			// ONEVENT - City growth
			gDLL->getEventReporterIFace()->cityGrowth(this, getOwnerINLINE());
		}
	}
	else if (getFood() < 0)
	{
		changeFood(-(getFood()));
		if (getPopulation() > 1)
		{
			if (!AI_removeWorstPopulationUnit(false))
			{
				AI_removeWorstPopulationUnit(true);
			}

			gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_CITY_STARVING", getNameKey()), "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_INFO, GC.getYieldInfo(YIELD_FOOD).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);
		}
		else if (!isNative())
		{
			changeOccupationTimer(2);
		}
	}
}

void CvCity::doYields()
{
	int aiYields[NUM_YIELD_TYPES];
	calculateNetYields(aiYields, NULL, NULL, true);

//VET NewCapacity - begin 4/9
	int iTotalYields = getTotalYieldStored();
//VET NewCapacity - end 4/9
	int iMaxCapacity = getMaxYieldCapacity();
	
	
	// R&R, ray, adjustment Domestic Markets
	int iTotalProfitFromDomesticMarket = 0;
	//bool hasMarket = isHasBuilding((SpecialBuildingTypes)GC.getDefineINT("BUILDING_MARKET"));
	bool hasMarket = isHasSpecialBuilding((SpecialBuildingTypes)GC.getDefineINT("SPECIALBUILDING_MARKET"));

	//Performance optimization
	if(!isNative() && hasMarket)
	{
		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			YieldTypes eYield = (YieldTypes) iYield;
			if (GC.getYieldInfo(eYield).isCargo())
			{
				//if (hasMarket && getYieldDemand(eYield) > 0 && getYieldStored(eYield) > 0)
				if (getYieldDemand(eYield) > 0 && (getYieldStored(eYield)+aiYields[eYield]) > 0) // R&R, ray, improvment from vetiarvind
				{
					int iAmount = getYieldDemand(eYield);
					int iAmountForSale = getYieldStored(eYield) + aiYields[eYield];
					if (iAmount > iAmountForSale)
					{
						iAmount = iAmountForSale;
					}
					int iProfit = iAmount * getYieldBuyPrice(eYield);

					// WTP, ray, Happiness - START 
					int iCityHappinessDomesticMarketGoldModifiers = getCityHappiness() - getCityUnHappiness();
					iProfit = (iProfit * (100 + iCityHappinessDomesticMarketGoldModifiers)) / 100;
					// WTP, ray, Happiness - END

					aiYields[eYield] -= iAmount;
					GET_PLAYER(getOwnerINLINE()).changeGold(iProfit);
					iTotalProfitFromDomesticMarket = iTotalProfitFromDomesticMarket + iProfit;
					//GET_PLAYER(getOwnerINLINE()).changeYieldTradedTotal(eYield, iDemand);
				}	
			}
		}
		if (iTotalProfitFromDomesticMarket != 0 && GC.getDOMESTIC_SALES_MESSAGES() == 1)
		{
			CvWString szBuffer = gDLL->getText("TXT_KEY_GOODS_DOMESTIC_SOLD", getNameKey(), iTotalProfitFromDomesticMarket);
			gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), true, true);
		}
	}
	// R&R, ray, adjustment Domestic Markets, END
	
	// R&R, ray, adjustment for less Custom House messages
	int iCustomHouseProfit = 0;
	
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		YieldTypes eYield = (YieldTypes) iYield;
		switch (eYield)
		{
		case YIELD_FOOD:
			// handled in doGrowth
			break;
		case YIELD_HAMMERS:
			// temporary storage for hammers. Production handled in doProduction
			setYieldStored(eYield, aiYields[eYield]);
			break;
		case YIELD_CROSSES:
			// handled in CvPlayer::doCrosses
			break;
		case YIELD_EDUCATION:
			{
				std::vector<CvUnit*> apStudents;
				for (uint i = 0; i < m_aPopulationUnits.size(); ++i)
				{
					CvUnit* pLoopUnit = m_aPopulationUnits[i];
					ProfessionTypes eProfession = pLoopUnit->getProfession();
					if (eProfession != NO_PROFESSION)
					{
						// R&R, ray , MYCP partially based on code of Aymerick - START
						if (GC.getProfessionInfo(eProfession).getYieldsProduced(0) == eYield)
						// R&R, ray , MYCP partially based on code of Aymerick - END
						{
							FAssert(!pLoopUnit->getUnitInfo().isTreasure());
							int iStudentOutput = getProfessionOutput(eProfession, pLoopUnit, NULL) * getBaseYieldRateModifier(YIELD_EDUCATION) / 100;
							FAssert(iStudentOutput > 0);
							pLoopUnit->setYieldStored(pLoopUnit->getYieldStored() + std::max(iStudentOutput, 1));

							// TAC - Messages - Ray - START
							BuildingTypes eSchoolBuilding = getYieldBuilding(YIELD_EDUCATION);
							int iEducationNeeded = educationThreshold() - pLoopUnit->getYieldStored();

							int rStudentOutput = std::max(iStudentOutput, 1);
							int iTurns = std::max(0, (iEducationNeeded + rStudentOutput - 1) / rStudentOutput);  // round up				
					
							if (iTurns == 2 || iTurns == 1) {
								CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_STUDENT_ALMOST_GRADUATED", iTurns, getNameKey(), GC.getBuildingInfo(eSchoolBuilding).getTextKeyWide());
								gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CULTUREEXPANDS", MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(YIELD_EDUCATION).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), true, true);

							}
							// TAC - Messages - Ray - END

						}
					}
				}
			}
			break;
		default:
			changeYieldStored(eYield, aiYields[eYield]);

			int iOverflowYieldSellPercent = getStorageLossSellPercentage();
			bool bIgnoresBoycott = getIgnoresBoycott();
			bool bHasUnlockedTradeSettings = getHasUnlockedStorageLossTradeSettings();

			if (GC.getYieldInfo(eYield).isCargo() && eYield != YIELD_LUMBER && eYield != YIELD_STONE) // we do not sell YIELD_LUMBER and Stone to Overflow or Custom House
			{
				//VET NewCapacity - begin 6/9 -- ray fix
				int iExcess = 0;
				if (GC.getNEW_CAPACITY())
				{
					// Here special sell behaviour for Custom House
					if (bHasUnlockedTradeSettings && iTotalYields < iMaxCapacity)
					{
						int sellThreshold = 0;
						// R&R, ray, finishing Custom House Screen
						if (isCustomHouseNeverSell(eYield))
						{
							sellThreshold = iMaxCapacity;
						}
						else 
						{
							sellThreshold = getCustomHouseSellThreshold(eYield);
						}

						//R&R, ray, fixing threshold displayed and internal differently for some gamespeeds e.g. Marathon
						//sellThreshold = sellThreshold * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getStoragePercent() / 100;
						iExcess = getYieldStored(eYield) - sellThreshold;
					}

					// normal overflow
					else
					{
						// R&R, ray, reprogrammed Overflow for New City Storage
						if (getYieldStored(eYield) > 0 && iTotalYields > iMaxCapacity)
						{
							iExcess = (iTotalYields - iMaxCapacity) * getYieldStored(eYield) / iTotalYields;
							if (iExcess == 0)
							{
								iExcess = 1;
							}
						}
					}
				}
				// no new Capacity
				else
				{
//VET NewCapacity - end 6/9 -- ray fix
					iExcess = getYieldStored(eYield) - iMaxCapacity;
				}
					
				if (iExcess > 0)
				{
					int iLoss = std::max(GC.getCITY_YIELD_DECAY_PERCENT() * iExcess / 100, GC.getMIN_CITY_YIELD_DECAY());
					iLoss = std::min(iLoss, iExcess);
					changeYieldStored(eYield, -iLoss);
					
					// R&R, ray , Changes to Custom House - START
					int iComparableProfitInEurope = GET_PLAYER(getOwnerINLINE()).getSellToEuropeProfit(eYield, iLoss);

					// Yield is boycotted but you have Custom House
					if (iComparableProfitInEurope == 0 && bIgnoresBoycott)
					{
						if (GET_PLAYER(getOwnerINLINE()).getParent() == NO_PLAYER)
						{
							iComparableProfitInEurope = 0;
						}

						else
						{
							int briberate = GET_PLAYER(getOwnerINLINE()).getTaxRate();
							int minPrice = GC.getYieldInfo(eYield).getMinimumBuyPrice();
							int iAmount = iLoss;
							iComparableProfitInEurope = iAmount * minPrice;
							iComparableProfitInEurope -= (iComparableProfitInEurope * briberate) / 100;
						}
					}

					int iProfit = iOverflowYieldSellPercent * iComparableProfitInEurope / 100;
					// R&R, ray , Changes to Custom House - END
					if (iProfit > 0)
					{
						CvPlayer& kPlayerEurope = GET_PLAYER(GET_PLAYER(getOwnerINLINE()).getParent());
						GET_PLAYER(getOwnerINLINE()).changeGold(iProfit * GET_PLAYER(getOwnerINLINE()).getExtraTradeMultiplier(kPlayerEurope.getID()) / 100);

						int iDiscountedLoss = iOverflowYieldSellPercent * iLoss / 100;
						GET_PLAYER(getOwnerINLINE()).changeYieldTradedTotal(eYield, iDiscountedLoss);
						kPlayerEurope.changeYieldTradedTotal(eYield, iDiscountedLoss);
						GC.getGameINLINE().changeYieldBoughtTotal(kPlayerEurope.getID(), eYield, -iDiscountedLoss);

						// R&R, ray , Changes to Custom House - START
						// Selling with Custom House will get Trade Founding Father Points
						// check if Custom House
						if (bHasUnlockedTradeSettings) 
						{
							for (int i = 0; i < GC.getNumFatherPointInfos(); ++i)
							{
								//Divide by 2 because Custom House gives only half as Europe does
								int iProfitForFatherPointsAtCustomHouse = iProfit / 2;
							
								FatherPointTypes ePointType = (FatherPointTypes) i;
								GET_PLAYER(getOwnerINLINE()).changeFatherPoints(ePointType, iProfitForFatherPointsAtCustomHouse * GC.getFatherPointInfo(ePointType).getEuropeTradeGoldPointPercent() / 100 );
							}

							// R&R, ray, adjustment for less Custom House messages
							iCustomHouseProfit = iCustomHouseProfit + iProfit; 
							//CvWString szBuffer = gDLL->getText("TXT_KEY_GOODS_SOLD_CUSTOM_HOUSE", iLoss, GC.getYieldInfo(eYield).getChar(), getNameKey(), iProfit);
							//gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BUILD_BANK", MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(eYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), true, true);
						}
						else 
						{
							CvWString szBuffer = gDLL->getText("TXT_KEY_GOODS_LOST_SOLD", iLoss, GC.getYieldInfo(eYield).getChar(), getNameKey(), iProfit);
							gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(eYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), true, true);
						}
						// R&R, ray , Changes to Custom House - END
					}
					else
					{
						CvWString szBuffer = gDLL->getText("TXT_KEY_GOODS_LOST", iLoss, GC.getYieldInfo(eYield).getChar(), getNameKey());
						gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(eYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);
					}
				}
				//VET NewCapacity -- ray fix for messages
				//else if (aiYields[eYield] > -iExcess)
				else if (aiYields[eYield] > -iExcess && !GC.getNEW_CAPACITY())
				{
					CvWString szBuffer = gDLL->getText("TXT_KEY_RUNNING_OUT_OF_SPACE",GC.getYieldInfo(eYield).getChar(), getNameKey());
					gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(eYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);
				}
				//VET NewCapacity -- ray fix for messages - START
				else if (iYield == 5 && GC.getNEW_CAPACITY() && (iMaxCapacity - iTotalYields) > 0 && (iMaxCapacity - iTotalYields) < (iMaxCapacity / 10)) //only do this message once, thus iYield 5
				{
					CvWString szBuffer = gDLL->getText("TXT_KEY_RUNNING_OUT_OF_SPACE_NEW_CAPACITY", getNameKey());
					gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);
				}
				//VET NewCapacity -- ray fix for messages - END

				if (aiYields[eYield] > 0)
				{
					for (int i = 0; i < GC.getNumFatherPointInfos(); ++i)
					{
						FatherPointTypes ePointType = (FatherPointTypes) i;
						GET_PLAYER(getOwnerINLINE()).changeFatherPoints(ePointType, aiYields[eYield] * GC.getFatherPointInfo(ePointType).getYieldPoints(eYield));
					}

					gDLL->getEventReporterIFace()->yieldProduced(getOwnerINLINE(), getID(), eYield);
				}
			}
		}
		doAutoExport(eYield); // auto traderoute - Nightinggale
	}

	// R&R, ray, adjustment for less Custom House messages
	if (iCustomHouseProfit > 0)
	{
		CvWString szBuffer = gDLL->getText("TXT_KEY_GOODS_SOLD_CUSTOM_HOUSE_SINGLE_MESSAGE", getNameKey(), iCustomHouseProfit);
		gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), true, true);
	}

}

/*
void CvCity::addTempHurryYieldsForProduction()
Complexity: O ( n )
Purpose:
	Adds the yields required for hurrying production to the storage. Empties temporary storage of yields for hurrying.
*/
void CvCity::addTempHurryYieldsForProduction()
{
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		YieldTypes eYield = (YieldTypes)iYield;
		changeYieldStored(eYield, getYieldRushed(eYield));
		changeYieldRushed(eYield, -getYieldRushed(eYield));
	}
}

void CvCity::doCulture()
{
	if (GC.getUSE_DO_CULTURE_CALLBACK()) // K-Mod. block unused python callbacks
	{
		CyCity* pyCity = new CyCity(this);
		CyArgsList argsList;
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
		long lResult = 0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "doCulture", argsList.makeFunctionArgs(), &lResult);
		delete pyCity;	// python fxn must not hold on to this pointer
		if (lResult == 1)
		{
			return;
		}
	}
	changeCulture(getOwnerINLINE(), getCultureRate(), false);
}


void CvCity::doPlotCulture(bool bUpdate, PlayerTypes ePlayer, int iCultureRate)
{
	CvPlot* pLoopPlot;
	int iDX, iDY;
	int iCultureRange;
	CultureLevelTypes eCultureLevel = (CultureLevelTypes)0;

	if (GC.getUSE_DO_PLOT_CULTURE_CALLBACK()) // K-Mod. block unused python callbacks
	{
		CyCity* pyCity = new CyCity(this);
		CyArgsList argsList;
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
		argsList.add(bUpdate);
		argsList.add(ePlayer);
		argsList.add(iCultureRate);
		long lResult = 0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "doPlotCulture", argsList.makeFunctionArgs(), &lResult);
		delete pyCity;	// python fxn must not hold on to this pointer
		if (lResult == 1)
		{
			return;
		}
	}

	FAssert(NO_PLAYER != ePlayer);

	if (getOwnerINLINE() == ePlayer)
	{
		eCultureLevel = getCultureLevel();
	}
	else
	{
		for (int iI = (GC.getNumCultureLevelInfos() - 1); iI > 0; iI--)
		{
			if (getCulture(ePlayer) >= GC.getGameINLINE().getCultureLevelThreshold((CultureLevelTypes) iI, getOwnerINLINE()))
			{
				eCultureLevel = (CultureLevelTypes)iI;
				break;
			}
		}
	}

	int iFreeCultureRate = GC.getDefineINT("CITY_FREE_CULTURE_GROWTH_FACTOR");
	if (getCulture(ePlayer) > 0)
	{
		if (eCultureLevel != NO_CULTURELEVEL)
		{
			for (iDX = -eCultureLevel; iDX <= eCultureLevel; iDX++)
			{
				for (iDY = -eCultureLevel; iDY <= eCultureLevel; iDY++)
				{
					iCultureRange = cultureDistance(iDX, iDY);

					if (iCultureRange <= eCultureLevel)
					{
						pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

						if (pLoopPlot != NULL)
						{
							if (pLoopPlot->isPotentialCityWorkForArea(area()))
							{
								pLoopPlot->changeCulture(ePlayer, (((eCultureLevel - iCultureRange) * iFreeCultureRate) + iCultureRate + 1), (bUpdate || !(pLoopPlot->isOwned())));
							}
						}
					}
				}
			}
		}
	}
}

void CvCity::doSpecialists()
{
	std::set<UnitTypes> setExisting;
	if (calculateNetYield(YIELD_EDUCATION) > 0)
	{
		for (uint i = 0; i < m_aPopulationUnits.size(); ++i)
		{
			UnitTypes eUnit =  m_aPopulationUnits[i]->getUnitType();
			if (setExisting.find(eUnit) == setExisting.end())
			{
				m_aiSpecialistWeights[eUnit] += GC.getUnitInfo(eUnit).getTeacherWeight();
				setExisting.insert(eUnit);
			}
		}
	}
}


bool CvCity::doCheckProduction()
{
	CLLNode<OrderData>* pOrderNode;
	OrderData* pOrder;
	UnitTypes eUpgradeUnit;
	int iUpgradeProduction;
	CvWString szBuffer;
	int iI;
	bool bOK = true;

	if (!isProduction() && !isDisorder() && isHuman() && !isProductionAutomated())
	{
		chooseProduction();
		return bOK;
	}

	for (iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		if (getFirstUnitOrder((UnitTypes)iI) != -1)
		{
			eUpgradeUnit = allUpgradesAvailable((UnitTypes)iI);

			if (eUpgradeUnit != NO_UNIT)
			{
				FAssertMsg(eUpgradeUnit != iI, "eUpgradeUnit is expected to be different from iI");
				iUpgradeProduction = getUnitProduction((UnitTypes)iI);
				setUnitProduction(((UnitTypes)iI), 0);
				setUnitProduction(eUpgradeUnit, iUpgradeProduction);

				pOrderNode = headOrderQueueNode();

				while (pOrderNode != NULL)
				{
					if (pOrderNode->m_data.eOrderType == ORDER_TRAIN)
					{
						if (pOrderNode->m_data.iData1 == iI)
						{
							GET_PLAYER(getOwnerINLINE()).changeUnitClassMaking(((UnitClassTypes)(GC.getUnitInfo((UnitTypes)(pOrderNode->m_data.iData1)).getUnitClassType())), -1);
							pOrderNode->m_data.iData1 = eUpgradeUnit;
							if (GET_PLAYER(getOwnerINLINE()).AI_unitValue(eUpgradeUnit, ((UnitAITypes)(pOrderNode->m_data.iData2)), area()) == 0)
							{
								area()->changeNumTrainAIUnits(getOwnerINLINE(), ((UnitAITypes)(pOrderNode->m_data.iData2)), -1);
								GET_PLAYER(getOwnerINLINE()).AI_changeNumTrainAIUnits(((UnitAITypes)(pOrderNode->m_data.iData2)), -1);
								pOrderNode->m_data.iData2 = GC.getUnitInfo(eUpgradeUnit).getDefaultUnitAIType();
								area()->changeNumTrainAIUnits(getOwnerINLINE(), ((UnitAITypes)(pOrderNode->m_data.iData2)), 1);
								GET_PLAYER(getOwnerINLINE()).AI_changeNumTrainAIUnits(((UnitAITypes)(pOrderNode->m_data.iData2)), 1);
							}
							GET_PLAYER(getOwnerINLINE()).changeUnitClassMaking(((UnitClassTypes)(GC.getUnitInfo((UnitTypes)(pOrderNode->m_data.iData1)).getUnitClassType())), 1);
						}
					}

					pOrderNode = nextOrderQueueNode(pOrderNode);
				}
			}
		}
	}

	for (iI = (getOrderQueueLength() - 1); iI >= 0; iI--)
	{
		pOrder = getOrderFromQueue(iI);

		if (pOrder != NULL)
		{
			if (!canContinueProduction(*pOrder))
			{
				popOrder(iI, false, true);
				bOK = false;
			}
		}
	}

	return bOK;
}

void CvCity::doCheat(bool bAlt, bool bShift, bool bCtrl)
{
	if (bCtrl)
	{
		changeCulture(getOwnerINLINE(), getCultureThreshold(), true);
	}
	else if(bShift)
	{
		changePopulation(1);
	}
	else if(bAlt)
	{
		for (int i = 0; i < GC.getNUM_YIELD_TYPES(); ++i)
		{
			changeYieldStored((YieldTypes)i, GC.getGameINLINE().getCargoYieldCapacity() / 2);
		}
		setRebelSentiment(getRebelSentiment() + GC.getGameINLINE().getCargoYieldCapacity());
	}
	else
	{
		popOrder(0, true);
	}
}

void CvCity::doProduction(bool bAllowNoProduction)
{
	//Move yields for hurry production from temp storage to city storage
	addTempHurryYieldsForProduction();

	if (GC.getUSE_DO_PRODUCTION_CALLBACK()) // K-Mod. block unused python callbacks
	{
		CyCity* pyCity = new CyCity(this);
		CyArgsList argsList;
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
		long lResult = 0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "doProduction", argsList.makeFunctionArgs(), &lResult);
		delete pyCity;	// python fxn must not hold on to this pointer
		if (lResult == 1)
		{
			return;
		}
	}

	if (!isHuman() || isProductionAutomated())
	{
		if (!isProduction() || isProductionConvince() || AI_isChooseProductionDirty())
		{
			AI_chooseProduction();
		}
	}

	if (!bAllowNoProduction && !isProduction())
	{
		return;
	}

	if (isDisorder())
	{
		return;
	}

	if (isProduction())
	{
		int iProduction = getStoredProductionDifference();

		FatherPointTypes eFatherPointType = getProductionFatherPoint();
		if (eFatherPointType != NO_FATHER_POINT_TYPE)
		{
			//Lumber mill is building points
			GET_PLAYER(getOwnerINLINE()).changeFatherPoints(eFatherPointType, iProduction * GC.getFatherPointInfo(eFatherPointType).getProductionConversionPoints());
		}
		else
		{
			if (!isProductionConvince())
			{
				setOverflowProduction(0);
			}
			changeProduction(iProduction);
			if (getProduction() >= getProductionNeeded(YIELD_HAMMERS))
			{
				popOrder(0, true, true);
			}
		}
	}
	else
	{
		changeOverflowProduction(getCurrentProductionDifference(false), getProductionModifier());
	}
}


void CvCity::doDecay()
{
	int iI;

	for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (getProductionBuilding() != ((BuildingTypes)iI))
		{
			if (getBuildingProduction((BuildingTypes)iI) > 0)
			{
				changeBuildingProductionTime(((BuildingTypes)iI), 1);

				if (isHuman())
				{
					if (getBuildingProductionTime((BuildingTypes)iI) > GC.getDefineINT("BUILDING_PRODUCTION_DECAY_TIME"))
					{
						setBuildingProduction(((BuildingTypes)iI), ((getBuildingProduction((BuildingTypes)iI) * GC.getDefineINT("BUILDING_PRODUCTION_DECAY_PERCENT")) / 100));
					}
				}
			}
			else
			{
				setBuildingProductionTime(((BuildingTypes)iI), 0);
			}
		}
	}

	for (iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		if (getProductionUnit() != ((UnitTypes)iI))
		{
			if (getUnitProduction((UnitTypes)iI) > 0)
			{
				changeUnitProductionTime(((UnitTypes)iI), 1);

				if (isHuman())
				{
					if (getUnitProductionTime((UnitTypes)iI) > GC.getDefineINT("UNIT_PRODUCTION_DECAY_TIME"))
					{
						setUnitProduction(((UnitTypes)iI), ((getUnitProduction((UnitTypes)iI) * GC.getDefineINT("UNIT_PRODUCTION_DECAY_PERCENT")) / 100));
					}
				}
			}
			else
			{
				setUnitProductionTime(((UnitTypes)iI), 0);
			}
		}
	}
}

void CvCity::doMissionaries()
{
	GET_PLAYER(getOwnerINLINE()).applyMissionaryPoints(this);
}

// WTP, ray, Native Trade Posts - START
void CvCity::doNativeTradePost()
{
	PlayerTypes ePlayer = getTradePostPlayer();
	if (ePlayer != NO_PLAYER)
	{
		int iNativeTradeModifierNation = 100 + GET_PLAYER(getOwnerINLINE()).getNativeTradeModifier();
		int iNativeTradeModifierTradePostOwner = 100 + GET_PLAYER(ePlayer).getNativeTradeModifier();
		int iNativeTradeRateCity = 100 + getNativeTradeRate();
		int iNativeVillagePopulation = getPopulation();
		int iGoldPerNative = GC.getTRADE_POST_GOLD_PER_NATIVE();

		int iTotalVillageGold= iNativeVillagePopulation * iGoldPerNative;
		int iTotalModifier = iNativeTradeRateCity * iNativeTradeModifierNation * iNativeTradeModifierTradePostOwner;

		// calculate fixed gold
		int calculatedNewGold = (iTotalVillageGold * iTotalModifier) / 100 / 100 / 100;
		// make a random from double value, because meanvalue should be similar to calculated value
		int doubleGoldValue = calculatedNewGold * 2;
		int randomizedGoldToAdd = GC.getGameINLINE().getSorenRandNum(doubleGoldValue, "Randomized Trade Post Gold");
		changeNativeTradePostGold(randomizedGoldToAdd);

		// now check the gold if it is enough to create a treasure
		int iCurrentTradePostGold = getNativeTradePostGold();
		int iThreshold = GC.getMAX_TREASURE_AMOUNT() / 3;
		
		if (iCurrentTradePostGold >= iThreshold)
		{
			// spawn the Treasure
			UnitClassTypes eUnitClass = (UnitClassTypes) GC.getDefineINT("TREASURE_UNITCLASS");
			UnitTypes eUnit = (UnitTypes) GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType()).getCivilizationUnits(eUnitClass);
			CvUnit* pTreasure = GET_PLAYER(ePlayer).initUnit(eUnit, (ProfessionTypes) GC.getUnitInfo(eUnit).getDefaultProfession(), plot()->getX_INLINE(), plot()->getY_INLINE(), NO_UNITAI, NO_DIRECTION, iCurrentTradePostGold);
			// when we create a treasure, set gold to 0
			setNativeTradePostGold(0);

			// we send a message now
			CvWString szBuffer = gDLL->getText("TXT_KEY_TRADE_POST_CREATED_TREASURE", getNameKey());
			gDLL->getInterfaceIFace()->addMessage(ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, pTreasure->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), true, true);
		}
	}
	return;
}
// WTP, ray, Native Trade Posts - END

// Private Functions...

// just-in-time yield arrays - start - Nightinggale
// bitmap to tell which arrays are saved
enum
{
	SAVE_BIT_CUSTOM_HOUSE_SELL_THRESHOLD = 1,
	SAVE_BIT_CUSTOM_HOUSE_NEVER_SELL     = 1 << 1,
	SAVE_BIT_ORDERED_STUDENTS            = 1 << 2,
	SAVE_BIT_ORDERED_STUDENTS_REPEAT     = 1 << 3,
	// traderoute just-in-time - start - Nightinggale
	SAVE_BIT_TRADE_IMPORTS               = 1 << 4,
	SAVE_BIT_TRADE_EXPORTS               = 1 << 5,
	SAVE_BIT_TRADE_THRESHOLD             = 1 << 6,
	// traderoute just-in-time - end - Nightinggale
	// transport feeder - start - Nightinggale
	SAVE_BIT_IMPORT_FEEDER               = 1 << 7,
	SAVE_BIT_IMPORT_STOP                 = 1 << 8,
	// transport feeder - end - Nightinggale
	SAVE_BIT_TRADE_MAX_THRESHOLD = 1 << 9// R&R mod, vetiarvind, max yield import limit
};
// just-in-time yield arrays - end - Nightinggale

void CvCity::read(FDataStreamBase* pStream)
{
	int iNumElts;

	// Init data before load
	reset();
	uint uiFlag=0;
	pStream->Read(&uiFlag);	// flags for expansion

	// just-in-time yield arrays - start - Nightinggale
	uint arrayBitmap = 0;
	if (uiFlag > 3)
	{
		pStream->Read(&arrayBitmap);
	} else {
		arrayBitmap  = SAVE_BIT_CUSTOM_HOUSE_SELL_THRESHOLD | SAVE_BIT_CUSTOM_HOUSE_NEVER_SELL;
		if (uiFlag == 3)
		{
			arrayBitmap |= SAVE_BIT_ORDERED_STUDENTS | SAVE_BIT_ORDERED_STUDENTS_REPEAT;
		}
	}
	// just-in-time yield arrays - start - Nightinggale
	pStream->Read(&m_iID);
	pStream->Read(&m_iX);
	pStream->Read(&m_iY);
	pStream->Read(&m_iRallyX);
	pStream->Read(&m_iRallyY);
	pStream->Read(&m_iGameTurnFounded);
	pStream->Read(&m_iGameTurnAcquired);
	pStream->Read(&m_iHighestPopulation);
	pStream->Read(&m_iWorkingPopulation);
	pStream->Read(&m_iNumBuildings);
	pStream->Read(&m_iHealRate);
	pStream->Read(&m_iFoodKept);
	pStream->Read(&m_iMaxFoodKeptPercent);
	pStream->Read(&m_iOverflowProduction);
	pStream->Read(&m_iMilitaryProductionModifier);
	pStream->Read(&m_iBuildingDefense);
	pStream->Read(&m_iBuildingBombardDefense);
	pStream->Read(&m_iFreeExperience);
	pStream->Read(&m_iDefenseDamage);
	pStream->Read(&m_iLastDefenseDamage);
	pStream->Read(&m_iOccupationTimer);
	pStream->Read(&m_iCultureUpdateTimer);
	pStream->Read(&m_iCitySizeBoost);
	pStream->Read(&m_iHammers);
	pStream->Read(&m_iMissionaryRate);
	pStream->Read(&m_iNativeTradeRate); // WTP, ray, Native Trade Posts - START
	pStream->Read(&m_iTradePostGold); // WTP, ray, Native Trade Posts - START
	pStream->Read(&m_bStirredUp); // R&R, ray , Stirring Up Natives
	pStream->Read(&m_iWorksWaterCount);
	pStream->Read(&m_iRebelSentiment);
	pStream->Read(&m_iCityHealth); // R&R, ray, Health
	pStream->Read(&m_iCityHappiness); // WTP, ray, Happiness - START
	pStream->Read(&m_iCityUnHappiness); // WTP, ray, Happiness - START
	pStream->Read(&m_iCityTimerFestivitiesOrUnrest ); // WTP, ray, Happiness - START
	pStream->Read(&m_iTeachUnitMultiplier);
	if (uiFlag > 1)
	{
		pStream->Read(&m_iEducationThresholdMultiplier);
	}

	pStream->Read(&m_bNeverLost);
	pStream->Read(&m_bBombarded);
	pStream->Read(&m_bProductionAutomated);
	pStream->Read(&m_bWallOverride);
	// m_bInfoDirty not saved...
	// m_bLayoutDirty not saved...

	pStream->Read((int*)&m_eOwner);
	pStream->Read((int*)&m_ePreviousOwner);
	pStream->Read((int*)&m_eOriginalOwner);
	pStream->Read((int*)&m_eCultureLevel);
	pStream->Read((int*)&m_eTeachUnitClass);
	if (uiFlag == 0)
	{
		m_eMissionaryPlayer = NO_PLAYER;
		CivilizationTypes eMissionaryCivilization;
		pStream->Read((int*)&eMissionaryCivilization);
		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
		{
			CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayer);
			if (kPlayer.isAlive() && kPlayer.getCivilizationType() == eMissionaryCivilization)
			{
				m_eMissionaryPlayer = (PlayerTypes) iPlayer;
				break;
			}
		}

		// WTP, ray, Native Trade Posts - START
		m_eTradePostPlayer = NO_PLAYER;
		CivilizationTypes eTradePostCivilization;
		pStream->Read((int*)&eTradePostCivilization);
		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
		{
			CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayer);
			if (kPlayer.isAlive() && kPlayer.getCivilizationType() == eTradePostCivilization)
			{
				m_eTradePostPlayer = (PlayerTypes) iPlayer;
				break;
			}
		}
		// WTP, ray, Native Trade Posts - END
	}
	else
	{
		pStream->Read((int*)&m_eMissionaryPlayer);
		pStream->Read((int*)&m_eTradePostPlayer); // WTP, ray, Native Trade Posts - START
	}

	pStream->Read(NUM_YIELD_TYPES, m_aiLandPlotYield); // R&R, ray, Landplot Yields
	pStream->Read(NUM_YIELD_TYPES, m_aiSeaPlotYield);
	pStream->Read(NUM_YIELD_TYPES, m_aiRiverPlotYield);
	pStream->Read(NUM_YIELD_TYPES, m_aiYieldRateModifier);
	pStream->Read(NUM_YIELD_TYPES, m_aiYieldStored);
//VET NewCapacity - begin 9/9
	m_iTotalYieldStored = 0;
	for(int i=3;i<NUM_YIELD_TYPES;i++)//without YIELD_FOOD, YIELD_LUMBER, YIELD_STONE
	{
		if (GC.getYieldInfo((YieldTypes)i).isCargo())
			{m_iTotalYieldStored += m_aiYieldStored[i];}
	}
	
//VET NewCapacity - begin 9/9
	pStream->Read(NUM_YIELD_TYPES, m_aiYieldRushed);
	// R&R, Androrc, Domestic Market
	pStream->Read(NUM_YIELD_TYPES, m_aiYieldBuyPrice);
	//Androrc End

	if (uiFlag > 5)
	{
		pStream->Read(&m_ePreferredYieldAtCityPlot);
	}

	// R&R, ray, finishing Custom House Screen
	ma_aiCustomHouseSellThreshold.read(pStream, arrayBitmap & SAVE_BIT_CUSTOM_HOUSE_SELL_THRESHOLD);
	ma_aiCustomHouseNeverSell.read(    pStream, arrayBitmap & SAVE_BIT_CUSTOM_HOUSE_NEVER_SELL);
	// R&R, ray, finishing Custom House Screen END

	// Teacher List - start - Nightinggale
	ma_OrderedStudents.read(      pStream, arrayBitmap & SAVE_BIT_ORDERED_STUDENTS);
	ma_OrderedStudentsRepeat.read(pStream, arrayBitmap & SAVE_BIT_ORDERED_STUDENTS_REPEAT);
	// Teacher List - end - Nightinggale

	pStream->Read(NUM_DOMAIN_TYPES, m_aiDomainFreeExperience);
	pStream->Read(NUM_DOMAIN_TYPES, m_aiDomainProductionModifier);
	pStream->Read(MAX_PLAYERS, m_aiCulture);

	pStream->Read(MAX_PLAYERS, m_abEverOwned);
	pStream->Read(MAX_TEAMS, m_abRevealed);
	pStream->Read(MAX_TEAMS, m_abScoutVisited);

	pStream->ReadString(m_szName);
	pStream->ReadString(m_szScriptData);

	pStream->Read(GC.getNumBuildingInfos(), m_paiBuildingProduction);
	pStream->Read(GC.getNumBuildingInfos(), m_paiBuildingProductionTime);
	pStream->Read(GC.getNumBuildingInfos(), m_paiBuildingOriginalOwner);
	pStream->Read(GC.getNumBuildingInfos(), m_paiBuildingOriginalTime);
	pStream->Read(GC.getNumUnitInfos(), m_paiUnitProduction);
	pStream->Read(GC.getNumUnitInfos(), m_paiUnitProductionTime);
	pStream->Read(GC.getNumUnitInfos(), m_aiSpecialistWeights);
	pStream->Read(GC.getNumUnitCombatInfos(), m_paiUnitCombatFreeExperience);
	pStream->Read(GC.getNumPromotionInfos(), m_paiFreePromotionCount);
	pStream->Read(GC.getNumBuildingInfos(), m_pabHasRealBuilding);
	pStream->Read(GC.getNumBuildingInfos(), m_pabHasFreeBuilding);

	pStream->Read(NUM_CITY_PLOTS, m_paiWorkingPlot);

	FAssert(m_aPopulationUnits.size() == 0);
	m_aPopulationUnits.clear();
	int iNumPopulation;
	pStream->Read(&iNumPopulation);
	for(int i=0;i<iNumPopulation;i++)
	{
		CvUnitAI *pUnit = new CvUnitAI();
		pUnit->read(pStream);
		m_aPopulationUnits.push_back(pUnit);
	}

	// traderoute just-in-time - start - Nightinggale
	if (uiFlag > 4)
	{
		ma_tradeImports.read(pStream, arrayBitmap & SAVE_BIT_TRADE_IMPORTS);
		ma_tradeExports.read(pStream, arrayBitmap & SAVE_BIT_TRADE_EXPORTS);
		ma_tradeThreshold.read(pStream, arrayBitmap & SAVE_BIT_TRADE_THRESHOLD);
		// transport feeder - start - Nightinggale
		ma_tradeImportsMaintain.read(pStream, arrayBitmap & SAVE_BIT_IMPORT_FEEDER);
		ma_tradeStopAutoImport.read(pStream, arrayBitmap & SAVE_BIT_IMPORT_STOP);
		// transport feeder - end - Nightinggale
		// R&R mod, vetiarvind, max yield import limit - start
		ma_tradeMaxThreshold.read(pStream, arrayBitmap & SAVE_BIT_TRADE_MAX_THRESHOLD); 
		// R&R mod, vetiarvind, max yield import limit - end
	} else {
		int iNumYields;
		std::vector<YieldTypes> aYields;
		pStream->Read(&iNumYields);
		for (int iI = 0; iI < iNumYields; iI++)
		{
			int iIndex;
			pStream->Read(&iIndex);
			ma_tradeImports.set(true, iIndex);
		}
		pStream->Read(&iNumYields);
		for (int iI = 0; iI < iNumYields; iI++)
		{
			int iIndex;
			pStream->Read(&iIndex);
			ma_tradeExports.set(true, iIndex);
		}
		pStream->Read(&iNumYields);
		for (int i = 0; i < iNumYields; ++i)
		{
			YieldTypes eYield;
			int iLevel;
			pStream->Read((int*)&eYield);
			pStream->Read(&iLevel);
			ma_tradeThreshold.set(iLevel, eYield);
		}
	}
	// traderoute just-in-time - end - Nightinggale
	
	m_orderQueue.Read(pStream);

	pStream->Read(&m_iPopulationRank);
	pStream->Read(&m_bPopulationRankValid);
	pStream->Read(NUM_YIELD_TYPES, m_aiBaseYieldRank);
	pStream->Read(NUM_YIELD_TYPES, m_abBaseYieldRankValid);
	pStream->Read(NUM_YIELD_TYPES, m_aiYieldRank);
	pStream->Read(NUM_YIELD_TYPES, m_abYieldRankValid);

	pStream->Read(&iNumElts);
	m_aEventsOccured.clear();
	for (int i = 0; i < iNumElts; ++i)
	{
		EventTypes eEvent;
		pStream->Read((int*)&eEvent);
		m_aEventsOccured.push_back(eEvent);
	}

	pStream->Read(&iNumElts);
	m_aBuildingYieldChange.clear();
	for (int i = 0; i < iNumElts; ++i)
	{
		BuildingYieldChange kChange;
		kChange.read(pStream);
		m_aBuildingYieldChange.push_back(kChange);
	}

	UpdateBuildingAffectedCache(); // building affected cache - Nightinggale
	this->setAutoThresholdCache(); // transport feeder - Nightinggale
	cache_storageLossTradeValues_usingRawData(); //caching storage loss trade values
}

void CvCity::write(FDataStreamBase* pStream)
{
	uint uiFlag = 6;
	pStream->Write(uiFlag);		// flag for expansion

	// just-in-time yield arrays - start - Nightinggale
	uint arrayBitmap = 0;
	arrayBitmap |= ma_aiCustomHouseSellThreshold.hasContent() ? SAVE_BIT_CUSTOM_HOUSE_SELL_THRESHOLD : 0;
	arrayBitmap |= ma_aiCustomHouseNeverSell.hasContent()     ? SAVE_BIT_CUSTOM_HOUSE_NEVER_SELL : 0;
	arrayBitmap |= ma_OrderedStudents.hasContent()            ? SAVE_BIT_ORDERED_STUDENTS : 0;
	arrayBitmap |= ma_OrderedStudentsRepeat.hasContent()      ? SAVE_BIT_ORDERED_STUDENTS_REPEAT : 0;
	// traderoute just-in-time - start - Nightinggale
	arrayBitmap |= ma_tradeImports.hasContent()               ? SAVE_BIT_TRADE_IMPORTS : 0;
	arrayBitmap |= ma_tradeExports.hasContent()               ? SAVE_BIT_TRADE_EXPORTS : 0;
	arrayBitmap |= ma_tradeThreshold.hasContent()             ? SAVE_BIT_TRADE_THRESHOLD : 0;
	// traderoute just-in-time - end - Nightinggale
	// transport feeder - start - Nightinggale
	arrayBitmap |= ma_tradeImportsMaintain.hasContent()       ? SAVE_BIT_IMPORT_FEEDER : 0;
	arrayBitmap |= ma_tradeStopAutoImport.hasContent()        ? SAVE_BIT_IMPORT_STOP : 0;
	// transport feeder - end - Nightinggale
	// R&R mod, vetiarvind, max yield import limit - Start
	arrayBitmap |= ma_tradeMaxThreshold.hasContent()					? SAVE_BIT_TRADE_MAX_THRESHOLD : 0;
	// R&R mod, vetiarvind, max yield import limit - End
	pStream->Write(arrayBitmap);
	// just-in-time yield arrays - end - Nightinggale
	
	pStream->Write(m_iID);
	pStream->Write(m_iX);
	pStream->Write(m_iY);
	pStream->Write(m_iRallyX);
	pStream->Write(m_iRallyY);
	pStream->Write(m_iGameTurnFounded);
	pStream->Write(m_iGameTurnAcquired);
	pStream->Write(m_iHighestPopulation);
	pStream->Write(m_iWorkingPopulation);
	pStream->Write(m_iNumBuildings);
	pStream->Write(m_iHealRate);
	pStream->Write(m_iFoodKept);
	pStream->Write(m_iMaxFoodKeptPercent);
	pStream->Write(m_iOverflowProduction);
	pStream->Write(m_iMilitaryProductionModifier);
	pStream->Write(m_iBuildingDefense);
	pStream->Write(m_iBuildingBombardDefense);
	pStream->Write(m_iFreeExperience);
	pStream->Write(m_iDefenseDamage);
	pStream->Write(m_iLastDefenseDamage);
	pStream->Write(m_iOccupationTimer);
	pStream->Write(m_iCultureUpdateTimer);
	pStream->Write(m_iCitySizeBoost);
	pStream->Write(m_iHammers);
	pStream->Write(m_iMissionaryRate);
	pStream->Write(m_iNativeTradeRate); // WTP, ray, Native Trade Posts - START
	pStream->Write(m_iTradePostGold); // WTP, ray, Native Trade Posts - START
	pStream->Write(m_bStirredUp); // R&R, ray , Stirring Up Natives
	pStream->Write(m_iWorksWaterCount);
	pStream->Write(m_iRebelSentiment);
	pStream->Write(m_iCityHealth); // R&R, ray, Health
	pStream->Write(m_iCityHappiness); // WTP, ray, Happiness - START
	pStream->Write(m_iCityUnHappiness); // WTP, ray, Happiness - START
	pStream->Write(m_iCityTimerFestivitiesOrUnrest); // WTP, ray, Happiness - START
	pStream->Write(m_iTeachUnitMultiplier);
	pStream->Write(m_iEducationThresholdMultiplier);

	pStream->Write(m_bNeverLost);
	pStream->Write(m_bBombarded);
	pStream->Write(m_bProductionAutomated);
	pStream->Write(m_bWallOverride);
	// m_bInfoDirty not saved...
	// m_bLayoutDirty not saved...

	pStream->Write(m_eOwner);
	pStream->Write(m_ePreviousOwner);
	pStream->Write(m_eOriginalOwner);
	pStream->Write(m_eCultureLevel);
	pStream->Write(m_eTeachUnitClass);
	pStream->Write(m_eMissionaryPlayer);
	pStream->Write(m_eTradePostPlayer); // WTP, ray, Native Trade Posts - START

	pStream->Write(NUM_YIELD_TYPES, m_aiLandPlotYield); // R&R, ray, Landplot Yields
	pStream->Write(NUM_YIELD_TYPES, m_aiSeaPlotYield);
	pStream->Write(NUM_YIELD_TYPES, m_aiRiverPlotYield);
	pStream->Write(NUM_YIELD_TYPES, m_aiYieldRateModifier);
	pStream->Write(NUM_YIELD_TYPES, m_aiYieldStored);
	pStream->Write(NUM_YIELD_TYPES, m_aiYieldRushed);
	// R&R, Androrc, Domestic Market
	pStream->Write(NUM_YIELD_TYPES, m_aiYieldBuyPrice);
	//Androrc End

	pStream->Write(m_ePreferredYieldAtCityPlot);

	// R&R, ray, finishing Custom House Screen
	ma_aiCustomHouseSellThreshold.write(pStream, arrayBitmap & SAVE_BIT_CUSTOM_HOUSE_SELL_THRESHOLD);
	ma_aiCustomHouseNeverSell.write    (pStream, arrayBitmap & SAVE_BIT_CUSTOM_HOUSE_NEVER_SELL);
	// R&R, ray, finishing Custom House Screen END

	// Teacher List - start - Nightinggale
	ma_OrderedStudents.write(      pStream, arrayBitmap & SAVE_BIT_ORDERED_STUDENTS);
	ma_OrderedStudentsRepeat.write(pStream, arrayBitmap & SAVE_BIT_ORDERED_STUDENTS_REPEAT);
	// Teacher List - end - Nightinggale

	pStream->Write(NUM_DOMAIN_TYPES, m_aiDomainFreeExperience);
	pStream->Write(NUM_DOMAIN_TYPES, m_aiDomainProductionModifier);
	pStream->Write(MAX_PLAYERS, m_aiCulture);

	pStream->Write(MAX_PLAYERS, m_abEverOwned);
	pStream->Write(MAX_TEAMS, m_abRevealed);
	pStream->Write(MAX_TEAMS, m_abScoutVisited);

	pStream->WriteString(m_szName);
	pStream->WriteString(m_szScriptData);

	pStream->Write(GC.getNumBuildingInfos(), m_paiBuildingProduction);
	pStream->Write(GC.getNumBuildingInfos(), m_paiBuildingProductionTime);
	pStream->Write(GC.getNumBuildingInfos(), m_paiBuildingOriginalOwner);
	pStream->Write(GC.getNumBuildingInfos(), m_paiBuildingOriginalTime);
	pStream->Write(GC.getNumUnitInfos(), m_paiUnitProduction);
	pStream->Write(GC.getNumUnitInfos(), m_paiUnitProductionTime);
	pStream->Write(GC.getNumUnitInfos(), m_aiSpecialistWeights);
	pStream->Write(GC.getNumUnitCombatInfos(), m_paiUnitCombatFreeExperience);
	pStream->Write(GC.getNumPromotionInfos(), m_paiFreePromotionCount);
	pStream->Write(GC.getNumBuildingInfos(), m_pabHasRealBuilding);
	pStream->Write(GC.getNumBuildingInfos(), m_pabHasFreeBuilding);

	pStream->Write(NUM_CITY_PLOTS, m_paiWorkingPlot);

	pStream->Write((int)m_aPopulationUnits.size());
	for(int i=0;i<(int)m_aPopulationUnits.size();i++)
	{
		m_aPopulationUnits[i]->write(pStream);
	}

	// traderoute just-in-time - start - Nightinggale
	ma_tradeImports.write(pStream, arrayBitmap & SAVE_BIT_TRADE_IMPORTS);
	ma_tradeExports.write(pStream, arrayBitmap & SAVE_BIT_TRADE_EXPORTS);
	ma_tradeThreshold.write(pStream, arrayBitmap & SAVE_BIT_TRADE_THRESHOLD);
	// traderoute just-in-time - end - Nightinggale
	// transport feeder - start - Nightinggale
 	ma_tradeImportsMaintain.write(pStream, arrayBitmap & SAVE_BIT_IMPORT_FEEDER);
	ma_tradeStopAutoImport.write(pStream, arrayBitmap & SAVE_BIT_IMPORT_STOP);
 	// transport feeder - end - Nightinggale
	// R&R mod, vetiarvind, max yield import limit - start
	ma_tradeMaxThreshold.write(pStream, arrayBitmap & SAVE_BIT_TRADE_MAX_THRESHOLD);
	// R&R mod, vetiarvind, max yield import limit - end
	m_orderQueue.Write(pStream);

	pStream->Write(m_iPopulationRank);
	pStream->Write(m_bPopulationRankValid);
	pStream->Write(NUM_YIELD_TYPES, m_aiBaseYieldRank);
	pStream->Write(NUM_YIELD_TYPES, m_abBaseYieldRankValid);
	pStream->Write(NUM_YIELD_TYPES, m_aiYieldRank);
	pStream->Write(NUM_YIELD_TYPES, m_abYieldRankValid);

	pStream->Write(m_aEventsOccured.size());
	for (std::vector<EventTypes>::iterator it = m_aEventsOccured.begin(); it != m_aEventsOccured.end(); ++it)
	{
		pStream->Write(*it);
	}

	pStream->Write(m_aBuildingYieldChange.size());
	for (std::vector<BuildingYieldChange>::iterator it = m_aBuildingYieldChange.begin(); it != m_aBuildingYieldChange.end(); ++it)
	{
		(*it).write(pStream);
	}
}


//------------------------------------------------------------------------------------------------
class VisibleBuildingComparator
{
public:
	bool operator() (BuildingTypes e1, BuildingTypes e2)
	{
		if(GC.getBuildingInfo(e1).getVisibilityPriority() > GC.getBuildingInfo(e2).getVisibilityPriority())
			return true;
		else if(GC.getBuildingInfo(e1).getVisibilityPriority() == GC.getBuildingInfo(e2).getVisibilityPriority())
		{
			//break ties by building type higher building type
			if(e1 > e2)
				return true;
		}

		return false;
	}
};

void CvCity::getVisibleBuildings(std::list<BuildingTypes>& kChosenVisible, int& iChosenNumGenerics) const
{
	const int iNumBuildings = GC.getNumBuildingInfos();

	std::vector<BuildingTypes> kVisible;

	for (int i = 0; i < iNumBuildings; i++)
	{
		const BuildingTypes eCurType = (BuildingTypes)i;

		if (!isHasBuilding(eCurType))
			continue;

		const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eCurType);
		const bool bNationalWonder = kBuilding.isNationalWonder();
		const bool bDefense = kBuilding.getDefenseModifier() > 0;

		if (bNationalWonder || bDefense)
		{
			// Defensive structures and national wonders are always visible
			kChosenVisible.push_back(eCurType);
		}
		else
		{
			// All other buildings are candidates for being non-visible
			kVisible.push_back(eCurType);
		}
	}

	// Unused feature, disabled until we determine if it's useful
	/*
	BonusTypes eBonus = plot()->getBonusType();
	if (eBonus != NO_BONUS)
	{
		BuildingTypes eBonusBuilding = (BuildingTypes)GC.getBonusInfo(eBonus).getBuilding();
		if (eBonusBuilding != NO_BUILDING)
		{
			kVisible.push_back(eBonusBuilding);
		}
	}
	*/

	// sort the visible ones by decreasing priority
	const VisibleBuildingComparator kComp;
	std::sort(kVisible.begin(), kVisible.end(), kComp);

	// how big is this city, in terms of buildings?
	// general rule: no more than fPercentUnique percent of a city can be uniques
	int iTotalVisibleBuildings;
	const static bool bIsExp = !stricmp(GC.getDefineSTRING("GAME_CITY_SIZE_METHOD"), "METHOD_EXPONENTIAL");

	if (bIsExp)
	{
		const static float fCitySizeExpMod = GC.getDefineFLOAT("GAME_CITY_SIZE_EXP_MODIFIER");
		int iCityScaleMod = ((int)(pow((float)getPopulation(), fCitySizeExpMod))) * 2;
		iTotalVisibleBuildings = (10 + iCityScaleMod);
	}
	else
	{
		const static float fLo = GC.getDefineFLOAT("GAME_CITY_SIZE_LINMAP_AT_0");
		const static float fHi = GC.getDefineFLOAT("GAME_CITY_SIZE_LINMAP_AT_50");
		const float fCurSize = (float)getPopulation();
		iTotalVisibleBuildings = int(((fHi - fLo) / 50.0f) * fCurSize + fLo);
	}

	const static float fMaxUniquePercent = GC.getDefineFLOAT("GAME_CITY_SIZE_MAX_PERCENT_UNIQUE");
	const int iMaxNumUniques = (int)(fMaxUniquePercent * iTotalVisibleBuildings);

	// compute how many buildings are generics vs. unique Civ buildings?
	int iNumUniques;
	if ((int)kVisible.size() > iMaxNumUniques)
	{
		iNumUniques = iMaxNumUniques;
	}
	else
	{
		iNumUniques = kVisible.size();
	}
	const int iNumGenerics = iTotalVisibleBuildings - iNumUniques + getCitySizeBoost();

	// return
	iChosenNumGenerics = iNumGenerics;
	for (int i = 0; i < iNumUniques; i++)
	{
		kChosenVisible.push_back(kVisible[i]);
	}
}

static int natGetDeterministicRandom(int iMin, int iMax, int iSeedX, int iSeedY)
{
	srand(7297 * iSeedX + 2909  * iSeedY);
	return (rand() % (iMax - iMin)) + iMin;
}

void CvCity::getVisibleEffects(ZoomLevelTypes eCurZoom, std::vector<const TCHAR*>& kEffectNames) const
{
	if (isOccupation() && isVisible(getTeam(), false))
	{
		if (eCurZoom  == ZOOM_DETAIL)
		{
			kEffectNames.push_back("EFFECT_CITY_BIG_BURNING_SMOKE");
			kEffectNames.push_back("EFFECT_CITY_FIRE");
		}
		else
		{
			kEffectNames.push_back("EFFECT_CITY_BIG_BURNING_SMOKE");
		}
		return;
	} 
}

void CvCity::getCityBillboardSizeIconColors(NiColorA& kDotColor, NiColorA& kTextColor) const
{
	NiColorA kPlayerColor = GC.getColorInfo((ColorTypes) GC.getPlayerColorInfo(GET_PLAYER(getOwnerINLINE()).getPlayerColor()).getColorTypePrimary()).getColor();
	NiColorA kGrowing;
	kGrowing = NiColorA(0.73f,1,0.73f,1);
	NiColorA kShrinking(1,0.73f,0.73f,1);
	NiColorA kStagnant(0.83f,0.83f,0.83f,1);
	NiColorA kUnknown(.5f,.5f,.5f,1);
	NiColorA kWhite(1,1,1,1);
	NiColorA kBlack(0,0,0,1);

	if ((getTeam() == GC.getGameINLINE().getActiveTeam()))
	{
		if (foodDifference() < 0)
		{
			if ((foodDifference() == -1) && (getFood() >= ((75 * growthThreshold()) / 100)))
			{
				kDotColor = kStagnant;
				kTextColor = kBlack;
			}
			else
			{
				kDotColor = kShrinking;
				kTextColor = kBlack;
			}
		}
		else if (foodDifference() > 0)
		{
			kDotColor = kGrowing;
			kTextColor = kBlack;
		}
		else if (foodDifference() == 0)
		{
			kDotColor = kStagnant;
			kTextColor = kBlack;
		}
	}
	else
	{
		kDotColor = kPlayerColor;
		NiColorA kPlayerSecondaryColor = GC.getColorInfo((ColorTypes) GC.getPlayerColorInfo(GET_PLAYER(getOwnerINLINE()).getPlayerColor()).getColorTypeSecondary()).getColor();
		kTextColor = kPlayerSecondaryColor;
	}
}

const TCHAR* CvCity::getCityBillboardProductionIcon() const
{
	if (isNative() && getOwnerINLINE() != GC.getGameINLINE().getActivePlayer() && AI_getDesiredYield() != NO_YIELD)
	{
		return GC.getYieldInfo(AI_getDesiredYield()).getButton();
	}

	if (canBeSelected() && isProduction())
	{
		CLLNode<OrderData>* pOrderNode;
		pOrderNode = headOrderQueueNode();
		FAssert(pOrderNode != NULL);

		const CvInfoBase* pInfo = getProductionInfo();
		if (pInfo != NULL)
		{
			return pInfo->getButton();
		}
	}

	return ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_NOPRODUCTION")->getPath();
}

bool CvCity::getCityBillboardTopBarValues(float& fStored, float& fRate, float& fRateExtra) const
{
	if (!canBeSelected())
	{
		return false;
	}

	// food bar
	float fThreshold = std::max(1.0f, (float) growthThreshold());
	if (foodDifference() < 0)
	{
		fStored = std::max(0.0f, (float) (getFood() + foodDifference())) / fThreshold;
		fRate = 0;
		fRateExtra = (float) std::min(-foodDifference(), getFood()) / fThreshold;
	}
	else
	{
		fStored = (float) getFood() / fThreshold;
		fRate = (float) foodDifference() / fThreshold;
		fRateExtra = 0;
	}

	return true;
}

bool CvCity::getCityBillboardBottomBarValues(float& fStored, float& fRate, float& fRateExtra) const
{
	// configure the production bars
	if (!canBeSelected() && !isScoutVisited(GC.getGameINLINE().getActiveTeam()))
	{
		return false;
	}

	if (!isNative() || getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
	{
		float fProductionDiff = (float) getCurrentProductionDifference(true);
		float fProductionNeeded = std::max(1.0f, (float) getProductionNeeded(YIELD_HAMMERS));

		fStored = (float) getProduction() / fProductionNeeded;
		fRate = fProductionDiff / fProductionNeeded;
		fRateExtra = 0.0f;
	}
	else
	{
		fStored = 0;
		fRate = 0;
		fRateExtra = 0;

		// show training progress
		int iBestTimer = MAX_INT;
		CvUnit* pBestUnit = NULL;
		CvPlot* pPlot = plot();
		CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = plot()->nextUnitNode(pUnitNode);

			if (pLoopUnit->getUnitTravelState() == UNIT_TRAVEL_STATE_LIVE_AMONG_NATIVES)
			{
				if (pLoopUnit->getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
				{
					if (pLoopUnit->getUnitTravelTimer() < iBestTimer)
					{
						iBestTimer = pLoopUnit->getUnitTravelTimer();
						pBestUnit = pLoopUnit;
					}
				}
			}
		}

		if (pBestUnit != NULL)
		{
			int iLearnTime = pBestUnit->getLearnTime();
			float fLearnTime = std::max(1.0f, (float) iLearnTime);
			fStored = (iLearnTime - iBestTimer) / fLearnTime;
			fRate = 1.0f / fLearnTime;
		}
	}

	return true;
}

bool CvCity::isValidBuildingLocation(BuildingTypes eBuilding) const
{
	// if both the river and water flags are set, we require one of the two conditions, not both
	if (GC.getBuildingInfo(eBuilding).isWater())
	{
		if (!GC.getBuildingInfo(eBuilding).isRiver() || !plot()->isRiver())
		{
			if (!isCoastal(GC.getBuildingInfo(eBuilding).getMinAreaSize()))
			{
				return false;
			}
		}
	}
	else
	{
		if (area()->getNumTiles() < GC.getBuildingInfo(eBuilding).getMinAreaSize())
		{
			return false;
		}

		if (GC.getBuildingInfo(eBuilding).isRiver())
		{
			if (!(plot()->isRiver()))
			{
				return false;
			}
		}
	}

	return true;
}

int CvCity::getTriggerValue(EventTriggerTypes eTrigger) const
{
	FAssert(eTrigger >= 0);
	FAssert(eTrigger < GC.getNumEventTriggerInfos());

	CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(eTrigger);


	if (!isEmpty(kTrigger.getPythonCanDoCity()))
	{
		long lResult;

		CyArgsList argsList;
		argsList.add(eTrigger);
		argsList.add(getOwnerINLINE());
		argsList.add(getID());

		gDLL->getPythonIFace()->callFunction(PYRandomEventModule, kTrigger.getPythonCanDoCity(), argsList.makeFunctionArgs(), &lResult);

		if (0 == lResult)
		{
			return MIN_INT;
		}
	}

	if (kTrigger.getNumBuildings() > 0 && kTrigger.getNumBuildingsRequired() > 0)
	{
		bool bFoundValid = false;

		for (int i = 0; i < kTrigger.getNumBuildingsRequired(); ++i)
		{
			if (kTrigger.getBuildingRequired(i) != NO_BUILDINGCLASS)
			{
				BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(kTrigger.getBuildingRequired(i));
				if (NO_BUILDING != eBuilding)
				{
					if (isHasRealBuilding(eBuilding))
					{
						bFoundValid = true;
					}
				}
			}
		}

		if (!bFoundValid)
		{
			return MIN_INT;
		}
	}

	if (kTrigger.getMinPopulation() > 0)
	{
		if (getPopulation() < kTrigger.getMinPopulation())
		{
			return MIN_INT;
		}
	}

	if (kTrigger.getMaxPopulation() > 0)
	{
		if (getPopulation() > kTrigger.getMaxPopulation())
		{
			return MIN_INT;
		}
	}

	if (kTrigger.isPrereqEventCity() && kTrigger.getNumPrereqEvents() > 0)
	{
		bool bFoundValid = true;

		for (int iI = 0; iI < kTrigger.getNumPrereqEvents(); ++iI)
		{
			if (!isEventOccured((EventTypes)kTrigger.getPrereqEvent(iI)))
			{
				bFoundValid = false;
				break;
			}
		}

		if (!bFoundValid)
		{
			return MIN_INT;
		}
	}


	int iValue = 0;

	if (0 == getFood() && kTrigger.getCityFoodWeight() > 0)
	{
		return MIN_INT;
	}

	iValue += getFood() * kTrigger.getCityFoodWeight();

	return iValue;
}

bool CvCity::canApplyEvent(EventTypes eEvent, const EventTriggeredData& kTriggeredData) const
{
	CvEventInfo& kEvent = GC.getEventInfo(eEvent);

	if (!kEvent.isCityEffect() && !kEvent.isOtherPlayerCityEffect())
	{
		return true;
	}

	if (-1 == kTriggeredData.m_iCityId && kEvent.isCityEffect())
	{
		return false;
	}

	if (-1 == kTriggeredData.m_iOtherPlayerCityId && kEvent.isOtherPlayerCityEffect())
	{
		return false;
	}

	if (kEvent.getFood() + ((100 + kEvent.getFoodPercent()) * getFood()) / 100 < 0)
	{
		return false;
	}

	if (kEvent.getPopulationChange() + getPopulation() <= 0)
	{
		return false;
	}

	if (kEvent.getCulture() + getCulture(getOwnerINLINE()) < 0)
	{
		return false;
	}

	if (kEvent.getBuildingClass() != NO_BUILDINGCLASS)
	{
		BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(kEvent.getBuildingClass());
		if (eBuilding == NO_BUILDING)
		{
			return false;
		}

		if (kEvent.getBuildingChange() > 0)
		{
			if (isHasRealBuilding(eBuilding))
			{
				return false;
			}
		}
		else if (kEvent.getBuildingChange() < 0)
		{
			if (!isHasRealBuilding(eBuilding) && kEvent.getBuildingChange() < 0)
			{
				return false;
			}
		}
	}

	if (kEvent.getMinPillage() > 0)
	{
		int iNumImprovements = 0;
		for (int i = 0; i < NUM_CITY_PLOTS; ++i)
		{
			if (CITY_HOME_PLOT != i)
			{
				CvPlot* pPlot = getCityIndexPlot(i);
				if (NULL != pPlot && pPlot->getOwnerINLINE() == getOwnerINLINE())
				{
					if (NO_IMPROVEMENT != pPlot->getImprovementType() && !GC.getImprovementInfo(pPlot->getImprovementType()).isPermanent())
					{
						++iNumImprovements;
					}
				}
			}
		}

		if (iNumImprovements < kEvent.getMinPillage())
		{
			return false;
		}
	}

	return true;
}

void CvCity::applyEvent(EventTypes eEvent, const EventTriggeredData& kTriggeredData, bool bClear)
{
	if (!canApplyEvent(eEvent, kTriggeredData))
	{
		return;
	}

	setEventOccured(eEvent, true);
	int iGrowthPercent = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
	CvEventInfo& kEvent = GC.getEventInfo(eEvent);

	if (kEvent.isCityEffect() || kEvent.isOtherPlayerCityEffect())
	{
		if (kEvent.getFood() != 0 || kEvent.getFoodPercent() != 0)
		{
			int iFoodQty = kEvent.getFood();
			iFoodQty *= iGrowthPercent;
			iFoodQty /=100;
			iFoodQty += (kEvent.getFoodPercent() * getFood()) / 100;
			changeFood( iFoodQty );
		}

		if (kEvent.getPopulationChange() != 0)
		{
			changePopulation(kEvent.getPopulationChange());
		}

		if (kEvent.getRevoltTurns() > 0)
		{
			int iRevoltTurns = kEvent.getRevoltTurns();
			iRevoltTurns  *= iGrowthPercent;
			iRevoltTurns /= 100;
			changeCultureUpdateTimer(iRevoltTurns);
			changeOccupationTimer(iRevoltTurns);
		}

		if (kEvent.getMaxPillage() > 0)
		{
			FAssert(kEvent.getMaxPillage() >= kEvent.getMinPillage());
			int iNumPillage = kEvent.getMinPillage() + GC.getGameINLINE().getSorenRandNum(kEvent.getMaxPillage() - kEvent.getMinPillage(), "Pick number of event pillaged plots");

			int iNumPillaged = 0;
			for (int i = 0; i < iNumPillage; ++i)
			{
				int iRandOffset = GC.getGameINLINE().getSorenRandNum(NUM_CITY_PLOTS, "Pick event pillage plot");
				for (int j = 0; j < NUM_CITY_PLOTS; ++j)
				{
					int iPlot = (j + iRandOffset) % NUM_CITY_PLOTS;
					if (CITY_HOME_PLOT != iPlot)
					{
						CvPlot* pPlot = getCityIndexPlot(iPlot);
						if (NULL != pPlot && pPlot->getOwnerINLINE() == getOwnerINLINE())
						{
							if (NO_IMPROVEMENT != pPlot->getImprovementType() && !GC.getImprovementInfo(pPlot->getImprovementType()).isPermanent())
							{
								CvWString szBuffer = gDLL->getText("TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED", GC.getImprovementInfo(pPlot->getImprovementType()).getTextKeyWide());
								gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGED", MESSAGE_TYPE_INFO, GC.getImprovementInfo(pPlot->getImprovementType()).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pPlot->getX_INLINE(), pPlot->getY_INLINE(), true, true);
								pPlot->setImprovementType(NO_IMPROVEMENT);
								++iNumPillaged;
								break;
							}
						}
					}
				}
			}

			PlayerTypes eOtherPlayer = kTriggeredData.m_eOtherPlayer;
			if (!kEvent.isCityEffect() && kEvent.isOtherPlayerCityEffect())
			{
				eOtherPlayer = kTriggeredData.m_ePlayer;
			}

			if (NO_PLAYER != eOtherPlayer)
			{
				CvWString szBuffer = gDLL->getText("TXT_KEY_EVENT_NUM_CITY_IMPROVEMENTS_DESTROYED", iNumPillaged, GET_PLAYER(getOwnerINLINE()).getCivilizationAdjectiveKey());
				gDLL->getInterfaceIFace()->addMessage(eOtherPlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGED", MESSAGE_TYPE_INFO);
			}
		}

		if (kEvent.getCulture() != 0)
		{
			int iCulture =  kEvent.getCulture();
			iCulture *= iGrowthPercent;
			iCulture /= 100;
			changeCulture(getOwnerINLINE(), iCulture, true);
		}

		// R&R, ray, change for Health in Events
		if (kEvent.getHealth() != 0)
		{
			changeCityHealth(kEvent.getHealth());
		}
	}


	if (kEvent.getUnitClass() != NO_UNITCLASS)
	{
		UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(kEvent.getUnitClass());
		if (eUnit != NO_UNIT)
		{
			for (int i = 0; i < kEvent.getNumUnits(); ++i)
			{
				GET_PLAYER(getOwnerINLINE()).initUnit(eUnit, (ProfessionTypes) GC.getUnitInfo(eUnit).getDefaultProfession(), getX_INLINE(), getY_INLINE());
			}
		}
	}

	if (kEvent.getBuildingClass() != NO_BUILDINGCLASS)
	{
		BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(kEvent.getBuildingClass());
		if (eBuilding != NO_BUILDING)
		{
			if (0 != kEvent.getBuildingChange())
			{
				setHasRealBuilding(eBuilding, kEvent.getBuildingChange() > 0);
			}
		}
	}

	if (kEvent.getNumBuildingYieldChanges() > 0)
	{
		for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); ++iBuildingClass)
		{
			for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
			{
				setBuildingYieldChange((BuildingClassTypes)iBuildingClass, (YieldTypes)iYield, getBuildingYieldChange((BuildingClassTypes)iBuildingClass, (YieldTypes)iYield) + kEvent.getBuildingYieldChange(iBuildingClass, iYield));
			}
		}
	}

	if (bClear)
	{
		for (int i = 0; i < GC.getNumEventInfos(); ++i)
		{
			setEventOccured((EventTypes)i, false);
		}
	}
}

bool CvCity::isEventOccured(EventTypes eEvent) const
{
	for (std::vector<EventTypes>::const_iterator it = m_aEventsOccured.begin(); it != m_aEventsOccured.end(); ++it)
	{
		if (*it == eEvent)
		{
			return true;
		}
	}

	return false;
}

void CvCity::setEventOccured(EventTypes eEvent, bool bOccured)
{
	for (std::vector<EventTypes>::iterator it = m_aEventsOccured.begin(); it != m_aEventsOccured.end(); ++it)
	{
		if (*it == eEvent)
		{
			if (!bOccured)
			{
				m_aEventsOccured.erase(it);
			}
			return;
		}
	}

	if (bOccured)
	{
		m_aEventsOccured.push_back(eEvent);
	}
}

// CACHE: cache frequently used values
///////////////////////////////////////
void CvCity::invalidatePopulationRankCache()
{
	m_bPopulationRankValid = false;
}

void CvCity::invalidateYieldRankCache(YieldTypes eYield)
{
	FAssertMsg(eYield >= NO_YIELD && eYield < NUM_YIELD_TYPES, "invalidateYieldRankCache passed bogus yield index");

	if (eYield == NO_YIELD)
	{
		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			m_abBaseYieldRankValid[iI] = false;
			m_abYieldRankValid[iI] = false;
		}
	}
	else
	{
		m_abBaseYieldRankValid[eYield] = false;
		m_abYieldRankValid[eYield] = false;
	}
}

int CvCity::getBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield) const
{
	for (std::vector<BuildingYieldChange>::const_iterator it = m_aBuildingYieldChange.begin(); it != m_aBuildingYieldChange.end(); ++it)
	{
		if ((*it).eBuildingClass == eBuildingClass && (*it).eYield == eYield)
		{
			return (*it).iChange;
		}
	}

	return 0;
}

void CvCity::setBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield, int iChange)
{
	for (std::vector<BuildingYieldChange>::iterator it = m_aBuildingYieldChange.begin(); it != m_aBuildingYieldChange.end(); ++it)
	{
		if ((*it).eBuildingClass == eBuildingClass && (*it).eYield == eYield)
		{
			int iOldChange = (*it).iChange;
			if (iOldChange != iChange)
			{

				if (iChange == 0)
				{
					m_aBuildingYieldChange.erase(it);
				}
				else
				{
					(*it).iChange = iChange;
				}

				BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(eBuildingClass);
				if (NO_BUILDING != eBuilding)
				{
					if (isHasBuilding(eBuilding))
					{
						setYieldRateDirty();
					}
				}
			}

			return;
		}
	}

	if (0 != iChange)
	{
		BuildingYieldChange kChange;
		kChange.eBuildingClass = eBuildingClass;
		kChange.eYield = eYield;
		kChange.iChange = iChange;
		m_aBuildingYieldChange.push_back(kChange);

		BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(eBuildingClass);
		if (NO_BUILDING != eBuilding)
		{
			if (isHasBuilding(eBuilding))
			{
				setYieldRateDirty();
			}
		}
	}
}

void CvCity::changeBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield, int iChange)
{
	setBuildingYieldChange(eBuildingClass, eYield, getBuildingYieldChange(eBuildingClass, eYield) + iChange);
}

void CvCity::liberate(bool bConquest)
{
	CvPlot* pPlot = plot();
	PlayerTypes ePlayer = getLiberationPlayer(bConquest);
	PlayerTypes eOwner = getOwnerINLINE();

	if (NO_PLAYER != ePlayer)
	{
		int iOldOwnerCulture = getCulture(eOwner);

		CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_CITY_LIBERATED", getNameKey(), GET_PLAYER(eOwner).getNameKey(), GET_PLAYER(ePlayer).getCivilizationAdjectiveKey());
		for (int iI = 0; iI < MAX_PLAYERS; ++iI)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
				{
					gDLL->getInterfaceIFace()->addMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_REVOLTEND", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
				}
			}
		}
		GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, eOwner, szBuffer, getX_INLINE(), getY_INLINE(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));

		GET_PLAYER(ePlayer).acquireCity(this, false, true);
		GET_PLAYER(ePlayer).AI_changeMemoryCount(eOwner, MEMORY_LIBERATED_CITIES, 1);

		if (NULL != pPlot)
		{
			CvCity* pCity = pPlot->getPlotCity();
			if (NULL != pCity)
			{
				pCity->setCulture(ePlayer, pCity->getCulture(ePlayer) + iOldOwnerCulture / 2, true);
			}
		}
	}
}

PlayerTypes CvCity::getLiberationPlayer(bool bConquest) const
{
	if (isCapital())
	{
		return NO_PLAYER;
	}

	if (isNative())
	{
		return NO_PLAYER;
	}

	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (kLoopPlayer.isAlive() && kLoopPlayer.getParent() == getOwnerINLINE())
		{
			CvCity* pLoopCapital = kLoopPlayer.getPrimaryCity();
			if (NULL != pLoopCapital)
			{
				if (pLoopCapital->area() == area())
				{
					return (PlayerTypes)iPlayer;
				}
			}
		}
	}

	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());

	PlayerTypes eBestPlayer = NO_PLAYER;
	int iBestValue = 0;

	int iTotalCulture = countTotalCulture();

	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);

		if (kLoopPlayer.isAlive())
		{
			if (kLoopPlayer.canReceiveTradeCity(getOwnerINLINE()))
			{
				CvCity* pCapital = kLoopPlayer.getPrimaryCity();
				if (NULL != pCapital)
				{
					int iCapitalDistance = ::plotDistance(getX_INLINE(), getY_INLINE(), pCapital->getX_INLINE(), pCapital->getY_INLINE());
					if (area() != pCapital->area())
					{
						iCapitalDistance *= 2;
					}

					int iCulture = getCulture((PlayerTypes)iPlayer);

					if (bConquest)
					{
						if (iPlayer == getOriginalOwner())
						{
							iCulture *= 3;
							iCulture /= 2;
						}
					}

					if (GET_PLAYER((PlayerTypes)iPlayer).getTeam() == getTeam())
					{
						iCulture *= 2;
						iCulture = (iCulture + iTotalCulture) / 2;
					}

					int iValue = std::max(1, iCulture) / std::max(1, iCapitalDistance);

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						eBestPlayer = (PlayerTypes)iPlayer;
					}
				}
			}
		}
	}

	if (NO_PLAYER != eBestPlayer)
	{
		if (getOwnerINLINE() == eBestPlayer)
		{
			return NO_PLAYER;
		}

		for (int iPlot = 0; iPlot < NUM_CITY_PLOTS; ++iPlot)
		{
			CvPlot* pLoopPlot = ::plotCity(getX_INLINE(), getY_INLINE(), iPlot);

			if (NULL != pLoopPlot)
			{
				if (pLoopPlot->isVisibleEnemyUnit(eBestPlayer))
				{
					return NO_PLAYER;
				}
			}
		}
	}

	return eBestPlayer;
}

int CvCity::getBestYieldAmountAvailable(ProfessionTypes eProfession, const CvUnit* pUnit) const
{
	if (eProfession == NO_PROFESSION)
	{
		return 0;
	}

	// R&R, ray , MYCP partially based on code of Aymerick - START
	FAssert(GC.getProfessionInfo(eProfession).getYieldsProduced(0) != NO_YIELD);
	// R&R, ray , MYCP partially based on code of Aymerick - END
	int iBestYieldAvailable = 0;

	if(pUnit != NULL)
	{
		CvPlot* pWorkingPlot = getPlotWorkedByUnit(pUnit);
		if(pWorkingPlot != NULL)
		{
			iBestYieldAvailable = pWorkingPlot->calculatePotentialProfessionYieldAmount(eProfession, pUnit, false);
		}
	}

	for (int iJ = 0; iJ < NUM_CITY_PLOTS; ++iJ)
	{
		if (iJ != CITY_HOME_PLOT)
		{
			if (!isUnitWorkingPlot(iJ))
			{
				CvPlot* pPlot = getCityIndexPlot(iJ);

				if (NULL != pPlot && canWork(pPlot))
				{
					int iYieldAmount = pPlot->calculatePotentialProfessionYieldAmount(eProfession, pUnit, false);
					if (iYieldAmount > iBestYieldAvailable)
					{
						iBestYieldAvailable = iYieldAmount;
					}
				}
			}
		}
	}

	if (pUnit->canHaveProfession(eProfession, false))
	{
		iBestYieldAvailable = std::max(iBestYieldAvailable, getProfessionOutput(eProfession, pUnit));
	}

	return iBestYieldAvailable;
}
// R&R, ray , MYCP partially based on code of Aymerick - START
int CvCity::getBestYieldsAmountAvailable(YieldTypes eYield, ProfessionTypes eProfession, const CvUnit* pUnit) const
{
	if (eProfession == NO_PROFESSION)
	{
		return 0;
	}
	
	FAssert(eYield != NO_YIELD);
	
	int iBestYieldAvailable = 0;

	if(pUnit != NULL)
	{
		CvPlot* pWorkingPlot = getPlotWorkedByUnit(pUnit);
		if(pWorkingPlot != NULL)
		{
			iBestYieldAvailable = pWorkingPlot->calculatePotentialProfessionYieldsAmount(eYield, eProfession, pUnit, false);
		}
	}

	for (int iJ = 0; iJ < NUM_CITY_PLOTS; ++iJ)
	{
		if (iJ != CITY_HOME_PLOT)
		{
			if (!isUnitWorkingPlot(iJ))
			{
				CvPlot* pPlot = getCityIndexPlot(iJ);

				if (NULL != pPlot && canWork(pPlot))
				{
					int iYieldAmount = pPlot->calculatePotentialProfessionYieldsAmount(eYield, eProfession, pUnit, false);
					if (iYieldAmount > iBestYieldAvailable)
					{
						iBestYieldAvailable = iYieldAmount;
					}
				}
			}
		}
	}

	if (pUnit->canHaveProfession(eProfession, false))
	{
		iBestYieldAvailable = std::max(iBestYieldAvailable, getProfessionOutput(eProfession, pUnit));
	}

	return iBestYieldAvailable;
}
// R&R, ray , MYCP partially based on code of Aymerick - END
void CvCity::addPopulationUnit(CvUnit* pUnit, ProfessionTypes eProfession)
{
	if (NULL == pUnit)
	{
		FAssert(false);
		return;
	}
	
	CvUnit* pTransferUnit = GET_PLAYER(pUnit->getOwnerINLINE()).getAndRemoveUnit(pUnit->getID());
	FAssert(pTransferUnit == pUnit);

	int iOldPopulation = getPopulation();
	m_aPopulationUnits.push_back(pTransferUnit);
	area()->changePower(getOwnerINLINE(), pTransferUnit->getPower());
	setYieldRateDirty();
	pTransferUnit->setProfession(eProfession);
	
	pTransferUnit->setColonistLocked(false);

	updatePopulation(iOldPopulation);

	gDLL->getEventReporterIFace()->populationJoined(getOwnerINLINE(), getID(), pTransferUnit->getID());
}

bool CvCity::removePopulationUnit(CvUnit* pUnit, bool bDelete, ProfessionTypes eProfession, bool bConquest)
{	
	int iUnitIndex = getPopulationUnitIndex(pUnit);
	if(iUnitIndex < 0)
	{
		FAssertMsg(false, "Could not find unit in city");
		return false;
	}

	if (!pUnit->canHaveProfession(eProfession, false))
	{
		FAssertMsg(false, "Illegal Profession");
		pUnit->setProfession(NO_PROFESSION);
		return false;
	}

	pUnit->setColonistLocked(false);

	//remove unit from worked plots
	CvPlot* pWorkedPlot = getPlotWorkedByUnit(pUnit);
	if (pWorkedPlot != NULL)
	{
			clearUnitWorkingPlot(pWorkedPlot);
	}

	int iOldPopulation = getPopulation();

	FAssert(pUnit->getOwnerINLINE() == getOwnerINLINE());
	m_aPopulationUnits.erase(std::remove(m_aPopulationUnits.begin(), m_aPopulationUnits.end(), pUnit));
	area()->changePower(getOwnerINLINE(), -pUnit->getPower());
	setYieldRateDirty();

	updatePopulation(iOldPopulation);

	if (bDelete)
	{
		pUnit->updateOwnerCache(-1);
		SAFE_DELETE(pUnit);
	}
	else
	{
		//transfer back to player
		GET_PLAYER(getOwnerINLINE()).addExistingUnit(pUnit);
		pUnit->addToMap(getX_INLINE(), getY_INLINE());
		pUnit->setProfession(eProfession);

		if (pUnit->getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->insertIntoSelectionList(pUnit, true, false);
		}

		gDLL->getEventReporterIFace()->populationUnjoined(getOwnerINLINE(), getID(), pUnit->getID());
	}

	// R&R Abandon City, ray START
	// Erik: Suppress the abandon message if city was razed due to conquest
	if (!bConquest && getPopulation() < 1 && isHuman() && (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())) // R&R, ray, code adjusted by checking for ActivePlayer to prevent wrong messages
	{
		CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_TEXT);
		pInfo->setText(gDLL->getText("ABANDONING_CITY"));
		gDLL->getInterfaceIFace()->addPopup(pInfo, getOwnerINLINE(), true, true);
		
	}
	// R&R Abandon City, ray END

	return true;
}

CvUnit* CvCity::removeUnitType(UnitTypes eUnit, ProfessionTypes eProfession)
{
	for (uint i = 0; i < m_aPopulationUnits.size(); ++i)
	{
		CvUnit* pUnit = m_aPopulationUnits[i];

		if (pUnit->getUnitType() == eUnit)
		{
			if (removePopulationUnit(pUnit, false, eProfession))
			{
				return pUnit;
			}
		}
	}

	return NULL;
}


void CvCity::removeNonCityPopulationUnits()
{
	bool bDone = false;
	while (!bDone)
	{
		bDone = true;
		for (uint i=0; i < m_aPopulationUnits.size(); i++)
		{
			CvUnit* pUnit = m_aPopulationUnits[i];
			ProfessionTypes eUnitProfession = pUnit->getProfession();
			if (NO_PROFESSION != eUnitProfession && !GC.getProfessionInfo(eUnitProfession).isCitizen())
			{
				//unit list changes, so break and repeat
				removePopulationUnit(pUnit, false, eUnitProfession);
				bDone = false;
				break;
			}
		}
	}
}

CvUnit* CvCity::getPopulationUnitById(int iUnitId) const
{
	for (uint i = 0; i < m_aPopulationUnits.size(); ++i)
	{
		CvUnit* pUnit = m_aPopulationUnits[i];
		if (pUnit->getID() == iUnitId)
		{
			return pUnit;
		}
	}

	return NULL;
}

CvUnit* CvCity::getPopulationUnitByIndex(int iUnitIndex) const
{
	if((iUnitIndex >= 0) && (iUnitIndex < (int)m_aPopulationUnits.size()))
	{
		return m_aPopulationUnits[iUnitIndex];
	}
	
	FAssert(false);
	return NULL;
}

int CvCity::getPopulationUnitIndex(CvUnit *pUnit) const
{
	for(int i=0;i<(int)m_aPopulationUnits.size();i++)
	{
		if(m_aPopulationUnits[i] == pUnit)
		{
			return i;
		}
	}

	return -1;
}

CvPlot* CvCity::getPlotWorkedByUnit(const CvUnit* pUnit) const
{
	for (int i = 0; i < NUM_CITY_PLOTS; ++i)
	{
		int iUnitId = m_paiWorkingPlot[i];
		if (iUnitId == pUnit->getID())
		{
			return getCityIndexPlot(i);
		}
	}

	return NULL;
}

CvUnit* CvCity::createYieldUnit(YieldTypes eYield, PlayerTypes ePlayer, int iYieldAmount)
{
	FAssert(getYieldStored(eYield) > 0);
	FAssert(iYieldAmount > 0);
	FAssert(ePlayer != NO_PLAYER);

	CvUnit* pUnit = NULL;
	CvPlayer& kOwner = GET_PLAYER(ePlayer);
	UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(kOwner.getCivilizationType()).getCivilizationUnits(GC.getYieldInfo(eYield).getUnitClass());
	if (NO_UNIT != eUnit)
	{
		pUnit = kOwner.initUnit(eUnit, (ProfessionTypes) GC.getUnitInfo(eUnit).getDefaultProfession(), getX_INLINE(), getY_INLINE(), NO_UNITAI, NO_DIRECTION, iYieldAmount);
		FAssertMsg(pUnit != NULL, "pUnit expected to be assigned (not NULL)");

		FAssert(iYieldAmount <= getYieldStored(eYield));
		changeYieldStored(eYield, -iYieldAmount);
		AI_changeTradeBalance(eYield, -iYieldAmount);
		if (NULL != pUnit)
		{
			pUnit->setHomeCity(this);
		}
	}

	return pUnit;
}

// Teacher List - start - Nightinggale
void CvCity::setOrderedStudents(UnitTypes eUnit, int iCount, bool bRepeat, bool bUpdateRepeat, bool bClearAll)
{
	if (bClearAll)
	{
		ma_OrderedStudents.reset();
		ma_OrderedStudentsRepeat.reset();
	} else {
		if (!(eUnit >= 0 && eUnit < GC.getNumUnitInfos() && iCount >= 0))
		{
			FAssert(eUnit >= 0);
			FAssert(eUnit < GC.getNumUnitInfos());
			FAssert(iCount < 0);
			return;
		}
	
		ma_OrderedStudents.set(iCount, eUnit);
		ma_OrderedStudentsRepeat.set(bRepeat, eUnit);
		if (bUpdateRepeat && iCount == 0)
		{
			checkOrderedStudentsForRepeats(eUnit);
		}
	}
	if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
	{
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
	}
}

void CvCity::checkOrderedStudentsForRepeats(UnitTypes eUnit)
{
	FAssert(eUnit >= 0);
	FAssert(eUnit < GC.getNumUnitInfos());

	if (ma_OrderedStudentsRepeat.isAllocated() && ma_OrderedStudents.isEmpty(false))
	{
		for (int iUnit = 0; iUnit < ma_OrderedStudentsRepeat.length(); iUnit++)
		{
			if (ma_OrderedStudentsRepeat.get(iUnit))
			{
				ma_OrderedStudents.set(1, iUnit);
			}
		}
		if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		}
	}
}

int CvCity::getOrderedStudents(UnitTypes eUnit)
{
	FAssert(eUnit >= 0);
	FAssert(eUnit < GC.getNumUnitInfos());
	return ma_OrderedStudents.get(eUnit);
}

bool CvCity::getOrderedStudentsRepeat(UnitTypes eUnit)
{
	FAssert(eUnit >= 0);
	FAssert(eUnit < GC.getNumUnitInfos());
	return ma_OrderedStudentsRepeat.get(eUnit);
}

// Teacher List - end - Nightinggale

UnitClassTypes CvCity::getTeachUnitClass() const
{
	return m_eTeachUnitClass;
}

void CvCity::setTeachUnitClass(UnitClassTypes eUnitClass)
{
	m_eTeachUnitClass = eUnitClass;
}

void CvCity::ejectTeachUnits()
{
	CvPlot *pPlot = plot();
	CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
	while (pUnitNode)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if (pLoopUnit->getUnitTravelState() == UNIT_TRAVEL_STATE_LIVE_AMONG_NATIVES)
		{
			pLoopUnit->setUnitTravelState(NO_UNIT_TRAVEL_STATE, false);
			pLoopUnit->setUnitTravelTimer(0);
		}
	}
}

bool CvCity::canProduceYield(YieldTypes eYield)
{
	CvPlayer& kPlayer = GET_PLAYER(getOwnerINLINE());
	for (int i = 0; i < GC.getNumProfessionInfos(); i++)
	{
		ProfessionTypes eLoopProfession = (ProfessionTypes)i;
		CvProfessionInfo& kLoopProfession = GC.getProfessionInfo(eLoopProfession);
		if (GC.getCivilizationInfo(kPlayer.getCivilizationType()).isValidProfession(eLoopProfession))
		{
			// R&R, ray , MYCP partially based on code of Aymerick - START
			if (kLoopProfession.getYieldsProduced(0) == eYield)
			// R&R, ray , MYCP partially based on code of Aymerick - END
			{
				if (kLoopProfession.isWorkPlot())
				{
					for (int j = 0; j < NUM_CITY_PLOTS; ++j)
					{
						CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), j);
						if (pLoopPlot != NULL)
						{
							if (pLoopPlot->calculateYield(eYield, false) > 0)
							{
								return true;
							}
						}
					}
				}
				else
				{
					if (getNumProfessionBuildingSlots(eLoopProfession) > 0)
					{
						return true;
					}
				}
			}
		}
	}
	
	return false;
}

// cache getMaxYieldCapacity - function namechange - Nightinggale
int CvCity::getMaxYieldCapacityUncached() const
{
	int iCapacity = GC.getGameINLINE().getCargoYieldCapacity();

	for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); ++iBuildingClass)
	{
		BuildingTypes eBuilding = (BuildingTypes) GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(iBuildingClass);
		if (eBuilding != NO_BUILDING)
		{
			if (isHasBuilding(eBuilding))
			{
				iCapacity += GC.getBuildingInfo(eBuilding).getYieldStorage() * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getStoragePercent() / 100;
			}
		}
	}
	for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); ++iTrait)
	{
		TraitTypes eTrait = (TraitTypes) iTrait;
		if (eTrait != NO_TRAIT)
		{
			if (hasTrait(eTrait))
			{
				iCapacity *= 100 + GC.getTraitInfo(eTrait).getStorageCapacityModifier();
				iCapacity /= 100;
			}
		}
	}			

	return iCapacity;
}

bool CvCity::isAutoRaze() const
{
	if (getPopulation() == 0)
	{
		return true;
	}

	if (!GC.getGameINLINE().isOption(GAMEOPTION_NO_CITY_RAZING))
	{
		if (getHighestPopulation() == 1)
		{
			return true;
		}

		if (GC.getGameINLINE().getMaxCityElimination() > 0)
		{
			return true;
		}
	}

	if (GC.getGameINLINE().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
	{
		return true;
	}

	if (getPreviousOwner() != NO_PLAYER)
	{
		if (GET_PLAYER(getPreviousOwner()).isNative() != isNative())
		{
			return true;
		}
	}

	return false;
}

void CvCity::setScoutVisited(TeamTypes eTeam, bool bVisited)
{
	FAssert(eTeam >= 0);
	FAssert(eTeam < MAX_TEAMS);

	if(bVisited != isScoutVisited(eTeam))
	{
		m_abScoutVisited[eTeam] = bVisited;
		setBillboardDirty(true);
	}
}


bool CvCity::isScoutVisited(TeamTypes eTeam) const
{
	FAssert(eTeam >= 0 && eTeam < MAX_TEAMS);
	if (eTeam == getTeam())
	{
		return true;
	}
	return m_abScoutVisited[eTeam];
}


GoodyTypes CvCity::getGoodyType(const CvUnit* pUnit) const
{
	if (!isNative())
	{
		return NO_GOODY;
	}

	for (int iTeam = 0; iTeam < MAX_TEAMS; ++iTeam)
	{
		if (iTeam != getTeam())
		{
			if (isScoutVisited((TeamTypes) iTeam))
			{
				return NO_GOODY;
			}
		}
	}

	int iBestValue = 0;
	GoodyTypes eBestGoody = NO_GOODY;
	for (int i = 0; i < GC.getHandicapInfo(getHandicapType()).getNumGoodies(); i++)
	{
		GoodyTypes eGoody = (GoodyTypes) GC.getHandicapInfo(getHandicapType()).getGoodies(i);

		FAssert(eGoody >= 0);
		FAssert(eGoody < GC.getNumGoodyInfos());

		if (pUnit == NULL || GET_PLAYER(pUnit->getOwnerINLINE()).canReceiveGoody(plot(), eGoody, pUnit))
		{
			int iRandValue = GC.getGameINLINE().getSorenRandNum(GC.getGoodyInfo(eGoody).getCityGoodyWeight(), "City Goodies");
			if(iRandValue > iBestValue)
			{
				iBestValue = iRandValue;
				eBestGoody = eGoody;
			}
		}
	}

	return eBestGoody;
}

int CvCity::doGoody(CvUnit* pUnit, GoodyTypes eGoody)
{
	int iValue = -1;
	FAssert(pUnit != NULL);

	CvPlayer& kPlayer = GET_PLAYER(pUnit->getOwnerINLINE());
	if (eGoody != NO_GOODY)
	{
		iValue = kPlayer.receiveGoody(plot(), eGoody, pUnit);
	}

	setScoutVisited(kPlayer.getTeam(), true);

	for (int i = 0; i < GC.getNumFatherPointInfos(); ++i)
	{
		FatherPointTypes ePointType = (FatherPointTypes) i;
		int gameSpeedMod =  GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
		int scoutPoints = GC.getFatherPointInfo(ePointType).getScoutVillagePoints();
		scoutPoints =  scoutPoints * gameSpeedMod/100;
		GET_PLAYER(pUnit->getOwnerINLINE()).changeFatherPoints(ePointType, scoutPoints);
	}

	return iValue;
}

PlayerTypes CvCity::getMissionaryPlayer() const
{
	return m_eMissionaryPlayer;
}

// WTP, ray, Native Trade Posts - START
PlayerTypes CvCity::getTradePostPlayer() const
{
	return m_eTradePostPlayer;
}
// WTP, ray, Native Trade Posts - END

CivilizationTypes CvCity::getMissionaryCivilization() const
{
	if (getMissionaryPlayer() == NO_PLAYER)
	{
		return NO_CIVILIZATION;
	}

	return GET_PLAYER(getMissionaryPlayer()).getCivilizationType();
}

// WTP, ray, Native Trade Posts - START
CivilizationTypes CvCity::getTradePostCivilization() const
{
	if (getTradePostPlayer() == NO_PLAYER)
	{
		return NO_CIVILIZATION;
	}

	return GET_PLAYER(getTradePostPlayer()).getCivilizationType();
}
// WTP, ray, Native Trade Posts - END

void CvCity::setMissionaryPlayer(PlayerTypes ePlayer)
{
	if (ePlayer != getMissionaryPlayer())
	{
		PlayerTypes eOldPlayer = getMissionaryPlayer();

		m_eMissionaryPlayer = ePlayer;

		if (eOldPlayer != NO_PLAYER)
		{
			CvWString szBuffer = gDLL->getText("TXT_KEY_MISSION_REMOVED", getNameKey(), GET_PLAYER(eOldPlayer).getCivilizationAdjectiveKey());

			for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
			{
				CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes) iPlayer);
				if (kLoopPlayer.isAlive())
				{
					if (isRevealed(kLoopPlayer.getTeam(), false))
					{
						gDLL->getInterfaceIFace()->addMessage(kLoopPlayer.getID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), getX_INLINE(), getY_INLINE(), false, false);
					}
				}
			}
		}

		if (getMissionaryPlayer() != NO_CIVILIZATION)
		{
			CvWString szBuffer = gDLL->getText("TXT_KEY_MISSION_ESTABLISHED", getNameKey(), GET_PLAYER(ePlayer).getCivilizationAdjectiveKey());

			for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
			{
				CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes) iPlayer);
				if (kLoopPlayer.isAlive())
				{
					if (isRevealed(kLoopPlayer.getTeam(), false))
					{
						gDLL->getInterfaceIFace()->addMessage(kLoopPlayer.getID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_ESTABLISH_MISSION).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
					}
				}
			}
		}

		setBillboardDirty(true);
	}
}

int CvCity::getMissionaryRate() const
{
	return m_iMissionaryRate;
}

void CvCity::setMissionaryRate(int iRate)
{
	m_iMissionaryRate = iRate;
}

// WTP, ray, Native Trade Posts - START
void CvCity::setTradePostPlayer(PlayerTypes ePlayer)
{
	if (ePlayer != getTradePostPlayer())
	{
		PlayerTypes eOldPlayer = getTradePostPlayer();

		m_eTradePostPlayer = ePlayer;

		if (eOldPlayer != NO_PLAYER)
		{
			CvWString szBuffer = gDLL->getText("TXT_KEY_TRADE_POST_REMOVED", getNameKey(), GET_PLAYER(eOldPlayer).getCivilizationAdjectiveKey());

			for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
			{
				CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes) iPlayer);
				if (kLoopPlayer.isAlive())
				{
					if (isRevealed(kLoopPlayer.getTeam(), false))
					{
						gDLL->getInterfaceIFace()->addMessage(kLoopPlayer.getID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), getX_INLINE(), getY_INLINE(), false, false);
					}
				}
			}
		}

		if (getTradePostPlayer() != NO_CIVILIZATION)
		{
			CvWString szBuffer = gDLL->getText("TXT_KEY_TRADE_POST_ESTABLISHED", getNameKey(), GET_PLAYER(ePlayer).getCivilizationAdjectiveKey());

			for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
			{
				CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes) iPlayer);
				if (kLoopPlayer.isAlive())
				{
					if (isRevealed(kLoopPlayer.getTeam(), false))
					{
						gDLL->getInterfaceIFace()->addMessage(kLoopPlayer.getID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_ESTABLISH_TRADE_POST).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
					}
				}
			}
		}

		setBillboardDirty(true);
	}
}

int CvCity::getNativeTradeRate() const
{
	return m_iNativeTradeRate;
}

void CvCity::setNativeTradeRate(int iRate)
{
	m_iNativeTradeRate = iRate;
}
// WTP, ray, Native Trade Posts - END

// R&R, ray , Stirring Up Natives - START
bool CvCity::getHasBeenStirredUp() const
{
	return m_bStirredUp;
}

void CvCity::setHasBeenStirredUp(bool stirredUp)
{
	m_bStirredUp = stirredUp;
}
// R&R, ray , Stirring Up Natives - END

void CvCity::doRebelSentiment()
{
	int iTurnFactor = std::max(1, GC.getDefineINT("REBEL_SENTIMENT_TURN_WEIGHT") * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent() / 100);

	int iPast = (iTurnFactor - 1) * getRebelSentiment();
	int iNew = 0;

	if (!GET_PLAYER(getOwnerINLINE()).isEurope())
	{
		iNew = calculateNetYield(YIELD_BELLS) * GC.getDefineINT("REBEL_SENTIMENT_BELLS_FACTOR");
	}

	if (!isHuman())
	{
		iNew *= 100 + GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIRebelModifier();
		iNew /= 100;
	}

	setRebelSentiment((iNew + iPast) / iTurnFactor);
}

int CvCity::getRebelPercent() const
{
	return std::max(0, std::min(100, getRebelSentiment() / std::max(1, getPopulation())));
}

int CvCity::getRebelSentiment() const
{
	return m_iRebelSentiment;
}

// R&R, ray, Health - START
int CvCity::getCityHealth() const
{
	return m_iCityHealth;
}


int CvCity::getCityHealthChangeFromPopulation() const
{
	int iNegHealthFromPopulation = 0;
	int iCityPopulation = getPopulation();
	int iMinCityPopForNegHealth = GC.getMIN_POP_NEG_HEALTH();
	
	// Negative Influence from Population only for human player
	if (isHuman() && iCityPopulation >= iMinCityPopForNegHealth)
	{
		int iDivisor = GC.getPOP_DIVISOR_NEG_HEALTH();
		int iSmoothener = (iMinCityPopForNegHealth / iDivisor) - 1;

		iNegHealthFromPopulation = (iCityPopulation / iDivisor) - iSmoothener;
	}

	return iNegHealthFromPopulation;
}

int CvCity::getCityHealthChange() const
{
	int iNegHealthFromPopulation = 0;
	int iPosHealthFromHealers = 0;

	// Negative Influence from Population
	iNegHealthFromPopulation = getCityHealthChangeFromPopulation();

	// Positive Influence from Healers
	iPosHealthFromHealers = calculateNetYield(YIELD_HEALTH);

	// balances existing city health back to 0 if no positive or negative health created
	if (iPosHealthFromHealers == 0 && iNegHealthFromPopulation == 0)
	{
		int iCurrentCityHealth = getCityHealth();
		if (iCurrentCityHealth > 0)
		{
			return -1;
		}
		else if (iCurrentCityHealth < 0)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	
	return (iPosHealthFromHealers - iNegHealthFromPopulation);
	
}

void CvCity::setCityHealth(int iValue)
{
	int iMax = GC.getMAX_CITY_HEALTH();
	int iLowest = GC.getLOWEST_CITY_HEALTH();

	// just for safety because of Events, do not create negative Health for AI
	if (!isHuman() && iValue < 0)
	{
		return;
	}

	if (iValue < iLowest)
	{
		m_iCityHealth = iLowest;
	}
	else if (iValue > iMax)
	{
		m_iCityHealth = iMax;
	}
	else
	{
		m_iCityHealth = iValue;
	}
}

void CvCity::changeCityHealth(int iValue)
{
	setCityHealth(m_iCityHealth + iValue);
}

void CvCity::doCityHealth()
{	
	changeCityHealth(getCityHealthChange());
}
// R&R, ray, Health - END


// WTP, ray, Happiness - START

void CvCity::doCityHappiness()
{	
	// we do not do this for every tiny village
	int iMinPopulation = GC.getMIN_POP_NEG_HAPPINESS() * 2;
	if (getPopulation() < iMinPopulation)
	{
		return;
	}

	// we check the timer to prevent firing this too often
	int iCurrentTimer = getCityTimerFestivitiesOrUnrest();
	if (iCurrentTimer > 0)
	{
		return;
	}

	// min Happiness
	int iHappinessBalance = getCityHappiness() - getCityUnHappiness();
	int minHappinessForFestivities = GC.getMIN_BALANCE_FESTIVITIES_HAPPINESS();

	// we check if we have the min value of Happiness
	if (iHappinessBalance < minHappinessForFestivities)
	{
		return;
	}

	// get random values
	int chanceForFestivities = GC.getBASE_CHANCE_FESTIVITIES_HAPPINESS();
	chanceForFestivities = chanceForFestivities * iHappinessBalance; // the chances are scaled by Happiness balance
	int randomValue = GC.getGameINLINE().getSorenRandNum(1000, "Festivities Happiness Triggered");

	// random has failed
	if (chanceForFestivities < randomValue)
	{
		return;
	}

	// first we reset the timer - because we are going to fire
	int gamespeedMod = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
	int iTimerResetValue = GC.getTIMER_FESTIVITIES_OR_UNRESTS()*gamespeedMod/100;
	setCityTimerFestivitiesOrUnrest(iTimerResetValue);

	// otherwise we do festivities - which give one of the father point categories by random; 
	int iMaxFoundingFatherPoints = GC.getFOUNDING_FAHTER_POINTS_FESTIVITIES_HAPPINESS();
	int iFoundingFatherPoints = GC.getGameINLINE().getSorenRandNum(iMaxFoundingFatherPoints, "Festivities Happiness Father Points");

	int iNumFatherPointInfos = GC.getNumFatherPointInfos();
	// we skip fathr points for Exploration - which is 0 that is why we first subract 2 and then add 1
	int randomFatherCategorySelection = GC.getGameINLINE().getSorenRandNum(iNumFatherPointInfos - 2, "Random Father Point selection");
	randomFatherCategorySelection = randomFatherCategorySelection + 1;
	FatherPointTypes ePointType = (FatherPointTypes) randomFatherCategorySelection;

	GET_PLAYER(getOwnerINLINE()).changeFatherPoints(ePointType, iFoundingFatherPoints);

	CvWString szBuffer;
	szBuffer = gDLL->getText("TXT_KEY_FESTIVITIES_BECAUSE_HAPPINESS", getNameKey(), iFoundingFatherPoints, GC.getFatherPointInfo(ePointType).getDescription());
	gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CULTUREEXPANDS", MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(YIELD_CULTURE).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE(), true, true);

	return;
}

void CvCity::doCityUnHappiness()
{	
	// we do not do this for AI
	if(!isHuman())
	{
		return;
	}

	// we do not do this for every tiny village
	int iMinPopulation = GC.getMIN_POP_NEG_HAPPINESS() * 2;
	if (getPopulation() < iMinPopulation)
	{
		return;
	}

	// we do not trigger again if already in Occupation
	if(getOccupationTimer() > 0)
	{
		return;
	}

	// we check the timer to prevent firing this too often
	int iCurrentTimer = getCityTimerFestivitiesOrUnrest();
	if (iCurrentTimer > 0)
	{
		return;
	}

	// min Happiness
	int iUnHappinessBalance = getCityUnHappiness() - getCityHappiness();
	int minUnHappinessForUnrest = GC.getMIN_BALANCE_UNREST_UNHAPPINESS();

	// we check if we have the min value of Happiness
	if (iUnHappinessBalance < minUnHappinessForUnrest)
	{
		return;
	}

	// get random values
	int chanceForUnrest = GC.getBASE_CHANCE_UNREST_UNHAPPINESS();
	chanceForUnrest = chanceForUnrest * iUnHappinessBalance; // the chances are scaled by UnHappiness balance
	int randomValue = GC.getGameINLINE().getSorenRandNum(1000, "Unrest UnHappiness");

	// random has failed
	if (chanceForUnrest < randomValue)
	{
		return;
	}

	// first we reset the timer - because we are going to fire
	int gamespeedMod = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
	int iTimerResetValue = GC.getTIMER_FESTIVITIES_OR_UNRESTS()*gamespeedMod/100;
	setCityTimerFestivitiesOrUnrest(iTimerResetValue);

	// now we do unrest 
	int iUnrestTime = GC.getTURNS_UNREST_UNHAPPINESS();
	setOccupationTimer(iUnrestTime*gamespeedMod/100);

	// add message
	CvWString szBuffer = gDLL->getText("TXT_KEY_CITY_UNREST_BECAUSE_UNHAPPINESS", getNameKey());
	gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CITYCAPTURED", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);

	return;
}

// basic set and get methods
int CvCity::getCityHappiness() const
{
	return m_iCityHappiness;
}

void CvCity::setCityHappiness(int iValue)
{
	if (iValue < 0)
	{
		return;
	}

	m_iCityHappiness = iValue;
}

void CvCity::updateCityHappiness()
{
	int iTotalCityHappiness = 0;

	// WTP, ray, trying to fix Rebel Rate Modifier on Happiness for Balancing - START
	// iTotalCityHappiness += calculateNetYield(YIELD_HAPPINESS);
	iTotalCityHappiness += getBaseRawYieldProduced(YIELD_HAPPINESS);
	// WTP, ray, trying to fix Rebel Rate Modifier on Happiness for Balancing - END
	iTotalCityHappiness += getHappinessFromCrosses();
	iTotalCityHappiness += getHappinessFromBells();
	iTotalCityHappiness += getHappinessFromHealth();
	iTotalCityHappiness += getHappinessFromCulture();
	iTotalCityHappiness += getHappinessFromEducation();
	iTotalCityHappiness += getHappinessFromDomesticDemandsFulfilled();
	iTotalCityHappiness += getHappinessFromTreaties();

	setCityHappiness(iTotalCityHappiness);
}

int CvCity::getCityUnHappiness() const
{
	return m_iCityUnHappiness;
}

void CvCity::setCityUnHappiness(int iValue)
{
	if (iValue < 0)
	{
		return;
	}

	m_iCityUnHappiness = iValue;
}

void CvCity::updateCityUnHappiness()
{
	int iTotalCityUnHappiness = 0;

	// WTP, ray, trying to fix Rebel Rate Modifier on Happiness for Balancing - START
	// iTotalCityUnHappiness += calculateNetYield(YIELD_UNHAPPINESS); // should not exist but in case it ever does
	iTotalCityUnHappiness += getBaseRawYieldProduced(YIELD_UNHAPPINESS); // should not exist but in case it ever does
	// WTP, ray, trying to fix Rebel Rate Modifier on Happiness for Balancing - END
	iTotalCityUnHappiness += getUnhappinessFromPopulation();
	iTotalCityUnHappiness += getUnhappinessFromSlavery(); 
	iTotalCityUnHappiness += getUnhappinessFromWars();
	iTotalCityUnHappiness += getUnhappinessFromMissingDefense();
	iTotalCityUnHappiness += getUnhappinessFromTaxRate();

	setCityUnHappiness(iTotalCityUnHappiness);
}

// specific computation methods for factors of Unhappiness
int CvCity::getUnhappinessFromPopulation() const
{
	int iUnHapPop = 0;
	int iPopulation = getPopulation();
	int iMinPopForHappiness = GC.getMIN_POP_NEG_HAPPINESS();

	// small help for AI
	if(!isHuman())
	{
		iMinPopForHappiness = iMinPopForHappiness*2;
	}

	iUnHapPop = iPopulation - iMinPopForHappiness;

	// to prevent negative Unhappiness in case we have less pop than min value for neg pop
	if (iUnHapPop > 0)
	{
		return iUnHapPop;
	}

	return 0;
}

int CvCity::getUnhappinessFromSlavery() const
{
	int iUnHapSlav = 0;
	for (int i = 0; i < getPopulation(); ++i)
	{
		CvUnit* pLoopUnit = getPopulationUnitByIndex(i);
		if (GC.getUnitInfo(pLoopUnit->getUnitType()).LbD_canRevolt())
		{
			iUnHapSlav++;
		}
	}

	// adjustment for Traits that reduce Unhappiness from Slaves
	// we only need to check this if we have slaves at all
	if (iUnHapSlav > 0)
	{
		int iTraitUnhappinessSlavesModifier = 0;
		for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); ++iTrait)
		{
			if (hasTrait((TraitTypes) iTrait))
			{
				CvTraitInfo& kTrait = GC.getTraitInfo((TraitTypes) iTrait);
				iTraitUnhappinessSlavesModifier += kTrait.getUnhappinessFromSlavesModifier();
			}
		}
		iUnHapSlav = iUnHapSlav * (100 - iTraitUnhappinessSlavesModifier) / 100; // the Modifier in XML is expected to be positive
	}
	return iUnHapSlav;
}

int CvCity::getUnhappinessFromWars() const
{
	int iUnHapWar = 0;
	int iFactorPerWar = GC.getPER_EUROPEAN_AT_WAR_UNHAPPINESS(); 
	int iNumEuropeanWars = 0;

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			// we do not count ourselves of course
			if (iI != GET_TEAM(getTeam()).getID())
			{
				// we count teams we are at war with that have European Players - thus we do not count Native-only Teams, Wild Animals, Kings, ...
				if (GET_TEAM(getTeam()).isAtWar((TeamTypes)iI) && GET_TEAM((TeamTypes)iI).hasColonialPlayer())
				{
					iNumEuropeanWars++;
				}
			}
		}
	}

	iUnHapWar = iFactorPerWar * iNumEuropeanWars;
	return iUnHapWar;
}

int CvCity::getUnhappinessFromMissingDefense() const
{
	int iUnHapMissingDef = 0;

	int iPopDefenseDivisor = GC.getPOP_DIVISOR_DEFENSE_UNHAPPINESS(); 
	// since this is a divisor it may never be 0 - to prevent CTD by messing up XML config
	if (iPopDefenseDivisor == 0)
	{
		iPopDefenseDivisor = 1;
	}

	int iDefendersNeeded = 0;
	iDefendersNeeded = getPopulation() / iPopDefenseDivisor;
	int iDefendersFound = plot()->getNumDefenders(getOwnerINLINE());
	iUnHapMissingDef = iDefendersNeeded - iDefendersFound;

	// we only have a Unappiness for missing defenders - theoreticyally it is possible that we have more
	if (iUnHapMissingDef > 0)
	{
		return iUnHapMissingDef;
	}
	
	// more defenders than needed does not help
	return 0;
}

int CvCity::getUnhappinessFromTaxRate() const
{
	int iUnHapTax = 0;

	int iTaxDivisor = GC.getTAX_DIVISOR_UNHAPPINESS(); 
	// since this is a divisor it may never be 0 - to prevent CTD by messing up XML config
	if (iTaxDivisor == 0)
	{
		iTaxDivisor = 1;
	}

	int iCurrentTaxRate = GET_PLAYER(getOwnerINLINE()).getTaxRate();
	iUnHapTax = iCurrentTaxRate / iTaxDivisor;

	// to ensure that small cities are not hit too much by Unhappiness from Taxes
	if (iUnHapTax > getPopulation())
	{
		iUnHapTax = getPopulation();
	}

	return iUnHapTax;
}

// specific computation methods for factors of Happiness
int CvCity::getHappinessFromCrosses() const
{
	int iHapCrosses = 0;
	int iCrosses = calculateNetYield(YIELD_CROSSES);
	int iPopulation = getPopulation();
	int iPopDivisor = GC.getPOP_DIVISOR_HAPPINESS();

	// to prevent division by 0
	if (iPopulation == 0)
	{
		iPopulation = 1;
	}
	if (iPopDivisor == 0)
	{
		iPopDivisor = 1;
	}
	if ((iPopulation / iPopDivisor) == 0)
	{
		iPopulation = 1;
		iPopDivisor = 1;
	}

	iHapCrosses = iCrosses / (iPopulation / iPopDivisor);

	if (iHapCrosses > (iPopulation / iPopDivisor))
	{
		iHapCrosses = (iPopulation / iPopDivisor);
	}
	// if we produce a little, we give at least 1 Happiness
	if (iHapCrosses == 0 && iCrosses > 0)
	{
		iHapCrosses = 1;
	}

	return iHapCrosses;
}

int CvCity::getHappinessFromBells() const
{
	int iHapBells = 0;
	int iBells = calculateNetYield(YIELD_BELLS);
	int iPopulation = getPopulation();
	int iPopDivisor = GC.getPOP_DIVISOR_HAPPINESS();

	// to prevent division by 0
	if (iPopulation == 0)
	{
		iPopulation = 1;
	}
	if (iPopDivisor == 0)
	{
		iPopDivisor = 1;
	}
	if ((iPopulation / iPopDivisor) == 0)
	{
		iPopulation = 1;
		iPopDivisor = 1;
	}

	iHapBells = iBells / (iPopulation / iPopDivisor);

	if (iHapBells > (iPopulation / iPopDivisor))
	{
		iHapBells = (iPopulation / iPopDivisor);
	}
	// if we produce a little, we give at least 1 Happiness
	if (iHapBells == 0 && iBells > 0)
	{
		iHapBells = 1;
	}
	return iHapBells;
}

int CvCity::getHappinessFromHealth() const
{
	int iHapHealth = 0;
	int iHealth = calculateNetYield(YIELD_HEALTH);
	int iPopulation = getPopulation();
	int iPopDivisor = GC.getPOP_DIVISOR_HAPPINESS();

	// to prevent division by 0
	if (iPopulation == 0)
	{
		iPopulation = 1;
	}
	if (iPopDivisor == 0)
	{
		iPopDivisor = 1;
	}
	if ((iPopulation / iPopDivisor) == 0)
	{
		iPopulation = 1;
		iPopDivisor = 1;
	}

	iHapHealth = iHealth / (iPopulation / iPopDivisor);
	
	if (iHapHealth > (iPopulation / iPopDivisor))
	{
		iHapHealth = (iPopulation / iPopDivisor);
	}
	// if we produce a little, we give at least 1 Happiness
	if (iHapHealth == 0 && iHealth > 0)
	{
		iHapHealth = 1;
	}
	return iHapHealth;
}

int CvCity::getHappinessFromCulture() const
{
	int iHapCulture = 0;
	int iCulture = calculateNetYield(YIELD_CULTURE);
	int iPopulation = getPopulation();
	int iPopDivisor = GC.getPOP_DIVISOR_HAPPINESS();

	// to prevent division by 0
	if (iPopulation == 0)
	{
		iPopulation = 1;
	}
	if (iPopDivisor == 0)
	{
		iPopDivisor = 1;
	}
	if ((iPopulation / iPopDivisor) == 0)
	{
		iPopulation = 1;
		iPopDivisor = 1;
	}

	iHapCulture = iCulture / ((iPopulation / iPopDivisor));

	if (iHapCulture > (iPopulation / iPopDivisor))
	{
		iHapCulture = (iPopulation / iPopDivisor);
	}
	// if we produce a little, we give at least 1 Happiness
	if (iHapCulture == 0 && iCulture > 0)
	{
		iCulture = 1;
	}
	return iHapCulture;
}

int CvCity::getHappinessFromEducation() const
{
	int iHapEducation = 0;
	int iEducation = calculateNetYield(YIELD_EDUCATION);
	int iPopulation = getPopulation();
	int iPopDivisor = GC.getPOP_DIVISOR_HAPPINESS();

	// to prevent division by 0
	if (iPopulation == 0)
	{
		iPopulation = 1;
	}
	if (iPopDivisor == 0)
	{
		iPopDivisor = 1;
	}
	if ((iPopulation / iPopDivisor) == 0)
	{
		iPopulation = 1;
		iPopDivisor = 1;
	}

	iHapEducation = iEducation / (iPopulation / iPopDivisor);

	if (iHapEducation > (iPopulation / iPopDivisor))
	{
		iHapEducation = (iPopulation / iPopDivisor);
	}
	// if we produce a little, we give at least 1 Happiness
	if (iHapEducation == 0 && iEducation > 0)
	{
		iHapEducation = 1;
	}
	return iHapEducation;
}

int CvCity::getHappinessFromDomesticDemandsFulfilled() const
{
	int iHapDomesticDemand = 0;

	// loop the Yields and find demand fulfilled
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		YieldTypes eYield = (YieldTypes) iYield;
		if (GC.getYieldInfo(eYield).isCargo())
		{
			if (getYieldDemand(eYield) > 0 && (getYieldStored(eYield) > 0))
			{
				iHapDomesticDemand++;
			}	
		}
	}

	return iHapDomesticDemand;
}

int CvCity::getHappinessFromTreaties() const
{
	// calcuation logic - calculation umber of Open Borders and Defensive Pacts
	int iNumOpenBorders = 0;
	int iNumDefensivePacts = 0;

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			// we do not count ourselves of course
			if (iI != GET_TEAM(getTeam()).getID())
			{
				// we count teams we have opne borders with that have European Players - thus we do not count Native-only Teams, Wild Animals, Kings, ...
				if (GET_TEAM(getTeam()).isOpenBorders((TeamTypes)iI) && GET_TEAM((TeamTypes)iI).hasColonialPlayer())
				{
					iNumOpenBorders++;
				}

				// we count teams we a defensive pact with that have European Players - thus we do not count Native-only Teams, Wild Animals, Kings, ...
				if (GET_TEAM(getTeam()).isDefensivePact((TeamTypes)iI) && GET_TEAM((TeamTypes)iI).hasColonialPlayer())
				{
					iNumDefensivePacts++;
				}
			}
		}
	}

	int iHapTreaties = iNumOpenBorders + iNumDefensivePacts;

	// we give only a max of 5 here - to be considered and checkecked in balanancing
	/*if (iHapTreaties > 5)
	{
		return 5;
	}*/

	return iHapTreaties;
}

int CvCity::getCityTimerFestivitiesOrUnrest() const
{
	return m_iCityTimerFestivitiesOrUnrest;
}

void CvCity::setCityTimerFestivitiesOrUnrest(int iValue)
{
	if (iValue < 0)
	{
		return;
	}

	m_iCityTimerFestivitiesOrUnrest = iValue;
}

void CvCity::changeCityTimerFestivitiesOrUnrest(int iValue) 
{
	if (getCityTimerFestivitiesOrUnrest() + iValue < 0)
	{
		m_iCityTimerFestivitiesOrUnrest = 0;
		return;
	}

	m_iCityTimerFestivitiesOrUnrest = m_iCityTimerFestivitiesOrUnrest + iValue;
}

// WTP, ray, Happiness - END

void CvCity::setTeachUnitMultiplier(int iModifier)
{
	m_iTeachUnitMultiplier = iModifier;
}

int CvCity::getTeachUnitMultiplier() const
{
	return m_iTeachUnitMultiplier;
}

void CvCity::setEducationThresholdMultiplier(int iModifier)
{
	m_iEducationThresholdMultiplier = iModifier;
}

int CvCity::getEducationThresholdMultiplier() const
{
	return m_iEducationThresholdMultiplier;
}

int CvCity::educationThreshold() const
{
	int iThreshold = ((GC.getEDUCATION_THRESHOLD() * std::max(0, (getEducationThresholdMultiplier()))) / 100);

	iThreshold *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
	iThreshold /= 100;

	iThreshold *= GC.getEraInfo(GC.getGameINLINE().getStartEra()).getGrowthPercent();
	iThreshold /= 100;

	return std::max(1, iThreshold);
}



void CvCity::setRebelSentiment(int iValue)
{
	m_iRebelSentiment = iValue;
	FAssert(getRebelSentiment() >= 0);	
}

UnitClassTypes CvCity::bestTeachUnitClass()
{
	PROFILE_FUNC();
	int iBestValue = 0;
	UnitClassTypes eBestUnitClass = NO_UNITCLASS;
	
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	
	std::vector<int> values(GC.getNumUnitClassInfos(), 0);
	for (int i = 0; i < GC.getNumUnitClassInfos(); ++i)
	{
		if (GC.getCivilizationInfo(kOwner.getCivilizationType()).getTeachUnitClassWeight(i) > 0)
		{
			UnitTypes eLoopUnit = (UnitTypes)GC.getUnitClassInfo((UnitClassTypes) i).getDefaultUnitIndex();
			if (eLoopUnit != NO_UNIT)
			{
				int iValue = 0;	
				ProfessionTypes eIdealProfession = kOwner.AI_idealProfessionForUnit(eLoopUnit);
		
				if (eIdealProfession == NO_PROFESSION)
				{
					iValue += 100;
				}
				else
				{
					CvProfessionInfo& kIdealProfession = GC.getProfessionInfo(eIdealProfession);
					
					if (!kIdealProfession.isCitizen())
					{
						iValue += 100;
					}
					else
					{
						// R&R, ray , MYCP partially based on code of Aymerick - START
						YieldTypes eWantedYield = (YieldTypes)kIdealProfession.getYieldsProduced(0);
						if (!kIdealProfession.isWorkPlot())
						{
							eWantedYield = (YieldTypes)kIdealProfession.getYieldsConsumed(0);
						}

						// R&R, ray , MYCP partially based on code of Aymerick - END

						if (eWantedYield == NO_YIELD)
						{
							iValue += 100;
						}
						else
						{
						
							int iPlotValue = 0;
							for (int j = 0; j < NUM_CITY_PLOTS; ++j)
							{
								CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), j);
								if (pLoopPlot != NULL)
								{
									if (pLoopPlot->isValidYieldChanges(eLoopUnit) && !(eWantedYield == YIELD_FOOD && pLoopPlot->isHills()))
									{
										int iBaseYield = pLoopPlot->calculateNatureYield(eWantedYield, getTeam(), true);
										if (iBaseYield > 0)
										{
											iPlotValue += 2;
										}
										
										if (pLoopPlot->getFeatureType() != NO_FEATURE)
										{
											int iChange = GC.getFeatureInfo(pLoopPlot->getFeatureType()).getYieldChange(eWantedYield);
											if (iChange > 0 && iBaseYield == 0)
											{
												iPlotValue ++; //R&R, ray, changes from agnat86
											}
											if (iChange < 0 && iBaseYield > 0)
											{
												iPlotValue --; //R&R, ray, changes from agnat86
											}
										}

										if (pLoopPlot->getBonusType() != NO_BONUS)
										{
											if (GC.getBonusInfo(pLoopPlot->getBonusType()).getYieldChange(eWantedYield) > 0)
											{
												iPlotValue += 8; //R&R, ray, changes from agnat86
											}
										}
									}
								}
							}
							
							iValue = 25 + 125 * iPlotValue / NUM_CITY_PLOTS;
						}
					}
				}
				
				iValue *= GC.getCivilizationInfo(kOwner.getCivilizationType()).getTeachUnitClassWeight(i);
				values[i] = iValue;
			}
		}
	}
	
	int iTotal = 0;
	int iCount = 0;
	for (int i = 0; i < GC.getNumUnitClassInfos(); ++i)
	{
		if (values[i] != 0)
		{
			iTotal += values[i];
			iCount ++;
		}
	}

	if (iCount == 0)
	{
		return NO_UNITCLASS;
	}
	
	int iLoop = 0;
	CvCity* pLoopCity;
	for (pLoopCity = kOwner.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kOwner.nextCity(&iLoop))
	{
		if (pLoopCity->getTeachUnitClass() != NO_UNITCLASS)
		{
			values[pLoopCity->getTeachUnitClass()] ++;
			values[pLoopCity->getTeachUnitClass()] /= 2;
		}
	}
	
	//This dampens the favortism towards the most abundant yields.
	int iAverage = iTotal / iCount;
	for (int i = 0; i < GC.getNumUnitClassInfos(); ++i)
	{
		int iValue = values[i];
		if (iValue > 0)
		{
			if (iValue > iAverage)
			{
				iValue = ((iValue - iAverage) / 4) + iAverage;
			}
			iValue = 1 + GC.getGameINLINE().getSorenRandNum(iValue, "Pick City Training");
			
			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestUnitClass = (UnitClassTypes)i;
			}
		}
	}
	
	return eBestUnitClass;
}

CvUnit* CvCity::ejectBestDefender(CvUnit* pCurrentBest, CvUnit* pAttacker)
{

	ProfessionTypes eProfession;
	CvUnit* pDefender = getBestDefender(&eProfession, pCurrentBest, pAttacker);

	if (pDefender != NULL)
	{
		if (pDefender != pCurrentBest)
		{
			if (!removePopulationUnit(pDefender, false, eProfession))
			{
				return pCurrentBest;
			}
		}
	}

	return pDefender;
}

CvUnit* CvCity::getBestDefender(ProfessionTypes* peProfession, CvUnit* pCurrentBest, const CvUnit* pAttacker) const
{
	//initialize profession
	if (peProfession != NULL)
	{
		*peProfession = NO_PROFESSION;
		if (pCurrentBest != NULL)
		{
			*peProfession = pCurrentBest->getProfession();
		}
	}

	if (pAttacker != NULL && !pAttacker->isEnemy(getTeam(), plot()))
	{
		return pCurrentBest;
	}

	if (pCurrentBest != NULL && !pCurrentBest->isUnarmed() && pCurrentBest->isCityDefender())
	{
		return pCurrentBest;
	}

	CvUnit* pBestUnit = pCurrentBest;
	ProfessionTypes eBestProfession = NO_PROFESSION;
	if(pBestUnit != NULL)
	{
		eBestProfession = pBestUnit->getProfession();
	}

	for (uint i = 0; i < m_aPopulationUnits.size(); ++i)
	{
		CvUnit* pLoopUnit = m_aPopulationUnits[i];
		ProfessionTypes eBestCitizenProfession = NO_PROFESSION;
		int iBestValue = 0;
		for (int iProfession = 0; iProfession < GC.getNumProfessionInfos(); ++iProfession)
		{
			ProfessionTypes eProfession = (ProfessionTypes) iProfession;
			if (GC.getProfessionInfo(eProfession).isCityDefender())
			{
				int iValue = GET_PLAYER(getOwnerINLINE()).AI_professionValue(eProfession, UNITAI_DEFENSIVE);
				if (iValue > iBestValue)
				{
					if (pLoopUnit->canHaveProfession(eProfession, false))
					{
						iBestValue = iValue;
						eBestCitizenProfession = eProfession;
					}
				}
			}
		}

		if (eBestCitizenProfession != NO_PROFESSION)
		{
			CvUnitTemporaryStrengthModifier modifier1(pBestUnit, eBestProfession);
			CvUnitTemporaryStrengthModifier modifier2(pLoopUnit, eBestCitizenProfession);
			if (pLoopUnit->isBetterDefenderThan(pBestUnit, pAttacker, false))
			{
				pBestUnit = pLoopUnit;
				eBestProfession = eBestCitizenProfession;
			}
		}
	}

	if (peProfession != NULL)
	{
		*peProfession = eBestProfession;
	}

	return pBestUnit;
}

bool CvCity::canTradeAway(PlayerTypes eToPlayer) const
{
	if (getLiberationPlayer(false) == eToPlayer)
	{
		return true;
	}

	if (!GET_PLAYER(eToPlayer).canReceiveTradeCity(getOwnerINLINE()))
	{
		return false;
	}

	if (0 != GC.getGameINLINE().getMaxCityElimination())
	{
		return false;
	}

	if (isCapital())
	{
		return false;
	}

	if (isNative() && !GET_PLAYER(eToPlayer).isNative())
	{
		if (plot()->getCulture(eToPlayer) < plot()->getCulture(getOwnerINLINE()))
		{
			return false;
		}
	}

	return true;
}

bool CvCity::educateStudent(int iUnitId, UnitTypes eUnit)
{	

	CvUnit* pUnit = getPopulationUnitById(iUnitId);
	if (pUnit == NULL)
	{
		return false;
	}

	CvPlayer& kPlayer = GET_PLAYER(getOwnerINLINE());
	int iPrice = getSpecialistTuition(eUnit);
	if (iPrice < 0)
	{
		return false;
	}

	pUnit->setYieldStored(0);
	// perform the conversion
	if (!removePopulationUnit(pUnit, false, (ProfessionTypes) GC.getCivilizationInfo(GET_PLAYER(getOwnerINLINE()).getCivilizationType()).getDefaultProfession()))
	{
		return false;
	}

	CvUnit* pLearnUnit = GET_PLAYER(getOwnerINLINE()).initUnit(eUnit, NO_PROFESSION, pUnit->getX_INLINE(), pUnit->getY_INLINE(), pUnit->AI_getUnitAIType());
	FAssert(pLearnUnit != NULL);
	pLearnUnit->convert(pUnit, true);

	// TAC - EDUCATION COST SYSTEM - koma13, KingMB, Netbandit - 23.11.09 - START
	/*
	// Original Code
	//
	// reduce accumulated weights, but round up
	for (int i = 0; i < GC.getNumUnitInfos(); ++i)
	{
		m_aiSpecialistWeights[i] = (m_aiSpecialistWeights[i] + 1) / 2;
	}
	*/
	// TAC - EDUCATION COST SYSTEM - koma13, KingMB, Netbandit - 23.11.09 - END


	setEducationThresholdMultiplier((getEducationThresholdMultiplier() * (100 + GC.getEDUCATION_THRESHOLD_INCREASE())) / 100);
	kPlayer.changeGold(-iPrice);

	BuildingTypes eSchoolBuilding = getYieldBuilding(YIELD_EDUCATION);
	FAssert(eSchoolBuilding != NO_BUILDING);
	if(eSchoolBuilding != NO_BUILDING)
	{
		CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_STUDENT_GRADUATED", GC.getUnitInfo(eUnit).getTextKeyWide(), getNameKey(), GC.getBuildingInfo(eSchoolBuilding).getTextKeyWide());
		gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CULTUREEXPANDS", MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(YIELD_EDUCATION).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), true, true);
	}

	// Teacher List - start - Nightinggale
	int OrderedStudents = getOrderedStudents(eUnit);
	if (OrderedStudents > 0)
	{
		setOrderedStudents(eUnit, OrderedStudents - 1, getOrderedStudentsRepeat(eUnit));
	}
	// Teacher List - end - Nightinggale

	return true;
}

/** NBMOD EDU **/

/*********************************************************************/
/**                                                                 **/
/** int CvCity::NBMOD_GetCityTeachLevel() const                     **/
/** Ermittelt die höchste Bildungsstufe, welche ein Gebäude der     **/
/** Stadt anbietet.                                                 **/
/**                                                                 **/
/*********************************************************************/

void CvCity::NBMOD_SetCityTeachLevelCache() // NBMOD EDU cache - Nightinggale
{
	int iMaxTeachLevel = 0;

	// alle möglichen Gebäude durchgehen
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		// abfragen ob dieses gebäude in der Stadt vorkommt
		if (isHasBuilding((BuildingTypes)iI))
		{
			if (GC.getBuildingInfo((BuildingTypes)iI).NBMOD_GetTeachLevel() > iMaxTeachLevel)
			{
				iMaxTeachLevel = GC.getBuildingInfo((BuildingTypes)iI).NBMOD_GetTeachLevel();
			}
		}
	}
	m_NBMOD_CityTeachLevel_cache = iMaxTeachLevel; // NBMOD EDU cache - Nightinggale
}

/** NBMOD EDU **/

int CvCity::getSpecialistTuition(UnitTypes eUnit) const
{
	if (m_aiSpecialistWeights[eUnit] <= 0)
	{
		return -1;
	}

	/** NBMOD EDU **/
	// Liegt das Bildungsniveau der Stadt unter dem für diese Einheit nötigen Niveau?
	if (NBMOD_GetCityTeachLevel() <  GC.getUnitInfo(eUnit).NBMOD_GetTeachLevel())
	{
        return -1;
	}
	/** NBMOD EDU **/
	
	int* pMaxElement = std::max_element(m_aiSpecialistWeights, m_aiSpecialistWeights + GC.getNumUnitInfos());
	int iBestWeight = *pMaxElement;
	if (iBestWeight <= 0)
	{
		return -1;
	}

	// TAC - EDUCATION COST SYSTEM - koma13, KingMB, Netbandit - 23.11.09 - START

	// Original Code
	/*
	iPrice *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
	iPrice /= 100;

	if (!isHuman())
	{
		iPrice *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAITrainPercent();
		iPrice /= 100;
	}

	iPrice *= iBestWeight - m_aiSpecialistWeights[eUnit];
	iPrice /= iBestWeight;
	*/

	int iPrice = GC.getDefineINT("EDUCATION_BASE_TUITION");

	if (GC.getUSE_TAC_EDUCATION_COST_SYSTEM() == 0)
	{
		iPrice *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
		iPrice /= 100;
		
		if (!isHuman())
		{
			iPrice *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAITrainPercent();
			iPrice /= 100;
		}
	
		iPrice *= iBestWeight - m_aiSpecialistWeights[eUnit];
		iPrice /= iBestWeight;
	} 
	else
	{
		double fPrice = double(iPrice);
		double fMulti = 0.0;

		// Ausbildungsstufen-Multiplikator entsprechend benötigter Ausbildungsstufe aus der Definitionsdatei holen
		if (GC.getUnitInfo(eUnit).NBMOD_GetTeachLevel() == 1)	// 1. (niedrigste) Ausblidungsstufe (Schule)
		{
			fMulti = GC.getTAC_EDUCATION_RANK1();
		}
		else if (GC.getUnitInfo(eUnit).NBMOD_GetTeachLevel() == 2)	// 2. (mittlere) Ausbilungsstufe (Hochschule)
		{
			fMulti = GC.getTAC_EDUCATION_RANK2();
		}
		else if (GC.getUnitInfo(eUnit).NBMOD_GetTeachLevel() == 3)	// 3. (höchste) Ausbildungsstufe (Universität)
		{
			fMulti = GC.getTAC_EDUCATION_RANK3();
		}
		else
		{
			return -1;
		}

		// Ausbildungskosten mit Ausbildungsstufen-Multiplikator anpassen
		fPrice *= fMulti;

		if (fPrice > 0.0)
		{
			// Ausbildungskosten an Geschwindigkeit anpassen
			fPrice *= float(GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent());
			fPrice /= 100;

			// durch 10 Teilbar machen und korrekt runden
			iPrice = int((fPrice / 10) + 0.5) * 10;
		} else {
			iPrice = 0;
		}
	}
	
	for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); ++iTrait)
	{
		TraitTypes eTrait = (TraitTypes) iTrait;
		if (eTrait != NO_TRAIT)
		{
			if (hasTrait(eTrait))
			{
				iPrice *= std::max(0, (GC.getTraitInfo(eTrait).getSpecialistPriceModifier() + 100));
				iPrice /= 100;
			}
		}
	}						//agnat86, added price modifier for Sophisticated Trait
	// TAC - EDUCATION COST SYSTEM - koma13, KingMB, Netbandit - 23.11.09 - END

	return iPrice;
}

bool CvCity::isExport(YieldTypes eYield) const
{
	// traderoute just-in-time - start - Nightinggale
	return ma_tradeExports.get(eYield);
	// traderoute just-in-time - end - Nightinggale
}

void CvCity::addExport(YieldTypes eYield, bool bUpdateRoutes)
{
	// traderoute just-in-time - start - Nightinggale
	if (ma_tradeExports.get(eYield))
	{
		return;
	}

	ma_tradeExports.set(true, eYield);
	// traderoute just-in-time - end - Nightinggale
	if (bUpdateRoutes)
	{
		for (int iRoutePlayer = 0; iRoutePlayer < MAX_PLAYERS; ++iRoutePlayer)
		{
			CvPlayer& kRoutePlayer = GET_PLAYER((PlayerTypes) iRoutePlayer);
			if (kRoutePlayer.isAlive() && kRoutePlayer.canLoadYield(getOwnerINLINE()))
			{
				for (int iCityPlayer = 0; iCityPlayer < MAX_PLAYERS; ++iCityPlayer)
				{
					CvPlayer& kCityPlayer = GET_PLAYER((PlayerTypes) iCityPlayer);
					if (kCityPlayer.isAlive() && kRoutePlayer.canUnloadYield((PlayerTypes) iCityPlayer))
					{
						int iLoop;
						for (CvCity* pLoopCity = kCityPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kCityPlayer.nextCity(&iLoop))
						{
							if (pLoopCity->isImport(eYield))
							{
								kRoutePlayer.addTradeRoute(getIDInfo(), pLoopCity->getIDInfo(), eYield);
							}
						}
					}
				}
				
				if (kRoutePlayer.isYieldEuropeTradable(eYield))
				{
					// TAC - AI Economy - koma13 - START
					//if (isHuman())
					if (isHuman() || isBestPortCity())
					// TAC - AI Economy - koma13 - END
					{
						kRoutePlayer.addTradeRoute(getIDInfo(), IDInfo((PlayerTypes) iRoutePlayer, CvTradeRoute::EUROPE_CITY_ID), eYield);
					}
				}
			}
		}
	}

	if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
	{
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Domestic_Advisor_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(TradeRoutesAdvisor_DIRTY_BIT, true);	// TAC - Trade Routes Advisor - koma13
	}
}

void CvCity::removeExport(YieldTypes eYield, bool bUpdateRoutes)
{
	// traderoute just-in-time - start - Nightinggale
	if (!ma_tradeExports.get(eYield))
	{
		return;
	}

	ma_tradeExports.set(false, eYield);
	// traderoute just-in-time - end - Nightinggale
	if (bUpdateRoutes)
	{
		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayer);
			if (kPlayer.isAlive())
			{
				std::vector<int> aiRouteIds;
				std::vector<CvTradeRoute*> aiRoutes;
				kPlayer.getTradeRoutes(aiRoutes);
				for (uint i = 0; i < aiRoutes.size(); ++i)
				{
					CvTradeRoute* pRoute = aiRoutes[i];
					if (pRoute != NULL && pRoute->getSourceCity() == getIDInfo() && pRoute->getYield() == eYield)
					{
						aiRouteIds.push_back(pRoute->getID());
					}
				}

				for (uint i = 0; i < aiRouteIds.size(); ++i)
				{
					kPlayer.removeTradeRoute(aiRouteIds[i]);
				}
			}
		}
	}

	if (getTeam() == GC.getGameINLINE().getActiveTeam())
	{
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Domestic_Advisor_DIRTY_BIT, true);
	}
}

void CvCity::updateExport(YieldTypes eYield)
{
	int iExportCount = 0;
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayer);
		if (kPlayer.isAlive())
		{
			std::vector<CvTradeRoute*> aiRoutes;
			kPlayer.getTradeRoutes(aiRoutes);
			for (uint i = 0; i < aiRoutes.size(); ++i)
			{
				CvTradeRoute* pRoute = aiRoutes[i];
				if (pRoute != NULL && pRoute->getSourceCity() == getIDInfo() && pRoute->getYield() == eYield)
				{
					++iExportCount;
				}
			}
		}
	}

	if (iExportCount > 0)
	{
		addExport(eYield, false);
	}
	else
	{
		removeExport(eYield, false);
	}
}

void CvCity::updateExports()
{
	for (int i = 0; i < NUM_YIELD_TYPES; ++i)
	{
		updateExport((YieldTypes) i);
	}
}

bool CvCity::isImport(YieldTypes eYield) const
{
	// traderoute just-in-time - start - Nightinggale
	return ma_tradeImports.get(eYield);
	// traderoute just-in-time - end - Nightinggale
}

void CvCity::addImport(YieldTypes eYield, bool bUpdateRoutes)
{
	// traderoute just-in-time - start - Nightinggale
	if (ma_tradeImports.get(eYield))
	{
		return;
	}

	ma_tradeImports.set(true, eYield);
	// traderoute just-in-time - end - Nightinggale
	if (bUpdateRoutes)
	{
		for (int iRoutePlayer = 0; iRoutePlayer < MAX_PLAYERS; ++iRoutePlayer)
		{
			CvPlayer& kRoutePlayer = GET_PLAYER((PlayerTypes) iRoutePlayer);
			if (kRoutePlayer.isAlive() && kRoutePlayer.canUnloadYield(getOwnerINLINE()))
			{
				for (int iCityPlayer = 0; iCityPlayer < MAX_PLAYERS; ++iCityPlayer)
				{
					CvPlayer& kCityPlayer = GET_PLAYER((PlayerTypes) iCityPlayer);
					if (kCityPlayer.isAlive() && kRoutePlayer.canLoadYield((PlayerTypes) iCityPlayer))
					{
						int iLoop;
						for (CvCity* pLoopCity = kCityPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kCityPlayer.nextCity(&iLoop))
						{
							if (pLoopCity->isExport(eYield))
							{
								kRoutePlayer.addTradeRoute(pLoopCity->getIDInfo(), getIDInfo(), eYield);
							}
						}
					}
				}
			}
		}
	}

	if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
	{
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Domestic_Advisor_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(TradeRoutesAdvisor_DIRTY_BIT, true);	// TAC - Trade Routes Advisor - koma13
	}
}

void CvCity::removeImport(YieldTypes eYield, bool bUpdateRoutes)
{
	// traderoute just-in-time - start - Nightinggale
	if (!ma_tradeImports.get(eYield) || getImportsMaintain(eYield))
	{
		return;
	}

	ma_tradeImports.set(false, eYield);
	// traderoute just-in-time - end - Nightinggale
	if (bUpdateRoutes)
	{
		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayer);
			if (kPlayer.isAlive())
			{
				std::vector<int> aiRouteIds;
				std::vector<CvTradeRoute*> aiRoutes;
				kPlayer.getTradeRoutes(aiRoutes);
				for (uint i = 0; i < aiRoutes.size(); ++i)
				{
					CvTradeRoute* pRoute = aiRoutes[i];
					if (pRoute != NULL && pRoute->getDestinationCity() == getIDInfo() && pRoute->getYield() == eYield)
					{
						aiRouteIds.push_back(pRoute->getID());
					}
				}

				for (uint i = 0; i < aiRouteIds.size(); ++i)
				{
					kPlayer.removeTradeRoute(aiRouteIds[i]);
				}
			}
		}
	}

	if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
	{
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Domestic_Advisor_DIRTY_BIT, true);
	}
}

void CvCity::updateImport(YieldTypes eYield)
{
	int iImportCount = 0;
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayer);
		if (kPlayer.isAlive())
		{
			std::vector<CvTradeRoute*> aiRoutes;
			kPlayer.getTradeRoutes(aiRoutes);
			for (uint i = 0; i < aiRoutes.size(); ++i)
			{
				CvTradeRoute* pRoute = aiRoutes[i];
				if (pRoute != NULL && pRoute->getDestinationCity() == getIDInfo() && pRoute->getYield() == eYield)
				{
					++iImportCount;
				}
			}
		}
	}

	if (iImportCount > 0)
	{
		addImport(eYield, false);
	}
	else
	{
		removeImport(eYield, false);
	}
}

void CvCity::updateImports()
{
	for (int i = 0; i < NUM_YIELD_TYPES; ++i)
	{
		updateImport((YieldTypes) i);
	}
}

void CvCity::removeTradeRoutes()
{
	for (int i = 0; i < NUM_YIELD_TYPES; ++i)
	{
		YieldTypes eYield = (YieldTypes) i;

		if (isExport(eYield))
		{
			removeExport(eYield);
		}

		if (isImport(eYield))
		{
			removeImport(eYield);
		}
	}
}

void CvCity::setMaintainLevel(YieldTypes eYield, int iMaintainLevel)
{
	if (getMaintainLevel(eYield) != iMaintainLevel)
	{
		// transport feeder - begin - Nightinggale
		int iOld = getMaintainLevel(eYield);
		// transport feeder - end - Nightinggale

		// traderoute just-in-time - start - Nightinggale
		ma_tradeThreshold.set(iMaintainLevel, eYield);
		// traderoute just-in-time - end - Nightinggale

		// transport feeder - begin - Nightinggale
		if (getAutoMaintainThreshold(eYield) < iMaintainLevel)
		{
			// threshold is increased.
			// increase cache without checking normal cache check as we know at this point that a full check will result in the same as using iMaintainLevel
			ma_tradeAutoThreshold.set(iMaintainLevel, eYield);
			checkImportsMaintain(eYield);
		} else if (iOld == getAutoMaintainThreshold(eYield))
		{
			// old auto threshold was the same as the user threshold.
			// do a full cache reset check
			setAutoThresholdCache(eYield);
		}
		// transport feeder - end - Nightinggale

		if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		}
	}
}

int CvCity::getMaintainLevel(YieldTypes eYield) const
{
	// traderoute just-in-time - start - Nightinggale
	return ma_tradeThreshold.get(eYield);
	// traderoute just-in-time - end - Nightinggale
}

// R&R mod, vetiarvind, max yield import limit - Start
void CvCity::setImportsLimit(YieldTypes eYield, int iValue)
{
	if (getImportsLimit(eYield) != iValue)
	{		
		ma_tradeMaxThreshold.set(iValue, eYield);		

		if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		}
	}
}

int CvCity::getImportsLimit(YieldTypes eYield) const
{		
	return ma_tradeMaxThreshold.get(eYield);	
}

// R&R mod, vetiarvind, max yield import limit - End

// transport feeder - start - Nightinggale

void CvCity::setImportsMaintain(YieldTypes eYield, bool bSetting)
{
	ma_tradeImportsMaintain.set(bSetting, eYield);

	bool bImportStopped = isAutoImportStopped(eYield);
	if (bImportStopped)
	{
		ma_tradeStopAutoImport.set(false, eYield);
	}
	checkImportsMaintain(eYield, bImportStopped);

	if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
	{
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Domestic_Advisor_DIRTY_BIT, true);
	}
}

// bUpdateScreen will mark the screen dirty even if nothing changed.
// used by setImportsMaintain() because that one changes something
void CvCity::checkImportsMaintain(YieldTypes eYield, bool bUpdateScreen)
{
	FAssert(eYield >= 0);
	FAssert(eYield < NUM_YIELD_TYPES);

	if (!ma_tradeImportsMaintain.get(eYield))
	{
		FAssert(!isAutoImportStopped(eYield));
		return;
	}
	
	FAssertMsg(isImport(eYield), "Feeder service is active without import enabled");

	int iMaintainLevel = getAutoMaintainThreshold(eYield);
	int iStoredLevel   = getYieldStored(eYield);
	int iNeededLevel   = getProductionNeeded(eYield);

	if (!isAutoImportStopped(eYield) && iStoredLevel >= iMaintainLevel)
	{
		ma_tradeStopAutoImport.set(true, eYield);
	} else if (isAutoImportStopped(eYield) && (iNeededLevel > iStoredLevel || (iStoredLevel <= (iMaintainLevel*3)/4))) {
		ma_tradeStopAutoImport.set(false, eYield);
	} else if (!bUpdateScreen) {
		// nothing changed. Do not continue to screen update code.
		return;
	}

	if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
	{
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Domestic_Advisor_DIRTY_BIT, true);
	}
}

void CvCity::setAutoThresholdCache(YieldTypes eYield)
{
	int iProductionNeeded = getProductionNeededUncached(eYield);

	ma_productionNeeded.set(iProductionNeeded, eYield);

	if (iProductionNeeded == MAX_INT)
	{
		// the city isn't producing anything. Set threshold to 0 instead of infinity.
		ma_tradeAutoThreshold.set(0, eYield);
	} else {
		ma_tradeAutoThreshold.set(iProductionNeeded, eYield);
	}

#if 0
	// only first in production queue
	//ma_tradeAutoThreshold.keepMax(getProductionNeeded(eYield), eYield);
	// no need for this part. getProductionNeeded() is called twice in a row. The second result should never be bigger than the first.
#else
	// everything in production queue
	for  (CLLNode<OrderData>* pOrderNode = headOrderQueueNode(); pOrderNode != NULL; pOrderNode = nextOrderQueueNode(pOrderNode))
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			ma_tradeAutoThreshold.keepMax(getYieldProductionNeeded((UnitTypes)pOrderNode->m_data.iData1, eYield), eYield);
			break;

		case ORDER_CONSTRUCT:
			ma_tradeAutoThreshold.keepMax(getYieldProductionNeeded((BuildingTypes)pOrderNode->m_data.iData1, eYield), eYield);
			break;
		}
	}
#endif

	std::vector< std::pair<OrderTypes, int> > aOrders;
	getOrdersWaitingForYield(aOrders, eYield, false, 0);


	int iMax = aOrders.size();
	for(int i = 0; i < iMax; ++i) {
		switch (aOrders[i].first)
		{
		case ORDER_TRAIN:
			ma_tradeAutoThreshold.keepMax(getYieldProductionNeeded((UnitTypes)aOrders[i].second, eYield), eYield);
			break;

		case ORDER_CONSTRUCT:
			ma_tradeAutoThreshold.keepMax(getYieldProductionNeeded((BuildingTypes)aOrders[i].second, eYield), eYield);
			break;
		}
	}

	// increase threshold to manually set threshold, if needed
	ma_tradeAutoThreshold.keepMax(ma_tradeThreshold.get(eYield), eYield);

	checkImportsMaintain(eYield);
}

void CvCity::setAutoThresholdCache()
{
	for (int i = 0; i < NUM_YIELD_TYPES; i++)
	{
		YieldTypes eYield = (YieldTypes)i;
		setAutoThresholdCache(eYield);
		checkImportsMaintain(eYield);
	}
}
// transport feeder - end - Nightinggale

// auto traderoute - start - Nightinggale
void CvCity::setAutoExport(YieldTypes eYield, bool bExport)
{
	ma_tradeAutoExport.set(bExport, eYield);
}

void CvCity::handleAutoTraderouteSetup(bool bReset, bool bImportAll, bool bAutoExportAll)
{
	if (bReset)
	{
		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			YieldTypes eYield = (YieldTypes) iYield;
			if (GC.getYieldInfo(eYield).isCargo())
			{
				// the easiest way to reset the settings for the yield in question is to call doTask for that yield
				// note that doTask should be called directly, not gDLL->sendDoTask()
				// this is because this function runs in parallel on all computers in the network
				// as a result, generating network traffic would be a mistake, which could cause bugs and lag
				// this is why the network communication is skipped in this case, something which would normally be a bug
				doTask(TASK_YIELD_TRADEROUTE, iYield, 0, false, false, false, false);
			}
		}
		return;
	}

	if (bImportAll || bAutoExportAll)
	{
		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			YieldTypes eYield = (YieldTypes) iYield;
			if (GC.getYieldInfo(eYield).isExportYield())
			{
				bool bImport = bImportAll || isImport(eYield);
				bool bExport = isExport(eYield);
				bool bMaintainImport = getImportsMaintain(eYield);
				bool bAutoExport     = bAutoExportAll || isAutoExport(eYield);
				int iMaintainLevel   = getMaintainLevel(eYield);
				int iImportLimitLevel= getImportsLimit(eYield);
		
				int iBuffer = iMaintainLevel & 0xFFFF; // lowest 16 bits
				iBuffer |= (iImportLimitLevel & 0xFFFF) << 16; // next 16 bits

				doTask(TASK_YIELD_TRADEROUTE, iYield, iBuffer, bImport, bExport, bMaintainImport, bAutoExport);
			}
		}
	}
}

void CvCity::doAutoExport(YieldTypes eYield)
{
	if (isAutoExport(eYield))
	{
		int iMaintainLevel = getMaintainLevel(eYield);
		int iStored = getYieldStored(eYield);
		if (iStored > iMaintainLevel)
		{
			addExport(eYield);
		}
		else
		{
			removeExport(eYield);
		}
	}
}
// auto traderoute - end - Nightinggale

// PatchMod: Achievements START
bool CvCity::isHasSpecialBuilding(int iValue) const
{
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (GC.getBuildingInfo((BuildingTypes)iI).getSpecialBuildingType() == iValue && isHasBuilding((BuildingTypes)iI))
		{
			return true;
		}
	}
	return false;
}
// PatchMod: Achievements END

// TAC - LbD - Ray - START

bool CvCity::LbD_try_become_expert(CvUnit* convUnit, int base, int increase, int pre_rounds, int l_level) 
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
	
	// do not use feature if City Population = 1, because might destroy city
	if (getPopulation() == 1)
	{
		return false;
	}

	int calculatedChance = (base + (workedRounds - pre_rounds) * increase / l_level);
	
	if(!isHuman())
	{	
		calculatedChance = calculatedChance * ki_modifier / 100;
	}
	
	for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); ++iTrait)
	{
		TraitTypes eTrait = (TraitTypes) iTrait;
		if (eTrait != NO_TRAIT)
		{
			if (hasTrait(eTrait))
			{
				calculatedChance *= GC.getTraitInfo(eTrait).getLearningByDoingModifier() + 100;
				calculatedChance /= 100;
			}
		}
	}
								//agnat86, added LbD modifier for Sophisticated Trait
	//ray Multiplayer Random Fix
	//int randomValue = rand() % 1000 + 1;
	int randomValue = GC.getGameINLINE().getSorenRandNum(1000, "LbD Expert City");
	
	// no Success if randomValue larger calculatedChance
	if (randomValue > calculatedChance)
	{
		return false;
	}
	
	// convert Unit to expert
	int expert = GC.getProfessionInfo(convUnit->getProfession()).LbD_getExpert();

		
	UnitTypes expertUnitType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(expert);
	FAssert(expertUnitType != NO_UNIT);
	//ray16
	CvUnit* expertUnit = GET_PLAYER(getOwnerINLINE()).initUnit(expertUnitType, NO_PROFESSION, getX_INLINE(), getY_INLINE(), convUnit->AI_getUnitAIType());
	FAssert(expertUnit != NULL);
	bool remove = removePopulationUnit(convUnit, false, (ProfessionTypes) GC.getCivilizationInfo(GET_PLAYER(getOwnerINLINE()).getCivilizationType()).getDefaultProfession());
	expertUnit->convert(convUnit, true);
	//addPopulationUnit(expertUnit, NO_PROFESSION);
	//AddMessage
	CvWString szBuffer = gDLL->getText("TXT_KEY_LBD_EXPERT_IN_CITY", getNameKey(), expertUnit->getUnitInfo().getDescription());
	//Ende ray16
	gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, expertUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), true, true);

	return true;
}

bool CvCity::LbD_try_get_free(CvUnit* convUnit, int base, int increase, int pre_rounds, int mod_crim, int mod_serv, int l_level) 
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
	
	// do not use feature if City Population = 1, because might destroy city
	if (getPopulation() == 1)
	{
		return false;
	}

	if (workedRounds < pre_rounds)
	{
		return false;
	}

	//cases criminal or servant
	int modcase = convUnit->getUnitInfo().getUnitClassType();

	// WTP, ray, LbD Slaves Revolt and Free - START
	// modified code to have specific Units for African Slaves and Native Slaves

	// store the UnitClass and modifier
	UnitTypes GeneratedUnitType;
	int mod;
	
	// convert Unit to Free Unit (Colonist, Freed Slave or Converted Native

	// if CRIMIAL general Default Pop-Unit
	if (modcase == GC.getDefineINT("UNITCLASS_PRISONER"))
	{
		mod = mod_crim; // Criminals are supposed to have a lower chance
		GeneratedUnitType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("DEFAULT_POPULATION_UNIT"));
	}

	// If African Slave generate a Freed Slave
	else if (modcase == GC.getDefineINT("UNITCLASS_AFRICAN_SLAVE"))
	{
		mod = mod_crim; // we just use the same lower chance as for Criminals
		GeneratedUnitType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_FREED_SLAVE"));
	}

	// If Native Slave generate a Converted Native
	else if (modcase == GC.getDefineINT("UNITCLASS_NATIVE_SLAVE"))
	{
		mod = mod_crim; // we just use the same lower chance as for Criminals
		GeneratedUnitType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_CONVERTED_NATIVE"));
	}

	//default case is servant (and potential others to be configured)
	else
	{
		mod = mod_serv; // Servants are supposed to have a higher chance
		GeneratedUnitType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("DEFAULT_POPULATION_UNIT"));
	}

	// WTP, ray, LbD Slaves Revolt and Free - END

	int calculatedChance = (base + (workedRounds - pre_rounds) * increase * l_level * mod);
	
	//ray Multiplayer Random Fix
	//int randomValue = rand() % 1000 + 1;
	int randomValue = GC.getGameINLINE().getSorenRandNum(1000, "LbD Free City");
	
	// no Success if randomValue larger calculatedChance
	if (randomValue > calculatedChance)
	{
		return false;
	}
	
	// spawn the Unit
	//ray16
	CvUnit* GeneratedUnit = GET_PLAYER(getOwnerINLINE()).initUnit(GeneratedUnitType, NO_PROFESSION, getX_INLINE(), getY_INLINE(), convUnit->AI_getUnitAIType());
	FAssert(GeneratedUnit != NULL);
	bool remove = removePopulationUnit(convUnit, false, (ProfessionTypes) GC.getCivilizationInfo(GET_PLAYER(getOwnerINLINE()).getCivilizationType()).getDefaultProfession());
	GeneratedUnit->convert(convUnit, true);
	// AddMessage
	CvWString szBuffer = gDLL->getText("TXT_KEY_LBD_FREE_IN_CITY", getNameKey());
	//Ende ray16
	gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, GeneratedUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), true, true);

	return true;
}

bool CvCity::LbD_try_escape(CvUnit* convUnit, int base, int mod_crim, int mod_serv) 
{	
	//Feature deactivated for KI
	if (!isHuman()) {
		return false;
	}

	// do not use feature if City Population = 1, because might destroy city
	if (getPopulation() == 1)
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

	// TODO: cases criminal or servant
	int calculatedChance = (base * mod);

	//ray Multiplayer Random Fix
	//int randomValue = rand() % 1000 + 1;
	int randomValue = GC.getGameINLINE().getSorenRandNum(1000, "LbD Escape City");
	
	// no Success if randomValue larger calculatedChance
	if (randomValue > calculatedChance)
	{
		return false;
	}

	const TCHAR* buttonStringForMessage = convUnit->getButton();

	// with true, we greate Unit with Default AI, otherwise it is AI_Flee
	createFleeingUnit(convUnit->getUnitType(), false);

	//Unit is then simply destroyed
	bool remove = removePopulationUnit(convUnit, false, (ProfessionTypes) GC.getCivilizationInfo(GET_PLAYER(getOwnerINLINE()).getCivilizationType()).getDefaultProfession());
	convUnit->kill(false);
	
	// AddMessage
	//ray16
	CvWString szBuffer = gDLL->getText("TXT_KEY_LBD_ESCAPE", getNameKey());
	//Ende ray16
	gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, buttonStringForMessage, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), true, true);

	return true;
}

// WTP, ray, LbD Slaves Revolt and Free - START
bool CvCity::LbD_try_revolt(CvUnit* convUnit, int base, int mod_crim, int mod_slave) 
{	
	//Feature deactivated for KI
	if (!isHuman()) {
		return false;
	}

	// do not use feature if City Population = 1, because might destroy city
	if (getPopulation() == 1)
	{
		return false;
	}

	//cases criminal or slave
	int modcase = convUnit->getUnitInfo().getUnitClassType();

	//default case is Criminal - also if we ever have something else
	int mod;
	UnitTypes GeneratedUnitType;
	
	// if African Slave
	if (modcase == GC.getDefineINT("UNITCLASS_AFRICAN_SLAVE"))
	{
		mod = mod_slave;
		GeneratedUnitType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_REVOLTING_SLAVE"));
	}
	// if Native Slave
	//default case is Criminal - also if we ever have something else
	else if (modcase == GC.getDefineINT("UNITCLASS_NATIVE_SLAVE"))
	{
		mod = mod_slave;
		GeneratedUnitType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_REVOLTING_NATIVE_SLAVE"));
	}
	//default case is Criminal - also if we ever have something else
	else
	{
		mod = mod_crim;
		GeneratedUnitType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_REVOLTING_CRIMINAL"));
	}

	// get chance and random value
	int calculatedChance = (base * mod);
	int randomValue = GC.getGameINLINE().getSorenRandNum(1000, "LbD Revolt Slave");
	
	// no Success if randomValue larger calculatedChance
	if (randomValue > calculatedChance)
	{
		return false;
	}

	// get Button of the Unit from UnitType for message
	// Test um Bug zu finden
	// CvUnit* GeneratedUnit = GET_PLAYER(getOwnerINLINE()).getUnit(GeneratedUnitType);
	// const TCHAR* buttonStringForMessage = GeneratedUnit->getButton();
	const TCHAR* buttonStringForMessage = convUnit->getButton();

	// with true, we greate Unit with Default AI, otherwise it is AI_Flee
	// Test um Bug zu finden
	createFleeingUnit(GeneratedUnitType, true);

	//Unit is then simply destroyed
	bool remove = removePopulationUnit(convUnit, false, (ProfessionTypes) GC.getCivilizationInfo(GET_PLAYER(getOwnerINLINE()).getCivilizationType()).getDefaultProfession());
	convUnit->kill(false);
	
	// AddMessage
	CvWString szBuffer = gDLL->getText("TXT_KEY_LBD_REVOLT", getNameKey());
	gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, buttonStringForMessage, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), true, true);

	return true;
}
// WTP, ray, LbD Slaves Revolt and Free - END

void CvCity::doLbD()
{	
	//getting global values for formula
	int base_chance_expert = GC.getLBD_BASE_CHANCE_EXPERT();	
	int chance_increase_expert = GC.getLBD_CHANCE_INCREASE_EXPERT();
	int pre_rounds_expert = GC.getLBD_PRE_ROUNDS_EXPERT();

	int base_chance_free = GC.getLBD_BASE_CHANCE_FREE();
	int chance_increase_free = GC.getLBD_CHANCE_INCREASE_FREE();
	int pre_rounds_free = GC.getLBD_PRE_ROUNDS_FREE();
	int mod_free_criminal = GC.getLBD_CHANCE_MOD_FREE_CRIMINAL();
	int mod_free_servant = GC.getLBD_CHANCE_MOD_FREE_SERVANT();

	int base_chance_escape = GC.getLBD_CHANCE_ESCAPE();
	int mod_escape_criminal = GC.getLBD_CHANCE_MOD_ESCAPE_CRIMINAL();
	int mod_escape_servant = GC.getLBD_CHANCE_MOD_ESCAPE_SERVANT();

	// WTP, ray, LbD Slaves Revolt and Free - START
	int base_chance_revolt = GC.getLBD_CHANCE_REVOLT();
	int mod_revolt_slave = GC.getLBD_CHANCE_MOD_REVOLT_SLAVE();
	int mod_revolt_criminal = GC.getLBD_CHANCE_MOD_REVOLT_CRIMINAL();
	// WTP, ray, LbD Slaves Revolt and Free - END

	//getting GameSpeedModifiert in percent
	int train_percent = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();

	//moddifying values with GameSpeed
	chance_increase_expert = chance_increase_expert / train_percent / 100;
	pre_rounds_expert = pre_rounds_expert * train_percent / 100;

	chance_increase_free = chance_increase_free / train_percent / 100;
	pre_rounds_free = pre_rounds_free * train_percent / 100;

	// WTP, ray, Happiness - START
	// we now calculate and apply Happiness vs. Unhappiness on it
	int iCityHappiness = getCityHappiness();
	int iCityUnHappiness = getCityUnHappiness();

	// this is the percentage rate we apply for positive features - it may become negative but that is intended
	int iPosHappinessBalance = iCityHappiness - iCityUnHappiness;

	// become expert Happiness vs Unhappiness modification
	base_chance_expert = base_chance_expert + (iPosHappinessBalance / 5); // chances are increased for positive values iPosHappinessBalance, decreased for negative values iPosHappinessBalance
	chance_increase_expert = chance_increase_expert + (iPosHappinessBalance / 5); // chances are increased for positive values iPosHappinessBalance, decreased for negative values iPosHappinessBalance
	pre_rounds_expert = pre_rounds_expert * (100 - iPosHappinessBalance) / 100; // pre rounds experts is shortened

	// become free Happiness vs Unhappiness modification
	base_chance_free = base_chance_free + (iPosHappinessBalance / 5); // chances are increased for positive values iPosHappinessBalance, decreased for negative values iPosHappinessBalance
	chance_increase_free = chance_increase_free + (iPosHappinessBalance / 5); // chances are increased for positive values iPosHappinessBalance, decreased for negative values iPosHappinessBalance
	pre_rounds_free = pre_rounds_free * (100 - iPosHappinessBalance) / 100; // pre rounds free is shortened

	// this is the percentage rate we apply for negative features - it may become negative but that is intended
	int iNegHappinessBalance = iCityUnHappiness - iCityHappiness;

	// escape and revolt Unhappiness vs Happiness modification
	base_chance_escape = base_chance_escape + (iNegHappinessBalance / 5); // chances are increased for positive values iNegHappinessBalance, decreased for negative values iNegHappinessBalance
	base_chance_revolt = base_chance_revolt + (iNegHappinessBalance / 5); // chances are increased for positive values iNegHappinessBalance, decreased for negative values iNegHappinessBalance

	// WTP, ray, Happiness - END

	// loop through units
	for (uint i = 0; i < m_aPopulationUnits.size(); ++i)
	{
		CvUnit* pLoopUnit = m_aPopulationUnits[i];

		bool lbd_expert_successful = false;
		bool lbd_free_successful = false;
		bool lbd_escape_successful = false;
		bool lbd_revolt_successful = false; // WTP, ray, LbD Slaves Revolt and Free - START
		

		// only do something for this unit if the profession does use LbD
		if (pLoopUnit->getProfession()!= NO_PROFESSION && GC.getProfessionInfo(pLoopUnit->getProfession()).LbD_isUsed())
		{
			
			//get LearnLevel of profession
			int learn_level = GC.getProfessionInfo(pLoopUnit->getProfession()).LbD_getLearnLevel();
			// just for safety, catch possible XML mistakes which might break calculation
			if (learn_level == 0)
			{
				learn_level = 1;
			}
	
			// try to become expert if poosible
			if(pLoopUnit->getUnitInfo().LbD_canBecomeExpert())
			{
				lbd_expert_successful = LbD_try_become_expert(pLoopUnit, base_chance_expert, chance_increase_expert, pre_rounds_expert, learn_level);
			}

			// try to become free if poosible
			if(pLoopUnit->getUnitInfo().LbD_canGetFree() && !lbd_expert_successful)
			{
				lbd_free_successful = LbD_try_get_free(pLoopUnit, base_chance_free, chance_increase_free, pre_rounds_free, mod_free_criminal, mod_free_servant, learn_level);
			}

			// try to escape if free unsuccesful and escape possible
			if(pLoopUnit->getUnitInfo().LbD_canEscape() && !lbd_free_successful && !lbd_expert_successful)
			{
				lbd_escape_successful = LbD_try_escape(pLoopUnit, base_chance_escape, mod_escape_criminal, mod_escape_servant);
			}

			// WTP, ray, LbD Slaves Revolt and Free - START
			// try to revolt if escape revolt possible and free unsuccessful and escape unsuccessufl
			if(pLoopUnit->getUnitInfo().LbD_canRevolt() && !lbd_escape_successful && !lbd_free_successful && !lbd_expert_successful)
			{
				lbd_revolt_successful = LbD_try_revolt(pLoopUnit, base_chance_revolt, mod_revolt_criminal, mod_revolt_slave);
			}
			// WTP, ray, LbD Slaves Revolt and Free - END

		}
		
	}

}
// TAC - LbD - Ray - END

// R&R, ray, Extra City Defense Attacks - START
void CvCity::doExtraCityDefenseAttacks()
{
	// Do the Cannons Defense Attack
	if (getBuildingDefense() > 120) // This is only possible if Citadel or Bastion have been built
	{

		int citadelDamageMultiplierLand = GC.getDefineINT("CITADEL_DAMAGE_MULTIPLIER_LAND");
		int citadelDamageMultiplierWater = GC.getDefineINT("CITADEL_DAMAGE_MULTIPLIER_WATER");

		CvWString szBuffer;
		CvPlot* pPlot = plot();
		CvPlot* pAdjacentPlot = NULL;
		CvUnit* pBombUnit = NULL;
		CvUnit* pLoopUnit = NULL;
		CvUnit* pLoopUnit2 = NULL;
		bool bCanBomb = false;
		CLLNode<IDInfo>* pUnitNode = NULL;
		CLLNode<IDInfo>* pUnitNode2 = NULL;
		bool alreadyBombarded = false;

		pUnitNode = pPlot->headUnitNode();

		while (pUnitNode && !alreadyBombarded)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = plot()->nextUnitNode(pUnitNode);
		
			bool isCannonType = (pLoopUnit->bombardRate() > 0 && pLoopUnit->getDomainType() == DOMAIN_LAND);
			if (isCannonType && pLoopUnit->getOwnerINLINE() == getOwnerINLINE())
			{
				bCanBomb = false;
				if (pLoopUnit->getFortifyTurns() > 0)
				{
					bCanBomb = true;
					pBombUnit = pLoopUnit;
				}

				if (bCanBomb)
				{
					for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
					{
						pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));
						if (pAdjacentPlot != NULL)
						{
							pUnitNode2 = pAdjacentPlot->headUnitNode();
							while (pUnitNode2)
							{
								pLoopUnit2 = ::getUnit(pUnitNode2->m_data);
								pUnitNode2 = pAdjacentPlot->nextUnitNode(pUnitNode2);

								if (getTeam() != pLoopUnit2->getTeam() && (GET_TEAM(getTeam()).isAtWar(pLoopUnit2->getTeam()) || pLoopUnit2->getUnitInfo().isHiddenNationality()) && !pLoopUnit2->isCargo())
								{
									int randNum = GC.getGameINLINE().getSorenRandNum(100, "City Defense Bomb");
									int iDamage = 0;
									// 10 Percent chance to totally miss
									if (randNum > 10)
									{
										iDamage = pLoopUnit2->maxHitPoints() * pBombUnit->bombardRate() / 100 * randNum / 10;

										// using XML balancing modifier
										if (pAdjacentPlot->isWater())
										{
											iDamage = iDamage * citadelDamageMultiplierWater;
										}
										else
										{
											iDamage = iDamage * citadelDamageMultiplierLand;
										}

										//taking into account different strengths of units
										int iDefenderCombatMod = pLoopUnit2->baseCombatStr();
										if (iDefenderCombatMod == 0)
										{
											iDefenderCombatMod = 1;
										}
										iDamage = iDamage / iDefenderCombatMod;

										//checking Terrain
										int iTerrainDamageMod = 0;
										if (pAdjacentPlot->isWater())
										{
											iTerrainDamageMod = 0;
										}
										else
										{
											if (pAdjacentPlot->isPeak())
											{
												iTerrainDamageMod = 50;
											}
											else if (pAdjacentPlot->isHills())
											{
												iTerrainDamageMod = 25;
											}

											//Forests, Jungle ...
											if (pAdjacentPlot->getTerrainType() != NO_TERRAIN)
											{
												iTerrainDamageMod = iTerrainDamageMod + 20;
											}
										}
										iDamage = iDamage * (100 - iTerrainDamageMod) / 100;
									}
									else
									{
										iDamage = 0;
									}

									pLoopUnit2->changeDamage(iDamage, pBombUnit);

									//case enemy is in water
									if (pAdjacentPlot->isWater())
									{
										if (iDamage > 0)
										{
											if (pLoopUnit2->isDead())
											{
												int iExperience = pLoopUnit2->attackXPValue();
												iExperience = ((iExperience * pLoopUnit2->currCombatStr(plot(), pBombUnit)) / pBombUnit->currCombatStr(NULL, NULL));
												iExperience = range(iExperience, GC.getDefineINT("MIN_EXPERIENCE_PER_COMBAT"), GC.getDefineINT("MAX_EXPERIENCE_PER_COMBAT"));

												pBombUnit->changeExperience(iExperience, pLoopUnit2->maxXPValue(), true, plot()->getOwnerINLINE() == pBombUnit->getOwnerINLINE(), true);

												szBuffer = gDLL->getText("TXT_KEY_FORTBOMB_SUNK_GOOD", getNameKey());
												gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE(), true, true);

												szBuffer = gDLL->getText("TXT_KEY_FORTBOMB_SUNK_BAD", getNameKey());
												gDLL->getInterfaceIFace()->addMessage(pLoopUnit2->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pLoopUnit2->getX_INLINE(), pLoopUnit2->getY_INLINE(), true, true);
											}
											else
											{
												szBuffer = gDLL->getText("TXT_KEY_FORTBOMB_HIT_GOOD", getNameKey());
												gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE(), true, true);

												szBuffer = gDLL->getText("TXT_KEY_FORTBOMB_HIT_BAD", getNameKey());
												gDLL->getInterfaceIFace()->addMessage(pLoopUnit2->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pLoopUnit2->getX_INLINE(), pLoopUnit2->getY_INLINE(), true, true);
											}
										}
										else
										{
											szBuffer = gDLL->getText("TXT_KEY_FORTBOMB_MISS_BAD", getNameKey());
											gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);

											szBuffer = gDLL->getText("TXT_KEY_FORTBOMB_MISS_GOOD", getNameKey());
											gDLL->getInterfaceIFace()->addMessage(pLoopUnit2->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pLoopUnit2->getX_INLINE(), pLoopUnit2->getY_INLINE(), true, true);
										}
									}
									//case water end

									//case enemy is on land 
									if (!pAdjacentPlot->isWater())
									{
										if (iDamage > 0)
										{
											if (pLoopUnit2->isDead())
											{
												int iExperience = pLoopUnit2->attackXPValue();
												iExperience = ((iExperience * pLoopUnit2->currCombatStr(plot(), pBombUnit)) / pBombUnit->currCombatStr(NULL, NULL));
												iExperience = range(iExperience, GC.getDefineINT("MIN_EXPERIENCE_PER_COMBAT"), GC.getDefineINT("MAX_EXPERIENCE_PER_COMBAT"));

												pBombUnit->changeExperience(iExperience, pLoopUnit2->maxXPValue(), true, plot()->getOwnerINLINE() == pBombUnit->getOwnerINLINE(), true);

												szBuffer = gDLL->getText("TXT_KEY_FORTBOMB_DESTROYED_GOOD", getNameKey());
												gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE(), true, true);

												szBuffer = gDLL->getText("TXT_KEY_FORTBOMB_DESTROYED_BAD", getNameKey());
												gDLL->getInterfaceIFace()->addMessage(pLoopUnit2->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pLoopUnit2->getX_INLINE(), pLoopUnit2->getY_INLINE(), true, true);
											}
											else
											{
												szBuffer = gDLL->getText("TXT_KEY_FORTBOMB_HIT_LAND_GOOD", getNameKey(), iDamage);
												gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE(), true, true);

												szBuffer = gDLL->getText("TXT_KEY_FORTBOMB_HIT_LAND_BAD", getNameKey(), iDamage);
												gDLL->getInterfaceIFace()->addMessage(pLoopUnit2->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pLoopUnit2->getX_INLINE(), pLoopUnit2->getY_INLINE(), true, true);
											}
										}
										else
										{
											szBuffer = gDLL->getText("TXT_KEY_FORTBOMB_MISS_LAND_BAD", getNameKey());
											gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);

											szBuffer = gDLL->getText("TXT_KEY_FORTBOMB_MISS_LAND_GOOD", getNameKey());
											gDLL->getInterfaceIFace()->addMessage(pLoopUnit2->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pLoopUnit2->getX_INLINE(), pLoopUnit2->getY_INLINE(), true, true);
										}
									}
									//case land end

									if (pAdjacentPlot->isActiveVisible(false))
									{
										// Bombard entity mission
										CvMissionDefinition kDefiniton;
										kDefiniton.setMissionTime(GC.getMissionInfo(MISSION_BOMBARD).getTime() * gDLL->getSecsPerTurn());
										kDefiniton.setMissionType(MISSION_BOMBARD);
										kDefiniton.setPlot(pAdjacentPlot);
										kDefiniton.setUnit(BATTLE_UNIT_ATTACKER, pBombUnit);
										kDefiniton.setUnit(BATTLE_UNIT_DEFENDER, pLoopUnit2);
										gDLL->getEntityIFace()->AddMission(&kDefiniton);
									}

									if (pLoopUnit2->isDead())
									{
										pLoopUnit2->kill(false);
									}
								}

								// we stop, because we have already fired once. reason is balancing
								alreadyBombarded = true;
								pUnitNode2 = NULL;
							}
						}
					}
				}
			}
		}
	}


	// Do the Infantery Defense Attack
	if (getBuildingDefense() >= 60) // This is only true for Fort, Fortress and Stronghold.
	{
		int defenseDamageMultiplier = GC.getDefineINT("DEFENSE_DAMAGE_MULTIPLIER");

		CvWString szBuffer;
		CvPlot* pPlot = plot();
		CvPlot* pAdjacentPlot = NULL;
		CvUnit* pDefenseUnit = NULL;
		CvUnit* pLoopUnit = NULL;
		CvUnit* pLoopUnit2 = NULL;
		bool bCanFire = false;
		CLLNode<IDInfo>* pUnitNode = NULL;
		CLLNode<IDInfo>* pUnitNode2 = NULL;
		bool alreadyFired = false;

		pUnitNode = pPlot->headUnitNode();

		while (pUnitNode && !alreadyFired)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = plot()->nextUnitNode(pUnitNode);

			bool isInfanteryType = (pLoopUnit->getUnitClassType() == GC.getDefineINT("UNITCLASS_KING_REINFORCEMENT_LAND") || GC.getDefineINT("UNITCLASS_CONTINENTAL_GUARD") || (pLoopUnit->getProfession() != NO_PROFESSION && GC.getProfessionInfo(pLoopUnit->getProfession()).isCityDefender() && !GC.getProfessionInfo(pLoopUnit->getProfession()).isUnarmed()));

			if (isInfanteryType && pLoopUnit->getOwnerINLINE() == getOwnerINLINE())
			{
				bCanFire = false;
				if (pLoopUnit->getFortifyTurns() > 0)
				{
					bCanFire = true;
					pDefenseUnit = pLoopUnit;
				}

				if (bCanFire)
				{
					for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
					{
						pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));
						//we do not fire on Water here
						if (pAdjacentPlot != NULL && !pAdjacentPlot->isWater())
						{	
							pUnitNode2 = pAdjacentPlot->headUnitNode();
							while (pUnitNode2)
							{
								pLoopUnit2 = ::getUnit(pUnitNode2->m_data);
								pUnitNode2 = pAdjacentPlot->nextUnitNode(pUnitNode2);

								if (getTeam() != pLoopUnit2->getTeam() && (GET_TEAM(getTeam()).isAtWar(pLoopUnit2->getTeam()) || pLoopUnit2->getUnitInfo().isHiddenNationality()) && !pLoopUnit2->isCargo())
								{
									int defenseRandNum = GC.getGameINLINE().getSorenRandNum(100, "City Defense Attack");
									int iDamage = 0;

									//10 percent chance to miss
									if (defenseRandNum > 10)
									{
										iDamage = pLoopUnit2->maxHitPoints()* defenseRandNum / 100;
										iDamage = iDamage * defenseDamageMultiplier;

										//taking into account different strengths of units
										int iDefenderCombatMod = pLoopUnit2->baseCombatStr();
										if (iDefenderCombatMod == 0)
										{
											iDefenderCombatMod = 1;
										}
										
										iDamage = iDamage / iDefenderCombatMod;
										
										//checking Terrain
										int iTerrainDamageMod = 0;
										if (pAdjacentPlot->isWater())
										{
											iTerrainDamageMod = 0;
										}
										else
										{
											if (pAdjacentPlot->isPeak())
											{
												iTerrainDamageMod = 50;
											}
											else if (pAdjacentPlot->isHills())
											{
												iTerrainDamageMod = 25;
											}

											//Forests, Jungle ...
											if (pAdjacentPlot->getTerrainType() != NO_TERRAIN)
											{
												iTerrainDamageMod = iTerrainDamageMod + 20;
											}
										}
										iDamage = iDamage * (100 - iTerrainDamageMod) / 100;

										pLoopUnit2->changeDamage(iDamage, pDefenseUnit);
									}

									//case enemy is on land 
									if (!pAdjacentPlot->isWater())
									{
										if (iDamage > 0)
										{
											if (pLoopUnit2->isDead())
											{
												int iExperience = pLoopUnit2->attackXPValue();
												iExperience = ((iExperience * pLoopUnit2->currCombatStr(plot(), pDefenseUnit)) / pDefenseUnit->currCombatStr(NULL, NULL));
												iExperience = range(iExperience, GC.getDefineINT("MIN_EXPERIENCE_PER_COMBAT"), GC.getDefineINT("MAX_EXPERIENCE_PER_COMBAT"));

												pDefenseUnit->changeExperience(iExperience, pLoopUnit2->maxXPValue(), true, plot()->getOwnerINLINE() == pDefenseUnit->getOwnerINLINE(), true);

												szBuffer = gDLL->getText("TXT_KEY_FORTDEFENSE_DESTROYED_GOOD", getNameKey());
												gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE(), true, true);

												szBuffer = gDLL->getText("TXT_KEY_FORTDEFENSE_DESTROYED_BAD", getNameKey());
												gDLL->getInterfaceIFace()->addMessage(pLoopUnit2->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pLoopUnit2->getX_INLINE(), pLoopUnit2->getY_INLINE(), true, true);
											}
											else
											{
												szBuffer = gDLL->getText("TXT_KEY_FORTDEFENSE_HIT_LAND_GOOD", getNameKey(), iDamage);
												gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE(), true, true);

												szBuffer = gDLL->getText("TXT_KEY_FORTDEFENSE_HIT_LAND_BAD", getNameKey(), iDamage);
												gDLL->getInterfaceIFace()->addMessage(pLoopUnit2->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pLoopUnit2->getX_INLINE(), pLoopUnit2->getY_INLINE(), true, true);
											}
										}
										else
										{
											szBuffer = gDLL->getText("TXT_KEY_FORTDEFENSE_MISS_LAND_BAD", getNameKey());
											gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);

											szBuffer = gDLL->getText("TXT_KEY_FORTDEFENSE_MISS_LAND_GOOD", getNameKey());
											gDLL->getInterfaceIFace()->addMessage(pLoopUnit2->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pLoopUnit2->getX_INLINE(), pLoopUnit2->getY_INLINE(), true, true);
										}
									}
									//case land end

									if (pAdjacentPlot->isActiveVisible(false))
									{
										// Bombard pDefenseUnit mission
										CvMissionDefinition kDefiniton;
										kDefiniton.setMissionTime(GC.getMissionInfo(MISSION_BOMBARD).getTime() * gDLL->getSecsPerTurn());
										kDefiniton.setMissionType(MISSION_BOMBARD);
										kDefiniton.setPlot(pAdjacentPlot);
										kDefiniton.setUnit(BATTLE_UNIT_ATTACKER, pDefenseUnit);
										kDefiniton.setUnit(BATTLE_UNIT_DEFENDER, pLoopUnit2);
										gDLL->getEntityIFace()->AddMission(&kDefiniton);
									}

									if (pLoopUnit2->isDead())
									{
										pLoopUnit2->kill(false);
									}
								}
								// we stop, because we have already fired once. reason is balancing
								alreadyFired = true;
								pUnitNode2 = NULL;
							}
						}
					}
				}
			}
		}
	}

	return;
}
// R&R, ray, Extra City Defense Attacks - END


//Androrc Domestic Market
int CvCity::getYieldBuyPrice(YieldTypes eYield) const
{
	FAssert(eYield >= 0);
	FAssert(eYield < NUM_YIELD_TYPES);
	return m_aiYieldBuyPrice[eYield];
}

// R&R, ray, adjustment Domestic Markets
// No messages, because too many messages get annoying
void CvCity::setYieldBuyPrice(YieldTypes eYield, int iPrice)
{
	FAssert(eYield >= 0);
	FAssert(eYield < NUM_YIELD_TYPES);

	iPrice = std::max(iPrice, 1);
	if (iPrice != getYieldBuyPrice(eYield))
	{
		m_aiYieldBuyPrice[eYield] = iPrice;
	}
}

// R&R, ray, adjustment Domestic Markets
// No demand from buildings, because this feature would not be used and cost performance
int CvCity::getYieldDemand(YieldTypes eYield) const
{
	FAssert(eYield >= 0);
	FAssert(eYield < NUM_YIELD_TYPES);

	int iDemand = 0;

	for (uint i = 0; i < m_aPopulationUnits.size(); ++i)
	{
		CvUnit* pLoopUnit = m_aPopulationUnits[i];
		if (GC.getUnitInfo(pLoopUnit->getUnitType()).getYieldDemand(eYield) != 0)
		{
			iDemand += GC.getUnitInfo(pLoopUnit->getUnitType()).getYieldDemand(eYield);
		}
	}

	// R&R, ray, adjustment to new Market Buildings
	int MarketLevel = 0;
	for (int i = 0; i < GC.getNumBuildingInfos(); ++i)
	{
		BuildingTypes eBuilding = (BuildingTypes) i;
		CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
		if (kBuilding.getSpecialBuildingType() == GC.getDefineINT("SPECIALBUILDING_MARKET") && isHasBuilding(eBuilding))
		{
			if (kBuilding.getSpecialBuildingPriority() > MarketLevel)
			{
				MarketLevel = kBuilding.getSpecialBuildingPriority();
			}
		}
	}

	int iRawDemand = iDemand;
	int iBuildingDemand = (iDemand * (MarketLevel * 50)) / 100; // 50 percent more demand per level

	return (iRawDemand + iBuildingDemand) / 100;
}

// R&R, ray, adjustment Domestic Markets
// needs to be checked for Special Cases like Luxury Goods
void CvCity::doPrices()
{
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		YieldTypes eYield = (YieldTypes) iYield;
		CvYieldInfo& kYield = GC.getYieldInfo(eYield);

		if (kYield.isCargo())
		{
			// Luxury Goods should always give a little profit
			if (eYield == YIELD_LUXURY_GOODS)
			{
				int iBuyPrice = GET_PLAYER(getOwnerINLINE()).getYieldSellPrice(eYield) + GC.getPRICE_DIFF_EUROPE_DOMESTIC_LUXURY_GOODS();
				setYieldBuyPrice(eYield, iBuyPrice);
			}
			else
			{
				int iBaseThreshold = kYield.getPriceChangeThreshold() * GC.getHandicapInfo(getHandicapType()).getEuropePriceThresholdMultiplier() * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent() / 10000;
				int iNewPrice = kYield.getBuyPriceLow() + GC.getGameINLINE().getSorenRandNum(kYield.getBuyPriceHigh() - kYield.getBuyPriceLow() + 1, "Price selection");
				iNewPrice += getYieldStored(eYield) / std::max(1, iBaseThreshold);

				if (GC.getGameINLINE().getSorenRandNum(100, "Price correction") < kYield.getPriceCorrectionPercent() * std::abs(iNewPrice - getYieldBuyPrice(eYield)))
				{
					iNewPrice = std::min(iNewPrice, getYieldBuyPrice(eYield) + 1);
					iNewPrice = std::max(iNewPrice, getYieldBuyPrice(eYield) - 1);
					setYieldBuyPrice(eYield, iNewPrice);
				}
			}
		}
	}
}
//Androrc End

// R&R, ray, finishing Custom House Screen
void CvCity::setCustomHouseSellThreshold(YieldTypes eYield, int iCustomHouseSellThreshold)
{
	FAssert(eYield >= 0);
	FAssert(eYield < NUM_YIELD_TYPES);

	if (iCustomHouseSellThreshold != getCustomHouseSellThreshold(eYield))
	{
		ma_aiCustomHouseSellThreshold.set(iCustomHouseSellThreshold, eYield);
		if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		}
	}
}

int CvCity::getCustomHouseSellThreshold(YieldTypes eYield) const
{
	return ma_aiCustomHouseSellThreshold.get(eYield);
}

void CvCity::setCustomHouseNeverSell(YieldTypes eYield, bool bNeverSell)
{
	if (isCustomHouseNeverSell(eYield) != bNeverSell)
	{
		ma_aiCustomHouseNeverSell.set(bNeverSell, eYield);

		if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		}
	}
}

bool CvCity::isCustomHouseNeverSell(YieldTypes eYield) const
{
	return ma_aiCustomHouseNeverSell.get(eYield);
}
// R&R, ray, finishing Custom House Screen END

// R&R, ray, Fleeing Units - START
// WTP, ray, LbD Slaves Revolt and Free - START - adjusted to also have DefaultAI
void CvCity::createFleeingUnit(UnitTypes eUnit, bool bDefautAI)
{

	if (GC.getGameINLINE().getBarbarianPlayer() == NO_PLAYER)
    {
        return;
    }

	CvPlayer& barbarianPlayer = GET_PLAYER(GC.getGameINLINE().getBarbarianPlayer());

	// find plot
	CvPlot* pBestPlot = NULL;
	int iBestValue = MAX_INT;

	for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

		if (pLoopPlot->getTeam() == NO_TEAM && !pLoopPlot->isWater() && !pLoopPlot->isPeak() && !pLoopPlot->isFort() && !pLoopPlot->isMonastery() && pLoopPlot->getNumUnits() == 0)
		{
			int iValue = (plotDistance(getX_INLINE(), getY_INLINE(), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE()) * 2);
			if (pLoopPlot->area() != area())
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

	// init fleeing or revolting Unit
	if (pBestPlot != NULL)
	{
		// Fleeing AI
		if (bDefautAI == false)
		{
			CvUnit* fleeingUnit = barbarianPlayer.initUnit(eUnit, (ProfessionTypes) GC.getUnitInfo(eUnit).getDefaultProfession(), pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), UNITAI_FLEEING);
		}
		//Default AI
		else
		{
			CvUnit* fleeingUnit = barbarianPlayer.initUnit(eUnit, (ProfessionTypes) GC.getUnitInfo(eUnit).getDefaultProfession(), pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), NO_UNITAI);
		}
	}
}
// R&R, ray, Fleeing Units - END

// R&R, ray, Entertainment Buildings - START
void CvCity::doEntertainmentBuildings()
{
	int iCulturePerTurn = getCultureRate();
	SpecialBuildingTypes eSpecialBuilding = (SpecialBuildingTypes) GC.getDefineINT("SPECIALBUILDING_TAVERN");

	int factorFromBuildingLevel = 0;
	BuildingTypes highestLevelEntertainmentBuilding = NO_BUILDING;

	if (eSpecialBuilding != NO_SPECIALBUILDING)
	{
		for (int i = 0; i < GC.getNumBuildingInfos(); ++i)
		{
			BuildingTypes eBuilding = (BuildingTypes) i;
			CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
			if (kBuilding.getSpecialBuildingType() == eSpecialBuilding)
			{
				if (isHasBuilding(eBuilding))
				{
					factorFromBuildingLevel = kBuilding.getSpecialBuildingPriority();
					highestLevelEntertainmentBuilding = eBuilding;
				}
			}
		}
	}

	int iGoldthroughCulture = iCulturePerTurn +  ((iCulturePerTurn * factorFromBuildingLevel) / 2); // 50 percent extra for each level
	iGoldthroughCulture = iGoldthroughCulture * (100 + getCityHappiness() - getCityUnHappiness()) / 100; // WTP, ray, Happiness - START

	if (highestLevelEntertainmentBuilding != NO_BUILDING && iGoldthroughCulture > 0)
	{
		GET_PLAYER(getOwnerINLINE()).changeGold(iGoldthroughCulture);
		CvWString szBuffer = gDLL->getText("TXT_KEY_GOLD_BY_ENTERTAINMENT", GC.getBuildingInfo(highestLevelEntertainmentBuilding).getDescription(), getNameKey(), iGoldthroughCulture);
		gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX_INLINE(), getY_INLINE(), true, true);
	}
}
// R&R, ray, Entertainment Buildings - END

void CvCity::setPreferredYieldAtCityPlot(YieldTypes eYield)
{
	FAssert(eYield >= NO_YIELD);
	if (eYield < NO_YIELD)
	{
		return;
	}

	if (eYield == m_ePreferredYieldAtCityPlot)
	{
		// don't dirty a lot of stuff for no change
		return;
	}

	// the actual data change
	m_ePreferredYieldAtCityPlot = eYield;


	// update cache
	plot()->updateYield(false);
	setYieldRateDirty();

	if ((getTeam() == GC.getGameINLINE().getActiveTeam()) || GC.getGameINLINE().isDebugMode())
	{
		plot()->updateSymbolDisplay();
	}

	if (isCitySelected())
	{
		gDLL->getInterfaceIFace()->setDirty(ResourceTable_DIRTY_BIT, true);
	}
}

// building affected cache - start - Nightinggale
void CvCity::UpdateBuildingAffectedCache()
{
	NBMOD_SetCityTeachLevelCache(); // NBMOD EDU cache - Nightinggale
	m_cache_MaxYieldCapacity = getMaxYieldCapacityUncached(); // cache getMaxYieldCapacity - Nightinggale

	// CvPlot::hasYield cache - start - Nightinggale
	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		CvPlot* pLoopPlot = getCityIndexPlot(iI);

		// only work on plots owned by the city owner
		// this is needed when loading savegames as not all players will be loaded at this point
		if (pLoopPlot != NULL && pLoopPlot->getOwnerINLINE() == getOwnerINLINE())
		{
			pLoopPlot->setYieldCache();
		}
	}
	// CvPlot::hasYield cache - end - Nightinggale
}
// building affected cache - end - Nightinggale

void CvCity::writeDesyncLog(FILE *f)
{
	fprintf(f, "\t\tCulture level: %d\n", getCultureLevel());
	fprintf(f, "\t\tCulture rate: %d\n", getCultureRate());
	fprintf(f, "\t\tCulture threshold: %d\n", getCultureThreshold());
	fprintf(f, "\t\tCulture update timer: %d\n", getCultureUpdateTimer());

	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		int iCulture = this->getCulture(static_cast<PlayerTypes>(i));
		if (iCulture > 0)
		{
			fprintf(f, "\t\tCulture player %d: %d\n", i, iCulture);
		}
	}
}
