#include "CvGameCoreDLL.h"
#include "CvDLLInterfaceIFaceBase.h"


CvPlayerCivEffect::CvPlayerCivEffect()
	: m_iAllowFoundCity(0)
	, m_ba_CacheAllowBuild                     (JIT_ARRAY_BUILD, true)
	, m_ja_iCacheFreePromotionsForProfessions  (JIT_ARRAY_PROFESSION      , JIT_ARRAY_PROMOTION)
	, m_ja_iCacheFreePromotionsForUnitClasses  (JIT_ARRAY_UNIT_CLASS      , JIT_ARRAY_PROMOTION)
{
}

CvPlayerCivEffect::~CvPlayerCivEffect()
{
}


void CvPlayerCivEffect::applyCivEffect(const CivEffectInfo& kCivEffect, int iChange, bool bForceUpdateCache)
{
	FAssert(iChange == 1 || iChange == -1);
	FAssert(getCivilizationType() != NO_CIVILIZATION);
	CvCivilizationInfo *pCivInfo = &GC.getCivilizationInfo(getCivilizationType());

	bool bUpdateBuilds          = bForceUpdateCache;
	bool bUpdateBuildings       = bForceUpdateCache;
	bool bUpdateImmigrants      = bForceUpdateCache;
	bool bUpdatePromotions      = bForceUpdateCache;
	bool bUpdateUnits           = bForceUpdateCache;

	m_ja_iCacheAllowsBonuses                                          .addCache(iChange, kCivEffect.getAllowedBonuses                (), pCivInfo);
	bUpdateBuilds |= m_ja_iCacheAllowsBuilds                          .addCache(iChange, kCivEffect.getAllowedBuilds                 (), pCivInfo);
	bUpdateBuildings |= m_ja_iCacheAllowsBuildings                    .addCache(iChange, kCivEffect.getAllowedBuildingClasses        (), pCivInfo);
	m_ja_iCacheAllowsCivics                                           .addCache(iChange, kCivEffect.getAllowedCivics                 (), pCivInfo);
	bUpdateImmigrants |= m_ja_iCacheAllowsImmigrants                  .addCache(iChange, kCivEffect.getAllowedImmigrants             (), pCivInfo);
	bUpdateBuilds |= m_ja_iCacheAllowsImprovements                    .addCache(iChange, kCivEffect.getAllowedImprovements           (), pCivInfo);
	bUpdatePromotions |= m_ja_iCacheAllowsProfessions                 .addCache(iChange, kCivEffect.getAllowedProfessions            (), pCivInfo);
	m_ja_iCacheAllowsPromotions                                       .addCache(iChange, kCivEffect.getAllowedPromotions             (), pCivInfo);
	bUpdateBuilds |= m_ja_iCacheAllowsRoutes                          .addCache(iChange, kCivEffect.getAllowedRoutes                 (), pCivInfo);
	bUpdateUnits |= m_ja_iCacheAllowsUnits                            .addCache(iChange, kCivEffect.getAllowedUnitClasses            (), pCivInfo);
	m_ja_iCacheAllowsYields                                           .addCache(iChange, kCivEffect.getAllowedYields                 (), pCivInfo);

	m_ja_iCacheAllowsConqueringCity                                   .addCache(iChange, kCivEffect.getAllowConqueringCity           (), pCivInfo);

	m_iAllowFoundCity += iChange * kCivEffect.getAllowFoundCity();
	m_iCacheCanUseDomesticMarket += iChange * kCivEffect.getCanUseDomesticMarket();

	m_iCacheNumUnitsOnDock += iChange * kCivEffect.getNumUnitsOnDockChange();


	bUpdatePromotions |= m_ja_iCacheFreePromotions                    .addCache(iChange, kCivEffect.getFreePromotions                (), pCivInfo);
	bUpdatePromotions |= m_ja_iCacheFreePromotionsForProfessions      .addCache(iChange, kCivEffect.getFreePromotionsForProfessions  (), pCivInfo);
	bUpdatePromotions |= m_ja_iCacheFreePromotionsForUnitClasses      .addCache(iChange, kCivEffect.getFreePromotionsForUnitCombats  (), pCivInfo);

	// The CivEffect has been applied. Now update secondary caches if needed

	if (gDLL->isGameInitializing() && !bForceUpdateCache)
	{
		// Don't waste time on secondary caches during initialization
		// The last call from rebuildCivEffectCache() will have bForceUpdateCache to set everything
		return;
	}

	// TODO: add get index to CivEffectInfo. Update cache instead of resetting.
	// add assert check for count >= 0
	// perhaps add max 1 too, unless a multiple bool is set in CivEffectInfo.
	m_ja_iHasCivEffectCache.reset();

	if (bUpdateBuilds)
	{
		// Something changed, which might affect which builds are allowed
		// reset and rebuild the BoolArray to make sure it's up to date
		m_ba_CacheAllowBuild.reset();
		for (BuildTypes eBuild = FIRST_BUILD; eBuild < NUM_BUILD_TYPES; ++eBuild)
		{
			if (this->m_ja_iCacheAllowsBuilds.get(eBuild) <= 0)
			{
				m_ba_CacheAllowBuild.set(false, eBuild);
				continue;
			}

			const CvBuildInfo& kBuild = GC.getBuildInfo(eBuild);

			ImprovementTypes eImprovement = static_cast<ImprovementTypes>(kBuild.getImprovement());
			if (eImprovement != NO_IMPROVEMENT && !this->canUseImprovement(eImprovement))
			{
				m_ba_CacheAllowBuild.set(false, eBuild);
				continue;
			}

			RouteTypes eRoute = static_cast<RouteTypes>(kBuild.getRoute());
			if (eRoute != NO_ROUTE && !this->canUseRoute(eRoute))
			{
				m_ba_CacheAllowBuild.set(false, eBuild);
				continue;
			}
		}
	}

	if (bUpdateBuildings)
	{
		m_at_AllowedBuildings.assign(m_ja_iCacheAllowsBuildings);
	}

	if (gDLL->isGameInitializing())
	{
		return;
	}

	if (bUpdateUnits || bUpdateImmigrants || kCivEffect.getNumUnitsOnDockChange() != 0)
	{
		if (!this->isNative() && !this->isEurope())
		{
			verifyImmigration();
			if (getID() == GC.getGameINLINE().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setDirty(EuropeScreen_DIRTY_BIT, true);
			}
		}
	}

	if (bUpdateBuildings || kCivEffect.getCanUseDomesticMarket() != 0)
	{
		int iLoop;
		for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			pLoopCity->UpdateBuildingAffectedCache();
		}
	}
}

