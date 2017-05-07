#include "CvTradeRouteGroup.h"
#include "CvGameCoreDLL.h"
#include "CvTradeRoute.h"
#include "CvDLLInterfaceIFaceBase.h"

CvTradeRouteGroup::CvTradeRouteGroup() 
{
	m_routes.clear();
}

CvTradeRouteGroup::~CvTradeRouteGroup()
{
	m_routes.clear();
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

	for (std::vector<CvTradeRoute>::iterator it = m_routes.begin(); it != m_routes.end(); ++it)
	{
		CvTradeRoute pTradeRoute = *it;
		if (pTradeRoute.getSourceCity() == tradeRoute->getSourceCity()
			&& pTradeRoute.getDestinationCity() == tradeRoute->getDestinationCity()
			&& pTradeRoute.getYield() == tradeRoute->getYield())
		{
			pTradeRoute.getID();
		}
	}
	
		
	m_routes.push_back(*tradeRoute);
	
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

	for (std::vector<CvTradeRoute>::iterator it = m_routes.begin(); it != m_routes.end(); ++it)
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
	m_routes.push_back(pTradeRoute);
	
	
}

void CvTradeRouteGroup::removeRoute(int routeId)
{
	for (std::vector<CvTradeRoute>::iterator it = m_routes.begin(); it != m_routes.end(); )
	{
		if((*it).getID() == routeId)
		{
			it = m_routes.erase(it);
		}
		else 
			++it;
	}	
}

void CvTradeRouteGroup::clearRoutes()
{
	m_routes.clear();
}

int CvTradeRouteGroup::getRouteCount() const
{
	return m_routes.size();
}

CvTradeRoute* CvTradeRouteGroup::getRouteByIndex(int iIdx)
{
	return &(m_routes.at(iIdx));
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


void CvTradeRouteGroup::read(FDataStreamBase* pStream)
{
	pStream->Read(&m_iId);
	pStream->ReadString(m_sName);

	{	
		int iNumElts;
		pStream->Read(&iNumElts);
		m_routes.clear();
		for (int i = 0; i < iNumElts; ++i)
		{
			CvTradeRoute tradeRoute;
			tradeRoute.read(pStream);
			m_routes.push_back(tradeRoute);
		}

	}
	
}

void CvTradeRouteGroup::write(FDataStreamBase* pStream)
{	
	pStream->Write(m_iId);	
	pStream->WriteString(m_sName);
		
	pStream->Write(m_routes.size());
	for (std::vector<CvTradeRoute>::iterator it = m_routes.begin(); it != m_routes.end(); ++it)
	{
		(*it).write(pStream);
	}
}
