//R&R mod, vetiarvind, Trade Groups
#pragma once

#ifndef CIV4_TRADE_ROUTE_GROUP_H
#define CIV4_TRADE_ROUTE_GROUP_H
#include "CvTradeRoute.h" //R&R mod, vetiarvind, trade groups
class CvTradeRouteGroup
{
public:
	CvTradeRouteGroup();
	~CvTradeRouteGroup();	

	int getID() const;
	void setID(int iId);
	void CvTradeRouteGroup::addRoute(CvTradeRoute* tradeRoute);
	void addRoute(const IDInfo& kSource, const IDInfo& kDestination, YieldTypes eYield);
	void removeRoute(int routeId);
	void clearRoutes();
	int getRouteCount() const;
	CvTradeRoute* getRouteByIndex(int iIdx);

	const CvWString getName(uint uiForm) const;
	void setName(const wchar* name);
	
	void read(CvSavegameReader reader);
	void write(CvSavegameWriter writer);

	void resetSavedData();
	
	static const int EUROPE_CITY_ID = -1;
	static const int ANYWHERE_CITY_ID = -2;

protected:	
	int m_iId;	
	CvWString m_sName;	
	std::vector<CvTradeRoute> m_Routes; 
	
};


#endif  // CIV4_TRADE_ROUTE_H
