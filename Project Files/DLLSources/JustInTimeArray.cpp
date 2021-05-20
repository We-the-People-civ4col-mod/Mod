// JustInTimeArray.cpp

// usage guide is written in JustInTimeArray.h
// file written by Nightinggale

#include "CvGameCoreDLL.h"
#include "CvXMLLoadUtility.h"
#include "CvDLLXMLIFaceBase.h"

#include "InfoArray.h"

#include "CvSavegame.h"

template<class T>
JustInTimeArray<T>::JustInTimeArray(JITarrayTypes eType, T eDefault)
: m_tArray(NULL)
, m_iType(eType)
, m_iLength(getArrayLength(eType))
, m_eDefault(eDefault)
{
	FAssertMsg(m_iLength != 0, "arrays can't have 0 length");
}

template<class T>
JustInTimeArray<T>::~JustInTimeArray()
{
	reset();
}

template<class T>
void JustInTimeArray<T>::reset()
{
	// only reset() and deconstructor should free memory as that can help debugging
	SAFE_DELETE_ARRAY(m_tArray);
}

template<class T>
void JustInTimeArray<T>::copy(const JustInTimeArray<T> &OtherArray)
{
	FAssert(this->getType() == OtherArray.getType());
	FAssert(this->m_eDefault == OtherArray.m_eDefault);

	if (!OtherArray.isAllocated())
	{ 
		reset();
		return;
	}

	if (!this->isAllocated())
	{
		this->allocate();
	}
	memcpy(this->m_tArray, OtherArray.m_tArray, length() * sizeof(T));
}

template<class T>
int JustInTimeArray<T>::set(T value, int iIndex)
{
	FAssert(iIndex >= 0);
	FAssert(iIndex < m_iLength);

	int iReturnVal = 0;

	if (m_tArray == NULL)
	{
		if (value == m_eDefault)
		{
			// no need to allocate memory to assign a default value
			return 0;
		}
		iReturnVal++;
		m_tArray = new T[m_iLength];
		for (int iIterator = 0; iIterator < m_iLength; ++iIterator)
		{
			m_tArray[iIterator] = m_eDefault;
		}
	}
	m_tArray[iIndex] = value;

	if (value == m_eDefault)
	{
		// release memory if it only has default values left
		if (isEmpty())
		{
			iReturnVal--;
		}
	}
	return iReturnVal;
}

template<class T>
int JustInTimeArray<T>::add(T value, int iIndex)
{
	if (value == 0)
	{
		// no data change
		return 0;
	}
	return set((T)(value + get(iIndex)), iIndex);
}

template<class T>
bool JustInTimeArray<T>::addCache(int iChange, int iIndex)
{
	if (iChange == 0 || iIndex < 0 || iIndex >= length())
	{
		return 0;
	}
	 
	bool bBefore = get(iIndex) > 0;
	add((T)iChange, iIndex);
	bool bAfter = get(iIndex) > 0;
	bool bChanged = !bBefore != !bAfter;

	return bChanged;
}

template<>
double JustInTimeArray<double>::getFloat(int iIndex) const
{
	FAssert(iIndex >= 0);
	FAssert(iIndex < m_iLength);
	if (m_tArray != NULL)
	{
		return m_tArray[iIndex] + FLOAT_OFFSET;
	}

	return m_eDefault + FLOAT_OFFSET;
}

template<class T>
T JustInTimeArray<T>::getFloat(int iIndex) const
{
	FAssertMsg(false, "getFloat only works for floating points");
	return (T)0;
}

