#include "CvGameCoreDLL.h"

#include "CvSavegame.h"

// set the default values
const UnitTypes defaultUnitType = NO_UNIT;
const DirectionTypes defaultFacingDirection = DIRECTION_SOUTH;
const int defaultID = 0;
const int defaultGroupID = FFreeList::INVALID_INDEX;
const int defaultHotKeyNumber = -1;
const int defaultX = INVALID_PLOT_COORD;
const int defaultY = INVALID_PLOT_COORD;
const int defaultLastMoveTurn = 0;
const int defaultGameTurnCreated = 0;
const int defaultDamage = 0;
const int defaultMoves = 0;
const int defaultExperience = 0;
const int defaultLevel = 1;
const int defaultCargo = 0;
const int defaultAttackPlotX = INVALID_PLOT_COORD;
const int defaultAttackPlotY = INVALID_PLOT_COORD;
const int defaultCombatTimer = 0;
const int defaultCombatDamage = 0;
const int defaultFortifyTurns = 0;

const int defaultExtraVisibilityRange = 0;

const int defaultImmobileTimer = 0;
const int defaultYieldStored = 0;
const int defaultExtraWorkRate = 0;
const int defaultUnitTravelTimer = 0;
const int defaultBadCityDefenderCount = 0;
const int defaultPostCombatPlotIndex = -1;
const int defaultLbDrounds = 0;
const int defaultLbDroundsBefore = 0;
const int defaultAmountForNativeTrade = 0;
const int defaultMoneyToBuyLand = 0;

const bool defaultMadeAttack = false;
const bool defaultPromotionReady = false;
const bool defaultDeathDelay = false;
const bool defaultCombatFocus = false;
const bool defaultColonistLocked = false;
const bool defaultGatheringResource = false;
const bool defaultIgnoreDanger = false;
const bool defaultBarbarian = false;

const ProfessionTypes defaultProfession = NO_PROFESSION;
const ProfessionTypes defaultLastProfession = NO_PROFESSION;
const ProfessionTypes defaultLastProfessionBefore = NO_PROFESSION;
const PlayerTypes defaultPlayerToBuyLand = NO_PLAYER;
const YieldTypes defaultYieldForNativeTrade = NO_YIELD;
const UnitTravelStates defaultUnitTravelState = NO_UNIT_TRAVEL_STATE;
const PlayerTypes defaultOwner = NO_PLAYER;
const PlayerTypes defaultCapturingPlayer = NO_PLAYER;
const UnitTypes defaultLeaderUnitType =  NO_UNIT;



// add to the end for backward savegame compatibility
enum SavegameVariableTypes
{
	UnitSave_END,
	UnitSave_Direction,
	UnitSave_UnitType,
	UnitSave_ID,
	UnitSave_GroupID,
	UnitSave_HotKeyNumber,
	UnitSave_X,
	UnitSave_Y,
	UnitSave_LastMoveTurn,
	UnitSave_GameTurnCreated,
	UnitSave_Damage,
	UnitSave_Moves,
	UnitSave_Experience,
	UnitSave_Level,
	UnitSave_Cargo,
	UnitSave_AttackPlotX,
	UnitSave_AttackPlotY,
	UnitSave_CombatTimer,
	UnitSave_CombatDamage,
	UnitSave_FortifyTurns,
	
	UnitSave_ExtraVisibilityRange,
	
	UnitSave_ImmobileTimer,
	UnitSave_YieldStored,
	UnitSave_ExtraWorkRate, // not used
	UnitSave_UnitTravelTimer,
	UnitSave_BadCityDefenderCount, // not used
	UnitSave_PostCombatPlotIndex,
	UnitSave_LbDrounds,
	UnitSave_AmountForNativeTrade,
	UnitSave_MoneyToBuyLand,

	UnitSave_MadeAttack,
	UnitSave_PromotionReady,
	UnitSave_DeathDelay,
	UnitSave_CombatFocus,
	UnitSave_ColonistLocked,
	UnitSave_GatheringResource,
	UnitSave_IgnoreDanger,
	UnitSave_Barbarian,

