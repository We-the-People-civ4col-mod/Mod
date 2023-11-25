#include "CvGameCoreDLL.h"
#include "CiGlobalsInfoContainer.h"

CiGlobals::CiGlobals(CiGlobalsInfoContainer& storage)
	: m_info(storage)
{
}


const CiCivCategoryInfo& CiGlobals::getInfo(CivCategoryTypes eCategory) const
{
	return m_info.m_CivCategoryTypes[eCategory];
}

const CiDomainInfo& CiGlobals::getInfo(DomainTypes eDomain) const
{
	return m_info.m_DomainTypes[eDomain];
}



//
// Cv compatible redirects
//
CvBonusInfo& CiGlobals::getInfo(BonusTypes eType)
{
	return CvGlobals::getInfo(eType);
}

CvFatherPointInfo& CiGlobals::getInfo(FatherPointTypes eType)
{
	return CvGlobals::getInfo(eType);
}

CvFeatureInfo& CiGlobals::getInfo(FeatureTypes eType)
{
	return CvGlobals::getInfo(eType);
}

CvImprovementInfo& CiGlobals::getInfo(ImprovementTypes eType)
{
	return CvGlobals::getInfo(eType);
}

CvRouteInfo& CiGlobals::getInfo(RouteTypes eType)
{
	return CvGlobals::getInfo(eType);
}

CvSpecialBuildingInfo& CiGlobals::getInfo(SpecialBuildingTypes eType)
{
	return CvGlobals::getInfo(eType);
}

CvTerrainInfo& CiGlobals::getInfo(TerrainTypes eType)
{
	return CvGlobals::getInfo(eType);
}

CvYieldInfo& CiGlobals::getInfo(YieldTypes eType)
{
	return CvGlobals::getInfo(eType);
}
