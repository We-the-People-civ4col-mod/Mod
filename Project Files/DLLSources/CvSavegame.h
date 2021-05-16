#ifndef SAVEGAME_H
#define SAVEGAME_H

class FDataStreamBase;
enum SavegameVariableTypes;
class FVariable;
enum eVariableType;
struct CvPopupButtonPython;

#include "JustInTimeArray.h"
#include "BoolArray.h"
#include "LinkedList.h"

#include "CvDiploParameters.h"
#include "CvPopupInfo.h"
#include "CvReplayMessage.h"

#define CHUNK_SIZE (MAX_UNSIGNED_SHORT)
#define COMPRESS_THRESHOLD (MAX_UNSIGNED_CHAR)

// enum values for each class used in the savegame.
// ideally each class using SavegameVariableTypes should have an index here.
// Extending to the end is savegame safe. Altering the existing layout isn't.
enum SavegameClassTypes
{
	SAVEGAME_CLASS_AREA,
	SAVEGAME_CLASS_MAP,
	SAVEGAME_CLASS_PLOT,
	SAVEGAME_CLASS_UNIT,
	SAVEGAME_CLASS_UNIT_AI,
	SAVEGAME_CLASS_CITY,
	SAVEGAME_CLASS_CITY_AI,
	SAVEGAME_CLASS_PLAYER,
	SAVEGAME_CLASS_PLAYER_AI,
	SAVEGAME_CLASS_POPUPINFO,
	SAVEGAME_CLASS_DIPLOPARAMETERS,
	SAVEGAME_CLASS_TALKINGHEADMESSAGE,
	SAVEGAME_CLASS_TRADEROUTE,
	SAVEGAME_CLASS_TRADEROUTEGROUP,
	SAVEGAME_CLASS_SELECTIONGROUP,
	SAVEGAME_CLASS_SELECTIONGROUP_AI,
	SAVEGAME_CLASS_GAME,
	SAVEGAME_CLASS_GAME_AI,
	SAVEGAME_CLASS_DEAL,
	SAVEGAME_CLASS_REPLAYMESSAGE,
	SAVEGAME_CLASS_TEAM,
	SAVEGAME_CLASS_TEAM_AI,

	NUM_SAVEGAME_CLASS_TYPES,

	FIRST_SAVEGAME_CLASS_TYPES = 0,
};

SET_ENUM_OPERATORS_AND_FORBID(SavegameClassTypes)

class CvSavegameReaderBase;
class CvSavegameWriterBase;

class CvSavegameReader
{
public:
	CvSavegameReader(CvSavegameReaderBase& readerBase);
	CvSavegameReader(const CvSavegameReader& reader);

	bool isDebug() const;

	void AssignClassType(SavegameClassTypes eType);

	void Read(SavegameVariableTypes& variable);

	void Read(unsigned long& variable);
	void Read(double& variable);
	void Read(int& variable);
	void Read(short& variable);
	void Read(char& variable);

	void Read(unsigned int& variable);
	void Read(unsigned short& variable);
	void Read(byte& variable);

	void Read(bool& variable);
	
	void Read(CvString& szString);
	void Read(CvWString& szString);

	// allocates memory
	void Read(char* szString);
	// allocates memory
	void Read(wchar* szString);
	
	template<class T1, class T2, int T3, class T4, class T5>
	void Read(EnumMapBase<T1, T2, T3, T4, T5>& em);

	template<class T1, class T2, class T3, int T4>
	void Read(EnumMap2DDefault<T1, T2, T3, T4>& em);

	template<class T>
	void Read(PlayerArrayBase<T>& array);

	template<class T>
	void Read(JustInTimeArray<T>& jitArray);

	template<class T>
	void Read(std::vector<T>& vec);

	template<class T>
	void Read(std::vector<T*>& vec);

	template<class T>
	void Read(std::list<T>& vec);

	template<class T>
	void Read(std::list<T*>& vec);

	template<class T1, class T2>
	void Read(std::pair<T1,T2>& pair);

	template<class T>
	void Read(FFreeListTrashArray<T>& array);

	template<class T>
	void Read(CvIdVector<T>& vec);

	template<class T>
	void Read(CLinkList<T>& lList);

	void Read(BoolArray& baArray);
	void Read(PlayerBoolArrayBase& array);
	void Read(CvTurnScoreMap& vec);
	void Read(CvEventMap& vec);

	// workaround because we can't use references on bitfields
	template<typename T>
	T ReadBitfield(T variable);