/*
template<>
bool JustInTimeArray<double>::addCache(int iChange, const InfoArray* pIarray, CvPlayer* pPlayer, int (CvPlayer::*fpConvert)(int) const, void (CvPlayer::*fpUpdate)(int, bool), JITarrayTypes eType)
{
	FAssert(pIarray->getDimentions() == 2);
	FAssert(iChange == 1 || iChange == -1);
	FAssert(!(pPlayer == NULL && fpConvert != NULL));
	FAssert(!(pPlayer == NULL && fpUpdate  != NULL));

	int iLength = pIarray->getLength();

	for (int i = 0; i < iLength; i++)
	{
		int iIndex = pIarray->getWithType(getType(), i, 0);
		FAssert(iIndex >= 0);
		if (fpConvert != NULL)
		{
			iIndex = (pPlayer->*fpConvert)(iIndex);
		}
		int iValue = pIarray->getWithType(pIarray->getType(1), i, 1);
		double fValue = (iValue + 100) / 100.0;

		FAssert(iValue != 0);
		FAssert(iIndex < this->m_iLength);
		if (iIndex >= 0 && iIndex < this->m_iLength)
		{
			if (iChange == 1)
			{
				set(get(iIndex) * fValue, iIndex);
			}
			else
			{
				set(get(iIndex) / fValue, iIndex);
			}
			if (fpUpdate != NULL)
			{
				// read below for reasonf for setting change to false
				(pPlayer->*fpUpdate)(iIndex, false);
			}
		}
	}

	// floating points should never be used for allow, hence the return value is unused
	// however to comply with the addCache template function declaration, it has to return a bool
	return false;
}
*/

// Adding an InfoArray
// this is used by CivEffects to store the combined values from multiple InfoArrays in a single JIT array
template<class T>
bool JustInTimeArray<T>::addCache(int iChange, const InfoArrayBase& kIarray, const CvCivilizationInfo *pCivInfo)
{
	bool bChanged = false;
	bool b2D = kIarray.getDimentions() == 2;
	int iLength = kIarray.getLength();

	FAssert(kIarray.getDimentions() <= 2);
	FAssert(iChange == 1 || iChange == -1);

	for (int i = 0; i < iLength; i++)
	{
		int iIndex = kIarray.getWithTypeWithConversion(getType(), i, 0, pCivInfo);
		if (iIndex >= 0)
		{
			int iValue = 1;
			if (b2D)
			{
				iValue = kIarray.getWithTypeWithConversion(kIarray.getType(1), i, 1, pCivInfo);
			}
			FAssert(iValue != 0);
			FAssert(iIndex < this->m_iLength);
			if (iIndex >= 0 && iIndex < this->m_iLength)
			{
				T tValue = (T)(iValue * iChange);
				bool bChangedThisIndex = this->addCache(tValue, iIndex);
				bChanged |= bChangedThisIndex;
			}
		}
	}

	return bChanged;
}

template<>
void JustInTimeArray<double>::addAll(int iChange, int iValue)
{
	FAssert(iChange == 1 || iChange == -1);
	if (iValue != 0)
	{
		double fValue = (iValue + 100) / 100.0;

		for (int iIndex = 0; iIndex < m_iLength; ++iIndex)
		{
			if (iChange == 1)
			{
				set(get(iIndex) * fValue, iIndex);
			}
			else
			{
				set(get(iIndex) / fValue, iIndex);
			}
		}
	}
}

template<class T>
void JustInTimeArray<T>::addAll(int iChange, int iValue)
{
	FAssert(iChange == 1 || iChange == -1);
	if (iValue != 0)
	{
		for (int iIndex = 0; iIndex < m_iLength; ++iIndex)
		{
			add((T)(iValue * iChange), iIndex);
		}
	}
}

template<class T>
void JustInTimeArray<T>::keepMax(T value, int iIndex)
{
	if (value > get(iIndex))
	{
		set(value, iIndex);
	}
}


template<class T>
void JustInTimeArray<T>::keepMin(T value, int iIndex)
{
	if (value < get(iIndex))
	{
		set(value, iIndex);
	}
}

template<class T>
int JustInTimeArray<T>::getPositiveCount() const
{
	if (!isAllocated())
	{
		return m_eDefault > 0 ? m_iLength : 0;
	}
	
	int iCount = 0;
	for (int i = 0; i < m_iLength; ++i)
	{
		if (get(i) > 0)
		{
			++iCount;
		}
	}
	return iCount;
}

template<class T>
T JustInTimeArray<T>::getMax() const
{
	if (!isAllocated())
	{
		return m_eDefault ;
	}
	return *std::max_element(m_tArray, m_tArray + m_iLength);
}

template<class T>
int JustInTimeArray<T>::safeSet(T value, int iIndex)
{
	if (iIndex >= 0 && iIndex < this->length())
	{
		return this->set(value, iIndex);
	}
	return 0;
}

