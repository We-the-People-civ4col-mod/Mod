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

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T>
class EnumMapBase
{
public:
	EnumMapBase();
	~EnumMapBase();

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

	// used to allow structs.cpp to save using EnumMap savegame code.
	// We can't change the memory layout defined in Structs.h, hence we are struck with vectors.
	// This workaround allows gaining the savegame benefits of xml index conversion and reduced filesize.
	void copyToVector(std::vector<T>& thisVector) const;
	void copyFromVector(const std::vector<T>& thisVector);

	void Read(CvSavegameReader& reader);
	void Write(CvSavegameWriter& writer) const;

	// operator overload
	EnumMapBase& operator=(const EnumMapBase &rhs);
	
	template<class T2, int DEFAULT2, int SIZE2, int SIZE_OF_T2>
	EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>& operator  = (const EnumMapBase<LengthType, T2, DEFAULT2, T_SUBSET, SIZE2, SIZE_OF_T2> &rhs);
	template<class T2, int DEFAULT2, int SIZE2, int SIZE_OF_T2>
	EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>& operator += (const EnumMapBase<LengthType, T2, DEFAULT2, T_SUBSET, SIZE2, SIZE_OF_T2> &rhs);
	template<class T2, int DEFAULT2, int SIZE2, int SIZE_OF_T2>
	EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>& operator -= (const EnumMapBase<LengthType, T2, DEFAULT2, T_SUBSET, SIZE2, SIZE_OF_T2> &rhs);
	template<class T2, int DEFAULT2, int SIZE2, int SIZE_OF_T2>
	bool operator == (const EnumMapBase<LengthType, T2, DEFAULT2, T_SUBSET, SIZE2, SIZE_OF_T2> &rhs) const;
	template<class T2, int DEFAULT2, int SIZE2, int SIZE_OF_T2>
	bool operator != (const EnumMapBase<LengthType, T2, DEFAULT2, T_SUBSET, SIZE2, SIZE_OF_T2> &rhs) const;
	

private:

