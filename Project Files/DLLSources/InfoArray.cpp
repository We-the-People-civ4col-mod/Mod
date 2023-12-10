#include "CvGameCoreDLL.h"
#include "CvXMLLoadUtility.h"
#include "CvDLLXMLIFaceBase.h"

#include "InfoArray.h"

// avoid magic numbers
// also placed in one location for consistency
const int iMAX_CHAR = MAX_CHAR;
const int iMIN_CHAR = MIN_CHAR;
const int iCHAR_SHIFT_OFFSET = 127;
const int iMAX_CHAR_SHIFT = iMAX_CHAR + iCHAR_SHIFT_OFFSET;
const int iMIN_CHAR_SHIFTED = -1;

// ensure that one byte is used and no more
BOOST_STATIC_ASSERT(iMAX_CHAR - iMIN_CHAR == 255);
BOOST_STATIC_ASSERT(iMAX_CHAR_SHIFT - iMIN_CHAR_SHIFTED == 255);

// bogus default constructor needed by python interface
/*
InfoArray::InfoArray()
	: m_iLength(0)
	, m_iNumDimentions(0)
	, m_pArray(NULL)
	, m_aiTypes(0)
{
	for (int i = 3; i >= 0; --i)
	{
		JITarrayTypes eType = JIT_ARRAY_NO_TYPE;
		FAssertMsg(eType >= 0 && eType <= 0xFF, "Argument too big to fit in a byte")
		m_aiTypes = m_aiTypes << 8;
		m_aiTypes |= eType & 0xFF;
	}
}
*/

InfoArrayBase::InfoArrayBase(JITarrayTypes eType0, JITarrayTypes eType1, JITarrayTypes eType2, JITarrayTypes eType3)
	: m_iLength(0)
	, m_iNumDimentions(0)
	, m_eMemoryLayout(INFO_MEM_DYNAMIC_SHORT)
	, m_pArrayShort(NULL)
	, m_aiTypes(0)
{
	std::vector<JITarrayTypes> aTypes;

	aTypes.push_back(eType0);
	aTypes.push_back(eType1);
	aTypes.push_back(eType2);
	aTypes.push_back(eType3);

	for (int i = aTypes.size() - 1; i >= 0; --i)
	{
		JITarrayTypes eType = aTypes[i];
		FAssertMsg(eType >= 0 && eType <= 0xFF, "Argument too big to fit in a byte")
		m_aiTypes = m_aiTypes << 8;
		m_aiTypes |= eType & 0xFF;
	}

	m_iNumDimentions = 4;

	for (int i = 0; i < 4; ++i)
	{
		if (getType(i) == JIT_ARRAY_NO_TYPE)
		{
			m_iNumDimentions = i;
			break;
		}
	}
	FAssert(m_iNumDimentions > 0);
}

InfoArrayBase::~InfoArrayBase()
{
	_Free();
}

///
/// read functions
/// always available and will not change the member variables at all
///

int InfoArrayBase::getInternal(int iIndex, int iDimention) const
{
	if (iIndex >= m_iLength)
	{
		return -1;
	}

	FAssert(iIndex >= 0);
	FAssert(iDimention >= 0 && iDimention < m_iNumDimentions);

	unsigned int iArrayIndex = (iIndex * m_iNumDimentions) + iDimention;

	switch (m_eMemoryLayout)
	{
	case INFO_MEM_DYNAMIC_SHORT:
		return m_pArrayShort[iArrayIndex];
	case INFO_MEM_DYNAMIC_BYTE:
		return m_pArrayChar[iArrayIndex];
	case INFO_MEM_DYNAMIC_BYTE_SHIFTED:
		return m_pArrayChar[iArrayIndex] + iCHAR_SHIFT_OFFSET;
	case INFO_MEM_STATIC_SHORT:
		return m_aiStaticArrayShort[iArrayIndex];
	case INFO_MEM_STATIC_BYTE:
		return m_aiStaticArrayChar[iArrayIndex];
	case INFO_MEM_STATIC_BYTE_SHIFTED:
		return m_aiStaticArrayChar[iArrayIndex] + iCHAR_SHIFT_OFFSET;
	}
	FAssertMsg(false, "Missing case");
	return -1; 
}