template<class T>
int JustInTimeArray<T>::safeAdd(T value, int iIndex)
{
	if (iIndex >= 0 && iIndex < this->length())
	{
		return this->add(value, iIndex);
	}
	return 0;
}

template<class T>
bool JustInTimeArray<T>::hasContent()
{
	if (m_tArray == NULL)
	{
		return false;
	}
	for (int iIterator = 0; iIterator < m_iLength; ++iIterator)
	{
		if (m_tArray[iIterator] != m_eDefault)
		{
			return true;
		}
	}

	reset();
	return false;
}

template<class T>
int JustInTimeArray<T>::getFirstNoneDefault()
{
	if (m_tArray == NULL)
	{
		return NO_NONEDEFAULT_ARRAY;
	}
	for (int iIterator = 0; iIterator < m_iLength; ++iIterator)
	{
		if (m_tArray[iIterator] != m_eDefault)
		{
			return iIterator;
		}
	}

	reset();
	return NO_NONEDEFAULT_ARRAY;
}

template<class T>
unsigned int JustInTimeArray<T>::getNumUsedElements(T* pNormalArray) const
{
	T* pPointer = pNormalArray != NULL ? pNormalArray : m_tArray;

	unsigned int iMax = 0;
	if (pPointer != NULL)
	{
		for (unsigned int iIndex = 0; iIndex < m_iLength; iIndex++)
		{
			if (pPointer[iIndex] != m_eDefault)
			{
				iMax = 1 + iIndex;
			}
		}
	}
	return iMax;
}

template<>
void JustInTimeArray<int>::generateInitCivEffect(const InfoArrayBase& kIarray)
{
	FAssert(kIarray.getDimentions() == 2);
	FAssert(this->getType() == kIarray.getType(0));

	for (int i = 0; i < kIarray.getLength(); ++i)
	{
		if (kIarray.getWithType(kIarray.getType(1), i, 1) > 0)
		{
			set(-1, kIarray.getWithType(getType(), i, 0));
		}
	}
}

template<class T>
void JustInTimeArray<T>::generateInitCivEffect(const InfoArrayBase& kIarray)
{
	// silly little function, which is needed because generateInitCivEffect can't handle floating points (compiler warning/error avoidance)
	FAssertMsg(false, "available only to ints");
}

template<class T>
void JustInTimeArray<T>::ReadWrite(bool bRead, FDataStreamBase* pStream)
{
	if (bRead)
	{
		Read(pStream);
	}
	else
	{
		Write(pStream);
	}
}

template<class T>
void JustInTimeArray<T>::read(FDataStreamBase* pStream, bool bRead)
{
	// backward compatible function
	reset();
	if (!bRead)
	{
		return;
	}
	
	for (unsigned int i = 0; i < m_iLength; ++i)
	{
		int iBuffer = 0;
		pStream->Read(&iBuffer);
		this->set(iBuffer, i);
	}
}

template<class T>
void JustInTimeArray<T>::write(FDataStreamBase* pStream, bool bWrite)
{
	// backward compatible function
	if (!bWrite)
	{
		return;
	}
	
	for (unsigned int i = 0; i < m_iLength; ++i)
	{
		int iBuffer = get(i);
		pStream->Write(iBuffer);
		this->set(iBuffer, i);
	}
}

template<class T>
void JustInTimeArray<T>::Read(FDataStreamBase* pStream)
{
	reset();
	allocate();

	unsigned short iLength = 0;
	pStream->Read(&iLength);

	if (iLength > 0)
	{
		pStream->Read(iLength, m_tArray);
	}
}

template<class T>
void JustInTimeArray<T>::Write(FDataStreamBase* pStream)
{
	unsigned short iLength = this->getNumUsedElements();

	pStream->Write(iLength);
	if (iLength > 0)
	{
		pStream->Write(iLength, m_tArray);
	}
}

template<class T>
void JustInTimeArray<T>::Read(CvSavegameReader& reader)
{
	reset();

	unsigned short iStart = NO_NONEDEFAULT_ARRAY;
	reader.Read(iStart);

	if (iStart == NO_NONEDEFAULT_ARRAY)
	{
		return;
	}

	unsigned short iLength = 0;
	reader.Read(iLength);

	for (int i = iStart; i < iLength; ++i)
	{
		T eBuffer;
		reader.Read(eBuffer);
		int iNewIndex = reader.ConvertIndex(getType(), i);
		if (iNewIndex >= 0 && iNewIndex < m_iLength)
		{
			set(eBuffer, iNewIndex);
		}
	}
}


