// team.cpp

#include "CvGameCoreDLL.h"
#include "CvPlayerAI.h"
#include "CvDefines.h"
#include "CvGameCoreUtils.h"
#include "CvGlobals.h"
#include "CvTeamAI.h"
#include "CvPlayerAI.h"
#include "CvGameAI.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvTeam.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLEventReporterIFaceBase.h"
#include "CvDLLEngineIFaceBase.h"
#include "CvDiploParameters.h"
#include "CvInfos.h"
#include "CvPopupInfo.h"
#include "CvDLLPythonIFaceBase.h"
#include "CyArgsList.h"
#include "FProfiler.h"

#include "CvSavegame.h"
#include "BetterBTSAI.h"
// Public Functions...

CvTeam::CvTeam()
{
	reset(FIRST_TEAM, true);
}


CvTeam::~CvTeam()
{
	uninit();
}


void CvTeam::init(TeamTypes eID)
{
	//--------------------------------
	// Init saved data
	reset(eID);

	//--------------------------------
	// Init non-saved data

	//--------------------------------
	// Init other game data
	AI_init();
}


void CvTeam::uninit()
{
}


// FUNCTION: reset()
// Initializes data members that are serialized.
void CvTeam::reset(TeamTypes eID, bool bConstructorCall)
{
	//--------------------------------
	// Uninit class
	uninit();

	resetSavedData(eID);

	if (!bConstructorCall)
	{
		AI_reset();
	}
}


void CvTeam::addTeam(TeamTypes eTeam)
{
	CLLNode<TradeData>* pNode;
	CvDeal* pLoopDeal;
	CvPlot* pLoopPlot;
	CvWString szBuffer;
	bool bValid;
	int iLoop;
	int iI, iJ;

	FAssert(eTeam != NO_TEAM);
	FAssert(eTeam != getID());

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if ((GET_PLAYER((PlayerTypes)iI).getTeam() != getID()) && (GET_PLAYER((PlayerTypes)iI).getTeam() != eTeam))
			{
				if (GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isHasMet(getID()) && GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isHasMet(eTeam))
				{
					szBuffer = gDLL->getText("TXT_KEY_MISC_PLAYER_PERMANENT_ALLIANCE", getName().GetCString(), GET_TEAM(eTeam).getName().GetCString());
					gDLL->UI().addPlayerMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_THEIRALLIANCE", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
				}
			}
		}
	}

	szBuffer = gDLL->getText("TXT_KEY_MISC_PLAYER_PERMANENT_ALLIANCE", getName().GetCString(), GET_TEAM(eTeam).getName().GetCString());
	GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), szBuffer, -1, -1, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));


	for (pLoopDeal = GC.getGameINLINE().firstDeal(&iLoop); pLoopDeal != NULL; pLoopDeal = GC.getGameINLINE().nextDeal(&iLoop))
	{
		if (((GET_PLAYER(pLoopDeal->getFirstPlayer()).getTeam() == getID()) && (GET_PLAYER(pLoopDeal->getSecondPlayer()).getTeam() == eTeam)) ||
			  ((GET_PLAYER(pLoopDeal->getFirstPlayer()).getTeam() == eTeam) && (GET_PLAYER(pLoopDeal->getSecondPlayer()).getTeam() == getID())))
		{
			bValid = true;

			if (pLoopDeal->getFirstTrades() != NULL)
			{
				for (pNode = pLoopDeal->getFirstTrades()->head(); pNode; pNode = pLoopDeal->getFirstTrades()->next(pNode))
				{
					if ((pNode->m_data.m_eItemType == TRADE_OPEN_BORDERS) ||
						  (pNode->m_data.m_eItemType == TRADE_DEFENSIVE_PACT) ||
						  (pNode->m_data.m_eItemType == TRADE_PEACE_TREATY))
					{
						bValid = false;
					}
				}
			}

			if (pLoopDeal->getSecondTrades() != NULL)
			{
				for (pNode = pLoopDeal->getSecondTrades()->head(); pNode; pNode = pLoopDeal->getSecondTrades()->next(pNode))
				{
					if ((pNode->m_data.m_eItemType == TRADE_OPEN_BORDERS) ||
						  (pNode->m_data.m_eItemType == TRADE_DEFENSIVE_PACT) ||
						  (pNode->m_data.m_eItemType == TRADE_PEACE_TREATY))
					{
						bValid = false;
					}
				}
			}

			if (!bValid)
			{
				pLoopDeal->kill(true, NO_TEAM);
			}
		}
	}

	shareItems(eTeam);
	GET_TEAM(eTeam).shareItems(getID());

	for (iI = 0; iI < GC.getNumFatherInfos(); ++iI)
	{
		FatherTypes eFather = (FatherTypes) iI;

		//transfer father ownership from other team to us
		if (GC.getGameINLINE().getFatherTeam(eFather) == eTeam)
		{
			GC.getGameINLINE().setFatherTeam(eFather, getID());
		}

		//give father benefits to other team's players
		if (GC.getGameINLINE().getFatherTeam(eFather) == getID())
		{
			GET_TEAM(eTeam).processFather(eFather, 1);
		}
	}

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		if ((iI != getID()) && (iI != eTeam))
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				if (GET_TEAM(eTeam).isHasMet((TeamTypes)iI))
				{
					meet(((TeamTypes)iI), false);
				}
				else if (isHasMet((TeamTypes)iI))
				{
					GET_TEAM(eTeam).meet(((TeamTypes)iI), false);
				}
			}
		}
	}

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		if ((iI != getID()) && (iI != eTeam))
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				if (GET_TEAM(eTeam).isAtWar((TeamTypes)iI))
				{
					declareWar(((TeamTypes)iI), false, GET_TEAM(eTeam).AI_getWarPlan((TeamTypes)iI));
				}
				else if (isAtWar((TeamTypes)iI))
				{
					GET_TEAM(eTeam).declareWar(((TeamTypes)iI), false, AI_getWarPlan((TeamTypes)iI));
				}
			}
		}
	}

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		if ((iI != getID()) && (iI != eTeam))
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				if (GET_TEAM(eTeam).isPermanentWarPeace((TeamTypes)iI))
				{
					setPermanentWarPeace(((TeamTypes)iI), true);
				}
				else if (isPermanentWarPeace((TeamTypes)iI))
				{
					GET_TEAM(eTeam).setPermanentWarPeace(((TeamTypes)iI), true);
				}
			}
		}
	}

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		if ((iI != getID()) && (iI != eTeam))
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				if (GET_TEAM(eTeam).isOpenBorders((TeamTypes)iI))
				{
					setOpenBorders(((TeamTypes)iI), true);
					GET_TEAM((TeamTypes)iI).setOpenBorders(getID(), true);
				}
				else if (isOpenBorders((TeamTypes)iI))
				{
					GET_TEAM(eTeam).setOpenBorders(((TeamTypes)iI), true);
					GET_TEAM((TeamTypes)iI).setOpenBorders(eTeam, true);
				}
			}
		}
	}

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		if ((iI != getID()) && (iI != eTeam))
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				if (GET_TEAM(eTeam).isDefensivePact((TeamTypes)iI))
				{
					setDefensivePact(((TeamTypes)iI), true);
					GET_TEAM((TeamTypes)iI).setDefensivePact(getID(), true);
				}
				else if (isDefensivePact((TeamTypes)iI))
				{
					GET_TEAM(eTeam).setDefensivePact(((TeamTypes)iI), true);
					GET_TEAM((TeamTypes)iI).setDefensivePact(eTeam, true);
				}
			}
		}
	}

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		if ((iI != getID()) && (iI != eTeam))
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				if (GET_TEAM(eTeam).isForcePeace((TeamTypes)iI))
				{
					setForcePeace(((TeamTypes)iI), true);
					GET_TEAM((TeamTypes)iI).setForcePeace(getID(), true);
				}
				else if (isForcePeace((TeamTypes)iI))
				{
					GET_TEAM(eTeam).setForcePeace(((TeamTypes)iI), true);
					GET_TEAM((TeamTypes)iI).setForcePeace(eTeam, true);
				}
			}
		}
	}

	shareCounters(eTeam);
	GET_TEAM(eTeam).shareCounters(getID());

	bool bActiveTeamSwitched = (GC.getGameINLINE().getActiveTeam() == eTeam);

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam)
		{
			GET_PLAYER((PlayerTypes)iI).setTeam(getID());
		}
	}

	for (iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

		pLoopPlot->changeVisibilityCount(getID(), pLoopPlot->getVisibilityCount(eTeam), NO_INVISIBLE);

		for (iJ = 0; iJ < GC.getNumInvisibleInfos(); iJ++)
		{
			pLoopPlot->changeInvisibleVisibilityCount(getID(), ((InvisibleTypes)iJ), pLoopPlot->getInvisibleVisibilityCount(eTeam, ((InvisibleTypes)iJ)));
		}

		if (pLoopPlot->isRevealed(eTeam, false))
		{
			pLoopPlot->setRevealed(getID(), true, false, eTeam);
		}
	}

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		if ((iI != getID()) && (iI != eTeam))
		{
			GET_TEAM((TeamTypes)iI).AI_setAtWarCounter(getID(), ((GET_TEAM((TeamTypes)iI).AI_getAtWarCounter(getID()) + GET_TEAM((TeamTypes)iI).AI_getAtWarCounter(eTeam)) / 2));
			GET_TEAM((TeamTypes)iI).AI_setAtPeaceCounter(getID(), ((GET_TEAM((TeamTypes)iI).AI_getAtPeaceCounter(getID()) + GET_TEAM((TeamTypes)iI).AI_getAtPeaceCounter(eTeam)) / 2));
			GET_TEAM((TeamTypes)iI).AI_setHasMetCounter(getID(), ((GET_TEAM((TeamTypes)iI).AI_getHasMetCounter(getID()) + GET_TEAM((TeamTypes)iI).AI_getHasMetCounter(eTeam)) / 2));
			GET_TEAM((TeamTypes)iI).AI_setDefensivePactCounter(getID(), ((GET_TEAM((TeamTypes)iI).AI_getDefensivePactCounter(getID()) + GET_TEAM((TeamTypes)iI).AI_getDefensivePactCounter(eTeam)) / 2));
			GET_TEAM((TeamTypes)iI).AI_setShareWarCounter(getID(), ((GET_TEAM((TeamTypes)iI).AI_getShareWarCounter(getID()) + GET_TEAM((TeamTypes)iI).AI_getShareWarCounter(eTeam)) / 2));
			GET_TEAM((TeamTypes)iI).AI_setWarSuccess(getID(), ((GET_TEAM((TeamTypes)iI).AI_getWarSuccess(getID()) + GET_TEAM((TeamTypes)iI).AI_getWarSuccess(eTeam)) / 2));
			GET_TEAM((TeamTypes)iI).AI_setEnemyPeacetimeTradeValue(getID(), ((GET_TEAM((TeamTypes)iI).AI_getEnemyPeacetimeTradeValue(getID()) + GET_TEAM((TeamTypes)iI).AI_getEnemyPeacetimeTradeValue(eTeam)) / 2));
			GET_TEAM((TeamTypes)iI).AI_setEnemyPeacetimeGrantValue(getID(), ((GET_TEAM((TeamTypes)iI).AI_getEnemyPeacetimeGrantValue(getID()) + GET_TEAM((TeamTypes)iI).AI_getEnemyPeacetimeGrantValue(eTeam)) / 2));

			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				GET_TEAM((TeamTypes)iI).AI_setWarPlan(getID(), NO_WARPLAN, false);
				GET_TEAM((TeamTypes)iI).AI_setWarPlan(eTeam, NO_WARPLAN, false);
			}
		}
	}

	AI_updateWorstEnemy();

	AI_updateAreaStragies();

	GC.getGameINLINE().updateScore(true);

	if (bActiveTeamSwitched)
	{
		for (iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
		{
			pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
			pLoopPlot->updateSymbols();
			pLoopPlot->updateFog();
			pLoopPlot->updateVisibility();
		}
		gDLL->getInterfaceIFace()->setDirty(MinimapSection_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(GlobeLayer_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(ColoredPlots_DIRTY_BIT, true);
	}

}


void CvTeam::shareItems(TeamTypes eTeam)
{
	FAssert(eTeam != NO_TEAM);
	FAssert(eTeam != getID());

	for (int iI = 0; iI < GC.getNumUnitClassInfos(); ++iI)
	{
		UnitClassTypes eUnitClass = (UnitClassTypes) iI;
		int iOtherTeamUnits = GET_TEAM(eTeam).getUnitsPurchasedHistory(eUnitClass);
		if (getUnitsPurchasedHistory(eUnitClass) < iOtherTeamUnits)
		{
			changeUnitsPurchasedHistory(eUnitClass, iOtherTeamUnits - getUnitsPurchasedHistory(eUnitClass));
		}
	}

	for (int iI = 0; iI < GC.getNumBonusInfos(); ++iI)
	{
		if (GET_TEAM(eTeam).isForceRevealedBonus((BonusTypes)iI))
		{
			setForceRevealedBonus((BonusTypes)iI, true);
		}
	}
}


void CvTeam::shareCounters(TeamTypes eTeam)
{
	CvTeam& kOtherTeam = GET_TEAM(eTeam);

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if ((iI != getID()) && (iI != eTeam))
		{
			if (kOtherTeam.AI_getAtWarCounter((TeamTypes)iI) > AI_getAtWarCounter((TeamTypes)iI))
			{
				AI_setAtWarCounter(((TeamTypes)iI), kOtherTeam.AI_getAtWarCounter((TeamTypes)iI));
			}

			if (kOtherTeam.AI_getAtPeaceCounter((TeamTypes)iI) > AI_getAtPeaceCounter((TeamTypes)iI))
			{
				AI_setAtPeaceCounter(((TeamTypes)iI), kOtherTeam.AI_getAtPeaceCounter((TeamTypes)iI));
			}

			if (kOtherTeam.AI_getHasMetCounter((TeamTypes)iI) > AI_getHasMetCounter((TeamTypes)iI))
			{
				AI_setHasMetCounter(((TeamTypes)iI), kOtherTeam.AI_getHasMetCounter((TeamTypes)iI));
			}

			if (kOtherTeam.AI_getOpenBordersCounter((TeamTypes)iI) > AI_getOpenBordersCounter((TeamTypes)iI))
			{
				AI_setOpenBordersCounter(((TeamTypes)iI), kOtherTeam.AI_getOpenBordersCounter((TeamTypes)iI));
			}

			if (kOtherTeam.AI_getDefensivePactCounter((TeamTypes)iI) > AI_getDefensivePactCounter((TeamTypes)iI))
			{
				AI_setDefensivePactCounter(((TeamTypes)iI), kOtherTeam.AI_getDefensivePactCounter((TeamTypes)iI));
			}

			if (kOtherTeam.AI_getShareWarCounter((TeamTypes)iI) > AI_getShareWarCounter((TeamTypes)iI))
			{
				AI_setShareWarCounter(((TeamTypes)iI), kOtherTeam.AI_getShareWarCounter((TeamTypes)iI));
			}

			if (kOtherTeam.AI_getWarSuccess((TeamTypes)iI) > AI_getWarSuccess((TeamTypes)iI))
			{
				AI_setWarSuccess(((TeamTypes)iI), kOtherTeam.AI_getWarSuccess((TeamTypes)iI));
			}

			if (kOtherTeam.AI_getEnemyPeacetimeTradeValue((TeamTypes)iI) > AI_getEnemyPeacetimeTradeValue((TeamTypes)iI))
			{
				AI_setEnemyPeacetimeTradeValue(((TeamTypes)iI), kOtherTeam.AI_getEnemyPeacetimeTradeValue((TeamTypes)iI));
			}

			if (kOtherTeam.AI_getEnemyPeacetimeGrantValue((TeamTypes)iI) > AI_getEnemyPeacetimeGrantValue((TeamTypes)iI))
			{
				AI_setEnemyPeacetimeGrantValue(((TeamTypes)iI), kOtherTeam.AI_getEnemyPeacetimeGrantValue((TeamTypes)iI));
			}

			kOtherTeam.AI_setWarPlan(((TeamTypes)iI), NO_WARPLAN, false);
		}
	}

	for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		changeUnitClassCount(((UnitClassTypes)iI), kOtherTeam.getUnitClassCount((UnitClassTypes)iI));
	}

	for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		changeBuildingClassCount((BuildingClassTypes)iI, kOtherTeam.getBuildingClassCount((BuildingClassTypes)iI));
	}

	for (int iI = 0; iI < GC.getNumFatherPointInfos(); iI++)
	{
		FatherPointTypes ePointType = (FatherPointTypes) iI;
		changeFatherPoints(ePointType, kOtherTeam.getFatherPoints(ePointType));
	}

	for (int iI = 0; iI < GC.getNumFatherInfos(); iI++)
	{
		FatherTypes eFather = (FatherTypes) iI;
		setFatherIgnore(eFather, isFatherIgnore(eFather) && kOtherTeam.isFatherIgnore(eFather));
	}
}