int InfoArrayBase::_getIndexOf(int iValue, int iDim) const
{
	for (int i = 0; i < m_iLength; ++i)
	{
		if (getInternal(i, iDim) == iValue)
		{
			return i;
		}
	}
	return -1;
}

void InfoArrayBase::_Free()
{
	switch (m_eMemoryLayout)
	{
	case INFO_MEM_DYNAMIC_SHORT:
		SAFE_DELETE_ARRAY(m_pArrayShort);
		break;
	case INFO_MEM_DYNAMIC_BYTE:
	case INFO_MEM_DYNAMIC_BYTE_SHIFTED:
		SAFE_DELETE_ARRAY(m_pArrayChar);
		break;
	}
	m_eMemoryLayout = INFO_MEM_DYNAMIC_SHORT;
	m_pArrayShort = NULL;
}

int InfoArrayBase::getIndex(int iIndex) const
{
	FAssert(m_iNumDimentions <= 2);
	for (int i = 0; i < m_iLength; i++)
	{
		if (getInternal(i) == iIndex)
		{
			return m_iNumDimentions == 2 ? getInternal(i, 1) : 1;
		}
	}
	return 0;
}

int InfoArrayBase::getIndex(int iIndex, int iSubIndex) const
{
	FAssert(m_iNumDimentions == 3);
	for (int i = 0; i < m_iLength; i++)
	{
		if (getInternal(i) == iIndex && getInternal(i,1) == iSubIndex)
		{
			return getInternal(i, 2);
		}
	}
	return 0;
}

int InfoArrayBase::getLength() const
{
	return m_iLength;
}

int InfoArrayBase::getDimentions() const
{
	return m_iNumDimentions;
}

bool InfoArrayBase::isBool() const
{
	return getType(getDimentions() - 1) != JIT_ARRAY_NO_TYPE;
}

int InfoArrayBase::getMin(int iDimention) const
{
	int iValue = MAX_INT;

	for (int i = 0; i < m_iLength; ++i)
	{
		int iLoopValue = getInternal(i, iDimention);
		if (iLoopValue < iValue)
		{
			iValue = iLoopValue;
		}
	}

	return iValue;
}

int InfoArrayBase::getMax(int iDimention) const
{
	int iValue = MIN_INT;

	for (int i = 0; i < m_iLength; ++i)
	{
		int iLoopValue = getInternal(i, iDimention);
		if (iLoopValue > iValue)
		{
			iValue = iLoopValue;
		}
	}

	return iValue;
}

bool InfoArrayBase::areIndexesValid(int iMax, int iDimention, int iMin) const
{
	for (int i = 0; i < m_iLength; ++i)
	{
		int iLoopValue = getInternal(i, iDimention);
		if (iLoopValue >= iMax || iLoopValue < iMin)
		{
			return false;
		}
	}

	return true;
}

int InfoArrayBase::getWithTypeWithConversion(JITarrayTypes eType, int iIndex, int iTokenIndex, const CvCivilizationInfo *pCivInfo) const
{
	if (eType == JIT_ARRAY_BUILDING && getType(iTokenIndex) == JIT_ARRAY_BUILDINGCLASS)
	{
		FAssertMsg(pCivInfo != NULL, "InfoArray::getWithType called without pCivInfo argument when in index conversion mode");
		int iValue = getInternal(iIndex, iTokenIndex);
		if (iValue >= 0 && iValue < GC.getNumBuildingClassInfos())
		{
			return pCivInfo->getCivilizationBuildings(iValue);
		}
	}
	else if (eType == JIT_ARRAY_UNIT && getType(iTokenIndex) == JIT_ARRAY_UNITCLASS)
	{
		FAssertMsg(pCivInfo != NULL, "InfoArray::getWithType called without pCivInfo argument when in index conversion mode");
		int iValue = getInternal(iIndex, iTokenIndex);
		if (iValue >= 0 && iValue < GC.getNumUnitClassInfos())
		{
			return pCivInfo->getCivilizationUnits(iValue);
		}
	}
	else
	{
		return getWithType(eType, iIndex, iTokenIndex);
	}

	return -1;
}

int InfoArrayBase::getWithType(JITarrayTypes eType, int iIndex, int iTokenIndex) const
{
	FAssert(GetBaseType(eType) == getType(iTokenIndex) || (eType >= NUM_JITarrayTypes && getType(iTokenIndex) >= NUM_JITarrayTypes));
	return getInternal(iIndex, iTokenIndex);
}