	// Add all enums used in savegames (single byte)
	void Read(ActivityTypes         & variable) { ReadEnum(variable); }
	void Read(AreaAITypes           & variable) { ReadEnum(variable); }
	void Read(AutomateTypes         & variable) { ReadEnum(variable); }
	void Read(ButtonPopupTypes      & variable) { ReadEnum(variable); }
	void Read(CardinalDirectionTypes& variable) { ReadEnum(variable); }
	void Read(CalendarTypes         & variable) { ReadEnum(variable); }
	void Read(ChatTargetTypes       & variable) { ReadEnum(variable); }
	void Read(CityPlotTypes         & variable) { ReadEnum(variable); }
	void Read(CustomMapOptionTypes  & variable) { ReadEnum(variable); }
	void Read(DiploCommentTypes     & variable) { ReadEnum(variable); }
	void Read(DirectionTypes        & variable) { ReadEnum(variable); }
	void Read(eVariableType         & variable) { ReadEnum(variable); }
	void Read(GameType              & variable) { ReadEnum(variable); }
	void Read(GameStateTypes        & variable) { ReadEnum(variable); }
	void Read(InterfaceMessageTypes & variable) { ReadEnum(variable); }
	void Read(MissionAITypes        & variable) { ReadEnum(variable); }
	void Read(MissionTypes          & variable) { ReadEnum(variable); }
	void Read(OrderTypes            & variable) { ReadEnum(variable); }
	void Read(PlayerTypes           & variable) { ReadEnum(variable); }
	void Read(PlotTypes             & variable) { ReadEnum(variable); }
	void Read(ReplayMessageTypes    & variable) { ReadEnum(variable); }
	void Read(SlotClaim             & variable) { ReadEnum(variable); }
	void Read(SlotStatus            & variable) { ReadEnum(variable); }
	void Read(TeamTypes             & variable) { ReadEnum(variable); }
	void Read(TurnTimerTypes        & variable) { ReadEnum(variable); }
	void Read(TradeableItems        & variable) { ReadEnum(variable); }
	void Read(UnitAIStates          & variable) { ReadEnum(variable); }
	void Read(UnitTravelStates      & variable) { ReadEnum(variable); }
	void Read(WarPlanTypes          & variable) { ReadEnum(variable); }


	// everything linked to xml file enums
	void Read(AchieveTypes          & variable) { ReadXmlEnum(variable); }
	void Read(ArtStyleTypes         & variable) { ReadXmlEnum(variable); }
	void Read(BonusTypes            & variable) { ReadXmlEnum(variable); }
	void Read(BuildTypes            & variable) { ReadXmlEnum(variable); }
	void Read(BuildingTypes         & variable) { ReadXmlEnum(variable); }
	void Read(BuildingClassTypes    & variable) { ReadXmlEnum(variable); }
	void Read(SpecialBuildingTypes  & variable) { ReadXmlEnum(variable); }
	void Read(CivEffectTypes        & variable) { ReadXmlEnum(variable); }
	void Read(CivicTypes            & variable) { ReadXmlEnum(variable); }
	void Read(CivicOptionTypes      & variable) { ReadXmlEnum(variable); }
	void Read(CivilizationTypes     & variable) { ReadXmlEnum(variable); }
	void Read(ClimateTypes          & variable) { ReadXmlEnum(variable); }
	void Read(ColorTypes            & variable) { ReadXmlEnum(variable); }
	void Read(CultureLevelTypes     & variable) { ReadXmlEnum(variable); }
	void Read(DiplomacyTypes        & variable) { ReadXmlEnum(variable); }
	void Read(EmphasizeTypes        & variable) { ReadXmlEnum(variable); }
	void Read(EraTypes              & variable) { ReadXmlEnum(variable); }
	void Read(EuropeTypes           & variable) { ReadXmlEnum(variable); }
	void Read(EventTypes            & variable) { ReadXmlEnum(variable); }
	void Read(EventTriggerTypes     & variable) { ReadXmlEnum(variable); }
	void Read(FatherTypes           & variable) { ReadXmlEnum(variable); }
	void Read(FatherPointTypes      & variable) { ReadXmlEnum(variable); }
	void Read(FeatTypes             & variable) { ReadXmlEnum(variable); }
	void Read(FeatureTypes          & variable) { ReadXmlEnum(variable); }
	void Read(GameOptionTypes       & variable) { ReadXmlEnum(variable); }
	void Read(GameSpeedTypes        & variable) { ReadXmlEnum(variable); }
	void Read(GoodyTypes            & variable) { ReadXmlEnum(variable); }
	void Read(HandicapTypes         & variable) { ReadXmlEnum(variable); }
	void Read(HurryTypes            & variable) { ReadXmlEnum(variable); }
	void Read(InvisibleTypes        & variable) { ReadXmlEnum(variable); }
	void Read(ImprovementTypes      & variable) { ReadXmlEnum(variable); }
	void Read(LeaderHeadTypes       & variable) { ReadXmlEnum(variable); }
	void Read(MemoryTypes           & variable) { ReadXmlEnum(variable); }
	void Read(PlayerColorTypes      & variable) { ReadXmlEnum(variable); }
	void Read(PlayerOptionTypes     & variable) { ReadXmlEnum(variable); }
	void Read(ProfessionTypes       & variable) { ReadXmlEnum(variable); }
	void Read(PromotionTypes        & variable) { ReadXmlEnum(variable); }
	void Read(RouteTypes            & variable) { ReadXmlEnum(variable); }
	void Read(SeaLevelTypes         & variable) { ReadXmlEnum(variable); }
	void Read(TerrainTypes          & variable) { ReadXmlEnum(variable); }
	void Read(TraitTypes            & variable) { ReadXmlEnum(variable); }
	void Read(UnitTypes             & variable) { ReadXmlEnum(variable); }
	void Read(UnitAITypes           & variable) { ReadXmlEnum(variable); }
	void Read(UnitClassTypes        & variable) { ReadXmlEnum(variable); }
	void Read(UnitCombatTypes       & variable) { ReadXmlEnum(variable); }
	void Read(SpecialUnitTypes      & variable) { ReadXmlEnum(variable); }
	void Read(VictoryTypes          & variable) { ReadXmlEnum(variable); }
	void Read(YieldTypes            & variable) { ReadXmlEnum(variable); }
	void Read(WorldSizeTypes        & variable) { ReadXmlEnum(variable); }

