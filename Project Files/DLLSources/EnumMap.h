#ifndef ENUM_MAP_H
#define ENUM_MAP_H
#pragma once


//
// EnumMap is a special case of map where there is a <key,value> pair for each key in an enum.
// https://docs.oracle.com/en/java/javase/13/docs/api/java.base/java/util/EnumMap.html
//
// Put in civ4 terms, it's an array with a fixed length and automated memory management.
// Memory is allocated using a lazy approach and occationally freed as well if it doesn't matter outside the class.
// This means like say std::vector, it can be used without considering memory allocations at all and it can't leak.
//
// The array is set up with template parameters as this allows "arguments" to the default constructor.
// Often an array of arrays can only use the default constructor for the inner array, making flexible default
// constructors important.
//
// Strict with types in arguments and assert checks (both compile and runtime) in order to catch bugs.
// Highly optimized for performance (particularly with hardcoded xml lengths) and low memory usage.
//
// Example usage:
// EnumMap<YieldTypes, bool>
// EnumMap<BuildingTypes, int>
// EnumMap<TeamTypes, PlayerTypes>
//
// See the end of the file for (often unneeded) additionaly features, like disabling type checks and altering default values.
// Default is 0, except if the second parameter is an enum, in which case the default is -1 (like NO_PLAYER)

enum
{
	ENUMMAP_SIZE_NATIVE,
	ENUMMAP_SIZE_1_BYTE,
	ENUMMAP_SIZE_2_BYTES,
	ENUMMAP_SIZE_BOOL,

	// max bytes used by class instance
	// used for determining if an array should be inside the class instance instead of a pointer.
	// this saves memory I/O, but with the tradeoff of making the instance bigger, which is not always wanted.
	// This is particularly useful if the array is no bigger than a pointer, like a char array of length 3-4.
	//
	// Note: the compiler will use 4 byte alignment, meaning this works best if set to a multiple of 4
	ENUMMAP_MAX_BYTES = 4,

	// set how many bits can be inlined in the class itself
	// if a bool array has <= this number of bits, then instead of allocating memory elsewhere
	// the data will be placed in the class itself
	ENUMMAP_MAX_INLINE_BOOL = 64,

	// bitmasks to get the bits, which gives the indexes to store 8 or 32 bit.
	// modulo is slow at runtime, binary AND is fast. They give the same result in this case.
	// Maybe the compiler will optimize to a binary and, but explicitly writing it means we are certain it will optimize.
	// In fact this way it's "optimized" even in debug builds.
	ENUMMAP_BITMASK_8_BIT = 7,
	ENUMMAP_BITMASK_32_BIT = 0x1F,
};

template<class IndexType, class T, int DEFAULT, class T_SUBSET = IndexType, class LengthType = IndexType >
class EnumMapBase
{
public:
	EnumMapBase();
	~EnumMapBase();

	// const values (per class)
	T getDefault() const;
	IndexType First() const;
	IndexType getLength() const;
	IndexType numElements() const;

	// array access
	T get(IndexType eIndex) const;
	void set(IndexType eIndex, T eValue);
	void add(IndexType eIndex, T eValue);

	// add bound checks. Ignore call if out of bound index
	void safeSet(IndexType eIndex, T eValue);
	void safeAdd(IndexType eIndex, T eValue);

	// add a number to all indexes
	void addAll(T eValue);
	
	// get the sum of all elements
	int getTotal() const;

	// Check if there is non-default contents.
	// isAllocated() test for a null pointer while hasContent() will loop the array to test each index for default value.
	// Useful to avoid looping all 0 arrays and when creating savegames.
	// Note: hasContent() can release memory if it doesn't alter what get() will return.
	bool isAllocated() const;
	bool hasContent() const;
	
	T getMin() const;
	T getMax() const;

	void keepMin(IndexType eIndex, T eValue);
	void keepMax(IndexType eIndex, T eValue);
	
	// memory allocation and freeing
	void reset();
	void setAll(T eValue);


	// used to allow structs.cpp to save using EnumMap savegame code.
	// We can't change the memory layout defined in Structs.h, hence we are struck with vectors.
	// This workaround allows gaining the savegame benefits of xml index conversion and reduced filesize.
	void copyToVector(std::vector<T>& thisVector) const;
	void copyFromVector(const std::vector<T>& thisVector);

	void Read(CvSavegameReader& reader);
	void Write(CvSavegameWriter& writer) const;


	////
	//// End of functions
	//// There is no need to keep reading this class declaration unless you are interested in the internal implementation
	////

	// compile time constants
	static const int SIZE = EnumMapGetDefault<T>::SIZE;
	static const int SIZE_OF_T = EnumMapGetDefault<T>::SIZE_OF_T;
	static const int LENGTH = EnumMapGetDefault<LengthType>::LENGTH;

	static const bool bINLINE_NATIVE = SIZE == ENUMMAP_SIZE_NATIVE && (LENGTH * SIZE_OF_T) <= ENUMMAP_MAX_BYTES;
	static const int NUM_NATIVE_BLOCKS = bINLINE_NATIVE ? LENGTH : 1;

	static const bool bINLINE_1_BYTE = SIZE == ENUMMAP_SIZE_1_BYTE && (LENGTH * SIZE_OF_T) <= ENUMMAP_MAX_BYTES;
	static const int NUM_1_BYTE_BLOCKS = bINLINE_1_BYTE ? LENGTH : 1;

	static const bool bINLINE_2_BYTES = SIZE == ENUMMAP_SIZE_2_BYTES && (LENGTH * SIZE_OF_T) <= ENUMMAP_MAX_BYTES;
	static const int NUM_2_BYTE_BLOCKS = bINLINE_2_BYTES ? LENGTH : 1;

	static const bool bINLINE_BOOL = SIZE == ENUMMAP_SIZE_BOOL && LENGTH <= ENUMMAP_MAX_INLINE_BOOL;
	static const int NUM_BOOL_BLOCKS = bINLINE_BOOL ? (LENGTH + 31) / 32 : 1;
	static const unsigned int BOOL_BLOCK_DEFAULT = DEFAULT ? MAX_UNSIGNED_INT : 0;


	static const bool bINLINE = bINLINE_NATIVE || bINLINE_1_BYTE || bINLINE_2_BYTES || bINLINE_BOOL;

	// operator overload
	EnumMapBase& operator=(const EnumMapBase &rhs);
	
	template<class T2, int DEFAULT2>
	EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>& operator  = (const EnumMapBase<IndexType, T2, DEFAULT2, T_SUBSET, LengthType> &rhs);
	template<class T2, int DEFAULT2>
	EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>& operator += (const EnumMapBase<IndexType, T2, DEFAULT2, T_SUBSET, LengthType> &rhs);
	template<class T2, int DEFAULT2>
	EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>& operator -= (const EnumMapBase<IndexType, T2, DEFAULT2, T_SUBSET, LengthType> &rhs);
	template<class T2, int DEFAULT2>
	bool operator == (const EnumMapBase<IndexType, T2, DEFAULT2, T_SUBSET, LengthType> &rhs) const;
	template<class T2, int DEFAULT2>
	bool operator != (const EnumMapBase<IndexType, T2, DEFAULT2, T_SUBSET, LengthType> &rhs) const;
	

private:

