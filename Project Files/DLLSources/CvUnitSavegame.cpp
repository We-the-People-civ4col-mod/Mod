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
const int defaultCargoCapacity =  0;
const int defaultAttackPlotX = INVALID_PLOT_COORD;
const int defaultAttackPlotY = INVALID_PLOT_COORD;
const int defaultCombatTimer = 0;
const int defaultCombatDamage = 0;
const int defaultFortifyTurns = 0;
const int defaultBlitzCount = 0;
const int defaultAmphibCount = 0;
const int defaultRiverCount = 0;
const int defaultEnemyRouteCount = 0;
const int defaultAlwaysHealCount = 0;
const int defaultHillsDoubleMoveCount = 0;
const int defaultExtraVisibilityRange = 0;
const int defaultExtraMoves = 0;
const int defaultExtraMoveDiscount = 0;
const int defaultExtraWithdrawal = 0;
const int defaultExtraBombardRate = 0;
const int defaultExtraEnemyHeal = 0;
const int defaultExtraNeutralHeal = 0;
const int defaultExtraFriendlyHeal = 0;
const int defaultSameTileHeal = 0;
const int defaultAdjacentTileHeal = 0;
const int defaultExtraCombatPercent = 0;
const int defaultExtraCityAttackPercent = 0;
const int defaultExtraCityDefensePercent = 0;
const int defaultExtraHillsAttackPercent = 0;
const int defaultExtraHillsDefensePercent = 0;
const int defaultExtraDomesticBonusPercent = 0;	
const int defaultPillageChange = 0;
const int defaultUpgradeDiscount = 0;
const int defaultExperiencePercent = 0;
const int defaultBaseCombat = 0;
const int defaultImmobileTimer = 0;
const int defaultYieldStored = 0;
const int defaultExtraWorkRate = 0;
const int defaultUnitTravelTimer = 0;
const int defaultBadCityDefenderCount = 0;
const int defaultUnarmedCount = 0;
const int defaultPostCombatPlotIndex = -1;
// 
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
	UnitSave_CargoCapacity,
	UnitSave_AttackPlotX,
	UnitSave_AttackPlotY,
	UnitSave_CombatTimer,
	UnitSave_CombatDamage,
	UnitSave_FortifyTurns,
	UnitSave_BlitzCount,
	UnitSave_AmphibCount,
	UnitSave_RiverCount,
	UnitSave_EnemyRouteCount,
	UnitSave_AlwaysHealCount,
	UnitSave_HillsDoubleMoveCount,
	UnitSave_ExtraVisibilityRange,
	UnitSave_ExtraMoves,
	UnitSave_ExtraMoveDiscount,
	UnitSave_ExtraWithdrawal,
	UnitSave_ExtraBombardRate,
	UnitSave_ExtraEnemyHeal,
	UnitSave_ExtraNeutralHeal,
	UnitSave_ExtraFriendlyHeal,
	UnitSave_SameTileHeal,
	UnitSave_AdjacentTileHeal,
	UnitSave_ExtraCombatPercent,
	UnitSave_ExtraCityAttackPercent,
	UnitSave_ExtraCityDefensePercent,
	UnitSave_ExtraHillsAttackPercent,
	UnitSave_ExtraHillsDefensePercent,
	UnitSave_ExtraDomesticBonusPercent,	
	UnitSave_PillageChange,
	UnitSave_UpgradeDiscount,
	UnitSave_ExperiencePercent,
	UnitSave_BaseCombat,
	UnitSave_ImmobileTimer,
	UnitSave_YieldStored,
	UnitSave_ExtraWorkRate,
	UnitSave_UnitTravelTimer,
	UnitSave_BadCityDefenderCount,
	UnitSave_UnarmedCount,
	UnitSave_PostCombatPlotIndex,
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
	case UnitSave_CargoCapacity: return "UnitSave_CargoCapacity";
	case UnitSave_AttackPlotX: return "UnitSave_AttackPlotX";
	case UnitSave_AttackPlotY: return "UnitSave_AttackPlotY";
	case UnitSave_CombatTimer: return "UnitSave_CombatTimer";
	case UnitSave_CombatDamage: return "UnitSave_CombatDamage";
	case UnitSave_FortifyTurns: return "UnitSave_FortifyTurns";
	case UnitSave_BlitzCount: return "UnitSave_BlitzCount";
	case UnitSave_AmphibCount: return "UnitSave_AmphibCount";
	case UnitSave_RiverCount: return "UnitSave_RiverCount";
	case UnitSave_EnemyRouteCount: return "UnitSave_EnemyRouteCount";
	case UnitSave_AlwaysHealCount: return "UnitSave_AlwaysHealCount";
	case UnitSave_HillsDoubleMoveCount: return "UnitSave_HillsDoubleMoveCount";
	case UnitSave_ExtraVisibilityRange: return "UnitSave_ExtraVisibilityRange";
	case UnitSave_ExtraMoves: return "UnitSave_ExtraMoves";
	case UnitSave_ExtraMoveDiscount: return "UnitSave_ExtraMoveDiscount";
	case UnitSave_ExtraWithdrawal: return "UnitSave_ExtraWithdrawal";
	case UnitSave_ExtraBombardRate: return "UnitSave_ExtraBombardRate";
	case UnitSave_ExtraEnemyHeal: return "UnitSave_ExtraEnemyHeal";
	case UnitSave_ExtraNeutralHeal: return "UnitSave_ExtraNeutralHeal";
	case UnitSave_ExtraFriendlyHeal: return "UnitSave_ExtraFriendlyHeal";
	case UnitSave_SameTileHeal: return "UnitSave_SameTileHeal";
	case UnitSave_AdjacentTileHeal: return "UnitSave_AdjacentTileHeal";
	case UnitSave_ExtraCombatPercent: return "UnitSave_ExtraCombatPercent";
	case UnitSave_ExtraCityAttackPercent: return "UnitSave_ExtraCityAttackPercent";
	case UnitSave_ExtraCityDefensePercent: return "UnitSave_ExtraCityDefensePercent";
	case UnitSave_ExtraHillsAttackPercent: return "UnitSave_ExtraHillsAttackPercent";
	case UnitSave_ExtraHillsDefensePercent: return "UnitSave_ExtraHillsDefensePercent";
	case UnitSave_ExtraDomesticBonusPercent: return "UnitSave_ExtraDomesticBonusPercent";
	case UnitSave_PillageChange: return "UnitSave_PillageChange";
	case UnitSave_UpgradeDiscount: return "UnitSave_UpgradeDiscount";
	case UnitSave_ExperiencePercent: return "UnitSave_ExperiencePercent";
	case UnitSave_BaseCombat: return "UnitSave_BaseCombat";
	case UnitSave_ImmobileTimer: return "UnitSave_ImmobileTimer";
	case UnitSave_YieldStored: return "UnitSave_YieldStored";
	case UnitSave_ExtraWorkRate: return "UnitSave_ExtraWorkRate";
	case UnitSave_UnitTravelTimer: return "UnitSave_UnitTravelTimer";
	case UnitSave_BadCityDefenderCount: return "UnitSave_BadCityDefenderCount";
	case UnitSave_UnarmedCount: return "UnitSave_UnarmedCount";
	case UnitSave_PostCombatPlotIndex: return "UnitSave_PostCombatPlotIndex";
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
	m_eUnitType = defaultUnitType;
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
	m_iBlitzCount = defaultBlitzCount;
	m_iAmphibCount = defaultAmphibCount;
	m_iRiverCount = defaultRiverCount;
	m_iEnemyRouteCount = defaultEnemyRouteCount;
	m_iAlwaysHealCount = defaultAlwaysHealCount;
	m_iHillsDoubleMoveCount = defaultHillsDoubleMoveCount;
	m_iExtraVisibilityRange = defaultExtraVisibilityRange;
	m_iExtraMoves = defaultExtraMoves;
	m_iExtraMoveDiscount = defaultExtraMoveDiscount;
	m_iExtraWithdrawal = defaultExtraWithdrawal;
	m_iExtraBombardRate = defaultExtraBombardRate;
	m_iExtraEnemyHeal = defaultExtraEnemyHeal;
	m_iExtraNeutralHeal = defaultExtraNeutralHeal;
	m_iExtraFriendlyHeal = defaultExtraFriendlyHeal;
	m_iSameTileHeal = defaultSameTileHeal;
	m_iAdjacentTileHeal = defaultAdjacentTileHeal;
	m_iExtraCombatPercent = defaultExtraCombatPercent;
	m_iExtraCityAttackPercent = defaultExtraCityAttackPercent;
	m_iExtraCityDefensePercent = defaultExtraCityDefensePercent;
	m_iExtraHillsAttackPercent = defaultExtraHillsAttackPercent;
	m_iExtraHillsDefensePercent = defaultExtraHillsDefensePercent;
	m_iExtraDomesticBonusPercent = defaultExtraDomesticBonusPercent;	
	m_iPillageChange = defaultPillageChange;
	m_iUpgradeDiscount = defaultUpgradeDiscount;
	m_iExperiencePercent = defaultExperiencePercent;
	m_iBaseCombat = defaultBaseCombat;
	m_iImmobileTimer = defaultImmobileTimer;
	m_iYieldStored = defaultYieldStored;
	m_iExtraWorkRate = defaultExtraWorkRate;
	m_iUnitTravelTimer = defaultUnitTravelTimer;
	m_iBadCityDefenderCount = defaultBadCityDefenderCount;
	m_iUnarmedCount = defaultUnarmedCount;
	m_iPostCombatPlotIndex = defaultPostCombatPlotIndex;

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
		case UnitSave_CargoCapacity: reader.Read(m_iCargoCapacity); break;
		case UnitSave_AttackPlotX: reader.Read(m_iAttackPlotX); break;
		case UnitSave_AttackPlotY: reader.Read(m_iAttackPlotY); break;
		case UnitSave_CombatTimer: reader.Read(m_iCombatTimer); break;
		case UnitSave_CombatDamage: reader.Read(m_iCombatDamage); break;
		case UnitSave_FortifyTurns: reader.Read(m_iFortifyTurns); break;
		case UnitSave_BlitzCount: reader.Read(m_iBlitzCount); break;
		case UnitSave_AmphibCount: reader.Read(m_iAmphibCount); break;
		case UnitSave_RiverCount: reader.Read(m_iRiverCount); break;
		case UnitSave_EnemyRouteCount: reader.Read(m_iEnemyRouteCount); break;
		case UnitSave_AlwaysHealCount: reader.Read(m_iAlwaysHealCount); break;
		case UnitSave_HillsDoubleMoveCount: reader.Read(m_iHillsDoubleMoveCount); break;
		case UnitSave_ExtraVisibilityRange: reader.Read(m_iExtraVisibilityRange); break;
		case UnitSave_ExtraMoves: reader.Read(m_iExtraMoves); break;
		case UnitSave_ExtraMoveDiscount: reader.Read(m_iExtraMoveDiscount); break;
		case UnitSave_ExtraWithdrawal: reader.Read(m_iExtraWithdrawal); break;
		case UnitSave_ExtraBombardRate: reader.Read(m_iExtraBombardRate); break;
		case UnitSave_ExtraEnemyHeal: reader.Read(m_iExtraEnemyHeal); break;
		case UnitSave_ExtraNeutralHeal: reader.Read(m_iExtraNeutralHeal); break;
		case UnitSave_ExtraFriendlyHeal: reader.Read(m_iExtraFriendlyHeal); break;
		case UnitSave_SameTileHeal: reader.Read(m_iSameTileHeal); break;
		case UnitSave_AdjacentTileHeal: reader.Read(m_iAdjacentTileHeal); break;
		case UnitSave_ExtraCombatPercent: reader.Read(m_iExtraCombatPercent); break;
		case UnitSave_ExtraCityAttackPercent: reader.Read(m_iExtraCityAttackPercent); break;
		case UnitSave_ExtraCityDefensePercent: reader.Read(m_iExtraCityDefensePercent); break;
		case UnitSave_ExtraHillsAttackPercent: reader.Read(m_iExtraHillsAttackPercent); break;
		case UnitSave_ExtraHillsDefensePercent: reader.Read(m_iExtraHillsDefensePercent); break;
		case UnitSave_ExtraDomesticBonusPercent: reader.Read(m_iExtraDomesticBonusPercent); break;	
		case UnitSave_PillageChange: reader.Read(m_iPillageChange); break;
		case UnitSave_UpgradeDiscount: reader.Read(m_iUpgradeDiscount); break;
		case UnitSave_ExperiencePercent: reader.Read(m_iExperiencePercent); break;
		case UnitSave_BaseCombat: reader.Read(m_iBaseCombat); break;
		case UnitSave_ImmobileTimer: reader.Read(m_iImmobileTimer); break;
		case UnitSave_YieldStored: reader.Read(m_iYieldStored); break;
		case UnitSave_ExtraWorkRate: reader.Read(m_iExtraWorkRate); break;
		case UnitSave_UnitTravelTimer: reader.Read(m_iUnitTravelTimer); break;
		case UnitSave_BadCityDefenderCount: reader.Read(m_iBadCityDefenderCount); break;
		case UnitSave_UnarmedCount: reader.Read(m_iUnarmedCount); break;
		case UnitSave_PostCombatPlotIndex: reader.Read(m_iPostCombatPlotIndex); break;
		}
	}
	
	// The unit is loaded. Now set up the cache according to the read data.

	FAssert(NO_UNIT != m_eUnitType);
	m_pUnitInfo = (NO_UNIT != m_eUnitType) ? &GC.getUnitInfo(m_eUnitType) : NULL;
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
	writer.Write(UnitSave_CargoCapacity, m_iCargoCapacity, defaultCargoCapacity);
	writer.Write(UnitSave_AttackPlotX, m_iAttackPlotX, defaultAttackPlotX);
	writer.Write(UnitSave_AttackPlotY, m_iAttackPlotY, defaultAttackPlotY);
	writer.Write(UnitSave_CombatTimer, m_iCombatTimer, defaultCombatTimer);
	writer.Write(UnitSave_CombatDamage, m_iCombatDamage, defaultCombatDamage);
	writer.Write(UnitSave_FortifyTurns, m_iFortifyTurns, defaultFortifyTurns);
	writer.Write(UnitSave_BlitzCount, m_iBlitzCount, defaultBlitzCount);
	writer.Write(UnitSave_AmphibCount, m_iAmphibCount, defaultAmphibCount);
	writer.Write(UnitSave_RiverCount, m_iRiverCount, defaultRiverCount);
	writer.Write(UnitSave_EnemyRouteCount, m_iEnemyRouteCount, defaultEnemyRouteCount);
	writer.Write(UnitSave_AlwaysHealCount, m_iAlwaysHealCount, defaultAlwaysHealCount);
	writer.Write(UnitSave_HillsDoubleMoveCount, m_iHillsDoubleMoveCount, defaultHillsDoubleMoveCount);
	writer.Write(UnitSave_ExtraVisibilityRange, m_iExtraVisibilityRange, defaultExtraVisibilityRange);
	writer.Write(UnitSave_ExtraMoves, m_iExtraMoves, defaultExtraMoves);
	writer.Write(UnitSave_ExtraMoveDiscount, m_iExtraMoveDiscount, defaultExtraMoveDiscount);
	writer.Write(UnitSave_ExtraWithdrawal, m_iExtraWithdrawal, defaultExtraWithdrawal);
	writer.Write(UnitSave_ExtraBombardRate, m_iExtraBombardRate, defaultExtraBombardRate);
	writer.Write(UnitSave_ExtraEnemyHeal, m_iExtraEnemyHeal, defaultExtraEnemyHeal);
	writer.Write(UnitSave_ExtraNeutralHeal, m_iExtraNeutralHeal, defaultExtraNeutralHeal);
	writer.Write(UnitSave_ExtraFriendlyHeal, m_iExtraFriendlyHeal, defaultExtraFriendlyHeal);
	writer.Write(UnitSave_SameTileHeal, m_iSameTileHeal, defaultSameTileHeal);
	writer.Write(UnitSave_AdjacentTileHeal, m_iAdjacentTileHeal, defaultAdjacentTileHeal);
	writer.Write(UnitSave_ExtraCombatPercent, m_iExtraCombatPercent, defaultExtraCombatPercent);
	writer.Write(UnitSave_ExtraCityAttackPercent, m_iExtraCityAttackPercent, defaultExtraCityAttackPercent);
	writer.Write(UnitSave_ExtraCityDefensePercent, m_iExtraCityDefensePercent, defaultExtraCityDefensePercent);
	writer.Write(UnitSave_ExtraHillsAttackPercent, m_iExtraHillsAttackPercent, defaultExtraHillsAttackPercent);
	writer.Write(UnitSave_ExtraHillsDefensePercent, m_iExtraHillsDefensePercent, defaultExtraHillsDefensePercent);
	writer.Write(UnitSave_ExtraDomesticBonusPercent, m_iExtraDomesticBonusPercent, defaultExtraDomesticBonusPercent);	
	writer.Write(UnitSave_PillageChange, m_iPillageChange, defaultPillageChange);
	writer.Write(UnitSave_UpgradeDiscount, m_iUpgradeDiscount, defaultUpgradeDiscount);
	writer.Write(UnitSave_ExperiencePercent, m_iExperiencePercent, defaultExperiencePercent);
	writer.Write(UnitSave_BaseCombat, m_iBaseCombat, defaultBaseCombat);
	writer.Write(UnitSave_ImmobileTimer, m_iImmobileTimer, defaultImmobileTimer);
	writer.Write(UnitSave_YieldStored, m_iYieldStored, defaultYieldStored);
	writer.Write(UnitSave_ExtraWorkRate, m_iExtraWorkRate, defaultExtraWorkRate);
	writer.Write(UnitSave_UnitTravelTimer, m_iUnitTravelTimer, defaultUnitTravelTimer);
	writer.Write(UnitSave_BadCityDefenderCount, m_iBadCityDefenderCount, defaultBadCityDefenderCount);
	writer.Write(UnitSave_UnarmedCount, m_iUnarmedCount, defaultUnarmedCount);
	writer.Write(UnitSave_PostCombatPlotIndex, m_iPostCombatPlotIndex, defaultPostCombatPlotIndex);

	writer.Write(UnitSave_END);
}
