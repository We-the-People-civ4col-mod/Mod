#include "CvGameCoreDLL.h"

#include "CvSavegame.h"



// set the default values
const int defaultDistanceMapDistance = -1;
const int defaultAttackOddsChange = 0;
const int defaultExtraGoldTarget = 0;
const int defaultAveragesCacheTurn = -1;
const UnitTypes defaultNextBuyUnit = NO_UNIT;
const UnitAITypes defaultNextBuyUnitAI = NO_UNITAI;
const int defaultNextBuyUnitValue = 0;
const ProfessionTypes defaultNextBuyProfession = NO_PROFESSION;
const UnitTypes defaultNextBuyProfessionUnit = NO_UNIT;
const UnitAITypes defaultNextBuyProfessionAI = NO_UNITAI;
const int defaultNextBuyProfessionValue = 0;
const int defaultTotalIncome = 0;
const int defaultHurrySpending = 0;
const int defaultImmigrantSpending = 0;
const int defaultEuropeYieldSpending = 0;

const int defaultUpgradeUnitsCacheTurn = -1;
const int defaultUpgradeUnitsCachedExpThreshold = 0;
const int defaultUpgradeUnitsCachedGold = 0;
const int defaultTurnLastProductionDirty = -1;
const int defaultTurnLastManagedPop = -1;
const int defaultMoveQueuePasses = 0;
const int defaultLastWave = -1;
const int defaultWaveIndex = -1;


enum SavegameVariableTypes
{
	PlayerSaveAI_END,
	PlayerSaveAI_distanceMap,
	PlayerSaveAI_DistanceMapDistance,
	PlayerSaveAI_AttackOddsChange,
	PlayerSaveAI_ExtraGoldTarget,
	PlayerSaveAI_AveragesCacheTurn,
	PlayerSaveAI_NextBuyUnit,
	PlayerSaveAI_NextBuyUnitAI,
	PlayerSaveAI_NextBuyUnitValue,
	PlayerSaveAI_NextBuyProfession,
	PlayerSaveAI_NextBuyProfessionUnit,
	PlayerSaveAI_NextBuyProfessionAI,
	PlayerSaveAI_NextBuyProfessionValue,
	PlayerSaveAI_TotalIncome,
	PlayerSaveAI_HurrySpending,
	PlayerSaveAI_ImmigrantSpending,
	PlayerSaveAI_EuropeYieldSpending,

	PlayerSaveAI_AverageYieldMultiplier,
	PlayerSaveAI_BestWorkedYieldPlots,
	PlayerSaveAI_BestUnworkedYieldPlots,
	PlayerSaveAI_YieldValuesTimes100,
	PlayerSaveAI_NumTrainAIUnits,
	PlayerSaveAI_NumAIUnits,
	PlayerSaveAI_NumRetiredAIUnits,
	PlayerSaveAI_UnitAIStrategyWeights,
	PlayerSaveAI_PeacetimeTradeValue,
	PlayerSaveAI_PeacetimeGrantValue,
	PlayerSaveAI_GoldTradedTo,
	PlayerSaveAI_AttitudeExtra,
	PlayerSaveAI_FirstContact,
	PlayerSaveAI_ContactTimer,
	PlayerSaveAI_MemoryCount,

	PlayerSaveAI_UpgradeUnitsCacheTurn,
	PlayerSaveAI_UpgradeUnitsCachedExpThreshold,
	PlayerSaveAI_UpgradeUnitsCachedGold,
	PlayerSaveAI_TurnLastProductionDirty,
	PlayerSaveAI_TurnLastManagedPop,
	PlayerSaveAI_MoveQueuePasses,
	PlayerSaveAI_LastWave,
	PlayerSaveAI_WaveIndex,

	PlayerSaveAI_AICitySites,
	PlayerSaveAI_unitPriorityHeap,
	PlayerSaveAI_UnitClassWeights,
	PlayerSaveAI_UnitCombatWeights,
	PlayerSaveAI_CloseBordersAttitudeCache,
	PlayerSaveAI_StolenPlotsAttitudeCache,
	PlayerSaveAI_Emotions,
	PlayerSaveAI_StrategyStartedTurn,
	PlayerSaveAI_StrategyData,

	NUM_SAVE_ENUM_VALUES,
};

