#include "CvInfoArrayInit.h"
#include "Constants.h"
#include <algorithm>


CvInfoArrayInit::CvInfoArrayInit(const std::vector<int>& vec, char(&m_Array_char)[4], unsigned char(&m_Array_uchar)[4], short(&m_Array_short)[2], char*& m_pArray_char, unsigned char*& m_pArray_uchar, short*& m_pArray_short)
	: m_vec(vec)
	, m_iExtra(0)
{
	int iMax = MIN_INT;
	int iMin = MAX_INT;
	for (unsigned int i = 0; i < vec.size(); ++i)
	{
		const int iTemp = vec[i];
		if (iTemp < iMin) iMin = iTemp;
		if (iTemp > iMax) iMax = iTemp;
	}

	const unsigned int iLength = vec.size();

	if (iMin >= MIN_CHAR && iMax <= MAX_CHAR)
	{
		m_ArrayType = iLength > 6 ? InfoArrayTypes::TYPE_CHAR : InfoArrayTypes::TYPE_POINTER_CHAR;
	}
	else if (iMin >= MIN_UNSIGNED_CHAR && iMax <= MAX_UNSIGNED_CHAR)
	{
		m_ArrayType = iLength > 6 ? InfoArrayTypes::TYPE_UCHAR : InfoArrayTypes::TYPE_POINTER_UCHAR;
	}
	else if (iMin >= -1 && iMax <= 254)
	{
		m_ArrayType = iLength > 6 ? InfoArrayTypes::TYPE_OFFSET_UCHAR : InfoArrayTypes::TYPE_POINTER_OFFSET_UCHAR;
	}
	else
	{
		m_ArrayType = iLength > 3 ? InfoArrayTypes::TYPE_SHORT : InfoArrayTypes::TYPE_POINTER_SHORT;
	}

	switch (m_ArrayType)
	{
	case InfoArrayTypes::TYPE_CHAR:
	{
		const int max = std::min(4U, iLength);
		for (int i = 0; i < max; ++i)
		{
			m_Array_char[i] = vec[i];
		}
		if (iLength == 5)
		{
			m_iExtra = vec[4];
		}
		else if (iLength == 6)
		{
			m_iExtra = vec[5] << 8 | vec[4];
		}
	}
		break;
	case InfoArrayTypes::TYPE_UCHAR:
	{
		const int max = std::min(4U, iLength);
		for (int i = 0; i < max; ++i)
		{
			m_Array_uchar[i] = vec[i];
		}
		if (iLength == 5)
		{
			m_iExtra = vec[4];
		}
		else if (iLength == 6)
		{
			m_iExtra = vec[5] << 8 | vec[4];
		}
	}
		break;
	case InfoArrayTypes::TYPE_SHORT:
	{
		const int max = std::min(2U, iLength);
		for (int i = 0; i < max; ++i)
		{
			m_Array_short[i] = vec[i];
		}
		if (iLength == 3)
		{
			m_iExtra = vec[2];
		}
	}
		break;
	case InfoArrayTypes::TYPE_OFFSET_UCHAR:
	{
		const int max = std::min(4U, iLength);
		for (int i = 0; i < max; ++i)
		{
			m_Array_uchar[i] = vec[i]+1;
		}
		if (iLength == 5)
		{
			m_iExtra = vec[4];
		}
		else if (iLength == 6)
		{
			m_iExtra = (vec[5]+1) << 8 | (vec[4]+1);
		}
	}
		break;
	case InfoArrayTypes::TYPE_POINTER_CHAR:
	{
		for (unsigned int i = 0; i < iLength; ++i)
		{
			m_pArray_char[i] = vec[i];
		}
	}
		break;
	case InfoArrayTypes::TYPE_POINTER_UCHAR:
	{
		for (unsigned int i = 0; i < iLength; ++i)
		{
			m_pArray_uchar[i] = vec[i];
		}
	}
		break;
	case InfoArrayTypes::TYPE_POINTER_SHORT:
	{
		for (unsigned int i = 0; i < iLength; ++i)
		{
			m_pArray_short[i] = vec[i];
		}
	}
		break;
	case InfoArrayTypes::TYPE_POINTER_OFFSET_UCHAR:
	{
		for (unsigned int i = 0; i < iLength; ++i)
		{
			m_pArray_uchar[i] = vec[i]+1;
		}
	}
		break;
	}
}

InfoArrayTypes::ArrayType CvInfoArrayInit::getType() const
{
	return m_ArrayType;
}

int CvInfoArrayInit::getExtra() const
{
	return m_iExtra;
}
