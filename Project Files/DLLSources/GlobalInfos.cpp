#include "CvGameCoreDLL.h"
#include "GlobalsInfoContainer.h"

GlobalInfos::GlobalInfos(GlobalsInfoContainer& storage)
	: m_info(storage)
{
}

const CvBuildInfo& GlobalInfos::getInfo(BuildTypes eBuild) const
{
	return m_info.m_BuildTypes[eBuild];
}

const CivilizationInfo& GlobalInfos::getInfo(CivilizationTypes eCiv) const
{
	return m_info.m_CivilizationTypes[eCiv];
}
const CivCategoryInfo& GlobalInfos::getInfo(CivCategoryTypes eCategory) const
{
	return m_info.m_CivCategoryTypes[eCategory];
}

const DomainInfo& GlobalInfos::getInfo(DomainTypes eDomain) const
{
	return m_info.m_DomainTypes[eDomain];
}


