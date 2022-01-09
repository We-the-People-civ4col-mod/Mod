#ifndef ENUM_MAP_H
#define ENUM_MAP_H
#pragma once

#include "CvGlobalConstants.h"


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


template<class IndexType, class T, int DEFAULT, class LengthType, int STATIC = VARINFO<T>::STATIC<VARINFO<LengthType>::LENGTH>::VAL, int TYPE = VARINFO<T>::TYPE>
class EnumMapBase
{
protected:
	// protected constructor does the same as abstract class in C#
	// it means the class can be inherited and used as function arguments, but it can't be allocated directly.
	EnumMapBase()
	{
		BOOST_STATIC_ASSERT(false);
	}
};


template<class IndexType, class T, int DEFAULT = VARINFO<T>::DEFAULT>
class EnumMap : public EnumMapBase <IndexType, T, DEFAULT, IndexType, VARINFO<T>::STATIC<VARINFO<IndexType>::LENGTH>::VAL, VARINFO<T>::TYPE> {};

#endif
