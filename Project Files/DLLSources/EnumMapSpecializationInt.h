#ifndef ENUM_MAP_SPECIALIZATION_DYNAMIC_CHAR_H
#define ENUM_MAP_SPECIALIZATION_DYNAMIC_CHAR_H
#pragma once

#include "EnumMap.h"
#include "EnumMapCore.h"



template<class IndexType, class T, int DEFAULT_INT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapVariable
{
	BOOST_STATIC_ASSERT(0);
};

template<class IndexType, class T, int DEFAULT_INT, class LengthType, class STORAGE>
class EnumMapVariable<IndexType, T, DEFAULT_INT, LengthType, VARIABLE_TYPE_STATIC, STORAGE, VARIABLE_LENGTH_ALL_KNOWN>
	: public EnumMapCore<IndexType, LengthType, VARIABLE_LENGTH_ALL_KNOWN>
{
public:
	static const T DEFAULT = (T)DEFAULT_INT;

	void reset();
	void allocate();
	bool isAllocated() const;

protected:
	void setALLDefault();

	EnumMapVariable();

	void assignmentOperator(const EnumMapVariable<IndexType, T, DEFAULT_INT, LengthType, VARIABLE_TYPE_STATIC, STORAGE, VARIABLE_LENGTH_ALL_KNOWN>& rhs);

	STORAGE m_pArray[COMPILE_NUM_ELEMENTS];
};

template<class IndexType, class T, int DEFAULT_INT, class LengthType, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapVariable<IndexType, T, DEFAULT_INT, LengthType, VARIABLE_TYPE_DYNAMIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>
	: public EnumMapCore<IndexType, LengthType, LENGTH_KNOWN_WHILE_COMPILING>
{
public:
	static const T DEFAULT = (T)DEFAULT_INT;

	void reset();
	void allocate();
	bool isAllocated() const;
	~EnumMapVariable();

protected:
	template<int iType>
	void setALLDefault();

	EnumMapVariable();

	void assignmentOperator(const EnumMapVariable<IndexType, T, DEFAULT_INT, LengthType, VARIABLE_TYPE_DYNAMIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>& rhs);

	STORAGE* m_pArray;

	// specialized functions
	// specialized functions can't be placed outside the class declaration unless the class itself is also specialize
	template<>
	void setALLDefault<1>()
	{
		allocate();
		memset(m_pArray, DEFAULT_INT, NUM_ELEMENTS * sizeof(STORAGE));
	}
};


template<class IndexType, class T, int DEFAULT_INT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapShared
	: public EnumMapVariable<IndexType, T, DEFAULT_INT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>
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
	int getNumNonDefaultElements() const;
	int GetNumPositiveElements() const;
	void generateInitCivEffect(const InfoArray<IndexType, int>& kIarray);
protected:
	EnumMapShared() : EnumMapVariable<IndexType, T, DEFAULT_INT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>() {}
};

//
// EnumMapBase
//

template<class IndexType, class T, int DEFAULT_INT, class LengthType, VariableStaticTypes STATIC, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapBase<IndexType, T, DEFAULT_INT, LengthType, STATIC, VARIABLE_TYPE_CHAR, LENGTH_KNOWN_WHILE_COMPILING>
	: public EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, STATIC, char, LENGTH_KNOWN_WHILE_COMPILING>
{
protected:
	EnumMapBase() : EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, STATIC, char, LENGTH_KNOWN_WHILE_COMPILING>() {}
};

template<class IndexType, class T, int DEFAULT_INT, class LengthType, VariableStaticTypes STATIC, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapBase<IndexType, T, DEFAULT_INT, LengthType, STATIC, VARIABLE_TYPE_SHORT, LENGTH_KNOWN_WHILE_COMPILING>
	: public EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, STATIC, short, LENGTH_KNOWN_WHILE_COMPILING>
{
protected:
	EnumMapBase() : EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, STATIC, short, LENGTH_KNOWN_WHILE_COMPILING>() {}
};