const char* getSavedEnumNamePlayerAi(SavegameVariableTypes eType)
{
	switch (eType)
	{
	case PlayerSaveAI_END: return "PlayerSaveAI_END";

	case PlayerSaveAI_distanceMap: return "PlayerSaveAI_distanceMap";
	case PlayerSaveAI_DistanceMapDistance: return "PlayerSaveAI_DistanceMapDistance";
	case PlayerSaveAI_AttackOddsChange: return "PlayerSaveAI_AttackOddsChange";
	case PlayerSaveAI_ExtraGoldTarget: return "PlayerSaveAI_ExtraGoldTarget";
	case PlayerSaveAI_AveragesCacheTurn: return "PlayerSaveAI_AveragesCacheTurn";
	case PlayerSaveAI_NextBuyUnit: return "PlayerSaveAI_NextBuyUnit";
	case PlayerSaveAI_NextBuyUnitAI: return "PlayerSaveAI_NextBuyUnitAI";
	case PlayerSaveAI_NextBuyUnitValue: return "PlayerSaveAI_NextBuyUnitValue";
	case PlayerSaveAI_NextBuyProfession: return "PlayerSaveAI_NextBuyProfession";
	case PlayerSaveAI_NextBuyProfessionUnit: return "PlayerSaveAI_NextBuyProfessionUnit";
	case PlayerSaveAI_NextBuyProfessionAI: return "PlayerSaveAI_NextBuyProfessionAI";
	case PlayerSaveAI_NextBuyProfessionValue: return "PlayerSaveAI_NextBuyProfessionValue";
	case PlayerSaveAI_TotalIncome: return "PlayerSaveAI_TotalIncome";
	case PlayerSaveAI_HurrySpending: return "PlayerSaveAI_HurrySpending";
	case PlayerSaveAI_ImmigrantSpending: return "PlayerSaveAI_ImmigrantSpending";
	case PlayerSaveAI_EuropeYieldSpending: return "PlayerSaveAI_EuropeYieldSpending";

	case PlayerSaveAI_AverageYieldMultiplier: return "PlayerSaveAI_AverageYieldMultiplier";
	case PlayerSaveAI_BestWorkedYieldPlots: return "PlayerSaveAI_BestWorkedYieldPlots";
	case PlayerSaveAI_BestUnworkedYieldPlots: return "PlayerSaveAI_BestUnworkedYieldPlots";
	case PlayerSaveAI_YieldValuesTimes100: return "PlayerSaveAI_YieldValuesTimes100";
	case PlayerSaveAI_NumTrainAIUnits: return "PlayerSaveAI_NumTrainAIUnits";
	case PlayerSaveAI_NumAIUnits: return "PlayerSaveAI_NumAIUnits";
	case PlayerSaveAI_NumRetiredAIUnits: return "PlayerSaveAI_NumRetiredAIUnits";
	case PlayerSaveAI_UnitAIStrategyWeights: return "PlayerSaveAI_UnitAIStrategyWeights";
	case PlayerSaveAI_PeacetimeTradeValue: return "PlayerSaveAI_PeacetimeTradeValue";
	case PlayerSaveAI_PeacetimeGrantValue: return "PlayerSaveAI_PeacetimeGrantValue";
	case PlayerSaveAI_GoldTradedTo: return "PlayerSaveAI_GoldTradedTo";
	case PlayerSaveAI_AttitudeExtra: return "PlayerSaveAI_AttitudeExtra";
	case PlayerSaveAI_FirstContact: return "PlayerSaveAI_FirstContact";
	case PlayerSaveAI_ContactTimer: return "PlayerSaveAI_ContactTimer";
	case PlayerSaveAI_MemoryCount: return "PlayerSaveAI_MemoryCount";

	case PlayerSaveAI_UpgradeUnitsCacheTurn: return "PlayerSaveAI_UpgradeUnitsCacheTurn";
	case PlayerSaveAI_UpgradeUnitsCachedExpThreshold: return "PlayerSaveAI_UpgradeUnitsCachedExpThreshold";
	case PlayerSaveAI_UpgradeUnitsCachedGold: return "PlayerSaveAI_UpgradeUnitsCachedGold";
	case PlayerSaveAI_TurnLastProductionDirty: return "PlayerSaveAI_TurnLastProductionDirty";
	case PlayerSaveAI_TurnLastManagedPop: return "PlayerSaveAI_TurnLastManagedPop";
	case PlayerSaveAI_MoveQueuePasses: return "PlayerSaveAI_MoveQueuePasses";
	case PlayerSaveAI_LastWave: return "PlayerSaveAI_LastWave";
	case PlayerSaveAI_WaveIndex: return "PlayerSaveAI_WaveIndex";

		case PlayerSaveAI_AICitySites: return "PlayerSaveAI_AICitySites";
		case PlayerSaveAI_unitPriorityHeap: return "PlayerSaveAI_unitPriorityHeap";
		case PlayerSaveAI_UnitClassWeights: return "PlayerSaveAI_UnitClassWeights";
		case PlayerSaveAI_UnitCombatWeights: return "PlayerSaveAI_UnitCombatWeights";
		case PlayerSaveAI_CloseBordersAttitudeCache: return "PlayerSaveAI_CloseBordersAttitudeCache";
		case PlayerSaveAI_StolenPlotsAttitudeCache: return "PlayerSaveAI_StolenPlotsAttitudeCache";
		case PlayerSaveAI_Emotions: return "PlayerSaveAI_Emotions";
		case PlayerSaveAI_StrategyStartedTurn: return "PlayerSaveAI_StrategyStartedTurn";
		case PlayerSaveAI_StrategyData: return "PlayerSaveAI_StrategyData";

	}
	FAssertMsg(0, "Missing case");
	return "";
}