	// class wrappers
	void Read(BuildingYieldChange   & variable) { variable.read(*this); }
	void Read(CvDiploParameters     & variable);
	void Read(CvPopupButtonPython   & variable);
	void Read(CvPopupInfo           & variable);
	void Read(CvTalkingHeadMessage  & variable);
	void Read(CvTradeRouteGroup     & variable);
	void Read(FVariable             & variable);
	void Read(CvRandom              & variable) { variable.read(*this); }
	void Read(CvReplayMessage       & variable) { variable.read(*this); }
	void Read(CvTradeRoute          & variable) { variable.read(*this); }
	void Read(CvUnit                & variable) { variable.read(*this); }
	void Read(CvUnitAI              & variable) { variable.read(*this); }
	void Read(EventTriggeredData    & variable) { variable.read(*this); }
	void Read(IDInfo                & variable) { variable.read(*this); }
	void Read(MissionData           & variable) { variable.read(*this); }
	void Read(OrderData             & variable) { variable.read(*this); }
	void Read(PlotExtraYield        & variable) { variable.read(*this); }
	void Read(TradeData             & variable) { variable.read(*this); }

	int ConvertIndex(JITarrayTypes eType, int iIndex) const;
	int GetXmlSize(JITarrayTypes eType) const;

	template<class T>
	void Discard();

#ifndef MakefileCompilation
	// remove IntelliSense errors, which causes bogus red lines in the code
	// This isn't compiled and won't effect the game at runtime

	void Read(wchar& variable); // the makefile will use short instead, but IntelliSense fail to detect this
#endif

private:

	template<typename T>
	void ReadEnum(T& variable);

	template<typename T>
	void ReadXmlEnum(T& variable);
	void ReadXmlEnum(int& iVariable, JITarrayTypes eType);

	void Read(byte* var, unsigned int iSize);
	int ReadBytes(int iNumBytes);
	unsigned int ReadUnsignedBytes(int iNumBytes);

	SavegameClassTypes m_eClassType;

	CvSavegameReaderBase& m_ReaderBase;

	// debug strings
	CvString m_szPreviousType;
	CvString m_szType;
};

class CvSavegameWriter
{
public:
	CvSavegameWriter(CvSavegameWriterBase& writerbase);
	CvSavegameWriter(const CvSavegameWriter& writer);

	bool isDebug() const;

	void AssignClassType(SavegameClassTypes eType);

	template<enum T>
	void Write(T variable);

