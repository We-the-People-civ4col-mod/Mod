//
// InfoArray
//
// It's a relative of JustInTime arrays though it's aimed at storing xml data in info classes.
//
// It's designed to store static data as in write once and then it becomes read only. Writing is mainly directly from xml.
// 
// read(tagname) is all it takes to read from xml, even if the tag is a list of lists.
// Well developed error detection and asserting when reading xml data.
// If xml contains something like Unit instead of UnitClass, it will tell you which file, what's wrong and what it expects.
//
// Rather than storing an entire array, it stores only non-zero elements. This makes it ideal for near empty arrays in xml.
// Examples could be free promotions, which would most often be empty, but if in use would often contain 1-3 elements.
// In this case it would store an array of PromotionTypes to enable.
//
// While it saves memory, the biggest benefit is that it allows looping of non-zero values only.
// For instance if an array contains a single promotion, it can be looped with one iteration instead of the number of promotions.
//
// The underlying structure is an array of "blocks". Each block is an array of 1-4 shorts (16 bit signed int)
//    First short is "the index" meaning that would be the index if it were a normal array.
//
//    Example:
//       We want to store that we increase production of hammers by 25% and crosses by 20%
//       get(0)   would provide YIELD_HAMMERS
//       get(0,1) would provide 25 as it is the value connected with YIELD_HAMMERS
//       get(1)   would provide YIELD_CROSSES
//       get(1,1) would provide 20
//       get(2)   would provide -1 as it is out of bounds
//
//  Looping an InfoArray can be done in two ways.
//     1: regular using getLength()
//     2: just increasing the argument for get() until it returns -1 as this mean it ended up outside the array
//
//  The second option has the benefit of the reading code not caring for length while looping.
//
// Written to work together with BoolArray, JustInTimeArray, JustInTimeArray2D and CacheArray2D.
//   addCache in those (add in BoolArray) can be used in those to make a list of combined values of multiple InfoArrays
// 
//
//  The class is split into two classes:
//    InfoArray: data and read functions
//    InfoArrayMod: functions to modify the content
//
//  The idea with the split is that the functions are declared as InfoArrayMod in whatever class they belong to.
//  This class will then have access to all the functions and can assign data as needed.
//  To provide outside access to the data, it can have an access function, which returns a const InfoArray pointer.
//  Because it is InfoArray and not InfoArrayMod, it will only have read functions.
//  Private can do this too, but this way the write functions aren't even in the IDE proposed list of functions.
//

#pragma once

#ifndef INFO_ARRAY_H
#define INFO_ARRAY_H

#ifndef FASSERT_ENABLE
// remove the type argument if it isn't used by asserts anyway
// this will make the compiler ignore getType() calls and hence reduce code size/complexity
#define getWithType(a, b, c) getWithType(b, c)
#endif

class CvCivilizationInfo;

template<class IndexType, class T>
class EnumMap;

class InfoArrayBase
{
	// make the python interface a friend as it needs to bypass the type check
	friend class CyInfoArray;
	// declare InfoArrayMod to be friend
	// this allows any InfoArrayMod to access getInternal directly, bypassing the type checks
	// this gives more coding freedom, but getType should be compared in asserts if done so to preserve the strict type checks
	friend class InfoArrayMod;
	template<int, typename>
	friend class InfoArrayToken;

protected:
	InfoArrayBase() {};

public:
	InfoArrayBase(JITarrayTypes eType0, JITarrayTypes eType1, JITarrayTypes eType2, JITarrayTypes eType3);
	~InfoArrayBase();

	int getIndex(int iIndex) const; // index as in JIT array - much slower than get() or a real JIT array
	int getIndex(int iIndex, int iSubIndex) const;
	int getLength() const;
	int getDimentions() const;
	bool isBool() const; // dimentions and types are the same 

	int getMin(int iDimention) const;
	int getMax(int iDimention) const;
	bool areIndexesValid(int iMax, int iDimention = 0, int iMin = 0) const;

	InfoArrayBase& operator=(const InfoArrayBase &rhs);

	JITarrayTypes getType(int i) const
	{
		FAssert(i >= 0 && i < 4);
		return (JITarrayTypes)((m_aiTypes >> (8 * i)) & 0xFF);
	}