void CvTeam::processFather(FatherTypes eFather, int iChange)
{
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayer);
		if (kPlayer.isAlive() && kPlayer.getTeam() == getID())
		{
			kPlayer.processFather(eFather, iChange);
		}
	}
}

void CvTeam::doTurn()
{
	PROFILE_FUNC();
	FAssertMsg(isAlive(), "isAlive is expected to be true");
	AI_doTurnPre();
	testFoundingFather();

	AI_doTurnPost();
}


void CvTeam::updateYield()
{
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				GET_PLAYER((PlayerTypes)iI).updateYield();
			}
		}
	}
}

bool CvTeam::canChangeWarPeace(TeamTypes eTeam) const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_NO_CHANGING_WAR_PEACE))
	{
		return false;
	}

	if (eTeam == getID())
	{
		return false;
	}

	if (isPermanentWarPeace(eTeam) || GET_TEAM(eTeam).isPermanentWarPeace(getID()))
	{
		return false;
	}

	return true;
}


bool CvTeam::canDeclareWar(TeamTypes eTeam) const
{
	if (eTeam == getID())
	{
		return false;
	}

	if (isAtWar(eTeam))
	{
		return false;
	}

	if (!(isAlive()) || !(GET_TEAM(eTeam).isAlive()))
	{
		return false;
	}

	if (!isHasMet(eTeam))
	{
		return false;
	}

	if (GET_TEAM(eTeam).isParentOf(getID()))
	{
		if (!canDoRevolution())
		{
			return false;
		}
	}
	else if (GET_TEAM(eTeam).hasEuropePlayer())
	{
		if (hasColonialPlayer() && !isInRevolution())
		{
			return false;
		}
	}


	if (hasColonialPlayer() && GET_TEAM(eTeam).hasColonialPlayer())
	{
		if (GC.getGame().getRemainingForcedPeaceTurns() > 0)
		{
			return false;
		}
	}

	if (hasEuropePlayer())
	{
		return false;
	}

	if (isForcePeace(eTeam))
	{
		return false;
	}

	if (!canChangeWarPeace(eTeam))
	{
		return false;
	}

	if (GC.getGameINLINE().isOption(GAMEOPTION_ALWAYS_PEACE))
	{
		return false;
	}

	if(GC.getUSE_CAN_DECLARE_WAR_CALLBACK())
	{
		CyArgsList argsList;
		argsList.add(getID());	// Team ID
		argsList.add(eTeam);	// pass in city class
		long lResult=0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "canDeclareWar", argsList.makeFunctionArgs(), &lResult);

		if (lResult == 0)
		{
			return false;
		}
	}

	return true;
}

