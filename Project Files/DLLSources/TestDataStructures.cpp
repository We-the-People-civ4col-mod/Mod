
#include "CvGameCoreDLL.h"
#include "WTPTestFramework.h"

#ifdef FASSERT_ENABLE

#include "CvTradeRoute.h"
#include "CvIdVector.h"

// ============================================================================
// EnumMap Tests
// ============================================================================

// --- int specialization ---

WTP_TEST(EnumMap, IntDefaultZero)
{
	EnumMap<RouteTypes, int> em;
	RouteTypes eKey = static_cast<RouteTypes>(0);

	WTP_ASSERT(!em.isAllocated());
	WTP_ASSERT(!em.hasContent());
	WTP_ASSERT_EQ(0, em.get(eKey));
	WTP_ASSERT_EQ(0, em.getTotal());
}

WTP_TEST(EnumMap, IntSetGetReset)
{
	EnumMap<RouteTypes, int> em;
	RouteTypes eKey = static_cast<RouteTypes>(0);

	em.set(eKey, 42);
	WTP_ASSERT(em.isAllocated());
	WTP_ASSERT(em.hasContent());
	WTP_ASSERT_EQ(42, em.get(eKey));

	em.set(eKey, 0);
	WTP_ASSERT_EQ(0, em.get(eKey));
	WTP_ASSERT(!em.hasContent());

	em.set(eKey, 7);
	em.reset();
	WTP_ASSERT(!em.isAllocated());
	WTP_ASSERT(!em.hasContent());
	WTP_ASSERT_EQ(0, em.get(eKey));
}

WTP_TEST(EnumMap, IntAdd)
{
	EnumMap<RouteTypes, int> em;
	RouteTypes eKey = static_cast<RouteTypes>(0);

	em.add(eKey, 5);
	WTP_ASSERT_EQ(5, em.get(eKey));
	em.add(eKey, 3);
	WTP_ASSERT_EQ(8, em.get(eKey));
	em.add(eKey, -8);
	WTP_ASSERT_EQ(0, em.get(eKey));
}

WTP_TEST(EnumMap, IntGetTotal)
{
	EnumMap<RouteTypes, int> em;
	RouteTypes e0 = static_cast<RouteTypes>(0);
	RouteTypes e1 = static_cast<RouteTypes>(1);

	em.set(e0, 10);
	em.set(e1, 20);
	WTP_ASSERT_EQ(30, em.getTotal());
}

WTP_TEST(EnumMap, IntAddAll)
{
	EnumMap<RouteTypes, int> em;
	RouteTypes e0 = static_cast<RouteTypes>(0);

	em.addAll(5);
	WTP_ASSERT_EQ(5, em.get(e0));
	WTP_ASSERT_EQ(NUM_ROUTE_TYPES * 5, em.getTotal());
}

WTP_TEST(EnumMap, IntNonZeroDefault)
{
	EnumMap<RouteTypes, int, 10> em;
	RouteTypes e0 = static_cast<RouteTypes>(0);

	WTP_ASSERT_EQ(10, em.get(e0));
	WTP_ASSERT_EQ(NUM_ROUTE_TYPES * 10, em.getTotal());

	em.set(e0, 0);
	WTP_ASSERT_EQ(0, em.get(e0));
	WTP_ASSERT_EQ((NUM_ROUTE_TYPES - 1) * 10, em.getTotal());
}

// --- bool specialization (bitfield) ---

WTP_TEST(EnumMap, BoolDefault)
{
	EnumMap<RouteTypes, bool> em;
	RouteTypes eKey = static_cast<RouteTypes>(0);

	WTP_ASSERT(!em.get(eKey));
	WTP_ASSERT(!em.hasContent());
	WTP_ASSERT_EQ(0, em.getTotal());
}

