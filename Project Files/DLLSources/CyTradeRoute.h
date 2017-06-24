#pragma once

#ifndef CyTradeRoute_h
#define CyTradeRoute_h

#include "CvTradeRoute.h"

class CyCity;

class CyTradeRoute
{
public:
	CyTradeRoute();
	CyTradeRoute(CvTradeRoute* pTradeRoute);

	int getID() const;
	IDInfo getSourceCity() const;
	IDInfo getDestinationCity() const;
	std::wstring getSourceCityNameKey() const;
	std::wstring getDestinationCityNameKey() const;
	std::wstring getSourceCityName() const;
	std::wstring getDestinationCityName() const;
	int getYield() const;

protected:
	CvTradeRoute* m_pTradeRoute;
};

#endif	// #ifndef CyTradeRoute_h
