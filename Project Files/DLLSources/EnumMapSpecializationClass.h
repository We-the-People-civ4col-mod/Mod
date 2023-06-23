#ifndef ENUM_MAP_FUNCTIONS_GENERIC_H
#define ENUM_MAP_FUNCTIONS_GENERIC_H
#pragma once

#include "EnumMap.h"

//
// EnumMap of classes
//
// Works like any other EnumMap, except it contains an array of class instances.
// For this reason a number of the normal functions aren't supported, most noteworthy get and set.
// Instead it makes use of the array index operator [], which returns a reference.
//
// Usage:
// em[index].
// This will grant full access to the inner class. If said class supports [] too, then it's em[][]
// There is support for EnumMaps of EnumMaps of EnumMaps of..... No hard limit, but such a massive data structure could indicate a poor code design.
//
// For savegames:
// Can save as long as there is savegame code for the inner class and said class has hasContent()
// hasContent() tells if a class instance should be saved or not, hence the fallback of always returning true should work.
// It is however preferred to have code to check if the instance has non-default data as that will reduce savegame size.




template<class IndexType, class T, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapBase<IndexType, T, 0, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_TYPE_CLASS, LENGTH_KNOWN_WHILE_COMPILING>
	: public EnumMapCore<IndexType, LengthType, LENGTH_KNOWN_WHILE_COMPILING>
{
public:
	EnumMapBase();
	~EnumMapBase();

	bool isAllocated() const;
	void allocate();
	void reset();

	bool hasContent();
	bool hasContent() const;

	int getNumTrueElements() const;

	T& getFast(IndexType eIndex);
	const T& getFast(IndexType eIndex) const;

	// operator overloading
	T& operator[](IndexType eIndex);
	const T& operator[](IndexType eIndex) const;

protected:
	std::vector<T> m_Array;

	void assignmentOperator(const EnumMapBase<IndexType, T, 0, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_TYPE_CLASS, LENGTH_KNOWN_WHILE_COMPILING>& rhs);
};

template<class IndexType, class T, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
EnumMapBase<IndexType, T, 0, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_TYPE_CLASS, LENGTH_KNOWN_WHILE_COMPILING>::EnumMapBase()
{
	BOOST_STATIC_ASSERT(VARINFO<T>::IS_CLASS == 1);
}

template<class IndexType, class T, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
EnumMapBase<IndexType, T, 0, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_TYPE_CLASS, LENGTH_KNOWN_WHILE_COMPILING>::~EnumMapBase()
{
}

template<class IndexType, class T, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
bool EnumMapBase<IndexType, T, 0, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_TYPE_CLASS, LENGTH_KNOWN_WHILE_COMPILING>::isAllocated() const
{
	return m_Array.size() > 0;
}

template<class IndexType, class T, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapBase<IndexType, T, 0, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_TYPE_CLASS, LENGTH_KNOWN_WHILE_COMPILING>::allocate()
{
#ifndef COMPILE_STATIC_TEST
	// resize will fail during static test as CvInfoBase intentionally has a private copy constructor
	if (!isAllocated())
	{
		m_Array.reserve(NUM_ELEMENTS);
		m_Array.resize(NUM_ELEMENTS);
	}
#endif
}

template<class IndexType, class T, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void EnumMapBase<IndexType, T, 0, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_TYPE_CLASS, LENGTH_KNOWN_WHILE_COMPILING>::reset()
{
	m_Array.resize(0);
}

template<class IndexType, class T, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
bool EnumMapBase<IndexType, T, 0, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_TYPE_CLASS, LENGTH_KNOWN_WHILE_COMPILING>::hasContent()
{
	if (isAllocated())
	{
		for (LengthType i = (LengthType)0; i < NUM_ELEMENTS; ++i)
		{
			if (m_Array[i].hasContent())
			{
				return true;
			}
		}
	}
	return false;
}

template<class IndexType, class T, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
bool EnumMapBase<IndexType, T, 0, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_TYPE_CLASS, LENGTH_KNOWN_WHILE_COMPILING>::hasContent() const
{
	if (isAllocated())
	{
		for (LengthType i = (LengthType)0; i < NUM_ELEMENTS; ++i)
		{
			if (m_Array[i].hasContent())
			{
				return true;
			}
		}
	}
	return false;
}

template<class IndexType, class T, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
int EnumMapBase<IndexType, T, 0, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_TYPE_CLASS, LENGTH_KNOWN_WHILE_COMPILING>::getNumTrueElements() const
{
	if (!isAllocated())
	{
		// assume no content if the array isn't allocated
		return 0;
	}

	int iCount = 0;
	for (LengthType i = (LengthType)0; i < NUM_ELEMENTS; ++i)
	{
		iCount += m_pArray[i].getNumTrueElements();
	}
	return iCount;
}

template<class IndexType, class T, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
T& EnumMapBase<IndexType, T, 0, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_TYPE_CLASS, LENGTH_KNOWN_WHILE_COMPILING>::getFast(IndexType eIndex)
{
	FAssert(isInRange(eIndex));
	FAssert(isAllocated());
	return m_Array[eIndex - FIRST];
}

template<class IndexType, class T, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
const T& EnumMapBase<IndexType, T, 0, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_TYPE_CLASS, LENGTH_KNOWN_WHILE_COMPILING>::getFast(IndexType eIndex) const
{
	FAssert(isInRange(eIndex));
	FAssert(isAllocated());
	return m_Array[eIndex - FIRST];
}

template<class IndexType, class T, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
T& EnumMapBase<IndexType, T, 0, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_TYPE_CLASS, LENGTH_KNOWN_WHILE_COMPILING>::operator[](IndexType eIndex)
{
	FAssert(isInRange(eIndex));
	allocate();
	return m_Array[eIndex - FIRST];
}

template<class IndexType, class T, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
const T& EnumMapBase<IndexType, T, 0, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_TYPE_CLASS, LENGTH_KNOWN_WHILE_COMPILING>::operator[](IndexType eIndex) const
{
	FAssert(isInRange(eIndex));
	if (isAllocated())
	{
		return m_Array[eIndex - FIRST];
	}
	else
	{
		// array not allocated
		// make an instance of the class in question and return that one
		// it's static anyway, so no harm in using a static instance over and over
		// this avoids allocations and dealing with memory leaks at the memory cost of one T instance per type of EnumMap from this file
		static T empty;
		return empty;
	}
}


template<class IndexType, class T, int DEFAULT, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
void assignmentOperator(const EnumMapBase<IndexType, T, DEFAULT, LengthType, VARIABLE_TYPE_DYNAMIC, VARIABLE_TYPE_CLASS, LENGTH_KNOWN_WHILE_COMPILING>& rhs)
{
	if (!rhs.isAllocated())
	{
		reset();
		return;
	}
	allocate();
	for (int i = 0; i < NUM_ELEMENTS; ++i)
	{
		m_pArray[i] = rhs.m_pArray[i];
	}

}

#endif