void CvTeam::declareWarNoRevolution(TeamTypes eTeam, bool bNewDiplo, WarPlanTypes eWarPlan, bool bPlaySound)
{
	PROFILE_FUNC();

	CLLNode<TradeData>* pNode;
	CvDiploParameters* pDiplo;
	CvDeal* pLoopDeal;
	CvWString szBuffer;
	bool bCancelDeal;
	int iLoop;
	int iI, iJ;

	FAssertMsg(eTeam != NO_TEAM, "eTeam is not assigned a valid value");
	FAssertMsg(eTeam != getID(), "eTeam is not expected to be equal with getID()");

	if (!isAtWar(eTeam))
	{
		for (pLoopDeal = GC.getGameINLINE().firstDeal(&iLoop); pLoopDeal != NULL; pLoopDeal = GC.getGameINLINE().nextDeal(&iLoop))
		{
			if (((GET_PLAYER(pLoopDeal->getFirstPlayer()).getTeam() == getID()) && (GET_PLAYER(pLoopDeal->getSecondPlayer()).getTeam() == eTeam)) ||
				((GET_PLAYER(pLoopDeal->getFirstPlayer()).getTeam() == eTeam) && (GET_PLAYER(pLoopDeal->getSecondPlayer()).getTeam() == getID())))
			{
				pLoopDeal->kill(true, getID());
			}
		}

		FAssertMsg(eTeam != getID(), "eTeam is not expected to be equal with getID()");
		setAtWar(eTeam, true);
		GET_TEAM(eTeam).setAtWar(getID(), true);

		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
			if ((kPlayer.getTeam() == getID()) || (kPlayer.getTeam() == eTeam))
			{
				kPlayer.validateMissions();
				kPlayer.validateTradePosts(); // WTP, ray, Native Trade Posts - START
			}
		}

		meet(eTeam, false);

		AI_setAtPeaceCounter(eTeam, 0);
		GET_TEAM(eTeam).AI_setAtPeaceCounter(getID(), 0);

		AI_setShareWarCounter(eTeam, 0);
		GET_TEAM(eTeam).AI_setShareWarCounter(getID(), 0);

		GET_TEAM(eTeam).AI_setWarPlan(getID(), WARPLAN_ATTACKED_RECENT);

		for (iI = 0; iI < MAX_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				if (!GET_TEAM(eTeam).isAtWar((TeamTypes)iI) && GET_TEAM(eTeam).AI_isChosenWar((TeamTypes)iI))
				{
					GET_TEAM(eTeam).AI_setWarPlan(((TeamTypes)iI), NO_WARPLAN);
				}
			}
		}

		if (NO_WARPLAN != eWarPlan)
		{
			AI_setWarPlan(eTeam, eWarPlan);
		}

		FAssert(!(AI_isSneakAttackPreparing(eTeam)));
		if ((AI_getWarPlan(eTeam) == NO_WARPLAN) || AI_isSneakAttackPreparing(eTeam))
		{
			if (isHuman())
			{
				AI_setWarPlan(eTeam, WARPLAN_TOTAL);
			}
			else if (GET_TEAM(eTeam).getAtWarCount() == 1)
			{
				AI_setWarPlan(eTeam, WARPLAN_LIMITED);
			}
			else
			{
				AI_setWarPlan(eTeam, WARPLAN_DOGPILE);
			}
		}

		GET_TEAM(getID()).AI_doDamages(eTeam, false);

		GC.getMapINLINE().verifyUnitValidPlot();

		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				GET_PLAYER((PlayerTypes)iI).verifyUnitStacksValid();
			}
		}

		GC.getGameINLINE().AI_makeAssignWorkDirty();

		if ((getID() == GC.getGameINLINE().getActiveTeam()) || (eTeam == GC.getGameINLINE().getActiveTeam()))
		{
			gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(ColoredPlots_DIRTY_BIT, true);
		}

		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				for (iJ = 0; iJ < MAX_PLAYERS; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iJ).isAlive())
					{
						if ((GET_PLAYER((PlayerTypes)iI).getTeam() == getID()) && (GET_PLAYER((PlayerTypes)iJ).getTeam() == eTeam))
						{
							GET_PLAYER((PlayerTypes)iI).AI_setFirstContact(((PlayerTypes)iJ), true);
							GET_PLAYER((PlayerTypes)iJ).AI_setFirstContact(((PlayerTypes)iI), true);
						}
					}
				}
			}
		}

		// Note: iI is a possible player on our team
		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				// Note: iJ  is the target of the declaration
				for (iJ = 0; iJ < MAX_PLAYERS; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iJ).isAlive())
					{
						if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
						{
							if (GET_PLAYER((PlayerTypes)iJ).getTeam() == eTeam)
							{
								// Set the target to remember the war declaration towards all players in the iI's team
								GET_PLAYER((PlayerTypes)iJ).AI_changeMemoryCount(((PlayerTypes)iI), MEMORY_DECLARED_WAR, 1);

								// Check if the target has a parent (Europe)
								if (GET_PLAYER((PlayerTypes)iJ).getParent() != NO_PLAYER)
								{
									// Erik: If the player declares war on the colonies of another European nation, add a major diplo penalty
									// with its king as well UNLESS they are already at war
									if (!GET_TEAM(GET_PLAYER(GET_PLAYER((PlayerTypes)iJ).getParent()).getTeam()).isAtWar(GET_PLAYER((PlayerTypes)iJ).getTeam()))
									{
										// Set the target's king to remember the war declaration
										GET_PLAYER(GET_PLAYER((PlayerTypes)iJ).getParent()).AI_changeMemoryCount(((PlayerTypes)iI), MEMORY_DECLARED_WAR, 1);
									}
									else if (GET_PLAYER((PlayerTypes)iJ).getTeam() != getID())
									{
										if (GET_TEAM(GET_PLAYER((PlayerTypes)iJ).getTeam()).isHasMet(eTeam))
										{
											if (GET_TEAM(GET_PLAYER((PlayerTypes)iJ).getTeam()).AI_getAttitude(eTeam) >= ATTITUDE_PLEASED)
											{
												GET_PLAYER((PlayerTypes)iJ).AI_changeMemoryCount(((PlayerTypes)iI), MEMORY_DECLARED_WAR_ON_FRIEND, 1);
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

		if (GC.getGameINLINE().isFinalInitialized() && !(gDLL->GetWorldBuilderMode()))
		{
			if (bNewDiplo)
			{
				if (!isHuman())
				{
					for (iI = 0; iI < MAX_PLAYERS; iI++)
					{
						if (GET_PLAYER((PlayerTypes)iI).isAlive())
						{
							if (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam)
							{
								if (GET_PLAYER(getLeaderID()).canContact((PlayerTypes)iI))
								{
									if (GET_PLAYER((PlayerTypes)iI).isHuman())
									{
										pDiplo = new CvDiploParameters(getLeaderID());
										FAssertMsg(pDiplo != NULL, "pDiplo must be valid");
										pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_DECLARE_WAR"));
										pDiplo->setAIContact(true);
										gDLL->beginDiplomacy(pDiplo, ((PlayerTypes)iI));
									}
								}
							}
						}
					}
				}
			}

			for (iI = 0; iI < MAX_PLAYERS; iI++)
			{
				CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
				if (kLoopPlayer.isAlive())
				{
					if (kLoopPlayer.getTeam() == getID())
					{
						szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_DECLARED_WAR_ON", GET_TEAM(eTeam).getName().GetCString());
						gDLL->UI().addPlayerMessage(((PlayerTypes)iI), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, bPlaySound ? "AS2D_DECLAREWAR" : NULL, MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));
					}
					else if (kLoopPlayer.getTeam() == eTeam)
					{
						szBuffer = gDLL->getText("TXT_KEY_MISC_DECLARED_WAR_ON_YOU", getName().GetCString());
						gDLL->UI().addPlayerMessage(((PlayerTypes)iI), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, bPlaySound ? "AS2D_DECLAREWAR" : NULL, MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));
					}
					else if ((GET_TEAM(kLoopPlayer.getTeam()).isHasMet(getID()) || hasEuropePlayer()) && GET_TEAM(kLoopPlayer.getTeam()).isHasMet(eTeam))
					{
						szBuffer = gDLL->getText("TXT_KEY_MISC_SOMEONE_DECLARED_WAR", getName().GetCString(), GET_TEAM(eTeam).getName().GetCString());
						gDLL->UI().addPlayerMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, bPlaySound ? "AS2D_THEIRDECLAREWAR" : NULL, MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));
					}
				}
			}

			szBuffer = gDLL->getText("TXT_KEY_MISC_SOMEONE_DECLARES_WAR", getName().GetCString(), GET_TEAM(eTeam).getName().GetCString());
			GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), szBuffer, -1, -1, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));
		}

		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
				{
					for (pLoopDeal = GC.getGameINLINE().firstDeal(&iLoop); pLoopDeal != NULL; pLoopDeal = GC.getGameINLINE().nextDeal(&iLoop))
					{
						bCancelDeal = false;

						if ((pLoopDeal->getFirstPlayer() == ((PlayerTypes)iI)) || (pLoopDeal->getSecondPlayer() == ((PlayerTypes)iI)))
						{
							for (pNode = pLoopDeal->headFirstTradesNode(); (pNode != NULL); pNode = pLoopDeal->nextFirstTradesNode(pNode))
							{
								if (pNode->m_data.m_eItemType == TRADE_DEFENSIVE_PACT)
								{
									bCancelDeal = true;
								}
							}

							for (pNode = pLoopDeal->headSecondTradesNode(); (pNode != NULL); pNode = pLoopDeal->nextSecondTradesNode(pNode))
							{
								if (pNode->m_data.m_eItemType == TRADE_DEFENSIVE_PACT)
								{
									bCancelDeal = true;
								}
							}
						}

						if (bCancelDeal)
						{
							pLoopDeal->kill(true, getID());
						}
					}
				}
			}
		}

		gDLL->getEventReporterIFace()->changeWar(true, getID(), eTeam);
		if (gTeamLogLevel >= 1)
			logBBAI(" Team %d (%S) declares war on team %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), eTeam); // BETTER_BTS_AI_MOD (10/02/09, jdog5000): AI logging

		cancelDefensivePacts(getID());

		for (iI = 0; iI < MAX_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				if (GET_TEAM((TeamTypes)iI).isDefensivePact(eTeam))
				{
					GET_TEAM((TeamTypes)iI).declareWar(getID(), bNewDiplo, WARPLAN_DOGPILE);
				}
			}
		}

		GET_TEAM(eTeam).cancelDefensivePacts(getID());
	}
}



