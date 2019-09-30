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
	}
	return "";
}

int getNumSavedEnumValuesCityAI()
{
	return NUM_SAVE_ENUM_VALUES;
}

// assign everything to default values
void CvCityAI::resetSavedData()
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

m_ja_iYieldOutputWeight.reset();
m_ja_iNeededYield.reset();
m_ja_iTradeBalance.reset();
m_ja_iYieldAdvantage.reset();
m_ja_iEmphasizeYieldCount.reset();
}

void CvCityAI::read(CvSavegameReader reader)
{
	reader.AssignClassType(SAVEGAME_CLASS_CITY_AI);

	// Init data before load
	// This will ensure that all variables not included in the savegame will have default values
	resetSavedData();

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
			case CitySaveAi_GiftTimer: reader.Read(m_iGiftTimer); break;
			case CitySaveAi_TradeTimer: reader.Read(m_iTradeTimer); break;
			case CitySaveAi_DesiredYield: reader.Read(m_eDesiredYield); break;
			case CitySaveAi_TargetSize: reader.Read(m_iTargetSize); break;
			case CitySaveAi_FoundValue: reader.Read(m_iFoundValue); break;
			case CitySaveAi_EmphasizeAvoidGrowthCount: reader.Read(m_iEmphasizeAvoidGrowthCount); break;

			case CitySaveAi_Port: reader.Read(m_bPort); break;
			case CitySaveAi_AssignWorkDirty: reader.Read(m_bAssignWorkDirty); break;
			case CitySaveAi_ChooseProductionDirty: reader.Read(m_bChooseProductionDirty); break;

			case CitySaveAi_YieldOutputWeight: reader.Read(m_ja_iYieldOutputWeight); break;
			case CitySaveAi_NeededYield: reader.Read(m_ja_iNeededYield); break;
			case CitySaveAi_TradeBalance: reader.Read(m_ja_iTradeBalance); break;
			case CitySaveAi_YieldAdvantage: reader.Read(m_ja_iYieldAdvantage); break;
			case CitySaveAi_EmphasizeYieldCount: reader.Read(m_ja_iEmphasizeYieldCount); break;

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

	writer.Write(CitySaveAi_YieldOutputWeight, m_ja_iYieldOutputWeight);
	writer.Write(CitySaveAi_NeededYield, m_ja_iNeededYield);
	writer.Write(CitySaveAi_TradeBalance, m_ja_iTradeBalance);
	writer.Write(CitySaveAi_YieldAdvantage, m_ja_iYieldAdvantage);
	writer.Write(CitySaveAi_EmphasizeYieldCount, m_ja_iEmphasizeYieldCount);
	writer.Write(Save_END);
}