	// the actual memory in this class
	// since it's a union, the goal is to keep all of them to 4 bytes
	// the exception is when bINLINE is set and is allowed to use more than 4 bytes
	// an example of this is <PlayerTypes, bool> has a bool for each player, hence 8 bytes
	union
	{
		T     * m_pArrayFull;
		short * m_pArrayShort;
		char  * m_pArrayChar;
		unsigned int * m_pArrayBool;
		
		T m_InlineNative[NUM_NATIVE_BLOCKS];
		char m_Inline_1_byte[NUM_1_BYTE_BLOCKS];
		short m_Inline_2_bytes[NUM_2_BYTE_BLOCKS];
		unsigned int m_InlineBoolArray[NUM_BOOL_BLOCKS];
	};

	// the code will technically still work if this fails, but it will waste memory
	BOOST_STATIC_ASSERT(sizeof(T) <= 4 || bINLINE_NATIVE);

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
		IndexType first;
		IndexType last;

		interval()
		{
			first = (IndexType)0;
			last = (IndexType)0;
		}
	};

	// bool helpers
	int getBoolArrayBlock(int iIndex) const
	{
		if (bINLINE_BOOL && NUM_BOOL_BLOCKS == 1)
		{
			// hardcode only using first block in arrays hardcoded to only use one block
			// with a bit of luck the compiler can optimize the array code away completely
			FAssert(iIndex < 32)
			return 0;
		}
		else
		{
			return iIndex / 32;
		}
	}

	int getBoolArrayIndexInBlock(int iIndex) const
	{
		return iIndex & ENUMMAP_BITMASK_32_BIT;
	}
	
	////
	////
	//// Specialized functions
	////
	//// The idea is that the "outside world" calls a non-template function,
	//// which then calls a template function for the same purpose.
	//// This allows the outside world to stay simple (get/set etc) while at the same time
	//// allow specialized functions for each.
	//// This makes it easier for the compiler to optimize and allows more code.
	//// The key difference is that before the compiler would compile for all cases and then hopefully
	//// discard unused cases. Now it only compiles the specialized case matching the class templates in question.
	//// This allows code, which calls overloaded functions, which doesn't support all cases of all template classes.
	////
	//// Written inside the class declaration due to a C++ limitation.
	//// According to the standard, partial specialized functions aren't allowed and
	//// specialized functions can't be used for unspecialized template classes.
	//// However written inside the class declaration, the compiler treats the class as specialized,
	//// hence allowing specialized functions
	////
	////

	// declarations
	// to avoid being mixed with what the outside world calls, all of them are private and have a _ prefix

	template <bool bInline, int iSize>
	T _get(int iIndex) const;

	template <bool bInline, int iSize>
	void _set(int iIndex, T eValue);

	template <bool bInline, int iSize>
	void _allocate(T eValue = (T)DEFAULT);

	template <bool bInline, int iSize>
	void _setAll(T val);

	template <bool bInline>
	unsigned int _getNumBoolBlocks() const;

	template <int iSize>
	void _Read(CvSavegameReader& reader);

	template <int iSize>
	void _Write(CvSavegameWriter& writer) const;

	//
	// The actual specialized impletation of the functions
	//

	// get
	template<>
	__forceinline T _get<false, ENUMMAP_SIZE_NATIVE>(int iIndex) const
	{
		return (T)(m_pArrayFull ? m_pArrayFull[iIndex] : DEFAULT);
	}
	template<>
	__forceinline T _get<false, ENUMMAP_SIZE_1_BYTE>(int iIndex) const
	{
		return (T)(m_pArrayChar ? m_pArrayChar[iIndex] : DEFAULT);
	}
	template<>
	__forceinline T _get<false, ENUMMAP_SIZE_2_BYTES>(int iIndex) const
	{
		return (T)(m_pArrayShort ? m_pArrayShort[iIndex] : DEFAULT);
	}
	template<>
	__forceinline T _get<false, ENUMMAP_SIZE_BOOL>(int iIndex) const
	{
		return m_pArrayBool ? HasBit(m_pArrayBool[getBoolArrayBlock(iIndex)], getBoolArrayIndexInBlock(iIndex)) : DEFAULT;
	}
	template<>
	__forceinline T _get<true, ENUMMAP_SIZE_NATIVE>(int iIndex) const
	{
		return m_InlineNative[iIndex];
	}
	template<>
	__forceinline T _get<true, ENUMMAP_SIZE_1_BYTE>(int iIndex) const
	{
		return (T)m_Inline_1_byte[iIndex];
	}
	template<>
	__forceinline T _get<true, ENUMMAP_SIZE_2_BYTES>(int iIndex) const
	{
		return (T)m_Inline_2_bytes[iIndex];
	}
	template<>
	__forceinline T _get<true, ENUMMAP_SIZE_BOOL>(int iIndex) const
	{
		return HasBit(m_InlineBoolArray[getBoolArrayBlock(iIndex)], getBoolArrayIndexInBlock(iIndex));
	}

	// set
	template<>
	__forceinline void _set<false, ENUMMAP_SIZE_NATIVE>(int iIndex, T eValue)
	{
		m_pArrayFull[iIndex] = eValue;
	}
	template<>
	__forceinline void _set<false, ENUMMAP_SIZE_1_BYTE>(int iIndex, T eValue)
	{
		m_pArrayChar[iIndex] = eValue;
	}
	template<>
	__forceinline void _set<false, ENUMMAP_SIZE_2_BYTES>(int iIndex, T eValue)
	{
		m_pArrayShort[iIndex] = eValue;
	}
	template<>
	__forceinline void _set<false, ENUMMAP_SIZE_BOOL>(int iIndex, T eValue)
	{
		SetBit(m_pArrayBool[getBoolArrayBlock(iIndex)], getBoolArrayIndexInBlock(iIndex), eValue ? 1 : 0);
	}
	template<>
	__forceinline void _set<true, ENUMMAP_SIZE_NATIVE>(int iIndex, T eValue)
	{
		m_InlineNative[iIndex] = eValue;
	}
	template<>
	__forceinline void _set<true, ENUMMAP_SIZE_1_BYTE>(int iIndex, T eValue)
	{
		m_Inline_1_byte[iIndex] = eValue;
	}
	template<>
	__forceinline void _set<true, ENUMMAP_SIZE_2_BYTES>(int iIndex, T eValue)
	{
		m_Inline_2_bytes[iIndex] = eValue;
	}
	template<>
	__forceinline void _set<true, ENUMMAP_SIZE_BOOL>(int iIndex, T eValue)
	{
		SetBit(m_InlineBoolArray[getBoolArrayBlock(iIndex)], getBoolArrayIndexInBlock(iIndex), eValue ? 1 : 0);
	}

	// setAll
	template<>
	__forceinline void _setAll<false, ENUMMAP_SIZE_NATIVE>(T eValue)
	{
		std::fill_n(m_pArrayFull, (int)numElements(), eValue);
	}
	template<>
	__forceinline void _setAll<false, ENUMMAP_SIZE_1_BYTE>(T eValue)
	{
		std::fill_n(m_pArrayChar, (int)numElements(), eValue);
	}
	template<>
	__forceinline void _setAll<false, ENUMMAP_SIZE_2_BYTES>(T eValue)
	{
		std::fill_n(m_pArrayShort, (int)numElements(), eValue);
	}
	template<>
	__forceinline void _setAll<false, ENUMMAP_SIZE_BOOL>(T eValue)
	{
		std::fill_n(m_pArrayBool, (int)_getNumBoolBlocks<false>(), eValue ? MAX_UNSIGNED_INT : 0);
	}
	template<>
	__forceinline void _setAll<true, ENUMMAP_SIZE_NATIVE>(T eValue)
	{
		std::fill_n(&m_InlineNative[0], (int)numElements(), eValue);
	}
	template<>
	__forceinline void _setAll<true, ENUMMAP_SIZE_1_BYTE>(T eValue)
	{
		std::fill_n(&m_Inline_1_byte[0], (int)numElements(), eValue);
	}
	template<>
	__forceinline void _setAll<true, ENUMMAP_SIZE_2_BYTES>(T eValue)
	{
		std::fill_n(&m_Inline_2_bytes[0], (int)numElements(), eValue);
	}
	template<>
	__forceinline void _setAll<true, ENUMMAP_SIZE_BOOL>(T eValue)
	{
		std::fill_n(&m_InlineBoolArray[0], (int)_getNumBoolBlocks<bINLINE>(), eValue ? MAX_UNSIGNED_INT : 0);
	}

	// allocate
	template<>
	void _allocate<false, ENUMMAP_SIZE_NATIVE>(T eValue)
	{
		FAssert(m_pArrayFull == NULL);
		m_pArrayFull = new T[numElements()];
		_setAll<bINLINE, SIZE>(eValue);
	}
	template<>
	void _allocate<false, ENUMMAP_SIZE_1_BYTE>(T eValue)
	{
		FAssert(m_pArrayChar == NULL);
		m_pArrayChar = new char[numElements()];
		_setAll<bINLINE, SIZE>(eValue);
	}
	template<>
	void _allocate<false, ENUMMAP_SIZE_2_BYTES>(T eValue)
	{
		FAssert(m_pArrayShort == NULL);
		m_pArrayShort = new short[numElements()];
		_setAll<bINLINE, SIZE>(eValue);
	}
	template<>
	void _allocate<false, ENUMMAP_SIZE_BOOL>(T eValue)
	{
		FAssert(m_pArrayBool == NULL);
		m_pArrayBool = new unsigned int[_getNumBoolBlocks<bINLINE>()];
		_setAll<bINLINE, SIZE>(eValue);
	}
	template<>
	void _allocate<true, ENUMMAP_SIZE_NATIVE>(T eValue)
	{
		FAssertMsg(false, "EnumMap::_allocate shouldn't be called for classes with inline memory");
	}
	template<>
	void _allocate<true, ENUMMAP_SIZE_1_BYTE>(T eValue)
	{
		FAssertMsg(false, "EnumMap::_allocate shouldn't be called for classes with inline memory");
	}
	template<>
	void _allocate<true, ENUMMAP_SIZE_2_BYTES>(T eValue)
	{
		FAssertMsg(false, "EnumMap::_allocate shouldn't be called for classes with inline memory");
	}
	template<>
	void _allocate<true, ENUMMAP_SIZE_BOOL>(T eValue)
	{
		FAssertMsg(false, "EnumMap::_allocate shouldn't be called for classes with inline memory");
	}

	////
	//// bool specialization
	////

	template <>
	__forceinline unsigned int _getNumBoolBlocks<false>() const
	{
		return (numElements() + 31) / 32;
	}

	template <>
	__forceinline unsigned int _getNumBoolBlocks<true>() const
	{
		return NUM_BOOL_BLOCKS;
	}

	template<>
	void _Read<ENUMMAP_SIZE_BOOL>(CvSavegameReader& reader)
	{
		const int iLength = ArrayType((LengthType)0) == NO_JIT_ARRAY_TYPE ? ArrayLength((LengthType)0) : reader.GetXmlSize(ArrayType((LengthType)0));

		LengthType eStart;
		LengthType eEnd;

		if (iLength <= 0x100)
		{
			byte iBuffer;
			reader.Read(iBuffer);
			eStart = (LengthType)iBuffer;
			reader.Read(iBuffer);
			eEnd = (LengthType)iBuffer;
		}
		else
		{
			unsigned short iBuffer;
			reader.Read(iBuffer);
			eStart = (LengthType)iBuffer;
			reader.Read(iBuffer);
			eEnd = (LengthType)iBuffer;
		}

		byte iBuffer = 0;

		for (LengthType eLoop = eStart; eLoop <= eEnd; ++eLoop)
		{
			const byte iBlockIndex = (eLoop - eStart) & ENUMMAP_BITMASK_8_BIT;
			if (iBlockIndex == 0)
			{
				// read next block
				reader.Read(iBuffer);
			}
			set((LengthType)reader.ConvertIndex(ArrayType(eStart), eLoop), HasBit(iBuffer, iBlockIndex));
		}
	}

	template<>
	void _Write<ENUMMAP_SIZE_BOOL>(CvSavegameWriter& writer) const
	{
		const int iLength = ArrayType((LengthType)0) == NO_JIT_ARRAY_TYPE ? ArrayLength((LengthType)0) : writer.GetXmlSize(ArrayType((LengthType)0));


		LengthType eStart = (LengthType)MAX_INT;
		LengthType eEnd = (LengthType)MIN_INT;

		for (LengthType eLoop = (LengthType)First(); eLoop < getLength(); ++eLoop)
		{
			if (get(eLoop))
			{
				if (eStart > eLoop)
				{
					eStart = eLoop;
				}
				if (eEnd < eLoop)
				{
					eEnd = eLoop;
				}
			}
		}

		// write empty array if needed

		if (eStart == (LengthType)MAX_INT)
		{
			// saving 1 to 0 will cause the reader to not read any data, hence no need to save blocks
			FAssert(eEnd == (LengthType)MIN_INT);
			eStart = (LengthType)1;
			eEnd = (LengthType)0;
		}

		if (iLength <= 0x100)
		{
			byte iBuffer = eStart;
			writer.Write(iBuffer);
			iBuffer = eEnd;
			writer.Write(iBuffer);
		}
		else
		{
			unsigned short iBuffer = eStart;
			writer.Write(iBuffer);
			iBuffer = eEnd;
			writer.Write(iBuffer);
		}

		byte iBuffer = 0;

		for (LengthType eLoop = eStart; eLoop <= eEnd; ++eLoop)
		{
			const byte iBlockIndex = (eLoop - eStart) & ENUMMAP_BITMASK_8_BIT;
			if(get(eLoop)){
				SetBit(iBuffer,iBlockIndex);
			}
			if (iBlockIndex == ENUMMAP_BITMASK_8_BIT)
			{
				 //now 8 bits are set, -> write to file
				writer.Write(iBuffer);
				iBuffer = 0;
			}
		}
		// there might be an unfinished byte, -> write to file
		const byte iBlockIndex = (eEnd - eStart) & ENUMMAP_BITMASK_8_BIT;
		if(iBlockIndex != ENUMMAP_BITMASK_8_BIT)
		{
			writer.Write(iBuffer);
		}
	}
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