template<class T>
void JustInTimeArray<T>::Write(CvSavegameWriter& writer)
{
	unsigned short iStart = this->getFirstNoneDefault();
	unsigned short iLength = this->getNumUsedElements();

	writer.Write(iStart);

	if (iStart == NO_NONEDEFAULT_ARRAY)
	{
		return;
	}

	// the conversion table will be needed on load
	// add one if it's not already added
	writer.GetXmlByteSize(this->getType());

	writer.Write(iLength);
	for (int i = iStart; i < iLength; ++i)
	{
		writer.Write(get(i));
	}
}


template<class T>
void JustInTimeArray<T>::allocate()
{
	if (!isAllocated())
	{
		m_tArray = new T[m_iLength];
	}

	if (sizeof(T) == 1 || m_eDefault == 0)
	{
		memset(m_tArray, m_eDefault, m_iLength * sizeof(T));
	}
	else
	{
		for (unsigned int i = 0; i < m_iLength; ++i)
		{
			m_tArray[i] = m_eDefault;
		}
	}
}

template<class T>
void JustInTimeArray<T>::read(CvXMLLoadUtility* pXML, const char* sTag)
{
	// read the data into a temp int array and then set the permanent array with those values.
	// this is a workaround for template issues
	FAssert(this->m_iLength > 0);
	int *iArray = new int [m_iLength];
	pXML->SetVariableListTagPair(&iArray, sTag, this->m_iLength, 0);
	for (int i = 0; i < this->m_iLength; i++)
	{
		this->set((T)iArray[i], i);
	}
	SAFE_DELETE_ARRAY(iArray);
	this->hasContent(); // release array if possible
}


/// operator overload

template<class T>
JustInTimeArray<T>& JustInTimeArray<T>::operator=(const JustInTimeArray<T> &rhs)
{
	FAssert(m_iLength == rhs.m_iLength);
	FAssert(m_iType == rhs.m_iType);

	for (int i=0; i < m_iLength; i++)
	{
		this->set(rhs.get(i), i);
	}
	
	this->hasContent(); // clear array if possible

	return *this;
}

template<class T>
JustInTimeArray<T>& JustInTimeArray<T>::operator+=(const JustInTimeArray<T> &rhs)
{
	FAssert(m_iLength == rhs.m_iLength);
	FAssert(m_iType == rhs.m_iType);

	if (rhs.isAllocated() || rhs.m_eDefault != 0)
	{
		for (int i=0; i < m_iLength; i++)
		{
			this->add(rhs.get(i), i);
		}
	}

	this->hasContent(); // clear array if possible

	return *this;
}

template<class T>
JustInTimeArray<T>& JustInTimeArray<T>::operator-=(const JustInTimeArray<T> &rhs)
{
	FAssert(m_iLength == rhs.m_iLength);
	FAssert(m_iType == rhs.m_iType);

	if (rhs.isAllocated() || rhs.m_eDefault != 0)
	{
		for (int i=0; i < m_iLength; i++)
		{
			this->add((T)(-rhs.get(i)), i);
		}
	}

	this->hasContent(); // clear array if possible

	return *this;
}

template<class T>
bool JustInTimeArray<T>::operator==(const JustInTimeArray<T> &rhs) const
{
	FAssert(m_iLength == rhs.m_iLength);
	FAssert(m_iType == rhs.m_iType);

	for (int i=0; i < m_iLength; i++)
	{
		if (get(i) != rhs.get(i))
		{
			return false;
		}
	}

	return true;
}

template<class T>
bool JustInTimeArray<T>::operator!=(const JustInTimeArray<T> &rhs) const
{
	return !(this == &rhs);
}

template<class T>
JustInTimeArray2D<T>::JustInTimeArray2D<T>(JITarrayTypes eType, JITarrayTypes eSubType, T eDefault)
	: tArray(NULL)
	, m_iType(eType)
	, m_iSubType(eSubType)
	, m_iLength(getArrayLength(eType))
	, m_eDefault(eDefault)
{
	FAssert(m_iLength > 1);
	m_iArraysInUse = 0;
}

