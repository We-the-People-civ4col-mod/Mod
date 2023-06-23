//  $Header: //depot/main/Civilization4/CvGameCoreDLL/CvStructs.cpp#3 $
//------------------------------------------------------------------------------------------------
//
//  ***************** CIV4 GAME ENGINE   ********************
//
//! \file		CvStructs.cpp
//! \author		Multiple
//! \brief		Implementation of basic Civ4 structures
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2005 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------

#include "CvGameCoreDLL.h"
#include "CvUnit.h"
//#include "CvStructs.h"

#include "CvSavegame.h"

void IDInfo::read(FDataStreamBase* pStream)
{
	pStream->Read(&eOwner);
	pStream->Read(&iID);
}

void IDInfo::write(FDataStreamBase* pStream) const
{
	pStream->Write(eOwner);
	pStream->Write(iID);
}

void IDInfo::read(CvSavegameReader& reader)
{
	reader.Read(eOwner);
	reader.Read(iID);
}

void IDInfo::write(CvSavegameWriter& writer) const
{
	writer.Write(eOwner);
	writer.Write(iID);
}

OrderData::OrderData()
{
}
OrderData::OrderData(OrderTypes eOrder)
	: eOrderType(eOrder)
	, iData1(-1)
	, iData2(-1)
	, bSave(false)
{
}

const OrderTypes OrderData::getType() const
{
	return eOrderType;
}
UnitTypes& OrderData::unit()
{
	FAssert(eOrderType == ORDER_TRAIN);
	return m_unit;
}
const UnitTypes OrderData::unit() const
{
	FAssert(eOrderType == ORDER_TRAIN);
	return m_unit;
}
UnitAITypes& OrderData::unitAI()
{
	FAssert(eOrderType == ORDER_TRAIN);
	return m_unitAI;
}
const UnitAITypes OrderData::unitAI() const
{
	FAssert(eOrderType == ORDER_TRAIN);
	return m_unitAI;
}
BuildingTypes& OrderData::building()
{
	FAssert(eOrderType == ORDER_CONSTRUCT);
	return m_building;
}
const BuildingTypes OrderData::building() const
{
	FAssert(eOrderType == ORDER_CONSTRUCT);
	return m_building;
}
FatherPointTypes& OrderData::fatherpoint()
{
	FAssert(eOrderType == ORDER_CONVINCE);
	return m_fatherpoint;
}
const FatherPointTypes OrderData::fatherpoint() const
{
	FAssert(eOrderType == ORDER_CONVINCE);
	return m_fatherpoint;
}

void OrderData::read(CvSavegameReader& reader)
{
	reader.Read(eOrderType);
	if (reader.getSavegameVersion() == 1)
	{
		reader.Read(iData1);
		reader.Read(iData2);
		reader.Read(bSave);
		return;
	}

	reader.Read(bSave);
	switch (eOrderType)
	{
	case ORDER_TRAIN:
		reader.Read(m_unit);
		reader.Read(m_unitAI);
		break;
	case ORDER_CONSTRUCT:
		reader.Read(m_building);
		break;
	case ORDER_CONVINCE:
		reader.Read(m_fatherpoint);
		break;
	}
}

void OrderData::write(CvSavegameWriter& writer) const
{
	writer.Write(eOrderType);
	writer.Write(bSave); 
	
	switch (eOrderType)
	{
	case ORDER_TRAIN:
		writer.Write(m_unit);
		writer.Write(m_unitAI);
		break;
	case ORDER_CONSTRUCT:
		writer.Write(m_building);
		break;
	case ORDER_CONVINCE:
		writer.Write(m_fatherpoint);
		break;
	}
}

void MissionData::read(CvSavegameReader& reader)
{
	// iData1 is BuildTypes when eMissionType == MISSION_BUILD
	reader.Read(eMissionType);
	if (reader.getSavegameVersion() > 1 && eMissionType == MISSION_BUILD)
	{
		reader.Read(eBuild);
	}
	else
	{
		reader.Read(iData1);
	}
	reader.Read(iData2);
	reader.Read(iFlags);
	reader.Read(iPushTurn);
}

void MissionData::write(CvSavegameWriter& writer) const
{
	writer.Write(eMissionType);
	if (eMissionType == MISSION_BUILD)
	{
		writer.Write(eBuild);
	}
	else
	{
		writer.Write(iData1);
	}
	writer.Write(iData2);
	writer.Write(iFlags);
	writer.Write(iPushTurn);
}

void TradeData::read(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_eItemType);
	pStream->Read(&m_iData1);
	m_kTransport.read(pStream);
	pStream->Read(&m_bOffering);
	pStream->Read(&m_bHidden);
}