template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType>
inline EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::EnumMapBase()
	: m_pArrayFull(NULL)
{
	// bools can only default to 0 or 1
	BOOST_STATIC_ASSERT(SIZE != ENUMMAP_SIZE_BOOL || DEFAULT == 0 || DEFAULT == 1);

	FAssertMsg(bINLINE || sizeof(*this) == 4, "EnumMap is supposed to only contain a pointer");
	FAssertMsg(getLength() >= (IndexType)0 && getLength() <= ArrayLength((LengthType)0), "Custom length out of range");
	FAssertMsg(First() >= (IndexType)0 && (First() < getLength() || First() == (IndexType)0), "Custom length out of range");

	if (bINLINE)
	{
		this->setAll((T)DEFAULT);
	}
}

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType>
inline EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::~EnumMapBase()
{
	reset();
}

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType>
__forceinline T EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::getDefault() const
{
	return (T)DEFAULT;
}

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType>
__forceinline IndexType EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::First() const
{
	return ArrayStart((T_SUBSET)0);
}

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType>
__forceinline IndexType EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::getLength() const
{
	return ArrayLength((T_SUBSET)0);
}

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType>
__forceinline IndexType EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::numElements() const
{
	// apparently subtracting two IndexTypes results in int, not IndexType
	return (IndexType)(getLength() - First());
}

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType>
__forceinline T EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::get(IndexType eIndex) const
{
	FAssert(eIndex >= First() && eIndex < getLength());
	return _get<bINLINE, SIZE>(eIndex - First());
}

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType>
inline void EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::set(IndexType eIndex, T eValue)
{
	FAssert(eIndex >= First() && eIndex < getLength());
	if (!bINLINE && m_pArrayFull == NULL)
	{
		if (eValue == DEFAULT) 
		{
			return;
		}
		_allocate<bINLINE, SIZE>();
	}
	_set<bINLINE, SIZE>(eIndex - First(), eValue);
}

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType>
inline void EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::add(IndexType eIndex, T eValue)
{
	BOOST_STATIC_ASSERT(SIZE != ENUMMAP_SIZE_BOOL);
	FAssert(eIndex >= First() && eIndex < getLength());
	if (eValue != 0)
	{
		set(eIndex, eValue + get(eIndex));
	}
}

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType>
inline void EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::safeSet(IndexType eIndex, T eValue)
{
	if (eIndex >= First() && eIndex < getLength())
	{
		set(eIndex, eValue);
	}
}

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType>
inline void EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::safeAdd(IndexType eIndex, T eValue)
{
	BOOST_STATIC_ASSERT(SIZE != ENUMMAP_SIZE_BOOL);
	if (eIndex >= First() && eIndex < getLength())
	{
		add(eIndex, eValue);
	}
}

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType>
inline void EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::addAll(T eValue)
{
	BOOST_STATIC_ASSERT(SIZE != ENUMMAP_SIZE_BOOL);
	if (eValue != 0)
	{
		for (IndexType eIndex = First(); eIndex < getLength(); ++eIndex)
		{
			add(eIndex, (T)eValue);
		}
	}
}

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType>
inline int EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::getTotal() const
{
	// bINLINE is set at compile time and if true, isAllocated will always be true
	// used here to tell the compiler that the true statement (not allocated) can be declared unreachable at compile time
	if (!bINLINE && !isAllocated())
	{
		// no need to loop through unallocated memory
		return DEFAULT * getLength();
	}
	else
	{
		int iReturnVal = 0;
		const int iLength = getLength();
		for (IndexType eIndex = First(); eIndex < iLength; ++eIndex)
		{
			iReturnVal += get(eIndex);
		}
		return iReturnVal;
	}
}

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType>
inline bool EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::isAllocated() const
{
	return bINLINE || m_pArrayFull != NULL;
}

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType>
inline bool EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::hasContent() const
{
	if (bINLINE_BOOL)
	{
		for (int i = 0; i < NUM_BOOL_BLOCKS; ++i)
		{
			if (m_InlineBoolArray[i] != BOOL_BLOCK_DEFAULT)
			{
				return true;
			}
		}
		return false;
	}

	if (bINLINE || m_pArrayFull != NULL)
	{
		if (SIZE == ENUMMAP_SIZE_BOOL)
		{
			const int iNumBlocks = _getNumBoolBlocks<bINLINE_BOOL>();
			for (int i = 0; i < iNumBlocks; ++i)
			{
				if (m_pArrayBool[i] != BOOL_BLOCK_DEFAULT)
				{
					return true;
				}
			}
		}
		else
		{
			for (IndexType eIndex = (IndexType)0; eIndex < numElements(); ++eIndex)
			{
				if (get(eIndex) != DEFAULT)
				{
					return true;
				}
			}
		}
		// now we cheat and alter data despite being const.
		// We just detected all data to be of the default value, meaning the array is not used.
		// Release the data to save memory. It won't change how the outside world view the EnumMap.
		if (!bINLINE)
		{
			(const_cast <EnumMapBase*> (this))->reset();
		}
	}
	return false;
}

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType>
inline T EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::getMin() const
{
	BOOST_STATIC_ASSERT(SIZE != ENUMMAP_SIZE_BOOL);
	if (bINLINE_NATIVE) {
		return (T)(*std::min_element(&m_InlineNative[0], &m_InlineNative[0] + numElements()));
	}
	if (bINLINE_1_BYTE) {
		return (T)(*std::min_element(&m_Inline_1_byte[0], &m_Inline_1_byte[0] + numElements()));
	}
	if (bINLINE_2_BYTES) {
		return (T)(*std::min_element(&m_Inline_2_bytes[0], &m_Inline_2_bytes[0] + numElements()));
	}
	if (m_pArrayFull == NULL)
	{
		return DEFAULT;
	}
	return (T)(*std::min_element(m_pArrayFull, m_pArrayFull + numElements()));
}

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType>
inline T EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::getMax() const
{
	BOOST_STATIC_ASSERT(SIZE != ENUMMAP_SIZE_BOOL);
	if(bINLINE_NATIVE)
	{
		return (T)(*std::max_element(&m_InlineNative[0], &m_InlineNative[0] + numElements()));
	}
	if (bINLINE_1_BYTE) {
		return (T)(*std::min_element(&m_Inline_1_byte[0], &m_Inline_1_byte[0] + numElements()));
	}
	if (bINLINE_2_BYTES) {
		return (T)(*std::min_element(&m_Inline_2_bytes[0], &m_Inline_2_bytes[0] + numElements()));
	}
	if (m_pArrayFull == NULL)
	{
		return DEFAULT;
	}
	return (T)(*std::max_element(m_pArrayFull, m_pArrayFull + numElements()));
}

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType>
inline void EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::keepMin(IndexType eIndex, T eValue)
{
	BOOST_STATIC_ASSERT(SIZE != ENUMMAP_SIZE_BOOL);
	FAssert(eIndex >= First() && eIndex < getLength());
	if (get(eIndex) > eValue)
	{
		set(eIndex, eValue);
	}
}

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType>
inline void EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::keepMax(IndexType eIndex, T eValue)
{
	BOOST_STATIC_ASSERT(SIZE != ENUMMAP_SIZE_BOOL);
	FAssert(eIndex >= First() && eIndex < getLength());
	if (get(eIndex) < eValue)
	{
		set(eIndex, eValue);
	}
}

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType>
inline void EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::reset()
{
	if (bINLINE)
	{
		// can't free inlined memory. Set it all to default instead
		setAll((T)DEFAULT);
	}
	else
	{
		// doesn't matter which one we free. They all point to the same memory address, which is what matters here.
		SAFE_DELETE_ARRAY(m_pArrayFull);
	}
}

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType>
inline void EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::setAll(T eValue)
{
	if (!bINLINE && m_pArrayChar == NULL)
	{
		if (eValue == DEFAULT)
		{
			return;
		}
		_allocate<bINLINE, SIZE>(eValue);
	}
	else
	{
		_setAll<bINLINE, SIZE>(eValue);
	}
}

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType>
inline void EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::copyToVector(std::vector<T>& thisVector) const
{
	BOOST_STATIC_ASSERT(SIZE != ENUMMAP_SIZE_BOOL);
	thisVector.reserve(getLength());
	thisVector.resize(getLength(), (T)DEFAULT);

	if (isAllocated())
	{
		for (IndexType eIndex = First(); eIndex < getLength(); ++eIndex)
		{
			thisVector[eIndex] = get(eIndex);
		}
	}
}

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType>
inline void EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::copyFromVector(const std::vector<T>& thisVector)
{
	BOOST_STATIC_ASSERT(SIZE != ENUMMAP_SIZE_BOOL);
	FAssert((unsigned int)getLength() == thisVector.size());

	for (IndexType eIndex = First(); eIndex < getLength(); ++eIndex)
	{
		set(eIndex, thisVector[eIndex]);
	}
}