template<class T>
JustInTimeArray2D<T>::JustInTimeArray2D<T>(int iLength, JITarrayTypes eSubType, T eDefault)
	: tArray(NULL)
	, m_iType(JIT_ARRAY_NO_TYPE)
	, m_iSubType(eSubType)
	, m_iLength(iLength)
	, m_eDefault(eDefault)
{
	FAssert(m_iLength > 1);
	m_iArraysInUse = 0;
}

template<class T>
JustInTimeArray2D<T>::~JustInTimeArray2D<T>()
{
	reset();
	maybeFreeArray();
}

template<class T>
void JustInTimeArray2D<T>::allocate()
{
	// This whole function is one big ugly hack.
	// The problem is that the regular approach with new only works with a default constructor.
	// We need a non-default one and the general advice is to use vectors, through they didn't work here either.
	// Instead the approach is to go a bit more low level, allocate the memory manually without calling a constructor
	//   next the memory is filled with data from the local JIT array, which did call the constructor.
	// This only works because all arrays are identical and no array in the JIT arrays has been allocated yet.
	
	if (tArray != NULL)
	{
		return;
	}

	JustInTimeArray<T> eBuffer((JITarrayTypes)m_iSubType, m_eDefault);

	tArray = (JustInTimeArray<T>*)malloc(sizeof(eBuffer)*m_iLength);

	JustInTimeArray<T> *pArray = tArray;

	for (int i=0; i < m_iLength; i++)
	{
		memcpy(pArray, &eBuffer, sizeof(eBuffer));
		pArray++;
	}
}

template<class T>
void JustInTimeArray2D<T>::maybeFreeArray()
{
	if (isAllocated() && m_iArraysInUse == 0)
	{
		free(tArray);
		tArray = NULL;
	}
}

template<class T>
void JustInTimeArray2D<T>::reset()
{
	if (isAllocated())
	{
		m_iArraysInUse = 0;
		for (int i=0; i < m_iLength; i++)
		{
			tArray[i].reset();
		}
	}
}

template<class T>
T JustInTimeArray2D<T>::get(int iIndex, int iSubIndex) const
{
	FAssert(iIndex >= 0 && iIndex < m_iLength);
	return isAllocated(iIndex) ? tArray[iIndex].get(iSubIndex) : m_eDefault;
}

template<class T>
void JustInTimeArray2D<T>::set(T eValue, int iIndex, int iSubIndex)
{
	FAssert(iIndex >= 0 && iIndex < m_iLength);

	if (!isAllocated() && eValue == m_eDefault)
	{
		// writing default value to unallocated memory.
		return;
	}
	allocate();
	m_iArraysInUse += tArray[iIndex].set(eValue, iSubIndex);
	maybeFreeArray();
}

template<class T>
void JustInTimeArray2D<T>::add(T eValue, int iIndex, int iSubIndex)
{
	FAssert(iIndex >= 0 && iIndex < m_iLength);

	if (eValue == 0)
	{
		// no change shouldn't be able to trigger memory allocation
		return;
	}
	allocate();
	m_iArraysInUse += tArray[iIndex].add(eValue, iSubIndex);
	maybeFreeArray();
}

template<class T>
bool JustInTimeArray2D<T>::addCache(int iChange, const InfoArrayBase* pIarray)
{
	bool bChanged = false;

	if (pIarray->getLength() == 0)
	{
		return false;
	}
	FAssert(iChange == 1 || iChange == -1);

	int iSubLength = getArrayLength((JITarrayTypes)m_iSubType);

	for (int i = 0; i < pIarray->getLength(); i++)
	{
		int iIndex = pIarray->getWithType(getType(), i, 0);
		if (iIndex >= 0 && iIndex < m_iLength)
		{
			int iSubIndex = pIarray->getWithType(getSubType(), i, 1);
			if (iSubIndex >= 0 && iSubIndex < iSubLength)
			{
				bool bBefore = get(iIndex, iSubIndex) > 0;
				allocate();
				this->add(iChange * pIarray->getInt(i, 2), iIndex, iSubIndex);
				bool bAfter = get(iIndex, iSubIndex) > 0;
				bChanged |= !bBefore != !bAfter; // XOR
			}
		}
	}

	return bChanged;
};

