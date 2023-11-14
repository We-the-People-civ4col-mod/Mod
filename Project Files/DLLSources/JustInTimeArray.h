#ifndef JUST_IN_TIME_ARRAY_H
#define JUST_IN_TIME_ARRAY_H
#pragma once
// JustInTimeArray.h

/*
 * JustInTimeArray is an array, which allocates just in time for when it's needed.
 * The deconstructor frees the memory, meaning memory allocation and deallocation is automated.
 *
 * Each array is of a type set by a specific xml file or other fixed length like MAX_PLAYERS
 *
 * While they work based on enum JITarrayTypes, the cleanest approach is to use a class name from JustInTimeTypes.h
 *
 * The constructor takes the an argument for the default value (default 0).
 * Any array without memory allocated will assume all the values to be the default and will not allocate by setting the default value.
 * Likewise reading from an unallocated array will return the default value. This means the array is perfectly usable without ever being allocated.
 * In most cases this doesn't matter to the use of the array. Code can use it, get, set and ignore memory management.
 *
 * Performance tip: isAllocated is a quick check to tell if all indexes are the default value. Useful if you only care about non-zero values.
 */

class CvXMLLoadUtility;
class InfoArrayBase;
class CvPlayer;
class CvCivilizationInfo;

class CvSavegameReader;
class CvSavegameWriter;

#include "CvEnums.h"
#include "JustInTimeArrayGetType.h"

template<class T> class JustInTimeArray
{
private:
	T* m_tArray;
	const unsigned short m_iLength;
	const unsigned char m_iType;
	const T m_eDefault;

public:
	// used by getFirstNoneDefault when used on an empty array
	static const int NO_NONEDEFAULT_ARRAY = 0xffff;

	JustInTimeArray(JITarrayTypes eType, T eDefault = (T)0);

	~JustInTimeArray();

	// restore all data to default values
	void reset();

	// non-allocated arrays contains only default values
	// this is a really fast content check without even looking at array content
	// note that it is possible to have allocated arrays with only default content
	inline bool isAllocated() const
	{
		return m_tArray != NULL;
	}

	inline int length() const
	{
		return m_iLength;
	}

	// get stored value
	inline T get(int iIndex) const
	{
		FAssert(iIndex >= 0);
		FAssert(iIndex < m_iLength);
		return m_tArray ? m_tArray[iIndex] : m_eDefault;
	}

	void copy(const JustInTimeArray<T> &OtherArray);

	// assign argument to storage
	// returns -1 when freeing the array and 1 when allocating. 0 in all other cases
	int set(T value, int iIndex);

	// add argument to stored value
	// returns -1 when freeing the array and 1 when allocating. 0 in all other cases
	int add(T value, int iIndex);

	// same as add, but checks index for out of bounds. If it is, then it returns false and does nothing else
	// returns true if an allow change value (a number goes 0->1 or 1->0)
	bool addCache(int iChange, int iIndex);

	// adds an info array to the array
	// use iChange to tell if it should be added or substracted
	// 1D array are read as all included indexes stores 1
	// returns true if an allow change value (a number goes 0->1 or 1->0)
	// pCivInfo is needed when going from BuildingClass->Building or UnitClass to Unit
	bool addCache(int iChange, const InfoArrayBase& kIarray, const CvCivilizationInfo *pCivInfo = NULL);

	// add a number to all indexes
	void addAll(int iChange, int iValue);

	// replace value with argument if argument is higher than the already stored value
	void keepMax(T value, int iIndex);

	// replace value with argument if argument is lower than the already stored value
	void keepMin(T value, int iIndex);

	int getPositiveCount() const;

	T getMax() const;

	// add bound checks. Ignore call if out of bound index
	int safeSet(T value, int iIndex);
	int safeAdd(T value, int iIndex);

	JITarrayTypes getType() const
	{
		return static_cast<JITarrayTypes>(m_iType);
	}

	bool hasContent() const;
	int getFirstNoneDefault() const;
	inline bool isEmpty() const
	{
		return !hasContent();
	}

	unsigned int getNumUsedElements(T* pNormalArray = NULL) const;

	// floating point cache modifiers
	T getFloat(int iIndex) const;

	// function used exclusively for generating the init CivEffect
	// it assigns 0 to any index in pIarray, which has a positive value
	void generateInitCivEffect(const InfoArrayBase& kIarray);

	// bEnable can be used like "uiFlag > x" to make oneline conditional loads
	// pNormalArray can be used to make read/write take place in an array other than the JIT array, but it must be allocated to the same length as the JIT array
	// this is normally not used, but it helps when making savegames resistant to xml changes while keeping vanilla code as untouched as possible
	// TODO remove this when cleaning up savegame code
	void read (FDataStreamBase* pStream, bool bEnable);
	void write(FDataStreamBase* pStream, bool bEnable);
	void Read(FDataStreamBase* pStream);
	void Write(FDataStreamBase* pStream) const;

	void Read(CvSavegameReader& reader);
	void Write(CvSavegameWriter& writer) const;

	void ReadWrite(bool bRead, FDataStreamBase* pStream);

	void read(CvXMLLoadUtility* pXML, const char* sTag);

private:

	void allocate();
public:

	// operator overload
	JustInTimeArray& operator=(const JustInTimeArray &rhs);
	JustInTimeArray& operator+=(const JustInTimeArray &rhs);
	JustInTimeArray& operator-=(const JustInTimeArray &rhs);
	bool operator==(const JustInTimeArray &rhs) const;
	bool operator!=(const JustInTimeArray &rhs) const;
};

