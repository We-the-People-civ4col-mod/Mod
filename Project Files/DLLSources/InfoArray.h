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

class CvXMLLoadUtility;
class CvCivilizationInfo;
class BoolArray;

template<class T> class JustInTimeArray;

template<class IndexType, class T>
class EnumMap;

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

#include "InfoArrayGet.h"

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
	void assign(const BoolArray* pBArray);
	void assign(const std::vector<int>& vec);
	void assign(const std::vector<bool>& vec);
	void convertClass(const InfoArrayBase* pArray, const CvCivilizationInfo* pCivInfo = NULL);
private:
	void readRecursive(CvXMLLoadUtility* pXML, int& iIndex, std::vector<short>& aArray, std::vector<short>& aIndex, int iLevel, const char *sTagName, const char* szType);

public:

	// writing assign JIT array code in the header to avoid template errors/warnings
	template<class T>
	void assign(const JustInTimeArray<T>* pJITArray)
	{
		FAssert(m_iNumDimentions == 2);
		SAFE_DELETE_ARRAY(m_pArray);
		m_iLength = 0;
		for (int i = 0; i < pJITArray->length(); i++)
		{
			if (pJITArray->get(i) != 0)
			{
				m_iLength++;
			}
		}

		if (m_iLength == 0)
		{
			return;
		}

		m_pArray = new short[m_iLength * 2];

		int iCounter = 0;

		for (int i = 0; i < pJITArray->length(); i++)
		{
			if (pJITArray->get(i) != 0)
			{
				m_pArray[iCounter] = i;
				iCounter++;
				m_pArray[iCounter] = pJITArray->get(i);
				iCounter++;
			}
		}
	}
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
	template<typename T>
	void assignFrom(const EnumMap<T0, T>& em);
	template<typename T>
	void addTo(EnumMap<T0, T> & em, int iChange = 1) const;
	template<typename T>
	void copyTo(EnumMap<T0, T> & em) const;
};
template<typename T0, typename T1>
class InfoArray2Only : public InfoArray2<T0, T1>
{
protected:
	InfoArray2Only(JITarrayTypes eType0, JITarrayTypes eType1, JITarrayTypes eType2, JITarrayTypes eType3)
		: InfoArray2<T0, T1>(eType0, eType1, eType2, eType3) {}
public:
	template<typename T>
	void assignFrom(const EnumMap<T0, T>& em);
	template<typename T>
	void addTo(EnumMap<T0, T> & em, int iChange = 1) const;
	template<typename T>
	void copyTo(EnumMap<T0, T> & em) const;
	void copyTo(EnumMap<T0, bool> & em) const;
};
template<typename T0, typename T1, typename T2>
class InfoArray3Only : public InfoArray3<T0, T1, T2>
{
protected:
	InfoArray3Only(JITarrayTypes eType0, JITarrayTypes eType1, JITarrayTypes eType2, JITarrayTypes eType3)
		: InfoArray3<T0, T1, T2>(eType0, eType1, eType2, eType3) {}
};
template<typename T0, typename T1, typename T2, typename T3>
class InfoArray4Only : public InfoArray4<T0, T1, T2, T3>
{
protected:
	InfoArray4Only(JITarrayTypes eType0, JITarrayTypes eType1, JITarrayTypes eType2, JITarrayTypes eType3)
		: InfoArray4<T0, T1, T2, T3>(eType0, eType1, eType2, eType3) {}
};

template<typename T0>
template<typename T>
void InfoArray1Only<T0>::assignFrom(const EnumMap<T0, T> & em)
{
	const int iLength = em.GetNumPositiveElements();
	_setLength(iLength);
	for (T0 eIndex = (T0)0; eIndex < em.getLength(); ++eIndex)
	{
		int iVal = em.get(eIndex);
		if (em.isPositive(iVal))
		{
			bool bDone = _setValue(eIndex);
#ifndef FASSERT_ENABLE
			if (bDone) return;
#endif
		}
	}
}


template<typename T0, typename T1>
template<typename T>
void InfoArray2Only<T0, T1>::assignFrom(const EnumMap<T0, T> & em)
{
	const bool bTypeCheck = !boost::is_same<T, bool>::value;
	BOOST_STATIC_ASSERT(bTypeCheck);
	const int iLength = em.GetNumNonZeroElements();
	_setLength(iLength);
	for (T0 eIndex = (T0)0; eIndex < em.getLength(); ++eIndex)
	{
		int iVal = em.get(eIndex);
		if (iVal != 0)
		{
			bool bDone = _setValue(eIndex, iVal);
#ifndef FASSERT_ENABLE
			if (bDone) return;
#endif
		}
	}
}

