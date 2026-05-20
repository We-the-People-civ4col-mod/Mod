#pragma once
#ifndef CyTradeRouteGroup_h
#define CyTradeRouteGroup_h
#include "CyTradeRoute.h"
//
// Python wrapper class for CvTradeRouteGroup
//
class CvTradeRouteGroup;

class CyTradeRouteGroup
{
public:
	CyTradeRouteGroup();
	CyTradeRouteGroup(CvTradeRouteGroup* pTradeGroup);

	int getID();
	std::wstring getName();
	void setName(std::wstring);
	
	void removeRoute(int routeId);	

	int getRouteCount();
	CyTradeRoute* CyTradeRouteGroup::getRouteByIndex(int iIndx);
	
private:
	CvTradeRouteGroup* m_pTradeGroup;
};
#endif	// CyTradeRouteGroup_h
