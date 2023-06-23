#pragma once

#ifndef CVSTRUCTS_H
#define CVSTRUCTS_H

// structs.h

//#include "CvEnums.h"
#include "CvString.h"

// XXX these should not be in the DLL per se (if the user changes them, we are screwed...)

/*
 * Despite the warning, this file isn't completely off limits to modding.
 * The warning says the exe assumes a certain memory layout of the structs.
 * This means whatever we do, we can't change the memory layout, which is severely limiting to what we can do.
 * We can't add/remove/alter variables and we can't add hidden variables like virtual function pointers
 *    (automatically created with virtual function declarations)
 * What we can do is everything, which doesn't affect the memory layout at all.
 * Realistically speaking, this is likely limited to adding new (non-virtual) functions.
 * This means it should be safe to add functions to support CvSavegame.h.
 * However since we can't change the variables themselves it makes no sense to add an enum to support changes in variables in savegames.
 * Savegames should use the vanilla approach of just read/write everything in order.
 * Maybe add something to conditionally save variables in big structs if they are prone to lots of default values.
 *
 * Nightinggale
 */


struct DllExport XYCoords
{
	XYCoords(int x=0, int y=0) : iX(x), iY(y) {}
	int iX;
	int iY;

	bool operator<  (const XYCoords xy) const { return ((iY < xy.iY) || (iY == xy.iY && iX < xy.iX)); }
	bool operator<= (const XYCoords xy) const { return ((iY < xy.iY) || (iY == xy.iY && iX <= xy.iX)); }
	bool operator!= (const XYCoords xy) const { return (iY != xy.iY || iX != xy.iX); }
	bool operator== (const XYCoords xy) const { return (!(iY != xy.iY || iX != xy.iX)); }
	bool operator>= (const XYCoords xy) const { return ((iY > xy.iY) || (iY == xy.iY && iX >= xy.iX)); }
	bool operator>  (const XYCoords xy) const { return ((iY > xy.iY) || (iY == xy.iY && iX > xy.iX)); }
};

struct DllExport IDInfo
{

	IDInfo(PlayerTypes eOwner = NO_PLAYER, int iID = -1) : eOwner(eOwner), iID(iID) {}
	PlayerTypes eOwner;
	int iID;

	bool operator== (const IDInfo& rhs) const { return (rhs.eOwner == eOwner && rhs.iID == iID); }
	bool operator!= (const IDInfo& rhs) const { return (!operator==(rhs)); }
	bool operator< (const IDInfo& rhs) const { if (eOwner != rhs.eOwner) return (eOwner < rhs.eOwner); if (iID != rhs.iID) return (iID < rhs.iID); return false; }

	void reset()
	{
		eOwner = NO_PLAYER;
		iID = -1;
	}

	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream) const;

	void read(CvSavegameReader& reader);
	void write(CvSavegameWriter& writer) const;
};

struct DllExport GameTurnInfo
{
	int iMonthIncrement;
	int iNumGameTurnsPerIncrement;
};

struct DllExport OrderData
{
	friend void CyStructsPythonInterface1();
private:
	OrderTypes eOrderType;
	union
	{
		int iData1;
		UnitTypes m_unit;
		BuildingTypes m_building;
		FatherPointTypes m_fatherpoint;
	};
	union
	{
		int iData2;
		UnitAITypes m_unitAI;
	};
public:
	bool bSave;

	OrderData();
	OrderData(OrderTypes);

	const OrderTypes getType() const;
	UnitTypes& unit();
	const UnitTypes unit() const;
	UnitAITypes& unitAI();
	const UnitAITypes unitAI() const;
	BuildingTypes& building();
	const BuildingTypes building() const;
	FatherPointTypes& fatherpoint();
	const FatherPointTypes fatherpoint() const;

	void read(CvSavegameReader& reader);
	void write(CvSavegameWriter& writer) const;
};
BOOST_STATIC_ASSERT(sizeof(OrderData) == 16);

struct DllExport MissionData
{
	MissionTypes eMissionType;
	union
	{
		int iData1;
		BuildTypes eBuild;
	};
	int iData2;
	int iFlags;
	int iPushTurn;
	
	void read(CvSavegameReader& reader);
	void write(CvSavegameWriter& writer) const;
};

