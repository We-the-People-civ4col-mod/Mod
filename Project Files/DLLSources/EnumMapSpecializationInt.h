#ifndef ENUM_MAP_SPECIALIZATION_DYNAMIC_CHAR_H
#define ENUM_MAP_SPECIALIZATION_DYNAMIC_CHAR_H
#pragma once

#include "EnumMap.h"

template<class IndexType, class T, int DEFAULT, class LengthType, int STATIC, class STORAGE, int LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapVariable
{
	BOOST_STATIC_ASSERT(0);
};

template<class IndexType, class T, int DEFAULT, class LengthType, class STORAGE, int LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>
	: public EnumMapCore<IndexType, LengthType, LENGTH_KNOWN_WHILE_COMPILING>
{
public:
	void reset()
	{
		setALLDefault();
	}
	void allocate()
	{
	}
	bool isAllocated() const
	{
		return true;
	}

protected:
	void setALLDefault()
	{
		for (LengthType i = (LengthType)0; i < VARINFO<LengthType>::length(); ++i)
		{
			m_pArray[i] = (STORAGE)DEFAULT;
		}
	}

	EnumMapVariable()
	{
		setALLDefault();
	}


	STORAGE m_pArray[VARINFO<LengthType>::LENGTH];
};

template<class IndexType, class T, int DEFAULT, class LengthType, class STORAGE, int LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>
	: public EnumMapCore<IndexType, LengthType, LENGTH_KNOWN_WHILE_COMPILING>
{
public:
	void reset()
	{
		SAFE_DELETE_ARRAY(m_pArray);
	}
	void allocate()
	{
		if (!isAllocated())
		{
			m_pArray = new STORAGE[VARINFO<LengthType>::length()];
			setALLDefault < DEFAULT == 0 || sizeof(STORAGE) == 1 ? 1 : 0> ();
		}
	}
	bool isAllocated() const
	{
		return m_pArray != NULL;
	}

	~EnumMapVariable()
	{
		SAFE_DELETE_ARRAY(m_pArray);
	}

protected:
	template<int iType>
	void setALLDefault()
	{
		allocate();
		for (LengthType i = (LengthType)0; i < VARINFO<LengthType>::length(); ++i)
		{
			m_pArray[i] = (STORAGE)DEFAULT;
		}
	}

	template<>
	void setALLDefault<1>()
	{
		allocate();
		memset(m_pArray, DEFAULT, VARINFO<LengthType>::length() * sizeof(STORAGE));
	}

	EnumMapVariable()
		: m_pArray(NULL)
	{}

	STORAGE* m_pArray;
};

