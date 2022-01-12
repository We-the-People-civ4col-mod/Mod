#ifndef ENUM_MAP_FUNCTIONS_CORE_H
#define ENUM_MAP_FUNCTIONS_CORE_H
#pragma once

#include "EnumMap.h"

// declare all 3 cases of EnumMapCore
// This allows partial specialization

template<class IndexType, class LengthType>
class EnumMapCore<IndexType, LengthType, 0>
{
public:
	const IndexType FIRST;
	const IndexType LAST : 16;
	const IndexType NUM_ELEMENTS : 16;

	bool isInRange(IndexType eIndex) const;

	EnumMapCore();
};

template<class IndexType, class LengthType>
class EnumMapCore<IndexType, LengthType, 1>
{
public:
	static const IndexType FIRST = VARINFO<LengthType>::FIRST;
	const IndexType LAST : 16;
	const IndexType NUM_ELEMENTS : 16;

	bool isInRange(IndexType eIndex) const;

	EnumMapCore();
};

template<class IndexType, class LengthType>
class EnumMapCore<IndexType, LengthType, 2>
{
public:
	static const IndexType FIRST = VARINFO<LengthType>::FIRST;
	static const IndexType LAST = VARINFO<LengthType>::LAST;
	static const IndexType NUM_ELEMENTS = VARINFO<LengthType>::NUM_ELEMENTS;

	bool isInRange(IndexType eIndex) const;
};

// constructors
// Ideally we wouldn't want variables in this class. However they are needed in order to provide compile time optimization for case 2.
// This could be avoided by using constexpr, which was introduced in C++11. Yeah we will waste a bit of memory due to our ancient compiler.
// Having said that, caching the range of the EnumMap next to the pointer isn't awful from a performance perspective.
template<class IndexType, class LengthType>
EnumMapCore<IndexType, LengthType, 0>::EnumMapCore()
	: FIRST(VARINFO<LengthType>::first())
	, LAST(VARINFO<LengthType>::last())
	, NUM_ELEMENTS(VARINFO<LengthType>::numElements())
{
}

template<class IndexType, class LengthType>
EnumMapCore<IndexType, LengthType, 1>::EnumMapCore()
	: LAST(VARINFO<LengthType>::last())
	, NUM_ELEMENTS(VARINFO<LengthType>::numElements())
{
}

template<class IndexType, class LengthType>
bool EnumMapCore<IndexType, LengthType, 0>::isInRange(IndexType eIndex) const
{
	return eIndex >= first() && eIndex <= last();
}

template<class IndexType, class LengthType>
bool EnumMapCore<IndexType, LengthType, 1>::isInRange(IndexType eIndex) const
{
	return eIndex >= FIRST && eIndex <= last();
}

template<class IndexType, class LengthType>
bool EnumMapCore<IndexType, LengthType, 2>::isInRange(IndexType eIndex) const
{
	return eIndex >= FIRST && eIndex <= LAST;
}

#endif
