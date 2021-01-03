#include "CvGameCoreDLL.h"
#include "CyData.h"

CyInfoArray::CyInfoArray()
	: m_Array(JIT_ARRAY_NO_TYPE, JIT_ARRAY_NO_TYPE, JIT_ARRAY_NO_TYPE, JIT_ARRAY_NO_TYPE)
{
}

CyInfoArray::CyInfoArray(const InfoArrayMod& parent)
	: m_Array(parent.getType(0), parent.getType(1), parent.getType(2), parent.getType(3))
{
	m_Array = parent;
}

CyInfoArray::CyInfoArray(const BoolArray& BArray)
	: m_Array(BArray.getType(), JIT_ARRAY_NO_TYPE, JIT_ARRAY_NO_TYPE, JIT_ARRAY_NO_TYPE)
{
	m_Array.assign(&BArray);
}

int CyInfoArray::get(int iIndex) const
{
	return getSub(iIndex, 0);
}

int CyInfoArray::get1(int iIndex) const
{
	return getSub(iIndex, 1);
}

int CyInfoArray::get2(int iIndex) const
{
	return getSub(iIndex, 2);
}

int CyInfoArray::get3(int iIndex) const
{
	return getSub(iIndex, 3);
}

int CyInfoArray::getSub(int iIndex, int iSubIndex) const
{
	return m_Array.pyGet(iIndex, iSubIndex);
}

int CyInfoArray::getLength() const
{
	return m_Array.getLength();
}

int CyInfoArray::getDimentions() const
{
	return m_Array.getDimentions();
}

JITarrayTypes CyInfoArray::getType(int i) const
{
	return m_Array.getType(i);
}