	void Write(unsigned long dVar);
	void Write(double dVar);
	void Write(int iVar);
	void Write(short iVar);
	void Write(char iVar);
	void Write(unsigned int iVar);
	void Write(unsigned short iVar);
	void Write(byte iVar);

	void Write(bool bVar);

	void Write(const CvString  & szString);
	void Write(const CvWString & szString);
	void Write(const char      * szString);
	void Write(const wchar     * szString);

	void Write(BoolArray& baArray);

	template<class T>
	void Write(PlayerArrayBase<T>& array);
	
	template<class T>
	void Write(JustInTimeArray<T>& jitArray);

	template<class T>
	void Write(const std::vector<T>& vec);

	template<class T>
	void Write(SavegameVariableTypes eType, const std::vector<T>& vec);

	template<class T>
	void Write(const std::vector<T*>& vec);

	template<class T>
	void Write(const std::list<T>& vec);

	template<class T>
	void Write(SavegameVariableTypes eType, const std::list<T>& vec);

	template<class T>
	void Write(const std::list<T*>& vec);

	template<class T1,class T2>
	void Write(const std::pair<T1,T2>& pair);
	
	template<class T>
	void Write(SavegameVariableTypes eType, FFreeListTrashArray<T>& array);
	
	template<class T>
	void Write(SavegameVariableTypes eType, CvIdVector<T>& vec);

	template<class T>
	void Write(SavegameVariableTypes eType, const std::vector<T*>& vec);

	void Write(SavegameVariableTypes eType);
	void Write(SavegameVariableTypes eType, const CvString  & szString);
	void Write(SavegameVariableTypes eType, const CvWString & szString);
	void Write(SavegameVariableTypes eType, const char      * szString);
	void Write(SavegameVariableTypes eType, const wchar     * szString);
	void Write(SavegameVariableTypes eType, BoolArray& baArray);
	void Write(SavegameVariableTypes eType, const PlayerBoolArrayBase& array);
	void Write(SavegameVariableTypes eType, const IDInfo& idInfo);
	void Write(SavegameVariableTypes eType, const CvTurnScoreMap& idInfo);
	void Write(SavegameVariableTypes eType, CvEventMap& idInfo);
	void Write(SavegameVariableTypes eType, CvRandom& rand);

	template<class T>
	void Write(SavegameVariableTypes eType, const PlayerArrayBase<T>& array);

	template<class T>
	void Write(SavegameVariableTypes eType, T eVariable, T eDefault);

	template<class T>
	void Write(SavegameVariableTypes eType, JustInTimeArray<T>& jitArray);

	template<class T1, class T2, int T3, class T4, class T5>
	void Write(EnumMapBase<T1, T2, T3, T4, T5>& em);

	template<class T1, class T2, int T3, class T4, class T5>
	void Write(SavegameVariableTypes eType, EnumMapBase<T1, T2, T3, T4, T5>& em);

	template<class T1, class T2, class T3, int T4>
	void Write(EnumMap2DDefault<T1, T2, T3, T4>& em);

	template<class T1, class T2, class T3, int T4>
	void Write(SavegameVariableTypes eType, EnumMap2DDefault<T1, T2, T3, T4>& em);

	template<class T>
	void Write(SavegameVariableTypes eType, const CLinkList<T>& lList);

	// Add all enums used in savegames (single byte)
	void Write(ActivityTypes          variable) { WriteEnum(variable); }
	void Write(AreaAITypes            variable) { WriteEnum(variable); }
	void Write(AutomateTypes          variable) { WriteEnum(variable); }
	void Write(ButtonPopupTypes       variable) { WriteEnum(variable); }
	void Write(CardinalDirectionTypes variable) { WriteEnum(variable); }
	void Write(CalendarTypes          variable) { WriteEnum(variable); }
	void Write(ChatTargetTypes        variable) { WriteEnum(variable); }
	void Write(CityPlotTypes          variable) { WriteEnum(variable); }
	void Write(CustomMapOptionTypes   variable) { WriteEnum(variable); }
	void Write(DiploCommentTypes      variable) { WriteEnum(variable); }
	void Write(DirectionTypes         variable) { WriteEnum(variable); }
	void Write(eVariableType          variable) { WriteEnum(variable); }
	void Write(GameType               variable) { WriteEnum(variable); }
	void Write(GameStateTypes         variable) { WriteEnum(variable); }
	void Write(InterfaceMessageTypes  variable) { WriteEnum(variable); }
	void Write(MissionAITypes         variable) { WriteEnum(variable); }
	void Write(MissionTypes           variable) { WriteEnum(variable); }
	void Write(OrderTypes             variable) { WriteEnum(variable); }
	void Write(PlayerTypes            variable) { WriteEnum(variable); }
	void Write(PlotTypes              variable) { WriteEnum(variable); }
	void Write(ReplayMessageTypes     variable) { WriteEnum(variable); }
	void Write(SlotClaim              variable) { WriteEnum(variable); }
	void Write(SlotStatus             variable) { WriteEnum(variable); }
	void Write(TeamTypes              variable) { WriteEnum(variable); }
	void Write(TurnTimerTypes         variable) { WriteEnum(variable); }
	void Write(TradeableItems         variable) { WriteEnum(variable); }
	void Write(UnitAIStates           variable) { WriteEnum(variable); }
	void Write(UnitTravelStates       variable) { WriteEnum(variable); }
	void Write(WarPlanTypes           variable) { WriteEnum(variable); }

