// playerAI.cpp

#include "CvGameCoreDLL.h"
#include "CvPlayerAI.h"
#include "CvRandom.h"
#include "CvGlobals.h"
#include "CvGameCoreUtils.h"
#include "CvMap.h"
#include "CvArea.h"
#include "CvPlot.h"
#include "CvGameAI.h"
#include "CvTeamAI.h"
#include "CvGameCoreUtils.h"
#include "CvDiploParameters.h"
#include "CvInitCore.h"
#include "CyArgsList.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLEntityIFaceBase.h"
#include "CvDLLPythonIFaceBase.h"
#include "CvDLLEngineIFaceBase.h"
#include "CvDLLEventReporterIFaceBase.h"
#include "CvInfos.h"
#include "CvPopupInfo.h"
#include "FProfiler.h"
#include "CvDLLFAStarIFaceBase.h"
#include "FAStarNode.h"
#include "CvTradeRoute.h"

#define DANGER_RANGE				(4)
#define GREATER_FOUND_RANGE			(5)
#define CIVIC_CHANGE_DELAY			(25)

// statics

CvPlayerAI* CvPlayerAI::m_aPlayers = NULL;

void CvPlayerAI::initStatics()
{
	m_aPlayers = new CvPlayerAI[MAX_PLAYERS];
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_aPlayers[iI].m_eID = ((PlayerTypes)iI);
	}
}

void CvPlayerAI::freeStatics()
{
	SAFE_DELETE_ARRAY(m_aPlayers);
}

bool CvPlayerAI::areStaticsInitialized()
{
	if(m_aPlayers == NULL)
	{
		return false;
	}

	return true;
}

DllExport CvPlayerAI& CvPlayerAI::getPlayerNonInl(PlayerTypes ePlayer)
{
	return getPlayer(ePlayer);
}

// Public Functions...

CvPlayerAI::CvPlayerAI()
{
	m_aiNumTrainAIUnits = new int[NUM_UNITAI_TYPES];
	m_aiNumAIUnits = new int[NUM_UNITAI_TYPES];
	m_aiNumRetiredAIUnits = new int[NUM_UNITAI_TYPES];
	m_aiUnitAIStrategyWeights = new int[NUM_UNITAI_TYPES];
	m_aiPeacetimeTradeValue = new int[MAX_PLAYERS];
	m_aiPeacetimeGrantValue = new int[MAX_PLAYERS];
	m_aiGoldTradedTo = new int[MAX_PLAYERS];
	m_aiAttitudeExtra = new int[MAX_PLAYERS];

	m_abFirstContact = new bool[MAX_PLAYERS];

	m_aaiContactTimer = new int*[MAX_PLAYERS];
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		m_aaiContactTimer[i] = new int[NUM_CONTACT_TYPES];
	}

	m_aaiMemoryCount = new int*[MAX_PLAYERS];
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		m_aaiMemoryCount[i] = new int[NUM_MEMORY_TYPES];
	}

	m_aiAverageYieldMultiplier = new int[NUM_YIELD_TYPES];

	m_aiUnitClassWeights = NULL;
	m_aiUnitCombatWeights = NULL;
	m_aiEmotions = new int[NUM_EMOTION_TYPES];
	m_aiStrategyStartedTurn = new int[NUM_STRATEGY_TYPES];
	m_aiStrategyData = new int[NUM_STRATEGY_TYPES];

	m_aiBestWorkedYieldPlots = new int[NUM_YIELD_TYPES];
	m_aiBestUnworkedYieldPlots = new int[NUM_YIELD_TYPES];
	m_aiYieldValuesTimes100 = new int[NUM_YIELD_TYPES];

	m_aiCloseBordersAttitudeCache = new int[MAX_PLAYERS];
	m_aiStolenPlotsAttitudeCache = new int[MAX_PLAYERS];

	AI_reset();
}


CvPlayerAI::~CvPlayerAI()
{
	AI_uninit();

	SAFE_DELETE_ARRAY(m_aiNumTrainAIUnits);
	SAFE_DELETE_ARRAY(m_aiNumAIUnits);
	SAFE_DELETE_ARRAY(m_aiNumRetiredAIUnits);
	SAFE_DELETE_ARRAY(m_aiUnitAIStrategyWeights);
	SAFE_DELETE_ARRAY(m_aiPeacetimeTradeValue);
	SAFE_DELETE_ARRAY(m_aiPeacetimeGrantValue);
	SAFE_DELETE_ARRAY(m_aiGoldTradedTo);
	SAFE_DELETE_ARRAY(m_aiAttitudeExtra);
	SAFE_DELETE_ARRAY(m_abFirstContact);
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		SAFE_DELETE_ARRAY(m_aaiContactTimer[i]);
	}
	SAFE_DELETE_ARRAY(m_aaiContactTimer);

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		SAFE_DELETE_ARRAY(m_aaiMemoryCount[i]);
	}
	SAFE_DELETE_ARRAY(m_aaiMemoryCount);

	SAFE_DELETE_ARRAY(m_aiAverageYieldMultiplier);
	SAFE_DELETE_ARRAY(m_aiCloseBordersAttitudeCache);
	SAFE_DELETE_ARRAY(m_aiStolenPlotsAttitudeCache);
	SAFE_DELETE_ARRAY(m_aiEmotions);
	SAFE_DELETE_ARRAY(m_aiStrategyStartedTurn);
	SAFE_DELETE_ARRAY(m_aiStrategyData);

	SAFE_DELETE_ARRAY(m_aiBestWorkedYieldPlots);
	SAFE_DELETE_ARRAY(m_aiBestUnworkedYieldPlots);
	SAFE_DELETE_ARRAY(m_aiYieldValuesTimes100);
	m_aTradeGroups.clear();
}


void CvPlayerAI::AI_init()
{
	AI_reset();

	//--------------------------------
	// Init other game data
	if ((GC.getInitCore().getSlotStatus(getID()) == SS_TAKEN) || (GC.getInitCore().getSlotStatus(getID()) == SS_COMPUTER))
	{
		FAssert(getPersonalityType() != NO_LEADER);
	}
}


void CvPlayerAI::AI_uninit()
{
	SAFE_DELETE_ARRAY(m_aiUnitClassWeights);
	SAFE_DELETE_ARRAY(m_aiUnitCombatWeights);
}


void CvPlayerAI::AI_reset()
{
	int iI;

	AI_uninit();

	m_iAttackOddsChange = 0;
	m_iExtraGoldTarget = 0;

	m_eNextBuyUnit = NO_UNIT;
	m_eNextBuyUnitAI = NO_UNITAI;
	m_iNextBuyUnitValue = 0;

	m_eNextBuyProfession = NO_PROFESSION;
	m_eNextBuyProfessionUnit = NO_UNIT;
	m_eNextBuyProfessionAI = NO_UNITAI;
	m_iNextBuyProfessionValue = 0;

	m_iTotalIncome = 0;
	m_iHurrySpending = 0;
	// TAC - AI More Immigrants - koma13 - START
	m_iImmigrantSpending = 0;
	// TAC - AI More Immigrants - koma13 - END
	m_iEuropeYieldSpending = 0;	// TAC - AI Economy - koma13
	for (iI = 0; iI < NUM_UNITAI_TYPES; iI++)
	{
		m_aiNumTrainAIUnits[iI] = 0;
		m_aiNumAIUnits[iI] = 0;
		m_aiNumRetiredAIUnits[iI] = 0;
		m_aiUnitAIStrategyWeights[iI] = 0;
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_aiPeacetimeTradeValue[iI] = 0;
		m_aiPeacetimeGrantValue[iI] = 0;
		m_aiGoldTradedTo[iI] = 0;
		m_aiAttitudeExtra[iI] = 0;
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_abFirstContact[iI] = false;
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		for (int iJ = 0; iJ < NUM_CONTACT_TYPES; iJ++)
		{
			m_aaiContactTimer[iI][iJ] = 0;
		}
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		for (int iJ = 0; iJ < NUM_MEMORY_TYPES; iJ++)
		{
			m_aaiMemoryCount[iI][iJ] = 0;
		}
	}

	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_aiAverageYieldMultiplier[iI] = 0;
		m_aiBestWorkedYieldPlots[iI] = -1;
		m_aiBestUnworkedYieldPlots[iI] = -1;
		m_aiYieldValuesTimes100[iI] = 0;
	}
	m_iAveragesCacheTurn = -1;

	m_iTurnLastProductionDirty = -1;
	m_iTurnLastManagedPop = -1;
	m_iMoveQueuePasses = 0;

	m_iUpgradeUnitsCacheTurn = -1;
	m_iUpgradeUnitsCachedExpThreshold = 0;
	m_iUpgradeUnitsCachedGold = 0;

	// TAC - AI Revolution - koma13 - START
	m_iLastWave = -1;
	m_iWaveIndex = -1;
	// TAC - AI Revolution - koma13 - END

	m_aiAICitySites.clear();

	FAssert(m_aiUnitClassWeights == NULL);
	m_aiUnitClassWeights = new int[GC.getNumUnitClassInfos()];
	for (iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		m_aiUnitClassWeights[iI] = 0;
	}

	FAssert(m_aiUnitCombatWeights == NULL);
	m_aiUnitCombatWeights = new int[GC.getNumUnitCombatInfos()];
	for (iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
	{
		m_aiUnitCombatWeights[iI] = 0;
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_aiCloseBordersAttitudeCache[iI] = 0;
		m_aiStolenPlotsAttitudeCache[iI] = 0;
	}

	for (iI = 0; iI < NUM_EMOTION_TYPES; iI++)
	{
		m_aiEmotions[iI] = 0;
	}

	for (iI = 0; iI < NUM_STRATEGY_TYPES; iI++)
	{
		m_aiStrategyStartedTurn[iI] = -1;
		m_aiStrategyData[iI] = -1;
	}

	m_iDistanceMapDistance = -1;
	m_distanceMap.clear();
	m_unitPriorityHeap.clear();
}

void CvPlayerAI::AI_doTurnPre()
{
	PROFILE_FUNC();

	FAssertMsg(getPersonalityType() != NO_LEADER, "getPersonalityType() is not expected to be equal with NO_LEADER");
	FAssertMsg(getLeaderType() != NO_LEADER, "getLeaderType() is not expected to be equal with NO_LEADER");
	FAssertMsg(getCivilizationType() != NO_CIVILIZATION, "getCivilizationType() is not expected to be equal with NO_CIVILIZATION");

	AI_invalidateCloseBordersAttitudeCache();

	AI_doCounter();

	AI_doEnemyUnitData();

	if (isHuman())
	{
		return;
	}

	m_unitPriorityHeap.clear();
	int iLoop;
	for (CvUnit* pUnit = firstUnit(&iLoop); pUnit != NULL; pUnit = nextUnit(&iLoop))
	{
		pUnit->AI_setMovePriority(0);
	}

	AI_doEmotions();

	AI_doUnitAIWeights();

	AI_doMilitary();

	AI_doStrategy();

	AI_updateYieldValues();
}


void CvPlayerAI::AI_doTurnPost()
{
	PROFILE_FUNC();

	if (isHuman())
	{
		return;
	}

	AI_doTradeRoutes();

	AI_doDiplo();
}


void CvPlayerAI::AI_doTurnUnitsPre()
{
	PROFILE_FUNC();
	AI_updateBestYieldPlots();
	AI_updateFoundValues();
	AI_doEmotions();

	if (!isHuman())
	{
		if (getParent() != NO_PLAYER)
		{
			AI_doProfessions();
			AI_doEurope();
		}
	}

	if (GC.getGameINLINE().getSorenRandNum(8, "AI Update Area Targets") == 0) // XXX personality???
	{
		AI_updateAreaTargets();
	}

	if (!isHuman())
	{
		AI_doMilitaryStrategy();
		AI_doSuppressRevolution();
	}

	if (isHuman())
	{
		return;
	}
}


void CvPlayerAI::AI_doTurnUnitsPost()
{
	PROFILE_FUNC();

	CvUnit* pLoopUnit;
	int iLoop;
	if (!isHuman() || isOption(PLAYEROPTION_AUTO_PROMOTION))
	{
		for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
		{
			pLoopUnit->AI_promote();
		}
	}

	if (isHuman())
	{
		return;
	}
}


void CvPlayerAI::AI_doPeace()
{
	PROFILE_FUNC();

	CvDiploParameters* pDiplo;
	CvCity* pBestReceiveCity;
	CvCity* pBestGiveCity;
	CvCity* pLoopCity;
	CLinkList<TradeData> ourList;
	CLinkList<TradeData> theirList;
	bool abContacted[MAX_TEAMS];
	TradeData item;
	int iReceiveGold;
	int iGiveGold;
	int iGold;
	int iValue;
	int iBestValue;
	int iOurValue;
	int iTheirValue;
	int iLoop;
	int iI;
	FAssert(!isHuman());

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		abContacted[iI] = false;
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (iI != getID())
			{
				if (canContact((PlayerTypes)iI) && AI_isWillingToTalk((PlayerTypes)iI))
				{
					if (!(GET_TEAM(getTeam()).isHuman()) && (GET_PLAYER((PlayerTypes)iI).isHuman() || !(GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isHuman())))
					{
						if (GET_TEAM(getTeam()).isAtWar(GET_PLAYER((PlayerTypes)iI).getTeam()))
						{
							if (!(GET_PLAYER((PlayerTypes)iI).isHuman()) || (GET_TEAM(getTeam()).getLeaderID() == getID()))
							{
								FAssertMsg(iI != getID(), "iI is not expected to be equal with getID()");
								FAssert(GET_PLAYER((PlayerTypes)iI).getTeam() != getTeam());

								if (GET_TEAM(getTeam()).AI_getAtWarCounter(GET_PLAYER((PlayerTypes)iI).getTeam()) > 10)
								{
									if (AI_getContactTimer(((PlayerTypes)iI), CONTACT_PEACE_TREATY) == 0)
									{
										bool bOffered = false;
										if (!bOffered)
										{
											if (GC.getGameINLINE().getSorenRandNum(GC.getLeaderHeadInfo(getPersonalityType()).getContactRand(CONTACT_PEACE_TREATY), "AI Diplo Peace Treaty") == 0)
											{
												setTradeItem(&item, TRADE_PEACE_TREATY, 0, NULL);

												if (canTradeItem(((PlayerTypes)iI), item, true) && GET_PLAYER((PlayerTypes)iI).canTradeItem(getID(), item, true))
												{
													iOurValue = GET_TEAM(getTeam()).AI_endWarVal(GET_PLAYER((PlayerTypes)iI).getTeam());
													iTheirValue = GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).AI_endWarVal(getTeam());
													iReceiveGold = 0;
													iGiveGold = 0;

													pBestReceiveCity = NULL;
													pBestGiveCity = NULL;

													if (iTheirValue > iOurValue)
													{
														if (iTheirValue > iOurValue)
														{
															iBestValue = 0;
														}

														iGold = std::min((iTheirValue - iOurValue), GET_PLAYER((PlayerTypes)iI).AI_maxGoldTrade(getID()));

														if (iGold > 0)
														{
															setTradeItem(&item, TRADE_GOLD, iGold, NULL);

															if (GET_PLAYER((PlayerTypes)iI).canTradeItem(getID(), item, true))
															{
																iReceiveGold = iGold;
																iOurValue += iGold;
															}
														}

														if (iTheirValue > iOurValue)
														{
															iBestValue = 0;

															for (pLoopCity = GET_PLAYER((PlayerTypes)iI).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iI).nextCity(&iLoop))
															{
																setTradeItem(&item, TRADE_CITIES, pLoopCity->getID(), NULL);

																if (GET_PLAYER((PlayerTypes)iI).canTradeItem(getID(), item, true))
																{
																	iValue = pLoopCity->plot()->calculateCulturePercent(getID());

																	if (iValue > iBestValue)
																	{
																		iBestValue = iValue;
																		pBestReceiveCity = pLoopCity;
																	}
																}
															}

															if (pBestReceiveCity != NULL)
															{
																iOurValue += AI_cityTradeVal(pBestReceiveCity);
															}
														}
													}
													else if (iOurValue > iTheirValue)
													{
														iBestValue = 0;
														iGold = std::min((iOurValue - iTheirValue), AI_maxGoldTrade((PlayerTypes)iI));

														if (iGold > 0)
														{
															setTradeItem(&item, TRADE_GOLD, iGold, NULL);

															if (canTradeItem(((PlayerTypes)iI), item, true))
															{
																iGiveGold = iGold;
																iTheirValue += iGold;
															}
														}

														iBestValue = 0;

														for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
														{
															setTradeItem(&item, TRADE_CITIES, pLoopCity->getID(), NULL);

															if (canTradeItem(((PlayerTypes)iI), item, true))
															{
																if (GET_PLAYER((PlayerTypes)iI).AI_cityTradeVal(pLoopCity) <= (iOurValue - iTheirValue))
																{
																	iValue = pLoopCity->plot()->calculateCulturePercent((PlayerTypes)iI);

																	if (iValue > iBestValue)
																	{
																		iBestValue = iValue;
																		pBestGiveCity = pLoopCity;
																	}
																}
															}
														}

														if (pBestGiveCity != NULL)
														{
															iTheirValue += GET_PLAYER((PlayerTypes)iI).AI_cityTradeVal(pBestGiveCity);
														}
													}

													if ((GET_PLAYER((PlayerTypes)iI).isHuman()) ? (iOurValue >= iTheirValue) : ((iOurValue > ((iTheirValue * 3) / 5)) && (iTheirValue > ((iOurValue * 3) / 5))))
													{
														ourList.clear();
														theirList.clear();

														setTradeItem(&item, TRADE_PEACE_TREATY, 0, NULL);

														ourList.insertAtEnd(item);
														theirList.insertAtEnd(item);
														if (iGiveGold != 0)
														{
															setTradeItem(&item, TRADE_GOLD, iGiveGold, NULL);
															ourList.insertAtEnd(item);
														}

														if (iReceiveGold != 0)
														{
															setTradeItem(&item, TRADE_GOLD, iReceiveGold, NULL);
															theirList.insertAtEnd(item);
														}

														if (pBestGiveCity != NULL)
														{
															setTradeItem(&item, TRADE_CITIES, pBestGiveCity->getID(), NULL);
															ourList.insertAtEnd(item);
														}

														if (pBestReceiveCity != NULL)
														{
															setTradeItem(&item, TRADE_CITIES, pBestReceiveCity->getID(), NULL);
															theirList.insertAtEnd(item);
														}

														if (GET_PLAYER((PlayerTypes)iI).isHuman())
														{
															if (!(abContacted[GET_PLAYER((PlayerTypes)iI).getTeam()]))
															{
																AI_changeContactTimer(((PlayerTypes)iI), CONTACT_PEACE_TREATY, GC.getLeaderHeadInfo(getPersonalityType()).getContactDelay(CONTACT_PEACE_TREATY));
																pDiplo = new CvDiploParameters(getID());
																FAssertMsg(pDiplo != NULL, "pDiplo must be valid");
																pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_OFFER_PEACE"));
																pDiplo->setAIContact(true);
																pDiplo->setOurOfferList(theirList);
																pDiplo->setTheirOfferList(ourList);
																gDLL->beginDiplomacy(pDiplo, (PlayerTypes)iI);
																abContacted[GET_PLAYER((PlayerTypes)iI).getTeam()] = true;
															}
														}
														else
														{
															GC.getGameINLINE().implementDeal(getID(), ((PlayerTypes)iI), &ourList, &theirList);
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
				}
			}
		}
	}
}


void CvPlayerAI::AI_updateFoundValues(bool bStartingLoc)
{
	PROFILE_FUNC();

	int iLoop;
	for(CvArea* pLoopArea = GC.getMapINLINE().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMapINLINE().nextArea(&iLoop))
	{
		pLoopArea->setBestFoundValue(getID(), 0);
	}

	if (bStartingLoc)
	{
		for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
		{
			GC.getMapINLINE().plotByIndexINLINE(iI)->setFoundValue(getID(), -1);
		}
	}
	else
	{
		for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
		{
			CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

			int iValue = 0;
			if (pLoopPlot->isRevealed(getTeam(), false))
			{
				long lResult=-1;
				if(GC.getUSE_GET_CITY_FOUND_VALUE_CALLBACK())
				{
					CyArgsList argsList;
					argsList.add((int)getID());
					argsList.add(pLoopPlot->getX());
					argsList.add(pLoopPlot->getY());
					gDLL->getPythonIFace()->callFunction(PYGameModule, "getCityFoundValue", argsList.makeFunctionArgs(), &lResult);
				}

				if (lResult == -1)
				{
					iValue = AI_foundValue(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE());
				}
				else
				{
					iValue = lResult;
				}
			}

			pLoopPlot->setFoundValue(getID(), iValue);

			CvArea* pArea = pLoopPlot->area();
			if (iValue > pArea->getBestFoundValue(getID()))
			{
				pArea->setBestFoundValue(getID(), iValue);
			}
		}
	}
}


void CvPlayerAI::AI_updateAreaTargets()
{
	CvArea* pLoopArea;
	int iLoop;

	for(pLoopArea = GC.getMapINLINE().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMapINLINE().nextArea(&iLoop))
	{
		if (!(pLoopArea->isWater()))
		{
			if (GC.getGameINLINE().getSorenRandNum(3, "AI Target City") == 0)
			{
				pLoopArea->setTargetCity(getID(), NULL);
			}
			else
			{
				pLoopArea->setTargetCity(getID(), AI_findTargetCity(pLoopArea));
			}
		}
	}
}


// Returns priority for unit movement (lower values move first...)
int CvPlayerAI::AI_movementPriority(CvSelectionGroup* pGroup)
{
	CvUnit* pHeadUnit;
	int iCurrCombat;
	int iBestCombat;

	pHeadUnit = pGroup->getHeadUnit();

	if (pHeadUnit != NULL)
	{
		if (pHeadUnit->hasCargo())
		{
			if (pHeadUnit->specialCargo() == NO_SPECIALUNIT)
			{
				return 0;
			}
			else
			{
				return 1;
			}
		}

		if (pHeadUnit->AI_getUnitAIType() == UNITAI_SETTLER)
		{
			return 2;
		}

		if (pHeadUnit->AI_getUnitAIType() == UNITAI_WORKER)
		{
			return 3;
		}

		if (pHeadUnit->AI_getUnitAIType() == UNITAI_SCOUT)
		{
			return 4;
		}

		if (pHeadUnit->bombardRate() > 0)
		{
			return 5;
		}

		if (pHeadUnit->canFight())
		{
			if (pHeadUnit->withdrawalProbability() > 20)
			{
				return 7;
			}

			if (pHeadUnit->withdrawalProbability() > 0)
			{
				return 8;
			}

			iCurrCombat = pHeadUnit->currCombatStr(NULL, NULL);
			iBestCombat = (GC.getGameINLINE().getBestLandUnitCombat() * 100);

			if (pHeadUnit->noDefensiveBonus())
			{
				iCurrCombat *= 3;
				iCurrCombat /= 2;
			}

			if (pHeadUnit->AI_isCityAIType())
			{
				iCurrCombat /= 2;
			}

			if (iCurrCombat > iBestCombat)
			{
				return 9;
			}
			else if (iCurrCombat > ((iBestCombat * 4) / 5))
			{
				return 10;
			}
			else if (iCurrCombat > ((iBestCombat * 3) / 5))
			{
				return 11;
			}
			else if (iCurrCombat > ((iBestCombat * 2) / 5))
			{
				return 12;
			}
			else if (iCurrCombat > ((iBestCombat * 1) / 5))
			{
				return 13;
			}
			else
			{
				return 14;
			}
		}

		return 15;
	}

	return 16;
}

void CvPlayerAI::AI_unitUpdate()
{
	PROFILE_FUNC();

	CLLNode<int>* pCurrUnitNode;
	CvSelectionGroup* pLoopSelectionGroup;
	CLinkList<int> tempGroupCycle;
	CLinkList<int> finalGroupCycle;


	if (GC.getGameINLINE().getGameTurn() != m_iTurnLastManagedPop)
	{
		//This should only be done once a turn, but must be done right before
		//units are moved else it's unfair on the AI.
		if (!isHuman())
		{
			int iLoop;
			CvCity* pLoopCity;
			for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				if (isNative())
				{
					pLoopCity->AI_doNative();
				}
				else if (pLoopCity->getPopulation() > 1)
				{
					int iValue = std::max(1, 11 / (1 + pLoopCity->getPopulation()));
					if (((GC.getGameINLINE().getGameTurn() + iLoop) % iValue) == 0)
					{
						bool bRemove = true;
						CvUnit* pRemoveUnit = pLoopCity->getPopulationUnitByIndex(0);

						if (pRemoveUnit->getProfession() != NO_PROFESSION)
						{
							CvProfessionInfo& kProfession = GC.getProfessionInfo(pRemoveUnit->getProfession());
							// R&R, ray , MYCP partially based on code of Aymerick - START
							YieldTypes eYieldProducedType = (YieldTypes)kProfession.getYieldsProduced(0);
							// R&R, ray , MYCP partially based on code of Aymerick - END		

							if (eYieldProducedType == YIELD_EDUCATION)
							{
								bRemove = false;
							}

							if (bRemove && (pRemoveUnit->AI_getIdealProfession() != NO_PROFESSION) && (pRemoveUnit->getProfession() == pRemoveUnit->AI_getIdealProfession()))
							{
								bRemove = false;
								// R&R, ray , MYCP partially based on code of Aymerick - START
								YieldTypes eYieldProducedType = (YieldTypes)kProfession.getYieldsProduced(0);
								// R&R, ray , MYCP partially based on code of Aymerick - END
								if (kProfession.isWorkPlot())
								{
									CvPlot* pWorkedPlot = pLoopCity->getPlotWorkedByUnit(pRemoveUnit);
									if (pWorkedPlot == NULL)
									{
										bRemove = true;
									}
									else
									{
										if ((pWorkedPlot->getBonusType() == NO_BONUS) || (GC.getBonusInfo(pWorkedPlot->getBonusType()).getYieldChange(eYieldProducedType) <= 0))
										{
											bRemove = true;
										}
										else
										{
											CvPlot* pBestWorkedPlot = AI_getBestWorkedYieldPlot(eYieldProducedType);
											if ((pBestWorkedPlot == NULL) || (pWorkedPlot->calculateBestNatureYield(eYieldProducedType, getTeam()) < pBestWorkedPlot->calculateBestNatureYield(eYieldProducedType, getTeam())))
											{
												bRemove = true;
											}
										}
									}
								}
								else
								{
									if (pLoopCity->AI_getYieldAdvantage(eYieldProducedType) < 100)
									{
										bRemove = true;
									}
								}
							}
						}
						if (bRemove)
						{
							pLoopCity->removePopulationUnit(pRemoveUnit, false, (ProfessionTypes) GC.getCivilizationInfo(getCivilizationType()).getDefaultProfession());
						}
					}
				}
			}

			for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
			{
				if (pLoopUnit->AI_getMovePriority() == 0)
				{
					pLoopUnit->AI_doInitialMovePriority();
				}
				else
				{
					FAssert(std::find(m_unitPriorityHeap.begin(), m_unitPriorityHeap.end(), pLoopUnit->getID()) != m_unitPriorityHeap.end());
				}
			}
			m_iTurnLastManagedPop = GC.getGameINLINE().getGameTurn();
			m_iMoveQueuePasses = 0;
		}
	}

	if (!hasBusyUnit())
	{
		pCurrUnitNode = headGroupCycleNode();

		while (pCurrUnitNode != NULL)
		{
			pLoopSelectionGroup = getSelectionGroup(pCurrUnitNode->m_data);
			pCurrUnitNode = nextGroupCycleNode(pCurrUnitNode);

			if (pLoopSelectionGroup->AI_isForceSeparate())
			{
				// do not split groups that are in the midst of attacking
				if (pLoopSelectionGroup->isForceUpdate() || !pLoopSelectionGroup->AI_isGroupAttack())
				{
					pLoopSelectionGroup->AI_separate();	// pointers could become invalid...
				}
			}
		}

		if (isHuman())
		{
			pCurrUnitNode = headGroupCycleNode();

			while (pCurrUnitNode != NULL)
			{
				pLoopSelectionGroup = getSelectionGroup(pCurrUnitNode->m_data);
				pCurrUnitNode = nextGroupCycleNode(pCurrUnitNode);

				if (pLoopSelectionGroup == NULL || pLoopSelectionGroup->AI_update())
				{
					break; // pointers could become invalid...
				}
			}
		}
        else
		{
			int iLoop;

			//Continue existing missions.
			for(pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
			{
				pLoopSelectionGroup->autoMission();
			}

			while (!m_unitPriorityHeap.empty())
			{
				AI_verifyMoveQueue();
				CvUnit* pUnit = AI_getNextMoveUnit();

				// TAC - AI BTS Groups - koma13 - START
				/*
				if ((pUnit != NULL) && shouldUnitMove(pUnit))
				{
					int iOriginalPriority = pUnit->AI_getMovePriority();
					if (iOriginalPriority > 0)
					{
						if (!pUnit->getGroup()->isBusy() && !pUnit->getGroup()->isCargoBusy())
						{
							pUnit->AI_update();
						}
						else
						{
							m_iMoveQueuePasses++;
							if (m_iMoveQueuePasses > 100)
							{
								FAssertMsg(false, "Forcing AI to abort turn");
								return;
							}
							AI_addUnitToMoveQueue(pUnit);
							return;
						}
					}
				}
				*/

				if (pUnit->getGroup()->getNumUnits() <= 1)
				{
					if ((pUnit != NULL) && shouldUnitMove(pUnit))
					{
						int iOriginalPriority = pUnit->AI_getMovePriority();
						if (iOriginalPriority > 0)
						{
							if (!pUnit->getGroup()->isBusy() && !pUnit->getGroup()->isCargoBusy())
							{
								pUnit->AI_update();
							}
							else
							{
								m_iMoveQueuePasses++;
								if (m_iMoveQueuePasses > 100)
								{
									FAssertMsg(false, "Forcing AI to abort turn");
									return;
								}
								AI_addUnitToMoveQueue(pUnit);
								return;
							}
						}
					}
				}
				else
				{
					AI_removeUnitFromMoveQueue(pUnit);
				}
			}
			
			AI_groupUpdate();
			// TAC - AI BTS Groups - koma13 - END

			for (CvSelectionGroup*pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
			{
				if (pLoopSelectionGroup->readyToMove())
				{
					pLoopSelectionGroup->pushMission(MISSION_SKIP);
				}
			}
		}
	}
}

// TAC - AI BTS Groups - koma13 - START
void CvPlayerAI::AI_groupUpdate()
{
	CLLNode<int>* pCurrUnitNode;
	CvSelectionGroup* pLoopSelectionGroup;
	CLinkList<int> tempGroupCycle;
	CLinkList<int> finalGroupCycle;
	int iValue;
	
	tempGroupCycle.clear();
	finalGroupCycle.clear();

	pCurrUnitNode = headGroupCycleNode();

	while (pCurrUnitNode != NULL)
	{
		pLoopSelectionGroup = getSelectionGroup(pCurrUnitNode->m_data);
		if (pLoopSelectionGroup->getNumUnits() > 1)
		{
			tempGroupCycle.insertAtEnd(pCurrUnitNode->m_data);
		}
		pCurrUnitNode = nextGroupCycleNode(pCurrUnitNode);
	}

	iValue = 0;

	while (tempGroupCycle.getLength() > 0)
	{
		pCurrUnitNode = tempGroupCycle.head();

		while (pCurrUnitNode != NULL)
		{
			pLoopSelectionGroup = getSelectionGroup(pCurrUnitNode->m_data);
			FAssertMsg(pLoopSelectionGroup != NULL, "selection group node with NULL selection group");

			if (AI_movementPriority(pLoopSelectionGroup) <= iValue)
			{
				finalGroupCycle.insertAtEnd(pCurrUnitNode->m_data);
				pCurrUnitNode = tempGroupCycle.deleteNode(pCurrUnitNode);
			}
			else
			{
				pCurrUnitNode = tempGroupCycle.next(pCurrUnitNode);
			}
		}

		iValue++;
	}

	pCurrUnitNode = finalGroupCycle.head();

	while (pCurrUnitNode != NULL)
	{
		pLoopSelectionGroup = getSelectionGroup(pCurrUnitNode->m_data);

		if (NULL != pLoopSelectionGroup)  // group might have been killed by a previous group update
		{
			if (pLoopSelectionGroup->AI_update())
			{
				break; // pointers could become invalid...
			}
		}

		pCurrUnitNode = finalGroupCycle.next(pCurrUnitNode);
	}

}
// TAC - AI BTS Groups - koma13 - END

void CvPlayerAI::AI_makeAssignWorkDirty()
{
	int iLoop;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->AI_setAssignWorkDirty(true);
	}
}


void CvPlayerAI::AI_assignWorkingPlots()
{
	AI_manageEconomy();

	int iLoop;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->AI_assignWorkingPlots();
	}
}


void CvPlayerAI::AI_updateAssignWork()
{
	int iLoop;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->AI_updateAssignWork();
	}
}


void CvPlayerAI::AI_makeProductionDirty()
{
	FAssertMsg(!isHuman(), "isHuman did not return false as expected");
	int iLoop;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->AI_setChooseProductionDirty(true);
	}
}


void CvPlayerAI::AI_conquerCity(CvCity* pCity)
{
	bool bRaze = false;
	if (canRaze(pCity))
	{
		bRaze = isNative();

		if (!bRaze)
		{
			CvCity* pNearestCity;
			int iRazeValue;

			iRazeValue = 0;
			if (GC.getGameINLINE().getElapsedGameTurns() > 20)
			{
				if (getNumCities() > 4)
				{
					pNearestCity = GC.getMapINLINE().findCity(pCity->getX_INLINE(), pCity->getY_INLINE(), NO_PLAYER, getTeam(), true, false, NO_TEAM, NO_DIRECTION, pCity);

					if (pNearestCity == NULL)
					{
						if (pCity->getPreviousOwner() != NO_PLAYER)
						{
							if (GET_TEAM(GET_PLAYER(pCity->getPreviousOwner()).getTeam()).countNumCitiesByArea(pCity->area()) > 3)
							{
								iRazeValue += 30;
							}
						}
					}
					else
					{
						int iDistance = plotDistance(pCity->getX_INLINE(), pCity->getY_INLINE(), pNearestCity->getX_INLINE(), pNearestCity->getY_INLINE());
						if ( iDistance > 12)
						{
							iRazeValue += iDistance * 2;
						}
					}

					int iCloseness = pCity->AI_playerCloseness(getID());
					if (iCloseness > 0)
					{
						iRazeValue -= 25;
						iRazeValue -= iCloseness * 2;
					}
					else
					{
						iRazeValue += 60;
					}

					if (pCity->area()->getCitiesPerPlayer(getID()) > 0)
					{
						iRazeValue += GC.getLeaderHeadInfo(getPersonalityType()).getRazeCityProb();
					}

					// TAC - AI Attack City - koma13 - START
					if (GET_PLAYER(pCity->getPreviousOwner()).isHuman())
					{
						int iChance = GC.getDefineINT("AI_CHANCE_FOR_RAZING_CITY");
						
						iRazeValue = std::max(0, iRazeValue);
						iRazeValue = std::min(iChance * 2, iRazeValue + iChance);
					}
					// TAC - AI Attack City - koma13 - END

					if (iRazeValue > 0)
					{
						if (GC.getGameINLINE().getSorenRandNum(100, "AI Raze City") < iRazeValue)
						{
							bRaze = true;
						}
					}
				}
			}
		}
	}

	if (bRaze)
	{
		pCity->doTask(TASK_RAZE);
	}
	else
	{
		gDLL->getEventReporterIFace()->cityAcquiredAndKept(GC.getGameINLINE().getActivePlayer(), pCity);
	}
}


bool CvPlayerAI::AI_acceptUnit(CvUnit* pUnit)
{
	return true;
}


bool CvPlayerAI::AI_captureUnit(UnitTypes eUnit, CvPlot* pPlot)
{
	CvCity* pNearestCity;

	FAssert(!isHuman());

	if (pPlot->getTeam() == getTeam())
	{
		return true;
	}

	pNearestCity = GC.getMapINLINE().findCity(pPlot->getX_INLINE(), pPlot->getY_INLINE(), NO_PLAYER, getTeam());

	if (pNearestCity != NULL)
	{
		if (plotDistance(pPlot->getX_INLINE(), pPlot->getY_INLINE(), pNearestCity->getX_INLINE(), pNearestCity->getY_INLINE()) <= 4)
		{
			return true;
		}
	}

	return false;
}


DomainTypes CvPlayerAI::AI_unitAIDomainType(UnitAITypes eUnitAI) const
{
	switch (eUnitAI)
	{
	case UNITAI_UNKNOWN:
		return NO_DOMAIN;
		break;

	case UNITAI_COLONIST:
	case UNITAI_ANIMAL: // R&R, ray, Wild Animals
	case UNITAI_FLEEING: // R&R, ray, Fleeing Units
	case UNITAI_SETTLER:
	case UNITAI_WORKER:
	case UNITAI_MISSIONARY:
	case UNITAI_TRADER: // WTP, ray, Native Trade Posts - START
	case UNITAI_SCOUT:
	case UNITAI_WAGON:
	case UNITAI_TREASURE:
	case UNITAI_YIELD:
	case UNITAI_GENERAL:
	case UNITAI_DEFENSIVE:
	case UNITAI_OFFENSIVE:
	case UNITAI_COUNTER:
		return DOMAIN_LAND;
		break;

	case UNITAI_ANIMAL_SEA: // R&R, ray, Wild Animals
	case UNITAI_WORKER_SEA: //TAC Whaling, ray
	case UNITAI_TRANSPORT_SEA:
	case UNITAI_TRANSPORT_COAST:
	case UNITAI_ASSAULT_SEA:
	case UNITAI_COMBAT_SEA:
	case UNITAI_PIRATE_SEA:
	case UNITAI_ESCORT_SEA:		// TAC - AI Escort Sea - koma13
		return DOMAIN_SEA;
		break;

	default:
		FAssert(false);
		break;
	}

	return NO_DOMAIN;
}

bool CvPlayerAI::AI_unitAIIsCombat(UnitAITypes eUnitAI) const
{
	switch (eUnitAI)
	{
	case UNITAI_UNKNOWN:
		return false;
		break;

	case UNITAI_FLEEING: // R&R, ray, Fleeing Units
	case UNITAI_COLONIST:
	case UNITAI_SETTLER:
	case UNITAI_WORKER:
	case UNITAI_MISSIONARY:
	case UNITAI_TRADER: // WTP, ray, Native Trade Posts - START
	case UNITAI_SCOUT:
	case UNITAI_WAGON:
	case UNITAI_TREASURE:
	case UNITAI_YIELD:
	case UNITAI_GENERAL:
	case UNITAI_TRANSPORT_COAST:
		return false;
		break;

	case UNITAI_ANIMAL: // R&R, ray, Wild Animals
	case UNITAI_DEFENSIVE:
	case UNITAI_OFFENSIVE:
	case UNITAI_COUNTER:
		return true;
		break;
	
	//TAC Whaling, ray
	case UNITAI_WORKER_SEA:
		return false;
		break;

	case UNITAI_ANIMAL_SEA: // R&R, ray, Wild Animals
	case UNITAI_TRANSPORT_SEA:
	case UNITAI_ASSAULT_SEA:
	case UNITAI_COMBAT_SEA:
	case UNITAI_PIRATE_SEA:
	case UNITAI_ESCORT_SEA: // TAC - AI Escort Sea - koma13
		return true;
		break;

	default:
		FAssert(false);
		break;
	}
	return false;
}


int CvPlayerAI::AI_yieldWeight(YieldTypes eYield) const
{
	return GC.getYieldInfo(eYield).getAIWeightPercent();
}

int CvPlayerAI::AI_estimatedColonistIncome(CvPlot* pPlot, CvUnit* pColonist)
{
	FAssert(pPlot != NULL);

	int iX = pPlot->getX_INLINE();
	int iY = pPlot->getY_INLINE();

	bool bFound = pColonist->canFound(pPlot);
	bool bJoin = pColonist->canJoinCity(pPlot);

	FAssert(!(bFound && bJoin));

	if (!(bFound || bJoin))
	{
		return -1;
	}

	if (!pPlot->isRevealed(getTeam(), false))
	{
		return -1;
	}

	//Calculate the income from the city tile plus the most profitable plot.
	int iTotal = 0;

	CvPlayer& kPlayerEurope = GET_PLAYER(getParent());

	if (bFound)
	{
		//cities get food and one other yield
		YieldTypes bestYield = NO_YIELD;
		int bestOutput = 0;
		for (int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			//ignore food and lumber and stone
			if ((i != YIELD_FOOD) && (i != YIELD_LUMBER) && (i != YIELD_STONE))
			{
				int natureYield = pPlot->calculateNatureYield((YieldTypes) i, getTeam(), false);
				if (natureYield > bestOutput)
				{
					bestYield = (YieldTypes) i;
					bestOutput = natureYield;
				}
			}
		}

		if (bestYield != NO_YIELD)
		{
			if (isYieldEuropeTradable(bestYield))
			{
				iTotal += kPlayerEurope.getYieldSellPrice(bestYield) * bestOutput;
			}
		}
	}

	int iBestValue = 0;
	for (int i = 0; i < NUM_CITY_PLOTS; i++)
	{
		CvPlot* pLoopPlot = plotCity(iX, iY, i);

		if (pLoopPlot != NULL)
		{
			if (!pLoopPlot->isBeingWorked())
			{
				YieldTypes bestYield = NO_YIELD;
				int bestOutput = 0;
				for (int j = 0; j < GC.getNumProfessionInfos(); j++)
				{
					ProfessionTypes loopProfession = (ProfessionTypes)j;
					if (GC.getCivilizationInfo(getCivilizationType()).isValidProfession(loopProfession))
					{
						CvProfessionInfo& kProfession = GC.getProfessionInfo(loopProfession);

						if (GC.getProfessionInfo(loopProfession).isWorkPlot())
						{
							CvProfessionInfo& kProfession = GC.getProfessionInfo(loopProfession);
							// R&R, ray , MYCP partially based on code of Aymerick - START
							YieldTypes eYield = (YieldTypes)kProfession.getYieldsProduced(0);
							// R&R, ray , MYCP partially based on code of Aymerick - END
							if ((eYield != NO_YIELD) && isYieldEuropeTradable(eYield))
							{
								int iValue = 0;
								int yield = pPlot->calculatePotentialProfessionYieldAmount(loopProfession, pColonist, false);
								if (eYield == YIELD_LUMBER || eYield == YIELD_STONE)
								{
									iValue += (yield * kPlayerEurope.getYieldSellPrice(eYield)) / 2;
								}
								else
								{
									iValue += yield * kPlayerEurope.getYieldBuyPrice(eYield);
								}

								iBestValue = std::max(iValue, iBestValue);
							}
						}
					}
				}
			}
		}
	}

	iTotal += iBestValue;

	if (bFound)
	{
		iTotal *= 7;
		iTotal /= 6 + getNumCities();
	}

	return iTotal;
}

int CvPlayerAI::AI_foundValue(int iX, int iY, int iMinRivalRange, bool bStartingLoc)
{
	PROFILE_FUNC();
	CvPlot* pPlot = GC.getMapINLINE().plotINLINE(iX, iY);

	if (!canFound(iX, iY))
	{
		return 0;
	}

	if (!bStartingLoc)
	{
		if (!pPlot->isRevealed(getTeam(), false))
		{
			return 0;
		}
	}

	// TAC - AI City Sites - koma13 - START
	if (!isNative())
	{
		if (pPlot->calculateNatureYield(YIELD_FOOD, getTeam(), true) < GC.getFOOD_CONSUMPTION_PER_POPULATION())
		{
				return 0;
		}
	}
	// TAC - AI City Sites - koma13 - END

	bool bNeedMoreExploring = false;
	if (getNumCities() == 0 && (pPlot->area()->getNumRevealedTiles(getTeam()) < 10))
	{
		bNeedMoreExploring = true;
	}

	if (isNative() && getNumCities() > 0)
	{
		int iRange = CITY_PLOTS_RADIUS * 2 - 1;

		int iCityDistance = AI_cityDistance(pPlot);
		if (iCityDistance == -1)
		{
			return 0;
		}
		if (iCityDistance < iRange || iCityDistance > (iRange * 3))
		{
			return 0;
		}

		for (int iDX = -iRange; iDX <= iRange; ++iDX)
		{
			for (int iDY = -iRange; iDY <= iRange; ++iDY)
			{
				CvPlot* pLoopPlot = plotXY(iX, iY, iDX, iDY);

				if (pLoopPlot != NULL)
				{
					if (pLoopPlot->isOwned())
					{
						return 0;
					}
				}
			}
		}
	}

	bool bIsCoastal = pPlot->isCoastalLand(GC.getMIN_WATER_SIZE_FOR_OCEAN());
	CvArea* pArea = pPlot->area();
	int iNumAreaCities = pArea->getCitiesPerPlayer(getID());

	bool bAdvancedStart = (getAdvancedStartPoints() >= 0);

	if (!bStartingLoc && !bAdvancedStart)
	{
		if (iNumAreaCities == 0)
		{
			if (getParent() != NO_PLAYER)
			{
				if (pPlot->getNearestEurope() == NO_EUROPE)
				{
					return 0;
				}
			}
		}
	}

	if (!bStartingLoc)
	{
		if (getNumCities() == 0)
		{
			if (pArea->getNumTiles() < (NUM_CITY_PLOTS * 3))
			{
				return 0;
			}
		}
	}

	if (bAdvancedStart)
	{
		//FAssert(!bStartingLoc);
		FAssert(GC.getGameINLINE().isOption(GAMEOPTION_ADVANCED_START) || GC.getCivilizationInfo(getCivilizationType()).getAdvancedStartPoints() > 0);
		if (bStartingLoc)
		{
			bAdvancedStart = false;
		}
	}

	if (iMinRivalRange != -1)
	{
		for (int iDX = -(iMinRivalRange); iDX <= iMinRivalRange; iDX++)
		{
			for (int iDY = -(iMinRivalRange); iDY <= iMinRivalRange; iDY++)
			{
				CvPlot* pLoopPlot = plotXY(iX, iY, iDX, iDY);

				if (pLoopPlot != NULL)
				{
					if (pLoopPlot->plotCheck(PUF_isOtherTeam, getID()) != NULL)
					{
						return 0;
					}
				}
			}
		}
	}

	int iOwnedTiles = 0;

	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		CvPlot* pLoopPlot = plotCity(iX, iY, iI);

		if (pLoopPlot == NULL)
		{
			iOwnedTiles++;
		}
		else if (pLoopPlot->isOwned() && !GET_PLAYER(pLoopPlot->getOwnerINLINE()).isNative())
        {
            if (pLoopPlot->getTeam() != getTeam())
            {
                iOwnedTiles++;
            }
        }
	}

	if (iOwnedTiles > (NUM_CITY_PLOTS / 3))
	{
		return 0;
	}

	int iBadTile = 0;
	int iNativeTile = 0;
	int iFriendlyTile = 0;
	int iColonialTile = 0;

	std::vector<int> aiFood(NUM_CITY_PLOTS, 0);

	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		CvPlot* pLoopPlot = plotCity(iX, iY, iI);

		if (pLoopPlot != NULL)
		{
			if (iI != CITY_HOME_PLOT)
			{
				if (pLoopPlot->isImpassable())
				{
					if (pLoopPlot->getBonusType() == NO_BONUS)
					{
						// Only count impassable plots that do not have any bonus resource as bad
						iBadTile += 2;
					}
				}
				else if (!pLoopPlot->isOwned() || GET_PLAYER(pLoopPlot->getOwnerINLINE()).isNative())
				{
					if (!pLoopPlot->isHills() && !pLoopPlot->isWater())
					{
						// R&R, ray, adjustment because of MYCP
						if (((pLoopPlot->calculateBestNatureYield(YIELD_FOOD, getTeam()) == 0) && (pLoopPlot->calculateBestNatureYield(YIELD_FUR, getTeam()) < 2)) || (pLoopPlot->calculateTotalBestNatureYield(getTeam()) <= 2))
						{
							iBadTile++;
						}
						else if (pLoopPlot->isWater() && !bIsCoastal && (pLoopPlot->calculateBestNatureYield(YIELD_FOOD, getTeam()) <= 1))
						{
							iBadTile++;
						}
					}
				}
			}
			if (pLoopPlot->isOwned())
			{
				if (GET_PLAYER(pLoopPlot->getOwnerINLINE()).isNative())
				{
					iNativeTile++;
				}
				else if (GET_PLAYER(pLoopPlot->getOwnerINLINE()).getParent() != NO_PLAYER)
				{
					iColonialTile += pLoopPlot->getCityRadiusCount();
					if (pLoopPlot->getTeam() == getTeam())
					{
						iFriendlyTile++;
					}
				}
			}
		}
		else
		{
			iBadTile++;
		}

	}

	if(!bStartingLoc)
	{
		if (bNeedMoreExploring)
		{
			if (iBadTile >= (NUM_CITY_PLOTS / 2))
			{
				return 0;
			}
		}
	}


	int aiBestWorkedYield[NUM_YIELD_TYPES];
	int aiBestUnworkedYield[NUM_YIELD_TYPES];

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		CvPlot* pWorkedPlot = AI_getBestWorkedYieldPlot((YieldTypes)iYield);
		if (pWorkedPlot == NULL)
		{
			aiBestWorkedYield[iYield] = 0;
		}
		else
		{
			aiBestWorkedYield[iYield] = pWorkedPlot->calculateBestNatureYield((YieldTypes)iYield, getTeam());
		}

		CvPlot* pUnworkedPlot = AI_getBestUnworkedYieldPlot((YieldTypes)iYield);
		if (pUnworkedPlot == NULL)
		{
			aiBestUnworkedYield[iYield] = 0;
		}
		else
		{
			aiBestUnworkedYield[iYield] = pUnworkedPlot->calculateBestNatureYield((YieldTypes)iYield, getTeam());
		}
	}

	int iTakenTiles = 0;
	int iTeammateTakenTiles = 0;
	int iValue = 1000;

	int iBestPlotValue = 0;
	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		CvPlot* pLoopPlot = plotCity(iX, iY, iI);

		if (pLoopPlot == NULL)
		{
			iTakenTiles++;
		}
		else
		{
			if (pLoopPlot->isCityRadius())
			{
				iTakenTiles++;

				if (pLoopPlot->getTeam() == getTeam() && pLoopPlot->getOwner() != getID())
				{
					iTeammateTakenTiles++;
				}
			}

			if (!pLoopPlot->isCityRadius() || (pLoopPlot->isOwned() && GET_PLAYER(pLoopPlot->getOwnerINLINE()).isNative()))
			{
				int iBestBonusAmount = 0;
				YieldTypes eBestBonusYield = NO_YIELD;

				int aiYield[NUM_YIELD_TYPES];

				for (int iYieldType = 0; iYieldType < NUM_YIELD_TYPES; ++iYieldType)
				{


					YieldTypes eYield = (YieldTypes)iYieldType;
					int iYield = pLoopPlot->calculateBestNatureYield(eYield, getTeam());



					if (iI == CITY_HOME_PLOT)
					{
						iYield += GC.getYieldInfo(eYield).getCityChange();
						//XXX make sure this reflects reality of Col
						iYield = std::max(iYield, GC.getYieldInfo(eYield).getMinCity());
					}

					aiYield[eYield] = iYield;
					if (eYield == YIELD_FOOD)
					{
						aiFood[iI] = iYield;
					}
				}

				if (iI == CITY_HOME_PLOT)
				{
					iValue += 2 * aiYield[YIELD_FOOD] * AI_yieldValue(YIELD_FOOD);

					YieldTypes bestYield = NO_YIELD;
					int bestOutput = 0;
					for (int i = 0; i < NUM_YIELD_TYPES; i++)
					{
						//ignore food and lumber and stone
						if ((i != YIELD_FOOD) && (i != YIELD_LUMBER) && (i != YIELD_STONE))
						{
							int natureYield = pPlot->calculateNatureYield((YieldTypes) i, getTeam(), false);
							if (natureYield > bestOutput)
							{
								bestYield = (YieldTypes) i;
								bestOutput = natureYield;
							}
						}
					}
					if (bestYield != NO_YIELD)
					{
						iValue += 2 * bestOutput * AI_yieldValue(bestYield);
					}
				}
				else
				{
					YieldTypes eBestYield = NO_YIELD;
					int iBestValue = 0;
					for (int iYieldType = 0; iYieldType < NUM_YIELD_TYPES; ++iYieldType)
					{
						YieldTypes eYield = (YieldTypes)iYieldType;

						if (aiYield[eYield] > 0)
						{
							int iYieldValue = aiYield[eYield] * AI_yieldValue(eYield);

							if (pLoopPlot->isWater())
							{
								iYieldValue /= 2;
							}

							if (getNumCities() > 0)
							{
								if (aiYield[eYield] > aiBestUnworkedYield[eYield])
								{
									iYieldValue *= (4 + aiYield[eYield]);
									iYieldValue /= (2 + aiBestUnworkedYield[eYield]);

									if (aiBestWorkedYield[eYield] == 0)
									{
										iYieldValue *= 2;
										if (aiBestUnworkedYield[eYield] == 0)
										{
											if (eYield == YIELD_LUMBER)
											{
												iYieldValue *= 4;
											}
											if (eYield == YIELD_STONE && getNumCities() > 4)
											{
												iYieldValue *= 2;
											}
										}
									}
								}
							}
							else
							{
								if (eYield == YIELD_LUMBER)
								{
									iYieldValue *= 2;
								}
							}

							iValue += iYieldValue / ((getNumCities() == 0) ? 3 : 8);
							if (iYieldValue > iBestValue)
							{
								iBestValue = iYieldValue;
								eBestYield = eYield;
							}
						}
					}

					if (getNumCities() == 0)
					{
						if (eBestYield == YIELD_FOOD)
						{
							iBestValue *= 150;
							iBestValue /= 100;
						}
					}

					iValue += iBestValue;

					iBestPlotValue = std::max(iBestPlotValue, iBestValue);

					if (pLoopPlot->getBonusType() != NO_BONUS && eBestYield != NO_YIELD)
					{
						iValue += iBestValue;
						if (GC.getBonusInfo(pLoopPlot->getBonusType()).getYieldChange(eBestYield) > 0)
						{
							iValue += iBestValue;
						}
					}
				}
			}
		}
	}

	iValue += iBestPlotValue;

	if (iTeammateTakenTiles > 1)
	{
		return 0;
	}

	if (pPlot->isCoastalLand(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
	{
		iValue *= 125;
		iValue /= 100;
	}

	if (bStartingLoc)
	{
		int iRange = GREATER_FOUND_RANGE;
		int iGreaterBadTile = 0;

		for (int iDX = -(iRange); iDX <= iRange; iDX++)
		{
			for (int iDY = -(iRange); iDY <= iRange; iDY++)
			{
				CvPlot* pLoopPlot = plotXY(iX, iY, iDX, iDY);

				if (pLoopPlot != NULL)
				{
					if (pLoopPlot->isWater() || (pLoopPlot->area() == pArea))
					{
						if (plotDistance(iX, iY, pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE()) <= iRange)
						{
						    int iTempValue = 0;
							iTempValue += (pLoopPlot->calculatePotentialYield(YIELD_FOOD, NULL, false) * 15);
							iTempValue += (pLoopPlot->calculatePotentialYield(YIELD_FUR, NULL, false) * 4); // R&R, ray, adjustment because of MYCP
							iValue += iTempValue;
							if (iTempValue < 21)
							{
								iGreaterBadTile += 2;
								if (pLoopPlot->getFeatureType() != NO_FEATURE)
								{
									// R&R, ray, adjustment because of MYCP
									//if (pLoopPlot->calculateBestNatureYield(YIELD_FOOD, getTeam()) > 1)
							    	if (pLoopPlot->calculateBestNatureYield(YIELD_FOOD, getTeam()) > 1 || pLoopPlot->calculateBestNatureYield(YIELD_FUR, getTeam()) > 2)
							    	{
										iGreaterBadTile--;
							    	}
								}
							}
						}
					}
				}
			}
		}

		if (!pPlot->isStartingPlot())
		{
			iGreaterBadTile /= 2;
			if (iGreaterBadTile > 12)
			{
				iValue *= 11;
				iValue /= iGreaterBadTile;
			}
		}

		int iWaterCount = 0;

		for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
		    CvPlot* pLoopPlot = plotCity(iX, iY, iI);

            if (pLoopPlot != NULL)
		    {
		        if (pLoopPlot->isWater())
		        {
		            iWaterCount ++;
		            if (pLoopPlot->calculatePotentialYield(YIELD_FOOD, NULL, false) <= 1)
		            {
		                iWaterCount++;
					}
				}
			}
		}
		iWaterCount /= 2;

		int iLandCount = (NUM_CITY_PLOTS - iWaterCount);

		if (iLandCount < (NUM_CITY_PLOTS / 2))
		{
		    //discourage very water-heavy starts.
		    iValue *= 1 + iLandCount;
		    iValue /= (1 + (NUM_CITY_PLOTS / 2));
		}
	}

	if (bStartingLoc)
	{
		if (pPlot->getMinOriginalStartDist() == -1)
		{
			iValue += (GC.getMapINLINE().maxStepDistance() * 100);
		}
		else
		{
			iValue *= (1 + 4 * pPlot->getMinOriginalStartDist());
			iValue /= (1 + 2 * GC.getMapINLINE().maxStepDistance());
		}

		//nice hacky way to avoid this messing with normalizer, use elsewhere?
		if (!pPlot->isStartingPlot())
		{
			int iMinDistanceFactor = MAX_INT;
			int iMinRange = startingPlotRange();

			iValue *= 100;
			for (int iJ = 0; iJ < MAX_PLAYERS; iJ++)
			{
				if (GET_PLAYER((PlayerTypes)iJ).isAlive())
				{
					if (iJ != getID())
					{
						int iClosenessFactor = GET_PLAYER((PlayerTypes)iJ).startingPlotDistanceFactor(pPlot, getID(), iMinRange);
						iMinDistanceFactor = std::min(iClosenessFactor, iMinDistanceFactor);

						if (iClosenessFactor < 1000)
						{
							iValue *= 2000 + iClosenessFactor;
							iValue /= 3000;
						}
					}
				}
			}

			if (iMinDistanceFactor > 1000)
			{
				if (isNative())
				{
					iValue *= 500 + iMinDistanceFactor;
					iValue /= 1500;
				}
				else
				{
					//give a maximum boost of 25% for somewhat distant locations, don't go overboard.
					iMinDistanceFactor = std::min(1500, iMinDistanceFactor);
					iValue *= (1000 + iMinDistanceFactor);
					iValue /= 2000;
				}
			}
			else if (iMinDistanceFactor < 1000)
			{
				//this is too close so penalize again.
				iValue *= iMinDistanceFactor;
				iValue /= 1000;
				iValue *= iMinDistanceFactor;
				iValue /= 1000;
			}

			iValue /= 10;
		}
	}

	if (getNumCities() > 0)
	{
		//Friendly City Distance Modifier
		if (isNative())
		{
			int iCityDistance = AI_cityDistance(pPlot);

			iCityDistance = std::min(iCityDistance, 15);

			iValue *= 10;
			iValue /= 6 + iCityDistance;

			if (iCityDistance < 4)
			{
				iValue /= 4 - iCityDistance;
			}
		}
		else
		{
			int iCityDistance = AI_cityDistance(pPlot);

			iCityDistance = std::min(iCityDistance, 10);

			int iMinDistance = GC.getMIN_CITY_RANGE() + 1;
			int iMaxDistance = iMinDistance + 2;

			if (iCityDistance < iMinDistance)
			{
				int iFactor = AI_isStrategy(STRATEGY_DENSE_CITY_SPACING) ? 4 : 1;
				iValue *= iFactor + iCityDistance;
				iValue /= iFactor + iMinDistance;
			}

			if (iCityDistance > iMaxDistance)
			{
				// TAC - AI City Sites - koma13 - START
				
				//iValue *= iMaxDistance;
				//iValue /= iCityDistance;
				
				CvArea* pArea = pPlot->area();
				bool bEuropeCities = false;

				if (pArea->getNumCities() > 0)
				{
					for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
					{
						CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
						if (kLoopPlayer.isAlive() && !kLoopPlayer.isNative())
						{
							if (pArea->getCitiesPerPlayer((PlayerTypes)iPlayer) > 0)
							{
								bEuropeCities = true;
								break;
							}
						}
					}
				}
				
				if (bEuropeCities)
				{
					iValue *= iMaxDistance;
					iValue /= iCityDistance;
				}
				
				// TAC - AI City Sites - koma13 - END
			}

			CvCity* pPrimaryCity = AI_getPrimaryCity();
			if (pPrimaryCity != NULL)
			{
				int iDistance = stepDistance(iX, iY, pPrimaryCity->getX_INLINE(), pPrimaryCity->getY_INLINE());
				
				// TAC - AI City Sites - koma13 - START
				if (pPlot->getArea() != pPrimaryCity->plot()->getArea())
				{
					iDistance = iMinDistance;
				}
				// TAC - AI City Sites - koma13 - END

				iValue *= 6;
				iValue /= 3 + iDistance;
				if (iDistance > 9)
				{
					iValue *= 3;
					iValue /= iDistance - 6;
				}
			}
		}
	}
	
	// TAC - AI City Sites - koma13 - START
	/*
	else if (!isNative())
	{// Ocean Distance Modifier
		iValue *= 8;
		iValue /= std::max(1, pPlot->getDistanceToOcean());
	}
	*/
	else if (!isNative())
	{
		iValue *= 2;
	}
	// TAC - AI City Sites - koma13 - END

	if (iValue <= 0)
	{
		return 1;
	}

	if (bNeedMoreExploring)
	{
		int iBonusCount = 0;
		int iLandCount = 0;
		for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
			CvPlot* pLoopPlot = plotCity(iX, iY, iI);

			if (iI != CITY_HOME_PLOT)
			{
				if ((pLoopPlot == NULL) || pLoopPlot->isImpassable())
				{
					return 1;
				}
				else
				{
					if (pLoopPlot->getBonusType() != NO_BONUS)
					{
						iBonusCount++;
					}
				}
				if (!pLoopPlot->isWater())
				{
					iLandCount++;
				}
			}

		}

		if (iBonusCount == 0)
		{
			return 1;
		}
		else if (iBonusCount == 1)
		{
			if (pPlot->getYield(YIELD_FOOD) < GC.getFOOD_CONSUMPTION_PER_POPULATION())
			{
				// R&R, ray, adjustment because of MYCP
				if (pPlot->getYield(YIELD_FUR) < GC.getFOOD_CONSUMPTION_PER_POPULATION())
				{
					return 1;
				}
			}
			if (iLandCount < (NUM_CITY_PLOTS / 2))
			{
				return 1;
			}
			iValue /= 4;
		}
	}

	//Modify value according to easily attainable food.
	//Making this more important for early cities.
	{
		int iFood = aiFood[CITY_HOME_PLOT];

		std::sort(aiFood.begin(), aiFood.end(), std::greater<int>());

		iFood += aiFood[0];

		int iConsumption = 4 * GC.getFOOD_CONSUMPTION_PER_POPULATION();

		if (iFood < iConsumption)
		{
			if (getNumCities() == 0)
			{
				return 1;
			}
			iValue *= 100 - 4 * (100 - 100 * iFood / iConsumption) / (4 + getNumCities());
			iValue /= 100;
		}
		else if (iFood > iConsumption)
		{
			iValue *= 100 + ((100 * iFood / iConsumption) - 100) / (1 + getNumCities());
			iValue /= 100;
		}

		if (isNative())
		{
			iValue *= iFood;
			iValue /= iConsumption;
		}
	}

	if (pPlot->getBonusType() != NO_BONUS)
	{
		iValue *= 2;
		iValue /= 3;
	}

	//Modify values according to other player culture
	if (!isNative())
	{
		iValue *= (NUM_CITY_PLOTS - iNativeTile);
		iValue /= NUM_CITY_PLOTS;

		if (iColonialTile > 0)
		{
			iValue *= std::max(1, (NUM_CITY_PLOTS - iColonialTile));
			iValue /= NUM_CITY_PLOTS;

			if (iFriendlyTile == 0)
			{
				iValue /= 2;
			}
		}
	}

	return std::max(1, iValue);
}

int CvPlayerAI::AI_foundValueNative(int iX, int iY, bool allowSettleOnBonus) const
{
	CvPlot* pPlot = plotXY(iX, iY, 0, 0);
	FAssert(pPlot != NULL);

	if (!allowSettleOnBonus && pPlot->getBonusType() != NO_BONUS)
	{
		return 0;
	}
		
	if (pPlot->isWater())
	{
		return 0;
	}

	if (pPlot->getOwnerINLINE() != getID())
	{
		return 0;
	}

	if (!canFound(iX, iY, false))
	{
		return 0;
	}

	int iYields = 0;
	for (int i = 0; i < NUM_YIELD_TYPES; i++)
	{
		if (i != YIELD_FOOD)
		{
			iYields += pPlot->getYield((YieldTypes)i);
		}
	}
	if (iYields == 0)
	{
		return 0;
	}

	int iWaterCount = 0;
	for (int iDirection = 0; iDirection < NUM_DIRECTION_TYPES; iDirection++)
	{
		CvPlot* pLoopPlot = plotDirection(iX, iY, (DirectionTypes)iDirection);
		if (pLoopPlot != NULL)
		{
			if (pLoopPlot->isWater() && !pLoopPlot->isLake())
			{
				iWaterCount++;
			}
		}
	}
	if (iWaterCount > 4)
	{
		return 0;
	}

	// R&R, ray, adjustment for Mapsize Gigantic
	bool giganticMap = (GC.getMapINLINE().getWorldSize() == WORLDSIZE_GIGANTIC);

	int iBadTileCount = 0;
	for (int iI = 0; iI < NUM_CITY_PLOTS; ++iI)
	{
		CvPlot* pLoopPlot = plotCity(iX, iY, iI);
		if (pLoopPlot != NULL)
		{
			if (pLoopPlot->isImpassable())
			{
				iBadTileCount++;
			}

			// R&R, ray, adjustment for Mapsize Gigantic - START
			else
			{
				if (giganticMap)
				{
					// R&R, ray, adjustment because of MYCP
					if (pLoopPlot->getYield(YIELD_FOOD) < 2 && pLoopPlot->getYield(YIELD_FUR) < 2)
					{
						iBadTileCount++;
					}
				}
				else
				{
					// R&R, ray, adjustment because of MYCP
					if (pLoopPlot->getYield(YIELD_FOOD) < 1 && pLoopPlot->getYield(YIELD_FUR) < 2)
					{
						iBadTileCount++;
					}
				}
			}
			// R&R, ray, adjustment for Mapsize Gigantic - END
		}
	}
	if (iBadTileCount > (NUM_CITY_PLOTS * 2) / 3)
	{
		return 0;
	}

	int iValue = 0;
	int iCityCount = 0;
	int iRange = 3;

	for (int iDX = -iRange; iDX <= iRange; iDX++)
	{
		for (int iDY = -iRange; iDY <= iRange; iDY++)
		{
			int iDistance = plotDistance(iDX, iDY, 0, 0);
			if (iDistance <= iRange)
			{
				CvPlot* pLoopPlot = plotXY(iX, iY, iDX, iDY);
				if (pLoopPlot != NULL)
				{
					if (pLoopPlot->isCity())
					{
						iCityCount++;
						// R&R, ray, general improvement
						// if (iDistance == 1)
						if (iDistance <= 2)
						{
							iCityCount++;
						}
					}
					else if (!pLoopPlot->isCityRadius())
					{
						if (pLoopPlot->isWater())
						{
							iValue ++;
						}
						else if (pLoopPlot->getOwnerINLINE() == getID())
						{
							iValue++;
						}
					}
				}
			}
		}
	}

	if (iCityCount > 1)
	{
		return 0;
	}

	// R&R, ray, adjustment for Mapsize Gigantic - START
	if (giganticMap)
	{
		if (iValue < (28 / (std::max(1, 3 - getNumCities()))))
		{
			return 0;
		}
	}

	else 
	{
		// TAC - AI More native settlements - START
		if (iValue < (26 / (std::max(1, 3 - getNumCities()))))
		{
			return 0;
		}
		// TAC - AI More native settlements - END
	}
	// R&R, ray, adjustment for Mapsize Gigantic - END

	iValue *= 100;
	iValue += GC.getGame().getSorenRandNum(300, "AI native city found value");
	return iValue;
}


bool CvPlayerAI::AI_isAreaAlone(CvArea* pArea)
{
	return ((pArea->getNumCities()) == GET_TEAM(getTeam()).countNumCitiesByArea(pArea));
}


bool CvPlayerAI::AI_isCapitalAreaAlone()
{
	CvCity* pCapitalCity;

	pCapitalCity = getPrimaryCity();

	if (pCapitalCity != NULL)
	{
		return AI_isAreaAlone(pCapitalCity->area());
	}

	return false;
}


bool CvPlayerAI::AI_isPrimaryArea(CvArea* pArea)
{
	CvCity* pCapitalCity;

	if (pArea->isWater())
	{
		return false;
	}

	if (pArea->getCitiesPerPlayer(getID()) > 2)
	{
		return true;
	}

	pCapitalCity = getPrimaryCity();

	if (pCapitalCity != NULL)
	{
		if (pCapitalCity->area() == pArea)
		{
			return true;
		}
	}

	return false;
}


int CvPlayerAI::AI_militaryWeight(CvArea* pArea)
{
	return (pArea->getPopulationPerPlayer(getID()) + pArea->getCitiesPerPlayer(getID()) + 1);
}


int CvPlayerAI::AI_targetCityValue(CvCity* pCity, bool bRandomize, bool bIgnoreAttackers)
{
	PROFILE_FUNC();

	CvCity* pNearestCity;
	CvPlot* pLoopPlot;
	int iValue;
	int iI;

	FAssertMsg(pCity != NULL, "City is not assigned a valid value");

	iValue = 1;

	iValue += ((pCity->getPopulation() * (50 + pCity->calculateCulturePercent(getID()))) / 100);

	if (pCity->getDefenseDamage() > 0)
	{
		iValue += ((pCity->getDefenseDamage() / 30) + 1);
	}

	if (pCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
	{
		iValue++;
	}

	if (pCity->isEverOwned(getID()))
	{
		iValue += 3;
	}
	if (!bIgnoreAttackers)
	{
	iValue += AI_adjacentPotentialAttackers(pCity->plot());
	}

	for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		pLoopPlot = plotCity(pCity->getX_INLINE(), pCity->getY_INLINE(), iI);

		if (pLoopPlot != NULL)
		{
			if (pLoopPlot->getBonusType() != NO_BONUS)
			{
				iValue++;
			}

			if (pLoopPlot->getOwnerINLINE() == getID())
			{
				iValue++;
			}

			if (pLoopPlot->isAdjacentPlayer(getID(), true))
			{
				iValue++;
			}
		}
	}

	pNearestCity = GC.getMapINLINE().findCity(pCity->getX_INLINE(), pCity->getY_INLINE(), getID());

	if (pNearestCity != NULL)
	{
		iValue += std::max(1, ((GC.getMapINLINE().maxStepDistance() * 2) - GC.getMapINLINE().calculatePathDistance(pNearestCity->plot(), pCity->plot())));
	}

	if (bRandomize)
	{
		iValue += GC.getGameINLINE().getSorenRandNum(((pCity->getPopulation() / 2) + 1), "AI Target City Value");
	}

	return iValue;
}


CvCity* CvPlayerAI::AI_findTargetCity(CvArea* pArea)
{
	CvCity* pLoopCity;
	CvCity* pBestCity;
	int iValue;
	int iBestValue;
	int iLoop;
	int iI;

	iBestValue = 0;
	pBestCity = NULL;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			// R&R, ray, Natives raiding party - START
			if ((isPotentialEnemy(getTeam(), GET_PLAYER((PlayerTypes)iI).getTeam())) || (AI_isPotentialRaidTarget((PlayerTypes)iI)))
			{
				for (pLoopCity = GET_PLAYER((PlayerTypes)iI).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iI).nextCity(&iLoop))
				{
					if (pLoopCity->area() == pArea)
					{
						iValue = AI_targetCityValue(pLoopCity, true);

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							pBestCity = pLoopCity;
						}
					}
				}
			}
			// R&R, ray, Natives raiding party - END
		}
	}

	return pBestCity;
}


int CvPlayerAI::AI_getPlotDanger(CvPlot* pPlot, int iRange, bool bTestMoves, bool bOffensive)
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pLoopPlot;
	int iCount;
	int iDistance;
	int iBorderDanger;
	int iDX, iDY;
	CvArea *pPlotArea = pPlot->area();

	iCount = 0;
	iBorderDanger = 0;

	if (iRange == -1)
	{
		iRange = DANGER_RANGE;
	}

	for (iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for (iDY = -(iRange); iDY <= iRange; iDY++)
		{
			pLoopPlot	= plotXY(pPlot->getX_INLINE(), pPlot->getY_INLINE(), iDX, iDY);

			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->area() == pPlotArea)
				{
					iDistance = stepDistance(pPlot->getX_INLINE(), pPlot->getY_INLINE(), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE());
					if (atWar(pLoopPlot->getTeam(), getTeam()))
					{
						if (iDistance == 1)
						{
							iBorderDanger++;
						}
						else if ((iDistance == 2) && (pLoopPlot->isRoute()))
						{
							iBorderDanger++;
						}
					}


					pUnitNode = pLoopPlot->headUnitNode();

					while (pUnitNode != NULL)
					{
						pLoopUnit = ::getUnit(pUnitNode->m_data);
						pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

						if (pLoopUnit->isEnemy(getTeam()))
						{
							if (bOffensive || pLoopUnit->canAttack())
							{
								if (!(pLoopUnit->isInvisible(getTeam(), false)))
								{
									if (bOffensive || pLoopUnit->canMoveOrAttackInto(pPlot))
									{
										if (!bTestMoves)
										{
											iCount++;
										}
										else
										{
											int iDangerRange = pLoopUnit->baseMoves();
											iDangerRange += ((pLoopPlot->isValidRoute(pLoopUnit)) ? 1 : 0);
											if (iDangerRange >= iDistance)
											{
												iCount++;
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
	}

	if (iBorderDanger > 0)
	{
		if (!isHuman() && (!pPlot->isCity() || bOffensive))
		{
			iCount += iBorderDanger;
		}
	}

	return iCount;
}

int CvPlayerAI::AI_getUnitDanger(CvUnit* pUnit, int iRange, bool bTestMoves, bool bAnyDanger) const
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pLoopPlot;
	int iCount;
	int iDistance;
	int iBorderDanger;
	int iDX, iDY;

	CvPlot* pPlot = pUnit->plot();
	iCount = 0;
	iBorderDanger = 0;

	if (iRange == -1)
	{
		iRange = DANGER_RANGE;
	}

	for (iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for (iDY = -(iRange); iDY <= iRange; iDY++)
		{
			pLoopPlot = plotXY(pPlot->getX_INLINE(), pPlot->getY_INLINE(), iDX, iDY);

			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->area() == pPlot->area())
				{
					iDistance = stepDistance(pPlot->getX_INLINE(), pPlot->getY_INLINE(), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE());
					if (atWar(pLoopPlot->getTeam(), getTeam()))
					{
						if (iDistance == 1)
						{
							iBorderDanger++;
						}
						else if ((iDistance == 2) && (pLoopPlot->isRoute()))
						{
							iBorderDanger++;
						}
					}


					pUnitNode = pLoopPlot->headUnitNode();

					while (pUnitNode != NULL)
					{
						pLoopUnit = ::getUnit(pUnitNode->m_data);
						pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

						if (atWar(pLoopUnit->getTeam(), getTeam()))
						{
							if (pLoopUnit->canAttack())
							{
								if (!(pLoopUnit->isInvisible(getTeam(), false)))
								{
									if (pLoopUnit->canMoveOrAttackInto(pPlot))
									{
										if (!bTestMoves)
										{
											iCount++;
										}
										else
										{
											int iDangerRange = pLoopUnit->baseMoves();
											iDangerRange += ((pLoopPlot->isValidRoute(pLoopUnit)) ? 1 : 0);
											if (iDangerRange >= iDistance)
											{
												iCount++;
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
	}

	if (iBorderDanger > 0)
	{
	    if (!isHuman() || pUnit->isAutomated())
	    {
            iCount += iBorderDanger;
	    }
	}

	return iCount;
}

// TAC - AI Improved Navel AI - koma13 - START
//int CvPlayerAI::AI_getWaterDanger(CvPlot* pPlot, int iRange, bool bTestMoves)
// Returns a count of hostile units (animals will be ignored since they do not represent a real threat) within iRange of the plot
// if bDangerMap is true the count will include a (decaying) history of hostiles that we've seen.
int CvPlayerAI::AI_getWaterDanger(CvPlot* pPlot, int iRange, bool bTestMoves, bool bDangerMap, bool bVisibleOnly) const
// TAC - AI Improved Navel AI - koma13 - END
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pLoopPlot;
	int iCount;

	iCount = 0;

	if (iRange == -1)
	{
		iRange = DANGER_RANGE;
	}

	//CvArea* pWaterArea = pPlot->waterArea();

	for (int iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for (int iDY = -(iRange); iDY <= iRange; iDY++)
		{
			pLoopPlot = plotXY(pPlot->getX_INLINE(), pPlot->getY_INLINE(), iDX, iDY);
			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->isWater())
				{
					if (pPlot->isAdjacentToArea(pLoopPlot->getArea()))
					{
						if (!bVisibleOnly || pLoopPlot->isVisible(getTeam(), false))	// TAC - AI Improved Navel AI - koma13
						{
							pUnitNode = pLoopPlot->headUnitNode();	
							while (pUnitNode != NULL)
							{
								pLoopUnit = ::getUnit(pUnitNode->m_data);
								pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);
	
								if (!pLoopUnit->isEnemy(getTeam()))
									continue;

								if (!pLoopUnit->canAttack())
									continue;

								if ((pLoopUnit->isInvisible(getTeam(), false)))
									continue;

								// Ignore animals
								if (pLoopUnit->getUnitInfo().isAnimal())
									continue;

								iCount++;
							}
						}

						// TAC - AI Improved Navel AI - koma13 - START
						if (bDangerMap)
						{
							iCount += (pLoopPlot->getDangerMap(getID()) > 0) ? 1 : 0; 
						}
						// TAC - AI Improved Navel AI - koma13 - END
					}
				}
			}
		}
	}

	return iCount;
}

int CvPlayerAI::AI_goldTarget()
{
	int iGold = 0;

	if (GC.getGameINLINE().getElapsedGameTurns() >= 40)
	{
		int iMultiplier = 0;
		iMultiplier += GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getFatherPercent();
		iMultiplier += GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
		iMultiplier += GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getConstructPercent();
		iMultiplier /= 3;

		iGold += ((getNumCities() * 3) + (getTotalPopulation() / 3));

		iGold += (GC.getGameINLINE().getElapsedGameTurns() / 2);

		iGold *= iMultiplier;
		iGold /= 100;

		bool bAnyWar = GET_TEAM(getTeam()).getAnyWarPlanCount() > 0;
		if (bAnyWar)
		{
			iGold *= 3;
			iGold /= 2;
		}
		iGold += (AI_goldToUpgradeAllUnits() / (bAnyWar ? 1 : 2));
	}

	return iGold + AI_getExtraGoldTarget();
}

DiploCommentTypes CvPlayerAI::AI_getGreeting(PlayerTypes ePlayer)
{
	TeamTypes eWorstEnemy;

	if (GET_PLAYER(ePlayer).getTeam() != getTeam())
	{
		eWorstEnemy = GET_TEAM(getTeam()).AI_getWorstEnemy();
		if ((eWorstEnemy != NO_TEAM) && (eWorstEnemy != GET_PLAYER(ePlayer).getTeam()) && GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(eWorstEnemy) && (GC.getASyncRand().get(4) == 0))
		{
			if (GET_PLAYER(ePlayer).AI_hasTradedWithTeam(eWorstEnemy) && !atWar(GET_PLAYER(ePlayer).getTeam(), eWorstEnemy))
			{
				return (DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_WORST_ENEMY_TRADING");
			}
			else
			{
				return (DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_WORST_ENEMY");
			}
		}
	}

	return (DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_GREETINGS");
}


bool CvPlayerAI::AI_isWillingToTalk(PlayerTypes ePlayer)
{
	FAssertMsg(getPersonalityType() != NO_LEADER, "getPersonalityType() is not expected to be equal with NO_LEADER");
	FAssertMsg(ePlayer != getID(), "shouldn't call this function on ourselves");

	if (GET_PLAYER(ePlayer).getTeam() == getTeam())
	{
		return true;
	}

	if (GET_TEAM(getTeam()).isHuman())
	{
		return false;
	}

	if (atWar(getTeam(), GET_PLAYER(ePlayer).getTeam()))
	{
		if (GET_TEAM(getTeam()).isParentOf(GET_PLAYER(ePlayer).getTeam()))
		{
			return false;
		}

		int iRefuseDuration = (GC.getLeaderHeadInfo(getPersonalityType()).getRefuseToTalkWarThreshold());

		if  (GET_TEAM(getTeam()).AI_isChosenWar(GET_PLAYER(ePlayer).getTeam()))
		{
			if (!isNative())
			{
				iRefuseDuration *= 2;
			}
		}
		else
		{
			if (isNative())
			{
				iRefuseDuration *= 2;
			}
		}

		int iOurSuccess = 1 + GET_TEAM(getTeam()).AI_getWarSuccess(GET_PLAYER(ePlayer).getTeam());
		int iTheirSuccess = 1 + GET_TEAM(GET_PLAYER(ePlayer).getTeam()).AI_getWarSuccess(getTeam());
		if (iTheirSuccess > iOurSuccess * 2)
		{
			iRefuseDuration *= 50 + ((50 * iOurSuccess * 2) / iTheirSuccess);
			iRefuseDuration /= 100;
		}

		if (isNative())
		{
			iRefuseDuration *= 2;
			int iGameTurns = GC.getGameINLINE().getEstimateEndTurn();
			int iCurrentTurn = GC.getGameINLINE().getGameTurn();

			if (!GET_TEAM(getTeam()).AI_isChosenWar((GET_PLAYER(ePlayer).getTeam())))
			{
				iCurrentTurn += iGameTurns / 2;
			}

			iRefuseDuration *= std::max(0, iCurrentTurn - iGameTurns / 12);
			iRefuseDuration /= iGameTurns;
		}

		if (GET_TEAM(getTeam()).AI_getAtWarCounter(GET_PLAYER(ePlayer).getTeam()) < iRefuseDuration)
		{
			return false;
		}
	}
	else
	{
		if (AI_getMemoryCount(ePlayer, MEMORY_STOPPED_TRADING_RECENT) > 0)
		{
			return false;
		}
	}

	return true;
}


// XXX what if already at war???
// Returns true if the AI wants to sneak attack...
bool CvPlayerAI::AI_demandRebukedSneak(PlayerTypes ePlayer)
{
	FAssertMsg(!isHuman(), "isHuman did not return false as expected");
	FAssertMsg(ePlayer != getID(), "shouldn't call this function on ourselves");

	FAssert(!(GET_TEAM(getTeam()).isHuman()));

	if (GC.getGameINLINE().getSorenRandNum(100, "AI Demand Rebuked") < GC.getLeaderHeadInfo(getPersonalityType()).getDemandRebukedSneakProb())
	{
		if (GET_TEAM(getTeam()).getPower() > GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getDefensivePower())
		{
			return true;
		}
	}

	return false;
}


// XXX what if already at war???
// Returns true if the AI wants to declare war...
bool CvPlayerAI::AI_demandRebukedWar(PlayerTypes ePlayer)
{
	FAssertMsg(!isHuman(), "isHuman did not return false as expected");
	FAssertMsg(ePlayer != getID(), "shouldn't call this function on ourselves");

	FAssert(!(GET_TEAM(getTeam()).isHuman()));

	// needs to be async because it only happens on the computer of the player who is in diplomacy...
	if (GC.getASyncRand().get(100, "AI Demand Rebuked ASYNC") < GC.getLeaderHeadInfo(getPersonalityType()).getDemandRebukedWarProb())
	{
		if (GET_TEAM(getTeam()).getPower() > GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getDefensivePower())
		{
			if (GET_TEAM(getTeam()).AI_isAllyLandTarget(GET_PLAYER(ePlayer).getTeam()))
			{
				return true;
			}
		}
	}

	return false;
}


// XXX maybe make this a little looser (by time...)
bool CvPlayerAI::AI_hasTradedWithTeam(TeamTypes eTeam)
{
	int iI;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam)
			{
				if ((AI_getPeacetimeGrantValue((PlayerTypes)iI) + AI_getPeacetimeTradeValue((PlayerTypes)iI)) > 0)
				{
					return true;
				}
			}
		}
	}

	return false;
}

// static
AttitudeTypes CvPlayerAI::AI_getAttitude(int iAttitudeVal)
{
	if (iAttitudeVal >= 10)
	{
		return ATTITUDE_FRIENDLY;
	}
	else if (iAttitudeVal >= 3)
	{
		return ATTITUDE_PLEASED;
	}
	else if (iAttitudeVal <= -10)
	{
		return ATTITUDE_FURIOUS;
	}
	else if (iAttitudeVal <= -3)
	{
		return ATTITUDE_ANNOYED;
	}
	else
	{
		return ATTITUDE_CAUTIOUS;
	}
}

AttitudeTypes CvPlayerAI::AI_getAttitude(PlayerTypes ePlayer, bool bForced)
{
	PROFILE_FUNC();

	FAssertMsg(ePlayer != getID(), "shouldn't call this function on ourselves");

	return (AI_getAttitude(AI_getAttitudeVal(ePlayer, bForced)));
}


int CvPlayerAI::AI_getAttitudeVal(PlayerTypes ePlayer, bool bForced)
{
	PROFILE_FUNC();

	FAssertMsg(ePlayer != getID(), "shouldn't call this function on ourselves");

	if (bForced)
	{
		if (getTeam() == GET_PLAYER(ePlayer).getTeam())
		{
			return 100;
		}
	}

	int iAttitude = GC.getLeaderHeadInfo(getPersonalityType()).getBaseAttitude();

	if (GET_PLAYER(ePlayer).isNative())
	{
		iAttitude += GC.getLeaderHeadInfo(getPersonalityType()).getNativeAttitude();
	}

	iAttitude += GC.getHandicapInfo(GET_PLAYER(ePlayer).getHandicapType()).getAttitudeChange();

	iAttitude -= std::max(0, (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getNumMembers() - GET_TEAM(getTeam()).getNumMembers()));

	if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).AI_getWarSuccess(getTeam()) > GET_TEAM(getTeam()).AI_getWarSuccess(GET_PLAYER(ePlayer).getTeam()))
	{
		iAttitude += GC.getLeaderHeadInfo(getPersonalityType()).getLostWarAttitudeChange();
	}

	iAttitude += AI_getCloseBordersAttitude(ePlayer);
	iAttitude += AI_getStolenPlotsAttitude(ePlayer);
	iAttitude += AI_getAlarmAttitude(ePlayer);
	iAttitude += AI_getRebelAttitude(ePlayer);
	iAttitude += AI_getWarAttitude(ePlayer);
	iAttitude += AI_getPeaceAttitude(ePlayer);
	iAttitude += AI_getOpenBordersAttitude(ePlayer);
	iAttitude += AI_getDefensivePactAttitude(ePlayer);
	iAttitude += AI_getRivalDefensivePactAttitude(ePlayer);
	iAttitude += AI_getShareWarAttitude(ePlayer);
	iAttitude += AI_getTradeAttitude(ePlayer);
	iAttitude += AI_getRivalTradeAttitude(ePlayer);

	for (int iI = 0; iI < NUM_MEMORY_TYPES; iI++)
	{
		iAttitude += AI_getMemoryAttitude(ePlayer, ((MemoryTypes)iI));
	}

	iAttitude += AI_getAttitudeExtra(ePlayer);

	return range(iAttitude, -100, 100);
}


int CvPlayerAI::AI_calculateStolenCityRadiusPlots(PlayerTypes ePlayer)
{
	PROFILE_FUNC();

	CvPlot* pLoopPlot;
	int iCount;
	int iI;

	FAssert(ePlayer != getID());

	iCount = 0;

	for (iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

		if (pLoopPlot->getOwnerINLINE() == ePlayer)
		{
			if (pLoopPlot->isPlayerCityRadius(getID()))
			{
				iCount++;
			}
		}
	}

	return iCount;
}


int CvPlayerAI::AI_getCloseBordersAttitude(PlayerTypes ePlayer)
{
	if (m_aiCloseBordersAttitudeCache[ePlayer] == MAX_INT)
	{
		if (isNative())
		{
			return 0;
		}

		if (getTeam() == GET_PLAYER(ePlayer).getTeam())
		{
			return 0;
		}

		int iPercent = std::min(60, (AI_calculateStolenCityRadiusPlots(ePlayer) * 3));

		if (GET_TEAM(getTeam()).AI_isLandTarget(GET_PLAYER(ePlayer).getTeam()))
		{
			iPercent += 40;
		}

		m_aiCloseBordersAttitudeCache[ePlayer] = ((GC.getLeaderHeadInfo(getPersonalityType()).getCloseBordersAttitudeChange() * iPercent) / 100);
	}

	return m_aiCloseBordersAttitudeCache[ePlayer];
}


int CvPlayerAI::AI_getStolenPlotsAttitude(PlayerTypes ePlayer)
{
	if (m_aiStolenPlotsAttitudeCache[ePlayer] == MAX_INT)
	{
		if (getTeam() == GET_PLAYER(ePlayer).getTeam())
		{
			return 0;
		}

		if (!isNative())
		{
			return 0;
		}

		int iStolenPlots = 0;
		for(int i=0;i<GC.getMapINLINE().numPlotsINLINE();i++)
		{
			CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(i);
			if(pLoopPlot->getOwnerINLINE() == ePlayer && pLoopPlot->getCulture(getID()) > pLoopPlot->getCulture(ePlayer))
			{
				++iStolenPlots;
			}
		}

		// change attitude by stolen plots per city
		m_aiStolenPlotsAttitudeCache[ePlayer] = GC.getLeaderHeadInfo(getPersonalityType()).getCloseBordersAttitudeChange() * iStolenPlots / std::max(getNumCities(), 1);
	}

	return m_aiStolenPlotsAttitudeCache[ePlayer];
}


int CvPlayerAI::AI_getAlarmAttitude(PlayerTypes ePlayer)
{
	if (getTeam() == GET_PLAYER(ePlayer).getTeam())
	{
		return 0;
	}

	AlarmTypes eAlarm = (AlarmTypes) GC.getLeaderHeadInfo(getLeaderType()).getAlarmType();
	if (eAlarm == NO_ALARM)
	{
		return 0;
	}

	int iAlarm = GET_PLAYER(ePlayer).getNumCities() * GC.getAlarmInfo(eAlarm).getNumColonies();
	iAlarm *= std::max(0, 100 + GET_PLAYER(ePlayer).getNativeAngerModifier());
	iAlarm /= 100;

	int iLoop;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iAlarm += pLoopCity->AI_calculateAlarm(ePlayer);
	}

	iAlarm *= GC.getLeaderHeadInfo(getPersonalityType()).getAlarmAttitudeChange();
	iAlarm /= std::max(1, GC.getAlarmInfo(eAlarm).getAttitudeDivisor());

	return std::min(iAlarm, 0);
}

int CvPlayerAI::AI_getRebelAttitude(PlayerTypes ePlayer)
{
	if (GET_PLAYER(ePlayer).getParent() != getID())
	{
		return 0;
	}

	if (GC.getLeaderHeadInfo(getPersonalityType()).getRebelAttitudeDivisor() == 0)
	{
		return 0;
	}

	int iBells = GET_PLAYER(ePlayer).getBellsStored();

	iBells *= 100;
	iBells /= std::max(1, GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent());
	iBells /= GC.getLeaderHeadInfo(getPersonalityType()).getRebelAttitudeDivisor();

	// Erik: Negative attitude from bells now scale with the rebel sentiment. 
	// This effectively disables the exploit that allows players to request discounted 
	// units from their king as long as the king is at least cautious
	const int iRebelPercent = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getRebelPercent();
	// TODO: The maximum should be a XML parameter
	const int iCappedBells = static_cast<int>(std::max(-20.0, iBells * ((100 - iRebelPercent) / 100.0)));
		
	return iCappedBells;
}

int CvPlayerAI::AI_getWarAttitude(PlayerTypes ePlayer)
{
	int iAttitude = 0;

	if (atWar(getTeam(), GET_PLAYER(ePlayer).getTeam()))
	{
		iAttitude -= 3;
	}

	if (GC.getLeaderHeadInfo(getPersonalityType()).getAtWarAttitudeDivisor() != 0)
	{
		int iAttitudeChange = (GET_TEAM(getTeam()).AI_getAtWarCounter(GET_PLAYER(ePlayer).getTeam()) / GC.getLeaderHeadInfo(getPersonalityType()).getAtWarAttitudeDivisor());
		iAttitude += range(iAttitudeChange, -(abs(GC.getLeaderHeadInfo(getPersonalityType()).getAtWarAttitudeChangeLimit())), abs(GC.getLeaderHeadInfo(getPersonalityType()).getAtWarAttitudeChangeLimit()));
	}

	return iAttitude;
}


int CvPlayerAI::AI_getPeaceAttitude(PlayerTypes ePlayer)
{
	if (GC.getLeaderHeadInfo(getPersonalityType()).getAtPeaceAttitudeDivisor() != 0)
	{
		int iAttitudeChange = (GET_TEAM(getTeam()).AI_getAtPeaceCounter(GET_PLAYER(ePlayer).getTeam()) / GC.getLeaderHeadInfo(getPersonalityType()).getAtPeaceAttitudeDivisor());
		return range(iAttitudeChange, -(abs(GC.getLeaderHeadInfo(getPersonalityType()).getAtPeaceAttitudeChangeLimit())), abs(GC.getLeaderHeadInfo(getPersonalityType()).getAtPeaceAttitudeChangeLimit()));
	}

	return 0;
}


int CvPlayerAI::AI_getOpenBordersAttitude(PlayerTypes ePlayer)
{
	if (!atWar(getTeam(), GET_PLAYER(ePlayer).getTeam()))
	{
		if (GC.getLeaderHeadInfo(getPersonalityType()).getOpenBordersAttitudeDivisor() != 0)
		{
			int iAttitudeChange = (GET_TEAM(getTeam()).AI_getOpenBordersCounter(GET_PLAYER(ePlayer).getTeam()) / GC.getLeaderHeadInfo(getPersonalityType()).getOpenBordersAttitudeDivisor());
			return range(iAttitudeChange, -(abs(GC.getLeaderHeadInfo(getPersonalityType()).getOpenBordersAttitudeChangeLimit())), abs(GC.getLeaderHeadInfo(getPersonalityType()).getOpenBordersAttitudeChangeLimit()));
		}
	}

	return 0;
}


int CvPlayerAI::AI_getDefensivePactAttitude(PlayerTypes ePlayer)
{
	if (!atWar(getTeam(), GET_PLAYER(ePlayer).getTeam()))
	{
		if (GC.getLeaderHeadInfo(getPersonalityType()).getDefensivePactAttitudeDivisor() != 0)
		{
			int iAttitudeChange = (GET_TEAM(getTeam()).AI_getDefensivePactCounter(GET_PLAYER(ePlayer).getTeam()) / GC.getLeaderHeadInfo(getPersonalityType()).getDefensivePactAttitudeDivisor());
			return range(iAttitudeChange, -(abs(GC.getLeaderHeadInfo(getPersonalityType()).getDefensivePactAttitudeChangeLimit())), abs(GC.getLeaderHeadInfo(getPersonalityType()).getDefensivePactAttitudeChangeLimit()));
		}
	}

	return 0;
}


int CvPlayerAI::AI_getRivalDefensivePactAttitude(PlayerTypes ePlayer)
{
	int iAttitude = 0;

	if (getTeam() == GET_PLAYER(ePlayer).getTeam())
	{
		return iAttitude;
	}

	if (!(GET_TEAM(getTeam()).isDefensivePact(GET_PLAYER(ePlayer).getTeam())))
	{
		iAttitude -= ((4 * GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getDefensivePactCount(GET_PLAYER(ePlayer).getTeam())) / std::max(1, (GC.getGameINLINE().countCivTeamsAlive() - 2)));
	}

	return iAttitude;
}


int CvPlayerAI::AI_getShareWarAttitude(PlayerTypes ePlayer)
{
	int iAttitudeChange;
	int iAttitude;

	iAttitude = 0;

	if (!atWar(getTeam(), GET_PLAYER(ePlayer).getTeam()))
	{
		if (GET_TEAM(getTeam()).AI_shareWar(GET_PLAYER(ePlayer).getTeam()))
		{
			iAttitude += GC.getLeaderHeadInfo(getPersonalityType()).getShareWarAttitudeChange();
		}

		if (GC.getLeaderHeadInfo(getPersonalityType()).getShareWarAttitudeDivisor() != 0)
		{
			iAttitudeChange = (GET_TEAM(getTeam()).AI_getShareWarCounter(GET_PLAYER(ePlayer).getTeam()) / GC.getLeaderHeadInfo(getPersonalityType()).getShareWarAttitudeDivisor());
			iAttitude += range(iAttitudeChange, -(abs(GC.getLeaderHeadInfo(getPersonalityType()).getShareWarAttitudeChangeLimit())), abs(GC.getLeaderHeadInfo(getPersonalityType()).getShareWarAttitudeChangeLimit()));
		}
	}

	return iAttitude;
}

int CvPlayerAI::AI_getTradeAttitude(PlayerTypes ePlayer)
{
	// XXX human only?
	return range(((AI_getPeacetimeGrantValue(ePlayer) + std::max(0, (AI_getPeacetimeTradeValue(ePlayer) - GET_PLAYER(ePlayer).AI_getPeacetimeTradeValue(getID())))) / ((GET_TEAM(getTeam()).AI_getHasMetCounter(GET_PLAYER(ePlayer).getTeam()) + 1) * 5)), 0, 4);
}

int CvPlayerAI::AI_getRivalTradeAttitude(PlayerTypes ePlayer)
{
	// XXX human only?
	return -(range(((GET_TEAM(getTeam()).AI_getEnemyPeacetimeGrantValue(GET_PLAYER(ePlayer).getTeam()) + (GET_TEAM(getTeam()).AI_getEnemyPeacetimeTradeValue(GET_PLAYER(ePlayer).getTeam()) / 3)) / ((GET_TEAM(getTeam()).AI_getHasMetCounter(GET_PLAYER(ePlayer).getTeam()) + 1) * 10)), 0, 4));
}


int CvPlayerAI::AI_getMemoryAttitude(PlayerTypes ePlayer, MemoryTypes eMemory)
{
	return ((AI_getMemoryCount(ePlayer, eMemory) * GC.getLeaderHeadInfo(getPersonalityType()).getMemoryAttitudePercent(eMemory)) / 100);
}

int CvPlayerAI::AI_dealVal(PlayerTypes ePlayer, const CLinkList<TradeData>* pList, bool bIgnoreAnnual, int iChange)
{
	int iValue = 0;

	FAssertMsg(ePlayer != getID(), "shouldn't call this function on ourselves");

	if (atWar(getTeam(), GET_PLAYER(ePlayer).getTeam()))
	{
		iValue += GET_TEAM(getTeam()).AI_endWarVal(GET_PLAYER(ePlayer).getTeam());
	}

	for (CLLNode<TradeData>* pNode = pList->head(); pNode; pNode = pList->next(pNode))
	{
		FAssertMsg(!(pNode->m_data.m_bHidden), "(pNode->m_data.m_bHidden) did not return false as expected");

		switch (pNode->m_data.m_eItemType)
		{
		case TRADE_CITIES:
			iValue += AI_cityTradeVal(GET_PLAYER(ePlayer).getCity(pNode->m_data.m_iData1));
			break;
		case TRADE_GOLD:
			iValue += (pNode->m_data.m_iData1 * AI_goldTradeValuePercent()) / 100;
			break;
		case TRADE_YIELD:
			iValue += AI_yieldTradeVal((YieldTypes) pNode->m_data.m_iData1, pNode->m_data.m_kTransport, ePlayer);
			break;
		case TRADE_MAPS:
			iValue += GET_TEAM(getTeam()).AI_mapTradeVal(GET_PLAYER(ePlayer).getTeam());
			break;
		case TRADE_OPEN_BORDERS:
			iValue += GET_TEAM(getTeam()).AI_openBordersTradeVal(GET_PLAYER(ePlayer).getTeam());
			break;
		case TRADE_DEFENSIVE_PACT:
			iValue += GET_TEAM(getTeam()).AI_defensivePactTradeVal(GET_PLAYER(ePlayer).getTeam());
			break;
		case TRADE_PEACE:
			iValue += GET_TEAM(getTeam()).AI_makePeaceTradeVal(((TeamTypes)(pNode->m_data.m_iData1)), GET_PLAYER(ePlayer).getTeam());
			break;
		case TRADE_WAR:
			iValue += GET_TEAM(getTeam()).AI_declareWarTradeVal(((TeamTypes)(pNode->m_data.m_iData1)), GET_PLAYER(ePlayer).getTeam());
			break;
		case TRADE_EMBARGO:
			iValue += AI_stopTradingTradeVal(((TeamTypes)(pNode->m_data.m_iData1)), ePlayer);
			break;
		}
	}

	return iValue;
}


bool CvPlayerAI::AI_goldDeal(const CLinkList<TradeData>* pList)
{
	CLLNode<TradeData>* pNode;

	for (pNode = pList->head(); pNode; pNode = pList->next(pNode))
	{
		FAssert(!(pNode->m_data.m_bHidden));

		switch (pNode->m_data.m_eItemType)
		{
		case TRADE_GOLD:
			return true;
			break;
		}
	}

	return false;
}


bool CvPlayerAI::AI_considerOffer(PlayerTypes ePlayer, const CLinkList<TradeData>* pTheirList, const CLinkList<TradeData>* pOurList, int iChange)
{
	CLLNode<TradeData>* pNode;
	int iThreshold;

	FAssertMsg(ePlayer != getID(), "shouldn't call this function on ourselves");

	if (AI_goldDeal(pTheirList) && AI_goldDeal(pOurList))
	{
		return false;
	}

	if (iChange > -1)
	{
		for (pNode = pOurList->head(); pNode; pNode = pOurList->next(pNode))
		{
			if (getTradeDenial(ePlayer, pNode->m_data) != NO_DENIAL)
			{
				return false;
			}
		}
	}

	if (GET_PLAYER(ePlayer).getTeam() == getTeam())
	{
		return true;
	}

	if ((pOurList->getLength() == 0) && (pTheirList->getLength() > 0))
	{
		return true;
	}

	int iOurValue = GET_PLAYER(ePlayer).AI_dealVal(getID(), pOurList, false, iChange);
	int iTheirValue = AI_dealVal(ePlayer, pTheirList, false, iChange);

	if (iOurValue > 0 && 0 == pTheirList->getLength() && 0 == iTheirValue)
	{
		if (AI_getAttitude(ePlayer) < ATTITUDE_PLEASED)
		{
			if (GET_TEAM(getTeam()).getPower() > ((GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getPower() * 4) / 3))
			{
				return false;
			}
		}

		if (AI_getMemoryCount(ePlayer, MEMORY_MADE_DEMAND_RECENT) > 0)
		{
			return false;
		}

		iThreshold = (GET_TEAM(getTeam()).AI_getHasMetCounter(GET_PLAYER(ePlayer).getTeam()) + 50);

		iThreshold *= 2;

		if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).AI_isLandTarget(getTeam()))
		{
			iThreshold *= 3;
		}

		iThreshold *= (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getPower() + 100);
		iThreshold /= (GET_TEAM(getTeam()).getPower() + 100);

		iThreshold -= GET_PLAYER(ePlayer).AI_getPeacetimeGrantValue(getID());

		return (iOurValue < iThreshold);
	}

	if (iChange < 0)
	{
		return (iTheirValue * 110 >= iOurValue * 100);
	}

	return (iTheirValue >= iOurValue);
}

int CvPlayerAI::AI_militaryHelp(PlayerTypes ePlayer, int& iNumUnits, UnitTypes& eUnit, ProfessionTypes& eProfession)
{
	FAssert(GET_PLAYER(ePlayer).getParent() == getID());

	iNumUnits = 0;
	eUnit = NO_UNIT;
	eProfession = NO_PROFESSION;

	if (AI_getAttitude(ePlayer) < ATTITUDE_CAUTIOUS)
	{
		return -1;
	}

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	int iBestValue = MAX_INT;
	for (int i = 0; i < GC.getNumUnitClassInfos(); ++i)
	{
		UnitTypes eLoopUnit = (UnitTypes) GC.getCivilizationInfo(kPlayer.getCivilizationType()).getCivilizationUnits(i);
		if (eLoopUnit != NO_UNIT)
		{

			//WTP, Ray, fixing Unit Variation for Buying Units from King - START
			CvUnitInfo& kUnit = GC.getUnitInfo(eLoopUnit);

			if (kUnit.getDomainType() == DOMAIN_LAND && kPlayer.getEuropeUnitBuyPrice(eLoopUnit) > 0)
			{
				// check Combat Value of Unit
				bool isValidbyUnitCombat = (kUnit.getCombat() > 5);

				// check Combat Change From Profession
				ProfessionTypes checkProfession = (ProfessionTypes) GC.getUnitInfo(eLoopUnit).getDefaultProfession();
				bool isValidbyDefaultProfessionCombatChange = false;
				if (checkProfession != NO_PROFESSION)
				{
					isValidbyDefaultProfessionCombatChange = (GC.getProfessionInfo(checkProfession).getCombatChange() > 3);
				}

				bool bValid = (isValidbyUnitCombat || isValidbyDefaultProfessionCombatChange);
				//WTP, Ray, fixing Unit Variation for Buying Units from King - END

				//WTP, Ray, replaced because otherwise King gives Medics, which also have Promotions
				/*
				for (int j = 0; j < GC.getNumPromotionInfos() && !bValid; ++j)
				{
					if (kUnit.getFreePromotions(j))
					{
						bValid = true;
					}
				}
				*/

				if (bValid)
				{
					int iValue = kPlayer.getUnitClassCount((UnitClassTypes) i);
					if (iValue < iBestValue)
					{
						iBestValue = iValue;
						eUnit = eLoopUnit;
						//WTP, Ray, fixing Unit Variation for Buying Units from King - END
						eProfession = (ProfessionTypes) GC.getUnitInfo(eLoopUnit).getDefaultProfession();
					}
				}
			}
		}
	}

	if (eUnit == NO_UNIT)
	{
		return -1;
	}

	iNumUnits = 1;
	//WTP, Ray, fixing Unit Variation for Buying Units from King - END
	//eProfession = (ProfessionTypes) GC.getUnitInfo(eUnit).getDefaultProfession();

	return kPlayer.getEuropeUnitBuyPrice(eUnit) * GC.getDefineINT("KING_BUY_UNIT_PRICE_MODIFIER") / 100;
}

bool CvPlayerAI::AI_counterPropose(PlayerTypes ePlayer, const CLinkList<TradeData>* pTheirList, const CLinkList<TradeData>* pOurList, CLinkList<TradeData>* pTheirInventory, CLinkList<TradeData>* pOurInventory, CLinkList<TradeData>* pTheirCounter, CLinkList<TradeData>* pOurCounter, const IDInfo& kTransport)
{

	CLLNode<TradeData>* pNode;
	CLLNode<TradeData>* pBestNode;
	CLLNode<TradeData>* pGoldNode;
	CvCity* pCity;
	bool bTheirGoldDeal;
	bool bOurGoldDeal;
	int iHumanDealWeight;
	int iAIDealWeight;
	int iGoldData;
	int iGoldWeight;
	int iWeight;
	int iBestWeight;
	int iBestValue;

	bTheirGoldDeal = AI_goldDeal(pTheirList);
	bOurGoldDeal = AI_goldDeal(pOurList);


	// R&R, ray, Bargaining - START
	bool acceptsBargaining = GET_PLAYER(getID()).isWillingToBargain();

	if(acceptsBargaining)
	{
		if (bOurGoldDeal)
		{
			int priceIncreaseMax = GC.getDefineINT("PRICE_INCREASE_BARGAIN_SELL");
			pNode = pOurInventory->head();
			if (pNode->m_data.m_eItemType == TRADE_GOLD)
			{
				int oldPrice = pNode->m_data.m_iData1;
				int randomPriceChange = GC.getGameINLINE().getSorenRandNum(priceIncreaseMax, "Natives Price Change Sell");
				if (randomPriceChange < priceIncreaseMax / 3)
				{
					randomPriceChange = priceIncreaseMax / 3;
				}
				int newPrice = oldPrice * (1000 + randomPriceChange) / 1000;

				// R&R, ray, change for Trait Trader
				newPrice = newPrice * (100 + GET_PLAYER(getID()).getNativeTradeModifier()) / 100;

				// R&R, ray, small correction to stop at max gold of player
				int iGoldAvailable = GET_PLAYER(getID()).AI_maxGoldTrade(ePlayer);
				if (newPrice >= iGoldAvailable)
				{
					newPrice = iGoldAvailable;
				}

				//setting value to new price
				pOurCounter->clear();
				CLLNode<TradeData>* newNode = pNode;
				newNode->m_data.m_iData1 = newPrice;
				pOurCounter->insertAtEnd(newNode->m_data);
			}
		}

		else if (bTheirGoldDeal)
		{
			int priceDecreaseMax = GC.getDefineINT("PRICE_DECREASE_BARGAIN_BUY");
			pNode = pTheirInventory->head();
			if (pNode->m_data.m_eItemType == TRADE_GOLD)
			{
				int oldPrice = pNode->m_data.m_iData1;
				int randomPriceChange = GC.getGameINLINE().getSorenRandNum(priceDecreaseMax, "Natives Price Change Buy");
				if (randomPriceChange < priceDecreaseMax / 3)
				{
					randomPriceChange = priceDecreaseMax / 3;
				}
				int newPrice = oldPrice * (1000 - randomPriceChange) / 1000;

				// R&R, ray, change for Trait Trader
				newPrice = newPrice * (100 - GET_PLAYER(getID()).getNativeTradeModifier()) / 100;

				//setting value to new price
				pTheirCounter->clear();
				CLLNode<TradeData>* newNode = pNode;
				newNode->m_data.m_iData1 = newPrice;
				pTheirCounter->insertAtEnd(newNode->m_data);
			}
		}

		//at the end reset value for Bargaining
		GET_PLAYER(getID()).setWillingToBargain(false);
		return true;
	}
	// R&R, ray, Bargaining - END

	if (bOurGoldDeal && bTheirGoldDeal)
	{	
		return false;
	}

	pGoldNode = NULL;

	iHumanDealWeight = AI_dealVal(ePlayer, pTheirList);
	iAIDealWeight = GET_PLAYER(ePlayer).AI_dealVal(getID(), pOurList);

	int iGoldValuePercent = AI_goldTradeValuePercent();

	pTheirCounter->clear();
	pOurCounter->clear();

	if (iAIDealWeight > iHumanDealWeight)
	{
		if (atWar(getTeam(), GET_PLAYER(ePlayer).getTeam()))
		{
			iBestValue = 0;
			iBestWeight = 0;
			pBestNode = NULL;

			for (pNode = pTheirInventory->head(); pNode && iAIDealWeight > iHumanDealWeight; pNode = pTheirInventory->next(pNode))
			{
				if (!pNode->m_data.m_bOffering && !pNode->m_data.m_bHidden)
				{
					if (pNode->m_data.m_eItemType == TRADE_CITIES)
					{
						FAssert(GET_PLAYER(ePlayer).canTradeItem(getID(), pNode->m_data));

						if (GET_PLAYER(ePlayer).getTradeDenial(getID(), pNode->m_data) == NO_DENIAL)
						{
							pCity = GET_PLAYER(ePlayer).getCity(pNode->m_data.m_iData1);

							if (pCity != NULL)
							{
								iWeight = AI_cityTradeVal(pCity);

								if (iWeight > 0)
								{
									const int iValue = AI_targetCityValue(pCity, false);

									if (iValue > iBestValue)
									{
										iBestValue = iValue;
										iBestWeight = iWeight;
										pBestNode = pNode;
									}
								}
							}
						}
					}
				}
			}

			if (pBestNode != NULL)
			{
				iHumanDealWeight += iBestWeight;
				pTheirCounter->insertAtEnd(pBestNode->m_data);
			}
		}

		for (pNode = pTheirInventory->head(); pNode && iAIDealWeight > iHumanDealWeight; pNode = pTheirInventory->next(pNode))
		{
			if (!pNode->m_data.m_bOffering && !pNode->m_data.m_bHidden)
			{
				FAssert(GET_PLAYER(ePlayer).canTradeItem(getID(), pNode->m_data));

				if (GET_PLAYER(ePlayer).getTradeDenial(getID(), pNode->m_data) == NO_DENIAL)
				{
					switch (pNode->m_data.m_eItemType)
					{
					case TRADE_GOLD:
						if (!bOurGoldDeal)
						{
							pGoldNode = pNode;
						}
						break;
					}
				}
			}
		}

		int iGoldWeight = iAIDealWeight - iHumanDealWeight;

		if (iGoldWeight > 0)
		{
			if (pGoldNode)
			{
				iGoldData = iGoldWeight * 100;
				iGoldData /= iGoldValuePercent;
				if ((iGoldData * iGoldValuePercent) < iGoldWeight)
				{
					iGoldData++;
				}
				if (GET_PLAYER(ePlayer).getMaxGoldTrade(getID(), kTransport) >= iGoldData)
				{
					pGoldNode->m_data.m_iData1 = iGoldData;
					iHumanDealWeight += (iGoldData * iGoldValuePercent) / 100;
					pTheirCounter->insertAtEnd(pGoldNode->m_data);
					pGoldNode = NULL;
				}
			}
		}

		for (pNode = pTheirInventory->head(); pNode && iAIDealWeight > iHumanDealWeight; pNode = pTheirInventory->next(pNode))
		{
			if (!pNode->m_data.m_bOffering && !pNode->m_data.m_bHidden)
			{
				if (pNode->m_data.m_eItemType == TRADE_MAPS)
				{
					FAssert(GET_PLAYER(ePlayer).canTradeItem(getID(), pNode->m_data));

					if (GET_PLAYER(ePlayer).getTradeDenial(getID(), pNode->m_data) == NO_DENIAL)
					{
						iWeight = GET_TEAM(getTeam()).AI_mapTradeVal(GET_PLAYER(ePlayer).getTeam());

						if (iWeight > 0)
						{
							iHumanDealWeight += iWeight;
							pTheirCounter->insertAtEnd(pNode->m_data);
						}
					}
				}
			}
		}

		iGoldWeight = iAIDealWeight - iHumanDealWeight;

		if (iGoldWeight > 0)
		{
			if (pGoldNode)
			{
				iGoldData = iGoldWeight * 100;
				iGoldData /= iGoldValuePercent;

				if ((iGoldWeight * 100) > (iGoldData * iGoldValuePercent))
				{
					iGoldData++;
				}

				iGoldData = std::min(iGoldData, GET_PLAYER(ePlayer).getMaxGoldTrade(getID(), kTransport));

				if (iGoldData > 0)
				{
					pGoldNode->m_data.m_iData1 = iGoldData;
					iHumanDealWeight += (iGoldData * iGoldValuePercent) / 100;
					pTheirCounter->insertAtEnd(pGoldNode->m_data);
					pGoldNode = NULL;
				}
			}
		}
	}
	else if (iHumanDealWeight > iAIDealWeight)
	{
		if (atWar(getTeam(), GET_PLAYER(ePlayer).getTeam()))
		{
				for (pNode = pOurInventory->head(); pNode; pNode = pOurInventory->next(pNode))
				{
					if (!pNode->m_data.m_bOffering && !pNode->m_data.m_bHidden)
					{
						if (pNode->m_data.m_eItemType == TRADE_PEACE_TREATY)
						{
							pOurCounter->insertAtEnd(pNode->m_data);
							break;
						}
					}
				}

			iBestValue = 0;
			iBestWeight = 0;
			pBestNode = NULL;

			for (pNode = pOurInventory->head(); pNode && iHumanDealWeight > iAIDealWeight; pNode = pOurInventory->next(pNode))
			{
				if (!pNode->m_data.m_bOffering && !pNode->m_data.m_bHidden)
				{
					if (pNode->m_data.m_eItemType == TRADE_CITIES)
					{
						FAssert(canTradeItem(ePlayer, pNode->m_data));

						if (getTradeDenial(ePlayer, pNode->m_data) == NO_DENIAL)
						{
							pCity = getCity(pNode->m_data.m_iData1);

							if (pCity != NULL)
							{
								iWeight = GET_PLAYER(ePlayer).AI_cityTradeVal(pCity);

								if (iWeight > 0)
								{
									const int iValue = GET_PLAYER(ePlayer).AI_targetCityValue(pCity, false);

									if (iValue > iBestValue)
									{
										if (iHumanDealWeight >= (iAIDealWeight + iWeight))
										{
											iBestValue = iValue;
											iBestWeight = iWeight;
											pBestNode = pNode;
										}
									}
								}
							}
						}
					}
				}
			}

			if (pBestNode != NULL)
			{
				iAIDealWeight += iBestWeight;
				pOurCounter->insertAtEnd(pBestNode->m_data);
			}
		}

		for (pNode = pOurInventory->head(); pNode && iHumanDealWeight > iAIDealWeight; pNode = pOurInventory->next(pNode))
		{
			if (!pNode->m_data.m_bOffering && !pNode->m_data.m_bHidden)
			{
				FAssert(canTradeItem(ePlayer, pNode->m_data));

				if (getTradeDenial(ePlayer, pNode->m_data) == NO_DENIAL)
				{
					switch (pNode->m_data.m_eItemType)
					{
					case TRADE_GOLD:
						if (!bTheirGoldDeal)
						{
							pGoldNode = pNode;
						}
						break;
					}
				}
			}
		}

		iGoldWeight = iHumanDealWeight - iAIDealWeight;

		if (iGoldWeight > 0)
		{
			if (pGoldNode)
			{
				int iGoldData = iGoldWeight * 100;
				iGoldData /= iGoldValuePercent;

				if (getMaxGoldTrade(ePlayer, kTransport) >= iGoldData)
				{
					pGoldNode->m_data.m_iData1 = iGoldData;
					iAIDealWeight += ((iGoldData * iGoldValuePercent) / 100);
					pOurCounter->insertAtEnd(pGoldNode->m_data);
					pGoldNode = NULL;
				}
			}
		}

		for (pNode = pOurInventory->head(); pNode && iHumanDealWeight > iAIDealWeight; pNode = pOurInventory->next(pNode))
		{
			if (!pNode->m_data.m_bOffering && !pNode->m_data.m_bHidden)
			{
				if (pNode->m_data.m_eItemType == TRADE_MAPS)
				{
					FAssert(canTradeItem(ePlayer, pNode->m_data));

					if (getTradeDenial(ePlayer, pNode->m_data) == NO_DENIAL)
					{
						iWeight = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).AI_mapTradeVal(getTeam());

						if (iWeight > 0)
						{
							if (iHumanDealWeight >= (iAIDealWeight + iWeight))
							{
								iAIDealWeight += iWeight;
								pOurCounter->insertAtEnd(pNode->m_data);
							}
						}
					}
				}
			}
		}

		iGoldWeight = iHumanDealWeight - iAIDealWeight;
		if (iGoldWeight > 0)
		{
			if (pGoldNode)
			{
				iGoldData = iGoldWeight * 100;
				iGoldData /= AI_goldTradeValuePercent();

				iGoldData = std::min(iGoldData, getMaxGoldTrade(ePlayer, kTransport));

				if (iGoldData > 0)
				{
					pGoldNode->m_data.m_iData1 = iGoldData;
					iAIDealWeight += (iGoldData * AI_goldTradeValuePercent()) / 100;
					pOurCounter->insertAtEnd(pGoldNode->m_data);
					pGoldNode = NULL;
				}
			}
		}
	}

	return ((iAIDealWeight <= iHumanDealWeight) && ((pOurList->getLength() > 0) || (pOurCounter->getLength() > 0) || (pTheirCounter->getLength() > 0)));
}


int CvPlayerAI::AI_maxGoldTrade(PlayerTypes ePlayer) const
{
	int iMaxGold;

	FAssert(ePlayer != getID());

	if (isHuman() || (GET_PLAYER(ePlayer).getTeam() == getTeam()))
	{
		iMaxGold = getGold();
	}
	else
	{
		iMaxGold = getTotalPopulation() * 10;

		iMaxGold *= (GET_TEAM(getTeam()).AI_getHasMetCounter(GET_PLAYER(ePlayer).getTeam()) + 10);

		iMaxGold *= GC.getLeaderHeadInfo(getPersonalityType()).getMaxGoldTradePercent();
		iMaxGold /= 100;

		iMaxGold -= AI_getGoldTradedTo(ePlayer);
		iMaxGold += GET_PLAYER(ePlayer).AI_getGoldTradedTo(getID());

		iMaxGold = std::min(iMaxGold, getGold());

		iMaxGold -= (iMaxGold % GC.getDefineINT("DIPLOMACY_VALUE_REMAINDER"));
	}

	return std::max(0, iMaxGold);
}

int CvPlayerAI::AI_cityTradeVal(CvCity* pCity, PlayerTypes eOwner)
{
	if (pCity == NULL)
	{
		return 0;
	}

	if (eOwner == NO_PLAYER)
	{
		eOwner = pCity->getOwnerINLINE();
	}
	FAssert(eOwner != getID());

	int iValue = 300;

	iValue += (pCity->getPopulation() * 50);
	iValue += (pCity->getCultureLevel() * 200);
	iValue += (((((pCity->getPopulation() * 50) + GC.getGameINLINE().getElapsedGameTurns() + 100) * 4) * pCity->plot()->calculateCulturePercent(eOwner)) / 100);

	if (!(pCity->isEverOwned(getID())))
	{
		iValue *= 3;
		iValue /= 2;
	}

	iValue -= (iValue % GC.getDefineINT("DIPLOMACY_VALUE_REMAINDER"));

	if (isHuman())
	{
		return std::max(iValue, GC.getDefineINT("DIPLOMACY_VALUE_REMAINDER"));
	}
	else
	{
		return iValue;
	}
}


DenialTypes CvPlayerAI::AI_cityTrade(CvCity* pCity, PlayerTypes ePlayer) const
{
	FAssert(pCity->getOwnerINLINE() == getID());

	if (pCity->getLiberationPlayer(false) == ePlayer)
	{
		return NO_DENIAL;
	}

	if (!(GET_PLAYER(ePlayer).isHuman()))
	{
		if (GET_PLAYER(ePlayer).getTeam() != getTeam())
		{
			if ((pCity->plot()->calculateCulturePercent(ePlayer) == 0) && !(pCity->isEverOwned(ePlayer)) && (GET_PLAYER(ePlayer).getNumCities() > 3))
			{
				CvCity* pNearestCity = GC.getMapINLINE().findCity(pCity->getX_INLINE(), pCity->getY_INLINE(), ePlayer, NO_TEAM, true, false, NO_TEAM, NO_DIRECTION, pCity);
				if ((pNearestCity == NULL) || (plotDistance(pCity->getX_INLINE(), pCity->getY_INLINE(), pNearestCity->getX_INLINE(), pNearestCity->getY_INLINE()) > 9))
				{
					return DENIAL_UNKNOWN;
				}
			}
		}
	}

	if (isHuman())
	{
		return NO_DENIAL;
	}

	if (atWar(getTeam(), GET_PLAYER(ePlayer).getTeam()))
	{
		return NO_DENIAL;
	}

	if (isNative() && !GET_PLAYER(ePlayer).isNative())
	{
		return NO_DENIAL;
	}

	if (GET_PLAYER(ePlayer).getTeam() != getTeam())
	{
		return DENIAL_NEVER;
	}

	if (pCity->calculateCulturePercent(getID()) > 50)
	{
		return DENIAL_TOO_MUCH;
	}

	return NO_DENIAL;
}


int CvPlayerAI::AI_stopTradingTradeVal(TeamTypes eTradeTeam, PlayerTypes ePlayer)
{
	CvDeal* pLoopDeal;
	int iModifier;
	int iValue;
	int iLoop;

	FAssertMsg(ePlayer != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(GET_PLAYER(ePlayer).getTeam() != getTeam(), "shouldn't call this function on ourselves");
	FAssertMsg(eTradeTeam != getTeam(), "shouldn't call this function on ourselves");
	FAssertMsg(GET_TEAM(eTradeTeam).isAlive(), "GET_TEAM(eWarTeam).isAlive is expected to be true");
	FAssertMsg(!atWar(eTradeTeam, GET_PLAYER(ePlayer).getTeam()), "eTeam should be at peace with eWarTeam");

	iValue = (50 + (GC.getGameINLINE().getGameTurn() / 2));
	iValue += (GET_TEAM(eTradeTeam).getNumCities() * 5);

	iModifier = 0;

	switch (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).AI_getAttitude(eTradeTeam))
	{
	case ATTITUDE_FURIOUS:
		break;

	case ATTITUDE_ANNOYED:
		iModifier += 25;
		break;

	case ATTITUDE_CAUTIOUS:
		iModifier += 50;
		break;

	case ATTITUDE_PLEASED:
		iModifier += 100;
		break;

	case ATTITUDE_FRIENDLY:
		iModifier += 200;
		break;

	default:
		FAssert(false);
		break;
	}

	iValue *= std::max(0, (iModifier + 100));
	iValue /= 100;

	if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isOpenBorders(eTradeTeam))
	{
		iValue *= 2;
	}

	if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isDefensivePact(eTradeTeam))
	{
		iValue *= 3;
	}

	for(pLoopDeal = GC.getGameINLINE().firstDeal(&iLoop); pLoopDeal != NULL; pLoopDeal = GC.getGameINLINE().nextDeal(&iLoop))
	{
		if (pLoopDeal->isCancelable(getID()) && !(pLoopDeal->isPeaceDeal()))
		{
			if (GET_PLAYER(pLoopDeal->getFirstPlayer()).getTeam() == GET_PLAYER(ePlayer).getTeam())
			{
				if (pLoopDeal->getLengthSecondTrades() > 0)
				{
					iValue += (GET_PLAYER(pLoopDeal->getFirstPlayer()).AI_dealVal(pLoopDeal->getSecondPlayer(), pLoopDeal->getSecondTrades()) * ((pLoopDeal->getLengthFirstTrades() == 0) ? 2 : 1));
				}
			}

			if (GET_PLAYER(pLoopDeal->getSecondPlayer()).getTeam() == GET_PLAYER(ePlayer).getTeam())
			{
				if (pLoopDeal->getLengthFirstTrades() > 0)
				{
					iValue += (GET_PLAYER(pLoopDeal->getSecondPlayer()).AI_dealVal(pLoopDeal->getFirstPlayer(), pLoopDeal->getFirstTrades()) * ((pLoopDeal->getLengthSecondTrades() == 0) ? 2 : 1));
				}
			}
		}
	}

	iValue -= (iValue % GC.getDefineINT("DIPLOMACY_VALUE_REMAINDER"));

	if (isHuman())
	{
		return std::max(iValue, GC.getDefineINT("DIPLOMACY_VALUE_REMAINDER"));
	}
	else
	{
		return iValue;
	}
}


DenialTypes CvPlayerAI::AI_stopTradingTrade(TeamTypes eTradeTeam, PlayerTypes ePlayer) const
{
	AttitudeTypes eAttitude;
	AttitudeTypes eAttitudeThem;
	int iI;

	FAssertMsg(ePlayer != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(GET_PLAYER(ePlayer).getTeam() != getTeam(), "shouldn't call this function on ourselves");
	FAssertMsg(eTradeTeam != getTeam(), "shouldn't call this function on ourselves");
	FAssertMsg(GET_TEAM(eTradeTeam).isAlive(), "GET_TEAM(eTradeTeam).isAlive is expected to be true");
	FAssertMsg(!atWar(getTeam(), eTradeTeam), "should be at peace with eTradeTeam");

	if (isHuman())
	{
		return NO_DENIAL;
	}

	eAttitude = GET_TEAM(getTeam()).AI_getAttitude(GET_PLAYER(ePlayer).getTeam());

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getTeam())
			{
				if (eAttitude <= GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getStopTradingRefuseAttitudeThreshold())
				{
					return DENIAL_ATTITUDE;
				}
			}
		}
	}

	eAttitudeThem = GET_TEAM(getTeam()).AI_getAttitude(eTradeTeam);

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getTeam())
			{
				if (eAttitudeThem > GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getStopTradingThemRefuseAttitudeThreshold())
				{
					return DENIAL_ATTITUDE_THEM;
				}
			}
		}
	}

	return NO_DENIAL;
}

int CvPlayerAI::AI_yieldTradeVal(YieldTypes eYield, const IDInfo& kTransport, PlayerTypes ePlayer)
{
	int iValue = 0;
	CvPlayerAI& kTradePlayer = GET_PLAYER(ePlayer);
	CvUnit* pTransport = ::getUnit(kTransport);
	if (pTransport != NULL)
	{
		int iAmount = kTradePlayer.getTradeYieldAmount(eYield, pTransport);
		if (isNative())
		{
			int iTotalStored = countTotalYieldStored(eYield);
			int iMaxStored;
			if (eYield == YIELD_FOOD)
			{
				iMaxStored = getNumCities() * getGrowthThreshold(1);
			}
			else
			{
				iMaxStored = getNumCities() * GC.getGameINLINE().getCargoYieldCapacity();
			}

			int iNativeConsumptionPercent = GC.getYieldInfo(eYield).getNativeConsumptionPercent();
			if (iNativeConsumptionPercent > 0)
			{
				iMaxStored *= iNativeConsumptionPercent;
				iMaxStored /= 80;
			}
			else
			{
				iMaxStored *= 2;
			}
			int iBuyPrice = GC.getYieldInfo(eYield).getNativeBuyPrice();
			CvCity* pCity = pTransport->plot()->getPlotCity();
			if (pCity != NULL)
			{
				if (eYield == pCity->AI_getDesiredYield() || eYield == YIELD_MUSKETS || eYield == YIELD_HORSES)
				{
					iBuyPrice *= 125;
					iBuyPrice /= 100;
				}
			}

			int iHighPricePercent = std::max(25, 100 - ((100 * iTotalStored) / iMaxStored));
			int iLowPricePercent = std::max(25, 100 - ((100 * std::min(iMaxStored, iTotalStored + iAmount)) / iMaxStored));

			iValue += (iAmount * iBuyPrice * (iHighPricePercent + iLowPricePercent)) / 200;
		}
		else if (kTradePlayer.isNative())
		{
			int iTotalStored = kTradePlayer.countTotalYieldStored(eYield);
			int iMaxStored;
			if (eYield == YIELD_FOOD)
			{
				iMaxStored = kTradePlayer.getNumCities() * getGrowthThreshold(1);
			}
			else
			{
				iMaxStored = kTradePlayer.getNumCities() * GC.getGameINLINE().getCargoYieldCapacity();
			}

			int iNativeConsumptionPercent = GC.getYieldInfo(eYield).getNativeConsumptionPercent();
			if (iNativeConsumptionPercent > 0)
			{
				iMaxStored *= iNativeConsumptionPercent;
				iMaxStored /= 80;
			}
			else
			{
				iMaxStored *= 2;
			}
			int iSellPrice = GC.getYieldInfo(eYield).getNativeSellPrice();

			int iHighPricePercent = std::max(25, 100 - ((100 * iTotalStored) / std::max(1, iMaxStored)));
			int iLowPricePercent = std::max(25, 100 - ((100 * std::max(0, iTotalStored - iAmount)) / std::max(1, iMaxStored)));

			iValue += (iAmount * iSellPrice * (iHighPricePercent + iLowPricePercent)) / 200;
		}
		else
		{
			CvCity* pCity = pTransport->plot()->getPlotCity();
			if (pCity != NULL)
			{
				/// Ist die Stadt vom Verkäufer?
				if (ePlayer == pCity->getOwnerINLINE())
				{
					/// Wird das angefragt Handelsgut (von der KI) zur Weiterverarbeitung gebraucht?
					if (kTradePlayer.AI_isYieldFinalProduct(eYield))
					{
						iValue += kTradePlayer.getSellToEuropeProfit(eYield, iAmount) * 95 / 100;
					}
					else
					{
						/// Das Gut wird zur Weiterverarbeitung gebraucht, daher überlegt sich die
						/// KI einen angemessenen Preis
						iValue += kTradePlayer.AI_yieldValue(eYield, true, iAmount);
					}
				}
				else
				{
					/// Verkäufer ist nicht Besitzer der Stadt (Ich verkaufe der KI)
					iValue += kTradePlayer.AI_yieldValue(eYield, false, iAmount);

					if ( AI_isYieldFinalProduct(eYield) )
					{
						iValue = std::min(iValue, getSellToEuropeProfit(eYield, iAmount) * 90 / 100);
					}
					else
					{
						/// Wenn ein kein Endprodukt ist bezahlt der Käufer höchsten 90% von dem für den Preis er es selbst verkaufen würde
						iValue = std::min(iValue, AI_yieldValue(eYield, true, iAmount) * 90 / 100);
					}
				}
			}
		}
	}
	// TAC - Endlos-Handelsbug Fix - Netbandit - DIV-1.06 - ENDE

	return iValue;
}

DenialTypes CvPlayerAI::AI_yieldTrade(YieldTypes eYield, const IDInfo& kTransport, PlayerTypes ePlayer) const
{
	CvUnit* pTransport = ::getUnit(kTransport);
	CvCity* pCity = pTransport->plot()->getPlotCity();
	if (pCity != NULL)
	{
		CvPlayer& kPlayer = GET_PLAYER(pCity->getOwnerINLINE());
		if (kPlayer.isNative())
		{
			if (getID() == pCity->getOwnerINLINE())
			{
				if (GC.getYieldInfo(eYield).getNativeSellPrice() == -1)
				{
					return DENIAL_NEVER;
				}
			}
			else
			{
				if (pCity->AI_getDesiredYield() == eYield)
				{
					return NO_DENIAL;
				}
				if (GC.getYieldInfo(eYield).getNativeBuyPrice() == -1)
				{
					return DENIAL_NOT_INTERESTED;
				}
			}

			bool bCanProduce = pCity->canProduceYield(eYield);
			if (eYield == YIELD_HORSES)
			{
				bCanProduce = false;
			}

			if (getID() == pCity->getOwnerINLINE())
			{
				if (!bCanProduce)
				{
					return DENIAL_UNKNOWN;
				}
			}
			else
			{
				if (bCanProduce)
				{
					return DENIAL_NO_GAIN;
				}
			}
		}
		else
		{
			if (getID() == pCity->getOwnerINLINE())
			{
				if (eYield == YIELD_BLADES || eYield == YIELD_MUSKETS || eYield == YIELD_CANNONS || eYield == YIELD_HORSES)
				{
					return DENIAL_NEVER;
				}
				if (AI_shouldBuyFromEurope(eYield))
				{
					return DENIAL_NO_GAIN;
				}
				if (pCity->calculateActualYieldConsumed(eYield) > 0)
				{
					return DENIAL_NO_GAIN;
				}
			}
		}
	}

	return NO_DENIAL;
}

int CvPlayerAI::AI_calculateDamages(TeamTypes eTeam)
{
	int iValue = 0;

	int iStolenPlotCost = 0;
	int iPopulationCost = 0;
	for (int iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		PlayerTypes ePlayer = (PlayerTypes)iI;
		CvPlayer& kPlayer = GET_PLAYER(ePlayer);
		if (kPlayer.isAlive())
		{
			if (kPlayer.getTeam() == eTeam)
			{
				int iStolenPlots = 0;
				for(int i=0;i<GC.getMapINLINE().numPlotsINLINE();i++)
				{
					CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(i);
					if(pLoopPlot->getOwnerINLINE() == ePlayer && pLoopPlot->getCulture(getID()) > pLoopPlot->getCulture(ePlayer))
					{
						iStolenPlotCost += pLoopPlot->getBuyPrice(ePlayer);
					}
				}

				iPopulationCost += kPlayer.getTotalPopulation() * 100;
			}
		}
	}

	iPopulationCost += 250;

	iValue += iPopulationCost;

	iValue += iStolenPlotCost / 4;

	return iValue;
}

int CvPlayerAI::AI_unitImpassableCount(UnitTypes eUnit)
{
	int iCount = 0;
	for (int iI = 0; iI < GC.getNumTerrainInfos(); iI++)
	{
		if (GC.getUnitInfo(eUnit).getTerrainImpassable(iI))
		{
				iCount++;
			}
		}

	for (int iI = 0; iI < GC.getNumFeatureInfos(); iI++)
	{
		if (GC.getUnitInfo(eUnit).getFeatureImpassable(iI))
		{
				iCount++;
			}
		}

	return iCount;
}

//Calculates the value of the unit as "Profit generated in 20 turns" in pCity,
//or if pCity is NULL it assumes that it will either found somewhere
//or has some other role.
int CvPlayerAI::AI_unitEconomicValue(UnitTypes eUnit, UnitAITypes* peUnitAI, CvCity* pCity)
{
	UnitAITypes eBestUnitAI = NO_UNITAI;

	if (getNumCities() == 0)
	{
		return 1;
	}

	CvUnitInfo& kUnitInfo = GC.getUnitInfo(eUnit);
	int iBestValue = 0;
	if (kUnitInfo.getUnitAIType(UNITAI_COLONIST))
	{
		if (pCity == NULL)
		{
			//Do we have an ideal profession?
			for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				YieldTypes eYield = (YieldTypes)iI;

				if (kUnitInfo.getYieldModifier(eYield) > 0)
				{
					for (int iJ = 0; iJ < GC.getNumProfessionInfos(); iJ++)
					{
						if (GC.getCivilizationInfo(getCivilizationType()).isValidProfession(iJ))
						{
							CvProfessionInfo& kProfessionInfo = GC.getProfessionInfo((ProfessionTypes)iJ);
							// R&R, ray , MYCP partially based on code of Aymerick - START
							if (kProfessionInfo.getNumYieldsProduced() >0 && kProfessionInfo.getYieldsProduced(0) == eYield)
							{
								if (kProfessionInfo.getNumYieldsConsumed() == 0)
								{
									// R&R, ray , MYCP partially based on code of Aymerick - END
									if (kProfessionInfo.isWorkPlot())
									{
										CvPlot* pCenter = AI_getTerritoryCenter();
										int iRadius = AI_getTerritoryRadius();
										for (int iX = -iRadius; iX <= iRadius; iX++)
										{
											for (int iY = -iRadius; iY <= iRadius; iY++)
											{
												CvPlot* pLoopPlot = plotXY(pCenter->getX_INLINE(), pCenter->getY_INLINE(), iX, iY);
												if ((pLoopPlot != NULL) && (pLoopPlot->getOwnerINLINE() == getID()) && (!pLoopPlot->isCity()))
												{
													int iAmount = pLoopPlot->calculatePotentialYield(eYield, NULL, false);

													iAmount *= 100 + kUnitInfo.getYieldModifier(eYield);
													iAmount /= 100;

													int iValue = 20 * AI_yieldValue(eYield) * iAmount;
													bool bValid = true;
													if (pLoopPlot->isBeingWorked())
													{
														iValue *= 75;
														iValue /= 100;

														CvCity* pCity = pLoopPlot->getWorkingCity();
														FAssert(pCity!= NULL);

														CvUnit* pWorkingUnit = pCity->getUnitWorkingPlot(pLoopPlot);
														FAssert(pWorkingUnit != NULL);

														if (pWorkingUnit != NULL)
														{
															FAssert(pWorkingUnit->getProfession() != NO_PROFESSION);
															// R&R, ray , MYCP partially based on code of Aymerick - START
															YieldTypes eTempYield = (YieldTypes)GC.getProfessionInfo(pWorkingUnit->getProfession()).getYieldsProduced(0);
															// R&R, ray , MYCP partially based on code of Aymerick - END

															int iTempAmount = pLoopPlot->getYield(eYield);
															int iTempValue = 20 * AI_yieldValue(eTempYield) * iTempAmount;

															if (iValue > (iTempValue * 4) / 3)
															{
																bValid = false;
															}
														}
													}

													if (bValid)
													{
														if (iValue > iBestValue)
														{
															iBestValue = iValue;
															eBestUnitAI = UNITAI_COLONIST;
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
				}
			}
		}
	}

	if (kUnitInfo.getUnitAIType(UNITAI_WAGON))
	{
		int iValue = 0;
		if (pCity != NULL)
		{
			int iCityCount = pCity->area()->getCitiesPerPlayer(getID());
			int iWagonCount = AI_totalAreaUnitAIs(pCity->area(), UNITAI_WAGON);

			int iNeededWagons = iCityCount;
			if (iNeededWagons < iWagonCount)
			{
				iValue += 100 * kUnitInfo.getCargoSpace();
			}
		}

		if (iValue > iBestValue)
		{
			iBestValue = iValue;
			eBestUnitAI = UNITAI_WAGON;
		}
	}

	if (kUnitInfo.getUnitAIType(UNITAI_TRANSPORT_COAST))
	{
		int iValue = 0;
		
		if (pCity != NULL)
		{
			CvArea* const pWaterArea = pCity->waterArea();
			
			if (pWaterArea != NULL)
			{
				const int iCoastalCityCount = countNumCoastalCities();
				const int iTransportCount = AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_TRANSPORT_COAST);

				int iNeededTransports = iCoastalCityCount / 2;
				if (iNeededTransports < iTransportCount)
				{
					iValue += 50 * kUnitInfo.getCargoSpace();
				}
			}
		}

		if (iValue > iBestValue)
		{
			iBestValue = iValue;
			eBestUnitAI = UNITAI_TRANSPORT_COAST;
		}
	}

	if (peUnitAI != NULL)
	{
		*peUnitAI = eBestUnitAI;
	}

	return iBestValue;
}

int CvPlayerAI::AI_unitValue(UnitTypes eUnit, UnitAITypes eUnitAI, CvArea* pArea)
{
	bool bValid;
	int iCombatValue;
	int iValue;

	FAssertMsg(eUnit != NO_UNIT, "Unit is not assigned a valid value");
	FAssertMsg(eUnitAI != NO_UNITAI, "UnitAI is not assigned a valid value");
	CvUnitInfo& kUnitInfo = GC.getUnitInfo(eUnit);

	if (kUnitInfo.getDomainType() != AI_unitAIDomainType(eUnitAI))
	{
		return 0;
	}

	if (kUnitInfo.getNotUnitAIType(eUnitAI))
	{
		return 0;
	}

	bValid = kUnitInfo.getUnitAIType(eUnitAI);

	if (!bValid)
	{
		switch (eUnitAI)
		{
		case UNITAI_UNKNOWN:
			break;
		case UNITAI_COLONIST:
			break;
		case UNITAI_ANIMAL: // R&R, ray, Wild Animals
			break;
		case UNITAI_ANIMAL_SEA: // R&R, ray, Wild Animals
			break;
		case UNITAI_FLEEING: // R&R, ray, Fleeing Units
			break;
		case UNITAI_SETTLER:
			break;
		case UNITAI_WORKER:
			break;
		case UNITAI_MISSIONARY:
			break;
		case UNITAI_TRADER: // WTP, ray, Native Trade Posts - START
			break;
		case UNITAI_SCOUT:
			break;
		case UNITAI_WAGON:
			break;
		case UNITAI_TREASURE:
			break;
		case UNITAI_YIELD:
			break;
		case UNITAI_GENERAL:
			break;
		case UNITAI_DEFENSIVE:
			break;
		case UNITAI_OFFENSIVE:
			break;
		case UNITAI_COUNTER:
			break;

		//TAC Whaling, ray
		case UNITAI_WORKER_SEA:
			if (kUnitInfo.isGatherBoat() && kUnitInfo.getMoves() > 0)
			{
				bValid = true;
			}
			break;

		case UNITAI_TRANSPORT_SEA:
		case UNITAI_TRANSPORT_COAST:
			break;
		case UNITAI_ASSAULT_SEA:
			// TAC - AI Assault Sea - koma13 - START
			//if ((kUnitInfo.getCargoSpace() > 0) && kUnitInfo.getMoves() > 0)
			if ((kUnitInfo.getCargoSpace() > 0) && (kUnitInfo.getMoves() > 0) && (kUnitInfo.getSpecialCargo() == NO_SPECIALUNIT))
			// TAC - AI Assault Sea - koma13 - END
			{
				bValid = true;
			}
			break;
		case UNITAI_COMBAT_SEA:
			break;
		case UNITAI_PIRATE_SEA:
			if (kUnitInfo.isHiddenNationality())
			{
				if ((kUnitInfo.getCombat() > 0) && kUnitInfo.getMoves() > 0)
				{
					bValid = true;
				}
			}
			break;

		// TAC - AI Escort Sea - koma13 - START
		case UNITAI_ESCORT_SEA:
			if (kUnitInfo.getCombat() > 0)
			{
				if (kUnitInfo.getCargoSpace() == 0)
				{
					//if (0 == AI_unitImpassableCount(eUnit))
					{
						bValid = true;
					}
				}
			}
			break;
		// TAC - AI Escort Sea - koma13 - END

		default:
			FAssert(false);
			break;
		}
	}

	if (!bValid)
	{
		return 0;
	}

	iCombatValue = GC.getGameINLINE().AI_combatValue(eUnit);

	iValue = 100;

	iValue += kUnitInfo.getAIWeight();

	int iEuropeCost = getEuropeUnitBuyPrice(eUnit);
	if (iEuropeCost > 0)
	{
		iValue += iEuropeCost;
	}

	return std::max(0, iValue);
}


//This function attempts to return how much gold this unit is worth.
int CvPlayerAI::AI_unitGoldValue(UnitTypes eUnit, UnitAITypes eUnitAI, CvArea* pArea) const
{
	bool bValid = false;
	int iValue = 0;

	FAssertMsg(eUnit != NO_UNIT, "Unit is not assigned a valid value");
	FAssertMsg(eUnitAI != NO_UNITAI, "UnitAI is not assigned a valid value");
	CvUnitInfo& kUnitInfo = GC.getUnitInfo(eUnit);

	if (kUnitInfo.getDomainType() != AI_unitAIDomainType(eUnitAI))
	{
		return 0;
	}

	if (kUnitInfo.getNotUnitAIType(eUnitAI))
	{
		return 0;
	}

	bValid = kUnitInfo.getUnitAIType(eUnitAI);

	if (!bValid)
	{
		switch (eUnitAI)
		{
		case UNITAI_UNKNOWN:
		case UNITAI_ANIMAL: // R&R, ray, Wild Animals
		case UNITAI_ANIMAL_SEA: // R&R, ray, Wild Animals
		case UNITAI_FLEEING: // R&R, ray, Fleeing Units
			break;

		case UNITAI_COLONIST:
		case UNITAI_SETTLER:
		case UNITAI_WORKER:
		case UNITAI_MISSIONARY:
		case UNITAI_TRADER: // WTP, ray, Native Trade Posts - START
		case UNITAI_SCOUT:
			if (kUnitInfo.getDefaultProfession() != NO_PROFESSION)
			{
				bValid = true;
			}
			break;

		case UNITAI_WAGON:
			if (kUnitInfo.getCargoSpace() > 0)
			{
				bValid = true;
			}
			break;

		case UNITAI_TREASURE:
			break;

		case UNITAI_YIELD:
			break;

		case UNITAI_GENERAL:
			break;

		case UNITAI_DEFENSIVE:
			if (kUnitInfo.getDefaultProfession() != NO_PROFESSION)
			{
				bValid = true;
			}
			break;

		case UNITAI_OFFENSIVE:
			if (kUnitInfo.getBombardRate() > 0)
			{
				bValid = true;
			}
			break;

		case UNITAI_COUNTER:
			if (kUnitInfo.getDefaultProfession() != NO_PROFESSION)
			{
				bValid = true;
			}
			break;

		//TAC Whaling, ray
		case UNITAI_WORKER_SEA:
			if (kUnitInfo.isGatherBoat())
			{
				bValid = true;
			}
			break;

		case UNITAI_TRANSPORT_SEA:
		case UNITAI_TRANSPORT_COAST:
			if (kUnitInfo.getCargoSpace() > 0)
			{
				bValid = true;
			}
			break;

		case UNITAI_ASSAULT_SEA:
			// TAC - AI Assault Sea - koma13 - START
			//if ((kUnitInfo.getCargoSpace() > 0) && kUnitInfo.getMoves() > 0)
			if ((kUnitInfo.getCargoSpace() > 0) && (kUnitInfo.getMoves() > 0) && (kUnitInfo.getSpecialCargo() == NO_SPECIALUNIT))
			// TAC - AI Assault Sea - koma13 - END
			{
				bValid = true;
			}
			break;

		case UNITAI_COMBAT_SEA:
			// TAC - AI purchases military units - koma13 - START
			//if (!kUnitInfo.isOnlyDefensive() && (kUnitInfo.getCombat() > 0) && kUnitInfo.getMoves() > 0)
			if (!kUnitInfo.isOnlyDefensive() && (kUnitInfo.getCombat() > 0) && kUnitInfo.getMoves() > 0 && kUnitInfo.getDefaultUnitAIType() != UNITAI_TRANSPORT_SEA && !kUnitInfo.isHiddenNationality())
			// TAC - AI purchases military units - koma13 - END
			{
				bValid = true;
			}
			break;
		case UNITAI_PIRATE_SEA:
			if (kUnitInfo.isHiddenNationality())
			{
				if ((kUnitInfo.getCombat() > 0) && kUnitInfo.getMoves() > 0)
				{
					bValid = true;
				}
			}
			break;
		
		// TAC - AI Escort Sea - koma13 - START
		case UNITAI_ESCORT_SEA:
			if (!kUnitInfo.isOnlyDefensive() && (kUnitInfo.getCombat() > 0) && kUnitInfo.getMoves() > 0)
			{
				if (kUnitInfo.getCargoSpace() == 0)
				{
					bValid = true;
				}
			}
			break;
		// TAC - AI Escort Sea - koma13 - END

		default:
			FAssert(false);
			break;
		}
	}

	if (!bValid)
	{
		return 0;
	}
	//This function specifically tries to estimate the gold value of a unit.

	int iOffenseCombatValue = kUnitInfo.getCombat() * 100;
	int iDefenseCombatValue = kUnitInfo.getCombat() * 100;

	if (kUnitInfo.isOnlyDefensive())
	{
		iOffenseCombatValue /= 4;
	}

	int iCargoValue = kUnitInfo.getCargoSpace() * 250;

	if (kUnitInfo.getDefaultProfession() != NO_PROFESSION)
	{
		iValue += std::max(0, kUnitInfo.getEuropeCost());
	}

	int iTempValue;

	switch (eUnitAI)
	{
	case UNITAI_UNKNOWN:
	case UNITAI_ANIMAL: // R&R, ray, Wild Animals
	case UNITAI_ANIMAL_SEA: // R&R, ray, Wild Animals
	case UNITAI_FLEEING: // R&R, ray, Fleeing Units
		break;

	case UNITAI_COLONIST:
	case UNITAI_SETTLER:
	case UNITAI_WORKER:
	case UNITAI_MISSIONARY:
	case UNITAI_TRADER: // WTP, ray, Native Trade Posts - START
	case UNITAI_SCOUT:
		break;

	case UNITAI_WAGON:
	case UNITAI_TRANSPORT_COAST:
		iTempValue = iCargoValue + iDefenseCombatValue / 2;
		iTempValue *= 1 + kUnitInfo.getMoves();
		iTempValue /= 2;
		iValue += iTempValue;
		break;


	case UNITAI_TREASURE:
		break;

	case UNITAI_YIELD:
		break;

	case UNITAI_GENERAL:
		break;

	case UNITAI_DEFENSIVE:
		iTempValue = iOffenseCombatValue / 2;
		iTempValue += iDefenseCombatValue;
		iTempValue += kUnitInfo.getBombardRate() * 50;
		iValue += iTempValue;
		break;

	case UNITAI_OFFENSIVE:
		iTempValue = iOffenseCombatValue;
		iTempValue += iDefenseCombatValue / 2;
		iTempValue += kUnitInfo.getBombardRate() * 50;

		iTempValue *= 2 + kUnitInfo.getMoves();
		iTempValue /= 3;
		iValue += iTempValue;
		break;

	case UNITAI_COUNTER:
		iTempValue = iOffenseCombatValue * 2 / 3;
		iTempValue += iDefenseCombatValue * 2 / 3;
		iTempValue += kUnitInfo.getBombardRate() * 50;

		iTempValue *= 2 + kUnitInfo.getMoves();
		iTempValue /= 3;
		iValue += iTempValue;
		break;

	//TAC Whaling, ray
	case UNITAI_WORKER_SEA:
		//iValue += ((4 + kUnitInfo.getMoves()) * ((kUnitInfo.getNumYieldsGatherable() * 250) + iDefenseCombatValue / 2)) / 6;

		iTempValue = ((4 + kUnitInfo.getMoves()) * (iCargoValue + iDefenseCombatValue / 2)) / 6;
		iTempValue += (kUnitInfo.getNumYieldsGatherable() * 250) / 6;
		iValue += iTempValue;
		break;

	case UNITAI_TRANSPORT_SEA:

		iValue += ((4 + kUnitInfo.getMoves()) * (iCargoValue + iDefenseCombatValue / 2)) / 7;

		break;

	case UNITAI_ASSAULT_SEA:
		// TAC - AI Assault Sea - koma13 - START
		FAssert(kUnitInfo.getSpecialCargo() == NO_SPECIALUNIT);
		if (kUnitInfo.getSpecialCargo() == NO_SPECIALUNIT)
		{
			iTempValue = iDefenseCombatValue + iCargoValue;
			iTempValue *= 4 + kUnitInfo.getMoves();
			iTempValue /= 7;
			if (kUnitInfo.isHiddenNationality())
			{
				iTempValue /= 2;
			}
			iValue += iTempValue;
		}
		// TAC - AI Assault Sea - koma13 - END
		break;

	case UNITAI_COMBAT_SEA:
		iTempValue = iOffenseCombatValue * 2 + iDefenseCombatValue;
		iTempValue *= 4 + kUnitInfo.getMoves();
		iTempValue /= 8;
		if (kUnitInfo.isHiddenNationality())
		{
			iTempValue /= 2;
		}
		iValue += iTempValue;
		break;

	case UNITAI_PIRATE_SEA:
		iTempValue = iOffenseCombatValue + iDefenseCombatValue + iCargoValue / 2;
		iTempValue *= 3 + kUnitInfo.getMoves();
		iTempValue /= 6;
		iValue += iTempValue;
		break;

	// TAC - AI Escort Sea - koma13 - START
	case UNITAI_ESCORT_SEA:
		iTempValue = iOffenseCombatValue * 2 + iDefenseCombatValue;
		iTempValue *= 4 + kUnitInfo.getMoves();
		iTempValue /= 8;
		if (kUnitInfo.isHiddenNationality())
		{
			iTempValue /= 2;
		}
		iValue += iTempValue;
		break;
	// TAC - AI Escort Sea - koma13 - END

	default:
		FAssert(false);
		break;
	}


	iValue +=  kUnitInfo.getAIWeight();

	return std::max(0, iValue);
}

//This function indicates how worthwhile the unit is to buy.
int CvPlayerAI::AI_unitValuePercent(UnitTypes eUnit, UnitAITypes* peUnitAI, CvArea* pArea)
{
	FAssertMsg(eUnit != NO_UNIT, "Unit is not assigned a valid value");
	CvUnitInfo& kUnitInfo = GC.getUnitInfo(eUnit);

	int iValue = 0;

	int iGoldCost = getEuropeUnitBuyPrice(eUnit);

	if (iGoldCost <= 0)
	{
		return -1;
	}


	//Transport Sea
	int iCargoSpace = kUnitInfo.getCargoSpace();
	if ((iCargoSpace > 1) && (kUnitInfo.getDomainType() == DOMAIN_SEA))
	{
		//Do we need a transport, period?
		int iTransportCount = AI_totalUnitAIs(UNITAI_TRANSPORT_SEA);
		if (iTransportCount == 0)
		{
			iValue += 200 + 50 * iCargoSpace;
		}
		else if (AI_totalUnitAIs(UNITAI_TREASURE) > 0) //Do we need a treasure transport?
		{
			int iLargestTreasureUnit = 0;
			int iTotalTreasure = 0;
			bool bValid = true;

			int iLoop;
			CvUnit* pLoopUnit;
			for (pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
			{
				if (pLoopUnit->canMove())
				{
					if (pLoopUnit->cargoSpace() > 0)
					{
						if (pLoopUnit->cargoSpace() >= iCargoSpace)
						{
							bValid = false;
							break;
						}
					}
					if (pLoopUnit->AI_getUnitAIType() == UNITAI_TREASURE)
					{
						int iSize = pLoopUnit->getUnitInfo().getRequiredTransportSize();
						if (iSize > 1)
						{
							iLargestTreasureUnit = std::max(iLargestTreasureUnit, iSize);
							if (iCargoSpace >= iSize)
							{
								iTotalTreasure += pLoopUnit->getYield();
							}
						}
					}
				}
			}

			if (kUnitInfo.getCargoSpace() >= iLargestTreasureUnit)
			{
				iValue += 100 + ((100 * iTotalTreasure) / iGoldCost);
			}
		}

		if (iTransportCount < (1 + getNumCities() / 3))
		{
			int iBestTransportSize = 0;
			int iLoop;
			CvUnit* pLoopUnit;
			for (pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
			{
				int iBestTransportSize = 1;
				if (pLoopUnit->AI_getUnitAIType() == UNITAI_TRANSPORT_SEA)
				{
					iBestTransportSize = std::max(iBestTransportSize, pLoopUnit->cargoSpace());
				}
			}

			if (iCargoSpace == iBestTransportSize)
			{
				iValue += 25;
			}
			else if (iCargoSpace > iBestTransportSize)
			{
				iValue += 50 + 10 * (iCargoSpace - iBestTransportSize);
			}
		}
	}

	//Warships
	if (kUnitInfo.getDomainType() == DOMAIN_SEA)
	{
		if (kUnitInfo.getCombat() > 0)
		{
			//Pirate
			if (getTotalPopulation() > 12)
			{
				if (kUnitInfo.isHiddenNationality())
				{
					if (AI_totalUnitAIs(UNITAI_PIRATE_SEA) <= (getNumCities() / 9))
					{
						iValue += 50 + getNumCities() * 5;
					}
				}
			}
		}
	}
	return iValue;
}

int CvPlayerAI::AI_totalUnitAIs(UnitAITypes eUnitAI)
{
	return (AI_getNumTrainAIUnits(eUnitAI) + AI_getNumAIUnits(eUnitAI));
}


int CvPlayerAI::AI_totalAreaUnitAIs(CvArea* pArea, UnitAITypes eUnitAI)
{
	return (pArea->getNumTrainAIUnits(getID(), eUnitAI) + pArea->getNumAIUnits(getID(), eUnitAI));
}


int CvPlayerAI::AI_totalWaterAreaUnitAIs(CvArea* pArea, UnitAITypes eUnitAI)
{
	CvCity* pLoopCity;
	int iCount;
	int iLoop;
	int iI;

	iCount = AI_totalAreaUnitAIs(pArea, eUnitAI);

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			for (pLoopCity = GET_PLAYER((PlayerTypes)iI).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iI).nextCity(&iLoop))
			{
				if (pLoopCity->waterArea() == pArea)
				{
					iCount += pLoopCity->plot()->plotCount(PUF_isUnitAIType, eUnitAI, -1, getID());

					if (pLoopCity->getOwnerINLINE() == getID())
					{
						iCount += pLoopCity->getNumTrainUnitAI(eUnitAI);
					}
				}
			}
		}
	}


	return iCount;
}

bool CvPlayerAI::AI_hasSeaTransport(const CvUnit* pCargo) const
{
	int iLoop;
	for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit != pCargo && pLoopUnit->getDomainType() == DOMAIN_SEA)
		{
			if (pLoopUnit->cargoSpace() >= pCargo->getUnitInfo().getRequiredTransportSize())
			{
				return true;
			}

		}
	}
	return false;
}

int CvPlayerAI::AI_neededExplorers(CvArea* pArea)
{
	FAssert(pArea != NULL);
	int iNeeded = 0;

	if (pArea->isWater())
	{
		iNeeded = std::min(iNeeded + (pArea->getNumUnrevealedTiles(getTeam()) / 400), std::min(2, ((getNumCities() / 2) + 1)));
	}
	else
	{
		iNeeded = std::min(iNeeded + (pArea->getNumUnrevealedTiles(getTeam()) / 150), std::min(3, ((getNumCities() / 3) + 2)));
	}

	if (0 == iNeeded)
	{
		if ((GC.getGameINLINE().countCivTeamsAlive() - 1) > GET_TEAM(getTeam()).getHasMetCivCount())
		{
			if (pArea->isWater())
			{
				if (GC.getMap().findBiggestArea(true) == pArea)
				{
					iNeeded++;
				}
			}
			else
			{
				if (getPrimaryCity() != NULL && pArea->getID() == getPrimaryCity()->getArea())
				{
					for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
					{
						CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);
						if (kPlayer.isAlive() && kPlayer.getTeam() != getTeam())
						{
							if (!GET_TEAM(getTeam()).isHasMet(kPlayer.getTeam()))
							{
								if (pArea->getCitiesPerPlayer(kPlayer.getID()) > 0)
								{
									iNeeded++;
									break;
								}
							}
						}
					}
				}
			}
		}
	}
	return iNeeded;

}


int CvPlayerAI::AI_neededWorkers(CvArea* pArea)
{
	CvCity* pLoopCity;
	int iCount;
	int iLoop;

	iCount = 0;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if ((pArea == NULL) || (pLoopCity->getArea() == pArea->getID()))
		{
			iCount += pLoopCity->AI_getWorkersNeeded();
		}
	}

	if (iCount == 0)
	{
		return 0;
	}

	return std::max(1, (iCount * 2) / 3);
}

int CvPlayerAI::AI_adjacentPotentialAttackers(CvPlot* pPlot, bool bTestCanMove)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pLoopPlot;
	int iCount;
	int iI;

	iCount = 0;

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

					if (pLoopUnit->getOwnerINLINE() == getID())
					{
						if (pLoopUnit->getDomainType() == ((pPlot->isWater()) ? DOMAIN_SEA : DOMAIN_LAND))
						{
							if (pLoopUnit->canAttack())
							{
								if (!bTestCanMove || pLoopUnit->canMove())
								{
									if (!(pLoopUnit->AI_isCityAIType()))
									{
										iCount++;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return iCount;
}


// TAC - AI City Defense - koma13 - START
//int CvPlayerAI::AI_totalMissionAIs(MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup)
int CvPlayerAI::AI_totalMissionAIs(MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup, UnitAITypes eUnitAI)
// TAC - AI City Defense - koma13 - END
{
	PROFILE_FUNC();

	CvSelectionGroup* pLoopSelectionGroup;
	int iCount;
	int iLoop;

	iCount = 0;

	for(pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
	{
		if (pLoopSelectionGroup != pSkipSelectionGroup)
		{
			if (pLoopSelectionGroup->AI_getMissionAIType() == eMissionAI)
			{
				// TAC - AI City Defense - koma13 - START
				//iCount += pLoopSelectionGroup->getNumUnits();
				if ((eUnitAI == NO_UNITAI) || (pLoopSelectionGroup->getHeadUnitAI() == eUnitAI))
				{
					iCount += pLoopSelectionGroup->getNumUnits();
				}
				// TAC - AI City Defense - koma13 - END
			}
		}
	}

	return iCount;
}

int CvPlayerAI::AI_areaMissionAIs(CvArea* pArea, MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup)
{
	PROFILE_FUNC();

	CvSelectionGroup* pLoopSelectionGroup;
	CvPlot* pMissionPlot;
	int iCount;
	int iLoop;

	iCount = 0;

	for(pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
	{
		if (pLoopSelectionGroup != pSkipSelectionGroup)
		{
			if (pLoopSelectionGroup->AI_getMissionAIType() == eMissionAI)
			{
				pMissionPlot = pLoopSelectionGroup->AI_getMissionAIPlot();

				if (pMissionPlot != NULL)
				{
					if (pMissionPlot->getArea() == pArea->getID())
					{
						iCount += pLoopSelectionGroup->getNumUnits();
					}
				}
			}
		}
	}

	return iCount;
}


int CvPlayerAI::AI_adjacantToAreaMissionAIs(CvArea* pArea, MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup)
{
	PROFILE_FUNC();

	CvSelectionGroup* pLoopSelectionGroup;
	CvPlot* pMissionPlot;
	int iCount;
	int iLoop;

	iCount = 0;

	for(pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
	{
		if (pLoopSelectionGroup != pSkipSelectionGroup)
		{
			if (pLoopSelectionGroup->AI_getMissionAIType() == eMissionAI)
			{
				pMissionPlot = pLoopSelectionGroup->AI_getMissionAIPlot();

				if (pMissionPlot != NULL)
				{
					if (pMissionPlot->isAdjacentToArea(pArea->getID()))
					{
						iCount += pLoopSelectionGroup->getNumUnits();
					}
				}
			}
		}
	}

	return iCount;
}


int CvPlayerAI::AI_plotTargetMissionAIs(CvPlot* pPlot, MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup, int iRange)
{
	int iClosestTargetRange;
	return AI_plotTargetMissionAIs(pPlot, &eMissionAI, 1, iClosestTargetRange, pSkipSelectionGroup, iRange);
}

int CvPlayerAI::AI_plotTargetMissionAIs(CvPlot* pPlot, MissionAITypes eMissionAI, int& iClosestTargetRange, CvSelectionGroup* pSkipSelectionGroup, int iRange)
{
	return AI_plotTargetMissionAIs(pPlot, &eMissionAI, 1, iClosestTargetRange, pSkipSelectionGroup, iRange);
}

int CvPlayerAI::AI_plotTargetMissionAIs(CvPlot* pPlot, MissionAITypes* aeMissionAI, int iMissionAICount, int& iClosestTargetRange, CvSelectionGroup* pSkipSelectionGroup, int iRange)
{
	PROFILE_FUNC();

	int iCount = 0;
	iClosestTargetRange = MAX_INT;

	CvSelectionGroup* pTransportSelectionGroup = NULL;
	if (pSkipSelectionGroup != NULL)
	{
		CvUnit* pHeadUnit = pSkipSelectionGroup->getHeadUnit();
		if (pHeadUnit->getTransportUnit() != NULL)
		{
			pTransportSelectionGroup = pHeadUnit->getTransportUnit()->getGroup();
		}
	}
	int iLoop;
	for(CvSelectionGroup* pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
	{
		if ((pSkipSelectionGroup == NULL) || ((pLoopSelectionGroup != pSkipSelectionGroup) && (pLoopSelectionGroup != pTransportSelectionGroup)))
		{
			CvPlot* pMissionPlot = pLoopSelectionGroup->AI_getMissionAIPlot();

			if (pMissionPlot != NULL)
			{
				MissionAITypes eGroupMissionAI = pLoopSelectionGroup->AI_getMissionAIType();
				int iDistance = stepDistance(pPlot->getX_INLINE(), pPlot->getY_INLINE(), pMissionPlot->getX_INLINE(), pMissionPlot->getY_INLINE());

				if (iDistance <= iRange)
				{
					for (int iMissionAIIndex = 0; iMissionAIIndex < iMissionAICount; iMissionAIIndex++)
					{
						if (eGroupMissionAI == aeMissionAI[iMissionAIIndex] || aeMissionAI[iMissionAIIndex] == NO_MISSIONAI)
						{
							if ((eGroupMissionAI != MISSIONAI_GUARD_CITY) || ((eGroupMissionAI == MISSIONAI_GUARD_CITY) && (pMissionPlot != pLoopSelectionGroup->plot())))	// TAC - AI City Defense - koma13
							{
								iCount += pLoopSelectionGroup->getNumUnits();

								if (iDistance < iClosestTargetRange)
								{
									iClosestTargetRange = iDistance;
								}
							}
						}
					}
				}
			}
		}
	}

	return iCount;
}
// TAC - AI Improved Naval AI - koma13 - START
int CvPlayerAI::AI_cargoSpaceToEurope(CvSelectionGroup* pSkipSelectionGroup)
{
	int iCount = 0;
	int iLoop;
	
	for(CvSelectionGroup* pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
	{
		if ((pSkipSelectionGroup == NULL) || (pLoopSelectionGroup != pSkipSelectionGroup))
		{
			CvUnit* pHeadUnit = pLoopSelectionGroup->getHeadUnit();

			if ((pHeadUnit != NULL) && (pHeadUnit->AI_getUnitAIType() == UNITAI_TRANSPORT_SEA))
			{
				MissionAITypes eMissionAI = pLoopSelectionGroup->AI_getMissionAIType();
				
				if ((eMissionAI == MISSIONAI_SAIL_TO_EUROPE) || (eMissionAI == MISSIONAI_SAIL_TO_AFRICA) || ((pHeadUnit->AI_getUnitAIState() == UNITAI_STATE_SAIL) && (eMissionAI != MISSIONAI_PICKUP)))
				{
					iCount += pHeadUnit->cargoSpace();
				}
			}
		}
	}

	return iCount;
}
// TAC - AI Improved Naval AI - koma13 - END

// TAC - AI Attack City - koma13, jdog5000(BBAI) - START
int CvPlayerAI::AI_cityTargetUnitsByPath(CvCity* pCity, CvSelectionGroup* pSkipSelectionGroup, int iMaxPathTurns) const
{
	PROFILE_FUNC();

	int iCount = 0;

	int iLoop;
	int iPathTurns;
	for(CvSelectionGroup* pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
	{
		if (pLoopSelectionGroup != pSkipSelectionGroup && pLoopSelectionGroup->plot() != NULL && pLoopSelectionGroup->getNumUnits() > 0)
		{
			CvPlot* pMissionPlot = pLoopSelectionGroup->AI_getMissionAIPlot();

			if (pMissionPlot != NULL )
			{
				int iDistance = stepDistance(pCity->getX_INLINE(), pCity->getY_INLINE(), pMissionPlot->getX_INLINE(), pMissionPlot->getY_INLINE());

				if (iDistance <= 1)
				{
					if( pLoopSelectionGroup->generatePath(pLoopSelectionGroup->plot(), pMissionPlot, 0, true, &iPathTurns) )
					{
						if( !(pLoopSelectionGroup->canAllMove()) )
						{
							iPathTurns++;
						}

						if( iPathTurns <= iMaxPathTurns )
						{
							iCount += pLoopSelectionGroup->getNumUnits();
						}
					}
				}
			}
		}
	}

	return iCount;
}
// TAC - AI Attack City - koma13, jdog5000(BBAI) - END

// TAC - AI Assault Sea - koma13, jdog5000(BBAI) - START
/*
int CvPlayerAI::AI_unitTargetMissionAIs(CvUnit* pUnit, MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup)
{
	return AI_unitTargetMissionAIs(pUnit, &eMissionAI, 1, pSkipSelectionGroup);
}

int CvPlayerAI::AI_unitTargetMissionAIs(CvUnit* pUnit, MissionAITypes* aeMissionAI, int iMissionAICount, CvSelectionGroup* pSkipSelectionGroup)
{
	PROFILE_FUNC();

	CvSelectionGroup* pLoopSelectionGroup;
	int iCount;
	int iLoop;

	CvSelectionGroup* pTransportSelectionGroup = NULL;
	if (pSkipSelectionGroup != NULL)
	{
		CvUnit* pHeadUnit = pSkipSelectionGroup->getHeadUnit();
		if (pHeadUnit->getTransportUnit() != NULL)
		{
			pTransportSelectionGroup = pHeadUnit->getTransportUnit()->getGroup();
		}
	}

	iCount = 0;
	for(pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
	{
		if ((pSkipSelectionGroup == NULL) || ((pLoopSelectionGroup != pSkipSelectionGroup) && (pLoopSelectionGroup != pTransportSelectionGroup)))
		{
			if (pLoopSelectionGroup->AI_getMissionAIUnit() == pUnit)
			{
				MissionAITypes eGroupMissionAI = pLoopSelectionGroup->AI_getMissionAIType();
				for (int iMissionAIIndex = 0; iMissionAIIndex < iMissionAICount; iMissionAIIndex++)
				{
					if (eGroupMissionAI == aeMissionAI[iMissionAIIndex] || NO_MISSIONAI == aeMissionAI[iMissionAIIndex])
					{
						iCount += pLoopSelectionGroup->getNumUnits();
					}
				}
			}
		}
	}

	return iCount;
}
*/

// TAC - AI Attack City - koma13 - START
int CvPlayerAI::AI_unitTargetMissionAIs(CvUnit* pUnit, MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup)
{
	//return AI_unitTargetMissionAIs(pUnit, &eMissionAI, 1, pSkipSelectionGroup, -1);
	return AI_unitTargetMissionAIs(pUnit, &eMissionAI, 1, pSkipSelectionGroup, -1, NO_UNITAI);
}

int CvPlayerAI::AI_unitTargetMissionAIs(CvUnit* pUnit, MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup, UnitAITypes eUnitAI)
{
	return AI_unitTargetMissionAIs(pUnit, &eMissionAI, 1, pSkipSelectionGroup, -1, eUnitAI);
}

int CvPlayerAI::AI_unitTargetMissionAIs(CvUnit* pUnit, MissionAITypes* aeMissionAI, int iMissionAICount, CvSelectionGroup* pSkipSelectionGroup)
{
	//return AI_unitTargetMissionAIs(pUnit, aeMissionAI, iMissionAICount, pSkipSelectionGroup, -1);
	return AI_unitTargetMissionAIs(pUnit, aeMissionAI, iMissionAICount, pSkipSelectionGroup, -1, NO_UNITAI);
}

int CvPlayerAI::AI_unitTargetMissionAIs(CvUnit* pUnit, MissionAITypes* aeMissionAI, int iMissionAICount, CvSelectionGroup* pSkipSelectionGroup, int iMaxPathTurns) const
{
	return AI_unitTargetMissionAIs(pUnit, aeMissionAI, iMissionAICount, pSkipSelectionGroup, iMaxPathTurns, NO_UNITAI);
}

//int CvPlayerAI::AI_unitTargetMissionAIs(CvUnit* pUnit, MissionAITypes* aeMissionAI, int iMissionAICount, CvSelectionGroup* pSkipSelectionGroup, int iMaxPathTurns) const
int CvPlayerAI::AI_unitTargetMissionAIs(CvUnit* pUnit, MissionAITypes* aeMissionAI, int iMissionAICount, CvSelectionGroup* pSkipSelectionGroup, int iMaxPathTurns, UnitAITypes eUnitAI) const
// TAC - AI Attack City - koma13 - END
{
	PROFILE_FUNC();

	CvSelectionGroup* pLoopSelectionGroup;
	int iCount;
	int iLoop;

	iCount = 0;

	for(pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
	{
		if (pLoopSelectionGroup != pSkipSelectionGroup)
		{
			if (pLoopSelectionGroup->AI_getMissionAIUnit() == pUnit)
			{
				MissionAITypes eGroupMissionAI = pLoopSelectionGroup->AI_getMissionAIType();
				int iPathTurns = MAX_INT;

				if( iMaxPathTurns >= 0 && (pUnit->plot() != NULL) && (pLoopSelectionGroup->plot() != NULL))
				{
					pLoopSelectionGroup->generatePath(pLoopSelectionGroup->plot(), pUnit->plot(), 0, false, &iPathTurns);
					if( !(pLoopSelectionGroup->canAllMove()) )
					{
						iPathTurns++;
					}
				}

				if ((iMaxPathTurns == -1) || (iPathTurns <= iMaxPathTurns))
				{
					for (int iMissionAIIndex = 0; iMissionAIIndex < iMissionAICount; iMissionAIIndex++)
					{
						if (eGroupMissionAI == aeMissionAI[iMissionAIIndex] || NO_MISSIONAI == aeMissionAI[iMissionAIIndex])
						{
							// TAC - AI Attack City - koma13 - START
							//iCount += pLoopSelectionGroup->getNumUnits();
							// R&R, ray, fix of Assert
							if (pLoopSelectionGroup->getNumUnits() > 0 && pLoopSelectionGroup->getHeadUnit() != NULL)
							{
								iCount += pLoopSelectionGroup->countNumUnitAIType(eUnitAI); //iCount += (pLoopSelectionGroup->getHeadUnit() != NULL) ? pLoopSelectionGroup->countNumUnitAIType(eUnitAI) : 0;
							}
							// TAC - AI Attack City - koma13 - END
						}
					}
				}
			}
		}
	}

	return iCount;
}
// TAC - AI Assault Sea - koma13, jdog5000(BBAI) - END

int CvPlayerAI::AI_enemyTargetMissionAIs(MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup)
{
	return AI_enemyTargetMissionAIs(&eMissionAI, 1, pSkipSelectionGroup);
}

int CvPlayerAI::AI_enemyTargetMissionAIs(MissionAITypes* aeMissionAI, int iMissionAICount, CvSelectionGroup* pSkipSelectionGroup)
{
	PROFILE_FUNC();

	CvSelectionGroup* pTransportSelectionGroup = NULL;
	if (pSkipSelectionGroup != NULL)
	{
		CvUnit* pHeadUnit = pSkipSelectionGroup->getHeadUnit();
		if (pHeadUnit->getTransportUnit() != NULL)
		{
			pTransportSelectionGroup = pHeadUnit->getTransportUnit()->getGroup();
		}
	}

	int iCount = 0;
	int iLoop;
	for(CvSelectionGroup* pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
	{
		if ((pSkipSelectionGroup == NULL) || ((pLoopSelectionGroup != pSkipSelectionGroup) && (pLoopSelectionGroup != pTransportSelectionGroup)))
		{
			CvPlot* pMissionPlot = pLoopSelectionGroup->AI_getMissionAIPlot();

			if (NULL != pMissionPlot && pMissionPlot->isOwned())
			{
				MissionAITypes eGroupMissionAI = pLoopSelectionGroup->AI_getMissionAIType();
				for (int iMissionAIIndex = 0; iMissionAIIndex < iMissionAICount; iMissionAIIndex++)
				{
						if (eGroupMissionAI == aeMissionAI[iMissionAIIndex] || NO_MISSIONAI == aeMissionAI[iMissionAIIndex])
					{
						if (GET_TEAM(getTeam()).AI_isChosenWar(pMissionPlot->getTeam()))
						{
							iCount += pLoopSelectionGroup->getNumUnits();
							iCount += pLoopSelectionGroup->getCargo();
						}
					}
				}
			}
		}
	}

	return iCount;
}

int CvPlayerAI::AI_wakePlotTargetMissionAIs(CvPlot* pPlot, MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup)
{
	PROFILE_FUNC();

	FAssert(pPlot != NULL);

	CvSelectionGroup* pTransportSelectionGroup = NULL;
	if (pSkipSelectionGroup != NULL)
	{
		CvUnit* pHeadUnit = pSkipSelectionGroup->getHeadUnit();
		if (pHeadUnit->getTransportUnit() != NULL)
		{
			pTransportSelectionGroup = pHeadUnit->getTransportUnit()->getGroup();
		}
	}

	int iCount = 0;

	int iLoop;
	for(CvSelectionGroup* pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
	{
		if ((pSkipSelectionGroup == NULL) || ((pLoopSelectionGroup != pSkipSelectionGroup) && (pLoopSelectionGroup != pTransportSelectionGroup)))
		{
			MissionAITypes eGroupMissionAI = pLoopSelectionGroup->AI_getMissionAIType();
			if (eMissionAI == NO_MISSIONAI || eMissionAI == eGroupMissionAI)
			{
				CvPlot* pMissionPlot = pLoopSelectionGroup->AI_getMissionAIPlot();
				if (pMissionPlot != NULL && pMissionPlot == pPlot)
				{
					iCount += pLoopSelectionGroup->getNumUnits();
					pLoopSelectionGroup->setActivityType(ACTIVITY_AWAKE);
				}
			}
		}
	}

	return iCount;
}

CivicTypes CvPlayerAI::AI_bestCivic(CivicOptionTypes eCivicOption)
{
	CivicTypes eBestCivic;
	int iValue;
	int iBestValue;
	int iI;

	iBestValue = MIN_INT;
	eBestCivic = NO_CIVIC;

	for (iI = 0; iI < GC.getNumCivicInfos(); iI++)
	{
		if (GC.getCivicInfo((CivicTypes)iI).getCivicOptionType() == eCivicOption)
		{
			if (canDoCivics((CivicTypes)iI))
			{
				iValue = AI_civicValue((CivicTypes)iI);

				if (isCivic((CivicTypes)iI))
				{
					iValue *= 16;
					iValue /= 15;
				}

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					eBestCivic = ((CivicTypes)iI);
				}
			}
		}
	}

	return eBestCivic;
}


int CvPlayerAI::AI_civicValue(CivicTypes eCivic)
{
	PROFILE_FUNC();

	FAssertMsg(eCivic < GC.getNumCivicInfos(), "eCivic is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eCivic >= 0, "eCivic is expected to be non-negative (invalid Index)");

	CvCivicInfo& kCivic = GC.getCivicInfo(eCivic);

	int iValue = (getNumCities() * 6);

	iValue += (GC.getCivicInfo(eCivic).getAIWeight() * getNumCities());

	iValue *= 10 + GC.getGameINLINE().getSorenRandNum(90, "AI choose revolution civics");

	return iValue;
}

int CvPlayerAI::AI_getAttackOddsChange()
{
	return m_iAttackOddsChange;
}


void CvPlayerAI::AI_setAttackOddsChange(int iNewValue)
{
	m_iAttackOddsChange = iNewValue;
}

int CvPlayerAI::AI_getExtraGoldTarget() const
{
	return m_iExtraGoldTarget;
}

void CvPlayerAI::AI_setExtraGoldTarget(int iNewValue)
{
	m_iExtraGoldTarget = iNewValue;
}

void CvPlayerAI::AI_chooseCivic(CivicOptionTypes eCivicOption)
{
	int iBestValue = MIN_INT;
	CivicTypes eBestCivic = NO_CIVIC;

	for (int iCivic = 0; iCivic < GC.getNumCivicInfos(); ++iCivic)
	{
		if (GC.getCivicInfo((CivicTypes) iCivic).getCivicOptionType() == eCivicOption)
		{
			if (canDoCivics((CivicTypes) iCivic))
			{
				int iValue = AI_civicValue((CivicTypes) iCivic);
				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					eBestCivic = (CivicTypes) iCivic;
				}
			}
		}
	}

	if (eBestCivic != NO_CIVIC)
	{
		setCivic(eCivicOption, eBestCivic);
	}
}

bool CvPlayerAI::AI_chooseGoody(GoodyTypes eGoody)
{
	return true;
}

CvCity* CvPlayerAI::AI_findBestCity() const
{
	CvCity* pBestCity = NULL;
	int iBestValue = 0;

	CvPlot* pTerritoryCenter = AI_getTerritoryCenter();

	int iLoop;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		int iValue = 1000 * (1 + pLoopCity->getPopulation());
		iValue *= 100 + 20 * (pLoopCity->plot()->getYield(YIELD_FOOD) - GC.getFOOD_CONSUMPTION_PER_POPULATION());
		iValue /= std::max(1, pLoopCity->plot()->getDistanceToOcean());

		iValue *= 1 + pLoopCity->area()->getCitiesPerPlayer(getID());
		iValue /= 4 + stepDistance(pTerritoryCenter->getX_INLINE(), pTerritoryCenter->getY_INLINE(), pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE());

		if (iValue > iBestValue)
		{
			pBestCity = pLoopCity;
			iBestValue = iValue;
		}
	}

	FAssert(pBestCity != NULL);
	return pBestCity;
}

// TAC - AI Economy - koma13 - START
//CvCity* CvPlayerAI::AI_findBestPort() const
CvCity* CvPlayerAI::AI_findBestPort(CvArea* pArea, CvCity* pNoAccessCity) const
// TAC - AI Economy - koma13 - END
{
	if (getParent() == NO_PLAYER)
	{
		return NULL;
	}

	// TAC - AI Economy - koma13 - START
	if (pArea != NULL && pArea->getCitiesPerPlayer(getID()) <= 1)
	{
		pArea = NULL;
	}
	// TAC - AI Economy - koma13 - END

	CvCity* pBestCity = NULL;
	int iBestValue = 0;
	int iLoop;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pArea == NULL || pArea == pLoopCity->area())	// TAC - AI Economy - koma13
		{
			if (pLoopCity->plot()->getNearestEurope() != NO_EUROPE)
			{
				// TAC - AI Economy - koma13 - START
				CvPlayerAI& kPlayer = GET_PLAYER(getID());

				if (pNoAccessCity == NULL || kPlayer.AI_isTradeRouteConnection(pNoAccessCity, pLoopCity))
				{
				// TAC - AI Economy - koma13 - END
					int iPortValue = 100000 / std::max(2, pLoopCity->plot()->getDistanceToOcean());
					iPortValue /= 100 + pLoopCity->getGameTurnFounded();

					if (iPortValue > iBestValue)
					{
						pBestCity = pLoopCity;
						iBestValue = iPortValue;
					}
				}
			}
		}
	}

	return pBestCity;
}

// TAC - AI Economy - koma13 - START
void CvPlayerAI::AI_updateBestPortCities()
{
	if (isHuman() || (getParent() == NO_PLAYER))
	{
		return;
	}

	if (getNumCities() == 0)
	{
		return;
	}
	
	CvCity* pLoopCity;
	int iLoop;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->AI_setPort(false);
	}

	if (getNumCities() == 1) // || AI_getNumAIUnits(UNITAI_WAGON) == 0)
	{
		CvCity* pBestPort = AI_findBestPort();
		if (pBestPort != NULL)
		{
			pBestPort->AI_setPort(true);
			return;
		}
	}

	std::vector<CvCity*> pNoAccessToBestPort;
	
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		CvArea* pLoopArea = pLoopCity->area();
		
		if (pLoopArea->getCitiesPerPlayer(getID()) > 1)
		{
			CvCity* pBestAreaPort = AI_findBestPort(pLoopArea);
				
			if (pBestAreaPort != NULL)
			{
				pBestAreaPort->AI_setPort(true);
			
				if (pBestAreaPort != pLoopCity)
				{
					CvPlayerAI& kPlayer = GET_PLAYER(getID());

					if (!kPlayer.AI_isTradeRouteConnection(pLoopCity, pBestAreaPort))
					{
						pNoAccessToBestPort.push_back(pLoopCity);
					}
				}
			}
		}
	}
	
	for (uint i = 0; i < pNoAccessToBestPort.size(); ++i)
	{
		CvCity* pBestAccessPort = AI_findBestPort(pNoAccessToBestPort[i]->area(), pNoAccessToBestPort[i]);
		if (pBestAccessPort != NULL)
		{
			pBestAccessPort->AI_setPort(true);
		}
	}
}
// TAC - AI Economy - koma13 - END
int CvPlayerAI::AI_getNumTrainAIUnits(UnitAITypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_UNITAI_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiNumTrainAIUnits[eIndex];
}


void CvPlayerAI::AI_changeNumTrainAIUnits(UnitAITypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_UNITAI_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiNumTrainAIUnits[eIndex] += iChange;
	FAssert(AI_getNumTrainAIUnits(eIndex) >= 0);
}


int CvPlayerAI::AI_getNumAIUnits(UnitAITypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_UNITAI_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiNumAIUnits[eIndex];
}


void CvPlayerAI::AI_changeNumAIUnits(UnitAITypes eIndex, int iChange)
{
	if (eIndex != NO_UNITAI)
	{
		m_aiNumAIUnits[eIndex] += iChange;
		FAssert(AI_getNumAIUnits(eIndex) >= 0);

#ifdef _DEBUG
		if (iChange > 0)
		{
			int iLoop;
			int iNumUnitAI = 0;
			for (CvUnit* pLoopUnit = firstUnit(&iLoop); NULL != pLoopUnit; pLoopUnit = nextUnit(&iLoop))
			{
				if (pLoopUnit->AI_getUnitAIType() == eIndex)
				{
					++iNumUnitAI;
				}
			}
			for (uint i = 0; i < m_aEuropeUnits.size(); ++i)
			{
				if (m_aEuropeUnits[i]->AI_getUnitAIType() == eIndex)
				{
					++iNumUnitAI;
				}
			}
			for (uint i = 0; i < m_aAfricaUnits.size(); ++i)
			{
				if (m_aAfricaUnits[i]->AI_getUnitAIType() == eIndex)
				{
					++iNumUnitAI;
				}
			}
			for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				for (int i = 0; i < pLoopCity->getPopulation(); ++i)
				{
					CvUnit* pLoopUnit = pLoopCity->getPopulationUnitByIndex(i);
					if (pLoopUnit->AI_getUnitAIType() == eIndex)
					{
						++iNumUnitAI;
					}
				}
			}
			FAssert(AI_getNumAIUnits(eIndex) == iNumUnitAI);
		}
#endif
	}
}

int CvPlayerAI::AI_getNumRetiredAIUnits(UnitAITypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_UNITAI_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiNumRetiredAIUnits[eIndex];
}


void CvPlayerAI::AI_changeNumRetiredAIUnits(UnitAITypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_UNITAI_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiNumRetiredAIUnits[eIndex] += iChange;
	FAssert(AI_getNumRetiredAIUnits(eIndex) >= 0);
}

int CvPlayerAI::AI_getPeacetimeTradeValue(PlayerTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiPeacetimeTradeValue[eIndex];
}


void CvPlayerAI::AI_changePeacetimeTradeValue(PlayerTypes eIndex, int iChange)
{
	PROFILE_FUNC();

	int iI;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiPeacetimeTradeValue[eIndex] = (m_aiPeacetimeTradeValue[eIndex] + iChange);
		FAssert(AI_getPeacetimeTradeValue(eIndex) >= 0);

		FAssert(iChange > 0);

		if (iChange > 0)
		{
			if (GET_PLAYER(eIndex).getTeam() != getTeam())
			{
				for (iI = 0; iI < MAX_TEAMS; iI++)
				{
					if (GET_TEAM((TeamTypes)iI).isAlive())
					{
						if (GET_TEAM((TeamTypes)iI).AI_getWorstEnemy() == getTeam())
						{
							GET_TEAM((TeamTypes)iI).AI_changeEnemyPeacetimeTradeValue(GET_PLAYER(eIndex).getTeam(), iChange);
						}
					}
				}
			}
		}
	}
}


int CvPlayerAI::AI_getPeacetimeGrantValue(PlayerTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiPeacetimeGrantValue[eIndex];
}


void CvPlayerAI::AI_changePeacetimeGrantValue(PlayerTypes eIndex, int iChange)
{
	PROFILE_FUNC();

	int iI;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiPeacetimeGrantValue[eIndex] = (m_aiPeacetimeGrantValue[eIndex] + iChange);
		FAssert(AI_getPeacetimeGrantValue(eIndex) >= 0);

		FAssert(iChange > 0);

		if (iChange > 0)
		{
			if (GET_PLAYER(eIndex).getTeam() != getTeam())
			{
				for (iI = 0; iI < MAX_TEAMS; iI++)
				{
					if (GET_TEAM((TeamTypes)iI).isAlive())
					{
						if (GET_TEAM((TeamTypes)iI).AI_getWorstEnemy() == getTeam())
						{
							GET_TEAM((TeamTypes)iI).AI_changeEnemyPeacetimeGrantValue(GET_PLAYER(eIndex).getTeam(), iChange);
						}
					}
				}
			}
		}
	}
}


int CvPlayerAI::AI_getGoldTradedTo(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiGoldTradedTo[eIndex];
}


void CvPlayerAI::AI_changeGoldTradedTo(PlayerTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiGoldTradedTo[eIndex] = (m_aiGoldTradedTo[eIndex] + iChange);
	FAssert(AI_getGoldTradedTo(eIndex) >= 0);
}


int CvPlayerAI::AI_getAttitudeExtra(PlayerTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiAttitudeExtra[eIndex];
}


void CvPlayerAI::AI_setAttitudeExtra(PlayerTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiAttitudeExtra[eIndex] = iNewValue;
}


void CvPlayerAI::AI_changeAttitudeExtra(PlayerTypes eIndex, int iChange)
{
	AI_setAttitudeExtra(eIndex, (AI_getAttitudeExtra(eIndex) + iChange));
}


bool CvPlayerAI::AI_isFirstContact(PlayerTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_abFirstContact[eIndex];
}


void CvPlayerAI::AI_setFirstContact(PlayerTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_abFirstContact[eIndex] = bNewValue;
}


int CvPlayerAI::AI_getContactTimer(PlayerTypes eIndex1, ContactTypes eIndex2)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < MAX_PLAYERS, "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_CONTACT_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");
	return m_aaiContactTimer[eIndex1][eIndex2];
}


void CvPlayerAI::AI_changeContactTimer(PlayerTypes eIndex1, ContactTypes eIndex2, int iChange)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < MAX_PLAYERS, "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_CONTACT_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");
	m_aaiContactTimer[eIndex1][eIndex2] = (AI_getContactTimer(eIndex1, eIndex2) + iChange);
	FAssert(AI_getContactTimer(eIndex1, eIndex2) >= 0);
}


int CvPlayerAI::AI_getMemoryCount(PlayerTypes eIndex1, MemoryTypes eIndex2)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < MAX_PLAYERS, "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_MEMORY_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");
	return m_aaiMemoryCount[eIndex1][eIndex2];
}


void CvPlayerAI::AI_changeMemoryCount(PlayerTypes eIndex1, MemoryTypes eIndex2, int iChange)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < MAX_PLAYERS, "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_MEMORY_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");
	m_aaiMemoryCount[eIndex1][eIndex2] += iChange;
	FAssert(AI_getMemoryCount(eIndex1, eIndex2) >= 0);
}

// Protected Functions...

void CvPlayerAI::AI_doTradeRoutes()
{
	//Yields are divided into several classes:
	//1) Final Products - Port cities import these and export to europe. Other cities, export them.
	//2) Utility such as Lumber, Tools - These are set Import/Export with the Maintain Level used to indicate how much are needed.
	//3) Raw Materials - Cities which consume these to produce final products, Import/Export, with a high maintain level.
	//	 Port cities Import/Export with no maintain level.

	//Generally, utility yields are set to Import/Export

	//Best Yield Destinations
	std::vector<CvCity*> yield_dests(NUM_YIELD_TYPES, NULL);

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
	{
		YieldTypes eLoopYield = (YieldTypes)iYield;

		if (GC.getYieldInfo(eLoopYield).isCargo() && (eLoopYield != YIELD_FOOD))
		{
			if (AI_isYieldFinalProduct(eLoopYield))
			{
				yield_dests[eLoopYield] = NULL;
			}
			else if ((eLoopYield == YIELD_TOOLS) || (eLoopYield == YIELD_LUMBER) || (eLoopYield == YIELD_STONE))
			{
				yield_dests[eLoopYield] = NULL;
			}
			else if ((eLoopYield == YIELD_BLADES) || (eLoopYield == YIELD_MUSKETS) || (eLoopYield == YIELD_CANNONS) || (eLoopYield == YIELD_HORSES))
			{
				yield_dests[eLoopYield] = NULL;
			}
			else
			{
				CvCity* pBestYieldCity = NULL;
				int iBestCityValue = 0;

				int iLoop;
				CvCity* pLoopCity;
				for(pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
				{
					for (int iProfession = 0; iProfession < GC.getNumProfessionInfos(); ++iProfession)
					{
						CvProfessionInfo& kLoopProfession = GC.getProfessionInfo((ProfessionTypes)iProfession);
						if (kLoopProfession.getYieldsConsumed(0) == eLoopYield)
						{
							int iValue = pLoopCity->getProfessionOutput((ProfessionTypes)iProfession, NULL);
							if (iValue > 0)
							{
								iValue *= 100;
								iValue += pLoopCity->getPopulation();
								if (iValue > iBestCityValue)
								{
									iBestCityValue = iValue;
									pBestYieldCity = pLoopCity;
								}
							}
						}
					}
				}
				yield_dests[eLoopYield] = pBestYieldCity;
				if (GC.getGameINLINE().getGameTurn() > 50)
				{
					// R&R, ray, catch NULL-POINTER
					if (pBestYieldCity != NULL)
					{
						// R&R, ray, fix for new storage capacity
						if (GC.getNEW_CAPACITY())
						{
							int iNewMaintainLevelForNewCapacity = GC.getGameINLINE().getCargoYieldCapacity();
							pBestYieldCity->setMaintainLevel(eLoopYield, iNewMaintainLevelForNewCapacity);
						}
						else
						{
							pBestYieldCity->setMaintainLevel(eLoopYield, pBestYieldCity->getMaxYieldCapacity() / 2);
						}
					}
				}
			}
		}
	}

	CvCity* pLoopCity;
	int iLoop;
	//Setup export trade routes.
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		int aiYields[NUM_YIELD_TYPES];
		pLoopCity->calculateNetYields(aiYields);

		int iCapacity = pLoopCity->getMaxYieldCapacity();
		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			YieldTypes eLoopYield = (YieldTypes)iYield;
			bool bAvailable = (aiYields[eLoopYield] > 0) || (pLoopCity->getYieldStored(eLoopYield) > 0);
			bool bShouldImport = false;
			bool bShouldExport = false;
			if (GC.getYieldInfo(eLoopYield).isCargo())
			{
				if (eLoopYield == YIELD_FOOD)
				{
					int iThreshold = pLoopCity->growthThreshold() / (2 + std::max(0, aiYields[YIELD_FOOD]));

					pLoopCity->setMaintainLevel(YIELD_FOOD, iThreshold);

					if (aiYields[YIELD_FOOD] > 0)
					{
						bShouldExport = true;
						pLoopCity->AI_setAvoidGrowth(false);
					}
					else
					{
						bShouldImport = true;
						pLoopCity->AI_setAvoidGrowth(true);
					}
				}
				// TAC - AI Economy - koma13 - START
				//else
				else if (eLoopYield != YIELD_LUMBER && eLoopYield != YIELD_STONE)
				// TAC - AI Economy - koma13 - END
				{
					if ((AI_isYieldFinalProduct(eLoopYield)) || AI_isYieldForSale(eLoopYield))
					{
						if (pLoopCity->AI_isPort())
						{
							bShouldImport = true;
						}
						else if (bAvailable)
						{
							bShouldExport = true;
						}
					}
					if (yield_dests[eLoopYield] == pLoopCity)
					{
						bShouldImport = true;
					}
					else if (yield_dests[eLoopYield] != NULL)
					{
						// TAC - AI Economy - koma13 - START
						for (int iProfession = 0; iProfession < GC.getNumProfessionInfos(); ++iProfession)
						{
							CvProfessionInfo& kLoopProfession = GC.getProfessionInfo((ProfessionTypes)iProfession);
							// R&R, ray , MYCP partially based on code of Aymerick - START
							bool bYieldSearched = false;
							if (kLoopProfession.getNumYieldsConsumed() == 1)
							{
								if (kLoopProfession.getYieldsConsumed(0) == eLoopYield)
								{
									bYieldSearched = true;
								}
							}
							else if (kLoopProfession.getNumYieldsConsumed() == 2)
							{
								if (kLoopProfession.getYieldsConsumed(0) == eLoopYield || kLoopProfession.getYieldsConsumed(1) == eLoopYield)
								{
									bYieldSearched = true;
								}
							}
							if (bYieldSearched)
							// R&R, ray , MYCP partially based on code of Aymerick - END
							{
								if (pLoopCity->professionCount((ProfessionTypes)iProfession) > 0)
								{
									bShouldImport = true;
									// R&R, ray, fix for new storage capacity
									if (GC.getNEW_CAPACITY())
									{
										int iNewMaintainLevelForNewCapacity = GC.getGameINLINE().getCargoYieldCapacity();
										pLoopCity->setMaintainLevel(eLoopYield, iNewMaintainLevelForNewCapacity);
									}
									else
									{
										pLoopCity->setMaintainLevel(eLoopYield, pLoopCity->getMaxYieldCapacity() / 3);
									}
								}
							}
						}
						//if (bAvailable)
						if (bAvailable && !bShouldImport)
						// TAC - AI Economy - koma13 - END
						{
							bShouldExport = true;
						}
					}
					else
					{
						int iMaintainLevel = pLoopCity->getMaintainLevel(eLoopYield);
						int iStored = pLoopCity->getYieldStored(eLoopYield);

//VET NewCapacity - begin 1/8
						if (GC.getNEW_CAPACITY())
						{
							int iCargoYields = 0;
							for (int iYield = 3; iYield < NUM_YIELD_TYPES; iYield++)//without YIELD_FOOD, YIELD_LUMBER, YIELD STONE
							{
								if ((pLoopCity->getYieldStored((YieldTypes)iYield) > 0) && (GC.getYieldInfo((YieldTypes)iYield).isCargo()))
									{iCargoYields++;}
							}
							if (iCargoYields < 1)
								{iCargoYields = 1;}
							iMaintainLevel /= iCargoYields;
							if (iStored > iMaintainLevel)
							{
								if ((aiYields[eLoopYield] > 0) || (pLoopCity->getTotalYieldStored() > (iCapacity * 90) / 100) || pLoopCity->isBestPortCity())
								{
									bShouldExport = true;
								}	
							}
							else if (iMaintainLevel > 0 && iStored < iMaintainLevel)
							{
								bShouldImport = true;

							}
						}
						else
						{
//VET NewCapacity - end 1/8					
							if (iStored > iMaintainLevel)
							{
								// TAC - AI Economy - koma13 - START
								//if ((aiYields[eLoopYield] > 0) || (iStored > (iCapacity * 90) / 100) || pLoopCity->AI_isPort())
								if ((aiYields[eLoopYield] > 0) || (iStored > (iCapacity * 90) / 100) || pLoopCity->isBestPortCity())
								// TAC - AI Economy - koma13 - END
								{
									bShouldExport = true;
								}
							}
							else if (iMaintainLevel > 0 && iStored < iMaintainLevel)
							{
								bShouldImport = true;
							}
//VET NewCapacity - begin 2/8
						}
//VET NewCapacity - end 2/8
					}
				}
				if (bShouldImport)
				{
					pLoopCity->addImport(eLoopYield);
				}
				else
				{
					pLoopCity->removeImport(eLoopYield);
				}
				if (bShouldExport)
				{
					pLoopCity->addExport(eLoopYield);
				}
				else
				{
					pLoopCity->removeExport(eLoopYield);
				}
			}
		}
	}
}

void CvPlayerAI::AI_doCounter()
{
	int iI, iJ;

	// R&R, ray, small code change from Commander Bello, united 2 for loops - START
	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			for (iJ = 0; iJ < NUM_CONTACT_TYPES; iJ++)
			{
				if (AI_getContactTimer(((PlayerTypes)iI), ((ContactTypes)iJ)) > 0)
				{
					AI_changeContactTimer(((PlayerTypes)iI), ((ContactTypes)iJ), -1);
				}
			}

			for (iJ = 0; iJ < NUM_MEMORY_TYPES; iJ++)
			{
				if (AI_getMemoryCount(((PlayerTypes)iI), ((MemoryTypes)iJ)) > 0)
				{
					if (GC.getLeaderHeadInfo(getPersonalityType()).getMemoryDecayRand(iJ) > 0)
					{
						if (GC.getGameINLINE().getSorenRandNum(GC.getLeaderHeadInfo(getPersonalityType()).getMemoryDecayRand(iJ), "Memory Decay") == 0)
						{
							AI_changeMemoryCount(((PlayerTypes)iI), ((MemoryTypes)iJ), -1);
						}
					}
				}
			}
		}
	}
	// R&R, ray, small code change from Commander Bello, united 2 for loops - END

	// TAC - AI Improved Navel AI - koma13 - START
	for (int i = 0; i < GC.getMap().numPlotsINLINE(); ++i)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndexINLINE(i);
		
		int iDanger = pLoopPlot->getDangerMap(getID());
		
		if (iDanger > 0)
		{
			pLoopPlot->setDangerMap(getID(), --iDanger);
		}
	}
	// TAC - AI Improved Navel AI - koma13 - END
	// TAC - AI Revolution - koma13 - START
	int iLastWave = AI_getLastWave();
	if (iLastWave > -1)
	{
		AI_setLastWave(iLastWave + 1);
	}
	// TAC - AI Revolution - koma13 - END
}


void CvPlayerAI::AI_doMilitary()
{


	AI_setAttackOddsChange(GC.getLeaderHeadInfo(getPersonalityType()).getBaseAttackOddsChange() +
		GC.getGameINLINE().getSorenRandNum(GC.getLeaderHeadInfo(getPersonalityType()).getAttackOddsChangeRand(), "AI Attack Odds Change #1") +
		GC.getGameINLINE().getSorenRandNum(GC.getLeaderHeadInfo(getPersonalityType()).getAttackOddsChangeRand(), "AI Attack Odds Change #2"));
}

void CvPlayerAI::AI_doDiplo()
{
	PROFILE_FUNC();

	FAssert(!isHuman());

	if (GC.getUSE_AI_DO_DIPLO_CALLBACK()) // K-Mod. block unused python callbacks
	{
		// allow python to handle it
		CyArgsList argsList;
		argsList.add(getID());
		long lResult = 0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "AI_doDiplo", argsList.makeFunctionArgs(), &lResult);
		if (lResult == 1)
		{
			return;
		}
	}

	// R&R, ray, the Church - START
	if (GC.getGameINLINE().isChurchPlayer(getID()))
	{
		return; // we do not want Church to initiate Trade, because currently that is useless
	}
	// R&R, ray, the Church - END

	std::vector<bool> abContacted(MAX_TEAMS, false);

	for (int iPass = 0; iPass < 2; iPass++)
	{
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			PlayerTypes ePlayer = (PlayerTypes) iI;
			CvPlayer& kPlayer = GET_PLAYER(ePlayer);
			if (kPlayer.isAlive() && ePlayer != getID())
			{
				if (kPlayer.isHuman() == (iPass == 1))
				{
					if (AI_doDiploCancelDeals((PlayerTypes) iI))
					{
						if (kPlayer.isHuman())
						{
							abContacted[kPlayer.getTeam()] = true;
						}
					}

					if (canContact(ePlayer) && AI_isWillingToTalk(ePlayer))
					{
						if (kPlayer.getTeam() != getTeam() && !(GET_TEAM(getTeam()).isHuman()) && (kPlayer.isHuman() || !(GET_TEAM(kPlayer.getTeam()).isHuman())))
						{
							FAssertMsg(iI != getID(), "iI is not expected to be equal with getID()");

							if (!(GET_TEAM(getTeam()).isAtWar(kPlayer.getTeam())))
							{
								if (AI_doDiploOfferCity(ePlayer))
								{
									if (kPlayer.isHuman())
									{
										abContacted[kPlayer.getTeam()] = true;
									}
								}

								if (!kPlayer.isHuman() || !abContacted[kPlayer.getTeam()])
								{
									if (AI_doDiploOfferAlliance(ePlayer))
									{
										if (kPlayer.isHuman())
										{
											abContacted[kPlayer.getTeam()] = true;
										}
										else
										{
											// move on to the next player since we are on the same team now
											break;
										}
									}
								}

								if (!kPlayer.isHuman() || !abContacted[kPlayer.getTeam()])
								{
									if (AI_doDiploAskJoinWar(ePlayer))
									{
										if (kPlayer.isHuman())
										{
											abContacted[kPlayer.getTeam()] = true;
										}
									}
								}

								if (!kPlayer.isHuman() || !abContacted[kPlayer.getTeam()])
								{
									if (AI_doDiploAskStopTrading(ePlayer))
									{
										if (kPlayer.isHuman())
										{
											abContacted[kPlayer.getTeam()] = true;
										}
									}
								}

								if (!kPlayer.isHuman() || !abContacted[kPlayer.getTeam()])
								{
									if (AI_doDiploGiveHelp(ePlayer))
									{
										if (kPlayer.isHuman())
										{
											abContacted[kPlayer.getTeam()] = true;
										}
									}
								}

								if (!kPlayer.isHuman() || !abContacted[kPlayer.getTeam()])
								{
									if (AI_doDiploAskForHelp(ePlayer))
									{
										if (kPlayer.isHuman())
										{
											abContacted[kPlayer.getTeam()] = true;
										}
									}
								}

								if (!kPlayer.isHuman() || !abContacted[kPlayer.getTeam()])
								{
									if (AI_doDiploDemandTribute(ePlayer))
									{
										if (kPlayer.isHuman())
										{
											abContacted[kPlayer.getTeam()] = true;
										}
									}
								}

								if (!kPlayer.isHuman() || !abContacted[kPlayer.getTeam()])
								{
									if (AI_doDiploKissPinky(ePlayer))
									{
										if (kPlayer.isHuman())
										{
											abContacted[kPlayer.getTeam()] = true;
										}
									}
								}

								if (!kPlayer.isHuman() || !abContacted[kPlayer.getTeam()])
								{
									if (AI_doDiploOpenBorders(ePlayer))
									{
										if (kPlayer.isHuman())
										{
											abContacted[kPlayer.getTeam()] = true;
										}
									}
								}

								if (!kPlayer.isHuman() || !abContacted[kPlayer.getTeam()])
								{
									if (AI_doDiploDefensivePact(ePlayer))
									{
										if (kPlayer.isHuman())
										{
											abContacted[kPlayer.getTeam()] = true;
										}
									}
								}

								if (!kPlayer.isHuman() || !abContacted[kPlayer.getTeam()])
								{
									if (AI_doDiploTradeMap(ePlayer))
									{
										if (kPlayer.isHuman())
										{
											abContacted[kPlayer.getTeam()] = true;
										}
									}
								}

								if (!kPlayer.isHuman() || !abContacted[kPlayer.getTeam()])
								{
									if (AI_doDiploDeclareWar(ePlayer))
									{
										if (kPlayer.isHuman())
									{
											abContacted[kPlayer.getTeam()] = true;
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
}


bool CvPlayerAI::AI_doDiploCancelDeals(PlayerTypes ePlayer)
{
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	if (kPlayer.getTeam() == getTeam())
	{
		return false;
	}

	bool bKilled = false;

	int iLoop;
	for (CvDeal* pLoopDeal = GC.getGameINLINE().firstDeal(&iLoop); pLoopDeal != NULL; pLoopDeal = GC.getGameINLINE().nextDeal(&iLoop))
	{
		if (pLoopDeal->isCancelable(getID()))
		{
			if ((GC.getGameINLINE().getGameTurn() - pLoopDeal->getInitialGameTurn()) >= (GC.getDefineINT("PEACE_TREATY_LENGTH") * 2))
			{
				bool bCancelDeal = false;

				if ((pLoopDeal->getFirstPlayer() == getID()) && (pLoopDeal->getSecondPlayer() == ePlayer))
				{
					if (kPlayer.isHuman())
					{
						if (!AI_considerOffer(ePlayer, pLoopDeal->getSecondTrades(), pLoopDeal->getFirstTrades(), -1))
						{
							bCancelDeal = true;
						}
					}
					else
					{
						for (CLLNode<TradeData>* pNode = pLoopDeal->getFirstTrades()->head(); pNode; pNode = pLoopDeal->getFirstTrades()->next(pNode))
						{
							if (getTradeDenial(ePlayer, pNode->m_data) != NO_DENIAL)
							{
								bCancelDeal = true;
								break;
							}
						}
					}
				}
				else if ((pLoopDeal->getFirstPlayer() == ePlayer) && (pLoopDeal->getSecondPlayer() == getID()))
				{
					if (kPlayer.isHuman())
					{
						if (!AI_considerOffer(ePlayer, pLoopDeal->getFirstTrades(), pLoopDeal->getSecondTrades(), -1))
						{
							bCancelDeal = true;
						}
					}
					else
					{
						for (CLLNode<TradeData>* pNode = pLoopDeal->getSecondTrades()->head(); pNode; pNode = pLoopDeal->getSecondTrades()->next(pNode))
						{
							if (getTradeDenial(ePlayer, pNode->m_data) != NO_DENIAL)
							{
								bCancelDeal = true;
								break;
							}
						}
					}
				}

				if (bCancelDeal)
				{
					if (canContact(ePlayer) && AI_isWillingToTalk(ePlayer))
					{
						if (kPlayer.isHuman())
						{
							CLinkList<TradeData> ourList;
							CLinkList<TradeData> theirList;

							for (CLLNode<TradeData>* pNode = pLoopDeal->headFirstTradesNode(); (pNode != NULL); pNode = pLoopDeal->nextFirstTradesNode(pNode))
							{
								if (pLoopDeal->getFirstPlayer() == getID())
								{
									ourList.insertAtEnd(pNode->m_data);
								}
								else
								{
									theirList.insertAtEnd(pNode->m_data);
								}
							}

							for (pNode = pLoopDeal->headSecondTradesNode(); (pNode != NULL); pNode = pLoopDeal->nextSecondTradesNode(pNode))
							{
								if (pLoopDeal->getSecondPlayer() == getID())
								{
									ourList.insertAtEnd(pNode->m_data);
								}
								else
								{
									theirList.insertAtEnd(pNode->m_data);
								}
							}

							CvDiploParameters* pDiplo = new CvDiploParameters(getID());
							pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_CANCEL_DEAL"));
							pDiplo->setAIContact(true);
							pDiplo->setOurOfferList(theirList);
							pDiplo->setTheirOfferList(ourList);
							gDLL->beginDiplomacy(pDiplo, ePlayer);
						}
					}

					pLoopDeal->kill(true, getTeam()); // XXX test this for AI...
					bKilled = true;
				}
			}
		}
	}

	return bKilled;
}



bool CvPlayerAI::AI_doDiploOfferCity(PlayerTypes ePlayer)
{
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	if (AI_getAttitude(ePlayer) < ATTITUDE_CAUTIOUS)
	{
		return false;
	}

	bool bOffered = false;
	int iLoop;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->getPreviousOwner() != ePlayer)
		{
			if (((pLoopCity->getGameTurnAcquired() + 4) % 20) == (GC.getGameINLINE().getGameTurn() % 20))
			{
				int iCount = 0;
				int iPossibleCount = 0;

				for (int iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
				{
					CvPlot* pLoopPlot = plotCity(pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE(), iJ);

					if (pLoopPlot != NULL)
					{
						if (pLoopPlot->getOwnerINLINE() == ePlayer)
						{
							++iCount;
						}

						++iPossibleCount;
					}
				}

				if (iCount >= (iPossibleCount / 2))
				{
					TradeData item;
					setTradeItem(&item, TRADE_CITIES, pLoopCity->getID(), NULL);

					if (canTradeItem((ePlayer), item, true))
					{
						CLinkList<TradeData> ourList;
						ourList.insertAtEnd(item);

						if (kPlayer.isHuman())
						{
							CvDiploParameters* pDiplo = new CvDiploParameters(getID());
							pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_OFFER_CITY"));
							pDiplo->setAIContact(true);
							pDiplo->setTheirOfferList(ourList);
							gDLL->beginDiplomacy(pDiplo, ePlayer);
						}
						else
						{
							GC.getGameINLINE().implementDeal(getID(), (ePlayer), &ourList, NULL);
						}
						bOffered = true;
					}
				}
			}
		}
	}

	return bOffered;
}


bool CvPlayerAI::AI_doDiploOfferAlliance(PlayerTypes ePlayer)
{
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	if (GET_TEAM(getTeam()).getLeaderID() != getID())
	{
		return false;
	}

	if (kPlayer.getParent() == getID())
	{
		return false;
	}

	if (AI_getContactTimer((ePlayer), CONTACT_PERMANENT_ALLIANCE) > 0)
	{
		return false;
	}

	if (GC.getGameINLINE().getSorenRandNum(GC.getLeaderHeadInfo(getPersonalityType()).getContactRand(CONTACT_PERMANENT_ALLIANCE), "AI Diplo Alliance") != 0)
	{
		return false;
	}

	bool bOffered = false;
	TradeData item;
	setTradeItem(&item, TRADE_PERMANENT_ALLIANCE, 0, NULL);

	if (canTradeItem((ePlayer), item, true) && kPlayer.canTradeItem(getID(), item, true))
	{
		CLinkList<TradeData> ourList;
		CLinkList<TradeData> theirList;
		ourList.insertAtEnd(item);
		theirList.insertAtEnd(item);

		bOffered = true;

		if (kPlayer.isHuman())
		{
			AI_changeContactTimer(ePlayer, CONTACT_PERMANENT_ALLIANCE, GC.getLeaderHeadInfo(getPersonalityType()).getContactDelay(CONTACT_PERMANENT_ALLIANCE));
			CvDiploParameters* pDiplo = new CvDiploParameters(getID());
			FAssertMsg(pDiplo != NULL, "pDiplo must be valid");
			pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_OFFER_DEAL"));
			pDiplo->setAIContact(true);
			pDiplo->setOurOfferList(theirList);
			pDiplo->setTheirOfferList(ourList);
			gDLL->beginDiplomacy(pDiplo, ePlayer);
		}
		else
		{
			GC.getGameINLINE().implementDeal(getID(), (ePlayer), &ourList, &theirList);
		}
	}

	return bOffered;
}


bool CvPlayerAI::AI_doDiploAskJoinWar(PlayerTypes ePlayer)
{
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	if (!kPlayer.isHuman())
	{
		return false;
	}

	if (GET_TEAM(getTeam()).getLeaderID() != getID())
	{
		return false;
	}

	if ((AI_getMemoryCount(ePlayer, MEMORY_DECLARED_WAR) > 0) || (AI_getMemoryCount(ePlayer, MEMORY_HIRED_WAR_ALLY) > 0))
	{
		return false;
	}

	if (AI_getContactTimer(ePlayer, CONTACT_JOIN_WAR) > 0)
	{
		return false;
	}

	if (GC.getGameINLINE().getSorenRandNum(GC.getLeaderHeadInfo(getPersonalityType()).getContactRand(CONTACT_JOIN_WAR), "AI Diplo Join War") != 0)
	{
		return false;
	}

	int iBestValue = 0;
	TeamTypes eBestTeam = NO_TEAM;
	for (int iJ = 0; iJ < MAX_TEAMS; ++iJ)
	{
		TeamTypes eLoopTeam = (TeamTypes) iJ;
		CvTeam& kLoopTeam = GET_TEAM(eLoopTeam);
		if (kLoopTeam.isAlive())
		{
			if (atWar(eLoopTeam, getTeam()) && !atWar(eLoopTeam, kPlayer.getTeam()))
			{
				if (GET_TEAM(kPlayer.getTeam()).isHasMet(eLoopTeam))
				{
					if (GET_TEAM(kPlayer.getTeam()).canDeclareWar(eLoopTeam))
					{
						int iValue = (1 + GC.getGameINLINE().getSorenRandNum(10000, "AI Joining War"));

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							eBestTeam = eLoopTeam;
						}
					}
				}
			}
		}
	}

	if (eBestTeam == NO_TEAM)
	{
		return false;
	}

	AI_changeContactTimer(ePlayer, CONTACT_JOIN_WAR, GC.getLeaderHeadInfo(getPersonalityType()).getContactDelay(CONTACT_JOIN_WAR));
	CvDiploParameters* pDiplo = new CvDiploParameters(getID());
	pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_JOIN_WAR"));
	pDiplo->addDiploCommentVariable(GET_PLAYER(GET_TEAM(eBestTeam).getLeaderID()).getCivilizationAdjectiveKey());
	pDiplo->setAIContact(true);
	pDiplo->setData(eBestTeam);
	gDLL->beginDiplomacy(pDiplo, ePlayer);

	return true;
}

bool CvPlayerAI::AI_doDiploAskStopTrading(PlayerTypes ePlayer)
{
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	if (!kPlayer.isHuman())
	{
		return false;
	}

	if (GET_TEAM(getTeam()).getLeaderID() != getID())
	{
		return false;
	}

	if (AI_getContactTimer((ePlayer), CONTACT_STOP_TRADING) > 0)
	{
		return false;
	}

	if (GC.getGameINLINE().getSorenRandNum(GC.getLeaderHeadInfo(getPersonalityType()).getContactRand(CONTACT_STOP_TRADING), "AI Diplo Stop Trading") != 0)
	{
		return false;
	}

	TeamTypes eBestTeam = GET_TEAM(getTeam()).AI_getWorstEnemy();
	if (eBestTeam == NO_TEAM)
	{
		return false;
	}

	if (!GET_TEAM(kPlayer.getTeam()).isHasMet(eBestTeam))
	{
		return false;
	}

	if (GET_TEAM(getTeam()).isParentOf(eBestTeam) && !::atWar(getTeam(), eBestTeam))
	{
		return false;
	}

	if (!kPlayer.canStopTradingWithTeam(eBestTeam))
	{
		return false;
	}

	FAssert(!atWar(kPlayer.getTeam(), eBestTeam));
	FAssert(kPlayer.getTeam() != eBestTeam);

	AI_changeContactTimer(ePlayer, CONTACT_STOP_TRADING, GC.getLeaderHeadInfo(getPersonalityType()).getContactDelay(CONTACT_STOP_TRADING));
	CvDiploParameters* pDiplo = new CvDiploParameters(getID());
	pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_STOP_TRADING"));
	pDiplo->addDiploCommentVariable(GET_PLAYER(GET_TEAM(eBestTeam).getLeaderID()).getCivilizationAdjectiveKey());
	pDiplo->setAIContact(true);
	pDiplo->setData(eBestTeam);
	gDLL->beginDiplomacy(pDiplo, ePlayer);

	return true;
}

bool CvPlayerAI::AI_doDiploGiveHelp(PlayerTypes ePlayer)
									{
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	if (!kPlayer.isHuman())
	{
		return false;
	}

	if (isNative())
	{
		return false;
	}

	if (GET_TEAM(getTeam()).getLeaderID() != getID())
	{
		return false;
	}

	if (AI_getAttitude(ePlayer) <= GC.getLeaderHeadInfo(kPlayer.getPersonalityType()).getNoGiveHelpAttitudeThreshold())
	{
		return false;
	}

	if (AI_getContactTimer((ePlayer), CONTACT_GIVE_HELP) > 0)
	{
		return false;
	}

	if (GET_TEAM(kPlayer.getTeam()).getAssets() > GET_TEAM(getTeam()).getAssets() / 2)
	{
		return false;
	}

	if (GC.getGameINLINE().getSorenRandNum(GC.getLeaderHeadInfo(getPersonalityType()).getContactRand(CONTACT_GIVE_HELP), "AI Diplo Give Help") != 0)
	{
		return false;
	}

	int iGold = AI_maxGoldTrade(ePlayer);
	if (iGold <= 0)
	{
		return false;
	}

	TradeData item;
	setTradeItem(&item, TRADE_GOLD, iGold, NULL);
	if (!canTradeItem(ePlayer, item, true))
	{
		return false;
	}

	CLinkList<TradeData> ourList;
	ourList.insertAtEnd(item);

	AI_changeContactTimer((ePlayer), CONTACT_GIVE_HELP, GC.getLeaderHeadInfo(getPersonalityType()).getContactDelay(CONTACT_GIVE_HELP));
	CvDiploParameters* pDiplo = new CvDiploParameters(getID());
	pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_GIVE_HELP"));
	pDiplo->setTheirOfferList(ourList);
	pDiplo->setAIContact(true);
	gDLL->beginDiplomacy(pDiplo, ePlayer);

	return true;
}


bool CvPlayerAI::AI_doDiploAskForHelp(PlayerTypes ePlayer)
{
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	if (!kPlayer.isHuman())
	{
		return false;
	}

	if (GET_TEAM(getTeam()).getLeaderID() != getID())
	{
		return false;
	}

	if (AI_getContactTimer((ePlayer), CONTACT_ASK_FOR_HELP) > 0)
	{
		return false;
	}

	if (GET_TEAM(kPlayer.getTeam()).getAssets() <= GET_TEAM(getTeam()).getAssets() / 2)
	{
		return false;
	}

	if (GC.getGameINLINE().getSorenRandNum(GC.getLeaderHeadInfo(getPersonalityType()).getContactRand(CONTACT_ASK_FOR_HELP), "AI Diplo Ask for Help") != 0)
	{
		return false;
	}

	int iGold = kPlayer.AI_maxGoldTrade(getID()) * GC.getGameINLINE().getSorenRandNum(100, "Ask for gold percent") / 100;
	if (iGold <= 0)
	{
		return false;
	}

	TradeData item;
	setTradeItem(&item, TRADE_GOLD, iGold, NULL);
	if (!canTradeItem(ePlayer, item, true))
	{
		return false;
	}

	CLinkList<TradeData> theirList;
	theirList.insertAtEnd(item);

	AI_changeContactTimer((ePlayer), CONTACT_GIVE_HELP, GC.getLeaderHeadInfo(getPersonalityType()).getContactDelay(CONTACT_GIVE_HELP));
	CvDiploParameters* pDiplo = new CvDiploParameters(getID());
	pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_ASK_FOR_HELP"));
	pDiplo->setOurOfferList(theirList);
	pDiplo->setAIContact(true);
	gDLL->beginDiplomacy(pDiplo, ePlayer);

	return true;
}


bool CvPlayerAI::AI_doDiploDemandTribute(PlayerTypes ePlayer)
{
	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);

	if (!kPlayer.isHuman())
	{
		return false;
	}

	if (GET_TEAM(getTeam()).getLeaderID() != getID())
	{
		return false;
	}

	if (!GET_TEAM(getTeam()).canDeclareWar(kPlayer.getTeam()))
	{
		return false;
	}

	if (GET_TEAM(getTeam()).AI_isSneakAttackPreparing(kPlayer.getTeam()))
	{
		return false;
	}

	if (GET_TEAM(kPlayer.getTeam()).getDefensivePower() >= GET_TEAM(getTeam()).getPower())
	{
		return false;
	}

	if (AI_getAttitude(ePlayer) > GC.getLeaderHeadInfo(kPlayer.getPersonalityType()).getDemandTributeAttitudeThreshold())
	{
		return false;
	}

	if (AI_getContactTimer((ePlayer), CONTACT_DEMAND_TRIBUTE) > 0)
	{
		return false;
	}

	if (GC.getGameINLINE().getSorenRandNum(GC.getLeaderHeadInfo(getPersonalityType()).getContactRand(CONTACT_DEMAND_TRIBUTE), "AI Diplo Demand Tribute") != 0)
	{
		return false;
	}

	TradeData item;
	int iReceiveGold = std::min(std::max(0, (kPlayer.getGold() - 50)), kPlayer.AI_goldTarget());
	iReceiveGold -= (iReceiveGold % GC.getDefineINT("DIPLOMACY_VALUE_REMAINDER"));
	if (iReceiveGold > 50)
	{
		setTradeItem(&item, TRADE_GOLD, iReceiveGold, NULL);
	}
	else if (GET_TEAM(getTeam()).AI_mapTradeVal(kPlayer.getTeam()) > 100)
	{
		setTradeItem(&item, TRADE_MAPS, 0, NULL);
	}

	if (!canTradeItem(ePlayer, item, true))
	{
		return false;
	}

	CLinkList<TradeData> theirList;
	theirList.insertAtEnd(item);

	AI_changeContactTimer((ePlayer), CONTACT_DEMAND_TRIBUTE, GC.getLeaderHeadInfo(getPersonalityType()).getContactDelay(CONTACT_DEMAND_TRIBUTE));
	CvDiploParameters* pDiplo = new CvDiploParameters(getID());
	pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_DEMAND_TRIBUTE"));
	pDiplo->setAIContact(true);
	pDiplo->setOurOfferList(theirList);
	gDLL->beginDiplomacy(pDiplo, ePlayer);

	return true;
}

/** NBMOD TAX **/

/***************************************************************************/
/**                                                                       **/
/** int CvPlayerAI::NBMOD_GetGoldAsk(int iWantedGold)                     **/
/**                                                                       **/
/** Diese Methode überwacht die Gold-Höchstgrenze.                        **/
/**                                                                       **/
/** Parameter:                                                            **/
/**  - ePlayer     = der Spieler                                          **/
/**                                                                       **/
/***************************************************************************/

int CvPlayerAI::NBMOD_GetGoldAsk(PlayerTypes ePlayer) const
{
	int iGold = GET_PLAYER(ePlayer).getGold(); // aktuellen Goldstand ermitteln
	int iMaxGold = iGold * GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).NBMOD_GetMaxGoldAskPercent() / 100;;
	int iMinGold = iGold * GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).NBMOD_GetMinGoldAskPercent() / 100;;
	int iWantedGold = 0;

	iMaxGold = iMaxGold - iMinGold;

	iWantedGold = iMinGold + iMaxGold * GC.getGameINLINE().getSorenRandNum(100, "Ask for pinky gold percent") / 100;

	return iWantedGold;
}

/** NBMOD TAX **/

bool CvPlayerAI::AI_doDiploKissPinky(PlayerTypes ePlayer)
{
	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);

	if (!kPlayer.isHuman())
	{
		return false;
	}

	if (GC.getEraInfo(kPlayer.getCurrentEra()).isRevolution())
	{
		return false;
	}

	if (kPlayer.getNumCities() == 0)
	{
		return false;
	}

	if (kPlayer.getParent() != getID())
	{
		return false;
	}

	if (GET_TEAM(getTeam()).getLeaderID() != getID())
	{
		return false;
	}

	if (AI_getContactTimer((ePlayer), CONTACT_DEMAND_TRIBUTE) > 0)
	{
		return false;
	}

	if (GC.getGameINLINE().getSorenRandNum(GC.getLeaderHeadInfo(getPersonalityType()).getContactRand(CONTACT_DEMAND_TRIBUTE), "AI Diplo Kiss Pinky") != 0)
	{
		return false;
	}

    /** NBMOD TAX **/
    /** Original
	int iMaxGoldPercent = GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIDeclareWarProb() * kPlayer.AI_maxGoldTrade(getID()) / 100;
	int iReceiveGold = iMaxGoldPercent * GC.getGameINLINE().getSorenRandNum(100, "Ask for pinky gold percent") / 100;
	iReceiveGold -= (iReceiveGold % GC.getDefineINT("DIPLOMACY_VALUE_REMAINDER"));
	**/

	int iReceiveGold = NBMOD_GetGoldAsk(ePlayer);

	/** NBMOD TAX **/
	if (iReceiveGold <= 0)
	{
		return false;
	}

	AI_changeContactTimer((ePlayer), CONTACT_DEMAND_TRIBUTE, GC.getLeaderHeadInfo(getPersonalityType()).getContactDelay(CONTACT_DEMAND_TRIBUTE));

	CvDiploParameters* pDiplo = new CvDiploParameters(getID());
	pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_KING_ASK_FOR_GOLD"));
	pDiplo->addDiploCommentVariable(iReceiveGold);
	pDiplo->setData(iReceiveGold);
	pDiplo->setAIContact(true);
	gDLL->beginDiplomacy(pDiplo, ePlayer);

	return true;
}

bool CvPlayerAI::AI_doDiploOpenBorders(PlayerTypes ePlayer)
{
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	if (GET_TEAM(getTeam()).getLeaderID() != getID())
	{
		return false;
	}

	if (getNumCities() == 0)
	{
		return false;
	}

	if (kPlayer.getNumCities() == 0)
	{
		return false;
	}

	if (AI_getContactTimer(ePlayer, CONTACT_OPEN_BORDERS) > 0)
	{
		return false;
	}

	if (GC.getGameINLINE().getSorenRandNum(GC.getLeaderHeadInfo(getPersonalityType()).getContactRand(CONTACT_OPEN_BORDERS), "AI Diplo Open Borders") != 0)
	{
		return false;
	}

	TradeData item;
	setTradeItem(&item, TRADE_OPEN_BORDERS, 0, NULL);

	if (!canTradeItem(ePlayer, item, true) || !kPlayer.canTradeItem(getID(), item, true))
	{
		return false;
	}

	CLinkList<TradeData> theirList;
	theirList.insertAtEnd(item);
	CLinkList<TradeData> ourList;
	ourList.insertAtEnd(item);

	if (kPlayer.isHuman())
	{
		AI_changeContactTimer(ePlayer, CONTACT_OPEN_BORDERS, GC.getLeaderHeadInfo(getPersonalityType()).getContactDelay(CONTACT_OPEN_BORDERS));
		CvDiploParameters* pDiplo = new CvDiploParameters(getID());
		pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_OFFER_DEAL"));
		pDiplo->setAIContact(true);
		pDiplo->setOurOfferList(theirList);
		pDiplo->setTheirOfferList(ourList);
		gDLL->beginDiplomacy(pDiplo, ePlayer);
	}
	else
	{
		GC.getGameINLINE().implementDeal(getID(), (ePlayer), &ourList, &theirList);
	}

	return true;
}

bool CvPlayerAI::AI_doDiploDefensivePact(PlayerTypes ePlayer)
{
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	if (GET_TEAM(getTeam()).getLeaderID() != getID())
	{
		return false;
	}

	if (kPlayer.getParent() == getID())
	{
		return false;
	}

	if (isEurope())
	{
		return false;
	}

	if (AI_getContactTimer(ePlayer, CONTACT_DEFENSIVE_PACT) > 0)
	{
		return false;
	}

	if (GC.getGameINLINE().getSorenRandNum(GC.getLeaderHeadInfo(getPersonalityType()).getContactRand(CONTACT_DEFENSIVE_PACT), "AI Diplo Defensive Pact") != 0)
	{
		return false;
	}

	TradeData item;
	setTradeItem(&item, TRADE_DEFENSIVE_PACT, 0, NULL);

	if (!canTradeItem(ePlayer, item, true) || !kPlayer.canTradeItem(getID(), item, true))
	{
		return false;
	}

	CLinkList<TradeData> theirList;
	theirList.insertAtEnd(item);
	CLinkList<TradeData> ourList;
	ourList.insertAtEnd(item);

	if (kPlayer.isHuman())
	{
		AI_changeContactTimer(ePlayer, CONTACT_DEFENSIVE_PACT, GC.getLeaderHeadInfo(getPersonalityType()).getContactDelay(CONTACT_DEFENSIVE_PACT));
		CvDiploParameters* pDiplo = new CvDiploParameters(getID());
		pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_OFFER_DEAL"));
		pDiplo->setAIContact(true);
		pDiplo->setOurOfferList(theirList);
		pDiplo->setTheirOfferList(ourList);
		gDLL->beginDiplomacy(pDiplo, ePlayer);
	}
	else
	{
		GC.getGameINLINE().implementDeal(getID(), ePlayer, &ourList, &theirList);
	}

	return true;
}

bool CvPlayerAI::AI_doDiploTradeMap(PlayerTypes ePlayer)
{
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	if (AI_getContactTimer(ePlayer, CONTACT_TRADE_MAP) > 0)
	{
		return false;
	}

	if (isEurope())
	{
		return false;
	}

	if (GC.getGameINLINE().getSorenRandNum(GC.getLeaderHeadInfo(getPersonalityType()).getContactRand(CONTACT_TRADE_MAP), "AI Diplo Trade Map") != 0)
	{
		return false;
	}

	TradeData item;
	setTradeItem(&item, TRADE_MAPS, 0, NULL);

	if (!kPlayer.canTradeItem(getID(), item, true) || !canTradeItem(ePlayer, item, true))
	{
		return false;
	}

	if (kPlayer.isHuman() && GET_TEAM(getTeam()).AI_mapTradeVal(kPlayer.getTeam()) < GET_TEAM(kPlayer.getTeam()).AI_mapTradeVal(getTeam()))
	{
		return false;
	}

	CLinkList<TradeData> theirList;
	theirList.insertAtEnd(item);
	CLinkList<TradeData> ourList;
	ourList.insertAtEnd(item);

	if (kPlayer.isHuman())
	{
		AI_changeContactTimer((ePlayer), CONTACT_TRADE_MAP, GC.getLeaderHeadInfo(getPersonalityType()).getContactDelay(CONTACT_TRADE_MAP));
		CvDiploParameters* pDiplo = new CvDiploParameters(getID());
		pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_OFFER_DEAL"));
		pDiplo->setAIContact(true);
		pDiplo->setOurOfferList(theirList);
		pDiplo->setTheirOfferList(ourList);
		gDLL->beginDiplomacy(pDiplo, ePlayer);
	}
	else
	{
		GC.getGameINLINE().implementDeal(getID(), ePlayer, &ourList, &theirList);
	}

	return true;
}

bool CvPlayerAI::AI_doDiploDeclareWar(PlayerTypes ePlayer)
{
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	int iDeclareWarTradeRand = GC.getLeaderHeadInfo(getPersonalityType()).getDeclareWarTradeRand();
	int iMinAtWarCounter = MAX_INT;
	for (int iJ = 0; iJ < MAX_TEAMS; iJ++)
	{
		if (GET_TEAM((TeamTypes)iJ).isAlive())
		{
			if (atWar(((TeamTypes)iJ), getTeam()))
			{
				int iAtWarCounter = GET_TEAM(getTeam()).AI_getAtWarCounter((TeamTypes)iJ);
				if (GET_TEAM(getTeam()).AI_getWarPlan((TeamTypes)iJ) == WARPLAN_DOGPILE)
				{
					iAtWarCounter *= 2;
					iAtWarCounter += 5;
				}
				iMinAtWarCounter = std::min(iAtWarCounter, iMinAtWarCounter);
			}
		}
	}

	if (iMinAtWarCounter < 10)
	{
		iDeclareWarTradeRand *= iMinAtWarCounter;
		iDeclareWarTradeRand /= 10;
		iDeclareWarTradeRand ++;
	}

	if (iMinAtWarCounter < 4)
	{
		iDeclareWarTradeRand /= 4;
		iDeclareWarTradeRand ++;
	}

	if (GC.getGameINLINE().getSorenRandNum(iDeclareWarTradeRand, "AI Diplo Declare War Trade") != 0)
	{
		return false;
	}

	int iGoldValuePercent = AI_goldTradeValuePercent();

	int iBestValue = 0;
	TeamTypes eBestTeam = NO_TEAM;
	for (int iJ = 0; iJ < MAX_TEAMS; iJ++)
	{
		if (GET_TEAM((TeamTypes)iJ).isAlive())
		{
			if (atWar(((TeamTypes) iJ), getTeam()) && !atWar(((TeamTypes) iJ), kPlayer.getTeam()))
			{
				if (GET_TEAM((TeamTypes)iJ).getAtWarCount() < std::max(2, (GC.getGameINLINE().countCivTeamsAlive() / 2)))
				{
					TradeData item;
					setTradeItem(&item, TRADE_WAR, iJ, NULL);

					if (kPlayer.canTradeItem(getID(), item, true))
					{
						int iValue = (1 + GC.getGameINLINE().getSorenRandNum(1000, "AI Declare War Trading"));

						iValue *= (101 + GET_TEAM((TeamTypes)iJ).AI_getAttitudeWeight(getTeam()));
						iValue /= 100;

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							eBestTeam = ((TeamTypes)iJ);
						}
					}
				}
			}
		}
	}

	if (eBestTeam == NO_TEAM)
	{
		return false;
	}

	iBestValue = 0;
	int iOurValue = GET_TEAM(getTeam()).AI_declareWarTradeVal(eBestTeam, kPlayer.getTeam());
	int iTheirValue = 0;
	int iReceiveGold = 0;
	int iGiveGold = 0;

	if (iTheirValue > iOurValue)
	{
		int iGold = std::min(((iTheirValue - iOurValue) * 100) / iGoldValuePercent, kPlayer.AI_maxGoldTrade(getID()));

		if (iGold > 0)
		{
			TradeData item;
			setTradeItem(&item, TRADE_GOLD, iGold, NULL);

			if (kPlayer.canTradeItem(getID(), item, true))
			{
				iReceiveGold = iGold;
				iOurValue += (iGold * iGoldValuePercent) / 100;
			}
		}
	}
	else if (iOurValue > iTheirValue)
	{
		int iGold = std::min(((iOurValue - iTheirValue) * 100) / iGoldValuePercent, AI_maxGoldTrade(ePlayer));

		if (iGold > 0)
		{
			TradeData item;
			setTradeItem(&item, TRADE_GOLD, iGold, NULL);

			if (canTradeItem((ePlayer), item, true))
			{
				iGiveGold = iGold;
				iTheirValue += (iGold * iGoldValuePercent) / 100;
			}
		}
	}

	if (iTheirValue <= (iOurValue * 3 / 4))
	{
		return false;
	}

	CLinkList<TradeData> theirList;
	CLinkList<TradeData> ourList;

	TradeData item;
	setTradeItem(&item, TRADE_WAR, eBestTeam, NULL);
	theirList.insertAtEnd(item);

	if (iGiveGold != 0)
	{
		setTradeItem(&item, TRADE_GOLD, iGiveGold, NULL);
		ourList.insertAtEnd(item);
	}

	if (iReceiveGold != 0)
	{
		setTradeItem(&item, TRADE_GOLD, iReceiveGold, NULL);
		theirList.insertAtEnd(item);
	}

	GC.getGameINLINE().implementDeal(getID(), (ePlayer), &ourList, &theirList);

	return true;
}


//Convert units from city population to map units (such as pioneers)
void CvPlayerAI::AI_doProfessions()
{

	std::map<int, bool> cityDanger;

	{
		int iLoop;
		CvCity* pLoopCity;
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			cityDanger[pLoopCity->getID()] = AI_getPlotDanger(pLoopCity->plot(), 5, true);
		}
	}

	for (int iI = 0; iI < NUM_UNITAI_TYPES; ++iI)
	{
		UnitAITypes eUnitAI = (UnitAITypes)iI;

		int iPriority = 0;

		if ((AI_unitAIValueMultipler(eUnitAI) > 0) && eUnitAI != UNITAI_DEFENSIVE)
		{
			ProfessionTypes eProfession = AI_idealProfessionForUnitAIType(eUnitAI);

			if ((eProfession != NO_PROFESSION) && (eUnitAI == UNITAI_SETTLER || (eProfession != (ProfessionTypes) GC.getCivilizationInfo(getCivilizationType()).getDefaultProfession())))
			{
				CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);

				bool bDone = false;

				int iLoop;
				CvCity* pLoopCity;

				for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
				{
					if (pLoopCity->getPopulation() > ((pLoopCity->getHighestPopulation() * 2) / 3))
					{
						if (!cityDanger[pLoopCity->getID()] || AI_unitAIIsCombat(eUnitAI))
						{
							for (int i = 0; i < pLoopCity->getPopulation(); ++i)
							{
								CvUnit* pUnit = pLoopCity->getPopulationUnitByIndex(i);
								if (pUnit != NULL)
								{
									// R&R, ray, code changes for Ideal Profession - START
									// ProfessionTypes eIdealProfession = AI_idealProfessionForUnit(pUnit->getUnitType());
									UnitClassTypes eUnitClassType = (UnitClassTypes)pUnit->getUnitClassType();
								
									// we need 3 possible ideal professions
									ProfessionTypes eIdealProfession = NO_PROFESSION;
									ProfessionTypes eSecondIdealProfession = NO_PROFESSION;
									ProfessionTypes eThirdIdealProfession = NO_PROFESSION;

									for (int iI = 0; iI < GC.getNumProfessionInfos(); ++iI)
									{

										ProfessionTypes eLoopProfession = (ProfessionTypes)iI;
										CvProfessionInfo& kProfession = GC.getProfessionInfo(eLoopProfession);

										if (kProfession.isCitizen())
										{
											if(eIdealProfession == NO_PROFESSION)
											{
												if ((UnitClassTypes)kProfession.LbD_getExpert() == eUnitClassType)
												{
													eIdealProfession = eLoopProfession;
												}
											}
											else if(eSecondIdealProfession == NO_PROFESSION)
											{
												if ((UnitClassTypes)kProfession.LbD_getExpert() == eUnitClassType)
												{
													eSecondIdealProfession = eLoopProfession;
												}
											}
											else
											{
												if ((UnitClassTypes)kProfession.LbD_getExpert() == eUnitClassType)
												{
													eThirdIdealProfession = eLoopProfession;
													break;
												}
											}
										}
									}
									
									if (eIdealProfession == NO_PROFESSION || pUnit->getProfession() != eIdealProfession)
									{
										if (eSecondIdealProfession == NO_PROFESSION || pUnit->getProfession() != eSecondIdealProfession)
										{
											if (eThirdIdealProfession == NO_PROFESSION || pUnit->getProfession() != eThirdIdealProfession)
											{
												if (pUnit->canHaveProfession(eProfession, false) && (AI_professionSuitability(pUnit, eProfession, pLoopCity->plot()) > 100))
												{
													pLoopCity->removePopulationUnit(pUnit, false, eProfession);
													pUnit->AI_setUnitAIType(eUnitAI);
													bDone = true;
													break;
												}
											}
										}
									}
									// R&R, ray, code changes for Ideal Profession - END
								}
							}
						}
					}
				}

				if (!bDone)
				{
					for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
					{
						int iBestValue = 0;
						CvUnit* pBestUnit = NULL;

						if (!cityDanger[pLoopCity->getID()] || AI_unitAIIsCombat(eUnitAI))
						{
							if (pLoopCity->getPopulation() > ((pLoopCity->getHighestPopulation() * 2) / 3))
							{
								for (int i = 0; i < pLoopCity->getPopulation(); ++i)
								{
									CvUnit* pUnit = pLoopCity->getPopulationUnitByIndex(i);
									if (pUnit != NULL)
									{
										if (pUnit->canHaveProfession(eProfession, false))
										{
											int iValue = AI_professionSuitability(pUnit, eProfession, pLoopCity->plot());

											if (pUnit->getProfession() == NO_PROFESSION)
											{

											}
											else if (pUnit->getProfession() == pUnit->AI_getIdealProfession())
											{
												iValue /= 4;
											}

											if (eUnitAI == UNITAI_SETTLER)
											{
												if (pUnit->AI_getIdealProfession() != NO_PROFESSION)
												{
													if ((pUnit->getProfession() != pUnit->AI_getIdealProfession()) && (GC.getProfessionInfo(pUnit->AI_getIdealProfession()).isWorkPlot()))
													{
														iValue *= 150;
														iValue /= 100;
													}
												}
												else
												{
													iValue *= 120;
													iValue /= 100;
												}
											}

											iValue *= 100;
											iValue += GC.getGameINLINE().getSorenRandNum(100, "AI pick unit");

											if (iValue > iBestValue)
											{
												iBestValue = iValue;
												pBestUnit = pUnit;
											}
										}
									}
								}
							}
						}
						if (pBestUnit != NULL)
						{
							pLoopCity->removePopulationUnit(pBestUnit, false, eProfession);
							pBestUnit->AI_setUnitAIType(eUnitAI);
						}
					}
				}
			}
		}
	}

	//Military
	int iLoop;
	CvCity* pLoopCity;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->AI_isDanger())
		{
			ProfessionTypes eProfession = AI_idealProfessionForUnitAIType(UNITAI_DEFENSIVE, pLoopCity);

			if ((eProfession != NO_PROFESSION) && (eProfession != (ProfessionTypes) GC.getCivilizationInfo(getCivilizationType()).getDefaultProfession()))
			{
				CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);
				bool bDone = false;

				int iNeededDefenders = pLoopCity->AI_neededDefenders();
				int iHaveDefenders = pLoopCity->AI_numDefenders(true, false);

				if (iHaveDefenders < iNeededDefenders)
				{
					while (!bDone)
					{
						int iBestValue = 0;
						CvUnit* pBestUnit = NULL;

						for (int i = 0; i < pLoopCity->getPopulation(); ++i)
						{
							CvUnit* pUnit = pLoopCity->getPopulationUnitByIndex(i);
							if (pUnit != NULL)
							{
								if (pUnit->canHaveProfession(eProfession, false))
								{
									int iValue = AI_professionSuitability(pUnit, eProfession, pLoopCity->plot());

									if (pUnit->getProfession() == NO_PROFESSION)
									{

									}
									else if (pUnit->getProfession() == pUnit->AI_getIdealProfession())
									{
										iValue /= 4;
									}

									iValue *= 100;
									iValue += GC.getGameINLINE().getSorenRandNum(100, "AI pick unit");

									if (iValue > iBestValue)
									{
										iBestValue = iValue;
										pBestUnit = pUnit;
									}
								}
							}
						}

						if (pBestUnit != NULL)
						{
							pLoopCity->removePopulationUnit(pBestUnit, false, eProfession);
							pBestUnit->AI_setUnitAIType(UNITAI_DEFENSIVE);
						}
						else
						{
							bDone = true;
						}
					}
				}
			}
		}
	}
}

void CvPlayerAI::AI_doEurope()
{
	//Buy Units.
	UnitTypes eBuyUnit;
	UnitAITypes eBuyUnitAI;
	int iBuyUnitValue;

	if (!canTradeWithEurope())
	{
		return;
	}

	if (!isHuman() && !isNative() && !isEurope())
	{
		//Always refresh at start of new turn (maybe do this smarter but it's okay for now)
		AI_updateNextBuyUnit();
		AI_updateNextBuyProfession();
	}

	// TAC - AI More Immigrants - koma13 - START
	if (GC.getGameINLINE().getGameTurn() < GC.getGameINLINE().AI_adjustedTurn(20))
	{
		AI_hurryDocks();
	}
	// TAC - AI More Immigrants - koma13 - END

	eBuyUnit = AI_nextBuyUnit(&eBuyUnitAI, &iBuyUnitValue);

	UnitTypes eBuyProfessionUnit;
	ProfessionTypes eBuyProfession;
	UnitAITypes eBuyProfessionAI;
	int iBuyProfessionValue;

	eBuyProfessionUnit = AI_nextBuyProfessionUnit(&eBuyProfession, &eBuyProfessionAI, &iBuyProfessionValue);

	int iUnitPrice = 0;

	if (eBuyUnit != NO_UNIT)
	{
		iUnitPrice = getEuropeUnitBuyPrice(eBuyUnit);
	}

	if ((eBuyUnit != NO_UNIT) && ((iBuyUnitValue > iBuyProfessionValue) || (iUnitPrice < getGold())))
	{
		if (getGold() > iUnitPrice)
		{
			CvUnit* pUnit = buyEuropeUnit(eBuyUnit, 100);

			FAssert(pUnit != NULL);
			pUnit->AI_setUnitAIType(eBuyUnitAI);
			// TAC - AI Military Buildup - koma13 - START
			if (AI_isStrategy(STRATEGY_MILITARY_BUILDUP))
			{
				AI_clearStrategy(STRATEGY_MILITARY_BUILDUP);
			}
			// TAC - AI Military Buildup - koma13 - END

			AI_updateNextBuyUnit();
		}
	}

	// TAC - AI Military Buildup - koma13 - START
	//if ((eBuyProfession != NO_PROFESSION) && (iBuyProfessionValue > iBuyUnitValue))
	if ((eBuyProfession != NO_PROFESSION) && (iBuyProfessionValue > iBuyUnitValue) && !AI_isStrategy(STRATEGY_MILITARY_BUILDUP))
	// TAC - AI Military Buildup - koma13 - END
	{
		ProfessionTypes eDefaultProfession = (ProfessionTypes) GC.getCivilizationInfo(getCivilizationType()).getDefaultProfession();

		int iBestValue = 0;
		CvUnit* pBestUnit = NULL;

		int iBuyPrice = -1;
		if (eBuyProfessionUnit != NO_UNIT)
		{
			iBuyPrice = getEuropeUnitBuyPrice(eBuyProfessionUnit);
		}

		CvProfessionInfo& kProfession = GC.getProfessionInfo(eBuyProfession);

		if (!kProfession.isCitizen() && (eBuyProfessionAI != UNITAI_COLONIST))
		{
			//Consider upgrading an existing unit.
			for (int i = 0; i < getNumEuropeUnits(); ++i)
			{
				CvUnit* pLoopUnit = getEuropeUnit(i);
				
				// TAC - AI purchases military units - koma13 - START
				//if (!pLoopUnit->AI_hasAIChanged(4))
				if (!pLoopUnit->AI_hasAIChanged(4) && (pLoopUnit->getUnitInfo().getTeacherWeight() <= 0) && !AI_unitAIIsCombat(eBuyProfessionAI))
				// TAC - AI purchases military units - koma13 - END
				{
					if (pLoopUnit->getProfession() == eBuyProfession)
					{
						int iValue = 200;
						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							pBestUnit = pLoopUnit;
						}
					}
					else
					{
						if (pLoopUnit->getProfession() == eDefaultProfession)
						{
							if (pLoopUnit->canHaveProfession(eBuyProfession, false))
							{
								int iValue = AI_professionSuitability(pLoopUnit, eBuyProfession, NULL);

								bool bValid = true;

								if (eBuyProfessionAI == UNITAI_SCOUT)
								{
									if (iValue < 100)
									{
										bValid = false;
									}
								}
								if (bValid)
								{
									iValue *= 100 + ((iBuyProfessionValue - 100) / 5);
									iValue /= 100;

									int iMinThreshold = 1;

									if (iValue >= iMinThreshold)
									{
										iValue *= 2;

										//XXX Perform Gold Cost of Upgrade Modification.
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
				}
			}
		}

		if (pBestUnit != NULL)
		{
			changeProfessionEurope(pBestUnit->getID(), eBuyProfession);
			FAssert(pBestUnit->getProfession() == eBuyProfession);
			pBestUnit->AI_setUnitAIType(eBuyProfessionAI);
		}
		else if (eBuyProfessionUnit != NO_UNIT)
		{
			FAssert(iBuyPrice >= 0);
			if (getGold() > iBuyPrice)
			{
				CvUnit* pUnit = buyEuropeUnit(eBuyProfessionUnit, 100);
				pUnit->AI_setUnitAIType(eBuyProfessionAI);
			}
		}
	}

	//arm any europe units that need it
	// TAC - AI purchases military units - koma13 - START
	/*
	for (int i = 0; i < getNumEuropeUnits(); i++)
	{
		CvUnit *pUnit = getEuropeUnit(i);

		int iUndefended = 0;
		int iNeeded = AI_totalDefendersNeeded(&iUndefended);
		if (iNeeded > 0 || AI_isStrategy(STRATEGY_REVOLUTION_PREPARING))
		{
			ProfessionTypes eBestProfession = NO_PROFESSION;
			if (GC.getGameINLINE().getSorenRandNum(100, "") < 50)
			{
				eBestProfession = GET_PLAYER(pUnit->getOwnerINLINE()).AI_idealProfessionForUnitAIType(UNITAI_DEFENSIVE);
			} 
			else 
			{
				eBestProfession = GET_PLAYER(pUnit->getOwnerINLINE()).AI_idealProfessionForUnitAIType(UNITAI_COUNTER);
			}

			if (eBestProfession != NO_PROFESSION && pUnit->canHaveProfession(eBestProfession, false))
			{
				changeProfessionEurope(pUnit->getID(), eBestProfession);
			}
		}
	}
	*/
	if (isEurope() || isNative())
	{
		return;
	}

	AI_updateNextBuyUnit(false);
	
	for (int i = 0; i < getNumEuropeUnits(); i++)
	{
		CvUnit *pUnit = getEuropeUnit(i);
		CvUnitInfo& kUnit = pUnit->getUnitInfo();

		if (kUnit.getTeacherWeight() <= 0 || AI_isStrategy(STRATEGY_REVOLUTION_PREPARING))
		{
			bool bProfessionChange = false;
			eBuyUnit = AI_nextBuyUnit(&eBuyUnitAI, &iBuyUnitValue);
			if (eBuyUnit != NO_UNIT)
			{
				ProfessionTypes eProfession = GET_PLAYER(pUnit->getOwnerINLINE()).AI_idealProfessionForUnitAIType(eBuyUnitAI);
				if (eProfession != NO_PROFESSION && pUnit->canHaveProfession(eProfession, false))
				{
					int iInitialPrice = getEuropeUnitBuyPrice(eBuyUnit, false);
					int iMaxPrice = (iInitialPrice * GC.getDefineINT("AI_EUROPE_PRICE_LIMIT_PERCENT")) / 100;
					int iCurrentPrice = getEuropeUnitBuyPrice(eBuyUnit);

					if ((pUnit->getEuropeProfessionChangeCost(eProfession) <= iCurrentPrice) || (iCurrentPrice >= iMaxPrice))
					{
						bProfessionChange = true;
						changeProfessionEurope(pUnit->getID(), eProfession);
						
						// TAC - AI Military Buildup - koma13 - START
						if (AI_isStrategy(STRATEGY_MILITARY_BUILDUP))
						{
							AI_clearStrategy(STRATEGY_MILITARY_BUILDUP);
						}
						// TAC - AI Military Buildup - koma13 - END
						AI_updateNextBuyUnit(false);
					}
				}
			}

			// TAC - AI Military Buildup - koma13 - START
			//if (!bProfessionChange)
			if (!bProfessionChange && (kUnit.getTeacherWeight() <= 0) && !AI_isStrategy(STRATEGY_MILITARY_BUILDUP))
			// TAC - AI Military Buildup - koma13 - END
			{
				if (AI_neededWorkers(NULL) > 0)
				{
					ProfessionTypes eProfession = GET_PLAYER(pUnit->getOwnerINLINE()).AI_idealProfessionForUnitAIType(UNITAI_WORKER);
					if (eProfession != NO_PROFESSION && pUnit->canHaveProfession(eProfession, false))
					{
						changeProfessionEurope(pUnit->getID(), eProfession);
					}
				}
				else
				{
					if (GC.getGameINLINE().getSorenRandNum(100, "AI buy counter unit") < 25)
					{
						if (AI_totalUnitAIs(UNITAI_COUNTER) < getNumCities())
						{
							ProfessionTypes eProfession = GET_PLAYER(pUnit->getOwnerINLINE()).AI_idealProfessionForUnitAIType(UNITAI_COUNTER);
							if (eProfession != NO_PROFESSION && pUnit->canHaveProfession(eProfession, false))
							{
								changeProfessionEurope(pUnit->getID(), eProfession);
							}
						}
					}
				}
			}
		}
	}
	// TAC - AI purchases military units - koma13 - END
}

void CvPlayerAI::AI_nativeYieldGift(CvUnit* pUnit)
{
	FAssert(pUnit != NULL);
	FAssert(pUnit->isOnMap());
	FAssert(pUnit->plot()->isCity());
	FAssert(isNative());

	CvCity* pHomeCity = pUnit->getHomeCity();
	if (pHomeCity == NULL)
	{
		pHomeCity = GC.getMapINLINE().findCity(pUnit->getX_INLINE(), pUnit->getY_INLINE(), pUnit->getOwnerINLINE());
		pUnit->setHomeCity(pHomeCity);
	}

	if (pHomeCity == NULL)
	{
		return;
	}

	YieldTypes eBestYield = NO_YIELD;
	int iBestValue = 0;

	for(int i=0;i<NUM_YIELD_TYPES;i++)
	{
		if (pHomeCity->canProduceYield((YieldTypes)i))
		{
			if ((YIELD_FOOD != i) && (YIELD_HORSES != i))
			{
				YieldTypes eYield = (YieldTypes) i;
				if (GC.getYieldInfo(eYield).getNativeSellPrice() > 0)
				{
					int iRandValue = 100 + GC.getGameINLINE().getSorenRandNum(900, "Native Yield Gift: pick yield");
					iRandValue *= pHomeCity->getYieldStored(eYield);
					if (iRandValue > iBestValue)
					{
						eBestYield = eYield;
						iBestValue = iRandValue;
					}
				}
			}
		}
	}

	if (eBestYield == NO_YIELD)
	{
		return;
	}

	CvCity* pOtherCity = pUnit->plot()->getPlotCity();
	FAssert(pOtherCity != NULL);
	FAssert(!pOtherCity->isNative());

	if(pOtherCity != NULL)
	{
		//give some yields from pBestCity to pOtherCity
		int iYieldPercent = 5 * (AI_getAttitudeVal(pOtherCity->getOwnerINLINE(), false) + GC.getGameINLINE().getSorenRandNum(10, "Native Yield Gift: pick amount"));
		int iYieldAmount = iYieldPercent * pHomeCity->getYieldStored(eBestYield) / 100;
		iYieldAmount = std::min(iYieldAmount, pHomeCity->getYieldStored(eBestYield));
		iYieldAmount = std::max(iYieldAmount, 0);
		if(iYieldAmount > 0)
		{
			pHomeCity->changeYieldStored(eBestYield, -iYieldAmount);
			pOtherCity->changeYieldStored(eBestYield, iYieldAmount);

			//AI_changeContactTimer(pOtherCity->getOwnerINLINE(), CONTACT_YIELD_GIFT, GC.getLeaderHeadInfo(getPersonalityType()).getContactDelay(CONTACT_YIELD_GIFT));

			//popup dialog
			CvPlayer& kOtherPlayer = GET_PLAYER(pOtherCity->getOwnerINLINE());
			if(kOtherPlayer.isHuman())
			{
				CvDiploParameters* pDiplo = new CvDiploParameters(getID());
				pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_NATIVES_YIELD_GIFT"));
				pDiplo->addDiploCommentVariable(iYieldAmount);
				pDiplo->addDiploCommentVariable(GC.getYieldInfo(eBestYield).getChar());
				pDiplo->addDiploCommentVariable(pOtherCity->getNameKey());
				pDiplo->setAIContact(true);
				gDLL->beginDiplomacy(pDiplo, kOtherPlayer.getID());
			}
		}
	}
}

// R&R, ray, Natives Trading - START
void CvPlayerAI::AI_nativeTrade(CvUnit* pUnit)
{
	FAssert(pUnit != NULL);
	FAssert(pUnit->isOnMap());
	FAssert(pUnit->plot()->isCity());
	FAssert(isNative());

	CvCity* pHomeCity = pUnit->getHomeCity();
	if (pHomeCity == NULL)
	{
		pHomeCity = GC.getMapINLINE().findCity(pUnit->getX_INLINE(), pUnit->getY_INLINE(), pUnit->getOwnerINLINE());
		pUnit->setHomeCity(pHomeCity);
	}

	if (pHomeCity == NULL)
	{
		return;
	}

	YieldTypes eBestYield = NO_YIELD;
	int iBestValue = 0;

	for(int i=0;i<NUM_YIELD_TYPES;i++)
	{
		if (pHomeCity->canProduceYield((YieldTypes)i))
		{
			if ((YIELD_FOOD != i) && (YIELD_HORSES != i))
			{
				YieldTypes eYield = (YieldTypes) i;
				if (GC.getYieldInfo(eYield).getNativeSellPrice() > 0)
				{
					int iRandValue = 100 + GC.getGameINLINE().getSorenRandNum(900, "Native Yield Gift: pick yield");
					iRandValue *= pHomeCity->getYieldStored(eYield);
					if (iRandValue > iBestValue)
					{
						eBestYield = eYield;
						iBestValue = iRandValue;
					}
				}
			}
		}
	}

	if (eBestYield == NO_YIELD)
	{
		return;
	}

	CvCity* pOtherCity = pUnit->plot()->getPlotCity();
	FAssert(pOtherCity != NULL);
	FAssert(!pOtherCity->isNative());

	if(pOtherCity != NULL)
	{
		//give some yields from pBestCity to pOtherCity
		int iYieldPercent = 5 * (AI_getAttitudeVal(pOtherCity->getOwnerINLINE(), false) + GC.getGameINLINE().getSorenRandNum(10, "Native Yield Trade: pick amount"));
		int iYieldAmount = iYieldPercent * pHomeCity->getYieldStored(eBestYield) / 100;
		iYieldAmount = std::min(iYieldAmount, pHomeCity->getYieldStored(eBestYield));
		iYieldAmount = std::max(iYieldAmount, 0);
		if(iYieldAmount > 0)
		{
			CvPlayer& kOtherPlayer = GET_PLAYER(pOtherCity->getOwnerINLINE());
			int priceToPay = GC.getYieldInfo(eBestYield).getNativeSellPrice() * iYieldAmount;

			// simple case for AI
			if(!kOtherPlayer.isHuman())
			{
				if (kOtherPlayer.getGold() >= priceToPay)
				{
					pOtherCity->changeYieldStored(eBestYield, iYieldAmount);
					kOtherPlayer.changeGold(-priceToPay);
					pHomeCity->changeYieldStored(eBestYield, -iYieldAmount);
					GET_PLAYER(pHomeCity->getOwnerINLINE()).changeGold(priceToPay);
				}
			}

			// popup dialog for Human
			else
			{
				CvDiploParameters* pDiplo = new CvDiploParameters(getID());
				if (kOtherPlayer.getGold() >= priceToPay)
				{
					pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_NATIVE_TRADE"));
				}
				else 
				{
					pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_NATIVE_TRADE_CANNOT_AFFORD"));
				}
				pDiplo->addDiploCommentVariable(iYieldAmount);
				pDiplo->addDiploCommentVariable(GC.getYieldInfo(eBestYield).getChar());
				pDiplo->addDiploCommentVariable(pOtherCity->getNameKey());
				pDiplo->addDiploCommentVariable(priceToPay);
				pDiplo->setData(pUnit->getID());
				// caching data in Unit
				pUnit->setYieldForNativeTrade(eBestYield);
				pUnit->setAmountForNativeTrade(iYieldAmount);

				pDiplo->setAIContact(true);
				gDLL->beginDiplomacy(pDiplo, kOtherPlayer.getID());
			}
		}
	}
}
// R&R, ray, Natives Trading - END

bool CvPlayerAI::AI_isYieldForSale(YieldTypes eYield) const
{
	if (!GC.getYieldInfo(eYield).isCargo())
	{
		return false;
	}

	switch (eYield)
	{
		case YIELD_FOOD:
		case YIELD_LUMBER:
		case YIELD_STONE:
			return false;
			break;
		case YIELD_HEMP:
			return true;
			break;
		case YIELD_ORE:
		case YIELD_SHEEP:
		case YIELD_CATTLE:
			return true;
			break;
		case YIELD_HORSES:
			return !AI_isYieldNeeded(eYield); // R&R, ray, AI improvement for horses - START
			break;
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
			return true;
			break;
		case YIELD_TRADE_GOODS:
			return false;
			break;
		case YIELD_ROPE:
		case YIELD_SAILCLOTH:
			return !AI_isYieldNeeded(eYield);
			break;
		// Erik: It does the AI little good to sell these yields
		case YIELD_TOOLS:
		case YIELD_BLADES:
		case YIELD_MUSKETS:
		case YIELD_CANNONS:
			return false;
			break;
		case YIELD_COCA_LEAVES:
		case YIELD_SILVER:
		case YIELD_GOLD:
		case YIELD_GEMS:
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
			return true;
			break;
		case YIELD_LUXURY_GOODS:
			return false;
			break;
		case YIELD_HAMMERS:
		case YIELD_BELLS:
		case YIELD_CROSSES:
		case YIELD_CULTURE:
		case YIELD_HEALTH: // R&R, ray, Health - START
		case YIELD_EDUCATION:
		case YIELD_HAPPINESS: // WTP, ray, Happiness - START
		case YIELD_UNHAPPINESS: // WTP, ray, Happiness - START
			FAssertMsg(false, "Selling intangibles?");
			break;
		default:
			FAssert(false);
	}

	return false;
}
// TAC - AI Economy - koma13 - START
bool CvPlayerAI::AI_isYieldNeeded(YieldTypes eYield, int iCapacityPercent, CvCity* pCity) const
{
	FAssert (eYield != NO_YIELD);
	
	if (eYield == YIELD_FOOD)
	{
		return true;
	}

	// R&R, ray, AI improvement for horses - START
	if (eYield == YIELD_HORSES)
	{
		int iHorsesAveragePerCity = 0;
		int itotalHorsesStored = 0;
		int iNumCities = 0;

		int iThreshold = 50;
		iThreshold *= GC.getEraInfo(GC.getGameINLINE().getStartEra()).getTrainPercent();
		iThreshold /= 100;

		CvCity* xLoopCity;
		int xLoop;

		for (xLoopCity = firstCity(&xLoop); xLoopCity != NULL; xLoopCity = nextCity(&xLoop))
		{
			if (pCity == NULL || pCity == xLoopCity)
			{
				itotalHorsesStored += xLoopCity->getYieldStored(eYield);
				iNumCities +=1;
			}
		}
		if (iNumCities != 0)
		{
			iHorsesAveragePerCity = itotalHorsesStored / iNumCities;
		}

		if (iHorsesAveragePerCity <= iThreshold)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	// R&R, ray, AI improvement for horses - END

	int iTotalNeeded = 0;

	CvCity* pLoopCity;
	int iLoop;
	
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pCity == NULL || pCity == pLoopCity)
		{
			if (iCapacityPercent == -1)
			{
				iTotalNeeded += pLoopCity->AI_getRequiredYieldLevel(eYield) - pLoopCity->getYieldStored(eYield);
			}
			else
			{
				// R&R, ray, fix for new storage capacity
				if (GC.getNEW_CAPACITY())
				{
					iTotalNeeded += ((pLoopCity->getMaxYieldCapacity() / 5 * iCapacityPercent) / 100) - pLoopCity->getYieldStored(eYield);
				}
				else
				{
					iTotalNeeded += ((pLoopCity->getMaxYieldCapacity() * iCapacityPercent) / 100) - pLoopCity->getYieldStored(eYield);
				}
			}
		}
	}

	if (iTotalNeeded > 0)
	{
		return true;
	}
	
	return false;
}
// TAC - AI Economy - koma13 - END

bool CvPlayerAI::AI_isYieldFinalProduct(YieldTypes eYield) const
{
	if (!GC.getYieldInfo(eYield).isCargo())
	{
		return false;
	}

	bool bFinal = true;

	switch (eYield)
	{
		case YIELD_FOOD:
		case YIELD_LUMBER:
		case YIELD_STONE:
			bFinal = false;
			break;
		case YIELD_HEMP:
		case YIELD_ORE:
		case YIELD_SHEEP:
		case YIELD_CATTLE:
			{
				int iLoop;
				CvCity* pLoopCity = NULL;
				for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
				{
					if (pLoopCity->AI_getNeededYield(eYield) > 0)
					{
						bFinal = false;
						break;
					}
				}
			}
			break;
		case YIELD_HORSES:
			bFinal = false;
			break;
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
				int iLoop;
				CvCity* pLoopCity = NULL;
				for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
				{
					if (pLoopCity->AI_getNeededYield(eYield) > 0)
					{
						bFinal = false;
						break;
					}
				}
			}
			break;
		case YIELD_TRADE_GOODS:
			bFinal = true;
			break;
		case YIELD_ROPE:
		case YIELD_SAILCLOTH:
		case YIELD_TOOLS:
		case YIELD_BLADES:
		case YIELD_MUSKETS:
		case YIELD_CANNONS:
			bFinal = false;
			break;
		case YIELD_COCA_LEAVES:
		case YIELD_SILVER:
		case YIELD_GOLD:
		case YIELD_GEMS:
		case YIELD_COCOA:
		case YIELD_COFFEE:
		case YIELD_CIGARS:
		case YIELD_WOOL_CLOTH:
			bFinal = true;
			break;
		case YIELD_CLOTH:
			{
				int iLoop;
				CvCity* pLoopCity = NULL;
				for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
				{
					if (pLoopCity->AI_getNeededYield(eYield) > 0)
					{
						bFinal = false;
						break;
					}
				}
			}
			break;
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
		case YIELD_LUXURY_GOODS:
			bFinal = true;
			break;
		case YIELD_HAMMERS:
		case YIELD_BELLS:
		case YIELD_CROSSES:
		case YIELD_CULTURE:
		case YIELD_HEALTH: // R&R, ray, Health - START
		case YIELD_EDUCATION:
		case YIELD_HAPPINESS: // WTP, ray, Happiness - START
		case YIELD_UNHAPPINESS: // WTP, ray, Happiness - START
			bFinal = false;
			FAssertMsg(false, "Selling intangibles?");
			break;
		default:
			FAssert(false);
	}

	return bFinal;
}

bool CvPlayerAI::AI_shouldBuyFromEurope(YieldTypes eYield) const
{
	if (!GC.getYieldInfo(eYield).isCargo())
	{
		return false;
	}

	bool bBuy = false;

	switch (eYield)
	{
		case YIELD_FOOD:
		case YIELD_LUMBER:
		case YIELD_STONE:
		case YIELD_HEMP:
		case YIELD_ORE:
		case YIELD_SHEEP:
		case YIELD_CATTLE:
		case YIELD_HORSES:
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
		case YIELD_TRADE_GOODS:
		case YIELD_ROPE:
		case YIELD_SAILCLOTH:
			bBuy = false;
			break;
		case YIELD_TOOLS:
			bBuy = AI_isYieldNeeded(eYield);
			break;
		case YIELD_BLADES:
		case YIELD_MUSKETS:
		case YIELD_CANNONS:
		case YIELD_SILVER:
		case YIELD_GOLD:
		case YIELD_GEMS:
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
		case YIELD_LUXURY_GOODS:
			bBuy = false;
			break;
		case YIELD_HAMMERS:
		case YIELD_BELLS:
		case YIELD_CROSSES:
		case YIELD_CULTURE:
		case YIELD_HEALTH: // R&R, ray, Health - START
		case YIELD_EDUCATION:
		case YIELD_HAPPINESS: // WTP, ray, Happiness - START
		case YIELD_UNHAPPINESS: // WTP, ray, Happiness - START
			bBuy = false;
			FAssertMsg(false, "Selling intangibles?");
			break;
		default:
			FAssert(false);
	}

	return bBuy;
}

// TAC - AI More food - koma13 - START
//int CvPlayerAI::AI_yieldValue(YieldTypes eYield, bool bProduce, int iAmount)
int CvPlayerAI::AI_yieldValue(YieldTypes eYield, bool bProduce, int iAmount, bool bFood)
// TAC - AI More food - koma13 - END
{
	int iValue = 0;
	if (bProduce)
	{
		iValue += 100 * (isNative() ? GC.getYieldInfo(eYield).getNativeBaseValue() : GC.getYieldInfo(eYield).getAIBaseValue());
	}
	if (eYield == YIELD_FOOD)
	{
		// TAC - AI More food - koma13 - START

		if (bFood)
		{
			iValue += 75 * (isNative() ? GC.getYieldInfo(eYield).getNativeBaseValue() : GC.getYieldInfo(eYield).getAIBaseValue());
		}
		// TAC - AI More food - koma13 - END
	}

	else if (isNative())
	{
		CvYieldInfo& kYieldInfo = GC.getYieldInfo(eYield);
		int iPrice = 0;
		int iValidPrices = 0;
		if (kYieldInfo.getNativeBuyPrice() > 0)
		{
			iPrice += kYieldInfo.getNativeBuyPrice();
			iValidPrices++;
		}
		if (kYieldInfo.getNativeSellPrice() > 0)
		{
			iPrice += kYieldInfo.getNativeSellPrice();
			iValidPrices++;
		}

		if (iPrice > 0)
		{
			//If both buy and sell, use average. Otherwise, use 2/3rd the value.
			iPrice *= 2;
			iPrice /= 2 + iValidPrices;

			iValue += iPrice * 100;
		}
	}
	else
	{
		iValue += m_aiYieldValuesTimes100[eYield];
	}

	iValue *= AI_yieldWeight(eYield);
	iValue /= 100;

	if (bProduce)
	{
		int iWeaponsMultiplier = 100;
		if (AI_isStrategy(STRATEGY_REVOLUTION_PREPARING))
		{
			iWeaponsMultiplier += 50;
		}

		int iGoodsMultiplier = 100;
		if (AI_isStrategy(STRATEGY_REVOLUTION_DECLARING))
		{
			iGoodsMultiplier -= 15;
			iWeaponsMultiplier += 25;
		}
		else if (AI_isStrategy(STRATEGY_CASH_FOCUS))
		{
			iGoodsMultiplier += 50;
		}

		if (AI_isStrategy(STRATEGY_REVOLUTION))
		{
			iGoodsMultiplier -= 15;
			iWeaponsMultiplier += 50;
		}

		switch (eYield)
		{
			case YIELD_FOOD:
				break;
			case YIELD_LUMBER:
			case YIELD_STONE:
				iValue *= 100;
				iValue /= iGoodsMultiplier;
				break;
			case YIELD_HEMP:
			case YIELD_ORE:
			case YIELD_SHEEP:
			case YIELD_CATTLE:
				iValue *= iGoodsMultiplier;
				iValue /= 100;
				break;
			case YIELD_HORSES:
				iValue *= iWeaponsMultiplier;
				iValue /= 100;
				break;
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
				iValue *= iGoodsMultiplier;
				iValue /= 100;
				break;
			case YIELD_TRADE_GOODS:
				break;
			case YIELD_ROPE:
			case YIELD_SAILCLOTH:
				iValue *= iWeaponsMultiplier;
				iValue /= 100;
				break;
			case YIELD_TOOLS:
				break;
			case YIELD_BLADES:
			case YIELD_MUSKETS:
			case YIELD_CANNONS:
				iValue *= iWeaponsMultiplier;
				iValue /= 100;
				break;
			case YIELD_COCA_LEAVES:
			case YIELD_SILVER:
			case YIELD_GOLD:
			case YIELD_GEMS:
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
				iValue *= iGoodsMultiplier;
				iValue /= 100;
				break;
			case YIELD_LUXURY_GOODS:
				break;
			case YIELD_HAMMERS:
				if (AI_isStrategy(STRATEGY_REVOLUTION_PREPARING))
				{
					iValue *= 125;
					iValue /= 100;
					if (AI_isStrategy(STRATEGY_REVOLUTION_DECLARING))
					{
						iValue *= 125;
						iValue /= 100;
					}
				}
				// R&R, ray, AI builds stronger - START
				else
				{
					iValue *= 150;
					iValue /= 100;
				}
				// R&R, ray, AI builds stronger - END
				break;
			case YIELD_BELLS:
				if (AI_isStrategy(STRATEGY_REVOLUTION_PREPARING))
				{
					iValue *= 150;
					iValue /= 100;
					if (AI_isStrategy(STRATEGY_REVOLUTION_DECLARING))
					{
						iValue *= 150;
						iValue /= 100;
					}
				}
				break;
			case YIELD_CROSSES:
			case YIELD_CULTURE:
			case YIELD_HEALTH: // R&R, ray, Health - START
			case YIELD_EDUCATION:
			case YIELD_HAPPINESS: // WTP, ray, Happiness - START
				iValue *= 100;
				iValue /= iWeaponsMultiplier;
				break;
			case YIELD_UNHAPPINESS: // WTP, ray, Happiness - START
				iValue = 0; // this Yield is just bad, it is never supposed to be created purolsely
			default:
			break;
		}
	}

	iValue *= iAmount;
	iValue /= 100;

	return iValue;
}

void CvPlayerAI::AI_updateYieldValues()
{
	if (isNative())
	{
		return;
	}

	PlayerTypes eParent = getParent();

	if (getParent() == NO_PLAYER)
	{
		eParent = getID();
	}

	CvPlayer& kParent = GET_PLAYER(eParent);
	for (int i = 0; i < NUM_YIELD_TYPES; ++i)
	{
		YieldTypes eYield = (YieldTypes)i;
		int iValue = 0;
		switch (eYield)
		{
			case YIELD_FOOD:
				iValue += (kParent.getYieldSellPrice(eYield) + kParent.getYieldBuyPrice(eYield)) / 2;
				break;
			case YIELD_LUMBER:
			case YIELD_STONE:
				// TAC - AI Economy - koma13 - START
				//iValue += (kParent.getYieldSellPrice(eYield) + kParent.getYieldBuyPrice(eYield)) / 2;
				iValue += (AI_isYieldNeeded(eYield, 50)) ? (kParent.getYieldSellPrice(eYield) + kParent.getYieldBuyPrice(eYield)) / 2 : kParent.getYieldBuyPrice(eYield);
				// TAC - AI Economy - koma13 - END
				break;
			case YIELD_HEMP:
			case YIELD_ORE:
			case YIELD_SHEEP:
			case YIELD_CATTLE:
				iValue += kParent.getYieldBuyPrice(eYield);
				break;
			case YIELD_HORSES:
				iValue += kParent.getYieldSellPrice(eYield);
				break;
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
				iValue += kParent.getYieldBuyPrice(eYield);
				break;
			case YIELD_TRADE_GOODS:
			case YIELD_ROPE:
			case YIELD_SAILCLOTH:
			case YIELD_TOOLS:
			case YIELD_BLADES:
			case YIELD_MUSKETS:
			case YIELD_CANNONS:
				iValue += kParent.getYieldSellPrice(eYield);
				break;
			case YIELD_COCA_LEAVES:
			case YIELD_SILVER:
			case YIELD_GOLD:
			case YIELD_GEMS:
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
				iValue += kParent.getYieldBuyPrice(eYield);
				break;
			case YIELD_LUXURY_GOODS:
				iValue += kParent.getYieldSellPrice(eYield);
				break;
			case YIELD_HAMMERS:
			case YIELD_BELLS:
			case YIELD_CROSSES:
			case YIELD_CULTURE:
			case YIELD_HEALTH:
			case YIELD_EDUCATION:
			case YIELD_HAPPINESS: // WTP, ray, Happiness - START
			case YIELD_UNHAPPINESS: // WTP, ray, Happiness - START
				break;
			default:
				FAssert(false);
		}

		m_aiYieldValuesTimes100[i] = 100 * iValue;
	}
	int iCrossValue = m_aiYieldValuesTimes100[YIELD_FOOD] * getGrowthThreshold(1) / (50 + immigrationThreshold() * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent() / 100);
	iCrossValue /= 2;

	//Crosses
	m_aiYieldValuesTimes100[YIELD_CROSSES] = std::max(m_aiYieldValuesTimes100[YIELD_CROSSES], iCrossValue);

	//The function is quite simple. Iterate over every citizen which has input yield.
	//Calculate the value of their output yield, and assign half of that to the input.
	if (!isHuman())
	{
		int iLoop;
		CvCity* pLoopCity;
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			for (int i = 0; i < pLoopCity->getPopulation(); ++i)
			{
				CvUnit* pLoopUnit = pLoopCity->getPopulationUnitByIndex(i);
				
				ProfessionTypes eProfession = pLoopUnit->getProfession();
				if (eProfession != NO_PROFESSION)
				{
					CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);
					if (kProfession.getYieldsConsumed(0) != NO_YIELD)
					{
						int iValue = 0;
						// R&R, ray , MYCP partially based on code of Aymerick - START
						FAssert(kProfession.getYieldsProduced(0) != NO_YIELD);//damn welfware cheats.
						int iInput = pLoopCity->getProfessionInput(eProfession, pLoopUnit);
						int iOutput = pLoopCity->getProfessionOutput(eProfession, pLoopUnit);

						int iProfit = (m_aiYieldValuesTimes100[kProfession.getYieldsProduced(0)] * iOutput);
						// R&R, ray , MYCP partially based on code of Aymerick - END
						// R&R, ray, fix on Zero Division
						if (iInput == 0)
						{
							iInput = 1;
						}
						int iInputValue = iProfit / (2 * iInput); //Assign 50% of the yield value to the input.
						m_aiYieldValuesTimes100[kProfession.getYieldsConsumed(0)] = std::max(iInputValue, m_aiYieldValuesTimes100[kProfession.getYieldsConsumed(0)]);
					}
				}


				// R&R, ray, code changes for Ideal Profession - START
				// ProfessionTypes eIdealProfesion = AI_idealProfessionForUnit(pLoopUnit->getUnitType());
				UnitClassTypes eUnitClassType = (UnitClassTypes)pLoopUnit->getUnitClassType();

				// we need 2 possible ideal professions for professions in buildings
				ProfessionTypes eIdealProfession = NO_PROFESSION;
				ProfessionTypes eSecondIdealProfession = NO_PROFESSION;

				for (int iI = 0; iI < GC.getNumProfessionInfos(); ++iI)
				{
					ProfessionTypes eLoopProfession = (ProfessionTypes)iI;
					CvProfessionInfo& kProfession = GC.getProfessionInfo(eLoopProfession);

					if (kProfession.isCitizen() && !kProfession.isWorkPlot()) // we are only interested in professions in buildings
					{
						if(eIdealProfession == NO_PROFESSION)
						{
							if ((UnitClassTypes)kProfession.LbD_getExpert() == eUnitClassType)
							{
								eIdealProfession = eLoopProfession;
							}
						}
						else 
						{	
							if ((UnitClassTypes)kProfession.LbD_getExpert() == eUnitClassType)
							{
								eSecondIdealProfession = eLoopProfession;
								break;
							}
						}
					}
				}


				if (eIdealProfession != NO_PROFESSION && eIdealProfession != eProfession)
				{
					CvProfessionInfo& kIdealPro = GC.getProfessionInfo(eIdealProfession);
					YieldTypes eYieldConsumed = (YieldTypes)kIdealPro.getYieldsConsumed(0);
					if (eYieldConsumed != NO_YIELD)
					{
						// R&R, ray , MYCP partially based on code of Aymerick - START
						YieldTypes eYieldProduced = (YieldTypes)kIdealPro.getYieldsProduced(0);
						FAssert(kIdealPro.getYieldsProduced(0) != NO_YIELD);
						// R&R, ray , MYCP partially based on code of Aymerick - END
						int iInputValue = m_aiYieldValuesTimes100[eYieldProduced] / 2;
						m_aiYieldValuesTimes100[eYieldConsumed] = std::max(iInputValue, m_aiYieldValuesTimes100[eYieldConsumed]);
					}

					if (eSecondIdealProfession != NO_PROFESSION && eSecondIdealProfession != eProfession)
					{

						CvProfessionInfo& kIdealPro2 = GC.getProfessionInfo(eSecondIdealProfession);
						YieldTypes eYieldConsumed2 = (YieldTypes)kIdealPro2.getYieldsConsumed(0);
						if (eYieldConsumed2 != NO_YIELD)
						{
							// R&R, ray , MYCP partially based on code of Aymerick - START
							YieldTypes eYieldProduced2 = (YieldTypes)kIdealPro2.getYieldsProduced(0);
							FAssert(kIdealPro2.getYieldsProduced(0) != NO_YIELD);
							// R&R, ray , MYCP partially based on code of Aymerick - END
							int iInputValue = m_aiYieldValuesTimes100[eYieldProduced2] / 2;
							m_aiYieldValuesTimes100[eYieldConsumed2] = std::max(iInputValue, m_aiYieldValuesTimes100[eYieldConsumed2]);
						}
					}
				}
				// R&R, ray, code changes for Ideal Profession - END
			}
		}
	}
}

int CvPlayerAI::AI_transferYieldValue(const IDInfo target, YieldTypes eYield, int iAmount)
{
	FAssertMsg(eYield > NO_YIELD, "Index out of bounds");
	FAssertMsg(eYield < NUM_YIELD_TYPES, "Index out of bounds");

	const IDInfo kEurope(getID(), CvTradeRoute::EUROPE_CITY_ID);
	CvCity* pCity = ::getCity(target);

	int iValue = 0;
	if (pCity != NULL)
	{
		int iStored = pCity->getYieldStored(eYield);
	
		int iMaxCapacity = (eYield == YIELD_FOOD) ? pCity->growthThreshold() : pCity->getMaxYieldCapacity();
		// transport feeder - start - Nightinggale
		//int iMaintainLevel = pCity->getMaintainLevel(eYield);
		int iMaintainLevel = pCity->getAutoMaintainThreshold(eYield);
		// transport feeder - end - Nightinggale
//VET NewCapacity - begin 3/8
		int iTotalStored = pCity->getTotalYieldStored();
		// R&R, ray, improvement
		if (GC.getNEW_CAPACITY() && (iTotalStored > iMaxCapacity / 2) )
		//if (GC.getNEW_CAPACITY())
		{
			int iCargoYields = 0;
			for (int iYield = 1; iYield < NUM_YIELD_TYPES; iYield++)//YIELD_FOOD
			{
				if ((pCity->getYieldStored((YieldTypes)iYield) > 0) && (GC.getYieldInfo(eYield).isCargo()))
					{iCargoYields++;}
			}
			if (iCargoYields < 1)
				{iCargoYields = 1;}
			iMaintainLevel /= iCargoYields;
		}
//VET NewCapacity - end 3/8
		FAssert(iMaxCapacity > 0);
		if (iAmount < 0) // Loading
		{
			int iSurplus = iStored - iMaintainLevel;
			if (iSurplus > 0)
			{
				iValue = std::min(iSurplus, -iAmount);
//VET BugFix - begin /

				//int iMaxCapacity = (eYield == YIELD_FOOD) ? pCity->growthThreshold() : iMaxCapacity = pCity->getMaxYieldCapacity();
				//FAssert(iMaxCapacity > 0);
//VET BugFix - end /
//VET NewCapacity - begin 4/8
				if (GC.getNEW_CAPACITY())
				{
					iValue *= 50 + ((100 * iStored) / std::max(1, iMaxCapacity));
					int iMax = iMaxCapacity * 9 / 10;
					if (iTotalStored >= iMax)
					{
						iValue *= 125 + ((100 * iMax) / iMax);
						iValue /= 100;
					}
				}
				else
				{
//VET NewCapacity - end 4/8
					iValue *= 50 + ((100 * iStored) / std::max(1, iMaxCapacity));
					if (iStored >= iMaxCapacity)
					{
						iValue *= 125 + ((100 * (iStored - iMaxCapacity)) / iMaxCapacity);
						iValue /= 100;
					}
//VET NewCapacity - begin 5/8
				}
//VET NewCapacity - end 5/8
			}
		}
		else //Unloading
		{
			if (iAmount > 0)
			{
//VET NewCapacity - begin 6/8
				iTotalStored += (pCity->AI_getTransitYield(eYield) * 75) / 100;
//VET NewCapacity - end 6/8
				iStored += (pCity->AI_getTransitYield(eYield) * 75) / 100;
				// Erik: Adding this since I suspect a bug may cause this variable to be negative
				FAssertMsg(iStored >= 0, "iStored expected to be >= 0");
			}

			iValue = iAmount * 100;
//VET NewCapacity - begin 7/8
			if (GC.getNEW_CAPACITY())
			{
				int iMax = iMaxCapacity * 9 / 10;
				if (iTotalStored > iMax)
				{
					iValue *= 10;
					iValue /= 100 + ((100 * (iTotalStored - iMax)) / iMax);
				}
				else
				{
					iValue *=  std::max(10, 10 + (100 * (iMaxCapacity - pCity->getTotalYieldStored())) / std::max(1, 10 + iMaxCapacity));
					iValue /= 100;
				}
			}
			else
			{
//VET NewCapacity - end 7/8
				if (iStored > iMaxCapacity)
				{
					iValue *= 10;
					iValue /= 100 + ((100 * (iStored - iMaxCapacity)) / iMaxCapacity);
				}
				else
				{
					iValue *=  std::max(10, 10 + (100 * (iMaxCapacity - pCity->getYieldStored(eYield))) / std::max(1, 10 + iMaxCapacity));
					iValue /= 100;
				}
//VET NewCapacity - begin 8/8
			}
//VET NewCapacity - end 8/8

			if (iStored < iMaintainLevel)
			{
				FAssert(iMaintainLevel > 0);
				iValue *= 125 + 75 * (iMaintainLevel - iStored) / iMaintainLevel;
				iValue /= 100;
			}

			/*
			int iProductionNeeded = 0;
			UnitTypes eUnit = pCity->getProductionUnit();
			if (eUnit != NO_UNIT)
			{
				iProductionNeeded = std::max(iProductionNeeded, pCity->getYieldProductionNeeded(eUnit, eYield));
			}
			BuildingTypes eBuilding = pCity->getProductionBuilding();
			if (eBuilding != NO_BUILDING)
			{
				iProductionNeeded = std::max(iProductionNeeded, pCity->getYieldProductionNeeded(eBuilding, eYield));
			}
			*/
			// production cache - start - Nightinggale
			int iProductionNeeded = pCity->getProductionNeeded(eYield);
			// production cache - end - Nightinggale

			// Erik: Production needed can be 0, so we have to deal with it to avoid dividing by zero
			if (iStored < iProductionNeeded && iProductionNeeded > 0)
			{
				iValue *= 150 + 100 * (iProductionNeeded - iStored) / iProductionNeeded;
				iValue /= 100;
				// transport feeder - start - Nightinggale
				iValue *= 2;
				// transport feeder - end - Nightinggale
			}

			// transport feeder - start - Nightinggale
			if (pCity->getImportsMaintain(eYield))
			{
				iValue *= 2;
			}
			// transport feeder - end - Nightinggale
		}
	}
	else if (target == kEurope)
	{
		if (iAmount < 0) //Loading
		{
			iValue = -iAmount;
		}
		else
		{
			iValue = iAmount;
		}
	}
	else
	{
		FAssertMsg(false, "Invalid Route Destination");
	}

	return iValue;
}

int CvPlayerAI::AI_countYieldWaiting()
{
	int iCount = 0;
	int iLoop;
	CvCity* pLoopCity;

	int iUnitSize = GC.getGameINLINE().getCargoYieldCapacity();
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		// Custom_House_Mod Start
		if (pLoopCity->isBestPortCity())
		// Custom_House_Mod End
		{
			for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
			{
				YieldTypes eLoopYield = (YieldTypes)iYield;
				int iTotal = pLoopCity->getYieldStored(eLoopYield);
				if (iTotal > 0)
				{
					if (pLoopCity->AI_shouldExportYield(eLoopYield))
					{
						iCount += (iTotal + iUnitSize / 2) / iUnitSize;
					}
				}
			}
		}
	}

	return iCount;
}

int CvPlayerAI::AI_highestYieldAdvantage(YieldTypes eYield)
{
	int iBestValue = 0;
	int iLoop;
	CvCity* pLoopCity;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iBestValue = std::max(iBestValue, pLoopCity->AI_getYieldAdvantage(eYield));
	}
	return iBestValue;
}

//Big function to do everything.
void CvPlayerAI::AI_manageEconomy()
{
	if (getNumCities() == 0)
	{
		return;
	}

	bool bAtWar = (GET_TEAM(getTeam()).getAnyWarPlanCount() > 0);
	int iLoop;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			int iWeight = 100;
			YieldTypes eLoopYield = (YieldTypes)iI;

			if (isNative())
			{
				iWeight = 60 + GC.getGame().getSorenRandNum(40, "AI Native Yield Value Randomization 1");

				if (pLoopCity->getTeachUnitClass() != NO_UNITCLASS)
				{
					UnitTypes eUnit = (UnitTypes)GC.getUnitClassInfo(pLoopCity->getTeachUnitClass()).getDefaultUnitIndex();
					if (eUnit != NO_UNIT)
					{
						iWeight += GC.getUnitInfo(eUnit).getYieldModifier(eLoopYield);
					}
				}

				if (eLoopYield == YIELD_FOOD)
				{
					iWeight += bAtWar ? 20 : 0;
				}
				else if (eLoopYield == YIELD_ORE)
				{
					iWeight *= 20 + GC.getGame().getSorenRandNum(80, "AI Native Yield Value Randomization 3");
					iWeight /= 100;
				}
				else if (eLoopYield == YIELD_HORSES)
				{
					iWeight += bAtWar ? 20 : 0;
				}

				int iTotalStored = countTotalYieldStored(eLoopYield);
				int iMaxStored = getNumCities() * GC.getGameINLINE().getCargoYieldCapacity();
				iMaxStored *= GC.getYieldInfo(eLoopYield).getNativeConsumptionPercent();

				int iModifier = 1 + std::max(10, 100 - (100 * iTotalStored) / iMaxStored);
				iWeight *= iModifier;
				iWeight /= 100;
			}

			int iEmphasize = 0;
			for (int i = 0; i < GC.getNumEmphasizeInfos(); ++i)
			{
				if (pLoopCity->AI_isEmphasize((EmphasizeTypes)i))
				{
					CvEmphasizeInfo& kEmphasize = GC.getEmphasizeInfo((EmphasizeTypes)i);
					int iValue = kEmphasize.getYieldChange(eLoopYield);
					if (iValue != 0)
					{
						iEmphasize = iValue;
						break;
					}
				}
			}

			iWeight *= 100 + 133 * std::max(0, iEmphasize);
			iWeight /= 100 + 166 * std::max(0, -iEmphasize);

			pLoopCity->AI_setYieldOutputWeight(eLoopYield, iWeight);
		}
	}

	//Calculate Comparative Advantage in producing various yields.

	//For averages.
	int aiBestYield[NUM_YIELD_TYPES];
	int aiWorstYield[NUM_YIELD_TYPES];

	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		aiBestYield[iYield] = 0;
		aiWorstYield[iYield] = MAX_INT;
	}

	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		for (int i = 0; i < GC.getNumProfessionInfos(); ++i)
		{
			CvProfessionInfo& kProfession = GC.getProfessionInfo((ProfessionTypes)i);

			if (kProfession.isCitizen())
			{
				// R&R, ray , MYCP partially based on code of Aymerick - START
				YieldTypes eYield = (YieldTypes)kProfession.getYieldsProduced(0);
				// R&R, ray , MYCP partially based on code of Aymerick - END
				if (eYield != NO_YIELD)
				{
					int iOutput = pLoopCity->getProfessionOutput((ProfessionTypes)i, NULL);
					pLoopCity->AI_setYieldAdvantage(eYield, iOutput);

					aiBestYield[eYield] = std::max(aiBestYield[eYield], iOutput);
					aiWorstYield[eYield] = std::min(aiWorstYield[eYield], iOutput);
				}
			}
		}
	}

	if (!isNative())
	{
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			int iAdvantageCount = 0;
			for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
			{
				YieldTypes eYield = (YieldTypes)iYield;
				if (aiBestYield[eYield] > 0)
				{
					int iAdvantage = (100 * pLoopCity->AI_getYieldAdvantage(eYield)) / aiBestYield[eYield];
					if (aiBestYield[eYield] == aiWorstYield[eYield])
					{
						iAdvantage *= 99;
						iAdvantage /= 100;
					}
					if (iAdvantage == 100)
					{
						iAdvantageCount++;
					}
					pLoopCity->AI_setYieldAdvantage(eYield, iAdvantage);
				}
			}
			pLoopCity->AI_setTargetSize(std::max(pLoopCity->getHighestPopulation(), 2 + iAdvantageCount * 3));
		}
	}

}

CvPlot* CvPlayerAI::AI_getTerritoryCenter() const
{
	if (getNumCities() == 0)
	{
		return NULL;
	}

	CvCity* pLoopCity;
	int iLoop;

	int iTotalX = 0;
	int iTotalY = 0;
	int iTotalWeight = 0;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		int iWeight = 1 + 10 * pLoopCity->AI_getTargetSize();
		if (pLoopCity->area()->getCitiesPerPlayer(getID()) == 1)
		{
			iWeight /= 2;
		}
		iWeight = std::max(1, iWeight);

		iTotalX += pLoopCity->getX_INLINE() * iWeight;
		iTotalY += pLoopCity->getY_INLINE() * iWeight;
		iTotalWeight += iWeight;
	}

	if (iTotalWeight == 0)
	{
		return NULL;
	}

	iTotalX += iTotalWeight / 2;
	iTotalY += iTotalWeight / 2;

	return GC.getMapINLINE().plotINLINE(iTotalX / iTotalWeight, iTotalY / iTotalWeight);
}

int CvPlayerAI::AI_getTerritoryRadius() const
{
	return 10;
}

void CvPlayerAI::AI_createNatives()
{
	AI_createNativeCities();
	if (getNumCities() == 0)
	{
		return;
	}

	int iLoop;
	CvCity* pLoopCity;
	int iCount = 0;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{

		int iExtraPop = std::max(1, ((getNumCities() - iCount) * 3) / getNumCities());

		if (iCount == 0)
		{
			iExtraPop++;
		}

		//Require a certain minimum food surplus.
		while ((pLoopCity->AI_getFoodGatherable(1 + iExtraPop, GC.getFOOD_CONSUMPTION_PER_POPULATION()) / 2) < GC.getFOOD_CONSUMPTION_PER_POPULATION() * (1 + iExtraPop))
		{
			iExtraPop--;
			if (iExtraPop == 0)
			{
				iExtraPop = 1;
				break;
			}
		}

		pLoopCity->changePopulation(iExtraPop);
		pLoopCity->AI_setTargetSize(pLoopCity->getPopulation());

		iCount++;

		int iBraveCount = pLoopCity->getPopulation() + 2;
		for (int iI = 0; iI < iBraveCount; iI++)
		{
			UnitTypes eBrave = AI_bestUnit(UNITAI_DEFENSIVE);
			if (eBrave != NO_UNIT)
			{
				initUnit(eBrave, (ProfessionTypes) GC.getUnitInfo(eBrave).getDefaultProfession(), pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE());
			}
		}
	}

	for (int iPass = 0; iPass < 2; ++iPass)
	{
		//Now provide some starting yield stockpiles.
		AI_manageEconomy();
		int aiYields[NUM_YIELD_TYPES];
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			pLoopCity->AI_assignWorkingPlots();
			pLoopCity->calculateNetYields(aiYields);

			for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
			{
				YieldTypes eYield = (YieldTypes) iYield;
				if (GC.getYieldInfo(eYield).isCargo())
				{
					pLoopCity->changeYieldStored(eYield, (aiYields[eYield] * (50 + GC.getGameINLINE().getSorenRandNum(50, "AI starting yields"))) / 10);
				}
			}
		}
	}
}

void CvPlayerAI::AI_createNativeCities()
{
	while (true)
	{
		int iBestValue = 0;
		CvPlot* pBestPlot = NULL;
		for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
		{
			CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
			int iValue = AI_foundValueNative(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), false);
			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestPlot = pLoopPlot;
			}
		}
		if (pBestPlot == NULL)
		{
			break;
		}

		CvCity* pCity = pBestPlot->getPlotCity();
		if (pCity != NULL)
		{
			FAssertMsg(false, "City already exists!");
			break;
		}

		found(pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE());
		pCity = pBestPlot->getPlotCity();
		if (pCity == NULL)
		{
			FAssertMsg(false, "Cannot found city!");
			break;
		}
		pCity->setCulture(getID(), 1, true);
	}

	for(int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
		int iCulture = 0;
		int iOursCount = 0;
		if (pLoopPlot->isCityRadius())
		{
			if (pLoopPlot->getOwnerINLINE() == getID())
			{
				iCulture += 90;//XMLize and adjust for game speed...
				if (pLoopPlot->getWorkingCity() != NULL)
				{
					iCulture += 10 * pLoopPlot->getWorkingCity()->getPopulation();
				}
			}
		}
		else
		{
			for (int iJ = 0; iJ < NUM_DIRECTION_TYPES; iJ++)
			{
				CvPlot* pDirectionPlot = plotDirection(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), (DirectionTypes)iJ);
				if (pDirectionPlot != NULL)
				{
					if (pDirectionPlot->isCityRadius() && pDirectionPlot->getOwnerINLINE() == getID())
					{
						iOursCount++;
					}
				}
			}

			int iRand = GC.getGame().getSorenRandNum(2, "AI plot");
			if ((iOursCount >= 3 + iRand) && (pLoopPlot->getOwnerINLINE() == getID() || !pLoopPlot->isOwned()))
			{
				iCulture += 40 + 10 * iOursCount;
			}
			else if (pLoopPlot->getOwnerINLINE() == getID())
			{
				pLoopPlot->setOwner(NO_PLAYER, false);
			}
		}

		int iBestYield = 0;
		int iTotalYield = 0;
		if (iCulture > 0)
		{
			for (int i = 0; i < NUM_YIELD_TYPES; i++)
			{
				YieldTypes eYield = (YieldTypes)i;
				iBestYield = std::max(iBestYield, pLoopPlot->getYield(eYield));
				iTotalYield += pLoopPlot->getYield(eYield);
			}

			iCulture += 10 * iBestYield + (5 * (iTotalYield - iBestYield));

			if (pLoopPlot->getFeatureType() != NO_FEATURE)
			{
				iCulture += pLoopPlot->getYield(YIELD_LUMBER) * 5;
			}

			iCulture *= 100 + GC.getGameINLINE().getSorenRandNum(40, "Native Plot Culture");
			iCulture /= 100;
			pLoopPlot->setCulture(getID(), iCulture, true);
		}
	}


	for(int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);


		if (pLoopPlot->getOwnerINLINE() == getID())
		{
			int iOursCount = 0;
			for (int iJ = 0; iJ < NUM_DIRECTION_TYPES; iJ++)
			{
				CvPlot* pDirectionPlot = plotDirection(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), (DirectionTypes)iJ);
				if (pDirectionPlot != NULL)
				{
					if (pDirectionPlot->getOwnerINLINE() == getID())
					{

						if (pLoopPlot->getX_INLINE() == pDirectionPlot->getX_INLINE() || pLoopPlot->getY_INLINE() == pDirectionPlot->getY_INLINE())
						{
							iOursCount++;
						}
					}
				}
			}
			if (iOursCount <= 1)
			{
				pLoopPlot->setCulture(getID(), 0, true);
			}
		}
	}
}

bool CvPlayerAI::AI_isKing()
{
	return isEurope();
}

CvPlot* CvPlayerAI::AI_getImperialShipSpawnPlot()
{
	CvPlot* pBestPlot = NULL;
	int iBestValue = 0;

	std::deque<bool> zoneAllowable(GC.getNumEuropeInfos(), false);
	bool bNoneAllowable = true;
	for (int iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kPlayer.isAlive() && (kPlayer.getParent() == getID()))
		{
			if (kPlayer.getStartingPlot() != NULL)
			{
				if (kPlayer.getStartingPlot()->getEurope() != NO_EUROPE)
				{
					zoneAllowable[kPlayer.getStartingPlot()->getEurope()] = true;
					bNoneAllowable = false;
				}
			}
		}
	}

	CvTeamAI& kTeam = GET_TEAM(getTeam());

	CvPlot* pTargetPlot = NULL;

	if (AI_isStrategy(STRATEGY_CONCENTRATED_ATTACK))
	{
		pTargetPlot = GC.getMapINLINE().plotByIndexINLINE(AI_getStrategyData(STRATEGY_CONCENTRATED_ATTACK));
	}

	for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

		EuropeTypes eEurope = pLoopPlot->getEurope();
		if (eEurope != NO_EUROPE)
		{
			if (bNoneAllowable || zoneAllowable[eEurope])
			{
				int iEnemyDistance = kTeam.AI_enemyCityDistance(pLoopPlot);
				int iValue = (bNoneAllowable || (!bNoneAllowable && iEnemyDistance == -1)) ? 100 : (10000 / (std::max(1, iEnemyDistance - 2)));

				int iLocation = 50;
				switch ((CardinalDirectionTypes)GC.getEuropeInfo(eEurope).getCardinalDirection())
				{
				case CARDINALDIRECTION_EAST:
				case CARDINALDIRECTION_WEST:
					iLocation = (100 * pLoopPlot->getY_INLINE() + 50) / GC.getMapINLINE().getGridHeightINLINE();
					break;
				case CARDINALDIRECTION_NORTH:
				case CARDINALDIRECTION_SOUTH:
					iLocation = (100 * pLoopPlot->getX_INLINE() + 50) / GC.getMapINLINE().getGridWidthINLINE();
					break;
				default:
					break;
				}

				if (AI_isStrategy(STRATEGY_DISTRIBUTED_ATTACK))
				{
					iValue /= 1 + AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_ASSAULT, NULL, 1);

					int iModifier = 2 * std::abs(iLocation - 50);

					if (iModifier > 95)
					{
						iModifier = 1;
					}
					else if (iModifier > 80)
					{
						iModifier *= (100 - iModifier);
						iModifier /= 100;
					}
					else
					{
						iModifier += 10;
					}

					iValue *= iModifier;
					iValue /= 100;
				}
				if (AI_isStrategy(STRATEGY_CONCENTRATED_ATTACK))
				{
					iValue /= 1 + AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_ASSAULT, NULL, 1);

					if (pTargetPlot == NULL)
					{
						int iModifier = 100 - 2 * std::abs(iLocation - 50);

						if (iModifier < 10)
						{
							iModifier = 1;
						}
						iValue *= iModifier;
						iValue /= 100;
					}
					else
					{
						iValue *= 10;
						iValue /= std::max(1, stepDistance(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), pTargetPlot->getX_INLINE(), pTargetPlot->getY_INLINE()) - 3);
					}
				}

				iValue *= 25 + GC.getGameINLINE().getSorenRandNum(75, "AI best imperial ship spawn plot");

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = pLoopPlot;
				}
			}
		}
	}

	FAssert(pBestPlot != NULL);

	return pBestPlot;
}

void CvPlayerAI::AI_addUnitToMoveQueue(CvUnit* pUnit)
{
	if (std::find(m_unitPriorityHeap.begin(), m_unitPriorityHeap.end(), pUnit->getID()) == m_unitPriorityHeap.end())
	{
		m_unitPriorityHeap.push_back(pUnit->getID());
		std::push_heap(m_unitPriorityHeap.begin(), m_unitPriorityHeap.end(), CvShouldMoveBefore(getID()));
	}
	else
	{
		std::make_heap(m_unitPriorityHeap.begin(), m_unitPriorityHeap.end(), CvShouldMoveBefore(getID()));
	}
}

void CvPlayerAI::AI_removeUnitFromMoveQueue(CvUnit* pUnit)
{
	std::vector<int>::iterator it;
	it = std::find(m_unitPriorityHeap.begin(), m_unitPriorityHeap.end(), pUnit->getID());
	if (it != m_unitPriorityHeap.end())
	{
		m_unitPriorityHeap.erase(it);
		std::make_heap(m_unitPriorityHeap.begin(), m_unitPriorityHeap.end(), CvShouldMoveBefore(getID()));
	}
}

void CvPlayerAI::AI_verifyMoveQueue()
{
	std::vector<int>::iterator it = std::partition(m_unitPriorityHeap.begin(), m_unitPriorityHeap.end(), CvShouldUnitMove(getID()));
	m_unitPriorityHeap.erase(it, m_unitPriorityHeap.end());
	std::make_heap(m_unitPriorityHeap.begin(), m_unitPriorityHeap.end(), CvShouldMoveBefore(getID()));
}

CvUnit* CvPlayerAI::AI_getNextMoveUnit()
{
	CvUnit* pUnit = getUnit(m_unitPriorityHeap.front());
	std::pop_heap(m_unitPriorityHeap.begin(), m_unitPriorityHeap.end(), CvShouldMoveBefore(getID()));
	m_unitPriorityHeap.pop_back();
	return pUnit;
}

int CvPlayerAI::AI_highestProfessionOutput(ProfessionTypes eProfession, const CvCity* pIgnoreCity)
{
	int iLoop;
	CvCity* pLoopCity;

	int iBestYield = 0;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity != pIgnoreCity)
		{
			iBestYield = std::max(iBestYield, pLoopCity->getProfessionOutput(eProfession, NULL));

			//Also consider buildings under construction.
			CLLNode<OrderData>* pOrderNode = pLoopCity->headOrderQueueNode();

			while (pOrderNode != NULL)
			{
				switch (pOrderNode->m_data.eOrderType)
				{
				case ORDER_TRAIN:
				case ORDER_CONVINCE:
					break;

				case ORDER_CONSTRUCT:
					{
						BuildingTypes eBuilding = ((BuildingTypes)(pOrderNode->m_data.iData1));
						if (eBuilding != NO_BUILDING)
						{
							if (GC.getProfessionInfo(eProfession).getSpecialBuilding() == GC.getBuildingInfo(eBuilding).getSpecialBuildingType())
							{
								iBestYield = std::max(iBestYield, GC.getBuildingInfo(eBuilding).getProfessionOutput());
							}
						}
						break;
					}

				default:
					FAssertMsg(false, "pOrderNode->m_data.eOrderType failed to match a valid option");
					break;
				}
				pOrderNode = pLoopCity->nextOrderQueueNode(pOrderNode);
			}
		}
	}

	return iBestYield;
}

CvCity* CvPlayerAI::AI_bestCityForBuilding(BuildingTypes eBuilding)
{
	int iBestValue = 0;
	CvCity* pBestCity = NULL;

	int iLoop;
	CvCity* pLoopCity;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (!pLoopCity->isHasConceptualBuilding(eBuilding))
		{
			int iValue = pLoopCity->AI_buildingValue(eBuilding);
			if (iValue > iBestValue)
			{
				pBestCity = pLoopCity;
				iBestValue = iValue;
			}
		}
	}

	return pBestCity;
}

UnitTypes CvPlayerAI::AI_bestUnit(UnitAITypes eUnitAI, CvArea* pArea)
{
	FAssertMsg(eUnitAI != NO_UNITAI, "UnitAI is not assigned a valid value");

	int iBestValue = 0;
	UnitTypes eBestUnit = NO_UNIT;

	for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		UnitTypes eLoopUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI)));

		if (eLoopUnit != NO_UNIT)
		{
			if (eUnitAI == NO_UNITAI || GC.getUnitInfo(eLoopUnit).getDefaultUnitAIType() == eUnitAI)
			{

				int iValue = AI_unitValue(eLoopUnit, eUnitAI, pArea);

				if (iValue > 0)
				{
					iValue *= (GC.getGameINLINE().getSorenRandNum(40, "AI Best Unit") + 100);
					iValue /= 100;

					iValue *= (getNumCities() + 2);
					iValue /= (getUnitClassCountPlusMaking((UnitClassTypes)iI) + getNumCities() + 2);

					FAssert((MAX_INT / 1000) > iValue);
					iValue *= 1000;

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

	return eBestUnit;
}

int CvPlayerAI::AI_desiredCityCount()
{
	bool bDense = AI_isStrategy(STRATEGY_DENSE_CITY_SPACING);
	int iCount = 0;

	int iStep = 4;

	int iTotal = getTotalPopulation();
	if (AI_isStrategy(STRATEGY_DENSE_CITY_SPACING))
	{
		iTotal *= 133;
		iTotal /= 100;
	}

	while (iTotal > 0)
	{
		iTotal -= iStep;
		iStep += 3 + std::max(0, iCount - 4);

		iCount++;
	}

	return std::max(1, iCount);
}

int CvPlayerAI::AI_professionBasicValue(ProfessionTypes eProfession, UnitTypes eUnit, CvCity* pCity)
{
	CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);
	// R&R, ray , MYCP partially based on code of Aymerick - START
	YieldTypes eYield = (YieldTypes)kProfession.getYieldsProduced(0);
	// R&R, ray , MYCP partially based on code of Aymerick - END
	if (eYield == NO_YIELD)
	{
		return 0;
	}

	CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);
	int iBestValue = 0;

	if (kProfession.isCitizen())
	{
		if (!kProfession.isWorkPlot())
		{
			int iNewOutput = pCity->AI_professionBasicOutput(eProfession, eUnit, NULL);
			int iProfessionCount = 0;
			bool bDone = false;
			for (int i = 0; i < pCity->getPopulation(); ++i)
			{
				CvUnit* pLoopUnit = pCity->getPopulationUnitByIndex(i);
				if (pLoopUnit->getProfession() == eProfession)
				{
					int iOldOutput = pCity->AI_professionBasicOutput(eProfession, pLoopUnit->getUnitType(), NULL);

					if (iNewOutput > iOldOutput)
					{
						int iValue = AI_yieldValue(eYield, true, iNewOutput);
						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							break;
						}
					}
					else
					{
						iProfessionCount ++;
					}
				}
			}
			if (iBestValue == 0)
			{
				if (iProfessionCount < pCity->getNumProfessionBuildingSlots(eProfession))
				{
					iBestValue = AI_yieldValue(eYield, true, iNewOutput);
				}
			}
		}
		else
		{
			for (int i = 0; i < NUM_CITY_PLOTS; ++i)
			{
				CvPlot* pLoopPlot = plotCity(pCity->getX_INLINE(), pCity->getY_INLINE(), i);
				if ((pLoopPlot != NULL) && (pLoopPlot->getWorkingCity()==pCity))
				{
					int iNewOutput = pCity->AI_professionBasicOutput(eProfession, eUnit, pLoopPlot);
					int iOldOutput = 0;
					if (pLoopPlot->isBeingWorked())
					{
						CvUnit* pWorkingUnit = pCity->getUnitWorkingPlot(pLoopPlot);
						if (pWorkingUnit != NULL)
						{
							if ((pWorkingUnit->getProfession() == eProfession))
							{
								iOldOutput = pCity->AI_professionBasicOutput(eProfession, pWorkingUnit->getUnitType(), pLoopPlot);
								if (iNewOutput <= iOldOutput)
								{
									iNewOutput = 0;
								}
							}
							else
							{
								bool bOverride = false;
								if ((pLoopPlot->getBonusType() != NO_BONUS) && (GC.getBonusInfo(pLoopPlot->getBonusType()).getYieldChange(eYield) > 0))
								{
									bOverride = true;
								}
								if (!bOverride)
								{
									int iBestYield = AI_getBestPlotYield(eYield);
									if (pLoopPlot->getBonusType() == NO_BONUS)
									{
										if (pLoopPlot->calculateBestNatureYield(eYield, getTeam()) >= iBestYield)
										{
											bOverride = true;
										}
									}
								}
								if (!bOverride)
								{
									iNewOutput = 0;
								}
							}
						}
					}

					if (iNewOutput > iOldOutput)
					{
						int iValue = AI_yieldValue(eYield, true, iNewOutput);
						if (iValue > iBestValue)
						{
							iBestValue = iValue;
						}
					}
				}
			}
		}
	}

	return iBestValue;
}

int CvPlayerAI::AI_professionUpgradeValue(ProfessionTypes eProfession, UnitTypes eUnit)
{
	CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);
	// R&R, ray , MYCP partially based on code of Aymerick - START
	YieldTypes eYield = (YieldTypes)kProfession.getYieldsProduced(0);
	// R&R, ray , MYCP partially based on code of Aymerick - END
	if (eYield == NO_YIELD)
	{
		return 0;
	}

	CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);

	int iBestValue = 0;
	CvCity* pBestCity = NULL;

	int iLoop;
	CvCity* pLoopCity;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		for (int i = 0; i < pLoopCity->getPopulation(); ++i)
		{
			CvUnit* pLoopUnit = pLoopCity->getPopulationUnitByIndex(i);

			if (pLoopUnit->getProfession() == eProfession)
			{
				int iExistingYield = 0;
				int iNewYield = 0;
				int iExtraMultiplier = 0;
				if (kProfession.isWorkPlot())
				{
					CvPlot* pWorkedPlot = pLoopCity->getPlotWorkedByUnit(pLoopUnit);
					if (pWorkedPlot != NULL)
					{
						iExistingYield = pWorkedPlot->calculatePotentialYield(eYield, getID(), pWorkedPlot->getImprovementType(), false, pWorkedPlot->getRouteType(), pLoopUnit->getUnitType(), false);
						iNewYield = pWorkedPlot->calculatePotentialYield(eYield, getID(), pWorkedPlot->getImprovementType(), false, pWorkedPlot->getRouteType(), eUnit, false);
						if (pWorkedPlot->getBonusType() != NO_BONUS && GC.getBonusInfo(pWorkedPlot->getBonusType()).getYieldChange(eYield) > 0)
						{
							iExtraMultiplier += 100;
						}
					}
				}
				else
				{
					CvUnitInfo& kLoopUnit = GC.getUnitInfo(pLoopUnit->getUnitType());
					iExistingYield = pLoopCity->getProfessionOutput(eProfession, pLoopUnit);
					iNewYield = (iExistingYield * 100) / (100 + kLoopUnit.getYieldModifier(eYield));
					iNewYield -= kLoopUnit.getYieldChange(eYield);
					iNewYield += kUnit.getYieldChange(eYield);
					iNewYield = iNewYield * (100 + kUnit.getYieldModifier(eYield)) / 100;
				}

				if (iNewYield > iExistingYield)
				{
					int iValue = AI_yieldValue(eYield, true, iNewYield - iExistingYield);
					iValue *= 100 + iExtraMultiplier;
					iValue /= 100;
					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestCity = pLoopCity;
					}
				}
			}
		}
	}

	return iBestValue;
}

int CvPlayerAI::AI_professionValue(ProfessionTypes eProfession, UnitAITypes eUnitAI)
{
	CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);
	if (kProfession.isCitizen())
	{
		return 0;
	}

	int iValue = 0;
	switch (eUnitAI)
	{
		case UNITAI_UNKNOWN:
		case UNITAI_ANIMAL: // R&R, ray, Wild Animals
		case UNITAI_ANIMAL_SEA: // R&R, ray, Wild Animals
		case UNITAI_FLEEING: // R&R, ray, Fleeing
			break;

		case UNITAI_COLONIST:
			{
				if (GC.getCivilizationInfo(getCivilizationType()).getDefaultProfession() == eProfession)
				{
					iValue += 100;
				}
			}
			break;

		case UNITAI_SETTLER:
			{
				if (kProfession.canFound())
				{
					iValue += 200; //WTP, ray, Settler Professsion - START
				}
			}
			break;

		case UNITAI_WORKER:
			{
				if (kProfession.getWorkRate() > 0)
				{
					iValue += kProfession.getWorkRate();
				}
			}
			break;

		case UNITAI_MISSIONARY:
			{
				if (kProfession.getMissionaryRate() > 0)
				{
					iValue += kProfession.getMissionaryRate();
				}
			}
			break;
		// WTP, ray, Native Trade Posts - START
		case UNITAI_TRADER:
			{
				if (kProfession.getNativeTradeRate() > 0)
				{
					iValue += kProfession.getNativeTradeRate();
				}
			}
			break;

		case UNITAI_SCOUT:
			{
				if (kProfession.isScout())
				{
					iValue += 100;
				}
				iValue += 20 * kProfession.getMovesChange();
			}
			break;

		case UNITAI_WAGON:
		case UNITAI_TREASURE:
		case UNITAI_YIELD:
		case UNITAI_GENERAL:
			break;

		case UNITAI_DEFENSIVE:
			{
				int iExtraCombatStrength = kProfession.getCombatChange() - GC.getProfessionInfo((ProfessionTypes) GC.getCivilizationInfo(getCivilizationType()).getDefaultProfession()).getCombatChange();
				if (isNative())
				{
					iValue += 10;
				}

				if (!kProfession.isUnarmed() && iExtraCombatStrength > 0)
				{
					if (kProfession.isCityDefender())
					{
						iValue += iExtraCombatStrength * 25;
					}
				}
			}
			break;

		case UNITAI_OFFENSIVE:
			// TAC - AI purchases military units - koma13 - START
			//if (isNative())
			{
				iValue += 10;
				int iExtraCombatStrength = kProfession.getCombatChange() - GC.getProfessionInfo((ProfessionTypes) GC.getCivilizationInfo(getCivilizationType()).getDefaultProfession()).getCombatChange();
				if (!kProfession.isUnarmed() && iExtraCombatStrength > 0)
				{
					iValue += iExtraCombatStrength * 15;
					//iValue += kProfession.getMovesChange() * 15;
					iValue += kProfession.getMovesChange() * 10;
				}
			}
			// TAC - AI purchases military units - koma13 - END

			break;
		case UNITAI_COUNTER:
			{
				int iExtraCombatStrength = kProfession.getCombatChange() - GC.getProfessionInfo((ProfessionTypes) GC.getCivilizationInfo(getCivilizationType()).getDefaultProfession()).getCombatChange();
				if (isNative())
				{
					iValue += 10;
				}

				if (isNative() || (!kProfession.isUnarmed() && iExtraCombatStrength > 0))
				{
					iValue += iExtraCombatStrength * 15;
					iValue *= 1 + kProfession.getMovesChange();
				}
			}
			break;
		
		case UNITAI_WORKER_SEA: //TAC Whaling, ray
		case UNITAI_TRANSPORT_SEA:
		case UNITAI_TRANSPORT_COAST:
		case UNITAI_ASSAULT_SEA:
		case UNITAI_COMBAT_SEA:
		case UNITAI_PIRATE_SEA:
		case UNITAI_ESCORT_SEA: // TAC - AI Escort Sea - koma13
			break;
		default:
			FAssert(false);
			break;
	}
	return iValue;
}

// R&R, ray, code changes for Ideal Profession - START
// Heavily modified
// simply returns the first Expert to Profession that matches UnitType
ProfessionTypes CvPlayerAI::AI_idealProfessionForUnit(UnitTypes eUnitType)
{
	int eUnitClassType = GC.getUnitInfo(eUnitType).getUnitClassType();
	ProfessionTypes eIdealProfession = NO_PROFESSION;

	for (int iI = 0; iI < GC.getNumProfessionInfos(); ++iI)
	{
		ProfessionTypes eLoopProfession = (ProfessionTypes)iI;
		CvProfessionInfo& kProfession = GC.getProfessionInfo(eLoopProfession);

		if (kProfession.isCitizen())
		{
			if (kProfession.LbD_getExpert() == eUnitClassType)
			{
				eIdealProfession = eLoopProfession;
				break;
			}
		}
	}
	return eIdealProfession;
}
// R&R, ray, code changes for Ideal Profession - END

ProfessionTypes CvPlayerAI::AI_idealProfessionForUnitAIType(UnitAITypes eUnitAI, CvCity* pCity)
{
	int iBestValue = 0;
	ProfessionTypes eBestProfession = NO_PROFESSION;

	for (int iI = 0; iI < GC.getNumProfessionInfos(); ++iI)
	{
		ProfessionTypes eLoopProfession = (ProfessionTypes)iI;
		CvProfessionInfo& kProfession = GC.getProfessionInfo(eLoopProfession);

		if (!(kProfession.isCitizen() || kProfession.isWorkPlot()))
		{
			//WTP, ray, Settler Professsion - START
			if (GC.getCivilizationInfo(getCivilizationType()).isValidProfession(eLoopProfession))
			{
				//we can only do this, if it is checking for a Unit in a City
				if (eUnitAI == UNITAI_SETTLER && kProfession.canFound() && pCity != NULL)
				{
					//we better check the size of the City
					//I have no idea, what the population check below in else is supposed to do
					if (pCity->getPopulation() > 1)
					{
						iBestValue = MAX_INT;
						eBestProfession = eLoopProfession;
						break; // do not loop through professions anymore, we already found the best Profession for SETTLER_AI
					}
				}
			
				else 
				//WTP, ray, Settler Professsion - END
				{
					CvUnit* pUnit = NULL;
					if (pCity != NULL)
					{
						pUnit = pCity->getPopulationUnitByIndex(0);
					}
					if (pUnit == NULL || pUnit->canHaveProfession(eLoopProfession, true))
					{
						int iValue = AI_professionValue(eLoopProfession, eUnitAI);

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							eBestProfession = eLoopProfession;
						}
					}
				}
			}
		}
	}

	return eBestProfession;
}

//100 means "An average use for Gold", so a higher multiplier here, means a higher priority on buying that unit type.
//Most notably a "0" means "Don't bother".
int CvPlayerAI::AI_unitAIValueMultipler(UnitAITypes eUnitAI)
{
	int iCurrentCount = AI_totalUnitAIs(eUnitAI);
	int iCount = AI_totalUnitAIs(eUnitAI) + AI_getNumRetiredAIUnits(eUnitAI);
	// WTP, ray, Native Trade Posts - START
	// adjustment for UNITAI_TRADER
	int iPopulation = AI_getPopulation() + AI_getNumRetiredAIUnits(UNITAI_MISSIONARY) + AI_getNumRetiredAIUnits(UNITAI_TRADER); 
	int iValue = 0;
	switch (eUnitAI)
	{
		case UNITAI_UNKNOWN:
			break;

		case UNITAI_COLONIST:
			{
				iValue = std::min(75, 20 + iPopulation);
				if (AI_isStrategy(STRATEGY_REVOLUTION_PREPARING))
				{
					iValue *= 75;
					iValue /= 100;
					if (AI_isStrategy(STRATEGY_REVOLUTION_DECLARING))
					{
						iValue *= 75;
						iValue /= 100;
					}
				}
			}
			break;

		case UNITAI_SETTLER:
			{
				if (!AI_isStrategy(STRATEGY_REVOLUTION_PREPARING))
				{
					int iDesiredCities = AI_desiredCityCount();
					if (iDesiredCities > (getNumCities() + iCount))
					{
						iValue = 100 + 150 * (iDesiredCities - (getNumCities() + iCount));
					}
				}
			}
			break;

		case UNITAI_WORKER:
			if (!AI_isStrategy(STRATEGY_REVOLUTION_DECLARING))
			{
				int iNeeded = AI_neededWorkers(NULL);

				if (iNeeded > iCount)
				{
					iValue = 100 + 20 * iNeeded + (50 * iNeeded) / (iCount + 1);
				}
			}
			break;

		case UNITAI_MISSIONARY:
			if (!AI_isStrategy(STRATEGY_REVOLUTION_PREPARING))
			{
				int iLowerPop = 12;
				int iPop = 15 + iCount * 5;
				int iModifier = 0;
				iModifier -= getNativeCombatModifier() * 4;
				iModifier += getMissionaryRateModifier();
				iModifier += 10 * GC.getLeaderHeadInfo(getLeaderType()).getNativeAttitude();
				iModifier += getMissionarySuccessPercent() - 50;
				if (iModifier != 0)
				{
					iLowerPop *= 100 + std::max(0, -iModifier);
					iLowerPop /= 100 + std::max(0, iModifier);
					iPop *= 100 + std::max(0, -iModifier);
					iPop /= 100 + std::max(0, iModifier);
				}

				iValue = (100 * std::max(0, iPopulation - (iLowerPop + iPop * iCount))) / iPop;
				if (iValue > 0)
				{
					iValue += 100;
				}
			}
			break;

		// WTP, ray, Native Trade Posts - START
		case UNITAI_TRADER:
			if (!AI_isStrategy(STRATEGY_REVOLUTION_PREPARING))
			{
				int iLowerPop = 12;
				int iPop = 15 + iCount * 5;
				int iModifier = 0;
				iModifier -= getNativeCombatModifier() * 4;
				iModifier += getNativeTradeModifier();
				iModifier += 10 * GC.getLeaderHeadInfo(getLeaderType()).getNativeAttitude();
				iModifier += getNativeTradePostSuccessPercent() - 50;
				if (iModifier != 0)
				{
					iLowerPop *= 100 + std::max(0, -iModifier);
					iLowerPop /= 100 + std::max(0, iModifier);
					iPop *= 100 + std::max(0, -iModifier);
					iPop /= 100 + std::max(0, iModifier);
				}

				iValue = (100 * std::max(0, iPopulation - (iLowerPop + iPop * iCount))) / iPop;
				if (iValue > 0)
				{
					iValue += 100;
				}
			}
			break;
		// WTP, ray, Native Trade Posts - END

		case UNITAI_SCOUT:
			if (!AI_isStrategy(STRATEGY_REVOLUTION_PREPARING))
			{
				if (iCount <= 1)
				{
					int iTotalUnexploredPlots = 0;

					CvArea* pLoopArea;
					int iLoop;
					for(pLoopArea = GC.getMapINLINE().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMapINLINE().nextArea(&iLoop))
					{
						if (!(pLoopArea->isWater()))
						{
							iTotalUnexploredPlots += pLoopArea->getNumRevealedTiles(getTeam());
						}
					}

					if (iTotalUnexploredPlots > 10)
					{
						if (iCount == 0)
						{
							iValue = 500;//scout is important.
						}
						else if (iCount == 1)
						{
							if ((iTotalUnexploredPlots > 500) && (iPopulation > 5))
							{
								iValue = 150;
							}
						}
					}
				}
			}
			break;

		case UNITAI_WAGON:
			{
				// Erik: The number of land transports should depend on the production
				// vs. consumption rate of all cargo yields as well as the number of cities
				const int iCityCount = getNumCities();
				const int iNeeded = iCityCount*iCityCount / (countNumCoastalCities() + 1);

				if (iCount < iNeeded)
				{
					iValue = 100 + (100 * (iNeeded - iCurrentCount));
				}				
			}
			break;

		case UNITAI_TRANSPORT_COAST:
			{
				const int iNeeded = countNumCoastalCities();

				if (iCount < iNeeded)
				{
					iValue = 100 + (100 * (iNeeded - iCurrentCount));
				}
			}
			break;

		case UNITAI_TREASURE:
		case UNITAI_YIELD:
		case UNITAI_GENERAL:
		case UNITAI_ANIMAL: // R&R, ray adjustments to new UnitAIs
		case UNITAI_ANIMAL_SEA: // R&R, ray adjustments to new UnitAIs
		case UNITAI_FLEEING: // R&R, ray adjustments to new UnitAIs
			break; 

		case UNITAI_DEFENSIVE:
			{
				if (isNative())
				{
					return 100;
				}

				bool bAtWar = GET_TEAM(getTeam()).getAnyWarPlanCount();
				
				// TAC - AI City Defense - koma13 - START
				/*
				int iLowerPop = bAtWar ? 8 : 15;
				int iPop = bAtWar ? (10 + 5 * iCount) : 17 + 6 * iCount;
				iValue = 110 * std::max(0, iPopulation - (iLowerPop + iPop * iCount)) / iPop;
				
				int iUndefended = 0;
				int iNeeded = AI_totalDefendersNeeded(&iUndefended);
				if (iUndefended > 0)
				{
					iValue += 100 + ((200 * iUndefended) / (1 + getNumCities()));
				}
				
				if (iCount < getNumCities())
				{
					iValue += 100 + ((200 * (getNumCities() - iCount)) / (1 + getNumCities()));
				}
				
				iValue += (200 * iNeeded) / (1 + getNumCities());
				
				if (AI_isStrategy(STRATEGY_REVOLUTION_DECLARING))
				{
					iValue += 3000 / (25 + iCount);
				}

				if (AI_isStrategy(STRATEGY_REVOLUTION_PREPARING))
				{
					iValue += 3000 / (25 + iCount);
				}

				if (getGold() > 5000)
				{
					iValue += 1;
					iValue *= 2;
				}
				*/

				int iUndefended = 0;
				int iNeeded = AI_totalDefendersNeeded(&iUndefended, NULL, true);
				
				if (iNeeded <= 0)
				{
					return 0;
				}

				if (iCount < getNumCities())
				{
					iValue += 100 + ((200 * (getNumCities() - iCount)) / (1 + getNumCities()));
				}
				
				iValue += ((bAtWar ? 100 : 50) * iNeeded) / (1 + getNumCities());
				
				if (AI_isStrategy(STRATEGY_REVOLUTION_DECLARING))
				{
					iValue += 3000 / (25 + iCount);
				}

				if (AI_isStrategy(STRATEGY_REVOLUTION_PREPARING))
				{
					iValue += 3000 / (25 + iCount);
				}
				// TAC - AI City Defense - koma13 - END
			}
			break;

		case UNITAI_OFFENSIVE:
			{
				bool bAtWar = GET_TEAM(getTeam()).getAnyWarPlanCount();
				int iLowerPop = bAtWar ? 9 : 15 ;
				int iPop = bAtWar ? 10 : 20;
				iValue = 100 * std::max(0, iPopulation - (iLowerPop + iPop * iCount)) / iPop;

				if (AI_isStrategy(STRATEGY_REVOLUTION_DECLARING))
				{
					iValue += 3000 / (40 + iCount);
				}
				else
				{
					if (iValue > 0)
					{
						iValue += 50 + getNativeCombatModifier() * 2;
					}
				}

				if (AI_isStrategy(STRATEGY_REVOLUTION_PREPARING))
				{
					iValue += 1500 / (40 + iCount);
				}
				
				// TAC - AI purchases military units - koma13 - START
				if (!isEurope() && !isNative())
				{
					iValue += 1500 / (40 + iCount);
				}
				// TAC - AI purchases military units - koma13 - END
			}
			break;

		case UNITAI_COUNTER:
			{
				// TAC - AI Counter Units - koma13 - START
				if (iCount >= getNumCities())
				{
					return 0;
				}
				// TAC - AI Counter Units - koma13 - END
				
				bool bAtWar = GET_TEAM(getTeam()).getAnyWarPlanCount();
				int iPop = bAtWar ? 12 : 20;
				int iLowerPop = bAtWar ? 8 : 14;
				iValue = 100 * std::max(0, iPopulation - (iLowerPop + iPop * iCount)) / iPop;
				if (iValue > 0)
				{
					iValue += 25;
				}

				if (AI_isStrategy(STRATEGY_REVOLUTION_DECLARING))
				{
					iValue += 3000 / (25 + iCount);
				}

				if (AI_isStrategy(STRATEGY_REVOLUTION_PREPARING))
				{
					iValue += 3000 / (25 + iCount);
				}

				if (getGold() > 5000)
				{
					iValue += 1;
					iValue *= 2;
				}
			}
			break;

		//TAC Whaling, ray
		case UNITAI_WORKER_SEA:
			if (!AI_isStrategy(STRATEGY_REVOLUTION))
			{
				int iNeeded = countNumCoastalCities();
				if (iCount < iNeeded)
				{
					iValue = 100 + 20 * iNeeded + (50 * iNeeded) / (iCount + 1);
				}
			}
			break;

		case UNITAI_TRANSPORT_SEA:
			if (!AI_isStrategy(STRATEGY_REVOLUTION))
			{
				// TAC - AI purchases military units - koma13 - START
				//if (iCount < 6)
				if (iCount < std::max(3, getNumCities()))
				{
					int iLowerPop = 5 - countNumCoastalCities();
					int iPop = 13 + 26 * iCount;
					
					iValue = 150 * std::max(0, iPopulation - (iLowerPop + iPop * iCount)) / iPop;

					//iValue += 25 * std::max(0, AI_countYieldWaiting() - 4 * iCount);
					iValue += 25 * std::max(0, AI_countYieldWaiting() - iCount);
					iValue += 25 * std::max(0, getNumEuropeUnits() - 4 * iCount);	
				}
				// TAC - AI purchases military units - koma13 - END
			}
			break;

		case UNITAI_ASSAULT_SEA:
			{
				if (AI_prepareAssaultSea())	// TAC - AI Assault Sea - koma13
				{
					int iLowerPop = 5;
					int iPop = 15 + 50 * iCount;
					iValue = (160 * std::max(0, iPopulation - (iLowerPop + iPop * iCount))) / iPop;
				}
			}
		case UNITAI_COMBAT_SEA:
			{
				int iLowerPop = 5;
				int iPop = 40 + 30 * iCount;
				if (AI_isStrategy(STRATEGY_REVOLUTION_PREPARING))
				{
					iPop /= 2;
				}
				iValue = (140 * std::max(0, iPopulation - (iLowerPop + iPop * iCount))) / iPop;
			}
			break;

		case UNITAI_PIRATE_SEA:
			{
				// TAC - AI purchases military units - koma13 - START
				int iCurrentCount = AI_totalUnitAIs(eUnitAI);
				//if (iCount < 2)
				if (iCurrentCount < 2)
				{
					/*
					int iLowerPop = 5;
					int iPop = 16 + 6 * iCount;
					iValue = (140 * std::max(0, iPopulation - (iLowerPop + iPop * iCount))) / iPop;
					*/
					int iLowerPop = 5;
					int iPop = 16 + 6 * iCurrentCount;
					iValue = (140 * std::max(0, iPopulation - (iLowerPop + iPop * iCurrentCount))) / iPop;
					
					int iGameTurn = GC.getGameINLINE().getGameTurn();
					int iEndTurn = GC.getGameINLINE().getEstimateEndTurn();
					int iTrainPercent = GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAITrainPercent();
					int iTempValue = (iTrainPercent * iEndTurn * ((iCount * iCount) / 2)) / ((1 + iGameTurn) * 3);	
					iValue = std::max(0, iValue - iTempValue);
				}
				// TAC - AI purchases military units - koma13 - END
			}
			break;

		// TAC - AI Escort Sea - koma13 - START
		case UNITAI_ESCORT_SEA:
			{
				if (iCount < AI_totalUnitAIs(UNITAI_ASSAULT_SEA))
				{
					int iLowerPop = 5;
					int iPop = 15 + 50 * iCount;
					iValue = (160 * std::max(0, iPopulation - (iLowerPop + iPop * iCount))) / iPop;
				}
			}
			break;
		// TAC - AI Escort Sea - koma13 - END

		default:
			FAssert(false);
			break;
	}

	iValue *= 100 + m_aiUnitAIStrategyWeights[eUnitAI];
	iValue /= 100;

	return iValue;
}

bool CvPlayerAI::AI_isCityAcceptingYield(CvCity* pCity, YieldTypes eYield)
{
	for (CvIdVector<CvTradeRoute>::iterator it = m_tradeRoutes.begin(); it != m_tradeRoutes.end(); ++it)
	{
		CvTradeRoute* pTradeRoute = it->second;

		if (pTradeRoute->getYield() == eYield)
		{
			if (pTradeRoute->getDestinationCity() == pCity->getIDInfo())
			{
				return true;
			}
		}
	}

	return false;
}

// TAC - AI Economy - koma13 - START
bool CvPlayerAI::AI_isTradeRouteConnection(CvCity* pSourceCity, CvCity* pDestinationCity)
{
	FAssert(pSourceCity != NULL || pDestinationCity != NULL);
	
	for (CvIdVector<CvTradeRoute>::iterator it = m_tradeRoutes.begin(); it != m_tradeRoutes.end(); ++it)
	{
		CvTradeRoute* pTradeRoute = it->second;

		if (pTradeRoute->getSourceCity() == pSourceCity->getIDInfo())
		{
			if (pTradeRoute->getDestinationCity() == pDestinationCity->getIDInfo())
			{
				return true;
			}
		}
	}

	return false;
}
// TAC - AI Economy - koma13 - END
int CvPlayerAI::AI_professionSuitability(UnitTypes eUnit, ProfessionTypes eProfession) const
{
	if (eProfession == NO_PROFESSION)
	{
		return 0;
	}

	if (!GC.getCivilizationInfo(getCivilizationType()).isValidProfession(eProfession))
	{
		return 0;
	}


	CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);

	if (kUnit.getDefaultProfession() == NO_PROFESSION)
	{
		return 0;
	}

	if (eProfession == (ProfessionTypes) GC.getCivilizationInfo(getCivilizationType()).getDefaultProfession())
	{
		return 100;
	}

	CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);
	int iValue = 100;

	int iPositiveYields = 0;
	int iNegativeYields = 0;

	int iProModifiers = 0;
	int iConModifiers = 0;

	if (kProfession.isWater() && kUnit.isWaterYieldChanges() || !kProfession.isWater() && kUnit.isLandYieldChanges())
	{

		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			YieldTypes eLoopYield = (YieldTypes)iYield;


			int iModifier = kUnit.getYieldModifier(eLoopYield);
			// XXX account for kUnit.getYieldChange, kUnit.getBonusYieldChange
			int iYieldChange = kUnit.getYieldChange(eLoopYield) * 2 + kUnit.getBonusYieldChange(eLoopYield);
			iModifier += 10 * iYieldChange;

			if (iModifier != 0)
			{
				// R&R, ray , MYCP partially based on code of Aymerick - START
				if (kProfession.getYieldsProduced(0) == eLoopYield)
				// R&R, ray , MYCP partially based on code of Aymerick - END
				{
					//We produce enhanced yield for this profession.
					if (iModifier > 0)
					{
						iProModifiers += iModifier;
					}
					else //We produce reduced yield for this profession.
					{
						iConModifiers += iModifier;
					}
				}
				else
				{
					//We produce enhanced yield for ANOTHER profession.
					if (iModifier > 0)
					{
						iNegativeYields = std::max(iModifier, iNegativeYields);
					}
				}
			}
		}

		iProModifiers += iPositiveYields / 8;
		iConModifiers += iNegativeYields / 20;

	}

	if (!kProfession.isCitizen())
	{
		int iChange = kUnit.getYieldModifier(YIELD_FOOD) / 10 + kUnit.getYieldChange(YIELD_FOOD) * 3 + kUnit.getYieldChange(YIELD_FOOD) * 2;
		if (iChange > 0)
		{
			iConModifiers += iChange;
		}
	}

	if (kProfession.getMissionaryRate() > 0)
	{
		int iModifier = kUnit.getMissionaryRateModifier();
		if (iModifier > 0)
		{
			iProModifiers += iModifier;
		}
		else
		{
			iConModifiers += iModifier / 8;
		}
	}

	// WTP, ray, Native Trade Posts - START
	if (kProfession.getNativeTradeRate() > 0)
	{
		int iModifier = kUnit.getNativeTradeRateModifier();
		if (iModifier > 0)
		{
			iProModifiers += iModifier;
		}
		else
		{
			iConModifiers += iModifier / 8;
		}
	}
	// WTP, ray, Native Trade Posts - EMD

	if (kProfession.getWorkRate() > 0)
	{
		int iModifier = kUnit.getWorkRate();
		if (iModifier > 0)
		{
			iProModifiers += iModifier;
		}
		else
		{
			iConModifiers += iModifier / 8;
		}
	}

	if (kProfession.isScout())
	{
		if (kUnit.isNoBadGoodies())
		{
			iProModifiers += 100;
		}
		else
		{
			iConModifiers += 5;
		}
	}

	for (int i = 0; i < GC.getNumPromotionInfos(); ++i)
	{
		if (kUnit.getFreePromotions(i))
		{
			if (kProfession.isUnarmed())
			{
				iConModifiers += 25;
			}
			else
			{
				iProModifiers += 25;
			}
		}
	}

	if (eProfession != (ProfessionTypes)GC.getCivilizationInfo(getCivilizationType()).getDefaultProfession())
	{
		if (kUnit.getDefaultProfession() == eProfession)
		{
			iProModifiers = std::max(100, iProModifiers);//Just in case.
		}
		else
		{
			if (hasContentsYieldEquipmentAmount((ProfessionTypes)kUnit.getDefaultProfession())) // cache CvPlayer::getYieldEquipmentAmount - Nightinggale
			{
				for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
				{
					if (getYieldEquipmentAmount((ProfessionTypes)kUnit.getDefaultProfession(), (YieldTypes) iYield) > 0)
					{
						if (getYieldEquipmentAmount(eProfession, (YieldTypes) iYield) == 0)
						{
							iConModifiers += 50;
						}
						break;
					}
				}
			}
		}
	}

	iValue *= 100 + iProModifiers;
	iValue /= 100 + iConModifiers;

	return iValue;
}

int CvPlayerAI::AI_professionSuitability(const CvUnit* pUnit, ProfessionTypes eProfession, const CvPlot* pPlot, UnitAITypes eUnitAI) const
{
	CvPlot* pCityPlot = NULL;
 	if (pPlot != NULL)
	{
		CvCity* pCity = pPlot->getPlotCity();
		if (pCity == NULL)
		{
			pCity = pPlot->getWorkingCity();
		}

		if (pCity != NULL)
		{
			pCityPlot = pCity->plot();
		}
	}

	if (!pUnit->canHaveProfession(eProfession, true, pCityPlot))
	{
		return 0;
	}

	int iValue = AI_professionSuitability(pUnit->getUnitType(), eProfession);

	if (eUnitAI != NO_UNITAI && pUnit != NULL)
	{
		int iPromotionCount = 0;
		for (int i = 0; i < GC.getNumPromotionInfos(); ++i)
		{
			PromotionTypes eLoopPromotion = (PromotionTypes)i;

			if (pUnit->isHasPromotion(eLoopPromotion))
			{
				iPromotionCount ++;
			}
		}
		iValue *= 100 + 5 * iPromotionCount;
		iValue /= 100;

		if (eUnitAI == UNITAI_OFFENSIVE)
		{
			iValue *= 100 + pUnit->cityAttackModifier();
			iValue /= 100;

			if (pUnit->isProfessionalMilitary())
			{
				iValue *= 200;
				iValue /= 100;
			}
		}
		else if (eUnitAI == UNITAI_DEFENSIVE)
		{
			iValue *= 100 + pUnit->cityDefenseModifier();
			iValue /= 100;
		
			if (pUnit->isProfessionalMilitary())
			{
				iValue *= 200;
				iValue /= 100;
			}
		}
	}

	if (pPlot == NULL)
	{
		return iValue;
	}

	CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);
	CvUnitInfo& kUnit = GC.getUnitInfo(pUnit->getUnitType());

	CvCity* pCity = pPlot->getPlotCity();
	if (pCity == NULL)
	{
		pCity = pPlot->getWorkingCity();
	}

	if (pCity == NULL || pCity->getOwnerINLINE() != getID())
	{
		return iValue;
	}

	bool bMismatchedBonus = false;
	int iExtraValue = 0;
	if (kProfession.isCitizen())
	{
		// R&R, ray , MYCP partially based on code of Aymerick - START
		YieldTypes eYieldProducedType = (YieldTypes)kProfession.getYieldsProduced(0);
		// R&R, ray , MYCP partially based on code of Aymerick - END
		FAssert(eYieldProducedType != NO_YIELD);

		if (kProfession.isWorkPlot())
		{
			if (pPlot->getWorkingCity() == pCity)
			{
				if (kProfession.isWater() == pPlot->isWater())
				{
					if (pPlot->getBonusType() != NO_BONUS)
					{
						int iBonusYield = GC.getBonusInfo(pPlot->getBonusType()).getYieldChange(eYieldProducedType);
						if (iBonusYield > 0)
						{
							int iPlotYield = pPlot->calculateNatureYield(eYieldProducedType, getTeam());
							if (iPlotYield > 0)
							{
								int iExtraYield = kUnit.getYieldChange(eYieldProducedType);
								iExtraYield += kUnit.getBonusYieldChange(eYieldProducedType);

								iExtraValue += (100 * iExtraYield) / iPlotYield;
							}
						}
						else
						{
							bMismatchedBonus = true;
						}
					}
				}
			}
		}
		else
		{
			if (pCity != NULL)
			{
				int iModifier = kUnit.getYieldModifier(eYieldProducedType);
				iModifier += kUnit.getYieldChange(eYieldProducedType) * 10;

				iModifier *= pCity->AI_getYieldAdvantage(eYieldProducedType);
				iModifier /= 100;

				iExtraValue += iModifier;
			}
		}
	}

	iValue += iExtraValue;
	if (bMismatchedBonus)
	{
		iValue *= 95;
		iValue /= 100;
	}

	if (eUnitAI == UNITAI_OFFENSIVE || eUnitAI == UNITAI_DEFENSIVE || eUnitAI == UNITAI_COUNTER)
	{
		const CvUnitInfo& kUnitInfo = pUnit->getUnitInfo();

		// For combat units, we penalize the use of valuable units
		if (abs(kUnitInfo.getEuropeCost()) > 1000)
		{
			iValue *= 50;
			iValue /= 100;
		}

		// Erik: If the unit is generally capable of escaping, we prefer it for a combat units since
		// it will prevent it from escaping
		if (kUnitInfo.LbD_canEscape())
		{
			iValue *= 150;
			iValue /= 100;
		}
	}

	return iValue;
}

void CvPlayerAI::AI_swapUnitJobs(CvUnit* pUnitA, CvUnit* pUnitB)
{
	FAssert(pUnitA->plot() == pUnitB->plot());

	UnitAITypes eUnitAI_A = pUnitA->AI_getUnitAIType();
	ProfessionTypes eProfession_A = pUnitA->getProfession();
	int iMovePriorityA = pUnitA->AI_getMovePriority();

	UnitAITypes eUnitAI_B = pUnitB->AI_getUnitAIType();
	ProfessionTypes eProfession_B = pUnitB->getProfession();
	int iMovePriorityB = pUnitB->AI_getMovePriority();

	CvProfessionInfo& kProfessionA = GC.getProfessionInfo(eProfession_A);
	CvProfessionInfo& kProfessionB = GC.getProfessionInfo(eProfession_B);

	CvCity* pCity = getPopulationUnitCity(pUnitA->getID());
	if (pCity == NULL)
	{
		FAssert(pUnitA->isOnMap());
		pCity = pUnitA->plot()->getPlotCity();
	}
	FAssert(pCity != NULL);

	ProfessionTypes eDefaultProfession = (ProfessionTypes) GC.getCivilizationInfo(getCivilizationType()).getDefaultProfession();

	//Ensure all units are added to city.
	if (pUnitA->isOnMap())
	{
		pCity->addPopulationUnit(pUnitA, NO_PROFESSION);
	}
	else
	{
		pUnitA->setProfession(NO_PROFESSION);
	}

	if (pUnitB->isOnMap())
	{
		pCity->addPopulationUnit(pUnitB, NO_PROFESSION);
	}
	else
	{
		pUnitB->setProfession(NO_PROFESSION);
	}

	if (kProfessionA.isCitizen())
	{
		pUnitB->setProfession(eProfession_A);
	}
	else
	{
		pCity->removePopulationUnit(pUnitB, false, eProfession_A);
		pUnitB->AI_setUnitAIType(eUnitAI_A);
		pUnitB->AI_setMovePriority(iMovePriorityA);
	}

	if (kProfessionB.isCitizen())
	{
		pUnitA->setProfession(eProfession_B);
	}
	else
	{
		pCity->removePopulationUnit(pUnitA, false, eProfession_B);
		pUnitA->AI_setMovePriority(iMovePriorityB);
		pUnitA->AI_setUnitAIType(eUnitAI_B);
	}
}

int CvPlayerAI::AI_sumAttackerStrength(CvPlot* pPlot, CvPlot* pAttackedPlot, int iRange, DomainTypes eDomainType, bool bCheckCanAttack, bool bCheckCanMove)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int	strSum = 0;

	for (int iX = -iRange; iX <= iRange; ++iX)
	{
		for (int iY = -iRange; iY <= iRange; ++iY)
		{
			CvPlot* pLoopPlot = plotXY(pPlot->getX_INLINE(), pPlot->getY_INLINE(), iX, iY);
			if (pLoopPlot != NULL)
			{
				pUnitNode = pLoopPlot->headUnitNode();

				while (pUnitNode != NULL)
				{
					pLoopUnit = ::getUnit(pUnitNode->m_data);
					pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

					if (pLoopUnit->getOwnerINLINE() == getID())
					{
						if (!pLoopUnit->isDead())
						{
							bool bCanAttack = pLoopUnit->canAttack();

							if (!bCheckCanAttack || bCanAttack)
							{
								if (!bCheckCanMove || pLoopUnit->canMove())
									if (!bCheckCanMove || pAttackedPlot == NULL || pLoopUnit->canMoveInto(pAttackedPlot, /*bAttack*/ true, /*bDeclareWar*/ true))
										if (eDomainType == NO_DOMAIN || pLoopUnit->getDomainType() == eDomainType)
											strSum += pLoopUnit->currEffectiveStr(pAttackedPlot, pLoopUnit);
							}
						}
					}
				}
			}
		}
	}

	return strSum;
}

int CvPlayerAI::AI_sumEnemyStrength(CvPlot* pPlot, int iRange, bool bAttack, DomainTypes eDomainType)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int	strSum = 0;

	for (int iX = -iRange; iX <= iRange; ++iX)
	{
		for (int iY = -iRange; iY <= iRange; ++iY)
		{
			CvPlot* pLoopPlot = plotXY(pPlot->getX_INLINE(), pPlot->getY_INLINE(), iX, iY);
			if (pLoopPlot != NULL)
			{
				pUnitNode = pLoopPlot->headUnitNode();

				while (pUnitNode != NULL)
				{
					pLoopUnit = ::getUnit(pUnitNode->m_data);
					pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

					if (pLoopUnit->isEnemy(getTeam(), pLoopPlot))
					{
						if (!pLoopUnit->isDead())
						{
							if (eDomainType == NO_DOMAIN || pLoopUnit->getDomainType() == eDomainType)
							{
								strSum += pLoopUnit->currEffectiveStr(pLoopPlot, bAttack ? pLoopUnit : NULL);
							}
						}
					}
				}
			}
		}
	}

	return strSum;
}

int CvPlayerAI::AI_setUnitAIStatesRange(CvPlot* pPlot, int iRange, UnitAIStates eNewUnitAIState, UnitAIStates eValidUnitAIState, const std::vector<UnitAITypes>& validUnitAITypes)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int iCount = 0;

	for (int iX = -iRange; iX <= iRange; ++iX)
	{
		for (int iY = -iRange; iY <= iRange; ++iY)
		{
			CvPlot* pLoopPlot = plotXY(pPlot->getX_INLINE(), pPlot->getY_INLINE(), iX, iY);
			if (pLoopPlot != NULL)
			{
				pUnitNode = pLoopPlot->headUnitNode();

				while (pUnitNode != NULL)
				{
					pLoopUnit = ::getUnit(pUnitNode->m_data);
					pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

					if ((eValidUnitAIState == NO_UNITAI_STATE) || (pLoopUnit->AI_getUnitAIState() == eValidUnitAIState))
					{
						if (std::find(validUnitAITypes.begin(), validUnitAITypes.end(), pLoopUnit->AI_getUnitAIType()) != validUnitAITypes.end())
						{
							pLoopUnit->AI_setUnitAIState(eNewUnitAIState);
							iCount++;
						}
					}
				}
			}
		}
	}
	return iCount;
}

//
// read object from a stream
// used during load
//
void CvPlayerAI::read(FDataStreamBase* pStream)
{
	CvPlayer::read(pStream);	// read base class data first

	uint uiFlag=0;
	pStream->Read(&uiFlag);	// flags for expansion

	if (uiFlag > 0)
	{
		uint iSize;
		pStream->Read(&iSize);
		if (iSize > 0)
		{
			m_distanceMap.resize(iSize);
			pStream->Read(iSize, &m_distanceMap[0]);
		}

		pStream->Read(&m_iDistanceMapDistance);
	}

	pStream->Read(&m_iAttackOddsChange);
	pStream->Read(&m_iExtraGoldTarget);

	pStream->Read(&m_iAveragesCacheTurn);

	pStream->Read((int*)&m_eNextBuyUnit);
	pStream->Read((int*)&m_eNextBuyUnitAI);
	pStream->Read(&m_iNextBuyUnitValue);
	pStream->Read((int*)&m_eNextBuyProfession);
	pStream->Read((int*)&m_eNextBuyProfessionUnit);
	pStream->Read((int*)&m_eNextBuyProfessionAI);
	pStream->Read(&m_iNextBuyProfessionValue);

	pStream->Read(&m_iTotalIncome);
	pStream->Read(&m_iHurrySpending);
	// TAC - AI More Immigrants - koma13 - START
	pStream->Read(&m_iImmigrantSpending);
	// TAC - AI More Immigrants - koma13 - END
	pStream->Read(&m_iEuropeYieldSpending);	// TAC - AI Economy - koma13

	pStream->Read(NUM_YIELD_TYPES, m_aiAverageYieldMultiplier);
	pStream->Read(NUM_YIELD_TYPES, m_aiBestWorkedYieldPlots);
	pStream->Read(NUM_YIELD_TYPES, m_aiBestUnworkedYieldPlots);
	pStream->Read(NUM_YIELD_TYPES, m_aiYieldValuesTimes100);

	pStream->Read(&m_iUpgradeUnitsCacheTurn);
	pStream->Read(&m_iUpgradeUnitsCachedExpThreshold);
	pStream->Read(&m_iUpgradeUnitsCachedGold);

	pStream->Read(NUM_UNITAI_TYPES, m_aiNumTrainAIUnits);
	pStream->Read(NUM_UNITAI_TYPES, m_aiNumAIUnits);
	pStream->Read(NUM_UNITAI_TYPES, m_aiNumRetiredAIUnits);
	pStream->Read(NUM_UNITAI_TYPES, m_aiUnitAIStrategyWeights);
	pStream->Read(MAX_PLAYERS, m_aiPeacetimeTradeValue);
	pStream->Read(MAX_PLAYERS, m_aiPeacetimeGrantValue);
	pStream->Read(MAX_PLAYERS, m_aiGoldTradedTo);
	pStream->Read(MAX_PLAYERS, m_aiAttitudeExtra);

	pStream->Read(MAX_PLAYERS, m_abFirstContact);

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		pStream->Read(NUM_CONTACT_TYPES, m_aaiContactTimer[i]);
	}
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		pStream->Read(uiFlag > 1 ? NUM_MEMORY_TYPES : NUM_MEMORY_TYPES - 1, m_aaiMemoryCount[i]);
	}

	pStream->Read(&m_iTurnLastProductionDirty);
	pStream->Read(&m_iTurnLastManagedPop);
	pStream->Read(&m_iMoveQueuePasses);

	// TAC - AI Revolution - koma13 - START
	pStream->Read(&m_iLastWave);
	pStream->Read(&m_iWaveIndex);
	// TAC - AI Revolution - koma13 - END

	{
		m_aiAICitySites.clear();
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			int iCitySite;
			pStream->Read(&iCitySite);
			m_aiAICitySites.push_back(iCitySite);
		}
	}

	if (uiFlag > 0)
	{
		uint iSize;
		pStream->Read(&iSize);
		if (iSize > 0)
		{
			m_unitPriorityHeap.resize(iSize);
			pStream->Read(iSize, &m_unitPriorityHeap[0]);
		}
	}

	pStream->Read(GC.getNumUnitClassInfos(), m_aiUnitClassWeights);
	pStream->Read(GC.getNumUnitCombatInfos(), m_aiUnitCombatWeights);
	pStream->Read(MAX_PLAYERS, m_aiCloseBordersAttitudeCache);
	pStream->Read(MAX_PLAYERS, m_aiStolenPlotsAttitudeCache);
	pStream->Read(NUM_EMOTION_TYPES, m_aiEmotions);
	pStream->Read(NUM_STRATEGY_TYPES, m_aiStrategyStartedTurn);
	pStream->Read(NUM_STRATEGY_TYPES, m_aiStrategyData);

	/// post load function - start - Nightinggale
	if (m_eID == (MAX_PLAYERS - 1))
	{
		// Done loading the last player
		// Execute the post load code
		postLoadGameFixes();
	}
	/// post load function - end - Nightinggale
}


//
// save object to a stream
// used during save
//
void CvPlayerAI::write(FDataStreamBase* pStream)
{
	CvPlayer::write(pStream);	// write base class data first

	uint uiFlag=2;
	pStream->Write(uiFlag);		// flag for expansion

	pStream->Write(m_distanceMap.size());
	if (!m_distanceMap.empty())
	{
		pStream->Write(m_distanceMap.size(), &m_distanceMap[0]);
	}
	pStream->Write(m_iDistanceMapDistance);

	pStream->Write(m_iAttackOddsChange);
	pStream->Write(m_iExtraGoldTarget);

	pStream->Write(m_iAveragesCacheTurn);

	pStream->Write(m_eNextBuyUnit);
	pStream->Write(m_eNextBuyUnitAI);
	pStream->Write(m_iNextBuyUnitValue);
	pStream->Write(m_eNextBuyProfession);
	pStream->Write(m_eNextBuyProfessionUnit);
	pStream->Write(m_eNextBuyProfessionAI);
	pStream->Write(m_iNextBuyProfessionValue);

	pStream->Write(m_iTotalIncome);
	pStream->Write(m_iHurrySpending);
	// TAC - AI More Immigrants - koma13 - START
	pStream->Write(m_iImmigrantSpending);
	// TAC - AI More Immigrants - koma13 - END
	pStream->Write(m_iEuropeYieldSpending);	// TAC - AI Economy - koma13

	pStream->Write(NUM_YIELD_TYPES, m_aiAverageYieldMultiplier);
	pStream->Write(NUM_YIELD_TYPES, m_aiBestWorkedYieldPlots);
	pStream->Write(NUM_YIELD_TYPES, m_aiBestUnworkedYieldPlots);
	pStream->Write(NUM_YIELD_TYPES, m_aiYieldValuesTimes100);

	pStream->Write(m_iUpgradeUnitsCacheTurn);
	pStream->Write(m_iUpgradeUnitsCachedExpThreshold);
	pStream->Write(m_iUpgradeUnitsCachedGold);

	pStream->Write(NUM_UNITAI_TYPES, m_aiNumTrainAIUnits);
	pStream->Write(NUM_UNITAI_TYPES, m_aiNumAIUnits);
	pStream->Write(NUM_UNITAI_TYPES, m_aiNumRetiredAIUnits);
	pStream->Write(NUM_UNITAI_TYPES, m_aiUnitAIStrategyWeights);
	pStream->Write(MAX_PLAYERS, m_aiPeacetimeTradeValue);
	pStream->Write(MAX_PLAYERS, m_aiPeacetimeGrantValue);
	pStream->Write(MAX_PLAYERS, m_aiGoldTradedTo);
	pStream->Write(MAX_PLAYERS, m_aiAttitudeExtra);

	pStream->Write(MAX_PLAYERS, m_abFirstContact);

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		pStream->Write(NUM_CONTACT_TYPES, m_aaiContactTimer[i]);
	}
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		pStream->Write(NUM_MEMORY_TYPES, m_aaiMemoryCount[i]);
	}

	pStream->Write(m_iTurnLastProductionDirty);
	pStream->Write(m_iTurnLastManagedPop);
	pStream->Write(m_iMoveQueuePasses);

	// TAC - AI Revolution - koma13 - START
	pStream->Write(m_iLastWave);
	pStream->Write(m_iWaveIndex);
	// TAC - AI Revolution - koma13 - END

	{
		uint iSize = m_aiAICitySites.size();
		pStream->Write(iSize);
		for (std::vector<int>::iterator it = m_aiAICitySites.begin(); it != m_aiAICitySites.end(); ++it)
		{
			pStream->Write((*it));
		}
	}

	pStream->Write(m_unitPriorityHeap.size());
	if (!m_unitPriorityHeap.empty())
	{
		pStream->Write(m_unitPriorityHeap.size(), &m_unitPriorityHeap[0]);
	}

	pStream->Write(GC.getNumUnitClassInfos(), m_aiUnitClassWeights);
	pStream->Write(GC.getNumUnitCombatInfos(), m_aiUnitCombatWeights);
	pStream->Write(MAX_PLAYERS, m_aiCloseBordersAttitudeCache);
	pStream->Write(MAX_PLAYERS, m_aiStolenPlotsAttitudeCache);
	pStream->Write(NUM_EMOTION_TYPES, m_aiEmotions);
	pStream->Write(NUM_STRATEGY_TYPES, m_aiStrategyStartedTurn);
	pStream->Write(NUM_STRATEGY_TYPES, m_aiStrategyData);
}


int CvPlayerAI::AI_eventValue(EventTypes eEvent, const EventTriggeredData& kTriggeredData)
{
	CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(kTriggeredData.m_eTrigger);
	CvEventInfo& kEvent = GC.getEventInfo(eEvent);

	int iNumCities = getNumCities();
	CvCity* pCity = getCity(kTriggeredData.m_iCityId);
	CvPlot* pPlot = GC.getMapINLINE().plot(kTriggeredData.m_iPlotX, kTriggeredData.m_iPlotY);
	CvUnit* pUnit = getUnit(kTriggeredData.m_iUnitId);

	int aiYields[NUM_YIELD_TYPES];

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		aiYields[iI] = 0;
	}

	if (NO_PLAYER != kTriggeredData.m_eOtherPlayer)
	{
		if (kEvent.isDeclareWar())
		{
			switch (AI_getAttitude(kTriggeredData.m_eOtherPlayer))
			{
			case ATTITUDE_FURIOUS:
			case ATTITUDE_ANNOYED:
			case ATTITUDE_CAUTIOUS:
				if (GET_TEAM(getTeam()).getDefensivePower() < GET_TEAM(GET_PLAYER(kTriggeredData.m_eOtherPlayer).getTeam()).getPower())
				{
					return -MAX_INT + 1;
				}
				break;
			case ATTITUDE_PLEASED:
			case ATTITUDE_FRIENDLY:
				return -MAX_INT + 1;
				break;
			}
		}
	}

	//Proportional to #turns in the game...
	//(AI evaluation will generally assume proper game speed scaling!)
	int iGameSpeedPercent = 100;

	int iValue = GC.getGameINLINE().getSorenRandNum(kEvent.getAIValue(), "AI Event choice");
	iValue += (getEventCost(eEvent, kTriggeredData.m_eOtherPlayer, false) + getEventCost(eEvent, kTriggeredData.m_eOtherPlayer, true)) / 2;
	if (kEvent.getUnitClass() != NO_UNITCLASS)
	{
		UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(kEvent.getUnitClass());
		if (eUnit != NO_UNIT)
		{
			//Although AI_unitValue compares well within units, the value is somewhat independent of cost
			int iUnitValue = 0;
			for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
			{
				iUnitValue += GC.getUnitInfo(eUnit).getYieldCost(iYield);
			}
			if (iUnitValue > 0)
			{
				iUnitValue *= 2;
			}
			else
			{
				iUnitValue = 200;
			}

			iUnitValue *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
			iValue += kEvent.getNumUnits() * iUnitValue;
		}
	}

	if (kEvent.isDisbandUnit())
	{
		CvUnit* pUnit = getUnit(kTriggeredData.m_iUnitId);
		if (NULL != pUnit)
		{
			int iUnitValue = 0;
			for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
			{
				iUnitValue += pUnit->getUnitInfo().getYieldCost(iYield);
			}
			if (iUnitValue > 0)
			{
				iUnitValue *= 2;
			}
			else
			{
				iUnitValue = 200;
			}

			iUnitValue *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
			iValue -= iUnitValue;
		}
	}

	if (kEvent.getBuildingClass() != NO_BUILDINGCLASS)
	{
		BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(kEvent.getBuildingClass());
		if (eBuilding != NO_BUILDING)
		{
			if (pCity)
			{
				//iValue += kEvent.getBuildingChange() * pCity->AI_buildingValue(eBuilding);
				int iBuildingValue = 0;
				for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
				{
					iBuildingValue += GC.getBuildingInfo(eBuilding).getYieldCost(iYield);
				}
				if (iBuildingValue > 0)
				{
					iBuildingValue *= 2;
				}
				else if (iBuildingValue == -1)
				{
					iBuildingValue = 300;
				}

				iBuildingValue *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getConstructPercent();
				iValue += kEvent.getBuildingChange() * iBuildingValue;
			}
		}
	}

	{	//Yield and other changes
		if (kEvent.getNumBuildingYieldChanges() > 0)
		{
			for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); ++iBuildingClass)
			{
				for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
				{
					aiYields[iYield] += kEvent.getBuildingYieldChange(iBuildingClass, iYield);
				}
			}
		}
	}

	if (kEvent.isCityEffect())
	{
		int iCityPopulation = -1;
		int iCityTurnValue = 0;
		if (NULL != pCity)
		{
			iCityPopulation = pCity->getPopulation();
		}

		if (-1 == iCityPopulation)
		{
			//What is going on here?
			iCityPopulation = 5;
		}

		iCityTurnValue += aiYields[YIELD_FOOD] * 5;

		iCityTurnValue += aiYields[YIELD_BELLS] * 3;
		iCityTurnValue += aiYields[YIELD_CROSSES] * 1;

		iValue += (iCityTurnValue * 20 * iGameSpeedPercent) / 100;

		iValue += kEvent.getFood();
		iValue += kEvent.getFoodPercent() / 4;
		iValue += kEvent.getPopulationChange() * 30;
		iValue -= kEvent.getRevoltTurns() * (12 + iCityPopulation * 16);
		iValue += kEvent.getCulture() / 2;
		iValue += kEvent.getHealth() / 2; // R&R, ray, change for Health in Events
	}
	else if (!kEvent.isOtherPlayerCityEffect())
	{
		int iPerTurnValue = 0;

		iValue += (iPerTurnValue * 20 * iGameSpeedPercent) / 100;

		iValue += (kEvent.getFood() * iNumCities);
		iValue += (kEvent.getFoodPercent() * iNumCities) / 4;
		iValue += (kEvent.getPopulationChange() * iNumCities * 40);
		iValue += iNumCities * kEvent.getCulture() / 2;
		iValue += iNumCities * kEvent.getHealth() / 2; // R&R, ray, change for Health in Events
	}

	if (NULL != pPlot)
	{
		if (kEvent.getImprovementChange() > 0)
		{
			iValue += (30 * iGameSpeedPercent) / 100;
		}
		else if (kEvent.getImprovementChange() < 0)
		{
			iValue -= (30 * iGameSpeedPercent) / 100;
		}

		if (kEvent.getRouteChange() > 0)
		{
			iValue += (10 * iGameSpeedPercent) / 100;
		}
		else if (kEvent.getRouteChange() < 0)
		{
			iValue -= (10 * iGameSpeedPercent) / 100;
		}

		for (int i = 0; i < NUM_YIELD_TYPES; ++i)
		{
			if (0 != kEvent.getPlotExtraYield(i))
			{
				if (pPlot->getWorkingCity() != NULL)
				{
					FAssertMsg(pPlot->getWorkingCity()->getOwner() == getID(), "Event creates a boni for another player?");
					aiYields[i] += kEvent.getPlotExtraYield(i);
				}
				else
				{
					iValue += (20 * 8 * kEvent.getPlotExtraYield(i) * iGameSpeedPercent) / 100;
				}
			}
		}
	}

	if (NULL != pUnit)
	{
		iValue += (2 * pUnit->baseCombatStr() * kEvent.getUnitExperience() * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent()) / 100;

		iValue -= 10 * kEvent.getUnitImmobileTurns();
	}

	{
		int iPromotionValue = 0;

		for (int i = 0; i < GC.getNumUnitCombatInfos(); ++i)
		{
			if (NO_PROMOTION != kEvent.getUnitCombatPromotion(i))
			{
				int iLoop;
				for (CvUnit* pLoopUnit = firstUnit(&iLoop); NULL != pLoopUnit; pLoopUnit = nextUnit(&iLoop))
				{
					if (pLoopUnit->getUnitCombatType() == i)
					{
						if (!pLoopUnit->isHasPromotion((PromotionTypes)kEvent.getUnitCombatPromotion(i)))
						{
							iPromotionValue += 5 * pLoopUnit->baseCombatStr();
						}
					}
				}

				iPromotionValue += iNumCities * 50;
			}
		}

		iValue += (iPromotionValue * iGameSpeedPercent) / 100;
	}

	int iOtherPlayerAttitudeWeight = 0;
	if (kTriggeredData.m_eOtherPlayer != NO_PLAYER)
	{
		iOtherPlayerAttitudeWeight = AI_getAttitudeWeight(kTriggeredData.m_eOtherPlayer);
		iOtherPlayerAttitudeWeight += 10 - GC.getGame().getSorenRandNum(20, "AI event value attitude");
	}

	if (NO_PLAYER != kTriggeredData.m_eOtherPlayer)
	{
		CvPlayerAI& kOtherPlayer = GET_PLAYER(kTriggeredData.m_eOtherPlayer);

		int iDiploValue = 0;
		//if we like this player then value positive attitude, if however we really hate them then
		//actually value negative attitude.
		iDiploValue += ((iOtherPlayerAttitudeWeight + 50) * kEvent.getAttitudeModifier() * GET_PLAYER(kTriggeredData.m_eOtherPlayer).getPower()) / std::max(1, getPower());

		if (kEvent.getTheirEnemyAttitudeModifier() != 0)
		{
			//Oh wow this sure is mildly complicated.
			TeamTypes eWorstEnemy = GET_TEAM(GET_PLAYER(kTriggeredData.m_eOtherPlayer).getTeam()).AI_getWorstEnemy();

			if (NO_TEAM != eWorstEnemy && eWorstEnemy != getTeam())
			{
			int iThirdPartyAttitudeWeight = GET_TEAM(getTeam()).AI_getAttitudeWeight(eWorstEnemy);

			//If we like both teams, we want them to get along.
			//If we like otherPlayer but not enemy (or vice-verca), we don't want them to get along.
			//If we don't like either, we don't want them to get along.
			//Also just value stirring up trouble in general.

			int iThirdPartyDiploValue = 50 * kEvent.getTheirEnemyAttitudeModifier();
			iThirdPartyDiploValue *= (iThirdPartyAttitudeWeight - 10);
			iThirdPartyDiploValue *= (iOtherPlayerAttitudeWeight - 10);
			iThirdPartyDiploValue /= 10000;

			if ((iOtherPlayerAttitudeWeight) < 0 && (iThirdPartyAttitudeWeight < 0))
			{
				iThirdPartyDiploValue *= -1;
			}

			iThirdPartyDiploValue /= 2;

			iDiploValue += iThirdPartyDiploValue;
		}
		}

		iDiploValue *= iGameSpeedPercent;
		iDiploValue /= 100;

		if (GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI))
		{
			//What is this "relationships" thing?
			iDiploValue /= 2;
		}

		if (kEvent.isGoldToPlayer())
		{
			//If the gold goes to another player instead of the void, then this is a positive
			//thing if we like the player, otherwise it's a negative thing.
			int iGiftValue = (getEventCost(eEvent, kTriggeredData.m_eOtherPlayer, false) + getEventCost(eEvent, kTriggeredData.m_eOtherPlayer, true)) / 2;
			iGiftValue *= -iOtherPlayerAttitudeWeight;
			iGiftValue /= 110;

			iValue += iGiftValue;
		}

		if (kEvent.isDeclareWar())
		{
			int iWarValue = (GET_TEAM(getTeam()).getDefensivePower() - GET_TEAM(GET_PLAYER(kTriggeredData.m_eOtherPlayer).getTeam()).getPower());// / std::max(1, GET_TEAM(getTeam()).getDefensivePower());
			iWarValue -= 30 * AI_getAttitudeVal(kTriggeredData.m_eOtherPlayer);
		}

		if (kEvent.getMaxPillage() > 0)
		{
			int iPillageValue = (40 * (kEvent.getMinPillage() + kEvent.getMaxPillage())) / 2;
			//If we hate them, this is good to do.
			iPillageValue *= 25 - iOtherPlayerAttitudeWeight;
			iPillageValue *= iGameSpeedPercent;
			iPillageValue /= 12500;
		}

		iValue += (iDiploValue * iGameSpeedPercent) / 100;
	}

	int iThisEventValue = iValue;
	//XXX THIS IS VULNERABLE TO NON-TRIVIAL RECURSIONS!
	//Event A effects Event B, Event B effects Event A
	for (int iEvent = 0; iEvent < GC.getNumEventInfos(); ++iEvent)
	{
		if (kEvent.getAdditionalEventChance(iEvent) > 0)
		{
			if (iEvent == eEvent)
			{
				//Infinite recursion is not our friend.
				//Fortunately we have the event value for this event - sans values of other events
				//disabled or cleared. Hopefully no events will be that complicated...
				//Double the value since it's recursive.
				iValue += (kEvent.getAdditionalEventChance(iEvent) * iThisEventValue) / 50;
			}
			else
			{
				iValue += (kEvent.getAdditionalEventChance(iEvent) * AI_eventValue((EventTypes)iEvent, kTriggeredData)) / 100;
			}
		}

		if (kEvent.getClearEventChance(iEvent) > 0)
		{
			if (iEvent == eEvent)
			{
				iValue -= (kEvent.getClearEventChance(iEvent) * iThisEventValue) / 50;
			}
			else
			{
				iValue -= (kEvent.getClearEventChance(iEvent) * AI_eventValue((EventTypes)iEvent, kTriggeredData)) / 100;
			}
		}
	}

	iValue *= 100 + GC.getGameINLINE().getSorenRandNum(20, "AI Event choice");
	iValue /= 100;

	return iValue;
}

EventTypes CvPlayerAI::AI_chooseEvent(int iTriggeredId)
{
	EventTriggeredData* pTriggeredData = getEventTriggered(iTriggeredId);
	if (NULL == pTriggeredData)
	{
		return NO_EVENT;
	}

	CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(pTriggeredData->m_eTrigger);

	int iBestValue = -MAX_INT;
	EventTypes eBestEvent = NO_EVENT;

	for (int i = 0; i < kTrigger.getNumEvents(); i++)
	{
		int iValue = -MAX_INT;
		if (kTrigger.getEvent(i) != NO_EVENT)
		{
			CvEventInfo& kEvent = GC.getEventInfo((EventTypes)kTrigger.getEvent(i));
			if (canDoEvent((EventTypes)kTrigger.getEvent(i), *pTriggeredData))
			{
				iValue = AI_eventValue((EventTypes)kTrigger.getEvent(i), *pTriggeredData);
			}
		}

		if (iValue > iBestValue)
		{
			iBestValue = iValue;
			eBestEvent = (EventTypes)kTrigger.getEvent(i);
		}
	}

	return eBestEvent;
}

void CvPlayerAI::AI_doNativeArmy(TeamTypes eTeam)
{
	CvTeamAI& kTeam = GET_TEAM(getTeam());
	FAssert(eTeam != NO_TEAM && eTeam != getTeam());

	int iTotalUnitCount = getTotalPopulation();

	int iGameTurn = GC.getGameINLINE().getGameTurn();
	int iEndTurn = GC.getGameINLINE().getEstimateEndTurn();
	
	iEndTurn *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getNativePacifismPercent();
	iEndTurn /= 100;

	int iOffensivePercent = std::max(40, 30 * iGameTurn * 2 / iEndTurn);
	int iCounterPercent = std::max(25, 15 * iGameTurn * 3 / iEndTurn);

	if (kTeam.AI_getWarPlan(eTeam) == WARPLAN_TOTAL || kTeam.AI_getWarPlan(eTeam) == WARPLAN_PREPARING_TOTAL)
	{
		iOffensivePercent *= 3;
		iOffensivePercent /= 2;
	}
	else if (kTeam.AI_getWarPlan(eTeam) == WARPLAN_ATTACKED_RECENT)
	{
		iOffensivePercent += 3;
		iOffensivePercent /= 4;
	}
	else
	{
		iCounterPercent += (iOffensivePercent * 1 + 2) / 3;
		iOffensivePercent = (iOffensivePercent * 2 + 1) / 3;
	}

	//First convert units which are already on the map.
	int iLoop;
	CvUnit* pLoopUnit;
	for (pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		CvArea* pArea = pLoopUnit->area();
		AreaAITypes eAreaAI = pArea->getAreaAIType(getTeam());
		if (pLoopUnit->AI_getUnitAIType() == UNITAI_DEFENSIVE)
		{
			int iValue = 0;

			int iAreaPopulation = pArea->getPopulationPerPlayer(getID()) + pArea->getUnitsPerPlayer(getID());

			CvCity* pLoopCity = pLoopUnit->plot()->getPlotCity();
			if (pLoopCity != NULL && pLoopCity->getOwnerINLINE() != getID())
			{
				pLoopCity = NULL;
			}
			if (pLoopCity == NULL || pLoopCity->AI_isDefended())
			{
				int iOffenseValue = iOffensivePercent * (iOffensivePercent - 100 * pArea->getNumAIUnits(getID(), UNITAI_OFFENSIVE) / iAreaPopulation);
				int iCounterValue = iCounterPercent * (iCounterPercent - 100 * pArea->getNumAIUnits(getID(), UNITAI_COUNTER) / iAreaPopulation);

				if (iOffenseValue >= 0 && iOffenseValue >= iCounterValue)
				{
					pLoopUnit->AI_setUnitAIType(UNITAI_OFFENSIVE);
				}
				else if (iCounterValue > 0)
				{
					pLoopUnit->AI_setUnitAIType(UNITAI_COUNTER);
				}
			}
		}
		else if (eAreaAI == AREAAI_NEUTRAL)
		{
			pLoopUnit->AI_setUnitAIType(UNITAI_DEFENSIVE);
		}
	}

	int iBestValue = 0;
	CvCity* pBestCity = NULL;

	int iInfiniteLoop = 0;
	while (true)
	{
		int iBestValue = 0;
		UnitAITypes eBestUnitAI = NO_UNITAI;
		CvCity* pBestCity = NULL;


		CvCity* pLoopCity;
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			if (pLoopCity->getPopulation() > 1)
			{
				CvArea* pArea = pLoopCity->area();
				AreaAITypes eAreaAI = pArea->getAreaAIType(getTeam());

				if (eAreaAI != AREAAI_NEUTRAL)
				{
					int iAreaPopulation = pArea->getPopulationPerPlayer(getID()) + pArea->getUnitsPerPlayer(getID());
					int iOffenseValue = iOffensivePercent * (iOffensivePercent - 100 * pArea->getNumAIUnits(getID(), UNITAI_OFFENSIVE) / iAreaPopulation);
					int iCounterValue = iCounterPercent * (iCounterPercent - 100 * pArea->getNumAIUnits(getID(), UNITAI_COUNTER) / iAreaPopulation);

					if (iOffenseValue > 0 || iCounterValue > 0)
					{
						int iValue = (100 * pLoopCity->getPopulation()) / pLoopCity->getHighestPopulation();

						iValue /= (3 + kTeam.AI_enemyCityDistance(pLoopCity->plot()));

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							pBestCity = pLoopCity;
							eBestUnitAI = (iOffenseValue > iCounterValue) ? UNITAI_OFFENSIVE : UNITAI_COUNTER;
						}
					}
				}
			}
		}

		if (pBestCity == NULL)
		{
			break;
		}
		if (iInfiniteLoop > 100)
		{
			FAssertMsg(false, "Infinite Loop in Native War Preperations");
			break;
		}
		CvUnit* pEjectUnit = pBestCity->AI_bestPopulationUnit(eBestUnitAI);
		if (pEjectUnit == NULL)
		{
			FAssertMsg(false, "Could not eject unit");
			break;
		}
	}
}

CvCity* CvPlayerAI::AI_getPrimaryCity()
{
	int iLoop;
	CvCity* pCity = firstCity(&iLoop);
	if (pCity != NULL)
	{
		return pCity;
	}
	return NULL;

}

int CvPlayerAI::AI_getOverpopulationPercent()
{
	int iCityPopulation = getTotalPopulation() - getNumUnits();

	if (iCityPopulation <= 0)
	{
		return 0;
	}

	int iTargetPop = (iCityPopulation + getNumCities()) * (100 + 100 / GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAITrainPercent());
	iTargetPop /= 100;

	return (100 * getNumUnits() / std::max(1, iTargetPop) - 100);
}

int CvPlayerAI::AI_countNumHomedUnits(CvCity* pCity, UnitAITypes eUnitAI, UnitAIStates eUnitAIState)
{
	int iCount = 0;

	int iLoop;
	for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		if (pCity == NULL || pLoopUnit->getHomeCity() == pCity)
		{
			if (eUnitAI == NO_UNITAI || pLoopUnit->AI_getUnitAIType() == eUnitAI)
			{
				if (eUnitAIState == NO_UNITAI_STATE || pLoopUnit->AI_getUnitAIState() == eUnitAIState)
				{
					iCount++;
				}
			}
		}
	}

	return iCount;
}

// R&R, ray, Natives raiding party - START
bool CvPlayerAI::AI_isPotentialRaidTarget(PlayerTypes ePlayer)
{
	if (!isNative())
	{
		return false;
	}

	if (GET_PLAYER(ePlayer).isNative())
	{
		return false;
	}

	if (AI_getAttitude(ePlayer) > GC.getLeaderHeadInfo(getPersonalityType()).getNativeRaidAttitudeThreshold())
	{
		return false;
	}

	return true;
}

bool CvPlayerAI::AI_hasPotentialRaidTarget()
{
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		if (AI_isPotentialRaidTarget((PlayerTypes)iPlayer))
		{
			return true;
		}
	}

	return false;
}

int CvPlayerAI::AI_getNumPotentialRaids()
{
	int iCount = 0;

	if (isNative())
	{
		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
			if (AI_isPotentialRaidTarget(kLoopPlayer.getID()))
			{
				if (!kLoopPlayer.isNative())
				{
					int iAttitudeVal = AI_getAttitudeVal(kLoopPlayer.getID());
					iAttitudeVal += GC.getNATIVE_POTENTIAL_RAID_TARGET_THRESHOLD();

					if (iAttitudeVal < 0)
					{
						iCount += -(iAttitudeVal);
					}
				}
			}
		}
	}

	return iCount;
}
// R&R, ray, Natives raiding party - END

void CvPlayerAI::AI_doMilitaryStrategy()
{

	//Iterate over every enemy city.
	//If we are sieging that city then evaluate the strength of our units vs the strength of the local defense (blah blah blah)

	//If we are in a strong position : CHARGE
	//If we are waiting for reinforcements: CAMP
	//If we are weak and no reinforcements are coming: RETREAT

	std::vector<UnitAITypes> militaryUnitAIs;

	militaryUnitAIs.push_back(UNITAI_COUNTER);
	militaryUnitAIs.push_back(UNITAI_OFFENSIVE);

	CvTeamAI& kTeam = GET_TEAM(getTeam());
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		WarPlanTypes eWarPlan = kTeam.AI_getWarPlan(kLoopPlayer.getTeam());
		if (kLoopPlayer.isAlive() && (kLoopPlayer.getTeam() != getTeam()) && (eWarPlan != NO_WARPLAN))
		{
			bool bKnockKnock_WhoseThere_Monty_MontyWho_MontyAndHisHordeDieDieDie = false;
			int iLoop;
			CvCity* pLoopCity;
			for (pLoopCity = kLoopPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kLoopPlayer.nextCity(&iLoop))
			{
				if (pLoopCity->plot()->isVisible(getTeam(), false) || isNative())
				{
					int iOurStrength = AI_sumAttackerStrength(pLoopCity->plot(), pLoopCity->plot(), 3, DOMAIN_LAND);
					if ((iOurStrength > 0) && (GC.getGameINLINE().getSorenRandNum(100, "AI new wave") < 25))
					{
						//Only consider enemy strength for total war, otherwise dribble on in.
						int iEnemyStrength = (eWarPlan == WARPLAN_TOTAL) ? AI_sumEnemyStrength(pLoopCity->plot(), 1, false, DOMAIN_LAND) : 0;
						if ((iOurStrength * 100) > iEnemyStrength * 150)
						{
							AI_setUnitAIStatesRange(pLoopCity->plot(), 3, UNITAI_STATE_CHARGING, UNITAI_STATE_GROUPING, militaryUnitAIs);
							bKnockKnock_WhoseThere_Monty_MontyWho_MontyAndHisHordeDieDieDie = true;
						}
					}
				}
			}

			if (bKnockKnock_WhoseThere_Monty_MontyWho_MontyAndHisHordeDieDieDie)
			{
				if (!atWar(getTeam(), kLoopPlayer.getTeam()))
				{
					WarPlanTypes eNewWarplan = WARPLAN_TOTAL;

					//Do the whole extortion thing
					kTeam.declareWar(kLoopPlayer.getTeam(), true, eNewWarplan);
				}
			}
		}
	}
	return;
}

void CvPlayerAI::AI_doSuppressRevolution()
{
	bool bContinue = false;
	PlayerTypes eColony = NO_PLAYER;
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (kLoopPlayer.isAlive())
		{
			if (GET_TEAM(getTeam()).isParentOf(kLoopPlayer.getTeam()))
			{
				if (atWar(getTeam(), kLoopPlayer.getTeam()))
				{
					eColony = (PlayerTypes)iPlayer;
					bContinue = true;
					break;
				}
			}
		}
	}

	if (!bContinue)
	{
		return;
	}

	CvPlayerAI& kColony = GET_PLAYER(eColony);

	if (!AI_isAnyStrategy())
	{
		AI_setStrategy(STRATEGY_SMALL_WAVES);

		int iTactics = GC.getGameINLINE().getSorenRandNum(5, "AI Choose Strategy");
		switch (iTactics)
		{
			case 0:
			case 1:
			case 2:
				AI_setStrategy(STRATEGY_CONCENTRATED_ATTACK);
				break;
			case 3:
			case 4:
				AI_setStrategy(STRATEGY_DISTRIBUTED_ATTACK);
				break;
			default:
				break;
		}
	}

	if (GC.getGameINLINE().getSorenRandNum(100, "AI change King Strategy") < 33)
	{
		if (AI_isStrategy(STRATEGY_CONCENTRATED_ATTACK) && (AI_getStrategyDuration(STRATEGY_CONCENTRATED_ATTACK) > 7))
		{
			AI_clearStrategy(STRATEGY_CONCENTRATED_ATTACK);

			AI_setStrategy(STRATEGY_DISTRIBUTED_ATTACK);
		}
		else if (AI_isStrategy(STRATEGY_DISTRIBUTED_ATTACK) && (AI_getStrategyDuration(STRATEGY_DISTRIBUTED_ATTACK) > 4))
		{
			AI_clearStrategy(STRATEGY_DISTRIBUTED_ATTACK);

			AI_setStrategy(STRATEGY_CONCENTRATED_ATTACK);
		}
	}

	if (AI_isStrategy(STRATEGY_CONCENTRATED_ATTACK) && AI_getStrategyData(STRATEGY_CONCENTRATED_ATTACK) == -1)
	{
		int iBestValue = 0;
		CvPlot* pBestPlot = NULL;
		//Select a target city.
		int iLoop;
		CvCity* pLoopCity;
		for (pLoopCity = kColony.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kColony.nextCity(&iLoop))
		{
			if (pLoopCity->plot()->getNearestEurope() != NO_EUROPE)
			{
				int iValue = pLoopCity->getHighestPopulation() * 50 + pLoopCity->plot()->getCrumbs();

				iValue *= 25 + GC.getGameINLINE().getSorenRandNum(75, "AI choose target for concentrated attack");
				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = pLoopCity->plot();
				}
			}
		}
		if (pBestPlot != NULL)
		{
			AI_setStrategy(STRATEGY_CONCENTRATED_ATTACK, GC.getMap().plotNum(pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE()));
		}
	}

	int iShipCount = 0;
	int iSoldierCount = 0;
	int iCargoSpace = 0;

	std::vector<CvUnit*> ships;
	std::vector<CvUnit*> soldiers;

	int iLoop;
	for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit->getDomainType() == DOMAIN_SEA)
		{
			if (pLoopUnit->getUnitTravelState() == UNIT_TRAVEL_STATE_IN_EUROPE)
			{
				ships.push_back(pLoopUnit);
				iShipCount++;
				iCargoSpace += pLoopUnit->cargoSpace();
			}
	    }
	}

	std::vector<int> shuffle(getNumEuropeUnits());
	for (int i = 0; i < getNumEuropeUnits(); ++i)
	{
		shuffle[i] = i;
	}
	GC.getGameINLINE().getSorenRand().shuffleArray(shuffle, NULL);

	for (int i = 0; i < getNumEuropeUnits(); ++i)
	{
		CvUnit* pLoopUnit = getEuropeUnit(shuffle[i]);
		FAssert(pLoopUnit != NULL);
        if (pLoopUnit->getDomainType() == DOMAIN_LAND)
		{
			soldiers.push_back(pLoopUnit);
			iSoldierCount++;
		}
	}

	if (iShipCount == 0)
	{
		//FAssertMsg(iSoldierCount == 0, "Uh oh, soldiers stuck in europe");
		return;
	}

	// TAC - AI Revolution - koma13 - START
	if (AI_getLastWave() == -1)
	{
		AI_setLastWave(0);
		AI_setWaveIndex(0);

		for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
		{
			CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
			if (pLoopPlot != NULL)
			{
				pLoopPlot->setRevealed(getTeam(), true, false, NO_TEAM);
			}
		}
	}
		
	int iWaveIndex = AI_getWaveIndex();
	int iWaveTurns = GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getWaveTurns();

	if (AI_getLastWave() > 0)
	{
		if (AI_getLastWave() < iWaveTurns)
		{
			return;
		}
	}
	// TAC - AI Revolution - koma13 - END

	int iTotalShipCount = AI_getNumAIUnits(UNITAI_COMBAT_SEA);

	int iShipsToLaunch = 0;

	if (AI_isStrategy(STRATEGY_SMALL_WAVES))//Set at start of revolution.
	{
		// TAC - AI Revolution - koma13 - START
		//iShipsToLaunch = (iTotalShipCount + 9) / 10;
		
		int iShipsLaunchedPercent = 0;
		
		for (int i = 0; i < iWaveIndex; ++i)
		{
			iShipsLaunchedPercent += GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getWaves(i);
		}

		// R&R, ray fix for CTD
		if(iWaveIndex + 1 == GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getNumWaves() || iShipsLaunchedPercent > 99)
		{
			iShipsToLaunch = iShipCount;
		}
		else
		{
			int iShipCount100Percent = iShipCount * 100 / (100 - iShipsLaunchedPercent);
			int iWave = GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getWaves(iWaveIndex);
		
			iShipsToLaunch = (iShipCount100Percent * iWave / 100);
		}
		// TAC - AI Revolution - koma13 - END
	}
	
	// TAC - AI Revolution - koma13 - START
	AI_setWaveIndex(iWaveIndex + 1);
	AI_setLastWave(0);
	
	/*
	else if (AI_isStrategy(STRATEGY_BUILDUP))//Set when first ship gets back.
	{
		if (iShipCount > (iTotalShipCount / 2))
		{
			iShipsToLaunch = (iTotalShipCount + 2) / 3;

			AI_clearStrategy(STRATEGY_BUILDUP);
			AI_setStrategy(STRATEGY_SMALL_WAVES);
		}
	}

	int iMinWaveSize = 3;

	if (iShipCount < iMinWaveSize)
	{
		if (iTotalShipCount >= iMinWaveSize)
		{
			return;
		}
	}

	iShipsToLaunch = std::max(iShipsToLaunch, iMinWaveSize);
	*/
	// TAC - AI Revolution - koma13 - END
	iShipsToLaunch = std::min(iShipsToLaunch, iShipCount);


	int iSoldiersToLoad = 0;

	int iMaxCargo = iCargoSpace * iShipsToLaunch / iShipCount;
	if (iSoldierCount < iMaxCargo)
	{
		iSoldiersToLoad = iSoldierCount;
	}
//	TAC - AI Revolution - koma13
//	else if (iSoldierCount < iMaxCargo * 2)
//	{
//		iSoldiersToLoad = iSoldierCount / 2;
//	}
//	TAC - AI Revolution - koma13	
	else
	{
		iSoldiersToLoad = iMaxCargo;
	}

	if (iSoldiersToLoad > 0)
	{
		int iSoldiersLoaded = 0;
		for (int i = 0; i < iShipCount; ++i)
		{
			CvUnit* pLoopUnit = ships[i];
			FAssert(pLoopUnit != NULL);
			if (i < iShipsToLaunch)
			{
				while (iSoldiersLoaded < iSoldiersToLoad)
				{
					CvUnit* pSoldier = soldiers[iSoldiersLoaded];
					FAssert(pSoldier != NULL);

					iSoldiersLoaded++;
					loadUnitFromEurope(pSoldier, pLoopUnit);
					if (pLoopUnit->isFull())
					{
						break;
					}
				}

				CvPlot* pTargetPlot = AI_getImperialShipSpawnPlot();

				if (!pLoopUnit->atPlot(pTargetPlot))
				{
					pLoopUnit->setXY(pTargetPlot->getX_INLINE(), pTargetPlot->getY_INLINE(), false, false, false);
				}


				pLoopUnit->crossOcean(UNIT_TRAVEL_STATE_FROM_EUROPE);
			}
		}
	}
	else if (iSoldierCount == 0)
	{
		//Lets wander around the New World!
		for (int i = 0; i < iShipCount; ++i)
		{
			CvUnit* pLoopUnit = ships[i];
			FAssert(pLoopUnit != NULL);

			CvPlot* pTargetPlot = AI_getImperialShipSpawnPlot();

			if (!pLoopUnit->atPlot(pTargetPlot))
			{
				pLoopUnit->setXY(pTargetPlot->getX_INLINE(), pTargetPlot->getY_INLINE(), false, false, false);
			}

			pLoopUnit->crossOcean(UNIT_TRAVEL_STATE_FROM_EUROPE);
		}
	}

}

void CvPlayerAI::AI_doUnitAIWeights()
{
	if ((GC.getGame().getGameTurn() == 2) || (GC.getGameINLINE().getSorenRandNum(50, "AI do Unit AI Weight Calculations") == 0))
	for (int i = 0; i < NUM_UNITAI_TYPES; ++i)
	{
		int iWeight = 90 + GC.getGameINLINE().getSorenRandNum(20, "AI Unit AI Weights");

		m_aiUnitAIStrategyWeights[i] = iWeight;
	}
}

void CvPlayerAI::AI_doEmotions()
{
	CvMap& kMap = GC.getMap();

	std::vector<short> const &distanceMap = *AI_getDistanceMap();

	int iGreedValue = 0;
	int iAnxietyValue = 0;
	int iAngerValue = 0;
	int iEnvyValue = 0;

	for (int i = 0; i < kMap.numPlotsINLINE(); ++i)
	{
		CvPlot* pLoopPlot = kMap.plotByIndexINLINE(i);
		if (distanceMap[i] != -1)
		{
			if (!pLoopPlot->isOwned())
			{
				if (pLoopPlot->getBonusType() != NO_BONUS)
				{
					if (pLoopPlot->isWater())
					{
						iGreedValue += 1;
					}
					else
					{
						iGreedValue += 2;
					}
				}
			}
			if (pLoopPlot->isVisible(getTeam(), false))
			{
				int iEnemyUnits = 0;
				int iFriendlyUnits = 0;
				int iNeutralColonialUnits = 0;
				int iTreasureUnits = 0;


				CLLNode<IDInfo>* pUnitNode;
				CvUnit* pLoopUnit;
				pUnitNode = pLoopPlot->headUnitNode();

				while (pUnitNode != NULL)
				{
					pLoopUnit = ::getUnit(pUnitNode->m_data);
					pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

					if (pLoopUnit->getTeam() == getTeam())
					{
						iFriendlyUnits++;
					}
					else if (pLoopUnit->isEnemy(getTeam(), pLoopPlot))
					{
						iEnemyUnits++;
					}
					else
					{
						if (!pLoopUnit->isNative())
						{
							iNeutralColonialUnits++;
						}
					}

					if (pLoopUnit->getTeam() != getTeam())
					{
						if ((pLoopUnit->getUnitInfo().isTreasure()) && pLoopUnit->getYieldStored() > 0)
						{
							iEnvyValue += pLoopUnit->getYieldStored() / 40;
						}
					}

					if (pLoopPlot->getOwnerINLINE() == getID())
					{
						iAnxietyValue += 5 * iEnemyUnits;
						if (isNative())
						{
							iAngerValue += 3 * iNeutralColonialUnits;
						}
					}
					else
					{
						if (isNative())
						{
							iAngerValue += iNeutralColonialUnits;
						}
						iAngerValue += 1 * iEnemyUnits;
						iAnxietyValue += 1 * iEnemyUnits;
					}
				}
			}
		}
	}
	iGreedValue /= 2;
	AI_changeEmotion(EMOTION_GREED, iGreedValue);
	AI_changeEmotion(EMOTION_ANXIETY, iAnxietyValue);
	AI_changeEmotion(EMOTION_ANGER, iAngerValue);

	//Anxiety
	int iLoop;
	CvCity* pLoopCity;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (!pLoopCity->AI_isDefended())
		{
			AI_changeEmotion(EMOTION_ANXIETY, pLoopCity->getPopulation());
		}
	}
}

void CvPlayerAI::AI_doStrategy()
{
	int iGameTurn = GC.getGameINLINE().getGameTurn();
	CvTeamAI& kTeam = GET_TEAM(getTeam());

	int iPercent = 10000 / GC.getGame().AI_adjustedTurn(100);;

	if (getParent() != NO_PLAYER)
	{
		if (iGameTurn == 1)
		{
			AI_setStrategy(STRATEGY_CASH_FOCUS);
			//AI_setStrategy(STRATEGY_SELL_TO_NATIVES);	// TAC - AI Economy - koma13
		}

		if (iGameTurn == GC.getGameINLINE().AI_adjustedTurn(10))
		{
			//Set initial strategies.
			if (GC.getGame().getSorenRandNum(100, "AI Fast Bells") < 30)
			{
				AI_setStrategy(STRATEGY_FAST_BELLS);
			}

			// TAC - AI City Sites - koma13 - START
			/*
			if (GC.getGameINLINE().getSorenRandNum(100, "AI Dense Spacing") < 50)
			{
				AI_setStrategy(STRATEGY_DENSE_CITY_SPACING);	
			}
			*/
			// TAC - AI City Sites - koma13 - END
		}

		if (!AI_isStrategy(STRATEGY_REVOLUTION_PREPARING))
		{
			if (!AI_isStrategy(STRATEGY_FAST_BELLS) && GC.getGameINLINE().getSorenRandNum(10000, "AI Fast Bells") < (2 * iPercent))
			{
				AI_setStrategy(STRATEGY_FAST_BELLS);
			}

			if (iGameTurn > GC.getGameINLINE().AI_adjustedTurn(35))
			{
				if (AI_isStrategy(STRATEGY_CASH_FOCUS) && GC.getGameINLINE().getSorenRandNum(10000, "AI Cash Focus") < (3 * iPercent))
				{
					AI_clearStrategy(STRATEGY_CASH_FOCUS);
				}
			}

			// TAC - AI Economy - koma13 - START
			/*
			if (!AI_isStrategy(STRATEGY_SELL_TO_NATIVES) && GC.getGameINLINE().getSorenRandNum(10000, "AI Sell to Natives") < (6 * iPercent))
			{
				AI_setStrategy(STRATEGY_SELL_TO_NATIVES);
			}
			*/
			// TAC - AI Economy - koma13 - END

			// TAC - AI City Sites - koma13 - START
			/*
			if (GC.getGameINLINE().getSorenRandNum(10000, "AI toggle spacing strategy") < (4 * iPercent))
			{
				if (AI_isStrategy(STRATEGY_DENSE_CITY_SPACING))
				{
					AI_clearStrategy(STRATEGY_DENSE_CITY_SPACING);
				}
				else
				{
					AI_setStrategy(STRATEGY_DENSE_CITY_SPACING);
				}
			}
			*/
			// TAC - AI City Sites - koma13 - END
		}
		
		// TAC - AI Military Buildup - koma13 - START
		if (AI_isStrategy(STRATEGY_MILITARY_BUILDUP))
		{
			if ((AI_bestBuildupUnitAI() == NO_UNITAI) || !canTradeWithEurope())
			{
				AI_clearStrategy(STRATEGY_MILITARY_BUILDUP);
			}
		}
		// TAC - AI Military Buildup - koma13 - END

		if (iGameTurn > GC.getGameINLINE().AI_adjustedTurn(100))
		{
			
			int iProb = 3 * iPercent;
			int iRebelPercent = GET_TEAM(getTeam()).getRebelPercent();
			iProb *= (100 + iRebelPercent + ((200 * iGameTurn) / GC.getGameINLINE().getEstimateEndTurn()));
			iProb /= 100;
			
			// TAC - AI Revolution - koma13 - START
			int iRebelsNeeded = GC.getAI_REBELS_FOR_REVOLUTION();
			
			if (iRebelPercent < iRebelsNeeded)
			{
				iProb = 0;
			}
			
			if (!AI_isStrategy(STRATEGY_REVOLUTION_PREPARING))
			{
				if (GC.getGameINLINE().getSorenRandNum(10000, "AI Start Revolution") < iProb)
				{
					AI_setStrategy(STRATEGY_REVOLUTION_PREPARING);
					AI_clearStrategy(STRATEGY_FAST_BELLS);
				}
			}
			/*
			else if (!AI_isStrategy(STRATEGY_REVOLUTION_DECLARING))
			{
				if ((iRebelPercent > 70) &&  (AI_getStrategyDuration(STRATEGY_REVOLUTION_PREPARING) > GC.getGameINLINE().AI_adjustedTurn(20)))
				{
					if (GC.getGameINLINE().getSorenRandNum(10000, "AI Start Revolution") < iProb * 6)
					{
						AI_setStrategy(STRATEGY_REVOLUTION_DECLARING);
					}
				}
			}
			else
			{
				FAssert(AI_isStrategy(STRATEGY_REVOLUTION_DECLARING));
				if (AI_getStrategyDuration(STRATEGY_REVOLUTION_DECLARING) > GC.getGameINLINE().AI_adjustedTurn(20))
				{
					int iValue = iRebelPercent + 100 * AI_getStrategyDuration(STRATEGY_REVOLUTION_DECLARING) / GC.getGameINLINE().AI_adjustedTurn(50);
					
					if (iValue > 125)
					{
						if (AI_totalDefendersNeeded(NULL) - AI_totalUnitAIs(UNITAI_OFFENSIVE) - AI_totalUnitAIs(UNITAI_COUNTER) - getNumCities() <= 0)
						{
							if (kTeam.canDoRevolution())
							{
								kTeam.doRevolution();
								AI_setStrategy(STRATEGY_REVOLUTION);
							}
						}
					}
				}
			}
			*/

			else if (!AI_isStrategy(STRATEGY_REVOLUTION_DECLARING))
			{
				if ((iRebelPercent > iRebelsNeeded) &&  (AI_getStrategyDuration(STRATEGY_REVOLUTION_PREPARING) > GC.getGameINLINE().AI_adjustedTurn(20)))
				{
					if (getEuropeMilitary() < NBMOD_GetColonialMilitaryValue())
					{
						AI_setStrategy(STRATEGY_REVOLUTION_DECLARING);
						//AI_clearStrategy(STRATEGY_GET_A_SHIP);	// TAC - AI Purchasing military units - koma13
					}
				}
			}
			else
			{
				FAssert(AI_isStrategy(STRATEGY_REVOLUTION_DECLARING));
				if (AI_getStrategyDuration(STRATEGY_REVOLUTION_DECLARING) > GC.getGameINLINE().AI_adjustedTurn(20))
				{
					int iValue = iRebelPercent + 100 * AI_getStrategyDuration(STRATEGY_REVOLUTION_DECLARING) / GC.getGameINLINE().AI_adjustedTurn(50);

					if (iValue > 125)
					{
						// We have to take into account the determined trait, promotions, bonus vs. the king, ability to
						// keep producing troops / weapons etc. The AI can probably declare with a significantly smaller army than the king and still win.
						if (getEuropeMilitary() < NBMOD_GetColonialMilitaryValue())
						{
							// Erik: This is wrong, if we have any plans, just cancel them and declare instead.
							// it's too late to try to conquer someone. If we're are war with indians we can
							// just choose a civic that gives instant peace.				
							bool bAtWar = (GET_TEAM(getTeam()).getAnyWarPlanCount() > 0);
							if (!bAtWar)
							{
								if (kTeam.canDoRevolution())
								{
									kTeam.doRevolution();
									AI_setStrategy(STRATEGY_REVOLUTION);
								}
							}
						}
					}
				}
			}
			// TAC - AI Revolution - koma13 - END
		}
	}

	if (isNative())
	{
		if (AI_isStrategy(STRATEGY_DIE_FIGHTING))
		{
			if (AI_getNumAIUnits(UNITAI_OFFENSIVE) == 0)
			{
				AI_clearStrategy(STRATEGY_DIE_FIGHTING);
			}
		}
		else if (kTeam.getAnyWarPlanCount() > 0)
		{
			for (int i = 0; i < MAX_PLAYERS; ++i)
			{
				PlayerTypes eLoopPlayer = (PlayerTypes)i;
				if (GET_PLAYER(eLoopPlayer).isAlive())
				{
					TeamTypes eLoopTeam = GET_PLAYER(eLoopPlayer).getTeam();
					if (kTeam.isAtWar((TeamTypes)i))
					{
						WarPlanTypes eWarPlan = kTeam.AI_getWarPlan(eLoopTeam);
						int iDuration = kTeam.AI_getWarPlanStateCounter(eLoopTeam);

						bool bRazedCity = AI_getMemoryAttitude(eLoopPlayer, MEMORY_RAZED_CITY) > 0;

						if (eWarPlan == WARPLAN_ATTACKED || eWarPlan == WARPLAN_TOTAL)
						{
							AI_setStrategy(STRATEGY_DIE_FIGHTING);
						}
					}
				}
			}
		}
	}
}

int CvPlayerAI::AI_countDeadlockedBonuses(CvPlot* pPlot)
{
    CvPlot* pLoopPlot;
    CvPlot* pLoopPlot2;
    int iDX, iDY;
    int iI;

    int iMinRange = GC.getMIN_CITY_RANGE();
    int iRange = iMinRange * 2;
    int iCount = 0;

    for (iDX = -(iRange); iDX <= iRange; iDX++)
    {
        for (iDY = -(iRange); iDY <= iRange; iDY++)
        {
            if (plotDistance(iDX, iDY, 0, 0) > CITY_PLOTS_RADIUS)
            {
                pLoopPlot = plotXY(pPlot->getX_INLINE(), pPlot->getY_INLINE(), iDX, iDY);

                if (pLoopPlot != NULL)
                {
                    if (pLoopPlot->getBonusType() != NO_BONUS)
                    {
                        if (!pLoopPlot->isCityRadius() && ((pLoopPlot->area() == pPlot->area()) || pLoopPlot->isWater()))
                        {
                            bool bCanFound = false;
                            bool bNeverFound = true;
                            //potentially blockable resource
                            //look for a city site within a city radius
                            for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
                            {
                                pLoopPlot2 = plotCity(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), iI);
                                if (pLoopPlot2 != NULL)
                                {
                                    //canFound usually returns very quickly
                                    if (canFound(pLoopPlot2->getX_INLINE(), pLoopPlot2->getY_INLINE(), false))
                                    {
                                        bNeverFound = false;
                                        if (stepDistance(pPlot->getX_INLINE(), pPlot->getY_INLINE(), pLoopPlot2->getX_INLINE(), pLoopPlot2->getY_INLINE()) > iMinRange)
                                        {
                                            bCanFound = true;
                                            break;
                                        }
                                    }
                                }
                            }
                            if (!bNeverFound && !bCanFound)
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

int CvPlayerAI::AI_getOurPlotStrength(CvPlot* pPlot, int iRange, bool bDefensiveBonuses, bool bTestMoves) const
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pLoopPlot;
	int iValue;
	int iDistance;
	int iDX, iDY;

	iValue = 0;

	for (iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for (iDY = -(iRange); iDY <= iRange; iDY++)
		{
			pLoopPlot = plotXY(pPlot->getX_INLINE(), pPlot->getY_INLINE(), iDX, iDY);

			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->area() == pPlot->area())
				{
				    iDistance = stepDistance(pPlot->getX_INLINE(), pPlot->getY_INLINE(), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE());
					pUnitNode = pLoopPlot->headUnitNode();

					while (pUnitNode != NULL)
					{
						pLoopUnit = ::getUnit(pUnitNode->m_data);
						pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

						if (pLoopUnit->getOwnerINLINE() == getID())
						{
							if ((bDefensiveBonuses && pLoopUnit->canDefend()) || pLoopUnit->canAttack())
							{
								if (!(pLoopUnit->isInvisible(getTeam(), false)))
								{
									if (pLoopUnit->atPlot(pPlot) || pLoopUnit->canMoveInto(pPlot) || pLoopUnit->canMoveInto(pPlot, /*bAttack*/ true))
									{
										if (!bTestMoves)
										{
											iValue += pLoopUnit->currEffectiveStr((bDefensiveBonuses ? pPlot : NULL), NULL);
										}
										else
										{
											if (pLoopUnit->baseMoves() >= iDistance)
											{
												iValue += pLoopUnit->currEffectiveStr((bDefensiveBonuses ? pPlot : NULL), NULL);
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
	}


	return iValue;
}

int CvPlayerAI::AI_getEnemyPlotStrength(CvPlot* pPlot, int iRange, bool bDefensiveBonuses, bool bTestMoves)
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvPlot* pLoopPlot;
	int iValue;
	int iDistance;
	int iDX, iDY;

	iValue = 0;

	for (iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for (iDY = -(iRange); iDY <= iRange; iDY++)
		{
			pLoopPlot = plotXY(pPlot->getX_INLINE(), pPlot->getY_INLINE(), iDX, iDY);

			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->area() == pPlot->area())
				{
				    iDistance = stepDistance(pPlot->getX_INLINE(), pPlot->getY_INLINE(), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE());
					pUnitNode = pLoopPlot->headUnitNode();

					while (pUnitNode != NULL)
					{
						pLoopUnit = ::getUnit(pUnitNode->m_data);
						pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

						if (atWar(pLoopUnit->getTeam(), getTeam()))
						{
							if ((bDefensiveBonuses && pLoopUnit->canDefend()) || pLoopUnit->canAttack())
							{
								if (!(pLoopUnit->isInvisible(getTeam(), false)))
								{
								    if (pPlot->isValidDomainForAction(*pLoopUnit))
								    {
                                        if (!bTestMoves)
                                        {
                                            iValue += pLoopUnit->currEffectiveStr((bDefensiveBonuses ? pPlot : NULL), NULL);
                                        }
                                        else
                                        {
                                            int iDangerRange = pLoopUnit->baseMoves();
                                            iDangerRange += ((pLoopPlot->isValidRoute(pLoopUnit)) ? 1 : 0);
                                            if (iDangerRange >= iDistance)
                                            {
                                                iValue += pLoopUnit->currEffectiveStr((bDefensiveBonuses ? pPlot : NULL), NULL);
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
	}


	return iValue;

}

int CvPlayerAI::AI_goldToUpgradeAllUnits(int iExpThreshold)
{
	if (m_iUpgradeUnitsCacheTurn == GC.getGameINLINE().getGameTurn() && m_iUpgradeUnitsCachedExpThreshold == iExpThreshold)
	{
		return m_iUpgradeUnitsCachedGold;
	}

	int iTotalGold = 0;

	CvCivilizationInfo& kCivilizationInfo = GC.getCivilizationInfo(getCivilizationType());

	// cache the value for each unit type
	std::vector<int> aiUnitUpgradePrice(GC.getNumUnitInfos(), 0);	// initializes to zeros

	int iLoop;
	for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		// if experience is below threshold, skip this unit
		if (pLoopUnit->getExperience() < iExpThreshold)
		{
			continue;
		}

		UnitTypes eUnitType = pLoopUnit->getUnitType();

		// check cached value for this unit type
		int iCachedUnitGold = aiUnitUpgradePrice[eUnitType];
		if (iCachedUnitGold != 0)
		{
			// if positive, add it to the sum
			if (iCachedUnitGold > 0)
			{
				iTotalGold += iCachedUnitGold;
			}

			// either way, done with this unit
			continue;
		}

		int iUnitGold = 0;
		int iUnitUpgradePossibilities = 0;

		UnitAITypes eUnitAIType = pLoopUnit->AI_getUnitAIType();
		CvArea* pUnitArea = pLoopUnit->area();
		int iUnitValue = AI_unitValue(eUnitType, eUnitAIType, pUnitArea);

		for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
		{
			UnitClassTypes eUpgradeUnitClassType = (UnitClassTypes) iI;
			UnitTypes eUpgradeUnitType = (UnitTypes)(kCivilizationInfo.getCivilizationUnits(iI));

			if (NO_UNIT != eUpgradeUnitType)
			{
				// is it better?
				int iUpgradeValue = AI_unitValue(eUpgradeUnitType, eUnitAIType, pUnitArea);
				if (iUpgradeValue > iUnitValue)
				{
					// is this a valid upgrade?
					if (pLoopUnit->upgradeAvailable(eUnitType, eUpgradeUnitClassType))
					{
						// can we actually make this upgrade?
						bool bCanUpgrade = false;
						CvCity* pCapitalCity = getPrimaryCity();
						if (pCapitalCity != NULL && pCapitalCity->canTrain(eUpgradeUnitType))
						{
							bCanUpgrade = true;
						}
						else
						{
							CvCity* pCloseCity = GC.getMapINLINE().findCity(pLoopUnit->getX_INLINE(), pLoopUnit->getY_INLINE(), getID(), NO_TEAM, true, (pLoopUnit->getDomainType() == DOMAIN_SEA));
							if (pCloseCity != NULL && pCloseCity->canTrain(eUpgradeUnitType))
							{
								bCanUpgrade = true;
							}
						}

						if (bCanUpgrade)
						{
							iUnitGold += pLoopUnit->upgradePrice(eUpgradeUnitType);
							iUnitUpgradePossibilities++;
						}
					}
				}
			}
		}

		// if we found any, find average and add to total
		if (iUnitUpgradePossibilities > 0)
		{
			iUnitGold /= iUnitUpgradePossibilities;

			// add to cache
			aiUnitUpgradePrice[eUnitType] = iUnitGold;

			// add to sum
			iTotalGold += iUnitGold;
		}
		else
		{
			// add to cache, dont upgrade to this type
			aiUnitUpgradePrice[eUnitType] = -1;
		}
	}

	m_iUpgradeUnitsCacheTurn = GC.getGameINLINE().getGameTurn();
	m_iUpgradeUnitsCachedExpThreshold = iExpThreshold;
	m_iUpgradeUnitsCachedGold = iTotalGold;

	return iTotalGold;
}

int CvPlayerAI::AI_goldTradeValuePercent()
{
	return 100;
}

int CvPlayerAI::AI_playerCloseness(PlayerTypes eIndex, int iMaxDistance)
{
	PROFILE_FUNC();
	CvCity* pLoopCity;
	int iLoop;
	int iValue;

	FAssert(GET_PLAYER(eIndex).isAlive());
	FAssert(eIndex != getID());

	iValue = 0;
	for (pLoopCity = GET_PLAYER(eIndex).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eIndex).nextCity(&iLoop))
	{
		iValue += pLoopCity->AI_playerCloseness(eIndex, iMaxDistance);
	}

	return iValue;
}

int CvPlayerAI::AI_targetValidity(PlayerTypes ePlayer)
{
	FAssert(ePlayer != NO_PLAYER);

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);

	if (!kPlayer.isAlive())
	{
		return 0;
	}
	if (kPlayer.getTeam() == getTeam())
	{
		return 0;
	}

	int iAggressionRange = 10;
	if (isNative())
	{
		iAggressionRange = 5;
	}

	int iValidTargetCount = 0;
	int iTotalValue = 0;

	if (!kPlayer.isAlive())
	{
		return 0;
	}

	int iLoop;
	CvCity* pLoopCity;
	for (pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
	{
		bool bLandTarget = (pLoopCity->area()->getCitiesPerPlayer(getID()) > 0);

		if (bLandTarget || !isNative())
		{
			int iDistance = AI_cityDistance(pLoopCity->plot());
			int iAdjustedRange = iAggressionRange;
			if (!bLandTarget)
			{
				iAdjustedRange *= 2;
			}

			if (iDistance <= iAdjustedRange)
			{
				iTotalValue += 100 - (100 * (iDistance - 1)) / iAdjustedRange;
				iValidTargetCount++;
			}
		}
	}

	int iValue = (2 * iTotalValue) / (1 + kPlayer.getNumCities() + iValidTargetCount);
	
	// TAC - AI Assault Sea - koma13 - START
	if (iValue == 0 && !isNative())
	{
		iValue += AI_totalUnitAIs(UNITAI_ASSAULT_SEA);
	}
	// TAC - AI Assault Sea - koma13 - END

	return iValue;
}

// TAC - AI City Defense - koma13 - START
//int CvPlayerAI::AI_totalDefendersNeeded(int* piUndefendedCityCount)
int CvPlayerAI::AI_totalDefendersNeeded(int* piUndefendedCityCount, CvArea* pArea, bool bIncludeStacks)
{
	PROFILE_FUNC();
	CvCity* pLoopCity;
	int iLoop;
	int iValue;

	int iTotalNeeded = 0;
	int iUndefendedCount = 0;

	iValue = 0;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pArea == NULL || pArea == pLoopCity->area())
		{
			int iHave = pLoopCity->AI_numDefenders(true, true);
			int iNeeded = pLoopCity->AI_neededDefenders();
	
			if (iNeeded > 0)
			{
				if (iHave == 0)
				{
					iUndefendedCount++;
				}
				iTotalNeeded += std::max(0, iNeeded - iHave);
			}
		}
	}
	
	if (bIncludeStacks)
	{
		for(CvSelectionGroup* pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
		{
			if (pLoopSelectionGroup->getNumUnits() > 1)
			{
				if (pLoopSelectionGroup->getHeadUnitAI() == UNITAI_OFFENSIVE)
				{
					if (pArea == NULL || pArea == pLoopSelectionGroup->area())
					{
						int iStackDefenders = pLoopSelectionGroup->countNumUnitAIType(UNITAI_DEFENSIVE) + AI_unitTargetMissionAIs(pLoopSelectionGroup->getHeadUnit(), MISSIONAI_GROUP, NULL, UNITAI_DEFENSIVE);
						if (iStackDefenders < 3)
						{
							iTotalNeeded += std::max(0, 3 - iStackDefenders);
						}
					}
				}
			}
		}
	}
	// TAC - AI City Defense - koma13 - END

	if (piUndefendedCityCount != NULL)
	{
		*piUndefendedCityCount = iUndefendedCount;
	}

	return iTotalNeeded;
}


int CvPlayerAI::AI_getTotalAreaCityThreat(CvArea* pArea)
{
	PROFILE_FUNC();
	CvCity* pLoopCity;
	int iLoop;
	int iValue;

	iValue = 0;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->getArea() == pArea->getID())
		{
			iValue += pLoopCity->AI_cityThreat();
		}
	}
	return iValue;
}

int CvPlayerAI::AI_countNumAreaHostileUnits(CvArea* pArea, bool bPlayer, bool bTeam, bool bNeutral, bool bHostile)
{
	PROFILE_FUNC();
	CvPlot* pLoopPlot;
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
		if ((pLoopPlot->area() == pArea) && pLoopPlot->isVisible(getTeam(), false) &&
			((bPlayer && pLoopPlot->getOwnerINLINE() == getID()) || (bTeam && pLoopPlot->getTeam() == getTeam())
				|| (bNeutral && !pLoopPlot->isOwned()) || (bHostile && pLoopPlot->isOwned() && GET_TEAM(getTeam()).isAtWar(pLoopPlot->getTeam()))))
			{
			iCount += pLoopPlot->plotCount(PUF_isEnemy, getID(), false, NO_PLAYER, NO_TEAM, PUF_isVisible, getID());
		}
	}
	return iCount;
}

//this doesn't include the minimal one or two garrison units in each city.
int CvPlayerAI::AI_getTotalFloatingDefendersNeeded(CvArea* pArea)
{
	PROFILE_FUNC();
	int iDefenders;
	int iCurrentEra = getCurrentEra();
	int iAreaCities = pArea->getCitiesPerPlayer(getID());

	iCurrentEra = std::max(0, iCurrentEra - GC.getGame().getStartEra() / 2);

	iDefenders = 1 + ((iCurrentEra + ((GC.getGameINLINE().getMaxCityElimination() > 0) ? 3 : 2)) * iAreaCities);
	iDefenders /= 3;
	iDefenders += pArea->getPopulationPerPlayer(getID()) / 7;

	if (pArea->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE)
	{
		iDefenders *= 2;
	}
	else if ((pArea->getAreaAIType(getTeam()) == AREAAI_OFFENSIVE) || (pArea->getAreaAIType(getTeam()) == AREAAI_MASSING))
	{
		iDefenders *= 2;
		iDefenders /= 3;
	}

	if (AI_getTotalAreaCityThreat(pArea) == 0)
	{
		iDefenders /= 2;
	}

	if (!GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI))
	{
		iDefenders *= 2;
		iDefenders /= 3;
	}

	iDefenders *= 60;
	iDefenders /= std::max(30, (GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAITrainPercent() - 20));

	if (getPrimaryCity() != NULL)
	{
		if (getPrimaryCity()->area() != pArea)
		{
			//Defend offshore islands only lightly.
			iDefenders = std::min(iDefenders, iAreaCities * iAreaCities - 1);
		}
	}
	// Super Forts begin *AI_defense* - Build a few extra floating defenders for occupying forts
	iDefenders += iAreaCities / 2;
	// Super Forts end
	return iDefenders;
}

int CvPlayerAI::AI_getTotalFloatingDefenders(CvArea* pArea)
{
	PROFILE_FUNC();
	int iCount = 0;

	iCount += AI_totalAreaUnitAIs(pArea, UNITAI_DEFENSIVE);
	iCount += AI_totalAreaUnitAIs(pArea, UNITAI_OFFENSIVE);
	iCount += AI_totalAreaUnitAIs(pArea, UNITAI_COUNTER);
	return iCount / 2;
}

RouteTypes CvPlayerAI::AI_bestAdvancedStartRoute(CvPlot* pPlot, int* piYieldValue)
{
	RouteTypes eBestRoute = NO_ROUTE;
	int iBestValue = -1;
    for (int iI = 0; iI < GC.getNumRouteInfos(); iI++)
    {
        RouteTypes eRoute = (RouteTypes)iI;

		int iValue = 0;
		int iCost = getAdvancedStartRouteCost(eRoute, true, pPlot);

		if (iCost >= 0)
		{
			iValue += GC.getRouteInfo(eRoute).getValue();

			if (iValue > 0)
			{
				int iYieldValue = 0;
				if (pPlot->getImprovementType() != NO_IMPROVEMENT)
				{
					iYieldValue += ((GC.getImprovementInfo(pPlot->getImprovementType()).getRouteYieldChanges(eRoute, YIELD_FOOD)) * 100);
				}
				iValue *= 1000;
				iValue /= (1 + iCost);

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					eBestRoute = eRoute;
					if (piYieldValue != NULL)
					{
						*piYieldValue = iYieldValue;
					}
				}
			}
		}
	}
	return eBestRoute;
}

UnitTypes CvPlayerAI::AI_bestAdvancedStartUnitAI(CvPlot* pPlot, UnitAITypes eUnitAI)
{
	UnitTypes eLoopUnit;
	UnitTypes eBestUnit;
	int iValue;
	int iBestValue;
	int iI, iJ, iK;

	FAssertMsg(eUnitAI != NO_UNITAI, "UnitAI is not assigned a valid value");

	iBestValue = 0;
	eBestUnit = NO_UNIT;

	for (iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		eLoopUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI)));

		if (eLoopUnit != NO_UNIT)
		{
			if (GC.getUnitInfo(eLoopUnit).getDefaultUnitAIType() == eUnitAI)
			{
				int iUnitCost = getAdvancedStartUnitCost(eLoopUnit, true, pPlot);
				if (iUnitCost >= 0)
				{
					iValue = AI_unitValue(eLoopUnit, eUnitAI, pPlot->area());

					if (iValue > 0)
					{
						//free promotions. slow?
						//only 1 promotion per source is counted (ie protective isn't counted twice)
						int iPromotionValue = 0;

						//special to the unit
						for (iJ = 0; iJ < GC.getNumPromotionInfos(); iJ++)
						{
							if (GC.getUnitInfo(eLoopUnit).getFreePromotions(iJ))
							{
								iPromotionValue += 15;
								break;
							}
						}

						for (iK = 0; iK < GC.getNumPromotionInfos(); iK++)
						{
							if (isFreePromotion((UnitCombatTypes)GC.getUnitInfo(eLoopUnit).getUnitCombatType(), (PromotionTypes)iK))
							{
								iPromotionValue += 15;
								break;
							}

							if (isFreePromotion((UnitClassTypes)GC.getUnitInfo(eLoopUnit).getUnitClassType(), (PromotionTypes)iK))
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
									if (GC.getTraitInfo((TraitTypes) iJ).isFreePromotion(iK))
									{
										if ((GC.getUnitInfo(eLoopUnit).getUnitCombatType() != NO_UNITCOMBAT) && GC.getTraitInfo((TraitTypes) iJ).isFreePromotionUnitCombat(GC.getUnitInfo(eLoopUnit).getUnitCombatType()))
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

						iValue *= (GC.getGameINLINE().getSorenRandNum(40, "AI Best Advanced Start Unit") + 100);
						iValue /= 100;

						iValue *= (getNumCities() + 2);
						iValue /= (getUnitClassCountPlusMaking((UnitClassTypes)iI) + getNumCities() + 2);

						FAssert((MAX_INT / 1000) > iValue);
						iValue *= 1000;

						iValue /= 1 + iUnitCost;

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

CvPlot* CvPlayerAI::AI_advancedStartFindCapitalPlot()
{
	CvPlot* pBestPlot = NULL;
	int iBestValue = -1;

	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (kPlayer.isAlive())
		{
			if (kPlayer.getTeam() == getTeam())
			{
				CvPlot* pLoopPlot = kPlayer.getStartingPlot();
				if (pLoopPlot != NULL)
				{
					if (getAdvancedStartCityCost(true, pLoopPlot) > 0)
					{
					int iX = pLoopPlot->getX_INLINE();
					int iY = pLoopPlot->getY_INLINE();

						int iValue = 1000;
						if (iPlayer == getID())
						{
							iValue += 1000;
						}
						else
						{
							iValue += GC.getGame().getSorenRandNum(100, "AI Advanced Start Choose Team Start");
						}
						CvCity * pNearestCity = GC.getMapINLINE().findCity(iX, iY, NO_PLAYER, getTeam());
						if (NULL != pNearestCity)
						{
							FAssert(pNearestCity->getTeam() == getTeam());
							int iDistance = stepDistance(iX, iY, pNearestCity->getX_INLINE(), pNearestCity->getY_INLINE());
							if (iDistance < 10)
							{
								iValue /= (10 - iDistance);
							}
						}

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							pBestPlot = pLoopPlot;
						}
					}
				}
				else
				{
					FAssertMsg(false, "StartingPlot for a live player is NULL!");
				}
			}
		}
	}

	if (pBestPlot != NULL)
	{
		return pBestPlot;
	}

	FAssertMsg(false, "AS: Failed to find a starting plot for a player");

	//Execution should almost never reach here.

	//Update found values just in case - particulary important for simultaneous turns.
	AI_updateFoundValues();

	pBestPlot = NULL;
	iBestValue = -1;

	if (NULL != getStartingPlot())
	{
		for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
		{
			CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
			if (pLoopPlot->getArea() == getStartingPlot()->getArea())
			{
				int iValue = pLoopPlot->getFoundValue(getID());
				if (iValue > 0)
				{
					if (getAdvancedStartCityCost(true, pLoopPlot) > 0)
					{
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

	if (pBestPlot != NULL)
	{
		return pBestPlot;
	}

	//Commence panic.
	FAssertMsg(false, "Failed to find an advanced start starting plot");
	return NULL;
}


bool CvPlayerAI::AI_advancedStartPlaceExploreUnits(bool bLand)
{
	CvPlot* pBestExplorePlot = NULL;
	int iBestExploreValue = 0;
	UnitTypes eBestUnitType = NO_UNIT;

	UnitAITypes eUnitAI = NO_UNITAI;
	if (bLand)
	{
		eUnitAI = UNITAI_SCOUT;
	}
	else if (!bLand)
	{
		return false;
	}

	int iLoop;
	CvCity* pLoopCity;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		CvPlot* pLoopPlot = pLoopCity->plot();
		CvArea* pLoopArea = bLand ? pLoopCity->area() : pLoopPlot->waterArea();

		if (pLoopArea != NULL)
			{
			int iValue = std::max(0, pLoopArea->getNumUnrevealedTiles(getTeam()) - 10) * 10;
			iValue += std::max(0, pLoopArea->getNumTiles() - 50);

				if (iValue > 0)
				{
					int iOtherPlotCount = 0;
					int iGoodyCount = 0;
					int iExplorerCount = 0;
				int iAreaId = pLoopArea->getID();

				int iRange = 4;
					for (int iX = -iRange; iX <= iRange; iX++)
					{
						for (int iY = -iRange; iY <= iRange; iY++)
						{
							CvPlot* pLoopPlot2 = plotXY(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), iX, iY);
						if (NULL != pLoopPlot2)
							{
								iExplorerCount += pLoopPlot2->plotCount(PUF_isUnitAIType, eUnitAI, -1, NO_PLAYER, getTeam());
							if (pLoopPlot2->getArea() == iAreaId)
							{
								if (pLoopPlot2->isGoody())
								{
									iGoodyCount++;
								}
								if (pLoopPlot2->getTeam() != getTeam())
								{
									iOtherPlotCount++;
								}
							}
						}
					}
				}

					iValue -= 300 * iExplorerCount;
					iValue += 200 * iGoodyCount;
					iValue += 10 * iOtherPlotCount;
					if (iValue > iBestExploreValue)
					{
						UnitTypes eUnit = AI_bestAdvancedStartUnitAI(pLoopPlot, eUnitAI);
						if (eUnit != NO_UNIT)
						{
							eBestUnitType = eUnit;
							iBestExploreValue = iValue;
							pBestExplorePlot = pLoopPlot;
						}
					}
				}
			}
		}

	if (pBestExplorePlot != NULL)
	{
		doAdvancedStartAction(ADVANCEDSTARTACTION_UNIT, pBestExplorePlot->getX_INLINE(), pBestExplorePlot->getY_INLINE(), eBestUnitType, true);
		return true;
	}
	return false;
}

void CvPlayerAI::AI_advancedStartRevealRadius(CvPlot* pPlot, int iRadius)
{
	for (int iRange = 1; iRange <=iRadius; iRange++)
	{
		for (int iX = -iRange; iX <= iRange; iX++)
		{
			for (int iY = -iRange; iY <= iRange; iY++)
			{
				if (plotDistance(0, 0, iX, iY) <= iRadius)
				{
					CvPlot* pLoopPlot = plotXY(pPlot->getX_INLINE(), pPlot->getY_INLINE(), iX, iY);

					if (NULL != pLoopPlot)
					{
						if (getAdvancedStartVisibilityCost(true, pLoopPlot) > 0)
						{
							doAdvancedStartAction(ADVANCEDSTARTACTION_VISIBILITY, pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), -1, true);
						}
					}
				}
			}
		}
	}
}

bool CvPlayerAI::AI_advancedStartPlaceCity(CvPlot* pPlot)
{
	if (isNative())
	{
		doAdvancedStartAction(ADVANCEDSTARTACTION_CITY, pPlot->getX(), pPlot->getY(), -1, true);
		return true;
	}
	//If there is already a city, then improve it.
	CvCity* pCity = pPlot->getPlotCity();
	if (pCity == NULL)
	{
		doAdvancedStartAction(ADVANCEDSTARTACTION_CITY, pPlot->getX(), pPlot->getY(), -1, true);

		pCity = pPlot->getPlotCity();
		if ((pCity == NULL) || (pCity->getOwnerINLINE() != getID()))
		{
			//this should never happen since the cost for a city should be 0 if
			//the city can't be placed.
			//(It can happen if another player has placed a city in the fog)
			FAssertMsg(false, "ADVANCEDSTARTACTION_CITY failed in unexpected way");
			return false;
		}
	}

	/*
	if (pCity->getCultureLevel() <= 1)
	{
		doAdvancedStartAction(ADVANCEDSTARTACTION_CULTURE, pPlot->getX(), pPlot->getY(), -1, true);
	}
	*/

	//to account for culture expansion.
	pCity->AI_updateBestBuild();

	int iPlotsImproved = 0;
	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		if (iI != CITY_HOME_PLOT)
		{
			CvPlot* pLoopPlot = plotCity(pPlot->getX_INLINE(), pPlot->getY_INLINE(), iI);
			if ((pLoopPlot != NULL) && (pLoopPlot->getWorkingCity() == pCity))
			{
				if (pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
				{
					iPlotsImproved++;
				}
			}
		}
	}

	int iTargetPopulation = (getCurrentEra() / 2 + 3);

	while (iPlotsImproved < iTargetPopulation)
	{
		CvPlot* pBestPlot;
		ImprovementTypes eBestImprovement = NO_IMPROVEMENT;
		int iBestValue = 0;
		for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
			int iValue = pCity->AI_getBestBuildValue(iI);
			if (iValue > iBestValue)
			{
				BuildTypes eBuild = pCity->AI_getBestBuild(iI);
				if (eBuild != NO_BUILD)
				{
					ImprovementTypes eImprovement = (ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement();
					if (eImprovement != NO_IMPROVEMENT)
					{
						CvPlot* pLoopPlot = plotCity(pCity->getX_INLINE(), pCity->getY_INLINE(), iI);
						if ((pLoopPlot != NULL) && (pLoopPlot->getImprovementType() != eImprovement))
						{
							eBestImprovement = eImprovement;
							pBestPlot = pLoopPlot;
							iBestValue = iValue;
						}
					}
				}
			}
		}

		if (iBestValue > 0)
		{

			FAssert(pBestPlot != NULL);
			doAdvancedStartAction(ADVANCEDSTARTACTION_IMPROVEMENT, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), eBestImprovement, true);
			iPlotsImproved++;
			if (pCity->getPopulation() < iPlotsImproved)
			{
				doAdvancedStartAction(ADVANCEDSTARTACTION_POP, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), -1, true);
			}
		}
		else
		{
			break;
		}
	}


	while (iPlotsImproved > pCity->getPopulation())
	{
		int iPopCost = getAdvancedStartPopCost(true, pCity);
		if (iPopCost <= 0 || iPopCost > getAdvancedStartPoints())
		{
			break;
		}
		doAdvancedStartAction(ADVANCEDSTARTACTION_POP, pPlot->getX_INLINE(), pPlot->getY_INLINE(), -1, true);
	}

	while (iTargetPopulation > pCity->getPopulation())
	{
		int iPopCost = getAdvancedStartPopCost(true, pCity);
		if (iPopCost <= 0 || iPopCost > getAdvancedStartPoints())
		{
			break;
		}
		doAdvancedStartAction(ADVANCEDSTARTACTION_POP, pPlot->getX_INLINE(), pPlot->getY_INLINE(), -1, true);
	}

	pCity->AI_updateAssignWork();

	return true;
}




//Returns false if we have no more points.
bool CvPlayerAI::AI_advancedStartDoRoute(CvPlot* pFromPlot, CvPlot* pToPlot)
{
	FAssert(pFromPlot != NULL);
	FAssert(pToPlot != NULL);

	FAStarNode* pNode;
	gDLL->getFAStarIFace()->ForceReset(&GC.getStepFinder());
	if (gDLL->getFAStarIFace()->GeneratePath(&GC.getStepFinder(), pFromPlot->getX_INLINE(), pFromPlot->getY_INLINE(), pToPlot->getX_INLINE(), pToPlot->getY_INLINE(), false, 0, true))
	{
		pNode = gDLL->getFAStarIFace()->GetLastNode(&GC.getStepFinder());
		if (pNode != NULL)
		{
			if (pNode->m_iData1 > (1 + stepDistance(pFromPlot->getX(), pFromPlot->getY(), pToPlot->getX(), pToPlot->getY())))
			{
				//Don't build convulted paths.
				return true;
			}
		}

		while (pNode != NULL)
		{
			CvPlot* pPlot = GC.getMapINLINE().plotSorenINLINE(pNode->m_iX, pNode->m_iY);
			RouteTypes eRoute = AI_bestAdvancedStartRoute(pPlot);
			if (eRoute != NO_ROUTE)
			{
				if (getAdvancedStartRouteCost(eRoute, true, pPlot) > getAdvancedStartPoints())
				{
					return false;
				}
				doAdvancedStartAction(ADVANCEDSTARTACTION_ROUTE, pNode->m_iX, pNode->m_iY, eRoute, true);
			}
			pNode = pNode->m_pParent;
		}
	}
	return true;
}
void CvPlayerAI::AI_advancedStartRouteTerritory()
{
	CvPlot* pLoopPlot;
	int iI;

	for (iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
		if ((pLoopPlot != NULL) && (pLoopPlot->getOwner() == getID()) && (pLoopPlot->getRouteType() == NO_ROUTE))
		{
			if (pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
			{
				if (pLoopPlot->getRouteType() == NO_ROUTE)
				{
					int iRouteYieldValue = 0;
					RouteTypes eRoute = (AI_bestAdvancedStartRoute(pLoopPlot, &iRouteYieldValue));
					if (eRoute != NO_ROUTE && iRouteYieldValue > 0)
					{
						doAdvancedStartAction(ADVANCEDSTARTACTION_ROUTE, pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), eRoute, true);
					}
				}
			}
		}
	}
}


void CvPlayerAI::AI_doAdvancedStart(bool bNoExit)
{
	if (NULL == getStartingPlot())
	{
		FAssert(false);
		return;
	}

	int iTargetCityCount = GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getTargetNumCities();

	iTargetCityCount = 1 + iTargetCityCount + GC.getGameINLINE().getSorenRandNum(2 * iTargetCityCount, "AI Native Civilization Size");
	iTargetCityCount /= 2;

	int iLoop;
	CvCity* pLoopCity;
	int iStartingPoints = getAdvancedStartPoints();
	int iRevealPoints;
	int iMilitaryPoints;
	int iCityPoints;

	bool bIsNative = isNative();

	if (bIsNative)
	{
		AI_createNatives();
		if (bNoExit)
		{
			return;
		}
		else
		{
			doAdvancedStartAction(ADVANCEDSTARTACTION_EXIT, -1, -1, -1, true);
		}
	}

	if (isNative())
	{
		iRevealPoints = (iStartingPoints * 20) / 100;
		iMilitaryPoints = (iStartingPoints * 40) / 100;
		iCityPoints = iStartingPoints - (iMilitaryPoints + iRevealPoints);
	}
	else
	{
		iRevealPoints = (iStartingPoints * 10) / 100;
		iMilitaryPoints = (iStartingPoints * (isHuman() ? 17 : 20)) / 100;
		iCityPoints = iStartingPoints - (iMilitaryPoints + iRevealPoints);
	}

	if (!bIsNative)
	{

		if (getPrimaryCity() != NULL)
		{
			AI_advancedStartPlaceCity(getPrimaryCity()->plot());
		}
		else
		{
			for (int iPass = 0; iPass < 2 && NULL == getPrimaryCity(); ++iPass)
			{
				CvPlot* pBestCapitalPlot = AI_advancedStartFindCapitalPlot();

				if (pBestCapitalPlot != NULL)
				{
					if (!AI_advancedStartPlaceCity(pBestCapitalPlot))
					{
						FAssertMsg(false, "AS AI: Unexpected failure placing capital");
					}
					break;
				}
				else
				{
					//If this point is reached, the advanced start system is broken.
					//Find a new starting plot for this player
					setStartingPlot(findStartingPlot(false), true);
					//Redo Starting visibility
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
				}
			}

			if (getPrimaryCity() == NULL)
			{
				if (!bNoExit)
				{
					doAdvancedStartAction(ADVANCEDSTARTACTION_EXIT, -1, -1, -1, true);
				}
				return;
			}
		}

		iCityPoints -= (iStartingPoints - getAdvancedStartPoints());

		int iLastPointsTotal = getAdvancedStartPoints();

		for (int iPass = 0; iPass < 6; iPass++)
		{
			for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
			{
				CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
				if (pLoopPlot->isRevealed(getTeam(), false))
				{
					if (pLoopPlot->getBonusType() != NO_BONUS)
					{
						AI_advancedStartRevealRadius(pLoopPlot, CITY_PLOTS_RADIUS);
					}
					else
					{
						for (int iJ = 0; iJ < NUM_CARDINALDIRECTION_TYPES; iJ++)
						{
							CvPlot* pLoopPlot2 = plotCardinalDirection(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), (CardinalDirectionTypes)iJ);
							if ((pLoopPlot2 != NULL) && (getAdvancedStartVisibilityCost(true, pLoopPlot2) > 0))
							{
								//Mildly maphackery but any smart human can see the terrain type of a tile.
								pLoopPlot2->getTerrainType();
								int iFoodYield = GC.getTerrainInfo(pLoopPlot2->getTerrainType()).getYield(YIELD_FOOD);
								if (pLoopPlot2->getFeatureType() != NO_FEATURE)
								{
									iFoodYield += GC.getFeatureInfo(pLoopPlot2->getFeatureType()).getYieldChange(YIELD_FOOD);
								}
								if (iFoodYield >= 2 || pLoopPlot2->isHills() || pLoopPlot2->isPeak() || pLoopPlot2->isRiver())
								{
									doAdvancedStartAction(ADVANCEDSTARTACTION_VISIBILITY, pLoopPlot2->getX_INLINE(), pLoopPlot2->getY_INLINE(), -1, true);
								}
							}
						}
					}
				}
				if ((iLastPointsTotal - getAdvancedStartPoints()) > iRevealPoints)
				{
					break;
				}
			}
		}

		iLastPointsTotal = getAdvancedStartPoints();
		iCityPoints = std::min(iCityPoints, iLastPointsTotal);
		int iArea = -1; //getStartingPlot()->getArea();
		bool bDonePlacingCities = false;
		for (int iPass = 0; iPass < 100; ++iPass)
		{
			int iBestFoundValue = 0;
			CvPlot* pBestFoundPlot = NULL;
			AI_updateFoundValues(false);
			for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
			{
				CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
				if (plotDistance(getStartingPlot()->getX_INLINE(), getStartingPlot()->getY_INLINE(), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE()) < 19)
				{
					int iFoundValue = pLoopPlot->getFoundValue(getID());
					if (isNative())
					{
						iFoundValue = iFoundValue + GC.getGameINLINE().getSorenRandNum(iFoundValue * 2, "AI place native city");
					}
					if (pLoopPlot->getFoundValue(getID()) > iBestFoundValue)
					{
						if (getAdvancedStartCityCost(true, pLoopPlot) > 0)
						{
							pBestFoundPlot = pLoopPlot;
							iBestFoundValue = pLoopPlot->getFoundValue(getID());
						}
					}
				}
			}

			if (isNative())
			{
				if (getNumCities() >= iTargetCityCount)
				{
					bDonePlacingCities = true;
				}
			}
			else if (iBestFoundValue < ((getNumCities() == 0) ? 1 : (500 + 250 * getNumCities())))
			{
				bDonePlacingCities = true;
			}

			if (pBestFoundPlot == NULL)
			{
				bDonePlacingCities = true;
			}

			if (!bDonePlacingCities)
			{
				int iCost = getAdvancedStartCityCost(true, pBestFoundPlot);
				if (iCost > getAdvancedStartPoints())
				{
					bDonePlacingCities = true;
				}// at 500pts, we have 200, we spend 100.
				else if (((iLastPointsTotal - getAdvancedStartPoints()) + iCost) > iCityPoints)
				{
					bDonePlacingCities = true;
				}
			}

			if (!bDonePlacingCities)
			{
				if (!AI_advancedStartPlaceCity(pBestFoundPlot))
				{
					FAssertMsg(false, "AS AI: Failed to place city (non-capital)");
					bDonePlacingCities = true;
				}
			}

			if (bDonePlacingCities)
			{
				break;
			}
		}


		//Land
		AI_advancedStartPlaceExploreUnits(true);
		if (getCurrentEra() > 2)
		{
			//Sea
			AI_advancedStartPlaceExploreUnits(false);
		}

		if (!isNative())
		{
			AI_advancedStartRouteTerritory();
		}

		bool bDoneBuildings = (iLastPointsTotal - getAdvancedStartPoints()) > iCityPoints;
		for (int iPass = 0; iPass < 10 && !bDoneBuildings; ++iPass)
		{
			for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				BuildingTypes eBuilding = pLoopCity->AI_bestAdvancedStartBuilding(iPass);
				if (eBuilding != NO_BUILDING)
				{
					bDoneBuildings = (iLastPointsTotal - (getAdvancedStartPoints() - getAdvancedStartBuildingCost(eBuilding, true, pLoopCity))) > iCityPoints;
					if (!bDoneBuildings)
					{
						doAdvancedStartAction(ADVANCEDSTARTACTION_BUILDING, pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE(), eBuilding, true);
					}
					else
					{
						//continue there might be cheaper buildings in other cities we can afford
					}
				}
			}
		}
	}

	//Units
	std::vector<UnitAITypes> aeUnitAITypes;
	if (!isNative())
	{
		aeUnitAITypes.push_back(UNITAI_COLONIST);
	}
	else
	{
		aeUnitAITypes.push_back(UNITAI_DEFENSIVE);
	}


	bool bDone = false;
	for (int iPass = 0; iPass < 6; ++iPass)
	{
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{

			{
				if (iPass > 0)
			{
					if (getAdvancedStartPopCost(true, pLoopCity) > getAdvancedStartPoints())
					{
						bDone = true;
						break;
					}
					doAdvancedStartAction(ADVANCEDSTARTACTION_POP, pLoopCity->getX(), pLoopCity->getY(), -1, true);
				}
				CvPlot* pUnitPlot = pLoopCity->plot();
				//Token defender
				UnitTypes eBestUnit = AI_bestAdvancedStartUnitAI(pUnitPlot, aeUnitAITypes[iPass % aeUnitAITypes.size()]);
				if (eBestUnit != NO_UNIT)
				{

					if (getAdvancedStartUnitCost(eBestUnit, true, pUnitPlot) > getAdvancedStartPoints())
					{
						bDone = true;
						break;
					}
					doAdvancedStartAction(ADVANCEDSTARTACTION_UNIT, pUnitPlot->getX(), pUnitPlot->getY(), eBestUnit, true);
				}
			}
		}
	}

	if (!bNoExit)
	{
		doAdvancedStartAction(ADVANCEDSTARTACTION_EXIT, -1, -1, -1, true);
	}

}

int CvPlayerAI::AI_getMinFoundValue()
{
	return 600;
}

int CvPlayerAI::AI_bestAreaUnitAIValue(UnitAITypes eUnitAI, CvArea* pArea, UnitTypes* peBestUnitType)
{

	CvCity* pCity = NULL;

	if (pArea != NULL)
	{
	if (getPrimaryCity() != NULL)
	{
		if (pArea->isWater())
		{
			if (getPrimaryCity()->plot()->isAdjacentToArea(pArea))
			{
				pCity = getPrimaryCity();
			}
		}
		else
		{
			if (getPrimaryCity()->getArea() == pArea->getID())
			{
				pCity = getPrimaryCity();
			}
		}
	}

	if (NULL == pCity)
	{
		CvCity* pLoopCity;
		int iLoop;
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			if (pArea->isWater())
			{
				if (pLoopCity->plot()->isAdjacentToArea(pArea))
				{
					pCity = pLoopCity;
					break;
				}
			}
			else
			{
				if (pLoopCity->getArea() == pArea->getID())
				{
					pCity = pLoopCity;
					break;
				}
			}
		}
	}
	}

	return AI_bestCityUnitAIValue(eUnitAI, pCity, peBestUnitType);

}

int CvPlayerAI::AI_bestCityUnitAIValue(UnitAITypes eUnitAI, CvCity* pCity, UnitTypes* peBestUnitType)
{
	UnitTypes eLoopUnit;
	int iValue;
	int iBestValue;
	int iI;

	FAssertMsg(eUnitAI != NO_UNITAI, "UnitAI is not assigned a valid value");

	iBestValue = 0;

	for (iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		eLoopUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI)));

		if (eLoopUnit != NO_UNIT)
		{
			if (!isHuman() || (GC.getUnitInfo(eLoopUnit).getDefaultUnitAIType() == eUnitAI))
			{
				if (NULL == pCity ? canTrain(eLoopUnit) : pCity->canTrain(eLoopUnit))
				{
					iValue = AI_unitValue(eLoopUnit, eUnitAI, (pCity == NULL) ? NULL : pCity->area());
					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						if (peBestUnitType != NULL)
						{
							*peBestUnitType = eLoopUnit;
						}
					}
				}
			}
		}
	}

	return iBestValue;
}

int CvPlayerAI::AI_calculateTotalBombard(DomainTypes eDomain)
{
	int iI;
	int iTotalBombard = 0;

	for (iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		UnitTypes eLoopUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI)));
		if (eLoopUnit != NO_UNIT)
		{
			if (GC.getUnitInfo(eLoopUnit).getDomainType() == eDomain)
			{
				int iBombardRate = GC.getUnitInfo(eLoopUnit).getBombardRate();

				if (iBombardRate > 0)
				{
					iTotalBombard += iBombardRate * getUnitClassCount((UnitClassTypes)iI);
				}
			}
		}
	}

	return iTotalBombard;
}

int CvPlayerAI::AI_getUnitClassWeight(UnitClassTypes eUnitClass)
{
	return m_aiUnitClassWeights[eUnitClass] / 100;
}

int CvPlayerAI::AI_getUnitCombatWeight(UnitCombatTypes eUnitCombat)
{
	return m_aiUnitCombatWeights[eUnitCombat] / 100;
}

void CvPlayerAI::AI_doEnemyUnitData()
{
	std::vector<int> aiUnitCounts(GC.getNumUnitInfos(), 0);

	std::vector<int> aiDomainSums(NUM_DOMAIN_TYPES, 0);

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int iI;

	int iOldTotal = 0;
	int iNewTotal = 0;


	for (iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{

		CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
		int iAdjacentAttackers = -1;
		if (pLoopPlot->isVisible(getTeam(), false))
		{
			pUnitNode = pLoopPlot->headUnitNode();

			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

				if (pLoopUnit->canFight())
				{
					int iUnitValue = 1;
					if (atWar(getTeam(), pLoopUnit->getTeam()))
					{
						iUnitValue += 10;

						if ((pLoopPlot->getOwnerINLINE() == getID()))
						{
							iUnitValue += 15;
						}
						else if (atWar(getTeam(), pLoopPlot->getTeam()))
						{
							if (iAdjacentAttackers == -1)
							{
								iAdjacentAttackers = GET_PLAYER(pLoopPlot->getOwnerINLINE()).AI_adjacentPotentialAttackers(pLoopPlot);
							}
							if (iAdjacentAttackers > 0)
							{
								iUnitValue += 15;
							}
						}
					}
					else if (pLoopUnit->getOwnerINLINE() != getID())
					{
						iUnitValue += pLoopUnit->canAttack() ? 4 : 1;
						if (pLoopPlot->getCulture(getID()) > 0)
						{
							iUnitValue += pLoopUnit->canAttack() ? 4 : 1;
						}
					}

					if (m_aiUnitClassWeights[pLoopUnit->getUnitClassType()] == 0)
					{
						iUnitValue *= 4;
					}

					iUnitValue *= pLoopUnit->baseCombatStr();
					aiUnitCounts[pLoopUnit->getUnitType()] += iUnitValue;
					aiDomainSums[pLoopUnit->getDomainType()] += iUnitValue;
					iNewTotal += iUnitValue;
				}
			}
		}
	}

	if (iNewTotal == 0)
	{
		//This should rarely happen.
		return;
	}

	//Decay
	for (iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		m_aiUnitClassWeights[iI] -= 100;
		m_aiUnitClassWeights[iI] *= 3;
		m_aiUnitClassWeights[iI] /= 4;
		m_aiUnitClassWeights[iI] = std::max(0, m_aiUnitClassWeights[iI]);
	}

	for (iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		if (aiUnitCounts[iI] > 0)
		{
			UnitTypes eLoopUnit = (UnitTypes)iI;
			aiUnitCounts[iI] = 0;
			FAssert(aiDomainSums[GC.getUnitInfo(eLoopUnit).getDomainType()] > 0);
			m_aiUnitClassWeights[GC.getUnitInfo(eLoopUnit).getUnitClassType()] += (5000 * aiUnitCounts[iI]) / std::max(1, aiDomainSums[GC.getUnitInfo(eLoopUnit).getDomainType()]);
		}
	}

	for (iI = 0; iI < GC.getNumUnitCombatInfos(); ++iI)
	{
		m_aiUnitCombatWeights[iI] = 0;
	}

	for (iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		if (m_aiUnitClassWeights[iI] > 0)
		{
			UnitTypes eUnit = (UnitTypes)GC.getUnitClassInfo((UnitClassTypes)iI).getDefaultUnitIndex();
			m_aiUnitCombatWeights[GC.getUnitInfo(eUnit).getUnitCombatType()] += m_aiUnitClassWeights[iI];

		}
	}

	for (iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
	{
		if (m_aiUnitCombatWeights[iI] > 25)
		{
			m_aiUnitCombatWeights[iI] += 2500;
		}
		else if (m_aiUnitCombatWeights[iI] > 0)
		{
			m_aiUnitCombatWeights[iI] += 1000;
		}
	}
}

int CvPlayerAI::AI_calculateUnitAIViability(UnitAITypes eUnitAI, DomainTypes eDomain)
{
	int iBestUnitAIStrength = 0;
	int iBestOtherStrength = 0;

	for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		UnitTypes eLoopUnit = (UnitTypes)GC.getUnitClassInfo((UnitClassTypes)iI).getDefaultUnitIndex();
		CvUnitInfo& kUnitInfo = GC.getUnitInfo((UnitTypes)iI);
		if (kUnitInfo.getDomainType() == eDomain)
		{
			if (m_aiUnitClassWeights[iI] > 0)
			{
				if (kUnitInfo.getUnitAIType(eUnitAI))
				{
					iBestUnitAIStrength = std::max(iBestUnitAIStrength, kUnitInfo.getCombat());
				}

				iBestOtherStrength = std::max(iBestOtherStrength, kUnitInfo.getCombat());
			}
		}
	}

	return (100 * iBestUnitAIStrength) / std::max(1, iBestOtherStrength);
}

int CvPlayerAI::AI_getAttitudeWeight(PlayerTypes ePlayer)
{
	int iAttitudeWeight = 0;
	switch (AI_getAttitude(ePlayer))
	{
	case ATTITUDE_FURIOUS:
		iAttitudeWeight = -100;
		break;
	case ATTITUDE_ANNOYED:
		iAttitudeWeight = -50;
		break;
	case ATTITUDE_CAUTIOUS:
		iAttitudeWeight = 0;
		break;
	case ATTITUDE_PLEASED:
		iAttitudeWeight = 50;
		break;
	case ATTITUDE_FRIENDLY:
		iAttitudeWeight = 100;
		break;
	}

	return iAttitudeWeight;
}

//vetiarvind, super forts - begin (mark as const)
int CvPlayerAI::AI_getPlotCanalValue(CvPlot* pPlot) const
{
	PROFILE_FUNC();

	FAssert(pPlot != NULL);

	// Super Forts begin *canal*

	if(pPlot == NULL) return 0;//R&R mod, vetiarvind, super forts merge, add null check just in case

	int iCanalValue = pPlot->getCanalValue();

	if (iCanalValue > 0)
	{

		if (pPlot->isOwned())
		{
			if (pPlot->getTeam() != getTeam())
			{
				return 0;
			}
			if (pPlot->isCityRadius())
			{
				CvCity* pWorkingCity = pPlot->getWorkingCity();
				if (pWorkingCity != NULL)
				{
					if (pWorkingCity->AI_getBestBuild(pWorkingCity->getCityPlotIndex(pPlot)) != NO_BUILD)
					{
						return 0;
					}
					if (pPlot->getImprovementType() != NO_IMPROVEMENT)
					{
						CvImprovementInfo &kImprovementInfo = GC.getImprovementInfo(pPlot->getImprovementType());
						if (!kImprovementInfo.isActsAsCity())
						{
							return 0;
						}
						// Decrease value when within radius of a city
						iCanalValue -= 5;
					}
				}
			}
		}

		for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			CvPlot* pLoopPlot = plotDirection(pPlot->getX_INLINE(), pPlot->getY_INLINE(), (DirectionTypes)iI);
			if (pLoopPlot != NULL)
			{
				/* original
				if (pLoopPlot->isCity(true))
				{
					return 0;
				}*/
				if (pLoopPlot->isCity(true) && (pLoopPlot->getCanalValue() > 0))
				{
					// Decrease value when adjacent to a city or fort with a canal value
					iCanalValue -= 10;
				}
			}
		}

		//super forts - begin
		iCanalValue *= 10;
		
		int iDefenseModifier = pPlot->defenseModifier(getTeam(), false);
		iCanalValue += iDefenseModifier; // Favor plots with higher defense
		//super forts - end
	}

	return std::max(0,iCanalValue);
	/* original
	CvArea* pSecondWaterArea = pPlot->secondWaterArea();
	if (pSecondWaterArea == NULL)
	{
		return 0;
	}
	return 10 * std::min(0, pSecondWaterArea->getNumTiles() - 2); //vetiarvind - i think this should be max though
	*/

	
}
// super forts end

// Super Forts begin *choke*
int CvPlayerAI::AI_getPlotChokeValue(CvPlot* pPlot) const
{
	PROFILE_FUNC();
	
	FAssert(pPlot != NULL);

	if(pPlot == NULL) return 0;//R&R mod, vetiarvind, super forts merge, add null check just in case

	int iChokeValue = pPlot->getChokeValue();

	if (iChokeValue > 0)
	{
		if (pPlot->isOwned())
		{
			if (pPlot->getTeam() != getTeam())
			{
				return 0;
			}
			if (pPlot->isCityRadius())
			{
				CvCity* pWorkingCity = pPlot->getWorkingCity();
				if (pWorkingCity != NULL)
				{
					// Left in this part from the original code. Might be needed to avoid workers from getting stuck in a loop?
					if (pWorkingCity->AI_getBestBuild(pWorkingCity->getCityPlotIndex(pPlot)) != NO_BUILD)
					{
						return 0;
					}
					if (pPlot->getImprovementType() != NO_IMPROVEMENT)
					{
						CvImprovementInfo &kImprovementInfo = GC.getImprovementInfo(pPlot->getImprovementType());
						if (!kImprovementInfo.isActsAsCity())
						{
							return 0;
						}
					}
					// Decrease value when within radius of a city
					iChokeValue -= 5;
				}
			}
		}
	
		for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			CvPlot* pLoopPlot = plotDirection(pPlot->getX_INLINE(), pPlot->getY_INLINE(), (DirectionTypes)iI);
			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->isCity(true) && (pLoopPlot->getChokeValue() > 0))
				{
					// Decrease value when adjacent to a city or fort with a choke value
					iChokeValue -= 10;
				}
			}
		}

		iChokeValue *= 10;
		// Favor plots with higher defense
		int iDefenseModifier = pPlot->defenseModifier(getTeam(), false);
		iChokeValue += iDefenseModifier;
	}

	return std::max(0,iChokeValue);
}
// Super Forts end

void CvPlayerAI::AI_diplomaticHissyFit(PlayerTypes ePlayer, int iAttitudeChange)
{
	if (ePlayer == NO_PLAYER)
	{
		FAssert(false);
		return;
	}
	CvPlayer& kPlayer = GET_PLAYER(ePlayer);

	if (iAttitudeChange >= 0)
	{
		return;
	}

	if (atWar(getTeam(), kPlayer.getTeam()))
	{
		return;
	}

	if (!kPlayer.isHuman())
	{
		return;
	}

	if (GET_TEAM(getTeam()).AI_performNoWarRolls(kPlayer.getTeam()))
	{
		return;
	}

	//Out of 1000, so 100 is a 10% chance.
	//Note this could be modified by all sorts of things. Difficulty level might be a good one.
	int iProbability = -iAttitudeChange * 100;

	iProbability /= (GET_TEAM(getTeam()).getNumMembers() * GET_TEAM(kPlayer.getTeam()).getNumMembers());

	if (iProbability < GC.getGameINLINE().getSorenRandNum(1000, "AI Diplomatic Hissy Fit"))
	{
		return;
	}

	GET_TEAM(getTeam()).declareWar(kPlayer.getTeam(), true, WARPLAN_EXTORTION);

}

UnitTypes CvPlayerAI::AI_nextBuyUnit(UnitAITypes* peUnitAI, int* piValue)
{
	if (peUnitAI != NULL)
	{
		*peUnitAI = m_eNextBuyUnitAI;
	}
	if (piValue != NULL)
	{
		*piValue = m_iNextBuyUnitValue;
	}
	return m_eNextBuyUnit;
}

UnitTypes CvPlayerAI::AI_nextBuyProfessionUnit(ProfessionTypes* peProfession, UnitAITypes* peUnitAI, int* piValue)
{
	if (peProfession != NULL)
	{
		*peProfession = m_eNextBuyProfession;
	}
	if (peUnitAI != NULL)
	{
		*peUnitAI = m_eNextBuyProfessionAI;
	}
	if (peUnitAI != NULL)
	{
		*piValue = m_iNextBuyProfessionValue;
	}
	return m_eNextBuyProfessionUnit;
}

// TAC - AI purchases military units - koma13 - START
//void CvPlayerAI::AI_updateNextBuyUnit()
void CvPlayerAI::AI_updateNextBuyUnit(bool bPriceLimit)
// TAC - AI purchases military units - koma13 - END
{
	PROFILE_FUNC();
	int iBestValue = 0;
	UnitTypes eBestUnit = NO_UNIT;
	UnitAITypes eBestUnitAI = NO_UNITAI;

	for (int iUnitAI = 0; iUnitAI < NUM_UNITAI_TYPES; ++iUnitAI)
	{
		// Erik: Do not consider units that cannot cross the ocean
		if (iUnitAI == UNITAI_TRANSPORT_COAST)
			continue;

		UnitAITypes eLoopUnitAI = (UnitAITypes) iUnitAI;
		bool bValid = false;

		int iMultipler = AI_unitAIValueMultipler(eLoopUnitAI);
		if (iMultipler > 0)
		{
			bValid = true;
		}
		
		// TAC - AI purchases military units - koma13 - START
		/*
		int iTreasureSum = -1;
		int iTreasureSize = -1;

		if ((eLoopUnitAI == UNITAI_TRANSPORT_SEA) && (AI_totalUnitAIs(UNITAI_TREASURE) > 0))
		{
			int iLoop;
			CvUnit* pLoopUnit;
			for (pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
			{
				if (pLoopUnit->canMove())
				{
					if (pLoopUnit->AI_getUnitAIType() == UNITAI_TREASURE)
					{
						int iSize = pLoopUnit->getUnitInfo().getRequiredTransportSize();
						if (iSize > 1)
						{
							iTreasureSize = std::max(iTreasureSize, iSize);
							iTreasureSum += pLoopUnit->getYieldStored();
						}
					}
				}
			}
			for (pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
			{
				if (pLoopUnit->AI_getUnitAIType() == UNITAI_TRANSPORT_SEA)
				{
					if (pLoopUnit->cargoSpace() >= iTreasureSize)
					{
						//Can already transport all treasure, cancel.
						iTreasureSize = -1;
						iTreasureSum = -1;
						break;
					}
				}
			}
		}
		if (iTreasureSum < 1)
		{
			iTreasureSum = 1;
		}
		if (iTreasureSize < 1)
		{
			iTreasureSize = 1;
		}
		if (iTreasureSum > 0)
		{
			bValid = true;
		}
		*/
		// TAC - AI purchases military units - koma13 - END
		// TAC - AI Assault Sea - koma13 - START
		if (eLoopUnitAI == UNITAI_ASSAULT_SEA || eLoopUnitAI == UNITAI_ESCORT_SEA)
		{
			if (!AI_prepareAssaultSea())
			{
				bValid = false;
			}
		}
		// TAC - AI Assault Sea - koma13 - END

		if (bValid)
		{
			for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
			{
				UnitTypes eLoopUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI)));
				if (eLoopUnit != NO_UNIT)
				{
					const CvUnitInfo& kUnitInfo = GC.getUnitInfo(eLoopUnit);
					if (kUnitInfo.getDefaultProfession() == NO_PROFESSION || kUnitInfo.getDefaultUnitAIType() == UNITAI_DEFENSIVE || kUnitInfo.getDefaultUnitAIType() == UNITAI_COUNTER)
					{
						int iPrice = getEuropeUnitBuyPrice(eLoopUnit);
						// TAC - AI purchases military units - koma13 - START
						//if (iPrice > 0)// && !kUnitInfo.getUnitAIType(eLoopUnitAI))
						int iInitialPrice = getEuropeUnitBuyPrice(eLoopUnit, false);
						int iMaxPrice = (iInitialPrice * GC.getDefineINT("AI_EUROPE_PRICE_LIMIT_PERCENT")) / 100;
						if ((iPrice > 0) && ((iPrice < iMaxPrice) || !bPriceLimit)) // && (iPrice <= getGold()))
						{
							/*
							if (iTreasureSum > 0) //Perform treasure calculations
							{
								if (kUnitInfo.getCargoSpace() >= iTreasureSize)
								{
									iMultipler += 100 + (100 * iTreasureSum) / iPrice;
									iPrice = std::max(iPrice / 3, iPrice - iTreasureSum);
								}
							}
							*/

							if (kUnitInfo.getDefaultUnitAIType() == UNITAI_COMBAT_SEA)
							{
								//if (kUnitInfo.isHiddenNationality())
								if (kUnitInfo.isHiddenNationality() || (kUnitInfo.getDefaultUnitAIType() == UNITAI_TRANSPORT_SEA))
								{
									iMultipler = 0;
								}
							}

							int iGoldValue = AI_unitGoldValue(eLoopUnit, eLoopUnitAI, NULL);
							int iValue = (iMultipler * iGoldValue) / iPrice;
							if (getEuropeUnitBuyPrice(eLoopUnit) > getGold())
							{
								if (iValue < 100)
								{
									iValue = 0;
								}
							}

							if (iValue > iBestValue && !kUnitInfo.getNotUnitAIType(eLoopUnitAI))
							{
								iBestValue = iValue;
								eBestUnit = eLoopUnit;
								eBestUnitAI = eLoopUnitAI;
							}
							// TAC - AI purchases military units - koma13 - END
						}
					}
				}
			}
		}
	}

	// TAC - AI Military Buildup - koma13 - START
	if (bPriceLimit && (eBestUnit != NO_UNIT))
	{
		if (getEuropeUnitBuyPrice(eBestUnit) > getGold())
		{
			if (!AI_isStrategy(STRATEGY_MILITARY_BUILDUP))
			{
				AI_setStrategy(STRATEGY_MILITARY_BUILDUP);
			}
		}
	}
	// TAC - AI Military Buildup - koma13 - END

	m_eNextBuyUnit = eBestUnit;
	m_eNextBuyUnitAI = eBestUnitAI;
	m_iNextBuyUnitValue = iBestValue;
}

int CvPlayerAI::AI_highestNextBuyValue()
{
	return std::max(m_iNextBuyUnitValue, m_iNextBuyProfessionValue);
}

void CvPlayerAI::AI_updateNextBuyProfession()
{
	PROFILE_FUNC();
	int iBestValue = 0;
	UnitTypes eBestProfessionUnit = NO_UNIT;
	ProfessionTypes eBestProfession = NO_PROFESSION;
	UnitAITypes eBestUnitAI = NO_UNITAI;

	// TAC - AI Economy - koma13 - START
	if (!AI_isStrategy(STRATEGY_REVOLUTION_PREPARING))
	// TAC - AI Economy - koma13 - END
	{
		ProfessionTypes eDefaultProfession = (ProfessionTypes) GC.getCivilizationInfo(getCivilizationType()).getDefaultProfession();
		int iColMultiplier = AI_unitAIValueMultipler(UNITAI_COLONIST);
		//Professions which work in cities.
		for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
		{
			UnitTypes eLoopUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI)));
			if (eLoopUnit != NO_UNIT)
			{
				CvUnitInfo& kUnitInfo = GC.getUnitInfo(eLoopUnit);
				int iPrice = getEuropeUnitBuyPrice(eLoopUnit);
				if (iPrice > 0)
				{
					if (kUnitInfo.getDefaultProfession() != NO_PROFESSION)
					{
						int iValue = 0;
						UnitAITypes eUnitAI = NO_UNITAI;
						//if (kUnitInfo.getDefaultProfession() == eDefaultProfession)
						{
							ProfessionTypes eProfession = AI_idealProfessionForUnit(eLoopUnit);
							if (eProfession != NO_PROFESSION)
							{
								// TAC - AI Economy - koma13 - START
								// R&R, ray , MYCP partially based on code of Aymerick - START
								YieldTypes eYieldConsumed = (YieldTypes)GC.getProfessionInfo(eProfession).getYieldsConsumed(0);
								YieldTypes eYieldProduced = (YieldTypes)GC.getProfessionInfo(eProfession).getYieldsProduced(0);
								// R&R, ray , MYCP partially based on code of Aymerick - END
								if ((eYieldConsumed != NO_YIELD) && (eYieldProduced != NO_YIELD) && GC.getYieldInfo(eYieldProduced).isCargo())
								// TAC - AI Economy - koma13 - END
								{
									int iValue = 50 + 3 * AI_professionUpgradeValue(eProfession, eLoopUnit);
									iValue *= iColMultiplier;
									iValue /= 100;
									int iExisting = getUnitClassCountPlusMaking((UnitClassTypes)iI);
									if (iExisting < 3)
									{
										iValue *= 100 + (5 + getTotalPopulation()) * kUnitInfo.getYieldModifier(YIELD_LUMBER) / (5 * (1 + iExisting));
										iValue /= 100;
										iValue *= 100 + (5 + getTotalPopulation()) * (44 * kUnitInfo.getYieldChange(YIELD_FOOD) + 34 * kUnitInfo.getBonusYieldChange(YIELD_FOOD)) / (5 * (1 + iExisting)) ;
										iValue /= 100;
										if (AI_isStrategy(STRATEGY_FAST_BELLS))
										{
											iValue *= 100 + kUnitInfo.getYieldModifier(YIELD_BELLS) / (2 + iExisting);
											iValue /= 100;
										}
									}
									for (int i = 0; i < NUM_YIELD_TYPES; ++i)
									{
										YieldTypes eLoopYield = (YieldTypes)i;
										int iModifier = kUnitInfo.getYieldModifier(eLoopYield);
										if (iModifier > 0)
										{
											if (AI_highestYieldAdvantage(eLoopYield) == 100)
											{
												if (!AI_isYieldFinalProduct(eLoopYield))
												{
													iModifier /= 4;
												}
												iValue *= 100 + iModifier;
												iValue /= 100;
											}
										}
									}
									if (iExisting < 4)
									{
										iValue *= 2;
										iValue /= 2 + iExisting;
										if (iValue > iBestValue)
										{
											iBestValue = iValue;
											eBestProfession = eDefaultProfession;
											eBestProfessionUnit = eLoopUnit;
											eBestUnitAI = UNITAI_COLONIST;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		// TAC - AI Economy - koma13 - START
		/*
		for (int iUnitAI = 0; iUnitAI < NUM_UNITAI_TYPES; ++iUnitAI)
		{
			UnitAITypes eLoopUnitAI = (UnitAITypes) iUnitAI;	
			if (eLoopUnitAI != UNITAI_COLONIST)
			{
				int iMultiplier = AI_unitAIValueMultipler(eLoopUnitAI);	
				if (iMultiplier > 0)
				{
					ProfessionTypes eProfession = AI_idealProfessionForUnitAIType(eLoopUnitAI);	
					if (eProfession != NO_PROFESSION)
					{
						UnitTypes eBestUnit = NO_UNIT;	
						for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
						{
							UnitTypes eLoopUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI)));	
							if (eLoopUnit != NO_UNIT)
							{
								CvUnitInfo& kUnitInfo = GC.getUnitInfo(eLoopUnit);	
								int iPrice = getEuropeUnitBuyPrice(eLoopUnit);
								if (iPrice > 0)
								{	
									int iValue = AI_professionSuitability(eLoopUnit, eProfession);
									if (iValue >= 100)
									{
										iValue *= 2 * iMultiplier;
										iValue /= 3 * 100;	
										if (iValue > iBestValue)
										{
											iBestValue = iValue;
											eBestProfession = eProfession;
											eBestProfessionUnit = eLoopUnit;
											eBestUnitAI = eLoopUnitAI;
										}
									}
								}
							}
						}	
						if (eBestUnit == NO_UNIT)
						{
							//Special Case.
							int iValue = iMultiplier;	
							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								eBestProfession = eProfession;
								eBestProfessionUnit = NO_UNIT;
								eBestUnitAI = eLoopUnitAI;
							}
						}
					}
				}
			}
		}
		*/
		// TAC - AI Economy - koma13 - END
	}
	
	m_eNextBuyProfession = eBestProfession;
	m_eNextBuyProfessionUnit = eBestProfessionUnit;
	m_eNextBuyProfessionAI = eBestUnitAI;
	m_iNextBuyProfessionValue = iBestValue;
}

void CvPlayerAI::AI_invalidateCloseBordersAttitudeCache()
{
	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		m_aiCloseBordersAttitudeCache[i] = MAX_INT;
		m_aiStolenPlotsAttitudeCache[i] = MAX_INT;
	}
}


EmotionTypes CvPlayerAI::AI_strongestEmotion()
{
	int iBestValue = 0;
	EmotionTypes eBestEmotion = NO_EMOTION;

	for (int i = 0; i < NUM_EMOTION_TYPES; ++i)
	{
		if (m_aiEmotions[i] > iBestValue)
		{
			iBestValue = m_aiEmotions[i];
			eBestEmotion = (EmotionTypes)i;
		}
	}

	return eBestEmotion;
}

int CvPlayerAI::AI_emotionWeight(EmotionTypes eEmotion)
{
	EmotionTypes eBestEmotion = AI_strongestEmotion();
	if (eBestEmotion == NO_EMOTION)
	{
		return 0;
	}
	return (100 * m_aiEmotions[eEmotion]) / (m_aiEmotions[eBestEmotion]);
}

int CvPlayerAI::AI_getEmotion(EmotionTypes eEmotion)
{
	FAssert(eEmotion > NO_EMOTION);
	FAssert(eEmotion < NUM_EMOTION_TYPES);
	return m_aiEmotions[eEmotion];
}

void CvPlayerAI::AI_setEmotion(EmotionTypes eEmotion, int iNewValue)
{
	FAssert(eEmotion > NO_EMOTION);
	FAssert(eEmotion < NUM_EMOTION_TYPES);
	m_aiEmotions[eEmotion] = iNewValue;
}

void CvPlayerAI::AI_changeEmotion(EmotionTypes eEmotion, int iChange)
{
	FAssert(eEmotion > NO_EMOTION);
	FAssert(eEmotion < NUM_EMOTION_TYPES);
	m_aiEmotions[eEmotion] += iChange;
}

bool CvPlayerAI::AI_isAnyStrategy() const
{
	for (int i = 0; i < NUM_STRATEGY_TYPES; ++i)
	{
		if (AI_isStrategy((StrategyTypes)i))
		{
			return true;
		}
	}

	return false;
}

bool CvPlayerAI::AI_isStrategy(StrategyTypes eStrategy) const
{
	FAssert(eStrategy > NO_STRATEGY);
	FAssert(eStrategy < NUM_STRATEGY_TYPES);
	return (m_aiStrategyStartedTurn[eStrategy] != -1);
}

int CvPlayerAI::AI_getStrategyDuration(StrategyTypes eStrategy) const
{
	FAssert(eStrategy > NO_STRATEGY);
	FAssert(eStrategy < NUM_STRATEGY_TYPES);
	if (!AI_isStrategy(eStrategy))
	{
		return -1;
	}

	return (GC.getGameINLINE().getGameTurn() - m_aiStrategyStartedTurn[eStrategy]);
}

int CvPlayerAI::AI_getStrategyData(StrategyTypes eStrategy)
{
	FAssert(eStrategy > NO_STRATEGY);
	FAssert(eStrategy < NUM_STRATEGY_TYPES);
	return m_aiStrategyData[eStrategy];
}

void CvPlayerAI::AI_setStrategy(StrategyTypes eStrategy, int iData)
{
	FAssert(eStrategy > NO_STRATEGY);
	FAssert(eStrategy < NUM_STRATEGY_TYPES);
	m_aiStrategyStartedTurn[eStrategy] = GC.getGameINLINE().getGameTurn();
	m_aiStrategyData[eStrategy] = iData;
}

void CvPlayerAI::AI_clearStrategy(StrategyTypes eStrategy)
{
	FAssert(eStrategy > NO_STRATEGY);
	FAssert(eStrategy < NUM_STRATEGY_TYPES);
	m_aiStrategyStartedTurn[eStrategy] = -1;
	m_aiStrategyData[eStrategy] = -1;
}
// TAC - AI Military Buildup - koma13 - START
UnitAITypes CvPlayerAI::AI_bestBuildupUnitAI()
{	
	UnitTypes eBuyUnit;
	UnitAITypes eBuyUnitAI;
	int iBuyUnitValue;

	eBuyUnit = AI_nextBuyUnit(&eBuyUnitAI, &iBuyUnitValue);
	
	if (eBuyUnit != NO_UNIT)
	{
		return eBuyUnitAI;
	}

	return NO_UNITAI;
}
// TAC - AI Military Buildup - koma13 - END

// TAC - AI Improved Naval AI - koma13 - START
bool CvPlayerAI::AI_isPathDanger(const CvSelectionGroup* pGroup, const CvPlot* pFromPlot, const CvPlot* pToPlot, int iRange) const
{
	CvPlot* pPlot = pGroup->plot();
		
	int iMovesLeft;
	int iStart;
	int iPathLength;
	
	if (iRange == -1)
	{
		iRange = GC.getAI_TRANSPORT_DANGER_RANGE();
	}
		
	iMovesLeft = (pGroup->getHeadUnit()->movesLeft() / GC.getMOVE_DENOMINATOR());
	iStart = (pPlot == pToPlot) ? iMovesLeft : 0;
	iPathLength = (pPlot == pFromPlot) ? pGroup->getPathLength() - iMovesLeft : pGroup->getPathLength();
		
	// R&R, ray, commented out unnecessary Asserts, probably used for testing feature
	// FAssert(iPathLength > 0);
	// FAssert(iStart < iPathLength);

	for (int i = iStart; i < iPathLength; ++i)
	{
		CvPlot* pLoopPlot = pGroup->getPathPlotByIndex(i);
			
		if (AI_getWaterDanger(pLoopPlot, iRange, true, true, true) > 0)
		{
			return true;
		}
	}

	return false;
}
// TAC - AI Improved Naval AI - koma13 - END

int CvPlayerAI::AI_cityDistance(CvPlot* pPlot)
{
	FAssert(pPlot != NULL);

	if (m_iDistanceMapDistance == -1)
	{
		AI_getDistanceMap();
	}

	return m_distanceMap[GC.getMapINLINE().plotNumINLINE(pPlot->getX_INLINE(), pPlot->getY_INLINE())];
}

//There's no need to save this (it is very fast to generate anyway)
std::vector<short>* CvPlayerAI::AI_getDistanceMap()
{
	if (m_iDistanceMapDistance != -1)
	{
		return &m_distanceMap;
	}

	int iMaxRange = MAX_SHORT;
	CvMap& kMap = GC.getMap();

	std::deque<int>plotQueue;
	m_distanceMap.resize(kMap.numPlotsINLINE());

	for (int i = 0; i < kMap.numPlotsINLINE(); ++i)
	{
		CvPlot* pLoopPlot = kMap.plotByIndexINLINE(i);

		if (pLoopPlot->isCity() && (pLoopPlot->getOwnerINLINE() == getID()))
		{
			plotQueue.push_back(i);
			m_distanceMap[i] = 0;
		}
		else
		{
			m_distanceMap[i] = iMaxRange;
		}
	}

	int iVisits = 0;
	while (!plotQueue.empty())
	{
		iVisits++;
		int iPlot = plotQueue.front();
		CvPlot* pPlot = kMap.plotByIndexINLINE(iPlot);
		plotQueue.pop_front();

		int iDistance = m_distanceMap[iPlot];
		iDistance += 1;

		if (iDistance < iMaxRange)
		{
			for (int iDirection = 0; iDirection < NUM_DIRECTION_TYPES; iDirection++)
			{
				CvPlot* pDirectionPlot = plotDirection(pPlot->getX_INLINE(), pPlot->getY_INLINE(), (DirectionTypes)iDirection);
				if (pDirectionPlot != NULL)
				{
					if ((pDirectionPlot->isWater() && pPlot->isWater())
						|| (!pDirectionPlot->isWater() && !pPlot->isWater())
							|| (pDirectionPlot->isWater() && (pPlot->isCity() && (pPlot->getOwnerINLINE() == getID()))))
					{

						int iPlotNum = kMap.plotNumINLINE(pDirectionPlot->getX_INLINE(), pDirectionPlot->getY_INLINE());
						if (iDistance < m_distanceMap[iPlotNum])
						{
							m_distanceMap[iPlotNum] = iDistance;
							plotQueue.push_back(iPlotNum);
						}
					}
				}
			}
		}
	}
	m_iDistanceMapDistance = iMaxRange;
	return &m_distanceMap;
}

void CvPlayerAI::AI_invalidateDistanceMap()
{
	m_iDistanceMapDistance = -1;
}

void CvPlayerAI::AI_updateBestYieldPlots()
{
	int aiBestWorkedYield[NUM_YIELD_TYPES];
	int aiBestUnworkedYield[NUM_YIELD_TYPES];

	for (int i = 0; i < NUM_YIELD_TYPES; ++i)
	{
		m_aiBestWorkedYieldPlots[i] = -1;
		m_aiBestUnworkedYieldPlots[i] = -1;

		aiBestWorkedYield[i] = 0;
		aiBestUnworkedYield[i] = 0;
	}
	CvMap& kMap = GC.getMapINLINE();
	for (int i = 0; i < kMap.numPlotsINLINE(); ++i)
	{
		CvPlot* pLoopPlot = kMap.plotByIndex(i);

		if (pLoopPlot->isCityRadius() && (pLoopPlot->getOwnerINLINE() == getID()))
		{
			for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
			{
				int iPlotYield = std::max(pLoopPlot->calculateNatureYield((YieldTypes)iYield, getTeam(), false), pLoopPlot->calculateNatureYield((YieldTypes)iYield, getTeam(), true));
				if (iPlotYield > 0)
				{
					if (pLoopPlot->isBeingWorked() && pLoopPlot->getYield((YieldTypes)iYield) > 0)
					{
						if (iPlotYield > aiBestWorkedYield[iYield])
						{
							aiBestWorkedYield[iYield] = iPlotYield;
							m_aiBestWorkedYieldPlots[iYield] = i;
						}
					}
					else
					{
						if (iPlotYield > aiBestUnworkedYield[iYield])
						{
							aiBestUnworkedYield[iYield] = iPlotYield;
							m_aiBestUnworkedYieldPlots[iYield] = i;
						}
					}
				}
			}
		}
	}
}

CvPlot* CvPlayerAI::AI_getBestWorkedYieldPlot(YieldTypes eYield)
{
	FAssertMsg(eYield > NO_YIELD, "Index out of bounds");
	FAssertMsg(eYield < NUM_YIELD_TYPES, "Index out of bounds");

	//Automatically returns NULL, if -1.
	return GC.getMapINLINE().plotByIndexINLINE(m_aiBestWorkedYieldPlots[eYield]);
}

CvPlot* CvPlayerAI::AI_getBestUnworkedYieldPlot(YieldTypes eYield)
{
	FAssertMsg(eYield > NO_YIELD, "Index out of bounds");
	FAssertMsg(eYield < NUM_YIELD_TYPES, "Index out of bounds");

	//Automatically returns NULL, if -1.
	return GC.getMapINLINE().plotByIndexINLINE(m_aiBestUnworkedYieldPlots[eYield]);
}

int CvPlayerAI::AI_getBestPlotYield(YieldTypes eYield)
{
	CvPlot* pPlot = AI_getBestWorkedYieldPlot(eYield);
	if (pPlot == NULL)
	{
		pPlot = AI_getBestUnworkedYieldPlot(eYield);
	}
	if (pPlot == NULL)
	{
		return 0;
	}
	return pPlot->calculateBestNatureYield(eYield, getTeam());
}

void CvPlayerAI::AI_changeTotalIncome(int iChange)
{
	m_iTotalIncome += iChange;
}

int CvPlayerAI::AI_getTotalIncome()
{
	return m_iTotalIncome;
}

void CvPlayerAI::AI_changeHurrySpending(int iChange)
{
	m_iHurrySpending += iChange;
}

int CvPlayerAI::AI_getHurrySpending()
{
	return m_iHurrySpending;
}

// TAC - AI More Immigrants - koma13 - START
void CvPlayerAI::AI_changeImmigrantSpending(int iChange)
{
	m_iImmigrantSpending += iChange;
}

int CvPlayerAI::AI_getImmigrantSpending()
{
	return m_iImmigrantSpending;
}
// TAC - AI More Immigrants - koma13 - END

// TAC - AI Economy - koma13 - START
void CvPlayerAI::AI_changeEuropeYieldSpending(int iChange)
{
	m_iEuropeYieldSpending += iChange;
}

int CvPlayerAI::AI_getEuropeYieldSpending()
{
	return m_iEuropeYieldSpending;
}
// TAC - AI Economy - koma13 - END

// TAC - AI Improved Naval AI - koma13 - START
bool CvPlayerAI::AI_needsProtection(UnitAITypes eUnitAI) const
{
	if (!isHuman())
	{
		switch (eUnitAI)
		{
			case UNITAI_TRANSPORT_SEA:
			case UNITAI_TRANSPORT_COAST:
				// Erik: Return value was removed by accident, sorry!
				return true;
				break;

			default:
				break;
		}
	}

	return false;
}
// TAC - AI Improved Naval AI - koma13 - END
int CvPlayerAI::AI_getPopulation()
{
	int iTotal = 0;

	int iLoop;
	CvCity* pLoopCity;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iTotal += pLoopCity->getPopulation();
	}

	return iTotal;
}

bool CvPlayerAI::AI_shouldAttackAdjacentCity(CvPlot* pPlot)
{
	FAssert(pPlot != NULL);

	for (int i = 0; i < NUM_DIRECTION_TYPES; ++i)
	{
		CvPlot* pLoopPlot = plotDirection(pPlot->getX_INLINE(), pPlot->getY_INLINE(), (DirectionTypes)i);
		if (pLoopPlot != NULL)
		{
			if (atWar(getTeam(), pLoopPlot->getTeam()))
			{
				CvCity* pPlotCity = pLoopPlot->getPlotCity();
				if (pPlotCity != NULL)
				{
					if (!pPlotCity->isBombarded())
					{
						return true;
					}

					if (((100 * pPlotCity->getDefenseDamage()) / std::max(1, GC.getMAX_CITY_DEFENSE_DAMAGE())) > 90)
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

int CvPlayerAI::AI_getNumProfessionUnits(ProfessionTypes eProfession)
{
	int iCount = 0;

	int iLoop;
	CvCity* pLoopCity;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		for (int i = 0; i < pLoopCity->getPopulation(); ++i)
		{
			CvUnit* pLoopUnit = pLoopCity->getPopulationUnitByIndex(i);

			if (pLoopUnit->getProfession() == eProfession)
			{
				iCount ++;
			}
		}
	}

	return iCount;
}

int CvPlayerAI::AI_countNumCityUnits(UnitTypes eUnit)
{
	int iCount = 0;

	int iLoop;
	CvCity* pLoopCity;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		for (int i = 0; i < pLoopCity->getPopulation(); ++i)
		{
			CvUnit* pLoopUnit = pLoopCity->getPopulationUnitByIndex(i);

			if (pLoopUnit->getUnitType() == eUnit)
			{
				iCount ++;
			}
		}
	}

	return iCount;
}

int CvPlayerAI::AI_getNumCityUnitsNeeded(UnitTypes eUnit)
{
	int iCount = 0;

	// R&R, ray, code changes for Ideal Profession - START
	//ProfessionTypes eIdealProfession = AI_idealProfessionForUnit(eUnit);

	ProfessionTypes eIdealProfession = NO_PROFESSION; // only need one ideal profession, since we just check slots in building for same specialist
	UnitClassTypes eUnitClassType = (UnitClassTypes)GC.getUnitInfo(eUnit).getUnitClassType();

	for (int iI = 0; iI < GC.getNumProfessionInfos(); ++iI)
	{
		ProfessionTypes eLoopProfession = (ProfessionTypes)iI;
		CvProfessionInfo& kProfession = GC.getProfessionInfo(eLoopProfession);

		if (kProfession.isCitizen())
		{
			if ((UnitClassTypes)kProfession.LbD_getExpert() == eUnitClassType)
			{
				if(kProfession.isWorkPlot()) // only specialists for building professions
				{
					break;
				}
				else
				{
					eIdealProfession = eLoopProfession;
					break;
				}
			}
		}
	}

	if (eIdealProfession == NO_PROFESSION)
	{
		return 0;
	}
	// R&R, ray , MYCP partially based on code of Aymerick - START
	YieldTypes eYieldProducedType = (YieldTypes)GC.getProfessionInfo(eIdealProfession).getYieldsProduced(0);
	// R&R, ray , MYCP partially based on code of Aymerick - END

	// R&R, ray, code changes for Ideal Profession - END

	int iLoop;
	CvCity* pLoopCity;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->AI_getYieldAdvantage(eYieldProducedType) == 100)
		{
			iCount += pLoopCity->getNumProfessionBuildingSlots(eIdealProfession);
		}
	}
	return iCount;
}

int CvPlayerAI::AI_countPromotions(PromotionTypes ePromotion, CvPlot* pPlot, int iRange, int* piUnitCount)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int iCount = 0;
	int iUnitCount = 0;

	for (int iX = -iRange; iX <= iRange; ++iX)
	{
		for (int iY = -iRange; iY <= iRange; ++iY)
		{
			CvPlot* pLoopPlot = plotXY(pPlot->getX_INLINE(), pPlot->getY_INLINE(), iX, iY);
			if (pLoopPlot != NULL)
			{
				pUnitNode = pLoopPlot->headUnitNode();

				while (pUnitNode != NULL)
				{
					pLoopUnit = ::getUnit(pUnitNode->m_data);
					pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

					if (pLoopUnit->getOwnerINLINE() == getID())
					{
						if (pLoopUnit->isHasPromotion(ePromotion))
						{
							iCount++;
						}

						iUnitCount++;
					}
				}
			}
		}
	}

	if (piUnitCount != NULL)
	{
		*piUnitCount = iUnitCount;
	}
	return iCount;
}

// TAC - AI More Immigrants - koma13 - START
void CvPlayerAI::AI_hurryDocks()
{
	if (GC.getAI_HURRY_IMMIGRANTS() <= 0)
	{
		return;
	}

	if (isHuman())
	{
		return;
	}

	if (getParent() == NO_PLAYER)
	{
		return;
	}

	if (!canTradeWithEurope())
	{
		return;
	}
	
	for (int iIndex = 0; iIndex < GC.getDefineINT("DOCKS_NEXT_UNITS"); ++iIndex)
	{	
		UnitTypes eLoopUnit = (UnitTypes)getDocksNextUnit(iIndex);
			
		if (eLoopUnit != NO_UNIT)
		{
			CvUnitInfo& kUnitInfo = GC.getUnitInfo(eLoopUnit);
			if (kUnitInfo.isNoBadGoodies())
			{
				if (canHurry((HurryTypes)1, iIndex))
				{
					hurry((HurryTypes)1, iIndex);
				}
			}
		}
	}
}
// TAC - AI More Immigrants - koma13 - END

// TAC - AI Assault Sea - koma13 - START
bool CvPlayerAI::AI_prepareAssaultSea()
{
	if (isNative() || (getParent() == NO_PLAYER))
	{
		return false;
	}
	if (GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAITrainPercent() > 75)
	{
		return false;
	}

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayerAI& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);

		if (kLoopPlayer.isAlive())
		{
			if (!kLoopPlayer.isNative())
			{
				if (iI != getID() && (getTeam() != kLoopPlayer.getTeam()))
				{
					if (GET_TEAM(getTeam()).isHasMet(kLoopPlayer.getTeam()))
					{
						int iLoop;
						for (CvCity* pLoopCity = kLoopPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kLoopPlayer.nextCity(&iLoop))
						{
							if (pLoopCity->plot()->isRevealed(getTeam(), false))
							{
								if (!AI_isPrimaryArea(pLoopCity->area()))
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
	return false;
}
// TAC - AI Assault Sea - koma13 - END
// TAC - AI Revolution - koma13 - START
int CvPlayerAI::AI_getLastWave() const
{
	return m_iLastWave;
}

void CvPlayerAI::AI_setLastWave(int iTurn)
{
	FAssert(iTurn >= 0);
	FAssert(iTurn <= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getWaveTurns());
	m_iLastWave = iTurn;
}

int CvPlayerAI::AI_getWaveIndex() const
{
	return m_iWaveIndex;
}

void CvPlayerAI::AI_setWaveIndex(int iNum)
{
	FAssert(iNum >= -1);
	FAssert(iNum <= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getNumWaves());
	m_iWaveIndex = iNum;
}
// TAC - AI Revolution - koma13 - END
// TAC - AI City Sites - koma13 - START
bool CvPlayerAI::AI_isLandLocked(bool bIgnoreCultureLock) const
{
	int iLoop;
	for(CvArea* pLoopArea = GC.getMapINLINE().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMapINLINE().nextArea(&iLoop))
	{
		if (!pLoopArea->isWater())
		{
			if (pLoopArea->getCitiesPerPlayer(getID()) > 0)
			{
				if (pLoopArea->getBestFoundValue(getID()) > (bIgnoreCultureLock ? 1 : 1500))
				{
					return false;
				}
			}
		}
	}

	return true;
}
// TAC - AI City Sites - koma13 - END

// Returns the best price that we can get for the yield considering the export markets available to us
// TODO: Consider natives, fellow Europeans, internal market, Port Royal
int CvPlayerAI::AI_getYieldBestExportPrice(YieldTypes eYield) const
{
	FAssert(!isNative());
	FAssert(GC.getYieldInfo(eYield).isCargo());

	PlayerTypes eParent = getParent();

	// This will allow the king to consider his own market for export
	if (getParent() == NO_PLAYER)
	{
		eParent = getID();
	}

	const CvPlayer& kParent = GET_PLAYER(eParent);

	// Return the max price we would get by selling in the currently AI accessibly ports
	return std::max(kParent.getYieldBuyPrice(eYield), kParent.getYieldAfricaBuyPrice(eYield));
}

// TAC - AI City Sites - koma13 - END
// TAC - AI City Sites - koma13 - END

// Returns true if we should prefer to to hurry a unit rather than buying an equivalent unit
bool CvPlayerAI::AI_shouldHurryUnit() const
{
	// Note: Hurrying is generally better in the early game since the AI economy is not yet sophisticated
	// enough to be able to properly employ specialists.
	// Hurrying is also preferable until the cost of hurrying approaches the cost of a free colonist
	int iEuropeMinBuyCost = INT_MAX;
	
	for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		const UnitTypes eLoopUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI)));
		if (eLoopUnit != NO_UNIT)
		{
			const CvUnitInfo& kUnitInfo = GC.getUnitInfo(eLoopUnit);
		
			// Find the cheapest European unit suitable for any profession that is for sale which cannot escape
			if (kUnitInfo.getDefaultUnitAIType() == UNITAI_COLONIST && kUnitInfo.getEuropeCost() > 0 && !kUnitInfo.LbD_canEscape())
			{
				if (getEuropeUnitBuyPrice(eLoopUnit) < iEuropeMinBuyCost)
				{
					iEuropeMinBuyCost = getEuropeUnitBuyPrice(eLoopUnit);
				}
			}
		}
	}
	
	// If we didn't find a single candidate then something is really wrong!
	FAssert(iEuropeMinBuyCost != INT_MAX);

	int iEuropeMinHurryCost = INT_MAX;
	
	// Find the cost of hurrying the cheapest unit suitable for any profession that cannot escape
	for (int iIndex = 0; iIndex < GC.getDefineINT("DOCKS_NEXT_UNITS"); ++iIndex)
	{
		const int iHurryCost = getHurryGold((HurryTypes)1, iIndex);
		if (iHurryCost < iEuropeMinHurryCost)
		{
			iEuropeMinHurryCost = iHurryCost;
		}
	}
	
	FAssert(iEuropeMinHurryCost != INT_MAX);
	return (iEuropeMinHurryCost < iEuropeMinBuyCost);
}

// Returns the index of the best unit (as in purchase price of an equivalent unit) to hurry 
int CvPlayerAI::AI_getBestDockUnit() const
{
	int iBestIndex = 0;
	int iBestAdvantage = 0;
	const int iDockCount = GC.getDefineINT("DOCKS_NEXT_UNITS");
	
	// TODO: This needs to be a while loop since the indices are not stable
	for (int iIndex = 0; iIndex < iDockCount; ++iIndex)
	{
		const UnitTypes eLoopUnit = (UnitTypes)getDocksNextUnit(iIndex);
		
		// At this point there should always be a unit available
		FAssert(eLoopUnit != NO_UNIT);
		
		const int iHurryCost = getHurryGold((HurryTypes)1, iIndex);
		
		const int iUnitCost = getEuropeUnitBuyPrice(eLoopUnit);
		
		// Note: unit cost may be negative for units that are available for purchasing though
		// the absolute value will still indicate the value of the unit
		const int iHurryCostAdvantage = std::abs(iUnitCost) - iHurryCost;
		
		if (iHurryCostAdvantage > iBestAdvantage)
		{
			iBestAdvantage = iHurryCostAdvantage;
			iBestIndex = iIndex;
		}
	}
	
	return iBestIndex;
}

// Determine the units on the docks that have the largest cost\value difference and hurry them
void CvPlayerAI::AI_hurryBestDockUnits(int iHurryCount)
{
	if (AI_canHurryDockUnit() && iHurryCount > 0)
	{
		// Attempt to fill the rest of our cargo holds with hurried colonists
		for (int iIndex = 0; iIndex < iHurryCount; iIndex++)
		{
			const int index = AI_getBestDockUnit();
			
			// Note: Will be false if we can't afford it
			const bool bCanHurry = canHurry((HurryTypes)1, iIndex);
			
			if (bCanHurry)
			{
				hurry((HurryTypes)1, iIndex);
			}
			else
			{
				return;
			}
		}
	}
}

bool CvPlayerAI::AI_canHurryDockUnit() const
{
	FAssert(!isHuman());
	
	if (GC.getAI_HURRY_IMMIGRANTS() <= 0)
	{
		return false;
	}
	
	if (getParent() == NO_PLAYER)
	{
		return false;
	}
	
	if (!canTradeWithEurope())
	{
		return false;
	}
	
	return true;
}
