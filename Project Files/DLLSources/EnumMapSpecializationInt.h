#ifndef ENUM_MAP_SPECIALIZATION_DYNAMIC_CHAR_H
#define ENUM_MAP_SPECIALIZATION_DYNAMIC_CHAR_H
#pragma once

#include "EnumMap.h"



template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapVariable
{
	BOOST_STATIC_ASSERT(0);
};

template<class IndexType, class T, int DEFAULT, class LengthType, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>
	: public EnumMapCore<IndexType, LengthType, LENGTH_KNOWN_WHILE_COMPILING>
{
public:
	void reset();
	void allocate();
	bool isAllocated() const;

protected:
	void setALLDefault();

	EnumMapVariable();

	STORAGE m_pArray[VARINFO<LengthType>::LENGTH];
};

template<class IndexType, class T, int DEFAULT, class LengthType, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>
	: public EnumMapCore<IndexType, LengthType, LENGTH_KNOWN_WHILE_COMPILING>
{
public:
	void reset();
	void allocate();
	bool isAllocated() const;
	~EnumMapVariable();

protected:
	template<int iType>
	void setALLDefault();

	EnumMapVariable();

	STORAGE* m_pArray;

	// specialized functions
	// specialized functions can't be placed outside the class declaration unless the class itself is also specialize
	template<>
	void setALLDefault<1>()
	{
		allocate();
		memset(m_pArray, DEFAULT, VARINFO<LengthType>::length() * sizeof(STORAGE));
	}
};


template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapShared
	: public EnumMapVariable<IndexType, T, DEFAULT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>
{
public:
	T get(IndexType eIndex) const;
	void set(IndexType eIndex, T eValue);
	void add(IndexType eIndex, T eValue);
	void keepMin(IndexType eIndex, T eValue);
	void keepMax(IndexType eIndex, T eValue);
	T getDefault() const;
	bool isDefault(IndexType eIndex) const;
	int getTotal() const;
	void setAll(T eValue);
	void addAll(T eValue);
	T getMin() const;
	T getMax() const;
	IndexType First() const;
	IndexType getEnd() const;
	IndexType getLength() const;
	bool hasContent() const;
	bool hasContent();
	void copyToVector(std::vector<T>& thisVector) const;
	void copyFromVector(const std::vector<T>& thisVector);
protected:
	EnumMapShared() : EnumMapVariable<IndexType, T, DEFAULT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>() {}
};

//
// EnumMapBase
//

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, VARIABLE_TYPE_CHAR, LENGTH_KNOWN_WHILE_COMPILING>
	: public EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, char, LENGTH_KNOWN_WHILE_COMPILING>
{
protected:
	EnumMapBase() : EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, char, LENGTH_KNOWN_WHILE_COMPILING>() {}
};

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, VARIABLE_TYPE_SHORT, LENGTH_KNOWN_WHILE_COMPILING>
	: public EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, short, LENGTH_KNOWN_WHILE_COMPILING>
{
protected:
	EnumMapBase() : EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, short, LENGTH_KNOWN_WHILE_COMPILING>() {}
};

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapBase<IndexType, T, DEFAULT, LengthType, STATIC, VARIABLE_TYPE_GENERIC, LENGTH_KNOWN_WHILE_COMPILING>
	: public EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, T, LENGTH_KNOWN_WHILE_COMPILING>
{
protected:
	EnumMapBase() : EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, T, LENGTH_KNOWN_WHILE_COMPILING>() {}
};


//
// static
//

template<class IndexType, class T, int DEFAULT, class LengthType, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::reset()
{
	setALLDefault();
}

template<class IndexType, class T, int DEFAULT, class LengthType, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::allocate()
{
}

template<class IndexType, class T, int DEFAULT, class LengthType, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
bool EnumMapVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::isAllocated() const
{
	return true;
}

template<class IndexType, class T, int DEFAULT, class LengthType, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::setALLDefault()
{
	for (LengthType i = (LengthType)0; i < VARINFO<LengthType>::length(); ++i)
	{
		m_pArray[i] = (STORAGE)DEFAULT;
	}
}