	// everything linked to xml file enums
	void Write(AchieveTypes          variable) { WriteXmlEnum(variable); }
	void Write(ArtStyleTypes         variable) { WriteXmlEnum(variable); }
	void Write(BonusTypes            variable) { WriteXmlEnum(variable); }
	void Write(BuildTypes            variable) { WriteXmlEnum(variable); }
	void Write(BuildingTypes         variable) { WriteXmlEnum(variable); }
	void Write(BuildingClassTypes    variable) { WriteXmlEnum(variable); }
	void Write(SpecialBuildingTypes  variable) { WriteXmlEnum(variable); }
	void Write(CivEffectTypes        variable) { WriteXmlEnum(variable); }
	void Write(CivicTypes            variable) { WriteXmlEnum(variable); }
	void Write(CivicOptionTypes      variable) { WriteXmlEnum(variable); }
	void Write(CivilizationTypes     variable) { WriteXmlEnum(variable); }
	void Write(ClimateTypes          variable) { WriteXmlEnum(variable); }
	void Write(ColorTypes            variable) { WriteXmlEnum(variable); }
	void Write(CultureLevelTypes     variable) { WriteXmlEnum(variable); }
	void Write(DiplomacyTypes        variable) { WriteXmlEnum(variable); }
	void Write(EmphasizeTypes        variable) { WriteXmlEnum(variable); }
	void Write(EraTypes              variable) { WriteXmlEnum(variable); }
	void Write(EuropeTypes           variable) { WriteXmlEnum(variable); }
	void Write(EventTypes            variable) { WriteXmlEnum(variable); }
	void Write(EventTriggerTypes     variable) { WriteXmlEnum(variable); }
	void Write(FatherTypes           variable) { WriteXmlEnum(variable); }
	void Write(FatherPointTypes      variable) { WriteXmlEnum(variable); }
	void Write(FeatTypes             variable) { WriteXmlEnum(variable); }
	void Write(FeatureTypes          variable) { WriteXmlEnum(variable); }
	void Write(GameOptionTypes       variable) { WriteXmlEnum(variable); }
	void Write(GameSpeedTypes        variable) { WriteXmlEnum(variable); }
	void Write(GoodyTypes            variable) { WriteXmlEnum(variable); }
	void Write(HandicapTypes         variable) { WriteXmlEnum(variable); }
	void Write(HurryTypes            variable) { WriteXmlEnum(variable); }
	void Write(ImprovementTypes      variable) { WriteXmlEnum(variable); }
	void Write(LeaderHeadTypes       variable) { WriteXmlEnum(variable); }
	void Write(MemoryTypes           variable) { WriteXmlEnum(variable); }
	void Write(PlayerColorTypes      variable) { WriteXmlEnum(variable); }
	void Write(PlayerOptionTypes     variable) { WriteXmlEnum(variable); }
	void Write(ProfessionTypes       variable) { WriteXmlEnum(variable); }
	void Write(PromotionTypes        variable) { WriteXmlEnum(variable); }
	void Write(RouteTypes            variable) { WriteXmlEnum(variable); }
	void Write(SeaLevelTypes         variable) { WriteXmlEnum(variable); }
	void Write(TerrainTypes          variable) { WriteXmlEnum(variable); }
	void Write(TraitTypes            variable) { WriteXmlEnum(variable); }
	void Write(UnitTypes             variable) { WriteXmlEnum(variable); }
	void Write(UnitAITypes           variable) { WriteXmlEnum(variable); }
	void Write(UnitClassTypes        variable) { WriteXmlEnum(variable); }
	void Write(UnitCombatTypes       variable) { WriteXmlEnum(variable); }
	void Write(SpecialUnitTypes      variable) { WriteXmlEnum(variable); }
	void Write(VictoryTypes          variable) { WriteXmlEnum(variable); }
	void Write(YieldTypes            variable) { WriteXmlEnum(variable); }
	void Write(WorldSizeTypes        variable) { WriteXmlEnum(variable); }