void TradeData::write(FDataStreamBase* pStream) const
{
	pStream->Write(m_eItemType);
	pStream->Write(m_iData1);
	m_kTransport.write(pStream);
	pStream->Write(m_bOffering);
	pStream->Write(m_bHidden);
}

void TradeData::read(CvSavegameReader& reader)
{
	// m_iData1 is YieldTypes if m_eItemType == TRADE_YIELD

	reader.Read(m_eItemType);
	if (reader.getSavegameVersion() > 1 && m_eItemType == TRADE_YIELD)
	{
		reader.Read(m_eYield);
	}
	else
	{
		reader.Read(m_iData1);
	}
	m_kTransport.read(reader);
	reader.Read(m_bOffering);
	reader.Read(m_bHidden);
}

void TradeData::write(CvSavegameWriter& writer) const
{
	writer.Write(m_eItemType);
	if (m_eItemType == TRADE_YIELD)
	{
		writer.Write(m_eYield);
	}
	else
	{
		writer.Write(m_iData1);
	}
	m_kTransport.write(writer);
	writer.Write(m_bOffering);
	writer.Write(m_bHidden);
}

int EventTriggeredData::getID() const
{
	return m_iId;
}

void EventTriggeredData::setID(int iID)
{
	m_iId = iID;
}

void EventTriggeredData::read(CvSavegameReader& reader)
{
	reader.Read(m_iId);
	reader.Read(m_eTrigger);
	reader.Read(m_iTurn);
	reader.Read(m_ePlayer);
	reader.Read(m_iCityId);
	reader.Read(m_iPlotX);
	reader.Read(m_iPlotY);
	reader.Read(m_iUnitId);
	reader.Read(m_eOtherPlayer);
	reader.Read(m_iOtherPlayerCityId);
	reader.Read(m_eBuilding);
	reader.Read(m_szText);
	reader.Read(m_szGlobalText);
}

void EventTriggeredData::write(CvSavegameWriter& writer) const
{
	writer.Write(m_iId);
	writer.Write(m_eTrigger);
	writer.Write(m_iTurn);
	writer.Write(m_ePlayer);
	writer.Write(m_iCityId);
	writer.Write(m_iPlotX);
	writer.Write(m_iPlotY);
	writer.Write(m_iUnitId);
	writer.Write(m_eOtherPlayer);
	writer.Write(m_iOtherPlayerCityId);
	writer.Write(m_eBuilding);
	writer.Write(m_szText);
	writer.Write(m_szGlobalText);
}

void PlotExtraYield::read(CvSavegameReader& reader)
{
	reader.Read(m_iX);
	reader.Read(m_iY);

	// use savegame code in EnumMap to allow for xml changes in yield xml
	EnumMap<YieldTypes, int> tempArray;
	reader.Read(tempArray);
	tempArray.copyToVector(m_aeExtraYield);
}

void PlotExtraYield::write(CvSavegameWriter& writer) const 
{
	writer.Write(m_iX);
	writer.Write(m_iY);

	// use savegame code in EnumMap to allow for xml changes in yield xml
	EnumMap<YieldTypes, int> tempArray;
	tempArray.copyFromVector(m_aeExtraYield);
	writer.Write(tempArray);
}

void BuildingYieldChange::read(FDataStreamBase* pStream)
{
	pStream->Read((int*)&eBuildingClass);
	pStream->Read((int*)&eYield);
	pStream->Read(&iChange);
}

void BuildingYieldChange::write(FDataStreamBase* pStream)
{
	pStream->Write(eBuildingClass);
	pStream->Write(eYield);
	pStream->Write(iChange);
}

void BuildingYieldChange::read(CvSavegameReader& reader)
{
	reader.Read(eBuildingClass);
	reader.Read(eYield);
	reader.Read(iChange);
}

void BuildingYieldChange::write(CvSavegameWriter& writer) const
{
	writer.Write(eBuildingClass);
	writer.Write(eYield);
	writer.Write(iChange);
}

void checkBattleUnitType(BattleUnitTypes unitType)
{
	FAssertMsg((unitType >= 0) && (unitType < BATTLE_UNIT_COUNT), "[Jason] Invalid battle unit type.");
}

CvBattleRound::CvBattleRound() :
	m_iWaveSize(0),
	m_bRangedRound(false)
{
	m_aNumKilled[BATTLE_UNIT_ATTACKER] = m_aNumKilled[BATTLE_UNIT_DEFENDER] = 0;
	m_aNumAlive[BATTLE_UNIT_ATTACKER] = m_aNumAlive[BATTLE_UNIT_DEFENDER] = 0;
}