void CvTeam::declareWar(TeamTypes eTeam, bool bNewDiplo, WarPlanTypes eWarPlan)
{
	CvTeam& kOtherTeam = GET_TEAM(eTeam);

	if (kOtherTeam.isParentOf(getID()) || isParentOf(eTeam))
	{
		doRevolution();

		// TAC - Messages - Ray - START
		//Alert all Human Players of Revolution

		CvPlayer& iPlayer = GET_PLAYER((PlayerTypes) getID());
		PlayerTypes kingID = iPlayer.getParent();
		CvPlayer& King = GET_PLAYER(kingID);

		for (int i = 0; i < MAX_PLAYERS; ++i)
		{
			CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) i);
			if (kPlayer.isAlive() && kPlayer.isHuman())
			{
				PlayerTypes kingID = kPlayer.getParent();
				CvPlayer& King = GET_PLAYER(kingID);
				CvWString szMessage = gDLL->getText("TXT_KEY_COMPETITIOR_INDEPENDENCE_WAR", iPlayer.getCivilizationAdjectiveKey(), King.getCivilizationDescription());
				gDLL->UI().addPlayerMessage(kPlayer.getID(), true, GC.getEVENT_MESSAGE_TIME(), szMessage, "AS2D_CITY_REVOLT", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), NULL, NULL, false, false);			
			}
		}
		// TAC - Messages - Ray - END

	}
	else
	{
		declareWarNoRevolution(eTeam, bNewDiplo, eWarPlan, true);
	}
}

void CvTeam::makePeace(TeamTypes eTeam, bool bBumpUnits)
{
	FAssertMsg(eTeam != NO_TEAM, "eTeam is not assigned a valid value");
	FAssertMsg(eTeam != getID(), "eTeam is not expected to be equal with getID()");

	if (isAtWar(eTeam))
	{
		FAssertMsg(eTeam != getID(), "eTeam is not expected to be equal with getID()");
		setAtWar(eTeam, false);
		GET_TEAM(eTeam).setAtWar(getID(), false);

		AI_setAtWarCounter(eTeam, 0);
		GET_TEAM(eTeam).AI_setAtWarCounter(getID(), 0);

		AI_setWarSuccess(eTeam, 0);
		GET_TEAM(eTeam).AI_setWarSuccess(getID(), 0);

		AI_setWarPlan(eTeam, NO_WARPLAN);
		GET_TEAM(eTeam).AI_setWarPlan(getID(), NO_WARPLAN);

		if (bBumpUnits)
		{
			GC.getMapINLINE().verifyUnitValidPlot();
		}

		GC.getGameINLINE().AI_makeAssignWorkDirty();

		if ((getID() == GC.getGameINLINE().getActiveTeam()) || (eTeam == GC.getGameINLINE().getActiveTeam()))
		{
			gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}

		CvWString szBuffer;

		if(isAlive()) // R&R, ray, no message for Peace when Player has died
		{

			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
					{
						szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_MADE_PEACE_WITH", GET_TEAM(eTeam).getName().GetCString());
						gDLL->UI().addPlayerMessage(((PlayerTypes)iI), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_MAKEPEACE", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
					}
					else if (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam)
					{
						szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_MADE_PEACE_WITH", getName().GetCString());
						gDLL->UI().addPlayerMessage(((PlayerTypes)iI), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_MAKEPEACE", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
					}
					else if (GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isHasMet(getID()) && GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isHasMet(eTeam))
					{
						szBuffer = gDLL->getText("TXT_KEY_MISC_SOMEONE_MADE_PEACE", getName().GetCString(), GET_TEAM(eTeam).getName().GetCString());
						gDLL->UI().addPlayerMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_THEIRMAKEPEACE", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
					}
				}
			}

		}

		szBuffer = gDLL->getText("TXT_KEY_MISC_SOMEONE_MADE_PEACE", getName().GetCString(), GET_TEAM(eTeam).getName().GetCString());
		GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), szBuffer, -1, -1, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));

		gDLL->getEventReporterIFace()->changeWar(false, getID(), eTeam);
		if (gTeamLogLevel >= 1)
			logBBAI(" Team % d(% S) and team % d(% S) make peace", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), eTeam, GET_PLAYER(GET_TEAM(eTeam).getLeaderID()).getCivilizationDescription(0)); // BETTER_BTS_AI_MOD, AI logging, 05/21/10, jdog5000
	}
}


bool CvTeam::canContact(TeamTypes eTeam) const
{
	int iI, iJ;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				for (iJ = 0; iJ < MAX_PLAYERS; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iJ).isAlive())
					{
						if (GET_PLAYER((PlayerTypes)iJ).getTeam() == eTeam)
						{
							if (GET_PLAYER((PlayerTypes)iI).canContact((PlayerTypes)iJ))
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


void CvTeam::meet(TeamTypes eTeam, bool bNewDiplo)
{
	if (!isHasMet(eTeam))
	{
		makeHasMet(eTeam, bNewDiplo);
		GET_TEAM(eTeam).makeHasMet(getID(), bNewDiplo);
		if (gTeamLogLevel >= 2 && GC.getGame().isFinalInitialized() && eTeam != getID() && isAlive() && GET_TEAM(eTeam).isAlive())
			logBBAI(" Team % d(% S) meets team % d(% S)", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), eTeam, GET_PLAYER(GET_TEAM(eTeam).getLeaderID()).getCivilizationDescription(0)); // BETTER_BTS_AI_MOD, AI logging, 02/20/10, jdog5000
	}
}


void CvTeam::signOpenBorders(TeamTypes eTeam)
{
	CLinkList<TradeData> ourList;
	CLinkList<TradeData> theirList;
	TradeData item;

	FAssert(eTeam != NO_TEAM);
	FAssert(eTeam != getID());

	if (!isAtWar(eTeam) && (getID() != eTeam))
	{
		setTradeItem(&item, TRADE_OPEN_BORDERS, 0, NULL);

		if (GET_PLAYER(getLeaderID()).canTradeItem(GET_TEAM(eTeam).getLeaderID(), item) && GET_PLAYER(GET_TEAM(eTeam).getLeaderID()).canTradeItem(getLeaderID(), item))
		{
			ourList.clear();
			theirList.clear();

			ourList.insertAtEnd(item);
			theirList.insertAtEnd(item);

			GC.getGameINLINE().implementDeal(getLeaderID(), (GET_TEAM(eTeam).getLeaderID()), &ourList, &theirList);
		}
	}
}


void CvTeam::signDefensivePact(TeamTypes eTeam)
{
	CLinkList<TradeData> ourList;
	CLinkList<TradeData> theirList;
	TradeData item;

	FAssert(eTeam != NO_TEAM);
	FAssert(eTeam != getID());

	if (!isAtWar(eTeam) && (getID() != eTeam))
	{
		setTradeItem(&item, TRADE_DEFENSIVE_PACT, 0, NULL);

		if (GET_PLAYER(getLeaderID()).canTradeItem(GET_TEAM(eTeam).getLeaderID(), item) && GET_PLAYER(GET_TEAM(eTeam).getLeaderID()).canTradeItem(getLeaderID(), item))
		{
			ourList.clear();
			theirList.clear();

			ourList.insertAtEnd(item);
			theirList.insertAtEnd(item);

			GC.getGameINLINE().implementDeal(getLeaderID(), (GET_TEAM(eTeam).getLeaderID()), &ourList, &theirList);
		}
	}
}

bool CvTeam::canSignDefensivePact(TeamTypes eTeam)
{
	for (int iTeam = 0; iTeam < MAX_TEAMS; ++iTeam)
	{
		if (iTeam != getID() && iTeam != eTeam)
		{
			// R&R, ray, Changes for Wild Animals
			// Wild Animals should not block defensive pacts
			if((TeamTypes)iTeam != GET_PLAYER(GC.getGameINLINE().getBarbarianPlayer()).getTeam() && (TeamTypes)iTeam != GET_PLAYER(GC.getGameINLINE().getChurchPlayer()).getTeam())
			{

				CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iTeam);
				if (kLoopTeam.isPermanentWarPeace(eTeam) != kLoopTeam.isPermanentWarPeace(getID()))
				{
					return false;
				}

				if (isPermanentWarPeace((TeamTypes)iTeam) != GET_TEAM(eTeam).isPermanentWarPeace((TeamTypes)iTeam))
				{
					return false;
				}
			}
		}
	}

	return true;
}