	UnitSave_Profession,
	UnitSave_LastProfession,
	UnitSave_PlayerToBuyLand,
	UnitSave_YieldForNativeTrade,
	UnitSave_UnitTravelState,
	UnitSave_Owner,
	UnitSave_CapturingPlayer,
	UnitSave_LeaderUnitType,

	UnitSave_combatUnit,
	UnitSave_transportUnit,
	UnitSave_homeCity,

	UnitSave_Name,
	UnitSave_ScriptData,

	UnitSave_HasRealPromotion,
	UnitSave_FreePromotionCount,

	UnitSave_LbDroundsBefore,
	UnitSave_LastProfessionBefore,

	NUM_SAVE_ENUM_VALUES,
};

const char* getSavedEnumNameUnit(SavegameVariableTypes eType)
{
	switch (eType)
	{
	case UnitSave_END: return "UnitSave_END";
	case UnitSave_Direction: return "UnitSave_Direction";
	case UnitSave_UnitType: return "UnitSave_UnitType";
	case UnitSave_ID: return "UnitSave_ID";
	case UnitSave_GroupID: return "UnitSave_GroupID";
	case UnitSave_HotKeyNumber: return "UnitSave_HotKeyNumber";
	case UnitSave_X: return "UnitSave_X";
	case UnitSave_Y: return "UnitSave_Y";
	case UnitSave_LastMoveTurn: return "UnitSave_LastMoveTurn";
	case UnitSave_GameTurnCreated: return "UnitSave_GameTurnCreated";
	case UnitSave_Damage: return "UnitSave_Damage";
	case UnitSave_Moves: return "UnitSave_Moves";
	case UnitSave_Experience: return "UnitSave_Experience";
	case UnitSave_Level: return "UnitSave_Level";
	case UnitSave_Cargo: return "UnitSave_Cargo";
	case UnitSave_AttackPlotX: return "UnitSave_AttackPlotX";
	case UnitSave_AttackPlotY: return "UnitSave_AttackPlotY";
	case UnitSave_CombatTimer: return "UnitSave_CombatTimer";
	case UnitSave_CombatDamage: return "UnitSave_CombatDamage";
	case UnitSave_FortifyTurns: return "UnitSave_FortifyTurns";

	case UnitSave_ExtraVisibilityRange: return "UnitSave_ExtraVisibilityRange";
	
	case UnitSave_ImmobileTimer: return "UnitSave_ImmobileTimer";
	case UnitSave_YieldStored: return "UnitSave_YieldStored";
	case UnitSave_ExtraWorkRate: return "UnitSave_ExtraWorkRate";
	case UnitSave_UnitTravelTimer: return "UnitSave_UnitTravelTimer";
	case UnitSave_BadCityDefenderCount: return "UnitSave_BadCityDefenderCount";
	case UnitSave_PostCombatPlotIndex: return "UnitSave_PostCombatPlotIndex";
	case UnitSave_LbDrounds: return "UnitSave_LbDrounds";
	case UnitSave_LbDroundsBefore: return "UnitSave_LbDroundsBefore";
	case UnitSave_AmountForNativeTrade: return "UnitSave_AmountForNativeTrade";
	case UnitSave_MoneyToBuyLand: return "UnitSave_MoneyToBuyLand";

	case UnitSave_MadeAttack: return "UnitSave_MadeAttack";
	case UnitSave_PromotionReady: return "UnitSave_PromotionReady";
	case UnitSave_DeathDelay: return "UnitSave_DeathDelay";
	case UnitSave_CombatFocus: return "UnitSave_CombatFocus";
	case UnitSave_ColonistLocked: return "UnitSave_ColonistLocked";
	case UnitSave_GatheringResource: return "UnitSave_GatheringResource";
	case UnitSave_IgnoreDanger: return "UnitSave_IgnoreDanger";
	case UnitSave_Barbarian: return "UnitSave_Barbarian";

	case UnitSave_Profession: return "UnitSave_Profession";
	case UnitSave_LastProfession: return "UnitSave_LastProfession";
	case UnitSave_LastProfessionBefore: return "UnitSave_LastProfessionBefore";
	case UnitSave_PlayerToBuyLand: return "UnitSave_PlayerToBuyLand";
	case UnitSave_YieldForNativeTrade: return "UnitSave_YieldForNativeTrade";
	case UnitSave_UnitTravelState: return "UnitSave_UnitTravelState";
	case UnitSave_Owner: return "UnitSave_Owner";
	case UnitSave_CapturingPlayer: return "UnitSave_CapturingPlayer";
	case UnitSave_LeaderUnitType: return "UnitSave_LeaderUnitType";

	case UnitSave_combatUnit: return "UnitSave_combatUnit";
	case UnitSave_transportUnit: return "UnitSave_transportUnit";
	case UnitSave_homeCity: return "UnitSave_homeCity";

	case UnitSave_Name: return "UnitSave_Name";
	case UnitSave_ScriptData: return "UnitSave_ScriptData";

	case UnitSave_HasRealPromotion: return "UnitSave_HasRealPromotion";
	case UnitSave_FreePromotionCount: return "UnitSave_FreePromotionCount";
}
	return "";
}

