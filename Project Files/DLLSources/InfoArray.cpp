#include "CvGameCoreDLL.h"
#include "CvXMLLoadUtility.h"
#include "CvDLLXMLIFaceBase.h"

#include "InfoArray.h"

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

InfoArray::InfoArray(JITarrayTypes eType0, JITarrayTypes eType1, JITarrayTypes eType2, JITarrayTypes eType3)
	: m_iLength(0)
	, m_iNumDimentions(0)
	, m_pArray(NULL)
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

InfoArray::~InfoArray()
{
	SAFE_DELETE_ARRAY(m_pArray);
}

///
/// read functions
/// always available and will not change the member variables at all
///

int InfoArray::getInternal(int iIndex, int iDimention) const
{
	if (iIndex >= m_iLength)
	{
		return -1;
	}

	FAssert(iIndex >= 0);
	FAssert(iDimention >= 0 && iDimention < m_iNumDimentions);
	FAssert(m_pArray != NULL);

	unsigned int iArrayIndex = (iIndex * m_iNumDimentions) + iDimention;

	return m_pArray[iArrayIndex];
}

int InfoArray::getIndex(int iIndex) const
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

int InfoArray::getIndex(int iIndex, int iSubIndex) const
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

int InfoArray::getLength() const
{
	return m_iLength;
}

int InfoArray::getDimentions() const
{
	return m_iNumDimentions;
}

bool InfoArray::isBool() const
{
	return getType(getDimentions() - 1) != JIT_ARRAY_NO_TYPE;
}

int InfoArray::getMin(int iDimention) const
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

int InfoArray::getMax(int iDimention) const
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

bool InfoArray::areIndexesValid(int iMax, int iDimention, int iMin) const
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

///
/// operator overload
///

InfoArray& InfoArray::operator=(const InfoArray &rhs)
{
	if (this != &rhs)
	{
		FAssert(this->m_aiTypes == rhs.m_aiTypes);

		this->m_iLength = rhs.m_iLength;

		SAFE_DELETE_ARRAY(this->m_pArray);

		int iLoopLength = m_iLength * m_iNumDimentions;

		if (iLoopLength > 0)
		{
			this->m_pArray = new short [iLoopLength];

			for (int i=0; i < iLoopLength; i++)
			{
				this->m_pArray[iLoopLength] = rhs.m_pArray[iLoopLength];
			}
		}
	}

	return *this;
}

///
/// write functions
/// Only available to InfoArrayMod and they will completely overwrite the existing content
///