	int getWithType(JITarrayTypes eType, int iIndex, int iTokenIndex) const
	{
		FAssert(GetBaseType(eType) == getType(iTokenIndex) || (eType >= NUM_JITarrayTypes && getType(iTokenIndex) >= NUM_JITarrayTypes));
		return getInternal(iIndex, iTokenIndex);
	}

	// adds UnitClass->Unit and BuildingClass->Building conversion. Otherwise the same as without the civinfo argument 
	int getWithTypeWithConversion(JITarrayTypes eType, int iIndex, int iTokenIndex, const CvCivilizationInfo *pCivInfo) const;

#include "InfoArrayGet.h"

protected:
	// needed by python interface. Do not use this in the DLL as it lacks type checking
	int pyGet(int iIndex, int iSubIndex) const {return getInternal(iIndex, iSubIndex);}

	int getInternal(int iIndex, int iTokenIndex = 0) const;
	int _getIndexOf(int iValue, int iDim) const;

	short m_iLength;
	char m_iNumDimentions;
	short* m_pArray;
	union
	{
		struct
		{
			JITarrayTypes m_eType0 : 8;
			JITarrayTypes m_eType1 : 8;
			JITarrayTypes m_eType2 : 8;
			JITarrayTypes m_eType3 : 8;
		};
		unsigned int m_aiTypes;
	};
};

///
/// write functions
/// These functions will completely overwrite the existing content
///
class InfoArrayMod : public InfoArrayBase
{
public:
	InfoArrayMod() {};
	InfoArrayMod(JITarrayTypes eType0, JITarrayTypes eType1 = JIT_ARRAY_NO_TYPE, JITarrayTypes eType2 = JIT_ARRAY_NO_TYPE, JITarrayTypes eType3 = JIT_ARRAY_NO_TYPE) : InfoArrayBase(eType0, eType1, eType2, eType3){};
	// bReadFloat will multiply the read value by 100 and store the int value of that
	// It doesn't provide extra functionality other than allowing floats to be written in XML for human readability

	// szType is always getType()
	// sTagName is the root xml tax name of the structure to be read
	void read(CvXMLLoadUtility* pXML, const char* szType, const char *sTagName);
	void assign(const BoolArray* pBArray);
	void assign(const std::vector<int>& vec);
	void assign(const std::vector<bool>& vec);
	void convertClass(const InfoArrayBase* pArray, const CvCivilizationInfo* pCivInfo = NULL);
private:
	void readRecursive(CvXMLLoadUtility* pXML, int& iIndex, std::vector<short>& aArray, std::vector<short>& aIndex, int iLevel, const char *sTagName, const char* szType);

public:

	// writing assign JIT array code in the header to avoid template errors/warnings
	template<class T>
	void assign(const JustInTimeArray<T>* pJITArray)
	{
		FAssert(m_iNumDimentions == 2);
		SAFE_DELETE_ARRAY(m_pArray);
		m_iLength = 0;
		for (int i = 0; i < pJITArray->length(); i++)
		{
			if (pJITArray->get(i) != 0)
			{
				m_iLength++;
			}
		}

		if (m_iLength == 0)
		{
			return;
		}

		m_pArray = new short[m_iLength * 2];

		int iCounter = 0;

		for (int i = 0; i < pJITArray->length(); i++)
		{
			if (pJITArray->get(i) != 0)
			{
				m_pArray[iCounter] = i;
				iCounter++;
				m_pArray[iCounter] = pJITArray->get(i);
				iCounter++;
			}
		}
	}
};

template<int DIMENTION, typename T> class InfoArrayToken {};
template <typename type> struct JIT_TYPE {};




#define INFO_ARRAY_CONST(type, JITtype )                                           \
template <> struct JIT_TYPE<type>                                                  \
{                                                                                  \
	static const JITarrayTypes TYPE = JITtype;                                     \
};

#define INFO_ARRAY_GET_SHARED(type, getName, JITtype, returnType)                  \
INFO_ARRAY_CONST(type, JITtype)                                                    \
template<int DIMENTION> class InfoArrayToken<DIMENTION, type>                      \
 : virtual protected InfoArrayMod                                                  \
{                                                                                  \
BOOST_STATIC_ASSERT(DIMENTION >= 0 && DIMENTION < 4);                              \
protected:                                                                         \
	InfoArrayToken() {}                                                            \
public:                                                                            \
	returnType getName(int iIndex) const                                           \
	{                                                                              \
		return static_cast< returnType > (getInternal(iIndex, DIMENTION));         \
	}                                                                              \
	returnType getWithTemplate(int iIndex, returnType eVar) const                  \
	{                                                                              \
		return static_cast< returnType > (getInternal(iIndex, DIMENTION));         \
	}                                                                              \
	int getIndexOf(returnType eValue) const                                        \
	{                                                                              \
		return _getIndexOf(eValue, DIMENTION);                                     \
	}                                                                              \
};

