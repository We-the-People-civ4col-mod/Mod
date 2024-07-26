#include "CvGameCoreDLL.h"
#include "CyInfos.h"


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
