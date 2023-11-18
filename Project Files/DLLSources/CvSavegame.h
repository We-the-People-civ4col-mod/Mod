#ifndef SAVEGAME_H
#define SAVEGAME_H

class FDataStreamBase;
enum SavegameVariableTypes;
class FVariable;
enum eVariableType;
struct CvPopupButtonPython;

#include "JustInTimeArray.h"
#include "LinkedList.h"

#include "CvDiploParameters.h"
#include "CvPopupInfo.h"
#include "CvReplayMessage.h"

// savegame debugging option
// note: might not result in usable savegames in case of failure
//#define WITH_DEBUG_WRITE_SAVEGAME

int getByteSizeForXML(JITarrayTypes eType);

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
	static unsigned int getSavegameVersion();

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
	void Read(char*& szString);
	// allocates memory
	void Read(wchar*& szString);

	template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, VariableTypes TYPE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
	void Read(EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, TYPE, LENGTH_KNOWN_WHILE_COMPILING>& em);

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
	void Read(Coordinates           & variable);
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
	template<int TYPE2>
	struct ReadEnumMap
	{
		template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, VariableTypes TYPE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
		static void Read(CvSavegameReader& reader, EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, TYPE, LENGTH_KNOWN_WHILE_COMPILING>& em);

	};


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
	friend class LogIntentHelper;
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

	void Write(CvString  const& szString);
	void Write(CvWString const& szString);
	void Write(char      const* szString);
	void Write(wchar     const* szString);

	template<class T>
	void Write(JustInTimeArray<T> const& jitArray);

	template<class T>
	void Write(std::vector<T> const& vec);

	template<class T>
	void Write(SavegameVariableTypes eType, std::vector<T> const& vec);

	template<class T>
	void Write(std::vector<T*> const& vec);

	template<class T>
	void Write(std::list<T> const& vec);

	template<class T>
	void Write(SavegameVariableTypes eType, std::list<T> const& vec);

	template<class T>
	void Write(std::list<T*> const& vec);

	template<class T1, class T2>
	void Write(std::pair<T1, T2> const& pair);

	template<class T>
	void Write(SavegameVariableTypes eType, FFreeListTrashArray<T> const& array);

	template<class T>
	void Write(SavegameVariableTypes eType, CvIdVector<T> const& vec);

	template<class T>
	void Write(SavegameVariableTypes eType, std::vector<T*> const& vec);

	void Write(SavegameVariableTypes eType);
	void Write(SavegameVariableTypes eType, CvString       const& szString);
	void Write(SavegameVariableTypes eType, CvWString      const& szString);
	void Write(SavegameVariableTypes eType, char           const* szString);
	void Write(SavegameVariableTypes eType, wchar          const* szString);
	void Write(SavegameVariableTypes eType, IDInfo         const& idInfo);
	void Write(SavegameVariableTypes eType, CvTurnScoreMap const& idInfo);
	void Write(SavegameVariableTypes eType, CvEventMap     const& idInfo);
	void Write(SavegameVariableTypes eType, CvRandom       const& rand);

	void Write(SavegameVariableTypes eType, const Coordinates &variable);

	template<class T>
	void Write(SavegameVariableTypes eType, T eVariable, T eDefault);

	template<class T>
	void Write(SavegameVariableTypes eType, JustInTimeArray<T> const& jitArray);

	template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, VariableTypes TYPE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
	void Write(EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, TYPE, LENGTH_KNOWN_WHILE_COMPILING> const& em);

	template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, VariableTypes TYPE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
	void Write(SavegameVariableTypes eType, EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, TYPE, LENGTH_KNOWN_WHILE_COMPILING> const& em);

	template<class T>
	void Write(SavegameVariableTypes eType, CLinkList<T> const& lList);

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
	void Write(BuildingYieldChange  const& variable) { variable.write(*this); }
	void Write(Coordinates          const& variable);
	void Write(CvDiploParameters    const& variable);
	void Write(CvPopupButtonPython  const& variable);
	void Write(CvPopupInfo          const& variable);
	void Write(CvTalkingHeadMessage const& variable);
	void Write(CvTradeRouteGroup    const& variable);
	void Write(FVariable            const& variable);
	void Write(CvRandom             const& variable) { variable.write(*this); }
	void Write(CvReplayMessage      const& variable) { variable.write(*this); }
	void Write(CvTradeRoute         const& variable) { variable.write(*this); }
	void Write(CvUnit               const& variable) { variable.write(*this); }
	void Write(CvUnitAI             const& variable) { variable.write(*this); }
	void Write(EventTriggeredData   const& variable) { variable.write(*this); }
	void Write(IDInfo               const& variable) { variable.write(*this); }
	void Write(MissionData          const& variable) { variable.write(*this); }
	void Write(OrderData            const& variable) { variable.write(*this); }
	void Write(PlotExtraYield       const& variable) { variable.write(*this); }
	void Write(TradeData            const& variable) { variable.write(*this); }

	// get the amount of bytes needed to save the variable in question
	// also tells the savegame that a conversion table is needed
	int GetXmlByteSize(JITarrayTypes eType);
	int GetXmlSize(JITarrayTypes eType);

	bool isLogWriting() const;


	void Log(const char* name) const;
	void Log(const char* name, const char* value) const;
	template<class T>
	void Log(const char* name, T eVariable) const;
	template<class T>
	void Log(SavegameVariableTypes eType, T eVariable) const;

	const char* getEnumName(SavegameVariableTypes eType) const;