#define INFO_ARRAY_GET(type, getName, JITtype)                                     \
	INFO_ARRAY_GET_SHARED(type, getName, JITtype, type)

#define INFO_ARRAY_GET_INT(type, getName, JITtype)                                 \
enum type {};                                                                      \
INFO_ARRAY_GET_SHARED(type, getName, JITtype, int)



enum JIT_NoneTypes {};
INFO_ARRAY_CONST(JIT_NoneTypes, JIT_ARRAY_NO_TYPE)
template<int DIMENTION> class InfoArrayToken<DIMENTION, JIT_NoneTypes>
{
protected:
	InfoArrayToken() {};
};

INFO_ARRAY_GET(AchieveTypes                 , getAchieve            , JIT_ARRAY_ACHIEVE            )
INFO_ARRAY_GET(ArtStyleTypes                , getArtStyle           , JIT_ARRAY_ART_STYLE          )
INFO_ARRAY_GET(BonusTypes                   , getBonus              , JIT_ARRAY_BONUS              )
INFO_ARRAY_GET(BuildTypes                   , getBuild              , JIT_ARRAY_BUILD              )
INFO_ARRAY_GET(BuildingTypes                , getBuilding           , JIT_ARRAY_BUILDING           )
INFO_ARRAY_GET(BuildingClassTypes           , getBuildingClass      , JIT_ARRAY_BUILDING_CLASS     )
INFO_ARRAY_GET(SpecialBuildingTypes         , getSpecialBuilding    , JIT_ARRAY_BUILDING_SPECIAL   )
INFO_ARRAY_GET(CivEffectTypes               , getCivEffect          , JIT_ARRAY_CIV_EFFECT         )
INFO_ARRAY_GET(CivicTypes                   , getCivic              , JIT_ARRAY_CIVIC              )
INFO_ARRAY_GET(CivicOptionTypes             , getCivicOption        , JIT_ARRAY_CIVIC_OPTION       )
INFO_ARRAY_GET(CivilizationTypes            , getCivilization       , JIT_ARRAY_CIVILIZATION       )
INFO_ARRAY_GET(ClimateTypes                 , getClimate            , JIT_ARRAY_CLIMATE            )
INFO_ARRAY_GET(ColorTypes                   , getColor              , JIT_ARRAY_COLOR              )
INFO_ARRAY_GET(CultureLevelTypes            , getCulture            , JIT_ARRAY_CULTURE            )
INFO_ARRAY_GET(DiplomacyTypes               , getDiplomacy          , JIT_ARRAY_DIPLO              )
INFO_ARRAY_GET(DomainTypes                  , getDomain             , JIT_ARRAY_DOMAIN             )
INFO_ARRAY_GET(EraTypes                     , getEra                , JIT_ARRAY_ERA                )
INFO_ARRAY_GET(EmphasizeTypes               , getEmphasize          , JIT_ARRAY_EMPHASIZE          )
INFO_ARRAY_GET(EuropeTypes                  , getEurope             , JIT_ARRAY_EUROPE             )
INFO_ARRAY_GET(EventTypes                   , getEvent              , JIT_ARRAY_EVENT              )
INFO_ARRAY_GET(EventTriggerTypes            , getEventTrigger       , JIT_ARRAY_EVENT_TRIGGER      )
INFO_ARRAY_GET(FatherTypes                  , getFather             , JIT_ARRAY_FATHER             )
INFO_ARRAY_GET(FatherPointTypes             , getFatherPoint        , JIT_ARRAY_FATHER_POINT       )
INFO_ARRAY_GET(FeatureTypes                 , getFeature            , JIT_ARRAY_FEATURE            )
INFO_ARRAY_GET(GameOptionTypes              , getGameOption         , JIT_ARRAY_GAME_OPTION        )
INFO_ARRAY_GET(GameSpeedTypes               , getGameSpeed          , JIT_ARRAY_GAME_SPEED         )
INFO_ARRAY_GET(GoodyTypes                   , getGoody              , JIT_ARRAY_GOODY              )
INFO_ARRAY_GET(HandicapTypes                , getHandicap           , JIT_ARRAY_HANDICAP           )
INFO_ARRAY_GET(HurryTypes                   , getHurry              , JIT_ARRAY_HURRY              )
INFO_ARRAY_GET(ImprovementTypes             , getImprovement        , JIT_ARRAY_IMPROVEMENT        )
INFO_ARRAY_GET(InvisibleTypes               , getInvisibiity        , JIT_ARRAY_INVISIBLE          )
INFO_ARRAY_GET(LeaderHeadTypes              , getLeaderHead         , JIT_ARRAY_LEADER_HEAD        )
INFO_ARRAY_GET(MemoryTypes                  , getMemory             , JIT_ARRAY_MEMORY             )
INFO_ARRAY_GET(PlayerColorTypes             , getPlayerColor        , JIT_ARRAY_PLAYER_COLOR       )
INFO_ARRAY_GET(PlayerOptionTypes            , getPlayerOption       , JIT_ARRAY_PLAYER_OPTION      )
INFO_ARRAY_GET(ProfessionTypes              , getProfession         , JIT_ARRAY_PROFESSION         )
INFO_ARRAY_GET(PromotionTypes               , getPromotion          , JIT_ARRAY_PROMOTION          )
INFO_ARRAY_GET(RouteTypes                   , getRoute              , JIT_ARRAY_ROUTE              )
INFO_ARRAY_GET(SeaLevelTypes                , getSeaLevel           , JIT_ARRAY_SEA_LEVEL          )
INFO_ARRAY_GET(TerrainTypes                 , getTerrain            , JIT_ARRAY_TERRAIN            )
INFO_ARRAY_GET(TraitTypes                   , getTrait              , JIT_ARRAY_TRAIT              )
INFO_ARRAY_GET(UnitTypes                    , getUnit               , JIT_ARRAY_UNIT               )
INFO_ARRAY_GET(UnitAITypes                  , getUnitAI             , JIT_ARRAY_UNIT_AI            )
INFO_ARRAY_GET(UnitClassTypes               , getUnitClass          , JIT_ARRAY_UNIT_CLASS         )
INFO_ARRAY_GET(UnitCombatTypes              , getUnitCombat         , JIT_ARRAY_UNIT_COMBAT        )
INFO_ARRAY_GET(SpecialUnitTypes             , getSpecialUnit        , JIT_ARRAY_UNIT_SPECIAL       )
INFO_ARRAY_GET(VictoryTypes                 , getVictory            , JIT_ARRAY_VICTORY            )
INFO_ARRAY_GET(WorldSizeTypes               , getWorldSize          , JIT_ARRAY_WORLD_SIZE         )
INFO_ARRAY_GET(YieldTypes                   , getYield              , JIT_ARRAY_YIELD              )

