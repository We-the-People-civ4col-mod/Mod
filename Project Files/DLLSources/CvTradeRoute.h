#pragma once

#ifndef CIV4_TRADE_ROUTE_H
#define CIV4_TRADE_ROUTE_H

class CvTradeRoute
{
public:
	CvTradeRoute();
	~CvTradeRoute();

	void init(const IDInfo& kSourceCity, const IDInfo& kDestinationCity, YieldTypes eYield);

	int getID() const;
	void setID(int iId);

	const IDInfo& getSourceCity() const;
	void setSourceCity(const IDInfo& kCity);
	const wchar* getSourceCityNameKey() const;

	const IDInfo& getDestinationCity() const;
	void setDestinationCity(const IDInfo& kCity);
	const wchar* getDestinationCityNameKey() const;

	YieldTypes getYield() const;
	void setYield(YieldTypes eYield);

	bool checkValid(PlayerTypes ePlayer) const;

	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);


	// Custom_House_Mod Start
	int getBestPortCityID(PlayerTypes ePlayer) const;
	// Custom_House_Mod End
	static const int EUROPE_CITY_ID = -1;
	static const int ANYWHERE_CITY_ID = -2;

protected:
	int m_iId;
	IDInfo m_kSourceCity;
	IDInfo m_kDestinationCity;
	YieldTypes m_eYield;

	void setActiveDirty();
};


#endif  // CIV4_TRADE_ROUTE_H