template<class IndexType, class T, int DEFAULT, class LengthType, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
EnumMapVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::EnumMapVariable()
{
	setALLDefault();
}



//
//  dynamic
//

template<class IndexType, class T, int DEFAULT, class LengthType, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::reset()
{
	SAFE_DELETE_ARRAY(m_pArray);
}

template<class IndexType, class T, int DEFAULT, class LengthType, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::allocate()
{
	if (!isAllocated())
	{
		m_pArray = new STORAGE[VARINFO<LengthType>::length()];
		setALLDefault < DEFAULT == 0 || sizeof(STORAGE) == 1 ? 1 : 0>();
	}
}

template<class IndexType, class T, int DEFAULT, class LengthType, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
bool EnumMapVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::isAllocated() const
{
	return m_pArray != NULL;
}

template<class IndexType, class T, int DEFAULT, class LengthType, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
EnumMapVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::~EnumMapVariable()
{
	SAFE_DELETE_ARRAY(m_pArray);
}

template<class IndexType, class T, int DEFAULT, class LengthType, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
template<int iType>
void EnumMapVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::setALLDefault()
{
	allocate();
	for (LengthType i = (LengthType)0; i < VARINFO<LengthType>::length(); ++i)
	{
		m_pArray[i] = (STORAGE)DEFAULT;
	}
}

template<class IndexType, class T, int DEFAULT, class LengthType, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
EnumMapVariable<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::EnumMapVariable()
	: m_pArray(NULL)
{}

//
// shared
//

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
T EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::get(IndexType eIndex) const
{
	FAssert(VARINFO<LengthType>::isInRange((LengthType)eIndex));
	return (T)(isAllocated() ? m_pArray[eIndex] : DEFAULT);
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::set(IndexType eIndex, T eValue)
{
	FAssert(VARINFO<LengthType>::isInRange((LengthType)eIndex));
	allocate();
	m_pArray[eIndex] = eValue;
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::add(IndexType eIndex, T eValue)
{
	FAssert(VARINFO<LengthType>::isInRange((LengthType)eIndex));
	allocate();
	m_pArray[eIndex] += eValue;
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::keepMin(IndexType eIndex, T eValue)
{
	FAssert(VARINFO<LengthType>::isInRange((LengthType)eIndex));
	if (eValue < get(eIndex))
	{
		set(eIndex, eValue);
	}
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::keepMax(IndexType eIndex, T eValue)
{
	FAssert(VARINFO<LengthType>::isInRange((LengthType)eIndex));
	if (eValue > get(eIndex))
	{
		set(eIndex, eValue);
	}
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
T EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::getDefault() const
{
	return (T)DEFAULT;
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
bool EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::isDefault(IndexType eIndex) const
{
	FAssert(VARINFO<LengthType>::isInRange((LengthType)eIndex));
	return get(eIndex) == getDefault();
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
int EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::getTotal() const
{
	int iTotal = 0;
	for (IndexType i = First(); i < getEnd(); ++i)
	{
		iTotal += get(i);
	}
	return iTotal;
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::setAll(T eValue)
{
	for (IndexType i = First(); i < getEnd(); ++i)
	{
		set(i, eValue);
	}
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::addAll(T eValue)
{
	for (IndexType i = First(); i < getEnd(); ++i)
	{
		add(i, eValue);
	}
}
	
template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
T EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::getMin() const
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

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
T EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::getMax() const
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

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
IndexType EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::First() const
{
	return VARINFO<LengthType>::start();
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
IndexType EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::getEnd() const
{
	return VARINFO<LengthType>::end();
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
IndexType EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::getLength() const
{
	return VARINFO<LengthType>::length();
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
bool EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::hasContent() const
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

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
bool EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::hasContent()
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

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::copyToVector(std::vector<T>& thisVector) const
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

template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapShared<IndexType, T, DEFAULT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::copyFromVector(const std::vector<T>& thisVector)
{
	FAssert((unsigned int)getLength() == thisVector.size());

	for (IndexType eIndex = First(); eIndex < getLength(); ++eIndex)
	{
		set(eIndex, thisVector[eIndex]);
	}
}

#endif