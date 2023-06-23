#include "CvGameCoreDLL.h"

#include "CvSavegame.h"

// set the default values
const int defaultGiftTimer = 0;
const int defaultTradeTimer = 0;
const YieldTypes defaultDesiredYield = NO_YIELD;
const int defaultTargetSize = 0;
const int defaultFoundValue = 0;
const int defaultEmphasizeAvoidGrowthCount = 0;
	
const bool defaultPort = false;
const bool defaultAssignWorkDirty = false;
const bool defaultChooseProductionDirty = false;

const bool defaultForceEmphasizeCulture = false;
const int defaultCachePlayerClosenessTurn = -1;
const int defaultCachePlayerClosenessDistance = -1;
const int defaultNeededFloatingDefenders = -1;
const int defaultNeededFloatingDefendersCacheTurn = -1;
const int defaultWorkersNeeded = 0;
const int defaultWorkersHave = 0;

enum SavegameVariableTypes
{
	Save_END,
	CitySaveAi_GiftTimer,
	CitySaveAi_TradeTimer,
	CitySaveAi_DesiredYield,
	CitySaveAi_TargetSize,
	CitySaveAi_FoundValue,
	CitySaveAi_EmphasizeAvoidGrowthCount,

	CitySaveAi_Port,
	CitySaveAi_AssignWorkDirty,
	CitySaveAi_ChooseProductionDirty,

	CitySaveAi_YieldOutputWeight,
	CitySaveAi_NeededYield,
	CitySaveAi_TradeBalance,
	CitySaveAi_YieldAdvantage,
	CitySaveAi_EmphasizeYieldCount,

	CitySaveAi_ForceEmphasizeCulture,
	CitySaveAi_CachePlayerClosenessTurn,
	CitySaveAi_CachePlayerClosenessDistance,
	CitySaveAi_NeededFloatingDefenders,
	CitySaveAi_NeededFloatingDefendersCacheTurn,
	CitySaveAi_WorkersNeeded,
	CitySaveAi_WorkersHave,
	CitySaveAi_Emphasize,
	CitySaveAi_PlayerCloseness,
	CitySaveAi_routeToCity,
	CitySaveAi_BestBuildValue,
	CitySaveAi_BestBuild,
	NUM_SAVE_ENUM_VALUES,
};

const char* getSavedEnumNameCityAi(SavegameVariableTypes eType)
{
	switch (eType)
	{
	case CitySaveAi_GiftTimer: return "CitySaveAi_GiftTimer";
	case CitySaveAi_TradeTimer: return "CitySaveAi_TradeTimer";
	case CitySaveAi_DesiredYield: return "CitySaveAi_DesiredYield";
	case CitySaveAi_TargetSize: return "CitySaveAi_TargetSize";
	case CitySaveAi_FoundValue: return "CitySaveAi_FoundValue";
	case CitySaveAi_EmphasizeAvoidGrowthCount: return "CitySaveAi_EmphasizeAvoidGrowthCount";
		
	case CitySaveAi_Port: return "CitySaveAi_Port";
	case CitySaveAi_AssignWorkDirty: return "CitySaveAi_AssignWorkDirty";
	case CitySaveAi_ChooseProductionDirty: return "CitySaveAi_ChooseProductionDirty";

	case CitySaveAi_YieldOutputWeight: return "CitySaveAi_YieldOutputWeight";
	case CitySaveAi_NeededYield: return "CitySaveAi_NeededYield";
	case CitySaveAi_TradeBalance: return "CitySaveAi_TradeBalance";
	case CitySaveAi_YieldAdvantage: return "CitySaveAi_YieldAdvantage";
	case CitySaveAi_EmphasizeYieldCount: return "CitySaveAi_EmphasizeYieldCount";

	case CitySaveAi_ForceEmphasizeCulture: return "CitySaveAi_ForceEmphasizeCulture";
	case CitySaveAi_CachePlayerClosenessTurn: return "CitySaveAi_CachePlayerClosenessTurn";
	case CitySaveAi_CachePlayerClosenessDistance: return "CitySaveAi_CachePlayerClosenessDistance";
	case CitySaveAi_NeededFloatingDefenders: return "CitySaveAi_NeededFloatingDefenders";
	case CitySaveAi_NeededFloatingDefendersCacheTurn: return "CitySaveAi_NeededFloatingDefendersCacheTurn";
	case CitySaveAi_WorkersNeeded: return "CitySaveAi_WorkersNeeded";
	case CitySaveAi_WorkersHave: return "CitySaveAi_WorkersHave";
	case CitySaveAi_Emphasize: return "CitySaveAi_Emphasize";
	case CitySaveAi_PlayerCloseness: return "CitySaveAi_PlayerCloseness";
	case CitySaveAi_routeToCity: return "CitySaveAi_routeToCity";

	case CitySaveAi_BestBuildValue: return "CitySaveAi_BestBuildValue";
	case CitySaveAi_BestBuild: return "CitySaveAi_BestBuild";
	}
	FAssertMsg(0, "Missing case");
	return "";
}

