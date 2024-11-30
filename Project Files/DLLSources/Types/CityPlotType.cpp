#include "../CvGameCoreDLL.h"
#include "CityPlotType.h"

CityPlotType::CityPlotType()
	: m_Value(CityPlotType::NONE)
{
}

CityPlotType::CityPlotType(CityPlotType::types val)
	: m_Value(val)
{
}

CityPlotType::CityPlotType(CityPlotTypes val)
	: m_Value(static_cast<CityPlotType::types>(val))
{
}

void CityPlotType::assignFromInt(int iNewValue)
{
	m_Value = static_cast<types>(iNewValue);
}

int CityPlotType::toInt() const
{
	return m_Value;
}

const CityPlotType::types CityPlotType::value() const
{
	return m_Value;
}

CityPlotType::operator const CityPlotType::types() const
{
	return m_Value;
}

CityPlotType::operator const CityPlotTypes() const
{
	return static_cast<CityPlotTypes>(m_Value);
}

bool CityPlotType::isInRange(types eValue)
{
	return eValue > NONE && eValue < NUM_ENTRIES;
}

bool CityPlotType::isInRange() const
{
	return m_Value > NONE && m_Value < NUM_ENTRIES;
}

bool CityPlotType::next()
{
	m_Value = static_cast<types>(m_Value+1);
	return m_Value < NUM_ENTRIES;
}

void CityPlotType::setupAuto()
{
}

void CityPlotType::setupCustom()
{
}

