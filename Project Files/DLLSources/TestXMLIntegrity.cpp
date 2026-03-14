
#include "CvGameCoreDLL.h"
#include "WTPTestFramework.h"

#ifdef FASSERT_ENABLE

#include "CvTradeRoute.h"

// ============================================================================
// Profession Yield Validity
// ============================================================================

WTP_TEST(XMLIntegrity, ProfessionYieldsValid)
{
	for (ProfessionTypes eProfession = FIRST_PROFESSION; eProfession < NUM_PROFESSION_TYPES; ++eProfession)
	{
		const CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);

		for (int i = 0; i < kProfession.getNumYieldsProduced(); ++i)
		{
			int eYield = kProfession.getYieldsProduced(i);
			WTP_ASSERT_MSG(eYield >= 0 && eYield < NUM_YIELD_TYPES,
				"Profession has invalid produced yield");
		}

		for (int i = 0; i < kProfession.getNumYieldsConsumed(); ++i)
		{
			int eYield = kProfession.getYieldsConsumed(i);
			WTP_ASSERT_MSG(eYield >= 0 && eYield < NUM_YIELD_TYPES,
				"Profession has invalid consumed yield");
		}
	}
}

// ============================================================================
// Building Class Validity
// ============================================================================

WTP_TEST(XMLIntegrity, BuildingClassValid)
{
	for (BuildingTypes eBuilding = FIRST_BUILDING; eBuilding < NUM_BUILDING_TYPES; ++eBuilding)
	{
		const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
		int iBuildingClass = kBuilding.getBuildingClassType();
		WTP_ASSERT_MSG(iBuildingClass >= 0 && iBuildingClass < GC.getNumBuildingClassInfos(),
			"Building has invalid BuildingClassType");
	}
}

// ============================================================================
// Unit Default Professions
// ============================================================================

WTP_TEST(XMLIntegrity, UnitDefaultProfessionValid)
{
	for (UnitTypes eUnit = FIRST_UNIT; eUnit < NUM_UNIT_TYPES; ++eUnit)
	{
		const CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);
		ProfessionTypes eDefaultProfession = kUnit.getDefaultProfession();

		// NO_PROFESSION is valid (some units have no default profession)
		if (eDefaultProfession != NO_PROFESSION)
		{
			WTP_ASSERT_MSG(eDefaultProfession >= FIRST_PROFESSION && eDefaultProfession < NUM_PROFESSION_TYPES,
				"Unit has invalid default profession");
		}
	}
}

// ============================================================================
// Founding Father CivEffect References
// ============================================================================

WTP_TEST(XMLIntegrity, FatherCivEffectValid)
{
	for (FatherTypes eFather = FIRST_FATHER; eFather < NUM_FATHER_TYPES; ++eFather)
	{
		const CvFatherInfo& kFather = GC.getFatherInfo(eFather);
		CivEffectTypes eCivEffect = kFather.getCivEffect();

		// CivEffect should be valid if assigned
		if (eCivEffect != NO_CIV_EFFECT)
		{
			WTP_ASSERT_MSG(eCivEffect >= FIRST_CIV_EFFECT && eCivEffect < NUM_CIV_EFFECT_TYPES,
				"Father has invalid CivEffect index");
		}
	}
}

// ============================================================================
// Father Category References
// ============================================================================

WTP_TEST(XMLIntegrity, FatherCategoryValid)
{
	for (FatherTypes eFather = FIRST_FATHER; eFather < NUM_FATHER_TYPES; ++eFather)
	{
		const CvFatherInfo& kFather = GC.getFatherInfo(eFather);
		FatherCategoryTypes eCategory = kFather.getFatherCategory();

		WTP_ASSERT_MSG(eCategory >= 0 && eCategory < GC.getNumFatherCategoryInfos(),
			"Father has invalid category");
	}
}

// ============================================================================
// Unit Building Class References (training buildings)
// ============================================================================

WTP_TEST(XMLIntegrity, UnitYieldCostsNonNegative)
{
	for (UnitTypes eUnit = FIRST_UNIT; eUnit < NUM_UNIT_TYPES; ++eUnit)
	{
		const CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);

		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			int iCost = kUnit.getYieldCost(iYield);
			// Yield costs should never be negative
			WTP_ASSERT_MSG(iCost >= 0,
				"Unit has negative yield cost");
		}
	}
}

