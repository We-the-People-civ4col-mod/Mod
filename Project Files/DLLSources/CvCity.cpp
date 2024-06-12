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

#include "CvSavegame.h"
#include "BetterBTSAI.h"

// Public Functions...

#pragma warning( disable: 4355 ) // using this as initializer in constructor

CvCity::CvCity()
	: m_yields(*this)
{

	m_paTradeCities = NULL;

	CvDLLEntity::createCityEntity(this);		// create and attach entity to city

	m_ePreferredYieldAtCityPlot = NO_YIELD;

	reset(0, NO_PLAYER, Coordinates(0, 0), true);
}

#pragma warning( default:4355 ) // using this as initializer in constructor

CvCity::~CvCity()
{
	CvDLLEntity::removeEntity();			// remove entity from engine
	CvDLLEntity::destroyEntity();			// delete CvCityEntity and detach from us

	uninit();
}


// void CvCity::init(int iID, PlayerTypes eOwner, int iX, int iY, bool bBumpUnits)
// {
// 	Coordinates initCoord(iX, iY);
// 	init(iID, eOwner, initCoord, bBumpUnits);
// }


void CvCity::init(int iID, PlayerTypes eOwner, Coordinates initCoord, bool bBumpUnits)
{
	std::vector<int> aOldAttitude(MAX_PLAYERS, 0);
	for (PlayerTypes ePlayer = FIRST_PLAYER; ePlayer < NUM_PLAYER_TYPES; ++ePlayer)
	{
		CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
		if (kPlayer.isAlive())
		{
			kPlayer.AI_invalidateCloseBordersAttitudeCache();
			aOldAttitude[ePlayer] = kPlayer.AI_getStolenPlotsAttitude(eOwner);
		}
	}

	CvPlot* pPlot;
	//BuildingTypes eLoopBuilding;

	pPlot = initCoord.plot();

	//--------------------------------
	// Log this event
	if (GC.getLogging())
	{
		if (gDLL->getChtLvl() > 0)
		{
			char szOut[1024];
			sprintf(szOut, "Player %d City %d built at %d:%d\n", eOwner, iID, initCoord.x(), initCoord.y());
			gDLL->messageControlLog(szOut);
		}
	}

	//--------------------------------
	// Init saved data
	reset(iID, eOwner, initCoord);
	// R&R, ray, adjustment Domestic Markets for Luxury Goods needed
	// R&R, Androrc, Domestic Market
	if(!isNative())
	{
		for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
		{
			const CvYieldInfo& kYield = GC.getYieldInfo(eYield);
			FAssert(kYield.getBuyPriceHigh() >= kYield.getBuyPriceLow());

			int iBuyPrice = 0;

			// Luxury Goods should also give a little profit
			if (eYield == YIELD_LUXURY_GOODS)
			{
				iBuyPrice = GET_PLAYER(getOwnerINLINE()).getYieldSellPrice(eYield) + GLOBAL_DEFINE_PRICE_DIFF_EUROPE_DOMESTIC_LUXURY_GOODS;
			}
			// WTP, now also Fieldworker Tools, but the Diff is only half
			else if (eYield == YIELD_FIELD_WORKER_TOOLS)
			{
				iBuyPrice = GET_PLAYER(getOwnerINLINE()).getYieldSellPrice(eYield) + (GLOBAL_DEFINE_PRICE_DIFF_EUROPE_DOMESTIC_LUXURY_GOODS/ 2);
			}
			// WTP, now also Household Goods, but the Diff is only half
			else if (eYield == YIELD_HOUSEHOLD_GOODS)
			{
				iBuyPrice = GET_PLAYER(getOwnerINLINE()).getYieldSellPrice(eYield) + (GLOBAL_DEFINE_PRICE_DIFF_EUROPE_DOMESTIC_LUXURY_GOODS / 2);
			}
			else
			{
				iBuyPrice = kYield.getBuyPriceLow() + GC.getGameINLINE().getSorenRandNum(kYield.getBuyPriceHigh() - kYield.getBuyPriceLow() + 1, "Yield Price");
				// WTP, trying to fix issue that Domestic Market became unattractive
				iBuyPrice += GLOBAL_DEFINE_PRICE_DIFF_EUROPE_DOMESTIC_OTHER_GOODS;
			}

			setYieldBuyPrice(eYield, iBuyPrice);

		}
	}
	//Androrc End

	// R&R, ray, finishing Custom House Screen
	// initializing default data
	if(!isNative())
	{
		for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
		{
			const CvYieldInfo& kYield = GC.getYieldInfo(eYield);
			const int iGameSpeedModifier = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getStoragePercent() / 100; // R&R, ray adjusting defaults of Custom House Screen to Gamespeed
			if (kYield.isCargo())
			{
				// strategic raw
				if (eYield == YIELD_ORE || eYield == YIELD_COAL || eYield == YIELD_CHAR_COAL || eYield == YIELD_PEAT || eYield == YIELD_HEMP || eYield == YIELD_FLAX)
				{
					m_em_iCustomHouseSellThreshold.set(eYield, GC.getCUSTOMHOUSE_STRATEGIC_RAW_SELL_THRESHOLD() * iGameSpeedModifier);
				}
				// strategic produced
				else if ( eYield == YIELD_TOOLS || eYield == YIELD_BLADES || eYield == YIELD_MUSKETS || eYield == YIELD_CANNONS || eYield == YIELD_BLACK_POWDER || eYield == YIELD_SHEEP || eYield == YIELD_GOATS || eYield == YIELD_PIGS || eYield == YIELD_CATTLE || eYield == YIELD_CHICKEN || eYield == YIELD_GEESE || eYield == YIELD_HORSES || eYield == YIELD_BAKERY_GOODS || eYield == YIELD_ROPE || eYield == YIELD_SAILCLOTH || eYield == YIELD_TRADE_GOODS || eYield == YIELD_FIELD_WORKER_TOOLS || eYield == YIELD_HOUSEHOLD_GOODS || eYield == YIELD_LUXURY_GOODS)
				{
					m_em_iCustomHouseSellThreshold.set(eYield, GC.getCUSTOMHOUSE_STRATEGIC_PRODUCED_SELL_THRESHOLD() * iGameSpeedModifier);
				}

				// raw
				else if ( eYield == YIELD_WOOL || eYield == YIELD_GOAT_HIDES ||eYield == YIELD_PIG_SKIN || eYield == YIELD_HIDES || eYield == YIELD_DOWNS || eYield == YIELD_FUR || eYield == YIELD_PREMIUM_FUR || eYield == YIELD_BARLEY || eYield == YIELD_SUGAR || eYield == YIELD_FRUITS || eYield == YIELD_GRAPES || eYield == YIELD_OLIVES || eYield == YIELD_RAPE || eYield == YIELD_COCOA_FRUITS || eYield == YIELD_COFFEE_BERRIES || eYield == YIELD_PEANUTS || eYield == YIELD_MILK || eYield == YIELD_RAW_SALT || eYield == YIELD_RED_PEPPER || eYield == YIELD_VANILLA_PODS || eYield == YIELD_COTTON || eYield == YIELD_INDIGO || eYield == YIELD_LOGWOOD || eYield == YIELD_COCHINEAL || eYield == YIELD_TOBACCO || eYield == YIELD_YERBA_LEAVES || eYield == YIELD_WHALE_BLUBBER || eYield == YIELD_VALUABLE_WOOD)
				{
					m_em_iCustomHouseSellThreshold.set(eYield, GC.getCUSTOMHOUSE_RAW_SELL_THRESHOLD() * iGameSpeedModifier);
				}

				// produced
				else if (eYield == YIELD_RICE || eYield == YIELD_CASSAVA || eYield == YIELD_COCA_LEAVES || eYield == YIELD_MAPLE_SIRUP || eYield == YIELD_KAUTSCHUK || eYield == YIELD_WILD_FEATHERS || eYield == YIELD_SILVER || eYield == YIELD_GOLD || eYield == YIELD_GEMS || eYield == YIELD_COCOA || eYield == YIELD_COFFEE || eYield == YIELD_ROASTED_PEANUTS || eYield == YIELD_CHEESE || eYield == YIELD_SALT || eYield == YIELD_SPICES || eYield == YIELD_VANILLA || eYield == YIELD_CHOCOLATE || eYield == YIELD_CLOTH || eYield == YIELD_COLOURED_CLOTH || eYield == YIELD_FESTIVE_CLOTHES || eYield == YIELD_WOOL_CLOTH || eYield == YIELD_COLOURED_WOOL_CLOTH || eYield == YIELD_EVERYDAY_CLOTHES || eYield == YIELD_PIG_LEATHER || eYield == YIELD_LEATHER || eYield == YIELD_GOAT_HIDE_BOOTS || eYield == YIELD_PADDED_LEATHER_COATS || eYield == YIELD_COATS || eYield == YIELD_PREMIUM_COATS || eYield == YIELD_BEER || eYield == YIELD_RUM || eYield == YIELD_HOOCH || eYield == YIELD_WINE || eYield == YIELD_OLIVE_OIL || eYield == YIELD_RAPE_OIL || eYield == YIELD_CIGARS || eYield == YIELD_YERBA_TEA || eYield == YIELD_WHALE_OIL || eYield == YIELD_POTTERY || eYield == YIELD_FURNITURE || eYield == YIELD_PADDED_FURNITURE)
				{
					m_em_iCustomHouseSellThreshold.set(eYield, GC.getCUSTOMHOUSE_PRODUCED_SELL_THRESHOLD() * iGameSpeedModifier);
				}

				// default for safety
				else
				{
					m_em_iCustomHouseSellThreshold.set(eYield, getYieldStored(eYield)* iGameSpeedModifier / 2);
				}

				// ray, making special storage capacity rules for Yields XML configurable
				// if (eYield == YIELD_FOOD || eYield == YIELD_LUMBER || eYield == YIELD_STONE)
				if(GC.getYieldInfo(eYield).isIgnoredForStorageCapacity())
				{
					m_em_bCustomHouseNeverSell.set(eYield, true);
				}
				else
				{
					m_em_bCustomHouseNeverSell.set(eYield, false);
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

	for (DirectionTypes eDirection = FIRST_DIRECTION; eDirection < NUM_DIRECTION_TYPES; ++eDirection)
	{
		CvPlot* const pAdjacentPlot = coord().neighbourPlot(eDirection);

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
	argsList.add(initCoord.x());
	argsList.add(initCoord.y());
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

	for (TeamTypes eTeam = FIRST_TEAM; eTeam < NUM_TEAM_TYPES; ++eTeam)
	{
		if (GET_TEAM(eTeam).isAlive())
		{
			if (pPlot->isVisible((eTeam), false))
			{
				setRevealed((eTeam), true);
			}
		}
	}

	// WTP, ray, refactored according to advice of Nightinggale
	for (BuildingTypes eLoopBuilding = FIRST_BUILDING; eLoopBuilding < NUM_BUILDING_TYPES; ++eLoopBuilding)
	{
		if (GET_PLAYER(getOwnerINLINE()).isBuildingFree(eLoopBuilding))
		{
			setHasFreeBuilding((eLoopBuilding), true);
		}
	}

	area()->changeCitiesPerPlayer(getOwnerINLINE(), 1);

	GET_TEAM(getTeam()).changeNumCities(1);

	GC.getGameINLINE().changeNumCities(1);

	setGameTurnFounded(GC.getGameINLINE().getGameTurn());
	setGameTurnAcquired(GC.getGameINLINE().getGameTurn());

	pPlot->updateYield(false);
	setYieldRateDirty();

	changePopulation(GLOBAL_DEFINE_INITIAL_CITY_POPULATION + GC.getEraInfo(GC.getGameINLINE().getStartEra()).getFreePopulation());

	GC.getMap().updateWorkingCity();

	GC.getGameINLINE().AI_makeAssignWorkDirty();

	GET_PLAYER(getOwnerINLINE()).setFoundedFirstCity(true);

	if (GC.getGameINLINE().isFinalInitialized() || isNative())
	{
		for (BuildingClassTypes eLoopBuildingClass = FIRST_BUILDINGCLASS; eLoopBuildingClass < NUM_BUILDINGCLASS_TYPES; ++eLoopBuildingClass)
		{
			if (GC.getCivilizationInfo(getCivilizationType()).isCivilizationFreeBuildingClass(eLoopBuildingClass))
			{
				BuildingTypes eLoopBuilding = ((BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(eLoopBuildingClass)));

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
	m_iSlaveWorkerProductionBonus = 0;

	m_iOppressometer = 0;
	m_iOppressometerGrowthModifier = 100;
}


void CvCity::uninit()
{
	// Teacher List - start - Nightinggale
	m_em_iOrderedStudents.reset();
	m_em_bOrderedStudentsRepeat.reset();
	// Teacher List - end - Nightinggale

	SAFE_DELETE_ARRAY(m_paTradeCities);

	m_orderQueue.clear();

	//free unit population
	for (uint i = 0; i < m_aPopulationUnits.size(); ++i)
	{
		SAFE_DELETE(m_aPopulationUnits[i]);
	}
	m_aPopulationUnits.clear();

	// traderoute just-in-time - start - Nightinggale
	m_em_bTradeImports.reset();
	m_em_bTradeExports.reset();
	m_em_iTradeThreshold.reset();
	// traderoute just-in-time - end - Nightinggale
	m_em_iTradeMaxThreshold.reset(); // R&R mod, vetiarvind, max yield import limit
	// transport feeder - start - Nightinggale
	m_em_bTradeImportsMaintain.reset();
	// transport feeder - end - Nightinggale
}

// FUNCTION: reset()
// Initializes data members that are serialized.
// void CvCity::reset(int iID, PlayerTypes eOwner, int iX, int iY, bool bConstructorCall)
// {
// 	Coordinates resetCoord(iX, iY);
// 	reset(iID, eOwner, resetCoord, bConstructorCall);
// }

void CvCity::reset(int iID, PlayerTypes eOwner, Coordinates resetCoord, bool bConstructorCall)
{
	//--------------------------------
	// Uninit class
	uninit();

	m_bInfoDirty = true;
	m_bLayoutDirty = false;

	m_iCacheMarketModifier = 0;

	resetSavedData(iID, eOwner, resetCoord, bConstructorCall);

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
			CvUnit* pLoopUnit = plot()->getUnitNodeLoop(pUnitNode);

			if (pLoopUnit != NULL && pLoopUnit->getUnitTravelState() == UNIT_TRAVEL_STATE_LIVE_AMONG_NATIVES)
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

	FOREACH(CityPlot)
	{
		CvPlot* pLoopPlot = getCityIndexPlot(eLoopCityPlot);

		if (pLoopPlot != NULL)
		{
			if (pLoopPlot->getWorkingCityOverride() == this)
			{
				pLoopPlot->setWorkingCityOverride(NULL);
			}
		}
	}

	setCultureLevel(NO_CULTURELEVEL);

	// WTP, ray, refactored according to advice of Nightinggale
	for (BuildingTypes eLoopBuilding = FIRST_BUILDING; eLoopBuilding < NUM_BUILDING_TYPES; ++eLoopBuilding)
	{
		setHasRealBuilding((eLoopBuilding), false);
		setHasFreeBuilding((eLoopBuilding), false);
	}

	for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
	{
		setYieldStored(eYield, 0);
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

	GC.getMap().updateWorkingCity();

	GC.getGameINLINE().AI_makeAssignWorkDirty();

	if (eOwner == GC.getGameINLINE().getActivePlayer())
	{
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
	}
}


void CvCity::doTurn()
{
	OOS_LOG("CvCity::doTurn start", getID());

	PROFILE_FUNC();

	if (!isBombarded())
	{
		changeDefenseDamage(-(GC.getDefineINT("CITY_DEFENSE_DAMAGE_HEAL_RATE")));
	}

	setLastDefenseDamage(getDefenseDamage());
	setBombarded(false);

	AI_doTurn();

	const bool bAllowNoProduction = !doCheckProduction();

	doSpecialists();

	if (!isDisorder())
	{
		if (!isNative())
		{
			updateSlaveWorkerProductionBonus(); // WTP, ray, since it is cached / recalculated it needs to be recalculated here
		}
		doYields();
		doGrowth();
		doCulture();
		doPlotCulture(false, getOwnerINLINE(), getCultureRate());
	}


	if (!m_bHasHurried && !isHuman() && !isNative())
	{
		// Hurry needs to happen just before the production processing to avoid
		// the hurried yields from being used for other purposes
		static_cast<CvCityAI*>(this)->AI_doHurry();
	}

	if (!isDisorder())
	{
		doProduction(bAllowNoProduction);
	}

	if (!isNative())
	{
		// WTP, ray fixing Asserts by functions being called during disorder - START
		// we do not call these anymore if in disorder
		if (!isDisorder())
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
			// WTP, ray, Crime and Law - START
			updateCityLaw();
			updateCityCrime();
			doCityLaw();
			doCityCrime();
			// WTP, ray, Crime and Law - END
			checkForDomesticDemandEvent(); // WTP, ray Domestic Market Events - START
			OOS_LOG("checkForDomesticDemandEvent end", getID());
		}
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
		FOREACH(CityPlot)
		{
			CvPlot* const pLoopPlot = getCityIndexPlot(eLoopCityPlot);

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
			gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_CITY_OCCUPATION_ENDED", getNameKey()), coord(), "AS2D_POSITIVE_DINK", MESSAGE_TYPE_INFO, GC.getYieldInfo(YIELD_CULTURE).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), true, true);
		}
		// WTP, ray, Change for Request "Occupation has ended" - START
	}

	for (uint i = 0; i < m_aPopulationUnits.size(); ++i)
	{
		m_aPopulationUnits[i]->doTurn();
	}

	// Fixing stuck units in colonies - Nightinggale - Start
	// Once in a while a citizen can lose profession in a colony
	// This can happen if fishermen are removed by pirates, but apparently also in other cases
	// Without a profession, the unit is hidden in the colony screen, hence no way for the player to change the profession
	// The code here will kick out such units, which will make them available to the player again
	// Doesn't fix the cause of the problem, but this way the player will regain lost units during new turn event
	if (isHuman() && !isOccupation())
	{
		std::vector<CvUnit*> stuckUnits;
		for (uint i = 0; i < m_aPopulationUnits.size(); ++i)
		{
			CvUnit* pUnit = m_aPopulationUnits[i];

			if (pUnit->getProfession() == NO_PROFESSION)
			{
				stuckUnits.push_back(pUnit);
			}
		}

		// At least 1 pop must remain
		if (stuckUnits.size() > 0 && (m_aPopulationUnits.size() - stuckUnits.size() > 0))
		{
			// The AI will reassign its colonist the next turn so only trigger this (i.e. warn)
			// human players
			FAssertMsg(false, "Citizen in colony stuck with NO_PROFESSION");

			const ProfessionTypes eDefaultProfession = GC.getCivilizationInfo(GET_PLAYER(getOwnerINLINE()).getCivilizationType()).getDefaultProfession();

			for (unsigned int i = 0; i < stuckUnits.size(); ++i)
			{
				removePopulationUnit(CREATE_ASSERT_DATA, stuckUnits[i], false, eDefaultProfession);
			}
		}
	}
	// Fixing stuck units in colonies - Nightinggale - End

	doOppressometerDecay();
	doOppressometerGrowth();

	// ONEVENT - Do turn
	gDLL->getEventReporterIFace()->cityDoTurn(this, getOwnerINLINE());

	OOS_LOG("CvCity::doTurn end", getID());
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
	FOREACH(CityPlot)
	{
		CvPlot* const pLoopPlot = getCityIndexPlot(eLoopCityPlot);

		if (pLoopPlot != NULL)
		{
			pLoopPlot->updateShowCitySymbols();
		}
	}
}


void CvCity::updateYield()
{
	FOREACH(CityPlot)
	{
		CvPlot* pLoopPlot = getCityIndexPlot(eLoopCityPlot);

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
	GET_PLAYER(getOwnerINLINE()).createGreatGeneral(eGreatGeneralUnit, bIncrementExperience, coord());
}

// R&R, ray, Great Admirals - START
void CvCity::createGreatAdmiral(UnitTypes eGreatAdmiralUnit, bool bIncrementExperience)
{
	GET_PLAYER(getOwnerINLINE()).createGreatAdmiral(eGreatAdmiralUnit, bIncrementExperience, coord());
}
// R&R, ray, Great Admirals - END

// WTP, ray, Lieutenants and Captains - START
void CvCity::createBraveLieutenant(UnitTypes eBraveLieutenantUnit)
{
	GET_PLAYER(getOwnerINLINE()).createBraveLieutenant(eBraveLieutenantUnit, coord());
}

void CvCity::createCapableCaptain(UnitTypes eCapableCaptainUnit)
{
	GET_PLAYER(getOwnerINLINE()).createCapableCaptain(eCapableCaptainUnit, coord());
}

// WTP, ray, Lieutenants and Captains - END

void CvCity::doTask(TaskTypes eTask, int iData1, int iData2, bool bOption, bool bAlt, bool bShift, bool bCtrl)
{
	switch (eTask)
	{
	case TASK_RAZE:
		GET_PLAYER(getOwnerINLINE()).raze(this);
		break;

	case TASK_GIFT:
		ejectTeachUnits();
		ejectMissionary();
		ejectTrader();
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
		ejectMissionary();
		ejectTrader();
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
		setRallyPlot(GC.getMap().plotINLINE(iData1, iData2));
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
			NetworkDataTradeRouteInts buffer(iData2);
			setImportsLimit ((YieldTypes) iData1, buffer.iImportLimitLevel);
			setMaintainLevel((YieldTypes) iData1, buffer.iMaintainLevel);
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


CvPlot* CvCity::getCityIndexPlot(CityPlotTypes eCityPlot) const
{
	return plotCity(getX_INLINE(), getY_INLINE(), (int)eCityPlot);
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

	CvUnit* pUnit = getUnitWorkingPlot((CityPlotTypes)iPlotIndex);
	if (pUnit != NULL)
	{
		CvPlot* pPlot = getCityIndexPlot((CityPlotTypes)iPlotIndex);

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

	FOREACH(CityPlot)
	{
		if (eLoopCityPlot != CITY_HOME_PLOT)
		{
			if (!isPlotProducingYields(eLoopCityPlot))
			{
				CvPlot* pLoopPlot = getCityIndexPlot(eLoopCityPlot);

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

	pPlot = getCityIndexPlot((CityPlotTypes)iIndex);

	if (pPlot != NULL)
	{
		pPlot->setWorkingCityOverride(NULL);
	}
}


int CvCity::countNumImprovedPlots(ImprovementTypes eImprovement, bool bPotential) const
{
	int iCount = 0;

	FOREACH(CityPlot)
	{
		const CvPlot* const pLoopPlot = getCityIndexPlot(eLoopCityPlot);

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
	int iCount = 0;

	FOREACH(CityPlot)
	{
		const CvPlot* const pLoopPlot = getCityIndexPlot(eLoopCityPlot);

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

	FOREACH(CityPlot)
	{
		CvPlot* pLoopPlot = getCityIndexPlot(eLoopCityPlot);

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
	// WTP, ray, refactored according to advice of Nightinggale
	for (BuildingTypes eBuilding = FIRST_BUILDING; eBuilding < NUM_BUILDING_TYPES; ++eBuilding)
	{
		const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
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
			// WTP, ray, refactored according to advice of Nightinggale
			for (BuildingTypes eBuilding = FIRST_BUILDING; eBuilding < NUM_BUILDING_TYPES; ++eBuilding)
			{
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

	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
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


int CvCity::findBaseYieldRateRank(YieldTypes eYield) const //note this function is not const
{
	if (!m_em_bBaseYieldRankValid.get(eYield))
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

		m_em_bBaseYieldRankValid.set(eYield, true);
		m_em_iBaseYieldRank.set(eYield,iRank);
	}

	return m_em_iBaseYieldRank.get(eYield);
}


int CvCity::findYieldRateRank(YieldTypes eYield) const  //note this function is not const
{
	if (!m_em_bYieldRankValid.get(eYield))
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

		m_em_bYieldRankValid.set(eYield, true);
		m_em_iYieldRank.set(eYield, iRank);
	}

	return m_em_iYieldRank.get(eYield);
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
		CyCity* const pyCity = new CyCity(const_cast<CvCity*>(this));
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
		CyCity* const pyCity = new CyCity(const_cast<CvCity*>(this));
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
		CyCity* const pyCity = new CyCity(const_cast<CvCity*>(this));
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

	//WTP, Nightinggale - Terrain locator - start
	const CvPlot* pPlot = plot();
	//WTP, Nightinggale - Terrain locator - end

	// TAC - AI Buildings - koma13 - START
	if (!isHuman())
	{
		//WTP, Nightinggale - Terrain locator - start
		if (!pPlot->hasNearbyPlotWith(kBuilding.getAIRequiredCatchmentAreaTerrains(), CITY_PLOTS_RADIUS))
		{
			return false;
		}
		if (!pPlot->hasNearbyPlotWith(kBuilding.getAIRequiredCatchmentAreaFeatures(), CITY_PLOTS_RADIUS))
		{
			return false;
		}
		if (!pPlot->hasNearbyPlotWith(kBuilding.AI_getRequiredCatchmentAreaPlotTypes(), CITY_PLOTS_RADIUS))
		{
			return false;
		}
		//WTP, Nightinggale - Terrain locator - end

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
			// WTP, fixed a small issue with Lakes
			// Terrains adjcancet already control if harbour buildings can be built - the size check has become unnecessary
			// if (isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()) && !plot()->isEuropeAccessable())
			if (!plot()->isEuropeAccessable())
			{
				for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
				{
					UnitTypes eLoopUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI)));
					if (eLoopUnit != NO_UNIT)
					{
						const CvUnitInfo& kUnit = GC.getUnitInfo(eLoopUnit);
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

	//WTP, Nightinggale - Terrain locator - start
	if (!pPlot->hasNearbyPlotWith(kBuilding.getRequiredAdjacentTerrains()))
	{
		return false;
	}
	if (!pPlot->hasNearbyPlotWith(kBuilding.getRequiredCatchmentAreaTerrains(), CITY_PLOTS_RADIUS))
	{
		return false;
	}
	if (!pPlot->hasNearbyPlotWith(kBuilding.getRequiredCatchmentAreaFeatures(), CITY_PLOTS_RADIUS))
	{
		return false;
	}
	//WTP, Nightinggale - Terrain locator - end

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

	if (!isValidBuildingLocation(kBuilding))
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
		CyCity* const pyCity = new CyCity(const_cast<CvCity*>(this));
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
		return (pOrderNode->m_data.getType() == ORDER_TRAIN);
	}

	return false;
}


bool CvCity::isProductionBuilding() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		return (pOrderNode->m_data.getType() == ORDER_CONSTRUCT);
	}

	return false;
}

bool CvCity::isProductionConvince() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		return (pOrderNode->m_data.getType() == ORDER_CONVINCE);
	}

	return false;
}


bool CvCity::canContinueProduction(OrderData order) const
{
	switch (order.getType())
	{
	case ORDER_TRAIN:
		return canTrain(order.unit(), true);
		break;

	case ORDER_CONSTRUCT:
		return canConstruct(order.building(), true);
		break;

	case ORDER_CONVINCE:
		return canConvince(order.fatherpoint(), true);
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
		switch (pOrderNode->m_data.getType())
		{
		case ORDER_TRAIN:
			return pOrderNode->m_data.unit();
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
		switch (pOrderNode->m_data.getType())
		{
		case ORDER_TRAIN:
			return pOrderNode->m_data.unitAI();
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
		switch (pOrderNode->m_data.getType())
		{
		case ORDER_TRAIN:
		case ORDER_CONVINCE:
			break;

		case ORDER_CONSTRUCT:
			return pOrderNode->m_data.building();
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
		switch (pOrderNode->m_data.getType())
		{
		case ORDER_TRAIN:
		case ORDER_CONSTRUCT:
			break;

		case ORDER_CONVINCE:
			return pOrderNode->m_data.fatherpoint();
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
		switch (pOrderNode->m_data.getType())
		{
		case ORDER_TRAIN:
			return &GC.getUnitInfo(pOrderNode->m_data.unit());
			break;

		case ORDER_CONSTRUCT:
			return &GC.getBuildingInfo(pOrderNode->m_data.building());
			break;

		case ORDER_CONVINCE: // Lumber Mill is building Founding Father Points -- DO NOT TOUCH
			return &GC.getFatherPointInfo(pOrderNode->m_data.fatherpoint());
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
		switch (pOrderNode->m_data.getType())
		{
		case ORDER_TRAIN:
			return getProductionTurnsLeft(pOrderNode->m_data.unit(), 0);
			break;

		case ORDER_CONSTRUCT:
			return getProductionTurnsLeft(pOrderNode->m_data.building(), 0);
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
		if (pOrderNode->m_data.getType() == ORDER_TRAIN)
		{
			if (pOrderNode->m_data.unit() == eUnit)
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
		if (pOrderNode->m_data.getType() == ORDER_CONSTRUCT)
		{
			if (pOrderNode->m_data.building() == eBuilding)
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
		if (pOrderNode->m_data.getType() == ORDER_TRAIN)
		{
			if (pOrderNode->m_data.unitAI() == eUnitAI)
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
		switch (pOrderNode->m_data.getType())
		{
		case ORDER_TRAIN:
			return getUnitProduction(pOrderNode->m_data.unit());
			break;

		case ORDER_CONSTRUCT:
			return getBuildingProduction(pOrderNode->m_data.building());
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
		switch (pOrderNode->m_data.getType())
		{
		case ORDER_TRAIN:
			return getYieldProductionNeeded(pOrderNode->m_data.unit(), eYield);
			break;

		case ORDER_CONSTRUCT:
			return getYieldProductionNeeded(pOrderNode->m_data.building(), eYield);
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
		switch (pOrderNode->m_data.getType())
		{
		case ORDER_TRAIN:
			return getProductionTurnsLeft(pOrderNode->m_data.unit(), 0);
			break;

		case ORDER_CONSTRUCT:
			return getProductionTurnsLeft(pOrderNode->m_data.building(), 0);
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
		switch (pOrderNode->m_data.getType())
		{
		case ORDER_TRAIN:
			return getProductionModifier(pOrderNode->m_data.unit());
			break;

		case ORDER_CONSTRUCT:
			return getProductionModifier(pOrderNode->m_data.building());
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
		for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
		{
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

		for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
		{
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
#if 0
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
#endif
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

	for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
	{
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
	OOS_LOG_3("Hurry", CvString(getName()).c_str(), getID());
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

	if (gCityLogLevel >= 2) // BETTER_BTS_AI_MOD, AI logging, 10/02/09, jdog5000
	{
		CvWStringBuffer szBuffer; CvWString szString;
		if (isProductionUnit())
			szString = GC.getUnitInfo(getProductionUnit()).getDescription();
		else if (isProductionBuilding())
			szString = GC.getBuildingInfo(getProductionBuilding()).getDescription();
		else if (isProductionConvince())
			szString = GC.getFatherPointInfo(getProductionFatherPoint()).getDescription();
		logBBAI(" Player %S City %S hurrying production of %S at cost of %d gold", GET_PLAYER(getOwnerINLINE()).getCivilizationDescription(),
			getName().GetCString(), szString.GetCString(), iHurryGold);
	}
}


void CvCity::processBuilding(BuildingTypes eBuilding, int iChange)
{
	FAssertMsg(iChange == 1 || iChange == -1, "The value of iChange has to be either 1 or -1.")

	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	//ray, removing hardcoded Roads for Buildings - START
	const RouteTypes eRouteTypeCreated = (RouteTypes)kBuilding.getRouteTypeCreated();
	if (eRouteTypeCreated > 0) //
	{
		CvPlot* pPlot = plot();
		if (pPlot->getRouteType() < eRouteTypeCreated)
		{
			pPlot->setRouteType(eRouteTypeCreated);
		}
	}
	//ray, removing hardcoded Roads for Buildings - END

	if (kBuilding.getFreePromotion() != NO_PROMOTION)
	{
		changeFreePromotionCount(((PromotionTypes)kBuilding.getFreePromotion()), iChange);
	}
	changeFreeExperience(kBuilding.getFreeExperience() * iChange);
	changeMaxFoodKeptPercent(kBuilding.getFoodKept() * iChange);
	changeHealRate(kBuilding.getHealRateChange() * iChange);
	changeMilitaryProductionModifier(kBuilding.getMilitaryProductionModifier() * iChange);
	changeWorksWaterCount(kBuilding.isWorksWater() ? iChange : 0);
	for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
	{
		changeLandPlotYield    (eYield, (kBuilding.getLandPlotYieldChange(eYield)  * iChange)); // R&R, ray, Landplot Yields
		changeSeaPlotYield     (eYield, (kBuilding.getSeaPlotYieldChange(eYield)   * iChange));
		changeRiverPlotYield   (eYield, (kBuilding.getRiverPlotYieldChange(eYield) * iChange));
		changeYieldRateModifier(eYield, (kBuilding.getYieldModifier(eYield)        * iChange));
	}
	setYieldRateDirty();
	for (UnitCombatTypes eUnitCombat = FIRST_UNITCOMBAT; eUnitCombat < NUM_UNITCOMBAT_TYPES; ++eUnitCombat)
	{
		changeUnitCombatFreeExperience(eUnitCombat, kBuilding.getUnitCombatFreeExperience(eUnitCombat) * iChange);
	}
	for (DomainTypes eDomain = FIRST_DOMAIN; eDomain < NUM_DOMAIN_TYPES; ++eDomain)
	{
		changeDomainFreeExperience(eDomain, kBuilding.getDomainFreeExperience(eDomain) * iChange);
		changeDomainProductionModifier(eDomain, kBuilding.getDomainProductionModifier(eDomain) * iChange);
	}
	OOS_LOG("Process Building", getTypeStr(eBuilding));
	GET_PLAYER(getOwnerINLINE()).changeAssets(kBuilding.getAssetValue() * iChange);
	area()->changePower(getOwnerINLINE(), (kBuilding.getPowerValue() * iChange));
	GET_PLAYER(getOwnerINLINE()).changePower(kBuilding.getPowerValue() * iChange);
	changeBuildingDefense(kBuilding.getDefenseModifier() * iChange);
	changeBuildingBombardDefense(kBuilding.getBombardDefenseModifier() * iChange);
	GET_TEAM(getTeam()).changeBuildingClassCount((BuildingClassTypes)kBuilding.getBuildingClassType(), iChange);
	GET_PLAYER(getOwnerINLINE()).changeBuildingClassCount((BuildingClassTypes)kBuilding.getBuildingClassType(), iChange);
	setLayoutDirty(true);

	changeCityBarracksSpace(kBuilding.getMaxBarracksSpaceProvided() * iChange);
	changeCityHarbourSpace (kBuilding.getMaxHarbourSpaceProvided () * iChange);
}

void CvCity::setBarrackHarbourCache()
{
	//WTP, ray, edited by aemon, new Harbor & Barracks System - START
	m_iCityHarbourSpace = GLOBAL_DEFINE_BASE_HARBOUR_SPACES_WITHOUT_BUILDINGS;
	m_iCityBarracksSpace = GLOBAL_DEFINE_BASE_BARRACKS_SPACES_WITHOUT_BUILDINGS;
	for (BuildingTypes eBuilding = FIRST_BUILDING; eBuilding < NUM_BUILDING_TYPES; ++eBuilding)
	{
		if (isHasBuilding(eBuilding))
		{
			const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
			changeCityBarracksSpace(kBuilding.getMaxBarracksSpaceProvided());
			changeCityHarbourSpace(kBuilding.getMaxHarbourSpaceProvided());
		}
	}
	//WTP, ray, edited by aemon, new Harbor & Barracks System - END
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
	// for Natives this is not used
	if (isNative())
	{
		return GET_PLAYER(getOwnerINLINE()).getArtStyleType();
	}

	// WTP, ray, merged Captured City Art from DPII - START
	if (GLOBAL_DEFINE_FOUNDER_CITY_ART > 0)
	{

		if (getOriginalOwner() != NO_PLAYER)
		{
			return GET_PLAYER(getOriginalOwner()).getArtStyleType();
		}
	}

	if (GLOBAL_DEFINE_CULTURE_CITY_ART > 0)
	{
		PlayerTypes eHighestCulturePlayer;
		eHighestCulturePlayer = findHighestCulture();
		return GET_PLAYER(eHighestCulturePlayer).getArtStyleType();
	}
	// WTP, ray, merged Captured City Art from DPII - END

	// old code
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
	if (SAVEGAME_IS_LOADING)
	{
		// when reloading a savegame from the ingame menu (not main menu), the exe might call this while loading
		// the main issue is that there exist a time after the map is cleared, but the new is not loaded yet
		// during this time the cities aren't always cleared yet and then this function is called
		// when this is called with no map loaded yet, the call to CvPlot will cause a crash
		// to get around this, just tell the exe nothing is visible until the savegame is done loading
		return false;
	}
	else
	{
		return plot()->isVisible(eTeam, bDebug);
	}
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
	return ((getPopulation() + iExtra) * GLOBAL_DEFINE_FOOD_CONSUMPTION_PER_POPULATION);
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
	// R&R, ray, Health
	// WTP, ray, Happiness - START
	int iHealthModifier = getCityHealth();
	int iCityModifer = getCityHappiness() - getCityUnHappiness();
	int iTotalModifier = iHealthModifier + iCityModifer;

	// WTP, ray, for safety
	if (iTotalModifier > 50)
	{
		iTotalModifier = 50;
	}

	if (iTotalModifier < -50)
	{
		iTotalModifier = -50;
	}

	return ((GET_PLAYER(getOwnerINLINE()).getGrowthThreshold(getPopulation()) * (100 - iTotalModifier)) / 100);
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
		switch (pOrderNode->m_data.getType())
		{
		case ORDER_TRAIN:
			iModifier = getHurryCostModifier(pOrderNode->m_data.unit(), bIgnoreNew);
			break;

		case ORDER_CONSTRUCT:
			iModifier = getHurryCostModifier(pOrderNode->m_data.building(), bIgnoreNew);
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
		for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
		{
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

CultureLevelTypes CvCity::cultureDistance(int iDX, int iDY) const
{
	return static_cast<CultureLevelTypes>(std::max(1, plotDistance(0, 0, iDX, iDY)));
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
		const CvUnitInfo& kUnit = GC.getUnitInfo(pUnit->getUnitType());
		iExtra += kUnit.getYieldChange(eYieldProduced);
	}

	int iProfessionOutput = 0;
	// WTP, ray, refactored according to advice of Nightinggale
	for (BuildingTypes eBuilding = FIRST_BUILDING; eBuilding < NUM_BUILDING_TYPES; ++eBuilding)
	{
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

	// WTP, ray, fix for Milk producing Cattle from thin air - START
	// case for e.g. Milk
	YieldTypes eYieldProduced2 = (YieldTypes) kProfessionInfo.getYieldsProduced(1);
	YieldTypes eFirstYieldConsumed = (YieldTypes) kProfessionInfo.getYieldsConsumed(0);
	if (eYieldProduced2 != NO_YIELD && eYieldProduced2 == eFirstYieldConsumed)
	{
		iProfessionOutput = std::min(iProfessionOutput, getYieldStored(eFirstYieldConsumed));
	}
	// WTP, ray, fix for Milk producing Cattle from thin air - END

	return iProfessionOutput;
}

int CvCity::getProfessionInput(ProfessionTypes eProfession, const CvUnit* pUnit) const
{
	FAssert(NO_PROFESSION != eProfession);
	if (NO_PROFESSION == eProfession)
	{
		return 0;
	}

	const CvProfessionInfo& kProfessionInfo = GC.getProfessionInfo(eProfession);
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

	const CvProfessionInfo& kProfessionInfo = GC.getProfessionInfo(eProfession);
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
				// WTP, ray, refactored according to advice of Nightinggale
				for (BuildingTypes eBuilding = FIRST_BUILDING; eBuilding < NUM_BUILDING_TYPES; ++eBuilding)
				{
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
	return m_coord.x();
}


int CvCity::getY() const
{
	return m_coord.y();
}


bool CvCity::at(int iX,  int iY) const
{
	return ((getX_INLINE() == iX) && (getY_INLINE() == iY));
}


bool CvCity::at(Coordinates coord) const
{
	return coord == m_coord;
}


bool CvCity::at(const CvPlot* pPlot) const
{
	return (plot() == pPlot);
}


CvPlot* CvCity::plot() const
{
	return GC.getMap().plotSoren(coord());
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
	return GC.getMap().plotSoren(m_rallyCoordinates);
}


void CvCity::setRallyPlot(CvPlot* pPlot)
{
	if (getRallyPlot() != pPlot)
	{
		if (pPlot != NULL)
		{
			m_rallyCoordinates = pPlot->coord();
		}
		else
		{
			m_rallyCoordinates.resetInvalid();
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

		GC.getMap().updateWorkingCity();
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
	OOS_LOG_3("Set population of city", CvString(getName()).c_str(), iNewValue);
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
	//WTP, ray, make real Population balanceable in XML
	long iRealPopulationMultiplier = (long) GC.getDefineINT("REAL_POPULATION_MULTIPLIER");

	return (getPopulation() * iRealPopulationMultiplier);
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


void CvCity::setFood(int iNewValue, bool bAllowNegative)
{
	setYieldStored(YIELD_FOOD, iNewValue);
	FAssert(bAllowNegative || getYieldStored(YIELD_FOOD) >= 0);
}


void CvCity::changeFood(int iChange, bool bAllowNegative)
{
	setFood(getFood() + iChange, bAllowNegative);
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
	// WTP, ray, Improvements give Bonus to their City - START
	// return (getBuildingDefense() + GET_PLAYER(getOwnerINLINE()).getCityDefenseModifier());
	int iHillModifier = 0;
	if (plot()->isHills())
	{
		iHillModifier = GC.getDefineINT("CITY_DEFENSE_HILL_BONUS");
	}
	return (getBuildingDefense() + GET_PLAYER(getOwnerINLINE()).getCityDefenseModifier() + getFortDefenseBonusForCity() + iHillModifier);
	// WTP, ray, Improvements give Bonus to their City - END
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
	if (getOccupationTimer() != iNewValue)
	{
		const bool bOldOccupation = isOccupation();

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
	const CultureLevelTypes eOldValue = getCultureLevel();

	if (eOldValue != eNewValue)
	{
		m_eCultureLevel = eNewValue;

		if (eOldValue != NO_CULTURELEVEL)
		{
			for (int iDX = -eOldValue; iDX <= eOldValue; iDX++)
			{
				for (int iDY = -eOldValue; iDY <= eOldValue; iDY++)
				{
					const CultureLevelTypes eCultureRange = cultureDistance(iDX, iDY);

					if (eCultureRange > getCultureLevel())
					{
						if (eCultureRange <= eOldValue)
						{
							FAssert(eCultureRange <= GC.getNumCultureLevelInfos());

							CvPlot*	const pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

							if (pLoopPlot != NULL)
							{
								pLoopPlot->changeCultureRangeCities(getOwnerINLINE(), eCultureRange, -1);
							}
						}
					}
				}
			}
		}

		if (getCultureLevel() != NO_CULTURELEVEL)
		{
			for (int iDX = -getCultureLevel(); iDX <= getCultureLevel(); iDX++)
			{
				for (int iDY = -getCultureLevel(); iDY <= getCultureLevel(); iDY++)
				{
					const CultureLevelTypes eCultureRange = cultureDistance(iDX, iDY);

					if (eCultureRange > eOldValue)
					{
						if (eCultureRange <= getCultureLevel())
						{
							FAssert(eCultureRange <= GC.getNumCultureLevelInfos());

							CvPlot* const pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

							if (pLoopPlot != NULL)
							{
								pLoopPlot->changeCultureRangeCities(getOwnerINLINE(), eCultureRange, 1);
							}
						}
					}
				}
			}
		}

		//update all affected plots
		int iRange = std::max(getCultureLevel(), eOldValue) + GC.getDefineINT("BUY_PLOT_CULTURE_RANGE");
		for (int iDX = -iRange; iDX <= iRange; iDX++)
		{
			for (int iDY = -iRange; iDY <= iRange; iDY++)
			{
				CvPlot* const pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

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
				CvWString szBuffer;
				szBuffer = gDLL->getText("TXT_KEY_MISC_BORDERS_EXPANDED", getNameKey());
				gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_CULTUREEXPANDS", MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(eCultureYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), true, true);

				if (getCultureLevel() == (GC.getNumCultureLevelInfos() - 1))
				{
					for (int iI = 0; iI < MAX_PLAYERS; iI++)
					{
						if (GET_PLAYER((PlayerTypes)iI).isAlive())
						{
							if (isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
							{
								szBuffer = gDLL->getText("TXT_KEY_MISC_CULTURE_LEVEL", getNameKey(), GC.getCultureLevelInfo(getCultureLevel()).getTextKeyWide());
								gDLL->UI().addPlayerMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_CULTURELEVEL", MESSAGE_TYPE_MAJOR_EVENT, GC.getYieldInfo(eCultureYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), true, true);
							}
							else
							{
								szBuffer = gDLL->getText("TXT_KEY_MISC_CULTURE_LEVEL_UNKNOWN", GC.getCultureLevelInfo(getCultureLevel()).getTextKeyWide());
								gDLL->UI().addPlayerMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CULTURELEVEL", MESSAGE_TYPE_MAJOR_EVENT, GC.getYieldInfo(eCultureYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
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

	// WTP, ray, removed bad bugfix try of vetiarvind which actually fixed nothing and caused a new bug
	if (!isOccupation())
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
	FAssert(validEnumRange(eIndex));
	return m_em_iLandPlotYield.get(eIndex);
}


void CvCity::changeLandPlotYield(YieldTypes eIndex, int iChange)
{
	FAssert(validEnumRange(eIndex));

	if (iChange != 0)
	{
		m_em_iLandPlotYield.add(eIndex, iChange);
		m_em_iLandPlotYield.set(eIndex, std::max(m_em_iLandPlotYield.get(eIndex), 0));
		updateYield();
	}
}
// R&R, ray, Landplot Yields -END

int CvCity::getSeaPlotYield(YieldTypes eIndex) const
{
	FAssert(validEnumRange(eIndex));
	return m_em_iSeaPlotYield.get(eIndex);
}


void CvCity::changeSeaPlotYield(YieldTypes eIndex, int iChange)
{
	FAssert(validEnumRange(eIndex));

	if (iChange != 0)
	{
		m_em_iSeaPlotYield.add(eIndex, iChange);
		m_em_iSeaPlotYield.set(eIndex, std::max(m_em_iSeaPlotYield.get(eIndex), 0));
		updateYield();
	}
}


int CvCity::getRiverPlotYield(YieldTypes eIndex) const
{
	FAssert(validEnumRange(eIndex));
	return m_em_iRiverPlotYield.get(eIndex);
}


void CvCity::changeRiverPlotYield(YieldTypes eIndex, int iChange)
{
	FAssert(validEnumRange(eIndex));

	if (iChange != 0)
	{
		m_em_iRiverPlotYield.add(eIndex, iChange);
		m_em_iRiverPlotYield.set(eIndex, std::max(m_em_iRiverPlotYield.get(eIndex), 0));
		updateYield();
	}
}

int CvCity::getRawYieldProduced(YieldTypes eYieldType) const
{
	// R&R, ray, safety check for negative Yield Modifiers from Health - START
	int iRawYield = yields().getBaseRawYieldProduced(eYieldType);

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
		iYieldConsumed = getPopulation() * GLOBAL_DEFINE_FOOD_CONSUMPTION_PER_POPULATION;
	}

	for (int i = 0; i < getPopulation(); ++i)
	{
		CvUnit* pUnit = getPopulationUnitByIndex(i);
		if (NULL != pUnit)
		{
			ProfessionTypes eProfession = pUnit->getProfession();
			if (NO_PROFESSION != eProfession)
			{
				const CvProfessionInfo& kProfessionInfo = GC.getProfessionInfo(eProfession);
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

	// WTP, ray, trying to fix Rebel Rate Modifier on Happiness for Balancing
	// Updated to exclude happiness internally to match GUI - Nightinggale
	if (eIndex != YIELD_HAPPINESS && eIndex != YIELD_UNHAPPINESS && eIndex != YIELD_LAW && eIndex != YIELD_CRIME)
	{
		iModifier += getRebelPercent() * GC.getMAX_REBEL_YIELD_MODIFIER() / 100;
	}

	// R&R, ray, Health
	iModifier += getCityHealth(); // negative CityHealth is possible

	if (isCapital())
	{
		iModifier += GET_PLAYER(getOwnerINLINE()).getCapitalYieldRateModifier(eIndex);
	}

	iModifier += iExtra;

	// WTP, ray, Improvements give Bonus to their City - START
	if (eIndex == YIELD_CROSSES)
	{
		iModifier += getMonasteryCrossBonusForCity();
	}
	// WTP, ray, Improvements give Bonus to their City - END

	// WTP, ray, Improvements give Bonus to their City - PART 2 - START
	if (eIndex == YIELD_FOOD)
	{
		iModifier += getImprovementFoodModifierForCity();
	}

	if (eIndex == YIELD_HAMMERS)
	{
		iModifier += getImprovementHammersModifierForCity();
	}

	if (eIndex == YIELD_TOOLS)
	{
		iModifier += getImprovementToolsModifierForCity();
	}
	// WTP, ray, Improvements give Bonus to their City - PART 2 - END

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
	FAssert(validEnumRange(eIndex));
	return m_em_iYieldRateModifier.get(eIndex);
}


void CvCity::changeYieldRateModifier(YieldTypes eIndex, int iChange)
{
	FAssert(validEnumRange(eIndex));

	if (iChange != 0)
	{
		m_em_iYieldRateModifier.add(eIndex, iChange);
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

	const CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);

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
				const CvBuildingInfo& kLoopBuilding = GC.getBuildingInfo(eLoopBuilding);
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
							const CvBuildingInfo& kLoopBuilding = GC.getBuildingInfo(eLoopBuilding);
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
	return m_em_iDomainFreeExperience.get(eIndex);
}


void CvCity::changeDomainFreeExperience(DomainTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex expected to be < NUM_DOMAIN_TYPES");
	m_em_iDomainFreeExperience.add(eIndex, iChange);
	FAssert(getDomainFreeExperience(eIndex) >= 0);
}


int CvCity::getDomainProductionModifier(DomainTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex expected to be < NUM_DOMAIN_TYPES");
	return m_em_iDomainProductionModifier.get(eIndex);
}


void CvCity::changeDomainProductionModifier(DomainTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex expected to be < NUM_DOMAIN_TYPES");
	m_em_iDomainProductionModifier.add(eIndex, iChange);
}


int CvCity::getCulture(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex expected to be < MAX_PLAYERS");
	return m_em_iCulture.get(eIndex);
}

int CvCity::countTotalCulture() const
{
	int iTotalCulture;

	iTotalCulture = 0;

	for(PlayerTypes eLoopPlayer = FIRST_PLAYER; eLoopPlayer < NUM_PLAYER_TYPES; eLoopPlayer++)
	{
		if (GET_PLAYER(eLoopPlayer).isAlive())
		{
			iTotalCulture += getCulture(eLoopPlayer);
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
		m_em_iCulture.set(eIndex, iNewValue);
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

int CvCity::getTotalYieldStored() const
{
	return m_iTotalYieldStored;
}

void CvCity::setTotalYieldStored(int iValue)
{
	m_iTotalYieldStored = iValue;
}

void CvCity::changeTotalYieldStored(int iChange)
{
	setTotalYieldStored(getTotalYieldStored() + iChange);
}

int CvCity::getYieldStored(YieldTypes eYield) const
{
	FAssert(validEnumRange(eYield));
	return m_em_iYieldStored.get(eYield);
}

void CvCity::setYieldStored(YieldTypes eYield, int iValue)
{
	FAssert(validEnumRange(eYield));
	FAssert(iValue >= 0 || eYield == YIELD_FOOD);

	if (iValue < 0 && eYield != YIELD_FOOD)
	{
		// this is such a critical bug that we want people to be alert to it even without having asserts enabled
		// this is the best chance we have of getting an autosave from the turn before it happened.
		//    Nightinggale
		CvString szDesc(gDLL->getText("TXT_KEY_ERROR_NEGATIVE_YIELD_STORAGE_DESC"));
		CvString szTitle(gDLL->getText("TXT_KEY_ERROR_NEGATIVE_YIELD_STORAGE_TITLE"));

		gDLL->MessageBox(szDesc.c_str(), szTitle.c_str());
	}

	int iChange = iValue - getYieldStored(eYield);
	if (iChange != 0)
	{
		// ray, making special storage capacity rules for Yields XML configurable
		if(!GC.getYieldInfo(eYield).isIgnoredForStorageCapacity() && GC.getYieldInfo(eYield).isCargo())
		{
			changeTotalYieldStored(iChange);
		}
		m_em_iYieldStored.set(eYield, iValue);

		if (!AI_isWorkforceHack())
		{
			checkCompletedBuilds(eYield, iChange);
		}

		OOS_LOG_3("Set yield stored", getTypeStr(eYield), iChange);
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
			FOREACH(CityPlot)
			{
				CvPlot* pLoopPlot = getCityIndexPlot(eLoopCityPlot);

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
	FAssert(validEnumRange(eYield));
	return m_em_iYieldRushed.get(eYield);
}

void CvCity::changeYieldRushed(YieldTypes eYield, int iChange)
{
	FAssert(validEnumRange(eYield));
	m_em_iYieldRushed.add(eYield, iChange);
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
		aiProducedYields[iYield] = yields().getBaseRawYieldProduced(eYield);
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
							gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(eYieldProduced).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), true, true);
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
				gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(eYieldConsumed).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), true, true);
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
				gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(eYieldConsumed).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), true, true);
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
					gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(eYieldConsumed).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), true, true);
				}
			}
		}
	}
	// TAC - Messages - Ray - END
}
// R&R, ray , MYCP partially based on code of Aymerick - END

int CvCity::calculateNetYield(YieldTypes eYield) const
{
	FAssert(validEnumRange(eYield));
	int aiYields[NUM_YIELD_TYPES];
	calculateNetYields(aiYields);
	return aiYields[eYield];
}

int CvCity::calculateActualYieldProduced(YieldTypes eYield) const
{
	FAssert(validEnumRange(eYield));
	int aiYields[NUM_YIELD_TYPES];
	int aiYieldsProduced[NUM_YIELD_TYPES];
	calculateNetYields(aiYields, aiYieldsProduced);
	return aiYieldsProduced[eYield];
}

int CvCity::calculateActualYieldConsumed(YieldTypes eYield) const
{
	FAssert(validEnumRange(eYield));
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
	const CvBuildingInfo &refBuildingInfo = GC.getBuildingInfo(eBuilding);

	//Use values for sell percantage, boycott ignoring and unlocking the trade settings, if they are better than the current ones.

	int i_building_StorageLossSellPercentage = refBuildingInfo.getStorageLossSellPercentage();
	if (i_building_StorageLossSellPercentage > m_iStorageLossSellPercentage)
	{
		m_iStorageLossSellPercentage = i_building_StorageLossSellPercentage;
	}

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

void CvCity::pushOrderInternal(BuildingTypes eBuilding)
{
	OrderData order(ORDER_CONSTRUCT);
	order.building() = eBuilding;

	if (gCityLogLevel >= 2)
	{
		logBBAI(" Player %S City %S considers hurrying of building %S", GET_PLAYER(getOwnerINLINE()).getCivilizationDescription(),
			getName().GetCString(), GC.getBuildingInfo(eBuilding).getDescription());
	}
	pushOrderInternal(order);
}

void CvCity::pushOrderInternal(UnitTypes eUnit)
{
	OrderData order(ORDER_TRAIN);
	order.unit() = eUnit;
	order.unitAI() = NO_UNITAI;

	if (gCityLogLevel >= 2)
	{
		logBBAI(" Player %S City %S considers hurrying of unit %S", GET_PLAYER(getOwnerINLINE()).getCivilizationDescription(),
			getName().GetCString(), GC.getUnitInfo(eUnit).getDescription());
	}
	pushOrderInternal(order);
}

void CvCity::pushOrderInternal(OrderData& order)
{
	order.bSave = false;
	m_orderQueue.insertAtBeginning(order);
	startHeadOrder();
}

void CvCity::popOrderInternal()
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode == NULL)
		return;

	m_orderQueue.deleteNode(pOrderNode);
	pOrderNode = NULL;
}

bool CvCity::isEverOwned(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex expected to be < MAX_PLAYERS");
	return m_em_bEverOwned.get(eIndex);
}


void CvCity::setEverOwned(PlayerTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex expected to be < MAX_PLAYERS");
	m_em_bEverOwned.set(eIndex, bNewValue);
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

		return m_em_bRevealed.get(eIndex);
	}
}


void CvCity::setRevealed(TeamTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex expected to be < MAX_TEAMS");

	if (isRevealed(eIndex, false) != bNewValue)
	{
		m_em_bRevealed.set(eIndex, bNewValue);

		updateVisibility();

		if (eIndex == GC.getGameINLINE().getActiveTeam())
		{
			FOREACH(CityPlot)
			{
				CvPlot* const pLoopPlot = getCityIndexPlot(eLoopCityPlot);

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
	gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, -1, szBuffer, coord(), ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), MESSAGE_TYPE_MAJOR_EVENT, NULL, NO_COLOR);

	szBuffer = gDLL->getText("TXT_KEY_MISC_CITY_IS_FOUNDED", getNameKey());
	GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_CITY_FOUNDED, getOwnerINLINE(), szBuffer, coord(), (ColorTypes)GC.getInfoTypeForString("COLOR_ALT_HIGHLIGHT_TEXT"));
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
	return m_em_iBuildingProduction.get(eIndex);
}


void CvCity::setBuildingProduction(BuildingTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");

	if (getBuildingProduction(eIndex) != iNewValue)
	{
		m_em_iBuildingProduction.set(eIndex, iNewValue);
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
	return m_em_iBuildingProductionTime.get(eIndex);
}


void CvCity::setBuildingProductionTime(BuildingTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");
	m_em_iBuildingProductionTime.set(eIndex, iNewValue);
	FAssert(getBuildingProductionTime(eIndex) >= 0);
}


void CvCity::changeBuildingProductionTime(BuildingTypes eIndex, int iChange)
{
	setBuildingProductionTime(eIndex, (getBuildingProductionTime(eIndex) + iChange));
}


PlayerTypes CvCity::getBuildingOriginalOwner(BuildingTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");
	return m_em_eBuildingOriginalOwner.get(eIndex);
}


int CvCity::getBuildingOriginalTime(BuildingTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex expected to be < GC.getNumBuildingInfos()");
	return m_em_iBuildingOriginalTime.get(eIndex);
}


int CvCity::getUnitProduction(UnitTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");
	return m_em_iUnitProduction.get(eIndex);
}


void CvCity::setUnitProduction(UnitTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");

	if (getUnitProduction(eIndex) != iNewValue)
	{
		m_em_iUnitProduction.set(eIndex, iNewValue);
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
	return m_em_iUnitProductionTime.get(eIndex);
}


void CvCity::setUnitProductionTime(UnitTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex expected to be < GC.getNumUnitInfos()");
	m_em_iUnitProductionTime.set(eIndex, iNewValue);
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
	return m_em_iUnitCombatFreeExperience.get(eIndex);
}


void CvCity::changeUnitCombatFreeExperience(UnitCombatTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumUnitCombatInfos(), "eIndex expected to be < GC.getNumUnitCombatInfos()");
	m_em_iUnitCombatFreeExperience.add(eIndex, iChange);
	FAssert(getUnitCombatFreeExperience(eIndex) >= 0);
}


int CvCity::getFreePromotionCount(PromotionTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumPromotionInfos(), "eIndex expected to be < GC.getNumPromotionInfos()");
	return m_em_iFreePromotionCount.get(eIndex);
}


bool CvCity::isFreePromotion(PromotionTypes eIndex) const
{
	return (getFreePromotionCount(eIndex) > 0);
}


void CvCity::changeFreePromotionCount(PromotionTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumPromotionInfos(), "eIndex expected to be < GC.getNumPromotionInfos()");
	m_em_iFreePromotionCount.add(eIndex, iChange);
	FAssert(getFreePromotionCount(eIndex) >= 0);
}


CvUnit* CvCity::getUnitWorkingPlot(CityPlotTypes ePlotIndex) const
{
	FAssertMsg(ePlotIndex >= 0, "iPlotIndex expected to be >= 0");
	FAssertMsg(ePlotIndex < NUM_CITY_PLOTS, "iPlotIndex expected to be < NUM_CITY_PLOTS");

	int iUnitId = m_em_iWorkingPlot.get(ePlotIndex);
	if (-1 != iUnitId)
	{
		return getPopulationUnitById(iUnitId);
	}

	return NULL;
}


bool CvCity::isPlotProducingYields(CityPlotTypes ePlotIndex) const
{
	FAssertMsg(ePlotIndex >= 0, "iPlotIndex expected to be >= 0");
	FAssertMsg(ePlotIndex < NUM_CITY_PLOTS, "iPlotIndex expected to be < NUM_CITY_PLOTS");

	if (CITY_HOME_PLOT == ePlotIndex)
	{
		return true;
	}

	if (-1 != m_em_iWorkingPlot.get(ePlotIndex))
	{
		return true;
	}

	return false;
}


bool CvCity::isPlotProducingYields(const CvPlot* pPlot) const
{
	CityPlotTypes ePlotIndex = (CityPlotTypes)getCityPlotIndex(pPlot);

	if (ePlotIndex != NO_CITY_PLOT)
	{
		return isPlotProducingYields(ePlotIndex);
	}

	return false;
}

bool CvCity::isUnitWorkingAnyPlot(const CvUnit* pUnit) const
{
	FOREACH(CityPlot)
	{
		int iUnitId = m_em_iWorkingPlot.get(eLoopCityPlot);
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
	CityPlotTypes eCityPlot = (CityPlotTypes)getCityPlotIndex(pPlot);

	if (eCityPlot != NO_CITY_PLOT)
	{
		return getUnitWorkingPlot(eCityPlot);
	}

	return NULL;
}


void CvCity::setUnitWorkingPlot(int iPlotIndex, int iUnitId)
{
	FAssertMsg(iPlotIndex >= 0, "iPlotIndex expected to be >= 0");
	FAssertMsg(iPlotIndex < NUM_CITY_PLOTS, "iPlotIndex expected to be < NUM_CITY_PLOTS");
	FAssertMsg(iUnitId >= 0, "iUnitId expected to be >= 0");
	FAssert(iPlotIndex != CITY_HOME_PLOT);

	if (isPlotProducingYields((CityPlotTypes)iPlotIndex))
	{
		FAssertMsg(false, "Trying to work a plot that's already being worked");
		return;
	}

	CvPlot* pPlot = getCityIndexPlot((CityPlotTypes)iPlotIndex);

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

		m_em_iWorkingPlot.set(iPlotIndex, iUnitId);

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

	if (!isPlotProducingYields((CityPlotTypes)iPlotIndex))
	{
		return;
	}

	CvPlot* pPlot = getCityIndexPlot((CityPlotTypes)iPlotIndex);
	if (pPlot != NULL)
	{
		m_em_iWorkingPlot.set(iPlotIndex, -1);

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

	const CityPlotTypes ePlotIndex = (CityPlotTypes)iPlotIndex;

	if (ePlotIndex == CITY_HOME_PLOT)
	{
		return;
	}

	CvPlot* pPlot = getCityIndexPlot(ePlotIndex);
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

		CvUnit* pUnitWorkingPlot = getUnitWorkingPlot(ePlotIndex);
		if (NULL != pUnitWorkingPlot)
		{
			clearUnitWorkingPlot(ePlotIndex);
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
		ProfessionTypes eBestProfession = NO_PROFESSION;

		if (pUnit != NULL)
		{
			pUnit->setColonistLocked(true);
			//code based on the one in setUnitWorkingPlot()
			FAssert(pUnit->isColonistLocked());
			if (pUnit->isColonistLocked())
			{
				//assign profession that produces yields
				int iBestYieldAmount = 0;
				for (ProfessionTypes eLoopProfession = FIRST_PROFESSION; eLoopProfession <NUM_PROFESSION_TYPES; ++eLoopProfession)
				{
					if (GET_PLAYER(getOwnerINLINE()).isProfessionValid(eLoopProfession, pUnit->getUnitType()) && pUnit->canHaveProfession(eLoopProfession, true))
					{
						if (GC.getProfessionInfo(eLoopProfession).getSpecialBuilding() == GC.getBuildingInfo(eBuilding).getSpecialBuildingType())
						{
							int iLoopYieldAmount = getProfessionOutput(eLoopProfession, pUnit);
							const CvProfessionInfo& professionInfo = GC.getProfessionInfo(eLoopProfession);
							// R&R, ray , MYCP partially based on code of Aymerick - START
							if (professionInfo.getYieldsConsumed(0) != NO_YIELD)
								// R&R, ray , MYCP partially based on code of Aymerick - END
							{
								// R&R, ray , MYCP partially based on code of Aymerick - START
								const int iUnproducedAmount = (getYieldStored((YieldTypes)professionInfo.getYieldsConsumed(0)) +
									getYieldRate((YieldTypes)professionInfo.getYieldsConsumed(0))) - getProfessionInput(eLoopProfession, pUnit);
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
							if (iLoopYieldAmount >= iBestYieldAmount)
							{
								eBestProfession = eLoopProfession;
								iBestYieldAmount = iLoopYieldAmount;
							}
						}
					}
				}
				//deleted assert here that no longer served a purpose
				if (eBestProfession != NO_PROFESSION)
				{
					pUnit->setProfession(eBestProfession);
				}
				else
				{
					pUnit->setColonistLocked(false);
				}
			}
		}
		if (isHuman() && bAskProfession && eBestProfession != NO_PROFESSION)
		{
			if (pUnit != NULL)
			{
				CvPopupInfo* const pInfo = new CvPopupInfo(BUTTONPOPUP_CHOOSE_PROFESSION, getID(), pUnit->getID(), 2);
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
						FAssert(!isPlotProducingYields((CityPlotTypes)iBestPlot));
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
			if (removePopulationUnit(CREATE_ASSERT_DATA, pUnit, false, GC.getCivilizationInfo(GET_PLAYER(getOwnerINLINE()).getCivilizationType()).getDefaultProfession()))
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

// WTP, ray, fix for SailTo - for the City - START
bool CvCity::isEuropeAccessable() const
{
	// trying to check the adjacent Plots of the City for Water that is accessible to Europe
	// however it should not be Large Rivers - at least for now
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		const CvPlot* const pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));
		if (pAdjacentPlot != NULL && pAdjacentPlot->isWater() && pAdjacentPlot->isEuropeAccessable() && (pAdjacentPlot->getTerrainType() == TERRAIN_COAST || pAdjacentPlot->getTerrainType() == TERRAIN_SHALLOW_COAST))
		{
			return true;
		}
	}
	return false;
}
// WTP, ray, fix for SailTo - for the City - END


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
	return m_em_bHasRealBuilding.get(eIndex);
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

		// WTP, ray, refactored according to advice of Nightinggale
		for (BuildingTypes eBuilding = FIRST_BUILDING; eBuilding < NUM_BUILDING_TYPES; ++eBuilding)
		{
			abOldBuildings[eBuilding] = isHasBuilding(eBuilding);
		}

		//Update the cities array for "real" buildings.
		m_em_bHasRealBuilding.set(eIndex, bNewValue);
		setYieldRateDirty();

		//Set values for player who built this building and game year depending on if it exists or not.
		if (isHasRealBuilding(eIndex))
		{
			m_em_eBuildingOriginalOwner.set(eIndex, eOriginalOwner);
			m_em_iBuildingOriginalTime.set(eIndex, iOriginalTime);
		}
		else
		{
			m_em_eBuildingOriginalOwner.set(eIndex, NO_PLAYER);
			m_em_iBuildingOriginalTime.set(eIndex, 0);
		}

		//Iterate over all the buildings in the game ...
		// WTP, ray, refactored according to advice of Nightinggale
		for (BuildingTypes eBuilding = FIRST_BUILDING; eBuilding < NUM_BUILDING_TYPES; ++eBuilding)
		{
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
	return m_em_bHasFreeBuilding.get(eIndex);
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
		// WTP, ray, refactored according to advice of Nightinggale
		for (BuildingTypes eBuilding = FIRST_BUILDING; eBuilding < NUM_BUILDING_TYPES; ++eBuilding)
		{
			abOldBuildings[eBuilding] = isHasBuilding(eBuilding);
		}

		//Update the cities array for "free" buildings.
		m_em_bHasFreeBuilding.set(eIndex, bNewValue);
		setYieldRateDirty();

		//Iterate over all the buildings in the game ...
		// WTP, ray, refactored according to advice of Nightinggale
		for (BuildingTypes eBuilding = FIRST_BUILDING; eBuilding < NUM_BUILDING_TYPES; ++eBuilding)
		{
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
	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eIndex);

	//Walk through all the possible buildings in the building slot of the given building ...
	//... and check if the given building is the building with the highest tier (SpecialBuildingPriority), built in that slot.
	BuildingTypes eNextBuilding = kBuilding.getIndexOf_NextBuildingType_In_SpecialBuilding();
	while (eNextBuilding != eIndex)
	{
		const CvBuildingInfo& kNextBuilding = GC.getBuildingInfo(eNextBuilding);
		if (isHasConceptualBuilding(eNextBuilding))
		{
			if (kBuilding.getSpecialBuildingPriority() < kNextBuilding.getSpecialBuildingPriority())
			{
				return false;
			}
		}

		eNextBuilding = kNextBuilding.getIndexOf_NextBuildingType_In_SpecialBuilding();
	}

	return true;
}

BuildingTypes CvCity::getDominantBuilding(SpecialBuildingTypes eSpecialBuilding) const
{
	// the the building present in the city, which has the highest special building priority.
	// relies on the <building,priority> InfoArray stored in CvSpecialBuildingInfo as this skips all the building info class lookup.
	if (eSpecialBuilding != NO_SPECIALBUILDING)
	{
		const InfoArray<BuildingTypes, int>& iaBuildings = GC.getSpecialBuildingInfo(eSpecialBuilding).getBuildings();

		int iBestPriority = -1;
		BuildingTypes eBestBuilding = NO_BUILDING;
		for (int i = 0; i < iaBuildings.getLength(); ++i)
		{
			const BuildingTypes eBuilding = iaBuildings.getBuilding(i);
			if (isHasConceptualBuilding(eBuilding))
			{
				const int iPriority = iaBuildings.getInt(i);
				if (iPriority > iBestPriority)
				{
					iBestPriority = iPriority;
					eBestBuilding = eBuilding;
				}
			}
		}
		return eBestBuilding;
	}
	return NO_BUILDING;
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

	OrderData order(eOrder);
	switch (order.getType())
	{
	case ORDER_TRAIN:
		order.unit() = static_cast<UnitTypes>(iData1);
		order.unitAI() = static_cast<UnitAITypes>(iData2);
		break;
	case ORDER_CONSTRUCT:
		order.building() = static_cast<BuildingTypes>(iData1);
		break;
	case ORDER_CONVINCE:
		order.fatherpoint() = static_cast<FatherPointTypes>(iData1);
		break;
	default:
		FAssertMsg(false, "unhandled case");
		return;
	}

	pushOrder(order, false, bAppend, bForce);
}

void CvCity::pushOrder(OrderData order, bool bPop, bool bAppend, bool bForce)
{
	if (bPop)
	{
		popOrder(0);
	}

	bool bValid = false;

	switch (order.getType())
	{
	case ORDER_TRAIN:
	{
		if (canTrain(order.unit()) || bForce)
		{
			if (order.unitAI() == NO_UNITAI)
			{
				order.unitAI() = (UnitAITypes)GC.getUnitInfo(order.unit()).getDefaultUnitAIType();
			}

			GET_PLAYER(getOwner()).changeUnitClassMaking(GC.getUnitInfo(order.unit()).getUnitClassType(), 1);
			area()->changeNumTrainAIUnits(getOwner(), order.unitAI(), 1);
			GET_PLAYER(getOwner()).AI_changeNumTrainAIUnits(order.unitAI(), 1);

			bValid = true;
			gDLL->getEventReporterIFace()->cityBuildingUnit(this, order.unit());

			if (gCityLogLevel >= 1)
			{
				CvWString szString;
				getUnitAIString(szString, order.unitAI());
				logBBAI(" Player %S City %S pushes production of unit %S with UNITAI %S", GET_PLAYER(getOwnerINLINE()).getCivilizationDescription(),
					getName().GetCString(), GC.getUnitInfo(order.unit()).getDescription(), szString.GetCString());
			}
		}
		break;
	}
	case ORDER_CONSTRUCT:
	{
		if (canConstruct(order.building()) || bForce)
		{
			GET_PLAYER(getOwnerINLINE()).changeBuildingClassMaking(((BuildingClassTypes)(GC.getBuildingInfo(order.building()).getBuildingClassType())), 1);

			bValid = true;

			gDLL->getEventReporterIFace()->cityBuildingBuilding(this, order.building());
			if (gCityLogLevel >= 1) logBBAI(" Player %S City %S pushes production of building %S", GET_PLAYER(getOwnerINLINE()).getCivilizationDescription(),
				getName().GetCString(), GC.getBuildingInfo(order.building()).getDescription());
		}
		break;
	}
	case ORDER_CONVINCE:
	{
		if (canConvince(order.fatherpoint()) || bForce)
		{
			bValid = true;
			if (gCityLogLevel >= 1) logBBAI(" Player % S City % S pushes convincing of type % S", GET_PLAYER(getOwnerINLINE()).getCivilizationDescription(),
				getName().GetCString(), GC.getFatherPointInfo(order.fatherpoint()).getDescription());
		}
		break;

	}
	default:
		FAssertMsg(false, "iOrder did not match a valid option");
		break;
	}

	if (!bValid)
	{
		return;
	}

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
	CvWString szBuffer;
	CvString szSound;
	bool bStart;
	bool bMessage;
	int iProductionNeeded;

	if (iNum == -1)
	{
		iNum = (getOrderQueueLength() - 1);
	}

	int iCount = 0;

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
		pushOrder(pOrderNode->m_data, false, true);
	}

	UnitTypes eTrainUnit = NO_UNIT;
	BuildingTypes eConstructBuilding = NO_BUILDING;

	switch (pOrderNode->m_data.getType())
	{
	case ORDER_TRAIN:
	{
		eTrainUnit = pOrderNode->m_data.unit();
		UnitAITypes eTrainAIUnit = pOrderNode->m_data.unitAI();
		FAssertMsg(eTrainUnit != NO_UNIT, "eTrainUnit is expected to be assigned a valid unit type");
		FAssertMsg(eTrainAIUnit != NO_UNITAI, "eTrainAIUnit is expected to be assigned a valid unit AI type");

		GET_PLAYER(getOwnerINLINE()).changeUnitClassMaking(GC.getUnitInfo(eTrainUnit).getUnitClassType(), -1);

		area()->changeNumTrainAIUnits(getOwnerINLINE(), eTrainAIUnit, -1);
		GET_PLAYER(getOwnerINLINE()).AI_changeNumTrainAIUnits(eTrainAIUnit, -1);

		if (bFinish)
		{
			m_bHasHurried = false;

			iProductionNeeded = getYieldProductionNeeded(eTrainUnit, YIELD_HAMMERS);

			const int iOverflow = std::max(0, getUnitProduction(eTrainUnit) - iProductionNeeded);
			changeOverflowProduction(iOverflow, getProductionModifier(eTrainUnit));
			setUnitProduction(eTrainUnit, 0);

			CvUnit* const pUnit = GET_PLAYER(getOwnerINLINE()).initUnit(eTrainUnit, GC.getUnitInfo(eTrainUnit).getDefaultProfession(), getX_INLINE(), getY_INLINE(), eTrainAIUnit);
			FAssertMsg(pUnit != NULL, "pUnit is expected to be assigned a valid unit object");

			pUnit->finishMoves();

			addProductionExperience(pUnit);

			CvPlot* const pRallyPlot = getRallyPlot();

			if (pRallyPlot != NULL)
			{
				pUnit->getGroup()->pushMission(MISSION_MOVE_TO, pRallyPlot->getX_INLINE(), pRallyPlot->getY_INLINE());
			}

			gDLL->getEventReporterIFace()->unitBuilt(this, pUnit);

			if (gCityLogLevel >= 1) { // BETTER_BTS_AI_MOD, AI logging, 10/02/09, jdog5000
				CvWString szString; getUnitAIString(szString, pUnit->AI_getUnitAIType());
				logBBAI("    City %S finishes production of unit %S with UNITAI %S", getName().GetCString(), pUnit->getName(0).GetCString(), szString.GetCString());
			}

		}
		break;
	}
	case ORDER_CONSTRUCT:
	{
		eConstructBuilding = pOrderNode->m_data.building();

		GET_PLAYER(getOwnerINLINE()).changeBuildingClassMaking(((BuildingClassTypes)(GC.getBuildingInfo(eConstructBuilding).getBuildingClassType())), -1);

		if (bFinish)
		{
			m_bHasHurried = false;

			iProductionNeeded = getYieldProductionNeeded(eConstructBuilding, YIELD_HAMMERS);
			const int iOverflow = std::max(0, getBuildingProduction(eConstructBuilding) - iProductionNeeded);
			changeOverflowProduction(iOverflow, getProductionModifier(eConstructBuilding));
			setBuildingProduction(eConstructBuilding, 0);
			setHasRealBuilding(eConstructBuilding, true);

			for (FatherPointTypes ePointType = FIRST_FATHER_POINT; ePointType < NUM_FATHER_POINT_TYPES; ++ePointType)
			{
				int gameSpeedMod =  GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
				int  buildBonus =  GC.getFatherPointInfo(ePointType).getBuildingPoints(GC.getBuildingInfo(eConstructBuilding).getBuildingClassType());
				buildBonus = buildBonus * gameSpeedMod /100;

				GET_PLAYER(getOwnerINLINE()).changeFatherPoints(ePointType, buildBonus);
			}

			gDLL->getEventReporterIFace()->buildingBuilt(this, eConstructBuilding);

			if (gCityLogLevel >= 1) // BETTER_BTS_AI_MOD, AI logging, 10/02/09, jdog5000
				logBBAI("    City %S finishes production of building %S", getName().GetCString(), GC.getBuildingInfo(eConstructBuilding).getDescription());
		}
		break;
	}
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

		gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), szSound, MESSAGE_TYPE_MINOR_EVENT, szIcon, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), true, true);
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

// Returns true if all yields are present to complete the current build, false otherwise. If eYieldException is not equal to NO_YIELD then
// the specified yield will not be included in the check. Intended to check if yields other than hammers are sufficient.
bool CvCity::checkRequiredYields(OrderTypes eOrder, BuildingTypes eBuilding, UnitTypes eUnit, YieldTypes eYieldException) const
{
	for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
	{
		if (eYieldException == eYield)
			continue;

		if (GC.getYieldInfo(eYield).isCargo())
		{
			int iAmount = 0;
			switch (eOrder)
			{
			case ORDER_TRAIN:
				iAmount = GET_PLAYER(getOwnerINLINE()).getYieldProductionNeeded(eUnit, eYield);
				break;
			case ORDER_CONSTRUCT:
				iAmount = GET_PLAYER(getOwnerINLINE()).getYieldProductionNeeded(eBuilding, eYield);
				break;
			default:
				break;
			}

			const int iYieldStored = getYieldStored(eYield);
			const int iYieldRushed = getYieldRushed(eYield);
			if (iAmount > iYieldStored + iYieldRushed)
			{
				return false;
			}
		}
	}

	return true;
}

bool CvCity::checkRequiredYields(OrderTypes eOrder, int iData1, YieldTypes eYieldException) const
{
	BuildingTypes eBuilding = eOrder == ORDER_CONSTRUCT ? static_cast<BuildingTypes>(iData1) : NO_BUILDING;
	UnitTypes eUnit = eOrder == ORDER_TRAIN ? static_cast<UnitTypes>(iData1) : NO_UNIT;
	return checkRequiredYields(eOrder, eBuilding, eUnit, eYieldException);
}

bool CvCity::checkRequiredYields(OrderData order, YieldTypes eYieldException) const
{
	switch (order.getType())
	{
	case ORDER_TRAIN:
		return checkRequiredYields(order.getType(), NO_BUILDING, order.unit(), eYieldException);
	case ORDER_CONSTRUCT:
		return checkRequiredYields(order.getType(), order.building(), NO_UNIT, eYieldException);
	default:
		return checkRequiredYields(order.getType(), NO_BUILDING, NO_UNIT, eYieldException);
	}
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

	if (!checkRequiredYields(pOrderNode->m_data))
	{
		return false;
	}

	for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
	{
		if (GC.getYieldInfo(eYield).isCargo())
		{
			int iNeeded = 0;
			switch (pOrderNode->m_data.getType())
			{
			case ORDER_TRAIN:
				iNeeded = GET_PLAYER(getOwnerINLINE()).getYieldProductionNeeded(pOrderNode->m_data.unit(), eYield);
				break;
			case ORDER_CONSTRUCT:
				iNeeded = GET_PLAYER(getOwnerINLINE()).getYieldProductionNeeded(pOrderNode->m_data.building(), eYield);
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

	for (UnitTypes eUnit = FIRST_UNIT; eUnit < NUM_UNIT_TYPES; ++eUnit)
	{
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
						if (pOrderNode->m_data.getType() == ORDER_TRAIN && pOrderNode->m_data.unit() == eUnit)
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

	// WTP, ray, refactored according to advice of Nightinggale
	for (BuildingTypes eBuilding = FIRST_BUILDING; eBuilding < NUM_BUILDING_TYPES; ++eBuilding)
	{
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
						if (pOrderNode->m_data.getType() == ORDER_CONSTRUCT && pOrderNode->m_data.building() == eBuilding)
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
	OOS_LOG("CvCity::doGrowth start", getID());
	int iDiff;

	if (GC.getUSE_DO_GROWTH_CALLBACK()) // K-Mod. block unused python callbacks
	{
		CyCity* const pyCity = new CyCity(static_cast<CvCity*>(this));
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

	changeFood(iDiff, true);
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
			// WTP, ray, Ethnically correct Population Growth - START
			UnitTypes eUnit = NO_UNIT;
			if (GLOBAL_DEFINE_ENABLE_ETHICALLY_CORRECT_GROWTH && !isNative())
			{
				// we have to cast from UnitClassTypes to int
				int iIDBestGrowthUnit = (int) bestGrowthUnitClass();
				eUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iIDBestGrowthUnit);
			}
			// old logic in else
			else
			{
				eUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("DEFAULT_POPULATION_UNIT"));
			}
			// WTP, ray, Ethnically correct Population Growth - END

			if (NO_UNIT != eUnit)
			{
				OOS_LOG_3("City growth unit", CvString(getName()).c_str(), getTypeStr(eUnit));
				GET_PLAYER(getOwnerINLINE()).initUnit(eUnit, GC.getCivilizationInfo(GET_PLAYER(getOwnerINLINE()).getCivilizationType()).getDefaultProfession(), getX_INLINE(), getY_INLINE());
				// WTP, ray, making this error save to prevent negative Storage bug - START
				int iFoodUsedForGrowth = growthThreshold() - getFoodKept();
				if (iFoodUsedForGrowth > getFood())
				{
					iFoodUsedForGrowth = getFood();
				}
				changeFood(-(std::max(0, iFoodUsedForGrowth)));
				// WTP, ray, making this error save to prevent negative Storage bug - END
			}

			gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_CITY_GROWTH", getNameKey()), coord(), "AS2D_POSITIVE_DINK", MESSAGE_TYPE_INFO, GC.getYieldInfo(YIELD_FOOD).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), true, true);


			// ONEVENT - City growth
			gDLL->getEventReporterIFace()->cityGrowth(this, getOwnerINLINE());
		}
	}
	else if (getFood() < 0)
	{
		// Food is reset to 0
		setFood(0);

		// Population is larger 1, we can eject citizens
		if (getPopulation() > 1)
		{
			if (!AI_removeWorstPopulationUnit(false))
			{
				AI_removeWorstPopulationUnit(true);
			}
			gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_CITY_STARVING", getNameKey()), coord(), "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_INFO, GC.getYieldInfo(YIELD_FOOD).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), true, true);
		}
		// WTP, ray, necessary changes related to branch PLAINS, which also allows settling in hostile Terrains without Food

		// Population is just 1, we do not want to abandon city
		else
		{
			int iFoodReceivedForStarvationDonation = GC.getDefineINT("CITY_STARVATION_DONATION_FOOD_RECEIVED");
			// Native Case: just to avoid triggering this unnecessarily for Natives
			if (isNative())
			{
				changeFood(iFoodReceivedForStarvationDonation);
			}

			// other players
			else
			{
				CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());
				int iGold = kPlayer.getGold();
				int iGoldToPayedForStarvationDonation = GC.getDefineINT("CITY_STARVATION_DONATION_GOLD_PAYED") * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getStoragePercent() / 100;
				int iOccupationTimerinCaseNoDonation = GC.getDefineINT("CITY_STARVATION_NO_DONATION_OCCUPATION_TIMER");

				// Case HUMAN: let us substract Gold for Human Player and trigger message about donation
				if (isHuman())
				{
					// We could donate food
					if (iGold > iGoldToPayedForStarvationDonation)
					{
						OOS_LOG_3("doGrowth donation", CvString(getName()).c_str(), iGoldToPayedForStarvationDonation);
						kPlayer.changeGold(-iGoldToPayedForStarvationDonation);
						changeFood(iFoodReceivedForStarvationDonation);
						gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_CITY_STARVING_BUT_COLONIES_PAID", getNameKey(), iGoldToPayedForStarvationDonation, iFoodReceivedForStarvationDonation), coord(), "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_INFO, GC.getYieldInfo(YIELD_FOOD).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), true, true);
					}

					// We did not have the gold, thus unrest
					// but only if the City is not already in unrest, to prevent endless loops
					else if (getOccupationTimer() == 0)
					{
						changeOccupationTimer(iOccupationTimerinCaseNoDonation);
						gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_CITY_STARVING_AND_REVOLTING", getNameKey()), coord(), "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_INFO, GC.getYieldInfo(YIELD_FOOD).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), true, true);
					}
				}

				// Case AI: We keep this simple for now
				// no unrest, receives gold but has to pay for it
				else
				{
					int iGoldForAI = iGoldToPayedForStarvationDonation/2;
					if (iGold > iGoldForAI)
					{
						kPlayer.changeGold(-iGoldForAI);
					}
					else
					{
						kPlayer.changeGold(-iGold);
					}
					changeFood(iFoodReceivedForStarvationDonation);
				}
			}
		}
	}
}

void CvCity::doYields()
{
	OOS_LOG("CvCity::doYields start", getID());
	int aiYields[NUM_YIELD_TYPES];
	calculateNetYields(aiYields, NULL, NULL, true);

	const int iTotalYields = getTotalYieldStored();
	const int iMaxCapacity = getMaxYieldCapacity();

	// WTP, ray, Happiness - START
	int iCityHappinessDomesticMarketGoldModifiers = getCityHappiness() - getCityUnHappiness();
	// WTP, ray, Happiness - END

	// WTP, ray, Crime and Law - START
	int iCityLawDomesticMarketGoldModifiers = getCityLaw() - getCityCrime();
	// WTP, ray, Crime and Law - END

	// WTP, ray, Domestic Market Profit Modifier - START
	int iDomesticMarketProfitModifierInPercent = GET_PLAYER(getOwnerINLINE()).getTotalPlayerDomesticMarketProfitModifierInPercent();
	// WTP, ray, Domestic Market Profit Modifier - END

	// R&R, ray, adjustment Domestic Markets


	if (getMarketModifier() > 0)
	{
		int iTotalProfitFromDomesticMarket = 0;
		YieldCargoArray<int> aYields;
		getYieldDemands(aYields);
		const InfoArray<YieldTypes>& kYieldArray = GC.getDomesticDemandYieldTypes();
		for (int i = 0; i < kYieldArray.getLength(); ++i)
		{
			const YieldTypes eYield = kYieldArray.get(i);

			FAssert(validEnumRange(eYield));
			int iAmount = aYields.get(eYield);
			if (iAmount > 0 && (getYieldStored(eYield) + aiYields[eYield]) > 0) // R&R, ray, improvment from vetiarvind
			{
				const int iAmountForSale = getYieldStored(eYield) + aiYields[eYield];
				if (iAmount > iAmountForSale)
				{
					iAmount = iAmountForSale;
				}
				int iProfit = iAmount * getYieldBuyPrice(eYield);

				// WTP, ray, Happiness - START
				iProfit = iProfit * (100 + iCityHappinessDomesticMarketGoldModifiers) / 100;
				// WTP, ray, Happiness - END

				// WTP, ray, Crime and Law - START
				iProfit = iProfit * (100 + iCityLawDomesticMarketGoldModifiers) / 100;
				// WTP, ray, Crime and Law - END

				// WTP, ray, Domestic Market Profit Modifier - START
				iProfit = iProfit * (100 + iDomesticMarketProfitModifierInPercent) / 100;
				// WTP, ray, Domestic Market Profit Modifier - END

				aiYields[eYield] -= iAmount;
				GET_PLAYER(getOwnerINLINE()).changeGold(iProfit);
				iTotalProfitFromDomesticMarket = iTotalProfitFromDomesticMarket + iProfit;
			}
		}
		if (iTotalProfitFromDomesticMarket != 0 && GC.getDOMESTIC_SALES_MESSAGES() == 1)
		{
			CvWString szBuffer = gDLL->getText("TXT_KEY_GOODS_DOMESTIC_SOLD", getNameKey(), iTotalProfitFromDomesticMarket);
			gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), NULL, MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), true, true);
		}
	}
	// R&R, ray, adjustment Domestic Markets, END

	// R&R, ray, adjustment for less Custom House messages
	int iCustomHouseProfit = 0;
	bool bPrintOnce = false;

	for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
	{

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
								gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_CULTUREEXPANDS", MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(YIELD_EDUCATION).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), true, true);

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

			// ray, making special storage capacity rules for Yields XML configurable
			if (GC.getYieldInfo(eYield).isCargo() && !GC.getYieldInfo(eYield).isIgnoredForStorageCapacity())
			{
				int iExcess = 0;
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
						OOS_LOG_3("doYields", CvString(getName()).c_str(), iProfit);
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
							gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), NULL, MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(eYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), true, true);
						}
						// R&R, ray , Changes to Custom House - END
					}
					else
					{
						CvWString szBuffer = gDLL->getText("TXT_KEY_GOODS_LOST", iLoss, GC.getYieldInfo(eYield).getChar(), getNameKey());
						gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(eYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), true, true);
					}
				}
				else if (!bPrintOnce && (iMaxCapacity - iTotalYields) > 0 && (iMaxCapacity - iTotalYields) < (iMaxCapacity / 10)) //only do this message once
				{
					bPrintOnce = true;
					CvWString szBuffer = gDLL->getText("TXT_KEY_RUNNING_OUT_OF_SPACE_NEW_CAPACITY", getNameKey());
					gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), true, true);
				}

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
		gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), NULL, MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), true, true);
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
	OOS_LOG("CvCity::doCulture start", getID());
	if (GC.getUSE_DO_CULTURE_CALLBACK()) // K-Mod. block unused python callbacks
	{
		CyCity* const pyCity = new CyCity(static_cast<CvCity*>(this));
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

	// WTP, ray, Political Points from Culture - START
	// we do this only for Colonies
	PlayerTypes eOwner = getOwnerINLINE();
	if (eOwner != NO_PLAYER && GET_PLAYER(eOwner).isPlayable())
	{
		//add culture to political points
		for (int i = 0; i < GC.getNumFatherPointInfos(); ++i)
		{
			FatherPointTypes ePointType = (FatherPointTypes) i;
			GET_PLAYER(eOwner).changeFatherPoints(ePointType, getCultureRate() * GC.getFatherPointInfo(ePointType).getYieldPoints(YIELD_CULTURE));
		}
	}
	// WTP, ray, Political Points from Culture - END
}


void CvCity::doPlotCulture(bool bUpdate, PlayerTypes ePlayer, int iCultureRate)
{
	OOS_LOG("CvCity::doPlotCulture start", getID());
	CultureLevelTypes eCultureLevel = (CultureLevelTypes)0;

	if (GC.getUSE_DO_PLOT_CULTURE_CALLBACK()) // K-Mod. block unused python callbacks
	{
		CyCity* const pyCity = new CyCity(static_cast<CvCity*>(this));
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
			for (int iDX = -eCultureLevel; iDX <= eCultureLevel; iDX++)
			{
				for (int iDY = -eCultureLevel; iDY <= eCultureLevel; iDY++)
				{
					const int iCultureRange = cultureDistance(iDX, iDY);

					if (iCultureRange <= eCultureLevel)
					{
						CvPlot* const pLoopPlot = plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);

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
	if (calculateNetYield(YIELD_EDUCATION) > 0)
	{
		std::set<UnitTypes> setExisting;
		for (uint i = 0; i < m_aPopulationUnits.size(); ++i)
		{
			UnitTypes eUnit =  m_aPopulationUnits[i]->getUnitType();
			if (setExisting.find(eUnit) == setExisting.end())
			{
				m_em_iSpecialistWeights.add(eUnit, GC.getUnitInfo(eUnit).getTeacherWeight());
				setExisting.insert(eUnit);
			}
		}
	}
}


bool CvCity::doCheckProduction()
{
	CLLNode<OrderData>* pOrderNode;
	UnitTypes eUpgradeUnit;
	int iUpgradeProduction;
	CvWString szBuffer;
	bool bOK = true;

	if (!isProduction() && !isDisorder() && isHuman() && !isProductionAutomated())
	{
		chooseProduction();
		return bOK;
	}

	for (UnitTypes eLoopUnit = FIRST_UNIT; eLoopUnit < NUM_UNIT_TYPES; ++eLoopUnit)
	{
		if (getFirstUnitOrder(eLoopUnit) != -1)
		{
			eUpgradeUnit = allUpgradesAvailable(eLoopUnit);

			if (eUpgradeUnit != NO_UNIT)
			{
				FAssertMsg(eUpgradeUnit != eLoopUnit, "eUpgradeUnit is expected to be different from iI");
				iUpgradeProduction = getUnitProduction(eLoopUnit);
				setUnitProduction(eLoopUnit, 0);
				setUnitProduction(eUpgradeUnit, iUpgradeProduction);

				pOrderNode = headOrderQueueNode();

				while (pOrderNode != NULL)
				{
					if (pOrderNode->m_data.getType() == ORDER_TRAIN)
					{
						OrderData& order = pOrderNode->m_data;
						if (order.unit() == eLoopUnit)
						{
							CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
							const CvUnitInfo& kUnitInfo = GC.getUnitInfo(order.unit());

							kOwner.changeUnitClassMaking(kUnitInfo.getUnitClassType(), -1);
							pOrderNode->m_data.unit() = eUpgradeUnit;
							if (kOwner.AI_unitValue(eUpgradeUnit, order.unitAI(), area()) == 0)
							{
								area()->changeNumTrainAIUnits(getOwnerINLINE(), order.unitAI(), -1);
								kOwner.AI_changeNumTrainAIUnits(order.unitAI(), -1);
								pOrderNode->m_data.unitAI() = (UnitAITypes)kUnitInfo.getDefaultUnitAIType();
								area()->changeNumTrainAIUnits(getOwnerINLINE(), order.unitAI(), 1);
								kOwner.AI_changeNumTrainAIUnits(order.unitAI(), 1);
							}
							kOwner.changeUnitClassMaking(kUnitInfo.getUnitClassType(), 1);
						}
					}

					pOrderNode = nextOrderQueueNode(pOrderNode);
				}
			}
		}
	}

	for (int iI = (getOrderQueueLength() - 1); iI >= 0; iI--)
	{
		OrderData* const pOrder = getOrderFromQueue(iI);

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
		for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
		{
			changeYieldStored(eYield, GC.getGameINLINE().getCargoYieldCapacity() / 2);
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
	OOS_LOG("CvCity::doProduction start", getID());
	//Move yields for hurry production from temp storage to city storage
	addTempHurryYieldsForProduction();

	if (GC.getUSE_DO_PRODUCTION_CALLBACK()) // K-Mod. block unused python callbacks
	{
		CyCity* const pyCity = new CyCity(static_cast<CvCity*>(this));
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
		if (!isProduction() || isProductionConvince() || AI_isChooseProductionDirty() ||
			getProduction() > getProductionNeeded(YIELD_HAMMERS))
		{
			if (!m_bHasHurried)
			{
				AI_chooseProduction();
			}
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
		const int iProduction = getStoredProductionDifference();

		const FatherPointTypes eFatherPointType = getProductionFatherPoint();
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
	// WTP, ray, refactored according to advice of Nightinggale
	for (BuildingTypes eBuilding = FIRST_BUILDING; eBuilding < NUM_BUILDING_TYPES; ++eBuilding)
	{
		if (getProductionBuilding() != eBuilding)
		{
			if (getBuildingProduction(eBuilding) > 0)
			{
				changeBuildingProductionTime(eBuilding, 1);

				if (isHuman())
				{
					if (getBuildingProductionTime(eBuilding) > GC.getDefineINT("BUILDING_PRODUCTION_DECAY_TIME"))
					{
						setBuildingProduction((eBuilding), ((getBuildingProduction(eBuilding) * GC.getDefineINT("BUILDING_PRODUCTION_DECAY_PERCENT")) / 100));
					}
				}
			}
			else
			{
				setBuildingProductionTime((eBuilding), 0);
			}
		}
	}

	for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
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
			OOS_LOG("Creating treasure from trading post", CvString(getName()).c_str());
			UnitClassTypes eUnitClass = (UnitClassTypes) GC.getDefineINT("TREASURE_UNITCLASS");
			UnitTypes eUnit = (UnitTypes) GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType()).getCivilizationUnits(eUnitClass);
			CvUnit* pTreasure = GET_PLAYER(ePlayer).initUnit(eUnit, GC.getUnitInfo(eUnit).getDefaultProfession(), plot()->getX_INLINE(), plot()->getY_INLINE(), NO_UNITAI, NO_DIRECTION, iCurrentTradePostGold);
			// when we create a treasure, set gold to 0
			setNativeTradePostGold(0);

			// we send a message now
			CvWString szBuffer = gDLL->getText("TXT_KEY_TRADE_POST_CREATED_TREASURE", getNameKey());
			gDLL->UI().addPlayerMessage(ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, pTreasure->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), true, true);
		}
	}
	return;
}
// WTP, ray, Native Trade Posts - END

// Private Functions...

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

void CvCity::getVisibleEffects(ZoomLevelTypes eCurZoom, std::vector<char const*>& kEffectNames) const
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
		else // Food difference is 0
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

char const* CvCity::getCityBillboardProductionIcon() const
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
		(void)pOrderNode; // Silence cppcheck
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
			CvUnit* pLoopUnit = plot()->getUnitNodeLoop(pUnitNode);

			if (pLoopUnit != NULL && pLoopUnit->getUnitTravelState() == UNIT_TRAVEL_STATE_LIVE_AMONG_NATIVES)
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

bool CvCity::isValidBuildingLocation(const CvBuildingInfo& kBuilding) const
{
	// if both the river and water flags are set, we require one of the two conditions, not both
	if (kBuilding.isWater())
	{
		if (!kBuilding.isRiver() || !plot()->isRiver())
		{
			if (!isCoastal(kBuilding.getMinAreaSize()))
			{
				return false;
			}
		}
	}
	else
	{
		if (area()->getNumTiles() < kBuilding.getMinAreaSize())
		{
			return false;
		}

		if (kBuilding.isRiver())
		{
			// WTP, fix for bRiver not considering Large Rivers
			// updated to use the newly added hasNearbyPlotWith - Nightinggale
			if (!(plot()->isRiver()) && !(plot()->hasNearbyPlotWith(TERRAIN_LARGE_RIVERS)))
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

	const InfoArray<BuildingClassTypes>& ReqBuildings = kTrigger.getBuildingsRequired();
	if (kTrigger.getNumBuildings() > 0 && ReqBuildings.getLength() > 0)
	{
		bool bFoundValid = false;

		for (int i = 0; i < ReqBuildings.getLength(); ++i)
		{
			// the InfoArray won't be containing NO_BUILDINGCLASS
			//if (ReqBuildings.getBuildingClass(i) != NO_BUILDINGCLASS)
			{
				BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(ReqBuildings.getBuildingClass(i));
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

	const InfoArray<EventTypes>& ReqEvents = kTrigger.getPrereqEvents();
	if (kTrigger.isPrereqEventCity() && ReqEvents.getLength() > 0)
	{
		bool bFoundValid = true;

		for (int iI = 0; iI < ReqEvents.getLength(); ++iI)
		{
			if (!isEventOccured(ReqEvents.getEvent(iI)))
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

	// changed to make apply and can apply calculate using the same code (read: same result) - Nightinggale
	if (kEvent.getFoodChange(this) + getFood() < 0)
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
		FOREACH(CityPlot)
		{
			if (eLoopCityPlot != CITY_HOME_PLOT)
			{
				CvPlot* pPlot = getCityIndexPlot(eLoopCityPlot);
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
		// changed to make apply and can apply calculate using the same code (read: same result) - Nightinggale
		changeFood(kEvent.getFoodChange(this));

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
				FOREACH(CityPlot)
				{
					CityPlotTypes ePlot = static_cast<CityPlotTypes>((eLoopCityPlot + iRandOffset) % NUM_CITY_PLOTS);
					if (ePlot != CITY_HOME_PLOT)
					{
						CvPlot* pPlot = getCityIndexPlot(ePlot);
						if (NULL != pPlot && pPlot->getOwnerINLINE() == getOwnerINLINE())
						{
							if (NO_IMPROVEMENT != pPlot->getImprovementType() && !GC.getImprovementInfo(pPlot->getImprovementType()).isPermanent())
							{
								CvWString szBuffer = gDLL->getText("TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED", GC.getImprovementInfo(pPlot->getImprovementType()).getTextKeyWide());
								gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_PILLAGED", MESSAGE_TYPE_INFO, GC.getImprovementInfo(pPlot->getImprovementType()).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), true, true);
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
				gDLL->UI().addPlayerMessage(eOtherPlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGED", MESSAGE_TYPE_INFO);
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
				OOS_LOG_3("Creating unit from event", getTypeStr(eEvent), getTypeStr(eUnit));
				GET_PLAYER(getOwnerINLINE()).initUnit(eUnit, GC.getUnitInfo(eUnit).getDefaultProfession(), getX_INLINE(), getY_INLINE());
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
		m_em_bBaseYieldRankValid.reset();
		m_em_bYieldRankValid.reset();
	}
	else
	{
		m_em_bBaseYieldRankValid.set(eYield, false);
		m_em_bYieldRankValid.set(eYield, false);
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
					gDLL->UI().addPlayerMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_REVOLTEND", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), true, true);
				}
			}
		}
		GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, eOwner, szBuffer, coord(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));

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

	FOREACH(CityPlot)
	{
		if (eLoopCityPlot != CITY_HOME_PLOT)
		{
			if (!isPlotProducingYields(eLoopCityPlot))
			{
				CvPlot* pPlot = getCityIndexPlot(eLoopCityPlot);

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

	FOREACH(CityPlot)
	{
		if (eLoopCityPlot != CITY_HOME_PLOT)
		{
			if (!isPlotProducingYields(eLoopCityPlot))
			{
				CvPlot* pPlot = getCityIndexPlot(eLoopCityPlot);

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

bool CvCity::removePopulationUnit(AssertCallerData assertData, CvUnit* pUnit, bool bDelete, ProfessionTypes eProfession, bool bConquest)
{
	int iUnitIndex = getPopulationUnitIndex(*pUnit);
	if(iUnitIndex < 0)
	{
		FAssertMsgWithCaller(assertData, false, "Could not find unit in city");
		return false;
	}

	if (!pUnit->canHaveProfession(eProfession, false))
	{
		FAssertMsgWithCaller(assertData, false, "Illegal Profession");
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

	FAssertWithCaller(assertData, pUnit->getOwnerINLINE() == getOwnerINLINE());
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
		pUnit->addToMap(coord());
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
			if (removePopulationUnit(CREATE_ASSERT_DATA, pUnit, false, eProfession))
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
				removePopulationUnit(CREATE_ASSERT_DATA, pUnit, false, eUnitProfession);
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

int CvCity::getPopulationUnitIndex(const CvUnit& kUnit) const
{
	for(uint i=0;i<m_aPopulationUnits.size();i++)
	{
		if(m_aPopulationUnits[i] == &kUnit)
		{
			return i;
		}
	}

	return -1;
}

CvPlot* CvCity::getPlotWorkedByUnit(const CvUnit* pUnit) const
{
	FOREACH(CityPlot)
	{
		int iUnitId = m_em_iWorkingPlot.get(eLoopCityPlot);
		if (iUnitId == pUnit->getID())
		{
			return getCityIndexPlot(eLoopCityPlot);
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
		pUnit = kOwner.initUnit(eUnit, GC.getUnitInfo(eUnit).getDefaultProfession(), getX_INLINE(), getY_INLINE(), NO_UNITAI, NO_DIRECTION, iYieldAmount);
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
		m_em_iOrderedStudents.reset();
		m_em_bOrderedStudentsRepeat.reset();
	} else {
		if (!(eUnit >= 0 && eUnit < GC.getNumUnitInfos() && iCount >= 0))
		{
			FAssert(eUnit >= 0);
			FAssert(eUnit < GC.getNumUnitInfos());
			FAssert(iCount < 0);
			return;
		}

		m_em_iOrderedStudents.set(eUnit, iCount);
		m_em_bOrderedStudentsRepeat.set(eUnit, bRepeat);
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
	FAssert(m_em_bOrderedStudentsRepeat.isInRange(eUnit));
	if (m_em_bOrderedStudentsRepeat.isAllocated() && m_em_iOrderedStudents.isAllocated())
	{
		for (UnitTypes eLoopUnit = m_em_bOrderedStudentsRepeat.FIRST; eLoopUnit <= m_em_bOrderedStudentsRepeat.LAST; ++eLoopUnit)
		{
			if (m_em_bOrderedStudentsRepeat.get(eLoopUnit))
			{
				m_em_iOrderedStudents.set(eLoopUnit, 1);
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
	return m_em_iOrderedStudents.get(eUnit);
}

bool CvCity::getOrderedStudentsRepeat(UnitTypes eUnit)
{
	FAssert(eUnit >= 0);
	FAssert(eUnit < GC.getNumUnitInfos());
	return m_em_bOrderedStudentsRepeat.get(eUnit);
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
		CvUnit* pLoopUnit = pPlot->getUnitNodeLoop(pUnitNode);

		if (pLoopUnit != NULL && pLoopUnit->getUnitTravelState() == UNIT_TRAVEL_STATE_LIVE_AMONG_NATIVES)
		{
			pLoopUnit->setUnitTravelState(NO_UNIT_TRAVEL_STATE, false);
			pLoopUnit->setUnitTravelTimer(0);
		}
	}
}

void CvCity::ejectMissionary()
{
	int missionaryRate = getMissionaryRate();
	if (missionaryRate > 0) {
		PlayerTypes missionaryPlayer = getMissionaryPlayer();

		if (missionaryPlayer == NO_PLAYER)
		{
			return;
		}

		UnitTypes EjectedMissionaryType = (UnitTypes)GC.getCivilizationInfo(GET_PLAYER(missionaryPlayer).getCivilizationType()).getCivilizationUnits(UNITCLASS_COLONIST);

		if (missionaryRate > GC.getProfessionInfo(PROFESSION_MISSIONARY).getMissionaryRate() ) {
			EjectedMissionaryType = (UnitTypes)GC.getCivilizationInfo(GET_PLAYER(missionaryPlayer).getCivilizationType()).getCivilizationUnits(GC.getProfessionInfo(PROFESSION_MISSIONARY).LbD_getExpert());
		}

		GET_PLAYER(missionaryPlayer).initUnit(EjectedMissionaryType, PROFESSION_MISSIONARY, getX_INLINE(), getY_INLINE());

		CvWString szBuffer = gDLL->getText("TXT_KEY_MISSIONARY_EJECTED_FROM_GIFTED_CITY", plot()->getPlotCity()->getNameKey());
		gDLL->UI().addPlayerMessage(missionaryPlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_ESTABLISH_MISSION).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), true, true);

		setMissionaryRate(0);
		setMissionaryPlayer(NO_PLAYER, false);
	}
}

void CvCity::ejectTrader()
{
	int nativeTradeRate = getNativeTradeRate();
	if (nativeTradeRate > 0) {
		PlayerTypes tradePostPlayer = getTradePostPlayer();

		if (tradePostPlayer == NO_PLAYER)
		{
			return;
		}

		UnitTypes EjectedTraderType = (UnitTypes)GC.getCivilizationInfo(GET_PLAYER(tradePostPlayer).getCivilizationType()).getCivilizationUnits(UNITCLASS_COLONIST);

		if (nativeTradeRate > GC.getProfessionInfo(PROFESSION_NATIVE_TRADER).getNativeTradeRate() ) {
			EjectedTraderType = (UnitTypes)GC.getCivilizationInfo(GET_PLAYER(tradePostPlayer).getCivilizationType()).getCivilizationUnits(GC.getProfessionInfo(PROFESSION_NATIVE_TRADER).LbD_getExpert());
		}

		GET_PLAYER(tradePostPlayer).initUnit(EjectedTraderType, PROFESSION_NATIVE_TRADER, getX_INLINE(), getY_INLINE());

		CvWString szBuffer = gDLL->getText("TXT_KEY_TRADER_EJECTED_FROM_GIFTED_CITY", plot()->getPlotCity()->getNameKey());
		gDLL->UI().addPlayerMessage(tradePostPlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_ESTABLISH_TRADE_POST).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), true, true);

		setNativeTradeRate(0);
		setTradePostPlayer(NO_PLAYER, false);
	}
}

bool CvCity::canProduceYield(YieldTypes eYield) const
{
	CvPlayer& kPlayer = GET_PLAYER(getOwnerINLINE());
	for (int i = 0; i < GC.getNumProfessionInfos(); i++)
	{
		const ProfessionTypes eLoopProfession = (ProfessionTypes)i;
		const CvProfessionInfo& kLoopProfession = GC.getProfessionInfo(eLoopProfession);
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
		// set allow conquer to CivEffect data rather than hardcoding - Nightinggale
		const CvPlayerAI& kPrevOwner = GET_PLAYER(getPreviousOwner());
		const CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());

		if (!kOwner.canConquerCity(kPrevOwner.getCivCategoryTypes()))
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
		m_em_bScoutVisited.set(eTeam, bVisited);
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
	return m_em_bScoutVisited.get(eTeam);
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
			OOS_LOG("can receive goody", getTypeStr(eGoody));
			int iRandValue = GC.getGameINLINE().getSorenRandNum(GC.getGoodyInfo(eGoody).getCityGoodyWeight(), "City Goodies");
			if(iRandValue > iBestValue)
			{
				iBestValue = iRandValue;
				eBestGoody = eGoody;
			}
		}
	}

	OOS_LOG("best goody", getTypeStr(eBestGoody));
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

void CvCity::setMissionaryPlayer(PlayerTypes ePlayer, bool bBurnMessage)
{
	if (ePlayer != getMissionaryPlayer())
	{
		PlayerTypes eOldPlayer = getMissionaryPlayer();

		m_eMissionaryPlayer = ePlayer;

		if (bBurnMessage && eOldPlayer != NO_PLAYER)
		{
			CvWString szBuffer = gDLL->getText("TXT_KEY_MISSION_REMOVED", getNameKey(), GET_PLAYER(eOldPlayer).getCivilizationAdjectiveKey());

			gDLL->UI().addAllPlayersMessage(false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), false, false);
		}

		if (getMissionaryPlayer() != NO_PLAYER)
		{
			CvWString szBuffer = gDLL->getText("TXT_KEY_MISSION_ESTABLISHED", getNameKey(), GET_PLAYER(ePlayer).getCivilizationAdjectiveKey());

			gDLL->UI().addAllPlayersMessage(false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_ESTABLISH_MISSION).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), true, true);
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
void CvCity::setTradePostPlayer(PlayerTypes ePlayer, bool bBurnMessage)
{
	if (ePlayer != getTradePostPlayer())
	{
		PlayerTypes eOldPlayer = getTradePostPlayer();

		m_eTradePostPlayer = ePlayer;

		if (bBurnMessage && eOldPlayer != NO_PLAYER)
		{
			CvWString szBuffer = gDLL->getText("TXT_KEY_TRADE_POST_REMOVED", getNameKey(), GET_PLAYER(eOldPlayer).getCivilizationAdjectiveKey());

			gDLL->UI().addAllPlayersMessage(false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), false, false);
		}

		if (getTradePostPlayer() != NO_PLAYER)
		{
			CvWString szBuffer = gDLL->getText("TXT_KEY_TRADE_POST_ESTABLISHED", getNameKey(), GET_PLAYER(ePlayer).getCivilizationAdjectiveKey());

			gDLL->UI().addAllPlayersMessage(false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_ESTABLISH_TRADE_POST).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), true, true);
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

// WTP, ray, Health Overhaul - START
int CvCity::getCityHealthChangeFromCentralPlot() const
{
	int iCityHealthChangeFromCentralPlot = 0;

	// getting the modifiers
	int iSweetWaterLocationChange = GC.getSWEET_WATER_CITY_LOCATION_HEALTH_BONUS();
	int iCoastalLocationChange = GC.getCOASTAL_CITY_LOCATION_HEALTH_BONUS();
	int iHillLocationChange = GC.getHILL_CITY_LOCATION_HEALTH_BONUS();
	int iBadLocationChange= GC.getBAD_CITY_LOCATION_HEALTH_MALUS();


	// getting City Center Plot
	CvPlot* pCityCenterPlot = plot();

	// let us be save, maybe Abandon City may mess around
	if (pCityCenterPlot != NULL)
	{
		if (pCityCenterPlot->isFreshWater())
		{
			iCityHealthChangeFromCentralPlot += iSweetWaterLocationChange;
		}

		if (pCityCenterPlot->isCoastalLand())
		{
			iCityHealthChangeFromCentralPlot += iCoastalLocationChange;
		}

		if (pCityCenterPlot->isHills())
		{
			iCityHealthChangeFromCentralPlot += iHillLocationChange;
		}

		if (GC.getTerrainInfo(pCityCenterPlot->getTerrainType()).isBadCityLocation())
		{
			iCityHealthChangeFromCentralPlot -= iBadLocationChange;
		}

	}

	return iCityHealthChangeFromCentralPlot;
}

int CvCity::getCityHealthChangeFromRessourcesInCityRadius() const
{
	int iCityHealthChangeFromRessourcesInCityRadius = 0;

	FOREACH(CityPlot)
	{
		CvPlot* pLoopPlot = getCityIndexPlot(eLoopCityPlot);

		if(pLoopPlot != NULL && pLoopPlot->getBonusType() != NO_BONUS)
		{
			// only if worked
			if (isPlotProducingYields(pLoopPlot))
			{
				iCityHealthChangeFromRessourcesInCityRadius += GC.getBonusInfo(pLoopPlot->getBonusType()).getHealthEffectFromRessource();
			}
		}
	}

	return iCityHealthChangeFromRessourcesInCityRadius;
}
// WTP, ray, Health Overhaul - END

int CvCity::getCityHealthChange() const
{
	int iNegHealthFromPopulation = getCityHealthChangeFromPopulation();
	int iPosHealthFromHealers = calculateNetYield(YIELD_HEALTH);

	// WTP, ray, Health Overhaul - START
	int iHealthFromCityCenterPlot = getCityHealthChangeFromCentralPlot();
	int iHealthChangeFromRessources = getCityHealthChangeFromRessourcesInCityRadius();

	int iTotalHealthChange = iPosHealthFromHealers - iNegHealthFromPopulation + iHealthFromCityCenterPlot + iHealthChangeFromRessources;
	return iTotalHealthChange;
	// WTP, ray, Health Overhaul - END
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


// WTP, ray, Improvements give Bonus to their City - START
int CvCity::getMonasteryCrossBonusForCity() const
{
	//not necessary for Natives, saves performance
	if (isNative())
	{
		return 0;
	}

	int iMonasteryCrossBonus = 0;
	int iMonasteryCrossBonusModifier = GC.getDefineINT("MONASTERY_CROSSES_MODIFIER_FOR_CITY");
	FOREACH(CityPlot)
	{
		CvPlot* pLoopPlot = getCityIndexPlot(eLoopCityPlot);
		// if it has a Monastery and a Missionary in it
		if(pLoopPlot != NULL && pLoopPlot->isMonastery() && (pLoopPlot->getMonasteryMissionary() != NULL))
		{
			// we double if it is second level improvement, which we know if it has no more upgrade
			if (GC.getImprovementInfo(pLoopPlot->getImprovementType()).getImprovementUpgrade() == NO_IMPROVEMENT)
			{
				iMonasteryCrossBonusModifier = iMonasteryCrossBonusModifier * 2;
			}
			// we give the Bonus only if also worked by a worker inside the City
			if (isPlotProducingYields(pLoopPlot))
			{
				iMonasteryCrossBonus += iMonasteryCrossBonusModifier;
			}
		}
	}
	return iMonasteryCrossBonus;
}

int CvCity::getFortDefenseBonusForCity() const
{
	//not necessary for Natives, saves performance
	if (isNative())
	{
		return 0;
	}

	int iFortDefenseBonus = 0;
	int iFortDefenseBonusModifier = GC.getDefineINT("FORT_DEFENSE_MODIFIER_FOR_CITY");
	FOREACH(CityPlot)
	{
		CvPlot* pLoopPlot = getCityIndexPlot(eLoopCityPlot);
		// if it has a Fort that is protected
		if(pLoopPlot != NULL && pLoopPlot->isFort() && (pLoopPlot->getFortDefender() != NULL))
		{
			// we double if it is second level improvement, which we know if it has no more upgrade
			if (GC.getImprovementInfo(pLoopPlot->getImprovementType()).getImprovementUpgrade() == NO_IMPROVEMENT)
			{
				iFortDefenseBonusModifier = iFortDefenseBonusModifier * 2;
			}
			// we give the Bonus only if also worked by a worker inside the City
			if (isPlotProducingYields(eLoopCityPlot))
			{
				iFortDefenseBonus += iFortDefenseBonusModifier;
			}
		}
	}
	return iFortDefenseBonus;
}
// WTP, ray, Improvements give Bonus to their City - END


// WTP, ray, Improvements give Bonus to their City - PART 2 - START
int CvCity::getImprovementFoodModifierForCity() const
{
	//not necessary for Natives, saves performance
	if (isNative())
	{
		return 0;
	}

	int FoodModifierForCity = 0;
	FOREACH(CityPlot)
	{
		CvPlot* pLoopPlot = getCityIndexPlot(eLoopCityPlot);
		if (pLoopPlot != NULL)
		{
			ImprovementTypes eImprovement = pLoopPlot->getImprovementType();
			if(eImprovement != NO_IMPROVEMENT)
			{
				const CvImprovementInfo& info = GC.getImprovementInfo(eImprovement);
				if (info.getFoodModifierForCity() > 0)
				{
					// we give the Bonus only if also worked by a worker inside the City
					if (isPlotProducingYields(eLoopCityPlot))
					{
						FoodModifierForCity += info.getFoodModifierForCity();
					}
				}
			}
		}
	}
	return FoodModifierForCity;
}

int CvCity::getImprovementHammersModifierForCity() const
{
	//not necessary for Natives, saves performance
	if (isNative())
	{
		return 0;
	}

	int HammersModifierForCity = 0;
	FOREACH(CityPlot)
	{
		CvPlot* pLoopPlot = getCityIndexPlot(eLoopCityPlot);
		if(pLoopPlot != NULL)
		{
			ImprovementTypes eImprovement = pLoopPlot->getImprovementType();
			if(eImprovement != NO_IMPROVEMENT)
			{
				const CvImprovementInfo& info = GC.getImprovementInfo(eImprovement);
				if (info.getHammersModifierForCity() > 0)
				{
					// we give the Bonus only if also worked by a worker inside the City
					if (isPlotProducingYields(eLoopCityPlot))
					{
						HammersModifierForCity += info.getHammersModifierForCity();
					}
				}
			}
		}
	}
	return HammersModifierForCity;
}

int CvCity::getImprovementToolsModifierForCity() const
{
	//not necessary for Natives, saves performance
	if (isNative())
	{
		return 0;
	}

	int ToolsModifierForCity = 0;
	FOREACH(CityPlot)
	{
		CvPlot* pLoopPlot = getCityIndexPlot(eLoopCityPlot);
		if (pLoopPlot != NULL)
		{
			ImprovementTypes eImprovement = pLoopPlot->getImprovementType();
			if(eImprovement != NO_IMPROVEMENT)
			{
				const CvImprovementInfo& info = GC.getImprovementInfo(eImprovement);
				if (info.getToolsModifierForCity() > 0)
				{
					// we give the Bonus only if also worked by a worker inside the City
					if (isPlotProducingYields(eLoopCityPlot))
					{
						ToolsModifierForCity += info.getToolsModifierForCity();
					}
				}
			}
		}
	}
	return ToolsModifierForCity;
}
// WTP, ray, Improvements give Bonus to their City - PART 2 - END

// WTP, ray, Improvements give Bonus to their City - PART 3 - START
int CvCity::getImprovementStorageModifierForCity() const
{
	//not necessary for Natives, saves performance
	if (isNative())
	{
		return 0;
	}

	int StorageModifierForCity = 0;
	FOREACH(CityPlot)
	{
		CvPlot* pLoopPlot = getCityIndexPlot(eLoopCityPlot);
		if (pLoopPlot != NULL)
		{
			ImprovementTypes eImprovement = pLoopPlot->getImprovementType();
			if(eImprovement != NO_IMPROVEMENT)
			{
				const CvImprovementInfo& info = GC.getImprovementInfo(eImprovement);
				if (info.getStorageModifierForCity() > 0)
				{
					// we give the Bonus only if also worked by a worker inside the City
					if (isPlotProducingYields(eLoopCityPlot))
					{
						StorageModifierForCity += info.getStorageModifierForCity();
					}
				}
			}
		}
	}

	// consider GameSpeed
	StorageModifierForCity = StorageModifierForCity* GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getStoragePercent() / 100;

	return StorageModifierForCity;
}
// WTP, ray, Improvements give Bonus to their City - PART 3 - END

// WTP, ray, Happiness - START
void CvCity::doCityHappiness()
{
	OOS_LOG("doCityHappiness", getID());

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
	int iMaxFoundingFatherPoints = GC.getFOUNDING_FATHER_POINTS_FESTIVITIES_HAPPINESS();
	int iFoundingFatherPoints = GC.getGameINLINE().getSorenRandNum(iMaxFoundingFatherPoints, "Festivities Happiness Father Points");

	int iNumFatherPointInfos = GC.getNumFatherPointInfos();
	// we skip fathr points for Exploration - which is 0 that is why we first subract 2 and then add 1
	int randomFatherCategorySelection = GC.getGameINLINE().getSorenRandNum(iNumFatherPointInfos - 2, "Random Father Point selection");
	randomFatherCategorySelection = randomFatherCategorySelection + 1;
	FatherPointTypes ePointType = (FatherPointTypes) randomFatherCategorySelection;

	// let us add a bit so it is not too low with bad luck
	iFoundingFatherPoints += iMaxFoundingFatherPoints/2;

	GET_PLAYER(getOwnerINLINE()).changeFatherPoints(ePointType, iFoundingFatherPoints);

	CvWString szBuffer;
	szBuffer = gDLL->getText("TXT_KEY_FESTIVITIES_BECAUSE_HAPPINESS", getNameKey(), iFoundingFatherPoints, GC.getFatherPointInfo(ePointType).getDescription());
	gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_CULTUREEXPANDS", MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(YIELD_CULTURE).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), true, true);

	return;
}

void CvCity::doCityUnHappiness()
{
	OOS_LOG("doCityUnHappiness", getID());

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
	gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_CITYCAPTURED", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), true, true);

	return;
}
// WTP, ray, Happiness - END

// WTP, ray, Crime and Law - START
void CvCity::doCityLaw()
{
	//add Law to political points as currently configured in XML
	int iNetCityLaw = getCityLaw() - getCityCrime();

	// we do not do anything if the net law is negative
	if (iNetCityLaw <= 0)
	{
		return;
	}

	PlayerTypes eOwner = getOwnerINLINE();
	for (int i = 0; i < GC.getNumFatherPointInfos(); ++i)
	{
		FatherPointTypes ePointType = (FatherPointTypes) i;
		GET_PLAYER(eOwner).changeFatherPoints(ePointType, iNetCityLaw * GC.getFatherPointInfo(ePointType).getYieldPoints(YIELD_LAW));
	}
	return;
}

void CvCity::doCityCrime()
{
	// this could most easily just steal money or cause unrest
	int iCityCrime = getCityCrime() - getCityLaw();

	// we do not do anything if the net crime is negative
	if (iCityCrime <= 0)
	{
		return;
	}

	PlayerTypes eOwner = getOwnerINLINE();
	CvPlayerAI& kPlayer = GET_PLAYER(eOwner);

	// we substract gold if there is enough
	if (kPlayer.getGold() > iCityCrime)
	{
		OOS_LOG_3("Crime", CvString(getName()).c_str(), iCityCrime);
		kPlayer.changeGold(-iCityCrime);

		// add message
		CvWString szBuffer = gDLL->getText("TXT_KEY_CITY_GOLD_STOLEN_BECAUSE_CRIME", getNameKey());
		gDLL->UI().addPlayerMessage(eOwner, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_CITYCRIME", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("INTERFACE_SHOW_CTIYCRIME")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), true, true);

	}

	// otherwise we set occupation if we are not already in occupation - but only for Human
	else if (isHuman() && !isOccupation())
	{
		if (iCityCrime < 5)
		{
			setOccupationTimer(1);
		}
		else
		{
			setOccupationTimer(2);
		}

		// add message
		CvWString szBuffer = gDLL->getText("TXT_KEY_CITY_UNREST_BECAUSE_CRIME", getNameKey());
		gDLL->UI().addPlayerMessage(eOwner, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_CITYCAPTURED", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), true, true);
	}

	return;
}
// WTP, ray, Crime and Law - END

// WTP, ray Domestic Market Events - START
void CvCity::checkForDomesticDemandEvent()
{
	OOS_LOG("checkForDomesticDemandEvent", getID());

	// ok, let us check all conditions where we do nothing and exit

	// if the feature is not activated, do nothing
	if (!GC.getENABLE_DOMESTIC_DEMAND_EVENTS())
	{
		return;
	}

	// we exit if there is still a Domestic Demand Timer, so the last Event was not long enough ago
	// before we do so however, we reduce the timer
	if (getTimerDomesticDemandEvent() > 0)
	{
		reduceTimerDomesticDemandEvent();
		return;
	}

	// we exit if there is already a Domestic Demand Event going on
	// before we do so however, we reduce the duration counter
	// if event is over, we also clear the modifier and start the timer for next
	if (getRemainingDurationDomesticDemandEvent() > 0)
	{
		reduceRemainingDurationDomesticDemandEvent();

		//  also in here we may start the Timer, to get some timely disance to the next
		// also in here we would then also set the modifier to 0 again
		if (getRemainingDurationDomesticDemandEvent() == 0)
		{
			// the event is over, set modifiser again to 0
			setDomesticDemandEventPriceModifier(0);
			setDomesticDemandEventDemandModifier(0);

			// the event is over, now start the timer so the next is not directly triggered
			setTimerDomesticDemandEvent(GC.getDefineINT("TIMER_BETWEEN_DOMESTIC_DEMAND_EVENTS"));
		}

		return;
	}


	// we exit if we the city is not owned by a Colonial Nation or if the Player is in Revolution
	PlayerTypes eOwner = getOwnerINLINE();
	if (eOwner == NO_PLAYER || !GET_PLAYER(getOwnerINLINE()).isPlayable() || GET_PLAYER(getOwnerINLINE()).isInRevolution())
	{
		return;
	}

	// we exit if we are in rebellion or City Health is negative or city too small
	if (isOccupation() || getCityHealth() < 0 || getPopulation() < GC.getDefineINT("MIN_CITY_POPULATION_DOMESTIC_DEMAND_EVENTS"))
	{
		return;
	}

	// we exit if there is no Market Building in the City
	if (getMarketModifier() == 0)
	{
		return;
	}

	// ok, now we can do the actual logic

	// 1. get the Happiness of the City and split into positive case or negative case
	int iHappinessBalance = getCityHappiness() - getCityUnHappiness();

	// it is balanced, do nothing
	if (iHappinessBalance == 0)
	{
		return;
	}

	// if it is positive, we may trigger a positive Domestic Market Event
	else if (iHappinessBalance > 0)
	{
		int iPositiveDomesticEventRandom = GC.getGameINLINE().getSorenRandNum(100, "Randomized Chance of Positive Domestic Demand Event");
		int iPositiveDomesticEventChance = iHappinessBalance * GC.getDefineINT("PERCENT_CHANCE_PER_HAPPINESS_DOMESTIC_DEMAND_EVENTS");

		// in case we have enough Happiness to pass the random check : trigger positive Domestic Demand Event
		if (iPositiveDomesticEventChance >= iPositiveDomesticEventRandom)
		{
			// one modifier is for demand, the other one is for price
			setDomesticDemandEventDemandModifier(GC.getDefineINT("DEMAND_INCREASE_PERCENT_DOMESTIC_DEMAND_EVENTS"));
			setDomesticDemandEventPriceModifier(GC.getDefineINT("PRICE_INCREASE_PERCENT_DOMESTIC_DEMAND_EVENTS"));

			// duration is also influenced by the Happiness Balance
			int iDurationOfDomesticDemandEvent = iHappinessBalance + GC.getDefineINT("BASE_DURATION_DOMESTIC_DOMESTIC_DEMAND_EVENTS");
			setDurationDomesticDemandEvent(iDurationOfDomesticDemandEvent);

			// add message
			CvWString szBuffer = gDLL->getText("TXT_KEY_CITY_DOMESTIC_MARKET_EVENT_POSITIVE", getNameKey());
			gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), true, true);
		}
	}

	// if iHappinessBalance is negative, we may trigger a negative Domestic Market Event
	else
	{
		int iNegativeDomesticEventRandom = GC.getGameINLINE().getSorenRandNum(100, "Randomized Chance of Negative Domestic Demand Event");
		int iNegativeDomesticEventChance = -1 * iHappinessBalance * GC.getDefineINT("PERCENT_CHANCE_PER_HAPPINESS_DOMESTIC_DEMAND_EVENTS");

		// in case we have enough Happiness to pass the random check : trigger positive Domestic Demand Event
		if (iNegativeDomesticEventChance >= iNegativeDomesticEventRandom)
		{
			// one modifier is for demand, the other one is for price
			setDomesticDemandEventDemandModifier(-GC.getDefineINT("DEMAND_DECREASE_PERCENT_DOMESTIC_DEMAND_EVENTS"));
			setDomesticDemandEventPriceModifier(-GC.getDefineINT("PRICE_DECREASE_PERCENT_DOMESTIC_DEMAND_EVENTS"));

			// duration is also influenced by the Happiness Balance
			int iDurationOfDomesticDemandEvent = -1 * iHappinessBalance + GC.getDefineINT("BASE_DURATION_DOMESTIC_DOMESTIC_DEMAND_EVENTS");
			setDurationDomesticDemandEvent(iDurationOfDomesticDemandEvent);

			// add message
			CvWString szBuffer = gDLL->getText("TXT_KEY_CITY_DOMESTIC_MARKET_EVENT_NEGATIVE", getNameKey());
			gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), true, true);
		}
	}

	return;
}

// the Duration methods
void CvCity::setDurationDomesticDemandEvent(int iDuration)
{
	m_iDomesticDemandEventDuration = iDuration;
}

void CvCity::reduceRemainingDurationDomesticDemandEvent()
{
	if (m_iDomesticDemandEventDuration > 0)
	{
		m_iDomesticDemandEventDuration = m_iDomesticDemandEventDuration -1;
	}
}

int CvCity::getRemainingDurationDomesticDemandEvent() const
{
	return m_iDomesticDemandEventDuration;
}

// the timer methods
void CvCity::setTimerDomesticDemandEvent(int iTimer)
{
	m_iDomesticDemandEventTimer = iTimer;
}

void CvCity::reduceTimerDomesticDemandEvent()
{
	if (m_iDomesticDemandEventTimer > 0)
	{
		m_iDomesticDemandEventTimer = m_iDomesticDemandEventTimer -1;
	}
}

int CvCity::getTimerDomesticDemandEvent() const
{
	return m_iDomesticDemandEventTimer;
}

// the price modifier
void CvCity::setDomesticDemandEventPriceModifier(int iPriceModifier)
{
	m_iDomesticDemandEventPriceModifier = iPriceModifier;
}

int CvCity::getDomesticDemandEventPriceModifier() const
{
	return m_iDomesticDemandEventPriceModifier;
}

// the deamnd modifier
void CvCity::setDomesticDemandEventDemandModifier(int iDemandModifier)
{
	m_iDomesticDemandEventDemandModifier = iDemandModifier;
}

int CvCity::getDomesticDemandEventDemandModifier() const
{
	return m_iDomesticDemandEventDemandModifier;
}
// WTP, ray Domestic Market Events - END

// WTP, ray, new Harbour System - START
int CvCity::getCityHarbourSpace() const
{
	if (!plot()->isCoastalLand())
	{
		return 0;
	}

	return m_iCityHarbourSpace > 0 ? m_iCityHarbourSpace : 0;
}

void CvCity::changeCityHarbourSpace(int iValue)
{
	m_iCityHarbourSpace += iValue;
}

int CvCity::getCityHarbourSpaceUsed() const
{
	int iCityHarbourSpaceUsed = 0;
	CvPlot* pPlot = plot();
	for (int i = 0; i < pPlot->getNumUnits(); ++i)
	{
		CvUnit* pLoopUnit = pPlot->getUnitByIndex(i);
		if (pLoopUnit != NULL && pLoopUnit->getDomainType() == DOMAIN_SEA)
		{
			iCityHarbourSpaceUsed += pLoopUnit->getUnitInfo().getHarbourSpaceNeeded();
		}
	}

	return iCityHarbourSpaceUsed;
}

bool CvCity::bShouldShowCityHarbourSystem() const
{
	if (GLOBAL_DEFINE_ENABLE_NEW_HARBOUR_SYSTEM && plot()->isCoastalLand() && isHuman())
	{
		return true;
	}

	return false;
}
// WTP, ray, new Harbour System - END


// WTP, ray, new Barracks System - START
int CvCity::getCityBarracksSpace() const
{
	return m_iCityBarracksSpace > 0 ? m_iCityBarracksSpace : 0;
}

void CvCity::changeCityBarracksSpace(int iValue)
{
	m_iCityBarracksSpace += iValue;
}

int CvCity::getCityBarracksSpaceUsed() const
{
	int iCityBarracksSpaceUsed = 0;
	CvPlot* pPlot = plot();
	for (int i = 0; i < pPlot->getNumUnits(); ++i)
	{
		CvUnit* pLoopUnit = pPlot->getUnitByIndex(i);
		// we only count Land Units that can attack, civil Units are not considered
		// we also not consider Units loaded on Ships
		// we also not consider Units of other Nations
		if (pLoopUnit != NULL && pLoopUnit->getDomainType() == DOMAIN_LAND && pLoopUnit->canAttack() && pLoopUnit->getTransportUnit() == NULL && pLoopUnit->getOwnerINLINE() == getOwnerINLINE())
		{
			iCityBarracksSpaceUsed += pLoopUnit->getUnitInfo().getBarracksSpaceNeeded();
			// we also need to consider Professions
			if (pLoopUnit->getProfession() != NO_PROFESSION)
			{
				iCityBarracksSpaceUsed += GC.getProfessionInfo(pLoopUnit->getProfession()).getBarracksSpaceNeededChange();
			}
		}
	}

	return iCityBarracksSpaceUsed;
}

bool CvCity::bShouldShowCityBarracksSystem() const
{
	if (GLOBAL_DEFINE_ENABLE_NEW_HARBOUR_SYSTEM && isHuman())
	{
		return true;
	}

	return false;
}
// WTP, ray, new Barracks System - END

// WTP, ray, Happiness - START
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
	iTotalCityHappiness += yields().getBaseRawYieldProduced(YIELD_HAPPINESS);
	// WTP, ray, trying to fix Rebel Rate Modifier on Happiness for Balancing - END
	iTotalCityHappiness += getHappinessFromCrosses();
	iTotalCityHappiness += getHappinessFromBells();
	iTotalCityHappiness += getHappinessFromHealth();
	iTotalCityHappiness += getHappinessFromCulture();
	iTotalCityHappiness += getHappinessFromLaw();
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
	iTotalCityUnHappiness += yields().getBaseRawYieldProduced(YIELD_UNHAPPINESS); // should not exist but in case it ever does
	// WTP, ray, trying to fix Rebel Rate Modifier on Happiness for Balancing - END
	iTotalCityUnHappiness += getUnhappinessFromPopulation();
	iTotalCityUnHappiness += getUnhappinessFromCrime();
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

int CvCity::getUnhappinessFromCrime() const
{
	int iUnHapCrime = 0;
	int iNetCrime = getCityCrime() - getCityLaw();

	if (iNetCrime <= 0)
	{
		return 0;
	}

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

	iUnHapCrime = iNetCrime / ((iPopulation / iPopDivisor));

	if (iUnHapCrime > (iPopulation / iPopDivisor))
	{
		iUnHapCrime = (iPopulation / iPopDivisor);
	}
	// if we produce a little, we give at least 1 Happiness
	if (iUnHapCrime == 0	)
	{
		iUnHapCrime = 1;
	}
	return iUnHapCrime;
}

int CvCity::getUnhappinessFromSlavery() const
{
	int iUnHapSlav = 0;
	for (int i = 0; i < getPopulation(); ++i)
	{
		CvUnit* const pLoopUnit = getPopulationUnitByIndex(i);
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
		for (TraitTypes eTrait = FIRST_TRAIT; eTrait < NUM_TRAIT_TYPES; ++eTrait)
		{
			if (hasTrait(eTrait))
			{
				const CvTraitInfo& kTrait = GC.getTraitInfo(eTrait);
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
	int iDefendersFound = 0;
	CvPlot* pPlot = plot();
	for (int i = 0; i < pPlot->getNumUnits(); ++i)
	{
		CvUnit* pLoopUnit = pPlot->getUnitByIndex(i);
		// we check for a Land Unit that can fight on that plot and is from our own Player
		if (pLoopUnit != NULL && pLoopUnit->getDomainType() == DOMAIN_LAND && pLoopUnit->canAttack() && pLoopUnit->getOwnerINLINE() == getOwnerINLINE())
		{
			iDefendersFound++;
		}
	}
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
		iHapCulture = 1;
	}
	return iHapCulture;
}

int CvCity::getHappinessFromLaw() const
{
	int iHapLaw = 0;
	int iNetLaw = getCityLaw() - getCityCrime();

	if (iNetLaw <= 0)
	{
		return 0;
	}

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

	iHapLaw = iNetLaw / ((iPopulation / iPopDivisor));

	if (iHapLaw > (iPopulation / iPopDivisor))
	{
		iHapLaw = (iPopulation / iPopDivisor);
	}
	// if we produce a little, we give at least 1 Happiness
	if (iHapLaw == 0)
	{
		iHapLaw = 1;
	}
	return iHapLaw;
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


// WTP, ray, Crime and Law - START
int CvCity::getCityLaw() const
{
	return m_iCityLaw;
}

void CvCity::setCityLaw(int iValue)
{
	if (iValue < 0)
	{
		return;
	}

	m_iCityLaw = iValue;
}

void CvCity::updateCityLaw()
{
	int iTotalCityLaw = 0;

	//iTotalCityLaw += getBaseRawYieldProduced(YIELD_LAW);
	iTotalCityLaw= calculateNetYield(YIELD_LAW);
	iTotalCityLaw += getLawFromCityDefenders();
	iTotalCityLaw += getLawFromCrosses();

	setCityLaw(iTotalCityLaw);
}

int CvCity::getCityCrime() const
{
	return m_iCityCrime;
}

void CvCity::setCityCrime(int iValue)
{
	if (iValue < 0)
	{
		return;
	}

	m_iCityCrime = iValue;
}

void CvCity::updateCityCrime()
{
	int iTotalCityCrime = 0;

	iTotalCityCrime += yields().getBaseRawYieldProduced(YIELD_CRIME); // should not exist but in case it ever does
	iTotalCityCrime += getCrimeFromPopulation();
	iTotalCityCrime += getCrimeFromUnhappiness();
	iTotalCityCrime += getCrimeFromWars();

	int iCrimeModifierFromOverflow = getCrimBonusFactorFromOverflow();
	iTotalCityCrime = iTotalCityCrime * (100 + iCrimeModifierFromOverflow) / 100;

	setCityCrime(iTotalCityCrime);
}

// specific methods
int CvCity::getLawFromCityDefenders() const
{
	int iDefendersFound = 0;
	CvPlot* pPlot = plot();
	for (int i = 0; i < pPlot->getNumUnits(); ++i)
	{
		CvUnit* pLoopUnit = pPlot->getUnitByIndex(i);
		// we check for a Land Unit that can fight on that plot and is from our own Player
		if (pLoopUnit != NULL && pLoopUnit->getDomainType() == DOMAIN_LAND && pLoopUnit->canAttack() && pLoopUnit->getOwnerINLINE() == getOwnerINLINE())
		{
			iDefendersFound++;
			iDefendersFound += pLoopUnit->getAdditionalLawToCityFromUnit(); // WTP, ray, Lawkeeper Promotion - START
		}
	}

	// we can never generate more than what we have as Population
	if (iDefendersFound > getPopulation())
	{
		iDefendersFound = getPopulation();
	}

	return iDefendersFound;
}

int CvCity::getLawFromCrosses() const
{
	int iLawCrosses = 0;
	int iCrosses = calculateNetYield(YIELD_CROSSES);
	int iPopulation = getPopulation();
	int iPopDivisor = GC.getPOP_DIVISOR_CRIME();

	if (iCrosses == 0)
	{
		return 0;
	}

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

	iLawCrosses = iCrosses / (iPopulation / iPopDivisor);

	if (iLawCrosses > (iPopulation / iPopDivisor))
	{
		iLawCrosses = (iPopulation / iPopDivisor);
	}
	// if we produce a little, we give at least 1 Happiness
	if (iLawCrosses == 0 && iCrosses > 0)
	{
		iLawCrosses = 1;
	}

	return iLawCrosses;
}

int CvCity::getCrimeFromPopulation() const
{
	int iCrimeFromPopulation = 0;
	int iPopulation = getPopulation();
	int iMinPopForCrime = GC.getMIN_POP_CRIME();

	// small help for AI
	if(!isHuman())
	{
		iMinPopForCrime = iMinPopForCrime*2;
	}

	// do not start with crime too early
	if (iPopulation < iMinPopForCrime)
	{
		return 0;
	}

	int iDivisor = GC.getPOP_DIVISOR_CRIME();
	// just for safety
	if (iDivisor == 0)
	{
		return 0;
	}

	int iSmoothener = (iMinPopForCrime / iDivisor) - 1;
	iCrimeFromPopulation = (iPopulation / iDivisor) - iSmoothener;

	return iCrimeFromPopulation;
}

int CvCity::getCrimeFromUnhappiness() const
{
	int iCrimeFromUnhappiness = 0;

	// we use the diff between Happiness and Unhappiness
	int iUnhappiness = getCityUnHappiness() - getCityHappiness();

	if (iUnhappiness <= 0)
	{
		return 0;
	}

	int iPopulation = getPopulation();
	int iPopDivisor = GC.getPOP_DIVISOR_CRIME();

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

	iCrimeFromUnhappiness = iUnhappiness / (iPopulation / iPopDivisor);

	if (iCrimeFromUnhappiness > (iPopulation / iPopDivisor))
	{
		iCrimeFromUnhappiness = (iPopulation / iPopDivisor);
	}
	// if we produce a little, we give at least 1 Happiness
	if (iCrimeFromUnhappiness == 0)
	{
		iCrimeFromUnhappiness = 1;
	}
	return iCrimeFromUnhappiness;
}

int CvCity::getCrimeFromWars() const
{
	int iCrimeFromWars = 0;
	int iFactorPerWar = GC.getPER_EUROPEAN_AT_WAR_CRIME();
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

	iCrimeFromWars = iFactorPerWar * iNumEuropeanWars;
	return iCrimeFromWars;
}

int CvCity::getCrimBonusFactorFromOverflow() const
{
	int iCrimeBonusFactorFromOverflow = 0;
	int iMaxFactor = GC.getCRIME_PERCENT_BONUS_FACTOR_OVERFLOW();

	// there is overflow
	if (getTotalYieldStored() > getMaxYieldCapacity())
	{
		iCrimeBonusFactorFromOverflow = iMaxFactor;
	}

	// sorage is moree than 2 thirds full
	else if (getTotalYieldStored() > getMaxYieldCapacity() / 3 * 2)
	{
		iCrimeBonusFactorFromOverflow = iMaxFactor / 3 * 2;
	}

	// storage is half full
	else if (getTotalYieldStored() > getMaxYieldCapacity() / 2)
	{
		iCrimeBonusFactorFromOverflow = iMaxFactor / 2;
	}

	return iCrimeBonusFactorFromOverflow;
}
// WTP, ray, Crime and Law - END


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

// WTP, ray, Ethnically correct Population Growth - START
UnitClassTypes CvCity::bestGrowthUnitClass() const
{
	UnitClassTypes eBestUnitClass = NO_UNITCLASS;

	std::map<EthnicityTypes, int> ethnicityCounts;
	std::map<CitizenStatusTypes, int> citizenStatusCounts;

	ethnicityCounts[ETHNICITY_EUROPEAN] = 0;
	ethnicityCounts[ETHNICITY_INDIO] = 0;
	ethnicityCounts[ETHNICITY_AFRICAN] = 0;
	ethnicityCounts[ETHNICITY_MESTIZZO] = 0;
	ethnicityCounts[ETHNICITY_MULATTO] = 0;

	citizenStatusCounts[CITIZEN_STATUS_FREE] = 0;
	citizenStatusCounts[CITIZEN_STATUS_INDENTURED] = 0;
	citizenStatusCounts[CITIZEN_STATUS_ENSLAVED] = 0;
	citizenStatusCounts[CITIZEN_STATUS_ELITE] = 0;

	EthnicityTypes eBestEthnicity = NO_ETHNICITY;
	CitizenStatusTypes eBestCitizenStatus = NO_CITIZEN_STATUS;

	for (uint i = 0; i < m_aPopulationUnits.size(); ++i)
	{
		CvUnit* pLoopUnit = m_aPopulationUnits[i];
		EthnicityTypes eEthnicityInsideCity = pLoopUnit->getUnitInfo().getEthnicity();
		CitizenStatusTypes eCitizenStatusInsideCity = pLoopUnit->getUnitInfo().getCitizenStatus();

		ethnicityCounts[eEthnicityInsideCity]++;
		citizenStatusCounts[eCitizenStatusInsideCity]++;
	}

	CvPlot* pCityCenterPlot = plot();
	CLLNode<IDInfo>* pUnitNode = pCityCenterPlot->headUnitNode();
	while (pUnitNode)
	{
		CvUnit* pLoopUnit2 = plot()->getUnitNodeLoop(pUnitNode);

		if (pLoopUnit2 != NULL && pLoopUnit2->getOwnerINLINE() == getOwnerINLINE())
		{
			EthnicityTypes eEthnicityCityPlot = pLoopUnit2->getUnitInfo().getEthnicity();
			CitizenStatusTypes eCitizenStatusCityPlot = pLoopUnit2->getUnitInfo().getCitizenStatus();

			ethnicityCounts[eEthnicityCityPlot]++;
			citizenStatusCounts[eCitizenStatusCityPlot]++;
		}
	}

	int iTotalEthnicityCount = 0;
	for (std::map<EthnicityTypes, int>::iterator it = ethnicityCounts.begin(); it != ethnicityCounts.end(); ++it) {
		iTotalEthnicityCount += it->second;
	}

	eBestEthnicity = selectWeightedRandom(ethnicityCounts, iTotalEthnicityCount);

	int iTotalCitizenStatusCount = 0;
	for (std::map<CitizenStatusTypes, int>::iterator it = citizenStatusCounts.begin(); it != citizenStatusCounts.end(); ++it) {
		iTotalCitizenStatusCount += it->second;
	}

	eBestCitizenStatus = selectWeightedRandom(citizenStatusCounts, iTotalCitizenStatusCount);

	if (eBestEthnicity == NO_ETHNICITY || eBestCitizenStatus == NO_CITIZEN_STATUS)
	{
		eBestUnitClass = GLOBAL_DEFINE_UNITCLASS_EUROPEAN_FREE;
	}
	else
	{
		if (eBestEthnicity == ETHNICITY_EUROPEAN) {
			switch (eBestCitizenStatus) {
			case CITIZEN_STATUS_FREE:
				eBestUnitClass = GLOBAL_DEFINE_UNITCLASS_EUROPEAN_FREE;
				break;
			case CITIZEN_STATUS_INDENTURED:
				eBestUnitClass = GLOBAL_DEFINE_UNITCLASS_EUROPEAN_INDENTURED;
				break;
			case CITIZEN_STATUS_ENSLAVED:
				eBestUnitClass = GLOBAL_DEFINE_UNITCLASS_EUROPEAN_ENSLAVED;
				break;
			case CITIZEN_STATUS_ELITE:
				eBestUnitClass = GLOBAL_DEFINE_UNITCLASS_EUROPEAN_ELITE;
				break;
			default:
				eBestUnitClass = GLOBAL_DEFINE_UNITCLASS_EUROPEAN_FREE;
				break;
			}
		}
		else if (eBestEthnicity == ETHNICITY_INDIO) {
			switch (eBestCitizenStatus) {
			case CITIZEN_STATUS_FREE:
				eBestUnitClass = GLOBAL_DEFINE_UNITCLASS_INDIO_FREE;
				break;
			case CITIZEN_STATUS_INDENTURED:
				eBestUnitClass = GLOBAL_DEFINE_UNITCLASS_INDIO_INDENTURED;
				break;
			case CITIZEN_STATUS_ENSLAVED:
				eBestUnitClass = GLOBAL_DEFINE_UNITCLASS_INDIO_ENSLAVED;
				break;
			case CITIZEN_STATUS_ELITE:
				eBestUnitClass = GLOBAL_DEFINE_UNITCLASS_INDIO_ELITE;
				break;
			default:
				eBestUnitClass = GLOBAL_DEFINE_UNITCLASS_EUROPEAN_FREE;
				break;
			}
		}
		else if (eBestEthnicity == ETHNICITY_AFRICAN) {
			switch (eBestCitizenStatus) {
			case CITIZEN_STATUS_FREE:
				eBestUnitClass = GLOBAL_DEFINE_UNITCLASS_AFRICAN_FREE;
				break;
			case CITIZEN_STATUS_INDENTURED:
				eBestUnitClass = GLOBAL_DEFINE_UNITCLASS_AFRICAN_INDENTURED;
				break;
			case CITIZEN_STATUS_ENSLAVED:
				eBestUnitClass = GLOBAL_DEFINE_UNITCLASS_AFRICAN_ENSLAVED;
				break;
			case CITIZEN_STATUS_ELITE:
				eBestUnitClass = GLOBAL_DEFINE_UNITCLASS_AFRICAN_ELITE;
				break;
			default:
				eBestUnitClass = GLOBAL_DEFINE_UNITCLASS_EUROPEAN_FREE;
				break;
			}
		}
		else if (eBestEthnicity == ETHNICITY_MESTIZZO) {
			switch (eBestCitizenStatus) {
			case CITIZEN_STATUS_FREE:
				eBestUnitClass = GLOBAL_DEFINE_UNITCLASS_MESTIZZO_FREE;
				break;
			case CITIZEN_STATUS_INDENTURED:
				eBestUnitClass = GLOBAL_DEFINE_UNITCLASS_MESTIZZO_INDENTURED;
				break;
			case CITIZEN_STATUS_ENSLAVED:
				eBestUnitClass = GLOBAL_DEFINE_UNITCLASS_MESTIZZO_ENSLAVED;
				break;
			case CITIZEN_STATUS_ELITE:
				eBestUnitClass = GLOBAL_DEFINE_UNITCLASS_MESTIZZO_ELITE;
				break;
			default:
				eBestUnitClass = GLOBAL_DEFINE_UNITCLASS_EUROPEAN_FREE;
				break;
			}
		}
		else if (eBestEthnicity == ETHNICITY_MULATTO) {
			switch (eBestCitizenStatus) {
			case CITIZEN_STATUS_FREE:
				eBestUnitClass = GLOBAL_DEFINE_UNITCLASS_MULATTO_FREE;
				break;
			case CITIZEN_STATUS_INDENTURED:
				eBestUnitClass = GLOBAL_DEFINE_UNITCLASS_MULATTO_INDENTURED;
				break;
			case CITIZEN_STATUS_ENSLAVED:
				eBestUnitClass = GLOBAL_DEFINE_UNITCLASS_MULATTO_ENSLAVED;
				break;
			case CITIZEN_STATUS_ELITE:
				eBestUnitClass = GLOBAL_DEFINE_UNITCLASS_MULATTO_ELITE;
				break;
			default:
				eBestUnitClass = GLOBAL_DEFINE_UNITCLASS_EUROPEAN_FREE;
				break;
			}
		}
	}

	if (eBestUnitClass == NO_UNITCLASS)
	{
		eBestUnitClass = GLOBAL_DEFINE_UNITCLASS_EUROPEAN_FREE;
	}

	return eBestUnitClass;
}
// WTP, ray, Ethnically correct Population Growth - END

CvUnit* CvCity::ejectBestDefender(CvUnit* pCurrentBest, CvUnit* pAttacker)
{

	ProfessionTypes eProfession;
	CvUnit* pDefender = getBestDefender(&eProfession, pCurrentBest, pAttacker);

	if (pDefender != NULL)
	{
		if (pDefender != pCurrentBest)
		{
			if (!removePopulationUnit(CREATE_ASSERT_DATA, pDefender, false, eProfession))
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
	if (!removePopulationUnit(CREATE_ASSERT_DATA, pUnit, false, GC.getCivilizationInfo(GET_PLAYER(getOwnerINLINE()).getCivilizationType()).getDefaultProfession()))
	{
		return false;
	}

	CvUnit* pLearnUnit = GET_PLAYER(getOwnerINLINE()).initUnit(eUnit, NO_PROFESSION, pUnit->getX_INLINE(), pUnit->getY_INLINE(), pUnit->AI_getUnitAIType());
	FAssert(pLearnUnit != NULL);
	pLearnUnit->convert(pUnit, true);

	OOS_LOG_3("Newly educated unit", CvString(getName()).c_str(), getTypeStr(eUnit));

	// TAC - EDUCATION COST SYSTEM - koma13, KingMB, Netbandit - 23.11.09 - START
	/*
	// Original Code
	//
	// reduce accumulated weights, but round up
	for (int i = 0; i < GC.getNumUnitInfos(); ++i)
	{
		m_em_iSpecialistWeights[i] = (m_em_iSpecialistWeights[i] + 1) / 2;
	}
	*/
	// TAC - EDUCATION COST SYSTEM - koma13, KingMB, Netbandit - 23.11.09 - END


	setEducationThresholdMultiplier((getEducationThresholdMultiplier() * (100 + GC.getEDUCATION_THRESHOLD_INCREASE())) / 100);
	OOS_LOG_3("educateStudent", CvString(getName()).c_str(), iPrice);
	kPlayer.changeGold(-iPrice);

	BuildingTypes eSchoolBuilding = getYieldBuilding(YIELD_EDUCATION);
	FAssert(eSchoolBuilding != NO_BUILDING);
	if(eSchoolBuilding != NO_BUILDING)
	{
		CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_STUDENT_GRADUATED", GC.getUnitInfo(eUnit).getTextKeyWide(), getNameKey(), GC.getBuildingInfo(eSchoolBuilding).getTextKeyWide());
		gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_CULTUREEXPANDS", MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(YIELD_EDUCATION).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), true, true);
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
/** Ermittelt die h�chste Bildungsstufe, welche ein Geb�ude der     **/
/** Stadt anbietet.                                                 **/
/**                                                                 **/
/*********************************************************************/

void CvCity::NBMOD_SetCityTeachLevelCache() // NBMOD EDU cache - Nightinggale
{
	int iMaxTeachLevel = 0;

	// loop all possible buildings
	// WTP, ray, refactored according to advice of Nightinggale
	for (BuildingTypes eBuilding = FIRST_BUILDING; eBuilding < NUM_BUILDING_TYPES; ++eBuilding)
	{
		// check if the Building exists in City
		if (isHasBuilding(eBuilding))
		{
			if (GC.getBuildingInfo(eBuilding).NBMOD_GetTeachLevel() > iMaxTeachLevel)
			{
				iMaxTeachLevel = GC.getBuildingInfo(eBuilding).NBMOD_GetTeachLevel();
			}
		}
	}
	m_NBMOD_CityTeachLevel_cache = iMaxTeachLevel; // NBMOD EDU cache - Nightinggale
}

/** NBMOD EDU **/

bool CvCity::canTeach(UnitTypes eUnit) const
{
	return getSpecialistTuition(eUnit) >= 0;
}

int CvCity::getSpecialistTuition(UnitTypes eUnit) const
{
	if (m_em_iSpecialistWeights.get(eUnit) <= 0)
	{
		return -1;
	}

	/** NBMOD EDU **/
	// Liegt das Bildungsniveau der Stadt unter dem f�r diese Einheit n�tigen Niveau?
	if (NBMOD_GetCityTeachLevel() <  GC.getUnitInfo(eUnit).NBMOD_GetTeachLevel())
	{
        return -1;
	}
	/** NBMOD EDU **/

	int iBestWeight = m_em_iSpecialistWeights.getMax();
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

	iPrice *= iBestWeight - m_em_iSpecialistWeights[eUnit];
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

		iPrice *= iBestWeight - m_em_iSpecialistWeights.get(eUnit);
		iPrice /= iBestWeight;
	}
	else
	{
		double fPrice = double(iPrice);
		double fMulti = 0.0;

		// Ausbildungsstufen-Multiplikator entsprechend ben�tigter Ausbildungsstufe aus der Definitionsdatei holen
		if (GC.getUnitInfo(eUnit).NBMOD_GetTeachLevel() == 1)	// 1. (niedrigste) Ausblidungsstufe (Schule)
		{
			fMulti = GC.getTAC_EDUCATION_RANK1();
		}
		else if (GC.getUnitInfo(eUnit).NBMOD_GetTeachLevel() == 2)	// 2. (mittlere) Ausbilungsstufe (Hochschule)
		{
			fMulti = GC.getTAC_EDUCATION_RANK2();
		}
		else if (GC.getUnitInfo(eUnit).NBMOD_GetTeachLevel() == 3)	// 3. (h�chste) Ausbildungsstufe (Universit�t)
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
	return m_em_bTradeExports.get(eYield);
	// traderoute just-in-time - end - Nightinggale
}

void CvCity::addExport(YieldTypes eYield, bool bUpdateRoutes)
{
	// traderoute just-in-time - start - Nightinggale
	if (m_em_bTradeExports.get(eYield))
	{
		return;
	}

	m_em_bTradeExports.set(eYield, true);
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
	if (!m_em_bTradeExports.get(eYield))
	{
		return;
	}

	m_em_bTradeExports.set(eYield, false);
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
	return m_em_bTradeImports.get(eYield);
	// traderoute just-in-time - end - Nightinggale
}

void CvCity::addImport(YieldTypes eYield, bool bUpdateRoutes)
{
	// traderoute just-in-time - start - Nightinggale
	if (m_em_bTradeImports.get(eYield))
	{
		return;
	}

	m_em_bTradeImports.set(eYield, true);
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
	if (!m_em_bTradeImports.get(eYield) || getImportsMaintain(eYield))
	{
		return;
	}

	m_em_bTradeImports.set(eYield, false);
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
	// replaced vanilla code as it only took care of import and export
	// this call will call the same button as the "clear all" button
	// this clears feeder service and auto export too
	handleAutoTraderouteSetup(true, false, false);
}

void CvCity::setMaintainLevel(YieldTypes eYield, int iMaintainLevel)
{
	if (getMaintainLevel(eYield) != iMaintainLevel)
	{
		// transport feeder - begin - Nightinggale
		int iOld = getMaintainLevel(eYield);
		// transport feeder - end - Nightinggale

		// traderoute just-in-time - start - Nightinggale
		m_em_iTradeThreshold.set(eYield, iMaintainLevel);
		// traderoute just-in-time - end - Nightinggale

		// transport feeder - begin - Nightinggale
		if (getAutoMaintainThreshold(eYield) < iMaintainLevel)
		{
			// threshold is increased.
			// increase cache without checking normal cache check as we know at this point that a full check will result in the same as using iMaintainLevel
			m_em_iTradeAutoThreshold.set(eYield, iMaintainLevel);
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
	return m_em_iTradeThreshold.get(eYield);
	// traderoute just-in-time - end - Nightinggale
}

// R&R mod, vetiarvind, max yield import limit - Start
void CvCity::setImportsLimit(YieldTypes eYield, int iValue)
{
	if (getImportsLimit(eYield) != iValue)
	{
		m_em_iTradeMaxThreshold.set(eYield, iValue);
		if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		}
	}
}

// Note: AI code should use the getMaxImportAmount wrapper rather than calling this function directly
int CvCity::getImportsLimit(YieldTypes eYield) const
{
	return m_em_iTradeMaxThreshold.get(eYield);
}

// Returns the max number of yield units that the city is willing to accept
int CvCity::getMaxImportAmount(YieldTypes eYield) const
{
	// automated transports will ignore import settings if loaded with something no city imports.
	// the alternative is to get the transport unit stuck, possibly forever.
	// for this reason, the assert message can trigger for rare, but intended behavior.
	//    Nightinggale
	//FAssert(isImport(eYield));

	const int iImportLimit = m_em_iTradeMaxThreshold.get(eYield);

	if (iImportLimit == 0)
	{
		// The city has not set a limit for this yield, return the amount of remaining storage
		return  getMaxYieldCapacity() - getTotalYieldStored();
	}
	else
	{
		// The city has set an import limit, return the max amount it can accept
		const int iRemainingCapacity = getMaxYieldCapacity() - getTotalYieldStored();
		return std::min(iRemainingCapacity, iImportLimit);
	}
}


// R&R mod, vetiarvind, max yield import limit - End

// transport feeder - start - Nightinggale

void CvCity::setImportsMaintain(YieldTypes eYield, bool bSetting)
{
	m_em_bTradeImportsMaintain.set(eYield, bSetting);

	bool bImportStopped = isAutoImportStopped(eYield);
	if (bImportStopped)
	{
		m_em_bTradeStopAutoImport.set(eYield, false);
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
	FAssert(validEnumRange(eYield));

	if (!m_em_bTradeImportsMaintain.get(eYield))
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
		m_em_bTradeStopAutoImport.set(eYield, true);
	} else if (isAutoImportStopped(eYield) && (iNeededLevel > iStoredLevel || (iStoredLevel <= (iMaintainLevel*3)/4))) {
		m_em_bTradeStopAutoImport.set(eYield, false);
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

	m_em_iProductionNeeded.set(eYield, iProductionNeeded);

	if (iProductionNeeded == MAX_INT)
	{
		// the city isn't producing anything. Set threshold to 0 instead of infinity.
		m_em_iTradeAutoThreshold.set(eYield, 0);
	} else {
		m_em_iTradeAutoThreshold.set(eYield, iProductionNeeded);
	}

#if 0
	// only first in production queue
	//ma_tradeAutoThreshold.keepMax(getProductionNeeded(eYield), eYield);
	// no need for this part. getProductionNeeded() is called twice in a row. The second result should never be bigger than the first.
#else
	// everything in production queue
	for  (CLLNode<OrderData>* pOrderNode = headOrderQueueNode(); pOrderNode != NULL; pOrderNode = nextOrderQueueNode(pOrderNode))
	{
		switch (pOrderNode->m_data.getType())
		{
		case ORDER_TRAIN:
			m_em_iTradeAutoThreshold.keepMax(eYield,getYieldProductionNeeded(pOrderNode->m_data.unit(), eYield));
			break;

		case ORDER_CONSTRUCT:
			m_em_iTradeAutoThreshold.keepMax(eYield, getYieldProductionNeeded(pOrderNode->m_data.building(), eYield));
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
			m_em_iTradeAutoThreshold.keepMax(eYield, getYieldProductionNeeded((UnitTypes)aOrders[i].second, eYield));
			break;

		case ORDER_CONSTRUCT:
			m_em_iTradeAutoThreshold.keepMax(eYield, getYieldProductionNeeded((BuildingTypes)aOrders[i].second, eYield));
			break;
		}
	}

	// increase threshold to manually set threshold, if needed
	m_em_iTradeAutoThreshold.keepMax(eYield, m_em_iTradeThreshold.get(eYield));

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
	m_em_bTradeAutoExport.set(eYield, bExport);
}

void CvCity::handleAutoTraderouteSetup(bool bReset, bool bImportAll, bool bAutoExportAll)
{
	// reset import/export for any yields that cannot be transported
	// they might have been accidentally added to import/exports if XML settings were changed inbetween
	// they cannot be added/removed manually
	for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
	{
		if (!GC.getYieldInfo(eYield).isCargo() || !GC.getYieldInfo(eYield).isExportYield())
		{
			doTask(TASK_YIELD_TRADEROUTE, eYield, 0, false, false, false, false);
		}
	}

	if (bReset)
	{
		for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
		{
			if (GC.getYieldInfo(eYield).isCargo())
			{
				// the easiest way to reset the settings for the yield in question is to call doTask for that yield
				// note that doTask should be called directly, not gDLL->sendDoTask()
				// this is because this function runs in parallel on all computers in the network
				// as a result, generating network traffic would be a mistake, which could cause bugs and lag
				// this is why the network communication is skipped in this case, something which would normally be a bug
				doTask(TASK_YIELD_TRADEROUTE, eYield, 0, false, false, false, false);
			}
		}
		return;
	}

	if (bImportAll || bAutoExportAll)
	{
		for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
		{
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

				doTask(TASK_YIELD_TRADEROUTE, eYield, iBuffer, bImport, bExport, bMaintainImport, bAutoExport);
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
	// WTP, ray, refactored according to advice of Nightinggale
	for (BuildingTypes eBuilding = FIRST_BUILDING; eBuilding < NUM_BUILDING_TYPES; ++eBuilding)
	{
		if (GC.getBuildingInfo(eBuilding).getSpecialBuildingType() == iValue && isHasBuilding(eBuilding))
		{
			return true;
		}
	}
	return false;
}
// PatchMod: Achievements END

// TAC - LbD - Ray - START

bool CvCity::LbD_try_become_expert(CvUnit* convUnit, int base, int increase, int pre_rounds, int l_level, int chance_increase_expert_from_teacher, int pre_rounds_expert_decrease_from_teacher)
{
	// get data from Unit
	int workedRounds = convUnit->getLbDrounds();
	ProfessionTypes lastProfession = convUnit->getLastLbDProfession();
	ProfessionTypes currentProfession = convUnit->getProfession();

	// WTP, ray, saving 1 more Profession for Fisher Issue - START
	int workedRoundsBefore = convUnit->getLbDroundsBefore();
	ProfessionTypes lastProfessionBefore = convUnit->getLastLbDProfessionBefore();
	// WTP, ray, saving 1 more Profession for Fisher Issue - END

	// WTP, ray, teacher addon for LbD - START
	// The Expert we might convert to later and also valid teachers
	int expert = GC.getProfessionInfo(currentProfession).LbD_getExpert();
	UnitTypes expertUnitType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(expert);
	bool bValidTeacherFound = false;

	// now we check if the esxpert is in the City as teacher
	for (uint i = 0; i < m_aPopulationUnits.size(); ++i)
	{
		if (m_aPopulationUnits[i]->getUnitInfo().getUnitClassType() == GC.getUnitInfo(expertUnitType).getUnitClassType())
		{
			bValidTeacherFound = true;
			continue;
		}
	}

	// we found a valid teacher and we add or substract teacher values
	if (bValidTeacherFound)
	{
		increase += chance_increase_expert_from_teacher;
		pre_rounds -= pre_rounds_expert_decrease_from_teacher;
	}
	// WTP, ray, teacher addon for LbD - END

	//getting KI-Modifier
	int ki_modifier = GC.getLBD_KI_MOD_EXPERT();

	// there is a Profession change
	if (currentProfession != lastProfession)
	{
		if(isHuman())
		{
			// old logic before Fisher Fix - Standard Case
			// except storing Last Profession and the round already worked init
			if (currentProfession != lastProfessionBefore)
			{
				//WTP, fix for LbD and Multiple Professions per Building / same Expert for Multiple Professions
				convUnit->setLastLbDProfession(currentProfession);

				bool bSameExpert = lastProfession != NO_PROFESSION && (expert == GC.getProfessionInfo(lastProfession).LbD_getExpert());
				//the Profession changes to another one with different Expert - thus we reset
				if(!bSameExpert)
				{
					//workedRounds = 1;
					convUnit->setLbDrounds(1);

					// WTP, ray, saving 1 more Profession for Fisher Issue
					// we need to store the old data
					convUnit->setLbDroundsBefore(workedRounds);
					convUnit->setLastLbDProfessionBefore(lastProfession);
				}
				//the Expert is the same - thus we normally increase
				else
				{
					convUnit->setLbDrounds(workedRounds + 1);
					//convUnit->setLbDroundsBefore(workedRounds + 1);
				}
			}

			// WTP, ray, saving 1 more Profession for Fisher Issue - START
			// otherwise current Profession is the Profession before: in this case we need to restore old data and switch
			else
			{
				// now we switch
				// check just for safety - should never happen
				if(lastProfessionBefore != NO_PROFESSION && lastProfession != NO_PROFESSION)
				{
					convUnit->setLbDrounds(workedRoundsBefore +1);
					convUnit->setLbDroundsBefore(workedRounds);
					convUnit->setLastLbDProfession(lastProfessionBefore);
					convUnit->setLastLbDProfessionBefore(lastProfession);
				}
			}
			// WTP, ray, saving 1 more Profession for Fisher Issue - END
		}

		// here little cheat for AI to cope with feature
		// profession change does not reset worked rounds
		else
		{
			//set the Profession of course
			convUnit->setLastLbDProfession(currentProfession);
			//we always simply add worked Turns
			convUnit->setLbDrounds(workedRounds + 1);
		}
	}

	// it is the same Profession
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

	calculatedChance = calculatedChance * GET_PLAYER(getOwnerINLINE()).getLearningByDoingModifier() / 100; // CivEffects - Nightinggale

	//ray Multiplayer Random Fix
	//int randomValue = rand() % 1000 + 1;
	int randomValue = GC.getGameINLINE().getSorenRandNum(1000, "LbD Expert City");

	// no Success if randomValue larger calculatedChance
	if (randomValue > calculatedChance)
	{
		return false;
	}

	FAssert(expertUnitType != NO_UNIT);
	//ray16
	OOS_LOG_3("Learning by doing", CvString(getName()).c_str(), getTypeStr(expertUnitType));
	CvUnit* expertUnit = GET_PLAYER(getOwnerINLINE()).initUnit(expertUnitType, NO_PROFESSION, getX_INLINE(), getY_INLINE(), convUnit->AI_getUnitAIType());
	FAssert(expertUnit != NULL);
	bool remove = removePopulationUnit(CREATE_ASSERT_DATA, convUnit, false, GC.getCivilizationInfo(GET_PLAYER(getOwnerINLINE()).getCivilizationType()).getDefaultProfession());
	FAssertMsg(remove, "Failed to remove unit!");
	(void)remove; // Silence cppcheck
	expertUnit->convert(convUnit, true);
	//addPopulationUnit(expertUnit, NO_PROFESSION);
	//AddMessage
	CvWString szBuffer = gDLL->getText("TXT_KEY_LBD_EXPERT_IN_CITY", getNameKey(), expertUnit->getUnitInfo().getDescription());
	//Ende ray16
	gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, expertUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), true, true);

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
	UnitClassTypes modcase = convUnit->getUnitInfo().getUnitClassType();

	// WTP, ray, LbD Slaves Revolt and Free - START
	// modified code to have specific Units for African Slaves and Native Slaves

	// store the UnitClass and modifier
	UnitTypes GeneratedUnitType;
	int mod;

	const CvPlayer& owner = GET_PLAYER(getOwnerINLINE());

	// convert Unit to Free Unit (Colonist, Freed Slave or Converted Native

	// if CRIMIAL or PRISONER of WAR general Default Pop-Unit, but lower chances
	if (modcase == GLOBAL_DEFINE_UNITCLASS_PRISONER || modcase == GLOBAL_DEFINE_UNITCLASS_PRISONER_OF_WAR)
	{
		mod = mod_crim; // Criminals are supposed to have a lower chance
		GeneratedUnitType = owner.getUnitType(GLOBAL_DEFINE_DEFAULT_POPULATION_UNIT);
	}

	// If African Slave generate a Freed Slave
	else if (modcase == GLOBAL_DEFINE_UNITCLASS_AFRICAN_SLAVE)
	{
		mod = mod_crim; // we just use the same lower chance as for Criminals
		GeneratedUnitType = owner.getUnitType(GLOBAL_DEFINE_UNITCLASS_FREED_SLAVE);
	}

	// If Native Slave generate a Converted Native
	else if (modcase == GLOBAL_DEFINE_UNITCLASS_NATIVE_SLAVE)
	{
		mod = mod_crim; // we just use the same lower chance as for Criminals
		GeneratedUnitType = owner.getUnitType(GLOBAL_DEFINE_UNITCLASS_CONVERTED_NATIVE);
	}

	//default case is servant (and potential others to be configured)
	else
	{
		mod = mod_serv; // Servants are supposed to have a higher chance
		GeneratedUnitType = owner.getUnitType(GLOBAL_DEFINE_DEFAULT_POPULATION_UNIT);
	}

	// WTP, ray, LbD Slaves Revolt and Free - END

	int calculatedChance = (base + (workedRounds - pre_rounds) * increase * l_level * mod);

	// WTP, ray, adding modifiers for other LBD features - START
	int iLearningByDoingFreeModifier = 0;
	for (TraitTypes eTrait = FIRST_TRAIT; eTrait < NUM_TRAIT_TYPES; ++eTrait)
	{
		if (hasTrait(eTrait))
		{
			iLearningByDoingFreeModifier = iLearningByDoingFreeModifier + GC.getTraitInfo(eTrait).getLearningByDoingFreeModifier();
		}
	}
	calculatedChance = calculatedChance * (100 + iLearningByDoingFreeModifier) / 100 ;
	// WTP, ray, adding modifiers for other LBD features - END

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
	OOS_LOG_3("Learning by doing (free)", CvString(getName()).c_str(), getTypeStr(GeneratedUnitType));
	CvUnit* GeneratedUnit = GET_PLAYER(getOwnerINLINE()).initUnit(GeneratedUnitType, NO_PROFESSION, getX_INLINE(), getY_INLINE(), convUnit->AI_getUnitAIType());
	FAssert(GeneratedUnit != NULL);
	bool remove = removePopulationUnit(CREATE_ASSERT_DATA, convUnit, false, GC.getCivilizationInfo(GET_PLAYER(getOwnerINLINE()).getCivilizationType()).getDefaultProfession());
	FAssertMsg(remove, "Failed to remove unit!");
	(void)remove; // Silence cppcheck
	GeneratedUnit->convert(convUnit, true);
	// AddMessage
	CvWString szBuffer = gDLL->getText("TXT_KEY_LBD_FREE_IN_CITY", getNameKey());
	//Ende ray16
	gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, GeneratedUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), true, true);

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
	UnitClassTypes modcase = convUnit->getUnitInfo().getUnitClassType();

	//default case is servant
	int mod = mod_serv;

	// if criminal or prisoner of war there is a bit higher chance of escaping
	if (modcase == GLOBAL_DEFINE_UNITCLASS_PRISONER || modcase == GLOBAL_DEFINE_UNITCLASS_PRISONER_OF_WAR)
	{
		mod = mod_crim;
	}

	// TODO: cases criminal or servant
	int calculatedChance = (base * mod);

	//WTP, ray, Slave Hunter and Slave Master - START
	int iSlaveRevoltReductionBonus = getSlaveRevoltReductionBonus();
	calculatedChance = calculatedChance * (100 - iSlaveRevoltReductionBonus) / 100;
	//WTP, ray, Slave Hunter and Slave Master - END

	// WTP, ray, adding modifiers for other LBD features - START
	int iLearningByDoingRunawayModifier = 0;
	for (TraitTypes eTrait = FIRST_TRAIT; eTrait < NUM_TRAIT_TYPES; ++eTrait)
	{
		if (hasTrait(eTrait))
		{
			iLearningByDoingRunawayModifier = iLearningByDoingRunawayModifier + GC.getTraitInfo(eTrait).getLearningByDoingRunawayModifier();
		}
	}
	calculatedChance = calculatedChance * (100 + iLearningByDoingRunawayModifier) / 100 ;
	// WTP, ray, adding modifiers for other LBD features - END

	//ray Multiplayer Random Fix
	//int randomValue = rand() % 1000 + 1;
	int randomValue = GC.getGameINLINE().getSorenRandNum(1000, "LbD Escape City");

	// no Success if randomValue larger calculatedChance
	if (randomValue > calculatedChance)
	{
		return false;
	}

	char const* buttonStringForMessage = convUnit->getButton();

	// with true, we greate Unit with Default AI, otherwise it is AI_Flee
	createFleeingUnit(convUnit->getUnitType(), false);

	//Unit is then simply destroyed
	bool remove = removePopulationUnit(CREATE_ASSERT_DATA, convUnit, false, GC.getCivilizationInfo(GET_PLAYER(getOwnerINLINE()).getCivilizationType()).getDefaultProfession());
	FAssertMsg(remove, "Failed to remove unit!");
	(void)remove; // Silence cppcheck
	convUnit->kill(false);

	// AddMessage
	//ray16
	CvWString szBuffer = gDLL->getText("TXT_KEY_LBD_ESCAPE", getNameKey());
	//Ende ray16
	gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, buttonStringForMessage, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), true, true);

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
	UnitClassTypes modcase = convUnit->getUnitInfo().getUnitClassType();

	//default case is Criminal - also if we ever have something else
	int mod;
	UnitTypes GeneratedUnitType;

	const CvPlayer& owner = GET_PLAYER(getOwnerINLINE());

	// if African Slave
	if (modcase == GLOBAL_DEFINE_UNITCLASS_AFRICAN_SLAVE)
	{
		mod = mod_slave;
		GeneratedUnitType = owner.getUnitType(GLOBAL_DEFINE_UNITCLASS_REVOLTING_SLAVE);
	}
	// if Native Slave
	else if (modcase == GLOBAL_DEFINE_UNITCLASS_NATIVE_SLAVE)
	{
		mod = mod_slave;
		GeneratedUnitType = owner.getUnitType(GLOBAL_DEFINE_UNITCLASS_REVOLTING_NATIVE_SLAVE);
	}
	// if Prisoner of War
	else if (modcase == GLOBAL_DEFINE_UNITCLASS_PRISONER_OF_WAR)
	{
		mod = mod_crim;
		GeneratedUnitType = owner.getUnitType(GLOBAL_DEFINE_UNITCLASS_REVOLTING_PRISONER_OF_WAR);
	}
	//default case is Criminal - also if we ever have something else
	else
	{
		mod = mod_crim;
		GeneratedUnitType = owner.getUnitType(GLOBAL_DEFINE_UNITCLASS_REVOLTING_CRIMINAL);
	}

	// get chance and random value
	int calculatedChance = (base * mod);

	//WTP, ray, Slave Hunter and Slave Master - START
	int iSlaveRevoltReductionBonus = getSlaveRevoltReductionBonus();
	calculatedChance = calculatedChance * (100 - iSlaveRevoltReductionBonus) / 100;
	//WTP, ray, Slave Hunter and Slave Master - END

	// WTP, ray, adding modifiers for other LBD features - START
	int iLearningByDoingRevoltModifier = 0;
	for (TraitTypes eTrait = FIRST_TRAIT; eTrait < NUM_TRAIT_TYPES; ++eTrait)
	{
		if (hasTrait(eTrait))
		{
			iLearningByDoingRevoltModifier = iLearningByDoingRevoltModifier + GC.getTraitInfo(eTrait).getLearningByDoingRevoltModifier();
		}
	}
	calculatedChance = calculatedChance * (100 + iLearningByDoingRevoltModifier) / 100 ;
	// WTP, ray, adding modifiers for other LBD features - END

	int randomValue = GC.getGameINLINE().getSorenRandNum(1000, "LbD Revolt Slave");

	// no Success if randomValue larger calculatedChance
	if (randomValue > calculatedChance)
	{
		return false;
	}

	// get Button of the Unit from UnitType for message
	// Test um Bug zu finden
	// CvUnit* GeneratedUnit = GET_PLAYER(getOwnerINLINE()).getUnit(GeneratedUnitType);
	// char const* buttonStringForMessage = GeneratedUnit->getButton();
	char const* buttonStringForMessage = convUnit->getButton();

	// with true, we greate Unit with Default AI, otherwise it is AI_Flee
	// Test um Bug zu finden
	createFleeingUnit(GeneratedUnitType, true);

	//Unit is then simply destroyed
	bool remove = removePopulationUnit(CREATE_ASSERT_DATA, convUnit, false, GC.getCivilizationInfo(GET_PLAYER(getOwnerINLINE()).getCivilizationType()).getDefaultProfession());
	FAssertMsg(remove, "Failed to remove unit!");
	(void)remove; // Silence cppcheck
	convUnit->kill(false);

	// AddMessage
	CvWString szBuffer = gDLL->getText("TXT_KEY_LBD_REVOLT", getNameKey());
	gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_MINOR_EVENT, buttonStringForMessage, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), true, true);

	return true;
}
// WTP, ray, LbD Slaves Revolt and Free - END

void CvCity::doLbD()
{
	//getting global values for formula
	int base_chance_expert = GC.getLBD_BASE_CHANCE_EXPERT();
	int chance_increase_expert = GC.getLBD_CHANCE_INCREASE_EXPERT();
	int pre_rounds_expert = GC.getLBD_PRE_ROUNDS_EXPERT();

	int chance_increase_expert_from_teacher = GC.getLBD_CHANCE_INCREASE_EXPERT_FROM_TEACHER(); // WTP, ray, teacher addon for LbD
	int pre_rounds_expert_decrease_from_teacher = GC.getLBD_PRE_ROUNDS_EXPERT_DECREASE_FROM_TEACHER();  // WTP, ray, teacher addon for LbD

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
	chance_increase_expert_from_teacher = chance_increase_expert_from_teacher * train_percent / 100;
	pre_rounds_expert_decrease_from_teacher = pre_rounds_expert_decrease_from_teacher * train_percent / 100;

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
		CvUnit* const pLoopUnit = m_aPopulationUnits[i];

		// only do something for this unit if the profession does use LbD
		if (pLoopUnit->getProfession()!= NO_PROFESSION && GC.getProfessionInfo(pLoopUnit->getProfession()).LbD_isUsed())
		{
			bool lbd_expert_successful = false;
			bool lbd_free_successful = false;
			bool lbd_escape_successful = false;

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
				// WTP, ray, teacher addon for LbD
				// adjusted method call with new teacher attributes
				lbd_expert_successful = LbD_try_become_expert(pLoopUnit, base_chance_expert, chance_increase_expert, pre_rounds_expert, learn_level, chance_increase_expert_from_teacher, pre_rounds_expert_decrease_from_teacher);
			}

			// try to become free if poosible
			if(!lbd_expert_successful && pLoopUnit->getUnitInfo().LbD_canGetFree())
			{
				lbd_free_successful = LbD_try_get_free(pLoopUnit, base_chance_free, chance_increase_free, pre_rounds_free, mod_free_criminal, mod_free_servant, learn_level);
			}

			// try to escape if free unsuccesful and escape possible
			if(!lbd_free_successful && !lbd_expert_successful && pLoopUnit->getUnitInfo().LbD_canEscape())
			{
				lbd_escape_successful = LbD_try_escape(pLoopUnit, base_chance_escape, mod_escape_criminal, mod_escape_servant);
			}

			// WTP, ray, LbD Slaves Revolt and Free - START
			// try to revolt if escape revolt possible and free unsuccessful and escape unsuccessufl
			if(!lbd_free_successful && !lbd_expert_successful && !lbd_escape_successful && pLoopUnit->getUnitInfo().LbD_canRevolt())
			{
				LbD_try_revolt(pLoopUnit, base_chance_revolt, mod_revolt_criminal, mod_revolt_slave);
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
		CLLNode<IDInfo>* pUnitNode = NULL;
		CLLNode<IDInfo>* pUnitNode2 = NULL;
		bool alreadyBombarded = false;
		bool bCanBomb = false;

		pUnitNode = pPlot->headUnitNode();

		while (pUnitNode && !alreadyBombarded)
		{
			pLoopUnit = plot()->getUnitNodeLoop(pUnitNode);

			if (pLoopUnit != NULL)
			{
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
									pLoopUnit2 = pAdjacentPlot->getUnitNodeLoop(pUnitNode2);

									if (pLoopUnit != NULL && (getTeam() != pLoopUnit2->getTeam() && (GET_TEAM(getTeam()).isAtWar(pLoopUnit2->getTeam()) || pLoopUnit2->getUnitInfo().isHiddenNationality()) && !pLoopUnit2->isCargo()))
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
													gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), true, true);

													szBuffer = gDLL->getText("TXT_KEY_FORTBOMB_SUNK_BAD", getNameKey());
													gDLL->UI().addPlayerMessage(pLoopUnit2->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, pLoopUnit2, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), true, true);
												}
												else
												{
													szBuffer = gDLL->getText("TXT_KEY_FORTBOMB_HIT_GOOD", getNameKey());
													gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), true, true);

													szBuffer = gDLL->getText("TXT_KEY_FORTBOMB_HIT_BAD", getNameKey());
													gDLL->UI().addPlayerMessage(pLoopUnit2->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, pLoopUnit2, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), true, true);
												}
											}
											else
											{
												szBuffer = gDLL->getText("TXT_KEY_FORTBOMB_MISS_BAD", getNameKey());
												gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), true, true);

												szBuffer = gDLL->getText("TXT_KEY_FORTBOMB_MISS_GOOD", getNameKey());
												gDLL->UI().addPlayerMessage(pLoopUnit2->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, pLoopUnit2, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), true, true);
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
													gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), true, true);

													szBuffer = gDLL->getText("TXT_KEY_FORTBOMB_DESTROYED_BAD", getNameKey());
													gDLL->UI().addPlayerMessage(pLoopUnit2->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, pLoopUnit2, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), true, true);
												}
												else
												{
													szBuffer = gDLL->getText("TXT_KEY_FORTBOMB_HIT_LAND_GOOD", getNameKey(), iDamage);
													gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), true, true);

													szBuffer = gDLL->getText("TXT_KEY_FORTBOMB_HIT_LAND_BAD", getNameKey(), iDamage);
													gDLL->UI().addPlayerMessage(pLoopUnit2->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, pLoopUnit2, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), true, true);
												}
											}
											else
											{
												szBuffer = gDLL->getText("TXT_KEY_FORTBOMB_MISS_LAND_BAD", getNameKey());
												gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), true, true);

												szBuffer = gDLL->getText("TXT_KEY_FORTBOMB_MISS_LAND_GOOD", getNameKey());
												gDLL->UI().addPlayerMessage(pLoopUnit2->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, pLoopUnit2, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), true, true);
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
	}


	// Do the Infantery Defense Attack
	if (getBuildingDefense() >= 60) // This is only true for Fort, Fortress and Stronghold.
	{
		CvWString szBuffer;
		CvPlot* pPlot = plot();
		CvPlot* pAdjacentPlot = NULL;
		CvUnit* pDefenseUnit = NULL;
		CvUnit* pLoopUnit = NULL;
		CvUnit* pLoopUnit2 = NULL;
		CLLNode<IDInfo>* pUnitNode = NULL;
		CLLNode<IDInfo>* pUnitNode2 = NULL;
		bool alreadyFired = false;
		bool bCanFire = false;

		pUnitNode = pPlot->headUnitNode();

		while (pUnitNode && !alreadyFired)
		{
			pLoopUnit = plot()->getUnitNodeLoop(pUnitNode);

			if (pLoopUnit != NULL)
			{
				bool isInfanteryType = false
					|| pLoopUnit->getUnitClassType() == GLOBAL_DEFINE_UNITCLASS_KING_REINFORCEMENT_LAND
					|| pLoopUnit->getUnitClassType() == GLOBAL_DEFINE_UNITCLASS_CONTINENTAL_GUARD
					|| (pLoopUnit->getProfession() != NO_PROFESSION && GC.getProfessionInfo(pLoopUnit->getProfession()).isCityDefender() && !GC.getProfessionInfo(pLoopUnit->getProfession()).isUnarmed());

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
						for (DirectionTypes eDirection = FIRST_DIRECTION; eDirection < NUM_DIRECTION_TYPES; ++eDirection)
						{
							pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), eDirection);
							//we do not fire on Water here
							if (pAdjacentPlot != NULL && !pAdjacentPlot->isWater())
							{
								pUnitNode2 = pAdjacentPlot->headUnitNode();
								while (pUnitNode2)
								{
									pLoopUnit2 = pAdjacentPlot->getUnitNodeLoop(pUnitNode2);

									if (pLoopUnit2 != NULL && getTeam() != pLoopUnit2->getTeam() && (GET_TEAM(getTeam()).isAtWar(pLoopUnit2->getTeam()) || pLoopUnit2->getUnitInfo().isHiddenNationality()) && !pLoopUnit2->isCargo())
									{
										int defenseRandNum = GC.getGameINLINE().getSorenRandNum(100, "City Defense Attack");
										int iDamage = 0;

										//10 percent chance to miss
										if (defenseRandNum > 10)
										{
											iDamage = pLoopUnit2->maxHitPoints()* defenseRandNum / 100;
											iDamage = iDamage * GLOBAL_DEFINE_DEFENSE_DAMAGE_MULTIPLIER;

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
													gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), true, true);

													szBuffer = gDLL->getText("TXT_KEY_FORTDEFENSE_DESTROYED_BAD", getNameKey());
													gDLL->UI().addPlayerMessage(pLoopUnit2->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, pLoopUnit2, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), true, true);
												}
												else
												{
													szBuffer = gDLL->getText("TXT_KEY_FORTDEFENSE_HIT_LAND_GOOD", getNameKey(), iDamage);
													gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), true, true);

													szBuffer = gDLL->getText("TXT_KEY_FORTDEFENSE_HIT_LAND_BAD", getNameKey(), iDamage);
													gDLL->UI().addPlayerMessage(pLoopUnit2->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, pLoopUnit2, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), true, true);
												}
											}
											else
											{
												szBuffer = gDLL->getText("TXT_KEY_FORTDEFENSE_MISS_LAND_BAD", getNameKey());
												gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), true, true);

												szBuffer = gDLL->getText("TXT_KEY_FORTDEFENSE_MISS_LAND_GOOD", getNameKey());
												gDLL->UI().addPlayerMessage(pLoopUnit2->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, pLoopUnit2, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), true, true);
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
	}

	return;
}
// R&R, ray, Extra City Defense Attacks - END


// WTP, ray fixing Domestic Demand Price issue
int CvCity::getYieldBuyPriceUnmodified(YieldTypes eYield) const
{
	FAssert(validEnumRange(eYield));
	return m_em_iYieldBuyPrice.get(eYield);

}

int CvCity::getYieldBuyPrice(YieldTypes eYield) const
{
	FAssert(validEnumRange(eYield));

	// WTP, ray Domestic Market Events - START
	// we modify the price in case there is a Market Event going on
	int iDomesticMarketEventModifier = getDomesticDemandEventPriceModifier();
	int iModifiedPrice = m_em_iYieldBuyPrice.get(eYield) * (100 + iDomesticMarketEventModifier) /  100;
	return iModifiedPrice;

}

// R&R, ray, adjustment Domestic Markets
// No messages, because too many messages get annoying
void CvCity::setYieldBuyPrice(YieldTypes eYield, int iPrice)
{
	FAssert(validEnumRange(eYield));

	iPrice = std::max(iPrice, 1);
	if (iPrice != getYieldBuyPriceUnmodified(eYield))
	{
		m_em_iYieldBuyPrice.set(eYield, iPrice);
	}
}

// R&R, ray, adjustment Domestic Markets
void CvCity::getYieldDemands(YieldCargoArray<int> &aYields) const
{
	if (this->getMarketModifier() == 0)
	{
		// everything will be 0. No need to calculate
		aYields.reset();
		return;
	}

	// Add building demands
	aYields.copy(getBuildingYieldDemands());

	// comment by ray: this here is just citizens
	// add unit demands
	for (uint i = 0; i < m_aPopulationUnits.size(); ++i)
	{
		CvUnit* pLoopUnit = m_aPopulationUnits[i];
		// reuse CivEffect cache code as it's essentially we same we need here: add a bunch of InfoArrays into one JIT array.
		aYields.addCache(1, GC.getUnitInfo(pLoopUnit->getUnitType()).getYieldDemands());
	}

	// WTP, ray, adjustments to Domestic Market to also consider Defenders on City Plot - START
	CvPlot* pCityCenterPlot = plot();
	CLLNode<IDInfo>* pUnitNode = pCityCenterPlot->headUnitNode();
	while (pUnitNode)
	{
		CvUnit* pLoopUnit = plot()->getUnitNodeLoop(pUnitNode);

		// however, we only consider Units that belong to the same Player as the City
		if (pLoopUnit != NULL && pLoopUnit->getOwnerINLINE() == getOwnerINLINE())
		{
			aYields.addCache(1, GC.getUnitInfo(pLoopUnit->getUnitType()).getYieldDemands());
		}
	}

	// WTP, ray, adjustments to Domestic Market to also consider Defenders on City Plot - END

	// apply market multiplier to each yield
	int iMarketModifier = this->getMarketModifier();

	// WTP, ray Domestic Market Events - START
	// here we add the event modifier on top, which may be positive or negative
	iMarketModifier = iMarketModifier + this->getDomesticDemandEventDemandModifier();
	// WTP, ray Domestic Market Events - END
	// for performance reasions, use getUnitYieldDemandTypes as it skips all yields no units/buildings will ever demand
	const InfoArray<YieldTypes>& kYieldArray = GC.getDomesticDemandYieldTypes();
	for (int i = 0; i < kYieldArray.getLength(); ++i)
	{
		YieldTypes eYield = kYieldArray.get(i);

		int iDemand = aYields.get(eYield);
		if (iDemand != 0) // skip calculating on something we know ends up as 0
		{
			// What goes on here looks significantly different from Androrc's original version, though it provides the same results.
			// original code:
			/// int iBuildingDemand = (iDemand * (MarketLevel * 50)) / 100; // 50 percent more demand per level
			/// return (iRawDemand + iBuildingDemand) / 100;

			// The current code essentially does the same. MarketLevel was 0-3 based on special building priority
			// while iMarketModifier is set in xml to be 100-250, in steps of 50
			// By starting from 100 instead of 0, iRawDemand + iBuildingDemand is no longer needed
			// The two divisions by 100 can then be combined into a single division of 100*100
			// The result is the same output, but around half the calculation time
			// Even better it puts the modifier in xml rather than some (for xml) hidden special building calculations

			iDemand *= iMarketModifier;
			iDemand /= 10000;
			aYields.set(iDemand, eYield);
		}
	}
}

int CvCity::getYieldDemand(YieldTypes eYield) const
{
	FAssert(validEnumRange(eYield));

	if (eYield >= NUM_CARGO_YIELD_TYPES)
	{
		// only cargo yields can have a demand
		// return 0 instead of calculating what we know will end up as 0
		return 0;
	}

	// Since the code can't calculate single yields anymore, generate the entire array.
	// Calls to this should be limited for performance reasons as setting the entire array in one call is much faster.
	// However this function avoid rewriting old code using the old system and reduce the focus to performance important code.
	// It's mainly the GUI, which keeps using the old code and it doesn't matter as long as it doesn't visibly lag.
	YieldCargoArray<int> aYields;
	getYieldDemands(aYields);
	return aYields.get(eYield);
}
// R&R, ray, adjustment Domestic Markets
// function completely rewritten by Nightinggale to fix extreme price bug
// needs to be checked for Special Cases like Luxury Goods
void CvCity::doPrices()
{
	// constants to determine when to change price
	// higher iPointsPerPriceDiff will make prices move more towards Europe prices
	// higher iPointsToTriggerPriceChange will increase the chance of no change
	// iDefaultPoints is how many points a yield has if there is no price diff
	// note if iDefaultPoints <= iPointsToTriggerPriceChange, then prices will only move towards Europe prices
	const int iPointsPerPriceDiff = 35;
	const int iPointsToTriggerPriceChange = 70;
	const int iDefaultPoints = 100;

	for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_CARGO_YIELD_TYPES; ++eYield)
	{
		const CvYieldInfo& kYield = GC.getYieldInfo(eYield);
		const CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());

		int iTargetPrice = kOwner.getYieldSellPrice(eYield);

		// todo: change price offset to an int in yield xml to make it configurable for each yield instead of hardcoding
		switch (eYield)
		{
		case YIELD_LUXURY_GOODS:
			// Luxury Goods should always give a little profit
			iTargetPrice += GLOBAL_DEFINE_PRICE_DIFF_EUROPE_DOMESTIC_LUXURY_GOODS;
			break;
		case YIELD_FIELD_WORKER_TOOLS:
		case YIELD_HOUSEHOLD_GOODS:
			// WTP, now also Fieldwoker Tools, but the Diff is only half
			// WTP, now also Household Goods, but the Diff is only half
			iTargetPrice += GLOBAL_DEFINE_PRICE_DIFF_EUROPE_DOMESTIC_LUXURY_GOODS / 2;
			break;
			// WTP, trying to fix issue that Domestic Market became unattractive
		default:
			iTargetPrice += GLOBAL_DEFINE_PRICE_DIFF_EUROPE_DOMESTIC_OTHER_GOODS;
			break;
		}

		const int iPriceDiff = iTargetPrice - getYieldBuyPriceUnmodified(eYield);

		int iMin = -iDefaultPoints;
		int iMax = iDefaultPoints;

		if (iPriceDiff > 0)
		{
			iMax += iPriceDiff * iPointsPerPriceDiff;
		}
		else
		{
			iMin += iPriceDiff * iPointsPerPriceDiff;
		}

		// get result in range iMin to iMax, both inclusive
		int iResult = GC.getGameINLINE().getSorenRandNum(iMax - iMin + 1, "Colony price change") + iMin;

		if (iResult >= iPointsToTriggerPriceChange)
		{
			setYieldBuyPrice(eYield, getYieldBuyPriceUnmodified(eYield) + 1);
		}
		else if (iResult <= -iPointsToTriggerPriceChange && kYield.getMinimumBuyPrice() < getYieldBuyPriceUnmodified(eYield))
		{
			setYieldBuyPrice(eYield, getYieldBuyPriceUnmodified(eYield) - 1);
		}
	}
}
//Androrc End

// R&R, ray, finishing Custom House Screen
void CvCity::setCustomHouseSellThreshold(YieldTypes eYield, int iCustomHouseSellThreshold)
{
	FAssert(validEnumRange(eYield));

	if (iCustomHouseSellThreshold != getCustomHouseSellThreshold(eYield))
	{
		m_em_iCustomHouseSellThreshold.set(eYield, iCustomHouseSellThreshold);
		if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		}
	}
}

int CvCity::getCustomHouseSellThreshold(YieldTypes eYield) const
{
	return m_em_iCustomHouseSellThreshold.get(eYield);
}

void CvCity::setCustomHouseNeverSell(YieldTypes eYield, bool bNeverSell)
{
	if (isCustomHouseNeverSell(eYield) != bNeverSell)
	{
		m_em_bCustomHouseNeverSell.set(eYield, bNeverSell);

		if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		}
	}
}

bool CvCity::isCustomHouseNeverSell(YieldTypes eYield) const
{
	return m_em_bCustomHouseNeverSell.get(eYield);
}
// R&R, ray, finishing Custom House Screen END

// R&R, ray, Fleeing Units - START
// WTP, ray, LbD Slaves Revolt and Free - START - adjusted to also have DefaultAI
void CvCity::createFleeingUnit(UnitTypes eUnit, bool bDefaultAI)
{
	if (GC.getGameINLINE().getBarbarianPlayer() == NO_PLAYER)
    {
        return;
    }

	CvPlayer& barbarianPlayer = GET_PLAYER(GC.getGameINLINE().getBarbarianPlayer());

	// find plot
	CvPlot* pBestPlot = NULL;
	int iBestValue = MAX_INT;

	for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);

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
		if (bDefaultAI == false)
		{
			barbarianPlayer.initUnit(eUnit, GC.getUnitInfo(eUnit).getDefaultProfession(), pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), UNITAI_FLEEING);
		}
		//Default AI
		else
		{
			barbarianPlayer.initUnit(eUnit, GC.getUnitInfo(eUnit).getDefaultProfession(), pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), NO_UNITAI);
		}
	}
}
// R&R, ray, Fleeing Units - END

// R&R, ray, Entertainment Buildings - START
void CvCity::doEntertainmentBuildings()
{
	int iCulturePerTurn = getCultureRate();
	int factorFromBuildingLevel = 0;
	BuildingTypes highestLevelEntertainmentBuilding = NO_BUILDING;

	// WTP, ray, refactored according to advice of Nightinggale
	for (BuildingTypes eBuilding = FIRST_BUILDING; eBuilding < NUM_BUILDING_TYPES; ++eBuilding)
	{
		if (isHasBuilding(eBuilding))
		{
			const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
			if (kBuilding.getEntertainmentGoldModifier() > factorFromBuildingLevel)
			{
				factorFromBuildingLevel = kBuilding.getEntertainmentGoldModifier();
				highestLevelEntertainmentBuilding = eBuilding;
			}
		}
	}

	int iGoldthroughCulture = iCulturePerTurn * factorFromBuildingLevel / 100; // now as defined in XML
	iGoldthroughCulture = iGoldthroughCulture * (100 + getCityHappiness() - getCityUnHappiness()) / 100; // WTP, ray, Happiness - START

	if (highestLevelEntertainmentBuilding != NO_BUILDING && iGoldthroughCulture > 0)
	{
		OOS_LOG_3("Entertainment building", CvString(getName()).c_str(), iGoldthroughCulture);
		GET_PLAYER(getOwnerINLINE()).changeGold(iGoldthroughCulture);
		CvWString szBuffer = gDLL->getText("TXT_KEY_GOLD_BY_ENTERTAINMENT", GC.getBuildingInfo(highestLevelEntertainmentBuilding).getDescription(), getNameKey(), iGoldthroughCulture);
		gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, coord(), NULL, MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), true, true);
	}
}
// R&R, ray, Entertainment Buildings - END

//WTP, ray, Slave Hunter and Slave Master - START

// reduced Revolting of slave workers
int CvCity::getSlaveRevoltReductionBonus() const
{
	if (isNative())
	{
		return 0;
	}

	int iSlaveRevoltReductionBonus = 0;
	int iMaxSlaveRevoltReductionBonusPerCity = GC.getMAX_SLAVE_REVOLT_REDUCTION_BONUS_PER_CITY();

	std::vector<CvUnit*> aUnits;
	CLLNode<IDInfo>* pUnitNode = plot()->headUnitNode();
	while (pUnitNode)
	{
		CvUnit* pLoopUnit = plot()->getUnitNodeLoop(pUnitNode);

		if (pLoopUnit != NULL && pLoopUnit->getOwnerINLINE() == getOwnerINLINE())
		{
			iSlaveRevoltReductionBonus += pLoopUnit->getSlaveRevoltReductionBonus();
		}
	}

	if (iSlaveRevoltReductionBonus > iMaxSlaveRevoltReductionBonusPerCity)
	{
		iSlaveRevoltReductionBonus = iMaxSlaveRevoltReductionBonusPerCity;
	}

	return iSlaveRevoltReductionBonus;
}

// increased Production of Slave workers
int CvCity::getSlaveWorkerProductionBonus() const
{
	return m_iSlaveWorkerProductionBonus;
}
//WTP, ray, Slave Hunter and Slave Master - END

// iBonus is set when a unit is about to enter the city since the unit will not be
// in the city plot yet
void CvCity::updateSlaveWorkerProductionBonus(int iBonus)
{
	if (isNative())
	{
		return;
	}

	int iSlaveWorkerProductionBonus = iBonus;
	int iMaxSlaveWorkerProductionBonusPerCity = GC.getMAX_SLAVE_WORKER_PRODUCTION_BONUS_PER_CITY();

	//std::vector<CvUnit*> aUnits;
	CLLNode<IDInfo>* pUnitNode = plot()->headUnitNode();
	while (pUnitNode)
	{
		CvUnit* pLoopUnit = plot()->getUnitNodeLoop(pUnitNode);

		if (pLoopUnit != NULL && pLoopUnit->getOwnerINLINE() == getOwnerINLINE())
		{
			iSlaveWorkerProductionBonus += pLoopUnit->getSlaveWorkerProductionBonus();
		}
	}

	if (iSlaveWorkerProductionBonus > iMaxSlaveWorkerProductionBonusPerCity)
	{
		iSlaveWorkerProductionBonus = iMaxSlaveWorkerProductionBonusPerCity;
	}

	m_iSlaveWorkerProductionBonus = iSlaveWorkerProductionBonus;
}
//WTP, ray, Slave Hunter and Slave Master - END


// WTP, ray, helper methods for Python Event System - Spawning Units and Barbarians on Plots - START
void CvCity::spawnOwnPlayerUnitOnPlotOfCity(int /*UnitClassTypes*/ iIndex) const
{
	CvPlayer& ownPlayer = GET_PLAYER(getOwnerINLINE());
	UnitTypes eUnitToSpawn = (UnitTypes)GC.getCivilizationInfo(ownPlayer.getCivilizationType()).getCivilizationUnits(iIndex);
	if (eUnitToSpawn != NO_UNIT)
	{
		OOS_LOG_3("spawnOwnPlayerUnitOnPlotOfCity", CvString(getName()).c_str(), getTypeStr(eUnitToSpawn));
		ownPlayer.initUnit(eUnitToSpawn, GC.getUnitInfo(eUnitToSpawn).getDefaultProfession(), getX_INLINE(), getY_INLINE(), NO_UNITAI);
	}
	return;
}

// careful with this, will take over City for Barbarians
void CvCity::spawnBarbarianUnitOnPlotOfCity(int /*UnitClassTypes*/ iIndex) const
{
	PlayerTypes eBarbarianPlayerType = GC.getGameINLINE().getBarbarianPlayer();
	if (eBarbarianPlayerType == NO_PLAYER)
    {
        return;
    }

	CvPlayer& barbarianPlayer = GET_PLAYER(eBarbarianPlayerType);
	UnitTypes eUnitToSpawn = (UnitTypes)GC.getCivilizationInfo(barbarianPlayer.getCivilizationType()).getCivilizationUnits(iIndex);
	if (eUnitToSpawn != NO_UNIT)
	{
		barbarianPlayer.initUnit(eUnitToSpawn, GC.getUnitInfo(eUnitToSpawn).getDefaultProfession(), getX_INLINE(), getY_INLINE(), NO_UNITAI);
	}
	return;
}

void CvCity::spawnOwnPlayerUnitOnAdjacentPlotOfCity(int /*UnitClassTypes*/ iIndex) const
{
	CvPlayer& ownPlayer = GET_PLAYER(getOwnerINLINE());
	UnitTypes eUnitToSpawn = (UnitTypes)GC.getCivilizationInfo(ownPlayer.getCivilizationType()).getCivilizationUnits(iIndex);
	if (eUnitToSpawn != NO_UNIT)
	{
		// we use this as last fallback if we do not find an adjacent plot below
		CvPlot* pPlotToSpawn = plot();

		// try to find a better adjacent plot
		for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			CvPlot* pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));
			if (pAdjacentPlot != NULL)
			{
				// if the adjacent Plot is valid and there are no other Units, prevent Cities for safety reasons
				if (pAdjacentPlot->isValidDomainForAction(eUnitToSpawn) && pAdjacentPlot->getNumUnits() == 0 && !pAdjacentPlot->isCity())
				{
					// we found a proper fallback solution and use it as spawning plot
					pPlotToSpawn = pAdjacentPlot;
					break;
				}
			}
		}
		// now we spawn and are done
		ownPlayer.initUnit(eUnitToSpawn, GC.getUnitInfo(eUnitToSpawn).getDefaultProfession(), pPlotToSpawn->getX_INLINE(), pPlotToSpawn->getY_INLINE(), NO_UNITAI);
	}
	return;
}

void CvCity::spawnBarbarianUnitOnAdjacentPlotOfCity(int /*UnitClassTypes*/ iIndex) const
{
	PlayerTypes eBarbarianPlayerType = GC.getGameINLINE().getBarbarianPlayer();
	if (eBarbarianPlayerType == NO_PLAYER)
    {
        return;
    }

	CvPlayer& barbarianPlayer = GET_PLAYER(eBarbarianPlayerType);
	UnitTypes eUnitToSpawn = (UnitTypes)GC.getCivilizationInfo(barbarianPlayer.getCivilizationType()).getCivilizationUnits(iIndex);
	if (eUnitToSpawn != NO_UNIT)
	{
		// we use this as last fallback belok
		CvPlot* pPlotToSpawn = plot();

		// try to find a better adjacent plot
		for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			CvPlot* pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));
			if (pAdjacentPlot != NULL)
			{
				// if the adjacent Plot is valid and there are no other Units, prevent Cities for safety reasons
				if (pAdjacentPlot->isValidDomainForAction(eUnitToSpawn) && pAdjacentPlot->getNumUnits() == 0 && !pAdjacentPlot->isCity())
				{
					// we found a proper fallback solution and use it as spawning plot
					pPlotToSpawn = pAdjacentPlot;
					break;
				}
			}
		}

		// now we spawn and are done
		barbarianPlayer.initUnit(eUnitToSpawn, GC.getUnitInfo(eUnitToSpawn).getDefaultProfession(), pPlotToSpawn->getX_INLINE(), pPlotToSpawn->getY_INLINE(), NO_UNITAI);
	}
	return;
}

bool CvCity::isOwnPlayerUnitOnAdjacentPlotOfCity(int /*UnitClassTypes*/ iIndex) const
{
	PlayerTypes eOwnPlayerType = getOwnerINLINE();
	CvPlayer& ownPlayer = GET_PLAYER(getOwnerINLINE());
	UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(ownPlayer.getCivilizationType()).getCivilizationUnits(iIndex);
	if (eUnit != NO_UNIT)
	{
		// we check the adjacent Plots
		for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			CvPlot* pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));
			if (pAdjacentPlot != NULL)
			{
				// if the adjacent Plot is valid and there are no other Units, prevent Cities for safety reasons
				CLLNode<IDInfo>* pUnitNode = pAdjacentPlot->headUnitNode();
				while (pUnitNode)
				{
					CvUnit* pLoopUnit = pAdjacentPlot->getUnitNodeLoop(pUnitNode);

					// check for owner and UnitType
					if (pLoopUnit != NULL && pLoopUnit->getOwnerINLINE() == eOwnPlayerType && pLoopUnit->getUnitType() == eUnit)
					{
						// we found a unit of our player;
						return true;
					}
				}
			}
		}
	}
	// nothing found, return false
	return false;
}

bool CvCity::isBarbarianUnitOnAdjacentPlotOfCity(int /*UnitClassTypes*/ iIndex) const
{
	PlayerTypes eBarbarianPlayerType = GC.getGameINLINE().getBarbarianPlayer();
	if (eBarbarianPlayerType == NO_PLAYER)
    {
        return false;
    }

	CvPlayer& barbarianPlayer = GET_PLAYER(eBarbarianPlayerType);
	UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(barbarianPlayer.getCivilizationType()).getCivilizationUnits(iIndex);
	if (eUnit != NO_UNIT)
	{
		// we check the adjacent Plots
		for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			CvPlot* pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), ((DirectionTypes)iI));
			if (pAdjacentPlot != NULL)
			{
				// if the adjacent Plot is valid and there are no other Units, prevent Cities for safety reasons
				CLLNode<IDInfo>* pUnitNode = pAdjacentPlot->headUnitNode();
				while (pUnitNode)
				{
					CvUnit* pLoopUnit = pAdjacentPlot->getUnitNodeLoop(pUnitNode);

					// check for owner and UnitType
					if (pLoopUnit != NULL && pLoopUnit->getOwnerINLINE() == eBarbarianPlayerType && pLoopUnit->getUnitType() == eUnit)
					{
						// we found a unit of our player;
						return true;
					}
				}
			}
		}
	}
	// nothing found, return false
	return false;
}
// WTP, ray, helper methods for Python Event System - Spawning Units and Barbarians on Plots - END

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
	FOREACH(CityPlot)
	{
		CvPlot* pLoopPlot = getCityIndexPlot(eLoopCityPlot);

		// only work on plots owned by the city owner
		// this is needed when loading savegames as not all players will be loaded at this point
		if (pLoopPlot != NULL && pLoopPlot->getOwnerINLINE() == getOwnerINLINE())
		{
			pLoopPlot->setYieldCache();
		}
	}
	// CvPlot::hasYield cache - end - Nightinggale

	m_iCacheMarketModifier = 0;
	const CvPlayerAI &kPlayer = GET_PLAYER(getOwnerINLINE());
	if (kPlayer.canUseDomesticMarket())
	{
		m_ja_iBuildingYieldDemands.reset();
		const InfoArray<BuildingTypes>& kBuildingArray = kPlayer.getAllowedBuildings();
		const int iNumBuildings = kBuildingArray.getLength();
		for (int i = 0; i < iNumBuildings; ++i)
		{
			const BuildingTypes eBuilding = kBuildingArray.get(i);
			if (isHasBuilding(eBuilding))
			{
				const CvBuildingInfo& kInfo = GC.getBuildingInfo(eBuilding);
				m_ja_iBuildingYieldDemands.addCache(1, kInfo.getYieldDemands());
				m_iCacheMarketModifier += kInfo.getDomesticMarketModifier();
			}
		}
	}
	setBarrackHarbourCache();
}
// building affected cache - end - Nightinggale

void CvCity::writeDesyncLog(FILE *f) const
{
	fprintf(f, "\t\tCoordinates: (%d, %d)\n", getX_INLINE(), getY_INLINE());
	fprintf(f, "\t\tPopulation: %d\n", getPopulation());

	fprintf(f, "\t\tHappiness: %d\n", getCityHappiness());
	fprintf(f, "\t\tUnhappiness: %d\n", getCityUnHappiness());
	fprintf(f, "\t\tHealth: %d\n", getCityHealth());

	fprintf(f, "\t\tCulture level: %d\n", getCultureLevel());
	fprintf(f, "\t\tCulture rate: %d\n", getCultureRate());
	fprintf(f, "\t\tCulture threshold: %d\n", getCultureThreshold());
	fprintf(f, "\t\tCulture update timer: %d\n", getCultureUpdateTimer());

	for (PlayerTypes ePlayer = FIRST_PLAYER; ePlayer < NUM_PLAYER_TYPES; ++ePlayer)
	{
		int iCulture = getCulture(ePlayer);
		if (iCulture > 0)
		{
			fprintf(f, "\t\tCulture player %d: %d\n", ePlayer, iCulture);
		}
	}

	fprintf(f, "\t\tWarehouse capacity: %d\n", getMaxYieldCapacity());

	for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
	{
		int iNum = getYieldStored(eYield);
		if (iNum != 0)
		{
			fprintf(f, "\t\t\t%S: %d\n", GC.getYieldInfo(eYield).getDescription(), iNum);
		}
	}
}

void CvCity::doOppressometerGrowth()
{
	if (isOccupation() || isNative())
	{
		// for now, don't run this for natives, but might be changed later if natives are made playable one day ;)
		return;
	}

	const int iOldOppressometer = getOppressometer();

	const int iSlaveWorkerProductionBonus = getSlaveWorkerProductionBonus();
	FOREACH(CityPlot)
	{
		CvPlot* pPlot = getCityIndexPlot(eLoopCityPlot);
		if (pPlot != NULL && isPlotProducingYields(eLoopCityPlot))
		{
			CvUnit* const pUnit = getUnitWorkingPlot(pPlot);
			if (pUnit != NULL)
			{
				const int iForcedLaborFactor = pUnit->getForcedLaborFactor();
				if (iForcedLaborFactor > 0)
				{
					for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; eYield++)
					{
						int iOppressometerGrowth = pPlot->getYield(eYield);

						if (iOppressometerGrowth > 0)
						{
							// forcedLaborFactor == 2 -> apply complete modifier, discriminationFactor == 1 -> apply 1/2 modifier (for indentured etc.)
							iOppressometerGrowth *= 100 + ((GET_PLAYER(getOwnerINLINE()).getOppressometerForcedLaborModifier() * iForcedLaborFactor) / 2);
							iOppressometerGrowth /= 100;

							iOppressometerGrowth *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getOppressometerGrowthHandicap();
							iOppressometerGrowth /= 100;

							const int iDiscriminationFactor = pUnit->getDiscriminationFactor();
							if (iDiscriminationFactor > 0)
							{
								// discriminationFactor == 2 -> apply complete modifier, discriminationFactor == 1 -> apply 1/2 modifier
								iOppressometerGrowth *= 100 + ((GET_PLAYER(getOwnerINLINE()).getOppressometerDiscriminationModifier() * iDiscriminationFactor) / 2);
								iOppressometerGrowth /= 100;
							}

							if (iSlaveWorkerProductionBonus > 0 && pUnit->getUnitInfo().getYieldChange(eYield) > 0)
							{
								// if slaves are pushed more by a slave master, they will suffer even more oppression
								iOppressometerGrowth *= (100 + iSlaveWorkerProductionBonus * 2);
								iOppressometerGrowth /= 100;
							}

							// normalize Oppressometer growth by game speed
							iOppressometerGrowth *= 100;
							iOppressometerGrowth /= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();

							growOppressometer(iOppressometerGrowth);
						}
					}
				}
			}
		}
	}
	// debug message - delete later!
	// gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_OPPRESSOMETER_GROWTH_CITY", getNameKey(), iOldOppressometer, getOppressometer()), coord(), "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_INFO);
}

void CvCity::changeOppressometer(int iChange)
{
	m_iOppressometer += iChange;
	FAssert(m_iOppressometer >= 0);
}

void CvCity::growOppressometer(int iChange)
{
// add to oppressometer modified by Oppressometer growth modifier
	FAssert(iChange >= 0);
	const int iEffectiveChange = (iChange * getOppressometerGrowthModifier()) / 100;
	changeOppressometer(iEffectiveChange);
}

void CvCity::changeOppressometerGrowthModifier(int iChange)
{
	m_iOppressometerGrowthModifier += iChange;
	FAssert(m_iOppressometerGrowthModifier > 0);
}

void CvCity::doOppressometerDecay()
{
	int iOppressometerDecayRate = GLOBAL_DEFINE_OPPRESSOMETER_DECAY_RATE_BASE; // (percent value)
	// start with a flat decay, later this could have modifiers
	// e.g. high percentage of non-free citizens should decrease decay rate
	// or FF with some change to it

	// normalize Oppressometer decay by game speed
	iOppressometerDecayRate *= 100;
	iOppressometerDecayRate /= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();

	int iOppressometerDecay = (getOppressometer() * iOppressometerDecayRate) / 100;
	if (iOppressometerDecay < 1 && getOppressometer() > 0)
	{
		iOppressometerDecay = 1; // always decay at least a bit
	}

	changeOppressometer(-iOppressometerDecay);
}

// WTP, ray, Center Plot specific Backgrounds - Start
TerrainTypes CvCity::getCenterPlotTerrainType() const
{
	CvPlot* pCityCenterPlot = plot();
	if (pCityCenterPlot != NULL)
	{
		return pCityCenterPlot->getTerrainType();
	}
	else
	{
		return NO_TERRAIN;
	}
}
// WTP, ray, Center Plot specific Backgrounds - END