int CvTeam::getAssets() const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iCount += GET_PLAYER((PlayerTypes)iI).getAssets();
			}
		}
	}

	return iCount;
}


int CvTeam::getPower() const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kLoopPlayer.isAlive())
		{
			if (kLoopPlayer.getTeam() == getID())
			{
				iCount += kLoopPlayer.getPower();
			}
		}
	}

	return iCount;
}


int CvTeam::getDefensivePower() const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iI);
		if (kLoopTeam.isAlive())
		{
			if (getID() == iI || isDefensivePact((TeamTypes)iI))
			{
				iCount += kLoopTeam.getPower();
			}
		}
	}

	return iCount;
}


int CvTeam::getEnemyPower() const
{
	int iCount = 0;

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iI);
		if (kLoopTeam.isAlive())
		{
			if (getID() != iI && isAtWar((TeamTypes)iI))
			{
				iCount += kLoopTeam.getPower();
			}
		}
	}

	return iCount;
}


int CvTeam::getAtWarCount() const
{
	int iCount = 0;

	for (TeamTypes iI = FIRST_TEAM; iI < MAX_TEAMS; ++iI)
	{
		// R&R, ray, Changes for Wild Animals
		// AI will not count fighting Animals as War
		const CvTeam& otherTeam = GET_TEAM(iI);
		if (otherTeam.isAlive() && !otherTeam.isBarbarian())
		{
			if (isAtWar(iI))
			{
				FAssert(iI != getID());
				FAssert(!(AI_isSneakAttackPreparing(iI)));
				iCount++;
			}
		}
	}

	return iCount;
}


int CvTeam::getWarPlanCount(WarPlanTypes eWarPlan) const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (AI_getWarPlan((TeamTypes)iI) == eWarPlan)
			{
				FAssert(iI != getID());
				iCount++;
			}
		}
	}

	return iCount;
}


int CvTeam::getAnyWarPlanCount() const
{
	int iCount = 0;

	for (TeamTypes iI = FIRST_TEAM; iI < MAX_TEAMS; ++iI)
	{
		// R&R, ray, Changes for Wild Animals
		// AI will not consider fighting Animals here
		const CvTeam& otherTeam = GET_TEAM(iI);
		if (otherTeam.isAlive() && !otherTeam.isBarbarian())
		{
			if (AI_getWarPlan(iI) != NO_WARPLAN)
			{
				FAssert(iI != getID());
				iCount++;
			}
		}
	}

	FAssert(iCount >= getAtWarCount());

	return iCount;
}


int CvTeam::getChosenWarCount() const
{
	int iCount = 0;

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (AI_isChosenWar((TeamTypes)iI))
			{
				FAssert(iI != getID());
				iCount++;
			}
		}
	}

	return iCount;
}


int CvTeam::getHasMetCivCount() const
{
	int iCount = 0;

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (iI != getID())
			{
				if (isHasMet((TeamTypes)iI))
				{
					FAssert(iI != getID());
					iCount++;
				}
			}
		}
	}

	return iCount;
}


bool CvTeam::hasMetHuman() const
{
	int iI;

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (iI != getID())
			{
				if (GET_TEAM((TeamTypes)iI).isHuman())
				{
					if (isHasMet((TeamTypes)iI))
					{
						FAssert(iI != getID());
						return true;
					}
				}
			}
		}
	}

	return false;
}


int CvTeam::getDefensivePactCount(TeamTypes eTeam) const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (iI != getID())
			{
				if (isDefensivePact((TeamTypes)iI))
				{
					if (NO_TEAM == eTeam || GET_TEAM(eTeam).isHasMet((TeamTypes)iI))
					{
						iCount++;
					}
				}
			}
		}
	}

	return iCount;
}

int CvTeam::getUnitClassMaking(UnitClassTypes eUnitClass) const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iCount += GET_PLAYER((PlayerTypes)iI).getUnitClassMaking(eUnitClass);
			}
		}
	}

	return iCount;
}


int CvTeam::getUnitClassCountPlusMaking(UnitClassTypes eIndex) const
{
	return (getUnitClassCount(eIndex) + getUnitClassMaking(eIndex));
}


int CvTeam::getBuildingClassMaking(BuildingClassTypes eBuildingClass) const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iCount += GET_PLAYER((PlayerTypes)iI).getBuildingClassMaking(eBuildingClass);
			}
		}
	}

	return iCount;
}


int CvTeam::getBuildingClassCountPlusMaking(BuildingClassTypes eIndex) const
{
	return (getBuildingClassCount(eIndex) + getBuildingClassMaking(eIndex));
}


int CvTeam::countTotalCulture() const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iCount += GET_PLAYER((PlayerTypes)iI).countTotalCulture();
			}
		}
	}

	return iCount;
}


int CvTeam::countNumUnitsByArea(CvArea* pArea) const
{
	PROFILE_FUNC();

	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iCount += pArea->getUnitsPerPlayer((PlayerTypes)iI);
			}
		}
	}

	return iCount;
}


int CvTeam::countNumCitiesByArea(CvArea* pArea) const
{
	PROFILE_FUNC();

	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iCount += pArea->getCitiesPerPlayer((PlayerTypes)iI);
			}
		}
	}

	return iCount;
}


int CvTeam::countTotalPopulationByArea(CvArea* pArea) const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iCount += pArea->getPopulationPerPlayer((PlayerTypes)iI);
			}
		}
	}

	return iCount;
}


int CvTeam::countPowerByArea(CvArea* pArea) const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iCount += pArea->getPower((PlayerTypes)iI);
			}
		}
	}

	return iCount;
}


int CvTeam::countEnemyPowerByArea(CvArea* pArea) const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() != getID())
			{
				if (isAtWar(GET_PLAYER((PlayerTypes)iI).getTeam()))
				{
					iCount += pArea->getPower((PlayerTypes)iI);
				}
			}
		}
	}

	return iCount;
}


int CvTeam::countNumAIUnitsByArea(CvArea* pArea, UnitAITypes eUnitAI) const
{
	PROFILE_FUNC();

	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iCount += pArea->getNumAIUnits(((PlayerTypes)iI), eUnitAI);
			}
		}
	}

	return iCount;
}


int CvTeam::countEnemyDangerByArea(CvArea* pArea) const
{
	PROFILE_FUNC();

	CvPlot* pLoopPlot;
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

		if (pLoopPlot != NULL)
		{
			if (pLoopPlot->area() == pArea)
			{
				if (pLoopPlot->getTeam() == getID())
				{
					iCount += pLoopPlot->plotCount(PUF_canDefendEnemy, getLeaderID(), false, NO_PLAYER, NO_TEAM, PUF_isVisible, getLeaderID());
				}
			}
		}
	}

	return iCount;
}

int CvTeam::getFatherCostModifier() const
{
	int iCostModifier = 100;

	iCostModifier *= GC.getHandicapInfo(getHandicapType()).getFatherPercent();
	iCostModifier /= 100;

	iCostModifier *= GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getFatherPercent();
	iCostModifier /= 100;

	iCostModifier *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getFatherPercent();
	iCostModifier /= 100;

	iCostModifier *= GC.getEraInfo(GC.getGameINLINE().getStartEra()).getFatherPercent();
	iCostModifier /= 100;

	iCostModifier *= std::max(0, ((GC.getDefineINT("FATHER_COST_EXTRA_TEAM_MEMBER_MODIFIER") * (getNumMembers() - 1)) + 100));
	iCostModifier /= 100;

	return iCostModifier;
}

int CvTeam::getFatherPointCost(FatherTypes eFather, FatherPointTypes ePointType) const
{
	return GC.getFatherInfo(eFather).getPointCost(ePointType) * getFatherCostModifier() / 100;
}

int CvTeam::getFatherPoints(FatherPointTypes ePointType) const
{
	FAssert((ePointType >= 0) && (ePointType < GC.getNumFatherPointInfos()));
	return m_em_iFatherPoints.get(ePointType);
}

void CvTeam::changeFatherPoints(FatherPointTypes ePointType, int iChange)
{
	FAssert((ePointType >= 0) && (ePointType < GC.getNumFatherPointInfos()));

	if (iChange != 0)
	{
		if (hasColonialPlayer())
		{
			m_em_iFatherPoints.add(ePointType, iChange);
			FAssert(m_em_iFatherPoints.get(ePointType) >= 0);
		}
	}
}

int CvTeam::getBestFatherPointMultiplier() const
{
	int iBestModifier = 0;

	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes) iPlayer);
		if (kLoopPlayer.isAlive() && kLoopPlayer.getTeam() == getID())
		{
			if (kLoopPlayer.getFatherPointMultiplier() > iBestModifier)
			{
				iBestModifier = kLoopPlayer.getFatherPointMultiplier();
			}
		}
	}

	return iBestModifier;
}

bool CvTeam::isFatherIgnore(FatherTypes eFather) const
{
	FAssert((eFather >= 0) && (eFather < GC.getNumFatherInfos()));
	return m_em_bFatherIgnore.get(eFather);
}

void CvTeam::setFatherIgnore(FatherTypes eFather, bool bValue)
{
	FAssert((eFather >= 0) && (eFather < GC.getNumFatherInfos()));
	m_em_bFatherIgnore.set(eFather, bValue);
}

