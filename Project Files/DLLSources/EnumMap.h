#ifndef ENUM_MAP_H
#define ENUM_MAP_H
#pragma once


//
// EnumMap is a special case of map where there is a <key,value> pair for each key in an enum.
// https://docs.oracle.com/en/java/javase/13/docs/api/java.base/java/util/EnumMap.html
//
// Put in civ4 terms, it's an array with a fixed length and automated memory management.
// The length is either the length of an xml file (like UnitTypes) or a fixed value (like MAX_PLAYERS)
//
// The array is set up with template parameters as this allows "arguments" to the default constructor.
// Often an array of arrays can only use the default constructor for the inner array, making flexible default
// constructors important.
//
// In most cases only the first two parameters need to be set. The last two only benefit from non-default values
// in very special cases.
//
// Template parameters:
//    LengthType: the type, which is used for index and array size.
//       For instance PlayerTypes means get will require a PlayerTypes argument and the array size is MAX_PLAYERS.
//
//    T: the data stored in the array. This is usually of the int family, but it can also be enums.
//
//    DEFAULT: (optional) sets the value the array will init to
//      If not mentioned, the value will be set by which type T is. 0 for int family and -1 (NO_) for enums
//      Note: using default and calling setAll() is preferred. See below.
//
//    T_SUBSET: (optional) Sets the range of the array if a subset is needed.
//      If you have an LengthType with a length of 100, but you only need 70-75, setting a nondefault here,
//      can make the array contain 6 items, which is then accessed with indexes 70-75
//      Default value will use LengthType, meaning it goes from 0 to 
//
//
// Try to keep the different types of parameters to a minimum. Each time a new set is used, a new set is compiled.
// Example:
//   EnumMap<PlayerTypes, int> A;
//   EnumMap<PlayerTypes, int, 1> B;
// This will create two different functions, compile and include both into the DLL file.
//   EnumMap<PlayerTypes, int> A;
//   EnumMap<PlayerTypes, int> B;
//   B.setAll(1);
// Same result, but since they now share the same parameters, the compiler will only make one set, which they will both call.
// It's not a major issue to make multiple, partly because most calls are inline anyway, but it should be mentioned.

template<class LengthType, class T, int DEFAULT = EnumMapGetDefault<T>::value , class T_SUBSET = LengthType >
class EnumMap
{
public:
	EnumMap();
	~EnumMap();

	// const values (per class)
	T getDefault() const;
	LengthType First() const;
	LengthType getLength() const;
	LengthType numElements() const;

	// array access
	T get(LengthType eIndex) const;
	void set(LengthType eIndex, T eValue);
	void add(LengthType eIndex, T eValue);

	// add bound checks. Ignore call if out of bound index
	void safeSet(LengthType eIndex, T eValue);
	void safeAdd(LengthType eIndex, T eValue);

	// add a number to all indexes
	void addAll(T eValue);
	
	// Check if there is non-default contents.
	// isAllocated() test for a null pointer while hasContent() will loop the array to test each index for default value.
	// Useful to avoid looping all 0 arrays and when creating savegames.
	// Note: hasContent() can release memory if it doesn't alter what get() will return.
	bool isAllocated() const;
	bool hasContent() const;
	
	T getMin() const;
	T getMax() const;

	void keepMin(LengthType eIndex, T eValue);
	void keepMax(LengthType eIndex, T eValue);
	
	// memory allocation and freeing
	void reset();
	void setAll(T eValue);
private:
	void allocate();

public:

	void Read(CvSavegameReader& reader);
	void Write(CvSavegameWriter& writer) const;

	// operator overload
	EnumMap& operator=(const EnumMap &rhs);
	
	template<class T2, int DEFAULT2>
	EnumMap<LengthType, T, DEFAULT, T_SUBSET>& operator  = (const EnumMap<LengthType, T2, DEFAULT2, T_SUBSET> &rhs);
	template<class T2, int DEFAULT2>
	EnumMap<LengthType, T, DEFAULT, T_SUBSET>& operator += (const EnumMap<LengthType, T2, DEFAULT2, T_SUBSET> &rhs);
	template<class T2, int DEFAULT2>
	EnumMap<LengthType, T, DEFAULT, T_SUBSET>& operator -= (const EnumMap<LengthType, T2, DEFAULT2, T_SUBSET> &rhs);
	template<class T2, int DEFAULT2>
	bool operator == (const EnumMap<LengthType, T2, DEFAULT2, T_SUBSET> &rhs) const;
	template<class T2, int DEFAULT2>
	bool operator != (const EnumMap<LengthType, T2, DEFAULT2, T_SUBSET> &rhs) const;
	

private:

