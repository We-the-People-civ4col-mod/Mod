#pragma once

#ifndef CIV4_CIVEFFECT_H
#define CIV4_CIVEFFECT_H

#include "CvPlayer.h"

class CvPlayerCivEffect : public CvPlayer
{
public:
	CvPlayerCivEffect();
	virtual ~CvPlayerCivEffect();

	int getCivEffectCount(CivEffectTypes eCivEffect) const;

	// pick the version with the referece if you have the instance ready
	void applyCivEffect(const CivEffectInfo& kCivEffect, int iChange = 1);
	void applyCivEffect(CivEffectTypes       eCivEffect, int iChange = 1);

	inline bool canUseBonus       (BonusTypes       eBonus        ) const { return m_ja_iCacheAllowsBonuses       .get(eBonus)       > 0; }
	inline bool canUseBuild       (BuildTypes       eBuild        ) const { return m_ba_CacheAllowBuild           .get(eBuild)          ; }
	inline bool canUseBuilding    (BuildingTypes    eBuilding     ) const { return m_ja_iCacheAllowsBuildings     .get(eBuilding)    > 0; }
	inline bool canUseCivic       (CivicTypes       eCivic        ) const { return m_ja_iCacheAllowsCivics        .get(eCivic)       > 0; }
	inline bool canUseImmigrant   (UnitTypes        eUnit         ) const { return m_ja_iCacheAllowsImmigrants    .get(eUnit)        > 0; }
	inline bool canUseImprovement (ImprovementTypes eImprovement  ) const { return m_ja_iCacheAllowsImprovements  .get(eImprovement) > 0; }
	inline bool canUseProfession  (ProfessionTypes  eProfession   ) const { return m_ja_iCacheAllowsProfessions   .get(eProfession)  > 0; }
	inline bool canUsePromotion   (PromotionTypes   ePromotion    ) const { return m_ja_iCacheAllowsPromotions    .get(ePromotion)   > 0; }
	inline bool canUseRoute       (RouteTypes       eRoute        ) const { return m_ja_iCacheAllowsRoutes        .get(eRoute)       > 0; }
	inline bool canUseUnit        (UnitTypes        eUnit         ) const { return m_ja_iCacheAllowsUnits         .get(eUnit)        > 0; }
	inline bool canUseYield       (YieldTypes       eYield        ) const { return m_ja_iCacheAllowsYields        .get(eYield)       > 0; }

	inline bool canConquerCity    (CivCategoryTypes eCivCategory  ) const { return m_ja_iCacheAllowsConqueringCity.get(eCivCategory) > 0; }
	inline bool canFoundCity()                                      const { return m_iAllowFoundCity > 0; }

	inline const BuildingTypeArray& getAllowedBuildingInfos()      const { return m_at_AllowedBuildings; }

	inline bool canUseDomesticMarket()                             const { return m_iCacheCanUseDomesticMarket > 0; }

	inline unsigned int getNumUnitsOnDock()                        const { return m_iCacheNumUnitsOnDock > 0 ? m_iCacheNumUnitsOnDock : 1; } // return 0 or negative crashes the game


	// unit
	bool hasFreePromotion(PromotionTypes ePromotion) const;
	bool hasFreePromotion(PromotionTypes ePromotion, ProfessionTypes eProfession) const;
	bool hasFreePromotion(PromotionTypes ePromotion, UnitCombatTypes eUnitCombat) const;

private:
	// Allow
	BonusArray          <char> m_ja_iCacheAllowsBonuses;
	BuildArray          <char> m_ja_iCacheAllowsBuilds;
	BuildingArray       <char> m_ja_iCacheAllowsBuildings;
	CivicArray          <char> m_ja_iCacheAllowsCivics;
	UnitArray           <char> m_ja_iCacheAllowsImmigrants;
	ImprovementArray    <char> m_ja_iCacheAllowsImprovements;
	ProfessionArray     <char> m_ja_iCacheAllowsProfessions;
	PromotionArray      <char> m_ja_iCacheAllowsPromotions;
	RouteArray          <char> m_ja_iCacheAllowsRoutes;
	UnitArray           <char> m_ja_iCacheAllowsUnits;
	YieldArray          <char> m_ja_iCacheAllowsYields;

	CivCategoryArray    <char> m_ja_iCacheAllowsConqueringCity;

	char m_iAllowFoundCity;

	// caches of allow caches
	BoolArray                  m_ba_CacheAllowBuild;
	BuildingTypeArray          m_at_AllowedBuildings;

	// City
	int    m_iCacheCanUseDomesticMarket;

	// Growth
	int    m_iCacheNumUnitsOnDock;

	// Units
	PromotionArray      <char> m_ja_iCacheFreePromotions;
	CacheArray2D        <char> m_ja_iCacheFreePromotionsForProfessions;
	CacheArray2D        <char> m_ja_iCacheFreePromotionsForUnitClasses;

	mutable CivEffectArray<short> m_ja_iHasCivEffectCache;

	// warning: it does indeed update the cache, hence writing to m_ba_HasCivEffectCache
	// It's const for everything else. This means const CvPlayerAI references can read the cache and update it if needed.
	// They can't however change any other member data.
	void updateHasCivEffectCache() const;

public:
	void resetCivEffectCache();
	void rebuildCivEffectCache();
	void applyCivEffect(const CivEffectInfo& kCivEffect, int iChange, bool bForceUpdateCache);
};


inline void CvPlayerCivEffect::applyCivEffect(const CivEffectInfo& kCivEffect, int iChange)
{
	applyCivEffect(kCivEffect, iChange, false);
}

inline void CvPlayerCivEffect::applyCivEffect(CivEffectTypes eCivEffect, int iChange)
{
	if (eCivEffect != NO_CIV_EFFECT)
	{
		applyCivEffect(GC.getCivEffectInfo(eCivEffect), iChange, false);
	}
}

inline bool CvPlayerCivEffect::hasFreePromotion(PromotionTypes ePromotion) const
{
	return m_ja_iCacheFreePromotions.get(ePromotion) > 0;
}

inline bool CvPlayerCivEffect::hasFreePromotion(PromotionTypes ePromotion, ProfessionTypes eProfession) const
{
	return m_ja_iCacheFreePromotionsForProfessions.get(eProfession, ePromotion) > 0;
}

inline bool CvPlayerCivEffect::hasFreePromotion(PromotionTypes ePromotion, UnitCombatTypes eUnitCombat) const
{
	return m_ja_iCacheFreePromotionsForUnitClasses.get(eUnitCombat, ePromotion) > 0;
}

#endif