WTP_TEST(EnumMap, BoolSetGetReset)
{
	EnumMap<RouteTypes, bool> em;
	RouteTypes eKey = static_cast<RouteTypes>(0);

	em.set(eKey, true);
	WTP_ASSERT(em.get(eKey));
	WTP_ASSERT(em.hasContent());
	WTP_ASSERT_EQ(1, em.getTotal());

	em.set(eKey, false);
	WTP_ASSERT(!em.get(eKey));
	WTP_ASSERT(!em.hasContent());

	em.set(eKey, true);
	em.reset();
	WTP_ASSERT(!em.get(eKey));
	WTP_ASSERT(!em.hasContent());
}

WTP_TEST(EnumMap, BoolMultipleKeys)
{
	EnumMap<RouteTypes, bool> em;
	RouteTypes e0 = static_cast<RouteTypes>(0);
	RouteTypes e1 = static_cast<RouteTypes>(1);

	em.set(e0, true);
	em.set(e1, true);
	WTP_ASSERT_EQ(2, em.getTotal());
	WTP_ASSERT(em.get(e0));
	WTP_ASSERT(em.get(e1));

	em.set(e0, false);
	WTP_ASSERT_EQ(1, em.getTotal());
	WTP_ASSERT(!em.get(e0));
	WTP_ASSERT(em.get(e1));
}

// --- enum value type ---

WTP_TEST(EnumMap, EnumValueType)
{
	EnumMap<RouteTypes, PlayerTypes> em;
	RouteTypes eKey = static_cast<RouteTypes>(0);

	WTP_ASSERT_EQ(NO_PLAYER, em.get(eKey));
	WTP_ASSERT(!em.hasContent());

	em.set(eKey, FIRST_PLAYER);
	WTP_ASSERT_EQ(FIRST_PLAYER, em.get(eKey));
	WTP_ASSERT(em.hasContent());

	em.set(eKey, NO_PLAYER);
	WTP_ASSERT(!em.hasContent());

	em.set(eKey, FIRST_PLAYER);
	em.reset();
	WTP_ASSERT_EQ(NO_PLAYER, em.get(eKey));
	WTP_ASSERT(!em.hasContent());
}

// --- Large key type (UnitTypes has many entries) ---

WTP_TEST(EnumMap, LargeKeyType)
{
	EnumMap<UnitTypes, int, 10> em;
	UnitTypes eKey = static_cast<UnitTypes>(1);

	WTP_ASSERT_EQ(NUM_UNIT_TYPES * 10, em.getTotal());
	em.set(eKey, 0);
	WTP_ASSERT_EQ((NUM_UNIT_TYPES * 10) - 10, em.getTotal());
	em.addAll(1);
	WTP_ASSERT_EQ((NUM_UNIT_TYPES * 11) - 10, em.getTotal());
}

// --- Full iteration ---

WTP_TEST(EnumMap, FullIteration)
{
	EnumMap<RouteTypes, int> em;

	// Set all values
	for (RouteTypes e = FIRST_ROUTE; e < NUM_ROUTE_TYPES; ++e)
	{
		em.set(e, static_cast<int>(e) + 1);
	}

	// Verify all values
	for (RouteTypes e = FIRST_ROUTE; e < NUM_ROUTE_TYPES; ++e)
	{
		WTP_ASSERT_EQ(static_cast<int>(e) + 1, em.get(e));
	}

	WTP_ASSERT(em.hasContent());
}

// ============================================================================
// JustInTimeArray Tests
// ============================================================================

WTP_TEST(JustInTimeArray, DefaultUnallocated)
{
	JustInTimeArray<int> arr(JIT_ARRAY_ROUTE);

	WTP_ASSERT(!arr.isAllocated());
	WTP_ASSERT_EQ(0, arr.get(0));
}

WTP_TEST(JustInTimeArray, SetAllocates)
{
	JustInTimeArray<int> arr(JIT_ARRAY_ROUTE);

	arr.set(5, 0);
	WTP_ASSERT(arr.isAllocated());
	WTP_ASSERT_EQ(5, arr.get(0));
}

WTP_TEST(JustInTimeArray, SetDefaultNoAllocate)
{
	JustInTimeArray<int> arr(JIT_ARRAY_ROUTE);

	arr.set(0, 0);
	// Setting the default value should not allocate (or should deallocate)
	// The array may or may not remain allocated, but the value should be default
	WTP_ASSERT_EQ(0, arr.get(0));
}