int getNumSavedEnumValuesCityAI()
{
	return NUM_SAVE_ENUM_VALUES;
}

// assign everything to default values
void CvCityAI::AI_resetSavedData()
{
m_iGiftTimer =  defaultGiftTimer;
m_iTradeTimer =  defaultTradeTimer;
m_eDesiredYield = defaultDesiredYield;
m_iTargetSize = defaultTargetSize;
m_iFoundValue = defaultFoundValue;
m_iEmphasizeAvoidGrowthCount = defaultEmphasizeAvoidGrowthCount;

m_bPort = defaultPort;
m_bAssignWorkDirty = defaultAssignWorkDirty;
m_bChooseProductionDirty = defaultChooseProductionDirty;

m_em_iYieldOutputWeight.reset();
m_em_iNeededYield.reset();
m_em_iTradeBalance.reset();
m_em_iYieldAdvantage.reset();
m_em_iEmphasizeYieldCount.reset();

m_bForceEmphasizeCulture = defaultForceEmphasizeCulture;
m_iCachePlayerClosenessTurn = defaultCachePlayerClosenessTurn;
m_iCachePlayerClosenessDistance = defaultCachePlayerClosenessDistance;
m_iNeededFloatingDefenders = defaultNeededFloatingDefenders;
m_iNeededFloatingDefendersCacheTurn = defaultNeededFloatingDefendersCacheTurn;
m_iWorkersNeeded = defaultWorkersNeeded;
m_iWorkersHave = defaultWorkersHave;
m_em_bEmphasize.reset();
m_em_iPlayerCloseness.reset();
m_routeToCity.reset();

m_em_iBestBuildValue.reset();
m_em_eBestBuild.reset();

}

void CvCityAI::read(CvSavegameReader reader)
{
	reader.AssignClassType(SAVEGAME_CLASS_CITY_AI);

	// Init data before load
	// This will ensure that all variables not included in the savegame will have default values
	AI_resetSavedData();

	// read base class. It's always placed first
	CvCity::read(reader);

	// loop read all the variables
	// As long as each variable has a CitySavegameVariables "header", order doesn't matter.
	// Variables can be read in any order and any number of variables can be skipped.
	bool bContinue = true;
	while (bContinue)
	{
		SavegameVariableTypes eType;
		reader.Read(eType);

		switch (eType)
		{
		case Save_END:
			bContinue = false;
			break;
			case CitySaveAi_GiftTimer                           : reader.Read(m_iGiftTimer)                                      ; break;
			case CitySaveAi_TradeTimer                          : reader.Read(m_iTradeTimer)                                     ; break;
			case CitySaveAi_DesiredYield                        : reader.Read(m_eDesiredYield)                                   ; break;
			case CitySaveAi_TargetSize                          : reader.Read(m_iTargetSize)                                     ; break;
			case CitySaveAi_FoundValue                          : reader.Read(m_iFoundValue)                                     ; break;
			case CitySaveAi_EmphasizeAvoidGrowthCount           : reader.Read(m_iEmphasizeAvoidGrowthCount)                      ; break;

			case CitySaveAi_Port                                : reader.Read(m_bPort)                                           ; break;
			case CitySaveAi_AssignWorkDirty                     : reader.Read(m_bAssignWorkDirty)                                ; break;
			case CitySaveAi_ChooseProductionDirty               : reader.Read(m_bChooseProductionDirty)                          ; break;

			case CitySaveAi_YieldOutputWeight                   : reader.Read(m_em_iYieldOutputWeight)                           ; break;
			case CitySaveAi_NeededYield                         : reader.Read(m_em_iNeededYield)                                 ; break;
			case CitySaveAi_TradeBalance                        : reader.Read(m_em_iTradeBalance)                                ; break;
			case CitySaveAi_YieldAdvantage                      : reader.Read(m_em_iYieldAdvantage)                              ; break;
			case CitySaveAi_EmphasizeYieldCount                 : reader.Read(m_em_iEmphasizeYieldCount)                         ; break;

			case CitySaveAi_ForceEmphasizeCulture               : reader.Read(m_bForceEmphasizeCulture)                          ; break;
			case CitySaveAi_CachePlayerClosenessTurn            : reader.Read(m_iCachePlayerClosenessTurn)                       ; break;
			case CitySaveAi_CachePlayerClosenessDistance        : reader.Read(m_iCachePlayerClosenessDistance)                   ; break;
			case CitySaveAi_NeededFloatingDefenders             : reader.Read(m_iNeededFloatingDefenders)                        ; break;
			case CitySaveAi_NeededFloatingDefendersCacheTurn    : reader.Read(m_iNeededFloatingDefendersCacheTurn)               ; break;
			case CitySaveAi_WorkersNeeded                       : reader.Read(m_iWorkersNeeded)                                  ; break;
			case CitySaveAi_WorkersHave                         : reader.Read(m_iWorkersHave)                                    ; break;
			case CitySaveAi_Emphasize                           : reader.Read(m_em_bEmphasize)                                   ; break;
			case CitySaveAi_PlayerCloseness                     : reader.Read(m_em_iPlayerCloseness)                             ; break;
			case CitySaveAi_routeToCity                         : reader.Read(m_routeToCity)                                     ; break;
			case CitySaveAi_BestBuildValue                      : reader.Read(m_em_iBestBuildValue)                              ; break;
			case CitySaveAi_BestBuild                           : reader.Read(m_em_eBestBuild)                                   ; break; 

			default: FAssert(false);
		}
	}
	
	// Loading done. Set up the cache (if any).

}

