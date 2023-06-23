#ifndef CY_INFOS_H
#define CY_INFOS_H
#include "CvInfos.h"


class CyAchieveInfo : public CvAchieveInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyAchieveInfo) == sizeof(CvAchieveInfo));

class CyEffectInfo : public CvEffectInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyEffectInfo) == sizeof(CvEffectInfo));

class CyTerrainInfo : public CvTerrainInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyTerrainInfo) == sizeof(CvTerrainInfo));

class CyBonusInfo : public CvBonusInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyBonusInfo) == sizeof(CvBonusInfo));

class CyFeatureInfo : public CvFeatureInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyFeatureInfo) == sizeof(CvFeatureInfo));

class CyCivilizationInfo : public CvCivilizationInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyCivilizationInfo) == sizeof(CvCivilizationInfo));

class CyLeaderHeadInfo : public CvLeaderHeadInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyLeaderHeadInfo) == sizeof(CvLeaderHeadInfo));

class CyTraitInfo : public CvTraitInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyTraitInfo) == sizeof(CvTraitInfo));

class CyUnitInfo : public CvUnitInfo
{
public:
	int getDefaultProfession() const;
	int getUnitClassType() const;
};
BOOST_STATIC_ASSERT(sizeof(CyUnitInfo) == sizeof(CvUnitInfo));

class CySpecialUnitInfo : public CvSpecialUnitInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CySpecialUnitInfo) == sizeof(CvSpecialUnitInfo));

class CyYieldInfo : public CvYieldInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyYieldInfo) == sizeof(CvYieldInfo));

class CyRouteInfo : public CvRouteInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyRouteInfo) == sizeof(CvRouteInfo));

class CyImprovementInfo : public CvImprovementInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyImprovementInfo) == sizeof(CvImprovementInfo));

class CyGoodyInfo : public CvGoodyInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyGoodyInfo) == sizeof(CvGoodyInfo));

class CyBuildInfo : public CvBuildInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyBuildInfo) == sizeof(CvBuildInfo));

class CyHandicapInfo : public CvHandicapInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyHandicapInfo) == sizeof(CvHandicapInfo));

class CyGameSpeedInfo : public CvGameSpeedInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyGameSpeedInfo) == sizeof(CvGameSpeedInfo));

class CyTurnTimerInfo : public CvTurnTimerInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyTurnTimerInfo) == sizeof(CvTurnTimerInfo));

class CyBuildingClassInfo : public CvBuildingClassInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyBuildingClassInfo) == sizeof(CvBuildingClassInfo));

class CyMissionInfo : public CvMissionInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyMissionInfo) == sizeof(CvMissionInfo));

class CyCommandInfo : public CvCommandInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyCommandInfo) == sizeof(CvCommandInfo));

class CyAutomateInfo : public CvAutomateInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyAutomateInfo) == sizeof(CvAutomateInfo));

class CyActionInfo : public CvActionInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyActionInfo) == sizeof(CvActionInfo));

class CyUnitClassInfo : public CvUnitClassInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyUnitClassInfo) == sizeof(CvUnitClassInfo));

class CyBuildingInfo : public CvBuildingInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyBuildingInfo) == sizeof(CvBuildingInfo));

class CyCivicInfo : public CvCivicInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyCivicInfo) == sizeof(CvCivicInfo));

class CyDiplomacyInfo : public CvDiplomacyInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyDiplomacyInfo) == sizeof(CvDiplomacyInfo));

class CyControlInfo : public CvControlInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyControlInfo) == sizeof(CvControlInfo));

class CySpecialBuildingInfo : public CvSpecialBuildingInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CySpecialBuildingInfo) == sizeof(CvSpecialBuildingInfo));

class CyPromotionInfo : public CvPromotionInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyPromotionInfo) == sizeof(CvPromotionInfo));

class CyProfessionInfo : public CvProfessionInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyProfessionInfo) == sizeof(CvProfessionInfo));

class CyEmphasizeInfo : public CvEmphasizeInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyEmphasizeInfo) == sizeof(CvEmphasizeInfo));

class CyCultureLevelInfo : public CvCultureLevelInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyCultureLevelInfo) == sizeof(CvCultureLevelInfo));

class CyEraInfo : public CvEraInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyEraInfo) == sizeof(CvEraInfo));

class CyVictoryInfo : public CvVictoryInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyVictoryInfo) == sizeof(CvVictoryInfo));

class CyWorldInfo : public CvWorldInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyWorldInfo) == sizeof(CvWorldInfo));

class CyClimateInfo : public CvClimateInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyClimateInfo) == sizeof(CvClimateInfo));

class CySeaLevelInfo : public CvSeaLevelInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CySeaLevelInfo) == sizeof(CvSeaLevelInfo));

class CyEuropeInfo : public CvEuropeInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyEuropeInfo) == sizeof(CvEuropeInfo));

class CyMainMenuInfo : public CvMainMenuInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyMainMenuInfo) == sizeof(CvMainMenuInfo));

class CyFatherInfo : public CvFatherInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyFatherInfo) == sizeof(CvFatherInfo));

class CyFatherPointInfo : public CvFatherPointInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyFatherPointInfo) == sizeof(CvFatherPointInfo));

class CyEventTriggerInfo : public CvEventTriggerInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyEventTriggerInfo) == sizeof(CvEventTriggerInfo));

class CyEventInfo : public CvEventInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyEventInfo) == sizeof(CvEventInfo));

class CyHurryInfo : public CvHurryInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyHurryInfo) == sizeof(CvHurryInfo));

class CyPlayerOptionInfo : public CvPlayerOptionInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyPlayerOptionInfo) == sizeof(CvPlayerOptionInfo));

class CyGraphicOptionInfo : public CvGraphicOptionInfo
{
public:
};
BOOST_STATIC_ASSERT(sizeof(CyGraphicOptionInfo) == sizeof(CvGraphicOptionInfo));

#endif
