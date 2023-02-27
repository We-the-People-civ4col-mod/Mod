//
// InfoArray
//
// It's a relative of JustInTime arrays though it's aimed at storing xml data in info classes.
//
// It's designed to store static data as in write once and then it becomes read only. Writing is mainly directly from xml.
// 
// read(tagname) is all it takes to read from xml, even if the tag is a list of lists.
// Well developed error detection and asserting when reading xml data.
// If xml contains something like Unit instead of UnitClass, it will tell you which file, what's wrong and what it expects.
//
// Rather than storing an entire array, it stores only non-zero elements. This makes it ideal for near empty arrays in xml.
// Examples could be free promotions, which would most often be empty, but if in use would often contain 1-3 elements.
// In this case it would store an array of PromotionTypes to enable.
//
// While it saves memory, the biggest benefit is that it allows looping of non-zero values only.
// For instance if an array contains a single promotion, it can be looped with one iteration instead of the number of promotions.
//
// The underlying structure is an array of "blocks". Each block is an array of 1-4 shorts (16 bit signed int)
//    First short is "the index" meaning that would be the index if it were a normal array.
//
//    Example:
//       We want to store that we increase production of hammers by 25% and crosses by 20%
//       get(0)   would provide YIELD_HAMMERS
//       get(0,1) would provide 25 as it is the value connected with YIELD_HAMMERS
//       get(1)   would provide YIELD_CROSSES
//       get(1,1) would provide 20
//       get(2)   would provide -1 as it is out of bounds
//
//  Looping an InfoArray can be done in two ways.
//     1: regular using getLength()
//     2: just increasing the argument for get() until it returns -1 as this mean it ended up outside the array
//
//  The second option has the benefit of the reading code not caring for length while looping.
//
// Written to work together with BoolArray, JustInTimeArray, JustInTimeArray2D and CacheArray2D.
//   addCache in those (add in BoolArray) can be used in those to make a list of combined values of multiple InfoArrays
// 
//
//  The class is split into two classes:
//    InfoArray: data and read functions
//    InfoArrayMod: functions to modify the content
//
//  The idea with the split is that the functions are declared as InfoArrayMod in whatever class they belong to.
//  This class will then have access to all the functions and can assign data as needed.
//  To provide outside access to the data, it can have an access function, which returns a const InfoArray pointer.
//  Because it is InfoArray and not InfoArrayMod, it will only have read functions.
//  Private can do this too, but this way the write functions aren't even in the IDE proposed list of functions.
//

#pragma once

#ifndef INFO_ARRAY_H
#define INFO_ARRAY_H

#ifndef FASSERT_ENABLE
// remove the type argument if it isn't used by asserts anyway
// this will make the compiler ignore getType() calls and hence reduce code size/complexity
#define getWithType(a, b, c) getWithType(b, c)
#endif

#include "CvEnums.h"

class CvXMLLoadUtility;
class CvCivilizationInfo;

class InfoArrayBase
{
	// make the python interface a friend as it needs to bypass the type check
	friend class CyInfoArray;
	// declare InfoArrayMod to be friend
	// this allows any InfoArrayMod to access getInternal directly, bypassing the type checks
	// this gives more coding freedom, but getType should be compared in asserts if done so to preserve the strict type checks
	friend class InfoArrayMod;
	template<int, typename>
	friend class InfoArrayToken;

protected:
	InfoArrayBase() {};

public:
	InfoArrayBase(JITarrayTypes eType0, JITarrayTypes eType1, JITarrayTypes eType2, JITarrayTypes eType3);
	~InfoArrayBase();

	int getIndex(int iIndex) const; // index as in JIT array - much slower than get() or a real JIT array
	int getIndex(int iIndex, int iSubIndex) const;
	int getLength() const;
	int getDimentions() const;
	bool isBool() const; // dimentions and types are the same 

	int getMin(int iDimention) const;
	int getMax(int iDimention) const;
	bool areIndexesValid(int iMax, int iDimention = 0, int iMin = 0) const;