template<class IndexType, class T, int DEFAULT, class LengthType, int STATIC, class STORAGE, int LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapShared
	: public EnumMapVariable<IndexType, T, DEFAULT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>
{
public:
	T get(IndexType eIndex) const
	{
		FAssert(VARINFO<LengthType>::isInRange((LengthType)eIndex));
		return (T)(isAllocated() ? m_pArray[eIndex] : DEFAULT);
	}
	void set(IndexType eIndex, T eValue)
	{
		FAssert(VARINFO<LengthType>::isInRange((LengthType)eIndex));
		allocate();
		m_pArray[eIndex] = eValue;
	}
	void add(IndexType eIndex, T eValue)
	{
		FAssert(VARINFO<LengthType>::isInRange((LengthType)eIndex));
		allocate();
		m_pArray[eIndex] += eValue;
	}

	void keepMin(IndexType eIndex, T eValue)
	{
		FAssert(VARINFO<LengthType>::isInRange((LengthType)eIndex));
		if (eValue < get(eIndex))
		{
			set(eIndex, eValue);
		}
	}
	void keepMax(IndexType eIndex, T eValue)
	{
		FAssert(VARINFO<LengthType>::isInRange((LengthType)eIndex));
		if (eValue > get(eIndex))
		{
			set(eIndex, eValue);
		}
	}

	T getDefault() const
	{
		return (T)DEFAULT;
	}

	bool isDefault(IndexType eIndex) const
	{
		FAssert(VARINFO<LengthType>::isInRange((LengthType)eIndex));
		return get(eIndex) == getDefault();
	}

	int getTotal() const
	{
		int iTotal = 0;
		for (IndexType i = First(); i < getEnd(); ++i)
		{
			iTotal += get(i);
		}
		return iTotal;
	}

	void setAll(T eValue)
	{
		for (IndexType i = First(); i < getEnd(); ++i)
		{
			set(i, eValue);
		}
	}
	void addAll(T eValue)
	{
		for (IndexType i = First(); i < getEnd(); ++i)
		{
			add(i, eValue);
		}
	}
	
	T getMin() const
	{
		T eMin = get((IndexType)0);
		for (IndexType i = First(); i < getEnd(); ++i)
		{
			T eLoopValue = get(i);
			if (eMin > eLoopValue)
			{
				eMin = eLoopValue;
			}
		}
		return eMin;
	}
	T getMax() const
	{
		T eMax = get((IndexType)0);
		for (IndexType i = First(); i < getEnd(); ++i)
		{
			T eLoopValue = get(i);
			if (eMax < eLoopValue)
			{
				eMax = eLoopValue;
			}
		}
		return eMax;
	}

	IndexType First() const
	{
		return VARINFO<LengthType>::start();
	}

	IndexType getEnd() const
	{
		return VARINFO<LengthType>::end();
	}

	IndexType getLength() const
	{
		return VARINFO<LengthType>::length();
	}
	bool hasContent() const
	{
		if (isAllocated())
		{
			for (IndexType i = First(); i < getEnd(); ++i)
			{
				if (!isDefault(i))
				{
					return true;
				}
			}
		}
		return false;
	}

	bool hasContent()
	{
		if (isAllocated())
		{
			for (IndexType i = First(); i < getEnd(); ++i)
			{
				if (get(i) != getDefault())
				{
					return true;
				}
			}
			reset();
		}
		return false;
	}

	void copyToVector(std::vector<T>& thisVector) const
	{
		thisVector.reserve(getLength());
		thisVector.resize(getLength(), getDefault());

		if (isAllocated())
		{
			for (IndexType eIndex = First(); eIndex < getLength(); ++eIndex)
			{
				thisVector[eIndex] = get(eIndex);
			}
		}
	}
	void copyFromVector(const std::vector<T>& thisVector)
	{
		FAssert((unsigned int)getLength() == thisVector.size());

		for (IndexType eIndex = First(); eIndex < getLength(); ++eIndex)
		{
			set(eIndex, thisVector[eIndex]);
		}
	}

protected:
	EnumMapShared() : EnumMapVariable<IndexType, T, DEFAULT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>() {}
};


//template<class IndexType, class T, int DEFAULT, class LengthType, int STATIC, int TYPE>
template<class IndexType, class T, int DEFAULT, class LengthType, int STATIC, int LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, VARIABLE_TYPE_CHAR, LENGTH_KNOWN_WHILE_COMPILING>
	: public EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, char, LENGTH_KNOWN_WHILE_COMPILING>
{
protected:
	EnumMapBase() : EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, char, LENGTH_KNOWN_WHILE_COMPILING>() {}
};

template<class IndexType, class T, int DEFAULT, class LengthType, int STATIC, int LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, VARIABLE_TYPE_SHORT, LENGTH_KNOWN_WHILE_COMPILING>
	: public EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, short, LENGTH_KNOWN_WHILE_COMPILING>
{
protected:
	EnumMapBase() : EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, short, LENGTH_KNOWN_WHILE_COMPILING>() {}
};

template<class IndexType, class T, int DEFAULT, class LengthType, int STATIC, int LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, VARIABLE_TYPE_GENERIC, LENGTH_KNOWN_WHILE_COMPILING>
	: public EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, T, LENGTH_KNOWN_WHILE_COMPILING>
{
protected:
	EnumMapBase() : EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, T, LENGTH_KNOWN_WHILE_COMPILING>() {}
};



#endif