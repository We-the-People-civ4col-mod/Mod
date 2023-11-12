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

#include "CvSavegame.h"
#include "BetterBTSAI.h"

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
	m_eUnitType(NO_UNIT),
	m_iID(-1),
	m_iVisibilityRange(-1),

	// unit yield cache - start - Nightinggale
	m_eCachedYield(NO_YIELD)
	// unit yield cache - end - Nightinggale
{
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


void CvUnit::init(int iID, UnitTypes eUnit, ProfessionTypes eProfession, UnitAITypes eUnitAI, PlayerTypes eOwner, Coordinates initCoord, DirectionTypes eFacingDirection, int iYieldStored)
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
		CvPlot* pPlot = initCoord.plot();
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
				CvWString szName = GC.getCivilizationInfo(getCivilizationType()).getGeneralNames(iIndex);

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
				CvWString szName = GC.getCivilizationInfo(getCivilizationType()).getAdmiralNames(iIndex);

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
				CvWString szName = GC.getCivilizationInfo(getCivilizationType()).getShipNames(iIndex);

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
	GC.getGameINLINE().incrementUnitClassCreatedCount(m_pUnitInfo->getUnitClassType());

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

	addToMap(initCoord);
	AI_setUnitAIType(eUnitAI);

	gDLL->getEventReporterIFace()->unitCreated(this);

	FAssert(GET_PLAYER(getOwnerINLINE()).checkPopulation());
}


void CvUnit::uninit()
{
}