///
/// operator overload
///

InfoArrayBase& InfoArrayBase::operator=(const InfoArrayBase &rhs)
{
	if (this != &rhs)
	{
		FAssert(this->m_aiTypes == rhs.m_aiTypes);
		_Free();
		m_iLength = rhs.m_iLength;
		m_eMemoryLayout = rhs.m_eMemoryLayout;

		if (m_iLength > 0)
		{
			const int iLength = m_iLength * getDimentions();

			switch (m_eMemoryLayout)
			{
			case INFO_MEM_DYNAMIC_SHORT:
				m_pArrayShort = new short[iLength];
				memcpy(m_pArrayShort, rhs.m_pArrayShort, iLength * sizeof(short));
				break;
			case INFO_MEM_DYNAMIC_BYTE:
			case INFO_MEM_DYNAMIC_BYTE_SHIFTED:
				m_pArrayChar = new char[iLength];
				memcpy(m_pArrayChar, rhs.m_pArrayChar, iLength * sizeof(char));
				break;
			case INFO_MEM_STATIC_SHORT:
				for (int i = 0; i < iLength; ++i)
				{
					m_aiStaticArrayShort[i] = rhs.m_aiStaticArrayShort[i];
				}
				break;
			case INFO_MEM_STATIC_BYTE:
			case INFO_MEM_STATIC_BYTE_SHIFTED:
				for (int i = 0; i < iLength; ++i)
				{
					m_aiStaticArrayChar[i] = rhs.m_aiStaticArrayChar[i];
				}
				break;
			}
		}
	}

	return *this;
}

///
/// write functions
/// Only available to InfoArrayMod and they will completely overwrite the existing content
///

void InfoArrayBase::readRecursive(CvXMLLoadUtility* pXML, int& iIndex, std::vector<short>& aArray, std::vector<short>& aIndex, int iLevel, const char *sTagName, const char* szType)
{
	int iIndexStart = iIndex;
	do
	{
		pXML->SkipToNextVal();

		if (iIndex == -1)
		{
			iIndex = iIndexStart;
		}

		if (gDLL->getXMLIFace()->SetToChild(pXML->GetXML()))
		{
			readRecursive(pXML, iIndex, aArray, aIndex, iLevel+1, sTagName, szType);
			gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		}
		else	
		{
			int iValue = 0;
			CvString szTextVal;
			pXML->GetXmlVal(szTextVal);

			JITarrayTypes eType = getType(iIndex);

			if (eType < JIT_ARRAY_NO_TYPE || eType >= MAX_JIT_ARRAY_VALUE)
			{
				// read type and convert to int
				iValue = getIndexOfType(eType, szTextVal);
				if (iValue < 0)
				{
					CvWString wideType(szType);
					CvWString wideName(sTagName);
					CvWString wideValue(szTextVal.c_str());
					CvWString wideArrayName(getArrayName(eType));
					CvString szDesc(gDLL->getText("TXT_KEY_ERROR_XML_WRONG_TYPE_STRING", wideType.c_str(), wideName.c_str(), wideValue.c_str(), wideArrayName.c_str()).c_str());
					CvString szTitle(gDLL->getText("TXT_KEY_ERROR_XML_TITLE"));

					gDLL->MessageBox(szDesc.c_str(), szTitle.c_str());
				}
			}
			else
			{
				// read int
				// for some reason GetXmlVal(int) sets garbage data
				// read string and convert that to int to get the correct result
				if (eType == JIT_ARRAY_MODIFIER_FLOAT || eType == JIT_ARRAY_FLOAT)
				{
					// we are loading a float. Multiply by 100 and store as an int
					// do the conversion in ASCII to avoid possible float rounding issues
					CvString szInt;
					int iDecimals = MIN_INT;
					for (int i = 0; i < szTextVal.GetLength(); ++i)
					{
						char cChar = szTextVal.GetCString()[i];

						if (iDecimals < 0 && cChar == '.')
						{
							iDecimals = 0;
							continue;
						}
						szInt += cChar;
						iDecimals++;
						if (iDecimals == 2)
						{
							break;
						}
					}
					for (; iDecimals < 2; ++iDecimals)
					{
						szInt += '0';
					}
					iValue = std::atoi(szInt.c_str());
				}
				else
				{
					iValue = std::atoi(szTextVal.c_str());
					if (eType == JIT_ARRAY_UNSIGNED_INT && iValue < 0)
					{
						CvWString wideType(szType);
						CvWString wideName(sTagName);
						CvString szDesc(gDLL->getText("TXT_KEY_ERROR_XML_UNSIGNED_NEGATIVE_NUMBER", wideType.c_str(), wideName.c_str(), iValue));
						CvString szTitle(gDLL->getText("TXT_KEY_ERROR_XML_TITLE"));

						gDLL->MessageBox(szDesc.c_str(), szTitle.c_str());
					}
				}
			}
			aIndex[iIndex] = iValue;
			iIndex++;
			if (iIndex == m_iNumDimentions)
			{
				iIndex = -1;
				for (int i = 0; i < m_iNumDimentions; i++)
				{
					aArray.push_back(aIndex[i]);
				}
			}
		}
		// 1D (bool) arrays should only read the first tag at level 2 for backward compability
	} while (gDLL->getXMLIFace()->NextSibling(pXML->GetXML()) && (m_iNumDimentions > 1 || iLevel < 1));
}

