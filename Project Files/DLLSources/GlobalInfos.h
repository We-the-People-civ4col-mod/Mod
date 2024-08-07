#pragma once

#include "CvEnums.h"

struct GlobalsInfoContainer;

class CvBuildInfo;
class CivilizationInfo;
class CivCategoryInfo;
class DomainInfo;

class GlobalInfos
{
	friend class CvXMLLoadUtility;
	friend class EXE_CvGlobals;
	friend class CvGlobals;
	friend class CvActionInfo;
public:
	GlobalInfos(GlobalsInfoContainer& storage);

	const CvBuildInfo& getInfo(BuildTypes eBuild) const;
	const CivilizationInfo& getInfo(CivilizationTypes eCiv) const;
	const CivCategoryInfo& getInfo(CivCategoryTypes eCategory) const;
	const DomainInfo& getInfo(DomainTypes eDomain) const;

protected:
	GlobalsInfoContainer& m_info;
};

extern GlobalInfos INFO;