	InfoArrayBase& operator=(const InfoArrayBase &rhs);

	JITarrayTypes getType(int i) const
	{
		FAssert(i >= 0 && i < 4);
		return (JITarrayTypes)((m_aiTypes >> (8 * i)) & 0xFF);
	}

	int getWithType(JITarrayTypes eType, int iIndex, int iTokenIndex) const;

	// adds UnitClass->Unit and BuildingClass->Building conversion. Otherwise the same as without the civinfo argument 
	int getWithTypeWithConversion(JITarrayTypes eType, int iIndex, int iTokenIndex, const CvCivilizationInfo *pCivInfo) const;

protected:
	// needed by python interface. Do not use this in the DLL as it lacks type checking
	int pyGet(int iIndex, int iSubIndex) const {return getInternal(iIndex, iSubIndex);}

	int getInternal(int iIndex, int iTokenIndex = 0) const;
	int _getIndexOf(int iValue, int iDim) const;

	void _setLength(int iLength);
	bool _setValue(int iValue0);
	bool _setValue(int iValue0, int iValue1);

	short m_iLength;
	char m_iNumDimentions : 7;
	bool m_bStatic : 1;
	union
	{
		short m_aiStaticArray[2];
		short* m_pArray;
	};
	union
	{
		struct
		{
			JITarrayTypes m_eType0 : 8;
			JITarrayTypes m_eType1 : 8;
			JITarrayTypes m_eType2 : 8;
			JITarrayTypes m_eType3 : 8;
		};
		unsigned int m_aiTypes;
	};

public:
	// bReadFloat will multiply the read value by 100 and store the int value of that
	// It doesn't provide extra functionality other than allowing floats to be written in XML for human readability

	// szType is always getType()
	// sTagName is the root xml tax name of the structure to be read
	void read(CvXMLLoadUtility* pXML, const char* szType, const char *sTagName);
private:
	void readRecursive(CvXMLLoadUtility* pXML, int& iIndex, std::vector<short>& aArray, std::vector<short>& aIndex, int iLevel, const char *sTagName, const char* szType);
};


enum JIT_NoneTypes {};


//
// Here comes a complex setup, which is based on a fairly simple idea.
// InfoArray inherits InfoArrayX (X being 1-4) and InfoArrayXOnly.
// This mean an InfoArray with two non-default parameters will have InfoArray1, InfoArray2 and InfoArray2Only.
// The extra class names allows partial specialization even in cases where the C++ standard doesn't support partial specialization.
// All classes allows specialization meaning we can add code for say interaction between UnitClassTypes and UnitTypes.
// Defining all 12 classes is however a bit lengthy (4 InfoArrayX, 4 InfoArrayXOnly, 4 InfoArraySelector)
//


template<typename T0>
class InfoArray1 {};
template<typename T0, typename T1>
class InfoArray2 {};
template<typename T0, typename T1, typename T2>
class InfoArray3 {};
template<typename T0, typename T1, typename T2, typename T3>
class InfoArray4 {};