template<class IndexType, class T, int DEFAULT_INT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapBase<IndexType, T, DEFAULT_INT, LengthType, VARIABLE_TYPE_STATIC, VARIABLE_TYPE_GENERIC, LENGTH_KNOWN_WHILE_COMPILING>
	: public EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, VARIABLE_TYPE_STATIC, T, LENGTH_KNOWN_WHILE_COMPILING>
{
public:
	// operator overloading
	T& operator[](IndexType eIndex);
	const T& operator[](IndexType eIndex) const;
protected:
	EnumMapBase() : EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, VARIABLE_TYPE_STATIC, T, LENGTH_KNOWN_WHILE_COMPILING>() {}
};

template<class IndexType, class T, int DEFAULT_INT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapBase<IndexType, T, DEFAULT_INT, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_TYPE_GENERIC, LENGTH_KNOWN_WHILE_COMPILING>
	: public EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, VARIABLE_TYPE_DYNAMIC, T, LENGTH_KNOWN_WHILE_COMPILING>
{
public:
	// operator overloading
	T& operator[](IndexType eIndex);
	const T& operator[](IndexType eIndex) const;
protected:
	EnumMapBase() : EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, VARIABLE_TYPE_DYNAMIC, T, LENGTH_KNOWN_WHILE_COMPILING>() {}
};


//
// static
//