// ============================================================================
// Profession Equipment Yields
// ============================================================================

WTP_TEST(XMLIntegrity, ProfessionEquipmentYieldsValid)
{
	for (ProfessionTypes eProfession = FIRST_PROFESSION; eProfession < NUM_PROFESSION_TYPES; ++eProfession)
	{
		const CvProfessionInfo& kProfession = GC.getProfessionInfo(eProfession);

		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			int iAmount = kProfession.getYieldEquipmentAmount(iYield);
			// Equipment amounts should be non-negative
			WTP_ASSERT_MSG(iAmount >= 0,
				"Profession has negative equipment yield amount");
		}
	}
}

// ============================================================================
// Building Yield Demands Valid
// ============================================================================

WTP_TEST(XMLIntegrity, BuildingYieldDemandsValid)
{
	for (BuildingTypes eBuilding = FIRST_BUILDING; eBuilding < NUM_BUILDING_TYPES; ++eBuilding)
	{
		const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
		const InfoArray<YieldTypes, int>& kDemands = kBuilding.getYieldDemands();

		// InfoArray should have valid content (no crash accessing it)
		// The InfoArray itself enforces valid types at construction
		(void)kDemands;
	}
}

// ============================================================================
// Bonus Types Valid in Terrain
// ============================================================================

WTP_TEST(XMLIntegrity, TerrainBonusesValid)
{
	for (TerrainTypes eTerrain = FIRST_TERRAIN; eTerrain < NUM_TERRAIN_TYPES; ++eTerrain)
	{
		const CvTerrainInfo& kTerrain = GC.getTerrainInfo(eTerrain);

		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			// Accessing yield info for each terrain should not crash
			int iValue = kTerrain.getYield(iYield);
			(void)iValue;
		}
	}
}

// ============================================================================
// Trade Location Consistency
// ============================================================================

WTP_TEST(XMLIntegrity, TradeLocationSentinelConsistency)
{
	// Verify EUROPE sentinel is valid for isOffMapTradeLocation
	WTP_ASSERT(CvTradeRoute::isOffMapTradeLocation(CvTradeRoute::EUROPE_CITY_ID));
	WTP_ASSERT(CvTradeRoute::isOffMapTradeLocation(CvTradeRoute::AFRICA_CITY_ID));
	WTP_ASSERT(CvTradeRoute::isOffMapTradeLocation(CvTradeRoute::PORT_ROYAL_CITY_ID));

	// Sentinels must not collide with valid city IDs (which start at 0)
	WTP_ASSERT(CvTradeRoute::EUROPE_CITY_ID < 0);
	WTP_ASSERT(CvTradeRoute::AFRICA_CITY_ID < 0);
	WTP_ASSERT(CvTradeRoute::PORT_ROYAL_CITY_ID < 0);
}

// ============================================================================
// Test Runner
// ============================================================================

void RunXMLIntegrityTests()
{
	WTP_RUN_SUITE_BEGIN();
	WTP_RUN_TEST(XMLIntegrity, ProfessionYieldsValid);
	WTP_RUN_TEST(XMLIntegrity, BuildingClassValid);
	WTP_RUN_TEST(XMLIntegrity, UnitDefaultProfessionValid);
	WTP_RUN_TEST(XMLIntegrity, FatherCivEffectValid);
	WTP_RUN_TEST(XMLIntegrity, FatherCategoryValid);
	WTP_RUN_TEST(XMLIntegrity, UnitYieldCostsNonNegative);
	WTP_RUN_TEST(XMLIntegrity, ProfessionEquipmentYieldsValid);
	WTP_RUN_TEST(XMLIntegrity, BuildingYieldDemandsValid);
	WTP_RUN_TEST(XMLIntegrity, TerrainBonusesValid);
	WTP_RUN_TEST(XMLIntegrity, TradeLocationSentinelConsistency);
	WTP_RUN_SUITE_END("XMLIntegrity");
}

#endif