void InfoArrayMod::readRecursive(CvXMLLoadUtility* pXML, int& iIndex, std::vector<short>& aArray, std::vector<short>& aIndex, int iLevel, const char *sTagName, const char* szType)
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

			if (eType < (NUM_JITarrayTypes*2) || eType >= MAX_JIT_ARRAY_VALUE)
			{
				// read type and convert to int
				iValue = getIndexForType(eType, szTextVal);
				FAssertMsg(iValue != -1, CvString::format("%s tag %s: the string %s is read as %s, but it isn't present in that XML file", szType, sTagName, szTextVal.c_str(), getArrayName(eType)).c_str());
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
					FAssertMsg(eType != JIT_ARRAY_UNSIGNED_INT || iValue >= 0, CvString::format("%s tag %s: negative numbers not allowed", szType, sTagName).c_str());
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

void InfoArrayMod::read(CvXMLLoadUtility* pXML, const char* szType, const char *sTagName)
{
	SAFE_DELETE_ARRAY(m_pArray);
	m_iLength = 0;

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
			
			int iArrayLength = aArray.size(); // local copy rather than the same function call over and over

			m_iLength = iArrayLength / m_iNumDimentions;

			if (m_iLength > 0)
			{
				m_pArray = new short[iArrayLength];
				for (int i = 0; i < iArrayLength; i++)
				{
					m_pArray[i] = aArray[i];
				}
			}

			gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
}

void InfoArrayMod::assign(const BoolArray* pBArray)
{
	FAssert(m_iNumDimentions == 1);
	SAFE_DELETE_ARRAY(m_pArray);
	m_iLength = 0;
	for (int i = 0; i < pBArray->length(); i++)
	{
		if (pBArray->get(i))
		{
			m_iLength++;
		}
	}
	

	if (m_iLength == 0)
	{
		return;
	}

	m_pArray = new short[m_iLength];

	int iCounter = 0;

	for (int i = 0; i < pBArray->length(); i++)
	{
		if (pBArray->get(i))
		{
			m_pArray[iCounter] = i;
			iCounter++;
		}
	}
}

void InfoArrayMod::convertClass(const InfoArray* pInfoArray, const CvCivilizationInfo* pCivInfo)
{
	FAssertMsg(pInfoArray->getType(0) == JIT_ARRAY_BUILDING_CLASS || pInfoArray->getType(0) == JIT_ARRAY_UNIT_CLASS, "InfoArray tries to resolve class in an array without classes");

	JITarrayTypes eType = getType(0);

	FAssertMsg((eType == JIT_ARRAY_BUILDING && pInfoArray->getType(0) == JIT_ARRAY_BUILDING_CLASS) || (eType == JIT_ARRAY_UNIT && pInfoArray->getType(0) == JIT_ARRAY_UNIT_CLASS), "InfoArray types mismatch");
	FAssertMsg(getType(1) == pInfoArray->getType(1) && getType(2) == pInfoArray->getType(2) && getType(3) == pInfoArray->getType(3), "InfoArray types mismatch");

	SAFE_DELETE_ARRAY(m_pArray);
	std::vector<short> aArray;

	if (eType == JIT_ARRAY_BUILDING)
	{
		for (int iI = 0; iI < pInfoArray->getLength(); ++iI)
		{
			BuildingClassTypes eBuildingClass = pInfoArray->getBuildingClass(iI);
			if (pCivInfo != NULL)
			{
				int iBuilding = pCivInfo->getCivilizationBuildings(eBuildingClass);
				if (iBuilding >= 0)
				{
					aArray.push_back(iBuilding);
					for (int i = 1; i < m_iNumDimentions; i++)
					{
						aArray.push_back(pInfoArray->getInternal(iI, i));
					}
				}
			}
			else
			{
				for (BuildingTypes eLoopBuilding = FIRST_BUILDING; eLoopBuilding < GC.getNumBuildingInfos(); ++eLoopBuilding)
				{
					CvBuildingInfo& kInfo = GC.getBuildingInfo(eLoopBuilding);
					if (kInfo.getBuildingClassType() == eBuildingClass)
					{
						aArray.push_back(eLoopBuilding);
						for (int i = 1; i < m_iNumDimentions; i++)
						{
							aArray.push_back(pInfoArray->getInternal(iI, i));
						}
					}
				}
			}
		}
	}
	else if (eType == JIT_ARRAY_UNIT)
	{
		for (int iI = 0; iI < pInfoArray->getLength(); ++iI)
		{
			UnitClassTypes eUnitClass = pInfoArray->getUnitClass(iI);
			if (pCivInfo != NULL)
			{
				int iUnit = pCivInfo->getCivilizationUnits(eUnitClass);
				if (iUnit >= 0)
				{
					aArray.push_back(iUnit);
					for (int i = 1; i < m_iNumDimentions; i++)
					{
						aArray.push_back(pInfoArray->getInternal(iI, i));
					}
				}
			}
			else
			{
				for (UnitTypes eLoopUnit = FIRST_UNIT; eLoopUnit < GC.getNumUnitInfos(); ++eLoopUnit)
				{
					CvUnitInfo& kInfo = GC.getUnitInfo(eLoopUnit);
					if (kInfo.getUnitClassType() == eUnitClass)
					{
						aArray.push_back(eLoopUnit);
						for (int i = 1; i < m_iNumDimentions; i++)
						{
							aArray.push_back(pInfoArray->getInternal(iI, i));
						}
					}
				}
			}
		}
	}
	else
	{
		FAssertMsg(false, "Unimplemented type");
	}

	int iArrayLength = aArray.size(); // local copy rather than the same function call over and over

	m_iLength = iArrayLength / m_iNumDimentions;

	if (m_iLength > 0)
	{
		m_pArray = new short[iArrayLength];
		for (int i = 0; i < iArrayLength; ++i)
		{
			m_pArray[i] = aArray[i];
		}
	}
}