template<class IndexType, class T, int DEFAULT_INT, class LengthType, class STORAGE>
void EnumMapVariable<IndexType, T, DEFAULT_INT, LengthType, VARIABLE_TYPE_STATIC, STORAGE, VARIABLE_LENGTH_ALL_KNOWN>::reset()
{
	setALLDefault();
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, class STORAGE>
void EnumMapVariable<IndexType, T, DEFAULT_INT, LengthType, VARIABLE_TYPE_STATIC, STORAGE, VARIABLE_LENGTH_ALL_KNOWN>::allocate()
{
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, class STORAGE>
bool EnumMapVariable<IndexType, T, DEFAULT_INT, LengthType, VARIABLE_TYPE_STATIC, STORAGE, VARIABLE_LENGTH_ALL_KNOWN>::isAllocated() const
{
	return true;
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, class STORAGE>
void EnumMapVariable<IndexType, T, DEFAULT_INT, LengthType, VARIABLE_TYPE_STATIC, STORAGE, VARIABLE_LENGTH_ALL_KNOWN>::setALLDefault()
{
	for (IndexType i = (IndexType)0; i < NUM_ELEMENTS; ++i)
	{
		m_pArray[i] = DEFAULT;
	}
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, class STORAGE>
EnumMapVariable<IndexType, T, DEFAULT_INT, LengthType, VARIABLE_TYPE_STATIC, STORAGE, VARIABLE_LENGTH_ALL_KNOWN>::EnumMapVariable()
{
	setALLDefault();
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, class STORAGE>
void EnumMapVariable<IndexType, T, DEFAULT_INT, LengthType, VARIABLE_TYPE_STATIC, STORAGE, VARIABLE_LENGTH_ALL_KNOWN>::assignmentOperator(const EnumMapVariable<IndexType, T, DEFAULT_INT, LengthType, VARIABLE_TYPE_STATIC, STORAGE, VARIABLE_LENGTH_ALL_KNOWN>& rhs)
{
	for (IndexType i = (Indextype)0; i < NUM_ELEMENTS; ++i)
	{
		m_pArray[i] = rhs.m_pArray[i];
	}
}


//
//  dynamic
//

template<class IndexType, class T, int DEFAULT_INT, class LengthType, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapVariable<IndexType, T, DEFAULT_INT, LengthType, VARIABLE_TYPE_DYNAMIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::reset()
{
	SAFE_DELETE_ARRAY(m_pArray);
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapVariable<IndexType, T, DEFAULT_INT, LengthType, VARIABLE_TYPE_DYNAMIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::allocate()
{
	if (!isAllocated())
	{
		m_pArray = new STORAGE[NUM_ELEMENTS];
		setALLDefault < DEFAULT_INT == 0 || sizeof(STORAGE) == 1 ? 1 : 0>();
	}
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
bool EnumMapVariable<IndexType, T, DEFAULT_INT, LengthType, VARIABLE_TYPE_DYNAMIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::isAllocated() const
{
	return m_pArray != NULL;
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
EnumMapVariable<IndexType, T, DEFAULT_INT, LengthType, VARIABLE_TYPE_DYNAMIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::~EnumMapVariable()
{
	SAFE_DELETE_ARRAY(m_pArray);
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
template<int iType>
void EnumMapVariable<IndexType, T, DEFAULT_INT, LengthType, VARIABLE_TYPE_DYNAMIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::setALLDefault()
{
	allocate();
	for (IndexType i = (IndexType)0; i < NUM_ELEMENTS; ++i)
	{
		m_pArray[i] = DEFAULT;
	}
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
EnumMapVariable<IndexType, T, DEFAULT_INT, LengthType, VARIABLE_TYPE_DYNAMIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::EnumMapVariable()
	: m_pArray(NULL)
{}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapVariable<IndexType, T, DEFAULT_INT, LengthType, VARIABLE_TYPE_DYNAMIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::assignmentOperator(const EnumMapVariable<IndexType, T, DEFAULT_INT, LengthType, VARIABLE_TYPE_DYNAMIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>& rhs)
{
	if (!rhs.isAllocated())
	{
		reset();
		return;
	}
	allocate();
	memcpy(m_pArray, rhs.m_pArray, sizeof(STORAGE)*NUM_ELEMENTS);
}

//
// shared
//

template<class IndexType, class T, int DEFAULT_INT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
T EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::get(IndexType eIndex) const
{
	FAssert(isInRange(eIndex));
	return isAllocated() ? (T)m_pArray[eIndex - FIRST] : DEFAULT;
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::set(IndexType eIndex, T eValue)
{
	FAssert(isInRange(eIndex));
	allocate();
	m_pArray[eIndex - FIRST] = eValue;
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::add(IndexType eIndex, T eValue)
{
	FAssert(isInRange(eIndex));
	allocate();
	m_pArray[eIndex - FIRST] += eValue;
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::keepMin(IndexType eIndex, T eValue)
{
	if (eValue < get(eIndex))
	{
		set(eIndex, eValue);
	}
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::keepMax(IndexType eIndex, T eValue)
{
	if (eValue > get(eIndex))
	{
		set(eIndex, eValue);
	}
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
T EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::getDefault() const
{
	return DEFAULT;
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
bool EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::isDefault(IndexType eIndex) const
{
	return get(eIndex) == DEFAULT;
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
int EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::getTotal() const
{
	if (isAllocated())
	{
		int iTotal = 0;
		for (IndexType i = (IndexType)0; i < NUM_ELEMENTS; ++i)
		{
			iTotal += m_pArray[i];
		}
		return iTotal;
	}
	else
	{
		return DEFAULT_INT * NUM_ELEMENTS;
	}
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::setAll(T eValue)
{
	if (!isAllocated())
	{
		if (eValue == DEFAULT)
		{
			return;
		}
		allocate();
	}
	for (IndexType i = (IndexType)0; i < NUM_ELEMENTS; ++i)
	{
		m_pArray[i] = eValue;
	}
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::addAll(T eValue)
{
	if (eValue == (T)0)
	{
		return;
	}
	allocate();
	for (IndexType i = (IndexType)0; i < NUM_ELEMENTS; ++i)
	{
		m_pArray[i] += eValue;
	}
}
	
template<class IndexType, class T, int DEFAULT_INT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
T EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::getMin() const
{
	if (!isAllocated())
	{
		return DEFAULT;
	}

	T eMin = get((IndexType)0);
	for (IndexType i = (IndexType)0; i < NUM_ELEMENTS; ++i)
	{
		T eLoopValue = m_pArray[i];
		if (eMin > eLoopValue)
		{
			eMin = eLoopValue;
		}
	}
	return eMin;
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
T EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::getMax() const
{
	if (!isAllocated())
	{
		return DEFAULT;
	}

	T eMax = get((IndexType)0);
	for (IndexType i = (IndexType)0; i < NUM_ELEMENTS; ++i)
	{
		T eLoopValue = m_pArray[i];
		if (eMax < eLoopValue)
		{
			eMax = eLoopValue;
		}
	}
	return eMax;
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
IndexType EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::First() const
{
	return VARINFO<LengthType>::start();
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
IndexType EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::getEnd() const
{
	return VARINFO<LengthType>::end();
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
IndexType EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::getLength() const
{
	return VARINFO<LengthType>::length();
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
bool EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::hasContent() const
{
	if (isAllocated())
	{
		for (IndexType i = (IndexType)0; i < NUM_ELEMENTS; ++i)
		{
			if (m_pArray[i] != DEFAULT)
			{
				return true;
			}
		}
	}
	return false;
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
bool EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::hasContent()
{
	if (isAllocated())
	{
		for (IndexType i = (IndexType)0; i < NUM_ELEMENTS; ++i)
		{
			if (m_pArray[i] != DEFAULT)
			{
				return true;
			}
		}
		reset();
	}
	return false;
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::copyToVector(std::vector<T>& thisVector) const
{
	thisVector.reserve(NUM_ELEMENTS);
	thisVector.resize(NUM_ELEMENTS, DEFAULT);

	if (isAllocated())
	{
		for (IndexType eIndex = FIRST; eIndex <= LAST; ++eIndex)
		{
			thisVector[eIndex - FIRST] = get(eIndex);
		}
	}
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::copyFromVector(const std::vector<T>& thisVector)
{
	FAssert((unsigned int)NUM_ELEMENTS == thisVector.size());

	for (IndexType eIndex = FIRST; eIndex <= LAST; ++eIndex)
	{
		set(eIndex, thisVector[eIndex - FIRST]);
	}
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
int EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::getNumNonDefaultElements() const
{
	int iNum = 0;
	for (IndexType eLoop = FIRST; eLoop <= LAST; ++eLoop)
	{
		if (get(eLoop) != DEFAULT)
		{
			++iNum;
		}
	}
	return iNum;
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
int EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::GetNumPositiveElements() const
{
	int iNum = 0;
	for (IndexType eLoop = FIRST; eLoop <= LAST; ++eLoop)
	{
		if (get(eLoop) > 0)
		{
			++iNum;
		}
	}
	return iNum;
}

template<class IndexType, class T, int DEFAULT_INT, class LengthType, VariableStaticTypes STATIC, class STORAGE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapShared<IndexType, T, DEFAULT_INT, LengthType, STATIC, STORAGE, LENGTH_KNOWN_WHILE_COMPILING>::generateInitCivEffect(const InfoArray<IndexType, int>& kIarray)
{
	for (int i = 0; i < kIarray.getLength(); ++i)
	{
		if (kIarray.get1(i) > 0)
		{
			set(kIarray.get0(i), 0);
		}
	}
}

//
// access by reference functions ( only)
//

template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
T& EnumMapBase<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_STATIC, VARIABLE_TYPE_GENERIC, LENGTH_KNOWN_WHILE_COMPILING>::operator[](IndexType eIndex)
{
	FAssert(isInRange(eIndex));
	return m_pArray[eIndex - FIRST];
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
const T& EnumMapBase<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_STATIC, VARIABLE_TYPE_GENERIC, LENGTH_KNOWN_WHILE_COMPILING>::operator[](IndexType eIndex) const
{
	FAssert(isInRange(eIndex));
	return m_pArray[eIndex - FIRST];
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
T& EnumMapBase<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_TYPE_GENERIC, LENGTH_KNOWN_WHILE_COMPILING>::operator[](IndexType eIndex)
{
	FAssert(isInRange(eIndex));
	allocate();
	return m_pArray[eIndex - FIRST];
}

template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
const T& EnumMapBase<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_TYPE_GENERIC, LENGTH_KNOWN_WHILE_COMPILING>::operator[](IndexType eIndex) const
{
	FAssert(isInRange(eIndex));
	if (isAllocated())
	{
		return m_pArray[eIndex - FIRST];
	}
	else
	{
		// array not allocated
		// make an instance of the class in question and return that one
		// it's static anyway, so no harm in using a static instance over and over
		// this avoids allocations and dealing with memory leaks at the memory cost of one T instance per type of EnumMap from this file
		static T empty = (T)DEFAULT;
		return empty;
	}
}

#endif