bool CvTeam::canConvinceFather(FatherTypes eFather) const
{
	// R&R, ray, Founding Fathers Available again, when Civ destroyed - START
	if (!GC.getGameINLINE().isOption(GAMEOPTION_USE_OLD_FOUNDING_FATHER_SYSTEM))
	{
		if(GC.getGameINLINE().getFatherTeam(eFather) != NO_TEAM && GET_TEAM(GC.getGameINLINE().getFatherTeam(eFather)).isAlive())
		{
			return false;
		}
	}
	else
	{
		if(GC.getGameINLINE().getFatherTeam(eFather) != NO_TEAM)
		{
			return false;
		}
	}
	// R&R, ray, Founding Fathers Available again, when Civ destroyed - END

	if(isFatherIgnore(eFather))
	{
		return false;
	}

    for (int iPoint = 0; iPoint < GC.getNumFatherPointInfos(); ++iPoint)
	{
		FatherPointTypes ePointType = (FatherPointTypes) iPoint;
		if(getFatherPoints(ePointType) < getFatherPointCost(eFather, ePointType))
		{
			return false;
		}
	}

	return true;
}

void CvTeam::convinceFather(FatherTypes eFather, bool bAccept)
{
	if (!canConvinceFather(eFather))
	{
		return;
	}

	if (bAccept)
	{
		for (int iPoint = 0; iPoint < GC.getNumFatherPointInfos(); ++iPoint)
		{
			FatherPointTypes ePointType = (FatherPointTypes) iPoint;
			changeFatherPoints(ePointType, -getFatherPointCost(eFather, ePointType));
		}

		GC.getGameINLINE().setFatherTeam(eFather, getID());

		for (TeamTypes eTeam = FIRST_TEAM; eTeam < NUM_TEAM_TYPES; ++eTeam)
		{
			// notify all teams that this FF is now taken (as of now, this has no effect)
			GET_TEAM(eTeam).notifyFatherAvailability(eFather, false);
		}

	}
	else //reject
	{
		setFatherIgnore(eFather, true);
	}

	// This is a recursive call, because we are called from testFoundingFather! Why?
	// If we disable it, there will only be one FF offered to each team per turn, which is preferable for balancing IMHO:
	//testFoundingFather();
}

bool CvTeam::isHuman() const
{
	PROFILE_FUNC();

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			if (GET_PLAYER((PlayerTypes)iI).isHuman())
			{
				return true;
			}
		}
	}

	return false;
}

bool CvTeam::hasNativePlayer() const
{
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			if (GET_PLAYER((PlayerTypes)iI).isNative())
			{
				return true;
			}
		}
	}

	return false;
}

bool CvTeam::hasColonialPlayer() const
{
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			if(GET_PLAYER((PlayerTypes)iI).getParent() != NO_PLAYER)
			{
				return true;
			}
		}
	}

	return false;
}

bool CvTeam::hasEuropePlayer() const
{
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			if(GET_PLAYER((PlayerTypes)iI).isEurope())
			{
				return true;
			}
		}
	}

	return false;
}

bool CvTeam::isBarbarian() const
{
	// Adds an easy way to identify the barbarian team.
	// Works without crashing even when game init calls it prior to the barbarian player getting added.
	// Added by Nightinggale
	const PlayerTypes ePlayerBarbarian = GC.getGameINLINE().getBarbarianPlayer();
	if (ePlayerBarbarian != NO_PLAYER)
	{
		return GET_PLAYER(ePlayerBarbarian).getTeam() == getID();
	}
	return false;
}

PlayerTypes CvTeam::getLeaderID() const
{
	int iI;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				return ((PlayerTypes)iI);
			}
		}
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			return ((PlayerTypes)iI);
		}
	}

	return NO_PLAYER;
}


HandicapTypes CvTeam::getHandicapType() const
{
	int iGameHandicap;
	int iCount;
	int iI;

	iGameHandicap = 0;
	iCount = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iGameHandicap += GET_PLAYER((PlayerTypes)iI).getHandicapType();
				iCount++;
			}
		}
	}

	if (iCount > 0)
	{
		FAssertMsg((iGameHandicap / iCount) >= 0, "(iGameHandicap / iCount) is expected to be non-negative (invalid Index)");
		return ((HandicapTypes)(iGameHandicap / iCount));
	}
	else
	{
		return ((HandicapTypes)(GC.getDefineINT("STANDARD_HANDICAP")));
	}
}


CvWString CvTeam::getName() const
{
	CvWString szBuffer;
	bool bFirst;
	int iI;

	bFirst = true;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			setListHelp(szBuffer, L"", GET_PLAYER((PlayerTypes)iI).getName(), L"/", bFirst);
			bFirst = false;
		}
	}

	return szBuffer;
}


int CvTeam::getNumMembers() const
{
	return m_iNumMembers;
}


void CvTeam::changeNumMembers(int iChange)
{
	m_iNumMembers += iChange;
	FAssert(getNumMembers() >= 0);
}


int CvTeam::getAliveCount() const
{
	return m_iAliveCount;
}


int CvTeam::isAlive() const
{
	return (getAliveCount() > 0);
}


void CvTeam::changeAliveCount(int iChange)
{
	m_iAliveCount += iChange;
	FAssert(getAliveCount() >= 0);
}


void CvTeam::addAliveMember(PlayerTypes ePlayer)
{
	changeAliveCount(1);
}


void CvTeam::removeAliveMember(PlayerTypes ePlayer)
{
	changeAliveCount(-1);

	if (!isAlive())
	{
		kill();
	}
}


int CvTeam::getEverAliveCount() const
{
	return m_iEverAliveCount;
}


int CvTeam::isEverAlive() const
{
	return (getEverAliveCount() > 0);
}


void CvTeam::changeEverAliveCount(int iChange)
{
	m_iEverAliveCount += iChange;
	FAssert(getEverAliveCount() >= 0);
}


void CvTeam::addEverAliveMember(PlayerTypes ePlayer)
{
	changeEverAliveCount(1);
}


void CvTeam::kill()
{
	makePeaceWithAll();

	if (!GC.getGameINLINE().isOption(GAMEOPTION_USE_OLD_FOUNDING_FATHER_SYSTEM))
	{
		freeFathers();
	}
}


void CvTeam::freeFathers()
{
	FAssert(!GC.getGameINLINE().isOption(GAMEOPTION_USE_OLD_FOUNDING_FATHER_SYSTEM));

	for (int iFather = 0; iFather < GC.getNumFatherInfos(); ++iFather)
	{
		FatherTypes eFather = (FatherTypes) iFather;
		if (GC.getGameINLINE().getFatherTeam(eFather) == getID())
		{
			GC.getGameINLINE().setFatherTeam(eFather, NO_TEAM);
			for (TeamTypes eTeam = FIRST_TEAM; eTeam < NUM_TEAM_TYPES; ++eTeam)
			{
				// notify all teams that this FF is available again
				GET_TEAM(eTeam).notifyFatherAvailability(eFather, true);
			}
		}
	}
}


void CvTeam::makePeaceWithAll()
{
	for (TeamTypes eTeam = FIRST_TEAM; eTeam < NUM_TEAM_TYPES; ++eTeam)
	{
		if (isAtWar(eTeam) && canChangeWarPeace(eTeam))
		{
			makePeace(eTeam);
		}
	}
}


void CvTeam::notifyFatherAvailability(FatherTypes eFather, bool bAvailability)
{
	if (bAvailability && isFatherIgnore(eFather))
	{
		// If we are notified that this FF is available again, reset our ignore status
		setFatherIgnore(eFather, false);
	}
}


int CvTeam::getNumCities() const
{
	return m_iNumCities;
}


void CvTeam::changeNumCities(int iChange)
{
	m_iNumCities += iChange;
	FAssert(getNumCities() >= 0);
}


int CvTeam::getTotalPopulation() const
{
	int iPopulation = 0;
	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) i);
		if (kPlayer.isAlive() && kPlayer.getTeam() == getID())
		{
			iPopulation += kPlayer.getTotalPopulation();
		}
	}

	return iPopulation;
}


int CvTeam::getTotalLand() const
{
	return m_iTotalLand;
}


void CvTeam::changeTotalLand(int iChange)
{
	m_iTotalLand += iChange;
	FAssert(getTotalLand() >= 0);
}

int CvTeam::getMapTradingCount() const
{
	return m_iMapTradingCount;
}


bool CvTeam::isMapTrading()	const
{
	return (getMapTradingCount() > 0);
}


void CvTeam::changeMapTradingCount(int iChange)
{
	m_iMapTradingCount = (m_iMapTradingCount + iChange);
	FAssert(getMapTradingCount() >= 0);
}

int CvTeam::getGoldTradingCount() const
{
	return m_iGoldTradingCount;
}


bool CvTeam::isGoldTrading() const
{
	return (getGoldTradingCount() > 0);
}


void CvTeam::changeGoldTradingCount(int iChange)
{
	m_iGoldTradingCount += iChange;
	FAssert(getGoldTradingCount() >= 0);
}


int CvTeam::getOpenBordersTradingCount() const
{
	return m_iOpenBordersTradingCount;
}


bool CvTeam::isOpenBordersTrading() const
{
	return (getOpenBordersTradingCount() > 0);
}


void CvTeam::changeOpenBordersTradingCount(int iChange)
{
	m_iOpenBordersTradingCount += iChange;
	FAssert(getOpenBordersTradingCount() >= 0);
}


int CvTeam::getDefensivePactTradingCount() const
{
	return m_iDefensivePactTradingCount;
}


bool CvTeam::isDefensivePactTrading() const
{
	return (getDefensivePactTradingCount() > 0);
}


void CvTeam::changeDefensivePactTradingCount(int iChange)
{
	m_iDefensivePactTradingCount = (m_iDefensivePactTradingCount + iChange);
	FAssert(getDefensivePactTradingCount() >= 0);
}


int CvTeam::getPermanentAllianceTradingCount() const
{
	return m_iPermanentAllianceTradingCount;
}


bool CvTeam::isPermanentAllianceTrading() const
{
	if (!(GC.getGameINLINE().isOption(GAMEOPTION_PERMANENT_ALLIANCES)))
	{
		return false;
	}

	return (getPermanentAllianceTradingCount() > 0);
}