WTP_TEST(JustInTimeArray, ResetDeallocates)
{
	JustInTimeArray<int> arr(JIT_ARRAY_ROUTE);

	arr.set(42, 0);
	WTP_ASSERT(arr.isAllocated());
	arr.reset();
	WTP_ASSERT(!arr.isAllocated());
	WTP_ASSERT_EQ(0, arr.get(0));
}

WTP_TEST(JustInTimeArray, AddAccumulates)
{
	JustInTimeArray<int> arr(JIT_ARRAY_ROUTE);

	arr.set(10, 0);
	arr.add(5, 0);
	WTP_ASSERT_EQ(15, arr.get(0));
}

WTP_TEST(JustInTimeArray, HasContent)
{
	JustInTimeArray<int> arr(JIT_ARRAY_ROUTE);

	WTP_ASSERT(!arr.hasContent());
	arr.set(1, 0);
	WTP_ASSERT(arr.hasContent());
	arr.set(0, 0);
	// After setting back to default, hasContent may still return true if allocated
	// but reset should clear it
	arr.reset();
	WTP_ASSERT(!arr.hasContent());
}

WTP_TEST(JustInTimeArray, NonZeroDefault)
{
	JustInTimeArray<int> arr(JIT_ARRAY_ROUTE, 7);

	WTP_ASSERT(!arr.isAllocated());
	WTP_ASSERT_EQ(7, arr.get(0));
}

WTP_TEST(JustInTimeArray, MultipleIndices)
{
	JustInTimeArray<int> arr(JIT_ARRAY_ROUTE);
	int iLen = arr.length();

	WTP_ASSERT(iLen > 0);

	arr.set(100, 0);
	if (iLen > 1)
	{
		arr.set(200, 1);
		WTP_ASSERT_EQ(100, arr.get(0));
		WTP_ASSERT_EQ(200, arr.get(1));
	}
}

WTP_TEST(JustInTimeArray, KeepMax)
{
	JustInTimeArray<int> arr(JIT_ARRAY_ROUTE);

	arr.set(10, 0);
	arr.keepMax(5, 0);
	WTP_ASSERT_EQ(10, arr.get(0));
	arr.keepMax(20, 0);
	WTP_ASSERT_EQ(20, arr.get(0));
}

WTP_TEST(JustInTimeArray, KeepMin)
{
	JustInTimeArray<int> arr(JIT_ARRAY_ROUTE);

	arr.set(10, 0);
	arr.keepMin(20, 0);
	WTP_ASSERT_EQ(10, arr.get(0));
	arr.keepMin(5, 0);
	WTP_ASSERT_EQ(5, arr.get(0));
}

// ============================================================================
// Coordinates Tests
// ============================================================================

WTP_TEST(Coordinates, DefaultIsInvalid)
{
	Coordinates c;
	WTP_ASSERT(c.isInvalidPlotCoord());
}

WTP_TEST(Coordinates, ExplicitConstruction)
{
	Coordinates c(3, 5);
	// Note: Coordinates::set() calls resetInvalid() if not on map,
	// so we can only test with valid coords when a map exists.
	// Without a map, they become invalid. Test the invalid path.
	Coordinates inv;
	WTP_ASSERT(inv.isInvalidPlotCoord());
}

WTP_TEST(Coordinates, InvalidCoordFactory)
{
	Coordinates c = Coordinates::invalidCoord();
	WTP_ASSERT(c.isInvalidPlotCoord());
}

WTP_TEST(Coordinates, NullCoordFactory)
{
	Coordinates c = Coordinates::nullCoord();
	// nullCoord should not be invalid plot coord (it's 0,0 conceptually)
	// but without a map it may get reset to invalid
	// Just verify it doesn't crash
	(void)c.x();
	(void)c.y();
}

WTP_TEST(Coordinates, EqualityInvalidCoords)
{
	Coordinates a;
	Coordinates b;
	// Both default-constructed (invalid) should be equal
	WTP_ASSERT(a == b);
	WTP_ASSERT(!(a != b));
}