struct DllExport TradeData
{
	TradeableItems m_eItemType;				//	What type of item is this
	union
	{
		int m_iData1;											//	Any additional data?
		YieldTypes m_eYield;
	};
	IDInfo m_kTransport;
	bool m_bOffering;									//	Is this item up for grabs?
	bool m_bHidden;										//	Are we hidden?

	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream) const;

	void read(CvSavegameReader& reader);
	void write(CvSavegameWriter& writer) const;
};

struct EventTriggeredData
{
	int m_iId;
	EventTriggerTypes m_eTrigger;
	int m_iTurn;
	PlayerTypes m_ePlayer;
	int m_iCityId;
	int m_iPlotX;
	int m_iPlotY;
	int m_iUnitId;
	PlayerTypes m_eOtherPlayer;
	int m_iOtherPlayerCityId;
	BuildingTypes m_eBuilding;
	CvWString m_szText;
	CvWString m_szGlobalText;

	int getID() const;
	void setID(int iID);

	void read(CvSavegameReader& reader);
	void write(CvSavegameWriter& writer) const;
};

struct EventMessage
{
	CvWString szDescription;
	int iExpirationTurn;
	int iX;
	int iY;

	// python friendly accessors
	std::wstring getDescription() const { return szDescription;	}
};

struct PlotExtraYield
{
	int m_iX;
	int m_iY;
	std::vector<int> m_aeExtraYield;

	void read(CvSavegameReader& reader);
	void write(CvSavegameWriter& writer) const;
};

typedef std::vector< std::pair<BuildingClassTypes, int> > BuildingChangeArray;

struct BuildingYieldChange
{
	BuildingClassTypes eBuildingClass;
	YieldTypes eYield;
	int iChange;

	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);

	void read(CvSavegameReader& reader);
	void write(CvSavegameWriter& writer) const;
};

struct DllExport FOWVis
{
	uint uiCount;
	POINT* pOffsets;  // array of "Offset" points

	// python friendly accessors
	POINT getOffsets(int i) const { return pOffsets[i]; }
};

struct DllExport PBGameSetupData
{
	PBGameSetupData();

	int iSize;
	int iClimate;
	int iSeaLevel;
	int iSpeed;
	int iEra;

	int iMaxTurns;
	int iCityElimination;
	int iAdvancedStartPoints;
	int iTurnTime;

	int iNumCustomMapOptions;
	int * aiCustomMapOptions;
	int getCustomMapOption(int iOption) {return aiCustomMapOptions[iOption];}

	int iNumVictories;
	bool * abVictories;
	bool getVictory(int iVictory) {return abVictories[iVictory];}

	std::wstring szMapName;
	std::wstring getMapName() {return szMapName;}

	std::vector<bool> abOptions;
	bool getOptionAt(int iOption) {return abOptions[iOption];}

	std::vector<bool> abMPOptions;
	bool getMPOptionAt(int iOption) {return abMPOptions[iOption];}
};

struct DllExport PBPlayerSetupData
{
	int iWho;
	int iCiv;
	int iLeader;
	int iTeam;
	int iDifficulty;

	std::wstring szStatusText;
	std::wstring getStatusText() {return szStatusText;}
};

struct DllExport PBPlayerAdminData
{
	std::wstring szName;
	std::wstring getName() {return szName;}
	std::wstring szPing;
	std::wstring getPing() {return szPing;}
	std::wstring szScore;
	std::wstring getScore() {return szScore;}
	bool bHuman;
	bool bClaimed;
	bool bTurnActive;
};

class CvUnit;
class CvPlot;

//! An enumeration for indexing units within a CvBattleDefinition
enum BattleUnitTypes
{
	BATTLE_UNIT_ATTACKER,	//!< Index referencing the attacking unit
	BATTLE_UNIT_DEFENDER,	//!< Index referencing the defending unit
	BATTLE_UNIT_COUNT		//!< The number of unit index references
};

void checkBattleUnitType(BattleUnitTypes unitType);

