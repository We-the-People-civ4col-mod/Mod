#include "CvGameCoreDLL.h"
#include "CvTradeRouteGroup.h"
#include "CvTradeRoute.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvSavegame.h"

CvTradeRouteGroup::CvTradeRouteGroup() 
{
	m_Routes.clear();
}

CvTradeRouteGroup::~CvTradeRouteGroup()
{
	m_Routes.clear();
}

int CvTradeRouteGroup::getID() const
{
	return m_iId;
}

void CvTradeRouteGroup::setID(int iId)
{
	m_iId = iId;
}

void CvTradeRouteGroup::addRoute(CvTradeRoute* tradeRoute)
{	

	for (std::vector<CvTradeRoute>::iterator it = m_Routes.begin(); it != m_Routes.end(); ++it)
	{
		CvTradeRoute pTradeRoute = *it;
		if (pTradeRoute.getSourceCity() == tradeRoute->getSourceCity()
			&& pTradeRoute.getDestinationCity() == tradeRoute->getDestinationCity()
			&& pTradeRoute.getYield() == tradeRoute->getYield())
		{
			pTradeRoute.getID();
		}
	}
	
		
	m_Routes.push_back(*tradeRoute);
	
}

void CvTradeRouteGroup::addRoute(const IDInfo& kSource, const IDInfo& kDestination, YieldTypes eYield)
{
	
	CvCity* pSourceCity = ::getCity(kSource);
	
	if (pSourceCity == NULL)
	{
		return;
	}
	

	CvCity* pDestinationCity = ::getCity(kDestination);
	
	if (kSource == kDestination)
	{
		return;
	}

	for (std::vector<CvTradeRoute>::iterator it = m_Routes.begin(); it != m_Routes.end(); ++it)
	{
		CvTradeRoute pTradeRoute = *it;
		if (pTradeRoute.getSourceCity() == kSource
			&& pTradeRoute.getDestinationCity() == kDestination
			&& pTradeRoute.getYield() == eYield)
		{
			pTradeRoute.getID();
		}
	}

	CvTradeRoute pTradeRoute = CvTradeRoute();

	pTradeRoute.init(kSource, kDestination, eYield);	
	m_Routes.push_back(pTradeRoute);
	
	
}

void CvTradeRouteGroup::removeRoute(int routeId)
{
	for (std::vector<CvTradeRoute>::iterator it = m_Routes.begin(); it != m_Routes.end(); )
	{
		if((*it).getID() == routeId)
		{
			it = m_Routes.erase(it);
		}
		else 
			++it;
	}	
}

void CvTradeRouteGroup::clearRoutes()
{
	m_Routes.clear();
}

int CvTradeRouteGroup::getRouteCount() const
{
	return m_Routes.size();
}

CvTradeRoute* CvTradeRouteGroup::getRouteByIndex(int iIdx)
{
	return &(m_Routes.at(iIdx));
}

const CvWString CvTradeRouteGroup::getName(uint uiForm) const
{
	return gDLL->getObjectText(m_sName, uiForm, true);
}

void CvTradeRouteGroup::setName(const wchar* szNewValue)
{
	CvWString szName(szNewValue);
	gDLL->stripSpecialCharacters(szName);
	if (!isEmpty(szName))
		m_sName = szName;
}
