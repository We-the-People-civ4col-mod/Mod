#pragma once
#ifndef CIV4_CITY_YIELDS_H
#define CIV4_CITY_YIELDS_H

class CvCity;

class CvCityYields
{
	const CvCity& m_city;
public:
	CvCityYields(const CvCity& city);

	int getBaseRawYieldProduced         (YieldTypes eYieldType) const;
	int getBaseRawYieldProducedIndoor   (YieldTypes eYieldType) const;
	int getBaseRawYieldProducedPlots    (YieldTypes eYieldType) const;
	int getBaseRawYieldProducedBuildings(YieldTypes eYieldType) const;
	int getBaseRawYieldProducedLeader   (YieldTypes eYieldType) const;

	// todo move relevant variables here. Not moved yet to avoid savegame compatibility issues
};

#endif
