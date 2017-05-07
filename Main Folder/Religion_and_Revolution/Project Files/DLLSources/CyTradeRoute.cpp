#include "CvGameCoreDLL.h"
#include "CyTradeRoute.h"
#include "CvTradeRoute.h"
#include "CyCity.h"


CyTradeRoute::CyTradeRoute() :
m_pTradeRoute(NULL)
{
}

CyTradeRoute::CyTradeRoute(CvTradeRoute* pTradeRoute) :
	m_pTradeRoute(pTradeRoute)
{
}

int CyTradeRoute::getID() const
{
	return m_pTradeRoute ? m_pTradeRoute->getID() : -1;
}

IDInfo CyTradeRoute::getSourceCity() const
{
	return (m_pTradeRoute ? m_pTradeRoute->getSourceCity() : IDInfo());
}

IDInfo CyTradeRoute::getDestinationCity() const
{
	return (m_pTradeRoute ? m_pTradeRoute->getDestinationCity() : IDInfo());
}

std::wstring CyTradeRoute::getSourceCityNameKey() const
{
	return (m_pTradeRoute ? m_pTradeRoute->getSourceCityNameKey() : L"");
}

std::wstring CyTradeRoute::getDestinationCityNameKey() const
{
	return (m_pTradeRoute ? m_pTradeRoute->getDestinationCityNameKey() : L"");
}

std::wstring CyTradeRoute::getSourceCityName() const
{
	return gDLL->getObjectText(m_pTradeRoute ? m_pTradeRoute->getSourceCityNameKey() : L"", 0);
}

std::wstring CyTradeRoute::getDestinationCityName() const
{
	return gDLL->getObjectText(m_pTradeRoute ? m_pTradeRoute->getDestinationCityNameKey() : L"", 0);
}

int CyTradeRoute::getYield() const
{
	return m_pTradeRoute ? m_pTradeRoute->getYield() : -1;
}