	T* m_pArray;

	enum
	{
		SAVE_ARRAY_MULTI_BYTE,
		SAVE_ARRAY_LAST_TOKEN,
		SAVE_ARRAY_INDEX_OFFSET,
		SAVE_ARRAY_EMPTY_BYTE = 0xFF,
		SAVE_ARRAY_EMPTY_SHORT = 0xFFFF,
	};

	class interval
	{
	public:
		LengthType first;
		LengthType last;

		interval()
		{
			first = (LengthType)0;
			last = (LengthType)0;
		}
	};
};

//
// Functions
//
// They are all inline, though that doesn't mean they will get inlined in the resulting dll file as it's only a recommendation.
// The keyword inline serves two purposes. One is inlining small functions in the caller code,
// while the other is telling the linker that a function can be present in multiple object files.
// If the linker detects two identical inlined functions, it will merge them into one in the resulting dll file,
// like the code had been written in a cpp file and only present in one object file.
//
// The result is that the template functions are all compiled while we don't have to consider if they are compiled more than once.
// Maybe they will get inlined, but more likely some of them (particularly savegame code) are too big and will not be inlined.
//
// To actually force the compiler to inline, the keyword __forceinline can be used, but this one should really be used with care.
// Actually inlining functions can slow down the code and inline is usually only good for very small functions, like get variable.
//


