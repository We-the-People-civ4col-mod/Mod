#ifndef ENUM_MAP_SPECIALIZATION_BOOL_H
#define ENUM_MAP_SPECIALIZATION_BOOL_H
#pragma once

#include "EnumMap.h"

struct BoolToken
{
	bool get(int iIndex) const
	{
		return HasBit(iData, iIndex & 0x1F);
	}
	void set(int iIndex, bool bValue)
	{
		SetBit(iData, iIndex & 0x1F, bValue);
	}

	union
	{
		unsigned int iData;
		struct
		{
			// We will likely never need to access these directly.
			// However it makes the debugger data more readable.
			bool b0 : 1;
			bool b1 : 1;
			bool b2 : 1;
			bool b3 : 1;
			bool b4 : 1;
			bool b5 : 1;
			bool b6 : 1;
			bool b7 : 1;
			bool b8 : 1;
			bool b9 : 1;
			bool b10 : 1;
			bool b11 : 1;
			bool b12 : 1;
			bool b13 : 1;
			bool b14 : 1;
			bool b15 : 1;
			bool b16 : 1;
			bool b17 : 1;
			bool b18 : 1;
			bool b19 : 1;
			bool b20 : 1;
			bool b21 : 1;
			bool b22 : 1;
			bool b23 : 1;
			bool b24 : 1;
			bool b25 : 1;
			bool b26 : 1;
			bool b27 : 1;
			bool b28 : 1;
			bool b29 : 1;
			bool b30 : 1;
			bool b31 : 1;
		};
	};
};

BOOST_STATIC_ASSERT(sizeof(BoolToken) == 4);

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapBoolVariable
{
	BOOST_STATIC_ASSERT(0);
};

template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_STATIC, LENGTH_KNOWN_WHILE_COMPILING>
	: public EnumMapCore<IndexType, LengthType, LENGTH_KNOWN_WHILE_COMPILING>
{
public:
	static const int iNumBlocks = (COMPILE_NUM_ELEMENTS + 31) / 32;
	static const byte DefaultByte = DEFAULT ? 0xFF : 0;
	static const unsigned int DefaultInt = DEFAULT == 0 ? 0 : MAX_UNSIGNED_INT;

	void reset();
	void allocate();
	bool isAllocated() const;
	bool hasContent() const;
	void releaseMemoryIfUnused();

	bool get(IndexType eIndex) const;
	void set(IndexType eIndex, bool bValue);

protected:
	BoolToken m_pArray[iNumBlocks];
	EnumMapBoolVariable();

	void assignmentOperator(const EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_STATIC, LENGTH_KNOWN_WHILE_COMPILING>& rhs);
};

template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, LENGTH_KNOWN_WHILE_COMPILING>
	: public EnumMapCore<IndexType, LengthType, LENGTH_KNOWN_WHILE_COMPILING>
{
public:
	static const byte DefaultByte = DEFAULT ? 0xFF : 0;
	static const unsigned int DefaultInt = DEFAULT == 0 ? 0 : MAX_UNSIGNED_INT;

	void reset();
	void allocate();
	bool isAllocated() const;
	bool hasContent();
	bool hasContent() const;
	void releaseMemoryIfUnused();

	bool get(IndexType eIndex) const;
	void set(IndexType eIndex, bool bValue);

protected:
	union
	{
		BoolToken m_eArray;
		BoolToken *m_pArray;
	};

	EnumMapBoolVariable();
	~EnumMapBoolVariable();

	int getNumBlocks() const;

	void assignmentOperator(const EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, LENGTH_KNOWN_WHILE_COMPILING>& rhs);
};

template<class IndexType, class T, int DEFAULT, class LengthType>
class EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_LENGTH_ALL_KNOWN>
	: public EnumMapCore<IndexType, LengthType, VARIABLE_LENGTH_ALL_KNOWN>
{
public:
	static const int iNumBlocks = (NUM_ELEMENTS + 31) / 32;
	static const byte DefaultByte = DEFAULT ? 0xFF : 0;
	static const unsigned int DefaultInt = DEFAULT == 0 ? 0 : MAX_UNSIGNED_INT;

	void reset();
	void allocate();
	bool isAllocated() const;
	bool hasContent();
	bool hasContent() const;
	void releaseMemoryIfUnused();

	bool get(IndexType eIndex) const;
	void set(IndexType eIndex, bool bValue);

protected:
	BoolToken *m_pArray;
	EnumMapBoolVariable();
	~EnumMapBoolVariable();

	void assignmentOperator(const EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_LENGTH_ALL_KNOWN>& rhs);
};

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, VARIABLE_TYPE_BOOL, LENGTH_KNOWN_WHILE_COMPILING>
	: public EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, STATIC, LENGTH_KNOWN_WHILE_COMPILING>
{
public:
	IndexType First() const;
	IndexType getLength() const;
	int getTotal() const;
	int getNumTrueElements() const;
protected:
	EnumMapBase() : EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, STATIC, LENGTH_KNOWN_WHILE_COMPILING>() {}
	BOOST_STATIC_ASSERT(DEFAULT == 0 || DEFAULT == 1); // bools can only be true or false
};