int getNumSavedEnumValuesPlayerAI()
{
	return NUM_SAVE_ENUM_VALUES;
}

// assign everything to default values
void CvPlayerAI::AI_resetSavedData()
{
	m_distanceMap.clear();
	m_iDistanceMapDistance = defaultDistanceMapDistance;
	m_iAttackOddsChange = defaultAttackOddsChange;
	m_iExtraGoldTarget,defaultExtraGoldTarget;
	m_iAveragesCacheTurn = defaultAveragesCacheTurn;
	m_eNextBuyUnit = defaultNextBuyUnit;
	m_eNextBuyUnitAI = defaultNextBuyUnitAI;
	m_iNextBuyUnitValue = defaultNextBuyUnitValue;
	m_eNextBuyProfession = defaultNextBuyProfession;
	m_eNextBuyProfessionUnit = defaultNextBuyProfessionUnit;
	m_eNextBuyProfessionAI = defaultNextBuyProfessionAI;
	m_iNextBuyProfessionValue = defaultNextBuyProfessionValue;
	m_iTotalIncome = defaultTotalIncome;
	m_iHurrySpending = defaultHurrySpending;
	m_iImmigrantSpending = defaultImmigrantSpending;
	m_iEuropeYieldSpending = defaultEuropeYieldSpending;

	m_em_iAverageYieldMultiplier.reset();
	m_em_iBestWorkedYieldPlots.reset();
	m_em_iBestUnworkedYieldPlots.reset();
	m_em_iYieldValuesTimes100.reset();
	m_em_iNumTrainAIUnits.reset();
	m_em_iNumAIUnits.reset();
	m_em_iNumRetiredAIUnits.reset();
	m_em_iUnitAIStrategyWeights.reset();
	m_em_iPeacetimeTradeValue.reset();
	m_em_iPeacetimeGrantValue.reset();
	m_em_iGoldTradedTo.reset();
	m_em_iAttitudeExtra.reset();
	m_em_bFirstContact.reset();
	m_em_iContactTimer.reset();
	m_em_iMemoryCount.reset();

	m_iUpgradeUnitsCacheTurn = defaultUpgradeUnitsCacheTurn;
	m_iUpgradeUnitsCachedExpThreshold = defaultUpgradeUnitsCachedExpThreshold;
	m_iUpgradeUnitsCachedGold = defaultUpgradeUnitsCachedGold;
	m_iTurnLastProductionDirty = defaultTurnLastProductionDirty;
	m_iTurnLastManagedPop = defaultTurnLastManagedPop;
	m_iMoveQueuePasses = defaultMoveQueuePasses;
	m_iLastWave = defaultLastWave;
	m_iWaveIndex = defaultWaveIndex;

	m_aiAICitySites.clear();
	m_unitPriorityHeap.clear();
	m_em_iUnitClassWeights.reset();
	m_em_iUnitCombatWeights.reset();
	m_em_iCloseBordersAttitudeCache.reset();
	m_em_iStolenPlotsAttitudeCache.reset();
	m_em_iEmotions.reset();
	m_em_iStrategyStartedTurn.reset();
	m_em_iStrategyData.reset();

}