// the only classes. Each instance of InfoArray will have precisely one of those
template<typename T0>
class InfoArray1Only : public InfoArray1<T0>
{
protected:
	InfoArray1Only(JITarrayTypes eType0, JITarrayTypes eType1, JITarrayTypes eType2, JITarrayTypes eType3)
		: InfoArray1<T0>(eType0, eType1, eType2, eType3) {}
public:
	template<typename Ta, class T, int DEFAULT>
	void assignFrom(const EnumMap<Ta, T, DEFAULT>& em);
	template<typename Ta, int DEFAULT>
	void assignFrom(const EnumMap<Ta, bool, DEFAULT>& em);
	template<typename Ta, class T, int DEFAULT>
	void addTo(EnumMap<Ta, T, DEFAULT> & em, int iChange = 1) const;
	template<typename Ta, class T, int DEFAULT>
	void addTo(EnumMap<Ta, T, DEFAULT> & em, int iChange, const CvCivilizationInfo* pCivInfo) const;
	template<typename Ta, int DEFAULT>
	void addTo(EnumMap<Ta, bool, DEFAULT> & em, int iChange, const CvCivilizationInfo* pCivInfo) const;
	template<typename Ta, class T, int DEFAULT>
	void copyTo(EnumMap<Ta, T, DEFAULT> & em) const;
};
template<typename T0, typename T1>
class InfoArray2Only : public InfoArray2<T0, T1>
{
protected:
	InfoArray2Only(JITarrayTypes eType0, JITarrayTypes eType1, JITarrayTypes eType2, JITarrayTypes eType3)
		: InfoArray2<T0, T1>(eType0, eType1, eType2, eType3) {}
public:
	template<typename Ta, class T, int DEFAULT>
	void assignFrom(const EnumMap<Ta, T, DEFAULT>& em);
	template<typename Ta, typename Tb, int DEFAULT>
	void assignFrom(const EnumMap2D<Ta, Tb, bool, DEFAULT> & em);
	template<typename Ta, class T, int DEFAULT>
	void addTo(EnumMap<Ta, T, DEFAULT> & em, int iChange = 1) const;
	template<typename Ta, class T, int DEFAULT>
	void addTo(EnumMap<Ta, T, DEFAULT> & em, int iChange, const CvCivilizationInfo* pCivInfo) const;
	template<typename Ta, int DEFAULT>
	void addTo(EnumMap<Ta, bool, DEFAULT> & em, int iChange, const CvCivilizationInfo* pCivInfo) const;
	template<typename Ta, class T, int DEFAULT>
	bool addCache(EnumMap<Ta, T, DEFAULT> & em, int iChange, const CvCivilizationInfo* pCivInfo) const;
	template<typename class T, int DEFAULT>
	bool addCache(EnumMap<BuildingTypes, T, DEFAULT> & em, int iChange, const CvCivilizationInfo* pCivInfo) const;
	template<typename class T, int DEFAULT>
	bool addCache(EnumMap<UnitTypes, T, DEFAULT> & em, int iChange, const CvCivilizationInfo* pCivInfo) const;
	template<typename Ta, class T, int DEFAULT>
	void copyTo(EnumMap<Ta, T, DEFAULT> & em) const;
	template<typename Ta, int DEFAULT>
	void copyTo(EnumMap<Ta, bool, DEFAULT> & em) const;
	template<typename Ta, typename Tb, typename T, int DEFAULT>
	void addTo(EnumMap2D<Ta, Tb, T, DEFAULT> & em, int iChange = 1) const;
	template<typename Ta, typename Tb, typename T, int DEFAULT>
	void addTo(EnumMap2D<Ta, Tb, T, DEFAULT> & em, int iChange, const CvCivilizationInfo* pCivInfo) const;
};
template<typename T0, typename T1, typename T2>
class InfoArray3Only : public InfoArray3<T0, T1, T2>
{
protected:
	InfoArray3Only(JITarrayTypes eType0, JITarrayTypes eType1, JITarrayTypes eType2, JITarrayTypes eType3)
		: InfoArray3<T0, T1, T2>(eType0, eType1, eType2, eType3) {}
public:
	template<typename Ta, typename Tb, typename T, int DEFAULT>
	void addTo(EnumMap2D<Ta, Tb, T, DEFAULT> & em, int iChange = 1) const;
	template<typename Ta, typename Tb, typename T, int DEFAULT>
	void addTo(EnumMap2D<Ta, Tb, T, DEFAULT> & em, int iChange, const CvCivilizationInfo* pCivInfo) const;
	template<typename Ta, typename Tb, int DEFAULT>
	void addTo(EnumMap2D<Ta, Tb, bool, DEFAULT> & em, int iChange, const CvCivilizationInfo* pCivInfo) const;
};
template<typename T0, typename T1, typename T2, typename T3>
class InfoArray4Only : public InfoArray4<T0, T1, T2, T3>
{
protected:
	InfoArray4Only(JITarrayTypes eType0, JITarrayTypes eType1, JITarrayTypes eType2, JITarrayTypes eType3)
		: InfoArray4<T0, T1, T2, T3>(eType0, eType1, eType2, eType3) {}
};