int getNumSavedEnumValuesUnit()
{
	return NUM_SAVE_ENUM_VALUES;
}

// assign everything to default values
void CvUnit::resetSavedData(int iID, UnitTypes eUnit, PlayerTypes eOwner, bool bConstructorCall)
{
	m_eFacingDirection = defaultFacingDirection;
	m_eUnitType = eUnit;
	m_pUnitInfo = (NO_UNIT != m_eUnitType) ? &GC.getUnitInfo(m_eUnitType) : NULL;

	m_iID = iID;
	m_iGroupID = defaultGroupID;
	m_iHotKeyNumber = defaultHotKeyNumber;
	m_iX = defaultX;
	m_iY = defaultY;
	m_iLastMoveTurn = defaultLastMoveTurn;
	m_iGameTurnCreated = defaultGameTurnCreated;
	m_iDamage = defaultDamage;
	m_iMoves = defaultMoves;
	m_iExperience = defaultExperience;
	m_iLevel = defaultLevel;
	m_iCargo = defaultCargo;
	m_iAttackPlotX = defaultAttackPlotX;
	m_iAttackPlotY = defaultAttackPlotY;
	m_iCombatTimer = defaultCombatTimer;
	m_iCombatDamage = defaultCombatDamage;
	m_iFortifyTurns = defaultFortifyTurns;
	
	m_iImmobileTimer = defaultImmobileTimer;
	m_iYieldStored = defaultYieldStored;
	m_iExtraWorkRate = defaultExtraWorkRate;
	m_iUnitTravelTimer = defaultUnitTravelTimer;
	m_iBadCityDefenderCount = defaultBadCityDefenderCount;
	m_iPostCombatPlotIndex = defaultPostCombatPlotIndex;
	m_iLbDrounds = defaultLbDrounds;
	m_iLbDroundsBefore = defaultLbDroundsBefore;
	m_iAmountForNativeTrade = defaultAmountForNativeTrade;
	m_iMoneyToBuyLand = defaultMoneyToBuyLand;

	m_bMadeAttack = defaultMadeAttack;	
	m_bPromotionReady = defaultPromotionReady;	
	m_bDeathDelay = defaultDeathDelay;	
	m_bCombatFocus = defaultCombatFocus;
	m_bColonistLocked = defaultColonistLocked;
	m_bGatheringResource = defaultGatheringResource;
	m_bIgnoreDanger = defaultIgnoreDanger;
	m_bBarbarian = defaultBarbarian;

	m_eProfession = defaultProfession;
	m_eLastProfession = defaultLastProfession;
	m_eLastProfessionBefore = defaultLastProfessionBefore;
	m_ePlayerToBuyLand = defaultPlayerToBuyLand;
	m_eYieldForNativeTrade = defaultYieldForNativeTrade;
	m_eUnitTravelState = defaultUnitTravelState;
	m_eOwner = eOwner;
	m_eCapturingPlayer = defaultCapturingPlayer;
	m_eLeaderUnitType = defaultLeaderUnitType;

	m_combatUnit.reset();
	m_transportUnit.reset();
	m_homeCity.reset();

	m_szName.clear();
	m_szScriptData.clear();

	m_ba_HasRealPromotion.reset();
	m_ja_iFreePromotionCount.reset();

 	resetPromotions();

}