void CvPlayerAI::read(CvSavegameReader reader)
{
	reader.AssignClassType(SAVEGAME_CLASS_PLAYER_AI);

	// Init data before load
	// This will ensure that all variables not included in the savegame will have default values
	AI_resetSavedData();

	// read base class. It's always placed first
	CvPlayer::read(reader);

	// loop read all the variables
	// As long as each variable has a PlayerSavegameVariables "header", order doesn't matter.
	// Variables can be read in any order and any number of variables can be skipped.
	bool bContinue = true;
	while (bContinue)
	{
		SavegameVariableTypes eType;
		reader.Read(eType);

		switch (eType)
		{
		case PlayerSaveAI_END: bContinue = false; break;
		case PlayerSaveAI_distanceMap: reader.Read(m_distanceMap); break;
		case PlayerSaveAI_DistanceMapDistance: reader.Read(m_iDistanceMapDistance); break;
		case PlayerSaveAI_AttackOddsChange: reader.Read(m_iAttackOddsChange); break;
		case PlayerSaveAI_ExtraGoldTarget: reader.Read(m_iExtraGoldTarget); break;
		case PlayerSaveAI_AveragesCacheTurn: reader.Read(m_iAveragesCacheTurn); break;
		case PlayerSaveAI_NextBuyUnit: reader.Read(m_eNextBuyUnit); break;
		case PlayerSaveAI_NextBuyUnitAI: reader.Read(m_eNextBuyUnitAI); break;
		case PlayerSaveAI_NextBuyUnitValue: reader.Read(m_iNextBuyUnitValue); break;
		case PlayerSaveAI_NextBuyProfession: reader.Read(m_eNextBuyProfession); break;
		case PlayerSaveAI_NextBuyProfessionUnit: reader.Read(m_eNextBuyProfessionUnit); break;
		case PlayerSaveAI_NextBuyProfessionAI: reader.Read(m_eNextBuyProfessionAI); break;
		case PlayerSaveAI_NextBuyProfessionValue: reader.Read(m_iNextBuyProfessionValue); break;
		case PlayerSaveAI_TotalIncome: reader.Read(m_iTotalIncome); break;
		case PlayerSaveAI_HurrySpending: reader.Read(m_iHurrySpending); break;
		case PlayerSaveAI_ImmigrantSpending: reader.Read(m_iImmigrantSpending); break;
		case PlayerSaveAI_EuropeYieldSpending: reader.Read(m_iEuropeYieldSpending); break;
		
		case PlayerSaveAI_AverageYieldMultiplier: reader.Read(m_em_iAverageYieldMultiplier); break;
		case PlayerSaveAI_BestWorkedYieldPlots: reader.Read(m_em_iBestWorkedYieldPlots); break;
		case PlayerSaveAI_BestUnworkedYieldPlots: reader.Read(m_em_iBestUnworkedYieldPlots); break;
		case PlayerSaveAI_YieldValuesTimes100: reader.Read(m_em_iYieldValuesTimes100); break;
		case PlayerSaveAI_NumTrainAIUnits: reader.Read(m_em_iNumTrainAIUnits); break;
		case PlayerSaveAI_NumAIUnits: reader.Read(m_em_iNumAIUnits); break;
		case PlayerSaveAI_NumRetiredAIUnits: reader.Read(m_em_iNumRetiredAIUnits); break;
		case PlayerSaveAI_UnitAIStrategyWeights: reader.Read(m_em_iUnitAIStrategyWeights); break;
		case PlayerSaveAI_PeacetimeTradeValue: reader.Read(m_em_iPeacetimeTradeValue); break;
		case PlayerSaveAI_PeacetimeGrantValue: reader.Read(m_em_iPeacetimeGrantValue); break;
		case PlayerSaveAI_GoldTradedTo: reader.Read(m_em_iGoldTradedTo); break;
		case PlayerSaveAI_AttitudeExtra: reader.Read(m_em_iAttitudeExtra); break;
		case PlayerSaveAI_FirstContact: reader.Read(m_em_bFirstContact); break;
		case PlayerSaveAI_ContactTimer: reader.Read(m_em_iContactTimer); break;
		case PlayerSaveAI_MemoryCount: reader.Read(m_em_iMemoryCount); break;

		case PlayerSaveAI_UpgradeUnitsCacheTurn: reader.Read(m_iUpgradeUnitsCacheTurn); break;
		case PlayerSaveAI_UpgradeUnitsCachedExpThreshold: reader.Read(m_iUpgradeUnitsCachedExpThreshold); break;
		case PlayerSaveAI_UpgradeUnitsCachedGold: reader.Read(m_iUpgradeUnitsCachedGold); break;
		case PlayerSaveAI_TurnLastProductionDirty: reader.Read(m_iTurnLastProductionDirty); break;
		case PlayerSaveAI_TurnLastManagedPop: reader.Read(m_iTurnLastManagedPop); break;
		case PlayerSaveAI_MoveQueuePasses: reader.Read(m_iMoveQueuePasses); break;
		case PlayerSaveAI_LastWave: reader.Read(m_iLastWave); break;
		case PlayerSaveAI_WaveIndex: reader.Read(m_iWaveIndex); break;

		case PlayerSaveAI_AICitySites: reader.Read(m_aiAICitySites); break;
		case PlayerSaveAI_unitPriorityHeap: reader.Read(m_unitPriorityHeap); break;
		case PlayerSaveAI_UnitClassWeights: reader.Read(m_em_iUnitClassWeights); break;
		case PlayerSaveAI_UnitCombatWeights: reader.Read(m_em_iUnitCombatWeights); break;
		case PlayerSaveAI_CloseBordersAttitudeCache: reader.Read(m_em_iCloseBordersAttitudeCache); break;
		case PlayerSaveAI_StolenPlotsAttitudeCache: reader.Read(m_em_iStolenPlotsAttitudeCache); break;
		case PlayerSaveAI_Emotions: reader.Read(m_em_iEmotions); break;
		case PlayerSaveAI_StrategyStartedTurn: reader.Read(m_em_iStrategyStartedTurn); break;
		case PlayerSaveAI_StrategyData: reader.Read(m_em_iStrategyData); break;
		}
	}
	
	// Loading done. Set up the cache (if any).

}