// enums not linked to xml
INFO_ARRAY_GET(CivCategoryTypes             , getCivCategory        , JIT_ARRAY_CIV_CATEGORY       )
INFO_ARRAY_GET(FeatTypes                    , getFeat               , JIT_ARRAY_FEAT               )
INFO_ARRAY_GET(StrategyTypes                , getStrategy           , JIT_ARRAY_STRATEGY           )
INFO_ARRAY_GET(PlayerTypes                  , getPlayer             , JIT_ARRAY_PLAYER             )
INFO_ARRAY_GET(PlotTypes                    , getPlotType           , JIT_ARRAY_PLOT_TYPE          )
INFO_ARRAY_GET(TeamTypes                    , getTeam               , JIT_ARRAY_TEAM               )

// int/float etc
INFO_ARRAY_GET_INT(ModifierTypes            , getModifier           , JIT_ARRAY_MODIFIER           )
INFO_ARRAY_GET_INT(ModifierFloatTypes       , getModifierFloat      , JIT_ARRAY_MODIFIER_FLOAT     )
INFO_ARRAY_GET_INT(AllowTypes               , getAllow              , JIT_ARRAY_ALLOW              )
INFO_ARRAY_GET_INT(IntTypes                 , getInt                , JIT_ARRAY_INT                )
INFO_ARRAY_GET_INT(UIntTypes                , getInt                , JIT_ARRAY_UNSIGNED_INT       )
INFO_ARRAY_GET_INT(FloatTypes               , getFloat              , JIT_ARRAY_FLOAT              )