void CvPlayerCivEffect::resetCivEffectCache()
{
	m_ja_iCacheAllowsBonuses.reset();
	m_ja_iCacheAllowsBuilds.reset();
	m_ja_iCacheAllowsBuildings.reset();
	m_ja_iCacheAllowsCivics.reset();
	m_ja_iCacheAllowsImmigrants.reset();
	m_ja_iCacheAllowsImprovements.reset();
	m_ja_iCacheAllowsProfessions.reset();
	m_ja_iCacheAllowsPromotions.reset();
	m_ja_iCacheAllowsRoutes.reset();
	m_ja_iCacheAllowsUnits.reset();
	m_ja_iCacheAllowsYields.reset();

	m_ba_CacheAllowBuild.reset();

	m_iCacheCanUseDomesticMarket = 0;

	m_iCacheNumUnitsOnDock = 0;

	m_ja_iCacheFreePromotions.reset();
	m_ja_iCacheFreePromotionsForProfessions.reset();
	m_ja_iCacheFreePromotionsForUnitClasses.reset();

	m_ja_iHasCivEffectCache.reset();
}

void CvPlayerCivEffect::rebuildCivEffectCache()
{
	resetCivEffectCache();

	if (getCivilizationType() == NO_CIVILIZATION)
	{
		return;
	}

	updateHasCivEffectCache();

	for (CivEffectTypes eCivEffect = FIRST_CIV_EFFECT; eCivEffect < m_ja_iHasCivEffectCache.length(); ++eCivEffect)
	{
		unsigned int iCount = m_ja_iHasCivEffectCache.get(eCivEffect);
		if (iCount > 0)
		{
			// the code asserts if iChange is other than 1 or -1.
			// Since this is the only time it makes sense to use higher numbers, get around this with a loop.
			// In most cases the loop will only execute once and it's not a high performance function anyway.
			for (unsigned int i = 0; i < iCount; ++i)
			{
				applyCivEffect(eCivEffect);
			}
		}
	}


	CvCivilizationInfo &kCivInfo = GC.getCivilizationInfo(getCivilizationType());

	// Setup some default data based on xml values
	// While those aren't from the CivEffect file itself, they are still useful to include in the cache.
	for (BuildingTypes eBuilding = FIRST_BUILDING; eBuilding < GC.getNumBuildingInfos(); ++eBuilding)
	{
		if (eBuilding != kCivInfo.getCivilizationBuildings(GC.getBuildingInfo(eBuilding).getBuildingClassType()))
		{
			m_ja_iCacheAllowsBuildings.set(-50, eBuilding);
		}
	}

	for (UnitTypes eUnit = FIRST_UNIT; eUnit < GC.getNumUnitInfos(); ++eUnit)
	{
		if (eUnit != kCivInfo.getCivilizationUnits(GC.getUnitInfo(eUnit).getUnitClassType()))
		{
			m_ja_iCacheAllowsUnits.set(-50, eUnit);
		}
	}

	// last CivEffect to be applied
	// use this to both apply the autogenerated CivEffect and force update secondary caches
	applyCivEffect(GC.getAutogeneratedCivEffect(), 1, true);
}

