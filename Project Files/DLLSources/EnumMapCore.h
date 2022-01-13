#ifndef ENUM_MAP_FUNCTIONS_CORE_H
#define ENUM_MAP_FUNCTIONS_CORE_H
#pragma once

#include "EnumMap.h"

// declare all 3 cases of EnumMapCore
// This allows partial specialization

template<class IndexType, class LengthType>
class EnumMapCore<IndexType, LengthType, VARIABLE_LENGTH_UNKNOWN>
{
public:
	const IndexType FIRST;
	const IndexType LAST : 16;
	const IndexType NUM_ELEMENTS : 16;

	bool isInRange(IndexType eIndex) const;

	EnumMapCore();
};

template<class IndexType, class LengthType>
class EnumMapCore<IndexType, LengthType, VARIABLE_LENGTH_FIRST_KNOWN>
{
public:
	static const IndexType FIRST = VARINFO<LengthType>::FIRST;
	const IndexType LAST : 16;
	const IndexType NUM_ELEMENTS : 16;

	bool isInRange(IndexType eIndex) const;

	EnumMapCore();
};

template<class IndexType, class LengthType>
class EnumMapCore<IndexType, LengthType, VARIABLE_LENGTH_ALL_KNOWN>
{
public:
	static const IndexType FIRST = VARINFO<LengthType>::FIRST;
	static const IndexType LAST = VARINFO<LengthType>::LAST;
	static const IndexType NUM_ELEMENTS = VARINFO<LengthType>::NUM_ELEMENTS;

	bool isInRange(IndexType eIndex) const;
protected:
	static const IndexType COMPILE_NUM_ELEMENTS = NUM_ELEMENTS;
};

template<class IndexType>
class EnumMapCore<IndexType, CityPlotTypes, VARIABLE_LENGTH_FIRST_KNOWN>
{
public:
	static const IndexType FIRST = static_cast<CityPlotTypes>(0);
	const IndexType LAST : 16;
	const IndexType NUM_ELEMENTS : 16;

	bool isInRange(IndexType eIndex) const;

	EnumMapCore();
protected:
	// reserve 25 elements at compile time as 9 elements fit in 25, but not the other way around
	// only really used for bools, which will reserve 32 anyway (size of int)
	static const IndexType COMPILE_NUM_ELEMENTS = NUM_CITY_PLOTS_2_PLOTS;
};

// constructors
// Ideally we wouldn't want variables in this class. However they are needed in order to provide compile time optimization for case 2.
// This could be avoided by using constexpr, which was introduced in C++11. Yeah we will waste a bit of memory due to our ancient compiler.
// Having said that, caching the range of the EnumMap next to the pointer isn't awful from a performance perspective.
template<class IndexType, class LengthType>
EnumMapCore<IndexType, LengthType, VARIABLE_LENGTH_UNKNOWN>::EnumMapCore()
	: FIRST(VARINFO<LengthType>::first())
	, LAST(VARINFO<LengthType>::last())
	, NUM_ELEMENTS(VARINFO<LengthType>::numElements())
{
}

template<class IndexType, class LengthType>
EnumMapCore<IndexType, LengthType, VARIABLE_LENGTH_FIRST_KNOWN>::EnumMapCore()
	: LAST(VARINFO<LengthType>::last())
	, NUM_ELEMENTS(VARINFO<LengthType>::numElements())
{
}

template<class IndexType>
EnumMapCore<IndexType, CityPlotTypes, VARIABLE_LENGTH_FIRST_KNOWN>::EnumMapCore()
	: LAST(LAST_CITY_PLOT)
	, NUM_ELEMENTS(NUM_CITY_PLOTS)
{
	FAssert(LAST_CITY_PLOT != FIRST_CITY_PLOT);
}

template<class IndexType, class LengthType>
bool EnumMapCore<IndexType, LengthType, VARIABLE_LENGTH_UNKNOWN>::isInRange(IndexType eIndex) const
{
	return eIndex >= FIRST && eIndex <= LAST;
}

template<class IndexType, class LengthType>
bool EnumMapCore<IndexType, LengthType, VARIABLE_LENGTH_FIRST_KNOWN>::isInRange(IndexType eIndex) const
{
	return eIndex >= FIRST && eIndex <= LAST;
}

template<class IndexType, class LengthType>
bool EnumMapCore<IndexType, LengthType, VARIABLE_LENGTH_ALL_KNOWN>::isInRange(IndexType eIndex) const
{
	return eIndex >= FIRST && eIndex <= LAST;
}

template<class IndexType>
bool EnumMapCore<IndexType, CityPlotTypes, VARIABLE_LENGTH_FIRST_KNOWN>::isInRange(IndexType eIndex) const
{
	return eIndex >= FIRST && eIndex <= LAST;
}

#endif
