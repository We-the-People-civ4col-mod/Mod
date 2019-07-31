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

class CvCivilizationInfo;
class InfoArray;

class InfoArray
{
	// declare InfoArrayMod to be friend
	// this allows any InfoArrayMod to access getInternal directly, bypassing the type checks
	// this gives more coding freedom, but getType should be compared in asserts if done so to preserve the strict type checks
	friend class InfoArrayMod;
public:
	InfoArray(JITarrayTypes eType0, JITarrayTypes eType1, JITarrayTypes eType2, JITarrayTypes eType3);
	~InfoArray();

	int getIndex(int iIndex) const; // index as in JIT array - much slower than get() or a real JIT array
	int getIndex(int iIndex, int iSubIndex) const;
	int getLength() const;
	int getDimentions() const;
	bool isBool() const; // dimentions and types are the same 

	int getMin(int iDimention) const;
	int getMax(int iDimention) const;
	bool areIndexesValid(int iMax, int iDimention = 0, int iMin = 0) const;

	InfoArray& operator=(const InfoArray &rhs);

	JITarrayTypes getType(int i) const
	{
		FAssert(i >= 0 && i < 4);
		return (JITarrayTypes)((m_aiTypes >> (8 * i)) & 0xFF);
	}

	int getInt(int iIndex, int iTokenIndex = 0) const
	{
		FAssertMsg(NUM_JITarrayTypes <= getType(iTokenIndex), "InfoArray called with the wrong get function");
		return getInternal(iIndex, iTokenIndex);
	}

	int getWithType(JITarrayTypes eType, int iIndex, int iTokenIndex) const
	{
		FAssert(eType == getType(iTokenIndex) || eType + NUM_JITarrayTypes == getType(iTokenIndex) ||  (eType >= NUM_JITarrayTypes && getType(iTokenIndex) >= NUM_JITarrayTypes));
		return getInternal(iIndex, iTokenIndex);
	}

	// adds UnitClass->Unit and BuildingClass->Building conversion. Otherwise the same as without the civinfo argument 
	int getWithTypeWithConversion(JITarrayTypes eType, int iIndex, int iTokenIndex, const CvCivilizationInfo *pCivInfo) const;

#include "InfoArrayGet.h"

	// needed by python interface. Do not use this in the DLL as it lacks type checking
	int pyGet(int iIndex, int iSubIndex) const {return getInternal(iIndex, iSubIndex);}

protected:
	int getInternal(int iIndex, int iTokenIndex = 0) const;
	short m_iLength;
	char m_iNumDimentions;
	short* m_pArray;
	unsigned int m_aiTypes;
};

///
/// write functions
/// These functions will completely overwrite the existing content
///
class InfoArrayMod : public InfoArray
{
public:
	InfoArrayMod(JITarrayTypes eType0, JITarrayTypes eType1 = JIT_ARRAY_NO_TYPE, JITarrayTypes eType2 = JIT_ARRAY_NO_TYPE, JITarrayTypes eType3 = JIT_ARRAY_NO_TYPE) : InfoArray(eType0, eType1, eType2, eType3){};
	// bReadFloat will multiply the read value by 100 and store the int value of that
	// It doesn't provide extra functionality other than allowing floats to be written in XML for human readability

	// szType is always getType()
	// sTagName is the root xml tax name of the structure to be read
	void read(CvXMLLoadUtility* pXML, const char* szType, const char *sTagName);
	void assign(const BoolArray* pBArray);
	void convertClass(const InfoArray* pArray, const CvCivilizationInfo* pCivInfo = NULL);
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

#endif