//!< An enumeration for indexing times within the CvBattleDefinition
enum BattleTimeTypes
{
	BATTLE_TIME_BEGIN,
	BATTLE_TIME_RANGED,
	BATTLE_TIME_END,
	BATTLE_TIME_COUNT
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  STRUCT:      CvBattleRound
//!  \brief		Represents a single round within a battle.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class DllExport CvBattleRound
{
public:
	CvBattleRound();
	bool isValid() const;

	bool isRangedRound() const;
	void setRangedRound(bool value);

	int getWaveSize() const;
	void setWaveSize(int size);

	int getNumKilled(BattleUnitTypes unitType) const;
	void setNumKilled(BattleUnitTypes unitType, int value);
	void addNumKilled(BattleUnitTypes unitType, int increment);

	int getNumAlive(BattleUnitTypes unitType) const;
	void setNumAlive(BattleUnitTypes unitType, int value);

private:
	int		m_aNumKilled[BATTLE_UNIT_COUNT];		//!< The number of units killed during this round for both sides
	int		m_aNumAlive[BATTLE_UNIT_COUNT];		//!< The number of units alive at the end of this round for both sides
	int		m_iWaveSize;				//!< The number of units that can perform exchanges
	bool	m_bRangedRound;				//!< true if this round is ranged, false otherwise
};

//------------------------------------------------------------------------------------------------

typedef std::vector<CvBattleRound> CvBattleRoundVector;		//!< Type declaration for a collection of battle round definitions

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvMissionDefinition
//!  \brief		Base mission definition struct
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class DllExport CvMissionDefinition
{
public:
	CvMissionDefinition();

	MissionTypes getMissionType() const;
	void setMissionType(MissionTypes missionType);

	float getMissionTime() const;
	void setMissionTime(float time);

	CvUnit *getUnit(BattleUnitTypes unitType) const;
	void setUnit(BattleUnitTypes unitType, CvUnit *unit);

	const CvPlot *getPlot() const;
	void setPlot(const CvPlot *plot);

protected:
	MissionTypes		m_eMissionType;			//!< The type of event
	CvUnit *			m_aUnits[BATTLE_UNIT_COUNT];		//!< The units involved
	float				m_fMissionTime;			//!< The amount of time that the event will take
	const CvPlot *		m_pPlot;					//!< The plot associated with the event
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  STRUCT:     CvBattleDefinition
//!  \brief		A definition passed to CvBattleManager to start a battle between units
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class DllExport CvBattleDefinition : public CvMissionDefinition
{
public:
	CvBattleDefinition();
	CvBattleDefinition( const CvBattleDefinition & kCopy );

	int getDamage(BattleUnitTypes unitType, BattleTimeTypes timeType) const;
	void setDamage(BattleUnitTypes unitType, BattleTimeTypes timeType, int damage);
	void addDamage(BattleUnitTypes unitType, BattleTimeTypes timeType, int increment);

	bool isOneStrike() const;

	bool isAdvanceSquare() const;
	void setAdvanceSquare(bool advanceSquare);

	int getNumRangedRounds() const;
	void setNumRangedRounds(int count);
	void addNumRangedRounds(int increment);

	int getNumMeleeRounds() const;
	void setNumMeleeRounds(int count);
	void addNumMeleeRounds(int increment);

	int getNumBattleRounds() const;
	void clearBattleRounds();
	CvBattleRound &getBattleRound(int index);
	const CvBattleRound &getBattleRound(int index) const;
	void addBattleRound(const CvBattleRound &round);
	void setBattleRound(int index, const CvBattleRound &round);

private:
	void checkBattleTimeType(BattleTimeTypes timeType) const;
	void checkBattleRound(int index) const;

	int	m_aDamage[BATTLE_UNIT_COUNT][BATTLE_TIME_COUNT];	//!< The beginning damage of the units
	int	m_iNumRangedRounds;				//!< The number of ranged rounds
	int	m_iNumMeleeRounds;				//!< The number of melee rounds
	bool m_bAdvanceSquare;					//!< true if the attacking unit should move into the new square
	CvBattleRoundVector	m_aBattleRounds;					//!< The rounds that define the battle plan
};

struct DllExport CvWidgetDataStruct
{
	int m_iData1;										//	The first bit of data
	int m_iData2;										//	The second piece of data

	bool m_bOption;									//	A boolean piece of data

	WidgetTypes m_eWidgetType;			//	What the 'type' of this widget is (for parsing help and executing actions)
};

#endif	// CVSTRUCTS_H