template<typename T0>
template<typename Ta, class T, int DEFAULT>
void InfoArray1Only<T0>::assignFrom(const EnumMap<Ta, T, DEFAULT> & em)
{
	const int iLength = em.GetNumPositiveElements();
	_setLength(iLength);
	for (T0 eIndex = em.FIRST; eIndex <= em.LAST; ++eIndex)
	{
		if (em.get(eIndex) > 0)
		{
			bool bDone = _setValue(eIndex);
#ifndef FASSERT_ENABLE
			if (bDone) return;
#endif
		}
	}
}

template<typename T0>
template<typename Ta, int DEFAULT>
void InfoArray1Only<T0>::assignFrom(const EnumMap<Ta, bool, DEFAULT> & em)
{
	const int iLength = em.getNumTrueElements();
	_setLength(iLength);
	for (T0 eIndex = em.FIRST; eIndex <= em.LAST; ++eIndex)
	{
		if (em.get(eIndex))
		{
			bool bDone = _setValue(eIndex);
#ifndef FASSERT_ENABLE
			if (bDone) return;
#endif
		}
	}
}


template<typename T0, typename T1>
template<typename Ta, class T, int DEFAULT>
void InfoArray2Only<T0, T1>::assignFrom(const EnumMap<Ta, T, DEFAULT> & em)
{
	BOOST_STATIC_ASSERT((!boost::is_same<T, bool>::value));
	const int iLength = em.getNumNonDefaultElements();
	_setLength(iLength);
	for (T0 eIndex = em.FIRST; eIndex <= em.LAST; ++eIndex)
	{
		int iVal = em.get(eIndex);
		if (iVal != em.DEFAULT)
		{
			bool bDone = _setValue(eIndex, iVal);
#ifndef FASSERT_ENABLE
			if (bDone) return;
#endif
		}
	}
}

template<typename T0, typename T1>
template<typename Ta, typename Tb, int DEFAULT>
void InfoArray2Only<T0, T1>::assignFrom(const EnumMap2D<Ta, Tb, bool, DEFAULT> & em)
{
	const int iLength = em.getNumTrueElements();
	_setLength(iLength);
	for (T0 eIndex0 = em.FIRST; eIndex0 <= em.LAST; ++eIndex0)
	{
		const EnumMap<Tb, bool, DEFAULT>& em1 = em[eIndex0];
		for (Tb eIndex1 = em1.FIRST; eIndex1 <= em1.LAST; ++eIndex1)
		{
			if (em1.get(eIndex1))
			{
				bool bDone = _setValue(eIndex0, eIndex1);
#ifndef FASSERT_ENABLE
				if (bDone) return;
#endif
			}
		}
	}
}

template<typename T0>
template<typename Ta, class T, int DEFAULT>
void InfoArray1Only<T0>::addTo(EnumMap<Ta, T, DEFAULT> & em, int iChange) const
{
	addTo(em, iChange, NULL);
}

template<typename T0>
template<typename Ta, class T, int DEFAULT>
void InfoArray1Only<T0>::addTo(EnumMap<Ta, T, DEFAULT> & em, int iChange, const CvCivilizationInfo* pCivInfo) const
{
	BOOST_STATIC_ASSERT((!boost::is_same<T, bool>::value));
	for (int i = 0; i < getLength(); ++i)
	{
		const T0 eIndex = pCivInfo->getCivSpecificForClass<Ta, T0>(get0(i));
		if (em.isInRange(eIndex))
		{
			em.add(eIndex, iChange);
		}
	}
}