void CvPlayerAI::write(CvSavegameWriter writer)
{
	writer.AssignClassType(SAVEGAME_CLASS_PLAYER_AI);

	// Write the data.
	// Use WriteSwitch since it will automatically include WriteSwitch in the savegame.
	// Also it will not save anything if the variable and the default values are identical.
	// If nothing is saved, the loading code will use the default values.
	// Less data saved/loaded means smaller savegames.

	// write base class first
	CvPlayer::write(writer);

	writer.Write(PlayerSaveAI_distanceMap, m_distanceMap);
	writer.Write(PlayerSaveAI_DistanceMapDistance, m_iDistanceMapDistance, defaultDistanceMapDistance);
	writer.Write(PlayerSaveAI_AttackOddsChange, m_iAttackOddsChange, defaultAttackOddsChange);
	writer.Write(PlayerSaveAI_ExtraGoldTarget, m_iExtraGoldTarget,defaultExtraGoldTarget);
	writer.Write(PlayerSaveAI_AveragesCacheTurn, m_iAveragesCacheTurn, defaultAveragesCacheTurn);
	writer.Write(PlayerSaveAI_NextBuyUnit, m_eNextBuyUnit, defaultNextBuyUnit);
	writer.Write(PlayerSaveAI_NextBuyUnitAI, m_eNextBuyUnitAI, defaultNextBuyUnitAI);
	writer.Write(PlayerSaveAI_NextBuyUnitValue, m_iNextBuyUnitValue, defaultNextBuyUnitValue);
	writer.Write(PlayerSaveAI_NextBuyProfession, m_eNextBuyProfession, defaultNextBuyProfession);
	writer.Write(PlayerSaveAI_NextBuyProfessionUnit, m_eNextBuyProfessionUnit, defaultNextBuyProfessionUnit);
	writer.Write(PlayerSaveAI_NextBuyProfessionAI, m_eNextBuyProfessionAI, defaultNextBuyProfessionAI);
	writer.Write(PlayerSaveAI_NextBuyProfessionValue, m_iNextBuyProfessionValue, defaultNextBuyProfessionValue);
	writer.Write(PlayerSaveAI_TotalIncome, m_iTotalIncome, defaultTotalIncome);
	writer.Write(PlayerSaveAI_HurrySpending, m_iHurrySpending, defaultHurrySpending);
	writer.Write(PlayerSaveAI_ImmigrantSpending, m_iImmigrantSpending, defaultImmigrantSpending);
	writer.Write(PlayerSaveAI_EuropeYieldSpending, m_iEuropeYieldSpending, defaultEuropeYieldSpending);

	writer.Write(PlayerSaveAI_AverageYieldMultiplier, m_em_iAverageYieldMultiplier);
	writer.Write(PlayerSaveAI_BestWorkedYieldPlots, m_em_iBestWorkedYieldPlots);
	writer.Write(PlayerSaveAI_BestUnworkedYieldPlots, m_em_iBestUnworkedYieldPlots);
	writer.Write(PlayerSaveAI_YieldValuesTimes100, m_em_iYieldValuesTimes100);
	writer.Write(PlayerSaveAI_NumTrainAIUnits, m_em_iNumTrainAIUnits);
	writer.Write(PlayerSaveAI_NumAIUnits, m_em_iNumAIUnits);
	writer.Write(PlayerSaveAI_NumRetiredAIUnits, m_em_iNumRetiredAIUnits);
	writer.Write(PlayerSaveAI_UnitAIStrategyWeights, m_em_iUnitAIStrategyWeights);
	writer.Write(PlayerSaveAI_PeacetimeTradeValue, m_em_iPeacetimeTradeValue);
	writer.Write(PlayerSaveAI_PeacetimeGrantValue, m_em_iPeacetimeGrantValue);
	writer.Write(PlayerSaveAI_GoldTradedTo, m_em_iGoldTradedTo);
	writer.Write(PlayerSaveAI_AttitudeExtra, m_em_iAttitudeExtra);
	writer.Write(PlayerSaveAI_FirstContact, m_em_bFirstContact);
	writer.Write(PlayerSaveAI_ContactTimer, m_em_iContactTimer);
	writer.Write(PlayerSaveAI_MemoryCount, m_em_iMemoryCount);

	writer.Write(PlayerSaveAI_UpgradeUnitsCacheTurn, m_iUpgradeUnitsCacheTurn, defaultUpgradeUnitsCacheTurn);
	writer.Write(PlayerSaveAI_UpgradeUnitsCachedExpThreshold, m_iUpgradeUnitsCachedExpThreshold, defaultUpgradeUnitsCachedExpThreshold);
	writer.Write(PlayerSaveAI_UpgradeUnitsCachedGold, m_iUpgradeUnitsCachedGold, defaultUpgradeUnitsCachedGold);
	writer.Write(PlayerSaveAI_TurnLastProductionDirty, m_iTurnLastProductionDirty, defaultTurnLastProductionDirty);
	writer.Write(PlayerSaveAI_TurnLastManagedPop, m_iTurnLastManagedPop, defaultTurnLastManagedPop);
	writer.Write(PlayerSaveAI_MoveQueuePasses, m_iMoveQueuePasses, defaultMoveQueuePasses);
	writer.Write(PlayerSaveAI_LastWave, m_iLastWave, defaultLastWave);
	writer.Write(PlayerSaveAI_WaveIndex, m_iWaveIndex, defaultWaveIndex);

	writer.Write(PlayerSaveAI_AICitySites, m_aiAICitySites);
	writer.Write(PlayerSaveAI_unitPriorityHeap, m_unitPriorityHeap);
	writer.Write(PlayerSaveAI_UnitClassWeights, m_em_iUnitClassWeights);
	writer.Write(PlayerSaveAI_UnitCombatWeights, m_em_iUnitCombatWeights);
	writer.Write(PlayerSaveAI_CloseBordersAttitudeCache, m_em_iCloseBordersAttitudeCache);
	writer.Write(PlayerSaveAI_StolenPlotsAttitudeCache, m_em_iStolenPlotsAttitudeCache);
	writer.Write(PlayerSaveAI_Emotions, m_em_iEmotions);
	writer.Write(PlayerSaveAI_StrategyStartedTurn, m_em_iStrategyStartedTurn);
	writer.Write(PlayerSaveAI_StrategyData, m_em_iStrategyData);
	

	writer.Write(PlayerSaveAI_END);
}