void CvTeam::changePermanentAllianceTradingCount(int iChange)
{
	m_iPermanentAllianceTradingCount = (m_iPermanentAllianceTradingCount + iChange);
	FAssert(getPermanentAllianceTradingCount() >= 0);
}

bool CvTeam::isMapCentering() const
{
	return m_bMapCentering;
}

void CvTeam::setMapCentering(bool bNewValue)
{
	if (isMapCentering() != bNewValue)
	{
		m_bMapCentering = bNewValue;

		if (getID() == GC.getGameINLINE().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(MinimapSection_DIRTY_BIT, true);
		}
	}
}

TeamTypes CvTeam::getID() const
{
	return m_eID;
}

bool CvTeam::isHasMet(TeamTypes eIndex)	const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	//FAssert((eIndex != getID()) || m_em_bHasMet[eIndex]);
	return m_em_bHasMet.get(eIndex);
}

void CvTeam::makeHasMet(TeamTypes eIndex, bool bNewDiplo)
{
	CvDiploParameters* pDiplo;
	int iI;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (!isHasMet(eIndex))
	{
		m_em_bHasMet.set(eIndex, true);

		AI_setAtPeaceCounter(eIndex, 0);
		AI_setAtWarCounter(eIndex, 0);

		if (GET_TEAM(eIndex).isHuman())
		{
			for (iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
					{
						if (!(GET_PLAYER((PlayerTypes)iI).isHuman()))
						{
							GET_PLAYER((PlayerTypes)iI).AI_makeProductionDirty();
						}
					}
				}
			}
		}

		if ((getID() == GC.getGameINLINE().getActiveTeam()) || (eIndex == GC.getGameINLINE().getActiveTeam()))
		{
			gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
		}

		if (GC.getGameINLINE().isOption(GAMEOPTION_ALWAYS_WAR))
		{
			if (isHuman() && getID() != eIndex && canDeclareWar(eIndex))
			{
				declareWar(eIndex, false, NO_WARPLAN);
			}
		}
		else
		{
			if (GC.getGameINLINE().isFinalInitialized() && !(gDLL->GetWorldBuilderMode()))
			{
				if (bNewDiplo)
				{
					if (!isHuman())
					{
						if (!isAtWar(eIndex))
						{
							for (iI = 0; iI < MAX_PLAYERS; iI++)
							{
								if (GET_PLAYER((PlayerTypes)iI).isAlive())
								{
									if (GET_PLAYER((PlayerTypes)iI).getTeam() == eIndex)
									{
										if (GET_PLAYER(getLeaderID()).canContact((PlayerTypes)iI))
										{
											if (GET_PLAYER((PlayerTypes)iI).isHuman())
											{
												pDiplo = new CvDiploParameters(getLeaderID());
												FAssertMsg(pDiplo != NULL, "pDiplo must be valid");
												pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_FIRST_CONTACT"));
												pDiplo->setAIContact(true);
												gDLL->beginDiplomacy(pDiplo, ((PlayerTypes)iI));
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

		if (GET_TEAM(eIndex).hasNativePlayer())
		{
			for (int i = 0; i < GC.getNumFatherPointInfos(); ++i)
			{
				FatherPointTypes ePointType = (FatherPointTypes) i;
				int gameSpeedMod =  GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
				changeFatherPoints(ePointType, GC.getFatherPointInfo(ePointType).getMeetNativePoints() * gameSpeedMod * getBestFatherPointMultiplier() / 10000);
			}
		}

		// report event to Python, along with some other key state
		gDLL->getEventReporterIFace()->firstContact(getID(), eIndex);
	}
}


bool CvTeam::isAtWar(TeamTypes eIndex) const
{
	if (eIndex == UNKNOWN_TEAM)
	{
		return true;
	}
	else if (eIndex == NO_TEAM)
	{
		return false;
	}
	else
	{
		return m_em_bAtWar.get(eIndex);
	}
}


void CvTeam::setAtWar(TeamTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (bNewValue)
	{
		AI_setOpenBordersCounter(eIndex, 0);
	}

	m_em_bAtWar.set(eIndex, bNewValue);
}


bool CvTeam::isPermanentWarPeace(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_bPermanentWarPeace.get(eIndex);
}


void CvTeam::setPermanentWarPeace(TeamTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_em_bPermanentWarPeace.set(eIndex, bNewValue);
}


bool CvTeam::isOpenBorders(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (isAtWar(eIndex))
	{
		return false;
	}

	return m_em_bOpenBorders.get(eIndex);
}


void CvTeam::setOpenBorders(TeamTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (isOpenBorders(eIndex) != bNewValue)
	{
		m_em_bOpenBorders.set(eIndex, bNewValue);

		AI_setOpenBordersCounter(eIndex, 0);

		GC.getMapINLINE().verifyUnitValidPlot();

		if ((getID() == GC.getGameINLINE().getActiveTeam()) || (eIndex == GC.getGameINLINE().getActiveTeam()))
		{
			gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
		}
	}

	FAssert(isOpenBorders(eIndex) == bNewValue);
}


bool CvTeam::isDefensivePact(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_bDefensivePact.get(eIndex);
}


void CvTeam::setDefensivePact(TeamTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (isDefensivePact(eIndex) != bNewValue)
	{
		m_em_bDefensivePact.set(eIndex, bNewValue);

		if ((getID() == GC.getGameINLINE().getActiveTeam()) || (eIndex == GC.getGameINLINE().getActiveTeam()))
		{
			gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
		}

		if (bNewValue && !GET_TEAM(eIndex).isDefensivePact(getID()))
		{
			CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_PLAYERS_SIGN_DEFENSIVE_PACT", getName().GetCString(), GET_TEAM(eIndex).getName().GetCString());
			GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), szBuffer, -1, -1, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
		}
	}
}


bool CvTeam::isForcePeace(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_bForcePeace.get(eIndex);
}


void CvTeam::setForcePeace(TeamTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_em_bForcePeace.set(eIndex, bNewValue);

	if (isForcePeace(eIndex))
	{
		if (AI_isSneakAttackPreparing(eIndex))
		{
			AI_setWarPlan(eIndex, NO_WARPLAN);
		}
	}
}

int CvTeam::getUnitClassCount(UnitClassTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_iUnitClassCount.get(eIndex);
}

void CvTeam::changeUnitClassCount(UnitClassTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_em_iUnitClassCount.add(eIndex, iChange);
	FAssert(getUnitClassCount(eIndex) >= 0);
}

int CvTeam::getBuildingClassCount(BuildingClassTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_iBuildingClassCount.get(eIndex);
}

void CvTeam::changeBuildingClassCount(BuildingClassTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_em_iBuildingClassCount.add(eIndex, iChange);
	FAssert(getBuildingClassCount(eIndex) >= 0);
}

int CvTeam::getUnitsPurchasedHistory(UnitClassTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_em_iEuropeUnitsPurchased.get(eIndex);
}

void CvTeam::changeUnitsPurchasedHistory(UnitClassTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_em_iEuropeUnitsPurchased.add(eIndex, iChange);
	FAssert(getUnitsPurchasedHistory(eIndex) >= 0);
}

// Protected Functions...
void CvTeam::testFoundingFather()
{
	bool bFound = false;
	for (int iFather = 0; iFather < GC.getNumFatherInfos() && !bFound; ++iFather)
	{
		FatherTypes eFather = (FatherTypes) iFather;
		if (canConvinceFather(eFather))
		{
			bFound = true;
			if (isHuman())
			{
				for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
				{
					CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayer);
					if (kPlayer.isAlive() && kPlayer.getTeam() == getID())
					{
						if (kPlayer.isHuman())
						{
							CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_FOUNDING_FATHER, eFather);
							gDLL->getInterfaceIFace()->addPopup(pInfo, (PlayerTypes) iPlayer);
						}
					}
				}
			}
			else //AI
			{
				convinceFather(eFather, true);
			}
		}
	}
}

void CvTeam::cancelDefensivePacts(TeamTypes eEndingTeam)
{
	CLLNode<TradeData>* pNode;
	CvDeal* pLoopDeal;
	bool bCancelDeal;
	int iLoop;

	for (pLoopDeal = GC.getGameINLINE().firstDeal(&iLoop); pLoopDeal != NULL; pLoopDeal = GC.getGameINLINE().nextDeal(&iLoop))
	{
		bCancelDeal = false;

		if ((GET_PLAYER(pLoopDeal->getFirstPlayer()).getTeam() == getID()) ||
			(GET_PLAYER(pLoopDeal->getSecondPlayer()).getTeam() == getID()))
		{
			for (pNode = pLoopDeal->headFirstTradesNode(); (pNode != NULL); pNode = pLoopDeal->nextFirstTradesNode(pNode))
			{
				if (pNode->m_data.m_eItemType == TRADE_DEFENSIVE_PACT)
				{
					bCancelDeal = true;
					break;
				}
			}

			if (!bCancelDeal)
			{
				for (pNode = pLoopDeal->headSecondTradesNode(); (pNode != NULL); pNode = pLoopDeal->nextSecondTradesNode(pNode))
				{
					if (pNode->m_data.m_eItemType == TRADE_DEFENSIVE_PACT)
					{
						bCancelDeal = true;
						break;
					}
				}
			}
		}

		if (bCancelDeal)
		{
			pLoopDeal->kill(true, eEndingTeam);
		}
	}
}


bool CvTeam::isFriendlyTerritory(TeamTypes eTeam) const
{
	if (eTeam == NO_TEAM)
	{
		return false;
	}

	if (eTeam == getID())
	{
		return true;
	}

	return false;
}

void CvTeam::setForceRevealedBonus(BonusTypes eBonus, bool bRevealed)
{
	if (isForceRevealedBonus(eBonus) == bRevealed)
	{
		return;
	}

	if (bRevealed)
	{
		m_aeRevealedBonuses.push_back(eBonus);
	}
	else
	{
		std::vector<BonusTypes>::iterator it;

		for (it = m_aeRevealedBonuses.begin(); it != m_aeRevealedBonuses.end(); ++it)
		{
			if (*it == eBonus)
			{
				m_aeRevealedBonuses.erase(it);
				break;
			}
		}
	}

	for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); ++iI)
	{
		CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

		if (pLoopPlot->getBonusType() == eBonus)
		{
			pLoopPlot->updateYield(true);
			pLoopPlot->setLayoutDirty(true);
		}
	}
}

bool CvTeam::isForceRevealedBonus(BonusTypes eBonus) const
{
	std::vector<BonusTypes>::const_iterator it;

	for (it = m_aeRevealedBonuses.begin(); it != m_aeRevealedBonuses.end(); ++it)
	{
		if (*it == eBonus)
		{
			return true;
		}
	}

	return false;
}

int CvTeam::countNumHumanGameTurnActive() const
{
	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);

		if (kLoopPlayer.isHuman() && kLoopPlayer.getTeam() == getID())
		{
			if (kLoopPlayer.isTurnActive())
			{
				++iCount;
			}
		}
	}

	return iCount;
}

void CvTeam::setTurnActive(bool bNewValue, bool bDoTurn)
{
	FAssert(GC.getGameINLINE().isSimultaneousTeamTurns());

	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (kPlayer.isAlive() && kPlayer.getTeam() == getID())
		{
			kPlayer.setTurnActive(bNewValue, bDoTurn);
		}
	}
}

bool CvTeam::isTurnActive() const
{
	FAssert(GC.getGameINLINE().isSimultaneousTeamTurns());

	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (kPlayer.getTeam() == getID())
		{
			if (kPlayer.isTurnActive())
			{
				return true;
			}
		}
	}

	return false;
}