// FUNCTION: reset()
// Initializes data members that are serialized.
void CvUnit::reset(int iID, UnitTypes eUnit, PlayerTypes eOwner, bool bConstructorCall)
{
	//--------------------------------
	// Uninit class
	uninit();

	resetSavedData(iID, eUnit, eOwner, bConstructorCall);
	if(eOwner!=NO_PLAYER){
		setPromotions();
	}
	m_bInfoBarDirty = false;
	m_iBaseCombat = (NO_UNIT != m_eUnitType) ? m_pUnitInfo->getCombat() : 0;
	m_iUnarmedCount = 0;

	// unit yield cache - start - Nightinggale
	updateYieldCache();
	// unit yield cache - end - Nightinggale

	if (!bConstructorCall)
	{
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
				pLoopUnit = pPlot->getUnitNodeLoop(pUnitNode);

				if (pLoopUnit != NULL && pLoopUnit->getTransportUnit() == pUnit)
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
					gDLL->UI().addPlayerMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getEraInfo(GC.getGameINLINE().getCurrentEra()).getAudioUnitDefeatScript(), MESSAGE_TYPE_MAJOR_EVENT);
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
		// WTP, ray, change because we want to capture Nation specific Units - START
		// eCaptureUnitType = getCaptureUnitType(GET_PLAYER(eCapturingPlayer).getCivilizationType());
		eCaptureUnitType = getUnitType();
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
				eCaptureProfession = GC.getUnitInfo(eCaptureUnitType).getDefaultProfession();
			}
			// WTP, ray fixing Unit duplication bug
			// WTP, jooe (2023-04-05): enable unit capture code here again, the duplication was done in CvUnit::updateCombat
			// if (eCaptureProfession == NO_PROFESSION || (eCaptureProfession != NO_PROFESSION && GC.getProfessionInfo(eCaptureProfession).getCombatChange() == 0))
			CvUnit* pkCapturedUnit = GET_PLAYER(eCapturingPlayer).initUnit(eCaptureUnitType, eCaptureProfession, pPlot->getX_INLINE(), pPlot->getY_INLINE(), NO_UNITAI, NO_DIRECTION, iYieldStored);
			if (pkCapturedUnit != NULL)
			{
				bool bAlive = true;
				if (pAttacker != NULL && pAttacker->getUnitInfo().isCapturesCargo())
				{
					pkCapturedUnit->jumpTo(pAttacker->coord());
					if((pkCapturedUnit->isCargo() || !pAttacker->plot()->isValidDomainForAction(*pkCapturedUnit)) && pkCapturedUnit->getTransportUnit() == NULL) //failed to load
					{
						bAlive = false;
						pkCapturedUnit->kill(false);
					}
				}

				if (bAlive)
				{
					pkCapturedUnit->addDamageRandom(10, 75, 5);

					szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_CAPTURED_UNIT", GC.getUnitInfo(eCaptureUnitType).getTextKeyWide());
					gDLL->UI().addPlayerMessage(eCapturingPlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, pkCapturedUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX_INLINE(), pPlot->getY_INLINE());

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

	jumpTo(Coordinates::invalidCoord(), true);

	joinGroup(NULL, false, false);
}

void CvUnit::addToMap(Coordinates targetCoord)
{
	if(!targetCoord.isInvalidPlotCoord())
	{
		//--------------------------------
		// Init pre-setup() data
		jumpTo(targetCoord, false, false);

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

void CvUnit::addToMap(CvPlot *targetPlot)
{
	addToMap(targetPlot->coord());
}

void CvUnit::updateOwnerCache(int iChange)
{
	OOS_LOG_3("Update owner cache", getTypeStr(getUnitClassType()), iChange);

	CvPlayer& kPlayer = GET_PLAYER(getOwnerINLINE());

	GET_TEAM(getTeam()).changeUnitClassCount(m_pUnitInfo->getUnitClassType(), iChange);
	kPlayer.changeUnitClassCount(m_pUnitInfo->getUnitClassType(), iChange);
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
/** Ermittelt die Schiffst�rke der Einheit.                               **/
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
	// WTP, ray, we also add logic to damage Units on Land by Land Storms like e.g. Blizzard and Sandstorm
	if (getDomainType() == DOMAIN_SEA || (getDomainType() == DOMAIN_LAND && !plot()->isWater()))
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
		//WTP, ray Negative Promotions - START
		if(!isHuman())
		{
			if (plot()->isCity())
			{
				cleanseAllNegativePromotions();
			}
		}
		else
		{
			cleanseAllNegativePromotions();
		}
		//WTP, ray Negative Promotions - END

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

	if (GLOBAL_DEFINE_USE_CLASSIC_MOVEMENT_SYSTEM)
	{
		setMoves(0);
	}
	else
	{
		// ray, new Movement Calculation - START
		// we do not reset to 0 anymore because this would prevent the new calulation
		// we give back the full movement points of the Unit instead: Unit, Profession, Promotion, Traits, ...
		// but of course we never give more than the Unit can actually have
		// setMoves(0);
		if ((getMoves() - maxMoves()) < 0)
		{
			setMoves(0);
		}
		else
		{
			changeMoves(-maxMoves());
		}
		// ray, new Movement Calculation - END
	}
}


bool CvUnit::testWillGetNegativePromotion() const
{
	const int iHitPointThresholdForNegativePromotion = GC.getDefineINT("HEALTH_THRESHOLD_IN_PERCENT_FOR_NEGATIVE_PROMOTION");
	const int iChanceForNegativePromotion = GC.getDefineINT("CHANCE_FOR_NEGATIVE_PROMOTION_IN_PER_MILLE");

	const int UnitMaxHitPoints = maxHitPoints(); // should never be 0 here, because Defender won
	const int UnitCurrentHitPoints = currHitPoints();
	const int UnitHitPointsPercentage = 100 * UnitCurrentHitPoints / UnitMaxHitPoints;

	if (UnitHitPointsPercentage < iHitPointThresholdForNegativePromotion)
	{
		int randomValueNegativePromotion = GC.getGameINLINE().getSorenRandNum(1000, "Check for Negativ Promotion");
		if (randomValueNegativePromotion < iChanceForNegativePromotion)
		{
			return true;
		}
	}
	return false;
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
		if (GC.getGameINLINE().getSorenRandNum(GLOBAL_DEFINE_COMBAT_DIE_SIDES, "Combat") < iDefenderOdds)
		{
			if (getDamage() + iAttackerDamage >= maxHitPoints())
			{
				if (GC.getGameINLINE().getSorenRandNum(100, "Withdrawal") < withdrawalProbability())
				{
					changeExperience(GLOBAL_DEFINE_EXPERIENCE_FROM_WITHDRAWL, pDefender->maxXPValue(), true, pPlot->getOwnerINLINE() == getOwnerINLINE(), true);
					// R&R, ray, adapted change from C.B., with minor modifications

					//WTP, ray Negative Promotions - START
					// this here is the Attacker
					if (testWillGetNegativePromotion())
					{
						acquireAnyNegativePromotion();
					}
					//WTP, ray Negative Promotions - END

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
						setPostCombatPlot(pCity->coord());
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
					pDefender->setPostCombatPlot(pCity->coord());
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
				// attacker died, defender survives
				int iExperience = defenseXPValue();
				iExperience = ((iExperience * iAttackerStrength) / iDefenderStrength);
				iExperience = range(iExperience, GLOBAL_DEFINE_MIN_EXPERIENCE_PER_COMBAT, GLOBAL_DEFINE_MAX_EXPERIENCE_PER_COMBAT);
				pDefender->changeExperience(iExperience, maxXPValue(), true, pPlot->getOwnerINLINE() == pDefender->getOwnerINLINE(), true);

				// WTP, ray, Lieutenants and Captains - START

				CvPlayer& kDefendingPlayer = GET_PLAYER(pDefender->getOwnerINLINE());

				if (kDefendingPlayer.isPlayable() || kDefendingPlayer.isEurope())
				{
					if (pDefender->getDomainType() == DOMAIN_LAND)
					{
						// WTP, ray, prevent unimmersive cases of Brave Lieutenants being spawned by Combat with e.g. Missioniaries
						if(!pDefender->isCapturableLandUnit())
						{
							int iBraveLieutenantChance = GLOBAL_DEFINE_LIEUTENANT_CHANCE_PER_LAND_COMBAT_IN_PERCENT;
							int randomValue = GC.getGameINLINE().getSorenRandNum(100, "Check for Lieutenant");
							if (iBraveLieutenantChance > randomValue)
							{
								UnitTypes eBraveLieutentantUnitTypes = kDefendingPlayer.getUnitType(UNITCLASS_BRAVE_LIEUTENANT);
								kDefendingPlayer.createBraveLieutenant(eBraveLieutentantUnitTypes, pDefender->coord());
							}
						}
					}
					else
					{
						int iCapableCaptainChance = GLOBAL_DEFINE_CAPTAIN_CHANCE_PER_SHIP_COMBAT_IN_PERCENT;
						int randomValue = GC.getGameINLINE().getSorenRandNum(100, "Check for Captain");
						if (iCapableCaptainChance > randomValue)
						{
							UnitTypes eCapableCaptainUnitTypes = kDefendingPlayer.getUnitType(UNITCLASS_CAPABLE_CAPTAIN);
							int iLoop;
							CvCity* capitalCity = kDefendingPlayer.firstCity(&iLoop);
							if (capitalCity != NULL)
							{
								kDefendingPlayer.createCapableCaptain(eCapableCaptainUnitTypes, capitalCity->coord());
							}
						}
					}
				}
				// WTP, ray, Lieutenants and Captains - END

				//WTP, ray Negative Promotions - START
				if (pDefender->testWillGetNegativePromotion())
				{
					pDefender->acquireAnyNegativePromotion();
				}
				//WTP, ray Negative Promotions - END

				// R&R, ray, adapted change from C.B., with minor modifications
				bCombatEndedUnresolved = false;
			}
			else
			{
				// defender died, attacker survives
				int iExperience = pDefender->attackXPValue();
				iExperience = ((iExperience * iDefenderStrength) / iAttackerStrength);
				iExperience = range(iExperience, GLOBAL_DEFINE_MIN_EXPERIENCE_PER_COMBAT, GLOBAL_DEFINE_MAX_EXPERIENCE_PER_COMBAT);
				changeExperience(iExperience, pDefender->maxXPValue(), true, pPlot->getOwnerINLINE() == getOwnerINLINE(), true);

				CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());

				// WTP, ray, Lieutenants and Captains - START
				if (kOwner.isPlayable() || kOwner.isEurope())
				{
					// WTP, ray, prevent unimmersive cases of Brave Lieutenants being spawned by Combat with e.g. Missioniaries
					if (getDomainType() == DOMAIN_LAND)
					{
						// WTP, ray, prevent unimmersive cases of Brave Lieutenants being spawned by Combat with e.g. Missioniaries
						if(!pDefender->isCapturableLandUnit())
						{
							const int iBraveLieutenantChance = GLOBAL_DEFINE_LIEUTENANT_CHANCE_PER_LAND_COMBAT_IN_PERCENT;
							const int randomValue = GC.getGameINLINE().getSorenRandNum(100, "Check for Lieutenant");
							if (iBraveLieutenantChance > randomValue)
							{
								const UnitTypes eBraveLieutentantUnitTypes = kOwner.getUnitType(UNITCLASS_BRAVE_LIEUTENANT);
								kOwner.createBraveLieutenant(eBraveLieutentantUnitTypes, coord());
							}
						}
					}
					else
					{
						const int iCapableCaptainChance = GLOBAL_DEFINE_CAPTAIN_CHANCE_PER_SHIP_COMBAT_IN_PERCENT;
						const int randomValue = GC.getGameINLINE().getSorenRandNum(100, "Check for Captain");
						if (iCapableCaptainChance > randomValue)
						{
							const UnitTypes eCapableCaptainUnitTypes = kOwner.getUnitType(UNITCLASS_CAPABLE_CAPTAIN);
							//in this case we need to get the Capitol City
							int iLoop;
							const CvCity* capitalCity = kOwner.firstCity(&iLoop);
							if (capitalCity != 0)
							{
								kOwner.createCapableCaptain(eCapableCaptainUnitTypes, capitalCity->coord());
							}
						}
					}
				}
				// WTP, ray, Lieutenants and Captains - END

				//WTP, ray Negative Promotions - START
				if (testWillGetNegativePromotion())
				{
					acquireAnyNegativePromotion();
				}
				//WTP, ray Negative Promotions - END

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
		iExperienceAttacker = range(iExperienceAttacker, GLOBAL_DEFINE_MIN_EXPERIENCE_PER_COMBAT, GLOBAL_DEFINE_MAX_EXPERIENCE_PER_COMBAT);
		changeExperience(iExperienceAttacker, pDefender->maxXPValue(), true, pPlot->getOwnerINLINE() == getOwnerINLINE(), true);

		int iExperienceDefender = std::max(1,(defenseXPValue()/2));
		iExperienceDefender = range(iExperienceDefender, GLOBAL_DEFINE_MIN_EXPERIENCE_PER_COMBAT, GLOBAL_DEFINE_MAX_EXPERIENCE_PER_COMBAT);
		pDefender->changeExperience(iExperienceDefender, maxXPValue(), true, pPlot->getOwnerINLINE() == pDefender->getOwnerINLINE(), true);

		//WTP, ray Negative Promotions - START

		// this here is the Attacker
		if (testWillGetNegativePromotion())
		{
			acquireAnyNegativePromotion();
		}

		// this here is the Defender
		if (pDefender->testWillGetNegativePromotion())
		{
			pDefender->acquireAnyNegativePromotion();
		}
		//WTP, ray Negative Promotions - END
	}
	// R&R, ray, adapted change from C.B., with minor modifications - END
}


void CvUnit::updateCombat(bool bQuick)
{
	CvWString szBuffer;

	bool bFinish = false;

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

	const bool bVisible = bQuick ? false : isCombatVisible(pDefender);

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
				const PlayerTypes eAttacker = getVisualOwner(pDefender->getTeam());
				CvWString szMessage;
				if (UNKNOWN_PLAYER != eAttacker)
				{
					szMessage = gDLL->getText("TXT_KEY_MISC_YOU_UNITS_UNDER_ATTACK", GET_PLAYER(eAttacker).getNameKey());
				}
				else
				{
					szMessage = gDLL->getText("TXT_KEY_MISC_YOU_UNITS_UNDER_ATTACK_UNKNOWN");
				}

				gDLL->UI().addPlayerMessage(pDefender->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szMessage, "AS2D_COMBAT", MESSAGE_TYPE_DISPLAY_ONLY, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pPlot->getX_INLINE(), pPlot->getY_INLINE(), true);
			}
		}

		FAssertMsg(pDefender != NULL, "Defender is not assigned a valid value");

		FAssertMsg(plot()->isFighting(), "Current unit instance plot is not fighting as expected");
		FAssertMsg(pPlot->isFighting(), "pPlot is not fighting as expected");

		// Determine if the defender can defend in this plot (ships inside a city cannot defend against land attacks)
		if (!pPlot->isValidDomainForAction(*pDefender))
		{
			// WTP, ray, fix for destroying ships instead of ejecting - START
			bool bShipCityEscape = false;
			const bool bPlotCityOrFort = pDefender->plot()->isCity() || pDefender->plot()->isFort();

			// Ships in Harbour Cities should be ejected damaged when City Captured
			if (bPlotCityOrFort && getDomainType()!= DOMAIN_SEA && pDefender->getDomainType() == DOMAIN_SEA)
			{
				CvPlot* pDefenderPlot = pDefender->plot();
				CvPlot* pEjectPlot = NULL;
				CvCity* pCityForMessage = pDefender->plot()->getPlotCity();

				// Try to find a suitable water plot to eject
				for (DirectionTypes eDirection = FIRST_DIRECTION; eDirection < NUM_DIRECTION_TYPES; ++eDirection)
				{
					CvPlot* pAdjacentPlot = plotDirection(pDefenderPlot->getX_INLINE(), pDefenderPlot->getY_INLINE(), eDirection);

					// Determine if we can escape to this plot
					if (pAdjacentPlot != NULL && pDefender->canMoveInto(*pAdjacentPlot))
					{
						pEjectPlot = pAdjacentPlot;
						break;
					}
				}

				// if suitable eject Plot found, give some random damage and move to eject Plot
				if (pEjectPlot != NULL)
				{
					pDefender->addDamageRandom(10, 75, 5);

					// clean up to avoid Asserts when changing positon
					setCombatUnit(NULL);
					pDefender->setCombatUnit(NULL);

					// changing position
					pDefender->jumpTo(pEjectPlot->coord());

					if (pCityForMessage != NULL)
					{
						// Send Messages
						szBuffer = gDLL->getText("TXT_KEY_MISC_ENEMY_SHIP_ESCAPED_CITY", pDefender->getName().GetCString(), pCityForMessage->getNameKey());
						gDLL->UI().addPlayerMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pEjectPlot->getX_INLINE(), pEjectPlot->getY_INLINE());

						szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_SHIP_ESCAPED_CITY", pDefender->getName().GetCString(), pCityForMessage->getNameKey());
						gDLL->UI().addPlayerMessage(pDefender->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pEjectPlot->getX_INLINE(), pEjectPlot->getY_INLINE());
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

		// case: Attacker died, defender won
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
				GET_TEAM(pDefender->getTeam()).AI_changeWarSuccess(getTeam(), GLOBAL_DEFINE_WAR_SUCCESS_DEFENDING);
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
				gDLL->UI().addPlayerMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getEraInfo(GC.getGameINLINE().getCurrentEra()).getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pPlot->getX_INLINE(), pPlot->getY_INLINE());
			}
			// R&R, ray, Monasteries and Forts - END

			szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_DIED_ATTACKING", getNameOrProfessionKey(), pDefender->getNameOrProfessionKey());
			gDLL->UI().addPlayerMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getEraInfo(GC.getGameINLINE().getCurrentEra()).getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pPlot->getX_INLINE(), pPlot->getY_INLINE());
			szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_KILLED_ENEMY_UNIT", pDefender->getNameOrProfessionKey(), getNameOrProfessionKey(), getVisualCivAdjective(pDefender->getTeam()));
			gDLL->UI().addPlayerMessage(pDefender->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getEraInfo(GC.getGameINLINE().getCurrentEra()).getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX_INLINE(), pPlot->getY_INLINE());

			// WTP, ray, fix for Human Unit not stopping automation after attacked - START
			// in this case the defender is alive and has won the battle
			if (pDefender->isHuman() && pDefender->isAutomated())
			{
				pDefender->getGroup()->setAutomateType(NO_AUTOMATE);
			}
			// WTP, ray, fix for Human Unit not stopping automation after attacked - END

			// report event to Python, along with some other key state
			gDLL->getEventReporterIFace()->combatResult(pDefender, this);

			// TAC - AI purchases military units - koma13 - START
			if (!isHuman() && getUnitInfo().isHiddenNationality())
			{
				GET_PLAYER(getOwnerINLINE()).AI_changeNumRetiredAIUnits(UNITAI_PIRATE_SEA, 1);
			}
			// TAC - AI purchases military units - koma13 - END
		}

		// case: Attacker won, defender died
		else if (pDefender->isDead())
		{
			CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());

			// TAC Capturing Ships - ray
			bool displayCapturedShipMessage = false;

			// ray, fix for bNoCapture being ingored
			if (getUnitInfo().isCapturesShips() && !pDefender->getUnitInfo().isAnimal() && !pDefender->getUnitInfo().isNoCapture())
			{
				int capturingShipChance = GC.getBASE_CHANCE_CAPTURING_SHIPS();
				const int randomShipCaptureValue = GC.getGameINLINE().getSorenRandNum(1000, "Capture Ships");
				// WTP, ray, Capture Ship chance increase - START
				const int iCaptureShipsChanceIncrease = getUnitInfo().getCaptureShipsChanceIncrease();
				capturingShipChance = capturingShipChance * (100 + iCaptureShipsChanceIncrease) / 100;
				// WTP, ray, Capture Ship chance increase - START

				if (capturingShipChance > randomShipCaptureValue)
				{
					bool bHasParents = kOwner.getParent() != NO_PLAYER;
					bool bIsEurope = kOwner.isEurope();

					if ((bHasParents && pDefender->getUnitInfo().getDomainType() == DOMAIN_SEA) || (bIsEurope && pDefender->getUnitInfo().isCapturesShips()))
					{
						bool bAtWar = GET_TEAM(getTeam()).isAtWar(pDefender->getTeam());

						if ((getUnitInfo().isCapturesCargo() && bAtWar) || !getUnitInfo().isCapturesCargo())
						{
							// duplicate defeated unit
							CvUnit* pkCapturedUnitAfterSeaFight = kOwner.initUnit(pDefender->getUnitType(), NO_PROFESSION, pPlot->getX_INLINE(), pPlot->getY_INLINE(), NO_UNITAI, NO_DIRECTION, pDefender->getYieldStored());
							displayCapturedShipMessage = true;

							pkCapturedUnitAfterSeaFight->setGameTurnCreated(pDefender->getGameTurnCreated());
							pkCapturedUnitAfterSeaFight->addDamageRandom(10, 75, 5);
							pkCapturedUnitAfterSeaFight->setFacingDirection(pDefender->getFacingDirection(false));
							pkCapturedUnitAfterSeaFight->setLevel(pDefender->getLevel());

							const int iOldModifier = std::max(1, 100 + GET_PLAYER(pDefender->getOwnerINLINE()).getLevelExperienceModifier());
							const int iOurModifier = std::max(1, 100 + GET_PLAYER(pkCapturedUnitAfterSeaFight->getOwnerINLINE()).getLevelExperienceModifier());
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
							// WTP, ray, captured Ships should also get a Negative Promotion
							pkCapturedUnitAfterSeaFight->acquireAnyNegativePromotion();
						}
					}
				}
			}
			// TAC Capturing Ships - ray - END

			// WTP, ray, Prisoners of War - START
			bool displayCapturedPrisoneOfWarMessage = false;

			// We only want to have this for Military Combat between Europeans and Kings, and only during War
			bool bAttackingPlayerInvalid = !kOwner.isPlayable(); // only playable Civs will capture Prisoners of War, to prevent future issues
			bool bDefendingPlayerInvalid = GET_PLAYER(pDefender->getOwner()).isNative() || GC.getGameINLINE().isBarbarianPlayer(pDefender->getOwner());
			bool bAttackingUnitInvalid = getDomainType() == DOMAIN_SEA || getUnitInfo().isAnimal() || getUnitInfo().isHiddenNationality();
			bool bDefendingUnitInvalid = pDefender->getDomainType() == DOMAIN_SEA || pDefender->getUnitInfo().isAnimal() || pDefender->isCapturableLandUnit() || pDefender->getUnitInfo().isHiddenNationality();
			bool bAtWar = GET_TEAM(getTeam()).isAtWar(pDefender->getTeam());

			if (!bAttackingPlayerInvalid && !bDefendingPlayerInvalid && !bAttackingUnitInvalid && !bDefendingUnitInvalid && bAtWar)
			{
				int capturingPrisonerOfWarChance = GLOBAL_DEFINE_BASE_CHANCE_CAPTURING_PRISONER_OF_WAR;
				const int randomCapturePrisonerOfWarValue = GC.getGameINLINE().getSorenRandNum(1000, "Capture Prisoner of War");

				// WTP, ray, here we could in the future have modifiers for specific Units
				//const int iCapturePrisonersOfWarChanceIncrease = getUnitInfo().getCapturePrisonersOfWarChanceIncrease();
				//capturingPrisonerOfWarChance = capturingPrisonerOfWarChance * (100 + iCapturePrisonersOfWarChanceIncrease) / 100;

				if (capturingPrisonerOfWarChance > randomCapturePrisonerOfWarValue)
				{	
					UnitTypes eCapturedPrisonerOfWar = kOwner.getUnitType(UNITCLASS_PRISONER_OF_WAR);
					CvUnit* PrisonerOfWarUnit = kOwner.initUnit(eCapturedPrisonerOfWar, GC.getUnitInfo(eCapturedPrisonerOfWar).getDefaultProfession(), pDefender->plot()->coord(), NO_UNITAI);
					// WTP, ray, Prisoners of War should also get some damage and a Negative Promotion
					if (PrisonerOfWarUnit != NULL)
					{
						PrisonerOfWarUnit->addDamageRandom(10, 75, 5);
						PrisonerOfWarUnit->acquireAnyNegativePromotion();
					}
					displayCapturedPrisoneOfWarMessage = true;
				}
			}
			// WTP, ray, Prisoners of War - END

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

			if (!getUnitInfo().isHiddenNationality() && !pDefender->getUnitInfo().isHiddenNationality())
			{
				// R&R, ray, Natives raiding party - START
				if (GET_PLAYER(getOwnerINLINE()).isNative() && pDefender->getUnitInfo().isTreasure())
				{
					int iRaidTreasureSum = pDefender->getYieldStored() * GC.getNATIVE_GOODS_RAID_PERCENT() / 100;
					OOS_LOG("Raid gold", iRaidTreasureSum);
					GET_PLAYER(getOwnerINLINE()).changeGold(iRaidTreasureSum);
				}

				if (!bIsNativeRaid)
				{
					GET_TEAM(getTeam()).AI_changeWarSuccess(pDefender->getTeam(), GLOBAL_DEFINE_WAR_SUCCESS_ATTACKING);
					if (GET_PLAYER(getOwnerINLINE()).isNative())
					{
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

			// WTP, ray, Prisoners of War - START
			if (displayCapturedPrisoneOfWarMessage)
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_CAPTURED_PRISONER_OF_WAR", pDefender->getNameOrProfessionKey());
			}
			// WTP, ray, Prisoners of War - END

			//ray14
			// R&R, ray, changes to Wild Animals - added addittional check so Animals do not capture
			// WTP, ray, allowing Wagon Trains to be caught again
			//WTP, ray, Large Rivers - START
			// allowing Cargo Ships on Large Rivers to be caugth as well
			// if (((pDefender->cargoSpace() > 0 && (pDefender->getDomainType() == DOMAIN_LAND)) || pDefender->getUnitInfo().isTreasure() || (pDefender->isUnarmed() && pDefender->getProfession() != NO_PROFESSION && GC.getProfessionInfo(pDefender->getProfession()).getCombatChange() > 0)) && !GET_PLAYER(getOwnerINLINE()).isNative() && !GC.getGameINLINE().isBarbarianPlayer(getOwnerINLINE()))
			// ray, fix for bNoCapture being ingored
			// we do not capture Units anymore that are flagged as bNoCapture
			// WTP, jooe (2023-04-05): disable unit capture in CvUnit::updateCombat() - START
			// jooe: completely disable unit capture here in CvUnit::updateCombat() - it should happen in CvUnit::kill instead!
			// if (!pDefender->getUnitInfo().isNoCapture())
			// {
			// 	if (((pDefender->cargoSpace() > 0 && (pDefender->getDomainType() == DOMAIN_LAND || (pDefender->getDomainType() == DOMAIN_SEA && pPlot->getTerrainType() == TERRAIN_LARGE_RIVERS))) || pDefender->getUnitInfo().isTreasure() || (pDefender->isUnarmed() && pDefender->getProfession() != NO_PROFESSION && GC.getProfessionInfo(pDefender->getProfession()).getCombatChange() > 0)) && !GET_PLAYER(getOwnerINLINE()).isNative() && !GC.getGameINLINE().isBarbarianPlayer(getOwnerINLINE()))
			// 	{
			// 		CvUnit* pkCapturedUnitAfterFight = GET_PLAYER(getOwnerINLINE()).initUnit(pDefender->getUnitType(), pDefender->getProfession(), pPlot->getX_INLINE(), pPlot->getY_INLINE(), NO_UNITAI, NO_DIRECTION, pDefender->getYieldStored());
			// 		pkCapturedUnitAfterFight->setDamage(GC.getMAX_HIT_POINTS() / 2);
			// 		szBuffer = gDLL->getText("TXT_KEY_UNIT_CAPTURED_AFTER_FIGHT", pDefender->getUnitInfo().getDescription());
			// 	}
			// }
			// WTP, jooe (2023-04-05): disable unit capture in CvUnit::updateCombat() - END
			//Ende ray14

			// R&R, ray, changes to Wild Animals - START
			if ((pDefender->getUnitInfo()).isAnimal())
			{
				int iGoldRewardRandomBase = GC.getWILD_ANIMAL_REWARD_RANDOM_BASE();
				int iGold = GC.getGameINLINE().getSorenRandNum(iGoldRewardRandomBase, "Animal Kill Reward");
				//WTP, ray, Animal Promotions increase gold from Animals - START
				//we now also apply the gold change from Promotions stored in the Unit
				iGold += (iGold * getAnimalGoldChange()) / 100;
				//WTP, ray, Animal Promotions increase gold from Animals - END
				OOS_LOG_3("Gold for killling animal", getOwnerINLINE(), iGold);
				GET_PLAYER(getOwnerINLINE()).changeGold(iGold);
				szBuffer = gDLL->getText("TXT_KEY_ANIMAL_REWARD", pDefender->getUnitInfo().getDescription(), iGold);
			}
			// R&R, ray, changes to Wild Animals - END

			// R&R, ray, Monasteries and Forts - START
			if (bIsNativeRaid && pPlot->isFort())
			{
				szBuffer = gDLL->getText("TXT_KEY_NATIVE_RAID_ATTACKED_FORT_BUT_DEFEATED");
				gDLL->UI().addPlayerMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getEraInfo(GC.getGameINLINE().getCurrentEra()).getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX_INLINE(), pPlot->getY_INLINE());
			}
			// R&R, ray, Monasteries and Forts - END

			gDLL->UI().addPlayerMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getEraInfo(GC.getGameINLINE().getCurrentEra()).getAudioUnitVictoryScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX_INLINE(), pPlot->getY_INLINE());
			if (getVisualOwner(pDefender->getTeam()) != getOwnerINLINE())
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED_UNKNOWN", pDefender->getNameOrProfessionKey(), getNameOrProfessionKey());
			}
			else
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WAS_DESTROYED", pDefender->getNameOrProfessionKey(), getNameOrProfessionKey(), getVisualCivAdjective(pDefender->getTeam()));
			}

			gDLL->UI().addPlayerMessage(pDefender->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer,GC.getEraInfo(GC.getGameINLINE().getCurrentEra()).getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pPlot->getX_INLINE(), pPlot->getY_INLINE());

			// report event to Python, along with some other key state
			gDLL->getEventReporterIFace()->combatResult(this, pDefender);

			bool bAdvance = false;
			const bool bRaided = raidWeapons(pDefender);

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

			// If defender is a civilian unit, capture it and all other civilian units on the plot
			if (	!isNoUnitCapture() &&
						pDefender->isCapturableLandUnit() &&
 						!GET_PLAYER(getOwnerINLINE()).isNative() &&
						!GC.getGameINLINE().isBarbarianPlayer(getOwnerINLINE()) &&
						!GC.getGameINLINE().isChurchPlayer(getOwnerINLINE()) )
			{
				pDefender->setCapturingPlayer(getOwnerINLINE());
				CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();

				// because kill can mess up the order in pUnitNode, store all IDs prior to any units being killed
				std::vector<IDInfo> IDs;
				while (pUnitNode != NULL)
				{
					IDs.push_back(pUnitNode->m_data);
					pUnitNode = pPlot->nextUnitNode(pUnitNode);
				}

				while (!IDs.empty())
				{
					// remove from the back as that hopefully always capture cargo before transports
					// when a transport is killed, the cargo dies instantly
					IDInfo info = IDs[IDs.size() - 1];
					IDs.pop_back();
					CvUnit* pLoopUnit = ::getUnit(info);
					if (pLoopUnit != NULL && pLoopUnit != pDefender)
					{
						if (isEnemy(pLoopUnit->getCombatTeam(getTeam(), pPlot), pPlot) &&
								(pLoopUnit->isCapturableLandUnit() || pLoopUnit->isYield()))
						{
							pLoopUnit->setCapturingPlayer(getOwnerINLINE());
							pLoopUnit->kill(false);
						}
					}
				}
			}

			bAdvance = canAdvance(pPlot, ((pDefender->canDefend()) ? 1 : 0));


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

		// case: Attacker won, defender escaped
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
			gDLL->UI().addPlayerMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pPlot->getX_INLINE(), pPlot->getY_INLINE());
			CvCity* pCity = pDefender->plot()->getPlotCity();
			if (pCity != NULL)
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_ENEMY_UNIT_ESCAPED", pDefender->getNameOrProfessionKey(), getNameOrProfessionKey(), pCity->getNameKey());
				gDLL->UI().addPlayerMessage(pDefender->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pCity->getX_INLINE(), pCity->getY_INLINE());
			}

			// WTP, ray, fix for Human Unit not stopping automation after attacked - START
			// in this case the defender is alive and lost but escaped
			if (pDefender->isHuman() && pDefender->isAutomated())
			{
				pDefender->getGroup()->setAutomateType(NO_AUTOMATE);
			}
			// WTP, ray, fix for Human Unit not stopping automation after attacked - END

			bool bAdvance = canAdvance(pPlot, 0);
			if (!bAdvance)
			{
				changeMoves(std::max(GLOBAL_DEFINE_MOVE_DENOMINATOR, pPlot->movementCost(this, plot())));

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

		// case: Attacker lost but escaped
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
				gDLL->UI().addPlayerMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pCity->getX_INLINE(), pCity->getY_INLINE());
			}
			szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_ESCAPED", pDefender->getNameOrProfessionKey(), getNameOrProfessionKey());
			gDLL->UI().addPlayerMessage(pDefender->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pPlot->getX_INLINE(), pPlot->getY_INLINE());

			// WTP, ray, fix for Human Unit not stopping automation after attacked - START
			// in this case the defender is alive and had won the battle
			if (pDefender->isHuman() && pDefender->isAutomated())
			{
				pDefender->getGroup()->setAutomateType(NO_AUTOMATE);
			}
			// WTP, ray, fix for Human Unit not stopping automation after attacked - END

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

		// case: draw, but sides withdraw
		else
		{
			szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_UNIT_WITHDRAW", getNameOrProfessionKey(), pDefender->getNameOrProfessionKey());
			gDLL->UI().addPlayerMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_OUR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX_INLINE(), pPlot->getY_INLINE());
			szBuffer = gDLL->getText("TXT_KEY_MISC_ENEMY_UNIT_WITHDRAW", getNameOrProfessionKey(), pDefender->getNameOrProfessionKey());
			gDLL->UI().addPlayerMessage(pDefender->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_THEIR_WITHDRAWL", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pPlot->getX_INLINE(), pPlot->getY_INLINE());

			// WTP, ray, fix for Human Unit not stopping automation after attacked - START
			// in this case we are sure that defender is alive since both are alive
			if (pDefender->isHuman() && pDefender->isAutomated())
			{
				pDefender->getGroup()->setAutomateType(NO_AUTOMATE);
			}
			// WTP, ray, fix for Human Unit not stopping automation after attacked - END

			changeMoves(std::max(GLOBAL_DEFINE_MOVE_DENOMINATOR, pPlot->movementCost(this, plot())));

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
						for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
						{
							if (GC.getImprovementInfo(eFinalImprovement).getRouteYieldChanges(eRoute, eYield) > 0)
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

	if (GC.getActionInfo(iAction).getCommandType() == COMMAND_SAIL_TO_EUROPE)
	{
		CLinkList<IDInfo> listCargo;
		getGroup()->buildCargoUnitList(listCargo);
		CLLNode<IDInfo>* pUnitNode = listCargo.head();
		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = listCargo.next(pUnitNode);

			if (pLoopUnit != NULL)
			{
				if (pLoopUnit->getYield() != NO_YIELD && GET_PLAYER(getOwnerINLINE()).isYieldEuropeTradable(pLoopUnit->getYield()))
				{
					return true;
				}

				if (pLoopUnit->getUnitInfo().isTreasure())
				{
					return true;
				}
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

	// WTP, ray, Construction Supplies - START
	case COMMAND_USE_CONSTRUCTION_SUPPLIES:
		if (canUseProductionSupplies())
		{
			return true;
		}
		break;
	// WTP, ray, Construction Supplies - END

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
				FAssert((YieldTypes)iData1 == getYield());
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

		// WTP, ray, Construction Supplies - START
		case COMMAND_USE_CONSTRUCTION_SUPPLIES:
			if(isGroupHead())
			{
				useProductionSupplies();
			}
			break;
		// WTP, ray, Construction Supplies - END

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


// Disabled by K-Mod. (This function is deprecated.)
/* FAStarNode* CvUnit::getPathLastNode() const
{
return getGroup()->getPathLastNode();
} */


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
// TAC - AI Improved Naval AI - koma13 - END

bool CvUnit::generatePath(const CvPlot* pToPlot, int iFlags, bool bReuse,
	int* piPathTurns, int iMaxPath,   // <advc.128>
	bool bUseTempFinder) const
{
	if (!bUseTempFinder) // </advc.128>
		return getGroup()->generatePath(plot(), pToPlot, iFlags, bReuse, piPathTurns, iMaxPath);
	// <advc.128>
	FAssert(!bReuse);
	KmodPathFinder temp_finder;
	temp_finder.SetSettings(getGroup(), iFlags, iMaxPath, GLOBAL_DEFINE_MOVE_DENOMINATOR);
	bool r = temp_finder.GeneratePath(pToPlot);
	if (piPathTurns != NULL)
		*piPathTurns = temp_finder.GetPathTurns();
	return r; // </advc.128>
}

// K-Mod. Return the standard pathfinder, for extracting path information.
KmodPathFinder& CvUnit::getPathFinder() const
{
	return CvSelectionGroup::path_finder;
}
// K-Mod end

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

	// WTP, ray, prevent first Settler Units to be caught in deadends by other Territory - START
	// for most of the "Forced Peace Period" - We will allow Units to pass other Territory
	if (GC.getGame().getRemainingForcedPeaceTurns() > 2)
	{
		return true;
	}
	// WTP, ray, prevent first Settler Units to be caught in deadends by other Territory - END

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
	if (canMoveInto(*pPlot, true, true))
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

/*  K-Mod. I've rearranged a few things to make the function slightly faster,
and added "bAssumeVisible" which signals that we should check for units on
the plot regardless of whether we can actually see. */
bool CvUnit::canMoveInto(CvPlot const& kPlot, bool bAttack, bool bDeclareWar, bool bIgnoreLoad, bool bAssumeVisible, // advc: 1st param was a pointer
	bool bDangerCheck) const // advc.001k
{
	if (atPlot(&kPlot))
	{
		return false;
	}

	// R&R, ray, changes to Wild Animals - START
	// we now just check that the Animals can not enter City Radius of "Non-Natives" anymore
	if (getUnitInfo().isAnimal())
	{
		PlayerTypes ePlotOwner = kPlot.getOwnerINLINE();
		if (ePlotOwner != NO_PLAYER)
		{
			CvPlayerAI& kPlayer = GET_PLAYER(ePlotOwner);
			// the owner of the Plot is not a Native
			// unless of course the Animal belongs to the Plot owner itself
			if (!kPlayer.isNative() && getOwnerINLINE() != ePlotOwner)
			{
				if (kPlot.isCityRadius())
				{
					return false;
				}
			}
		}
	}
	// R&R, ray, changes to Wild Animals - END

	if (kPlot.isImpassable())
	{
		if (!canMoveImpassable())
		{
			return false;
		}
	}

	// WTP, ray, Canal - START
	// in Canals, which are actually on land plots, we do not want to have any Ships bigger than Coastal Ships or GatherBoats
	if (getUnitInfo().getDomainType() == DOMAIN_SEA && !kPlot.isWater() && !getUnitInfo().getTerrainImpassable(TERRAIN_OCEAN) && !(getUnitInfo().isGatherBoat() && getUnitInfo().getHarbourSpaceNeeded() == 1) && kPlot.getImprovementType() != NO_IMPROVEMENT && GC.getImprovementInfo(kPlot.getImprovementType()).isCanal())
	{
		return false;
	}
	// WTP, ray, Canal - END

	const FeatureTypes eFeature = kPlot.getFeatureType();

	// Prevent the AI from moving through storms and sustaining damage
	// Strictly speaking this should preferably be handled by either the path cost or a separate PF flag
	if (getGroup()->AI_isControlled() && (eFeature != NO_FEATURE) &&  GC.getFeatureInfo(eFeature).getTurnDamage() > 0)
	{
		return false;
	}

	const CvArea *const pPlotArea = kPlot.area();
	TeamTypes ePlotTeam = kPlot.getTeam();
	bool bCanEnterArea = canEnterArea(kPlot.getOwnerINLINE(), pPlotArea);
	const DomainTypes eDomainType = getDomainType();

	if (bCanEnterArea)
	{
		if (eFeature != NO_FEATURE)
		{
			const CvFeatureInfo& kFeatureInfo = GC.getFeatureInfo(eFeature);

			// Prevent the AI from moving through storms and sustaining damage
			// Strictly speaking this should preferably be handled by either the path cost or a separate PF flag
			// Also checking isGeneratedEveryRound() in case a mod-mod adds turn damage to certain plots and
			// crossing them would be the only way to make forward progress
			if (getGroup()->AI_isControlled() && kFeatureInfo.getTurnDamage() > 0 &&
				kFeatureInfo.isGeneratedEveryRound())
			{
				return false;
			}

			if (m_pUnitInfo->getFeatureImpassable(eFeature))
			{
				if (DOMAIN_SEA != eDomainType || kPlot.getTeam() != getTeam())  // sea units can enter impassable in own cultural borders
				{
					return false;
				}
				// Erik: The AI may attempt to pathfind into unrevealed ocean plots so have have to check for the plot being revealed
				if (DOMAIN_SEA == eDomainType && !kPlot.isRevealed(getTeam(), false))
				{
					return false;
				}
			}
		}

		if (m_pUnitInfo->getTerrainImpassable(kPlot.getTerrainType()))
		{
			//WTP, ray, Large Rivers - START
			// allow all Land Units to enter Large Rivers with Improvement

			bool bLandUnitMayPassLargeRiverDueToImprovement = false;
			bool bLandUnitMayPassLargeRiverDueToTerrainFeature = false;
			bool bLandUnitMayPassLargeRiverDueToProfession = false;
			bool bLandUnitMayBeLoaded = false;

			if (eDomainType == DOMAIN_LAND && kPlot.getTerrainType() == TERRAIN_LARGE_RIVERS)
			{
				//WTP, ray, small adaptation to ensure that not all Improvements allow Movement on Large Rivers by adding "Outside Borders check" which is only true for "Ferry Station" aka "Raft Station"
				// TODO: Maybe create a new XML tag "bAllowLargeRiverMovement in XML of Improvements instead - might be cleaner
				bLandUnitMayPassLargeRiverDueToImprovement = (kPlot.getImprovementType() != NO_IMPROVEMENT && GC.getImprovementInfo(kPlot.getImprovementType()).getTerrainMakesValid(TERRAIN_LARGE_RIVERS) && GC.getImprovementInfo(kPlot.getImprovementType()).isOutsideBorders());
				//WTP, ray, small adaptation to ensure that not all Terrain Features allow Movement on Large Rivers by adding "bNoImprovement check" which is only true for "River Ford" (at least on Terrain Large Rivers
				// TODO: Maybe create a new XML tag "bAllowLargeRiverMovement in XML of Terrain Features instead - might be cleaner
				bLandUnitMayPassLargeRiverDueToTerrainFeature = (kPlot.getFeatureType() != NO_FEATURE && GC.getFeatureInfo(kPlot.getFeatureType()).isTerrain(TERRAIN_LARGE_RIVERS) && GC.getFeatureInfo(kPlot.getFeatureType()).isNoImprovement());
				bLandUnitMayPassLargeRiverDueToProfession = (getProfession() != NO_PROFESSION && GC.getProfessionInfo(getProfession()).isCanCrossLargeRivers());
				bLandUnitMayBeLoaded = canLoad(&kPlot, false);
			}

			// stop large ships from entering Large Rivers in own Terrain
			// if (DOMAIN_SEA != getDomainType() || ePlotTeam != getTeam()) // sea units can enter impassable in own cultural borders
			if (DOMAIN_SEA != getDomainType() || ePlotTeam != getTeam() || kPlot.getTerrainType() == TERRAIN_LARGE_RIVERS || kPlot.getTerrainType() == TERRAIN_LAKE || kPlot.getTerrainType() == TERRAIN_ICE_LAKE || kPlot.getTerrainType() == TERRAIN_SHALLOW_COAST)
			{
				if (bIgnoreLoad || !canLoad(&kPlot, true))
				{
					if (bLandUnitMayPassLargeRiverDueToImprovement == false && bLandUnitMayPassLargeRiverDueToTerrainFeature == false && bLandUnitMayPassLargeRiverDueToProfession == false && bLandUnitMayBeLoaded == false)
					{
						return false;
					}
				}
			}
			//WTP, ray, Large Rivers - END
		}
	}

	// WTP, ray, new Harbour System - START
	// of course this is only checked if new Harbour System is enabled
	// We check this only for Humans, AI can ignore it because it is too difficult to teach it
	if (GLOBAL_DEFINE_ENABLE_NEW_HARBOUR_SYSTEM && isHuman())
	{
		// Stop Ships from Entering a City in which harbour is full - e might also check "automated"
		if (DOMAIN_SEA == getDomainType() && kPlot.isCity(true, getTeam()))
		{
			// Case real City
			if (kPlot.getImprovementType() == NO_IMPROVEMENT)
			{
				// this is only checked for Colonial Cities, Native Villages can always be entered
				CvCity* pCity = kPlot.getPlotCity();
				if (pCity != NULL)
				{
					if(!pCity->isNative())
					{
						int iHarbourSpaceNeededByUnit = getUnitInfo().getHarbourSpaceNeeded();

						// Caclulating free Harbour Space in City
						int iHarbourSpaceMaxInCity = kPlot.getPlotCity()->getCityHarbourSpace();
						int iHarbourSpaceUsedInCity = kPlot.getPlotCity()->getCityHarbourSpaceUsed();
						int iHarbourSpaceAvailableInCity = iHarbourSpaceMaxInCity - iHarbourSpaceUsedInCity;

						if (iHarbourSpaceNeededByUnit > iHarbourSpaceAvailableInCity)
						{
							return false;
						}
					}
				}
			}

			// Case "actAsCity" Improvement - also considered as City
			else
			{
				// just to ensure that something may be messed up in the future
				bool bWeCheckAllowedUnitsOnPlot = (kPlot.isFort() || kPlot.isMonastery() || kPlot.isCanal());
				if (bWeCheckAllowedUnitsOnPlot)
				{
					// this is how much the Unit needs
					int iHarbourSpaceNeededByUnit = getUnitInfo().getHarbourSpaceNeeded();

					// we check how many Units that place would allow
					int iImprovementHarbourSpace = GLOBAL_DEFINE_BASE_HARBOUR_SPACES_WITHOUT_BUILDINGS;
					// it is the second level Improvement, so we double - unless for canal, which has no upgrade
					if (GC.getImprovementInfo(kPlot.getImprovementType()).getImprovementUpgrade() == NO_IMPROVEMENT && !kPlot.isCanal())
					{
						iImprovementHarbourSpace = iImprovementHarbourSpace * 2;
					}

					// now we calculate how much is already used
					int iImprovementHarbourSpaceUsed = 0;
					for (int i = 0; i < kPlot.getNumUnits(); ++i)
					{
						CvUnit* pLoopUnit = kPlot.getUnitByIndex(i);
						// we only count Land Units that can attack, civil Units are not considered
						// we also not consider Units loaded on Ships
						// we also not consider Units of other Nations
						if (pLoopUnit != NULL && pLoopUnit->getDomainType() == DOMAIN_SEA)
						{
							iImprovementHarbourSpaceUsed += pLoopUnit->getUnitInfo().getHarbourSpaceNeeded();
						}
					}

					// we now know how much is available
					int iImprovementShipSpaceAvailable = iImprovementHarbourSpace - iImprovementHarbourSpaceUsed;
					if (iHarbourSpaceNeededByUnit > iImprovementShipSpaceAvailable)
					{
						return false;
					}
				}
			}
		}
	}
	// WTP, ray, new Harbour System - END

	// WTP, ray, new Barracks System - START
	// of course this is only checked if new Barracks System is enabled
	// We check this only for Humans, AI can ignore it because it is too difficult to teach it
	if (GLOBAL_DEFINE_ENABLE_NEW_BARRACKS_SYSTEM && isHuman())
	{
		// Stop Combat Land Units from Entering a City in which barracks are full - we might also check "automated"
		// here we also check just for can Attack - we check City owner further down
		if (DOMAIN_LAND == getDomainType() && kPlot.isCity(true, getTeam()) && canAttack())
		{
			// Case real City
			if (kPlot.getImprovementType() == NO_IMPROVEMENT)
			{
				// this is only checked for Colonial Cities, Native Villages can always be entered
				CvCity* pCity = kPlot.getPlotCity();
				if (pCity != NULL)
				{
					// here we ensure we check this only for the Owner of the Unit being Owner of the City
					if(!pCity->isNative() && pCity->getOwnerINLINE() == getOwnerINLINE())
					{
						int iBarracksSpaceNeededByUnit = getUnitInfo().getBarracksSpaceNeeded();

						// we also need to check the PRofession
						if (getProfession() != NO_PROFESSION)
						{
							iBarracksSpaceNeededByUnit += GC.getProfessionInfo(getProfession()).getBarracksSpaceNeededChange();
						}

						// Caclulating free Harbour Space in City
						int iBarracksSpaceMaxInCity = kPlot.getPlotCity()->getCityBarracksSpace();
						int iBarracksSpaceUsedInCity = kPlot.getPlotCity()->getCityBarracksSpaceUsed();
						int iBarracksSpaceAvailableInCity = iBarracksSpaceMaxInCity - iBarracksSpaceUsedInCity;

						if (iBarracksSpaceNeededByUnit > iBarracksSpaceAvailableInCity)
						{
							return false;
						}
					}
				}
			}

			// Case "actAsCity" Improvement - also considered as City
			else
			{
				// just to ensure that something may be messed up in the future
				bool bWeCheckAllowedUnitsOnPlot = (kPlot.isFort() || kPlot.isMonastery());
				if (bWeCheckAllowedUnitsOnPlot)
				{
					// this is how much the Unit needs
					int iBarracksSpaceNeededByUnit = getUnitInfo().getBarracksSpaceNeeded();
					// we also need to check the PRofession
					if (getProfession() != NO_PROFESSION)
					{
						iBarracksSpaceNeededByUnit += GC.getProfessionInfo(getProfession()).getBarracksSpaceNeededChange();
					}

					// we check how many Units that place would allow
					int iImprovementBarracksSpace = GLOBAL_DEFINE_BASE_BARRACKS_SPACES_WITHOUT_BUILDINGS;
					// it is the second level Improvement, so we double
					if (GC.getImprovementInfo(kPlot.getImprovementType()).getImprovementUpgrade() == NO_IMPROVEMENT)
					{
						iImprovementBarracksSpace = iImprovementBarracksSpace * 2;
					}

					// now we calculate how much is already used
					int iImprovementBarracksSpaceUsed = 0;
					for (int i = 0; i < kPlot.getNumUnits(); ++i)
					{
						CvUnit* pLoopUnit = kPlot.getUnitByIndex(i);
						// we only count Land Units that can attack, civil Units are not considered
						// we also not consider Units loaded on Ships
						// we also not consider Units of other Nations
						if (pLoopUnit != NULL && pLoopUnit->getDomainType() == DOMAIN_LAND && pLoopUnit->canAttack() && pLoopUnit->getTransportUnit() == NULL && pLoopUnit->getOwnerINLINE() == getOwnerINLINE())
						{
							iImprovementBarracksSpaceUsed += pLoopUnit->getUnitInfo().getBarracksSpaceNeeded();
							// we also need to consider Professions
							if (pLoopUnit->getProfession() != NO_PROFESSION)
							{
								iImprovementBarracksSpaceUsed += GC.getProfessionInfo(pLoopUnit->getProfession()).getBarracksSpaceNeededChange();
							}
						}
					}

					// we now know how much is available
					int iBarracksSpaceAvailable = iImprovementBarracksSpace - iImprovementBarracksSpaceUsed;
					if (iBarracksSpaceNeededByUnit > iBarracksSpaceAvailable)
					{
						return false;
					}
				}
			}
		}
	}
	// WTP, ray, new Barracks System - END


	if (m_pUnitInfo->getMoves() == 0)
	{
		return false;
	}

	switch (eDomainType)
	{
	case DOMAIN_SEA:
		if (!kPlot.isWater() && !m_pUnitInfo->isCanMoveAllTerrain())
		{
			if (!kPlot.isFriendlyCity(*this, true) || !kPlot.isCoastalLand())
			{
				return false;
			}
		}

		// PatchMod: Stop MoW's entering native settlements START
		if (kPlot.isCity() && GET_PLAYER(kPlot.getOwnerINLINE()).isNative())
		{
			if (GET_PLAYER(getOwnerINLINE()).isEurope())
			{
                return false;
			}
		}
		// PatchMod: Stop MoW's entering native settlements END
		// TAC - Pirates can't enter foreign settlements - START
		if (kPlot.isCity() && !GET_PLAYER(kPlot.getOwnerINLINE()).isNative() && kPlot.getOwnerINLINE() != getOwnerINLINE())
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
		if (kPlot.isPeak() && kPlot.getRouteType() == NO_ROUTE)
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
		if (m_pUnitInfo->isTreasure() && kPlot.isGoody())
		{
			return false;
		}

		// R&R, ray, Monasteries and Forts - START
		if (m_pUnitInfo->isAnimal())
		{
			//WTP, Protected Hostile Goodies - small adaptation
			if (kPlot.isFort() || kPlot.isMonastery() || kPlot.isCity() || kPlot.isGoodyForSpawningHostileCriminals() || kPlot.isGoodyForSpawningHostileNatives())
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
			//Check if the Plot has plastered road or railroad - needs to be adjusted
			//WTP, ray, now Carriages may also use Ferry Stations
			//if (kPlot.getRouteType() != 2)
			if (kPlot.getRouteType() < 2)
			{
				bool bLandUnitMayPassLargeRiverDueToImprovement = kPlot.getTerrainType() == TERRAIN_LARGE_RIVERS && (kPlot.getImprovementType() != NO_IMPROVEMENT && GC.getImprovementInfo(kPlot.getImprovementType()).getTerrainMakesValid(TERRAIN_LARGE_RIVERS) && GC.getImprovementInfo(kPlot.getImprovementType()).isOutsideBorders());

				if (!bLandUnitMayPassLargeRiverDueToImprovement)
				{
					return false;
				}
			}
		}
		// R&R, ray, End Logic for Trains

		if (kPlot.isWater() && !m_pUnitInfo->isCanMoveAllTerrain())
		{
			if (bIgnoreLoad || plot()->isWater() || !canLoad(&kPlot, false))
			{
				//WTP, ray, Large Rivers - START
				// allowing all Land Units to enter Large Rivers
				// further below we will code exceptions - Maybe also configure in XML
				//return false;
				if(kPlot.getTerrainType() != TERRAIN_LARGE_RIVERS)
				{
					return false;
				}
				//WTP, ray, Large Rivers - END
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
		if (isNoCityCapture() && kPlot.isEnemyCity(*this))
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
			if (!isHuman() || (kPlot.isVisible(getTeam(), false)))
			{
				if (kPlot.isVisibleEnemyUnit(this) != bAttack)
				{
					//FAssertMsg(isHuman() || (!bDeclareWar || (pPlot->isVisibleOtherUnit(getOwnerINLINE()) != bAttack)), "hopefully not an issue, but tracking how often this is the case when we dont want to really declare war");
					if (!bDeclareWar || (kPlot.isVisibleOtherUnit(getOwnerINLINE()) != bAttack && !(bAttack && kPlot.getPlotCity() && !isNoCityCapture())))
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
			if (!isHuman() || kPlot.isVisible(getTeam(), false))
			{
				if (kPlot.isEnemyCity(*this))
				{
					return false;
				}

				if (kPlot.isVisibleEnemyUnit(this))
				{
					return false;
				}
			}
		}
	}

	if (isHuman())
	{
		ePlotTeam = kPlot.getRevealedTeam(getTeam(), false);
		bCanEnterArea = canEnterArea(kPlot.getRevealedOwner(getTeam(), false), pPlotArea);
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
				if (!(getGroup()->AI_isDeclareWar(&kPlot)))
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
		argsList.add(kPlot.getX());	// Plot X
		argsList.add(kPlot.getY());	// Plot Y
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
	return (canMoveInto(*pPlot, false, bDeclareWar) || canMoveInto(*pPlot, true, bDeclareWar));
}


bool CvUnit::canMoveThrough(const CvPlot* pPlot) const
{
	return canMoveInto(*pPlot, false, false, true);
}


void CvUnit::attack(CvPlot* pPlot, bool bQuick)
{
	FAssert(canMoveInto(*pPlot, true));
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

	jumpTo(pPlot->coord(), true, true, bShow, bShow);

	// R&R, Robert Surcouf, Damage on Storm plots, Start
	// R&R, bugfix: we only damage ships on Sea and not the transported units, ray, START
	// WTP, ray, we also add logic to damage Units on Land by Land Storms like e.g. Blizzard and Sandstorm
	if (getDomainType() == DOMAIN_SEA || (getDomainType() == DOMAIN_LAND && !plot()->isWater()))
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
					CvDLLEngineIFaceBase::TriggerEffectArguments arguments;

					arguments.effect = GC.getFeatureInfo(featureType).getEffectType();
					if (arguments.effect != NO_EFFECT)
					{
						arguments.pt3Point = pPlot->getPoint();
						arguments.rotation = GC.getASyncRand().get(360);
						arguments.audio = "AS3D_UN_BIRDS_SCATTER";
						gDLL->getEngineIFace()->TriggerEffect(arguments);
					}
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

	CvCity* pNearestCity = GC.getMap().findCity(coord(), getOwnerINLINE());
	int iBestValue = MAX_INT;
	CvPlot* pBestPlot = NULL;

	for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(iI);

		if (isValidPlot(pLoopPlot))
		{
			if (canMoveInto(*pLoopPlot))
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
						int iValue = (plotDistance(coord(), pLoopPlot->coord()) * 2);

						if (pNearestCity != NULL)
						{
							iValue += plotDistance(pLoopPlot->coord(), pNearestCity->coord());
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
		jumpTo(pBestPlot->coord());
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

		if (pLoopUnit != NULL && pLoopUnit->getUnitInfo().getTerrainImpassable(TERRAIN_OCEAN))
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

	case AUTOMATE_SAIL_TO_EUROPE:
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

			// WTP, ray Slave Ship
			// we allow Slave Ships to sail to Port Royal as well
			if (pLoopUnit != NULL && !pLoopUnit->getUnitInfo().isHiddenNationality() && !pLoopUnit->getUnitInfo().isSlaveShip())
			{
				return false;
			}
			pUnitNode = getGroup()->nextUnitNode(pUnitNode);
		}
		break;

	//TAC Whaling, ray
	case AUTOMATE_TRANSPORT_ROUTES:
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
			CvUnit* pLoopUnit = pPlot->getUnitNodeLoop(pUnitNode);

			if (pLoopUnit != NULL && pLoopUnit->getTransportUnit() == this)
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
		pLoopUnit = pPlot->getUnitNodeLoop(pUnitNode);

		if (pLoopUnit != NULL && pLoopUnit->getTransportUnit() == this)
		{
			pLoopUnit->gift(false);
		}
	}

	FAssertMsg(plot()->getOwnerINLINE() != NO_PLAYER, "plot()->getOwnerINLINE() is not expected to be equal with NO_PLAYER");
	OOS_LOG("gift unit", getTypeStr(getUnitType()));
	pGiftUnit = GET_PLAYER(plot()->getOwnerINLINE()).initUnit(getUnitType(), getProfession(), getX_INLINE(), getY_INLINE(), AI_getUnitAIType(), getFacingDirection(false), getYieldStored());

	FAssertMsg(pGiftUnit != NULL, "GiftUnit is not assigned a valid value");

	eOwner = getOwnerINLINE();

	pGiftUnit->convert(this, true);

	int iUnitValue = 0;
	for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
	{
		iUnitValue += pGiftUnit->getUnitInfo().getYieldCost(eYield);
	}
	GET_PLAYER(pGiftUnit->getOwnerINLINE()).AI_changePeacetimeGrantValue(eOwner, iUnitValue / 5);

	szBuffer = gDLL->getText("TXT_KEY_MISC_GIFTED_UNIT_TO_YOU", GET_PLAYER(eOwner).getNameKey(), pGiftUnit->getNameKey());
	gDLL->UI().addPlayerMessage(pGiftUnit->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNITGIFTED", MESSAGE_TYPE_INFO, pGiftUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pGiftUnit->getX_INLINE(), pGiftUnit->getY_INLINE(), true, true);

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

	// WTP, ray, let us not have Land Transports transport other Land Transports, it feels stupid and might allow exploits - START
	bool bTransportedUnitIsLandTransport = (getDomainType() == DOMAIN_LAND && getUnitInfo().getCargoSpace() > 0);
	bool bTransportingUnitIsLandTransport = (pTransport->getDomainType() == DOMAIN_LAND && getUnitInfo().getCargoSpace() > 0);
	if (bTransportedUnitIsLandTransport && bTransportingUnitIsLandTransport)
	{
		return false;
	}
	// WTP, ray, let us not have Land Transports transport other Land Transports, it feels stupid and might allow exploits - END

	// WTP, ray Slave Ship - START
	// a Slave Ship can only carry Slaves or Goods
	// to avoid AI issues - only for Human player
	if (pTransport->getUnitInfo().isSlaveShip() && isHuman())
	{
		// it is neither Goods nor a Slave
		if (getSpecialUnitType() == NO_SPECIALUNIT && !getUnitInfo().LbD_canRevolt() && getUnitInfo().getProductionWhenUsed() <= 0)
		{
			return false;
		}
	}
	// WTP, ray Slave Ship - END

	// WTP, ray Treasure Ship - START
	// a Treasure Ship can only carry Treasures or Goods
	// to avoid AI issues - only for Human player
	if (pTransport->getUnitInfo().isTreasureShip() && isHuman())
	{
		// it is neither Goods nor a Slave
		// WTP, ray, Construction Supplies - START
		if (getSpecialUnitType() == NO_SPECIALUNIT && !getUnitInfo().isTreasure() && getUnitInfo().getProductionWhenUsed() <= 0)
		{
			return false;
		}
	}
	// WTP, ray Treasure Ship - END

	// WTP, ray Troop Ship - START
	// a Troop Ship can only carry Troops or Goods
	// to avoid AI issues - only for Human player
	if (pTransport->getUnitInfo().isTroopShip() && isHuman())
	{
		// it is neither Goods nor a Slave
		if (getSpecialUnitType() == NO_SPECIALUNIT && !canAttack() && getUnitClassType() != UNITCLASS_GREAT_GENERAL && getUnitClassType() != UNITCLASS_GREAT_ADMIRAL && getUnitClassType() != UNITCLASS_BRAVE_LIEUTENANT && getUnitClassType() != UNITCLASS_CAPABLE_CAPTAIN && getUnitInfo().getProductionWhenUsed() <= 0)
		{
			return false;
		}
	}
	// WTP, ray Treasure Ship - END

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

	// check if the unit is trying to flee a city with unrest
	if (!canLeaveCity())
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

	if (m_pUnitInfo->getTerrainImpassable(pPlot->getTerrainType()))
	{
		//WTP, ray, fix for Transports catching Units on Large River - could happen on Fords and on Ferries - START
		if (pPlot->getFeatureType() == NO_FEATURE && pPlot->getImprovementType() == NO_IMPROVEMENT)
		{
			// Added destination check by Nightinggale
			CvSelectionGroup* pGroup = getGroup();
			if (pGroup && pGroup->headMissionQueueNode() && pGroup->headMissionQueueNode()->m_data.eMissionType == MISSION_MOVE_TO)
			{
				// unit is moving into terrain, which will normally require loading on to a ship
				if (pGroup->headMissionQueueNode()->m_data.iData1 != pPlot->getX_INLINE()
					|| pGroup->headMissionQueueNode()->m_data.iData2 != pPlot->getY_INLINE())
				{
					// unit is moving on to a plot on the other side, possibly with automated moves
					// unit should not board and instead let it continue to the other side
					return false;
				}

			}
			return true;
		}
		else
		{
			return false;
		}
		//WTP, ray, fix for Transports catching Units on Large River - could happen on Fords and on Ferries - END
	}

	if (!pPlot->isValidDomainForLocation(*this))
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
	for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
	{
		if(canLoadYield(pPlot, eYield, bTrade))
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
			// R&R, ray, improvement
			if (!isHuman() && (pCity->getTotalYieldStored() > pCity->getMaxYieldCapacity() / 2))
			{
				// ray, making special storage capacity rules for Yields XML configurable
				int iCargoYields = 0;
				// for (YieldTypes eLoopYield = YIELD_HEMP; eLoopYield < NUM_YIELD_TYPES; ++eLoopYield)// without YIELD_FOOD, YIELD_LUMBER, YIELD_STONE
				for (YieldTypes eLoopYield = FIRST_YIELD; eLoopYield < NUM_YIELD_TYPES; ++eLoopYield)
				{
					// ray, making special storage capacity rules for Yields XML configurable
					if ((pCity->getYieldStored(eLoopYield) > 0) && (GC.getYieldInfo(eLoopYield).isCargo()) && !(GC.getYieldInfo(eLoopYield).isIgnoredForStorageCapacity()))
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

	//WTP, forbid Gather Boats to Trade with Natives - START
	if (getUnitInfo().isGatherBoat())
	{
		return false;
	}
	//WTP, forbid Gather Boats to Trade with Natives - START

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
		for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
		{
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

	const CvPlayer& kPlayer = GET_PLAYER(getOwnerINLINE());

	const UnitTypes eUnit = kPlayer.getUnitType(GLOBAL_DEFINE_DEFAULT_POPULATION_UNIT);
	if (eUnit == NO_UNIT)
	{
		return false;
	}

	//WTP, fix crash for clearing Unit Specialization in revolting City - START
	CvCity* pCity = kPlayer.getPopulationUnitCity(getID());
	if (pCity != NULL)
	{
		if (pCity->isOccupation())
		{
			return false;
		}
	}
	//WTP, fix crash for clearing Unit Specialization in revolting City - END

	return true;
}

void CvUnit::clearSpecialty()
{
	if (!canClearSpecialty())
	{
		return;
	}

	const bool bLocked = isColonistLocked();
	CvPlayer& kPlayer = GET_PLAYER(getOwnerINLINE());

	UnitTypes eUnit = kPlayer.getUnitType(GLOBAL_DEFINE_DEFAULT_POPULATION_UNIT);
	CvUnit* pNewUnit = kPlayer.initUnit(eUnit, NO_PROFESSION, getX_INLINE(), getY_INLINE(), AI_getUnitAIType());
	FAssert(pNewUnit != NULL);

	CvCity *pCity = kPlayer.getPopulationUnitCity(getID());
	if (pCity != NULL)
	{
		pNewUnit->convert(this, false);
		pCity->replaceCitizen(pNewUnit->getID(), getID(), false);
		pNewUnit->setColonistLocked(bLocked);
		// TAC - Clear specialty fix - koma13 - START
		//pCity->removePopulationUnit(this, true, NO_PROFESSION);
		if (kPlayer.getPopulationUnitCity(getID()) != NULL)
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
	// WTP, ray Slave Ship
	// we allow Slave Ships to sail to Port Royal as well
	if (!getUnitInfo().isHiddenNationality() && !getUnitInfo().isSlaveShip())
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

		if (pLoopUnit != NULL && pLoopUnit->getTransportUnit() == NULL)
		{
			// WTP, ray Slave Ship
			// we allow Slave Ships to sail to Port Royal as well
			if (!pLoopUnit->getUnitInfo().isHiddenNationality() && !getUnitInfo().isSlaveShip())
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
		CvUnit* pLoopUnit = pPlot->getUnitNodeLoop(pUnitNode);

		if (pLoopUnit != NULL && canLoadUnit(pLoopUnit, pPlot, bCheckCity))
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

	for (iPass = 0; iPass < 3; iPass++)
	{
		pUnitNode = pPlot->headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = pPlot->getUnitNodeLoop(pUnitNode);

			if (pLoopUnit != NULL && canLoadUnit(pLoopUnit, pPlot, bCheckCity))
			{
				// First pass matches only ships that are not sleeping, subsequent matches ignore activity
				if ((iPass == 0 && pLoopUnit->getGroup()->getActivityType() != ACTIVITY_SLEEP && pLoopUnit->getOwnerINLINE() == getOwnerINLINE()) ||
					(iPass == 1 && pLoopUnit->getOwnerINLINE() == getOwnerINLINE()) ||
					(iPass == 2 && pLoopUnit->getTeam() == getTeam()))
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

	// WTP, ray, Barracks System, check if there is still enough Barracks Space - START
	// Comment: I am afraid to damage AI logc thus I will only check this for Human
	if (GLOBAL_DEFINE_ENABLE_NEW_BARRACKS_SYSTEM && isHuman())
	{
		// Case real City
		if (plot()->getImprovementType() == NO_IMPROVEMENT)
		{
			// this is only checked for Colonial Cities, Native Villages can always be entered
			CvCity* pCity = plot()->getPlotCity();
			if (pCity != NULL)
			{
				// here we ensure we check this only for the Owner of the Unit being Owner of the City
				if(!pCity->isNative() && pCity->getOwnerINLINE() == getOwnerINLINE())
				{
					int iBarracksSpaceNeededByUnit = getUnitInfo().getBarracksSpaceNeeded();
					if (getProfession() != NO_PROFESSION)
					{
						iBarracksSpaceNeededByUnit += GC.getProfessionInfo(getProfession()).getBarracksSpaceNeededChange();
					}

					// WTP, ray, fix issue of unloading Non-Military Units not allowed
					if (iBarracksSpaceNeededByUnit > 0)
					{
						// Caclulating free Barracks Space in City
						int iBarracksSpaceMaxInCity = plot()->getPlotCity()->getCityBarracksSpace();
						int iBarracksSpaceUsedInCity = plot()->getPlotCity()->getCityBarracksSpaceUsed();
						int iBarracksSpaceAvailableInCity = iBarracksSpaceMaxInCity - iBarracksSpaceUsedInCity;

						if (iBarracksSpaceNeededByUnit > iBarracksSpaceAvailableInCity)
						{
							return false;
						}
					}
				}
			}
		}

		// Case "actAsCity" Improvement - also considered as City
		else
		{
			// just to ensure that something may be messed up in the future
			bool bWeCheckAllowedUnitsOnPlot = (plot()->isFort() || plot()->isMonastery());
			if (bWeCheckAllowedUnitsOnPlot)
			{
				int iBarracksSpaceNeededByUnit = getUnitInfo().getBarracksSpaceNeeded();
				if (getProfession() != NO_PROFESSION)
				{
					iBarracksSpaceNeededByUnit += GC.getProfessionInfo(getProfession()).getBarracksSpaceNeededChange();
				}

				// we check how many Units that place would allow
				int iImprovementBarracksSpace = GLOBAL_DEFINE_BASE_BARRACKS_SPACES_WITHOUT_BUILDINGS;
				// it is the second level Improvement, so we double
				if (GC.getImprovementInfo(plot()->getImprovementType()).getImprovementUpgrade() == NO_IMPROVEMENT)
				{
					iImprovementBarracksSpace = iImprovementBarracksSpace * 2;
				}

				// now we calculate how much is already used
				int iImprovementBarracksSpaceUsed = 0;
				for (int i = 0; i < plot()->getNumUnits(); ++i)
				{
					CvUnit* pLoopUnit = plot()->getUnitByIndex(i);
					// we only count Land Units that can attack, civil Units are not considered
					// we also not consider Units loaded on Ships
					// we also not consider Units of other Nations
					if (pLoopUnit != NULL && pLoopUnit->getDomainType() == DOMAIN_LAND && pLoopUnit->canAttack() && pLoopUnit->getTransportUnit() == NULL && pLoopUnit->getOwnerINLINE() == getOwnerINLINE())
					{
						iImprovementBarracksSpaceUsed += pLoopUnit->getUnitInfo().getBarracksSpaceNeeded();
						// we also need to consider Professions
						if (pLoopUnit->getProfession() != NO_PROFESSION)
						{
							iImprovementBarracksSpaceUsed += GC.getProfessionInfo(pLoopUnit->getProfession()).getBarracksSpaceNeededChange();
						}
					}
				}

				// we now know how much is available
				int iBarracksSpaceAvailable = iImprovementBarracksSpace - iImprovementBarracksSpaceUsed;
				if (iBarracksSpaceNeededByUnit > iBarracksSpaceAvailable)
				{
					return false;
				}
			}
		}
	}
	// WTP, ray, Barracks System, check if there is still enough Barracks Space - END

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
		CvUnit* pLoopUnit = pPlot->getUnitNodeLoop(pUnitNode);

		if (pLoopUnit != NULL && pLoopUnit->getTransportUnit() == this)
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
		pLoopUnit = pPlot->getUnitNodeLoop(pUnitNode);

		if (pLoopUnit != NULL && pLoopUnit->getTransportUnit() == this)
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
		gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_LEARN).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);
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
			CvUnit* pLoopUnit = plot()->getUnitNodeLoop(pUnitNode);
			// this checks if there is already another Native Living in this village
			// if yes, we send a message and exit the method by return
			if (pLoopUnit != NULL && pLoopUnit->getUnitTravelState() == UNIT_TRAVEL_STATE_LIVE_AMONG_NATIVES)
			{
				CvWString szBuffer = gDLL->getText("TXT_KEY_NATIVES_NOT_WILLING_ALREADY_COLONIST_LEARNING", plot()->getPlotCity()->getNameKey());
				gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_LEARN).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);
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

	pCity->setTeachUnitMultiplier(pCity->getTeachUnitMultiplier() * (100 + GLOBAL_DEFINE_NATIVE_TEACH_THRESHOLD_INCREASE) / 100);
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

	OOS_LOG("CvUnit::doLearn", getTypeStr(eUnitType));
	CvUnit* pLearnUnit = GET_PLAYER(getOwnerINLINE()).initUnit(eUnitType, getProfession(), getX_INLINE(), getY_INLINE(), AI_getUnitAIType());
	FAssert(pLearnUnit != NULL);
	pLearnUnit->joinGroup(getGroup());
	pLearnUnit->convert(this, true);

	gDLL->getEventReporterIFace()->unitLearned(pLearnUnit->getOwnerINLINE(), pLearnUnit->getID());

	if (gUnitLogLevel >= 1)
	{
		CvWString szTempString;
		getUnitAIString(szTempString, AI_getUnitAIType());

		logBBAI(" Player %S Unit %S has finished learning with UnitAI (%S)", GET_PLAYER(getOwnerINLINE()).getCivilizationDescription(),
			getNameAndProfession().GetCString(), szTempString.GetCString());
	}
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

	// this here is basically pointless now, but I still leave it in because it might be faster than below
	if (!pCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
	{
		return false;
	}

	// WTP, ray, fix small issue with King Treasure Transport at Lakes - START
	// this is just needed for incredibly huge Lakes or Ice Lakes
	// if there is no TERRAIN_COAST, TERRAIN_SHALLOW_COAST or TERRAIN_LARGE_RIVER there can be no Ocean access
	if (!plot()->hasNearbyPlotWith(TERRAIN_COAST) && !plot()->hasNearbyPlotWith(TERRAIN_SHALLOW_COAST) && !plot()->hasNearbyPlotWith(TERRAIN_LARGE_RIVERS))
	{
		return false;
	}
	// WTP, ray, fix small issue with King Treasure Transport at Lakes - END

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
		// WTP, ray, refactored according to advice of Nightinggale
		for (BuildingTypes eLoopBuilding = FIRST_BUILDING; eLoopBuilding < NUM_BUILDING_TYPES; ++eLoopBuilding)
		{
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
		int iCommission = GLOBAL_DEFINE_KING_TRANSPORT_TREASURE_COMISSION;
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
	GET_PLAYER(getOwnerINLINE()).sellYieldUnitToEurope(this, getYieldStored(), GLOBAL_DEFINE_KING_TRANSPORT_TREASURE_COMISSION);
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
	CvPlayer& kUnitOwner = GET_PLAYER(getOwnerINLINE());

	// R&R, ray, Natives do not talk when furious - START
	int currentPlayerAttitude = kCityOwner.AI_getAttitude(getOwnerINLINE(), false);
	if (isHuman() &&  currentPlayerAttitude == 0)
	{
		CvWString szBuffer = gDLL->getText("TXT_KEY_NATIVES_NOT_WILLING_TO_TALK", pCity->getNameKey());
		gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_ESTABLISH_MISSION).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);
		return;
	}
	// R&R, ray, Natives do not talk when furious - ELSE

	if (GC.getGameINLINE().getSorenRandNum(100, "Mission failure roll") > getMissionarySuccessPercent())
	{
		CvWString szBuffer = gDLL->getText("TXT_KEY_MISSION_FAILED", plot()->getPlotCity()->getNameKey());
		gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_ESTABLISH_MISSION).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
		kCityOwner.AI_changeMemoryCount((getOwnerINLINE()), MEMORY_MISSIONARY_FAIL, 1);

		//Ramstormp, Disillusioned missionary - START
		if (GC.getGameINLINE().getSorenRandNum(100, "Dis mission roll") < getFailedMissionarySurvivalPercent())
		{
			UnitTypes FailedMissionaryType = kUnitOwner.getUnitType(UNITCLASS_FAILED_MISSIONARY);
			// WTP, ray, just for safety
			if (FailedMissionaryType != NO_UNIT)
			{
				CvUnit* FailedMissionaryUnit = GET_PLAYER(getOwnerINLINE()).initUnit(FailedMissionaryType, GC.getUnitInfo(FailedMissionaryType).getDefaultProfession(), pCity->getX_INLINE(), pCity->getY_INLINE());
				//  WTP, ray, we still need to display a message - your missionary failed and became a Failed Missionary
				CvWString szBuffer = gDLL->getText("TXT_KEY_FAILED_MISSIONARY_SPAWNED_FROM_FAIL", plot()->getPlotCity()->getNameKey());
				gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_ESTABLISH_MISSION).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
			}
		}
		//Ramstormp, Disillusioned missionary - END
	}

	else
	{
		kUnitOwner.setMissionarySuccessPercent(kUnitOwner.getMissionarySuccessPercent() * GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getMissionFailureThresholdPercent() / 100);

		int iMissionaryRate = GC.getProfessionInfo(getProfession()).getMissionaryRate() * (100 + getUnitInfo().getMissionaryRateModifier()) / 100;
		if (!isHuman())
		{
			iMissionaryRate = (iMissionaryRate * 100 + 50) / GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIGrowthPercent();
		}

		// R&R, ray, Rebuild Missioning Start
		int attitudeIncreaseForMission = GLOBAL_DEFINE_GAIN_ATTITUDE_FOR_MISSION;
		kCityOwner.AI_changeAttitudeExtra(getOwnerINLINE(), attitudeIncreaseForMission);
		if (pCity->getMissionaryPlayer() != NO_PLAYER)
		{
			CvPlayer& oldMissionaryPlayer = GET_PLAYER(pCity->getMissionaryPlayer());
			const int attitudeDecreaseForDestroyingMission = GLOBAL_DEFINE_OTHER_EUROPEAN_ANGRY_FOR_DESTROYING_MISSION;
			oldMissionaryPlayer.AI_changeAttitudeExtra(getOwnerINLINE(), -attitudeDecreaseForDestroyingMission);

			//Ramstormp, Disillusioned Missionary - START
			if (GC.getGameINLINE().getSorenRandNum(100, "Dis missal roll") < GLOBAL_DEFINE_EXPELLED_MISSIONARY_SURVIVAL_CHANCE)
			{
				const UnitTypes FailedMissionaryType = kUnitOwner.getUnitType(UNITCLASS_FAILED_MISSIONARY);
				if (FailedMissionaryType != NO_UNIT)
				{
					CvUnit* FailedMissionaryUnit = oldMissionaryPlayer.initUnit(FailedMissionaryType, GC.getUnitInfo(FailedMissionaryType).getDefaultProfession(), pCity->getX_INLINE(), pCity->getY_INLINE());
					//  WTP, ray, we still need to display a message - your missionary was thrown out because new Mission and became a Failed Missionary
					CvWString szBuffer = gDLL->getText("TXT_KEY_FAILED_MISSIONARY_SPAWNED_FROM_MISSION_REPLACED", plot()->getPlotCity()->getNameKey());
					gDLL->UI().addPlayerMessage(pCity->getMissionaryPlayer(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_ESTABLISH_MISSION).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
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
		gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_ESTABLISH_TRADE_POST).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);
		return;
	}
	// R&R, ray, Natives do not talk when furious - ELSE

	if (GC.getGameINLINE().getSorenRandNum(100, "Trade Post failure roll") > getNativeTradePostSuccessPercent())
	{
		CvWString szBuffer = gDLL->getText("TXT_KEY_TRADE_POST_FAILED", plot()->getPlotCity()->getNameKey());
		gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_ESTABLISH_TRADE_POST).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
		// removed the Memory Count because it display strange unfitting since it is not a Mission and there is no special Memory Count for Trade Posts
		//GET_PLAYER(pCity->getOwnerINLINE()).AI_changeMemoryCount((getOwnerINLINE()), MEMORY_MISSIONARY_FAIL, 1);

		// WTP, ray, Failed Trader - START
		// we use the same configurations for the survival of Trader as for Missionary
		if (GC.getGameINLINE().getSorenRandNum(100, "Dis mission roll") < getFailedTraderSurvivalPercent())
		{
			UnitTypes FailedTraderType = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getDefineINT("UNITCLASS_FAILED_TRADER"));
			// WTP, ray, just for safety
			if (FailedTraderType != NO_UNIT)
			{
				CvUnit* FailedTraderUnit = GET_PLAYER(getOwnerINLINE()).initUnit(FailedTraderType, GC.getUnitInfo(FailedTraderType).getDefaultProfession(), pCity->getX_INLINE(), pCity->getY_INLINE());
				//  WTP, ray, we still need to display a message - your trader failed and became a Failed Trader
				CvWString szBuffer = gDLL->getText("TXT_KEY_FAILED_TRADER_SPAWNED_FROM_FAIL", plot()->getPlotCity()->getNameKey());
				gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_ESTABLISH_TRADE_POST).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
			}
		}
		// WTP, ray, Failed Trader - END
	}
	else
	{
		CvPlayer& kUnitOwner = GET_PLAYER(getOwnerINLINE());

		kUnitOwner.setNativeTradePostSuccessPercent(kUnitOwner.getNativeTradePostSuccessPercent() * GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getMissionFailureThresholdPercent() / 100);

		int iNativeTradeRate = GC.getProfessionInfo(getProfession()).getNativeTradeRate() * (100 + getUnitInfo().getNativeTradeRateModifier()) / 100;
		if (!isHuman())
		{
			iNativeTradeRate = (iNativeTradeRate * 100 + 50) / GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIGrowthPercent();
		}

		// change Attitudes
		int attitudeIncreaseForTradePost = GLOBAL_DEFINE_GAIN_ATTITUDE_FOR_TRADE_POST;
		kCityOwner.AI_changeAttitudeExtra(getOwnerINLINE(), attitudeIncreaseForTradePost);
		if (pCity->getTradePostPlayer() != NO_PLAYER)
		{
			CvPlayer& oldTradePostPlayer = GET_PLAYER(pCity->getTradePostPlayer());
			int attitudeDecreaseForDestroyingTradePost = GLOBAL_DEFINE_OTHER_EUROPEAN_ANGRY_FOR_DESTROYING_TRADE_POST;
			oldTradePostPlayer.AI_changeAttitudeExtra(getOwnerINLINE(), -attitudeDecreaseForDestroyingTradePost);

			// WTP, ray, Failed Trader - START
			// we use the same configurations for expelling existing Trader as for Missionary
			if (GC.getGameINLINE().getSorenRandNum(100, "Dis missal roll") < GLOBAL_DEFINE_EXPELLED_MISSIONARY_SURVIVAL_CHANCE)
			{
				const UnitTypes FailedTraderType = kUnitOwner.getUnitType(UNITCLASS_FAILED_TRADER);
				if (FailedTraderType != NO_UNIT)
				{
					//  WTP, ray, we still need to display a message - your trader was thrown out because new Trader and became a Failed Trader
					CvUnit* FailedTraderUnit = oldTradePostPlayer.initUnit(FailedTraderType, GC.getUnitInfo(FailedTraderType).getDefaultProfession(), pCity->getX_INLINE(), pCity->getY_INLINE());
					CvWString szBuffer = gDLL->getText("TXT_KEY_FAILED_TRADER_SPAWNED_FROM_TRADE_POST_REPLACED", plot()->getPlotCity()->getNameKey());
					gDLL->UI().addPlayerMessage(pCity->getTradePostPlayer(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_ESTABLISH_TRADE_POST).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
				}
			}
			// WTP, ray, Failed Trader - END
		}

		pCity->setTradePostPlayer(getOwnerINLINE());
		pCity->setNativeTradeRate(iNativeTradeRate);

		const int gameSpeedMod = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
		for (FatherPointTypes ePointType = FIRST_FATHER_POINT; ePointType <NUM_FATHER_POINT_TYPES; ++ePointType)
		{
			kUnitOwner.changeFatherPoints(ePointType, GC.getFatherPointInfo(ePointType).getMissionaryPoints() * gameSpeedMod/100);
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
		return GET_PLAYER(getOwnerINLINE()).getMissionarySuccessPercent() * (100 + (getUnitInfo().getMissionaryRateModifier() * GLOBAL_DEFINE_MISSIONARY_RATE_EFFECT_ON_SUCCESS / 100)) / 100;
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
	int attitudeChanceImprovement = GLOBAL_DEFINE_CHANCE_IMPROVEMENT_EACH_ATTITIUDE_LEVEL;
	int penaltyExisingMission = 0;
	int attitudeNativeToOldMissioningPlayer = 0;

	if (pCity->getMissionaryPlayer() != NO_PLAYER)
	{
		attitudeNativeToOldMissioningPlayer = kCityOwner.AI_getAttitude(pCity->getMissionaryPlayer(), false);
		penaltyExisingMission = GLOBAL_DEFINE_CHANCE_PENALTY_OTHER_MISSION_ALREADY_EXISTS;
	}

	// 4. Getting the expertmodifier of this unit
	int expertModifier = getUnitInfo().getMissionaryRateModifier();

	// 5. Getting Min and Max
	int minChance = GLOBAL_DEFINE_MIN_CHANCE_MISSIONING;
	int maxChance = GLOBAL_DEFINE_MAX_CHANCE_MISSIONING;

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
		survivalChance = GLOBAL_DEFINE_FAILED_EXPERT_MISSIONARY_SURVIVAL_CHANCE;
	}

	// otherwise it is not an Expert
	else
	{
		survivalChance = GLOBAL_DEFINE_FAILED_REGULAR_MISSIONARY_SURVIVAL_CHANCE;
	}

	// just for safety if XML config was messed up, checking for smaller 0 is enough
	if (survivalChance < 0)
	{
		return 0;
	}
	return survivalChance;
}
//Ramstormp, Disillusioned missionary - END


//Ramstormp, Disillusioned missionary - START
int CvUnit::getFailedTraderSurvivalPercent() const
{
	int survivalChance = 0;

	// this is checking for an Expert
	if (getUnitInfo().getNativeTradeRateModifier() > 0)
	{
		survivalChance = GLOBAL_DEFINE_FAILED_EXPERT_MISSIONARY_SURVIVAL_CHANCE;
	}

	// otherwise it is not an Expert
	else
	{
		survivalChance = GLOBAL_DEFINE_FAILED_REGULAR_MISSIONARY_SURVIVAL_CHANCE;
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
		return GET_PLAYER(getOwnerINLINE()).getNativeTradePostSuccessPercent() * (100 + (getUnitInfo().getNativeTradeRateModifier() * GLOBAL_DEFINE_TRADER_RATE_EFFECT_ON_SUCCESS / 100)) / 100;
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
	int attitudeChanceImprovement = GLOBAL_DEFINE_CHANCE_IMPROVEMENT_EACH_ATTITIUDE_LEVEL;
	int penaltyExisingTradePost = 0;
	int attitudeNativeToOldTradePostPlayer = 0;

	if (pCity->getTradePostPlayer() != NO_PLAYER)
	{
		attitudeNativeToOldTradePostPlayer = kCityOwner.AI_getAttitude(pCity->getTradePostPlayer(), false);
		penaltyExisingTradePost = GLOBAL_DEFINE_CHANCE_PENALTY_OTHER_TRADE_POST_ALREADY_EXISTS;
	}

	// 4. Getting the expertmodifier of this unit
	int expertModifier = getUnitInfo().getNativeTradeRateModifier();

	// 5. Getting Min and Max
	const int minChance = GLOBAL_DEFINE_MIN_CHANCE_TRADE_POST;
	const int maxChance = GLOBAL_DEFINE_MAX_CHANCE_TRADE_POST;

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
		gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_STIR_UP_NATIVES).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);
	}
	// R&R, ray, Natives do not talk when furious - ELSE

	else if (GC.getGameINLINE().getSorenRandNum(100, "Stir Up failure roll") > getStirUpSuccessPercent())
	{
		CvWString szBuffer = gDLL->getText("TXT_KEY_STIR_UP_FAILED", plot()->getPlotCity()->getNameKey());
		gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_STIR_UP_NATIVES).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);

		kCityOwner.AI_changeAttitudeExtra(getOwnerINLINE(), -1);
	}
	else
	{
		CvWString szBuffer = gDLL->getText("TXT_KEY_STIR_UP_SUCCESSFULL", plot()->getPlotCity()->getNameKey());
		gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_STIR_UP_NATIVES).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), getX_INLINE(), getY_INLINE(), true, true);

		const UnitTypes DefaultUnitType = kCityOwner.getUnitType(GLOBAL_DEFINE_DEFAULT_POPULATION_UNIT);
		if (NO_UNIT != DefaultUnitType)
		{
			CvUnit* pUnit = kCityOwner.initUnit(DefaultUnitType, GC.getCivilizationInfo(kCityOwner.getCivilizationType()).getDefaultProfession(), getX_INLINE(), getY_INLINE());

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

	const int stirUpBaseChance = GLOBAL_DEFINE_STIR_UP_BASE_CHANCE;
	const int stirUpAttitudeChange = GLOBAL_DEFINE_STIR_UP_CHANCE_INCREASE_PER_ATTITUDE_LEVEL;

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

	const int max = GLOBAL_DEFINE_STIR_UP_CHANCE_MAX;
	const int min = GLOBAL_DEFINE_STIR_UP_CHANCE_MIN;

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
	// WTP, ray, fix for e.g. Native Mercenaries or Rangers not being able to speak to Chiefs - START
	/*ProfessionTypes eProfession = getProfession();
	if (eProfession == NO_PROFESSION)
	{
		return false;
	}*/
	// The only Units that still cannot speak to Chiefs are Ships and Land Transports or Treasures or Animals (from e.g. Event)
	if (cargoSpace() > 0 || getUnitInfo().isTreasure() || getUnitInfo().isAnimal() || isGoods())
	{
		return false;
	}
	// WTP, ray, fix for e.g. Native Mercenaries or Rangers not being able to speak to Chiefs - END

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

	OOS_LOG("CvUnit::speakWithChief goody type", getTypeStr(eGoody));

	if (isHuman() && !GET_PLAYER(eNativePlayer).isHuman())
	{
		// R&R, ray, Natives do not talk when furious - START
		int currentPlayerAttitude = GET_PLAYER(eNativePlayer).AI_getAttitude(getOwnerINLINE(), false);
		if (currentPlayerAttitude == 0)
		{
			CvWString szBuffer = gDLL->getText("TXT_KEY_NATIVES_NOT_WILLING_TO_TALK", pCity->getNameKey());
			gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, GC.getCommandInfo(COMMAND_SPEAK_WITH_CHIEF).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE(), true, true);
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
	//WTP, ray Negative Promotions - START
	//we allow to heal also if Health is perfect if a negative Promotion exists
	//if (!isHurt())
	if (!isHurt() && !hasNegativePromotion())
	//WTP, ray Negative Promotions - END
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

	// WTP, ray, Plot Heal Modifier for Improvements - START
	if (pPlot->getImprovementType() != NO_IMPROVEMENT)
	{
		ImprovementTypes eImprovement = pPlot->getImprovementType();
		if (GC.getImprovementInfo(eImprovement).getHealModifier() > 0)
		{
			iTotalHeal += GC.getImprovementInfo(eImprovement).getHealModifier();
		}
	}
	// WTP, ray, Plot Heal Modifier for Improvements - END

	if (pPlot->isCity(true, getTeam()))
	{
		iTotalHeal += (GLOBAL_DEFINE_CITY_HEAL_RATE * (100 + (GET_TEAM(getTeam()).isFriendlyTerritory(pPlot->getTeam()) ? getExtraFriendlyHeal() : getExtraNeutralHeal()))) / 100;
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
				iTotalHeal += (GLOBAL_DEFINE_ENEMY_HEAL_RATE * (100 + getExtraEnemyHeal())) / 100;
			}
			else
			{
				iTotalHeal += (GLOBAL_DEFINE_NEUTRAL_HEAL_RATE * (100 + getExtraNeutralHeal())) / 100;;
			}
		}
		else
		{
			iTotalHeal += (GLOBAL_DEFINE_FRIENDLY_HEAL_RATE * (100 + getExtraFriendlyHeal())) / 100;;
		}
	}

	// XXX optimize this (save it?)
	iBestHeal = 0;

	pUnitNode = pPlot->headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = pPlot->getUnitNodeLoop(pUnitNode);

		if (pLoopUnit != NULL && pLoopUnit->getTeam() == getTeam()) // XXX what about alliances?
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
					pLoopUnit = pLoopPlot->getUnitNodeLoop(pUnitNode);

					if (pLoopUnit != NULL && pLoopUnit->getTeam() == getTeam()) // XXX what about alliances?
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
	// WTP, ray, issue #892 - bombard should set movement points to 0 - START
	// changeMoves(GLOBAL_DEFINE_MOVE_DENOMINATOR);
	setMoves(maxMoves());
	// WTP, ray, issue #892 - bombard should set movement points to 0 - END

	// Super Forts begin *bombard* *text*
	//R&R mod, vetiarvind, super forts merge ..invert if clause and let R&R code be the case when pBombardCity != null
	if(pBombardCity == NULL)
	{
		pTargetPlot->changeDefenseDamage(bombardRate());

		CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_DEFENSES_IN_CITY_REDUCED_TO", GC.getImprovementInfo(pTargetPlot->getImprovementType()).getText(),
			(GC.getImprovementInfo(pTargetPlot->getImprovementType()).getDefenseModifier()-pTargetPlot->getDefenseDamage()), GET_PLAYER(getOwnerINLINE()).getNameKey());
		gDLL->UI().addPlayerMessage(pTargetPlot->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", MESSAGE_TYPE_INFO, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pTargetPlot->getX_INLINE(), pTargetPlot->getY_INLINE(), true, true);

		szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_REDUCE_CITY_DEFENSES", getNameKey(), GC.getImprovementInfo(pTargetPlot->getImprovementType()).getText(),
			(GC.getImprovementInfo(pTargetPlot->getImprovementType()).getDefenseModifier()-pTargetPlot->getDefenseDamage()));
		gDLL->UI().addPlayerMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARD", MESSAGE_TYPE_INFO, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pTargetPlot->getX_INLINE(), pTargetPlot->getY_INLINE());
	}
	else
	{
		pBombardCity->changeDefenseModifier(-(bombardRate() * std::max(0, 100 - pBombardCity->getBuildingBombardDefense())) / 100);

		CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_DEFENSES_IN_CITY_REDUCED_TO", pBombardCity->getNameKey(), pBombardCity->getDefenseModifier(), GET_PLAYER(getOwnerINLINE()).getNameKey());
		gDLL->UI().addPlayerMessage(pBombardCity->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED", MESSAGE_TYPE_INFO, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pBombardCity->getX_INLINE(), pBombardCity->getY_INLINE(), true, true);

		szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_REDUCE_CITY_DEFENSES", getNameOrProfessionKey(), pBombardCity->getNameKey(), pBombardCity->getDefenseModifier());
		gDLL->UI().addPlayerMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARD", MESSAGE_TYPE_INFO, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pBombardCity->getX_INLINE(), pBombardCity->getY_INLINE());

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
	// WTP, ray, fix for pillaging Animals - START
	if (getUnitInfo().isAnimal())
	{
		return false;
	}
	// WTP, ray, fix for pillaging Animals - END

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
			long lPillageGold = -1; // K-Mod
			int iPillageGold = 0;

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
				OOS_LOG("CvUnit::pillage gold", iPillageGold);
				GET_PLAYER(getOwnerINLINE()).changeGold(iPillageGold);

				szBuffer = gDLL->getText("TXT_KEY_MISC_PLUNDERED_GOLD_FROM_IMP", iPillageGold, GC.getImprovementInfo(pPlot->getImprovementType()).getTextKeyWide());
				gDLL->UI().addPlayerMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGE", MESSAGE_TYPE_INFO, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pPlot->getX_INLINE(), pPlot->getY_INLINE());

				if (pPlot->isOwned())
				{
					szBuffer = gDLL->getText("TXT_KEY_MISC_IMP_DESTROYED", GC.getImprovementInfo(pPlot->getImprovementType()).getTextKeyWide(), getNameOrProfessionKey(), getVisualCivAdjective(pPlot->getTeam()));
					gDLL->UI().addPlayerMessage(pPlot->getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGED", MESSAGE_TYPE_INFO, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pPlot->getX_INLINE(), pPlot->getY_INLINE(), true, true);
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

	changeMoves(GLOBAL_DEFINE_MOVE_DENOMINATOR);

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

		if (!(GET_PLAYER(getOwnerINLINE()).canFound(pPlot->coord(), bTestVisible)))
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
		int iFoodDifference = plot()->calculateNatureYield(YIELD_FOOD, getTeam(), true) - GLOBAL_DEFINE_FOOD_CONSUMPTION_PER_POPULATION;
		bool bInland = !plot()->isCoastalLand(GLOBAL_DEFINE_MIN_WATER_SIZE_FOR_OCEAN);

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
		for (int i = 0; i < NUM_CITY_PLOTS_1_PLOT; ++i) // R&R, ray, 2 Plot Radius
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
		for (int i = 0; i < NUM_CITY_PLOTS_1_PLOT; ++i) // R&R, ray, 2 Plot Radius
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
			m_iMoneyToBuyLand = iCost;
			m_ePlayerToBuyLand = eNativeOwner;
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
	if (m_ePlayerToBuyLand != NO_PLAYER)
	{
		//resetting to Peace if Acquiring City has caused war
		if (GET_TEAM(GET_PLAYER(m_ePlayerToBuyLand).getTeam()).isAtWar(getTeam()))
		{
			GET_TEAM(GET_PLAYER(m_ePlayerToBuyLand).getTeam()).makePeace(getTeam());
			CvWString szBuffer = gDLL->getText("TXT_KEY_MAKE_PEACE_AFTER_ACQUIRE_CITY", GET_PLAYER(m_ePlayerToBuyLand).getNameKey());
			gDLL->UI().addPlayerMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getEraInfo(GC.getGameINLINE().getCurrentEra()).getAudioUnitDefeatScript(), MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), NULL, NULL);
		}

		OOS_LOG("CvUnit::buyLandAfterAcquire", m_iMoneyToBuyLand);
		GET_PLAYER(m_ePlayerToBuyLand).changeGold((m_iMoneyToBuyLand * GLOBAL_DEFINE_BUY_PLOT_SELLER_INCOME_PERCENT) / 100);
		GET_PLAYER(getOwnerINLINE()).AI_changeGoldTradedTo(m_ePlayerToBuyLand, m_iMoneyToBuyLand);
		GET_PLAYER(getOwnerINLINE()).changeGold(m_iMoneyToBuyLand * -1);
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
		for (int i = 0; i < NUM_CITY_PLOTS_1_PLOT; ++i) // R&R, ray, 2 Plot Radius
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

	CvPlot* pCityPlot = m_coord.plot();
	FAssert(NULL != pCityPlot);
	if (pCityPlot != NULL)
	{
		if (pCityPlot->isActiveVisible(false))
		{
			NotifyEntity(MISSION_FOUND);

			CvDLLEngineIFaceBase::TriggerEffectArguments arguments;
			arguments.effect = EFFECT_SETTLERSMOKE;
			arguments.pt3Point = pCityPlot->getPoint();
			arguments.rotation = GC.getASyncRand().get(360);
			arguments.audio = "AS3D_UN_FOUND_CITY";
			gDLL->getEngineIFace()->TriggerEffect(arguments);
		}

		CvCity* pCity = GET_PLAYER(getOwnerINLINE()).found(coord());

		FAssert(NULL != pCity);
		if (NULL != pCity)
		{
			pCity->addPopulationUnit(this, NO_PROFESSION);

			//WTP, ray, Settler Professsion - START
			//consuming YIELDS when Founding a City
			for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
			{
				pCity->setYieldStored(eYield, 0);
			}
			//WTP, ray, Settler Professsion - END
		}
	}

	// TAC - AI City Defense - koma13 - START
	CvPlayer &kPlayer = GET_PLAYER(getOwnerINLINE());
	PlayerTypes eParent = kPlayer.getParent();

	if (!isHuman() && eParent != NO_PLAYER)
	{
		// R&R, ray, changed for atmospherical reasons for AI to not always have cannons - START

		int iFreeUnitRand = GC.getGameINLINE().getSorenRandNum(10, "Pick Free Unit");
		UnitTypes eFreeDefender = NO_UNIT;

		// Militia
		if (iFreeUnitRand <= 3)
		{
			eFreeDefender = kPlayer.getUnitType(UNITCLASS_MILITIA);
		}

		// Veteran
		else if (iFreeUnitRand <= 7)
		{
			eFreeDefender = kPlayer.getUnitType(UNITCLASS_VETERAN);
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
		if (pCity->getRawYieldProduced(YIELD_FOOD) < pCity->getPopulation() * GLOBAL_DEFINE_FOOD_CONSUMPTION_PER_POPULATION)
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
			// ray, new Movement Calculation - START
			// if (movesLeft() == 0)
			if (movesLeft() <= 0)
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
#if 0
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
#endif
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
	if (kCity.AI_getFoodGatherable(iNewPop, 0) >= iNewPop * GLOBAL_DEFINE_FOOD_CONSUMPTION_PER_POPULATION)
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
		//WTP, ray, Large Rivers - Start
		// allowing Pioneers to Build on Large Rivers
		if (pPlot->getTerrainType() != TERRAIN_LARGE_RIVERS)
		{
			return false;
		}
		//WTP, ray, Large Rivers - Start
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

	OOS_LOG("CvUnit::build", getTypeStr(eBuild));
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

		const CvPlayer& kPlayer = GET_PLAYER(getOwnerINLINE());

		// WTP, ray, Lieutenants and Captains - START
		bool bAlradyHasLeader = false;
		const UnitTypes eGeneralUnit = kPlayer.getUnitType(UNITCLASS_GREAT_GENERAL);
		const UnitTypes eAdmiralUnit = kPlayer.getUnitType(UNITCLASS_GREAT_ADMIRAL);
		const UnitTypes eLieutenantUnit = kPlayer.getUnitType(UNITCLASS_BRAVE_LIEUTENANT);
		const UnitTypes eCaptainUnit = kPlayer.getUnitType(UNITCLASS_CAPABLE_CAPTAIN);

		if (   isHasPromotion(GC.getUnitInfo(eGeneralUnit)   .getLeaderPromotion()) 
			|| isHasPromotion(GC.getUnitInfo(eAdmiralUnit)   .getLeaderPromotion())
			|| isHasPromotion(GC.getUnitInfo(eLieutenantUnit).getLeaderPromotion())
			|| isHasPromotion(GC.getUnitInfo(eCaptainUnit)   .getLeaderPromotion())
			)
		{
			bAlradyHasLeader = true;
		}

		if (bAlradyHasLeader == true)
		{
			return false;
		}
		// WTP, ray, Lieutenants and Captains - END

		if (!GC.getPromotionInfo(ePromotion).isLeader())
		{
			return false;
		}

		CvUnit* pWarlord = kPlayer.getUnit(iLeaderUnitId);
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

	CvSelectionGroup::path_finder.Reset(); // K-Mod. (This currently isn't important, because the AI doesn't use promotions mid-turn anyway.)

	if (IsSelected())
	{
		gDLL->getInterfaceIFace()->playGeneralSound(GC.getPromotionInfo(ePromotion).getSound());

		gDLL->getInterfaceIFace()->setDirty(UnitInfo_DIRTY_BIT, true);
		gDLL->getFAStarIFace()->ForceReset(&GC.getInterfacePathFinder()); // K-Mod.
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

	PromotionTypes eLeaderPromotion = m_pUnitInfo->getLeaderPromotion();

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
	const PromotionTypes kLeaderPromotion = kUnitInfo.getLeaderPromotion();

	if (NO_PROMOTION == kLeaderPromotion) //this unit is not a leader
		return 0;

	if (-1 == iUnitId)
	{
		CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
		while(pUnitNode != NULL)
		{
			CvUnit* pUnit = pPlot->getUnitNodeLoop(pUnitNode);

			// WTP, fixing Generals and Admirals to lead civilists or small tiny fishing boats - START
			if (pUnit != NULL && pUnit != this && pUnit->getOwnerINLINE() == getOwnerINLINE() &&
				((pUnit->getDomainType() == DOMAIN_LAND && pUnit->canAttack()) || (pUnit->getDomainType() == DOMAIN_SEA && pUnit->baseCombatStr() >= 20)))
			{
				if (pUnit->canPromote(kLeaderPromotion, getID()))
				{
					++iNumUnits;
				}
			}

			// WTP, ray, Lieutenants and Captains - END
		}
	}
	else
	{
		CvUnit* pUnit = GET_PLAYER(getOwnerINLINE()).getUnit(iUnitId);

		// WTP, fixing Generals and Admirals to lead civilists or small tiny fishing boats - START
		if (pUnit != NULL && pUnit != this &&
			((pUnit->getDomainType() == DOMAIN_LAND && pUnit->canAttack()) || (pUnit->getDomainType() == DOMAIN_SEA && pUnit->baseCombatStr() >= 20)))
		{
			if (pUnit->canPromote(kLeaderPromotion, getID()))
			{
				iNumUnits = 1;
			}
		}

		// WTP, ray, Lieutenants and Captains - END
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
			// Ramstormp, WtP, Generals and admirals only share experience with units in the army or the navy respectively - START
			CvUnit* pUnit = pPlot->getUnitNodeLoop(pUnitNode);
			// WTP, adjustment ray, small improvement, let us read the UnitClassTypes only once
			const UnitClassTypes eLeaderUnitClassType = getUnitClassType();
			// Navy Case with Great Admiral
			if (eLeaderUnitClassType == UNITCLASS_GREAT_ADMIRAL || eLeaderUnitClassType == UNITCLASS_CAPABLE_CAPTAIN)
			{
				// not really happy about the >= 20 being hardcoded but for now it prevents e.g. Fishing Boat, which is good
				if (pUnit != NULL && pUnit != this && pUnit->getOwnerINLINE() == getOwnerINLINE() && pUnit->canAcquirePromotionAny() && pUnit->getDomainType() == DOMAIN_SEA && pUnit->baseCombatStr() >= 20)
				{
					++iNumUnits;
				}
			}
			// Army Case with Great General
			else if (eLeaderUnitClassType == UNITCLASS_GREAT_GENERAL || eLeaderUnitClassType == UNITCLASS_BRAVE_LIEUTENANT)
			{
				// here we could easily switch to canAttack
				if (pUnit != NULL && pUnit != this && pUnit->getOwnerINLINE() == getOwnerINLINE() && pUnit->canAcquirePromotionAny() && pUnit->getDomainType() == DOMAIN_LAND && pUnit->canAttack())
				{
					++iNumUnits;
				}
			}

			// WTP, ray we keep this for safety in case something ever changes
			// old default case if we ever have something else than a Great General Unitclass / Great Admiral Unitclass
			else
			{
				if (pUnit != NULL && pUnit != this && pUnit->getOwnerINLINE() == getOwnerINLINE() && pUnit->canAcquirePromotionAny())
				{
					++iNumUnits;
				}
			}
			// Ramstormp, WtP, Generals and admirals only share experience with units in the army or the navy respectively - END
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
				CvUnit* pUnit = pPlot->getUnitNodeLoop(pUnitNode);
				// Ramstormp, WtP, Generals and admirals only share experience with units in the army or the navy respectively - START
				// Navy Case with Great Admiral

				// WTP, adjustment ray, small improvement, let us read the UnitClassTypes only once
				const UnitClassTypes eLeaderUnitClassType = getUnitClassType();
				if (eLeaderUnitClassType == UNITCLASS_GREAT_ADMIRAL || eLeaderUnitClassType == UNITCLASS_CAPABLE_CAPTAIN)
				{
					// not really happy about the >= 20 being hardcoded but for now it prevents e.g. Fishing Boat, which is good
					if (pUnit != NULL && pUnit != this && pUnit->getOwnerINLINE() == getOwnerINLINE() && pUnit->canAcquirePromotionAny() && pUnit->getDomainType() == DOMAIN_SEA && pUnit->baseCombatStr() >= 20)
					{
						pUnit->changeExperience(i < iRemainder ? iMinExperiencePerUnit + 1 : iMinExperiencePerUnit);
						pUnit->testPromotionReady();
					}
				}
				// Army Case with Great General
				else if (eLeaderUnitClassType == UNITCLASS_GREAT_GENERAL || eLeaderUnitClassType == UNITCLASS_BRAVE_LIEUTENANT)
				{
					// not really happy about the > 2 being hardcoded but for now it prevents normal settlers which is good
					if (pUnit != NULL && pUnit != this && pUnit->getOwnerINLINE() == getOwnerINLINE() && pUnit->canAcquirePromotionAny() && pUnit->getDomainType() == DOMAIN_LAND && pUnit->canAttack())
					{
						pUnit->changeExperience(i < iRemainder ? iMinExperiencePerUnit + 1 : iMinExperiencePerUnit);
						pUnit->testPromotionReady();
					}
				}

				// WTP, ray we keep this for safety in case something ever changes
				// old default case if we ever have something else than a Great General Unitclass / Great Admiral Unitclass
				else
				{
					if (pUnit != NULL && pUnit != this && pUnit->getOwnerINLINE() == getOwnerINLINE() && pUnit->canAcquirePromotionAny())
					{
						pUnit->changeExperience(i < iRemainder ? iMinExperiencePerUnit+1 : iMinExperiencePerUnit);
						pUnit->testPromotionReady();
					}
				}

				// Ramstormp, WtP, Generals and admirals only share experience with units in the army or the navy respectively - END
				i++;
			}

			return true;
		}
	}

	return false;
}

int CvUnit::getStackExperienceToGive(int iNumUnits) const
{
	return (m_pUnitInfo->getLeaderExperience() * (100 + std::min(50, (iNumUnits - 1) * GLOBAL_DEFINE_WARLORD_EXTRA_EXPERIENCE_PER_UNIT_PERCENT))) / 100;
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

	iPrice = GLOBAL_DEFINE_BASE_UNIT_UPGRADE_COST;

	iPrice += (std::max(0, (GET_PLAYER(getOwnerINLINE()).getYieldProductionNeeded(eUnit, YIELD_HAMMERS) - GET_PLAYER(getOwnerINLINE()).getYieldProductionNeeded(getUnitType(), YIELD_HAMMERS))) * GLOBAL_DEFINE_UNIT_UPGRADE_COST_PER_PRODUCTION);

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

	if (!upgradeAvailable(getUnitType(), kUnitInfo.getUnitClassType()))
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
		CvUnit* pLoopUnit = plot()->getUnitNodeLoop(pUnitNode);

		if (pLoopUnit != NULL && pLoopUnit->getTransportUnit() == this)
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
		CvCity* pClosestCity = GC.getMap().findCity(getX_INLINE(), getY_INLINE(), NO_PLAYER, getTeam(), true, bCoastalOnly);
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

	OOS_LOG("CvUnit::upgrade", getTypeStr(eUnit));
	pUpgradeUnit = GET_PLAYER(getOwnerINLINE()).initUnit(eUnit, getProfession(), getX_INLINE(), getY_INLINE(), AI_getUnitAIType());

	FAssertMsg(pUpgradeUnit != NULL, "UpgradeUnit is not assigned a valid value");

	pUpgradeUnit->joinGroup(getGroup());

	pUpgradeUnit->convert(this, true);

	pUpgradeUnit->finishMoves();

	if (pUpgradeUnit->getLeaderUnitType() == NO_UNIT)
	{
		if (pUpgradeUnit->getExperience() > GLOBAL_DEFINE_MAX_EXPERIENCE_AFTER_UPGRADE)
		{
			pUpgradeUnit->setExperience(GLOBAL_DEFINE_MAX_EXPERIENCE_AFTER_UPGRADE);
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
	// completely rewritten function

	FAssert(m_iVisibilityRange == visibilityRangeUncached())
	int iRange = m_iVisibilityRange;

	const CvPlot* pPlot = plot();
	if (pPlot != NULL)
	{
		iRange += pPlot->getUnitVisibilityBonus();
	}

	return iRange;

}

int CvUnit::visibilityRangeUncached() const
{
	int iRange = GC.getUNIT_VISIBILITY_RANGE();

	const UnitCombatTypes eCombat = getUnitCombatType();

	if (eCombat != NO_UNITCOMBAT)
	{
		for (PromotionTypes ePromotion = FIRST_PROMOTION; ePromotion < NUM_PROMOTION_TYPES; ++ePromotion)
		{
			if (isHasPromotion(ePromotion))
			{
				iRange += GC.getPromotionInfo(ePromotion).getVisibilityChange();
			}
		}
	}

	return iRange;
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
	return (baseMoves() * GLOBAL_DEFINE_MOVE_DENOMINATOR);
}


int CvUnit::movesLeft() const
{
	if (GLOBAL_DEFINE_USE_CLASSIC_MOVEMENT_SYSTEM)
	{
		return std::max(0, (maxMoves() - getMoves()));
	}
	else
	{
		// ray, new Movement Calculation - START
		// this can get smaller than 0 now
		return (maxMoves() - getMoves());
	}
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
bool CvUnit::canBuildRoute(RouteTypes ePreferredRoute) const
{
	for (BuildTypes eBuild = FIRST_BUILD; eBuild < NUM_BUILD_TYPES; eBuild++)
	{
		RouteTypes eRoute = ((RouteTypes)(GC.getBuildInfo(eBuild).getRoute()));
		if (eRoute != NO_ROUTE)
		{
			if (ePreferredRoute == NO_ROUTE && getUnitInfo().getBuilds(eBuild))
			{
				// there is a route type that can be built
				return true;
			}
			if (eRoute == ePreferredRoute && getUnitInfo().getBuilds(eBuild))
			{
				// the reqeusted route type can be built
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
			return pGroup->getBestBuildRouteBuild(plot(), NO_ROUTE);
			// NO_ROUTE means: give the best route, no preference!

		case MISSION_ROUTE_TO_ROAD:
			return pGroup->getBestBuildRouteBuild(plot(), ROUTE_ROAD);

		case MISSION_ROUTE_TO_COUNTRY_ROAD:
			return pGroup->getBestBuildRouteBuild(plot(), ROUTE_COUNTRY_ROAD);

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
			return pGroup->headMissionQueueNode()->m_data.eBuild;
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
	// WTP, ray, fixing strange behaviour of Buccanneers - START
	bool bExceptionForHiddenNationalityInTransport = false;
	if (m_pUnitInfo->isHiddenNationality() && getTransportUnit() != NULL)
	{
		bExceptionForHiddenNationalityInTransport = true;
	}
	// WTP, ray, fixing strange behaviour of Buccanneers - END

	// WTP, jooe - 2023-04-08: allow coexisting if a unit is going to be captured this turn - START
	if (getCapturingPlayer() != NO_PLAYER && GET_PLAYER(getCapturingPlayer()).getTeam() == eTeam)
	{
		return true;
	}
	// WTP, jooe - 2023-04-08: allow coexisting if a unit is going to be captured this turn - END

	if (!m_pUnitInfo->isInvisible() && !bExceptionForHiddenNationalityInTransport)
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
			for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
			{
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
			for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
			{
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
		iMaxValue = std::min(iMaxValue, GLOBAL_DEFINE_ANIMAL_MAX_XP_VALUE);
	}

	// WTP, XP cap to prevent farming XP from Runaways
	if (getUnitInfo().LbD_canEscape())
	{
		// WTP, ray, modified XP cap code of devolution in case of Runaways by additional check for Profession Combat Change
		// it can only be a Runaway if CombatChange of the Profession is 1 or maybe 0 if config changes
		ProfessionTypes eUnitProfession = getProfession();
		if (eUnitProfession != NO_PROFESSION && GC.getProfessionInfo(eUnitProfession).getCombatChange() <= 1)
		{
			iMaxValue = std::min(iMaxValue, GLOBAL_DEFINE_ESCAPE_MAX_XP_VALUE);
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
					//WTP, ray, Large Rivers - START
					// Correcting that Ships eveade to Cities without Ocean access
					// if (pLoopCity->plot()->isFriendlyCity(*this, false))
					if (pLoopCity->plot()->isFriendlyCity(*this, false) && pLoopCity->plot()->hasDeepWaterCoast())
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

	int iModifier = std::max(0, GET_TEAM(getTeam()).getRebelPercent() - GLOBAL_DEFINE_REBEL_PERCENT_FOR_REVOLUTION);

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
	// WTP, ray, Cannons to Professions - START
	int iBombardRateChangeProfession = 0;
	if (getProfession() != NO_PROFESSION)
	{
		iBombardRateChangeProfession = GC.getProfessionInfo(getProfession()).getBombardRateChangeProfession();
	}

	return (m_pUnitInfo->getBombardRate() + getExtraBombardRate() + iBombardRateChangeProfession);
	// WTP, ray, Cannons to Professions - START
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
		// WTP, ray, Construction Supplies - START
		//if (specialCargo() != eSpecialCargo)
		if (specialCargo() != eSpecialCargo && eDomainCargo != DOMAIN_IMMOBILE)
		{
			return 0;
		}
	}

	if (domainCargo() != NO_DOMAIN)
	{
		// WTP, ray, Construction Supplies - START
		//if (domainCargo() != eDomainCargo)
		if (domainCargo() != eDomainCargo && eDomainCargo != DOMAIN_IMMOBILE)
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
		pLoopUnit = pPlot->getUnitNodeLoop(pUnitNode);

		if (pLoopUnit != NULL && pLoopUnit->getTransportUnit() == this)
		{
			if (!pLoopUnit->isGoods())
			{
				return true;
			}
		}
	}

	return false;
}

bool CvUnit::isYield() const
{
	return (getUnitInfo().getSpecialUnitType() == SPECIALUNIT_YIELD_CARGO);
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
		pLoopUnit = pPlot->getUnitNodeLoop(pUnitNode);

		if (pLoopUnit != NULL && pLoopUnit->getTransportUnit() == this)
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
		CvUnit* pLoopUnit = pPlot->getUnitNodeLoop(pUnitNode);

		if (pLoopUnit != NULL && pLoopUnit->getTransportUnit() == this)
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
		pLoopUnit = pPlot->getUnitNodeLoop(pUnitNode);

		if (pLoopUnit != NULL && pLoopUnit->getTransportUnit() == this)
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

		if (pLoopUnit != NULL && pLoopUnit->getYield() == NO_YIELD)
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

	// Note: Adding MOVE_MAX_MOVES to prevent the assert for non-ai usage of the group pathfinder since
	// that shoud be safe here and is easier than adding another pf flag

	CvCity* pSource = ::getCity(pTradeRoute->getSourceCity());
	// TAC - Trade Routes Advisor - koma13 - START
	//if (pSource == NULL || !generatePath(pSource->plot(), 0, bReusePath))
	if (pSource == NULL || !generatePath(pSource->plot(), (isIgnoreDanger() ? MOVE_IGNORE_DANGER | MOVE_MAX_MOVES : MOVE_NO_ENEMY_TERRITORY | MOVE_MAX_MOVES), bReusePath))
	// TAC - Trade Routes Advisor - koma13 - END
	{
		return false;
	}

	CvCity* pDestination = ::getCity(pTradeRoute->getDestinationCity());
	// TAC - Trade Routes Advisor - koma13 - START
	//if (pDestination != NULL && !generatePath(pDestination->plot(), 0, bReusePath))
	if (pDestination != NULL && !generatePath(pDestination->plot(), (isIgnoreDanger() ? MOVE_IGNORE_DANGER | MOVE_MAX_MOVES : MOVE_NO_ENEMY_TERRITORY | MOVE_MAX_MOVES), bReusePath))
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
	return m_iLbDrounds;
}

void CvUnit::setLbDrounds(int newRounds)
{
	m_iLbDrounds = newRounds;
}

ProfessionTypes CvUnit::getLastLbDProfession() const
{
	return m_eLastProfession;
}

void CvUnit::setLastLbDProfession(ProfessionTypes eProfession)
{
	m_eLastProfession = eProfession;
}
// TAC - LbD - Ray - END

// WTP, ray, saving 1 more Profession for Fisher Issue - START
int CvUnit::getLbDroundsBefore() const
{
	return m_iLbDroundsBefore;
}

void CvUnit::setLbDroundsBefore(int newRounds)
{
	m_iLbDroundsBefore = newRounds;
}

ProfessionTypes CvUnit::getLastLbDProfessionBefore() const
{
	return m_eLastProfessionBefore;
}

void CvUnit::setLastLbDProfessionBefore(ProfessionTypes eProfession)
{
	m_eLastProfessionBefore = eProfession;
}
// WTP, ray, saving 1 more Profession for Fisher Issue - END



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
	return getX_INLINE();
}


int CvUnit::getY() const
{
	return getY_INLINE();
}


void CvUnit::setXY(int iX, int iY, bool bGroup, bool bUpdate, bool bShow, bool bCheckPlotVisible)
{
	Coordinates toCoord(iX, iY);
	jumpTo(toCoord, bGroup, bUpdate, bShow, bCheckPlotVisible);
}


void CvUnit::jumpTo(CvPlot *plot, bool bGroup, bool bUpdate, bool bShow, bool bCheckPlotVisible)
{
	jumpTo(plot->coord(), bGroup, bUpdate, bShow, bCheckPlotVisible);
}


void CvUnit::jumpTo(Coordinates toCoord, bool bGroup, bool bUpdate, bool bShow, bool bCheckPlotVisible)
{
	CLLNode<IDInfo>* pUnitNode;
	CvCity* pOldCity;
	CvCity* pNewCity;
	CvCity* pWorkingCity;
	CvUnit* pTransportUnit = NULL;
	CvUnit* pLoopUnit;
	CvPlot* pOldPlot;
	CvPlot* pNewPlot;
	CvPlot* pLoopPlot;
	ActivityTypes eOldActivityType;
	int iI;

	// OOS!! Temporary for Out-of-Sync madness debugging...
	if (GC.getLogging())
	{
		if (gDLL->getChtLvl() > 0)
		{
			char szOut[1024];
			sprintf(szOut, "Player %d Unit %d (%S's %S) moving from %d:%d to %d:%d\n", getOwnerINLINE(), getID(), GET_PLAYER(getOwnerINLINE()).getNameKey(), getName().GetCString(), getX_INLINE(), getY_INLINE(), toCoord.x(), toCoord.y());
			gDLL->messageControlLog(szOut);
		}
	}

	FAssert(!at(toCoord) || coord().isInvalidPlotCoord());
	FAssert(!isFighting());
	FAssert(toCoord.isInvalidPlotCoord() || (GC.getMap().plotINLINE(toCoord)->getX_INLINE() == toCoord.x()));
	FAssert(toCoord.isInvalidPlotCoord() || (GC.getMap().plotINLINE(toCoord)->getY_INLINE() == toCoord.y()));

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

	pNewPlot = GC.getMap().plotINLINE(toCoord);

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

		// Update city caches if required when leaving the city
		if (pOldCity != NULL && getSlaveWorkerProductionBonus() != 0)
		{
			pOldCity->updateSlaveWorkerProductionBonus();
		}

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
		m_coord = pNewPlot->coord();
	}
	else
	{
		m_coord.resetInvalid();
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
				GET_TEAM(getTeam()).AI_changeWarSuccess(pNewCity->getTeam(), GLOBAL_DEFINE_WAR_SUCCESS_CITY_CAPTURING);
				PlayerTypes eNewOwner = GET_PLAYER(getOwnerINLINE()).pickConqueredCityOwner(*pNewCity);

				if (NO_PLAYER != eNewOwner)
				{
					//ray18
					// buy Land from Natives after acquire city
					doAcquireCheckNatives();
					//Ende ray18
					GET_PLAYER(eNewOwner).acquireCity(pNewCity, true, false); // will delete the pointer
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
						gDLL->UI().addPlayerMessage(pNewPlot->getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CITYCAPTURED", MESSAGE_TYPE_MAJOR_EVENT, GC.getImprovementInfo(eImprovement).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pNewPlot->getX_INLINE(), pNewPlot->getY_INLINE(), true, true);
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

		if (pNewCity != NULL)
		{
			const int iSlaveWorkerProductionBonus = getSlaveWorkerProductionBonus();
			// Update city caches if required when entering a city
			if (iSlaveWorkerProductionBonus != 0)
			{
				pNewCity->updateSlaveWorkerProductionBonus(iSlaveWorkerProductionBonus);
			}
		}

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
				pLoopUnit = pOldPlot->getUnitNodeLoop(pUnitNode);

				if (pLoopUnit != NULL && pLoopUnit->getTransportUnit() == this)
				{
					pLoopUnit->jumpTo(toCoord, bGroup, bUpdate);
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


bool CvUnit::at(Coordinates testCoord) const
{
	return (testCoord == coord());
}


bool CvUnit::atPlot(const CvPlot* pPlot) const
{
	return (plot() == pPlot);
}


CvPlot* CvUnit::plot() const
{
	if(coord().isInvalidPlotCoord())
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
		return GC.getMap().plotSoren(coord());
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

//WTP, ray, Large Rivers - Nightinggale addition
// Requesting the area for the domain for the unit
// Does the same as vanilla except if plot is a large river, then land units
// will get the area of a land plot next to the plot the unit is on
// It will still return the water area if CvPlot fails to find a land plot
int CvUnit::getArea() const
{
	CvPlot* pPlot = plot();
	if (pPlot == NULL)
	{
		return FFreeList::INVALID_INDEX;
	}

	return pPlot->getArea(getDomainType());
}

// get the areaID of a plot in the 3x3 area with the unit in the center
int CvUnit::getLandArea() const
{
	if (getX_INLINE() != INVALID_PLOT_COORD && getY_INLINE() != INVALID_PLOT_COORD)
	{
		// scanning the 3x3 area
		// Use plotCity for order as this will start with the center plot
		// This means unless the unit is on water (like large river), the result will be the same as getArea()
		for (int i = 0; i < 9; ++i)
		{
			const CvPlot* pPlot = plotCity(getX_INLINE(), getY_INLINE(), i);
			if (pPlot != NULL && !pPlot->isWater())
			{
				return pPlot->getArea();
			}
		}
	}

	// failed to locate land. Rely on vanilla code for what to do now
	return getArea();
}

CvArea* CvUnit::area() const
{
	CvPlot* pPlot = plot();
	if (pPlot == NULL)
	{
		return NULL;
	}

	return pPlot->area(getDomainType());
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


void CvUnit::addDamageRandom(int iMinDamage, int iMaxDamage, int iMinHealthPercentageRemaining)
{
	const int iRemainingHitPointsPercentage = ( currHitPoints() * 100 / maxHitPoints() );
	if ( iRemainingHitPointsPercentage > iMinHealthPercentageRemaining )
	{
		const int iDamageRand = std::max(iMinDamage, GC.getGameINLINE().getSorenRandNum(iMaxDamage, "random damage"));
		int iDamageToApply = ( maxHitPoints() * iRemainingHitPointsPercentage * iDamageRand ) / ( 100 * 100 );
		if (((( currHitPoints() - iDamageToApply ) * maxHitPoints() ) / 100 ) <= iMinHealthPercentageRemaining )
		{
			setDamage (( maxHitPoints() * ( 100 - iMinHealthPercentageRemaining)) / 100 );
		} else {
			changeDamage( iDamageToApply );
		}
	}
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
	return GC.getMap().plotSoren(m_iAttackPlotX, m_iAttackPlotY);
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
	iNewValue = range(iNewValue, 0, GLOBAL_DEFINE_MAX_FORTIFY_TURNS);

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
	return m_iVisibilityRange - GC.getUNIT_VISIBILITY_RANGE();
}


void CvUnit::changeVisibilityRange(int iChange)
{
	if (iChange != 0)
	{
		updateVisibilityCache(m_iVisibilityRange + iChange);
	}
}


int CvUnit::getExtraMoves() const
{
	return m_iExtraMoves;
}


void CvUnit::changeExtraMoves(int iChange)
{
	m_iExtraMoves += iChange;
	FAssert(getExtraMoves() >= 0 || hasNegativePromotion());
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

	// WTP, jooe: disable this assert, because it triggers due to the new "broken cannons" promotion
	// FAssert(getExtraBombardRate() >= 0);
}


int CvUnit::getExtraEnemyHeal() const
{
	return m_iExtraEnemyHeal;
}


void CvUnit::changeExtraEnemyHeal(int iChange)
{
	m_iExtraEnemyHeal = (m_iExtraEnemyHeal + iChange);
	//WTP, ray, removed because of Negative Promotions
	//FAssert(getExtraEnemyHeal() >= 0);
}


int CvUnit::getExtraNeutralHeal() const
{
	return m_iExtraNeutralHeal;
}


void CvUnit::changeExtraNeutralHeal(int iChange)
{
	m_iExtraNeutralHeal = (m_iExtraNeutralHeal + iChange);
	//WTP, ray, removed because of Negative Promotions
	//FAssert(getExtraNeutralHeal() >= 0);
}


int CvUnit::getExtraFriendlyHeal() const
{
	return m_iExtraFriendlyHeal;
}


void CvUnit::changeExtraFriendlyHeal(int iChange)
{
	m_iExtraFriendlyHeal = (m_iExtraFriendlyHeal + iChange);
	//WTP, ray, removed because of Negative Promotions
	//FAssert(getExtraFriendlyHeal() >= 0);
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

//WTP, ray, Animal Promotions increase gold from Animals - START
int CvUnit::getAnimalGoldChange() const
{
	return m_iAnimalGoldChange;
}

void CvUnit::changeAnimalGoldChange(int iChange)
{
	if (iChange != 0)
	{
		m_iAnimalGoldChange += iChange;

		setInfoBarDirty(true);
	}
}
//WTP, ray, Animal Promotions increase gold from Animals - END

//WTP, ray, Slave Hunter and Slave Master - START
int CvUnit::getSlaveRevoltReductionBonus() const
{
	return m_iSlaveRevoltReductionBonus;
}

void CvUnit::changeSlaveRevoltReductionBonus(int iChange)
{
	if (iChange != 0)
	{
		m_iSlaveRevoltReductionBonus += iChange;

		setInfoBarDirty(true);
	}
}

int CvUnit::getSlaveWorkerProductionBonus() const
{
	return m_iSlaveWorkerProductionBonus;
}

void CvUnit::changeSlaveWorkerProductionBonus(int iChange)
{
	if (iChange != 0)
	{
		m_iSlaveWorkerProductionBonus += iChange;

		setInfoBarDirty(true);
	}
}
//WTP, ray, Slave Hunter and Slave Master - END

// WTP, ray, Lawkeeper Promotion - START
int CvUnit::getAdditionalLawToCityFromUnit() const
{
	return m_iAdditionalLawToCityUnit;
}

void CvUnit::changeAdditionalLawToCityFromUnit(int iChange)
{
	if (iChange != 0)
	{
		m_iAdditionalLawToCityUnit += iChange;

		setInfoBarDirty(true);
	}
}
// WTP, ray, Lawkeeper Promotion - END


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

// check if a unit can leave a colony
// note that it will return true if the unit isn't a citizen
bool CvUnit::canLeaveCity() const
{
	if (getProfession() != NO_PROFESSION && GC.getProfessionInfo(getProfession()).isCitizen())
	{
		CvCity* pCity = getCity();
		if (pCity != NULL && pCity->isOccupation())
		{
			return false;
		}
	}
	return true;
}

ProfessionTypes CvUnit::getProfession() const
{
	return m_eProfession;
}

bool CvUnit::setProfession(ProfessionTypes eProfession, bool bForce, bool bRemoveYieldsFromCity)
{
	if (!bForce && !canHaveProfession(eProfession, false))
	{
		FAssertMsg(false, "Unit can not have profession");
		return false;
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
					return true;
				}
			}
		}

		// clean up from old profession
		processProfession(getProfession(), -1, false, bRemoveYieldsFromCity);
		ProfessionTypes eOldProfession = getProfession();

		// actually change profession
		m_eProfession = eProfession;

		if (getProfessionUnitCombatType(eOldProfession) != getProfessionUnitCombatType(getProfession()))
		{
			// set cached data from promotions
			setPromotions();
		}
		processProfession(getProfession(), 1, true, bRemoveYieldsFromCity);

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

	return true;
}

bool CvUnit::canHaveProfession(ProfessionTypes eProfession, bool bBumpOther, const CvPlot* pPlot, bool bForceCheck) const
{
	if (NO_PROFESSION == eProfession)
	{
		return true;
	}

	if (!bForceCheck && eProfession == getProfession())
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
	if (eProfession == PROFESSION_WHALING_BOAT_WORKING)
	{
		if (!isWhalingBoat())
		{
			return false;
		}
	}
	//End TAC Whaling, ray

	// R&R, ray, High Sea Fishing - START
	if (eProfession == PROFESSION_FISHING_BOAT_WORKING)
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
		// occupation/unrest will cause units to vanish if they leave
		// better prevent citizens from leaving if that's the case
		if (!canLeaveCity() && !kNewProfession.isCitizen())
		{
			return false;
		}

		//make sure all equipment is available
		if (!pCity->AI_isWorkforceHack())
		{
			if ((kOwner.hasContentsYieldEquipmentAmountSecure(getProfession()) || kOwner.hasContentsYieldEquipmentAmount(eProfession)) && !gDLL->GetWorldBuilderMode()) // cache CvPlayer::getYieldEquipmentAmount - Nightinggale
			{
				for (YieldTypes eYieldType = FIRST_YIELD; eYieldType < NUM_YIELD_TYPES; ++eYieldType)
				{
					const int iYieldCarried  = kOwner.getYieldEquipmentAmountSecure(getProfession(), eYieldType); // cache CvPlayer::getYieldEquipmentAmount - Nightinggale
					const int iYieldRequired = kOwner.getYieldEquipmentAmount(eProfession, eYieldType);
					if (iYieldRequired > 0)
					{
						const int iMissing = iYieldRequired - iYieldCarried;

						if (iMissing > pCity->getYieldStored(eYieldType))
						{
							return false;
						}
					}
				}
			}

			if (!kNewProfession.isCitizen())
			{
				// ray, new Movement Calculation - START
				// if (movesLeft() == 0)
				if (movesLeft() <= 0)
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
			for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
			{
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
			if (!getUnitInfo().isGatherBoat() && !gDLL->GetWorldBuilderMode())
			{
				return false;
			}
			//End TAC Whaling, ray
		}
	}

	// WTP, ray, Barracks System, check if there is still enough Barracks Space - START
	// Comment: I am afraid to damage AI logc thus I will only check this for Human
	int iBarracksSpaceChangeByProfessionChange = kNewProfession.getBarracksSpaceNeededChange();
	if (GLOBAL_DEFINE_ENABLE_NEW_BARRACKS_SYSTEM && pPlot != NULL && isHuman() && iBarracksSpaceChangeByProfessionChange > 0)
	{
		// Case real City
		if (pPlot->getImprovementType() == NO_IMPROVEMENT)
		{
			// this is only checked for Colonial Cities, Native Villages can always be entered
			CvCity* pCity = pPlot->getPlotCity();
			if (pCity != NULL)
			{
				// here we ensure we check this only for the Owner of the Unit being Owner of the City
				if(!pCity->isNative() && pCity->getOwnerINLINE() == getOwnerINLINE())
				{
					// this is how much the Unit without a Profession needs
					// but we do not need to calculate it because it will not change
					// int iBarracksSpaceNeededByUnit = getUnitInfo().getBarracksSpaceNeeded();

					int iCurrentBarracksSpaceNeededByProfession = 0; // default case for No Profession
					if (getProfession() != NO_PROFESSION)
					{
						iCurrentBarracksSpaceNeededByProfession = GC.getProfessionInfo(getProfession()).getBarracksSpaceNeededChange();
					}

					// So when changing the profession the new value will be added and the old will be substracted
					int iAdditionalBarracksSpaceNeededForProfession = iBarracksSpaceChangeByProfessionChange - iCurrentBarracksSpaceNeededByProfession;

					// Caclulating free Barracks Space in City
					int iBarracksSpaceMaxInCity = pPlot->getPlotCity()->getCityBarracksSpace();
					int iBarracksSpaceUsedInCity = pPlot->getPlotCity()->getCityBarracksSpaceUsed();
					int iBarracksSpaceAvailableInCity = iBarracksSpaceMaxInCity - iBarracksSpaceUsedInCity;

					if (iAdditionalBarracksSpaceNeededForProfession > iBarracksSpaceAvailableInCity)
					{
						return false;
					}
				}
			}
		}

		// Case "actAsCity" Improvement does not need to be considered - already caught otherwise
		// Profession changes like this are not allowed outside Cities
	}
	// WTP, ray, Barracks System, check if there is still enough Barracks Space - END


	return true;
}

void CvUnit::processProfession(ProfessionTypes eProfession, int iChange, bool bUpdateCity, bool bRemoveYieldsFromCity)
{
	CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());

	if (iChange != 0)
	{
		OOS_LOG_3("processProfession", getTypeStr(eProfession), iChange);
		processProfessionStats(eProfession, iChange);

		if (eProfession != NO_PROFESSION)
		{
			CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);

			kOwner.changeAssets(iChange * kProfession.getAssetValue());

			int iPower = iChange * kProfession.getPowerValue();

			if (GET_PLAYER(getOwnerINLINE()).hasContentsYieldEquipmentAmount(eProfession)) // cache CvPlayer::getYieldEquipmentAmount - Nightinggale
			{
				for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
				{
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

	if (bRemoveYieldsFromCity && pCity != NULL && pCity->getOwnerINLINE() == getOwnerINLINE())
	{
		if (iChange != 0 && !gDLL->GetWorldBuilderMode())
		{
			if (GET_PLAYER(getOwnerINLINE()).hasContentsYieldEquipmentAmountSecure(eProfession)) // cache CvPlayer::getYieldEquipmentAmount - Nightinggale
			{
				for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
				{
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
			const CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);
			processProfessionStatsUnsaved(kProfession, iChange);

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

void CvUnit::processProfessionStatsUnsaved(const CvProfessionInfo& kProfession, int iChange)
{
	FAssert(iChange != 0);

	const CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());
	ProfessionTypes eProfession = kProfession.getIndex();

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
	for (YieldTypes eYieldType = FIRST_YIELD; eYieldType < NUM_YIELD_TYPES; ++eYieldType)
	{
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

CvPlayer &CvUnit::getOwnerR() const
{
	return GET_PLAYER(getOwnerINLINE());
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
			if (getOwnerINLINE() != plot()->getOwnerINLINE())
			{
				return UNKNOWN_PLAYER;
			}

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
		return CIVILIZATION_BARBARIAN;
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


bool CvUnit::isCapturableLandUnit() const
{
	return	( isUnarmed() ||
						getUnitInfo().isTreasure() ||
						(getUnitInfo().getCargoSpace() > 0 && getUnitInfo().getDomainType() == DOMAIN_LAND) );
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
	return m_pUnitInfo->getUnitClassType();
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
		setPostCombatPlot(coord());
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
					jumpTo(pPlot);
					finishMoves();
				}
			}
			setPostCombatPlot(Coordinates::invalidCoord());
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
	return GC.getMap().plotByIndexINLINE(m_iPostCombatPlotIndex);
}

void CvUnit::setPostCombatPlot(Coordinates coord)
{
	m_iPostCombatPlotIndex = coord.isOnMap() ? coord.plotNum() : -1;
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
				jumpTo(pTransportUnit->coord());
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
	return m_ja_iExtraDomainModifier.get(eIndex);
}


void CvUnit::changeExtraDomainModifier(DomainTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_DOMAIN_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_ja_iExtraDomainModifier.add(iChange, eIndex);
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
	return m_ja_iTerrainDoubleMoveCount.get(eIndex);
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
	m_ja_iTerrainDoubleMoveCount.add(iChange, eIndex);
	FAssert(getTerrainDoubleMoveCount(eIndex) >= 0);
}


int CvUnit::getFeatureDoubleMoveCount(FeatureTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumFeatureInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_ja_iFeatureDoubleMoveCount.get(eIndex);
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
	m_ja_iFeatureDoubleMoveCount.add(iChange, eIndex);
	FAssert(getFeatureDoubleMoveCount(eIndex) >= 0);
}


int CvUnit::getExtraTerrainAttackPercent(TerrainTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_ja_iExtraTerrainAttackPercent.get(eIndex);
}


void CvUnit::changeExtraTerrainAttackPercent(TerrainTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_ja_iExtraTerrainAttackPercent.add(iChange, eIndex);

		setInfoBarDirty(true);
	}
}

int CvUnit::getExtraTerrainDefensePercent(TerrainTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_ja_iExtraTerrainDefensePercent.get(eIndex);
}


void CvUnit::changeExtraTerrainDefensePercent(TerrainTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_ja_iExtraTerrainDefensePercent.add(iChange, eIndex);

		setInfoBarDirty(true);
	}
}

int CvUnit::getExtraFeatureAttackPercent(FeatureTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumFeatureInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_ja_iExtraFeatureAttackPercent.get(eIndex);
}


void CvUnit::changeExtraFeatureAttackPercent(FeatureTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumFeatureInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_ja_iExtraFeatureAttackPercent.add(iChange, eIndex);

		setInfoBarDirty(true);
	}
}

int CvUnit::getExtraFeatureDefensePercent(FeatureTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumFeatureInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_ja_iExtraFeatureDefensePercent.get(eIndex);
}


void CvUnit::changeExtraFeatureDefensePercent(FeatureTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumFeatureInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_ja_iExtraFeatureDefensePercent.add(iChange, eIndex);

		setInfoBarDirty(true);
	}
}

int CvUnit::getExtraUnitClassAttackModifier(UnitClassTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_ja_iExtraUnitClassAttackModifier.get(eIndex);
}

void CvUnit::changeExtraUnitClassAttackModifier(UnitClassTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_ja_iExtraUnitClassAttackModifier.add(iChange, eIndex);
}

int CvUnit::getExtraUnitClassDefenseModifier(UnitClassTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_ja_iExtraUnitClassDefenseModifier.get(eIndex);
}

void CvUnit::changeExtraUnitClassDefenseModifier(UnitClassTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_ja_iExtraUnitClassDefenseModifier.add(iChange, eIndex);
}

int CvUnit::getExtraUnitCombatModifier(UnitCombatTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitCombatInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_ja_iExtraUnitCombatModifier.get(eIndex);
}

void CvUnit::changeExtraUnitCombatModifier(UnitCombatTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitCombatInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_ja_iExtraUnitCombatModifier.add(iChange,eIndex);
}

bool CvUnit::canAcquirePromotion(PromotionTypes ePromotion) const
{
	FAssertMsg(ePromotion >= 0, "ePromotion is expected to be non-negative (invalid Index)");
	FAssertMsg(ePromotion < GC.getNumPromotionInfos(), "ePromotion is expected to be within maximum bounds (invalid Index)");

	if (isHasPromotion(ePromotion))
	{
		return false;
	}

	//WTP, ray, Promotions not earned by XP
	// this section is actually written by Nightinggale, but kept ray in the previous line as it makes searching easier
	const CvPromotionInfo& kPromotion = GC.getPromotionInfo(ePromotion);

	if (kPromotion.isNotEarnedByXP())
	{
		return false;
	}
	//WTP, ray, Promotions not earned by XP

	if (kPromotion.getPrereqPromotion() != NO_PROMOTION)
	{
		if (!isHasPromotion((PromotionTypes)(kPromotion.getPrereqPromotion())))
		{
			return false;
		}
	}

	if (kPromotion.getPrereqOrPromotion1() != NO_PROMOTION)
	{
		if (!isHasPromotion((PromotionTypes)(kPromotion.getPrereqOrPromotion1())))
		{
			if ((kPromotion.getPrereqOrPromotion2() == NO_PROMOTION) || !isHasPromotion((PromotionTypes)(kPromotion.getPrereqOrPromotion2())))
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

//WTP, ray Negative Promotions - START
bool CvUnit::canAcquireNegativePromotion(PromotionTypes ePromotion) const
{
	FAssertMsg(ePromotion >= 0, "ePromotion is expected to be non-negative (invalid Index)");
	FAssertMsg(ePromotion < GC.getNumPromotionInfos(), "ePromotion is expected to be within maximum bounds (invalid Index)");

	if (isHasPromotion(ePromotion))
	{
		return false;
	}

	const CvPromotionInfo& kPromotion = GC.getPromotionInfo(ePromotion);

	// check that this is in fact a Negative Promotion
	if (!kPromotion.isNegativePromotion())
	{
		return false;
	}

	// Negatives currently have no prerequs but who kno
	if (kPromotion.getPrereqPromotion() != NO_PROMOTION)
	{
		if (!isHasPromotion((PromotionTypes)(kPromotion.getPrereqPromotion())))
		{
			return false;
		}
	}

	if (kPromotion.getPrereqOrPromotion1() != NO_PROMOTION)
	{
		if (!isHasPromotion((PromotionTypes)(kPromotion.getPrereqOrPromotion1())))
		{
			if ((kPromotion.getPrereqOrPromotion2() == NO_PROMOTION) || !isHasPromotion((PromotionTypes)(kPromotion.getPrereqOrPromotion2())))
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

void CvUnit::acquireAnyNegativePromotion()
{
	PromotionTypes eNegativePromotionToGive = NO_PROMOTION;
	int iBestNegativePromotionRandValue = 0;

	for (PromotionTypes ePromotion = FIRST_PROMOTION; ePromotion < NUM_PROMOTION_TYPES; ++ePromotion)
	{
		if (canAcquireNegativePromotion(ePromotion))
		{
			const int iNegativePromotionRand = GC.getGameINLINE().getSorenRandNum(100, "Negative Promotion Rand");
			if (iNegativePromotionRand > iBestNegativePromotionRandValue)
			{
				eNegativePromotionToGive = ePromotion;
				iBestNegativePromotionRandValue = iNegativePromotionRand;
			}
		}
	}

	// if we found a Negative Promotion, let us apply it
	if (eNegativePromotionToGive != NO_PROMOTION)
	{
		setHasRealPromotion(eNegativePromotionToGive, true);
		CvWString szBuffer = gDLL->getText("TXT_KEY_UNIT_HAS_ACQUIRED_NEGATIVE_PROMOTION", getUnitInfo().getTextKeyWide(), GC.getPromotionInfo(eNegativePromotionToGive).getTextKeyWide());
		gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, GC.getPromotionInfo(eNegativePromotionToGive).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX_INLINE(), getY_INLINE());
	}

	return;
}

void CvUnit::cleanseAllNegativePromotions()
{
	if (!m_embisPromotionApplied.hasContent())
	{
		// no need to loop promotions if the unit doesn't have any promotions at all
		return;
	}

	bool bNegativePromotionCleansed = false;

	for (PromotionTypes ePromotion = FIRST_PROMOTION; ePromotion < NUM_PROMOTION_TYPES; ++ePromotion)
	{
		if (isHasPromotion(ePromotion) && GC.getPromotionInfo(ePromotion).isNegativePromotion())
		{
			setHasRealPromotion(ePromotion, false);
			bNegativePromotionCleansed = true;
		}
	}

	// if we found a Negative Promotion, let us report removal
	if (bNegativePromotionCleansed)
	{
		CvWString szBuffer = gDLL->getText("TXT_KEY_UNIT_WAS_CLEASED_OF_NEGATIVE_PROMOTIONS", getUnitInfo().getTextKeyWide());
		gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, getUnitInfo().getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE());
	}

	return;
}

bool CvUnit::hasNegativePromotion() const
{
	bool bHasNegativePromotion = false;
	for (int iI = 0; iI < GC.getNumPromotionInfos(); ++iI)
	{
		if (GC.getPromotionInfo((PromotionTypes) iI).isNegativePromotion() && isHasPromotion((PromotionTypes) iI))
		{
			bHasNegativePromotion = true;
		}
	}

	return bHasNegativePromotion;
}

//WTP, ray Negative Promotions - END

bool CvUnit::isPromotionValid(PromotionTypes ePromotion) const
{
	CvPromotionInfo& kPromotion = GC.getPromotionInfo(ePromotion);

	if (kPromotion.isGraphicalOnly() && !kPromotion.isLeader())
	{
		return false;
	}

	if (isOnlyDefensive() && kPromotion.isNotAvailableForDefensiveUnit())
	{
		return false;
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

	if (kPromotion.getWithdrawalChange() + withdrawalProbability() > GLOBAL_DEFINE_MAX_WITHDRAWAL_PROBABILITY)
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

bool CvUnit::isHasRealPromotion(PromotionTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumPromotionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_embHasRealPromotion.get(eIndex);
}

void CvUnit::setHasRealPromotion(PromotionTypes eIndex, bool bValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumPromotionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (isHasRealPromotion(eIndex) != bValue)
	{
		m_embHasRealPromotion.set(eIndex, bValue);

		setPromotions(eIndex);

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

	changeVisibilityRange(GC.getPromotionInfo(ePromotion).getVisibilityChange() * iChange);
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
	changeAnimalGoldChange(GC.getPromotionInfo(ePromotion).getAnimalGoldChange() * iChange); //WTP, ray, Animal Promotions increase gold from Animals
	changeSlaveRevoltReductionBonus(GC.getPromotionInfo(ePromotion).getSlaveRevoltReductionBonus() * iChange); //WTP, ray, Slave Hunter and Slave Master
	changeSlaveWorkerProductionBonus(GC.getPromotionInfo(ePromotion).getSlaveWorkerProductionBonus() * iChange); //WTP, ray, Slave Hunter and Slave Master
	changeAdditionalLawToCityFromUnit(GC.getPromotionInfo(ePromotion).getAdditionalLawToCity() * iChange); // WTP, ray, Lawkeeper Promotion - START
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

void CvUnit::resetPromotions()
{
	if (getOwnerINLINE() != NO_PLAYER)
	{
		plot()->changeAdjacentSight(getTeam(), m_iVisibilityRange, false, this);
	}

	m_iBlitzCount = 0;
	m_iAmphibCount = 0;
	m_iRiverCount = 0;
	m_iEnemyRouteCount = 0;
	m_iAlwaysHealCount = 0;
	m_iHillsDoubleMoveCount = 0;

	updateVisibilityCache(GC.getUNIT_VISIBILITY_RANGE());
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

	m_iAnimalGoldChange = 0; //WTP, ray, Animal Promotions increase gold from Animals
	m_iSlaveRevoltReductionBonus = 0; //WTP, ray, Slave Hunter and Slave Master
	m_iSlaveWorkerProductionBonus = 0; //WTP, ray, Slave Hunter and Slave Master
	m_iAdditionalLawToCityUnit = 0; // WTP, ray, Lawkeeper Promotion - START

	m_iUpgradeDiscount = 0;
	m_iExperiencePercent = 0;
	m_iCargoCapacity = (NO_UNIT != m_eUnitType) ? m_pUnitInfo->getCargoSpace() : 0;

	m_ja_iExtraTerrainAttackPercent.reset();
	m_ja_iExtraTerrainDefensePercent.reset();
	m_ja_iTerrainDoubleMoveCount.reset();

	m_ja_iExtraFeatureAttackPercent.reset();
	m_ja_iExtraFeatureDefensePercent.reset();
	m_ja_iFeatureDoubleMoveCount.reset();

	m_ja_iExtraUnitClassAttackModifier.reset();
	m_ja_iExtraUnitClassDefenseModifier.reset();

	m_ja_iExtraUnitCombatModifier.reset();
	m_ja_iExtraDomainModifier.reset();

	if (getOwnerINLINE() != NO_PLAYER)
	{
		plot()->changeAdjacentSight(getTeam(), m_iVisibilityRange, true, this);
	}
}

void CvUnit::setPromotions(PromotionTypes ePromotion)
{
	FAssert(ePromotion < GC.getNumPromotionInfos());
	FAssert(getOwnerINLINE() != NO_PLAYER);

	UnitCombatTypes eUnitCombat = getUnitCombatType();

	int iOldVisibilityRange = m_iVisibilityRange;

	// Only update promotions if the unit can have some or there are already promotions applied
	// most calls will likely be from
	if (eUnitCombat != NO_UNITCOMBAT || m_embisPromotionApplied.hasContent())
	{

		CvPlayerAI &kOwner = GET_PLAYER(getOwnerINLINE());

		PromotionTypes eLoopPromotion = ePromotion != NO_PROMOTION ? ePromotion : m_embisPromotionApplied.FIRST;
		const PromotionTypes eLastPromotion = ePromotion != NO_PROMOTION ? ePromotion : m_embisPromotionApplied.LAST;

		ProfessionTypes eProfession = getProfession();

		bool bFoundAnyPromotions = false;

		for (; eLoopPromotion <= eLastPromotion; ++eLoopPromotion)
		{
			bool bHasPromotion = false;
			if (eUnitCombat != NO_UNITCOMBAT && kOwner.canUsePromotion(eLoopPromotion) && GC.getPromotionInfo(eLoopPromotion).getUnitCombat(eUnitCombat))
			{
				// The unit/profession combo can use the promotion in question. Now check if it's present
				if (isHasRealPromotion(eLoopPromotion))
				{
					bHasPromotion = true;
				}
				else if (getFreePromotionCount(eLoopPromotion) > 0)
				{
					bHasPromotion = true;
				}
				else if (kOwner.hasFreePromotion(eLoopPromotion))
				{
					bHasPromotion = true;
				}
				else if (eUnitCombat != NO_UNITCOMBAT && kOwner.hasFreePromotion(eLoopPromotion, eUnitCombat))
				{
					bHasPromotion = true;
				}
				else if (eProfession != NO_PROFESSION && kOwner.hasFreePromotion(eLoopPromotion, eProfession))
				{
					bHasPromotion = true;
				}
			}

			if (bHasPromotion)
			{
				bFoundAnyPromotions = true;
				if (!m_embisPromotionApplied.get(eLoopPromotion))
				{
					m_embisPromotionApplied.set(eLoopPromotion, true);
					processPromotion(eLoopPromotion, 1);
				}
			}
			else
			{
				if (m_embisPromotionApplied.get(eLoopPromotion))
				{
					m_embisPromotionApplied.set(eLoopPromotion, false);
					processPromotion(eLoopPromotion, -1);
				}
			}
		}

		if (!bFoundAnyPromotions)
		{
			// try to release the array
			// See top check with unit combat to see why it's good to release the array if possible. There is more to it than just memory usage.
			m_embisPromotionApplied.releaseMemoryIfUnused();
		}
	}

	if (iOldVisibilityRange != visibilityRange())
	{
		plot()->changeAdjacentSight(getTeam(), iOldVisibilityRange, false, this);
		plot()->changeAdjacentSight(getTeam(), visibilityRange(), true, this);
	}
}

void CvUnit::setFreePromotionCount(PromotionTypes eIndex, int iValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumPromotionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(iValue >= 0, "promotion value going negative");

	if (getFreePromotionCount(eIndex) != iValue)
	{
		m_ja_iFreePromotionCount.set(iValue, eIndex);

		setPromotions(eIndex);

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
	return m_ja_iFreePromotionCount.get(eIndex);
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

	iTheirOdds = ((GLOBAL_DEFINE_COMBAT_DIE_SIDES * iTheirStrength) / (iOurStrength + iTheirStrength));
	int iStrengthFactor = ((iOurFirepower + iTheirFirepower + 1) / 2);

	iOurDamage = std::max(1, ((GLOBAL_DEFINE_COMBAT_DAMAGE * (iTheirFirepower + iStrengthFactor)) / (iOurFirepower + iStrengthFactor)));
	iTheirDamage = std::max(1, ((GLOBAL_DEFINE_COMBAT_DAMAGE * (iOurFirepower + iStrengthFactor)) / (iTheirFirepower + iStrengthFactor)));
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

	const InfoArray<UnitClassTypes>& ReqUnits = kTrigger.getUnitsRequired();
	if (ReqUnits.getLength() > 0)
	{
		bool bFoundValid = false;
		for (int i = 0; i < ReqUnits.getLength(); ++i)
		{
			if (getUnitClassType() == ReqUnits.getUnitClass(i))
			{
				// WTP, ray, trying to fix wrong plot selection for Unit Events - START
				if (kTrigger.isUnitsOnPlot())
				{
					if(plot() == pPlot)
					{
						bFoundValid = true;
						break;
					}
				}
				// old code
				else
				{
					bFoundValid = true;
					break;
				}
				// WTP, ray, trying to fix wrong plot selection for Unit Events - START
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
		gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szText, "AS2D_UNITGIFTED", MESSAGE_TYPE_INFO, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"), getX_INLINE(), getY_INLINE(), true, true);
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

char const* CvUnit::getButton() const
{
	const CvArtInfoUnit* pArtInfo = getArtInfo(0);

	if (NULL != pArtInfo)
	{
		return pArtInfo->getButton();
	}

	return m_pUnitInfo->getButton();
}

char const* CvUnit::getFullLengthIcon() const
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
		OOS_LOG_3("set yield stored", getTypeStr(getUnitType()), iChange);
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
	for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
	{
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
					if (pLoopUnit != NULL && pLoopUnit->getTransportUnit() == this)
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
	for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexINLINE(iI);
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
		jumpTo(pBestPlot->coord());
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

	for (int iI = 0; iI < GC.getMap().numPlotsINLINE(); iI++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexINLINE(iI);
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


// Allow natives to raid weapons and horses from a defeated unit or city
// Let the unit change profession to a random profession, which is better from a military point of view than the current one
// Won't remove yield cost from any city
// Argument aYields should be the amount of yields available from unit or city
// Return value is true if anything was stolen
// If returning true, aYields contains the amount of yields stolen for the profession change
//   note that aYields can contain negative values on return
bool CvUnit::raidWeapons(std::vector<int>& aYields)
{
	const CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());
	const ProfessionTypes eCurrentProfession = getProfession();
	std::vector<ProfessionTypes> aProfessions;
	for (ProfessionTypes eProfession = FIRST_PROFESSION; eProfession < NUM_PROFESSION_TYPES; ++eProfession)
	{
		//ray15
		//if (canHaveProfession(eProfession, false))
		if (kOwner.isProfessionValid(eProfession, getUnitType()))
		{
			//ray15 here only the if statement modified
			if (eCurrentProfession == NO_PROFESSION || GC.getProfessionInfo(eProfession).getCombatChange() > GC.getProfessionInfo(eCurrentProfession).getCombatChange() || (GC.getProfessionInfo(eProfession).getCombatChange() >= GC.getProfessionInfo(eCurrentProfession).getCombatChange() && GC.getProfessionInfo(eProfession).getMovesChange() > GC.getProfessionInfo(eCurrentProfession).getMovesChange()) )
			{
				bool bCanHaveProfession = false;
				for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
				{
					int iYieldRequired = kOwner.getYieldEquipmentAmount(eProfession, eYield);
					if (iYieldRequired > 0)
					{
						bCanHaveProfession = true;
						if (eCurrentProfession != NO_PROFESSION)
						{
							iYieldRequired -= kOwner.getYieldEquipmentAmount(eCurrentProfession, eYield);
						}

						//ray Korrektur negativer Warenbestand
						//if (iYieldRequired > 0 && aYields[iYield] == 0)
						if (iYieldRequired > aYields[eYield])
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

	ProfessionTypes eNewProfession = aProfessions[GC.getGameINLINE().getSorenRandNum(aProfessions.size(), "Choose raid weapons")];
	// change to selected profession without paying for it
	// if a city has to lose yields, let the calling function take care of that
	setProfession(eNewProfession, true, false);

	// aYields was used to tell how much is available to be stolen
	// this info is no longer needed and instead it will be used to reply how much was actually stolen
	for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
	{
		aYields[eYield] = kOwner.getYieldEquipmentAmount(eNewProfession, eYield);
		if (eCurrentProfession != NO_PROFESSION)
		{
			aYields[eYield] -= kOwner.getYieldEquipmentAmount(eCurrentProfession, eYield);
		}
	}

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
	for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
	{
		aYields[eYield] = pCity->getYieldStored(eYield);
	}

	if (!raidWeapons(aYields))
	{
		return false;
	}

	for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
	{
		if (aYields[eYield] > 0)
		{
			pCity->changeYieldStored(eYield, -aYields[eYield]);

			CvWString szString = gDLL->getText("TXT_KEY_GOODS_RAIDED", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), pCity->getNameKey(), aYields[eYield], GC.getYieldInfo(eYield).getTextKeyWide());
			gDLL->UI().addPlayerMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, GC.getYieldInfo(eYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pCity->getX_INLINE(), pCity->getY_INLINE());
			gDLL->UI().addPlayerMessage(pCity->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, GC.getYieldInfo(eYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pCity->getX_INLINE(), pCity->getY_INLINE());
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
		for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
		{
			aYields[eYield] += kOwner.getYieldEquipmentAmount(pUnit->getProfession(), eYield);
		}
	}

	if (!raidWeapons(aYields))
	{
		return false;
	}

	for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
	{
		if (aYields[eYield] > 0)
		{
			CvWString szString = gDLL->getText("TXT_KEY_WEAPONS_CAPTURED", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), pUnit->getNameOrProfessionKey(), aYields[eYield], GC.getYieldInfo(eYield).getTextKeyWide());
			gDLL->UI().addPlayerMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_CITYRAZE", MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(eYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pUnit->getX_INLINE(), pUnit->getY_INLINE(), true, true);
			gDLL->UI().addPlayerMessage(pUnit->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_CITYRAZE", MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(eYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pUnit->getX_INLINE(), pUnit->getY_INLINE(), true, true);
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

	OOS_LOG("Raid gold A", iNumTotalGold);
	GET_PLAYER(getOwnerINLINE()).changeGold(iNumTotalGold);
	if (NO_PLAYER != eTargetPlayer)
	{
		OOS_LOG("Raid gold B", iNumTotalGold);
		GET_PLAYER(eTargetPlayer).changeGold(-iNumTotalGold);
	}

	GET_TEAM(getTeam()).AI_changeDamages(pCity->getTeam(), -iNumTotalGold);

	CvPlot* pPlot = pCity->plot();
	if (pPlot->isVisible(GC.getGame().getActiveTeam(), false))
	{
		CvDLLEngineIFaceBase::TriggerEffectArguments arguments;
		arguments.effect = GC.getEntityEventInfo(GC.getMissionInfo(MISSION_PILLAGE).getEntityEvent()).getEffectType();
		arguments.pt3Point = pPlot->getPoint();
		arguments.rotation = GC.getASyncRand().get(360);
		arguments.audio = "AS3D_UN_CITY_EXPLOSION";
		gDLL->getEngineIFace()->TriggerEffect(arguments);
	}

	CvWString szString = gDLL->getText("TXT_KEY_TREASURY_RAIDED", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), pCity->getNameKey(), iNumTotalGold);
	gDLL->UI().addPlayerMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_CITYRAID", MESSAGE_TYPE_MINOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("SCREEN_GOLD_PILE")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pCity->getX_INLINE(), pCity->getY_INLINE(), true, true);
	gDLL->UI().addPlayerMessage(pCity->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_CITYRAID", MESSAGE_TYPE_MINOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("SCREEN_GOLD_PILE")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pCity->getX_INLINE(), pCity->getY_INLINE(), true, true);

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
	// WTP, ray, refactored according to advice of Nightinggale
	for (BuildingTypes eBuilding = FIRST_BUILDING; eBuilding < NUM_BUILDING_TYPES; eBuilding++)
	{
		CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
		// R&R, ray fix for Buildings with workers destoryed
		// only buildings without workers possible will be destroyed
		if (pCity->isHasRealBuilding(eBuilding) && kBuilding.getMaxWorkers() == 0)
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
		CvDLLEngineIFaceBase::TriggerEffectArguments arguments;
		arguments.effect = GC.getEntityEventInfo(GC.getMissionInfo(MISSION_PILLAGE).getEntityEvent()).getEffectType();
		arguments.pt3Point = pPlot->getPoint();
		arguments.rotation = GC.getASyncRand().get(360);
		arguments.audio = "AS3D_UN_CITY_EXPLOSION";
		gDLL->getEngineIFace()->TriggerEffect(arguments);
	}

	CvWString szString = gDLL->getText("TXT_KEY_BUILDING_RAIDED", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), pCity->getNameKey(), GC.getBuildingInfo(eTargetBuilding).getTextKeyWide());
	gDLL->UI().addPlayerMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_CITYRAID", MESSAGE_TYPE_MINOR_EVENT, GC.getBuildingInfo(eTargetBuilding).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pCity->getX_INLINE(), pCity->getY_INLINE(), true, true);
	gDLL->UI().addPlayerMessage(pCity->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_CITYRAID", MESSAGE_TYPE_MINOR_EVENT, GC.getBuildingInfo(eTargetBuilding).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pCity->getX_INLINE(), pCity->getY_INLINE(), true, true);

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

		int iMaxRaidedProduction = GC.getNATIVE_PRODUCTION_RAID_MIN();
		iMaxRaidedProduction += + GC.getGameINLINE().getSorenRandNum(GC.getNATIVE_PRODUCTION_RAID_RANDOM(), "Raid random building production amount");

		const bool bOnlyPartialProductionRaided = iMaxRaidedProduction < pCity->getBuildingProduction(eTarget);

		if (bOnlyPartialProductionRaided)
		{
			pCity->changeBuildingProduction(eTarget, -iMaxRaidedProduction);
		}
		else
		{
			pCity->setBuildingProduction(eTarget, 0);
		}

		GET_TEAM(getTeam()).AI_changeDamages(pCity->getTeam(), -GC.getBuildingInfo(eTarget).getAssetValue());

		CvPlot* pPlot = pCity->plot();
		if (pPlot->isVisible(GC.getGame().getActiveTeam(), false))
		{
			CvDLLEngineIFaceBase::TriggerEffectArguments arguments;
			arguments.effect = GC.getEntityEventInfo(GC.getMissionInfo(MISSION_PILLAGE).getEntityEvent()).getEffectType();
			arguments.pt3Point = pPlot->getPoint();
			arguments.rotation = GC.getASyncRand().get(360);
			arguments.audio = "AS3D_UN_CITY_EXPLOSION";
			gDLL->getEngineIFace()->TriggerEffect(arguments);
		}

		CvWString szString = gDLL->getText(bOnlyPartialProductionRaided ? "TXT_KEY_PRODUCTION_RAIDED_PARTIAL" : "TXT_KEY_PRODUCTION_RAIDED", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), pCity->getNameKey(), GC.getBuildingInfo(eTarget).getTextKeyWide());
		gDLL->UI().addPlayerMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, pCity, "AS2D_CITYRAID", MESSAGE_TYPE_MINOR_EVENT, GC.getBuildingInfo(eTarget).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), true, true);
		gDLL->UI().addPlayerMessage(pCity->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, pCity, "AS2D_CITYRAID", MESSAGE_TYPE_MINOR_EVENT, GC.getBuildingInfo(eTarget).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), true, true);
	}
	else if (pCity->isProductionUnit())
	{
		UnitTypes eTarget = pCity->getProductionUnit();
		int iProductionTurnLeft = pCity->getProductionTurnsLeft(eTarget, 0);

		if (pCity->getUnitProduction(eTarget) == 0)
		{
			return false;
		}

		int iMaxRaidedProduction = GC.getNATIVE_PRODUCTION_RAID_MIN();
		iMaxRaidedProduction += + GC.getGameINLINE().getSorenRandNum(GC.getNATIVE_PRODUCTION_RAID_RANDOM(), "Raid random unit production amount");

		const bool bOnlyPartialProductionRaided = iMaxRaidedProduction < pCity->getUnitProduction(eTarget);

		if (bOnlyPartialProductionRaided)
		{
			pCity->changeUnitProduction(eTarget, -iMaxRaidedProduction);
		}
		else
		{
			pCity->setUnitProduction(eTarget, 0);
		}

		GET_TEAM(getTeam()).AI_changeDamages(pCity->getTeam(), -GC.getUnitInfo(eTarget).getAssetValue());

		CvPlot* pPlot = pCity->plot();
		if (pPlot->isVisible(GC.getGame().getActiveTeam(), false))
		{
			CvDLLEngineIFaceBase::TriggerEffectArguments arguments;
			arguments.effect = GC.getEntityEventInfo(GC.getMissionInfo(MISSION_PILLAGE).getEntityEvent()).getEffectType();
			arguments.pt3Point = pPlot->getPoint();
			arguments.rotation = GC.getASyncRand().get(360);
			arguments.audio = "AS3D_UN_CITY_EXPLOSION";
			gDLL->getEngineIFace()->TriggerEffect(arguments);
		}

		CvWString szString = gDLL->getText(bOnlyPartialProductionRaided ? "TXT_KEY_PRODUCTION_RAIDED_PARTIAL" : "TXT_KEY_PRODUCTION_RAIDED", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), pCity->getNameKey(), GC.getUnitInfo(eTarget).getTextKeyWide());
		gDLL->UI().addPlayerMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, pCity, "AS2D_CITYRAID", MESSAGE_TYPE_MINOR_EVENT, GC.getUnitInfo(eTarget).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), true, true);
		gDLL->UI().addPlayerMessage(pCity->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, pCity, "AS2D_CITYRAID", MESSAGE_TYPE_MINOR_EVENT, GC.getUnitInfo(eTarget).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), true, true);
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
		CvDLLEngineIFaceBase::TriggerEffectArguments arguments;
		arguments.effect = GC.getEntityEventInfo(GC.getMissionInfo(MISSION_PILLAGE).getEntityEvent()).getEffectType();
		arguments.pt3Point = pPlot->getPoint();
		arguments.rotation = GC.getASyncRand().get(360);
		arguments.audio = "AS3D_UN_CITY_EXPLOSION";
		gDLL->getEngineIFace()->TriggerEffect(arguments);
	}

	CvWString szString = gDLL->getText("TXT_KEY_SCALP_RAIDED", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), pCity->getNameKey(), GC.getUnitInfo(pTargetUnit->getUnitType()).getTextKeyWide());
	gDLL->UI().addPlayerMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_CITYRAID", MESSAGE_TYPE_MINOR_EVENT, pTargetUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pCity->getX_INLINE(), pCity->getY_INLINE(), true, true);
	gDLL->UI().addPlayerMessage(pCity->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_CITYRAID", MESSAGE_TYPE_MINOR_EVENT, pTargetUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pCity->getX_INLINE(), pCity->getY_INLINE(), true, true);

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
		CvUnit* pLoopUnit = pPlot->getUnitNodeLoop(pUnitNode);

		if (pLoopUnit != NULL && pLoopUnit->getDomainType() == DOMAIN_SEA)
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
		CvDLLEngineIFaceBase::TriggerEffectArguments arguments;
		arguments.effect = GC.getEntityEventInfo(GC.getMissionInfo(MISSION_PILLAGE).getEntityEvent()).getEffectType();
		arguments.pt3Point = pPlot->getPoint();
		arguments.rotation = GC.getASyncRand().get(360);
		arguments.audio = "AS3D_UN_CITY_EXPLOSION";
		gDLL->getEngineIFace()->TriggerEffect(arguments);
	}

	CvWString szString = gDLL->getText("TXT_KEY_HARBOR_RAIDED", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), pCity->getNameKey(), GC.getUnitInfo(pTargetUnit->getUnitType()).getTextKeyWide());
	gDLL->UI().addPlayerMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_CITYRAID", MESSAGE_TYPE_MINOR_EVENT, pTargetUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pCity->getX_INLINE(), pCity->getY_INLINE(), true, true);
	gDLL->UI().addPlayerMessage(pCity->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_CITYRAID", MESSAGE_TYPE_MINOR_EVENT, pTargetUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pCity->getX_INLINE(), pCity->getY_INLINE(), true, true);

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

				UnitTypes SlaveType = kPlayer.getUnitType(UNITCLASS_NATIVE_SLAVE);
				CvUnit* SlaveUnit = kPlayer.initUnit(SlaveType, GC.getUnitInfo(SlaveType).getDefaultProfession(), pCity->getX_INLINE(), pCity->getY_INLINE(), NO_UNITAI);

				CvWString szString = gDLL->getText("TXT_KEY_RAIDING_NATIVE_CAPTURED", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), pCity->getNameKey());
				gDLL->UI().addPlayerMessage(pCity->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, SlaveUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pCity->getX_INLINE(), pCity->getY_INLINE());
				bRaided = true;
			}
			else
			{
				CvWString szString = gDLL->getText("TXT_KEY_NOTHING_RAIDED", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), pCity->getNameKey());
				gDLL->UI().addPlayerMessage(pCity->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_REVOLTEND", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"));
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
		gDLL->UI().addPlayerMessage(pCity->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_REVOLTEND", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"));
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
	{
		for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
		{
			if (pCity->getYieldStored(eYield) > 0 && GC.getYieldInfo(eYield).isCargo())
			{
				aYields.push_back(eYield);
			}
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
		pHomeCity = GC.getMap().findCity(pCity->getX_INLINE(), pCity->getY_INLINE(), getOwnerINLINE());
	}
	if (pHomeCity != NULL)
	{
		pHomeCity->changeYieldStored(eYield, iYieldsStolen);
	}

	CvWString szString = gDLL->getText("TXT_KEY_GOODS_RAIDED", GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey(), pCity->getNameKey(), iYieldsStolen, GC.getYieldInfo(eYield).getTextKeyWide());
	gDLL->UI().addPlayerMessage(getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, GC.getYieldInfo(eYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pCity->getX_INLINE(), pCity->getY_INLINE());
	gDLL->UI().addPlayerMessage(pCity->getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_UNITCAPTURE", MESSAGE_TYPE_INFO, GC.getYieldInfo(eYield).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pCity->getX_INLINE(), pCity->getY_INLINE());
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

		for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
		{
			if (kUnit.canGatherYield(eYield))
			{
				aiYieldsLoaded.push_back(eYield);
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

				if (pLoopUnit == NULL)
				{
					continue;
				}

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
		pPlot = GC.getMap().plot(ePlot->getX_INLINE(), ePlot->getY_INLINE());
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
			CvCity* pNearestCity = GC.getMap().findCity(pPlot->getX_INLINE(), pPlot->getY_INLINE(), pPlot->getOwner(), pPlot->getTeam(), false, true);

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
				setProfession(PROFESSION_WHALING_BOAT_WORKING);
			}
		}
		else
		{
			if (eCurrentProfession == PROFESSION_WHALING_BOAT_WORKING)
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
				setProfession(PROFESSION_FISHING_BOAT_WORKING);
			}
		}
		else
		{
			if (eCurrentProfession == PROFESSION_FISHING_BOAT_WORKING)
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
			for (YieldTypes eYield = FIRST_YIELD; eYield < NUM_YIELD_TYPES; ++eYield)
			{
				if (eBonusYieldChanges[eYield] > 0)
				{
					UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getYieldInfo(eYield).getUnitClass());
					CvUnit* eGatheredUnit = GET_PLAYER(getOwnerINLINE()).initUnit(eUnit, NO_PROFESSION, getX_INLINE(), getY_INLINE(), NO_UNITAI, NO_DIRECTION, eBonusYieldChanges[eYield]);

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
			gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS3D_UN_OCEAN_END1", MESSAGE_TYPE_INFO, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE(), true, true);
		}
		else
		{
			CvWString szBuffer = gDLL->getText("TXT_KEY_FISHER_FULL", getNameKey());
			gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS3D_UN_OCEAN_END1", MESSAGE_TYPE_INFO, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE(), true, true);
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
	return (getUnitClassType() == UNITCLASS_WHALING_BOAT);
}
//End TAC Whaling, ray

// R&R, ray, High Sea Fishing - START
bool CvUnit::isFishingBoat() const
{
	return (getUnitClassType() == UNITCLASS_FISHING_BOAT);
}
// R&R, ray, High Sea Fishing - END

// R&R, ray, Natives Trading - START
void CvUnit::setYieldForNativeTrade(YieldTypes nativeTradeYield)
{
	m_eYieldForNativeTrade = nativeTradeYield;
}

void CvUnit::setAmountForNativeTrade(int nativeTradeAmount)
{
	m_iAmountForNativeTrade = nativeTradeAmount;
}

YieldTypes CvUnit::getYieldForNativeTrade() const
{
	return m_eYieldForNativeTrade;
}

int CvUnit::getAmountForNativeTrade() const
{
	return m_iAmountForNativeTrade;
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
		CvUnit* pLoopUnit = plot()->getUnitNodeLoop(pUnitNode);

		if (pLoopUnit != NULL && pLoopUnit->getTransportUnit() == this)
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
		pLoopUnit = plot()->getUnitNodeLoop(pUnitNode);

		// we only count our own treasuers and only the ones that are smaller than max gold amount
		if (pLoopUnit != NULL && pLoopUnit->getUnitInfo().isTreasure() && pLoopUnit->getOwner() == getOwner() && pLoopUnit->getYieldStored() < maxTreasureGold)
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
		pLoopUnit = plot()->getUnitNodeLoop(pUnitNode);

		// WTP, ray, small improvements
		// we only count gold of our own treasuers and those that are not yet at max
		// after that we can directly kill them, so no extra loop is needed
		if (pLoopUnit != NULL && pLoopUnit->getUnitInfo().isTreasure() && pLoopUnit->getOwner() == getOwner() && pLoopUnit->getYieldStored() < maxTreasureGold)
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

	const UnitClassTypes eUnitClass = UNITCLASS_TREASURE;
	if (eUnitClass == NO_UNITCLASS)
	{
		FAssertMsg(eUnitClass != NO_UNITCLASS, "Failed to find treasure unitclass while merging");
		return; //Something went wrong
	}
	const UnitTypes eUnit = GET_PLAYER(getOwnerINLINE()).getUnitType(eUnitClass);

	if (eUnit == NO_UNIT)
	{
		FAssertMsg(eUnit != NO_UNIT, "Failed to find treasure unit while merging");
		return;
	}

	FAssert(GC.getUnitInfo(eUnit).isTreasure());

	for (int treasures = 0; treasures < treasureCount_MaxAmount; treasures++)
	{
		OOS_LOG("Create treasure", maxTreasureGold);
		CvUnit* pTreasure = GET_PLAYER(getOwnerINLINE()).initUnit(eUnit, GC.getUnitInfo(eUnit).getDefaultProfession(), plot()->getX_INLINE(), plot()->getY_INLINE(), NO_UNITAI, NO_DIRECTION, maxTreasureGold);
		// set Movement Points to 0, to prevent cheating
		pTreasure->setMoves(maxMoves());
	}
	if (restAmount > 0)
	{
		OOS_LOG("Create treasure", restAmount);
		CvUnit* pTreasure = GET_PLAYER(getOwnerINLINE()).initUnit(eUnit, GC.getUnitInfo(eUnit).getDefaultProfession(), plot()->getX_INLINE(), plot()->getY_INLINE(), NO_UNITAI, NO_DIRECTION, restAmount);
		// set Movement Points to 0, to prevent cheating
		pTreasure->setMoves(maxMoves());
	}
}
// WTP, merge Treasures, of Raubwuerger - END

// WTP, ray, Construction Supplies - START
bool CvUnit::canUseProductionSupplies() const
{
	// WTP, ray, small improvements
	// merge only available for treasures
	if (getUnitInfo().getProductionWhenUsed() <= 0)
	{
		return false;
	}

	// only in Cities
	if (plot()->isCity() == false)
	{
		return false;
	}

	// only for our own Cities
	if (plot()->getPlotCity()->getOwnerINLINE() != getOwnerINLINE())
	{
		return false;
	}

	return true;
}

void CvUnit::useProductionSupplies()
{
	int iProductionSuppliesToBeUsed = getUnitInfo().getProductionWhenUsed();
	CvCity* pCity = plot()->getPlotCity();

	// just for safety - should never happen due to "canUse.." check
	if (pCity == NULL)
	{
		return;
	}

	iProductionSuppliesToBeUsed *= GLOBAL_DEFINE_UNIT_PRODUCTION_PERCENT;
	iProductionSuppliesToBeUsed /= 100;

	iProductionSuppliesToBeUsed *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
	iProductionSuppliesToBeUsed /= 100;

	iProductionSuppliesToBeUsed *= GC.getEraInfo(GC.getGameINLINE().getStartEra()).getTrainPercent();
	iProductionSuppliesToBeUsed /= 100;

	if (!isHuman() && !isNative())
	{
		iProductionSuppliesToBeUsed *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAITrainPercent();
		iProductionSuppliesToBeUsed /= 100;

		iProductionSuppliesToBeUsed *= std::max(0, ((GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIPerEraModifier() * GET_PLAYER(getOwnerINLINE()).getCurrentEra()) + 100));
		iProductionSuppliesToBeUsed /= 100;
	}

	gDLL->UI().addPlayerMessage(getOwnerINLINE(), false, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_CITY_CONSTRUCTION_SUPPLIES_USED", pCity->getNameKey()), "AS2D_POSITIVE_DINK", MESSAGE_TYPE_INFO, getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX_INLINE(), getY_INLINE(), true, true);
	pCity->changeProduction(iProductionSuppliesToBeUsed);
	kill(true);

	return;
}
// WTP, ray, Construction Supplies - END

// WTP, ray, helper methods for Python Event System - Spawning Units and Barbarians on Plots - START
void CvUnit::spawnOwnPlayerUnitOnPlotOfUnit(int /*UnitClassTypes*/ iIndex) const
{
	CvPlayer& ownPlayer = GET_PLAYER(getOwnerINLINE());
	UnitTypes eUnitToSpawn = (UnitTypes)GC.getCivilizationInfo(ownPlayer.getCivilizationType()).getCivilizationUnits(iIndex);
	if (eUnitToSpawn != NO_UNIT)
	{
		OOS_LOG("CvUnit::spawnOwnPlayerUnitOnPlotOfUnit", getTypeStr(eUnitToSpawn));
		CvUnit* eOwnUnitToSpawn = ownPlayer.initUnit(eUnitToSpawn, GC.getUnitInfo(eUnitToSpawn).getDefaultProfession(), getX_INLINE(), getY_INLINE(), NO_UNITAI);
	}
	return;
}

void CvUnit::spawnBarbarianUnitOnPlotOfUnit(int /*UnitClassTypes*/ iIndex) const
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
		CvUnit* eBarbarianUnitToSpawn = barbarianPlayer.initUnit(eUnitToSpawn, GC.getUnitInfo(eUnitToSpawn).getDefaultProfession(), getX_INLINE(), getY_INLINE(), NO_UNITAI);
	}
	return;
}

void CvUnit::spawnOwnPlayerUnitOnAdjacentPlotOfUnit(int /*UnitClassTypes*/ iIndex) const
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
		CvUnit* eOwnUnitToSpawn = ownPlayer.initUnit(eUnitToSpawn, GC.getUnitInfo(eUnitToSpawn).getDefaultProfession(), pPlotToSpawn->getX_INLINE(), pPlotToSpawn->getY_INLINE(), NO_UNITAI);
	}
	return;
}

void CvUnit::spawnBarbarianUnitOnAdjacentPlotOfUnit(int /*UnitClassTypes*/ iIndex) const
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
		CvUnit* eBarbarianUnitToSpawn = barbarianPlayer.initUnit(eUnitToSpawn, GC.getUnitInfo(eUnitToSpawn).getDefaultProfession(), pPlotToSpawn->getX_INLINE(), pPlotToSpawn->getY_INLINE(), NO_UNITAI);
	}
	return;
}

bool CvUnit::isOwnPlayerUnitOnAdjacentPlotOfUnit(int /*UnitClassTypes*/ iIndex) const
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

bool CvUnit::isBarbarianUnitOnAdjacentPlotOfUnit(int /*UnitClassTypes*/ iIndex) const
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

// Erik: We should come up with a XML tag (e.g. bJoin vs. bFound) so that we don't need to hard-code this
bool CvUnit::isPrisonerOrSlave() const
{
	const UnitClassTypes unitClassIntToBeChecked = m_pUnitInfo->getUnitClassType();

	if (unitClassIntToBeChecked == UNITCLASS_PRISONER_OF_WAR || unitClassIntToBeChecked == UNITCLASS_NATIVE_SLAVE || unitClassIntToBeChecked == UNITCLASS_AFRICAN_SLAVE)
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

void CvUnit::read(FDataStreamBase* pStream)
{
	CvSavegameReaderBase readerbase(pStream);
	CvSavegameReader reader(readerbase);

	read(reader);
}

void CvUnit::write(FDataStreamBase* pStream)
{
	CvSavegameWriterBase writerbase(pStream);
	CvSavegameWriter writer(writerbase);
	write(writer);
	writerbase.WriteFile();
}

bool CvUnit::isForcedLaborer() const
{
	CitizenStatusTypes eCitizenStatus = getUnitInfo().getCitizenStatus();
	if (eCitizenStatus == CITIZEN_STATUS_ENSLAVED)
	{
		return true;
	}
	return false;
}

int CvUnit::getForcedLaborFactor() const
{
	// just a simple wrapper function, but could be changed later
	CitizenStatusTypes eCitizenStatus = getUnitInfo().getCitizenStatus();
	if (eCitizenStatus == CITIZEN_STATUS_ENSLAVED)
	{
		return 2;
	}
	else if (eCitizenStatus == CITIZEN_STATUS_INDENTURED)
	{
		return 1;
	}

	return 0;
};

int CvUnit::getDiscriminationFactor() const
{
	EthnicityTypes eEthnicity = getUnitInfo().getEthnicity();
	if (getOwnerR().isNative())
	{
		if (eEthnicity == ETHNICITY_INDIO)
		{
			return 0;
		}
		else if (eEthnicity == ETHNICITY_MESTIZZO)
		{
		return 1;
		}
		else
		{
			return 2;
		}
	}
	else if (getOwnerR().isColonialNation())
	{
		if (eEthnicity == ETHNICITY_EUROPEAN)
		{
			return 0;
		}
		else if (eEthnicity == ETHNICITY_MESTIZZO || eEthnicity == ETHNICITY_MULATTO)
		{
		return 1;
		}
		else
		{
			return 2;
		}
	}
	return 0;
}