template<typename T0>
template<typename Ta, int DEFAULT>
void InfoArray1Only<T0>::addTo(EnumMap<Ta, bool, DEFAULT> & em, int iChange, const CvCivilizationInfo* pCivInfo) const
{
	for (int i = 0; i < getLength(); ++i)
	{
		const T0 eIndex = pCivInfo->getCivSpecificForClass<Ta, T0>(get0(i));
		if (em.isInRange(eIndex))
		{
			em.set(eIndex, iChange);
		}
	}
}

template<typename T0, typename T1>
template<typename Ta, class T, int DEFAULT>
void InfoArray2Only<T0, T1>::addTo(EnumMap<Ta, T, DEFAULT> & em, int iChange) const
{
	addTo(em, iChange, NULL);
}

template<typename T0, typename T1>
template<typename Ta, class T, int DEFAULT>
void InfoArray2Only<T0, T1>::addTo(EnumMap<Ta, T, DEFAULT> & em, int iChange, const CvCivilizationInfo* pCivInfo) const
{
	BOOST_STATIC_ASSERT((!boost::is_same<T, bool>::value));
	for (int i = 0; i < getLength(); ++i)
	{
		const T0 eIndex = pCivInfo->getCivSpecificForClass<Ta, T0>(get0(i));
		if (em.isInRange(eIndex))
		{
			em.add(eIndex, get1(i) * iChange);
		}
	}
}

template<typename T0, typename T1>
template<typename Ta, int DEFAULT>
void InfoArray2Only<T0, T1>::addTo(EnumMap<Ta, bool, DEFAULT> & em, int iChange, const CvCivilizationInfo* pCivInfo) const
{
	for (int i = 0; i < getLength(); ++i)
	{
		const T0 eIndex = pCivInfo->getCivSpecificForClass<Ta, T0>(get0(i));
		if (em.isInRange(eIndex))
		{
			em.set(eIndex, (get1(i) * iChange) > 0);
		}
	}
}

template<typename T0, typename T1>
template<typename Ta, class T, int DEFAULT>
bool InfoArray2Only<T0, T1>::addCache(EnumMap<Ta, T, DEFAULT> & em, int iChange, const CvCivilizationInfo* pCivInfo) const
{
	BOOST_STATIC_ASSERT((!boost::is_same<T, bool>::value));
	bool bChanged = false;
	for (int i = 0; i < getLength(); ++i)
	{
		const T0 eIndex = get0(i);
		if (em.isInRange(eIndex))
		{
			bool bBefore = get1(i) > 0;
			em.add(eIndex, get1(i) * iChange);
			bool bAfter = get1(i) > 0;
			bChanged |= bBefore != bAfter;
		}
	}
	return bChanged;
}

template<typename T0, typename T1>
template<class T, int DEFAULT>
bool InfoArray2Only<T0, T1>::addCache(EnumMap<BuildingTypes, T, DEFAULT> & em, int iChange, const CvCivilizationInfo* pCivInfo) const
{
	BOOST_STATIC_ASSERT((!boost::is_same<T, bool>::value));
	BOOST_STATIC_ASSERT((boost::is_same<T0, BuildingClassTypes>::value));
	bool bChanged = false;
	for (int i = 0; i < getLength(); ++i)
	{
		const BuildingTypes eIndex = pCivInfo->getCivSpecificForClass<BuildingTypes, T0>(get0(i));
		if (em.isInRange(eIndex))
		{
			bool bBefore = get1(i) > 0;
			em.add(eIndex, get1(i) * iChange);
			bool bAfter = get1(i) > 0;
			bChanged |= bBefore != bAfter;
		}
	}
	return bChanged;
}

