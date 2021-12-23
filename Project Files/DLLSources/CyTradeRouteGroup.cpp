#include "CvGameCoreDLL.h"
#include "CyTradeRoute.h"
#include "CyTradeRouteGroup.h"
#include "CvTradeRouteGroup.h"
#include "CvGlobals.h"

CyTradeRouteGroup::CyTradeRouteGroup() : m_pTradeGroup(NULL){}
CyTradeRouteGroup::CyTradeRouteGroup(CvTradeRouteGroup* pTradeGroup) : m_pTradeGroup(pTradeGroup)
{

}

int CyTradeRouteGroup::getID()
{
	return m_pTradeGroup->getID();
}

std::wstring CyTradeRouteGroup::getName()
{
	return m_pTradeGroup ? m_pTradeGroup->getName(0) : std::wstring();
}


void CyTradeRouteGroup::setName(std::wstring szName)
{	
	if (m_pTradeGroup)
		m_pTradeGroup->setName(CvWString(szName).GetCString());
}
	

void CyTradeRouteGroup::removeRoute(int routeId)
{
	if (m_pTradeGroup)
		m_pTradeGroup->removeRoute(routeId);
}

int CyTradeRouteGroup::getRouteCount()
{
	return m_pTradeGroup->getRouteCount();
}

CyTradeRoute* CyTradeRouteGroup::getRouteByIndex(int iIndx)
{
	return new CyTradeRoute(m_pTradeGroup->getRouteByIndex(iIndx));
}