template<typename T0>
template<typename T>
void InfoArray1Only<T0>::addTo(EnumMap<T0, T> & em, int iChange) const
{
	const bool bTypeCheck = !boost::is_same<T, bool>::value;
	BOOST_STATIC_ASSERT(bTypeCheck);
	for (int i = 0; i < getLength(); ++i)
	{
		em.add((T0)getInternal(i), iChange);
	}
}

template<typename T0, typename T1>
template<typename T>
void InfoArray2Only<T0, T1>::addTo(EnumMap<T0, T> & em, int iChange) const
{
	const bool bTypeCheck = !boost::is_same<T, bool>::value;
	BOOST_STATIC_ASSERT(bTypeCheck);
	for (int i = 0; i < getLength(); ++i)
	{
		em.add((T0)getInternal(i), getInternal(i, 1) * iChange);
	}
}

template<typename T0>
template<typename T>
void InfoArray1Only<T0>::copyTo(EnumMap<T0, T> & em) const
{
	em.reset();
	for (int i = 0; i < getLength(); ++i)
	{
		em.set((T0)getInternal(i), 1);
	}
}

template<typename T0, typename T1>
template<typename T>
void InfoArray2Only<T0, T1>::copyTo(EnumMap<T0, T> & em) const
{
	em.reset();
	for (int i = 0; i < getLength(); ++i)
	{
		em.set((T0)getInternal(i), getInternal(i, 1));
	}
}

template<typename T0, typename T1>
void InfoArray2Only<T0, T1>::copyTo(EnumMap<T0, bool> & em) const
{
	em.reset();
	for (int i = 0; i < getLength(); ++i)
	{
		em.set((T0)getInternal(i), getInternal(i, 1) > 0);
	}
}

// the "real" classes. All declared in defines as they become type strict (returns T0 type etc)

#define INFO_ARRAY_GET_1(type, getName, JITtype, returnType)                                                                                                  \
template<> class InfoArray1<type>                                                                                                                             \
	: protected InfoArrayBase                                                                                                                                 \
		, public boost::noncopyable                                                                                                                           \
{                                                                                                                                                             \
friend class CvCity;                                                                                                                                          \
friend class CvGlobals;                                                                                                                                       \
friend class CivEffectInfo;                                                                                                                                   \
friend class CvPlayerCivEffect;                                                                                                                               \
friend class CvInfoBase;                                                                                                                                      \
protected:                                                                                                                                                    \
	InfoArray1(JITarrayTypes eType0, JITarrayTypes eType1, JITarrayTypes eType2, JITarrayTypes eType3) : InfoArrayBase(eType0, eType1, eType2, eType3) {}     \
public:                                                                                                                                                       \
	int getLength() const {return InfoArrayBase::getLength();}                                                                                                \
	returnType getName(int iIndex) const                                                                                                                      \
	{                                                                                                                                                         \
		return static_cast< returnType > (getInternal(iIndex, 0));                                                                                            \
	}                                                                                                                                                         \
	returnType getWithTemplate(int iIndex, returnType eVar) const                                                                                             \
	{                                                                                                                                                         \
		return static_cast< returnType > (getInternal(iIndex, 0));                                                                                            \
	}                                                                                                                                                         \
	int getIndexOf(returnType eValue) const                                                                                                                   \
	{                                                                                                                                                         \
		return _getIndexOf(eValue, 0);                                                                                                                        \
	}                                                                                                                                                         \
};