void CvUnit::read(CvSavegameReader reader)
{
	reader.AssignClassType(SAVEGAME_CLASS_UNIT);

	// Init data before load
	// This will ensure that all variables not included in the savegame will have default values
	reset();

	// loop read all the variables
	// As long as each variable has a UnitSavegameVariables "header", order doesn't matter.
	// Variables can be read in any order and any number of variables can be skipped.
	bool bContinue = true;
	while (bContinue)
	{
		SavegameVariableTypes eType;
		reader.Read(eType);

		switch (eType)
		{
		case UnitSave_END: bContinue = false; break;

		case UnitSave_Direction: reader.Read(m_eFacingDirection); break;
		case UnitSave_UnitType: reader.Read(m_eUnitType); break;

		case UnitSave_ID: reader.Read(m_iID); break;
		case UnitSave_GroupID: reader.Read(m_iGroupID); break;
		case UnitSave_HotKeyNumber: reader.Read(m_iHotKeyNumber); break;
		case UnitSave_X: reader.Read(m_iX); break;
		case UnitSave_Y: reader.Read(m_iY); break;
		case UnitSave_LastMoveTurn: reader.Read(m_iLastMoveTurn); break;
		case UnitSave_GameTurnCreated: reader.Read(m_iGameTurnCreated); break;
		case UnitSave_Damage: reader.Read(m_iDamage); break;
		case UnitSave_Moves: reader.Read(m_iMoves); break;
		case UnitSave_Experience: reader.Read(m_iExperience); break;
		case UnitSave_Level: reader.Read(m_iLevel); break;
		case UnitSave_Cargo: reader.Read(m_iCargo); break;
		case UnitSave_AttackPlotX: reader.Read(m_iAttackPlotX); break;
		case UnitSave_AttackPlotY: reader.Read(m_iAttackPlotY); break;
		case UnitSave_CombatTimer: reader.Read(m_iCombatTimer); break;
		case UnitSave_CombatDamage: reader.Read(m_iCombatDamage); break;
		case UnitSave_FortifyTurns: reader.Read(m_iFortifyTurns); break;

		case UnitSave_ExtraVisibilityRange: 
		{
			//reader.Read(m_iExtraVisibilityRange); break;
			int iTemp = 0;
			reader.Read(iTemp);
			break;
		}
		case UnitSave_ImmobileTimer: reader.Read(m_iImmobileTimer); break;
		case UnitSave_YieldStored: reader.Read(m_iYieldStored); break;
		case UnitSave_ExtraWorkRate: reader.Discard<int>(); break;
		case UnitSave_UnitTravelTimer: reader.Read(m_iUnitTravelTimer); break;
		case UnitSave_BadCityDefenderCount: reader.Discard<int>(); break;
		case UnitSave_PostCombatPlotIndex: reader.Read(m_iPostCombatPlotIndex); break;
		case UnitSave_LbDrounds: reader.Read(m_iLbDrounds); break;
		case UnitSave_LbDroundsBefore: reader.Read(m_iLbDroundsBefore); break;
		case UnitSave_AmountForNativeTrade: reader.Read(m_iAmountForNativeTrade); break;
		case UnitSave_MoneyToBuyLand: reader.Read(m_iMoneyToBuyLand); break;

		case UnitSave_MadeAttack: reader.Read(m_bMadeAttack); break;
		case UnitSave_PromotionReady: reader.Read(m_bPromotionReady); break;
		case UnitSave_DeathDelay: reader.Read(m_bDeathDelay); break;
		case UnitSave_CombatFocus: reader.Read(m_bCombatFocus); break;
		case UnitSave_ColonistLocked: reader.Read(m_bColonistLocked); break;
		case UnitSave_GatheringResource: reader.Read(m_bGatheringResource); break;
		case UnitSave_IgnoreDanger: reader.Read(m_bIgnoreDanger); break;
		case UnitSave_Barbarian: reader.Read(m_bBarbarian); break;

		case UnitSave_Profession: reader.Read(m_eProfession); break;
		case UnitSave_LastProfession: reader.Read(m_eLastProfession); break;
		case UnitSave_LastProfessionBefore: reader.Read(m_eLastProfessionBefore); break;
		case UnitSave_PlayerToBuyLand: reader.Read(m_ePlayerToBuyLand); break;
		case UnitSave_YieldForNativeTrade: reader.Read(m_eYieldForNativeTrade); break;
		case UnitSave_UnitTravelState: reader.Read(m_eUnitTravelState); break;
		case UnitSave_Owner: reader.Read(m_eOwner); break;
		case UnitSave_CapturingPlayer: reader.Read(m_eCapturingPlayer); break;
		case UnitSave_LeaderUnitType: reader.Read(m_eLeaderUnitType); break;

		case UnitSave_combatUnit: reader.Read(m_combatUnit); break;
		case UnitSave_transportUnit: reader.Read(m_transportUnit); break;
		case UnitSave_homeCity: reader.Read(m_homeCity); break;

		case UnitSave_Name: reader.Read(m_szName); break;
		case UnitSave_ScriptData: reader.Read(m_szScriptData); break;

		case UnitSave_HasRealPromotion: reader.Read(m_ba_HasRealPromotion); break;
		case UnitSave_FreePromotionCount: reader.Read(m_ja_iFreePromotionCount); break;
		}
	}
	
	// The unit is loaded. Now set up the cache according to the read data.

	FAssert(NO_UNIT != m_eUnitType);
	m_pUnitInfo = &GC.getUnitInfo(m_eUnitType);
	m_iBaseCombat = m_pUnitInfo->getCombat();

	// unit yield cache - start - Nightinggale
	updateYieldCache();
	// unit yield cache - end - Nightinggale

	// clear garbage from the savegame
	// due to the visibility range chache in CvPlot, this can't be done by simply not saving the data 
	resetPromotions();

	// update profession/promotion cache
	if (getProfession() != NO_PROFESSION)
	{
		processProfessionStatsUnsaved(GC.getProfessionInfo(getProfession()), 1);
	}
	setPromotions();


}