private:
	const char* getLogFile() const;
	int m_iLogIntent;

	template<int TYPE2>
	struct WriteEnumMap
	{
		template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, VariableTypes TYPE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
		static void Write(CvSavegameWriter& kWriter, EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, TYPE, LENGTH_KNOWN_WHILE_COMPILING> const& em);

		template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, VariableTypes TYPE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
		static void Log(CvSavegameWriter& kWriter, SavegameVariableTypes eType, EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, TYPE, LENGTH_KNOWN_WHILE_COMPILING> const& em);
	};

	template<typename T>
	void WriteEnum(T variable);

	template<typename T>
	void WriteXmlEnum(T variable);
	void WriteXmlEnum(int iVariable, JITarrayTypes eType);
	void Write(byte*var, unsigned int iSize);

	SavegameClassTypes m_eClassType;

	CvSavegameWriterBase& m_writerbase;
	std::vector<byte>& m_vector;

#ifdef WITH_DEBUG_WRITE_SAVEGAME
public:
	void DEBUG_startA();
	void DEBUG_startB();
	void DEBUG_end();
	bool DEBUG_compare() const;
protected:
	int m_iDebugWriteMode;
	std::vector<byte> m_vectorA;
	std::vector<byte> m_vectorB;
#endif
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

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, VariableTypes TYPE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
inline void CvSavegameReader::Read(EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, TYPE, LENGTH_KNOWN_WHILE_COMPILING>& em)
{
	ReadEnumMap<TYPE>::Read(*this, em);
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
	for (typename std::vector<T>::iterator it = vec.begin(); it != vec.end(); ++it)
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
	for (typename std::vector<T*>::iterator it = vec.begin(); it != vec.end(); ++it)
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
	for (typename std::list<T>::iterator it = vec.begin(); it != vec.end(); ++it)
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
	for (typename std::list<T*>::iterator it = vec.begin(); it != vec.end(); ++it)
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
inline void CvSavegameWriter::Write(JustInTimeArray<T> const& jitArray)
{
	jitArray.Write(*this);
}

template<class T>
inline void CvSavegameWriter::Write(std::vector<T> const& vec)
{
	unsigned short iLength = vec.size();
	Write(iLength);
	for (typename std::vector<T>::const_iterator it = vec.begin(); it != vec.end(); ++it)
	{
		Write((T)*it);
	}
}

template<class T>
inline void CvSavegameWriter::Write(SavegameVariableTypes eType, std::vector<T> const& vec)
{
	if (vec.size() > 0)
	{
		Write(eType);
		Write(vec);
	}
}

template<class T>
inline void CvSavegameWriter::Write(std::vector<T*> const& vec)
{
	unsigned short iLength = vec.size();
	Write(iLength);
	for (typename std::vector<T*>::const_iterator it = vec.begin(); it != vec.end(); ++it)
	{
		Write(**it);
	}
}

template<>
inline void CvSavegameWriter::Write(std::vector<CvUnit*> const& vec) {
	unsigned short iLength = vec.size();
	Write(iLength);
	for (std::vector<CvUnit*>::const_iterator it = vec.begin(); it != vec.end(); ++it) {
		Write(static_cast <CvUnitAI&>(**it));
	}
}

template<class T>
inline void CvSavegameWriter::Write(std::list<T> const& vec)
{
	unsigned short iLength = vec.size();
	Write(iLength);
	for (typename std::list<T>::const_iterator it = vec.begin(); it != vec.end(); ++it)
	{
		Write((T)*it);
	}
}

template<class T>
inline void CvSavegameWriter::Write(SavegameVariableTypes eType, std::list<T> const& vec)
{
	if (vec.size() > 0)
	{
		Write(eType);
		Write(vec);
	}
}

template<class T>
inline void CvSavegameWriter::Write(std::list<T*> const& vec)
{
	unsigned short iLength = vec.size();
	Write(iLength);
	for (typename std::list<T*>::const_iterator it = vec.begin(); it != vec.end(); ++it)
	{
		Write(**it);
	}
}

template<class T1, class T2>
void CvSavegameWriter::Write(std::pair<T1, T2> const& pair)
{
	Write(pair.first);
	Write(pair.second);
}

template<class T>
inline void CvSavegameWriter::Write(SavegameVariableTypes eType, FFreeListTrashArray<T> const& array)
{
	if (array.getCount() > 0)
	{
		Write(eType);
		WriteStreamableFFreeListTrashArray(array, *this);
	}
}

template<class T>
inline void CvSavegameWriter::Write(SavegameVariableTypes eType, CvIdVector<T> const& vec)
{
	if (vec.size() > 0)
	{
		Write(eType);
		vec.Write(this);
	}
}

template<class T>
inline void CvSavegameWriter::Write(SavegameVariableTypes eType, std::vector<T*> const& vec)
{
	if (vec.size() > 0)
	{
		Write(eType);
		Write(vec);
	}
}

template<class T>
inline void CvSavegameWriter::Write(SavegameVariableTypes eType, T eVariable, T eDefault)
{
	if (eVariable != eDefault)
	{
		Log(eType, eVariable);
		Write(eType);
		Write(eVariable);
	}
}

template<class T>
inline void CvSavegameWriter::Write(SavegameVariableTypes eType, JustInTimeArray<T> const& jitArray)
{
	if (jitArray.hasContent())
	{
		Write(eType);
		Write(jitArray);
	}
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, VariableTypes TYPE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
inline void CvSavegameWriter::Write(EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, TYPE, LENGTH_KNOWN_WHILE_COMPILING> const& em)
{
	WriteEnumMap<TYPE>::Write(*this, em);
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, VariableTypes TYPE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
inline void CvSavegameWriter::Write(SavegameVariableTypes eType, EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, TYPE, LENGTH_KNOWN_WHILE_COMPILING> const& em)
{
	if (em.hasContent())
	{
		WriteEnumMap<TYPE>::Log(*this, eType, em);
		Write(eType);
		Write(em);
	}
}

template<class T>
inline void CvSavegameWriter::Write(SavegameVariableTypes eType, CLinkList<T> const& lList)
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

template<class T>
void CvSavegameWriter::Log(SavegameVariableTypes eType, T eVariable) const
{
	Log(getEnumName(eType), getTypeStr(eVariable));
}

template<class T>
void CvSavegameWriter::Log(const char* name, T eVariable) const
{
	if (!isLogWriting())
	{
		return;
	}
	Log(name, getTypeStr(eVariable));
}

template<>
template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, VariableTypes TYPE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
inline void CvSavegameWriter::WriteEnumMap<VARIABLE_TYPE_CLASS>::Log(CvSavegameWriter& kWriter, SavegameVariableTypes eType, EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, TYPE, LENGTH_KNOWN_WHILE_COMPILING> const& em)
{
	if (!kWriter.isLogWriting())
	{
		return;
	}

	if (!em.hasContent())
	{
		return;
	}

	LogIntentHelper helper(kWriter, "EnumMap (2D)", kWriter.getEnumName(eType));

	for (IndexType loopVar = em.FIRST; loopVar <= em.LAST; ++loopVar)
	{
		if (em[loopVar].hasContent())
		{
			kWriter.Log(getTypeStr(loopVar), 1);
			// TODO: loop inner EnumMap
		}
	}
}

template<int TYPE2>
template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, VariableTypes TYPE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
inline void CvSavegameWriter::WriteEnumMap<TYPE2>::Log(CvSavegameWriter& kWriter, SavegameVariableTypes eType, EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, TYPE, LENGTH_KNOWN_WHILE_COMPILING> const& em)
{
	if (!kWriter.isLogWriting())
	{
		return;
	}

	if (!em.hasContent())
	{
		return;
	}

	LogIntentHelper helper(kWriter, "EnumMap", kWriter.getEnumName(eType));

	for (IndexType loopVar = em.FIRST; loopVar <= em.LAST; ++loopVar)
	{
		if (em.get(loopVar) != DEFAULT)
		{
			kWriter.Log(getTypeStr(loopVar), getTypeStr(em.get(loopVar)));
		}
	}
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
	CvSavegameReaderBase(FDataStreamBase* pStream, unsigned int iSavegameVersion);

	~CvSavegameReaderBase();

	static unsigned int getSavegameVersion();

private:

	void init();

	void Read(byte* var, unsigned int iSize);
	int ReadChunk();

	FDataStreamBase* m_pStream;
	byte* m_Memory;
	byte* m_MemoryAllocation;
	byte *m_MemoryEnd;
	unsigned int m_iSize;
	unsigned int m_iRead;
	static unsigned int m_iSavegameVersion;

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

#include "CvSavegameEnumMap.h"

class LogIntentHelper
{
public:
	LogIntentHelper(CvSavegameWriter& kWriter, const char* title);
	LogIntentHelper(CvSavegameWriter& kWriter, const char* title, const char* name);
	~LogIntentHelper();

private:
	CvSavegameWriter& m_kWriter;
};

#endif