template<class T>
bool JustInTimeArray2D<T>::isAllocated() const
{
	return tArray != NULL;
}

template<class T>
bool JustInTimeArray2D<T>::isAllocated(int iIndex) const
{
	FAssert(iIndex >= 0 && iIndex < m_iLength);
	return tArray != NULL && tArray[iIndex].isAllocated();
}


template<class T>
void JustInTimeArray2D<T>::Read(FDataStreamBase* pStream)
{
	ReadWrite(true, pStream);
}

template<class T>
void JustInTimeArray2D<T>::Write(FDataStreamBase* pStream)
{
	ReadWrite(false, pStream);
}

template<class T>
void JustInTimeArray2D<T>::ReadWrite(bool bRead, FDataStreamBase* pStream)
{
	
	unsigned short iLength = 0;

	if (bRead)
	{
		reset();
	}
	else
	{
		if (isAllocated())
		{
			// determine how many arrays should be saved
			// we will skip saving empty arrays if possible
			for (int i = 0; i < m_iLength; i++)
			{
				if (tArray[i].isAllocated())
				{
					iLength = i + 1;
				}
			}
		}
	}

	CvInitCore& eInitCore = GC.getInitCore();

	if (bRead)
	{
		pStream->Read(&iLength);
	}
	else
	{
		pStream->Write(iLength);
	}

	for (int i = 0; i < iLength; i++)
	{
		int iIndex = i;

		/*
		if (bRead && m_iType != JIT_ARRAY_NO_TYPE)
		{
			iIndex = eInitCore.convertArrayInfo((JITarrayTypes)m_iType, iIndex);
		}
		*/

		if (iIndex >= 0 && iIndex < m_iLength)
		{
			allocate();
			tArray[iIndex].ReadWrite(bRead, pStream);
			if (bRead && tArray[iIndex].isAllocated())
			{
				// count that a new array is allocated
				m_iArraysInUse++;
			}
		}
		else
		{
			// index is no longer in the array. Read it from the savegame and discard the result.
			JustInTimeArray<T> eBuffer((JITarrayTypes)m_iSubType);
			eBuffer.Read(pStream);
		}
	}
	maybeFreeArray();
	
}

template<class T>
CacheArray2D<T>::CacheArray2D(int iLength, int iSubLength)
	: m_iLength(iLength)
	, m_iSubLength(iSubLength)
	, m_aArray(NULL)
#ifdef FASSERT_ENABLE
	, m_iType(JIT_ARRAY_NO_TYPE)
	, m_iSubType(JIT_ARRAY_NO_TYPE)
#endif
{
}

template<class T>
CacheArray2D<T>::CacheArray2D(int iLength, JITarrayTypes eSubType)
	: m_iLength(iLength)
	, m_iSubLength(getArrayLength(eSubType))
	, m_aArray(NULL)
#ifdef FASSERT_ENABLE
	, m_iType(JIT_ARRAY_NO_TYPE)
	, m_iSubType(eSubType)
#endif
{
}

template<class T>
CacheArray2D<T>::CacheArray2D(JITarrayTypes eType, int iSubLength)
	: m_iLength(getArrayLength(eType))
	, m_iSubLength(iSubLength)
	, m_aArray(NULL)
#ifdef FASSERT_ENABLE
	, m_iType(eType)
	, m_iSubType(JIT_ARRAY_NO_TYPE)
#endif
{
}

template<class T>
CacheArray2D<T>::CacheArray2D(JITarrayTypes eType, JITarrayTypes eSubType)
	: m_iLength(getArrayLength(eType))
	, m_iSubLength(getArrayLength(eSubType))
	, m_aArray(NULL)
#ifdef FASSERT_ENABLE
	, m_iType(eType)
	, m_iSubType(eSubType)
#endif
{
}

template<class T>
CacheArray2D<T>::~CacheArray2D<T>()
{
	reset();
}