#define INFO_ARRAY_GET_2(type, getName, JITtype, returnType)                                                                                                  \
template<typename T0> class InfoArray2<T0, type> : public InfoArray1<T0>                                                                                      \
{                                                                                                                                                             \
protected:                                                                                                                                                    \
	InfoArray2(JITarrayTypes eType0, JITarrayTypes eType1, JITarrayTypes eType2, JITarrayTypes eType3)                                                        \
	: InfoArray1<T0>(eType0, eType1, eType2, eType3) {}                                                                                                       \
public:                                                                                                                                                       \
	returnType getName(int iIndex) const                                                                                                                      \
	{                                                                                                                                                         \
		return static_cast< returnType > (getInternal(iIndex, 1));                                                                                            \
	}                                                                                                                                                         \
	returnType getWithTemplate(int iIndex, returnType eVar) const                                                                                             \
	{                                                                                                                                                         \
		return static_cast< returnType > (getInternal(iIndex, 1));                                                                                            \
	}                                                                                                                                                         \
	int getIndexOf(returnType eValue) const                                                                                                                   \
	{                                                                                                                                                         \
		return _getIndexOf(eValue, 1);                                                                                                                        \
	}                                                                                                                                                         \
};

#define INFO_ARRAY_GET_3(type, getName, JITtype, returnType)                                                                                                  \
template<typename T0, typename T1> class InfoArray3<T0, T1, type> : public InfoArray2<T0, T1>                                                                 \
{                                                                                                                                                             \
protected:                                                                                                                                                    \
	InfoArray3(JITarrayTypes eType0, JITarrayTypes eType1, JITarrayTypes eType2, JITarrayTypes eType3)                                                        \
	: InfoArray2<T0, T1>(eType0, eType1, eType2, eType3) {}                                                                                                   \
public:                                                                                                                                                       \
	returnType getName(int iIndex) const                                                                                                                      \
	{                                                                                                                                                         \
		return static_cast< returnType > (getInternal(iIndex, 2));                                                                                            \
	}                                                                                                                                                         \
	returnType getWithTemplate(int iIndex, returnType eVar) const                                                                                             \
	{                                                                                                                                                         \
		return static_cast< returnType > (getInternal(iIndex, 2));                                                                                            \
	}                                                                                                                                                         \
	int getIndexOf(returnType eValue) const                                                                                                                   \
	{                                                                                                                                                         \
		return _getIndexOf(eValue, 2);                                                                                                                        \
	}                                                                                                                                                         \
};

#define INFO_ARRAY_GET_4(type, getName, JITtype, returnType)                  \
template<typename T0, typename T1, typename T2> class InfoArray4<T0, T1, T2, type> : public InfoArray3<T0, T1, T2>                                            \
{                                                                                                                                                             \
protected:                                                                                                                                                    \
	InfoArray4(JITarrayTypes eType0, JITarrayTypes eType1, JITarrayTypes eType2, JITarrayTypes eType3)                                                        \
      : InfoArray3<T0, T1, T2>(eType0, eType1, eType2, eType3) {}                                                                                             \
public:                                                                                                                                                       \
	returnType getName(int iIndex) const                                                                                                                      \
	{                                                                                                                                                         \
		return static_cast< returnType > (getInternal(iIndex, 3));                                                                                            \
	}                                                                                                                                                         \
	returnType getWithTemplate(int iIndex, returnType eVar) const                                                                                             \
	{                                                                                                                                                         \
		return static_cast< returnType > (getInternal(iIndex, 3));                                                                                            \
	}                                                                                                                                                         \
	int getIndexOf(returnType eValue) const                                                                                                                   \
	{                                                                                                                                                         \
		return _getIndexOf(eValue, 3);                                                                                                                        \
	}                                                                                                                                                         \
};





#define INFO_ARRAY_GET_ALL(type, getName, JITtype, type2)                                                                                                     \
    INFO_ARRAY_GET_1(type, getName, JITtype, type2)                                                                                                           \
	INFO_ARRAY_GET_2(type, getName, JITtype, type2)                                                                                                           \
	INFO_ARRAY_GET_3(type, getName, JITtype, type2)                                                                                                           \
	INFO_ARRAY_GET_4(type, getName, JITtype, type2)

#define INFO_ARRAY_GET(type, getName, JITtype)                                                                                                                \
	INFO_ARRAY_GET_ALL(type, getName, JITtype, type)


#define INFO_ARRAY_GET_INT(type, getName, JITtype)                                                                                                            \
	INFO_ARRAY_GET_2(type, getName, JITtype, type)                                                                                                            \
	INFO_ARRAY_GET_3(type, getName, JITtype, type)                                                                                                            \
	INFO_ARRAY_GET_4(type, getName, JITtype, type)



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
	void assignFrom(const EnumMap<T0, T>& em);
	void addTo(EnumMap<T0, T> & em, int iChange = 1) const;
	void copyTo(EnumMap<T0, T> & em) const;
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