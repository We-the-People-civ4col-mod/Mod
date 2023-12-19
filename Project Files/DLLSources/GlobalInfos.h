#pragma once

struct GlobalsInfoContainer;

class CivilizationInfo;
class CivCategoryInfo;
class DomainInfo;

class GlobalInfos
{
	friend class CvXMLLoadUtility;
public:
	GlobalInfos(GlobalsInfoContainer& storage);

	const CivilizationInfo& getInfo(CivilizationTypes eCiv) const;
	const CivCategoryInfo& getInfo(CivCategoryTypes eCategory) const;
	const DomainInfo& getInfo(DomainTypes eDomain) const;

protected:
	GlobalsInfoContainer& m_info;
};

extern GlobalInfos INFO;
