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

template<class IndexType, class T, int DEFAULT, class LengthType, int STATIC, int LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapBoolVariable
{
	BOOST_STATIC_ASSERT(0);
};

template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_STATIC, LENGTH_KNOWN_WHILE_COMPILING>
	: public EnumMapCore<IndexType, LengthType, LENGTH_KNOWN_WHILE_COMPILING>
{
public:
	static const Length = VARINFO<IndexType>::LENGTH;
	static const int iNumBlocks = (VARINFO<IndexType>::LENGTH + 31) / 32;
	static const byte DefaultByte = DEFAULT ? 0xFF : 0;

	IndexType getLength() const
	{
		return Length;
	}

	void reset()
	{
		memset(m_pArray, DefaultByte, iNumBlocks * 4);
	}
	void allocate()
	{
	}
	bool isAllocated() const
	{
		return true;
	}

	bool get(IndexType eIndex) const
	{
		return m_pArray[eIndex / 32].get(eIndex);
	}
	void set(IndexType eIndex, bool bValue)
	{
		m_pArray[eIndex / 32].set(eIndex, bValue);
	}

protected:
	BoolToken m_pArray[iNumBlocks];

	EnumMapBoolVariable()
	{
		reset();
	}
};

template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, LENGTH_KNOWN_WHILE_COMPILING>
	: public EnumMapCore<IndexType, LengthType, LENGTH_KNOWN_WHILE_COMPILING>
{
public:
	IndexType getLength() const
	{
		return VARINFO<LengthType>::length();
	}
	static const byte DefaultByte = DEFAULT ? 0xFF : 0;

	void reset()
	{
		SAFE_DELETE_ARRAY(m_pArray);
	}
	void allocate()
	{
		if (!isAllocated())
		{
			reset();
			m_pArray = new  BoolToken[getNumBlocks()];
			memset(m_pArray, DefaultByte, getNumBlocks() * 4);
		}
	}
	bool isAllocated() const
	{
		return m_pArray != NULL;
	}

	bool get(IndexType eIndex) const
	{
		return isAllocated() ? m_pArray[eIndex / 32].get(eIndex) : DEFAULT;
	}
	void set(IndexType eIndex, bool bValue)
	{
		if (isAllocated() || bValue != DEFAULT)
		{
			allocate();
			m_pArray[eIndex / 32].set(eIndex, bValue);
		}
	}

protected:
	BoolToken *m_pArray;

	EnumMapBoolVariable()
		: m_pArray(NULL)
	{
	}

	int getNumBlocks()
	{
		return (getLength() + 31) / 32;
	}
};

template<class IndexType, class T, int DEFAULT, class LengthType, int STATIC, int LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, VARIABLE_TYPE_BOOL, LENGTH_KNOWN_WHILE_COMPILING>
	: public EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, STATIC, LENGTH_KNOWN_WHILE_COMPILING>
{
public:
	bool hasContent() const
	{
		if (isAllocated())
		{
			for (IndexType i = First(); i < End(); ++i)
			{
				if (get(i) != DEFAULT)
				{
					return true;
				}
			}
		}
		return false;
	}
	IndexType First() const
	{
		return VARINFO<LengthType>::start();
	}

	IndexType End() const
	{
		return VARINFO<LengthType>::end();
	}

	IndexType getLength() const
	{
		return VARINFO<LengthType>::length();
	}

	IndexType numElements() const
	{
		return VARINFO<LengthType>::length();
	}
	int getTotal() const
	{
		int iTotal = 0;
		if (isAllocated())
		{
			for (IndexType i = First(); i < End(); ++i)
			{
				if (get(i))
				{
					++iTotal;
				}
			}
		}
		else
		{
			iTotal = getLength() * DEFAULT;
		}
		return iTotal;
	}

protected:
	EnumMapBase() : EnumMapBoolVariable<IndexType, T, DEFAULT, LengthType, STATIC, LENGTH_KNOWN_WHILE_COMPILING>() {}
	BOOST_STATIC_ASSERT(DEFAULT == 0 || DEFAULT == 1);
};


#endif