bool CvBattleRound::isValid() const
{
	bool bValid = true;

	// Valid if no more than the wave size was killed, and didn't kill more attackers than were defenders or vv.
	bValid &= (m_aNumKilled[BATTLE_UNIT_ATTACKER] + m_aNumKilled[BATTLE_UNIT_DEFENDER] <= m_iWaveSize);
	bValid &= (m_aNumKilled[BATTLE_UNIT_ATTACKER] <= m_aNumAlive[BATTLE_UNIT_DEFENDER]);
	bValid &= (m_aNumKilled[BATTLE_UNIT_DEFENDER] <= m_aNumAlive[BATTLE_UNIT_ATTACKER]);
	return bValid;
}

bool CvBattleRound::isRangedRound() const
{
	return m_bRangedRound;
}

void CvBattleRound::setRangedRound(bool value)
{
	m_bRangedRound = value;
}

int CvBattleRound::getWaveSize() const
{
	return m_iWaveSize;
}

void CvBattleRound::setWaveSize(int size)
{
	m_iWaveSize = size;
}

int CvBattleRound::getNumKilled(BattleUnitTypes unitType) const
{
	checkBattleUnitType(unitType);
	return m_aNumKilled[unitType];
}

void CvBattleRound::setNumKilled(BattleUnitTypes unitType, int value)
{
	checkBattleUnitType(unitType);
	m_aNumKilled[unitType] = value;
}

void CvBattleRound::addNumKilled(BattleUnitTypes unitType, int increment)
{
	checkBattleUnitType(unitType);
	m_aNumKilled[unitType] += increment;
}

int CvBattleRound::getNumAlive(BattleUnitTypes unitType) const
{
	checkBattleUnitType(unitType);
	return m_aNumAlive[unitType];
}

void CvBattleRound::setNumAlive(BattleUnitTypes unitType, int value)
{
	checkBattleUnitType(unitType);
	m_aNumAlive[unitType] = value;
}

//------------------------------------------------------------------------------------------------
// FUNCTION:    CvMissionDefinition::CvMissionDefinition
//! \brief      Default constructor.
//------------------------------------------------------------------------------------------------
CvMissionDefinition::CvMissionDefinition() :
	m_fMissionTime(0.0f),
	m_eMissionType(NO_MISSION),
	m_pPlot(NULL)
{
	for(int i=0;i<BATTLE_UNIT_COUNT;i++)
		m_aUnits[i] = NULL;
}

MissionTypes CvMissionDefinition::getMissionType() const
{
	return m_eMissionType;
}

void CvMissionDefinition::setMissionType(MissionTypes missionType)
{
	m_eMissionType = missionType;
}

float CvMissionDefinition::getMissionTime() const
{
	return m_fMissionTime;
}

void CvMissionDefinition::setMissionTime(float time)
{
	m_fMissionTime = time;
}

CvUnit *CvMissionDefinition::getUnit(BattleUnitTypes unitType) const
{
	checkBattleUnitType(unitType);
	return m_aUnits[unitType];
}

void CvMissionDefinition::setUnit(BattleUnitTypes unitType, CvUnit *unit)
{
	checkBattleUnitType(unitType);
	m_aUnits[unitType] = unit;
}

const CvPlot *CvMissionDefinition::getPlot() const
{
	return m_pPlot;
}

void CvMissionDefinition::setPlot(const CvPlot *plot)
{
	m_pPlot = plot;
}

//------------------------------------------------------------------------------------------------
// FUNCTION:    CvBattleDefinition::CvBattleDefinition
//! \brief      Constructor.
//------------------------------------------------------------------------------------------------
CvBattleDefinition::CvBattleDefinition() :
	m_bAdvanceSquare(false),
	CvMissionDefinition()
{
	m_fMissionTime = 0.0f;
	m_eMissionType = MISSION_BEGIN_COMBAT;
	m_iNumMeleeRounds = 0;
	m_iNumRangedRounds = 0;

	for(int i=0;i<BATTLE_UNIT_COUNT;i++)
	{
		m_aUnits[i] = NULL;
		for(int j=0;j<BATTLE_TIME_COUNT;j++)
			m_aDamage[i][j] = 0;
	}
}

//------------------------------------------------------------------------------------------------
// FUNCTION:    CvBattleDefinition::CvBattleDefinition
//! \brief      Copy constructor
//! \param      kCopy The object to copy
//------------------------------------------------------------------------------------------------
CvBattleDefinition::CvBattleDefinition( const CvBattleDefinition & kCopy ) :
	m_bAdvanceSquare( kCopy.m_bAdvanceSquare )
{
	m_fMissionTime = kCopy.m_fMissionTime;
	m_eMissionType = MISSION_BEGIN_COMBAT;
	m_iNumMeleeRounds = kCopy.m_iNumMeleeRounds;
	m_iNumRangedRounds = kCopy.m_iNumRangedRounds;

	for(int i=0;i<BATTLE_UNIT_COUNT;i++)
	{
		m_aUnits[i] = kCopy.m_aUnits[i];
		for(int j=0;j<BATTLE_TIME_COUNT;j++)
			m_aDamage[i][j] = kCopy.m_aDamage[i][j];
	}

	m_aBattleRounds.assign(kCopy.m_aBattleRounds.begin(), kCopy.m_aBattleRounds.end());
}

