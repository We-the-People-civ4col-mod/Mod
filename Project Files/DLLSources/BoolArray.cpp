// BoolArray.cpp

// usage guide is written in BoolArray.h
// file written by Nightinggale

#include "CvGameCoreDLL.h"
#include "CvXMLLoadUtility.h"
#include "CvDLLXMLIFaceBase.h"
#include "CvGameAI.h"
#include "CvInitCore.h"

#define BOOL_BLOCK ( iIndex >> 5 )
#define BOOL_INDEX ( iIndex & 0x1F )

BoolArray::BoolArray(JITarrayTypes eType, bool bDefault)
: m_iArray(NULL)
, m_iType(eType)
, m_iLength(getArrayLength(eType))
, m_bDefault(bDefault)
{}

BoolArray::~BoolArray()
{
	SAFE_DELETE_ARRAY(m_iArray);
}

void BoolArray::reset()
{
	// only reset() and deconstructor should free memory as that can help debugging
	SAFE_DELETE_ARRAY(m_iArray);
}


bool BoolArray::get(int iIndex) const
{
	FAssert(iIndex >= 0);
	FAssert(iIndex < m_iLength);
	return m_iArray ? HasBit(m_iArray[BOOL_BLOCK], BOOL_INDEX) : m_bDefault;
}

void BoolArray::set(bool bValue, int iIndex)
{
	FAssert(iIndex >= 0);
	FAssert(iIndex < m_iLength);

	if (m_iArray == NULL)
	{
		if (!bValue == !m_bDefault)
		{
			// no need to allocate memory to assign a default value
			return;
		}
		int iLength = (m_iLength + 31) / 32;
		m_iArray = new unsigned int[iLength];
		for (int i = 0; i < iLength; i++)
		{
			m_iArray[i] = m_bDefault ? MAX_UNSIGNED_INT : 0;
		}
	}
	
	if (bValue)
	{
		m_iArray[BOOL_BLOCK] |= SETBIT(BOOL_INDEX);
	}
	else
	{
		m_iArray[BOOL_BLOCK] &= ~SETBIT(BOOL_INDEX);
	}
	
	if (!bValue != !m_bDefault)
	{
		hasContent();
	}
}

void BoolArray::safeSet(bool bValue, int iIndex)
{
	if (iIndex >= 0 && iIndex < length())
	{
		set(bValue, iIndex);
	}
}

bool BoolArray::hasContent(bool bRelease)
{
	if (m_iArray == NULL)
	{
		return false;
	}

	int iLength = (m_iLength + 31) / 32;
	for (int i = 0; i < iLength; i++)
	{
		if (m_bDefault)
		{
			if (~m_iArray[i])
			{
				return true;
			}
		}
		else
		{
			if (m_iArray[i])
			{
				return true;
			}
		}
	}

	reset();
	return false;
};

int BoolArray::getNumUsedElements() const
{
	int iMax = 0;
	if (isAllocated())
	{
		for (unsigned int iIndex = 0; iIndex < m_iLength; iIndex++)
		{
			if (get(iIndex) != m_bDefault)
			{
				iMax = 1 + iIndex;
			}
		}
	}
	return iMax;
}

int BoolArray::getNumTrueElements() const
{
	if (!isAllocated())
	{
		return m_bDefault ? m_iLength : 0;
	}

	int iCount = 0;
	for (int i = 0; i < m_iLength; ++i)
	{
		if (get(i))
		{
			++iCount;
		}
	}
	return iCount;
}

bool BoolArray::add(const InfoArray& kIarray)
{
	bool bChanged = false;
	int iLength = kIarray.getLength();

	FAssert(kIarray.getDimentions() == 1);

	for (int i = 0; i < iLength; i++)
	{
		int iIndex = kIarray.getWithType(getType(), i, 0);
		if (!get(iIndex))
		{
			set(true, iIndex);
			bChanged = true;
		}
	}

	return bChanged;
}

void BoolArray::assign(const InfoArray& kIarray)
{
	FAssert(m_bDefault == 0);
	// reset and then add results in the same as assign. No need to write multiple functions to do the same
	reset();
	add(kIarray);
}

void BoolArray::read(FDataStreamBase* pStream, bool bEnable)
{
	// always reset the array even if load is disabled
	reset();

	if (!bEnable)
	{
		return;
	}

	for (int i = 0; i < length(); ++i)
	{
		bool bBuffer = false;
		pStream->Read(&bBuffer);
		set(bBuffer, i);
	}
}

void BoolArray::write(FDataStreamBase* pStream, bool bEnable)
{
	if (!bEnable)
	{
		return;
	}

	for (int i = 0; i < length(); ++i)
	{
		pStream->Write(get(i));
	}
}

void BoolArray::Read(FDataStreamBase* pStream)
{
	reset();

	unsigned short iNumElements = 0;
	pStream->Read(&iNumElements);

	if (iNumElements > 0)
	{
		// first write a non-default value to the first element in the array
		// this will ensure the array is allocated
		set(!m_bDefault, 0);

		int iNumBlocks = (iNumElements + 0x1F) >> 5;
		pStream->Read(iNumBlocks, m_iArray);
	}
}

void BoolArray::Write(FDataStreamBase* pStream)
{
	unsigned short iNumElements = getNumUsedElements();

	pStream->Write(iNumElements);

	if (iNumElements == 0)
	{
		reset();
	}
	else
	{
		int iNumBlocks = (iNumElements + 0x1F) >> 5;
		pStream->Write(iNumBlocks, m_iArray);
	}
}

void BoolArray::read(CvXMLLoadUtility* pXML, const char* sType, const char* sTag)
{
	// BoolArray doesn't have any xml reading code
	// Instead make a temp InfoArray and rely on the error checking code in that one

	// first get the type, which the InfoArray is supposed to use
	JITarrayTypes eType = (JITarrayTypes)m_iType;
	InfoArrayMod array(eType);
	array.read(pXML, sType, sTag);

	// Transfer the InfoArray into the BoolArray
	assign(array);
}

BoolArray& BoolArray::operator=(const BoolArray &rhs)
{
	FAssert(m_iLength == rhs.m_iLength);
	FAssert(m_iType == rhs.m_iType);

	// TODO use memcpy for faster execution
	// Current code is faster to code because it relies on reusing already working code
	for (int i = 0; i < m_iLength; i++)
	{
		this->set(rhs.get(i), i);
	}

	this->hasContent(); // clear array if possible

	return *this;
}
