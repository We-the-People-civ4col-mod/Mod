#pragma once

// Struct to contain all EnumMaps used to store xml data
// Placing them directly in CiGlobals means that header file has to include all the info classes, hence all files includes all of them
// Placing them here mean only 3 cpp file will need to include all infos

#include "CvEnums.h"
#include "CiInfos.h"

struct CiGlobalsInfoContainer
{
	EnumMap<CivilizationTypes, CiCivilizationInfo> m_CivilizationTypes;
	EnumMap<CivCategoryTypes, CiCivCategoryInfo> m_CivCategoryTypes;
	EnumMap<DomainTypes, CiDomainInfo> m_DomainTypes;
};