void CvCityAI::write(CvSavegameWriter writer)
{
	writer.AssignClassType(SAVEGAME_CLASS_CITY_AI);

	// Write the data.
	// Use WriteSwitch since it will automatically include WriteSwitch in the savegame.
	// Also it will not save anything if the variable and the default values are identical.
	// If nothing is saved, the loading code will use the default values.
	// Less data saved/loaded means smaller savegames.

	// write base class first
	CvCity::write(writer);
	writer.Write(CitySaveAi_GiftTimer, m_iGiftTimer, defaultGiftTimer);
	writer.Write(CitySaveAi_TradeTimer, m_iTradeTimer, defaultTradeTimer);
	writer.Write(CitySaveAi_DesiredYield, m_eDesiredYield, defaultDesiredYield);
	writer.Write(CitySaveAi_TargetSize, m_iTargetSize, defaultTargetSize);
	writer.Write(CitySaveAi_FoundValue, m_iFoundValue, defaultFoundValue);
	writer.Write(CitySaveAi_EmphasizeAvoidGrowthCount, m_iEmphasizeAvoidGrowthCount, defaultEmphasizeAvoidGrowthCount);

	writer.Write(CitySaveAi_Port, m_bPort, defaultPort);
	writer.Write(CitySaveAi_AssignWorkDirty, m_bAssignWorkDirty, defaultAssignWorkDirty);
	writer.Write(CitySaveAi_ChooseProductionDirty, m_bChooseProductionDirty, defaultChooseProductionDirty);

	writer.Write(CitySaveAi_YieldOutputWeight, m_em_iYieldOutputWeight);
	writer.Write(CitySaveAi_NeededYield, m_em_iNeededYield);
	writer.Write(CitySaveAi_TradeBalance, m_em_iTradeBalance);
	writer.Write(CitySaveAi_YieldAdvantage, m_em_iYieldAdvantage);
	writer.Write(CitySaveAi_EmphasizeYieldCount, m_em_iEmphasizeYieldCount);

	writer.Write(CitySaveAi_ForceEmphasizeCulture, m_bForceEmphasizeCulture, defaultForceEmphasizeCulture);
	writer.Write(CitySaveAi_CachePlayerClosenessTurn, m_iCachePlayerClosenessTurn, defaultCachePlayerClosenessTurn);
	writer.Write(CitySaveAi_CachePlayerClosenessDistance, m_iCachePlayerClosenessDistance, defaultCachePlayerClosenessDistance);
	writer.Write(CitySaveAi_NeededFloatingDefenders, m_iNeededFloatingDefenders, defaultNeededFloatingDefenders);
	writer.Write(CitySaveAi_NeededFloatingDefendersCacheTurn, m_iNeededFloatingDefendersCacheTurn, defaultNeededFloatingDefendersCacheTurn);
	writer.Write(CitySaveAi_WorkersNeeded, m_iWorkersNeeded, defaultWorkersNeeded);
	writer.Write(CitySaveAi_WorkersHave, m_iWorkersHave, defaultWorkersHave);

	writer.Write(CitySaveAi_Emphasize, m_em_bEmphasize);
	writer.Write(CitySaveAi_PlayerCloseness, m_em_iPlayerCloseness);

	writer.Write(CitySaveAi_routeToCity, m_routeToCity);

	writer.Write(CitySaveAi_BestBuildValue, m_em_iBestBuildValue);
	writer.Write(CitySaveAi_BestBuild, m_em_eBestBuild);

	writer.Write(Save_END);
}
