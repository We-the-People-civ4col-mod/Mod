#include "CvGameCoreDLL.h"
#include "CyInfos.h"

bool CyBuildInfo::isFeatureRemove(int i) const
{
	return CvBuildInfo::getFeatureRemove(static_cast<FeatureTypes>(i)) != NULL;
}

int CyBuildInfo::getFeatureTime(int i) const
{
	const FeatureStruct* featureStruct = CvBuildInfo::getFeatureRemove(static_cast<FeatureTypes>(i));
	return featureStruct ? featureStruct->iTime : 0;
}

int CyBuildInfo::getFeatureYield(int iFeature, int iYield) const
{
	const FeatureStruct* featureStruct = CvBuildInfo::getFeatureRemove(static_cast<FeatureTypes>(iFeature));
	return featureStruct ? featureStruct->Yields.get(iYield) : 0;
}

int CyControlInfo::getActionInfoIndex() const
{
	return CvControlInfo::getActionInfoIndex().value();
}

int CyUnitInfo::getDefaultProfession() const
{
	return CvUnitInfo::getDefaultProfession();
}

int CyUnitInfo::getLeaderPromotion() const
{
	return CvUnitInfo::getLeaderPromotion();
}

int CyPromotionInfo::getActionInfoIndex() const
{
	return CvPromotionInfo::getActionInfoIndex().value();
}

int CyUnitInfo::getUnitClassType() const
{
	return CvUnitInfo::getUnitClassType();
}