template<typename T0, typename T1>
template<class T, int DEFAULT>
bool InfoArray2Only<T0, T1>::addCache(EnumMap<UnitTypes, T, DEFAULT> & em, int iChange, const CvCivilizationInfo* pCivInfo) const
{
	BOOST_STATIC_ASSERT((!boost::is_same<T, bool>::value));
	BOOST_STATIC_ASSERT((boost::is_same<T0, UnitClassTypes>::value));
	bool bChanged = false;
	for (int i = 0; i < getLength(); ++i)
	{
		const UnitTypes eIndex = pCivInfo->getCivSpecificForClass<UnitTypes, T0>(get0(i));
		if (em.isInRange(eIndex))
		{
			bool bBefore = get1(i) > 0;
			em.add(eIndex, get1(i) * iChange);
			bool bAfter = get1(i) > 0;
			bChanged |= bBefore != bAfter;
		}
	}
	return bChanged;
}

template<typename T0>
template<typename Ta, class T, int DEFAULT>
void InfoArray1Only<T0>::copyTo(EnumMap<Ta, T, DEFAULT> & em) const
{
	em.reset();
	for (int i = 0; i < getLength(); ++i)
	{
		em.set(get0(i), 1);
	}
}

template<typename T0, typename T1>
template<typename Ta, class T, int DEFAULT>
void InfoArray2Only<T0, T1>::copyTo(EnumMap<Ta, T, DEFAULT> & em) const
{
	em.reset();
	for (int i = 0; i < getLength(); ++i)
	{
		const T0 eIndex = get0(i);
		if (em.isInRange(eIndex))
		{
			em.set(eIndex, get1(i));
		}
	}
}

template<typename T0, typename T1>
template<typename Ta, int DEFAULT>
void InfoArray2Only<T0, T1>::copyTo(EnumMap<Ta, bool, DEFAULT> & em) const
{
	em.reset();
	for (int i = 0; i < getLength(); ++i)
	{
		const T0 eIndex = get0(i);
		if (em.isInRange(eIndex))
		{
			em.set(eIndex, get1(i) > (T0)0);
		}
	}
}

template<typename T0, typename T1>
template<typename Ta, typename Tb, typename T, int DEFAULT>
void InfoArray2Only<T0, T1>::addTo(EnumMap2D<Ta, Tb, T, DEFAULT> & em, int iChange) const
{
	addTo(em, iChange, NULL);
}

template<typename T0, typename T1>
template<typename Ta, typename Tb, typename T, int DEFAULT>
void InfoArray2Only<T0, T1>::addTo(EnumMap2D<Ta, Tb, T, DEFAULT> & em, int iChange, const CvCivilizationInfo* pCivInfo) const
{
	BOOST_STATIC_ASSERT((!boost::is_same<Tb, bool>::value));
	for (int i = 0; i < getLength(); ++i)
	{
		const Ta eIndex0 = pCivInfo->getCivSpecificForClass<Ta, T0>(get0(i));
		if (em.isInRange(eIndex0))
		{
			if (em[eIndex0].isInRange(pCivInfo->getCivSpecificForClass<Tb, T1>(get1(i))))
			{
				em[eIndex0].add(pCivInfo->getCivSpecificForClass<Tb, T1>(get1(i)), iChange);
			}
		}
	}
}

template<typename T0, typename T1, typename T2>
template<typename Ta, typename Tb, typename T, int DEFAULT>
void InfoArray3Only<T0, T1, T2>::addTo(EnumMap2D<Ta, Tb, T, DEFAULT> & em, int iChange) const
{
	addTo(em, iChange, NULL);
}

template<typename T0, typename T1, typename T2>
template<typename Ta, typename Tb, typename T, int DEFAULT>
void InfoArray3Only<T0, T1, T2>::addTo(EnumMap2D<Ta, Tb, T, DEFAULT> & em, int iChange, const CvCivilizationInfo* pCivInfo) const
{
	BOOST_STATIC_ASSERT((!boost::is_same<T, bool>::value));
	for (int i = 0; i < getLength(); ++i)
	{
		const Ta eIndex0 = pCivInfo->getCivSpecificForClass<Ta, T0>(get0(i));
		if (em.isInRange(eIndex0))
		{
			if (em[eIndex0].isInRange(pCivInfo->getCivSpecificForClass<Tb, T1>(get1(i))))
			{
				em[eIndex0].add(pCivInfo->getCivSpecificForClass<Tb, T1>(get1(i)), get2(i) * iChange);
			}
		}
	}
}