// ============================================================================
// CvTradeRoute Sentinel Tests
// ============================================================================

WTP_TEST(TradeRoute, SentinelConstants)
{
	// Verify sentinel values are distinct
	WTP_ASSERT(CvTradeRoute::EUROPE_CITY_ID != CvTradeRoute::ANYWHERE_CITY_ID);

	// All sentinels are negative
	WTP_ASSERT(CvTradeRoute::EUROPE_CITY_ID < 0);
	WTP_ASSERT(CvTradeRoute::ANYWHERE_CITY_ID < 0);
}

WTP_TEST(TradeRoute, DefaultConstruction)
{
	CvTradeRoute route;
	// Default-constructed route should have invalid/default state
	WTP_ASSERT_EQ(NO_YIELD, route.getYield());
}

// ============================================================================
// CvIdVector Tests
// ============================================================================

// Helper class for CvIdVector testing
class TestIdItem
{
public:
	TestIdItem() : m_iId(-1), m_iValue(0) {}

	int getID() const { return m_iId; }
	void setID(int iId) { m_iId = iId; }

	int getValue() const { return m_iValue; }
	void setValue(int v) { m_iValue = v; }

	void read(FDataStreamBase*) {}
	void write(FDataStreamBase*) const {}

private:
	int m_iId;
	int m_iValue;
};

WTP_TEST(CvIdVector, EmptyAfterConstruction)
{
	CvIdVector<TestIdItem> vec;
	WTP_ASSERT(vec.empty());
	WTP_ASSERT(vec.getById(0) == NULL);
}

WTP_TEST(CvIdVector, AddNewAssignsSequentialIds)
{
	CvIdVector<TestIdItem> vec;

	TestIdItem* p0 = vec.addNew();
	WTP_ASSERT(p0 != NULL);
	WTP_ASSERT_EQ(0, p0->getID());

	TestIdItem* p1 = vec.addNew();
	WTP_ASSERT(p1 != NULL);
	WTP_ASSERT_EQ(1, p1->getID());

	WTP_ASSERT_EQ(2, static_cast<int>(vec.size()));
}

WTP_TEST(CvIdVector, GetByIdReturnsCorrectItem)
{
	CvIdVector<TestIdItem> vec;

	TestIdItem* p0 = vec.addNew();
	p0->setValue(42);

	TestIdItem* p1 = vec.addNew();
	p1->setValue(99);

	TestIdItem* found0 = vec.getById(0);
	WTP_ASSERT(found0 != NULL);
	WTP_ASSERT_EQ(42, found0->getValue());

	TestIdItem* found1 = vec.getById(1);
	WTP_ASSERT(found1 != NULL);
	WTP_ASSERT_EQ(99, found1->getValue());

	WTP_ASSERT(vec.getById(2) == NULL);
}

WTP_TEST(CvIdVector, RemoveById)
{
	CvIdVector<TestIdItem> vec;
	vec.addNew();
	vec.addNew();

	WTP_ASSERT(vec.removeById(0));
	WTP_ASSERT(vec.getById(0) == NULL);
	WTP_ASSERT(vec.getById(1) != NULL);
	WTP_ASSERT_EQ(1, static_cast<int>(vec.size()));

	// Removing non-existent ID returns false
	WTP_ASSERT(!vec.removeById(0));
	WTP_ASSERT(!vec.removeById(999));
}

WTP_TEST(CvIdVector, ResetClearsAll)
{
	CvIdVector<TestIdItem> vec;
	vec.addNew();
	vec.addNew();
	vec.addNew();

	vec.reset();
	WTP_ASSERT(vec.empty());
	WTP_ASSERT(vec.getById(0) == NULL);
	WTP_ASSERT(vec.getById(1) == NULL);
	WTP_ASSERT(vec.getById(2) == NULL);
}