template<class LengthType, class T, int DEFAULT, class T_SUBSET>
inline EnumMap<LengthType, T, DEFAULT, T_SUBSET>::EnumMap()
	: m_pArray(NULL)
{
	FAssertMsg(sizeof(*this) == 4, "EnumMap is supposed to only contain a pointer");
	FAssertMsg(getLength() >= 0 && getLength() <= ArrayLength((LengthType)0), "Custom length out of range");
	FAssertMsg(First() >= 0 && First() <= getLength(), "Custom length out of range");
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET>
inline EnumMap<LengthType, T, DEFAULT, T_SUBSET>::~EnumMap()
{
	reset();
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET>
__forceinline T EnumMap<LengthType, T, DEFAULT, T_SUBSET>::getDefault() const
{
	return (T)DEFAULT;
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET>
__forceinline LengthType EnumMap<LengthType, T, DEFAULT, T_SUBSET>::First() const
{
	return ArrayStart((T_SUBSET)0);
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET>
__forceinline LengthType EnumMap<LengthType, T, DEFAULT, T_SUBSET>::getLength() const
{
	return ArrayLength((T_SUBSET)0);
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET>
__forceinline LengthType EnumMap<LengthType, T, DEFAULT, T_SUBSET>::numElements() const
{
	// apparently subtracting two LengthTypes results in int, not LengthType
	return (LengthType)(getLength() - First());
}


template<class LengthType, class T, int DEFAULT, class T_SUBSET>
inline T EnumMap<LengthType, T, DEFAULT, T_SUBSET>::get(LengthType eIndex) const
{
	FAssert(eIndex >= First() && eIndex < getLength());
	return m_pArray ? m_pArray[eIndex - First()] : DEFAULT;
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET>
inline void EnumMap<LengthType, T, DEFAULT, T_SUBSET>::set(LengthType eIndex, T eValue)
{
	FAssert(eIndex >= First() && eIndex < getLength());
	if (m_pArray == NULL)
	{
		if (eValue != DEFAULT)
		allocate();
	}
	m_pArray[eIndex - First()] = eValue;
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET>
inline void EnumMap<LengthType, T, DEFAULT, T_SUBSET>::add(LengthType eIndex, T eValue)
{
	FAssert(eIndex >= First() && eIndex < getLength());
	if (eValue != 0)
	{
		set(eIndex, eValue + get(eIndex));
	}
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET>
inline void EnumMap<LengthType, T, DEFAULT, T_SUBSET>::safeSet(LengthType eIndex, T eValue)
{
	if (eIndex >= First() && eIndex < getLength())
	{
		set(eIndex, eValue);
	}
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET>
inline void EnumMap<LengthType, T, DEFAULT, T_SUBSET>::safeAdd(LengthType eIndex, T eValue)
{
	if (eIndex >= First() && eIndex < getLength())
	{
		add(eIndex, eValue);
	}
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET>
inline void EnumMap<LengthType, T, DEFAULT, T_SUBSET>::addAll(T eValue)
{
	if (eValue != 0)
	{
		if (m_pArray == NULL) allocate();
		for (T eIndex = First(); eIndex < getLength(); ++eIndex)
		{
			m_pArray[eIndex - First()] += eValue;
		}
	}
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET>
inline bool EnumMap<LengthType, T, DEFAULT, T_SUBSET>::isAllocated() const
{
	return m_pArray != NULL;
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET>
inline bool EnumMap<LengthType, T, DEFAULT, T_SUBSET>::hasContent() const
{
	if (m_pArray != NULL)
	{
		for (T eIndex = (T)0; eIndex < numElements(); ++eIndex)
		{
			if (m_pArray[eIndex] != DEFAULT)
			{
				return true;
			}
		}
		// now we cheat and alter data despite being const.
		// We just detected all data to be of the default value, meaning the array is not used.
		// Release the data to save memory. It won't change how the outside world view the EnumMap.
		(const_cast <EnumMap*> (this))->reset();
	}
	return false;
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET>
inline T EnumMap<LengthType, T, DEFAULT, T_SUBSET>::getMin() const
{
	if (m_pArray == NULL)
	{
		return DEFAULT;
	}
	return *std::min_element(m_pArray, m_pArray + numElements());
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET>
inline T EnumMap<LengthType, T, DEFAULT, T_SUBSET>::getMax() const
{
	if (m_pArray == NULL)
	{
		return DEFAULT;
	}
	return *std::max_element(m_pArray, m_pArray + numElements());
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET>
inline void EnumMap<LengthType, T, DEFAULT, T_SUBSET>::keepMin(LengthType eIndex, T eValue)
{
	FAssert(eIndex >= First() && eIndex < getLength());
	if (get(eIndex) > eValue)
	{
		set(eIndex, eValue);
	}
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET>
inline void EnumMap<LengthType, T, DEFAULT, T_SUBSET>::keepMax(LengthType eIndex, T eValue)
{
	FAssert(eIndex >= First() && eIndex < getLength());
	if (get(eIndex) < eValue)
	{
		set(eIndex, eValue);
	}
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET>
inline void EnumMap<LengthType, T, DEFAULT, T_SUBSET>::reset()
{
	SAFE_DELETE_ARRAY(m_pArray);
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET>
inline void EnumMap<LengthType, T, DEFAULT, T_SUBSET>::setAll(T eValue)
{
	if (m_pArray == NULL)
	{
		if (eValue == getDefault())
		{
			return;
		}
		m_pArray = new T[numElements()];
	}

	std::fill_n(m_pArray, numElements(), eValue);
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET>
inline void EnumMap<LengthType, T, DEFAULT, T_SUBSET>::allocate()
{
	FAssert(m_pArray == NULL);
	m_pArray = new T[numElements()];

	// memset is a whole lot faster. However it only works on bytes.
	// Optimize for the default case where default is 0.
	// We miss constexpr for this, but the compiler might figure it out anyway due to __forceinline.
	if (sizeof(T) == 1 || DEFAULT == 0)
	{
		memset(m_pArray, DEFAULT, numElements() * sizeof(T));
	}
	else
	{
		std::fill_n(m_pArray, numElements(), DEFAULT);
	}
}



template<class LengthType, class T, int DEFAULT, class T_SUBSET>
inline void EnumMap<LengthType, T, DEFAULT, T_SUBSET>::Read(CvSavegameReader& reader)
{
	// savegame format is as follows:
	// first index
	// last index
	// loop writing contents from first to last, both inclusive.
	// An array can be split into multiple subarrays, called tokens.
	// Any data not saved will be set to the default value on load.
	// This will greatly reduce savegame size if just a few entries have non-default data.
	//
	// To reduce overhead, the size of the indexes are shortened to be as short as possible, but still contain the max value.
	// There are two bools in each token. One states it's the last token and the other says first == last.
	// Those two are saved together with the first index, meaning only 14 bits are available for saved array length.
	// If the length of the array can fit in 6 bits, then first index is saved in just a single byte.
	// CvSavegameReader takes care of saving the last index, meaning it's a single byte if the length is max 250.
	// Last index is not saved if the bool for first == last is set.
	// As a result, the overhead for each token can be 1-4 bytes.

	const int iLength = ArrayType((LengthType)0) == NUM_JITarrayTypes ? ArrayLength((LengthType)0) : reader.GetXmlSize(ArrayType((LengthType)0));
	const bool bUseTwoByteStart = iLength > 64;

	int iBuffer;

	if (bUseTwoByteStart)
	{
		unsigned short iReadBuffer;
		reader.Read(iReadBuffer);
		iBuffer = iReadBuffer;
		if (iBuffer == SAVE_ARRAY_EMPTY_BYTE)
		{
			// empty array
			return;
		}
	}
	else
	{
		byte iReadBuffer;
		reader.Read(iReadBuffer);
		iBuffer = iReadBuffer;
		if (iBuffer == SAVE_ARRAY_EMPTY_SHORT)
		{
			// empty array
			return;
		}
	}

	while (true)
	{
		LengthType iFirst = (LengthType)(iBuffer >> SAVE_ARRAY_INDEX_OFFSET);
		bool bMultiByte = HasBit(iBuffer, SAVE_ARRAY_MULTI_BYTE);
		bool bLast = HasBit(iBuffer, SAVE_ARRAY_LAST_TOKEN);

		if (bMultiByte)
		{
			LengthType iLast;
			reader.Read(iLast);
			for (LengthType i = iFirst; i <= iLast; ++i)
			{
				// use a buffer because then it doesn't matter if T, array size and save size differs.
				T tBuffer;
				reader.Read(tBuffer);
				LengthType iIndex = (LengthType)reader.ConvertIndex(ArrayType((LengthType)0), i);
				set(iIndex, tBuffer);
			}
		}
		else
		{
			// single variable token
			T tBuffer;
			reader.Read(tBuffer);
			LengthType iIndex = (LengthType)reader.ConvertIndex(ArrayType((LengthType)0), iFirst);
			set(iIndex, tBuffer);
		}

		if (bLast)
		{
			return;
		}
		if (bUseTwoByteStart)
		{
			unsigned short iReadBuffer;
			reader.Read(iReadBuffer);
			iBuffer = iReadBuffer;
		}
		else
		{
			byte iReadBuffer;
			reader.Read(iReadBuffer);
			iBuffer = iReadBuffer;
		}
	}
}


template<class LengthType, class T, int DEFAULT, class T_SUBSET>
inline void EnumMap<LengthType, T, DEFAULT, T_SUBSET>::Write(CvSavegameWriter& writer) const
{
	const int iLength = ArrayType((LengthType)0) == NUM_JITarrayTypes ? ArrayLength((LengthType)0) : writer.GetXmlSize(ArrayType((LengthType)0));
	const bool bUseTwoByteStart = iLength > 64;

	// first support writing an empty array
	// this is likely never needed, but support it for completeness and stability
	if (!hasContent())
	{
		if (bUseTwoByteStart)
		{
			unsigned short iBuffer = SAVE_ARRAY_EMPTY_SHORT;
			writer.Write(iBuffer);
		}
		else
		{
			byte iBuffer = SAVE_ARRAY_EMPTY_BYTE;
			writer.Write(iBuffer);
		}
		return;
	}

	std::vector<interval> tokens(0);
	interval* pInterval = NULL;

	// generate tokens
	// pInterval points to the token currently being created
	// this means it both acts as a pointer to where to store iLast, but also storing the state
	for (LengthType eIndex = First(); eIndex < getLength(); ++eIndex)
	{
		if (get(eIndex) != getDefault())
		{
			if (pInterval == NULL)
			{
				interval eTemp;
				tokens.push_back(eTemp);
				pInterval = &tokens[tokens.size() - 1];
				pInterval->first = eIndex;
			}
			pInterval->last = eIndex;
		}
		else
		{
			pInterval = NULL;
		}
	}

	int iEnd = tokens.size() - 1;

	// save the tokens
	for (int i = 0; i <= iEnd; ++i)
	{
		LengthType iFirst = tokens[i].first;
		LengthType iLast = tokens[i].last;

		int iBuffer = iFirst << SAVE_ARRAY_INDEX_OFFSET;

		if (i == iEnd)
		{
			SetBit(iBuffer, SAVE_ARRAY_LAST_TOKEN);
		}

		if (iFirst == iLast)
		{
			if (bUseTwoByteStart)
			{
				writer.Write((unsigned short)iBuffer);
			}
			else
			{
				writer.Write((byte)iBuffer);
			}
			writer.Write(get(iFirst));
		}
		else
		{
			SetBit(iBuffer, SAVE_ARRAY_MULTI_BYTE);
			if (bUseTwoByteStart)
			{
				writer.Write((unsigned short)iBuffer);
			}
			else
			{
				writer.Write((byte)iBuffer);
			}
			writer.Write(iLast);
			for (int i = iFirst; i <= iLast; ++i)
			{
				writer.Write(get((LengthType)i));
			}
		}
	}
}



template<class LengthType, class T, int DEFAULT, class T_SUBSET>
inline EnumMap<LengthType, T, DEFAULT, T_SUBSET>& EnumMap<LengthType, T, DEFAULT, T_SUBSET>::operator=(const EnumMap &rhs)
{
	if (rhs.isAllocated())
	{
		if (m_pArray == NULL) allocate();
		memcpy(m_pArray, rhs.m_pArray, numElements() * sizeof(T));
	}
	else
	{
		reset();
	}

	return *this;
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET> template<class T2, int DEFAULT2>
inline EnumMap<LengthType, T, DEFAULT, T_SUBSET>& EnumMap<LengthType, T, DEFAULT, T_SUBSET>::operator=(const EnumMap<LengthType, T2, DEFAULT2, T_SUBSET> &rhs)
{
	if (rhs.isAllocated())
	{
		for (LengthType eIndex = First(); eIndex < getLength(); ++eIndex)
		{
			set(eIndex, rhs.get(eIndex));
		}
	}
	else
	{
		assignAll(DEFAULT2);
	}

	return *this;
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET> template<class T2, int DEFAULT2>
inline EnumMap<LengthType, T, DEFAULT, T_SUBSET>& EnumMap<LengthType, T, DEFAULT, T_SUBSET>::operator+=(const EnumMap<LengthType, T2, DEFAULT2, T_SUBSET> &rhs)
{
	if (rhs.isAllocated())
	{
		for (T eIndex = First(); eIndex < getLength(); ++eIndex)
		{
			add(eIndex, rhs.get(eIndex));
		}
	}
	else if (DEFAULT2 != 0)
	{
		addAll(DEFAULT2);
	}

	return *this;
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET> template<class T2, int DEFAULT2>
inline EnumMap<LengthType, T, DEFAULT, T_SUBSET>& EnumMap<LengthType, T, DEFAULT, T_SUBSET>::operator-=(const EnumMap<LengthType, T2, DEFAULT2, T_SUBSET> &rhs)
{
	if (rhs.isAllocated())
	{
		for (T eIndex = First(); eIndex < getLength(); ++eIndex)
		{
			add(eIndex, -rhs.get(eIndex));
		}
	}
	else if (DEFAULT2 != 0)
	{
		addAll(-DEFAULT2);
	}

	return *this;
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET> template<class T2, int DEFAULT2>
inline bool EnumMap<LengthType, T, DEFAULT, T_SUBSET>::operator==(const EnumMap<LengthType, T2, DEFAULT2, T_SUBSET> &rhs) const
{
	if (!rhs.isAllocated() && !isAllocated())
	{
		return DEFAULT == DEFAULT2;
	}

	if (sizeof(T) == sizeof(T2) && rhs.isAllocated() && isAllocated())
	{
		return memcmp(m_pArray, rhs.m_pArray, getLength() * sizeof(T)) == 0;
	}

	for (T eIndex = First(); eIndex < getLength(); ++eIndex)
	{
		if (get(eIndex) != rhs.get(eIndex))
		{
			return false;
		}
	}
	return true;
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET> template<class T2, int DEFAULT2>
inline bool EnumMap<LengthType, T, DEFAULT, T_SUBSET>::operator!=(const EnumMap<LengthType, T2, DEFAULT2, T_SUBSET> &rhs) const
{
	if (!rhs.isAllocated() && !isAllocated())
	{
		return DEFAULT != DEFAULT2;
	}

	if (sizeof(T) == sizeof(T2) && rhs.isAllocated() && isAllocated())
	{
		return memcmp(m_pArray, rhs.m_pArray, getLength() * sizeof(T)) != 0;
	}

	for (T eIndex = First(); eIndex < getLength(); ++eIndex)
	{
		if (get(eIndex) == rhs.get(eIndex))
		{
			return false;
		}
	}
	return true;
}

//
//
// type setup
// most of this aims at being set up at compile time
//
//

template <class T>
struct EnumMapGetDefault
{
};

#define SET_ARRAY_DEFAULT( X ) \
__forceinline X ArrayDefault( X var) { return 0; } \
template <> struct EnumMapGetDefault<X> {enum { value = 0 }; };

SET_ARRAY_DEFAULT(int);
SET_ARRAY_DEFAULT(short);
SET_ARRAY_DEFAULT(char);
SET_ARRAY_DEFAULT(unsigned int);
SET_ARRAY_DEFAULT(unsigned short);
SET_ARRAY_DEFAULT(byte);

#define SET_ARRAY_XML_ENUM( VAR, NUM_TYPES, JIT_TYPE ) \
template <> struct EnumMapGetDefault<VAR> {enum { value = -1 }; }; \
__forceinline VAR ArrayStart(VAR var) { return (VAR)0; } \
__forceinline VAR ArrayLength(VAR var) { return NUM_TYPES; } \
__forceinline JITarrayTypes ArrayType(VAR var) { return JIT_TYPE; }


SET_ARRAY_XML_ENUM(ArtStyleTypes       , (ArtStyleTypes)GC.getNumArtStyleTypes(), JIT_ARRAY_ART_STYLE);
SET_ARRAY_XML_ENUM(BonusTypes          , NUM_BONUS_TYPES          , JIT_ARRAY_BONUS           );
SET_ARRAY_XML_ENUM(BuildTypes          , NUM_BUILD_TYPES          , JIT_ARRAY_BUILD           );
SET_ARRAY_XML_ENUM(BuildingTypes       , NUM_BUILDING_TYPES       , JIT_ARRAY_BUILDING        );
SET_ARRAY_XML_ENUM(BuildingClassTypes  , NUM_BUILDINGCLASS_TYPES  , JIT_ARRAY_BUILDING_CLASS  );
SET_ARRAY_XML_ENUM(SpecialBuildingTypes, NUM_SPECIALBUILDING_TYPES, JIT_ARRAY_BUILDING_SPECIAL);
SET_ARRAY_XML_ENUM(CivEffectTypes      , NUM_CIV_EFFECT_TYPES     , JIT_ARRAY_CIV_EFFECT      );
SET_ARRAY_XML_ENUM(CivicTypes          , NUM_CIVIC_TYPES          , JIT_ARRAY_CIVIC           );
SET_ARRAY_XML_ENUM(CivicOptionTypes    , NUM_CIVICOPTION_TYPES    , JIT_ARRAY_CIVIC_OPTION    );
SET_ARRAY_XML_ENUM(CivilizationTypes   , NUM_CIVILIZATION_TYPES   , JIT_ARRAY_CIVILIZATION    );
SET_ARRAY_XML_ENUM(ClimateTypes        , NUM_CLIMATE_TYPES        , JIT_ARRAY_CLIMATE         );
SET_ARRAY_XML_ENUM(ColorTypes          , (ColorTypes)GC.getNumColorInfos(), JIT_ARRAY_COLOR   );
SET_ARRAY_XML_ENUM(CultureLevelTypes   , NUM_CULTURELEVEL_TYPES   , JIT_ARRAY_CULTURE         );
SET_ARRAY_XML_ENUM(DiplomacyTypes      , NUM_DIPLOMACY_TYPES      , JIT_ARRAY_DIPLO           );
SET_ARRAY_XML_ENUM(EraTypes            , NUM_ERA_TYPES            , JIT_ARRAY_ERA             );
SET_ARRAY_XML_ENUM(EmphasizeTypes      , NUM_EMPHASIZE_TYPES      , JIT_ARRAY_EMPHASIZE       );
SET_ARRAY_XML_ENUM(EuropeTypes         , NUM_EUROPE_TYPES         , JIT_ARRAY_EUROPE          );
SET_ARRAY_XML_ENUM(EventTypes          , NUM_EVENT_TYPES          , JIT_ARRAY_EVENT           );
SET_ARRAY_XML_ENUM(EventTriggerTypes   , NUM_EVENTTRIGGER_TYPES   , JIT_ARRAY_EVENT_TRIGGER   );
SET_ARRAY_XML_ENUM(FatherTypes         , NUM_FATHER_TYPES         , JIT_ARRAY_FATHER          );
SET_ARRAY_XML_ENUM(FatherPointTypes    , NUM_FATHER_POINT_TYPES   , JIT_ARRAY_FATHER_POINT    );
SET_ARRAY_XML_ENUM(FeatureTypes        , NUM_FEATURE_TYPES        , JIT_ARRAY_FEATURE         );
SET_ARRAY_XML_ENUM(GameOptionTypes     , NUM_GAMEOPTION_TYPES     , JIT_ARRAY_GAME_OPTION     );
SET_ARRAY_XML_ENUM(GameSpeedTypes      , NUM_GAMESPEED_TYPES      , JIT_ARRAY_GAME_SPEED      );
SET_ARRAY_XML_ENUM(GoodyTypes          , NUM_GOODY_TYPES          , JIT_ARRAY_GOODY           );
SET_ARRAY_XML_ENUM(HandicapTypes       , NUM_HANDICAP_TYPES       , JIT_ARRAY_HANDICAP        );
SET_ARRAY_XML_ENUM(HurryTypes          , NUM_HURRY_TYPES          , JIT_ARRAY_HURRY           );
SET_ARRAY_XML_ENUM(ImprovementTypes    , NUM_IMPROVEMENT_TYPES    , JIT_ARRAY_IMPROVEMENT     );
SET_ARRAY_XML_ENUM(LeaderHeadTypes     , NUM_LEADER_TYPES         , JIT_ARRAY_LEADER_HEAD     );
SET_ARRAY_XML_ENUM(MemoryTypes         , NUM_MEMORY_TYPES         , JIT_ARRAY_MEMORY          );
SET_ARRAY_XML_ENUM(PlayerColorTypes    , (PlayerColorTypes)GC.getNumPlayerColorInfos(), JIT_ARRAY_PLAYER_COLOR);
SET_ARRAY_XML_ENUM(PlayerOptionTypes   , NUM_PLAYEROPTION_TYPES   , JIT_ARRAY_PLAYER_OPTION   );
SET_ARRAY_XML_ENUM(ProfessionTypes     , NUM_PROFESSION_TYPES     , JIT_ARRAY_PROFESSION      );
SET_ARRAY_XML_ENUM(PromotionTypes      , NUM_PROMOTION_TYPES      , JIT_ARRAY_PROMOTION       );
SET_ARRAY_XML_ENUM(RouteTypes          , NUM_ROUTE_TYPES          , JIT_ARRAY_ROUTE           );
SET_ARRAY_XML_ENUM(SeaLevelTypes       , NUM_SEALEVEL_TYPES       , JIT_ARRAY_SEA_LEVEL       );
SET_ARRAY_XML_ENUM(TerrainTypes        , NUM_TERRAIN_TYPES        , JIT_ARRAY_TERRAIN         );
SET_ARRAY_XML_ENUM(TraitTypes          , NUM_TRAIT_TYPES          , JIT_ARRAY_TRAIT           );
SET_ARRAY_XML_ENUM(UnitTypes           , NUM_UNIT_TYPES           , JIT_ARRAY_UNIT            );
SET_ARRAY_XML_ENUM(UnitAITypes         , NUM_UNITAI_TYPES         , JIT_ARRAY_UNIT_AI         );
SET_ARRAY_XML_ENUM(UnitClassTypes      , NUM_UNITCLASS_TYPES      , JIT_ARRAY_UNIT_CLASS      );
SET_ARRAY_XML_ENUM(UnitCombatTypes     , NUM_UNITCOMBAT_TYPES     , JIT_ARRAY_UNIT_COMBAT     );
SET_ARRAY_XML_ENUM(SpecialUnitTypes    , (SpecialUnitTypes)GC.getNumSpecialUnitInfos(), JIT_ARRAY_UNIT_SPECIAL);
SET_ARRAY_XML_ENUM(VictoryTypes        , NUM_VICTORY_TYPES        , JIT_ARRAY_VICTORY         );
SET_ARRAY_XML_ENUM(WorldSizeTypes      , NUM_WORLDSIZE_TYPES      , JIT_ARRAY_WORLD_SIZE      );
SET_ARRAY_XML_ENUM(YieldTypes          , NUM_YIELD_TYPES          , JIT_ARRAY_YIELD           );



SET_ARRAY_XML_ENUM(PlayerTypes, NUM_PLAYER_TYPES, NO_JIT_ARRAY_TYPE);
SET_ARRAY_XML_ENUM(TeamTypes, NUM_TEAM_TYPES, NO_JIT_ARRAY_TYPE);


#endif