	// class wrappers
	void Write(BuildingYieldChange  &variable) { variable.write(*this); }
	void Write(CvDiploParameters    &variable);
	void Write(CvPopupButtonPython  &variable);
	void Write(CvPopupInfo          &variable);
	void Write(CvTalkingHeadMessage &variable);
	void Write(CvTradeRouteGroup    &variable);
	void Write(FVariable            &variable);
	void Write(CvRandom             &variable) { variable.write(*this); }
	void Write(CvReplayMessage      &variable) { variable.write(*this); }
	void Write(CvTradeRoute         &variable) { variable.write(*this); }
	void Write(CvUnit               &variable) { variable.write(*this); }
	void Write(CvUnitAI             &variable) { variable.write(*this); }
	void Write(EventTriggeredData   &variable) { variable.write(*this); }
	void Write(IDInfo               &variable) { variable.write(*this); }
	void Write(MissionData          &variable) { variable.write(*this); }
	void Write(OrderData            &variable) { variable.write(*this); }
	void Write(PlotExtraYield       &variable) { variable.write(*this); }
	void Write(TradeData            &variable) { variable.write(*this); }

	// get the amount of bytes needed to save the variable in question
	// also tells the savegame that a conversion table is needed
	int GetXmlByteSize(JITarrayTypes eType);
	int GetXmlSize(JITarrayTypes eType);

private:

	template<typename T>
	void WriteEnum(T variable);

	template<typename T>
	void WriteXmlEnum(T variable);
	void WriteXmlEnum(int iVariable, JITarrayTypes eType);
	void Write(byte*var, unsigned int iSize);

	SavegameClassTypes m_eClassType;

	CvSavegameWriterBase& m_writerbase;
	std::vector<byte>& m_vector;
};

//
// Inline functions to allow easy handling of templates
//

//
// Reader
//

template<typename T>
inline void CvSavegameReader::ReadEnum(T& variable)
{
	char iBuffer;
	Read(iBuffer);
	variable = (T)iBuffer;
}

template<typename T>
inline void CvSavegameReader::ReadXmlEnum(T& variable)
{
	int iBuffer;
	ReadXmlEnum(iBuffer, getJITarrayType(variable));
	variable = static_cast<T>(iBuffer);
}

template<typename T>
inline T CvSavegameReader::ReadBitfield(T variable)
{
	Read(variable);
	return variable;
}

template<class T1, class T2, int T3, class T4, class T5>
inline void CvSavegameReader::Read(EnumMapBase<T1, T2, T3, T4, T5>& em)
{
	em.Read(*this);
}

template<class T1, class T2, class T3, int T4>
inline void CvSavegameReader::Read(EnumMap2DDefault<T1, T2, T3, T4>& em)
{
	em.Read(*this);
}

template<class T>
inline void CvSavegameReader::Read(PlayerArrayBase<T>& array)
{
	array.Read(*this);
}

template<class T>
inline void CvSavegameReader::Read(JustInTimeArray<T>& jitArray)
{
	jitArray.Read(*this);
}

template<class T>
inline void CvSavegameReader::Read(std::vector<T>& vec)
{
	unsigned short iLength;
	Read(iLength);
	vec.resize(iLength);
	for (std::vector<T>::iterator it = vec.begin(); it != vec.end(); ++it)
	{
		Read(*it);
	}
}

template<>
inline void CvSavegameReader::Read(std::vector<CvUnit*>& vec)
{
	// specialized function because CvCity::m_aPopulationUnits is a vector of CvUnit.
	// however CvUnit is an abstract class and needs to be allocated with CvUnitAI.

	unsigned short iLength;
	Read(iLength);
	vec.resize(iLength);
	for (std::vector<CvUnit*>::iterator it = vec.begin(); it != vec.end(); ++it)
	{
		*it = new CvUnitAI;
		Read(static_cast<CvUnitAI&>(**it));
	}
}

template<class T>
inline void CvSavegameReader::Read(std::vector<T*>& vec)
{
	unsigned short iLength;
	Read(iLength);
	vec.resize(iLength);
	for (std::vector<T*>::iterator it = vec.begin(); it != vec.end(); ++it)
	{
		*it = new T;
		Read(**it);
	}
}

