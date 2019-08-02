#include "CvGameCoreDLL.h"

#include "InfoCacheArray.h"

InfoCacheArray::InfoCacheArray()
	: m_pInfo(NULL)
	, m_iLength(0)
{
}

InfoCacheArray::~InfoCacheArray()
{
	SAFE_DELETE_ARRAY(m_pInfo);
}

void InfoCacheArray::assign(const BoolArray& kArray)
{
	SAFE_DELETE_ARRAY(m_pInfo);
	m_iLength = kArray.getNumTrueElements();
	if (m_iLength == 0)
	{
		return;
	}
	m_pInfo = new const CvInfoBase*[m_iLength];
	
	int iCounter = 0;
	for (int i = 0; i < kArray.length() && iCounter < m_iLength; ++i)
	{
		if (kArray.get(i))
		{
			m_pInfo[iCounter] = getBaseInfo(kArray.getType(), i);
			++iCounter;
		}
	}
}

template<typename T>
void InfoCacheArray::assign(const JustInTimeArray<T>& kArray)
{
	SAFE_DELETE_ARRAY(m_pInfo);
	m_iLength = kArray.getPositiveCount();
	if (m_iLength == 0)
	{
		return;
	}
	m_pInfo = new const CvInfoBase*[m_iLength];
	
	int iCounter = 0;
	for (int i = 0; i < kArray.length() && iCounter < m_iLength; ++i)
	{
		if (kArray.get(i) > 0)
		{
			m_pInfo[iCounter] = getBaseInfo(kArray.getType(), i);
			++iCounter;
		}
	}
}

void ProfessionInfoArray::assign(const BoolArray& kArray)
{
	FAssert(kArray.getType() == JIT_ARRAY_PROFESSION);
	InfoCacheArray::assign(kArray);
}

void PromotionInfoArray::assign(const BoolArray& kArray)
{
	FAssert(kArray.getType() == JIT_ARRAY_PROMOTION);
	InfoCacheArray::assign(kArray);
}

void YieldInfoArray::assign(const BoolArray& kArray)
{
	FAssert(kArray.getType() == JIT_ARRAY_YIELD);
	InfoCacheArray::assign(kArray);
}

// tell the compiler which templates for JustInTimeArray it should compile this file for
// the function itself is bogus and should never be called
// hopefully the linker will optimize it away
void InfoCacheArrayTemplate()
{
	FAssert(false);

	// types to assign
	YieldArray<int> int_array;
	YieldArray<unsigned short> ushort_array;
	YieldArray<char> char_array;

	// assign all of them to base
	YieldInfoArray base;
	base.assign(int_array);
	base.assign(ushort_array);
	base.assign(char_array);
}