//
// static functions
//

template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_STATIC, LENGTH_KNOWN_WHILE_COMPILING>::reset()
{
	memset(m_pArray, DefaultByte, iNumBlocks * 4);
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_STATIC, LENGTH_KNOWN_WHILE_COMPILING>::allocate()
{
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
bool EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_STATIC, LENGTH_KNOWN_WHILE_COMPILING>::isAllocated() const
{
	return true;
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
bool EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_STATIC, LENGTH_KNOWN_WHILE_COMPILING>::hasContent() const
{
	for (int i = 0; i < iNumBlocks; ++i)
	{
		if (m_pArray[i].iData != DefaultInt)
		{
			return true;
		}
	}
	return false;
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_STATIC, LENGTH_KNOWN_WHILE_COMPILING>::releaseMemoryIfUnused()
{
	// it's static. Nothing to release.
	// needed purely because this function has to exist for all specialization cases
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
bool EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_STATIC, LENGTH_KNOWN_WHILE_COMPILING>::get(IndexType eIndex) const
{
	FAssert(isInRange(eIndex));
	eIndex = eIndex - FIRST;
	return m_pArray[eIndex / 32].get(eIndex);
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_STATIC, LENGTH_KNOWN_WHILE_COMPILING>::set(IndexType eIndex, bool bValue)
{
	FAssert(isInRange(eIndex));
	eIndex = eIndex - FIRST;
	m_pArray[eIndex / 32].set(eIndex, bValue);
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_STATIC, LENGTH_KNOWN_WHILE_COMPILING>::EnumMapBoolVariable()
{
	reset();
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void assignmentOperator(const EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_STATIC, LENGTH_KNOWN_WHILE_COMPILING>& rhs)
{
	for (int i = 0; i < iNumBlocks; ++i)
	{
		m_pArray[i] = rhs.m_pArray[i];
	}
}

//
// Dynamic functions
// Unknown length
//

template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, LENGTH_KNOWN_WHILE_COMPILING>::reset()
{
	if (NUM_ELEMENTS <= 32)
	{
		m_eArray.iData = DefaultInt;
	}
	else
	{
		SAFE_DELETE_ARRAY(m_pArray);
	}
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, LENGTH_KNOWN_WHILE_COMPILING>::allocate()
{
	if (!isAllocated())
	{
		FAssert(NUM_ELEMENTS > 32);
		m_pArray = new  BoolToken[getNumBlocks()];
		memset(m_pArray, DefaultByte, getNumBlocks() * 4);
	}
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
bool EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, LENGTH_KNOWN_WHILE_COMPILING>::isAllocated() const
{
	return NUM_ELEMENTS <= 32 || m_pArray != NULL;
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
bool EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, LENGTH_KNOWN_WHILE_COMPILING>::hasContent()
{
	if (NUM_ELEMENTS <= 32)
	{
		return m_eArray.iData != DefaultInt;
	}
	if (isAllocated())
	{
		const int iNumBlocks = getNumBlocks();
		for (int i = 0; i < iNumBlocks; ++i)
		{
			if (m_pArray[i].iData != DefaultInt)
			{
				return true;
			}
		}
		reset();
	}
	return false;
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
bool EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, LENGTH_KNOWN_WHILE_COMPILING>::hasContent() const
{
	if (NUM_ELEMENTS <= 32)
	{
		return m_eArray.iData != DefaultInt;
	}
	if (isAllocated())
	{
		const int iNumBlocks = getNumBlocks();
		for (int i = 0; i < iNumBlocks; ++i)
		{
			if (m_pArray[i].iData != DefaultInt)
			{
				return true;
			}
		}
	}
	return false;
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, LENGTH_KNOWN_WHILE_COMPILING>::releaseMemoryIfUnused()
{
	hasContent();
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
bool EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, LENGTH_KNOWN_WHILE_COMPILING>::get(IndexType eIndex) const
{
	FAssert(isInRange(eIndex));
	eIndex = eIndex - FIRST;
	if (NUM_ELEMENTS <= 32)
	{
		return m_eArray.get(eIndex);
	}
	else
	{
		return isAllocated() ? m_pArray[eIndex / 32].get(eIndex) : DEFAULT;
	}
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, LENGTH_KNOWN_WHILE_COMPILING>::set(IndexType eIndex, bool bValue)
{
	FAssert(isInRange(eIndex));
	if (isAllocated() || bValue != DEFAULT)
	{
		allocate();
		eIndex = eIndex - FIRST;
		if (NUM_ELEMENTS <= 32)
		{
			m_eArray.set(eIndex, bValue);
		}
		else
		{
			m_pArray[eIndex / 32].set(eIndex, bValue);
		}
	}
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, LENGTH_KNOWN_WHILE_COMPILING>::EnumMapBoolVariable()
	: m_pArray(NULL)
{
	reset();
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, LENGTH_KNOWN_WHILE_COMPILING>::~EnumMapBoolVariable()
{
	reset();
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
int EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, LENGTH_KNOWN_WHILE_COMPILING>::getNumBlocks() const
{
	return (NUM_ELEMENTS + 31) / 32;
}


template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void assignmentOperator(const EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, LENGTH_KNOWN_WHILE_COMPILING>& rhs)
{
	if (NUM_ELEMENTS <= 32)
	{
		m_eArray.iData = rhs.m_eArray.iData;
	}
	else
	{
		if (!rhs.isAllocated())
		{
			reset();
			return;
		}
		allocate();
		memcpy(m_pArray, rhs.m_pArray, NUM_BLOCKS * sizeof(BoolToken));
	}
}

//
// Dynamic functions
// Known length
//

template<class IndexType, class T, int DEFAULT, class LengthType>
void EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_LENGTH_ALL_KNOWN>::reset()
{
	SAFE_DELETE_ARRAY(m_pArray);
}

template<class IndexType, class T, int DEFAULT, class LengthType>
void EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_LENGTH_ALL_KNOWN>::allocate()
{
	if (!isAllocated())
	{
		m_pArray = new  BoolToken[iNumBlocks];
		memset(m_pArray, DefaultByte, iNumBlocks * 4);
	}
}

template<class IndexType, class T, int DEFAULT, class LengthType>
bool EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_LENGTH_ALL_KNOWN>::isAllocated() const
{
	return m_pArray != NULL;
}

template<class IndexType, class T, int DEFAULT, class LengthType>
bool EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_LENGTH_ALL_KNOWN>::hasContent()
{
	if (isAllocated())
	{
		for (int i = 0; i < iNumBlocks; ++i)
		{
			if (m_pArray[i].iData != DefaultInt)
			{
				return true;
			}
		}
		reset();
	}
	return false;
}

template<class IndexType, class T, int DEFAULT, class LengthType>
bool EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_LENGTH_ALL_KNOWN>::hasContent() const
{
	if (isAllocated())
	{
		for (int i = 0; i < iNumBlocks; ++i)
		{
			if (m_pArray[i].iData != DefaultInt)
			{
				return true;
			}
		}
	}
	return false;
}

template<class IndexType, class T, int DEFAULT, class LengthType>
void EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_LENGTH_ALL_KNOWN>::releaseMemoryIfUnused()
{
	hasContent();
}

template<class IndexType, class T, int DEFAULT, class LengthType>
bool EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_LENGTH_ALL_KNOWN>::get(IndexType eIndex) const
{
	FAssert(isInRange(eIndex));
	eIndex = eIndex - FIRST;
	return isAllocated() ? m_pArray[eIndex / 32].get(eIndex) : DEFAULT;
}

template<class IndexType, class T, int DEFAULT, class LengthType>
void EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_LENGTH_ALL_KNOWN>::set(IndexType eIndex, bool bValue)
{
	FAssert(isInRange(eIndex));
	if (isAllocated() || bValue != DEFAULT)
	{
		allocate();
		eIndex = eIndex - FIRST;
		m_pArray[eIndex / 32].set(eIndex, bValue);
	}
}

template<class IndexType, class T, int DEFAULT, class LengthType>
EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_LENGTH_ALL_KNOWN>::EnumMapBoolVariable()
	: m_pArray(NULL)
{
}

template<class IndexType, class T, int DEFAULT, class LengthType>
EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_LENGTH_ALL_KNOWN>::~EnumMapBoolVariable()
{
	SAFE_DELETE_ARRAY(m_pArray);
}

template<class IndexType, class T, int DEFAULT, class LengthType>
void EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_LENGTH_ALL_KNOWN>::assignmentOperator(const EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_LENGTH_ALL_KNOWN>& rhs)
{
	if (!rhs.isAllocated())
	{
		reset();
		return;
	}
	allocate();
	memcpy(m_pArray, rhs.m_pArray, NUM_BLOCKS * sizeof(BoolToken));
}


//
// Base class (entry)
//

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
IndexType EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, VARIABLE_TYPE_BOOL, LENGTH_KNOWN_WHILE_COMPILING>::First() const
{
	return FIRST;
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
IndexType EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, VARIABLE_TYPE_BOOL, LENGTH_KNOWN_WHILE_COMPILING>::getLength() const
{
	return LAST + (IndexType)1;
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
int EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, VARIABLE_TYPE_BOOL, LENGTH_KNOWN_WHILE_COMPILING>::getTotal() const
{
	if (isAllocated())
	{
		int iTotal = 0;
		for (IndexType i = FIRST; i <= LAST; ++i)
		{
			if (get(i))
			{
				++iTotal;
			}
		}
		return iTotal;
	}
	else
	{
		return NUM_ELEMENTS * DEFAULT;
	}
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
int EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, VARIABLE_TYPE_BOOL, LENGTH_KNOWN_WHILE_COMPILING>::getNumTrueElements() const
{
	if (!isAllocated())
	{
		return DEFAULT * (int)NUM_ELEMENTS;
	}
	int iCount = 0;
	for (IndexType eLoop = FIRST; eLoop <= LAST; ++eLoop)
	{
		if (get(eLoop))
		{
			++iCount;
		}
	}
	return iCount;
}

#endif