template<class T>
inline void CvSavegameReader::Read(std::list<T>& vec)
{
	unsigned short iLength;
	Read(iLength);
	vec.resize(iLength);
	for (std::list<T>::iterator it = vec.begin(); it != vec.end(); ++it)
	{
		Read(*it);
	}
}

template<class T>
inline void CvSavegameReader::Read(std::list<T*>& vec)
{
	unsigned short iLength;
	Read(iLength);
	vec.resize(iLength);
	for (std::list<T*>::iterator it = vec.begin(); it != vec.end(); ++it)
	{
		*it = new T;
		Read(**it);
	}
}

template<class T1, class T2>
void CvSavegameReader::Read(std::pair<T1,T2>& pair)
{
	Read(pair.first);
	Read(pair.second);
}

template<class T>
inline void CvSavegameReader::Read(FFreeListTrashArray<T>& array)
{
	ReadStreamableFFreeListTrashArray(array, *this);
}

template<class T>
inline void CvSavegameReader::Read(CvIdVector<T>& vec)
{
	vec.Read(this);
}

template<class T>
inline void CvSavegameReader::Read(CLinkList<T>& lList)
{
	lList.Read(*this);
}

// read data and discard without using it
// used for making savegame backward compatible if something used to be saved, but is no longer needed
// most likely example is if the value is recalculated on load
template<class T>
inline void CvSavegameReader::Discard()
{
	T buffer = (T)0;
	Read(buffer);
}


//
// Writer
//

template<enum T>
inline void CvSavegameWriter::Write(T variable)
{
	Write((byte*)&variable, sizeof(T));
}

template<class T>
inline void CvSavegameWriter::Write(PlayerArrayBase<T>& array)
{
	array.Write(*this);
}

template<class T>
inline void CvSavegameWriter::Write(JustInTimeArray<T>& jitArray)
{
	jitArray.Write(*this);
}

template<class T>
inline void CvSavegameWriter::Write(const std::vector<T>& vec)
{
	unsigned short iLength = vec.size();
	Write(iLength);
	for (std::vector<T>::const_iterator it = vec.begin(); it != vec.end(); ++it)
	{
		Write((T)*it);
	}
}

template<class T>
inline void CvSavegameWriter::Write(SavegameVariableTypes eType, const std::vector<T>& vec)
{
	if (vec.size() > 0)
	{
		Write(eType);
		Write(vec);
	}
}

template<class T>
inline void CvSavegameWriter::Write(const std::vector<T*>& vec)
{
	unsigned short iLength = vec.size();
	Write(iLength);
	for (std::vector<T*>::const_iterator it = vec.begin(); it != vec.end(); ++it)
	{
		Write(**it);
	}
}

template<>
inline void CvSavegameWriter::Write(const std::vector<CvUnit*>& vec) {
	unsigned short iLength = vec.size();
	Write(iLength);
	for (std::vector<CvUnit*>::const_iterator it = vec.begin(); it != vec.end(); ++it) {
		Write(static_cast <CvUnitAI&>(**it));
	}
}

template<class T>
inline void CvSavegameWriter::Write(const std::list<T>& vec)
{
	unsigned short iLength = vec.size();
	Write(iLength);
	for (std::list<T>::const_iterator it = vec.begin(); it != vec.end(); ++it)
	{
		Write((T)*it);
	}
}

template<class T>
inline void CvSavegameWriter::Write(SavegameVariableTypes eType, const std::list<T>& vec)
{
	if (vec.size() > 0)
	{
		Write(eType);
		Write(vec);
	}
}

template<class T>
inline void CvSavegameWriter::Write(const std::list<T*>& vec)
{
	unsigned short iLength = vec.size();
	Write(iLength);
	for (std::list<T*>::const_iterator it = vec.begin(); it != vec.end(); ++it)
	{
		Write(**it);
	}
}

template<class T1,class T2>
	void CvSavegameWriter::Write(const std::pair<T1,T2>& pair)
	{
		Write(pair.first);
		Write(pair.second);
	}

template<class T>
inline void CvSavegameWriter::Write(SavegameVariableTypes eType, FFreeListTrashArray<T>& array)
{
	if (array.getCount() > 0)
	{
		Write(eType);
		WriteStreamableFFreeListTrashArray(array, *this);
	}
}