template<typename T0, typename T1, typename T2>
template<typename Ta, typename Tb, int DEFAULT>
void InfoArray3Only<T0, T1, T2>::addTo(EnumMap2D<Ta, Tb, bool, DEFAULT> & em, int iChange, const CvCivilizationInfo* pCivInfo) const
{
	for (int i = 0; i < getLength(); ++i)
	{
		const Ta eIndex0 = pCivInfo->getCivSpecificForClass<Ta, T0>(get0(i));
		if (em.isInRange(eIndex0))
		{
			if (em[eIndex0].isInRange(pCivInfo->getCivSpecificForClass<Tb, T1>(get1(i))))
			{
				em[eIndex0].set(pCivInfo->getCivSpecificForClass<Tb, T1>(get1(i)), (get2(i) * iChange) > 0);
			}
		}
	}
}

template<typename T0, typename T1 = JIT_NoneTypes, typename T2 = JIT_NoneTypes, typename T3 = JIT_NoneTypes>
class InfoArray : public InfoArray4Only<T0, T1, T2, T3>
{
public:
	InfoArray() : InfoArray4Only<T0, T1, T2, T3>(VARINFO<T0>::JIT, VARINFO<T1>::JIT, VARINFO<T2>::JIT, VARINFO<T3>::JIT)
	{};

	// the actual functions are in child classes. They are added here to get an overview
	// note that some might not work in all cases and some might have different arguments depending on template
#if 0
	int getLength() const;

	T getType(int iIndex) const; // like getBonus, getImprovement etc
	T getWithTemplate(int iIndex, T eVar) const; // get, which works with an argument. Useful for calling from a template function using a templat eVar type

	// EnumMap interaction
	void assignFrom(const EnumMapBase<T0, T>& em);
	void addTo(EnumMapBase<Ta, Tb> & em, int iChange = 1, const CvCivilizationInfo* pCivInfo = NULL) const;
	void copyTo(EnumMapBase<T0, T> & em) const;

	void addTo(EnumMap2D<Ta, Tb, Tc> & em, int iChange = 1, const CvCivilizationInfo* pCivInfo = NULL) const;

	// addTo uses pCivInfo if InfoArray is UnitClassTypes and EnumMap is UnitTypes. Same with buildings
	// in all other cases pCivInfo isn't used and compilation fails unless T0 == Ta
#endif
};

template<typename T0, typename T1, typename T2>
class InfoArray< T0, T1, T2, JIT_NoneTypes> : public InfoArray3Only<T0, T1, T2>
{
public:
	InfoArray() : InfoArray3Only<T0, T1, T2>(VARINFO<T0>::JIT, VARINFO<T1>::JIT, VARINFO<T2>::JIT, JIT_ARRAY_NO_TYPE)
	{};
};

template<typename T0, typename T1>
class InfoArray< T0, T1, JIT_NoneTypes, JIT_NoneTypes> : public InfoArray2Only<T0, T1>
{
public:
	InfoArray() : InfoArray2Only<T0, T1>(VARINFO<T0>::JIT, VARINFO<T1>::JIT, JIT_ARRAY_NO_TYPE, JIT_ARRAY_NO_TYPE)
	{};
};


template<typename T0>
class InfoArray< T0, JIT_NoneTypes, JIT_NoneTypes, JIT_NoneTypes> : public InfoArray1Only<T0>
{
public:
	InfoArray() : InfoArray1Only<T0>(VARINFO<T0>::JIT, JIT_ARRAY_NO_TYPE, JIT_ARRAY_NO_TYPE, JIT_ARRAY_NO_TYPE)
	{};
};


#endif
