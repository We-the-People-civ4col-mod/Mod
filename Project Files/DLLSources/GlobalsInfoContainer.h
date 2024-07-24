#pragma once

// Struct to contain all EnumMaps used to store xml data
// Placing them directly in CiGlobals means that header file has to include all the info classes, hence all files includes all of them
// Placing them here mean only 3 cpp file will need to include all infos

#include "CvEnums.h"
#include "Infos.h"
#include "CvInfos.h"

struct GlobalsInfoContainer
{
	std::vector<CvActionInfo> m_actions;
	EnumMap<BuildTypes, CvBuildInfo> m_BuildTypes;
	EnumMap<CivilizationTypes, CivilizationInfo> m_CivilizationTypes;
	EnumMap<CivCategoryTypes, CivCategoryInfo> m_CivCategoryTypes;
	EnumMap<DomainTypes, DomainInfo> m_DomainTypes;
	EnumMap<UnitAITypes, UnitAIInfo> m_UnitAITypes;
};