void InfoArrayBase::read(CvXMLLoadUtility* pXML, const char* szType, const char *sTagName)
{
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),sTagName))
	{
		if (gDLL->getXMLIFace()->SetToChild(pXML->GetXML()))
		{
			std::vector<short> aArray;
			std::vector<short> aIndex;
			for (int i = 0; i < m_iNumDimentions; i++)
			{
				aIndex.push_back(0);
			}

			int iIndex = 0;
			readRecursive(pXML, iIndex, aArray, aIndex, 0, sTagName, szType);
			
			assignFromVector(aArray);

			gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
}

void InfoArrayBase::assignFromVector(const std::vector<short> vec)
{
	_Free();

	const int iArrayLength = vec.size(); // local copy rather than the same function call over and over

	m_iLength = iArrayLength / m_iNumDimentions;

	int iMin = 0;
	int iMax = 0;

	for (unsigned int i = 0; i < vec.size(); ++i)
	{
		const short value = vec[i];
		if (iMin > value) iMin = value;
		if (iMax < value) iMax = value;
	}

	if (iMin >= iMIN_CHAR && iMax <= iMAX_CHAR)
	{
		if (iArrayLength <= 4)
		{
			m_eMemoryLayout = INFO_MEM_STATIC_BYTE;
			for (int i = 0; i < iArrayLength; ++i)
			{
				m_aiStaticArrayChar[i] = (char)vec[i];
			}
		}
		else
		{
			m_eMemoryLayout = INFO_MEM_DYNAMIC_BYTE;
			m_pArrayChar = new char[iArrayLength];
			for (int i = 0; i < iArrayLength; ++i)
			{
				m_pArrayChar[i] = (char)vec[i];
			}
		}
	}
	else if (iMin >= -1 && iMax <= iMAX_CHAR_SHIFT)
	{
		if (iArrayLength <= 4)
		{
			m_eMemoryLayout = INFO_MEM_STATIC_BYTE_SHIFTED;
			for (int i = 0; i < iArrayLength; ++i)
			{
				m_aiStaticArrayChar[i] = vec[i] - iCHAR_SHIFT_OFFSET;
			}
		}
		else
		{
			m_eMemoryLayout = INFO_MEM_DYNAMIC_BYTE_SHIFTED;
			m_pArrayChar = new char[iArrayLength];
			for (int i = 0; i < iArrayLength; ++i)
			{
				m_pArrayChar[i] = vec[i] - iCHAR_SHIFT_OFFSET;
			}
		}
	}
	else
	{
		if (iArrayLength <= 2)
		{
			m_eMemoryLayout = INFO_MEM_STATIC_SHORT;
			for (int i = 0; i < iArrayLength; ++i)
			{
				m_aiStaticArrayShort[i] = vec[i];
			}
		}
		else
		{
			m_eMemoryLayout = INFO_MEM_DYNAMIC_SHORT;
			m_pArrayShort = new short[iArrayLength];
			for (int i = 0; i < iArrayLength; ++i)
			{
				m_pArrayShort[i] = vec[i];
			}
		}
	}
}
