#pragma once

#include "CvGlobals.h"

struct CiGlobalsInfoContainer;

class CiDomainInfo;

class CiGlobals : public CvGlobals
{
	friend class CvXMLLoadUtility;
public:
	CiGlobals(CiGlobalsInfoContainer& storage);

	static CiGlobals& getInstance();

	const CiDomainInfo& getInfo(DomainTypes eDomain) const;




	// Cv compatibility redirects
	CvBonusInfo& getInfo(BonusTypes eType);
	CvFatherPointInfo& getInfo(FatherPointTypes eType);
	CvFeatureInfo& getInfo(FeatureTypes eType);
	CvImprovementInfo& getInfo(ImprovementTypes eType);
	CvRouteInfo& getInfo(RouteTypes eType);
	CvSpecialBuildingInfo& getInfo(SpecialBuildingTypes eType);
	CvTerrainInfo& getInfo(TerrainTypes eType);
	CvYieldInfo& getInfo(YieldTypes eYieldNum);

protected:
	CiGlobalsInfoContainer& m_info;
};

extern CiGlobals gGlobals;	// for debugging

//
// inlines
//
inline CiGlobals& CiGlobals::getInstance()
{
	return gGlobals;
}

#define GC CiGlobals::getInstance()