int CvPlayerCivEffect::getCivEffectCount(CivEffectTypes eCivEffect) const
{
	if (m_ja_iHasCivEffectCache.isEmpty())
	{
		// the cache is out of date. Update it.
		updateHasCivEffectCache();
	}
	return m_ja_iHasCivEffectCache.get(eCivEffect);
}

void CvPlayerCivEffect::updateHasCivEffectCache() const
{
	m_ja_iHasCivEffectCache.reset();

	m_ja_iHasCivEffectCache.safeAdd(1, CIV_EFFECT_DEFAULT_ALL);
	m_ja_iHasCivEffectCache.safeAdd(1, this->isHuman() ? CIV_EFFECT_DEFAULT_HUMAN : CIV_EFFECT_DEFAULT_AI);

	for (TraitTypes eTrait = FIRST_TRAIT; eTrait < NUM_TRAIT_TYPES; ++eTrait)
	{
		if (this->hasTrait(eTrait))
		{
			m_ja_iHasCivEffectCache.safeAdd(1, GC.getTraitInfo(eTrait).getCivEffect());
		}
	}

	for (CivicOptionTypes eCivicOption = FIRST_CIVICOPTION; eCivicOption < NUM_CIVICOPTION_TYPES; ++eCivicOption)
	{
		CivicTypes eCivic = getCivic(eCivicOption);
		if (eCivic != NO_CIVIC)
		{
			m_ja_iHasCivEffectCache.safeAdd(1, GC.getCivicInfo(eCivic).getCivEffect());
		}
	}

	TeamTypes eTeam = this->getTeam();
	for (FatherTypes eFather = FIRST_FATHER; eFather < NUM_FATHER_TYPES; ++eFather)
	{
		if (GC.getGameINLINE().getFatherTeam(eFather) == eTeam)
		{
			m_ja_iHasCivEffectCache.safeAdd(1, GC.getFatherInfo(eFather).getCivEffect());
		}
	}

	for (EraTypes eEra = FIRST_ERA; eEra <= getCurrentEra() && eEra < NUM_ERA_TYPES; ++eEra)
	{
		m_ja_iHasCivEffectCache.safeAdd(1, GC.getEraInfo(eEra).getCivEffect());
	}

	CvCivilizationInfo &kCivInfo = GC.getCivilizationInfo(getCivilizationType());

	m_ja_iHasCivEffectCache.safeAdd(1, kCivInfo.getCivEffect());
	switch (kCivInfo.getCivCategoryTypes())
	{
	case CIV_CATEGORY_EUROPEAN:
		m_ja_iHasCivEffectCache.safeAdd(1, CIV_EFFECT_DEFAULT_EUROPEAN);
		break;
	case CIV_CATEGORY_NATIVE:
		m_ja_iHasCivEffectCache.safeAdd(1, CIV_EFFECT_DEFAULT_NATIVE);
		break;
	case CIV_CATEGORY_KING:
		m_ja_iHasCivEffectCache.safeAdd(1, CIV_EFFECT_DEFAULT_KING);
		break;
	case CIV_CATEGORY_BARBARIAN:
		m_ja_iHasCivEffectCache.safeAdd(1, CIV_EFFECT_DEFAULT_BARBARIAN);
		break;
	case CIV_CATEGORY_CHURCH:
		m_ja_iHasCivEffectCache.safeAdd(1, CIV_EFFECT_DEFAULT_CHURCH);
		break;
	case CIV_CATEGORY_NOT_SET:
		break;
	}
	BOOST_STATIC_ASSERT(NUM_CIV_CATEGORY_TYPES == 6);
}