void CvUnit::write(CvSavegameWriter writer)
{
	writer.AssignClassType(SAVEGAME_CLASS_UNIT);

	// Write the data.
	// Use WriteSwitch since it will automatically include WriteSwitch in the savegame.
	// Also it will not save anything if the variable and the default values are identical.
	// If nothing is saved, the loading code will use the default values.
	// Less data saved/loaded means smaller savegames.

	writer.Write(UnitSave_Direction, m_eFacingDirection, defaultFacingDirection);
	writer.Write(UnitSave_UnitType, m_eUnitType, defaultUnitType);
	writer.Write(UnitSave_ID, m_iID, defaultID);
	writer.Write(UnitSave_GroupID, m_iGroupID, defaultGroupID);
	writer.Write(UnitSave_HotKeyNumber, m_iHotKeyNumber, defaultHotKeyNumber);
	writer.Write(UnitSave_X, m_iX, defaultX);
	writer.Write(UnitSave_Y, m_iY, defaultY);
	writer.Write(UnitSave_LastMoveTurn, m_iLastMoveTurn, defaultLastMoveTurn);
	writer.Write(UnitSave_GameTurnCreated, m_iGameTurnCreated, defaultGameTurnCreated);
	writer.Write(UnitSave_Damage, m_iDamage, defaultDamage);
	writer.Write(UnitSave_Moves, m_iMoves, defaultMoves);
	writer.Write(UnitSave_Experience, m_iExperience, defaultExperience);
	writer.Write(UnitSave_Level, m_iLevel, defaultLevel);
	writer.Write(UnitSave_Cargo, m_iCargo, defaultCargo);
	writer.Write(UnitSave_AttackPlotX, m_iAttackPlotX, defaultAttackPlotX);
	writer.Write(UnitSave_AttackPlotY, m_iAttackPlotY, defaultAttackPlotY);
	writer.Write(UnitSave_CombatTimer, m_iCombatTimer, defaultCombatTimer);
	writer.Write(UnitSave_CombatDamage, m_iCombatDamage, defaultCombatDamage);
	writer.Write(UnitSave_FortifyTurns, m_iFortifyTurns, defaultFortifyTurns);

	//writer.Write(UnitSave_ExtraVisibilityRange, m_iExtraVisibilityRange, defaultExtraVisibilityRange);

	writer.Write(UnitSave_ImmobileTimer, m_iImmobileTimer, defaultImmobileTimer);
	writer.Write(UnitSave_YieldStored, m_iYieldStored, defaultYieldStored);
	writer.Write(UnitSave_UnitTravelTimer, m_iUnitTravelTimer, defaultUnitTravelTimer);
	writer.Write(UnitSave_PostCombatPlotIndex, m_iPostCombatPlotIndex, defaultPostCombatPlotIndex);

	writer.Write(UnitSave_LbDrounds, m_iLbDrounds, defaultLbDrounds);
	writer.Write(UnitSave_LbDroundsBefore, m_iLbDroundsBefore, defaultLbDroundsBefore);
	writer.Write(UnitSave_AmountForNativeTrade, m_iAmountForNativeTrade, defaultAmountForNativeTrade);
	writer.Write(UnitSave_MoneyToBuyLand, m_iMoneyToBuyLand, defaultMoneyToBuyLand);

	writer.Write(UnitSave_MadeAttack, m_bMadeAttack, defaultMadeAttack);
	writer.Write(UnitSave_PromotionReady, m_bPromotionReady, defaultPromotionReady);
	writer.Write(UnitSave_DeathDelay, m_bDeathDelay, defaultDeathDelay);
	writer.Write(UnitSave_CombatFocus, m_bCombatFocus, defaultCombatFocus);
	writer.Write(UnitSave_ColonistLocked, m_bColonistLocked, defaultColonistLocked);
	writer.Write(UnitSave_GatheringResource, m_bGatheringResource, defaultGatheringResource);
	writer.Write(UnitSave_IgnoreDanger, m_bIgnoreDanger, defaultIgnoreDanger);
	writer.Write(UnitSave_Barbarian, m_bBarbarian, defaultBarbarian);

	writer.Write(UnitSave_Profession, m_eProfession, defaultProfession);
	writer.Write(UnitSave_LastProfession, m_eLastProfession, defaultLastProfession);
	writer.Write(UnitSave_LastProfessionBefore, m_eLastProfessionBefore, defaultLastProfessionBefore);
	writer.Write(UnitSave_PlayerToBuyLand, m_ePlayerToBuyLand, defaultPlayerToBuyLand);
	writer.Write(UnitSave_YieldForNativeTrade, m_eYieldForNativeTrade, defaultYieldForNativeTrade);
	writer.Write(UnitSave_UnitTravelState, m_eUnitTravelState, defaultUnitTravelState);
	writer.Write(UnitSave_Owner, m_eOwner, defaultOwner);
	writer.Write(UnitSave_CapturingPlayer, m_eCapturingPlayer, defaultCapturingPlayer);
	writer.Write(UnitSave_LeaderUnitType, m_eLeaderUnitType, defaultLeaderUnitType);

	writer.Write(UnitSave_combatUnit, m_combatUnit);
	writer.Write(UnitSave_transportUnit, m_transportUnit);
	writer.Write(UnitSave_homeCity, m_homeCity);

	writer.Write(UnitSave_Name, m_szName);
	writer.Write(UnitSave_ScriptData, m_szScriptData);

	writer.Write(UnitSave_HasRealPromotion, m_ba_HasRealPromotion);
	writer.Write(UnitSave_FreePromotionCount, m_ja_iFreePromotionCount);

	writer.Write(UnitSave_END);
}
