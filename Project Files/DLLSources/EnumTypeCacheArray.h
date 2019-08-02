#pragma once

///
//// EnumTypeCacheArray
///
//// The goal of this class is to create an array of enum types for looping.
//// Unlike normal looping, EnumTypeCacheArray is designed to loop a subset and that way increase performance.
/// 
//// Usage:
///
//// get(i): returns the enum type at index i. -1 if out of scope
//// getLength() returns the number of elements in the array
///  loop control: either loop getLength() amounts of time or until get() returns -1.
//// assign(BoolArray or JustInTimeArray): the only way to write contents
///
///
//// Usage example:
///  Goal: only check citizens for yield consumption, which they can consume.
///  Make a BoolArray of length yields and loop CvUnitInfos
///  For each yield, which can be consumed, set the BoolArray to true
///  Make a YieldCacheArray and assign the BoolArray
///  From now on whenever all consumable yields are looped, use the YieldTypeArray
///  This way yields, which aren't consumed by any unit will not be checked, hence reducing loop iterations.
///  

#include "CvEnums.h"

///
//// While EnumTypeCacheArray can be used directly, the code becomes more readable
//// if the it isn't and instead a class from the end of this file is used instead.
///

template<typename T>
class EnumTypeCacheArray : private boost::noncopyable
{
public:
	EnumTypeCacheArray();
	~EnumTypeCacheArray();

	int getLength() const { return m_iLength; }
	T get(int i) const;

	void assign(const BoolArray& pArray);

	template<typename U>
	void assign(const JustInTimeArray<U>& pArray);

private:
	T *m_pTypes;
	int m_iLength;
};

// cpp file for EnumTypeCacheArray.
// It's included as a header file to get around the issue of declaring which templates to compile for.
// By including it in all files, each file can compile for the template(s) needed for that file.
// The alternative would be to maintain a list of all types possible in this file, which would quickly become lengthy.
// 
// Not only is it an issue to set up for each enum type, each has to be set with BoolArray and one JIT array for each template of JIT array.
// Do the math and it becomes 5 lines for each of the current enum types (currently 42) + one setup line just to support the current JIT arrays.
// Add a new JIT array (say unsigned char) and it becomes an additional line for each enum type + one line setup.

template<typename T>
EnumTypeCacheArray<T>::EnumTypeCacheArray()
	: m_pTypes(NULL)
	, m_iLength(0)
{
}

template<typename T>
EnumTypeCacheArray<T>::~EnumTypeCacheArray()
{
	SAFE_DELETE_ARRAY(m_pTypes);
}

template<typename T>
inline T EnumTypeCacheArray<T>::get(int i) const
{
	if (i >= 0 && i < m_iLength)
	{
		return m_pTypes[i];
	}
	return static_cast<T>(-1);
}

template<typename T>
void EnumTypeCacheArray<T>::assign(const BoolArray& kArray)
{
	// getJITarrayType() is an overloaded function, which returns the enum type belonging to T.
	// This means the argument isn't used for anything other than picking which overloaded function to call.
	// Here all it needs is a variable of type T and since there are none ready, typecasting an int will do.
	FAssert(getJITarrayType((T)m_iLength) == kArray.getType());
	SAFE_DELETE_ARRAY(m_pTypes);
	m_iLength = kArray.getNumTrueElements();
	if (m_iLength == 0)
	{
		return;
	}
	m_pTypes = new T[m_iLength];

	int iCounter = 0;
	for (int i = 0; i < kArray.length() && iCounter < m_iLength; ++i)
	{
		if (kArray.get(i))
		{
			m_pTypes[iCounter] = static_cast<T>(i);
			++iCounter;
		}
	}
}

template<typename T>
template<typename U>
void EnumTypeCacheArray<T>::assign(const JustInTimeArray<U>& kArray)
{
	// getJITarrayType() is an overloaded function, which returns the enum type belonging to T.
	// This means the argument isn't used for anything other than picking which overloaded function to call.
	// Here all it needs is a variable of type T and since there are none ready, typecasting an int will do.
	FAssert(getJITarrayType((T)m_iLength) == kArray.getType() || (kArray.getType() - NUM_JITarrayTypes) == getJITarrayType((T)m_iLength));
	// note: JIT array+NUM_JITarrayTypes is a subtype of the same type, like JIT_ARRAY_CARGO_YIELD is a subset of JIT_ARRAY_YIELD
	// Here we don't care if it's a full set or a subset
	SAFE_DELETE_ARRAY(m_pTypes);
	m_iLength = kArray.getPositiveCount();
	if (m_iLength == 0)
	{
		return;
	}
	m_pTypes = new T[m_iLength];

	int iCounter = 0;
	for (int i = 0; i < kArray.length() && iCounter < m_iLength; ++i)
	{
		if (kArray.get(i))
		{
			m_pTypes[iCounter] = static_cast<T>(i);
			++iCounter;
		}
	}
}

////
//// End of cpp file
////

////
//// Declaring classes, which assigns enum types without messing with templates
///  This makes the code more readable, though it doesn't directly add more features
////

class BuildingTypeArray          : public EnumTypeCacheArray<BuildingTypes> {};
class ProfessionTypeArray        : public EnumTypeCacheArray<ProfessionTypes> {};
class PromotionTypeArray         : public EnumTypeCacheArray<PromotionTypes> {};
class YieldTypeArray             : public EnumTypeCacheArray<YieldTypes> {};