//
// Uses multiple inheritage with a single parent
// Uses virtual parents to get this to work properly
// Details: https://www.geeksforgeeks.org/multiple-inheritance-in-c/
//

template<typename T0, typename T1 = JIT_NoneTypes, typename T2 = JIT_NoneTypes, typename T3 = JIT_NoneTypes>
class InfoArray : virtual protected InfoArrayMod
	, public InfoArrayToken<0, T0>
	, public InfoArrayToken<1, T1>
	, public InfoArrayToken<2, T2>
	, public InfoArrayToken<3, T3>
	, public boost::noncopyable
{
	// classes, which somehow bypasses the compile time type check
	// TODO remove as many as possible
	friend class CvCity;
	friend class CvGlobals;
	friend class CivEffectInfo;
	friend class CvPlayerCivEffect;
	friend class CvInfoBase;
public:
	InfoArray() : InfoArrayMod(JIT_TYPE<T0>::TYPE, JIT_TYPE<T1>::TYPE, JIT_TYPE<T2>::TYPE, JIT_TYPE<T3>::TYPE)
		, InfoArrayToken<0, T0>()
		, InfoArrayToken<1, T1>()
		, InfoArrayToken<2, T2>()
		, InfoArrayToken<3, T3>()
	{};

	int getLength() const;

	// interaction with EnumMap
	template<typename T>
	void assignFrom(const EnumMap<T0, T>& em);
	void assignFrom(const EnumMap<T0, bool>& em);

	template<typename T>
	void addTo(EnumMap<T0, T> & em, int iChange = 1) const;

	template<typename T>
	void copyTo(EnumMap<T0, T> & em) const;
};


template<typename T0, typename T1, typename T2, typename T3>
int InfoArray<T0, T1, T2, T3>::getLength() const
{
	return InfoArrayBase::getLength();
}

template<typename T0, typename T1, typename T2, typename T3>
template<typename T>
void InfoArray<T0, T1, T2, T3>::assignFrom(const EnumMap<T0, T> & em)
{
	const bool bTypeCheck = boost::is_same<T2, JIT_NoneTypes>::value && boost::is_same<T3, JIT_NoneTypes>::value;
	BOOST_STATIC_ASSERT(bTypeCheck);
	// buffer everything in a vector to avoid the need for templates in InfoArray.cpp
	std::vector<int> vec;
	em.copyToVector(vec);
	assign(vec);
}

template<typename T0, typename T1, typename T2, typename T3>
void InfoArray<T0, T1, T2, T3>::assignFrom(const EnumMap<T0, bool> & em)
{
	const bool bTypeCheck = boost::is_same<T1, JIT_NoneTypes>::value && boost::is_same<T2, JIT_NoneTypes>::value && boost::is_same<T3, JIT_NoneTypes>::value;
	BOOST_STATIC_ASSERT(bTypeCheck);
	// buffer everything in a vector to avoid the need for templates in InfoArray.cpp
	std::vector<bool> vec;
	vec.reserve(em.LENGTH);
	for (int i = 0; i < em.LENGTH; ++i)
	{
		vec.push_back(em.get((T0)i));
	}
	assign(vec);
}

template<typename T0, typename T1, typename T2, typename T3>
template<typename T>
void InfoArray<T0, T1, T2, T3>::addTo(EnumMap<T0, T> & em, int iChange) const
{
	const bool bTypeCheck = boost::is_same<T2, JIT_NoneTypes>::value && boost::is_same<T3, JIT_NoneTypes>::value;
	BOOST_STATIC_ASSERT(bTypeCheck);

	FAssert(iChange == 1 || iChange == -1);
	for (int i = 0; i < this->m_iLength; ++i)
	{
		int iValue = m_iNumDimentions == 2 ? getInternal(i, 1) : 1;
		iValue *= iChange;
		em.add((T0)getInternal(i, 0), iValue);
	}
}

template<typename T0, typename T1, typename T2, typename T3>
template<typename T>
void InfoArray<T0, T1, T2, T3>::copyTo(EnumMap<T0, T> & em) const
{
	em.setAll(0);
	addTo(em);
}

#endif