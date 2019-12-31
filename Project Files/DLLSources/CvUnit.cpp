// unit.cpp

#include "CvGameCoreDLL.h"
#include "CvUnit.h"
#include "CvArea.h"
#include "CvPlot.h"
#include "CvCity.h"
#include "CvGlobals.h"
#include "CvGameCoreUtils.h"
#include "CvGameAI.h"
#include "CvMap.h"
#include "CvPlayerAI.h"
#include "CvRandom.h"
#include "CvTeamAI.h"
#include "CvGameCoreUtils.h"
#include "CyUnit.h"
#include "CyArgsList.h"
#include "CyPlot.h"
#include "CvDLLEntityIFaceBase.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLEngineIFaceBase.h"
#include "CvDLLEventReporterIFaceBase.h"
#include "CvDLLPythonIFaceBase.h"
#include "CvDLLFAStarIFaceBase.h"
#include "CvInfos.h"
#include "FProfiler.h"
#include "CvPopupInfo.h"
#include "CvArtFileMgr.h"
#include "CvDiploParameters.h"
#include "CvTradeRoute.h"

// Public Functions...

CvUnitTemporaryStrengthModifier::CvUnitTemporaryStrengthModifier(CvUnit* pUnit, ProfessionTypes eProfession) :
	m_pUnit(pUnit),
	m_eProfession(eProfession)
{
	if (m_pUnit != NULL)
	{
		m_pUnit->processProfessionStats(m_pUnit->getProfession(), -1);
		m_pUnit->processProfessionStats(m_eProfession, 1);
	}
}

CvUnitTemporaryStrengthModifier::~CvUnitTemporaryStrengthModifier()
{
	if (m_pUnit != NULL)
	{
		m_pUnit->processProfessionStats(m_eProfession, -1);
		m_pUnit->processProfessionStats(m_pUnit->getProfession(), 1);
	}
}


CvUnit::CvUnit() :
	m_pabHasRealPromotion(NULL),
	m_paiFreePromotionCount(NULL),
	m_paiTerrainDoubleMoveCount(NULL),
	m_paiFeatureDoubleMoveCount(NULL),
	m_paiExtraTerrainAttackPercent(NULL),
	m_paiExtraTerrainDefensePercent(NULL),
	m_paiExtraFeatureAttackPercent(NULL),
	m_paiExtraFeatureDefensePercent(NULL),
	m_paiExtraUnitCombatModifier(NULL),
	m_paiExtraUnitClassAttackModifier(NULL),
	m_paiExtraUnitClassDefenseModifier(NULL),
	m_eUnitType(NO_UNIT),
	m_iID(-1),

	// TAC - LbD - Ray - START
	m_LbDrounds(0),
	m_lastProfession(NO_PROFESSION),
	// TAC - LbD - Ray - END

	//ray18
	moneyToBuyLand(0),
	playerToBuyLand(NO_PLAYER),
	//ray18 End

	// unit yield cache - start - Nightinggale
	m_eCachedYield(NO_YIELD),
	// unit yield cache - end - Nightinggale

	// R&R, ray, Natives Trading - START
	m_AmountForNativeTrade(0),
	m_YieldForNativeTrade(NO_YIELD)
	// R&R, ray, Natives Trading - END

{
	m_aiExtraDomainModifier = new int[NUM_DOMAIN_TYPES];


	CvDLLEntity::createUnitEntity(this);		// create and attach entity to unit

	reset(0, NO_UNIT, NO_PLAYER, true);
}


CvUnit::~CvUnit()
{
	if (!gDLL->GetDone() && GC.IsGraphicsInitialized())						// don't need to remove entity when the app is shutting down, or crash can occur
	{
		gDLL->getEntityIFace()->RemoveUnitFromBattle(this);
		CvDLLEntity::removeEntity();		// remove entity from engine
	}

	CvDLLEntity::destroyEntity();			// delete CvUnitEntity and detach from us

	uninit();

	SAFE_DELETE_ARRAY(m_aiExtraDomainModifier);
}

void CvUnit::reloadEntity()
{
	//has not been initialized so don't reload
	if(!gDLL->getEntityIFace()->isInitialized(getEntity()))
	{
		return;
	}

	bool bSelected = IsSelected();

	//destroy old entity
	if (!gDLL->GetDone() && GC.IsGraphicsInitialized()) // don't need to remove entity when the app is shutting down, or crash can occur
	{
		gDLL->getEntityIFace()->RemoveUnitFromBattle(this);
		CvDLLEntity::removeEntity();		// remove entity from engine
	}

	CvDLLEntity::destroyEntity();			// delete CvUnitEntity and detach from us

	//creat new one
	CvDLLEntity::createUnitEntity(this);		// create and attach entity to unit
	setupGraphical();
	if (bSelected)
	{
		gDLL->getInterfaceIFace()->insertIntoSelectionList(this, false, false);
	}
}


void CvUnit::init(int iID, UnitTypes eUnit, ProfessionTypes eProfession, UnitAITypes eUnitAI, PlayerTypes eOwner, int iX, int iY, DirectionTypes eFacingDirection, int iYieldStored)
{
	CvWString szBuffer;
	int iUnitName;
	int iI, iJ;

	FAssert(NO_UNIT != eUnit);

	//--------------------------------
	// Init saved data
	reset(iID, eUnit, eOwner);

	m_iYieldStored = iYieldStored;
	m_eFacingDirection = eFacingDirection;
	if(m_eFacingDirection == NO_DIRECTION)
	{
		CvPlot* pPlot = GC.getMapINLINE().plotINLINE(iX, iY);
		if((pPlot != NULL) && pPlot->isWater() && (getDomainType() == DOMAIN_SEA))
		{
			m_eFacingDirection = (DirectionTypes) GC.getDefineINT("WATER_UNIT_FACING_DIRECTION");
		}
		else
		{
			m_eFacingDirection = DIRECTION_SOUTH;
		}
	}

	// TAC - Great General Names - Ray - START
	//Neuer Code
	//identifiziere General anhand UnitClass
	if((UnitClassTypes) getUnitClassType() == GC.getDefineINT("UNITCLASS_GREAT_GENERAL")) 
	{
		//iUnitName = GC.getGameINLINE().getUnitCreatedCount(getUnitType());
		//Fix to few General Names
		iUnitName = GET_PLAYER(getOwnerINLINE()).getUnitClassCount((UnitClassTypes) getUnitClassType());

		int maxNumberGenerals = GC.getCivilizationInfo(getCivilizationType()).getNumGeneralNames();
	
		if (iUnitName < maxNumberGenerals){

			int iOffset = GC.getGameINLINE().getSorenRandNum(maxNumberGenerals, "Unit name selection");
			
			for (iI = 0; iI < maxNumberGenerals; iI++)
			{
				int iIndex = (iI + iOffset) % maxNumberGenerals;
				CvWString szName = CvWString(GC.getCivilizationInfo(getCivilizationType()).getGeneralNames(iIndex));

				if (!GC.getGameINLINE().isGreatGeneralBorn(szName))
				{
					setName(szName);
					GC.getGameINLINE().addGreatGeneralBornName(szName);
					break;
				}

			}
		}
	}

	// R&R, ray, Great Admirals - START
	//Neuer Code
	//identifiziere General anhand UnitClass
	else if((UnitClassTypes) getUnitClassType() == GC.getDefineINT("UNITCLASS_GREAT_ADMIRAL")) 
	{
		//iUnitName = GC.getGameINLINE().getUnitCreatedCount(getUnitType());
		//Fix to few Admiral Names
		iUnitName = GET_PLAYER(getOwnerINLINE()).getUnitClassCount((UnitClassTypes) getUnitClassType());

		int maxNumberAdmirals = GC.getCivilizationInfo(getCivilizationType()).getNumAdmiralNames();
	
		if (iUnitName < maxNumberAdmirals){

			int iOffset = GC.getGameINLINE().getSorenRandNum(maxNumberAdmirals, "Unit name selection");
			
			for (iI = 0; iI < maxNumberAdmirals; iI++)
			{
				int iIndex = (iI + iOffset) % maxNumberAdmirals;
				CvWString szName = CvWString(GC.getCivilizationInfo(getCivilizationType()).getAdmiralNames(iIndex));

				if (!GC.getGameINLINE().isGreatAdmiralBorn(szName))
				{
					setName(szName);
					GC.getGameINLINE().addGreatAdmiralBornName(szName);
					break;
				}

			}
		}
	}

	// TAC - Ship Names - Ray - START
	//Neuer Code
	//identifiziere Shiffe anhand Domain - Kaperschiffe ausgeschlossen
	else if(GC.getUnitInfo(getUnitType()).getDomainType() == DOMAIN_SEA && !GC.getUnitInfo(getUnitType()).isHiddenNationality()) {
		iUnitName = GC.getGameINLINE().getUnitCreatedCount(getUnitType());
		int maxNumberShips = GC.getCivilizationInfo(getCivilizationType()).getNumShipNames();
	
		if (iUnitName < maxNumberShips){

			int iOffset = GC.getGameINLINE().getSorenRandNum(maxNumberShips, "Unit name selection");
			
			for (iI = 0; iI < maxNumberShips; iI++)
			{
				int iIndex = (iI + iOffset) % maxNumberShips;
				CvWString szName = CvWString(GC.getCivilizationInfo(getCivilizationType()).getShipNames(iIndex));

				if (!GC.getGameINLINE().isShipNamed(szName))
				{
					setName(szName);
					GC.getGameINLINE().addShipName(szName);
					break;
				}

			}
		}
	}
	// Alter Code nur einfach in else
	else {
		iUnitName = GC.getGameINLINE().getUnitCreatedCount(getUnitType());
		int iNumNames = m_pUnitInfo->getNumUnitNames();
		if (iUnitName < iNumNames)
		{
			int iOffset = GC.getGameINLINE().getSorenRandNum(iNumNames, "Unit name selection");

			for (iI = 0; iI < iNumNames; iI++)
			{
				int iIndex = (iI + iOffset) % iNumNames;
				CvWString szName = gDLL->getText(m_pUnitInfo->getUnitNames(iIndex));
				if (!GC.getGameINLINE().isGreatGeneralBorn(szName))
				{
					setName(szName);
					GC.getGameINLINE().addGreatGeneralBornName(szName);
					break;
				}
			}
		}
	}
	
	// TAC - Great General Names - Ray - END

	setGameTurnCreated(GC.getGameINLINE().getGameTurn());

	GC.getGameINLINE().incrementUnitCreatedCount(getUnitType());
	GC.getGameINLINE().incrementUnitClassCreatedCount((UnitClassTypes)(m_pUnitInfo->getUnitClassType()));

	updateOwnerCache(1);

	for (iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		if (m_pUnitInfo->getFreePromotions(iI))
		{
			setHasRealPromotion(((PromotionTypes)iI), true);
		}
	}

	if (NO_UNITCLASS != getUnitClassType())
	{
		for (iJ = 0; iJ < GC.getNumPromotionInfos(); iJ++)
		{
			if (GET_PLAYER(getOwnerINLINE()).isFreePromotion(getUnitClassType(), (PromotionTypes)iJ))
			{
				changeFreePromotionCount(((PromotionTypes)iJ), 1);
			}
		}
	}

	processUnitCombatType((UnitCombatTypes) m_pUnitInfo->getUnitCombatType(), 1);

	updateBestLandCombat();

	AI_init();

	setProfession(eProfession);

	if (isNative() || GET_PLAYER(getOwnerINLINE()).isEurope())
	{
		std::vector<int> aiPromo(GC.getNumPromotionInfos(), 0);
		for (iI = 0; iI < GC.getNumPromotionInfos(); iI++)
		{
			aiPromo[iI] = iI;
		}
		GC.getGameINLINE().getSorenRand().shuffleArray(aiPromo, NULL);

		for (iI = 0; iI < GC.getNumPromotionInfos(); iI++)
		{
			PromotionTypes eLoopPromotion = (PromotionTypes)aiPromo[iI];
			if (canAcquirePromotion(eLoopPromotion))
			{
				if (GC.getPromotionInfo(eLoopPromotion).getPrereqPromotion() != NO_PROMOTION || GC.getPromotionInfo(eLoopPromotion).getPrereqOrPromotion1() != NO_PROMOTION)
				{
					if (GC.getGameINLINE().getSorenRandNum(100, "AI free native/europe promotion") < 25)
					{
						setHasRealPromotion(eLoopPromotion, true);
						break;
					}
				}
			}
		}
	}

	addToMap(iX, iY);
	AI_setUnitAIType(eUnitAI);

	gDLL->getEventReporterIFace()->unitCreated(this);

	FAssert(GET_PLAYER(getOwnerINLINE()).checkPopulation());
}


void CvUnit::uninit()
{
	SAFE_DELETE_ARRAY(m_pabHasRealPromotion);
	SAFE_DELETE_ARRAY(m_paiFreePromotionCount);
	SAFE_DELETE_ARRAY(m_paiTerrainDoubleMoveCount);
	SAFE_DELETE_ARRAY(m_paiFeatureDoubleMoveCount);
	SAFE_DELETE_ARRAY(m_paiExtraTerrainAttackPercent);
	SAFE_DELETE_ARRAY(m_paiExtraTerrainDefensePercent);
	SAFE_DELETE_ARRAY(m_paiExtraFeatureAttackPercent);
	SAFE_DELETE_ARRAY(m_paiExtraFeatureDefensePercent);
	SAFE_DELETE_ARRAY(m_paiExtraUnitClassAttackModifier);
	SAFE_DELETE_ARRAY(m_paiExtraUnitClassDefenseModifier);
	SAFE_DELETE_ARRAY(m_paiExtraUnitCombatModifier);
}


// FUNCTION: reset()
// Initializes data members that are serialized.
void CvUnit::reset(int iID, UnitTypes eUnit, PlayerTypes eOwner, bool bConstructorCall)
{
	int iI;

	//--------------------------------
	// Uninit class
	uninit();

	m_iID = iID;
	// TAC - LbD - Ray - START
	m_LbDrounds = 0 ;
	m_lastProfession = NO_PROFESSION;
	// TAC - LbD - Ray - END

	//ray18
	moneyToBuyLand = 0;
	playerToBuyLand = NO_PLAYER;
	//Ende ray18

	// R&R, ray, Natives Trading - START
	m_AmountForNativeTrade = 0;
	m_YieldForNativeTrade = NO_YIELD;
	// R&R, ray, Natives Trading - END

	m_iGroupID = FFreeList::INVALID_INDEX;
	m_iHotKeyNumber = -1;
	m_iX = INVALID_PLOT_COORD;
	m_iY = INVALID_PLOT_COORD;
	m_iLastMoveTurn = 0;
	m_iGameTurnCreated = 0;
	m_iDamage = 0;
	m_iMoves = 0;
	m_iExperience = 0;
	m_iLevel = 1;
	m_iCargo = 0;
	m_iAttackPlotX = INVALID_PLOT_COORD;
	m_iAttackPlotY = INVALID_PLOT_COORD;
	m_iCombatTimer = 0;
	m_iCombatDamage = 0;
	m_iFortifyTurns = 0;
	m_iBlitzCount = 0;
	m_iAmphibCount = 0;
	m_iRiverCount = 0;
	m_iEnemyRouteCount = 0;
	m_iAlwaysHealCount = 0;
	m_iHillsDoubleMoveCount = 0;
	m_iExtraVisibilityRange = 0;
	m_iExtraMoves = 0;
	m_iExtraMoveDiscount = 0;
	m_iExtraWithdrawal = 0;
	m_iExtraBombardRate = 0;
	m_iExtraEnemyHeal = 0;
	m_iExtraNeutralHeal = 0;
	m_iExtraFriendlyHeal = 0;
	m_iSameTileHeal = 0;
	m_iAdjacentTileHeal = 0;
	m_iExtraCombatPercent = 0;
	m_iExtraCityAttackPercent = 0;
	m_iExtraCityDefensePercent = 0;
	m_iExtraHillsAttackPercent = 0;
	m_iExtraHillsDefensePercent = 0;
	m_iExtraDomesticBonusPercent = 0;			
	m_iPillageChange = 0;
	m_iUpgradeDiscount = 0;
	m_iExperiencePercent = 0;
	m_eFacingDirection = DIRECTION_SOUTH;
	m_iImmobileTimer = 0;
	m_iYieldStored = 0;
	m_iExtraWorkRate = 0;
	m_iUnitTravelTimer = 0;
	m_iBadCityDefenderCount = 0;
	m_iUnarmedCount = 0;

	m_bMadeAttack = false;
	m_bPromotionReady = false;
	m_bDeathDelay = false;
	m_bCombatFocus = false;
	m_bInfoBarDirty = false;
	m_bColonistLocked = false;
	m_bGatheringResource = false; //TAC Whaling, ray
	m_bBarbarian = false; // < JAnimals Mod Start >
	m_bIgnoreDanger = false; // TAC - Trade Routes Advisor - koma13 - START

	m_eOwner = eOwner;
	m_eCapturingPlayer = NO_PLAYER;
	m_eUnitType = eUnit;
	m_pUnitInfo = (NO_UNIT != m_eUnitType) ? &GC.getUnitInfo(m_eUnitType) : NULL;
	m_iBaseCombat = (NO_UNIT != m_eUnitType) ? m_pUnitInfo->getCombat() : 0;
	m_eLeaderUnitType = NO_UNIT;
	m_iCargoCapacity = (NO_UNIT != m_eUnitType) ? m_pUnitInfo->getCargoSpace() : 0;
	m_eProfession = NO_PROFESSION;
	m_eUnitTravelState = NO_UNIT_TRAVEL_STATE;

	m_combatUnit.reset();
	m_transportUnit.reset();
	m_homeCity.reset();
	m_iPostCombatPlotIndex = -1;

	for (iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		m_aiExtraDomainModifier[iI] = 0;
	}

	clear(m_szName);
	m_szScriptData ="";

	// unit yield cache - start - Nightinggale
	updateYieldCache();
	// unit yield cache - end - Nightinggale

	if (!bConstructorCall)
	{
		FAssertMsg((0 < GC.getNumPromotionInfos()), "GC.getNumPromotionInfos() is not greater than zero but an array is being allocated in CvUnit::reset");
		m_paiFreePromotionCount = new int[GC.getNumPromotionInfos()];
		m_pabHasRealPromotion = new bool[GC.getNumPromotionInfos()];
		for (iI = 0; iI < GC.getNumPromotionInfos(); iI++)
		{
			m_pabHasRealPromotion[iI] = false;
			m_paiFreePromotionCount[iI] = 0;
		}

		FAssertMsg((0 < GC.getNumTerrainInfos()), "GC.getNumTerrainInfos() is not greater than zero but a float array is being allocated in CvUnit::reset");
		m_paiTerrainDoubleMoveCount = new int[GC.getNumTerrainInfos()];
		m_paiExtraTerrainAttackPercent = new int[GC.getNumTerrainInfos()];
		m_paiExtraTerrainDefensePercent = new int[GC.getNumTerrainInfos()];
		for (iI = 0; iI < GC.getNumTerrainInfos(); iI++)
		{
			m_paiTerrainDoubleMoveCount[iI] = 0;
			m_paiExtraTerrainAttackPercent[iI] = 0;
			m_paiExtraTerrainDefensePercent[iI] = 0;
		}

		FAssertMsg((0 < GC.getNumFeatureInfos()), "GC.getNumFeatureInfos() is not greater than zero but a float array is being allocated in CvUnit::reset");
		m_paiFeatureDoubleMoveCount = new int[GC.getNumFeatureInfos()];
		m_paiExtraFeatureDefensePercent = new int[GC.getNumFeatureInfos()];
		m_paiExtraFeatureAttackPercent = new int[GC.getNumFeatureInfos()];
		for (iI = 0; iI < GC.getNumFeatureInfos(); iI++)
		{
			m_paiFeatureDoubleMoveCount[iI] = 0;
			m_paiExtraFeatureAttackPercent[iI] = 0;
			m_paiExtraFeatureDefensePercent[iI] = 0;
		}

		FAssertMsg((0 < GC.getNumUnitCombatInfos()), "GC.getNumUnitCombatInfos() is not greater than zero but an array is being allocated in CvUnit::reset");
		m_paiExtraUnitCombatModifier = new int[GC.getNumUnitCombatInfos()];
		for (iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
		{
			m_paiExtraUnitCombatModifier[iI] = 0;
		}

		FAssertMsg((0 < GC.getNumUnitClassInfos()), "GC.getNumUnitClassInfos() is not greater than zero but an array is being allocated in CvUnit::reset");
		m_paiExtraUnitClassAttackModifier = new int[GC.getNumUnitClassInfos()];
		for (iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
		{
			m_paiExtraUnitClassAttackModifier[iI] = 0;
		}

		FAssertMsg((0 < GC.getNumUnitClassInfos()), "GC.getNumUnitClassInfos() is not greater than zero but an array is being allocated in CvUnit::reset");
		m_paiExtraUnitClassDefenseModifier = new int[GC.getNumUnitClassInfos()];
		for (iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
		{
			m_paiExtraUnitClassDefenseModifier[iI] = 0;
		}

		AI_reset();
	}
}


//////////////////////////////////////
// graphical only setup
//////////////////////////////////////
void CvUnit::setupGraphical()
{
	if (!GC.IsGraphicsInitialized())
	{
		return;
	}

	CvDLLEntity::setup();
}


void CvUnit::convert(CvUnit* pUnit, bool bKill)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pTransportUnit;
	CvUnit* pLoopUnit;

	setGameTurnCreated(pUnit->getGameTurnCreated());
	setDamage(pUnit->getDamage());
	setMoves(pUnit->getMoves());
	setYieldStored(pUnit->getYieldStored());
	setFacingDirection(pUnit->getFacingDirection(false));

	setLevel(pUnit->getLevel());
	int iOldModifier = std::max(1, 100 + GET_PLAYER(pUnit->getOwnerINLINE()).getLevelExperienceModifier());
	int iOurModifier = std::max(1, 100 + GET_PLAYER(getOwnerINLINE()).getLevelExperienceModifier());
	setExperience(std::max(0, (pUnit->getExperience() * iOurModifier) / iOldModifier));

	setName(pUnit->getNameNoDesc());
	setLeaderUnitType(pUnit->getLeaderUnitType());
	if (bKill)
	{
		ProfessionTypes eProfession = pUnit->getProfession();
		CvCity* pCity = pUnit->getCity();
		if (pCity != NULL)
		{
			pCity->AI_setWorkforceHack(true);
		}
		pUnit->setProfession(NO_PROFESSION);  // leave equipment behind
		setProfession(eProfession, true);
		if (pCity != NULL)
		{
			pCity->AI_setWorkforceHack(false);
		}
	}
	setUnitTravelState(pUnit->getUnitTravelState(), false);
	setUnitTravelTimer(pUnit->getUnitTravelTimer());

	for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		PromotionTypes ePromotion = (PromotionTypes) iI;
		if (pUnit->isHasRealPromotion(ePromotion))
		{
			setHasRealPromotion(ePromotion, true);
		}
	}

	pTransportUnit = pUnit->getTransportUnit();

	bool bAlive = true;
	if (pTransportUnit != NULL)
	{
		pUnit->setTransportUnit(NULL, false);
		bAlive = setTransportUnit(pTransportUnit);
	}

	if (bAlive)
	{
		if (pUnit->IsSelected() && isOnMap() && getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->insertIntoSelectionList(this, true, false);
		}
	}

	CvPlot* pPlot = pUnit->plot();
	if (pPlot != NULL)
	{
		if (bAlive)
		{
			pUnitNode = pPlot->headUnitNode();

			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = pPlot->nextUnitNode(pUnitNode);

				if (pLoopUnit->getTransportUnit() == pUnit)
				{
					pLoopUnit->setTransportUnit(this);
				}
			}
		}

		if (bKill)
		{
			pUnit->kill(true);
		}
	}
	else //off map
	{
		if (bKill)
		{
			pUnit->updateOwnerCache(-1);
			SAFE_DELETE(pUnit);
		}
	}
}


void CvUnit::kill(bool bDelay, CvUnit* pAttacker)
{
	PROFILE_FUNC();

	CvWString szBuffer;

	CvPlot* pPlot = plot();
	FAssertMsg(pPlot != NULL, "Plot is not assigned a valid value");
	FAssert(GET_PLAYER(getOwnerINLINE()).checkPopulation());

	static std::vector<IDInfo> oldUnits;
	oldUnits.erase(oldUnits.begin(), oldUnits.end());
	CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();

	while (pUnitNode != NULL)
	{
		oldUnits.push_back(pUnitNode->m_data);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);
	}

	for(int i=0;i<(int)oldUnits.size();i++)
	{
		CvUnit* pLoopUnit = ::getUnit(oldUnits[i]);

		if (pLoopUnit != NULL)
		{
			if (pLoopUnit->getTransportUnit() == this)
			{
				//save old units because kill will clear the static list
				std::vector<IDInfo> tempUnits = oldUnits;

				if (pPlot->isValidDomainForLocation(*pLoopUnit))
				{
					pLoopUnit->setCapturingPlayer(getCapturingPlayer());
				}

				if (pLoopUnit->getCapturingPlayer() == NO_PLAYER)
				{
					if (pAttacker != NULL && pAttacker->getUnitInfo().isCapturesCargo())
					{
						pLoopUnit->setCapturingPlayer(pAttacker->getOwnerINLINE());
					}
				}

				pLoopUnit->kill(false, pAttacker);

				oldUnits = tempUnits;
			}
		}
	}

	if (pAttacker != NULL)
	{
		gDLL->getEventReporterIFace()->unitKilled(this, pAttacker->getOwnerINLINE());

		if (NO_UNIT != getLeaderUnitType())
		{
			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					szBuffer = gDLL->getText("TXT_KEY_MISC_GENERAL_KILLED", getNameKey());
					gDLL->getInterfaceIFace()->addMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getEraInfo(GC.getGameINLINE().getCurrentEra()).getAudioUnitDefeatScript(), MESSAGE_TYPE_MAJOR_EVENT);
				}
			}
		}
	}

	if (bDelay)
	{
		startDelayedDeath();
		return;
	}

	finishMoves();

	int iYieldStored = getYieldStored();
	setYieldStored(0);

	removeFromMap();
	updateOwnerCache(-1);

	PlayerTypes eOwner = getOwnerINLINE();
	PlayerTypes eCapturingPlayer = getCapturingPlayer();
	UnitTypes eCaptureUnitType = NO_UNIT;
	ProfessionTypes eCaptureProfession = getProfession();
	FAssert(eCaptureProfession == NO_PROFESSION || !GC.getProfessionInfo(eCaptureProfession).isCitizen());
	if (eCapturingPlayer != NO_PLAYER)
	{
		eCaptureUnitType = getCaptureUnitType(GET_PLAYER(eCapturingPlayer).getCivilizationType());
	}
	YieldTypes eYield = getYield();

	gDLL->getEventReporterIFace()->unitLost(this);

    GET_PLAYER(getOwnerINLINE()).AI_removeUnitFromMoveQueue(this);
	GET_PLAYER(getOwnerINLINE()).deleteUnit(getID());

	FAssert(GET_PLAYER(eOwner).checkPopulation());

	if ((eCapturingPlayer != NO_PLAYER) && (eCaptureUnitType != NO_UNIT))
	{
		if (GET_PLAYER(eCapturingPlayer).isHuman() || GET_PLAYER(eCapturingPlayer).AI_captureUnit(eCaptureUnitType, pPlot) || 0 == GC.getDefineINT("AI_CAN_DISBAND_UNITS"))
		{
			if (!GET_PLAYER(eCapturingPlayer).isProfessionValid(eCaptureProfession, eCaptureUnitType))
			{
				eCaptureProfession = (ProfessionTypes) GC.getUnitInfo(eCaptureUnitType).getDefaultProfession();
			}
			CvUnit* pkCapturedUnit = GET_PLAYER(eCapturingPlayer).initUnit(eCaptureUnitType, eCaptureProfession, pPlot->getX_INLINE(), pPlot->getY_INLINE(), NO_UNITAI, NO_DIRECTION, iYieldStored);

			if (pkCapturedUnit != NULL)
			{
				bool bAlive = true;
				if (pAttacker != NULL && pAttacker->getUnitInfo().isCapturesCargo())
				{
					pkCapturedUnit->setXY(pAttacker->getX_INLINE(), pAttacker->getY_INLINE());
					if(pkCapturedUnit->getTransportUnit() == NULL) //failed to load
					{
						bAlive = false;
						pkCapturedUnit->kill(false);
					}
				}

				if (bAlive)
				{
					szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_CAPTURED_UNIT", GC.getUnitInfo(eCaptureUnitType).getTextKeyWide());
					gDLL->getInterfaceIFace()->addMessage(eCapturingPlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, pkCapturedUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX_INLINE(), pPlot->getY_INLINE());

					if (!pkCapturedUnit->isCargo())
					{
						// Add a captured mission
						CvMissionDefinition kMission;
						kMission.setMissionTime(GC.getMissionInfo(MISSION_CAPTURED).getTime() * gDLL->getSecsPerTurn());
						kMission.setUnit(BATTLE_UNIT_ATTACKER, pkCapturedUnit);
						kMission.setUnit(BATTLE_UNIT_DEFENDER, NULL);
						kMission.setPlot(pPlot);
						kMission.setMissionType(MISSION_CAPTURED);
						gDLL->getEntityIFace()->AddMission(&kMission);
					}

					pkCapturedUnit->finishMoves();

					if (!GET_PLAYER(eCapturingPlayer).isHuman())
					{
						CvPlot* pPlot = pkCapturedUnit->plot();
						if (pPlot && !pPlot->isCity(false))
						{
							if (GET_PLAYER(eCapturingPlayer).AI_getPlotDanger(pPlot) && GC.getDefineINT("AI_CAN_DISBAND_UNITS"))
							{
								pkCapturedUnit->kill(false);
							}
						}
					}
				}
			}
		}
	}
}

void CvUnit::removeFromMap()
{
	if ((getX_INLINE() != INVALID_PLOT_COORD) && (getY_INLINE() != INVALID_PLOT_COORD))
	{
		if (IsSelected())
		{
			if (gDLL->getInterfaceIFace()->getLengthSelectionList() == 1)
			{
				if (!(gDLL->getInterfaceIFace()->isFocused()) && !(gDLL->getInterfaceIFace()->isCitySelection()) && !(gDLL->getInterfaceIFace()->isDiploOrPopupWaiting()))
				{
					GC.getGameINLINE().updateSelectionList();
				}

				if (IsSelected())
				{
					gDLL->getInterfaceIFace()->setCycleSelectionCounter(1);
				}
				else
				{
					gDLL->getInterfaceIFace()->setDirty(SelectionCamera_DIRTY_BIT, true);
				}
			}
		}

		gDLL->getInterfaceIFace()->removeFromSelectionList(this);

		// XXX this is NOT a hack, without it, the game crashes.
		gDLL->getEntityIFace()->RemoveUnitFromBattle(this);

		plot()->setFlagDirty(true);

		FAssertMsg(!isCombat(), "isCombat did not return false as expected");

		CvUnit* pTransportUnit = getTransportUnit();

		if (pTransportUnit != NULL)
		{
			setTransportUnit(NULL);
		}

		AI_setMovePriority(0);

		FAssertMsg(getAttackPlot() == NULL, "The current unit instance's attack plot is expected to be NULL");
		FAssertMsg(getCombatUnit() == NULL, "The current unit instance's combat unit is expected to be NULL");

		if (!gDLL->GetDone() && GC.IsGraphicsInitialized())	// don't need to remove entity when the app is shutting down, or crash can occur
		{
			CvDLLEntity::removeEntity();		// remove entity from engine
		}

		CvDLLEntity::destroyEntity();
		CvDLLEntity::createUnitEntity(this);		// create and attach entity to unit
	}

	AI_setUnitAIType(NO_UNITAI);

	setXY(INVALID_PLOT_COORD, INVALID_PLOT_COORD, true);

	joinGroup(NULL, false, false);
}

void CvUnit::addToMap(int iPlotX, int iPlotY)
{
	if((iPlotX != INVALID_PLOT_COORD) && (iPlotY != INVALID_PLOT_COORD))
	{
		//--------------------------------
		// Init pre-setup() data
		setXY(iPlotX, iPlotY, false, false);

		//--------------------------------
		// Init non-saved data
		setupGraphical();

		//--------------------------------
		// Init other game data
		plot()->updateCenterUnit();

		plot()->setFlagDirty(true);
	}

	if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
	{
		gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
	}
}

void CvUnit::updateOwnerCache(int iChange)
{
	CvPlayer& kPlayer = GET_PLAYER(getOwnerINLINE());

	GET_TEAM(getTeam()).changeUnitClassCount(((UnitClassTypes)(m_pUnitInfo->getUnitClassType())), iChange);
	kPlayer.changeUnitClassCount(((UnitClassTypes)(m_pUnitInfo->getUnitClassType())), iChange);
	kPlayer.changeAssets(getAsset() * iChange);
	kPlayer.changePower(getPower() * iChange);
	CvArea* pArea = area();
	if (pArea != NULL)
	{
		pArea->changePower(getOwnerINLINE(), getPower() * iChange);
	}
	if (m_pUnitInfo->isFound())
	{
		GET_PLAYER(getOwnerINLINE()).changeTotalPopulation(iChange);
	}
}


void CvUnit::NotifyEntity(MissionTypes eMission)
{
	gDLL->getEntityIFace()->NotifyEntity(getUnitEntity(), eMission);
}

/** NBMOD REF **/

/***************************************************************************/
/**                                                                       **/
/** float CvUnit::NBMOD_GetShipStrength() const                           **/
/**                                                                       **/
/** Ermittelt die Schiffstärke der Einheit.                               **/
/**                                                                       **/
/***************************************************************************/

float CvUnit::NBMOD_GetShipStrength() const
{

    // ist es eine Seeeinheit und kann diese angreifen?
    if (m_pUnitInfo->getDomainType() == DOMAIN_SEA && canAttack())
    {
        return baseCombatStr() * GC.getUnitInfo(getUnitType()).NBMOD_GetStrengthWeight();
    }

    return 0.0;

}

/** NBMOD REF **/


void CvUnit::doTurn()
{
	PROFILE_FUNC();

	FAssertMsg(!isDead(), "isDead did not return false as expected");
	FAssert(getGroup() != NULL || GET_PLAYER(getOwnerINLINE()).getPopulationUnitCity(getID()) != NULL);

	testPromotionReady();
	
	// R&R, Robert Surcouf, Damage on Storm plots, Start
	// R&R, bugfix: we only damage ships on Sea and not the transported units, ray, START
	if (getDomainType() == DOMAIN_SEA)
	{
		FeatureTypes eFeature = plot()->getFeatureType();
		if (NO_FEATURE != eFeature)
		{
			int iPotentialDamageFromFeaturePercent = GC.getFeatureInfo(eFeature).getTurnDamage();
			if (0 != iPotentialDamageFromFeaturePercent)
			{
				// R&R, bugfix: we never destroy a unit from feature damage, ray, START
				int iPotentialDamage = (maxHitPoints() * iPotentialDamageFromFeaturePercent) / 100;
				if (currHitPoints() > iPotentialDamage)
				{
					//changeDamage(GC.getFeatureInfo(eFeature).getTurnDamage(), NO_PLAYER); Version Beyond The Sword
					changeDamage(iPotentialDamage, NULL);
				}
				// R&R, bugfix: we never destroy a unit from feature damage, ray, END
			}
		}
	}
	// R&R, Robert Surcouf, Damage on Storm plots, End
	
	if (hasMoved())
	{
		if (isAlwaysHeal())
		{
			doHeal();
		}
	}
	else
	{
		if (isHurt())
		{
			doHeal();
		}

		if (!isCargo())
		{
			changeFortifyTurns(1);
		}
	}

	changeImmobileTimer(-1);
	doUnitTravelTimer();

	setMadeAttack(false);

	setMoves(0);
}


void CvUnit::resolveCombat(CvUnit* pDefender, CvPlot* pPlot, CvBattleDefinition& kBattle)
{
	CombatDetails cdAttackerDetails;
	CombatDetails cdDefenderDetails;

	int iAttackerStrength = currCombatStr(NULL, NULL, &cdAttackerDetails);
	int iAttackerFirepower = currFirepower(NULL, NULL);
	int iDefenderStrength;
	int iAttackerDamage;
	int iDefenderDamage;
	int iDefenderOdds;

	getDefenderCombatValues(*pDefender, pPlot, iAttackerStrength, iAttackerFirepower, iDefenderOdds, iDefenderStrength, iAttackerDamage, iDefenderDamage, &cdDefenderDetails);

	if (isHuman() || pDefender->isHuman())
	{
		CyArgsList pyArgsCD;
		pyArgsCD.add(gDLL->getPythonIFace()->makePythonObject(&cdAttackerDetails));
		pyArgsCD.add(gDLL->getPythonIFace()->makePythonObject(&cdDefenderDetails));
		pyArgsCD.add(getCombatOdds(this, pDefender));
		gDLL->getEventReporterIFace()->genericEvent("combatLogCalc", pyArgsCD.makeFunctionArgs());
	}

	// R&R, ray, adapted change from C.B., with minor modifications - START
	bool bCombatEndedUnresolved = true;

	//	while (true)
	for (int iI = 0; iI < 7; iI++)
	// R&R, ray, adapted change from C.B., with minor modifications - END
	{
		if (GC.getGameINLINE().getSorenRandNum(GC.getDefineINT("COMBAT_DIE_SIDES"), "Combat") < iDefenderOdds)
		{
			if (getDamage() + iAttackerDamage >= maxHitPoints())
			{
				if (GC.getGameINLINE().getSorenRandNum(100, "Withdrawal") < withdrawalProbability())
				{
					changeExperience(GC.getDefineINT("EXPERIENCE_FROM_WITHDRAWL"), pDefender->maxXPValue(), true, pPlot->getOwnerINLINE() == getOwnerINLINE(), true);
					// R&R, ray, adapted change from C.B., with minor modifications
					bCombatEndedUnresolved = false;
					break;
				}

				if (GC.getGameINLINE().getSorenRandNum(100, "Evasion") < getEvasionProbability(*pDefender))
				{
					// evasion
					CvCity* pCity = getEvasionCity();
					FAssert(pCity != NULL);
					if (pCity != NULL)
					{
						setPostCombatPlot(pCity->getX_INLINE(), pCity->getY_INLINE());
						// R&R, ray, adapted change from C.B., with minor modifications
						bCombatEndedUnresolved = false;
						break;
					}
				}
			}

			changeDamage(iAttackerDamage, pDefender);

			cdAttackerDetails.iCurrHitPoints = currHitPoints();

			if (isHuman() || pDefender->isHuman())
			{
				CyArgsList pyArgs;
				pyArgs.add(gDLL->getPythonIFace()->makePythonObject(&cdAttackerDetails));
				pyArgs.add(gDLL->getPythonIFace()->makePythonObject(&cdDefenderDetails));
				pyArgs.add(1);
				pyArgs.add(iAttackerDamage);
				gDLL->getEventReporterIFace()->genericEvent("combatLogHit", pyArgs.makeFunctionArgs());
			}
		}
		else
		{
			if (pDefender->getDamage() + iDefenderDamage >= pDefender->maxHitPoints() && GC.getGameINLINE().getSorenRandNum(100, "Evasion") < pDefender->getEvasionProbability(*this))
			{
				// evasion
				CvCity* pCity = pDefender->getEvasionCity();
				FAssert(pCity != NULL);
				if (pCity != NULL)
				{
					pDefender->setPostCombatPlot(pCity->getX_INLINE(), pCity->getY_INLINE());
					// R&R, ray, adapted change from C.B., with minor modifications
					bCombatEndedUnresolved = false;
					break;
				}
			}

			pDefender->changeDamage(iDefenderDamage, this);

			cdDefenderDetails.iCurrHitPoints=pDefender->currHitPoints();

			if (isHuman() || pDefender->isHuman())
			{
				CyArgsList pyArgs;
				pyArgs.add(gDLL->getPythonIFace()->makePythonObject(&cdAttackerDetails));
				pyArgs.add(gDLL->getPythonIFace()->makePythonObject(&cdDefenderDetails));
				pyArgs.add(0);
				pyArgs.add(iDefenderDamage);
				gDLL->getEventReporterIFace()->genericEvent("combatLogHit", pyArgs.makeFunctionArgs());
			}
		}

		if (isDead() || pDefender->isDead())
		{
			if (isDead())
			{
				int iExperience = defenseXPValue();
				iExperience = ((iExperience * iAttackerStrength) / iDefenderStrength);
				iExperience = range(iExperience, GC.getDefineINT("MIN_EXPERIENCE_PER_COMBAT"), GC.getDefineINT("MAX_EXPERIENCE_PER_COMBAT"));
				pDefender->changeExperience(iExperience, maxXPValue(), true, pPlot->getOwnerINLINE() == pDefender->getOwnerINLINE(), true);

				// R&R, ray, adapted change from C.B., with minor modifications
				bCombatEndedUnresolved = false;
			}
			else
			{
				int iExperience = pDefender->attackXPValue();
				iExperience = ((iExperience * iDefenderStrength) / iAttackerStrength);
				iExperience = range(iExperience, GC.getDefineINT("MIN_EXPERIENCE_PER_COMBAT"), GC.getDefineINT("MAX_EXPERIENCE_PER_COMBAT"));
				changeExperience(iExperience, pDefender->maxXPValue(), true, pPlot->getOwnerINLINE() == getOwnerINLINE(), true);

				// R&R, ray, adapted change from C.B., with minor modifications
				bCombatEndedUnresolved = false;
			}

			break;
		}
	}

	// R&R, ray, adapted change from C.B., with minor modifications - START
	// give half XP from unresolved combat to both sides - IF no clear winner, no withdrawal and no evasion
	if (bCombatEndedUnresolved)
	{
		int iExperienceAttacker = std::max(1,(pDefender->attackXPValue()/2));
		iExperienceAttacker = range(iExperienceAttacker, GC.getDefineINT("MIN_EXPERIENCE_PER_COMBAT"), GC.getDefineINT("MAX_EXPERIENCE_PER_COMBAT"));
		changeExperience(iExperienceAttacker, pDefender->maxXPValue(), true, pPlot->getOwnerINLINE() == getOwnerINLINE(), true);
	
		int iExperienceDefender = std::max(1,(defenseXPValue()/2));
		iExperienceDefender = range(iExperienceDefender, GC.getDefineINT("MIN_EXPERIENCE_PER_COMBAT"), GC.getDefineINT("MAX_EXPERIENCE_PER_COMBAT"));
		pDefender->changeExperience(iExperienceDefender, maxXPValue(), true, pPlot->getOwnerINLINE() == pDefender->getOwnerINLINE(), true);
	}
	// R&R, ray, adapted change from C.B., with minor modifications - END
}


void CvUnit::updateCombat(bool bQuick)
{
	CvWString szBuffer;

	bool bFinish = false;
	bool bVisible = false;

	if (getCombatTimer() > 0)
	{
		changeCombatTimer(-1);

		if (getCombatTimer() > 0)
		{
			return;
		}
		else
		{
			bFinish = true;
		}
	}

	CvPlot* pPlot = getAttackPlot();

	if (pPlot == NULL)
	{
		return;
	}

	// R&R, ray, Natives raiding party - START
	bool bIsNativeRaid = (AI_getUnitAIState() == UNITAI_STATE_RAIDING_PARTY) ? true : false;
	CvUnit* pDefender = NULL;
	if (bFinish)
	{
		pDefender = getCombatUnit();
	}
	else
	{
		pDefender = pPlot->getBestDefender(NO_PLAYER, getOwnerINLINE(), this, (AI_getUnitAIState() == UNITAI_STATE_RAIDING_PARTY) ? false : true);
	}
	// R&R, ray, Natives raiding party - END

	if (pDefender == NULL)
	{
		setAttackPlot(NULL);
		setCombatUnit(NULL);

		getGroup()->groupMove(pPlot, true, ((canAdvance(pPlot, 0)) ? this : NULL));

		getGroup()->clearMissionQueue();

		return;
	}
	// TAC - Whaling - koma13 - START
	else if (pDefender->isGatheringResource())
	{
		pDefender->convertFishingBoat();
		pDefender->setGatheringResource(false);
	}
	// TAC - Whaling - koma13 - END

	//check if quick combat
	if (!bQuick)
	{
		bVisible = isCombatVisible(pDefender);
	}

	//FAssertMsg((pPlot == pDefender->plot()), "There is not expected to be a defender or the defender's plot is expected to be pPlot (the attack plot)");

	//if not finished and not fighting yet, set up combat damage and mission
	if (!bFinish)
	{
		if (!isFighting())
		{
			if (plot()->isFighting() || pPlot->isFighting())
			{
				return;
			}

			setMadeAttack(true);

			//rotate to face plot
			DirectionTypes newDirection = estimateDirection(this->plot(), pDefender->plot());
			if (newDirection != NO_DIRECTION)
			{
				setFacingDirection(newDirection);
			}

			//rotate enemy to face us
			newDirection = estimateDirection(pDefender->plot(), this->plot());
			if (newDirection != NO_DIRECTION)
			{
				pDefender->setFacingDirection(newDirection);
			}

			setCombatUnit(pDefender, true);
			pDefender->setCombatUnit(this, false);

			pDefender->getGroup()->clearMissionQueue();

			bool bFocused = (bVisible && isCombatFocus() && gDLL->getInterfaceIFace()->isCombatFocus());

			if (bFocused)
			{
				DirectionTypes directionType = directionXY(plot(), pPlot);
				//								N			NE				E				SE					S				SW					W				NW
				NiPoint2 directions[8] = {NiPoint2(0, 1), NiPoint2(1, 1), NiPoint2(1, 0), NiPoint2(1, -1), NiPoint2(0, -1), NiPoint2(-1, -1), NiPoint2(-1, 0), NiPoint2(-1, 1)};
				NiPoint3 attackDirection = NiPoint3(directions[directionType].x, directions[directionType].y, 0);
				float plotSize = GC.getPLOT_SIZE();
				NiPoint3 lookAtPoint(plot()->getPoint().x + plotSize / 2 * attackDirection.x, plot()->getPoint().y + plotSize / 2 * attackDirection.y, (plot()->getPoint().z + pPlot->getPoint().z) / 2);
				attackDirection.Unitize();
				gDLL->getInterfaceIFace()->lookAt(lookAtPoint, (((getOwnerINLINE() != GC.getGameINLINE().getActivePlayer()) || gDLL->getGraphicOption(GRAPHICOPTION_NO_COMBAT_ZOOM)) ? CAMERALOOKAT_BATTLE : CAMERALOOKAT_BATTLE_ZOOM_IN), attackDirection);
			}
			else
			{
				PlayerTypes eAttacker = getVisualOwner(pDefender->getTeam());
				CvWString szMessage;
				if (UNKNOWN_PLAYER != eAttacker)
				{
					szMessage = gDLL->getText("TXT_KEY_MISC_YOU_UNITS_UNDER_ATTACK", GET_PLAYER(eAttacker).getNameKey());
				}
				else
				{
					szMessage = gDLL->getText("TXT_KEY_MISC_YOU_UNITS_UNDER_ATTACK_UNKNOWN");
				}

				gDLL->getInterfaceIFace()->addMessage(pDefender->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szMessage, "AS2D_COMBAT", MESSAGE_TYPE_DISPLAY_ONLY, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pPlot->getX_INLINE(), pPlot->getY_INLINE(), true);
			}
		}

		FAssertMsg(pDefender != NULL, "Defender is not assigned a valid value");

		FAssertMsg(plot()->isFighting(), "Current unit instance plot is not fighting as expected");
		FAssertMsg(pPlot->isFighting(), "pPlot is not fighting as expected");

		if (!pDefender->canDefend())
		{
			// WTP, ray, fix for destroying ships instead of ejecting - START
			bool bShipCityEscape = false;
			bool bPlotCityOrFort = pDefender->plot()->isCity() || pDefender->plot()->isFort();

			// Ships in Harbour Cities should be ejected damaged when City Captured
			if (pDefender != NULL && bPlotCityOrFort && getDomainType()!= DOMAIN_SEA && pDefender->getDomainType() == DOMAIN_SEA)
			{
				CvPlot* DefenderPlot = pDefender->plot();
				CvPlot* ejectPlot = NULL;
				CvPlot* pAdjacentPlot = NULL;
				CvCity* CityForMessage = pDefender->plot()->getPlotCity();
				
				// Try to find a suitable water plot to eject
				for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
				{
					pAdjacentPlot = plotDirection(DefenderPlot->getX_INLINE(), DefenderPlot->getY_INLINE(), ((DirectionTypes)iI));
					// must be water without enemies
					if (pAdjacentPlot != NULL && pAdjacentPlot->isWater() && pAdjacentPlot->getNumVisibleEnemyDefenders(pDefender) == 0)
					{
						ejectPlot = pAdjacentPlot;
						break;
					}
				}

				// if suitable eject Plot found, give some random damage and move to eject Plot
				if (ejectPlot != NULL)
				{
					int iDamageRand = std::max(10, GC.getGameINLINE().getSorenRandNum(75, "random ship damage"));
					pDefender->setDamage(GC.getMAX_HIT_POINTS() * iDamageRand / 100);

					// clean up to avoid Asserts when changing positon
					setCombatUnit(NULL);
					pDefender->setCombatUnit(NULL);

					// changing position
					pDefender->setXY(ejectPlot->getX_INLINE(), ejectPlot->getY_INLINE());
					
					if (CityForMessage != NULL)
					{
						// Send Messages
						szBuffer = gDLL->getText("TXT_KEY_MISC_ENEMY_SHIP_ESCAPED_CITY", pDefender->getName().GetCString(), CityForMessage->getNameKey());
						gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), ejectPlot->getX_INLINE(), ejectPlot->getY_INLINE());

						szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_SHIP_ESCAPED_CITY", pDefender->getName().GetCString(), CityForMessage->getNameKey());
						gDLL->getInterfaceIFace()->addMessage(pDefender->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), ejectPlot->getX_INLINE(), ejectPlot->getY_INLINE());
					}

					// set to true to avoid killing the Defender Unit in next if below
					bShipCityEscape = true;
				}
			}
			// WTP, ray, fix for destroying ships instead of ejecting - END
			
			if (bShipCityEscape == false) // WTP, ray, fix for destroying ships instead of ejecting
			{
				if (!bVisible)
				{
					bFinish = true;
				}
			
				else
				{
					CvMissionDefinition kMission;
					kMission.setMissionTime(getCombatTimer() * gDLL->getSecsPerTurn());
					kMission.setMissionType(MISSION_SURRENDER);
					kMission.setUnit(BATTLE_UNIT_ATTACKER, this);
					kMission.setUnit(BATTLE_UNIT_DEFENDER, pDefender);
					kMission.setPlot(pPlot);
					gDLL->getEntityIFace()->AddMission(&kMission);

					// Surrender mission
					setCombatTimer(GC.getMissionInfo(MISSION_SURRENDER).getTime());

					GC.getGameINLINE().incrementTurnTimer(getCombatTimer());
				}
				// Kill them!
				pDefender->setDamage(GC.getMAX_HIT_POINTS());
			}
		}
		else
		{
			CvBattleDefinition kBattle;
			kBattle.setUnit(BATTLE_UNIT_ATTACKER, this);
			kBattle.setUnit(BATTLE_UNIT_DEFENDER, pDefender);
			kBattle.setDamage(BATTLE_UNIT_ATTACKER, BATTLE_TIME_BEGIN, getDamage());
			kBattle.setDamage(BATTLE_UNIT_DEFENDER, BATTLE_TIME_BEGIN, pDefender->getDamage());

			resolveCombat(pDefender, pPlot, kBattle);

			if (!bVisible)
			{
				bFinish = true;
			}
			else
			{
				kBattle.setDamage(BATTLE_UNIT_ATTACKER, BATTLE_TIME_END, getDamage());
				kBattle.setDamage(BATTLE_UNIT_DEFENDER, BATTLE_TIME_END, pDefender->getDamage());
				kBattle.setAdvanceSquare(canAdvance(pPlot, 1));

				if (isRanged() && pDefender->isRanged()) //ranged
				{
					kBattle.setDamage(BATTLE_UNIT_ATTACKER, BATTLE_TIME_RANGED, kBattle.getDamage(BATTLE_UNIT_ATTACKER, BATTLE_TIME_END));
					kBattle.setDamage(BATTLE_UNIT_DEFENDER, BATTLE_TIME_RANGED, kBattle.getDamage(BATTLE_UNIT_DEFENDER, BATTLE_TIME_END));
				}
				else if(kBattle.isOneStrike()) //melee dies right away
				{
					kBattle.setDamage(BATTLE_UNIT_ATTACKER, BATTLE_TIME_RANGED, kBattle.getDamage(BATTLE_UNIT_ATTACKER, BATTLE_TIME_END));
					kBattle.setDamage(BATTLE_UNIT_DEFENDER, BATTLE_TIME_RANGED, kBattle.getDamage(BATTLE_UNIT_DEFENDER, BATTLE_TIME_END));
				}
				else //melee fighting
				{
					kBattle.addDamage(BATTLE_UNIT_ATTACKER, BATTLE_TIME_RANGED, kBattle.getDamage(BATTLE_UNIT_ATTACKER, BATTLE_TIME_BEGIN));
					kBattle.addDamage(BATTLE_UNIT_DEFENDER, BATTLE_TIME_RANGED, kBattle.getDamage(BATTLE_UNIT_DEFENDER, BATTLE_TIME_BEGIN));
				}

				int iTurns = planBattle( kBattle);
				kBattle.setMissionTime(iTurns * gDLL->getSecsPerTurn());
				setCombatTimer(iTurns);

				GC.getGameINLINE().incrementTurnTimer(getCombatTimer());

				if (pPlot->isActiveVisible(false))
				{
					ExecuteMove(0.5f, true);
					gDLL->getEntityIFace()->AddMission(&kBattle);
				}
			}
		}
	}

	if (bFinish)
	{
		if (bVisible)
		{
			if (isCombatFocus() && gDLL->getInterfaceIFace()->isCombatFocus())
			{
				if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
				{
					gDLL->getInterfaceIFace()->releaseLockedCamera();
				}
			}
		}

		//end the combat mission if this code executes first
		gDLL->getEntityIFace()->RemoveUnitFromBattle(this);
		gDLL->getEntityIFace()->RemoveUnitFromBattle(pDefender);
		setAttackPlot(NULL);
		bool bDefenderEscaped = (pDefender->getPostCombatPlot() != pPlot);
		bool bAttackerEscaped = (getPostCombatPlot() != plot());
		setCombatUnit(NULL);
		pDefender->setCombatUnit(NULL);
		NotifyEntity(MISSION_DAMAGE);
		pDefender->NotifyEntity(MISSION_DAMAGE);

		if (isDead())
		{
			// PatchMod: Achievements START
			if(!getUnitInfo().isAnimal())
				{
				if (getDomainType() == DOMAIN_SEA) // R&R, ray, Achievments for Sea Combats
				{
					GET_PLAYER(pDefender->getOwnerINLINE()).changeNumSeaCombatsWon(1);
				}
				else
				{
					GET_PLAYER(pDefender->getOwnerINLINE()).changeNumCombatsWon(1);
				}
			}
			// PatchMod: Achievements END

			if (!m_pUnitInfo->isHiddenNationality() && !pDefender->getUnitInfo().isHiddenNationality())
			{
				GET_TEAM(pDefender->getTeam()).AI_changeWarSuccess(getTeam(), GC.getDefineINT("WAR_SUCCESS_DEFENDING"));
			}

			// R&R, ray, Natives raiding party - START
			CvCity* pCity = pPlot->getPlotCity();
			if (pCity != NULL)
			{
				if (bIsNativeRaid)
				{
					raidCity(pCity);
				}
				else
				{
					raidGoods(pCity);
				}
			}
			// R&R, ray, Natives raiding party - END

			// R&R, ray, Monasteries and Forts - START
			if (bIsNativeRaid && pPlot->isFort())
			{
				szBuffer = gDLL->getText("TXT_KEY_NATIVE_RAID_ATTACKED_FORT");
				gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getEraInfo(GC.getGameINLINE().getCurrentEra()).getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pPlot->getX_INLINE(), pPlot->getY_INLINE());
			}
			// R&R, ray, Monasteries and Forts - END

			szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_DIED_ATTACKING", getNameOrProfessionKey(), pDefender->getNameOrProfessionKey());
			gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getEraInfo(GC.getGameINLINE().getCurrentEra()).getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pPlot->getX_INLINE(), pPlot->getY_INLINE());
			szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_KILLED_ENEMY_UNIT", pDefender->getNameOrProfessionKey(), getNameOrProfessionKey(), getVisualCivAdjective(pDefender->getTeam()));
			gDLL->getInterfaceIFace()->addMessage(pDefender->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getEraInfo(GC.getGameINLINE().getCurrentEra()).getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX_INLINE(), pPlot->getY_INLINE());

			// report event to Python, along with some other key state
			gDLL->getEventReporterIFace()->combatResult(pDefender, this);

			//ray15
			bool bRaided = pDefender->raidWeapons(this);
			//Ende ray15
			
			// TAC - AI purchases military units - koma13 - START
			if (!isHuman() && m_pUnitInfo->isHiddenNationality())
			{
				GET_PLAYER(getOwnerINLINE()).AI_changeNumRetiredAIUnits(UNITAI_PIRATE_SEA, 1);
			}
			// TAC - AI purchases military units - koma13 - END
		}
		else if (pDefender->isDead())
		{
			// TAC Capturing Ships - ray
			bool displayCapturedShipMessage = false;
			int capturingShipChance = GC.getBASE_CHANCE_CAPTURING_SHIPS();
			int randomShipCaptureValue = GC.getGameINLINE().getSorenRandNum(1000, "Capture Ships");
					
			if (m_pUnitInfo->isCapturesShips() && !(pDefender->getUnitInfo()).isAnimal())
			{
				if (capturingShipChance > randomShipCaptureValue)
				{
					bool bHasParents = (GET_PLAYER(getOwnerINLINE()).getParent() != NO_PLAYER);
					bool bIsEurope = GET_PLAYER(getOwnerINLINE()).isEurope();

					if ((bHasParents && pDefender->getUnitInfo().getDomainType() == DOMAIN_SEA) || (bIsEurope && pDefender->getUnitInfo().isCapturesShips()))
					{
						bool bAtWar = GET_TEAM(getTeam()).isAtWar(pDefender->getTeam());

						if ((m_pUnitInfo->isCapturesCargo() && bAtWar) || !m_pUnitInfo->isCapturesCargo())
						{
							// duplicate defeated unit
							CvUnit* pkCapturedUnitAfterSeaFight = GET_PLAYER(getOwnerINLINE()).initUnit(pDefender->getUnitType(), NO_PROFESSION, pPlot->getX_INLINE(), pPlot->getY_INLINE(), NO_UNITAI, NO_DIRECTION, pDefender->getYieldStored());
							int iDamageRand = std::max(10, GC.getGameINLINE().getSorenRandNum(75, "random ship damage"));
							int iOldModifier = std::max(1, 100 + GET_PLAYER(pDefender->getOwnerINLINE()).getLevelExperienceModifier());
							int iOurModifier = std::max(1, 100 + GET_PLAYER(pkCapturedUnitAfterSeaFight->getOwnerINLINE()).getLevelExperienceModifier());
							displayCapturedShipMessage = true;

							pkCapturedUnitAfterSeaFight->setGameTurnCreated(pDefender->getGameTurnCreated());
							pkCapturedUnitAfterSeaFight->setDamage(GC.getMAX_HIT_POINTS() * iDamageRand / 100);
							pkCapturedUnitAfterSeaFight->setFacingDirection(pDefender->getFacingDirection(false));
							pkCapturedUnitAfterSeaFight->setLevel(pDefender->getLevel());
							pkCapturedUnitAfterSeaFight->setExperience(std::max(0, (pDefender->getExperience() * iOurModifier) / iOldModifier));
							pkCapturedUnitAfterSeaFight->setName(pDefender->getNameNoDesc());
							pkCapturedUnitAfterSeaFight->setLeaderUnitType(pDefender->getLeaderUnitType());
							for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
							{
								PromotionTypes ePromotion = (PromotionTypes) iI;
								if (pDefender->isHasRealPromotion(ePromotion))
								{
									pkCapturedUnitAfterSeaFight->setHasRealPromotion(ePromotion, true);
								}
							}
						}
					}
				}
			}
			// TAC Capturing Ships - ray - END

			// PatchMod: Achievements START
			if(!pDefender->getUnitInfo().isAnimal())
			{
				if (getDomainType() == DOMAIN_SEA) // R&R, ray, Achievments for Sea Combats
				{
					GET_PLAYER(getOwnerINLINE()).changeNumSeaCombatsWon(1);
				}
				else
				{
					GET_PLAYER(getOwnerINLINE()).changeNumCombatsWon(1);
				}
			}
			// PatchMod: Achievements END
			
			// TAC - AI Improved Naval AI - koma13 - START
			if (GET_PLAYER(pDefender->getOwnerINLINE()).AI_needsProtection(pDefender->AI_getUnitAIType()))
			{
				pDefender->plot()->setDangerMap(pDefender->getOwnerINLINE(), GC.getAI_LOST_TRANSPORT_MEMORY_COUNT());
			}
			// TAC - AI Improved Naval AI - koma13 - END
			
			// TAC - AI purchases military units - koma13 - START
			if (!pDefender->isHuman() && pDefender->getUnitInfo().isHiddenNationality())
			{
				GET_PLAYER(pDefender->getOwnerINLINE()).AI_changeNumRetiredAIUnits(UNITAI_PIRATE_SEA, 1);
			}
			// TAC - AI purchases military units - koma13 - END

			if (!m_pUnitInfo->isHiddenNationality() && !pDefender->getUnitInfo().isHiddenNationality())
			{
				// R&R, ray, Natives raiding party - START
				if (bIsNativeRaid)
				{
					if (GET_PLAYER(getOwnerINLINE()).isNative())
					{
						if (pDefender->getUnitInfo().isTreasure())
						{
							int iRaidTreasureSum = pDefender->getYieldStored() * GC.getNATIVE_GOODS_RAID_PERCENT() / 100;
							GET_PLAYER(getOwnerINLINE()).changeGold(iRaidTreasureSum);
						}
					}
				}
				else
				{
					GET_TEAM(getTeam()).AI_changeWarSuccess(pDefender->getTeam(), GC.getDefineINT("WAR_SUCCESS_ATTACKING"));
					if (GET_PLAYER(getOwnerINLINE()).isNative())
					{
						if (pDefender->getUnitInfo().isTreasure())
						{
							int iRaidTreasureSum = pDefender->getYieldStored() * GC.getNATIVE_GOODS_RAID_PERCENT() / 100;
							GET_PLAYER(getOwnerINLINE()).changeGold(iRaidTreasureSum);
						}

						GET_TEAM(getTeam()).AI_changeDamages(pDefender->getTeam(), -2 * pDefender->getUnitInfo().getAssetValue());
					}
				}
				// R&R, ray, Natives raiding party - END
			}

			szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_DESTROYED_ENEMY", getNameOrProfessionKey(), pDefender->getNameOrProfessionKey());

			// TAC Capturing Ships - ray
			if (displayCapturedShipMessage)
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_CAPTURED_ENEMY_SHIP", pDefender->getNameOrProfessionKey());
			}
			// TAC Capturing Ships - ray - END

			//ray14
			// R&R, ray, changes to Wild Animals - added addittional check so Animals do not capture
			// WTP, ray, allowing Wagon Trains to be caught again
			if (((pDefender->cargoSpace() > 0 && (pDefender->getDomainType() == DOMAIN_LAND)) || pDefender->getUnitInfo().isTreasure() || (pDefender->isUnarmed() && pDefender->getProfession() != NO_PROFESSION && GC.getProfessionInfo(pDefender->getProfession()).getCombatChange() > 0)) && !GET_PLAYER(getOwnerINLINE()).isNative() && !GC.getGameINLINE().isBarbarianPlayer(getOwnerINLINE())) 
			{
				CvUnit* pkCapturedUnitAfterFight = GET_PLAYER(getOwnerINLINE()).initUnit(pDefender->getUnitType(), pDefender->getProfession(), pPlot->getX_INLINE(), pPlot->getY_INLINE(), NO_UNITAI, NO_DIRECTION, pDefender->getYieldStored());
				pkCapturedUnitAfterFight->setDamage(GC.getMAX_HIT_POINTS() / 2);	
				szBuffer = gDLL->getText("TXT_KEY_UNIT_CAPTURED_AFTER_FIGHT", pDefender->getUnitInfo().getDescription());				
			}
			//Ende ray14

			// R&R, ray, changes to Wild Animals - START
			if ((pDefender->getUnitInfo()).isAnimal())
			{
				int iGoldRewardRandomBase = GC.getWILD_ANIMAL_REWARD_RANDOM_BASE();
				int iGold = GC.getGameINLINE().getSorenRandNum(iGoldRewardRandomBase, "Animal Kill Reward");
				GET_PLAYER(getOwnerINLINE()).changeGold(iGold);
				szBuffer = gDLL->getText("TXT_KEY_ANIMAL_REWARD", pDefender->getUnitInfo().getDescription(), iGold);
			}
			// R&R, ray, changes to Wild Animals - END

			// R&R, ray, Monasteries and Forts - START
			if (bIsNativeRaid && pPlot->isFort())
			{
				szBuffer = gDLL->getText("TXT_KEY_NATIVE_RAID_ATTACKED_FORT_BUT_DEFEATED");
				gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getEraInfo(GC.getGameINLINE().getCurrentEra()).getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX_INLINE(), pPlot->getY_INLINE());
			}
			// R&R, ray, Monasteries and Forts - END

			gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getEraInfo(GC.getGameINLINE().getCurrentEra()).getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX_INLINE(), pPlot->getY_INLINE());
			if (getVisualOwner(pDefender->getTeam()) != getOwnerINLINE())
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_UNKNOWN", pDefender->getNameOrProfessionKey(), getNameOrProfessionKey());
			}
			else
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED", pDefender->getNameOrProfessionKey(), getNameOrProfessionKey(), getVisualCivAdjective(pDefender->getTeam()));
			}

			gDLL->getInterfaceIFace()->addMessage(pDefender->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer,GC.getEraInfo(GC.getGameINLINE().getCurrentEra()).getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pPlot->getX_INLINE(), pPlot->getY_INLINE());

			// report event to Python, along with some other key state
			gDLL->getEventReporterIFace()->combatResult(this, pDefender);

			bool bAdvance = false;
			bool bRaided = raidWeapons(pDefender);

			// WTP, ray, prevent Barbarian Civs to eject all the unarmed Units to defend a City it just conquered - START
			// if (!pDefender->isUnarmed() || GET_PLAYER(getOwnerINLINE()).isNative())
			if ((!pDefender->isUnarmed() && !pDefender->isBarbarian()) || GET_PLAYER(getOwnerINLINE()).isNative())
			{
				CvCity* pCity = pPlot->getPlotCity();
				if (NULL != pCity && pCity->getOwnerINLINE() == pDefender->getOwnerINLINE())
				{
					if (pPlot->getNumVisibleEnemyDefenders(this) <= 1)
					{
						pCity->ejectBestDefender(NULL, NULL);
					}
				}
			}
			else
			{
				if (!isNoUnitCapture())
				{
					CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
					while (pUnitNode != NULL)
					{
						CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
						pUnitNode = pPlot->nextUnitNode(pUnitNode);

						if (pLoopUnit != pDefender)
						{
							if (isEnemy(pLoopUnit->getCombatTeam(getTeam(), pPlot), pPlot))
							{
								pLoopUnit->setCapturingPlayer(getOwnerINLINE());
							}
						}
					}
				}
			}

			bAdvance = canAdvance(pPlot, ((pDefender->canDefend()) ? 1 : 0));

			if (bAdvance)
			{
				if (!isNoUnitCapture())
				{
					if (!pDefender->canDefend())
					{
						pDefender->setCapturingPlayer(getOwnerINLINE());
					}
				}
			}

			pDefender->kill(false);
			pDefender = NULL;

			if (!bAdvance)
			{
				changeMoves(pPlot->movementCost(this, plot()));

				if (!canMove() || !isBlitz())
				{
					if (IsSelected())
					{
						if (gDLL->getInterfaceIFace()->getLengthSelectionList() > 1)
						{
							gDLL->getInterfaceIFace()->removeFromSelectionList(this);
						}
					}
				}
			}

			if (!bRaided)
			{
				// R&R, ray, Natives raiding party - START
				CvCity* pCity = pPlot->getPlotCity();
				if (pCity != NULL)
				{
					if (bIsNativeRaid)
					{
						raidCity(pCity);
					}
					else
					{
						if (!raidWeapons(pCity))
						{
							raidGoods(pCity);
						}
					}
				}
				// R&R, ray, Natives raiding party - END
			}

			if (pPlot->getNumVisibleEnemyDefenders(this) == 0)
			{
				getGroup()->groupMove(pPlot, true, ((bAdvance) ? this : NULL));
			}

			// This is is put before the plot advancement, the unit will always try to walk back
			// to the square that they came from, before advancing.
			getGroup()->clearMissionQueue();
		}
		else if (bDefenderEscaped)
		{
			// PatchMod: Achievements START
			if(!pDefender->getUnitInfo().isAnimal())
			{
				if (getDomainType() == DOMAIN_SEA) // R&R, ray, Achievments for Sea Combats
				{
					GET_PLAYER(getOwnerINLINE()).changeNumSeaCombatsWon(1);
				}
				else
				{
					GET_PLAYER(getOwnerINLINE()).changeNumCombatsWon(1);
				}
			}
			// PatchMod: Achievements END

			szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_ESCAPED", getNameOrProfessionKey(), pDefender->getNameOrProfessionKey());
			gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pPlot->getX_INLINE(), pPlot->getY_INLINE());
			CvCity* pCity = pDefender->plot()->getPlotCity();
			if (pCity != NULL)
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_ENEMY_UNIT_ESCAPED", pDefender->getNameOrProfessionKey(), getNameOrProfessionKey(), pCity->getNameKey());
				gDLL->getInterfaceIFace()->addMessage(pDefender->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pCity->getX_INLINE(), pCity->getY_INLINE());
			}

			bool bAdvance = canAdvance(pPlot, 0);
			if (!bAdvance)
			{
				changeMoves(std::max(GC.getMOVE_DENOMINATOR(), pPlot->movementCost(this, plot())));

				if (!canMove() || !isBlitz())
				{
					if (IsSelected())
					{
						if (gDLL->getInterfaceIFace()->getLengthSelectionList() > 1)
						{
							gDLL->getInterfaceIFace()->removeFromSelectionList(this);
						}
					}
				}
			}

			// R&R, ray, Natives raiding party - START
			CvCity* pRaidCity = pPlot->getPlotCity();
			if (pRaidCity != NULL)
			{
				if (bIsNativeRaid)
				{
					raidCity(pRaidCity);
				}
				else
				{
					raidGoods(pRaidCity);
				}
			}
			// R&R, ray, Natives raiding party - END

			//ray FIX City-Destruction with fleeing ship
			//deleted the following IF that made absolutelty no sense 
			//if (m_pUnitInfo->isCapturesCargo())			

			if (pPlot->getNumVisibleEnemyDefenders(this) == 0)
			{
				getGroup()->groupMove(pPlot, true, ((bAdvance) ? this : NULL));
			}

			getGroup()->clearMissionQueue();
		}
		else if (bAttackerEscaped)
		{
			// PatchMod: Achievements START
			if(!getUnitInfo().isAnimal())
			{
				if (getDomainType() == DOMAIN_SEA) // R&R, ray, Achievments for Sea Combats
				{
					GET_PLAYER(pDefender->getOwnerINLINE()).changeNumSeaCombatsWon(1);
				}
				else
				{
					GET_PLAYER(pDefender->getOwnerINLINE()).changeNumCombatsWon(1);
				}
			}
			// PatchMod: Achievements END

			CvCity* pCity = plot()->getPlotCity();
			if (pCity != NULL)
			{
				// R&R, ray, Natives raiding party - START
				if (bIsNativeRaid)
				{
					raidCity(pCity);
				}
				// R&R, ray, Natives raiding party - END

				szBuffer = gDLL->getText("TXT_KEY_MISC_ENEMY_UNIT_ESCAPED", getNameOrProfessionKey(), pDefender->getNameOrProfessionKey(), pCity->getNameKey());
				gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pCity->getX_INLINE(), pCity->getY_INLINE());
			}
			szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_ESCAPED", pDefender->getNameOrProfessionKey(), getNameOrProfessionKey());
			gDLL->getInterfaceIFace()->addMessage(pDefender->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pPlot->getX_INLINE(), pPlot->getY_INLINE());

			if (IsSelected())
			{
				if (gDLL->getInterfaceIFace()->getLengthSelectionList() > 1)
				{
					gDLL->getInterfaceIFace()->removeFromSelectionList(this);
				}
			}


			// This is is put before the plot advancement, the unit will always try to walk back
			// to the square that they came from, before advancing.
			getGroup()->clearMissionQueue();
		}
		else
		{
			szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WITHDRAW", getNameOrProfessionKey(), pDefender->getNameOrProfessionKey());
			gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX_INLINE(), pPlot->getY_INLINE());
			szBuffer = gDLL->getText("TXT_KEY_MISC_ENEMY_UNIT_WITHDRAW", getNameOrProfessionKey(), pDefender->getNameOrProfessionKey());
			gDLL->getInterfaceIFace()->addMessage(pDefender->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pPlot->getX_INLINE(), pPlot->getY_INLINE());

			changeMoves(std::max(GC.getMOVE_DENOMINATOR(), pPlot->movementCost(this, plot())));

			// R&R, ray, Natives raiding party - START
			CvCity* pCity = pPlot->getPlotCity();
			if (pCity != NULL)
			{
				if (bIsNativeRaid)
				{
					raidCity(pCity);
				}
				else
				{
					raidGoods(pCity);
				}
			}
			// R&R, ray, Natives raiding party - END

			getGroup()->clearMissionQueue();
		}
	}
}


bool CvUnit::isActionRecommended(int iAction)
{
	CvCity* pWorkingCity;
	ImprovementTypes eImprovement;
	ImprovementTypes eFinalImprovement;
	BuildTypes eBuild;
	RouteTypes eRoute;
	BonusTypes eBonus;
	int iIndex;

	if (getOwnerINLINE() != GC.getGameINLINE().getActivePlayer())
	{
		return false;
	}

	if (GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_NO_UNIT_RECOMMENDATIONS))
	{
		return false;
	}

	CyUnit* pyUnit = new CyUnit(this);
	CyArgsList argsList;
	argsList.add(gDLL->getPythonIFace()->makePythonObject(pyUnit));	// pass in unit class
	argsList.add(iAction);
	long lResult=0;
	gDLL->getPythonIFace()->callFunction(PYGameModule, "isActionRecommended", argsList.makeFunctionArgs(), &lResult);
	delete pyUnit;	// python fxn must not hold on to this pointer
	if (lResult == 1)
	{
		return true;
	}

	CvPlot* pPlot = gDLL->getInterfaceIFace()->getGotoPlot();
	if (pPlot == NULL)
	{
		if (gDLL->shiftKey())
		{
			pPlot = getGroup()->lastMissionPlot();
		}
	}

	if (pPlot == NULL)
	{
		pPlot = plot();
	}

	switch (GC.getActionInfo(iAction).getMissionType())
	{
	case MISSION_FORTIFY:
		if (pPlot->isCity(true, getTeam()))
		{
			if (canDefend(pPlot) && !isUnarmed())
			{
				if (pPlot->getNumDefenders(getOwnerINLINE()) < ((atPlot(pPlot)) ? 2 : 1))
				{
					return true;
				}
			}
		}
		break;
	case MISSION_HEAL:
		if (isHurt())
		{
			if (!hasMoved())
			{
				if ((pPlot->getTeam() == getTeam()) || (healTurns(pPlot) < 4))
				{
					return true;
				}
			}
		}
		break;

	case MISSION_FOUND:
		if (canFound(pPlot))
		{
			if (pPlot->isBestAdjacentFound(getOwnerINLINE()))
			{
				return true;
			}
		}
		break;

	case MISSION_BUILD:
		if (pPlot->getOwner() == getOwnerINLINE())
		{
			eBuild = ((BuildTypes)(GC.getActionInfo(iAction).getMissionData()));
			FAssert(eBuild != NO_BUILD);
			FAssertMsg(eBuild < GC.getNumBuildInfos(), "Invalid Build");

			if (canBuild(pPlot, eBuild))
			{
				eImprovement = ((ImprovementTypes)(GC.getBuildInfo(eBuild).getImprovement()));
				eRoute = ((RouteTypes)(GC.getBuildInfo(eBuild).getRoute()));
				eBonus = pPlot->getBonusType();
				pWorkingCity = pPlot->getWorkingCity();

				if (pPlot->getImprovementType() == NO_IMPROVEMENT)
				{
					if (pWorkingCity != NULL)
					{
						iIndex = pWorkingCity->getCityPlotIndex(pPlot);

						if (iIndex != -1)
						{
							if (pWorkingCity->AI_getBestBuild(iIndex) == eBuild)
							{
								return true;
							}
						}
					}

					if (eImprovement != NO_IMPROVEMENT)
					{
						if (pPlot->getImprovementType() == NO_IMPROVEMENT)
						{
							if (pWorkingCity != NULL)
							{
								// R&R, ray, maybe change this
								if (GC.getImprovementInfo(eImprovement).getYieldIncrease(YIELD_FOOD) > 0)
								{
									return true;
								}
							}
						}
					}
				}

				if (eRoute != NO_ROUTE)
				{
					if (!(pPlot->isRoute()))
					{
						if (eBonus != NO_BONUS)
						{
							return true;
						}

						if (pWorkingCity != NULL)
						{
							if (pPlot->isRiver())
							{
								return true;
							}
						}
					}

					eFinalImprovement = eImprovement;

					if (eFinalImprovement == NO_IMPROVEMENT)
					{
						eFinalImprovement = pPlot->getImprovementType();
					}

					if (eFinalImprovement != NO_IMPROVEMENT)
					{
						for (int i = 0; i < NUM_YIELD_TYPES; ++i)
						{
							if (GC.getImprovementInfo(eFinalImprovement).getRouteYieldChanges(eRoute, (YieldTypes)i) > 0)
							{
								return true;
							}
						}
					}
				}
			}
		}
		break;

	//TAC Whaling, ray
	case MISSION_WHALING:
		if (isWhalingBoat() && canGatherResource(pPlot))
		{
			return true;
		}
		break;
	//End TAC Whaling, ray

	// R&R, ray, High Sea Fishing - START
	case MISSION_FISHING:
		if (isFishingBoat() && canGatherResource(pPlot))
		{
			return true;
		}
		break;
	// R&R, ray, High Sea Fishing - END

	default:
		break;
	}

	if (GC.getActionInfo(iAction).getAutomateType() == AUTOMATE_SAIL || GC.getActionInfo(iAction).getCommandType() == COMMAND_SAIL_TO_EUROPE)
	{
		CLinkList<IDInfo> listCargo;
		getGroup()->buildCargoUnitList(listCargo);
		CLLNode<IDInfo>* pUnitNode = listCargo.head();
		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = listCargo.next(pUnitNode);

			if (pLoopUnit->getYield() != NO_YIELD && GET_PLAYER(getOwnerINLINE()).isYieldEuropeTradable(pLoopUnit->getYield()))
			{
				return true;
			}

			if (pLoopUnit->getUnitInfo().isTreasure())
			{
				return true;
			}
		}

		if (getCargo() == 0)
		{
			if (GET_PLAYER(getOwnerINLINE()).getNumEuropeUnits() > 0)
			{
				return true;
			}
		}
	}

	switch (GC.getActionInfo(iAction).getCommandType())
	{
	case COMMAND_PROMOTION:
	case COMMAND_PROMOTE:
	case COMMAND_KING_TRANSPORT:
	case COMMAND_ESTABLISH_MISSION:
	/*case COMMAND_STIR_UP_NATIVES: // R&R, ray , Stirring Up Natives - START
		return false; // Eventually AI could be coded here
		break;*/
	case COMMAND_SPEAK_WITH_CHIEF:
	case COMMAND_YIELD_TRADE:
	case COMMAND_LEARN:
	case COMMAND_ESTABLISH_TRADE_POST: // WTP, ray, Native Trade Posts - START
		return true;
		break;
	default:
		break;
	}

	return false;
}


bool CvUnit::isBetterDefenderThan(const CvUnit* pDefender, const CvUnit* pAttacker, bool bBreakTies) const
{
	int iOurDefense;
	int iTheirDefense;

	if (pDefender == NULL)
	{
		return true;
	}

	TeamTypes eAttackerTeam = NO_TEAM;
	if (NULL != pAttacker)
	{
		eAttackerTeam = pAttacker->getTeam();
	}

	if (canCoexistWithEnemyUnit(eAttackerTeam))
	{
		return false;
	}

	if (!canDefend())
	{
		return false;
	}

	if (canDefend() && !(pDefender->canDefend()))
	{
		return true;
	}

	bool bOtherUnarmed = pDefender->isUnarmed();
	if (isUnarmed() != bOtherUnarmed)
	{
		return bOtherUnarmed;
	}

	iOurDefense = currCombatStr(plot(), pAttacker);

	iOurDefense /= (getCargo() + 1);

	iTheirDefense = pDefender->currCombatStr(plot(), pAttacker);

	iTheirDefense /= (pDefender->getCargo() + 1);

	if (iOurDefense == iTheirDefense)
	{
		if (isOnMap() && !pDefender->isOnMap())
		{
			++iOurDefense;
		}
		else if (!isOnMap() && pDefender->isOnMap())
		{
			++iTheirDefense;
		}
		if (NO_UNIT == getLeaderUnitType() && NO_UNIT != pDefender->getLeaderUnitType())
		{
			++iOurDefense;
		}
		else if (NO_UNIT != getLeaderUnitType() && NO_UNIT == pDefender->getLeaderUnitType())
		{
			++iTheirDefense;
		}
		else if (bBreakTies && isBeforeUnitCycle(this, pDefender))
		{
			++iOurDefense;
		}
	}

	return (iOurDefense > iTheirDefense);
}


bool CvUnit::canDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible, bool bTestBusy)
{
	CvUnit* pUnit;

	if (bTestBusy && getGroup()->isBusy())
	{
		return false;
	}

	switch (eCommand)
	{
	case COMMAND_PROMOTION:
		if (canPromote((PromotionTypes)iData1, iData2))
		{
			return true;
		}
		break;

	case COMMAND_UPGRADE:
		if (canUpgrade(((UnitTypes)iData1), bTestVisible))
		{
			return true;
		}
		break;

	case COMMAND_AUTOMATE:
		if (canAutomate((AutomateTypes)iData1))
		{
			return true;
		}
		break;

	case COMMAND_WAKE:
		if (!isAutomated() && isWaiting())
		{
			return true;
		}
		break;

	case COMMAND_CANCEL:
	case COMMAND_CANCEL_ALL:
		if (!isAutomated() && (getGroup()->getLengthMissionQueue() > 0))
		{
			return true;
		}
		break;

	case COMMAND_STOP_AUTOMATION:
		if (isAutomated())
		{
			return true;
		}
		break;

	case COMMAND_DELETE:
		if (canScrap())
		{
			return true;
		}
		break;

	case COMMAND_GIFT:
		if (canGift(bTestVisible))
		{
			return true;
		}
		break;

	case COMMAND_LOAD:
		if (canLoad(plot(), true))
		{
			return true;
		}
		break;

	case COMMAND_LOAD_YIELD:
		if (canLoadYield(plot(), (YieldTypes) iData1, false))
		{
			return true;
		}
		break;

	case COMMAND_LOAD_CARGO:
		if (canLoadYield(plot(), NO_YIELD, false))
		{
			return true;
		}
		break;

	case COMMAND_LOAD_UNIT:
		pUnit = ::getUnit(IDInfo(((PlayerTypes)iData1), iData2));
		if (pUnit != NULL)
		{
			if (canLoadUnit(pUnit, plot(), true))
			{
				return true;
			}
		}
		break;

	case COMMAND_YIELD_TRADE:
		if (canTradeYield(plot()))
		{
			return true;
		}
		break;

	case COMMAND_SAIL_TO_EUROPE:
		if (canCrossOcean(plot(), (UnitTravelStates)iData1))
		{
			return true;
		}
		break;
	/*** TRIANGLETRADE 10/25/08 by DPII ***/
	case COMMAND_SAIL_TO_AFRICA:
		if (canSailToAfrica(plot()))
		{
			return true;
		}
		break;
	
	// R&R, ray, Port Royal
	case COMMAND_SAIL_TO_PORT_ROYAL:
		if (canSailToPortRoyal(plot()))
		{
			return true;
		}
		break;

	case COMMAND_CHOOSE_TRADE_ROUTES:
	case COMMAND_ASSIGN_TRADE_ROUTE:
		if (iData2 == 0 || canAssignTradeRoute(iData1))
		{
			return true;
		}
		break;

	case COMMAND_PROMOTE:
		{
			CvSelectionGroup* pSelection = gDLL->getInterfaceIFace()->getSelectionList();
			if (pSelection != NULL)
			{
				if (pSelection->isPromotionReady())
				{
					return true;
				}
			}
		}
		break;

	//TAC Whaling, ray
	case COMMAND_PROFESSION:
		{
			if (iData1 == -1)
			{
				CvSelectionGroup* pSelection = gDLL->getInterfaceIFace()->getSelectionList();
				if (pSelection != NULL)
				{
					if (pSelection->canChangeProfession())
					{
						if (!getUnitInfo().isGatherBoat())
						{
							return true;
						}
					}
				}
			}
			else
			{
				if (canHaveProfession((ProfessionTypes) iData1, false))
				{
					if (!getUnitInfo().isGatherBoat())
					{
						return true;
					}
				}
			}
		}
		break;
	//End TAC Whaling, ray

	case COMMAND_CLEAR_SPECIALTY:
		if (canClearSpecialty())
		{
			return true;
		}
		break;

	case COMMAND_UNLOAD:
		if (canUnload())
		{
			return true;
		}
		break;

	case COMMAND_UNLOAD_ALL:
		if (canUnloadAll())
		{
			return true;
		}
		break;

	case COMMAND_LEARN:
		if (canLearn())
		{
			return true;
		}
		break;

	case COMMAND_KING_TRANSPORT:
		if (canKingTransport())
		{
			return true;
		}
		break;

	case COMMAND_ESTABLISH_MISSION:
		if (canEstablishMission())
		{
			return true;
		}
		break;

	// WTP, ray, Native Trade Posts - START
	case COMMAND_ESTABLISH_TRADE_POST:
		if (canEstablishTradePost())
		{
			return true;
		}
		break;

	// R&R, ray , Stirring Up Natives - START
	case COMMAND_STIR_UP_NATIVES:
		if (canStirUp())
		{
			return true;
		}
		break;
	// R&R, ray , Stirring Up Natives - END

	case COMMAND_SPEAK_WITH_CHIEF:
		if (canSpeakWithChief(plot()))
		{
			return true;
		}
		break;

	case COMMAND_HOTKEY:
		if (isGroupHead())
		{
			return true;
		}
		break;
	
	// TAC - Goto Menu - koma13 - START
	case COMMAND_GOTO_MENU:
		if (getTransportUnit() == NULL || plot()->isValidDomainForAction(getUnitType()))
		{
			// WTP, ray, prevent Coastal Ships to Display EUROPE, AFRICA and Port Royal in GO-TO -START
			// Erik: Disable the goto menu for coastal transports for now until we figure
			// out how to filter the unreachable cities
			// if (canCrossCoastOnly())
			// {
			//	 return false;
			// }			
			// WTP, ray, prevent Coastal Ships to Display EUROPE, AFRICA and Port Royal in GO-TO -END
			return true;
			/*
			if (canCrossOcean(plot(), UNIT_TRAVEL_STATE_TO_EUROPE) || canAutoCrossOcean(plot()))
			{
				return true;
			}
			
			int iLoop;
			for (CvCity* pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop))
			{
				if (plot() != pLoopCity->plot())
				{
					if (generatePath(pLoopCity->plot(), 0, false))
					{
						return true;
					}
				}
			}
			*/
		}
		break;
	// TAC - Goto Menu - koma13 - END

	// TAC - Trade Routes Advisor - koma13 - START
	case COMMAND_IGNORE_DANGER:
		if (isGroupHead())
		{
			return true;
		}
		break;
	// TAC - Trade Routes Advisor - koma13 - END
	
	// WTP, merge Treasures, of Raubwuerger - START
	case COMMAND_MERGE_TREASURES:
		if (canMergeTreasures())
		{
			return true;
		}
		break;
	// R&R, ray , Stirring Up Natives - - END

	default:
		FAssert(false);
		break;
	}

	return false;
}


void CvUnit::doCommand(CommandTypes eCommand, int iData1, int iData2)
{
	CvUnit* pUnit;
	bool bCycle;

	bCycle = false;

	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (canDoCommand(eCommand, iData1, iData2))
	{
		switch (eCommand)
		{
		case COMMAND_PROMOTION:
			promote((PromotionTypes)iData1, iData2);
			break;

		case COMMAND_UPGRADE:
			upgrade((UnitTypes)iData1);
			bCycle = true;
			break;

		case COMMAND_AUTOMATE:
			automate((AutomateTypes)iData1);
			bCycle = true;
			break;

		case COMMAND_WAKE:
			getGroup()->setActivityType(ACTIVITY_AWAKE);
			break;

		case COMMAND_CANCEL:
			//TAC Whaling, ray
			if (isGatheringResource())
			{
				convertFishingBoat();
				setGatheringResource(false);
			}
			//End TAC Whaling, ray
			getGroup()->popMission();
			break;

		case COMMAND_CANCEL_ALL:
			//TAC Whaling, ray
			if (isGatheringResource())
			{
				convertFishingBoat();
				setGatheringResource(false);
			}
			//End TAC Whaling, ray
			getGroup()->clearMissionQueue();
			break;

		case COMMAND_STOP_AUTOMATION:
			//TAC Whaling, ray
			if (isGatheringResource())
			{
				convertFishingBoat();
				setGatheringResource(false);
			}
			//End TAC Whaling, ray
			getGroup()->setAutomateType(NO_AUTOMATE);
			break;

		case COMMAND_DELETE:
			scrap();
			bCycle = true;
			break;

		case COMMAND_GIFT:
			gift();
			bCycle = true;
			break;

		case COMMAND_LOAD:
			load(true);
			bCycle = true;
			break;

		case COMMAND_LOAD_YIELD:
			{
				if (iData2 >= 0)
				{
					loadYieldAmount((YieldTypes) iData1, iData2, false);
				}
				else
				{
					loadYield((YieldTypes) iData1, false);
				}
			}
			break;

		case COMMAND_LOAD_CARGO:
			{
				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_LOAD_CARGO);
				gDLL->getInterfaceIFace()->addPopup(pInfo);
			}
			break;

		case COMMAND_LOAD_UNIT:
			pUnit = ::getUnit(IDInfo(((PlayerTypes)iData1), iData2));
			if (pUnit != NULL)
			{
				loadUnit(pUnit);
				bCycle = true;
			}
			break;

		case COMMAND_YIELD_TRADE:
			tradeYield();
			break;

		case COMMAND_SAIL_TO_EUROPE:
			if (iData2 != NO_EUROPE)
			{
				setSailEurope((EuropeTypes) iData2);
			}
			crossOcean((UnitTravelStates) iData1);
			break;
		/*** TRIANGLETRADE 10/25/08 by DPII ***/
		case COMMAND_SAIL_TO_AFRICA:
			sailToAfrica();
		// R&R, ray, Port Royal
		case COMMAND_SAIL_TO_PORT_ROYAL:
			sailToPortRoyal();

		case COMMAND_CHOOSE_TRADE_ROUTES:
			// TAC - Trade Routes Advisor - koma13 - START
			/*
			if (GET_PLAYER(getOwnerINLINE()).getNumTradeRoutes() > 0)
			{
				if (gDLL->getInterfaceIFace()->getHeadSelectedUnit() == this)
				{
					CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_TRADE_ROUTES, getID());
					gDLL->getInterfaceIFace()->addPopup(pInfo, getOwnerINLINE(), true, true);
				}
			}
			else
			{
			}
			*/
			if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
			{
				if (gDLL->getInterfaceIFace()->getHeadSelectedUnit() == this)
				{
					CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN);
					pInfo->setText(L"showTradeRoutesAdvisor");
					gDLL->getInterfaceIFace()->addPopup(pInfo, getOwnerINLINE(), false);
				}
			}
			// TAC - Trade Routes Advisor - koma13 - END

			break;

		case COMMAND_ASSIGN_TRADE_ROUTE:
			if (isGroupHead())
			{
				getGroup()->assignTradeRoute(iData1, iData2);
			}
			break;

		case COMMAND_PROMOTE:
			if (gDLL->getInterfaceIFace()->getHeadSelectedUnit() == this)
			{
				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_PROMOTE);
				pInfo->setData1(getGroupID());
				gDLL->getInterfaceIFace()->addPopup(pInfo, getOwnerINLINE(), true, true);
			}
			break;

		case COMMAND_PROFESSION:
			if (iData1 == -1)
			{
				if (gDLL->getInterfaceIFace()->getHeadSelectedUnit() == this)
				{
					CvPlot* pPlot = plot();
					if (pPlot != NULL)
					{
						CvCity* pCity = pPlot->getPlotCity();
						if (pCity != NULL)
						{
							CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CHOOSE_PROFESSION, pCity->getID(), getID());
							gDLL->getInterfaceIFace()->addPopup(pInfo, getOwnerINLINE(), true, true);
						}
					}
				}
			}
			else
			{
				setProfession((ProfessionTypes) iData1);
			}
			break;

		case COMMAND_CLEAR_SPECIALTY:
			clearSpecialty();
			break;

		case COMMAND_UNLOAD:
			if (iData2 >= 0)
			{
				FAssert(iData1 == getYield());
				FAssert(getYield() != NO_YIELD);
				unloadStoredAmount(iData2);
			}
			else
			{
				unload();
			}
			bCycle = true;
			break;

		case COMMAND_UNLOAD_ALL:
			unloadAll();
			bCycle = true;
			break;

		case COMMAND_LEARN:
			learn();
			bCycle = true;
			break;

		case COMMAND_KING_TRANSPORT:
			kingTransport(false);
			bCycle = true;
			break;

		case COMMAND_ESTABLISH_MISSION:
			establishMission();
			bCycle = true;
			break;

		// WTP, ray, Native Trade Posts - START
		case COMMAND_ESTABLISH_TRADE_POST:
			establishTradePost();
			bCycle = true;
			break;

		// R&R, ray , Stirring Up Natives - START
		case COMMAND_STIR_UP_NATIVES:
			if(isGroupHead())
			{
				stirUpNatives();
			}
			//bCycle = true; // test
			break;
		// R&R, ray , Stirring Up Natives - END

		case COMMAND_SPEAK_WITH_CHIEF:
			if(isGroupHead())
			{
				getGroup()->speakWithChief();
			}
			break;

		case COMMAND_HOTKEY:
			setHotKeyNumber(iData1);
			break;
		
		// TAC - Goto Menu - koma13 - START
		case COMMAND_GOTO_MENU:
			if (gDLL->getInterfaceIFace()->getHeadSelectedUnit() == this)
			{
				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_GOTO_MENU);
				pInfo->setData1(getID());
				gDLL->getInterfaceIFace()->addPopup(pInfo, getOwnerINLINE(), true, true);
			}
			break;
		// TAC - Goto Menu - koma13 - END

		// TAC - Trade Routes Advisor - koma13 - START
		case COMMAND_IGNORE_DANGER:
			if (iData1 > 0)
			{
				setIgnoreDanger(true);
			}
			else
			{
				setIgnoreDanger(false);
			}
			break;
		// TAC - Trade Routes Advisor - koma13 - END

		// WTP, merge Treasures, of Raubwuerger - START
		// ray, small improvement
		case COMMAND_MERGE_TREASURES:
			if(isGroupHead())
			{
				mergeTreasures();
			}
			break;
		// R&R, ray , Stirring Up Natives - - END
		
		default:
			FAssert(false);
			break;
		}
	}

	if (bCycle)
	{
		if (IsSelected())
		{
			gDLL->getInterfaceIFace()->setCycleSelectionCounter(1);
		}
	}

	if (getGroup() != NULL)
	{
		getGroup()->doDelayedDeath();
	}
}


FAStarNode* CvUnit::getPathLastNode() const
{
	return getGroup()->getPathLastNode();
}


CvPlot* CvUnit::getPathEndTurnPlot() const
{
	return getGroup()->getPathEndTurnPlot();
}

int CvUnit::getPathCost() const
{
	return getGroup()->getPathCost();
}

// TAC - AI Improved Naval AI - koma13 - START
/*
bool CvUnit::generatePath(const CvPlot* pToPlot, int iFlags, bool bReuse, int* piPathTurns) const
{
	return getGroup()->generatePath(plot(), pToPlot, iFlags, bReuse, piPathTurns);
}
*/

bool CvUnit::generatePath(const CvPlot* pToPlot, int iFlags, bool bReuse, int* piPathTurns, bool bIgnoreDanger) const
{
	return getGroup()->generatePath(plot(), pToPlot, iFlags, bReuse, piPathTurns, bIgnoreDanger);
}
// TAC - AI Improved Naval AI - koma13 - END

bool CvUnit::canEnterTerritory(PlayerTypes ePlayer, bool bIgnoreRightOfPassage) const
{
	if (ePlayer == NO_PLAYER)
	{
		return true;
	}

	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

	if (GET_TEAM(getTeam()).isFriendlyTerritory(eTeam))
	{
		return true;
	}

	// R&R, ray, changes to Wild Animals - START
	if (getUnitInfo().isAnimal())
	{
		return false;
	}
	// R&R, ray, changes to Wild Animals - END

	// R&R, ray, Fleeing Units START
	if(AI_getUnitAIType() == UNITAI_FLEEING)
	{
		return false;
	}
	// R&R, ray, Fleeing Units - END

	if (isEnemy(eTeam))
	{
		return true;
	}

	if (isRivalTerritory())
	{
		return true;
	}

	if (alwaysInvisible())
	{
		return true;
	}

	if (GET_PLAYER(getOwnerINLINE()).isAlwaysOpenBorders())
	{
		return true;
	}

	if (GET_PLAYER(ePlayer).isAlwaysOpenBorders())
	{
		return true;
	}

	if (!bIgnoreRightOfPassage)
	{
		if (GET_TEAM(getTeam()).isOpenBorders(eTeam))
		{
			return true;
		}
	}

	return false;
}


bool CvUnit::canEnterArea(PlayerTypes ePlayer, const CvArea* pArea, bool bIgnoreRightOfPassage) const
{
	if (!canEnterTerritory(ePlayer, bIgnoreRightOfPassage))
	{
		return false;
	}

	return true;
}

// Returns the ID of the team to declare war against
TeamTypes CvUnit::getDeclareWarUnitMove(const CvPlot* pPlot) const
{
	FAssert(isHuman());

	if (!pPlot->isVisible(getTeam(), false))
	{
		return NO_TEAM;
	}

	bool bCityThreat = canAttack() && !isNoCityCapture() && getDomainType() == DOMAIN_LAND;
	if (getProfession() != NO_PROFESSION && GC.getProfessionInfo(getProfession()).isScout())
	{
		bCityThreat = false;
	}

	//check territory
	TeamTypes eRevealedTeam = pPlot->getRevealedTeam(getTeam(), false);
	PlayerTypes eRevealedPlayer = pPlot->getRevealedOwner(getTeam(), false);
	if (eRevealedTeam != NO_TEAM)
	{
		if (GET_TEAM(getTeam()).canDeclareWar(pPlot->getTeam()))
		{
			if (!canEnterArea(eRevealedPlayer, pPlot->area()))
			{
				return eRevealedTeam;
			}

			if(getDomainType() == DOMAIN_SEA && !canCargoEnterArea(eRevealedPlayer, pPlot->area(), false) && getGroup()->isAmphibPlot(pPlot))
			{
				return eRevealedTeam;
			}

			if (pPlot->isCity() && bCityThreat)
			{
				if (GET_PLAYER(eRevealedPlayer).isAlwaysOpenBorders())
				{
					return eRevealedTeam;
				}
			}
		}
	}

	//check unit
	if (canMoveInto(pPlot, true, true))
	{
		CvUnit* pUnit = pPlot->plotCheck(PUF_canDeclareWar, getOwnerINLINE(), isAlwaysHostile(pPlot), NO_PLAYER, NO_TEAM, PUF_isVisible, getOwnerINLINE());
		if (pUnit != NULL)
		{
			if (!pPlot->isCity() || bCityThreat)
			{
				return pUnit->getTeam();
			}
		}
	}

	return NO_TEAM;
}

bool CvUnit::canMoveInto(const CvPlot* pPlot, bool bAttack, bool bDeclareWar, bool bIgnoreLoad) const
{
	FAssertMsg(pPlot != NULL, "Plot is not assigned a valid value");

	if (atPlot(pPlot))
	{
		return false;
	}

	if (pPlot->isImpassable())
	{
		if (!canMoveImpassable())
		{
			return false;
		}
	}

	const FeatureTypes eFeature = pPlot->getFeatureType();

	// Prevent the AI from moving through storms and sustaining damage
	// Strictly speaking this should preferably be handled by either the path cost or a separate PF flag
	if (getGroup()->AI_isControlled() && (eFeature != NO_FEATURE) &&  GC.getFeatureInfo(eFeature).getTurnDamage() > 0)
	{
		return false;
	}

	CvArea *pPlotArea = pPlot->area();
	TeamTypes ePlotTeam = pPlot->getTeam();
	bool bCanEnterArea = canEnterArea(pPlot->getOwnerINLINE(), pPlotArea);
	if (bCanEnterArea)
	{	
		if (eFeature != NO_FEATURE)
		{
			if (m_pUnitInfo->getFeatureImpassable(eFeature))
			{
				if (DOMAIN_SEA != getDomainType() || ePlotTeam != getTeam()) // sea units can enter impassable in own cultural borders
				{
					return false;
				}
				// Erik: The AI may attempt to pathfind into unrevealed ocean plots so have have to check for the plot being revealed
				if (DOMAIN_SEA == getDomainType() && !pPlot->isRevealed(getTeam(), false))
				{
					return false;
				}
			}
		}

		if (m_pUnitInfo->getTerrainImpassable(pPlot->getTerrainType()))
		{
			if (DOMAIN_SEA != getDomainType() || ePlotTeam != getTeam()) // sea units can enter impassable in own cultural borders
			{
				if (bIgnoreLoad || !canLoad(pPlot, true))
				{
					return false;
				}
			}
		}
	}

	if (m_pUnitInfo->getMoves() == 0)
	{
		return false;
	}

	switch (getDomainType())
	{
	case DOMAIN_SEA:
		if (!pPlot->isWater() && !m_pUnitInfo->isCanMoveAllTerrain())
		{
			if (!pPlot->isFriendlyCity(*this, true) || !pPlot->isCoastalLand())
			{
				return false;
			}
		}

		// PatchMod: Stop MoW's entering native settlements START
		if (pPlot->isCity() && GET_PLAYER(pPlot->getOwnerINLINE()).isNative())
		{
			if (GET_PLAYER(getOwnerINLINE()).isEurope())
			{
                return false;
			}
		}
		// PatchMod: Stop MoW's entering native settlements END
		// TAC - Pirates can't enter foreign settlements - START
		if (pPlot->isCity() && !GET_PLAYER(pPlot->getOwnerINLINE()).isNative() && pPlot->getOwnerINLINE() != getOwnerINLINE())
		{
			if (AI_getUnitAIType() == UNITAI_PIRATE_SEA)
			{
				return false;
			}
		}
		// TAC - Pirates can't enter foreign settlements - END

		break;

	case DOMAIN_LAND:

		// R&R, ray, Start Logik for Peaks
		if (pPlot->isPeak() && pPlot->getRouteType() == NO_ROUTE)
		{
			// Anything else than Natives, Animals, Pioneers, Scouts, Native Mercenaries and Ranges cannot pass Peaks without Roads
			/*
			if (!isNative() && !m_pUnitInfo->isAnimal() && !(getProfession() != NO_PROFESSION && GC.getProfessionInfo(getProfession()).getWorkRate() != 0) && !m_pUnitInfo->isNoBadGoodies() && m_pUnitInfo->getUnitClassType() != GC.getDefineINT("UNITCLASS_NATIVE_MERC") && m_pUnitInfo->getUnitClassType() != GC.getDefineINT("UNITCLASS_RANGER"))
			*/
			/// Move Into Peak - start - Nightinggale
			// replaced DLL hardcoding with pure XML setup
			// it's more friendly to xml modders and it's actually faster at runtime because less data will have to be checked
			if (!(m_pUnitInfo->allowsMoveIntoPeak() || (getProfession() != NO_PROFESSION && GC.getProfessionInfo(getProfession()).allowsMoveIntoPeak())))
			/// Move Into Peak - end - Nightinggale
			{
				return false;
			}
		}
		// R&R, ray, End Logik for Peaks

		// R&R, ray, Changes for Treasures, START
		if (m_pUnitInfo->isTreasure() && pPlot->isGoody())
		{
			return false;
		}

		// R&R, ray, Monasteries and Forts - START
		if (m_pUnitInfo->isAnimal())
		{
			if (pPlot->isFort() || pPlot->isMonastery() || pPlot->isCity())
			{
				return false;
			}
		}
		// R&R, ray, Monasteries and Forts - END

		//removed due to problems
		//if (getUnitInfo().isNoRevealMap() && ( !pPlot->isRevealed(getTeam(), false) || pPlot->isAdjacentNonrevealed(getTeam())))
		//{
		//	return false;
		//}
		// R&R, ray, Changes for Treasures, END

		// R&R, ray, Start Logic for Trains
		if (getUnitInfo().getCargoSpace() == 6) // easisest way to identify train
		{ 
			//Check if the Plot has railroad
			if (pPlot->getRouteType() != 2)
			{
				return false;
			}
		}
		// R&R, ray, End Logic for Trains

		if (pPlot->isWater() && !m_pUnitInfo->isCanMoveAllTerrain())
		{
			if (bIgnoreLoad || plot()->isWater() || !canLoad(pPlot, false))
			{
				return false;
			}
		}
		break;

	case DOMAIN_IMMOBILE:
		return false;
		break;

	default:
		FAssert(false);
		break;
	}

	if (!bAttack)
	{
		if (isNoCityCapture() && pPlot->isEnemyCity(*this))
		{
			return false;
		}
	}

	if (bAttack)
	{
		if (isMadeAttack() && !isBlitz())
		{
			return false;
		}
	}

	if (canAttack())
	{
		if (bAttack || !canCoexistWithEnemyUnit(NO_TEAM))
		{
			if (!isHuman() || (pPlot->isVisible(getTeam(), false)))
			{
				if (pPlot->isVisibleEnemyUnit(this) != bAttack)
				{
					//FAssertMsg(isHuman() || (!bDeclareWar || (pPlot->isVisibleOtherUnit(getOwnerINLINE()) != bAttack)), "hopefully not an issue, but tracking how often this is the case when we dont want to really declare war");
					if (!bDeclareWar || (pPlot->isVisibleOtherUnit(getOwnerINLINE()) != bAttack && !(bAttack && pPlot->getPlotCity() && !isNoCityCapture())))
					{
						return false;
					}
				}
			}
		}
	}
	else
	{
		if (bAttack)
		{
			return false;
		}

		if (!canCoexistWithEnemyUnit(NO_TEAM))
		{
			if (!isHuman() || pPlot->isVisible(getTeam(), false))
			{
				if (pPlot->isEnemyCity(*this))
				{
					return false;
				}

				if (pPlot->isVisibleEnemyUnit(this))
				{
					return false;
				}
			}
		}
	}

	if (isHuman())
	{
		ePlotTeam = pPlot->getRevealedTeam(getTeam(), false);
		bCanEnterArea = canEnterArea(pPlot->getRevealedOwner(getTeam(), false), pPlotArea);
	}

	if (!bCanEnterArea)
	{
		FAssert(ePlotTeam != NO_TEAM);

		if (!(GET_TEAM(getTeam()).canDeclareWar(ePlotTeam)))
		{
			return false;
		}

		if (isHuman())
		{
			if (!bDeclareWar)
			{
				return false;
			}
		}
		else
		{
			if (GET_TEAM(getTeam()).AI_isSneakAttackReady(ePlotTeam))
			{
				if (!(getGroup()->AI_isDeclareWar(pPlot)))
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
	}

	if (GC.getUSE_UNIT_CANNOT_MOVE_INTO_CALLBACK())
	{
		// Python Override
		CyArgsList argsList;
		argsList.add(getOwnerINLINE());	// Player ID
		argsList.add(getID());	// Unit ID
		argsList.add(pPlot->getX());	// Plot X
		argsList.add(pPlot->getY());	// Plot Y
		long lResult=0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "unitCannotMoveInto", argsList.makeFunctionArgs(), &lResult);

		if (lResult != 0)
		{
			return false;
		}
	}

	return true;
}


bool CvUnit::canMoveOrAttackInto(const CvPlot* pPlot, bool bDeclareWar) const
{
	return (canMoveInto(pPlot, false, bDeclareWar) || canMoveInto(pPlot, true, bDeclareWar));
}


bool CvUnit::canMoveThrough(const CvPlot* pPlot) const
{
	return canMoveInto(pPlot, false, false, true);
}


void CvUnit::attack(CvPlot* pPlot, bool bQuick)
{
	FAssert(canMoveInto(pPlot, true));
	FAssert(getCombatTimer() == 0);

	setAttackPlot(pPlot);

	updateCombat(bQuick);
}

void CvUnit::move(CvPlot* pPlot, bool bShow)
{
	FAssert(canMoveOrAttackInto(pPlot) || isMadeAttack());

	CvPlot* pOldPlot = plot();

	//TAC Whaling, ray
	if (isGatheringResource())
	{
		convertFishingBoat();
		setGatheringResource(false);
	
		return;
	}
	//End TAC Whaling, ray
	
	changeMoves(pPlot->movementCost(this, plot()));

	setXY(pPlot->getX_INLINE(), pPlot->getY_INLINE(), true, true, bShow, bShow);
	
	// R&R, Robert Surcouf, Damage on Storm plots, Start
	// R&R, bugfix: we only damage ships on Sea and not the transported units, ray, START
	if (getDomainType() == DOMAIN_SEA)
	{
		FeatureTypes eFeature = plot()->getFeatureType();
		if (NO_FEATURE != eFeature)
		{
			int iPotentialDamageFromFeaturePercent = GC.getFeatureInfo(eFeature).getTurnDamage();
			if (0 != iPotentialDamageFromFeaturePercent)
			{
				// R&R, bugfix: we never destroy a unit from feature damage, ray, START
				int iPotentialDamage = (maxHitPoints() * iPotentialDamageFromFeaturePercent) / 100;
				if (currHitPoints() > iPotentialDamage)
				{
					//changeDamage(GC.getFeatureInfo(eFeature).getTurnDamage(), NO_PLAYER); Version Beyond The Sword
					changeDamage(iPotentialDamage, NULL);
				}
				// R&R, bugfix: we never destroy a unit from feature damage, ray, END
			}
		}
	}
	// R&R, Robert Surcouf, Damage on Storm plots, End
	
	//change feature
	FeatureTypes featureType = pPlot->getFeatureType();
	if(featureType != NO_FEATURE)
	{
		CvString featureString(GC.getFeatureInfo(featureType).getOnUnitChangeTo());
		if(!featureString.IsEmpty())
		{
			FeatureTypes newFeatureType = (FeatureTypes) GC.getInfoTypeForString(featureString);
			pPlot->setFeatureType(newFeatureType);
		}
	}

	if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
	{
		if (!(pPlot->isOwned()))
		{
			//spawn birds if trees present - JW
			if (featureType != NO_FEATURE)
			{
				if (GC.getASyncRand().get(100) < GC.getFeatureInfo(featureType).getEffectProbability())
				{
					EffectTypes eEffect = (EffectTypes)GC.getInfoTypeForString(GC.getFeatureInfo(featureType).getEffectType());
					gDLL->getEngineIFace()->TriggerEffect(eEffect, pPlot->getPoint(), (float)(GC.getASyncRand().get(360)));
					gDLL->getInterfaceIFace()->playGeneralSound("AS3D_UN_BIRDS_SCATTER", pPlot->getPoint());
				}
			}
		}
	}

	gDLL->getEventReporterIFace()->unitMove(pPlot, this, pOldPlot);
}

// false if unit is killed
bool CvUnit::jumpToNearestValidPlot()
{
	FAssertMsg(!isAttacking(), "isAttacking did not return false as expected");
	FAssertMsg(!isFighting(), "isFighting did not return false as expected");

	CvCity* pNearestCity = GC.getMapINLINE().findCity(getX_INLINE(), getY_INLINE(), getOwnerINLINE());
	int iBestValue = MAX_INT;
	CvPlot* pBestPlot = NULL;

	for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

		if (isValidPlot(pLoopPlot))
		{
			if (canMoveInto(pLoopPlot))
			{
				FAssertMsg(!atPlot(pLoopPlot), "atPlot(pLoopPlot) did not return false as expected");

				if (pLoopPlot->isRevealed(getTeam(), false))
				{
					// Erik: TODO: Do not consider cities not owned by the player
					// For now we disallow all cities as teleport target
					if (pLoopPlot->isCity()/* && pLoopPlot->getPlotCity()->getTeam() != getTeam()*/)
					{ 
						continue;
					}
						
					// TAC - AI Mauritsstad Bugfix - koma13 - START
					bool bEuropeTravelstate = (getUnitTravelState() == UNIT_TRAVEL_STATE_FROM_EUROPE || getUnitTravelState() == UNIT_TRAVEL_STATE_IN_EUROPE || getUnitTravelState() == UNIT_TRAVEL_STATE_TO_EUROPE);
					if (getUnitTravelState() == NO_UNIT_TRAVEL_STATE || (bEuropeTravelstate && pLoopPlot->isEurope()))
					{
						int iValue = (plotDistance(getX_INLINE(), getY_INLINE(), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE()) * 2);

						if (pNearestCity != NULL)
						{
							iValue += plotDistance(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), pNearestCity->getX_INLINE(), pNearestCity->getY_INLINE());
						}	
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
					// TAC - AI Mauritsstad Bugfix - koma13 - END
				}
			}
		}
	}

	bool bValid = true;
	if (pBestPlot != NULL)
	{
		setXY(pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE());
	}
	else
	{
		kill(false);
		bValid = false;
	}

	return bValid;
}

bool CvUnit::isValidPlot(const CvPlot* pPlot) const
{
	if (!pPlot->isValidDomainForLocation(*this))
	{
		return false;
	}

	if (!canEnterArea(pPlot->getOwnerINLINE(), pPlot->area()))
	{
		return false;
	}

	TeamTypes ePlotTeam = pPlot->getTeam();
	if (ePlotTeam != NO_TEAM)
	{
		if (pPlot->isCity(true, ePlotTeam) && !canCoexistWithEnemyUnit(ePlotTeam) && isEnemy(ePlotTeam))
		{
			return false;
		}
	}

	return true;
}

int CvUnit::canCrossCoastOnly() const
{
	CLLNode<IDInfo>* pUnitNode = getGroup()->headUnitNode();

	// Determine if the unit may enter non-coast water plots
	while (pUnitNode != NULL)
	{
		CvUnit *pLoopUnit = ::getUnit(pUnitNode->m_data);

		if (pLoopUnit->getUnitInfo().getTerrainImpassable(TERRAIN_OCEAN))
		{
			return true;
		}
		pUnitNode = getGroup()->nextUnitNode(pUnitNode);
	}

	return false;
}

bool CvUnit::canAutomate(AutomateTypes eAutomate) const
{
	const bool canCrossOcean = !canCrossCoastOnly();
	CLLNode<IDInfo>* pUnitNode = NULL;

	if (eAutomate == NO_AUTOMATE)
	{
		return false;
	}

	switch (eAutomate)
	{
	case AUTOMATE_BUILD:
		if (workRate(true) <= 0)
		{
			return false;
		}
		break;

	case AUTOMATE_CITY:
		if (workRate(true) <= 0)
		{
			return false;
		}
		if (!plot()->isCityRadius())
		{
			return false;
		}
		if ((plot()->getWorkingCity() == NULL) || plot()->getWorkingCity()->getOwnerINLINE() != getOwnerINLINE())
		{
			return false;
		}
		break;

	case AUTOMATE_EXPLORE:
		if ((!canFight() && (getDomainType() != DOMAIN_SEA)) || (getDomainType() == DOMAIN_IMMOBILE))
		{
			return false;
		}
		break;

	case AUTOMATE_SAIL:
		if (!canCrossOcean || !canAutoCrossOcean(plot()))
		{
			return false;
		}
		break;

	/*** TRIANGLETRADE 10/28/08 by DPII ***/
	case AUTOMATE_SAIL_TO_AFRICA:
		if (!canCrossOcean || !canAutoCrossOcean(plot()))
		{
			return false;
		}
		break;
	
	// R&R, ray, Port Royal
	case AUTOMATE_SAIL_TO_PORT_ROYAL:	
		//RaR, ray, fix for Port Royal during WOI - START
		/*
		if (!canCrossOcean(plot(), UNIT_TRAVEL_STATE_TO_PORT_ROYAL)) // need to check
		{
			return false;
		}
		*/

		if (plot()->isEurope())
		{
			return false;
		}

		if (!GET_PLAYER(getOwnerINLINE()).canTradeWithPortRoyal())
		{
			return false;
		}

		if (getDomainType() != DOMAIN_SEA)
		{
			return false;
		}

		if (!plot()->isEuropeAccessable())
		{
			return false;
		}
		
		// Erik: We need to determine if all the selected units
		// can travel to Port Royal
		pUnitNode = getGroup()->headUnitNode();

		while (pUnitNode != NULL)
		{
			CvUnit *pLoopUnit = ::getUnit(pUnitNode->m_data);

			if (!pLoopUnit->getUnitInfo().isHiddenNationality())
			{
				return false;
			}
			pUnitNode = getGroup()->nextUnitNode(pUnitNode);
		}
		break;

	//TAC Whaling, ray
	case AUTOMATE_TRANSPORT_ROUTES:
		if (cargoSpace() == 0 || getUnitInfo().isGatherBoat())
		{
			return false;
		}
		break;

	case AUTOMATE_TRANSPORT_FULL:
		if (cargoSpace() == 0 || getUnitInfo().isGatherBoat())
		{
			return false;
		}
		break;

	case AUTOMATE_WORKER_SEA:
		if (!getUnitInfo().isGatherBoat())
		{
			return false;
		}
		break;
	//End TAC Whaling, ray

	case AUTOMATE_FULL:
		if (!GC.getGameINLINE().isDebugMode())
		{
			return false;
		}
		break;

	default:
		FAssert(false);
		break;
	}

	return true;
}


void CvUnit::automate(AutomateTypes eAutomate)
{
	if (canAutomate(eAutomate))
	{
		getGroup()->setAutomateType(eAutomate);
	}
}


bool CvUnit::canScrap() const
{
	if (plot()->isFighting())
	{
		return false;
	}

	return true;
}


void CvUnit::scrap()
{
	if (!canScrap())
	{
		return;
	}

	kill(true);
}


bool CvUnit::canGift(bool bTestVisible, bool bTestTransport)
{
	CvPlot* pPlot = plot();
	CvUnit* pTransport = getTransportUnit();
	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());

	if (!(pPlot->isOwned()))
	{
		return false;
	}

	if (pPlot->getOwnerINLINE() == getOwnerINLINE())
	{
		return false;
	}

	if (!GET_PLAYER(pPlot->getOwnerINLINE()).isProfessionValid(getProfession(), getUnitType()))
	{
		return false;
	}

	if (pPlot->isVisibleEnemyUnit(this))
	{
		return false;
	}

	if (pPlot->isVisibleEnemyUnit(pPlot->getOwnerINLINE()))
	{
		return false;
	}

	if (!pPlot->isValidDomainForLocation(*this) && NULL == pTransport)
	{
		return false;
	}

	if (hasCargo())
	{
		CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = pPlot->nextUnitNode(pUnitNode);

			if (pLoopUnit->getTransportUnit() == this)
			{
				if (!pLoopUnit->canGift(false, false))
				{
					return false;
				}
			}
		}
	}

	if (bTestTransport)
	{
		if (pTransport != NULL && pTransport->getTeam() != pPlot->getTeam())
		{
			return false;
		}
	}

	if (!bTestVisible)
	{
		if (!(GET_PLAYER(pPlot->getOwnerINLINE()).AI_acceptUnit(this)))
		{
			return false;
		}
	}

	if (atWar(pPlot->getTeam(), getTeam()))
	{
		return false;
	}

	// to shut down free units from king exploit
	if (kOwner.getNumCities() == 0)
	{
		return false;
	}

	// to shut down free ship from king exploit
	if (kOwner.getParent() != NO_PLAYER)
	{
		CvPlayer& kEurope = GET_PLAYER(kOwner.getParent());
		if (kEurope.isAlive() && kEurope.isEurope() && !::atWar(getTeam(), kEurope.getTeam()) && getDomainType() == DOMAIN_SEA)
		{
			bool bHasOtherShip = false;
			int iLoop;
			for (CvUnit* pLoopUnit = kOwner.firstUnit(&iLoop); pLoopUnit != NULL && !bHasOtherShip; pLoopUnit = kOwner.nextUnit(&iLoop))
			{
				if (pLoopUnit != this && pLoopUnit->getDomainType() == DOMAIN_SEA)
				{
					bHasOtherShip = true;
				}
			}

			if (!bHasOtherShip )
			{
				return false;
			}
		}
	}

	return true;
}


void CvUnit::gift(bool bTestTransport)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pGiftUnit;
	CvUnit* pLoopUnit;
	CvPlot* pPlot;
	CvWString szBuffer;
	PlayerTypes eOwner;

	if (!canGift(false, bTestTransport))
	{
		return;
	}

	pPlot = plot();

	pUnitNode = pPlot->headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if (pLoopUnit->getTransportUnit() == this)
		{
			pLoopUnit->gift(false);
		}
	}

	FAssertMsg(plot()->getOwnerINLINE() != NO_PLAYER, "plot()->getOwnerINLINE() is not expected to be equal with NO_PLAYER");
	pGiftUnit = GET_PLAYER(plot()->getOwnerINLINE()).initUnit(getUnitType(), getProfession(), getX_INLINE(), getY_INLINE(), AI_getUnitAIType(), getFacingDirection(false), getYieldStored());

	FAssertMsg(pGiftUnit != NULL, "GiftUnit is not assigned a valid value");

	eOwner = getOwnerINLINE();

	pGiftUnit->convert(this, true);

	int iUnitValue = 0;
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		iUnitValue += pGiftUnit->getUnitInfo().getYieldCost(iYield);
	}
	GET_PLAYER(pGiftUnit->getOwnerINLINE()).AI_changePeacetimeGrantValue(eOwner, iUnitValue / 5);

	szBuffer = gDLL->getText("TXT_KEY_MISC_GIFTED_UNIT_TO_YOU", GET_PLAYER(eOwner).getNameKey(), pGiftUnit->getNameKey());
	gDLL->getInterfaceIFace()->addMessage(pGiftUnit->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNITGIFTED", MESSAGE_TYPE_INFO, pGiftUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pGiftUnit->getX_INLINE(), pGiftUnit->getY_INLINE(), true, true);

	// Python Event
	gDLL->getEventReporterIFace()->unitGifted(pGiftUnit, getOwnerINLINE(), plot());
}


bool CvUnit::canLoadUnit(const CvUnit* pTransport, const CvPlot* pPlot, bool bCheckCity) const
{
	FAssert(pTransport != NULL);
	FAssert(pPlot != NULL);

	if (getUnitTravelState() != pTransport->getUnitTravelState())
	{
		return false;
	}

	if (pTransport == this)
	{
		return false;
	}

	if (getTransportUnit() == pTransport)
	{
		return false;
	}

	if (pTransport->getTeam() != getTeam())
	{
		return false;
	}

	if (getCargo() > 0)
	{
		return false;
	}

	if (pTransport->isCargo())
	{
		return false;
	}

	// PatchMod: Berth size START
	if (pTransport->cargoSpaceAvailable(getSpecialUnitType(), getDomainType()) < getUnitInfo().getBerthSize())
	{
		return false;
	}
//	if (!(pTransport->cargoSpaceAvailable(getSpecialUnitType(), getDomainType())))
//	{
//		return false;
//	}
	// PatchMod: Berth size END

	if (pTransport->cargoSpace() < getUnitInfo().getRequiredTransportSize())
	{
		return false;
	}

	if (!(pTransport->atPlot(pPlot)))
	{
		return false;
	}

	if (bCheckCity && !pPlot->isCity(true))
	{
		return false;
	}

	return true;
}


void CvUnit::loadUnit(CvUnit* pTransport)
{
	if (!canLoadUnit(pTransport, plot(), true))
	{
		return;
	}

	setTransportUnit(pTransport);
}

bool CvUnit::shouldLoadOnMove(const CvPlot* pPlot) const
{
	if (isCargo())
	{
		return false;
	}

	if (getYield() != NO_YIELD)
	{
		CvCity* pCity = pPlot->getPlotCity();
		if (pCity != NULL && GET_PLAYER(getOwnerINLINE()).canUnloadYield(pCity->getOwnerINLINE()))
		{
			return false;
		}
	}

	if (getUnitTravelState() != NO_UNIT_TRAVEL_STATE)
	{
		return false;
	}

	if (!pPlot->isValidDomainForLocation(*this))
	{
		return true;
	}

	if (m_pUnitInfo->getTerrainImpassable(pPlot->getTerrainType()))
	{
		return true;
	}

	return false;
}

int CvUnit::getLoadedYieldAmount(YieldTypes eYield) const
{
	CvPlot* pPlot = plot();
	if (pPlot == NULL)
	{
		return 0;
	}

	int iTotal = 0;
	//check if room in other cargo
	for (int i=0;i<pPlot->getNumUnits();i++)
	{
		CvUnit* pLoopUnit = pPlot->getUnitByIndex(i);
		if(pLoopUnit != NULL)
		{
			if(pLoopUnit->getTransportUnit() == this)
			{
				if(pLoopUnit->getYield() == eYield)
				{
					iTotal += pLoopUnit->getYieldStored();
				}
			}
		}
	}

	return iTotal;
}

int CvUnit::getLoadYieldAmount(YieldTypes eYield) const
{
	CvPlot* pPlot = plot();
	if (pPlot == NULL)
	{
		return 0;
	}

	bool bFull = isFull();
	if (!bFull)
	{
		UnitClassTypes eUnitClass = (UnitClassTypes) GC.getYieldInfo(eYield).getUnitClass();
		FAssert(eUnitClass != NO_UNITCLASS);
		if (eUnitClass != NO_UNITCLASS)
		{
			UnitTypes eUnit = (UnitTypes) GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(eUnitClass);
			if (eUnit != NO_UNIT)
			{
				CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);
				if (!cargoSpaceAvailable((SpecialUnitTypes) kUnit.getSpecialUnitType(), (DomainTypes) kUnit.getDomainType()))
				{
					bFull = true;
				}

				if (cargoSpace() < kUnit.getRequiredTransportSize())
				{
					bFull = true;
				}
			}
		}
	}

	if (!bFull)
	{
		return GC.getGameINLINE().getCargoYieldCapacity();
	}

	//check if room in other cargo
	for (int i=0;i<pPlot->getNumUnits();i++)
	{
		CvUnit* pLoopUnit = pPlot->getUnitByIndex(i);
		if(pLoopUnit != NULL)
		{
			if(pLoopUnit->getTransportUnit() == this)
			{
				if(pLoopUnit->getYield() == eYield)
				{
					int iSpaceAvailable = GC.getGameINLINE().getCargoYieldCapacity() - pLoopUnit->getYieldStored();
					//check if space available
					if(iSpaceAvailable > 0)
					{
						return iSpaceAvailable;
					}
				}
			}
		}
	}

	return 0;
}

bool CvUnit::canLoadYields(const CvPlot* pPlot, bool bTrade) const
{
	for (int iYield = 0; iYield <  NUM_YIELD_TYPES; ++iYield)
	{
		if(canLoadYield(pPlot, (YieldTypes) iYield, bTrade))
		{
			return true;
		}
	}

	return false;
}

bool CvUnit::canLoadYield(const CvPlot* pPlot, YieldTypes eYield, bool bTrade) const
{
	//TAC Whaling, ray
	//if (getUnitInfo().isGatherBoat())
	//{
	//	return false;
	//}
	//End TAC Whaling, ray
	if (eYield == NO_YIELD)
	{
		FAssert(!bTrade);
		return canLoadYields(pPlot, bTrade);
	}

	CvYieldInfo& kYield = GC.getYieldInfo(eYield);

	if (kYield.isCargo() && !isCargo())
	{
		if (pPlot != NULL)
		{
			CvCity* pCity = pPlot->getPlotCity();
			if (NULL != pCity)
			{
				if(GET_PLAYER(getOwnerINLINE()).canLoadYield(pCity->getOwnerINLINE()) || bTrade)
				{
					if (kYield.isCargo())
					{
						if (pCity->getYieldStored(eYield) > 0)
						{
							if (getLoadYieldAmount(eYield) > 0)
							{
								return true;
							}
						}
					}
				}
			}
		}
	}

	return false;
}

//R&R mod, vetiarvind, max yield import limit - start
//void CvUnit::loadYield(YieldTypes eYield, bool bTrade)
int CvUnit::loadYield(YieldTypes eYield, bool bTrade)
{	
	/*if (!canLoadYield(plot(), eYield, bTrade))
	{
		return 0;
	}*/

	
	return loadYieldAmount(eYield, getMaxLoadYieldAmount(eYield), bTrade);
	//loadYieldAmount(eYield, getMaxLoadYieldAmount(eYield), bTrade);
//R&R mod, vetiarvind, max yield import limit - end
}

////R&R mod, vetiarvind, max yield import limit - start
//void CvUnit::loadYieldAmount(YieldTypes eYield, int iAmount, bool bTrade)
int CvUnit::loadYieldAmount(YieldTypes eYield, int iAmount, bool bTrade)
////R&R mod, vetiarvind, max yield import limit - end
{
	if (!canLoadYield(plot(), eYield, bTrade))
	{
		return 0;
	}

	if (iAmount <= 0 || iAmount > getMaxLoadYieldAmount(eYield))
	{
		return 0;
	}

	CvUnit* pUnit = plot()->getPlotCity()->createYieldUnit(eYield, getOwnerINLINE(), iAmount);
	FAssert(pUnit != NULL);
	if(pUnit != NULL)
	{
		pUnit->setTransportUnit(this);
	}
	return iAmount; //R&R mod, vetiarvind, max yield import limit
}

int CvUnit::getMaxLoadYieldAmount(YieldTypes eYield) const
{
	int iMaxAmount = GC.getGameINLINE().getCargoYieldCapacity();
	iMaxAmount = std::min(iMaxAmount, getLoadYieldAmount(eYield));
	CvCity* pCity = plot()->getPlotCity();
	if (pCity != NULL)
	{
		int iMaxAvailable = pCity->getYieldStored(eYield);
		if (!isHuman() || isAutomated())
		{
//VET NewCapacity - begin 1/1
			//iMaxAvailable -= pCity->getMaintainLevel(eYield);

			// R&R, ray, improvement
			//if (GC.getNEW_CAPACITY())
			if (GC.getNEW_CAPACITY() && !isHuman() && (pCity->getTotalYieldStored() > pCity->getMaxYieldCapacity() / 2))
			//if (GC.getNEW_CAPACITY() && !isHuman())
			{
				int iCargoYields = 0;
				for (int iYield = 3; iYield < NUM_YIELD_TYPES; iYield++)// without YIELD_FOOD, YIELD_LUMBER, YIELD_STONE
				{
					if ((pCity->getYieldStored((YieldTypes)iYield) > 0) && (GC.getYieldInfo(eYield).isCargo()))
						{iCargoYields++;}
				}
				
				if(iCargoYields > 0)//R&R mod, vetiarvind, max yield import limit
					iMaxAvailable -= (pCity->getMaintainLevel(eYield) / iCargoYields);
			}
			else
				{
					//iMaxAvailable -= pCity->getMaintainLevel(eYield);
					// transport feeder - start - Nightinggale
					iMaxAvailable -= pCity->getAutoMaintainThreshold(eYield);
					// transport feeder - end - Nightinggale
				}
//VET NewCapacity - end 1/1
		}
		iMaxAmount = std::min(iMaxAmount, iMaxAvailable);
	}

	return std::max(iMaxAmount, 0);
}

bool CvUnit::canTradeYield(const CvPlot* pPlot) const
{
	FAssert(pPlot != NULL);

	CvCity* pCity = pPlot->getPlotCity();
	if (pCity == NULL)
	{
		return false;
	}

	if (pCity->getOwnerINLINE() == getOwnerINLINE())
	{
		return false;
	}

	if (cargoSpace() == 0)
	{
		return false;
	}

	if (!canMove())
	{
		return false;
	}

	//check if we have any yield cargo
	bool bYieldFound = false;
	if (hasCargo())
	{
		for (int i=0;i<pPlot->getNumUnits();i++)
		{
			CvUnit* pLoopUnit = pPlot->getUnitByIndex(i);
			if (pLoopUnit != NULL)
			{
				if (pLoopUnit->getTransportUnit() == this)
				{
					if (pLoopUnit->getYield() != NO_YIELD)
					{
						bYieldFound = true;
						break;
					}
				}
			}
		}
	}

	//check if the city has any cargo that we can fit
	if(!bYieldFound)
	{
		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			YieldTypes eYield = (YieldTypes) iYield;
			if ((pCity->getYieldStored(eYield) > 0) && (getLoadYieldAmount(eYield) > 0))
			{
				bYieldFound = true;
				break;
			}
		}
	}

	if (!bYieldFound)
	{
		return false;
	}

	return true;
}

void CvUnit::tradeYield()
{
	if(!canTradeYield(plot()))
	{
		return;
	}

	PlayerTypes eOtherPlayer = plot()->getOwnerINLINE();

	//both human
	if (GET_PLAYER(getOwnerINLINE()).isHuman() && GET_PLAYER(eOtherPlayer).isHuman())
	{
		if (GC.getGameINLINE().isPbem() || GC.getGameINLINE().isHotSeat() || (GC.getGameINLINE().isPitboss() && !gDLL->isConnected(GET_PLAYER(eOtherPlayer).getNetID())))
		{
			if (gDLL->isMPDiplomacy())
			{
				gDLL->beginMPDiplomacy(eOtherPlayer, false, false, getIDInfo());
			}
		}
		else if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
		{
			//clicking the flashing goes through CvPlayer::contact where it sends the response message
			gDLL->sendContactCiv(NETCONTACT_INITIAL, eOtherPlayer, getID());
		}
	}
	else if(GET_PLAYER(getOwnerINLINE()).isHuman()) //we're human contacting them
	{
		CvDiploParameters* pDiplo = new CvDiploParameters(eOtherPlayer);
		pDiplo->setDiploComment((DiploCommentTypes) GC.getInfoTypeForString("AI_DIPLOCOMMENT_TRADING"));
		pDiplo->setTransport(getIDInfo());
		pDiplo->setCity(plot()->getPlotCity()->getIDInfo());
		// R&R, ray, Bargaining - Start
		if (GET_PLAYER(getOwnerINLINE()).isHuman() && GET_PLAYER(eOtherPlayer).getTimeNoTrade() > 0)
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_TEXT);
			pInfo->setText(gDLL->getText("NO_TRADE_BECAUSE_BAD_BARGAIN", GET_PLAYER(eOtherPlayer).getCivilizationDescription()));
			gDLL->getInterfaceIFace()->addPopup(pInfo, GC.getGameINLINE().getActivePlayer(), true, true);
		}
		else
		{
			//alte Logik
			gDLL->beginDiplomacy(pDiplo, getOwnerINLINE());
		}
		// R&R, ray, Bargaining - End
	}
	else if(GET_PLAYER(eOtherPlayer).isHuman()) //they're human contacting us
	{
		CvDiploParameters* pDiplo = new CvDiploParameters(getOwnerINLINE());
		pDiplo->setDiploComment((DiploCommentTypes) GC.getInfoTypeForString("AI_DIPLOCOMMENT_TRADING"));
		pDiplo->setTransport(getIDInfo());
		gDLL->beginDiplomacy(pDiplo, getOwnerINLINE());
	}
	else //both AI
	{
		FAssertMsg(false, "Don't go through here. Implement deals directly.");
	}
}

bool CvUnit::canClearSpecialty() const
{
	if (m_pUnitInfo->getTeacherWeight() <= 0)
	{
		return false;
	}

	UnitTypes eUnit = (UnitTypes) GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("DEFAULT_POPULATION_UNIT"));
	if (eUnit == NO_UNIT)
	{
		return false;
	}

	return true;
}

void CvUnit::clearSpecialty()
{
	if (!canClearSpecialty())
	{
		return;
	}

	bool bLocked = isColonistLocked();
	UnitTypes eUnit = (UnitTypes) GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("DEFAULT_POPULATION_UNIT"));
	CvUnit* pNewUnit = GET_PLAYER(getOwnerINLINE()).initUnit(eUnit, NO_PROFESSION, getX_INLINE(), getY_INLINE(), AI_getUnitAIType());
	FAssert(pNewUnit != NULL);

	CvCity *pCity = GET_PLAYER(getOwnerINLINE()).getPopulationUnitCity(getID());
	if (pCity != NULL)
	{
		pNewUnit->convert(this, false);
		pCity->replaceCitizen(pNewUnit->getID(), getID(), false);
		pNewUnit->setColonistLocked(bLocked);
		// TAC - Clear specialty fix - koma13 - START
		//pCity->removePopulationUnit(this, true, NO_PROFESSION);
		if (GET_PLAYER(getOwnerINLINE()).getPopulationUnitCity(getID()) != NULL)
		{
			pCity->removePopulationUnit(this, true, NO_PROFESSION);
		}
		else
		{
			kill(false);
		}
		// TAC - Clear specialty fix - koma13 - END
	}
	else
	{
		pNewUnit->convert(this, true);
	}
}

bool CvUnit::canAutoCrossOcean(const CvPlot* pPlot) const
{
	if (canCrossOcean(pPlot, UNIT_TRAVEL_STATE_TO_EUROPE))
	{
		return false;
	}

	if (!GET_PLAYER(getOwnerINLINE()).canTradeWithEurope())
	{
		return false;
	}

	if (getDomainType() != DOMAIN_SEA)
	{
		return false;
	}

	if (!pPlot->isEuropeAccessable())
	{
		return false;
	}

	return true;
}

bool CvUnit::canCrossOcean(const CvPlot* pPlot, UnitTravelStates eNewState) const
{
	if (getTransportUnit() != NULL)
	{
		return false;
	}

	if (getUnitTravelState() == NO_UNIT_TRAVEL_STATE && !canMove())
	{
		return false;
	}

	switch (getUnitTravelState())
	{
	case NO_UNIT_TRAVEL_STATE:
		if (eNewState != UNIT_TRAVEL_STATE_TO_EUROPE && eNewState != UNIT_TRAVEL_STATE_TO_AFRICA && eNewState != UNIT_TRAVEL_STATE_TO_PORT_ROYAL) // R&R, ray, adjustments for Africa and Port Royal
		{
			return false;
		}
		if (eNewState != UNIT_TRAVEL_STATE_TO_PORT_ROYAL && !GET_PLAYER(getOwnerINLINE()).canTradeWithEurope()) // R&R, ray, adjustments for Port Royal
		//if (!GET_PLAYER(getOwnerINLINE()).canTradeWithEurope())
		{
			return false;
		}
		break;
	case UNIT_TRAVEL_STATE_IN_EUROPE:
		if (eNewState != UNIT_TRAVEL_STATE_FROM_EUROPE)
		{
			return false;
		}
		break;
	case UNIT_TRAVEL_STATE_IN_AFRICA: // R&R, ray, adjustments for Africa
		if (eNewState != UNIT_TRAVEL_STATE_FROM_AFRICA)
		{
			return false;
		}
		break;
	case UNIT_TRAVEL_STATE_IN_PORT_ROYAL: // R&R, ray, Port Royal
		if (eNewState != UNIT_TRAVEL_STATE_FROM_PORT_ROYAL)
		{
			return false;
		}
		break;
	default:
		FAssertMsg(false, "Invalid trip");
		return false;
		break;
	}

	FAssert(pPlot != NULL);
	if (!pPlot->isEurope())
	{
		return false;
	}

	return true;
}

void CvUnit::crossOcean(UnitTravelStates eNewState)
{
	if (!canCrossOcean(plot(), eNewState))
	{
		return;
	}

	int iTravelTime = GC.getEuropeInfo(plot()->getEurope()).getTripLength();

	iTravelTime *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
	iTravelTime /= 100;

	for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); ++iTrait)
	{
		TraitTypes eTrait = (TraitTypes) iTrait;
		if (GET_PLAYER(getOwnerINLINE()).hasTrait(eTrait))
		{
			iTravelTime *= 100 + GC.getTraitInfo(eTrait).getEuropeTravelTimeModifier();
			iTravelTime /= 100;
		}
	}

	setUnitTravelState(eNewState, false);
	if (iTravelTime > 0)
	{
		setUnitTravelTimer(iTravelTime);
	}
	else
	{
		setUnitTravelTimer(1);
		doUnitTravelTimer();
		finishMoves();
	}
}

/*** TRIANGLETRADE 10/28/08 by DPII ***/
bool CvUnit::canSailToAfrica(const CvPlot* pPlot, UnitTravelStates eNewState) const
{
	if (getTransportUnit() != NULL)
	{
		return false;
	}

	if (getUnitTravelState() == NO_UNIT_TRAVEL_STATE && !canMove())
	{
		return false;
	}

	switch (getUnitTravelState())
	{
	case NO_UNIT_TRAVEL_STATE:
		if (!GET_PLAYER(getOwnerINLINE()).canTradeWithAfrica())
		{
			return false;
		}
		break;
	case UNIT_TRAVEL_STATE_IN_AFRICA:
	case UNIT_TRAVEL_STATE_IN_PORT_ROYAL:
	case UNIT_TRAVEL_STATE_IN_EUROPE:
		return true;
		break;
	case UNIT_TRAVEL_STATE_FROM_EUROPE:
	case UNIT_TRAVEL_STATE_FROM_AFRICA:
	case UNIT_TRAVEL_STATE_FROM_PORT_ROYAL:
	case UNIT_TRAVEL_STATE_TO_EUROPE:
	case UNIT_TRAVEL_STATE_TO_AFRICA:
	case UNIT_TRAVEL_STATE_TO_PORT_ROYAL:
		return false;
		break;
	default:
		FAssertMsg(false, "Invalid trip");
		return false;
		break;
	}

	FAssert(pPlot != NULL);
	if (!pPlot->isEurope())
	{
		return false;
	}

	return true;
}

void CvUnit::sailToAfrica(UnitTravelStates eNewState)
{
	if (!canSailToAfrica(plot()))
	{
		return;
	}

	int iTravelTime = GC.getEuropeInfo(plot()->getEurope()).getTripLength();

	iTravelTime *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
	iTravelTime /= 100;

	for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); ++iTrait)
	{
		TraitTypes eTrait = (TraitTypes) iTrait;
		if (GET_PLAYER(getOwnerINLINE()).hasTrait(eTrait))
		{
			iTravelTime *= 100 + GC.getTraitInfo(eTrait).getEuropeTravelTimeModifier();
			iTravelTime /= 100;
		}
	}
	if (eNewState == NO_UNIT_TRAVEL_STATE)
	{
		switch (getUnitTravelState())
		{
		case NO_UNIT_TRAVEL_STATE:
		case UNIT_TRAVEL_STATE_IN_EUROPE:
		case UNIT_TRAVEL_STATE_IN_PORT_ROYAL:
			eNewState = UNIT_TRAVEL_STATE_TO_AFRICA;
			break;
		case UNIT_TRAVEL_STATE_IN_AFRICA:
			eNewState = UNIT_TRAVEL_STATE_FROM_AFRICA;
			break;
		}
	}

	setUnitTravelState(eNewState, false);
	if (iTravelTime > 0)
	{
		setUnitTravelTimer(iTravelTime);
	}
	else
	{
		setUnitTravelTimer(1);
		doUnitTravelTimer();
		finishMoves();
	}
}

// R&R, ray, Port Royal
bool CvUnit::canSailToPortRoyal(const CvPlot* pPlot, UnitTravelStates eNewState) const
{
	// only Ships with hidden nationality can sail to Port Royal
	if (!getUnitInfo().isHiddenNationality())
	{
		return false;
	}
	
	if (getTransportUnit() != NULL)
	{
		return false;
	}

	if (getUnitTravelState() == NO_UNIT_TRAVEL_STATE && !canMove())
	{
		return false;
	}

	switch (getUnitTravelState())
	{
	case NO_UNIT_TRAVEL_STATE:
		if (!GET_PLAYER(getOwnerINLINE()).canTradeWithPortRoyal())
		{
			return false;
		}
		break;
	case UNIT_TRAVEL_STATE_IN_AFRICA:
	case UNIT_TRAVEL_STATE_IN_PORT_ROYAL:
	case UNIT_TRAVEL_STATE_IN_EUROPE:
		return true;
		break;
	case UNIT_TRAVEL_STATE_FROM_EUROPE:
	case UNIT_TRAVEL_STATE_FROM_AFRICA:
	case UNIT_TRAVEL_STATE_FROM_PORT_ROYAL:
	case UNIT_TRAVEL_STATE_TO_EUROPE:
	case UNIT_TRAVEL_STATE_TO_AFRICA:
	case UNIT_TRAVEL_STATE_TO_PORT_ROYAL:
		return false;
		break;
	default:
		FAssertMsg(false, "Invalid trip");
		return false;
		break;
	}
	/*R&R, vetiarvind, goto for hidden nationality fix start
	FAssert(pPlot != NULL); 
	if (!pPlot->isEurope())
	{
		return false;
	}
	*/	
	if(pPlot!=NULL && !pPlot->isEurope())
		return false;
	//R&R, vetiarvind, goto for hidden nationality fix - end
	// R&R, ray, fix for grouped ships being able to sail to Port Royal - START
	CLLNode<IDInfo>* pUnitNode = getGroup()->headUnitNode();
	CvUnit* pLoopUnit;

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = getGroup()->nextUnitNode(pUnitNode);

		if (pLoopUnit->getTransportUnit() == NULL)
		{
			if (!pLoopUnit->getUnitInfo().isHiddenNationality())
			{
				return false;
			}
		}
	}
	// R&R, ray, fix for grouped ships being able to sail to Port Royal - END

	return true;
}

void CvUnit::sailToPortRoyal(UnitTravelStates eNewState)
{
	if (!canSailToPortRoyal(plot()))
	{
		return;
	}

	int iTravelTime = GC.getEuropeInfo(plot()->getEurope()).getTripLength();

	iTravelTime *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
	iTravelTime /= 100;

	for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); ++iTrait)
	{
		TraitTypes eTrait = (TraitTypes) iTrait;
		if (GET_PLAYER(getOwnerINLINE()).hasTrait(eTrait))
		{
			iTravelTime *= 100 + GC.getTraitInfo(eTrait).getEuropeTravelTimeModifier();
			iTravelTime /= 100;
		}
	}
	if (eNewState == NO_UNIT_TRAVEL_STATE)
	{
		switch (getUnitTravelState())
		{
		case NO_UNIT_TRAVEL_STATE:
		case UNIT_TRAVEL_STATE_IN_EUROPE:
		case UNIT_TRAVEL_STATE_IN_AFRICA:
			eNewState = UNIT_TRAVEL_STATE_TO_PORT_ROYAL;
			break;
		case UNIT_TRAVEL_STATE_IN_PORT_ROYAL:
			eNewState = UNIT_TRAVEL_STATE_FROM_PORT_ROYAL;
			break;
		}
	}

	setUnitTravelState(eNewState, false);
	if (iTravelTime > 0)
	{
		setUnitTravelTimer(iTravelTime);
	}
	else
	{
		setUnitTravelTimer(1);
		doUnitTravelTimer();
		finishMoves();
	}
}
// R&R, ray, Port Royal - END

bool CvUnit::canLoad(const CvPlot* pPlot, bool bCheckCity) const
{
	PROFILE_FUNC();

	FAssert(pPlot != NULL);

	CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if (canLoadUnit(pLoopUnit, pPlot, bCheckCity))
		{
			return true;
		}
	}

	return false;
}


bool CvUnit::load(bool bCheckCity)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pPlot;
	int iPass;

	if (!canLoad(plot(), bCheckCity))
	{
		return true;
	}

	pPlot = plot();

	for (iPass = 0; iPass < 2; iPass++)
	{
		pUnitNode = pPlot->headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = pPlot->nextUnitNode(pUnitNode);

			if (canLoadUnit(pLoopUnit, pPlot, bCheckCity))
			{
				if ((iPass == 0) ? (pLoopUnit->getOwnerINLINE() == getOwnerINLINE()) : (pLoopUnit->getTeam() == getTeam()))
				{
					if (!setTransportUnit(pLoopUnit))
					{
						return false;
					}
					break;
				}
			}
		}

		if (isCargo())
		{
			break;
		}
	}

	return true;
}


bool CvUnit::canUnload() const
{
	if (getTransportUnit() == NULL)
	{
		return false;
	}

	if (!plot()->isValidDomainForLocation(*this))
	{
		return false;
	}

	YieldTypes eYield = getYield();
	if (eYield != NO_YIELD)
	{
		CvCity* pCity = plot()->getPlotCity();
		FAssert(pCity != NULL);
		if (pCity == NULL || !GET_PLAYER(getOwnerINLINE()).canUnloadYield(pCity->getOwnerINLINE()))
		{
			return false;
		}
	}

	return true;
}


void CvUnit::unload()
{
	if (!canUnload())
	{
		return;
	}

	setTransportUnit(NULL);
}

// returns true if the unit is still alive
void CvUnit::unloadStoredAmount(int iAmount)
{
	if (!canUnload())
	{
		return;
	}

	FAssert(iAmount <= getYieldStored());
	if (iAmount > getYieldStored())
	{
		return;
	}

	FAssert(isGoods());

	doUnloadYield(iAmount);
}

void CvUnit::doUnloadYield(int iAmount)
{
	YieldTypes eYield = getYield();
	FAssert(eYield != NO_YIELD);
	if (eYield == NO_YIELD)
	{
		return;
	}

	if (getYieldStored() == 0)
	{
		return;
	}

	CvUnit* pUnloadingUnit = this;
	if (iAmount < getYieldStored())
	{
		UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getYieldInfo(eYield).getUnitClass());
		if (NO_UNIT != eUnit)
		{			
			pUnloadingUnit = GET_PLAYER(getOwnerINLINE())
				.initUnit(eUnit, getProfession(), getX_INLINE(), getY_INLINE(), NO_UNITAI, NO_DIRECTION, iAmount);
			FAssert(pUnloadingUnit != NULL);
			
			setYieldStored(getYieldStored() - iAmount);
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		}
	}


	CvCity* pCity = plot()->getPlotCity();
	if (pCity != NULL)
	{
		// R&R mod, vetiarvind, max yield import limit - start (bug fix of native code)
		//pCity->changeYieldStored(eYield, pUnloadingUnit->getYieldStored());
		pCity->changeYieldStored(eYield, iAmount);
		// R&R mod, vetiarvind, max yield import limit - end
		pCity->AI_changeTradeBalance(eYield, iAmount);
		if (pCity->AI_getDesiredYield() == eYield)
		{
			if (iAmount > GC.getGameINLINE().getSorenRandNum(pCity->getMaxYieldCapacity(), "change desired yield"))
			{
				pCity->AI_assignDesiredYield();
			}
		}
		pUnloadingUnit->setYieldStored(0);
	}

}

bool CvUnit::canUnloadAll() const
{
	if (getCargo() == 0)
	{
		return false;
	}

	CvPlot* pPlot = plot();
	if(pPlot == NULL)
	{
		return false;
	}

	CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if (pLoopUnit->getTransportUnit() == this)
		{
			if(pLoopUnit->canUnload())
			{
				return true;
			}
		}
	}

	return false;
}


void CvUnit::unloadAll()
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pPlot;

	if (!canUnloadAll())
	{
		return;
	}

	pPlot = plot();

	pUnitNode = pPlot->headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if (pLoopUnit->getTransportUnit() == this)
		{
			if (pLoopUnit->canUnload())
			{
				pLoopUnit->setTransportUnit(NULL);
			}
			else
			{
				FAssert(isHuman());
				pLoopUnit->getGroup()->setActivityType(ACTIVITY_AWAKE);
			}
		}
	}
}

bool CvUnit::canLearn() const
{
	UnitTypes eUnitType = getLearnUnitType(plot());
	if(eUnitType == NO_UNIT)
	{
		return false;
	}

	if (isCargo() && !canUnload())
	{
		return false;
	}

	if (!canMove())
	{
		return false;
	}

	// R&R, ray, Natives do not talk when furious - START
	// Erik: natives will not teach if a they are still angry due to bargaining
	if (isHuman() && isAutomated())
	{
		const CvCity *const pCity = plot()->getPlotCity();
		const PlayerTypes eNativePlayer = pCity->getOwnerINLINE();
		CvPlayerAI& nativePlayer = GET_PLAYER(eNativePlayer);

		if (!nativePlayer.isHuman())
		{
			int currentPlayerAttitude = nativePlayer.AI_getAttitude(getOwnerINLINE(), false);
			if (currentPlayerAttitude == ATTITUDE_FURIOUS || nativePlayer.getTimeNoTrade() > 0)
			{
				return false;
			}
		}
	}
	// R&R, ray, Natives do not talk when furious - END

	return true;
}

void CvUnit::learn()
{
	if(!canLearn())
	{
		return;
	}

	CvCity* pCity = plot()->getPlotCity();
	PlayerTypes eNativePlayer = pCity->getOwnerINLINE();

	// R&R, ray, Natives do not talk when furious - START
	int currentPlayerAttitude = GET_PLAYER(eNativePlayer).AI_getAttitude(getOwnerINLINE(), false);
	if (isHuman() && currentPlayerAttitude == 0)
	{
		CvWString szBuffer = gDLL->getText("TXT_KEY_NATIVES_NOT_WILLING_TO_TALK", plot()->getPlotCity()->getNameKey());
		gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_LEARN).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);
		return;
	}
	// R&R, ray, Natives do not talk when furious - ELSE

	// WTP, ray, Game Option only 1 Colonist living in Village - START
	else if (isHuman() && GC.getGameINLINE().isOption(GAMEOPTION_ONLY_ONE_COLONIST_PER_VILLAGE))
	{
		std::vector<CvUnit*> aUnits;
		CLLNode<IDInfo>* pUnitNode = plot()->headUnitNode();
		while (pUnitNode)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = plot()->nextUnitNode(pUnitNode);
			// this checks if there is already another Native Living in this village
			// if yes, we send a message and exit the method by return
			if (pLoopUnit->getUnitTravelState() == UNIT_TRAVEL_STATE_LIVE_AMONG_NATIVES)
			{
				CvWString szBuffer = gDLL->getText("TXT_KEY_NATIVES_NOT_WILLING_ALREADY_COLONIST_LEARNING", plot()->getPlotCity()->getNameKey());
				gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_LEARN).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);
				return;
			}
		}
	}
	// WTP, ray, Game Option only 1 Colonist living in Village - END

	if (isHuman() && !getGroup()->AI_isControlled() && !GET_PLAYER(eNativePlayer).isHuman())
	{
		UnitTypes eUnitType = getLearnUnitType(plot());
		FAssert(eUnitType != NO_UNIT);

		CvDiploParameters* pDiplo = new CvDiploParameters(eNativePlayer);
		pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_LIVE_AMONG_NATIVES"));
		pDiplo->addDiploCommentVariable(pCity->getNameKey());
		pDiplo->addDiploCommentVariable(GC.getUnitInfo(eUnitType).getTextKeyWide());
		pDiplo->setData(getID());
		pDiplo->setAIContact(true);
		pDiplo->setCity(pCity->getIDInfo());
		gDLL->beginDiplomacy(pDiplo, getOwnerINLINE());
	}
	else
	{
		doLiveAmongNatives();
	}
}

void CvUnit::doLiveAmongNatives()
{
	if(!canLearn())
	{
		return;
	}

	unload();

	CvCity* pCity = plot()->getPlotCity();

	pCity->setTeachUnitMultiplier(pCity->getTeachUnitMultiplier() * (100 + GC.getDefineINT("NATIVE_TEACH_THRESHOLD_INCREASE")) / 100);
	int iLearnTime = getLearnTime();
	if (iLearnTime > 0)
	{
		setUnitTravelState(UNIT_TRAVEL_STATE_LIVE_AMONG_NATIVES, false);
		setUnitTravelTimer(iLearnTime);
	}
	else
	{
		doLearn();
	}
}

void CvUnit::doLearn()
{
	if(!canLearn())
	{
		return;
	}

	UnitTypes eUnitType = getLearnUnitType(plot());
	FAssert(eUnitType != NO_UNIT);

	CvUnit* pLearnUnit = GET_PLAYER(getOwnerINLINE()).initUnit(eUnitType, getProfession(), getX_INLINE(), getY_INLINE(), AI_getUnitAIType());
	FAssert(pLearnUnit != NULL);
	pLearnUnit->joinGroup(getGroup());
	pLearnUnit->convert(this, true);

	gDLL->getEventReporterIFace()->unitLearned(pLearnUnit->getOwnerINLINE(), pLearnUnit->getID());
}

UnitTypes CvUnit::getLearnUnitType(const CvPlot* pPlot) const
{
	if (getUnitInfo().getLearnTime() < 0)
	{
		return NO_UNIT;
	}

	if (pPlot == NULL)
	{
		return NO_UNIT;
	}

	CvCity* pCity = pPlot->getPlotCity();
	if (pCity == NULL)
	{
		return NO_UNIT;
	}

	if (pCity->getOwnerINLINE() == getOwnerINLINE())
	{
		return NO_UNIT;
	}

	if (!pCity->isScoutVisited(getTeam()))
	{
		return NO_UNIT;
	}

	UnitClassTypes eTeachUnitClass = pCity->getTeachUnitClass();
	if (eTeachUnitClass == NO_UNITCLASS)
	{
		return NO_UNIT;
	}

	UnitTypes eTeachUnit = (UnitTypes) GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(eTeachUnitClass);
	if (eTeachUnit == getUnitType())
	{
		return NO_UNIT;
	}

	return eTeachUnit;
}

int CvUnit::getLearnTime() const
{
	CvCity* pCity = plot()->getPlotCity();
	if (pCity == NULL)
	{
		return MAX_INT;
	}

	int iLearnTime = m_pUnitInfo->getLearnTime() * pCity->getTeachUnitMultiplier() / 100;

	iLearnTime *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
	iLearnTime /= 100;

	for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); ++iTrait)
	{
		TraitTypes eTrait = (TraitTypes) iTrait;
		if (GET_PLAYER(getOwnerINLINE()).hasTrait(eTrait) || GET_PLAYER(pCity->getOwnerINLINE()).hasTrait(eTrait))
		{
			iLearnTime *= 100 + GC.getTraitInfo(eTrait).getLearnTimeModifier();
			iLearnTime /= 100;
		}
	}

	return iLearnTime;
}


bool CvUnit::canKingTransport() const
{
	PlayerTypes eParent = GET_PLAYER(getOwnerINLINE()).getParent();
	if (eParent == NO_PLAYER || !GET_PLAYER(eParent).isAlive() || ::atWar(getTeam(), GET_PLAYER(eParent).getTeam()))
	{
		return false;
	}

	if (!canMove())
	{
		return false;
	}

	CvPlot* pPlot = plot();
	if (pPlot == NULL)
	{
		return false;
	}

	if (pPlot->getTeam() != getTeam())
	{
		return false;
	}

	CvCity* pCity = pPlot->getPlotCity();
	if (pCity == NULL)
	{
		return false;
	}

	if (!pCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
	{
		return false;
	}

	if (getYieldStored() == 0)
	{
		return false;
	}

	if (!m_pUnitInfo->isTreasure())
	{
		return false;
	}

	// R&R, ray, Abandon City, START
	if (isHuman())
	{
		bool hasHarbor = false;
		for (int i = 0; i < GC.getNumBuildingInfos(); ++i)
		{
			BuildingTypes eLoopBuilding = (BuildingTypes)i;
			CvBuildingInfo& kLoopBuilding = GC.getBuildingInfo(eLoopBuilding);
			if(kLoopBuilding.getSeaPlotYieldChange(YIELD_FOOD) > 1 && pCity->isHasBuilding(eLoopBuilding))
			{
				hasHarbor = true;
				break;
			}
		}

		if (!hasHarbor)
		{
			return false;
		}
	}
	// R&R, ray, Abandon City, END

	return true;
}

void CvUnit::kingTransport(bool bSkipPopup)
{
	if (!canKingTransport())
	{
		return;
	}

	if (isHuman() && !bSkipPopup)
	{
		CvDiploParameters* pDiplo = new CvDiploParameters(GET_PLAYER(getOwnerINLINE()).getParent());
		pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_TREASURE_TRANSPORT"));
		pDiplo->setData(getID());
		int iCommission = GC.getDefineINT("KING_TRANSPORT_TREASURE_COMISSION");
		pDiplo->addDiploCommentVariable(iCommission);
		int iAmount = getYieldStored();
		iAmount -= (iAmount * iCommission) / 100;
		iAmount -= (iAmount * GET_PLAYER(getOwnerINLINE()).getTaxRate()) / 100;
		pDiplo->addDiploCommentVariable(iAmount);
		pDiplo->setAIContact(true);
		gDLL->beginDiplomacy(pDiplo, getOwnerINLINE());
	}
	else
	{
		doKingTransport();
	}
}

void CvUnit::doKingTransport()
{
	GET_PLAYER(getOwnerINLINE()).sellYieldUnitToEurope(this, getYieldStored(), GC.getDefineINT("KING_TRANSPORT_TREASURE_COMISSION"));
}


bool CvUnit::canEstablishMission() const
{
	if (getProfession() == NO_PROFESSION)
	{
		return false;
	}

	if (GC.getProfessionInfo(getProfession()).getMissionaryRate() <= 0)
	{
		return false;
	}

	if(!canMove())
	{
		return false;
	}

	CvPlot* pPlot = plot();
	if (pPlot == NULL)
	{
		return false;
	}

	CvCity* pCity = pPlot->getPlotCity();
	if (pCity == NULL)
	{
		return false;
	}

	CvPlayer& kCityOwner = GET_PLAYER(pCity->getOwnerINLINE());
	if (!kCityOwner.canHaveMission(getOwnerINLINE()))
	{
		return false;
	}

	if (pCity->getMissionaryCivilization() == getCivilizationType())
	{
		return false;
	}

	// R&R, ray, Natives do not talk when furious - START
	if (isHuman() && !kCityOwner.isHuman() && isAutomated())
	{
		int currentPlayerAttitude = kCityOwner.AI_getAttitude(getOwnerINLINE(), false);
		if (currentPlayerAttitude == 0)
		{
			return false;
		}
	}
	// R&R, ray, Natives do not talk when furious - END

	return true;
}


// WTP, ray, Native Trade Posts - START
bool CvUnit::canEstablishTradePost() const
{
	if (getProfession() == NO_PROFESSION)
	{
		return false;
	}

	if (GC.getProfessionInfo(getProfession()).getNativeTradeRate() <= 0)
	{
		return false;
	}

	if(!canMove())
	{
		return false;
	}

	CvPlot* pPlot = plot();
	if (pPlot == NULL)
	{
		return false;
	}

	CvCity* pCity = pPlot->getPlotCity();
	if (pCity == NULL)
	{
		return false;
	}

	CvPlayer& kCityOwner = GET_PLAYER(pCity->getOwnerINLINE());
	if (!kCityOwner.canHaveTradePost(getOwnerINLINE()))
	{
		return false;
	}

	if (pCity->getTradePostCivilization() == getCivilizationType())
	{
		return false;
	}

	// R&R, ray, Natives do not talk when furious - START
	if (isHuman() && !kCityOwner.isHuman() && isAutomated())
	{
		int currentPlayerAttitude = kCityOwner.AI_getAttitude(getOwnerINLINE(), false);
		if (currentPlayerAttitude == 0)
		{
			return false;
		}
	}
	// R&R, ray, Natives do not talk when furious - END

	return true;
}
// WTP, ray, Native Trade Posts - END


void CvUnit::establishMission()
{
	if (!canEstablishMission())
	{
		return;
	}

	CvCity* pCity = plot()->getPlotCity();
	CvPlayer& kCityOwner = GET_PLAYER(pCity->getOwnerINLINE());

	// R&R, ray, Natives do not talk when furious - START
	int currentPlayerAttitude = kCityOwner.AI_getAttitude(getOwnerINLINE(), false);
	if (isHuman() &&  currentPlayerAttitude == 0)
	{
		CvWString szBuffer = gDLL->getText("TXT_KEY_NATIVES_NOT_WILLING_TO_TALK", pCity->getNameKey());
		gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_ESTABLISH_MISSION).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);
		return;
	}
	// R&R, ray, Natives do not talk when furious - ELSE

	if (GC.getGameINLINE().getSorenRandNum(100, "Mission failure roll") > getMissionarySuccessPercent())
	{
		CvWString szBuffer = gDLL->getText("TXT_KEY_MISSION_FAILED", plot()->getPlotCity()->getNameKey());
		gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_ESTABLISH_MISSION).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
		GET_PLAYER(pCity->getOwnerINLINE()).AI_changeMemoryCount((getOwnerINLINE()), MEMORY_MISSIONARY_FAIL, 1);

		//Ramstormp, Disillusioned missionary - START
		if (GC.getGameINLINE().getSorenRandNum(100, "Dis mission roll") < getFailedMissionarySurvivalPercent())
		{
			UnitTypes FailedMissionaryType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_FAILED_MISSIONARY"));
			// WTP, ray, just for safety
			if (FailedMissionaryType != NO_UNIT)
			{
				CvUnit* FailedMissionaryUnit = GET_PLAYER(getOwnerINLINE()).initUnit(FailedMissionaryType, (ProfessionTypes)GC.getUnitInfo(FailedMissionaryType).getDefaultProfession(), pCity->getX_INLINE(), pCity->getY_INLINE());
				//  WTP, ray, we still need to display a message - your missionary failed and became a Failed Missionary
				CvWString szBuffer = gDLL->getText("TXT_KEY_FAILED_MISSIONARY_SPAWNED_FROM_FAIL", plot()->getPlotCity()->getNameKey());
				gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_ESTABLISH_MISSION).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
			}
		} 
		//Ramstormp, Disillusioned missionary - END
	}

	else
	{
		GET_PLAYER(getOwnerINLINE()).setMissionarySuccessPercent(GET_PLAYER(getOwnerINLINE()).getMissionarySuccessPercent() * GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getMissionFailureThresholdPercent() / 100);

		int iMissionaryRate = GC.getProfessionInfo(getProfession()).getMissionaryRate() * (100 + getUnitInfo().getMissionaryRateModifier()) / 100;
		if (!isHuman())
		{
			iMissionaryRate = (iMissionaryRate * 100 + 50) / GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIGrowthPercent();
		}

		// R&R, ray, Rebuild Missioning Start
		int attitudeIncreaseForMission = GC.getDefineINT("GAIN_ATTITUDE_FOR_MISSION");
		kCityOwner.AI_changeAttitudeExtra(getOwnerINLINE(), attitudeIncreaseForMission);
		if (pCity->getMissionaryPlayer() != NO_PLAYER)
		{
			CvPlayer& oldMissionaryPlayer = GET_PLAYER(pCity->getMissionaryPlayer());
			int attitudeDecreaseForDestroyingMission = GC.getDefineINT("OTHER_EUROPEAN_ANGRY_FOR_DESTROYING_MISSION");
			oldMissionaryPlayer.AI_changeAttitudeExtra(getOwnerINLINE(), -attitudeDecreaseForDestroyingMission);

			//Ramstormp, Disillusioned Missionary - START
			if (GC.getGameINLINE().getSorenRandNum(100, "Dis missal roll") < GC.getDefineINT("EXPELLED_MISSIONARY_SURVIVAL_CHANCE"))
			{
				UnitTypes FailedMissionaryType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_FAILED_MISSIONARY"));
				if (FailedMissionaryType != NO_UNIT)
				{
					//  WTP, ray, we still need to display a message - your missionary was thrown out because new Mission and became a Failed Missionary
					CvUnit* FailedMissionaryUnit = oldMissionaryPlayer.initUnit(FailedMissionaryType, (ProfessionTypes)GC.getUnitInfo(FailedMissionaryType).getDefaultProfession(), pCity->getX_INLINE(), pCity->getY_INLINE());
					//  WTP, ray, we still need to display a message - your missionary was thrown out because new Mission and became a Failed Missionary
					CvWString szBuffer = gDLL->getText("TXT_KEY_FAILED_MISSIONARY_SPAWNED_FROM_MISSION_REPLACED", plot()->getPlotCity()->getNameKey());
					gDLL->getInterfaceIFace()->addMessage(pCity->getMissionaryPlayer(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_ESTABLISH_MISSION).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
				}
			}
			//Ramstormp, Disillusioned missionary - END

		}
		// R&R, ray, Rebuild Missioning End

		pCity->setMissionaryPlayer(getOwnerINLINE());
		pCity->setMissionaryRate(iMissionaryRate);

		for (int i = 0; i < GC.getNumFatherPointInfos(); ++i)
		{
			FatherPointTypes ePointType = (FatherPointTypes) i;
			int gameSpeedMod =  GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
			GET_PLAYER(getOwnerINLINE()).changeFatherPoints(ePointType, GC.getFatherPointInfo(ePointType).getMissionaryPoints() * gameSpeedMod/100);
		}
	}

	kill(true);
}

// WTP, ray, Native Trade Posts - START
void CvUnit::establishTradePost()
{
	if (!canEstablishTradePost())
	{
		return;
	}

	CvCity* pCity = plot()->getPlotCity();
	CvPlayer& kCityOwner = GET_PLAYER(pCity->getOwnerINLINE());

	// R&R, ray, Natives do not talk when furious - START
	int currentPlayerAttitude = kCityOwner.AI_getAttitude(getOwnerINLINE(), false);
	if (isHuman() &&  currentPlayerAttitude == 0)
	{
		CvWString szBuffer = gDLL->getText("TXT_KEY_NATIVES_NOT_WILLING_TO_TALK", pCity->getNameKey());
		gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_ESTABLISH_TRADE_POST).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);
		return;
	}
	// R&R, ray, Natives do not talk when furious - ELSE

	if (GC.getGameINLINE().getSorenRandNum(100, "Trade Post failure roll") > getNativeTradePostSuccessPercent())
	{
		CvWString szBuffer = gDLL->getText("TXT_KEY_TRADE_POST_FAILED", plot()->getPlotCity()->getNameKey());
		gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_ESTABLISH_TRADE_POST).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
		// removed the Memory Count because it display strange unfitting since it is not a Mission and there is no special Memory Count for Trade Posts
		//GET_PLAYER(pCity->getOwnerINLINE()).AI_changeMemoryCount((getOwnerINLINE()), MEMORY_MISSIONARY_FAIL, 1);
	}
	else
	{
		GET_PLAYER(getOwnerINLINE()).setNativeTradePostSuccessPercent(GET_PLAYER(getOwnerINLINE()).getNativeTradePostSuccessPercent() * GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getMissionFailureThresholdPercent() / 100);

		int iNativeTradeRate = GC.getProfessionInfo(getProfession()).getNativeTradeRate() * (100 + getUnitInfo().getNativeTradeRateModifier()) / 100;
		if (!isHuman())
		{
			iNativeTradeRate = (iNativeTradeRate * 100 + 50) / GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIGrowthPercent();
		}

		// change Attitudes
		int attitudeIncreaseForTradePost = GC.getDefineINT("GAIN_ATTITUDE_FOR_TRADE_POST");
		kCityOwner.AI_changeAttitudeExtra(getOwnerINLINE(), attitudeIncreaseForTradePost);
		if (pCity->getTradePostPlayer() != NO_PLAYER)
		{
			CvPlayer& oldTradePostPlayer = GET_PLAYER(pCity->getTradePostPlayer());
			int attitudeDecreaseForDestroyingTradePost = GC.getDefineINT("OTHER_EUROPEAN_ANGRY_FOR_DESTROYING_TRADE_POST");
			oldTradePostPlayer.AI_changeAttitudeExtra(getOwnerINLINE(), -attitudeDecreaseForDestroyingTradePost);
		}

		pCity->setTradePostPlayer(getOwnerINLINE());
		pCity->setNativeTradeRate(iNativeTradeRate);

		for (int i = 0; i < GC.getNumFatherPointInfos(); ++i)
		{
			FatherPointTypes ePointType = (FatherPointTypes) i;
			int gameSpeedMod =  GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
			GET_PLAYER(getOwnerINLINE()).changeFatherPoints(ePointType, GC.getFatherPointInfo(ePointType).getMissionaryPoints() * gameSpeedMod/100);
		}
	}

	kill(true);
}
// WTP, ray, Native Trade Posts - END

int CvUnit::getMissionarySuccessPercent() const
{
	// R&R, ray, Rebuild Missioning Start

	CvCity* pCity = NULL;
	CvPlot* pPlot = plot();
	if (pPlot != NULL)
	{
		pCity = pPlot->getPlotCity();
	}

	//needed for UnitAI, includes small fix
	if (pCity == NULL || !pCity->isNative())
	{
		//Old Code
		return GET_PLAYER(getOwnerINLINE()).getMissionarySuccessPercent() * (100 + (getUnitInfo().getMissionaryRateModifier() * GC.getDefineINT("MISSIONARY_RATE_EFFECT_ON_SUCCESS") / 100)) / 100;
	}

	// 1. Find out how many missions already exist at this Native-Nation -> Missioning Rate
	CvPlayer& kCityOwner = GET_PLAYER(pCity->getOwnerINLINE());

	int numCitiesTotal = kCityOwner.getNumCities();

	int numCitiesMissioned = 0;
	
	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = kCityOwner.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kCityOwner.nextCity(&iLoop))
	{
		if (pLoopCity->getMissionaryPlayer() != NO_PLAYER)
		{
			numCitiesMissioned = (numCitiesMissioned + 1);
		}
	}

	// 2. Get Attitude of Natives to currently missioning Player
	int attitudeNativeToMissioningPlayer = kCityOwner.AI_getAttitude(getOwnerINLINE(), false);

	// 3. Check Attitudes of old missioninh player and existing Mission
	int attitudeChanceImprovement = GC.getDefineINT("CHANCE_IMPROVEMENT_EACH_ATTITIUDE_LEVEL");
	int penaltyExisingMission = 0;
	int attitudeNativeToOldMissioningPlayer = 0;

	if (pCity->getMissionaryPlayer() != NO_PLAYER)
	{
		attitudeNativeToOldMissioningPlayer = kCityOwner.AI_getAttitude(pCity->getMissionaryPlayer(), false);
		penaltyExisingMission = GC.getDefineINT("CHANCE_PENALTY_OTHER_MISSION_ALREADY_EXISTS");
	}

	// 4. Getting the expertmodifier of this unit
	int expertModifier = getUnitInfo().getMissionaryRateModifier();

	// 5. Getting Min and Max
	int minChance = GC.getDefineINT("MIN_CHANCE_MISSIONING");
	int maxChance = GC.getDefineINT("MAX_CHANCE_MISSIONING");

	// 6. Putting it all together

	//expert starts with higher value
	int totalChance = 50 + expertModifier / 2;
	//the more missions exist, the harder it gets
	totalChance = totalChance - 100 * numCitiesMissioned / numCitiesTotal;
	//now take a look at attitudes
	totalChance = totalChance + attitudeChanceImprovement * (attitudeNativeToMissioningPlayer - attitudeNativeToOldMissioningPlayer);
	//now substract the penalty for exising mission
	totalChance = totalChance - penaltyExisingMission;

	if (totalChance > maxChance)
	{
		totalChance = maxChance;
	}

	else if (totalChance < minChance)
	{
		totalChance = minChance;
	}

	return totalChance;
	// R&R, ray, Rebuild Missioning End
}


//Ramstormp, Disillusioned missionary - START
int CvUnit::getFailedMissionarySurvivalPercent() const
{
	int survivalChance = 0;

	// this is checking for an Expert
	if (getUnitInfo().getMissionaryRateModifier() > 0)
	{
		survivalChance = GC.getDefineINT("FAILED_EXPERT_MISSIONARY_SURVIVAL_CHANCE");
	}

	// otherwise it is not an Expert
	else
	{
		survivalChance = GC.getDefineINT("FAILED_REGULAR_MISSIONARY_SURVIVAL_CHANCE");
	}

	// just for safety if XML config was messed up, checking for smaller 0 is enough
	if (survivalChance < 0)
	{	
		return 0;
	}
	return survivalChance;
}
//Ramstormp, Disillusioned missionary - END


// WTP, ray, Native Trade Posts - START
int CvUnit::getNativeTradePostSuccessPercent() const
{
	CvCity* pCity = NULL;
	CvPlot* pPlot = plot();
	if (pPlot != NULL)
	{
		pCity = pPlot->getPlotCity();
	}

	//needed for UnitAI, includes small fix
	if (pCity == NULL || !pCity->isNative())
	{
		//Old Code
		return GET_PLAYER(getOwnerINLINE()).getNativeTradePostSuccessPercent() * (100 + (getUnitInfo().getNativeTradeRateModifier() * GC.getDefineINT("TRADER_RATE_EFFECT_ON_SUCCESS") / 100)) / 100;
	}

	// 1. Find out how many missions already exist at this Native-Nation -> Missioning Rate
	CvPlayer& kCityOwner = GET_PLAYER(pCity->getOwnerINLINE());

	int numCitiesTotal = kCityOwner.getNumCities();

	int numCitiesTradePosts = 0;
	
	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = kCityOwner.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kCityOwner.nextCity(&iLoop))
	{
		if (pLoopCity->getTradePostPlayer() != NO_PLAYER)
		{
			numCitiesTradePosts = (numCitiesTradePosts + 1);
		}
	}

	// 2. Get Attitude of Natives to currently missioning Player
	int attitudeNativeToTradePostPlayer = kCityOwner.AI_getAttitude(getOwnerINLINE(), false);

	// 3. Check Attitudes of old missioninh player and existing Mission
	int attitudeChanceImprovement = GC.getDefineINT("CHANCE_IMPROVEMENT_EACH_ATTITIUDE_LEVEL");
	int penaltyExisingTradePost = 0;
	int attitudeNativeToOldTradePostPlayer = 0;

	if (pCity->getTradePostPlayer() != NO_PLAYER)
	{
		attitudeNativeToOldTradePostPlayer = kCityOwner.AI_getAttitude(pCity->getTradePostPlayer(), false);
		penaltyExisingTradePost = GC.getDefineINT("CHANCE_PENALTY_OTHER_TRADE_POST_ALREADY_EXISTS");
	}

	// 4. Getting the expertmodifier of this unit
	int expertModifier = getUnitInfo().getNativeTradeRateModifier();

	// 5. Getting Min and Max
	int minChance = GC.getDefineINT("MIN_CHANCE_TRADE_POST");
	int maxChance = GC.getDefineINT("MAX_CHANCE_TRADE_POST");

	// 6. Putting it all together

	//expert starts with higher value
	int totalChance = 50 + expertModifier / 2;
	//the more trade posts exist, the harder it gets
	totalChance = totalChance - 100 * numCitiesTradePosts / numCitiesTotal;
	//now take a look at attitudes
	totalChance = totalChance + attitudeChanceImprovement * (attitudeNativeToTradePostPlayer - attitudeNativeToOldTradePostPlayer);
	//now substract the penalty for exising trade post
	totalChance = totalChance - penaltyExisingTradePost;

	if (totalChance > maxChance)
	{
		totalChance = maxChance;
	}

	else if (totalChance < minChance)
	{
		totalChance = minChance;
	}

	return totalChance;

}
// WTP, ray, Native Trade Posts - END


// R&R, ray , Stirring Up Natives - START
bool CvUnit::canStirUp() const
{
	// for Safety
	if (!isHuman())
	{
		return false;
	}

	if (getProfession() == NO_PROFESSION)
	{
		return false;
	}

	if (GC.getProfessionInfo(getProfession()).getMissionaryRate() <= 0)
	{
		return false;
	}

	if(!canMove())
	{
		return false;
	}

	CvPlot* pPlot = plot();
	if (pPlot == NULL)
	{
		return false;
	}

	CvCity* pCity = pPlot->getPlotCity();
	if (pCity == NULL)
	{
		return false;
	}

	CvPlayer& kCityOwner = GET_PLAYER(pCity->getOwnerINLINE());
	if (!kCityOwner.canHaveMission(getOwnerINLINE()))
	{
		return false;
	}

	if (pCity->getMissionaryCivilization() == getCivilizationType())
	{
		return false;
	}

	// check if available Native
	// No, we better simply create a new Unit

	// check if already stirred up
	if (pCity->getHasBeenStirredUp())
	{
		return false;
	}

	return true;
}


void CvUnit::stirUpNatives()
{
	if (!canStirUp())
	{
		return;
	}

	CvCity* pCity = plot()->getPlotCity();
	pCity->setHasBeenStirredUp(true);

	CvPlayer& kCityOwner = GET_PLAYER(pCity->getOwnerINLINE());

	// R&R, ray, Natives do not talk when furious - START
	int currentPlayerAttitude = kCityOwner.AI_getAttitude(getOwnerINLINE(), false);
	if (isHuman() && currentPlayerAttitude == 0)
	{
		CvWString szBuffer = gDLL->getText("TXT_KEY_NATIVES_NOT_WILLING_TO_TALK", plot()->getPlotCity()->getNameKey());
		gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_STIR_UP_NATIVES).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);
	}
	// R&R, ray, Natives do not talk when furious - ELSE

	else if (GC.getGameINLINE().getSorenRandNum(100, "Stir Up failure roll") > getStirUpSuccessPercent())
	{
		CvWString szBuffer = gDLL->getText("TXT_KEY_STIR_UP_FAILED", plot()->getPlotCity()->getNameKey());
		gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_STIR_UP_NATIVES).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);
		
		kCityOwner.AI_changeAttitudeExtra(getOwnerINLINE(), -1);
	}
	else
	{
		CvWString szBuffer = gDLL->getText("TXT_KEY_STIR_UP_SUCCESSFULL", plot()->getPlotCity()->getNameKey());
		gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_STIR_UP_NATIVES).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
	
		UnitTypes DefaultUnitType = (UnitTypes)GC.getCivilizationInfo(kCityOwner.getCivilizationType()).getCivilizationUnits(GC.getDefineINT("DEFAULT_POPULATION_UNIT"));		
		if (NO_UNIT != DefaultUnitType)
		{
			CvUnit* pUnit = kCityOwner.initUnit(DefaultUnitType, (ProfessionTypes) GC.getCivilizationInfo(kCityOwner.getCivilizationType()).getDefaultProfession(), getX_INLINE(), getY_INLINE());			
			
			CvPlayerAI& kNativeAI = GET_PLAYER(pCity->getOwner());
			if (kNativeAI.AI_hasPotentialRaidTarget())
			{	
				if (kNativeAI.AI_findTargetCity(pCity->area()) != NULL)
				{
					pUnit->AI_setUnitAIState(UNITAI_STATE_RAIDING_PARTY);
				}
			}
		}
	}

	return;
}

int CvUnit::getStirUpSuccessPercent() const
{
	CvPlot* pPlot = plot();

	if (pPlot == NULL)
	{
		return 0;
	}

	CvCity* pCity = pPlot->getPlotCity();
	if (pCity == NULL)
	{
		return 0;
	}

	CvPlayer& kCityOwner = GET_PLAYER(pCity->getOwnerINLINE());

	int attitudeNativeToMissionaryPlayer = kCityOwner.AI_getAttitude(getOwnerINLINE(), false);

	int stirUpBaseChance = GC.getDefineINT("STIR_UP_BASE_CHANCE");
	int stirUpAttitudeChange = GC.getDefineINT("STIR_UP_CHANCE_INCREASE_PER_ATTITUDE_LEVEL");

	int stirUpChances = stirUpBaseChance  + attitudeNativeToMissionaryPlayer * stirUpAttitudeChange;

	// higher Chances for Experts
	int expertModifier = getUnitInfo().getMissionaryRateModifier();
	if (expertModifier > 0)
	{
		stirUpChances = stirUpChances + expertModifier / 5;
	}

	if (pCity->getMissionaryPlayer() != NO_PLAYER)
	{
		stirUpChances = stirUpChances - 50;
	}

	int max = GC.getDefineINT("STIR_UP_CHANCE_MAX");
	int min = GC.getDefineINT("STIR_UP_CHANCE_MIN");

	if (stirUpChances > max)
	{
		stirUpChances = max;
	}

	if (stirUpChances < min)
	{
		stirUpChances = min;
	}

	return stirUpChances;
}
// R&R, ray , Stirring Up Natives - END

bool CvUnit::canSpeakWithChief(CvPlot* pPlot) const
{
	ProfessionTypes eProfession = getProfession();
	if (eProfession == NO_PROFESSION)
	{
		return false;
	}

	if (pPlot != NULL)
	{
		CvCity* pCity = pPlot->getPlotCity();
		if (pCity == NULL)
		{
			return false;
		}

		if (!pCity->isNative())
		{
			return false;
		}

		if (pCity->isScoutVisited(getTeam()))
		{
			return false;
		}

		// R&R, ray, Natives do not talk when furious - START
		if (isHuman() && isAutomated())
		{
			PlayerTypes eNativePlayer = pCity->getOwnerINLINE();
			if (!GET_PLAYER(eNativePlayer).isHuman())
			{
				int currentPlayerAttitude = GET_PLAYER(eNativePlayer).AI_getAttitude(getOwnerINLINE(), false);
				if (currentPlayerAttitude == 0)
				{
					return false;
				}
			}
		}
		// R&R, ray, Natives do not talk when furious - END

	}

	if (isNative())
	{
		return false;
	}
	// < JAnimals Mod Start >
	if (isBarbarian())
	{
		return false;
	}
	// < JAnimals Mod End >
	if (!canMove())
	{
		return false;
	}

	return true;
}

void CvUnit::speakWithChief()
{
	if(!canSpeakWithChief(plot()))
	{
		return;
	}

	CvCity* pCity = plot()->getPlotCity();
	GoodyTypes eGoody = pCity->getGoodyType(this);
	PlayerTypes eNativePlayer = pCity->getOwnerINLINE();

	if (isHuman() && !GET_PLAYER(eNativePlayer).isHuman())
	{
		// R&R, ray, Natives do not talk when furious - START
		int currentPlayerAttitude = GET_PLAYER(eNativePlayer).AI_getAttitude(getOwnerINLINE(), false);
		if (currentPlayerAttitude == 0)
		{
			CvWString szBuffer = gDLL->getText("TXT_KEY_NATIVES_NOT_WILLING_TO_TALK", pCity->getNameKey());
			gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_SPEAK_WITH_CHIEF).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);
			return;
		}
		// R&R, ray, Natives do not talk when furious - END

		CvWString szExpertText;
		int iGoodyValue = pCity->doGoody(this, eGoody);
		UnitClassTypes eTeachUnitClass = pCity->getTeachUnitClass();
		if (eTeachUnitClass != NO_UNITCLASS)
		{
			UnitTypes eTeachUnit = (UnitTypes) GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(eTeachUnitClass);
			if (eTeachUnit != NO_UNIT)
			{
				szExpertText = gDLL->getText("AI_DIPLO_CHIEF_LEARN_UNIT_DESCRIPTION", GC.getUnitInfo(eTeachUnit).getTextKeyWide());
			}
		}

		CvWString szYieldText;
		YieldTypes eDesiredYield = pCity->AI_getDesiredYield();
		if (eDesiredYield != NO_YIELD)
		{
			szYieldText = gDLL->getText("AI_DIPLO_CHIEF_DESIRED_YIELD_DESCRIPTION", GC.getYieldInfo(eDesiredYield).getTextKeyWide());
		}

		CvWString szGoodyText;
		if (eGoody != NO_GOODY)
		{
			szGoodyText = gDLL->getText(GC.getGoodyInfo(eGoody).getChiefTextKey(), iGoodyValue);
		}

		CvDiploParameters* pDiplo = new CvDiploParameters(pCity->getOwnerINLINE());
		pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_CHIEF_GOODY"));
		pDiplo->addDiploCommentVariable(pCity->getNameKey());
		pDiplo->addDiploCommentVariable(szExpertText);
		pDiplo->addDiploCommentVariable(szYieldText);
		pDiplo->addDiploCommentVariable(szGoodyText);
		pDiplo->setAIContact(true);
		pDiplo->setCity(pCity->getIDInfo());
		gDLL->beginDiplomacy(pDiplo, getOwnerINLINE());
	}
	else
	{
		pCity->doGoody(this, eGoody);
	}
}


bool CvUnit::canHold(const CvPlot* pPlot) const
{
	return true;
}


bool CvUnit::canSleep(const CvPlot* pPlot) const
{
	if (isFortifyable())
	{
		return false;
	}

	if (isWaiting())
	{
		return false;
	}

	return true;
}


bool CvUnit::canFortify(const CvPlot* pPlot) const
{
	if (!isFortifyable())
	{
		return false;
	}

	if (isWaiting())
	{
		return false;
	}

	return true;
}


bool CvUnit::canHeal(const CvPlot* pPlot) const
{
	if (!isHurt())
	{
		return false;
	}

	if (isWaiting())
	{
		return false;
	}

	if (healRate(pPlot) <= 0)
	{
		return false;
	}

	return true;
}


bool CvUnit::canSentry(const CvPlot* pPlot) const
{
	if (!canDefend(pPlot))
	{
		return false;
	}

	if (isWaiting())
	{
		return false;
	}

	return true;
}


int CvUnit::healRate(const CvPlot* pPlot) const
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pLoopPlot;
	int iHeal;
	int iBestHeal;
	int iI;


	int iTotalHeal = 0;

	if (pPlot->isCity(true, getTeam()))
	{
		iTotalHeal += GC.getDefineINT("CITY_HEAL_RATE") + (GET_TEAM(getTeam()).isFriendlyTerritory(pPlot->getTeam()) ? getExtraFriendlyHeal() : getExtraNeutralHeal());
		CvCity* pCity = pPlot->getPlotCity();
		if (pCity && !pCity->isOccupation())
		{
			iTotalHeal += pCity->getHealRate();
		}
	}
	else
	{
		if (!GET_TEAM(getTeam()).isFriendlyTerritory(pPlot->getTeam()))
		{
			if (isEnemy(pPlot->getTeam(), pPlot))
			{
				iTotalHeal += (GC.getDefineINT("ENEMY_HEAL_RATE") + getExtraEnemyHeal());
			}
			else
			{
				iTotalHeal += (GC.getDefineINT("NEUTRAL_HEAL_RATE") + getExtraNeutralHeal());
			}
		}
		else
		{
			iTotalHeal += (GC.getDefineINT("FRIENDLY_HEAL_RATE") + getExtraFriendlyHeal());
		}
	}

	// XXX optimize this (save it?)
	iBestHeal = 0;

	pUnitNode = pPlot->headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if (pLoopUnit->getTeam() == getTeam()) // XXX what about alliances?
		{
			iHeal = pLoopUnit->getSameTileHeal();

			if (iHeal > iBestHeal)
			{
				iBestHeal = iHeal;
			}
		}
	}

	for (iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(pPlot->getX_INLINE(), pPlot->getY_INLINE(), ((DirectionTypes)iI));

		if (pLoopPlot != NULL)
		{
			if (pLoopPlot->area() == pPlot->area())
			{
				pUnitNode = pLoopPlot->headUnitNode();

				while (pUnitNode != NULL)
				{
					pLoopUnit = ::getUnit(pUnitNode->m_data);
					pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

					if (pLoopUnit->getTeam() == getTeam()) // XXX what about alliances?
					{
						iHeal = pLoopUnit->getAdjacentTileHeal();

						if (iHeal > iBestHeal)
						{
							iBestHeal = iHeal;
						}
					}
				}
			}
		}
	}

	iTotalHeal += iBestHeal;
	// XXX

	return iTotalHeal;
}


int CvUnit::healTurns(const CvPlot* pPlot) const
{
	if (!isHurt())
		return 0;

	int iHeal = healRate(pPlot);

	/*************************************************************************************************/
	/* UNOFFICIAL_PATCH                       06/02/10                           LunarMongoose       */
	/*                                                                                               */
	/* Bugfix                                                                                        */
	/*************************************************************************************************/
	// Mongoose FeatureDamageFix
	FeatureTypes eFeature = pPlot->getFeatureType();
	if (eFeature != NO_FEATURE)
	{
		iHeal -= GC.getFeatureInfo(eFeature).getTurnDamage();
	}
	/*************************************************************************************************/
	/* UNOFFICIAL_PATCH                         END                                                  */
	/*************************************************************************************************/

	if (iHeal > 0)
	{
		/*
		iTurns = (getDamage() / iHeal);

		if ((getDamage() % iHeal) != 0)
		{
		iTurns++;
		}

		return iTurns; */
		return (getDamage() + iHeal - 1) / iHeal; // K-Mod (same, but faster)
	}
	else
	{
		return MAX_INT;
	}
}


void CvUnit::doHeal()
{
	changeDamage(-(healRate(plot())));
}


CvCity* CvUnit::bombardTarget(const CvPlot* pPlot) const
{
	int iBestValue = MAX_INT;
	CvCity* pBestCity = NULL;

	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pLoopPlot = plotDirection(pPlot->getX_INLINE(), pPlot->getY_INLINE(), ((DirectionTypes)iI));

		if (pLoopPlot != NULL)
		{
			CvCity* pLoopCity = pLoopPlot->getPlotCity();

			if (pLoopCity != NULL)
			{
				if (pLoopCity->isBombardable(this))
				{
					int iValue = pLoopCity->getDefenseDamage();

					// always prefer cities we are at war with
					if (isEnemy(pLoopCity->getTeam(), pPlot))
					{
						iValue *= 128;
					}

					if (iValue < iBestValue)
					{
						iBestValue = iValue;
						pBestCity = pLoopCity;
					}
				}
			}
		}
	}

	return pBestCity;
}

// Super Forts begin *bombard*
CvPlot* CvUnit::bombardImprovementTarget(const CvPlot* pPlot) const
{
	int iBestValue = MAX_INT;
	CvPlot* pBestPlot = NULL;

	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pLoopPlot = plotDirection(pPlot->getX_INLINE(), pPlot->getY_INLINE(), ((DirectionTypes)iI));

		if (pLoopPlot != NULL)
		{
			if (pLoopPlot->isBombardable(this))
			{
				int iValue = pLoopPlot->getDefenseDamage();

				// always prefer cities we are at war with
				if (isEnemy(pLoopPlot->getTeam(), pPlot))
				{
					iValue *= 128;
				}

				if (iValue < iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = pLoopPlot;
				}
			}
		}
	}

	return pBestPlot;
}
// Super Forts end


bool CvUnit::canBombard(const CvPlot* pPlot) const
{
	if (bombardRate() <= 0)
	{
		return false;
	}

	if (isMadeAttack())
	{
		return false;
	}

	if (isCargo())
	{
		return false;
	}

	// Super Forts begin *bombard*
	if (bombardTarget(pPlot) == NULL && bombardImprovementTarget(pPlot) == NULL)
	//if (bombardTarget(pPlot) == NULL) - Original Code
	// Super Forts end
	{
		return false;
	}

	return true;
}


bool CvUnit::bombard()
{
	CvPlot* pPlot = plot();
	if (!canBombard(pPlot))
	{
		return false;
	}

	CvCity* pBombardCity = bombardTarget(pPlot);
	// Super Forts begin *bombard*
	//FAssertMsg(pBombardCity != NULL, "BombardCity is not assigned a valid value"); - Removed for Super Forts

	CvPlot* pTargetPlot;
	//CvPlot* pTargetPlot = pBombardCity->plot(); - Original Code
	if(pBombardCity != NULL)
	{
		pTargetPlot = pBombardCity->plot();
	}
	else
	{
		pTargetPlot = bombardImprovementTarget(pPlot);
	}
	// Super Forts end

	if (!isEnemy(pTargetPlot->getTeam()))
	{
		getGroup()->groupDeclareWar(pTargetPlot, true);
	}

	if (!isEnemy(pTargetPlot->getTeam()))
	{
		return false;
	}

	setMadeAttack(true);
	changeMoves(GC.getMOVE_DENOMINATOR());

	// Super Forts begin *bombard* *text*	
	//R&R mod, vetiarvind, super forts merge ..invert if clause and let R&R code be the case when pBombardCity != null
	if(pBombardCity == NULL)
	{		
		pTargetPlot->changeDefenseDamage(bombardRate());

		CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_DEFENSES_IN_CITY_REDUCED_TO", GC.getImprovementInfo(pTargetPlot->getImprovementType()).getText(),
			(GC.getImprovementInfo(pTargetPlot->getImprovementType()).getDefenseModifier()-pTargetPlot->getDefenseDamage()), GET_PLAYER(getOwnerINLINE()).getNameKey());
		gDLL->getInterfaceIFace()->addMessage(pTargetPlot->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", MESSAGE_TYPE_INFO, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pTargetPlot->getX_INLINE(), pTargetPlot->getY_INLINE(), true, true);

		szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_REDUCE_CITY_DEFENSES", getNameKey(), GC.getImprovementInfo(pTargetPlot->getImprovementType()).getText(), 
			(GC.getImprovementInfo(pTargetPlot->getImprovementType()).getDefenseModifier()-pTargetPlot->getDefenseDamage()));
		gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARD", MESSAGE_TYPE_INFO, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pTargetPlot->getX_INLINE(), pTargetPlot->getY_INLINE());		
	}
	else
	{

		pBombardCity->changeDefenseModifier(-(bombardRate() * std::max(0, 100 - pBombardCity->getBuildingBombardDefense())) / 100);

		CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_DEFENSES_IN_CITY_REDUCED_TO", pBombardCity->getNameKey(), pBombardCity->getDefenseModifier(), GET_PLAYER(getOwnerINLINE()).getNameKey());
		gDLL->getInterfaceIFace()->addMessage(pBombardCity->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", MESSAGE_TYPE_INFO, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pBombardCity->getX_INLINE(), pBombardCity->getY_INLINE(), true, true);

		szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_REDUCE_CITY_DEFENSES", getNameOrProfessionKey(), pBombardCity->getNameKey(), pBombardCity->getDefenseModifier());
		gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARD", MESSAGE_TYPE_INFO, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pBombardCity->getX_INLINE(), pBombardCity->getY_INLINE());

	}//super forts
	if (pPlot->isActiveVisible(false))
	{
		// Super Forts begin *bombard*
		CvUnit *pDefender = pTargetPlot->getBestDefender(NO_PLAYER, getOwnerINLINE(), this, true);
		//CvUnit *pDefender = pBombardCity->plot()->getBestDefender(NO_PLAYER, getOwnerINLINE(), this, true); - Original Code
		// Super Forts end		

		// Bombard entity mission
		CvMissionDefinition kDefiniton;
		kDefiniton.setMissionTime(GC.getMissionInfo(MISSION_BOMBARD).getTime() * gDLL->getSecsPerTurn());
		kDefiniton.setMissionType(MISSION_BOMBARD);
		// Super Forts begin *bombard*
		kDefiniton.setPlot(pTargetPlot);
		//kDefiniton.setPlot(pBombardCity->plot()); - Original Code
		// Super Forts end
		kDefiniton.setUnit(BATTLE_UNIT_ATTACKER, this);
		kDefiniton.setUnit(BATTLE_UNIT_DEFENDER, pDefender);
		gDLL->getEntityIFace()->AddMission(&kDefiniton);
	}

	return true;
}


bool CvUnit::canPillage(const CvPlot* pPlot) const
{
	if (!canAttack())
	{
		return false;
	}

	if (pPlot->isCity())
	{
		return false;
	}

	if (pPlot->getImprovementType() == NO_IMPROVEMENT)
	{
		if (!(pPlot->isRoute()))
		{
			return false;
		}
	}
	else
	{
		if (GC.getImprovementInfo(pPlot->getImprovementType()).isPermanent())
		{
			return false;
		}
	}

	if (pPlot->isOwned())
	{
		if (!potentialWarAction(pPlot))
		{
			if ((pPlot->getImprovementType() == NO_IMPROVEMENT) || (pPlot->getOwnerINLINE() != getOwnerINLINE()))
			{
				return false;
			}
		}
	}

	if (!(pPlot->isValidDomainForAction(*this)))
	{
		return false;
	}

	return true;
}


bool CvUnit::pillage()
{
	CvWString szBuffer;
	int iPillageGold;
	long lPillageGold;
	ImprovementTypes eTempImprovement = NO_IMPROVEMENT;
	RouteTypes eTempRoute = NO_ROUTE;

	CvPlot* pPlot = plot();

	if (!canPillage(pPlot))
	{
		return false;
	}

	if (pPlot->isOwned())
	{
		// we should not be calling this without declaring war first, so do not declare war here
		if (!isEnemy(pPlot->getTeam(), pPlot))
		{
			if ((pPlot->getImprovementType() == NO_IMPROVEMENT) || (pPlot->getOwnerINLINE() != getOwnerINLINE()))
			{
				return false;
			}
		}
	}

	if (pPlot->getImprovementType() != NO_IMPROVEMENT)
	{
		eTempImprovement = pPlot->getImprovementType();
		int iGrowthPercent = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();

		if (pPlot->getTeam() != getTeam())
		{
			// Use python to determine pillage amounts...
			//lPillageGold = 0;
			lPillageGold = -1; // K-Mod

			if (GC.getUSE_DO_PILLAGE_GOLD_CALLBACK()) // K-Mod. I've writen C to replace the python callback.
			{
				CyPlot* pyPlot = new CyPlot(pPlot);
				CyUnit* pyUnit = new CyUnit(this);

				CyArgsList argsList;
				argsList.add(gDLL->getPythonIFace()->makePythonObject(pyPlot));	// pass in plot class
				argsList.add(gDLL->getPythonIFace()->makePythonObject(pyUnit));	// pass in unit class

				gDLL->getPythonIFace()->callFunction(PYGameModule, "doPillageGold", argsList.makeFunctionArgs(),&lPillageGold);

				delete pyPlot;	// python fxn must not hold on to this pointer
				delete pyUnit;	// python fxn must not hold on to this pointer

			
				lPillageGold *= iGrowthPercent;
				lPillageGold /= 100;
				iPillageGold = (int)lPillageGold;
			}
			// K-Mod. C version of the original python code
			if (lPillageGold < 0)
			{
				int iPillageBase = GC.getImprovementInfo((ImprovementTypes)pPlot->getImprovementType()).getPillageGold();
				iPillageGold = 0;
				iPillageGold += GC.getGameINLINE().getSorenRandNum(iPillageBase, "Pillage Gold 1");
				iPillageGold += GC.getGameINLINE().getSorenRandNum(iPillageBase, "Pillage Gold 2");
				iPillageGold += getPillageChange() * iPillageGold / 100;
			}
			// K-Mod end

			if (iPillageGold > 0)
			{
				GET_PLAYER(getOwnerINLINE()).changeGold(iPillageGold);

				szBuffer = gDLL->getText("TXT_KEY_MISC_PLUNDERED_GOLD_FROM_IMP", iPillageGold, GC.getImprovementInfo(pPlot->getImprovementType()).getTextKeyWide());
				gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGE", MESSAGE_TYPE_INFO, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX_INLINE(), pPlot->getY_INLINE());

				if (pPlot->isOwned())
				{
					szBuffer = gDLL->getText("TXT_KEY_MISC_IMP_DESTROYED", GC.getImprovementInfo(pPlot->getImprovementType()).getTextKeyWide(), getNameOrProfessionKey(), getVisualCivAdjective(pPlot->getTeam()));
					gDLL->getInterfaceIFace()->addMessage(pPlot->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGED", MESSAGE_TYPE_INFO, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pPlot->getX_INLINE(), pPlot->getY_INLINE(), true, true);
				}
			}
		}

		pPlot->setImprovementType((ImprovementTypes)(GC.getImprovementInfo(pPlot->getImprovementType()).getImprovementPillage()));
	}
	else if (pPlot->isRoute())
	{
		eTempRoute = pPlot->getRouteType();
		pPlot->setRouteType(NO_ROUTE); // XXX downgrade rail???
	}

	changeMoves(GC.getMOVE_DENOMINATOR());

	if (pPlot->isActiveVisible(false))
	{
		// Pillage entity mission
		CvMissionDefinition kDefiniton;
		kDefiniton.setMissionTime(GC.getMissionInfo(MISSION_PILLAGE).getTime() * gDLL->getSecsPerTurn());
		kDefiniton.setMissionType(MISSION_PILLAGE);
		kDefiniton.setPlot(pPlot);
		kDefiniton.setUnit(BATTLE_UNIT_ATTACKER, this);
		kDefiniton.setUnit(BATTLE_UNIT_DEFENDER, NULL);
		gDLL->getEntityIFace()->AddMission(&kDefiniton);
	}

	if (eTempImprovement != NO_IMPROVEMENT || eTempRoute != NO_ROUTE)
	{
		gDLL->getEventReporterIFace()->unitPillage(this, eTempImprovement, eTempRoute, getOwnerINLINE());
	}

	return true;
}

bool CvUnit::canFound(const CvPlot* pPlot, bool bTestVisible) const
{
	// R&R, ray, changes to Wild Animals - START
	if(GC.getGameINLINE().isBarbarianPlayer(getOwnerINLINE()))
	{
		return false;
	}
	// R&R, ray, changes to Wild Animals - END

	// Erik: Eventually we'll check against a XML tag, but this'll have to do for now
	if (isPrisonerOrSlave())
	{
		return false;
	}

	if (!m_pUnitInfo->isFound())
	{
		return false;
	}

	if (getProfession() != NO_PROFESSION)
	{
		if (!GC.getProfessionInfo(getProfession()).canFound())
		{
			return false;
		}
	}

	if (pPlot != NULL)
	{
		if (pPlot->isCity())
		{
			return false;
		}

		if (!(GET_PLAYER(getOwnerINLINE()).canFound(pPlot->getX_INLINE(), pPlot->getY_INLINE(), bTestVisible)))
		{
			return false;
		}
	}

	return true;
}


bool CvUnit::found()
{
	if (!canFound(plot()))
	{
		return false;
	}

	CvPlayer& kPlayer = GET_PLAYER(getOwnerINLINE());

	PlayerTypes eParent = kPlayer.getParent();
	if (eParent != NO_PLAYER && !GC.getEraInfo(kPlayer.getCurrentEra()).isRevolution() && !isAutomated())
	{
		int iFoodDifference = plot()->calculateNatureYield(YIELD_FOOD, getTeam(), true) - GC.getFOOD_CONSUMPTION_PER_POPULATION();
		bool bInland = !plot()->isCoastalLand(GC.getDefineINT("MIN_WATER_SIZE_FOR_OCEAN"));

		DiploCommentTypes eDiploComment = NO_DIPLOCOMMENT;
		if (iFoodDifference < 0 && kPlayer.shouldDisplayFeatPopup(FEAT_CITY_NO_FOOD))
		{
			eDiploComment = (DiploCommentTypes) GC.getInfoTypeForString("AI_DIPLOCOMMENT_FOUND_CITY_NO_FOOD");
			kPlayer.setFeatAccomplished(FEAT_CITY_NO_FOOD, true);
		}
		else if (bInland && kPlayer.shouldDisplayFeatPopup(FEAT_CITY_INLAND))
		{
			eDiploComment = (DiploCommentTypes) GC.getInfoTypeForString("AI_DIPLOCOMMENT_FOUND_CITY_INLAND");
			kPlayer.setFeatAccomplished(FEAT_CITY_INLAND, true);
		}

		if (eDiploComment != NO_DIPLOCOMMENT)
		{
			CvDiploParameters* pDiplo = new CvDiploParameters(eParent);
			pDiplo->setDiploComment(eDiploComment);
			pDiplo->setData(getID());
			pDiplo->setAIContact(true);
			gDLL->beginDiplomacy(pDiplo, getOwnerINLINE());
			return true;
		}
	}

	return doFoundCheckNatives();
}

bool CvUnit::doFoundCheckNatives()
{
	if (!canFound(plot()))
	{
		return false;
	}

	if (isHuman() && !isAutomated())
	{
		PlayerTypes eNativeOwner = NO_PLAYER;
		int iCost = 0;
		for (int i = 0; i < NUM_CITY_PLOTS_RADIUS_1; ++i) // R&R, ray, 2 Plot Radius
		{
			CvPlot* pLoopPlot = ::plotCity(getX_INLINE(), getY_INLINE(), i);
			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->isOwned() && !pLoopPlot->isCity())
				{
					if (GET_PLAYER(pLoopPlot->getOwnerINLINE()).isNative() && !GET_TEAM(pLoopPlot->getTeam()).isAtWar(getTeam()))
					{
						eNativeOwner = pLoopPlot->getOwnerINLINE();
						iCost += pLoopPlot->getBuyPrice(getOwnerINLINE());
					}
				}
			}
		}

		if (eNativeOwner != NO_PLAYER)
		{
			GET_TEAM(getTeam()).meet(GET_PLAYER(eNativeOwner).getTeam(), false);
		}

		if (eNativeOwner != NO_PLAYER && !GET_PLAYER(eNativeOwner).isHuman())
		{
			CvDiploParameters* pDiplo = new CvDiploParameters(eNativeOwner);
			if (GET_PLAYER(getOwnerINLINE()).getNumCities() == 0)
			{
				pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_FOUND_FIRST_CITY"));
			}
			else if(iCost > GET_PLAYER(getOwnerINLINE()).getGold())
			{
				pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_FOUND_CITY_CANT_AFFORD"));
				pDiplo->addDiploCommentVariable(iCost);
			}
			else
			{
				pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_FOUND_CITY"));
				pDiplo->addDiploCommentVariable(iCost);
			}
			pDiplo->setData(getID());
			pDiplo->setAIContact(true);
			gDLL->beginDiplomacy(pDiplo, getOwnerINLINE());
		}
		else
		{
			doFound(false);
		}
	}
	else
	{
		AI_doFound();
	}

	return true;
}

//ray18
bool CvUnit::doAcquireCheckNatives()
{
	if (isHuman() && !isAutomated())
	{
		PlayerTypes eNativeOwner = NO_PLAYER;
		int iCost = 0;
		for (int i = 0; i < NUM_CITY_PLOTS_RADIUS_1; ++i) // R&R, ray, 2 Plot Radius
		{
			CvPlot* pLoopPlot = ::plotCity(getX_INLINE(), getY_INLINE(), i);
			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->isOwned() && !pLoopPlot->isCity())
				{
					PlayerTypes ePossibleNativeOwner = pLoopPlot->findHighestCulturePlayer();
					//if (GET_PLAYER(pLoopPlot->getOwnerINLINE()).isNative() && !GET_TEAM(pLoopPlot->getTeam()).isAtWar(getTeam()))
					if (GET_PLAYER(ePossibleNativeOwner).isNative() && !GET_TEAM(GET_PLAYER(ePossibleNativeOwner).getTeam()).isAtWar(getTeam()))
					{
						eNativeOwner = ePossibleNativeOwner;
						iCost += pLoopPlot->getBuyPrice(ePossibleNativeOwner);
					}
				}
			}
		}

		if (eNativeOwner != NO_PLAYER)
		{
			moneyToBuyLand = iCost;
			playerToBuyLand = eNativeOwner;
			GET_TEAM(getTeam()).meet(GET_PLAYER(eNativeOwner).getTeam(), false);
		}

		if (eNativeOwner != NO_PLAYER && !GET_PLAYER(eNativeOwner).isHuman())
		{
			CvDiploParameters* pDiplo = new CvDiploParameters(eNativeOwner);
			
			if(iCost > GET_PLAYER(getOwnerINLINE()).getGold())
			{
				pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_ACQUIRE_CITY_CANT_AFFORD"));
				pDiplo->addDiploCommentVariable(iCost);
			}
			else
			{
				pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_ACQUIRE_CITY"));
				pDiplo->addDiploCommentVariable(iCost);
			}
		
			pDiplo->setData(getID());
			pDiplo->setAIContact(true);
			gDLL->beginDiplomacy(pDiplo, getOwnerINLINE());
		}
	}
		
	return true;
}

void CvUnit::buyLandAfterAcquire()
{
	if (playerToBuyLand != NO_PLAYER)
	{
		//resetting to Peace if Acquiring City has caused war
		if (GET_TEAM(GET_PLAYER(playerToBuyLand).getTeam()).isAtWar(getTeam()))
		{
			GET_TEAM(GET_PLAYER(playerToBuyLand).getTeam()).makePeace(getTeam());
			CvWString szBuffer = gDLL->getText("TXT_KEY_MAKE_PEACE_AFTER_ACQUIRE_CITY", GET_PLAYER(playerToBuyLand).getNameKey());
			gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getEraInfo(GC.getGameINLINE().getCurrentEra()).getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), NULL, NULL);
		}

		GET_PLAYER(playerToBuyLand).changeGold((moneyToBuyLand * GC.getDefineINT("BUY_PLOT_SELLER_INCOME_PERCENT")) / 100);
		GET_PLAYER(getOwnerINLINE()).AI_changeGoldTradedTo(playerToBuyLand, moneyToBuyLand);
		GET_PLAYER(getOwnerINLINE()).changeGold(moneyToBuyLand * -1);		
	}
}
//Ende ray18

bool CvUnit::doFound(bool bBuyLand)
{
	if (!canFound(plot()))
	{
		return false;
	}

	if (GC.getGameINLINE().getActivePlayer() == getOwnerINLINE())
	{
		gDLL->getInterfaceIFace()->lookAt(plot()->getPoint(), CAMERALOOKAT_NORMAL);
	}

	//first city takes land for free
	bool bIsFirstCity = (GET_PLAYER(getOwnerINLINE()).getNumCities() == 0);
	if (bBuyLand || bIsFirstCity)
	{
		for (int i = 0; i < NUM_CITY_PLOTS_RADIUS_1; ++i) // R&R, ray, 2 Plot Radius
		{
			CvPlot* pLoopPlot = ::plotCity(getX_INLINE(), getY_INLINE(), i);
			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->isOwned() && !pLoopPlot->isCity())
				{
					//don't buy land if at war, it will be taken
					if (GET_PLAYER(pLoopPlot->getOwnerINLINE()).isNative() && !GET_TEAM(pLoopPlot->getTeam()).isAtWar(getTeam()))
					{
						GET_PLAYER(getOwnerINLINE()).buyLand(pLoopPlot, bIsFirstCity);
					}
				}
			}
		}
	}

	GET_PLAYER(getOwnerINLINE()).found(getX_INLINE(), getY_INLINE());

	CvPlot* pCityPlot = GC.getMapINLINE().plotINLINE(getX_INLINE(), getY_INLINE());
	FAssert(NULL != pCityPlot);
	if (pCityPlot != NULL)
	{
		if (pCityPlot->isActiveVisible(false))
		{
			NotifyEntity(MISSION_FOUND);
			EffectTypes eEffect = (EffectTypes)GC.getInfoTypeForString("EFFECT_SETTLERSMOKE");
			gDLL->getEngineIFace()->TriggerEffect(eEffect, pCityPlot->getPoint(), (float)(GC.getASyncRand().get(360)));
			gDLL->getInterfaceIFace()->playGeneralSound("AS3D_UN_FOUND_CITY", pCityPlot->getPoint());
		}

		CvCity* pCity = pCityPlot->getPlotCity();
		FAssert(NULL != pCity);
		if (NULL != pCity)
		{
			pCity->addPopulationUnit(this, NO_PROFESSION);

			//WTP, ray, Settler Professsion - START
			//consuming YIELDS when Founding a City
			for (int i=0; i < NUM_YIELD_TYPES; ++i)
			{
				pCity->setYieldStored((YieldTypes) i, 0);
			}
			//WTP, ray, Settler Professsion - END
		}
	}

	// TAC - AI City Defense - koma13 - START
	CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());
	PlayerTypes eParent = kPlayer.getParent();

	if (!isHuman() && eParent != NO_PLAYER)
	{
		// R&R, ray, changed for atmospherical reasons for AI to not always have cannons - START

		int iFreeUnitRand = GC.getGameINLINE().getSorenRandNum(10, "Pick Free Unit");
		UnitTypes eFreeDefender = NO_UNIT;

		// Militia
		if (iFreeUnitRand <= 3)
		{
			eFreeDefender = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_MILITIA"));
		}

		// Veteran
		else if (iFreeUnitRand <= 7)
		{
			eFreeDefender = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_VETERAN"));
		}

		// currently usually a cannon
		else
		{
			int iLowestCost = MAX_INT;
		
			for (int iUnitClass = 0; iUnitClass < GC.getNumUnitClassInfos(); ++iUnitClass)
			{
				UnitTypes eLoopUnit = (UnitTypes) GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iUnitClass);
				if (eLoopUnit != NO_UNIT)
				{
					if (GC.getUnitInfo(eLoopUnit).getDefaultUnitAIType() == UNITAI_DEFENSIVE)
					{
						int iCost = kPlayer.getEuropeUnitBuyPrice(eLoopUnit);
						if (iCost < iLowestCost && iCost >= 0)
						{
							iLowestCost = iCost;
							eFreeDefender = eLoopUnit;
						}
					}
				}
			}
		}

		// now create the unit
		if (eFreeDefender != NO_UNIT)
		{
			for (int iI = 0; iI < GC.getAI_FREE_CITY_DEFENDERS(); iI++)
			{
				CvUnit* pUnit = kPlayer.initEuropeUnit(eFreeDefender);
				FAssert(pUnit != NULL);
				pUnit->AI_setUnitAIType(UNITAI_DEFENSIVE);

				//kPlayer.AI_updateNextBuyUnit();
			}
		}
		// R&R, ray, changed for atmospherical reasons for AI to not always have cannons - START
	}
	// TAC - AI City Defense - koma13 - END
	
	return true;
}
// TAC - Clear Specialty Fix - koma13 - START
//bool CvUnit::canJoinCity(const CvPlot* pPlot, bool bTestVisible) const
bool CvUnit::canJoinCity(const CvPlot* pPlot, bool bTestVisible, bool bIgnoreFood) const
// TAC - Clear Specialty Fix - koma13 - END
{
	CvCity* pCity = pPlot->getPlotCity();

	if (pCity == NULL)
	{
		return false;
	}

	if (pCity->getOwnerINLINE() != getOwnerINLINE())
	{
		return false;
	}

	if (pCity->isDisorder())
	{
		return false;
	}

	if (!m_pUnitInfo->isFound())
	{
		return false;
	}

	if (isDelayedDeath())
	{
		return false;
	}

	if (!bTestVisible)
	{
		if (pCity->getRawYieldProduced(YIELD_FOOD) < pCity->getPopulation() * GC.getFOOD_CONSUMPTION_PER_POPULATION())
		{
			// TAC - Clear Specialty Fix - koma13 - START
			//if (!canJoinStarvingCity(*pCity))
			if (!canJoinStarvingCity(*pCity) && !bIgnoreFood)
			{
				return false;
			}
			// TAC - Clear Specialty Fix - koma13 - END
		}

		ProfessionTypes eProfession = getProfession();
		if (eProfession == NO_PROFESSION || GC.getProfessionInfo(eProfession).isUnarmed() || GC.getProfessionInfo(eProfession).isCitizen())
		{
			if (movesLeft() == 0)
			{
				return false;
			}
		}
		else
		{
			if (hasMoved())
			{
				return false;
			}
		}
	}
	// TAC - AI Attack City - koma13 - START
	if (!isHuman())
	{
		if (AI_getUnitAIType() == UNITAI_OFFENSIVE)
		{
			return false;
		}
		//if (getExperiencePercent() > 0)
		//{
		//	return false;
		//}
	}
	// TAC - AI Attack City - koma13 - END

	return true;
}

bool CvUnit::canJoinStarvingCity(const CvCity& kCity) const
{
	FAssert(kCity.foodDifference() < 0);

	if (kCity.getYieldStored(YIELD_FOOD) >= GC.getGameINLINE().getCargoYieldCapacity() / 4)
	{
		return true;
	}

	int iNewPop = kCity.getPopulation() + 1;
	if (kCity.AI_getFoodGatherable(iNewPop, 0) >= iNewPop * GC.getFOOD_CONSUMPTION_PER_POPULATION())
	{
		return true;
	}

	if (!isHuman())
	{
		ProfessionTypes eProfession = AI_getIdealProfession();
		if (eProfession != NO_PROFESSION)
		{
			// R&R, ray , MYCP partially based on code of Aymerick - START
			for (int i = 0; i < GC.getProfessionInfo(eProfession).getNumYieldsProduced(); i++)
			{
				if (GC.getProfessionInfo(eProfession).getYieldsProduced(i) == YIELD_FOOD)
				{
					return true;
				}
			}
			// R&R, ray , MYCP partially based on code of Aymerick - END
		}
	}

	return false;
}

bool CvUnit::joinCity()
{
	if (!canJoinCity(plot()))
	{
		return false;
	}

	if (plot()->isActiveVisible(false))
	{
		NotifyEntity(MISSION_JOIN_CITY);
	}

	CvCity* pCity = plot()->getPlotCity();

	if (pCity != NULL)
	{
		pCity->addPopulationUnit(this, NO_PROFESSION);
	}

	return true;
}

bool CvUnit::canBuild(const CvPlot* pPlot, BuildTypes eBuild, bool bTestVisible) const
{
    FAssertMsg(eBuild < GC.getNumBuildInfos(), "Index out of bounds");

	if (!(m_pUnitInfo->getBuilds(eBuild)))
	{
		return false;
	}

	if (workRate(true) <= 0)
	{
		return false;
	}

	if (!(GET_PLAYER(getOwnerINLINE()).canBuild(pPlot, eBuild, false, bTestVisible)))
	{
		return false;
	}

	if (!pPlot->isValidDomainForAction(*this))
	{
		return false;
	}

	// R&R, ray, prevent Forts and Monasteries to be built on Peaks - start
	if (pPlot->isPeak())
	{
		ImprovementTypes eImprovement = (ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement();
		if (eImprovement != NO_IMPROVEMENT && (GC.getImprovementInfo(eImprovement).isFort() || GC.getImprovementInfo(eImprovement).isMonastery()))
		{
			return false;
		}
	}
	// R&R, ray, prevent Forts and Monasteries to be built on Peaks - end

	// R&R, ray, AI Improvement - START
	if (!isHuman() || isAutomated())  //R&R 2.3, ray's fix for automated pioneers
	{
		// only go into here, if there is a Terrain Feature and build would remove it
		if (pPlot->getFeatureType() != NO_FEATURE && GC.getBuildInfo(eBuild).isFeatureRemove(pPlot->getFeatureType()))
		{	
			// R&R, ray, AI Improvement to not let AI destroy valuable Bonusses - START
			if (pPlot->getBonusType() != NO_BONUS)
			{
				CvBonusInfo& kBonus = GC.getBonusInfo(pPlot->getBonusType());
				if (!kBonus.isTerrain(pPlot->getTerrainType()) && kBonus.isFeature(pPlot->getFeatureType()))
				{
					return false;		
				}
			}
			// R&R, ray, AI Improvement to not let AI destroy valuable Bonusses - END

			// R&R, ray, prevent AI from stupidly making existing improvements useless -START
			if (pPlot->getImprovementType() != NO_IMPROVEMENT)
			{
				return false;
			}
			// R&R, ray, prevent AI from stupidly making existing improvements useless - END
		}
		// R&R, ray, prevent AI from stupidly replacing existing improvements -START
		if (GC.getBuildInfo(eBuild).getImprovement() != NO_IMPROVEMENT && pPlot->getImprovementType() != NO_IMPROVEMENT)
		{
			return false;
		}
		// R&R, ray, prevent AI from stupidly replacing improvements -END
	}
	// R&R, ray, AI Improvement - END

	return true;
}

// Returns true if build finished...
bool CvUnit::build(BuildTypes eBuild)
{
	bool bFinished;

	FAssertMsg(eBuild < GC.getNumBuildInfos(), "Invalid Build");

	if (!canBuild(plot(), eBuild))
	{
		return false;
	}

	// Note: notify entity must come before changeBuildProgress - because once the unit is done building,
	// that function will notify the entity to stop building.
	NotifyEntity((MissionTypes)GC.getBuildInfo(eBuild).getMissionType());

	GET_PLAYER(getOwnerINLINE()).changeGold(-(GET_PLAYER(getOwnerINLINE()).getBuildCost(plot(), eBuild)));

	bFinished = plot()->changeBuildProgress(eBuild, workRate(false), getTeam());

	finishMoves(); // needs to be at bottom because movesLeft() can affect workRate()...

	if (bFinished)
	{
		// Super Forts begin *culture*
		if (GC.getBuildInfo(eBuild).getImprovement() != NO_IMPROVEMENT)
		{
			if(GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement()).isActsAsCity())
			{
				if(plot()->getOwner() == NO_PLAYER)
				{
					plot()->setOwner(getOwnerINLINE(),true);
				}
			}
		}
		// Super Forts end
		if (GC.getBuildInfo(eBuild).isKill())
		{
			kill(true);
		}
	}

	// Python Event
	gDLL->getEventReporterIFace()->unitBuildImprovement(this, eBuild, bFinished);

	return bFinished;
}


bool CvUnit::canPromote(PromotionTypes ePromotion, int iLeaderUnitId) const
{
	if (ePromotion == NO_PROMOTION)
	{
		return false;
	}

	if (iLeaderUnitId >= 0)
	{
		if (iLeaderUnitId == getID())
		{
			return false;
		}

		if (!GC.getPromotionInfo(ePromotion).isLeader())
		{
			return false;
		}

		CvUnit* pWarlord = GET_PLAYER(getOwnerINLINE()).getUnit(iLeaderUnitId);
		if (pWarlord == NULL)
		{
			return false;
		}

		if (pWarlord->getUnitInfo().getLeaderPromotion() != ePromotion)
		{
			return false;
		}

		if (!canAcquirePromotion(ePromotion))
		{
			return false;
		}

		if (!canAcquirePromotionAny())
		{
			return false;
		}
	}
	else
	{
		if (GC.getPromotionInfo(ePromotion).isLeader())
		{
			return false;
		}

		if (!canAcquirePromotion(ePromotion))
		{
			return false;
		}

		if (!isPromotionReady())
		{
			return false;
		}
	}

	return true;
}

void CvUnit::promote(PromotionTypes ePromotion, int iLeaderUnitId)
{
	if (!canPromote(ePromotion, iLeaderUnitId))
	{
		return;
	}

	if (iLeaderUnitId >= 0)
	{
		CvUnit* pWarlord = GET_PLAYER(getOwnerINLINE()).getUnit(iLeaderUnitId);
		if (pWarlord)
		{
			pWarlord->giveExperience();
			if (!pWarlord->getNameNoDesc().empty())
			{
				setName(pWarlord->getNameKey());
			}

			//update graphics models
			m_eLeaderUnitType = pWarlord->getUnitType();
			reloadEntity();
		}
	}

	if (!GC.getPromotionInfo(ePromotion).isLeader())
	{
		changeLevel(1);
		changeDamage(-(getDamage() / 2));
	}

	setHasRealPromotion(ePromotion, true);

	testPromotionReady();

	if (IsSelected())
	{
		gDLL->getInterfaceIFace()->playGeneralSound(GC.getPromotionInfo(ePromotion).getSound());

		gDLL->getInterfaceIFace()->setDirty(UnitInfo_DIRTY_BIT, true);
	}
	else
	{
		setInfoBarDirty(true);
	}

	gDLL->getEventReporterIFace()->unitPromoted(this, ePromotion);
}

bool CvUnit::lead(int iUnitId)
{
	if (!canLead(plot(), iUnitId))
	{
		return false;
	}

	PromotionTypes eLeaderPromotion = (PromotionTypes)m_pUnitInfo->getLeaderPromotion();

	if (-1 == iUnitId)
	{
		FAssert(isHuman());
		CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_LEADUNIT, eLeaderPromotion, getID());
		if (pInfo)
		{
			gDLL->getInterfaceIFace()->addPopup(pInfo, getOwnerINLINE(), true);
		}
		return false;
	}
	else
	{
		CvUnit* pUnit = GET_PLAYER(getOwnerINLINE()).getUnit(iUnitId);
		if (!pUnit || !pUnit->canPromote(eLeaderPromotion, getID()))
		{
			return false;
		}

		pUnit->promote(eLeaderPromotion, getID());

		if (plot()->isActiveVisible(false))
		{
			NotifyEntity(MISSION_LEAD);
		}

		kill(true);

		return true;
	}
}


int CvUnit::canLead(const CvPlot* pPlot, int iUnitId) const
{
	PROFILE_FUNC();

	if (isDelayedDeath())
	{
		return 0;
	}

	if (NO_UNIT == getUnitType())
	{
		return 0;
	}

	int iNumUnits = 0;
	CvUnitInfo& kUnitInfo = getUnitInfo();

	if (-1 == iUnitId)
	{
		CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
		while(pUnitNode != NULL)
		{
			CvUnit* pUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = pPlot->nextUnitNode(pUnitNode);

			if (pUnit && pUnit != this && pUnit->getOwnerINLINE() == getOwnerINLINE() && pUnit->canPromote((PromotionTypes)kUnitInfo.getLeaderPromotion(), getID()))
			{
				++iNumUnits;
			}
		}
	}
	else
	{
		CvUnit* pUnit = GET_PLAYER(getOwnerINLINE()).getUnit(iUnitId);
		if (pUnit && pUnit != this && pUnit->canPromote((PromotionTypes)kUnitInfo.getLeaderPromotion(), getID()))
		{
			iNumUnits = 1;
		}
	}
	return iNumUnits;
}


int CvUnit::canGiveExperience(const CvPlot* pPlot) const
{
	int iNumUnits = 0;

	if (NO_UNIT != getUnitType() && m_pUnitInfo->getLeaderExperience() > 0)
	{
		CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
		while(pUnitNode != NULL)
		{
			CvUnit* pUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = pPlot->nextUnitNode(pUnitNode);

			if (pUnit && pUnit != this && pUnit->getOwnerINLINE() == getOwnerINLINE() && pUnit->canAcquirePromotionAny())
			{
				++iNumUnits;
			}
		}
	}

	return iNumUnits;
}

bool CvUnit::giveExperience()
{
	CvPlot* pPlot = plot();

	if (pPlot)
	{
		int iNumUnits = canGiveExperience(pPlot);
		if (iNumUnits > 0)
		{
			int iTotalExperience = getStackExperienceToGive(iNumUnits);

			int iMinExperiencePerUnit = iTotalExperience / iNumUnits;
			int iRemainder = iTotalExperience % iNumUnits;

			CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
			int i = 0;
			while(pUnitNode != NULL)
			{
				CvUnit* pUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = pPlot->nextUnitNode(pUnitNode);

				if (pUnit && pUnit != this && pUnit->getOwnerINLINE() == getOwnerINLINE() && pUnit->canAcquirePromotionAny())
				{
					pUnit->changeExperience(i < iRemainder ? iMinExperiencePerUnit+1 : iMinExperiencePerUnit);
					pUnit->testPromotionReady();
				}

				i++;
			}

			return true;
		}
	}

	return false;
}

int CvUnit::getStackExperienceToGive(int iNumUnits) const
{
	return (m_pUnitInfo->getLeaderExperience() * (100 + std::min(50, (iNumUnits - 1) * GC.getDefineINT("WARLORD_EXTRA_EXPERIENCE_PER_UNIT_PERCENT")))) / 100;
}

int CvUnit::upgradePrice(UnitTypes eUnit) const
{
	int iPrice;

	CyArgsList argsList;
	argsList.add(getOwner());
	argsList.add(getID());
	argsList.add((int) eUnit);
	long lResult=0;
	gDLL->getPythonIFace()->callFunction(PYGameModule, "getUpgradePriceOverride", argsList.makeFunctionArgs(), &lResult);
	if (lResult >= 0)
	{
		return lResult;
	}

	iPrice = GC.getDefineINT("BASE_UNIT_UPGRADE_COST");

	iPrice += (std::max(0, (GET_PLAYER(getOwnerINLINE()).getYieldProductionNeeded(eUnit, YIELD_HAMMERS) - GET_PLAYER(getOwnerINLINE()).getYieldProductionNeeded(getUnitType(), YIELD_HAMMERS))) * GC.getDefineINT("UNIT_UPGRADE_COST_PER_PRODUCTION"));

	if (!isHuman())
	{
		iPrice *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIUnitUpgradePercent();
		iPrice /= 100;

		iPrice *= std::max(0, ((GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIPerEraModifier() * GET_PLAYER(getOwnerINLINE()).getCurrentEra()) + 100));
		iPrice /= 100;
	}

	iPrice -= (iPrice * getUpgradeDiscount()) / 100;

	return iPrice;
}


bool CvUnit::upgradeAvailable(UnitTypes eFromUnit, UnitClassTypes eToUnitClass, int iCount) const
{
	UnitTypes eLoopUnit;
	int iI;
	int numUnitClassInfos = GC.getNumUnitClassInfos();

	if (iCount > numUnitClassInfos)
	{
		return false;
	}

	CvUnitInfo &fromUnitInfo = GC.getUnitInfo(eFromUnit);

	if (fromUnitInfo.getUpgradeUnitClass(eToUnitClass))
	{
		return true;
	}

	for (iI = 0; iI < numUnitClassInfos; iI++)
	{
		if (fromUnitInfo.getUpgradeUnitClass(iI))
		{
			eLoopUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI)));

			if (eLoopUnit != NO_UNIT)
			{
				if (upgradeAvailable(eLoopUnit, eToUnitClass, (iCount + 1)))
				{
					return true;
				}
			}
		}
	}

	return false;
}


bool CvUnit::canUpgrade(UnitTypes eUnit, bool bTestVisible) const
{
	if (eUnit == NO_UNIT)
	{
		return false;
	}

	if(!isReadyForUpgrade())
	{
		return false;
	}

	if (!bTestVisible)
	{
		if (GET_PLAYER(getOwnerINLINE()).getGold() < upgradePrice(eUnit))
		{
			return false;
		}
	}

	if (hasUpgrade(eUnit))
	{
		return true;
	}

	return false;
}

bool CvUnit::isReadyForUpgrade() const
{
	if (!canMove())
	{
		return false;
	}

	if (plot()->getTeam() != getTeam())
	{
		return false;
	}

	return true;
}

// has upgrade is used to determine if an upgrade is possible,
// it specifically does not check whether the unit can move, whether the current plot is owned, enough gold
// those are checked in canUpgrade()
// does not search all cities, only checks the closest one
bool CvUnit::hasUpgrade(bool bSearch) const
{
	return (getUpgradeCity(bSearch) != NULL);
}

// has upgrade is used to determine if an upgrade is possible,
// it specifically does not check whether the unit can move, whether the current plot is owned, enough gold
// those are checked in canUpgrade()
// does not search all cities, only checks the closest one
bool CvUnit::hasUpgrade(UnitTypes eUnit, bool bSearch) const
{
	return (getUpgradeCity(eUnit, bSearch) != NULL);
}

// finds the 'best' city which has a valid upgrade for the unit,
// it specifically does not check whether the unit can move, or if the player has enough gold to upgrade
// those are checked in canUpgrade()
// if bSearch is true, it will check every city, if not, it will only check the closest valid city
// NULL result means the upgrade is not possible
CvCity* CvUnit::getUpgradeCity(bool bSearch) const
{
	CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());
	UnitAITypes eUnitAI = AI_getUnitAIType();
	CvArea* pArea = area();

	int iCurrentValue = kPlayer.AI_unitValue(getUnitType(), eUnitAI, pArea);

	int iBestSearchValue = MAX_INT;
	CvCity* pBestUpgradeCity = NULL;

	for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		int iNewValue = kPlayer.AI_unitValue(((UnitTypes)iI), eUnitAI, pArea);
		if (iNewValue > iCurrentValue)
		{
			int iSearchValue;
			CvCity* pUpgradeCity = getUpgradeCity((UnitTypes)iI, bSearch, &iSearchValue);
			if (pUpgradeCity != NULL)
			{
				// if not searching or close enough, then this match will do
				if (!bSearch || iSearchValue < 16)
				{
					return pUpgradeCity;
				}

				if (iSearchValue < iBestSearchValue)
				{
					iBestSearchValue = iSearchValue;
					pBestUpgradeCity = pUpgradeCity;
				}
			}
		}
	}

	return pBestUpgradeCity;
}

// finds the 'best' city which has a valid upgrade for the unit, to eUnit type
// it specifically does not check whether the unit can move, or if the player has enough gold to upgrade
// those are checked in canUpgrade()
// if bSearch is true, it will check every city, if not, it will only check the closest valid city
// if iSearchValue non NULL, then on return it will be the city's proximity value, lower is better
// NULL result means the upgrade is not possible
CvCity* CvUnit::getUpgradeCity(UnitTypes eUnit, bool bSearch, int* iSearchValue) const
{
	if (eUnit == NO_UNIT)
	{
		return NULL;
	}

	CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());
	CvUnitInfo& kUnitInfo = GC.getUnitInfo(eUnit);

	if (GC.getCivilizationInfo(kPlayer.getCivilizationType()).getCivilizationUnits(kUnitInfo.getUnitClassType()) != eUnit)
	{
		return NULL;
	}

	if (!upgradeAvailable(getUnitType(), ((UnitClassTypes)(kUnitInfo.getUnitClassType()))))
	{
		return NULL;
	}

	if (kUnitInfo.getCargoSpace() < getCargo())
	{
		return NULL;
	}

	CLLNode<IDInfo>* pUnitNode = plot()->headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = plot()->nextUnitNode(pUnitNode);

		if (pLoopUnit->getTransportUnit() == this)
		{
			if (kUnitInfo.getSpecialCargo() != NO_SPECIALUNIT)
			{
				if (kUnitInfo.getSpecialCargo() != pLoopUnit->getSpecialUnitType())
				{
					return NULL;
				}
			}

			if (kUnitInfo.getDomainCargo() != NO_DOMAIN)
			{
				if (kUnitInfo.getDomainCargo() != pLoopUnit->getDomainType())
				{
					return NULL;
				}
			}
		}
	}

	// sea units must be built on the coast
	bool bCoastalOnly = (getDomainType() == DOMAIN_SEA);

	// results
	int iBestValue = MAX_INT;
	CvCity* pBestCity = NULL;

	// if search is true, check every city for our team
	if (bSearch)
	{
		TeamTypes eTeam = getTeam();
		int iArea = getArea();
		int iX = getX_INLINE(), iY = getY_INLINE();

		// check every player on our team's cities
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			// is this player on our team?
			CvPlayerAI& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
			if (kLoopPlayer.isAlive() && kLoopPlayer.getTeam() == eTeam)
			{
				int iLoop;
				for (CvCity* pLoopCity = kLoopPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kLoopPlayer.nextCity(&iLoop))
				{
					// if coastal only, then make sure we are coast
					CvArea* pWaterArea = NULL;
					if (!bCoastalOnly || ((pWaterArea = pLoopCity->waterArea()) != NULL && !pWaterArea->isLake()))
					{
						// can this city tran this unit?
						if (pLoopCity->canTrain(eUnit, false, false, true))
						{
							int iValue = plotDistance(iX, iY, pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE());

							// if not same area, not as good (lower numbers are better)
							if (iArea != pLoopCity->getArea() && (!bCoastalOnly || iArea != pWaterArea->getID()))
							{
								iValue *= 16;
							}

							// if we cannot path there, not as good (lower numbers are better)
							if (!generatePath(pLoopCity->plot(), 0, true))
							{
								iValue *= 16;
							}

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
	else
	{
		// find the closest city
		CvCity* pClosestCity = GC.getMapINLINE().findCity(getX_INLINE(), getY_INLINE(), NO_PLAYER, getTeam(), true, bCoastalOnly);
		if (pClosestCity != NULL)
		{
			// if we can train, then return this city (otherwise it will return NULL)
			if (pClosestCity->canTrain(eUnit, false, false, true))
			{
				// did not search, always return 1 for search value
				iBestValue = 1;

				pBestCity = pClosestCity;
			}
		}
	}

	// return the best value, if non-NULL
	if (iSearchValue != NULL)
	{
		*iSearchValue = iBestValue;
	}

	return pBestCity;
}

void CvUnit::upgrade(UnitTypes eUnit)
{
	CvUnit* pUpgradeUnit;

	if (!canUpgrade(eUnit))
	{
		return;
	}

	GET_PLAYER(getOwnerINLINE()).changeGold(-(upgradePrice(eUnit)));

	pUpgradeUnit = GET_PLAYER(getOwnerINLINE()).initUnit(eUnit, getProfession(), getX_INLINE(), getY_INLINE(), AI_getUnitAIType());

	FAssertMsg(pUpgradeUnit != NULL, "UpgradeUnit is not assigned a valid value");

	pUpgradeUnit->joinGroup(getGroup());

	pUpgradeUnit->convert(this, true);

	pUpgradeUnit->finishMoves();

	if (pUpgradeUnit->getLeaderUnitType() == NO_UNIT)
	{
		if (pUpgradeUnit->getExperience() > GC.getDefineINT("MAX_EXPERIENCE_AFTER_UPGRADE"))
		{
			pUpgradeUnit->setExperience(GC.getDefineINT("MAX_EXPERIENCE_AFTER_UPGRADE"));
		}
	}
}


HandicapTypes CvUnit::getHandicapType() const
{
	return GET_PLAYER(getOwnerINLINE()).getHandicapType();
}


CivilizationTypes CvUnit::getCivilizationType() const
{
	return GET_PLAYER(getOwnerINLINE()).getCivilizationType();
}

const wchar* CvUnit::getVisualCivAdjective(TeamTypes eForTeam) const
{
	if (getVisualOwner(eForTeam) == getOwnerINLINE())
	{
		return GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey();
	}

	return L"";
}

SpecialUnitTypes CvUnit::getSpecialUnitType() const
{
	return ((SpecialUnitTypes)(m_pUnitInfo->getSpecialUnitType()));
}


UnitTypes CvUnit::getCaptureUnitType(CivilizationTypes eCivilization) const
{
	FAssert(eCivilization != NO_CIVILIZATION);
    UnitTypes eCaptureUnit = NO_UNIT;
	if(m_pUnitInfo->getUnitCaptureClassType() != NO_UNITCLASS)
	{
		eCaptureUnit = (UnitTypes)GC.getCivilizationInfo(eCivilization).getCivilizationUnits(m_pUnitInfo->getUnitCaptureClassType());
	}

	if (eCaptureUnit == NO_UNIT && isUnarmed())
	{
		eCaptureUnit = (UnitTypes)GC.getCivilizationInfo(eCivilization).getCivilizationUnits(getUnitClassType());
	}

	if (eCaptureUnit == NO_UNIT)
	{
		return NO_UNIT;
	}

	CvUnitInfo& kUnitInfo = GC.getUnitInfo(eCaptureUnit);
	if (kUnitInfo.getDefaultProfession() != NO_PROFESSION)
	{
		CvCivilizationInfo& kCivInfo = GC.getCivilizationInfo(eCivilization);
		if (!kCivInfo.isValidProfession(kUnitInfo.getDefaultProfession()))
		{
			return NO_UNIT;
		}
	}

	return eCaptureUnit;
}

UnitCombatTypes CvUnit::getProfessionUnitCombatType(ProfessionTypes eProfession) const
{
	if (eProfession != NO_PROFESSION)
	{
		UnitCombatTypes eUnitCombat = (UnitCombatTypes) GC.getProfessionInfo(eProfession).getUnitCombatType();
		if(eUnitCombat != NO_UNITCOMBAT)
		{
			return eUnitCombat;
		}
	}

	return ((UnitCombatTypes)(m_pUnitInfo->getUnitCombatType()));
}

void CvUnit::processUnitCombatType(UnitCombatTypes eUnitCombat, int iChange)
{
	if (iChange != 0)
	{
		//update unit combat changes
		for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
		{
			if (GET_PLAYER(getOwnerINLINE()).hasTrait((TraitTypes)iI))
			{
				for (int iJ = 0; iJ < GC.getNumPromotionInfos(); iJ++)
				{
					if (GC.getTraitInfo((TraitTypes) iI).isFreePromotion(iJ))
					{
						if ((eUnitCombat != NO_UNITCOMBAT) && GC.getTraitInfo((TraitTypes) iI).isFreePromotionUnitCombat(eUnitCombat))
						{
							changeFreePromotionCount(((PromotionTypes)iJ), iChange);
						}
					}
				}
			}
		}

		if (NO_UNITCOMBAT != eUnitCombat)
		{
			for (int iJ = 0; iJ < GC.getNumPromotionInfos(); iJ++)
			{
				if (GET_PLAYER(getOwnerINLINE()).isFreePromotion(eUnitCombat, (PromotionTypes)iJ))
				{
					changeFreePromotionCount(((PromotionTypes)iJ), iChange);
				}
			}
		}

		if (IsSelected())
		{
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		}
	}
}

UnitCombatTypes CvUnit::getUnitCombatType() const
{
	return getProfessionUnitCombatType(getProfession());
}


DomainTypes CvUnit::getDomainType() const
{
	return ((DomainTypes)(m_pUnitInfo->getDomainType()));
}


InvisibleTypes CvUnit::getInvisibleType() const
{
	return ((InvisibleTypes)(m_pUnitInfo->getInvisibleType()));
}

int CvUnit::getNumSeeInvisibleTypes() const
{
	return m_pUnitInfo->getNumSeeInvisibleTypes();
}

InvisibleTypes CvUnit::getSeeInvisibleType(int i) const
{
	return (InvisibleTypes)(m_pUnitInfo->getSeeInvisibleType(i));
}

bool CvUnit::isHuman() const
{
	return GET_PLAYER(getOwnerINLINE()).isHuman();
}

bool CvUnit::isNative() const
{
	return GET_PLAYER(getOwnerINLINE()).isNative();
}

int CvUnit::visibilityRange() const
{
	// R&R mod, vetiarvind merge Super Forts begin *vision*
	int iImprovementVisibilityChange = 0;
	CvPlot* pPlot = plot();
	if(pPlot != NULL && pPlot->getImprovementType() != NO_IMPROVEMENT) //added null check
	{
		iImprovementVisibilityChange = GC.getImprovementInfo(plot()->getImprovementType()).getVisibilityChange();
	}
	return (GC.getUNIT_VISIBILITY_RANGE() + getExtraVisibilityRange() + iImprovementVisibilityChange);
	// Super Forts end
	// Original --	return (GC.getUNIT_VISIBILITY_RANGE() + getExtraVisibilityRange()); */
	
}

int CvUnit::baseMoves() const
{
	int iBaseMoves = m_pUnitInfo->getMoves();
	iBaseMoves += getExtraMoves();
	iBaseMoves += GET_PLAYER(getOwnerINLINE()).getUnitMoveChange(getUnitClassType());

	if(getProfession() != NO_PROFESSION)
	{
		iBaseMoves += GET_PLAYER(getOwnerINLINE()).getProfessionMoveChange(getProfession());
	}

	return iBaseMoves;
}


int CvUnit::maxMoves() const
{
	return (baseMoves() * GC.getMOVE_DENOMINATOR());
}


int CvUnit::movesLeft() const
{
	return std::max(0, (maxMoves() - getMoves()));
}


bool CvUnit::canMove() const
{
	if (isDead())
	{
		return false;
	}

	if (getMoves() >= maxMoves())
	{
		return false;
	}

	if (getImmobileTimer() > 0)
	{
		return false;
	}

	if (!isOnMap())
	{
		return false;
	}

	return true;
}


bool CvUnit::hasMoved()	const
{
	return (getMoves() > 0);
}


// XXX should this test for coal?
bool CvUnit::canBuildRoute() const
{
	int iI;

	for (iI = 0; iI < GC.getNumBuildInfos(); iI++)
	{
		if (GC.getBuildInfo((BuildTypes)iI).getRoute() != NO_ROUTE)
		{
			if (m_pUnitInfo->getBuilds(iI))
			{
				return true;
			}
		}
	}

	return false;
}

BuildTypes CvUnit::getBuildType() const
{
	CvSelectionGroup* pGroup = getGroup();
	if (pGroup == NULL)
	{
		return NO_BUILD;
	}

	if (pGroup->headMissionQueueNode() != NULL)
	{
		switch (pGroup->headMissionQueueNode()->m_data.eMissionType)
		{
		case MISSION_MOVE_TO:
			break;

		case MISSION_ROUTE_TO:
			{
				BuildTypes eBuild;
				if (pGroup->getBestBuildRoute(plot(), &eBuild) != NO_ROUTE)
				{
					return eBuild;
				}
			}
			break;

		case MISSION_MOVE_TO_UNIT:
		case MISSION_SKIP:
		case MISSION_SLEEP:
		case MISSION_FORTIFY:
		case MISSION_HEAL:
		case MISSION_SENTRY:
		case MISSION_BOMBARD:
		case MISSION_PILLAGE:
		case MISSION_FOUND:
		case MISSION_JOIN_CITY:
		case MISSION_LEAD:
		case MISSION_WHALING: //TAC Whaling, ray
		case MISSION_FISHING: // R&R, ray, High Sea Fishing
			break;

		case MISSION_BUILD:
			return (BuildTypes)pGroup->headMissionQueueNode()->m_data.iData1;
			break;

		default:
			FAssert(false);
			break;
		}
	}

	return NO_BUILD;
}


int CvUnit::workRate(bool bMax) const
{
	if (!bMax)
	{
		if (!canMove())
		{
			return 0;
		}
	}

	int iRate = m_pUnitInfo->getWorkRate() + getExtraWorkRate();

	iRate *= std::max(0, (GET_PLAYER(getOwnerINLINE()).getWorkerSpeedModifier() + m_pUnitInfo->getWorkRateModifier() + 100));
	iRate /= 100;

	if (!isHuman())
	{
		iRate *= std::max(0, (GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIWorkRateModifier() + 100));
		iRate /= 100;
	}
	
	for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); ++iTrait)
	{
		TraitTypes eTrait = (TraitTypes) iTrait;
		if (eTrait != NO_TRAIT)
		{
			if (GET_PLAYER(getOwnerINLINE()).hasTrait(eTrait))
			{
				iRate *= 100 + GC.getTraitInfo(eTrait).getPioneerSpeedModifier();
				iRate /= 100;
			}
		}			//Schmiddie, added Pioneer Speed modifier for Terraformer Trait
	}
	return iRate;
}

void CvUnit::changeExtraWorkRate(int iChange)
{
	m_iExtraWorkRate += iChange;
}

int CvUnit::getExtraWorkRate() const
{
	return m_iExtraWorkRate;

}

bool CvUnit::isNoBadGoodies() const
{
	return m_pUnitInfo->isNoBadGoodies();
}


bool CvUnit::isOnlyDefensive() const
{
	return m_pUnitInfo->isOnlyDefensive();
}


bool CvUnit::isNoUnitCapture() const
{
	return m_pUnitInfo->isNoCapture();
}


bool CvUnit::isNoCityCapture() const
{
	return m_pUnitInfo->isNoCapture();
}


bool CvUnit::isRivalTerritory() const
{
	return m_pUnitInfo->isRivalTerritory();
}

bool CvUnit::canCoexistWithEnemyUnit(TeamTypes eTeam) const
{
	if (!m_pUnitInfo->isInvisible())
	{
		if (getInvisibleType() == NO_INVISIBLE)
		{
			return false;
		}

		if (NO_TEAM == eTeam || plot()->isInvisibleVisible(eTeam, getInvisibleType()))
		{
			return false;
		}
	}

	return true;
}

bool CvUnit::isFighting() const
{
	return (getCombatUnit() != NULL);
}


bool CvUnit::isAttacking() const
{
	return (getAttackPlot() != NULL && !isDelayedDeath());
}


bool CvUnit::isDefending() const
{
	return (isFighting() && !isAttacking());
}


bool CvUnit::isCombat() const
{
	return (isFighting() || isAttacking());
}


int CvUnit::maxHitPoints() const
{
	return GC.getMAX_HIT_POINTS();
}


int CvUnit::currHitPoints()	const
{
	return (maxHitPoints() - getDamage());
}


bool CvUnit::isHurt() const
{
	return (getDamage() > 0);
}


bool CvUnit::isDead() const
{
	return (getDamage() >= maxHitPoints());
}


void CvUnit::setBaseCombatStr(int iCombat)
{
	m_iBaseCombat = iCombat;
	updateBestLandCombat();
}

int CvUnit::baseCombatStr() const
{
	return m_iBaseCombat;
}

void CvUnit::updateBestLandCombat()
{
	if (getDomainType() == DOMAIN_LAND)
	{
		if (baseCombatStr() > GC.getGameINLINE().getBestLandUnitCombat())
		{
			GC.getGameINLINE().setBestLandUnitCombat(baseCombatStr());
		}
	}
}


// maxCombatStr can be called in four different configurations
//		pPlot == NULL, pAttacker == NULL for combat when this is the attacker
//		pPlot valid, pAttacker valid for combat when this is the defender
//		pPlot valid, pAttacker == NULL (new case), when this is the defender, attacker unknown
//		pPlot valid, pAttacker == this (new case), when the defender is unknown, but we want to calc approx str
//			note, in this last case, it is expected pCombatDetails == NULL, it does not have to be, but some
//			values may be unexpectedly reversed in this case (iModifierTotal will be the negative sum)
int CvUnit::maxCombatStr(const CvPlot* pPlot, const CvUnit* pAttacker, CombatDetails* pCombatDetails) const
{
	int iCombat;

	FAssertMsg((pPlot == NULL) || (pPlot->getTerrainType() != NO_TERRAIN), "(pPlot == NULL) || (pPlot->getTerrainType() is not expected to be equal with NO_TERRAIN)");

	// handle our new special case
	const	CvPlot*	pAttackedPlot = NULL;
	bool	bAttackingUnknownDefender = false;
	if (pAttacker == this)
	{
		bAttackingUnknownDefender = true;
		pAttackedPlot = pPlot;

		// reset these values, we will fiddle with them below
		pPlot = NULL;
		pAttacker = NULL;
	}
	// otherwise, attack plot is the plot of us (the defender)
	else if (pAttacker != NULL)
	{
		pAttackedPlot = plot();
	}

	if (pCombatDetails != NULL)
	{
		pCombatDetails->iExtraCombatPercent = 0;
		pCombatDetails->iNativeCombatModifierTB = 0;
		pCombatDetails->iNativeCombatModifierAB = 0;
		pCombatDetails->iPlotDefenseModifier = 0;
		pCombatDetails->iFortifyModifier = 0;
		pCombatDetails->iCityDefenseModifier = 0;
		pCombatDetails->iHillsAttackModifier = 0;
		pCombatDetails->iHillsDefenseModifier = 0;
		pCombatDetails->iDomesticBonusModifier = 0;	
		pCombatDetails->iFeatureAttackModifier = 0;
		pCombatDetails->iFeatureDefenseModifier = 0;
		pCombatDetails->iTerrainAttackModifier = 0;
		pCombatDetails->iTerrainDefenseModifier = 0;
		pCombatDetails->iCityAttackModifier = 0;
		pCombatDetails->iDomainDefenseModifier = 0;
		pCombatDetails->iClassDefenseModifier = 0;
		pCombatDetails->iClassAttackModifier = 0;
		pCombatDetails->iCombatModifierA = 0;
		pCombatDetails->iCombatModifierT = 0;
		pCombatDetails->iDomainModifierA = 0;
		pCombatDetails->iDomainModifierT = 0;
		pCombatDetails->iRiverAttackModifier = 0;
		pCombatDetails->iAmphibAttackModifier = 0;
		pCombatDetails->iRebelPercentModifier = 0;
		pCombatDetails->iModifierTotal = 0;
		pCombatDetails->iBaseCombatStr = 0;
		pCombatDetails->iCombat = 0;
		pCombatDetails->iMaxCombatStr = 0;
		pCombatDetails->iCurrHitPoints = 0;
		pCombatDetails->iMaxHitPoints = 0;
		pCombatDetails->iCurrCombatStr = 0;
		pCombatDetails->eOwner = getOwnerINLINE();
		pCombatDetails->eVisualOwner = getVisualOwner();
		if (getProfession() == NO_PROFESSION)
		{
			pCombatDetails->sUnitName = getName().GetCString();
		}
		else
		{
			pCombatDetails->sUnitName = CvWString::format(L"%s (%s)", GC.getProfessionInfo(getProfession()).getDescription(), getName().GetCString());
		}
	}

	if (baseCombatStr() == 0)
	{
		return 0;
	}

	int iModifier = 0;
	int iExtraModifier;

	iExtraModifier = getExtraCombatPercent();
	iModifier += iExtraModifier;
	if (pCombatDetails != NULL)
	{
		pCombatDetails->iExtraCombatPercent = iExtraModifier;
	}

	if (pAttacker != NULL)
	{
		if (isNative())
		{
			iExtraModifier = -GET_PLAYER(pAttacker->getOwnerINLINE()).getNativeCombatModifier();
			if (!pAttacker->isHuman())
			{
				iExtraModifier -= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAINativeCombatModifier();
			}
			iModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iNativeCombatModifierTB = iExtraModifier;
			}
		}

		if (pAttacker->isNative())
		{
			iExtraModifier = GET_PLAYER(getOwnerINLINE()).getNativeCombatModifier();
			if (!isHuman())
			{
				iExtraModifier += GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAINativeCombatModifier();
			}
			iModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iNativeCombatModifierAB = iExtraModifier;
			}
		}

		iExtraModifier = rebelModifier(pAttacker->getOwnerINLINE()) - pAttacker->rebelModifier(getOwnerINLINE());
		iModifier += iExtraModifier;
		if (pCombatDetails != NULL)
		{
			pCombatDetails->iRebelPercentModifier = iExtraModifier;
		}
	}

	// add defensive bonuses (leaving these out for bAttackingUnknownDefender case)
	if (pPlot != NULL)
	{
		if (!noDefensiveBonus())
		{
			iExtraModifier = pPlot->defenseModifier(getTeam());
			iModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iPlotDefenseModifier = iExtraModifier;
			}
		}

		iExtraModifier = fortifyModifier();
		iModifier += iExtraModifier;
		if (pCombatDetails != NULL)
		{
			pCombatDetails->iFortifyModifier = iExtraModifier;
		}

		if (pPlot->isCity(true, getTeam()))
		{
			iExtraModifier = cityDefenseModifier();
			iModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iCityDefenseModifier = iExtraModifier;
			}
		}

		if (pPlot->isHills() || pPlot->isPeak())
		{
			iExtraModifier = hillsDefenseModifier();
			iModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iHillsDefenseModifier = iExtraModifier;
			}
		}

			if (pPlot->getOwner() == getOwner())
		{
			iExtraModifier = DomesticBonusModifier();
			iModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iDomesticBonusModifier = iExtraModifier;
			}
		}	
			//Schmiddie, combat bonus within cultural borders

		if (pPlot->getFeatureType() != NO_FEATURE)
		{
			iExtraModifier = featureDefenseModifier(pPlot->getFeatureType());
			iModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iFeatureDefenseModifier = iExtraModifier;
			}
		}
		else
		{
			iExtraModifier = terrainDefenseModifier(pPlot->getTerrainType());
			iModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iTerrainDefenseModifier = iExtraModifier;
			}
		}
	}

	// if we are attacking to an plot with an unknown defender, the calc the modifier in reverse
	if (bAttackingUnknownDefender)
	{
		pAttacker = this;
	}

	// calc attacker bonueses
	if (pAttacker != NULL)
	{
		int iTempModifier = 0;

		if (pAttackedPlot->isCity(true, getTeam()))
		{
			iExtraModifier = -pAttacker->cityAttackModifier();
			iTempModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iCityAttackModifier = iExtraModifier;
			}
		}

		if (pAttackedPlot->isHills() || pAttackedPlot->isPeak())
		{
			iExtraModifier = -pAttacker->hillsAttackModifier();
			iTempModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iHillsAttackModifier = iExtraModifier;
			}
		}

		if (pAttackedPlot->getOwner() == pAttacker->getOwner())
		{
			iExtraModifier = -pAttacker->DomesticBonusModifier();
			iTempModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iDomesticBonusModifier = iExtraModifier;
			}
		}		

		if (pAttackedPlot->getFeatureType() != NO_FEATURE)
		{
			iExtraModifier = -pAttacker->featureAttackModifier(pAttackedPlot->getFeatureType());
			iTempModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iFeatureAttackModifier = iExtraModifier;
			}
		}
		else
		{
			iExtraModifier = -pAttacker->terrainAttackModifier(pAttackedPlot->getTerrainType());
			iModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iTerrainAttackModifier = iExtraModifier;
			}
		}

		// only compute comparisions if we are the defender with a known attacker
		if (!bAttackingUnknownDefender)
		{
			FAssertMsg(pAttacker != this, "pAttacker is not expected to be equal with this");

			iExtraModifier = unitClassDefenseModifier(pAttacker->getUnitClassType());
			iTempModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iClassDefenseModifier = iExtraModifier;
			}

			iExtraModifier = -pAttacker->unitClassAttackModifier(getUnitClassType());
			iTempModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iClassAttackModifier = iExtraModifier;
			}

			if (pAttacker->getUnitCombatType() != NO_UNITCOMBAT)
			{
				iExtraModifier = unitCombatModifier(pAttacker->getUnitCombatType());
				iTempModifier += iExtraModifier;
				if (pCombatDetails != NULL)
				{
					pCombatDetails->iCombatModifierA = iExtraModifier;
				}
			}
			if (getUnitCombatType() != NO_UNITCOMBAT)
			{
				iExtraModifier = -pAttacker->unitCombatModifier(getUnitCombatType());
				iTempModifier += iExtraModifier;
				if (pCombatDetails != NULL)
				{
					pCombatDetails->iCombatModifierT = iExtraModifier;
				}
			}

			iExtraModifier = domainModifier(pAttacker->getDomainType());
			iTempModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iDomainModifierA = iExtraModifier;
			}

			iExtraModifier = -pAttacker->domainModifier(getDomainType());
			iTempModifier += iExtraModifier;
			if (pCombatDetails != NULL)
			{
				pCombatDetails->iDomainModifierT = iExtraModifier;
			}
		}

		if (!(pAttacker->isRiver()))
		{
			if (pAttacker->plot()->isRiverCrossing(directionXY(pAttacker->plot(), pAttackedPlot)))
			{
				iExtraModifier = -GC.getRIVER_ATTACK_MODIFIER();
				iTempModifier += iExtraModifier;
				if (pCombatDetails != NULL)
				{
					pCombatDetails->iRiverAttackModifier = iExtraModifier;
				}
			}
		}

		if (!(pAttacker->isAmphib()))
		{
			if (!(pAttackedPlot->isWater()) && pAttacker->plot()->isWater())
			{
				iExtraModifier = -GC.getAMPHIB_ATTACK_MODIFIER();
				iTempModifier += iExtraModifier;
				if (pCombatDetails != NULL)
				{
					pCombatDetails->iAmphibAttackModifier = iExtraModifier;
				}
			}
		}

		// if we are attacking an unknown defender, then use the reverse of the modifier
		if (bAttackingUnknownDefender)
		{
			iModifier -= iTempModifier;
		}
		else
		{
			iModifier += iTempModifier;
		}
	}

	if (pCombatDetails != NULL)
	{
		pCombatDetails->iModifierTotal = iModifier;
		pCombatDetails->iBaseCombatStr = baseCombatStr();
	}

	if (iModifier > 0)
	{
		iCombat = (baseCombatStr() * (iModifier + 100));
	}
	else
	{
		iCombat = ((baseCombatStr() * 10000) / (100 - iModifier));
	}

	if (pCombatDetails != NULL)
	{
		pCombatDetails->iCombat = iCombat;
		pCombatDetails->iMaxCombatStr = std::max(1, iCombat);
		pCombatDetails->iCurrHitPoints = currHitPoints();
		pCombatDetails->iMaxHitPoints = maxHitPoints();
		pCombatDetails->iCurrCombatStr = ((pCombatDetails->iMaxCombatStr * pCombatDetails->iCurrHitPoints) / pCombatDetails->iMaxHitPoints);
	}

	return std::max(1, iCombat);
}


int CvUnit::currCombatStr(const CvPlot* pPlot, const CvUnit* pAttacker, CombatDetails* pCombatDetails) const
{
	return ((maxCombatStr(pPlot, pAttacker, pCombatDetails) * currHitPoints()) / maxHitPoints());
}


int CvUnit::currFirepower(const CvPlot* pPlot, const CvUnit* pAttacker) const
{
	return ((maxCombatStr(pPlot, pAttacker) + currCombatStr(pPlot, pAttacker) + 1) / 2);
}

// this nomalizes str by firepower, useful for quick odds calcs
// the effect is that a damaged unit will have an effective str lowered by firepower/maxFirepower
// doing the algebra, this means we mulitply by 1/2(1 + currHP)/maxHP = (maxHP + currHP) / (2 * maxHP)
int CvUnit::currEffectiveStr(const CvPlot* pPlot, const CvUnit* pAttacker, CombatDetails* pCombatDetails) const
{
	int currStr = currCombatStr(pPlot, pAttacker, pCombatDetails);

	currStr *= (maxHitPoints() + currHitPoints());
	currStr /= (2 * maxHitPoints());

	return currStr;
}

float CvUnit::maxCombatStrFloat(const CvPlot* pPlot, const CvUnit* pAttacker) const
{
	return (((float)(maxCombatStr(pPlot, pAttacker))) / 100.0f);
}


float CvUnit::currCombatStrFloat(const CvPlot* pPlot, const CvUnit* pAttacker) const
{
	return (((float)(currCombatStr(pPlot, pAttacker))) / 100.0f);
}

bool CvUnit::isUnarmed() const
{
	if (baseCombatStr() == 0)
	{
		return true;
	}

	if (getUnarmedCount() > 0)
	{
		return true;
	}

	return false;
}

int CvUnit::getPower() const
{
	int iPower = m_pUnitInfo->getPowerValue();
	if (getProfession() != NO_PROFESSION)
	{
		iPower += GC.getProfessionInfo(getProfession()).getPowerValue();

		if (GET_PLAYER(getOwnerINLINE()).hasContentsYieldEquipmentAmount(getProfession())) // cache CvPlayer::getYieldEquipmentAmount - Nightinggale
		{
			for (int i = 0; i < NUM_YIELD_TYPES; ++i)
			{
				YieldTypes eYield = (YieldTypes) i;
				iPower += GC.getYieldInfo(eYield).getPowerValue() * GET_PLAYER(getOwnerINLINE()).getYieldEquipmentAmount(getProfession(), eYield);
			}
		}
	}

	YieldTypes eYield = getYield();
	if (eYield != NO_YIELD)
	{
		iPower += GC.getYieldInfo(eYield).getPowerValue() * getYieldStored();
	}

	return iPower;
}

int CvUnit::getAsset() const
{
	int iAsset = m_pUnitInfo->getAssetValue();
	if (getProfession() != NO_PROFESSION)
	{
		iAsset += GC.getProfessionInfo(getProfession()).getAssetValue();
		if (GET_PLAYER(getOwnerINLINE()).hasContentsYieldEquipmentAmount(getProfession())) // cache CvPlayer::getYieldEquipmentAmount - Nightinggale
		{
			for (int i = 0; i < NUM_YIELD_TYPES; ++i)
			{
				YieldTypes eYield = (YieldTypes) i;
				iAsset += GC.getYieldInfo(eYield).getAssetValue() * GET_PLAYER(getOwnerINLINE()).getYieldEquipmentAmount(getProfession(), eYield);
			}
		}
	}
	YieldTypes eYield = getYield();
	if (eYield != NO_YIELD)
	{
		iAsset += GC.getYieldInfo(eYield).getAssetValue() * getYieldStored();
	}
	return iAsset;
}

bool CvUnit::canFight() const
{
	return (baseCombatStr() > 0);
}


bool CvUnit::canAttack() const
{
	if (!canFight())
	{
		return false;
	}

	if (isOnlyDefensive())
	{
		return false;
	}

	if (isUnarmed())
	{
		return false;
	}

	return true;
}


bool CvUnit::canDefend(const CvPlot* pPlot) const
{
	if (pPlot == NULL)
	{
		pPlot = plot();
	}

	if (!canFight())
	{
		return false;
	}

	if (getCapturingPlayer() != NO_PLAYER)
	{
		return false;
	}

	if (!pPlot->isValidDomainForAction(*this))
	{
		return false;
	}

	return true;
}


bool CvUnit::canSiege(TeamTypes eTeam) const
{
	if (!canDefend())
	{
		return false;
	}

	if (!isEnemy(eTeam))
	{
		return false;
	}

	if (!isNeverInvisible())
	{
		return false;
	}

	return true;
}

bool CvUnit::isAutomated() const
{
	return getGroup()->isAutomated();
}

bool CvUnit::isWaiting() const
{
	return getGroup()->isWaiting();
}

bool CvUnit::isFortifyable() const
{
	if (!canFight())
	{
		return false;
	}

	if (noDefensiveBonus())
	{
		return false;
	}

	if (!isOnMap())
	{
		return false;
	}

	if (getDomainType() == DOMAIN_SEA)
	{
		return false;
	}

	return true;
}


int CvUnit::fortifyModifier() const
{
	if (!isFortifyable())
	{
		return 0;
	}

	return (getFortifyTurns() * GC.getFORTIFY_MODIFIER_PER_TURN());
}


int CvUnit::experienceNeeded() const
{
	if (GC.getUSE_GET_EXPERIENCE_NEEDED_CALLBACK()) // K-Mod. I've writen C to replace the python callback.
	{

		// Use python to determine pillage amounts...
		int iExperienceNeeded;
		long lExperienceNeeded;

		lExperienceNeeded = 0;
		iExperienceNeeded = 0;

		CyArgsList argsList;
		argsList.add(getLevel());	// pass in the units level
		argsList.add(getOwner());	// pass in the units

		gDLL->getPythonIFace()->callFunction(PYGameModule, "getExperienceNeeded", argsList.makeFunctionArgs(), &lExperienceNeeded);

		iExperienceNeeded = (int)lExperienceNeeded;

		return iExperienceNeeded;
	}
	// K-Mod. C version of the original python code.
	// Note: python rounds towards negative infinity, but C++ rounds towards 0.
	// So the code needs to be slightly different to achieve the same effect.
	int iExperienceNeeded = getLevel() * getLevel() + 1;

	int iModifier = GET_PLAYER(getOwnerINLINE()).getLevelExperienceModifier();
	if (iModifier != 0)
		iExperienceNeeded = (iExperienceNeeded * (100 + iModifier) + 99) / 100;

	return iExperienceNeeded;
	// K-Mod end
}


int CvUnit::attackXPValue() const
{
	return m_pUnitInfo->getXPValueAttack();
}

int CvUnit::defenseXPValue() const
{
	return m_pUnitInfo->getXPValueDefense();
}

int CvUnit::maxXPValue() const
{
	int iMaxValue = MAX_INT;

	// WTP, XP cap to prevent farming XP from Animals
	if (getUnitInfo().isAnimal())
	{
		iMaxValue = std::min(iMaxValue, GC.getDefineINT("ANIMAL_MAX_XP_VALUE"));
	}

	// WTP, XP cap to prevent farming XP from Runaways
	if (getUnitInfo().LbD_canEscape())
	{
		// WTP, ray, modified XP cap code of devolution in case of Runaways by additional check for Profession Combat Change
		// it can only be a Runaway if CombatChange of the Profession is 1 or maybe 0 if config changes
		ProfessionTypes eUnitProfession = getProfession();
		if (eUnitProfession != NO_PROFESSION && GC.getProfessionInfo(eUnitProfession).getCombatChange() <= 1)
		{
			iMaxValue = std::min(iMaxValue, GC.getDefineINT("ESCAPE_MAX_XP_VALUE"));
		}
	}

	return iMaxValue;
}

bool CvUnit::isRanged() const
{
	CvUnitInfo * pkUnitInfo = &getUnitInfo();
	for (int i = 0; i < pkUnitInfo->getGroupDefinitions(getProfession()); i++ )
	{
		if ( !getArtInfo(i)->getActAsRanged() )
		{
			return false;
		}
	}
	return true;
}

bool CvUnit::alwaysInvisible() const
{
	if (!isOnMap())
	{
		return true;
	}

	return m_pUnitInfo->isInvisible();
}

bool CvUnit::noDefensiveBonus() const
{
	ProfessionTypes eProfession = getProfession();
	if (eProfession != NO_PROFESSION && GC.getProfessionInfo(eProfession).isNoDefensiveBonus())
	{
		return true;
	}

	if (m_pUnitInfo->isNoDefensiveBonus())
	{
		return true;
	}

	return false;
}

bool CvUnit::canMoveImpassable() const
{
	return m_pUnitInfo->isCanMoveImpassable();
}

bool CvUnit::flatMovementCost() const
{
	return m_pUnitInfo->isFlatMovementCost();
}


bool CvUnit::ignoreTerrainCost() const
{
	return m_pUnitInfo->isIgnoreTerrainCost();
}


bool CvUnit::isNeverInvisible() const
{
	return (!alwaysInvisible() && (getInvisibleType() == NO_INVISIBLE));
}


bool CvUnit::isInvisible(TeamTypes eTeam, bool bDebug, bool bCheckCargo) const
{
	if (!isOnMap())
	{
		return true;
	}

	if (bDebug && GC.getGameINLINE().isDebugMode())
	{
		return false;
	}

	if (getTeam() == eTeam)
	{
		return false;
	}

	if (alwaysInvisible())
	{
		return true;
	}

	if (bCheckCargo && isCargo())
	{
		return true;
	}

	if (getInvisibleType() == NO_INVISIBLE)
	{
		return false;
	}

	// R&R, ray, Rangers
	if (getInvisibleType() == INVISIBLE_RANGER)
	{
		if (plot()->getFeatureType() == NO_FEATURE)
		{
			return false;
		}
	}
	// R&R, ray, Rangers, END

	return !(plot()->isInvisibleVisible(eTeam, getInvisibleType()));
}


int CvUnit::withdrawalProbability() const
{
	return std::max(0, (m_pUnitInfo->getWithdrawalProbability() + getExtraWithdrawal()));
}

int CvUnit::getEvasionProbability(const CvUnit& kAttacker) const
{
	CvCity* pEvasionCity = getEvasionCity();
	if (pEvasionCity == NULL)
	{
		return 0;
	}

	return 100 * maxMoves() / std::max(1, maxMoves() + kAttacker.maxMoves());
}

CvCity* CvUnit::getEvasionCity() const
{
	if (!isOnMap())
	{
		return NULL;
	}

	CvCity* pBestCity = NULL;
	int iBestDistance = MAX_INT;
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayer);
		if (kPlayer.isAlive() && kPlayer.getTeam() == getTeam())
		{
			int iLoop;
			for (CvCity* pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				if (pLoopCity->getArea() == getArea() || pLoopCity->plot()->isAdjacentToArea(getArea()))
				{
					if (pLoopCity->plot()->isFriendlyCity(*this, false))
					{
						for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); ++iBuildingClass)
						{
							if (m_pUnitInfo->isEvasionBuilding(iBuildingClass))
							{
								if (pLoopCity->isHasBuildingClass((BuildingClassTypes) iBuildingClass))
								{
									int iDistance = ::plotDistance(getX_INLINE(), getY_INLINE(), pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE());
									if (iDistance < iBestDistance)
									{
										iBestDistance = iDistance;
										pBestCity = pLoopCity;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return pBestCity;
}

int CvUnit::cityAttackModifier() const
{
	return (m_pUnitInfo->getCityAttackModifier() + getExtraCityAttackPercent());
}


int CvUnit::cityDefenseModifier() const
{
	return (m_pUnitInfo->getCityDefenseModifier() + getExtraCityDefensePercent());
}

int CvUnit::hillsAttackModifier() const
{
	return (m_pUnitInfo->getHillsAttackModifier() + getExtraHillsAttackPercent());
}


int CvUnit::hillsDefenseModifier() const
{
	return (m_pUnitInfo->getHillsDefenseModifier() + getExtraHillsDefensePercent());
}

int CvUnit::DomesticBonusModifier() const
{
	return (m_pUnitInfo->getDomesticBonusModifier() + getExtraDomesticBonusPercent());
}

int CvUnit::terrainAttackModifier(TerrainTypes eTerrain) const
{
	FAssertMsg(eTerrain >= 0, "eTerrain is expected to be non-negative (invalid Index)");
	FAssertMsg(eTerrain < GC.getNumTerrainInfos(), "eTerrain is expected to be within maximum bounds (invalid Index)");
	return (m_pUnitInfo->getTerrainAttackModifier(eTerrain) + getExtraTerrainAttackPercent(eTerrain));
}


int CvUnit::terrainDefenseModifier(TerrainTypes eTerrain) const
{
	FAssertMsg(eTerrain >= 0, "eTerrain is expected to be non-negative (invalid Index)");
	FAssertMsg(eTerrain < GC.getNumTerrainInfos(), "eTerrain is expected to be within maximum bounds (invalid Index)");
	return (m_pUnitInfo->getTerrainDefenseModifier(eTerrain) + getExtraTerrainDefensePercent(eTerrain));
}


int CvUnit::featureAttackModifier(FeatureTypes eFeature) const
{
	FAssertMsg(eFeature >= 0, "eFeature is expected to be non-negative (invalid Index)");
	FAssertMsg(eFeature < GC.getNumFeatureInfos(), "eFeature is expected to be within maximum bounds (invalid Index)");
	return (m_pUnitInfo->getFeatureAttackModifier(eFeature) + getExtraFeatureAttackPercent(eFeature));
}

int CvUnit::featureDefenseModifier(FeatureTypes eFeature) const
{
	FAssertMsg(eFeature >= 0, "eFeature is expected to be non-negative (invalid Index)");
	FAssertMsg(eFeature < GC.getNumFeatureInfos(), "eFeature is expected to be within maximum bounds (invalid Index)");
	return (m_pUnitInfo->getFeatureDefenseModifier(eFeature) + getExtraFeatureDefensePercent(eFeature));
}

int CvUnit::unitClassAttackModifier(UnitClassTypes eUnitClass) const
{
	FAssertMsg(eUnitClass >= 0, "eUnitClass is expected to be non-negative (invalid Index)");
	FAssertMsg(eUnitClass < GC.getNumUnitClassInfos(), "eUnitClass is expected to be within maximum bounds (invalid Index)");
	return m_pUnitInfo->getUnitClassAttackModifier(eUnitClass) + getExtraUnitClassAttackModifier(eUnitClass);
}


int CvUnit::unitClassDefenseModifier(UnitClassTypes eUnitClass) const
{
	FAssertMsg(eUnitClass >= 0, "eUnitClass is expected to be non-negative (invalid Index)");
	FAssertMsg(eUnitClass < GC.getNumUnitClassInfos(), "eUnitClass is expected to be within maximum bounds (invalid Index)");
	return m_pUnitInfo->getUnitClassDefenseModifier(eUnitClass) + getExtraUnitClassDefenseModifier(eUnitClass);
}


int CvUnit::unitCombatModifier(UnitCombatTypes eUnitCombat) const
{
	FAssertMsg(eUnitCombat >= 0, "eUnitCombat is expected to be non-negative (invalid Index)");
	FAssertMsg(eUnitCombat < GC.getNumUnitCombatInfos(), "eUnitCombat is expected to be within maximum bounds (invalid Index)");
	return (m_pUnitInfo->getUnitCombatModifier(eUnitCombat) + getExtraUnitCombatModifier(eUnitCombat));
}


int CvUnit::domainModifier(DomainTypes eDomain) const
{
	FAssertMsg(eDomain >= 0, "eDomain is expected to be non-negative (invalid Index)");
	FAssertMsg(eDomain < NUM_DOMAIN_TYPES, "eDomain is expected to be within maximum bounds (invalid Index)");
	return (m_pUnitInfo->getDomainModifier(eDomain) + getExtraDomainModifier(eDomain));
}

int CvUnit::rebelModifier(PlayerTypes eOtherPlayer) const
{
	if (GET_PLAYER(getOwnerINLINE()).getParent() != eOtherPlayer)
	{
		return 0;
	}

	int iModifier = std::max(0, GET_TEAM(getTeam()).getRebelPercent() - GC.getDefineINT("REBEL_PERCENT_FOR_REVOLUTION"));

	iModifier *= GET_PLAYER(getOwnerINLINE()).getRebelCombatPercent();
	iModifier /= 100;

	if (!isHuman())
	{
		iModifier += GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIKingCombatModifier();
	}

	return iModifier;
}


int CvUnit::bombardRate() const
{
	return (m_pUnitInfo->getBombardRate() + getExtraBombardRate());
}


SpecialUnitTypes CvUnit::specialCargo() const
{
	return ((SpecialUnitTypes)(m_pUnitInfo->getSpecialCargo()));
}


DomainTypes CvUnit::domainCargo() const
{
	return ((DomainTypes)(m_pUnitInfo->getDomainCargo()));
}


int CvUnit::cargoSpace() const
{
	return m_iCargoCapacity;
}

void CvUnit::changeCargoSpace(int iChange)
{
	if (iChange != 0)
	{
		m_iCargoCapacity += iChange;
		FAssert(m_iCargoCapacity >= 0);
		setInfoBarDirty(true);
	}
}

bool CvUnit::isFull() const
{
	return (getCargo() >= cargoSpace());
}


int CvUnit::cargoSpaceAvailable(SpecialUnitTypes eSpecialCargo, DomainTypes eDomainCargo) const
{
	if (specialCargo() != NO_SPECIALUNIT)
	{
		if (specialCargo() != eSpecialCargo)
		{
			return 0;
		}
	}

	if (domainCargo() != NO_DOMAIN)
	{
		if (domainCargo() != eDomainCargo)
		{
			return 0;
		}
	}

	return std::max(0, (cargoSpace() - getCargo()));
}


bool CvUnit::hasCargo() const
{
	return (getCargo() > 0);
}

bool CvUnit::hasAnyUnitInCargo() const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pPlot;

	pPlot = plot();

	pUnitNode = pPlot->headUnitNode();
	
	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		
		if (pLoopUnit->getTransportUnit() == this)
		{
			if (!pLoopUnit->isGoods())
			{
				return true;
			}
		}

		pUnitNode = pPlot->nextUnitNode(pUnitNode);
	}
	
	return false;
}

bool CvUnit::canCargoAllMove() const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pPlot;

	pPlot = plot();

	pUnitNode = pPlot->headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if (pLoopUnit->getTransportUnit() == this)
		{
			if (pLoopUnit->getDomainType() == DOMAIN_LAND)
			{
				if (!(pLoopUnit->canMove()))
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool CvUnit::canCargoEnterArea(PlayerTypes ePlayer, const CvArea* pArea, bool bIgnoreRightOfPassage) const
{
	CvPlot* pPlot = plot();

	CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if (pLoopUnit->getTransportUnit() == this)
		{
			if (!pLoopUnit->canEnterArea(ePlayer, pArea, bIgnoreRightOfPassage))
			{
				return false;
			}
		}
	}

	return true;
}

int CvUnit::getUnitAICargo(UnitAITypes eUnitAI) const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pPlot;
	int iCount;

	iCount = 0;

	pPlot = plot();

	pUnitNode = pPlot->headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if (pLoopUnit->getTransportUnit() == this)
		{
			if (pLoopUnit->AI_getUnitAIType() == eUnitAI)
			{
				iCount++;
			}
		}
	}

	return iCount;
}

bool CvUnit::canAssignTradeRoute(int iRouteID, bool bReusePath) const
{
	PROFILE_FUNC();

	// TAC - Trade Routes Advisor - koma13 - START
	//if (cargoSpace() < 1 || GET_PLAYER(getOwnerINLINE()).getNumTradeRoutes() < 1)
	if (cargoSpace() < 1)
	// TAC - Trade Routes Advisor - koma13 - END
	{
		return false;
	}

	CvSelectionGroup* pGroup = getGroup();
	if (pGroup == NULL)
	{
		return false;
	}

	// TAC - Trade Routes Advisor - koma13 - START
	/*
	if (iRouteID == -1)
	{
		return true;
	}
	*/

	CLinkList<IDInfo> listCargo;
	pGroup->buildCargoUnitList(listCargo);
	CLLNode<IDInfo>* pUnitNode = listCargo.head();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = listCargo.next(pUnitNode);

		if (pLoopUnit->getYield() == NO_YIELD)
		{
			return false;
		}
	}

	if (iRouteID == -1)
	{
		return true;
	}
	// TAC - Trade Routes Advisor - koma13 - END

	PlayerTypes ePlayer = getOwnerINLINE();
	FAssert(ePlayer != NO_PLAYER);
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	CvTradeRoute* pTradeRoute = kPlayer.getTradeRoute(iRouteID);
	if (pTradeRoute == NULL)
	{
		return false;
	}

	if (pTradeRoute->getYield() == NO_YIELD)
	{
		return false;
	}

	if (pTradeRoute->getDestinationCity() == IDInfo(ePlayer, CvTradeRoute::EUROPE_CITY_ID))
	{
		// Erik: Coastal transports cannot trade with Europe
		if (canCrossCoastOnly())
		{
			return false;
		}

		if (getDomainType() != DOMAIN_SEA)
		{
			return false;
		}

		if (!kPlayer.isYieldEuropeTradable(pTradeRoute->getYield()))
		{
			return false;
		}
	}

	CvCity* pSource = ::getCity(pTradeRoute->getSourceCity());
	// TAC - Trade Routes Advisor - koma13 - START
	//if (pSource == NULL || !generatePath(pSource->plot(), 0, bReusePath))
	if (pSource == NULL || !generatePath(pSource->plot(), (isIgnoreDanger() ? MOVE_IGNORE_DANGER : MOVE_NO_ENEMY_TERRITORY), bReusePath))
	// TAC - Trade Routes Advisor - koma13 - END
	{
		return false;
	}

	CvCity* pDestination = ::getCity(pTradeRoute->getDestinationCity());
	// TAC - Trade Routes Advisor - koma13 - START
	//if (pDestination != NULL && !generatePath(pDestination->plot(), 0, bReusePath))
	if (pDestination != NULL && !generatePath(pDestination->plot(), (isIgnoreDanger() ? MOVE_IGNORE_DANGER : MOVE_NO_ENEMY_TERRITORY), bReusePath))
	// TAC - Trade Routes Advisor - koma13 - END
	{
		return false;
	}

	return true;
}


int CvUnit::getID() const
{
	return m_iID;
}

// TAC - LbD - Ray - START
int CvUnit::getLbDrounds() const
{
	return m_LbDrounds;
}

void CvUnit::setLbDrounds(int newRounds)
{
	m_LbDrounds = newRounds;
}

ProfessionTypes CvUnit::getLastLbDProfession() const
{
	return m_lastProfession;
}

void CvUnit::setLastLbDProfession(ProfessionTypes eProfession)
{
	m_lastProfession = eProfession;
}
// TAC - LbD - Ray - END


int CvUnit::getIndex() const
{
	return (getID() & FLTA_INDEX_MASK);
}


IDInfo CvUnit::getIDInfo() const
{
	IDInfo unit(getOwnerINLINE(), getID());
	return unit;
}


void CvUnit::setID(int iID)
{
	m_iID = iID;
}


int CvUnit::getGroupID() const
{
	return m_iGroupID;
}


bool CvUnit::isInGroup() const
{
	return(getGroupID() != FFreeList::INVALID_INDEX);
}


bool CvUnit::isGroupHead() const // XXX is this used???
{
	return (getGroup()->getHeadUnit() == this);
}


CvSelectionGroup* CvUnit::getGroup() const
{
	return GET_PLAYER(getOwnerINLINE()).getSelectionGroup(getGroupID());
}


bool CvUnit::canJoinGroup(const CvPlot* pPlot, CvSelectionGroup* pSelectionGroup) const
{
	CvUnit* pHeadUnit;

	// do not allow someone to join a group that is about to be split apart
	// this prevents a case of a never-ending turn
	if (pSelectionGroup->AI_isForceSeparate())
	{
		return false;
	}

	if (pSelectionGroup->getOwnerINLINE() == NO_PLAYER)
	{
		pHeadUnit = pSelectionGroup->getHeadUnit();

		if (pHeadUnit != NULL)
		{
			if (pHeadUnit->getOwnerINLINE() != getOwnerINLINE())
			{
				return false;
			}
		}
	}
	else
	{
		if (pSelectionGroup->getOwnerINLINE() != getOwnerINLINE())
		{
			return false;
		}
	}

	if (pSelectionGroup->getNumUnits() > 0)
	{
		if (!(pSelectionGroup->atPlot(pPlot)))
		{
			return false;
		}

		if (pSelectionGroup->getDomainType() != getDomainType())
		{
			return false;
		}
	}

	return true;
}


void CvUnit::joinGroup(CvSelectionGroup* pSelectionGroup, bool bRemoveSelected, bool bRejoin)
{
	CvSelectionGroup* pOldSelectionGroup;
	CvSelectionGroup* pNewSelectionGroup;
	CvPlot* pPlot;

	pOldSelectionGroup = GET_PLAYER(getOwnerINLINE()).getSelectionGroup(getGroupID());

	if ((pSelectionGroup != pOldSelectionGroup) || (pOldSelectionGroup == NULL))
	{
		pPlot = plot();

		if (pSelectionGroup != NULL)
		{
			pNewSelectionGroup = pSelectionGroup;
		}
		else
		{
			if (bRejoin)
			{
				pNewSelectionGroup = GET_PLAYER(getOwnerINLINE()).addSelectionGroup();
				pNewSelectionGroup->init(pNewSelectionGroup->getID(), getOwnerINLINE());
			}
			else
			{
				pNewSelectionGroup = NULL;
			}
		}

		if ((pNewSelectionGroup == NULL) || canJoinGroup(plot(), pNewSelectionGroup))
		{
			if (pOldSelectionGroup != NULL)
			{
				bool bWasHead = false;
				if (!isHuman())
				{
					if (pOldSelectionGroup->getNumUnits() > 1)
					{
						if (pOldSelectionGroup->getHeadUnit() == this)
						{
							bWasHead = true;
						}
					}
				}

				pOldSelectionGroup->removeUnit(this);

				// if we were the head, if the head unitAI changed, then force the group to separate (non-humans)
				if (bWasHead)
				{
					FAssert(pOldSelectionGroup->getHeadUnit() != NULL);
					if (pOldSelectionGroup->getHeadUnit()->AI_getUnitAIType() != AI_getUnitAIType())
					{
						pOldSelectionGroup->AI_makeForceSeparate();
					}
				}
			}

			if ((pNewSelectionGroup != NULL) && pNewSelectionGroup->addUnit(this, !isOnMap()))
			{
				m_iGroupID = pNewSelectionGroup->getID();
			}
			else
			{
				m_iGroupID = FFreeList::INVALID_INDEX;
			}

			if (getGroup() != NULL)
			{
				if (getGroup()->getNumUnits() > 1)
				{
					getGroup()->setActivityType(ACTIVITY_AWAKE);
				}
				else
				{
					GET_PLAYER(getOwnerINLINE()).updateGroupCycle(this);
				}
			}

			if (getTeam() == GC.getGameINLINE().getActiveTeam())
			{
				if (pPlot != NULL)
				{
					pPlot->setFlagDirty(true);
				}
			}

			if (pPlot == gDLL->getInterfaceIFace()->getSelectionPlot())
			{
				gDLL->getInterfaceIFace()->setDirty(PlotListButtons_DIRTY_BIT, true);
			}
		}

		if (bRemoveSelected)
		{
			if (IsSelected())
			{
				gDLL->getInterfaceIFace()->removeFromSelectionList(this);
			}
		}
	}
}


int CvUnit::getHotKeyNumber()
{
	return m_iHotKeyNumber;
}


void CvUnit::setHotKeyNumber(int iNewValue)
{
	CvUnit* pLoopUnit;
	int iLoop;

	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (getHotKeyNumber() != iNewValue)
	{
		if (iNewValue != -1)
		{
			for(pLoopUnit = GET_PLAYER(getOwnerINLINE()).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(getOwnerINLINE()).nextUnit(&iLoop))
			{
				if (pLoopUnit->getHotKeyNumber() == iNewValue)
				{
					pLoopUnit->setHotKeyNumber(-1);
				}
			}
		}

		m_iHotKeyNumber = iNewValue;

		if (IsSelected())
		{
			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		}
	}
}


int CvUnit::getX() const
{
	return m_iX;
}


int CvUnit::getY() const
{
	return m_iY;
}


void CvUnit::setXY(int iX, int iY, bool bGroup, bool bUpdate, bool bShow, bool bCheckPlotVisible)
{
	CLLNode<IDInfo>* pUnitNode;
	CvCity* pOldCity;
	CvCity* pNewCity;
	CvCity* pWorkingCity;
	CvUnit* pTransportUnit;
	CvUnit* pLoopUnit;
	CvPlot* pOldPlot;
	CvPlot* pNewPlot;
	CvPlot* pLoopPlot;
	CLinkList<IDInfo> oldUnits;
	ActivityTypes eOldActivityType;
	int iI;

	// OOS!! Temporary for Out-of-Sync madness debugging...
	if (GC.getLogging())
	{
		if (gDLL->getChtLvl() > 0)
		{
			char szOut[1024];
			sprintf(szOut, "Player %d Unit %d (%S's %S) moving from %d:%d to %d:%d\n", getOwnerINLINE(), getID(), GET_PLAYER(getOwnerINLINE()).getNameKey(), getName().GetCString(), getX_INLINE(), getY_INLINE(), iX, iY);
			gDLL->messageControlLog(szOut);
		}
	}

	FAssert(!at(iX, iY) || (iX == INVALID_PLOT_COORD) || (iY == INVALID_PLOT_COORD));
	FAssert(!isFighting());
	FAssert((iX == INVALID_PLOT_COORD) || (GC.getMapINLINE().plotINLINE(iX, iY)->getX_INLINE() == iX));
	FAssert((iY == INVALID_PLOT_COORD) || (GC.getMapINLINE().plotINLINE(iX, iY)->getY_INLINE() == iY));

	if (getGroup() != NULL)
	{
		eOldActivityType = getGroup()->getActivityType();
	}
	else
	{
		eOldActivityType = NO_ACTIVITY;
	}

	if (!bGroup)
	{
		joinGroup(NULL, true);
	}

	pNewPlot = GC.getMapINLINE().plotINLINE(iX, iY);

	if (pNewPlot != NULL)
	{
		pTransportUnit = getTransportUnit();

		if (pTransportUnit != NULL)
		{
			if (!(pTransportUnit->atPlot(pNewPlot)))
			{
				setTransportUnit(NULL);
			}
		}

		if (canFight() && isOnMap())
		{
			oldUnits.clear();

			pUnitNode = pNewPlot->headUnitNode();

			while (pUnitNode != NULL)
			{
				oldUnits.insertAtEnd(pUnitNode->m_data);
				pUnitNode = pNewPlot->nextUnitNode(pUnitNode);
			}

			pUnitNode = oldUnits.head();

			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = oldUnits.next(pUnitNode);

				if (pLoopUnit != NULL && pLoopUnit->isOnMap())
				{
					if (isEnemy(pLoopUnit->getTeam(), pNewPlot) || pLoopUnit->isEnemy(getTeam()))
					{
						if (!pLoopUnit->canCoexistWithEnemyUnit(getTeam()))
						{
							if (NO_UNITCLASS == pLoopUnit->getUnitInfo().getUnitCaptureClassType() && pLoopUnit->canDefend(pNewPlot))
							{
								pLoopUnit->jumpToNearestValidPlot(); // can kill unit
							}
							else
							{
								if (!m_pUnitInfo->isHiddenNationality() && !pLoopUnit->getUnitInfo().isHiddenNationality())
								{
									GET_TEAM(getTeam()).AI_changeWarSuccess(pLoopUnit->getTeam(), GC.getDefineINT("WAR_SUCCESS_UNIT_CAPTURING"));
								}

								if (!isNoUnitCapture())
								{
									pLoopUnit->setCapturingPlayer(getOwnerINLINE());
								}

								pLoopUnit->kill(false, this);
							}
						}
					}
				}
			}
		}
	}

	pOldPlot = plot();

	if (pOldPlot != NULL)
	{
		pOldPlot->removeUnit(this, bUpdate);

		pOldPlot->changeAdjacentSight(getTeam(), visibilityRange(), false, this);

		pOldPlot->area()->changeUnitsPerPlayer(getOwnerINLINE(), -1);
		pOldPlot->area()->changePower(getOwnerINLINE(), -getPower());

		if (AI_getUnitAIType() != NO_UNITAI)
		{
			pOldPlot->area()->changeNumAIUnits(getOwnerINLINE(), AI_getUnitAIType(), -1);
		}

		setLastMoveTurn(GC.getGameINLINE().getTurnSlice());

		pOldCity = pOldPlot->getPlotCity();

		pWorkingCity = pOldPlot->getWorkingCity();

		if (pWorkingCity != NULL)
		{
			if (canSiege(pWorkingCity->getTeam()))
			{
				pWorkingCity->AI_setAssignWorkDirty(true);
			}
		}

		if (pOldPlot->isWater())
		{
			for (iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				pLoopPlot = plotDirection(pOldPlot->getX_INLINE(), pOldPlot->getY_INLINE(), ((DirectionTypes)iI));

				if (pLoopPlot != NULL)
				{
					if (pLoopPlot->isWater())
					{
						pWorkingCity = pLoopPlot->getWorkingCity();

						if (pWorkingCity != NULL)
						{
							if (canSiege(pWorkingCity->getTeam()))
							{
								pWorkingCity->AI_setAssignWorkDirty(true);
							}
						}
					}
				}
			}
		}

		if (pOldPlot->isActiveVisible(true))
		{
			pOldPlot->updateMinimapColor();
		}

		if (pOldPlot == gDLL->getInterfaceIFace()->getSelectionPlot())
		{
			gDLL->getInterfaceIFace()->verifyPlotListColumn();

			gDLL->getInterfaceIFace()->setDirty(PlotListButtons_DIRTY_BIT, true);
		}
	}

	if (pNewPlot != NULL)
	{
		m_iX = pNewPlot->getX_INLINE();
		m_iY = pNewPlot->getY_INLINE();
	}
	else
	{
		m_iX = INVALID_PLOT_COORD;
		m_iY = INVALID_PLOT_COORD;
		AI_setMovePriority(0);
	}

	FAssertMsg(plot() == pNewPlot, "plot is expected to equal pNewPlot");

	if (pNewPlot != NULL)
	{
		pNewCity = pNewPlot->getPlotCity();

		if (pNewCity != NULL)
		{
			if (isEnemy(pNewCity->getTeam()) && !canCoexistWithEnemyUnit(pNewCity->getTeam()) && canFight())
			{
				GET_TEAM(getTeam()).AI_changeWarSuccess(pNewCity->getTeam(), GC.getDefineINT("WAR_SUCCESS_CITY_CAPTURING"));
				PlayerTypes eNewOwner = GET_PLAYER(getOwnerINLINE()).pickConqueredCityOwner(*pNewCity);

				if (NO_PLAYER != eNewOwner)
				{	
					//ray18
					// buy Land from Natives after acquire city
					bool checkAcuqireNatives = doAcquireCheckNatives();
					//Ende ray18
					GET_PLAYER(eNewOwner).acquireCity(pNewCity, true, false); // will delete the pointer

					pNewCity = NULL;
				}
			}
		}

		// Super Forts begin *culture* *text*
		ImprovementTypes eImprovement = pNewPlot->getImprovementType();
		if(eImprovement != NO_IMPROVEMENT)
		{
			if(GC.getImprovementInfo(eImprovement).isActsAsCity() && !isNoUnitCapture())
			{
				if(pNewPlot->getOwner() != NO_PLAYER)
				{
					if(isEnemy(pNewPlot->getTeam()) && !canCoexistWithEnemyUnit(pNewPlot->getTeam()) && canFight())
					{
						CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_CITY_CAPTURED_BY", GC.getImprovementInfo(eImprovement).getText(), GET_PLAYER(getOwnerINLINE()).getCivilizationDescriptionKey());
						gDLL->getInterfaceIFace()->addMessage(pNewPlot->getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CITYCAPTURED", MESSAGE_TYPE_MAJOR_EVENT, GC.getImprovementInfo(eImprovement).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pNewPlot->getX_INLINE(), pNewPlot->getY_INLINE(), true, true);
						pNewPlot->setOwner(getOwnerINLINE(),true);
					}
				}
				else
				{
					pNewPlot->setOwner(getOwnerINLINE(),true);
				}
			}
		}
		// Super Forts end

		//update facing direction
		if(pOldPlot != NULL)
		{
			DirectionTypes newDirection = estimateDirection(pOldPlot, pNewPlot);
			if(newDirection != NO_DIRECTION)
				m_eFacingDirection = newDirection;
		}

		//update cargo mission animations
		if (isCargo())
		{
			if (eOldActivityType != ACTIVITY_MISSION)
			{
				getGroup()->setActivityType(eOldActivityType);
			}
		}

		setFortifyTurns(0);

		pNewPlot->changeAdjacentSight(getTeam(), visibilityRange(), true, this); // needs to be here so that the square is considered visible when we move into it...

		pNewPlot->addUnit(this, bUpdate);

		pNewPlot->area()->changeUnitsPerPlayer(getOwnerINLINE(), 1);
		pNewPlot->area()->changePower(getOwnerINLINE(), getPower());

		if (AI_getUnitAIType() != NO_UNITAI)
		{
			pNewPlot->area()->changeNumAIUnits(getOwnerINLINE(), AI_getUnitAIType(), 1);
		}

		if (shouldLoadOnMove(pNewPlot))
		{
			load(false);
		}

		for (int iDX = -1; iDX <= 1; ++iDX)
		{
			for (int iDY = -1; iDY <= 1; ++iDY)
			{
				CvPlot* pLoopPlot = ::plotXY(getX_INLINE(), getY_INLINE(), iDX, iDY);
				if (pLoopPlot != NULL)
				{
					for (iI = 0; iI < MAX_TEAMS; iI++)
					{
						TeamTypes eLoopTeam = (TeamTypes) iI;
						if (GET_TEAM(eLoopTeam).isAlive())
						{
							if (!isInvisible(eLoopTeam, false) && getVisualOwner(eLoopTeam) == getOwnerINLINE())
							{
								if (pLoopPlot->plotCount(PUF_isVisualTeam, eLoopTeam, getTeam(), NO_PLAYER, eLoopTeam, PUF_isVisible, getOwnerINLINE(), -1) > 0)
								{
									GET_TEAM(eLoopTeam).meet(getTeam(), true);
								}
							}
						}
					}

					if (pLoopPlot->isOwned() && getVisualOwner(pLoopPlot->getTeam()) == getOwnerINLINE())
					{
						if (pLoopPlot->isCity() || !GET_PLAYER(pLoopPlot->getOwnerINLINE()).isAlwaysOpenBorders())
						{
							GET_TEAM(pLoopPlot->getTeam()).meet(getTeam(), true);
						}
					}
				}
			}
		}

		pNewCity = pNewPlot->getPlotCity();

		pWorkingCity = pNewPlot->getWorkingCity();

		if (pWorkingCity != NULL)
		{
			if (canSiege(pWorkingCity->getTeam()))
			{
				pWorkingCity->verifyWorkingPlot(pWorkingCity->getCityPlotIndex(pNewPlot));
			}
		}

		/* original bts code
		if (pNewPlot->isWater())
		{
			for (iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				pLoopPlot = plotDirection(pNewPlot->getX_INLINE(), pNewPlot->getY_INLINE(), ((DirectionTypes)iI));

				if (pLoopPlot != NULL)
				{
					if (pLoopPlot->isWater())
					{
						pWorkingCity = pLoopPlot->getWorkingCity();

						if (pWorkingCity != NULL)
						{
							if (canSiege(pWorkingCity->getTeam()))
							{
								pWorkingCity->verifyWorkingPlot(pWorkingCity->getCityPlotIndex(pLoopPlot));
							}
						}
					}
				}
			}
		}
		} */ // disabled by K-Mod. The game mechanics that this was meant to handle are no longer used. (Nothing to do with K-Mod.)

		if (pNewPlot->isActiveVisible(true))
		{
			pNewPlot->updateMinimapColor();
		}

		if (GC.IsGraphicsInitialized())
		{
			//override bShow if check plot visible
			if (bCheckPlotVisible)
			{
				if (!pNewPlot->isActiveVisible(true) && ((pOldPlot == NULL) || !pOldPlot->isActiveVisible(true)))
				{
					bShow = false;
				}
			}

			if (bShow)
			{
				QueueMove(pNewPlot);
			}
			else
			{
				SetPosition(pNewPlot);
			}
		}

		if (pNewPlot == gDLL->getInterfaceIFace()->getSelectionPlot())
		{
			gDLL->getInterfaceIFace()->verifyPlotListColumn();

			gDLL->getInterfaceIFace()->setDirty(PlotListButtons_DIRTY_BIT, true);
		}
	}

	if (pOldPlot != NULL)
	{
		if (hasCargo())
		{
			pUnitNode = pOldPlot->headUnitNode();

			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = pOldPlot->nextUnitNode(pUnitNode);

				if (pLoopUnit->getTransportUnit() == this)
				{
					pLoopUnit->setXY(iX, iY, bGroup, bUpdate);
					if (pLoopUnit->getYield() != NO_YIELD)
					{
						pNewPlot->addCrumbs(10);
					}
				}
			}
		}
	}

	FAssert(pOldPlot != pNewPlot || pNewPlot == NULL);
	GET_PLAYER(getOwnerINLINE()).updateGroupCycle(this);

	setInfoBarDirty(true);

	if (IsSelected())
	{
		gDLL->getInterfaceIFace()->setDirty(ColoredPlots_DIRTY_BIT, true);
	}

	//update glow
	gDLL->getEntityIFace()->updateEnemyGlow(getUnitEntity());

	// report event to Python, along with some other key state
	gDLL->getEventReporterIFace()->unitSetXY(pNewPlot, this);

	if (pNewPlot != NULL)
	{
		if (pNewPlot->isGoody(getTeam()))
		{
			for (int i = 0; i < GC.getNumFatherPointInfos(); ++i)
			{
				FatherPointTypes ePointType = (FatherPointTypes) i;
				GET_PLAYER(getOwnerINLINE()).changeFatherPoints(ePointType, GC.getFatherPointInfo(ePointType).getGoodyPoints() * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getFatherPercent() / 100);
			}

			 
			if (pTransportUnit == NULL || !pNewPlot->isWater())//R&R, ray, transported units should not trigger goodies, so transports themself will
				GET_PLAYER(getOwnerINLINE()).doGoody(pNewPlot, this);
		}
	}
	// PatchMod: Achievements START
	GET_PLAYER(getOwnerINLINE()).doAchievements(true);
	// PatchMod: Achievements END
}


bool CvUnit::at(int iX, int iY) const
{
	return((getX_INLINE() == iX) && (getY_INLINE() == iY));
}


bool CvUnit::atPlot(const CvPlot* pPlot) const
{
	return (plot() == pPlot);
}


CvPlot* CvUnit::plot() const
{
	if((getX_INLINE() == INVALID_PLOT_COORD) || (getY_INLINE() == INVALID_PLOT_COORD))
	{
		CvCity *pCity = GET_PLAYER(getOwnerINLINE()).getPopulationUnitCity(getID());
		if (pCity == NULL)
		{
			return NULL;
		}
		else
		{
			return pCity->plot();
		}
	}
	else
	{
		return GC.getMapINLINE().plotSorenINLINE(getX_INLINE(), getY_INLINE());
	}
}

CvCity* CvUnit::getCity() const
{
	CvPlot* pPlot = plot();
	if (pPlot != NULL)
	{
		return pPlot->getPlotCity();
	}
	return NULL;
}

int CvUnit::getArea() const
{
	CvPlot* pPlot = plot();
	if (pPlot == NULL)
	{
		return FFreeList::INVALID_INDEX;
	}

	return pPlot->getArea();
}


CvArea* CvUnit::area() const
{
	CvPlot* pPlot = plot();
	if (pPlot == NULL)
	{
		return NULL;
	}

	return pPlot->area();
}


int CvUnit::getLastMoveTurn() const
{
	return m_iLastMoveTurn;
}


void CvUnit::setLastMoveTurn(int iNewValue)
{
	m_iLastMoveTurn = iNewValue;
	FAssert(getLastMoveTurn() >= 0);
}


int CvUnit::getGameTurnCreated() const
{
	return m_iGameTurnCreated;
}


void CvUnit::setGameTurnCreated(int iNewValue)
{
	m_iGameTurnCreated = iNewValue;
	FAssert(getGameTurnCreated() >= 0);
}


int CvUnit::getDamage() const
{
	return m_iDamage;
}


void CvUnit::setDamage(int iNewValue, CvUnit* pAttacker, bool bNotifyEntity)
{
	int iOldValue;

	iOldValue = getDamage();

	m_iDamage = range(iNewValue, 0, maxHitPoints());

	FAssertMsg(currHitPoints() >= 0, "currHitPoints() is expected to be non-negative (invalid Index)");

	if ((iOldValue != getDamage()) && isOnMap())
	{
		if (GC.getGameINLINE().isFinalInitialized() && bNotifyEntity)
		{
			NotifyEntity(MISSION_DAMAGE);
		}

		setInfoBarDirty(true);

		if (IsSelected())
		{
			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		}

		if (plot() == gDLL->getInterfaceIFace()->getSelectionPlot())
		{
			gDLL->getInterfaceIFace()->setDirty(PlotListButtons_DIRTY_BIT, true);
		}
	}

	if (isDead())
	{
		kill(true, pAttacker);
	}
}

void CvUnit::changeDamage(int iChange, CvUnit* pAttacker)
{
	setDamage((getDamage() + iChange), pAttacker);
}


int CvUnit::getMoves() const
{
	return m_iMoves;
}


void CvUnit::setMoves(int iNewValue)
{
	CvPlot* pPlot;

	if (getMoves() != iNewValue)
	{
		pPlot = plot();

		m_iMoves = iNewValue;

		FAssert(getMoves() >= 0);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			if (pPlot != NULL)
			{
				pPlot->setFlagDirty(true);
			}
		}

		if (IsSelected())
		{
			gDLL->getFAStarIFace()->ForceReset(&GC.getInterfacePathFinder());

			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		}

		if (pPlot == gDLL->getInterfaceIFace()->getSelectionPlot())
		{
			gDLL->getInterfaceIFace()->setDirty(PlotListButtons_DIRTY_BIT, true);
		}
	}
}


void CvUnit::changeMoves(int iChange)
{
	setMoves(getMoves() + iChange);
}


void CvUnit::finishMoves()
{
	setMoves(maxMoves());
}


int CvUnit::getExperience() const
{
	return m_iExperience;
}


void CvUnit::setExperience(int iNewValue, int iMax)
{
	if ((getExperience() != iNewValue) && (getExperience() < ((iMax == -1) ? MAX_INT : iMax)))
	{
		m_iExperience = std::min(((iMax == -1) ? MAX_INT : iMax), iNewValue);
		FAssert(getExperience() >= 0);

		if (IsSelected())
		{
			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		}
	}
}


void CvUnit::changeExperience(int iChange, int iMax, bool bFromCombat, bool bInBorders, bool bUpdateGlobal)
{
	int iUnitExperience = iChange;

	if (bFromCombat)
	{
		CvPlayer& kPlayer = GET_PLAYER(getOwnerINLINE());

		int iCombatExperienceMod = 100 + kPlayer.getGreatGeneralRateModifier();

		if (bInBorders)
		{
			iCombatExperienceMod += kPlayer.getDomesticGreatGeneralRateModifier() + kPlayer.getExpInBorderModifier();
			iUnitExperience += (iChange * kPlayer.getExpInBorderModifier()) / 100;
		}

		if (bUpdateGlobal)
		{
			// R&R, ray, Great Admirals - START
			if (getDomainType() == DOMAIN_SEA)
			{
				kPlayer.changeSeaCombatExperience((iChange * iCombatExperienceMod) / 100);
			}
			else
			{
				kPlayer.changeCombatExperience((iChange * iCombatExperienceMod) / 100);
			}
			// R&R, ray, Great Admirals -END
		}

		if (getExperiencePercent() != 0)
		{
			iUnitExperience *= std::max(0, 100 + getExperiencePercent());
			iUnitExperience /= 100;
		}
	}

	setExperience((getExperience() + iUnitExperience), iMax);
}


int CvUnit::getLevel() const
{
	return m_iLevel;
}


void CvUnit::setLevel(int iNewValue)
{
	if (getLevel() != iNewValue)
	{
		m_iLevel = iNewValue;
		FAssert(getLevel() >= 0);

		if (getLevel() > GET_PLAYER(getOwnerINLINE()).getHighestUnitLevel())
		{
			GET_PLAYER(getOwnerINLINE()).setHighestUnitLevel(getLevel());
		}

		if (IsSelected())
		{
			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		}
	}
}


void CvUnit::changeLevel(int iChange)
{
	setLevel(getLevel() + iChange);
}


int CvUnit::getCargo() const
{
	return m_iCargo;
}


void CvUnit::changeCargo(int iChange)
{
	m_iCargo += iChange;
	FAssert(getCargo() >= 0);
}


CvPlot* CvUnit::getAttackPlot() const
{
	return GC.getMapINLINE().plotSorenINLINE(m_iAttackPlotX, m_iAttackPlotY);
}


void CvUnit::setAttackPlot(const CvPlot* pNewValue)
{
	if (getAttackPlot() != pNewValue)
	{
		if (pNewValue != NULL)
		{
			m_iAttackPlotX = pNewValue->getX_INLINE();
			m_iAttackPlotY = pNewValue->getY_INLINE();
		}
		else
		{
			m_iAttackPlotX = INVALID_PLOT_COORD;
			m_iAttackPlotY = INVALID_PLOT_COORD;
		}
	}
}

int CvUnit::getCombatTimer() const
{
	return m_iCombatTimer;
}


void CvUnit::setCombatTimer(int iNewValue)
{
	m_iCombatTimer = iNewValue;
	FAssert(getCombatTimer() >= 0);
}


void CvUnit::changeCombatTimer(int iChange)
{
	setCombatTimer(getCombatTimer() + iChange);
}

int CvUnit::getCombatDamage() const
{
	return m_iCombatDamage;
}


void CvUnit::setCombatDamage(int iNewValue)
{
	m_iCombatDamage = iNewValue;
	FAssert(getCombatDamage() >= 0);
}


int CvUnit::getFortifyTurns() const
{
	return m_iFortifyTurns;
}


void CvUnit::setFortifyTurns(int iNewValue)
{
	iNewValue = range(iNewValue, 0, GC.getDefineINT("MAX_FORTIFY_TURNS"));

	if (iNewValue != getFortifyTurns())
	{
		m_iFortifyTurns = iNewValue;
		setInfoBarDirty(true);
	}
}


void CvUnit::changeFortifyTurns(int iChange)
{
	setFortifyTurns(getFortifyTurns() + iChange);
}


int CvUnit::getBlitzCount() const
{
	return m_iBlitzCount;
}


bool CvUnit::isBlitz() const
{
	return (getBlitzCount() > 0);
}


void CvUnit::changeBlitzCount(int iChange)
{
	m_iBlitzCount = (m_iBlitzCount + iChange);
	FAssert(getBlitzCount() >= 0);
}


int CvUnit::getAmphibCount() const
{
	return m_iAmphibCount;
}


bool CvUnit::isAmphib() const
{
	return (getAmphibCount() > 0);
}


void CvUnit::changeAmphibCount(int iChange)
{
	m_iAmphibCount = (m_iAmphibCount + iChange);
	FAssert(getAmphibCount() >= 0);
}


int CvUnit::getRiverCount() const
{
	return m_iRiverCount;
}


bool CvUnit::isRiver() const
{
	return (getRiverCount() > 0);
}


void CvUnit::changeRiverCount(int iChange)
{
	m_iRiverCount = (m_iRiverCount + iChange);
	FAssert(getRiverCount() >= 0);
}


int CvUnit::getEnemyRouteCount() const
{
	return m_iEnemyRouteCount;
}


bool CvUnit::isEnemyRoute() const
{
	return (getEnemyRouteCount() > 0);
}


void CvUnit::changeEnemyRouteCount(int iChange)
{
	m_iEnemyRouteCount = (m_iEnemyRouteCount + iChange);
	FAssert(getEnemyRouteCount() >= 0);
}


int CvUnit::getAlwaysHealCount() const
{
	return m_iAlwaysHealCount;
}


bool CvUnit::isAlwaysHeal() const
{
	return (getAlwaysHealCount() > 0);
}


void CvUnit::changeAlwaysHealCount(int iChange)
{
	m_iAlwaysHealCount = (m_iAlwaysHealCount + iChange);
	FAssert(getAlwaysHealCount() >= 0);
}


int CvUnit::getHillsDoubleMoveCount() const
{
	return m_iHillsDoubleMoveCount;
}


bool CvUnit::isHillsDoubleMove() const
{
	return (getHillsDoubleMoveCount() > 0);
}


void CvUnit::changeHillsDoubleMoveCount(int iChange)
{
	m_iHillsDoubleMoveCount = (m_iHillsDoubleMoveCount + iChange);
	FAssert(getHillsDoubleMoveCount() >= 0);
}

int CvUnit::getExtraVisibilityRange() const
{
	return m_iExtraVisibilityRange;
}


void CvUnit::changeExtraVisibilityRange(int iChange)
{
	if (iChange != 0)
	{
		plot()->changeAdjacentSight(getTeam(), visibilityRange(), false, this);

		m_iExtraVisibilityRange += iChange;
		FAssert(getExtraVisibilityRange() >= 0);

		plot()->changeAdjacentSight(getTeam(), visibilityRange(), true, this);
	}
}


int CvUnit::getExtraMoves() const
{
	return m_iExtraMoves;
}


void CvUnit::changeExtraMoves(int iChange)
{
	m_iExtraMoves += iChange;
	FAssert(getExtraMoves() >= 0);
}


int CvUnit::getExtraMoveDiscount() const
{
	return m_iExtraMoveDiscount;
}


void CvUnit::changeExtraMoveDiscount(int iChange)
{
	m_iExtraMoveDiscount = (m_iExtraMoveDiscount + iChange);
	FAssert(getExtraMoveDiscount() >= 0);
}

int CvUnit::getExtraWithdrawal() const
{
	return m_iExtraWithdrawal;
}


void CvUnit::changeExtraWithdrawal(int iChange)
{
	m_iExtraWithdrawal = (m_iExtraWithdrawal + iChange);
	FAssert(getExtraWithdrawal() >= 0);
}

int CvUnit::getExtraBombardRate() const
{
	return m_iExtraBombardRate;
}


void CvUnit::changeExtraBombardRate(int iChange)
{
	m_iExtraBombardRate = (m_iExtraBombardRate + iChange);
	FAssert(getExtraBombardRate() >= 0);
}


int CvUnit::getExtraEnemyHeal() const
{
	return m_iExtraEnemyHeal;
}


void CvUnit::changeExtraEnemyHeal(int iChange)
{
	m_iExtraEnemyHeal = (m_iExtraEnemyHeal + iChange);
	FAssert(getExtraEnemyHeal() >= 0);
}


int CvUnit::getExtraNeutralHeal() const
{
	return m_iExtraNeutralHeal;
}


void CvUnit::changeExtraNeutralHeal(int iChange)
{
	m_iExtraNeutralHeal = (m_iExtraNeutralHeal + iChange);
	FAssert(getExtraNeutralHeal() >= 0);
}


int CvUnit::getExtraFriendlyHeal() const
{
	return m_iExtraFriendlyHeal;
}


void CvUnit::changeExtraFriendlyHeal(int iChange)
{
	m_iExtraFriendlyHeal = (m_iExtraFriendlyHeal + iChange);
	FAssert(getExtraFriendlyHeal() >= 0);
}


int CvUnit::getSameTileHeal() const
{
	return m_iSameTileHeal;
}


void CvUnit::changeSameTileHeal(int iChange)
{
	m_iSameTileHeal = (m_iSameTileHeal + iChange);
	FAssert(getSameTileHeal() >= 0);
}


int CvUnit::getAdjacentTileHeal() const
{
	return m_iAdjacentTileHeal;
}


void CvUnit::changeAdjacentTileHeal(int iChange)
{
	m_iAdjacentTileHeal = (m_iAdjacentTileHeal + iChange);
	FAssert(getAdjacentTileHeal() >= 0);
}


int CvUnit::getExtraCombatPercent() const
{
	return m_iExtraCombatPercent + GET_PLAYER(getOwnerINLINE()).getUnitStrengthModifier(getUnitClassType());
}


void CvUnit::changeExtraCombatPercent(int iChange)
{
	if (iChange != 0)
	{
		m_iExtraCombatPercent += iChange;

		setInfoBarDirty(true);
	}
}


int CvUnit::getExtraCityAttackPercent() const
{
	return m_iExtraCityAttackPercent;
}


void CvUnit::changeExtraCityAttackPercent(int iChange)
{
	if (iChange != 0)
	{
		m_iExtraCityAttackPercent = (m_iExtraCityAttackPercent + iChange);

		setInfoBarDirty(true);
	}
}


int CvUnit::getExtraCityDefensePercent() const
{
	return m_iExtraCityDefensePercent;
}


void CvUnit::changeExtraCityDefensePercent(int iChange)
{
	if (iChange != 0)
	{
		m_iExtraCityDefensePercent = (m_iExtraCityDefensePercent + iChange);

		setInfoBarDirty(true);
	}
}


int CvUnit::getExtraHillsAttackPercent() const
{
	return m_iExtraHillsAttackPercent;
}


void CvUnit::changeExtraHillsAttackPercent(int iChange)
{
	if (iChange != 0)
	{
		m_iExtraHillsAttackPercent = (m_iExtraHillsAttackPercent + iChange);

		setInfoBarDirty(true);
	}
}


int CvUnit::getExtraHillsDefensePercent() const
{
	return m_iExtraHillsDefensePercent;
}


void CvUnit::changeExtraHillsDefensePercent(int iChange)
{
	if (iChange != 0)
	{
		m_iExtraHillsDefensePercent = (m_iExtraHillsDefensePercent + iChange);

		setInfoBarDirty(true);
	}
}

int CvUnit::getExtraDomesticBonusPercent() const
{
	return m_iExtraDomesticBonusPercent;
}


void CvUnit::changeExtraDomesticBonusPercent(int iChange)
{
	if (iChange != 0)
	{
		m_iExtraDomesticBonusPercent = (m_iExtraDomesticBonusPercent + iChange);

		setInfoBarDirty(true);
	}
}	

int CvUnit::getPillageChange() const
{
	return m_iPillageChange;
}

void CvUnit::changePillageChange(int iChange)
{
	if (iChange != 0)
	{
		m_iPillageChange += iChange;

		setInfoBarDirty(true);
	}
}

int CvUnit::getUpgradeDiscount() const
{
	return m_iUpgradeDiscount;
}

void CvUnit::changeUpgradeDiscount(int iChange)
{
	if (iChange != 0)
	{
		m_iUpgradeDiscount += iChange;

		setInfoBarDirty(true);
	}
}

int CvUnit::getExperiencePercent() const
{
	return m_iExperiencePercent;
}

void CvUnit::changeExperiencePercent(int iChange)
{
	if (iChange != 0)
	{
		m_iExperiencePercent += iChange;

		setInfoBarDirty(true);
	}
}

DirectionTypes CvUnit::getFacingDirection(bool checkLineOfSightProperty) const
{
	if (checkLineOfSightProperty)
	{
		if (m_pUnitInfo->isLineOfSight())
		{
			return m_eFacingDirection; //only look in facing direction
		}
		else
		{
			return NO_DIRECTION; //look in all directions
		}
	}
	else
	{
		return m_eFacingDirection;
	}
}

void CvUnit::setFacingDirection(DirectionTypes eFacingDirection)
{
	if (eFacingDirection != m_eFacingDirection)
	{
		if (m_pUnitInfo->isLineOfSight())
		{
			//remove old fog
			plot()->changeAdjacentSight(getTeam(), visibilityRange(), false, this);

			//change direction
			m_eFacingDirection = eFacingDirection;

			//clear new fog
			plot()->changeAdjacentSight(getTeam(), visibilityRange(), true, this);

			gDLL->getInterfaceIFace()->setDirty(ColoredPlots_DIRTY_BIT, true);
		}
		else
		{
			m_eFacingDirection = eFacingDirection;
		}

		if (isOnMap())
		{
			//update formation
			NotifyEntity(NO_MISSION);
		}
	}
}

void CvUnit::rotateFacingDirectionClockwise()
{
	//change direction
	DirectionTypes eNewDirection = (DirectionTypes) ((m_eFacingDirection + 1) % NUM_DIRECTION_TYPES);
	setFacingDirection(eNewDirection);
}

void CvUnit::rotateFacingDirectionCounterClockwise()
{
	//change direction
	DirectionTypes eNewDirection = (DirectionTypes) ((m_eFacingDirection + NUM_DIRECTION_TYPES - 1) % NUM_DIRECTION_TYPES);
	setFacingDirection(eNewDirection);
}

ProfessionTypes CvUnit::getProfession() const
{
	return m_eProfession;
}

void CvUnit::setProfession(ProfessionTypes eProfession, bool bForce)
{
	if (!bForce && !canHaveProfession(eProfession, false))
	{
		FAssertMsg(false, "Unit can not have profession");
		return;
	}

	if (getProfession() != eProfession)
	{
		if (getProfession() != NO_PROFESSION)
		{
			if (GC.getProfessionInfo(getProfession()).isCitizen())
			{
				AI_setOldProfession(getProfession());
			}
		}
		if (isOnMap() && eProfession != NO_PROFESSION && GC.getProfessionInfo(eProfession).isCitizen())
		{
			CvCity* pCity = plot()->getPlotCity();
			if (pCity != NULL)
			{
				if (canJoinCity(plot()))
				{
					pCity->addPopulationUnit(this, eProfession);
					bool bLock = true;
					if (GC.getProfessionInfo(eProfession).isWorkPlot())
					{
						int iPlotIndex = pCity->AI_bestProfessionPlot(eProfession, this);
						if (iPlotIndex != -1)
						{
							pCity->alterUnitWorkingPlot(iPlotIndex, getID(), false);
						}
						else
						{
							bLock = false;
						}
					}

					setColonistLocked(bLock);
					return;
				}
			}
		}

		processProfession(getProfession(), -1, false);
		if (getProfessionUnitCombatType(getProfession()) != getProfessionUnitCombatType(eProfession))
		{
			for (int iPromotion = 0; iPromotion < GC.getNumPromotionInfos(); ++iPromotion)
			{
				if (isHasPromotion((PromotionTypes) iPromotion))
				{
					processPromotion((PromotionTypes) iPromotion, -1);
				}
			}
		}
		ProfessionTypes eOldProfession = getProfession();
		m_eProfession = eProfession;
		if (getProfessionUnitCombatType(eOldProfession) != getProfessionUnitCombatType(getProfession()))
		{
			for (int iPromotion = 0; iPromotion < GC.getNumPromotionInfos(); ++iPromotion)
			{
				if (isHasPromotion((PromotionTypes) iPromotion))
				{
					processPromotion((PromotionTypes) iPromotion, 1);
				}
			}
		}
		processProfession(getProfession(), 1, true);

		//reload unit model
		reloadEntity();
		gDLL->getInterfaceIFace()->setDirty(Domestic_Advisor_DIRTY_BIT, true);
	}

	if (eProfession != NO_PROFESSION)
	{
		CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);
		if (!kProfession.isCitizen())
		{
			if (kProfession.getDefaultUnitAIType() != NO_UNITAI)
			{
				AI_setUnitAIType((UnitAITypes)kProfession.getDefaultUnitAIType());
			}
		}
	}
}

bool CvUnit::canHaveProfession(ProfessionTypes eProfession, bool bBumpOther, const CvPlot* pPlot) const
{
	if (NO_PROFESSION == eProfession)
	{
		return true;
	}
	
	if (eProfession == getProfession())
	{
		return true;
	}
	///TK Viscos Mod
	//for (iJ = 0; iJ < GC.getNumProfessionInfos(); iJ++)
    //{
    if (GC.getUnitInfo(getUnitType()).getProfessionsNotAllowed(eProfession))
    {
        return false;
    }
    //}
	///Tk end

	CvProfessionInfo& kNewProfession = GC.getProfessionInfo(eProfession);
	
	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());

	if (!kOwner.isProfessionValid(eProfession, getUnitType()))
	{
		return false;
	}

	//TAC Whaling, ray
	if (eProfession == (ProfessionTypes)GC.getPROFESSION_WHALING_BOAT_WORKING()) // cache ship profession - Nightinggale
	{
		if (!isWhalingBoat())
		{
			return false;
		}
	}
	//End TAC Whaling, ray

	// R&R, ray, High Sea Fishing - START
	if (eProfession == (ProfessionTypes)GC.getPROFESSION_FISHING_BOAT_WORKING()) // cache ship profession - Nightinggale
	{
		if (!isFishingBoat())
		{
			return false;
		}
	}
	// R&R, ray, High Sea Fishing - END

	// R&R, ray , MYCP partially based on code of Aymerick - START
	for (int i = 0; i < kNewProfession.getNumYieldsProduced(); i++)
	{
		if (kNewProfession.getYieldsProduced(i) == YIELD_EDUCATION)
		{
			if (m_pUnitInfo->getStudentWeight() <= 0)
			{
				return false;
			}
		}
	}
	// R&R, ray , MYCP partially based on code of Aymerick - END
	if (pPlot == NULL)
	{
		pPlot = plot();
	}

	CvCity* pCity = NULL;
	if (pPlot != NULL)
	{
		if (pPlot->getOwnerINLINE() == getOwnerINLINE())
		{
			pCity = pPlot->getPlotCity();
		}
	}
	if (pCity == NULL)
	{
		pCity = kOwner.getPopulationUnitCity(getID());
	}

	bool bEuropeUnit = false;
	if (pCity == NULL)
	{
		CvUnit* pUnit = kOwner.getEuropeUnitById(getID());
		bEuropeUnit = (pUnit != NULL);
		FAssert(pUnit == this || pUnit == NULL);
	}

	if (pCity != NULL)
	{
		//make sure all equipment is available
		if (!pCity->AI_isWorkforceHack())
		{
			if (kOwner.hasContentsYieldEquipmentAmountSecure(getProfession()) || kOwner.hasContentsYieldEquipmentAmount(eProfession)) // cache CvPlayer::getYieldEquipmentAmount - Nightinggale
			{
				for (int i=0; i < NUM_YIELD_TYPES; ++i)
				{
					YieldTypes eYieldType = (YieldTypes) i;
					int iYieldCarried  = kOwner.getYieldEquipmentAmountSecure(getProfession(), eYieldType); // cache CvPlayer::getYieldEquipmentAmount - Nightinggale
					int iYieldRequired = kOwner.getYieldEquipmentAmount(eProfession, eYieldType);
					if (iYieldRequired > 0)
					{
						int iMissing = iYieldRequired - iYieldCarried;

						//WTP, ray, Settler Professsion - START
						// we only do this if the city is larger 3
						if (!kOwner.isHuman() && !kOwner.isNative() && kNewProfession.canFound() &&  pCity->getPopulation() > 3)
						{
							int iYieldsStoredInCity = pCity->getYieldStored(eYieldType);
							if (iMissing > iYieldsStoredInCity)
							{
								// we check the requiredm Yields for AI so it can equip Settlers
								int iYieldAmountToBeAdded = iMissing - iYieldsStoredInCity;

								//I explicitly use Europe Sell Price because Europe Buy Price would be too expensive.
								int iPriceSettlerYieldPrice = iYieldAmountToBeAdded * kOwner.getYieldSellPrice(eYieldType);

								// we give Yields required for a little gold
								if (kOwner.getGold() > iPriceSettlerYieldPrice)
								{
									pCity->changeYieldStored(eYieldType, iYieldAmountToBeAdded);
									kOwner.changeGold(-iPriceSettlerYieldPrice);
								}
							}
						}
						//WTP, ray, Settler Professsion - END

						if (iMissing > pCity->getYieldStored(eYieldType))
						{
							return false;
						}
					}
				}
			}

			if (!kNewProfession.isCitizen())
			{
				if (movesLeft() == 0)
				{
					return false;
				}
			}
		}
	}

	if (bEuropeUnit && !kOwner.isEurope())
	{
		if (getEuropeProfessionChangeCost(eProfession) > kOwner.getGold())
		{
			return false;
		}

		if (kOwner.hasContentsYieldEquipmentAmount(eProfession)) // cache CvPlayer::getYieldEquipmentAmount - Nightinggale
		{
			for (int i=0; i < NUM_YIELD_TYPES; ++i)
			{
				YieldTypes eYield = (YieldTypes) i;
				if (!kOwner.isYieldEuropeTradable(eYield))
				{
					if (kOwner.getYieldEquipmentAmount(eProfession, eYield) > kOwner.getYieldEquipmentAmount(getProfession(), eYield))
					{
						return false;
					}
				}
			}
		}
	}

	if (pCity != NULL)
	{
		 if (!pCity->AI_isWorkforceHack())
		 {
			//check if special building has been built
			if (kNewProfession.getSpecialBuilding() != NO_SPECIALBUILDING)
			{
				if (pCity->getProfessionOutput(eProfession, this) <= 0)
				{
					return false;
				}
			}

			// check against building max
			if (!bBumpOther)
			{
				if (!pCity->isAvailableProfessionSlot(eProfession, this))
				{
					return false;
				}
			}

			//do not allow leaving empty city
			if (!kNewProfession.isCitizen() && !isOnMap())
			{
				// R&R, ray, Abandon City, START
				if (pCity->getPopulation() <= 1) 
				{
					if (!isHuman()) // allow only humans to disband cities
					{
						return false;
					}

					// R&R, ray, removed it because I did not like it anymore
					/*else
					{
						bool hasWar = false;
						TeamTypes ownTeamID = kOwner.getTeam();
						CvTeam& ownTeam = GET_TEAM(ownTeamID);

						// R&R, ray, changes to Wild Animals - START
						TeamTypes barbarianTeam = GET_PLAYER(GC.getGameINLINE().getBarbarianPlayer()).getTeam();

						for (int iJ = 0; iJ < MAX_TEAMS; iJ++)
						{
							// R&R, ray, changes to Wild Animals - START
							if ((iJ != barbarianTeam) && ownTeam.isAtWar((TeamTypes)iJ) && GET_TEAM((TeamTypes)iJ).isAlive())
							{
								hasWar = true;
								break;
							}	
						}
						if (hasWar)
						{
							if (pCity->getPopulation() < 1)
							{
								if (eProfession == (ProfessionTypes) GC.getCivilizationInfo(getCivilizationType()).getDefaultProfession())
								{
									CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_TEXT);
									pInfo->setText(gDLL->getText("NO_ABANDONING_CITY_BECAUSE_WAR"));
									gDLL->getInterfaceIFace()->addPopup(pInfo, getOwnerINLINE(), true, true);
								}
							}
							return false;
						}
					}*/
				}
				
				if (pCity->getPopulation() <= 0) // Still can't create a unit if no population
				{
					return false;
				}
				// R&R, ray, Abandon City,  END
			}

			if (kNewProfession.isCitizen() && isOnMap())
			{
				if (!canJoinCity(pPlot))
				{
					return false;
				}
			}
		 }
	}
	else
	{
		if (kNewProfession.isCitizen())
		{
			return false;
		}
		
		if (isOnMap())
		{
			//TAC Whaling, ray
			if (!getUnitInfo().isGatherBoat())
			{
				return false;
			}
			//End TAC Whaling, ray
		}
	}

	return true;
}

void CvUnit::processProfession(ProfessionTypes eProfession, int iChange, bool bUpdateCity)
{
	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());

	if (iChange != 0)
	{
		processProfessionStats(eProfession, iChange);

		if (eProfession != NO_PROFESSION)
		{
			CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);

			kOwner.changeAssets(iChange * kProfession.getAssetValue());

			int iPower = iChange * kProfession.getPowerValue();

			if (GET_PLAYER(getOwnerINLINE()).hasContentsYieldEquipmentAmount(eProfession)) // cache CvPlayer::getYieldEquipmentAmount - Nightinggale
			{
				for (int i = 0; i < NUM_YIELD_TYPES; ++i)
				{
					YieldTypes eYield = (YieldTypes) i;
					int iYieldAmount = GET_PLAYER(getOwnerINLINE()).getYieldEquipmentAmount(eProfession, eYield);
					iPower += iChange * GC.getYieldInfo(eYield).getPowerValue() * iYieldAmount;
					kOwner.changeAssets(iChange * GC.getYieldInfo(eYield).getAssetValue() * iYieldAmount);
				}
			}

			kOwner.changePower(iPower);
			CvArea* pArea = area();
			if (pArea != NULL)
			{
				pArea->changePower(getOwnerINLINE(), iPower);
			}
		}
	}

	CvCity* pCity = kOwner.getPopulationUnitCity(getID());
	if (pCity == NULL)
	{
		CvPlot* pPlot = plot();
		if (pPlot != NULL)
		{
			pCity = pPlot->getPlotCity();
		}
	}

	if (pCity != NULL && pCity->getOwnerINLINE() == getOwnerINLINE())
	{
		if (iChange != 0)
		{
			if (GET_PLAYER(getOwnerINLINE()).hasContentsYieldEquipmentAmountSecure(eProfession)) // cache CvPlayer::getYieldEquipmentAmount - Nightinggale
			{
				for (int i = 0; i < NUM_YIELD_TYPES; i++)
				{
					YieldTypes eYield = (YieldTypes) i;
					pCity->changeYieldStored(eYield, -iChange * kOwner.getYieldEquipmentAmount(eProfession, eYield));
				}
			}
		}

		if (bUpdateCity)
		{
			pCity->setYieldRateDirty();
			pCity->updateYield();
			CvPlot* pPlot = pCity->getPlotWorkedByUnit(this);
			if(pPlot != NULL)
			{
				pCity->verifyWorkingPlot(pCity->getCityPlotIndex(pPlot));
			}
			pCity->AI_setAssignWorkDirty(true);
		}
	}
}

void CvUnit::processProfessionStats(ProfessionTypes eProfession, int iChange)
{
	if (iChange != 0)
	{
		CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());
		if (eProfession != NO_PROFESSION)
		{
			CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);
			setBaseCombatStr(baseCombatStr() + iChange * (kProfession.getCombatChange() + kOwner.getProfessionCombatChange(eProfession)));
			changeExtraMoves(iChange * kProfession.getMovesChange());
			changeExtraWorkRate(iChange *  kProfession.getWorkRate());
			if (!kProfession.isCityDefender())
			{
				changeBadCityDefenderCount(iChange);
			}
			if (kProfession.isUnarmed())
			{
				changeUnarmedCount(iChange);
			}

			for (int iPromotion = 0; iPromotion < GC.getNumPromotionInfos(); iPromotion++)
			{
				if (kProfession.isFreePromotion(iPromotion))
				{
					changeFreePromotionCount((PromotionTypes) iPromotion, iChange);
				}
			}
		}

		processUnitCombatType(getProfessionUnitCombatType(eProfession), iChange);
	}
}


int CvUnit::getProfessionChangeYieldRequired(ProfessionTypes eProfession, YieldTypes eYield) const
{
	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());
	int iYieldCarried = 0;
	if (getProfession() != NO_PROFESSION)
	{
		iYieldCarried += kOwner.getYieldEquipmentAmount(getProfession(), eYield);
	}
	return (kOwner.getYieldEquipmentAmount(eProfession, eYield) - iYieldCarried);
}


int CvUnit::getEuropeProfessionChangeCost(ProfessionTypes eProfession) const
{
	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());
	FAssert(kOwner.getParent() != NO_PLAYER);
	CvPlayer& kEurope = GET_PLAYER(kOwner.getParent());

	int iGoldCost = 0;
	for (int i=0; i < NUM_YIELD_TYPES; ++i)
	{
		YieldTypes eYieldType = (YieldTypes) i;
		int iMissing = getProfessionChangeYieldRequired(eProfession, eYieldType);
		if (iMissing > 0)
		{
			iGoldCost += kEurope.getYieldSellPrice(eYieldType) * iMissing;
		}
		else if (iMissing < 0)
		{
			iGoldCost -= kOwner.getSellToEuropeProfit(eYieldType, -iMissing);
		}
	}

	return iGoldCost;
}

int CvUnit::getImmobileTimer() const
{
	return m_iImmobileTimer;
}

void CvUnit::setImmobileTimer(int iNewValue)
{
	if (iNewValue != getImmobileTimer())
	{
		m_iImmobileTimer = iNewValue;

		setInfoBarDirty(true);
	}
}

void CvUnit::changeImmobileTimer(int iChange)
{
	if (iChange != 0)
	{
		setImmobileTimer(std::max(0, getImmobileTimer() + iChange));
	}
}

bool CvUnit::isMadeAttack() const
{
	return m_bMadeAttack;
}


void CvUnit::setMadeAttack(bool bNewValue)
{
	m_bMadeAttack = bNewValue;
}


bool CvUnit::isPromotionReady() const
{
	return m_bPromotionReady;
}

void CvUnit::setPromotionReady(bool bNewValue)
{
	if (isPromotionReady() != bNewValue)
	{
		m_bPromotionReady = bNewValue;

		if (m_bPromotionReady)
		{
			getGroup()->setAutomateType(NO_AUTOMATE);
			getGroup()->clearMissionQueue();
			getGroup()->setActivityType(ACTIVITY_AWAKE);
		}

		gDLL->getEntityIFace()->showPromotionGlow(getUnitEntity(), bNewValue);

		if (IsSelected())
		{
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		}
	}
}


void CvUnit::testPromotionReady()
{
	setPromotionReady((getExperience() >= experienceNeeded()) && canAcquirePromotionAny());
}


bool CvUnit::isDelayedDeath() const
{
	return m_bDeathDelay;
}


void CvUnit::startDelayedDeath()
{
	m_bDeathDelay = true;
}


// Returns true if killed...
bool CvUnit::doDelayedDeath()
{
	if (m_bDeathDelay && !isFighting())
	{
		kill(false);
		return true;
	}

	return false;
}


bool CvUnit::isCombatFocus() const
{
	return m_bCombatFocus;
}


bool CvUnit::isInfoBarDirty() const
{
	return m_bInfoBarDirty;
}


void CvUnit::setInfoBarDirty(bool bNewValue)
{
	m_bInfoBarDirty = bNewValue;
}

PlayerTypes CvUnit::getOwner() const
{
	return getOwnerINLINE();
}

PlayerTypes CvUnit::getVisualOwner(TeamTypes eForTeam) const
{
	if (NO_TEAM == eForTeam)
	{
		eForTeam = GC.getGameINLINE().getActiveTeam();
	}

	if (getTeam() != eForTeam)
	{
		if (m_pUnitInfo->isHiddenNationality())
		{
			if (!plot()->isCity(true, getTeam()))
			{
				return UNKNOWN_PLAYER;
			}
		}
	}

	return getOwnerINLINE();
}


PlayerTypes CvUnit::getCombatOwner(TeamTypes eForTeam, const CvPlot* pPlot) const
{
	if (eForTeam != UNKNOWN_TEAM && getTeam() != eForTeam && eForTeam != NO_TEAM)
	{
		if (isAlwaysHostile(pPlot))
		{
			return UNKNOWN_PLAYER;
		}
	}

	return getOwnerINLINE();
}

TeamTypes CvUnit::getTeam() const
{
	return GET_PLAYER(getOwnerINLINE()).getTeam();
}

TeamTypes CvUnit::getCombatTeam(TeamTypes eForTeam, const CvPlot* pPlot) const
{
	TeamTypes eTeam;
	PlayerTypes eOwner = getCombatOwner(eForTeam, pPlot);
	switch (eOwner)
	{
	case UNKNOWN_PLAYER:
		eTeam = UNKNOWN_TEAM;
		break;
	case NO_PLAYER:
		eTeam = NO_TEAM;
		break;
	default:
		eTeam = GET_PLAYER(eOwner).getTeam();
		break;
	}

	return eTeam;
}

CivilizationTypes CvUnit::getVisualCiv(TeamTypes eForTeam) const
{
	PlayerTypes eOwner = getVisualOwner(eForTeam);
	if (eOwner == UNKNOWN_PLAYER)
	{
		return (CivilizationTypes) GC.getDefineINT("BARBARIAN_CIVILIZATION");
	}

	return GET_PLAYER(eOwner).getCivilizationType();
}

PlayerColorTypes CvUnit::getPlayerColor(TeamTypes eForTeam) const
{
	PlayerTypes eOwner = getVisualOwner(eForTeam);
	if (eOwner == UNKNOWN_PLAYER || eOwner == NO_PLAYER)
	{
		return (PlayerColorTypes) GC.getCivilizationInfo(getVisualCiv(eForTeam)).getDefaultPlayerColor();
	}

	return GET_PLAYER(eOwner).getPlayerColor();
}

PlayerTypes CvUnit::getCapturingPlayer() const
{
	return m_eCapturingPlayer;
}


void CvUnit::setCapturingPlayer(PlayerTypes eNewValue)
{
	m_eCapturingPlayer = eNewValue;
}


UnitTypes CvUnit::getUnitType() const
{
	return m_eUnitType;
}

CvUnitInfo &CvUnit::getUnitInfo() const
{
	return *m_pUnitInfo;
}


UnitClassTypes CvUnit::getUnitClassType() const
{
	return (UnitClassTypes)m_pUnitInfo->getUnitClassType();
}

UnitTypes CvUnit::getLeaderUnitType() const
{
	return m_eLeaderUnitType;
}

void CvUnit::setLeaderUnitType(UnitTypes leaderUnitType)
{
	if(m_eLeaderUnitType != leaderUnitType)
	{
		m_eLeaderUnitType = leaderUnitType;
		reloadEntity();
	}
}

CvUnit* CvUnit::getCombatUnit() const
{
	return getUnit(m_combatUnit);
}


void CvUnit::setCombatUnit(CvUnit* pCombatUnit, bool bAttacking)
{
	if (isCombatFocus())
	{
		gDLL->getInterfaceIFace()->setCombatFocus(false);
	}

	if (pCombatUnit != NULL)
	{
		if (bAttacking)
		{
			if (GC.getLogging())
			{
				if (gDLL->getChtLvl() > 0)
				{
					// Log info about this combat...
					char szOut[1024];
					sprintf( szOut, "*** KOMBAT!\n     ATTACKER: Player %d Unit %d (%S's %S), CombatStrength=%d\n     DEFENDER: Player %d Unit %d (%S's %S), CombatStrength=%d\n",
						getOwnerINLINE(), getID(), GET_PLAYER(getOwnerINLINE()).getName(), getName().GetCString(), currCombatStr(NULL, NULL),
						pCombatUnit->getOwnerINLINE(), pCombatUnit->getID(), GET_PLAYER(pCombatUnit->getOwnerINLINE()).getName(), pCombatUnit->getName().GetCString(), pCombatUnit->currCombatStr(pCombatUnit->plot(), this));
					gDLL->messageControlLog(szOut);
				}
			}
		}

		FAssertMsg(getCombatUnit() == NULL, "Combat Unit is not expected to be assigned");
		FAssertMsg(!(plot()->isFighting()), "(plot()->isFighting()) did not return false as expected");
		m_bCombatFocus = (bAttacking && !(gDLL->getInterfaceIFace()->isFocusedWidget()) && ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) || ((pCombatUnit->getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && !(GC.getGameINLINE().isMPOption(MPOPTION_SIMULTANEOUS_TURNS)))));
		m_combatUnit = pCombatUnit->getIDInfo();
		setCombatDamage(0);
		setPostCombatPlot(getX_INLINE(), getY_INLINE());
	}
	else
	{
		if(getCombatUnit() != NULL)
		{
			FAssertMsg(getCombatUnit() != NULL, "getCombatUnit() is not expected to be equal with NULL");
			FAssertMsg(plot()->isFighting(), "plot()->isFighting is expected to be true");
			m_bCombatFocus = false;
			m_combatUnit.reset();
			setCombatDamage(0);

			if (IsSelected())
			{
				gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
			}

			if (plot() == gDLL->getInterfaceIFace()->getSelectionPlot())
			{
				gDLL->getInterfaceIFace()->setDirty(PlotListButtons_DIRTY_BIT, true);
			}

			CvPlot* pPlot = getPostCombatPlot();
			if (pPlot != plot())
			{
				if (pPlot->isFriendlyCity(*this, true))
				{
					setXY(pPlot->getX_INLINE(), pPlot->getY_INLINE());
					finishMoves();
				}
			}
			setPostCombatPlot(INVALID_PLOT_COORD, INVALID_PLOT_COORD);
		}
	}

	setCombatTimer(0);
	setInfoBarDirty(true);

	if (isCombatFocus())
	{
		gDLL->getInterfaceIFace()->setCombatFocus(true);
	}
}

CvPlot* CvUnit::getPostCombatPlot() const
{
	return GC.getMapINLINE().plotByIndexINLINE(m_iPostCombatPlotIndex);
}

void CvUnit::setPostCombatPlot(int iX, int iY)
{
	m_iPostCombatPlotIndex = GC.getMapINLINE().isPlotINLINE(iX, iY) ? GC.getMapINLINE().plotNumINLINE(iX, iY) : -1;
}

CvUnit* CvUnit::getTransportUnit() const
{
	return getUnit(m_transportUnit);
}


bool CvUnit::isCargo() const
{
	return (getTransportUnit() != NULL);
}

// returns false if the unit is killed
bool CvUnit::setTransportUnit(CvUnit* pTransportUnit, bool bUnload)
{
	CvUnit* pOldTransportUnit = getTransportUnit();

	// PatchMod: Berth Size START
    int iCargoSize = getUnitInfo().getBerthSize();
	if (iCargoSize < 1 || iCargoSize > getUnitInfo().getRequiredTransportSize())
	{
		iCargoSize = getUnitInfo().getRequiredTransportSize();
	}
    // PatchMod: Berth Size END

	if (pOldTransportUnit != pTransportUnit)
	{
		if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		}

		CvPlot* pPlot = plot();

		if (pOldTransportUnit != NULL)
		{
			// PatchMod: Berth Size START
		    pOldTransportUnit->changeCargo(-iCargoSize);
			//pOldTransportUnit->changeCargo(-1);
			// PatchMod: Berth Size END
		}
		m_transportUnit.reset();

		if (pTransportUnit != NULL)
		{
			FAssertMsg(pTransportUnit->cargoSpaceAvailable(getSpecialUnitType(), getDomainType()) > 0 || getYield() != NO_YIELD, "Cargo space is expected to be available");

			setUnitTravelState(pTransportUnit->getUnitTravelState(), false);

			//check if combining cargo
			YieldTypes eYield = getYield();
			if (eYield != NO_YIELD)
			{
				CvPlot* pPlot = pTransportUnit->plot();
				if (pPlot != NULL)
				{
					for (int i = 0; i < pPlot->getNumUnits(); i++)
					{
						CvUnit* pLoopUnit = pPlot->getUnitByIndex(i);
						if(pLoopUnit != NULL)
						{
							if (pLoopUnit->getTransportUnit() == pTransportUnit)
							{
								if (pLoopUnit->getYield() == eYield)
								{
									//merge yields
									int iTotalYields = pLoopUnit->getYieldStored() + getYieldStored();
									int iYield1 = std::min(iTotalYields, GC.getGameINLINE().getCargoYieldCapacity());
									int iYield2 = iTotalYields - iYield1;
									pLoopUnit->setYieldStored(iYield1);
									setYieldStored(iYield2);

									//all yields have been transferred to another unit
									if (getYieldStored() == 0)
									{
										kill(true);
										return false;
									}

									//check if load anymore of this cargo
									if (pTransportUnit->getLoadYieldAmount(eYield) == 0)
									{
										return true;
									}
								}
							}
						}
					}
				}
			}

			joinGroup(NULL, true); // Because what if a group of 3 tries to get in a transport which can hold 2...

			m_transportUnit = pTransportUnit->getIDInfo();

			getGroup()->setActivityType(ACTIVITY_SLEEP);

			if (pPlot != pTransportUnit->plot())
			{
				FAssert(getUnitTravelState() != NO_UNIT_TRAVEL_STATE);
				setXY(pTransportUnit->getX_INLINE(), pTransportUnit->getY_INLINE());
			}

			// PatchMod: Berth Size START
            pTransportUnit->changeCargo(iCargoSize);
			//pTransportUnit->changeCargo(1);
			// PatchMod: Berth Size END
			pTransportUnit->getGroup()->setActivityType(ACTIVITY_AWAKE);
		}
		else //dropped off of vehicle
		{
			if (!isHuman() && (getMoves() < maxMoves()))
			{
				if (pOldTransportUnit != NULL)
				{
					AI_setMovePriority(pOldTransportUnit->AI_getMovePriority() + 1);
				}
			}
			else
			{
				if (getGroup()->getActivityType() != ACTIVITY_MISSION)
				{
					getGroup()->setActivityType(ACTIVITY_AWAKE);
				}
			}

			//place yields into city
			if (bUnload && getYield() != NO_YIELD)
			{
				doUnloadYield(getYieldStored());
			}
		}

		if (pPlot != NULL)
		{
			pPlot->updateCenterUnit();
		}
	}

	return true;
}


int CvUnit::getExtraDomainModifier(DomainTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiExtraDomainModifier[eIndex];
}


void CvUnit::changeExtraDomainModifier(DomainTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiExtraDomainModifier[eIndex] = (m_aiExtraDomainModifier[eIndex] + iChange);
}


const CvWString CvUnit::getName(uint uiForm) const
{
	CvWString szBuffer;

	if (isEmpty(m_szName))
	{
		return m_pUnitInfo->getDescription(uiForm);
	}

	szBuffer.Format(L"%s (%s)", m_szName.GetCString(), m_pUnitInfo->getDescription(uiForm));

	return szBuffer;
}


const wchar* CvUnit::getNameKey() const
{
	if (isEmpty(m_szName))
	{
		return m_pUnitInfo->getTextKeyWide();
	}
	else
	{
		return m_szName.GetCString();
	}
}


const CvWString CvUnit::getNameNoDesc() const
{
	return m_szName.GetCString();
}

const CvWString CvUnit::getNameAndProfession() const
{
	CvWString szText;

	//TAC Whaling, ray
	if (NO_PROFESSION != getProfession() && !getUnitInfo().isGatherBoat())
	{
		szText.Format(L"%s (%s)", GC.getProfessionInfo(getProfession()).getDescription(), getName().GetCString());
	}
	else
	{
		szText = getName();
	}

	return szText;
}

const wchar* CvUnit::getNameOrProfessionKey() const
{
	if(getProfession() != NO_PROFESSION)
	{
		return GC.getProfessionInfo(getProfession()).getTextKeyWide();
	}
	//ray messages Ship Names
	else if (getDomainType() == DOMAIN_SEA){
		return m_pUnitInfo->getDescription();
	}

	else
	{
		return getNameKey();
	}
}

void CvUnit::setName(CvWString szNewValue)
{
	gDLL->stripSpecialCharacters(szNewValue);

	m_szName = szNewValue;

	if (IsSelected())
	{
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
	}
}


std::string CvUnit::getScriptData() const
{
	return m_szScriptData;
}


void CvUnit::setScriptData(std::string szNewValue)
{
	m_szScriptData = szNewValue;
}


int CvUnit::getTerrainDoubleMoveCount(TerrainTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiTerrainDoubleMoveCount[eIndex];
}


bool CvUnit::isTerrainDoubleMove(TerrainTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return (getTerrainDoubleMoveCount(eIndex) > 0);
}


void CvUnit::changeTerrainDoubleMoveCount(TerrainTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiTerrainDoubleMoveCount[eIndex] = (m_paiTerrainDoubleMoveCount[eIndex] + iChange);
	FAssert(getTerrainDoubleMoveCount(eIndex) >= 0);
}


int CvUnit::getFeatureDoubleMoveCount(FeatureTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumFeatureInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiFeatureDoubleMoveCount[eIndex];
}


bool CvUnit::isFeatureDoubleMove(FeatureTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumFeatureInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return (getFeatureDoubleMoveCount(eIndex) > 0);
}


void CvUnit::changeFeatureDoubleMoveCount(FeatureTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumFeatureInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiFeatureDoubleMoveCount[eIndex] = (m_paiFeatureDoubleMoveCount[eIndex] + iChange);
	FAssert(getFeatureDoubleMoveCount(eIndex) >= 0);
}


int CvUnit::getExtraTerrainAttackPercent(TerrainTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiExtraTerrainAttackPercent[eIndex];
}


void CvUnit::changeExtraTerrainAttackPercent(TerrainTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiExtraTerrainAttackPercent[eIndex] += iChange;

		setInfoBarDirty(true);
	}
}

int CvUnit::getExtraTerrainDefensePercent(TerrainTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiExtraTerrainDefensePercent[eIndex];
}


void CvUnit::changeExtraTerrainDefensePercent(TerrainTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiExtraTerrainDefensePercent[eIndex] += iChange;

		setInfoBarDirty(true);
	}
}

int CvUnit::getExtraFeatureAttackPercent(FeatureTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumFeatureInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiExtraFeatureAttackPercent[eIndex];
}


void CvUnit::changeExtraFeatureAttackPercent(FeatureTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumFeatureInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiExtraFeatureAttackPercent[eIndex] += iChange;

		setInfoBarDirty(true);
	}
}

int CvUnit::getExtraFeatureDefensePercent(FeatureTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumFeatureInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiExtraFeatureDefensePercent[eIndex];
}


void CvUnit::changeExtraFeatureDefensePercent(FeatureTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumFeatureInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiExtraFeatureDefensePercent[eIndex] += iChange;

		setInfoBarDirty(true);
	}
}

int CvUnit::getExtraUnitClassAttackModifier(UnitClassTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiExtraUnitClassAttackModifier[eIndex];
}

void CvUnit::changeExtraUnitClassAttackModifier(UnitClassTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiExtraUnitClassAttackModifier[eIndex] += iChange;
}

int CvUnit::getExtraUnitClassDefenseModifier(UnitClassTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiExtraUnitClassDefenseModifier[eIndex];
}

void CvUnit::changeExtraUnitClassDefenseModifier(UnitClassTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiExtraUnitClassDefenseModifier[eIndex] += iChange;
}

int CvUnit::getExtraUnitCombatModifier(UnitCombatTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitCombatInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiExtraUnitCombatModifier[eIndex];
}

void CvUnit::changeExtraUnitCombatModifier(UnitCombatTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitCombatInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiExtraUnitCombatModifier[eIndex] += iChange;
}

bool CvUnit::canAcquirePromotion(PromotionTypes ePromotion) const
{
	FAssertMsg(ePromotion >= 0, "ePromotion is expected to be non-negative (invalid Index)");
	FAssertMsg(ePromotion < GC.getNumPromotionInfos(), "ePromotion is expected to be within maximum bounds (invalid Index)");

	if (isHasPromotion(ePromotion))
	{
		return false;
	}

	if (GC.getPromotionInfo(ePromotion).getPrereqPromotion() != NO_PROMOTION)
	{
		if (!isHasPromotion((PromotionTypes)(GC.getPromotionInfo(ePromotion).getPrereqPromotion())))
		{
			return false;
		}
	}

	if (GC.getPromotionInfo(ePromotion).getPrereqOrPromotion1() != NO_PROMOTION)
	{
		if (!isHasPromotion((PromotionTypes)(GC.getPromotionInfo(ePromotion).getPrereqOrPromotion1())))
		{
			if ((GC.getPromotionInfo(ePromotion).getPrereqOrPromotion2() == NO_PROMOTION) || !isHasPromotion((PromotionTypes)(GC.getPromotionInfo(ePromotion).getPrereqOrPromotion2())))
			{
				return false;
			}
		}
	}
	if (!isPromotionValid(ePromotion))
	{
		return false;
	}

	return true;
}

bool CvUnit::isPromotionValid(PromotionTypes ePromotion) const
{
	CvPromotionInfo& kPromotion = GC.getPromotionInfo(ePromotion);

	if (kPromotion.isGraphicalOnly() && !kPromotion.isLeader())
	{
		return false;
	}

	if (isOnlyDefensive())
	{
		if (kPromotion.getCityAttackPercent() != 0)
		{
			return false;
		}
		if (kPromotion.getWithdrawalChange() != 0)
		{
			return false;
		}
		if (kPromotion.isBlitz())
		{
			return false;
		}
		if (kPromotion.isAmphib())
		{
			return false;
		}
		if (kPromotion.isRiver())
		{
			return false;
		}
		if (kPromotion.getHillsAttackPercent() != 0)
		{
			return false;
		}
		for (int iTerrain = 0; iTerrain < GC.getNumTerrainInfos(); ++iTerrain)
		{
			if (kPromotion.getTerrainAttackPercent(iTerrain) != 0)
			{
				return false;
			}
		}
		for (int iFeature = 0; iFeature < GC.getNumFeatureInfos(); ++iFeature)
		{
			if (kPromotion.getFeatureAttackPercent(iFeature) != 0)
			{
				return false;
			}
		}
		if (kPromotion.getWithdrawalChange() != 0)
		{
			return false;
		}
	}

	if (NO_PROMOTION != kPromotion.getPrereqPromotion())
	{
		if (!isPromotionValid((PromotionTypes)kPromotion.getPrereqPromotion()))
		{
			return false;
		}
	}

	PromotionTypes ePrereq1 = (PromotionTypes)kPromotion.getPrereqOrPromotion1();
	PromotionTypes ePrereq2 = (PromotionTypes)kPromotion.getPrereqOrPromotion2();
	if (NO_PROMOTION != ePrereq1 || NO_PROMOTION != ePrereq2)
	{
		bool bValid = false;
		if (!bValid)
		{
			if (NO_PROMOTION != ePrereq1 && isPromotionValid(ePrereq1))
			{
				bValid = true;
			}
		}

		if (!bValid)
		{
			if (NO_PROMOTION != ePrereq2 && isPromotionValid(ePrereq2))
			{
				bValid = true;
			}
		}

		if (!bValid)
		{
			return false;
		}
	}

	if (getUnitCombatType() == NO_UNITCOMBAT)
	{
		return false;
	}

	if (!kPromotion.getUnitCombat(getUnitCombatType()))
	{
		return false;
	}

	if (kPromotion.getWithdrawalChange() + withdrawalProbability() > GC.getDefineINT("MAX_WITHDRAWAL_PROBABILITY"))
	{
		return false;
	}

	return true;
}


bool CvUnit::canAcquirePromotionAny() const
{
	int iI;

	for (iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		if (canAcquirePromotion((PromotionTypes)iI))
		{
			return true;
		}
	}

	return false;
}


bool CvUnit::isHasPromotion(PromotionTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumPromotionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	CvPromotionInfo& kPromotion = GC.getPromotionInfo(eIndex);

	UnitCombatTypes eUnitCombat = getUnitCombatType();
	if (eUnitCombat == NO_UNITCOMBAT)
	{
		return false;
	}

	if (!kPromotion.getUnitCombat(eUnitCombat))
	{
		return false;
	}

	if (getFreePromotionCount(eIndex) <= 0 && !isHasRealPromotion(eIndex))
	{
		return false;
	}

	return true;
}

bool CvUnit::isHasRealPromotion(PromotionTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumPromotionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabHasRealPromotion[eIndex];
}

void CvUnit::setHasRealPromotion(PromotionTypes eIndex, bool bValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumPromotionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (isHasRealPromotion(eIndex) != bValue)
	{
		if (isHasPromotion(eIndex))
		{
			processPromotion(eIndex, -1);
		}

		m_pabHasRealPromotion[eIndex] = bValue;

		if (isHasPromotion(eIndex))
		{
			processPromotion(eIndex, 1);
		}

		if (IsSelected())
		{
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		}
	}
}

void CvUnit::changeFreePromotionCount(PromotionTypes eIndex, int iChange)
{
	if (iChange != 0)
	{
		setFreePromotionCount(eIndex, getFreePromotionCount(eIndex) + iChange);
	}
}

void CvUnit::processPromotion(PromotionTypes ePromotion, int iChange)
{
	changeBlitzCount((GC.getPromotionInfo(ePromotion).isBlitz()) ? iChange : 0);
	changeAmphibCount((GC.getPromotionInfo(ePromotion).isAmphib()) ? iChange : 0);
	changeRiverCount((GC.getPromotionInfo(ePromotion).isRiver()) ? iChange : 0);
	changeEnemyRouteCount((GC.getPromotionInfo(ePromotion).isEnemyRoute()) ? iChange : 0);
	changeAlwaysHealCount((GC.getPromotionInfo(ePromotion).isAlwaysHeal()) ? iChange : 0);
	changeHillsDoubleMoveCount((GC.getPromotionInfo(ePromotion).isHillsDoubleMove()) ? iChange : 0);

	changeExtraVisibilityRange(GC.getPromotionInfo(ePromotion).getVisibilityChange() * iChange);
	changeExtraMoves(GC.getPromotionInfo(ePromotion).getMovesChange() * iChange);
	changeExtraMoveDiscount(GC.getPromotionInfo(ePromotion).getMoveDiscountChange() * iChange);
	changeExtraWithdrawal(GC.getPromotionInfo(ePromotion).getWithdrawalChange() * iChange);
	changeExtraBombardRate(GC.getPromotionInfo(ePromotion).getBombardRateChange() * iChange);
	changeExtraEnemyHeal(GC.getPromotionInfo(ePromotion).getEnemyHealChange() * iChange);
	changeExtraNeutralHeal(GC.getPromotionInfo(ePromotion).getNeutralHealChange() * iChange);
	changeExtraFriendlyHeal(GC.getPromotionInfo(ePromotion).getFriendlyHealChange() * iChange);
	changeSameTileHeal(GC.getPromotionInfo(ePromotion).getSameTileHealChange() * iChange);
	changeAdjacentTileHeal(GC.getPromotionInfo(ePromotion).getAdjacentTileHealChange() * iChange);
	changeExtraCombatPercent(GC.getPromotionInfo(ePromotion).getCombatPercent() * iChange);
	changeExtraCityAttackPercent(GC.getPromotionInfo(ePromotion).getCityAttackPercent() * iChange);
	changeExtraCityDefensePercent(GC.getPromotionInfo(ePromotion).getCityDefensePercent() * iChange);
	changeExtraHillsAttackPercent(GC.getPromotionInfo(ePromotion).getHillsAttackPercent() * iChange);
	changeExtraHillsDefensePercent(GC.getPromotionInfo(ePromotion).getHillsDefensePercent() * iChange);
	changeExtraDomesticBonusPercent(GC.getPromotionInfo(ePromotion).getDomesticBonusPercent() * iChange);	
	changePillageChange(GC.getPromotionInfo(ePromotion).getPillageChange() * iChange);
	changeUpgradeDiscount(GC.getPromotionInfo(ePromotion).getUpgradeDiscount() * iChange);
	changeExperiencePercent(GC.getPromotionInfo(ePromotion).getExperiencePercent() * iChange);
	changeCargoSpace(GC.getPromotionInfo(ePromotion).getCargoChange() * iChange);

	for (int iI = 0; iI < GC.getNumTerrainInfos(); iI++)
	{
		changeExtraTerrainAttackPercent(((TerrainTypes)iI), (GC.getPromotionInfo(ePromotion).getTerrainAttackPercent(iI) * iChange));
		changeExtraTerrainDefensePercent(((TerrainTypes)iI), (GC.getPromotionInfo(ePromotion).getTerrainDefensePercent(iI) * iChange));
		changeTerrainDoubleMoveCount(((TerrainTypes)iI), ((GC.getPromotionInfo(ePromotion).getTerrainDoubleMove(iI)) ? iChange : 0));
	}

	for (int iI = 0; iI < GC.getNumFeatureInfos(); iI++)
	{
		changeExtraFeatureAttackPercent(((FeatureTypes)iI), (GC.getPromotionInfo(ePromotion).getFeatureAttackPercent(iI) * iChange));
		changeExtraFeatureDefensePercent(((FeatureTypes)iI), (GC.getPromotionInfo(ePromotion).getFeatureDefensePercent(iI) * iChange));
		changeFeatureDoubleMoveCount(((FeatureTypes)iI), ((GC.getPromotionInfo(ePromotion).getFeatureDoubleMove(iI)) ? iChange : 0));
	}

	for (int iI = 0; iI < GC.getNumUnitClassInfos(); ++iI)
	{
		changeExtraUnitClassAttackModifier((UnitClassTypes)iI, GC.getPromotionInfo(ePromotion).getUnitClassAttackModifier(iI) * iChange);
		changeExtraUnitClassDefenseModifier((UnitClassTypes)iI, GC.getPromotionInfo(ePromotion).getUnitClassDefenseModifier(iI) * iChange);
	}

	for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
	{
		changeExtraUnitCombatModifier(((UnitCombatTypes)iI), (GC.getPromotionInfo(ePromotion).getUnitCombatModifierPercent(iI) * iChange));
	}

	for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		changeExtraDomainModifier(((DomainTypes)iI), (GC.getPromotionInfo(ePromotion).getDomainModifierPercent(iI) * iChange));
	}
}


void CvUnit::setFreePromotionCount(PromotionTypes eIndex, int iValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumPromotionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(iValue >= 0, "promotion value going negative");

	if (getFreePromotionCount(eIndex) != iValue)
	{
		if (isHasPromotion(eIndex))
		{
			processPromotion(eIndex, -1);
		}

		m_paiFreePromotionCount[eIndex] = iValue;

		if (isHasPromotion(eIndex))
		{
			processPromotion(eIndex, 1);
		}

		if (IsSelected())
		{
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		}
	}
}

int CvUnit::getFreePromotionCount(PromotionTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumPromotionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiFreePromotionCount[eIndex];
}

int CvUnit::getSubUnitCount() const
{
	return m_pUnitInfo->getGroupSize(getProfession());
}


int CvUnit::getSubUnitsAlive() const
{
	return getSubUnitsAlive( getDamage());
}


int CvUnit::getSubUnitsAlive(int iDamage) const
{
	if (iDamage >= maxHitPoints())
	{
		return 0;
	}
	else
	{
		return std::max(1, (((m_pUnitInfo->getGroupSize(getProfession()) * (maxHitPoints() - iDamage)) + (maxHitPoints() / ((m_pUnitInfo->getGroupSize(getProfession()) * 2) + 1))) / maxHitPoints()));
	}
}
// returns true if unit can initiate a war action with plot (possibly by declaring war)
bool CvUnit::potentialWarAction(const CvPlot* pPlot) const
{
	TeamTypes ePlotTeam = pPlot->getTeam();
	TeamTypes eUnitTeam = getTeam();

	if (ePlotTeam == NO_TEAM)
	{
		return false;
	}

	if (isEnemy(ePlotTeam, pPlot))
	{
		return true;
	}

	if (getGroup()->AI_isDeclareWar(pPlot) && GET_TEAM(eUnitTeam).AI_getWarPlan(ePlotTeam) != NO_WARPLAN)
	{
		return true;
	}

	return false;
}

void CvUnit::read(FDataStreamBase* pStream)
{
	// Init data before load
	reset();

	uint uiFlag=0;
	pStream->Read(&uiFlag);	// flags for expansion

	pStream->Read(&m_iID);
	pStream->Read(&m_iGroupID);
	pStream->Read(&m_iHotKeyNumber);
	pStream->Read(&m_iX);
	pStream->Read(&m_iY);
	pStream->Read(&m_iLastMoveTurn);
	pStream->Read(&m_iGameTurnCreated);
	pStream->Read(&m_iDamage);
	pStream->Read(&m_iMoves);
	pStream->Read(&m_iExperience);
	pStream->Read(&m_iLevel);
	pStream->Read(&m_iCargo);
	pStream->Read(&m_iCargoCapacity);
	pStream->Read(&m_iAttackPlotX);
	pStream->Read(&m_iAttackPlotY);
	pStream->Read(&m_iCombatTimer);
	pStream->Read(&m_iCombatDamage);
	pStream->Read(&m_iFortifyTurns);
	pStream->Read(&m_iBlitzCount);
	pStream->Read(&m_iAmphibCount);
	pStream->Read(&m_iRiverCount);
	pStream->Read(&m_iEnemyRouteCount);
	pStream->Read(&m_iAlwaysHealCount);
	pStream->Read(&m_iHillsDoubleMoveCount);
	pStream->Read(&m_iExtraVisibilityRange);
	pStream->Read(&m_iExtraMoves);
	pStream->Read(&m_iExtraMoveDiscount);
	pStream->Read(&m_iExtraWithdrawal);
	pStream->Read(&m_iExtraBombardRate);
	pStream->Read(&m_iExtraEnemyHeal);
	pStream->Read(&m_iExtraNeutralHeal);
	pStream->Read(&m_iExtraFriendlyHeal);
	pStream->Read(&m_iSameTileHeal);
	pStream->Read(&m_iAdjacentTileHeal);
	pStream->Read(&m_iExtraCombatPercent);
	pStream->Read(&m_iExtraCityAttackPercent);
	pStream->Read(&m_iExtraCityDefensePercent);
	pStream->Read(&m_iExtraHillsAttackPercent);
	pStream->Read(&m_iExtraHillsDefensePercent);
	pStream->Read(&m_iExtraDomesticBonusPercent);	
	pStream->Read(&m_iPillageChange);
	pStream->Read(&m_iUpgradeDiscount);
	pStream->Read(&m_iExperiencePercent);
	pStream->Read(&m_iBaseCombat);
	pStream->Read((int*)&m_eFacingDirection);
	pStream->Read(&m_iImmobileTimer);
	pStream->Read(&m_iYieldStored);
	pStream->Read(&m_iExtraWorkRate);
	pStream->Read((int*)&m_eProfession);

	// TAC - LbD - Ray - START
	pStream->Read(&m_LbDrounds);
	pStream->Read((int*)&m_lastProfession);
	// TAC - LbD - Ray - END

	//ray18
	pStream->Read(&moneyToBuyLand);
	pStream->Read((int*)&playerToBuyLand);	
	//Ende ray18

	// R&R, ray, Natives Trading - START
	pStream->Read(&m_AmountForNativeTrade);
	pStream->Read((int*)&m_YieldForNativeTrade);
	// R&R, ray, Natives Trading - END

	pStream->Read(&m_iUnitTravelTimer);
	pStream->Read(&m_iBadCityDefenderCount);
	pStream->Read(&m_iUnarmedCount);
	pStream->Read((int*)&m_eUnitTravelState);

	pStream->Read(&m_bMadeAttack);
	pStream->Read(&m_bPromotionReady);
	pStream->Read(&m_bDeathDelay);
	pStream->Read(&m_bCombatFocus);
	// m_bInfoBarDirty not saved...
	pStream->Read(&m_bColonistLocked);

	pStream->Read(&m_bGatheringResource); //TAC Whaling, ray
	
	pStream->Read(&m_bIgnoreDanger); // TAC - Trade Routes Advisor - koma13 - START
	// < JAnimals Mod Start >
	pStream->Read(&m_bBarbarian);
	// < JAnimals Mod End >
	pStream->Read((int*)&m_eOwner);
	pStream->Read((int*)&m_eCapturingPlayer);
	pStream->Read((int*)&m_eUnitType);
	FAssert(NO_UNIT != m_eUnitType);
	m_pUnitInfo = (NO_UNIT != m_eUnitType) ? &GC.getUnitInfo(m_eUnitType) : NULL;
	pStream->Read((int*)&m_eLeaderUnitType);

	m_combatUnit.read(pStream);
	pStream->Read(&m_iPostCombatPlotIndex);
	m_transportUnit.read(pStream);
	m_homeCity.read(pStream);

	pStream->Read(NUM_DOMAIN_TYPES, m_aiExtraDomainModifier);

	pStream->ReadString(m_szName);
	pStream->ReadString(m_szScriptData);

	pStream->Read(GC.getNumPromotionInfos(), m_pabHasRealPromotion);
	pStream->Read(GC.getNumPromotionInfos(), m_paiFreePromotionCount);
	pStream->Read(GC.getNumTerrainInfos(), m_paiTerrainDoubleMoveCount);
	pStream->Read(GC.getNumFeatureInfos(), m_paiFeatureDoubleMoveCount);
	pStream->Read(GC.getNumTerrainInfos(), m_paiExtraTerrainAttackPercent);
	pStream->Read(GC.getNumTerrainInfos(), m_paiExtraTerrainDefensePercent);
	pStream->Read(GC.getNumFeatureInfos(), m_paiExtraFeatureAttackPercent);
	pStream->Read(GC.getNumFeatureInfos(), m_paiExtraFeatureDefensePercent);
	pStream->Read(GC.getNumUnitClassInfos(), m_paiExtraUnitClassAttackModifier);
	pStream->Read(GC.getNumUnitClassInfos(), m_paiExtraUnitClassDefenseModifier);
	pStream->Read(GC.getNumUnitCombatInfos(), m_paiExtraUnitCombatModifier);

	// unit yield cache - start - Nightinggale
	updateYieldCache();
	// unit yield cache - end - Nightinggale
}


void CvUnit::write(FDataStreamBase* pStream)
{
	uint uiFlag=0;
	pStream->Write(uiFlag);		// flag for expansion

	pStream->Write(m_iID);
	pStream->Write(m_iGroupID);
	pStream->Write(m_iHotKeyNumber);
	pStream->Write(m_iX);
	pStream->Write(m_iY);
	pStream->Write(m_iLastMoveTurn);
	pStream->Write(m_iGameTurnCreated);
	pStream->Write(m_iDamage);
	pStream->Write(m_iMoves);
	pStream->Write(m_iExperience);
	pStream->Write(m_iLevel);
	pStream->Write(m_iCargo);
	pStream->Write(m_iCargoCapacity);
	pStream->Write(m_iAttackPlotX);
	pStream->Write(m_iAttackPlotY);
	pStream->Write(m_iCombatTimer);
	pStream->Write(m_iCombatDamage);
	pStream->Write(m_iFortifyTurns);
	pStream->Write(m_iBlitzCount);
	pStream->Write(m_iAmphibCount);
	pStream->Write(m_iRiverCount);
	pStream->Write(m_iEnemyRouteCount);
	pStream->Write(m_iAlwaysHealCount);
	pStream->Write(m_iHillsDoubleMoveCount);
	pStream->Write(m_iExtraVisibilityRange);
	pStream->Write(m_iExtraMoves);
	pStream->Write(m_iExtraMoveDiscount);
	pStream->Write(m_iExtraWithdrawal);
	pStream->Write(m_iExtraBombardRate);
	pStream->Write(m_iExtraEnemyHeal);
	pStream->Write(m_iExtraNeutralHeal);
	pStream->Write(m_iExtraFriendlyHeal);
	pStream->Write(m_iSameTileHeal);
	pStream->Write(m_iAdjacentTileHeal);
	pStream->Write(m_iExtraCombatPercent);
	pStream->Write(m_iExtraCityAttackPercent);
	pStream->Write(m_iExtraCityDefensePercent);
	pStream->Write(m_iExtraHillsAttackPercent);
	pStream->Write(m_iExtraHillsDefensePercent);
	pStream->Write(m_iExtraDomesticBonusPercent);	
	pStream->Write(m_iPillageChange);
	pStream->Write(m_iUpgradeDiscount);
	pStream->Write(m_iExperiencePercent);
	pStream->Write(m_iBaseCombat);
	pStream->Write(m_eFacingDirection);
	pStream->Write(m_iImmobileTimer);
	pStream->Write(m_iYieldStored);
	pStream->Write(m_iExtraWorkRate);
	pStream->Write(m_eProfession);

	// TAC - LbD - Ray - START
	pStream->Write(m_LbDrounds);
	pStream->Write(m_lastProfession);
	// TAC - LbD - Ray - END

	//ray18
	pStream->Write(moneyToBuyLand);
	pStream->Write(playerToBuyLand);
	//Ende ray18

	// R&R, ray, Natives Trading - START
	pStream->Write(m_AmountForNativeTrade);
	pStream->Write(m_YieldForNativeTrade);
	// R&R, ray, Natives Trading -END

	pStream->Write(m_iUnitTravelTimer);
	pStream->Write(m_iBadCityDefenderCount);
	pStream->Write(m_iUnarmedCount);
	pStream->Write(m_eUnitTravelState);

	pStream->Write(m_bMadeAttack);
	pStream->Write(m_bPromotionReady);
	pStream->Write(m_bDeathDelay);
	pStream->Write(m_bCombatFocus);
	// m_bInfoBarDirty not saved...
	pStream->Write(m_bColonistLocked);

	pStream->Write(m_bGatheringResource); //TAC Whaling, ray

	pStream->Write(m_bIgnoreDanger); // TAC - Trade Routes Advisor - koma13 - START
	// < JAnimals Mod Start >
	pStream->Write(m_bBarbarian);
	// < JAnimals Mod End >
	pStream->Write(m_eOwner);
	pStream->Write(m_eCapturingPlayer);
	pStream->Write(m_eUnitType);
	pStream->Write(m_eLeaderUnitType);

	m_combatUnit.write(pStream);
	pStream->Write(m_iPostCombatPlotIndex);
	m_transportUnit.write(pStream);
	m_homeCity.write(pStream);

	pStream->Write(NUM_DOMAIN_TYPES, m_aiExtraDomainModifier);

	pStream->WriteString(m_szName);
	pStream->WriteString(m_szScriptData);

	pStream->Write(GC.getNumPromotionInfos(), m_pabHasRealPromotion);
	pStream->Write(GC.getNumPromotionInfos(), m_paiFreePromotionCount);
	pStream->Write(GC.getNumTerrainInfos(), m_paiTerrainDoubleMoveCount);
	pStream->Write(GC.getNumFeatureInfos(), m_paiFeatureDoubleMoveCount);
	pStream->Write(GC.getNumTerrainInfos(), m_paiExtraTerrainAttackPercent);
	pStream->Write(GC.getNumTerrainInfos(), m_paiExtraTerrainDefensePercent);
	pStream->Write(GC.getNumFeatureInfos(), m_paiExtraFeatureAttackPercent);
	pStream->Write(GC.getNumFeatureInfos(), m_paiExtraFeatureDefensePercent);
	pStream->Write(GC.getNumUnitClassInfos(), m_paiExtraUnitClassAttackModifier);
	pStream->Write(GC.getNumUnitClassInfos(), m_paiExtraUnitClassDefenseModifier);
	pStream->Write(GC.getNumUnitCombatInfos(), m_paiExtraUnitCombatModifier);
}

// Protected Functions...

bool CvUnit::canAdvance(const CvPlot* pPlot, int iThreshold) const
{
	FAssert(canFight());
	FAssert(getDomainType() != DOMAIN_IMMOBILE);

	if (pPlot->getNumVisibleEnemyDefenders(this) > iThreshold)
	{
		return false;
	}

	if (isNoCityCapture() && pPlot->isEnemyCity(*this))
	{
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------
// FUNCTION:    CvUnit::planBattle
//! \brief      Determines in general how a battle will progress.
//!
//!				Note that the outcome of the battle is not determined here. This function plans
//!				how many sub-units die and in which 'rounds' of battle.
//! \param      kBattleDefinition The battle definition, which receives the battle plan.
//! \retval     The number of game turns that the battle should be given.
//------------------------------------------------------------------------------------------------
int CvUnit::planBattle( CvBattleDefinition & kBattleDefinition ) const
{
#define BATTLE_TURNS_SETUP 4
#define BATTLE_TURNS_ENDING 4
#define BATTLE_TURNS_MELEE 6
#define BATTLE_TURNS_RANGED 6
#define BATTLE_TURN_RECHECK 4

	int								aiUnitsBegin[BATTLE_UNIT_COUNT];
	int								aiUnitsEnd[BATTLE_UNIT_COUNT];
	int								aiToKillMelee[BATTLE_UNIT_COUNT];
	int								aiToKillRanged[BATTLE_UNIT_COUNT];
	CvBattleRoundVector::iterator	iIterator;
	int								i, j;
	bool							bIsLoser;
	int								iRoundIndex;
	int								iRoundCheck = BATTLE_TURN_RECHECK;

	// Initial conditions
	kBattleDefinition.setNumRangedRounds(0);
	kBattleDefinition.setNumMeleeRounds(0);

	increaseBattleRounds( kBattleDefinition);

	// Keep randomizing until we get something valid
	do
	{
		iRoundCheck++;
		if (( iRoundCheck >= BATTLE_TURN_RECHECK ) && !kBattleDefinition.isOneStrike())
		{
			increaseBattleRounds( kBattleDefinition);
			iRoundCheck = 0;
		}

		// Make sure to clear the battle plan, we may have to do this again if we can't find a plan that works.
		kBattleDefinition.clearBattleRounds();

		// Create the round list
		CvBattleRound kRound;
		int iTotalRounds = kBattleDefinition.getNumRangedRounds() + kBattleDefinition.getNumMeleeRounds();
		kBattleDefinition.setBattleRound(iTotalRounds, kRound);

		// For the attacker and defender
		for ( i = 0; i < BATTLE_UNIT_COUNT; i++ )
		{
			// Gather some initial information
			BattleUnitTypes unitType = (BattleUnitTypes) i;
			aiUnitsBegin[unitType] = kBattleDefinition.getUnit(unitType)->getSubUnitsAlive(kBattleDefinition.getDamage(unitType, BATTLE_TIME_BEGIN));
			aiToKillRanged[unitType] = computeUnitsToDie( kBattleDefinition, true, unitType);
			aiToKillMelee[unitType] = computeUnitsToDie( kBattleDefinition, false, unitType);
			aiUnitsEnd[unitType] = aiUnitsBegin[unitType] - aiToKillMelee[unitType] - aiToKillRanged[unitType];

			// Make sure that if they aren't dead at the end, they have at least one unit left
			if ( aiUnitsEnd[unitType] == 0 && !kBattleDefinition.getUnit(unitType)->isDead() )
			{
				aiUnitsEnd[unitType]++;
				if ( aiToKillMelee[unitType] > 0 )
				{
					aiToKillMelee[unitType]--;
				}
				else
				{
					aiToKillRanged[unitType]--;
				}
			}

			// If one unit is the loser, make sure that at least one of their units dies in the last round
			if ( aiUnitsEnd[unitType] == 0 )
			{
				kBattleDefinition.getBattleRound(iTotalRounds - 1).addNumKilled(unitType, 1);
				if ( aiToKillMelee[unitType] > 0)
				{
					aiToKillMelee[unitType]--;
				}
				else
				{
					aiToKillRanged[unitType]--;
				}
			}

			// Randomize in which round each death occurs
			bIsLoser = aiUnitsEnd[unitType] == 0;

			// Randomize the ranged deaths
			for ( j = 0; j < aiToKillRanged[unitType]; j++ )
			{
				iRoundIndex = GC.getGameINLINE().getSorenRandNum( range( kBattleDefinition.getNumRangedRounds(), 0, kBattleDefinition.getNumRangedRounds()), "Ranged combat death");
				kBattleDefinition.getBattleRound(iRoundIndex).addNumKilled(unitType, 1);
			}

			// Randomize the melee deaths
			for ( j = 0; j < aiToKillMelee[unitType]; j++ )
			{
				iRoundIndex = GC.getGameINLINE().getSorenRandNum( range( kBattleDefinition.getNumMeleeRounds() - (bIsLoser ? 1 : 2 ), 0, kBattleDefinition.getNumMeleeRounds()), "Melee combat death");
				kBattleDefinition.getBattleRound(kBattleDefinition.getNumRangedRounds() + iRoundIndex).addNumKilled(unitType, 1);
			}

			// Compute alive sums
			int iNumberKilled = 0;
			for(int j=0;j<kBattleDefinition.getNumBattleRounds();j++)
			{
				CvBattleRound &round = kBattleDefinition.getBattleRound(j);
				round.setRangedRound(j < kBattleDefinition.getNumRangedRounds());
				iNumberKilled += round.getNumKilled(unitType);
				round.setNumAlive(unitType, aiUnitsBegin[unitType] - iNumberKilled);
			}
		}

		// Now compute wave sizes
		for(int i=0;i<kBattleDefinition.getNumBattleRounds();i++)
		{
			CvBattleRound &round = kBattleDefinition.getBattleRound(i);
			round.setWaveSize(computeWaveSize(round.isRangedRound(), round.getNumAlive(BATTLE_UNIT_ATTACKER) + round.getNumKilled(BATTLE_UNIT_ATTACKER), round.getNumAlive(BATTLE_UNIT_DEFENDER) + round.getNumKilled(BATTLE_UNIT_DEFENDER)));
		}

		if ( iTotalRounds > 400 )
		{
			kBattleDefinition.setNumMeleeRounds(1);
			kBattleDefinition.setNumRangedRounds(0);
			break;
		}
	}
	while ( !verifyRoundsValid( kBattleDefinition ) && !kBattleDefinition.isOneStrike());

	//add a little extra time for leader to surrender
	bool attackerLeader = false;
	bool defenderLeader = false;
	bool attackerDie = false;
	bool defenderDie = false;
	int lastRound = kBattleDefinition.getNumBattleRounds() - 1;
	if(kBattleDefinition.getUnit(BATTLE_UNIT_ATTACKER)->getLeaderUnitType() != NO_UNIT)
		attackerLeader = true;
	if(kBattleDefinition.getUnit(BATTLE_UNIT_DEFENDER)->getLeaderUnitType() != NO_UNIT)
		defenderLeader = true;
	if(kBattleDefinition.getBattleRound(lastRound).getNumAlive(BATTLE_UNIT_ATTACKER) == 0)
		attackerDie = true;
	if(kBattleDefinition.getBattleRound(lastRound).getNumAlive(BATTLE_UNIT_DEFENDER) == 0)
		defenderDie = true;

	int extraTime = 0;
	if((attackerLeader && attackerDie) || (defenderLeader && defenderDie))
		extraTime = BATTLE_TURNS_MELEE;

	return BATTLE_TURNS_SETUP + BATTLE_TURNS_ENDING + kBattleDefinition.getNumMeleeRounds() * BATTLE_TURNS_MELEE + kBattleDefinition.getNumRangedRounds() * BATTLE_TURNS_MELEE + extraTime;
}

//------------------------------------------------------------------------------------------------
// FUNCTION:	CvBattleManager::computeDeadUnits
//! \brief		Computes the number of units dead, for either the ranged or melee portion of combat.
//! \param		kDefinition The battle definition.
//! \param		bRanged true if computing the number of units that die during the ranged portion of combat,
//!					false if computing the number of units that die during the melee portion of combat.
//! \param		iUnit The index of the unit to compute (BATTLE_UNIT_ATTACKER or BATTLE_UNIT_DEFENDER).
//! \retval		The number of units that should die for the given unit in the given portion of combat
//------------------------------------------------------------------------------------------------
int CvUnit::computeUnitsToDie( const CvBattleDefinition & kDefinition, bool bRanged, BattleUnitTypes iUnit ) const
{
	FAssertMsg( iUnit == BATTLE_UNIT_ATTACKER || iUnit == BATTLE_UNIT_DEFENDER, "Invalid unit index");

	BattleTimeTypes iBeginIndex = bRanged ? BATTLE_TIME_BEGIN : BATTLE_TIME_RANGED;
	BattleTimeTypes iEndIndex = bRanged ? BATTLE_TIME_RANGED : BATTLE_TIME_END;
	return kDefinition.getUnit(iUnit)->getSubUnitsAlive(kDefinition.getDamage(iUnit, iBeginIndex)) -
		kDefinition.getUnit(iUnit)->getSubUnitsAlive( kDefinition.getDamage(iUnit, iEndIndex));
}

//------------------------------------------------------------------------------------------------
// FUNCTION:    CvUnit::verifyRoundsValid
//! \brief      Verifies that all rounds in the battle plan are valid
//! \param      vctBattlePlan The battle plan
//! \retval     true if the battle plan (seems) valid, false otherwise
//------------------------------------------------------------------------------------------------
bool CvUnit::verifyRoundsValid( const CvBattleDefinition & battleDefinition ) const
{
	for(int i=0;i<battleDefinition.getNumBattleRounds();i++)
	{
		if(!battleDefinition.getBattleRound(i).isValid())
			return false;
	}
	return true;
}

//------------------------------------------------------------------------------------------------
// FUNCTION:    CvUnit::increaseBattleRounds
//! \brief      Increases the number of rounds in the battle.
//! \param      kBattleDefinition The definition of the battle
//------------------------------------------------------------------------------------------------
void CvUnit::increaseBattleRounds( CvBattleDefinition & kBattleDefinition ) const
{
	if(kBattleDefinition.isOneStrike())
	{
		kBattleDefinition.addNumRangedRounds(1);
	}
	else if ( kBattleDefinition.getUnit(BATTLE_UNIT_ATTACKER)->isRanged() && kBattleDefinition.getUnit(BATTLE_UNIT_DEFENDER)->isRanged())
	{
		kBattleDefinition.addNumRangedRounds(1);
	}
	else
	{
		kBattleDefinition.addNumMeleeRounds(1);
	}
}

//------------------------------------------------------------------------------------------------
// FUNCTION:    CvUnit::computeWaveSize
//! \brief      Computes the wave size for the round.
//! \param      bRangedRound true if the round is a ranged round
//! \param		iAttackerMax The maximum number of attackers that can participate in a wave (alive)
//! \param		iDefenderMax The maximum number of Defenders that can participate in a wave (alive)
//! \retval     The desired wave size for the given parameters
//------------------------------------------------------------------------------------------------
int CvUnit::computeWaveSize( bool bRangedRound, int iAttackerMax, int iDefenderMax ) const
{
	FAssertMsg( getCombatUnit() != NULL, "You must be fighting somebody!" );
	int aiDesiredSize[BATTLE_UNIT_COUNT];
	if ( bRangedRound )
	{
		aiDesiredSize[BATTLE_UNIT_ATTACKER] = getUnitInfo().getRangedWaveSize(getProfession());
		aiDesiredSize[BATTLE_UNIT_DEFENDER] = getCombatUnit()->getUnitInfo().getRangedWaveSize(getProfession());
	}
	else
	{
		aiDesiredSize[BATTLE_UNIT_ATTACKER] = getUnitInfo().getMeleeWaveSize(getProfession());
		aiDesiredSize[BATTLE_UNIT_DEFENDER] = getCombatUnit()->getUnitInfo().getMeleeWaveSize(getProfession());
	}

	aiDesiredSize[BATTLE_UNIT_DEFENDER] = aiDesiredSize[BATTLE_UNIT_DEFENDER] <= 0 ? iDefenderMax : aiDesiredSize[BATTLE_UNIT_DEFENDER];
	aiDesiredSize[BATTLE_UNIT_ATTACKER] = aiDesiredSize[BATTLE_UNIT_ATTACKER] <= 0 ? iDefenderMax : aiDesiredSize[BATTLE_UNIT_ATTACKER];
	return std::min( std::min( aiDesiredSize[BATTLE_UNIT_ATTACKER], iAttackerMax ), std::min( aiDesiredSize[BATTLE_UNIT_DEFENDER],
		iDefenderMax) );
}

bool CvUnit::isEnemy(TeamTypes eTeam, const CvPlot* pPlot) const
{
	if (NULL == pPlot)
	{
		pPlot = plot();
	}

	return (::atWar(getCombatTeam(eTeam, pPlot), eTeam));
}

bool CvUnit::isPotentialEnemy(TeamTypes eTeam, const CvPlot* pPlot) const
{
	if (NULL == pPlot)
	{
		pPlot = plot();
	}

	return (::isPotentialEnemy(getCombatTeam(eTeam, pPlot), eTeam));
}

void CvUnit::getDefenderCombatValues(CvUnit& kDefender, const CvPlot* pPlot, int iOurStrength, int iOurFirepower, int& iTheirOdds, int& iTheirStrength, int& iOurDamage, int& iTheirDamage, CombatDetails* pTheirDetails) const
{
	iTheirStrength = kDefender.currCombatStr(pPlot, this, pTheirDetails);
	int iTheirFirepower = kDefender.currFirepower(pPlot, this);

	FAssert((iOurStrength + iTheirStrength) > 0);
	FAssert((iOurFirepower + iTheirFirepower) > 0);

	iTheirOdds = ((GC.getDefineINT("COMBAT_DIE_SIDES") * iTheirStrength) / (iOurStrength + iTheirStrength));
	int iStrengthFactor = ((iOurFirepower + iTheirFirepower + 1) / 2);

	iOurDamage = std::max(1, ((GC.getDefineINT("COMBAT_DAMAGE") * (iTheirFirepower + iStrengthFactor)) / (iOurFirepower + iStrengthFactor)));
	iTheirDamage = std::max(1, ((GC.getDefineINT("COMBAT_DAMAGE") * (iOurFirepower + iStrengthFactor)) / (iTheirFirepower + iStrengthFactor)));
}

int CvUnit::getTriggerValue(EventTriggerTypes eTrigger, const CvPlot* pPlot, bool bCheckPlot) const
{
	CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(eTrigger);
	if (kTrigger.getNumUnits() <= 0)
	{
		return MIN_INT;
	}

	if (!isEmpty(kTrigger.getPythonCanDoUnit()))
	{
		long lResult;

		CyArgsList argsList;
		argsList.add(eTrigger);
		argsList.add(getOwnerINLINE());
		argsList.add(getID());

		gDLL->getPythonIFace()->callFunction(PYRandomEventModule, kTrigger.getPythonCanDoUnit(), argsList.makeFunctionArgs(), &lResult);

		if (0 == lResult)
		{
			return MIN_INT;
		}
	}

	if (kTrigger.getNumUnitsRequired() > 0)
	{
		bool bFoundValid = false;
		for (int i = 0; i < kTrigger.getNumUnitsRequired(); ++i)
		{
			if (getUnitClassType() == kTrigger.getUnitRequired(i))
			{
				bFoundValid = true;
				break;
			}
		}

		if (!bFoundValid)
		{
			return MIN_INT;
		}
	}

	if (bCheckPlot)
	{
		if (kTrigger.isUnitsOnPlot())
		{
			if (!plot()->canTrigger(eTrigger, getOwnerINLINE()))
			{
				return MIN_INT;
			}
		}
	}

	int iValue = 0;

	if (0 == getDamage() && kTrigger.getUnitDamagedWeight() > 0)
	{
		return MIN_INT;
	}

	iValue += getDamage() * kTrigger.getUnitDamagedWeight();

	iValue += getExperience() * kTrigger.getUnitExperienceWeight();

	if (NULL != pPlot)
	{
		iValue += plotDistance(getX_INLINE(), getY_INLINE(), pPlot->getX_INLINE(), pPlot->getY_INLINE()) * kTrigger.getUnitDistanceWeight();
	}

	return iValue;
}

bool CvUnit::canApplyEvent(EventTypes eEvent) const
{
	CvEventInfo& kEvent = GC.getEventInfo(eEvent);

	if (0 != kEvent.getUnitExperience())
	{
		if (!canAcquirePromotionAny())
		{
			return false;
		}
	}

	if (NO_PROMOTION != kEvent.getUnitPromotion())
	{
		if (!canAcquirePromotion((PromotionTypes)kEvent.getUnitPromotion()))
		{
			return false;
		}
	}

	// R&R, ray, this is stupid non-sense for CivCol
	//if (kEvent.getUnitImmobileTurns() > 0)
	//{
	//	if (!canAttack())
	//	{
	//		return false;
	//	}
	//}

	return true;
}

void CvUnit::applyEvent(EventTypes eEvent)
{
	if (!canApplyEvent(eEvent))
	{
		return;
	}

	CvEventInfo& kEvent = GC.getEventInfo(eEvent);

	if (0 != kEvent.getUnitExperience())
	{
		setDamage(0);
		changeExperience(kEvent.getUnitExperience());
	}

	if (NO_PROMOTION != kEvent.getUnitPromotion())
	{
		setHasRealPromotion((PromotionTypes)kEvent.getUnitPromotion(), true);
	}

	if (kEvent.getUnitImmobileTurns() > 0)
	{
		
		int iImmobileTurns = kEvent.getUnitImmobileTurns();
		int iGrowthPercent = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
		iImmobileTurns *= iGrowthPercent;
		iImmobileTurns /= 100;
		changeImmobileTimer(iImmobileTurns);
		CvWString szText = gDLL->getText("TXT_KEY_EVENT_UNIT_IMMOBILE", getNameOrProfessionKey(),iImmobileTurns);
		gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szText, "AS2D_UNITGIFTED", MESSAGE_TYPE_INFO, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
	}

	CvWString szNameKey(kEvent.getUnitNameKey());

	if (!szNameKey.empty())
	{
		setName(gDLL->getText(kEvent.getUnitNameKey()));
	}

	if (kEvent.isDisbandUnit())
	{
		kill(false);
	}
}

const CvArtInfoUnit* CvUnit::getArtInfo(int i) const
{
	//Androrc UnitArtStyles
//	return m_pUnitInfo->getArtInfo(i, getProfession());
	return m_pUnitInfo->getUnitArtStylesArtInfo(i, getProfession(), (UnitArtStyleTypes) GC.getCivilizationInfo(getCivilizationType()).getUnitArtStyleType());
	//Androrc End
}

const TCHAR* CvUnit::getButton() const
{
	const CvArtInfoUnit* pArtInfo = getArtInfo(0);

	if (NULL != pArtInfo)
	{
		return pArtInfo->getButton();
	}

	return m_pUnitInfo->getButton();
}

const TCHAR* CvUnit::getFullLengthIcon() const
{
	const CvArtInfoUnit* pArtInfo = getArtInfo(0);

	if (NULL != pArtInfo)
	{
		return pArtInfo->getFullLengthIcon();
	}

	return NULL;
}

bool CvUnit::isAlwaysHostile(const CvPlot* pPlot) const
{
	if (!m_pUnitInfo->isAlwaysHostile())
	{
		return false;
	}

	if (NULL != pPlot && pPlot->isCity(true, getTeam()))
	{
		return false;
	}

	return true;
}

bool CvUnit::verifyStackValid()
{
	if (plot()->isVisibleEnemyUnit(this))
	{
		return jumpToNearestValidPlot();
	}

	return true;
}

void CvUnit::setYieldStored(int iYieldAmount)
{
	int iChange = (iYieldAmount - getYieldStored());
	if (iChange != 0)
	{
		FAssert(iYieldAmount >= 0);
		m_iYieldStored = iYieldAmount;


		YieldTypes eYield = getYield();
		if (eYield != NO_YIELD)
		{
			GET_PLAYER(getOwnerINLINE()).changePower(iChange * GC.getYieldInfo(eYield).getPowerValue());
			GET_PLAYER(getOwnerINLINE()).changeAssets(iChange * GC.getYieldInfo(eYield).getAssetValue());
			CvArea* pArea = area();
			if (pArea  != NULL)
			{
				pArea->changePower(getOwnerINLINE(), iChange * GC.getYieldInfo(eYield).getPowerValue());
			}
			if (getYieldStored() == 0)
			{
				kill(true);
			}
		}
		else
		{
			if (!m_pUnitInfo->isTreasure() && getYieldStored() > 0)
			{
				CvPlayer& kPlayer = GET_PLAYER(getOwnerINLINE());
				CvCity* pCity = kPlayer.getPopulationUnitCity(getID());
				if (pCity != NULL)
				{
					if (getYieldStored() >= pCity->educationThreshold())
					{
						if (isHuman())
						{
							// Teacher List - start - Nightinggale
							CvPlayer& kPlayer = GET_PLAYER(GC.getGameINLINE().getActivePlayer());
							std::vector<UnitTypes> ordered_units;
							// make a list of ordered units, where the owner can affort training them.
							for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
							{
								int iPrice = pCity->getSpecialistTuition((UnitTypes) iI);
								if (iPrice >= 0 && iPrice <= kPlayer.getGold())
								{
									UnitTypes eUnitType = (UnitTypes) iI;
									for(int count = 0; count <	pCity->getOrderedStudents(eUnitType); count++)
									{
										// add one for each unit ordered, not just one for each type as a random one is selected in the end.
										ordered_units.push_back(eUnitType);
									}
								}
							}
							
							if (!ordered_units.empty())
							{
								// Train the unit into
								int random_num = ordered_units.size();
								if (random_num == 1)
								{
									// The vector contains only one unit. The "random" unit has to be the first.
									random_num = 0;
								} else {
									random_num = GC.getGameINLINE().getSorenRandNum(random_num, "Pick unit for training");
								}
								pCity->educateStudent(this->getID(), ordered_units[random_num]);
							}
							else
							{
								// no ordered units can be trained
								// original code to open the popup to pick a unit
								CvPopupInfo* pPopupInfo = new CvPopupInfo(BUTTONPOPUP_CHOOSE_EDUCATION, pCity->getID(), getID());
								gDLL->getInterfaceIFace()->addPopup(pPopupInfo, getOwnerINLINE());
							}
							// Teacher List - end - Nightinggale
						}
						else
						{
							pCity->AI_educateStudent(getID());
						}
					}
				}
			}
		}
	}
}

int CvUnit::getYieldStored() const
{
	return m_iYieldStored;
}

YieldTypes CvUnit::getYield() const
// unit yield cache - start - Nightinggale
{
	FAssert(m_eCachedYield == getYieldUncached());
	return m_eCachedYield;
}

void CvUnit::updateYieldCache()
{
	m_eCachedYield = m_pUnitInfo != NULL ? getYieldUncached() : NO_YIELD;
}

YieldTypes CvUnit::getYieldUncached() const
// unit yield cache - end - Nightinggale
{
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		YieldTypes eYield = (YieldTypes) iYield;
		if(getUnitClassType() == GC.getYieldInfo(eYield).getUnitClass())
		{
			return eYield;
		}
	}

	return NO_YIELD;
}

bool CvUnit::isGoods() const
{
	if (getYieldStored() > 0)
	{
		if (m_pUnitInfo->isTreasure())
		{
			return true;
		}

		if (getYield() != NO_YIELD)
		{
			if (GC.getYieldInfo(getYield()).isCargo())
			{
				return true;
			}
		}
	}

	return false;
}


// Private Functions...

//check if quick combat
bool CvUnit::isCombatVisible(const CvUnit* pDefender) const
{
	bool bVisible = false;

	if (!m_pUnitInfo->isQuickCombat())
	{
		if (NULL == pDefender || !pDefender->getUnitInfo().isQuickCombat())
		{
			if (isHuman())
			{
				if (!GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_QUICK_ATTACK))
				{
					bVisible = true;
				}
			}
			else if (NULL != pDefender && pDefender->isHuman())
			{
				if (!GET_PLAYER(pDefender->getOwnerINLINE()).isOption(PLAYEROPTION_QUICK_DEFENSE))
				{
					bVisible = true;
				}
			}
		}
	}

	return bVisible;
}

void CvUnit::changeBadCityDefenderCount(int iChange)
{
	m_iBadCityDefenderCount += iChange;
	FAssert(getBadCityDefenderCount() >= 0);
}

int CvUnit::getBadCityDefenderCount() const
{
	return m_iBadCityDefenderCount;
}

bool CvUnit::isCityDefender() const
{
	return (getBadCityDefenderCount() == 0);
}


void CvUnit::changeUnarmedCount(int iChange)
{
	m_iUnarmedCount += iChange;
	FAssert(getUnarmedCount() >= 0);
}

int CvUnit::getUnarmedCount() const
{
	return m_iUnarmedCount;
}

int CvUnit::getUnitTravelTimer() const
{
	return m_iUnitTravelTimer;
}

void CvUnit::setUnitTravelTimer(int iValue)
{
	m_iUnitTravelTimer = iValue;
	FAssert(getUnitTravelTimer() >= 0);
}

UnitTravelStates CvUnit::getUnitTravelState() const
{
	return m_eUnitTravelState;
}

void CvUnit::setUnitTravelState(UnitTravelStates eState, bool bShowEuropeScreen)
{
	if (getUnitTravelState() != eState)
	{
		CvPlot* pPlot = plot();
		if (pPlot != NULL)
		{
			pPlot->changeAdjacentSight(getTeam(), visibilityRange(), false, this);
		}

		UnitTravelStates eFromState = getUnitTravelState();
		m_eUnitTravelState = eState;

		if (pPlot != NULL)
		{
			if (eFromState == UNIT_TRAVEL_STATE_FROM_EUROPE)
			{
				EuropeTypes eEurope = pPlot->getEurope();
				if (eEurope != NO_EUROPE)
				{
					switch (GC.getEuropeInfo(eEurope).getCardinalDirection())
					{
					case CARDINALDIRECTION_EAST:
						setFacingDirection(DIRECTION_WEST);
						break;
					case CARDINALDIRECTION_WEST:
						setFacingDirection(DIRECTION_EAST);
						break;
					case CARDINALDIRECTION_NORTH:
						setFacingDirection(DIRECTION_SOUTH);
						break;
					case CARDINALDIRECTION_SOUTH:
						setFacingDirection(DIRECTION_NORTH);
						break;
					}
				}
			}
			/*** TRIANGLETRADE 10/28/08 by DPII ***/
			// R&R, ray, modified
			if (eFromState == UNIT_TRAVEL_STATE_FROM_AFRICA)
			{
				EuropeTypes eAfrica = pPlot->getEurope();
				if (eAfrica != NO_EUROPE)
				{
					switch (GC.getEuropeInfo(eAfrica).getCardinalDirection())
					{
					case CARDINALDIRECTION_EAST:
						setFacingDirection(DIRECTION_WEST);
						break;
					case CARDINALDIRECTION_WEST:
						setFacingDirection(DIRECTION_EAST);
						break;
					case CARDINALDIRECTION_NORTH:
						setFacingDirection(DIRECTION_SOUTH);
						break;
					case CARDINALDIRECTION_SOUTH:
						setFacingDirection(DIRECTION_NORTH);
						break;
					}
				}
			}
			/**************************************/
			
			// R&R, ray, Port Royal
			if (eFromState == UNIT_TRAVEL_STATE_FROM_PORT_ROYAL)
			{
				EuropeTypes PortRoyal = pPlot->getEurope();
				if (PortRoyal != NO_EUROPE)
				{
					switch (GC.getEuropeInfo(PortRoyal).getCardinalDirection())
					{
					case CARDINALDIRECTION_EAST:
						setFacingDirection(DIRECTION_WEST);
						break;
					case CARDINALDIRECTION_WEST:
						setFacingDirection(DIRECTION_EAST);
						break;
					case CARDINALDIRECTION_NORTH:
						setFacingDirection(DIRECTION_SOUTH);
						break;
					case CARDINALDIRECTION_SOUTH:
						setFacingDirection(DIRECTION_NORTH);
						break;
					}
				}
			}
			// R&R, ray, Port Royal - END

			pPlot->changeAdjacentSight(getTeam(), visibilityRange(), true, this);

			if (hasCargo())
			{
				for(CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode(); pUnitNode != NULL; pUnitNode = pPlot->nextUnitNode(pUnitNode))
				{
					CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
					if (pLoopUnit->getTransportUnit() == this)
					{
						pLoopUnit->setUnitTravelState(eState, false);
					}
				}
			}
		}

		if (getGroup() != NULL)
		{
			if (!isHuman())
			{
				// Erik: Unconditionally separate all units (all units will be re-assigned to a group with the unit as its single member)
				getGroup()->AI_separate();
			}
			else
			{
				getGroup()->splitGroup(1, this);
			}
		}

		if (!isOnMap())
		{
			if (IsSelected())
			{
				gDLL->getInterfaceIFace()->removeFromSelectionList(this);
			}
		}
		else
		{
			GET_PLAYER(getOwnerINLINE()).updateGroupCycle(this);
		}

		//popup europe screen
		if (bShowEuropeScreen)
		{
			if (getUnitTravelState() == UNIT_TRAVEL_STATE_IN_EUROPE)
			{
				if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
				{
					bool bFound = false;
					const CvPopupQueue& kPopups = GET_PLAYER(getOwnerINLINE()).getPopups();
					for (CvPopupQueue::const_iterator it = kPopups.begin(); it != kPopups.end(); ++it)
					{
						CvPopupInfo* pInfo = *it;
						if (NULL != pInfo)
						{
							if (pInfo->getButtonPopupType() == BUTTONPOPUP_PYTHON_SCREEN && pInfo->getText() == L"showEuropeScreen")
							{
								bFound = true;
								break;
							}
						}
					}

					if(!bFound)
					{
						CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN);
						pInfo->setText(L"showEuropeScreen");
						gDLL->getInterfaceIFace()->addPopup(pInfo, getOwnerINLINE(), false);
					}
				}
			}

			/*** TRIANGLETRADE 10/28/08 by DPII START ***/
			if (getUnitTravelState() == UNIT_TRAVEL_STATE_IN_AFRICA)
			{
				if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
				{
					bool bFound = false;
					const CvPopupQueue& kPopups = GET_PLAYER(getOwnerINLINE()).getPopups();
					for (CvPopupQueue::const_iterator it = kPopups.begin(); it != kPopups.end(); ++it)
					{
						CvPopupInfo* pInfo = *it;
						if (NULL != pInfo)
						{
							if (pInfo->getButtonPopupType() == BUTTONPOPUP_PYTHON_SCREEN && pInfo->getText() == L"showAfricaScreen")
							{
								bFound = true;
								break;
							}
						}
					}

					if(!bFound)
					{
						CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN);
						pInfo->setText(L"showAfricaScreen");
						gDLL->getInterfaceIFace()->addPopup(pInfo, getOwnerINLINE(), false);
					}
				}
			}
			/**************************************/

			// R&R, ray, Port Royal
			if (getUnitTravelState() == UNIT_TRAVEL_STATE_IN_PORT_ROYAL)
			{
				if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
				{
					bool bFound = false;
					const CvPopupQueue& kPopups = GET_PLAYER(getOwnerINLINE()).getPopups();
					for (CvPopupQueue::const_iterator it = kPopups.begin(); it != kPopups.end(); ++it)
					{
						CvPopupInfo* pInfo = *it;
						if (NULL != pInfo)
						{
							if (pInfo->getButtonPopupType() == BUTTONPOPUP_PYTHON_SCREEN && pInfo->getText() == L"showPortRoyalScreen")
							{
								bFound = true;
								break;
							}
						}
					}

					if(!bFound)
					{
						CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN);
						pInfo->setText(L"showPortRoyalScreen");
						gDLL->getInterfaceIFace()->addPopup(pInfo, getOwnerINLINE(), false);
					}
				}
			}
			// R&R, ray, Port Royal- END
		}

		if (getOwnerINLINE() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(EuropeScreen_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(AfricaScreen_DIRTY_BIT, true); /*** TRIANGLETRADE 10/28/08 by DPII ***/
			gDLL->getInterfaceIFace()->setDirty(PortRoyalScreen_DIRTY_BIT, true); // R&R, ray, Port Royal
		}

		gDLL->getEventReporterIFace()->unitTravelStateChanged(getOwnerINLINE(), eState, getID());

		if (pPlot != NULL)
		{
			pPlot->updateCenterUnit();
		}
	}
}

bool CvUnit::setSailEurope(EuropeTypes eEurope)
{
	CvPlot* pBestPlot = NULL;

	if (eEurope == NO_EUROPE)
	{
		return true;
	}

	if (plot()->getEurope() == eEurope)
	{
		return true;
	}

	CvPlayerAI& kLoopPlayer = GET_PLAYER(getOwnerINLINE());
	for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		CvPlot* pPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
		int iAvgDistance = 0;
		int iBestDistance = 100000;
		if (pPlot->getEurope() == eEurope)
		{
			if (pPlot->isRevealed(getTeam(), false))
			{
				if (kLoopPlayer.getNumCities() > 0)
				{
					int iLoop;
					for (CvCity* pLoopCity = kLoopPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kLoopPlayer.nextCity(&iLoop))
					{
						iAvgDistance += stepDistance(pPlot->getX_INLINE(), pPlot->getY_INLINE(), pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE());
					}
				}
				else
				{
					iAvgDistance += stepDistance(pPlot->getX_INLINE(), pPlot->getY_INLINE(), kLoopPlayer.getStartingPlot()->getX_INLINE(), kLoopPlayer.getStartingPlot()->getY_INLINE());
				}

				if (iAvgDistance > 0 && iAvgDistance < iBestDistance)
				{
					iBestDistance = iAvgDistance;
					pBestPlot = pPlot;
				}
			}
		}
	}
	if (pBestPlot != NULL)
	{
		setXY(pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE());
		return true;
	}
	return false;
}

bool CvUnit::canSailEurope(EuropeTypes eEurope) const
{
	if (eEurope == NO_EUROPE)
	{
		return true;
	}

	if (plot()->getEurope() == eEurope)
	{
		return true;
	}

	for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		CvPlot* pPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
		if (pPlot->isRevealed(getTeam(), false))
		{
			if (pPlot->getEurope() == eEurope)
			{
				return true;
			}
		}
	}
	return false;
}

void CvUnit::setHomeCity(CvCity* pNewValue)
{
	if (pNewValue == NULL)
	{
		m_homeCity.reset();
	}
	else
	{
		if (AI_getUnitAIType() == UNITAI_WORKER)
		{
			CvCity* pExistingCity = getHomeCity();
			if (pExistingCity != NULL && pExistingCity != pNewValue)
			{
				getHomeCity()->AI_changeWorkersHave(-1);
			}
			pNewValue->AI_changeWorkersHave(+1);
		}
		m_homeCity = pNewValue->getIDInfo();
	}
}

CvCity* CvUnit::getHomeCity() const
{
	return ::getCity(m_homeCity);
}

bool CvUnit::isOnMap() const
{
	if (getUnitTravelState() != NO_UNIT_TRAVEL_STATE)
	{
		return false;
	}

	if((getX_INLINE() == INVALID_PLOT_COORD) || (getY_INLINE() == INVALID_PLOT_COORD))
	{
		return false;
	}

	return true;
}


void CvUnit::doUnitTravelTimer()
{
	if (getUnitTravelTimer() > 0)
	{
		setUnitTravelTimer(getUnitTravelTimer() - 1);

		if (getUnitTravelTimer() == 0)
		{
			switch (getUnitTravelState())
			{
			case UNIT_TRAVEL_STATE_FROM_EUROPE:
				setUnitTravelState(NO_UNIT_TRAVEL_STATE, false);
				break;
			case UNIT_TRAVEL_STATE_TO_EUROPE:
				setUnitTravelState(UNIT_TRAVEL_STATE_IN_EUROPE, true);
				break;
			case UNIT_TRAVEL_STATE_LIVE_AMONG_NATIVES:
				setUnitTravelState(NO_UNIT_TRAVEL_STATE, false);
				doLearn();
				break;
			/*** TRIANGLETRADE 10/25/08 by DPII ***/
			case UNIT_TRAVEL_STATE_FROM_AFRICA:
				setUnitTravelState(NO_UNIT_TRAVEL_STATE, false);
				break;
			case UNIT_TRAVEL_STATE_TO_AFRICA:
				setUnitTravelState(UNIT_TRAVEL_STATE_IN_AFRICA, true);
				break;
			/**************************************/
			// R&R, ray, Port Royal
			case UNIT_TRAVEL_STATE_FROM_PORT_ROYAL:
				setUnitTravelState(NO_UNIT_TRAVEL_STATE, false);
				break;
			case UNIT_TRAVEL_STATE_TO_PORT_ROYAL:
				setUnitTravelState(UNIT_TRAVEL_STATE_IN_PORT_ROYAL, true);
				break;
			// R&R, ray, Port Royal - END
			default:
				FAssertMsg(false, "Unit arriving from nowhere");
				break;
			}
		}
	}
}

bool CvUnit::isColonistLocked()
{
	return m_bColonistLocked;
}

void CvUnit::setColonistLocked(bool bNewValue)
{
	if (m_bColonistLocked != bNewValue)
	{
		m_bColonistLocked = bNewValue;

		if (bNewValue == true)
		{
			CvCity* pCity = GET_PLAYER(getOwnerINLINE()).getPopulationUnitCity(getID());

			FAssert(pCity != NULL);

			CvPlot* pPlot = pCity->getPlotWorkedByUnit(this);

			if (pPlot != NULL)
			{
				//Ensure it is not stolen.
				pPlot->setWorkingCityOverride(pCity);
			}
		}
	}
}

// TAC - Trade Routes Advisor - koma13 - START
bool CvUnit::isIgnoreDanger() const
{
	return m_bIgnoreDanger;
}

void CvUnit::setIgnoreDanger(bool bNewValue)
{
	if (m_bIgnoreDanger != bNewValue)
	{
		m_bIgnoreDanger = bNewValue;
	}
}
// TAC - Trade Routes Advisor - koma13 - END

// < JAnimals Mod Start >
bool CvUnit::isBarbarian() const
{
	return m_bBarbarian;
}

void CvUnit::setBarbarian(bool bNewValue)
{
    if (bNewValue != isBarbarian())
    {
        m_bBarbarian = bNewValue;
    }
}
// < JAnimals Mod End >

bool CvUnit::raidWeapons(std::vector<int>& aYields)
{
	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());
	ProfessionTypes eCurrentProfession = getProfession();
	std::vector<ProfessionTypes> aProfessions;
	for (int iProfession = 0; iProfession < GC.getNumProfessionInfos(); ++iProfession)
	{
		ProfessionTypes eProfession = (ProfessionTypes) iProfession;
		
		//ray15 
		//if (canHaveProfession(eProfession, false))
		if (kOwner.isProfessionValid(eProfession, getUnitType()))
		{
			//ray15 here only the if statement modified
			if (eCurrentProfession == NO_PROFESSION || GC.getProfessionInfo(eProfession).getCombatChange() > GC.getProfessionInfo(eCurrentProfession).getCombatChange() || (GC.getProfessionInfo(eProfession).getCombatChange() >= GC.getProfessionInfo(eCurrentProfession).getCombatChange() && GC.getProfessionInfo(eProfession).getMovesChange() > GC.getProfessionInfo(eCurrentProfession).getMovesChange()) )
			{
				bool bCanHaveProfession = false;
				for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
				{
					YieldTypes eYield = (YieldTypes) iYield;
					//ray Korrektur negativer Warenbestand
					//int iYieldRequired = kOwner.getYieldEquipmentAmount(eProfession, eYield);
					int iYieldRequired = GC.getProfessionInfo(eProfession).getYieldEquipmentAmount(eYield);
					if (iYieldRequired > 0)
					{
						bCanHaveProfession = true;
						if (eCurrentProfession != NO_PROFESSION)
						{
							iYieldRequired -= kOwner.getYieldEquipmentAmount(eCurrentProfession, eYield);
						}
						
						//ray Korrektur negativer Warenbestand
						//if (iYieldRequired > 0 && aYields[iYield] == 0)
						if (iYieldRequired > aYields[iYield])
						{
							bCanHaveProfession = false;
							break;
						}
					}
				}

				if (bCanHaveProfession)
				{
					aProfessions.push_back(eProfession);
				}
			}
		}
	}

	if (aProfessions.empty())
	{
		return false;
	}

	ProfessionTypes eProfession = aProfessions[GC.getGameINLINE().getSorenRandNum(aProfessions.size(), "Choose raid weapons")];

	// R&R, ray, corrected small bug - START
	// Force Porfession Change
	// but first make sure, that if in case of Native defending own city no negative yields occur
	CvCity* pCity = plot()->getPlotCity();
	if (pCity != NULL && pCity->isNative())
	{
		for (int zYield = 0; zYield < NUM_YIELD_TYPES; ++zYield)
		{
			YieldTypes tYield = (YieldTypes) zYield;
			pCity->changeYieldStored(tYield, aYields[zYield]);
		}
	}
	setProfession(eProfession, true);
	// R&R, ray, corrected small bug - END

	return true;

}


bool CvUnit::raidWeapons(CvCity* pCity)
{
	if (!isNative())
	{
		return false;
	}

	if (!isEnemy(pCity->getTeam()))
	{
		return false;
	}

	if (GC.getGameINLINE().getSorenRandNum(200, "Weapons raid") < pCity->getDefenseModifier())
	{
		return false;
	}

	std::vector<int> aYields(NUM_YIELD_TYPES);
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		// R&R, ray, code improvement - START
		YieldTypes cYield = (YieldTypes) iYield;
		if(cYield  == YIELD_HORSES || cYield  == YIELD_MUSKETS)
		{
			aYields[iYield] = pCity->getYieldStored(cYield);
		}
		else
		{
			aYields[iYield] = 0;
		}
		// R&R, ray, code improvement - END
	}

	if (!raidWeapons(aYields))
	{
		return false;
	}

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		YieldTypes eYield = (YieldTypes) iYield;
		if (aYields[iYield] > 0)
		{
			pCity->changeYieldStored(eYield, -aYields[iYield]);

			CvWString szString = gDLL->getText("TXT_KEY_GOODS_RAIDED", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), pCity->getNameKey(), aYields[iYield], GC.getYieldInfo(eYield).getTextKeyWide());
			gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, GC.getYieldInfo(eYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pCity->getX_INLINE(), pCity->getY_INLINE());
			gDLL->getInterfaceIFace()->addMessage(pCity->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, GC.getYieldInfo(eYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pCity->getX_INLINE(), pCity->getY_INLINE());
		}
	}
	return true;
}


// R&R, ray, Natives raiding party - START
bool CvUnit::raidWeapons(CvUnit* pUnit)
{
	if (!isNative())
	{
		return false;
	}

	FAssert(pUnit->isDead());

	if (!isEnemy(pUnit->getTeam()))
	{
		if (AI_getUnitAIState() != UNITAI_STATE_RAIDING_PARTY)
		{
			return false;
		}
	}

	std::vector<int> aYields(NUM_YIELD_TYPES, 0);
	CvPlayer& kOwner = GET_PLAYER(pUnit->getOwnerINLINE());
	if (kOwner.hasContentsYieldEquipmentAmountSecure(pUnit->getProfession())) // cache CvPlayer::getYieldEquipmentAmount - Nightinggale
	{


		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			aYields[iYield] += kOwner.getYieldEquipmentAmount(pUnit->getProfession(), (YieldTypes) iYield);
		}
	}

	if (!raidWeapons(aYields))
	{
		return false;
	}

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		YieldTypes eYield = (YieldTypes) iYield;
		if (aYields[iYield] > 0)
		{
			CvWString szString = gDLL->getText("TXT_KEY_WEAPONS_CAPTURED", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), pUnit->getNameOrProfessionKey(), aYields[iYield], GC.getYieldInfo(eYield).getTextKeyWide());
			gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_CITYRAZE", MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(eYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pUnit->getX_INLINE(), pUnit->getY_INLINE(), true, true);
			gDLL->getInterfaceIFace()->addMessage(pUnit->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_CITYRAZE", MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(eYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pUnit->getX_INLINE(), pUnit->getY_INLINE(), true, true);
		}
	}

	return true;
}

bool CvUnit::raidTreasury(CvCity* pCity)
{
	if (!isNative())
	{
		return false;
	}

	if (!isEnemy(pCity->getTeam()))
	{
		if (AI_getUnitAIState() != UNITAI_STATE_RAIDING_PARTY)
		{
			return false;
		}
	}

	if (GC.getGameINLINE().getSorenRandNum(200, "Treasury raid") < pCity->getDefenseModifier())
	{
		return false;
	}

	PlayerTypes eTargetPlayer = pCity->getOwnerINLINE();
	if (NO_PLAYER == eTargetPlayer)
	{
		return false;
	}

	int iNumTotalGold = GET_PLAYER(eTargetPlayer).getGold();
	iNumTotalGold *= pCity->getPopulation();
	iNumTotalGold /= std::max(1, GET_PLAYER(eTargetPlayer).getTotalPopulation());

	if (iNumTotalGold == 0)
	{
		return false;
	}

	GET_PLAYER(getOwnerINLINE()).changeGold(iNumTotalGold);
	if (NO_PLAYER != eTargetPlayer)
	{
		GET_PLAYER(eTargetPlayer).changeGold(-iNumTotalGold);
	}

	GET_TEAM(getTeam()).AI_changeDamages(pCity->getTeam(), -iNumTotalGold);

	CvPlot* pPlot = pCity->plot();
	if (pPlot->isVisible(GC.getGame().getActiveTeam(), false))
	{
		EffectTypes eEffect = GC.getEntityEventInfo(GC.getMissionInfo(MISSION_PILLAGE).getEntityEvent()).getEffectType();
		gDLL->getEngineIFace()->TriggerEffect(eEffect, pPlot->getPoint(), (float)(GC.getASyncRand().get(360)));
		gDLL->getInterfaceIFace()->playGeneralSound("AS3D_UN_CITY_EXPLOSION", pPlot->getPoint());
	}

	CvWString szString = gDLL->getText("TXT_KEY_TREASURY_RAIDED", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), pCity->getNameKey(), iNumTotalGold);
	gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_CITYRAID", MESSAGE_TYPE_MINOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("SCREEN_GOLD_PILE")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pCity->getX_INLINE(), pCity->getY_INLINE(), true, true);
	gDLL->getInterfaceIFace()->addMessage(pCity->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_CITYRAID", MESSAGE_TYPE_MINOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("SCREEN_GOLD_PILE")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pCity->getX_INLINE(), pCity->getY_INLINE(), true, true);

	return true;
}

bool CvUnit::raidBuilding(CvCity* pCity)
{
	if (!isNative())
	{
		return false;
	}

	if (!isEnemy(pCity->getTeam()))
	{
		if (AI_getUnitAIState() != UNITAI_STATE_RAIDING_PARTY)
		{
			return false;
		}
	}

	if (GC.getGameINLINE().getSorenRandNum(200, "Buildings raid") < pCity->getDefenseModifier())
	{
		return false;
	}

	std::vector<BuildingTypes> aBuildings;
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		BuildingTypes eBuilding = (BuildingTypes)iI;
		CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
		// R&R, ray fix for Buildings with workers destoryed
		// only buildings without workers possible will be destroyed
		if (pCity->isHasBuilding(eBuilding) && kBuilding.getMaxWorkers() == 0)
		{
			aBuildings.push_back(eBuilding);
		}
	}

	if (aBuildings.empty())
	{
		return false;
	}

	BuildingTypes eTargetBuilding = aBuildings[GC.getGameINLINE().getSorenRandNum(aBuildings.size(), "Choose raid building")];
	pCity->setHasRealBuilding(eTargetBuilding, false);

	GET_TEAM(getTeam()).AI_changeDamages(pCity->getTeam(), -GC.getBuildingInfo(eTargetBuilding).getAssetValue());

	CvPlot* pPlot = pCity->plot();
	if (pPlot->isVisible(GC.getGame().getActiveTeam(), false))
	{
		EffectTypes eEffect = GC.getEntityEventInfo(GC.getMissionInfo(MISSION_PILLAGE).getEntityEvent()).getEffectType();
		gDLL->getEngineIFace()->TriggerEffect(eEffect, pPlot->getPoint(), (float)(GC.getASyncRand().get(360)));
		gDLL->getInterfaceIFace()->playGeneralSound("AS3D_UN_CITY_EXPLOSION", pPlot->getPoint());
	}

	CvWString szString = gDLL->getText("TXT_KEY_BUILDING_RAIDED", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), pCity->getNameKey(), GC.getBuildingInfo(eTargetBuilding).getTextKeyWide());
	gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_CITYRAID", MESSAGE_TYPE_MINOR_EVENT, GC.getBuildingInfo(eTargetBuilding).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pCity->getX_INLINE(), pCity->getY_INLINE(), true, true);
	gDLL->getInterfaceIFace()->addMessage(pCity->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_CITYRAID", MESSAGE_TYPE_MINOR_EVENT, GC.getBuildingInfo(eTargetBuilding).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pCity->getX_INLINE(), pCity->getY_INLINE(), true, true);

	return true;
}

bool CvUnit::raidProduction(CvCity* pCity)
{
	if (!isNative())
	{
		return false;
	}

	if (!isEnemy(pCity->getTeam()))
	{
		if (AI_getUnitAIState() != UNITAI_STATE_RAIDING_PARTY)
		{
			return false;
		}
	}

	if (GC.getGameINLINE().getSorenRandNum(200, "Production raid") < pCity->getDefenseModifier())
	{
		return false;
	}

	if (!pCity->isProduction() || pCity->isProductionConvince())
	{
		return false;
	}

	if (pCity->isProductionBuilding())
	{
		BuildingTypes eTarget = pCity->getProductionBuilding();
		int iProductionTurnLeft = pCity->getProductionTurnsLeft(eTarget, 0);

		if (pCity->getBuildingProduction(eTarget) == 0)
		{
			return false;
		}

		pCity->setBuildingProduction(eTarget, 0);

		GET_TEAM(getTeam()).AI_changeDamages(pCity->getTeam(), -GC.getBuildingInfo(eTarget).getAssetValue());

		CvPlot* pPlot = pCity->plot();
		if (pPlot->isVisible(GC.getGame().getActiveTeam(), false))
		{
			EffectTypes eEffect = GC.getEntityEventInfo(GC.getMissionInfo(MISSION_PILLAGE).getEntityEvent()).getEffectType();
			gDLL->getEngineIFace()->TriggerEffect(eEffect, pPlot->getPoint(), (float)(GC.getASyncRand().get(360)));
			gDLL->getInterfaceIFace()->playGeneralSound("AS3D_UN_CITY_EXPLOSION", pPlot->getPoint());
		}

		CvWString szString = gDLL->getText("TXT_KEY_PRODUCTION_RAIDED", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), pCity->getNameKey(), GC.getBuildingInfo(eTarget).getTextKeyWide());
		gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_CITYRAID", MESSAGE_TYPE_MINOR_EVENT, GC.getBuildingInfo(eTarget).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pCity->getX_INLINE(), pCity->getY_INLINE(), true, true);
		gDLL->getInterfaceIFace()->addMessage(pCity->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_CITYRAID", MESSAGE_TYPE_MINOR_EVENT, GC.getBuildingInfo(eTarget).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pCity->getX_INLINE(), pCity->getY_INLINE(), true, true);
	}
	else if (pCity->isProductionUnit())
	{
		UnitTypes eTarget = pCity->getProductionUnit();
		int iProductionTurnLeft = pCity->getProductionTurnsLeft(eTarget, 0);

		if (pCity->getUnitProduction(eTarget) == 0)
		{
			return false;
		}

		pCity->setUnitProduction(eTarget, 0);

		GET_TEAM(getTeam()).AI_changeDamages(pCity->getTeam(), -GC.getUnitInfo(eTarget).getAssetValue());

		CvPlot* pPlot = pCity->plot();
		if (pPlot->isVisible(GC.getGame().getActiveTeam(), false))
		{
			EffectTypes eEffect = GC.getEntityEventInfo(GC.getMissionInfo(MISSION_PILLAGE).getEntityEvent()).getEffectType();
			gDLL->getEngineIFace()->TriggerEffect(eEffect, pPlot->getPoint(), (float)(GC.getASyncRand().get(360)));
			gDLL->getInterfaceIFace()->playGeneralSound("AS3D_UN_CITY_EXPLOSION", pPlot->getPoint());
		}

		CvWString szString = gDLL->getText("TXT_KEY_PRODUCTION_RAIDED", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), pCity->getNameKey(), GC.getUnitInfo(eTarget).getTextKeyWide());
		gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_CITYRAID", MESSAGE_TYPE_MINOR_EVENT, GC.getUnitInfo(eTarget).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pCity->getX_INLINE(), pCity->getY_INLINE(), true, true);
		gDLL->getInterfaceIFace()->addMessage(pCity->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_CITYRAID", MESSAGE_TYPE_MINOR_EVENT, GC.getUnitInfo(eTarget).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pCity->getX_INLINE(), pCity->getY_INLINE(), true, true);
	}

	return true;
}

bool CvUnit::raidScalp(CvCity* pCity)
{
	if (!isNative())
	{
		return false;
	}

	if (!isEnemy(pCity->getTeam()))
	{
		if (AI_getUnitAIState() != UNITAI_STATE_RAIDING_PARTY)
		{
			return false;
		}
	}

	if (GC.getGameINLINE().getSorenRandNum(200, "Scalps raid") < pCity->getDefenseModifier())
	{
		return false;
	}

	if (pCity->getPopulation() < 2)
	{
		return false;
	}

	std::vector<CvUnit*> aPopulationUnits;
	for (int iI = 0; iI < pCity->getPopulation(); iI++)
	{
		aPopulationUnits.push_back(pCity->getPopulationUnitByIndex(iI));
	}

	if (aPopulationUnits.empty())
	{
		return false;
	}

	CvUnit* pTargetUnit = aPopulationUnits[GC.getGameINLINE().getSorenRandNum(aPopulationUnits.size(), "Choose raid scalp")];

	if (pTargetUnit == NULL)
	{
		return false;
	}

	CvPlot* pPlot = pCity->plot();
	if (pPlot->isVisible(GC.getGame().getActiveTeam(), false))
	{
		EffectTypes eEffect = GC.getEntityEventInfo(GC.getMissionInfo(MISSION_PILLAGE).getEntityEvent()).getEffectType();
		gDLL->getEngineIFace()->TriggerEffect(eEffect, pPlot->getPoint(), (float)(GC.getASyncRand().get(360)));
		gDLL->getInterfaceIFace()->playGeneralSound("AS3D_UN_CITY_EXPLOSION", pPlot->getPoint());
	}

	CvWString szString = gDLL->getText("TXT_KEY_SCALP_RAIDED", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), pCity->getNameKey(), GC.getUnitInfo(pTargetUnit->getUnitType()).getTextKeyWide());
	gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_CITYRAID", MESSAGE_TYPE_MINOR_EVENT, pTargetUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pCity->getX_INLINE(), pCity->getY_INLINE(), true, true);
	gDLL->getInterfaceIFace()->addMessage(pCity->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_CITYRAID", MESSAGE_TYPE_MINOR_EVENT, pTargetUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pCity->getX_INLINE(), pCity->getY_INLINE(), true, true);

	GET_TEAM(getTeam()).AI_changeDamages(pCity->getTeam(), -pTargetUnit->getAsset());

	pCity->removePopulationUnit(pTargetUnit, false, NO_PROFESSION);
	pTargetUnit->kill(false, this);

	return true;
}

bool CvUnit::raidHarbor(CvCity* pCity)
{
	if (!isNative())
	{
		return false;
	}

	if (!isEnemy(pCity->getTeam()))
	{
		if (AI_getUnitAIState() != UNITAI_STATE_RAIDING_PARTY)
		{
			return false;
		}
	}

	if (GC.getGameINLINE().getSorenRandNum(200, "Harbor raid") < pCity->getDefenseModifier())
	{
		return false;
	}

	CvPlot* pPlot = pCity->plot();
	if (pPlot == NULL)
	{
		return false;
	}

	std::vector<CvUnit*> aShipUnits;

	CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if (pLoopUnit->getDomainType() == DOMAIN_SEA)
		{
			if (pLoopUnit->getOwner() == pCity->getOwner())
			{
				aShipUnits.push_back(pLoopUnit);
			}
		}
	}

	if (aShipUnits.empty())
	{
		return false;
	}

	CvUnit* pTargetUnit = aShipUnits[GC.getGameINLINE().getSorenRandNum(aShipUnits.size(), "Choose raid harbor")];

	if (pTargetUnit == NULL)
	{
		return false;
	}

	int iDamage = GC.getGameINLINE().getSorenRandNum(pTargetUnit->currHitPoints(), "Choose raid harbor");

	if (iDamage < 1)
	{
		return false;
	}

	if (pPlot->isVisible(GC.getGame().getActiveTeam(), false))
	{
		EffectTypes eEffect = GC.getEntityEventInfo(GC.getMissionInfo(MISSION_PILLAGE).getEntityEvent()).getEffectType();
		gDLL->getEngineIFace()->TriggerEffect(eEffect, pPlot->getPoint(), (float)(GC.getASyncRand().get(360)));
		gDLL->getInterfaceIFace()->playGeneralSound("AS3D_UN_CITY_EXPLOSION", pPlot->getPoint());
	}

	CvWString szString = gDLL->getText("TXT_KEY_HARBOR_RAIDED", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), pCity->getNameKey(), GC.getUnitInfo(pTargetUnit->getUnitType()).getTextKeyWide());
	gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_CITYRAID", MESSAGE_TYPE_MINOR_EVENT, pTargetUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pCity->getX_INLINE(), pCity->getY_INLINE(), true, true);
	gDLL->getInterfaceIFace()->addMessage(pCity->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_CITYRAID", MESSAGE_TYPE_MINOR_EVENT, pTargetUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pCity->getX_INLINE(), pCity->getY_INLINE(), true, true);

	GET_TEAM(getTeam()).AI_changeDamages(pCity->getTeam(), -iDamage);

	pTargetUnit->changeDamage(iDamage, this);

	return true;
}

bool CvUnit::raidCity(CvCity* pCity)
{
	FAssertMsg(isNative(), "Only Native can raid cities");
	bool bRaidingNativeDefeated = isDead();

	bool bRaided = false;

	for (int iPass = 0; iPass < 3; iPass++)
	{
		int iRaidStength = GC.getGameINLINE().getSorenRandNum(1000, "Choose raiding party resolve");

		// Here we only have small effects, because raid was fought off
		if (bRaidingNativeDefeated)
		{
			if (iRaidStength > 900)
			{
				bRaided = raidProduction(pCity);
			}
			else if (iRaidStength > 800)
			{
				bRaided = raidTreasury(pCity);
			}
			else if (iRaidStength > 500)
			{
				bRaided = raidGoods(pCity);
			}
			else if (iRaidStength > 200)
			{
				bRaided = raidWeapons(pCity);
			}
			else if (iRaidStength > 100)
			{
				CvPlayer& kPlayer = GET_PLAYER(pCity->getOwnerINLINE());

				UnitTypes SlaveType = (UnitTypes)GC.getCivilizationInfo(kPlayer.getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_NATIVE_SLAVE"));
				CvUnit* SlaveUnit = kPlayer.initUnit(SlaveType, (ProfessionTypes) GC.getUnitInfo(SlaveType).getDefaultProfession(), pCity->getX_INLINE(), pCity->getY_INLINE(), NO_UNITAI);

				CvWString szString = gDLL->getText("TXT_KEY_RAIDING_NATIVE_CAPTURED", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), pCity->getNameKey());				
				gDLL->getInterfaceIFace()->addMessage(pCity->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, SlaveUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pCity->getX_INLINE(), pCity->getY_INLINE());		
				bRaided = true;
			}
			else
			{
				CvWString szString = gDLL->getText("TXT_KEY_NOTHING_RAIDED", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), pCity->getNameKey());
				gDLL->getInterfaceIFace()->addMessage(pCity->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_REVOLTEND", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"));
				bRaided = true;
			}
		}

		// Here we have all effects posssilbe, because raid was successfull
		else
		{
			if (iRaidStength > 900)
			{
				bRaided = raidScalp(pCity);
			}
			else if (iRaidStength > 800)
			{
				bRaided = raidHarbor(pCity);
			}
			else if (iRaidStength > 700)
			{
				bRaided = raidBuilding(pCity);
			}
			else if (iRaidStength > 600)
			{
				bRaided = raidProduction(pCity);
			}
			else if (iRaidStength > 500)
			{
				bRaided = raidTreasury(pCity);
			}
			else if (iRaidStength > 250)
			{
				bRaided = raidGoods(pCity);
			}
			else
			{
				bRaided = raidWeapons(pCity);
			}
		}

		// we have a raid event, so we break the for loop
		if (bRaided)
		{
			break;
		}
	}

	// if 3 loops did not successfully get raid even, try a last time
	if (!bRaided)
	{		
		bRaided = raidGoods(pCity);
	}

	if (!bRaided)
	{
		CvWString szString = gDLL->getText("TXT_KEY_NOTHING_RAIDED", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), pCity->getNameKey());
		gDLL->getInterfaceIFace()->addMessage(pCity->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_REVOLTEND", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"));
	}

	return bRaided;
}
// R&R, ray, Natives raiding party - END

bool CvUnit::raidGoods(CvCity* pCity)
{
	if (!isNative())
	{
		return false;
	}

	if (!isEnemy(pCity->getTeam()))
	{
		return false;
	}

	if (GC.getGameINLINE().getSorenRandNum(200, "Goods raid") < pCity->getDefenseModifier())
	{
		return false;
	}

	std::vector<YieldTypes> aYields;
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		YieldTypes eYield = (YieldTypes) iYield;
		if (pCity->getYieldStored(eYield) > 0 && GC.getYieldInfo(eYield).isCargo())
		{
			aYields.push_back(eYield);
		}
	}

	if (aYields.empty())
	{
		return false;
	}

	YieldTypes eYield = aYields[GC.getGameINLINE().getSorenRandNum(aYields.size(), "Choose raid goods")];
	int iYieldsStolen = std::min(pCity->getYieldStored(eYield), GC.getGameINLINE().getCargoYieldCapacity() * GC.getNATIVE_GOODS_RAID_PERCENT() / 100);

	FAssert(iYieldsStolen > 0);
	if (iYieldsStolen <= 0)
	{
		return false;
	}

	pCity->changeYieldStored(eYield, -iYieldsStolen);

	GET_TEAM(getTeam()).AI_changeDamages(pCity->getTeam(), -GET_PLAYER(getOwnerINLINE()).AI_yieldValue(eYield, true, iYieldsStolen));

	CvCity* pHomeCity = getHomeCity();
	if (pHomeCity == NULL)
	{
		pHomeCity = GC.getMapINLINE().findCity(pCity->getX_INLINE(), pCity->getY_INLINE(), getOwnerINLINE());
	}
	if (pHomeCity != NULL)
	{
		pHomeCity->changeYieldStored(eYield, iYieldsStolen);
	}

	CvWString szString = gDLL->getText("TXT_KEY_GOODS_RAIDED", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), pCity->getNameKey(), iYieldsStolen, GC.getYieldInfo(eYield).getTextKeyWide());
	gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, GC.getYieldInfo(eYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pCity->getX_INLINE(), pCity->getY_INLINE());
	gDLL->getInterfaceIFace()->addMessage(pCity->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, GC.getYieldInfo(eYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pCity->getX_INLINE(), pCity->getY_INLINE());
	return true;
}


//TAC Whaling, ray
bool CvUnit::isFullToBrim() const
{
	if (isFull())
	{
		CvUnitInfo& kUnit = getUnitInfo();
		std::vector<int> aiYieldsLoaded;
		int iNumUnitYield = kUnit.getNumYieldsGatherable();

		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			if (kUnit.canGatherYield(iYield))
			{
				aiYieldsLoaded.push_back(iYield);
			}
		}

		CLinkList<IDInfo> listCargo;
		getGroup()->buildCargoUnitList(listCargo);
		CLLNode<IDInfo>* pUnitNode = listCargo.head();
		int iCargoSpace = cargoSpace();

		if (!aiYieldsLoaded.empty())
		{
			while (pUnitNode != NULL)
			{
				CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = listCargo.next(pUnitNode);

				if (pLoopUnit->getYieldStored() != GC.getGameINLINE().getCargoYieldCapacity())
				{
					return false;
				}

				iCargoSpace--;

				if (iCargoSpace == 0)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool CvUnit::canGatherResource(const CvPlot* ePlot, bool bTestVisible) const
{
	if (!getUnitInfo().isGatherBoat())
	{
		return false;
	}

	CvPlot* pPlot = NULL;
	if (ePlot == NULL)
	{
		pPlot = plot();
	}
	else
	{
		pPlot = GC.getMapINLINE().plot(ePlot->getX_INLINE(), ePlot->getY_INLINE());
	}

	if (pPlot == NULL)
	{
		return false;
	}

	if (pPlot->isVisibleEnemyUnit(this))
	{
		return false;
	}

	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pAdjacentPlot = plotDirection(pPlot->getX_INLINE(), pPlot->getY_INLINE(), ((DirectionTypes)iI));
		if (pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->isVisibleEnemyUnit(this))
			{
				return false;
			}
		}
	}

	if (!bTestVisible)
	{
		if (!pPlot->isWater())
		{
			return false;
		}

		if (pPlot->isCityRadius())
		{
			CvCity* pNearestCity = GC.getMapINLINE().findCity(pPlot->getX_INLINE(), pPlot->getY_INLINE(), pPlot->getOwner(), pPlot->getTeam(), false, true);

			if (pNearestCity != NULL)
			{
				if (pPlot->isBeingWorked())
				{
					return false;
				}
				else if (!pNearestCity->isNative())
				{
					return false;
				}
			}
		}

		if (isFullToBrim())
		{
			return false;
		}

		// R&R, ray, High Sea Fishing - START
		bool bWhaleBonus = false;
		bool bFishBonus = false;


		if (pPlot->getBonusType() != NO_BONUS)
		{
			BonusTypes eFishBonus = pPlot->getBonusType();
			if (GC.getBonusInfo(eFishBonus).isWhalingboatWorkable())
			{
				if (!isWhalingBoat())
				{
					return false;
				}
				bWhaleBonus = true;
			}
			else if (GC.getBonusInfo(eFishBonus).isFishingboatWorkable())
			{
				if (!isFishingBoat())
				{
					return false;
				}
				bFishBonus = true;
			}
		}

		if (bWhaleBonus)
		{
			for (int i = 0; i < pPlot->getNumUnits(); i++)
			{
				CvUnit* pLoopUnit = pPlot->getUnitByIndex(i);
				if (pLoopUnit != NULL)
				{
					if (pLoopUnit != this && pLoopUnit->getGroup()->headMissionQueueNode() != NULL)
					{
						if  (pLoopUnit->getGroup()->headMissionQueueNode()->m_data.eMissionType == MISSION_WHALING)
						{
							return false;
						}
					}
				}
			}
		}
		else if (bFishBonus)
		{
			for (int i = 0; i < pPlot->getNumUnits(); i++)
			{
				CvUnit* pLoopUnit = pPlot->getUnitByIndex(i);
				if (pLoopUnit != NULL)
				{
					if (pLoopUnit != this && pLoopUnit->getGroup()->headMissionQueueNode() != NULL)
					{
						if  (pLoopUnit->getGroup()->headMissionQueueNode()->m_data.eMissionType == MISSION_FISHING)
						{
							return false;
						}
					}
				}
			}
		}
		// R&R, ray, High Sea Fishing- END
		else
		{
			return false;
		}
	}

	return true;
}

bool CvUnit::isGatheringResource() const
{
	return m_bGatheringResource;
}

void CvUnit::setGatheringResource(bool bNewValue)
{
	m_bGatheringResource = bNewValue;
}

void CvUnit::convertFishingBoat()
{
	if (isWhalingBoat())
	{
		ProfessionTypes eCurrentProfession = getProfession();
		if (!isGatheringResource())
		{
			if (eCurrentProfession == NO_PROFESSION)
			{
				setProfession((ProfessionTypes)(GC.getPROFESSION_WHALING_BOAT_WORKING()));
			}
		}
		else
		{
			if (eCurrentProfession == ((ProfessionTypes)(GC.getPROFESSION_WHALING_BOAT_WORKING())))
			{
				setProfession(NO_PROFESSION);
			}
		}
	}
	// R&R, ray, High Sea Fishing - START
	else if (isFishingBoat())
	{
		ProfessionTypes eCurrentProfession = getProfession();
		if (!isGatheringResource())
		{
			if (eCurrentProfession == NO_PROFESSION)
			{
				setProfession((ProfessionTypes)GC.getPROFESSION_FISHING_BOAT_WORKING());
			}
		}
		else
		{
			if (eCurrentProfession == ((ProfessionTypes)GC.getPROFESSION_FISHING_BOAT_WORKING()))
			{
				setProfession(NO_PROFESSION);
			}
		}
	}
	// R&R, ray, High Sea Fishing - END

}

bool CvUnit::gatherResource()
{
	// Return true means I am done Fishing or I can't fish anymore
	if (!canGatherResource(plot()))
	{
		return true;
	}

	bool bWhalerFull = false;

	//koma13
	//if (!hasMoved())
	if (movesLeft() > 0)
	{
		if (!isGatheringResource())
		{
			convertFishingBoat();
			setGatheringResource(true);
		}
		
		CvPlot* pPlot = plot();
		int iCargoSpace = cargoSpace();

		BonusTypes eBonus = plot()->getBonusType();
		int iNumYieldChanges = GC.getBonusInfo(eBonus).getNumYieldChanges();
		std::vector<int> eBonusYieldChanges = GC.getBonusInfo(eBonus).getYieldChangesArray();

		if (pPlot != NULL)
		{
			for (int i = 0; i < pPlot->getNumUnits(); i++)
			{
				CvUnit* pLoopUnit = pPlot->getUnitByIndex(i);
				if (pLoopUnit != NULL)
				{
					if (pLoopUnit->getTransportUnit() == this)
					{
						YieldTypes eYield = pLoopUnit->getYield();
						if (eYield != NO_YIELD)
						{
							int iYieldStored = pLoopUnit->getYieldStored();
							int iCargoCapacity = GC.getGameINLINE().getCargoYieldCapacity();
							int iLoadedAmount = pLoopUnit->getLoadedYieldAmount(eYield);

							// R&R, ray, High Sea Fishing - START
							int iGatherAmount = eBonusYieldChanges[(int)eYield];
							// we give minimum 10
							if (iGatherAmount < 10 && iGatherAmount > 0)
							{
								iGatherAmount = 10;
							}

							if ((iYieldStored + iGatherAmount) == iCargoCapacity)
							{
								pLoopUnit->setYieldStored(iCargoCapacity);
								eBonusYieldChanges[(int)eYield] = 0;
								iCargoSpace--;
							}
							else if ((iYieldStored + iGatherAmount) > iCargoCapacity)
							{
								eBonusYieldChanges[(int)eYield] = (iYieldStored + iGatherAmount) - iCargoCapacity;
								iCargoSpace--;
								pLoopUnit->setYieldStored(iCargoCapacity);
							}
							else
							{
								pLoopUnit->setYieldStored(iGatherAmount + iYieldStored);
								eBonusYieldChanges[(int)eYield] = 0;
							}
							// R&R, ray, High Sea Fishing - END
						}
					}
				}

				if (iCargoSpace <= 0)
				{
					bWhalerFull = true;
					break;
				}
			}
		}

		if (!bWhalerFull)
		{
			for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
			{
				if (eBonusYieldChanges[iYield] > 0)
				{
					YieldTypes eYield = (YieldTypes)iYield;
					UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getYieldInfo(eYield).getUnitClass());
					CvUnit* eGatheredUnit = GET_PLAYER(getOwnerINLINE()).initUnit(eUnit, NO_PROFESSION, getX_INLINE(), getY_INLINE(), NO_UNITAI, NO_DIRECTION, eBonusYieldChanges[iYield]);

					if (eGatheredUnit != NULL)
					{
						eGatheredUnit->setTransportUnit(this);
					}
				}
			}
		}
	}

	finishMoves();

	if (bWhalerFull)
	{
		// R&R, ray, High Sea Fishing - START
		if (isWhalingBoat())
		{
			CvWString szBuffer = gDLL->getText("TXT_KEY_WHALER_FULL", getNameKey());
			gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS3D_UN_OCEAN_END1", MESSAGE_TYPE_INFO, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE(), true, true);
		}
		else
		{
			CvWString szBuffer = gDLL->getText("TXT_KEY_FISHER_FULL", getNameKey());
			gDLL->getInterfaceIFace()->addMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS3D_UN_OCEAN_END1", MESSAGE_TYPE_INFO, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE(), true, true);
		}
		// R&R, ray, High Sea Fishing - END

		convertFishingBoat();
		setGatheringResource(false);

		return true;
	}

	return false;
}

bool CvUnit::isWhalingBoat() const
{
	return (getUnitClassType() == (UnitClassTypes)GC.getUNITCLASS_WHALING_BOAT());
}
//End TAC Whaling, ray

// R&R, ray, High Sea Fishing - START
bool CvUnit::isFishingBoat() const
{
	return (getUnitClassType() == (UnitClassTypes)GC.getUNITCLASS_FISHING_BOAT());
}
// R&R, ray, High Sea Fishing - END

// R&R, ray, Natives Trading - START
void CvUnit::setYieldForNativeTrade(YieldTypes nativeTradeYield)
{
	m_YieldForNativeTrade = nativeTradeYield;
}

void CvUnit::setAmountForNativeTrade(int nativeTradeAmount)
{
	m_AmountForNativeTrade = nativeTradeAmount;
}

YieldTypes CvUnit::getYieldForNativeTrade() const
{
	return m_YieldForNativeTrade;
}

int CvUnit::getAmountForNativeTrade() const
{
	return m_AmountForNativeTrade;
}
// R&R, ray, Natives Trading - END

// Returns 0 if there are no yield units on the transport to evaluate
int CvUnit::getCargoValue(Port port) const
{
	CvPlot const* const pPlot = plot();

	// TODO: Determine if the plot can  ever be NULL
	FAssert(pPlot != NULL);

	CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());
	CvPlayerAI& kEuropePlayer = GET_PLAYER(kOwner.getParent());

	int sellValue = 0;

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = plot()->nextUnitNode(pUnitNode);

		if (pLoopUnit->getTransportUnit() == this)
		{
			YieldTypes eYield = pLoopUnit->getYield();

			if (eYield != NO_YIELD)
			{
				if (port == EUROPE && kOwner.isYieldEuropeTradable(eYield))
				{
					sellValue += pLoopUnit->getYieldStored() * kEuropePlayer.getYieldBuyPrice(eYield);
				}
				if (port == AFRICA && kOwner.isYieldAfricaTradable(eYield))
				{
					sellValue += pLoopUnit->getYieldStored() * kEuropePlayer.getYieldAfricaBuyPrice(eYield);
				}
			}
		}
	}

	return sellValue;
}

// WTP, merge Treasures, of Raubwuerger - START
bool CvUnit::canMergeTreasures() const
{
	// WTP, ray, small improvements
	// merge only available for treasures
	if (getUnitInfo().isTreasure() == false)
	{
		return false;
	}

	// only in Cities or Native Villages
	if (plot()->isCity() == false)
	{
		return false;
	}

	// we can only merge if we are still smaller than max gold amount
	int maxTreasureGold = GC.getMAX_TREASURE_AMOUNT();
	int iGameSpeedModifier = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getStoragePercent() / 100;
	maxTreasureGold = maxTreasureGold * iGameSpeedModifier;

	if(getYieldStored() >= maxTreasureGold)
	{
		return false;
	}

	// we need to check if we have at least 2 valid treasures
	CLLNode<IDInfo>* pUnitNode = plot()->headUnitNode();
	CvUnit* pLoopUnit;
	int validTreasuresCounter = 0;

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = plot()->nextUnitNode(pUnitNode);

		// we only count our own treasuers and only the ones that are smaller than max gold amount
		if (pLoopUnit->getUnitInfo().isTreasure() && pLoopUnit->getOwner() == getOwner() && pLoopUnit->getYieldStored() < maxTreasureGold)
		{
			validTreasuresCounter++;
		}
	}

	return validTreasuresCounter >= 2;
}

void CvUnit::mergeTreasures()
{
	// probably not needed because merge is only called after caMerge - but for safety, let us keep it
	if (canMergeTreasures() == false)
	{
		return;
	}

	// this stores the total gold we find in valid treasures to merge
	int overallAmount = 0;

	CLLNode<IDInfo>* pUnitNode = plot()->headUnitNode();
	CvUnit* pLoopUnit;
	int maxTreasureGold = GC.getMAX_TREASURE_AMOUNT();
	int iGameSpeedModifier = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getStoragePercent() / 100;
	maxTreasureGold = maxTreasureGold * iGameSpeedModifier;

	// first we count the gold of these treasures
	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = plot()->nextUnitNode(pUnitNode);

		// WTP, ray, small improvements
		// we only count gold of our own treasuers and those that are not yet at max
		// after that we can directly kill them, so no extra loop is needed
		if (pLoopUnit->getUnitInfo().isTreasure() && pLoopUnit->getOwner() == getOwner() && pLoopUnit->getYieldStored() < maxTreasureGold)
		{
			overallAmount += pLoopUnit->getYieldStored();
			pLoopUnit->kill(true);
		}
	}
	
	createTreasures(overallAmount, maxTreasureGold);

	return;
}

void CvUnit::createTreasures(int overallAmount, int maxTreasureGold)
{

	FAssert(overallAmount > 0);
	FAssert(maxTreasureGold > 0);
	int treasureCount_MaxAmount = overallAmount / maxTreasureGold;
	
	int restAmount = overallAmount - (treasureCount_MaxAmount * maxTreasureGold);

	UnitClassTypes eUnitClass = (UnitClassTypes)GC.getDefineINT("TREASURE_UNITCLASS");
	if (eUnitClass == NO_UNITCLASS)
	{
		return; //Something went wrong
	}
	UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(eUnitClass);

	if (eUnit == NO_UNIT)
	{
		FAssert(GC.getUnitInfo(eUnit).isTreasure());
		return;
	}

	for (int treasures = 0; treasures < treasureCount_MaxAmount; treasures++)
	{
		CvUnit* pTreasure = GET_PLAYER(getOwnerINLINE()).initUnit(eUnit, (ProfessionTypes) GC.getUnitInfo(eUnit).getDefaultProfession(), plot()->getX_INLINE(), plot()->getY_INLINE(), NO_UNITAI, NO_DIRECTION, maxTreasureGold);
		// set Movement Points to 0, to prevent cheating
		pTreasure->setMoves(maxMoves());
	}
	if (restAmount >= 0)
	{
		CvUnit* pTreasure = GET_PLAYER(getOwnerINLINE()).initUnit(eUnit, (ProfessionTypes)GC.getUnitInfo(eUnit).getDefaultProfession(), plot()->getX_INLINE(), plot()->getY_INLINE(), NO_UNITAI, NO_DIRECTION, restAmount);
		// set Movement Points to 0, to prevent cheating
		pTreasure->setMoves(maxMoves());
	}
}
// WTP, merge Treasures, of Raubwuerger - END


// Erik: We should come up with a XML tag (e.g. bJoin vs. bFound) so that we don't need to hard-code this
bool CvUnit::isPrisonerOrSlave() const
{
	const int unitClassIntToBeChecked = m_pUnitInfo->getUnitClassType();

	if (unitClassIntToBeChecked == GC.getDefineINT("UNITCLASS_PRISONER") || unitClassIntToBeChecked == GC.getDefineINT("UNITCLASS_NATIVE_SLAVE") || unitClassIntToBeChecked == GC.getDefineINT("UNITCLASS_AFRICAN_SLAVE"))
	{
		return true;
	}
	else
	{
		return false;
	}
}

// Erik: Intended to by used by AI to determine if this unit should ever be considered for non-military uses.
// Hopefully this should help prevent the AI from fielding pioneers with a GG attached!
bool CvUnit::isProfessionalMilitary() const
{
	if (getUnitCombatType() == NO_UNITCOMBAT)
	{
		return false;
	}

	for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		// TODO: We need to ignore free-promotions since they should not count towards
		// military professionals since all units will have these
		if (isHasPromotion((PromotionTypes)iI))
		{
			const CvPromotionInfo& kPromotion = GC.getPromotionInfo((PromotionTypes)iI);

			// We can make this more sophisticated, but for now we concentrate on GGs
			if (kPromotion.isLeader())
			{
				return true;
			}
		}
	}

	return false;
}