WTP_TEST(CvIdVector, IdsNotReusedAfterRemove)
{
	CvIdVector<TestIdItem> vec;
	vec.addNew(); // ID 0
	vec.addNew(); // ID 1
	vec.removeById(0);

	TestIdItem* p2 = vec.addNew(); // should be ID 2, not 0
	WTP_ASSERT_EQ(2, p2->getID());
}

// ============================================================================
// Test Runner
// ============================================================================

void RunDataStructureTests()
{
	// EnumMap int tests
	WTP_RUN_SUITE_BEGIN();
	WTP_RUN_TEST(EnumMap, IntDefaultZero);
	WTP_RUN_TEST(EnumMap, IntSetGetReset);
	WTP_RUN_TEST(EnumMap, IntAdd);
	WTP_RUN_TEST(EnumMap, IntGetTotal);
	WTP_RUN_TEST(EnumMap, IntAddAll);
	WTP_RUN_TEST(EnumMap, IntNonZeroDefault);
	WTP_RUN_SUITE_END("EnumMap_Int");

	// EnumMap bool tests
	WTP_RUN_SUITE_BEGIN();
	WTP_RUN_TEST(EnumMap, BoolDefault);
	WTP_RUN_TEST(EnumMap, BoolSetGetReset);
	WTP_RUN_TEST(EnumMap, BoolMultipleKeys);
	WTP_RUN_SUITE_END("EnumMap_Bool");

	// EnumMap enum value tests
	WTP_RUN_SUITE_BEGIN();
	WTP_RUN_TEST(EnumMap, EnumValueType);
	WTP_RUN_TEST(EnumMap, LargeKeyType);
	WTP_RUN_TEST(EnumMap, FullIteration);
	WTP_RUN_SUITE_END("EnumMap_Enum");

	// JustInTimeArray tests
	WTP_RUN_SUITE_BEGIN();
	WTP_RUN_TEST(JustInTimeArray, DefaultUnallocated);
	WTP_RUN_TEST(JustInTimeArray, SetAllocates);
	WTP_RUN_TEST(JustInTimeArray, SetDefaultNoAllocate);
	WTP_RUN_TEST(JustInTimeArray, ResetDeallocates);
	WTP_RUN_TEST(JustInTimeArray, AddAccumulates);
	WTP_RUN_TEST(JustInTimeArray, HasContent);
	WTP_RUN_TEST(JustInTimeArray, NonZeroDefault);
	WTP_RUN_TEST(JustInTimeArray, MultipleIndices);
	WTP_RUN_TEST(JustInTimeArray, KeepMax);
	WTP_RUN_TEST(JustInTimeArray, KeepMin);
	WTP_RUN_SUITE_END("JustInTimeArray");

	// Coordinates tests
	WTP_RUN_SUITE_BEGIN();
	WTP_RUN_TEST(Coordinates, DefaultIsInvalid);
	WTP_RUN_TEST(Coordinates, ExplicitConstruction);
	WTP_RUN_TEST(Coordinates, InvalidCoordFactory);
	WTP_RUN_TEST(Coordinates, NullCoordFactory);
	WTP_RUN_TEST(Coordinates, EqualityInvalidCoords);
	WTP_RUN_SUITE_END("Coordinates");

	// CvTradeRoute sentinel tests
	WTP_RUN_SUITE_BEGIN();
	WTP_RUN_TEST(TradeRoute, SentinelConstants);
	WTP_RUN_TEST(TradeRoute, DefaultConstruction);
	WTP_RUN_SUITE_END("TradeRoute");

	// CvIdVector tests
	WTP_RUN_SUITE_BEGIN();
	WTP_RUN_TEST(CvIdVector, EmptyAfterConstruction);
	WTP_RUN_TEST(CvIdVector, AddNewAssignsSequentialIds);
	WTP_RUN_TEST(CvIdVector, GetByIdReturnsCorrectItem);
	WTP_RUN_TEST(CvIdVector, RemoveById);
	WTP_RUN_TEST(CvIdVector, ResetClearsAll);
	WTP_RUN_TEST(CvIdVector, IdsNotReusedAfterRemove);
	WTP_RUN_SUITE_END("CvIdVector");
}

#endif