//
// Savegame code
//

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType>
__forceinline void EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::Read(CvSavegameReader& reader)
{
	_Read<SIZE>(reader);
}

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType>
template<int SIZE2>
inline void EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::_Read(CvSavegameReader& reader)
{
	BOOST_STATIC_ASSERT(SIZE == SIZE2);
	BOOST_STATIC_ASSERT(SIZE != ENUMMAP_SIZE_BOOL);

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
		IndexType iFirst = (IndexType)(iBuffer >> SAVE_ARRAY_INDEX_OFFSET);
		bool bMultiByte = HasBit(iBuffer, SAVE_ARRAY_MULTI_BYTE);
		bool bLast = HasBit(iBuffer, SAVE_ARRAY_LAST_TOKEN);

		if (bMultiByte)
		{
			IndexType iLast;
			if (iLength > 256)
			{
				unsigned short iReadBuffer;
				reader.Read(iReadBuffer);
				iLast = (IndexType)iReadBuffer;
			}
			else
			{
				byte iReadBuffer;
				reader.Read(iReadBuffer);
				iLast = (IndexType)iReadBuffer;
			}
			
			for (IndexType i = iFirst; i <= iLast; ++i)
			{
				// use a buffer because then it doesn't matter if T, array size and save size differs.
				T tBuffer;
				reader.Read(tBuffer);
				IndexType iIndex = (IndexType)reader.ConvertIndex(ArrayType((LengthType)0), i);
				if (iIndex != (IndexType)-1)
				{
					// -1 means xml entry was removed. Discard the data in question and hope the savegame will still work
					set(iIndex, tBuffer);
				}
			}
		}
		else
		{
			// single variable token
			T tBuffer;
			reader.Read(tBuffer);
			IndexType iIndex = (IndexType)reader.ConvertIndex(ArrayType((LengthType)0), iFirst);
			if (iIndex != (IndexType)-1)
			{
				// -1 means xml entry was removed. Discard the data in question and hope the savegame will still work
				set(iIndex, tBuffer);
			}
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


template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType>
__forceinline void EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::Write(CvSavegameWriter& writer) const
{
	_Write<SIZE>(writer);
}

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType>
template<int SIZE2>
inline void EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::_Write(CvSavegameWriter& writer) const
{
	BOOST_STATIC_ASSERT(SIZE == SIZE2);
	BOOST_STATIC_ASSERT(SIZE != ENUMMAP_SIZE_BOOL);

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
	for (IndexType eIndex = First(); eIndex < getLength(); ++eIndex)
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
		IndexType iFirst = tokens[i].first;
		IndexType iLast = tokens[i].last;

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
			
			for (int i = iFirst; i <= (int)iLast; ++i)
			{
				writer.Write(get((IndexType)i));
			}
		}
	}
}