	union
	{
		T     * m_pArrayFull;
		short * m_pArrayShort;
		char  * m_pArrayChar;
	};

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


template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T>
inline EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::EnumMapBase()
	: m_pArrayFull(NULL)
{
	FAssertMsg(sizeof(*this) == 4, "EnumMap is supposed to only contain a pointer");
	FAssertMsg(getLength() >= 0 && getLength() <= ArrayLength((LengthType)0), "Custom length out of range");
	FAssertMsg(First() >= 0 && First() <= getLength(), "Custom length out of range");
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T>
inline EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::~EnumMapBase()
{
	reset();
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T>
__forceinline T EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::getDefault() const
{
	return (T)DEFAULT;
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T>
__forceinline LengthType EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::First() const
{
	return ArrayStart((T_SUBSET)0);
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T>
__forceinline LengthType EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::getLength() const
{
	return ArrayLength((T_SUBSET)0);
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T>
__forceinline LengthType EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::numElements() const
{
	// apparently subtracting two LengthTypes results in int, not LengthType
	return (LengthType)(getLength() - First());
}


template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T>
inline T EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::get(LengthType eIndex) const
{
	FAssert(eIndex >= First() && eIndex < getLength());

	switch (SIZE)
	{
	case 1:  return (T)(m_pArrayChar  ? m_pArrayChar [eIndex - First()] : DEFAULT);
	case 2:  return (T)(m_pArrayShort ? m_pArrayShort[eIndex - First()] : DEFAULT);
	default: return (T)(m_pArrayFull  ? m_pArrayFull [eIndex - First()] : DEFAULT);
	}
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T>
inline void EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::set(LengthType eIndex, T eValue)
{
	FAssert(eIndex >= First() && eIndex < getLength());
	if (m_pArrayFull == NULL)
	{
		if (eValue == DEFAULT) 
		{
			return;
		}
		allocate();
	}

	switch (SIZE)
	{
	case 1:  m_pArrayChar [eIndex - First()] = (char )eValue; break;
	case 2:  m_pArrayShort[eIndex - First()] = (short)eValue; break;
	default: m_pArrayFull [eIndex - First()] =        eValue; break;
	}
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T>
inline void EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::add(LengthType eIndex, T eValue)
{
	FAssert(eIndex >= First() && eIndex < getLength());
	if (eValue != 0)
	{
		set(eIndex, eValue + get(eIndex));
	}
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T>
inline void EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::safeSet(LengthType eIndex, T eValue)
{
	if (eIndex >= First() && eIndex < getLength())
	{
		set(eIndex, eValue);
	}
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T>
inline void EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::safeAdd(LengthType eIndex, T eValue)
{
	if (eIndex >= First() && eIndex < getLength())
	{
		add(eIndex, eValue);
	}
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T>
inline void EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::addAll(T eValue)
{
	if (eValue != 0)
	{
		for (LengthType eIndex = First(); eIndex < getLength(); ++eIndex)
		{
			add(eIndex, (T)eValue);
		}
	}
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T>
inline bool EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::isAllocated() const
{
	return m_pArrayFull != NULL;
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T>
inline bool EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::hasContent() const
{
	if (m_pArrayFull != NULL)
	{
		for (LengthType eIndex = (LengthType)0; eIndex < numElements(); ++eIndex)
		{
			if (get(eIndex) != DEFAULT)
			{
				return true;
			}
		}
		// now we cheat and alter data despite being const.
		// We just detected all data to be of the default value, meaning the array is not used.
		// Release the data to save memory. It won't change how the outside world view the EnumMap.
		(const_cast <EnumMapBase*> (this))->reset();
	}
	return false;
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T>
inline T EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::getMin() const
{
	if (m_pArray == NULL)
	{
		return DEFAULT;
	}
	return (T)(*std::min_element(m_pArray, m_pArray + numElements()));
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T>
inline T EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::getMax() const
{
	if (m_pArray == NULL)
	{
		return DEFAULT;
	}
	return (T)(*std::max_element(m_pArray, m_pArray + numElements()));
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T>
inline void EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::keepMin(LengthType eIndex, T eValue)
{
	FAssert(eIndex >= First() && eIndex < getLength());
	if (get(eIndex) > eValue)
	{
		set(eIndex, eValue);
	}
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T>
inline void EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::keepMax(LengthType eIndex, T eValue)
{
	FAssert(eIndex >= First() && eIndex < getLength());
	if (get(eIndex) < eValue)
	{
		set(eIndex, eValue);
	}
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T>
inline void EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::reset()
{
	// doesn't matter which one we free. They all point to the same memory address, which is what matters here.
	SAFE_DELETE_ARRAY(m_pArrayFull);
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T>
inline void EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::setAll(T eValue)
{
	if (m_pArrayChar == NULL)
	{
		if (eValue == DEFAULT)
		{
			return;
		}
		m_pArrayChar = new char[numElements() * SIZE_OF_T];
	}

	if (SIZE_OF_T == 1 || eValue == 0)
	{
		memset(m_pArrayFull, eValue, numElements() * SIZE_OF_T);
	}
	else if (SIZE == 2)
	{
		std::fill_n(m_pArrayShort, numElements(), eValue);
	}
	else
	{
		std::fill_n(m_pArrayFull, numElements(), eValue);
	}
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T>
inline void EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::allocate()
{
	FAssert(m_pArrayChar == NULL);

	m_pArrayChar = new char[numElements() * SIZE_OF_T];

	// memset is a whole lot faster. However it only works on bytes.
	// Optimize for the default case where default is 0.
	if (SIZE_OF_T == 1 || DEFAULT == 0)
	{
		memset(m_pArrayFull, DEFAULT, numElements() * SIZE_OF_T);
	}
	else if (SIZE == 2)
	{
		std::fill_n(m_pArrayShort, numElements(), (T)DEFAULT);
	}
	else
	{
		std::fill_n(m_pArrayFull, numElements(), (T)DEFAULT);
	}
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T>
inline void EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::copyToVector(std::vector<T>& thisVector) const
{
	thisVector.reserve(getLength());
	thisVector.resize(getLength(), (T)DEFAULT);

	if (isAllocated())
	{
		if (SIZE == 0)
		{
			memcpy(&thisVector[0], m_pArrayFull, getLength() * sizeof(T));
		}
		else
		{
			for (LengthType eIndex = First(); eIndex < getLength(); ++eIndex)
			{
				thisVector[eIndex] = get(eIndex);
			}
		}
	}
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T>
inline void EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::copyFromVector(const std::vector<T>& thisVector)
{
	FAssert((unsigned int)getLength() == thisVector.size());

	if (!isAllocated())
	{
		allocate();
	}

	if (SIZE == 0)
	{
		memcpy(m_pArrayFull, &thisVector[0], getLength() * sizeof(T));
	}
	else
	{
		for (LengthType eIndex = First(); eIndex < getLength(); ++eIndex)
		{
			set(eIndex, thisVector[eIndex]);
		}
	}
}

//
// Savegame code
//

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T>
inline void EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::Read(CvSavegameReader& reader)
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

	const int iLength = ArrayType((LengthType)0) == NO_JIT_ARRAY_TYPE ? ArrayLength((LengthType)0) : reader.GetXmlSize(ArrayType((LengthType)0));
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
			if (iLength > 256)
			{
				unsigned short iReadBuffer;
				reader.Read(iReadBuffer);
				iLast = (LengthType)iReadBuffer;
			}
			else
			{
				byte iReadBuffer;
				reader.Read(iReadBuffer);
				iLast = (LengthType)iReadBuffer;
			}
			
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


template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T>
inline void EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::Write(CvSavegameWriter& writer) const
{
	const int iLength = ArrayType((LengthType)0) == NO_JIT_ARRAY_TYPE ? ArrayLength((LengthType)0) : writer.GetXmlSize(ArrayType((LengthType)0));
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
			if (iLength > 256)
			{
				writer.Write((unsigned short)iLast);
			}
			else
			{
				writer.Write((byte)iLast);
			}
			
			for (int i = iFirst; i <= iLast; ++i)
			{
				writer.Write(get((LengthType)i));
			}
		}
	}
}

//
// operator overloads
//

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T>
inline EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>& EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::operator=(const EnumMapBase &rhs)
{
	if (rhs.isAllocated())
	{
		if (m_pArrayFull == NULL) allocate();
		memcpy(m_pArrayFull, rhs.m_pArrayFull, numElements() * SIZE_OF_T);
	}
	else
	{
		reset();
	}

	return *this;
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T> template<class T2, int DEFAULT2, int SIZE2, int SIZE_OF_T2>
inline EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>& EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::operator=(const EnumMapBase<LengthType, T2, DEFAULT2, T_SUBSET, SIZE2, SIZE_OF_T2> &rhs)
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

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T> template<class T2, int DEFAULT2, int SIZE2, int SIZE_OF_T2>
inline EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>& EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::operator+=(const EnumMapBase<LengthType, T2, DEFAULT2, T_SUBSET, SIZE2, SIZE_OF_T2> &rhs)
{
	if (rhs.isAllocated())
	{
		for (LengthType eIndex = First(); eIndex < getLength(); ++eIndex)
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

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T> template<class T2, int DEFAULT2, int SIZE2, int SIZE_OF_T2>
inline EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>& EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::operator-=(const EnumMapBase<LengthType, T2, DEFAULT2, T_SUBSET, SIZE2, SIZE_OF_T2> &rhs)
{
	if (rhs.isAllocated())
	{
		for (LengthType eIndex = First(); eIndex < getLength(); ++eIndex)
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

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T> template<class T2, int DEFAULT2, int SIZE2, int SIZE_OF_T2>
inline bool EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::operator==(const EnumMapBase<LengthType, T2, DEFAULT2, T_SUBSET, SIZE2, SIZE_OF_T2> &rhs) const
{
	if (!rhs.isAllocated() && !isAllocated())
	{
		return DEFAULT == DEFAULT2;
	}

	if (SIZE == SIZE2 && SIZE_OF_T == SIZE_OF_T2 && rhs.isAllocated() && isAllocated())
	{
		return memcmp(m_pArrayChar, rhs.m_pArrayChar, getLength() * SIZE_OF_T) == 0;
	}

	for (LengthType eIndex = First(); eIndex < getLength(); ++eIndex)
	{
		if (get(eIndex) != rhs.get(eIndex))
		{
			return false;
		}
	}
	return true;
}

template<class LengthType, class T, int DEFAULT, class T_SUBSET, int SIZE, int SIZE_OF_T> template<class T2, int DEFAULT2, int SIZE2, int SIZE_OF_T2>
inline bool EnumMapBase<LengthType, T, DEFAULT, T_SUBSET, SIZE, SIZE_OF_T>::operator!=(const EnumMapBase<LengthType, T2, DEFAULT2, T_SUBSET, SIZE2, SIZE_OF_T2> &rhs) const
{
	if (!rhs.isAllocated() && !isAllocated())
	{
		return DEFAULT != DEFAULT2;
	}

	if (SIZE == SIZE2 && SIZE_OF_T == SIZE_OF_T2 && rhs.isAllocated() && isAllocated())
	{
		return memcmp(m_pArrayChar, rhs.m_pArrayChar, getLength() * SIZE_OF_T) != 0;
	}

	for (LengthType eIndex = First(); eIndex < getLength(); ++eIndex)
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
template <> struct EnumMapGetDefault<X> \
{ \
	enum { value = 0, SIZE = 0, SIZE_OF_T = sizeof(X) }; \
};

SET_ARRAY_DEFAULT(int);
SET_ARRAY_DEFAULT(short);
SET_ARRAY_DEFAULT(char);
SET_ARRAY_DEFAULT(unsigned int);
SET_ARRAY_DEFAULT(unsigned short);
SET_ARRAY_DEFAULT(byte);

#define SET_ARRAY_XML_ENUM( VAR, NUM_TYPES, JIT_TYPE, VAR_SIZE ) \
__forceinline VAR ArrayStart(VAR var) { return (VAR)0; } \
__forceinline VAR ArrayLength(VAR var) { return NUM_TYPES; } \
__forceinline JITarrayTypes ArrayType(VAR var) { return JIT_TYPE; } \
template <> struct EnumMapGetDefault<VAR> \
{ \
	enum { value = -1, SIZE = VAR_SIZE, SIZE_OF_T = SIZE }; \
};

// Byte size is set in enums
// If the length isn't known at compile time, 2 is assumed.
// Setting the byte size means say PlayerTypes will use 1 byte instead of 4. Works because MAX_PLAYERS <= 0x7F

//                 type                , length                   , JIT_ARRAY type            , byte size
SET_ARRAY_XML_ENUM(ArtStyleTypes       , NUM_ARTSTYLE_TYPES       , JIT_ARRAY_ART_STYLE       , BYTESIZE_ARTSTYLE_TYPES       );
SET_ARRAY_XML_ENUM(BonusTypes          , NUM_BONUS_TYPES          , JIT_ARRAY_BONUS           , BYTESIZE_BONUS_TYPES          );
SET_ARRAY_XML_ENUM(BuildTypes          , NUM_BUILD_TYPES          , JIT_ARRAY_BUILD           , BYTESIZE_BUILD_TYPES          );
SET_ARRAY_XML_ENUM(BuildingTypes       , NUM_BUILDING_TYPES       , JIT_ARRAY_BUILDING        , BYTESIZE_BUILDING_TYPES       );
SET_ARRAY_XML_ENUM(BuildingClassTypes  , NUM_BUILDINGCLASS_TYPES  , JIT_ARRAY_BUILDING_CLASS  , BYTESIZE_BUILDINGCLASS_TYPES  );
SET_ARRAY_XML_ENUM(SpecialBuildingTypes, NUM_SPECIALBUILDING_TYPES, JIT_ARRAY_BUILDING_SPECIAL, BYTESIZE_SPECIALBUILDING_TYPES);
SET_ARRAY_XML_ENUM(CivEffectTypes      , NUM_CIV_EFFECT_TYPES     , JIT_ARRAY_CIV_EFFECT      , BYTESIZE_CIV_EFFECT_TYPES     );
SET_ARRAY_XML_ENUM(CivicTypes          , NUM_CIVIC_TYPES          , JIT_ARRAY_CIVIC           , BYTESIZE_CIVIC_TYPES          );
SET_ARRAY_XML_ENUM(CivicOptionTypes    , NUM_CIVICOPTION_TYPES    , JIT_ARRAY_CIVIC_OPTION    , BYTESIZE_CIVICOPTION_TYPES    );
SET_ARRAY_XML_ENUM(CivilizationTypes   , NUM_CIVILIZATION_TYPES   , JIT_ARRAY_CIVILIZATION    , BYTESIZE_CIVILIZATION_TYPES   );
SET_ARRAY_XML_ENUM(ClimateTypes        , NUM_CLIMATE_TYPES        , JIT_ARRAY_CLIMATE         , BYTESIZE_CLIMATE_TYPES        );
SET_ARRAY_XML_ENUM(ColorTypes          , NUM_COLOR_TYPES          , JIT_ARRAY_COLOR           , BYTESIZE_COLOR_TYPES          );
SET_ARRAY_XML_ENUM(CultureLevelTypes   , NUM_CULTURELEVEL_TYPES   , JIT_ARRAY_CULTURE         , BYTESIZE_CULTURELEVEL_TYPES   );
SET_ARRAY_XML_ENUM(DiplomacyTypes      , NUM_DIPLOMACY_TYPES      , JIT_ARRAY_DIPLO           , BYTESIZE_DIPLOMACY_TYPES      );
SET_ARRAY_XML_ENUM(EraTypes            , NUM_ERA_TYPES            , JIT_ARRAY_ERA             , BYTESIZE_ERA_TYPES            );
SET_ARRAY_XML_ENUM(EmphasizeTypes      , NUM_EMPHASIZE_TYPES      , JIT_ARRAY_EMPHASIZE       , BYTESIZE_EMPHASIZE_TYPES      );
SET_ARRAY_XML_ENUM(EuropeTypes         , NUM_EUROPE_TYPES         , JIT_ARRAY_EUROPE          , BYTESIZE_EUROPE_TYPES         );
SET_ARRAY_XML_ENUM(EventTypes          , NUM_EVENT_TYPES          , JIT_ARRAY_EVENT           , BYTESIZE_EVENT_TYPES          );
SET_ARRAY_XML_ENUM(EventTriggerTypes   , NUM_EVENTTRIGGER_TYPES   , JIT_ARRAY_EVENT_TRIGGER   , BYTESIZE_EVENTTRIGGER_TYPES   );
SET_ARRAY_XML_ENUM(FatherTypes         , NUM_FATHER_TYPES         , JIT_ARRAY_FATHER          , BYTESIZE_FATHER_TYPES         );
SET_ARRAY_XML_ENUM(FatherPointTypes    , NUM_FATHER_POINT_TYPES   , JIT_ARRAY_FATHER_POINT    , BYTESIZE_FATHER_POINT_TYPES   );
SET_ARRAY_XML_ENUM(FeatureTypes        , NUM_FEATURE_TYPES        , JIT_ARRAY_FEATURE         , BYTESIZE_FEATURE_TYPES        );
SET_ARRAY_XML_ENUM(GameOptionTypes     , NUM_GAMEOPTION_TYPES     , JIT_ARRAY_GAME_OPTION     , BYTESIZE_GAMEOPTION_TYPES     );
SET_ARRAY_XML_ENUM(GameSpeedTypes      , NUM_GAMESPEED_TYPES      , JIT_ARRAY_GAME_SPEED      , BYTESIZE_GAMESPEED_TYPES      );
SET_ARRAY_XML_ENUM(GoodyTypes          , NUM_GOODY_TYPES          , JIT_ARRAY_GOODY           , BYTESIZE_GOODY_TYPES          );
SET_ARRAY_XML_ENUM(HandicapTypes       , NUM_HANDICAP_TYPES       , JIT_ARRAY_HANDICAP        , BYTESIZE_HANDICAP_TYPES       );
SET_ARRAY_XML_ENUM(HurryTypes          , NUM_HURRY_TYPES          , JIT_ARRAY_HURRY           , BYTESIZE_HURRY_TYPES          );
SET_ARRAY_XML_ENUM(ImprovementTypes    , NUM_IMPROVEMENT_TYPES    , JIT_ARRAY_IMPROVEMENT     , BYTESIZE_IMPROVEMENT_TYPES    );
SET_ARRAY_XML_ENUM(InvisibleTypes      , NUM_INVISIBLE_TYPES      , JIT_ARRAY_INVISIBLE       , BYTESIZE_INVISIBLE_TYPES      );
SET_ARRAY_XML_ENUM(LeaderHeadTypes     , NUM_LEADER_TYPES         , JIT_ARRAY_LEADER_HEAD     , BYTESIZE_LEADER_TYPES         );
SET_ARRAY_XML_ENUM(MemoryTypes         , NUM_MEMORY_TYPES         , JIT_ARRAY_MEMORY          , BYTESIZE_MEMORY_TYPES         );
SET_ARRAY_XML_ENUM(PlayerColorTypes    , NUM_PLAYERCOLOR_TYPES    , JIT_ARRAY_PLAYER_COLOR    , BYTESIZE_PLAYERCOLOR_TYPES    );
SET_ARRAY_XML_ENUM(PlayerOptionTypes   , NUM_PLAYEROPTION_TYPES   , JIT_ARRAY_PLAYER_OPTION   , BYTESIZE_PLAYEROPTION_TYPES   );
SET_ARRAY_XML_ENUM(ProfessionTypes     , NUM_PROFESSION_TYPES     , JIT_ARRAY_PROFESSION      , BYTESIZE_PROFESSION_TYPES     );
SET_ARRAY_XML_ENUM(PromotionTypes      , NUM_PROMOTION_TYPES      , JIT_ARRAY_PROMOTION       , BYTESIZE_PROMOTION_TYPES      );
SET_ARRAY_XML_ENUM(RouteTypes          , NUM_ROUTE_TYPES          , JIT_ARRAY_ROUTE           , BYTESIZE_ROUTE_TYPES          );
SET_ARRAY_XML_ENUM(SeaLevelTypes       , NUM_SEALEVEL_TYPES       , JIT_ARRAY_SEA_LEVEL       , BYTESIZE_SEALEVEL_TYPES       );
SET_ARRAY_XML_ENUM(TerrainTypes        , NUM_TERRAIN_TYPES        , JIT_ARRAY_TERRAIN         , BYTESIZE_TERRAIN_TYPES        );
SET_ARRAY_XML_ENUM(TraitTypes          , NUM_TRAIT_TYPES          , JIT_ARRAY_TRAIT           , BYTESIZE_TRAIT_TYPES          );
SET_ARRAY_XML_ENUM(UnitTypes           , NUM_UNIT_TYPES           , JIT_ARRAY_UNIT            , BYTESIZE_UNIT_TYPES           );
SET_ARRAY_XML_ENUM(UnitAITypes         , NUM_UNITAI_TYPES         , JIT_ARRAY_UNIT_AI         , BYTESIZE_UNITAI_TYPES         );
SET_ARRAY_XML_ENUM(UnitClassTypes      , NUM_UNITCLASS_TYPES      , JIT_ARRAY_UNIT_CLASS      , BYTESIZE_UNITCLASS_TYPES      );
SET_ARRAY_XML_ENUM(UnitCombatTypes     , NUM_UNITCOMBAT_TYPES     , JIT_ARRAY_UNIT_COMBAT     , BYTESIZE_UNITCOMBAT_TYPES     );
SET_ARRAY_XML_ENUM(SpecialUnitTypes    , NUM_SPECIALUNIT_TYPES    , JIT_ARRAY_UNIT_SPECIAL    , BYTESIZE_SPECIALUNIT_TYPES    );
SET_ARRAY_XML_ENUM(VictoryTypes        , NUM_VICTORY_TYPES        , JIT_ARRAY_VICTORY         , BYTESIZE_VICTORY_TYPES        );
SET_ARRAY_XML_ENUM(WorldSizeTypes      , NUM_WORLDSIZE_TYPES      , JIT_ARRAY_WORLD_SIZE      , BYTESIZE_WORLDSIZE_TYPES      );
SET_ARRAY_XML_ENUM(YieldTypes          , NUM_YIELD_TYPES          , JIT_ARRAY_YIELD           , BYTESIZE_YIELD_TYPES          );


SET_ARRAY_XML_ENUM(AreaAITypes         , NUM_AREAAI_TYPES         , NO_JIT_ARRAY_TYPE         , BYTESIZE_AREAAI_TYPES         );
SET_ARRAY_XML_ENUM(PlayerTypes         , NUM_PLAYER_TYPES         , NO_JIT_ARRAY_TYPE         , BYTESIZE_PLAYER_TYPES         );
SET_ARRAY_XML_ENUM(TeamTypes           , NUM_TEAM_TYPES           , NO_JIT_ARRAY_TYPE         , BYTESIZE_TEAM_TYPES           );


//
// List of various types of EnumMaps
// In most cases it's not nice code to include all parameters from EnumMapBase.
// Adding other classes, which always sets the default makes it easier to add EnumMaps as arguments to functions etc.
//

template<class LengthType, class T, int DEFAULT>
class EnumMapDefault : public EnumMapBase <LengthType, T, DEFAULT, LengthType, EnumMapGetDefault<T>::SIZE, EnumMapGetDefault<T>::SIZE_OF_T > {};

template<class LengthType, class T>
class EnumMap : public EnumMapBase <LengthType, T, EnumMapGetDefault<T>::value, LengthType, EnumMapGetDefault<T>::SIZE, EnumMapGetDefault<T>::SIZE_OF_T > {};

#endif