template<class T>
T CacheArray2D<T>::get(int iIndex, int iSubIndex) const
{
	if (m_aArray == NULL)
	{
		return 0;
	}

	FAssert(iIndex >= 0 && iIndex < m_iLength);
	FAssert(iSubIndex >= 0 && iSubIndex < m_iSubLength);

	int iArrayIndex = (iIndex * m_iLength) + iSubIndex;
	return m_aArray[iArrayIndex];
}

template<class T>
T CacheArray2D<T>::getAccumulative(int iIndex, int iSubIndex) const
{
	return get(iIndex, iSubIndex < m_iSubLength ? iSubIndex : (m_iSubLength-1));
}


template<class T>
bool CacheArray2D<T>::addCache(int iChange, const InfoArrayBase& kIarray, const CvCivilizationInfo *pCivInfo)
{
	FAssert(iChange == 1 || iChange == -1);
	FAssert(kIarray.getDimentions() == 3 || (kIarray.getDimentions() == 2 && kIarray.isBool()));

	bool bChanged = false;

	for (int i = 0; i < kIarray.getLength(); i++)
	{
		int iIndex = kIarray.getWithTypeWithConversion(getType(), i, 0, pCivInfo);
		if (iIndex >= 0 && iIndex < m_iLength)
		{
			int iSubIndex = kIarray.getWithTypeWithConversion(getSubType(), i, 1, pCivInfo);
			if (iSubIndex >= 0 && iIndex < m_iSubLength)
			{
				allocate();
				int iArrayIndex = (iIndex * m_iLength) + iSubIndex;
				bool bBefore = m_aArray[iArrayIndex] > 0;
				T tModifier = (kIarray.getDimentions() == 3 ? kIarray.getWithTypeWithConversion(kIarray.getType(2), i, 2, pCivInfo) : 1) * iChange;
				m_aArray[iArrayIndex] += tModifier;
				bool bAfter = m_aArray[iArrayIndex] > 0;
				bChanged |= !bBefore != !bAfter;
			}
		}
	}

	return bChanged;
}

template<class T>
void CacheArray2D<T>::addCacheAccumulative(int iChange, const InfoArrayBase& kIarray, const CvCivilizationInfo *pCivInfo)
{
	FAssert(iChange == 1 || iChange == -1);
	FAssert(kIarray.getDimentions() == 2);

	for (int i = 0; i < kIarray.getLength(); i++)
	{
		int iIndex = kIarray.getWithTypeWithConversion(getType(), i, 0, pCivInfo);
		if (iIndex >= 0 && iIndex < m_iLength)
		{
			int iSubIndex = kIarray.getWithTypeWithConversion(getSubType(), i, 1, pCivInfo);
			if (iSubIndex >= 0 && iIndex < m_iSubLength)
			{
				allocate();
				for (int i = iSubIndex; i < m_iSubLength; i++)
				{
					int iArrayIndex = (iIndex * m_iLength) + iSubIndex;
					m_aArray[iArrayIndex] += iChange;
					FAssert(m_aArray[iArrayIndex] >= 0);
				}
			}
		}
	}
}

template<class T>
void CacheArray2D<T>::allocate()
{
	if (m_aArray == NULL || m_iLength > 0 || m_iSubLength > 0)
	{
		int iLength = m_iLength * m_iSubLength;
		m_aArray = new T [iLength];
	}
}

template<class T>
void CacheArray2D<T>::reset()
{
	SAFE_DELETE_ARRAY(m_aArray);
}

template<class T>
void CacheArray2D<T>::assign(T iNewValue, int iIndex = -1, int iSubIndex = -1)
{
	allocate();

	if (iIndex != -1 && iSubIndex != -1)
	{
		int iArrayIndex = (iIndex * m_iLength) + iSubIndex;
		m_aArray[iArrayIndex] = iNewValue;
		return;
	}

	int iLength = m_iLength * m_iSubLength;
	for (int i = 0; i < iLength; i++)
	{
		m_aArray[i] = iNewValue;
	}
}


// tell the compile which template types to compile for

// IMPORTANT: do not make one with <bool>
// Use BoolArray instead

template class JustInTimeArray <int>;
template class JustInTimeArray <unsigned short>;
template class JustInTimeArray <short>;
template class JustInTimeArray <char>;

// compile 2D JIT arrays

// compile 2D cache arrays
template class CacheArray2D <char>;