int CvBattleDefinition::getDamage(BattleUnitTypes unitType, BattleTimeTypes timeType) const
{
	checkBattleUnitType(unitType);
	checkBattleTimeType(timeType);
	return m_aDamage[unitType][timeType];
}

void CvBattleDefinition::setDamage(BattleUnitTypes unitType, BattleTimeTypes timeType, int damage)
{
	checkBattleUnitType(unitType);
	checkBattleTimeType(timeType);
	m_aDamage[unitType][timeType] = damage;
}

void CvBattleDefinition::addDamage(BattleUnitTypes unitType, BattleTimeTypes timeType, int increment)
{
	checkBattleUnitType(unitType);
	checkBattleTimeType(timeType);
	m_aDamage[unitType][timeType] += increment;
}

bool CvBattleDefinition::isOneStrike() const
{
	CvUnit* pAttacker = getUnit(BATTLE_UNIT_ATTACKER);
	CvUnit* pDefender = getUnit(BATTLE_UNIT_DEFENDER);
	if(pAttacker->isRanged() != pDefender->isRanged())
	{
		BattleUnitTypes eMeleeUnitType = pAttacker->isRanged() ? BATTLE_UNIT_DEFENDER : BATTLE_UNIT_ATTACKER;
		BattleUnitTypes eRangedUnitType = pDefender->isRanged() ? BATTLE_UNIT_DEFENDER : BATTLE_UNIT_ATTACKER;
		if(getDamage(eMeleeUnitType, BATTLE_TIME_END) >= GC.getMAX_HIT_POINTS())
		{
			if((getDamage(eRangedUnitType, BATTLE_TIME_END) == getDamage(eRangedUnitType, BATTLE_TIME_BEGIN)) || (getDamage(eRangedUnitType, BATTLE_TIME_END) < GC.getMAX_HIT_POINTS() / 2))
			{
				return true;
			}
		}
	}

	return false;
}

bool CvBattleDefinition::isAdvanceSquare() const
{
	return m_bAdvanceSquare;
}

void CvBattleDefinition::setAdvanceSquare(bool advanceSquare)
{
	m_bAdvanceSquare = advanceSquare;
}

int CvBattleDefinition::getNumRangedRounds() const
{
	return m_iNumRangedRounds;
}

void CvBattleDefinition::setNumRangedRounds(int count)
{
	m_iNumRangedRounds = count;
}

void CvBattleDefinition::addNumRangedRounds(int increment)
{
	m_iNumRangedRounds += increment;
}

int CvBattleDefinition::getNumMeleeRounds() const
{
	return m_iNumMeleeRounds;
}

void CvBattleDefinition::setNumMeleeRounds(int count)
{
	m_iNumMeleeRounds = count;
}

void CvBattleDefinition::addNumMeleeRounds(int increment)
{
	m_iNumMeleeRounds += increment;
}

int CvBattleDefinition::getNumBattleRounds() const
{
	return m_aBattleRounds.size();
}

void CvBattleDefinition::clearBattleRounds()
{
	m_aBattleRounds.clear();
}

CvBattleRound &CvBattleDefinition::getBattleRound(int index)
{
	checkBattleRound(index);
	return m_aBattleRounds[index];
}

const CvBattleRound &CvBattleDefinition::getBattleRound(int index) const
{
	checkBattleRound(index);
	return m_aBattleRounds[index];
}

void CvBattleDefinition::addBattleRound(const CvBattleRound &round)
{
	m_aBattleRounds.push_back(round);
}

void CvBattleDefinition::setBattleRound(int index, const CvBattleRound &round)
{
	m_aBattleRounds.assign(index, round);
}

void CvBattleDefinition::checkBattleTimeType(BattleTimeTypes timeType) const
{
	FAssertMsg((timeType >= 0) && (timeType < BATTLE_TIME_COUNT), "[Jason] Invalid battle time type.");
}

void CvBattleDefinition::checkBattleRound(int index) const
{
	FAssertMsg((index >= 0) && (index < (int)m_aBattleRounds.size()), "[Jason] Invalid battle round index.");
}

PBGameSetupData::PBGameSetupData()
{
	for (int i = 0; i < NUM_GAMEOPTION_TYPES; i++)
	{
		abOptions.push_back(false);
	}
	for (int i = 0; i < NUM_MPOPTION_TYPES; i++)
	{
		abMPOptions.push_back(false);
	}
}