//
// operator overloads
//

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType>
inline EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>& EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::operator=(const EnumMapBase &rhs)
{
	BOOST_STATIC_ASSERT(false); // implementation isn't generic yet
	BOOST_STATIC_ASSERT(SIZE != ENUMMAP_SIZE_BOOL);
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

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType> template<class T2, int DEFAULT2>
inline EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>& EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::operator=(const EnumMapBase<IndexType, T2, DEFAULT2, T_SUBSET, LengthType> &rhs)
{
	BOOST_STATIC_ASSERT(false); // implementation isn't generic yet
	BOOST_STATIC_ASSERT(SIZE != ENUMMAP_SIZE_BOOL);
	if (rhs.isAllocated())
	{
		for (IndexType eIndex = First(); eIndex < getLength(); ++eIndex)
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

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType> template<class T2, int DEFAULT2>
inline EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>& EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::operator+=(const EnumMapBase<IndexType, T2, DEFAULT2, T_SUBSET, LengthType> &rhs)
{
	BOOST_STATIC_ASSERT(false); // implementation isn't generic yet
	BOOST_STATIC_ASSERT(SIZE != ENUMMAP_SIZE_BOOL);
	if (rhs.isAllocated())
	{
		for (IndexType eIndex = First(); eIndex < getLength(); ++eIndex)
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

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType> template<class T2, int DEFAULT2>
inline EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>& EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::operator-=(const EnumMapBase<IndexType, T2, DEFAULT2, T_SUBSET, LengthType> &rhs)
{
	BOOST_STATIC_ASSERT(false); // implementation isn't generic yet
	BOOST_STATIC_ASSERT(SIZE != ENUMMAP_SIZE_BOOL);
	if (rhs.isAllocated())
	{
		for (IndexType eIndex = First(); eIndex < getLength(); ++eIndex)
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

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType> template<class T2, int DEFAULT2>
inline bool EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::operator==(const EnumMapBase<IndexType, T2, DEFAULT2, T_SUBSET, LengthType> &rhs) const
{
	BOOST_STATIC_ASSERT(false); // implementation isn't generic yet
	BOOST_STATIC_ASSERT(SIZE != ENUMMAP_SIZE_BOOL);
	if (!rhs.isAllocated() && !isAllocated())
	{
		return DEFAULT == DEFAULT2;
	}

	if (SIZE == SIZE2 && SIZE_OF_T == SIZE_OF_T2 && rhs.isAllocated() && isAllocated())
	{
		return memcmp(m_pArrayChar, rhs.m_pArrayChar, getLength() * SIZE_OF_T) == 0;
	}

	for (IndexType eIndex = First(); eIndex < getLength(); ++eIndex)
	{
		if (get(eIndex) != rhs.get(eIndex))
		{
			return false;
		}
	}
	return true;
}

template<class IndexType, class T, int DEFAULT, class T_SUBSET, class LengthType> template<class T2, int DEFAULT2>
inline bool EnumMapBase<IndexType, T, DEFAULT, T_SUBSET, LengthType>::operator!=(const EnumMapBase<IndexType, T2, DEFAULT2, T_SUBSET, LengthType> &rhs) const
{
	BOOST_STATIC_ASSERT(false); // implementation isn't generic yet
	BOOST_STATIC_ASSERT(SIZE != ENUMMAP_SIZE_BOOL);
	if (!rhs.isAllocated() && !isAllocated())
	{
		return DEFAULT != DEFAULT2;
	}

	if (SIZE == SIZE2 && SIZE_OF_T == SIZE_OF_T2 && rhs.isAllocated() && isAllocated())
	{
		return memcmp(m_pArrayChar, rhs.m_pArrayChar, getLength() * SIZE_OF_T) != 0;
	}

	for (IndexType eIndex = First(); eIndex < getLength(); ++eIndex)
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

__forceinline bool ArrayDefault(bool var) { return 0; } \
template <> struct EnumMapGetDefault<bool> \
{ \
enum { DEFAULT_VALUE = 0, SIZE = ENUMMAP_SIZE_BOOL, SIZE_OF_T = sizeof(char) }; \
};

#define SET_ARRAY_DEFAULT( X ) \
__forceinline X ArrayDefault( X var) { return 0; } \
template <> struct EnumMapGetDefault<X> \
{ \
	enum { DEFAULT_VALUE = 0, SIZE = ENUMMAP_SIZE_NATIVE, SIZE_OF_T = sizeof(X) }; \
};

SET_ARRAY_DEFAULT(int);
SET_ARRAY_DEFAULT(short);
SET_ARRAY_DEFAULT(char);
SET_ARRAY_DEFAULT(unsigned int);
SET_ARRAY_DEFAULT(unsigned short);
SET_ARRAY_DEFAULT(byte);

#define SET_ARRAY_XML_ENUM( VAR, NUM_TYPES, JIT_TYPE, COMPILE_NUM_TYPES ) \
__forceinline VAR ArrayStart(VAR var) { return (VAR)0; } \
__forceinline VAR ArrayLength(VAR var) { return NUM_TYPES; } \
__forceinline JITarrayTypes ArrayType(VAR var) { return JIT_TYPE; } \
template <> struct EnumMapGetDefault<VAR> \
{ \
	enum { \
		DEFAULT_VALUE = -1, \
		SIZE = COMPILE_NUM_TYPES > 128 ? ENUMMAP_SIZE_2_BYTES : ENUMMAP_SIZE_1_BYTE, \
		SIZE_OF_T = SIZE, \
		LENGTH = COMPILE_NUM_TYPES, \
	}; \
};

// Byte size is set in enums
// If the length isn't known at compile time, MAX_SHORT is assumed.
// Setting the byte size means say PlayerTypes will use 1 byte instead of 4. Works because MAX_PLAYERS <= 128

//                 type                , length                   , JIT_ARRAY type            , length at compile time (MAX_SHORT if unknown)
SET_ARRAY_XML_ENUM(ArtStyleTypes       , NUM_ARTSTYLE_TYPES       , JIT_ARRAY_ART_STYLE       , COMPILE_TIME_NUM_ARTSTYLE_TYPES       );
SET_ARRAY_XML_ENUM(BonusTypes          , NUM_BONUS_TYPES          , JIT_ARRAY_BONUS           , COMPILE_TIME_NUM_BONUS_TYPES          );
SET_ARRAY_XML_ENUM(BuildTypes          , NUM_BUILD_TYPES          , JIT_ARRAY_BUILD           , COMPILE_TIME_NUM_BUILD_TYPES          );
SET_ARRAY_XML_ENUM(BuildingTypes       , NUM_BUILDING_TYPES       , JIT_ARRAY_BUILDING        , COMPILE_TIME_NUM_BUILDING_TYPES       );
SET_ARRAY_XML_ENUM(BuildingClassTypes  , NUM_BUILDINGCLASS_TYPES  , JIT_ARRAY_BUILDING_CLASS  , COMPILE_TIME_NUM_BUILDINGCLASS_TYPES  );
SET_ARRAY_XML_ENUM(SpecialBuildingTypes, NUM_SPECIALBUILDING_TYPES, JIT_ARRAY_BUILDING_SPECIAL, COMPILE_TIME_NUM_SPECIALBUILDING_TYPES);
SET_ARRAY_XML_ENUM(CivEffectTypes      , NUM_CIV_EFFECT_TYPES     , JIT_ARRAY_CIV_EFFECT      , COMPILE_TIME_NUM_CIV_EFFECT_TYPES     );
SET_ARRAY_XML_ENUM(CivicTypes          , NUM_CIVIC_TYPES          , JIT_ARRAY_CIVIC           , COMPILE_TIME_NUM_CIVIC_TYPES          );
SET_ARRAY_XML_ENUM(CivicOptionTypes    , NUM_CIVICOPTION_TYPES    , JIT_ARRAY_CIVIC_OPTION    , COMPILE_TIME_NUM_CIVICOPTION_TYPES    );
SET_ARRAY_XML_ENUM(CivilizationTypes   , NUM_CIVILIZATION_TYPES   , JIT_ARRAY_CIVILIZATION    , COMPILE_TIME_NUM_CIVILIZATION_TYPES   );
SET_ARRAY_XML_ENUM(ClimateTypes        , NUM_CLIMATE_TYPES        , JIT_ARRAY_CLIMATE         , COMPILE_TIME_NUM_CLIMATE_TYPES        );
SET_ARRAY_XML_ENUM(ColorTypes          , NUM_COLOR_TYPES          , JIT_ARRAY_COLOR           , COMPILE_TIME_NUM_COLOR_TYPES          );
SET_ARRAY_XML_ENUM(ContactTypes        , NUM_CONTACT_TYPES        , JIT_ARRAY_CONTACT         , COMPILE_TIME_NUM_CONTACT_TYPES        );
SET_ARRAY_XML_ENUM(CultureLevelTypes   , NUM_CULTURELEVEL_TYPES   , JIT_ARRAY_CULTURE         , COMPILE_TIME_NUM_CULTURELEVEL_TYPES   );
SET_ARRAY_XML_ENUM(DiplomacyTypes      , NUM_DIPLOMACY_TYPES      , JIT_ARRAY_DIPLO           , COMPILE_TIME_NUM_DIPLOMACY_TYPES      );
SET_ARRAY_XML_ENUM(DomainTypes         , NUM_DOMAIN_TYPES         , JIT_ARRAY_DOMAIN          , COMPILE_TIME_NUM_DOMAIN_TYPES         );
SET_ARRAY_XML_ENUM(EraTypes            , NUM_ERA_TYPES            , JIT_ARRAY_ERA             , COMPILE_TIME_NUM_ERA_TYPES            );
SET_ARRAY_XML_ENUM(EmotionTypes        , NUM_EMOTION_TYPES        , JIT_ARRAY_EMOTION         , COMPILE_TIME_NUM_EMOTION_TYPES        );
SET_ARRAY_XML_ENUM(EmphasizeTypes      , NUM_EMPHASIZE_TYPES      , JIT_ARRAY_EMPHASIZE       , COMPILE_TIME_NUM_EMPHASIZE_TYPES      );
SET_ARRAY_XML_ENUM(EuropeTypes         , NUM_EUROPE_TYPES         , JIT_ARRAY_EUROPE          , COMPILE_TIME_NUM_EUROPE_TYPES         );
SET_ARRAY_XML_ENUM(EventTypes          , NUM_EVENT_TYPES          , JIT_ARRAY_EVENT           , COMPILE_TIME_NUM_EVENT_TYPES          );
SET_ARRAY_XML_ENUM(EventTriggerTypes   , NUM_EVENTTRIGGER_TYPES   , JIT_ARRAY_EVENT_TRIGGER   , COMPILE_TIME_NUM_EVENTTRIGGER_TYPES   );
SET_ARRAY_XML_ENUM(FatherTypes         , NUM_FATHER_TYPES         , JIT_ARRAY_FATHER          , COMPILE_TIME_NUM_FATHER_TYPES         );
SET_ARRAY_XML_ENUM(FatherPointTypes    , NUM_FATHER_POINT_TYPES   , JIT_ARRAY_FATHER_POINT    , COMPILE_TIME_NUM_FATHER_POINT_TYPES   );
SET_ARRAY_XML_ENUM(FeatTypes           , NUM_FEAT_TYPES           , JIT_ARRAY_FEAT            , COMPILE_TIME_NUM_FEAT_TYPES           );
SET_ARRAY_XML_ENUM(FeatureTypes        , NUM_FEATURE_TYPES        , JIT_ARRAY_FEATURE         , COMPILE_TIME_NUM_FEATURE_TYPES        );
SET_ARRAY_XML_ENUM(GameOptionTypes     , NUM_GAMEOPTION_TYPES     , JIT_ARRAY_GAME_OPTION     , COMPILE_TIME_NUM_GAMEOPTION_TYPES     );
SET_ARRAY_XML_ENUM(GameSpeedTypes      , NUM_GAMESPEED_TYPES      , JIT_ARRAY_GAME_SPEED      , COMPILE_TIME_NUM_GAMESPEED_TYPES      );
SET_ARRAY_XML_ENUM(GoodyTypes          , NUM_GOODY_TYPES          , JIT_ARRAY_GOODY           , COMPILE_TIME_NUM_GOODY_TYPES          );
SET_ARRAY_XML_ENUM(HandicapTypes       , NUM_HANDICAP_TYPES       , JIT_ARRAY_HANDICAP        , COMPILE_TIME_NUM_HANDICAP_TYPES       );
SET_ARRAY_XML_ENUM(HurryTypes          , NUM_HURRY_TYPES          , JIT_ARRAY_HURRY           , COMPILE_TIME_NUM_HURRY_TYPES          );
SET_ARRAY_XML_ENUM(ImprovementTypes    , NUM_IMPROVEMENT_TYPES    , JIT_ARRAY_IMPROVEMENT     , COMPILE_TIME_NUM_IMPROVEMENT_TYPES    );
SET_ARRAY_XML_ENUM(InvisibleTypes      , NUM_INVISIBLE_TYPES      , JIT_ARRAY_INVISIBLE       , COMPILE_TIME_NUM_INVISIBLE_TYPES      );
SET_ARRAY_XML_ENUM(LeaderHeadTypes     , NUM_LEADER_TYPES         , JIT_ARRAY_LEADER_HEAD     , COMPILE_TIME_NUM_LEADER_TYPES         );
SET_ARRAY_XML_ENUM(MemoryTypes         , NUM_MEMORY_TYPES         , JIT_ARRAY_MEMORY          , COMPILE_TIME_NUM_MEMORY_TYPES         );
SET_ARRAY_XML_ENUM(PlayerColorTypes    , NUM_PLAYERCOLOR_TYPES    , JIT_ARRAY_PLAYER_COLOR    , COMPILE_TIME_NUM_PLAYERCOLOR_TYPES    );
SET_ARRAY_XML_ENUM(PlayerOptionTypes   , NUM_PLAYEROPTION_TYPES   , JIT_ARRAY_PLAYER_OPTION   , COMPILE_TIME_NUM_PLAYEROPTION_TYPES   );
SET_ARRAY_XML_ENUM(ProfessionTypes     , NUM_PROFESSION_TYPES     , JIT_ARRAY_PROFESSION      , COMPILE_TIME_NUM_PROFESSION_TYPES     );
SET_ARRAY_XML_ENUM(PromotionTypes      , NUM_PROMOTION_TYPES      , JIT_ARRAY_PROMOTION       , COMPILE_TIME_NUM_PROMOTION_TYPES      );
SET_ARRAY_XML_ENUM(RouteTypes          , NUM_ROUTE_TYPES          , JIT_ARRAY_ROUTE           , COMPILE_TIME_NUM_ROUTE_TYPES          );
SET_ARRAY_XML_ENUM(SeaLevelTypes       , NUM_SEALEVEL_TYPES       , JIT_ARRAY_SEA_LEVEL       , COMPILE_TIME_NUM_SEALEVEL_TYPES       );
SET_ARRAY_XML_ENUM(StrategyTypes       , NUM_STRATEGY_TYPES       , JIT_ARRAY_STRATEGY        , COMPILE_TIME_NUM_STRATEGY_TYPES       );
SET_ARRAY_XML_ENUM(TerrainTypes        , NUM_TERRAIN_TYPES        , JIT_ARRAY_TERRAIN         , COMPILE_TIME_NUM_TERRAIN_TYPES        );
SET_ARRAY_XML_ENUM(TraitTypes          , NUM_TRAIT_TYPES          , JIT_ARRAY_TRAIT           , COMPILE_TIME_NUM_TRAIT_TYPES          );
SET_ARRAY_XML_ENUM(UnitTypes           , NUM_UNIT_TYPES           , JIT_ARRAY_UNIT            , COMPILE_TIME_NUM_UNIT_TYPES           );
SET_ARRAY_XML_ENUM(UnitAITypes         , NUM_UNITAI_TYPES         , JIT_ARRAY_UNIT_AI         , COMPILE_TIME_NUM_UNITAI_TYPES         );
SET_ARRAY_XML_ENUM(UnitClassTypes      , NUM_UNITCLASS_TYPES      , JIT_ARRAY_UNIT_CLASS      , COMPILE_TIME_NUM_UNITCLASS_TYPES      );
SET_ARRAY_XML_ENUM(UnitCombatTypes     , NUM_UNITCOMBAT_TYPES     , JIT_ARRAY_UNIT_COMBAT     , COMPILE_TIME_NUM_UNITCOMBAT_TYPES     );
SET_ARRAY_XML_ENUM(SpecialUnitTypes    , NUM_SPECIALUNIT_TYPES    , JIT_ARRAY_UNIT_SPECIAL    , COMPILE_TIME_NUM_SPECIALUNIT_TYPES    );
SET_ARRAY_XML_ENUM(VictoryTypes        , NUM_VICTORY_TYPES        , JIT_ARRAY_VICTORY         , COMPILE_TIME_NUM_VICTORY_TYPES        );
SET_ARRAY_XML_ENUM(WorldSizeTypes      , NUM_WORLDSIZE_TYPES      , JIT_ARRAY_WORLD_SIZE      , COMPILE_TIME_NUM_WORLDSIZE_TYPES      );
SET_ARRAY_XML_ENUM(YieldTypes          , NUM_YIELD_TYPES          , JIT_ARRAY_YIELD           , COMPILE_TIME_NUM_YIELD_TYPES          );


SET_ARRAY_XML_ENUM(AreaAITypes         , NUM_AREAAI_TYPES         , NO_JIT_ARRAY_TYPE         , COMPILE_TIME_NUM_AREAAI_TYPES         );
SET_ARRAY_XML_ENUM(CivCategoryTypes    , NUM_CIV_CATEGORY_TYPES   , JIT_ARRAY_CIV_CATEGORY    , COMPILE_TIME_NUM_CIV_CATEGORY_TYPES   );
SET_ARRAY_XML_ENUM(PlayerTypes         , NUM_PLAYER_TYPES         , NO_JIT_ARRAY_TYPE         , COMPILE_TIME_NUM_PLAYER_TYPES         );
SET_ARRAY_XML_ENUM(TeamTypes           , NUM_TEAM_TYPES           , NO_JIT_ARRAY_TYPE         , COMPILE_TIME_NUM_TEAM_TYPES           );
SET_ARRAY_XML_ENUM(WarPlanTypes        , NUM_WARPLAN_TYPES        , NO_JIT_ARRAY_TYPE         , COMPILE_TIME_NUM_WARPLAN_TYPES        );


#define SET_ARRAY_XML_ENUM_LENGTH_ONLY( VAR, FIRST, NUM_TYPES, JIT_TYPE, COMPILE_LENGTH ) \
__forceinline VAR ArrayStart(VAR var) { return FIRST; } \
__forceinline VAR ArrayLength(VAR var) { return NUM_TYPES; } \
__forceinline JITarrayTypes ArrayType(VAR var) { return JIT_TYPE; } \
template <> struct EnumMapGetDefault<VAR> \
{ \
	enum { \
		LENGTH = COMPILE_LENGTH, \
	}; \
};

SET_ARRAY_XML_ENUM_LENGTH_ONLY(CityPlotTypes      , FIRST_CITY_PLOT          , NUM_CITY_PLOTS           , NO_JIT_ARRAY_TYPE, NUM_CITY_PLOTS_2_PLOTS);

//
// List of various types of EnumMaps
// In most cases it's not nice code to include all parameters from EnumMapBase.
// Adding other classes, which always sets the default makes it easier to add EnumMaps as arguments to functions etc.
//

// The different classes:
// EnumMap:
///  the default, which takes a length (xml file) and type to store. Use this one as much as possible.
// EnumMapDefault:
///  same as EnumMap, but you can set what the default value should be (read: the value assigned by constructor/reset)
///  Note: indexes at default value aren't saved, hence saving an EnumMap with lots of default values will take less file space
// EnumMapInt:
///  Allows the index to be set by int instead of the enum type
///  Do not use if it can be avoided. The tradeoff of easier coding is that the compiler can no longer catch bugs.
///  The index arguments are consistently set to require the correct enum types specifically to catch bugs where arguments are switched etc.
///  Using EnumMapInt can easily end up with the compiler accepting swapped arguments etc.
///  For this reason, only use this if you know you have to typecast anyway for each call, like when using CityPlotTypes for length.

template<class IndexType, class T, int DEFAULT>
class EnumMapDefault : public EnumMapBase <IndexType, T, DEFAULT> {};

template<class IndexType, class T>
class EnumMap : public EnumMapBase <IndexType, T, EnumMapGetDefault<T>::DEFAULT_VALUE> {};

template<class IndexType, class T, int DEFAULT = 0>
class EnumMapInt : public EnumMapBase <int, T, DEFAULT, IndexType, IndexType> {};

#endif