template<class T>
inline void CvSavegameWriter::Write(SavegameVariableTypes eType, CvIdVector<T>& vec)
{
	if (vec.size() > 0)
	{
		Write(eType);
		vec.Write(this);
	}
}

template<class T>
inline void CvSavegameWriter::Write(SavegameVariableTypes eType, const std::vector<T*>& vec)
{
	if (vec.size() > 0)
	{
		Write(eType);
		Write(vec);
	}
}

template<class T>
inline void CvSavegameWriter::Write(SavegameVariableTypes eType, const PlayerArrayBase<T>& array)
{
	if (array.hasContent())
	{
		Write(eType);
		Write(array);
	}
}

template<class T>
inline void CvSavegameWriter::Write(SavegameVariableTypes eType, T eVariable, T eDefault)
{
	if (eVariable != eDefault)
	{
		Write(eType);
		Write(eVariable);
	}
}

template<class T>
inline void CvSavegameWriter::Write(SavegameVariableTypes eType, JustInTimeArray<T>& jitArray)
{
	if (jitArray.hasContent())
	{
		Write(eType);
		Write(jitArray);
	}
}

template<class T1, class T2, int T3, class T4, class T5>
inline void CvSavegameWriter::Write(EnumMapBase<T1, T2, T3, T4, T5>& em)
{
	em.Write(*this);
}

template<class T1, class T2, int T3, class T4, class T5>
inline void CvSavegameWriter::Write(SavegameVariableTypes eType, EnumMapBase<T1, T2, T3, T4, T5>& em)
{
	if (em.hasContent())
	{
		Write(eType);
		Write(em);
	}
}

template<class T1, class T2, class T3, int T4>
inline void CvSavegameWriter::Write(EnumMap2DDefault<T1, T2, T3, T4>& em)
{
	em.Write(*this);
}

template<class T1, class T2, class T3, int T4>
inline void CvSavegameWriter::Write(SavegameVariableTypes eType, EnumMap2DDefault<T1, T2, T3, T4>& em)
{
	if (em.hasContent())
	{
		Write(eType);
		Write(em);
	}
}

template<class T>
inline void CvSavegameWriter::Write(SavegameVariableTypes eType, const CLinkList<T>& lList)
{
	if (lList.getLength()>0)
	{
		Write(eType);
		lList.Write(*this);
	}
}

template<typename T>
inline void CvSavegameWriter::WriteEnum(T variable)
{
	FAssert((int)variable >= (char)MIN_CHAR && (int)variable <= MAX_CHAR);
	char iBuffer = variable;
	Write(iBuffer);
}

template<typename T>
inline void CvSavegameWriter::WriteXmlEnum(T variable)
{
	WriteXmlEnum(variable, getJITarrayType(variable));
}

///
/// base classes
/// do not work with these directly. Make once instance when called from the exe and that's it.
/// Instantly make an instance of the reader/writer with a reference to the base class and then use the reader/writer instead.
///
/// The idea is that the reader/writer will make a new instance for each function call.
/// Since they don't actually contain data, but rather references to the data in the base class, it works like it's just one instance.
/// However since it's one instance for each function call, it will be possible to add variables, which are local to the function in question.
///
/// This allows the reader/writer to be aware of which class it's working on without losing this info by calling some other classes.
/// For instance CvMap knows it's CvMap. It calls CvPlot multiple times, which then knows it's CvPlot, but when it returns, it's back to CvMap without any extra code.
///

class CvSavegameBase
{
public:
	CvSavegameBase();

	bool isDebug() const;
	bool isCompressed() const;

protected:
	unsigned int m_iFlag;
};

class CvSavegameReaderBase
	: CvSavegameBase
{
	friend class CvSavegameReader;
public:
	CvSavegameReaderBase(FDataStreamBase* pStream);
	
	~CvSavegameReaderBase();

private:

	void Read(byte* var, unsigned int iSize);
	int ReadChunk();

	FDataStreamBase* m_pStream;
	byte* m_Memory;
	byte* m_MemoryAllocation;
	byte *m_MemoryEnd;
	unsigned int m_iSize;
	unsigned int m_iRead;

};

class CvSavegameWriterBase
	: CvSavegameBase
{
	friend class CvSavegameWriter;
public:
	CvSavegameWriterBase(FDataStreamBase* pStream);

	void WriteFile();
	void InitSavegame();
	int GetXmlByteSize(JITarrayTypes eType);

private:

	void WriteTableString(const char *szString);

	FDataStreamBase* m_pStream;
	std::vector<byte> m_vector;
	std::vector<byte> m_table;
};

#endif
