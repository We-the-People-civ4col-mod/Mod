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

template<class IndexType, class LengthType, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapCore
{
public:
	static const IndexType FIRST;
	static const IndexType LAST;
	static const IndexType NUM_ELEMENTS;

	bool isInRange(IndexType eIndex) const;

protected:
	// do not let the "outside world" access members, which aren't available in all cases
	static const IndexType COMPILE_NUM_ELEMENTS; // only available when static arrays is an option
};



template<class IndexType, class T, int DEFAULT, class LengthType, VariableStaticTypes STATIC, VariableTypes TYPE, VariableLengthTypes LENGTH_KNOWN_WHILE_COMPILING>
class EnumMapBase
	: public EnumMapCore<IndexType, LengthType, LENGTH_KNOWN_WHILE_COMPILING>
{
	// list of EnumMap member functions
	// note that not all functions are available for all combinations
	// for instance adding 5 to all values makes no sense when T is bool or class
	// using an invalid function will fail at compile time, stating no such member function

	bool isAllocated() const;
	void allocate();
	void reset();
	bool hasContent();
	bool hasContent() const;

	// access elements by value
	// not available when T is a class
	T get(IndexType eIndex) const;
	void set(IndexType eIndex, T bValue);
	void add(IndexType eIndex, T eValue);
	void keepMin(IndexType eIndex, T eValue);
	void keepMax(IndexType eIndex, T eValue);
	int getTotal() const;
	void setAll(T eValue);
	void addAll(T eValue);
	T getMin() const;
	T getMax() const;

	void copyToVector(std::vector<T>& thisVector) const;
	void copyFromVector(const std::vector<T>& thisVector);

	// access elements by reference
	// not available when T is a bool or enum
	T& operator[](IndexType eIndex);
	const T& operator[](IndexType eIndex) const;


	// instances are only allowed to use specialized classes, not this generic one
	// make sure this class can't compile
	BOOST_STATIC_ASSERT(false);
};


template<class IndexType, class T, int DEFAULT = VARINFO<T>::DEFAULT>
class EnumMap : public EnumMapBase <IndexType, T, DEFAULT, IndexType, VARINFO<T>::STATIC<VARINFO<IndexType>::LENGTH>::VAL, VARINFO<T>::TYPE, VARINFO<IndexType>::LENGTH_KNOWN_WHILE_COMPILING>
{
public:
	EnumMap<IndexType, T, DEFAULT>& operator=(const EnumMap<IndexType, T, DEFAULT>& rhs)
	{
		FAssertMsg(this != &rhs, "EnumMap can't assign (=) into itself");
		EnumMapBase<IndexType, T, DEFAULT, IndexType, VARINFO<T>::STATIC<VARINFO<IndexType>::LENGTH>::VAL, VARINFO<T>::TYPE, VARINFO<IndexType>::LENGTH_KNOWN_WHILE_COMPILING>::assignmentOperator(rhs);
		return *this;
	}
};

// define some classes for EnumMaps of EnumMaps
// while technically not needed, it will make it way easier when using one of those as function argument, particularly in case of function overloading
template<typename A, typename B, class T, int DEFAULT = VARINFO<T>::DEFAULT>
class EnumMap2D
	: public EnumMap<A, EnumMap<B, T, DEFAULT> >
{
};

template<typename A, typename B, typename C, class T, int DEFAULT = VARINFO<T>::DEFAULT>
class EnumMap3D
	: public EnumMap<A, EnumMap<B, EnumMap<C, T, DEFAULT> > >
{
};

template<typename A, typename B, typename C, typename D, class T, int DEFAULT = VARINFO<T>::DEFAULT>
class EnumMap4D
	: public EnumMap<A, EnumMap<B, EnumMap<C, EnumMap<D, T, DEFAULT> > > >
{
};

#endif
