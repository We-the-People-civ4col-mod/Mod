#ifndef SAVEGAME_H
#define SAVEGAME_H

class FDataStreamBase;
enum SavegameVariableTypes;

#include "JustInTimeArray.h"
#include "BoolArray.h"
#include "LinkedList.h"

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
	NUM_SAVEGAME_CLASS_TYPES,

	FIRST_SAVEGAME_CLASS_TYPES = 0,
};

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
	
	template<class T1, class T2, int T3, class T4, class T5, int T6, int T7>
	void Read(EnumMapBase<T1, T2, T3, T4, T5, T6, T7>& em);

	template<class T1, class T2, class T3, int T4>
	void Read(EnumMap2DDefault<T1, T2, T3, T4>& em);

	template<class T>
	void Read(PlayerArrayBase<T>& array);

	template<class T>
	void Read(JustInTimeArray<T>& jitArray);

	template<class T>
	void Read(CLinkList<T>& lList);

	void Read(BoolArray& baArray);
	void Read(PlayerBoolArrayBase& array);
	void Read(IDInfo& idInfo);

	// workaround because we can't use references on bitfields
	template<typename T>
	T ReadBitfield(T variable);

	// Add all enums used in savegames (single byte)
	void Read(AreaAITypes           & variable) { ReadEnum(variable); }
	void Read(CardinalDirectionTypes& variable) { ReadEnum(variable); }
	void Read(CalendarTypes         & variable) { ReadEnum(variable); }
	void Read(CityPlotTypes         & variable) { ReadEnum(variable); }
	void Read(CustomMapOptionTypes  & variable) { ReadEnum(variable); }
	void Read(DirectionTypes        & variable) { ReadEnum(variable); }
	void Read(GameType              & variable) { ReadEnum(variable); }
	void Read(OrderTypes            & variable) { ReadEnum(variable); }
	void Read(PlayerTypes           & variable) { ReadEnum(variable); }
	void Read(PlotTypes             & variable) { ReadEnum(variable); }
	void Read(SlotClaim             & variable) { ReadEnum(variable); }
	void Read(SlotStatus            & variable) { ReadEnum(variable); }
	void Read(TeamTypes             & variable) { ReadEnum(variable); }
	void Read(TurnTimerTypes        & variable) { ReadEnum(variable); }
	void Read(TradeableItems        & variable) { ReadEnum(variable); }
	void Read(UnitAIStates          & variable) { ReadEnum(variable); }
	void Read(UnitTravelStates      & variable) { ReadEnum(variable); }


	// everything linked to xml file enums
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

	int ConvertIndex(JITarrayTypes eType, int iIndex) const;
	int GetXmlSize(JITarrayTypes eType) const;

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

	void Write(SavegameVariableTypes eType);
	void Write(SavegameVariableTypes eType, const CvString  & szString);
	void Write(SavegameVariableTypes eType, const CvWString & szString);
	void Write(SavegameVariableTypes eType, const char      * szString);
	void Write(SavegameVariableTypes eType, const wchar     * szString);
	void Write(SavegameVariableTypes eType, BoolArray& baArray);
	void Write(SavegameVariableTypes eType, PlayerBoolArrayBase& array);
	void Write(SavegameVariableTypes eType, IDInfo& idInfo);

	template<class T>
	void Write(SavegameVariableTypes eType, PlayerArrayBase<T>& array);

	template<class T>
	void Write(SavegameVariableTypes eType, T eVariable, T eDefault);

	template<class T>
	void Write(SavegameVariableTypes eType, JustInTimeArray<T>& jitArray);

	template<class T1, class T2, int T3, class T4, class T5, int T6, int T7>
	void Write(EnumMapBase<T1, T2, T3, T4, T5, T6, T7>& em);

	template<class T1, class T2, int T3, class T4, class T5, int T6, int T7>
	void Write(SavegameVariableTypes eType, EnumMapBase<T1, T2, T3, T4, T5, T6, T7>& em);

	template<class T1, class T2, class T3, int T4>
	void Write(EnumMap2DDefault<T1, T2, T3, T4>& em);

	template<class T1, class T2, class T3, int T4>
	void Write(SavegameVariableTypes eType, EnumMap2DDefault<T1, T2, T3, T4>& em);

	template<class T>
	void Write(SavegameVariableTypes eType, CLinkList<T>& lList);

	// Add all enums used in savegames (single byte)
	void Write(AreaAITypes            variable) { WriteEnum(variable); }
	void Write(CardinalDirectionTypes variable) { WriteEnum(variable); }
	void Write(CalendarTypes          variable) { WriteEnum(variable); }
	void Write(CustomMapOptionTypes   variable) { WriteEnum(variable); }
	void Write(DirectionTypes         variable) { WriteEnum(variable); }
	void Write(GameType               variable) { WriteEnum(variable); }
	void Write(OrderTypes             variable) { WriteEnum(variable); }
	void Write(PlayerTypes            variable) { WriteEnum(variable); }
	void Write(PlotTypes              variable) { WriteEnum(variable); }
	void Write(SlotClaim              variable) { WriteEnum(variable); }
	void Write(SlotStatus             variable) { WriteEnum(variable); }
	void Write(TeamTypes              variable) { WriteEnum(variable); }
	void Write(TurnTimerTypes         variable) { WriteEnum(variable); }
	void Write(UnitAIStates           variable) { WriteEnum(variable); }
	void Write(UnitTravelStates       variable) { WriteEnum(variable); }

	// everything linked to xml file enums
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

template<class T1, class T2, int T3, class T4, class T5, int T6, int T7>
inline void CvSavegameReader::Read(EnumMapBase<T1, T2, T3, T4, T5, T6, T7>& em)
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
inline void CvSavegameReader::Read(CLinkList<T>& lList)
{
	lList.Read(*this);
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
inline void CvSavegameWriter::Write(SavegameVariableTypes eType, PlayerArrayBase<T>& array)
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

template<class T1, class T2, int T3, class T4, class T5, int T6, int T7>
inline void CvSavegameWriter::Write(EnumMapBase<T1, T2, T3, T4, T5, T6, T7>& em)
{
	em.Write(*this);
}

template<class T1, class T2, int T3, class T4, class T5, int T6, int T7>
inline void CvSavegameWriter::Write(SavegameVariableTypes eType, EnumMapBase<T1, T2, T3, T4, T5, T6, T7>& em)
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
inline void CvSavegameWriter::Write(SavegameVariableTypes eType, CLinkList<T>& lList)
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
	FAssert(variable >= (char)MIN_CHAR && variable <= MAX_CHAR);
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

	FDataStreamBase* m_pStream;
	byte* m_Memory;
	byte* m_MemoryAllocation;
	byte *m_MemoryEnd;
	unsigned int m_iSize;
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