// 2D JIT array
template<class T> class JustInTimeArray2D
{
private:
	JustInTimeArray<T> *tArray;
	const unsigned short m_iLength;
	unsigned short m_iArraysInUse;
	const unsigned char m_iType;
	const unsigned char m_iSubType;
	const T m_eDefault;

	void allocate();
	void maybeFreeArray();

public:

	JustInTimeArray2D(JITarrayTypes eType, JITarrayTypes eSubType, T eDefault = (T)0)
		: m_tArray(nullptr)
		, m_iType(eType)
		, m_iSubType(eSubType)
		, m_iLength(getArrayLength(eType))
		, m_eDefault(eDefault)
	{
		FAssert(m_iLength > 1);
		m_iArraysInUse = 0;
	}
	JustInTimeArray2D(int iLength, JITarrayTypes eSubType, T eDefault = (T)0)
		: m_tArray(nullptr)
		, m_iType(JIT_ARRAY_NO_TYPE)
		, m_iSubType(eSubType)
		, m_iLength(iLength)
		, m_eDefault(eDefault)
	{
		FAssert(m_iLength > 1);
		m_iArraysInUse = 0;
	}
	~JustInTimeArray2D();

	// reset all arrays
	void reset();
	T get(int iIndex, int iSubIndex) const;
	void set(T eValue, int iIndex, int iSubIndex);
	void add(T eValue, int iIndex, int iSubIndex);

	// returns true if an allow change value (a number goes 0->1 or 1->0)
	bool addCache(int iChange, const InfoArrayBase* pIarray);

	// tells if primary array is allocated
	bool isAllocated() const;

	// tell if a specific array has an array allocated
	// returns false if the array itself isn't in the primary array
	bool isAllocated(int iIndex) const;

	JITarrayTypes getType() const
	{
		return static_cast<JITarrayTypes>(m_iType);
	}

	JITarrayTypes getSubType() const
	{
		return static_cast<JITarrayTypes>(m_iSubType);
	}

	void Read (FDataStreamBase* pStream);
	void Write(FDataStreamBase* pStream);
	void ReadWrite(bool bRead, FDataStreamBase* pStream);
};

template<class T> class CacheArray2D
{
public:
	CacheArray2D(int iLength, int iSubLength);
	CacheArray2D(int iLength, JITarrayTypes eSubType);
	CacheArray2D(JITarrayTypes eType, int iSubLength);
	CacheArray2D(JITarrayTypes eType, JITarrayTypes eSubType);
	~CacheArray2D();

	void reset();
	T get(int iIndex, int iSubIndex) const;
	T getAccumulative(int iIndex, int iSubIndex) const;

	// returns true if an allow change value (a number goes 0->1 or 1->0)
	bool addCache(int iChange, const InfoArrayBase& kIarray, const CvCivilizationInfo *pCivInfo = NULL);

	// adds numbers to index and all higher indexes. Used for bonus like +1 if number is 3 or higher
	void addCacheAccumulative(int iChange, const InfoArrayBase& kIarray, const CvCivilizationInfo *pCivInfo = NULL);

	// set all variables to a specific value (reset frees the memory and is better than assigning 0)
	void assign(T iNewValue, int iIndex = -1, int iSubIndex = -1);

	int getLength() const {return m_iLength;};
	int getSubLength() const {return m_iSubLength;};

#ifdef FASSERT_ENABLE
	JITarrayTypes getType() const
	{
		return static_cast<JITarrayTypes>(m_iType);
	}

	JITarrayTypes getSubType() const
	{
		return static_cast<JITarrayTypes>(m_iSubType);
	}
#else
	JITarrayTypes getType() const
	{
		return NO_JIT_ARRAY_TYPE;
	}

	JITarrayTypes getSubType() const
	{
		return NO_JIT_ARRAY_TYPE;
	}
#endif

protected:
	void allocate();

	const short m_iLength;
	const short m_iSubLength;
#ifdef FASSERT_ENABLE
	const unsigned char m_iType;
	const unsigned char m_iSubType;
#endif
	T* m_aArray;
};

class CvInfoBase;

JITarrayTypes GetBaseType(JITarrayTypes eType);
bool isHardcodedArray(JITarrayTypes eType);
int getArrayLength(JITarrayTypes eType);
const CvInfoBase* getBaseInfo(JITarrayTypes eType, int iIndex);
const char* getArrayType(JITarrayTypes eType, int iIndex);
CvWString getArrayTypeWide(JITarrayTypes eType, int iIndex);
const char* getArrayName(JITarrayTypes eType);
CvWString getArrayNameWide(JITarrayTypes eType);
const char* getArrayPrefix(JITarrayTypes eType);
int getIndexForType(JITarrayTypes eType, const char* pTypeString);
int getIndexForTypeAddingPrefix(JITarrayTypes eType, const char* pTypeString);
const char* getArrayTypeWithoutPrefix(JITarrayTypes eType, int iIndex);
JITarrayTypes getJITArrayTypeFromString(const char* szType);

template<typename T>
T getIndexForType(const char* pTypeString, const char* pAssertMsg = NULL)
{
	int iIndex = getIndexForType(VARINFO<T>::JIT, pTypeString);
	FAssertMsg(iIndex != -1 || pAssertMsg == NULL, pAssertMsg);
	return (T)iIndex;
}


#include "JustInTimeArrayTypes.h"

#endif