int CvTeam::getRebelPercent() const
{
	PROFILE_FUNC();

	int iRebelSentiment = 0;

	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayer);
		if (kPlayer.isAlive() && kPlayer.getTeam() == getID() && kPlayer.getParent() != NO_PLAYER && GET_PLAYER(kPlayer.getParent()).isAlive())
		{
			int iLoop;
			for (CvCity* pCity = kPlayer.firstCity(&iLoop); pCity != NULL; pCity = kPlayer.nextCity(&iLoop))
			{
				iRebelSentiment += pCity->getRebelSentiment();
			}
		}
	}

	return std::max(0, std::min(100, (iRebelSentiment / std::max(1, getTotalPopulation()))));
}

bool CvTeam::canDoRevolution() const
{
	if (GC.getGameINLINE().getGameState() != GAMESTATE_ON)
	{
		return false;
	}

	bool bRevolution = false;
	for (int iVictory = 0; iVictory < GC.getNumVictoryInfos() && !bRevolution; ++iVictory)
	{
		if (GC.getGameINLINE().isVictoryValid((VictoryTypes) iVictory))
		{
			CvVictoryInfo& kVictory = GC.getVictoryInfo((VictoryTypes) iVictory);
			if (GC.getVictoryInfo((VictoryTypes) iVictory).isRevolution())
			{
				bRevolution = true;
			}
		}
	}

	if (!bRevolution)
	{
		return false;
	}

	if (isInRevolution())
	{
		return false;
	}

	if (getRebelPercent() < GC.getDefineINT("REBEL_PERCENT_FOR_REVOLUTION"))
	{
		return false;
	}

	return true;
}

bool CvTeam::isInRevolution() const
{
	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) i);
		if (kPlayer.isAlive() && kPlayer.getTeam() == getID() && kPlayer.isInRevolution())
		{
			return true;
		}
	}

	return false;
}

void CvTeam::doRevolution()
{
	if (!canDoRevolution())
	{
		return;
	}

	CvGame& kGame = GC.getGameINLINE();
	if (!kGame.isMaxTurnsExtended())
	{
		int iRevolutionTurns = GC.getGameSpeedInfo(kGame.getGameSpeedType()).getRevolutionTurns();
		if (iRevolutionTurns + kGame.getElapsedGameTurns() > kGame.getMaxTurns())
		{
			kGame.setMaxTurns(iRevolutionTurns + kGame.getElapsedGameTurns());
			kGame.setEstimateEndTurn(iRevolutionTurns + kGame.getGameTurn());
			kGame.setMaxTurnsExtended(true);
		}
	}

	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kTeamPlayer = GET_PLAYER((PlayerTypes) iPlayer);

		if (kTeamPlayer.isAlive() && kTeamPlayer.getTeam() == getID())
		{
			if (kTeamPlayer.getParent() != NO_PLAYER)
			{
				CvPlayerAI& kParent = GET_PLAYER(kTeamPlayer.getParent());

				GET_TEAM(kParent.getTeam()).declareWarNoRevolution(getID(), true, WARPLAN_TOTAL, false);

				//create REF in Europe
				for (int i = 0; i < kTeamPlayer.getNumRevolutionEuropeUnits(); ++i)
				{
					CvUnit* pRevolutionUnit = NULL;
					if (GC.getUnitInfo(kTeamPlayer.getRevolutionEuropeUnit(i)).getDomainType() == DOMAIN_SEA)
					{
						CvPlot* pOceanPlot = kParent.AI_getImperialShipSpawnPlot();
						if (pOceanPlot != NULL)
						{
							pRevolutionUnit = kParent.initUnit(kTeamPlayer.getRevolutionEuropeUnit(i), kTeamPlayer.getRevolutionEuropeProfession(i), pOceanPlot->getX_INLINE(), pOceanPlot->getY_INLINE());
							pRevolutionUnit->setUnitTravelState(UNIT_TRAVEL_STATE_IN_EUROPE, false);
						}
					}
					else
					{
						pRevolutionUnit = kParent.initEuropeUnit(kTeamPlayer.getRevolutionEuropeUnit(i));
						pRevolutionUnit->setProfession(kTeamPlayer.getRevolutionEuropeProfession(i));
					}
					FAssert(pRevolutionUnit != NULL);
				}
				kTeamPlayer.clearRevolutionEuropeUnits();
				kTeamPlayer.setYieldEuropeTradableAll();
				kTeamPlayer.doEra();
				kTeamPlayer.validateTradeRoutes();
			}
		}
	}

	if (GC.getGameINLINE().getActiveTeam() == getID())
	{
		gDLL->getInterfaceIFace()->setDirty(ColoredPlots_DIRTY_BIT, true);
	}

	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayer);
		if (kPlayer.isAlive() && kPlayer.getTeam() == getID())
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_MOVIE);
			pInfo->setText(CvWString("ART_DEF_MOVIE_REVOLUTION"));
			gDLL->getInterfaceIFace()->addPopup(pInfo, (PlayerTypes) iPlayer);

			for (int iCivicOption = 0; iCivicOption < GC.getNumCivicOptionInfos(); ++iCivicOption)
			{
				if (kPlayer.isHuman())
				{
					CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CIVIC_OPTION, iCivicOption);
					gDLL->getInterfaceIFace()->addPopup(pInfo, (PlayerTypes) iPlayer);
				}
				else
				{
					kPlayer.AI_chooseCivic((CivicOptionTypes) iCivicOption);
				}
			}
			//TAC Revolution Support
			kPlayer.checkForRevolutionSupport();
			//End TAC Revolution Support
		}
	}
}

bool CvTeam::isParentOf(TeamTypes eChildTeam) const
{
	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		CvPlayer& kChildPlayer = GET_PLAYER((PlayerTypes) i);
        if (kChildPlayer.isEverAlive() && (kChildPlayer.getTeam() == eChildTeam))
		{
			PlayerTypes eParent = kChildPlayer.getParent();
			if (eParent != NO_PLAYER)
			{
				CvPlayer& kParentPlayer = GET_PLAYER(eParent);
				if (kParentPlayer.getTeam() == getID())
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool CvTeam::checkIndependence() const
{
	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) i);
		if (kPlayer.getTeam() == getID() && kPlayer.isAlive())
		{
			// One player on the team is enough for team independence
			if (kPlayer.checkIndependence())
			{
				return true;
			}
		}
	}

	return false;
}

// CACHE: cache frequently used values
///////////////////////////////////////

// PatchMod: Victorys START
int CvTeam::getTotalProductionRate() const
{
	int iProductionRate = 0;
	int iPlayerProduction = 0;
	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) i);
		if (kPlayer.isAlive() && kPlayer.getTeam() == getID() && !kPlayer.isEurope())
		{
			iPlayerProduction = 0;
			for (int iK = 0; iK < NUM_YIELD_TYPES; iK++)
			{
				iPlayerProduction += kPlayer.getYieldRate((YieldTypes)iK);
			}
			if (!kPlayer.isHuman())
			{
				iPlayerProduction *= GC.getHandicapInfo(getHandicapType()).getAIAdvancedStartPercent();
				iPlayerProduction /= 100;
				iPlayerProduction *= 3;
			}
			iProductionRate += iPlayerProduction;
		}
	}

	return iProductionRate;
}
// PatchMod: Victorys END

void CvTeam::read(FDataStreamBase* pStream)
{
	CvSavegameReaderBase readerbase(pStream);
	CvSavegameReader reader(readerbase);

	read(reader);
}

void CvTeam::write(FDataStreamBase* pStream)
{
	CvSavegameWriterBase writerbase(pStream);
	CvSavegameWriter writer(writerbase);
	write(writer);
	writerbase.WriteFile();
}
